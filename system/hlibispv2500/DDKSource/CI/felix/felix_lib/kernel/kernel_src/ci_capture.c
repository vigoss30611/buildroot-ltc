/**
*******************************************************************************
@file ci_capture.c

@brief

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

#include <tal.h>
#include <reg_io2.h>

//#include <hw_struct/ctx_pointers.h>
#ifdef INFOTM_ISP
#include <hw_struct/save.h>
#endif //INFOTM_ISP

#ifdef INFOTM_ENABLE_ISP_DEBUG
#include <registers/fields_context0.h>
#include <registers/fields_gasket_mipi.h>
#include <registers/fields_gasket_parallel.h>
#endif //INFOTM_ENABLE_ISP_DEBUG


#include "ci/ci_api_structs.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_hwstruct.h"
#include "ci_kernel/ci_debug.h"

#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
#include <gzip_fileio.h>
#ifndef FELIX_DUMP_SHOT_FILE_PFRX
#define FELIX_DUMP_SHOT_FILE_PFRX "shot"
#endif
#ifndef FELIX_DUMP_REGISTER_FIELDS
#define FELIX_DUMP_REGISTER_FIELDS IMG_TRUE
#endif
#ifndef FELIX_DUMP_COMPRESSED_REGISTERS
#define FELIX_DUMP_COMPRESSED_REGISTERS IMG_FALSE
#endif
#endif /* CI_DEBUG_FCT && FELIX_FAKE */

/** @brief helps reduce the line size when doing pdump comments */
#define REGFELIXCONTEXT g_psCIDriver->sTalHandles.hRegFelixContext

/*-----------------------------------------------------------------------------
 * these functions have static scope
 *---------------------------------------------------------------------------*/

/**
 * @ingroup KRN_CI_PipelineCapture
 *
 * @param listParam a KRN_CI_SHOT pointer (from the list)
 * @param status looked for
 */
static IMG_BOOL8 FindFirstWithStatus(void* listParam, void* status)
{
    KRN_CI_SHOT *pGivenShot = (KRN_CI_SHOT*)listParam;
    enum KRN_CI_SHOT_eSTATUS* searchedStatus =
        (enum KRN_CI_SHOT_eSTATUS*)status;

    if (pGivenShot->eStatus == *searchedStatus)
    {
        return IMG_FALSE; // stop processing
    }
    return IMG_TRUE; // continue processing
}

/**
 * @ingroup KRN_CI_PipelineCapture
 */
static IMG_BOOL8 List_SearchShot(void *listelem, void *lookingFor)
{
    int *identifier = (int*)lookingFor;
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)listelem;

    if (pShot->iIdentifier == *identifier) return IMG_FALSE;
    return IMG_TRUE;
}

/**
 * @ingroup KRN_CI_PipelineCapture
 */
static IMG_BOOL8 List_MapShots(void *listelem, void *directory)
{
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)listelem;
    struct MMUDirectory *pDirectory = (struct MMUDirectory*)directory;
    IMG_RESULT ret = KRN_CI_ShotMapMemory(pShot, pDirectory);

    if (IMG_SUCCESS != ret)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

IMG_BOOL8 List_MapBuffer(void *listelem, void *directory)
{
    KRN_CI_BUFFER *pBuffer = (KRN_CI_BUFFER*)listelem;
    struct MMUDirectory *pDirectory = (struct MMUDirectory*)directory;
    int ret;

    if (pBuffer->type != CI_BUFF_HDR_RD)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "dynamic buffer (WO)");
        ret = SYS_MemMap(&(pBuffer->sMemory), pDirectory, MMU_WO);
    }
    else
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "dynamic buffer (RO)");
        ret = SYS_MemMap(&(pBuffer->sMemory), pDirectory, MMU_RO);
    }
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to map a specifiable buffer "\
            "(SYS_MemMap returned %d)\n", ret);
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

/**
 * @ingroup KRN_CI_PipelineCapture
 */
static IMG_BOOL8 List_UnmapShots(void *listelem, void *unused)
{
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)listelem;

    (void)unused;
    KRN_CI_ShotUnmapMemory(pShot);
    return IMG_TRUE;
}

/**
 * @ingroup KRN_CI_PipelineCapture
 */
static IMG_BOOL8 List_UnmapBuffer(void *listelem, void *unused)
{
    KRN_CI_BUFFER *pBuffer = (KRN_CI_BUFFER*)listelem;

//#ifdef INFOTM_ENABLE_ISP_DEBUG
//    printk("@@@ List_UnmapBuffer: %s, %p\n", pBuffer->sMemory.szMemName, pBuffer->sMemory.pMapping);
//#endif //INFOTM_ENABLE_ISP_DEBUG
    (void)unused;
    if (pBuffer->sMemory.pMapping)
    {
        SYS_MemUnmap(&(pBuffer->sMemory));
    }
    return IMG_TRUE;
}

#ifdef INFOTM_ISP
void KRN_CI_PipelineDumpListElements(KRN_CI_PIPELINE *pPipeline, IMG_BOOL bPrintGasketInfo)
{
#ifdef INFOTM_ENABLE_ISP_DEBUG
	CI_GASKET_INFO GasketInfo;
	IMG_UINT8 GasketNum = 0;

	printk("@@@  ISP_List[%d]: %d %d %d %d\n", pPipeline->userPipeline.ui8Context,
								pPipeline->sList_available.ui32Elements,
								pPipeline->sList_pending.ui32Elements,
								pPipeline->sList_processed.ui32Elements,
								pPipeline->sList_sent.ui32Elements);


	if (bPrintGasketInfo)
	{
		GasketNum = (pPipeline->userPipeline.ui8Context == 0) ? 0 : 2; //0:MIPI0, 1:MIPI1, 2:DVP

		HW_CI_ReadGasket(&GasketInfo, GasketNum);

		printk("@@@ Gasket=%d, eType=%d, FrameCount=%d\n", GasketNum, GasketInfo.eType, GasketInfo.ui32FrameCount);
		if (GasketInfo.eType & CI_GASKET_MIPI)
		{
			printk("@@@ Gasket MIPI FIFO %d - Enabled lanes %d\n", GasketInfo.ui8MipiFifoFull, GasketInfo.ui8MipiEnabledLanes);
			printk("@@@ Gasket MIPI CRC Error: 0x%X\n", GasketInfo.ui8MipiCrcError);
			printk("@@@ Gasket MIPI Header Error: 0x%X\n", GasketInfo.ui8MipiHdrError);
			printk("@@@ Gasket MIPI ECC Error: 0x%X\n", GasketInfo.ui8MipiEccError);
			printk("@@@ Gasket MIPI ECC Correted: 0x%X\n", GasketInfo.ui8MipiEccCorrected);
		}
	}

	//HW_CI_ReadContextToPrint(0);
#endif //INFOTM_ENABLE_ISP_DEBUG
}
#endif //INFOTM_ISP
/**
 * @ingroup KRN_CI_PipelineCapture
 * @brief Release a shot (makes it AVAILABLE) and push it back to the
 * available list (and its buffers to the availableBuffer list)
 * @warning Assumes pPipeline->sAvailableLock is locked!
 */
