/* Copyright 2013 Google Inc. All Rights Reserved. */

/* TODO(mheikkinen) do pthread wrapper. */
#include <pthread.h>

#include "basetype.h"
#include "decapicommon.h"
#include "dwl.h"
#include "fifo.h"
#include "regdrv.h"
#include "vp9decapi.h"
#include "vp9hwd_asic.h"
#include "vp9hwd_container.h"
#include "vp9hwd_output.h"

#define EOS_MARKER (-1)

static u32 CycleCount(struct Vp9DecContainer *dec_cont);
static i32 FindIndex(struct Vp9DecContainer *dec_cont, const u32 *address);
static i32 NextOutput(struct Vp9DecContainer *dec_cont);
static i32 Vp9ProcessAsicStatus(struct Vp9DecContainer *dec_cont,
                                u32 asic_status, u32 *error_concealment);
static void Vp9ConstantConcealment(struct Vp9DecContainer *dec_cont, u8 value);

u32 CycleCount(struct Vp9DecContainer *dec_cont) {
  u32 cycles = 0;
  u32 mbs = (NEXT_MULTIPLE(dec_cont->height, 16) *
    NEXT_MULTIPLE(dec_cont->width, 16)) >> 8;
  if (mbs)
    cycles = GetDecRegister(dec_cont->vp9_regs, HWIF_PERF_CYCLE_COUNT) / mbs;
  DEBUG_PRINT(("Pic %3d cycles/mb %4d\n", dec_cont->pic_number, cycles));

  return cycles;
}

i32 FindIndex(struct Vp9DecContainer *dec_cont, const u32 *address) {
  i32 i;
  struct DWLLinearMem *pictures;

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN)
#ifndef USE_EXTERNAL_BUFFER
    pictures = dec_cont->asic_buff->raster_luma;
#else
    pictures = dec_cont->asic_buff->raster;
#endif
  else
    pictures = dec_cont->asic_buff->pictures;

  for (i = 0; i < (i32)dec_cont->num_buffers; i++)
    if ((*(pictures + i)).virtual_address == address) break;
  ASSERT((u32)i < dec_cont->num_buffers);
  return i;
}

i32 NextOutput(struct Vp9DecContainer *dec_cont) {
  i32 i;
  u32 j;
  i32 output_i = -1;
  u32 size;
  FifoObject tmp;

  size = FifoCount(dec_cont->fifo_display);

  /* If there are pictures in the display reordering buffer, check them
   * first to see if our next output is there. */
  for (j = 0; j < size; j++) {
    FifoPop(dec_cont->fifo_display, &tmp, FIFO_EXCEPTION_DISABLE);
    i = (i32)(*(addr_t *)&tmp);

    if (dec_cont->asic_buff->display_index[i] == dec_cont->pic_number) {
      /*  fifo_display had the right output. */
      output_i = i;
      break;
    } else {
      tmp = (FifoObject)(addr_t)i;
      FifoPush(dec_cont->fifo_display, tmp, FIFO_EXCEPTION_DISABLE);
    }
  }

  /* Look for output in decode ordered out_fifo. */
  while (output_i < 0) {
    /* Blocks until next output is available */
    FifoPop(dec_cont->fifo_out, &tmp, FIFO_EXCEPTION_DISABLE);
    i = (i32)(*(addr_t *)&tmp);
    if (i == EOS_MARKER) return i;

    if (dec_cont->asic_buff->display_index[i] == dec_cont->pic_number) {
      /*  fifo_out had the right output. */
      output_i = i;
    } else {
      /* Until we get the next picture in display order, push the outputs
      * to the display reordering fifo */
      tmp = (FifoObject)(addr_t)i;
      FifoPush(dec_cont->fifo_display, tmp, FIFO_EXCEPTION_DISABLE);
    }
  }

  return output_i;
}

