/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vp9_entropymode.h"
#include "vp9_modecont.h"
#include "sw_debug.h"
#include <string.h>

static const vp9_prob
    default_kf_uv_probs[VP9_INTRA_MODES][VP9_INTRA_MODES - 1] = {
        {144, 11, 54, 157, 195, 130, 46, 58, 108} /* y = dc */,
        {118, 15, 123, 148, 131, 101, 44, 93, 131} /* y = v */,
        {113, 12, 23, 188, 226, 142, 26, 32, 125} /* y = h */,
        {120, 11, 50, 123, 163, 135, 64, 77, 103} /* y = d45 */,
        {113, 9, 36, 155, 111, 157, 32, 44, 161} /* y = d135 */,
        {116, 9, 55, 176, 76, 96, 37, 61, 149} /* y = d117 */,
        {115, 9, 28, 141, 161, 167, 21, 25, 193} /* y = d153 */,
        {120, 12, 32, 145, 195, 142, 32, 38, 86} /* y = d27 */,
        {116, 12, 64, 120, 140, 125, 49, 115, 121} /* y = d63 */,
        {102, 19, 66, 162, 182, 122, 35, 59, 128} /* y = tm */
};

static const vp9_prob
    default_if_y_probs[BLOCK_SIZE_GROUPS][VP9_INTRA_MODES - 1] = {
        {65, 32, 18, 144, 162, 194, 41, 51, 98} /* block_size < 8x8 */,
        {132, 68, 18, 165, 217, 196, 45, 40, 78} /* block_size < 16x16 */,
        {173, 80, 19, 176, 240, 193, 64, 35, 46} /* block_size < 32x32 */,
        {221, 135, 38, 194, 248, 121, 96, 85, 29} /* block_size >= 32x32 */
};

static const vp9_prob
    default_if_uv_probs[VP9_INTRA_MODES][VP9_INTRA_MODES - 1] = {
        {120, 7, 76, 176, 208, 126, 28, 54, 103} /* y = dc */,
        {48, 12, 154, 155, 139, 90, 34, 117, 119} /* y = v */,
        {67, 6, 25, 204, 243, 158, 13, 21, 96} /* y = h */,
        {97, 5, 44, 131, 176, 139, 48, 68, 97} /* y = d45 */,
        {83, 5, 42, 156, 111, 152, 26, 49, 152} /* y = d135 */,
        {80, 5, 58, 178, 74, 83, 33, 62, 145} /* y = d117 */,
        {86, 5, 32, 154, 192, 168, 14, 22, 163} /* y = d153 */,
        {85, 5, 32, 156, 216, 148, 19, 29, 73} /* y = d27 */,
        {77, 7, 64, 116, 132, 122, 37, 126, 120} /* y = d63 */,
        {101, 21, 107, 181, 192, 103, 19, 67, 125} /* y = tm */
};

const vp9_prob vp9_partition_probs
    [NUM_FRAME_TYPES][NUM_PARTITION_CONTEXTS][PARTITION_TYPES] =
        { /* 1 byte padding */
         {/* frame_type = keyframe */
          /* 8x8 -> 4x4 */
          {158, 97, 94, 0} /* a/l both not split */,
          {93, 24, 99, 0} /* a split, l not split */,
          {85, 119, 44, 0} /* l split, a not split */,
          {62, 59, 67, 0} /* a/l both split */,
          /* 16x16 -> 8x8 */
          {149, 53, 53, 0} /* a/l both not split */,
          {94, 20, 48, 0} /* a split, l not split */,
          {83, 53, 24, 0} /* l split, a not split */,
          {52, 18, 18, 0} /* a/l both split */,
          /* 32x32 -> 16x16 */
          {150, 40, 39, 0} /* a/l both not split */,
          {78, 12, 26, 0} /* a split, l not split */,
          {67, 33, 11, 0} /* l split, a not split */,
          {24, 7, 5, 0} /* a/l both split */,
          /* 64x64 -> 32x32 */
          {174, 35, 49, 0} /* a/l both not split */,
          {68, 11, 27, 0} /* a split, l not split */,
          {57, 15, 9, 0} /* l split, a not split */,
          {12, 3, 3, 0} /* a/l both split */
         },
         {/* frame_type = interframe */
          /* 8x8 -> 4x4 */
          {199, 122, 141, 0} /* a/l both not split */,
          {147, 63, 159, 0} /* a split, l not split */,
          {148, 133, 118, 0} /* l split, a not split */,
          {121, 104, 114, 0} /* a/l both split */,
          /* 16x16 -> 8x8 */
          {174, 73, 87, 0} /* a/l both not split */,
          {92, 41, 83, 0} /* a split, l not split */,
          {82, 99, 50, 0} /* l split, a not split */,
          {53, 39, 39, 0} /* a/l both split */,
          /* 32x32 -> 16x16 */
          {177, 58, 59, 0} /* a/l both not split */,
          {68, 26, 63, 0} /* a split, l not split */,
          {52, 79, 25, 0} /* l split, a not split */,
          {17, 14, 12, 0} /* a/l both split */,
          /* 64x64 -> 32x32 */
          {222, 34, 30, 0} /* a/l both not split */,
          {72, 16, 44, 0} /* a split, l not split */,
          {58, 32, 12, 0} /* l split, a not split */,
          {10, 7, 6, 0} /* a/l both split */
         }};

/* Array indices are identical to previously-existing INTRAMODECONTEXTNODES. */

