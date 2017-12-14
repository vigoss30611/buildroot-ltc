/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Top level decoder interface (not API though). */

#ifndef HEVC_DECODER_H_
#define HEVC_DECODER_H_

#include "basetype.h"
#include "hevc_storage.h"
#include "hevc_container.h"
#include "hevc_dpb.h"

/* enumerated return values of the functions */
enum {
  HEVC_RDY,
  HEVC_PIC_RDY,
  HEVC_HDRS_RDY,
#ifdef USE_EXTERNAL_BUFFER
  HEVC_BUFFER_NOT_READY,
#endif
  HEVC_ERROR,
  HEVC_PARAM_SET_ERROR,
  HEVC_NEW_ACCESS_UNIT,
  HEVC_NONREF_PIC_SKIPPED
};

void HevcInit(struct Storage *storage, u32 no_output_reordering);
u32 HevcDecode(struct HevcDecContainer *dec_cont, const u8 *byte_strm, u32 len,
               u32 pic_id, u32 *read_bytes);
void HevcShutdown(struct Storage *storage);

const struct DpbOutPicture *HevcNextOutputPicture(struct Storage *storage);

u32 HevcPicWidth(struct Storage *storage);
u32 HevcPicHeight(struct Storage *storage);
u32 HevcVideoRange(struct Storage *storage);
u32 HevcMatrixCoefficients(struct Storage *storage);
u32 HevcIsMonoChrome(struct Storage *storage);
void HevcCroppingParams(struct Storage *storage, u32 *cropping_flag, u32 *left,
                        u32 *width, u32 *top, u32 *height);

u32 HevcCheckValidParamSets(struct Storage *storage);

void HevcFlushBuffer(struct Storage *storage);

u32 HevcAspectRatioIdc(const struct Storage *storage);
void HevcSarSize(const struct Storage *storage, u32 *sar_width,
                 u32 *sar_height);

u32 HevcLumaBitDepth(struct Storage *storage);
u32 HevcChromaBitDepth(struct Storage *storage);

#endif /* #ifdef HEVC_DECODER_H_ */
