/**
******************************************************************************
 @file sys_mem_carveout.c

 @brief Implementation of Platform allocator for carved out memory
 
 Implementation of PCI carved out memory for IMG on board PCI memory.
 
 There is information about the device address and it can use the MMU heap
 to choose where to allocate in the reserved memory.
 
 Can be used as an example on carved out memory allocation.
 
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

******************************************************************************/

#include <ci_internal/sys_mem.h>

#include <target.h>
#include <mmulib/mmu.h> /* compiled value for device page size */
#include <mmulib/heap.h>
#include <felix_hw_info.h> /* SYSMEM_ALIGNMENT */
#include <ci_kernel/ci_kernel.h> /* g_psCIDriver, CI_FATAL */

#ifndef IMG_KERNEL_MODULE
#error should be only in kernel
#endif
/* Should be used with TAL light! */

#include <linux/mutex.h>
#include <linux/mm.h> /* vma_area_struct and remap_pfn_range */

#include <img_defs.h>
#include <img_errors.h>

#define VMALLOC_THRESH (2 * PAGE_SIZE)

// uncomment to add MMU page writes and CPU virt addresses translations
//#define debug_verbose

/**
 * @brief Used for physical linear alloc
 */
struct SYS_MEM_ALLOC
{
	/**
     * @brief Heap to manage the carved-out memory
     *
     * @note Virtual address is the CPU accessible address
     */
    IMGMMU_HeapAlloc *pHeapAlloc;
    /**
     * @brief physical address is the HW accessible address
     * (can be just an offset in DEV mem instead of global system address)
     * @note 64b to support extra addressing on 32b OS
     */
    IMG_UINT64 uiPhysAddress;
    /**
     * @brief list of physical addresses for Platform_MemGetPhysPages
     *
     * @note Each address of the list is separated by IMGMMU_GetPageSize()
     * which can be different from PAGE_SIZE
     */
    IMG_UINT64 *pPhysList;
};

static IMGMMU_Heap *g_carveoutMemHeap;
static DEFINE_MUTEX(g_memoryLock);

/* Assumes g_memoryLock is locked */
static IMG_RESULT verifyIfHeapCreated(void)
{
	IMG_RESULT ret = IMG_SUCCESS;

	if (g_carveoutMemHeap)
	{
		/* Carveout memory heap already allocated */
		return ret;
	}

	CI_DEBUG("Create memory heap in area where carveout memory is defined"\
        "CPUVirt=%u MemSize=%u\n",
        g_psCIDriver->pDevice->uiMemoryCPUVirtual,
        g_psCIDriver->pDevice->uiMemorySize);
	/*
	 * Create heap for platform defined memory space using physical addresses.
	 * Using PAGE_SIZE instead of SYSMEM_ALIGNMENT because it is going to be
	 * mapped to user-space.
	 */
	g_carveoutMemHeap = IMGMMU_HeapCreate(
			g_psCIDriver->pDevice->uiMemoryCPUVirtual,
			PAGE_SIZE,
			g_psCIDriver->pDevice->uiMemorySize,
			&ret);
	if (g_carveoutMemHeap == NULL)
	{
		CI_FATAL("Failed to create the physical memory heap!\n");
	}
	return ret;
}

/*
 * Heap is created in virtual memory space which is mapped directly to physical
 * carveout memory (contiguous). There is just need to allocate memory in
 * virtual mem space and calculate offset for physical memory.
 */
static IMG_UINT64 getPhysicalAddress(IMG_UINTPTR virtAddr)
{
	const IMG_UINT64 physBaseAddr = g_psCIDriver->pDevice->uiDevMemoryPhysical;
	const IMG_UINT64 offset = virtAddr - g_carveoutMemHeap->uiVirtAddrStart;

#ifdef debug_verbose
    CI_DEBUG("transform CPU virtual=0x%x (base 0x%x) to device physical 0x%x+0x%x=0x%x\n",
        (IMG_UINT32)virtAddr, (IMG_UINT32)g_carveoutMemHeap->uiVirtAddrStart, 
        (IMG_UINT32)physBaseAddr, (IMG_UINT32)offset, (IMG_UINT32)(physBaseAddr+offset));
#endif
    return physBaseAddr + offset;
}

