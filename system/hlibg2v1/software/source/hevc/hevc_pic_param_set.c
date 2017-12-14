/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Picture Parameter Set decoding. */

#include "hevc_pic_param_set.h"
#include "hevc_exp_golomb.h"
#include "hevc_cfg.h"
#include "hevc_byte_stream.h"
#include "dwl.h"
#include "hevc_util.h"

static u32 HevcDecodeTileInfo(struct StrmData *stream,
                              struct TileInfo *tile_info);
u32 ScalingListData(u8 scaling_list[4][6][64], struct StrmData *stream);

static const u32 scan4x4[16] = {0, 4, 1,  8,  5, 2,  12, 9,
                                6, 3, 13, 10, 7, 14, 11, 15};

static const u32 scan8x8[64] = {
    0,  8,  1,  16, 9,  2,  24, 17, 10, 3,  32, 25, 18, 11, 4,  40,
    33, 26, 19, 12, 5,  48, 41, 34, 27, 20, 13, 6,  56, 49, 42, 35,
    28, 21, 14, 7,  57, 50, 43, 36, 29, 22, 15, 58, 51, 44, 37, 30,
    23, 59, 52, 45, 38, 31, 60, 53, 46, 39, 61, 54, 47, 62, 55, 63};

const u8 default4x4_0_2[16] = {16, 16, 16, 16, 16, 16, 16, 16,
                               16, 16, 16, 16, 16, 16, 16, 16};
const u8 default4x4_3_5[16] = {16, 16, 16, 16, 16, 16, 16, 16,
                               16, 16, 16, 16, 16, 16, 16, 16};
const u8 default8x8_0_2[64] = {
    16, 16, 16, 16, 17, 18, 21, 24, 16, 16, 16, 16, 17, 19, 22, 25,
    16, 16, 17, 18, 20, 22, 25, 29, 16, 16, 18, 21, 24, 27, 31, 36,
    17, 17, 20, 24, 30, 35, 41, 47, 18, 19, 22, 27, 35, 44, 54, 65,
    21, 22, 25, 31, 41, 54, 70, 88, 24, 25, 29, 36, 47, 65, 88, 115};
const u8 default8x8_3_5[64] = {
    16, 16, 16, 16, 17, 18, 20, 24, 16, 16, 16, 17, 18, 20, 24, 25,
    16, 16, 17, 18, 20, 24, 25, 28, 16, 17, 18, 20, 24, 25, 28, 33,
    17, 18, 20, 24, 25, 28, 33, 41, 18, 20, 24, 25, 28, 33, 41, 54,
    20, 24, 25, 28, 33, 41, 54, 71, 24, 25, 28, 33, 41, 54, 71, 91};

const u32 list_size[] = {16, 64, 64, 64};
const u8 *default_ptr[4][6] = {{default4x4_0_2, default4x4_0_2, default4x4_0_2,
                                default4x4_3_5, default4x4_3_5, default4x4_3_5},
                               {default8x8_0_2, default8x8_0_2, default8x8_0_2,
                                default8x8_3_5, default8x8_3_5, default8x8_3_5},
                               {default8x8_0_2, default8x8_0_2, default8x8_0_2,
                                default8x8_3_5, default8x8_3_5, default8x8_3_5},
                               {default8x8_0_2, default8x8_3_5}};

