/**
*******************************************************************************
 @file mmu_defs.h

 @brief Internal MMU library header used to define MMU information at
 compilation time and have access to the error printing functions

 @note some constants are defined as static cosnt values to propagate signed
 operations

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
#ifndef MMU_DEFS_H
#define MMU_DEFS_H

#include <stdarg.h>  // for the printing function

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup IMGMMU_lib
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMGMMU_int documentation module
 *---------------------------------------------------------------------------*/

#ifndef IMGMMU_PHYS_SIZE
/** @brief MMU physical address size in bits */
#define IMGMMU_PHYS_SIZE 40
#endif

#ifndef IMGMMU_VIRT_SIZE
/** @brief MMU virtual address size in bits */
#define IMGMMU_VIRT_SIZE 32
#endif

#ifndef IMGMMU_PAGE_SIZE

/** @brief Page size in bytes */
#define IMGMMU_PAGE_SIZE 4096u
/** @brief should be log2(IMGMMU_PAGE_SIZE) */
#define IMGMMU_PAGE_SHIFT 12
/** @brief should be log2(IMGMMU_PAGE_SIZE)*2-2 */
#define IMGMMU_DIR_SHIFT 22

#endif

static const IMG_SIZE MMU_PAGE_SIZE = IMGMMU_PAGE_SIZE;
static const unsigned int MMU_PAGE_SHIFT = IMGMMU_PAGE_SHIFT;
static const unsigned int MMU_DIR_SHIFT = IMGMMU_DIR_SHIFT;

/** @brief Page offset mask in virtual address - bottom bits */
static const IMG_SIZE VIRT_PAGE_OFF_MASK = ((1 << IMGMMU_PAGE_SHIFT) - 1);

/** @brief Page table index mask in virtual address - middle bits */
static const IMG_SIZE VIRT_PAGE_TBL_MASK =
    (((1 << IMGMMU_DIR_SHIFT) - 1) & ~(((1 << IMGMMU_PAGE_SHIFT) - 1)));

/** @brief Directory index mask in virtual address - high bits */
static const IMG_SIZE VIRT_DIR_IDX_MASK = (~((1 << IMGMMU_DIR_SHIFT) - 1));

#if IMGMMU_VIRT_SIZE == 32
/**
 * @brief maximum number of pagetable that can be stored in the directory
 * entry
 */
#define IMGMMU_N_TABLE (IMGMMU_PAGE_SIZE/4u)
/** @brief maximum number of page mapping in the pagetable */
#define IMGMMU_N_PAGE (IMGMMU_PAGE_SIZE/4u)
#else
/* it is unlikely to change anyway */
#error "need an update for the new virtual address size"
#endif

/** @brief Memory flag used to mark a page mapping as valid */
#define MMU_FLAG_VALID 0x1
/** @brief Memory flag used to mark a page mapping as valid */
#define MMU_FLAG_INVALID 0x0

/*
 * internal printing functions
 */

void _MMU_Log(int err, const char *function, IMG_UINT32 line,
    const char* format, ...);

#define MMU_LogError(...) _MMU_Log(1, __FUNCTION__, __LINE__, __VA_ARGS__)

#define MMU_LogDebug(...)
// #define MMU_LogDebug(...) _MMU_Log(0, __FUNCTION__, __LINE__, __VA_ARGS__)

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the IMGMMU_int documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MMU_DEFS_H */
