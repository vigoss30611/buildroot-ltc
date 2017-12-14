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
--  Abstract : Top level control of the decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_decoder.c,v $
--  $Date: 2010/10/26 09:05:16 $
--  $Revision: 1.30 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "h264hwd_container.h"

#include "h264hwd_decoder.h"
#include "h264hwd_nal_unit.h"
#include "h264hwd_byte_stream.h"
#include "h264hwd_seq_param_set.h"
#include "h264hwd_pic_param_set.h"
#include "h264hwd_slice_header.h"
#include "h264hwd_slice_data.h"
#include "h264hwd_neighbour.h"
#include "h264hwd_util.h"
#include "h264hwd_dpb.h"

#include "h264hwd_conceal.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name: h264bsdInit

        Functional description:
            Initialize the decoder.

        Inputs:
            noOutputReordering  flag to indicate the decoder that it does not
                                have to perform reordering of display images.

        Outputs:
            pStorage            pointer to initialized storage structure

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdInit(storage_t * pStorage, u32 noOutputReordering,
    u32 useSmoothingBuffer)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    h264bsdInitStorage(pStorage);

    pStorage->noReordering = noOutputReordering;
    pStorage->useSmoothing = useSmoothingBuffer;
    pStorage->dpb = pStorage->dpbs[0];
    pStorage->sliceHeader = pStorage->sliceHeaders[0];

}

