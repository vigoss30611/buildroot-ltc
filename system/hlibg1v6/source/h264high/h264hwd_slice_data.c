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
--  Abstract : Decode slice data, i.e. macroblocks of a slice, from the stream
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_slice_data.c,v $
--  $Date: 2008/10/27 12:58:51 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "dwl.h"
#include "h264hwd_container.h"

#include "h264hwd_slice_data.h"
#include "h264hwd_util.h"
#include "h264hwd_vlc.h"

#include "h264hwd_exports.h"

/*------------------------------------------------------------------------------

   5.1  Function name: h264bsdDecodeSliceData

        Functional description:
            Decode one slice. Function decodes stream data, i.e. macroblocks
            and possible skip_run fields. h264bsdDecodeMacroblock function is
            called to handle all other macroblock related processing.
            Macroblock to slice group mapping is considered when next
            macroblock to process is determined (h264bsdNextMbAddress function)
            map

        Inputs:
            pStrmData       pointer to stream data structure
            pStorage        pointer to storage structure
            currImage       pointer to current processed picture, needed for
                            intra prediction of the macroblocks
            pSliceHeader    pointer to slice header of the current slice

        Outputs:
            currImage       processed macroblocks are written to current image
            pStorage        mbStorage structure of each processed macroblock
                            is updated here

        Returns:
            HANTRO_OK       success
            HANTRO_NOK      invalid stream data

------------------------------------------------------------------------------*/

