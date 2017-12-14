/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : ...
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "regdrv.h"
#include "vp9hwd_asic.h"
#include "vp9hwd_container.h"
#include "vp9hwd_output.h"
#include "vp9hwd_probs.h"
#include "commonconfig.h"
#include "vp9_entropymv.h"

#define MAX_TILE_COLS 20
#define MAX_TILE_ROWS 22

#define DEC_MODE_VP9 13

#ifdef SET_EMPTY_PICTURE_DATA /* USE THIS ONLY FOR DEBUGGING PURPOSES */
static void Vp9SetEmptyPictureData(struct Vp9DecContainer *dec_cont);
#endif
static i32 Vp9MallocRefFrm(struct Vp9DecContainer *dec_cont, u32 index);
#ifndef USE_EXTERNAL_BUFFER
static i32 Vp9FreeRefFrm(struct Vp9DecContainer *dec_cont, u32 index);
#endif
static i32 Vp9AllocateSegmentMap(struct Vp9DecContainer *dec_cont);
static i32 Vp9FreeSegmentMap(struct Vp9DecContainer *dec_cont);
static i32 Vp9ReallocateSegmentMap(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetTileInfo(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetReferenceFrames(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetSegmentation(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetLoopFilter(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetPictureDimensions(struct Vp9DecContainer *dec_cont);
static void Vp9AsicSetOutput(struct Vp9DecContainer *dec_cont);
static u32 RequiredBufferCount(struct Vp9DecContainer *dec_cont);

u32 RequiredBufferCount(struct Vp9DecContainer *dec_cont) {
  return (Vp9BufferQueueCountReferencedBuffers(dec_cont->bq) + 2);
}

void Vp9AsicInit(struct Vp9DecContainer *dec_cont) {

  DWLmemset(dec_cont->vp9_regs, 0, sizeof(dec_cont->vp9_regs));

  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_MODE, DEC_MODE_VP9);

  SetCommonConfigRegs(dec_cont->vp9_regs);
  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_OUT_EC_BYPASS, dec_cont->use_video_compressor ? 0 : 1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_APF_ONE_PID, dec_cont->use_fetch_one_pic? 1 : 0);
}

#ifndef USE_EXTERNAL_BUFFER
i32 Vp9AsicAllocateMem(struct Vp9DecContainer *dec_cont) {

  const void *dwl = dec_cont->dwl;
  i32 dwl_ret;
  u32 size;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  size = sizeof(struct Vp9EntropyProbs);
  dwl_ret = DWLMallocLinear(dwl, size, &asic_buff->prob_tbl);
  if (dwl_ret != DWL_OK) goto err;

  size = sizeof(struct Vp9EntropyCounts);
  dwl_ret = DWLMallocLinear(dwl, size, &asic_buff->ctx_counters);
  if (dwl_ret != DWL_OK) goto err;

  /* max number of tiles times width and height (2 bytes each),
   * rounding up to next 16 bytes boundary + one extra 16 byte
   * chunk (HW guys wanted to have this) */
  size = (MAX_TILE_COLS * MAX_TILE_ROWS * 4 * sizeof(u16) + 15 + 16) & ~0xF;
  dwl_ret = DWLMallocLinear(dwl, size, &asic_buff->tile_info);
  if (dwl_ret != DWL_OK) goto err;

  DWLmemset(asic_buff->tile_info.virtual_address, 0, asic_buff->tile_info.size);

  return 0;
err:
  Vp9AsicReleaseMem(dec_cont);
  return -1;
}

void Vp9AsicReleaseMem(struct Vp9DecContainer *dec_cont) {
  const void *dwl = dec_cont->dwl;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (asic_buff->prob_tbl.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->prob_tbl);
    DWLmemset(&asic_buff->prob_tbl, 0, sizeof(asic_buff->prob_tbl));
  }
  if (asic_buff->ctx_counters.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->ctx_counters);
    DWLmemset(&asic_buff->ctx_counters, 0, sizeof(asic_buff->ctx_counters));
  }
  if (asic_buff->tile_info.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->tile_info);
    asic_buff->tile_info.virtual_address = NULL;
  }
}

/* Allocate filter memories that are dependent on tile structure and height. */
i32 Vp9AsicAllocateFilterBlockMem(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_tile_cols = 1 << dec_cont->decoder.log2_tile_columns;
  u32 pixel_width = dec_cont->decoder.bit_depth;
  if (num_tile_cols < 2) return HANTRO_OK;

  u32 height32 = NEXT_MULTIPLE(asic_buff->height, 64);
  u32 size = 8 * height32 * (num_tile_cols - 1);    /* Luma filter mem */
  size += (8 + 8) * height32 * (num_tile_cols - 1); /* Chroma */
  size = size * pixel_width / 8;
  if (asic_buff->filter_mem.logical_size >= size) return HANTRO_OK;

  /* If already allocated, release the old, too small buffers. */
  Vp9AsicReleaseFilterBlockMem(dec_cont);

  i32 dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->filter_mem);
  if (dwl_ret != DWL_OK) {
    Vp9AsicReleaseFilterBlockMem(dec_cont);
    return HANTRO_NOK;
  }

  size = 16 * (height32 / 4) * (num_tile_cols - 1);
  dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->bsd_control_mem);
  if (dwl_ret != DWL_OK) {
    Vp9AsicReleaseFilterBlockMem(dec_cont);
    return HANTRO_NOK;
  }
  return HANTRO_OK;
}

void Vp9AsicReleaseFilterBlockMem(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  if (asic_buff->filter_mem.virtual_address != NULL) {
    DWLFreeLinear(dec_cont->dwl, &asic_buff->filter_mem);
    asic_buff->filter_mem.virtual_address = NULL;
    asic_buff->filter_mem.size = 0;
  }
  if (asic_buff->bsd_control_mem.virtual_address != NULL) {
    DWLFreeLinear(dec_cont->dwl, &asic_buff->bsd_control_mem);
    asic_buff->bsd_control_mem.virtual_address = NULL;
    asic_buff->bsd_control_mem.size = 0;
  }
}

i32 Vp9AsicAllocatePictures(struct Vp9DecContainer *dec_cont) {
  u32 i;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  dec_cont->active_segment_map = 0;
  Vp9AllocateSegmentMap(dec_cont);

  DWLmemset(asic_buff->pictures, 0, sizeof(asic_buff->pictures));

  dec_cont->num_buffers += dec_cont->n_extra_frm_buffers;
  dec_cont->n_extra_frm_buffers = 0;
  if (dec_cont->num_buffers > VP9DEC_MAX_PIC_BUFFERS)
    dec_cont->num_buffers = VP9DEC_MAX_PIC_BUFFERS;

  dec_cont->bq = Vp9BufferQueueInitialize(dec_cont->num_buffers);
  if (dec_cont->bq == NULL) {
    Vp9AsicReleasePictures(dec_cont);
    return -1;
  }

  for (i = 0; i < dec_cont->num_buffers; i++) {
    if (Vp9MallocRefFrm(dec_cont, i)) {
      Vp9AsicReleasePictures(dec_cont);
      return -1;
    }
  }

  ASSERT(asic_buff->width / 4 < 0x1FFF);
  ASSERT(asic_buff->height / 4 < 0x1FFF);
  SetDecRegister(dec_cont->vp9_regs, HWIF_MAX_CB_SIZE, 6); /* 64x64 */
  SetDecRegister(dec_cont->vp9_regs, HWIF_MIN_CB_SIZE, 3); /* 8x8 */

  asic_buff->out_buffer_i = -1;

  return 0;
}

void Vp9AsicReleasePictures(struct Vp9DecContainer *dec_cont) {
  u32 i;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  for (i = 0; i < dec_cont->num_buffers; i++) {
    Vp9FreeRefFrm(dec_cont, i);
  }

  if (dec_cont->bq) {
    Vp9BufferQueueRelease(dec_cont->bq);
    dec_cont->bq = NULL;
  }

  DWLmemset(asic_buff->pictures, 0, sizeof(asic_buff->pictures));

  Vp9FreeSegmentMap(dec_cont);
}

i32 Vp9AllocateFrame(struct Vp9DecContainer *dec_cont, u32 index) {
  if (Vp9MallocRefFrm(dec_cont, index)) return HANTRO_NOK;

  dec_cont->num_buffers++;
  Vp9BufferQueueAddBuffer(dec_cont->bq);

  return HANTRO_OK;
}

i32 Vp9ReallocateFrame(struct Vp9DecContainer *dec_cont, u32 index) {
  i32 ret = HANTRO_OK;
  pthread_mutex_lock(&dec_cont->sync_out);
  while (dec_cont->asic_buff->display_index[index])
      pthread_cond_wait(&dec_cont->sync_out_cv, &dec_cont->sync_out);
  Vp9FreeRefFrm(dec_cont, index);
  ret = Vp9MallocRefFrm(dec_cont, index);
  pthread_mutex_unlock(&dec_cont->sync_out);

  return ret;
}

i32 Vp9MallocRefFrm(struct Vp9DecContainer *dec_cont, u32 index) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_ctbs, luma_size, chroma_size, dir_mvs_size;
  u32 raster_luma_size, raster_chroma_size;
  i32 dwl_ret;
#ifdef DOWN_SCALER
  u32 dscale_luma_size, dscale_chroma_size;
#endif
  u32 pic_width_in_cbsy, pic_height_in_cbsy;
  u32 pic_width_in_cbsc, pic_height_in_cbsc;
  u32 luma_table_size, chroma_table_size;
  u32 bit_depth, rs_bit_depth;
  struct DWLLinearMem empty = {0};

  bit_depth = dec_cont->decoder.bit_depth;
  rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                  dec_cont->use_p010_output ? 16 : bit_depth;

  luma_size = asic_buff->height * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  chroma_size = (asic_buff->height / 2) * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  dir_mvs_size = num_ctbs * 64 * 16; /* MVs (16 MBs / CTB * 16 bytes / MB) */
  raster_luma_size = NEXT_MULTIPLE(asic_buff->width * rs_bit_depth, 16 * 8) / 8 * asic_buff->height;
  raster_chroma_size = raster_luma_size / 2;

#ifdef DOWN_SCALER
  dscale_luma_size = NEXT_MULTIPLE((asic_buff->width >> dec_cont->down_scale_x_shift) * rs_bit_depth, 16 * 8) / 8 *
                     (asic_buff->height >> dec_cont->down_scale_y_shift);
  dscale_chroma_size = NEXT_MULTIPLE(dscale_luma_size / 2, 16);
