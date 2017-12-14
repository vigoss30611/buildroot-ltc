/** 
*******************************************************************************
@file           linux-kernel/img_systypes.h
 
@brief          Base type definitions using linux kernel headers
 
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
******************************************************************************/

#ifndef __IMG_SYSTYPES__
#define __IMG_SYSTYPES__

#ifdef __cplusplus
// this could even be erroneous because that's a kernel header!
extern "C" {
#endif

/* Posix environment */
#include <linux/stddef.h>  // size_t and ptrdiff_t
#include <linux/types.h>

/*
 * integral types
 */

//typedef char IMG_CHAR; 
typedef s16 IMG_WCHAR;

//typedef int IMG_INT;
typedef s8 IMG_INT8;
typedef s16 IMG_INT16;
typedef s32 IMG_INT32;
typedef s64 IMG_INT64;

//typedef unsigned int IMG_UINT;
typedef u8 IMG_UINT8;
typedef u16 IMG_UINT16;
typedef u32 IMG_UINT32;
typedef u64 IMG_UINT64;

/*
 * memory related
 */
typedef u8 IMG_BYTE; /**< @brief Atom of memory */
/**
 * @brief Unsigned integer returned by sizeof operator (i.e. big enough to
 * hold any memory allocation) (C89)
 */
typedef size_t IMG_SIZE;
/** @brief Integer vairable that can hold a pointer value (C99) */
typedef uintptr_t IMG_UINTPTR;
/**
 * @brief Large enought to hold the signed difference of 2 pointer
 * values (C89)
 */
typedef ptrdiff_t IMG_PTRDIFF;

#ifdef __cplusplus
}  //extern C
#endif

#endif /* __IMG_SYSTYPES__ */