const vp9_tree_index vp9_intra_mode_tree[VP9_INTRA_MODES * 2 - 2] = {
    -DC_PRED, 2,            /* 0 = DC_NODE */
    -TM_PRED, 4,            /* 1 = TM_NODE */
    -V_PRED, 6,             /* 2 = V_NODE */
    8, 12,                  /* 3 = COM_NODE */
    -H_PRED, 10,            /* 4 = H_NODE */
    -D135_PRED, -D117_PRED, /* 5 = D135_NODE */
    -D45_PRED, 14,          /* 6 = D45_NODE */
    -D63_PRED, 16,          /* 7 = D63_NODE */
    -D153_PRED, -D27_PRED   /* 8 = D153_NODE */
};

const vp9_tree_index vp9_sb_mv_ref_tree[6] = {-ZEROMV, 2,       -NEARESTMV,
                                              4,       -NEARMV, -NEWMV};

const vp9_tree_index vp9_partition_tree[6] = {
    -PARTITION_NONE, 2, -PARTITION_HORZ, 4, -PARTITION_VERT, -PARTITION_SPLIT};

struct vp9_token vp9_sb_mv_ref_encoding_array[VP9_INTER_MODES];

static const vp9_prob default_intra_inter_p[INTRA_INTER_CONTEXTS] = {9,   102,
                                                                     187, 225};

static const vp9_prob default_comp_inter_p[COMP_INTER_CONTEXTS] = {
    239, 183, 119, 96, 41};

static const vp9_prob default_comp_ref_p[REF_CONTEXTS] = {50,  126, 123,
                                                          221, 226};

static const vp9_prob default_single_ref_p[REF_CONTEXTS][2] = {
    {33, 16}, {77, 74}, {142, 142}, {172, 170}, {238, 247}};

const vp9_prob vp9_default_tx_probs_32x32p
    [TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 1] = {{3, 136, 37, }, {5, 52, 13, }, };
const vp9_prob vp9_default_tx_probs_16x16p
    [TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 2] = {{20, 152, }, {15, 101, }, };
const vp9_prob vp9_default_tx_probs_8x8p[TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 3] =
    {{100, }, {66, }, };

const vp9_prob vp9_default_mbskip_probs[MBSKIP_CONTEXTS] = {192, 128, 64};

void Vp9InitMbmodeProbs(struct Vp9Decoder *x) {
  int i, j;

  for (i = 0; i < BLOCK_SIZE_GROUPS; i++) {
    for (j = 0; j < 8; j++)
      x->entropy.a.sb_ymode_prob[i][j] = default_if_y_probs[i][j];
    x->entropy.a.sb_ymode_prob_b[i][0] = default_if_y_probs[i][8];
  }

  for (i = 0; i < VP9_INTRA_MODES; i++) {
    for (j = 0; j < 8; j++)
      x->entropy.kf_uv_mode_prob[i][j] = default_kf_uv_probs[i][j];
    x->entropy.kf_uv_mode_prob_b[i][0] = default_kf_uv_probs[i][8];

    for (j = 0; j < 8; j++)
      x->entropy.a.uv_mode_prob[i][j] = default_if_uv_probs[i][j];
    x->entropy.a.uv_mode_prob_b[i][0] = default_if_uv_probs[i][8];
  }

  memcpy(x->entropy.a.switchable_interp_prob, vp9_switchable_interp_prob,
         sizeof(vp9_switchable_interp_prob));

  memcpy(x->entropy.a.partition_prob, vp9_partition_probs,
         sizeof(vp9_partition_probs));

  memcpy(x->entropy.a.intra_inter_prob, default_intra_inter_p,
         sizeof(default_intra_inter_p));
  memcpy(x->entropy.a.comp_inter_prob, default_comp_inter_p,
         sizeof(default_comp_inter_p));
  memcpy(x->entropy.a.comp_ref_prob, default_comp_ref_p,
         sizeof(default_comp_ref_p));
  memcpy(x->entropy.a.single_ref_prob, default_single_ref_p,
         sizeof(default_single_ref_p));
  memcpy(x->entropy.a.tx32x32_prob, vp9_default_tx_probs_32x32p,
         sizeof(vp9_default_tx_probs_32x32p));
  memcpy(x->entropy.a.tx16x16_prob, vp9_default_tx_probs_16x16p,
         sizeof(vp9_default_tx_probs_16x16p));
  memcpy(x->entropy.a.tx8x8_prob, vp9_default_tx_probs_8x8p,
         sizeof(vp9_default_tx_probs_8x8p));
  memcpy(x->entropy.a.mbskip_probs, vp9_default_mbskip_probs,
         sizeof(vp9_default_mbskip_probs));

  for (i = 0; i < VP9_INTRA_MODES; i++) {
    for (j = 0; j < VP9_INTRA_MODES; j++) {
      memcpy(x->entropy.kf_bmode_prob[i][j], vp9_kf_default_bmode_probs[i][j],
             8);
      x->entropy.kf_bmode_prob_b[i][j][0] = vp9_kf_default_bmode_probs[i][j][8];
    }
  }
}

const vp9_tree_index vp9_switchable_interp_tree[VP9_SWITCHABLE_FILTERS * 2 -
                                                2] = {-0, 2, -1, -2};
const vp9_prob vp9_switchable_interp_prob
    [VP9_SWITCHABLE_FILTERS + 1][VP9_SWITCHABLE_FILTERS - 1] = {
        {235, 162, }, {36, 255, }, {34, 3, }, {149, 144, }, };

void Vp9InitModeContexts(struct Vp9Decoder *pc) {
  ASSERT(sizeof(vp9_default_inter_mode_prob) ==
         sizeof(pc->entropy.a.inter_mode_prob));
  memcpy(pc->entropy.a.inter_mode_prob, vp9_default_inter_mode_prob,
         sizeof(vp9_default_inter_mode_prob));
}
