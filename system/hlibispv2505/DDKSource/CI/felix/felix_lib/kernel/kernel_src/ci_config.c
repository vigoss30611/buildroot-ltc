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
#include <linux/fr.h>

/*-----------------------------------------------------------------------------
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
    KRN_CI_BufferClear(pBuff);
    IMG_FREE(pBuff);
    return IMG_TRUE;
}

IMG_BOOL8 ListDestroy_KRN_CI_LSH_Matrix(void *elem, void *param)
{
    KRN_CI_BUFFER *pBuff = (KRN_CI_BUFFER *)elem;
    KRN_CI_LSH_MATRIX *pMat = container_of(pBuff, KRN_CI_LSH_MATRIX, sBuffer);
    KRN_CI_BufferClear(pBuff);
    IMG_FREE(pMat);
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

IMG_BOOL8 KRN_CI_PipelineVerify(const CI_PIPELINE_CONFIG *pPipeline,
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

#ifdef BRN58117
    {
        /* BRN58117: if DE point 2 is the only one active and memory is
         * stalled HW can lock up. Work arround is to enable statistics
         * output later on.
         * The mask is the stats that do not solve the problem */
        static const int config_mask = CI_SAVE_EXS_GLOBAL | CI_SAVE_EXS_REGION
            | CI_SAVE_CRC
            | CI_SAVE_TIMESTAMP;

        CI_PIPELINE *pCIPipeline =
            container_of(pPipeline, CI_PIPELINE, config);

        if (CI_INOUT_FILTER_LINESTORE == pPipeline->eDataExtraction
            && PXL_NONE != pPipeline->eDispType.eFmt
            && TYPE_BAYER == pPipeline->eDispType.eBuffer
            && PXL_NONE == pPipeline->eEncType.eFmt
            && PXL_NONE == pPipeline->eRaw2DExtraction.eFmt
            && PXL_NONE == pPipeline->eHDRExtType.eFmt
            && 0 == (pCIPipeline->sDefectivePixels.\
            eDPFEnable & CI_DPF_WRITE_MAP_ENABLED)
            && 0 == (pPipeline->eOutputConfig&(~config_mask))
            && 0 == pCIPipeline->stats.sWhiteBalanceStats.ui8ActiveROI)
        {
            CI_FATAL("BRN58117 DE point 2 enabled without stats may lock-up" \
                "the HW - enable stats after FLS\n");
            return IMG_FALSE;
        }
    }
#endif /* BRN58117 */
    return IMG_TRUE;
}

