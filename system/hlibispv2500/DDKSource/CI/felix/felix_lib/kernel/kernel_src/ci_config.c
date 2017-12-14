/**
*******************************************************************************
@file ci_config.c

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

#include "ci/ci_api_structs.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_hwstruct.h"
#include "ci_kernel/ci_connection.h" // for PAGE_SHIFT
#include "ci_kernel/ci_ioctrl.h" // for CI_ALLOC_PARAM

#ifdef FELIX_FAKE
#include <ci_kernel/ci_debug.h> // offset in virtual memory heaps
#endif

#include <reg_io2.h>

#include <mmulib/heap.h>

/*-------------------------------------------------------------------------
 * Following elements are in the IMG_CI_CONFIG functions group
 *---------------------------------------------------------------------------*/

// used to clean the lists of the capture part of the pipeline
IMG_BOOL8 ListDestroy_KRN_CI_Shot(void* elem, void *param)
{
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)elem;
    KRN_CI_ShotDestroy((KRN_CI_SHOT*)pShot);
    return IMG_TRUE;
}

IMG_BOOL8 ListDestroy_KRN_CI_Buffer(void *elem, void *param)
{
    KRN_CI_BUFFER *pBuff = (KRN_CI_BUFFER *)elem;
    KRN_CI_BufferDestroy(pBuff);
    return IMG_TRUE;
}

IMG_BOOL8 ListSearch_KRN_CI_Buffer(void *listelem, void *lookingFor)
{
    KRN_CI_BUFFER *pBuff = (KRN_CI_BUFFER*)listelem;
    int *identifier = (int*)lookingFor;

    if ( pBuff->ID == *identifier )
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

#ifdef INFOTM_ISP
IMG_BOOL8 ListSearch_KRN_CI_Buffer_Type(void *listelem, void *lookingForType)
{
    KRN_CI_BUFFER *pBuff = (KRN_CI_BUFFER*)listelem;
    int *pBufType = (int*)lookingForType;

    if ( pBuff->type == *pBufType )
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}
#endif //INFOTM_ISP

IMG_BOOL8 KRN_CI_PipelineVerify(const CI_PIPELINE *pPipeline,
    const CI_HWINFO *pHWInfo)
{
    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pHWInfo);

    if ( (pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT)==0
        && pPipeline->eHDRExtType.eFmt != PXL_NONE )
    {
        CI_FATAL("The HDR Extraction point is not supported by "\
            "HW version %d.%d\n",
            pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor
        );
        return IMG_FALSE;
    }
    if ( (pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT)==0
        && pPipeline->eRaw2DExtraction.eFmt != PXL_NONE )
    {
        CI_FATAL("The HDR Extraction point is not supported by "\
            "HW version %d.%d\n",
            pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor
        );
        return IMG_FALSE;
    }

    if ( (pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_TILING)==0
        && pPipeline->bSupportTiling )
    {
        CI_FATAL("Tiling support is not possible with HW version %d.%d\n",
            pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor
        );
        return IMG_FALSE;
    }
    
    return IMG_TRUE;
}

