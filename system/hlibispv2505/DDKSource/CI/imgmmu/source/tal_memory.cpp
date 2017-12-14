/**
*******************************************************************************
@file tal_memory.cpp

@brief TAL allocator functions implementation

@note if TAL_MEM_MMU_NAME defined it will be used as a prefix for the pdump
MMU pages names

@note if TAL_MEM_NAME defined it will be used as a prefix for the pdump
device memory names

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
#include <mmulib/mmu.h>
#include <talalloc/talalloc.h>

#include <img_defs.h>
#include <img_errors.h>
#include <tal.h>
#include <map>
#include <algorithm>  // std::min

/**
 * @brief TAL Internal variable to use
 */
#ifndef TAL_INT_VAR
#define TAL_INT_VAR 0
#endif /* TAL_INT_VAR */

#ifdef TAL_MEM_MMU_NAME
static IMG_UINT32 g_ui32MMUAllocNumber = 0;
#endif

#ifndef MMU_MALLOC_UPDATE_DEVICE
#define MMU_MALLOC_UPDATE_DEVICE IMG_TRUE
#endif

static IMG_UINT32 g_MMU_PHYS_SIZE = (IMGMMU_PHYS_SIZE);

extern "C" {

    struct AllocCell
    {
        /** parent allocator handle */
        IMG_HANDLE talHandle;
        /** index in the map - duplicated for ease of search */
        IMG_UINT64 ui64Offset;
    };

    /** @brief stores IMG_HANDLE (TAL_sMemspaceCB*) to use when allocating */
    IMG_HANDLE g_pTalMemoryRegionHandle = NULL;

    struct IntMMUPage
    {
        struct MMUPage sPage;
        /**
         * @brief Memory handle (TAL_sMemspaceCB*) used for allocation - needed
         * to run pdump variables commands
         */
        IMG_HANDLE sMemspaceCB;
        /** @brief store this address as physical page */
        struct AllocCell sAllocCell;
    };

#include "mmu_defs.h"
}

/** map of physical addresses (masked PAGE_SIZE bits) to tal allocations */
static std::map<IMG_UINT64, struct AllocCell> g_allocCellMap;

static IMG_UINT32 getCPUPageMask()
{
    IMG_UINT32 tmpPage = IMGMMU_GetCPUPageSize();
    IMG_UINT32 pageShift = 0;

    while (tmpPage >>= 1)  /* compute log2() */
    {
        pageShift++;
    }

    return (1 << pageShift) - 1;
}

/*
 * public functions for MMU interface TAL helper
 */

void IMGMMU_TALPageSetMemRegion(IMG_HANDLE pTalHandle)
{
    g_pTalMemoryRegionHandle = pTalHandle;
}

struct MMUPage* IMGMMU_TALPageAlloc()
{
    struct IntMMUPage *pNew = NULL;
    void *pCpuVirt = NULL;
    IMG_HANDLE tmpHandle;
    char *allocName = NULL;
    IMG_RESULT ret;
#ifdef TAL_MEM_MMU_NAME
    static char name[64];
#endif

    IMG_ASSERT(g_pTalMemoryRegionHandle != NULL);
    IMG_ASSERT(IMGMMU_GetCPUPageSize() >= MMU_PAGE_SIZE);

    pNew = (struct IntMMUPage*)IMG_CALLOC(1, sizeof(struct IntMMUPage));
    if (pNew == NULL)
    {
        return NULL;
    }

    pCpuVirt = IMG_CALLOC(IMGMMU_GetCPUPageSize(), 1);
    if (pCpuVirt == NULL)
    {
        IMG_FREE(pNew);
        return NULL;
    }

#ifdef TAL_MEM_MMU_NAME
    {
        static int pageallocnb = 0;
        IMG_ASSERT(strlen(TAL_MEM_MMU_NAME) < 64 - 7);

        snprintf(name, sizeof(name), "%s%d", TAL_MEM_MMU_NAME, pageallocnb++);
        allocName = name;
    }
#endif

