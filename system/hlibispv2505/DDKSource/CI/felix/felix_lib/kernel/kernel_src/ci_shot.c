/**
 ******************************************************************************
 @file ci_shot.c

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

 *****************************************************************************/

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "ci/ci_api_structs.h"
#include "ci_kernel/ci_kernel.h"

#include "ci_kernel/ci_hwstruct.h"
#include <felixcommon/ci_alloc_info.h>

#include <tal.h>
#include <reg_io2.h>

/*-----------------------------------------------------------------------------
 * these functions have static scope
 *---------------------------------------------------------------------------*/

static void IMG_CI_ShotFreeBuffers(KRN_CI_SHOT *pBuffer)
{
    IMG_RESULT ret;

    if (pBuffer->phwEncoderOutput)
    {
        KRN_CI_BufferClear(pBuffer->phwEncoderOutput);
        IMG_FREE(pBuffer->phwEncoderOutput);
        pBuffer->phwEncoderOutput = NULL;
    }
    if (pBuffer->phwDisplayOutput)
    {
        KRN_CI_BufferClear(pBuffer->phwDisplayOutput);
        IMG_FREE(pBuffer->phwDisplayOutput);
        pBuffer->phwDisplayOutput = NULL;
    }
    if (pBuffer->phwHDRExtOutput)
    {
        KRN_CI_BufferClear(pBuffer->phwHDRExtOutput);
        IMG_FREE(pBuffer->phwHDRExtOutput);
        pBuffer->phwHDRExtOutput = NULL;
    }
    if (pBuffer->phwRaw2DExtOutput)
    {
        KRN_CI_BufferClear(pBuffer->phwRaw2DExtOutput);
        IMG_FREE(pBuffer->phwRaw2DExtOutput);
        pBuffer->phwRaw2DExtOutput = NULL;
    }

    // these buffers should always exist
    if (pBuffer->hwSave.sMemory.uiAllocated > 0)
    {
        CI_DEBUG("Freeing hwSave...\n");
        ret = SYS_MemFree(&(pBuffer->hwSave.sMemory));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free the HW Save device memory\n");
        }
        CI_DEBUG("Freeing hwSave DONE.\n");
    }

    if (pBuffer->hwLoadStructure.sMemory.uiAllocated > 0)
    {
        CI_DEBUG("Freeing hwLoadStructure...\n");
        ret = SYS_MemFree(&(pBuffer->hwLoadStructure.sMemory));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free the HW Configuration device memory\n");
        }
        CI_DEBUG("Freeing hwLoadStructure DONE.\n");
    }

    if (pBuffer->hwLinkedList.uiAllocated > 0)
    {
        CI_DEBUG("Freeing hwLinkedList...\n");
        ret = SYS_MemFree(&(pBuffer->hwLinkedList));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free the HW Pointers device memory\n");
        }
        CI_DEBUG("Freeing hwLinkedList DONE.\n");
    }

    if (pBuffer->sDPFWrite.sMemory.uiAllocated > 0)
    {
        CI_DEBUG("Freeing DPF...\n");
        ret = SYS_MemFree(&(pBuffer->sDPFWrite.sMemory));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free the DPF output map device memory\n");
        }
        CI_DEBUG("Freeing DPF DONE.\n");
    }

    if (pBuffer->sENSOutput.sMemory.uiAllocated > 0)
    {
        CI_DEBUG("Freeing ENS...\n");
        ret = SYS_MemFree(&(pBuffer->sENSOutput.sMemory));
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to free the ENS output device memory\n");
        }
        CI_DEBUG("Freeing ENS DONE.\n");
    }
}