/* Parameter 'extra' is a TAL memory handle */
struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE uiRealAlloc, void *extra, IMG_RESULT *ret)
{
    const IMG_UINT32 uiMMUPageSize = IMGMMU_GetPageSize();
	struct SYS_MEM_ALLOC *memAlloc = NULL;
    IMG_UINT32 numDevPages = uiRealAlloc/PAGE_SIZE;
	IMG_UINT32 i;
	(void)extra; /* unused */

	IMG_ASSERT(ret != NULL);

    /* PAGE_SIZE should be a multiple of MMU page size should (4096) */
    if (PAGE_SIZE < uiMMUPageSize)
    {
        *ret = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }
    if (PAGE_SIZE%uiMMUPageSize)
    {
        *ret = IMG_ERROR_NOT_SUPPORTED;
        return NULL;
    }
    
    if (uiRealAlloc % PAGE_SIZE != 0 || uiRealAlloc % uiMMUPageSize != 0)
    {
        *ret = IMG_ERROR_INVALID_PARAMETERS;
        goto err_alloc_size;
    }
    
    memAlloc = (struct SYS_MEM_ALLOC*)IMG_CALLOC(1, 
        sizeof(struct SYS_MEM_ALLOC));
	if (memAlloc == NULL)
	{
		*ret = IMG_ERROR_MALLOC_FAILED;
		goto err_psmem_failed;
	}

	mutex_lock(&g_memoryLock);
	if (verifyIfHeapCreated() == IMG_SUCCESS )
	{
		/* Allocate memory from previously defined memory heap */
		memAlloc->pHeapAlloc =
			IMGMMU_HeapAllocate(g_carveoutMemHeap, uiRealAlloc, ret);
	}
	mutex_unlock(&g_memoryLock);

	if (memAlloc->pHeapAlloc == NULL)
	{
		*ret = IMG_ERROR_MALLOC_FAILED;
		goto err_heap_alloc;
	}

	memAlloc->uiPhysAddress =
		getPhysicalAddress(memAlloc->pHeapAlloc->uiVirtualAddress);
        
    CI_DEBUG("using carveout memory cpu: 0x%" IMG_PTRDPR "x phys: 0x%" IMG_I64PR
             "x %"IMG_SIZEPR"u bytes\n",
            memAlloc->pHeapAlloc->uiVirtualAddress,
            memAlloc->uiPhysAddress,
            uiRealAlloc);

	/*
	 * List all physical pages if they were allocated by pages (otherwise page
	 * list is NULL).
	 * Allocation may be bigger than a few pages, we need to check if we should
	 * use vmalloc instead.
	 */
	if (numDevPages * sizeof(IMG_UINT64) > VMALLOC_THRESH)
	{
		/* Use vmalloc */
		memAlloc->pPhysList =
			(IMG_UINT64*)IMG_BIGALLOC(numDevPages * sizeof(IMG_UINT64));
	}
	else
	{
		memAlloc->pPhysList =
			(IMG_UINT64*)IMG_MALLOC(numDevPages * sizeof(IMG_UINT64));
	}

	if (memAlloc->pPhysList == NULL)
	{
		*ret = IMG_ERROR_MALLOC_FAILED;
		goto err_physlist_alloc;
	}

	for (i = 0; i < numDevPages; i++)
	{
        // the memory is carved out: it is linear - no need for complex computation
		memAlloc->pPhysList[i] = memAlloc->uiPhysAddress + i * PAGE_SIZE;
	}

	// this would call TALMEM_Malloc but TAL light does not do anything for allocations
	// TAL light just copies the memory pointer as the TAL Handle
	
	*ret = IMG_SUCCESS;
	return memAlloc;

err_physlist_alloc:
err_heap_alloc:
	IMG_FREE(memAlloc);
err_psmem_failed:
err_alloc_size:
    CI_FATAL("Failed to allocate memory!\n");
	return NULL;
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE uiRealAlloc, 
    void *extra, IMG_RESULT *ret)
{
    /* carved-out memory is allocated contiguously all the time */
	return Platform_MemAlloc(uiRealAlloc, extra, ret);
}

void Platform_MemFree(struct SYS_MEM_ALLOC *memAlloc)
{
    IMG_UINT32 numDevPages = memAlloc->pHeapAlloc->uiAllocSize/PAGE_SIZE;

	IMG_ASSERT(memAlloc != NULL);

	mutex_lock(&g_memoryLock);
	IMGMMU_HeapFree(memAlloc->pHeapAlloc);
	/* Check if there is no more memory in use */
	if (g_carveoutMemHeap->uiSize == 0)
	{
		CI_DEBUG("destroy carveout memory heap\n");
		IMGMMU_HeapDestroy(g_carveoutMemHeap);
		g_carveoutMemHeap = NULL;
	}
	mutex_unlock(&g_memoryLock);

	if (memAlloc->pPhysList != NULL)
	{
		/* Check if physical pages list was allocated using vmalloc */
		if (numDevPages * sizeof(IMG_UINT64) > VMALLOC_THRESH)
		{
			/* Was allocated using vmalloc */
			IMG_BIGFREE(memAlloc->pPhysList);
		}
		else
		{
			IMG_FREE(memAlloc->pPhysList);
		}
	}

	IMG_FREE(memAlloc);
}

