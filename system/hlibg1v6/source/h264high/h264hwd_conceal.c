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
--
--  Abstract : Error concealment
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_conceal.c,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#include "h264hwd_container.h"
#include "h264hwd_conceal.h"
#include "h264hwd_util.h"
#include "h264hwd_dpb.h"

/*------------------------------------------------------------------------------
    Function name : h264bsdConceal
    Description   : Performe error concelament for an erroneous picture

    Return type   : void
    Argument      : storage_t *pStorage
    Argument      : DecAsicBuffers_t * pAsicBuff
    Argument      : u32 sliceType
------------------------------------------------------------------------------*/
void h264bsdConceal(storage_t * pStorage, DecAsicBuffers_t * pAsicBuff,
                    u32 sliceType)
{
    u32 i, tmp;
    i32 refID;

    u32 *pAsicCtrl = pAsicBuff->mbCtrl.virtualAddress;
    mbStorage_t *pMb = pStorage->mb;

    ASSERT(pStorage);
    ASSERT(pAsicBuff);

    DEBUG_PRINT(("h264bsdConceal: Concealing %s slice\n",
                 IS_I_SLICE(sliceType) ? "intra" : "inter"));

    refID = -1;

    /* use reference picture with smallest available index */
    if(IS_P_SLICE(sliceType))
    {
        i = 0;
        do
        {
            refID = h264bsdGetRefPicData(pStorage->dpb, i);
            i++;
        }
        while(i < 16 && refID == -1);
    }

    i = 0;
    /* find first properly decoded macroblock -> start point for concealment */
    while(i < pStorage->picSizeInMbs && !pMb[i].decoded)
    {
        i++;
    }

    /* whole picture lost -> copy previous or set grey */
    if(i == pStorage->picSizeInMbs)
    {
        DEBUG_PRINT(("h264bsdConceal: whole picture lost\n"));
        if(IS_I_SLICE(sliceType) || refID == -1)
        {
            /* QP = 40 and all residual blocks empty */
            /* DC predi. & chroma DC pred. --> grey */
            tmp = ((u32) HW_I_16x16 << 29) | (2U << 27) | (40U << 15) | 0x7F;

            for(i = pStorage->picSizeInMbs; i > 0; i--)
            {
                *pAsicCtrl++ = tmp;
                *pAsicCtrl++ = 0;
            }

            pMb->mbType_asic = I_16x16_2_0_0;
        }
        else
        {
            /* QP = 40 and all residual blocks empty */
            tmp = ((u32) HW_P_SKIP << 29) | (40U << 15) | 0x7F;

            for(i = pStorage->picSizeInMbs; i > 0; i--)
            {
                *pAsicCtrl++ = tmp;
                *pAsicCtrl++ = 0;
            }

            /* inter prediction using zero mv and the newest reference */
            pMb->mbType_asic = P_Skip;
            pMb->refID[0] = refID;
        }

        pStorage->numConcealedMbs = pStorage->picSizeInMbs;

        return;
    }

    for(i = 0; i < pStorage->picSizeInMbs; i++, pMb++)
    {
        /* if mb was not correctly decoded, set parameters for concealment */
        if(!pMb->decoded)
        {
            DEBUG_PRINT(("h264bsdConceal: Concealing mb #%u\n", i));

            pStorage->numConcealedMbs++;

            /* intra pred. if it is an intra mb or if the newest reference image
             * is not available */
            if(IS_I_SLICE(sliceType) || refID == -1)
            {
                /* QP = 40 and all residual blocks empty */
                /* DC predi. & chroma DC pred. --> grey */
                tmp =
                    ((u32) HW_I_16x16 << 29) | (2U << 27) | (40U << 15) | 0x7F;

                *pAsicCtrl++ = tmp;
                *pAsicCtrl++ = 0;

                pMb->mbType_asic = I_16x16_2_0_0;
            }
            else
            {
                DEBUG_PRINT(("h264bsdConceal: NOT I slice conceal \n"));
                /* QP = 40 and all residual blocks empty */
                tmp = ((u32) HW_P_SKIP << 29) | (40U << 15) | 0x7F;
                *pAsicCtrl++ = tmp;
                *pAsicCtrl++ = 0;

                /* inter prediction using zero mv and the newest reference */
                pMb->mbType_asic = P_Skip;
                pMb->refID[0] = refID;
            }
        }
        else
            pAsicCtrl += 2;
    }
}