IMG_RESULT KRN_CI_PipelineInit(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_CONNECTION *pConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT( pPipeline != NULL );
    IMG_ASSERT( g_psCIDriver != NULL );

    SYS_LockInit(&(pPipeline->sListLock));
    List_init(&(pPipeline->sList_available));
    List_init(&(pPipeline->sList_sent));

#ifdef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_SpinlockInit(&(pPipeline->sProcessingSpinlock));
#endif
    SYS_SemInit(&(pPipeline->sProcessedSem), 0);
    List_init(&(pPipeline->sList_processed));
    List_init(&(pPipeline->sList_pending));
    List_init(&(pPipeline->sList_availableBuffers));
    pPipeline->uiWantedDPFBuffer = 0;
    pPipeline->uiENSSizeNeeded = 0;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "pipeline config load structure");

    // using the MMU - if not MMU heap will be NULL and not used
    if ( g_psCIDriver->sMMU.uiAllocAtom > 0 )
    {
        IMG_INT32 i;

        for ( i = 0 ; i < CI_N_HEAPS ; i++ )
        {
            if (g_psCIDriver->sMMU.aHeapSize[i] > 0)
            {
                CI_DEBUG("create virt heap %d (%"IMG_SIZEPR"u bytes)\n",
                    i, g_psCIDriver->sMMU.aHeapSize[i]);
                pPipeline->apHeap[i] = IMGMMU_HeapCreate(
                    g_psCIDriver->sMMU.aHeapStart[i],
                    g_psCIDriver->sMMU.uiAllocAtom,
                    g_psCIDriver->sMMU.aHeapSize[i], &ret);

                if (!pPipeline->apHeap[i])
                {
                    CI_FATAL("Failed to create SW heap %u (%" IMG_SIZEPR "u "\
                        "to %" IMG_SIZEPR "u alloc_atom=%" IMG_SIZEPR "u)\n",
                             i, g_psCIDriver->sMMU.aHeapStart[i],
                             g_psCIDriver->sMMU.aHeapSize[i],
                             g_psCIDriver->sMMU.uiAllocAtom);
                    i--;
                    while(i>=0)
                    {
                        IMGMMU_HeapDestroy(pPipeline->apHeap[i]);
                        pPipeline->apHeap[i] = NULL;
                        i--;
                    }
                    return ret;
                }
#ifdef FELIX_FAKE
                if(aVirtualHeapsOffset[i]>0)
                {
                    // this allocation is lost!
                    IMGMMU_HeapAlloc *res = NULL;
                    IMG_UINT32 offset = aVirtualHeapsOffset[i];
                    char message[64];

                    offset += aVirtualHeapsOffset[i]%
                        g_psCIDriver->sMMU.uiAllocAtom;
                    sprintf(message, "ctx %d pre-allocates %uB from "\
                        "virtual heap %d",
                        pPipeline->userPipeline.ui8Context, offset, i);
                    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
                        message);

                    res = IMGMMU_HeapAllocate(pPipeline->apHeap[i], offset,
                        &ret);
                    if(!res)
                    {
                        CI_FATAL("failed to add offset %d (from %d) "\
                            "to heap %d\n",
                            offset, aVirtualHeapsOffset[i], i);
                    }
                }
#endif
            }
        }
    }

    pPipeline->pLoadStructStamp = (char*)IMG_CALLOC(
        1, HW_CI_LoadStructureSize());
    if (!pPipeline->pLoadStructStamp)
    {
        CI_FATAL("failed to create the HW load structure stamp "\
            "(%"IMG_SIZEPR"u Bytes)\n", HW_CI_LoadStructureSize());
        KRN_CI_PipelineDestroy(pPipeline);
        return IMG_ERROR_MALLOC_FAILED;
    }

    pPipeline->pConnection = pConnection;

    // if supporting tiling setup the tiling alignment
    if (pPipeline->userPipeline.bSupportTiling)
    {
        IMG_UINT32 uiMaxTileStride = 0;
        CI_SHOT config;
#ifdef FELIX_FAKE
        char message[256];
#endif

        config.bEncTiled = pPipeline->userPipeline.bSupportTiling;
        config.bDispTiled = pPipeline->userPipeline.bSupportTiling;
        config.bHDRExtTiled = pPipeline->userPipeline.bSupportTiling;

        // will compute the tiled stride afterwards!
        pPipeline->uiTiledStride = 0;
        ret = KRN_CI_BufferFromConfig(&config, pPipeline, IMG_FALSE);
        if (ret)
        {
            CI_FATAL("failed to compute sizes to allocate a buffer with "\
                "tiling support\n");
            KRN_CI_PipelineDestroy(pPipeline);
            return ret;
        }
        
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aEncYSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aEncCbCrSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aDispSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aHDRExtSize[0]);

        pPipeline->uiTiledStride = uiMaxTileStride;
        pPipeline->uiTiledAlignment =
            KRN_CI_MMUTilingAlignment(uiMaxTileStride);

#ifdef FELIX_FAKE
        sprintf(message, "Compute tiled stride for Context %d: Y=0x%x "\
            "CbCr=0x%x Disp=0x%x HDRExt=0x%x - max=0x%x virt_algin=0x%x",
            pPipeline->userPipeline.ui8Context,
            config.aEncYSize[0], config.aEncCbCrSize[0], config.aDispSize[0],
            config.aHDRExtSize[0], pPipeline->uiTiledStride,
            pPipeline->uiTiledAlignment);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#endif
    }

    return IMG_SUCCESS;
}	

