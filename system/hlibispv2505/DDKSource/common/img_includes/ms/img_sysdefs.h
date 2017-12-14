/** 
************************************************************************** 
@file           ms/img_sysdefs.h
 
@brief          
 
@copyright Imagination Technologies Ltd. All Rights Reserved. 
    
@license        <Strictly Confidential.> 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 
**************************************************************************/

#ifndef __IMG_SYSDEFS__
#define __IMG_SYSDEFS__

#include <string.h> // strcpy, ...
#include <assert.h>
#include <stdlib.h> // malloc, ...
#include <stdio.h> // printf for IMG_ASSERT

#include <memory.h> // memcpy, ...

#ifdef __cplusplus
extern "C" {
#endif

/*
 * language abstraction
 */
 
#define IMG_CONST const
#ifdef __cplusplus
#define IMG_INLINE inline
#else
#define IMG_INLINE __inline
#endif

// useful?
//#define IMG_CALLCONV __stdcall
//#define IMG_INTERNAL
//#define IMG_EXPORT	__declspec(dllexport)

#define IMG_LITTLE_ENDIAN	(0)
#define IMG_BIG_ENDIAN		(1)
/* Win32 is little endian */
#define	IMG_BYTE_ORDER		IMG_LITTLE_ENDIAN

/*
 * memory operation
 */

#define	IMG_MEMCPY(dest,src,size)	memcpy	(dest,src,size)
#define	IMG_MEMSET(ptr,val,size)	memset	(ptr,val,size)
#define IMG_MEMCMP(A,B,size)		memcmp	(A,B,size)
#define IMG_MEMMOVE(dest,src,size)	memmove	(dest,src,size)

/** @brief C89 says: space is unitialized */
#define IMG_SYSMALLOC(size)       malloc(size)
/** @brief C89 says: space is initialized to zero bytes */
#define IMG_SYSCALLOC(nelem, elem_size)    calloc(nelem, elem_size)
/** @brief C89 says: if the size is larger the new space is uninitialized. */
#define IMG_SYSREALLOC(ptr, size)   realloc(ptr, size)
#define IMG_SYSFREE(ptr)            free(ptr)
#define IMG_SYSBIGALLOC(size)       IMG_SYSMALLOC(size)
#define IMG_SYSBIGFREE(ptr)         IMG_SYSFREE(ptr)
/** @brief Aligns the memory - on windows it does nothing */
#define IMG_ALIGN(bytes)

/** @brief 64bit value format prefix - e.g. IMG_UINT64 i -> printf("long %" IMG_I64PR "d", i) */
#define IMG_I64PR "I64"
/** @brief IMG_SIZE (i.e. size_t) format prefix - e.g. IMG_SIZE s -> printf("sizeof= %" IMG_SIZEPR "u", s) */
#define IMG_SIZEPR "I"
/** @brief IMG_PTRDIFF (i.e. ptrdiff_t) format prefix - e.g. IMG_PTRDIFF diff -> printf(diff= 0x%" IMG_PTRDPR "x", s) */
#define IMG_PTRDPR IMG_SIZEPR

/*
 * string operation
 */
#define IMG_SYS_STRDUP(ptr)		_strdup(ptr)
#define IMG_STRCMP(A,B)		strcmp(A,B)
#define IMG_STRCPY(dest,src)		strcpy(dest, src)
#define IMG_STRNCPY(dest,src,size) strncpy(dest, src, size)
#define IMG_STRLEN(ptr) strlen(ptr)
 
/*
 * file operation
 */

 /*
 * If IMG_NO_FSEEK64 is defined then the FSEEK64 is not set
 * This allows the projects to choose if they want FSEEK64 support
 *
 * Similar operation for FTELL64
 */
 
#ifndef IMG_NO_FTELL64
#define IMG_FTELL64 _ftelli64
#endif
#ifndef IMG_NO_FSEEK64
#define IMG_FSEEK64 _fseeki64
#endif

/*
 * assert behaviour
 *                               NDEBUG
 *                      0                 1
 * EXIT_ON_ASSERT  0   test||print        void
 *                 1   assert()           assert() // ignored  no print, no exit
 */
 
// C89 says: if NDEBUG is defined at the time <assert.h> is included the assert macro is ignored
#ifndef EXIT_ON_ASSERT // NO_EXIT_ON_ASSERT is defined

#ifdef NDEBUG // assert should be ignored
#define IMG_ASSERT(expected) ((void)0)
#else
#define IMG_ASSERT(expected) (void)( (expected) || (fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expected, __FILE__, __LINE__),0) )
#endif

#endif // NO_EXIT_ON_ASSERT
#ifndef IMG_ASSERT
#define IMG_ASSERT(expected) assert(expected)
#endif

/**
 * @brief Cast a member of a structure out to the containing structure
 * @param ptr the pointer to the member.
 * @param type the type of the container struct.
 * @param member the name of the member within the container struct.
 */
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/* because not available directly on win32 but very useful
 * sometimes also as sprintf_s */
#define snprintf _snprintf
#define vnsprintf _vnsprintf

#ifdef __cplusplus
}
#endif

#endif // __IMG_SYSDEFS__