static IMG_RESULT IMG_PipelineReleaseShot(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_SHOT *pShot)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pShot);

	pShot->eStatus = CI_SHOT_AVAILABLE;

    // push back buffers into the available list
    if (pShot->phwEncoderOutput)
    {
        IMG_ASSERT(pShot->phwEncoderOutput->sCell.object);
        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pShot->phwEncoderOutput->sCell));
        // shot does not own the buffer anymore
        pShot->phwEncoderOutput = NULL;
        // should never be something else: just found it in the list!
        IMG_ASSERT(ret == IMG_SUCCESS);
    }
    if (pShot->phwDisplayOutput)
    {
        IMG_ASSERT(pShot->phwDisplayOutput->sCell.object);
        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pShot->phwDisplayOutput->sCell));
        // shot does not own the buffer anymore
        pShot->phwDisplayOutput = NULL;
        // should never be something else: just found it in the list!
        IMG_ASSERT(ret == IMG_SUCCESS);
    }
    if (pShot->phwHDRExtOutput)
    {
        IMG_ASSERT(pShot->phwHDRExtOutput->sCell.object);
        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pShot->phwHDRExtOutput->sCell));
        // shot does not own the buffer anymore
        pShot->phwHDRExtOutput = NULL;
        // should never be something else: just found it in the list!
        IMG_ASSERT(ret == IMG_SUCCESS);
    }
    if (pShot->phwHDRInsertion)
    {
        IMG_ASSERT(pShot->phwHDRInsertion->sCell.object);
        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pShot->phwHDRInsertion->sCell));
        // shot does not own the buffer anymore
        pShot->phwHDRInsertion = NULL;
        // should never be something else: just found it in the list!
        IMG_ASSERT(ret == IMG_SUCCESS);
    }
    if (pShot->phwRaw2DExtOutput)
    {
        IMG_ASSERT(pShot->phwRaw2DExtOutput->sCell.object);
        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pShot->phwRaw2DExtOutput->sCell));
        // shot does not own the buffer anymore
        pShot->phwRaw2DExtOutput = NULL;
        // should never be something else: just found it in the list!
        IMG_ASSERT(ret == IMG_SUCCESS);
    }
    List_pushBack(&(pPipeline->sList_available), &(pShot->sListCell));

    return ret;
}

/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_PipelineCapture functions group
 *---------------------------------------------------------------------------*/

