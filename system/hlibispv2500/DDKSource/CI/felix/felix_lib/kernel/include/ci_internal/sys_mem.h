/**
*******************************************************************************
 @file sys_mem.h

 @brief Device Memory handler - function definitions

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

#ifndef SYS_MEM_H_
#define SYS_MEM_H_

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>
#include <ci_internal/sys_device.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MMUHeap;  // from mmulib/heap.h
struct MMUMapping;  // from mmulib/mmu.h
struct MMUDirectory;  // from mmulib/mmu.h
struct MMUPage;  // from mmulib/mmu.h

struct vm_area_struct;  // linux/mm_types.h

// KRN_CI_API defined in ci_kernel.h
/**
 * @defgroup CI_KERNEL_TOOLS CI Kernel tools (SYS_)
 * @brief Kernel-side tools
 *
 * Defines functionalities that are dependent on the system the driver was 
 * compiled for (e.g. memory access)
 *
 * @ingroup KRN_CI_API
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_API documentation module
 *---------------------------------------------------------------------------*/

/** not defined if using FELIX_FAKE */
#if !defined(PAGE_SIZE) && defined(FELIX_FAKE)
#include <sys/sys_userio_fake.h>
#endif

struct SYS_MEM_ALLOC; /**< opaque type defined by allocator */

typedef struct SYS_MEM
{
    /**
     * @brief opaque type storing the allocation - chosen according to MMU
     * choice
     */
    struct SYS_MEM_ALLOC *pAlloc;
    IMG_SIZE uiAllocated;
    struct MMUHeapAlloc *pDevVirtAddr;
    /**
     * @brief additional virtual memory - 1st address has to be a multiple of
     * that
     */
    IMG_UINT32 ui32VirtualAlignment;
    /**
     * @brief associated mapping
     */
    struct MMUMapping *pMapping;
    IMG_BOOL8 bWasImported;
#ifdef INFOTM_ISP
    char szMemName[20];
    void* pVirtualAddress;
    void* pPhysAddress;
#endif //INFOTM_ISP

} SYS_MEM;

enum SYS_MEM_UPD
{
    SYS_MEM_UPD_HOST = 0,
    SYS_MEM_UPD_DEV,
};

/**
 * @name Device memory generic functions (SYS_Mem)
 * @brief Memory management functions
 *
 * Functions for allocating, freeing and getting CPU access.
 *
 * Currently these do not do any checking that the read/write get/release
 * behaviour matches up.
 * In theory it would be possible to do GetReadPointer followed by release
 * write pointer.
 * This would result in device memory updates in both directions.
 *
 * The Allocate function takes a hMemHandle, this allows it to know nothing
 * about the driver context.
 *
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the SYS_MEM functions group
 *---------------------------------------------------------------------------*/

/* no dynamic allocation of that structure */
/*IMG_RESULT IMG_CI_MemstructCreate(SYS_MEM **ppMemstruct);*/
/*IMG_RESULT IMG_CI_MemstructDestroy(SYS_MEM *pMemstruct);*/

/**
 * @brief Use a setted up memory structure and allocates its associated device
 * memory
 *
 * @param psMem
 * @param uiSize allocation size (in bytes)
 * @param pHeap heap to use to allocate device virtual address (if using MMU)
 * @param ui32VirtualAlignment allocates additional virtual memory to cope with
 *  possible alignment (e.g. for tiling 1st first address has to be aligned to
 * a defined value)
 *
 * The allocation size is rounded up using SYSMEM_ALIGNMENT.
 *
 * If CI_MEMSET_ALIGNMENT is defined the additional memory will be set using
 * this value.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETER if psMem is NULL or some of its
 * parameters are invalid
 * @return IMG_ERROR_MEMORY_IN_USE if psMem->hMem or psMem->pvCpuLin is not
 * NULL
 * @return IMG_ERROR_NOT_SUPPORTED if the driver global pointer is NULL
 */
#ifdef INFOTM_ISP
IMG_RESULT SYS_MemAlloc(SYS_MEM *psMem, IMG_SIZE uiSize, struct MMUHeap *pHeap,
    IMG_UINT32 ui32VirtualAlignment, char* pszName);
#else
IMG_RESULT SYS_MemAlloc(SYS_MEM *psMem, IMG_SIZE uiSize, struct MMUHeap *pHeap,
    IMG_UINT32 ui32VirtualAlignment);
#endif //INFOTM_ISP

/**
 * @brief Liberates memory allocated with SYS_MemAlloc() or SYS_MemImport()
 */
