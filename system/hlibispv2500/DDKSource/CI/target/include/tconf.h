/*! 
******************************************************************************
 @file   : tconf.h

 @brief	 API for the target config file parser

 @Author Ray Livesley

 @date   17/10/2006
 
         <b>Copyright 2006 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         Functions used to read a Target Config File.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0

******************************************************************************/

#ifndef __TCONF_H__
#define __TCONF_H__


#include "target.h"
#include "devif_setup.h"
#include "img_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Target Configuration functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Configuration group
 *------------------------------------------------------------------------*/
#endif /* DOXYGEN_CREATE_MODULES */


/*!
******************************************************************************

 @Function				TCONF_TargetConfigure
 
 @Description 
 
 This function is used to setup the target configuration from a Target Config
 file.  Typically this function is called from with the TARGET_Initialise
 function.
 
 This function will register the target/system memory spaces
 with the TAL using the pointer to TAL_MemSpaceRegister() passed via
 pfnTAL_MemSpaceRegister.

 The function also configures the memory regions accessible via 
 Address Allocation APIs. see addr_alloc.h

 @Input		pszConfigFile   : A zero terminated string containing the path
                              and name of the config file.

 @Input		pfnTAL_MemSpaceRegister : A pointer to the TAL_MemSpaceRegister
						      function.
						      
 @Input		phTargetSetup : A pointer into which to return the target config handle

 @Return	Error Return code.

******************************************************************************/
extern IMG_RESULT TCONF_TargetConfigure(
	const IMG_CHAR *               pszConfigFile,
	TAL_pfnTAL_MemSpaceRegister		pfnTAL_MemSpaceRegister,
	IMG_HANDLE *					phTargetSetup
);


/*!
******************************************************************************

 @Function				TCONF_Deinitialise
 
 @Description 
 
 This function is used to deinitialise the configurable target - freeing memory
 structrues etc.
 
 @Input		hTargetSetup : The target config handle
 
 @Return	None.

******************************************************************************/
extern IMG_VOID TCONF_Deinitialise(IMG_HANDLE hTargetSetup);


/*!
******************************************************************************

 @Function				TCONF_SaveConfig
 
 @Description 
 
 This function is used to save a copy of the config file to a given folder.

 @Input		hTargetSetup : The target config handle
 
 @Input		pszFileName		: A zero terminated string containing the path
                              and name of the target file.

 @Return	IMG_RESULT		: Result.

******************************************************************************/
extern IMG_RESULT TCONF_SaveConfig(IMG_HANDLE hTargetSetup, 
								   IMG_CHAR * pszFileName);

/*!
******************************************************************************

 @Function				TCONF_OverrideWrapper

 @Description

 This function overrides the wrapper setup in the config file

 @Input		pcArgs		:	command line arguements

 @Return	IMG_RESULT	:	Reports result of processing override parameters

 Syntax:

 SocketIF: s \<device name> \<port> [ \<hostname> ]

 ******************************************************************************/
IMG_RESULT TCONF_OverrideWrapper(
    IMG_CHAR *					    pcArgs
);


#define tconf_OverrideWrapper TCONF_OverrideWrapper

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Configuration group
 *------------------------------------------------------------------------*/
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif /* DOXYGEN_CREATE_MODULES */

#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif /* __TCONF_H__	*/
