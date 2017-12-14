/*!
 ******************************************************************************
 @file sys_mem.c

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
#include <tal.h>
#include <mmulib/mmu.h>
#include <mmulib/heap.h>

#include "ci_internal/sys_mem.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_debug.h" // debugfs access

/*
   IMG_RESULT IMG_CI_MemstructCreate(SYS_MEM **ppMemstruct)
   {
   if ( ppMemstruct == NULL )
   {
   CI_FATAL("ppMemstruct is NULL\n");
   return IMG_ERROR_INVALID_PARAMETERS;
   }

 *ppMemstruct = (SYS_MEM*)IMG_CALLOC(1, sizeof(SYS_MEM));

 if ( *ppMemstruct == NULL )
 {
 CI_FATAL("allocation of a memory structure failed (%uB)\n", sizeof(SYS_MEM));
 return IMG_ERROR_MALLOC_FAILED;
 }

 (*ppMemstruct)->hMem = NULL;
 (*ppMemstruct)->pvCpuLin = NULL;

 return IMG_SUCCESS;
 }

 IMG_RESULT IMG_CI_MemstructDestroy(SYS_MEM *pMemstruct)
 {
 if ( pMemstruct == NULL )
 {
 CI_FATAL("pMemstruct is NULL\n");
 return IMG_ERROR_INVALID_PARAMETERS;
 }

 if ( pMemstruct->hMem != NULL || pMemstruct->pvCpuLin != NULL )
 {
 CI_FATAL("memory structures handles are still allocated\n");
 return IMG_ERROR_MEMORY_IN_USE;
 }

 IMG_FREE(pMemstruct);

 return IMG_SUCCESS;
 }
 */
