/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "basetype.h"
#include "decapicommon.h"
#include "fifo.h"
#include "vp9decapi.h"
#include "version.h"

#include "dwl.h"
#include "regdrv.h"

#include "vp9hwd_container.h"
#include "vp9hwd_asic.h"
#include "vp9hwd_headers.h"
#include "vp9hwd_output.h"
#include "sw_util.h"

static u32 Vp9CheckSupport(struct Vp9DecContainer *dec_cont);
static void Vp9Freeze(struct Vp9DecContainer *dec_cont);
static i32 Vp9DecodeHeaders(struct Vp9DecContainer *dec_cont,
                            const struct Vp9DecInput *input);

Vp9DecBuild Vp9DecGetBuild(void) {
  Vp9DecBuild build_info;

  (void)DWLmemset(&build_info, 0, sizeof(build_info));

  build_info.sw_build = HANTRO_DEC_SW_BUILD;
  build_info.hw_build = DWLReadAsicID();

  DWLReadAsicConfig(&build_info.hw_config);

  return build_info;
}

enum DecRet Vp9DecInit(Vp9DecInst *dec_inst, const void *dwl, struct Vp9DecConfig *dec_cfg) {
  struct Vp9DecContainer *dec_cont;

  struct DWLInitParam dwl_init;
  DWLHwConfig hw_cfg;
  u32 id;
  u32 is_legacy = 0;

  (void)dec_cfg->dpb_flags;

/* check that right shift on negative numbers is performed signed */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif

  if (dec_inst == NULL || dwl == NULL) {
    return DEC_PARAM_ERROR;
  }

  *dec_inst = NULL; /* return NULL instance for any error */

  id = DWLReadAsicID();
  DWLReadAsicConfig(&hw_cfg);
  if ((id & 0x0000F000) >> 12 == 0 && /* MAJOR_VERSION */
      (id & 0x00000FF0) >> 4 == 0)    /* MINOR_VERSION */ {
    /* It's legacy release, which doesn't set id correctly. */
    is_legacy = 1;
    hw_cfg.hevc_support = 1;
    hw_cfg.vp9_support = 1;
    hw_cfg.hevc_main10_support = 0;
    hw_cfg.vp9_10bit_support = 0;
    hw_cfg.ds_support = 0;
    hw_cfg.rfc_support = 0;
    hw_cfg.ring_buffer_support = 0;
  } else if ((id & 0x0000F000) >> 12 == 0 && /* MAJOR_VERSION */
             (id & 0x00000FF0) >> 4 == 0x18) /* MINOR_VERSION */ {
    /* Legacy release without correct config register */
    hw_cfg.hevc_support = 1;
    hw_cfg.vp9_support = 1;
    hw_cfg.hevc_main10_support = 1;
    hw_cfg.vp9_10bit_support = 1;
    hw_cfg.ds_support = 1;
    hw_cfg.rfc_support = 1;
    hw_cfg.ring_buffer_support = 1;
  }
  /* check that hevc decoding supported in HW */
  if (!hw_cfg.vp9_support) {
    return DEC_FORMAT_NOT_SUPPORTED;
  }

  if (!hw_cfg.rfc_support && dec_cfg->use_video_compressor) {
    return DEC_PARAM_ERROR;
  }

#ifdef DOWN_SCALER
  if (!hw_cfg.ds_support &&
      (dec_cfg->dscale_cfg.down_scale_x != 1) && (dec_cfg->dscale_cfg.down_scale_y != 1)) {
    return DEC_PARAM_ERROR;
  }
#endif

  if (!hw_cfg.ring_buffer_support && dec_cfg->use_ringbuffer) {
    return DEC_PARAM_ERROR;
  }

  if ((!hw_cfg.fmt_p010_support && dec_cfg->pixel_format == DEC_OUT_PIXEL_P010) ||
      (!hw_cfg.fmt_customer1_support && dec_cfg->pixel_format == DEC_OUT_PIXEL_CUSTOMER1) ||
      (!hw_cfg.addr_64bit_support && sizeof(void *) == 8) ||
      (!hw_cfg.mrb_prefetch && !dec_cfg->use_fetch_one_pic))
    return DEC_PARAM_ERROR;

