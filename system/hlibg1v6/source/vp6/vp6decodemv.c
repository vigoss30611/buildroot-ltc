/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp6decodemv.c,v $
-  $Revision: 1.2 $
-  $Date: 2008/04/24 12:08:29 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp6decodemv.h"
#include "vp6dec.h"

/**************************************************************************** 
 * 
 *  ROUTINE       :     VP6HWConfigureMvEntropyDecoder
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      u8 FrameType  : Type of the frame.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Builds the MV entropy decoding tree.
 *
 *  SPECIAL NOTES :     None. 
 *
***************************************************************************/
void VP6HWConfigureMvEntropyDecoder(PB_INSTANCE * pbi, u8 FrameType)
{
    i32 i;

    (void) FrameType;

    /* This funciton is not called at all for a BASE_FRAME
     * Read any changes to mv probabilities. */
    for(i = 0; i < 2; i++)
    {
        /* Short vector probability */
        if(VP6HWDecodeBool(&pbi->br, VP6HWMvUpdateProbs[i][0]))
        {
            pbi->IsMvShortProb[i] =
                VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
            if(pbi->IsMvShortProb[i] == 0)
                pbi->IsMvShortProb[i] = 1;

            pbi->probMvUpdate = 1;
        }

        /* Sign probability */
        if(VP6HWDecodeBool(&pbi->br, VP6HWMvUpdateProbs[i][1]))
        {
            pbi->MvSignProbs[i] =
                VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
            if(pbi->MvSignProbs[i] == 0)
                pbi->MvSignProbs[i] = 1;

            pbi->probMvUpdate = 1;
        }
    }

    /* Short vector tree node probabilities */
    for(i = 0; i < 2; i++)
    {
        u32 j;
        u32 MvUpdateProbsOffset = 2;    /* Offset into MvUpdateProbs[i][] */

        for(j = 0; j < 7; j++)
        {
            if(VP6HWDecodeBool
               (&pbi->br, VP6HWMvUpdateProbs[i][MvUpdateProbsOffset]))
            {
                pbi->MvShortProbs[i][j] =
                    VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
                if(pbi->MvShortProbs[i][j] == 0)
                    pbi->MvShortProbs[i][j] = 1;

                pbi->probMvUpdate = 1;
            }
            MvUpdateProbsOffset++;
        }
    }

    /* Long vector tree node probabilities */
    for(i = 0; i < 2; i++)
    {
        u32 j;
        u32 MvUpdateProbsOffset = 2 + 7;

        for(j = 0; j < LONG_MV_BITS; j++)
        {
            if(VP6HWDecodeBool
               (&pbi->br, VP6HWMvUpdateProbs[i][MvUpdateProbsOffset]))
            {
                pbi->MvSizeProbs[i][j] =
                    VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
                if(pbi->MvSizeProbs[i][j] == 0)
                    pbi->MvSizeProbs[i][j] = 1;

                pbi->probMvUpdate = 1;
            }
            MvUpdateProbsOffset++;
        }
    }
}