enum DecRet Vp9DecPictureConsumed(Vp9DecInst dec_inst,
                                  const struct Vp9DecPicture *picture) {
  if (dec_inst == NULL || picture == NULL) {
    return DEC_PARAM_ERROR;
  }
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
  struct Vp9DecPicture pic = *picture;

  /* Remove the reference to the buffer. */
  Vp9BufferQueueRemoveRef(dec_cont->bq,
                          FindIndex(dec_cont, pic.output_luma_base));

  pthread_mutex_lock(&dec_cont->sync_out);
  // Release buffer for use as an output (i.e. "show existing frame"). A buffer can
  // be in the output queue once at a time.
  dec_cont->asic_buff->display_index[FindIndex(dec_cont, pic.output_luma_base)] = 0;

  pthread_cond_signal(&dec_cont->sync_out_cv);
  pthread_mutex_unlock(&dec_cont->sync_out);

  return DEC_OK;
}

enum DecRet Vp9DecNextPicture(Vp9DecInst dec_inst,
                              struct Vp9DecPicture *output) {
  i32 i;
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;
  if (dec_inst == NULL || output == NULL) {
    return DEC_PARAM_ERROR;
  }

  /* Check for valid decoder instance */
  if (dec_cont->checksum != dec_cont) {
    return DEC_NOT_INITIALIZED;
  }

  /*  NextOutput will block until there is an output. */
  i = NextOutput(dec_cont);
  if (i == EOS_MARKER) {
    return DEC_END_OF_STREAM;
  }

  ASSERT(i >= 0 && (u32)i < dec_cont->num_buffers);
  *output = dec_cont->asic_buff->picture_info[i];
  output->pic_id = dec_cont->pic_number++;

  return DEC_PIC_RDY;
}

enum DecRet Vp9DecEndOfStream(Vp9DecInst dec_inst) {
  if (dec_inst == NULL) {
    return DEC_PARAM_ERROR;
  }
  struct Vp9DecContainer *dec_cont = (struct Vp9DecContainer *)dec_inst;

  /* Don't do end of stream twice. This is not thread-safe, so it must be
   * called from the single input thread that is also used to call
   * Vp9DecDecode. */
  if (dec_cont->dec_stat == VP9DEC_END_OF_STREAM) {
    ASSERT(0); /* Let the assert kill the stuff in debug mode */
    return DEC_END_OF_STREAM;
  }

  (void)VP9SyncAndOutput(dec_cont);

  /* If buffer queue has been already initialized, we can use it to track
   * pending cores and outputs safely. */
  if (dec_cont->bq) {
    /* if the references and queue were already flushed, cannot
     * do it again. */
    if (dec_cont->asic_buff->out_buffer_i != VP9_UNDEFINED_BUFFER) {
      u32 i = 0;
      /* Workaround for ref counting since this buffer is never used. */
      Vp9BufferQueueRemoveRef(dec_cont->bq, dec_cont->asic_buff->out_buffer_i);
      dec_cont->asic_buff->out_buffer_i = VP9_UNDEFINED_BUFFER;

      for (i = 0; i < dec_cont->num_buffers; i++) {
        Vp9BufferQueueRemoveRef(dec_cont->bq,
                                Vp9BufferQueueGetRef(dec_cont->bq, i));
      }
    }
  }
  FifoPush(dec_cont->fifo_out, (void *)EOS_MARKER, FIFO_EXCEPTION_DISABLE);
  return DEC_OK;
}

void Vp9PicToOutput(struct Vp9DecContainer *dec_cont) {
  struct PicCallbackArg info = dec_cont->pic_callback_arg[0];
  FifoObject tmp;

  pthread_mutex_lock(&dec_cont->sync_out);
  while (dec_cont->asic_buff->display_index[info.index])
      pthread_cond_wait(&dec_cont->sync_out_cv, &dec_cont->sync_out);
  pthread_mutex_unlock(&dec_cont->sync_out);

  info.pic.cycles_per_mb = CycleCount(dec_cont);
  dec_cont->asic_buff->picture_info[info.index] = info.pic;
  if (info.show_frame) {
    dec_cont->asic_buff->display_index[info.index] = dec_cont->display_number++;
    tmp = (FifoObject)(addr_t)info.index;
    FifoPush(dec_cont->fifo_out, tmp, FIFO_EXCEPTION_DISABLE);
  }
}

