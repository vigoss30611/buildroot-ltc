/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hevc_container.h"
#include "hevc_decoder.h"
#include "hevc_nal_unit.h"
#include "hevc_byte_stream.h"
#include "hevc_seq_param_set.h"
#include "hevc_pic_param_set.h"
#include "hevc_slice_header.h"
#include "hevc_util.h"
#include "hevc_dpb.h"
#include "hevc_exp_golomb.h"

/* Initializes the decoder instance. */
void HevcInit(struct Storage *storage, u32 no_output_reordering) {

  /* Variables */

  /* Code */

  ASSERT(storage);

  HevcInitStorage(storage);

  storage->no_reordering = no_output_reordering;
  storage->poc_last_display = INIT_POC;
}

/* check if pic needs to be skipped due to random access */
u32 SkipPicture(struct Storage *storage, struct NalUnit *nal_unit) {

  if ((nal_unit->nal_unit_type == NAL_CODED_SLICE_RASL_N ||
       nal_unit->nal_unit_type == NAL_CODED_SLICE_RASL_R) &&
      storage->poc->pic_order_cnt < storage->poc_last_display) {
    return HANTRO_TRUE;
  }
  /* CRA and not start of sequence -> all pictures may be output */
  if (storage->poc_last_display != INIT_POC &&
      nal_unit->nal_unit_type == NAL_CODED_SLICE_CRA)
    storage->poc_last_display = -INIT_POC;
  else if (IS_RAP_NAL_UNIT(nal_unit))
    storage->poc_last_display = storage->poc->pic_order_cnt;

  return HANTRO_FALSE;
}

/* Decode a NAL unit until a slice header. This function calls other modules
 * to perform tasks like
    - extract and decode NAL unit from the byte stream
    - decode parameter sets
    - decode slice header and slice data
    - conceal errors in the picture
    - perform deblocking filtering */