IMG_RESULT KRN_CI_PipelineAddShot(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_SHOT *pShot)
{
    IMG_RESULT ret;

    IMG_ASSERT(pPipeline != NULL);
    IMG_ASSERT(pShot != NULL);

    ret = KRN_CI_ShotConfigure(pShot, pPipeline);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to configure the new shot\n");
        return ret;
    }

    IMG_ASSERT(pPipeline->pConnection);

    /** @ if the pipeline is capturing map the new buffer
     * - otherwise it is going to be mapped at starting time */

    // create a bunch of unique identifier for the memory mapping
    SYS_LockAcquire(&(pPipeline->pConnection->sLock));
    {
        /* dynamic buffers should be mapped when allocated in
         * KRN_CI_PipelineCreateBuffer */
    	//printk("AddShot: pShot->hwSave.sMemory.uiAllocated=%d\n", pShot->hwSave.sMemory.uiAllocated);
        if ( pShot->hwSave.sMemory.uiAllocated > 0 )
        {
            if ( pPipeline->pConnection->iMemMapBase < 0 )
            {
                // prevent overflow
                pPipeline->pConnection->iMemMapBase = 0;
            }
            pShot->hwSave.ID = (++(pPipeline->pConnection->iMemMapBase));
        }

        if ( pShot->sDPFWrite.sMemory.uiAllocated > 0 )
        {
            if ( pPipeline->pConnection->iMemMapBase < 0 )
            {
                // prevent overflow
                pPipeline->pConnection->iMemMapBase = 0;
            }
            pShot->sDPFWrite.ID = (++(pPipeline->pConnection->iMemMapBase));
        }

        if ( pShot->sENSOutput.sMemory.uiAllocated > 0 )
        {
            if ( pPipeline->pConnection->iMemMapBase < 0 )
            {
                // prevent overflow
                pPipeline->pConnection->iMemMapBase = 0;
            }
            pShot->sENSOutput.ID = (++(pPipeline->pConnection->iMemMapBase));
        }

        /** @ what if the identifier rolls over? unlikely because i32
         * allocated buffer per connection */
        pShot->iIdentifier = ++(pPipeline->pConnection->iShotIdBase);

        /* the buffer first goes to the unmapped connection's list
         *- when all its buffers are mapped it becomes available */
        // for user space the allocation and mapping is the same action
        ret = List_pushBack(&(pPipeline->pConnection->sList_unmappedShots),
            &(pShot->sListCell));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to add the buffer to the unmapped list\n");
            pShot->pPipeline = NULL;
        }
        //printk("AddShot: ShotType=%d, ID=%d, sList_unmappedShots=%d\n", pShot->eKrnShotType, pShot->iIdentifier, pPipeline->pConnection->sList_unmappedShots.ui32Elements);
    }
    SYS_LockRelease(&(pPipeline->pConnection->sLock));

    /* negative if overflow - not likely to happen (INT_MAX different
     * buffers for single connection) */
    IMG_ASSERT(pShot->iIdentifier > 0);

    return ret;
}

KRN_CI_BUFFER* KRN_CI_PipelineCreateBuffer(KRN_CI_PIPELINE *pPipeline,
    struct CI_ALLOC_PARAM *pParam, IMG_RESULT *ret)
{
    struct MMUHeap *pHeap = NULL;
    CI_SHOT config;
    KRN_CI_BUFFER *pBuffer = NULL;
    int bufferType = 0;
    IMG_UINT32 uiVirtAlignment = 0;
    IMG_UINT32 ui32Size = 0;
#ifdef INFOTM_ISP
    static int EncID = 0;
    static int DispID = 0;
    static int HdrExtID = 0;
    static int HdrInsID = 0;
    static int Raw2dID = 0;
    char szName[20];
    IMG_UINT32 Width = 0, Height = 0;
#endif

    IMG_ASSERT(pPipeline != NULL);
    IMG_ASSERT(pParam != NULL);
    IMG_ASSERT(ret != NULL);
    IMG_ASSERT(pPipeline->pConnection);

    if( pParam->bTiled && !pPipeline->userPipeline.bSupportTiling )
    {
        CI_FATAL("Pipeline is not configured to support tiling, use "\
            "bSupportTiling before registration to allow "\
            "impotation/allocation of tiled buffers\n");
        *ret = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }

    pHeap = pPipeline->apHeap[CI_IMAGE_HEAP];
    if ( pParam->bTiled )
    {
        pHeap = pPipeline->apHeap[CI_TILED_IMAGE_HEAP0
            + pPipeline->userPipeline.ui8Context];
        // should have been computed at registration time
        IMG_ASSERT(pPipeline->uiTiledAlignment>0);
        uiVirtAlignment = pPipeline->uiTiledAlignment;
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "using tiled virtual heap");
    }
    config.bEncTiled = pParam->bTiled;
    config.bDispTiled = pParam->bTiled;
    config.bHDRExtTiled = pParam->bTiled;

    // compute sizes for the buffer

    *ret = KRN_CI_BufferFromConfig(&config, pPipeline, IMG_FALSE);
    if (*ret)
    {
        CI_FATAL("failed to compute sizes to allocate a buffer\n");
        return NULL;
    }

    switch(pParam->type)
    {
    case CI_ALLOC_ENC:
        ui32Size = config.aEncYSize[0] * config.aEncYSize[1]
            + config.aEncCbCrSize[0]*config.aEncCbCrSize[1];
        bufferType = CI_BUFF_ENC;
#ifdef INFOTM_ISP
        sprintf(szName, "Enc(%d)", EncID++);
        Width = pPipeline->userPipeline.ui16MaxEncOutWidth;
        Height = pPipeline->userPipeline.ui16MaxEncOutHeight;
#ifdef INFOTM_ENABLE_ISP_DEBUG
        printk("@@@ CreateBuf[%d]: EncYSize[0]=%d, EncYSize[1]=%d, EncCbCrSize[0]=%d, EncCbCrSize[1]=%d\n",
                    pPipeline->userPipeline.ui8Context,
                    config.aEncYSize[0],
                    config.aEncYSize[1],
                    config.aEncCbCrSize[0],
                    config.aEncCbCrSize[1]);
#endif //INFOTM_ENABLE_ISP_DEBUG
#endif //INFOTM_ISP
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate encoder output");
        break;

    case CI_ALLOC_DISP:
        ui32Size = config.aDispSize[0] * config.aDispSize[1];
        bufferType = CI_BUFF_DISP;
#ifdef INFOTM_ISP
        sprintf(szName, "Disp(%d)", DispID++);
        Width = pPipeline->userPipeline.ui16MaxDispOutWidth;
        Height = pPipeline->userPipeline.ui16MaxDispOutHeight;
#endif //INFOTM_ISP
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate display/data-extraction output");
        break;

    case CI_ALLOC_HDREXT:
        ui32Size = config.aHDRExtSize[0] * config.aHDRExtSize[1];
        bufferType = CI_BUFF_HDR_WR;
#ifdef INFOTM_ISP
        sprintf(szName, "HdrExt(%d)", HdrExtID++);
        Width = config.aHDRExtSize[0];
        Height = config.aHDRExtSize[1];
#endif //INFOTM_ISP		
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate HDR-extraction output");
        break;

    case CI_ALLOC_HDRINS:
        ui32Size = config.aHDRInsSize[0] * config.aHDRInsSize[1];
        bufferType = CI_BUFF_HDR_RD;
#ifdef INFOTM_ISP
        sprintf(szName, "HdrIns(%d)", HdrInsID++);
        Width = config.aHDRInsSize[0];
        Height = config.aHDRInsSize[1];
#endif //INFOTM_ISP
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate HDR-insertion output");
        break;

    case CI_ALLOC_RAW2D:
        ui32Size = config.aRaw2DSize[0] * config.aRaw2DSize[1];
        bufferType = CI_BUFF_RAW2D;
#ifdef INFOTM_ISP
        sprintf(szName, "Raw2D(%d)", Raw2dID++);
        Width = config.aRaw2DSize[0];
        Height = config.aRaw2DSize[1];
#endif //INFOTM_ISP
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate RAW2D-extraction output");
        break;
		
    }

    if (pParam->uiSize == 0)
    {
        pParam->uiSize = ui32Size;
    }
    else if (ui32Size > pParam->uiSize)
    {
        CI_FATAL("given size is too small to support the given format!\n");
        *ret = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }

    // allocate the buffer
#ifdef INFOTM_ISP
    if (pPipeline->userPipeline.uiTotalPipeline > 1)        
    {
        int iNb = pPipeline->userPipeline.uiTotalPipeline;

        if (pParam->type == CI_ALLOC_ENC)
        {
            if (pPipeline->userPipeline.ui8Context == 0)
                pParam->uiSize *= iNb;
            else
                pParam->uiSize = 4096;
        }
    }

    pBuffer = KRN_CI_BufferCreate(pParam->uiSize, pHeap, uiVirtAlignment,
        pParam->fd, ret, szName);

#ifdef INFOTM_ENABLE_ISP_DEBUG
    printk("@@@ CreateBuf[%d]: Name=%s, Type=%d, ID=%d, Size=%d, WxH=[%d x %d]\n",
                    pPipeline->userPipeline.ui8Context,
                    szName,
                    pParam->type,
                    pParam->fd,
                    pParam->uiSize,
                    Width,
                    Height);