#ifndef USE_EXTERNAL_BUFFER
void Vp9SetupPicToOutput(struct Vp9DecContainer *dec_cont) {
  struct PicCallbackArg *args = &dec_cont->pic_callback_arg[dec_cont->core_id];
  u32 bit_depth = dec_cont->decoder.bit_depth;
  u32 rs_bit_depth;
  rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                  dec_cont->use_p010_output ? 16 : bit_depth;

  args->index = dec_cont->asic_buff->out_buffer_i;
  args->fifo_out = dec_cont->fifo_out;

  if (dec_cont->decoder.show_existing_frame) {
    args->pic = dec_cont->asic_buff->picture_info[args->index];
    args->pic.is_intra_frame = 0;
    args->show_frame = 1;
    return;
  }
  args->show_frame = dec_cont->decoder.show_frame;
  /* Fill in the picture information for everything we know. */
  args->pic.is_intra_frame = dec_cont->decoder.key_frame;
  args->pic.is_golden_frame = 0;
  /* Frame size and format information. */
  args->pic.frame_width = NEXT_MULTIPLE(dec_cont->width, 8);
  args->pic.frame_height = NEXT_MULTIPLE(dec_cont->height, 8);
  args->pic.coded_width = dec_cont->width;
  args->pic.coded_height = dec_cont->height;
  args->pic.output_format = dec_cont->output_format;
  args->pic.bit_depth_luma = args->pic.bit_depth_chroma = bit_depth;
  args->pic.pic_stride = NEXT_MULTIPLE(args->pic.frame_width * bit_depth, 16 * 8) / 8;
#ifdef DOWN_SCALER
  args->pic.dscale_width = 0;
  args->pic.dscale_height = 0;
  args->pic.dscale_stride = 0;
  args->pic.output_dscale_luma_base = NULL;
  args->pic.output_dscale_luma_bus_address = 0;
  args->pic.output_dscale_chroma_base = NULL;
  args->pic.output_chroma_bus_address = 0;

  if (dec_cont->down_scale_enabled) {
    u32 decoded_width = NEXT_MULTIPLE(dec_cont->width, 8);
    args->pic.dscale_width = (dec_cont->width / 2 >> dec_cont->down_scale_x_shift) << 1;
    args->pic.dscale_height = (dec_cont->height / 2 >> dec_cont->down_scale_y_shift) << 1;
    args->pic.dscale_stride = NEXT_MULTIPLE((decoded_width >> dec_cont->down_scale_x_shift) * rs_bit_depth, 16 * 8) / 8;
    args->pic.output_dscale_luma_base =
        dec_cont->asic_buff->dscale_luma[args->index].virtual_address;
    args->pic.output_dscale_luma_bus_address =
        dec_cont->asic_buff->dscale_luma[args->index].bus_address;
    args->pic.output_dscale_chroma_base =
        dec_cont->asic_buff->dscale_chroma[args->index].virtual_address;
    args->pic.output_dscale_chroma_bus_address =
        dec_cont->asic_buff->dscale_chroma[args->index].bus_address;
  }
#endif
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    args->pic.pic_stride = NEXT_MULTIPLE(args->pic.frame_width * rs_bit_depth, 16 * 8) / 8;
    args->pic.output_luma_base =
        dec_cont->asic_buff->raster_luma[args->index].virtual_address;
    args->pic.output_luma_bus_address =
        dec_cont->asic_buff->raster_luma[args->index].bus_address;
    args->pic.output_chroma_base =
        dec_cont->asic_buff->raster_chroma[args->index].virtual_address;
    args->pic.output_chroma_bus_address =
        dec_cont->asic_buff->raster_chroma[args->index].bus_address;
  } else {
    args->pic.output_luma_base =
        dec_cont->asic_buff->pictures[args->index].virtual_address;
    args->pic.output_luma_bus_address =
        dec_cont->asic_buff->pictures[args->index].bus_address;
    args->pic.output_chroma_base =
        dec_cont->asic_buff->pictures_c[args->index].virtual_address;
    args->pic.output_chroma_bus_address =
        dec_cont->asic_buff->pictures_c[args->index].bus_address;

    if (dec_cont->use_video_compressor) {
      /* Compression table info. */
      args->pic.output_rfc_luma_base =
        dec_cont->asic_buff->cbs_luma_table[args->index].virtual_address;
      args->pic.output_rfc_luma_bus_address =
          dec_cont->asic_buff->cbs_luma_table[args->index].bus_address;
      args->pic.output_rfc_chroma_base =
          dec_cont->asic_buff->cbs_chroma_table[args->index].virtual_address;
      args->pic.output_rfc_chroma_bus_address =
          dec_cont->asic_buff->cbs_chroma_table[args->index].bus_address;
    }
  }

  /* Set pixel format */
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN || dec_cont->down_scale_enabled) {
    if (dec_cont->use_p010_output && bit_depth > 8)
      args->pic.pixel_format = DEC_OUT_PIXEL_P010;
    else if (dec_cont->pixel_format == DEC_OUT_PIXEL_CUSTOMER1)
      args->pic.pixel_format = DEC_OUT_PIXEL_CUSTOMER1;
    else if (dec_cont->use_8bits_output)
      args->pic.pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
    else
      args->pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  } else {
    /* Reference buffer. */
    args->pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  }

  /* Finally, set the information we don't know yet to 0. */
  args->pic.nbr_of_err_mbs = 0; /* To be set after decoding. */
  args->pic.pic_id = 0;         /* To be set after output reordering. */
}
#else

