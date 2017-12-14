/**
 ******************************************************************************
 @file sys_mem_pagealloc.c

 @brief Allocator implementation using page alloc (contiguous virtual memory)

 @note No changes should be needed if PAGE_SIZE >= device MMU page size but
 untested

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
#include <ci_internal/sys_mem.h>

#include <target.h>
#include <mmulib/mmu.h>
#include <mmulib/heap.h>
#include <ci_internal/sys_mem.h>
#include <ci_kernel/ci_kernel.h>

#include <img_defs.h>
#include <img_errors.h>

#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>

#define VMALLOC_THRESH (2 * PAGE_SIZE)

/**
 * @brief Used for physical linear alloc
 */
struct SYS_MEM_ALLOC
{
    unsigned numPages;
    /**
     * == NULL if contiguous otherwise allocated using vmalloc or kmalloc
     * according to its size
     */
    struct page **pages;
    /**
     * created at allocation time using vmalloc or kmalloc according to its
     * size
     */
    IMG_UINT64 *physList;
    /**
     * result of kmalloc() when contiguous - result of vmap() otherwise
     */
    void *virtMem;
};

static int populatePhysList(struct SYS_MEM_ALLOC *pAlloc)
{
    unsigned pg_i;
    IMG_UINT64 *physAddrs = NULL;
    int usevmalloc = 0;

    IMG_ASSERT(pAlloc != NULL);
    IMG_ASSERT(pAlloc->physList == NULL);
    IMG_ASSERT(pAlloc->numPages > 0);

    usevmalloc = (pAlloc->numPages * (sizeof(*physAddrs)) > VMALLOC_THRESH);
    if (usevmalloc)
    {
        physAddrs = (IMG_UINT64 *)IMG_BIGALLOC(
            pAlloc->numPages * (sizeof(*physAddrs)));
    }
    else
    {
        physAddrs = (IMG_UINT64 *)IMG_MALLOC(
            pAlloc->numPages * (sizeof(*physAddrs)));
    }

    if (!physAddrs)
    {
        CI_FATAL("failed to allocate list of physical addresses\n");
        return 1;
    }

    if (pAlloc->pages != NULL)
    {
        for (pg_i = 0; pg_i < pAlloc->numPages; ++pg_i)
        {
            physAddrs[pg_i] = page_to_pfn(pAlloc->pages[pg_i]) << PAGE_SHIFT;
            /* Invalidate the region (whether it's cached or not) */
            /*dma_map_page(g_psCIDriver->pDevice->data,
                psMem->pAlloc->pages[pg_i], 0, PAGE_SIZE, DMA_FROM_DEVICE);*/
        }
    }
    else
    {
        // contiguous allocation
        uintptr_t physAddr = virt_to_phys(pAlloc->virtMem);

        for (pg_i = 0; pg_i < pAlloc->numPages; ++pg_i)
        {
            physAddrs[pg_i] = (physAddr + pg_i*PAGE_SIZE) << PAGE_SHIFT;
        }
    }

    pAlloc->physList = physAddrs;

    return 0;
}

/* 'extra' contains TAL memory handle */
struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE uiRealAlloc, void *extra,
    IMG_RESULT *ret)
{
    const IMG_UINT32 uiMMUPageSize = IMGMMU_GetPageSize();
    unsigned pg_i, pgrm_i, numPages;
    struct page **pages;
    pgprot_t pageProt = PAGE_KERNEL;
    struct SYS_MEM_ALLOC *pAlloc = NULL;
    int usevmalloc = 0;

    IMG_ASSERT(ret != NULL);
    IMG_ASSERT((uiRealAlloc % PAGE_SIZE) == 0);
    /* ensure we can map it later */
    IMG_ASSERT((uiRealAlloc % uiMMUPageSize) == 0);

    numPages = uiRealAlloc / PAGE_SIZE;

    pAlloc = (struct SYS_MEM_ALLOC*)IMG_CALLOC(1, sizeof(*pAlloc));
    if (pAlloc == NULL)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to allocate internal structure\n");
        goto err_alloc;
    }

    usevmalloc = ((numPages * sizeof(*pages)) > VMALLOC_THRESH);
    if (usevmalloc)
    {
        pages = IMG_BIGALLOC(numPages * (sizeof(*pages)));
    }
    else
    {
        pages = IMG_MALLOC(numPages * (sizeof(*pages)));
    }
    if (!pages)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to allocate list of pages\n");
        goto err_pages;
    }

    for (pg_i = 0; pg_i < numPages; ++pg_i)
    {
        /* allocate physical address */
        pages[pg_i] = alloc_page(GFP_KERNEL);
        if (!pages[pg_i])
        {
            *ret = IMG_ERROR_MALLOC_FAILED;
            CI_FATAL("Failed to allocate a page\n");
            goto err_page_alloc;
        }
    }

    pAlloc->numPages = numPages;
    pAlloc->pages = pages;
    /* Use uncached ... */
    pageProt = pgprot_noncached(pageProt);
    pAlloc->virtMem = vmap(pages, numPages, VM_MAP, pageProt);
    if (pAlloc->virtMem == NULL)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to map to CPU virtual space\n");
        goto err_page_alloc;
    }

    /* populate the list of physical addresses */
    if (populatePhysList(pAlloc) != 0)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to populate physical list\n");
        goto err_page_pop;
    }

    *ret = IMG_SUCCESS;
    return pAlloc;

