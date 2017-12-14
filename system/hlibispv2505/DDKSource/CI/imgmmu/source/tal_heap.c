/**
*******************************************************************************
@file tal_heap.c

@brief Implementation of the MMU library heap using TAL

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
#include <mmulib/heap.h>
#include <addr_alloc.h>

#include <img_defs.h>
#include <img_errors.h>

#ifndef container_of
#error the wrong img includes are used
#endif

#include "mmu_defs.h"  // access to MMU info and error printing function

/**
 * @defgroup IMGMMU_TALHeap Heap management using TAL
 * @brief Virtual address management using ADDR functions from TAL
 *
 * Implements the IMGMMU Heap functions and use a private structure container
 * to store the TAL handles to ADDR objects.
 *
 * @image html "TAL Heap_class.png" "TAL Heap structure organisation"
 * @ingroup IMGMMU_heap
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMGMMU_TAL_int module
 *---------------------------------------------------------------------------*/

/**
 * @brief The Heap information - contains an IMGMMU_Heap that is given to the
 * caller
 */
struct TAL_Heap
{
    /** @brief TAL heap context */
    ADDR_sContext sTalHeapCtx;
    /** @brief TAL Heap region associated to the heap context */
    ADDR_sRegion sTalHeapRegion;
    /** @brief TAL memory region name (from configuration) */
    const char *pszMemoryRegionName;

    /** @brief Number of associated allocations */
    IMG_UINT32 uiNAlloc;
    /**
     * @brief current total alloc size - allocation may not be contiguous
     * but helps for debugging */
    IMG_SIZE uiAllocSize;
    /** @brief MMU lib heap info part (public element) */
    IMGMMU_Heap sHeapInfo;
};

/**
 * @brief The Heap allocation - contains an IMGMMU_HeapAlloc that is given
 * to the caller
 */
struct TAL_HeapAlloc
{
    /** @brief Associated heap */
    struct TAL_Heap *pHeap;
    /** @brief MMU lib allocation part (public element) */
    IMGMMU_HeapAlloc sVirtualMem;
};

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the IMGMMU_TAL module
 *---------------------------------------------------------------------------*/

/*
 * public functions
 */