void Vp9SetupPicToOutput(struct Vp9DecContainer *dec_cont) {
  struct PicCallbackArg *args = &dec_cont->pic_callback_arg[dec_cont->core_id];
  u32 bit_depth = dec_cont->decoder.bit_depth;
  u32 rs_bit_depth;
  rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                  dec_cont->use_p010_output ? 16 : bit_depth;

  args->index = dec_cont->asic_buff->out_buffer_i;
  args->fifo_out = dec_cont->fifo_out;

  if (dec_cont->decoder.show_existing_frame) {
    args->pic = dec_cont->asic_buff->picture_info[args->index];
    args->pic.is_intra_frame = 0;
    args->show_frame = 1;
    return;
  }
  args->show_frame = dec_cont->decoder.show_frame;
  /* Fill in the picture information for everything we know. */
  args->pic.is_intra_frame = dec_cont->decoder.key_frame;
  args->pic.is_golden_frame = 0;
  /* Frame size and format information. */
  args->pic.frame_width = NEXT_MULTIPLE(dec_cont->width, 8);
  args->pic.frame_height = NEXT_MULTIPLE(dec_cont->height, 8);
  args->pic.coded_width = dec_cont->width;
  args->pic.coded_height = dec_cont->height;
  args->pic.output_format = dec_cont->output_format;
  args->pic.bit_depth_luma = args->pic.bit_depth_chroma = bit_depth;
  args->pic.pic_stride = NEXT_MULTIPLE(args->pic.frame_width * bit_depth, 16 * 8) / 8;
#ifdef DOWN_SCALER
  args->pic.dscale_width = 0;
  args->pic.dscale_height = 0;
  args->pic.dscale_stride = 0;
  args->pic.output_dscale_luma_base = NULL;
  args->pic.output_dscale_luma_bus_address = 0;
  args->pic.output_dscale_chroma_base = NULL;
  args->pic.output_chroma_bus_address = 0;

  if (dec_cont->down_scale_enabled) {
    u32 decoded_width = NEXT_MULTIPLE(dec_cont->width, 8);
    args->pic.dscale_width = (dec_cont->width / 2 >> dec_cont->down_scale_x_shift) << 1;
    args->pic.dscale_height = (dec_cont->height / 2 >> dec_cont->down_scale_y_shift) << 1;
    args->pic.dscale_stride = NEXT_MULTIPLE((decoded_width >> dec_cont->down_scale_x_shift) * rs_bit_depth, 16 * 8) / 8;
    args->pic.output_dscale_luma_base =
        dec_cont->asic_buff->dscale[args->index].virtual_address;
    args->pic.output_dscale_luma_bus_address =
        dec_cont->asic_buff->dscale[args->index].bus_address;
    args->pic.output_dscale_chroma_base =
        dec_cont->asic_buff->dscale[args->index].virtual_address + dec_cont->asic_buff->dscale_c_offset[args->index] / 4;
    args->pic.output_dscale_chroma_bus_address =
        dec_cont->asic_buff->dscale[args->index].bus_address + dec_cont->asic_buff->dscale_c_offset[args->index];
  }
#endif
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    args->pic.pic_stride = NEXT_MULTIPLE(args->pic.frame_width * rs_bit_depth, 16 * 8) / 8;
    args->pic.output_luma_base =
        dec_cont->asic_buff->raster[args->index].virtual_address;
    args->pic.output_luma_bus_address =
        dec_cont->asic_buff->raster[args->index].bus_address;
    args->pic.output_chroma_base =
        dec_cont->asic_buff->raster[args->index].virtual_address + dec_cont->asic_buff->raster_c_offset[args->index] / 4;
    args->pic.output_chroma_bus_address =
        dec_cont->asic_buff->raster[args->index].bus_address + dec_cont->asic_buff->raster_c_offset[args->index];
  } else {
    args->pic.output_luma_base =
        dec_cont->asic_buff->pictures[args->index].virtual_address;
    args->pic.output_luma_bus_address =
        dec_cont->asic_buff->pictures[args->index].bus_address;
    args->pic.output_chroma_base =
        dec_cont->asic_buff->pictures[args->index].virtual_address + dec_cont->asic_buff->pictures_c_offset[args->index] / 4;
    args->pic.output_chroma_bus_address =
        dec_cont->asic_buff->pictures[args->index].bus_address + dec_cont->asic_buff->pictures_c_offset[args->index];

    if (dec_cont->use_video_compressor) {
      /* Compression table info. */
      args->pic.output_rfc_luma_base =
        dec_cont->asic_buff->pictures[args->index].virtual_address + dec_cont->asic_buff->cbs_y_tbl_offset [args->index] / 4;
      args->pic.output_rfc_luma_bus_address =
          dec_cont->asic_buff->pictures[args->index].bus_address + dec_cont->asic_buff->cbs_y_tbl_offset [args->index];
      args->pic.output_rfc_chroma_base =
          dec_cont->asic_buff->pictures[args->index].virtual_address + dec_cont->asic_buff->cbs_c_tbl_offset [args->index] / 4;
      args->pic.output_rfc_chroma_bus_address =
          dec_cont->asic_buff->pictures[args->index].bus_address + dec_cont->asic_buff->cbs_c_tbl_offset [args->index];
    }
  }

  /* Set pixel format */
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN || dec_cont->down_scale_enabled) {
    if (dec_cont->use_p010_output && bit_depth > 8)
      args->pic.pixel_format = DEC_OUT_PIXEL_P010;
    else if (dec_cont->pixel_format == DEC_OUT_PIXEL_CUSTOMER1)
      args->pic.pixel_format = DEC_OUT_PIXEL_CUSTOMER1;
    else if (dec_cont->use_8bits_output)
      args->pic.pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
    else
      args->pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  } else {
    /* Reference buffer. */
    args->pic.pixel_format = DEC_OUT_PIXEL_DEFAULT;
  }

  /* Finally, set the information we don't know yet to 0. */
  args->pic.nbr_of_err_mbs = 0; /* To be set after decoding. */
  args->pic.pic_id = 0;         /* To be set after output reordering. */
}
#endif