IMG_RESULT KRN_CI_PipelineTriggerShoot(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bBlocking, CI_BUFFID *pBuffId)
{
    sCell_T *pGivenBuffer = NULL;
    KRN_CI_SHOT *pCurrent = NULL;
    // cumulative CI_MODULE_UPDATE from pipeline
    IMG_UINT32 ui32ElemToUpdate = CI_UPD_NONE;
    int ret = 0;
    IMG_UINT32 needed; // store some needed size in if

    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pBuffId);
    if (!pPipeline->bStarted)
    {
        CI_FATAL("capture is not started.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

#ifdef CI_DEBUGFS
    g_ui32NCTXSubmitted[pPipeline->userPipeline.ui8Context]++;
#endif

    /* BRN50357 data insertion has to be on or off or memory will get
    * corrupted in HW */
    if (PXL_NONE != pPipeline->userPipeline.eHDRInsType.eFmt
        && 0 == pBuffId->idHDRIns)
    {
        CI_FATAL("If HDR Insertion is enabled it has to be pushed for "\
            "every frame!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /* pBuffId->raw2DStride not checked because it does not
     * have to be a multiple of SYSMEM_ALIGNMENT */
    if (pBuffId->encStrideY%SYSMEM_ALIGNMENT
        || pBuffId->encStrideC%SYSMEM_ALIGNMENT
        || pBuffId->dispStride%SYSMEM_ALIGNMENT
        || pBuffId->HDRExtStride%SYSMEM_ALIGNMENT
        || pBuffId->HDRInsStride%SYSMEM_ALIGNMENT)
    {
        CI_FATAL("A proposed stride is not a multiple of %u Bytes\n",
            SYSMEM_ALIGNMENT);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    /* pBuffId->raw2DOffset not checked because it does not
     * have to be a multiple of SYSMEM_ALIGNMENT */
    if (pBuffId->encOffsetY%SYSMEM_ALIGNMENT
        || pBuffId->encOffsetY%SYSMEM_ALIGNMENT
        || pBuffId->dispOffset%SYSMEM_ALIGNMENT
        || pBuffId->HDRExtOffset%SYSMEM_ALIGNMENT
        || pBuffId->HDRInsOffset%SYSMEM_ALIGNMENT)
    {
        CI_FATAL("A proposed offset is not a multiple of %u Bytes\n",
            SYSMEM_ALIGNMENT);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // acquire an available slot in the semaphore
    if (bBlocking)
    {
        ret = SYS_SemTimedDecrement(
            &(g_psCIDriver->aListQueue[pPipeline->userPipeline.ui8Context]),
            g_psCIDriver->uiSemWait);
        if (IMG_SUCCESS != ret)
        {
            if (IMG_ERROR_TIMEOUT != ret)
            {
                CI_FATAL("Failed to wait on the semaphore\n");
                return IMG_ERROR_FATAL;
            }
#ifndef FELIX_FAKE
            CI_FATAL("Timed-out waiting for semaphore\n");
#endif
            return ret;
        }
    }
    else
    {
        ret = SYS_SemTryDecrement(
            &(g_psCIDriver->aListQueue[pPipeline->userPipeline.ui8Context]));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Interrupted waiting for semaphore\n");
            return IMG_ERROR_INTERRUPTED;
        }
    }

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        enum KRN_CI_SHOT_eSTATUS lookingFor = CI_SHOT_AVAILABLE;
        sCell_T *pFound = NULL;
        KRN_CI_SHOT *pShot = NULL;

        pGivenBuffer = List_visitor(&(pPipeline->sList_available),
            &lookingFor, &FindFirstWithStatus);
        if (pGivenBuffer)
        {
            List_detach(pGivenBuffer);
            pShot = (KRN_CI_SHOT*)pGivenBuffer->object;

            if (pShot && 0 != pBuffId->encId)
            {
                pFound = List_visitor(&(pPipeline->sList_availableBuffers),
                    &(pBuffId->encId), &ListSearch_KRN_CI_Buffer);
                if (!pFound)
                {
                    CI_FATAL("given enc buffer %d was not found\n",
                        pBuffId->encId);
                    pShot = NULL;
                }
                else
                {
                    pShot->phwEncoderOutput = (KRN_CI_BUFFER*)pFound->object;
                    if (CI_BUFF_ENC == pShot->phwEncoderOutput->type)
                    {
                        List_detach(pFound);
                    }
                    else
                    {
                        CI_FATAL("given enc buffer %d is not of the correct "\
                            "type\n", pBuffId->encId);
                        pShot = NULL;
                    }
                }
            }
            if (pShot && 0 != pBuffId->dispId)
            {
                pFound = List_visitor(&(pPipeline->sList_availableBuffers),
                    &(pBuffId->dispId), &ListSearch_KRN_CI_Buffer);
                if (!pFound)
                {
                    CI_FATAL("given disp buffer %d was not found\n",
                        pBuffId->dispId);
                    pShot = NULL;
                }
                else
                {
                    pShot->phwDisplayOutput = (KRN_CI_BUFFER*)pFound->object;
                    if (CI_BUFF_DISP == pShot->phwDisplayOutput->type)
                    {
                        List_detach(pFound);
                    }
                    else
                    {
                        CI_FATAL("given disp buffer %d is not of the "\
                            "correct type\n", pBuffId->dispId);
                        pShot = NULL;
                    }
                }
            }
            if (pShot && 0 != pBuffId->idHDRExt)
            {
                pFound = List_visitor(&(pPipeline->sList_availableBuffers),
                    &(pBuffId->idHDRExt), &ListSearch_KRN_CI_Buffer);
                if (!pFound)
                {
                    CI_FATAL("given HDR Ext buffer %d was not found\n",
                        pBuffId->idHDRExt);
                    pShot = NULL;
                }
                else
                {
                    pShot->phwHDRExtOutput = (KRN_CI_BUFFER*)pFound->object;
                    if (CI_BUFF_HDR_WR == pShot->phwHDRExtOutput->type)
                    {
                        List_detach(pFound);
                    }
                    else
                    {
                        CI_FATAL("given HDR Extraction buffer %d is not of "\
                            "the correct type\n", pBuffId->idHDRExt);
                        pShot = NULL;
                    }
                }
            }
            if (pShot && 0 != pBuffId->idHDRIns)
            {
                pFound = List_visitor(&(pPipeline->sList_availableBuffers),
                    &(pBuffId->idHDRIns), &ListSearch_KRN_CI_Buffer);
                if (!pFound)
                {
                    CI_FATAL("given HDR Ins buffer %d was not found\n",
                        pBuffId->idHDRIns);
                    pShot = NULL;
                }
                else
                {
                    pShot->phwHDRInsertion = (KRN_CI_BUFFER*)pFound->object;
                    if (CI_BUFF_HDR_RD == pShot->phwHDRInsertion->type)
                    {
                        List_detach(pFound);
                    }
                    else
                    {
                        CI_FATAL("given HDR Insertion buffer %d is not of "\
                            "the correct type\n", pBuffId->idHDRIns);
                        pShot = NULL;
                    }
                }
            }
            if (pShot && 0 != pBuffId->idRaw2D)
            {
                pFound = List_visitor(&(pPipeline->sList_availableBuffers),
                    &(pBuffId->idRaw2D), &ListSearch_KRN_CI_Buffer);
                if (!pFound)
                {
                    CI_FATAL("given RAW2D Ext buffer %d was not found\n",
                        pBuffId->idRaw2D);
                    pShot = NULL;
                }
                else
                {
                    pShot->phwRaw2DExtOutput = (KRN_CI_BUFFER*)pFound->object;
                    if (CI_BUFF_RAW2D == pShot->phwRaw2DExtOutput->type)
                    {
                        List_detach(pFound);
                    }
                    else
                    {
                        CI_FATAL("given Raw2D Extraction buffer %d is not "\
                            "the correct type\n", pBuffId->idRaw2D);
                        pShot = NULL;
                    }
                }
            }

            if (!pShot) // error
            {
                /* just re-populate the pointer to liberate the
                 * reserved buffers */
                pShot = (KRN_CI_SHOT*)pGivenBuffer->object;

                if (pShot->phwEncoderOutput)
                {
                    List_pushBack(&(pPipeline->sList_availableBuffers),
                        &(pShot->phwEncoderOutput->sCell));
                    pShot->phwEncoderOutput = NULL;
                }
                if (pShot->phwDisplayOutput)
                {
                    List_pushBack(&(pPipeline->sList_availableBuffers),
                        &(pShot->phwDisplayOutput->sCell));
                    pShot->phwDisplayOutput = NULL;
                }
                if (pShot->phwHDRExtOutput)
                {
                    List_pushBack(&(pPipeline->sList_availableBuffers),
                        &(pShot->phwHDRExtOutput->sCell));
                    pShot->phwHDRExtOutput = NULL;
                }
                if (pShot->phwHDRInsertion)
                {
                    List_pushBack(&(pPipeline->sList_availableBuffers),
                        &(pShot->phwHDRInsertion->sCell));
                    pShot->phwHDRInsertion = NULL;
                }
                if (pShot->phwRaw2DExtOutput)
                {
                    List_pushBack(&(pPipeline->sList_availableBuffers),
                        &(pShot->phwRaw2DExtOutput->sCell));
                    pShot->phwRaw2DExtOutput = NULL;
                }

                List_pushBack(&(pPipeline->sList_available), pGivenBuffer);
                pShot = NULL;
                pGivenBuffer = NULL;
            }
        } // if pGivenBuffer != NULL
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    if (!pGivenBuffer)
    {
        CI_WARNING("no available buffer in the queue\n");
        SYS_SemIncrement(
            &(g_psCIDriver->aListQueue[pPipeline->userPipeline.ui8Context]));
        return IMG_ERROR_NOT_SUPPORTED;
    }

#ifdef FELIX_FAKE
    // space to help hw debug
    TALPDUMP_Comment(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context], "");
    CI_PDUMP_COMMENT(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context],
        "trigger");
    TALPDUMP_Comment(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context], "");
#endif

    /*
     * copy the configuration from capture to shot
     */

    pCurrent = (KRN_CI_SHOT*)pGivenBuffer->object;

    KRN_CI_ShotClean(pCurrent);  // clean the output device memory

    if (CI_UPD_NONE != pPipeline->eLastUpdated)
    {
        HW_CI_ShotUpdateLS(pCurrent);

#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
        KRN_CI_DebugRegisterOverride(pPipeline->userPipeline.ui8Context,
            &(pCurrent->hwLoadStructure));
        /* another device memory update is needed make sure device is up to
         * date in NDMA setup */
        Platform_MemUpdateDevice(&(pCurrent->hwLoadStructure));
#endif
        ui32ElemToUpdate = pPipeline->eLastUpdated;
        pPipeline->eLastUpdated = CI_UPD_NONE;
    }

    /** @ update current shot dimensions using current scaler output
     * configuration
     */

    /* update shot config to show if a buffer is tiled or not after triggering
     * HW because we need the normal sizes to configure it */
    if (pCurrent->phwDisplayOutput)
    {
        pCurrent->userShot.bDispTiled = pCurrent->phwDisplayOutput->bTiled;
    }
    if (pCurrent->phwEncoderOutput)
    {
        pCurrent->userShot.bEncTiled = pCurrent->phwEncoderOutput->bTiled;
    }
    if (pCurrent->phwHDRExtOutput)
    {
        pCurrent->userShot.bHDRExtTiled = pCurrent->phwHDRExtOutput->bTiled;
    }

    // configures the dynamic buffer sizes from the pipeline's configuration
    /* considered as the "minimum" strides computation */
    /* if tiled cannot changed stride as it is configured in the MMU */
    ret = KRN_CI_BufferFromConfig(&pCurrent->userShot, pPipeline, IMG_TRUE);
    /* if encOffsetC is 0 then driver should compute it
     * however the KRN_CI_BufferFromConfig() assumes the luma plane
     * has no offset and the original stride - if that changed the
     * chroma offset needs to be adjusted */

    if (pCurrent->phwEncoderOutput && !ret)
    {
        if (pBuffId->encStrideY)
        {
            if (pBuffId->encStrideY >= pCurrent->userShot.aEncYSize[0]
                && !pCurrent->userShot.bEncTiled)
            {
                pCurrent->userShot.aEncYSize[0] = pBuffId->encStrideY;
                pCurrent->userShot.aEncOffset[1] =
                    (pCurrent->userShot.aEncYSize[0]
                    * pCurrent->userShot.aEncYSize[1]);
            }
            else
            {
                CI_FATAL("Proposed encoder luma stride %u is too small "\
                    "- need at least %u Bytes or buffer is tiled (tiled %d)\n",
                    pBuffId->encStrideY, pCurrent->userShot.aEncYSize[0],
                    (int)pCurrent->userShot.bEncTiled);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        if (pBuffId->encStrideC)
        {
            if (pBuffId->encStrideC >= pCurrent->userShot.aEncCbCrSize[0]
                && !pCurrent->userShot.bEncTiled)
            {
                pCurrent->userShot.aEncCbCrSize[0] = pBuffId->encStrideC;
            }
            else
            {
                CI_FATAL("Proposed encoder chroma stride %u is too small "\
                    "- need at least %u Bytes or buffer is tiled (tiled %d)\n",
                    pBuffId->encStrideC, pCurrent->userShot.aEncCbCrSize[0],
                    (int)pCurrent->userShot.bEncTiled);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        if (pBuffId->encOffsetY)
        {
            // is always bigger because proposed luma offset is 0
            if (!pCurrent->userShot.bEncTiled)
            {
                pCurrent->userShot.aEncOffset[0] = pBuffId->encOffsetY;
                pCurrent->userShot.aEncOffset[1] +=
                    pCurrent->userShot.aEncOffset[0];
            }
            else
            {
                CI_FATAL("Cannot change offset if Encoder buffer is tiled\n");
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        if (pBuffId->encOffsetC)
        {
            if (pBuffId->encOffsetC >= pCurrent->userShot.aEncOffset[1])
            {
                pCurrent->userShot.aEncOffset[1] = pBuffId->encOffsetC;
            }
            else
            {
                CI_FATAL("Proposed encoder chroma offset %u is too small "\
                    "- need at least %u Bytes or buffer is tiled (tiled %d)\n",
                    pBuffId->encOffsetC,
                    pCurrent->userShot.aEncOffset[1],
                    (int)pCurrent->userShot.bEncTiled);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }

        /* should not happen but we check anyway because size verification
         * is based on the assumption that chroma is after luma */
        needed = pCurrent->userShot.aEncOffset[0]
            + (pCurrent->userShot.aEncYSize[0]
            * pCurrent->userShot.aEncYSize[1]);
        if (pCurrent->userShot.aEncOffset[1] < needed)
        {
            CI_FATAL("chroma offset %u is invalid need at least %u Bytes\n",
                pCurrent->userShot.aEncOffset[1], needed);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
        //pCurrent->userShot.bEncTiled = pCurrent->phwEncoderOutput->bTiled;
#ifdef INFOTM_ISP
        if (1 == pPipeline->userPipeline.uiTotalPipeline)
        {
#endif

            // WARNING: assumes chroma is after luma, before offset used to be:
            /*if ((pCurrent->userShot.aEncYSize[0]
                * pCurrent->userShot.aEncYSize[1])
                + (pCurrent->userShot.aEncCbCrSize[0]
                * pCurrent->userShot.aEncCbCrSize[1])
                > pCurrent->phwEncoderOutput->sMemory.uiAllocated)*/
            needed = pCurrent->userShot.aEncOffset[1]
                + (pCurrent->userShot.aEncCbCrSize[0]
                * pCurrent->userShot.aEncCbCrSize[1]);
            if (needed > pCurrent->phwEncoderOutput->sMemory.uiAllocated)
            {
                CI_FATAL("configured encoder sizes/offsets %u do not fit in "\
                    "allocated buffer of %u Bytes\n",
                    needed, pCurrent->phwEncoderOutput->sMemory.uiAllocated);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
#ifdef INFOTM_ISP
        }
#endif
    }
    if (pCurrent->phwDisplayOutput && !ret)
    {
        if (pBuffId->dispStride)
        {
            if (pBuffId->dispStride >= pCurrent->userShot.aDispSize[0]
                && !pCurrent->userShot.bDispTiled)
            {
                pCurrent->userShot.aDispSize[0] = pBuffId->dispStride;
            }
            else
            {
                CI_FATAL("Proposed display stride %u is too small " \
                    "- need at least %u Bytes or buffer is tiled (tiled %d)\n",
                    pBuffId->dispStride, pCurrent->userShot.aDispSize[0],
                    (int)pCurrent->userShot.bDispTiled);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        if (pBuffId->dispOffset)
        {
            if (!pCurrent->userShot.bDispTiled)
            {
                pCurrent->userShot.ui32DispOffset = pBuffId->dispOffset;
            }
            else
            {
                CI_FATAL("Cannot change offset if Display buffer is tiled\n");
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        //pCurrent->userShot.bDispTiled = pCurrent->phwDisplayOutput->bTiled;

        needed = pCurrent->userShot.ui32DispOffset
            + (pCurrent->userShot.aDispSize[0]
            * pCurrent->userShot.aDispSize[1]);
        if (needed > pCurrent->phwDisplayOutput->sMemory.uiAllocated)
        {
            CI_FATAL("configured display sizes/offset of %u do not fit in "\
                "allocated buffer of %u Bytes\n",
                needed, pCurrent->phwDisplayOutput->sMemory.uiAllocated);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
    }
    if (pCurrent->phwHDRExtOutput && !ret)
    {
        if (pBuffId->HDRExtStride)
        {
            if (pBuffId->HDRExtStride >= pCurrent->userShot.aHDRExtSize[0]
                && !pCurrent->userShot.bHDRExtTiled)
            {
                pCurrent->userShot.aHDRExtSize[0] = pBuffId->HDRExtStride;
            }
            else
            {
                CI_FATAL("Proposed HDRExt stride %u is too small "\
                    "- need at least %u Bytes or buffer is tiled (tiled %d)\n",
                    pBuffId->HDRExtStride, pCurrent->userShot.aHDRExtSize[0],
                    (int)pCurrent->userShot.bHDRExtTiled);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        if (pBuffId->HDRExtOffset)
        {
            if (!pCurrent->userShot.bHDRExtTiled)
            {
                pCurrent->userShot.ui32HDRExtOffset = pBuffId->HDRExtOffset;
            }
            else
            {
                CI_FATAL("Cannot change offset if HDR Extraction buffer is "\
                    "tiled\n");
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }

        //pCurrent->userShot.bHDRExtTiled = pCurrent->phwHDRExtOutput->bTiled;
        needed = pCurrent->userShot.ui32HDRExtOffset
            + (pCurrent->userShot.aHDRExtSize[0]
            * pCurrent->userShot.aHDRExtSize[1]);
        if (needed > pCurrent->phwHDRExtOutput->sMemory.uiAllocated)
        {
            CI_FATAL("configured HDRExt sizes/offset %u do not fit in "\
                "allocated buffer of %u Bytes\n",
                needed, pCurrent->phwHDRExtOutput->sMemory.uiAllocated);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
    }
    if (pCurrent->phwHDRInsertion && !ret)
    {
        if (pBuffId->HDRInsStride)
        {
            if (pBuffId->HDRInsStride >= pCurrent->userShot.aHDRInsSize[0])
            {
                pCurrent->userShot.aHDRInsSize[0] = pBuffId->HDRInsStride;
            }
            else
            {
                CI_FATAL("Proposed HDRIns stride %u is too small "\
                    "- need at least %u Bytes\n",
                    pBuffId->HDRInsStride, pCurrent->userShot.aHDRInsSize[0]);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        //if (pBuffId->HDRInsOffset)
        {
            // cannot be tiled so no check
            pCurrent->userShot.ui32HDRInsOffset = pBuffId->HDRInsOffset;
        }

        needed = pCurrent->userShot.ui32HDRInsOffset
            + (pCurrent->userShot.aHDRInsSize[0]
            * pCurrent->userShot.aHDRInsSize[1]);
        if (needed > pCurrent->phwHDRInsertion->sMemory.uiAllocated)
        {
            CI_FATAL("configured HDRIns sizes/offset %u do not fit in "\
                "allocated buffer %u Bytes\n",
                needed, pCurrent->phwHDRInsertion->sMemory.uiAllocated);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }

        // update the cache
        Platform_MemUpdateDevice(&(pCurrent->phwHDRInsertion->sMemory));
    }
    if (pCurrent->phwRaw2DExtOutput && !ret)
    {
        if (pBuffId->raw2DStride)
        {
            if (pBuffId->raw2DStride > pCurrent->userShot.aRaw2DSize[0])
            {
                pCurrent->userShot.aRaw2DSize[0] = pBuffId->raw2DStride;
            }
            else
            {
                CI_FATAL("Proposed Raw2D stride %u is too small "\
                    "- need at least %u Bytes\n",
                    pBuffId->raw2DStride, pCurrent->userShot.aRaw2DSize[0]);
                ret = IMG_ERROR_NOT_SUPPORTED;
            }
        }
        //if (pBuffId->raw2DOffset)
        {
            // cannot be tiled so no check
            pCurrent->userShot.ui32Raw2DOffset = pBuffId->raw2DOffset;
        }

        needed = pCurrent->userShot.ui32Raw2DOffset
            + (pCurrent->userShot.aRaw2DSize[0]
            * pCurrent->userShot.aRaw2DSize[1]);
        if (needed > pCurrent->phwRaw2DExtOutput->sMemory.uiAllocated)
        {
            CI_FATAL("configured Raw2D sizes/offset %u do not fit in "\
                "allocated buffer of %u Bytes\n",
                needed, pCurrent->phwRaw2DExtOutput->sMemory.uiAllocated);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
    }

    if (ret)
    {
        CI_FATAL("Failed to configure the buffer sizes!\n");

        SYS_LockAcquire(&(pPipeline->sListLock));
        {

            if (pCurrent->phwEncoderOutput)
            {
                List_pushBack(&(pPipeline->sList_availableBuffers),
                    &(pCurrent->phwEncoderOutput->sCell));
                pCurrent->phwEncoderOutput = NULL;
            }
            if (pCurrent->phwDisplayOutput)
            {
                List_pushBack(&(pPipeline->sList_availableBuffers),
                    &(pCurrent->phwDisplayOutput->sCell));
                pCurrent->phwDisplayOutput = NULL;
            }
            if (pCurrent->phwHDRExtOutput)
            {
                List_pushBack(&(pPipeline->sList_availableBuffers),
                    &(pCurrent->phwHDRExtOutput->sCell));
                pCurrent->phwHDRExtOutput = NULL;
            }
            if (pCurrent->phwHDRInsertion)
            {
                List_pushBack(&(pPipeline->sList_availableBuffers),
                    &(pCurrent->phwHDRInsertion->sCell));
                pCurrent->phwHDRInsertion = NULL;
            }
            if (pCurrent->phwRaw2DExtOutput)
            {
                List_pushBack(&(pPipeline->sList_availableBuffers),
                    &(pCurrent->phwRaw2DExtOutput->sCell));
                pCurrent->phwRaw2DExtOutput = NULL;
            }

            List_pushBack(&(pPipeline->sList_available), pGivenBuffer);

        }
        SYS_LockRelease(&(pPipeline->sListLock));

        SYS_SemIncrement(
            &(g_psCIDriver->aListQueue[pPipeline->userPipeline.ui8Context]));

        return IMG_ERROR_NOT_SUPPORTED;
    }

    // update some fields of the linked list
    HW_CI_ShotUpdateAddresses(pCurrent);

    /* configures the LL in HW now that we have the correct LL identifier
     * and output sizes */
    HW_CI_ShotLoadLinkedList(pCurrent, pPipeline->userPipeline.eOutputConfig,
        ui32ElemToUpdate);

    KRN_CI_ShotGetStatsConfig(pCurrent);  // copy the stats setup

#ifndef FELIX_FAKE
    /*
     * update dev memory if non UMA
     *
     * should not be done with fake driver as it would override dynamic
     * addresses in pdump
     */
    Platform_MemUpdateDevice(&(pCurrent->hwLinkedList));
#else
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "Would LDB HW linked list here but may create un-playable pdumps");
#endif

#ifdef FELIX_UNIT_TESTS
    /* we need to do that because there is no HW to do it when running unit
     * tests */
    KRN_CI_PipelineUpdateSave(pCurrent);
#endif

    // status becomes pending
    pCurrent->eStatus = CI_SHOT_PENDING;

#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pPipeline->sListLock));
#else
	SYS_SpinlockAcquire(&(pPipeline->sProcessingSpinlock));
#endif
    {
        List_pushBack(&(pPipeline->sList_pending), pGivenBuffer);

#ifdef CI_DEBUGFS
        g_ui32NCTXTriggered[pPipeline->userPipeline.ui8Context]++;
#endif

    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pPipeline->sListLock));
#else
	SYS_SpinlockRelease(&(pPipeline->sProcessingSpinlock));
#endif
    /*
     * update the HW - as semaphore was already acquired we should be allright
     */
    HW_CI_PipelineEnqueueShot(pPipeline, pCurrent);

#ifdef FELIX_FAKE
#ifdef CI_DEBUG_FCT
    /**
     * this is done in the lock because it only 1 capture per context is
     * possible
     * and only 1 file per context is used
     * may be dangerous for capNb variable but it's an int and should be
     * atomic as only set and ++ are done
     *
     * @note this is FELIX_FAKE only!
     */
    if (KRN_CI_DebugRegisterDumpEnabled())
    {
        SYS_LockAcquire(&(pPipeline->sListLock));
        {
            static IMG_UINT32 capNb = 0;
            static IMG_UINT32 ctxNb[CI_N_CONTEXT];
            char header[128];
            char file[32];
            IMG_UINT32 iCtx = 0;

            if (0 == capNb) // first time
            {
                for (iCtx = 0; iCtx < CI_N_CONTEXT; iCtx++)
                    ctxNb[iCtx] = 0;
            }

            iCtx = pPipeline->userPipeline.ui8Context;
            sprintf(header, "ctx %d - frame %d - total shot %d", iCtx,
                ctxNb[iCtx]++, capNb++);
            sprintf(file, "%s%d.txt", FELIX_DUMP_SHOT_FILE_PFRX, iCtx);
            /* may introduce some delay but should be used only in debug!
             * overwrite file if it's the first dump */
            KRN_CI_DebugDumpShot(pCurrent, file, header,
                FELIX_DUMP_REGISTER_FIELDS, capNb == 1 ? IMG_FALSE : IMG_TRUE);
        }
        SYS_LockRelease(&(pPipeline->sListLock));
    }
#endif

    // space to help HW debug
    TALPDUMP_Comment(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context], "");
    CI_PDUMP_COMMENT(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context],
        "trigger completed");
    TALPDUMP_Comment(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context], "");
#endif /* FELIX_FAKE */

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
////////////////////////////////////////
#if defined(INFOTM_ENABLE_ISP_DEBUG)
void dump_debug_reg(KRN_CI_PIPELINE *pPipeline)
{
	IMG_UINT32 context0_offset[] = 
	{
		0x0008,
		0x0014,
		0x0018,
		0x0020,
		0x0024,
		0x0028,
		0x002C,
		0x0030,
		0x0034,
		0x0038,
		0x0040,
		0x0050,
		0x00C0,
		0x00D0,
		0x00D4,
		0x00D8,
		0x00DC
	};

	IMG_UINT32 gaskets_offset[] = 
	{
		0x0010,
		0x0014,
		0x0060,
		0x0064,
		0x0068,
		0x0080,
		0x0100,
	};

	extern KRN_CI_DRIVER *g_psCIDriver;
	IMG_UINT32 ui32Val;
	int i;

	printk("=== context %d ===\n", pPipeline->userPipeline.ui8Context);
	for (i = 0; i < sizeof(context0_offset)/sizeof(context0_offset[0]); i ++)
	{
			TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], context0_offset[i], &ui32Val);
		printk("===hRegFelixContext[0].offset[0x%x] is 0x%x===\n", context0_offset[i], ui32Val);
	}

	CI_FATAL("===gaskets[0]===\n");
	for (i = 0; i < sizeof(gaskets_offset)/sizeof(gaskets_offset[0]); i ++)
	{
			TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[0], gaskets_offset[i], &ui32Val);
		printk("===hRegGaskets[0].offset[0x%x] is 0x%x===\n", gaskets_offset[i], ui32Val);
	}
}
#endif //INFOTM_ENABLE_ISP_DEBUG
////////////////////////////////////////
#endif //INFOTM_ISP
IMG_RESULT KRN_CI_PipelineAcquireShot(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bBlocking, KRN_CI_SHOT **ppShot)
{
    KRN_CI_SHOT *pShot = NULL;
    sCell_T *pBufferCell = NULL;
#if defined(W_TIME_PRINT)
    struct timespec t0, t1, t2, t3;
#endif

    IMG_ASSERT(pPipeline);

    CI_DEBUG("acquiring buffer (blocking=%d)\n", bBlocking);
#if defined(W_TIME_PRINT)
    getnstimeofday(&t0);
#endif

    if (!bBlocking)
    {
        if (SYS_SemTryDecrement(&(pPipeline->sProcessedSem)) != IMG_SUCCESS)
        {
            CI_DEBUG("the capture's Processed buffer list is empty\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
    }
    else
    {
        int ret = 0;
        CI_DEBUG("Waiting on the semaphore %d ms...", g_psCIDriver->uiSemWait);
        ret = SYS_SemTimedDecrement(
            &(pPipeline->sProcessedSem), g_psCIDriver->uiSemWait);
        if (IMG_SUCCESS != ret)
        {
            if (IMG_ERROR_TIMEOUT != ret)
            {
#ifndef FELIX_FAKE // otherwise polution of message
                CI_FATAL("waiting on the semaphore failed\n");
#endif
                return IMG_ERROR_INTERRUPTED;
            }

#ifdef INFOTM_ISP
			//KRN_CI_PipelineDumpListElements(pPipeline, IMG_TRUE);
			CI_FATAL("@@@ Isp acquire timeout: %d\n", g_psCIDriver->uiSemWait);

			///////////////////////////////
			#if defined(INFOTM_ENABLE_ISP_DEBUG)
			//dump_debug_reg(pPipeline);
			//KRN_CI_InfotmDebugDumpRegs(pPipeline->userPipeline.ui8Context, IMG_TRUE);
			#endif //INFOTM_ENABLE_ISP_DEBUG
			///////////////////////////////
#endif //INFOTM_ISP
            return ret;
        }
    }

#if defined(W_TIME_PRINT)
    getnstimeofday(&t1);
    W_TIME_PRINT(t0, t1, "get semaphore");
#endif
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pPipeline->sListLock));
#else
    SYS_SpinlockAcquire(&(pPipeline->sProcessingSpinlock));
#endif
    {
        pBufferCell = List_popFront(&(pPipeline->sList_processed));
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pPipeline->sListLock));
#else
    SYS_SpinlockRelease(&(pPipeline->sProcessingSpinlock));
#endif
    /* we accessed the semaphore, it should be the same than the list number
     * unless there was a failure somewhere... */
    if (!pBufferCell)
    {
        CI_FATAL("got a complete frame without a frame in the processed "\
            "list...\n");
        return IMG_ERROR_FATAL;
    }

    CI_PDUMP_COMMENT(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context],
        "start");

    pShot = (KRN_CI_SHOT*)pBufferCell->object;
    pShot->eStatus = CI_SHOT_SENT;


    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        List_pushBack(&(pPipeline->sList_sent), pBufferCell);
    }
    SYS_LockRelease(&(pPipeline->sListLock));

#if defined(W_TIME_PRINT)
    getnstimeofday(&t2);

    W_TIME_PRINT(t1, t2, "get buffer");
#endif

    // update the config used from value given by the HW
    Platform_MemUpdateHost(&(pShot->hwSave.sMemory));

    if (pShot->phwEncoderOutput)
    {
        Platform_MemUpdateHost(&(pShot->phwEncoderOutput->sMemory));
        //pShot->userShot.bEncTiled = pShot->phwEncoderOutput->bTiled;
        // see warning later
    }
    if (pShot->phwDisplayOutput)
    {
        Platform_MemUpdateHost(&(pShot->phwDisplayOutput->sMemory));
        //pShot->userShot.bDispTiled = pShot->phwDisplayOutput->bTiled;
        // see warning later
    }
    if (pShot->phwHDRExtOutput)
    {
        Platform_MemUpdateHost(&(pShot->phwHDRExtOutput->sMemory));
        //pShot->userShot.bHDRExtTiled = pShot->phwHDRExtOutput->bTiled;
        // see warning later
    }
    if (pShot->phwHDRInsertion)
    {
        Platform_MemUpdateHost(&(pShot->phwHDRInsertion->sMemory));
    }
    if (pShot->phwRaw2DExtOutput)
    {
        Platform_MemUpdateHost(&(pShot->phwRaw2DExtOutput->sMemory));
    }

    // a shot should always have a save structure
    IMG_ASSERT(pShot->hwSave.sMemory.uiAllocated > 0);

    // update DPF output map if present
    if (pShot->sDPFWrite.sMemory.uiAllocated > 0)
    {
        Platform_MemUpdateHost(&(pShot->sDPFWrite.sMemory));
    }

    // update ENS output if present
    if (pShot->sENSOutput.sMemory.uiAllocated > 0)
    {
        Platform_MemUpdateHost(&(pShot->sENSOutput.sMemory));
    }

#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
    KRN_CI_CRCCheck(pShot);
#endif

    /* warning: do not write to userShot because it is not sent back to user
     * space! */

    *ppShot = pShot;
    CI_PDUMP_COMMENT(REGFELIXCONTEXT[pPipeline->userPipeline.ui8Context],
        "done");

#if defined(W_TIME_PRINT)
    getnstimeofday(&t3);

    W_TIME_PRINT(t2, t3, "update host memory");
    W_TIME_PRINT(t0, t3, "=");
#endif

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineRelaseShot(KRN_CI_PIPELINE *pPipeline, int iShotId)
{
    IMG_RESULT ret;
    sCell_T *pFound = NULL;

    if (!pPipeline)
    {
        CI_FATAL("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        pFound = List_visitor(&(pPipeline->sList_sent),
            &iShotId, &List_SearchShot);

        if (pFound)
        {
            KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)pFound->object;

            ret = List_detach(pFound);
            // should never be something else: just found it in the list!
            IMG_ASSERT(ret == IMG_SUCCESS);

            ret = IMG_PipelineReleaseShot(pPipeline, pShot);
        }
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    if (!pFound)
    {
        CI_FATAL("could not find the buffer in the capture's sent buffer "\
            "list\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineStartCapture(KRN_CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct MMUDirectory *pDir = NULL;
    IMG_BOOL8 bDeshading = IMG_FALSE;
    int i;
    IMG_UINT8 ctx = 0;

    IMG_ASSERT(pPipeline);
    if (pPipeline->bStarted)
    {
        CI_WARNING("capture is already started.\n");
        return IMG_SUCCESS;
    }

    ctx = pPipeline->userPipeline.ui8Context;

    if (ctx >= g_psCIDriver->sHWInfo.config_ui8NContexts)
    {
        CI_FATAL("trying to use context %d - driver only supports context 0 "\
            "to %d\n", ctx,
            g_psCIDriver->sHWInfo.config_ui8NContexts - 1);
        return IMG_ERROR_FATAL;
    }

    if (pPipeline->userPipeline.sImagerInterface.ui8Imager
        >= g_psCIDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("trying to use imager %d - driver only supports imagers 0 "\
            "to %d\n", pPipeline->userPipeline.sImagerInterface.ui8Imager,
            g_psCIDriver->sHWInfo.config_ui8NImagers - 1);
        return IMG_ERROR_FATAL;
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pPipeline->sListLock));
#else
    SYS_SpinlockAcquire(&(pPipeline->sProcessingSpinlock));
#endif
    {
        if (pPipeline->sList_processed.ui32Elements > 0
            || pPipeline->sList_pending.ui32Elements > 0)
        {
            ret = IMG_ERROR_UNEXPECTED_STATE;
        }
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pPipeline->sListLock));
#else
    SYS_SpinlockRelease(&(pPipeline->sProcessingSpinlock));
#endif
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("There are still some buffers in processed/pending list\n");
        return ret;
    }

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        /* does not check pPipeline->sList_availableBuffers.ui32Elements
         * because it is legitimate to enable a capture
         * just to get statistics output */
        if (0 == pPipeline->sList_available.ui32Elements)
        {
            ret = IMG_ERROR_UNEXPECTED_STATE;
        }
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("The Shot list is empty! Call Add pool to create shots "\
            "before starting!\n");
        return ret;
    }

    CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " configure");

    /*
     * if data extraction is enabled we need to check that the current DE is
     * correct
     * if possible DE is updated
     */
    if (CI_INOUT_NONE != pPipeline->userPipeline.eDataExtraction)
    {
        ret = KRN_CI_DriverEnableDataExtraction(
            pPipeline->userPipeline.eDataExtraction);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Wanted Data Extraction point is not available\n");
            CI_PDUMP_COMMENT(
                REGFELIXCONTEXT[ctx],
                " chosen DE not available");
            return ret;
        }
    }

    ret = KRN_CI_DriverAcquireContext(pPipeline);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to acquire context %d\n", ctx);
        CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " failed acquiring HW");
        return ret;
    }
    // bStarted became IMG_TRUE if IMG_SUCCESS was returned

    // check deshading size fits context limitations
    bDeshading = KRN_CI_DriverCheckDeshading(pPipeline);
    if (!bDeshading)
    {
        CI_FATAL("the deshading grid is too big for the context limits\n");
        CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " LSH too big");

        if (KRN_CI_DriverReleaseContext(pPipeline) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to release context after failure!\n");
        }
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /* no need to apply the configuration ASAP because nothing has been
     * submitted yet */
    ret = KRN_CI_PipelineUpdate(pPipeline, IMG_TRUE, IMG_FALSE);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to set original configuration\n");
        if (KRN_CI_DriverReleaseContext(pPipeline) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to release context after failure!\n");
        }

        CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx],
            " failed updating configuration");
        return ret;
    }

    // when starting all modules need update
    pPipeline->eLastUpdated = CI_UPD_ALL;

    if (pPipeline->uiWantedDPFBuffer > 0)
    {
        ret = KRN_CI_DriverAcquireDPFBuffer(0, pPipeline->uiWantedDPFBuffer);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Not enough DPF internal buffer available\n");
            if (KRN_CI_DriverReleaseContext(pPipeline) != IMG_SUCCESS)
            {
                CI_FATAL("Failed to release context after failure!\n");
            }

            CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " failed DPF reserve");
            return ret;
        }
    }

    // we can now use the directory to map things
    pDir = g_psCIDriver->sMMU.apDirectory[ctx];

    if (pDir) // if we are using the MMU
    {
        SYS_LockAcquire(&(pPipeline->sListLock));
        {
            List_visitor(&(pPipeline->sList_available), pDir, &List_MapShots);
            List_visitor(&(pPipeline->sList_sent), pDir, &List_MapShots);
            List_visitor(&(pPipeline->sList_availableBuffers), pDir,
                &List_MapBuffer);
        }
        SYS_LockRelease(&(pPipeline->sListLock));

        SYS_MemMap(&(pPipeline->sDeshadingGrid), pDir, MMU_RO);
        SYS_MemMap(&(pPipeline->sDPFReadMap), pDir, MMU_RO);

        if (0 != pPipeline->uiTiledStride)
        {
            KRN_CI_MMUSetTiling(&(g_psCIDriver->sMMU), IMG_TRUE,
                pPipeline->uiTiledStride, ctx);
        }

        /* BRN48951 recomends SW to flush the cache for every buffer mapping
         * but we do it once all buffers are mapped */
        KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU), ctx, IMG_TRUE);
    }

    /* reset the global semaphore of available number of elements in the LL
     * in case it got de-synchronised */
    do
    {
        ret = SYS_SemTryDecrement(&(g_psCIDriver->aListQueue[ctx]));
    } while (IMG_SUCCESS == ret);

    for (i = 0; i < g_psCIDriver->sHWInfo.context_aPendingQueue[ctx]; i++)
    {
        SYS_SemIncrement(&(g_psCIDriver->aListQueue[ctx]));
    }

#ifdef INFOTM_ENABLE_ISP_DEBUG
    printk("@@@ PipeStart[%d]: \n",	pPipeline->userPipeline.ui8Context);
#endif //INFOTM_ENABLE_ISP_DEBUG

    // this enables the interrupts for this context
    ret = HW_CI_PipelineRegStart(pPipeline);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to setup the HW\n");
        if (pPipeline->uiWantedDPFBuffer > 0)
        {
            // release the acquired internal buffer
            KRN_CI_DriverAcquireDPFBuffer(pPipeline->uiWantedDPFBuffer, 0);
        }
        if (KRN_CI_DriverReleaseContext(pPipeline) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to release context after failure!\n");
        }

        CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " setup HW failed");
        return ret;
    }
