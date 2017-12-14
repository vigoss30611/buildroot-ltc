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
--  Abstract : Intra prediction for macroblock
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_intra_prediction.c,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#include "h264hwd_container.h"
#include "h264hwd_util.h"
#include "h264hwd_macroblock_layer.h"
#include "h264hwd_neighbour.h"
#include "h264hwd_exports.h"

static u32 Intra16x16Prediction(mbStorage_t * pMb,
                                macroblockLayer_t * mbLayer,
                                u32 constrainedIntraPred,
                                DecAsicBuffers_t * pAsicBuff);

static u32 Intra4x4Prediction(mbStorage_t * pMb, macroblockLayer_t * mbLayer,
                              u32 constrainedIntraPred,
                              DecAsicBuffers_t * pAsicBuff);

static u32 DetermineIntra4x4PredMode(macroblockLayer_t * pMbLayer,
                                     u32 available, neighbour_t * nA,
                                     neighbour_t * nB, u32 index,
                                     /*@null@ */ mbStorage_t * nMbA,
                                     /*@null@ */ mbStorage_t * nMbB);

static u32 GetIntraNeighbour(u32 sliceId, mbStorage_t * nMb);

static u32 CheckIntraChromaPrediction(u32 predMode, u32 availableA,
                                      u32 availableB, u32 availableD);

/*------------------------------------------------------------------------------
    Function name   : PrepareIntraPrediction
    Description     :
    Return type     : u32
    Argument        : mbStorage_t * pMb
    Argument        : macroblockLayer_t * mbLayer
    Argument        : u32 constrainedIntraPred
    Argument        : DecAsicBuffers_t * pAsicBuff
------------------------------------------------------------------------------*/
u32 PrepareIntraPrediction(mbStorage_t * pMb, macroblockLayer_t * mbLayer,
                           u32 constrainedIntraPred,
                           DecAsicBuffers_t * pAsicBuff)
{
    u32 tmp;

    if(h264bsdMbPartPredMode(pMb->mbType) == PRED_MODE_INTRA4x4)
    {
        tmp = Intra4x4Prediction(pMb, mbLayer, constrainedIntraPred, pAsicBuff);
        if(tmp != HANTRO_OK)
            return (tmp);
    }
    else
    {
        tmp =
            Intra16x16Prediction(pMb, mbLayer, constrainedIntraPred, pAsicBuff);
        if(tmp != HANTRO_OK)
            return (tmp);
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------
    Function name   : Intra16x16Prediction
    Description     :
    Return type     : u32
    Argument        : mbStorage_t * pMb
    Argument        : macroblockLayer_t * mbLayer
    Argument        : u32 constrainedIntraPred
    Argument        : DecAsicBuffers_t * pAsicBuff
------------------------------------------------------------------------------*/
u32 Intra16x16Prediction(mbStorage_t * pMb, macroblockLayer_t * mbLayer,
                         u32 constrainedIntraPred,
                         DecAsicBuffers_t * pAsicBuff)
{
    u32 mode, tmp;
    u32 availableA, availableB, availableD;

    ASSERT(h264bsdPredModeIntra16x16(pMb->mbType) < 4);

    availableA = h264bsdIsNeighbourAvailable(pMb, pMb->mbA);
    if(availableA && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbA->mbType) == PRED_MODE_INTER))
        availableA = HANTRO_FALSE;
    availableB = h264bsdIsNeighbourAvailable(pMb, pMb->mbB);
    if(availableB && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbB->mbType) == PRED_MODE_INTER))
        availableB = HANTRO_FALSE;
    availableD = h264bsdIsNeighbourAvailable(pMb, pMb->mbD);
    if(availableD && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbD->mbType) == PRED_MODE_INTER))
        availableD = HANTRO_FALSE;

    mode = h264bsdPredModeIntra16x16(pMb->mbType);

    switch (mode)
    {
    case 0:    /* Intra_16x16_Vertical */
        if(!availableB)
            return (HANTRO_NOK);
        break;

    case 1:    /* Intra_16x16_Horizontal */
        if(!availableA)
            return (HANTRO_NOK);
        break;

    case 2:    /* Intra_16x16_DC */
        break;

    default:   /* case 3: Intra_16x16_Plane */
        if(!availableA || !availableB || !availableD)
            return (HANTRO_NOK);
        break;
    }

    tmp = CheckIntraChromaPrediction(mbLayer->mbPred.intraChromaPredMode,
                                     availableA, availableB, availableD);
    if(tmp != HANTRO_OK)
        return (tmp);

    if(pMb->decoded > 1)
    {
        goto end;
    }

    /* update ASIC MB control field */
    {
        u32 tmp2;
        u32 *pAsicCtrl =
            pAsicBuff->mbCtrl.virtualAddress +
            (pAsicBuff->currentMB * (ASIC_MB_CTRL_BUFFER_SIZE / 4));

        tmp2 = (u32) HW_I_16x16 << 29;
        tmp2 |= mode << 27;

        tmp2 |= mbLayer->mbPred.intraChromaPredMode << 25;
        tmp2 |= ((availableA == HANTRO_TRUE ? 1U : 0U) << 24);
        tmp2 |= ((availableB == HANTRO_TRUE ? 1U : 0U) << 23);

        tmp2 |= pMb->qpY << 15;
        tmp2 |= (u32) (mbLayer->filterOffsetA & 0x0F) << 11;
        tmp2 |= (u32) (mbLayer->filterOffsetB & 0x0F) << 7;

        tmp2 |= pAsicBuff->notCodedMask;

        pAsicCtrl[0] = tmp2;

        tmp2 = GetIntraNeighbour(pMb->sliceId, pMb->mbD) << 31;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbB) << 30;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbC) << 29;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbA) << 28;

        tmp2 |= pAsicBuff->rlcWords << 19;
        tmp2 |= mbLayer->disableDeblockingFilterIdc << 17;

        pAsicCtrl[1] = tmp2;
    }

  end:
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------
    Function name   : Intra4x4Prediction
    Description     :
    Return type     : u32
    Argument        : mbStorage_t * pMb
    Argument        : macroblockLayer_t * mbLayer
    Argument        : u32 constrainedIntraPred
    Argument        : DecAsicBuffers_t * pAsicBuff