i32 Vp9ProcessAsicStatus(struct Vp9DecContainer *dec_cont, u32 asic_status,
                         u32 *error_concealment) {
  /* Handle system error situations */
  if (asic_status == VP9HWDEC_SYSTEM_TIMEOUT) {
    /* This timeout is DWL(software/os) generated */
    return DEC_HW_TIMEOUT;
  } else if (asic_status == VP9HWDEC_SYSTEM_ERROR) {
    return DEC_SYSTEM_ERROR;
  } else if (asic_status == VP9HWDEC_HW_RESERVED) {
    return DEC_HW_RESERVED;
  }

  /* Handle possible common HW error situations */
  if (asic_status & DEC_HW_IRQ_BUS) {
    return DEC_HW_BUS_ERROR;
  }

  /* for all the rest we will output a picture (concealed or not) */
  if ((asic_status & DEC_HW_IRQ_TIMEOUT) || (asic_status & DEC_HW_IRQ_ERROR) ||
      (asic_status & DEC_HW_IRQ_ASO) /* to signal lost residual */) {
    /* This timeout is HW generated */
    if (asic_status & DEC_HW_IRQ_TIMEOUT) {
#ifdef VP9HWTIMEOUT_ASSERT
      ASSERT(0);
#endif
      DEBUG_PRINT(("IRQ: HW TIMEOUT\n"));
    } else {
      DEBUG_PRINT(("IRQ: STREAM ERROR\n"));
    }

    /* normal picture freeze */
    *error_concealment = 1;
  } else if (asic_status & DEC_HW_IRQ_RDY) {
    DEBUG_PRINT(("IRQ: PICTURE RDY\n"));

    if (dec_cont->decoder.key_frame) {
      dec_cont->picture_broken = 0;
      dec_cont->force_intra_freeze = 0;
    }
  } else {
    ASSERT(0);
  }

  return DEC_OK;
}

