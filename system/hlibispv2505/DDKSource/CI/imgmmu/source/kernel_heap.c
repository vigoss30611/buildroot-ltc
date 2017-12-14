/**
*******************************************************************************
@file kernel_heap.c

@brief MMU Library: device virtual allocation (heap) implementation using
gen_alloc from the Linux kernel

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
#include <mmulib/heap.h>

#include <img_errors.h>
#include <img_defs.h>
#include <linux/genalloc.h>
#include <linux/version.h>

#include "mmu_defs.h"  // access to MMU info and error printing function

/** @brief Internal heap object using genalloc */
struct GEN_Heap
{
    struct gen_pool *pPool;
    /**
     * we could use gen_get_size() but it goes through the list, it's easier
     * to maintain a simple counter
     */
    IMG_SIZE uiNAlloc;
    /** @brief public element */
    IMGMMU_Heap sHeapInfo;
};

/**
 * @brief The Heap allocation - contains an IMGMMU_HeapAlloc that is given
 * to the caller
 */
struct GEN_HeapAlloc
{
    /** @brief Associated heap */
    struct GEN_Heap *pHeap;
    /** @brief MMU lib allocation part (public element) */
    IMGMMU_HeapAlloc sVirtualMem;
};

/**
 *  can be used for debugging
 *
 * example: gen_pool_for_each_chunk(pInternalAlloc->pHeap->pPool,
 * &pool_crawler, pInternalAlloc);
 */
static void pool_crawler(struct gen_pool *pool, struct gen_pool_chunk *chunk,
    void *data) __maybe_unused;

static void pool_crawler(struct gen_pool *pool, struct gen_pool_chunk *chunk,
    void *data)
{
    printk(KERN_INFO "pool 0x%p has chunk 0x%lx to 0x%lx (size = %lu B)\n",
        data, chunk->start_addr, chunk->end_addr,
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,12,0)
        // no -1 used when computing end address in genalloc
        (chunk->end_addr - chunk->start_addr)
#else
        (chunk->end_addr - chunk->start_addr + 1)
#endif
        );
}

