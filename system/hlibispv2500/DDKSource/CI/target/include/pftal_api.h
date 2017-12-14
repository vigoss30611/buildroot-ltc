/*! 
******************************************************************************
 @file   : pftal_api.h

 @brief  Interface to the Portablity Framework version of the TAL.

 @Author Imagination Technologies

 @date   14/06/2012
 
         <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
         This file contains the user mode interface to the Portablity Framerk
		 version of the TAL.  Use for setting up the TAL structures in kernel
		 mode.

******************************************************************************/

#if !defined (__PFTAL_API_H__)
#define __PFTAL_API_H__

#include "tal.h"
#include "img_types"

#if defined(__cplusplus)
extern "C" {
#endif


#ifdef  __RPCCODEGEN__
  #define rpc_prefix      PFTAL
  #define rpc_filename    pftal_api
#endif


/*!
******************************************************************************

 @Function				PFTAL_MemSpaceRegister
 
 @Description 
 
 This function is used to register a memory space with the Portablity Framework
 version of the TAL.
 
 @Input		pszDeviceName	: The device name.

 @Input		ui32LenDeviceName : Length of the device name (in bytes)
 
 @Input		ui32DevFlags	: The device flags.
 
 @Input		pszMemSpaceName : The memory space name.

 @Input		ui32LenDeviceName : Length of the memory space name (in bytes)
  
 @Input		eMemSpaceType	: The type of memory space.
  
 @Input		ui64BaseOffset	: The memory space base offset.
  
 @Input		ui64Size		: The szie of the memory space (in bytes).
  
 @Input		ui32IntNum		: Interupt number.

 @Return    IMG_RESULT :    This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT PFTAL_MemSpaceRegister(
	IMG_CHAR *					pszDeviceName,
	IMG_UINT32					ui32LenDeviceName,
	IMG_UINT32					ui32DevFlags,
	IMG_CHAR *					pszMemSpaceName,
	IMG_UINT32					ui32LenMemSpaceName,
	TAL_eMemSpaceType			eMemSpaceType,
	IMG_UINT64					ui64BaseOffset,
	IMG_UINT64					ui64Size,
	IMG_UINT32					ui32IntNum
);

#if defined (__cplusplus)
}
#endif
 
#endif /* __PFTAL_API_H__	*/