  /* init struct struct DWL for the specified client */
  dwl_init.client_type = DWL_CLIENT_TYPE_VP9_DEC;

  /* allocate instance */
  dec_cont =
      (struct Vp9DecContainer *)DWLmalloc(sizeof(struct Vp9DecContainer));

  (void)DWLmemset(dec_cont, 0, sizeof(struct Vp9DecContainer));
  dec_cont->dwl = dwl;

  dec_cont->down_scale_enabled = 0;
  dec_cont->legacy_regs = 0;  // is_legacy;

#ifdef DOWN_SCALER
  /* Down scaler ratio */
  if ((dec_cfg->dscale_cfg.down_scale_x == 1) || (dec_cfg->dscale_cfg.down_scale_y == 1)) {
    dec_cont->down_scale_x_shift = 0;
    dec_cont->down_scale_y_shift = 0;
  } else if ((dec_cfg->dscale_cfg.down_scale_x != 2 &&
      dec_cfg->dscale_cfg.down_scale_x != 4 &&
      dec_cfg->dscale_cfg.down_scale_x != 8 ) ||
      (dec_cfg->dscale_cfg.down_scale_y != 2 &&
      dec_cfg->dscale_cfg.down_scale_y != 4 &&
      dec_cfg->dscale_cfg.down_scale_y != 8 )) {
    return (DEC_PARAM_ERROR);
  } else {
    u32 scale_table[9] = {0, 0, 1, 0, 2, 0, 0, 0, 3};

    dec_cont->down_scale_enabled = 1;
    dec_cont->down_scale_x_shift = scale_table[dec_cfg->dscale_cfg.down_scale_x];
    dec_cont->down_scale_y_shift = scale_table[dec_cfg->dscale_cfg.down_scale_y];
  }
#endif
  dec_cont->vp9_10bit_support = hw_cfg.vp9_10bit_support;
  dec_cont->use_video_compressor = dec_cfg->use_video_compressor;
  dec_cont->use_ringbuffer = dec_cfg->use_ringbuffer;
  dec_cont->use_fetch_one_pic = dec_cfg->use_fetch_one_pic;
  dec_cont->use_8bits_output = (dec_cfg->pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 1 : 0;
  dec_cont->use_p010_output = (dec_cfg->pixel_format == DEC_OUT_PIXEL_P010) ? 1 : 0;
  dec_cont->pixel_format = dec_cfg->pixel_format;

  /* initial setup of instance */

  dec_cont->dec_stat = VP9DEC_INITIALIZED;
  dec_cont->checksum = dec_cont; /* save instance as a checksum */

  if (dec_cfg->num_frame_buffers > VP9DEC_MAX_PIC_BUFFERS)
    dec_cfg->num_frame_buffers = VP9DEC_MAX_PIC_BUFFERS;

  dec_cont->num_buffers = dec_cfg->num_frame_buffers;

  Vp9AsicInit(dec_cont); /* Init ASIC */

#ifndef USE_EXTERNAL_BUFFER
  if (Vp9AsicAllocateMem(dec_cont) != 0) {
    DWLfree(dec_cont);
    return DEC_MEMFAIL;
  }
#endif

  dec_cont->pic_number = dec_cont->display_number = 1;
  dec_cont->intra_freeze = dec_cfg->use_video_freeze_concealment;
  dec_cont->picture_broken = 0;
  dec_cont->decoder.refbu_pred_hits = 0;

  if (FifoInit(VP9DEC_MAX_PIC_BUFFERS, &dec_cont->fifo_out) != FIFO_OK)
    return DEC_MEMFAIL;

  if (FifoInit(VP9DEC_MAX_PIC_BUFFERS, &dec_cont->fifo_display) != FIFO_OK)
    return DEC_MEMFAIL;

  if (pthread_mutex_init(&dec_cont->sync_out, NULL) ||
      pthread_cond_init(&dec_cont->sync_out_cv, NULL))
    return DEC_SYSTEM_ERROR;

  DWLmemcpy(&dec_cont->hw_cfg, &hw_cfg, sizeof(DWLHwConfig));

  dec_cont->output_format = dec_cfg->output_format;
  /* TODO this limit could later be given through the API. Dynamic reference
     frame allocation can allocate buffers dynamically up to this limit.
     Otherwise sw stops and waits for the free buffer to emerge to the fifo */
  dec_cont->dynamic_buffer_limit = VP9DEC_DYNAMIC_PIC_LIMIT;

#ifdef USE_EXTERNAL_BUFFER
  dec_cont->ext_buffer_config  = 0;
#ifdef DOWN_SCALER
  if (dec_cont->down_scale_enabled)
    dec_cont->ext_buffer_config |= 1 << DOWNSCALE_OUT_BUFFER;
  else
#endif
  if (dec_cfg->output_format == DEC_OUT_FRM_RASTER_SCAN)
    dec_cont->ext_buffer_config |= 1 << RASTERSCAN_OUT_BUFFER;
  else if (dec_cfg->output_format == DEC_OUT_FRM_TILED_4X4)
    dec_cont->ext_buffer_config  = 1 << REFERENCE_BUFFER;
#endif

  /* return new instance to application */
  *dec_inst = (Vp9DecInst)dec_cont;

  return DEC_OK;
}

void Vp9DecRelease(Vp9DecInst dec_inst) {

  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
  const void *dwl;

  /* Check for valid decoder instance */
  if (dec_cont == NULL || dec_cont->checksum != dec_cont) {
    return;
  }

  dwl = dec_cont->dwl;

  if (dec_cont->asic_running) {
    DWLDisableHw(dwl, dec_cont->core_id, 1 * 4, 0); /* stop HW */
    DWLReleaseHw(dwl, dec_cont->core_id);           /* release HW lock */
    dec_cont->asic_running = 0;
  }

  Vp9AsicReleaseMem(dec_cont);
  Vp9AsicReleaseFilterBlockMem(dec_cont);
  Vp9AsicReleasePictures(dec_cont);

  if (dec_cont->fifo_out) FifoRelease(dec_cont->fifo_out);
  if (dec_cont->fifo_display) FifoRelease(dec_cont->fifo_display);

  pthread_cond_destroy(&dec_cont->sync_out_cv);
  pthread_mutex_destroy(&dec_cont->sync_out);

  dec_cont->checksum = NULL;
  DWLfree(dec_cont);

  return;
}

enum DecRet Vp9DecGetInfo(Vp9DecInst dec_inst, struct Vp9DecInfo *dec_info) {
  const struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;