#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
    else
    {
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
#else
		SYS_SpinlockAcquire(&(g_psCIDriver->sActiveContextLock));
#endif
        /**
         * this is done in the lock because it only 1 capture per context is
         * possible
         * and only 1 file per context is used
         */
        if (KRN_CI_DebugRegisterDumpEnabled())
        {
            IMG_HANDLE hFile = NULL;
            IMG_BOOL bCompress = FELIX_DUMP_COMPRESSED_REGISTERS;
            IMG_RESULT iRet = IMG_SUCCESS;
            IMG_CHAR pszFilename[64];
            // so first time file is reset - then info are added
            static IMG_BOOL8 bAppendFile = IMG_FALSE;

            sprintf(pszFilename, "context%d.txt", ctx);

            if (!IMG_FILEIO_UsesZLib() && bCompress)
            {
                CI_WARNING("FileIO library does not have zlib - cannot "\
                    "write compressed registers\n");
                bCompress = IMG_FALSE;
            }

            iRet = IMG_FILEIO_OpenFile(pszFilename, bAppendFile ? "a" : "w",
                &hFile, bCompress);
            if (IMG_SUCCESS == iRet)
            {
                bAppendFile = IMG_TRUE;
                KRN_CI_DebugDumpContext(hFile, "dumping after configuration\n",
                    ctx,
                    FELIX_DUMP_REGISTER_FIELDS);
                IMG_FILEIO_CloseFile(hFile);
            }
            else
            {
                CI_FATAL("failed to open '%s'\n", pszFilename);
            }
        }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#else
		SYS_SpinlockRelease(&(g_psCIDriver->sActiveContextLock));
#endif
    }
