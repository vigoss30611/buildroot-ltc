/**
******************************************************************************
@file sys_mem_ion.c

@brief implementation of SYS_MEM using ion for android

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

/*
 * gcc defines "linux" as "1".
 * [ http://stackoverflow.com/questions/19210935 ]
 * IMG_KERNEL_ION_HEADER can be <linux/ion.h>, which expands to <1/ion.h>
 */
#undef linux
#if !defined(IMG_KERNEL_ION_HEADER)
#error Define IMG_KERNEL_ION_HEADER pointing to ion.h!
#endif
#if !defined(IMG_KERNEL_ION_PRIV_HEADER)
#error Define IMG_KERNEL_ION_PRIV_HEADER pointing to ion_priv.h!
#endif

#include <ci_internal/sys_mem.h>
#include <ci_kernel/ci_kernel.h>
#include <linux/vmalloc.h>
#include <linux/scatterlist.h>
#include IMG_KERNEL_ION_HEADER
#include IMG_KERNEL_ION_PRIV_HEADER
#include <mmulib/mmu.h>
#include <mmulib/heap.h>
#include <img_defs.h>
#include <img_errors.h>
#include <target.h>

/* Use system memory heap (ION_SYSTEM_HEAP) in case of
 * Android emulator - this is unified memory model.
 */
#if !defined(ALLOC_SYSTEM_MEM) && !defined(ALLOC_CARVEOUT_MEM)
  #error "Type of memory allocation not defined!"
#endif

static DEFINE_MUTEX(ionMemoryLock);

struct SYS_MEM_ALLOC {
    struct ion_handle *ionHandle;
    struct ion_client *ionClient;

    void *virtAddr;
    ion_phys_addr_t physAddr;
    size_t physLength;

    IMG_UINT64 *devPhysList;
    IMG_UINT64 *cpuPhysList;

    IMG_BOOL kernelMapped;
    IMG_BOOL kernelImported;
};

struct SYS_MEM_PageIon {
    struct ion_handle *ionHandle;
    struct ion_client *ionClient;
    struct MMUPage mmuPage;
};

enum physicalAddrType {
	DEVICE_PHYS_ADDRESS,
	CPU_PHYS_ADDRESS
};

static struct ion_client* getIonClient(void)
{
	static struct ion_client *ion_client = NULL;
	struct ion_device *ion_dev = NULL;

	if (ion_client)
		return ion_client;

    /* Make sure that the ion_device_create() will always create a singleton */
	ion_dev = ion_device_create(NULL);

	if (!ion_dev) {
		CI_FATAL("ION device is NULL!\n");
		return NULL;
	}

	ion_client = ion_client_create(ion_dev, "felix_ion");

	if (!ion_client)
		CI_FATAL("ion_client_create failed!\n");

	return ion_client;
}

/*
 * Device sees carveout memory at different address than cpu (from PCI perspective).
 * When system memory is used (bus master mode is enabled), physical addresses
 * for device and cpu are the same.
 */