IMG_RESULT KRN_CI_PipelineInit(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_CONNECTION *pConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT( pPipeline != NULL );
    IMG_ASSERT(pConnection != NULL );
    IMG_ASSERT( g_psCIDriver != NULL );

    SYS_LockInit(&(pPipeline->sListLock));
    List_init(&(pPipeline->sList_available));
    List_init(&(pPipeline->sList_sent));

    SYS_SemInit(&(pPipeline->sProcessedSem), 0);
    List_init(&(pPipeline->sList_processed));
    List_init(&(pPipeline->sList_pending));
    List_init(&(pPipeline->sList_availableBuffers));
    List_init(&(pPipeline->sList_matrixBuffers));
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
                        pPipeline->pipelineConfig.ui8Context, offset, i);
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
    pPipeline->pMatrixUsed = NULL;

    // if supporting tiling setup the tiling alignment
    if (pPipeline->pipelineConfig.bSupportTiling)
    {
        IMG_UINT32 uiMaxTileStride = 0;
        CI_SHOT config;
#ifdef FELIX_FAKE
        char message[256];
#endif

        config.bEncTiled = pPipeline->pipelineConfig.bSupportTiling;
        config.bDispTiled = pPipeline->pipelineConfig.bSupportTiling;
        config.bHDRExtTiled = pPipeline->pipelineConfig.bSupportTiling;

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
        if (g_psCIDriver->uiTilingStride > 0)
        {
            if (uiMaxTileStride > g_psCIDriver->uiTilingStride)
            {
                CI_FATAL("computed tiled stride %uB does not meet insmod "\
                    "tiling stride of %uB\n",
                    uiMaxTileStride, g_psCIDriver->uiTilingStride);
                KRN_CI_PipelineDestroy(pPipeline);
                return IMG_ERROR_NOT_SUPPORTED;
            }
            uiMaxTileStride = g_psCIDriver->uiTilingStride;
        }

        pPipeline->uiTiledStride = uiMaxTileStride;
        pPipeline->uiTiledAlignment =
            KRN_CI_MMUTilingAlignment(uiMaxTileStride);

#ifdef FELIX_FAKE
        sprintf(message, "Compute tiled stride for Context %d: Y=0x%x "\
            "CbCr=0x%x Disp=0x%x HDRExt=0x%x - max=0x%x virt_algin=0x%x",
            pPipeline->pipelineConfig.ui8Context,
            config.aEncYSize[0], config.aEncCbCrSize[0], config.aDispSize[0],
            config.aHDRExtSize[0], pPipeline->uiTiledStride,
            pPipeline->uiTiledAlignment);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#endif
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineConfigUpdate(KRN_CI_PIPELINE *pPipeline,
    const struct CI_PIPE_INFO *pPipeInfo)
{
    IMG_RESULT ret;
    IMG_UINT32 width;  // IIF width to use with ENS output size

    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pPipeInfo);

    // copy information from the pipeInfo
    IMG_MEMCPY(&(pPipeline->pipelineConfig), &(pPipeInfo->config),
        sizeof(CI_PIPELINE_CONFIG));
    IMG_MEMCPY(&(pPipeline->iifConfig), &(pPipeInfo->iifConfig),
        sizeof(CI_MODULE_IIF));
    IMG_MEMCPY(&(pPipeline->ensConfig), &(pPipeInfo->ensConfig),
        sizeof(CI_MODULE_ENS));
    IMG_MEMCPY(&(pPipeline->lshConfig), &(pPipeInfo->lshConfig),
        sizeof(CI_MODULE_LSH));
    IMG_MEMCPY(&(pPipeline->dpfConfig), &(pPipeInfo->dpfConfig),
        sizeof(CI_MODULE_DPF));
    IMG_MEMCPY(&(pPipeline->awsConfig), &(pPipeInfo->sAWSCurveCoeffs),
        sizeof(CI_AWS_CURVE_COEFFS));

    // if supporting tiling setup the tiling alignment
    if (pPipeline->pipelineConfig.bSupportTiling)
    {
        IMG_UINT32 uiMaxTileStride = 0;
        CI_SHOT config;
#ifdef FELIX_FAKE
        char message[256];
#endif
        config.bEncTiled = pPipeline->pipelineConfig.bSupportTiling;
        config.bDispTiled = pPipeline->pipelineConfig.bSupportTiling;
        config.bHDRExtTiled = pPipeline->pipelineConfig.bSupportTiling;

        // will compute the tiled stride afterwards!
        pPipeline->uiTiledStride = 0;
        ret = KRN_CI_BufferFromConfig(&config, pPipeline, IMG_FALSE);
        if (ret)
        {
            CI_FATAL("failed to compute sizes to allocate a buffer with "\
                "tiling support\n");
            return ret;
        }

        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aEncYSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aEncCbCrSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aDispSize[0]);
        uiMaxTileStride = IMG_MAX_INT(uiMaxTileStride, config.aHDRExtSize[0]);
        if (g_psCIDriver->uiTilingStride > 0)
        {
            if (uiMaxTileStride > g_psCIDriver->uiTilingStride)
            {
                CI_FATAL("computed tiled stride %uB does not meet insmod "\
                    "tiling stride of %uB\n",
                    uiMaxTileStride, g_psCIDriver->uiTilingStride);
                KRN_CI_PipelineDestroy(pPipeline);
                return IMG_ERROR_NOT_SUPPORTED;
            }
            uiMaxTileStride = g_psCIDriver->uiTilingStride;
        }

        pPipeline->uiTiledStride = uiMaxTileStride;
        pPipeline->uiTiledAlignment =
            KRN_CI_MMUTilingAlignment(uiMaxTileStride);

#if defined(FELIX_FAKE)
        sprintf(message, "Compute tiled stride for Context %d: Y=0x%x "\
            "CbCr=0x%x Disp=0x%x HDRExt=0x%x - max=0x%x virt_algin=0x%x",
            pPipeline->pipelineConfig.ui8Context,
            config.aEncYSize[0], config.aEncCbCrSize[0], config.aDispSize[0],
            config.aHDRExtSize[0], pPipeline->uiTiledStride,
            pPipeline->uiTiledAlignment);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#endif
    }

    width = (pPipeline->iifConfig.ui16ImagerSize[0] + 1) * 2;
    pPipeline->uiENSSizeNeeded = HW_CI_EnsOutputSize(
        pPipeline->ensConfig.ui8Log2NCouples, width);

    //  eagle DPF now in LL
    if ((pPipeline->dpfConfig.eDPFEnable | CI_DPF_READ_MAP_ENABLED))
    {
        /** @ a new method of updates for DPF should be found to avoid
         * having to do multi-level copy_from_user */
        // warning: the internal buffer is reserved when starting the capture!
        ret = INT_CI_PipelineSetDPFRead(pPipeline,
            pPipeline->dpfConfig.sInput.apDefectInput);
        if (ret)
        {
            CI_FATAL("failed to get the DPF read map - returned %d\n", ret);
            return ret;
        }
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
        if (pShot->hwLoadStructure.sMemory.uiAllocated > 0)
        {
            if (pPipeline->pConnection->iMemMapBase < 0)
            {
                // prevent overflow
                pPipeline->pConnection->iMemMapBase = 0;
            }
            pShot->hwLoadStructure.ID =
                (++(pPipeline->pConnection->iMemMapBase));
        }

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

    if( pParam->bTiled && !pPipeline->pipelineConfig.bSupportTiling )
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
            + pPipeline->pipelineConfig.ui8Context];
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
        Width = pPipeline->pipelineConfig.ui16MaxEncOutWidth;
        Height = pPipeline->pipelineConfig.ui16MaxEncOutHeight;