/*------------------------------------------------------------------------------

    Function: h264bsdDecodeVlc

        Functional description:
            Decode a NAL unit until a slice header. This function calls other modules to perform
            tasks like
                * extract and decode NAL unit from the byte stream
                * decode parameter sets
                * decode slice header and slice data
                * conceal errors in the picture
                * perform deblocking filtering

            This function contains top level control logic of the decoder.

        Inputs:
            pStorage        pointer to storage data structure
            byteStrm        pointer to stream buffer given by application
            len             length of the buffer in bytes
            picId           identifier for a picture, assigned by the
                            application

        Outputs:
            readBytes       number of bytes read from the stream is stored
                            here

        Returns:
            H264BSD_RDY             decoding finished, nothing special
            H264BSD_PIC_RDY         decoding of a picture finished
            H264BSD_HDRS_RDY        param sets activated, information like
                                    picture dimensions etc can be read
            H264BSD_ERROR           error in decoding
            H264BSD_PARAM_SET_ERROR serius error in decoding, failed to
                                    activate param sets

------------------------------------------------------------------------------*/
u32 h264bsdDecode(decContainer_t * pDecCont, const u8 * byteStrm, u32 len,
                  u32 picId, u32 * readBytes)
{

/* Variables */

    u32 tmp, ppsId, spsId;
    u32 accessUnitBoundaryFlag = HANTRO_FALSE;
    u32 picReady = HANTRO_FALSE;
    storage_t *pStorage;
    nalUnit_t nalUnit;
    seqParamSet_t seqParamSet;
    picParamSet_t picParamSet;
    strmData_t strm;

    u32 ret = 0;

    DEBUG_PRINT(("h264bsdDecode\n"));

/* Code */
    ASSERT(pDecCont);
    ASSERT(byteStrm);
    ASSERT(len);
    ASSERT(readBytes);

    pStorage = &pDecCont->storage;
    ASSERT(pStorage);

    DEBUG_PRINT(("Valid slice in access unit %d\n",
                 pStorage->validSliceInAccessUnit));

    if(pDecCont->rlcMode)
    {
        pStorage->strm[0].removeEmul3Byte = 1;
        strm.removeEmul3Byte = 1;
    }
    else
    {
        pStorage->strm[0].removeEmul3Byte = 0;
        strm.removeEmul3Byte = 0;
    }

    /* if previous buffer was not finished and same pointer given -> skip NAL
     * unit extraction */
    if(pStorage->prevBufNotFinished && byteStrm == pStorage->prevBufPointer)
    {
        strm = pStorage->strm[0];
        strm.pStrmCurrPos = strm.pStrmBuffStart;
        strm.strmBuffReadBits = strm.bitPosInWord = 0;
        *readBytes = pStorage->prevBytesConsumed;
    }
    else
    {
        tmp =
            h264bsdExtractNalUnit(byteStrm, len, &strm, readBytes,
                                  pDecCont->rlcMode);
        if(tmp != HANTRO_OK)
        {
            ERROR_PRINT("BYTE_STREAM");
            return (H264BSD_ERROR);
        }
        /* store stream */
        pStorage->strm[0] = strm;
        pStorage->prevBytesConsumed = *readBytes;
        pStorage->prevBufPointer = byteStrm;
    }

    pStorage->prevBufNotFinished = HANTRO_FALSE;

    tmp = h264bsdDecodeNalUnit(&strm, &nalUnit);
    if(tmp != HANTRO_OK)
    {
        ret = H264BSD_ERROR;
        goto NEXT_NAL;
    }

    /* Discard unspecified, reserved, SPS extension and auxiliary picture slices */
    if(nalUnit.nalUnitType == 0 ||
        (nalUnit.nalUnitType >= 13 &&
         (pStorage->mvc == 0 ||
          (nalUnit.nalUnitType != 14 &&
           nalUnit.nalUnitType != 15 &&
           nalUnit.nalUnitType != 20))))
    {
        DEBUG_PRINT(("DISCARDED NAL (UNSPECIFIED, REGISTERED, SPS ext or AUX slice)\n"));
        ret = H264BSD_RDY;
        goto NEXT_NAL;
    }

    if(pDecCont->skipNonReference &&
       nalUnit.nalRefIdc == 0 )
    {
        DEBUG_PRINT(("DISCARDED NAL (NON-REFERENCE PICTURE)\n"));
        ret = H264BSD_NONREF_PIC_SKIPPED;
        goto NEXT_NAL;
    }

    if(!pStorage->checkedAub)
    {
        tmp = h264bsdCheckAccessUnitBoundary(&strm,
                                             &nalUnit,
                                             pStorage, &accessUnitBoundaryFlag);
        if(tmp != HANTRO_OK)
        {
            ERROR_PRINT("ACCESS UNIT BOUNDARY CHECK");
            if(tmp == PARAM_SET_ERROR)
                ret = (H264BSD_PARAM_SET_ERROR);
            else
                ret = (H264BSD_ERROR);
            goto NEXT_NAL;
        }
    }
    else
    {
        pStorage->checkedAub = 0;
    }

    if(accessUnitBoundaryFlag)
    {
        DEBUG_PRINT(("Access unit boundary, NAL TYPE %d\n",
                     nalUnit.nalUnitType));

        /* conceal if picture started and param sets activated */
        if(pStorage->picStarted && pStorage->activeSps != NULL)
        {
            if(pDecCont->rlcMode)   /* Error conceal in RLC mode */
            {

                DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

                ASSERT(pAsicBuff);
                ASSERT(pAsicBuff->mbCtrl.virtualAddress);
                DEBUG_PRINT(("CONCEALING..."));

                if(!pStorage->validSliceInAccessUnit)
                {
                    DEBUG_PRINT(("!validSliceunit\n"));
                    pStorage->currImage->data =
                        h264bsdAllocateDpbImage(pStorage->dpb);
                    h264bsdInitRefPicList(pStorage->dpb);

                    h264bsdConceal(pStorage, pAsicBuff, P_SLICE);
                }
                else
                {
                    DEBUG_PRINT(("validSliceunit\n"));
                    h264bsdConceal(pStorage, pAsicBuff,
                                   pStorage->sliceHeader->sliceType);
                }

                picReady = HANTRO_TRUE;

                /* current NAL unit should be decoded on next activation -> set
                 * readBytes to 0 */
                *readBytes = 0;
                pStorage->prevBufNotFinished = HANTRO_TRUE;
                DEBUG_PRINT(("...DONE\n"));

            }
            else    /* VLC  mode */
            {
                DEBUG_PRINT(("New access unit and previous not finished\n"));
                DEBUG_PRINT(("PICTURE FREEZE CONCEAL...\n"));

                if(!pStorage->validSliceInAccessUnit)
                {
                    DEBUG_PRINT(("!validSliceunit\n"));
                    if (!pStorage->view && !pStorage->secondField)
                    {
                        pStorage->currImage->data =
                            h264bsdAllocateDpbImage(pStorage->dpb);
                        pStorage->sliceHeader->fieldPicFlag = 0;
                    }
                    else if (pStorage->view)
                    {
                        pStorage->validSliceInAccessUnit = 1;
                        if (!pStorage->baseOppositeFieldPic)
                        {
                            pStorage->currImage->data =
                                h264bsdAllocateDpbImage(pStorage->dpb);
                        }
                        else
                        {
                            pStorage->baseOppositeFieldPic = 0;
                            pStorage->currImage->data =
                                pStorage->dpb->currentOut->data;
                        }
                        pStorage->sliceHeader = pStorage->sliceHeaders[0];
                    }
                    h264bsdInitRefPicList(pStorage->dpb);
                }

                pStorage->skipRedundantSlices = HANTRO_FALSE;

                /* current NAL unit should be decoded on next activation -> set
                 * readBytes to 0 */
                *readBytes = 0;
                pStorage->prevBufNotFinished = HANTRO_TRUE;
                DEBUG_PRINT(("...DONE\n"));

                return (H264BSD_NEW_ACCESS_UNIT);
            }
        }
        else
        {
            DEBUG_PRINT(("vali slice false\n"));
            pStorage->validSliceInAccessUnit = HANTRO_FALSE;
        }

        pStorage->skipRedundantSlices = HANTRO_FALSE;
    }

    if(!picReady)
    {
        DEBUG_PRINT(("nal unit type: %d\n", nalUnit.nalUnitType));

        switch (nalUnit.nalUnitType)
        {
        case NAL_SEQ_PARAM_SET:
        case NAL_SUBSET_SEQ_PARAM_SET:
            DEBUG_PRINT(("SEQ PARAM SET\n"));
            tmp = h264bsdDecodeSeqParamSet(&strm, &seqParamSet,
                nalUnit.nalUnitType == NAL_SEQ_PARAM_SET ? 0 : 1);
            if(tmp != HANTRO_OK)
            {
                ERROR_PRINT("SEQ_PARAM_SET decoding");
                FREE(seqParamSet.offsetForRefFrame);
                FREE(seqParamSet.vuiParameters);
                ret = H264BSD_ERROR;
            }
            else
            {
                tmp = h264bsdStoreSeqParamSet(pStorage, &seqParamSet);
                if(tmp != HANTRO_OK)
                {
                    ERROR_PRINT("SEQ_PARAM_SET allocation");
                    ret = H264BSD_ERROR;
                }
                if (nalUnit.nalUnitType == NAL_SUBSET_SEQ_PARAM_SET)
                {
                    pStorage->viewId[0] = seqParamSet.mvc.viewId[0];
                    pStorage->viewId[1] = seqParamSet.mvc.viewId[1];
                }
            }

            ret = H264BSD_RDY;
            goto NEXT_NAL;

        case NAL_PIC_PARAM_SET:
            DEBUG_PRINT(("PIC PARAM SET\n"));
            tmp = h264bsdDecodePicParamSet(&strm, &picParamSet);
            if(tmp != HANTRO_OK)
            {
                ERROR_PRINT("PIC_PARAM_SET decoding");
                FREE(picParamSet.runLength);
                FREE(picParamSet.topLeft);
                FREE(picParamSet.bottomRight);
                FREE(picParamSet.sliceGroupId);
                ret = H264BSD_ERROR;
            }
            else
            {
                tmp = h264bsdStorePicParamSet(pStorage, &picParamSet);
                if(tmp != HANTRO_OK)
                {
                    ERROR_PRINT("PIC_PARAM_SET allocation");
                    ret = H264BSD_ERROR;
                }
            }
            ret = H264BSD_RDY;
            goto NEXT_NAL;

        case NAL_CODED_SLICE_IDR:
            DEBUG_PRINT(("IDR "));
            /* fall through */
        case NAL_CODED_SLICE:
        case NAL_CODED_SLICE_EXT:
            DEBUG_PRINT(("decode slice header\n"));

            if (nalUnit.nalUnitType == NAL_CODED_SLICE_EXT)
            {
                /* base not yet initialized -> skip */
                if (pStorage->activeViewSps[0] == NULL)
                    goto NEXT_NAL;

                pStorage->view = 1;
                /* view_id not equal to view_id of the 1. non-base view or
                 * base view picture missing -> skip */
                if (nalUnit.viewId != pStorage->viewId[pStorage->view] ||
                    (pStorage->numViews && pStorage->nextView == 0))
                {
                    goto NEXT_NAL;
                }
            }
            else
                pStorage->view = 0;

            /* base view detected, but stereo expected -> go back and freeze
             * previous stereo view picture */
            if (pStorage->view == 0 && pStorage->numViews &&
                pStorage->nextView != 0 && pStorage->activeViewSps[0])
            {
                *readBytes = 0;
                pStorage->prevBufNotFinished = HANTRO_TRUE;
                pStorage->view = 1;
                pStorage->dpb = pStorage->dpbs[pStorage->view];

                pStorage->validSliceInAccessUnit = 1;

                if (!pStorage->baseOppositeFieldPic)
                {
                    pStorage->currImage->data =
                        h264bsdAllocateDpbImage(pStorage->dpb);
                }
                else
                {
                    pStorage->baseOppositeFieldPic = 0;
                    pStorage->currImage->data =
                        pStorage->dpb->currentOut->data;
                }
                h264bsdInitRefPicList(pStorage->dpb);

                return (H264BSD_NEW_ACCESS_UNIT);
            }

            /* picture successfully finished and still decoding same old
             * access unit -> no need to decode redundant slices */
            if(pStorage->skipRedundantSlices)
            {
                DEBUG_PRINT(("skipping redundant slice\n"));
                ret = H264BSD_RDY;
                goto NEXT_NAL;
            }

            pStorage->picStarted = HANTRO_TRUE;

            if(h264bsdIsStartOfPicture(pStorage))
            {
                pStorage->numConcealedMbs = 0;
                pStorage->currentPicId = picId;

                tmp = h264bsdCheckPpsId(&strm, &ppsId);
                ASSERT(tmp == HANTRO_OK);
                /* store old activeSpsId and return headers ready
                 * indication if activeSps changes */
                spsId = pStorage->activeViewSpsId[pStorage->view];

                tmp = h264bsdActivateParamSets(pStorage, ppsId,
                                               IS_IDR_NAL_UNIT(&nalUnit) ?
                                               HANTRO_TRUE : HANTRO_FALSE);
                if(tmp != HANTRO_OK || pStorage->activeSps == NULL ||
                   pStorage->activePps == NULL)
                {
                    ERROR_PRINT("Param set activation");
                    ret = H264BSD_PARAM_SET_ERROR;
                    if (pStorage->view &&
                        pStorage->activeSps == NULL)
                        pStorage->numViews = 0;
                    goto NEXT_NAL;
                }

                if(spsId != pStorage->activeSpsId)
                {
                    seqParamSet_t *oldSPS = NULL;
                    seqParamSet_t *newSPS = pStorage->activeSps;
                    u32 noOutputOfPriorPicsFlag = 1;

                    if(pStorage->oldSpsId < MAX_NUM_SEQ_PARAM_SETS)
                    {
                        oldSPS = pStorage->sps[pStorage->oldSpsId];
                    }

                    *readBytes = 0;
                    pStorage->prevBufNotFinished = HANTRO_TRUE;

                    if(IS_IDR_NAL_UNIT(&nalUnit))
                    {
                        tmp =
                            h264bsdCheckPriorPicsFlag(&noOutputOfPriorPicsFlag,
                                                      &strm, newSPS,
                                                      pStorage->activePps,
                                                      NAL_CODED_SLICE_IDR);
                                                      /*nalUnit.nalUnitType);*/
                    }
                    else
                    {
                        tmp = HANTRO_NOK;
                    }

                    if((tmp != HANTRO_OK) ||
                       (noOutputOfPriorPicsFlag != 0) ||
                       (pStorage->dpb->noReordering) ||
                       (oldSPS == NULL) ||
                       (oldSPS->picWidthInMbs != newSPS->picWidthInMbs) ||
                       (oldSPS->picHeightInMbs != newSPS->picHeightInMbs) ||
                       (oldSPS->maxDpbSize != newSPS->maxDpbSize))
                    {
                        pStorage->dpb->flushed = 0;
                    }
                    else
                    {
                        h264bsdFlushDpb(pStorage->dpb);
                    }

                    pStorage->oldSpsId = pStorage->activeSpsId;
                    pStorage->picStarted = HANTRO_FALSE;
                    return (H264BSD_HDRS_RDY);
                }

                if(pStorage->activePps->numSliceGroups != 1 &&
                   !pDecCont->rlcMode)
                {
                    pDecCont->reallocate = 1;
                    pDecCont->tryVlc = 0;
                    pStorage->picStarted = HANTRO_FALSE;
                    *readBytes = 0;
                    return (H264BSD_FMO);
                }

                if(pStorage->activePps->numSliceGroups == 1 &&
                   pDecCont->rlcMode && !pStorage->asoDetected)
                {
                    if(pDecCont->forceRlcMode)
                        pDecCont->tryVlc = 0;
                    else
                    {
                        DEBUG_PRINT(("h264bsdDecode: no FMO/ASO detected, switch to VLC\n"));
                        pDecCont->tryVlc = 1;

                        /* we have removed emulation prevevention 3B from stream,
                         * cannot go to VLC for this frame; but try the
                         * next one */

                        /* pDecCont->rlcMode = 0; */
                    }
                }
            }

            tmp = h264bsdDecodeSliceHeader(&strm, pStorage->sliceHeader + 1,
                                           pStorage->activeSps,
                                           pStorage->activePps, &nalUnit);
            if(tmp != HANTRO_OK)
            {
                ERROR_PRINT("SLICE_HEADER");
                ret = H264BSD_ERROR;
                goto NEXT_NAL;
            }
#if 0//through test,we found  there are some bugs for this skip 
	     //add by franklin,for B frame skip
	     {
	     		sliceHeader_t * tmpSliceHeader = pStorage->sliceHeader + 1;
			if(IS_B_SLICE(tmpSliceHeader->sliceType) && 
				pDecCont->skipNonReference){
				ret = H264BSD_NONREF_PIC_SKIPPED;
				goto NEXT_NAL;
			}
	     }
#endif 
            if(h264bsdIsStartOfPicture(pStorage))
            {
                /* stereo view picture, non-matching field pic flag or bottom
                 * field flag -> freeze previous stereo view frame of field
                 * (frame/field and top/bottom info from base view) */
                if (pStorage->view &&
                    ((pStorage->sliceHeader[1].fieldPicFlag !=
                      pStorage->sliceHeaders[0][1].fieldPicFlag) ||
                     (pStorage->sliceHeader[1].bottomFieldFlag !=
                      pStorage->sliceHeaders[0][1].bottomFieldFlag)))
                {
                    pStorage->validSliceInAccessUnit = 1;
                    pStorage->sliceHeader = pStorage->sliceHeaders[0];

                    if (!pStorage->baseOppositeFieldPic)
                    {
                        pStorage->currImage->data =
                            h264bsdAllocateDpbImage(pStorage->dpb);
                    }
                    else
                    {
                        pStorage->currImage->data =
                            pStorage->dpb->currentOut->data;
                    }
                    h264bsdInitRefPicList(pStorage->dpb);

                    return (H264BSD_NEW_ACCESS_UNIT);
                }

                tmp = pStorage->secondField;
                if(pStorage->view ?
                    pStorage->baseOppositeFieldPic :
                    (pStorage->baseOppositeFieldPic =
                     h264bsdIsOppositeFieldPic(pStorage->sliceHeader + 1,
                                             pStorage->sliceHeader + 0,
                                             &pStorage->secondField,
                                             pStorage->dpb->prevRefFrameNum,
                                             pStorage->aub->newPicture)))
                {
                    /* TODO: make sure that currentOut->status[field] is
                     * EMPTY */
                    if(pStorage->dpb->delayedOut != 0)
                    {
                        /* we delayed the output while waiting for second field */
                        pStorage->dpb->outBuf[pStorage->dpb->delayedId].fieldPicture = 0;   /* mark both fields available */
                        DEBUG_PRINT(("Second field coming...\n"));
                    }
                    pStorage->currImage->data = pStorage->dpb->currentOut->data;
                }
                else
                {
                    if(pStorage->dpb->delayedOut != 0 ||
                        /* missing field and PP running */
                        ((pStorage->numViews == 0 ?
                            tmp : (pStorage->sliceHeader->fieldPicFlag &&
                            pStorage->view == 0)) &&
                         pDecCont->pp.decPpIf.ppStatus == DECPP_PIC_NOT_FINISHED))
                    {
                        pStorage->secondField = 0;
                        DEBUG_PRINT(("Second field missing...Output delayed stuff\n"));
                        *readBytes = 0;
                        pStorage->prevBufNotFinished = HANTRO_TRUE;
                        pStorage->dpb->delayedOut = 0;
                        pStorage->checkedAub = 1;
                        return (H264BSD_UNPAIRED_FIELD);
                    }

                    if(!IS_IDR_NAL_UNIT(&nalUnit) &&
                       !pDecCont->modeChange && !pDecCont->gapsCheckedForThis)
                    {
                        DEBUG_PRINT(("Check gaps in frame num; mode change %d\n",
                                     pDecCont->modeChange));
                        tmp = h264bsdCheckGapsInFrameNum(pStorage->dpb,
                                                         pStorage->
                                                         sliceHeader[1].
                                                         frameNum,
                                                         nalUnit.nalRefIdc !=
                                                         0 ? HANTRO_TRUE :
                                                         HANTRO_FALSE,
                                                         pStorage->activeSps->
                                                         gapsInFrameNumValueAllowedFlag);

                        pDecCont->gapsCheckedForThis = HANTRO_TRUE;
                        if(tmp != HANTRO_OK)
                        {
                            pDecCont->gapsCheckedForThis = HANTRO_FALSE;
                            ERROR_PRINT("Gaps in frame num");
                            ret = H264BSD_ERROR;
                            goto NEXT_NAL;
                        }
                    }

                    pStorage->currImage->data =
                        h264bsdAllocateDpbImage(pStorage->dpb);

#ifdef SET_EMPTY_PICTURE_DATA   /* USE THIS ONLY FOR DEBUGGING PURPOSES */
                    {
                        i32 bgd = SET_EMPTY_PICTURE_DATA;

                        G1DWLmemset(pStorage->currImage->data->virtualAddress,
                                  bgd, pStorage->currImage->data->size);
                    }
#endif
                }
            }
            else
            {
                if(!pDecCont->rlcMode &&
                   pStorage->sliceHeader[1].redundantPicCnt != 0)
                {
                    ret = H264BSD_RDY;
                    goto NEXT_NAL;
                }
            }

            DEBUG_PRINT(("vali slice TRUE\n"));

            /* store slice header to storage if successfully decoded */
            pStorage->sliceHeader[0] = pStorage->sliceHeader[1];
            pStorage->validSliceInAccessUnit = HANTRO_TRUE;
            pStorage->prevNalUnit[0] = nalUnit;

            if(IS_B_SLICE(pStorage->sliceHeader[1].sliceType))
            {
                if((pDecCont->h264ProfileSupport == H264_BASELINE_PROFILE) ||
                   (pDecCont->rlcMode != 0))
                {
                    ERROR_PRINT("B_SLICE not allowed in baseline decoder");
                    ret = H264BSD_ERROR;
                    goto NEXT_NAL;
                }

                if(pDecCont->asicBuff->enableDmvAndPoc == 0)
                {
                    DEBUG_PRINT(("B_SLICE in baseline stream!!! DMV and POC writing were not enabled!"));
                    DEBUG_PRINT(("B_SLICE decoding will not be accurate for a while!"));

                    /* enable DMV and POC writing */
                    pDecCont->asicBuff->enableDmvAndPoc = 1;
                }
            }

            /* For VLC mode, end SW decode here */
            if(!pDecCont->rlcMode)
            {
                DEBUG_PRINT(("\tVLC mode! Skip slice data decoding\n"));
                if(pDecCont->is8190 == 0)
                {
                    h264bsdInitRefPicList(pStorage->dpb);
                }

                SetPicNums(pStorage->dpb, pStorage->sliceHeader->frameNum);

                return (H264BSD_PIC_RDY);
            }
            /* TODO pitäskö siirtää pelkästään rlc moodille, note that
             * SetPicNums called only inside h264bsdReorderRefPicList() */

            h264bsdInitRefPicList(pStorage->dpb);
            tmp = h264bsdReorderRefPicList(pStorage->dpb,
                                           &pStorage->sliceHeader->
                                           refPicListReordering,
                                           pStorage->sliceHeader->frameNum,
                                           pStorage->sliceHeader->
                                           numRefIdxL0Active);
            if(tmp != HANTRO_OK)
            {
                ERROR_PRINT("h264bsdReorderRefPicList failed\n");
                ret = H264BSD_ERROR;
                goto NEXT_NAL;
            }

            h264bsdComputeSliceGroupMap(pStorage,
                                        pStorage->sliceHeader->
                                        sliceGroupChangeCycle);

            tmp = h264bsdDecodeSliceData(pDecCont, &strm,
                                         pStorage->sliceHeader);

            if(tmp != HANTRO_OK)
            {
                h264bsdMarkSliceCorrupted(pStorage,
                                          pStorage->sliceHeader->
                                          firstMbInSlice);
                return (H264BSD_ERROR);
            }

            if(h264bsdIsEndOfPicture(pStorage))
            {
                picReady = HANTRO_TRUE;
                DEBUG_PRINT(("Skip redundant RLC\n"));
                pStorage->skipRedundantSlices = HANTRO_TRUE;
            }
            break;

        case NAL_SEI:
            DEBUG_PRINT(("SEI MESSAGE, NOT DECODED\n"));
            ret = H264BSD_RDY;
            goto NEXT_NAL;
        case NAL_END_OF_SEQUENCE:
            DEBUG_PRINT(("END_OF_SEQUENCE, NOT DECODED\n"));
            ret = H264BSD_RDY;
            goto NEXT_NAL;
        case NAL_END_OF_STREAM:
            DEBUG_PRINT(("END_OF_STREAM, NOT DECODED\n"));
            ret = H264BSD_RDY;
            goto NEXT_NAL;
        case NAL_PREFIX:
            pStorage->view = 0;
            pStorage->nonInterViewRef = nalUnit.interViewFlag == 0;
            goto NEXT_NAL;
        default:
            DEBUG_PRINT(("NOT IMPLEMENTED YET %d\n", nalUnit.nalUnitType));
            ret = H264BSD_RDY;
            goto NEXT_NAL;
        }
    }

    if(picReady)
    {
        return (H264BSD_PIC_RDY);
    }
    else
        return (H264BSD_RDY);

  NEXT_NAL:
    if(!pDecCont->rlcMode)
    {
        const u8 *next =
            h264bsdFindNextStartCode(strm.pStrmBuffStart, strm.strmBuffSize);

        if(next != NULL)
        {
            *readBytes = (u32) (next - byteStrm);
            pStorage->prevBytesConsumed = *readBytes;
        }
    }

    return ret;

}

