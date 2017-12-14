/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : ...
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp9hwd_headers.h"
#include "vp9hwd_probs.h"
#include "dwl.h"
#include "sw_debug.h"
#include "sw_stream.h"
#include "vp9_entropymode.h"

/* Define here if it's necessary to print errors */
#define DEC_HDRS_ERR(x)

#define VP9_KEY_FRAME_START_CODE_0 0x49
#define VP9_KEY_FRAME_START_CODE_1 0x83
#define VP9_KEY_FRAME_START_CODE_2 0x42

#define MIN_TILE_WIDTH 256
#define MAX_TILE_WIDTH 4096
#define MIN_TILE_WIDTH_SBS (MIN_TILE_WIDTH >> 6)
#define MAX_TILE_WIDTH_SBS (MAX_TILE_WIDTH >> 6)

static void DecodeSegmentationData(struct StrmData *rb, struct Vp9Decoder *dec);
static void DecodeLfParams(struct StrmData *rb, struct Vp9Decoder *dec);
static i32 DecodeQuantizerDelta(struct StrmData *rb);
static u32 ReadTileSize(const u8 *cx_size);
static void GetTileNBits(struct Vp9Decoder *dec, u32 *min_log2_ntiles_ptr,
                         u32 *delta_log2_ntiles);
static void ExistingRefRegister(struct Vp9Decoder *dec, u32 map);

void ExistingRefRegister(struct Vp9Decoder *dec, u32 map) {
  dec->existing_ref_map |= map;
}

void GetTileNBits(struct Vp9Decoder *dec, u32 *min_log2_ntiles_ptr,
                  u32 *delta_log2_ntiles) {
  const int sb_cols = (dec->width + 63) >> 6;
  u32 min_log2_ntiles, max_log2_ntiles;

  for (max_log2_ntiles = 0; (sb_cols >> max_log2_ntiles) >= MIN_TILE_WIDTH_SBS;
       max_log2_ntiles++) {
  }
  if (max_log2_ntiles > 0) max_log2_ntiles--;
  for (min_log2_ntiles = 0; (MAX_TILE_WIDTH_SBS << min_log2_ntiles) < sb_cols;
       min_log2_ntiles++) {
  }

  ASSERT(max_log2_ntiles >= min_log2_ntiles);
  *min_log2_ntiles_ptr = min_log2_ntiles;
  *delta_log2_ntiles = max_log2_ntiles - min_log2_ntiles;
}

void SetupFrameSize(struct StrmData *rb, struct Vp9Decoder *dec) {
  /* Frame width */
  dec->width = SwGetBits(rb, 16) + 1;
  STREAM_TRACE("frame_width", dec->width);

  /* Frame height */
  dec->height = SwGetBits(rb, 16) + 1;
  STREAM_TRACE("frame_height", dec->height);

  dec->scaling_active = SwGetBits(rb, 1);
  STREAM_TRACE("Scaling active", dec->scaling_active);

  if (dec->scaling_active) {
    /* Scaled frame width */
    dec->scaled_width = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("scaled_frame_width", dec->scaled_width);

    /* Scaled frame height */
    dec->scaled_height = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("scaled_frame_height", dec->scaled_height);
  }
}

void SetupFrameSizeWithRefs(struct StrmData *rb,
                            struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  struct DecAsicBuffers *asic_buff = dec_cont->asic_buff;
  u32 tmp, index;
  i32 found, i;
  u32 prev_width, prev_height;

  found = 0;
  dec->resolution_change = 0;
  prev_width = dec->width;
  prev_height = dec->height;
  for (i = 0; i < ALLOWED_REFS_PER_FRAME; ++i) {
    tmp = SwGetBits(rb, 1);
    STREAM_TRACE("use_prev_frame_size", tmp);
    if (tmp) {
      found = 1;
      /* Get resolution from frame buffer [active_ref_idx[i]] */
      index = Vp9BufferQueueGetRef(dec_cont->bq, dec->active_ref_idx[i]);
      dec->width = asic_buff->picture_info[index].coded_width;
      dec->height = asic_buff->picture_info[index].coded_height;
      STREAM_TRACE("frame_width", dec->width);
      STREAM_TRACE("frame_height", dec->height);
      break;
    }
  }

  if (!found) {
    /* Frame width */
    dec->width = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("frame_width", dec->width);

    /* Frame height */
    dec->height = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("frame_height", dec->height);
  }

  if (dec->width != prev_width || dec->height != prev_height)
    dec->resolution_change = 1; /* Signal resolution change for this frame */

  dec->scaling_active = SwGetBits(rb, 1);
  STREAM_TRACE("scaling active", dec->scaling_active);

  if (dec->scaling_active) {
    /* Scaled frame width */
    dec->scaled_width = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("scaled_frame_width", dec->scaled_width);

    /* Scaled frame height */
    dec->scaled_height = SwGetBits(rb, 16) + 1;
    STREAM_TRACE("scaled_frame_height", dec->scaled_height);
  }
}

