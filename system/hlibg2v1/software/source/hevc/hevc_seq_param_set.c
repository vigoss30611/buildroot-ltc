/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Sequence Parameter Set decoding. */

#include "hevc_seq_param_set.h"
#include "hevc_exp_golomb.h"
#include "hevc_vui.h"
#include "hevc_cfg.h"
#include "hevc_byte_stream.h"
#include "hevc_util.h"
#include "dwl.h"
#include <stdlib.h>

/* enumeration to indicate invalid return value from the GetDpbSize function */
enum {
  INVALID_DPB_SIZE = 0x7FFFFFFF
};

u32 ScalingListData(u8 scaling_list[4][6][64], struct StrmData *stream);

/* comparison of elements in ref pic set, returns <0, 0 and >0 for cases when
 * delta_poc in first elem is smaller, equal and larger then second elem */
i32 CompareRefPicElems(const void *p1, const void *p2) {
  return (((struct StRefPicElem *)p1)->delta_poc -
          ((struct StRefPicElem *)p2)->delta_poc);
}

/* inverse of the previous */
i32 CompareRefPicElemsInv(const void *p1, const void *p2) {
  return (((struct StRefPicElem *)p2)->delta_poc -
          ((struct StRefPicElem *)p1)->delta_poc);
}

/* Decode short term reference picture set information from the stream. */
u32 HevcDecodeShortTermRefPicSet(struct StrmData *stream,
                                 struct StRefPicSet *st_rps, u32 slice_header,
                                 u32 first) {

  u32 tmp, i, j, value;
  u32 delta_idx;
  i32 delta_rps;
  i32 delta_poc;
  u32 num_delta_pocs;
  u32 use_delta, used_by_curr_pic;

  ASSERT(stream);
  ASSERT(st_rps);

  /* predict from another ref pic set */
  if (!first)
    tmp = SwGetBits(stream, 1);
  else
    tmp = 0;
  if (tmp) {
    if (slice_header)
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    else
      value = 0;
    delta_idx = value + 1;
    /* copy */
    *st_rps = *(st_rps - delta_idx);
    /* sign */
    delta_rps = SwGetBits(stream, 1);
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    delta_rps = delta_rps ? -(i32)(value + 1) : (i32)(value + 1);
    num_delta_pocs = st_rps->num_positive_pics + st_rps->num_negative_pics;
    for (i = 0, j = 0; i <= num_delta_pocs; i++) {
      used_by_curr_pic = SwGetBits(stream, 1);
      if (!used_by_curr_pic)
        use_delta = SwGetBits(stream, 1);
      else
        use_delta = 1;
      if (use_delta) {
        delta_poc =
            (i < num_delta_pocs ? st_rps->elem[i].delta_poc : 0) + delta_rps;
        st_rps->elem[j].delta_poc = delta_poc;
        st_rps->elem[j].used_by_curr_pic = used_by_curr_pic;
        j++;
      }
    }
    num_delta_pocs = j;
    /* sort whole list in ascending order */
    qsort(st_rps->elem, num_delta_pocs, sizeof(struct StRefPicElem),
          CompareRefPicElems);

    for (i = 0; i < num_delta_pocs; i++) {
      if (st_rps->elem[i].delta_poc > 0) break;
    }
    st_rps->num_negative_pics = i;
    st_rps->num_positive_pics = num_delta_pocs - i;

    /* flip the order of negative pocs */
    qsort(st_rps->elem, i, sizeof(struct StRefPicElem), CompareRefPicElemsInv);

    /*
    for (i = 0; i < num_delta_pocs; i++)
        printf("%d ", st_rps->elem[i].delta_poc);
    printf("\n");
    */
  } else {
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    st_rps->num_negative_pics = value;
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    st_rps->num_positive_pics = value;

    delta_poc = 0;
    for (i = 0, j = 0; i < st_rps->num_negative_pics; i++, j++) {
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
      delta_poc -= value + 1;
      st_rps->elem[j].delta_poc = delta_poc;
      tmp = SwGetBits(stream, 1);
      st_rps->elem[j].used_by_curr_pic = tmp;
    }
    delta_poc = 0;
    for (i = 0; i < st_rps->num_positive_pics; i++, j++) {
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
      delta_poc += value + 1;
      st_rps->elem[j].delta_poc = delta_poc;
      tmp = SwGetBits(stream, 1);
      st_rps->elem[j].used_by_curr_pic = tmp;
    }
  }

  return HANTRO_OK;
}