  if (dec_inst == NULL || dec_info == NULL) {
    return DEC_PARAM_ERROR;
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return DEC_NOT_INITIALIZED;
  }

  if (dec_cont->dec_stat == VP9DEC_INITIALIZED) {
    return DEC_HDRS_NOT_RDY;
  }

  dec_info->vp_version = dec_cont->decoder.vp_version;
  dec_info->vp_profile = dec_cont->decoder.vp_profile;
  dec_info->output_format = dec_cont->output_format;
  dec_info->bit_depth = dec_cont->decoder.bit_depth;

  /* Fragments have 8 pixels */
  dec_info->coded_width = dec_cont->decoder.width;
  dec_info->coded_height = dec_cont->decoder.height;
  dec_info->frame_height = NEXT_MULTIPLE(dec_cont->decoder.height, 8);
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN)
    dec_info->frame_width = NEXT_MULTIPLE(dec_cont->decoder.width, 16);
  else
    dec_info->frame_width = NEXT_MULTIPLE(dec_cont->decoder.width, 8);

  dec_info->scaled_width = dec_cont->decoder.scaled_width;
  dec_info->scaled_height = dec_cont->decoder.scaled_height;
  dec_info->dpb_mode = DEC_DPB_FRAME;

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN || dec_cont->down_scale_enabled) {
    if (dec_cont->use_p010_output && dec_info->bit_depth > 8) {
      dec_info->pixel_format = DEC_OUT_PIXEL_P010;
      dec_info->bit_depth = 16;
    } else if (dec_cont->use_8bits_output) {
      dec_info->pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
      dec_info->bit_depth = 8;
    }else {
      dec_info->pixel_format = DEC_OUT_PIXEL_DEFAULT;
    }
  }
  else {
    /* Reference buffer. */
    dec_info->pixel_format = DEC_OUT_PIXEL_DEFAULT;
  }

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    dec_info->pic_stride = NEXT_MULTIPLE(NEXT_MULTIPLE(dec_cont->decoder.width, 8) * dec_info->bit_depth, 128) / 8;
  } else {
    /* Reference buffer. */
    dec_info->pic_stride = NEXT_MULTIPLE(dec_cont->decoder.width, 8) * dec_info->bit_depth / 8;
  }

  dec_info->pic_buff_size = dec_cont->num_buffers;
  return DEC_OK;
}