u32 CheckSyncCode(struct StrmData *rb) {
  if (SwGetBits(rb, 8) != VP9_KEY_FRAME_START_CODE_0 ||
      SwGetBits(rb, 8) != VP9_KEY_FRAME_START_CODE_1 ||
      SwGetBits(rb, 8) != VP9_KEY_FRAME_START_CODE_2) {
    DEC_HDRS_ERR("VP9 Key-frame start code missing or invalid!\n");
    return HANTRO_NOK;
  }
  return HANTRO_OK;
}

#define RESERVED \
  if ((tmp = SwGetBits(&rb, 1))) STREAM_TRACE("Reserved bit, must be zero", tmp)

u32 Vp9DecodeFrameTag(const u8 *strm, u32 data_len,
                              const u8 *buf, u32 buf_len,
                              struct Vp9DecContainer *dec_cont) {
  struct Vp9Decoder *dec = &dec_cont->decoder;
  /* Use common bit parse but don't remove emulation prevention bytes. */
  struct StrmData rb = {buf, strm, 0, buf_len, data_len, 0, 1, 0, dec_cont->use_ringbuffer};
  u32 key_frame = 0;
  u32 err, tmp, i, delta_log2_tiles;

  tmp = SwGetBits(&rb, 2);
  STREAM_TRACE("Frame marker", tmp);
  if (tmp == END_OF_STREAM) return HANTRO_NOK;

  dec->vp_version = SwGetBits(&rb, 1);
  STREAM_TRACE("VP version", dec->vp_version);
  dec->vp_profile = (SwGetBits(&rb, 1)<<1) + dec->vp_version;
  if (dec->vp_profile>2) {
    dec->vp_profile += SwGetBits(&rb, 1);
  }

  dec->reset_frame_flags = 0;
  dec->show_existing_frame = SwGetBits(&rb, 1);
  STREAM_TRACE("Show existing frame", dec->show_existing_frame);
  if (dec->show_existing_frame) {
    if (dec_cont->dec_stat == VP9DEC_INITIALIZED)  return HANTRO_NOK;
    i32 idx_to_show = SwGetBits(&rb, 3);
    dec->show_existing_frame_index = dec->ref_frame_map[idx_to_show];
    dec->refresh_frame_flags = 0;
    dec->loop_filter_level = 0;
    return HANTRO_OK;
  }

  key_frame = SwGetBits(&rb, 1);
  dec->key_frame = !key_frame;
  dec->show_frame = SwGetBits(&rb, 1);
  dec->error_resilient = SwGetBits(&rb, 1);
  STREAM_TRACE("Key frame", dec->key_frame);
  STREAM_TRACE("Show frame", dec->show_frame);
  STREAM_TRACE("Error resilient", dec->error_resilient);

  if (dec->error_resilient == END_OF_STREAM) return HANTRO_NOK;

  if (dec->key_frame) {
    err = CheckSyncCode(&rb);
    if (err == HANTRO_NOK) return err;

    dec->bit_depth = dec->vp_profile >= 2 ? (SwGetBits(&rb, 1) ? 12 : 10) : 8;
    STREAM_TRACE("Bit depth", dec->bit_depth);
    if (dec->bit_depth == 12) return HANTRO_NOK;
    dec->color_space = SwGetBits(&rb, 3);
    STREAM_TRACE("Color space", dec->color_space);
    if (dec->color_space != 7) /* != s_rgb */
    {
      tmp = SwGetBits(&rb, 1);
      STREAM_TRACE("YUV range", tmp);
      if (dec->vp_version == 1) {
        dec->subsampling_x = SwGetBits(&rb, 1);
        dec->subsampling_y = SwGetBits(&rb, 1);
        tmp = SwGetBits(&rb, 1);
        STREAM_TRACE("Subsampling X", dec->subsampling_x);
        STREAM_TRACE("Subsampling Y", dec->subsampling_y);
        STREAM_TRACE("Alpha plane", tmp);
      } else {
        dec->subsampling_x = dec->subsampling_y = 1;
      }
    } else {
      if (dec->vp_version == 1) {
        dec->subsampling_x = dec->subsampling_y = 0;
        tmp = SwGetBits(&rb, 1);
        STREAM_TRACE("Alpha plane", tmp);
      } else {
        STREAM_TRACE("RGB not supported in profile", 0);
        return HANTRO_NOK;
      }
    }

    dec->reset_frame_flags = 1;
    //dec->refresh_frame_flags = (1 << ALLOWED_REFS_PER_FRAME) - 1;
    dec->refresh_frame_flags = (1 << NUM_REF_FRAMES) - 1;

    for (i = 0; i < ALLOWED_REFS_PER_FRAME; i++) {
      dec->active_ref_idx[i] = 0; /* TODO next free frame buffer */
      ExistingRefRegister(dec, 1<<i);
    }
    SetupFrameSize(&rb, dec);
  } else {
    if (!dec->show_frame) {
      dec->intra_only = SwGetBits(&rb, 1);
    } else {
      dec->intra_only = 0;
    }
    if (dec_cont->dec_stat == VP9DEC_INITIALIZED && dec->intra_only == 0) {
      /* we need an intra frame before decoding anything else */
      return HANTRO_NOK;
    }

    STREAM_TRACE("Intra only", dec->intra_only);

    if (!dec->error_resilient) {
      dec->reset_frame_context = SwGetBits(&rb, 2);
    } else {
      dec->reset_frame_context = 0;
    }
    STREAM_TRACE("Reset frame context", dec->reset_frame_context);

    if (dec->intra_only) {
      err = CheckSyncCode(&rb);
      if (err == HANTRO_NOK) return err;

      dec->bit_depth = dec->vp_profile>=2? SwGetBits(&rb, 1)? 12 : 10 : 8;
      if (dec->bit_depth == 12) return HANTRO_NOK;
      if (dec->vp_profile > 0) {
        dec->color_space = SwGetBits(&rb, 3);
        STREAM_TRACE("Color space", dec->color_space);
        if (dec->color_space != 7) /* != s_rgb */
        {
          tmp = SwGetBits(&rb, 1);
          STREAM_TRACE("YUV range", tmp);
          if (dec->vp_version == 1) {
            dec->subsampling_x = SwGetBits(&rb, 1);
            dec->subsampling_y = SwGetBits(&rb, 1);
            tmp = SwGetBits(&rb, 1);
            STREAM_TRACE("Subsampling X", dec->subsampling_x);
            STREAM_TRACE("Subsampling Y", dec->subsampling_y);
            STREAM_TRACE("Alpha plane", tmp);
          } else {
            dec->subsampling_x = dec->subsampling_y = 1;
          }
        } else {
          if (dec->vp_version == 1) {
            dec->subsampling_x = dec->subsampling_y = 0;
            tmp = SwGetBits(&rb, 1);
            STREAM_TRACE("Alpha plane", tmp);
          } else {
            STREAM_TRACE("RGB not supported in profile", 0);
            return HANTRO_NOK;
          }
        }
      }
      /* Refresh reference frame flags */
      dec->refresh_frame_flags = SwGetBits(&rb, NUM_REF_FRAMES);
      STREAM_TRACE("refresh_frame_flags", dec->refresh_frame_flags);
      SetupFrameSize(&rb, dec);
    } else {
      /* Refresh reference frame flags */
      dec->refresh_frame_flags = SwGetBits(&rb, NUM_REF_FRAMES);
      STREAM_TRACE("refresh_frame_flags", dec->refresh_frame_flags);

      for (i = 0; i < ALLOWED_REFS_PER_FRAME; i++) {
        i32 ref_frame_num = SwGetBits(&rb, NUM_REF_FRAMES_LG2);
        i32 mapped_ref = dec->ref_frame_map[ref_frame_num];
        STREAM_TRACE("active_reference_frame_idx", ref_frame_num);
        STREAM_TRACE("active_reference_frame_num", mapped_ref);
        dec->active_ref_idx[i] = mapped_ref;

        dec->ref_frame_sign_bias[i + 1] = SwGetBits(&rb, 1);
        STREAM_TRACE("ref_frame_sign_bias", dec->ref_frame_sign_bias[i + 1]);
      }

      SetupFrameSizeWithRefs(&rb, dec_cont);

      dec->allow_high_precision_mv = SwGetBits(&rb, 1);
      STREAM_TRACE("high_precision_mv", dec->allow_high_precision_mv);

      tmp = SwGetBits(&rb, 1);
      STREAM_TRACE("mb_switchable_mcomp_filt", tmp);
      if (tmp) {
        dec->mcomp_filter_type = SWITCHABLE;
      } else {
        dec->mcomp_filter_type = SwGetBits(&rb, 2);
        STREAM_TRACE("mcomp_filter_type", dec->mcomp_filter_type);
      }

      dec->allow_comp_inter_inter = 0;
      for (i = 0; i < ALLOWED_REFS_PER_FRAME; i++) {
        dec->allow_comp_inter_inter |=
            (i > 0) &&
            (dec->ref_frame_sign_bias[i + 1] != dec->ref_frame_sign_bias[1]);
      }

      /* Setup reference frames for compound prediction. */
      if (dec->allow_comp_inter_inter) {
        if (dec->ref_frame_sign_bias[LAST_FRAME] ==
            dec->ref_frame_sign_bias[GOLDEN_FRAME]) {
          dec->comp_fixed_ref = ALTREF_FRAME;
          dec->comp_var_ref[0] = LAST_FRAME;
          dec->comp_var_ref[1] = GOLDEN_FRAME;
        } else if (dec->ref_frame_sign_bias[LAST_FRAME] ==
                   dec->ref_frame_sign_bias[ALTREF_FRAME]) {
          dec->comp_fixed_ref = GOLDEN_FRAME;
          dec->comp_var_ref[0] = LAST_FRAME;
          dec->comp_var_ref[1] = ALTREF_FRAME;
        } else {
          dec->comp_fixed_ref = LAST_FRAME;
          dec->comp_var_ref[0] = GOLDEN_FRAME;
          dec->comp_var_ref[1] = ALTREF_FRAME;
        }
      }
    }
    ExistingRefRegister(dec, dec->refresh_frame_flags);
  }

  if (!dec->error_resilient) {
    /* Refresh entropy probs,
     * 0 == this frame probs are used only for this frame decoding,
     * 1 == this frame probs will be stored for future reference */
    dec->refresh_entropy_probs = SwGetBits(&rb, 1);
    dec->frame_parallel_decoding = SwGetBits(&rb, 1);
    STREAM_TRACE("Refresh frame context", dec->refresh_entropy_probs);
    STREAM_TRACE("Frame parallel", dec->frame_parallel_decoding);
  } else {
    dec->refresh_entropy_probs = 0;
    dec->frame_parallel_decoding = 1;
  }

  dec->frame_context_idx = SwGetBits(&rb, NUM_FRAME_CONTEXTS_LG2);
  STREAM_TRACE("Frame context index", dec->frame_context_idx);

  if (dec->key_frame || dec->error_resilient || dec->intra_only)
    Vp9ResetDecoder(&dec_cont->decoder, dec_cont->asic_buff);
  Vp9GetProbs(dec);

  /* Loop filter */
  DecodeLfParams(&rb, dec);

  /* Quantizers */
  dec->qp_yac = SwGetBits(&rb, 8);
  STREAM_TRACE("qp_y_ac", dec->qp_yac);
  dec->qp_ydc = DecodeQuantizerDelta(&rb);
  dec->qp_ch_dc = DecodeQuantizerDelta(&rb);
  dec->qp_ch_ac = DecodeQuantizerDelta(&rb);

  dec->lossless = dec->qp_yac == 0 && dec->qp_ydc == 0 && dec->qp_ch_dc == 0 &&
                  dec->qp_ch_ac == 0;

  /* Setup segment based adjustments */
  DecodeSegmentationData(&rb, dec);

  /* Tile dimensions */
  GetTileNBits(dec, &dec->log2_tile_columns, &delta_log2_tiles);
  while (delta_log2_tiles--) {
    tmp = SwGetBits(&rb, 1);
    if (tmp == END_OF_STREAM) return HANTRO_NOK;
    if (tmp) {
      dec->log2_tile_columns++;
    } else {
      break;
    }
  }
  STREAM_TRACE("log2_tile_columns", dec->log2_tile_columns);

  dec->log2_tile_rows = SwGetBits(&rb, 1);
  if (dec->log2_tile_rows) dec->log2_tile_rows += SwGetBits(&rb, 1);
  STREAM_TRACE("log2_tile_rows", dec->log2_tile_rows);

  /* Size of frame headers */
  dec->offset_to_dct_parts = SwGetBits(&rb, 16);
  if (dec->offset_to_dct_parts == END_OF_STREAM) return HANTRO_NOK;

  dec->frame_tag_size = (rb.strm_buff_read_bits + 7) / 8;

  STREAM_TRACE("First partition size", dec->offset_to_dct_parts);
  STREAM_TRACE("Frame tag size", dec->frame_tag_size);

  return HANTRO_OK;
}