/*------------------------------------------------------------------------------

    Function: h264bsdShutdown

        Functional description:
            Shutdown a decoder instance. Function frees all the memories
            allocated for the decoder instance.

        Inputs:
            pStorage    pointer to storage data structure

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdShutdown(storage_t * pStorage)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(pStorage);

    for(i = 0; i < MAX_NUM_SEQ_PARAM_SETS; i++)
    {
        if(pStorage->sps[i])
        {
            FREE(pStorage->sps[i]->offsetForRefFrame);
            FREE(pStorage->sps[i]->vuiParameters);
            FREE(pStorage->sps[i]);
        }
    }

    for(i = 0; i < MAX_NUM_PIC_PARAM_SETS; i++)
    {
        if(pStorage->pps[i])
        {
            FREE(pStorage->pps[i]->runLength);
            FREE(pStorage->pps[i]->topLeft);
            FREE(pStorage->pps[i]->bottomRight);
            FREE(pStorage->pps[i]->sliceGroupId);
            FREE(pStorage->pps[i]);
        }
    }

    FREE(pStorage->mb);
    FREE(pStorage->sliceGroupMap);
}

/*------------------------------------------------------------------------------

    Function: h264bsdNextOutputPicture

        Functional description:
            Get next output picture in display order.

        Inputs:
            pStorage    pointer to storage data structure

        Returns:
            pointer to the picture data
            NULL if no pictures available for display

------------------------------------------------------------------------------*/