#ifdef INFOTM_ISP
IMG_RESULT SYS_MemAlloc(SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment, char* pszName)
#else
IMG_RESULT SYS_MemAlloc(SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment)
#endif //INFOTM_ISP
{
    IMG_SIZE uiRealAlloc;
    IMG_RESULT ret;
#ifdef FELIX_FAKE
    char message[128];
#endif

    IMG_ASSERT( psMem != NULL );
    IMG_ASSERT( g_psCIDriver != NULL );

    if ( uiSize <= 0 )
    {
        CI_FATAL("uiSize is <= 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( psMem->pAlloc != NULL )
    {
        CI_FATAL("handles are different than NULL\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    /* force the rounding up to the SYSMEM_ALIGNMENT
     * uiRealAlloc = (uiSize+SYSMEM_ALIGNMENT-1)/SYSMEM_ALIGNMENT
     * *SYSMEM_ALIGNMENT;
     * use rounding to page size because of the mapping to
     * user-space (that is done via page offsets) */
    uiRealAlloc = (uiSize+PAGE_SIZE-1)/PAGE_SIZE *PAGE_SIZE;
    IMG_ASSERT(uiRealAlloc%SYSMEM_ALIGNMENT==0);
    IMG_ASSERT(uiRealAlloc>=uiSize);

    if(pHeap != NULL) // using MMU
    {
        // allocate device virtual address from the heap
        psMem->ui32VirtualAlignment = ui32VirtualAlignment;
        psMem->pDevVirtAddr = IMGMMU_HeapAllocate(pHeap,
            uiRealAlloc+ui32VirtualAlignment, &ret);
        if ( psMem->pDevVirtAddr == NULL )
        {
            CI_FATAL("Failed to allocate device virtual address "\
                "(returned %d)\n", ret);
            return ret;
        }

#ifdef FELIX_FAKE
        sprintf(message, "Virt addr=0x%"IMG_PTRDPR"x 0x%"IMG_SIZEPR"x B "\
            "(+align 0x%x B) = 0x%"IMG_SIZEPR"x",
            psMem->pDevVirtAddr->uiVirtualAddress, uiRealAlloc,
            ui32VirtualAlignment, uiRealAlloc+ui32VirtualAlignment);
        TALPDUMP_Comment(g_psCIDriver->sTalHandles.hMemHandle, message);
        TALPDUMP_Comment(g_psCIDriver->sTalHandles.hMemHandle,
            "allocating physical pages");
#else
        CI_DEBUG("Virt addr=0x%"IMG_PTRDPR"x 0x%"IMG_SIZEPR"x B "\
            "(+align 0x%x B) = 0x%"IMG_SIZEPR"x",
            psMem->pDevVirtAddr->uiVirtualAddress, uiRealAlloc,
            ui32VirtualAlignment, uiRealAlloc+ui32VirtualAlignment);
#endif

        psMem->pAlloc = Platform_MemAlloc(uiRealAlloc,
            g_psCIDriver->sTalHandles.hMemHandle, &ret);
        // the memory is not mapped yet

        if ( ret != IMG_SUCCESS )
        {
            IMGMMU_HeapFree(psMem->pDevVirtAddr);
        }
        psMem->bWasImported = IMG_FALSE;
    }
    else
    {
        psMem->pAlloc = Platform_MemAllocContiguous(uiRealAlloc,
            g_psCIDriver->sTalHandles.hMemHandle, &ret);

    }

#ifdef CI_MEMSET_ALLOC
    CI_DEBUG("Memset allocation to 0x%x\n", CI_MEMSET_ALLOC);
    IMG_MEMSET(Platform_MemGetMemory(psMem), CI_MEMSET_ALLOC, uiRealAlloc);
    Platform_MemUpdateDevice(psMem);
#endif

#ifdef CI_MEMSET_ALIGNMENT
    if ( ret == IMG_SUCCESS && uiSize < uiRealAlloc )
    {
        char *hostMem = (char*)Platform_MemGetMemory(psMem);

#ifdef FELIX_FAKE
        sprintf(message, "size=%"IMG_SIZEPR"uB, realAlloc=%"IMG_SIZEPR"uB, "\
            "additional memory memset to 0x%x",
            uiSize, uiRealAlloc, CI_MEMSET_ALIGNMENT);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
        CI_DEBUG("size=%"IMG_SIZEPR"uB, realAlloc=%"IMG_SIZEPR"uB, "\
            "additional memory memset to 0x%x",
            uiSize, uiRealAlloc, CI_MEMSET_ALIGNMENT);
#endif

        IMG_MEMSET((hostMem)+uiSize, CI_MEMSET_ALIGNMENT, uiRealAlloc-uiSize);
    }
#ifdef FELIX_FAKE
    else
    {
        sprintf(message, "no additional memory to memset to 0x%x",
            CI_MEMSET_ALIGNMENT);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
    }
#endif
#endif

    if ( ret == IMG_SUCCESS )
    {
#ifdef FELIX_FAKE
        /* need to update device to ensure memory is the same when
         * re-running pdump */
        Platform_MemUpdateDevice(psMem);
        TALPDUMP_BarrierWithId(CI_SYNC_MEM_ALLOC);
#endif
        psMem->uiAllocated = uiRealAlloc;
#ifdef CI_DEBUGFS
        mutex_lock(&g_DebugFSDevMemMutex);
        g_ui32DevMemUsed += uiRealAlloc;
        g_ui32DevMemMaxUsed = IMG_MAX_INT(g_ui32DevMemUsed,
            g_ui32DevMemMaxUsed);
        mutex_unlock(&g_DebugFSDevMemMutex);
#endif // CI_DEBUGFS
#ifdef INFOTM_ISP
        IMG_STRNCPY(psMem->szMemName, pszName, 19);
        psMem->pVirtualAddress = Platform_MemGetMemory(psMem);
        psMem->pPhysAddress = (void*)Platform_MemGetDevMem(psMem, 0);
#endif
    }

#ifdef INFOTM_ISP
#ifdef INFOTM_ENABLE_ISP_DEBUG
    if(pHeap != NULL) // using MMU
    {
        printk("@@@ SYS_MemAlloc: name=%s, VirtAddr=0x%p, PhysAddr=0x%p, size=%d, RealSize=%d, pDevVirtAddr=0x%p, (0x%p, 0x%zx)\n",
                        psMem->szMemName,
                        psMem->pVirtualAddress,
                        psMem->pPhysAddress,
                        uiSize, uiRealAlloc,
                        psMem->pDevVirtAddr,
                        (void*)psMem->pDevVirtAddr->uiVirtualAddress,
                        psMem->pDevVirtAddr->uiAllocSize);
    }
    else
    {
        printk("@@@ SYS_MemAlloc: name=%s, VirtAddr=0x%p, PhysAddr=0x%p, size=%d, RealSize=%d\n",
                        psMem->szMemName,
                        psMem->pVirtualAddress,
                        psMem->pPhysAddress,
                        uiSize, uiRealAlloc);
    }
#endif //INFOTM_ENABLE_ISP_DEBUG
#endif //INFOTM_ISP
    return ret;
}

IMG_RESULT SYS_MemFree(SYS_MEM *psMem)
{
    IMG_ASSERT( psMem != NULL );

#ifdef FELIX_FAKE
    /* when using multiple pdump this is used to insure that the all
     * scripts are in sync */
    TALPDUMP_BarrierWithId(CI_SYNC_MEM_FREE);
#endif
    psMem->ui32VirtualAlignment = 0;

    if(psMem->pDevVirtAddr != NULL) // using MMU
    {
        if ( psMem->pMapping != NULL )
        {
#ifdef INFOTM_ISP
			CI_FATAL("%s, Memory is still mapped - cannot be freed\n", psMem->szMemName);
#else
            CI_FATAL("Memory is still mapped - cannot be freed\n");
#endif //INFOTM_ISP
            return IMG_ERROR_UNEXPECTED_STATE;
        }

        IMGMMU_HeapFree(psMem->pDevVirtAddr);
        psMem->pDevVirtAddr = NULL;
    }
    if ( psMem->pAlloc != NULL )
    {
        if ( psMem->bWasImported == IMG_TRUE )
        {
            Platform_MemRelease(psMem->pAlloc);
        }
        else
        {
            Platform_MemFree(psMem->pAlloc);
        }
    }
#ifdef CI_DEBUGFS
    mutex_lock(&g_DebugFSDevMemMutex);
    g_ui32DevMemUsed -= psMem->uiAllocated;
    mutex_unlock(&g_DebugFSDevMemMutex);
#endif // CI_DEBUGFS

    // set everything to 0 or NULL so that it can be reused
    IMG_MEMSET(psMem, 0, sizeof(SYS_MEM));

    return IMG_SUCCESS;
}

IMG_UINT32 SYS_MemGetFirstAddress(const SYS_MEM *psMem)
{
    IMG_UINT32 address = 0;

    IMG_ASSERT(psMem != NULL );
    IMG_ASSERT(psMem->pAlloc != NULL);

    if(psMem->pDevVirtAddr != NULL)
    {
        address = psMem->pDevVirtAddr->uiVirtualAddress;
        if ( psMem->ui32VirtualAlignment )
        {
            /* move the virtual address a bit further to be a multiple of
             * psMem->ui32VirtualAlignment */
            IMG_UINT32 uiOff = address%(psMem->ui32VirtualAlignment);
            if ( uiOff )
            {
                address += psMem->ui32VirtualAlignment - uiOff;
            }
        }
    }
    else
    {
        address = (IMG_UINT32)Platform_MemGetDevMem(psMem, 0);
    }

    return address;
}

IMG_RESULT SYS_MemMap(SYS_MEM *psMem, struct MMUDirectory *pDir,
    unsigned int flag)
{
    IMG_ASSERT(psMem != NULL);

    if ( psMem->pDevVirtAddr != NULL )
    {
        IMG_RESULT ret;
        IMG_UINT64 *aPageList = NULL;
        struct MMUHeapAlloc devVirtAddr;
#ifdef FELIX_FAKE
        char message[64];

        sprintf(message, "mapping virt=0x%"IMG_PTRDPR"x "\
            "size=0x%"IMG_SIZEPR"x B to dir 0x%p",
            psMem->pDevVirtAddr->uiVirtualAddress,
            psMem->pDevVirtAddr->uiAllocSize, pDir);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#endif

        IMG_ASSERT(pDir != NULL);

        if ( psMem->pMapping != NULL )
        {
            CI_FATAL("Memory already mapped\n");
            return IMG_ERROR_UNEXPECTED_STATE;
        }

        aPageList = Platform_MemGetPhysPages(psMem);
        if (!aPageList)
        {
            CI_FATAL("Failed to get list of physical pages\n");
            return IMG_ERROR_FATAL;
        }

        // to align to system alignment
        devVirtAddr.uiVirtualAddress = SYS_MemGetFirstAddress(psMem);
        devVirtAddr.uiAllocSize = psMem->uiAllocated;

        psMem->pMapping = IMGMMU_DirectoryMap(pDir, aPageList,
            &devVirtAddr, flag, &ret);
        if ( psMem->pMapping == NULL )
        {
#ifdef INFOTM_ISP
			CI_FATAL("%s, Failed to map memory\n", psMem->szMemName);
#else
            CI_FATAL("Failed to map memory\n");
#endif //INFOTM_ISP
            return ret;
        }
//#ifdef INFOTM_ISP
//		#ifdef INFOTM_ENABLE_ISP_DEBUG
//		printk("%s, pMapping=%p\n", psMem->szMemName, psMem->pMapping);
//		#endif //INFOTM_ENABLE_ISP_DEBUG
//#endif //INFOTM_ISP
#ifdef FELIX_FAKE
        /* when using multiple pdump this is used to insure that the all
         * scripts are in sync */
        TALPDUMP_BarrierWithId(CI_SYNC_MEM_MAP);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#endif
    }
    /* could be else from above but it's easier to read that way with
     * the FELIX_FAKE macro... */
    else if ( psMem->uiAllocated > 0 && psMem->pDevVirtAddr == NULL )
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
            "no virtual address - mapping ignored");
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_MemUnmap(SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);

    if ( psMem->pDevVirtAddr != NULL )
    {
#ifdef FELIX_FAKE
        char message[64];
#endif
        if ( psMem->pMapping == NULL )
        {
#ifdef INFOTM_ISP
			CI_FATAL("%s, Memory not mapped\n", psMem->szMemName);
#else
            CI_FATAL("Memory not mapped\n");
#endif //INFOTM_ISP
            return IMG_ERROR_UNEXPECTED_STATE;
        }

#ifdef FELIX_FAKE
        /* when using multiple pdump this is used to insure that the all
         * scripts are in sync */
        TALPDUMP_BarrierWithId(CI_SYNC_MEM_UNMAP);

        sprintf(message, "unmapping virt=0x%"IMG_PTRDPR"x "\
            "size=0x%"IMG_SIZEPR"x B",
            psMem->pDevVirtAddr->uiVirtualAddress,
            psMem->pDevVirtAddr->uiAllocSize);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
#endif
//#ifdef INFOTM_ISP
//		#ifdef INFOTM_ENABLE_ISP_DEBUG
//		printk("%s, unmapping=%p\n", psMem->szMemName, psMem->pMapping);
//		#endif //INFOTM_ENABLE_ISP_DEBUG
//#endif //INFOTM_ISP
        IMGMMU_DirectoryUnMap(psMem->pMapping);
        psMem->pMapping = NULL;
    }
    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
IMG_RESULT SYS_MemImport(struct SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment, int fd, char* pszName)
#else
IMG_RESULT SYS_MemImport(struct SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment, int fd)
#endif //INFOTM_ISP
{
    IMG_RESULT ret;
    IMG_ASSERT(psMem != NULL);

    if (psMem->pAlloc != NULL)
    {
        CI_FATAL("handles are different than NULL\n");
        ret = IMG_ERROR_MEMORY_IN_USE;
        goto error_general;
    }
    else
    {
        IMG_ASSERT(!psMem->uiAllocated);
        psMem->pAlloc = Platform_MemImport(fd, uiSize, &ret);
    }

    if (ret !=IMG_SUCCESS)
    {
        CI_FATAL("Platform_MemImport failed\n");
        goto error_general;
    }
    psMem->uiAllocated = uiSize;
    psMem->bWasImported = IMG_TRUE;

    IMG_ASSERT(psMem->uiAllocated % SYSMEM_ALIGNMENT == 0);

    if (pHeap != NULL) // using MMU
    {
        // allocate device virtual address from the heap
        psMem->ui32VirtualAlignment = ui32VirtualAlignment;
        psMem->pDevVirtAddr = IMGMMU_HeapAllocate(pHeap,
            psMem->uiAllocated+ui32VirtualAlignment, &ret);
        if ( psMem->pDevVirtAddr == NULL )
        {
            CI_FATAL("Failed to allocate device virtual address "\
                "(returned %d)\n", ret);
            Platform_MemRelease(psMem->pAlloc);
            return ret;
        }
    }

#ifdef CI_DEBUGFS
    mutex_lock(&g_DebugFSDevMemMutex);
    g_ui32DevMemUsed += psMem->uiAllocated;
    g_ui32DevMemMaxUsed = IMG_MAX_INT(g_ui32DevMemUsed, g_ui32DevMemMaxUsed);
    mutex_unlock(&g_DebugFSDevMemMutex);
#endif // CI_DEBUGFS

error_general:
    return ret;
}

IMG_RESULT SYS_MemWriteWord(SYS_MEM *psMem, IMG_SIZE uiOff,
    IMG_UINT32 ui32Value)
{
    IMG_RESULT ret;
    IMG_HANDLE pCurrMem = NULL;
    IMG_SIZE uiOff2 = uiOff;

    IMG_ASSERT(psMem != NULL);

    if ( uiOff >= psMem->uiAllocated )
    {
        /*CI_FATAL("trying to access %" IMG_SIZEPR "u in a structure of "\
            "size %" IMG_SIZEPR "u\n", ui64Offset, psMem->uiAllocated);*/
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCurrMem = Platform_MemGetTalHandle(psMem, uiOff, &uiOff2);

    ret = TALMEM_WriteWord32(pCurrMem, uiOff2, ui32Value);

    return ret;
}

IMG_RESULT SYS_MemReadWord(SYS_MEM *psMem, IMG_SIZE uiOff,
    IMG_UINT32 *pui32Value)
{
    IMG_RESULT ret;
    IMG_HANDLE pCurrMem = NULL;
    IMG_SIZE uiOff2 = uiOff;

    IMG_ASSERT(psMem != NULL);

    if ( uiOff >= psMem->uiAllocated )
    {
        /*CI_FATAL("trying to access %" IMG_SIZEPR "u in a structure of "\
            "size %" IMG_SIZEPR "u\n", ui64Offset, psMem->uiAllocated);*/
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCurrMem = Platform_MemGetTalHandle(psMem, uiOff, &uiOff2);

    ret = TALMEM_ReadWord32(pCurrMem, uiOff2, pui32Value);

    return ret;
}

IMG_RESULT SYS_MemWriteAddress(SYS_MEM *psMem, IMG_SIZE uiOffset,
    const SYS_MEM *pToWrite, IMG_SIZE uiToWriteOffset)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_HANDLE pCurrMem = NULL;
    IMG_SIZE uiOff2 = uiOffset;

    IMG_ASSERT(psMem != NULL );
    IMG_ASSERT(pToWrite != NULL);

    if ( uiOffset >= psMem->uiAllocated )
    {
        CI_WARNING("trying to access %" IMG_SIZEPR "u in a structure of "\
            "size %" IMG_SIZEPR "u\n", uiOffset, psMem->uiAllocated);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if ( uiToWriteOffset >= pToWrite->uiAllocated )
    {
        CI_WARNING("trying to write offset %" IMG_SIZEPR "u from a "\
            "structure of size %" IMG_SIZEPR "u\n",
            uiToWriteOffset, pToWrite->uiAllocated);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCurrMem = Platform_MemGetTalHandle(psMem, uiOffset, &uiOff2);
    if ( pToWrite->pDevVirtAddr )
    {
        // to cope with virtual address alignment
        IMG_UINT32 uiFirstPage = SYS_MemGetFirstAddress(pToWrite);

        ret = TALMEM_WriteWord32(pCurrMem, uiOff2,
            uiFirstPage + uiToWriteOffset);
    }
    else
    {
#ifdef FELIX_FAKE
        IMG_SIZE uiToWriteOff2 = uiToWriteOffset;
        IMG_HANDLE pToWriteMem = Platform_MemGetTalHandle(pToWrite,
            uiToWriteOffset, &uiToWriteOff2);

        ret = TALMEM_WriteDevMemRef32(pCurrMem, uiOff2, pToWriteMem,
            uiToWriteOff2);
#else
        IMG_UINT32 toWriteMem = Platform_MemGetDevMem(pToWrite,
            uiToWriteOffset);

        IMG_ASSERT(g_psCIDriver);
        IMG_ASSERT(g_psCIDriver->pDevice);

        // remove the device offset - should be done in the Platform
        /*toWriteMem = toWriteMem - g_psCIDriver->pDevice->uiMemoryPhysical
         * + g_psCIDriver->pDevice->uiDevMemoryPhysical; */
        CI_DEBUG("transform 0x%x to 0x%x\n",
            (int)Platform_MemGetDevMem(pToWrite, uiToWriteOffset), toWriteMem);

        ret = TALMEM_WriteWord32(pCurrMem, uiOff2, toWriteMem);
#endif
    }

    return ret;
}

IMG_RESULT SYS_MemWriteAddressToReg(IMG_HANDLE pTalRegHandle,
    IMG_SIZE uiOffset, const SYS_MEM *pToWrite, IMG_SIZE uiToWriteOffset)
{
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;

    IMG_ASSERT(pTalRegHandle != NULL);
    IMG_ASSERT(pToWrite != NULL);

    if ( uiToWriteOffset >= pToWrite->uiAllocated )
    {
        /*CI_FATAL("trying to write offset %" IMG_SIZEPR "u from a structure "\
            "of size %" IMG_SIZEPR "u\n",
            uiToWriteOffset, pToWrite->uiAllocated);*/
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pToWrite->pDevVirtAddr )
    {
        // to cope with virtual address alignment
        IMG_UINT32 uiFirstPage = SYS_MemGetFirstAddress(pToWrite);

        ret = TALREG_WriteWord32(pTalRegHandle, uiOffset, uiFirstPage
            + uiToWriteOffset);
    }
    else
    {
#ifdef FELIX_FAKE
        IMG_SIZE uiOff2 = uiToWriteOffset;
        IMG_HANDLE pCurrMem = Platform_MemGetTalHandle(pToWrite,
            uiToWriteOffset, &uiOff2);

        ret = TALREG_WriteDevMemRef32(pTalRegHandle, uiOffset, pCurrMem,
            uiOff2);
#else
        IMG_UINT32 toWriteMem = Platform_MemGetDevMem(pToWrite,
            uiToWriteOffset);

        IMG_ASSERT(g_psCIDriver);
        IMG_ASSERT(g_psCIDriver->pDevice);

        // remove the device offset - should be done in the Platform
        /* toWriteMem = toWriteMem - g_psCIDriver->pDevice->uiMemoryPhysical
         * + g_psCIDriver->pDevice->uiDevMemoryPhysical;*/
        CI_DEBUG("transform 0x%x to 0x%x\n",
            (int)Platform_MemGetDevMem(pToWrite, uiToWriteOffset),
            toWriteMem);

        ret = TALREG_WriteWord32(pTalRegHandle, uiOffset, toWriteMem);
#endif
    }

    return ret;
}
