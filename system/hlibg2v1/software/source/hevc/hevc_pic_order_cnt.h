/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Picture Order Count (POC) related functionality. */

#ifndef HEVC_PIC_ORDER_CNT_H_
#define HEVC_PIC_ORDER_CNT_H_

#include "basetype.h"
#include "hevc_seq_param_set.h"
#include "hevc_slice_header.h"
#include "hevc_nal_unit.h"

#define INIT_POC 0x7FFFFFFF

/* structure to store information computed for previous picture, needed for
 * POC computation of a picture. Two first fields for POC type 0, last two
 * for types 1 and 2 */
struct PocStorage {
  u32 prev_pic_order_cnt_lsb;
  i32 prev_pic_order_cnt_msb;
  i32 pic_order_cnt;
};

void HevcDecodePicOrderCnt(struct PocStorage *poc, u32 max_pic_order_cnt_lsb,
                           const struct SliceHeader *slice_header,
                           const struct NalUnit *nal_unit);

#endif /* #ifdef HEVC_PIC_ORDER_CNT_H_ */
