/**
*******************************************************************************
 @file talalloc.h

 @brief Allocation using TAL Memory interface - example of non Unified Memory
 Access allocator

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
#ifndef MMU_TALALLOC_H
#define MMU_TALALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mmulib/mmu.h>
#include <img_types.h>

struct MMUHeap;  // forward declaration: see mmulib/heap.h

/**
 * @defgroup IMGMMU_TALALLOC TAL Allocator
 * @brief The TAL implementation of the MMU allocator
 * @image html "TAL Allocator_class.png" "TAL Allocator structure organisation"
 *
 * @warning All memory operations on blocks allocated by TALALLOC should be
 * using TALALLOC_ComputeOffset() to get the correct TAL IMG_HANDLE/ offset
 * for that operation.
 *
 * @ingroup IMGMMU_lib
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMGMMU_TAL module
 *---------------------------------------------------------------------------*/

/**
 * @brief Stores information about an allocation done with TAL Memory interface
 *
 * @note The address of pPage->sMMUPage can be used as the first MMUPage of
 * the chain used for allocation when invocking IMGMMU_DirectoryMap().
 */
typedef struct MMU_TALAlloc
{
    /** @brief linear copy of all the sub-pages - this is "host" memory */
    void *pHostLinear;
    /** @brief allocation size in bytes */
    IMG_SIZE uiAllocSize;
    /** @brief number of pages associated with this allocation in aPhysAlloc */
    IMG_SIZE uiNCPUPages;
    /**
     * @brief associated TAL memory region with the tal device memory
     * (TAL_sMemSpaceCB*)
     */
    IMG_HANDLE sTalMemHandle;
    /**
     * @brief physical address for the CPU pages - this is just casted
     * IMG_HANDLE (TAL_psDeviceMemCB)
     */
    IMG_HANDLE *aPhysAlloc;
    /** @brief physical addresses to give when mapping using the MMU */
    IMG_UINT64 *aDevAddr;
} IMGMMU_TALAlloc;

/**
 * @name Allocator functions
 * @{
 */

/**
 * @brief Allocate some device memory using the TAL Memory interface
 *
 * Will allocate as many blocks of IMGMMU_PAGE_SIZE using TAL MEM as needed.
 * A "host" linear memory will also be allocated to allow direct modification
 * of the memory.
 *
 * @warning The allocated host linear memory is not memset to any value
 * (it is using IMG_MALLOC)
 *
 * It is a non Unified Memory Access system, therefore memory should be
 * updated:
 * @li using TALALLOC_UpdateHost() to update from device before read access
 * @li using TALALLOC_UpdateDevice() to update the device before write access
 *
 * @param sTalMemHandle TAL Memory Register bank - asserts that it is not NULL
 * @param uiSize in bytes to allocate (will be round up to match the number
 * of pages) - asserts that it is not NULL
 * @param pResult result output - asserts that it is not NULL
 *
 * @return The pointer to the IMGMMU_TALAlloc structure and pResult to
 * IMG_SUCCESS
 * @return NULL in case of error and pResult contains:
 * @li IMG_ERROR_INVALID_PARAMETERS if the allocation failed to round up the
 * size
 * @li IMG_ERROR_MALLOC_FAILED if the structure allocation failed (including
 * its components and the TAL host linear equivalent of the memory to
 * allocate)
 * @li IMG_ERROR_FATAL if TALMEM failed to allocate one of the needed page
 */
IMGMMU_TALAlloc* TALALLOC_Malloc(IMG_HANDLE sTalMemHandle, IMG_SIZE uiSize,
    IMG_RESULT *pResult);

/** @brief Liberate the allocated memory (host and pages) */
IMG_RESULT TALALLOC_Free(IMGMMU_TALAlloc *pAlloc);

/** @brief Returns the array of physical pages */
const IMG_UINT64* TALALLOC_GetPhysMem(IMGMMU_TALAlloc *pAlloc);

/** @brief Update the host linear memory from the device pages */
IMG_RESULT TALALLOC_UpdateHost(IMGMMU_TALAlloc *pAlloc);

/** @brief Update partial host memory */
IMG_RESULT TALALLOC_UpdateHostRegion(IMGMMU_TALAlloc *pAlloc,
    IMG_UINT64 offset, IMG_UINT64 size);

/** @brief Update the device pages from the host linear memory */
IMG_RESULT TALALLOC_UpdateDevice(IMGMMU_TALAlloc *pAlloc);

