/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_COMMON_VP9_ENTROPYMV_H_
#define VP9_COMMON_VP9_ENTROPYMV_H_

#include "commonvp9.h"
#include "vp9_treecoder.h"
#include "vp9hwd_decoder.h"

void Vp9EntropyMvInit();
void Vp9InitMvProbs(struct Vp9Decoder* x);

void Vp9AdaptNmvProbs(struct Vp9Decoder* cm);

#define LOW_PRECISION_MV_UPDATE /* Use 7 bit forward update */

extern const vp9_tree_index vp9_mv_class_tree[2 * MV_CLASSES - 2];
extern struct vp9_token vp9_mv_class_encodings[MV_CLASSES];

extern const vp9_tree_index vp9_mv_class0_tree[2 * CLASS0_SIZE - 2];
extern struct vp9_token vp9_mv_class0_encodings[CLASS0_SIZE];

extern const vp9_tree_index vp9_mv_fp_tree[2 * 4 - 2];
extern struct vp9_token vp9_mv_fp_encodings[4];

#endif  // VP9_COMMON_VP9_ENTROPYMV_H_
