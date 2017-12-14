/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Picture Parameter Set (PPS) decoding and storage. */

#ifndef HEVC_PIC_PARAM_SET_H_
#define HEVC_PIC_PARAM_SET_H_

#include "basetype.h"
#include "hevc_cfg.h"
#include "sw_stream.h"

struct TileInfo {
  u32 num_tile_columns;
  u32 num_tile_rows;
  u32 uniform_spacing;
  u32 col_width[MAX_TILE_COLS];
  u32 row_height[MAX_TILE_ROWS];
};

/* data structure to store PPS information decoded from the stream */
struct PicParamSet {
  u32 pic_parameter_set_id;
  u32 seq_parameter_set_id;
  u32 dependent_slice_segments_enabled;
  u32 sign_data_hiding_flag;
  u32 cabac_init_present;
  u32 num_ref_idx_l0_active;
  u32 num_ref_idx_l1_active;
  u32 pic_init_qp;
  u32 constrained_intra_pred_flag;
  u32 transform_skip_enable;
  u32 cu_qp_delta_enabled;
  u32 diff_cu_qp_delta_depth;
  i32 cb_qp_offset;
  i32 cr_qp_offset;
  u32 slice_level_chroma_qp_offsets_present;
  u32 weighted_pred_flag;
  u32 weighted_bi_pred_flag;
  u32 output_flag_present;
  u32 trans_quant_bypass_enable;
  u32 tiles_enabled_flag;
  u32 entropy_coding_sync_enabled_flag;
  struct TileInfo tile_info;
  u32 loop_filter_across_tiles_enabled_flag;
  u32 loop_filter_across_slices_enabled_flag;
  u32 deblocking_filter_control_present_flag;
  u32 deblocking_filter_override_enabled_flag;
  u32 disable_deblocking_filter_flag;
  i32 beta_offset;
  i32 tc_offset;
  u32 scaling_list_present_flag;
  u8 scaling_list[4][6][64];
  u32 lists_modification_present_flag;
  u32 log_parallel_merge_level;
  u32 num_extra_slice_header_bits;
  u32 slice_header_extension_present_flag;
};

u32 HevcDecodePicParamSet(struct StrmData *stream,
                          struct PicParamSet *pic_param_set);
void DefaultScalingList(u8 scaling_list[4][6][64]);

#endif /* #ifdef HEVC_PIC_PARAM_SET_H_ */
