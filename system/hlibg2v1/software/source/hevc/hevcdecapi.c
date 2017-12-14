/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * HEVC decoder API. */

#include "basetype.h"
#include "version.h"
#include "hevc_container.h"
#include "hevcdecapi.h"
#include "hevc_decoder.h"
#include "hevc_util.h"
#include "hevc_dpb.h"
#include "hevc_asic.h"
#include "regdrv.h"
#include "hevc_byte_stream.h"
#include "deccfg.h"
#include "commonconfig.h"
#include "dwl.h"
#include "version.h"
#include "decapicommon.h"

static void HevcUpdateAfterPictureDecode(struct HevcDecContainer *dec_cont);
static u32 HevcSpsSupported(const struct HevcDecContainer *dec_cont);
static u32 HevcPpsSupported(const struct HevcDecContainer *dec_cont);

static u32 HevcAllocateResources(struct HevcDecContainer *dec_cont);
static void HevcInitPicFreezeOutput(struct HevcDecContainer *dec_cont,
                                    u32 from_old_dpb);
static void HevcGetSarInfo(const struct Storage *storage, u32 *sar_width,
                           u32 *sar_height);
extern void HevcPreparePpRun(struct HevcDecContainer *dec_cont);

static enum DecRet HevcDecNextPictureInternal(
    struct HevcDecContainer *dec_cont);
static void HevcCycleCount(struct HevcDecContainer *dec_cont);

#define DEC_DPB_NOT_INITIALIZED -1
#define DEC_MODE_HEVC 12

/* Initializes decoder software. Function reserves memory for the
 * decoder instance and calls HevcInit to initialize the
 * instance data. */
enum DecRet HevcDecInit(HevcDecInst *dec_inst, const void *dwl, struct HevcDecConfig *dec_cfg) {

  /*@null@ */ struct HevcDecContainer *dec_cont;

  struct DWLInitParam dwl_init;
  DWLHwConfig hw_cfg;
  u32 id;
  u32 is_legacy = 0;

/* check that right shift on negative numbers is performed signed */
/*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
  /*lint -restore */

  if (dec_inst == NULL || dwl == NULL || dec_cfg == NULL) {
    return (DEC_PARAM_ERROR);
  }

  *dec_inst = NULL; /* return NULL instance for any error */

  (void)DWLmemset(&hw_cfg, 0, sizeof(DWLHwConfig));
  id = DWLReadAsicID();
  DWLReadAsicConfig(&hw_cfg);
  if ((id & 0x0000F000) >> 12 == 0 && /* MAJOR_VERSION */
      (id & 0x00000FF0) >> 4 == 0)    /* MINOR_VERSION */ {
    is_legacy = 1;
    hw_cfg.hevc_support = 1;
    hw_cfg.vp9_support = 1;
    hw_cfg.hevc_main10_support = 0;
    hw_cfg.vp9_10bit_support = 0;
    hw_cfg.ds_support = 0;
    hw_cfg.rfc_support = 0;
    hw_cfg.ring_buffer_support = 0;
    hw_cfg.fmt_p010_support = 0;
    hw_cfg.fmt_customer1_support = 0;
    hw_cfg.addr_64bit_support = 0;
    hw_cfg.mrb_prefetch = 0;
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
    hw_cfg.fmt_p010_support = 0;
    hw_cfg.fmt_customer1_support = 0;
    hw_cfg.addr_64bit_support = 0;
    hw_cfg.mrb_prefetch = 0;
  }
  /* check that hevc decoding supported in HW */
  if (!hw_cfg.hevc_support) {
    return DEC_FORMAT_NOT_SUPPORTED;
  }

  if (!hw_cfg.rfc_support && dec_cfg->use_video_compressor) {
    return DEC_PARAM_ERROR;
  }

#ifdef DOWN_SCALER
  if (!hw_cfg.ds_support && (dec_cfg->dscale_cfg.down_scale_x != 1) && (dec_cfg->dscale_cfg.down_scale_y != 1)) {
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

  /* TODO: ? */
  dwl_init.client_type = DWL_CLIENT_TYPE_HEVC_DEC;

  dec_cont =
      (struct HevcDecContainer *)DWLmalloc(sizeof(struct HevcDecContainer));

  if (dec_cont == NULL) {
    return (DEC_MEMFAIL);
  }

  (void)DWLmemset(dec_cont, 0, sizeof(struct HevcDecContainer));
  dec_cont->dwl = dwl;

  HevcInit(&dec_cont->storage, dec_cfg->no_output_reordering);

  dec_cont->dec_state = HEVCDEC_INITIALIZED;

  SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_MODE, DEC_MODE_HEVC);

  SetCommonConfigRegs(dec_cont->hevc_regs);

#ifndef _DISABLE_PIC_FREEZE
  dec_cont->storage.intra_freeze = dec_cfg->use_video_freeze_concealment;
#endif
  dec_cont->storage.picture_broken = HANTRO_FALSE;

  /* max decodable picture width and height*/
  dec_cont->max_dec_pic_width = hw_cfg.max_dec_pic_width;
  dec_cont->max_dec_pic_height = hw_cfg.max_dec_pic_height;

  dec_cont->checksum = dec_cont; /* save instance as a checksum */

  *dec_inst = (HevcDecInst)dec_cont;

  /* Init frame buffer list */
  InitList(&dec_cont->fb_list);

  dec_cont->storage.dpb[0].fb_list = &dec_cont->fb_list;
  dec_cont->storage.dpb[1].fb_list = &dec_cont->fb_list;
  dec_cont->output_format = dec_cfg->output_format;

  if (dec_cfg->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    dec_cont->storage.raster_enabled = 1;
  }

  dec_cont->down_scale_enabled = 0;
  dec_cont->storage.down_scale_enabled = 0;
  dec_cont->use_8bits_output = (dec_cfg->pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 1 : 0;
  dec_cont->use_p010_output = (dec_cfg->pixel_format == DEC_OUT_PIXEL_P010) ? 1 : 0;
  dec_cont->pixel_format = dec_cfg->pixel_format;
  dec_cont->storage.use_8bits_output = dec_cont->use_8bits_output;
  dec_cont->storage.use_p010_output = dec_cont->use_p010_output;

#ifdef DOWN_SCALER
  /* Down scaler ratio */
  if ((dec_cfg->dscale_cfg.down_scale_x == 1) || (dec_cfg->dscale_cfg.down_scale_y == 1)) {
    dec_cont->down_scale_enabled = 0;
    dec_cont->down_scale_x = 1;
    dec_cont->down_scale_y = 1;
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
    dec_cont->down_scale_x = dec_cfg->dscale_cfg.down_scale_x;
    dec_cont->down_scale_y = dec_cfg->dscale_cfg.down_scale_y;

    dec_cont->storage.down_scale_enabled = 1;
    //dec_cont->storage.down_scale_x_shift = (dec_cont->down_scale_x >> 2) + 1;
    //dec_cont->storage.down_scale_y_shift = (dec_cont->down_scale_y >> 2) + 1;
    dec_cont->storage.down_scale_x_shift = scale_table[dec_cont->down_scale_x];
    dec_cont->storage.down_scale_y_shift = scale_table[dec_cont->down_scale_y];
  }
#endif

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
  dec_cont->hevc_main10_support = hw_cfg.hevc_main10_support;
  dec_cont->use_video_compressor = dec_cfg->use_video_compressor;
  dec_cont->use_ringbuffer = dec_cfg->use_ringbuffer;
  dec_cont->use_fetch_one_pic = dec_cfg->use_fetch_one_pic;
  dec_cont->storage.use_video_compressor = dec_cfg->use_video_compressor;
  dec_cont->legacy_regs = 0; //is_legacy;
  return (DEC_OK);
}

/* This function provides read access to decoder information. This
 * function should not be called before HevcDecDecode function has
 * indicated that headers are ready. */
enum DecRet HevcDecGetInfo(HevcDecInst dec_inst, struct HevcDecInfo *dec_info) {
  u32 cropping_flag;
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  struct Storage *storage;