/**
* @warning assumes that pBuffer->userShot already has ION FDs setup when using
* ION
*/
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_RESULT IMG_CI_ShotMallocBuffers(KRN_CI_SHOT *pBuffer)
{
    IMG_RESULT ret;
    struct MMUHeap *pHeap = NULL;

#ifdef FELIX_FAKE
    char message[64];
#endif
#ifdef INFOTM_ISP
    char szName[20];
    static int count = 0;
#endif

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pBuffer->pPipeline);

    if (pBuffer->phwEncoderOutput
        || pBuffer->phwDisplayOutput
        || pBuffer->phwHDRExtOutput
        || pBuffer->phwRaw2DExtOutput
        || pBuffer->hwLinkedList.uiAllocated > 0
        || pBuffer->hwLoadStructure.sMemory.uiAllocated > 0
        || pBuffer->hwSave.sMemory.uiAllocated > 0)
    {
        CI_FATAL("HW Output Buffers, Context Pointers, Configuration or "\
            "Save memory is already allocated\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    /*
    * buffers that cannot be imported (using DATA heap)
    */
    pHeap = pBuffer->pPipeline->apHeap[CI_DATA_HEAP];

    /*
    * HW Save
    */
#ifdef FELIX_FAKE
    sprintf(message, "save struct (ctx %d)",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
    CI_DEBUG("save struct (ctx %d)\n",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
#endif
#ifdef INFOTM_ISP
    count++;
    sprintf(szName, "Save(%d)", count);
    ret = SYS_MemAlloc(&(pBuffer->hwSave.sMemory),
        HW_CI_SaveStructureSize(), pHeap, 0, szName);
#else
    ret = SYS_MemAlloc(&(pBuffer->hwSave.sMemory),
        HW_CI_SaveStructureSize(), pHeap, 0);
#endif
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to allocate the HW Save structure\n");
        goto shot_malloc_failed;
    }
    /* will be set once mapped to user-space; once all types are set the
     * Shot is considered available */
    //pBuffer->hwSave.type = CI_BUFF_STAT;
    pBuffer->userShot.statsSize = pBuffer->hwSave.sMemory.uiAllocated;

    /*
    * HW Config
    */
#ifdef FELIX_FAKE
    sprintf(message, "load struct (ctx %d)",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
    CI_DEBUG("load struct (ctx %d)\n",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
#endif

#ifdef INFOTM_ISP
    sprintf(szName, "Load(%d)", count);
    ret = SYS_MemAlloc(&(pBuffer->hwLoadStructure.sMemory),
        HW_CI_LoadStructureSize(), pHeap, 0, szName);
#else
    ret = SYS_MemAlloc(&(pBuffer->hwLoadStructure.sMemory),
        HW_CI_LoadStructureSize(), pHeap, 0);
#endif //INFOTM_ISP
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to allocate the HW Configuration structure\n");
        goto shot_malloc_failed;
    }
    /* will be set once mapped to user-space; once all types are set the
    * Shot is considered available */
    //pBuffer->hwSave.type = CI_BUFF_LOAD;
    pBuffer->userShot.loadSize = pBuffer->hwLoadStructure.sMemory.uiAllocated;

    /* the memory is copied from the Pipeline's config so there is no need to
     * set it up */

    /*
    * HW Context pointers
    */
#ifdef FELIX_FAKE
    sprintf(message, "linked list struct (ctx %d)",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
    CI_DEBUG("linked list struct (ctx %d)\n",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
#endif
#ifdef INFOTM_ISP
    sprintf(szName, "LinkedList(%d)", count);
    ret = SYS_MemAlloc(&(pBuffer->hwLinkedList),
        HW_CI_LinkedListSize(), pHeap, 0, szName);
#else
    ret = SYS_MemAlloc(&(pBuffer->hwLinkedList),
        HW_CI_LinkedListSize(), pHeap, 0);
#endif //INFOTM_ISP
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to allocate the HW Context structure\n");
        goto shot_malloc_failed;
    }

    /*
    * DPF Write map
    */
#ifdef FELIX_FAKE
    sprintf(message, "DPF write map (ctx %d)",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
    CI_DEBUG("DPF write map (ctx %d)\n",
        pBuffer->pPipeline->pipelineConfig.ui8Context);
#endif

#ifdef INFOTM_ISP
    if ((pBuffer->pPipeline->dpfConfig.eDPFEnable & CI_DPF_WRITE_MAP_ENABLED)
        && (pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize > 0))
#else
    if (pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize > 0)
#endif
    {
#ifdef INFOTM_ISP
        sprintf(szName, "DPFWrite(%d)", count);
        ret = SYS_MemAlloc(&(pBuffer->sDPFWrite.sMemory),
            pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize, pHeap, 0, szName);
#else
        ret = SYS_MemAlloc(&(pBuffer->sDPFWrite.sMemory),
            pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize, pHeap, 0);
#endif //INFOTM_ISP
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to alloc DPF output map buffer\n");
            goto shot_malloc_failed;
        }
#ifdef INFOTM_ISP
        /* will be set once mapped to user-space; once all types are set the Shot
         * is considered available */
        //pBuffer->sDPFWrite.type = CI_BUFF_DPF_WR;
        pBuffer->userShot.uiDPFSize =
            pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize;
#endif
    }
#ifdef INFOTM_ISP
    else
    {
        pBuffer->userShot.uiDPFSize = 0;
    }
#else
    /* will be set once mapped to user-space; once all types are set the Shot
     * is considered available */
    //pBuffer->sDPFWrite.type = CI_BUFF_DPF_WR;
    pBuffer->userShot.uiDPFSize =
        pBuffer->pPipeline->pipelineConfig.ui32DPFWriteMapSize;
#endif

    /*
     * ENS output buffer
     */
    if ((pBuffer->pPipeline->pipelineConfig.eOutputConfig & CI_SAVE_ENS) != 0)
    {
#ifdef FELIX_FAKE
        sprintf(message, "ENS output (ctx %d)",
            pBuffer->pPipeline->pipelineConfig.ui8Context);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
        CI_DEBUG("ENS output (ctx %d)\n",
            pBuffer->pPipeline->pipelineConfig.ui8Context);
#endif
        pBuffer->userShot.uiENSSize =
            HW_CI_EnsOutputSize(
            pBuffer->pPipeline->ensConfig.ui8Log2NCouples,
            (pBuffer->pPipeline->iifConfig.ui16ImagerSize[1] + 1) * 2);
        IMG_ASSERT(pBuffer->userShot.uiENSSize > 0);

#ifdef INFOTM_ISP
        sprintf(szName, "ENSOutput(%d)", count);
        ret = SYS_MemAlloc(&(pBuffer->sENSOutput.sMemory),
            pBuffer->userShot.uiENSSize, pHeap, 0, szName);
#else
        ret = SYS_MemAlloc(&(pBuffer->sENSOutput.sMemory),
            pBuffer->userShot.uiENSSize, pHeap, 0);
#endif //INFOTM_ISP
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to alloc ENS output\n");
            goto shot_malloc_failed;
        }

        /* will be set once mapped to user-space; once all types are set the
         * Shot is considered available */
        //pBuffer->sENSOutput.type = CI_BUFF_ENS_OUT;
    }
    else
    {
        pBuffer->userShot.uiENSSize = 0;
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "- done");

    return ret;

shot_malloc_failed:
    IMG_CI_ShotFreeBuffers(pBuffer);
    return IMG_ERROR_MALLOC_FAILED;
}

/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_Shot functions group
 *---------------------------------------------------------------------------*/

#ifdef INFOTM_ISP
IMG_RESULT KRN_CI_BufferInit(KRN_CI_BUFFER *pBuffer, IMG_UINT32 ui32Size,
    struct MMUHeap *pHeap, IMG_UINT32 uiVirtualAlignment, IMG_UINT32 buffId, char* pszName)
#else
IMG_RESULT KRN_CI_BufferInit(KRN_CI_BUFFER *pBuffer, IMG_UINT32 ui32Size,
    struct MMUHeap *pHeap, IMG_UINT32 uiVirtualAlignment, IMG_UINT32 buffId)
#endif //INFOTM_ISP
{
    IMG_RESULT ret;
    IMG_UINT32 ui32ToAllocate = ui32Size;

    IMG_ASSERT(CI_ALLOC_GetSysmemAlignment() == SYSMEM_ALIGNMENT);
    IMG_ASSERT(CI_ALLOC_GetMinTilingStride() >= MMU_MIN_TILING_STR);
    IMG_ASSERT(CI_ALLOC_GetMaxTilingStride() <= MMU_MAX_TILING_STR);

    if (ui32Size > 0)
    {
        if (0 == buffId)
        {
#ifdef INFOTM_ISP
            ret = SYS_MemAlloc(&(pBuffer->sMemory), ui32ToAllocate, pHeap,
                uiVirtualAlignment, pszName);
#else
            ret = SYS_MemAlloc(&(pBuffer->sMemory), ui32ToAllocate, pHeap,
                uiVirtualAlignment);
#endif //INFOTM_ISP
            if (ret)
            {
                CI_FATAL("failed to allocate memory on the device\n");
                return ret;
            }
        }
        else
        {
#ifdef INFOTM_ISP
            ret = SYS_MemImport(&(pBuffer->sMemory), ui32ToAllocate, pHeap,
                uiVirtualAlignment, buffId, pszName);
#else
            ret = SYS_MemImport(&(pBuffer->sMemory), ui32ToAllocate, pHeap,
                uiVirtualAlignment, buffId);
#endif //INFOTM_ISP
            if (ret)
            {
                CI_FATAL("failed to allocate memory on the device\n");
                return ret;
            }
            if (pBuffer->sMemory.uiAllocated < ui32ToAllocate)
            {
                CI_FATAL("buffer to import is %"IMG_SIZEPR"u B but buffer "\
                    "needs %u B\n", pBuffer->sMemory.uiAllocated,
                    ui32ToAllocate);
                SYS_MemFree(&(pBuffer->sMemory));
                return IMG_ERROR_FATAL;
            }
        }

#ifdef DEBUG_MEMSET_IMG_BUFFERS
        {
            void *ptr = Platform_MemGetMemory(&(pBuffer->sMemory));
            CI_DEBUG("memset output\n");

            IMG_MEMSET(ptr, 42, ui32ToAllocate); // 0x2A

            Platform_MemUpdateDevice(&(pBuffer->sMemory));
        }
#endif
        return IMG_SUCCESS;
    }

    CI_FATAL("Cannot allocate a buffer of size 0\n");
    return IMG_ERROR_FATAL;
}


void KRN_CI_BufferClear(KRN_CI_BUFFER *pBuff)
{
    if (pBuff->sMemory.pMapping != NULL)
    {
        SYS_MemUnmap(&(pBuff->sMemory));
    }
    SYS_MemFree(&(pBuff->sMemory));
}

IMG_RESULT KRN_CI_BufferFromConfig(CI_SHOT *pUserShot,
    const KRN_CI_PIPELINE *pPipeline, IMG_BOOL8 bScaleOutput)
{
    const CI_PIPELINE_CONFIG *pUserConfig = &(pPipeline->pipelineConfig);
    struct CI_TILINGINFO sTilingInfo;
    struct CI_SIZEINFO sSizeInfo;
    IMG_RESULT ret = IMG_ERROR_FATAL;
    IMG_BOOL8 bTiling = IMG_FALSE;
    IMG_UINT32 sensorWidth =
        (pPipeline->iifConfig.ui16ImagerSize[0] + 1)*CI_CFA_WIDTH;
    IMG_UINT32 sensorHeight =
        (pPipeline->iifConfig.ui16ImagerSize[1] + 1)*CI_CFA_HEIGHT;

    IMG_ASSERT(pUserShot);
    IMG_ASSERT(pPipeline);

    CI_ALLOC_GetTileInfo(g_psCIDriver->sMMU.uiTiledScheme, &sTilingInfo);

    //
    // HDR EXTRACTION OUTPUT (done first because biggest when tiled)
    //
    pUserShot->ui32HDRExtOffset = 0;
    if (TYPE_NONE != pUserConfig->eHDRExtType.eBuffer)
    {
        bTiling = pUserShot->bHDRExtTiled;
        IMG_MEMSET(&sSizeInfo, 0, sizeof(struct CI_SIZEINFO));

        // HW does not support another format at the moment
        IMG_ASSERT(pUserConfig->eHDRExtType.eFmt == BGR_101010_32);

        ret = CI_ALLOC_RGBSizeInfo(&(pUserConfig->eHDRExtType),
            sensorWidth, sensorHeight, bTiling ? &sTilingInfo : NULL,
            &sSizeInfo);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to configure HDR Extraction output sizes!\n");
            return ret;
        }

        pUserShot->aHDRExtSize[0] = sSizeInfo.ui32Stride;
        pUserShot->aHDRExtSize[1] = sSizeInfo.ui32Height;

        if (bTiling && 0 != pPipeline->uiTiledStride)
        {
            IMG_ASSERT(pPipeline->uiTiledStride >= pUserShot->aHDRExtSize[0]);
            pUserShot->aHDRExtSize[0] = pPipeline->uiTiledStride;
        }
    }
    else
    {
        pUserShot->aHDRExtSize[0] = 0;
        pUserShot->aHDRExtSize[1] = 0;
    }

    //
    // HDR INSERTION (cannot be tiled)
    //
    pUserShot->ui32HDRInsOffset = 0;
    if (TYPE_NONE != pUserConfig->eHDRInsType.eBuffer)
    {
        IMG_MEMSET(&sSizeInfo, 0, sizeof(struct CI_SIZEINFO));

        ret = CI_ALLOC_RGBSizeInfo(&(pUserConfig->eHDRInsType),
            sensorWidth, sensorHeight, NULL, &sSizeInfo);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to configure HDR Insertion sizes!\n");
            return ret;
        }

        pUserShot->aHDRInsSize[0] = sSizeInfo.ui32Stride;
        pUserShot->aHDRInsSize[1] = sSizeInfo.ui32Height;
    }
    else
    {
        pUserShot->aHDRInsSize[0] = 0;
        pUserShot->aHDRInsSize[1] = 0;
    }

    //
    // RAW 2D extraction (cannot be tiled)
    //
    pUserShot->ui32Raw2DOffset = 0;
    if (TYPE_NONE != pUserConfig->eRaw2DExtraction.eBuffer)
    {
        IMG_MEMSET(&sSizeInfo, 0, sizeof(struct CI_SIZEINFO));

        ret = CI_ALLOC_Raw2DSizeInfo(&(pUserConfig->eRaw2DExtraction),
            sensorWidth, sensorHeight, NULL, &sSizeInfo);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to configure RAW2D output sizes!\n");
            return ret;
        }

        pUserShot->aRaw2DSize[0] = sSizeInfo.ui32Stride;
        pUserShot->aRaw2DSize[1] = sSizeInfo.ui32Height;
    }
    else
    {
        pUserShot->aRaw2DSize[0] = 0;
        pUserShot->aRaw2DSize[1] = 0;
    }

    //
    // DISPLAY OUTPUT (both bayer or rgb)
    //
    pUserShot->ui32DispOffset = 0;
    if (TYPE_NONE != pUserConfig->eDispType.eBuffer)
    {
        IMG_UINT32 ui32Width = pUserConfig->ui16MaxDispOutWidth;
        IMG_UINT32 ui32Height = pUserConfig->ui16MaxDispOutHeight;

        bTiling = pUserShot->bDispTiled;
        IMG_MEMSET(&sSizeInfo, 0, sizeof(struct CI_SIZEINFO));

        // should be also fine for packed YUV444
        if (TYPE_RGB == pUserConfig->eDispType.eBuffer)
        {
            ret = CI_ALLOC_RGBSizeInfo(&(pUserConfig->eDispType),
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        } else if (TYPE_YUV == pUserConfig->eDispType.eBuffer)
        {
            ret = CI_ALLOC_YUVSizeInfo(&(pUserConfig->eDispType),
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        } else if (TYPE_BAYER == pUserConfig->eDispType.eBuffer)
        {
            ret = CI_ALLOC_RGBSizeInfo(&(pUserConfig->eDispType),
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        } else {
            CI_FATAL("Display buffer type not recognized = %d !\n", pUserConfig->eDispType.eBuffer);
            return ret;
        }

        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to configure Display output sizes!\n");
            return ret;
        }

        pUserShot->aDispSize[0] = sSizeInfo.ui32Stride;
        pUserShot->aDispSize[1] = sSizeInfo.ui32Height;

        if (bTiling && 0 != pPipeline->uiTiledStride)
        {
            IMG_ASSERT(pPipeline->uiTiledStride >= pUserShot->aDispSize[0]);
            pUserShot->aDispSize[0] = pPipeline->uiTiledStride;
        }
    }
    else
    {
        pUserShot->aDispSize[0] = 0;
        pUserShot->aDispSize[1] = 0;
    }

    //
    // ENCODER OUTPUT
    //
    pUserShot->aEncOffset[0] = 0;
    if (TYPE_NONE != pUserConfig->eEncType.eBuffer)
    {
        IMG_UINT32 ui32Width = pUserConfig->ui16MaxEncOutWidth;
        IMG_UINT32 ui32Height = pUserConfig->ui16MaxEncOutHeight;

        bTiling = pUserShot->bEncTiled;
        IMG_MEMSET(&sSizeInfo, 0, sizeof(struct CI_SIZEINFO));

        ret = CI_ALLOC_YUVSizeInfo(&(pUserConfig->eEncType),
            ui32Width, ui32Height,
            bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to configure YUV output sizes!\n");
            return ret;
        }

        pUserShot->aEncYSize[0] = sSizeInfo.ui32Stride;
        pUserShot->aEncYSize[1] = sSizeInfo.ui32Height;
        pUserShot->aEncCbCrSize[0] = sSizeInfo.ui32CStride;
        pUserShot->aEncCbCrSize[1] = sSizeInfo.ui32CHeight;

        if (bTiling && 0 != pPipeline->uiTiledStride)
        {
            IMG_ASSERT(pPipeline->uiTiledStride >= pUserShot->aEncYSize[0]);
            IMG_ASSERT(pPipeline->uiTiledStride >= pUserShot->aEncCbCrSize[0]);
            pUserShot->aEncYSize[0] = pPipeline->uiTiledStride;
            pUserShot->aEncCbCrSize[0] = pPipeline->uiTiledStride;
        }

        // assumes luma start first and chorma is just afterwards
        pUserShot->aEncOffset[1] =
            pUserShot->aEncYSize[0] * pUserShot->aEncYSize[1];
#if 0
        printk("@@@ BufCfg: ui32Width=%d, ui32Height=%d, aEncYSize[0]=%d, aEncYSize[1]=%d, aEncCbCrSize[0]=%d, aEncCbCrSize[1]=%d\n",
                    ui32Width,
                    ui32Height,
                    pUserShot->aEncYSize[0],
                    pUserShot->aEncYSize[1],
                    pUserShot->aEncCbCrSize[0],
                    pUserShot->aEncCbCrSize[1]);
#endif
    }
    else
    {
        pUserShot->aEncYSize[0] = 0;
        pUserShot->aEncYSize[1] = 0;
        pUserShot->aEncCbCrSize[0] = 0;
        pUserShot->aEncCbCrSize[1] = 0;
        //pUserShot->aEncOffset[0] = 0;
        pUserShot->aEncOffset[1] = 0;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_ShotConfigure(KRN_CI_SHOT *pShot, KRN_CI_PIPELINE *pPipeline)
{
    CI_SHOT *pUserShot = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pPipeline);

    if (pShot->pPipeline)
    {
        CI_FATAL("the shot already belong to a configuration\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    pUserShot = &(pShot->userShot);
    pShot->pPipeline = pPipeline;

    // allocates the internal parts of the shot
    ret = IMG_CI_ShotMallocBuffers(pShot);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to allocate the shot's HW buffers\n");
        pShot->pPipeline = NULL;
        return ret;
    }

    return IMG_SUCCESS;
}

KRN_CI_SHOT* KRN_CI_ShotCreate(IMG_RESULT *ret)
{
    KRN_CI_SHOT *pBuffer = NULL;

    IMG_ASSERT(ret);

    pBuffer = (KRN_CI_SHOT*)IMG_CALLOC(1, sizeof(KRN_CI_SHOT));
    if (!pBuffer)
    {
        CI_FATAL("allocation of a buffer failed (%lu B)\n",
            sizeof(KRN_CI_SHOT));
        *ret = IMG_ERROR_MALLOC_FAILED;
        return pBuffer;
    }

    /* the pointers and statistics can be allocated now because their size
     * does not change but this is delayed to allocate all at once */

    pBuffer->sListCell.object = pBuffer;

    *ret = IMG_SUCCESS;
    return pBuffer;
}

IMG_RESULT KRN_CI_ShotMapMemory(KRN_CI_SHOT *pBuffer,
struct MMUDirectory *pDirectory)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(pBuffer);
    IMG_ASSERT(pDirectory);
    IMG_ASSERT(g_psCIDriver);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "MMU mapping of Shot");
    CI_DEBUG("MMU mapping of shot:\n");

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "linked list mapping (RO)");
    CI_DEBUG("linked list mapping (RO)\n");
    ret = SYS_MemMap(&(pBuffer->hwLinkedList), pDirectory, MMU_RO);

    if (ret == IMG_SUCCESS)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "load structure mapping (RO)");
        CI_DEBUG("load structure mapping (RO)\n");
        ret = SYS_MemMap(&(pBuffer->hwLoadStructure.sMemory), pDirectory,
            MMU_RO);
    }

    // this should be done when mapping available buffers
    // but all other Shots need the mapping as well
    if (ret == IMG_SUCCESS && pBuffer->phwEncoderOutput != NULL)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "encoder output mapping (WO)");
        CI_DEBUG("encoder output mapping (WO) - ID = 0x%x\n",
            pBuffer->phwEncoderOutput->ID);
        ret = SYS_MemMap(&(pBuffer->phwEncoderOutput->sMemory), pDirectory,
            MMU_WO);
    }

    // this should be done when mapping available buffers
    // but all other Shots need the mapping as well
    CI_DEBUG("pBuffer->phwDisplayOutput = %p)\n", pBuffer->phwDisplayOutput);
    if (IMG_SUCCESS == ret && pBuffer->phwDisplayOutput)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "display output mapping (WO)");
        CI_DEBUG("display output mapping (WO - ID = 0x%x)\n",
            pBuffer->phwDisplayOutput->ID);
        ret = SYS_MemMap(&(pBuffer->phwDisplayOutput->sMemory), pDirectory,
            MMU_WO);
    }

    // this should be done when mapping available buffers
    // but all other Shots need the mapping as well
    if (IMG_SUCCESS == ret && pBuffer->phwHDRExtOutput)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "HDR Extraction output mapping (WO)");
        CI_DEBUG("HDRExt output mapping (WO - ID = 0x%x)\n",
            pBuffer->phwHDRExtOutput->ID);
        ret = SYS_MemMap(&(pBuffer->phwHDRExtOutput->sMemory), pDirectory,
            MMU_WO);
    }

    // this should be done when mapping available buffers
    // but all other Shots need the mapping as well
    if (IMG_SUCCESS == ret && pBuffer->phwRaw2DExtOutput)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "Raw 2D Extraction output mapping (WO)");
        CI_DEBUG("Raw2DExt output mapping (WO - ID = 0x%x)\n",
            pBuffer->phwRaw2DExtOutput->ID);
        ret = SYS_MemMap(&(pBuffer->phwRaw2DExtOutput->sMemory), pDirectory,
            MMU_WO);
    }

    if (IMG_SUCCESS == ret)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "save structure mapping (WO)");
        CI_DEBUG("save structure mapping (WO) - sMemory = %p ID = 0x%x\n",
            &pBuffer->hwSave.sMemory, pBuffer->hwSave.ID);
        ret = SYS_MemMap(&(pBuffer->hwSave.sMemory), pDirectory, MMU_WO);
    }

    if (IMG_SUCCESS == ret && pBuffer->sDPFWrite.sMemory.uiAllocated > 0)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "DPF output mapping (WO)");
        CI_DEBUG("DPF output mapping (WO) - ID = 0x%x\n",
            pBuffer->sDPFWrite.ID);
        ret = SYS_MemMap(&(pBuffer->sDPFWrite.sMemory), pDirectory, MMU_WO);
    }

    if (IMG_SUCCESS == ret && pBuffer->sENSOutput.sMemory.uiAllocated > 0)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "ENS output mapping (WO)");
        CI_DEBUG("ENS output mapping (WO) - ID = 0x%x\n",
            pBuffer->sENSOutput.ID);
        ret = SYS_MemMap(&(pBuffer->sENSOutput.sMemory), pDirectory, MMU_WO);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "MMU mapping done");

    return ret;
}