#endif /* CI_DEBUG_FCT && FELIX_FAKE */

    CI_PDUMP_COMMENT(REGFELIXCONTEXT[ctx], " configure done");
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineStopCapture(KRN_CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret;

    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pPipeline->userPipeline.ui8Context < CI_N_CONTEXT);

#ifdef INFOTM_ENABLE_ISP_DEBUG
    printk("@@@ PipeStop [%d]: \n",	pPipeline->userPipeline.ui8Context);
#endif //INFOTM_ENABLE_ISP_DEBUG

    if (!pPipeline->bStarted)
    {
        CI_FATAL("capture is already stopped.\n");
        return IMG_SUCCESS;
    }

    HW_CI_PipelineRegStop(pPipeline, IMG_TRUE);

    /* do not need to wait for the Link list to be cleared
     * we could also reset the associated the semaphore but it is done at
     * starting time */

    // acquire the slow lock first
    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        sCell_T *pWaiting = NULL;
        KRN_CI_SHOT *pShot = NULL;

        if (g_psCIDriver->sMMU.uiAllocAtom > 0) // using the MMU
        {
            /* unmap all available shots from the MMU before adding the
             * pending and processed one to the list */
            List_visitor(&(pPipeline->sList_available), NULL,
                &List_UnmapShots);

            /* unmap all sent to user buffers from MMU (they don't need to be
             * there anymore and it does not affect the memory itself) */
            List_visitor(&(pPipeline->sList_sent), NULL, &List_UnmapShots);
        }

        /* assumes that the KRN_CI_DriverReleaseContext() stopped the
         * interrupts for that context! */

        /**
         * @warning As interrupts are stopped before cleaning the pending
         * list it is not needed to lock the spinlock
         */
        pWaiting = List_popFront(&(pPipeline->sList_pending));
        while (pWaiting)
        {
            pShot = (KRN_CI_SHOT*)pWaiting->object;

            if (g_psCIDriver->sMMU.uiAllocAtom > 0)
            {
                KRN_CI_ShotUnmapMemory(pShot);
            }

            IMG_PipelineReleaseShot(pPipeline, pShot);
            pWaiting = List_popFront(&(pPipeline->sList_pending));
        }

        pWaiting = List_popFront(&(pPipeline->sList_processed));
        while (pWaiting)
        {
            /* decrement the associated semaphore so that the count keeps in
             * sync */
            ret = SYS_SemTryDecrement(&(pPipeline->sProcessedSem));
            if (IMG_SUCCESS != ret) // unlikely
            {
                CI_WARNING("sProcessedSem was not in sync with "\
                    "sList_processed!\n");
            }
            pShot = (KRN_CI_SHOT*)pWaiting->object;

            if (g_psCIDriver->sMMU.uiAllocAtom > 0)
            {
                KRN_CI_ShotUnmapMemory(pShot);
            }

            IMG_PipelineReleaseShot(pPipeline, pShot);
            pWaiting = List_popFront(&(pPipeline->sList_processed));
        }
        // unmap all imported buffers
        List_visitor(&(pPipeline->sList_availableBuffers), NULL,
            &List_UnmapBuffer);
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    if (g_psCIDriver->sMMU.uiAllocAtom > 0) // using the MMU
    {
        SYS_MemUnmap(&(pPipeline->sDPFReadMap));
        SYS_MemUnmap(&(pPipeline->sDeshadingGrid));

        /* BRN48951 recomends SW to flush the cache for every buffer mapping
         * but we do it once all buffers are unmapped */
        KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU),
            pPipeline->userPipeline.ui8Context, IMG_TRUE);
    }

    if (pPipeline->uiWantedDPFBuffer > 0)
    {
        KRN_CI_DriverAcquireDPFBuffer(pPipeline->uiWantedDPFBuffer, 0);
    }

    ret = KRN_CI_DriverReleaseContext(pPipeline);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to release context %d\n",
            pPipeline->userPipeline.ui8Context);
        return ret;
    }
    //pPipeline->bStarted became IMG_FALSE;
    return IMG_SUCCESS;
}