  if (dec_inst == NULL || dec_info == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  storage = &dec_cont->storage;

  if (storage->active_sps == NULL || storage->active_pps == NULL) {
    return (DEC_HDRS_NOT_RDY);
  }

  dec_info->pic_width = HevcPicWidth(storage);
  dec_info->pic_height = HevcPicHeight(storage);
  dec_info->video_range = HevcVideoRange(storage);
  dec_info->matrix_coefficients = HevcMatrixCoefficients(storage);
  dec_info->mono_chrome = HevcIsMonoChrome(storage);
  //dec_info->pic_buff_size = storage->dpb->dpb_size + 1;
  dec_info->pic_buff_size = storage->active_sps->max_dpb_size + 1 + 2;
  dec_info->multi_buff_pp_size =
      storage->dpb->no_reordering ? 2 : dec_info->pic_buff_size;
  dec_info->dpb_mode = dec_cont->dpb_mode;

  HevcGetSarInfo(storage, &dec_info->sar_width, &dec_info->sar_height);

  HevcCroppingParams(storage, &cropping_flag,
                     &dec_info->crop_params.crop_left_offset,
                     &dec_info->crop_params.crop_out_width,
                     &dec_info->crop_params.crop_top_offset,
                     &dec_info->crop_params.crop_out_height);

  if (cropping_flag == 0) {
    dec_info->crop_params.crop_left_offset = 0;
    dec_info->crop_params.crop_top_offset = 0;
    dec_info->crop_params.crop_out_width = dec_info->pic_width;
    dec_info->crop_params.crop_out_height = dec_info->pic_height;
  }

  dec_info->output_format = dec_cont->output_format;
  dec_info->bit_depth = ((HevcLumaBitDepth(storage) != 8) || (HevcChromaBitDepth(storage) != 8)) ? 10 : 8;

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

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN)
    dec_info->pic_stride = NEXT_MULTIPLE(dec_info->pic_width * dec_info->bit_depth, 128) / 8;
  else
    /* Reference buffer. */
    dec_info->pic_stride = dec_info->pic_width * dec_info->bit_depth / 8;

  return (DEC_OK);
}

/* Releases the decoder instance. Function calls HevcShutDown to
 * release instance data and frees the memory allocated for the
 * instance. */
void HevcDecRelease(HevcDecInst dec_inst) {

  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  const void *dwl;

  if (dec_cont == NULL) {
    return;
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return;
  }

  dwl = dec_cont->dwl;

  if (dec_cont->asic_running) {
    /* stop HW */
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ, 0);
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_E, 0);
    DWLDisableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                 dec_cont->hevc_regs[1]);
    DWLReleaseHw(dwl, dec_cont->core_id); /* release HW lock */
    dec_cont->asic_running = 0;

    /* Decrement usage for DPB buffers */
    DecrementDPBRefCount(dec_cont->storage.dpb);
  }

  HevcShutdown(&dec_cont->storage);

#ifndef USE_EXTERNAL_BUFFER
  HevcFreeDpb(dwl, dec_cont->storage.dpb);
#else
  HevcFreeDpb(dec_cont, dec_cont->storage.dpb);
#endif
  if (dec_cont->storage.raster_buffer_mgr)
    RbmRelease(dec_cont->storage.raster_buffer_mgr);

#ifndef USE_EXTERNAL_BUFFER
  ReleaseAsicBuffers(dwl, dec_cont->asic_buff);
#else
  ReleaseAsicBuffers(dec_cont, dec_cont->asic_buff);
#endif
  ReleaseAsicTileEdgeMems(dec_cont);

  ReleaseList(&dec_cont->fb_list);

  dec_cont->checksum = NULL;
  DWLfree(dec_cont);

  return;
}