------------------------------------------------------------------------------*/
u32 Intra4x4Prediction(mbStorage_t * pMb, macroblockLayer_t * mbLayer,
                       u32 constrainedIntraPred, DecAsicBuffers_t * pAsicBuff)
{
    u32 block;
    u32 mode, tmp;
    neighbour_t neighbour, neighbourB;
    mbStorage_t *nMb, *nMb2;

    u32 availableA, availableB, availableC, availableD;

    for(block = 0; block < 16; block++)
    {
        ASSERT(pMb->intra4x4PredMode[block] < 9);

        neighbour = *h264bsdNeighbour4x4BlockA(block);
        nMb = h264bsdGetNeighbourMb(pMb, neighbour.mb);
        availableA = h264bsdIsNeighbourAvailable(pMb, nMb);
        if(availableA && constrainedIntraPred &&
           (h264bsdMbPartPredMode(nMb->mbType) == PRED_MODE_INTER))
        {
            availableA = HANTRO_FALSE;
        }

        neighbourB = *h264bsdNeighbour4x4BlockB(block);
        nMb2 = h264bsdGetNeighbourMb(pMb, neighbourB.mb);
        availableB = h264bsdIsNeighbourAvailable(pMb, nMb2);
        if(availableB && constrainedIntraPred &&
           (h264bsdMbPartPredMode(nMb2->mbType) == PRED_MODE_INTER))
        {
            availableB = HANTRO_FALSE;
        }

        mode = DetermineIntra4x4PredMode(mbLayer,
                                         (u32) (availableA && availableB),
                                         &neighbour, &neighbourB, block, nMb,
                                         nMb2);
        pMb->intra4x4PredMode[block] = (u8) mode;

        if(pMb->decoded == 1)
        {
            pMb->intra4x4PredMode_asic[block] = (u8) mode;
        }

        neighbour = *h264bsdNeighbour4x4BlockD(block);
        nMb = h264bsdGetNeighbourMb(pMb, neighbour.mb);
        availableD = h264bsdIsNeighbourAvailable(pMb, nMb);
        if(availableD && constrainedIntraPred &&
           (h264bsdMbPartPredMode(nMb->mbType) == PRED_MODE_INTER))
        {
            availableD = HANTRO_FALSE;
        }

        switch (mode)
        {
        case 0:    /* Intra_4x4_Vertical */
            if(!availableB)
                return (HANTRO_NOK);
            break;
        case 1:    /* Intra_4x4_Horizontal */
            if(!availableA)
                return (HANTRO_NOK);
            break;
        case 2:    /* Intra_4x4_DC */
            break;
        case 3:    /* Intra_4x4_Diagonal_Down_Left */
            if(!availableB)
                return (HANTRO_NOK);
            break;
        case 4:    /* Intra_4x4_Diagonal_Down_Right */
            if(!availableA || !availableB || !availableD)
                return (HANTRO_NOK);
            break;
        case 5:    /* Intra_4x4_Vertical_Right */
            if(!availableA || !availableB || !availableD)
                return (HANTRO_NOK);
            break;
        case 6:    /* Intra_4x4_Horizontal_Down */
            if(!availableA || !availableB || !availableD)
                return (HANTRO_NOK);
            break;
        case 7:    /* Intra_4x4_Vertical_Left */
            if(!availableB)
                return (HANTRO_NOK);
            break;
        default:   /* case 8 Intra_4x4_Horizontal_Up */
            if(!availableA)
                return (HANTRO_NOK);
            break;
        }
    }

    availableA = h264bsdIsNeighbourAvailable(pMb, pMb->mbA);
    if(availableA && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbA->mbType) == PRED_MODE_INTER))
        availableA = HANTRO_FALSE;
    availableB = h264bsdIsNeighbourAvailable(pMb, pMb->mbB);
    if(availableB && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbB->mbType) == PRED_MODE_INTER))
        availableB = HANTRO_FALSE;
    availableC = h264bsdIsNeighbourAvailable(pMb, pMb->mbC);
    if(availableC && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbC->mbType) == PRED_MODE_INTER))
        availableC = HANTRO_FALSE;
    availableD = h264bsdIsNeighbourAvailable(pMb, pMb->mbD);
    if(availableD && constrainedIntraPred &&
       (h264bsdMbPartPredMode(pMb->mbD->mbType) == PRED_MODE_INTER))
        availableD = HANTRO_FALSE;

    tmp = CheckIntraChromaPrediction(mbLayer->mbPred.intraChromaPredMode,
                                     availableA, availableB, availableD);
    if(tmp != HANTRO_OK)
        return (tmp);

    if(pMb->decoded > 1)
    {
        goto end;
    }

    /* update ASIC MB control field */
    {
        u32 tmp2;
        u32 *pAsicCtrl =
            pAsicBuff->mbCtrl.virtualAddress +
            (pAsicBuff->currentMB * (ASIC_MB_CTRL_BUFFER_SIZE / 4));

        tmp2 = (u32) HW_I_4x4 << 29;

        tmp2 |= mbLayer->mbPred.intraChromaPredMode << 25;
        tmp2 |= ((availableA == HANTRO_TRUE ? 1U : 0U) << 24);
        tmp2 |= ((availableB == HANTRO_TRUE ? 1U : 0U) << 23);
        tmp2 |= ((availableC == HANTRO_TRUE ? 1U : 0U) << 22);

        tmp2 |= pMb->qpY << 15;
        tmp2 |= (u32) (mbLayer->filterOffsetA & 0x0F) << 11;
        tmp2 |= (u32) (mbLayer->filterOffsetB & 0x0F) << 7;

        tmp2 |= pAsicBuff->notCodedMask;

        pAsicCtrl[0] = tmp2;

        tmp2 = GetIntraNeighbour(pMb->sliceId, pMb->mbD) << 31;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbB) << 30;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbC) << 29;
        tmp2 |= GetIntraNeighbour(pMb->sliceId, pMb->mbA) << 28;

        tmp2 |= pAsicBuff->rlcWords << 19;
        tmp2 |= mbLayer->disableDeblockingFilterIdc << 17;

        pAsicCtrl[1] = tmp2;
    }

  end:
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------
    Function name   : DetermineIntra4x4PredMode
    Description     :
    Return type     : u32
    Argument        : macroblockLayer_t * pMbLayer
    Argument        : u32 available
    Argument        : neighbour_t * nA
    Argument        : neighbour_t * nB
    Argument        : u32 index
    Argument        : mbStorage_t * nMbA
    Argument        : mbStorage_t * nMbB
