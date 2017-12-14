/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Common definitions and utility functions. */

#ifndef HEVC_UTIL_H_
#define HEVC_UTIL_H_

#include "basetype.h"
#include "sw_stream.h"
#include "sw_util.h"
#include "sw_debug.h"

u32 HevcRbspTrailingBits(struct StrmData *strm_data);
u32 HevcMoreRbspData(struct StrmData *strm_data);
u32 HevcCheckCabacZeroWords(struct StrmData *strm_data);

#endif /* #ifdef HEVC_UTIL_H_ */
