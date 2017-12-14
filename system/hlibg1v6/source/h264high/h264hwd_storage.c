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
--  Abstract : Storage handling functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_storage.c,v $
--  $Date: 2011/01/05 08:08:31 $
--  $Revision: 1.20 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "h264hwd_storage.h"
#include "h264hwd_util.h"
#include "h264hwd_neighbour.h"
#include "h264hwd_slice_group_map.h"
#include "h264hwd_dpb.h"
#include "h264hwd_nal_unit.h"
#include "h264hwd_slice_header.h"
#include "h264hwd_seq_param_set.h"
#include "dwl.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

static u32 CheckPps(picParamSet_t * pps, seqParamSet_t * sps);

/*------------------------------------------------------------------------------

    Function name: h264bsdInitStorage

        Functional description:
            Initialize storage structure. Sets contents of the storage to '0'
            except for the active parameter set ids, which are initialized
            to invalid values.

        Inputs:

        Outputs:
            pStorage    initialized data stored here

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdInitStorage(storage_t * pStorage)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(pStorage);

    (void) G1DWLmemset(pStorage, 0, sizeof(storage_t));

    pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
    pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;
    for (i = 0; i < MAX_NUM_VIEWS; i++)
        pStorage->activeViewSpsId[i] = MAX_NUM_SEQ_PARAM_SETS;
    pStorage->oldSpsId = MAX_NUM_SEQ_PARAM_SETS;
    pStorage->aub->firstCallFlag = HANTRO_TRUE;
}

/*------------------------------------------------------------------------------

    Function: h264bsdStoreSeqParamSet

        Functional description:
            Store sequence parameter set into the storage. If active SPS is
            overwritten -> check if contents changes and if it does, set
            parameters to force reactivation of parameter sets

        Inputs:
            pStorage        pointer to storage structure
            pSeqParamSet    pointer to param set to be stored

        Outputs:
            none

        Returns:
            HANTRO_OK                success
            MEMORY_ALLOCATION_ERROR  failure in memory allocation

------------------------------------------------------------------------------*/

