/**
*******************************************************************************
@file ci_intdg_km.c

@brief internal data generator implementation

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
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_ioctrl.h"
#include "ci_kernel/ci_hwstruct.h"
#include "mmulib/heap.h"

static void List_clearDGBuffer(void *elem)
{
    KRN_CI_DatagenFreeFrame((KRN_CI_DGBUFFER*)elem);
}

static IMG_BOOL8 List_MapDGBuffer(void *listelem, void *directory)
{
    KRN_CI_DGBUFFER *pBuffer = (KRN_CI_DGBUFFER*)listelem;
    struct MMUDirectory *pDirectory = (struct MMUDirectory*)directory;
    IMG_RESULT ret;

    ret = SYS_MemMap(&(pBuffer->sMemory), pDirectory, MMU_RO);
    if (IMG_SUCCESS != ret)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_UnmapDGBuffer(void *listelem, void *unusedparam)
{
    KRN_CI_DGBUFFER *pBuffer = (KRN_CI_DGBUFFER*)listelem;
    IMG_RESULT ret;

    ret = SYS_MemUnmap(&(pBuffer->sMemory));
    return IMG_TRUE;
}

IMG_BOOL8 ListDestroy_KRN_CI_DGBuffer(void *elem, void *param)
{
    KRN_CI_DGBUFFER *pBuff = (KRN_CI_DGBUFFER *)elem;

    if (pBuff->sMemory.pMapping != NULL)
    {
        SYS_MemUnmap(&(pBuff->sMemory));
    }
    KRN_CI_DatagenFreeFrame(pBuff);

    return IMG_TRUE;
}

IMG_RESULT KRN_CI_DatagenInit(KRN_CI_DATAGEN *pDG,
    KRN_CI_CONNECTION *pConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 uiDirIndex = 0;

    IMG_ASSERT(NULL != pDG);
    IMG_ASSERT(NULL == pDG->pHeap);
    IMG_ASSERT(NULL != g_psCIDriver);

    // map the frames to the MMU
    // uiDirIndex = pDG->userDG.ui8IIFDGIndex;
    /* uiDirIndex forced to 0 because all internal DG use the same requestor
     * - if not the line above would be used */
    if (g_psCIDriver->sMMU.apDirectory[uiDirIndex])
    {
        pDG->pHeap = IMGMMU_HeapCreate(
            g_psCIDriver->sMMU.aHeapStart[CI_INTDG_HEAP],
            g_psCIDriver->sMMU.uiAllocAtom,
            g_psCIDriver->sMMU.aHeapSize[CI_INTDG_HEAP],
            &ret);
        if (ret != IMG_SUCCESS)
        {
            CI_FATAL("Failed to allocate internal DG virtual memory heap\n");
        }
    }

    pDG->pConnection = pConnection;
    pDG->sCell.object = pDG;

    ret = SYS_LockInit(&(pDG->slistLock));
    if (IMG_SUCCESS != ret) // unlikely
    {
        CI_FATAL("Failed to initialise lists lock\n");
        return ret;
    }

    ret = List_init(&(pDG->sList_available));
    if (IMG_SUCCESS != ret) // unlikely
    {
        CI_FATAL("Failed to initialise available list\n");
        return ret;
    }

    ret = List_init(&(pDG->sList_pending));
    if (IMG_SUCCESS != ret) // unlikely
    {
        CI_FATAL("Failed to initialise pending list\n");
        return ret;
    }

    ret = List_init(&(pDG->sList_busy));
    if (IMG_SUCCESS != ret) // unlikely
    {
        CI_FATAL("Failed to initialise busy list\n");
        return ret;
    }

    ret = List_init(&(pDG->sList_processed));
    if (IMG_SUCCESS != ret) // unlikely
    {
        CI_FATAL("Failed to initialise processed list\n");
        return ret;
    }

    // generate unique ID
    SYS_LockAcquire(&(pConnection->sLock));
    {
        // pre-incremental to avoid ui32PipelineIDBase to be 0
        pDG->ui32Identifier = ++(pConnection->ui32IIFDGIDBase);

        ret = List_pushBack(&(pConnection->sList_iifdg), &(pDG->sCell));
        if (ret != IMG_SUCCESS)
        {
            /* cannot fail to insert an object unless list is NULL or
             * cell already attached */
            CI_FATAL("failed to add the configuration to the internal "\
                "datagenerator list\n");
        }
    }
    SYS_LockRelease(&(pConnection->sLock));

    return ret;
}