enum DecRet Vp9DecDecode(Vp9DecInst dec_inst, const struct Vp9DecInput *input,
                         struct Vp9DecOutput *output) {
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
  i32 ret;
  /* Check that function input parameters are valid */
  if (input == NULL || output == NULL || dec_inst == NULL) {
    return DEC_PARAM_ERROR;
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return DEC_NOT_INITIALIZED;
  }

  if ((input->data_len > DEC_X170_MAX_STREAM) ||
      X170_CHECK_VIRTUAL_ADDRESS(input->stream) ||
      X170_CHECK_BUS_ADDRESS(input->stream_bus_address) ||
      X170_CHECK_VIRTUAL_ADDRESS(input->buffer) ||
      X170_CHECK_BUS_ADDRESS_AGLINED(input->buffer_bus_address)) {
    return DEC_PARAM_ERROR;
  }

 #ifdef USE_EXTERNAL_BUFFER
  /* If there are more buffers to be allocated or to be freed, waiting for buffers ready. */
  if (dec_cont->buf_to_free != NULL ||
      dec_cont->next_buf_size != 0)
      return DEC_WAITING_FOR_BUFFER;
#endif

  ret = VP9SyncAndOutput(dec_cont);
  if (ret) return ret;

 #ifdef USE_EXTERNAL_BUFFER
  ret = Vp9AsicAllocateMem(dec_cont);
  if (ret != DEC_OK) return DEC_WAITING_FOR_BUFFER;
 #endif

  if (dec_cont->dec_stat == VP9DEC_NEW_HEADERS) {
  /* TODO: Allocate picture buffers here. */
 #ifndef USE_EXTERNAL_BUFFER
    if (Vp9AsicAllocatePictures(dec_cont) != 0) {
      return DEC_MEMFAIL;
    }
 #else
    if (Vp9AsicAllocatePictures(dec_cont) != 0) {
      return DEC_WAITING_FOR_BUFFER;
    }
 #endif
    dec_cont->dec_stat = VP9DEC_DECODING;
  }
 #ifdef USE_EXTERNAL_BUFFER
   else if (dec_cont->dec_stat == VP9DEC_WAITING_FOR_BUFFER) {
   }
 #endif
  else if (input->data_len) {
    /* Decode SW part of the frame */
    ret = Vp9DecodeHeaders(dec_cont, input);
    if (ret) return ret;
  }
  /* missing picture, conceal */
  else {
    if (dec_cont->force_intra_freeze || dec_cont->prev_is_key) {
      dec_cont->decoder.probs_decoded = 0;
      Vp9Freeze(dec_cont);
      return DEC_PIC_DECODED;
    }
  }

  /* Get free picture buffer */
  ret = Vp9GetRefFrm(dec_cont);
  if (ret) {
 #ifdef USE_EXTERNAL_BUFFER
    dec_cont->dec_stat = VP9DEC_WAITING_FOR_BUFFER;
 #endif
    return ret;
  }

  /* Verify we have enough auxilary buffer for filter tile edge data. */
  ret = Vp9AsicAllocateFilterBlockMem(dec_cont);
  if (ret) {
#ifdef USE_EXTERNAL_BUFFER
    dec_cont->dec_stat = VP9DEC_WAITING_FOR_BUFFER;
#endif
   return ret;
  }
#ifdef USE_EXTERNAL_BUFFER
  dec_cont->dec_stat = VP9DEC_DECODING;
#endif
  /* prepare asic */
  Vp9AsicProbUpdate(dec_cont);

  Vp9AsicInitPicture(dec_cont);

  Vp9AsicStrmPosUpdate(dec_cont, input->stream_bus_address, input->data_len,
                      input->buffer_bus_address, input->buff_len);
  /* run the hardware */
  Vp9AsicRun(dec_cont);

  return DEC_PIC_DECODED;
}