err_page_pop:
    vunmap(pAlloc->virtMem);
err_page_alloc:
    for (pgrm_i = 0; pgrm_i < pg_i; ++pgrm_i) {
        __free_pages(pages[pgrm_i], 0);
    }
    IMG_BIGFREE(pages);
err_pages:
    IMG_FREE(pAlloc);
    pAlloc = NULL;
err_alloc:
    return pAlloc;
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE uiRealAlloc,
    void *extra, IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *pAlloc = NULL;
    IMG_ASSERT(ret != NULL);

    pAlloc = (struct SYS_MEM_ALLOC*)IMG_CALLOC(1, sizeof(*pAlloc));
    if (pAlloc == NULL)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto err_alloc;
    }

    pAlloc->virtMem = kmalloc(uiRealAlloc, GFP_KERNEL);
    if (pAlloc->virtMem == NULL)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto err_kmalloc;
    }

    // populate the list of physical addresses
    if (populatePhysList(pAlloc) != 0)
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to populate physical list\n");
        goto err_page_pop;
    }

    *ret = IMG_SUCCESS;
    return pAlloc;

err_page_pop:
    kfree(pAlloc->virtMem);
err_kmalloc:
    IMG_FREE(pAlloc);
    pAlloc = NULL;
err_alloc:
    return pAlloc;
}

void Platform_MemFree(struct SYS_MEM_ALLOC *pAlloc)
{
    unsigned pg_i;
    int usevmalloc = 0;

    IMG_ASSERT(pAlloc != NULL);

    if (pAlloc->pages != NULL)
    {
        vunmap(pAlloc->virtMem);

        for (pg_i = 0; pg_i < pAlloc->numPages; ++pg_i) {
            __free_pages(pAlloc->pages[pg_i], 0);
        }

        usevmalloc = ((pAlloc->numPages * sizeof(*(pAlloc->pages)))
        > VMALLOC_THRESH);
        if (usevmalloc)
        {
            IMG_BIGFREE(pAlloc->pages);
        }
        else
        {
            IMG_FREE(pAlloc->pages);
        }
    }
    else
    {
        // contiguous allocation
        kfree(pAlloc->virtMem);
    }

    usevmalloc = (pAlloc->numPages * (sizeof(*(pAlloc->physList)))
    > VMALLOC_THRESH);
    if (usevmalloc)
    {
        IMG_BIGFREE(pAlloc->physList);
    }
    else
    {
        IMG_FREE(pAlloc->physList);
    }
    IMG_FREE(pAlloc);
}

void* Platform_MemGetMemory(SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    return psMem->pAlloc->virtMem;
}

IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    /*dma_sync_single_for_cpu(g_psCIDriver.pDev, psMem->pAlloc->virtMem,
        PAGES_SIZE, DMA_FROM_DEVICE);*/
    return IMG_SUCCESS;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    /*dma_sync_single_for_device(g_psCIDriver.pDev, psMem->pAlloc->virtMem,
        PAGES_SIZE, DMA_TO_DEVICE);*/
    return IMG_SUCCESS;
}

IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);
    IMG_ASSERT(psMem->pAlloc->physList != NULL);

    return psMem->pAlloc->physList;
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE uiOffset)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    return (IMG_UINTPTR)psMem->pAlloc->virtMem;
}

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
    *ret = IMG_ERROR_NOT_SUPPORTED;
    return NULL;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC *pAlloc)
{
    /* Not used */
    (void)pAlloc;
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem,
struct vm_area_struct *vma)
{
    IMG_UINT64 *physPages = Platform_MemGetPhysPages(psMem);

    IMG_UINT32 noPages = (vma->vm_end - vma->vm_start) / PAGE_SIZE;
    IMG_UINT32 pageIdx = 0;
    IMG_UINT32 start = vma->vm_start;
    IMG_UINT64 pgoff = 0;

    IMG_ASSERT(physPages);
    IMG_ASSERT(!((vma->vm_end - vma->vm_start) % PAGE_SIZE));