IMG_RESULT KRN_CI_ShotUnmapMemory(KRN_CI_SHOT *pBuffer)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(pBuffer);

    if (g_psCIDriver)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "MMU unmapping of frame");
    }

    SYS_MemUnmap(&(pBuffer->hwLinkedList));
    SYS_MemUnmap(&(pBuffer->hwLoadStructure.sMemory));

    // this should be done when un-mapping available buffers
    // but all other Shots need the mapping as well
    if (pBuffer->phwEncoderOutput)
    {
        SYS_MemUnmap(&(pBuffer->phwEncoderOutput->sMemory));
    }
    if (pBuffer->phwDisplayOutput)
    {
        SYS_MemUnmap(&(pBuffer->phwDisplayOutput->sMemory));
    }
    if (pBuffer->phwHDRExtOutput)
    {
        SYS_MemUnmap(&(pBuffer->phwHDRExtOutput->sMemory));
    }
    if (pBuffer->phwRaw2DExtOutput)
    {
        SYS_MemUnmap(&(pBuffer->phwRaw2DExtOutput->sMemory));
    }
    SYS_MemUnmap(&(pBuffer->hwSave.sMemory));
    SYS_MemUnmap(&(pBuffer->sDPFWrite.sMemory));
    SYS_MemUnmap(&(pBuffer->sENSOutput.sMemory));

    if (g_psCIDriver)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "MMU unmapping done");
    }

    return ret;
}