#endif //INFOTM_ISP
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "allocate encoder output");
        break;

    case CI_ALLOC_DISP:
        // should be also fine for packed YUV444
        ui32Size = config.aDispSize[0] * config.aDispSize[1];
        bufferType = CI_BUFF_DISP;
#ifdef INFOTM_ISP
        sprintf(szName, "Disp(%d)", DispID++);
        Width = pPipeline->pipelineConfig.ui16MaxDispOutWidth;
        Height = pPipeline->pipelineConfig.ui16MaxDispOutHeight;
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
        CI_FATAL("given size %uB is too small to support the given format "\
            "(need %uB)!\n", pParam->uiSize, ui32Size);
        *ret = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }

#if defined(INFOTM_ISP)
    if (pParam->type == CI_ALLOC_ENC && pPipeline->pipelineConfig.bIsLinkMode)
    {
        pParam->uiSize = pPipeline->pipelineConfig.ui32LinkEncBufSize;
    }
#endif //INFOTM_ISP

#if 0//def INFOTM_ENABLE_ISP_DEBUG
    printk("@@@ CreateBuf   : Name=%s, ID=%d, Type=%d, Size=%d, WxH=[%d x %d]\n",\
                szName, pParam->fd, pParam->type, pParam->uiSize, Width, Height);
