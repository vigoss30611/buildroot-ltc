/*!
******************************************************************************
 @file   : tal_internal.h

 @brief :	Internal functions required for the Target Abstraction Layer.

 @Author Tom Hollis

 @date   19/08/2010

         <b>Copyright 2010 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

@Description
         Target Abstraction Layer (TAL) Internal Header File.

 @Platform
	     Platform Independent


******************************************************************************/

#include "img_types.h"

#if !defined (__TAL_INTERNAL_H__)
#define __TAL_INTERNAL_H__
#if defined (__cplusplus)
extern "C" {
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Internal functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Internal group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALINTVAR_ExecCommand

******************************************************************************/
IMG_RESULT TALINTVAR_ExecCommand(
		IMG_UINT32						ui32CommandId,
		IMG_HANDLE						hDestRegMemSpace,
		IMG_UINT32						ui32DestRegId,
		IMG_HANDLE						hOpRegMemSpace,
		IMG_UINT32                      ui32OpRegId,
		IMG_HANDLE						hLastOpMemSpace,
		IMG_UINT64						ui64LastOperand,
		IMG_BOOL						bIsRegisterLastOperand
);

/*!
******************************************************************************

 @Function				TALREG_ReadWord

******************************************************************************/
IMG_RESULT TALREG_ReadWord(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_PVOID						pvValue,
    IMG_BOOL                        bVerify,
	IMG_BOOL						bTest,
	IMG_UINT32						ui32VerCount,
	IMG_UINT32						ui32VerTime,
    IMG_BOOL                        bCallback,
	IMG_UINT32						ui32Flags
);

/*!
******************************************************************************

 @Function				TALREG_WriteWord

******************************************************************************/
IMG_RESULT TALREG_WriteWord ( 
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace,
    IMG_BOOL                        bCallback,
    IMG_UINT32						ui32Flags
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Internal group
 *------------------------------------------------------------------------*/
 /**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#if defined (__cplusplus)
}
#endif

#endif //__TAL_INTERNAL_H__