/* Decodes picture parameter set information from the stream. */
u32 HevcDecodePicParamSet(struct StrmData *stream,
                          struct PicParamSet *pic_param_set) {

  u32 tmp, value;
  i32 itmp;

  ASSERT(stream);
  ASSERT(pic_param_set);

  (void)DWLmemset(pic_param_set, 0, sizeof(struct PicParamSet));

  tmp =
      HevcDecodeExpGolombUnsigned(stream, &pic_param_set->pic_parameter_set_id);
  if (pic_param_set->pic_parameter_set_id >= MAX_NUM_PIC_PARAM_SETS) {
    ERROR_PRINT("pic_parameter_set_id");
    return (HANTRO_NOK);
  }

  tmp =
      HevcDecodeExpGolombUnsigned(stream, &pic_param_set->seq_parameter_set_id);
  if (pic_param_set->seq_parameter_set_id >= MAX_NUM_SEQ_PARAM_SETS) {
    ERROR_PRINT("seq_param_set_id");
    return (HANTRO_NOK);
  }

  tmp = SwGetBits(stream, 1);
  pic_param_set->dependent_slice_segments_enabled = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->output_flag_present = tmp;

  tmp = SwGetBits(stream, 3);
  pic_param_set->num_extra_slice_header_bits = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->sign_data_hiding_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->cabac_init_present = tmp;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  pic_param_set->num_ref_idx_l0_active = value + 1;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  pic_param_set->num_ref_idx_l1_active = value + 1;
  if (pic_param_set->num_ref_idx_l0_active > 15 ||
      pic_param_set->num_ref_idx_l1_active > 15) {
    ERROR_PRINT("num_ref_idx_lx_default_active_minus1");
    return HANTRO_NOK;
  }

  /* pic_init_qp_minus26 */
  /* The value of init_qp_minus26 shall be in the range of -(26 + QpBdOffset_Y) to +25, inclusive*/
  tmp = HevcDecodeExpGolombSigned(stream, &itmp);
  if ((itmp < -(26 + 12)) || (itmp > 25)) {
    ERROR_PRINT("pic_init_qp_minus26");
    return (HANTRO_NOK);
  }
  pic_param_set->pic_init_qp = (u32)(itmp + 26);

  tmp = SwGetBits(stream, 1);
  pic_param_set->constrained_intra_pred_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->transform_skip_enable = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->cu_qp_delta_enabled = tmp;

  if (pic_param_set->cu_qp_delta_enabled) {
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    pic_param_set->diff_cu_qp_delta_depth = value;
  }

  tmp = HevcDecodeExpGolombSigned(stream, &itmp);
  pic_param_set->cb_qp_offset = itmp;

  tmp = HevcDecodeExpGolombSigned(stream, &itmp);
  pic_param_set->cr_qp_offset = itmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->slice_level_chroma_qp_offsets_present = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->weighted_pred_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->weighted_bi_pred_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->trans_quant_bypass_enable = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->tiles_enabled_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->entropy_coding_sync_enabled_flag = tmp;

  if (pic_param_set->tiles_enabled_flag) {
    tmp = HevcDecodeTileInfo(stream, &pic_param_set->tile_info);
    if (tmp != HANTRO_OK) return tmp;

    /* TODO: standard does not have this condition, but HM10.0 does */
    if (pic_param_set->tile_info.num_tile_columns ||
        pic_param_set->tile_info.num_tile_rows) {
      tmp = SwGetBits(stream, 1);
      pic_param_set->loop_filter_across_tiles_enabled_flag = tmp;
    }
  }

  tmp = SwGetBits(stream, 1);
  pic_param_set->loop_filter_across_slices_enabled_flag = tmp;

  tmp = SwGetBits(stream, 1);
  pic_param_set->deblocking_filter_control_present_flag = tmp;

  if (pic_param_set->deblocking_filter_control_present_flag) {
    tmp = SwGetBits(stream, 1);
    pic_param_set->deblocking_filter_override_enabled_flag = tmp;

    tmp = SwGetBits(stream, 1);
    pic_param_set->disable_deblocking_filter_flag = tmp;

    if (!pic_param_set->disable_deblocking_filter_flag) {
      tmp = HevcDecodeExpGolombSigned(stream, &itmp);
      pic_param_set->beta_offset = 2 * itmp;
      tmp = HevcDecodeExpGolombSigned(stream, &itmp);
      pic_param_set->tc_offset = 2 * itmp;
    }
  }

  tmp = SwGetBits(stream, 1);
  pic_param_set->scaling_list_present_flag = tmp;

  if (pic_param_set->scaling_list_present_flag) {
    tmp = ScalingListData(pic_param_set->scaling_list, stream);
  }

  tmp = SwGetBits(stream, 1);
  pic_param_set->lists_modification_present_flag = tmp;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  pic_param_set->log_parallel_merge_level = value + 2;

  /* shall be 0 */
  tmp = SwGetBits(stream, 1);
  pic_param_set->slice_header_extension_present_flag = tmp;

  /* pps_extension */
  if (SwGetBits(stream, 1) == 1)
    tmp = HevcNextStartCode(stream);
  else
    tmp = HevcRbspTrailingBits(stream);

  /* ignore possible errors in trailing bits of parameters sets */
  return (HANTRO_OK);
}