#endif //INFOTM_ENABLE_ISP_DEBUG
#else
    pBuffer = KRN_CI_BufferCreate(pParam->uiSize, pHeap, uiVirtAlignment,
        pParam->fd, ret);
#endif //INFOTM_ISP
    if (!pBuffer)
    {
        CI_FATAL("Failed to allocate the buffer\n");
    }
    else
    {
        pBuffer->pPipeline = pPipeline;
        pBuffer->type = bufferType;
        pBuffer->bTiled = pParam->bTiled;
#ifdef INFOTM_ISP
        pBuffer->pDualBuffer = NULL;
#endif //INFOTM_ISP
    }

    return pBuffer;
}

IMG_RESULT KRN_CI_PipelineAddBuffer(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_BUFFER *pBuffer)
{
    IMG_RESULT ret;

    IMG_ASSERT(pPipeline != NULL);
    IMG_ASSERT(pPipeline->pConnection != NULL);
    IMG_ASSERT(pBuffer != NULL);
    /** @ if the pipeline is capturing map the new buffer
     * - otherwise it is going to be mapped at starting time */

    // create a bunch of unique identifier for the memory mapping
    SYS_LockAcquire(&(pPipeline->pConnection->sLock));
    {
        if ( pPipeline->pConnection->iMemMapBase < 0 )
        {
            pPipeline->pConnection->iMemMapBase = 0; // prevent overflow
        }
        pBuffer->ID = (++(pPipeline->pConnection->iMemMapBase));

        /* dynamic buffers are added to the list of unmapped buffers until
         * mapped in user-space - see KRN_CI_PipelineBufferMapped */
        ret=List_pushBack(&(pPipeline->pConnection->sList_unmappedBuffers),
            &(pBuffer->sCell));
    }
    SYS_LockRelease(&(pPipeline->pConnection->sLock));

    return ret;
}

IMG_RESULT KRN_CI_PipelineDeregisterBuffer(KRN_CI_PIPELINE *pPipeline,
    IMG_UINT32 id)
{
    sCell_T *pFound = NULL;
    KRN_CI_BUFFER* pBuffer = NULL;

    IMG_ASSERT(pPipeline != NULL);

    // allow deregistration while running
    /*if (pPipeline->bStarted)
    {
        CI_FATAL("Can't deregister a buffer while pipeline is started!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }*/

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        pFound = List_visitor(&(pPipeline->sList_availableBuffers),
            (void*)&id, &ListSearch_KRN_CI_Buffer);
        if (pFound)
        {
            List_detach(pFound);
            pBuffer = (KRN_CI_BUFFER*)pFound->object;
        }
    }
    SYS_LockRelease(&(pPipeline->sListLock));


    if (!pBuffer)
    {
        CI_WARNING("No such buffer registered in CI kernel-side");
        return IMG_ERROR_FATAL;
    }
    
    // if still mapped when destroyed it will be unmapped from MMU
    // @ do we need to flush the MMU if it was unmapped?
    KRN_CI_BufferDestroy(pBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineShotMapped(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_SHOT *pShot)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct MMUDirectory *pDir = NULL;

    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pShot);

    /* remove the shot from unmapped shots list */
    SYS_LockAcquire(&(pPipeline->pConnection->sLock));
    {
        ret = List_detach(&(pShot->sListCell));
    }
    SYS_LockRelease(&(pPipeline->pConnection->sLock));

    // The only way for this to fail is a wrong parameter
    IMG_ASSERT(IMG_SUCCESS == ret);

    // we can now use the directory to map things
    pDir = g_psCIDriver->sMMU.apDirectory[pPipeline->userPipeline.ui8Context];

    // if we are using the MMU
    if ( pDir && pPipeline->bStarted )
    {
        ret = KRN_CI_ShotMapMemory(pShot, pDir);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to map the new buffer to MMU\n");
            KRN_CI_ShotDestroy(pShot);
            return ret;
        }
    }

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        CI_DEBUG("Adding Shot to sList_available: phwEncoderOutput->"\
            "sMemory=%p, phwDisplayOutput->sMemory=%p, hwSave.sMemory=%p, "\
            "sDPFWrite.sMemory=%p, sENSOutput.sMemory=%p\n",
                &pShot->phwEncoderOutput->sMemory,
                &pShot->phwDisplayOutput->sMemory,
                &pShot->hwSave.sMemory,
                &pShot->sDPFWrite.sMemory,
                &pShot->sENSOutput.sMemory);

        ret = List_pushBack(&(pPipeline->sList_available),
            &(pShot->sListCell));

        /* dynamic buffers should be mapped with KRN_CI_PipelineBufferMapped
         * as they are not attached to Shot yet */
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    return ret;
}