static ion_phys_addr_t getDevMemPhysAddress(ion_phys_addr_t cpuDevMemPhysAddr)
{
#if defined(ALLOC_CARVEOUT_MEM)
	/* Calculate physical address offset and add it to the physical address
	 * seen by device.
	 * uiDevMemoryPhysical - start of carveout memory seen by device
	 * uiMemoryPhysical - start of carveout memory seen by CPU
	 * physAddr - physical memory address allocated by ION seen by CPU
	 */
	ion_phys_addr_t offset = cpuDevMemPhysAddr - g_psCIDriver->pDevice->uiMemoryPhysical;
    return g_psCIDriver->pDevice->uiDevMemoryPhysical + offset;
#else
    return cpuDevMemPhysAddr;
#endif
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE size,
        void *extra, IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *mem = NULL;
#if defined(ALLOC_CARVEOUT_MEM)
	unsigned int heap_id_mask = ION_HEAP_CARVEOUT_MASK;
#else
	unsigned int heap_id_mask = ION_HEAP_SYSTEM_CONTIG_MASK;
#endif

    mutex_lock(&ionMemoryLock);
    IMG_ASSERT(ret != NULL);
    mem = (struct SYS_MEM_ALLOC *)IMG_CALLOC(1, sizeof(*mem));

    if (!mem) {
        CI_FATAL("IMG_CALLOC failed!\n");
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto error_calloc;
    }

    mem->ionClient = getIonClient();
    // assumption: size is already aligned by sys mem
    mem->ionHandle = ion_alloc(mem->ionClient, size, 0, heap_id_mask, 0);

    if (IS_ERR_OR_NULL(mem->ionHandle)) {
        CI_FATAL("ion_alloc failed!\n");
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto error_ion_alloc;
    }

    // get physical address
    if (ion_phys(mem->ionClient, mem->ionHandle, &mem->physAddr,
                &mem->physLength)) {
        CI_FATAL("ion_phys failed\n");
        *ret = IMG_ERROR_FATAL;
        goto error_ion_phys;
    }

    IMG_ASSERT(mem->physAddr);
    IMG_ASSERT(mem->physLength);

    // get virtual address
    mem->virtAddr = ion_map_kernel(mem->ionClient, mem->ionHandle);
    IMG_ASSERT(mem->virtAddr);

    mutex_unlock(&ionMemoryLock);
    mem->kernelMapped = true;
    mem->kernelImported = false;

    *ret = IMG_SUCCESS;

    return mem;
error_ion_phys:
    ion_free(mem->ionClient, mem->ionHandle);
error_ion_alloc:
    IMG_FREE(mem);
error_calloc:
    mutex_unlock(&ionMemoryLock);
    return NULL;
}

struct SYS_MEM_ALLOC * Platform_MemAlloc(IMG_SIZE size, void *extra,
        IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *mem = NULL;
#if defined(ALLOC_CARVEOUT_MEM)
	unsigned int heap_id_mask = ION_HEAP_CARVEOUT_MASK;
#else
	unsigned int heap_id_mask = ION_HEAP_SYSTEM_MASK;
#endif

    mutex_lock(&ionMemoryLock);
    IMG_ASSERT(ret != NULL);
    mem = (struct SYS_MEM_ALLOC *)IMG_CALLOC(1, sizeof(*mem));

    if (!mem) {
        CI_FATAL("IMG_CALLOC failed!\n");
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto error_calloc;
    }

    mem->ionClient = getIonClient();
    // assumption: size is already aligned by sys mem
    mem->ionHandle = ion_alloc(mem->ionClient, size, 0, heap_id_mask, 0);

    if (IS_ERR_OR_NULL(mem->ionHandle)) {
        CI_FATAL("ion_alloc failed!\n");
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto error_ion_alloc;
    }

    // get virtual address
    mem->virtAddr = ion_map_kernel(mem->ionClient, mem->ionHandle);
    IMG_ASSERT(mem->virtAddr);

    mutex_unlock(&ionMemoryLock);
    mem->kernelMapped = true;
    mem->kernelImported = false;

    *ret = IMG_SUCCESS;

    return mem;
error_ion_alloc:
    IMG_FREE(mem);
error_calloc:
    mutex_unlock(&ionMemoryLock);
    return NULL;
}

void Platform_MemFree(struct SYS_MEM_ALLOC *mem)
{
    IMG_ASSERT(mem->ionClient);
    IMG_ASSERT(mem->ionHandle);

    mutex_lock(&ionMemoryLock);
    if (mem->kernelMapped)
        ion_unmap_kernel(mem->ionClient, mem->ionHandle);

    ion_free(mem->ionClient, mem->ionHandle);

    if (mem->devPhysList)
            IMG_BIGFREE(mem->devPhysList);

    if (mem->cpuPhysList)
            IMG_BIGFREE(mem->cpuPhysList);

    IMG_FREE(mem);
    mutex_unlock(&ionMemoryLock);
}

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
    ion_phys_addr_t physAddr = 0;
    struct SYS_MEM_PageIon * mmuPageIon = NULL;
    size_t physLength = 0;
#if defined(ALLOC_CARVEOUT_MEM)
	/*
	 * MMU pages should be allocated in carveout memory as system contiguous
	 * is in different address space and device has no access to it.
	 */
	unsigned int heap_id_mask = ION_HEAP_CARVEOUT_MASK;