    ret = TALMEM_Malloc(g_pTalMemoryRegionHandle, (IMG_CHAR*)pCpuVirt,
        IMGMMU_GetCPUPageSize(), IMGMMU_GetCPUPageSize(), &(tmpHandle),
        MMU_MALLOC_UPDATE_DEVICE, allocName);
    if (ret != IMG_SUCCESS)
    {
        IMG_FREE(pCpuVirt);
        IMG_FREE(pNew);
        return NULL;
    }
    pNew->sMemspaceCB = g_pTalMemoryRegionHandle;
    pNew->sPage.uiCpuVirtAddr = (IMG_UINTPTR)pCpuVirt;
    pNew->sAllocCell.talHandle = tmpHandle;
    pNew->sAllocCell.ui64Offset = TALDEBUG_GetDevMemAddress(tmpHandle);
    // (IMG_UINT64)&(pNew->sAllocCell);
    pNew->sPage.uiPhysAddr = pNew->sAllocCell.ui64Offset;

    IMG_ASSERT(g_allocCellMap.find(pNew->sAllocCell.ui64Offset)
        == g_allocCellMap.end());
    g_allocCellMap[pNew->sAllocCell.ui64Offset] = pNew->sAllocCell;

    // returns the address to the public part of the structure
    return &(pNew->sPage);
}

void IMGMMU_TALPageFree(struct MMUPage *pPage)
{
    struct IntMMUPage *pIntPage = NULL;

    IMG_ASSERT(pPage != NULL);

    pIntPage = container_of(pPage, struct IntMMUPage, sPage);

    std::map<IMG_UINT64, struct AllocCell>::iterator it =
        g_allocCellMap.find(pIntPage->sAllocCell.ui64Offset);
    IMG_ASSERT(it != g_allocCellMap.end());
    g_allocCellMap.erase(it);

    IMG_FREE((void*)pPage->uiCpuVirtAddr);
    TALMEM_Free(&(pIntPage->sAllocCell.talHandle));

    IMG_FREE(pIntPage);
}

void IMGMMU_TALPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags)
{
    struct IntMMUPage *pIntPage = NULL;
    IMG_ASSERT(pWriteTo != NULL);

    pIntPage = container_of(pWriteTo, struct IntMMUPage, sPage);

    if (uiToWrite != 0)
    {
        IMG_RESULT result;

        const struct AllocCell *pToWrite = NULL;
        IMG_HANDLE toWriteHandle = NULL;

        IMG_UINT64 toSearch = uiToWrite&(~getCPUPageMask());
        std::map<IMG_UINT64, struct AllocCell>::const_iterator it =
            g_allocCellMap.find(toSearch);
        IMG_ASSERT(it != g_allocCellMap.end());

        pToWrite = &(it->second);

        IMG_ASSERT(g_pTalMemoryRegionHandle != NULL);

        // modifies toWriteOffset to the offset in the return tal handle
        result = TALINTVAR_WriteMemRef(pToWrite->talHandle,
            uiToWrite - pToWrite->ui64Offset, pIntPage->sMemspaceCB,
            TAL_INT_VAR);

        if (result == IMG_SUCCESS && (g_MMU_PHYS_SIZE - IMGMMU_VIRT_SIZE) > 0)
        {
            /* does a right shift to fit a 40b address in 32b - if not used
             * and the MMU is more than 32b the "upper address" register of
             * the MMU HW can be used to set which 4GB of the memory should
             * be used */
            result = TALINTVAR_RunCommand(TAL_PDUMPL_INTREG_SHR,
                pIntPage->sMemspaceCB, TAL_INT_VAR, pIntPage->sMemspaceCB,
                TAL_INT_VAR, pIntPage->sMemspaceCB,
                (g_MMU_PHYS_SIZE - IMGMMU_VIRT_SIZE), IMG_FALSE);
        }

        if (result == IMG_SUCCESS)
        {
            result = TALINTVAR_RunCommand(TAL_PDUMPL_INTREG_OR,
                pIntPage->sMemspaceCB, TAL_INT_VAR, pIntPage->sMemspaceCB,
                TAL_INT_VAR, pIntPage->sMemspaceCB, flags, IMG_FALSE);
        }

        if (result == IMG_SUCCESS)
        {
            // memory is stored in 32b -> offset of entry*4
            result = TALINTVAR_WriteToMem32(pIntPage->sAllocCell.talHandle,
                offset * 4, pIntPage->sMemspaceCB, TAL_INT_VAR);
        }

        IMG_ASSERT(result == IMG_SUCCESS);
    }
    else  // if pToWrite is NULL only the flags should be written
    {
        TALMEM_WriteWord32(pIntPage->sAllocCell.talHandle, offset * 4, flags);
    }
}

