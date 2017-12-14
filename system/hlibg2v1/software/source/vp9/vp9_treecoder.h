/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_COMMON_VP9_TREECODER_H_
#define VP9_COMMON_VP9_TREECODER_H_

#include "commonvp9.h"

#define vp9_prob_half ((vp9_prob)128)

typedef i8 vp9_tree_index;

#define vp9_complement(x) (255 - x)
#define ROUND_POWER_OF_TWO(value, n) (((value) + (1 << ((n) - 1))) >> (n))

/* We build coding trees compactly in arrays.
   Each node of the tree is a pair of vp9_tree_indices.
   Array index often references a corresponding probability table.
   Index <= 0 means done encoding/decoding and value = -Index,
   Index > 0 means need another bit, specification at index.
   Nonnegative indices are always even;  processing begins at node 0. */

typedef const vp9_tree_index vp9_tree[], *vp9_tree_p;

struct vp9_token {
  int value;
  int Len;
};

/* Convert array of token occurrence counts into a table of probabilities
   for the associated binary encoding tree.  Also writes count of branches
   taken for each node on the tree; this facilitiates decisions as to
   probability updates. */

void Vp9TreeProbsFromDistribution(vp9_tree tree, vp9_prob probs[/* n-1 */],
                                  unsigned int branch_ct[/* n-1 */][2],
                                  const unsigned int num_events[/* n */],
                                  unsigned int tok0_offset);

static __inline vp9_prob ClipProb(int p) {
  return (p > 255) ? 255u : (p < 1) ? 1u : p;
}

static __inline vp9_prob GetProb(int num, int den) {
  return (den == 0) ? 128u : ClipProb((num * 256 + (den >> 1)) / den);
}

static __inline vp9_prob GetBinaryProb(int n0, int n1) {
  return GetProb(n0, n0 + n1);
}

/* this function assumes prob1 and prob2 are already within [1,255] range */
static __inline vp9_prob WeightedProb(int prob1, int prob2, int factor) {
  return ROUND_POWER_OF_TWO(prob1 * (256 - factor) + prob2 * factor, 8);
}

#endif  // VP9_COMMON_VP9_TREECODER_H_