const dpbOutPicture_t *h264bsdNextOutputPicture(storage_t * pStorage)
{
/* Variables */

    const dpbOutPicture_t *pOut;

/* Code */

    ASSERT(pStorage);

    pOut = h264bsdDpbOutputPicture(pStorage->dpb);

    /* store pointer to alternate chroma output if needed */
    if (pStorage->enable2ndChroma && pOut &&
        (!pStorage->activeSps || !pStorage->activeSps->monoChrome))
    {
        pStorage->pCh2 =
            (u32 *)((u8*)pOut->data->virtualAddress + pStorage->dpb->ch2Offset);
        pStorage->bCh2 = pOut->data->busAddress + pStorage->dpb->ch2Offset;
    }
    else
    {
        pStorage->pCh2 = NULL;
        pStorage->bCh2 = 0;
    }

    return pOut;
}

/*------------------------------------------------------------------------------

    Function: h264bsdPicWidth

        Functional description:
            Get width of the picture in macroblocks

        Inputs:
            pStorage    pointer to storage data structure

        Outputs:
            none

        Returns:
            picture width
            0 if parameters sets not yet activated

------------------------------------------------------------------------------*/

u32 h264bsdPicWidth(storage_t * pStorage)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    if(pStorage->activeSps)
        return (pStorage->activeSps->picWidthInMbs);
    else
        return (0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdPicHeight

        Functional description:
            Get height of the picture in macroblocks

        Inputs:
            pStorage    pointer to storage data structure

        Outputs:
            none

        Returns:
            picture width
            0 if parameters sets not yet activated

------------------------------------------------------------------------------*/

u32 h264bsdPicHeight(storage_t * pStorage)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    if(pStorage->activeSps)
        return (pStorage->activeSps->picHeightInMbs);
    else
        return (0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdIsMonoChrome

        Functional description:

        Inputs:
            pStorage    pointer to storage data structure

        Outputs:

        Returns:

------------------------------------------------------------------------------*/

u32 h264bsdIsMonoChrome(storage_t * pStorage)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    if(pStorage->activeSps)
        return (pStorage->activeSps->monoChrome);
    else
        return (0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdFlushBuffer

        Functional description:
            Flush the decoded picture buffer, see dpb.c for details

        Inputs:
            pStorage    pointer to storage data structure

------------------------------------------------------------------------------*/

void h264bsdFlushBuffer(storage_t * pStorage)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    h264bsdFlushDpb(pStorage->dpbs[0]);
    h264bsdFlushDpb(pStorage->dpbs[1]);
}

/*------------------------------------------------------------------------------

    Function: h264bsdCheckValidParamSets

        Functional description:
            Check if any valid parameter set combinations (SPS/PPS) exists.

        Inputs:
            pStorage    pointer to storage structure

        Returns:
            1       at least one valid SPS/PPS combination found
            0       no valid param set combinations found

------------------------------------------------------------------------------*/

u32 h264bsdCheckValidParamSets(storage_t * pStorage)
{

/* Variables */

/* Code */

    ASSERT(pStorage);

    return (h264bsdValidParamSets(pStorage) == HANTRO_OK ? 1 : 0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdAspectRatioIdc

        Functional description:
            Get value of aspect_ratio_idc received in the VUI data

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            value of aspect_ratio_idc if received
            0   otherwise (this is the default value)

------------------------------------------------------------------------------*/
u32 h264bsdAspectRatioIdc(const storage_t * pStorage)
{
/* Variables */
    const seqParamSet_t *sps;

/* Code */

    ASSERT(pStorage);
    sps = pStorage->activeSps;

    if(sps && sps->vuiParametersPresentFlag &&
       sps->vuiParameters->aspectRatioPresentFlag)
        return (sps->vuiParameters->aspectRatioIdc);
    else    /* default unspecified */
        return (0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdSarSize

        Functional description:
            Get value of sample_aspect_ratio size received in the VUI data

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            values of sample_aspect_ratio size if received
            0   otherwise (this is the default value)

------------------------------------------------------------------------------*/
void h264bsdSarSize(const storage_t * pStorage, u32 * sar_width,
                    u32 * sar_height)
{
/* Variables */
    const seqParamSet_t *sps;

/* Code */

    ASSERT(pStorage);
    sps = pStorage->activeSps;

    if(sps && pStorage->activeSps->vuiParametersPresentFlag &&
       sps->vuiParameters->aspectRatioPresentFlag &&
       sps->vuiParameters->aspectRatioIdc == 255)
    {
        *sar_width = sps->vuiParameters->sarWidth;
        *sar_height = sps->vuiParameters->sarHeight;
    }
    else
    {
        *sar_width = 0;
        *sar_height = 0;
    }

}

/*------------------------------------------------------------------------------

    Function: h264bsdVideoRange

        Functional description:
            Get value of video_full_range_flag received in the VUI data.

        Inputs:
            pStorage    pointer to storage structure

        Returns:
            1   video_full_range_flag received and value is 1
            0   otherwise

------------------------------------------------------------------------------*/

u32 h264bsdVideoRange(storage_t * pStorage)
{
/* Variables */
    const seqParamSet_t *sps;

/* Code */

    ASSERT(pStorage);
    sps = pStorage->activeSps;

    if(sps && sps->vuiParametersPresentFlag &&
       sps->vuiParameters->videoSignalTypePresentFlag &&
       sps->vuiParameters->videoFullRangeFlag)
        return (1);
    else    /* default value of video_full_range_flag is 0 */
        return (0);

}

/*------------------------------------------------------------------------------

    Function: h264bsdMatrixCoefficients

        Functional description:
            Get value of matrix_coefficients received in the VUI data

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            value of matrix_coefficients if received
            2   otherwise (this is the default value)

------------------------------------------------------------------------------*/

u32 h264bsdMatrixCoefficients(storage_t * pStorage)
{
/* Variables */
    const seqParamSet_t *sps;

/* Code */

    ASSERT(pStorage);
    sps = pStorage->activeSps;

    if(sps && sps->vuiParametersPresentFlag &&
       sps->vuiParameters->videoSignalTypePresentFlag &&
       sps->vuiParameters->colourDescriptionPresentFlag)
        return (sps->vuiParameters->matrixCoefficients);
    else    /* default unspecified */
        return (2);

}

/*------------------------------------------------------------------------------

    Function: hh264bsdCroppingParams

        Functional description:
            Get cropping parameters of the active SPS

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            croppingFlag    flag indicating if cropping params present is
                            stored here
            leftOffset      cropping left offset in pixels is stored here
            width           width of the image after cropping is stored here
            topOffset       cropping top offset in pixels is stored here
            height          height of the image after cropping is stored here

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdCroppingParams(storage_t * pStorage, u32 * croppingFlag,
                           u32 * leftOffset, u32 * width, u32 * topOffset,
                           u32 * height)
{
/* Variables */
    const seqParamSet_t *sps;
    u32 tmp1, tmp2;

/* Code */

    ASSERT(pStorage);
    sps = pStorage->activeSps;

    if(sps && sps->frameCroppingFlag)
    {
        tmp1 = sps->monoChrome ? 1 : 2;
        tmp2 = sps->frameMbsOnlyFlag ? 1 : 2;
        *croppingFlag = 1;
        *leftOffset = tmp1 * sps->frameCropLeftOffset;
        *width = 16 * sps->picWidthInMbs -
            tmp1 * (sps->frameCropLeftOffset + sps->frameCropRightOffset);

        *topOffset = tmp1 * tmp2 * sps->frameCropTopOffset;
        *height = 16 * sps->picHeightInMbs -
            tmp1 * tmp2 * (sps->frameCropTopOffset +
                           sps->frameCropBottomOffset);
    }
    else
    {
        *croppingFlag = 0;
        *leftOffset = 0;
        *width = 0;
        *topOffset = 0;
        *height = 0;
    }

}
