/**
*******************************************************************************
 @file mmulib/mmu.h

 @brief MMU Library

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
#ifndef IMGMMU_MMU_H
#define IMGMMU_MMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <img_types.h>

/**
 * @defgroup IMGMMU_lib The MMU page table manager
 * @brief The Memory Mapping Unit page table manager library handles the
 * memory hierarchy for a multi-directory MMU.
 *
 * The library is composed of several elements:
 * @li the Page Table management, responsible for managing the device memory
 * used for the mapping. This requires some functions from the Allocator to
 * access the memory and a virtual address from a Heap.
 * @li the Heap interface, that is the SW heap API one can re-implement (or
 * use the provided TAL one). This is responsible for choosing a virtual
 * address for an allocation.
 * @li the Allocator, that is not implemented in this library, is responsible
 * for providing physical pages list (when mapping) and a few memory operation
 * functions (IMGMMU_Info).
 * An example TAL allocator is provided in this code and can be used when
 * running in full user-space with pdumps.
 *
 * Some pre-processor values can be defined to customise the MMU:
 * @li IMGMMU_PHYS_SIZE physical address size of the MMU in bits (default: 40)
 * - only used for the default memory write function
 * @li IMGMMU_PAGE_SIZE page size in bytes (default: 4096) - not used directly
 * in the MMU code, but the allocator should take it into account
 * If IMGMMU_PAGE_SIZE is defined the following HAVE TO be defined as well:
 * @li IMGMMU_PAGE_SHIFT as log2(IMGMMU_PAGE_SIZE) (default: 12) - used in
 * virtual address to determine the position of the page offset
 * @li IMGMMU_DIR_SHIFT as log2(IMGMMU_PAGE_SIZE)*2-2 (default: 22) - used in
 * virtual address to determine the position of the directory offset
 *
 * Uses IMG_KERNEL_MODULE to know if currently in kernel code or not when
 * printing errors and accessing PAGE_SIZE.
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMGMMU_lib documentation module
 *---------------------------------------------------------------------------*/

/**
 * @name MMU page table management
 * @brief The public functions to use to control the MMU.
 * @image html MMU_class.png "MMU structure organisation"
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the public functions
 *---------------------------------------------------------------------------*/

/** @brief Opaque type representing an MMU Directory page */
struct MMUDirectory;
/** @brief Opaque type representing an MMU Mapping */
struct MMUMapping;

struct MMUPage;
struct MMUHeapAlloc;

/**
 * @brief Pointer to a function implemented by the used allocator to create 1
 * page table (used for the MMU mapping - directory page and mapping page)
 *
 * This is not done internally to allow the usage of different allocators
 *
 * @return A populated MMUPage structure with the result of the page
 * allocation
 * @return NULL if the allocation failed
 *
 * @see IMGMMU_pfnPageFree to liberate the page
 */
typedef struct MMUPage* (*IMGMMU_pfnPageAlloc)(void);
/**
 * @brief Pointer to a function to free the allocated page table used for MMU
 * mapping
 *
 * This is not done internally to allow the usage of different allocators
 *
 * @see IMGMMU_pfnPageAlloc to allocate the page
 */
typedef void (*IMGMMU_pfnPageFree)(struct MMUPage*);

/**
 * @brief Pointer to a function to update Device memory on non Unified Memory
 */
typedef void (*IMGMMU_pfnPageUpdateDevice)(struct MMUPage*);

/**
 * @brief Write to a device address.
 *
 * This is not done internally to allow debug operations such a pdumping to
 * occur
 *
 * This function should do all the shifting and masking needed for the used
 * MMU
 *
 * @param pWriteTo page to update - asserts it is not NULL
 * @param offset in entries (32b word)
 * @param uiToWrite physical address to write
 * @param flags bottom part of the entry used as flags for the MMU (including
 * valid flag)
 */
typedef void (*IMGMMU_pfnPageWrite)(struct MMUPage *pWriteTo,
    unsigned int offset, IMG_UINT64 uiToWrite, unsigned int flags);

/**
 * @brief Reads a 32 word on a physical page
 *
 * This is used only when debug operations occures (e.g. access page table
 * entry that caused a page fault)
 *
 * @param pReadFrom physical page - asserts it is not NULL
 * @param offset in entries (32b word)
 */
typedef IMG_UINT32 (*IMGMMU_pfnPageRead)(struct MMUPage *pReadFrom,
    unsigned int offset);