/* Decode stream data. Calls HevcDecode to do the actual decoding. */
enum DecRet HevcDecDecode(HevcDecInst dec_inst,
                          const struct HevcDecInput *input,
                          struct HevcDecOutput *output) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  u32 strm_len;
  const u8 *tmp_stream;
  enum DecRet return_value = DEC_STRM_PROCESSED;

  /* Check that function input parameters are valid */
  if (input == NULL || output == NULL || dec_inst == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  if (input->data_len == 0 || input->data_len > DEC_X170_MAX_STREAM ||
      X170_CHECK_VIRTUAL_ADDRESS(input->stream) ||
      X170_CHECK_BUS_ADDRESS(input->stream_bus_address) ||
      X170_CHECK_VIRTUAL_ADDRESS(input->buffer) ||
      X170_CHECK_BUS_ADDRESS_AGLINED(input->buffer_bus_address)) {
    return DEC_PARAM_ERROR;
  }

  dec_cont->stream_pos_updated = 0;
  output->strm_curr_pos = NULL;
  dec_cont->hw_stream_start_bus = input->stream_bus_address;
  dec_cont->hw_buffer_start_bus = input->buffer_bus_address;
  dec_cont->hw_stream_start = input->stream;
  dec_cont->hw_buffer = input->buffer;
  strm_len = dec_cont->hw_length = input->data_len;
  dec_cont->hw_buffer_length = input->buff_len;
  tmp_stream = input->stream;

#ifdef USE_EXTERNAL_BUFFER
  /* If there are more buffers to be allocated or to be freed, waiting for buffers ready. */
  if (dec_cont->buf_to_free != NULL ||
      dec_cont->next_buf_size != 0 ||
      dec_cont->rbm_release) {
    return_value = DEC_WAITING_FOR_BUFFER;
    goto end;
  }
#endif

  do {
    u32 dec_result;
    u32 num_read_bytes = 0;
    struct Storage *storage = &dec_cont->storage;
    dec_cont->u32Flush = 0;
    if (dec_cont->dec_state == HEVCDEC_NEW_HEADERS) {
      dec_result = HEVC_HDRS_RDY;
      dec_cont->dec_state = HEVCDEC_INITIALIZED;
    } else if (dec_cont->dec_state == HEVCDEC_BUFFER_EMPTY) {
      DEBUG_PRINT(("HevcDecDecode: Skip HevcDecode\n"));
      DEBUG_PRINT(("HevcDecDecode: Jump to HEVC_PIC_RDY\n"));

      dec_result = HEVC_PIC_RDY;
     }
#ifdef USE_EXTERNAL_BUFFER
    else if (dec_cont->dec_state == HEVCDEC_WAITING_FOR_BUFFER) {
      DEBUG_PRINT(("HevcDecDecode: Skip HevcDecode\n"));
      DEBUG_PRINT(("HevcDecDecode: Jump to HEVC_PIC_RDY\n"));

      dec_result = HEVC_BUFFER_NOT_READY;

    }
#endif
    else {
      dec_result = HevcDecode(dec_cont, tmp_stream, strm_len, input->pic_id,
                              &num_read_bytes);

      ASSERT(num_read_bytes <= strm_len);
    }

    tmp_stream += num_read_bytes;
    if(tmp_stream >= dec_cont->hw_buffer + dec_cont->hw_buffer_length && dec_cont->use_ringbuffer)
      tmp_stream -= dec_cont->hw_buffer_length;
    strm_len -= num_read_bytes;

    switch (dec_result) {
#ifndef USE_EXTERNAL_BUFFER
      case HEVC_HDRS_RDY: {
        if (storage->dpb->flushed && storage->dpb->num_out) {
          /* output first all DPB stored pictures */
            /*
              patch by infotm when sps change need flush all dpb to output
              return_value  DEC_PENDING_FLUSH; just notify app that this frame data is not decoded
          */
          /* flush any remaining pictures form DPB */
          HevcFlushBuffer(&dec_cont->storage);

          FinalizeOutputAll(&dec_cont->fb_list);

          while (HevcDecNextPictureInternal(dec_cont) == DEC_PIC_RDY)
            ;
          /* wake-up output thread */
          PushOutputPic(&dec_cont->fb_list, NULL, -1);

          /* block until all output is handled */
          WaitListNotInUse(&dec_cont->fb_list);
          while(IsOutputEmpty(&dec_cont->fb_list) == 0);
          /*
              patch by infotm end
          */
          dec_cont->dec_state = HEVCDEC_NEW_HEADERS;
          return_value = DEC_PENDING_FLUSH;
          strm_len = 0;
          storage->dpb->flushed = 0;
          dec_cont->u32Flush = 0;
          break;
        }
        else if (dec_cont->u32Flush == 1)
        {
            dec_cont->dec_state = HEVCDEC_NEW_HEADERS;
            strm_len = 0;
            return_value = DEC_PENDING_FLUSH;
            storage->dpb->flushed = 0;
            dec_cont->u32Flush = 0;
            break;
        }

        /* Make sure that all frame buffers are not in use before
         * reseting DPB (i.e. all HW cores are idle and all output
         * processed) */
        WaitListNotInUse(&dec_cont->fb_list);

        if (!HevcSpsSupported(dec_cont)) {
          storage->active_sps_id = MAX_NUM_SEQ_PARAM_SETS;
          storage->active_pps_id = MAX_NUM_PIC_PARAM_SETS;

          return_value = DEC_STREAM_NOT_SUPPORTED;
        } else if ((HevcAllocateSwResources(dec_cont->dwl, storage) != 0) ||
                   (HevcAllocateResources(dec_cont) != 0)) {
          /* signal that decoder failed to init parameter sets */
          /* TODO: miten viewit */
          storage->active_sps_id = MAX_NUM_SEQ_PARAM_SETS;
          storage->active_pps_id = MAX_NUM_PIC_PARAM_SETS;

          return_value = DEC_MEMFAIL;
        } else {
#if 0
                    u32 max_id = dec_cont->storage.dpb->no_reordering ? 1 :
                        dec_cont->storage.dpb->dpb_size;

                    /* Reset multibuffer status */
                    HevcPpMultiInit(dec_cont, max_id);
#endif

          return_value = DEC_HDRS_RDY;
        }

        /* Reset strm_len only for base view -> no HDRS_RDY to
         * application when param sets activated for stereo view */
        strm_len = 0;

        dec_cont->dpb_mode = DEC_DPB_DEFAULT;

/* Initialize tiled mode */
#if 0
                if( dec_cont->tiled_mode_support &&
                    DecCheckTiledMode( dec_cont->tiled_mode_support,
                        dec_cont->dpb_mode, 0) != HANTRO_OK )
                {
                    return_value = DEC_PARAM_ERROR;
                }
#endif

        break;
      }
#else
      /* Split the old case HEVC_HDRS_RDY into case HEVC_HDRS_RDY & HEVC_BUFFER_NOT_READY. */
      /* In case HEVC_BUFFER_NOT_READY, we will allocate resources. */
      case HEVC_HDRS_RDY: {
        if (storage->dpb->flushed && storage->dpb->num_out) {
          /* output first all DPB stored pictures */
          storage->dpb->flushed = 0;
          dec_cont->dec_state = HEVCDEC_NEW_HEADERS;
          return_value = DEC_STRM_PROCESSED;
          strm_len = 0;
          break;
        }

        /* Make sure that all frame buffers are not in use before
         * reseting DPB (i.e. all HW cores are idle and all output
         * processed) */
        WaitListNotInUse(&dec_cont->fb_list);

        if (!HevcSpsSupported(dec_cont)) {
          storage->active_sps_id = MAX_NUM_SEQ_PARAM_SETS;
          storage->active_pps_id = MAX_NUM_PIC_PARAM_SETS;

          return_value = DEC_STREAM_NOT_SUPPORTED;

          strm_len = 0;
          dec_cont->dpb_mode = DEC_DPB_DEFAULT;
        } else {
          dec_result = HEVC_BUFFER_NOT_READY;
          dec_cont->dec_state = HEVCDEC_WAITING_FOR_BUFFER;
          strm_len = 0;
          return_value = DEC_HDRS_RDY;
        }

        strm_len = 0;
        dec_cont->dpb_mode = DEC_DPB_DEFAULT;
/* Initialize tiled mode */
#if 0
                if( dec_cont->tiled_mode_support &&
                    DecCheckTiledMode( dec_cont->tiled_mode_support,
                        dec_cont->dpb_mode, 0) != HANTRO_OK )
                {
                    return_value = DEC_PARAM_ERROR;
                }
#endif
        break;
      }
      case HEVC_BUFFER_NOT_READY: {
        i32 ret;

        ret = HevcAllocateSwResources(dec_cont->dwl, storage, dec_cont);
        if (ret != HANTRO_OK) goto RESOURCE_NOT_READY;

        ret = HevcAllocateResources(dec_cont);
        if (ret != HANTRO_OK) goto RESOURCE_NOT_READY;

#if 0
        u32 max_id = dec_cont->storage.dpb->no_reordering ? 1 :
            dec_cont->storage.dpb->dpb_size;

        /* Reset multibuffer status */
        HevcPpMultiInit(dec_cont, max_id);
#endif

RESOURCE_NOT_READY:
        if (ret){
          if (ret == DEC_WAITING_FOR_BUFFER)
            return_value = ret;
          else {
            /* TODO: miten viewit */
            storage->active_sps_id = MAX_NUM_SEQ_PARAM_SETS;
            storage->active_pps_id = MAX_NUM_PIC_PARAM_SETS;

            return_value = DEC_MEMFAIL;          /* signal that decoder failed to init parameter sets */
          }

          strm_len = 0;

          //dec_cont->dpb_mode = DEC_DPB_DEFAULT;

        } else {
          dec_cont->dec_state = HEVCDEC_INITIALIZED;
          //return_value = DEC_HDRS_RDY;
        }

        /* Reset strm_len only for base view -> no HDRS_RDY to
         * application when param sets activated for stereo view */
         //strm_len = 0;

         //dec_cont->dpb_mode = DEC_DPB_DEFAULT;

/* Initialize tiled mode */
#if 0
                if( dec_cont->tiled_mode_support &&
                    DecCheckTiledMode( dec_cont->tiled_mode_support,
                        dec_cont->dpb_mode, 0) != HANTRO_OK )
                {
                    return_value = DEC_PARAM_ERROR;
                }
#endif

        break;
      }
#endif
      case HEVC_PIC_RDY: {
        u32 asic_status;
        u32 picture_broken;
        struct HevcDecAsic *asic_buff = dec_cont->asic_buff;

        picture_broken = (storage->picture_broken && storage->intra_freeze &&
                          !IS_IDR_NAL_UNIT(storage->prev_nal_unit));

        if (dec_cont->dec_state != HEVCDEC_BUFFER_EMPTY && !picture_broken) {
          /* setup the reference frame list; just at picture start */
          if (!HevcPpsSupported(dec_cont)) {
            storage->active_sps_id = MAX_NUM_SEQ_PARAM_SETS;
            storage->active_pps_id = MAX_NUM_PIC_PARAM_SETS;

            return_value = DEC_STREAM_NOT_SUPPORTED;
            goto end;
          }

          asic_buff->out_buffer = storage->curr_image->data;

          asic_buff->chroma_qp_index_offset = storage->active_pps->cb_qp_offset;
          asic_buff->chroma_qp_index_offset2 =
              storage->active_pps->cr_qp_offset;

          IncrementDPBRefCount(dec_cont->storage.dpb);

          /* Allocate memory for asic filter or reallocate in case old
             one is too small. */
#ifndef USE_EXTERNAL_BUFFER
          if (AllocateAsicTileEdgeMems(dec_cont) != HANTRO_OK) {
            return_value = DEC_MEMFAIL;
            goto end;
          }
#endif

          HevcSetRegs(dec_cont);

          /* determine initial reference picture lists */
          HevcInitRefPicList(dec_cont);

          DEBUG_PRINT(("Save DPB status\n"));
          /* we trust our memcpy; ignore return value */
          (void)DWLmemcpy(&storage->dpb[1], &storage->dpb[0],
                          sizeof(*storage->dpb));

          DEBUG_PRINT(("Save POC status\n"));
          (void)DWLmemcpy(&storage->poc[1], &storage->poc[0],
                          sizeof(*storage->poc));

          /* create output picture list */
          HevcUpdateAfterPictureDecode(dec_cont);

          /* enable output writing by default */
          dec_cont->asic_buff->disable_out_writing = 0;

          /* prepare PP if needed */
          /*HevcPreparePpRun(dec_cont);*/
        } else {
          dec_cont->dec_state = HEVCDEC_INITIALIZED;
        }

        /* run asic and react to the status */
        if (!picture_broken) {
          asic_status = HevcRunAsic(dec_cont, asic_buff);
        } else {
          if (dec_cont->storage.pic_started) {
            HevcInitPicFreezeOutput(dec_cont, 0);
            HevcUpdateAfterPictureDecode(dec_cont);
          }
          asic_status = DEC_HW_IRQ_ERROR;
        }

        if (!dec_cont->asic_running && !picture_broken)
          DecrementDPBRefCount(dec_cont->storage.dpb);

        /* Handle system error situations */
        if (asic_status == X170_DEC_TIMEOUT) {
          /* This timeout is DWL(software/os) generated */
          return DEC_HW_TIMEOUT;
        } else if (asic_status == X170_DEC_SYSTEM_ERROR) {
          return DEC_SYSTEM_ERROR;
        } else if (asic_status == X170_DEC_HW_RESERVED) {
          return DEC_HW_RESERVED;
        }

        /* Handle possible common HW error situations */
        if (asic_status & DEC_HW_IRQ_BUS) {
          output->strm_curr_pos = (u8 *)input->stream;
          output->strm_curr_bus_address = input->stream_bus_address;
          output->data_left = input->data_len;

          return DEC_HW_BUS_ERROR;
        }
        /* Handle stream error dedected in HW */
        else if ((asic_status & DEC_HW_IRQ_TIMEOUT) ||
                 (asic_status & DEC_HW_IRQ_ERROR)) {
          /* This timeout is HW generated */
          if (asic_status & DEC_HW_IRQ_TIMEOUT) {
            DEBUG_PRINT(("IRQ: HW TIMEOUT\n"));
#ifdef TIMEOUT_ASSERT
            ASSERT(0);
#endif
          } else {
            DEBUG_PRINT(("IRQ: STREAM ERROR dedected\n"));
          }

          if (dec_cont->packet_decoded != HANTRO_TRUE) {
            DEBUG_PRINT(("Reset pic_started\n"));
            dec_cont->storage.pic_started = HANTRO_FALSE;
          }

          dec_cont->storage.picture_broken = HANTRO_TRUE;
          HevcInitPicFreezeOutput(dec_cont, 1);

          {
            struct StrmData *strm = dec_cont->storage.strm;

            if (HevcNextStartCode(strm) == HANTRO_OK) {
              if(strm->strm_curr_pos >= tmp_stream)
                strm_len -= (strm->strm_curr_pos - tmp_stream);
              else
                strm_len -= (strm->strm_curr_pos + strm->strm_buff_size - tmp_stream);
              tmp_stream = strm->strm_curr_pos;
            }
          }

          dec_cont->stream_pos_updated = 0;
        } else if (asic_status & DEC_HW_IRQ_BUFFER) {
          /* TODO: Need to check for CABAC zero words here? */
          DEBUG_PRINT(("IRQ: BUFFER EMPTY\n"));

          /* a packet successfully decoded, don't Reset pic_started flag if
           * there is a need for rlc mode */
          dec_cont->dec_state = HEVCDEC_BUFFER_EMPTY;
          dec_cont->packet_decoded = HANTRO_TRUE;
          output->data_left = 0;

          return DEC_STRM_PROCESSED;
        } else /* OK in here */
        {
          if (IS_IDR_NAL_UNIT(storage->prev_nal_unit)) {
            dec_cont->storage.picture_broken = HANTRO_FALSE;
          }
        }

#if 0
                CHECK CABAC WORDS
                    struct StrmData strm_tmp = *dec_cont->storage.strm;
                    tmp = dec_cont->hw_stream_start-input->stream;
                    strm_tmp.strm_curr_pos = dec_cont->hw_stream_start;
                    strm_tmp.strm_buff_read_bits = 8*tmp;
                    strm_tmp.bit_pos_in_word = 0;
                    strm_tmp.strm_buff_size = input->data_len;
                    tmp = HevcCheckCabacZeroWords( &strm_tmp );
                    if( tmp != HANTRO_OK )
                    {
                        DEBUG_PRINT(("Error decoding CABAC zero words\n"));
                        {
                            struct StrmData *strm = dec_cont->storage.strm;
                            const u8 *next =
                                HevcFindNextStartCode(strm->strm_buff_start,
                                                         strm->strm_buff_size);

                            if(next != NULL)
                            {
                                u32 consumed;

                                tmp_stream -= num_read_bytes;
                                strm_len += num_read_bytes;

                                consumed = (u32) (next - tmp_stream);
                                tmp_stream += consumed;
                                strm_len -= consumed;
                            }
                        }

                }
#endif

        /* For the switch between modes */
        dec_cont->packet_decoded = HANTRO_FALSE;
        dec_cont->pic_number++;
        HevcCycleCount(dec_cont);

#ifdef FFWD_WORKAROUND
        storage->prev_idr_pic_ready = IS_IDR_NAL_UNIT(storage->prev_nal_unit);
#endif /* FFWD_WORKAROUND */
        {
          u32 sublayer = storage->active_sps->max_sub_layers - 1;
          u32 max_latency =
              dec_cont->storage.active_sps->max_num_reorder_pics[sublayer] +
              (dec_cont->storage.active_sps->max_latency_increase[sublayer]
                   ? dec_cont->storage.active_sps
                             ->max_latency_increase[sublayer] -
                         1
                   : 0);

          HevcDpbCheckMaxLatency(dec_cont->storage.dpb, max_latency);
        }

        return_value = DEC_PIC_DECODED;
        strm_len = 0;
        break;
      }
      case HEVC_PARAM_SET_ERROR: {
        if (!HevcValidParamSets(&dec_cont->storage) && strm_len == 0) {
          return_value = DEC_STRM_ERROR;
        }

        /* update HW buffers if VLC mode */
        dec_cont->hw_length -= num_read_bytes;
        if(tmp_stream >= input->stream)
          dec_cont->hw_stream_start_bus =
              input->stream_bus_address + (u32)(tmp_stream - input->stream);
        else
          dec_cont->hw_stream_start_bus =
              input->stream_bus_address + (u32)(tmp_stream + input->buff_len - input->stream);

        dec_cont->hw_stream_start = tmp_stream;

        break;
      }
      case HEVC_NEW_ACCESS_UNIT: {
        dec_cont->stream_pos_updated = 0;

        dec_cont->storage.picture_broken = HANTRO_TRUE;
        HevcInitPicFreezeOutput(dec_cont, 0);

        HevcUpdateAfterPictureDecode(dec_cont);

        /* PP will run in HevcDecNextPicture() for this concealed picture */
        return_value = DEC_PIC_DECODED;

        dec_cont->pic_number++;
        strm_len = 0;

        break;
      }
      case HEVC_NONREF_PIC_SKIPPED:
        return_value = DEC_NONREF_PIC_SKIPPED;
      /* fall through */
      default: /* case HEVC_ERROR, HEVC_RDY */
      {
        dec_cont->hw_length -= num_read_bytes;
        if(tmp_stream >= input->stream)
          dec_cont->hw_stream_start_bus =
              input->stream_bus_address + (u32)(tmp_stream - input->stream);
        else
          dec_cont->hw_stream_start_bus =
              input->stream_bus_address + (u32)(tmp_stream + input->buff_len - input->stream);

        dec_cont->hw_stream_start = tmp_stream;
      }
    }
  } while (strm_len);

end:

  /*  If Hw decodes stream, update stream buffers from "storage" */
  if (dec_cont->stream_pos_updated) {
    output->strm_curr_pos = (u8 *)dec_cont->hw_stream_start;
    output->strm_curr_bus_address = dec_cont->hw_stream_start_bus;
    output->data_left = dec_cont->hw_length;
  } else {
    /* else update based on SW stream decode stream values */
    u32 data_consumed = (u32)(tmp_stream - input->stream);
    if(tmp_stream >= input->stream)
      data_consumed = (u32)(tmp_stream - input->stream);
    else
      data_consumed = (u32)(tmp_stream + input->buff_len - input->stream);

    output->strm_curr_pos = (u8 *)tmp_stream;
    output->strm_curr_bus_address = input->stream_bus_address + data_consumed;
    if(output->strm_curr_bus_address >= (input->buffer_bus_address + input->buff_len))
      output->strm_curr_bus_address -= input->buff_len;

    output->data_left = input->data_len - data_consumed;
  }
  ASSERT(output->strm_curr_bus_address <= (input->buffer_bus_address + input->buff_len));

  FinalizeOutputAll(&dec_cont->fb_list);

  while (HevcDecNextPictureInternal(dec_cont) == DEC_PIC_RDY)
    ;

  return (return_value);
}

