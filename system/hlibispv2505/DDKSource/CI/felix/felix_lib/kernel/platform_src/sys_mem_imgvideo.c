/**
******************************************************************************
@file sys_mem_imgvideo.c

@brief Implementation of memory access with SYSMEM from imgvideo

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
#include <ci_kernel/ci_kernel.h>
#include <mmulib/mmu.h>
#include <mmulib/heap.h>
#include <img_defs.h>
#include <img_types.h>
#include <img_errors.h>
#include <sysdev_utils.h>
#include <sysmem_utils.h>
#include <target.h>

#include <linux/vmalloc.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>  // phys_to_dma

#define MEM_POOL(psMem) (((SYSMEMU_sPages*)psMem)->memHeap->memId)

typedef SYSMEMU_sPages SYS_MEM_ALLOC;

/*
 * used by sys_device_*
 */
IMG_PHYSADDR Platform_paddr_to_devpaddr(struct SYSDEVU_sInfo *sysdev,
    IMG_PHYSADDR paddr) {
#if defined(SYSMEM_UNIFIED_VMALLOC)
    return phys_to_dma(sysdev->native_device, paddr);
#else
    SYS_DEVICE *pSysDevice = NULL;
    IMG_UINT64 size;
    IMG_PHYSADDR membase, devpaddr = 0x0;

    IMG_ASSERT(sysdev);
    IMG_ASSERT(sysdev->pPrivate);
    pSysDevice = sysdev->pPrivate;

    membase = pSysDevice->uiMemoryPhysical;
    size = pSysDevice->uiMemorySize;

    if (paddr >= membase && paddr < membase + size) {
        devpaddr = paddr - membase;
    }
    else {
        IMG_ASSERT(IMG_FALSE);
    }

    /* Return the device physical address...*/
    return devpaddr;
#endif
}

/*
 *
 */

struct MMUPage_imgvideo {
    SYSMEMU_sPages *psPages;
    struct MMUPage sPage;
};

static IMG_RESULT init_imgvideo(void)
{
    IMG_RESULT ui32Result = IMG_SUCCESS;

    CI_DEBUG("empty init imgvideo\n");

    return ui32Result;
}

static IMG_RESULT getPagesAddr(IMG_VOID *psMem,
    void **pvCpuKmAddr, IMG_PHYSADDR *ppaAddr)
{
    IMG_RESULT ret;

    ret = SYSMEMU_GetCpuKmAddr(pvCpuKmAddr, (IMG_HANDLE)psMem);
    IMG_ASSERT(ret == IMG_SUCCESS);
    if (ret != IMG_SUCCESS)
        return ret;

    if (ppaAddr != NULL) {
        IMG_PHYSADDR cpuPAddr;
        cpuPAddr = SYSMEMU_CpuKmAddrToCpuPAddr(MEM_POOL(psMem),
            *pvCpuKmAddr);
        IMG_ASSERT(cpuPAddr > 0);

        /**ppaAddr = SYSDEVU_CpuPAddrToDevPAddr(pSysDev->sMemPool,
            cpuPAddr);*/

        // this calculates memory offset as seen by pci device
        *ppaAddr = (cpuPAddr - g_psCIDriver->pDevice->uiMemoryPhysical)
            + g_psCIDriver->pDevice->uiDevMemoryPhysical;
    }
    return ret;
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE size, void *extra,
    IMG_RESULT *ret)
{
    SYSDEVU_sInfo *pSysDev = (SYSDEVU_sInfo *)g_psCIDriver->pDevice->data;
    IMG_HANDLE phPagesHandle;

    init_imgvideo();

    *ret = SYSMEMU_AllocatePages(size, SYS_MEMATTRIB_UNCACHED,
        pSysDev->sMemPool, &phPagesHandle, NULL);
    return phPagesHandle;
}

struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE size, void *extra,
    IMG_RESULT *ret)
{
    SYSDEVU_sInfo *pSysDev = (SYSDEVU_sInfo *)g_psCIDriver->pDevice->data;
    IMG_HANDLE phPagesHandle;

    init_imgvideo();

    *ret = SYSMEMU_AllocatePages(size, SYS_MEMATTRIB_UNCACHED,
        pSysDev->sMemPool, &phPagesHandle, NULL);
    return phPagesHandle;
}