#else
	unsigned int heap_id_mask = ION_HEAP_SYSTEM_CONTIG_MASK;
#endif

    mutex_lock(&ionMemoryLock);

    mmuPageIon = (struct SYS_MEM_PageIon *)IMG_CALLOC(1, sizeof(*mmuPageIon));

    if (!mmuPageIon) {
        CI_FATAL("IMG_CALLOC failed!\n");
        goto error_calloc;
    }

    mmuPageIon->ionClient = getIonClient();
    mmuPageIon->ionHandle = ion_alloc(mmuPageIon->ionClient, PAGE_SIZE,
            PAGE_SIZE, heap_id_mask, 0);

    if (IS_ERR_OR_NULL(mmuPageIon->ionHandle)) {
        CI_FATAL("ion_alloc failed!\n");
        goto error_ion_alloc;
    }

    mmuPageIon->mmuPage.uiCpuVirtAddr = (IMG_UINTPTR)ion_map_kernel(
            mmuPageIon->ionClient, mmuPageIon->ionHandle);
    IMG_ASSERT(mmuPageIon->mmuPage.uiCpuVirtAddr);

    if (ion_phys(mmuPageIon->ionClient, mmuPageIon->ionHandle, &physAddr,
                &physLength)) {
        CI_FATAL("ion_phys failed!\n");
        goto error_ion_phys;
    }

    IMG_ASSERT(physAddr);
    mmuPageIon->mmuPage.uiPhysAddr = getDevMemPhysAddress(physAddr);

    mutex_unlock(&ionMemoryLock);

    return &mmuPageIon->mmuPage;
error_ion_phys:
    ion_unmap_kernel(mmuPageIon->ionClient, mmuPageIon->ionHandle);
error_ion_alloc:
    mutex_unlock(&ionMemoryLock);
    IMG_FREE(mmuPageIon);
error_calloc:
    return NULL;
}

void Platform_MMU_MemPageFree(struct MMUPage *mmuPage)
{
    struct SYS_MEM_PageIon *mmuPageIon = NULL;
    IMG_ASSERT(mmuPage);

    mmuPageIon = container_of(mmuPage, struct SYS_MEM_PageIon, mmuPage);

    IMG_ASSERT(mmuPageIon->ionClient);
    IMG_ASSERT(mmuPageIon->ionHandle);

    mutex_lock(&ionMemoryLock);
    ion_unmap_kernel(mmuPageIon->ionClient, mmuPageIon->ionHandle);
    ion_free(mmuPageIon->ionClient, mmuPageIon->ionHandle);
    IMG_FREE(mmuPageIon);
    mutex_unlock(&ionMemoryLock);
}

void Platform_MMU_MemPageWrite(struct MMUPage *pageWriteTo, unsigned int offset,
        IMG_UINT64 toWrite, unsigned int flags)
{
    IMG_UINT32 *dirMemory = NULL;
    IMG_UINT64 currPhysAddr = toWrite;
    IMG_UINT32 extAddress = 0;

    IMG_ASSERT(pageWriteTo != NULL);

    dirMemory = (IMG_UINT32 *)pageWriteTo->uiCpuVirtAddr;

    // Shifts to do to make the extended address fits into the entries of the
    // MMU pages.
    extAddress = KRN_CI_MMUSize(&(g_psCIDriver->sMMU)) - 32;

    // Assumes that the MMU HW has the extra-bits enabled (this simple function
    // has no way of knowing).
    if (extAddress > 0) {
#ifndef ANDROID
        CI_DEBUG("shift addresses for MMU by %d bits\n", extAddress);
#endif
        currPhysAddr >>= extAddress;
    }

    // The IMGMMU_PAGE_SHIFT bottom bits should be masked because page
    // allocation IMGMMU_PAGE_SHIFT - (IMGMMU_PHYS_SIZE - IMGMMU_VIRT_SIZE)
    // are used for flags so it's ok.
    dirMemory[offset] = (IMG_UINT32)currPhysAddr | (flags);
}