#endif

  if (dec_cont->use_video_compressor) {
    pic_width_in_cbsy = (asic_buff->width + 8 - 1)/8;
    pic_width_in_cbsy = NEXT_MULTIPLE(pic_width_in_cbsy, 16);
    pic_width_in_cbsc = (asic_buff->width + 16 - 1)/16;
    pic_width_in_cbsc = NEXT_MULTIPLE(pic_width_in_cbsc, 16);
    pic_height_in_cbsy = (asic_buff->height + 8 - 1)/8;
    pic_height_in_cbsc = (asic_buff->height/2 + 4 - 1)/4;

    /* luma table size */
    luma_table_size = NEXT_MULTIPLE(pic_width_in_cbsy * pic_height_in_cbsy, 16);
    /* chroma table size */
    chroma_table_size = NEXT_MULTIPLE(pic_width_in_cbsc * pic_height_in_cbsc, 16);
  } else {
    luma_table_size = chroma_table_size = 0;
  }

  /* luma */
  dwl_ret =
      DWLMallocRefFrm(dec_cont->dwl, luma_size, &asic_buff->pictures[index]);
  /* chroma */
  dwl_ret |= DWLMallocRefFrm(dec_cont->dwl, chroma_size,
                             &asic_buff->pictures_c[index]);
  /* colocated mvs */
  dwl_ret |=
      DWLMallocRefFrm(dec_cont->dwl, dir_mvs_size, &asic_buff->dir_mvs[index]);
  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    /* raster luma */
    dwl_ret |= DWLMallocLinear(dec_cont->dwl, raster_luma_size,
                               &asic_buff->raster_luma[index]);
    /* raster chroma */
    dwl_ret |= DWLMallocLinear(dec_cont->dwl, raster_chroma_size,
                               &asic_buff->raster_chroma[index]);
  } else {
    asic_buff->raster_luma[index] = empty;
    asic_buff->raster_chroma[index] = empty;
  }

#ifdef DOWN_SCALER
    if (dec_cont->down_scale_enabled) {
      /* raster luma */
      dwl_ret |= DWLMallocLinear(dec_cont->dwl, dscale_luma_size,
                                 &asic_buff->dscale_luma[index]);
      /* raster chroma */
      dwl_ret |= DWLMallocLinear(dec_cont->dwl, dscale_chroma_size,
                                &asic_buff->dscale_chroma[index]);
    } else {
      asic_buff->dscale_luma[index] = empty;
      asic_buff->dscale_chroma[index] = empty;
    }
#endif

  if (dec_cont->use_video_compressor) {
    /* luma */
    dwl_ret |= DWLMallocRefFrm(dec_cont->dwl, luma_table_size,
                              &asic_buff->cbs_luma_table[index]);
    /* chroma */
    dwl_ret |= DWLMallocRefFrm(dec_cont->dwl, chroma_table_size,
                              &asic_buff->cbs_chroma_table[index]);
  }

  if (dwl_ret != DWL_OK) {
    //Vp9AsicReleasePictures(dec_cont);
    return HANTRO_NOK;
  }

  return HANTRO_OK;
}

i32 Vp9FreeRefFrm(struct Vp9DecContainer *dec_cont, u32 index) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (asic_buff->pictures[index].virtual_address != NULL)
    DWLFreeRefFrm(dec_cont->dwl, &asic_buff->pictures[index]);
  if (asic_buff->pictures_c[index].virtual_address != NULL)
    DWLFreeRefFrm(dec_cont->dwl, &asic_buff->pictures_c[index]);
  if (asic_buff->dir_mvs[index].virtual_address != NULL)
    DWLFreeRefFrm(dec_cont->dwl, &asic_buff->dir_mvs[index]);
  if (asic_buff->raster_luma[index].virtual_address != NULL)
    DWLFreeLinear(dec_cont->dwl, &asic_buff->raster_luma[index]);
  if (asic_buff->raster_chroma[index].virtual_address != NULL)
    DWLFreeLinear(dec_cont->dwl, &asic_buff->raster_chroma[index]);
#ifdef DOWN_SCALER
  if (asic_buff->dscale_luma[index].virtual_address != NULL)
    DWLFreeLinear(dec_cont->dwl, &asic_buff->dscale_luma[index]);
  if (asic_buff->dscale_chroma[index].virtual_address != NULL)
    DWLFreeLinear(dec_cont->dwl, &asic_buff->dscale_chroma[index]);
#endif

  if (dec_cont->use_video_compressor) {
    if (asic_buff->cbs_luma_table[index].virtual_address != NULL)
      DWLFreeRefFrm(dec_cont->dwl, &asic_buff->cbs_luma_table[index]);
    if (asic_buff->cbs_chroma_table[index].virtual_address != NULL)
      DWLFreeRefFrm(dec_cont->dwl, &asic_buff->cbs_chroma_table[index]);
  }
  return HANTRO_OK;
}

i32 Vp9GetRefFrm(struct Vp9DecContainer *dec_cont) {
  u32 limit = dec_cont->dynamic_buffer_limit;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_sbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  u32 bit_depth = dec_cont->decoder.bit_depth;
  u32 rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                 (dec_cont->use_p010_output ? 16 : bit_depth);
  u32 raster_luma_size = NEXT_MULTIPLE(asic_buff->width * rs_bit_depth, 16 * 8) / 8 * asic_buff->height;
  if (RequiredBufferCount(dec_cont) < limit)
    limit = RequiredBufferCount(dec_cont);

  asic_buff->out_buffer_i = Vp9BufferQueueGetBuffer(dec_cont->bq, limit);
  if (asic_buff->out_buffer_i < 0) {
    if (Vp9AllocateFrame(dec_cont, dec_cont->num_buffers)) return DEC_MEMFAIL;
    asic_buff->out_buffer_i = Vp9BufferQueueGetBuffer(dec_cont->bq, limit);
  }

  /* Reallocate picture memories if needed */
  if (asic_buff->pictures[asic_buff->out_buffer_i].logical_size <
      (NEXT_MULTIPLE(asic_buff->width * dec_cont->decoder.bit_depth, 16 * 8) / 8 * asic_buff->height) ||
      asic_buff->dir_mvs[asic_buff->out_buffer_i].logical_size <
        num_sbs * 64 * 16 ||
      (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN &&
       asic_buff->raster_luma[asic_buff->out_buffer_i].logical_size <
         raster_luma_size)) {
    /* Reallocate bigger picture buffer into current index */
    if (Vp9ReallocateFrame(dec_cont, asic_buff->out_buffer_i))
      return DEC_MEMFAIL;
  }

  if (Vp9ReallocateSegmentMap(dec_cont)) return DEC_MEMFAIL;

  return HANTRO_OK;
}

/* Reallocates segment maps if resolution changes bigger than initial
   resolution. Needs synchronization if SW is running parallel with HW */
i32 Vp9ReallocateSegmentMap(struct Vp9DecContainer *dec_cont) {
  i32 dwl_ret;
  u32 num_ctbs, memory_size;
  const void *dwl = dec_cont->dwl;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  struct DWLLinearMem segment_map[2];
  struct Vp9Decoder *dec = &dec_cont->decoder;

  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  memory_size = num_ctbs * 32; /* Segment map uses 32 bytes / CTB */

  if (dec->key_frame || dec->resolution_change) {
    for (u32 i = 0; i < 2; i++) {
      struct DWLLinearMem mem = asic_buff->segment_map[i];
      if (mem.virtual_address) DWLmemset(mem.virtual_address, 0, mem.size);
    }
  }

  /* Do nothing if we have big enough buffers for segment maps */
  if (memory_size <= asic_buff->segment_map_size) return HANTRO_OK;

  /* Allocate new segment maps for larger resolution and initialize them to zero. */
  dwl_ret = DWLMallocLinear(dwl, memory_size, &segment_map[0]);
  dwl_ret |= DWLMallocLinear(dwl, memory_size, &segment_map[1]);
  if (dwl_ret != DWL_OK) {
    Vp9AsicReleasePictures(dec_cont);
    return HANTRO_NOK;
  }
  DWLmemset(segment_map[0].virtual_address, 0, memory_size);
  DWLmemset(segment_map[1].virtual_address, 0, memory_size);
  /* Copy existing segment maps into new buffers */
  DWLmemcpy(segment_map[0].virtual_address,
            asic_buff->segment_map[0].virtual_address,
            asic_buff->segment_map[0].size);
  DWLmemcpy(segment_map[1].virtual_address,
            asic_buff->segment_map[1].virtual_address,
            asic_buff->segment_map[1].size);
  /* Free old segment maps */
  Vp9FreeSegmentMap(dec_cont);

  asic_buff->segment_map_size = memory_size;
  asic_buff->segment_map[0] = segment_map[0];
  asic_buff->segment_map[1] = segment_map[1];

  return HANTRO_OK;
}

i32 Vp9AllocateSegmentMap(struct Vp9DecContainer *dec_cont) {
  i32 dwl_ret;
  u32 num_ctbs, memory_size;
  const void *dwl = dec_cont->dwl;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  memory_size = num_ctbs * 32; /* Segment map uses 32 bytes / CTB */

  dwl_ret = DWLMallocLinear(dwl, memory_size, &asic_buff->segment_map[0]);
  dwl_ret |= DWLMallocLinear(dwl, memory_size, &asic_buff->segment_map[1]);
  if (dwl_ret != DWL_OK) {
    Vp9AsicReleasePictures(dec_cont);
    return HANTRO_NOK;
  }

  asic_buff->segment_map_size = memory_size;
  DWLmemset(asic_buff->segment_map[0].virtual_address, 0,
            asic_buff->segment_map_size);
  DWLmemset(asic_buff->segment_map[1].virtual_address, 0,
            asic_buff->segment_map_size);

  return HANTRO_OK;
}

i32 Vp9FreeSegmentMap(struct Vp9DecContainer *dec_cont) {
  u32 i;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  for (i = 0; i < 2; i++) {
    if (asic_buff->segment_map[i].virtual_address != NULL) {
      DWLFreeLinear(dec_cont->dwl, &asic_buff->segment_map[i]);
    }
  }
  DWLmemset(asic_buff->segment_map, 0, sizeof(asic_buff->segment_map));

  return HANTRO_OK;
}