u32 HevcDecode(struct HevcDecContainer *dec_cont, const u8 *byte_strm, u32 strm_len,
               u32 pic_id, u32 *read_bytes) {

  u32 tmp, pps_id, sps_id;
  u32 access_unit_boundary_flag = HANTRO_FALSE;
  struct Storage *storage;
  struct NalUnit nal_unit;
  struct SeqParamSet seq_param_set;
  struct PicParamSet pic_param_set;
  struct StrmData strm;
  const u8 *strm_buf = dec_cont->hw_buffer;
  u32 buf_len = dec_cont->hw_buffer_length;

  u32 ret = 0;

  DEBUG_PRINT(("HevcDecode\n"));

  ASSERT(dec_cont);
  ASSERT(byte_strm);
  ASSERT(strm_buf);
  ASSERT(strm_len);
  ASSERT(buf_len);
  ASSERT(read_bytes);

  storage = &dec_cont->storage;
  ASSERT(storage);

  DEBUG_PRINT(
      ("Valid slice in access unit %d\n", storage->valid_slice_in_access_unit));

  strm.remove_emul3_byte = 0;
  strm.is_rb = dec_cont->use_ringbuffer;

  /* if previous buffer was not finished and same pointer given -> skip NAL
   * unit extraction */
  if (storage->prev_buf_not_finished &&
      byte_strm == storage->prev_buf_pointer) {
    strm = storage->strm[0];
    *read_bytes = storage->prev_bytes_consumed;
  } else {
    tmp = HevcExtractNalUnit(byte_strm, strm_len, strm_buf, buf_len, &strm,
                              read_bytes, &dec_cont->start_code_detected);
    if (tmp != HANTRO_OK) {
      ERROR_PRINT("BYTE_STREAM");
      return (HEVC_ERROR);
    }
    /* store stream */
    storage->strm[0] = strm;
    storage->prev_bytes_consumed = *read_bytes;
    storage->prev_buf_pointer = byte_strm;
  }

  storage->prev_buf_not_finished = HANTRO_FALSE;

  tmp = HevcDecodeNalUnit(&strm, &nal_unit);
  if (tmp != HANTRO_OK) {
    ret = HEVC_ERROR;
    goto NEXT_NAL;
  }

  /* Discard unspecified and  reserved */
  if ((nal_unit.nal_unit_type >= 10 && nal_unit.nal_unit_type <= 15) ||
      nal_unit.nal_unit_type >= 41) {
    DEBUG_PRINT(("DISCARDED NAL (UNSPECIFIED, RESERVED etc)\n"));
    ret = HEVC_RDY;
    goto NEXT_NAL;
  }

  if (!storage->checked_aub) {
    tmp = HevcCheckAccessUnitBoundary(&strm, &nal_unit, storage,
                                      &access_unit_boundary_flag);
    if (tmp != HANTRO_OK) {
      ERROR_PRINT("ACCESS UNIT BOUNDARY CHECK");
      ret = HEVC_ERROR;
      goto NEXT_NAL;
    }
  } else {
    storage->checked_aub = 0;
  }

  if (access_unit_boundary_flag) {
    DEBUG_PRINT(
        ("Access unit boundary, NAL TYPE %d\n", nal_unit.nal_unit_type));
    storage->valid_slice_in_access_unit = HANTRO_FALSE;
  }

  DEBUG_PRINT(("nal unit type: %d\n", nal_unit.nal_unit_type));

  switch (nal_unit.nal_unit_type) {
    case NAL_SEQ_PARAM_SET:
      DEBUG_PRINT(("SEQ PARAM SET\n"));
      tmp = HevcDecodeSeqParamSet(&strm, &seq_param_set);
      if (tmp != HANTRO_OK) {
        ERROR_PRINT("SEQ_PARAM_SET decoding");
        ret = HEVC_ERROR;
      } else {
        tmp = HevcStoreSeqParamSet(storage, &seq_param_set);
        if (tmp != HANTRO_OK) {
          ERROR_PRINT("SEQ_PARAM_SET allocation");
          ret = HEVC_ERROR;
        }
      }
      ret = HEVC_RDY;
      goto NEXT_NAL;

    case NAL_PIC_PARAM_SET:
      DEBUG_PRINT(("PIC PARAM SET\n"));
      tmp = HevcDecodePicParamSet(&strm, &pic_param_set);
      if (tmp != HANTRO_OK) {
        ERROR_PRINT("PIC_PARAM_SET decoding");
        ret = HEVC_ERROR;
      } else {
        tmp = HevcStorePicParamSet(storage, &pic_param_set);
        if (tmp != HANTRO_OK) {
          ERROR_PRINT("PIC_PARAM_SET allocation");
          ret = HEVC_ERROR;
        }
      }
      ret = HEVC_RDY;
      goto NEXT_NAL;

    case NAL_END_OF_SEQUENCE:
      storage->poc_last_display = INIT_POC;
      break;

    default:

      if (IS_SLICE_NAL_UNIT(&nal_unit)) {
        DEBUG_PRINT(("decode slice header\n"));

        storage->pic_started = HANTRO_TRUE;

        if (HevcIsStartOfPicture(storage)) {
          storage->current_pic_id = pic_id;

          tmp = HevcCheckPpsId(&strm, &pps_id, IS_RAP_NAL_UNIT(&nal_unit));
          ASSERT(tmp == HANTRO_OK);
          /* store old active_sps_id and return headers ready
           * indication if active_sps changes */
          sps_id = storage->active_sps_id;

          tmp = HevcActivateParamSets(storage, pps_id,
                                      IS_RAP_NAL_UNIT(&nal_unit));
          if (tmp != HANTRO_OK || storage->active_sps == NULL ||
              storage->active_pps == NULL) {
            ERROR_PRINT("Param set activation");
            ret = HEVC_PARAM_SET_ERROR;
            goto NEXT_NAL;
          }

          if (sps_id != storage->active_sps_id) {
            struct SeqParamSet *old_sps = NULL;
            /*struct SeqParamSet *new_sps = storage->active_sps;*/
            u32 no_output_of_prior_pics_flag = 1;

            if (storage->old_sps_id < MAX_NUM_SEQ_PARAM_SETS) {
              old_sps = storage->sps[storage->old_sps_id];
            }

            *read_bytes = 0;
            storage->prev_buf_not_finished = HANTRO_TRUE;

            if (IS_RAP_NAL_UNIT(&nal_unit)) {
              tmp =
                  HevcCheckPriorPicsFlag(&no_output_of_prior_pics_flag, &strm);
              /* TODO: why? */
              /*tmp = HANTRO_NOK;*/
            } else {
              tmp = HANTRO_NOK;
            }

            if ((tmp != HANTRO_OK) || (no_output_of_prior_pics_flag != 0) ||
                (nal_unit.nal_unit_type == NAL_CODED_SLICE_CRA) ||
                (storage->dpb->no_reordering) || (old_sps == NULL) /*||
                       (old_sps->pic_width != new_sps->pic_width) ||
                       (old_sps->pic_height != new_sps->pic_height) ||
                       (old_sps->max_dpb_size != new_sps->max_dpb_size)*/) {
              storage->dpb->flushed = 0;
            } else {
              HevcFlushDpb(storage->dpb);
            }

            storage->old_sps_id = storage->active_sps_id;
            storage->pic_started = HANTRO_FALSE;
            dec_cont->u32Flush = 1;
            return (HEVC_HDRS_RDY);
          }
        }

        tmp = HevcDecodeSliceHeader(&strm, storage->slice_header + 1,
                                    storage->active_sps, storage->active_pps,
                                    &nal_unit);
        if (tmp != HANTRO_OK) {
          ERROR_PRINT("SLICE_HEADER");
          ret = HEVC_ERROR;
          goto NEXT_NAL;
        }

        DEBUG_PRINT(("valid slice TRUE\n"));

        /* store slice header to storage if successfully decoded */
        storage->slice_header[0] = storage->slice_header[1];
        storage->prev_nal_unit[0] = nal_unit;

        HevcDecodePicOrderCnt(storage->poc,
                              storage->active_sps->max_pic_order_cnt_lsb,
                              storage->slice_header, &nal_unit);

        /* check if picture shall be skipped (non-decodable picture after
         * random access etc) */
        if (SkipPicture(storage, &nal_unit)) {
          ret = HEVC_RDY;
          goto NEXT_NAL;
        }

        HevcSetRefPics(storage->dpb, storage->slice_header,
                       storage->poc->pic_order_cnt, storage->active_sps,
                       IS_IDR_NAL_UNIT(&nal_unit),
                       nal_unit.nal_unit_type == NAL_CODED_SLICE_CRA &&
                           storage->no_rasl_output);

        if (nal_unit.nal_unit_type == NAL_CODED_SLICE_CRA &&
            storage->no_rasl_output)
          storage->no_rasl_output = 0;

        HevcDpbUpdateOutputList(storage->dpb);

        if (HevcIsStartOfPicture(storage)) {
          storage->curr_image->data = HevcAllocateDpbImage(
              storage->dpb, storage->poc->pic_order_cnt,
              storage->slice_header->pic_order_cnt_lsb,
              IS_IDR_NAL_UNIT(storage->prev_nal_unit), storage->current_pic_id,
              nal_unit.nal_unit_type == NAL_CODED_SLICE_TSA_R ||
                  nal_unit.nal_unit_type == NAL_CODED_SLICE_STSA_R);

#ifdef SET_EMPTY_PICTURE_DATA /* USE THIS ONLY FOR DEBUGGING PURPOSES */
          {
            i32 bgd = SET_EMPTY_PICTURE_DATA;

            DWLPrivateAreaMemset(storage->curr_image->data->virtual_address, bgd,
                      storage->curr_image->data->size);
          }
#endif

          if (storage->dpb->no_reordering) {
            /* output this picture next */
            struct DpbStorage *dpb = storage->dpb;
            struct DpbOutPicture *dpb_out = &dpb->out_buf[dpb->out_index_w];
            const struct DpbPicture *current_out = dpb->current_out;

            dpb_out->data = current_out->data;
            dpb_out->is_idr = current_out->is_idr;
            dpb_out->pic_id = current_out->pic_id;
            dpb_out->num_err_mbs = current_out->num_err_mbs;
            dpb_out->mem_idx = current_out->mem_idx;
            dpb_out->tiled_mode = current_out->tiled_mode;

            dpb_out->pic_width = dpb->pic_width;
            dpb_out->pic_height = dpb->pic_height;
            dpb_out->crop_params = dpb->crop_params;
            dpb_out->bit_depth_luma = dpb->bit_depth_luma;
            dpb_out->bit_depth_chroma = dpb->bit_depth_chroma;

            dpb->num_out++;
            dpb->out_index_w++;
            if (dpb->out_index_w == dpb->dpb_size + 1) dpb->out_index_w = 0;

            MarkTempOutput(dpb->fb_list, current_out->mem_idx);
          }
        }

        storage->valid_slice_in_access_unit = HANTRO_TRUE;

        return (HEVC_PIC_RDY);
      } else {
        DEBUG_PRINT(("SKIP DECODING %d\n", nal_unit.nal_unit_type));
        ret = HEVC_RDY;
        goto NEXT_NAL;
      }
  }

NEXT_NAL:
  if (HevcNextStartCode(&strm) == HANTRO_OK) {
    if (strm.strm_curr_pos < byte_strm)
      *read_bytes = (u32)(strm.strm_curr_pos - byte_strm + strm.strm_buff_size);
    else
      *read_bytes = (u32)(strm.strm_curr_pos - byte_strm);
    storage->prev_bytes_consumed = *read_bytes;
  }

  return ret;
}