#endif //INFOTM_ENABLE_ISP_DEBUG

    pBuffer = (KRN_CI_BUFFER*)IMG_CALLOC(1, sizeof(KRN_CI_BUFFER));
    if (!pBuffer)
    {
        CI_FATAL("Failed to allocate internal buffer structure "\
            "(%"IMG_SIZEPR"d Bytes)\n", sizeof(KRN_CI_BUFFER));
        *ret = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    pBuffer->sCell.object = pBuffer;
    // allocate the buffer
#ifdef INFOTM_ISP
    *ret = KRN_CI_BufferInit(pBuffer, pParam->uiSize, pHeap, uiVirtAlignment,
        pParam->fd, szName);
#else
    *ret = KRN_CI_BufferInit(pBuffer, pParam->uiSize, pHeap, uiVirtAlignment,
        pParam->fd);
#endif //INFOTM_ISP
    if (*ret)
    {
        CI_FATAL("Failed to allocate the buffer\n");
        IMG_FREE(pBuffer);
        pBuffer = NULL;
    }
    else
    {
        if(pParam->type == CI_ALLOC_ENC) {
            SYS_MEM sMemory = pBuffer->sMemory;
            struct SYS_MEM_ALLOC {
                struct fr *fr;
                /**
                 * created at allocation time using vmalloc or kmalloc according to its
                 * size
                 */
                IMG_UINT64 *physList;
            };
            struct fr* fr = ((struct SYS_MEM_ALLOC *)(sMemory.pAlloc))->fr;
            //printk("Addr: 0x%x, Y, x-y: %d-%d, UV, x-y: %d-%d======\n", fr->ring->virt_addr, config.aEncYSize[0],config.aEncYSize[1], config.aEncCbCrSize[0],config.aEncCbCrSize[1]);
            memset(fr->ring->virt_addr + config.aEncYSize[0] * config.aEncYSize[1], 0x80, config.aEncCbCrSize[0]*config.aEncCbCrSize[1]);
        }
        pBuffer->pPipeline = pPipeline;
        pBuffer->type = bufferType;
        pBuffer->bTiled = pParam->bTiled;
    }

    return pBuffer;
}

KRN_CI_BUFFER* KRN_CI_PipelineCreateLSHBuffer(KRN_CI_PIPELINE *pPipeline,
    struct CI_ALLOC_PARAM *pParam, IMG_RESULT *ret)
{
    struct MMUHeap *pHeap = NULL;
    KRN_CI_LSH_MATRIX *pMatrix = NULL;
    int bufferType = CI_BUFF_LSH_IN;
    IMG_UINT32 uiVirtAlignment = 0;

    IMG_ASSERT(pPipeline != NULL);
    IMG_ASSERT(pParam != NULL);
    IMG_ASSERT(ret != NULL);
    IMG_ASSERT(pPipeline->pConnection);

    pHeap = pPipeline->apHeap[CI_DATA_HEAP];

    if (0 == pParam->uiSize || pParam->bTiled || 0 != pParam->fd)
    {
        CI_FATAL("given size is 0 or asked for tiled LSH\n");
        return NULL;
    }

    pMatrix = (KRN_CI_LSH_MATRIX *)IMG_CALLOC(1, sizeof(KRN_CI_LSH_MATRIX));
    if (!pMatrix)
    {
        CI_FATAL("failed to allocate internal structure\n");
        return NULL;
    }

    pMatrix->sBuffer.sCell.object = &(pMatrix->sBuffer);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "allocate LSH matrix");
#ifdef INFOTM_ISP
    *ret = KRN_CI_BufferInit(&(pMatrix->sBuffer), pParam->uiSize, pHeap,
        uiVirtAlignment, pParam->fd, "LSH_matrix");
#else
    *ret = KRN_CI_BufferInit(&(pMatrix->sBuffer), pParam->uiSize, pHeap,
        uiVirtAlignment, pParam->fd);