u32 Vp9CheckSupport(struct Vp9DecContainer *dec_cont) {

  DWLHwConfig hw_config;

  DWLReadAsicConfig(&hw_config);

  if ((dec_cont->asic_buff->width > hw_config.max_dec_pic_width) ||
      (dec_cont->asic_buff->height > hw_config.max_dec_pic_height) ||
      (dec_cont->asic_buff->width < MIN_PIC_WIDTH) ||
      (dec_cont->asic_buff->height < MIN_PIC_HEIGHT)) {
    return HANTRO_NOK;
  }

  return HANTRO_OK;
}

void Vp9Freeze(struct Vp9DecContainer *dec_cont) {
  /* Rollback entropy probabilities if refresh is not set */
  if (dec_cont->decoder.probs_decoded &&
      dec_cont->decoder.refresh_entropy_probs == HANTRO_FALSE) {
    DWLmemcpy(&dec_cont->decoder.entropy, &dec_cont->decoder.entropy_last,
              sizeof(struct Vp9EntropyProbs));
  }
  /* lost accumulated coeff prob updates -> force video freeze until next
   * keyframe received */
  else if (dec_cont->prob_refresh_detected) {
    dec_cont->force_intra_freeze = 1;
  }

  dec_cont->picture_broken = 1;

  /* TODO Reset mv memory if needed? */
}