void* Platform_MemGetMemory(SYS_MEM *psMem)
{
	IMG_ASSERT(psMem != NULL);
	IMG_ASSERT(psMem->pAlloc != NULL);

	return (void*)psMem->pAlloc->pHeapAlloc->uiVirtualAddress;
}

IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem)
{
	/* Mapped in non-cached area */
	return IMG_SUCCESS;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem)
{
	/* Mapped in non-cached area */
	return IMG_SUCCESS;
}

IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem)
{
	IMG_ASSERT(psMem != NULL);
	IMG_ASSERT(psMem->pAlloc != NULL);

	return psMem->pAlloc->pPhysList;
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE uiOffset)
{
	IMG_ASSERT(psMem != NULL);
	IMG_ASSERT(psMem->pAlloc != NULL);

	return psMem->pAlloc->uiPhysAddress + uiOffset;
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem, 
    struct vm_area_struct *vma)
{
    /*
     * Could use SYS_MemGetPhysPages() to get all pages but carveout memory is
     * contiguous
     */
    IMG_UINTPTR pAddr = Platform_MemGetDevMem(psMem, 0);
    if (0 == g_psCIDriver->pDevice->uiDevMemoryPhysical)
    {
        pAddr += g_psCIDriver->pDevice->uiMemoryPhysical;
    }

    CI_DEBUG("CI mmap phys 0x%" IMG_PTRDPR"x to user-space (page offset 0x%" \
        IMG_PTRDPR"x)\n", pAddr, pAddr>>PAGE_SHIFT);

    /* Convert to a page offset */
    vma->vm_pgoff = pAddr >> PAGE_SHIFT;
    /* Insure it is not cached */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                vma->vm_end - vma->vm_start, vma->vm_page_prot))
    {
        return IMG_ERROR_INTERRUPTED;
    }

    /* : add remap function and open and close to support forking */
    vma->vm_ops = NULL;
    /* Need that in case of forks */
    /* simple_vma_open(vma); */

    return IMG_SUCCESS;
}

IMG_HANDLE Platform_MemGetTalHandle(IMG_CONST SYS_MEM *psMem, 
    IMG_SIZE uiOffset, IMG_SIZE *pOff2)
{
    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    IMG_ASSERT(psMem->pAlloc->pHeapAlloc);
    IMG_ASSERT(pOff2);
    
    /* CPU virtual address is contiguous */
    *pOff2 = uiOffset;
	return (IMG_HANDLE)(psMem->pAlloc->pHeapAlloc->uiVirtualAddress);
}

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
    *ret = IMG_ERROR_NOT_SUPPORTED;
    return NULL;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC * mem)
{
    (void)mem;/* unused */
}

/*
 * MMU allocator
 */

struct SYS_MemPage
{
	struct MMUPage sPage;
	IMGMMU_HeapAlloc *psMem;
};

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
	struct SYS_MemPage *memPage = NULL;

	memPage = (struct SYS_MemPage*)IMG_MALLOC(sizeof(struct SYS_MemPage));

	mutex_lock(&g_memoryLock);
	if (verifyIfHeapCreated() == IMG_SUCCESS)
	{
		IMG_RESULT ret;
		memPage->psMem = IMGMMU_HeapAllocate(
				g_carveoutMemHeap, g_carveoutMemHeap->uiAllocAtom, &ret);
	}
	mutex_unlock(&g_memoryLock);

	if (memPage->psMem == NULL)
	{
		IMG_FREE(memPage);
		return NULL;
	}

	memPage->sPage.uiCpuVirtAddr = memPage->psMem->uiVirtualAddress;
	memPage->sPage.uiPhysAddr = getPhysicalAddress(
        memPage->psMem->uiVirtualAddress);
    
    CI_DEBUG("using carveout PCI memory (MMU) cpu: 0x%" IMG_PTRDPR "x phys: 0x%" \
        IMG_I64PR "x %"IMG_SIZEPR"u bytes\n", memPage->sPage.uiCpuVirtAddr,
        memPage->sPage.uiPhysAddr, memPage->psMem->uiAllocSize);

	return &(memPage->sPage);
}

