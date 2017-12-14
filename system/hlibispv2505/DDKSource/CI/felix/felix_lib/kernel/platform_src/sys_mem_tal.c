/**
 ******************************************************************************
 @file sys_mem_tal.c

 @brief Allocator implementation using TAL MEM and TAL alloc

 @note Can only work if using the fake driver (otherwise TALMEM does not
 allocate!)

 It is better to use carveout or pagealloc as references for implementations
 as this deals with pdump generation

 @warning TAL alloc supports setting the CPU page size when faking insmod!

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
#include <talalloc/talalloc.h>
#include <mmulib/mmu.h>
#include <mmulib/heap.h>
#include <felix_hw_info.h> // SYSMEM_ALIGNMENT

#include <img_defs.h>
#include <img_errors.h>

#ifndef FELIX_FAKE
#error This should be used only with simulator driver
#else
#include <sys/sys_userio_fake.h> // struct vm_area_struct and SYS_MEM_ALLOC
#endif

// extra as an TAL memory handle
struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE uiRealAlloc, void *extra,
    IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *psMem = NULL;
    IMG_ASSERT(ret != NULL);

    psMem = (struct SYS_MEM_ALLOC*)IMG_CALLOC(1, sizeof(struct SYS_MEM_ALLOC));
    if ( psMem == NULL )
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    // allocate physical address
    psMem->pMMUAlloc = TALALLOC_Malloc(extra, uiRealAlloc, ret);
    if ( psMem->pMMUAlloc == NULL )
    {
        IMG_FREE(psMem);
        psMem = NULL;
    }

    return psMem;
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE uiRealAlloc,
    void *extra, IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *psMem = NULL;
    IMG_ASSERT(ret != NULL);

    psMem = (struct SYS_MEM_ALLOC*)IMG_CALLOC(1, sizeof(struct SYS_MEM_ALLOC));
    if ( psMem == NULL )
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    psMem->pCPUMem = IMG_MALLOC(uiRealAlloc);
    if ( psMem->pCPUMem == NULL )
    {
        IMG_FREE(psMem);
        psMem = NULL;
        *ret = IMG_ERROR_MALLOC_FAILED;
        return NULL;
    }

    *ret = TALMEM_Malloc(extra, (IMG_CHAR*)psMem->pCPUMem, uiRealAlloc,
        SYSMEM_ALIGNMENT, &(psMem->pTalHandle), IMG_TRUE, NULL);
    if ( *ret != IMG_SUCCESS )
    {
        IMG_FREE(psMem->pCPUMem);
        IMG_FREE(psMem);
        psMem = NULL;
    }
    return psMem;
}

void Platform_MemFree(struct SYS_MEM_ALLOC *psMem)
{
    IMG_ASSERT(psMem != NULL);

    if ( psMem->pMMUAlloc != NULL )
    {
        TALALLOC_Free(psMem->pMMUAlloc);
    }
    else
    {
        TALMEM_Free(&(psMem->pTalHandle));
        IMG_FREE(psMem->pCPUMem);
    }
    IMG_FREE(psMem);
}

void* Platform_MemGetMemory(SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    if ( psMem->pAlloc->pMMUAlloc != NULL )
    {
        return psMem->pAlloc->pMMUAlloc->pHostLinear;
    }
    return psMem->pAlloc->pCPUMem;
}

IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem)
{
    IMG_RESULT ret;
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    if ( psMem->pAlloc->pMMUAlloc != NULL )
    {
        ret = TALALLOC_UpdateHost((IMGMMU_TALAlloc*)psMem->pAlloc->pMMUAlloc);
    }
    else
    {
        ret = TALMEM_UpdateHost(psMem->pAlloc->pTalHandle);
    }
    return ret;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem)
{
    IMG_RESULT ret;
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    if ( psMem->pAlloc->pMMUAlloc != NULL )
    {
        ret = TALALLOC_UpdateDevice(psMem->pAlloc->pMMUAlloc);
    }
    else
    {
        ret = TALMEM_UpdateDevice(psMem->pAlloc->pTalHandle);
    }
    return ret;
}

IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    if ( psMem->pAlloc->pMMUAlloc != NULL )
    {
        return (IMG_UINT64 *)TALALLOC_GetPhysMem(psMem->pAlloc->pMMUAlloc);
    }
    return NULL;
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE uiOffset)
{
    IMG_UINTPTR ret = 0;
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);

    if (psMem->pAlloc->pMMUAlloc != NULL) // using MMU
    {
        IMG_HANDLE pCurrMem = NULL;
        IMG_SIZE uiOff = uiOffset;

        pCurrMem = TALALLOC_ComputeOffset(psMem->pAlloc->pMMUAlloc, &uiOff);
        if ( pCurrMem == NULL )
        {
            return IMG_ERROR_FATAL;
        }

        ret = (IMG_UINTPTR)TALDEBUG_GetDevMemAddress(pCurrMem);
    }
    else
    {
        ret = (IMG_UINTPTR)TALDEBUG_GetDevMemAddress(
            psMem->pAlloc->pTalHandle);
    }
    return ret;
}

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
    *ret = IMG_ERROR_NOT_SUPPORTED;
    return NULL;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC * mem)
{
    (void)mem;//unused
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem,
    struct vm_area_struct *vma)
{
    vma->vm_private_data = Platform_MemGetMemory(psMem);
    return IMG_SUCCESS;
}

IMG_HANDLE Platform_MemGetTalHandle(const SYS_MEM *psMem, IMG_SIZE uiOffset,
    IMG_SIZE *pOff2)
{
    IMG_ASSERT(pOff2 != NULL);
    IMG_ASSERT(psMem != NULL);

    *pOff2 = uiOffset;
    if ( psMem->pAlloc->pMMUAlloc != NULL )
    {
        IMG_HANDLE pCurrMem = NULL;

        pCurrMem = TALALLOC_ComputeOffset(psMem->pAlloc->pMMUAlloc, pOff2);
        return pCurrMem;
    }
    //else
    return psMem->pAlloc->pTalHandle;
}

//
// MMU functions
//

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
    IMG_ASSERT(IMG_FALSE); // should not be used
    return NULL;
}

void Platform_MMU_MemPageFree(struct MMUPage *pPub)
{
    IMG_ASSERT(IMG_FALSE); // should not be used - TAL alloc is used
}

void Platform_MMU_MemPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags)
{
    IMG_ASSERT(IMG_FALSE); // should not be used - TAL alloc is used
}

IMG_UINT32 Platform_MMU_MemPageRead(struct MMUPage *pReadFrom,
    unsigned int offset)
{
    IMG_ASSERT(IMG_FALSE); // should not be used - TAL alloc is used
    return 0;
}

#ifdef CI_PDP

#include "ci_internal/ci_pdp.h"

#include "ci_kernel/ci_kernel.h"

/**
 * this is a fake implementation to have a pdp address written in pdump
 */