void Vp9UpdateProbabilities(struct Vp9DecContainer *dec_cont) {
  /* Read context counters from HW output memory. */
  DWLmemcpy(&dec_cont->decoder.ctx_ctr,
            dec_cont->asic_buff->ctx_counters.virtual_address,
            sizeof(struct Vp9EntropyCounts));

  /* Backward adaptation of probs based on context counters. */
  if (!dec_cont->decoder.error_resilient &&
      !dec_cont->decoder.frame_parallel_decoding) {
    Vp9AdaptCoefProbs(&dec_cont->decoder);
    if (!dec_cont->decoder.key_frame && !dec_cont->decoder.intra_only) {
      Vp9AdaptModeProbs(&dec_cont->decoder);
      Vp9AdaptModeContext(&dec_cont->decoder);
      Vp9AdaptNmvProbs(&dec_cont->decoder);
    }
  }

  /* Store the adapted probs as base for following frames. */
  Vp9StoreProbs(&dec_cont->decoder);
}
#else

i32 Vp9AsicAllocateMem(struct Vp9DecContainer *dec_cont) {

//  const void *dwl = dec_cont->dwl;
//  i32 dwl_ret;
  u32 size;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

#if 0
  if (asic_buff->prob_tbl.virtual_address == NULL) {
    dec_cont->next_buf_size = sizeof(struct Vp9EntropyProbs);
    return DEC_WAITING_FOR_BUFFER;
  }

  if (asic_buff->ctx_counters.virtual_address == NULL) {
    dec_cont->next_buf_size = sizeof(struct Vp9EntropyCounts);
    return DEC_WAITING_FOR_BUFFER;
  }

  /* max number of tiles times width and height (2 bytes each),
   * rounding up to next 16 bytes boundary + one extra 16 byte
   * chunk (HW guys wanted to have this) */
  if (asic_buff->tile_info.virtual_address == NULL) {
    dec_cont->next_buf_size = (MAX_TILE_COLS * MAX_TILE_ROWS * 2 * sizeof(u16) + 15 + 16) & ~0xF;
    return DEC_WAITING_FOR_BUFFER;
  }
#else
  asic_buff->prob_tbl_offset = 0;
  asic_buff->ctx_counters_offset = asic_buff->prob_tbl_offset
                                 + NEXT_MULTIPLE(sizeof(struct Vp9EntropyProbs), 16);
  asic_buff->tile_info_offset = asic_buff->ctx_counters_offset
                                 + NEXT_MULTIPLE(sizeof(struct Vp9EntropyCounts), 16);

  size = NEXT_MULTIPLE(sizeof(struct Vp9EntropyProbs), 16)
       + NEXT_MULTIPLE(sizeof(struct Vp9EntropyCounts), 16)
       + NEXT_MULTIPLE((MAX_TILE_COLS * MAX_TILE_ROWS * 4 * sizeof(u16) + 15 + 16) & ~0xF, 16);

  if (asic_buff->misc_linear.virtual_address == NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, MISC_LINEAR_BUFFER)) {
      dec_cont->next_buf_size = size;
      dec_cont->buf_to_free = NULL;
      dec_cont->buf_type = MISC_LINEAR_BUFFER;
      dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      i32 ret = 0;
      ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->misc_linear);

      if (ret) return DEC_MEMFAIL;
    }
  }
#endif

  return DEC_OK;
}

i32 Vp9AsicReleaseMem(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (asic_buff->misc_linear.virtual_address != NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, MISC_LINEAR_BUFFER)) {
      dec_cont->buf_to_free = &asic_buff->misc_linear;
      dec_cont->next_buf_size = 0;
      return DEC_WAITING_FOR_BUFFER;
    } else {
      DWLFreeLinear(dec_cont->dwl, &asic_buff->misc_linear);
    }
  }

  return DWL_OK;
}

/* Allocate filter memories that are dependent on tile structure and height. */
i32 Vp9AsicAllocateFilterBlockMem(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_tile_cols = 1 << dec_cont->decoder.log2_tile_columns;
  u32 pixel_width = dec_cont->decoder.bit_depth;
  i32 dwl_ret;
  if (num_tile_cols < 2) return HANTRO_OK;

  u32 height32 = NEXT_MULTIPLE(asic_buff->height, 64);
  u32 size = 8 * height32 * (num_tile_cols - 1);    /* Luma filter mem */
  size += (8 + 8) * height32 * (num_tile_cols - 1); /* Chroma */
  size = size * pixel_width / 8;
  asic_buff->filter_mem_offset = 0;
  asic_buff->bsd_control_mem_offset = size;
  size += 16 * (height32 / 4) * (num_tile_cols - 1);

  if (asic_buff->tile_edge.logical_size >= size) return HANTRO_OK;

  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, TILE_EDGE_BUFFER)) {
    // Release old buffer.
    if (asic_buff->tile_edge.virtual_address != NULL) {
      dec_cont->buf_to_free = &asic_buff->tile_edge;
      dwl_ret = DEC_WAITING_FOR_BUFFER;
    } else {
      dec_cont->buf_to_free = NULL;
      dwl_ret = HANTRO_OK;
    }

    if (asic_buff->tile_edge.virtual_address == NULL) {
      dec_cont->next_buf_size = size;
      dec_cont->buf_type = TILE_EDGE_BUFFER;
      dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      asic_buff->realloc_tile_edge_mem = 1;
      return DEC_WAITING_FOR_BUFFER;
    }
  } else {
    /* If already allocated, release the old, too small buffers. */
    Vp9AsicReleaseFilterBlockMem(dec_cont);

    i32 dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->tile_edge);
    if (dwl_ret != DWL_OK) {
      Vp9AsicReleaseFilterBlockMem(dec_cont);
      return HANTRO_NOK;
    }
  }

  return HANTRO_OK;
}

i32 Vp9AsicReleaseFilterBlockMem(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, TILE_EDGE_BUFFER)) {
    if (asic_buff->tile_edge.virtual_address != NULL) {
      DWLFreeLinear(dec_cont->dwl, &asic_buff->tile_edge);
      asic_buff->tile_edge.virtual_address = NULL;
      asic_buff->tile_edge.size = 0;
    }
  }

  return DWL_OK;
}

i32 Vp9AsicAllocatePictures(struct Vp9DecContainer *dec_cont) {
  u32 i;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  dec_cont->active_segment_map = 0;
  if (Vp9AllocateSegmentMap(dec_cont) != HANTRO_OK)
    return DEC_WAITING_FOR_BUFFER;

  dec_cont->num_buffers += dec_cont->n_extra_frm_buffers;
  dec_cont->n_extra_frm_buffers = 0;
  if (dec_cont->num_buffers > VP9DEC_MAX_PIC_BUFFERS)
    dec_cont->num_buffers = VP9DEC_MAX_PIC_BUFFERS;

  /* Require external picutre buffers. */
  for (i = 0; i < dec_cont->num_buffers; i++) {
    if (Vp9MallocRefFrm(dec_cont, i) != HANTRO_OK)
      return DEC_WAITING_FOR_BUFFER;
  }

  //DWLmemset(asic_buff->pictures, 0, sizeof(asic_buff->pictures));

  dec_cont->bq = Vp9BufferQueueInitialize(dec_cont->num_buffers);
  if (dec_cont->bq == NULL) {
    Vp9AsicReleasePictures(dec_cont);
    return -1;
  }

  ASSERT(asic_buff->width / 4 < 0x1FFF);
  ASSERT(asic_buff->height / 4 < 0x1FFF);
  SetDecRegister(dec_cont->vp9_regs, HWIF_MAX_CB_SIZE, 6); /* 64x64 */
  SetDecRegister(dec_cont->vp9_regs, HWIF_MIN_CB_SIZE, 3); /* 8x8 */

  asic_buff->out_buffer_i = -1;

  return HANTRO_OK;

}

void Vp9AsicReleasePictures(struct Vp9DecContainer *dec_cont) {
  u32 i;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  for (i = 0; i < dec_cont->num_buffers; i++) {
    if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER) &&
        asic_buff->pictures[i].virtual_address != NULL)
      DWLFreeRefFrm(dec_cont->dwl, &asic_buff->pictures[i]);
    if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER) &&
        asic_buff->raster[i].virtual_address != NULL)
      DWLFreeLinear(dec_cont->dwl, &asic_buff->raster[i]);
#ifdef DOWN_SCALER
    if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER) &&
        asic_buff->dscale[i].virtual_address != NULL)
      DWLFreeLinear(dec_cont->dwl, &asic_buff->dscale[i]);
#endif
  }


  if (dec_cont->bq) {
    Vp9BufferQueueRelease(dec_cont->bq);
    dec_cont->bq = NULL;
  }

  DWLmemset(asic_buff->pictures, 0, sizeof(asic_buff->pictures));

  Vp9FreeSegmentMap(dec_cont);
}

i32 Vp9AllocateFrame(struct Vp9DecContainer *dec_cont, u32 index) {
  i32 ret = HANTRO_OK;

  if (Vp9MallocRefFrm(dec_cont, index)) ret = HANTRO_NOK;

  /* Following code will be excuted in Vp9DecAddBuffer(...) when the frame buffer is allocated externally. (bottom half) */
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
    dec_cont->num_buffers++;
    Vp9BufferQueueAddBuffer(dec_cont->bq);
  }

  return ret;
}

i32 Vp9ReallocateFrame(struct Vp9DecContainer *dec_cont, u32 index) {
  i32 ret = HANTRO_OK;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  pthread_mutex_lock(&dec_cont->sync_out);
  while (dec_cont->asic_buff->display_index[index])
      pthread_cond_wait(&dec_cont->sync_out_cv, &dec_cont->sync_out);

  /* Reallocate larger picture buffer into current index */
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
    if (asic_buff->pictures[index].virtual_address != NULL)
      DWLFreeRefFrm(dec_cont->dwl, &asic_buff->pictures[index]);
    ret |= DWLMallocRefFrm(dec_cont->dwl, asic_buff->picture_size, &asic_buff->pictures[index]);
  }
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
    if (asic_buff->raster[index].virtual_address != NULL)
      DWLFreeLinear(dec_cont->dwl, &asic_buff->raster[index]);
    if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN)
      ret |= DWLMallocLinear(dec_cont->dwl, asic_buff->raster_size, &asic_buff->raster[index]);
  }
#ifdef DOWN_SCALER
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
    if (asic_buff->dscale[index].virtual_address != NULL)
      DWLFreeLinear(dec_cont->dwl, &asic_buff->dscale[index]);
    if (dec_cont->down_scale_enabled)
      ret |= DWLMallocLinear(dec_cont->dwl, asic_buff->dscale_size, &asic_buff->dscale[index]);
  }
#endif

  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER) ||
#ifdef DOWN_SCALER
      IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER) ||
#endif
      IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
    // All the external buffer free/malloc will be done in bottom half in AddBuffer()...
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
      dec_cont->buf_to_free = &asic_buff->pictures[index];
      dec_cont->next_buf_size = asic_buff->picture_size;
      dec_cont->buf_type = REFERENCE_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 1;