// can be used to verify the buffer size
// but is approximate (to the page) and expensive as going through the list
static IMG_UINT32 getBuffefLength(struct ion_client *ionClient,
        struct ion_handle *ionHandle)
{
    IMG_UINT32 buffLength = 0;
    struct scatterlist *scatterList = NULL;
    struct sg_table *sgTable = NULL;

    IMG_ASSERT(ionClient);
    IMG_ASSERT(ionHandle);

    sgTable = ion_sg_table(ionClient, ionHandle);
    IMG_ASSERT(sgTable);

    scatterList = sgTable->sgl;

    while (scatterList) {
        buffLength += scatterList->length;
        scatterList = sg_next(scatterList);
    }

    CI_DEBUG("buffer length = 0x%x\n", buffLength);
    IMG_ASSERT(buffLength);

    return buffLength;
}

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC * mem = NULL;

    IMG_ASSERT(allocationSize);

    mutex_lock(&ionMemoryLock);
    mem = (struct SYS_MEM_ALLOC *)IMG_CALLOC(1, sizeof(*mem));

    if (!mem) {
        CI_FATAL("IMG_CALLOC failed!\n");
        *ret = IMG_ERROR_MALLOC_FAILED;
        goto error_calloc;
    }

    mem->ionClient = getIonClient();
    CI_DEBUG("importing buffer from user space fd=%#x\n", fd);
    mem->ionHandle = ion_import_dma_buf(mem->ionClient, fd);

    if (IS_ERR_OR_NULL(mem->ionHandle)) {
        CI_FATAL("ion_import_dma_buf failed! err=0x%x\n", mem->ionHandle);
        mutex_unlock(&ionMemoryLock);
        *ret = IMG_ERROR_FATAL;
        goto error_import_buf;
    }

    // get virtual address
    mem->virtAddr = ion_map_kernel(mem->ionClient, mem->ionHandle);
    IMG_ASSERT(mem->virtAddr);

    mem->kernelMapped = true;
    mem->kernelImported = true;

    //*allocationSize = getBuffefLength(mem->ionClient, mem->ionHandle);

    mutex_unlock(&ionMemoryLock);
    *ret = IMG_SUCCESS;

    return mem;
error_import_buf:
    IMG_FREE(mem);
error_calloc:
    mutex_unlock(&ionMemoryLock);
    return NULL;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC *mem)
{
    IMG_ASSERT(mem);
    IMG_ASSERT(mem->kernelImported);

    mutex_lock(&ionMemoryLock);
    if (mem->kernelMapped) {
        ion_unmap_kernel(mem->ionClient, mem->ionHandle);
        ion_free(mem->ionClient, mem->ionHandle);
        mem->kernelMapped = false;
        mem->kernelImported = false;
    }
    IMG_FREE(mem);
    mutex_unlock(&ionMemoryLock);
}

IMG_HANDLE Platform_MemGetTalHandle(IMG_CONST SYS_MEM *psMem, IMG_SIZE uiOffset, IMG_SIZE *pOff2)
{
    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    IMG_ASSERT(pOff2);

    // CPU virtual address is contiguous
    *pOff2 = uiOffset;
	return (IMG_HANDLE)psMem->pAlloc->virtAddr;
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *mem, IMG_SIZE offset)
{
    IMG_ASSERT(mem);
    IMG_ASSERT(mem->pAlloc);
    IMG_ASSERT(mem->pAlloc->physAddr);
    IMG_ASSERT(mem->pAlloc->physLength);

    if (offset >= mem->pAlloc->physLength)
        return IMG_ERROR_INVALID_PARAMETERS;

    return (IMG_UINTPTR)(getDevMemPhysAddress(mem->pAlloc->physAddr) + offset);
}

IMG_RESULT Platform_MemUpdateHost(SYS_MEM *mem)
{
    return IMG_SUCCESS;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *mem)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(mem);

    return ret;
}

void* Platform_MemGetMemory(SYS_MEM *mem)
{
    IMG_ASSERT(mem);
    IMG_ASSERT(mem->pAlloc);
    IMG_ASSERT(mem->pAlloc->virtAddr);

    return mem->pAlloc->virtAddr;
}