u32 Vp9DecodeFrameHeader(const u8 *stream, u32 strm_len, struct VpBoolCoder *bc,
                                  const u8 *buffer, u32 buf_len, struct Vp9Decoder *dec) {
  u32 tmp, i, j, k;
  struct Vp9EntropyProbs *fc = &dec->entropy; /* Frame context */

  if (dec->width == 0 || dec->height == 0) {
    DEC_HDRS_ERR("Invalid size!\n");
    return HANTRO_NOK;
  }

  /* Store probs for context backward adaptation. */
  Vp9StoreAdaptProbs(dec);

  if(stream >= (buffer + buf_len))
    stream -= buf_len;

  /* Start bool coder */
  Vp9BoolStart(bc, stream, strm_len, buffer, buf_len);

  /* Setup transform mode and probs */
  if (dec->lossless) {
    dec->transform_mode = ONLY_4X4;
  } else {
    dec->transform_mode = Vp9ReadBits(bc, 2);
    STREAM_TRACE("transform_mode", dec->transform_mode);
    if (dec->transform_mode == ALLOW_32X32) {
      dec->transform_mode += Vp9ReadBits(bc, 1);
      STREAM_TRACE("transform_mode", dec->transform_mode);
    }
    if (dec->transform_mode == TX_MODE_SELECT) {
      for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
        for (j = 0; j < TX_SIZE_MAX_SB - 3; ++j) {
          tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
          STREAM_TRACE("tx8x8_prob_update", tmp);
          if (tmp) {
            u8 *prob = &fc->a.tx8x8_prob[i][j];
            *prob = Vp9ReadProbDiffUpdate(bc, *prob);
            STREAM_TRACE("tx8x8_prob", *prob);
          }
        }
      }

      for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
        for (j = 0; j < TX_SIZE_MAX_SB - 2; ++j) {
          tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
          STREAM_TRACE("tx16x16_prob_update", tmp);
          if (tmp) {
            u8 *prob = &fc->a.tx16x16_prob[i][j];
            *prob = Vp9ReadProbDiffUpdate(bc, *prob);
            STREAM_TRACE("tx16x16_prob", *prob);
          }
        }
      }

      for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
        for (j = 0; j < TX_SIZE_MAX_SB - 1; ++j) {
          tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
          STREAM_TRACE("tx32x32_prob_update", tmp);
          if (tmp) {
            u8 *prob = &fc->a.tx32x32_prob[i][j];
            *prob = Vp9ReadProbDiffUpdate(bc, *prob);
            STREAM_TRACE("tx32x32_prob", *prob);
          }
        }
      }
    }
  }

  /* Coefficient probability update */
  tmp = Vp9DecodeCoeffUpdate(bc, fc->a.prob_coeffs);
  if (tmp != HANTRO_OK) return (tmp);
  if (dec->transform_mode > ONLY_4X4) {
    tmp = Vp9DecodeCoeffUpdate(bc, fc->a.prob_coeffs8x8);
    if (tmp != HANTRO_OK) return (tmp);
  }
  if (dec->transform_mode > ALLOW_8X8) {
    tmp = Vp9DecodeCoeffUpdate(bc, fc->a.prob_coeffs16x16);
    if (tmp != HANTRO_OK) return (tmp);
  }
  if (dec->transform_mode > ALLOW_16X16) {
    tmp = Vp9DecodeCoeffUpdate(bc, fc->a.prob_coeffs32x32);
    if (tmp != HANTRO_OK) return (tmp);
  }

  dec->probs_decoded = 1;

  for (k = 0; k < MBSKIP_CONTEXTS; ++k) {
    tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
    STREAM_TRACE("mbskip_prob_update", tmp);
    if (tmp) {
      fc->a.mbskip_probs[k] = Vp9ReadProbDiffUpdate(bc, fc->a.mbskip_probs[k]);
      STREAM_TRACE("mbskip_prob", fc->a.mbskip_probs[k]);
    }
  }

  if (!dec->key_frame && !dec->intra_only) {
    for (i = 0; i < INTER_MODE_CONTEXTS; i++) {
      for (j = 0; j < VP9_INTER_MODES - 1; j++) {
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("inter_mode_prob_update", tmp);
        if (tmp) {
          u8 *prob = &fc->a.inter_mode_prob[i][j];
          *prob = Vp9ReadProbDiffUpdate(bc, *prob);
          STREAM_TRACE("inter_mode_prob", *prob);
        }
      }
    }

    if (dec->mcomp_filter_type == SWITCHABLE) {
      for (j = 0; j < VP9_SWITCHABLE_FILTERS + 1; ++j) {
        for (i = 0; i < VP9_SWITCHABLE_FILTERS - 1; ++i) {
          tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
          STREAM_TRACE("switchable_filter_prob_update", tmp);
          if (tmp) {
            u8 *prob = &fc->a.switchable_interp_prob[j][i];
            *prob = Vp9ReadProbDiffUpdate(bc, *prob);
            STREAM_TRACE("switchable_interp_prob", *prob);
          }
        }
      }
    }

    for (i = 0; i < INTRA_INTER_CONTEXTS; i++) {
      tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
      STREAM_TRACE("intra_inter_prob_update", tmp);
      if (tmp) {
        u8 *prob = &fc->a.intra_inter_prob[i];
        *prob = Vp9ReadProbDiffUpdate(bc, *prob);
        STREAM_TRACE("intra_inter_prob", *prob);
      }
    }

    /* Compound prediction mode probabilities */
    if (dec->allow_comp_inter_inter) {
      tmp = Vp9ReadBits(bc, 1);
      STREAM_TRACE("comp_pred_mode", tmp);
      dec->comp_pred_mode = tmp;
      if (tmp) {
        tmp = Vp9ReadBits(bc, 1);
        STREAM_TRACE("comp_pred_mode_hybrid", tmp);
        dec->comp_pred_mode += tmp;
        if (dec->comp_pred_mode == HYBRID_PREDICTION) {
          for (i = 0; i < COMP_INTER_CONTEXTS; i++) {
            tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
            STREAM_TRACE("comp_inter_prob_update", tmp);
            if (tmp) {
              u8 *prob = &fc->a.comp_inter_prob[i];
              *prob = Vp9ReadProbDiffUpdate(bc, *prob);
              STREAM_TRACE("comp_inter_prob", *prob);
            }
          }
        }
      }
    } else {
      dec->comp_pred_mode = SINGLE_PREDICTION_ONLY;
    }

    if (dec->comp_pred_mode != COMP_PREDICTION_ONLY) {
      for (i = 0; i < REF_CONTEXTS; i++) {
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("single_ref_prob_update", tmp);
        if (tmp) {
          u8 *prob = &fc->a.single_ref_prob[i][0];
          *prob = Vp9ReadProbDiffUpdate(bc, *prob);
          STREAM_TRACE("single_ref_prob", *prob);
        }
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("single_ref_prob_update", tmp);
        if (tmp) {
          u8 *prob = &fc->a.single_ref_prob[i][1];
          *prob = Vp9ReadProbDiffUpdate(bc, *prob);
          STREAM_TRACE("single_ref_prob", *prob);
        }
      }
    }

    if (dec->comp_pred_mode != SINGLE_PREDICTION_ONLY) {
      for (i = 0; i < REF_CONTEXTS; i++) {
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("comp_ref_prob_update", tmp);
        if (tmp) {
          u8 *prob = &fc->a.comp_ref_prob[i];
          *prob = Vp9ReadProbDiffUpdate(bc, *prob);
          STREAM_TRACE("comp_ref_prob", *prob);
        }
      }
    }

    /* Superblock intra luma pred mode probabilities */
    for (j = 0; j < BLOCK_SIZE_GROUPS; ++j) {
      for (i = 0; i < 8; ++i) {
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("ymode_prob_update", tmp);
        if (tmp) {
          fc->a.sb_ymode_prob[j][i] =
              Vp9ReadProbDiffUpdate(bc, fc->a.sb_ymode_prob[j][i]);
          STREAM_TRACE("ymode_prob", fc->a.sb_ymode_prob[j][i]);
        }
      }
      tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
      STREAM_TRACE("ymode_prob_update", tmp);
      if (tmp) {
        fc->a.sb_ymode_prob_b[j][0] =
            Vp9ReadProbDiffUpdate(bc, fc->a.sb_ymode_prob_b[j][0]);
        STREAM_TRACE("ymode_prob", fc->a.sb_ymode_prob_b[j][0]);
      }
    }

    for (j = 0; j < NUM_PARTITION_CONTEXTS; j++) {
      for (i = 0; i < PARTITION_TYPES - 1; i++) {
        tmp = Vp9DecodeBool(bc, VP9_DEF_UPDATE_PROB);
        STREAM_TRACE("partition_prob_update", tmp);
        if (tmp) {
          u8 *prob = &fc->a.partition_prob[INTER_FRAME][j][i];
          *prob = Vp9ReadProbDiffUpdate(bc, *prob);
          STREAM_TRACE("partition_prob", *prob);
        }
      }
    }

    /* Motion vector tree update */
    tmp = Vp9DecodeMvUpdate(bc, dec);
    if (tmp != HANTRO_OK) return (tmp);
  }

  STREAM_TRACE("decoded_header_bytes", bc->pos);

  /* When first tile row has zero-height skip it. Cannot be done when pic height
   * smaller than 3 SB rows, in this case more than one tile rows may be skipped
   * and needs to be handled in stream parsing. */
  if ((dec->height + 63) / 64 >= 3)
  {
    const u8 *strm = stream + dec->offset_to_dct_parts;
    if ((1 << dec->log2_tile_rows) > (dec->height + 63) / 64) {
      for (j = (1 << dec->log2_tile_columns); j; j--) {
        tmp = ReadTileSize(strm);
        strm += 4 + tmp;
        dec->offset_to_dct_parts += 4 + tmp;
        STREAM_TRACE("Tile row with h=0, skipping", tmp);
        if (strm > stream + strm_len) return HANTRO_NOK;
      }
    }
  }

  if (bc->strm_error) return (HANTRO_NOK);

  return (HANTRO_OK);
}