#endif
    } else if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
      dec_cont->buf_to_free = &asic_buff->raster[index];
      dec_cont->next_buf_size = asic_buff->raster_size;
      dec_cont->buf_type = RASTERSCAN_OUT_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    }
#ifdef DOWN_SCALER
    else if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
      dec_cont->buf_to_free = &asic_buff->dscale[index];
      dec_cont->next_buf_size = asic_buff->dscale_size;
      dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    }
#endif
    asic_buff->realloc_out_buffer = 1;
    dec_cont->buf_num = 1;
    dec_cont->buffer_index = asic_buff->out_buffer_i;

    pthread_mutex_unlock(&dec_cont->sync_out);
    return DEC_WAITING_FOR_BUFFER;
  }

  pthread_mutex_unlock(&dec_cont->sync_out);

  return ret;
}

i32 Vp9MallocRefFrm(struct Vp9DecContainer *dec_cont, u32 index) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_ctbs, luma_size, chroma_size, dir_mvs_size;
  u32 raster_luma_size, raster_chroma_size;
  u32 dscale_luma_size, dscale_chroma_size;
  u32 pic_width_in_cbsy, pic_height_in_cbsy;
  u32 pic_width_in_cbsc, pic_height_in_cbsc;
  u32 luma_table_size, chroma_table_size;
  u32 bit_depth, rs_bit_depth;
  i32 dwl_ret = DWL_OK;

  bit_depth = dec_cont->decoder.bit_depth;
  rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                 (dec_cont->use_p010_output) ? 16 : bit_depth;

  luma_size = asic_buff->height * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  chroma_size = (asic_buff->height / 2) * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  dir_mvs_size = num_ctbs * 64 * 16; /* MVs (16 MBs / CTB * 16 bytes / MB) */
  raster_luma_size = NEXT_MULTIPLE(asic_buff->width * rs_bit_depth, 16 * 8) / 8 * asic_buff->height;
  raster_chroma_size = raster_luma_size / 2;
#ifdef DOWN_SCALER
  dscale_luma_size = NEXT_MULTIPLE((asic_buff->width >> dec_cont->down_scale_x_shift) * rs_bit_depth, 16 * 8) / 8 *
                     (asic_buff->height >> dec_cont->down_scale_y_shift);
  dscale_chroma_size = NEXT_MULTIPLE(dscale_luma_size / 2, 16);
#else
  dscale_luma_size = 0;
  dscale_chroma_size = 0;
#endif

  if (dec_cont->use_video_compressor) {
    pic_width_in_cbsy = (asic_buff->width + 8 - 1)/8;
    pic_width_in_cbsy = NEXT_MULTIPLE(pic_width_in_cbsy, 16);
    pic_width_in_cbsc = (asic_buff->width + 16 - 1)/16;
    pic_width_in_cbsc = NEXT_MULTIPLE(pic_width_in_cbsc, 16);
    pic_height_in_cbsy = (asic_buff->height + 8 - 1)/8;
    pic_height_in_cbsc = (asic_buff->height/2 + 4 - 1)/4;

    /* luma table size */
    luma_table_size = NEXT_MULTIPLE(pic_width_in_cbsy * pic_height_in_cbsy, 16);
    /* chroma table size */
    chroma_table_size = NEXT_MULTIPLE(pic_width_in_cbsc * pic_height_in_cbsc, 16);
  } else {
    luma_table_size = chroma_table_size = 0;
  }

  /* luma */
  dec_cont->buffer_index = index;
  asic_buff->pictures_c_offset[index] = luma_size;
  asic_buff->dir_mvs_offset[index] = asic_buff->pictures_c_offset[index] + chroma_size;
  if (dec_cont->use_video_compressor) {
    asic_buff->cbs_y_tbl_offset[index] = asic_buff->dir_mvs_offset[index] + dir_mvs_size;
    asic_buff->cbs_c_tbl_offset[index] = asic_buff->cbs_y_tbl_offset[index] + luma_table_size;
  } else {
    asic_buff->cbs_y_tbl_offset[index] = 0;
    asic_buff->cbs_c_tbl_offset[index] = 0;
  }
  asic_buff->raster_c_offset[index] = raster_luma_size;
#ifdef DOWN_SCALER
  asic_buff->dscale_c_offset[index] = dscale_luma_size;
#endif
  asic_buff->picture_size = luma_size + chroma_size + dir_mvs_size + luma_table_size + chroma_table_size;
  asic_buff->raster_size = raster_luma_size + raster_chroma_size;
#ifdef DOWN_SCALER
  if (dec_cont->down_scale_enabled)
    asic_buff->dscale_size = dscale_luma_size + dscale_chroma_size;
  else
    asic_buff->dscale_size = 0;
#else
  asic_buff->dscale_size = 0;
#endif

  if (asic_buff->pictures[index].virtual_address == NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
      dec_cont->next_buf_size = asic_buff->picture_size;
      dec_cont->buf_type = REFERENCE_BUFFER;
      if (index == 0)
        dec_cont->buf_num = dec_cont->num_buffers;
      else
        dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 1;
#endif
      return DEC_WAITING_FOR_BUFFER;
    }
    else {
      dwl_ret |= DWLMallocRefFrm(dec_cont->dwl, asic_buff->picture_size, &asic_buff->pictures[index]);
    }
  }

  if (asic_buff->raster[index].virtual_address == NULL
     && dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
      dec_cont->next_buf_size = asic_buff->raster_size;
      dec_cont->buf_type = RASTERSCAN_OUT_BUFFER;
      if (index == 0)
        dec_cont->buf_num = dec_cont->num_buffers;
      else dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      dwl_ret |= DWLMallocLinear(dec_cont->dwl, asic_buff->raster_size, &asic_buff->raster[index]);
    }
  }

#ifdef DOWN_SCALER
  if (asic_buff->dscale[index].virtual_address == NULL && dec_cont->down_scale_enabled) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
      dec_cont->next_buf_size = asic_buff->dscale_size;
      dec_cont->buf_type = DOWNSCALE_OUT_BUFFER;
      if (index == 0)
        dec_cont->buf_num = dec_cont->num_buffers;
      else dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      dwl_ret |= DWLMallocLinear(dec_cont->dwl, asic_buff->dscale_size, &asic_buff->dscale[index]);
    }
  }
#endif

  if (dwl_ret != DWL_OK) {
    Vp9AsicReleasePictures(dec_cont);
    return HANTRO_NOK;
  }

  return HANTRO_OK;
}

#if 0
i32 Vp9FreeRefFrm(struct Vp9DecContainer *dec_cont, u32 index) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (asic_buff->pictures[index].virtual_address != NULL) {
    /* Reallocate bigger picture buffer into current index */
    dec_cont->buffer_index = index;
    dec_cont->buf_to_free = &asic_buff->pictures[index];
    dec_cont->next_buf_size = 0;
    dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 1;
#endif
    dec_cont->buf_type = REFERENCE_BUFFER;
    return DEC_WAITING_FOR_BUFFER;
  }
  return HANTRO_OK;
}
#endif

void Vp9CalculateBufSize(struct Vp9DecContainer *dec_cont, i32 index) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 num_ctbs, luma_size, chroma_size, dir_mvs_size;
  u32 raster_luma_size, raster_chroma_size;
  u32 dscale_luma_size, dscale_chroma_size;
  u32 pic_width_in_cbsy, pic_height_in_cbsy;
  u32 pic_width_in_cbsc, pic_height_in_cbsc;
  u32 luma_table_size, chroma_table_size;
  u32 bit_depth, rs_bit_depth;

  bit_depth = dec_cont->decoder.bit_depth;
  rs_bit_depth = (dec_cont->use_8bits_output || bit_depth == 8) ? 8 :
                 (dec_cont->use_p010_output) ? 16 : bit_depth;

  luma_size = asic_buff->height * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  chroma_size = (asic_buff->height / 2) * NEXT_MULTIPLE(asic_buff->width * bit_depth, 16 * 8) / 8;
  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  dir_mvs_size = num_ctbs * 64 * 16; /* MVs (16 MBs / CTB * 16 bytes / MB) */
  raster_luma_size = NEXT_MULTIPLE(asic_buff->width * rs_bit_depth, 16 * 8) / 8 * asic_buff->height;
  raster_chroma_size = raster_luma_size / 2;
#ifdef DOWN_SCALER
  dscale_luma_size = NEXT_MULTIPLE((asic_buff->width >> dec_cont->down_scale_x_shift) * rs_bit_depth, 16 * 8) / 8 *
                     (asic_buff->height >> dec_cont->down_scale_y_shift);
  dscale_chroma_size = NEXT_MULTIPLE(dscale_luma_size / 2, 16);
#else
  dscale_luma_size = 0;
  dscale_chroma_size = 0;
#endif

  if (dec_cont->use_video_compressor) {
    pic_width_in_cbsy = (asic_buff->width + 8 - 1)/8;
    pic_width_in_cbsy = NEXT_MULTIPLE(pic_width_in_cbsy, 16);
    pic_width_in_cbsc = (asic_buff->width + 16 - 1)/16;
    pic_width_in_cbsc = NEXT_MULTIPLE(pic_width_in_cbsc, 16);
    pic_height_in_cbsy = (asic_buff->height + 8 - 1)/8;
    pic_height_in_cbsc = (asic_buff->height/2 + 4 - 1)/4;

    /* luma table size */
    luma_table_size = NEXT_MULTIPLE(pic_width_in_cbsy * pic_height_in_cbsy, 16);
    /* chroma table size */
    chroma_table_size = NEXT_MULTIPLE(pic_width_in_cbsc * pic_height_in_cbsc, 16);
  } else {
    luma_table_size = chroma_table_size = 0;
  }

  asic_buff->picture_size = luma_size + chroma_size + dir_mvs_size + luma_table_size + chroma_table_size;
  asic_buff->raster_size = raster_luma_size + raster_chroma_size;
#ifdef DOWN_SCALER
  if (dec_cont->down_scale_enabled)
    asic_buff->dscale_size = dscale_luma_size + dscale_chroma_size;
  else
    asic_buff->dscale_size = 0;
#else
  asic_buff->dscale_size = 0;
#endif

  asic_buff->pictures_c_offset[index] = luma_size;
  asic_buff->dir_mvs_offset[index] = asic_buff->pictures_c_offset[index] + chroma_size;
  if (dec_cont->use_video_compressor) {
    asic_buff->cbs_y_tbl_offset[index] = asic_buff->dir_mvs_offset[index] + dir_mvs_size;
    asic_buff->cbs_c_tbl_offset[index] = asic_buff->cbs_y_tbl_offset[index] + luma_table_size;
  } else {
    asic_buff->cbs_y_tbl_offset[index] = 0;
    asic_buff->cbs_c_tbl_offset[index] = 0;
  }
  asic_buff->raster_c_offset[index] = raster_luma_size;
