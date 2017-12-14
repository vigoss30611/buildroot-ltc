/**
*******************************************************************************
 @file sys_userio.h

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
#ifndef SYS_USERIO_H_
#define SYS_USERIO_H_

#include <img_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System file handler - it is an opaque type that is going to be 
 * different in fake device version and real device version
 */
struct __SYS_FILE
{
    int file;
};
typedef struct __SYS_FILE SYS_FILE;

// because off_t does not exists on windows
#ifdef WIN32
typedef long off_t;  // NOLINT

/**
 * @brief Fake open options
 */
enum OpenOpt
{
    O_RDONLY = 1<<0,
    O_WRONLY = 1<<1,
    O_RDWR = 1<<2,
};
enum MapProt
{
    PROT_NONE,
    PROT_EXEC,
    PROT_READ,
    PROT_WRITE
};
enum MapFlags
{
    MAP_PRIVATE,
    MAP_SHARED,
    // plenty of others that are not used
};

enum SyncFlags
{
    MS_SYNC,
    MS_ASYNC,
    MS_INVALIDATE,
};

#else
#include <sys/types.h>
#include <fcntl.h>  // open flags
#include <sys/mman.h>  // map options
#endif

/**
 * @brief Equivalent to system call open() 
 *
 * @Input pszDevName device name
 * @Input flags open flags
 * @Input extra is an <b>optional</b> pointer to struct SYS_IO_FakeOperation
 * (sys_userio_fake.h) when using fake device implementation - NULL if using
 * normal call
 *
 * @return the created SYS_FILE
 * @return NULL on failure
 */
SYS_FILE * SYS_IO_Open(const char *pszDevName, int flags, void *extra);

/**
 * @brief Equivalent to system call close()
 */
int SYS_IO_Close(SYS_FILE *pFile);

/**
 * @brief Equivalent to system call ioctl()
 */
int SYS_IO_Control(SYS_FILE *pFile, unsigned int command, long int parameter);

/**
 * @brief Equivalent to system call mmap2() (similar to mmap() but expects an
 * offset in pages instead of bytes)
 *
 * @return NULL on error (MAP_FAILED like mmap)
 */
void* SYS_IO_MemMap2(SYS_FILE *pFile, size_t length, int prot, int flags,
    off_t offset);

/**
 * @brief Equivalent to system call munmap()
 */
int SYS_IO_MemUnmap(SYS_FILE *pFile, void *addr, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* SYS_USERIO_H_ */