void IMGMMU_TALPageUpdate(struct MMUPage *pPage)
{
    struct IntMMUPage *pIntPage = NULL;
    IMG_ASSERT(pPage != NULL);

    pIntPage = container_of(pPage, struct IntMMUPage, sPage);

    /* the Update to device should not be done for PDUMP to be replayable
     * as the LDB will override the dynamic addresses written in
     * IMGMMU_TALPageWrite
     * but some updates may be needed on systems with a cache!
     * it is however useful when debugging page failures (to see state of MMU
     * pages) */
    IMG_ASSERT(g_pTalMemoryRegionHandle != NULL);
    TALPDUMP_Comment(g_pTalMemoryRegionHandle, "Update device memory for MMU");

    TALMEM_UpdateDevice(pIntPage->sAllocCell.talHandle);
}

IMG_HANDLE IMGMMU_TALDirectoryGetMemHandle(struct MMUDirectory *pDirectory)
{
    IMGMMU_Page *pPage = NULL;
    struct IntMMUPage *pIntPage = NULL;

    IMG_ASSERT(pDirectory != NULL);

    pPage = IMGMMU_DirectoryGetPage(pDirectory);
    pIntPage = container_of(pPage, struct IntMMUPage, sPage);
    return pIntPage->sAllocCell.talHandle;
}

IMG_UINT32 IMGMMU_TALGetMMUSize(void)
{
    return g_MMU_PHYS_SIZE;
}

void IMGMMU_TALSetMMUSize(IMG_UINT32 ui32MMUSize)
{
    IMG_ASSERT(ui32MMUSize >= IMGMMU_VIRT_SIZE);
    g_MMU_PHYS_SIZE = ui32MMUSize;
}

/*
 * TAL Allocator
 */