#ifdef DOWN_SCALER
  asic_buff->dscale_c_offset[index] = dscale_luma_size;
#endif
}

i32 Vp9GetRefFrm(struct Vp9DecContainer *dec_cont) {
  u32 limit = dec_cont->dynamic_buffer_limit;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  if (RequiredBufferCount(dec_cont) < limit)
    limit = RequiredBufferCount(dec_cont);

  if (asic_buff->realloc_tile_edge_mem) return HANTRO_OK;

  if (!asic_buff->realloc_out_buffer && !asic_buff->realloc_seg_map_buffer) {
    asic_buff->out_buffer_i = Vp9BufferQueueGetBuffer(dec_cont->bq, limit);
    if (asic_buff->out_buffer_i < 0) {
      if (Vp9AllocateFrame(dec_cont, dec_cont->num_buffers)) {
        /* Request for a new buffer. */
        asic_buff->realloc_out_buffer = 0;
        return DEC_WAITING_FOR_BUFFER;
      }
      //asic_buff->out_buffer_i = Vp9BufferQueueGetBuffer(dec_cont->bq, limit);
    }

    /* Caculate the buffer size required for current picture. */
    Vp9CalculateBufSize(dec_cont, asic_buff->out_buffer_i);
  }

  /* Reallocate picture memories if needed */
  if (asic_buff->pictures[asic_buff->out_buffer_i].logical_size < asic_buff->picture_size ||
      (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN &&
       asic_buff->raster[asic_buff->out_buffer_i].logical_size < asic_buff->raster_size)) {
    i32 dwl_ret = Vp9ReallocateFrame(dec_cont, asic_buff->out_buffer_i);
    if (dwl_ret) return dwl_ret;
  }

  asic_buff->realloc_out_buffer = 0;

  if (Vp9ReallocateSegmentMap(dec_cont) != HANTRO_OK) {
    asic_buff->realloc_seg_map_buffer = 1;
    return DEC_WAITING_FOR_BUFFER;
  }
  asic_buff->realloc_seg_map_buffer = 0;

  return HANTRO_OK;
}

/* Reallocates segment maps if resolution changes bigger than initial
   resolution. Needs synchronization if SW is running parallel with HW */
i32 Vp9ReallocateSegmentMap(struct Vp9DecContainer *dec_cont) {
  i32 dwl_ret;
  u32 num_ctbs, memory_size;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  memory_size = num_ctbs * 32; /* Segment map uses 32 bytes / CTB */

  /* Do nothing if we have big enough buffers for segment maps */
  if (memory_size <= asic_buff->segment_map_size) return HANTRO_OK;

  /* Allocate new segment maps for larger resolution */
  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, SEGMENT_MAP_BUFFER)) {
    dec_cont->buf_to_free = NULL;
    dec_cont->next_buf_size = memory_size * 2;
    asic_buff->segment_map_size_new = memory_size;
    dec_cont->buf_type = SEGMENT_MAP_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
    dec_cont->is_frame_buffer = 0;
#endif
    return DEC_WAITING_FOR_BUFFER;
  } else {
    /* Allocate new segment maps for larger resolution */
    struct DWLLinearMem new_segment_map;
    i32 new_segment_size = memory_size;

    dwl_ret = DWLMallocLinear(dec_cont->dwl, memory_size * 2, &new_segment_map);

    if (dwl_ret != DWL_OK) {
      //Vp9AsicReleasePictures(dec_cont);
      return HANTRO_NOK;
    }
    /* Copy existing segment maps into new buffers */
    DWLmemcpy(new_segment_map.virtual_address,
              asic_buff->segment_map.virtual_address,
              asic_buff->segment_map_size);
    DWLmemcpy((u8 *)new_segment_map.virtual_address + new_segment_size,
              (u8 *)asic_buff->segment_map.virtual_address + asic_buff->segment_map_size,
              asic_buff->segment_map_size);
    /* Free old segment maps */
    Vp9FreeSegmentMap(dec_cont);

    asic_buff->segment_map_size = new_segment_size;
    asic_buff->segment_map = new_segment_map;
  }

  return HANTRO_OK;
}


i32 Vp9AllocateSegmentMap(struct Vp9DecContainer *dec_cont) {
  i32 dwl_ret;
  u32 num_ctbs, memory_size;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  num_ctbs = ((asic_buff->width + 63) / 64) * ((asic_buff->height + 63) / 64);
  memory_size = num_ctbs * 32; /* Segment map uses 32 bytes / CTB */

  if (asic_buff->segment_map.virtual_address == NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, SEGMENT_MAP_BUFFER)) {
      dec_cont->next_buf_size = memory_size * 2;
      dec_cont->buf_to_free = NULL;
      dec_cont->buf_type = SEGMENT_MAP_BUFFER;
      asic_buff->segment_map_size = memory_size;
      asic_buff->segment_map_size_new = memory_size;
      dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      dwl_ret = DWLMallocLinear(dec_cont->dwl, memory_size * 2, &asic_buff->segment_map);

      if (dwl_ret) return DEC_MEMFAIL;
      asic_buff->segment_map_size = memory_size;
    }
  }

  return HANTRO_OK;
}

i32 Vp9FreeSegmentMap(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (asic_buff->segment_map.virtual_address != NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, SEGMENT_MAP_BUFFER)) {
      dec_cont->buf_to_free = &asic_buff->segment_map;
      dec_cont->next_buf_size = 0;
      dec_cont->buf_type = SEGMENT_MAP_BUFFER;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      DWLFreeLinear(dec_cont->dwl, &asic_buff->segment_map);
      asic_buff->segment_map.virtual_address = NULL;
      asic_buff->segment_map.size = 0;
    }
  }

  return HANTRO_OK;
}

void Vp9UpdateProbabilities(struct Vp9DecContainer *dec_cont) {
  /* Read context counters from HW output memory. */
  DWLmemcpy(&dec_cont->decoder.ctx_ctr,
            dec_cont->asic_buff->misc_linear.virtual_address + dec_cont->asic_buff->ctx_counters_offset / 4,
            sizeof(struct Vp9EntropyCounts));

  /* Backward adaptation of probs based on context counters. */
  if (!dec_cont->decoder.error_resilient &&
      !dec_cont->decoder.frame_parallel_decoding) {
    Vp9AdaptCoefProbs(&dec_cont->decoder);
    if (!dec_cont->decoder.key_frame && !dec_cont->decoder.intra_only) {
      Vp9AdaptModeProbs(&dec_cont->decoder);
      Vp9AdaptModeContext(&dec_cont->decoder);
      Vp9AdaptNmvProbs(&dec_cont->decoder);
    }
  }

  /* Store the adapted probs as base for following frames. */
  Vp9StoreProbs(&dec_cont->decoder);
}

#endif

#ifdef SET_EMPTY_PICTURE_DATA /* USE THIS ONLY FOR DEBUGGING PURPOSES */
void Vp9SetEmptyPictureData(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 index = asic_buff->out_buffer_i;

  DWLPrivateAreaMemset(asic_buff->pictures[index].virtual_address, SET_EMPTY_PICTURE_DATA,
            asic_buff->pictures[index].size);
  DWLPrivateAreaMemset(asic_buff->pictures_c[index].virtual_address,
            SET_EMPTY_PICTURE_DATA, asic_buff->pictures_c[index].size);
  DWLmemset(asic_buff->dir_mvs[index].virtual_address, SET_EMPTY_PICTURE_DATA,
            asic_buff->dir_mvs[index].size);

  if (!dec_cont->compress_bypass) {
    DWLmemset(asic_buff->cbs_luma_table[index].virtual_address,
              SET_EMPTY_PICTURE_DATA, asic_buff->cbs_luma_table[index].size);
    DWLmemset(asic_buff->cbs_chroma_table[index].virtual_address,
              SET_EMPTY_PICTURE_DATA, asic_buff->cbs_chroma_table[index].size);
  }

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
    DWLPrivateAreaMemset(asic_buff->raster_luma[index].virtual_address,
              SET_EMPTY_PICTURE_DATA, asic_buff->raster_luma[index].size);
    DWLPrivateAreaMemset(asic_buff->raster_chroma[index].virtual_address,
              SET_EMPTY_PICTURE_DATA, asic_buff->raster_chroma[index].size);
  }
}
#endif

void Vp9AsicSetTileInfo(struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_TILE_BASE,
               asic_buff->tile_info.bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_TILE_BASE,
               asic_buff->misc_linear.bus_address + asic_buff->tile_info_offset);
#endif
  SetDecRegister(dec_cont->vp9_regs, HWIF_TILE_ENABLE,
                 dec->log2_tile_columns || dec->log2_tile_rows);

  if (dec->log2_tile_columns || dec->log2_tile_rows) {
    u32 tile_rows = (1 << dec->log2_tile_rows);
    u32 tile_cols = (1 << dec->log2_tile_columns);
    u32 i, j, h, tmp, prev_h, prev_w;
#ifndef USE_EXTERNAL_BUFFER
    u16 *p = (u16 *)asic_buff->tile_info.virtual_address;
#else
    u16 *p = (u16 *)((u8 *)asic_buff->misc_linear.virtual_address + asic_buff->tile_info_offset);
#endif
    u32 w_sbs = (asic_buff->width + 63) / 64;
    u32 h_sbs = (asic_buff->height + 63) / 64;

    /* write width + height for each tile in pic */
    for (i = 0, prev_h = 0; i < tile_rows; i++) {
      tmp = (i + 1) * h_sbs / tile_rows;
      h = tmp - prev_h;
      prev_h = tmp;

      /* When first tile row has zero height it's skipped by SW.
       * When pic height (rounded up) is less than 3 SB rows, more than one
       * tile row may be skipped and needs to be handled in sys model stream
       * parsing. HW does not support pic heights < 180 -> does not affect HW. */
      if (h_sbs >= 3 && !i && !h) continue;

      for (j = 0, prev_w = 0; j < tile_cols; j++) {
        tmp = (j + 1) * w_sbs / tile_cols;
        *p++ = tmp - prev_w;
        *p++ = h;
        prev_w = tmp;
      }
    }

    if(dec_cont->legacy_regs){
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_COLS_V0, tile_cols);
      if (h_sbs >= 3)
        SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS_V0,
                       MIN(tile_rows, h_sbs));
      else
        SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS_V0, tile_rows);
    }
    else{
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_COLS, tile_cols);
      if (h_sbs >= 3)
        SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS,
                       MIN(tile_rows, h_sbs));
      else
        SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS, tile_rows);
    }
  } else {
    /* just one "tile", dimensions equal to pic size in SBs */
#ifndef USE_EXTERNAL_BUFFER
    u16 *p = (u16 *)asic_buff->tile_info.virtual_address;
#else
    u16 *p = (u16 *)((u8 *)asic_buff->misc_linear.virtual_address + asic_buff->tile_info_offset);
#endif
    p[0] = (asic_buff->width + 63) / 64;
    p[1] = (asic_buff->height + 63) / 64;
    if (dec_cont->legacy_regs) {
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_COLS_V0, 1);
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS_V0, 1);
    } else {
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_COLS, 1);
      SetDecRegister(dec_cont->vp9_regs, HWIF_NUM_TILE_ROWS, 1);
    }
  }
}