/* Returns the SW and HW build information. */
HevcDecBuild HevcDecGetBuild(void) {
  HevcDecBuild build_info;

  (void)DWLmemset(&build_info, 0, sizeof(build_info));

  build_info.sw_build = HANTRO_DEC_SW_BUILD;
  build_info.hw_build = DWLReadAsicID();

  DWLReadAsicConfig(&build_info.hw_config);

  return build_info;
}

/* Updates decoder instance after decoding of current picture */
void HevcUpdateAfterPictureDecode(struct HevcDecContainer *dec_cont) {

  struct Storage *storage = &dec_cont->storage;

  HevcResetStorage(storage);

  storage->pic_started = HANTRO_FALSE;
  storage->valid_slice_in_access_unit = HANTRO_FALSE;
}

/* Checks if active SPS is valid, i.e. supported in current profile/level */
u32 HevcSpsSupported(const struct HevcDecContainer *dec_cont) {
  const struct SeqParamSet *sps = dec_cont->storage.active_sps;

  /* check picture size (minimum defined in decapicommon.h) */
  if (sps->pic_width > dec_cont->max_dec_pic_width ||
      sps->pic_height > dec_cont->max_dec_pic_height) {
      ERROR_PRINT(("Warning Picture size May be not supported\n"));
  }
  if ((sps->pic_width  * sps->pic_height ) > (dec_cont->max_dec_pic_width * dec_cont->max_dec_pic_height) ||
      sps->pic_width < MIN_PIC_WIDTH || sps->pic_height < MIN_PIC_HEIGHT) {
    DEBUG_PRINT(("Picture size not supported!\n"));
    return 0;
  }

  /* check hevc main 10 profile supported or not*/
  if (((sps->bit_depth_luma != 8) || (sps->bit_depth_chroma != 8)) &&
      !dec_cont->hevc_main10_support) {
    DEBUG_PRINT(("Hevc main 10 profile not supported!\n"));
    return 0;
  }

  return 1;
}