u32 ProfileAndLevel(struct StrmData *stream, struct Profile *p,
                    u32 profile_present, u32 max_num_sub_layers) {

  u32 i;
  u32 sub_profile_present[MAX_SUB_LAYERS], sub_level_present[MAX_SUB_LAYERS];

  if (profile_present) {
    p->general_profile_space = SwGetBits(stream, 2);
    p->general_tier_flag = SwGetBits(stream, 1);
    p->general_profile_idc = SwGetBits(stream, 5);
    p->general_profile_compatibility_flags = SwShowBits(stream, 32);
    SwFlushBits(stream, 32);
    p->progressive_source_flag = SwGetBits(stream, 1);
    p->interlaced_source_flag = SwGetBits(stream, 1);
    p->non_packed_constraint_flag = SwGetBits(stream, 1);
    p->frame_only_contraint_flag = SwGetBits(stream, 1);
    /* reserved_zero_44bits */
    (void)SwGetBits(stream, 16);
    (void)SwGetBits(stream, 16);
    (void)SwGetBits(stream, 12);
  }
  p->general_level_idc = SwGetBits(stream, 8);
  for (i = 0; i < max_num_sub_layers - 1; i++) {
    sub_profile_present[i] = SwGetBits(stream, 1);
    sub_level_present[i] = SwGetBits(stream, 1);
  }
  if (max_num_sub_layers > 1) {
    for (; i < 8; i++) /* reserved_zero_2bits */
      SwFlushBits(stream, 2);
  }

  for (i = 0; i < max_num_sub_layers - 1; i++) {
    if (sub_profile_present[i]) {
      p->sub_layer_profile_space[i] = SwGetBits(stream, 2);
      p->sub_layer_tier_flag[i] = SwGetBits(stream, 1);
      p->sub_layer_profile_idc[i] = SwGetBits(stream, 5);
      p->sub_layer_profile_compatibility_flags[i] = SwShowBits(stream, 32);
      SwFlushBits(stream, 32);
      p->sub_layer_progressive_source_flag[i] = SwGetBits(stream, 1);
      p->sub_layer_interlaced_source_flag[i] = SwGetBits(stream, 1);
      p->sub_layer_non_packed_constraint_flag[i] = SwGetBits(stream, 1);
      p->sub_layer_frame_only_contraint_flag[i] = SwGetBits(stream, 1);
      /* reserved_zero_44bits */
      (void)SwGetBits(stream, 16);
      (void)SwGetBits(stream, 16);
      (void)SwGetBits(stream, 12);
    }
    if (sub_level_present[i]) p->sub_layer_level_idc[i] = SwGetBits(stream, 8);
  }

  return HANTRO_OK;
}