IMG_RESULT SYS_MemFree(SYS_MEM *psMem);

/**
 * @brief Get 1st address of an allocation (1st virtual if allocated using MMU
 * mapping or 1st physical otherwise)
 */
IMG_UINT32 SYS_MemGetFirstAddress(const SYS_MEM *psMem);

/**
 * @brief When mapping memory to the device MMU
 */
IMG_RESULT SYS_MemMap(SYS_MEM *psMem, struct MMUDirectory *pDir,
    unsigned int flag);

/** 
 * @brief To unmpa memory mapped with SYS_MemMap()
 */
IMG_RESULT SYS_MemUnmap(SYS_MEM *psMem);

/**
 * @brief Import memory allocated outside - released when SYS_MemFree
 *
 * @param psMem
 * @param uiSize in bytes
 * @param[in] pHeap virtual memory heap to use
 * @param ui32VirtualAlignment allocates additional virtual memory to cope with
 * possible alignment (e.g. for tiling 1st first address has to be aligned to a
 * defined value)
 * @param fd import mechanism ID to use
 */
#ifdef INFOTM_ISP
IMG_RESULT SYS_MemImport(SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment, int fd, char* pszName);
#else
IMG_RESULT SYS_MemImport(SYS_MEM *psMem, IMG_SIZE uiSize,
    struct MMUHeap *pHeap, IMG_UINT32 ui32VirtualAlignment, int fd);
#endif //INFOTM_ISP

/**
 * @brief release memory imported with SYS_MemImport()
 */
IMG_RESULT SYS_MemRelease(SYS_MEM *psMem);

/**
 * @brief Writes ui32Value into psMem+uiOffset
 *
 * @warning Platform_MemUpdateDevice() may need to be called before or after
 * this function!
 *
 * @note This can be done using Platform_MemGetMemory() and then writing to the
 * correct offset.
 * But this solution is writing pdumps when running FELIX_FAKE
 *
 * @note Uses Platform_MemGetTalHandle() to get
 */
IMG_RESULT SYS_MemWriteWord(SYS_MEM *psMem, IMG_SIZE uiOff,
    IMG_UINT32 ui32Value);

/**
 * @brief Read a 32b value
 *
 * @warning Platform_MemUpdateHost() may need to be called before or after this
 * function!
 *
 * @note This can be done using Platform_MemGetMemory() and then reading from
 * the correct offset.
 * But this solution is writing pdumps when running FELIX_FAKE
 */
IMG_RESULT SYS_MemReadWord(SYS_MEM *psMem, IMG_SIZE uiOff,
    IMG_UINT32 *pui32Value);

/**
 * @brief Writes pToWrite address+uiToWriteOffset into psMem+uiOffset
 *
 * @warning Platform_MemGetMemory() and Platform_MemUpdateDevice() still need
 * to be called before or after this function!
 *
 * @note This can be done using Platform_MemGetMemory() and then writing to the
 * correct offset.
 * But this solution is writing pdumps when running FELIX_FAKE
 */
IMG_RESULT SYS_MemWriteAddress(SYS_MEM *psMem, IMG_SIZE uiOffset,
    const SYS_MEM *pToWrite, IMG_SIZE uiToWriteOffset);

/**
 * @brief Writes pToWrite+uiToWriteOffset into the registers
 *
 * @warning Platform_MemUpdateDevice still need to be called before or after
 * this function to ensure the HW has access to the correct memory
 */
IMG_RESULT SYS_MemWriteAddressToReg(IMG_HANDLE pTalRegHandle,
    IMG_SIZE uiOffset, const SYS_MEM *pToWrite, IMG_SIZE uiToWriteOffset);

/**
 * @}
 *
 * @name Platform System memory management functions (Platform_)
 * @brief This is defined by the compiled allocator: TAL, ION or OS specific
 * @{
 */

/**
 * @brief Allocates a bit of memory that does not have to be contiguous
 *
 * @param[in] uiRealAlloc size in Bytes
 * @param[in] extra additional information needed when using Fake driver
 * (base TAL handle for the memory)
 * @param[out] ret result (IMG_SUCCESS or other)
 *
 * @return the new allocation or NULL if failed and ret contains a return code
 *
 * See @ref Platform_MemAllocContiguous()
 */
struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE uiRealAlloc, void *extra,
    IMG_RESULT *ret);

