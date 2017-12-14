/**
*******************************************************************************
@file ci_pipeline.c

@brief Implementation of the CI_PIPELINE functions

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"

#ifdef IMG_KERNEL_MODULE
#error "this should not be in a kernel module"
#endif

#include "ci_internal/ci_errors.h"  // toErrno and toImgResult
#include "ci_kernel/ci_ioctrl.h"

#include <ctx_reg_precisions.h>
#include <hw_struct/save.h>

#define LOG_TAG "CI_API"
#include <felixcommon/userlog.h>

/*-----------------------------------------------------------------------------
 * Following elements are in static scope
 *---------------------------------------------------------------------------*/

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_ClearBuffers(void *listBuffer, void *param)
{
    INT_BUFFER *pBuffer = (INT_BUFFER*)listBuffer;
    INT_CONNECTION *pIntCon = (INT_CONNECTION*)param;

    if (pIntCon != NULL && pBuffer->memory != NULL)
    {
        SYS_IO_MemUnmap(pIntCon->fileDesc, pBuffer->memory, pBuffer->uiSize);
    }

    IMG_FREE(pBuffer);
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_ClearLSHMat(void *listBuffer, void *param)
{
    INT_LSHMAT *pBuffer = (INT_LSHMAT*)listBuffer;
    INT_CONNECTION *pIntCon = (INT_CONNECTION*)param;

    if (pIntCon != NULL && pBuffer->memory != NULL)
    {
        SYS_IO_MemUnmap(pIntCon->fileDesc,
            pBuffer->memory,
            pBuffer->uiSize);
    }

    IMG_FREE(pBuffer);
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_FindAvailableBuffer(void *listBuffer, void *param)
{
    INT_BUFFER *pBuff = (INT_BUFFER*)listBuffer;
    int type = *((int*)param);

    if (pBuff->type == type && pBuff->eStatus == INT_BUFFER_AVAILABLE)
    {
        return IMG_FALSE;  // we found one - we stop
    }
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_FindAvailableHDRBuffer(void *listBuffer, void *param)
{
    INT_BUFFER *pBuff = (INT_BUFFER*)listBuffer;

    if (pBuff->type == CI_ALLOC_HDRINS && pBuff->bHDRReserved == IMG_FALSE
        && pBuff->eStatus == INT_BUFFER_AVAILABLE)
    {
        return IMG_FALSE;  // we found one - we stop
    }
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_SearchShot(void *listelem, void *lookingFor)
{
    int *identifier = (int*)lookingFor;
    INT_SHOT *pShot = (INT_SHOT*)listelem;

    if (pShot->iIdentifier == *identifier) return IMG_FALSE;
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_SearchFirstAcquiredShot(void *listelem, void *notUsed)
{
    INT_SHOT *pShot = (INT_SHOT*)listelem;

    if (pShot->bAcquired) return IMG_FALSE;
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_SearchBuffer(void *listBuffer, void *param)
{
    INT_BUFFER *pBuff = (INT_BUFFER*)listBuffer;
    IMG_INT32 id = *((IMG_INT32*)param);

    if (pBuff->ID == id)
    {
        return IMG_FALSE;  // we found it, we stop
    }
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_SearchLSHMatrix(void *listBuffer, void *param)
{
    INT_LSHMAT *pBuff = (INT_LSHMAT*)listBuffer;
    IMG_INT32 id = *((IMG_INT32*)param);

    if (pBuff->ID == id)
    {
        return IMG_FALSE;  // we found it, we stop
    }
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_SearchBufferIonFd(void *listBuffer, void *param)
{
    INT_BUFFER *pBuff = (INT_BUFFER*)listBuffer;
    IMG_INT32 ionFd = *((IMG_UINT32*)param);

    if (pBuff->ionFd == ionFd)
    {
        return IMG_FALSE;  // we found it, we stop
    }
    return IMG_TRUE;
}


/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_ChangeBufferStatus(void *listBuffer, void *param)
{
    INT_BUFFER *pBuff = (INT_BUFFER*)listBuffer;
    int *status = ((int*)param);

    if (pBuff->eStatus == (enum INT_BUFFER_STATUS)status[0])
    {
        pBuff->eStatus = (enum INT_BUFFER_STATUS)status[1];
    }
    return IMG_TRUE;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 List_ClearShots(void *listShot, void *param)
{
    INT_SHOT *pShot = (INT_SHOT*)listShot;
    INT_CONNECTION *pIntCon = (INT_CONNECTION*)param;

    // if pIntCon is NULL we do not know the file descriptor
    if (pIntCon != NULL)
    {
        // encoder and display buffers are in a different list
        if (pShot->publicShot.pStatistics)
        {
            SYS_IO_MemUnmap(pIntCon->fileDesc, pShot->publicShot.pStatistics,
                pShot->publicShot.statsSize);
            pShot->publicShot.pStatistics = NULL;
        }
        if (pShot->publicShot.pDPFMap)
        {
            SYS_IO_MemUnmap(pIntCon->fileDesc, pShot->publicShot.pDPFMap,
                pShot->publicShot.uiDPFSize);
            pShot->publicShot.pDPFMap = NULL;
        }
        if (pShot->publicShot.pENSOutput)
        {
            SYS_IO_MemUnmap(pIntCon->fileDesc, pShot->publicShot.pENSOutput,
                pShot->publicShot.uiENSSize);
            pShot->publicShot.pENSOutput = NULL;
        }
    }
    IMG_FREE(pShot);

    return IMG_TRUE;  // continue
}

/**
 * @ingroup INT_FCT
 * @brief Extract the information from the pipeline into a ioctl structure
 *
 * @param[in] pPipeline
 * @param[out] pPipeInfo
 */
static void IMG_CI_PipelineToInfo(const CI_PIPELINE *pPipeline,
	struct CI_PIPE_INFO *pPipeInfo)
{
    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pPipeInfo);

    IMG_MEMSET(pPipeInfo, 0, sizeof(struct CI_PIPE_INFO));

    IMG_MEMCPY(&(pPipeInfo->config), &(pPipeline->config),
        sizeof(CI_PIPELINE_CONFIG));

    IMG_MEMCPY(&(pPipeInfo->iifConfig), &(pPipeline->sImagerInterface),
        sizeof(CI_MODULE_IIF));
    IMG_MEMCPY(&(pPipeInfo->lshConfig), &(pPipeline->sDeshading),
        sizeof(CI_MODULE_LSH));
    IMG_MEMCPY(&(pPipeInfo->dpfConfig), &(pPipeline->sDefectivePixels),
        sizeof(CI_MODULE_DPF));
    IMG_MEMCPY(&(pPipeInfo->ensConfig), &(pPipeline->stats.sEncStats),
        sizeof(CI_MODULE_ENS));
    IMG_MEMCPY(&(pPipeInfo->sAWSCurveCoeffs),
        &(pPipeline->stats.sAutoWhiteBalanceStats.sCurveCoeffs),
        sizeof(CI_AWS_CURVE_COEFFS));
}

/**
 * @ingroup INT_FCT
 */
static IMG_RESULT IMG_PipelineTriggerShoot(INT_PIPELINE *pIntPipe,
    IMG_BOOL8 bBlocking, const CI_BUFFID *pBuffId)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct CI_BUFFER_TRIGG param;
    INT_BUFFER *pFoundEnc = NULL,
        *pFoundDisp = NULL,
        *pFoundHDRExt = NULL,
        *pFoundRaw2D = NULL,
        *pFoundHDRIns = NULL;
    CI_BUFFID search;
    sCell_T *found = NULL;

    IMG_MEMSET(&param, 0, sizeof(struct CI_BUFFER_TRIGG));
    param.captureId = pIntPipe->ui32Identifier;
    param.bBlocking = bBlocking;
    // param.capturePrivate = 0; // not used
    // param.bufferId = 0; // not used

    if (!pIntPipe->bStarted)
    {
        LOG_ERROR("pipeline capture is not started\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // cannot be started and not registered

    if (!pBuffId)
    {
        // search for 1st available buffers of each type bu HDRIns

        if (PXL_NONE != pIntPipe->publicPipeline.config.eHDRInsType.eFmt)
        {
            LOG_ERROR("Cannot use first available buffer when using "\
                "HDR Insertion - specific buffers should be specified!\n");
            ret = IMG_ERROR_NOT_SUPPORTED;
            goto trigger_clean;
        }

        ret = CI_PipelineFindFirstAvailable(&(pIntPipe->publicPipeline),
            &search);

        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to find first available buffers\n");
            goto trigger_clean;
        }
    }
    else
    {
        IMG_MEMCPY(&search, pBuffId, sizeof(CI_BUFFID));
    }

    if (search.encId > 0)
    {
        found = List_visitor(&(pIntPipe->sList_buffers),
            (void*)&(search.encId), &List_SearchBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find specified encoder output "\
                "buffer %#x!\n", search.encId);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
        pFoundEnc = container_of(found, INT_BUFFER, sCell);

        if (INT_BUFFER_AVAILABLE != pFoundEnc->eStatus)
        {
            LOG_ERROR("Encoder buffer %u found but it is not "\
                "available for capture\n", search.encId);
            pFoundEnc = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
    }

    if (search.dispId > 0)
    {
        found = List_visitor(&(pIntPipe->sList_buffers),
            (void*)&(search.dispId), &List_SearchBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find specified display output "\
                "buffer %#x!\n", search.dispId);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
        pFoundDisp = container_of(found, INT_BUFFER, sCell);

        if (INT_BUFFER_AVAILABLE != pFoundDisp->eStatus)
        {
            LOG_ERROR("Display buffer %u found but it is not "\
                "available for capture\n", search.dispId);
            pFoundDisp = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
    }

    if (search.idHDRExt > 0)
    {
        found = List_visitor(&(pIntPipe->sList_buffers),
            (void*)&(search.idHDRExt), &List_SearchBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find specified HDR extraction output "\
                "buffer %u!\n", search.idHDRExt);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
        pFoundHDRExt = container_of(found, INT_BUFFER, sCell);

        if (INT_BUFFER_AVAILABLE != pFoundHDRExt->eStatus)
        {
            LOG_ERROR("HDR Extraction buffer %u found but it is not "\
                "available for capture\n", search.idHDRExt);
            pFoundHDRExt = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
    }

    if (search.idHDRIns > 0)
    {
        found = List_visitor(&(pIntPipe->sList_buffers),
            (void*)&(search.idHDRIns), &List_SearchBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find specified HDRIns buffer %u!\n",
                search.idHDRIns);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
        pFoundHDRIns = container_of(found, INT_BUFFER, sCell);

        /* check for later because only HDRIns type use
            * INT_BUFFER::bHDRReserved */
        if (CI_ALLOC_HDRINS != pFoundHDRIns->type)
        {
            LOG_ERROR("Specified buffer %d for HDRIns is not an "\
                "HDRIns buffer\n", search.idHDRIns);
            pFoundHDRIns = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }

        if (INT_BUFFER_AVAILABLE != pFoundHDRIns->eStatus)
        {
            LOG_ERROR("HDRIns buffer %u found but it is not "\
                "available for capture\n", search.idHDRIns);
            pFoundHDRIns = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
    }

    if (search.idRaw2D > 0)
    {
        found = List_visitor(&(pIntPipe->sList_buffers),
            (void*)&(search.idRaw2D), &List_SearchBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find specified RAW2D extraction output "\
                "buffer %u!\n", search.idRaw2D);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
        pFoundRaw2D = container_of(found, INT_BUFFER, sCell);

        if (pFoundRaw2D->eStatus != INT_BUFFER_AVAILABLE)
        {
            LOG_ERROR("Raw2D buffer %u found but it is not "\
                "available for capture\n", search.idRaw2D);
            pFoundRaw2D = NULL;
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            goto trigger_clean;
        }
    }

    if (pFoundEnc)
    {
        param.bufferIds.encId = pFoundEnc->ID;
        if (pBuffId)
        {
            param.bufferIds.encStrideY = pBuffId->encStrideY;
            param.bufferIds.encStrideC = pBuffId->encStrideC;
            param.bufferIds.encOffsetY = pBuffId->encOffsetY;
            param.bufferIds.encOffsetC = pBuffId->encOffsetC;
        }
        pFoundEnc->eStatus = INT_BUFFER_PENDING;
    }
    if (pFoundDisp)
    {
        param.bufferIds.dispId = pFoundDisp->ID;
        if (pBuffId)
        {
            param.bufferIds.dispStride = pBuffId->dispStride;
            param.bufferIds.dispOffset = pBuffId->dispOffset;
        }
        pFoundDisp->eStatus = INT_BUFFER_PENDING;
    }
    if (pFoundHDRExt)
    {
        param.bufferIds.idHDRExt = pFoundHDRExt->ID;
        if (pBuffId)
        {
            param.bufferIds.HDRExtStride = pBuffId->HDRExtStride;
            param.bufferIds.HDRExtOffset = pBuffId->HDRExtOffset;
        }
        pFoundHDRExt->eStatus = INT_BUFFER_PENDING;
    }
    if (pFoundHDRIns)
    {
        param.bufferIds.idHDRIns = pFoundHDRIns->ID;
        if (pBuffId)
        {
            param.bufferIds.HDRInsStride = pBuffId->HDRInsStride;
            param.bufferIds.HDRInsOffset = pBuffId->HDRInsOffset;
        }
        pFoundHDRIns->eStatus = INT_BUFFER_PENDING;
        pFoundHDRIns->bHDRReserved = IMG_FALSE;
    }
    if (pFoundRaw2D)
    {
        param.bufferIds.idRaw2D = pFoundRaw2D->ID;
        if (pBuffId)
        {
            param.bufferIds.raw2DStride = pBuffId->raw2DStride;
            param.bufferIds.raw2DOffset = pBuffId->raw2DOffset;
        }
        pFoundRaw2D->eStatus = INT_BUFFER_PENDING;
    }

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_CAPT_TRG,
        (long)&param);
    if (ret < 0)
    {
        LOG_ERROR("Failed to add a frame to be captured\n");
        ret = toImgResult(ret);

        if (IMG_ERROR_UNEXPECTED_STATE == ret
            && !CI_PipelineIsStarted(&(pIntPipe->publicPipeline)))
        {
            LOG_INFO("Capture was stopped!\n");
        }
        goto trigger_clean;
    }
    return ret;

trigger_clean:
    if (pFoundEnc) pFoundEnc->eStatus = INT_BUFFER_AVAILABLE;
    if (pFoundDisp) pFoundDisp->eStatus = INT_BUFFER_AVAILABLE;
    if (pFoundHDRExt) pFoundHDRExt->eStatus = INT_BUFFER_AVAILABLE;
    if (pFoundHDRIns) pFoundHDRIns->eStatus = INT_BUFFER_AVAILABLE;
    if (pFoundRaw2D) pFoundRaw2D->eStatus = INT_BUFFER_AVAILABLE;

    return ret;
}

/**
 * @ingroup INT_FCT
 */
static IMG_RESULT IMG_PipelineAcquireBuffer(CI_PIPELINE *pPipeline,
    IMG_BOOL8 bBlocking, CI_SHOT **ppBuffer)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    INT_SHOT *pShot = NULL;

    if (!pPipeline || !ppBuffer)
    {
        LOG_ERROR("pPipeline or ppBuffer is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (0 == pIntPipe->ui32Identifier)
    {
        LOG_ERROR("pPipeline is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    else
    {
        // blocking version controlled by bBlocking
        /* captureId, bBlocking, capturePrivate (out), bufferId (out),
         * uiEncoder (out), uiDisplay (out) */
        struct CI_BUFFER_PARAM param;
        IMG_MEMSET(&param, 0, sizeof(struct CI_BUFFER_PARAM));
        param.captureId = pIntPipe->ui32Identifier;
        param.bBlocking = bBlocking;

#ifdef FELIX_FAKE
        /* when using fake interface the timeout in the kernel size is not
         * enough so we just continue trying */
        do
        {
            ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
                CI_IOCTL_CAPT_BAQ, (long)&param);
        } while (ret == -EINTR || (bBlocking && ret == -ETIME));
#else
        do
        {
            ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
                CI_IOCTL_CAPT_BAQ, (long)&param);
        } while (ret == -EINTR);
#endif
        // equivalent to IMG_ERROR_INTERRUPTED
        /* returned when the blocking command was interrupted but not
         * completed so it needs to be done again */

        if (ret < 0 || param.shotId <= 0)
        {
            if (bBlocking)
            {
                LOG_ERROR("Failed to acquire a buffer (returned %d "\
                        "- shot id %d)\n", ret, param.shotId);
            }
            return toImgResult(ret);
        }
        else
        {
            INT_BUFFER *pBuff = NULL;
            sCell_T *pFound = NULL;
            int reg;

            pFound = List_visitor(&(pIntPipe->sList_shots), &(param.shotId),
                &List_SearchShot);
            if (!pFound)
            {
                LOG_ERROR("Failed to find the acquired buffer in the user "\
                    "side list! param.shotId=%d\n", param.shotId);
                goto acquire_failed;
            }
            pShot = container_of(pFound, INT_SHOT, sCell);
            /* if already acquired then there is a problem of communication
             * with kernel side */
            IMG_ASSERT(pShot->bAcquired == IMG_FALSE);
            pShot->bAcquired = IMG_TRUE;
            // copy the values from the kernel-side
            // done from stats
            // pShot->publicShot.ui8PrivateValue = param.shot.ui8PrivateValue;
            // done from stats
            // pShot->publicShot.bFrameError = param.shot.bFrameError;

            pShot->publicShot.ui32LinkedListTS = param.shot.ui32LinkedListTS;
            pShot->publicShot.ui32InterruptTS = param.shot.ui32InterruptTS;
#ifdef INFOTM_ISP
            pShot->publicShot.ui64SystemTS = param.shot.ui64SystemTS;
#endif

            *ppBuffer = &(pShot->publicShot);

            // search for buffers - 0 means buffer not found
            if (param.shot.encId)
            {
                pFound = List_visitor(&(pIntPipe->sList_buffers),
                    &(param.shot.encId), &List_SearchBuffer);
                if (pFound == NULL)
                {
                    LOG_ERROR("Failed to find the acquired the specified "\
                        "enc buffer (id=%d) in the user side list!\n",
                        param.shot.encId);
                    goto acquire_failed;
                }
                pBuff = container_of(pFound, INT_BUFFER, sCell);
                pShot->pEncOutput = pBuff;
                pBuff->eStatus = INT_BUFFER_ACQUIRED;

                pShot->publicShot.encId = param.shot.encId;
                pShot->publicShot.pEncoderOutput = pShot->pEncOutput->memory;
#ifdef INFOTM_ISP
                pShot->publicShot.pEncoderOutputPhyAddr = param.shot.pEncoderOutputPhyAddr;
#endif //INFOTM_ISP			
                pShot->publicShot.aEncYSize[0] = param.shot.aEncYSize[0];
                pShot->publicShot.aEncYSize[1] = param.shot.aEncYSize[1];
                pShot->publicShot.aEncCbCrSize[0] = param.shot.aEncCbCrSize[0];
                pShot->publicShot.aEncCbCrSize[1] = param.shot.aEncCbCrSize[1];
                pShot->publicShot.aEncOffset[0] = param.shot.aEncOffset[0];
                pShot->publicShot.aEncOffset[1] = param.shot.aEncOffset[1];
                pShot->publicShot.bEncTiled = param.shot.bEncTiled;
            }
            else
            {
                pShot->publicShot.pEncoderOutput = NULL;
                pShot->publicShot.encId = 0;
            }

            if (param.shot.dispId)
            {
                pFound = List_visitor(&(pIntPipe->sList_buffers),
                    &(param.shot.dispId), &List_SearchBuffer);
                if (!pFound)
                {
                    LOG_ERROR("Failed to find the acquired the specified "\
                        "disp buffer (id=%d) in the user side list!\n",
                        param.shot.dispId);
                    goto acquire_failed;
                }
                pBuff = container_of(pFound, INT_BUFFER, sCell);
                pShot->pDispOutput = pBuff;
                pBuff->eStatus = INT_BUFFER_ACQUIRED;

                pShot->publicShot.dispId = param.shot.dispId;
                pShot->publicShot.pDisplayOutput = pShot->pDispOutput->memory;
#ifdef INFOTM_ISP			
                pShot->publicShot.pDisplayOutputPhyAddr = param.shot.pDisplayOutputPhyAddr;
#endif //INFOTM_ISP			
                pShot->publicShot.aDispSize[0] = param.shot.aDispSize[0];
                pShot->publicShot.aDispSize[1] = param.shot.aDispSize[1];
                pShot->publicShot.ui32DispOffset = param.shot.ui32DispOffset;
                pShot->publicShot.bDispTiled = param.shot.bDispTiled;
            }
            else
            {
                pShot->publicShot.pDisplayOutput = NULL;
            }

            if (param.shot.HDRExtId)
            {
                pFound = List_visitor(&(pIntPipe->sList_buffers),
                    &(param.shot.HDRExtId), &List_SearchBuffer);
                if (!pFound)
                {
                    LOG_ERROR("Failed to find the acquired the specified "\
                        "HDRExt buffer (id=%d) in the user side list!\n",
                        param.shot.HDRExtId);
                    goto acquire_failed;
                }
                pBuff = container_of(pFound, INT_BUFFER, sCell);
                pShot->pHDRExtOutput = pBuff;
                pBuff->eStatus = INT_BUFFER_ACQUIRED;

                pShot->publicShot.HDRExtId = param.shot.HDRExtId;
                pShot->publicShot.pHDRExtOutput = pShot->pHDRExtOutput->memory;
#ifdef INFOTM_ISP				
                pShot->publicShot.pHDRExtOutputPhyAddr = param.shot.pHDRExtOutputPhyAddr;
#endif //INFOTM_ISP				
                pShot->publicShot.aHDRExtSize[0] = param.shot.aHDRExtSize[0];
                pShot->publicShot.aHDRExtSize[1] = param.shot.aHDRExtSize[1];
                pShot->publicShot.ui32HDRExtOffset =
                    param.shot.ui32HDRExtOffset;
                pShot->publicShot.bHDRExtTiled = param.shot.bHDRExtTiled;
            }
            else
            {
                pShot->publicShot.pHDRExtOutput = NULL;
            }

            // HDR Insertion is not part of the shot

            if (param.shot.raw2DExtId)
            {
                pFound = List_visitor(&(pIntPipe->sList_buffers),
                    &(param.shot.raw2DExtId), &List_SearchBuffer);
                if (pFound == NULL)
                {
                    LOG_ERROR("Failed to find the acquired the specified "\
                        "Raw2dExt buffer (id=%d) in the user side list!\n",
                        param.shot.HDRExtId);
                    goto acquire_failed;
                }
                pBuff = container_of(pFound, INT_BUFFER, sCell);
                pShot->pRaw2DExtOutput = pBuff;
                pBuff->eStatus = INT_BUFFER_ACQUIRED;

                pShot->publicShot.raw2DExtId = param.shot.raw2DExtId;
                pShot->publicShot.pRaw2DExtOutput =
                    pShot->pRaw2DExtOutput->memory;
#ifdef INFOTM_ISP			
                pShot->publicShot.pRaw2DExtOutputPhyAddr = param.shot.pRaw2DExtOutputPhyAddr;
#endif //INFOTM_ISP	
                pShot->publicShot.aRaw2DSize[0] = param.shot.aRaw2DSize[0];
                pShot->publicShot.aRaw2DSize[1] = param.shot.aRaw2DSize[1];
                pShot->publicShot.ui32Raw2DOffset =
                    param.shot.ui32Raw2DOffset;
            }
            else
            {
                pShot->publicShot.pRaw2DExtOutput = NULL;
                pShot->publicShot.raw2DExtId = 0;
            }

            // offsets are in bytes - registers values are 32b
            reg = ((IMG_INT32*)pShot->publicShot.pStatistics)
                [(FELIX_SAVE_CONTEXT_LOGS_OFFSET/4)];
            pShot->publicShot.ui8PrivateValue =
                (reg&FELIX_SAVE_CONTEXT_LOGS_SAVE_CONFIG_TAG_MASK)
                    >> FELIX_SAVE_CONTEXT_LOGS_SAVE_CONFIG_TAG_SHIFT;
            pShot->publicShot.bFrameError = IMG_FALSE;
            if ((reg&FELIX_SAVE_CONTEXT_LOGS_FRAME_ERROR_MASK))
            {
                pShot->publicShot.bFrameError = IMG_TRUE;
            }
            pShot->publicShot.i32MissedFrames = param.shot.i32MissedFrames;

            pShot->publicShot.eOutputConfig = param.shot.eOutputConfig;
        }
    }

    return IMG_SUCCESS;

acquire_failed:
    /* unlikely to happen (need failure to find kernel given ID in
     * user-side list) */
    /// @ release the buffer on kernel side?
    if (pShot)
    {
        if (pShot->pEncOutput)
        {
            pShot->pEncOutput->eStatus = INT_BUFFER_PENDING;
        }
        if (pShot->pDispOutput)
        {
            pShot->pDispOutput->eStatus = INT_BUFFER_PENDING;
        }
        if (pShot->pHDRExtOutput)
        {
            pShot->pHDRExtOutput->eStatus = INT_BUFFER_PENDING;
        }
        // HDR insertion is not part of the shot
        if (pShot->pRaw2DExtOutput)
        {
            pShot->pRaw2DExtOutput->eStatus = INT_BUFFER_PENDING;
        }
    }
    return IMG_ERROR_FATAL;
}

/**
 * @ingroup INT_FCT
 */
static IMG_RESULT IMG_CI_PipelineUpdate(CI_PIPELINE *pPipeline,
    int eUpdateMask, int *peFailure, IMG_BOOL8 bASAP)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;

    if (!pPipeline || CI_UPD_NONE == eUpdateMask)
    {
        LOG_ERROR("pPipeline or pParam is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (peFailure)
    {
        *peFailure = CI_UPD_NONE;
    }

    ret = IMG_SUCCESS;
    if (pIntPipe->ui32Identifier > 0)
    {
        struct CI_PIPE_PARAM param;
        param.identifier = pIntPipe->ui32Identifier;
        param.eUpdateMask = eUpdateMask;
        param.bUpdateASAP = bASAP;
        param.loadstructure = pIntPipe->pLoadStructure;

        // verify that the capture is correct before updating it
        ret = CI_PipelineVerify(pPipeline);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to verify the capture\n");
            return ret;
        }

        IMG_CI_PipelineToInfo(pPipeline, &(param.info));

        // can choose here which function to call, for the moment no choice
        // could also use a pointer to function
#if FELIX_VERSION_MAJ == 2
        ret = INT_CI_Load_HW2(pIntPipe, pIntPipe->pLoadStructure);
#else
#error "unsupported HW architecture"
#endif
        if (ret)
        {
            LOG_ERROR("failed to write the load structure!\n");
            return ret;
        }

        ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_PIPE_UPD, (long)&param);
        if (ret < 0)
        {
            LOG_ERROR("Failed to update the capture (ret %d)\n", ret);
            ret = toImgResult(ret);
            if (peFailure)
            {
                *peFailure = param.eUpdateMask;
            }
        }
    }
    else
    {
        LOG_ERROR("The capture is not registered\n");
        ret = IMG_ERROR_NOT_INITIALISED;
    }

    return ret;
}

/** @brief converts from @ref CI_BUFFTYPE to @ref CI_ALLOC_BUFF */
static IMG_RESULT IMG_CI_ConvertType(enum CI_BUFFTYPE eBuffer,
    enum CI_ALLOC_BUFF *type)
{
    switch (eBuffer)
    {
    case CI_TYPE_DISPLAY:
    case CI_TYPE_DATAEXT:
        *type = CI_ALLOC_DISP;
        break;

    case CI_TYPE_ENCODER:
        *type = CI_ALLOC_ENC;
        break;

    case CI_TYPE_HDREXT:
        *type = CI_ALLOC_HDREXT;
        break;

    case CI_TYPE_HDRINS:
        *type = CI_ALLOC_HDRINS;
        break;

    case CI_TYPE_RAW2D:
        *type = CI_ALLOC_RAW2D;
        break;

    default:
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

/**
 * @brief Creates a buffer object in kernel and user space
 *
 * @param pPipeline
 * @param eBuffer
 * @param ui32Size in bytes - 0 means it will be computed in kernel side
 * @param bTiled
 * @param ionFd if != 0 uses import mechanism, otherwise uses allocation
 * mechanism
 * @param pBufferId optional output
 */
static IMG_RESULT IMG_CI_PipelineCreateBuffer(CI_PIPELINE *pPipeline,
    enum CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size, IMG_BOOL8 bTiled,
    IMG_UINT32 ionFd, IMG_UINT32 *pBufferId)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    struct CI_ALLOC_PARAM param;
    INT_BUFFER *pBuff = NULL;
    int page_prot = PROT_READ;  // most buffers are read only in user-space
    // but HDR insertion buffers have to be write
    enum CI_ALLOC_BUFF buffType = CI_ALLOC_ENC;

    if (!pPipeline || CI_TYPE_NONE == eBuffer)
    {
        LOG_ERROR("pPipeline is NULL or buffer type is NONE\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    LOG_DEBUG("CI_BUFFTYE=%d tiled=%d ionFd = 0x%x\n", (int)eBuffer,
        (int)bTiled, ionFd);

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (0 == pIntPipe->ui32Identifier)
    {
        LOG_ERROR("Pipeline is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (bTiled && !pIntPipe->bSupportTiling)  // also checked in kernel-side
    {
        LOG_ERROR("Pipeline is not configured to support tiling, use "\
            "bSupportTiling before registration to allow "\
            "impotation/allocation of tiled buffers\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (CI_TYPE_NONE == eBuffer
        || (CI_TYPE_ENCODER == eBuffer
        && TYPE_YUV != pPipeline->config.eEncType.eBuffer)
        || (CI_TYPE_DISPLAY == eBuffer
        && (TYPE_RGB != pPipeline->config.eDispType.eBuffer
            // YUV444 packed
         && TYPE_YUV != pPipeline->config.eDispType.eBuffer))
        || (CI_TYPE_DATAEXT == eBuffer
        && TYPE_BAYER != pPipeline->config.eDispType.eBuffer)
        || (CI_TYPE_HDREXT == eBuffer
        && TYPE_RGB != pPipeline->config.eHDRExtType.eBuffer)
        || (CI_TYPE_HDRINS == eBuffer
        && TYPE_RGB != pPipeline->config.eHDRInsType.eBuffer)
        || (CI_TYPE_RAW2D == eBuffer
        && TYPE_BAYER != pPipeline->config.eRaw2DExtraction.eBuffer)
        )
    {
        LOG_ERROR("Given buffer type %d is not configured in current "\
            "Pipeline setup (encoder=%d, display=%d, hdrExt=%d, hdrIns=%d, "\
            "raw2D=%d)\n",
            (int)eBuffer,
            (int)pPipeline->config.eEncType.eBuffer,
            (int)pPipeline->config.eDispType.eBuffer,
            (int)pPipeline->config.eHDRExtType.eBuffer,
            (int)pPipeline->config.eHDRInsType.eBuffer,
            (int)pPipeline->config.eRaw2DExtraction.eBuffer);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    IMG_MEMSET(&param, 0, sizeof(struct CI_ALLOC_PARAM));
    param.configId = pIntPipe->ui32Identifier;
    param.fd = ionFd;
    param.uiSize = ui32Size;  // if 0 kernel side will compute the size
    param.bTiled = bTiled;

    if (CI_TYPE_HDRINS == eBuffer)
    {
        page_prot = PROT_WRITE;  // this buffer needs writing
    }
    if (IMG_CI_ConvertType(eBuffer, &buffType))
    {
        LOG_ERROR("unsupoprted buffer format configured!\n");
        return IMG_ERROR_FATAL;
    }
    param.type = buffType;
    // param.uiMemMapId as output
    // param.uiSize as output if uiSize == 0

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_CREATE_BUFF, (long)&param);
    if (ret)
    {
        LOG_ERROR("Failed to create buffers in kernel space "\
            "(returned %d)\n", ret);
        return toImgResult(ret);
    }

    pBuff = (INT_BUFFER *)IMG_CALLOC(1, sizeof(INT_BUFFER));
    if (!pBuff)
    {
        LOG_ERROR("Failed to allocate new internal Buffer\n");
        /// @ how to notify kernel-space?
        return IMG_ERROR_MALLOC_FAILED;
    }

    // so that both container_of and the internal pointer work
    pBuff->sCell.object = pBuff;
    pBuff->ID = param.uiMemMapId;
    pBuff->uiSize = param.uiSize;
    pBuff->type = buffType;
    pBuff->eStatus = INT_BUFFER_AVAILABLE;
    pBuff->ionFd = ionFd;
    pBuff->bIsTiled = bTiled;
    pBuff->bHDRReserved = IMG_FALSE;

    pBuff->memory = SYS_IO_MemMap2(pIntPipe->pConnection->fileDesc,
        pBuff->uiSize, page_prot, MAP_SHARED, pBuff->ID);

    if (!pBuff->memory)
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("Failed to map Buffer to user-space\n");
        IMG_FREE(pBuff);
        /// @ how to notify kernel-space?
    }
    else
    {
        ret = List_pushBack(&(pIntPipe->sList_buffers), &(pBuff->sCell));
        if (IMG_SUCCESS != ret)
        {
            // unlikely!!! could be an assert
            LOG_ERROR("Failed to add buffer to the Pipeline list!\n");
            SYS_IO_MemUnmap(pIntPipe->pConnection->fileDesc, pBuff->memory,
                pBuff->uiSize);
            IMG_FREE(pBuff);
        }
        LOG_DEBUG("CI map in buffer type CI_ALLOC_BUFF=%d of size 0x%lx\n",
            param.type, pBuff->uiSize);
    }

    if (IMG_SUCCESS == ret && pBufferId)
    {
        *pBufferId = pBuff->ID;
    }

    return ret;
}

/** structure used when computing the linestore */
struct _sTaken_
{
    /** start position in the linestore */
    IMG_UINT32 start;
    /** size in the linestore */
    IMG_UINT32 size;
    /** helpful for debug purposses */
    IMG_UINT32 ctx;
};

/** sort the structure used when computing the linestore */
static int cmpTaken(const void *p1, const void *p2)
{
    const struct _sTaken_ *taken1 = (const struct _sTaken_*)p1;
    const struct _sTaken_ *taken2 = (const struct _sTaken_*)p2;

    // start1 < start2  -> return negative = p1 goes before p2
    // start2 < start1 -> return positive = p1 goes after p2
    return (taken1->start - taken2->start);
}

/*-----------------------------------------------------------------------------
 * Part of the CI library
 *---------------------------------------------------------------------------*/

IMG_RESULT CI_PipelineCreate(CI_PIPELINE **ppPipeline,
    CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    INT_PIPELINE *pIntPipe = NULL;
    INT_CONNECTION *pIntCon = NULL;
    IMG_UINT32 loadStructSize =
        INT_CI_SizeLS_HW2();  // no other choices yet

    if (!ppPipeline || !pConnection)
    {
        LOG_ERROR("ppPipeline or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppPipeline)
    {
        LOG_ERROR("pipeline alread allocated\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    // all fields are 0
    pIntPipe = (INT_PIPELINE*)IMG_CALLOC(1, sizeof(INT_PIPELINE));
    if (pIntPipe == NULL)
    {
        LOG_ERROR("failed to allocate internal configuration object "\
            "(%lu B)\n", (unsigned long)sizeof(INT_PIPELINE));
        return IMG_ERROR_MALLOC_FAILED;
    }

    //pIntPipe->pLoadStructure = NULL;
    pIntPipe->pLoadStructure = IMG_CALLOC(1, loadStructSize);
    if (!pIntPipe->pLoadStructure)
    {
        LOG_ERROR("failed to allocate internal load structure object "\
            "(%u B)\n", loadStructSize);
        IMG_FREE(pIntPipe);
        return IMG_ERROR_MALLOC_FAILED;
    }

    /* so that container_of and the internal pointer work
     pIntPipe->sCell.object = pIntPipe; */
    // pIntPipe->publicPipeline.eOutputConfig = CI_SAVE_ALL;
    pIntPipe->publicPipeline.config.eOutputConfig = CI_SAVE_NO_STATS;
    pIntPipe->publicPipeline.config.eDataExtraction = CI_INOUT_NONE;

    pIntPipe->publicPipeline.config.ui8Context =
        pConnection->sHWInfo.config_ui8NContexts;  // too big to be valid

    pIntPipe->publicPipeline.config.ui16SysBlackLevel =
        16 << SYS_BLACK_LEVEL_FRAC;

    ret = List_init(&(pIntPipe->sList_shots));
    if (ret)
    {
        // unlikely
        LOG_ERROR("failed to create the shot list\n");
        goto config_init_failure;
    }
    ret = List_init(&(pIntPipe->sList_buffers));
    if (ret)
    {
        // unlikely
        LOG_ERROR("failed to create the buffer list\n");
        goto config_init_failure;
    }
    ret = List_init(&(pIntPipe->sList_lshmat));
    if (ret)
    {
        // unlikely
        LOG_ERROR("failed to create the LSH matrix list\n");
        goto config_init_failure;
    }

    /*
     * init all modules
     */

    // IIF is all 0
    pIntPipe->publicPipeline.sImagerInterface.ui8Imager =
        pConnection->sHWInfo.config_ui8NImagers; // too big to be valid

    // BLC is all 0
#ifndef BLC_NOT_AVAILABLE
    pIntPipe->publicPipeline.sBlackCorrection.ui16PixelMax =
        (1 << pConnection->sHWInfo.config_ui8BitDepth) - 1;
#endif

    // RLT is all 0
    // LSH is all 0
    // WBC is all 0
    // DNS is all 0 but the following
    pIntPipe->publicPipeline.sDenoiser.bCombineChannels = IMG_TRUE;

    // DPF is all 0
    pIntPipe->publicPipeline.sDefectivePixels.sInput.apDefectInput = NULL;
    // LCA is all 0

    // add DMO

    // CCM is all 0
    // MGM is all 0

    // GMA is all 0
    // R2Y is all 0 but the type
    pIntPipe->publicPipeline.sRGBToYCC.eType = RGB_TO_YCC;

    // MIE is all 0
    // TNM is all 0

    // SHA is all 0 but denoise bypass
    pIntPipe->publicPipeline.sSharpening.bDenoiseBypass = IMG_TRUE;

    // ESC is all 0 but the following options
    pIntPipe->publicPipeline.sEncoderScaler.bIsDisplay = IMG_FALSE;
    pIntPipe->publicPipeline.sEncoderScaler.bBypassScaler = IMG_TRUE;

    // add VSS

    // DSC is all 0 but the following options
    pIntPipe->publicPipeline.sDisplayScaler.bIsDisplay = IMG_TRUE;
    pIntPipe->publicPipeline.sDisplayScaler.bBypassScaler = IMG_TRUE;

    // add HUS

    // Y2R is all 0 but the type
    pIntPipe->publicPipeline.sYCCToRGB.eType = YCC_TO_RGB;

    // DGM is all 0

    // EXS is all 0
    // FOS is all 0
    // WBS is all 0
    // HIS is all 0
    // AWS is all 0
    // FLD is all 0
    // ENS is all 0

    /*
    * when all is well we add the pipeline to the list of available ones
    */

    pIntPipe->sCell.object = pIntPipe;
    ret = List_pushBack((&pIntCon->sList_pipelines), &(pIntPipe->sCell));
    /* the insertion cannot fail, the cell is allocated and not attached to
     * a list yet */
    IMG_ASSERT(ret == IMG_SUCCESS);

    pIntPipe->pConnection = pIntCon;

    *ppPipeline = &(pIntPipe->publicPipeline);

    return IMG_SUCCESS;

config_init_failure:
    CI_PipelineDestroy(&(pIntPipe->publicPipeline));
    *ppPipeline = NULL;
    return ret;
}

IMG_RESULT CI_PipelineRegister(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    int returnedID = 0;
    struct CI_PIPE_INFO param;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier != 0)
    {
        LOG_ERROR("already registered!\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    ret = CI_PipelineVerify(pPipeline);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Configuration is not correct!\n");
        return ret;
    }

    IMG_CI_PipelineToInfo(pPipeline, &param);

    returnedID = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_PIPE_REG, (long int)&(param));
    // <= 0 because negative is error and 0 is invalid ID
    if (returnedID <= 0)
    {
        LOG_ERROR("Failed to register the configuration "\
            "(returned %d - 0 is invalid ID)\n", returnedID);
        return toImgResult(returnedID);
    }
    pIntPipe->ui32Identifier = returnedID;
    pIntPipe->bSupportTiling = pPipeline->config.bSupportTiling;

    // do the 1st update
    ret = CI_PipelineUpdate(pPipeline, CI_UPD_ALL, NULL);
    if (ret)
    {
        LOG_ERROR("Failed to do the 1st update!\n");
        //return ret;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineStartCapture(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    if (0 == pIntPipe->ui32Identifier)
    {
        LOG_ERROR("the pipeline is not registered!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    /* it is disabled because it is legitimate to configure the pipeline just
     * to get statistics outputs */
    // if (pIntPipe->sList_buffers.ui32Elements == 0)
    // {
        // LOG_ERROR("No buffer found in the list - allocate or import "\
        //    "buffers before starting if output buffers are needed\n");
        // return IMG_ERROR_NOT_INITIALISED;
    // }

    /* valgrind may complain that ui32Identifier is unaddressable memory
     * but we don't use it as an address so it is safe */
    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_CAPT_STA,
        (long)pIntPipe->ui32Identifier);
    if (ret < 0)
    {
        LOG_ERROR("Failed to start the capture\n");
        return toImgResult(ret);
    }
    pIntPipe->bStarted = IMG_TRUE;

    return ret;
}

IMG_BOOL8 CI_PipelineIsStarted(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    int status;
    IMG_BOOL prev = IMG_FALSE;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);
    prev = pIntPipe->bStarted;

    status = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_CAPT_ISS, (long)pIntPipe->ui32Identifier);
    if (status < 0)
    {
        LOG_ERROR("Failed to verify if the capture was started\n");
        return toImgResult(status);
    }
    pIntPipe->bStarted = status ? IMG_TRUE : IMG_FALSE;

    // if pipeline was stopped we clear all pending to available
    if (pIntPipe->bStarted != prev && prev == IMG_TRUE)
    {
        int changeStatus[2] = {
            (int)INT_BUFFER_PENDING,
            (int)INT_BUFFER_AVAILABLE
        };
        List_visitor(&(pIntPipe->sList_buffers), &(changeStatus[0]),
            &List_ChangeBufferStatus);
    }

    return pIntPipe->bStarted;
}

IMG_RESULT CI_PipelineStopCapture(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->bStarted == IMG_FALSE)
    {
        LOG_ERROR("pipeline capture is not started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    // cannot be started and not registered

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_CAPT_STP,
        (long)pIntPipe->ui32Identifier);
    if (ret < 0)
    {
        LOG_ERROR("Failed to stop the capture\n");
        return toImgResult(ret);
    }

    // all buffers that were pending become available again
    {
        int status[2] = {
            (int)INT_BUFFER_PENDING,
            (int)INT_BUFFER_AVAILABLE
        };
        List_visitor(&(pIntPipe->sList_buffers), &(status[0]),
            &List_ChangeBufferStatus);
    }

    pIntPipe->bStarted = IMG_FALSE;

    return ret;
}

IMG_RESULT CI_PipelineTriggerShoot(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    ret = IMG_PipelineTriggerShoot(pIntPipe, IMG_TRUE, NULL);
    return ret;
}

IMG_RESULT CI_PipelineTriggerShootNB(CI_PIPELINE *pPipeline)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    ret = IMG_PipelineTriggerShoot(pIntPipe, IMG_FALSE,  NULL);
    return ret;
}

IMG_RESULT CI_PipelineTriggerSpecifiedShoot(CI_PIPELINE *pPipeline,
    const CI_BUFFID *pBuffId)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret;

    if (!pPipeline || !pBuffId)
    {
        LOG_ERROR("pPipeline or pBuffId is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    ret = IMG_PipelineTriggerShoot(pIntPipe, IMG_TRUE, pBuffId);
    return ret;
}

IMG_RESULT CI_PipelineTriggerSpecifiedShootNB(CI_PIPELINE *pPipeline,
    CI_BUFFID *pBuffId)
{
    INT_PIPELINE* pIntPipe = NULL;
    IMG_RESULT ret;

    if (!pPipeline || !pBuffId)
    {
        LOG_ERROR("pPipeline or pBuffId is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    ret = IMG_PipelineTriggerShoot(pIntPipe, IMG_FALSE, pBuffId);
    return ret;
}

IMG_RESULT CI_PipelineAcquireShot(CI_PIPELINE *pPipeline, CI_SHOT **ppBuffer)
{
    // blocking call
    return IMG_PipelineAcquireBuffer(pPipeline, IMG_TRUE, ppBuffer);
}

IMG_RESULT CI_PipelineAcquireShotNB(CI_PIPELINE *pPipeline,
    CI_SHOT **ppBuffer)
{
    // non blocking call
    return IMG_PipelineAcquireBuffer(pPipeline, IMG_FALSE, ppBuffer);
}

IMG_RESULT CI_PipelineExtractConfig(const CI_SHOT *pShot,
    CI_PIPELINE_STATS *pConfig)
{
    INT_SHOT *pIntShot = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pShot || !pConfig)
    {
        LOG_ERROR("pShot or pConfig is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntShot = container_of(pShot, INT_SHOT, publicShot);
    IMG_ASSERT(pIntShot);

    if (pIntShot->bAcquired == IMG_FALSE)
    {
        LOG_ERROR("pShot is not acquired!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(pConfig, 0, sizeof(CI_PIPELINE_STATS));

    // choose according to HW version here
#if FELIX_VERSION_MAJ == 2
    INT_CI_Revert_HW2(pShot, pConfig);
#else
#error unsupported HW architecture
#endif

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineReleaseShot(CI_PIPELINE *pPipeline, CI_SHOT *pBuffer)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    INT_SHOT *pShot = NULL;

    if (!pPipeline || !pBuffer)
    {
        LOG_ERROR("pPipeline or pBuffer is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    pShot = container_of(pBuffer, INT_SHOT, publicShot);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);
    IMG_ASSERT(pShot);

    if (pIntPipe->ui32Identifier == 0 || pShot->bAcquired == IMG_FALSE)
    {
        LOG_ERROR("pPipeline is not registered or buffer was not acquired\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    else
    {
        // releasing does not care about blocking or not
        /* captureId, bBlocking (not used), capturePrivate (not used),
         * bufferId (out), uiEncoder (out), uiDisplay (out)*/
        struct CI_BUFFER_TRIGG param;
        IMG_MEMSET(&param, 0, sizeof(struct CI_BUFFER_TRIGG));
        param.captureId = pIntPipe->ui32Identifier;
        param.bBlocking = IMG_FALSE;
        param.shotId = pShot->iIdentifier;

        ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_CAPT_BRE, (long)&param);
        if (ret < 0)
        {
            LOG_ERROR("Failed to release a buffer (returned %d)\n", ret);
            return toImgResult(ret);
        }

        pShot->bAcquired = IMG_FALSE;

        if (pShot->pEncOutput)
        {
            IMG_ASSERT(pShot->pEncOutput->sCell.object != NULL);

            pShot->pEncOutput->eStatus = INT_BUFFER_AVAILABLE;
            List_pushBack(&(pIntPipe->sList_buffers),
                &(pShot->pEncOutput->sCell));

            pShot->pEncOutput = NULL;
            pShot->publicShot.pEncoderOutput = NULL;
        }
        if (pShot->pDispOutput)
        {
            IMG_ASSERT(pShot->pDispOutput->sCell.object != NULL);

            pShot->pDispOutput->eStatus = INT_BUFFER_AVAILABLE;
            List_pushBack(&(pIntPipe->sList_buffers),
                &(pShot->pDispOutput->sCell));

            pShot->pDispOutput = NULL;
            pShot->publicShot.pDisplayOutput = NULL;
        }
        if (pShot->pHDRExtOutput)
        {
            IMG_ASSERT(pShot->pHDRExtOutput->sCell.object != NULL);

            pShot->pHDRExtOutput->eStatus = INT_BUFFER_AVAILABLE;
            List_pushBack(&(pIntPipe->sList_buffers),
                &(pShot->pHDRExtOutput->sCell));

            pShot->pHDRExtOutput = NULL;
            pShot->publicShot.pHDRExtOutput = NULL;
        }
        if (pShot->pRaw2DExtOutput)
        {
            IMG_ASSERT(pShot->pRaw2DExtOutput->sCell.object != NULL);

            pShot->pRaw2DExtOutput->eStatus = INT_BUFFER_AVAILABLE;
            List_pushBack(&(pIntPipe->sList_buffers),
                &(pShot->pRaw2DExtOutput->sCell));

            pShot->pRaw2DExtOutput = NULL;
            pShot->publicShot.pRaw2DExtOutput = NULL;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineAcquireHDRBuffer(CI_PIPELINE *pPipeline,
    CI_BUFFER *pFrame, IMG_UINT32 id)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;
    INT_BUFFER *pBuffer = NULL;

    if (!pPipeline || !pFrame)
    {
        LOG_ERROR("pPipeline or pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    if (id != 0)
    {
        // search a specific buffer
        pFound = List_visitor(&(pIntPipe->sList_buffers), &id,
            &List_SearchBuffer);
    }
    else
    {
        // search first available buffer
        pFound = List_visitor(&(pIntPipe->sList_buffers), NULL,
            &List_FindAvailableHDRBuffer);
    }

    if (pFound)
    {
        pBuffer = container_of(pFound, INT_BUFFER, sCell);
        if (CI_ALLOC_HDRINS != pBuffer->type)
        {
            LOG_ERROR("Buffer %d is not of HDR insertion type!\n", id);
            pBuffer = NULL;
        }
        if (pBuffer->bHDRReserved || INT_BUFFER_AVAILABLE != pBuffer->eStatus)
        {
            LOG_ERROR("HDRIns Buffer %d is already reserved or not "\
                "available!\n", id);
            pBuffer = NULL;
        }
    }
    else
    {
        LOG_ERROR("Could not find Buffer (id=%d)\n", id);
    }

    if (!pBuffer)
    {
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    pBuffer->bHDRReserved = IMG_TRUE;
    pFrame->id = pBuffer->ID;
    pFrame->data = pBuffer->memory;
    pFrame->ui32Size = pBuffer->uiSize;

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineReleaseHDRBuffer(CI_PIPELINE *pPipeline, IMG_UINT32 id)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;
    INT_BUFFER *pBuffer = NULL;

    if (!pPipeline || id == 0)
    {
        LOG_ERROR("pPipeline is NULL or id is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    // search the specified buffer
    pFound = List_visitor(&(pIntPipe->sList_buffers), &id,
        &List_SearchBuffer);

    if (pFound)
    {
        pBuffer = container_of(pFound, INT_BUFFER, sCell);
        if (CI_ALLOC_HDRINS != pBuffer->type)
        {
            LOG_ERROR("Buffer %d is not of HDR insertion type!\n", id);
            pBuffer = NULL;
        }
        if (pBuffer->bHDRReserved == IMG_FALSE
            || INT_BUFFER_AVAILABLE != pBuffer->eStatus)
        {
            LOG_ERROR("HDRIns Buffer %d is not reserved or not available!\n",
                id);
            pBuffer = NULL;
        }
    }
    else
    {
        LOG_ERROR("Could not find Buffer (id=%d)\n", id);
    }

    if (!pBuffer)
    {
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    pBuffer->bHDRReserved = IMG_FALSE;

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineAcquireLSHMatrix(CI_PIPELINE *pPipeline,
    IMG_UINT32 id, CI_LSHMAT *pMatrix)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;
    INT_LSHMAT *pBuffer = NULL;

    if (!pPipeline || !pMatrix || 0 == id)
    {
        LOG_ERROR("pPipeline or pMatrix is NULL or id is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    // search the specified buffer
    pFound = List_visitor(&(pIntPipe->sList_lshmat), &id,
        &List_SearchLSHMatrix);

    if (pFound)
    {
        pBuffer = container_of(pFound, INT_LSHMAT, sCell);

        if (INT_BUFFER_AVAILABLE != pBuffer->eStatus)
        {
            LOG_ERROR("LSH matrix %d not available\n", id);
            pBuffer = NULL;
        }
        else
        {
            struct CI_LSH_GET_PARAM sParam;
            IMG_MEMSET(&sParam, 0, sizeof(struct CI_LSH_GET_PARAM));
            sParam.configId = pIntPipe->ui32Identifier;
            sParam.matrixId = pBuffer->ID;

            ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
                CI_IOCTL_GET_LSH, (long)&sParam);
            if (ret)
            {
                ret = toImgResult(ret);
                if (IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE == ret)
                {
                    LOG_ERROR("LSH matrix %d is not available in kernel\n",
                        id);
                    // change buffer status as it's not available?
                    // pBuffer->eStatus = INT_BUFFER_PENDING;
                }
                else
                {
                    LOG_ERROR("LSH matrix %d not found in kernel-side\n", id);
                }
                pBuffer = NULL;
            }
            else
            {
                pBuffer->eStatus = INT_BUFFER_ACQUIRED;
            }
        }
    }
    else
    {
        LOG_ERROR("Could not find LSH matrix %d in user-side\n", id);
    }

    if (!pBuffer)
    {
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    pMatrix->id = pBuffer->ID;
    pMatrix->data = pBuffer->memory;
    pMatrix->ui32Size = pBuffer->uiSize;
    IMG_MEMCPY(&(pMatrix->config), &(pBuffer->config),
        sizeof(CI_MODULE_LSH_MAT));
    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineReleaseLSHMatrix(CI_PIPELINE *pPipeline,
    CI_LSHMAT *pMatrix)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;
    IMG_UINT32 id = 0;

    if (!pPipeline || !pMatrix || 0 == pMatrix->id)
    {
        LOG_ERROR("pPipeline or pMatrix is NULL or id is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    // search the specified buffer
    id = pMatrix->id;
    pFound = List_visitor(&(pIntPipe->sList_lshmat), &id,
        &List_SearchLSHMatrix);

    if (pFound)
    {
        struct CI_LSH_SET_PARAM sParam;
        INT_LSHMAT *pBuffer = NULL;

        IMG_MEMSET(&sParam, 0, sizeof(struct CI_LSH_SET_PARAM));
        pBuffer = container_of(pFound, INT_LSHMAT, sCell);

        if (INT_BUFFER_ACQUIRED != pBuffer->eStatus)
        {
            LOG_ERROR("LSH matrix %d not acquired\n", id);
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }

        // copy only the configuration
        IMG_MEMCPY(&(pBuffer->config), &(pMatrix->config),
            sizeof(CI_MODULE_LSH_MAT));

        sParam.configId = pIntPipe->ui32Identifier;
        sParam.matrixId = pBuffer->ID;
        sParam.ui16SkipX = pBuffer->config.ui16SkipX;
        sParam.ui16SkipY = pBuffer->config.ui16SkipY;
        sParam.ui16OffsetX = pBuffer->config.ui16OffsetX;
        sParam.ui16OffsetY = pBuffer->config.ui16OffsetY;
        sParam.ui8TileSizeLog2 = pBuffer->config.ui8TileSizeLog2;
        sParam.ui8BitsPerDiff = pBuffer->config.ui8BitsPerDiff;
        sParam.ui16Width = pBuffer->config.ui16Width;
        sParam.ui16Height = pBuffer->config.ui16Height;
        sParam.ui16LineSize = pBuffer->config.ui16LineSize;
        sParam.ui32Stride = pBuffer->config.ui32Stride;

        ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_SET_LSH, (long)&sParam);
        if (ret)
        {
            /* this is unlikely, and not sure what to do because user
                * already updated the memory so the matrix changed... */
            LOG_ERROR("Failed to update kernel side for LSH matrix %d!\n",
                id);
            return IMG_ERROR_FATAL;
        }

        pBuffer->eStatus = INT_BUFFER_AVAILABLE;
    }
    else
    {
        LOG_ERROR("Could not find LSH matrix %d\n", id);
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineUpdateLSHMatrix(CI_PIPELINE *pPipeline, IMG_UINT32 id)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    struct CI_LSH_CHANGE_PARAM sParam;
    INT_LSHMAT *pBuffer = NULL;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    if (0 != id)  // if 0 we want to disable LSH
    {
        // search the specified buffer
        sCell_T *pFound = List_visitor(&(pIntPipe->sList_lshmat), &id,
            &List_SearchLSHMatrix);

        if (pFound)
        {
            pBuffer = container_of(pFound, INT_LSHMAT, sCell);

            if (INT_BUFFER_AVAILABLE != pBuffer->eStatus)
            {
                LOG_ERROR("LSH matrix %d is not available\n", id);
                return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            }

            ret = CI_ModuleLSH_verif(&(pBuffer->config),
                &(pPipeline->sImagerInterface),
                &(pIntPipe->pConnection->publicConnection.sHWInfo));
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("The lens shading values are not correct!\n");
                return ret;
            }
        }
        else
        {
            LOG_ERROR("Could not find LSH matrix %d\n", id);
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct CI_LSH_CHANGE_PARAM));
    sParam.configId = pIntPipe->ui32Identifier;
    if (pBuffer)
    {
        sParam.matrixId = pBuffer->ID;
    }
    // otherwise 0 and we disable the matrix

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_CHA_LSH, (long)&sParam);
    if (ret)
    {
        /* this is unlikely, and not sure what to do because user
        * already updated the memory so the matrix changed... */
        LOG_ERROR("Failed to update kernel side for LSH matrix %d!\n",
            id);
        return toImgResult(ret);
    }

    if (pBuffer)
    {
        pBuffer->eStatus = INT_BUFFER_PENDING;
    }
    if (pIntPipe->pMatrix)
    {
        pIntPipe->pMatrix->eStatus = INT_BUFFER_AVAILABLE;
    }
    pIntPipe->pMatrix = pBuffer;  // may set to NULL if id == 0

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineFindFirstAvailable(CI_PIPELINE *pPipeline,
    CI_BUFFID *pIds)
{
    INT_PIPELINE *pIntPipe = NULL;
    int type;
    sCell_T *found = NULL;
    INT_BUFFER *pBuff = NULL;

    if (!pPipeline || !pIds)
    {
        LOG_ERROR("pPipeline or pIds is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_MEMSET(pIds, 0, sizeof(CI_BUFFID));

    if (PXL_NONE != pIntPipe->publicPipeline.config.eEncType.eFmt)
    {
        type = CI_ALLOC_ENC;
        found = List_visitor(&(pIntPipe->sList_buffers), &type,
            &List_FindAvailableBuffer);

        if (!found)
        {
            LOG_ERROR("Failed to find an available encoder "\
                "output buffer!\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
        pBuff = container_of(found, INT_BUFFER, sCell);
        pIds->encId = pBuff->ID;
    }

    if (PXL_NONE != pIntPipe->publicPipeline.config.eDispType.eFmt)
    {
        type = CI_ALLOC_DISP;
        found = List_visitor(&(pIntPipe->sList_buffers), &type,
            &List_FindAvailableBuffer);

        if (found == NULL)
        {
            LOG_ERROR("Failed to find an available display "\
                "output buffer!\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
        pBuff = container_of(found, INT_BUFFER, sCell);
        pIds->dispId = pBuff->ID;
    }

    if (PXL_NONE != pIntPipe->publicPipeline.config.eHDRExtType.eFmt)
    {
        type = CI_ALLOC_HDREXT;
        found = List_visitor(&(pIntPipe->sList_buffers), &type,
            &List_FindAvailableBuffer);

        if (found == NULL)
        {
            LOG_ERROR("Failed to find an available HDR Extraction "\
                "output buffer!\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
        pBuff = container_of(found, INT_BUFFER, sCell);
        pIds->idHDRExt = pBuff->ID;
    }

    // do not search for HDRIns - use reservation

    if (PXL_NONE != pIntPipe->publicPipeline.config.eRaw2DExtraction.eFmt)
    {
        type = CI_ALLOC_RAW2D;
        found = List_visitor(&(pIntPipe->sList_buffers), &type,
            &List_FindAvailableBuffer);

        if (found == NULL)
        {
            LOG_ERROR("Failed to find an available Raw2D Extraction "\
                "output buffer!\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
        pBuff = container_of(found, INT_BUFFER, sCell);
        pIds->idRaw2D = pBuff->ID;
    }

    return IMG_SUCCESS;
}

IMG_BOOL8 CI_PipelineHasPending(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int result = 0;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pPipeline == NULL)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier > 0)
    {
        result = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_PIPE_PEN, (long)pIntPipe->ui32Identifier);
        if (result < 0)
        {
            LOG_ERROR("Failed to verify if the capture has pending buffers\n");
        }
        return result > 0 ? IMG_TRUE : IMG_FALSE;
    }

    LOG_ERROR("Capture is not registered\n");
    return IMG_FALSE;
}

IMG_INT32 CI_PipelineHasWaiting(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int result = 0;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return -1;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier > 0)
    {
        result = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_PIPE_WAI, (long)pIntPipe->ui32Identifier);
        if (result < 0)
        {
            LOG_ERROR("Failed to verify if the capture has waiting " \
                "buffers\n");
        }
        return result;
    }

    LOG_ERROR("pipeline is not registered\n");
    return -1;
}

IMG_BOOL8 CI_PipelineHasAvailableShots(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret;
    struct CI_HAS_AVAIL sParam;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_FALSE;
    }
    sParam.captureId = pIntPipe->ui32Identifier;
    sParam.uiBuffers = 0;
    sParam.uiShots = 0;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_PIPE_AVL,
        (long)(&sParam));
    if (ret < 0)
    {
        LOG_ERROR("Failed to verify if the configuration has available "\
            "shots (returned %d)\n", ret);
        return toImgResult(ret);
    }
    return sParam.uiShots == 0 ? IMG_FALSE : IMG_TRUE;
}

IMG_BOOL8 CI_PipelineHasAvailableBuffers(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret;
    struct CI_HAS_AVAIL sParam;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_FALSE;
    }
    sParam.captureId = pIntPipe->ui32Identifier;
    sParam.uiBuffers = 0;
    sParam.uiShots = 0;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_PIPE_AVL,
        (long)(&sParam));
    if (ret < 0)
    {
        LOG_ERROR("Failed to verify if the configuration has available "\
            "buffers (returned %d)\n", ret);
        return toImgResult(ret);
    }
    return sParam.uiBuffers == 0 ? IMG_FALSE : IMG_TRUE;
}

IMG_BOOL8 CI_PipelineHasAvailable(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret;
    struct CI_HAS_AVAIL sParam;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_FALSE;
    }
    sParam.captureId = pIntPipe->ui32Identifier;
    sParam.uiBuffers = 0;
    sParam.uiShots = 0;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc, CI_IOCTL_PIPE_AVL,
        (long)(&sParam));
    if (ret < 0)
    {
        LOG_ERROR("Failed to verify if the configuration has available "\
            "buffers & shots (returned %d)\n", ret);
        return toImgResult(ret);
    }

    return sParam.uiBuffers > 0 && sParam.uiShots > 0 ? IMG_TRUE : IMG_FALSE;
}

IMG_INT32 CI_PipelineHasBuffers(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret = 0;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return -1;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return -1;
    }

    // different than available buffers!
    ret = pIntPipe->sList_buffers.ui32Elements;
    return ret;
}

IMG_INT32 CI_PipelineHasLSHBuffers(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret = 0;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return -1;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return -1;
    }

    ret = pIntPipe->sList_lshmat.ui32Elements;
    return ret;
}

IMG_BOOL8 CI_PipelineHasAcquired(const CI_PIPELINE *pPipeline)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret = 0;
    sCell_T *pFound = NULL;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_FALSE;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_FALSE;
    }

    pFound = List_visitor(&(pIntPipe->sList_shots), NULL,
        &List_SearchFirstAcquiredShot);
    if (!pFound)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

IMG_RESULT CI_PipelineFindBufferFromImportFd(const CI_PIPELINE *pPipeline,
    IMG_UINT32 ionFd, IMG_UINT32 *pBufferId)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret = 0;
    INT_BUFFER *pBuffer = NULL;
    sCell_T *found = NULL;

    if (!pPipeline || ionFd == 0 || !pBufferId)
    {
        LOG_ERROR("pPipeline or pBufferId is NULL or ionFd is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    found = List_visitor(&(pIntPipe->sList_buffers), &(ionFd),
        &List_SearchBufferIonFd);

    if (!found)
    {
        LOG_ERROR("Failed to find provided ionFd=%d", ionFd);
        return IMG_ERROR_FATAL;
    }

    pBuffer = container_of(found, INT_BUFFER, sCell);
    *pBufferId = pBuffer->ID;

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineGetBufferInfo(const CI_PIPELINE *pPipeline,
    IMG_UINT32 buffId, CI_BUFFER_INFO *pInfo)
{
    INT_PIPELINE *pIntPipe = NULL;
    int ret = 0;
    sCell_T *found = NULL;
    IMG_UINT32 id = buffId;
    INT_BUFFER *pBuff = NULL;

    if (!pPipeline || buffId == 0 || !pInfo)
    {
        LOG_ERROR("pPipeline or pBufferId is NULL or buffId is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    found = List_visitor(&(pIntPipe->sList_buffers), &(id),
        &List_SearchBuffer);

    if (!found)
    {
        LOG_ERROR("Failed to find provided buffId=%d", buffId);
        return IMG_ERROR_FATAL;
    }

    pBuff = container_of(found, INT_BUFFER, sCell);

    IMG_MEMSET(pInfo, 0, sizeof(CI_BUFFER_INFO));
    pInfo->id = pBuff->ID;
    pInfo->ionFd = pBuff->ionFd;
    pInfo->ui32Size = pBuff->uiSize;
    pInfo->type = pBuff->type;
    pInfo->bAvailable = IMG_FALSE;
    if ((CI_TYPE_HDRINS == pBuff->type
        && !pBuff->bHDRReserved)
        ||
        (CI_TYPE_HDRINS != pBuff->type
        && INT_BUFFER_AVAILABLE == pBuff->eStatus))
    {
        pInfo->bAvailable = IMG_TRUE;
    }
    pInfo->bIsTiled = pBuff->bIsTiled;

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineUpdate(CI_PIPELINE *pPipeline, int eUpdateMask,
    int *peFailure)
{
    return IMG_CI_PipelineUpdate(pPipeline, eUpdateMask, peFailure,
        IMG_FALSE);
}

IMG_RESULT CI_PipelineUpdateASAP(CI_PIPELINE *pPipeline, int eUpdateMask,
    int *peFailure)
{
    return IMG_CI_PipelineUpdate(pPipeline, eUpdateMask, peFailure, IMG_TRUE);
}

IMG_RESULT CI_PipelineDestroy(CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    // if pIntPipe->pConnection is NULL it's cleaning time, no need to be picky
    if (pIntPipe->pConnection != NULL)
    {
        if (pIntPipe->ui32Identifier != 0 && pIntPipe->bStarted)
        {
            ret = CI_PipelineStopCapture(pPipeline);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("could not stop the capture before destruction\n");
                return ret;
            }
        }
        List_detach(&(pIntPipe->sCell));
    }

    if (pIntPipe->ui32Identifier != 0)
    {
        // if not registered no shots can be allocated
        ret = CI_PipelineDeleteShots(pPipeline);
        if (ret)
        {
            LOG_ERROR("Failed to clear the shots\n");
            // return ret;
        }
    }

    pFound = List_visitor(&(pIntPipe->sList_buffers), pIntPipe->pConnection,
        &List_ClearBuffers);
    if (pFound)
    {
        LOG_ERROR("Failed to clear the list of buffers\n");
    }

    pFound = List_visitor(&(pIntPipe->sList_lshmat), pIntPipe->pConnection,
        &List_ClearLSHMat);
    if (pFound)
    {
        LOG_ERROR("Failed to clear the list of LSH matrix\n");
    }

    if (pPipeline->sDefectivePixels.sInput.apDefectInput)
    {
        IMG_FREE(pPipeline->sDefectivePixels.sInput.apDefectInput);
        pPipeline->sDefectivePixels.sInput.apDefectInput = NULL;
    }

    if (pIntPipe->pConnection)
    {
        if (pIntPipe->ui32Identifier != 0)
        {
            // if registered to the kernel side
            ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
                CI_IOCTL_PIPE_DEL, (long)(pIntPipe->ui32Identifier));
            if (0 != ret)
            {
                LOG_ERROR("Failed to deregister the configuration "\
                    "(returned %d)\n", ret);
                ret = toImgResult(ret);
            }
        }
    }

    IMG_FREE(pIntPipe);

    return ret;
}

IMG_RESULT CI_PipelineDeregisterBuffer(CI_PIPELINE *pPipeline,
    IMG_UINT32 bufferId)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T* pFound = NULL;
    INT_BUFFER* pBuffer = NULL;
    struct CI_ALLOC_PARAM param;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    // now allow to deregister buffers while running
    /*if (pIntPipe->bStarted)
    {
        LOG_ERROR("Cannot destroy buffer while pipeline is started\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }*/

    pFound = List_visitor(&(pIntPipe->sList_buffers), (void*)&bufferId,
        &List_SearchBuffer);
    if (!pFound)
    {
        LOG_ERROR("Buffer %d not found in userspace buffer list!\n", bufferId);
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pBuffer = container_of(pFound, INT_BUFFER, sCell);

    if (pBuffer->eStatus != INT_BUFFER_AVAILABLE)
    {
        LOG_ERROR("Cannot deregister a buffer that is acquired or pending "\
            "for capture - release it beforehand\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    List_detach(pFound);

    param.configId = pIntPipe->ui32Identifier;
    param.type = pBuffer->type;
    param.uiMemMapId = bufferId;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_DEREG_BUFF, (long)&param);
    if (ret < 0)
    {
        LOG_ERROR("Failed to deregister buffer %d\n", bufferId);
        List_pushBack(&(pIntPipe->sList_buffers), pFound);
        return toImgResult(ret);
    }

    // unmap from user-space
    SYS_IO_MemUnmap(pIntPipe->pConnection->fileDesc, pBuffer->memory,
        pBuffer->uiSize);
    IMG_FREE(pBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineVerify(CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret;
    IMG_UINT32 width, height;
    INT_PIPELINE *pIntPipe = NULL;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pPipeline->config.ui8Context >= CI_N_CONTEXT )
    {
        LOG_ERROR("chosen context %d is not supported (max %d)\n",
            pPipeline->config.ui8Context, CI_N_CONTEXT);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    // verfy capabilities of HW according to version
    {
        const CI_HWINFO *pHWInfo =
            &(pIntPipe->pConnection->publicConnection.sHWInfo);

        if ( !(pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT)
            && pPipeline->config.eHDRExtType.eFmt != PXL_NONE )
        {
            LOG_ERROR("The HDR Extraction point is not supported by "\
                "HW version %d.%d\n",
                pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        if ( !(pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT)
            && pPipeline->config.eRaw2DExtraction.eFmt != PXL_NONE )
        {
            LOG_ERROR("The HDR Extraction point is not supported by "\
                "HW version %d.%d\n",
                pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        if ( !(pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_TILING)
            && pPipeline->config.bSupportTiling )
        {
            LOG_ERROR("Tiling support is not possible with "\
                "HW version %d.%d\n",
                pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    /* technically the HW does not support BGR formats
     * but if R2Y is configured to swap channels it is possible to output
     * different order RGB */
    /*if (pPipeline->eDispType.eFmt == BGR_101010_32)
    {
        LOG_ERROR("The RGB format %s can only be used for HDR Extraction "\
            "- please use %s\n",
            FormatString(pPipeline->eDispType.eFmt),
            FormatString(RGB_101010_32));
        return IMG_ERROR_NOT_SUPPORTED;
    }*/
    if (pPipeline->config.eDataExtraction == CI_INOUT_NONE
        && pPipeline->config.eDispType.ui8BitDepth > 10)
    {
        LOG_ERROR("The RGB format %s cannot be used for display output\n",
            FormatString(pPipeline->config.eDispType.eFmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if ( pPipeline->config.eDispType.eFmt == BAYER_TIFF_10
        || pPipeline->config.eDispType.eFmt == BAYER_TIFF_12 )
    {
        LOG_ERROR("The Bayer TIFF format %s can only be used for Raw 2D "\
            "Extraction - please use other format e.g. %s\n",
            FormatString(pPipeline->config.eDispType.eFmt),
            FormatString(BAYER_RGGB_10));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if ( pPipeline->config.eHDRExtType.eFmt != PXL_NONE
        && pPipeline->config.eHDRExtType.eFmt != BGR_101010_32 )
    {
        LOG_ERROR("The HDR Extraction format can only be %s for this "\
            "version of the HW (found %s)\n",
            FormatString(BGR_101010_32),
            FormatString(pPipeline->config.eHDRExtType.eFmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (PXL_NONE != pPipeline->config.eHDRInsType.eFmt)
    {
        if (BGR_161616_64 != pPipeline->config.eHDRInsType.eFmt)
        {
            LOG_ERROR("The HDR Insertion format can only be %s for this "\
                "version of the HW (found %s)\n",
                FormatString(BGR_161616_64),
                FormatString(pPipeline->config.eHDRInsType.eFmt));
            return IMG_ERROR_NOT_SUPPORTED;
        }

#ifdef BRN50034
        /*
         * BRN50034: if enabled HDF insertion max size is single context size
         */
        if ((pPipeline->sImagerInterface.ui16ImagerSize[0] + 1) * 2
            > pIntPipe->pConnection->publicConnection.sHWInfo.\
            context_aMaxWidthMult[pPipeline->config.ui8Context])
        {
            LOG_ERROR("The HDR Insertion can only be enabled for a maximum "\
                "size supported by multiple contexts\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
#endif
    }

    if ( pPipeline->config.eRaw2DExtraction.eFmt != PXL_NONE
        && pPipeline->config.eRaw2DExtraction.eFmt != BAYER_TIFF_10
        && pPipeline->config.eRaw2DExtraction.eFmt != BAYER_TIFF_12 )
    {
        LOG_ERROR("The Raw2D output format %s is not supported - it needs"\
            "to be a TIFF format\n",
            FormatString(pPipeline->config.eRaw2DExtraction.eFmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = CI_ModuleIIF_verif(&(pPipeline->sImagerInterface),
        pPipeline->config.ui8Context,
        &(pIntPipe->pConnection->publicConnection.sHWInfo));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The Imager Interface values are not correct!\n");
        return ret;
    }

    // CI_ModuleBLC_verif()

    // CI_ModuleRLT_verif()

    if (pIntPipe->pMatrix)
    {
        ret = CI_ModuleLSH_verif(&(pIntPipe->pMatrix->config),
            &(pPipeline->sImagerInterface),
            &(pIntPipe->pConnection->publicConnection.sHWInfo));
        if (ret)
        {
            LOG_ERROR("The lens shading values are not correct!\n");
            return ret;
        }
    }
    ret = CI_ModuleWBC_verif(&(pPipeline->sWhiteBalance));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The WBC configuration is not correct!\n");
        return ret;
    }
    // CI_ModuleDNS_verif()

    ret = CI_ModuleDPF_verif(&pPipeline->sDefectivePixels,
        &(pIntPipe->pConnection->publicConnection.sHWInfo));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The defective pixel setup is not correct!\n");
        return ret;
    }
    // CI_ModuleLCA_verif()
    // add DMO
    // CI_ModuleCCM_verif()
    // CI_ModuleMGM_verif()
    // nothing to verify for GMA
    // CI_ModuleR2Y_verif()
    // CI_ModuleMIE_verif()
    // add HSS

    ret = CI_ModuleTNM_verif(&pPipeline->sToneMapping);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The Tone Mapping configuration is not correct\n");
        return ret;
    }
    // add DN2
    // CI_MoudleSHA_verif()

    // 420 output but encoder outputs 422
    if (pPipeline->config.eEncType.ui8VSubsampling == 2
        && pPipeline->sEncoderScaler.bOutput422)
    {
        LOG_ERROR("The encoder scaler is configured to output 422 while "\
            "pipeline output is configured for 420\n");
        return IMG_ERROR_FATAL;
    }

    ret = CI_ModuleScaler_verif(&pPipeline->sEncoderScaler);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The encoder scaler values are not correct!\n");
        return ret;
    }

    ret = CI_ModuleScaler_verif(&pPipeline->sDisplayScaler);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("The display scaler values are not correct!\n");
        return ret;
    }

    // add HUS
    // CI_ModuleY2R_verif()
    // CI_ModuleDGM_verif()

    // statistics

    /** @ verify that the stats are using a clipping rect that is
     * available with the output of the IIF */
    if ((pPipeline->config.eOutputConfig&CI_SAVE_EXS_REGION) != 0)
    {
        ret = CI_StatsEXS_verif(&pPipeline->stats.sExposureStats);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Exposure statistics configuration is not correct!\n");
            return ret;
        }
    }
    // CI_StatsFOS_verif()

    ret = CI_StatsWBS_verif(&pPipeline->stats.sWhiteBalanceStats);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("White Balance statistics configuration is not correct!\n");
        return ret;
    }
    ret = CI_StatsAWS_verif(&pPipeline->stats.sAutoWhiteBalanceStats,
            &pPipeline->sImagerInterface);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Auto White Balance statistics configuration is not "
            "correct!\n");
        return ret;
    }
    if ((pPipeline->config.eOutputConfig&(CI_SAVE_HIST_GLOBAL | CI_SAVE_HIST_REGION)) != 0)
    {
        ret = CI_StatsHIS_verif(&pPipeline->stats.sHistStats);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Histogram statistics configuration is not correct!\n");
            return ret;
        }
    }
    if ((pPipeline->config.eOutputConfig&CI_SAVE_FLICKER) != 0)
    {
        ret = CI_StatsFLD_verif(&pPipeline->stats.sFlickerStats);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("The flicker detection values are not correct!\n");
            return ret;
        }
    }

    if ((pPipeline->config.eOutputConfig&CI_SAVE_ENS) != 0)
    {
        ret = CI_StatsENS_verif(&pPipeline->stats.sEncStats);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("The Encoder Stats values are not correct!\n");
            return ret;
        }
    }

    /*
     * check other information
     */

    // register size is stored that way
    width = pPipeline->sEncoderScaler.aOutputSize[0]*2 +2;
    height = pPipeline->sEncoderScaler.aOutputSize[1]+1;
    if ( pPipeline->config.eEncType.eBuffer != TYPE_NONE &&
        (width > pPipeline->config.ui16MaxEncOutWidth
        || height > pPipeline->config.ui16MaxEncOutHeight) )
    {
        LOG_ERROR("Encoder output size is too big for allocated buffers\n");
        return IMG_ERROR_FATAL;
    }

    // register size is stored that way
    width = pPipeline->sDisplayScaler.aOutputSize[0]*2 +2;
    height = pPipeline->sDisplayScaler.aOutputSize[1]+1;
    if ( pPipeline->config.eDispType.eBuffer != TYPE_NONE
        && pPipeline->config.eDataExtraction == CI_INOUT_NONE &&
        (width > pPipeline->config.ui16MaxDispOutWidth
        || height > pPipeline->config.ui16MaxDispOutHeight) )
    {
        LOG_ERROR("Display output size is too big for allocated buffers\n");
        return IMG_ERROR_FATAL;
    }

    if ( pPipeline->config.eDataExtraction != CI_INOUT_NONE &&
        (pPipeline->config.ui16MaxDispOutWidth *
            pPipeline->config.ui16MaxDispOutHeight) <= 0 )
    {
        LOG_ERROR("Data Extraction is enabled but display output size is 0\n");
        return IMG_ERROR_FATAL;
    }

    if ( pPipeline->config.ui8Context >=
        pIntPipe->pConnection->publicConnection.sHWInfo.config_ui8NContexts )
    {
        LOG_ERROR("maximum number of context is %u (using %u)\n",
            pIntPipe->pConnection->publicConnection.\
            sHWInfo.config_ui8NContexts,
            pPipeline->config.ui8Context);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineImportBuffer(CI_PIPELINE *pPipeline,
    enum CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size, IMG_BOOL8 bTiled,
    IMG_UINT32 ionFd, IMG_UINT32 *pBufferId)
{
    if (ionFd == 0)
    {
        LOG_ERROR("Given buffer ID %d is invalid!\n", ionFd);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_CI_PipelineCreateBuffer(pPipeline, eBuffer, ui32Size,
        bTiled, ionFd, pBufferId);
}

IMG_RESULT CI_PipelineAllocateBuffer(CI_PIPELINE *pPipeline,
    enum CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size, IMG_BOOL8 bTiled,
    IMG_UINT32 *pBufferId)
{
    return IMG_CI_PipelineCreateBuffer(pPipeline, eBuffer, ui32Size, bTiled,
        0, pBufferId);
}

IMG_RESULT CI_PipelineAllocateLSHMatrix(CI_PIPELINE *pPipeline,
    IMG_UINT32 ui32Size, IMG_UINT32 *pMatrixId)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    struct CI_ALLOC_PARAM param;
    INT_LSHMAT *pBuff = NULL;
    const int page_prot = PROT_WRITE;  // LSH matrix need to be written to

    if (!pPipeline || 0 == ui32Size)
    {
        LOG_ERROR("pPipeline is NULL or size is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (0 == pIntPipe->ui32Identifier)
    {
        LOG_ERROR("Pipeline is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&param, 0, sizeof(struct CI_ALLOC_PARAM));
    param.configId = pIntPipe->ui32Identifier;
    param.fd = 0;
    param.uiSize = ui32Size;
    param.bTiled = IMG_FALSE;  // matrix cannot be tiled
    param.type = CI_ALLOC_LSHMAT;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_CREATE_BUFF, (long)&param);
    if (ret)
    {
        LOG_ERROR("Failed to create lsh matrix in kernel space "\
            "(returned %d)\n", ret);
        return toImgResult(ret);
    }

    pBuff = (INT_LSHMAT *)IMG_CALLOC(1, sizeof(INT_LSHMAT));
    if (!pBuff)
    {
        LOG_ERROR("Failed to allocate new internal LSH matrix\n");
        /// @ how to notify kernel-space?
        return IMG_ERROR_MALLOC_FAILED;
    }

    // so that both container_of and the internal pointer work
    pBuff->sCell.object = pBuff;
    pBuff->eStatus = INT_BUFFER_AVAILABLE;
    pBuff->ID = param.uiMemMapId;
    pBuff->uiSize = param.uiSize;
    pBuff->memory =
        SYS_IO_MemMap2(pIntPipe->pConnection->fileDesc,
        pBuff->uiSize, page_prot, MAP_SHARED,
        pBuff->ID);

    if (!pBuff->memory)
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("Failed to map Buffer to user-space\n");
        IMG_FREE(pBuff);
        /// @ how to notify kernel-space?
    }
    else
    {
        ret = List_pushBack(&(pIntPipe->sList_lshmat), &(pBuff->sCell));
        if (ret)
        {
            // unlikely!!! could be an assert
            LOG_ERROR("Failed to add buffer to the Pipeline list!\n");
            SYS_IO_MemUnmap(pIntPipe->pConnection->fileDesc,
                pBuff->memory,
                pBuff->uiSize);
            IMG_FREE(pBuff);
        }
        LOG_DEBUG("CI map in LSH matrix of size 0x%lx\n",
            pBuff->uiSize);
    }

    if (IMG_SUCCESS == ret && pMatrixId)
    {
        *pMatrixId = pBuff->ID;
    }
    return ret;
}

IMG_RESULT CI_PipelineDeregisterLSHMatrix(CI_PIPELINE *pPipeline,
    IMG_UINT32 matrixId)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T* pFound = NULL;
    INT_LSHMAT* pBuffer = NULL;
    struct CI_ALLOC_PARAM param;

    if (!pPipeline || 0 == matrixId)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);

    pFound = List_visitor(&(pIntPipe->sList_lshmat), &matrixId,
        &List_SearchLSHMatrix);
    if (!pFound)
    {
        LOG_ERROR("LSH matrix %d not found in userspace buffer list!\n",
            matrixId);
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    pBuffer = container_of(pFound, INT_LSHMAT, sCell);

    List_detach(pFound);

    param.configId = pIntPipe->ui32Identifier;
    param.type = CI_ALLOC_LSHMAT;
    param.uiMemMapId = pBuffer->ID;

    ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
        CI_IOCTL_DEREG_BUFF, (long)&param);
    if (ret < 0)
    {
        LOG_ERROR("Failed to deregister LSH matrix %d\n", matrixId);
        List_pushBack(&(pIntPipe->sList_lshmat), pFound);
        return toImgResult(ret);
    }

    // unmap from user-space
    SYS_IO_MemUnmap(pIntPipe->pConnection->fileDesc,
        pBuffer->memory,
        pBuffer->uiSize);
    IMG_FREE(pBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT CI_PipelineComputeLinestore(CI_PIPELINE *pPipeline)
{
    struct _sTaken_ aTaken[CI_N_CONTEXT];
    IMG_INT32 nTaken = 0;
    int ctx;
    IMG_RESULT ret;
    IMG_UINT32 gapSize = 0, gapStart = 0, neededSize = 0;
    INT_PIPELINE *pIntPipe = NULL;
    CI_LINESTORE sLocalCopy;
    /*  >= 0 if found a context that reserved linestore using value > than
     * its context_aMaxWidthMult */
    int foundSingle = -1;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe);
    IMG_ASSERT(pIntPipe->pConnection);

    if (pIntPipe->bStarted == IMG_TRUE)
    {
        LOG_ERROR("cannot change linestore of started pipeline!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    neededSize = (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2;
    if (neededSize == 0)
    {
        LOG_ERROR("IIF not configured! width is 0!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* update the linestore from the driver - if a context is running its
     * size should be updated and can be changed*/
    ret = CI_DriverGetLinestore(&(pIntPipe->pConnection->publicConnection));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to access the driver to get current linestore!\n");
        return ret;
    }
    IMG_MEMCPY(&sLocalCopy,
        &(pIntPipe->pConnection->publicConnection.sLinestoreStatus),
        sizeof(CI_LINESTORE));

    if ( sLocalCopy.aActive[pPipeline->config.ui8Context] == IMG_TRUE )
    {
        LOG_ERROR("context is already active: no point to compute a "\
            "linestore for it (will not start)\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // find all the taken locations
    for (ctx = 0 ; ctx <
        pIntPipe->pConnection->publicConnection.sHWInfo.config_ui8NContexts ;
        ctx++)
    {
        if (sLocalCopy.aActive[ctx] == IMG_TRUE)
        {
            aTaken[nTaken].start = sLocalCopy.aStart[ctx];
            aTaken[nTaken].size = sLocalCopy.aSize[ctx];
            aTaken[nTaken].ctx = ctx;
            nTaken++;

            if (sLocalCopy.aSize[ctx] >
                pIntPipe->pConnection->publicConnection.\
                sHWInfo.context_aMaxWidthMult[ctx])
            {
                foundSingle = ctx;
            }
        }
        else
        {  // disable that context from the check
            sLocalCopy.aStart[ctx] = -1;
            sLocalCopy.aSize[ctx] = 0;
        }
    }

    // cannot be the current context because linestore is reserved when starting
    // and we checked this Pipeline object is not started
    if (foundSingle >= 0)
    {
        LOG_ERROR("Context %d is running using more than its "\
            "multi-context size - cannot reserve linestore for "\
            "context ctx %d\n",
            foundSingle, pPipeline->config.ui8Context);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (nTaken > 1)
    {
        // sort the taken elements using start position
        qsort(aTaken, nTaken, sizeof(struct _sTaken_), &cmpTaken);
    }

    // if we try to configure that context and it is not already started
    // find the first available gap that is big enough
    gapStart = 0;
    /* assumes no other contexts are running - we can try to use the
     * maximum size! */
    gapSize = pIntPipe->pConnection->publicConnection.\
        sHWInfo.context_aMaxWidthSingle[pPipeline->config.ui8Context];

    if (nTaken > 0)
    {
        /* when at least 1 context is enabled we search for the 1st big
         * enough gap */
        for (ctx = 0 ; ctx < nTaken ; ctx++)
        {
            gapSize = aTaken[ctx].start - gapStart;
            if (gapSize >= neededSize)
            {
                //LOG_INFO("found gap [%d for %d]\n", gapStart, gapSize);
                break;
            }
            gapStart = (aTaken[ctx].start+aTaken[ctx].size);
        }
        if (gapSize < neededSize)
        {
            // after last taken checked and it is still too small try after it
            IMG_UINT32 gapStart2 =
                (aTaken[nTaken-1].start + aTaken[nTaken-1].size);
            IMG_UINT32 gapSize2 = pIntPipe->pConnection->publicConnection.\
                sHWInfo.ui32MaxLineStore - gapStart2;
            if (gapSize2 > gapSize)
            {
                gapSize = gapSize2;
                gapStart = gapStart2;
            }
        }
    }

    if (gapSize < neededSize)
    {
        LOG_ERROR("cannot fit ctx %d into linestore (need %d, biggest gap "\
            "found is %d)\n", pPipeline->config.ui8Context, neededSize, gapSize);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    else
    {
        /* only interesting to update taken if an array of pipline
         * structure was given*/
        /*aTaken[nTaken].start = gapStart;
        aTaken[nTaken].size = neededSize;
        aTaken[nTaken].ctx = pPipeline->ui8Context;
        nTaken++;*/

        // the sizes are computed when submitted
        sLocalCopy.aStart[pPipeline->config.ui8Context] = gapStart;
    }

    ret = CI_DriverSetLinestore(&(pIntPipe->pConnection->publicConnection),
        &(sLocalCopy));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("computed linestore refused\n");
        /* no need to update the linestore from the driver to revert
         * changes as we gave a copy not the original one */
        // CI_DriverGetLinestore();
    }
    return ret;
}

IMG_RESULT CI_PipelineAddPool(CI_PIPELINE *pPipeline, IMG_UINT32 ui32NBuffers)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    IMG_UINT32 length = 0;
    struct CI_POOL_PARAM param;
    unsigned int i;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline or pBuffTiled is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }


    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe != NULL);
    IMG_ASSERT(pIntPipe->pConnection != NULL);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("configuration is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (ui32NBuffers == 0)
    {
        LOG_WARNING("no buffer to add...\n");
        return IMG_SUCCESS;
    }

    for (i = 0 ; i < ui32NBuffers && ret == IMG_SUCCESS ; i++)
    {
        INT_SHOT *pNewShot = (INT_SHOT*)IMG_CALLOC(1, sizeof(INT_SHOT));

        if (pNewShot == NULL)
        {
            LOG_ERROR("Failed to allocate one of new buffer (%luB) %d/%d\n",
                (unsigned long)sizeof(INT_SHOT), i, ui32NBuffers);
            return IMG_ERROR_MALLOC_FAILED;
        }

        IMG_MEMSET(&param, 0, sizeof(struct CI_POOL_PARAM));
        param.configId = pIntPipe->ui32Identifier;

        ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_PIPE_ADD, (long)&param);
        if (0 != ret)
        {
            LOG_ERROR("Failed to add buffer %d/%d to the configuration "\
                "(returned %d)\n", i, ui32NBuffers, ret);
            IMG_FREE(pNewShot);
            return toImgResult(ret);
        }

        // copy the sample information (such as stride)
        IMG_MEMCPY(&(pNewShot->publicShot), &param.sample, sizeof(CI_SHOT));

        pNewShot->iIdentifier = param.iShotId;
        pNewShot->publicShot.pEncoderOutput = NULL;
        pNewShot->publicShot.pDisplayOutput = NULL;
        // so that both container_of and the internal pointer work
        pNewShot->sCell.object = pNewShot;
        // stat size and DPF output size copied from sample

        /* dynamic buffers should be allocated with
         * CI_PipelineAllocateBuffer() */

        if (param.aMemMapIds[CI_POOL_MAP_ID_STAT] != 0)
        {
            /* statistics are not in a INT_BUFFER because they are not
             * chosable by high level when triggering a shoot */
            pNewShot->publicShot.pStatistics =
                SYS_IO_MemMap2(pIntPipe->pConnection->fileDesc,
                    pNewShot->publicShot.statsSize, PROT_READ, MAP_SHARED,
                    param.aMemMapIds[CI_POOL_MAP_ID_STAT]);
            if (!pNewShot->publicShot.pStatistics)
            {
                ret = IMG_ERROR_FATAL;
                LOG_ERROR("Failed to map the statistics to "\
                    "user-space %d/%d\n", i, ui32NBuffers);
            }
        }
        else
        {
            pNewShot->publicShot.pStatistics = NULL;
        }

        if (param.aMemMapIds[CI_POOL_MAP_ID_DPF] != 0)
        {
            /* DPF is not in a INT_BUFFER because they are not chosable by
             * high level when triggering a shoot */
            pNewShot->publicShot.pDPFMap =
                SYS_IO_MemMap2(pIntPipe->pConnection->fileDesc,
                    pNewShot->publicShot.uiDPFSize, PROT_READ, MAP_SHARED,
                    param.aMemMapIds[CI_POOL_MAP_ID_DPF]);
            if (!pNewShot->publicShot.pDPFMap)
            {
                ret = IMG_ERROR_FATAL;
                LOG_ERROR("Failed to map the DPF output to user-space "\
                    "%d/%d\n", i, ui32NBuffers);
            }
        }
        else
        {
            pNewShot->publicShot.pDPFMap = NULL;
        }

        if (param.aMemMapIds[CI_POOL_MAP_ID_ENS] != 0)
        {
            /* ENS is not in a INT_BUFFER because they are not chosable by
             * high level when triggering a shoot*/
            pNewShot->publicShot.pENSOutput =
                SYS_IO_MemMap2(pIntPipe->pConnection->fileDesc,
                pNewShot->publicShot.uiENSSize, PROT_READ, MAP_SHARED,
                param.aMemMapIds[CI_POOL_MAP_ID_ENS]);
            if (!pNewShot->publicShot.pENSOutput)
            {
                ret = IMG_ERROR_FATAL;
                LOG_ERROR("Failed to map the ENS output to user-space "\
                    "%d/%d\n", i, ui32NBuffers);
            }
        }
        else
        {
            pNewShot->publicShot.pENSOutput = NULL;
        }

        if (IMG_SUCCESS != ret)
        {
            /* clears stats, DPF, ENS and the shot object - does not clear
             * the output buffers*/
            List_ClearShots(pNewShot, pIntPipe->pConnection);
        }
        else
        {
            ret = List_pushBack(&(pIntPipe->sList_shots), &(pNewShot->sCell));
            /* insertion on a list should not fail - the cell is already
             * allocated and not attached */
            IMG_ASSERT(ret == IMG_SUCCESS);
        }
    } // for nb buffers

    return ret;
}

IMG_RESULT CI_PipelineDeleteShots(CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret = IMG_SUCCESS;
    INT_PIPELINE *pIntPipe = NULL;
    sCell_T *pFound = NULL;

    if (!pPipeline)
    {
        LOG_ERROR("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntPipe = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    IMG_ASSERT(pIntPipe != NULL);

    if (pIntPipe->ui32Identifier == 0)
    {
        LOG_ERROR("pipeline is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    // we need to empty the shots list
    pFound = List_popFront(&(pIntPipe->sList_shots));
    while (NULL != pFound)
    {
        List_ClearShots(pFound->object, pIntPipe->pConnection);
        pFound = List_popFront(&(pIntPipe->sList_shots));
    }

    // if no connection we are just cleaning memory
    if (pIntPipe->pConnection)
    {
        ret = SYS_IO_Control(pIntPipe->pConnection->fileDesc,
            CI_IOCTL_PIPE_REM, (long)pIntPipe->ui32Identifier);
        if (0 != ret)
        {
            LOG_ERROR("Failed to remove shots (ret=%d)\n", toImgResult(ret));
            return toImgResult(ret);
        }
    }

    return ret;
}