u32 h264bsdDecodeSliceData(decContainer_t * pDecCont, strmData_t * pStrmData,
                           sliceHeader_t * pSliceHeader)
{

/* Variables */

    u32 tmp;
    u32 skipRun;
    u32 prevSkipped;
    u32 currMbAddr;
    u32 moreMbs;
    u32 mbCount;
    i32 qpY;
    macroblockLayer_t *mbLayer;

    storage_t *pStorage;
    DecAsicBuffers_t *pAsicBuff = NULL;
    sliceStorage_t *slice;

/* Code */

    ASSERT(pStrmData);
    ASSERT(pSliceHeader);
    ASSERT(pDecCont);

    pStorage = &pDecCont->storage;
    mbLayer = pStorage->mbLayer;
    slice = pStorage->slice;

    pAsicBuff = pDecCont->asicBuff;

    currMbAddr = pSliceHeader->firstMbInSlice;

    ASSERT(currMbAddr < pStorage->picSizeInMbs);

    skipRun = 0;
    prevSkipped = HANTRO_FALSE;

    /* increment slice index, will be one for decoding of the first slice of
     * the picture */
    slice->sliceId++;

    /* lastMbAddr stores address of the macroblock that was last successfully
     * decoded, needed for error handling */
    slice->lastMbAddr = 0;

    mbCount = 0;
    /* initial quantization parameter for the slice is obtained as the sum of
     * initial QP for the picture and sliceQpDelta for the current slice */
    qpY = (i32) pStorage->activePps->picInitQp + pSliceHeader->sliceQpDelta;

    do
    {
        mbStorage_t *mb = pStorage->mb + currMbAddr;

        /* primary picture and already decoded macroblock -> error */
        if(!pSliceHeader->redundantPicCnt && mb->decoded)
        {
            ERROR_PRINT("Primary and already decoded");
            return (HANTRO_NOK);
        }

        mb->sliceId = slice->sliceId;

        if(!IS_I_SLICE(pSliceHeader->sliceType))
        {
            {
                if(!prevSkipped)
                {
                    tmp = h264bsdDecodeExpGolombUnsigned(pStrmData, &skipRun);
                    if(tmp != HANTRO_OK)
                        return (tmp);
                    if(skipRun == (pStorage->picSizeInMbs << 1) &&
                       pSliceHeader->frameNum == 0xF)
                    {
                        skipRun = MIN(0, pStorage->picSizeInMbs - currMbAddr);
                    }
                    /* skip_run shall be less than or equal to number of
                     * macroblocks left */
#ifdef HANTRO_PEDANTIC_MODE
                    else if(skipRun > (pStorage->picSizeInMbs - currMbAddr))
                    {
                        ERROR_PRINT("skip_run");
                        return (HANTRO_NOK);
                    }
#endif /* HANTRO_PEDANTIC_MODE */

                    if(skipRun)
                    {
                        /*mbPred_t *mbPred = &mbLayer->mbPred; */

                        prevSkipped = HANTRO_TRUE;
                        /*G1DWLmemset(&mbLayer->mbPred, 0, sizeof(mbPred_t)); */
                        /*G1DWLmemset(mbPred->remIntra4x4PredMode, 0, sizeof(mbPred->remIntra4x4PredMode)); */
                        /*G1DWLmemset(mbPred->refIdxL0, 0, sizeof(mbPred->refIdxL0)); */
                        /* mark current macroblock skipped */
                        mbLayer->mbType = P_Skip;
                    }
                }
            }
        }
        mbLayer->mbQpDelta = 0;

        {
            if(skipRun)
            {
                /*DEBUG_PRINT(("Skipping macroblock %d\n", currMbAddr)); */
                skipRun--;
            }
            else
            {
                prevSkipped = HANTRO_FALSE;
                tmp = h264bsdDecodeMacroblockLayerCavlc(pStrmData, mbLayer,
                                                        mb, pSliceHeader);
                if(tmp != HANTRO_OK)
                {
                    ERROR_PRINT("macroblock_layer");
                    return (tmp);
                }
            }
        }

        mbLayer->filterOffsetA = pSliceHeader->sliceAlphaC0Offset;
        mbLayer->filterOffsetB = pSliceHeader->sliceBetaOffset;
        mbLayer->disableDeblockingFilterIdc =
            pSliceHeader->disableDeblockingFilterIdc;

        pAsicBuff->currentMB = currMbAddr;
        tmp = h264bsdDecodeMacroblock(pStorage, currMbAddr, &qpY, pAsicBuff);

        if(tmp != HANTRO_OK)
        {
            ERROR_PRINT("MACRO_BLOCK");
            return (tmp);
        }

        /* increment macroblock count only for macroblocks that were decoded
         * for the first time (redundant slices) */
        if(mb->decoded == 1)
            mbCount++;

        {
            /* keep on processing as long as there is stream data left or
             * processing of macroblocks to be skipped based on the last skipRun is
             * not finished */
            moreMbs = (h264bsdMoreRbspData(pStrmData) ||
                       skipRun) ? HANTRO_TRUE : HANTRO_FALSE;
        }

        /* lastMbAddr is only updated for intra slices (all macroblocks of
         * inter slices will be lost in case of an error) */
        if(IS_I_SLICE(pSliceHeader->sliceType))
            slice->lastMbAddr = currMbAddr;

        currMbAddr = h264bsdNextMbAddress(pStorage->sliceGroupMap,
                                          pStorage->picSizeInMbs, currMbAddr);
        /* data left in the buffer but no more macroblocks for current slice
         * group -> error */
        if(moreMbs && !currMbAddr)
        {
            ERROR_PRINT("Next mb address");
            return (HANTRO_NOK);
        }

    }
    while(moreMbs);

    if((slice->numDecodedMbs + mbCount) > pStorage->picSizeInMbs)
    {
        ERROR_PRINT("Num decoded mbs");
        return (HANTRO_NOK);
    }

    slice->numDecodedMbs += mbCount;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.3  Function name: h264bsdMarkSliceCorrupted

        Functional description:
            Mark macroblocks of the slice corrupted. If lastMbAddr in the slice
            storage is set -> picWidhtInMbs (or at least 10) macroblocks back
            from  the lastMbAddr are marked corrupted. However, if lastMbAddr
            is not set -> all macroblocks of the slice are marked.

        Inputs:
            pStorage        pointer to storage structure
            firstMbInSlice  address of the first macroblock in the slice, this
                            identifies the slice to be marked corrupted

        Outputs:
            pStorage        mbStorage for the corrupted macroblocks updated

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdMarkSliceCorrupted(storage_t * pStorage, u32 firstMbInSlice)
{

/* Variables */

    u32 tmp, i;
    u32 sliceId;
    u32 currMbAddr;

/* Code */

    ASSERT(pStorage);
    ASSERT(firstMbInSlice < pStorage->picSizeInMbs);

    currMbAddr = firstMbInSlice;

    sliceId = pStorage->slice->sliceId;

    /* DecodeSliceData sets lastMbAddr for I slices -> if it was set, go back
     * MAX(picWidthInMbs, 10) macroblocks and start marking from there */
    if(pStorage->slice->lastMbAddr)
    {
        ASSERT(pStorage->mb[pStorage->slice->lastMbAddr].sliceId == sliceId);
        i = pStorage->slice->lastMbAddr - 1;
        tmp = 0;
        while(i > currMbAddr)
        {
            if(pStorage->mb[i].sliceId == sliceId)
            {
                tmp++;
                if(tmp >= MAX(pStorage->activeSps->picWidthInMbs, 10))
                    break;
            }
            i--;
        }
        currMbAddr = i;
    }

    do
    {

        if((pStorage->mb[currMbAddr].sliceId == sliceId) &&
           (pStorage->mb[currMbAddr].decoded))
        {
            pStorage->mb[currMbAddr].decoded--;
        }
        else
        {
            break;
        }

        currMbAddr = h264bsdNextMbAddress(pStorage->sliceGroupMap,
                                          pStorage->picSizeInMbs, currMbAddr);

    }
    while(currMbAddr);

}
