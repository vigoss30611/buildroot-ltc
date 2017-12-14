/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Video Usability Information (VUI) decoding and storage. */

#ifndef HEVC_VUI_H_
#define HEVC_VUI_H_

#include "basetype.h"
#include "sw_stream.h"
#include "hevc_cfg.h"

#define MAX_CPB_CNT 32

/* enumerated sample aspect ratios, ASPECT_RATIO_M_N means M:N */
enum {
  ASPECT_RATIO_UNSPECIFIED = 0,
  ASPECT_RATIO_1_1,
  ASPECT_RATIO_12_11,
  ASPECT_RATIO_10_11,
  ASPECT_RATIO_16_11,
  ASPECT_RATIO_40_33,
  ASPECT_RATIO_24_11,
  ASPECT_RATIO_20_11,
  ASPECT_RATIO_32_11,
  ASPECT_RATIO_80_33,
  ASPECT_RATIO_18_11,
  ASPECT_RATIO_15_11,
  ASPECT_RATIO_64_33,
  ASPECT_RATIO_160_99,
  ASPECT_RATIO_EXTENDED_SAR = 255
};

/* storage for VUI parameters */
struct VuiParameters {
  u32 aspect_ratio_present_flag;
  u32 aspect_ratio_idc;
  u32 sar_width;
  u32 sar_height;
  u32 video_signal_type_present_flag;
  u32 video_format;
  u32 video_full_range_flag;
  u32 colour_description_present_flag;
  u32 colour_primaries;
  u32 transfer_characteristics;
  u32 matrix_coefficients;
};

u32 HevcDecodeVuiParameters(struct StrmData *stream,
                            struct VuiParameters *vui_parameters);

#endif /* #ifdef HEVC_VUI_H_ */