u32 h264bsdStoreSeqParamSet(storage_t * pStorage, seqParamSet_t * pSeqParamSet)
{

/* Variables */

    u32 id;

/* Code */

    ASSERT(pStorage);
    ASSERT(pSeqParamSet);
    ASSERT(pSeqParamSet->seqParameterSetId < MAX_NUM_SEQ_PARAM_SETS);

    id = pSeqParamSet->seqParameterSetId;

    /* seq parameter set with id not used before -> allocate memory */
    if(pStorage->sps[id] == NULL)
    {
        ALLOCATE(pStorage->sps[id], 1, seqParamSet_t);
        if(pStorage->sps[id] == NULL)
            return (MEMORY_ALLOCATION_ERROR);
    }
    /* sequence parameter set with id equal to id of active sps */
    else if(id == pStorage->activeViewSpsId[0] ||
            id == pStorage->activeViewSpsId[1] )
    {
        /* if seq parameter set contents changes
         *    -> overwrite and re-activate when next IDR picture decoded
         *    ids of active param sets set to invalid values to force
         *    re-activation. Memories allocated for old sps freed
         * otherwise free memeries allocated for just decoded sps and
         * continue */
        u32 viewId = id == pStorage->activeViewSpsId[1];
        if(h264bsdCompareSeqParamSets(pSeqParamSet,
                pStorage->activeViewSps[viewId]) != 0)
        {
            FREE(pStorage->sps[id]->offsetForRefFrame);
            FREE(pStorage->sps[id]->vuiParameters);
            /* overwriting active sps of current view */
            if (viewId == pStorage->view)
            {
                pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS + 1;
                pStorage->activeSps = NULL;
                pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS + 1;
                pStorage->activePps = NULL;
            }
            pStorage->activeViewSpsId[viewId] = MAX_NUM_SEQ_PARAM_SETS + 1;
            pStorage->activeViewSps[pStorage->view] = NULL;
        }
        else
        {
            FREE(pSeqParamSet->offsetForRefFrame);
            FREE(pSeqParamSet->vuiParameters);
            return (HANTRO_OK);
        }
    }
    /* overwrite seq param set other than active one -> free memories
     * allocated for old param set */
    else
    {
        FREE(pStorage->sps[id]->offsetForRefFrame);
        FREE(pStorage->sps[id]->vuiParameters);
    }

    *pStorage->sps[id] = *pSeqParamSet;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: h264bsdStorePicParamSet

        Functional description:
            Store picture parameter set into the storage. If active PPS is
            overwritten -> check if active SPS changes and if it does -> set
            parameters to force reactivation of parameter sets

        Inputs:
            pStorage        pointer to storage structure
            pPicParamSet    pointer to param set to be stored

        Outputs:
            none

        Returns:
            HANTRO_OK                success
            MEMORY_ALLOCATION_ERROR  failure in memory allocation

------------------------------------------------------------------------------*/

void h264bsdModifyScalingLists(storage_t *pStorage, picParamSet_t *pPicParamSet)
{
    u32 i;
    seqParamSet_t *sps;

    sps = pStorage->sps[pPicParamSet->seqParameterSetId];
    /* SPS not yet decoded -> cannot copy */
    /* TODO: set flag to handle "missing" SPS lists properly */
    if (sps == NULL)
        return;

    if (!pPicParamSet->scalingMatrixPresentFlag &&
        sps->scalingMatrixPresentFlag)
    {
        pPicParamSet->scalingMatrixPresentFlag = 1;
        (void)G1DWLmemcpy(pPicParamSet->scalingList, sps->scalingList,
            sizeof(sps->scalingList));
    }
    else if (sps->scalingMatrixPresentFlag)
    {
        if (!pPicParamSet->scalingListPresent[0])
        {
            /* we trust our memcpy */
            (void)G1DWLmemcpy(pPicParamSet->scalingList[0], sps->scalingList[0],
                16*sizeof(u8));
            for (i = 1; i < 3; i++)
                if (!pPicParamSet->scalingListPresent[i])
                    (void)G1DWLmemcpy(pPicParamSet->scalingList[i],
                        pPicParamSet->scalingList[i-1],
                        16*sizeof(u8));
        }
        if (!pPicParamSet->scalingListPresent[3])
        {
            (void)G1DWLmemcpy(pPicParamSet->scalingList[3], sps->scalingList[3],
                16*sizeof(u8));
            for (i = 4; i < 6; i++)
                if (!pPicParamSet->scalingListPresent[i])
                    (void)G1DWLmemcpy(pPicParamSet->scalingList[i],
                        pPicParamSet->scalingList[i-1],
                        16*sizeof(u8));
        }
        for (i = 6; i < 8; i++)
            if (!pPicParamSet->scalingListPresent[i])
                (void)G1DWLmemcpy(pPicParamSet->scalingList[i], sps->scalingList[i],
                    64*sizeof(u8));
    }
}

u32 h264bsdStorePicParamSet(storage_t * pStorage, picParamSet_t * pPicParamSet)
{

/* Variables */

    u32 id;

/* Code */

    ASSERT(pStorage);
    ASSERT(pPicParamSet);
    ASSERT(pPicParamSet->picParameterSetId < MAX_NUM_PIC_PARAM_SETS);
    ASSERT(pPicParamSet->seqParameterSetId < MAX_NUM_SEQ_PARAM_SETS);

    id = pPicParamSet->picParameterSetId;

    /* pic parameter set with id not used before -> allocate memory */
    if(pStorage->pps[id] == NULL)
    {
        ALLOCATE(pStorage->pps[id], 1, picParamSet_t);
        if(pStorage->pps[id] == NULL)
            return (MEMORY_ALLOCATION_ERROR);
    }
    /* picture parameter set with id equal to id of active pps */
    else if(id == pStorage->activePpsId)
    {
        /* check whether seq param set changes, force re-activation of
         * param set if it does. Set activeSpsId to invalid value to
         * accomplish this */
        if(pPicParamSet->seqParameterSetId != pStorage->activeSpsId)
        {
            pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS + 1;
        }
        /* free memories allocated for old param set */
        FREE(pStorage->pps[id]->runLength);
        FREE(pStorage->pps[id]->topLeft);
        FREE(pStorage->pps[id]->bottomRight);
        FREE(pStorage->pps[id]->sliceGroupId);
    }
    /* overwrite pic param set other than active one -> free memories
     * allocated for old param set */
    else
    {
        FREE(pStorage->pps[id]->runLength);
        FREE(pStorage->pps[id]->topLeft);
        FREE(pStorage->pps[id]->bottomRight);
        FREE(pStorage->pps[id]->sliceGroupId);
    }

    /* Modify scaling_lists if necessary */
    h264bsdModifyScalingLists(pStorage, pPicParamSet);

    *pStorage->pps[id] = *pPicParamSet;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: h264bsdActivateParamSets

        Functional description:
            Activate certain SPS/PPS combination. This function shall be
            called in the beginning of each picture. Picture parameter set
            can be changed as wanted, but sequence parameter set may only be
            changed when the starting picture is an IDR picture.

            When new SPS is activated the function allocates memory for
            macroblock storages and slice group map and (re-)initializes the
            decoded picture buffer. If this is not the first activation the old
            allocations are freed and FreeDpb called before new allocations.

        Inputs:
            pStorage        pointer to storage data structure
            ppsId           identifies the PPS to be activated, SPS id obtained
                            from the PPS
            isIdr           flag to indicate if the picture is an IDR picture

        Outputs:
            none

        Returns:
            HANTRO_OK       success
            HANTRO_NOK      non-existing or invalid param set combination,
                            trying to change SPS with non-IDR picture
            MEMORY_ALLOCATION_ERROR     failure in memory allocation

------------------------------------------------------------------------------*/

u32 h264bsdActivateParamSets(storage_t * pStorage, u32 ppsId, u32 isIdr)
{
    u32 tmp;

    ASSERT(pStorage);
    ASSERT(ppsId < MAX_NUM_PIC_PARAM_SETS);

    /* check that pps and corresponding sps exist */
    if((pStorage->pps[ppsId] == NULL) ||
       (pStorage->sps[pStorage->pps[ppsId]->seqParameterSetId] == NULL))
    {
        return (HANTRO_NOK);
    }

    /* check that pps parameters do not violate picture size constraints */
    tmp = CheckPps(pStorage->pps[ppsId],
                   pStorage->sps[pStorage->pps[ppsId]->seqParameterSetId]);
    if(tmp != HANTRO_OK)
        return (tmp);

    /* first activation */
    if(pStorage->activePpsId == MAX_NUM_PIC_PARAM_SETS)
    {
        pStorage->activePpsId = ppsId;
        pStorage->activePps = pStorage->pps[ppsId];
        pStorage->activeSpsId = pStorage->activePps->seqParameterSetId;
        pStorage->activeViewSpsId[pStorage->view] =
            pStorage->activePps->seqParameterSetId;
        pStorage->activeSps = pStorage->sps[pStorage->activeSpsId];
        pStorage->activeViewSps[pStorage->view] =
            pStorage->sps[pStorage->activeSpsId];
    }
    else if(ppsId != pStorage->activePpsId)
    {
        /* sequence parameter set shall not change but before an IDR picture */
        if (pStorage->pps[ppsId]->seqParameterSetId !=
            pStorage->activeViewSpsId[pStorage->view])
        {
            DEBUG_PRINT(("SEQ PARAM SET CHANGING...\n"));
            if(isIdr)
            {
                pStorage->activePpsId = ppsId;
                pStorage->activePps = pStorage->pps[ppsId];
                pStorage->activeViewSpsId[pStorage->view] =
                    pStorage->activePps->seqParameterSetId;
                pStorage->activeViewSps[pStorage->view] =
                    pStorage->sps[pStorage->activeViewSpsId[pStorage->view]];
            }
            else
            {
                if (pStorage->view && pStorage->activeViewSps[1] == NULL)
                    pStorage->view = 0;
                DEBUG_PRINT(("TRYING TO CHANGE SPS IN NON-IDR SLICE\n"));
                return (HANTRO_NOK);
            }
        }
        else
        {
            pStorage->activePpsId = ppsId;
            pStorage->activePps = pStorage->pps[ppsId];
        }
    }
    /* In case this view uses same PPS as a previous view, and SPS has 
     * not been activated for this view yet. */
    else if(pStorage->activeViewSps[pStorage->view] == NULL)
    {
        pStorage->activeSpsId = pStorage->activePps->seqParameterSetId;
        pStorage->activeViewSpsId[pStorage->view] =
            pStorage->activePps->seqParameterSetId;
        pStorage->activeSps = pStorage->sps[pStorage->activeSpsId];
        pStorage->activeViewSps[pStorage->view] =
            pStorage->sps[pStorage->activeSpsId];
    }

    if (/*isIdr ||*/ pStorage->view)
    {
        pStorage->numViews = pStorage->view != 0;
    }

    pStorage->activeSpsId = pStorage->activeViewSpsId[pStorage->view];
    pStorage->activeSps = pStorage->activeViewSps[pStorage->view];
    pStorage->dpb = pStorage->dpbs[pStorage->view];
    pStorage->sliceHeader = pStorage->sliceHeaders[pStorage->view];

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: h264bsdResetStorage

        Functional description:
            Reset contents of the storage. This should be called before
            processing of new image is started.

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdResetStorage(storage_t * pStorage)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(pStorage);

    pStorage->slice->numDecodedMbs = 0;
    pStorage->slice->sliceId = 0;
#ifdef FFWD_WORKAROUND
    pStorage->prevIdrPicReady = HANTRO_FALSE;
#endif /* FFWD_WORKAROUND */

    if(pStorage->mb != NULL)
    {
        for(i = 0; i < pStorage->picSizeInMbs; i++)
        {
            pStorage->mb[i].sliceId = 0;
            pStorage->mb[i].decoded = 0;
        }
    }
}

/*------------------------------------------------------------------------------

    Function: h264bsdIsStartOfPicture

        Functional description:
            Determine if the decoder is in the start of a picture. This
            information is needed to decide if h264bsdActivateParamSets and
            h264bsdCheckGapsInFrameNum functions should be called. Function
            considers that new picture is starting if no slice headers
            have been successfully decoded for the current access unit.

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            none

        Returns:
            HANTRO_TRUE        new picture is starting
            HANTRO_FALSE       not starting

------------------------------------------------------------------------------*/

u32 h264bsdIsStartOfPicture(storage_t * pStorage)
{

/* Variables */

/* Code */

    if(pStorage->validSliceInAccessUnit == HANTRO_FALSE)
        return (HANTRO_TRUE);
    else
        return (HANTRO_FALSE);

}

/*------------------------------------------------------------------------------

    Function: h264bsdIsEndOfPicture

        Functional description:
            Determine if the decoder is in the end of a picture. This
            information is needed to determine when deblocking filtering
            and reference picture marking processes should be performed.

            If the decoder is processing primary slices the return value
            is determined by checking the value of numDecodedMbs in the
            storage. On the other hand, if the decoder is processing
            redundant slices the numDecodedMbs may not contain valid
            informationa and each macroblock has to be checked separately.

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            none

        Returns:
            HANTRO_TRUE        end of picture
            HANTRO_FALSE       noup

------------------------------------------------------------------------------*/

u32 h264bsdIsEndOfPicture(storage_t * pStorage)
{

/* Variables */

    u32 i, tmp;

/* Code */

    ASSERT(pStorage != NULL);

    /* primary picture */
    if(!pStorage->sliceHeader[0].redundantPicCnt)
    {
        if(pStorage->slice->numDecodedMbs == pStorage->picSizeInMbs)
            return (HANTRO_TRUE);
    }
    else
    {
        ASSERT(pStorage->mb != NULL);

        for(i = 0, tmp = 0; i < pStorage->picSizeInMbs; i++)
            tmp += pStorage->mb[i].decoded ? 1 : 0;

        if(tmp == pStorage->picSizeInMbs)
            return (HANTRO_TRUE);
    }

    return (HANTRO_FALSE);

}

/*------------------------------------------------------------------------------

    Function: h264bsdComputeSliceGroupMap

        Functional description:
            Compute slice group map. Just call h264bsdDecodeSliceGroupMap with
            appropriate parameters.

        Inputs:
            pStorage                pointer to storage structure
            sliceGroupChangeCycle

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdComputeSliceGroupMap(storage_t * pStorage,
                                 u32 sliceGroupChangeCycle)
{

/* Variables */

/* Code */

    h264bsdDecodeSliceGroupMap(pStorage->sliceGroupMap,
                               pStorage->activePps, sliceGroupChangeCycle,
                               pStorage->activeSps->picWidthInMbs,
                               pStorage->activeSps->picHeightInMbs);

}

/*------------------------------------------------------------------------------

    Function: h264bsdCheckAccessUnitBoundary

        Functional description:
            Check if next NAL unit starts a new access unit. Following
            conditions specify start of a new access unit:

                -NAL unit types 6-11, 13-18 (e.g. SPS, PPS)

           following conditions checked only for slice NAL units, values
           compared to ones obtained from previous slice:

                -NAL unit type differs (slice / IDR slice)
                -frame_num differs
                -nal_ref_idc differs and one of the values is 0
                -POC information differs
                -both are IDR slices and idr_pic_id differs

        Inputs:
            strm        pointer to stream data structure
            nuNext      pointer to NAL unit structure
            storage     pointer to storage structure

        Outputs:
            accessUnitBoundaryFlag  the result is stored here, HANTRO_TRUE for
                                    access unit boundary, HANTRO_FALSE otherwise

        Returns:
            HANTRO_OK           success
            HANTRO_NOK          failure, invalid stream data
            PARAM_SET_ERROR     invalid param set usage

------------------------------------------------------------------------------*/

u32 h264bsdCheckAccessUnitBoundary(strmData_t * strm,
                                   nalUnit_t * nuNext,
                                   storage_t * storage,
                                   u32 * accessUnitBoundaryFlag)
{

/* Variables */

    u32 tmp, ppsId, frameNum, idrPicId, picOrderCntLsb;
    u32 fieldPicFlag = 0, bottomFieldFlag = 0;
    i32 deltaPicOrderCntBottom, deltaPicOrderCnt[2];
#ifdef FFWD_WORKAROUND
    u32 firstMbInSlice = 0;
    u32 redundantPicCnt = 0;
#endif /* FFWD_WORKAROUND */
    seqParamSet_t *sps;
    picParamSet_t *pps;
    u32 view = 0;

/* Code */

    ASSERT(strm);
    ASSERT(nuNext);
    ASSERT(storage);
    ASSERT(storage->sps);
    ASSERT(storage->pps);

    DEBUG_PRINT(("h264bsdCheckAccessUnitBoundary #\n"));
    /* initialize default output to HANTRO_FALSE */
    *accessUnitBoundaryFlag = HANTRO_FALSE;

    /* TODO field_pic_flag, bottom_field_flag */

    if(((nuNext->nalUnitType > 5) && (nuNext->nalUnitType < 12)) ||
       ((nuNext->nalUnitType > 12) && (nuNext->nalUnitType <= 18)))
    {
        *accessUnitBoundaryFlag = HANTRO_TRUE;
        return (HANTRO_OK);
    }
    else if(nuNext->nalUnitType != NAL_CODED_SLICE &&
            nuNext->nalUnitType != NAL_CODED_SLICE_IDR &&
            nuNext->nalUnitType != NAL_CODED_SLICE_EXT)
    {
        return (HANTRO_OK);
    }

    /* check if this is the very first call to this function */
    if(storage->aub->firstCallFlag)
    {
        *accessUnitBoundaryFlag = HANTRO_TRUE;
        storage->aub->firstCallFlag = HANTRO_FALSE;
    }

    /* get picture parameter set id */
    tmp = h264bsdCheckPpsId(strm, &ppsId);
    if(tmp != HANTRO_OK)
        return (tmp);

    if (nuNext->nalUnitType == NAL_CODED_SLICE_EXT)
        view = 1;

    /* store sps and pps in separate pointers just to make names shorter */
    pps = storage->pps[ppsId];
    if(pps == NULL || storage->sps[pps->seqParameterSetId] == NULL ||
       (storage->activeViewSpsId[view] != MAX_NUM_SEQ_PARAM_SETS &&
        pps->seqParameterSetId != storage->activeViewSpsId[view] &&
        nuNext->nalUnitType != NAL_CODED_SLICE_IDR))
        return (PARAM_SET_ERROR);
    sps = storage->sps[pps->seqParameterSetId];

    /* another view does not start new access unit unless new viewId is
     * smaller than previous, but other views are handled like new access units
     * (param set activation etc) */
    if(storage->aub->nuPrev->viewId != nuNext->viewId)
        *accessUnitBoundaryFlag = HANTRO_TRUE;

    if(storage->aub->nuPrev->nalRefIdc != nuNext->nalRefIdc &&
       (storage->aub->nuPrev->nalRefIdc == 0 || nuNext->nalRefIdc == 0))
    {
        *accessUnitBoundaryFlag = HANTRO_TRUE;
        storage->aub->newPicture = HANTRO_TRUE;
    }
    else
        storage->aub->newPicture = HANTRO_FALSE;

    if((storage->aub->nuPrev->nalUnitType == NAL_CODED_SLICE_IDR &&
        nuNext->nalUnitType != NAL_CODED_SLICE_IDR) ||
       (storage->aub->nuPrev->nalUnitType != NAL_CODED_SLICE_IDR &&
        nuNext->nalUnitType == NAL_CODED_SLICE_IDR))
        *accessUnitBoundaryFlag = HANTRO_TRUE;

    tmp = h264bsdCheckFrameNum(strm, sps->maxFrameNum, &frameNum);
    if(tmp != HANTRO_OK)
        return (HANTRO_NOK);

    if(storage->aub->prevFrameNum != frameNum)
    {
        storage->aub->prevFrameNum = frameNum;
        *accessUnitBoundaryFlag = HANTRO_TRUE;
    }

    tmp = h264bsdCheckFieldPicFlag(strm, sps->maxFrameNum, nuNext->nalUnitType,
        !sps->frameMbsOnlyFlag, &fieldPicFlag);

    if (fieldPicFlag != storage->aub->prevFieldPicFlag)
    {
        storage->aub->prevFieldPicFlag = fieldPicFlag;
        *accessUnitBoundaryFlag = HANTRO_TRUE;
    }

    tmp = h264bsdCheckBottomFieldFlag(strm, sps->maxFrameNum,
        nuNext->nalUnitType,
        !sps->frameMbsOnlyFlag, &bottomFieldFlag);

    DEBUG_PRINT(("FIELD %d bottom %d\n",fieldPicFlag, bottomFieldFlag));

    if (bottomFieldFlag != storage->aub->prevBottomFieldFlag)
    {
        storage->aub->prevBottomFieldFlag = bottomFieldFlag;
        *accessUnitBoundaryFlag = HANTRO_TRUE;
    }

    if(nuNext->nalUnitType == NAL_CODED_SLICE_IDR)
    {
        tmp = h264bsdCheckIdrPicId(strm, sps->maxFrameNum,
            nuNext->nalUnitType, !sps->frameMbsOnlyFlag, &idrPicId);
        if(tmp != HANTRO_OK)
            return (HANTRO_NOK);

        if(storage->aub->nuPrev->nalUnitType == NAL_CODED_SLICE_IDR &&
           storage->aub->prevIdrPicId != idrPicId)
            *accessUnitBoundaryFlag = HANTRO_TRUE;

#ifdef FFWD_WORKAROUND
        /* FFWD workaround */
        if(!*accessUnitBoundaryFlag )
        {
            /* if prev IDR pic ready and first MB is zero */
            tmp = h264bsdCheckFirstMbInSlice( strm,
                nuNext->nalUnitType,
                &firstMbInSlice );
            if( tmp != HANTRO_OK )
                return (HANTRO_NOK);
            if(storage->prevIdrPicReady && firstMbInSlice == 0)
            {
                /* Just to make sure, check that next IDR is not marked as
                 * redundant */
                tmp = h264bsdCheckRedundantPicCnt( strm,
                    sps, pps, nuNext->nalUnitType,
                    &redundantPicCnt );
                if( tmp != HANTRO_OK )
                    return (HANTRO_NOK);
                if( redundantPicCnt == 0 )
                {
                    *accessUnitBoundaryFlag = HANTRO_TRUE;
                }
            }
        }
#endif /* FFWD_WORKAROUND */

        storage->aub->prevIdrPicId = idrPicId;
    }

    if(sps->picOrderCntType == 0)
    {
        tmp = h264bsdCheckPicOrderCntLsb(strm, sps, nuNext->nalUnitType,
                                         &picOrderCntLsb);
        if(tmp != HANTRO_OK)
            return (HANTRO_NOK);

        if(storage->aub->prevPicOrderCntLsb != picOrderCntLsb)
        {
            storage->aub->prevPicOrderCntLsb = picOrderCntLsb;
            *accessUnitBoundaryFlag = HANTRO_TRUE;
        }

        if(pps->picOrderPresentFlag)
        {
            tmp = h264bsdCheckDeltaPicOrderCntBottom(strm, sps,
                                                     nuNext->nalUnitType,
                                                     &deltaPicOrderCntBottom);
            if(tmp != HANTRO_OK)
                return (tmp);

            if(storage->aub->prevDeltaPicOrderCntBottom !=
               deltaPicOrderCntBottom)
            {
                storage->aub->prevDeltaPicOrderCntBottom =
                    deltaPicOrderCntBottom;
                *accessUnitBoundaryFlag = HANTRO_TRUE;
            }
        }
    }
    else if(sps->picOrderCntType == 1 && !sps->deltaPicOrderAlwaysZeroFlag)
    {
        tmp = h264bsdCheckDeltaPicOrderCnt(strm, sps, nuNext->nalUnitType,
                                           pps->picOrderPresentFlag,
                                           deltaPicOrderCnt);
        if(tmp != HANTRO_OK)
            return (tmp);

        if(storage->aub->prevDeltaPicOrderCnt[0] != deltaPicOrderCnt[0])
        {
            storage->aub->prevDeltaPicOrderCnt[0] = deltaPicOrderCnt[0];
            *accessUnitBoundaryFlag = HANTRO_TRUE;
        }

        if(pps->picOrderPresentFlag)
            if(storage->aub->prevDeltaPicOrderCnt[1] != deltaPicOrderCnt[1])
            {
                storage->aub->prevDeltaPicOrderCnt[1] = deltaPicOrderCnt[1];
                *accessUnitBoundaryFlag = HANTRO_TRUE;
            }
    }

    *storage->aub->nuPrev = *nuNext;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: CheckPps

        Functional description:
            Check picture parameter set. Contents of the picture parameter
            set information that depends on the image dimensions is checked
            against the dimensions in the sps.

        Inputs:
            pps     pointer to picture paramter set
            sps     pointer to sequence parameter set

        Outputs:
            none

        Returns:
            HANTRO_OK      everything ok
            HANTRO_NOK     invalid data in picture parameter set

------------------------------------------------------------------------------*/
u32 CheckPps(picParamSet_t * pps, seqParamSet_t * sps)
{

    u32 i;
    u32 picSize;

    picSize = sps->picWidthInMbs * sps->picHeightInMbs;

    /* check slice group params */
    if(pps->numSliceGroups > 1)
    {
        /* no FMO supported if stream may contain interlaced stuff */
        if (sps->frameMbsOnlyFlag == 0)
            return(HANTRO_NOK);

        if(pps->sliceGroupMapType == 0)
        {
            ASSERT(pps->runLength);
            for(i = 0; i < pps->numSliceGroups; i++)
            {
                if(pps->runLength[i] > picSize)
                    return (HANTRO_NOK);
            }
        }
        else if(pps->sliceGroupMapType == 2)
        {
            ASSERT(pps->topLeft);
            ASSERT(pps->bottomRight);
            for(i = 0; i < pps->numSliceGroups - 1; i++)
            {
                if(pps->topLeft[i] > pps->bottomRight[i] ||
                   pps->bottomRight[i] >= picSize)
                    return (HANTRO_NOK);

                if((pps->topLeft[i] % sps->picWidthInMbs) >
                   (pps->bottomRight[i] % sps->picWidthInMbs))
                    return (HANTRO_NOK);
            }
        }
        else if(pps->sliceGroupMapType > 2 && pps->sliceGroupMapType < 6)
        {
            if(pps->sliceGroupChangeRate > picSize)
                return (HANTRO_NOK);
        }
        else if (pps->sliceGroupMapType == 6 &&
                 pps->picSizeInMapUnits < picSize)
            return(HANTRO_NOK);
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: h264bsdValidParamSets

        Functional description:
            Check if any valid SPS/PPS combination exists in the storage.
            Function tries each PPS in the buffer and checks if corresponding
            SPS exists and calls CheckPps to determine if the PPS conforms
            to image dimensions of the SPS.

        Inputs:
            pStorage    pointer to storage structure

        Outputs:
            HANTRO_OK   there is at least one valid combination
            HANTRO_NOK  no valid combinations found

------------------------------------------------------------------------------*/

u32 h264bsdValidParamSets(storage_t * pStorage)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(pStorage);

    for(i = 0; i < MAX_NUM_PIC_PARAM_SETS; i++)
    {
        if(pStorage->pps[i] &&
           pStorage->sps[pStorage->pps[i]->seqParameterSetId] &&
           CheckPps(pStorage->pps[i],
                    pStorage->sps[pStorage->pps[i]->seqParameterSetId]) ==
           HANTRO_OK)
        {
            return (HANTRO_OK);
        }
    }

    return (HANTRO_NOK);

}

/*------------------------------------------------------------------------------
    Function name   : h264bsdAllocateSwResources
    Description     :
    Return type     : u32
    Argument        : const void *dwl
    Argument        : storage_t * pStorage
    Argument        : u32 isHighSupported
------------------------------------------------------------------------------*/
u32 h264bsdAllocateSwResources(const void *dwl, storage_t * pStorage,
                               u32 isHighSupported)
{
    u32 tmp;
    u32 noReorder;
    const seqParamSet_t *pSps = pStorage->activeSps;
    u32 maxDpbSize;

    pStorage->picSizeInMbs = pSps->picWidthInMbs * pSps->picHeightInMbs;
    pStorage->currImage->width = pSps->picWidthInMbs;
    pStorage->currImage->height = pSps->picHeightInMbs;

    /* dpb output reordering disabled if
     * 1) application set noReordering flag
     * 2) POC type equal to 2
     * 3) num_reorder_frames in vui equal to 0 */
    if(pStorage->noReordering ||
       pSps->picOrderCntType == 2 ||
       (pSps->vuiParametersPresentFlag &&
        pSps->vuiParameters->bitstreamRestrictionFlag &&
        !pSps->vuiParameters->numReorderFrames))
        noReorder = HANTRO_TRUE;
    else
        noReorder = HANTRO_FALSE;

    if (pStorage->view == 0)
        maxDpbSize = pSps->maxDpbSize;
    else
        /* stereo view dpb size at least equal to base view size (to make sure
         * that base view pictures get output in correct display order) */
        maxDpbSize = MAX(pSps->maxDpbSize, pStorage->activeViewSps[0]->maxDpbSize);

    /* note that calling ResetDpb here results in losing all
     * pictures currently in DPB -> nothing will be output from
     * the buffer even if noOutputOfPriorPicsFlag is HANTRO_FALSE */
    tmp = h264bsdResetDpb(dwl, pStorage->dpb, pStorage->picSizeInMbs,
                          maxDpbSize, pSps->numRefFrames,
                          pSps->maxFrameNum, noReorder, pStorage->useSmoothing,
                          pSps->monoChrome, isHighSupported,
                          pStorage->enable2ndChroma && !pSps->monoChrome,
                          pStorage->multiBuffPp);
    if(tmp != HANTRO_OK)
        return (tmp);

    return HANTRO_OK;
}
