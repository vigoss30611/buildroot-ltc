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
--  Abstract : Macroblock level stream decoding and macroblock reconstruction
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_macroblock_layer.c,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "h264hwd_macroblock_layer.h"
#include "h264hwd_slice_header.h"
#include "h264hwd_util.h"
#include "h264hwd_vlc.h"
#include "h264hwd_cavlc.h"
#include "h264hwd_nal_unit.h"
#include "h264hwd_neighbour.h"

#include "h264hwd_dpb.h"
#include "dwl.h"
#include "h264hwd_exports.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

static u32 DecodeMbPred(strmData_t *, mbPred_t *, mbType_e, u32,
                        mbStorage_t * );
static u32 DecodeSubMbPred(strmData_t *, subMbPred_t *, mbType_e, u32,
                           mbStorage_t * );
static u32 DecodeResidual(strmData_t *, macroblockLayer_t *, mbStorage_t *);
static u32 DetermineNc(mbStorage_t *, u32, u8 *);

static u32 CbpIntra16x16(mbType_e);

static u32 h264bsdNumMbPart(mbType_e mbType);

static void WritePCMToAsic(const u8 *, DecAsicBuffers_t *);
static void WriteRlcToAsic(mbType_e, u32 cbp, residual_t *, DecAsicBuffers_t *);

static void WriteBlock(const u16 *, u32 *, u32 **, u32 *);
static void WriteSubBlock(const u16 *, u32 *, u32 **, u32 *);

/*------------------------------------------------------------------------------

    Function name: h264bsdDecodeMacroblockLayerCavlc

        Functional description:
          Parse macroblock specific information from bit stream. Called
          when entropy_coding_mode_flag = 0.

        Inputs:
          pStrmData         pointer to stream data structure
          pMb               pointer to macroblock storage structure
          pSliceHdr         pointer to slice header data

        Outputs:
          pMbLayer          stores the macroblock data parsed from stream

        Returns:
          HANTRO_OK         success
          HANTRO_NOK        end of stream or error in stream

------------------------------------------------------------------------------*/

u32 h264bsdDecodeMacroblockLayerCavlc(strmData_t * pStrmData,
                                      macroblockLayer_t * pMbLayer,
                                      mbStorage_t * pMb,
                                      const sliceHeader_t * pSliceHdr )
{

/* Variables */

    u32 tmp, i, value;
    i32 itmp;
    mbPartPredMode_e partMode;

/* Code */

    ASSERT(pStrmData);
    ASSERT(pMbLayer);

    (void)G1DWLmemset(pMbLayer->residual.totalCoeff, 0, 24);

    tmp = h264bsdDecodeExpGolombUnsigned(pStrmData, &value);

    if(IS_I_SLICE(pSliceHdr->sliceType))
    {
        if((value + 6) > 31 || tmp != HANTRO_OK)
            return (HANTRO_NOK);
        pMbLayer->mbType = (mbType_e) (value + 6);
    }
    else
    {
        if((value + 1) > 31 || tmp != HANTRO_OK)
            return (HANTRO_NOK);
        pMbLayer->mbType = (mbType_e) (value + 1);
    }

    if(pMbLayer->mbType == I_PCM)
    {
        u8 *level;

        while(!h264bsdIsByteAligned(pStrmData))
        {
            /* pcm_alignment_zero_bit */
            tmp = h264bsdGetBits(pStrmData, 1);
            if(tmp)
                return (HANTRO_NOK);
        }

        level = (u8 *) pMbLayer->residual.rlc;
        for(i = 384; i > 0; i--)
        {
            value = h264bsdGetBits(pStrmData, 8);
            if(value == END_OF_STREAM)
                return (HANTRO_NOK);
            *level++ = (u8) value;
        }
    }
    else
    {
        partMode = h264bsdMbPartPredMode(pMbLayer->mbType);
        if((partMode == PRED_MODE_INTER) &&
           (h264bsdNumMbPart(pMbLayer->mbType) == 4))
        {
            tmp = DecodeSubMbPred(pStrmData, &pMbLayer->subMbPred,
                                  pMbLayer->mbType,
                                  pSliceHdr->numRefIdxL0Active,
                                  pMb );
        }
        else
        {
            tmp = DecodeMbPred(pStrmData, &pMbLayer->mbPred,
                               pMbLayer->mbType, pSliceHdr->numRefIdxL0Active,
                               pMb );
        }
        if(tmp != HANTRO_OK)
            return (tmp);

        if(partMode != PRED_MODE_INTRA16x16)
        {
            tmp = h264bsdDecodeExpGolombMapped(pStrmData, &value,
                                               (u32) (partMode ==
                                                       PRED_MODE_INTRA4x4));
            if(tmp != HANTRO_OK)
                return (tmp);
            pMbLayer->codedBlockPattern = value;
        }
        else
        {
            pMbLayer->codedBlockPattern = CbpIntra16x16(pMbLayer->mbType);
        }

        if(pMbLayer->codedBlockPattern || (partMode == PRED_MODE_INTRA16x16))
        {
            tmp = h264bsdDecodeExpGolombSigned(pStrmData, &itmp);
            if(tmp != HANTRO_OK ||
               (u32) (itmp + 26) > 51U /*(itmp >= -26) || (itmp < 26) */ )
                return (HANTRO_NOK);

            pMbLayer->mbQpDelta = itmp;

            tmp = DecodeResidual(pStrmData, pMbLayer, pMb);

            if(tmp != HANTRO_OK)
                return (tmp);
        }
    }

    return (HANTRO_OK);
}