void Vp9AsicSetReferenceFrames(struct Vp9DecContainer *dec_cont) {
  u32 tmp1, tmp2, i;
  u32 cur_height, cur_width;
  struct Vp9Decoder *dec = &dec_cont->decoder;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 index_ref[VP9_ACTIVE_REFS];

  for (i = 0; i < VP9_ACTIVE_REFS; i++) {
    index_ref[i] = Vp9BufferQueueGetRef(dec_cont->bq, dec->active_ref_idx[i]);
  }
  /* unrounded picture dimensions */
  cur_width = dec_cont->decoder.width;
  cur_height = dec_cont->decoder.height;

  /* last reference */
  tmp1 = asic_buff->picture_info[index_ref[0]].coded_width;
  tmp2 = asic_buff->picture_info[index_ref[0]].coded_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_LREF_WIDTH, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_LREF_HEIGHT, tmp2);

  tmp1 = (tmp1 << VP9_REF_SCALE_SHIFT) / cur_width;
  tmp2 = (tmp2 << VP9_REF_SCALE_SHIFT) / cur_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_LREF_HOR_SCALE, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_LREF_VER_SCALE, tmp2);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_YBASE,
               asic_buff->pictures[index_ref[0]].bus_address);
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_CBASE,
               asic_buff->pictures_c[index_ref[0]].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_CBASE,
               asic_buff->pictures[index_ref[0]].bus_address + asic_buff->pictures_c_offset[index_ref[0]]);
#endif

#ifndef USE_EXTERNAL_BUFFER
  if (!dec_cont->use_video_compressor) {
    ASSERT(asic_buff->cbs_luma_table[index_ref[0]].bus_address == 0);
    ASSERT(asic_buff->cbs_chroma_table[index_ref[0]].bus_address == 0);
  } else {
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_TYBASE,
                 asic_buff->cbs_luma_table[index_ref[0]].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_TCBASE,
                 asic_buff->cbs_chroma_table[index_ref[0]].bus_address);
  }
#else
  if (!dec_cont->use_video_compressor) {
    ASSERT(asic_buff->cbs_y_tbl_offset[index_ref[0]] == 0);
    ASSERT(asic_buff->cbs_c_tbl_offset[index_ref[0]] == 0);
  } else {
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_TYBASE,
                 asic_buff->pictures[index_ref[0]].bus_address + asic_buff->cbs_y_tbl_offset[index_ref[0]]);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_TCBASE,
                 asic_buff->pictures[index_ref[0]].bus_address + asic_buff->cbs_c_tbl_offset[index_ref[0]]);
  }
#endif

  /* Colocated MVs are always from previous decoded frame */
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_DBASE,
               asic_buff->dir_mvs[asic_buff->prev_out_buffer_i].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER0_DBASE,
               asic_buff->pictures[asic_buff->prev_out_buffer_i].bus_address + asic_buff->dir_mvs_offset[asic_buff->prev_out_buffer_i]);
#endif
 SetDecRegister(dec_cont->vp9_regs, HWIF_LAST_SIGN_BIAS,
                 dec->ref_frame_sign_bias[LAST_FRAME]);

  /* golden reference */
  tmp1 = asic_buff->picture_info[index_ref[1]].coded_width;
  tmp2 = asic_buff->picture_info[index_ref[1]].coded_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_GREF_WIDTH, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_GREF_HEIGHT, tmp2);

  tmp1 = (tmp1 << VP9_REF_SCALE_SHIFT) / cur_width;
  tmp2 = (tmp2 << VP9_REF_SCALE_SHIFT) / cur_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_GREF_HOR_SCALE, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_GREF_VER_SCALE, tmp2);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_YBASE,
               asic_buff->pictures[index_ref[1]].bus_address);
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_CBASE,
               asic_buff->pictures_c[index_ref[1]].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_CBASE,
               asic_buff->pictures[index_ref[1]].bus_address + asic_buff->pictures_c_offset[index_ref[1]]);
#endif

  if (!dec_cont->use_video_compressor) {
#ifndef USE_EXTERNAL_BUFFER
    ASSERT(asic_buff->cbs_luma_table[index_ref[1]].bus_address == 0);
    ASSERT(asic_buff->cbs_chroma_table[index_ref[1]].bus_address == 0);
#else
    ASSERT(asic_buff->cbs_y_tbl_offset[index_ref[1]] == 0);
    ASSERT(asic_buff->cbs_c_tbl_offset[index_ref[1]] == 0);
#endif
  } else {
#ifndef USE_EXTERNAL_BUFFER
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_TYBASE,
                 asic_buff->cbs_luma_table[index_ref[1]].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_TCBASE,
                 asic_buff->cbs_chroma_table[index_ref[1]].bus_address);
#else
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_TYBASE,
                 asic_buff->pictures[index_ref[1]].bus_address + asic_buff->cbs_y_tbl_offset[index_ref[1]]);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER4_TCBASE,
                 asic_buff->pictures[index_ref[1]].bus_address + asic_buff->cbs_c_tbl_offset[index_ref[1]]);
#endif
  }

  SetDecRegister(dec_cont->vp9_regs, HWIF_GREF_SIGN_BIAS,
                 dec->ref_frame_sign_bias[GOLDEN_FRAME]);

  /* alternate reference */
  tmp1 = asic_buff->picture_info[index_ref[2]].coded_width;
  tmp2 = asic_buff->picture_info[index_ref[2]].coded_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_AREF_WIDTH, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_AREF_HEIGHT, tmp2);

  tmp1 = (tmp1 << VP9_REF_SCALE_SHIFT) / cur_width;
  tmp2 = (tmp2 << VP9_REF_SCALE_SHIFT) / cur_height;
  SetDecRegister(dec_cont->vp9_regs, HWIF_AREF_HOR_SCALE, tmp1);
  SetDecRegister(dec_cont->vp9_regs, HWIF_AREF_VER_SCALE, tmp2);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_YBASE,
               asic_buff->pictures[index_ref[2]].bus_address);
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_CBASE,
               asic_buff->pictures_c[index_ref[2]].bus_address);

#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_CBASE,
               asic_buff->pictures[index_ref[2]].bus_address + asic_buff->pictures_c_offset[index_ref[2]]);
#endif
  SetDecRegister(dec_cont->vp9_regs, HWIF_AREF_SIGN_BIAS,
                 dec->ref_frame_sign_bias[ALTREF_FRAME]);

#ifndef USE_EXTERNAL_BUFFER
  if (!dec_cont->use_video_compressor) {
    ASSERT(asic_buff->cbs_luma_table[index_ref[2]].bus_address == 0);
    ASSERT(asic_buff->cbs_chroma_table[index_ref[2]].bus_address == 0);
  } else {
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_TYBASE,
                 asic_buff->cbs_luma_table[index_ref[2]].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_TCBASE,
                 asic_buff->cbs_chroma_table[index_ref[2]].bus_address);
  }
#else
  if (!dec_cont->use_video_compressor) {
    ASSERT(asic_buff->cbs_y_tbl_offset[index_ref[2]] == 0);
    ASSERT(asic_buff->cbs_c_tbl_offset[index_ref[2]] == 0);
  } else {
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_TYBASE,
                 asic_buff->pictures[index_ref[2]].bus_address + asic_buff->cbs_y_tbl_offset[index_ref[2]]);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_REFER5_TCBASE,
                 asic_buff->pictures[index_ref[2]].bus_address + asic_buff->cbs_c_tbl_offset[index_ref[2]]);
  }
#endif
}