IMGMMU_TALAlloc* TALALLOC_Malloc(IMG_HANDLE sTalMemHandle, IMG_SIZE uiSize,
    IMG_RESULT *pResult)
{
    IMG_INT32 i32NCPUPages = 0;  // number of CPU pages
    IMG_INT32 i;
    IMG_INT32 nDevPerCPUPage = 1;
    IMGMMU_TALAlloc *pNew = NULL;
    const IMG_UINT32 pagesize = IMGMMU_GetCPUPageSize();
    IMG_RESULT ret;

    IMG_ASSERT(sTalMemHandle != NULL);
    IMG_ASSERT(pResult != NULL);
    IMG_ASSERT(uiSize > 0);

    i32NCPUPages = uiSize / pagesize;
    if (uiSize%pagesize != 0)
    {
        i32NCPUPages++;
    }

    if (i32NCPUPages <= 0)
    {
        *pResult = IMG_ERROR_INVALID_PARAMETERS;
        return NULL;
    }

    if (pagesize < MMU_PAGE_SIZE || pagesize%MMU_PAGE_SIZE != 0)
    {
        // CPU page must be at least as big as MMU page and a multiple of it
        *pResult = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }

    nDevPerCPUPage = pagesize / MMU_PAGE_SIZE;
    IMG_ASSERT(nDevPerCPUPage > 0);

    pNew = (IMGMMU_TALAlloc*)IMG_CALLOC(1, sizeof(IMGMMU_TALAlloc));
    if (pNew == NULL)
    {
        goto pNew_failed;
    }

    pNew->aPhysAlloc = (IMG_HANDLE*)IMG_CALLOC(i32NCPUPages,
        sizeof(IMG_HANDLE));
    if (pNew->aPhysAlloc == NULL)
    {
        goto aPhysAlloc_failed;
    }
    pNew->aDevAddr = (IMG_UINT64*)IMG_CALLOC(i32NCPUPages, sizeof(IMG_UINT64));
    if (pNew->aDevAddr == NULL)
    {
        goto aDevAddr_failed;
    }

    pNew->uiAllocSize = i32NCPUPages*pagesize;
    pNew->pHostLinear = IMG_CALLOC(pNew->uiAllocSize, 1);
    pNew->sTalMemHandle = sTalMemHandle;
    pNew->uiNCPUPages = i32NCPUPages;

    if (pNew->pHostLinear == NULL)
    {
        goto pHostLinear_failed;
    }

    for (i = 0; i < i32NCPUPages; i++)
    {
        char *pCPU = (char*)pNew->pHostLinear;
        char *allocName = NULL;
        IMG_HANDLE tmpHandle;
#ifdef TAL_MEM_NAME
        char name[64];
        {
            static int allocnb = 0;

            // 10^6 block is 4GB for 4kB blocks..
            IMG_ASSERT(strlen(TAL_MEM_NAME) <= 64-6);

            snprintf(name, sizeof(name), "%s%d", TAL_MEM_NAME, allocnb++);
            allocName = name;
        }
#endif

        pCPU = pCPU + i*pagesize;

        ret = TALMEM_Malloc(sTalMemHandle, pCPU, pagesize, 32, &(tmpHandle),
            IMG_FALSE, allocName);
        if (ret != IMG_SUCCESS)
        {
            goto dev_add_failed;
        }

        /* use the TAL handle as a physical address (it will be converted
         * back when using IMGMMU_TALPageWrite) */
        pNew->aPhysAlloc[i] = tmpHandle;  // TO_ADDR(tmpHandle);
        pNew->aDevAddr[i] = TALDEBUG_GetDevMemAddress(tmpHandle);

        struct AllocCell sCell;
        sCell.talHandle = tmpHandle;
        sCell.ui64Offset = pNew->aDevAddr[i];

        IMG_ASSERT(g_allocCellMap.find(sCell.ui64Offset)
            == g_allocCellMap.end());
        g_allocCellMap[sCell.ui64Offset] = sCell;
    }

    *pResult = IMG_SUCCESS;
    return pNew;

dev_add_failed:
    i--;
    while (i >= 0)
    {
        pNew->aDevAddr[i] = 0;  // 0 isn't really illegal but it cleans a bit
        TALMEM_Free((IMG_HANDLE*)&(pNew->aPhysAlloc[i]));
        i--;
    }
    IMG_FREE(pNew->pHostLinear);
pHostLinear_failed:
    IMG_FREE(pNew->aDevAddr);
aDevAddr_failed:
    IMG_FREE(pNew->aPhysAlloc);
aPhysAlloc_failed:
    IMG_FREE(pNew);
pNew_failed:
    *pResult = IMG_ERROR_FATAL;
    return NULL;
}

IMG_RESULT TALALLOC_Free(IMGMMU_TALAlloc *pAlloc)
{
    IMG_UINT32 i;

    if (pAlloc == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0; i < pAlloc->uiNCPUPages; i++)
    {
        // cannot assert != 0 because 0 is legitimate physical address
        // IMG_ASSERT(pAlloc->aDevAddr[i] != 0);
        IMG_ASSERT(pAlloc->aPhysAlloc[i] != 0);

        std::map<IMG_UINT64, struct AllocCell>::iterator it =
            g_allocCellMap.find(TALDEBUG_GetDevMemAddress(
            pAlloc->aPhysAlloc[i]));
        IMG_ASSERT(it != g_allocCellMap.end());
        g_allocCellMap.erase(it);

        TALMEM_Free((IMG_HANDLE*)&(pAlloc->aPhysAlloc[i]));
    }

    IMG_FREE(pAlloc->pHostLinear);
    IMG_FREE(pAlloc->aDevAddr);
    IMG_FREE(pAlloc->aPhysAlloc);
    IMG_FREE(pAlloc);

    return IMG_SUCCESS;
}

