/*!
******************************************************************************
 @file   : tal_setup.h

 @brief	API for the Target Abstraction Layer Setup.

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
         Target Abstraction Layer (TAL) Setup Function Header File.

 @Platform
	     Platform Independent


******************************************************************************/
/*
******************************************************************************
 *****************************************************************************/

#if !defined (__TALSETUP_H__)
#define __TALSETUP_H__

#include "tal_defs.h"

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
 * @name Setup functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Setup group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TAL_pfnTAL_MemSpaceRegister

 @Description

 This is a prototype for the TAL_MemSpaceRegister function.

 @Input		psMemSpaceInfo	: Information about the new memory space.
 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an
								error code.

******************************************************************************/
typedef IMG_RESULT ( * TAL_pfnTAL_MemSpaceRegister) (
	TAL_psMemSpaceInfo          psMemSpaceInfo
);


/*!
******************************************************************************

 @Function				TALSETUP_Initialise

 @Description

 This function is used to initialise the TAL and should be called at start-up.
 Subsequent calls are ignored.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
IMG_RESULT TALSETUP_Initialise (IMG_VOID);


/*!
******************************************************************************

 @Function				TALSETUP_Deinitialise

 @Description

 This function is used to de-initialise the TAL.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
							error code.

******************************************************************************/
IMG_RESULT TALSETUP_Deinitialise (IMG_VOID);


/*!
******************************************************************************

 @Function				TALSETUP_Reset

 @Description

 This function is used to reset the TAL.  This frees any memory previously
 allocated.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
IMG_RESULT TALSETUP_Reset (IMG_VOID);


/*!
******************************************************************************

 @Function				TALSETUP_IsInitialised

 @Description

 This function is used to test if the TAL is initialised.

 @Return	IMG_BOOL	: This function returns either
						  IMG_TRUE if the TAL is initialised
                          or IMG_FALSE if not

******************************************************************************/
IMG_BOOL TALSETUP_IsInitialised (IMG_VOID);

/*!
******************************************************************************

 @Function				TALSETUP_MemSpaceRegister

 @Description

 This function is used to register a memory space with the TAL.

 @Input		psMemSpaceCB	: A to the memory space control block to be registered
								with the TAL (see the Wrapper APIs)
 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an
								error code.

******************************************************************************/
IMG_RESULT TALSETUP_MemSpaceRegister (
	TAL_psMemSpaceInfo			psMemSpaceCB
);

/*!
******************************************************************************

 @Function				TALSETUP_OverrideMemSpaceSetup

 @Description

 This function is used to override the MemSpace Setup

 @Input		pfnTAL_MemSpaceRegisterOverride	: A pointer to the MemSpaceRegister function

 @Return	None

******************************************************************************/
IMG_VOID TALSETUP_OverrideMemSpaceSetup (
	TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegisterOverride
);

/*!
******************************************************************************

 @Function				TALSETUP_RegisterPollFailFunc

 @Description

 This function is used to register a "poll" fail function - which will be called
 by the TAL if a POL fails - before the TAL "asserts".

 @Input		pfnPollFailFunc : A pointer to the wrapper de-initialise function.
 @Return	None.

******************************************************************************/
IMG_VOID TALSETUP_RegisterPollFailFunc (
	TAL_pfnPollFailFunc         pfnPollFailFunc
);

/*!
******************************************************************************

 @Function				TALSETUP_SetTargetApplicationFlags

 @Description

 This function is used to pass any application specific flags that are used by the target.
 The call should be made with the same configuration passed via the WRAP_SetApplicationFlags
 function.

 @Input	    ui32TargetAppFlags  : Required application flags.
 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
IMG_VOID TALSETUP_SetTargetApplicationFlags(
	IMG_UINT32					ui32TargetAppFlags
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Setup group
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

#endif //__TALSETUP_H__

