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
#include "commonvp9.h"
#include "vp9hwd_probs.h"
#include "vp9_default_coef_probs.h"
#include "vp9_entropymode.h"
#include "vp9_entropymv.h"
#include "vp9_treecoder.h"
#include "dwl.h"
#include "sw_debug.h"

/* Define if it's necessary to trace probability updates */
/*#define TRACE_HDR_PROBS*/
/*#define TRACE_PROB_TABLES*/
/*#define COEF_COUNT_TESTING*/
/*#define MODE_COUNT_TESTING*/

/* Array indices are identical to previously-existing CONTEXT_NODE indices */

const vp9_tree_index vp9_coef_tree[22] =     /* corresponding _CONTEXT_NODEs */
    {-DCT_EOB_TOKEN, 2,                      /* 0 = EOB */
     -ZERO_TOKEN, 4,                         /* 1 = ZERO */
     -ONE_TOKEN, 6,                          /* 2 = ONE */
     8, 12,                                  /* 3 = LOW_VAL */
     -TWO_TOKEN, 10,                         /* 4 = TWO */
     -THREE_TOKEN, -FOUR_TOKEN,              /* 5 = THREE */
     14, 16,                                 /* 6 = HIGH_LOW */
     -DCT_VAL_CATEGORY1, -DCT_VAL_CATEGORY2, /* 7 = CAT_ONE */
     18, 20,                                 /* 8 = CAT_THREEFOUR */
     -DCT_VAL_CATEGORY3, -DCT_VAL_CATEGORY4, /* 9 = CAT_THREE */
     -DCT_VAL_CATEGORY5, -DCT_VAL_CATEGORY6  /* 10 = CAT_FIVE */
};

const vp9_tree_index vp9_coefmodel_tree[6] = {
    -DCT_EOB_MODEL_TOKEN, 2,          /* 0 = EOB */
    -ZERO_TOKEN,          4,          /* 1 = ZERO */
    -ONE_TOKEN,           -TWO_TOKEN, /* 2 = ONE */
};

