/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Picture Order Count decoding. */

#include "hevc_util.h"
#include "hevc_pic_order_cnt.h"

/* Compute picture order count for a picture. */
void HevcDecodePicOrderCnt(struct PocStorage *poc, u32 max_pic_order_cnt_lsb,
                           const struct SliceHeader *slice_header,
                           const struct NalUnit *nal_unit) {

  i32 pic_order_cnt = 0;

  ASSERT(poc);
  ASSERT(slice_header);
  ASSERT(nal_unit);

  /* set prev_pic_order_cnt values for IDR frame */
  if (IS_IDR_NAL_UNIT(nal_unit)) {
    poc->prev_pic_order_cnt_msb = 0;
    poc->prev_pic_order_cnt_lsb = 0;
  }

  if (poc->pic_order_cnt != INIT_POC) {
    /* compute pic_order_cnt_msb (stored in pic_order_cnt variable) */
    if ((slice_header->pic_order_cnt_lsb < poc->prev_pic_order_cnt_lsb) &&
        ((poc->prev_pic_order_cnt_lsb - slice_header->pic_order_cnt_lsb) >=
         max_pic_order_cnt_lsb / 2)) {
      pic_order_cnt = poc->prev_pic_order_cnt_msb + (i32)max_pic_order_cnt_lsb;
    } else if ((slice_header->pic_order_cnt_lsb >
                poc->prev_pic_order_cnt_lsb) &&
               ((slice_header->pic_order_cnt_lsb -
                 poc->prev_pic_order_cnt_lsb) > max_pic_order_cnt_lsb / 2)) {
      pic_order_cnt = poc->prev_pic_order_cnt_msb - (i32)max_pic_order_cnt_lsb;
    } else
      pic_order_cnt = poc->prev_pic_order_cnt_msb;
  }

  /* standard specifies that prev_pic_order_cnt_msb is from previous
   * rererence frame with temporal_id equal to 0 -> replace old value
   * only if current frame matches */
  if ((nal_unit->nal_unit_type == NAL_CODED_SLICE_TRAIL_R ||
       nal_unit->nal_unit_type == NAL_CODED_SLICE_TSA_R ||
       nal_unit->nal_unit_type == NAL_CODED_SLICE_STSA_R ||
       (nal_unit->nal_unit_type >= NAL_CODED_SLICE_BLA_W_LP &&
        nal_unit->nal_unit_type <= NAL_CODED_SLICE_CRA)) &&
      nal_unit->temporal_id == 0) {
    poc->prev_pic_order_cnt_msb = pic_order_cnt;
    poc->prev_pic_order_cnt_lsb = slice_header->pic_order_cnt_lsb;
  }

  pic_order_cnt += (i32)slice_header->pic_order_cnt_lsb;

  poc->pic_order_cnt = pic_order_cnt;
}