/* Checks if active PPS is valid, i.e. supported in current profile/level */
u32 HevcPpsSupported(const struct HevcDecContainer *dec_cont) {
  return dec_cont ? 1 : 0;
}

/* Allocates necessary memory buffers. */
u32 HevcAllocateResources(struct HevcDecContainer *dec_cont) {
  u32 ret;
  struct HevcDecAsic *asic = dec_cont->asic_buff;
  struct Storage *storage = &dec_cont->storage;
  const struct SeqParamSet *sps = storage->active_sps;

  SetDecRegister(dec_cont->hevc_regs, HWIF_PIC_WIDTH_IN_CBS,
                 storage->pic_width_in_cbs);
  SetDecRegister(dec_cont->hevc_regs, HWIF_PIC_HEIGHT_IN_CBS,
                 storage->pic_height_in_cbs);

  {
    u32 ctb_size = 1 << sps->log_max_coding_block_size;
    u32 pic_width_in_ctbs = storage->pic_width_in_ctbs * ctb_size;
    u32 pic_height_in_ctbs = storage->pic_height_in_ctbs * ctb_size;

    u32 partial_ctb_h = sps->pic_width != pic_width_in_ctbs ? 1 : 0;
    u32 partial_ctb_v = sps->pic_height != pic_height_in_ctbs ? 1 : 0;

    SetDecRegister(dec_cont->hevc_regs, HWIF_PARTIAL_CTB_X, partial_ctb_h);
    SetDecRegister(dec_cont->hevc_regs, HWIF_PARTIAL_CTB_Y, partial_ctb_v);

    u32 min_cb_size = 1 << sps->log_min_coding_block_size;
    SetDecRegister(dec_cont->hevc_regs, HWIF_PIC_WIDTH_4X4,
                   (storage->pic_width_in_cbs * min_cb_size) >> 2);

    SetDecRegister(dec_cont->hevc_regs, HWIF_PIC_HEIGHT_4X4,
                   (storage->pic_height_in_cbs * min_cb_size) >> 2);
  }

#ifndef USE_EXTERNAL_BUFFER
  ReleaseAsicBuffers(dec_cont->dwl, asic);
#else
  ReleaseAsicBuffers(dec_cont, asic);
#endif
  ret = AllocateAsicBuffers(dec_cont, asic);
  if (ret == HANTRO_OK) {
      ret = AllocateAsicTileEdgeMems(dec_cont);
    }

  return ret;
}

/* Performs picture freeze for output. */
void HevcInitPicFreezeOutput(struct HevcDecContainer *dec_cont,
                             u32 from_old_dpb) {

  u32 index = 0;
  const u8 *ref_data;
  struct Storage *storage = &dec_cont->storage;
  const u32 dvm_mem_size = storage->dmv_mem_size;
  void *dvm_base = (u8 *)storage->curr_image->data->virtual_address +
                   dec_cont->storage.dpb->dir_mv_offset;

#ifdef _DISABLE_PIC_FREEZE
  return;
#endif

  /* for concealment after a HW error report we use the saved reference list */
  struct DpbStorage *dpb = &storage->dpb[from_old_dpb];

  do {
    ref_data = HevcGetRefPicData(dpb, dpb->list[index]);
    index++;
  } while (index < dpb->dpb_size && ref_data == NULL);

  ASSERT(dpb->current_out->data != NULL);

  /* Reset DMV storage for erroneous pictures */
  (void)DWLmemset(dvm_base, 0, dvm_mem_size);

  if (ref_data == NULL) {
    DEBUG_PRINT(("HevcInitPicFreezeOutput: pic freeze memset\n"));
    (void)DWLPrivateAreaMemset(storage->curr_image->data->virtual_address, 128,
                    storage->pic_size * 3 / 2);
  } else {
    DEBUG_PRINT(("HevcInitPicFreezeOutput: pic freeze memcopy\n"));
    (void)DWLPrivateAreaMemcpy(storage->curr_image->data->virtual_address, ref_data,
                    storage->pic_size * 3 / 2);
  }

  dpb = &storage->dpb[0]; /* update results for current output */

  {
    i32 i = dpb->num_out;
    u32 tmp = dpb->out_index_r;

    while ((i--) > 0) {
      if (tmp == dpb->dpb_size + 1) tmp = 0;

      if (dpb->out_buf[tmp].data == storage->curr_image->data) {
        dpb->out_buf[tmp].num_err_mbs = /* */ 1;
        break;
      }
      tmp++;
    }

    i = dpb->dpb_size + 1;

    while ((i--) > 0) {
      if (dpb->buffer[i].data == storage->curr_image->data) {
        ASSERT(&dpb->buffer[i] == dpb->current_out);
        break;
      }
    }
  }
}

