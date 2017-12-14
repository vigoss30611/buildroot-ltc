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
--  Abstract : Inter prediction. MV and referece frame update
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_inter_prediction.c,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#include "h264hwd_container.h"
#include "h264hwd_util.h"
#include "h264hwd_macroblock_layer.h"
#include "h264hwd_neighbour.h"
#include "h264hwd_exports.h"

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

static u32 MvPredictionSkip(mbStorage_t * pMb, dpbStorage_t * dpb);

static u32 MvPrediction16x16(mbStorage_t * pMb, dpbStorage_t * dpb);
static u32 MvPrediction16x8(mbStorage_t * pMb, dpbStorage_t * dpb);
static u32 MvPrediction8x16(mbStorage_t * pMb, dpbStorage_t * dpb);
static u32 MvPrediction8x8(mbStorage_t * pMb, subMbPred_t * subMbPred,
                           dpbStorage_t * dpb);

static u32 GetInterNeighbour(u32 sliceId, /*@null@ */ mbStorage_t * nMb);

/*------------------------------------------------------------------------------
    Function name   : PrepareInterPrediction
    Description     : Processes one inter macroblock. Writes MB control data
    Return type     : u32 - 0 for success or a negative error code
    Argument        : mbStorage_t * pMb - pointer to macroblock specific information
    Argument        : macroblockLayer_t * pMbLayer - macroblock layer data
    Argument        : dpbStorage_t * dpb - DPB data
    Argument        : DecAsicBuffers_t * pAsicBuff - SW/HW interface
------------------------------------------------------------------------------*/
u32 PrepareInterPrediction(mbStorage_t * pMb, macroblockLayer_t * pMbLayer,
                           dpbStorage_t * dpb, DecAsicBuffers_t * pAsicBuff)
{

    ASSERT(pMb);
    ASSERT(h264bsdMbPartPredMode(pMb->mbType) == PRED_MODE_INTER);
    ASSERT(pMbLayer);

    /* if decoded flag > 1 -> mb has already been successfully decoded and
     * written to output -> do not write again */
    if(pMb->decoded > 1)
        goto end;

    switch (pMb->mbType)
    {
    case P_Skip:
        if(MvPredictionSkip(pMb, dpb) != HANTRO_OK)
            return (HANTRO_NOK);
        break;

    case P_L0_16x16:
        if(MvPrediction16x16(pMb, dpb) != HANTRO_OK)
            return (HANTRO_NOK);

        break;

    case P_L0_L0_16x8:
        if(MvPrediction16x8(pMb, dpb) != HANTRO_OK)
            return (HANTRO_NOK);

        break;

    case P_L0_L0_8x16:
        if(MvPrediction8x16(pMb, dpb) != HANTRO_OK)
            return (HANTRO_NOK);
        break;

    default:   /* P_8x8 and P_8x8ref0 */
        if(MvPrediction8x8(pMb, &pMbLayer->subMbPred, dpb) != HANTRO_OK)
            return (HANTRO_NOK);
        break;
    }

    /* update ASIC MB control field */
    {
        u32 tmp;
        u32 *pAsicCtrl =
            pAsicBuff->mbCtrl.virtualAddress +
            (pAsicBuff->currentMB * (ASIC_MB_CTRL_BUFFER_SIZE / 4));

        switch (pMb->mbType)
        {
        case P_Skip:
            tmp = (u32) HW_P_SKIP << 29;
            break;

        case P_L0_16x16:
            tmp = (u32) HW_P_16x16 << 29;
            break;

        case P_L0_L0_16x8:
            tmp = (u32) HW_P_16x8 << 29;
            break;

        case P_L0_L0_8x16:
            tmp = (u32) HW_P_8x16 << 29;
            break;

        default:   /* P_8x8 and P_8x8ref0 */
            tmp = (u32) HW_P_8x8 << 29;
            tmp |= pMbLayer->subMbPred.subMbType[0] << 27;
            tmp |= pMbLayer->subMbPred.subMbType[1] << 25;
            tmp |= pMbLayer->subMbPred.subMbType[2] << 23;
            tmp |= pMbLayer->subMbPred.subMbType[3] << 21;

            break;
        }

        tmp |= pMb->qpY << 15;
        tmp |= (u32) (pMbLayer->filterOffsetA & 0x0F) << 11;
        tmp |= (u32) (pMbLayer->filterOffsetB & 0x0F) << 7;

        tmp |= pAsicBuff->notCodedMask;

        pAsicCtrl[0] = tmp;

        {
            tmp = GetInterNeighbour(pMb->sliceId, pMb->mbD) << 31;
            tmp |= GetInterNeighbour(pMb->sliceId, pMb->mbB) << 30;
            tmp |= GetInterNeighbour(pMb->sliceId, pMb->mbC) << 29;
            tmp |= GetInterNeighbour(pMb->sliceId, pMb->mbA) << 28;
            tmp |= pAsicBuff->rlcWords << 19;
        }

        tmp |= pMbLayer->disableDeblockingFilterIdc << 17;

        pAsicCtrl[1] = tmp;
    }

  end:
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: MvPrediction16x16

        Functional description:
            Motion vector prediction for 16x16 partition mode

------------------------------------------------------------------------------*/

u32 MvPrediction16x16(mbStorage_t * pMb, dpbStorage_t * dpb)
{
    u32 refIndex;
    i32 tmp;

    refIndex = pMb->refIdxL0[0];

    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[0] = (u8) tmp;

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: MvPredictionSkip

        Functional description:
            Motion vector prediction skipped macroblock

------------------------------------------------------------------------------*/

u32 MvPredictionSkip(mbStorage_t * pMb, dpbStorage_t * dpb)
{
    u32 refIndex = 0;
    i32 tmp;

    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[0] = (u8) tmp;

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: MvPrediction16x8

        Functional description:
            Motion vector prediction for 16x8 partition mode

------------------------------------------------------------------------------*/

u32 MvPrediction16x8(mbStorage_t * pMb, dpbStorage_t * dpb)
{
    u32 refIndex;
    i32 tmp;

    /* first partition */
    refIndex = pMb->refIdxL0[0];
    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[0] = (u8) tmp;

    /* second partition */
    refIndex = pMb->refIdxL0[1];

    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[1] = (u8) tmp;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: MvPrediction8x16

        Functional description:
            Motion vector prediction for 8x16 partition mode

------------------------------------------------------------------------------*/

u32 MvPrediction8x16(mbStorage_t * pMb, dpbStorage_t * dpb)
{
    u32 refIndex;
    i32 tmp;

    /* first partition */
    refIndex = pMb->refIdxL0[0];

    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[0] = (u8) tmp;

    /* second partition */
    refIndex = pMb->refIdxL0[1];

    tmp = h264bsdGetRefPicData(dpb, refIndex);
    if(tmp == -1)
        return (HANTRO_NOK);

    pMb->refID[1] = (u8) tmp;

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: MvPrediction8x8

        Functional description:
            Motion vector prediction for 8x8 partition mode

------------------------------------------------------------------------------*/
u32 MvPrediction8x8(mbStorage_t * pMb, subMbPred_t * subMbPred,
                    dpbStorage_t * dpb)
{
    u32 i;
    const u8 *refIdxL0 = pMb->refIdxL0;
    u8 *refID = pMb->refID;

    for(i = 4; i > 0; i--)
    {
        u32 refIndex;
        i32 tmp;

        refIndex = *refIdxL0++;
        tmp = h264bsdGetRefPicData(dpb, refIndex);
        if(tmp == -1)
            return (HANTRO_NOK);

        *refID++ = (u8) tmp;

    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: GetInterNeighbour

        Functional description:
            Checks if macroblock 'nMb' is part of slice 'sliceId'

------------------------------------------------------------------------------*/

u32 GetInterNeighbour(u32 sliceId, mbStorage_t * nMb)
{
    if(nMb && (sliceId == nMb->sliceId))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