i32 VP9SyncAndOutput(struct Vp9DecContainer *dec_cont) {
  i32 ret = 0;
  u32 asic_status;
  u32 error_concealment = 0;
  /* aliases */
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  /* If hw was running, sync with hw and output picture */
  if (dec_cont->asic_running) {
    asic_status = Vp9AsicSync(dec_cont);

    /* Handle asic return status */
    ret = Vp9ProcessAsicStatus(dec_cont, asic_status, &error_concealment);
    if (ret) return ret;

    /* Adapt probabilities */
    /* TODO should this be done after error handling? */
    Vp9UpdateProbabilities(dec_cont);
    /* Update reference frame flags */
    Vp9UpdateRefs(dec_cont, error_concealment);

    /* Store prev out info */
    if (!error_concealment || dec_cont->intra_only || dec_cont->pic_number==1) {
      if (error_concealment) Vp9ConstantConcealment(dec_cont, 128);
      asic_buff->prev_out_buffer_i = asic_buff->out_buffer_i;

      Vp9PicToOutput(dec_cont);
    } else {
      dec_cont->picture_broken = 1;
    }
    asic_buff->out_buffer_i = VP9_UNDEFINED_BUFFER;
  }
  return ret;
}

#ifndef USE_EXTERNAL_BUFFER
void Vp9ConstantConcealment(struct Vp9DecContainer *dec_cont, u8 pixel_value) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  i32 index = asic_buff->out_buffer_i;

  dec_cont->picture_broken = 1;
  DWLPrivateAreaMemset(asic_buff->pictures[index].virtual_address, pixel_value,
            asic_buff->pictures[index].size);
  DWLPrivateAreaMemset(asic_buff->pictures_c[index].virtual_address, pixel_value,
            asic_buff->pictures_c[index].size);
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    DWLPrivateAreaMemset(asic_buff->raster_luma[index].virtual_address, pixel_value,
              asic_buff->raster_luma[index].size);
    DWLPrivateAreaMemset(asic_buff->raster_chroma[index].virtual_address, pixel_value,
              asic_buff->raster_chroma[index].size);
  }
}
#else
void Vp9ConstantConcealment(struct Vp9DecContainer *dec_cont, u8 pixel_value) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  i32 index = asic_buff->out_buffer_i;

  dec_cont->picture_broken = 1;
  // Size of picture (luma & chroma) is just the offset of dir mv,
  // which is stored next to picture buffer.
  DWLPrivateAreaMemset(asic_buff->pictures[index].virtual_address, pixel_value,
            asic_buff->dir_mvs_offset[index]);

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    DWLPrivateAreaMemset(asic_buff->raster[index].virtual_address, pixel_value,
              asic_buff->raster[index].size);
  }
}
#endif