static IMG_UINT64* memGetPhysPages(const SYS_MEM *mem, enum physicalAddrType addrType)
{
	IMG_UINT64 *physList;
	IMG_UINT64 **reqPhysList	=
		addrType == DEVICE_PHYS_ADDRESS
		? &mem->pAlloc->devPhysList
		: &mem->pAlloc->cpuPhysList ;
	IMG_UINT32 pageIdx = 0;
	IMG_UINT32 noPages = (mem->uiAllocated + PAGE_SIZE - 1) / PAGE_SIZE;
	struct scatterlist *scatterList = NULL;
	struct sg_table *sgTable = NULL;

	IMG_ASSERT(mem);
	IMG_ASSERT(mem->pAlloc);
	IMG_ASSERT(mem->uiAllocated);
	/* Check if list of physical pages is already allocated */
	if (*reqPhysList)
	{
		return *reqPhysList;
	}
#ifndef ANDROID
	CI_DEBUG("uiAllocated=0x%x\n", mem->uiAllocated);
#endif
	IMG_ASSERT(IMGMMU_GetPageSize() == PAGE_SIZE);

	physList = (IMG_UINT64 *)IMG_BIGALLOC((noPages) * sizeof(IMG_UINT64));

	IMG_ASSERT(physList);

	sgTable = ion_sg_table(mem->pAlloc->ionClient, mem->pAlloc->ionHandle);
	IMG_ASSERT(sgTable);

	scatterList = sgTable->sgl;

	while (scatterList) {
		IMG_UINT32 offset;
		dma_addr_t chunkBase;

		// Get physical addresses from scatter list
		chunkBase =
			addrType == DEVICE_PHYS_ADDRESS
			? getDevMemPhysAddress(sg_phys(scatterList))
			: sg_phys(scatterList);
#ifndef ANDROID
		CI_DEBUG("scatterList->length=%d\n", scatterList->length);
#endif
		for (offset = 0; offset < scatterList->length;
				offset += PAGE_SIZE, pageIdx++) {

			physList[pageIdx] = chunkBase + offset;
#ifndef ANDROID
			CI_DEBUG("physList[%d]=%llx\n", pageIdx, physList[pageIdx]);
#endif
		}
		scatterList = sg_next(scatterList);
	}

	*reqPhysList = physList;
	return physList;
}

/* Returns list of physical pages seen by device */
IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *mem)
{
    return memGetPhysPages(mem, DEVICE_PHYS_ADDRESS);
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem, struct vm_area_struct *vma)
{
	/* Get list of physical pages seen by cpu */
	IMG_UINT64 *physPages = memGetPhysPages(psMem, CPU_PHYS_ADDRESS);

    IMG_UINT32 noPages = (vma->vm_end - vma->vm_start) / PAGE_SIZE;
    IMG_UINT32 pageIdx = 0;
    IMG_UINT32 start = vma->vm_start;
    IMG_UINT64 pgoff = 0;

    vma->vm_pgoff = physPages[0] >> PAGE_SHIFT; // convert to a page offset
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot); // insure it is not cached

    IMG_ASSERT(physPages);
    IMG_ASSERT(!((vma->vm_end - vma->vm_start) % PAGE_SIZE));

    for (pageIdx = 0; pageIdx < noPages; pageIdx++)
    {
        pgoff = physPages[pageIdx] >> PAGE_SHIFT; // convert to a page offset

        if (remap_pfn_range(vma, start, pgoff, PAGE_SIZE, vma->vm_page_prot))
        {
            return IMG_ERROR_INTERRUPTED;
        }
        start += PAGE_SIZE;
    }

    vma->vm_ops = NULL; /// @ add remap function and open and close to support forking
    //simple_vma_open(vma); // need that in case of forks

    return IMG_SUCCESS;
}

/*
 * PDP is a specific part of the FPGA - may not apply to all devices
 */

#include "ci_internal/ci_pdp.h"

IMG_RESULT Platform_MemWritePDPAddress(const SYS_MEM *pToWrite, IMG_SIZE uiToWriteOffset)
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
			(void*)(g_psCIDriver->pDevice->uiBoardCPUVirtual + BAR0_PDP_OFFSET + BAR0_PDP_ADDRESS));

	return IMG_SUCCESS;
}