/* Returns the sample aspect ratio info */
void HevcGetSarInfo(const struct Storage *storage, u32 *sar_width,
                    u32 *sar_height) {
  switch (HevcAspectRatioIdc(storage)) {
    case 0:
      *sar_width = 0;
      *sar_height = 0;
      break;
    case 1:
      *sar_width = 1;
      *sar_height = 1;
      break;
    case 2:
      *sar_width = 12;
      *sar_height = 11;
      break;
    case 3:
      *sar_width = 10;
      *sar_height = 11;
      break;
    case 4:
      *sar_width = 16;
      *sar_height = 11;
      break;
    case 5:
      *sar_width = 40;
      *sar_height = 33;
      break;
    case 6:
      *sar_width = 24;
      *sar_height = 1;
      break;
    case 7:
      *sar_width = 20;
      *sar_height = 11;
      break;
    case 8:
      *sar_width = 32;
      *sar_height = 11;
      break;
    case 9:
      *sar_width = 80;
      *sar_height = 33;
      break;
    case 10:
      *sar_width = 18;
      *sar_height = 11;
      break;
    case 11:
      *sar_width = 15;
      *sar_height = 11;
      break;
    case 12:
      *sar_width = 64;
      *sar_height = 33;
      break;
    case 13:
      *sar_width = 160;
      *sar_height = 99;
      break;
    case 255:
      HevcSarSize(storage, sar_width, sar_height);
      break;
    default:
      *sar_width = 0;
      *sar_height = 0;
  }
}

/* Get last decoded picture if any available. No pictures are removed
 * from output nor DPB buffers. */
enum DecRet HevcDecPeek(HevcDecInst dec_inst, struct HevcDecPicture *output) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  struct DpbPicture *current_out = dec_cont->storage.dpb->current_out;

  if (dec_inst == NULL || output == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  if (dec_cont->dec_state != HEVCDEC_NEW_HEADERS &&
      dec_cont->storage.dpb->fullness && current_out != NULL &&
      current_out->status != EMPTY) {

    u32 cropping_flag;

    output->output_picture = current_out->data->virtual_address;
    output->output_picture_bus_address = current_out->data->bus_address;
    output->pic_id = current_out->pic_id;
    output->is_idr_picture = current_out->is_idr;

    output->pic_width = HevcPicWidth(&dec_cont->storage);
    output->pic_height = HevcPicHeight(&dec_cont->storage);

    HevcCroppingParams(&dec_cont->storage, &cropping_flag,
                       &output->crop_params.crop_left_offset,
                       &output->crop_params.crop_out_width,
                       &output->crop_params.crop_top_offset,
                       &output->crop_params.crop_out_height);

    if (cropping_flag == 0) {
      output->crop_params.crop_left_offset = 0;
      output->crop_params.crop_top_offset = 0;
      output->crop_params.crop_out_width = output->pic_width;
      output->crop_params.crop_out_height = output->pic_height;
    }

    return (DEC_PIC_RDY);
  } else {
    return (DEC_OK);
  }
}

enum DecRet HevcDecNextPictureInternal(struct HevcDecContainer *dec_cont) {
  struct HevcDecPicture out_pic;
  struct HevcDecInfo dec_info;
  const struct DpbOutPicture *dpb_out = NULL;
  u32 bit_depth, out_bit_depth;
#ifdef DOWN_SCALER
  u32 pic_width, pic_height;
#endif

  dpb_out = HevcNextOutputPicture(&dec_cont->storage);

  if (dpb_out == NULL) return DEC_OK;

#ifdef DOWN_SCALER
  pic_width = dpb_out->pic_width;
  pic_height = dpb_out->pic_height;
#endif
  bit_depth = (dpb_out->bit_depth_luma == 8 && dpb_out->bit_depth_chroma == 8) ? 8 : 10;
  out_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                   dec_cont->use_p010_output ? 16 : 10;
  out_pic.bit_depth_luma = dpb_out->bit_depth_luma;
  out_pic.bit_depth_chroma = dpb_out->bit_depth_chroma;
  out_pic.output_format = dec_cont->output_format;
  out_pic.pic_height = dpb_out->pic_height;
  out_pic.pic_width = dpb_out->pic_width;
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN)
    out_pic.pic_stride = NEXT_MULTIPLE(dpb_out->pic_width * out_bit_depth, 16 * 8) / 8;
  else
    out_pic.pic_stride = dpb_out->pic_width * bit_depth / 8;
  out_pic.output_picture = dpb_out->data->virtual_address;
  out_pic.output_picture_bus_address = dpb_out->data->bus_address;
  out_pic.output_rfc_luma_base = dpb_out->data->virtual_address + dec_cont->storage.dpb[0].cbs_tbl_offsety;
  out_pic.output_rfc_luma_bus_address = dpb_out->data->bus_address + dec_cont->storage.dpb[0].cbs_tbl_offsety;
  out_pic.output_rfc_chroma_base = dpb_out->data->virtual_address + dec_cont->storage.dpb[0].cbs_tbl_offsetc;
  out_pic.output_rfc_chroma_bus_address = dpb_out->data->bus_address + dec_cont->storage.dpb[0].cbs_tbl_offsetc;
  ASSERT(out_pic.output_picture);
  ASSERT(out_pic.output_picture_bus_address);
  out_pic.pic_id = dpb_out->pic_id;
  out_pic.is_idr_picture = dpb_out->is_idr;
  out_pic.pic_corrupt = dpb_out->num_err_mbs ? 1 : 0;
  out_pic.crop_params = dpb_out->crop_params;
  out_pic.cycles_per_mb = dpb_out->cycles_per_mb;
#ifdef DOWN_SCALER
  if (dec_cont->storage.down_scale_enabled) {
    out_pic.dscale_stride = NEXT_MULTIPLE((pic_width >> dec_cont->storage.down_scale_x_shift) * out_bit_depth, 16 * 8) / 8;
    out_pic.dscale_width = (pic_width / 2 >> dec_cont->storage.down_scale_x_shift) << 1;
    out_pic.dscale_height = (pic_height / 2 >> dec_cont->storage.down_scale_y_shift) << 1;
  } else {
    out_pic.dscale_stride = 0;
    out_pic.dscale_width = 0;
    out_pic.dscale_height = 0;
    out_pic.output_downscale_picture = NULL;
    out_pic.output_downscale_picture_bus_address = 0;
    out_pic.output_downscale_picture_chroma = NULL;
    out_pic.output_downscale_picture_chroma_bus_address = 0;
  }
#endif

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN || dec_cont->down_scale_enabled) {
    if (dec_cont->use_p010_output && bit_depth > 8)
      out_pic.pixel_format = DEC_OUT_PIXEL_P010;
    else if (dec_cont->pixel_format == DEC_OUT_PIXEL_CUSTOMER1)
      out_pic.pixel_format = DEC_OUT_PIXEL_CUSTOMER1;
    else if (dec_cont->use_8bits_output)
      out_pic.pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
    else
      out_pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  } else {
    /* Reference buffer. */
    out_pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  }
  (void)HevcDecGetInfo((HevcDecInst)dec_cont, &dec_info);
  (void)DWLmemcpy(&out_pic.dec_info, &dec_info, sizeof(struct HevcDecInfo));
  out_pic.dec_info.pic_buff_size = dec_cont->storage.dpb->tot_buffers;

  PushOutputPic(&dec_cont->fb_list, &out_pic, dpb_out->mem_idx);

  return (DEC_PIC_RDY);
}

enum DecRet HevcDecNextPicture(HevcDecInst dec_inst,
                               struct HevcDecPicture *picture) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  if (dec_inst == NULL || picture == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  if (dec_cont->dec_state == HEVCDEC_EOS && IsOutputEmpty(&dec_cont->fb_list)) {
    return (DEC_END_OF_STREAM);
  }

  if (PeekOutputPic(&dec_cont->fb_list, picture)) {
    /* Do the tiled-to-raster conversion if necessary. */
    if (dec_cont->storage.raster_enabled || dec_cont->storage.down_scale_enabled) {
      struct DWLLinearMem tiled, raster;
      tiled.virtual_address = (u32 *)picture->output_picture;
      tiled.bus_address = picture->output_picture_bus_address;
      if (dec_cont->storage.raster_enabled) {
        raster = RbmGetRasterBuffer(dec_cont->storage.raster_buffer_mgr, tiled);
        picture->output_picture = raster.virtual_address;
        picture->output_picture_bus_address = raster.bus_address;
      }
#ifdef DOWN_SCALER
      if (dec_cont->storage.down_scale_enabled) {
        struct DWLLinearMem dscale;
        dscale = RbmGetDscaleBuffer(dec_cont->storage.raster_buffer_mgr, tiled);
        picture->output_downscale_picture = dscale.virtual_address;
        picture->output_downscale_picture_bus_address = dscale.bus_address;
        picture->output_downscale_picture_chroma = dscale.virtual_address +
          picture->dscale_stride * (picture->pic_height >> dec_cont->storage.down_scale_y_shift) / 4;
        picture->output_downscale_picture_chroma_bus_address = dscale.bus_address +
          picture->dscale_stride * (picture->pic_height >> dec_cont->storage.down_scale_y_shift);
      }
#endif
    }
    return (DEC_PIC_RDY);
  } else {
    return (DEC_OK);
  }
}