/*------------------------------------------------------------------------------

    Function: h264bsdMbPartPredMode

        Functional description:
          Returns the prediction mode of a macroblock type

------------------------------------------------------------------------------*/

mbPartPredMode_e h264bsdMbPartPredMode(mbType_e mbType)
{

    ASSERT(mbType <= 31);

    if((mbType <= P_8x8ref0))
        return (PRED_MODE_INTER);
    else if(mbType == I_4x4)
        return (PRED_MODE_INTRA4x4);
    else
        return (PRED_MODE_INTRA16x16);

}

/*------------------------------------------------------------------------------

    Function: h264bsdNumMbPart

        Functional description:
          Returns the amount of macroblock partitions in a macroblock type

------------------------------------------------------------------------------*/

u32 h264bsdNumMbPart(mbType_e mbType)
{
    ASSERT(h264bsdMbPartPredMode(mbType) == PRED_MODE_INTER);

    switch (mbType)
    {
    case P_L0_16x16:
    case P_Skip:
        return (1);

    case P_L0_L0_16x8:
    case P_L0_L0_8x16:
        return (2);

        /* P_8x8 or P_8x8ref0 */
    default:
        return (4);
    }
}

/*------------------------------------------------------------------------------

    Function: h264bsdNumSubMbPart

        Functional description:
          Returns the amount of sub-partitions in a sub-macroblock type

------------------------------------------------------------------------------*/

u32 h264bsdNumSubMbPart(subMbType_e subMbType)
{
    ASSERT(subMbType <= P_L0_4x4);

    switch (subMbType)
    {
    case P_L0_8x8:
        return (1);

    case P_L0_8x4:
    case P_L0_4x8:
        return (2);

        /* P_L0_4x4 */
    default:
        return (4);
    }
}

/*------------------------------------------------------------------------------

    Function: DecodeMbPred

        Functional description:
          Parse macroblock prediction information from bit stream and store
          in 'pMbPred'.

------------------------------------------------------------------------------*/