IMG_RESULT KRN_CI_PipelineBufferMapped(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_BUFFER *pBuffer)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct MMUDirectory *pDir = NULL;

    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pBuffer);
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pPipeline->userPipeline.ui8Context < CI_N_CONTEXT);

    pDir = g_psCIDriver->sMMU.apDirectory[pPipeline->userPipeline.ui8Context];

    /* remove the shot from unmapped shots list */
    SYS_LockAcquire(&(pPipeline->pConnection->sLock));
    {
        ret = List_detach(&(pBuffer->sCell));
    }
    SYS_LockRelease(&(pPipeline->pConnection->sLock));
    // The only way for this to fail is a wrong parameter
    IMG_ASSERT(IMG_SUCCESS == ret);

    if (pPipeline->bStarted && pDir)
    {
        // use the list function to not re-implement the choice
        ret = List_MapBuffer(pBuffer, pDir);

        if (IMG_TRUE != ret)
        {
            /* unlikely to fail but if that happens user-space will have
             * one additional buffer it cannot use... */
            CI_FATAL("Failed to map the buffer to the device MMU - "\
                "destroying it\n");
            KRN_CI_BufferDestroy(pBuffer);
            return IMG_ERROR_FATAL;
        }

        /* BRN48951 recomends SW to flush the cache for every buffer mapping */
        KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU),
            pPipeline->userPipeline.ui8Context, IMG_TRUE);
    }

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
#ifdef INFOTM_ISP
        if (pPipeline->userPipeline.uiTotalPipeline > 1)
        {
            if (pPipeline->userPipeline.ui8Context == 1 && pPipeline->pPrevPipeline != NULL && pBuffer->type == CI_BUFF_ENC)
            {
                sCell_T *pFound = NULL;
                KRN_CI_PIPELINE *pPrevPipeline = (KRN_CI_PIPELINE*)pPipeline->pPrevPipeline;
                KRN_CI_BUFFER* pFoundBuffer;

                pFound = List_visitor(&(pPrevPipeline->sList_availableBuffers),	&(pBuffer->type), &ListSearch_KRN_CI_Buffer_Type);
                if (pFound)
                {
                    pFoundBuffer = (void*)pFound->object;
                    if (pFoundBuffer->type == CI_BUFF_ENC)
                    {
#ifdef INFOTM_ENABLE_ISP_DEBUG
                        printk("@@@    LinkBuf[%d]: name=%s, Type=%d, ID=%d, VirtAddr=0x%p, PhysAddr=0x%p\n",
                                    pPrevPipeline->userPipeline.ui8Context,
                                    pFoundBuffer->sMemory.szMemName,
                                    pFoundBuffer->type,
                                    pFoundBuffer->ID,
                                    pFoundBuffer->sMemory.pVirtualAddress,
                                    pFoundBuffer->sMemory.pPhysAddress);
#endif
                        List_detach(pFound);
                        pBuffer->pDualBuffer = (void*)pFoundBuffer;
                        List_pushBack(&(pPrevPipeline->sList_availableBuffers), &(pFoundBuffer->sCell));
                    }
                }
            }
        }
#endif //INFOTM_ISP

        ret = List_pushBack(&(pPipeline->sList_availableBuffers),
            &(pBuffer->sCell));
#ifdef INFOTM_ENABLE_ISP_DEBUG
        printk("@@@    BufMap[%d]: name=%s, Type=%d, ID=%d, VirtAddr=0x%p, PhysAddr=0x%p\n",
                        pPipeline->userPipeline.ui8Context,
                        pBuffer->sMemory.szMemName,
                        pBuffer->type,
                        pBuffer->ID,
                        pBuffer->sMemory.pVirtualAddress,
                        pBuffer->sMemory.pPhysAddress);
#endif //INFOTM_ENABLE_ISP_DEBUG

    }
    SYS_LockRelease(&(pPipeline->sListLock));

    return ret;
}

