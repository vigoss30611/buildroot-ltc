/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Slice header decoding. */

#include "hevc_slice_header.h"
#include "hevc_exp_golomb.h"
#include "hevc_nal_unit.h"
#include "hevc_dpb.h"
#include "hevc_util.h"
#include "dwl.h"
#include "sw_util.h"

/* Decode slice header data from the stream. */
u32 HevcDecodeSliceHeader(struct StrmData* stream,
                          struct SliceHeader* slice_header,
                          struct SeqParamSet* seq_param_set,
                          struct PicParamSet* pic_param_set,
                          struct NalUnit* nal_unit) {

  u32 tmp, i, value;
  u32 read_bits;
  u32 first_slice_in_pic;
  u32 tmp_count;

  ASSERT(stream);
  ASSERT(slice_header);
  ASSERT(seq_param_set);
  ASSERT(pic_param_set);
  ASSERT(IS_SLICE_NAL_UNIT(nal_unit));

  (void)DWLmemset(slice_header, 0, sizeof(struct SliceHeader));

  /* first slice in pic */
  tmp = SwGetBits(stream, 1);
  first_slice_in_pic = tmp;

  if (IS_RAP_NAL_UNIT(nal_unit))
    slice_header->no_output_of_prior_pics_flag = SwGetBits(stream, 1);

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  slice_header->pic_parameter_set_id = value;

  /* SW only decodes the first slice in the pic -> return error if not
   * first slice.
   * TODO: error handling, if first slice is missing, should we read
   * something (poc?) to properly handle missing stuff */
  if (!first_slice_in_pic) return HANTRO_NOK;

  /* TODO: related to error handling */
  /*
  if (pic_param_set->dependent_slice_segments_enabled)
  {
      tmp = SwGetBits(stream, 1);
      slice_header->dependent_slice_flag = tmp;
  }
  pic_size_in_ctbs =
      NEXT_MULTIPLE(seq_param_set->pic_width,  max_cb_size) *
      NEXT_MULTIPLE(seq_param_set->pic_height, max_cb_size);

  tmp = SwNumBits(pic_size_in_ctbs);
  slice_header->slice_address = SwGetBits(stream, tmp);
  */

  /* if (!slice_header->dependent_slice_flag) */

  tmp = SwGetBits(stream, pic_param_set->num_extra_slice_header_bits);

  tmp = HevcDecodeExpGolombUnsigned(stream, &value);
  slice_header->slice_type = value;

  /* slice type has to be either I, P or B slice. Only I slice is
   * allowed when current NAL unit is an RAP NAL */
  if (!IS_I_SLICE(slice_header->slice_type) &&
      (IS_RAP_NAL_UNIT(nal_unit) ||
       seq_param_set->max_dec_pic_buffering[seq_param_set->max_sub_layers -
                                            1] == 0)) {
    ERROR_PRINT("slice_type");
    return (HANTRO_NOK);
  }

  /* to determine number of bits skipped by hw slice header decoding */
  read_bits = stream->strm_buff_read_bits;
  tmp_count = stream->emul_byte_count;
  stream->emul_byte_count = 0;
  ;

  if (pic_param_set->output_flag_present)
    slice_header->pic_output_flag = SwGetBits(stream, 1);

  /* Not supported yet
  if (seq_param_set->separate_colour_plane)
      slice_header->colour_plane_id = SwGetBits(stream, 2);
  */

  if (!IS_IDR_NAL_UNIT(nal_unit)) {
    tmp =
        SwGetBits(stream, SwNumBits(seq_param_set->max_pic_order_cnt_lsb - 1));
    slice_header->pic_order_cnt_lsb = tmp;

    slice_header->short_term_ref_pic_set_sps_flag = SwGetBits(stream, 1);
    if (!slice_header->short_term_ref_pic_set_sps_flag) {
      tmp = HevcDecodeShortTermRefPicSet(
          stream,
          /* decoded into last position in st_ref_pic_set array of sps */
          seq_param_set->st_ref_pic_set +
              seq_param_set->num_short_term_ref_pic_sets,
          1, seq_param_set->num_short_term_ref_pic_sets == 0);
      slice_header->short_term_ref_pic_set_idx =
          seq_param_set->num_short_term_ref_pic_sets;
      slice_header->st_ref_pic_set =
          seq_param_set
              ->st_ref_pic_set[slice_header->short_term_ref_pic_set_idx];
      DWLmemset(seq_param_set->st_ref_pic_set +
                    slice_header->short_term_ref_pic_set_idx,
                0, sizeof(struct StRefPicSet));

    } else {
      if (seq_param_set->num_short_term_ref_pic_sets > 1) {
        i = SwNumBits(seq_param_set->num_short_term_ref_pic_sets - 1);
        slice_header->short_term_ref_pic_set_idx = SwGetBits(stream, i);
      }
      slice_header->st_ref_pic_set =
          seq_param_set
              ->st_ref_pic_set[slice_header->short_term_ref_pic_set_idx];
    }

    if (seq_param_set->long_term_ref_pic_present) {
      u32 tot_long_term;
      u32 len;

      if (seq_param_set->num_long_term_ref_pics) {
        tmp = HevcDecodeExpGolombUnsigned(stream, &value);
        slice_header->num_long_term_sps = value;
      }
      tmp = HevcDecodeExpGolombUnsigned(stream, &value);
      slice_header->num_long_term_pics = value;

      tot_long_term =
          slice_header->num_long_term_sps + slice_header->num_long_term_pics;

      if (seq_param_set->num_long_term_ref_pics > 1)
        len = SwNumBits(seq_param_set->num_long_term_ref_pics - 1);
      else
        len = 0;
      for (i = 0; i < tot_long_term; i++) {
        if (i < slice_header->num_long_term_sps) {
          tmp = SwGetBits(stream, len);
          slice_header->lt_idx_sps[i] = tmp;
          slice_header->used_by_curr_pic_lt[i] =
              seq_param_set->used_by_curr_pic_lt[tmp];
        } else {
          slice_header->poc_lsb_lt[i] = SwGetBits(
              stream, SwNumBits(seq_param_set->max_pic_order_cnt_lsb - 1));
          slice_header->used_by_curr_pic_lt[i] = SwGetBits(stream, 1);
        }

        slice_header->delta_poc_msb_present_flag[i] = SwGetBits(stream, 1);

        if (slice_header->delta_poc_msb_present_flag[i]) {
          tmp = HevcDecodeExpGolombUnsigned(stream, &value);
          slice_header->delta_poc_msb_cycle_lt[i] = value;
        }
        /* delta_poc_msb_cycle is delta to previous value, long-term
         * pics from SPS and long-term pics specified here handled
         * separately */
        if (i && i != slice_header->num_long_term_sps)
          slice_header->delta_poc_msb_cycle_lt[i] +=
              slice_header->delta_poc_msb_cycle_lt[i - 1];
      }
    }
  }

  slice_header->hw_skip_bits = stream->strm_buff_read_bits - read_bits;
  slice_header->hw_skip_bits -= stream->emul_byte_count * 8;
  stream->emul_byte_count += tmp_count;

  return (HANTRO_OK);
}

/* Peek value of pic_parameter_set_id from the slice header. Function does not
 * modify current stream positions but copies the stream data structure to tmp
 * structure which is used while accessing stream data. */
u32 HevcCheckPpsId(struct StrmData* stream, u32* pic_param_set_id,
                   u32 is_rap_nal_unit) {

  u32 tmp, value;
  struct StrmData tmp_strm_data[1];

  ASSERT(stream);

  /* don't touch original stream position params */
  *tmp_strm_data = *stream;

  /* first slice in pic */
  tmp = SwGetBits(tmp_strm_data, 1);

  if (is_rap_nal_unit) tmp = SwGetBits(tmp_strm_data, 1);

  tmp = HevcDecodeExpGolombUnsigned(tmp_strm_data, &value);
  *pic_param_set_id = value;

  return (tmp);
}

u32 HevcCheckPriorPicsFlag(u32* no_output_of_prior_pics_flag,
                           const struct StrmData* stream) {

  u32 tmp;

  tmp = SwShowBits(stream, 2);

  *no_output_of_prior_pics_flag = tmp & 0x1;

  return (HANTRO_OK);
}