void Platform_MemFree(struct SYS_MEM_ALLOC *mem)
{
    IMG_HANDLE phPagesHandle = mem;  // this is a (SYSMEMU_sPages *)
    SYSMEMU_FreePages(phPagesHandle);
}

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
    SYSDEVU_sInfo *pSysDev = (SYSDEVU_sInfo *)g_psCIDriver->pDevice->data;
    struct MMUPage_imgvideo *mmuPageImg = NULL;
    IMG_RESULT ret;
    IMG_PHYSADDR cpuPhys = 0;

    init_imgvideo();

    mmuPageImg = (struct MMUPage_imgvideo *)IMG_CALLOC(1, sizeof(*mmuPageImg));

    if (!mmuPageImg) {
        CI_FATAL("IMG_CALLOC failed!\n");
        return NULL;
    }

    // should use MMU_PAGE_SIZE not PAGE_SIZE
    ret = SYSMEMU_AllocatePages(PAGE_SIZE, SYS_MEMATTRIB_UNCACHED,
        pSysDev->sMemPool, (IMG_HANDLE *)&mmuPageImg->psPages, NULL);
    IMG_ASSERT(ret == IMG_SUCCESS);
    if (ret != IMG_SUCCESS) {
        CI_FATAL("SYSMEMU_AllocatePages failed!\n");
        IMG_FREE(mmuPageImg);
        return NULL;
    }

    ret = getPagesAddr(mmuPageImg->psPages,
        (IMG_VOID **)&mmuPageImg->sPage.uiCpuVirtAddr,
        &cpuPhys);
    mmuPageImg->sPage.uiPhysAddr = cpuPhys;

    CI_DEBUG("mmu page allocated cpuV=0x%x devP=0x%x\n",
        mmuPageImg->sPage.uiCpuVirtAddr,
        mmuPageImg->sPage.uiPhysAddr);

    IMG_ASSERT(ret == IMG_SUCCESS);
    if (ret != IMG_SUCCESS) {
        CI_FATAL("getPagesAddr failed!\n");
        SYSMEMU_FreePages(mmuPageImg->psPages);
        IMG_FREE(mmuPageImg);
        return NULL;
    }

    return &mmuPageImg->sPage;
}

void Platform_MMU_MemPageFree(struct MMUPage *mmuPage)
{
    struct MMUPage_imgvideo *mmuPageImg = NULL;
    IMG_ASSERT(mmuPage);

    mmuPageImg = container_of(mmuPage, struct MMUPage_imgvideo, sPage);

    IMG_ASSERT(mmuPageImg->psPages);
    SYSMEMU_FreePages(mmuPageImg->psPages);
    IMG_FREE(mmuPageImg);
}

void Platform_MMU_MemPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags)
{
    IMG_UINT32 *pDirMemory = NULL;
    IMG_UINT64 uiCurrPhysAddr = uiToWrite;
    IMG_UINT32 extAddress = 0;

    struct MMUPage_imgvideo *mmuPageImg = NULL;

    IMG_ASSERT(pWriteTo != NULL);
    mmuPageImg = container_of(pWriteTo, struct MMUPage_imgvideo, sPage);

    pDirMemory = (IMG_UINT32*)pWriteTo->uiCpuVirtAddr;

    /*
     * Shifts to do to make the extended address fits into the entries of the
     * MMU pages.
     */
    extAddress = KRN_CI_MMUSize(&(g_psCIDriver->sMMU)) - 32;

    /*
     * Assumes that the MMU HW has the extra-bits enabled (this simple function
     * has no way of knowing).
     */
    if (extAddress > 0)
    {
        uiCurrPhysAddr >>= extAddress;
    }

    pDirMemory[offset] = (IMG_UINT32)uiCurrPhysAddr | (flags);
    if (flags != 0)  // otherwise polluted when initialising pages...
    {
        /*CI_DEBUG("(MMU) write 0x%x at 0x%llx[+0x%x]\n",
            (IMG_UINT32)uiCurrPhysAddr | (flags),
            (unsigned long long)mmuPageImg->sPage.uiPhysAddr, offset);*/
    }
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

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
    // allocationSize type?
    SYSDEVU_sInfo *pSysDev = (SYSDEVU_sInfo *)g_psCIDriver->pDevice->data;
    SYSMEMU_sPages *pPages = NULL;
    IMG_UINT64 *pui64PAddr;
    IMG_SIZE numPages;
    SYS_eMemPool eMemPool;

    init_imgvideo();

    numPages = (allocationSize + PAGE_SIZE - 1) / PAGE_SIZE;
    pui64PAddr = (IMG_UINT64 *)IMG_BIGALLOC(numPages * sizeof(*pui64PAddr));
    IMG_ASSERT(pui64PAddr);
    if (!pui64PAddr) {
        *ret = IMG_ERROR_FATAL;
        return NULL;
    }

#if defined(SYSMEM_DMABUF_IMPORT)
    /* In this configuration, buffers are always imported by dmabuf heap
     * which is temporarily stored in secureMemPool */
    eMemPool = pSysDev->secureMemPool;
#else
    eMemPool = pSysDev->sMemPool;
#endif

    pPages = SYSMEMU_ImportPages(eMemPool, pSysDev,
        numPages * PAGE_SIZE,
        SYS_MEMATTRIB_UNCACHED | SYS_MEMATTRIB_WRITECOMBINE,
        fd,
        pui64PAddr, /* pPhyAddrs */
        NULL, /* priv */
        IMG_TRUE); /* kernelMapped */

    //  change imgvideo to not need the page address table
    IMG_BIGFREE(pui64PAddr);
    if (pPages == NULL) {
        CI_FATAL("SYSMEMU_ImportPages failed\n");
        *ret = IMG_ERROR_FATAL;
        return NULL;
    }

    *ret = IMG_SUCCESS;
    return (struct SYS_MEM_ALLOC *)pPages;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC *psMem)
{
    /* nothing to do here */
}