i32 DecodeQuantizerDelta(struct StrmData *rb) {
  u32 sign;
  i32 delta;

  if (SwGetBits(rb, 1)) {
    STREAM_TRACE("qp_delta_present", 1);
    delta = SwGetBits(rb, 4);
    STREAM_TRACE("qp_delta", delta);
    sign = SwGetBits(rb, 1);
    STREAM_TRACE("qp_delta_sign", sign);
    if (sign) delta = -delta;
    return delta;
  } else {
    STREAM_TRACE("qp_delta_present", 0);
    return 0;
  }
}

u32 Vp9SetPartitionOffsets(const u8 *stream, u32 len, struct Vp9Decoder *dec) {
  u32 offset = 0;
  u32 base_offset;
  u32 ret_val = HANTRO_OK;

  stream += dec->frame_tag_size;
  stream += dec->offset_to_dct_parts;

  base_offset = dec->frame_tag_size + dec->offset_to_dct_parts;

  dec->dct_partition_offsets[0] = base_offset + offset;
  if (dec->dct_partition_offsets[0] >= len) {
    dec->dct_partition_offsets[0] = len - 1;
    ret_val = HANTRO_NOK;
  }

  return ret_val;
}

u32 ReadTileSize(const u8 *cx_size) {
  u32 size;
  size = (u32)(DWLPrivateAreaReadByte(cx_size + 3)) +
         ((u32)(DWLPrivateAreaReadByte(cx_size + 2)) << 8) +
         ((u32)(DWLPrivateAreaReadByte(cx_size + 1)) << 16) +
         ((u32)(DWLPrivateAreaReadByte(cx_size + 0)) << 24);
  return size;
}

