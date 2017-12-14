/**
*******************************************************************************
@file sys_userio_fake.c

@brief implementation of fake system calls to directly hook into fake-kernel
module

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
#include "sys/sys_userio.h"

#include "sys/sys_userio_fake.h"

#include <img_defs.h>
#include <img_errors.h>

/**
 * This implementation is the abstraction of "Fake device" where the open()
 * calls is directly linked to the kernel-module code.
 */
struct __SYS_FILE
{
    /**
     * @brief extra is a pointer to struct SYS_IO_FakeOperation
     * (sys_userio_fake.h) - NULL if using normal call
     */
    struct file_operations *pFakeOps;
    struct krn_env sKrnEnv;
};

// #ifndef PAGE_SHIFT
// #define PAGE_SHIFT 12
// #endif

SYS_FILE * SYS_IO_Open(const char *pszDevName, int flags, void *extra)
{
    int ret = 0;
    SYS_FILE *pFile = NULL;

    // it is a bug not to give a device name
    IMG_ASSERT(pszDevName);
    // it is a bug not to give an extra pointer
    IMG_ASSERT(extra);
    // it is a bug not to have this function
    IMG_ASSERT(((struct file_operations*)extra)->open);

    pFile = (SYS_FILE*)IMG_CALLOC(1, sizeof(SYS_FILE));
    if (!pFile)
    {
        return pFile;
    }

    pFile->pFakeOps = (struct file_operations *)extra;

    ret = pFile->pFakeOps->open(&(pFile->sKrnEnv.inode),
        &(pFile->sKrnEnv.fdes));
    if (ret)
    {
        IMG_FREE(pFile);
        return NULL;
    }

    return pFile;
}

int SYS_IO_Close(SYS_FILE *pFile)
{
    int ret = 0;

    // it is a bug not to give a file object
    IMG_ASSERT(pFile != NULL);
    // it is a bug not to have the fake operations
    IMG_ASSERT(pFile->pFakeOps != NULL);
    // it is a bug not to have this function
    IMG_ASSERT(pFile->pFakeOps->release != NULL);

    ret = pFile->pFakeOps->release(&(pFile->sKrnEnv.inode),
        &(pFile->sKrnEnv.fdes));
    if (0 == ret)
    {
        IMG_FREE(pFile);
    }

    return ret;
}

int SYS_IO_Control(SYS_FILE *pFile, unsigned int command, long int parameter)
{
    int ret = 0;

    // it is a bug not to give a file object
    IMG_ASSERT(pFile != NULL);
    // it is a bug not to have the fake operations
    IMG_ASSERT(pFile->pFakeOps != NULL);
    // it is a bug not to have this function
    IMG_ASSERT(pFile->pFakeOps->unlocked_ioctl != NULL);

    ret = pFile->pFakeOps->unlocked_ioctl(&(pFile->sKrnEnv.fdes), command,
        parameter);

    return ret;
}

void* SYS_IO_MemMap2(SYS_FILE *pFile, size_t length, int prot, int flags,
    off_t offset)
{
    int ret = 0;
    struct vm_area_struct vma = { prot, flags, offset, NULL, length };

    // it is a bug not to give a file object
    IMG_ASSERT(pFile != NULL);
    // it is a bug not to have the fake operations
    IMG_ASSERT(pFile->pFakeOps != NULL);
    // it is a bug not to have this function
    IMG_ASSERT(pFile->pFakeOps->mmap != NULL);
    // linux kernel divide the offset to be in pages not in bytes
    // vma.vm_pgoff >>= PAGE_SHIFT;

    ret = pFile->pFakeOps->mmap(&(pFile->sKrnEnv.fdes), &vma);
    if (ret)
    {
        return NULL;
    }

    return vma.vm_private_data;
}

int SYS_IO_MemUnmap(SYS_FILE *pFile, void *addr, size_t length)
{
    int ret = 0;
    (void)addr;  // unsused
    (void)length;  // unused

    // it is a bug not to give a file object
    IMG_ASSERT(pFile != NULL);
    // it is a bug not to have the fake operations
    IMG_ASSERT(pFile->pFakeOps != NULL);
    // it is a bug not to have this function
    IMG_ASSERT(pFile->pFakeOps->munmap != NULL);

    ret = pFile->pFakeOps->munmap(addr, length);

    return ret;
}

static IMG_UINT32 g_pageSize = 4096;
static IMG_UINT32 g_pageShift = 12;

unsigned int getPageSize()
{
    return g_pageSize;
}

unsigned int getPageShift()
{
    return g_pageShift;
}

int setPageSize(unsigned int pageSize)
{
    IMG_UINT32 pageShift = 0;
    IMG_UINT32 tmpPage = pageSize;

    while (tmpPage >>= 1)  // compute log2()
    {
        pageShift++;
    }

    if (1 << pageShift != pageSize)  // page size has to be a power of 2
    {
        return -1;
    }

    g_pageSize = pageSize;
    g_pageShift = pageShift;

    return 0;
}