IMG_HANDLE Platform_MemGetTalHandle(IMG_CONST SYS_MEM *psMem,
    IMG_SIZE uiOffset, IMG_SIZE *pOff2)
{
    IMG_HANDLE hMem;
    IMG_RESULT ret;

    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    IMG_ASSERT(pOff2);
    // CPU virtual address is contiguous
    *pOff2 = uiOffset;

    ret = getPagesAddr(psMem->pAlloc, &hMem, NULL);
    IMG_ASSERT(ret == IMG_SUCCESS);
    return hMem;
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE offset)
{
    //  Should check if it is continuous
    IMG_HANDLE hMem;
    IMG_PHYSADDR paMem;
    IMG_RESULT ret;

    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    ret = getPagesAddr(psMem->pAlloc, &hMem, &paMem);
    IMG_ASSERT(ret == IMG_SUCCESS);
    return paMem + offset;
}

IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem)
{
    SYSMEMU_UpdateMemory((SYSMEMU_sPages *)psMem->pAlloc, DEV_TO_CPU);
    return IMG_SUCCESS;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem)
{
    SYSMEMU_UpdateMemory((SYSMEMU_sPages *)psMem->pAlloc, CPU_TO_DEV);
    return IMG_SUCCESS;
}

void* Platform_MemGetMemory(SYS_MEM *psMem)
{
    IMG_HANDLE hMem;
    IMG_RESULT ret;

    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    ret = getPagesAddr(psMem->pAlloc, &hMem, NULL);
    IMG_ASSERT(ret == IMG_SUCCESS);
    return hMem;
}

IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem)
{
    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    return ((SYSMEMU_sPages *)psMem->pAlloc)->ppaPhysAddr;
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem,
struct vm_area_struct *vma)
{
    IMG_RESULT ret;
    IMG_PHYSADDR cpuPAddr = 0;
    void *cpuVirt;

    ret = SYSMEMU_GetCpuKmAddr(&cpuVirt, psMem->pAlloc);
    IMG_ASSERT(ret == IMG_SUCCESS);
    if (ret != IMG_SUCCESS)
    {
        return IMG_ERROR_FATAL;
    }

    cpuPAddr = SYSMEMU_CpuKmAddrToCpuPAddr(MEM_POOL(psMem->pAlloc), cpuVirt);

    vma->vm_pgoff = cpuPAddr >> PAGE_SHIFT;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    /*if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                vma->vm_end - vma->vm_start, vma->vm_page_prot))
                {
                return IMG_ERROR_INTERRUPTED;
                }*/

    // SYSMEMU_MapToUser assumes the vma structure is ready
    ret = SYSMEMU_MapToUser((SYSMEMU_sPages *)psMem->pAlloc, vma);
    IMG_ASSERT(ret == IMG_SUCCESS);
    return IMG_SUCCESS;
}

//
// PDP is a specific part of the FPGA - may not apply to all devices
//

#include "ci_internal/ci_pdp.h"

IMG_RESULT Platform_MemWritePDPAddress(const SYS_MEM *pToWrite,
    IMG_SIZE uiToWriteOffset)
{
    IMG_UINT32 uiVal = 0;

    IMG_ASSERT(pToWrite != NULL);
    IMG_ASSERT(pToWrite->pAlloc != NULL);

    if (uiToWriteOffset >= pToWrite->uiAllocated)
    {
        CI_FATAL("cannot write further than allocated\n");
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
        uiVal, BAR0_ENABLE_PDP_BIT, BAR0_PDP_OFFSET + BAR0_PDP_ADDRESS);
    iowrite32(uiVal >> 4 | BAR0_ENABLE_PDP_BIT,
        (void*)(g_psCIDriver->pDevice->uiBoardCPUVirtual
        + BAR0_PDP_OFFSET + BAR0_PDP_ADDRESS));

    return IMG_SUCCESS;
}
