/**
 ******************************************************************************
 @file dg_krncamera.c

 @brief kernel-side implementation for the Data Generator

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

 *****************************************************************************/
#include "dg_kernel/dg_camera.h"
#include "dg_kernel/dg_debug.h"
#include "ci_kernel/ci_kernel.h"  // CI_FATAL

#include "dg_kernel/dg_ioctl.h"

#include <tal.h>
#include <reg_io2.h>

#include <registers/ext_data_generator.h>
// #include <hw_struct/ext_data_generation.h>  // dg linked list not used

#include <mmulib/heap.h>

#define HW_DG_OFF(reg) DATA_GENERATION_POINTERS_GROUP_ ## reg ## _OFFSET

#ifdef FELIX_UNIT_TESTS

extern IMG_BOOL8 gbUseTalNULL;

#define FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value) \
    (((IMG_UINT32) (ui32value) << (group##_##reg##_##field##_SHIFT)) \
    & (group##_##reg##_##field##_MASK))

#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value) \
    if ( gbUseTalNULL == IMG_TRUE ) \
TALREG_WriteWord32(TalHandle, group ## _ ## reg ## _OFFSET, \
    FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value));

#else
#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value)
#endif

IMG_BOOL8 List_SearchFrame(void *listElem, void *searched)
{
    unsigned int *pId = (unsigned int*)searched;
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listElem;

    if (pFrame->identifier == *pId)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_MapFrame(void *listelem, void *directory)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listelem;
    struct MMUDirectory *pDir = (struct MMUDirectory*)directory;

    if (KRN_DG_FrameMap(pFrame, pDir) != IMG_SUCCESS)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_UnmapFrame(void *listelem, void *unused)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listelem;

    (void)unused;
    KRN_DG_FrameUnmap(pFrame);
    return IMG_TRUE;
}

/*
 * camera
 */

KRN_DG_CAMERA* KRN_DG_CameraCreate(void)
{
    IMG_RESULT ret;
    IMG_UINT32 i;
    KRN_DG_CAMERA *pNew = NULL;

    pNew = (KRN_DG_CAMERA*)IMG_CALLOC(1, sizeof(KRN_DG_CAMERA));
    if (!pNew)
    {
        CI_FATAL("Failed to allocate KRN_DG_CAMERA %luB\n",
            (unsigned)sizeof(KRN_DG_CAMERA));
        return NULL;
    }

    pNew->sCell.object = pNew;

    ret = SYS_LockInit(&(pNew->sLock));
    if (ret)  // unlikely
    {
        goto cam_lock_failed;
    }

    ret = SYS_SemInit(&(pNew->sSemProcessed), 0);
    if (ret)  // unlikely
    {
        goto cam_sem_failed;
    }

    ret = List_init(&(pNew->sList_available));
    if (ret)
    {
        goto cam_avail_failed;
    }

    ret = List_init(&(pNew->sList_converting));
    if (ret)
    {
        goto cam_convert_failed;
    }

    ret = List_init(&(pNew->sList_pending));
    if (ret)
    {
        goto cam_pending_failed;
    }

    ret = List_init(&(pNew->sList_processed));
    if (ret)
    {
        goto cam_processed_failed;
    }

    for (i = 0; g_pDGDriver->sMMU.apDirectory[0] != NULL && i < DG_N_HEAPS;
        i++)
    {
        pNew->apHeap[i] = IMGMMU_HeapCreate(
            g_pDGDriver->sMMU.aHeapStart[i],
            g_pDGDriver->sMMU.uiAllocAtom,
            g_pDGDriver->sMMU.aHeapSize[i], &ret);
        if (ret)
        {
            while (i > 0)
            {
                IMGMMU_HeapDestroy(pNew->apHeap[i - 1]);
                i--;
            }
            goto cam_processed_failed;
        }
    }

    return pNew;

cam_processed_failed:
    // processed list is empty
cam_pending_failed:
    // converting list is empty
cam_convert_failed :
    // available list is empty
cam_avail_failed :
    SYS_SemDestroy(&(pNew->sSemProcessed));
cam_sem_failed:
    SYS_LockDestroy(&(pNew->sLock));
cam_lock_failed:
    IMG_FREE(pNew);
    return NULL;
}

IMG_RESULT KRN_DG_CameraShoot(KRN_DG_CAMERA *pCamera,
    struct DG_FRAME_PARAM *pParam)
{
    KRN_DG_FRAME *pFrame = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pCamera);
    IMG_ASSERT(pParam);

    if (!pCamera->bStarted && pParam->bOption)
    {
        CI_FATAL("Cannot trigger on a stopped camera!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    SYS_LockAcquire(&(pCamera->sLock));
    {
        pFound = List_visitor(&(pCamera->sList_converting),
            (void*)&(pParam->frameId), &List_SearchFrame);

        if (pFound)
        {
            pFrame = container_of(pFound, KRN_DG_FRAME, sListCell);
        }
    }
    SYS_LockRelease(&(pCamera->sLock));

    if (!pFrame)
    {
        CI_FATAL("could not find frame %d\n", pParam->frameId);
        return IMG_ERROR_FATAL;
    }

#ifdef DEBUG_FIRST_LINE
    {
        int *pMem = (int*)Platform_MemGetMemory(&(pFrame->sBuffer));
        IMG_SIZE m = pFrame->sBuffer.uiAllocated/sizeof(int);
        CI_DEBUG("DG image first bytes: 0x%8x 0x%8x 0x%8x 0x%8x\n",
            pMem[0], pMem[1], pMem[2], pMem[3]);
        CI_DEBUG("DG image last bytes: 0x%8x 0x%8x 0x%8x 0x%8x\n",
            pMem[m-4], pMem[m-3], pMem[m-2], pMem[m-1]);
}
#endif

    if (pParam->bOption)
    {
        if (FELIX_MIN_H_BLANKING > pParam->ui32HoriBlanking
            || FELIX_MIN_V_BLANKING > pParam->ui32VertBlanking)
        {
            CI_FATAL("frame proposed blanking of %ux%u is too small, "\
                "min is %ux%u\n",
                pParam->ui32HoriBlanking, pParam->ui32VertBlanking,
                FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        // copy the additional information
        pFrame->ui16Width = pParam->ui16Width;
        pFrame->ui16Height = pParam->ui16Height;
        pFrame->uiStride = pParam->ui32Stride;
        if (pParam->ui32HoriBlanking < DG_H_BLANKING_SUB)
        {
            pFrame->ui32HoriBlanking = 0;
        }
        else
        {
            pFrame->ui32HoriBlanking =
                pParam->ui32HoriBlanking - DG_H_BLANKING_SUB;
        }
        pFrame->ui32VertBlanking = pParam->ui32VertBlanking;

        /* this could be "long" so it's better outside the lock */
        Platform_MemUpdateDevice(&(pFrame->sBuffer));

        SYS_LockAcquire(&(pCamera->sLock));
        {
            List_detach(pFound);  // detach from sList_converting
            List_pushBack(&(pCamera->sList_pending), pFound);

            // 1st element need to be submitted
            if (1 == pCamera->sList_pending.ui32Elements)
            {
                IMG_RESULT ret = HW_DG_CameraSetNextFrame(pCamera, pFrame);
                IMG_ASSERT(IMG_SUCCESS == ret);
            }
            else
            {
                CI_DEBUG("SW enqueue frame %u in DG %u\n",
                    pFrame->identifier,
                    pCamera->publicCamera.ui8Gasket);
            }
        }
        SYS_LockRelease(&(pCamera->sLock));
#ifdef CI_DEBUGFS
        g_ui32NDGSubmitted[pCamera->publicCamera.ui8Gasket]++;
#endif
    }
    else
    {
        SYS_LockAcquire(&(pCamera->sLock));
        {
            List_detach(pFound);  // detach from sList_converting
            List_pushBack(&(pCamera->sList_available), pFound);
        }
        SYS_LockRelease(&(pCamera->sLock));
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_CameraWaitProcessed(KRN_DG_CAMERA *pCamera,
    IMG_BOOL8 bBlocking)
{
    sCell_T *pFound = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pCamera);

    if (!pCamera->bStarted)
    {
        CI_FATAL("Cannot wait for processed frame on a stopped camera!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (!bBlocking)
    {
        ret = SYS_SemTryDecrement(&(pCamera->sSemProcessed));
        if (IMG_SUCCESS != ret)
        {
            CI_DEBUG("the datagen's Processed buffer list is empty\n");
            return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
    }
    else
    {
        CI_DEBUG("Waiting on the semaphore %d ms...", g_psCIDriver->uiSemWait);
        ret = SYS_SemTimedDecrement(
            &(pCamera->sSemProcessed), g_psCIDriver->uiSemWait);
        if (IMG_SUCCESS != ret)
        {
            if (IMG_ERROR_TIMEOUT != ret)
            {
#ifndef FELIX_FAKE  // otherwise polution of message
                CI_FATAL("waiting on the semaphore failed\n");
#endif
                return IMG_ERROR_INTERRUPTED;
            }
            CI_DEBUG("Waiting on the semaphore failed (TIMEOUT)!");
            return ret;
        }
    }

    pFound = List_popFront(&(pCamera->sList_processed));
    IMG_ASSERT(NULL != pFound);  // otherwise semaphore is out of sync!
    List_pushBack(&(pCamera->sList_available), pFound);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_CameraVerify(const DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(pCamera);

    switch (pCamera->eFormat)
    {
    case CI_DGFMT_MIPI:
    case CI_DGFMT_MIPI_LF:
        if (10 != pCamera->ui8FormatBitdepth
            && 12 != pCamera->ui8FormatBitdepth
            && 14 != pCamera->ui8FormatBitdepth)
        {
            CI_FATAL("unsupported MIPI bitdepth %d\n",
                (int)pCamera->ui8FormatBitdepth);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
        break;
    case CI_DGFMT_PARALLEL:
        if (10 != pCamera->ui8FormatBitdepth
            && 12 != pCamera->ui8FormatBitdepth)
        {
            CI_FATAL("unsupported Parallel bitdepth %d\n",
                (int)pCamera->ui8FormatBitdepth);
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
        break;
    default:
        CI_FATAL("unsupported format %d", (int)pCamera->eFormat);
        ret = IMG_ERROR_NOT_SUPPORTED;
    }

    if (pCamera->ui8Gasket >= g_pDGDriver->sHWInfo.config_ui8NDatagen)
    {
        CI_FATAL("chosen gasket %d does not have an associated datagen\n",
            (int)pCamera->ui8Gasket);
        ret = IMG_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

IMG_RESULT KRN_DG_CameraStart(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct MMUDirectory *pDir = NULL;
    IMG_HANDLE regHandle = NULL;

    IMG_ASSERT(pCamera);
    // there should be no left over frames!
    IMG_ASSERT(pCamera->sList_converting.ui32Elements == 0);
    IMG_ASSERT(pCamera->sList_pending.ui32Elements == 0);

    ret = KRN_DG_CameraVerify(&(pCamera->publicCamera));
    if (ret)
    {
        CI_FATAL("Failed to configure camera\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    regHandle = g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8Gasket];

    ret = KRN_DG_DriverAcquireContext(pCamera);
    if (ret)
    {
        CI_FATAL("Failed to acquire HW context!\n");
        return ret;
    }

    CI_PDUMP_COMMENT(regHandle, "start");

    pDir = g_pDGDriver->sMMU.apDirectory[pCamera->publicCamera.ui8Gasket];
    if (pDir)
    {
        // no interrupt should occure: DG HW is not started yet
        // SYS_LockAcquire(&(pCamera->sPendingLock));
        {
            /* should not disable interrupts when mapping - returns NULL
             * if all cells were visited */
            if (List_visitor(&(pCamera->sList_available), pDir,
                &List_MapFrame) != NULL)
            {
                return IMG_ERROR_FATAL;  // failed to map
            }
        }
        // SYS_LockRelease(&(pCamera->sPendingLock));

        KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU),
            pCamera->publicCamera.ui8Gasket, IMG_TRUE);
    }

    ret = HW_DG_CameraConfigure(pCamera);
    if (ret)
    {
        CI_FATAL("Failed to configure the HW!\n");
        KRN_DG_DriverReleaseContext(pCamera);
    }
    else
    {
        pCamera->bStarted = IMG_TRUE;
    }

    CI_PDUMP_COMMENT(regHandle, "done");

    return ret;
}

IMG_RESULT KRN_DG_CameraStop(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
#ifdef FELIX_FAKE
    IMG_HANDLE dgReg =
        g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8Gasket];

    CI_PDUMP_COMMENT(dgReg, "start");
#endif

    SYS_LockAcquire(&(pCamera->sLock));
    {
        sCell_T *pWaiting = List_popFront(&(pCamera->sList_converting));
        while (pWaiting)
        {
            List_pushBack(&(pCamera->sList_available), pWaiting);

            pWaiting = List_popFront(&(pCamera->sList_converting));
        }

        /* if the stop was forced the list may be empty when receiving
         * an interrupt */
        pWaiting = List_popFront(&(pCamera->sList_pending));
        while (pWaiting)
        {
            List_pushBack(&(pCamera->sList_available), pWaiting);

            pWaiting = List_popFront(&(pCamera->sList_pending));
        }

        // all processed frames become available again
        pWaiting = List_popFront(&(pCamera->sList_processed));
        while (pWaiting)
        {
            ret = SYS_SemDecrement(&(pCamera->sSemProcessed));
            /* unlikely so could be assert but we're in a lock */
            if (IMG_SUCCESS != ret)
            {
                CI_WARNING("processed semaphore out of sync with list\n");
            }
            List_pushBack(&(pCamera->sList_available), pWaiting);

            pWaiting = List_popFront(&(pCamera->sList_processed));
        }

    }
    SYS_LockRelease(&(pCamera->sLock));

    ret = HW_DG_CameraStop(pCamera);
    if (ret)
    {
        CI_FATAL("Failed to stop the HW!\n");
        return ret;
    }

    ret = KRN_DG_DriverReleaseContext(pCamera);
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to release the DG Camera\n");
        return ret;
    }

    /* unmap from MMU before adding the flushed buffers - do not disable
     * interrupts for that */
    List_visitor(&(pCamera->sList_available), NULL, &List_UnmapFrame);

    if (g_pDGDriver->sMMU.uiAllocAtom > 0)
    {
        KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU),
            pCamera->publicCamera.ui8Gasket, IMG_TRUE);
    }

    pCamera->bStarted = IMG_FALSE;

#ifdef FELIX_FAKE
    CI_PDUMP_COMMENT(dgReg, "done");
#endif

    return ret;
}

IMG_RESULT KRN_DG_CameraDestroy(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret;
    IMG_UINT32 i;

    IMG_ASSERT(pCamera);

    /* if g_pDGDriver is NULL we are cleaning the left-overs so it does
     * not matter much does it? */
    if (pCamera->bStarted == IMG_TRUE && g_pDGDriver != NULL)
    {
        ret = KRN_DG_CameraStop(pCamera);
        if (ret)
        {
            CI_FATAL("Failed to stop DG camera\n");
            return ret;
        }
    }

    /* no real need for locking the capture is stopped no other object
     * can access it */
    // SYS_LockAcquire(&(pCamera->sPendingLock));
    {
        /* these lists can't use the clear() function because the cells
         * should not be destroyed! */
        List_visitor(&(pCamera->sList_available), NULL, &List_DestroyFrames);

        List_visitor(&(pCamera->sList_converting), NULL, &List_DestroyFrames);

        List_visitor(&(pCamera->sList_pending), NULL, &List_DestroyFrames);

        List_visitor(&(pCamera->sList_processed), NULL, &List_DestroyFrames);
    }
    // SYS_LockRelease(&(pCamera->sPendingLock));

    SYS_LockDestroy(&(pCamera->sLock));
    SYS_SemDestroy(&(pCamera->sSemProcessed));

    for (i = 0; i < DG_N_HEAPS; i++)
    {
        if (pCamera->apHeap[i] != NULL)
        {
            IMGMMU_HeapDestroy(pCamera->apHeap[i]);
        }
    }

    IMG_FREE(pCamera);

    return IMG_SUCCESS;
}

/*
 * frame
 */

IMG_RESULT KRN_DG_FrameCreate(KRN_DG_FRAME **ppFrame, IMG_UINT32 ui32Size,
    KRN_DG_CAMERA *pCam)
{
    IMG_RESULT ret;

    IMG_ASSERT(ppFrame);
    IMG_ASSERT(!*ppFrame);
    IMG_ASSERT(pCam);

    if (0 == ui32Size)
    {
        CI_FATAL("The allocation size is 0B!\n");
        return IMG_ERROR_FATAL;
    }

    *ppFrame = (KRN_DG_FRAME*)IMG_CALLOC(1, sizeof(KRN_DG_FRAME));

    if (!*ppFrame)
    {
        CI_FATAL("Failed to allocate KRN_DG_FRAME %uB\n",
            sizeof(KRN_DG_FRAME));
        return IMG_ERROR_MALLOC_FAILED;
    }

    (*ppFrame)->sListCell.object = *ppFrame;
    (*ppFrame)->pParent = pCam;
    // need to be aligned to memory per line
    (*ppFrame)->uiStride = 0;  // not known yet
//        (pCam->publicCamera.uiStride
//        + SYSMEM_ALIGNMENT - 1)
//        / SYSMEM_ALIGNMENT * SYSMEM_ALIGNMENT;

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " - start");

    ret = SYS_MemAlloc(&((*ppFrame)->sBuffer), ui32Size,
        pCam->apHeap[DG_IMAGE_HEAP], 0);
    if (ret)
    {
        CI_FATAL("failed to allocate frame buffer\n");
        IMG_FREE(*ppFrame);
        *ppFrame = NULL;
        return ret;
    }

#if defined(DEBUG_MEMSET_IMG_BUFFERS)
    {
        void *ptr = Platform_MemGetMemory(&((*ppFrame)->sBuffer));
        CI_DEBUG("memset DG buffer\n");

        IMG_MEMSET(ptr, 51, allocSize);  // 0x33

        Platform_MemUpdateDevice(&((*ppFrame)->sBuffer));
}
#endif

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " - done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameMap(KRN_DG_FRAME *pFrame, struct MMUDirectory *pDir)
{
    IMG_ASSERT(pFrame);

    if (pDir)
    {
        CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "map DG buffer");
        SYS_MemMap(&(pFrame->sBuffer), pDir, MMU_RO);
    }
    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameUnmap(KRN_DG_FRAME *pFrame)
{
    IMG_ASSERT(pFrame);

    SYS_MemUnmap(&(pFrame->sBuffer));

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameDestroy(KRN_DG_FRAME *pFrame)
{
    IMG_ASSERT(pFrame);

    if (pFrame->sBuffer.uiAllocated > 0)
    {
        SYS_MemFree(&(pFrame->sBuffer));
    }
    IMG_FREE(pFrame);

    return IMG_SUCCESS;
}