// public struct
/**
 * @note Description of the virtual address (examples are for the typical
 * 4kBytes page):
 * @li the size is given by ui32VirtAddrSize (e.g. 32)
 * @li the MSB are the directory index, they are between ui32DirShift and
 * ui32VirtAddrSize (e.g. dir shift = 22, bits 22 to 31 is directory index)
 * @li the "middle bits" are the page table index, they are between 
 * ui32PageShift and ui32DirShift (e.g. page table index = 12, bits 12 to 21
 * is the page table index)
 * @li the LSB are the offset in the page (bits 0 to 11)
 */
typedef struct MMUInfo
{
    /** @brief allocate a physical page used in MMU mapping */
    IMGMMU_pfnPageAlloc pfnPageAlloc;
    /** @brief liberate a physical page used in MMU mapping */
    IMGMMU_pfnPageFree pfnPageFree;
    /**
     * @brief write a physical address onto a page - optional, if NULL
     * internal function is used 
     *
     * The internal function assumes that IMGMMU_PHYS_SIZE is the MMU size.
     *
     * @note if NULL pfnPageRead is also set
     *
     * @warning The function assumes that the physical page memory is
     * accessible
     */
    IMGMMU_pfnPageWrite pfnPageWrite;
    /**
     * @brief read a physical page offset - optional, if NULL access to page
     * table and directory entries is not supported
     *
     * @note If pfnPageWrite and pfnPageRead are NULL then the internal
     * function is used.
     *
     * @warning The function assumes that the physical page memory is
     * accessible
     */
    IMGMMU_pfnPageRead pfnPageRead;
    /**
     * @brief update a physical page on device if non UMA - optional, can be
     * NULL if update are not needed
     */
    IMGMMU_pfnPageUpdateDevice pfnPageUpdate;
} IMGMMU_Info;

/** @brief Page table entry - used when allocating the MMU pages */
typedef struct MMUPage
{
    /** 
     * @note Use ui64 instead of uintptr_t to support extended physical
     * address on 32b OS
     */
    IMG_UINT64 uiPhysAddr;
    IMG_UINTPTR uiCpuVirtAddr;
#ifdef INFOTM_ISP		
    void *priv;
#endif //INFOTM_ISP		
} IMGMMU_Page;

/**
 * @brief Access the compilation specified page size of the MMU (in Bytes)
 */
IMG_SIZE IMGMMU_GetPageSize(void);

/**
 * @brief Access the compilation specified physical size of the MMU (in bits)
 */
IMG_SIZE IMGMMU_GetPhysicalSize(void);

/**
 * @brief Access the compilation specified virtual address size of the MMU
 * (in bits)
 */
IMG_SIZE IMGMMU_GetVirtualSize(void);

/**
 * @brief Access the CPU page size - similar to PAGE_SIZE macro in Linux
 *
 * Not directly using PAGE_SIZE because we need a run-time configuration of
 * the PAGE_SIZE when running against simulators and different projects define
 * PAGE_SIZE in different ways...
 *
 * The default size is using the PAGE_SIZE macro if defined (or 4kB if not
 * defined when running against simulators)
 */
IMG_SIZE IMGMMU_GetCPUPageSize(void);

/**
 * @brief Change run-time CPU page size
 *
 * @warning to use against simulators only! default of IMGMMU_GetCPUPageSize()
 * is PAGE_SIZE otherwise!
 */
IMG_RESULT IMGMMU_SetCPUPageSize(IMG_SIZE pagesize);

/**
 * @brief Create a directory entry based on a given directory configuration
 *
 * @warning Obviously creation of the directory allocates memory - do not call
 * while interrupts are disabled
 *
 * @param pMMUFunctions is copied and not modified - contains the functions to
 * use to manage page table memory
 * @param pResult where to store the error code, should not be NULL (trapped
 * by assert)
 *
 * @return The opaque handle to the MMUDirectory object and pResult to
 * IMG_SUCCESS
 * @return NULL in case of an error and pResult has the value:
 * @li IMG_ERROR_INVALID_PARAMETERS if pDirectoryConfig is NULL or does not
 * contain function pointers
 * @li IMG_ERROR_MALLOC_FAILED if an internal allocation failed
 * @li IMG_ERROR_FATAL if the given IMGMMU_pfnPageAlloc returned NULL
 */
struct MMUDirectory* IMGMMU_DirectoryCreate(const IMGMMU_Info *pMMUFunctions,
    IMG_RESULT *pResult);

/**
 * @brief Destroy the MMUDirectory - assumes that the HW is not going to
 * access the memory any-more
 *
 * Does not invalidate any memory because it assumes that everything is not
 * used any-more
 */