void Platform_MMU_MemPageFree(struct MMUPage *mmuPage)
{
	struct SYS_MemPage *memPage =
		container_of(mmuPage, struct SYS_MemPage, sPage);
	IMG_ASSERT(mmuPage != NULL);

	mutex_lock(&g_memoryLock);
	if (verifyIfHeapCreated() == IMG_SUCCESS)
	{
        CI_DEBUG("free PCI memory (MMU) cpu: 0x%" IMG_PTRDPR "x phys: 0x%" \
            IMG_I64PR "x %" IMG_SIZEPR "u bytes)\n",
            memPage->sPage.uiCpuVirtAddr,
            memPage->sPage.uiPhysAddr,
            memPage->psMem->uiAllocSize);
		IMGMMU_HeapFree(memPage->psMem);
		/* Check if there is no more memory allocated on carveout heap */
		if (g_carveoutMemHeap->uiSize == 0)
		{
			CI_DEBUG("Destroy carveout memory heap\n");
			IMGMMU_HeapDestroy(g_carveoutMemHeap);
			g_carveoutMemHeap = NULL;
		}
	}
	mutex_unlock(&g_memoryLock);

	IMG_FREE(memPage);
}

void Platform_MMU_MemPageWrite(struct MMUPage *pWriteTo, unsigned int offset, 
    IMG_UINT64 uiToWrite, unsigned int flags)
{
	IMG_UINT32 *pDirMemory = NULL;
	IMG_UINT64 uiCurrPhysAddr = uiToWrite;
	IMG_UINT32 extAddress = 0;

    struct SYS_MemPage *pPage = NULL;

    IMG_ASSERT(pWriteTo != NULL);
    pPage = container_of(pWriteTo, struct SYS_MemPage, sPage);
	
	pDirMemory = (IMG_UINT32*)pWriteTo->uiCpuVirtAddr;

	/*
	 * Shifts to do to make the extended address fits into the entries of the
	 * MMU pages.
	 */
	extAddress = KRN_CI_MMUSize(&(g_psCIDriver->sMMU)) - 32;

	/*
	 * Assumes that the MMU HW has the extra-bits enabled (this simple function
	 * as no way of knowing).
	 */
	if (extAddress > 0)
	{
		uiCurrPhysAddr >>= extAddress;
	}

	pDirMemory[offset] = (IMG_UINT32)uiCurrPhysAddr | (flags);
    if ( flags != 0 ) // otherwise polluted when initialising pages...
    {
#ifdef debug_verbose
        CI_DEBUG("(MMU) write 0x%x at 0x%"IMG_I64PR"x[+0x%x]\n", 
             (IMG_UINT32)uiCurrPhysAddr | (flags), 
             pPage->sPage.uiPhysAddr, offset);
#endif
    }
}

IMG_UINT32 Platform_MMU_MemPageRead(struct MMUPage *pReadFrom, unsigned int offset)
{
	/* Should not be used - internal MMU function used */
	IMG_ASSERT(IMG_FALSE);
	return 0;
}



/*
 * PDP is a specific part of the FPGA - may not apply to all devices
 */

#include "ci_internal/ci_pdp.h"
#include <asm/io.h>

IMG_RESULT Platform_MemWritePDPAddress(const SYS_MEM *pToWrite, 
    IMG_SIZE uiToWriteOffset)
{
	IMG_UINT32 uiVal = 0;

	IMG_ASSERT(pToWrite != NULL);
	IMG_ASSERT(pToWrite->pAlloc != NULL);

	if (uiToWriteOffset >= pToWrite->uiAllocated)
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (pToWrite->pDevVirtAddr != NULL)
	{
		IMG_UINT64 *aPhysList = Platform_MemGetPhysPages(pToWrite);
		uiVal = (IMG_UINT32)aPhysList[0] + uiToWriteOffset;
	}
	else
	{
		/* Device can only use 32b addresses so it should not be 64b anyway */
		uiVal = (IMG_UINT32)Platform_MemGetDevMem(pToWrite, uiToWriteOffset);
	}

    CI_DEBUG("writing PDP address 0x%x with flag 0x%x @ 0x%x\n",
        uiVal, BAR0_ENABLE_PDP_BIT, BAR0_PDP_OFFSET+BAR0_PDP_ADDRESS);
    iowrite32(
        uiVal >> 4 | BAR0_ENABLE_PDP_BIT,
        (void*)(g_psCIDriver->pDevice->uiBoardCPUVirtual + BAR0_PDP_OFFSET
        + BAR0_PDP_ADDRESS));

	return IMG_SUCCESS;
}
