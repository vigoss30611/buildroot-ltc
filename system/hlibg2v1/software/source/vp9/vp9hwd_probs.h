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

#ifndef __VP9_PROBS_H__
#define __VP9_PROBS_H__

#include "basetype.h"
#include "vp9hwd_bool.h"
#include "vp9hwd_decoder.h"

void Vp9ResetProbs(struct Vp9Decoder* dec);
void Vp9GetProbs(struct Vp9Decoder* dec);
void Vp9StoreProbs(struct Vp9Decoder* dec);
void Vp9StoreAdaptProbs(struct Vp9Decoder* dec);
void Vp9ComputeModRefProbs(struct Vp9Decoder* dec);
u32 Vp9DecodeMvUpdate(struct VpBoolCoder* bc, struct Vp9Decoder* dec);
u32 Vp9DecodeCoeffUpdate(
    struct VpBoolCoder* bc,
    u8 prob_coeffs[BLOCK_TYPES][REF_TYPES][COEF_BANDS][PREV_COEF_CONTEXTS]
                  [ENTROPY_NODES_PART1]);
void Vp9AdaptCoefProbs(struct Vp9Decoder* cm);
void Vp9AdaptModeProbs(struct Vp9Decoder* cm);
void Vp9AdaptModeContext(struct Vp9Decoder* cm);
vp9_prob Vp9ReadProbDiffUpdate(struct VpBoolCoder* bc, int oldp);

#endif /* __VP9_PROBS_H__ */