static IMG_RESULT IMG_CI_PipelineUpdateSubmitted(KRN_CI_PIPELINE *pPipeline)
{
    sCell_T *pHeadCell = NULL, *pCurrentCell = NULL;
    KRN_CI_SHOT *pCurrent = NULL;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockAcquire(&(pPipeline->sListLock));
#else
    SYS_SpinlockAcquire(&(pPipeline->sProcessingSpinlock));
#endif
    {
        pHeadCell = List_getHead(&(pPipeline->sList_pending));
        // apply new configuration from TAIL
        pCurrentCell = List_getTail(&(pPipeline->sList_pending));
    }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LockRelease(&(pPipeline->sListLock));
#else
    SYS_SpinlockRelease(&(pPipeline->sProcessingSpinlock));
#endif
    /* apply the new configuration from the TAIL to the cell before the head
     * (the head is submitted to HW and most likely fetched already) */
    while ( pCurrentCell && pCurrentCell != pHeadCell )
    {
        pCurrent = (KRN_CI_SHOT*)pCurrentCell->object;

        HW_CI_ShotUpdateSubmittedLS(pCurrent);
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockAcquire(&(pPipeline->sListLock));
#else
        SYS_SpinlockAcquire(&(pPipeline->sProcessingSpinlock));
#endif
        {
            // in case the head changed
            pHeadCell = List_getHead(&(pPipeline->sList_pending));
            pCurrentCell = List_getPrev(pCurrentCell);
        }
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
        SYS_LockRelease(&(pPipeline->sListLock));
#else
        SYS_SpinlockRelease(&(pPipeline->sProcessingSpinlock));
#endif
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
    return IMG_SUCCESS;
}

/** @ use CI_UPD_FLAG or the load flag to know which module to update
 * in memory (at the moment all are updated) */
IMG_RESULT KRN_CI_PipelineUpdate(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bRegAccess, IMG_BOOL8 bASAP)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT( pPipeline != NULL );
    IMG_ASSERT( g_psCIDriver != NULL );

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    //Platform_MemGetMemory(&(pPipeline->sHWLoadStructStamp));

    HW_CI_Load_BLC(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sBlackCorrection));

    HW_CI_Load_RLT(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sRawLUT));

    if ( bRegAccess )
    {
        HW_CI_Load_LSH_DS(pPipeline->pLoadStructStamp,
            &(pPipeline->userPipeline.sDeshading));

        ret = HW_CI_Reg_LSH_DS(pPipeline);
        if ( IMG_SUCCESS != ret )
        {
            CI_FATAL("failed to update the LSH registers\n");
            return ret;
        }

        HW_CI_Reg_DPF(pPipeline);
    }

    HW_CI_Load_LSH_WB(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sWhiteBalance));

    HW_CI_Load_DNS(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sDenoiser));

    HW_CI_Load_DPF(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sDefectivePixels));

    HW_CI_Load_LCA(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sChromaAberration));

    HW_CI_Load_CCM(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sColourCorrection));

    HW_CI_Load_MGM(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sMainGamutMapper));

    HW_CI_Load_GMA(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sGammaCorrection));

    HW_CI_Load_R2Y(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sRGBToYCC));

    HW_CI_Load_MIE(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sImageEnhancer));

    HW_CI_Load_TNM(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sToneMapping));

    HW_CI_Load_SHA(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sSharpening));

    HW_CI_Load_Scaler(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sEncoderScaler));

    HW_CI_Load_Scaler(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sDisplayScaler));

    HW_CI_Load_Y2R(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sYCCToRGB));

    HW_CI_Load_DGM(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sDisplayGamutMapper));

    // statistics

    HW_CI_Load_EXS(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sExposureStats));

    HW_CI_Load_FOS(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sFocusStats));

    HW_CI_Load_WBS(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sWhiteBalanceStats));

    HW_CI_Load_HIS(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sHistStats));

    HW_CI_Load_FLD(pPipeline->pLoadStructStamp,
        &(pPipeline->userPipeline.sFlickerStats));

    if ( !bRegAccess && bASAP )
    {
        IMG_CI_PipelineUpdateSubmitted(pPipeline); // cannot fail yet
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

    return ret;
}