const IMG_UINT64* TALALLOC_GetPhysMem(IMGMMU_TALAlloc *pAlloc)
{
    return pAlloc->aDevAddr;
}

IMG_RESULT TALALLOC_UpdateHost(IMGMMU_TALAlloc *pAlloc)
{
    IMG_UINT32 i;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pAlloc == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0; i < pAlloc->uiNCPUPages && ret == IMG_SUCCESS; i++)
    {
        ret = TALMEM_UpdateHost(pAlloc->aPhysAlloc[i]);
    }

    return ret;
}

IMG_RESULT TALALLOC_UpdateHostRegion(IMGMMU_TALAlloc *pAlloc,
    IMG_UINT64 offset, IMG_UINT64 size)
{
    IMG_INT64 page_idx;
    IMG_UINT64 first_page, first_adress, first_size;
    IMG_INT64 remaining_size, current_size;
    IMG_RESULT ret = IMG_SUCCESS;
    const IMG_UINT32 pagesize = IMGMMU_GetCPUPageSize();

    if (pAlloc == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (offset + size > pAlloc->uiAllocSize)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    first_page = offset / pagesize;
    first_adress = offset % pagesize;
    first_size = std::min(pagesize - first_adress, size);
    remaining_size = size - first_size;
    page_idx = first_page;

    ret = TALMEM_UpdateHostRegion(pAlloc->aPhysAlloc[page_idx++],
        first_adress, first_size);
    while (remaining_size > 0 && ret == IMG_SUCCESS)
    {
        current_size = std::min(remaining_size, (IMG_INT64)pagesize);
        ret = TALMEM_UpdateHostRegion(pAlloc->aPhysAlloc[page_idx++], 0,
            current_size);

        remaining_size -= current_size;
    }

    return ret;
}

IMG_RESULT TALALLOC_UpdateDevice(IMGMMU_TALAlloc *pAlloc)
{
    IMG_UINT32 i;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pAlloc == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0; i < pAlloc->uiNCPUPages && ret == IMG_SUCCESS; i++)
    {
        ret = TALMEM_UpdateDevice(pAlloc->aPhysAlloc[i]);
    }

    return ret;
}

IMG_RESULT TALALLOC_UpdateDeviceRegion(IMGMMU_TALAlloc *pAlloc,
    IMG_UINT64 offset, IMG_UINT64 size)
{
    IMG_INT64 page_idx;
    IMG_UINT64 first_page, first_adress, first_size;
    IMG_INT64 remaining_size, current_size;
    IMG_RESULT ret = IMG_SUCCESS;
    const IMG_UINT32 pagesize = IMGMMU_GetCPUPageSize();

    if (pAlloc == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (offset + size > pAlloc->uiAllocSize)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    first_page = offset / pagesize;
    first_adress = offset % pagesize;
    first_size = std::min(pagesize - first_adress, size);
    remaining_size = size - first_size;
    page_idx = first_page;

    ret = TALMEM_UpdateHostRegion(pAlloc->aPhysAlloc[page_idx++],
        first_adress, first_size);
    while (remaining_size > 0 && ret == IMG_SUCCESS)
    {
        current_size = std::min(remaining_size, (IMG_INT64)pagesize);
        ret = TALMEM_UpdateDeviceRegion(pAlloc->aPhysAlloc[page_idx++], 0,
            current_size);

        remaining_size -= current_size;
    }

    return ret;
}

IMG_HANDLE TALALLOC_ComputeOffset(IMGMMU_TALAlloc *pAlloc, IMG_SIZE *uiOff)
{
    IMG_SIZE uiOriginalOffset = 0;
    IMG_UINT32 p = 0;  // current page
    const IMG_UINT32 pagesize = IMGMMU_GetCPUPageSize();

    IMG_ASSERT(pAlloc != NULL);
    IMG_ASSERT(uiOff != NULL);

    uiOriginalOffset = *uiOff;

    while (*uiOff >= pagesize)
    {
        *uiOff -= pagesize;
        p++;
    }

    if (p < pAlloc->uiNCPUPages)
    {
        return pAlloc->aPhysAlloc[p];
    }
    return NULL;
}