#ifdef INFOTM_ENABLE_ISP_DEBUG

static FELIX_INFOTM_DEBUG_FIELDS // felix_context0_fields
static FELIX_GAS_MIPI_FIELDS // felix_gas_mipi_fields
static FELIX_GAS_PARA_FIELDS //felix_gas_para_fields

IMG_RESULT KRN_CI_InfotmDebugRegisterPrint(IMG_CONST FieldDefnListEl *pField, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct)
{
	char buffer[512];
	IMG_UINT32 ui32RegVal;

	if ( pField == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (bIsMemstruct==IMG_FALSE)
	{
		TALREG_ReadWord32(hTalRegBankHandle, pField->ui32RegOffset, &ui32RegVal);
	}
	else
	{
		TALMEM_ReadWord32(hTalRegBankHandle, pField->ui32RegOffset, &ui32RegVal);
	}

	if ( bDumpFields == IMG_FALSE )
	{
		sprintf(buffer, "[offs 0x%08X] %-43s = 0x%08X\n", pField->ui32RegOffset, pField->szRegName, ui32RegVal);
	}
	else
	{
		IMG_UINT32 ui32FieldVal = 0;
		// index starts at 0 -> needs +1 for the number of bits
		// then create the mask and left shift it to its right position in the word
		IMG_UINT32 ui32Mask = ((1<<(pField->ui32FieldEnd-pField->ui32FieldStart +1))-1)<<pField->ui32FieldStart;

		ui32FieldVal = (ui32RegVal&ui32Mask)>>pField->ui32FieldStart;

		sprintf(buffer, "[mask 0x%08X]   .%-40s = %u (0x%X)\n", ui32Mask, pField->szFieldName, ui32FieldVal, ui32FieldVal);
	}

	printk("%s", buffer);

	return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_InfotmDebugRegistersDump(FieldDefnListEl *aSortedBlock, IMG_SIZE nElem, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct)
{
	IMG_UINT32 i;
	FieldDefnListEl *pPrevious = NULL, *pCurrent = NULL;

	if ( aSortedBlock == NULL || nElem == 0 )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	for ( i = 0 ; i < nElem ; i++ )
	{
		pCurrent = &(aSortedBlock[i]);

		// 1st register or same register than previously written one
		// or
		// if same register than previously only dump if bDumpFields is IMG_TRUE
		if ( pPrevious == NULL ||
				pCurrent->ui32RegOffset != pPrevious->ui32RegOffset )
		{
			if ( KRN_CI_InfotmDebugRegisterPrint(pCurrent, hTalRegBankHandle, IMG_FALSE, bIsMemstruct) != IMG_SUCCESS )
			{
				return IMG_ERROR_FATAL;
			}
			if ( bDumpFields == IMG_TRUE )
			{
				// only writes the field
				if ( KRN_CI_InfotmDebugRegisterPrint(pCurrent, hTalRegBankHandle, IMG_TRUE, bIsMemstruct) != IMG_SUCCESS )
				{
					return IMG_ERROR_FATAL;
				}
			}
			pPrevious = pCurrent;
		}
		else if ( bDumpFields == IMG_TRUE )
		{
			// only writes the field
			if ( KRN_CI_InfotmDebugRegisterPrint(pCurrent, hTalRegBankHandle, IMG_TRUE, bIsMemstruct) != IMG_SUCCESS )
			{
				return IMG_ERROR_FATAL;
			}
			pPrevious = pCurrent;
		}
	}

	return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_InfotmDebugDumpRegs(IMG_UINT32 ui32ContextNb, IMG_BOOL bDumpFields)
{
	static IMG_SIZE nElem = 0;


	if (ui32ContextNb >= CI_N_CONTEXT)
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( g_psCIDriver == NULL )
	{
		return IMG_ERROR_UNEXPECTED_STATE;
	}

	printk("===== Context %d =====\n", ui32ContextNb);
	nElem = sizeof(felix_infotm_debug_fields)/sizeof(FieldDefnListEl);
	if ( KRN_CI_InfotmDebugRegistersDump(felix_infotm_debug_fields, nElem, g_psCIDriver->sTalHandles.hRegFelixContext[ui32ContextNb], bDumpFields, IMG_FALSE) != IMG_SUCCESS )
	{
		CI_FATAL("failed to dump Felix Context %d registers\n", ui32ContextNb);
		return IMG_ERROR_FATAL;
	}

	if (ui32ContextNb == 0)
	{
		printk("===== Gasket %d (MIPI) =====\n", 0);
		nElem = sizeof(felix_gas_mipi_fields)/sizeof(FieldDefnListEl);
		if ( KRN_CI_InfotmDebugRegistersDump(felix_gas_mipi_fields, nElem, g_psCIDriver->sTalHandles.hRegGaskets[0], bDumpFields, IMG_FALSE) != IMG_SUCCESS )
		{
			CI_FATAL("failed to dump Felix Gasket %d (MIPI) registers\n", 0);
			return IMG_ERROR_FATAL;
		}
	}
	else
	{
		printk("===== Gasket %d (DVP) =====\n", 2);
		nElem = sizeof(felix_gas_para_fields)/sizeof(FieldDefnListEl);
		if ( KRN_CI_InfotmDebugRegistersDump(felix_gas_para_fields, nElem, g_psCIDriver->sTalHandles.hRegGaskets[2], bDumpFields, IMG_FALSE) != IMG_SUCCESS )
		{
			CI_FATAL("failed to dump Felix Gasket %d (DVP) registers\n", 2);
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}
#endif //INFOTM_ENABLE_ISP_DEBUG