------------------------------------------------------------------------------*/
u32 DetermineIntra4x4PredMode(macroblockLayer_t * pMbLayer,
                              u32 available, neighbour_t * nA,
                              neighbour_t * nB, u32 index, mbStorage_t * nMbA,
                              mbStorage_t * nMbB)
{
    u32 mode1, mode2;
    mbStorage_t *pMb;

    ASSERT(pMbLayer);

    if(!available)  /* dc only prediction? */
        mode1 = 2;
    else
    {
        pMb = nMbA;
        if(h264bsdMbPartPredMode(pMb->mbType) == PRED_MODE_INTRA4x4)
        {
            mode1 = (u32) pMb->intra4x4PredMode[nA->index];
        }
        else
            mode1 = 2;

        pMb = nMbB;
        if(h264bsdMbPartPredMode(pMb->mbType) == PRED_MODE_INTRA4x4)
        {
            mode2 = (u32) pMb->intra4x4PredMode[nB->index];
        }
        else
            mode2 = 2;

        mode1 = MIN(mode1, mode2);
    }

    {
        mbPred_t *mbPred = &pMbLayer->mbPred;

        if(!mbPred->prevIntra4x4PredModeFlag[index])
        {
            if(mbPred->remIntra4x4PredMode[index] < mode1)
            {
                mode1 = mbPred->remIntra4x4PredMode[index];
            }
            else
            {
                mode1 = mbPred->remIntra4x4PredMode[index] + 1;
            }
        }
    }
    return (mode1);
}

/*------------------------------------------------------------------------------

    Function: GetIntraNeighbour

        Functional description:
            Checks if macroblock 'nMb' is part of slice 'sliceId'

------------------------------------------------------------------------------*/
u32 GetIntraNeighbour(u32 sliceId, mbStorage_t * nMb)
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

/*------------------------------------------------------------------------------

    Function: CheckIntraChromaPrediction

        Functional description:
         Check that the intra chroma prediction mode is valid!

------------------------------------------------------------------------------*/
u32 CheckIntraChromaPrediction(u32 predMode, u32 availableA, u32 availableB,
                               u32 availableD)
{
    switch (predMode)
    {
    case 0:    /* Intra_Chroma_DC */
        break;
    case 1:    /* Intra_Chroma_Horizontal */
        if(!availableA)
            return (HANTRO_NOK);
        break;
    case 2:    /* Intra_Chroma_Vertical */
        if(!availableB)
            return (HANTRO_NOK);
        break;
    case 3:    /* Intra_Chroma_Plane */
        if(!availableA || !availableB || !availableD)
            return (HANTRO_NOK);
        break;
    default:
        ASSERT(predMode < 4);
        return HANTRO_NOK;
    }

    return (HANTRO_OK);
}
