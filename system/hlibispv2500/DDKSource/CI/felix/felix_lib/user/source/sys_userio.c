/**
*******************************************************************************
@file sys_userio.c

@brief Implementation of SYS_IO for real systems

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

#include <fcntl.h>  // open
#include <unistd.h>  // close
#include <sys/ioctl.h>  // ioctl
#include <sys/mman.h>  // mmap
#include <errno.h>

#include <img_defs.h>  // NO_LINT
#define LOG_TAG "SYS_USERIO"
#include <felixcommon/userlog.h>

#include "ci/ci_api_internal.h"

struct __SYS_FILE
{
    int file;
};

SYS_FILE* SYS_IO_Open(const char *pszDevName, int flags, void *extra)
{
    int ret;
    SYS_FILE *pFile = (SYS_FILE*)IMG_CALLOC(1, sizeof(SYS_FILE));
    (void)extra;  // unused

    if (!pFile)
    {
        LOG_ERROR("Failed to allocate memory!\n");
        return NULL;
    }

    ret = open(pszDevName, flags);
    if (ret >= 0)
    {
        pFile->file = ret;
    }
    else
    {
        LOG_ERROR("Failed to open '%s'!\n", pszDevName);
        IMG_FREE(pFile);
        pFile = NULL;
    }
    return pFile;
}

IMG_RESULT SYS_IO_Close(SYS_FILE *pFile)
{
    int ret = 0;
    IMG_ASSERT(pFile != NULL);  // it is a bug not to give a file object
    ret = close(pFile->file);
    if (0 == ret)
    {
        IMG_FREE(pFile);
    }
    return ret;
}

int SYS_IO_Control(SYS_FILE *pFile, unsigned int command, long int parameter)
{
    int ret = 0;
    IMG_ASSERT(pFile != NULL);  // it is a bug not to give a file object
    ret = ioctl(pFile->file, command, parameter);
    if (ret >= 0)
    {
        return ret;
    }
    else
    {
        /* ioctl return code is cropped by the kernel when negative and
         * the error code (positive part) is put into errno */
        return -errno;
    }
}

IMG_INLINE void* SYS_IO_MemMap2(SYS_FILE *pFile, size_t length, int prot,
    int flags, off_t offset)
{
    void *ret = NULL;
    IMG_ASSERT(pFile != NULL);  // it is a bug not to give a file object
    /* printf("user mmap(NULL, length=%ld, prot=0x%X, flags=0x%X, file=%p,
        offset=%ld)\n", length, prot, flags, pFile, offset);*/
    /* the difference between mmap and mmap2 is that mmap2 is taking offset
     * in pages rather than bytes (to support larger range of memory when
     * running 32b OS) and apparently is only available for 32b Linux
     * on 64b OS we can use mmap() and getpagesize() to have the same
     * interface (offset in pages instead of bytes as expected by mmap()
     */
#ifdef mmap2
    ret = mmap2(NULL, length, prot, flags, pFile->file, offset);
#else
    ret = mmap(NULL, length, prot, flags, pFile->file, offset*getpagesize());
#endif

    if (MAP_FAILED == ret)
    {
        return NULL;
    }
    return ret;
}

IMG_INLINE IMG_RESULT SYS_IO_MemUnmap(SYS_FILE *pFile, void *addr,
    size_t length)
{
    IMG_ASSERT(pFile != NULL);
    (void)pFile;  // unsused
    return munmap(addr, length);
}