/*!\brief Mark last output picture consumed
 *
 * Application calls this after it has finished processing the picture
 * returned by HevcDecMCNextPicture.
 */

enum DecRet HevcDecPictureConsumed(HevcDecInst dec_inst,
                                   const struct HevcDecPicture *picture) {
  u32 id;
  const struct DpbStorage *dpb;
  struct HevcDecPicture pic;
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  if (dec_inst == NULL || picture == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  dpb = dec_cont->storage.dpb;

  /* Do the raster-to-tiled conversion if necessary. */
  pic = *picture;
  if (dec_cont->storage.raster_enabled || dec_cont->storage.down_scale_enabled) {
    struct DWLLinearMem tiled, key;
    if (dec_cont->storage.raster_enabled && pic.output_picture != NULL) {
      key.virtual_address = (u32 *)pic.output_picture;
      key.bus_address = pic.output_picture_bus_address;
    }
#ifdef DOWN_SCALER
    else {
      key.virtual_address = (u32 *)pic.output_downscale_picture;
      key.bus_address = pic.output_downscale_picture_bus_address;
    }
#endif
    tiled = RbmGetTiledBuffer(dec_cont->storage.raster_buffer_mgr, key);
    pic.output_picture = tiled.virtual_address;
    pic.output_picture_bus_address = tiled.bus_address;
  }

  /* find the mem descriptor for this specific buffer */
  for (id = 0; id < dpb->tot_buffers; id++) {
    if (pic.output_picture_bus_address == dpb->pic_buffers[id].bus_address &&
        pic.output_picture == dpb->pic_buffers[id].virtual_address) {
      break;
    }
  }

  /* check that we have a valid id */
  if (id >= dpb->tot_buffers) return DEC_PARAM_ERROR;

  PopOutputPic(&dec_cont->fb_list, dpb->pic_buff_id[id]);

  return DEC_OK;
}

enum DecRet HevcDecEndOfStream(HevcDecInst dec_inst) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  if (dec_inst == NULL) {
    return (DEC_PARAM_ERROR);
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return (DEC_NOT_INITIALIZED);
  }

  if (dec_cont->asic_running) {
    /* stop HW */
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ, 0);
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_E, 0);
    DWLDisableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                 dec_cont->hevc_regs[1]);

    DWLReleaseHw(dec_cont->dwl, dec_cont->core_id); /* release HW lock */

    DecrementDPBRefCount(dec_cont->storage.dpb);

    dec_cont->asic_running = 0;
  }

  /* flush any remaining pictures form DPB */
  HevcFlushBuffer(&dec_cont->storage);

  FinalizeOutputAll(&dec_cont->fb_list);

  while (HevcDecNextPictureInternal(dec_cont) == DEC_PIC_RDY)
    ;

  /* After all output pictures were pushed, update decoder status to
   * reflect the end-of-stream situation. This way the HevcDecNextPicture
   * will not block anymore once all output was handled.
   */
  dec_cont->dec_state = HEVCDEC_EOS;

  /* wake-up output thread */
  PushOutputPic(&dec_cont->fb_list, NULL, -1);

  /* block until all output is handled */
  WaitListNotInUse(&dec_cont->fb_list);

  return (DEC_OK);
}

enum DecRet HevcDecUseExtraFrmBuffers(HevcDecInst dec_inst, u32 n) {

  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  dec_cont->storage.n_extra_frm_buffers = n;

  return DEC_OK;
}

void HevcCycleCount(struct HevcDecContainer *dec_cont) {
  struct DpbPicture *current_out = dec_cont->storage.dpb->current_out;
  u32 cycles = 0;
  u32 mbs = (HevcPicWidth(&dec_cont->storage) *
    HevcPicHeight(&dec_cont->storage)) >> 8;
  if (mbs)
    cycles = GetDecRegister(dec_cont->hevc_regs, HWIF_PERF_CYCLE_COUNT) / mbs;

  current_out->cycles_per_mb = cycles;
}

#ifdef USE_EXTERNAL_BUFFER
enum DecRet HevcDecAddBuffer(HevcDecInst dec_inst,
                     struct DWLLinearMem *info) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;
  enum DecRet dec_ret = DEC_OK;
  struct Storage *storage = &dec_cont->storage;

  if (dec_inst == NULL || info == NULL ||
      X170_CHECK_VIRTUAL_ADDRESS(info->virtual_address) ||
      X170_CHECK_BUS_ADDRESS_AGLINED(info->bus_address) ||
      info->logical_size < dec_cont->next_buf_size) {
    return DEC_PARAM_ERROR;
  }

  if (dec_cont->buf_num == 0)
    return DEC_EXT_BUFFER_REJECTED;

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
  case TILE_EDGE_BUFFER:
    asic_buff->tile_edge = *info;
    dec_cont->buf_to_free = NULL;
    dec_cont->next_buf_size = 0;
    dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 0;