#endif //INFOTM_ISP
    if (*ret)
    {
        CI_FATAL("Failed to allocate the buffer\n");
        IMG_FREE(pMatrix);
        return NULL;
    }
    else
    {
        pMatrix->sBuffer.pPipeline = pPipeline;
        pMatrix->sBuffer.type = bufferType;
        pMatrix->sBuffer.bTiled = IMG_FALSE;
    }

    return &(pMatrix->sBuffer);
}

IMG_RESULT KRN_CI_PipelineAddBuffer(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_BUFFER *pBuffer)
{
    IMG_RESULT ret;

    IMG_ASSERT(pPipeline != NULL);
    IMG_ASSERT(pPipeline->pConnection != NULL);
    IMG_ASSERT(pBuffer != NULL);

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
        ret = List_pushBack(&(pPipeline->pConnection->sList_unmappedBuffers),
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
    KRN_CI_BufferClear(pBuffer);
    IMG_FREE(pBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_PipelineDeregisterLSHBuffer(KRN_CI_PIPELINE *pPipeline,
    IMG_UINT32 id)
{
    sCell_T *pFound = NULL;
    KRN_CI_LSH_MATRIX *pMatrix = NULL;

    IMG_ASSERT(pPipeline != NULL);

    // because stored using KRN_CI_LSH_MATRIX::KRN_CI_BUFFER::sCell
    // if the matrix list needs a lock proction lock here
    pFound = List_visitor(&(pPipeline->sList_matrixBuffers),
        (void*)&id, &ListSearch_KRN_CI_Buffer);
    if (pFound)
    {
        KRN_CI_BUFFER *pMatrixBuffer =
            container_of(pFound, KRN_CI_BUFFER, sCell);
        pMatrix =
            container_of(pMatrixBuffer, KRN_CI_LSH_MATRIX, sBuffer);

        if (pMatrixBuffer->used > 0)
        {
            CI_FATAL("matrix is in use - cannot deregister\n");
            pMatrix = NULL;
        }
        else
        {
            List_detach(pFound);
        }
    }
    else
    {
        CI_WARNING("No such LSH buffer registered in CI kernel-side");
    }
    // unlock here

    if (NULL == pMatrix)
    {
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    if (pPipeline->pMatrixUsed == pMatrix)
    {
        pPipeline->pMatrixUsed = NULL;
    }

    KRN_CI_BufferClear(&(pMatrix->sBuffer));
    IMG_FREE(pMatrix);

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
    pDir = g_psCIDriver->sMMU.apDirectory[pPipeline->pipelineConfig.ui8Context];

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
                pShot->phwEncoderOutput ? &pShot->phwEncoderOutput->sMemory : NULL,
                pShot->phwDisplayOutput ? &pShot->phwDisplayOutput->sMemory : NULL,
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
    IMG_ASSERT(pPipeline->pipelineConfig.ui8Context < CI_N_CONTEXT);

    pDir = g_psCIDriver->sMMU.apDirectory[pPipeline->pipelineConfig.ui8Context];

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
            KRN_CI_BufferClear(pBuffer);
            IMG_FREE(pBuffer);
            return IMG_ERROR_FATAL;
        }

#ifdef BRN48951
        /* BRN48951 recomends SW to flush the cache for every buffer mapping */
        KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU),
            pPipeline->pipelineConfig.ui8Context, IMG_TRUE);
#endif /* BRN48951 */
    }

    if (CI_BUFF_LSH_IN == pBuffer->type)
    {
        ret = List_pushBack(&(pPipeline->sList_matrixBuffers), 
            &(pBuffer->sCell));
    }
    else
    {
        SYS_LockAcquire(&(pPipeline->sListLock));
        {
            ret = List_pushBack(&(pPipeline->sList_availableBuffers),
                &(pBuffer->sCell));
        }
        SYS_LockRelease(&(pPipeline->sListLock));
    }

    return ret;
}

