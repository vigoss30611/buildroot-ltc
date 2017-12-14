/*! 
******************************************************************************
 @file   : target_light.c

 @brief  

 @Author Imagination Technologies

 @date   03/11/2011
 
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

#include "tal.h"
#include "target.h" // to declare the target functions

#include <img_defs.h>
#include <img_errors.h>

static TAL_sMemSpaceInfo *gpsMemSpaceInfo = NULL; // use to set up config from structure - pointer to the start of an array of TAL_sMemSpaceInfo
static IMG_SIZE gUiNMemspaceInfo = 0; // number of mem space info in gpsMemSpaceInfo
static TAL_sDeviceInfo gsDeviceInfo;

// TAL Light does not support config files
void TARGET_SetConfigFile(const IMG_CHAR *pszFilePath)
{
  (void)pszFilePath;
}

void TARGET_SetMemSpaceInfo(TAL_sMemSpaceInfo *pStartOfArray, IMG_SIZE uiNElem, void *pDevInfo)
{
  gpsMemSpaceInfo = pStartOfArray;
  gUiNMemspaceInfo = uiNElem;
  gsDeviceInfo = *(TAL_sDeviceInfo*)pDevInfo; // copy
}

IMG_RESULT TARGET_Initialise (TAL_pfnTAL_MemSpaceRegister		pfnTAL_MemSpaceRegister)
{
   IMG_UINT32 i;
   IMG_RESULT result = IMG_SUCCESS;

   for (i = 0; i < gUiNMemspaceInfo && result == IMG_SUCCESS; i++)
   {
		// add the device offset
	    if ( gpsMemSpaceInfo[i].eMemSpaceType == TAL_MEMSPACE_REGISTER )
		{
			gpsMemSpaceInfo[i].sRegInfo.ui64RegBaseAddr += gsDeviceInfo.ui64DeviceBaseAddr;
		}
		else // TAL_MEMSPACE_MEMORY
		{
			gpsMemSpaceInfo[i].sMemInfo.ui64MemBaseAddr += gsDeviceInfo.ui64MemBaseAddr;
		}
		result = pfnTAL_MemSpaceRegister(&gpsMemSpaceInfo[i]);
   }

   return result;
}

void TARGET_Deinitialise (void)
{
  gpsMemSpaceInfo = NULL;
  gUiNMemspaceInfo = 0;
}

/*void TCONF_OverrideWrapper (void)
{
	// Shouldn't really be in here, but currently no better place for it, will be replaced with further target code development
}*/


