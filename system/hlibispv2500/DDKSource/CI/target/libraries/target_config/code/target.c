/*! 
******************************************************************************
 @file   : target.c

 @brief  

 @Author Ray Livesley

 @date   05/01/2005
 
         <b>Copyright 2005 by Imagination Technologies Limited.</b>\n
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
         Dummy Target for PDMUP testing.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#include <img_errors.h>

#include "tal.h"
#include "target.h"
#include "tconf.h"
#if defined (TAL_PORT_FWRK)
#include "pftal_api.h"
#endif

char TARGET_Name[] = "Configurable Wrapper";

static IMG_CHAR *pszConfigFile;  // Default is the hard-coded string.
static IMG_HANDLE ghTconfSetup = IMG_NULL;

static TAL_sDeviceInfo gsDeviceInfo; // not static because used in tal.c for normal tal if loading from structure
static TAL_sMemSpaceInfo *gpsMemSpaceInfo = NULL; // use to set up config from structure - pointer to the start of an array of TAL_sMemSpaceInfo
static IMG_SIZE gUiNMemspaceInfo = 0; // number of mem space info in gpsMemSpaceInfo

#if defined (TAL_PORT_FWRK)
static IMG_BOOL gbConfigLoaded = IMG_FALSE;		//<! IMG_TRUE when config loaded
#endif

/*!
******************************************************************************

 @Function              TARGET_SetConfigFile

******************************************************************************/
IMG_VOID TARGET_SetConfigFile(const IMG_CHAR *pszFilePath)
{
    pszConfigFile = (IMG_CHAR *) pszFilePath;  // Override the default with the given file.
}

void TARGET_SetMemSpaceInfo(TAL_sMemSpaceInfo *pStartOfArray, IMG_SIZE uiNElem, void *pDevInfo)
{
	gpsMemSpaceInfo = pStartOfArray;
	gUiNMemspaceInfo = uiNElem;
	gsDeviceInfo = *(TAL_sDeviceInfo*)pDevInfo; // copy
}

#if defined (TAL_PORT_FWRK)
/*!
******************************************************************************

 @Function				target_fnTAL_MemSpaceRegister

******************************************************************************/
IMG_RESULT target_fnTAL_MemSpaceRegister (
	TAL_psMemSpaceInfo          psMemSpaceInfo
)
{
	IMG_UINT32					ui32Result;
	IMG_UINT64					ui64BaseOffset;
	IMG_UINT64					ui64Size;
	IMG_UINT32					ui32IntNum = 0;

	/* Pick out teh base and size. */
	switch (psMemSpaceInfo->eMemSpaceType)
	{
	case TAL_MEMSPACE_REGISTER:
		ui64BaseOffset	= psMemSpaceInfo->sRegInfo.ui64RegBaseAddr;
		ui64Size		= psMemSpaceInfo->sRegInfo.ui32RegSize;
		ui32IntNum		= psMemSpaceInfo->sRegInfo.ui32intnum;
		break;

	case TAL_MEMSPACE_MEMORY:
		ui64BaseOffset	= psMemSpaceInfo->sMemInfo.ui64MemBaseAddr;
		ui64Size		= psMemSpaceInfo->sMemInfo.ui64MemSize;
		break;

	default:
		IMG_ASSERT(0);
	}

	/* Register with the PF TAL. */
	ui32Result = PFTAL_MemSpaceRegister(psMemSpaceInfo->psDevInfo->pszDeviceName,
											strlen(psMemSpaceInfo->psDevInfo->pszDeviceName),
											psMemSpaceInfo->psDevInfo->ui32DevFlags,
											psMemSpaceInfo->pszMemSpaceName,
											strlen(psMemSpaceInfo->pszMemSpaceName),
											psMemSpaceInfo->eMemSpaceType,
											ui64BaseOffset,
											ui64Size,
											ui32IntNum);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		return ui32Result;
	}

	return IMG_SUCCESS;
}
#endif


/*!
******************************************************************************

 @Function				TARGET_LoadConfig

******************************************************************************/

IMG_RESULT TARGET_LoadConfig(IMG_VOID)
{
#if defined (TAL_PORT_FWRK)
	IMG_UINT32			ui32Result;

	/* If config has been loaded...*/
	if (gbConfigLoaded)
	{
		/* Nothing to do. */
		return IMG_SUCCESS;
	}

	gbConfigLoaded = IMG_TRUE;

	ui32Result = TCONF_TargetConfigure (pszConfigFile, target_fnTAL_MemSpaceRegister);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		return ui32Result;
	}

	/* We can no deinitilise the target config as we have finished with it. */
	TCONF_Deinitialise();

#endif
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TARGET_Initialise

 ******************************************************************************/
IMG_RESULT TARGET_Initialise (
	TAL_pfnTAL_MemSpaceRegister		pfnTAL_MemSpaceRegister
)
{
	IMG_RESULT ret = IMG_ERROR_FATAL;
	if ( pszConfigFile != NULL )
	{
		// load the config file
		ret = TCONF_TargetConfigure (pszConfigFile, pfnTAL_MemSpaceRegister, &ghTconfSetup);
	}
	else if ( gpsMemSpaceInfo != NULL  )
	{
		IMG_UINT32 i;
		ret = IMG_SUCCESS;
  
	   for (i = 0; i < gUiNMemspaceInfo && ret == IMG_SUCCESS; i++)
	   {
		   gpsMemSpaceInfo[i].psDevInfo = &gsDeviceInfo; // copy device information
		   ret = pfnTAL_MemSpaceRegister(&gpsMemSpaceInfo[i]);
	   }
	}
	return ret;
}

/*!
******************************************************************************

 @Function				TARGET_Deinitialise

 ******************************************************************************/
IMG_VOID TARGET_Deinitialise (IMG_VOID)
{
#if !defined (TAL_PORT_FWRK)
	if (ghTconfSetup != NULL)
	{
		TCONF_Deinitialise(ghTconfSetup);
		ghTconfSetup = NULL;
	}
#endif
	gpsMemSpaceInfo = NULL;
	gUiNMemspaceInfo = 0;
	pszConfigFile = NULL;
}