void DecodeSegmentationData(struct StrmData *rb, struct Vp9Decoder *dec) {
  u32 tmp, sign, i, j;
  struct Vp9EntropyProbs *fc = &dec->entropy; /* Frame context */

  /* Segmentation enabled? */
  dec->segment_enabled = SwGetBits(rb, 1);
  STREAM_TRACE("segment_enabled", dec->segment_enabled);

  dec->segment_map_update = 0;
  dec->segment_map_temporal_update = 0;
  if (!dec->segment_enabled) return;

  /* Segmentation map update */
  dec->segment_map_update = SwGetBits(rb, 1);
  STREAM_TRACE("segment_map_update", dec->segment_map_update);
  if (dec->segment_map_update) {
    for (i = 0; i < MB_SEG_TREE_PROBS; i++) {
      tmp = SwGetBits(rb, 1);
      STREAM_TRACE("segment_tree_prob_update", tmp);
      if (tmp) {
        fc->mb_segment_tree_probs[i] = SwGetBits(rb, 8);
        STREAM_TRACE("segment_tree_prob", tmp);
      } else {
        fc->mb_segment_tree_probs[i] = 255;
      }
    }

    /* Read the prediction probs needed to decode the segment id */
    dec->segment_map_temporal_update = SwGetBits(rb, 1);
    STREAM_TRACE("segment_map_temporal_update",
                 dec->segment_map_temporal_update);
    for (i = 0; i < PREDICTION_PROBS; i++) {
      if (dec->segment_map_temporal_update) {
        tmp = SwGetBits(rb, 1);
        STREAM_TRACE("segment_pred_prob_update", tmp);
        if (tmp) {
          fc->segment_pred_probs[i] = SwGetBits(rb, 8);
          STREAM_TRACE("segment_pred_prob", tmp);
        } else {
          fc->segment_pred_probs[i] = 255;
        }
      } else {
        fc->segment_pred_probs[i] = 255;
      }
    }
  }
  /* Segment feature data update */
  tmp = SwGetBits(rb, 1);
  STREAM_TRACE("segment_data_update", tmp);
  if (tmp) {
    /* Absolute/relative mode */
    dec->segment_feature_mode = SwGetBits(rb, 1);
    STREAM_TRACE("segment_abs_delta", dec->segment_feature_mode);

    /* Clear all previous segment data */
    DWLmemset(dec->segment_feature_enable, 0,
              sizeof(dec->segment_feature_enable));
    DWLmemset(dec->segment_feature_data, 0, sizeof(dec->segment_feature_data));

    for (i = 0; i < MAX_MB_SEGMENTS; i++) {
      for (j = 0; j < SEG_LVL_MAX; j++) {
        dec->segment_feature_enable[i][j] = SwGetBits(rb, 1);
        STREAM_TRACE("segment_feature_enable",
                     dec->segment_feature_enable[i][j]);

        if (dec->segment_feature_enable[i][j]) {
          /* Feature data, bits changes for every feature */
          dec->segment_feature_data[i][j] =
              SwGetBitsUnsignedMax(rb, vp9_seg_feature_data_max[j]);
          STREAM_TRACE("segment_feature_data", dec->segment_feature_data[i][j]);
          /* Sign if needed */
          if (vp9_seg_feature_data_signed[j]) {
            sign = SwGetBits(rb, 1);
            STREAM_TRACE("segment_feature_sign", sign);
            if (sign)
              dec->segment_feature_data[i][j] =
                  -dec->segment_feature_data[i][j];
          }
        }
      }
    }
  }
}