void Vp9ResetProbs(struct Vp9Decoder *dec) {
  i32 i, j, k, l, m;

  Vp9InitModeContexts(dec);
  Vp9InitMbmodeProbs(dec);
  Vp9InitMvProbs(dec);

  /* Copy the default probs into two separate prob tables: part1 and part2. */
  for (i = 0; i < BLOCK_TYPES; i++) {
    for (j = 0; j < REF_TYPES; j++) {
      for (k = 0; k < COEF_BANDS; k++) {
        for (l = 0; l < PREV_COEF_CONTEXTS; l++) {
          if (l >= 3 && k == 0) continue;

          for (m = 0; m < UNCONSTRAINED_NODES; m++) {
            dec->entropy.a.prob_coeffs[i][j][k][l][m] =
                default_coef_probs_4x4[i][j][k][l][m];
            dec->entropy.a.prob_coeffs8x8[i][j][k][l][m] =
                default_coef_probs_8x8[i][j][k][l][m];
            dec->entropy.a.prob_coeffs16x16[i][j][k][l][m] =
                default_coef_probs_16x16[i][j][k][l][m];
            dec->entropy.a.prob_coeffs32x32[i][j][k][l][m] =
                default_coef_probs_32x32[i][j][k][l][m];
          }
        }
      }
    }
  }

  if (dec->key_frame || dec->error_resilient || dec->reset_frame_context == 3) {
    /* Store the default probs for all saved contexts */
    for (i = 0; i < NUM_FRAME_CONTEXTS; i++)
      DWLmemcpy(&dec->entropy_last[i], &dec->entropy,
                sizeof(struct Vp9EntropyProbs));
  } else if (dec->reset_frame_context == 2) {
    DWLmemcpy(&dec->entropy_last[dec->frame_context_idx], &dec->entropy,
              sizeof(struct Vp9EntropyProbs));
  }

#ifdef TRACE_PROB_TABLES
#define D 32
  {
    struct Vp9EntropyProbs p;
    struct Vp9EntropyCounts c;
    u32 o, s;
    printf("Probability structs:\n");
    printf("sizeof(struct Vp9EntropyProbs) = %d_b\n",
           sizeof(struct Vp9EntropyProbs));
    printf("sizeof(struct Vp9EntropyCounts) = %d_b\n",
           sizeof(struct Vp9EntropyCounts));
    printf("Prob tables aligned to addresses: base, firstbyte, size\n");
    o = (u8 *)(p.kf_bmode_prob) - (u8 *)&p;
    s = sizeof(p.kf_bmode_prob);
    printf("probs.kf_bmode_prob             %5d, %5d, %5dB\n", o / D, o % D, s);

    o = (u8 *)(p.kf_bmode_prob_b) - (u8 *)&p;
    s = sizeof(p.kf_bmode_prob_b);
    printf("probs.kf_bmode_prob_b            %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)(p.ref_pred_probs) - (u8 *)&p;
    s = sizeof(p.ref_pred_probs);
    printf("probs.ref_pred_probs            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.mb_segment_tree_probs) - (u8 *)&p;
    s = sizeof(p.mb_segment_tree_probs);
    printf("probs.mb_segment_tree_probs     %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.segment_pred_probs) - (u8 *)&p;
    s = sizeof(p.segment_pred_probs);
    printf("probs.segment_pred_probs        %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.ref_scores) - (u8 *)&p;
    s = sizeof(p.ref_scores);
    printf("probs.ref_scores                %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.prob_comppred) - (u8 *)&p;
    s = sizeof(p.prob_comppred);
    printf("probs.prob_comppred             %5d, %5d, %5dB\n", o / D, o % D, s);

    o = (u8 *)(p.kf_uv_mode_prob) - (u8 *)&p;
    s = sizeof(p.kf_uv_mode_prob);
    printf("probs.kf_uv_mode_prob           %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.kf_uv_mode_prob_b) - (u8 *)&p;
    s = sizeof(p.kf_uv_mode_prob_b);
    printf("probs.kf_uv_mode_prob_b          %5d, %5d, %5dB\n\n", o / D, o % D,
           s);

    o = (u8 *)(p.a.inter_mode_prob) - (u8 *)&p;
    s = sizeof(p.a.inter_mode_prob);
    printf("probs.a.inter_mode_prob         %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.intra_inter_prob) - (u8 *)&p;
    s = sizeof(p.a.intra_inter_prob);
    printf("probs.a.intra_inter_prob        %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.uv_mode_prob) - (u8 *)&p;
    s = sizeof(p.a.uv_mode_prob);
    printf("probs.a.uv_mode_prob            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.tx8x8_prob) - (u8 *)&p;
    s = sizeof(p.a.tx8x8_prob);
    printf("probs.a.tx8x8_prob              %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.tx16x16_prob) - (u8 *)&p;
    s = sizeof(p.a.tx16x16_prob);
    printf("probs.a.tx16x16_prob            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.tx32x32_prob) - (u8 *)&p;
    s = sizeof(p.a.tx32x32_prob);
    printf("probs.a.tx32x32_prob            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.sb_ymode_prob_b) - (u8 *)&p;
    s = sizeof(p.a.sb_ymode_prob_b);
    printf("probs.a.sb_ymode_prob_b          %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)(p.a.sb_ymode_prob) - (u8 *)&p;
    s = sizeof(p.a.sb_ymode_prob);
    printf("probs.a.sb_ymode_prob           %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.partition_prob) - (u8 *)&p;
    s = sizeof(p.a.partition_prob);
    printf("probs.a.partition_prob          %5d, %5d, %5dB\n", o / D, o % D, s);

    o = (u8 *)(p.a.uv_mode_prob_b) - (u8 *)&p;
    s = sizeof(p.a.uv_mode_prob_b);
    printf("probs.a.uv_mode_prob_b           %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)(p.a.switchable_interp_prob) - (u8 *)&p;
    s = sizeof(p.a.switchable_interp_prob);
    printf("probs.a.switchable_interp_prob  %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.comp_inter_prob) - (u8 *)&p;
    s = sizeof(p.a.comp_inter_prob);
    printf("probs.a.comp_inter_prob         %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.mbskip_probs) - (u8 *)&p;
    s = sizeof(p.a.mbskip_probs);
    printf("probs.a.mbskip_probs            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.joints) - (u8 *)&p;
    s = sizeof(p.a.nmvc.joints);
    printf("probs.a.nmvc.joints             %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.sign) - (u8 *)&p;
    s = sizeof(p.a.nmvc.sign);
    printf("probs.a.nmvc.sign               %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.class0) - (u8 *)&p;
    s = sizeof(p.a.nmvc.class0);
    printf("probs.a.nmvc.class0             %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.classes) - (u8 *)&p;
    s = sizeof(p.a.nmvc.classes);
    printf("probs.a.nmvc.classes            %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.class0_fp) - (u8 *)&p;
    s = sizeof(p.a.nmvc.class0_fp);
    printf("probs.a.nmvc.class0_fp          %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)&(p.a.nmvc.bits) - (u8 *)&p;
    s = sizeof(p.a.nmvc.bits);
    printf("probs.a.nmvc.bits               %5d, %5d, %5dB\n", o / D, o % D, s);

    o = (u8 *)(p.a.single_ref_prob) - (u8 *)&p;
    s = sizeof(p.a.single_ref_prob);
    printf("probs.a.single_ref_prob         %5d, %5d, %5dB\n", o / D, o % D, s);
    o = (u8 *)(p.a.comp_ref_prob) - (u8 *)&p;
    s = sizeof(p.a.comp_ref_prob);
    printf("probs.a.comp_ref_prob           %5d, %5d, %5dB\n", o / D, o % D, s);

    o = (u8 *)&(p.a.prob_coeffs) - (u8 *)&p;
    s = sizeof(p.a.prob_coeffs);
    printf("probs.a.prob_coeffs              %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)&(p.a.prob_coeffs8x8) - (u8 *)&p;
    s = sizeof(p.a.prob_coeffs8x8);
    printf("probs.a.prob_coeffs8x8           %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)&(p.a.prob_coeffs16x16) - (u8 *)&p;
    s = sizeof(p.a.prob_coeffs16x16);
    printf("probs.a.prob_coeffs16x16         %5d, %5d, %5dB\n", o / D, o % D,
           s);
    o = (u8 *)&(p.a.prob_coeffs32x32) - (u8 *)&p;
    s = sizeof(p.a.prob_coeffs32x32);
    printf("probs.a.prob_coeffs32x32         %5d, %5d, %5dB\n", o / D, o % D,
           s);
    printf("\n");

    printf(
        "Ctx counters aligned to addresses: base, first_counter, "
        "num_counters\n");
    o = (u8 *)&(c.inter_mode_counts) - (u8 *)&c;
    s = sizeof(c.inter_mode_counts);
    printf("ctr.inter_mode_counts           %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.sb_ymode_counts) - (u8 *)&c;
    s = sizeof(c.sb_ymode_counts);
    printf("ctr.sb_ymode_counts             %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.uv_mode_counts) - (u8 *)&c;
    s = sizeof(c.uv_mode_counts);
    printf("ctr.uv_mode_counts              %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.partition_counts) - (u8 *)&c;
    s = sizeof(c.partition_counts);
    printf("ctr.partition_counts            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.switchable_interp_counts) - (u8 *)&c;
    s = sizeof(c.switchable_interp_counts);
    printf("ctr.switchable_interp_counts    %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.intra_inter_count) - (u8 *)&c;
    s = sizeof(c.intra_inter_count);
    printf("ctr.intra_inter_count           %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.comp_inter_count) - (u8 *)&c;
    s = sizeof(c.comp_inter_count);
    printf("ctr.comp_inter_count            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.single_ref_count) - (u8 *)&c;
    s = sizeof(c.single_ref_count);
    printf("ctr.single_ref_count            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.comp_ref_count) - (u8 *)&c;
    s = sizeof(c.comp_ref_count);
    printf("ctr.comp_ref_count              %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.tx32x32_count) - (u8 *)&c;
    s = sizeof(c.tx32x32_count);
    printf("ctr.tx32x32_count               %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.tx16x16_count) - (u8 *)&c;
    s = sizeof(c.tx16x16_count);
    printf("ctr.tx16x16_count               %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.tx8x8_count) - (u8 *)&c;
    s = sizeof(c.tx8x8_count);
    printf("ctr.tx8x8_count                 %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.mbskip_count) - (u8 *)&c;
    s = sizeof(c.mbskip_count);
    printf("ctr.mbskip_count                %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.joints) - (u8 *)&c;
    s = sizeof(c.nmvcount.joints);
    printf("ctr.nmvcount.joints             %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.sign) - (u8 *)&c;
    s = sizeof(c.nmvcount.sign);
    printf("ctr.nmvcount.sign               %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.classes) - (u8 *)&c;
    s = sizeof(c.nmvcount.classes);
    printf("ctr.nmvcount.classes            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.class0) - (u8 *)&c;
    s = sizeof(c.nmvcount.class0);
    printf("ctr.nmvcount.class0             %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.bits) - (u8 *)&c;
    s = sizeof(c.nmvcount.bits);
    printf("ctr.nmvcount.bits               %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.class0_fp) - (u8 *)&c;
    s = sizeof(c.nmvcount.class0_fp);
    printf("ctr.nmvcount.class0_fp          %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.fp) - (u8 *)&c;
    s = sizeof(c.nmvcount.fp);
    printf("ctr.nmvcount.fp                 %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.class0_hp) - (u8 *)&c;
    s = sizeof(c.nmvcount.class0_hp);
    printf("ctr.nmvcount.class0_hp          %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.nmvcount.hp) - (u8 *)&c;
    s = sizeof(c.nmvcount.hp);
    printf("ctr.nmvcount.hp                 %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.count_coeffs) - (u8 *)&c;
    s = sizeof(c.count_coeffs);
    printf("ctr.count_coeffs                 %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.count_coeffs8x8) - (u8 *)&c;
    s = sizeof(c.count_coeffs8x8);
    printf("ctr.count_coeffs8x8              %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.count_coeffs16x16) - (u8 *)&c;
    s = sizeof(c.count_coeffs16x16);
    printf("ctr.count_coeffs16x16            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.count_coeffs32x32) - (u8 *)&c;
    s = sizeof(c.count_coeffs32x32);
    printf("ctr.count_coeffs32x32            %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    o = (u8 *)&(c.count_eobs) - (u8 *)&c;
    s = sizeof(c.count_eobs);
    printf("ctr.count_eobs                   %5d, %5d, %5d\n", o / D, o % D / 4,
           s / 4);
    printf("\n");
  }
#endif
}

void Vp9GetProbs(struct Vp9Decoder *dec) {
  /* Frame context tells which frame is used as reference, make
   * a copy of the context to use as base for this frame probs. */
  dec->entropy = dec->entropy_last[dec->frame_context_idx];
}

void Vp9StoreProbs(struct Vp9Decoder *dec) {
  /* After adaptation the probs refresh either ARF or IPF probs. */
  if (dec->refresh_entropy_probs) {
    dec->entropy_last[dec->frame_context_idx] = dec->entropy;
  }
}

void Vp9StoreAdaptProbs(struct Vp9Decoder *dec) {
  /* Adaptation is based on previous ctx before update. */
  dec->prev_ctx = dec->entropy.a;
}

static void UpdateNmv(struct VpBoolCoder *bc, vp9_prob *const p,
                      const vp9_prob upd_p) {
  u32 tmp = Vp9DecodeBool(bc, upd_p);
  if (tmp) {
    *p = (Vp9ReadBits(bc, 7) << 1) | 1;
    STREAM_TRACE("mv_prob", *p);
  }
}

u32 Vp9DecodeMvUpdate(struct VpBoolCoder *bc, struct Vp9Decoder *dec) {
  u32 i, j, k;
  struct NmvContext *mvctx = &dec->entropy.a.nmvc;

  for (j = 0; j < MV_JOINTS - 1; ++j) {
    UpdateNmv(bc, &mvctx->joints[j], VP9_NMV_UPDATE_PROB);
  }
  for (i = 0; i < 2; ++i) {
    UpdateNmv(bc, &mvctx->sign[i], VP9_NMV_UPDATE_PROB);
    for (j = 0; j < MV_CLASSES - 1; ++j) {
      UpdateNmv(bc, &mvctx->classes[i][j], VP9_NMV_UPDATE_PROB);
    }
    for (j = 0; j < CLASS0_SIZE - 1; ++j) {
      UpdateNmv(bc, &mvctx->class0[i][j], VP9_NMV_UPDATE_PROB);
    }
    for (j = 0; j < MV_OFFSET_BITS; ++j) {
      UpdateNmv(bc, &mvctx->bits[i][j], VP9_NMV_UPDATE_PROB);
    }
  }

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < CLASS0_SIZE; ++j) {
      for (k = 0; k < 3; ++k)
        UpdateNmv(bc, &mvctx->class0_fp[i][j][k], VP9_NMV_UPDATE_PROB);
    }
    for (j = 0; j < 3; ++j) {
      UpdateNmv(bc, &mvctx->fp[i][j], VP9_NMV_UPDATE_PROB);
    }
  }

  if (dec->allow_high_precision_mv) {
    for (i = 0; i < 2; ++i) {
      UpdateNmv(bc, &mvctx->class0_hp[i], VP9_NMV_UPDATE_PROB);
      UpdateNmv(bc, &mvctx->hp[i], VP9_NMV_UPDATE_PROB);
    }
  }

  return HANTRO_OK;
}

static int MergeIndex(int v, int n, int modulus) {
  int max1 = (n - 1 - modulus / 2) / modulus + 1;
  if (v < max1)
    v = v * modulus + modulus / 2;
  else {
    int w;
    v -= max1;
    w = v;
    v += (v + modulus - modulus / 2) / modulus;
    while (v % modulus == modulus / 2 ||
           w != v - (v + modulus - modulus / 2) / modulus)
      v++;
  }
  return v;
}

static int Vp9InvRecenterNonneg(int v, int m) {
  if (v > (m << 1))
    return v;
  else if ((v & 1) == 0)
    return (v >> 1) + m;
  else
    return m - ((v + 1) >> 1);
}

static int InvRemapProb(int v, int m) {
  const int n = 255;
  v = MergeIndex(v, n - 1, MODULUS_PARAM);
  m--;
  if ((m << 1) <= n) {
    return 1 + Vp9InvRecenterNonneg(v + 1, m);
  } else {
    return n - Vp9InvRecenterNonneg(v + 1, n - 1 - m);
  }
}

vp9_prob Vp9ReadProbDiffUpdate(struct VpBoolCoder *bc, int oldp) {
  int delp = Vp9DecodeSubExp(bc, 4, 255);
  return (vp9_prob)InvRemapProb(delp, oldp);
}

u32 Vp9DecodeCoeffUpdate(
    struct VpBoolCoder *bc,
    u8 prob_coeffs[BLOCK_TYPES][REF_TYPES][COEF_BANDS][PREV_COEF_CONTEXTS]
                  [ENTROPY_NODES_PART1]) {
  u32 i, j, k, l, m;
  u32 tmp;

  tmp = Vp9ReadBits(bc, 1);
  STREAM_TRACE("coeff_prob_update_flag", tmp);
  if (!tmp) return HANTRO_OK;

  for (i = 0; i < BLOCK_TYPES; i++) {
    for (j = 0; j < REF_TYPES; j++) {
      for (k = 0; k < COEF_BANDS; k++) {
        for (l = 0; l < PREV_COEF_CONTEXTS; l++) {
          if (l >= 3 && k == 0) continue;

          for (m = 0; m < UNCONSTRAINED_NODES; m++) {
            tmp = Vp9DecodeBool(bc, 252);
            CHECK_END_OF_STREAM(tmp);
            if (tmp) {
              u8 old, new;
              old = prob_coeffs[i][j][k][l][m];
              new = Vp9ReadProbDiffUpdate(bc, old);
              STREAM_TRACE("coeff_prob_delta_subexp", new);
              CHECK_END_OF_STREAM(tmp);
#ifdef TRACE_HDR_PROBS
              printf("hdr prob[%d][%d][%d][%d][%d] %d -> %d\n", i, j, k, l, m,
                     old, new);
#endif
              prob_coeffs[i][j][k][l][m] = new;
            }
          }
        }
      }
    }
  }
  return HANTRO_OK;
}

#define COEF_COUNT_SAT 24
#define COEF_MAX_UPDATE_FACTOR 112
#define COEF_COUNT_SAT_KEY 24
#define COEF_MAX_UPDATE_FACTOR_KEY 112
#define COEF_COUNT_SAT_AFTER_KEY 24
#define COEF_MAX_UPDATE_FACTOR_AFTER_KEY 128

static void UpdateCoefProbs(
    u8 dst_coef_probs[BLOCK_TYPES][REF_TYPES][COEF_BANDS][PREV_COEF_CONTEXTS]
                     [ENTROPY_NODES_PART1],
    u8 pre_coef_probs[BLOCK_TYPES][REF_TYPES][COEF_BANDS][PREV_COEF_CONTEXTS]
                     [ENTROPY_NODES_PART1],
    vp9_coeff_count *coef_counts,
    u32 (*eob_counts)[REF_TYPES][COEF_BANDS][PREV_COEF_CONTEXTS], int count_sat,
    int update_factor) {
  int t, i, j, k, l, count;
  unsigned int branch_ct[ENTROPY_NODES][2];
  vp9_prob coef_probs[ENTROPY_NODES];
  int factor;

  for (i = 0; i < BLOCK_TYPES; ++i)
    for (j = 0; j < REF_TYPES; ++j)
      for (k = 0; k < COEF_BANDS; ++k) {
        for (l = 0; l < PREV_COEF_CONTEXTS; ++l) {
          if (l >= 3 && k == 0) continue;
          Vp9TreeProbsFromDistribution(vp9_coefmodel_tree, coef_probs,
                                       branch_ct, coef_counts[i][j][k][l], 0);
          branch_ct[0][1] = eob_counts[i][j][k][l] - branch_ct[0][0];
          coef_probs[0] = GetBinaryProb(branch_ct[0][0], branch_ct[0][1]);
          for (t = 0; t < UNCONSTRAINED_NODES; ++t) {
            count = branch_ct[t][0] + branch_ct[t][1];
            count = count > count_sat ? count_sat : count;
            factor = (update_factor * count / count_sat);
            dst_coef_probs[i][j][k][l][t] = WeightedProb(
                pre_coef_probs[i][j][k][l][t], coef_probs[t], factor);
#ifdef TRACE_HDR_PROBS
            if (pre_coef_probs[i][j][k][l][t] != dst_coef_probs[i][j][k][l][t])
              printf("prob[%d][%d][%d][%d][%d] %d -> %d\n", i, j, k, l, t,
                     pre_coef_probs[i][j][k][l][t],
                     dst_coef_probs[i][j][k][l][t]);
#endif
          }
        }
      }
}

void Vp9AdaptCoefProbs(struct Vp9Decoder *cm) {
  int count_sat;
  int update_factor; /* denominator 256 */

  if (cm->key_frame || cm->intra_only) {
    update_factor = COEF_MAX_UPDATE_FACTOR_KEY;
    count_sat = COEF_COUNT_SAT_KEY;
  } else if (cm->prev_is_key_frame) {
    update_factor = COEF_MAX_UPDATE_FACTOR_AFTER_KEY; /* adapt quickly */
    count_sat = COEF_COUNT_SAT_AFTER_KEY;
  } else {
    update_factor = COEF_MAX_UPDATE_FACTOR;
    count_sat = COEF_COUNT_SAT;
  }

#ifdef COEF_COUNT_TESTING
  {
    i32 i, j, k, t;
    printf(
        "static const unsigned int\ncoef_counts"
        "[BLOCK_TYPES] [COEF_BANDS]"
        "[PREV_COEF_CONTEXTS] [UNCONSTRAINED_NODES+1] = {\n");
    for (i = 0; i < BLOCK_TYPES; ++i) {
      printf("  {\n");
      for (j = 0; j < COEF_BANDS; ++j) {
        printf("    {\n");
        for (k = 0; k < PREV_COEF_CONTEXTS; ++k) {
          printf("      {");
          for (t = 0; t < UNCONSTRAINED_NODES + 1; ++t)
            printf("%d, ", cm->ctx_ctr.count_coeffs[i][j][k][t]);
          printf("},\n");
        }
        printf("    },\n");
      }
      printf("  },\n");
    }
    printf("};\n");
    printf(
        "static const unsigned int\ncoef_counts_8x8"
        "[BLOCK_TYPES_8X8] [COEF_BANDS]"
        "[PREV_COEF_CONTEXTS] [UNCONSTRAINED_NODES+1] = {\n");
    for (i = 0; i < BLOCK_TYPES; ++i) {
      printf("  {\n");
      for (j = 0; j < COEF_BANDS; ++j) {
        printf("    {\n");
        for (k = 0; k < PREV_COEF_CONTEXTS; ++k) {
          printf("      {");
          for (t = 0; t < UNCONSTRAINED_NODES + 1; ++t)
            printf("%d, ", cm->ctx_ctr.count_coeffs8x8[i][j][k][t]);
          printf("},\n");
        }
        printf("    },\n");
      }
      printf("  },\n");
    }
    printf("};\n");
  }
#endif

  UpdateCoefProbs(cm->entropy.a.prob_coeffs, cm->prev_ctx.prob_coeffs,
                  cm->ctx_ctr.count_coeffs, cm->ctx_ctr.count_eobs[TX_4X4],
                  count_sat, update_factor);
  UpdateCoefProbs(cm->entropy.a.prob_coeffs8x8, cm->prev_ctx.prob_coeffs8x8,
                  cm->ctx_ctr.count_coeffs8x8, cm->ctx_ctr.count_eobs[TX_8X8],
                  count_sat, update_factor);
  UpdateCoefProbs(cm->entropy.a.prob_coeffs16x16, cm->prev_ctx.prob_coeffs16x16,
                  cm->ctx_ctr.count_coeffs16x16,
                  cm->ctx_ctr.count_eobs[TX_16X16], count_sat, update_factor);
  UpdateCoefProbs(cm->entropy.a.prob_coeffs32x32, cm->prev_ctx.prob_coeffs32x32,
                  cm->ctx_ctr.count_coeffs32x32,
                  cm->ctx_ctr.count_eobs[TX_32X32], count_sat, update_factor);
}

void tx_counts_to_branch_counts_32x32(unsigned int *tx_count_32x32p,
                                      unsigned int (*ct_32x32p)[2]) {
  ct_32x32p[0][0] = tx_count_32x32p[TX_4X4];
  ct_32x32p[0][1] = tx_count_32x32p[TX_8X8] + tx_count_32x32p[TX_16X16] +
                    tx_count_32x32p[TX_32X32];
  ct_32x32p[1][0] = tx_count_32x32p[TX_8X8];
  ct_32x32p[1][1] = tx_count_32x32p[TX_16X16] + tx_count_32x32p[TX_32X32];
  ct_32x32p[2][0] = tx_count_32x32p[TX_16X16];
  ct_32x32p[2][1] = tx_count_32x32p[TX_32X32];
}

void tx_counts_to_branch_counts_16x16(unsigned int *tx_count_16x16p,
                                      unsigned int (*ct_16x16p)[2]) {
  ct_16x16p[0][0] = tx_count_16x16p[TX_4X4];
  ct_16x16p[0][1] = tx_count_16x16p[TX_8X8] + tx_count_16x16p[TX_16X16];
  ct_16x16p[1][0] = tx_count_16x16p[TX_8X8];
  ct_16x16p[1][1] = tx_count_16x16p[TX_16X16];
}

void tx_counts_to_branch_counts_8x8(unsigned int *tx_count_8x8p,
                                    unsigned int (*ct_8x8p)[2]) {
  ct_8x8p[0][0] = tx_count_8x8p[TX_4X4];
  ct_8x8p[0][1] = tx_count_8x8p[TX_8X8];
}

#define MODE_COUNT_SAT 20
#define MODE_MAX_UPDATE_FACTOR 128
#define MAX_PROBS 32
static void UpdateModeProbs(int n_modes, const vp9_tree_index *tree,
                            unsigned int *cnt, vp9_prob *pre_probs,
                            vp9_prob *pre_probs_b, vp9_prob *dst_probs,
                            vp9_prob *dst_probs_b, u32 tok0_offset) {
  vp9_prob probs[MAX_PROBS];
  unsigned int branch_ct[MAX_PROBS][2];
  int t, count, factor;

  ASSERT(n_modes - 1 < MAX_PROBS);
  Vp9TreeProbsFromDistribution(tree, probs, branch_ct, cnt, tok0_offset);
  for (t = 0; t < n_modes - 1; ++t) {
    count = branch_ct[t][0] + branch_ct[t][1];
    count = count > MODE_COUNT_SAT ? MODE_COUNT_SAT : count;
    factor = (MODE_MAX_UPDATE_FACTOR * count / MODE_COUNT_SAT);
    if (t < 8 || dst_probs_b == NULL)
      dst_probs[t] = WeightedProb(pre_probs[t], probs[t], factor);
    else
      dst_probs_b[t - 8] = WeightedProb(pre_probs_b[t - 8], probs[t], factor);
  }
}

static int UpdateModeCt(vp9_prob pre_prob, vp9_prob prob,
                        unsigned int branch_ct[2]) {
  int factor, count = branch_ct[0] + branch_ct[1];
  count = count > MODE_COUNT_SAT ? MODE_COUNT_SAT : count;
  factor = (MODE_MAX_UPDATE_FACTOR * count / MODE_COUNT_SAT);
  return WeightedProb(pre_prob, prob, factor);
}

static int update_mode_ct2(vp9_prob pre_prob, unsigned int branch_ct[2]) {
  return UpdateModeCt(pre_prob, GetBinaryProb(branch_ct[0], branch_ct[1]),
                      branch_ct);
}

void Vp9AdaptModeProbs(struct Vp9Decoder *cm) {
  i32 i, j;
  struct Vp9AdaptiveEntropyProbs *fc = &cm->entropy.a;

#ifdef MODE_COUNT_TESTING
  int t;

  printf(
      "static const unsigned int\nymode_counts"
      "[VP9_INTRA_MODES] = {\n");
  for (t = 0; t < VP9_INTRA_MODES; ++t)
    printf("%d, ", cm->ctx_ctr.ymode_counts[t]);
  printf("};\n");
  printf(
      "static const unsigned int\nuv_mode_counts"
      "[VP9_INTRA_MODES] [VP9_INTRA_MODES] = {\n");
  for (i = 0; i < VP9_INTRA_MODES; ++i) {
    printf("  {");
    for (t = 0; t < VP9_INTRA_MODES; ++t)
      printf("%d, ", cm->ctx_ctr.uv_mode_counts[i][t]);
    printf("},\n");
  }
  printf("};\n");
  printf(
      "static const unsigned int\nbmode_counts"
      "[VP9_INTRA_MODES] = {\n");
  for (t = 0; t < VP9_INTRA_MODES; ++t)
    printf("%d, ", cm->ctx_ctr.bmode_counts[t]);
  printf("};\n");
  printf("static const unsigned int\ntx8x8_counts = {\n");
  for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
    for (j = 0; j < TX_SIZE_MAX_SB - 2; ++j) {
      printf("%d, ", cm->ctx_ctr.tx8x8_count[i][j]);
    }
    printf("\n");
  }
  printf("};\n");
  printf("static const unsigned int\ntx16x16_counts = {\n");
  for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
    for (j = 0; j < TX_SIZE_MAX_SB - 1; ++j) {
      printf("%d, ", cm->ctx_ctr.tx16x16_count[i][j]);
    }
    printf("\n");
  }
  printf("};\n");
  printf("static const unsigned int\ntx32x32_counts = {\n");
  for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
    for (j = 0; j < TX_SIZE_MAX_SB; ++j) {
      printf("%d, ", cm->ctx_ctr.tx32x32_count[i][j]);
    }
    printf("\n");
  }
  printf("};\n");
#endif

  for (i = 0; i < INTRA_INTER_CONTEXTS; i++)
    fc->intra_inter_prob[i] = update_mode_ct2(cm->prev_ctx.intra_inter_prob[i],
                                              cm->ctx_ctr.intra_inter_count[i]);
  for (i = 0; i < COMP_INTER_CONTEXTS; i++)
    fc->comp_inter_prob[i] = update_mode_ct2(cm->prev_ctx.comp_inter_prob[i],
                                             cm->ctx_ctr.comp_inter_count[i]);
  for (i = 0; i < REF_CONTEXTS; i++)
    fc->comp_ref_prob[i] = update_mode_ct2(cm->prev_ctx.comp_ref_prob[i],
                                           cm->ctx_ctr.comp_ref_count[i]);
  for (i = 0; i < REF_CONTEXTS; i++)
    for (j = 0; j < 2; j++)
      fc->single_ref_prob[i][j] =
          update_mode_ct2(cm->prev_ctx.single_ref_prob[i][j],
                          cm->ctx_ctr.single_ref_count[i][j]);

  for (i = 0; i < BLOCK_SIZE_GROUPS; ++i) {
    UpdateModeProbs(
        VP9_INTRA_MODES, vp9_intra_mode_tree, cm->ctx_ctr.sb_ymode_counts[i],
        cm->prev_ctx.sb_ymode_prob[i], cm->prev_ctx.sb_ymode_prob_b[i],
        cm->entropy.a.sb_ymode_prob[i], cm->entropy.a.sb_ymode_prob_b[i], 0);
  }
  for (i = 0; i < VP9_INTRA_MODES; ++i) {
    UpdateModeProbs(
        VP9_INTRA_MODES, vp9_intra_mode_tree, cm->ctx_ctr.uv_mode_counts[i],
        cm->prev_ctx.uv_mode_prob[i], cm->prev_ctx.uv_mode_prob_b[i],
        cm->entropy.a.uv_mode_prob[i], cm->entropy.a.uv_mode_prob_b[i], 0);
  }
  for (i = 0; i < NUM_PARTITION_CONTEXTS; i++)
    UpdateModeProbs(PARTITION_TYPES, vp9_partition_tree,
                    cm->ctx_ctr.partition_counts[i],
                    cm->prev_ctx.partition_prob[INTER_FRAME][i], NULL,
                    cm->entropy.a.partition_prob[INTER_FRAME][i], NULL, 0);

  if (cm->mcomp_filter_type == SWITCHABLE) {
    for (i = 0; i <= VP9_SWITCHABLE_FILTERS; ++i) {
      UpdateModeProbs(VP9_SWITCHABLE_FILTERS, vp9_switchable_interp_tree,
                      cm->ctx_ctr.switchable_interp_counts[i],
                      cm->prev_ctx.switchable_interp_prob[i], NULL,
                      cm->entropy.a.switchable_interp_prob[i], NULL, 0);
    }
  }
  if (cm->transform_mode == TX_MODE_SELECT) {
    int j;
    unsigned int branch_ct_8x8p[TX_SIZE_MAX_SB - 3][2];
    unsigned int branch_ct_16x16p[TX_SIZE_MAX_SB - 2][2];
    unsigned int branch_ct_32x32p[TX_SIZE_MAX_SB - 1][2];
    for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
      tx_counts_to_branch_counts_8x8(cm->ctx_ctr.tx8x8_count[i],
                                     branch_ct_8x8p);
      for (j = 0; j < TX_SIZE_MAX_SB - 3; ++j) {
        int factor;
        int count = branch_ct_8x8p[j][0] + branch_ct_8x8p[j][1];
        vp9_prob prob =
            GetBinaryProb(branch_ct_8x8p[j][0], branch_ct_8x8p[j][1]);
        count = count > MODE_COUNT_SAT ? MODE_COUNT_SAT : count;
        factor = (MODE_MAX_UPDATE_FACTOR * count / MODE_COUNT_SAT);
        fc->tx8x8_prob[i][j] =
            WeightedProb(cm->prev_ctx.tx8x8_prob[i][j], prob, factor);
      }
    }
    for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
      tx_counts_to_branch_counts_16x16(cm->ctx_ctr.tx16x16_count[i],
                                       branch_ct_16x16p);
      for (j = 0; j < TX_SIZE_MAX_SB - 2; ++j) {
        int factor;
        int count = branch_ct_16x16p[j][0] + branch_ct_16x16p[j][1];
        vp9_prob prob =
            GetBinaryProb(branch_ct_16x16p[j][0], branch_ct_16x16p[j][1]);
        count = count > MODE_COUNT_SAT ? MODE_COUNT_SAT : count;
        factor = (MODE_MAX_UPDATE_FACTOR * count / MODE_COUNT_SAT);
        fc->tx16x16_prob[i][j] =
            WeightedProb(cm->prev_ctx.tx16x16_prob[i][j], prob, factor);
      }
    }
    for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
      tx_counts_to_branch_counts_32x32(cm->ctx_ctr.tx32x32_count[i],
                                       branch_ct_32x32p);
      for (j = 0; j < TX_SIZE_MAX_SB - 1; ++j) {
        int factor;
        int count = branch_ct_32x32p[j][0] + branch_ct_32x32p[j][1];
        vp9_prob prob =
            GetBinaryProb(branch_ct_32x32p[j][0], branch_ct_32x32p[j][1]);
        count = count > MODE_COUNT_SAT ? MODE_COUNT_SAT : count;
        factor = (MODE_MAX_UPDATE_FACTOR * count / MODE_COUNT_SAT);
        fc->tx32x32_prob[i][j] =
            WeightedProb(cm->prev_ctx.tx32x32_prob[i][j], prob, factor);
      }
    }
  }
  for (i = 0; i < MBSKIP_CONTEXTS; ++i)
    fc->mbskip_probs[i] = update_mode_ct2(cm->prev_ctx.mbskip_probs[i],
                                          cm->ctx_ctr.mbskip_count[i]);
}

#define MVREF_COUNT_SAT 20
#define MVREF_MAX_UPDATE_FACTOR 128
void Vp9AdaptModeContext(struct Vp9Decoder *cm) {
  int i, j;
  u32(*mode_ct)[VP9_INTER_MODES - 1][2] = cm->ctx_ctr.inter_mode_counts;

  for (j = 0; j < INTER_MODE_CONTEXTS; j++) {
    for (i = 0; i < VP9_INTER_MODES - 1; i++) {
      int count = mode_ct[j][i][0] + mode_ct[j][i][1], factor;

      count = count > MVREF_COUNT_SAT ? MVREF_COUNT_SAT : count;
      factor = (MVREF_MAX_UPDATE_FACTOR * count / MVREF_COUNT_SAT);
      cm->entropy.a.inter_mode_prob[j][i] = WeightedProb(
          cm->prev_ctx.inter_mode_prob[j][i],
          GetBinaryProb(mode_ct[j][i][0], mode_ct[j][i][1]), factor);
    }
  }
}