/* Decodes tile information from the stream. */
u32 HevcDecodeTileInfo(struct StrmData *stream, struct TileInfo *tile_info) {

  /* Variables */

  u32 tmp, i, value;

  /* Code */

  ASSERT(stream);
  ASSERT(tile_info);

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  tile_info->num_tile_columns = value + 1;
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  tile_info->num_tile_rows = value + 1;
  if (tile_info->num_tile_columns > MAX_TILE_COLS ||
      tile_info->num_tile_rows > MAX_TILE_ROWS)
    return HANTRO_NOK;

  tmp = SwGetBits(stream, 1);
  tile_info->uniform_spacing = tmp;
  if (!tile_info->uniform_spacing) {
    for (i = 0; i < tile_info->num_tile_columns - 1; i++) {
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
      tile_info->col_width[i] = value + 1;
    }
    for (i = 0; i < tile_info->num_tile_rows - 1; i++) {
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
      tile_info->row_height[i] = value + 1;
    }
  }

  return HANTRO_OK;
}

/* Decodes scaling list for certain size_id and matrix_id. */
u32 ScalingList(u8 *scaling_list, struct StrmData *stream, u32 size_id,
                u8 *dc_coeff) {

  u32 tmp;
  u32 next_coef = 8;
  u32 i = 0, size;
  i32 delta, dc_coef;
  const u32 *scan;

  ASSERT(size_id < 2 || dc_coeff);

  if (size_id > 1) {
    tmp = HevcDecodeExpGolombSigned(stream, &dc_coef);
    dc_coef += 8;
    *dc_coeff = dc_coef;
    next_coef = dc_coef;
  }

  size = size_id ? 64 : 16;
  scan = size == 16 ? scan4x4 : scan8x8;

  for (i = 0; i < size; i++) {
    /* scaling_list_delta_coef */
    tmp = HevcDecodeExpGolombSigned(stream, &delta);
    next_coef = (next_coef + delta + 256) & 0xFF;

    scaling_list[scan[i]] = next_coef;
  }

  return tmp;
}

u32 ScalingListData(u8 scaling_list[4][6][64], struct StrmData *stream) {

  u32 i, j, tmp;
  u32 pred_matrix_id_delta;

  /* size_id */
  for (i = 0; i < 4; i++) {
    /* matrix_id */
    for (j = 0; j < (i == 3 ? 2 : 6); j++) {
      /* scaling_list_pred_mode_flag */
      tmp = SwGetBits(stream, 1);
      if (!tmp) {
        tmp = HevcDecodeExpGolombUnsigned(stream, &pred_matrix_id_delta);
        /* ref matrix id shall be >= 0 */
        if (pred_matrix_id_delta > j) return HANTRO_NOK;
        if (pred_matrix_id_delta == 0) /* use default */
        {
          if (i == 0)
            DWLmemset(scaling_list[i][j], 16, list_size[i] * sizeof(u8));
          else
            DWLmemcpy(scaling_list[i][j], default_ptr[i][j],
                      list_size[i] * sizeof(u8));
        } else {
          DWLmemcpy(scaling_list[i][j],
                    scaling_list[i][j - pred_matrix_id_delta],
                    list_size[i] * sizeof(u8));
          if (i >= 2)
            scaling_list[0][0][16 + (i - 2) * 6 + j] =
                scaling_list[0][0][16 + (i - 2) * 6 + j - pred_matrix_id_delta];
        }
      } else {
        ScalingList(scaling_list[i][j], stream, i,
                    /* separate dc coeffs stored after first 4x4 matrix */
                    i >= 2 ? scaling_list[0][0] + 16 + (i - 2) * 6 + j : NULL);
      }
    }
  }
  return HANTRO_OK;
}

void DefaultScalingList(u8 scaling_list[4][6][64]) {

  u32 i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < (i == 3 ? 2 : 6); j++) {
      DWLmemcpy(scaling_list[i][j], default_ptr[i][j],
                list_size[i] * sizeof(u8));
    }
  }
}
