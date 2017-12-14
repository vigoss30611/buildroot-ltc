/** 
************************************************************************** 
@file           ms/img_systypes.h
 
@brief          Base type definitions using Microsoft headers
 
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

/* Windows environment */
#include <stddef.h>

/*
 * integral types
 */

//typedef             char    IMG_CHAR; 
typedef          wchar_t    IMG_WCHAR;

//typedef	             int	IMG_INT;
typedef	          __int8	IMG_INT8;
typedef	         __int16	IMG_INT16;
typedef	         __int32	IMG_INT32;
typedef	         __int64	IMG_INT64;

//typedef	    unsigned int	IMG_UINT;
typedef	 unsigned __int8	IMG_UINT8;
typedef	unsigned __int16	IMG_UINT16;
typedef	unsigned __int32	IMG_UINT32;
typedef	unsigned __int64	IMG_UINT64;

/*
 * memory related
 */
typedef	 unsigned __int8	IMG_BYTE;   	/**< @brief Atom of memory */
typedef           size_t    IMG_SIZE;     /**< @brief Unsigned integer returned by sizeof operator (i.e. big enough to hold any memory allocation) (C89) */
typedef        uintptr_t    IMG_UINTPTR;    /**< @brief Integer vairable that can hold a pointer value (C99) */
typedef        ptrdiff_t    IMG_PTRDIFF;    /**< @brief Large enought to hold the signed difference of 2 pointer values (C89) */

#ifdef __cplusplus
}//extern C
#endif

#endif /* __IMG_SYSTYPES__ */