IMGMMU_Heap* IMGMMU_HeapCreate(IMG_UINTPTR uiVirtAddrStart,
    IMG_SIZE uiAllocAtom, IMG_SIZE uiSize, IMG_RESULT *pResult)
{
    struct TAL_Heap *pNeo = NULL;

    IMG_ASSERT(pResult != NULL);
    /* not sure that one makes sence - could be related to burst size rather
     * than page size */
    if (uiSize%uiAllocAtom != 0 ||
        (uiVirtAddrStart != 0 && uiVirtAddrStart%uiAllocAtom != 0)
        )
    {
        MMU_LogError("Wrong input param %" IMG_SIZEPR "u not multiple of %" \
            IMG_SIZEPR "u (%u), %" IMG_SIZEPR "u not multiple of %" \
            IMG_SIZEPR "u (%u)\n",
            uiSize, uiAllocAtom, uiSize%uiAllocAtom, uiVirtAddrStart,
            uiAllocAtom, uiVirtAddrStart%uiAllocAtom);
        *pResult = IMG_ERROR_INVALID_PARAMETERS;
        return NULL;
    }

    pNeo = (struct TAL_Heap*)IMG_CALLOC(1, sizeof(struct TAL_Heap));
    if (pNeo == NULL)
    {
        *pResult = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    ADDR_CxInitialise(&(pNeo->sTalHeapCtx));
    pNeo->sTalHeapRegion.ui64BaseAddr = (IMG_UINT64)uiVirtAddrStart;
    pNeo->sTalHeapRegion.ui64Size = (IMG_UINT64)uiSize;
    ADDR_CxDefineMemoryRegion(&(pNeo->sTalHeapCtx), &(pNeo->sTalHeapRegion));

    pNeo->sHeapInfo.uiVirtAddrStart = uiVirtAddrStart;
    pNeo->sHeapInfo.uiAllocAtom = uiAllocAtom;
    pNeo->sHeapInfo.uiSize = uiSize;
    pNeo->pszMemoryRegionName = "";

    *pResult = IMG_SUCCESS;
    return &(pNeo->sHeapInfo);
}

IMGMMU_HeapAlloc* IMGMMU_HeapAllocate(IMGMMU_Heap *pHeap, IMG_SIZE uiSize,
    IMG_RESULT *pResult)
{
    struct TAL_Heap *pInternalHeap = NULL;
    struct TAL_HeapAlloc *pNeo = NULL;
    IMG_UINT64 proposedAddr = (IMG_UINT64)-1;

    IMG_ASSERT(pResult != NULL);
    IMG_ASSERT(pHeap != NULL);
    pInternalHeap = container_of(pHeap, struct TAL_Heap, sHeapInfo);

    if (uiSize%pHeap->uiAllocAtom != 0)
    {
        *pResult = IMG_ERROR_INVALID_PARAMETERS;
        return NULL;
    }

    pNeo = (struct TAL_HeapAlloc*)IMG_CALLOC(1, sizeof(struct TAL_HeapAlloc));
    if (pNeo == NULL)
    {
        *pResult = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    proposedAddr = ADDR_CxMalloc1(&(pInternalHeap->sTalHeapCtx),
        (IMG_CHAR*)pInternalHeap->pszMemoryRegionName, uiSize,
        pHeap->uiAllocAtom);
    /* because of 32b/64b difference of size in uintptr_t and that CxMalloc1
     * returns -1 as wrong address instead of having a return code */
    pNeo->sVirtualMem.uiVirtualAddress = (IMG_UINTPTR)proposedAddr;
    pNeo->sVirtualMem.uiAllocSize = uiSize;
    pNeo->pHeap = pInternalHeap;

    /* ADDR_CxMalloc1 put this value as default... so it is a mess because
     * 32b and 64b need a casting or not... */
    if (proposedAddr == -1LL)
    {
        *pResult = IMG_ERROR_NOT_SUPPORTED;
        IMG_FREE(pNeo);
        return NULL;
    }

    pInternalHeap->uiNAlloc++;
    pInternalHeap->uiAllocSize += uiSize;

    *pResult = IMG_SUCCESS;
    return &(pNeo->sVirtualMem);
}

IMG_RESULT IMGMMU_HeapFree(IMGMMU_HeapAlloc *pHeapAlloc)
{
    struct TAL_HeapAlloc *pInternalAlloc = NULL;

    IMG_ASSERT(pHeapAlloc != NULL);
    pInternalAlloc = container_of(pHeapAlloc, struct TAL_HeapAlloc,
        sVirtualMem);

    IMG_ASSERT(pInternalAlloc->pHeap->uiNAlloc > 0);
    pInternalAlloc->pHeap->uiNAlloc--;
    pInternalAlloc->pHeap->uiAllocSize -=
        pInternalAlloc->sVirtualMem.uiAllocSize;

    ADDR_CxFree(&(pInternalAlloc->pHeap->sTalHeapCtx),
        (IMG_CHAR*)pInternalAlloc->pHeap->pszMemoryRegionName,
        pHeapAlloc->uiVirtualAddress);

    IMG_FREE(pInternalAlloc);
    return IMG_SUCCESS;
}

IMG_RESULT IMGMMU_HeapDestroy(IMGMMU_Heap *pHeap)
{
    struct TAL_Heap *pInternalHeap = NULL;

    IMG_ASSERT(pHeap != NULL);
    pInternalHeap = container_of(pHeap, struct TAL_Heap, sHeapInfo);

    if (pInternalHeap->uiNAlloc > 0)
    {
        MMU_LogError("destroying a heap with non-freed allocation\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ADDR_CxDeinitialise(&(pInternalHeap->sTalHeapCtx));

    IMG_FREE(pInternalHeap);
    return IMG_SUCCESS;
}

/*
 * tal helper
 */

void IMGMMU_TALHeapSetMemRegion(struct MMUHeap *pHeap,
    const char *pszMemoryRegionName)
{
    struct TAL_Heap *pInternalHeap = NULL;

    IMG_ASSERT(pHeap != NULL);
    pInternalHeap = container_of(pHeap, struct TAL_Heap, sHeapInfo);

    pInternalHeap->pszMemoryRegionName = pszMemoryRegionName;
}