void DecodeLfParams(struct StrmData *rb, struct Vp9Decoder *dec) {
  u32 sign, tmp, j;

  if (dec->key_frame || dec->error_resilient || dec->intra_only) {
    DWLmemset(dec->mb_ref_lf_delta, 0, sizeof(dec->mb_ref_lf_delta));
    DWLmemset(dec->mb_mode_lf_delta, 0, sizeof(dec->mb_mode_lf_delta));
    dec->mb_ref_lf_delta[0] = 1;
    dec->mb_ref_lf_delta[1] = 0;
    dec->mb_ref_lf_delta[2] = -1;
    dec->mb_ref_lf_delta[3] = -1;
  }

  /* Loop filter adjustments */
  dec->loop_filter_level = SwGetBits(rb, 6);
  dec->loop_filter_sharpness = SwGetBits(rb, 3);
  STREAM_TRACE("loop_filter_level", dec->loop_filter_level);
  STREAM_TRACE("loop_filter_sharpness", dec->loop_filter_sharpness);

  /* Adjustments enabled? */
  dec->mode_ref_lf_enabled = SwGetBits(rb, 1);
  STREAM_TRACE("loop_filter_adj_enable", dec->mode_ref_lf_enabled);

  if (dec->mode_ref_lf_enabled) {
    /* Mode update? */
    tmp = SwGetBits(rb, 1);
    STREAM_TRACE("loop_filter_adj_update", tmp);
    if (tmp) {
      /* Reference frame deltas */
      for (j = 0; j < MAX_REF_LF_DELTAS; j++) {
        tmp = SwGetBits(rb, 1);
        STREAM_TRACE("ref_frame_delta_update", tmp);
        if (tmp) {
          /* Payload */
          tmp = SwGetBits(rb, 6);
          /* Sign */
          sign = SwGetBits(rb, 1);
          STREAM_TRACE("loop_filter_payload", tmp);
          STREAM_TRACE("loop_filter_sign", sign);

          dec->mb_ref_lf_delta[j] = tmp;
          if (sign) dec->mb_ref_lf_delta[j] = -dec->mb_ref_lf_delta[j];
        }
      }

      /* Mode deltas */
      for (j = 0; j < MAX_MODE_LF_DELTAS; j++) {
        tmp = SwGetBits(rb, 1);
        STREAM_TRACE("mb_type_delta_update", tmp);
        if (tmp) {
          /* Payload */
          tmp = SwGetBits(rb, 6);
          /* Sign */
          sign = SwGetBits(rb, 1);
          STREAM_TRACE("loop_filter_payload", tmp);
          STREAM_TRACE("loop_filter_sign", sign);

          dec->mb_mode_lf_delta[j] = tmp;
          if (sign) dec->mb_mode_lf_delta[j] = -dec->mb_mode_lf_delta[j];
        }
      }
    }
  } /* Mb mode/ref lf adjustment */
}