IMG_RESULT KRN_CI_DatagenClear(KRN_CI_DATAGEN *pDG)
{
    IMG_ASSERT(NULL != pDG);
    IMG_ASSERT(NULL != pDG->pConnection);

    SYS_LockAcquire(&(pDG->pConnection->sLock));
    {
        List_detach(&(pDG->sCell)); // detach from pConnection->iifdg
    }
    SYS_LockRelease(&(pDG->pConnection->sLock));

    // HW should be stoped, we cannot get interrupts on that object
    //SYS_LockAcquire(&(pDG->slistLock));
    {
        List_clearObjects(&(pDG->sList_available), &List_clearDGBuffer);
        List_clearObjects(&(pDG->sList_pending), &List_clearDGBuffer);
        List_clearObjects(&(pDG->sList_busy), &List_clearDGBuffer);
        List_clearObjects(&(pDG->sList_processed), &List_clearDGBuffer);
    }
    //SYS_LockDestroy(&(pDG->slistLock));

    SYS_LockDestroy(&(pDG->slistLock));

    if (pDG->pHeap)
    {
        IMGMMU_HeapDestroy(pDG->pHeap);
        pDG->pHeap = NULL;
    }

    IMG_FREE(pDG);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DatagenAllocateFrame(KRN_CI_DATAGEN *pDG,
struct CI_DG_FRAMEINFO *pFrameParam)
{
    KRN_CI_DGBUFFER *pNewBuffer = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(NULL != pDG);
    IMG_ASSERT(NULL != pFrameParam);

    pNewBuffer = (KRN_CI_DGBUFFER*)IMG_CALLOC(1, sizeof(KRN_CI_DGBUFFER));
    if (pNewBuffer == NULL)
    {
        CI_FATAL("Failed to allocate internal DG buffer structure "\
            "(%ld Bytes)\n", sizeof(KRN_CI_DGBUFFER));
        return IMG_ERROR_MALLOC_FAILED;
    }
    pNewBuffer->pDatagen = pDG;
    pNewBuffer->sCell.object = pNewBuffer;

#ifdef INFOTM_ISP
    ret = SYS_MemAlloc(&(pNewBuffer->sMemory), pFrameParam->uiSize,
        pDG->pHeap, 0, "IntDgBuf");
#else
    ret = SYS_MemAlloc(&(pNewBuffer->sMemory), pFrameParam->uiSize,
        pDG->pHeap, 0);
#endif //INFOTM_ISP
    if (ret != IMG_SUCCESS)
    {
        IMG_FREE(pNewBuffer);
        CI_FATAL("Failed to allocate device memory\n");
        return ret;
    }

    SYS_LockAcquire(&(pDG->pConnection->sLock));
    {
        if (pDG->pConnection->iMemMapBase < 0)
        {
            pDG->pConnection->iMemMapBase = 0; // prevent overflow
        }
        pNewBuffer->ID = (++(pDG->pConnection->iMemMapBase));//<<PAGE_SHIFT;

        List_pushBack(&(pDG->pConnection->sList_unmappedDGBuffers),
            &(pNewBuffer->sCell));
    }
    SYS_LockRelease(&(pDG->pConnection->sLock));

    pFrameParam->mmapId = pNewBuffer->ID;

    // frame will be added to sList_available when mmap is called

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DatagenAcquireFrame(KRN_CI_DATAGEN *pDG,
struct CI_DG_FRAMEINFO *pFrameParam)
{
    KRN_CI_DGBUFFER *pBuff = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pDG);
    IMG_ASSERT(pFrameParam);

    if (pDG->bNeedsReset)
    {
        CI_FATAL("Cannot acquire more frame - IntDG received DG_INT_ERROR "\
            "- restart capture needed\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    SYS_LockAcquire(&(pDG->slistLock));
    {
        // make any remaining processed frame available
        pFound = List_popFront(&(pDG->sList_processed));
        while (pFound != NULL)
        {
            List_pushBack(&(pDG->sList_available), pFound);
            pFound = List_popFront(&(pDG->sList_processed));  // next
        }

        if (pFrameParam->uiFrameId > 0)
        {
            pFound = List_visitor(&(pDG->sList_available),
                &(pFrameParam->uiFrameId), &List_FindDGBuffer);
        }
        else
        {
            pFound = List_getHead(&(pDG->sList_available));
        }

        if (pFound != NULL)
        {
            List_detach(pFound);

            List_pushBack(&(pDG->sList_pending), pFound);

            pBuff = (KRN_CI_DGBUFFER*)pFound->object;
        }
    }
    SYS_LockRelease(&(pDG->slistLock));

    if (pBuff == NULL)
    {
        if (pFrameParam->uiFrameId > 0)
        {
            CI_FATAL("Failed to find frame %d in available list!\n",
                pFrameParam->uiFrameId);
        }
        else
        {
            CI_FATAL("Available list is empty!\n");
        }
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    pFrameParam->uiFrameId = pBuff->ID;

    return IMG_SUCCESS;
}

// assumes that the buffer was found in the correct list
IMG_RESULT KRN_CI_DatagenFreeFrame(KRN_CI_DGBUFFER *pDGBuffer)
{
    IMG_RESULT ret;

    IMG_ASSERT(NULL != pDGBuffer);

    /* in case it is mapped (deleting while running is not possible but may
     * be in the future) */
    //SYS_MemUnmap(&(pDGBuffer->sMemory));

    ret = SYS_MemFree(&(pDGBuffer->sMemory));
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to free device memory\n");
    }

    IMG_FREE(pDGBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DatagenTriggerFrame(KRN_CI_DATAGEN *pDG,
struct CI_DG_FRAMETRIG *pFrameInfo)
{
    KRN_CI_DGBUFFER *pBuffer = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    int identifier = 0;
    sCell_T *pFound = NULL;

    IMG_ASSERT(NULL != pDG);
    IMG_ASSERT(pFrameInfo != NULL);

    identifier = pFrameInfo->uiFrameId;

    if (!pDG->bCapturing)
    {
        CI_FATAL("Capture needs to be started before triggering frames!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    if (pDG->bNeedsReset)
    {
        CI_FATAL("Cannot add more frames - IntDG received DG_INT_ERROR "\
            "- restart capture needed\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    SYS_LockAcquire(&(pDG->slistLock));
    {
        pFound = List_visitor(&(pDG->sList_pending), &identifier,
            &List_FindDGBuffer);
        /* not doing everything in a single lock because it may take time
         * to update the device memory */
    }
    SYS_LockRelease(&(pDG->slistLock));

    if (NULL != pFound)
    {
        pBuffer = (KRN_CI_DGBUFFER*)pFound->object;

        Platform_MemUpdateDevice(&(pBuffer->sMemory));

        if (pFrameInfo->uiStride*pFrameInfo->uiHeight
            <= pBuffer->sMemory.uiAllocated)
        {
            pBuffer->ui32Stride = pFrameInfo->uiStride;
            pBuffer->ui16Width = pFrameInfo->uiWidth;
            pBuffer->ui16Height = pFrameInfo->uiHeight;
            pBuffer->ui16HorizontalBlanking = pFrameInfo->uiHorizontalBlanking;
            pBuffer->ui16VerticalBlanking = pFrameInfo->uiVerticalBlanking;

            // sListLock to protect sList_pending
            // sLisSpinLock to protect sList_busy
            SYS_LockAcquire(&(pDG->slistLock));
            {
                List_detach(pFound);  // access to pending list!
                List_pushBack(&(pDG->sList_busy), pFound);

                // 1st element need to be submitted
                if (pDG->sList_busy.ui32Elements == 1)
                {
                    HW_CI_DatagenTrigger(pBuffer);
                }
            }
            SYS_LockRelease(&(pDG->slistLock));

            ret = IMG_SUCCESS;
        }
        else
        {
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }
    }



    if (NULL == pBuffer)
    {
        CI_FATAL("Failed to find the frame %d in pending list\n",
            pFrameInfo->uiFrameId);
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("Given frame info need %d Bytes - selected frame only "\
            "has %d B allocated\n",
            pFrameInfo->uiStride*pFrameInfo->uiHeight,
            pBuffer->sMemory.uiAllocated);
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DatagenReleaseFrame(KRN_CI_DATAGEN *pDG,
    IMG_UINT32 ui32FrameID)
{
    sCell_T *pFound = NULL;

    IMG_ASSERT(NULL != pDG);

    SYS_LockAcquire(&(pDG->slistLock));
    {
        int identifier = ui32FrameID;

        pFound = List_visitor(&(pDG->sList_pending), &identifier,
            &List_FindDGBuffer);

        if (pFound)
        {
            List_detach(pFound);
            List_pushBack(&(pDG->sList_available), pFound);
        }
    }
    SYS_LockRelease(&(pDG->slistLock));

    if (NULL == pFound)
    {
        CI_FATAL("Failed to find the frame %d in pending list\n",
            ui32FrameID);
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_DatagenStart(KRN_CI_DATAGEN *pDG,
    const CI_DATAGEN *pDGConfig)
{
    IMG_RESULT ret;
    struct MMUDirectory *pDir = NULL;
    IMG_UINT32 uiDirIndex = 0;

    IMG_ASSERT(NULL != pDG);

    if (pDG->bCapturing)
    {
        ret = IMG_MEMCMP(&(pDG->userDG), pDGConfig, sizeof(CI_DATAGEN));
        if (0 == ret)
        {
            CI_FATAL("Datagen already started!\n");
            return IMG_SUCCESS;
        }
        else
        {
            CI_FATAL("Datagen already started! cannot change its "\
                "configuration without stoping it first!\n");
            return IMG_ERROR_UNEXPECTED_STATE;
        }
    }
    else
    {
        IMG_MEMCPY(&(pDG->userDG), pDGConfig, sizeof(CI_DATAGEN));
    }

    ret = KRN_CI_DriverAcquireDatagen(pDG);

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to acquire internal data generator\n");
        return ret;
    }

    pDG->bCapturing = IMG_TRUE;

    // map the frames to the MMU
    // uiDirIndex = pDG->userDG.ui8IIFDGIndex;
    /* uiDirIndex forced to 0 because all internal DG use the same requestor
     * - if not the line above would be used */
    uiDirIndex = 0;
    pDir = g_psCIDriver->sMMU.apDirectory[uiDirIndex];

    SYS_LockAcquire(&(pDG->slistLock));
    {
        if (0 != pDG->sList_busy.ui32Elements)
        {
            CI_WARNING("busy list has elements!\n");
        }
        List_visitor(&(pDG->sList_available), pDir, &List_MapDGBuffer);
        List_visitor(&(pDG->sList_pending), pDir, &List_MapDGBuffer);
        // should do nothing as list should be emtpy!
        //List_visitor(&(pDG->sList_busy), pDir, &List_MapDGBuffer);
        // should do nothing as list should be emtpy!
        //List_visitor(&(pDG->sList_processed), pDir, &List_MapDGBuffer);
    }
    SYS_LockRelease(&(pDG->slistLock));

    /* BRN48951 recomends SW to flush the cache for every buffer mapping but
     * we do it once all buffers are mapped */
    KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU), uiDirIndex, IMG_TRUE);

    // configure and start HW - no frame pushed yet
    ret = HW_CI_DatagenStart(pDG);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to start IIF DG %d\n", pDG->userDG.ui8IIFDGIndex);
        KRN_CI_DatagenStop(pDG);
    }

    return ret;
}

IMG_RESULT KRN_CI_DatagenStop(KRN_CI_DATAGEN *pDG)
{
    IMG_RESULT ret;
    IMG_UINT32 uiDirIndex = 0;

    IMG_ASSERT(NULL != pDG);

    if (!pDG->bCapturing)
    {
        CI_WARNING("Datagen is already stopped!\n");
        return IMG_SUCCESS;
    }

    // we should remove all submitted frames but the last one
    /* if there is a running frame we should be waiting for it to be
     * captured before stopping */

    // stop HW and disables the interrupts
    HW_CI_DatagenStop(pDG);

    // unmap all from MMU
    // all busy frames become available again!
    SYS_LockAcquire(&(pDG->slistLock));
    {
        sCell_T *pHead = NULL;

        // push all busy or processed buffers into available for unmapping
        pHead = List_popFront(&(pDG->sList_busy));
        while (pHead != NULL)
        {
            List_pushBack(&(pDG->sList_available), pHead);
            pHead = List_popFront(&(pDG->sList_busy)); // next
        }

        pHead = List_popFront(&(pDG->sList_processed));
        while (pHead != NULL)
        {
            List_pushBack(&(pDG->sList_available), pHead);
            pHead = List_popFront(&(pDG->sList_processed)); // next
        }

        List_visitor(&(pDG->sList_available), NULL, &List_UnmapDGBuffer);
        List_visitor(&(pDG->sList_pending), NULL, &List_UnmapDGBuffer);
    }
    SYS_LockRelease(&(pDG->slistLock));

    // uiDirIndex = pDG->userDG.ui8IIFDGIndex;
    /* uiDirIndex forced to 0 because all internal DG use the same requestor
     * - if not the line above would be used */
    uiDirIndex = 0;

    /* BRN48951 recomends SW to flush the cache for every buffer mapping but
     * we do it once all buffers are unmapped */
    KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU), uiDirIndex, IMG_TRUE);

    ret = KRN_CI_DriverReleaseDatagen(pDG);

    // should be releasable if bCapture was true!!!
    IMG_ASSERT(ret == IMG_SUCCESS);
    pDG->bCapturing = IMG_FALSE;

    return IMG_SUCCESS;
}