void Vp9AsicSetSegmentation(struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 *vp9_regs = dec_cont->vp9_regs;
  u32 s;
  u32 segval[MAX_MB_SEGMENTS][SEG_LVL_MAX];

  /* Resolution change will clear any previous segment map. */
  if (dec->resolution_change) {
#ifndef USE_EXTERNAL_BUFFER
    DWLmemset(asic_buff->segment_map[0].virtual_address, 0,
              asic_buff->segment_map_size);
    DWLmemset(asic_buff->segment_map[1].virtual_address, 0,
              asic_buff->segment_map_size);
#else
    DWLmemset(asic_buff->segment_map.virtual_address, 0,
              asic_buff->segment_map.logical_size);
#endif
  }
  /* Segmentation */
  SetDecRegister(vp9_regs, HWIF_SEGMENT_E, dec->segment_enabled);
  SetDecRegister(vp9_regs, HWIF_SEGMENT_UPD_E, dec->segment_map_update);
  SetDecRegister(vp9_regs, HWIF_SEGMENT_TEMP_UPD_E,
                 dec->segment_map_temporal_update);
  /* Set filter level and QP for every segment ID. Initialize all
   * segments with default QP and filter level. */
  for (s = 0; s < MAX_MB_SEGMENTS; s++) {
    segval[s][0] = dec->qp_yac;
    segval[s][1] = dec->loop_filter_level;
    segval[s][2] = 0; /* segment ref_frame disabled */
    segval[s][3] = 0; /* segment skip disabled */
  }
  /* If a feature is enabled for a segment, overwrite the default. */
  if (dec->segment_enabled) {
    i32(*segdata)[SEG_LVL_MAX] = dec->segment_feature_data;

    if (dec->segment_feature_mode == VP9_SEG_FEATURE_ABS) {
      for (s = 0; s < MAX_MB_SEGMENTS; s++) {
        if (dec->segment_feature_enable[s][0]) segval[s][0] = segdata[s][0];
        if (dec->segment_feature_enable[s][1]) segval[s][1] = segdata[s][1];
        if (!dec->key_frame && dec->segment_feature_enable[s][2])
          segval[s][2] = segdata[s][2] + 1;
        if (dec->segment_feature_enable[s][3]) segval[s][3] = 1;
      }
    } else /* delta mode */
    {
      for (s = 0; s < MAX_MB_SEGMENTS; s++) {
        if (dec->segment_feature_enable[s][0])
          segval[s][0] = CLIP3(0, 255, dec->qp_yac + segdata[s][0]);
        if (dec->segment_feature_enable[s][1])
          segval[s][1] = CLIP3(0, 63, dec->loop_filter_level + segdata[s][1]);
        if (!dec->key_frame && dec->segment_feature_enable[s][2])
          segval[s][2] = segdata[s][2] + 1;
        if (dec->segment_feature_enable[s][3]) segval[s][3] = 1;
      }
    }
  }
  /* Write QP, filter level, ref frame and skip for every segment */
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG0, segval[0][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG0, segval[0][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG0, segval[0][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG0, segval[0][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG1, segval[1][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG1, segval[1][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG1, segval[1][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG1, segval[1][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG2, segval[2][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG2, segval[2][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG2, segval[2][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG2, segval[2][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG3, segval[3][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG3, segval[3][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG3, segval[3][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG3, segval[3][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG4, segval[4][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG4, segval[4][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG4, segval[4][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG4, segval[4][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG5, segval[5][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG5, segval[5][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG5, segval[5][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG5, segval[5][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG6, segval[6][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG6, segval[6][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG6, segval[6][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG6, segval[6][3]);
  SetDecRegister(vp9_regs, HWIF_QUANT_SEG7, segval[7][0]);
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL_SEG7, segval[7][1]);
  SetDecRegister(vp9_regs, HWIF_REFPIC_SEG7, segval[7][2]);
  SetDecRegister(vp9_regs, HWIF_SKIP_SEG7, segval[7][3]);
}

void Vp9AsicSetLoopFilter(struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  u32 *vp9_regs = dec_cont->vp9_regs;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  /* loop filter */
  SetDecRegister(vp9_regs, HWIF_FILT_LEVEL, dec->loop_filter_level);
  SetDecRegister(vp9_regs, HWIF_FILTERING_DIS, dec->loop_filter_level == 0);
  SetDecRegister(vp9_regs, HWIF_FILT_SHARPNESS, dec->loop_filter_sharpness);

  if (dec->mode_ref_lf_enabled) {
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_0, dec->mb_ref_lf_delta[0]);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_1, dec->mb_ref_lf_delta[1]);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_2, dec->mb_ref_lf_delta[2]);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_3, dec->mb_ref_lf_delta[3]);
    SetDecRegister(vp9_regs, HWIF_FILT_MB_ADJ_0, dec->mb_mode_lf_delta[0]);
    SetDecRegister(vp9_regs, HWIF_FILT_MB_ADJ_1, dec->mb_mode_lf_delta[1]);
  } else {
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_0, 0);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_1, 0);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_2, 0);
    SetDecRegister(vp9_regs, HWIF_FILT_REF_ADJ_3, 0);
    SetDecRegister(vp9_regs, HWIF_FILT_MB_ADJ_0, 0);
    SetDecRegister(vp9_regs, HWIF_FILT_MB_ADJ_1, 0);
  }
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_VERT_FILT_BASE,
               asic_buff->filter_mem.bus_address);
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_BSD_CTRL_BASE,
               asic_buff->bsd_control_mem.bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_VERT_FILT_BASE,
               asic_buff->tile_edge.bus_address + asic_buff->filter_mem_offset);
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_BSD_CTRL_BASE,
               asic_buff->tile_edge.bus_address + asic_buff->bsd_control_mem_offset);
#endif
}

void Vp9AsicSetPictureDimensions(struct Vp9DecContainer *dec_cont) {
  /* Write dimensions for the current picture
     (This is needed when scaling is used) */
  SetDecRegister(dec_cont->vp9_regs, HWIF_PIC_WIDTH_IN_CBS,
                 (dec_cont->width + 7) / 8);
  SetDecRegister(dec_cont->vp9_regs, HWIF_PIC_HEIGHT_IN_CBS,
                 (dec_cont->height + 7) / 8);
  SetDecRegister(dec_cont->vp9_regs, HWIF_PIC_WIDTH_4X4,
                 NEXT_MULTIPLE(dec_cont->width, 8) >> 2);
  SetDecRegister(dec_cont->vp9_regs, HWIF_PIC_HEIGHT_4X4,
                 NEXT_MULTIPLE(dec_cont->height, 8) >> 2);
}

void Vp9AsicSetOutput(struct Vp9DecContainer *dec_cont) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_OUT_DIS, 0);
  SetDecRegister(dec_cont->vp9_regs, HWIF_OUTPUT_8_BITS, dec_cont->use_8bits_output);
  SetDecRegister(dec_cont->vp9_regs,
                 HWIF_OUTPUT_FORMAT,
                 dec_cont->use_p010_output ? 1 :
                 (dec_cont->pixel_format == DEC_OUT_PIXEL_CUSTOMER1 ? 2 : 0));

#ifdef CLEAR_OUT_BUFFER
#ifndef USE_EXTERNAL_BUFFER
  DWLPrivateAreaMemset(asic_buff->pictures[asic_buff->out_buffer_i].virtual_address,
         0, asic_buff->pictures[asic_buff->out_buffer_i].size);
  DWLPrivateAreaMemset(asic_buff->pictures_c[asic_buff->out_buffer_i].virtual_address,
         0, asic_buff->pictures_c[asic_buff->out_buffer_i].size);
#else
  DWLPrivateAreaMemset(asic_buff->pictures[asic_buff->out_buffer_i].virtual_address,
         0, asic_buff->dir_mvs_offset[asic_buff->out_buffer_i]);
#endif
#endif

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_YBASE,
               asic_buff->pictures[asic_buff->out_buffer_i].bus_address);
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_CBASE,
               asic_buff->pictures_c[asic_buff->out_buffer_i].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_CBASE,
               asic_buff->pictures[asic_buff->out_buffer_i].bus_address + asic_buff->pictures_c_offset[asic_buff->out_buffer_i]);
#endif

  if (dec_cont->use_video_compressor) {
#ifndef USE_EXTERNAL_BUFFER
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_TYBASE,
                 asic_buff->cbs_luma_table[asic_buff->out_buffer_i].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_TCBASE,
                 asic_buff->cbs_chroma_table[asic_buff->out_buffer_i].bus_address);
#else
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_TYBASE,
                 asic_buff->pictures[asic_buff->out_buffer_i].bus_address + asic_buff->cbs_y_tbl_offset[asic_buff->out_buffer_i]);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_TCBASE,
                 asic_buff->pictures[asic_buff->out_buffer_i].bus_address + asic_buff->cbs_c_tbl_offset[asic_buff->out_buffer_i]);
#endif
  }
#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_DBASE,
               asic_buff->dir_mvs[asic_buff->out_buffer_i].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_OUT_DBASE,
               asic_buff->pictures[asic_buff->out_buffer_i].bus_address + asic_buff->dir_mvs_offset[asic_buff->out_buffer_i]);
#endif

  /* Raster output configuration. */
  /* TODO(vmr): Add inversion for first version if needed and
     comment well. */
  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_OUT_RS_E,
                 dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN);

  if (dec_cont->output_format == DEC_OUT_FRM_RASTER_SCAN) {
#ifndef USE_EXTERNAL_BUFFER
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_RSY_BASE,
                 asic_buff->raster_luma[asic_buff->out_buffer_i].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_RSC_BASE,
                 asic_buff->raster_chroma[asic_buff->out_buffer_i].bus_address);
#else
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_RSY_BASE,
                 asic_buff->raster[asic_buff->out_buffer_i].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_RSC_BASE,
                 asic_buff->raster[asic_buff->out_buffer_i].bus_address + asic_buff->raster_c_offset[asic_buff->out_buffer_i]);
#endif
  }
#ifdef DOWN_SCALER
  if (dec_cont->down_scale_enabled) {
#ifndef USE_EXTERNAL_BUFFER
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_DSY_BASE,
                 asic_buff->dscale_luma[asic_buff->out_buffer_i].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_DSC_BASE,
                 asic_buff->dscale_chroma[asic_buff->out_buffer_i].bus_address);
#else
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_DSY_BASE,
                   asic_buff->dscale[asic_buff->out_buffer_i].bus_address);
    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_DEC_DSC_BASE,
                 asic_buff->dscale[asic_buff->out_buffer_i].bus_address + asic_buff->dscale_c_offset[asic_buff->out_buffer_i]);
#endif
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_DS_E, dec_cont->down_scale_enabled);
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_DS_X,
                   dec_cont->down_scale_x_shift - 1);
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_DS_Y,
                   dec_cont->down_scale_y_shift - 1);
  }
#endif
}
void Vp9AsicInitPicture(struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  u32 *vp9_regs = dec_cont->vp9_regs;

#ifdef SET_EMPTY_PICTURE_DATA /* USE THIS ONLY FOR DEBUGGING PURPOSES */
  Vp9SetEmptyPictureData(dec_cont);
#endif

  Vp9AsicSetOutput(dec_cont);

  if (!dec->key_frame && !dec->intra_only) {
    Vp9AsicSetReferenceFrames(dec_cont);
  }

  Vp9AsicSetTileInfo(dec_cont);

  Vp9AsicSetSegmentation(dec_cont);

  Vp9AsicSetLoopFilter(dec_cont);

  Vp9AsicSetPictureDimensions(dec_cont);

  SetDecRegister(vp9_regs, HWIF_BIT_DEPTH_Y_MINUS8, dec->bit_depth - 8);
  SetDecRegister(vp9_regs, HWIF_BIT_DEPTH_C_MINUS8, dec->bit_depth - 8);

  /* QP deltas applied after choosing base QP based on segment ID. */
  SetDecRegister(vp9_regs, HWIF_QP_DELTA_Y_DC, dec->qp_ydc);
  SetDecRegister(vp9_regs, HWIF_QP_DELTA_CH_DC, dec->qp_ch_dc);
  SetDecRegister(vp9_regs, HWIF_QP_DELTA_CH_AC, dec->qp_ch_ac);
  SetDecRegister(vp9_regs, HWIF_LOSSLESS_E, dec->lossless);

  /* Mark intra_only frame also a keyframe but copy inter probabilities to
     partition probs for the stream decoding. */
  SetDecRegister(vp9_regs, HWIF_IDR_PIC_E, (dec->key_frame || dec->intra_only));

  SetDecRegister(vp9_regs, HWIF_TRANSFORM_MODE, dec->transform_mode);
  SetDecRegister(vp9_regs, HWIF_MCOMP_FILT_TYPE, dec->mcomp_filter_type);
  SetDecRegister(vp9_regs, HWIF_HIGH_PREC_MV_E,
                 !dec->key_frame && dec->allow_high_precision_mv);
  SetDecRegister(vp9_regs, HWIF_COMP_PRED_MODE, dec->comp_pred_mode);
  SetDecRegister(vp9_regs, HWIF_TEMPOR_MVP_E,
                 !dec->error_resilient && !dec->key_frame &&
                     !dec->prev_is_key_frame && !dec->intra_only &&
                     !dec->resolution_change && dec->prev_show_frame);
  SetDecRegister(vp9_regs, HWIF_COMP_PRED_FIXED_REF, dec->comp_fixed_ref);
  SetDecRegister(vp9_regs, HWIF_COMP_PRED_VAR_REF0, dec->comp_var_ref[0]);
  SetDecRegister(vp9_regs, HWIF_COMP_PRED_VAR_REF1, dec->comp_var_ref[1]);

  if (!dec_cont->conceal) {
    if (dec->key_frame)
      SetDecRegister(dec_cont->vp9_regs, HWIF_WRITE_MVS_E, 0);
    else
      SetDecRegister(dec_cont->vp9_regs, HWIF_WRITE_MVS_E, 1);
  }
}