IMGMMU_Heap* IMGMMU_HeapCreate(IMG_UINTPTR uiVirtAddrStart,
    IMG_SIZE uiAllocAtom, IMG_SIZE uiSize, IMG_RESULT *pResult)
{
    struct GEN_Heap *pNeo = NULL;
    int minAllocOrder = 0;  // log2 of the alloc atom
    IMG_SIZE tmpSize = uiAllocAtom;
    int ret;
    IMG_UINTPTR uiStart = uiVirtAddrStart;

    IMG_ASSERT(pResult != NULL);
    /* this could be caught by the later assert on tmpSize but it's
     * easier to see if it was an overflow or empty heap this way */
    IMG_ASSERT(uiSize > 0);

    /* not sure uiVirtAddrStart makes sence - could be related to burst size
     * rather than page size */
    if (uiSize%uiAllocAtom != 0
        || (uiVirtAddrStart != 0 && uiVirtAddrStart%uiAllocAtom != 0))
    {
        MMU_LogError("Wrong input param %" IMG_SIZEPR "u not multiple of %" \
            IMG_SIZEPR "u (%u), %" IMG_SIZEPR "u not multiple of %" \
            IMG_SIZEPR "u (%u)\n",
            uiSize, uiAllocAtom, uiSize%uiAllocAtom, uiVirtAddrStart,
            uiAllocAtom, uiVirtAddrStart%uiAllocAtom);
        *pResult = IMG_ERROR_INVALID_PARAMETERS;
        return NULL;
    }

    pNeo = (struct GEN_Heap*)IMG_CALLOC(1, sizeof(struct GEN_Heap));
    if (pNeo == NULL)
    {
        *pResult = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    pNeo->uiNAlloc = 0;

    // compute log2 of the alloc atom
    while (tmpSize >>= 1)
    {
        minAllocOrder++;
    }

    /* ugly fix for trouble using gen_pool_alloc() when allocating a block
     * gen_pool_alloc() returns 0 on error alought 0 can be a valid first
     * virtual address
     * therefore all addresses are offseted by the allocation atom to insure 0
     * is the actual error code */
    if (uiVirtAddrStart == 0)
    {
        // otherwise it is uiVritAddrStart
        uiStart = uiVirtAddrStart + uiAllocAtom;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 12, 0)
    // genalloc does not apply -1 to compute the end address until 3.12
    tmpSize = uiStart + uiSize;
#else
    tmpSize = uiStart + uiSize - 1;
#endif
    IMG_ASSERT(tmpSize > uiStart);  // too big! it did an overflow

    MMU_LogDebug("create genalloc pool of order %u\n", minAllocOrder);
    // -1: not using real inode
    pNeo->pPool = gen_pool_create(minAllocOrder, -1);

    if (pNeo->pPool == NULL)
    {
        *pResult = IMG_ERROR_MALLOC_FAILED;
        MMU_LogError("Failure to create the genalloc pool\n");
        IMG_FREE(pNeo);
        return NULL;
    }

    MMU_LogDebug("pool 0x%p order %u region from 0x%" IMG_PTRDPR "x for %" \
        IMG_SIZEPR "u bytes\n",
        pNeo->pPool, minAllocOrder, uiStart, uiSize);

    ret = gen_pool_add(pNeo->pPool, uiStart, uiSize, -1);
    if (ret != 0)
    {
        *pResult = IMG_ERROR_FATAL;
        MMU_LogError("Failure to configure the new genalloc pool: "\
            "returned %d\n", ret);
        gen_pool_destroy(pNeo->pPool);
        IMG_FREE(pNeo);
        return NULL;
    }

    // gen_pool_for_each_chunk(pNeo->pPool, &pool_crawler, pNeo->pPool);

    pNeo->sHeapInfo.uiVirtAddrStart = uiVirtAddrStart;
    pNeo->sHeapInfo.uiAllocAtom = uiAllocAtom;
    pNeo->sHeapInfo.uiSize = uiSize;

    *pResult = IMG_SUCCESS;
    return &(pNeo->sHeapInfo);
}

IMGMMU_HeapAlloc* IMGMMU_HeapAllocate(IMGMMU_Heap *pHeap, IMG_SIZE uiSize,
    IMG_RESULT *pResult)
{
    struct GEN_Heap *pInternalHeap = NULL;
    struct GEN_HeapAlloc *pNeo = NULL;

    IMG_ASSERT(pResult != NULL);
    IMG_ASSERT(pHeap != NULL);
    pInternalHeap = container_of(pHeap, struct GEN_Heap, sHeapInfo);

    if (uiSize%pHeap->uiAllocAtom != 0 || uiSize == 0)
    {
        MMU_LogError("invalid allocation size (0x%x)\n", uiSize);
        *pResult = IMG_ERROR_INVALID_PARAMETERS;
        return NULL;
    }

    pNeo = (struct GEN_HeapAlloc*)IMG_CALLOC(1, sizeof(struct GEN_HeapAlloc));
    if (pNeo == NULL)
    {
        MMU_LogError("failed to allocate internal structure\n");
        *pResult = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }
    MMU_LogDebug("heap 0x%p alloc %u\n", pInternalHeap->pPool, uiSize);

    /* gen_pool_alloc returns 0 on error - that is a problem when 1st valid
     * address is 0 - check HeapCreate for explanations */
    pNeo->sVirtualMem.uiVirtualAddress = gen_pool_alloc(pInternalHeap->pPool,
        uiSize);

    if (pNeo->sVirtualMem.uiVirtualAddress == 0)
    {
        MMU_LogError("failed to allocate from gen_pool_alloc\n");
        *pResult = IMG_ERROR_NOT_SUPPORTED;
        IMG_FREE(pNeo);
        return NULL;
    }

    MMU_LogDebug(KERN_INFO "heap 0x%p alloc 0x%p %u B atom %u B\n",
        pInternalHeap->pPool, pNeo->sVirtualMem.uiVirtualAddress, uiSize,
        pInternalHeap->sHeapInfo.uiAllocAtom);

    // if base address is 0 we applied an offset
    if (pInternalHeap->sHeapInfo.uiVirtAddrStart == 0)
    {
        pNeo->sVirtualMem.uiVirtualAddress -=
            pInternalHeap->sHeapInfo.uiAllocAtom;
    }
    pNeo->sVirtualMem.uiAllocSize = uiSize;
    pNeo->pHeap = pInternalHeap;

    pInternalHeap->uiNAlloc++;

    /* gen_pool_for_each_chunk(pInternalHeap->pPool, &pool_crawler,
     pInternalHeap->pPool); */

    *pResult = IMG_SUCCESS;
    return &(pNeo->sVirtualMem);
}

IMG_RESULT IMGMMU_HeapFree(IMGMMU_HeapAlloc *pAlloc)
{
    struct GEN_HeapAlloc *pInternalAlloc = NULL;
    IMG_UINTPTR uiAddress = 0;

    IMG_ASSERT(pAlloc != NULL);
    pInternalAlloc = container_of(pAlloc, struct GEN_HeapAlloc, sVirtualMem);

    IMG_ASSERT(pInternalAlloc->pHeap != NULL);
    IMG_ASSERT(pInternalAlloc->pHeap->pPool != NULL);
    IMG_ASSERT(pInternalAlloc->pHeap->uiNAlloc > 0);

    MMU_LogDebug("heap 0x%p free 0x%p %u B\n", pInternalAlloc->pHeap->pPool,
        pAlloc->uiVirtualAddress, pAlloc->uiAllocSize);

    /* gen_pool_for_each_chunk(pInternalAlloc->pHeap->pPool, &pool_crawler,.
     pInternalAlloc->pHeap->pPool); */

    uiAddress = pAlloc->uiVirtualAddress;
    // see the explanation in HeapCreate() to know why + uiAllocAtom
    if (pInternalAlloc->pHeap->sHeapInfo.uiVirtAddrStart == 0)
    {
        uiAddress += pInternalAlloc->pHeap->sHeapInfo.uiAllocAtom;
    }

    gen_pool_free(pInternalAlloc->pHeap->pPool, uiAddress,
        pAlloc->uiAllocSize);

    pInternalAlloc->pHeap->uiNAlloc--;

    IMG_FREE(pInternalAlloc);
    return IMG_SUCCESS;
}

IMG_RESULT IMGMMU_HeapDestroy(IMGMMU_Heap *pHeap)
{
    struct GEN_Heap *pInternalHeap = NULL;

    IMG_ASSERT(pHeap != NULL);
    pInternalHeap = container_of(pHeap, struct GEN_Heap, sHeapInfo);

    if (pInternalHeap->uiNAlloc > 0)
    {
        MMU_LogError("destroying a heap with non-freed allocation\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (pInternalHeap->pPool != NULL)
    {
        MMU_LogDebug("destroying genalloc pool 0x%p\n", pInternalHeap->pPool);
        gen_pool_destroy(pInternalHeap->pPool);
    }
    IMG_FREE(pInternalHeap);
    return IMG_SUCCESS;
}
