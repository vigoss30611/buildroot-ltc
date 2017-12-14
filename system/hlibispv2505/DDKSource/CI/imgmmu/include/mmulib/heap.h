/**
*******************************************************************************
 @file heap.h

 @brief MMU Library: device virtual allocation (heap)

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
#ifndef IMGMMU_HEAP_H
#define IMGMMU_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <img_types.h>

/**
 * @defgroup IMGMMU_heap MMU Heap Interface
 * @brief The API for the device virtual address Heap - must be implemented
 * (see tal_heap.c for an example implementation)
 * @ingroup IMGMMU_lib
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMGMMU_heap documentation module
 *---------------------------------------------------------------------------*/

/** @brief An allocation on a heap. */
typedef struct MMUHeapAlloc
{
    /** @brief Start of the allocation */
    IMG_UINTPTR uiVirtualAddress;
    /** @brief Size in bytes */
    IMG_SIZE uiAllocSize;
} IMGMMU_HeapAlloc;

/**
 * @brief A virtual address heap - not directly related to HW MMU directory
 * entry
 */
typedef struct MMUHeap
{
    /** @brief Start of device virtual address */
    IMG_UINTPTR uiVirtAddrStart;
    /** @brief Allocation atom in bytes */
    IMG_SIZE uiAllocAtom;
    /** @brief Total size of the heap in bytes */
    IMG_SIZE uiSize;
} IMGMMU_Heap;

/**
 * @name Device virtual address allocation (heap management)
 * @{
 */

/**
 * @brief Create a Heap
 *
 * @param uiVirtAddrStart start of the heap - must be a multiple of uiAllocAtom
 * @param uiAllocAtom the minimum possible allocation on the heap in bytes 
 * - usually related to the system page size
 * @param uiSize total size of the heap in bytes
 * @param pResult must be non-NULL - used to give detail about error
 *
 * @return pointer to the new Heap object and pResult is IMG_SUCCESS
 * @return NULL and the value of pResult can be:
 * @li IMG_ERROR_MALLOC_FAILED if internal allocation failed
 */
IMGMMU_Heap* IMGMMU_HeapCreate(IMG_UINTPTR uiVirtAddrStart,
    IMG_SIZE uiAllocAtom, IMG_SIZE uiSize, IMG_RESULT *pResult);

/**
 * @brief Allocate from a heap
 *
 * @warning Heap do not relate to each other, therefore one must insure that
 * they should not overlap if they should not.
 *
 * @param pHeap must not be NULL
 * @param uiSize allocation size in bytes
 * @param pResult must be non-NULL - used to give details about error
 *
 * @return pointer to the new HeapAlloc object and pResult is IMG_SUCCESS
 * @return NULL and the value of pResult can be:
 * @li IMG_ERROR_INVALID_PARAMETERS if the give size is not a multiple of 
 * pHeap->uiAllocAtom
 * @li IMG_ERROR_MALLOC_FAILED if the internal structure allocation failed
 * @li IMG_ERROR_NOT_SUPPORTED if the internal device memory allocator did not
 * find a suitable virtual address
 */
IMGMMU_HeapAlloc* IMGMMU_HeapAllocate(IMGMMU_Heap *pHeap, IMG_SIZE uiSize,
    IMG_RESULT *pResult);

/**
 * @brief Liberate an allocation
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT IMGMMU_HeapFree(IMGMMU_HeapAlloc *pAlloc);

/**
 * @brief Destroy a heap object
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the given Heap still has attached
 * allocation
 */
IMG_RESULT IMGMMU_HeapDestroy(IMGMMU_Heap *pHeap);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the public functions
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the IMGMMU_heap documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* IMGMMU_HEAP_H */
