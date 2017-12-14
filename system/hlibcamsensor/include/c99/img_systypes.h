/** 
************************************************************************** 
@file           c99/img_systypes.h
 
@brief          Base type definitions using C99 headers
 
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

#ifndef __IMG_SYSTYPES__
#define __IMG_SYSTYPES__

#ifdef __cplusplus
extern "C" {
#endif

/* Posix environment */
#include <stdint.h>
#include <stddef.h> // size_t and ptrdiff_t
#include <wchar.h>

/*
 * integral types
 */

//typedef             char    IMG_CHAR; 
typedef         wchar_t    IMG_WCHAR;

//typedef	             int	IMG_INT;
typedef	          int8_t	IMG_INT8;
typedef	         int16_t	IMG_INT16;
typedef	         int32_t	IMG_INT32;
typedef	         int64_t	IMG_INT64;

//typedef	    unsigned int	IMG_UINT;
typedef          uint8_t	IMG_UINT8;
typedef	        uint16_t	IMG_UINT16;
typedef	        uint32_t	IMG_UINT32;
typedef	        uint64_t	IMG_UINT64;

/*
 * memory related
 */
typedef	         uint8_t	IMG_BYTE;   	/**< @brief Atom of memory */
typedef           size_t    IMG_SIZE;     /**< @brief Unsigned integer returned by sizeof operator (i.e. big enough to hold any memory allocation) (C89) */
typedef        uintptr_t    IMG_UINTPTR;    /**< @brief Integer vairable that can hold a pointer value (C99) */
typedef        ptrdiff_t    IMG_PTRDIFF;    /**< @brief Large enought to hold the signed difference of 2 pointer values (C89) */

#ifdef __cplusplus
}//extern C
#endif

#endif /* __IMG_SYSTYPES__ */