i32 Vp9DecodeHeaders(struct Vp9DecContainer *dec_cont,
                     const struct Vp9DecInput *input) {
  i32 ret;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  struct Vp9Decoder *dec = &dec_cont->decoder;

  dec_cont->prev_is_key = dec->key_frame;
  dec->prev_is_key_frame = dec->key_frame;
  dec->prev_show_frame = dec->show_frame;
  dec->probs_decoded = 0;
  dec->last_width = dec->width;
  dec->last_height = dec->height;

  /* decode frame tag */
  ret = Vp9DecodeFrameTag(input->stream, input->data_len, input->buffer, input->buff_len, dec_cont);
  if (ret != HANTRO_OK) {
    if (dec->bit_depth == 12)
      return DEC_STREAM_NOT_SUPPORTED;
    else if ((dec->bit_depth == 10) && !dec_cont->vp9_10bit_support)
      return DEC_STREAM_NOT_SUPPORTED;
    else if ((dec_cont->pic_number == 1 && !dec->prev_is_key_frame) ||
        dec_cont->dec_stat != VP9DEC_DECODING)
      return DEC_STRM_ERROR;
    else {
      Vp9Freeze(dec_cont);
      return DEC_PIC_DECODED;
    }
  } else if (ret == HANTRO_OK && dec->show_existing_frame) {
    asic_buff->out_buffer_i =
        Vp9BufferQueueGetRef(dec_cont->bq, dec->show_existing_frame_index);
    Vp9BufferQueueAddRef(dec_cont->bq, asic_buff->out_buffer_i);
    Vp9SetupPicToOutput(dec_cont);
    asic_buff->out_buffer_i = -1;
    Vp9PicToOutput(dec_cont);
    return DEC_PIC_DECODED;
  }
  /* Decode frame header (now starts bool coder as well) */
  ret = Vp9DecodeFrameHeader(input->stream + dec->frame_tag_size,
                             input->data_len - dec->frame_tag_size,
                             &dec_cont->bc, input->buffer,
                             input->buff_len, &dec_cont->decoder);
  if (ret != HANTRO_OK) {
    if ((dec_cont->pic_number == 1 && !dec->prev_is_key_frame) ||
        dec_cont->dec_stat != VP9DEC_DECODING)
      return DEC_STRM_ERROR;
    else {
      Vp9Freeze(dec_cont);
      return DEC_PIC_DECODED;
    }
  }
  /* flag the stream as non "error-resilient" */
  else if (dec->refresh_entropy_probs)
    dec_cont->prob_refresh_detected = 1;

  ret = Vp9SetPartitionOffsets(input->stream, input->data_len,
                               &dec_cont->decoder);
  /* ignore errors in partition offsets if HW error concealment used
   * (assuming parts of stream missing -> partition start offsets may
   * be larger than amount of stream in the buffer) */
  if (ret != HANTRO_OK) {
    if ((dec_cont->pic_number == 1 && !dec->prev_is_key_frame) ||
      dec_cont->dec_stat != VP9DEC_DECODING)
      return DEC_STRM_ERROR;
    else {
      Vp9Freeze(dec_cont);
      return DEC_PIC_DECODED;
    }
  }

  asic_buff->width = NEXT_MULTIPLE(dec->width, 8);
  asic_buff->height = NEXT_MULTIPLE(dec->height, 8);
  /* If the frame dimensions are not supported by HW,
     release allocated picture buffers and return error */
  if (((dec_cont->width != dec->width) || (dec_cont->height != dec->height)) &&
      (Vp9CheckSupport(dec_cont) != HANTRO_OK)) {
    //Vp9AsicReleaseFilterBlockMem(dec_cont);
    //Vp9AsicReleasePictures(dec_cont);
    dec_cont->dec_stat = VP9DEC_INITIALIZED;
    return DEC_STREAM_NOT_SUPPORTED;
  }

  dec_cont->width = dec->width;
  dec_cont->height = dec->height;

  if (dec_cont->dec_stat == VP9DEC_INITIALIZED) {
    dec_cont->dec_stat = VP9DEC_NEW_HEADERS;
    return DEC_HDRS_RDY;
  }

  /* If we are here and dimensions are still 0, it means that we have
   * yet to decode a valid keyframe, in which case we must give up. */
  if (dec_cont->width == 0 || dec_cont->height == 0) {
    return DEC_STRM_PROCESSED;
  }

  /* If output picture is broken and we are not decoding a base frame,
   * don't even start HW, just output same picture again. */
  if (!dec->key_frame && dec_cont->picture_broken &&
      (dec_cont->intra_freeze || dec_cont->force_intra_freeze)) {
    Vp9Freeze(dec_cont);
    return DEC_PIC_DECODED;
  }

  return DEC_OK;
}

#ifdef USE_EXTERNAL_BUFFER