    /* Convert to a page offset */
    vma->vm_pgoff = physPages[0] >> PAGE_SHIFT;
    /* Insure it is not cached */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    for (pageIdx = 0; pageIdx < noPages; pageIdx++)
    {
        pgoff = physPages[pageIdx] >> PAGE_SHIFT;  // convert to a page offset

        if (remap_pfn_range(vma, start, pgoff, PAGE_SIZE, vma->vm_page_prot))
        {
            CI_FATAL("Failed to remap page\n");
            return IMG_ERROR_INTERRUPTED;
        }
        start += PAGE_SIZE;
    }

    vma->vm_ops = NULL;
    /// @ add remap function and open and close to support forking
    /*simple_vma_open(vma);*/ /* need that in case of forks*/

    return IMG_SUCCESS;
}

IMG_HANDLE Platform_MemGetTalHandle(const SYS_MEM *psMem, IMG_SIZE uiOffset,
    IMG_SIZE *pOff2)
{
    IMG_ASSERT(pOff2 != NULL);
    IMG_ASSERT(psMem != NULL);

    *pOff2 = uiOffset;
    return psMem->pAlloc->virtMem;
}

/*
 * MMU functions
 */

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
    struct MMUPage *mmuPage;
    struct page *page;

    mmuPage = (struct MMUPage *)IMG_CALLOC(1, sizeof(*mmuPage));
    if (mmuPage == NULL)
    {
        CI_FATAL("Failed to allocate memory for MMU page structure\n");
        goto err_alloc;
    }

    page = alloc_page(GFP_KERNEL);
    if (page == NULL)
    {
        CI_FATAL("Failed to allocate page from kernel\n");
        goto err_page;
    }

    mmuPage->uiCpuVirtAddr = (IMG_UINTPTR)page_address(page);
    mmuPage->uiPhysAddr = page_to_pfn(page) << PAGE_SHIFT;
    return mmuPage;

err_page:
    IMG_FREE(mmuPage);
err_alloc:
    return NULL;
}

void Platform_MMU_MemPageFree(struct MMUPage *mmuPage)
{
    struct page *page = pfn_to_page(mmuPage->uiPhysAddr >> PAGE_SHIFT);

    __free_pages(page, 0);
    IMG_FREE(mmuPage);
}

void Platform_MMU_MemPageWrite(struct MMUPage *pageWriteTo,
    unsigned int offset, IMG_UINT64 toWrite, unsigned int flags)
{
    IMG_UINT32 *dirMemory = NULL;
    IMG_UINT64 currPhysAddr = toWrite;
    IMG_UINT32 extAddress = 0;

    IMG_ASSERT(pageWriteTo != NULL);

    dirMemory = (IMG_UINT32 *)pageWriteTo->uiCpuVirtAddr;

    /*
     * Shifts to do to make the extended address fits into the entries of the
     * MMU pages.
     */
    extAddress = KRN_CI_MMUSize(&(g_psCIDriver->sMMU)) - 32;

    /*
     * Assumes that the MMU HW has the extra-bits enabled (this simple function
     * has no way of knowing).
     */
    if (extAddress > 0) {
        CI_DEBUG("shift addresses for MMU by %d bits\n", extAddress);
        currPhysAddr >>= extAddress;
    }

    /*
     * The IMGMMU_PAGE_SHIFT bottom bits should be masked because page
     * allocation IMGMMU_PAGE_SHIFT - (IMGMMU_PHYS_SIZE - IMGMMU_VIRT_SIZE)
     * are used for flags so it's ok.
     */
    dirMemory[offset] = (IMG_UINT32)currPhysAddr | (flags);
}

/*
IMG_UINT32 Platform_MMU_MemPageRead(struct MMUPage *pReadFrom,
    unsigned int offset)
{
    // Should not be needed - internal MMU function used
    // if needed add it to KRN_CI_MMUDirCreate()
    IMG_ASSERT(IMG_FALSE);
    return 0;
}*/

void Platform_MMU_MemPageUpdate(struct MMUPage *pPage)
{
    /* Should invalidate the cache so that this page has the correct data in
    * memory
    * For FPGA the memory is uncached so no need to do anything
    */
}

#ifdef CI_PDP

#include "ci_internal/ci_pdp.h"
#include "ci_kernel/ci_kernel.h"

IMG_RESULT Platform_MemWritePDPAddress(const SYS_MEM *pToWrite,
    IMG_SIZE uiToWriteOffset)
{
    /*
     * For proper display PDP needs contiguous physical memory which is not the
     * case for page alloc allocator.
     */
    return IMG_ERROR_NOT_SUPPORTED;
}
#endif /* CI_PDP */
