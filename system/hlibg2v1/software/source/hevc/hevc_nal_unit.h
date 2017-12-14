/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * NAL unit handling related definitions. */

#ifndef HEVC_NAL_UNIT_H_
#define HEVC_NAL_UNIT_H_

#include "basetype.h"
#include "sw_stream.h"
#include "hevc_nal_unit_type.h"

/* macro to determine if NAL unit pointed by nal_unit contains an IDR slice */
#define IS_IDR_NAL_UNIT(nal_unit)                           \
  ((nal_unit)->nal_unit_type == NAL_CODED_SLICE_IDR_W_LP || \
   (nal_unit)->nal_unit_type == NAL_CODED_SLICE_IDR_N_LP)

#define IS_RAP_NAL_UNIT(nal_unit) \
  ((nal_unit)->nal_unit_type >=   \
   NAL_CODED_SLICE_BLA_W_LP&&(nal_unit)->nal_unit_type <= NAL_CODED_SLICE_CRA)

#define IS_SLICE_NAL_UNIT(nal_unit)                             \
  (/*(nal_unit)->nal_unit_type >= NAL_CODED_SLICE_TRAIL_N && */ \
   (nal_unit)->nal_unit_type <= NAL_CODED_SLICE_CRA)

struct NalUnit {
  enum NalUnitType nal_unit_type;
  u32 temporal_id;
};

u32 HevcDecodeNalUnit(struct StrmData* stream, struct NalUnit* nal_unit);

#endif /* #ifdef HEVC_NAL_UNIT_H_ */
