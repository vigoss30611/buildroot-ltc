/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "commonvp9.h"

const u8 vp9_default_inter_mode_prob[INTER_MODE_CONTEXTS][4] = {
    {2, 173, 34, 0},  // 0 = both zero mv
    {7, 145, 85, 0},  // 1 = one zero mv + one a predicted mv
    {7, 166, 63, 0},  // 2 = two predicted mvs
    {7, 94, 66, 0},   // 3 = one predicted/zero and one new mv
    {8, 64, 46, 0},   // 4 = two new mvs
    {17, 81, 31, 0},  // 5 = one intra neighbour + x
    {25, 29, 30, 0},  // 6 = two intra neighbours
};