IMG_RESULT KRN_CI_ShotDestroy(KRN_CI_SHOT *pBuffer)
{
    if (!pBuffer)
    {
        CI_FATAL("pBuffer is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* NULL when destroying the driver - print warning that some shots were
     * destroyed by driver */
    if (g_psCIDriver)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");
    }
    else
    {
        CI_WARNING("cleaning a left-over shot %p\n", pBuffer);
        KRN_CI_ShotUnmapMemory(pBuffer);
        /* because the capture may not have been stopped properly but we need
         * to clean up */
        /* mmu should have been disabled anyway, so nobody will try to access
         * that bit of memory */
    }

    IMG_CI_ShotFreeBuffers(pBuffer);

    if (pBuffer->sListCell.pContainer)
    {
        CI_WARNING("Destroying a shot which is still attached to a list\n");
    }

    IMG_FREE(pBuffer);

    if (g_psCIDriver)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_ShotClean(KRN_CI_SHOT *pBuffer)
{
    IMG_ASSERT(pBuffer);
    IMG_ASSERT(g_psCIDriver);
    // should already have been allocated
    IMG_ASSERT(pBuffer->hwSave.sMemory.uiAllocated > 0);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "clean outputs");

    /* clean the device memory used for statistics so that there is no
     * garbage memory */
    IMG_MEMSET(Platform_MemGetMemory(&(pBuffer->hwSave.sMemory)),
        0, HW_CI_SaveStructureSize());
    Platform_MemUpdateDevice(&(pBuffer->hwSave.sMemory));

#ifdef FELIX_FAKE
    if (pBuffer->sDPFWrite.sMemory.uiAllocated > 0)
    {
        // clean the device memory used for the DPF output for test purposes
        IMG_MEMSET(Platform_MemGetMemory(&(pBuffer->sDPFWrite.sMemory)),
            0, pBuffer->userShot.uiDPFSize);
        Platform_MemUpdateDevice(&(pBuffer->sDPFWrite.sMemory));
    }
    if (pBuffer->sENSOutput.sMemory.uiAllocated > 0)
    {
        // clean the device memory used for the ENS output for test purposes
        IMG_MEMSET(Platform_MemGetMemory(&(pBuffer->sENSOutput.sMemory)),
            0, pBuffer->userShot.uiENSSize);
        Platform_MemUpdateDevice(&(pBuffer->sENSOutput.sMemory));
    }
#endif

    return IMG_SUCCESS;
}

