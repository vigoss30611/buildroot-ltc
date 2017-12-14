/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Slice header decoding and storage. */

#ifndef HEVC_SLICE_HEADER_H_
#define HEVC_SLICE_HEADER_H_

#include "basetype.h"
#include "sw_stream.h"
#include "hevc_cfg.h"
#include "hevc_seq_param_set.h"
#include "hevc_pic_param_set.h"
#include "hevc_nal_unit.h"

enum {
  B_SLICE = 0,
  P_SLICE = 1,
  I_SLICE = 2,
};

enum {
  NO_LONG_TERM_FRAME_INDICES = 0xFFFF
};

/* macro to determine if slice is an inter slice, slice_types 0 and 5 */
#define IS_P_SLICE(slice_type) \
  (((slice_type) == P_SLICE) || ((slice_type) == P_SLICE + 5))

/* macro to determine if slice is an intra slice, slice_types 2 and 7 */
#define IS_I_SLICE(slice_type) \
  (((slice_type) == I_SLICE) || ((slice_type) == I_SLICE + 5))

#define IS_B_SLICE(slice_type) \
  (((slice_type) == B_SLICE) || ((slice_type) == B_SLICE + 5))

/* structure to store slice header data decoded from the stream */
struct SliceHeader {
  u32 no_output_of_prior_pics_flag;
  u32 pic_parameter_set_id;
  u32 slice_address;
  u32 dependent_slice_flag;
  u32 slice_type;
  u32 pic_output_flag;
  /*u32 colour_plane_id;*/
  u32 pic_order_cnt_lsb;
  u32 short_term_ref_pic_set_sps_flag;
  u32 short_term_ref_pic_set_idx;
  struct StRefPicSet st_ref_pic_set;
  u32 num_long_term_sps;
  u32 num_long_term_pics;
  u32 lt_idx_sps[MAX_NUM_LT_REF_PICS_SPS];
  u32 poc_lsb_lt[MAX_NUM_LT_REF_PICS_SPS];
  u32 used_by_curr_pic_lt[MAX_NUM_LT_REF_PICS_SPS];
  u32 delta_poc_msb_present_flag[MAX_NUM_LT_REF_PICS_SPS];
  u32 delta_poc_msb_cycle_lt[MAX_NUM_LT_REF_PICS_SPS];
  /*u32 slice_sao_luma_flag;*/
  /*u32 slice_sao_chroma_flag;*/
  u32 temporal_mvp_enable_flag;
  u32 hw_skip_bits;
};

u32 HevcDecodeSliceHeader(struct StrmData *stream,
                          struct SliceHeader *slice_header,
                          struct SeqParamSet *seq_param_set,
                          struct PicParamSet *pic_param_set,
                          struct NalUnit *nal_unit);

u32 HevcCheckPpsId(struct StrmData *stream, u32 *pps_id, u32 is_rap_nal_unit);

u32 HevcCheckPicOrderCntLsb(struct StrmData *stream,
                            struct SeqParamSet *seq_param_set,
                            enum NalUnitType nal_unit_type,
                            u32 *pic_order_cnt_lsb);

u32 HevcCheckPriorPicsFlag(u32 *no_output_of_prior_pics_flag,
                           const struct StrmData *stream);

u32 HevcCheckSliceAddress(struct StrmData *stream,
                          enum NalUnitType nal_unit_type, u32 *slice_address);

#endif /* #ifdef HEVC_SLICE_HEADER_H_ */
