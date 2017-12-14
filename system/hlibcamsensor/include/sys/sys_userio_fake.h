/**
*******************************************************************************
 @file sys_userio_fake.h

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

******************************************************************************/
#ifndef SYS_USERIO_FAKE_H_
#define SYS_USERIO_FAKE_H_

#include <img_types.h>  // for IMG_HANDLE in SYS_MEM_ALLOC
#include <stddef.h>  // size_t

#ifdef __cplusplus
extern "C" {
#endif

/** default 4096 but can be changed at run-time for testing */
#define PAGE_SIZE getPageSize()
/** default 12 but can be changed at run-time for testing */
#define PAGE_SHIFT getPageShift()

/**
 * @brief Fake inode (from the linux kernel)
 *
 * Used when faking IOCTL in user mode
 */
struct inode
{
    int placeholder;
};

/**
 * @brief Fake file structure (from the linux kernel)
 *
 * Used when faking IOCTL in user mode
 */
struct file
{
    /** @brief Can be used to store private data */
    void* private_data;
};

typedef int pgprot_t;

struct vm_area_struct
{
    /** Access permissions of this VMA. */
    pgprot_t vm_page_prot;
    /** Flags, see mm.h. */
    unsigned long vm_flags;
    /** Offset (within vm_file) in PAGE_SIZE units, *not* PAGE_CACHE_SIZE */
    unsigned long vm_pgoff;

    /** pointer to the result memory when using fake device */
    void* vm_private_data;
    size_t fake_length;
};

struct krn_env
{
    struct file fdes;
    struct inode inode;
};

/** @brief similar to the struct file_opearations defined in linux/fs.h */
struct file_operations
{
    int (*open)(struct inode *inode, struct file *flip);
    int (*release)(struct inode *inode, struct file *flip);
    long (*unlocked_ioctl)(struct file *flip, unsigned int ioctl_cmd,
        unsigned long ioctl_param);
    int (*mmap)(struct file *flip, struct vm_area_struct *vma);
    /**
     * @brief This is not going to be linked to kernel-module call
     * - does not exists in the linux structure
     */
    int (*munmap)(void *addr, size_t length);
};

/**
 * @brief defined in sys_userio_fake.c to allow changing the page size of
 * the system
 */
unsigned int getPageSize();
/**
 * @brief defined in sys_userio_fake.c to allow changing the page size of
 * the system
 */
unsigned int getPageShift();
/**
 * @brief defined in sys_userio_fake.c to allow changing the page size of
 * the system
 */
int setPageSize(unsigned int pageSize);

struct MMU_TALAlloc;  // tallalloc.h - needed for SYS_MEM_ALLOC

/**
 * @brief Used for physical linear alloc
 *
 * Should normally be defined in sys_mem_tal.c but we need the structure
 * when using unit-tests
 */
struct SYS_MEM_ALLOC
{
    /**
     * used for non-contiguous buffers, should be first to be used in
     * sys_mem.c
     */
    struct MMU_TALAlloc *pMMUAlloc;

    /** used for contiguous buffers */
    IMG_HANDLE pTalHandle;
    void *pCPUMem;
};

#ifdef __cplusplus
}
#endif

#endif /* SYS_USERIO_FAKE_H_ */