IMG_RESULT Platform_MemWritePDPAddress(const SYS_MEM *pToWrite,
    IMG_SIZE uiToWriteOffset)
{
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;
    int internalVar = 1;
    IMG_HANDLE sBar0Handle = TAL_GetMemSpaceHandle(BAR0_BANK);

    IMG_ASSERT(pToWrite != NULL);
    IMG_ASSERT(pToWrite->pAlloc != NULL);

    if ( uiToWriteOffset >= pToWrite->uiAllocated )
    {
        /*CI_FATAL("trying to write offset %" IMG_SIZEPR "u from a "\
            "structure of size %" IMG_SIZEPR "u\n",
            uiToWriteOffset, pToWrite->uiAllocated);*/
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if(pToWrite->pDevVirtAddr != NULL)
    {
        /* Do nothinng with MMU for now!
         * - later should get the physical address */
    }
    else
    {
        /*
         * because we generate pdumps that need to be replayable no matter
         * the address chosen for the block we have to use internal variables
         * to shift the address and write the enable
         */

        TALINTVAR_WriteMemRef(pToWrite->pAlloc->pTalHandle, uiToWriteOffset,
            sBar0Handle, internalVar);
        // does a right shift of the address
        TALINTVAR_RunCommand(TAL_PDUMPL_INTREG_SHR, sBar0Handle, internalVar,
            sBar0Handle, internalVar, sBar0Handle, 4, IMG_FALSE);
        TALINTVAR_RunCommand(TAL_PDUMPL_INTREG_OR, sBar0Handle, internalVar,
            sBar0Handle, internalVar, sBar0Handle, BAR0_ENABLE_PDP_BIT,
            IMG_FALSE); // set enable bit
        TALINTVAR_WriteToReg32(sBar0Handle, BAR0_PDP_ADDRESS, sBar0Handle,
            internalVar);
    }

    return ret;
}

#endif // CI_PDP