static IMG_RESULT IMG_CI_PipelineUpdateSubmitted(KRN_CI_PIPELINE *pPipeline)
{
    sCell_T *pHeadCell = NULL, *pCurrentCell = NULL;
    KRN_CI_SHOT *pCurrent = NULL;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    SYS_LockAcquire(&(pPipeline->sListLock));
    {
        pHeadCell = List_getHead(&(pPipeline->sList_pending));
        // apply new configuration from TAIL
        pCurrentCell = List_getTail(&(pPipeline->sList_pending));
    }
    SYS_LockRelease(&(pPipeline->sListLock));

    /* apply the new configuration from the TAIL to the cell before the head
     * (the head is submitted to HW and most likely fetched already) */
    while ( pCurrentCell && pCurrentCell != pHeadCell )
    {
        pCurrent = (KRN_CI_SHOT*)pCurrentCell->object;

        HW_CI_ShotUpdateSubmittedLS(pCurrent);

        SYS_LockAcquire(&(pPipeline->sListLock));
        {
            // in case the head changed
            pHeadCell = List_getHead(&(pPipeline->sList_pending));
            pCurrentCell = List_getPrev(pCurrentCell);
        }
        SYS_LockRelease(&(pPipeline->sListLock));
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

    if (bRegAccess)
    {
        ret = HW_CI_Reg_LSH_DS(pPipeline);
        if ( IMG_SUCCESS != ret )
        {
            CI_FATAL("failed to update the LSH registers\n");
            return ret;
        }

        HW_CI_Reg_DPF(pPipeline);

        HW_CI_Reg_AWS(pPipeline);
    }

    if (!bRegAccess && bASAP)
    {
        IMG_CI_PipelineUpdateSubmitted(pPipeline);  // cannot fail
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

    return ret;
}

IMG_RESULT KRN_CI_PipelineUpdateMatrix(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_LSH_MATRIX *pMatrix)
{
    IMG_ASSERT(pPipeline);

    // when starting this is called to update the matrix
    if (pPipeline->pMatrixUsed && pPipeline->pMatrixUsed != pMatrix)
    {
        if (pPipeline->bStarted)
        {
            pPipeline->pMatrixUsed->sBuffer.used--;
            // assumes LSH matrix can be used only by 1 context
            IMG_ASSERT(pPipeline->pMatrixUsed->sBuffer.used == 0);
        }
        CI_DEBUG("LSH matrix %d not used anymore\n",
            pPipeline->pMatrixUsed->sBuffer.ID);
    }
    pPipeline->pMatrixUsed = pMatrix;  // may set to NULL

    if (pPipeline->bStarted)
    {
        // should update only the address as the config is the same
        HW_CI_Reg_LSH_Matrix(pPipeline);
        if (pPipeline->pMatrixUsed)
        {
            pPipeline->pMatrixUsed->sBuffer.used++;
            // we expect to have only 1 context using a matrix
            IMG_ASSERT(pPipeline->pMatrixUsed->sBuffer.used == 1);
            /* update the config stamp for the LSH_ALIGNMENT_X is not
            * needed because the configuration did not change
            * but we do it in case a new matrix is loaded when no frames
            * were pushed yet */
            HW_CI_Load_LSH_Matrix(pPipeline->pLoadStructStamp,
                &(pPipeline->pMatrixUsed->sConfig));
            CI_DEBUG("LSH matrix %d configured in HW\n",
                pPipeline->pMatrixUsed->sBuffer.ID);
        }
        else
        {
            CI_DEBUG("LSH matrix disabled in HW\n");
        }
    }
    return IMG_SUCCESS;
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

    /* usually access to LSH matrix is the same than buffers because it is
     * stored using KRN_CI_LSH_MATRIX::KRN_CI_BUFFER::sCell but here we want
     * to destroy the structure so we need a specific function */
    CI_DEBUG("clean LSH matrix list\n");
    pFound = List_visitor(&(pPipeline->sList_matrixBuffers), NULL,
        &ListDestroy_KRN_CI_LSH_Matrix);
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

    IMG_FREE(pPipeline);

    return IMG_SUCCESS;
}