void Vp9AsicStrmPosUpdate(struct Vp9DecContainer *dec_cont,
                          addr_t strm_bus_address, u32 data_len,
                          addr_t buf_bus_address, u32 buf_len)
{
  u32 tmp, hw_bit_pos;
  addr_t tmp_addr;
  u32 is_rb = dec_cont->use_ringbuffer;
  struct Vp9Decoder *dec = &dec_cont->decoder;

  DEBUG_PRINT(("Vp9AsicStrmPosUpdate:\n"));

  /* Bit position where SW has decoded frame headers.
  tmp = (dec_cont->bc.pos) * 8 + (8 - dec_cont->bc.count);*/

  /* Residual partition after frame header partition. */
  tmp = dec->frame_tag_size + dec->offset_to_dct_parts;

  if(is_rb) {
    u32 turn_around = 0;
    tmp_addr = strm_bus_address + tmp;
    if(tmp_addr >= (buf_bus_address + buf_len)) {
      tmp_addr -= buf_len;
      turn_around = 1;
    }

    hw_bit_pos = (tmp_addr & DEC_HW_ALIGN_MASK) * 8;
    tmp_addr &= (~DEC_HW_ALIGN_MASK); /* align the base */

    SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_START_BIT, hw_bit_pos);

    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_STREAM_BASE, buf_bus_address);

    /* Total stream length passed to HW. */
    if (turn_around)
      tmp = data_len + strm_bus_address - (tmp_addr + buf_len);
    else
      tmp = data_len + strm_bus_address - tmp_addr;
    SetDecRegister(dec_cont->vp9_regs, HWIF_STREAM_LEN, tmp);

    /* stream data start offset */
    tmp_addr = tmp_addr - buf_bus_address;
    SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_START_OFFSET, tmp_addr);

    /* stream buffer size */
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_BUF_LEN, buf_len);
  } else {
    tmp_addr = strm_bus_address + tmp;

    hw_bit_pos = (tmp_addr & DEC_HW_ALIGN_MASK) * 8;
    tmp_addr &= (~DEC_HW_ALIGN_MASK); /* align the base */

    SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_START_BIT, hw_bit_pos);

    SET_ADDR_REG(dec_cont->vp9_regs, HWIF_STREAM_BASE, tmp_addr);

    /* Total stream length passed to HW. */
    tmp = data_len - (tmp_addr - strm_bus_address);

    SetDecRegister(dec_cont->vp9_regs, HWIF_STREAM_LEN, tmp);

    /* stream data start offset */
    tmp = (u32)(tmp_addr - buf_bus_address);
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_START_OFFSET, 0);

    /* stream buffer size */
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->vp9_regs, HWIF_STRM_BUF_LEN, buf_len - tmp);
  }

  DEBUG_PRINT(("STREAM BUS ADDR: 0x%08x\n", strm_bus_address));
  DEBUG_PRINT(("HW STREAM BASE: 0x%08x\n", tmp_addr));
  DEBUG_PRINT(("HW START BIT: %d\n", hw_bit_pos));
  DEBUG_PRINT(("HW STREAM LEN: %d\n", tmp));
}

u32 Vp9AsicRun(struct Vp9DecContainer *dec_cont) {
  i32 ret = 0;
  if (!dec_cont->asic_running) {
    ret = DWLReserveHw(dec_cont->dwl, &dec_cont->core_id);
    if (ret != DWL_OK) {
      return VP9HWDEC_HW_RESERVED;
    }

    dec_cont->asic_running = 1;

    FlushDecRegisters(dec_cont->dwl, dec_cont->core_id, dec_cont->vp9_regs);

    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_E, 1);
    DWLEnableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1, dec_cont->vp9_regs[1]);
  } else {
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 13,
                dec_cont->vp9_regs[13]);
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 14,
                dec_cont->vp9_regs[14]);
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 15,
                dec_cont->vp9_regs[15]);
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 1, dec_cont->vp9_regs[1]);
  }

  Vp9SetupPicToOutput(dec_cont);
  return ret;
}

u32 Vp9AsicSync(struct Vp9DecContainer *dec_cont) {
  u32 asic_status = 0;
  i32 ret;

  ret = DWLWaitHwReady(dec_cont->dwl, dec_cont->core_id,
                       (u32)DEC_X170_TIMEOUT_LENGTH);

  if (ret != DWL_HW_WAIT_OK) {
    ERROR_PRINT("DWLWaitHwReady");
    DEBUG_PRINT(("DWLWaitHwReady returned: %d\n", ret));

    /* Reset HW */
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_IRQ, 0);
    SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_E, 0);

    DWLDisableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                 dec_cont->vp9_regs[1]);

    dec_cont->asic_running = 0;
    DWLReleaseHw(dec_cont->dwl, dec_cont->core_id);

    return (ret == DWL_HW_WAIT_ERROR) ? VP9HWDEC_SYSTEM_ERROR
                                      : VP9HWDEC_SYSTEM_TIMEOUT;
  }

  RefreshDecRegisters(dec_cont->dwl, dec_cont->core_id, dec_cont->vp9_regs);

  /* React to the HW return value */

  asic_status = GetDecRegister(dec_cont->vp9_regs, HWIF_DEC_IRQ_STAT);

  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_IRQ_STAT, 0);
  SetDecRegister(dec_cont->vp9_regs, HWIF_DEC_IRQ, 0); /* just in case */

  /* HW done, release it! */
  DWLDisableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1, dec_cont->vp9_regs[1]);
  dec_cont->asic_running = 0;

  DWLReleaseHw(dec_cont->dwl, dec_cont->core_id);

  return asic_status;
}

#ifndef USE_EXTERNAL_BUFFER
void Vp9AsicProbUpdate(struct Vp9DecContainer *dec_cont) {
  struct DWLLinearMem *segment_map = dec_cont->asic_buff->segment_map;
  u8 *asic_prob_base = (u8 *)dec_cont->asic_buff->prob_tbl.virtual_address;

  /* Write probability tables to HW memory */
  DWLmemcpy(asic_prob_base, &dec_cont->decoder.entropy,
            sizeof(struct Vp9EntropyProbs));

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_PROB_TAB_BASE,
               dec_cont->asic_buff->prob_tbl.bus_address);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_CTX_COUNTER_BASE,
               dec_cont->asic_buff->ctx_counters.bus_address);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_SEGMENT_READ_BASE,
               segment_map[dec_cont->active_segment_map].bus_address);
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_SEGMENT_WRITE_BASE,
               segment_map[1 - dec_cont->active_segment_map].bus_address);

  /* Update active segment map for next frame */
  if (dec_cont->decoder.segment_map_update)
    dec_cont->active_segment_map = 1 - dec_cont->active_segment_map;
}
#else
void Vp9AsicProbUpdate(struct Vp9DecContainer *dec_cont) {
  struct DWLLinearMem *segment_map = &dec_cont->asic_buff->segment_map;
  u8 *asic_prob_base = (u8 *)dec_cont->asic_buff->misc_linear.virtual_address + dec_cont->asic_buff->prob_tbl_offset;

  /* Write probability tables to HW memory */
  DWLmemcpy(asic_prob_base, &dec_cont->decoder.entropy,
            sizeof(struct Vp9EntropyProbs));

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_PROB_TAB_BASE,
               dec_cont->asic_buff->misc_linear.bus_address + dec_cont->asic_buff->prob_tbl_offset);

  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_CTX_COUNTER_BASE,
               dec_cont->asic_buff->misc_linear.bus_address + dec_cont->asic_buff->ctx_counters_offset);

#if 0
  SetDecRegister(dec_cont->vp9_regs, HWIF_SEGMENT_READ_BASE_LSB,
                 segment_map[dec_cont->active_segment_map].bus_address);
  SetDecRegister(dec_cont->vp9_regs, HWIF_SEGMENT_WRITE_BASE_LSB,
                 segment_map[1 - dec_cont->active_segment_map].bus_address);
#else
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_SEGMENT_READ_BASE,
               segment_map->bus_address + dec_cont->active_segment_map * dec_cont->asic_buff->segment_map_size);
  SET_ADDR_REG(dec_cont->vp9_regs, HWIF_SEGMENT_WRITE_BASE,
               segment_map->bus_address + (1 - dec_cont->active_segment_map) * dec_cont->asic_buff->segment_map_size);
#endif

  /* Update active segment map for next frame */
  if (dec_cont->decoder.segment_map_update)
    dec_cont->active_segment_map = 1 - dec_cont->active_segment_map;
}
#endif

void Vp9UpdateRefs(struct Vp9DecContainer *dec_cont, u32 corrupted) {
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;

  if (!corrupted || (corrupted && dec_cont->pic_number != 1)) {
    if (dec_cont->decoder.reset_frame_flags) {
      Vp9BufferQueueUpdateRef(dec_cont->bq, (1 << NUM_REF_FRAMES) - 1,
        REFERENCE_NOT_SET);
      dec_cont->decoder.reset_frame_flags = 0;
    }
    Vp9BufferQueueUpdateRef(dec_cont->bq, dec_cont->decoder.refresh_frame_flags,
                            asic_buff->out_buffer_i);
  }
  if (!dec_cont->decoder.show_frame ||
      (dec_cont->pic_number != 1 && corrupted)) {
      /* If the picture will not be outputted, we need to remove ref used to
          protect the output. */
    Vp9BufferQueueRemoveRef(dec_cont->bq, asic_buff->out_buffer_i);
  }
}