IMG_RESULT KRN_CI_PipelineDestroy(KRN_CI_PIPELINE *pPipeline)
{
    IMG_RESULT ret = IMG_SUCCESS;
    /* all the free check for NULL beforehand because this could be called
     * when creation failed */
    IMG_UINT32 i;
    sCell_T *pFound = NULL;

    if (!pPipeline)
    {
        CI_FATAL("pPipeline is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!g_psCIDriver)
    {
        CI_WARNING("cleaning left-over config 0x%p\n", pPipeline);
        // unmap to allow destruction
        SYS_MemUnmap(&(pPipeline->sDeshadingGrid));
        SYS_MemUnmap(&(pPipeline->sDPFReadMap));
        /* in case it is already unmapped it has no effects, otherwise the
         * MMU is paused so nothing can access it anyway */
    }
    else
    {
        if ( pPipeline->bStarted == IMG_TRUE )
        {
            CI_DEBUG("Stopping pipeline before destruction\n");
            ret = KRN_CI_PipelineStopCapture(pPipeline);
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to stop the capture before destroying it!\n");
                ///@ what should we do? return?
            }
        }
    }

    /*
     * clean used device memory
     */
    CI_DEBUG("clean LSH\n");
    if (pPipeline->sDeshadingGrid.uiAllocated > 0)
    {
        ret = SYS_MemFree(&(pPipeline->sDeshadingGrid));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free lens shading device memory "\
                "(returned %d)\n", ret);
        }
        // this is not critical because the memory may not have been allocated
    }

    if (pPipeline->pLoadStructStamp)
    {
        IMG_FREE(pPipeline->pLoadStructStamp);
    }

    // DPF clean
    CI_DEBUG("clean DPF\n");
    SYS_MemFree(&(pPipeline->sDPFReadMap));
    /*
     * clean internal objects
     */

    ret = SYS_LockDestroy(&(pPipeline->sListLock));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to delete the avaialble lock\n");
    }

    ret = SYS_SemDestroy(&(pPipeline->sProcessedSem));
    if (ret)  // unlikely
    {
        CI_FATAL("failed to delete the semaphore\n");
    }

#ifdef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    if ( SYS_SpinlockDestroy(&(pPipeline->sProcessingSpinlock)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to delete the processed spinlock\n");
    }
#endif

    /* should clear the lists as well but the cells objects are not allocated
     * so it's not necessary */
    CI_DEBUG("clean available shots\n");
    pFound = List_visitor(&(pPipeline->sList_available), NULL,
        &ListDestroy_KRN_CI_Shot);
    if (pFound)
    {
        CI_FATAL("failed to visit the whole available list to clean it\n");
    }

    CI_DEBUG("clean sent shots\n");
    pFound = List_visitor(&(pPipeline->sList_sent), NULL,
        &ListDestroy_KRN_CI_Shot);
    if (pFound)
    {
        CI_FATAL("failed to visit the whole sent list to clean it\n");
    }

    CI_DEBUG("clean processed shots\n");
    pFound = List_visitor(&(pPipeline->sList_processed), NULL,
        &ListDestroy_KRN_CI_Shot);
    if (pFound)
    {
        CI_FATAL("failed to visit the whole processed list to clean it\n");
    }

    CI_DEBUG("clean pending shots\n");
    pFound = List_visitor(&(pPipeline->sList_pending), NULL,
        &ListDestroy_KRN_CI_Shot);
    if (pFound)
    {
        CI_FATAL("failed to visit the whole pending list to clean it\n");
    }

    CI_DEBUG("clean available buffer list\n");
    pFound = List_visitor(&(pPipeline->sList_availableBuffers), NULL,
        &ListDestroy_KRN_CI_Buffer);
    if (pFound)
    {
        CI_FATAL("failed to visit the whole buffer list to clean it\n");
    }

    // clean heaps at last
    for ( i = 0 ; i < CI_N_HEAPS ; i++ )
    {
        if ( pPipeline->apHeap[i] != NULL )
        {
            CI_DEBUG("destroy virt heap %d (%"IMG_SIZEPR"u bytes)\n",
                i, g_psCIDriver->sMMU.aHeapSize[i]);
            IMGMMU_HeapDestroy(pPipeline->apHeap[i]);
            pPipeline->apHeap[i] = NULL;
        }
    }

    /*
     * clean the user copy
     */
    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        if ( pPipeline->userPipeline.sDeshading.matrixDiff[i] )
        {
            CI_DEBUG("user-copy of LSH.matrixDiff[%d] != NULL - some "\
                "copy_to_user did not clean after itself!\n", i);
        }
        if ( pPipeline->userPipeline.sDeshading.matrixStart[i] )
        {
            CI_DEBUG("user-copy of LSH.matrixStart[%d] != NULL - some "\
                "copy_to_user did not clean after itself!\n", i);
        }
    }
    if ( pPipeline->userPipeline.sDefectivePixels.sInput.apDefectInput )
    {
        CI_DEBUG("user-copy of DPF.input != NULL - some copy_to_user "\
            "did not clean after itself!\n");
    }

    IMG_FREE(pPipeline);

    return IMG_SUCCESS;
}

