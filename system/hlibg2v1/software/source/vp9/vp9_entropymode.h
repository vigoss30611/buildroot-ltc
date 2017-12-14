/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_COMMON_VP9_ENTROPYMODE_H_
#define VP9_COMMON_VP9_ENTROPYMODE_H_

#include "commonvp9.h"
#include "vp9_treecoder.h"
#include "vp9hwd_decoder.h"

#define DEFAULT_COMP_INTRA_PROB 32

#define VP9_DEF_INTERINTRA_PROB 248
#define VP9_UPD_INTERINTRA_PROB 192
#define SEPARATE_INTERINTRA_UV 0

extern const vp9_prob vp9_kf_default_bmode_probs
    [VP9_INTRA_MODES][VP9_INTRA_MODES][VP9_INTRA_MODES - 1];

extern const vp9_tree_index vp9_intra_mode_tree[];
extern const vp9_tree_index vp9_sb_mv_ref_tree[];

extern struct vp9_token vp9_intra_mode_encodings[VP9_INTRA_MODES];

/* Inter mode values do not start at zero */

extern struct vp9_token vp9_sb_mv_ref_encoding_array[VP9_INTER_MODES];

/* probability models for partition information */
extern const vp9_tree_index vp9_partition_tree[];
extern struct vp9_token vp9_partition_encodings[PARTITION_TYPES];
extern const vp9_prob vp9_partition_probs
    [NUM_FRAME_TYPES][NUM_PARTITION_CONTEXTS][PARTITION_TYPES];

void Vp9EntropyModeInit(void);

struct VP9Common;

void Vp9InitMbmodeProbs(struct Vp9Decoder *x);

extern void Vp9InitModeContexts(struct Vp9Decoder *pc);

extern const enum InterpolationFilterType
    vp9_switchable_interp[VP9_SWITCHABLE_FILTERS];

extern const int vp9_switchable_interp_map[SWITCHABLE + 1];

extern const vp9_tree_index
    vp9_switchable_interp_tree[2 * (VP9_SWITCHABLE_FILTERS - 1)];

extern struct vp9_token vp9_switchable_interp_encodings[VP9_SWITCHABLE_FILTERS];

extern const vp9_prob vp9_switchable_interp_prob[VP9_SWITCHABLE_FILTERS +
                                                 1][VP9_SWITCHABLE_FILTERS - 1];

extern const vp9_prob
    vp9_default_tx_probs_32x32p[TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 1];
extern const vp9_prob
    vp9_default_tx_probs_16x16p[TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 2];
extern const vp9_prob
    vp9_default_tx_probs_8x8p[TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 3];

#endif
