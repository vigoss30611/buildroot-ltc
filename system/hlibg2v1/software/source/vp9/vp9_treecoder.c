/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG
#include <assert.h>
#endif
#include <stdio.h>

#include "vp9_treecoder.h"

static unsigned int ConvertDistribution(unsigned int i, vp9_tree tree,
                                        vp9_prob probs[],
                                        unsigned int branch_ct[][2],
                                        const unsigned int num_events[],
                                        unsigned int tok0_offset) {
  unsigned int left, right;

  if (tree[i] <= 0) {
    left = num_events[-tree[i] - tok0_offset];
  } else {
    left = ConvertDistribution(tree[i], tree, probs, branch_ct, num_events,
                               tok0_offset);
  }
  if (tree[i + 1] <= 0) {
    right = num_events[-tree[i + 1] - tok0_offset];
  } else {
    right = ConvertDistribution(tree[i + 1], tree, probs, branch_ct, num_events,
                                tok0_offset);
  }
  probs[i >> 1] = GetBinaryProb(left, right);
  branch_ct[i >> 1][0] = left;
  branch_ct[i >> 1][1] = right;
  return left + right;
}

void Vp9TreeProbsFromDistribution(vp9_tree tree, vp9_prob probs[/* n-1 */],
                                  unsigned int branch_ct[/* n-1 */][2],
                                  const unsigned int num_events[/* n */],
                                  unsigned int tok0_offset) {
  ConvertDistribution(0, tree, probs, branch_ct, num_events, tok0_offset);
}