/* Decode sequence parameter set information from the stream. */
u32 HevcDecodeSeqParamSet(struct StrmData *stream,
                          struct SeqParamSet *seq_param_set) {

  u32 tmp, i, value;
  u32 poc_bit_length;

  ASSERT(stream);
  ASSERT(seq_param_set);

  (void)DWLmemset(seq_param_set, 0, sizeof(struct SeqParamSet));

  /* video_parameter_set_id */
  tmp = SwGetBits(stream, 4);

  tmp = SwGetBits(stream, 3);
  seq_param_set->max_sub_layers = tmp + 1;

  tmp = SwGetBits(stream, 1);
  seq_param_set->temporal_id_nesting = tmp;

  tmp = ProfileAndLevel(stream, &seq_param_set->profile, 1,
                        seq_param_set->max_sub_layers);

  tmp =
      HevcDecodeExpGolombUnsigned(stream, &seq_param_set->seq_parameter_set_id);
  if (tmp != HANTRO_OK) return (tmp);
  if (seq_param_set->seq_parameter_set_id >= MAX_NUM_SEQ_PARAM_SETS) {
    ERROR_PRINT("seq_param_set_id");
    return (HANTRO_NOK);
  }

  /* chroma_format_idc */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  seq_param_set->chroma_format_idc = value;

  if (seq_param_set->chroma_format_idc == 3)
    seq_param_set->separate_colour_plane = SwGetBits(stream, 1);

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  seq_param_set->pic_width = value;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  seq_param_set->pic_height = value;

  tmp = SwGetBits(stream, 1);
  seq_param_set->pic_cropping_flag = (tmp == 1) ? HANTRO_TRUE : HANTRO_FALSE;

  if (seq_param_set->pic_cropping_flag) {
    u32 sub_width_c = 1, sub_height_c = 1;
    tmp = HevcDecodeExpGolombUnsigned(stream,
                                      &seq_param_set->pic_crop_left_offset);
    if (tmp != HANTRO_OK) return (tmp);
    tmp = HevcDecodeExpGolombUnsigned(stream,
                                      &seq_param_set->pic_crop_right_offset);
    if (tmp != HANTRO_OK) return (tmp);
    tmp = HevcDecodeExpGolombUnsigned(stream,
                                      &seq_param_set->pic_crop_top_offset);
    if (tmp != HANTRO_OK) return (tmp);
    tmp = HevcDecodeExpGolombUnsigned(stream,
                                      &seq_param_set->pic_crop_bottom_offset);
    if (tmp != HANTRO_OK) return (tmp);

    /* check that frame cropping params are valid, parameters shall
     * specify non-negative area within the original picture */
    if (((i32)seq_param_set->pic_crop_left_offset * sub_width_c >
         ((i32)seq_param_set->pic_width -
          ((i32)seq_param_set->pic_crop_right_offset * sub_width_c + 1))) ||
        ((i32)seq_param_set->pic_crop_top_offset * sub_height_c >
         ((i32)seq_param_set->pic_height -
          ((i32)seq_param_set->pic_crop_bottom_offset * sub_height_c + 1)))) {
      ERROR_PRINT("pic_cropping");
      return (HANTRO_NOK);
    }
  }

  /* bit_depth_luma_minus8 */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  seq_param_set->bit_depth_luma = value + 8;

  /* bit_depth_chroma_minus8 */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  seq_param_set->bit_depth_chroma = value + 8;

  /* log2_max_pic_order_cnt_lsb_minus4 */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  if (tmp != HANTRO_OK) return (tmp);
  if (value > 12) {
    ERROR_PRINT("log2_max_pic_order_cnt_lsb_minus4");
    return (HANTRO_NOK);
  }
  /* max_pic_order_cnt_lsb = 2^(log2_max_pic_order_cnt_lsb_minus4 + 4) */
  poc_bit_length = value + 4;
  seq_param_set->max_pic_order_cnt_lsb = 1 << (value + 4);

  tmp = SwGetBits(stream, 1);
  seq_param_set->sub_layer_ordering_info_present = tmp;

  i = seq_param_set->sub_layer_ordering_info_present
          ? 0
          : seq_param_set->max_sub_layers - 1;
  for (; i < seq_param_set->max_sub_layers; i++) {
    /* max_dec_pic_buffering */
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->max_dec_pic_buffering[i] = value;
    /* num_reorder_pics */
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->max_num_reorder_pics[i] = value;
    /* max_latency_increase */
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->max_latency_increase[i] = value;
  }

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->log_min_coding_block_size = (value + 3);
  /* diff_max_min */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->log_max_coding_block_size =
      seq_param_set->log_min_coding_block_size + value;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->log_min_transform_block_size = (value + 2);
  /* diff_max_min */
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->log_max_transform_block_size =
      seq_param_set->log_min_transform_block_size + value;

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->max_transform_hierarchy_depth_inter = value;
  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->max_transform_hierarchy_depth_intra = value;

  tmp = SwGetBits(stream, 1);
  seq_param_set->scaling_list_enable = tmp;

  if (seq_param_set->scaling_list_enable) {
    tmp = SwGetBits(stream, 1);
    seq_param_set->scaling_list_present_flag = tmp;

    if (seq_param_set->scaling_list_present_flag) {
      tmp = ScalingListData(seq_param_set->scaling_list, stream);
    }
  }

  tmp = SwGetBits(stream, 1);
  seq_param_set->asymmetric_motion_partitions_enable = tmp;

  tmp = SwGetBits(stream, 1);
  seq_param_set->sample_adaptive_offset_enable = tmp;

  tmp = SwGetBits(stream, 1);
  seq_param_set->pcm_enabled = tmp;

  if (seq_param_set->pcm_enabled) {
    tmp = SwGetBits(stream, 4);
    seq_param_set->pcm_bit_depth_luma = tmp + 1;
    tmp = SwGetBits(stream, 4);
    seq_param_set->pcm_bit_depth_chroma = tmp + 1;
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->log_min_pcm_block_size = (value + 3);
    /* diff_max_min */
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->log_max_pcm_block_size =
        seq_param_set->log_min_pcm_block_size + value;
    tmp = SwGetBits(stream, 1);
    seq_param_set->pcm_loop_filter_disable = tmp;
  }

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  seq_param_set->num_short_term_ref_pic_sets = value;
  for (i = 0; i < seq_param_set->num_short_term_ref_pic_sets; i++) {
    tmp = HevcDecodeShortTermRefPicSet(
        stream, seq_param_set->st_ref_pic_set + i, 0, i == 0);
  }

  tmp = SwGetBits(stream, 1);
  seq_param_set->long_term_ref_pic_present = tmp;

  if (seq_param_set->long_term_ref_pic_present) {
    tmp = HevcDecodeExpGolombUnsigned(stream, &value);
    seq_param_set->num_long_term_ref_pics = value;

    /*num_bits = SwNumBits(seq_param_set->num_long_term_ref_pics);*/
    for (i = 0; i < seq_param_set->num_long_term_ref_pics; i++) {
      seq_param_set->lt_ref_pic_poc_lsb[i] = SwGetBits(stream, poc_bit_length);
      seq_param_set->used_by_curr_pic_lt[i] = SwGetBits(stream, 1);
    }
  }

  tmp = SwGetBits(stream, 1);
  seq_param_set->temporal_mvp_enable = tmp;

  tmp = SwGetBits(stream, 1);
  seq_param_set->strong_intra_smoothing_enable = tmp;

  /* TODO: this is actually max_dec_pic_buffering_minus1 */
#if 0
  seq_param_set->max_dpb_size = MAX(
      1,
      seq_param_set->max_dec_pic_buffering[seq_param_set->max_sub_layers - 1]);
#else
  seq_param_set->max_dpb_size = 1;
#endif

  tmp = SwGetBits(stream, 1);
  if (tmp == END_OF_STREAM) return (HANTRO_NOK);
  seq_param_set->vui_parameters_present_flag = tmp;

  if (seq_param_set->vui_parameters_present_flag) {
    tmp = HevcDecodeVuiParameters(stream, &seq_param_set->vui_parameters);
    /* parts of VUI not decoded (not used/needed) -> need to skip to end of
     * NAL unit */
  }

  /* skip end of VUI params and sps_extension */
  if (seq_param_set->vui_parameters_present_flag || SwGetBits(stream, 1) == 1)
    tmp = HevcNextStartCode(stream);
  else
    tmp = HevcRbspTrailingBits(stream);

  /* ignore possible errors in trailing bits of parameters sets */
  return (HANTRO_OK);
}

/* Returns true if two sequence parameter sets are equal. */
u32 HevcCompareSeqParamSets(struct SeqParamSet *sps1,
                            struct SeqParamSet *sps2) {

  u32 i;
  u8 *p1, *p2;

  /* TODO: should we add memcmp to dwl? */

  p1 = (u8 *)sps1;
  p2 = (u8 *)sps2;
  for (i = 0; i < sizeof(struct SeqParamSet); i++) {
    if (*p1++ != *p2++) return 0;
  }

  return 1;
}