#endif
    break;
  case REFERENCE_BUFFER: {
    i32 i = dec_cont->buffer_index;
    struct DpbStorage *dpb = dec_cont->storage.dpb;
    if (i < dpb->tot_buffers) {
      dpb->pic_buffers[i] = *info;
      if (i < dpb->dpb_size + 1) {
        u32 id = AllocateIdUsed(dpb->fb_list, dpb->pic_buffers + i);
        if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

        dpb->buffer[i].data = dpb->pic_buffers + i;
        dpb->buffer[i].mem_idx = id;
        dpb->pic_buff_id[i] = id;
      } else {
        u32 id = AllocateIdFree(dpb->fb_list, dpb->pic_buffers + i);
        if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

        dpb->pic_buff_id[i] = id;
      }

      {
        void *base =
            (char *)(dpb->pic_buffers[i].virtual_address) + dpb->dir_mv_offset;
        (void)DWLmemset(base, 0, info->logical_size - dpb->dir_mv_offset);
      }

      dec_cont->buffer_index++;
      dec_cont->buf_num--;
    }

    if (dec_cont->buffer_index < dpb->tot_buffers)
      dec_ret = DEC_WAITING_FOR_BUFFER;
    else if (storage->raster_enabled) {
      /* Reference buffer done. */
      /* Continue to allocate raster scan / down scale buffer if needed. */
      const struct SeqParamSet *sps = storage->active_sps;
//      struct DpbInitParams dpb_params;
      struct RasterBufferParams params;
      u32 pixel_width = (sps->bit_depth_luma == 8 && sps->bit_depth_chroma == 8) ? 8 : 10;
      u32 rs_pixel_width = (dec_cont->use_8bits_output || pixel_width == 8) ? 8 :
                           (dec_cont->use_p010_output ? 16 : 10);

      params.dwl = dec_cont->dwl;
      params.num_buffers = storage->dpb->tot_buffers;
      for (int i = 0; i < params.num_buffers; i++)
        dec_cont->tiled_buffers[i] = storage->dpb[0].pic_buffers[i];
      params.tiled_buffers = dec_cont->tiled_buffers;
      /* Raster out writes to modulo-16 widths. */
      params.width = NEXT_MULTIPLE(sps->pic_width * rs_pixel_width, 16 * 8) / 8;
      params.height = sps->pic_height;
      params.ext_buffer_config = dec_cont->ext_buffer_config;
#ifdef DOWN_SCALER
      if (storage->down_scale_enabled) {
        params.ds_width = NEXT_MULTIPLE((sps->pic_width >> storage->down_scale_x_shift) * rs_pixel_width, 16 * 8) / 8;
        params.ds_height = sps->pic_height >> storage->down_scale_y_shift;
      } else {
        params.ds_width = 0;
        params.ds_height = 0;
      }
#endif
      dec_cont->params = params;

      if (storage->raster_buffer_mgr) {
        dec_cont->_buf_to_free = RbmReleaseBuffer(storage->raster_buffer_mgr);

        if (dec_cont->_buf_to_free.virtual_address != 0) {
          dec_cont->buf_to_free = &dec_cont->_buf_to_free;
          dec_cont->next_buf_size = 0;
          dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
          dec_cont->is_frame_buffer = 0;
#endif
          dec_ret = DEC_WAITING_FOR_BUFFER;
          dec_cont->rbm_release = 1;
          break;
        }
        RbmRelease(storage->raster_buffer_mgr);
        storage->raster_buffer_mgr = NULL;
        dec_cont->rbm_release = 0;
      }
      storage->raster_buffer_mgr = RbmInit(params);
      if (storage->raster_buffer_mgr == NULL) return HANTRO_NOK;

      if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
        /* Allocate raster scan / down scale buffers. */
        dec_cont->buf_type = RASTERSCAN_OUT_BUFFER;
        dec_cont->next_buf_size = params.width * params.height * 3 / 2;
        dec_cont->buf_to_free = NULL;
        dec_cont->buf_num = dec_cont->params.num_buffers;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif

        dec_cont->buffer_index = 0;
        dec_ret = DEC_WAITING_FOR_BUFFER;
      }
#ifdef DOWN_SCALER
      else if (dec_cont->down_scale_enabled &&
               IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
        dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
        dec_cont->next_buf_size = dec_cont->params.ds_width * dec_cont->params.ds_height * 3 / 2;
        dec_cont->buf_to_free = NULL;
        dec_cont->buf_num = dec_cont->params.num_buffers;
        dec_ret = DEC_WAITING_FOR_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
      }
#endif
      else {
        dec_cont->buf_to_free = NULL;
        dec_cont->next_buf_size = 0;
        dec_cont->buffer_index = 0;
        dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
      }
    } else {
      dec_cont->buf_to_free = NULL;
      dec_cont->next_buf_size = 0;
      dec_cont->buffer_index = 0;
      dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    }
    break;
  }
  case RASTERSCAN_OUT_BUFFER: {
    u32 i = dec_cont->buffer_index;

    ASSERT(storage->raster_enabled);
    ASSERT(dec_cont->buffer_index < dec_cont->params.num_buffers);

    RbmAddRsBuffer(storage->raster_buffer_mgr, info, i);
    dec_cont->buffer_index++;
    dec_cont->buf_num--;

    /* Need to add all the picture buffers in state HEVCDEC_NEW_HEADERS. */
    /* Reference buffer always allocated along with raster/dscale buffer. */
    /* Step for raster buffer. */
    if (dec_cont->buffer_index < dec_cont->params.num_buffers)
      dec_ret = DEC_WAITING_FOR_BUFFER;
    else {
#ifdef DOWN_SCALER
      if (dec_cont->down_scale_enabled &&
          IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
        dec_cont->buffer_index = 0;
        dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
        dec_cont->next_buf_size = dec_cont->params.ds_width * dec_cont->params.ds_height * 3 / 2;
        dec_cont->buf_to_free = NULL;
        dec_cont->buf_num = dec_cont->params.num_buffers;
        dec_ret = DEC_WAITING_FOR_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
      }
      else
#endif
      {
        dec_cont->next_buf_size = 0;
        dec_cont->buf_to_free = NULL;
        dec_cont->buffer_index = 0;
        dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 0;
#endif
      }
    }
    break;
  }
#ifdef DOWN_SCALER
  case DOWNSCALE_OUT_BUFFER: {
    u32 i = dec_cont->buffer_index;

    ASSERT(storage->down_scale_enabled);
    ASSERT(dec_cont->buffer_index < dec_cont->params.num_buffers);

    RbmAddDsBuffer(storage->raster_buffer_mgr, info, i);

    dec_cont->buffer_index++;
    dec_cont->buf_num--;

    if (dec_cont->buffer_index == dec_cont->params.num_buffers) {
      dec_cont->next_buf_size = 0;
      dec_cont->buf_to_free = NULL;
      dec_cont->buffer_index = 0;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    } else {
      dec_cont->buf_to_free = NULL;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      dec_ret = DEC_WAITING_FOR_BUFFER;
    }
    break;
  }
#endif
  default:
    break;
  }

  return dec_ret;
}


enum DecRet HevcDecGetBufferInfo(HevcDecInst dec_inst, struct HevcDecBufferInfo *mem_info) {
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
//  enum DecRet dec_ret = DEC_OK;
  struct DWLLinearMem empty = {0};

  if (dec_inst == NULL || mem_info == NULL) {
    return DEC_PARAM_ERROR;
  }

  if (dec_cont->buf_to_free == NULL && dec_cont->next_buf_size == 0) {
    if (dec_cont->rbm_release == 1) {
      /* All the raster buffers should be freed before being reallocated. */
      struct Storage *storage = &dec_cont->storage;

      ASSERT(storage->raster_buffer_mgr);
      if (storage->raster_buffer_mgr) {
        dec_cont->_buf_to_free = RbmReleaseBuffer(storage->raster_buffer_mgr);

        if (dec_cont->_buf_to_free.virtual_address != 0) {
          dec_cont->buf_to_free = &dec_cont->_buf_to_free;
          dec_cont->next_buf_size = 0;
          dec_cont->rbm_release = 1;
          dec_cont->buf_num = 0;
#ifdef ASIC_TRACE_SUPPORT
          dec_cont->is_frame_buffer = 0;
#endif
        } else {
          RbmRelease(storage->raster_buffer_mgr);
          dec_cont->rbm_release = 0;

          storage->raster_buffer_mgr = RbmInit(dec_cont->params);
          if (storage->raster_buffer_mgr == NULL) return DEC_OK;

          /* Allocate raster scan / down scale buffers. */
          if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
            dec_cont->buf_type = RASTERSCAN_OUT_BUFFER;
            dec_cont->next_buf_size = dec_cont->params.width * dec_cont->params.height * 3 / 2;
          }
#ifdef DOWN_SCALER
          else if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
            dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
            dec_cont->next_buf_size = dec_cont->params.ds_width * dec_cont->params.ds_height * 3 / 2;
          }
#endif
          dec_cont->buf_to_free = NULL;
          dec_cont->buf_num = dec_cont->params.num_buffers;
#ifdef ASIC_TRACE_SUPPORT
          dec_cont->is_frame_buffer = 0;
#endif
          dec_cont->buffer_index = 0;

          mem_info->buf_to_free = empty;
          mem_info->next_buf_size = dec_cont->next_buf_size;
          mem_info->buf_num = dec_cont->buf_num;
#ifdef ASIC_TRACE_SUPPORT
          mem_info->is_frame_buffer = dec_cont->is_frame_buffer;
#endif
          return DEC_OK;
        }
      }

    } else {
      /* External reference buffer: release done. */
      mem_info->buf_to_free = empty;
      mem_info->next_buf_size = dec_cont->next_buf_size;
      mem_info->buf_num = dec_cont->buf_num;
#ifdef ASIC_TRACE_SUPPORT
      mem_info->is_frame_buffer = dec_cont->is_frame_buffer;
#endif
      return DEC_OK;
    }
  }

  if (dec_cont->buf_to_free) {
    mem_info->buf_to_free = *dec_cont->buf_to_free;

    // TODO(min): here we assume that the buffer should be freed externally.
    dec_cont->buf_to_free->virtual_address = NULL;
    dec_cont->buf_to_free = NULL;
  } else
    mem_info->buf_to_free = empty;
  mem_info->next_buf_size = dec_cont->next_buf_size;
  mem_info->buf_num = dec_cont->buf_num;

  ASSERT((mem_info->buf_num && mem_info->next_buf_size) || (mem_info->buf_to_free.virtual_address != NULL));
#ifdef ASIC_TRACE_SUPPORT
  mem_info->is_frame_buffer = dec_cont->is_frame_buffer;
#endif

  return DEC_WAITING_FOR_BUFFER;
}
#endif