/* Shutdown a decoder instance. Function frees all the memories allocated
 * for the decoder instance. */
void HevcShutdown(struct Storage *storage) {

  u32 i;

  ASSERT(storage);

  for (i = 0; i < MAX_NUM_SEQ_PARAM_SETS; i++) {
    if (storage->sps[i]) {
      FREE(storage->sps[i]);
    }
  }

  for (i = 0; i < MAX_NUM_PIC_PARAM_SETS; i++) {
    if (storage->pps[i]) {
      FREE(storage->pps[i]);
    }
  }
}

/* Get next output picture in display order. */
const struct DpbOutPicture *HevcNextOutputPicture(struct Storage *storage) {

  const struct DpbOutPicture *out;

  ASSERT(storage);

  out = HevcDpbOutputPicture(storage->dpb);

  return out;
}

/* Returns picture width in samples. */
u32 HevcPicWidth(struct Storage *storage) {

  ASSERT(storage);

  if (storage->active_sps) {
#if 0
    if (storage->raster_enabled)
      return ((storage->active_sps->pic_width + 15) & ~15);
    else
#endif
      return (storage->active_sps->pic_width);
  } else
    return (0);
}

/* Returns picture height in samples. */
u32 HevcPicHeight(struct Storage *storage) {

  ASSERT(storage);

  if (storage->active_sps)
    return (storage->active_sps->pic_height);
  else
    return (0);
}