/**
 * @brief Allocates a bit of memory that has to be contiguous.
 *
 * Memory will be released with Platform_MemFree()
 *
 * @param[in] uiRealAlloc size in Bytes
 * @param[in] extra additional information needed when using Fake driver
 * (base TAL handle for the memory)
 * @param[out] ret result (IMG_SUCCESS or other)
 *
 * @return the new allocation or NULL if failed and ret contains a return code
 *
 * See @ref Platform_MemAlloc()
 */
struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE uiRealAlloc,
    void *extra, IMG_RESULT *ret);

/**
 * @brief Liberate some allocated memory
 */
void Platform_MemFree(struct SYS_MEM_ALLOC *psMem);

/**
 * @brief Imports memory allocated by another source
 *
 * Memory will be released with Platform_MemRelease()
 *
 * @param[in] fd file descriptor to import
 * @param[out] allocationSize size of the imported memory
 * @param[out] ret result (IMG_SUCCESS or other)
 *
 * @return the new allocation or NULL if failed and ret contains a return code
 */
struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret);

/**
 * @brief Release imported memory
 */
void Platform_MemRelease(struct SYS_MEM_ALLOC * mem);

/**
 * @brief Get CPU accessible memory
 */
void* Platform_MemGetMemory(SYS_MEM *psMem);

/**
 * @brief Should update CPU side memory in NUMA systems - does nothing on UMA
 * systems
 */
IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem);

/**
 * @brief Should update Device side memory in NUMA systems - does nothing on
 * UMA system
 */
IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem);

/**
 * @brief Returns the physical page list used for the allocation
 * 
 * Should contain allocSize/PAGE_SIZE elements
 *
 * @note May need some cleaning in the future (current allocator returns the
 * actual list not a copy so no cleaning needed!)
 */
IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem);

/**
 * @brief Get physical device address (debug only?)
 */
IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE uiOff);

/**
 * @brief use when mapping kernel-space memory to user-space (ci_ioctl_km.c
 * IMG_CI_map())
 */
IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem,
    struct vm_area_struct *vma);

/**
 * @brief Access to the TAL HANDLE used when allocating memory to allow 
 * read/write to memory (or use memory to write to registers) in SYS_
 * functions
 *
 * When using Fake driver this should return the TAL handle for the page to
 * write into.
 * If the allocation is virtual will return the actual page in the list and
 * pOff2 will be set to the offset in that page.
 *
 * For real driver the memory should be mapped to the CPU and so that the CPU
 * virtual address is linear.
 * Therefore this function should return the CPU virtual address and pOff2
 * should be the same than uiOffset
 *
 * Example with fake driver:
 * @li allocation of 7kB of memory will result in 2 pages allocation
 * (2*4k = 8k): page 0 and page 1
 * @li access to offset 1000 in the memory will need to access to page 0,
 * offset 1000
 * @li access to offset 5000 in the memory will need to access to page 1,
 * offset 904
 *
 * @param[in] psMem allocated memory
 * @param[in] uiOffset offset in the memory
 * @param[out] pOff2 offset in the memory after transformation
 *
 * @return memory handle for the TAL
 */
IMG_HANDLE Platform_MemGetTalHandle(const SYS_MEM *psMem, IMG_SIZE uiOffset,
    IMG_SIZE *pOff2);

/**
 * @}
 *
 * @name Platform MMU memory management functions (Platform_)
 * @brief This is defined by the compiled allocator: TAL, ION or OS specific
 * to manage the memory the MMU block will use
 *
 * @note IMGMMU_pfnPageUpdateDevice is not implemented because it is assumed 
 * that the MMU memory is uncached and does not need flushing
 * @{
 */

/**
 * @brief Allocate a page for the MMU mapping
 *
 * Should implement IMGMMU_pfnPageAlloc
 */
struct MMUPage* Platform_MMU_MemPageAlloc(void);

/**
 * @brief Free a page used for MMU mapping
 *
 * Should implement IMGMMU_pfnPageFree
 */
void Platform_MMU_MemPageFree(struct MMUPage *pPub);

/**
 * @brief Function to use when writing to a page
 *
 * Should implement IMGMMU_pfnPageWrite
 */
void Platform_MMU_MemPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags);

/**
 * @brief Function to use when reading from a page
 *
 * Should implement IMGMMU_pfnPageRead
 */
IMG_UINT32 Platform_MMU_MemPageRead(struct MMUPage *pReadFrom,
    unsigned int offset);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of System memory management
 *---------------------------------------------------------------------------*/
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_KERNEL_TOOLS documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SYS_MEM_H_ */
