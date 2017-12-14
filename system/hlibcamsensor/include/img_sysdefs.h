/**
**************************************************************************
@file           c99/img_sysdefs.h

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

#include <string.h> // strcpy, ... and memcpy, ...
#include <assert.h>
#ifdef __android__
// in adroid use the system log
#include <android/log.h>
#endif
#include <stdlib.h> // malloc, ...
#include <stdio.h> // printf for IMG_ASSERT

#ifdef __cplusplus
extern "C" {
#endif

/*
 * language abstraction
 */

#define IMG_CONST const
#define IMG_INLINE inline

// useful?
//#define IMG_CALLCONV __stdcall
//#define IMG_INTERNAL
//#define IMG_EXPORT	__declspec(dllexport)

// not very nice
#define IMG_LITTLE_ENDIAN	(1234)
#define IMG_BIG_ENDIAN		(4321)
/* Win32 is little endian */
#define	IMG_BYTE_ORDER		IMG_LITTLE_ENDIAN

/**
 * @brief 64bit value prefix - e.g. printf("long %" IMG_I64PR "d")
 */
#ifdef __GNUC__
#  ifdef __LP64__
#    define IMG_I64PR "l"
#  else
#    define IMG_I64PR "ll"
#  endif
#elif defined(__WORDSIZE)
#  if __WORDSIZE == 64
#    define IMG_I64PR "l"
#  else
#    define IMG_I64PR "ll"
#  endif
#else
#  if defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#    define IMG_I64PR "l"
#  else
#    define IMG_I64PR "ll"
#  endif
#endif
/** @brief IMG_SIZE (i.e. size_t) format prefix - e.g. IMG_SIZE s -> printf("sizeof= %" IMG_SIZEPR "u", s) */
#define IMG_SIZEPR "z"
/** @brief IMG_PTRDIFF (i.e. ptrdiff_t) format prefix - e.g. IMG_PTRDIFF diff -> printf(diff= 0x%" IMG_PTRDPR "x", s) */
#define IMG_PTRDPR "t"
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
/** @brief With GCC this aligns the memory */
#define IMG_ALIGN(bytes)       __attribute__ ((aligned (bytes)))

/*
 * string operation
 */
#define IMG_SYS_STRDUP(ptr)		strdup(ptr)
#define IMG_STRCMP(A,B)		strcmp(A,B)
#define IMG_STRCPY(dest,src)		strcpy(dest,src)
#define IMG_STRNCPY(dest,src,size)	strncpy(dest,src,size)
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
#if defined(__APPLE__)
#ifndef IMG_NO_FTELL64
#define IMG_FTELL64 ftello
#endif
#ifndef IMG_NO_FSEEK64
#define IMG_FSEEK64 fseeko
#endif
#else
#ifndef IMG_NO_FTELL64
#define IMG_FTELL64 ftello64
#endif
#ifndef IMG_NO_FSEEK64
#define IMG_FSEEK64 fseeko64
#endif
#endif
/*
 * assert behaviour
 *                               NDEBUG
 *                      0                 1
 * EXIT_ON_ASSERT  0   test||print        void
 *                 1   assert()           assert() // ignored: no print, no exit
 *
 * on android uses the system log: __android_log_assert and __android_log_print
 */

// C89 says: if NDEBUG is defined at the time <assert.h> is included the assert macro is ignored
#ifndef EXIT_ON_ASSERT // NO_EXIT_ON_ASSERT is defined

#ifdef NDEBUG // assert should be ignored
#define IMG_ASSERT(expected) ((void)0)
#else

#ifdef __android__
#define IMG_ASSERT(expected) (void)( (expected) || (__android_log_print(ANDROID_LOG_ERROR, "IMG", "Assertion failed: %s, file %s, line %d\n", #expected, __FILE__, __LINE__),0) )
#else
#define IMG_ASSERT(expected) (void)( (expected) || (fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expected, __FILE__, __LINE__),0) )
#endif

#endif // NDEBUG

#endif // NO_EXIT_ON_ASSERT
#ifndef IMG_ASSERT
#ifdef __android__
#define IMG_ASSERT(expected) (void)((expected) ? 0 : (__android_log_assert(#expected, "IMG", "ERROR: Assert '%s' at %s:%d", #expected, __FILE__,__LINE__),0))
#else
#define IMG_ASSERT(expected) assert(expected)
#endif //__android__
#endif // not def IMG_ASSERT

/**
 * @brief Cast a member of a structure out to the containing structure
 * @param ptr the pointer to the member.
 * @param type the type of the container struct.
 * @param member the name of the member within the container struct.
 */
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#ifdef __cplusplus
}
#endif

#endif // __IMG_SYSDEFS__