u32 DecodeMbPred(strmData_t * pStrmData, mbPred_t * pMbPred, mbType_e mbType,
                 u32 numRefIdxActive, mbStorage_t * pMb )
{

/* Variables */

    u32 tmp, i, j, value;
    i32 itmp;

/* Code */

    ASSERT(pStrmData);
    ASSERT(pMbPred);

    switch (h264bsdMbPartPredMode(mbType))
    {
    case PRED_MODE_INTER:  /* PRED_MODE_INTER */
        if(numRefIdxActive > 1)
        {
            u8 *refIdxL0 = pMb->refIdxL0;

            for(i = h264bsdNumMbPart(mbType); i--;)
            {
                tmp = h264bsdDecodeExpGolombTruncated(pStrmData, &value,
                                                      (u32) (numRefIdxActive >
                                                              2));
                if(tmp != HANTRO_OK || value >= numRefIdxActive)
                    return (HANTRO_NOK);

                *refIdxL0++ = value;
            }
        }
        else
        {
            u8 *refIdxL0 = pMb->refIdxL0;

            for(i = 4; i > 0; i--)
            {
                *refIdxL0++ = 0;
            }
        }

        /* mvd decoding */
        {
            mv_t *mvdL0 = pMb->mv;
            u32 offsToNext;

            if( mbType == P_L0_L0_8x16 )    offsToNext = 4;
            else                            offsToNext = 8; /* "incorrect" for
                                                             * 16x16, but it
                                                             * doesn't matter */
            for(i = h264bsdNumMbPart(mbType); i--;)
            {
                tmp = h264bsdDecodeExpGolombSigned(pStrmData, &itmp);
                if(tmp != HANTRO_OK)
                    return (tmp);
                if (itmp < -16384 || itmp > 16383)
                    return(HANTRO_NOK);
                mvdL0->hor = (i16) itmp;
                tmp = h264bsdDecodeExpGolombSigned(pStrmData, &itmp);
                if(tmp != HANTRO_OK)
                    return (tmp);
                if (itmp < -4096 || itmp > 4095)
                    return(HANTRO_NOK);
                mvdL0->ver = (i16) itmp;

                mvdL0 += offsToNext;
            }
        }
        break;

    case PRED_MODE_INTRA4x4:
        {
            u32 *prevIntra4x4PredModeFlag = pMbPred->prevIntra4x4PredModeFlag;
            u32 *remIntra4x4PredMode = pMbPred->remIntra4x4PredMode;

            for(i = 2; i > 0; i--)
            {
                value = h264bsdShowBits(pStrmData,32);
                tmp = 0;
                for(j = 8; j--;)
                {
                    u32 b = value & 0x80000000 ? HANTRO_TRUE : HANTRO_FALSE;

                    *prevIntra4x4PredModeFlag++ = b;

                    value <<= 1;

                    if(!b)
                    {
                        *remIntra4x4PredMode++ = value >> 29;
                        value <<= 3;
                        tmp++;
                    }
                    else
                    {
                        remIntra4x4PredMode++;
                    }
                }

                if(h264bsdFlushBits(pStrmData, 8 + 3 * tmp) == END_OF_STREAM)
                    return (HANTRO_NOK);
            }
        }
        /* fall-through */

    case PRED_MODE_INTRA16x16:
        tmp = h264bsdDecodeExpGolombUnsigned(pStrmData, &value);
        if(tmp != HANTRO_OK || value > 3)
            return (HANTRO_NOK);
        pMbPred->intraChromaPredMode = value;
        break;
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: DecodeSubMbPred

        Functional description:
          Parse sub-macroblock prediction information from bit stream and
          store in 'pMbPred'.

------------------------------------------------------------------------------*/

u32 DecodeSubMbPred(strmData_t * pStrmData, subMbPred_t * pSubMbPred,
                    mbType_e mbType, u32 numRefIdxActive,
                    mbStorage_t * pMb )
{

/* Variables */

    u32 tmp, i, j;

/* Code */

    ASSERT(pStrmData);
    ASSERT(pSubMbPred);
    ASSERT(h264bsdMbPartPredMode(mbType) == PRED_MODE_INTER);

    {
        subMbType_e *subMbType = pSubMbPred->subMbType;

        for(i = 4; i > 0; i--)
        {
            u32 value;

            tmp = h264bsdDecodeExpGolombUnsigned(pStrmData, &value);
            if(tmp != HANTRO_OK || value > 3)
                return (HANTRO_NOK);
            *subMbType++ = (subMbType_e) value;
        }
    }

    if((numRefIdxActive > 1) && (mbType != P_8x8ref0))
    {
        u8 *refIdxL0 = pMb->refIdxL0;
        u32 greaterThanOne = (numRefIdxActive > 2) ? HANTRO_TRUE : HANTRO_FALSE;

        for(i = 4; i > 0; i--)
        {
            u32 value;

            tmp = h264bsdDecodeExpGolombTruncated(pStrmData, &value,
                                                  greaterThanOne);
            if(tmp != HANTRO_OK || value >= numRefIdxActive)
                return (HANTRO_NOK);
            *refIdxL0++ = value;
        }
    }
    else
    {
        u8 *refIdxL0 = pMb->refIdxL0;

        for(i = 4; i > 0; i--)
        {
            *refIdxL0++ = 0;
        }
    }

    for(i = 0; i < 4; i++)
    {
        mv_t *mvdL0 = pMb->mv + i * 4;
        subMbType_e subMbType;
        u32 offsToNext;
        static const u32 mvdOffs[4] = { 0, 2, 1, 1 }; /* offset to next sub mb
                                                       * part motion vector */
        subMbType = pSubMbPred->subMbType[i];
        offsToNext = mvdOffs[(u32)subMbType];

        for(j = h264bsdNumSubMbPart(subMbType); j--;)
        {
            i32 value;

            tmp = h264bsdDecodeExpGolombSigned(pStrmData, &value);
            if(tmp != HANTRO_OK)
                return (tmp);
            if (value < -16384 || value > 16383)
                return(HANTRO_NOK);
            mvdL0->hor = (i16) value;

            tmp = h264bsdDecodeExpGolombSigned(pStrmData, &value);
            if(tmp != HANTRO_OK)
                return (tmp);
            if (value < -4096 || value > 4095)
                return(HANTRO_NOK);
            mvdL0->ver = (i16) value;

            mvdL0 += offsToNext;
        }
    }

    return (HANTRO_OK);

}


/*------------------------------------------------------------------------------

    Function: DetermineNc

        Functional description:
          Returns the nC of a block.

------------------------------------------------------------------------------*/

u32 DetermineNc(mbStorage_t * pMb, u32 blockIndex, u8 * pTotalCoeff)
{

/* Variables */

    u32 n, tmp;
    const neighbour_t *neighbourA, *neighbourB;
    u8 neighbourAindex, neighbourBindex;

/* Code */

    ASSERT(blockIndex < 24);

    /* if neighbour block belongs to current macroblock totalCoeff array
     * mbStorage has not been set/updated yet -> use pTotalCoeff */
    neighbourA = h264bsdNeighbour4x4BlockA(blockIndex);
    neighbourB = h264bsdNeighbour4x4BlockB(blockIndex);
    neighbourAindex = neighbourA->index;
    neighbourBindex = neighbourB->index;
    if(neighbourA->mb == MB_CURR && neighbourB->mb == MB_CURR)
    {
        n = (pTotalCoeff[neighbourAindex] +
             pTotalCoeff[neighbourBindex] + 1) >> 1;
    }
    else if(neighbourA->mb == MB_CURR)
    {
        n = pTotalCoeff[neighbourAindex];
        if(h264bsdIsNeighbourAvailable(pMb, pMb->mbB))
        {
            n = (n + pMb->mbB->totalCoeff[neighbourBindex] + 1) >> 1;
        }
    }
    else if(neighbourB->mb == MB_CURR)
    {
        n = pTotalCoeff[neighbourBindex];
        if(h264bsdIsNeighbourAvailable(pMb, pMb->mbA))
        {
            n = (n + pMb->mbA->totalCoeff[neighbourAindex] + 1) >> 1;
        }
    }
    else
    {
        n = tmp = 0;
        if(h264bsdIsNeighbourAvailable(pMb, pMb->mbA))
        {
            n = pMb->mbA->totalCoeff[neighbourAindex];
            tmp = 1;
        }
        if(h264bsdIsNeighbourAvailable(pMb, pMb->mbB))
        {
            if(tmp)
                n = (n + pMb->mbB->totalCoeff[neighbourBindex] + 1) >> 1;
            else
                n = pMb->mbB->totalCoeff[neighbourBindex];
        }
    }

    return (n);

}

/*------------------------------------------------------------------------------

    Function: DecodeResidual

        Functional description:
          Parse residual information from bit stream and store in 'pResidual'.

------------------------------------------------------------------------------*/

u32 DecodeResidual(strmData_t * pStrmData, macroblockLayer_t * pMbLayer,
                   mbStorage_t * pMb)
{
    u32 i, tmp;
    u32 blockCoded, blockIndex;
    u32 is16x16 = 0;    /* no I_16x16 by default */
    u8 *coeff;
    u16 *level;

    residual_t *pResidual = &pMbLayer->residual;
    mbType_e mbType = pMbLayer->mbType;
    u32 codedBlockPattern = pMbLayer->codedBlockPattern;

    ASSERT(pStrmData);
    ASSERT(pResidual);

    level = pResidual->rlc;
    coeff = pResidual->totalCoeff;

    /* luma DC is at index 24 */
    if(h264bsdMbPartPredMode(mbType) == PRED_MODE_INTRA16x16)
    {
        i32 nc = (i32) DetermineNc(pMb, 0, pResidual->totalCoeff);

        tmp =
            h264bsdDecodeResidualBlockCavlc(pStrmData, &level[24 * 18], nc, 16);
        if(tmp == (u32)(~0))
            return (tmp);

        coeff[24] = tmp;

        is16x16 = 1;
    }

    blockIndex = 0;
    /* rest of luma */
    for(i = 4; i > 0; i--)
    {
        /* luma cbp in bits 0-3 */
        blockCoded = codedBlockPattern & 0x1;
        codedBlockPattern >>= 1;
        if(blockCoded)
        {
            u32 j;

            for(j = 4; j > 0; j--)
            {
                u32 max_coeffs = 16;

                i32 nc =
                    (i32) DetermineNc(pMb, blockIndex, pResidual->totalCoeff);

                if(is16x16)
                {
                    max_coeffs = 15;
                }

                tmp = h264bsdDecodeResidualBlockCavlc(pStrmData,
                                                      level, nc, max_coeffs);
                if(tmp == (u32)(~0))
                    return (tmp);

                *coeff++ = tmp;
                level += 18;
                blockIndex++;
            }
        }
        else
        {
            coeff += 4;
            level += 4 * 18;
            blockIndex += 4;
        }
    }

    level = pResidual->rlc + 25 * 18;
    coeff = pResidual->totalCoeff + 25;
    /* chroma DC block are at indices 25 and 26 */
    if(codedBlockPattern)
    {
        tmp = h264bsdDecodeResidualBlockCavlc(pStrmData, level, -1, 4);
        if(tmp == (u32)(~0))
            return (tmp);

        *coeff++ = tmp;

        tmp = h264bsdDecodeResidualBlockCavlc(pStrmData, level + 6, -1, 4);
        if(tmp == (u32)(~0))
            return (tmp);

        *coeff = tmp;
    }

    level = pResidual->rlc + 16 * 18;
    coeff = pResidual->totalCoeff + 16;
    /* chroma AC */
    blockCoded = codedBlockPattern & 0x2;
    if(blockCoded)
    {
        for(i = 8; i > 0; i--)
        {
            i32 nc = (i32) DetermineNc(pMb, blockIndex, pResidual->totalCoeff);

            tmp = h264bsdDecodeResidualBlockCavlc(pStrmData, level, nc, 15);
            if(tmp == (u32)(~0))
                return (tmp);

            *coeff++ = tmp;

            level += 18;
            blockIndex++;
        }
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: CbpIntra16x16

        Functional description:
          Returns the coded block pattern for intra 16x16 macroblock.

------------------------------------------------------------------------------*/

u32 CbpIntra16x16(mbType_e mbType)
{

/* Variables */

    u32 cbp;
    u32 tmp;

/* Code */

    ASSERT(mbType >= I_16x16_0_0_0 && mbType <= I_16x16_3_2_1);

    if(mbType >= I_16x16_0_0_1)
        cbp = 15;
    else
        cbp = 0;

    /* tmp is 0 for I_16x16_0_0_0 mb type */
    /* ignore lint warning on arithmetic on enum's */
    tmp = /*lint -e(656) */ ((u32) mbType - (u32) I_16x16_0_0_0) >> 2;
    if(tmp > 2)
        tmp -= 3;

    cbp += tmp << 4;

    return (cbp);

}

/*------------------------------------------------------------------------------

    Function: h264bsdPredModeIntra16x16

        Functional description:
          Returns the prediction mode for intra 16x16 macroblock.

------------------------------------------------------------------------------*/

u32 h264bsdPredModeIntra16x16(mbType_e mbType)
{

/* Variables */

    u32 tmp;

/* Code */

    ASSERT(mbType >= I_16x16_0_0_0 && mbType <= I_16x16_3_2_1);

    /* tmp is 0 for I_16x16_0_0_0 mb type */
    /* ignore lint warning on arithmetic on enum's */
    tmp = /*lint -e(656) */ (mbType - I_16x16_0_0_0);

    return (tmp & 0x3);

}

/*------------------------------------------------------------------------------

    Function: h264bsdDecodeMacroblock

        Functional description:
          Decode one macroblock and write into output image.

        Inputs:
          pStorage      decoder storage
          mbNum         current macroblock number
          qpY           pointer to slice QP
          pAsicBuff     asic interface

        Outputs:
          pMb           structure is updated with current macroblock
          currImage     decoded macroblock is written into output image

        Returns:
          HANTRO_OK     success
          HANTRO_NOK    error in macroblock decoding

------------------------------------------------------------------------------*/

u32 h264bsdDecodeMacroblock(storage_t * pStorage, u32 mbNum, i32 * qpY,
                            DecAsicBuffers_t * pAsicBuff)
{

    u32 tmp;
    mbType_e mbType;

    macroblockLayer_t *pMbLayer = pStorage->mbLayer;
    mbStorage_t *pMb = pStorage->mb + mbNum;
    residual_t *residual = &pMbLayer->residual;

    ASSERT(pMb);
    ASSERT(pMbLayer);
    ASSERT(qpY && *qpY < 52);

    mbType = pMbLayer->mbType;
    pMb->mbType = mbType;

    if(!pMb->decoded)
        pMb->mbType_asic = mbType;

    pMb->decoded++;

    if(mbType == I_PCM)
    {
        pMb->qpY = 0;

        /* if decoded flag > 1 -> mb has already been successfully decoded and
         * written to output -> do not write again */
        (void)G1DWLmemset(pMb->totalCoeff, 16, 24);

        if(pMb->decoded <= 1)
        {
            /* write out PCM data to residual buffer */
            WritePCMToAsic((u8 *) residual->rlc, pAsicBuff);
            /* update ASIC MB control field */
            {
                u32 *pAsicCtrl = pAsicBuff->mbCtrl.virtualAddress +
                    (pAsicBuff->currentMB * (ASIC_MB_CTRL_BUFFER_SIZE/4));

                *pAsicCtrl++ = ((u32) (HW_I_PCM) << 29) |
                       ((u32) (pMbLayer->filterOffsetA & 0x0F) << 11) |
                       ((u32) (pMbLayer->filterOffsetB & 0x0F) << 7);
                tmp = 0;
                if (pMb->mbD && pMb->sliceId == pMb->mbD->sliceId)
                    tmp |= 1U<<31;
                if (pMb->mbB && pMb->sliceId == pMb->mbB->sliceId)
                    tmp |= 1U<<30;
                if (pMb->mbC && pMb->sliceId == pMb->mbC->sliceId)
                    tmp |= 1U<<29;
                if (pMb->mbA && pMb->sliceId == pMb->mbA->sliceId)
                    tmp |= 1U<<28;
                tmp |= ((384U/2) << 19);
                tmp |= pMbLayer->disableDeblockingFilterIdc << 17;
                *pAsicCtrl = tmp;
            }
        }

        return (HANTRO_OK);
    }

    /* else */

    if(mbType != P_Skip)
    {
        i32 tmpQp = *qpY;

        (void)G1DWLmemcpy(pMb->totalCoeff, residual->totalCoeff, 24);

        /* update qpY */
        if(pMbLayer->mbQpDelta)
        {
            tmpQp += pMbLayer->mbQpDelta;
            if(tmpQp < 0)
                tmpQp += 52;
            else if(tmpQp > 51)
                tmpQp -= 52;
        }

        pMb->qpY = (u32) tmpQp;
        *qpY = tmpQp;

        /* write out residual to ASIC */
        if(pMb->decoded <= 1)
        {
            WriteRlcToAsic(mbType, pMbLayer->codedBlockPattern, residual,
                           pAsicBuff);
        }

    }
    else
    {
        (void)G1DWLmemset(pMb->totalCoeff, 0, 24);
        pMb->qpY = (u32) * qpY;
        pAsicBuff->notCodedMask = 0x3F;
        pAsicBuff->rlcWords = 0;
    }

    if(h264bsdMbPartPredMode(mbType) != PRED_MODE_INTER)
    {
        u32 cipf = pStorage->activePps->constrainedIntraPredFlag;

        tmp = PrepareIntraPrediction(pMb, pMbLayer, cipf, pAsicBuff);
    }
    else
    {
        dpbStorage_t *dpb = pStorage->dpb;

        tmp = PrepareInterPrediction(pMb, pMbLayer, dpb, pAsicBuff);
    }

    return (tmp);
}

/*------------------------------------------------------------------------------

    Function: h264bsdSubMbPartMode

        Functional description:
          Returns the macroblock's sub-partition mode.

------------------------------------------------------------------------------*/
#if 0  /* not used */
subMbPartMode_e h264bsdSubMbPartMode(subMbType_e subMbType)
{
    ASSERT(subMbType < 4);

    return ((subMbPartMode_e) subMbType);
}
#endif
/*------------------------------------------------------------------------------
    Function name : WritePCMToAsic
    Description   :

    Return type   : void
    Argument      : const u8 * lev
    Argument      : DecAsicBuffers_t * pAsicBuff
------------------------------------------------------------------------------*/
void WritePCMToAsic(const u8 * lev, DecAsicBuffers_t * pAsicBuff)
{
    i32 i;

    u32 *pRes = (pAsicBuff->residual.virtualAddress +
                 pAsicBuff->currentMB * (880 / 4));
    for(i = 384 / 4; i > 0; i--)
    {
        u32 tmp;

        tmp = (*lev++) << 24;
        tmp |= (*lev++) << 16;
        tmp |= (*lev++) << 8;
        tmp |= (*lev++);

        *pRes++ = tmp;
    }
}

/*------------------------------------------------------------------------------
    Function name   : WriteRlcToAsic
    Description     :
    Return type     : void
    Argument        : mbType_e mbType
    Argument        : u32 cbp
    Argument        : residual_t * residual
    Argument        : DecAsicBuffers_t * pAsicBuff
------------------------------------------------------------------------------*/
void WriteRlcToAsic(mbType_e mbType, u32 cbp, residual_t * residual,
                    DecAsicBuffers_t * pAsicBuff)
{
    u32 block;
    u32 nc;
    u32 wrtBuff;
    u32 word_count;

    const u16 *rlc = residual->rlc;

    u32 *pRes = pAsicBuff->residual.virtualAddress +
        pAsicBuff->currentMB * (ASIC_MB_RLC_BUFFER_SIZE / 4);

    ASSERT(pAsicBuff->residual.virtualAddress != NULL);
    ASSERT(pRes != NULL);

    nc = 0;
    word_count = 0;
    wrtBuff = 0;

    /* write out luma DC for intra 16x16 */
    if(h264bsdMbPartPredMode(mbType) == PRED_MODE_INTRA16x16)
    {
        const u16 *pTmp = (rlc + 24 * 18);
        const u8 *coeff = residual->totalCoeff;

        WriteSubBlock(pTmp, &wrtBuff, &pRes, &word_count);
        for(block = 4; block > 0; block--)
        {
            u32 j, bc = 0;

            for(j = 4; j > 0; j--)
            {
                if(*coeff++ != 0)
                    bc++;
            }
            if(!bc)
            {
                cbp &= (~(u32)(1 << (4 - block)));
            }
        }
    }
    else if(cbp == 0)   /* empty macroblock */
    {
        nc = 0x3F;
        goto end;
    }

    /* write out rest of luma */
    for(block = 4; block > 0; block--)
    {
        nc <<= 1;

        /* update notcoded block mask */
        if((cbp & 0x01))
        {
            WriteBlock(rlc, &wrtBuff, &pRes, &word_count);
        }
        else
        {
            nc |= 1;
        }

        rlc += 4 * 18;
        cbp >>= 1;
    }

    /* chroma DC always written out */
    if(cbp == 0)
    {
        const u16 dcRlc = 0;

        WriteSubBlock(&dcRlc, &wrtBuff, &pRes, &word_count);
        WriteSubBlock(&dcRlc, &wrtBuff, &pRes, &word_count);
    }
    else
    {
        const u16 *dcRlc = residual->rlc + (25 * 18);

        WriteSubBlock(dcRlc, &wrtBuff, &pRes, &word_count);
        WriteSubBlock(dcRlc + 6, &wrtBuff, &pRes, &word_count);
    }

    if((cbp & 0x02) == 0)   /* no chroma AC */
    {
        nc = (nc << 2) | 0x03;
    }
    else
    {
        /* write out chroma */
        const u8 *coeff = residual->totalCoeff + 16;

        nc <<= 1;

        /* update notcoded block mask */
        /* Cb */
        if((coeff[0] == 0) && (coeff[1] == 0) &&
           (coeff[2] == 0) && (coeff[3] == 0))
        {
            nc |= 1;
        }
        else
        {
            WriteBlock(rlc, &wrtBuff, &pRes, &word_count);
        }

        rlc += 4 * 18;

        nc <<= 1;

        coeff += 4;
        /* Cr */
        if((coeff[0] == 0) && (coeff[1] == 0) &&
           (coeff[2] == 0) && (coeff[3] == 0))
        {
            nc |= 1;
        }
        else
        {
            WriteBlock(rlc, &wrtBuff, &pRes, &word_count);
        }
    }

  end:

    if(word_count & 0x01)
    {
        *pRes = wrtBuff;
    }

    pAsicBuff->notCodedMask = nc;
    pAsicBuff->rlcWords = word_count;
}

/*------------------------------------------------------------------------------
    Function name   : WriteSubBlock
    Description     :
    Return type     : void
    Argument        : const u16 * rlc
    Argument        : u32 * pWrtBuff
    Argument        : u32 ** res
    Argument        : u32 * pWordCount
------------------------------------------------------------------------------*/
void WriteSubBlock(const u16 * rlc, u32 * pWrtBuff, u32 ** res,
                   u32 * pWordCount)
{
    /*lint -efunc(416, WriteSubBlock) */
    /*lint -efunc(661, WriteSubBlock) */
    /*lint -efunc(662, WriteSubBlock) */

    /* Reason: when rlc[1] words = 0 so, overflow cannot occure in pTmp */
    /* this happens for 0 chromaDC values which are always written out */

    u32 wrtBuff = *pWrtBuff;
    u32 *pRes = *res;
    u32 word_count = *pWordCount;

    const u16 *pTmp = rlc;
    u16 words;

    {
        u16 rlc_ctrl = *pTmp++;

        words = rlc_ctrl >> 11;

        if((word_count++) & 0x01)
        {
            wrtBuff |= rlc_ctrl;
            *pRes++ = wrtBuff;
        }
        else
        {
            wrtBuff = rlc_ctrl << 16;
        }

        if(rlc_ctrl & 0x01)
        {
            words++;
        }
        else
        {
            pTmp++;
        }
    }

    for(; words > 0; words--)
    {
        if((word_count++) & 0x01)
        {
            wrtBuff |= *pTmp++;
            *pRes++ = wrtBuff;
        }
        else
        {
            wrtBuff = (*pTmp++) << 16;
        }
    }

    *pWrtBuff = wrtBuff;
    *res = pRes;
    *pWordCount = word_count;
}

/*------------------------------------------------------------------------------
    Function name   : WriteBlock
    Description     :
    Return type     : void
    Argument        : const u16 * rlc
    Argument        : u32 * pWrtBuff
    Argument        : u32 ** res
    Argument        : u32 * pWordCount
------------------------------------------------------------------------------*/
void WriteBlock(const u16 * rlc, u32 * pWrtBuff, u32 ** res, u32 * pWordCount)
{
    i32 i;

    for(i = 4; i > 0; i--)
    {
        WriteSubBlock(rlc, pWrtBuff, res, pWordCount);

        rlc += 18;
    }
}
