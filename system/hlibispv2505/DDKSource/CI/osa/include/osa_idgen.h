/*! 
******************************************************************************
 @file   : idgen_api.h

 @brief  The ID Generation API.

 @Author Imagination Technologies

 @date   05/01/2011
 
         <b>Copyright 2011 by Imagination Technologies Limited.</b>\n
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
         This file contains the header file information for the
		 ID Generation API.

 \n<b>Platform:</b>\n
	     Platform Independent 

******************************************************************************/
/* 
******************************************************************************
*****************************************************************************/


#if !defined (__OSA_IDGEN_H__)
#define __OSA_IDGEN_H__	//!< Defined to prevent file being included more than once

#include <img_errors.h>
#include <img_types.h>
#include <img_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <lst.h>



/*!
******************************************************************************

 @Function				OSAIDGEN_CreateContext
 
 @Description 
 
 This function is used to create Id generation context.

 NOTE: Should only be called once to setup the context structure.

 NOTE: The client is responsible for providing thread/process safe locks on
 the context structure to maintain coherency.

 @Input		ui32MaxId	    : The maximum valid Id.

 @Input		ui32BlkSize	    : The number of handle per block. In case of incrementing
                              ids, size of the Hash table.

 @Input     bIncId          : Incrementing Ids
 
 @Output	phIdGenHandle	: A pointer used to return the context handle.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT OSAIDGEN_CreateContext(
	IMG_UINT32					ui32MaxId,
	IMG_UINT32					ui32BlkSize,
    IMG_BOOL                    bIncId,
	IMG_HANDLE *				phIdGenHandle
);


/*!
******************************************************************************

 @Function				OSAIDGEN_DestroyContext
 
 @Description 
 
 This function is used to destroy an Id generation context.  This function
 discards any handle blocks associated with the context.

 NOTE: The client is responsible for providing thread/process safe locks on
 the context structure to maintain coherency.
 
 @Input		hIdGenHandle	: The context handle.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT OSAIDGEN_DestroyContext(
	IMG_HANDLE 					hIdGenHandle
);


/*!
******************************************************************************

 @Function				OSAIDGEN_AllocId
 
 @Description 
 
 This function is used to associate a handle with an Id.

 NOTE: The client is responsible for providing thread/process safe locks on
 the context structure to maintain coherency.
 
 @Input		hIdGenHandle	: The context handle.

 @Input		hHandle		: The handle to be associated with the Id.

 @Output	pui32Id		: A pointer used to return the allocated Id.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT OSAIDGEN_AllocId(
	IMG_HANDLE 					hIdGenHandle,
	IMG_HANDLE					hHandle,
	IMG_UINT32 *				pui32Id
);


/*!
******************************************************************************

 @Function				OSAIDGEN_FreeId
 
 @Description 
 
 This function is used to free an Id.

 NOTE: The client is responsible for providing thread/process safe locks on
 the context structure to maintain coherency.
 
 @Input		hIdGenHandle	: The context handle.

 @Input		ui32Id		: The Id.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT OSAIDGEN_FreeId(
	IMG_HANDLE 					hIdGenHandle,
	IMG_UINT32  				ui32Id
);


/*!
******************************************************************************

 @Function				OSAIDGEN_GetHandle
 
 @Description 
 
 This function is used to get the handle associated with an Id.

 NOTE: The client is responsible for providing thread/process safe locks on
 the context structure to maintain coherency.
 
 @Input		hIdGenHandle	: The context handle.

 @Input		ui32Id		: The Id.

 @Output	phHandle	: A pointer used to return the handle associated
						  with the Id.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
extern IMG_RESULT OSAIDGEN_GetHandle(
	IMG_HANDLE 					hIdGenHandle,
	IMG_UINT32  				ui32Id,
	IMG_HANDLE *				phHandle
);

#if defined(__cplusplus)
}
#endif
 
#endif /* __IDGEN_API_H__	*/