/** @brief Update partial host memory */
IMG_RESULT TALALLOC_UpdateDeviceRegion(IMGMMU_TALAlloc *pAlloc,
    IMG_UINT64 offset, IMG_UINT64 size);

/**
 * @brief Returns the IMG_HANDLE to use with TAL to read/write at this offset
 * of the allocation.
 *
 * The return handle/offset should be used with TALREG_ or TALMEM_ functions.
 *
 * For example to read a value from memory:
 * @code
 IMG_HANDLE pCurrMem = NULL;
 IMG_SIZE uiOff2 = uiOff;

 pCurrMem = TALALLOC_ComputeOffset(pMMUAlloc, &uiOff2);
 if (NULL == pCurrMem)
 {
    return IMG_ERROR_FATAL;
 }

 ret = TALMEM_ReadWord32(pCurrMem, uiOff2, pui32Value);
 @endcode
 *
 * @param pAlloc allocation to use - asserts it is not NULL
 * @param[in,out] uiOff the current offset value (and output the result
 * offset) - asserts it is not NULL
 *
 * @return the structure pointer or NULL on error
 */
IMG_HANDLE TALALLOC_ComputeOffset(IMGMMU_TALAlloc *pAlloc, IMG_SIZE *uiOff);

/**
 * @}
 * @name TAL Help functions
 * @brief Function that can be used when compiling the MMU lib with TAL
 * allocator
 * @{
 */

/**
 * @brief Get the used physical size of the MMU
 *
 * @see IMGMMU_TALSetMMUSize() to set the size to use
 */
IMG_UINT32 IMGMMU_TALGetMMUSize(void);

/**
 * @brief Set the physical size of the MMU to use (can be read from registers)
 *
 * @see IMGMMU_TALGetMMUSize() to get the currently used one
 */
void IMGMMU_TALSetMMUSize(IMG_UINT32 ui32Size);

/**
 * @brief This function can be used to set the memory region name
 * (from the TAL) associated with a heap
 *
 * @param pHeap the heap to alterate (asserts if NULL)
 * @param pszMemoryRegionName name of the memory region in the TAL
 */
void IMGMMU_TALHeapSetMemRegion(struct MMUHeap *pHeap,
    const char *pszMemoryRegionName);

/**
 * @brief Set TAL handle (TAL_sMemspaceCB*) to use from now on in
 * IMGMMU_TALPageAlloc()
 */
void IMGMMU_TALPageSetMemRegion(IMG_HANDLE pTalHandle);

/**
 * @brief Function to allocate a physical page using the TAL memory interface
 *
 * Give this function to the MMU in IMGMMU_Info::pfnPageAlloc
 *
 * Stores IMG_HANDLE (TAL_psDeviceMemCB) in uiPhysAddr
 *
 * @note TALMEM_Malloc() is called without the UpdateDevice according to
 * compilation time macro MMU_MALLOC_UPDATE_DEVICE (default to IMG_TRUE)
 *
 * @see Implements IMGMMU_pfnPageAlloc
 */
struct MMUPage* IMGMMU_TALPageAlloc(void);

/**
 * @brief Function to liberate a page
 *
 * Give this function to the MMU in IMGMMU_Info::pfnPageFree
 *
 * @see Implements IMGMMU_pfnPageFree
 */
void IMGMMU_TALPageFree(struct MMUPage *pPage);

/**
 * @brief Write a memory reference to a page offset
 *
 * Give this function to the MMU in IMGMMU_Info::pfnPageWrite
 *
 * @note The given physical address is casted to a IMG_HANDLE
 * (TAL_psDeviceMemCB) generated when allocating using TALALLOC_Malloc()
 * - if 0 then the casting is not done
 *
 * @see Implements IMGMMU_pfnPageWrite
 */
void IMGMMU_TALPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags);

/**
 * @brief Update device memory in non UMA
 *
 * Give this function to the MMU in IMGMMU_Info::pfnPageUpdateDevice
 *
 * @warning Do not use to have pdumps that are replayable if memory base
 * address changed
 */
void IMGMMU_TALPageUpdate(struct MMUPage *pPage);

/**
 * @brief Similar to IMGMMU_DirectoryGetPage() but get the TAL memory
 * handle directly
 *
 * @param pDirectory IMGMMU_DirectoryGetPage() asserts if pDirectory is NULL
 *
 * @return The TAL memory handle associated with the directory physical page
 */
IMG_HANDLE IMGMMU_TALDirectoryGetMemHandle(struct MMUDirectory *pDirectory);

/**
 * @}
 */

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the IMGMMU_TAL documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MMU_TALALLOC_H */