//#define EXTERNAL_BUFFER_INFO
enum DecRet Vp9DecAddBuffer(Vp9DecInst dec_inst,
                     struct DWLLinearMem *info) {
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  enum DecRet dec_ret = DEC_OK;
  static struct DWLLinearMem old_segment_map;

  if (dec_inst == NULL || info == NULL ||
      X170_CHECK_VIRTUAL_ADDRESS(info->virtual_address) ||
      X170_CHECK_BUS_ADDRESS_AGLINED(info->bus_address) ||
      info->logical_size < dec_cont->next_buf_size) {
    return DEC_PARAM_ERROR;
  }

  if (dec_cont->buf_num == 0)
    return DEC_EXT_BUFFER_REJECTED;

#ifdef EXTERNAL_BUFFER_INFO
  printf(__FUNCTION__);
  printf("() adds external buffer with size %d\n", info->size);
#endif

  switch (dec_cont->buf_type) {
  case MISC_LINEAR_BUFFER:
    asic_buff->misc_linear = *info;
    dec_cont->buf_to_free = NULL;
    dec_cont->next_buf_size = 0;
    dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 0;
#endif
    break;
  case REFERENCE_BUFFER: {
    dec_cont->add_buffer = 0;
    if (dec_cont->dec_stat == VP9DEC_WAITING_FOR_BUFFER &&
        dec_cont->buffer_index == dec_cont->num_buffers) {
      /* Need to allocate a new buffer during decoding... */
      dec_cont->num_buffers++;
      dec_cont->add_buffer = 1;
    }

    ASSERT(dec_cont->buffer_index < dec_cont->num_buffers);
    asic_buff->pictures[dec_cont->buffer_index] = *info;
    // dec_cont->buf_num--;
    if (dec_cont->dec_stat == VP9DEC_NEW_HEADERS)
      dec_cont->buffer_index++;

    if (dec_cont->dec_stat != VP9DEC_NEW_HEADERS ||
        dec_cont->buffer_index >= dec_cont->num_buffers) {
      /* Need to add all the picture buffers in state VP9DEC_NEW_HEADERS. */
      /* Reference buffer always allocated along with raster/dscale buffer. */
      /* Step for raster buffer. */
      if (dec_cont->buffer_index >= dec_cont->num_buffers)
        dec_cont->buffer_index = 0;
      if (dec_cont->add_buffer) {
        /* Need to allocate a new buffer during decoding... */
        Vp9BufferQueueAddBuffer(dec_cont->bq);
        dec_cont->add_buffer = 0;
      }
      if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN
          && IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
        dec_cont->buf_type = RASTERSCAN_OUT_BUFFER;
        dec_cont->next_buf_size = asic_buff->raster_size;
        dec_cont->buf_to_free = asic_buff->realloc_out_buffer ? &asic_buff->raster[dec_cont->buffer_index] : NULL;
        dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
        dec_ret = DEC_WAITING_FOR_BUFFER;

      }
#ifdef DOWN_SCALER
      else if (dec_cont->down_scale_enabled
          && IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
        dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
        dec_cont->next_buf_size = asic_buff->dscale_size;
        dec_cont->buf_to_free = asic_buff->realloc_out_buffer ? &asic_buff->dscale[dec_cont->buffer_index] : NULL;
        dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
        dec_ret = DEC_WAITING_FOR_BUFFER;
      }
#endif
      else {
        dec_cont->next_buf_size = 0;
        // dec_cont->buf_num = 0;
        dec_cont->buf_to_free = NULL;

        if (dec_cont->add_buffer)
          Vp9BufferQueueAddBuffer(dec_cont->bq);
      }
    } else
      dec_ret = DEC_WAITING_FOR_BUFFER;
    break;
  }
  case RASTERSCAN_OUT_BUFFER: {
    ASSERT(dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN);
    ASSERT(dec_cont->buffer_index < dec_cont->num_buffers);

    asic_buff->raster[dec_cont->buffer_index] = *info;
    dec_cont->buffer_index++;
    //dec_cont->buf_num--;
    if (dec_cont->dec_stat != VP9DEC_NEW_HEADERS ||
        dec_cont->buffer_index >= dec_cont->num_buffers) {
      /* Need to add all the picture buffers in state VP9DEC_NEW_HEADERS. */
      /* Reference buffer always allocated along with raster/dscale buffer. */
      /* Step for raster buffer. */
      dec_cont->buffer_index = 0;
#ifdef DOWN_SCALER
      if (dec_cont->down_scale_enabled
          && IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
        dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
        dec_cont->next_buf_size = asic_buff->dscale_size;
        dec_cont->buf_to_free = asic_buff->realloc_out_buffer ? &asic_buff->dscale[dec_cont->buffer_index] : NULL;
        dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
        dec_ret = DEC_WAITING_FOR_BUFFER;
      }
      else
#endif
      {
        dec_cont->next_buf_size = 0;
        dec_cont->buf_to_free = NULL;
        dec_cont->buffer_index = 0;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
#if 0
        if (dec_cont->add_buffer) {
          /* Need to allocate a new buffer during decoding... */
          Vp9BufferQueueAddBuffer(dec_cont->bq);
        }
#endif
      }
    } else
      dec_ret = DEC_WAITING_FOR_BUFFER;
    break;
  }
#ifdef DOWN_SCALER
  case DOWNSCALE_OUT_BUFFER:
    ASSERT(dec_cont->buffer_index < dec_cont->num_buffers);
    asic_buff->dscale[dec_cont->buffer_index] = *info;
    dec_cont->buffer_index++;
    //dec_cont->buf_num--;
    if (dec_cont->dec_stat != VP9DEC_NEW_HEADERS ||
        dec_cont->buffer_index >= dec_cont->num_buffers) {
      dec_cont->next_buf_size = 0;
      //dec_cont->buf_num = 0;
      dec_cont->buf_to_free = NULL;
      dec_cont->buffer_index = 0;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    } else
      dec_ret = DEC_WAITING_FOR_BUFFER;
    break;
#endif
  case TILE_EDGE_BUFFER:
    asic_buff->tile_edge = *info;
    dec_cont->buf_to_free = NULL;
    dec_cont->next_buf_size = 0;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 0;
#endif
    asic_buff->realloc_tile_edge_mem = 0;
    break;
  case SEGMENT_MAP_BUFFER:
    // Copy old segment to new segment map.
    old_segment_map = asic_buff->segment_map;
    asic_buff->segment_map = *info;

    if (old_segment_map.virtual_address != NULL) {
      /* Copy existing segment maps into new buffers */
      DWLmemcpy(info->virtual_address,
            old_segment_map.virtual_address,
            asic_buff->segment_map_size);
      DWLmemcpy((u8 *)info->virtual_address + asic_buff->segment_map_size_new,
            (u8 *)old_segment_map.virtual_address + asic_buff->segment_map_size,
            asic_buff->segment_map_size);

      /* Free existing segment maps. */
      dec_cont->buf_to_free = &old_segment_map;
    } else {
      dec_cont->buf_to_free = NULL;
      DWLmemset(info->virtual_address, 0, info->logical_size);
    }

    asic_buff->segment_map_size = asic_buff->segment_map_size_new;
    dec_cont->next_buf_size = 0;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 0;
#endif
    break;
  default:
    break;
  }

  return dec_ret;
}