IMG_RESULT IMGMMU_DirectoryDestroy(struct MMUDirectory *pDirectory);

/**
 * @brief Get access to the page table structure used in the directory (to be
 * able to write it to registers)
 *
 * @param pDirectory asserts if pDirectory is NULL
 *
 * @return the page table structure used
 */
IMGMMU_Page* IMGMMU_DirectoryGetPage(struct MMUDirectory *pDirectory);

/**
 * @brief Returns the directory entry value associated with the virtual
 * address
 *
 * @return -1 if the Directory's pfnPageRead is NULL
 *
 * @see IMGMMU_DirectoryGetPageTableEntry()
 */
IMG_UINT32 IMGMMU_DirectoryGetDirectoryEntry(struct MMUDirectory *pDirectory,
    IMG_UINTPTR uiVirtualAddress);

/**
 * @brief Returns the page table entry value associated with the virtual
 * address
 *
 * @return -1 if the Directory's pfnPageRead is NULL or if the associate page
 * table is invalid in the directory map
 *
 * @see IMGMMU_DirectoryGetDirectoryEntry()
 */
IMG_UINT32 IMGMMU_DirectoryGetPageTableEntry(struct MMUDirectory *pDirectory,
    IMG_UINTPTR uiVirtualAddress);

/**
 * @brief Create a PageTable mapping for a list of physical pages and device
 * virtual address
 *
 * @warning Mapping can cause memory allocation (missing pages) - do not call
 * while interrupts are disabled
 *
 * @param pDirectoryEntry directory to use for the mapping
 * @param aPhysPageList sorted array of physical addresses (ascending order).
 * The number of elements is
 * pDevVirtAddr->uiAllocSize/IMGMMU_GetCPUPageSize().
 * The device MMU page size IMGMMU_GetPageSize() is used to know how many of
 * the given addresses to generate when the device MMU page size is smaller
 * than the CPU page size.
 * @warning The function does not verify that the CPU page size is a multiple
 * of the device page size. The user of that function should verify that
 * beforehand as it is a SOC design decision.
 * @note This array can potentially be big, the caller may need to use vmalloc
 * if running the linux kernel (e.g. mapping a 1080p NV12 is 760 entries, 6080
 * Bytes - 2 CPU pages needed, fine with kmalloc; 4k NV12 is 3038 entries,
 * 24304 Bytes - 6 CPU pages needed, kmalloc would try to find 8 contiguous
 * pages which may be problematic if memory is fragmented)
 * @param pDevVirtAddr associated device virtual address. Given structure is
 * copied
 * @param uiMapFlags flags to apply on the page (typically 0x2 for Write Only,
 * 0x4 for Read Only) - the flag should not set bit 1 as 0x1 is the valid
 * flag.
 * @param pResult where to store the error code, should not be NULL
 *
 * @return The opaque handle to the MMUMapping object and pResult to
 * IMG_SUCCESS
 * @return NULL in case of an error an pResult has the value:
 * @li IMG_ERROR_INVALID_PARAMETERS if the allocation size is not a multiple
 * of IMGMMU_GetPageSize(),
 *     if the given list of page table is too long or not long enough for the
 * mapping or
 *     if the give flags set the invalid bit
 * @li IMG_ERROR_MEMORY_IN_USE if the virtual memory is already mapped
 * @li IMG_ERROR_MALLOC_FAILED if an internal allocation failed
 * @li IMG_ERROR_FATAL if a page creation failed
 */
struct MMUMapping* IMGMMU_DirectoryMap(struct MMUDirectory *pDirectoryEntry,
    IMG_UINT64 *aPhysPageList, const struct MMUHeapAlloc *pDevVirtAddr,
    unsigned int uiMapFlags, IMG_RESULT *pResult);

/**
 * @brief Un-map the mapped pages (invalidate their entries) and destroy the
 * mapping object
 *
 * This does not destroy the created Page Table (even if they are becoming
 * un-used) and does not change the Directory valid bits.
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT IMGMMU_DirectoryUnMap(struct MMUMapping *pMapping);

/**
 * @brief Remove the internal Page Table structures and physical pages that
 * have no mapping attached to them
 *
 * @note This function does not have to be used, but can be used to clean some
 * un-used memory out and clean the Directory valid bits.
 *
 * @return The number of clean directory entries
 */
IMG_UINT32 IMGMMU_DirectoryClean(struct MMUDirectory *pDirectory);

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
 * End of the IMGMMU_lib documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* IMGMMU_MMU_H */