/* Returns true if chroma format is 4:0:0. */
u32 HevcIsMonoChrome(struct Storage *storage) {

  ASSERT(storage);

  if (storage->active_sps)
    return (storage->active_sps->chroma_format_idc == 0);
  else
    return (0);
}

/* Flushes the decoded picture buffer, see dpb.c for details. */
void HevcFlushBuffer(struct Storage *storage) {

  ASSERT(storage);

  HevcFlushDpb(storage->dpb);
}

/* Get value of aspect_ratio_idc received in the VUI data. */
u32 HevcAspectRatioIdc(const struct Storage *storage) {

  const struct SeqParamSet *sps;

  ASSERT(storage);
  sps = storage->active_sps;

  if (sps && sps->vui_parameters_present_flag &&
      sps->vui_parameters.aspect_ratio_present_flag)
    return (sps->vui_parameters.aspect_ratio_idc);
  else /* default unspecified */
    return (0);
}

/* Get value of sample_aspect_ratio size received in the VUI data. */
void HevcSarSize(const struct Storage *storage, u32 *sar_width,
                 u32 *sar_height) {

  const struct SeqParamSet *sps;

  ASSERT(storage);
  sps = storage->active_sps;

  if (sps && storage->active_sps->vui_parameters_present_flag &&
      sps->vui_parameters.aspect_ratio_present_flag &&
      sps->vui_parameters.aspect_ratio_idc == 255) {
    *sar_width = sps->vui_parameters.sar_width;
    *sar_height = sps->vui_parameters.sar_height;
  } else {
    *sar_width = 0;
    *sar_height = 0;
  }
}

/* Get value of video_full_range_flag received in the VUI data. */
u32 HevcVideoRange(struct Storage *storage) {

  const struct SeqParamSet *sps;

  ASSERT(storage);
  sps = storage->active_sps;

  if (sps && sps->vui_parameters_present_flag &&
      sps->vui_parameters.video_signal_type_present_flag &&
      sps->vui_parameters.video_full_range_flag)
    return (1);
  else /* default value of video_full_range_flag is 0 */
    return (0);
}

/* Get value of matrix_coefficients received in the VUI data. */
u32 HevcMatrixCoefficients(struct Storage *storage) {

  const struct SeqParamSet *sps;

  ASSERT(storage);
  sps = storage->active_sps;

  if (sps && sps->vui_parameters_present_flag &&
      sps->vui_parameters.video_signal_type_present_flag &&
      sps->vui_parameters.colour_description_present_flag)
    return (sps->vui_parameters.matrix_coefficients);
  else /* default unspecified */
    return (2);
}

/* Get cropping parameters of the active SPS. */
void HevcCroppingParams(struct Storage *storage, u32 *cropping_flag,
                        u32 *left_offset, u32 *width, u32 *top_offset,
                        u32 *height) {

  const struct SeqParamSet *sps;
  u32 tmp1, tmp2;

  ASSERT(storage);
  sps = storage->active_sps;

  if (sps && sps->pic_cropping_flag) {
    tmp1 = 2;
    tmp2 = 1;
    *cropping_flag = 1;
    *left_offset = tmp1 * sps->pic_crop_left_offset;
    *width = sps->pic_width -
             tmp1 * (sps->pic_crop_left_offset + sps->pic_crop_right_offset);

    *top_offset = tmp1 * tmp2 * sps->pic_crop_top_offset;
    *height = sps->pic_height - tmp1 * tmp2 * (sps->pic_crop_top_offset +
                                               sps->pic_crop_bottom_offset);
  } else {
    *cropping_flag = 0;
    *left_offset = 0;
    *width = 0;
    *top_offset = 0;
    *height = 0;
  }
}

/* Returns luma bit depth. */
u32 HevcLumaBitDepth(struct Storage *storage) {

  ASSERT(storage);

  if (storage->active_sps)
    return (storage->active_sps->bit_depth_luma);
  else
    return (0);
}

/* Returns chroma bit depth. */
u32 HevcChromaBitDepth(struct Storage *storage) {

  ASSERT(storage);

  if (storage->active_sps)
    return (storage->active_sps->bit_depth_chroma);
  else
    return (0);
}