enum DecRet Vp9DecGetBufferInfo(Vp9DecInst dec_inst, struct Vp9DecBufferInfo *mem_info) {
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
//  enum DecRet dec_ret = DEC_OK;
  struct DWLLinearMem empty = {0};

  if (dec_inst == NULL || mem_info == NULL) {
    return DEC_PARAM_ERROR;
  }

  if (dec_cont->buf_to_free == NULL && dec_cont->next_buf_size == 0)
    return DEC_OK;

  if (dec_cont->buf_to_free) {
    mem_info->buf_to_free = *dec_cont->buf_to_free;

    // TODO(min): here we assume that the buffer should be freed externally.
    dec_cont->buf_to_free->virtual_address = NULL;
    dec_cont->buf_to_free = NULL;
  } else
    mem_info->buf_to_free = empty;

  mem_info->next_buf_size = dec_cont->next_buf_size;
  mem_info->buf_num = dec_cont->buf_num;
#ifdef ASIC_TRACE_SUPPORT
  mem_info->is_frame_buffer = dec_cont->is_frame_buffer;
#endif

#ifdef EXTERNAL_BUFFER_INFO
  printf(__FUNCTION__);
  printf("() requests %d external buffers with size of %d\n", mem_info->buf_num, mem_info->next_buf_size);
#endif

  return DEC_WAITING_FOR_BUFFER;
}
#endif

enum DecRet Vp9DecUseExtraFrmBuffers(Vp9DecInst dec_inst, u32 n) {

  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;

  dec_cont->n_extra_frm_buffers = n;

  return DEC_OK;
}
