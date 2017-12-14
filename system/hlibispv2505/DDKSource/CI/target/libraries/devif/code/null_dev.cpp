/*!
******************************************************************************
 @file   : null_dev.cpp

 @brief

 @Author Tom Hollis

 @date   19/11/2008

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
         Inactive Device interface functions for the wrapper.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <stdio.h>
#include <map>
#include "devif_config.h"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

using namespace std;

typedef struct DEVIF_NULL_MallocInfo_tag
{
	IMG_UINT64 ui64StartAddress;
	IMG_UINT64 ui64Size;
	IMG_UINT32 * paAllocatedSpace;
	struct DEVIF_NULL_MallocInfo_tag * psNext;
} DEVIF_NULL_MallocInfo;


typedef struct
{
	map<IMG_UINT64, DEVIF_NULL_MallocInfo*> PageMap;
	map<IMG_UINT64, IMG_UINT64> RegisterMap;
}DEVIF_NULL_DeviceInfo;


/*!
******************************************************************************

 @Function				DEVIF_NULL_Initialise

******************************************************************************/
static
IMG_RESULT DEVIF_NULL_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
    (void)psDeviceCB; // unused
	//printf("DEVIF: Initialise %s\n", (IMG_CHAR*)psDeviceCB->pvDevIfInfo);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_ReadRegister

******************************************************************************/
static
IMG_UINT32 DEVIF_NULL_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DEVIF_NULL_DeviceInfo * psDeviceInfo = (DEVIF_NULL_DeviceInfo *)psDeviceCB->pvDevIfInfo;
	IMG_UINT32 ui32Value;


	ui32Value = (IMG_UINT32) psDeviceInfo->RegisterMap[ui64RegAddress]; 
	return ui32Value;
}



/*!
******************************************************************************

 @Function				DEVIF_NULL_WriteRegister

******************************************************************************/
static
IMG_VOID DEVIF_NULL_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_NULL_DeviceInfo * psDeviceInfo = (DEVIF_NULL_DeviceInfo *)psDeviceCB->pvDevIfInfo;


	psDeviceInfo->RegisterMap[ui64RegAddress] = ui32Value; 
}


/*!
******************************************************************************

 @Function				DEVIF_NULL_MallocMemory

******************************************************************************/
static
IMG_VOID DEVIF_NULL_MallocMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
    (void)psDeviceCB; (void)ui64DevAddress; (void)ui32Size; // unused
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_MallocHostVirt

******************************************************************************/
static
IMG_UINT64 DEVIF_NULL_MallocHostVirt (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64		ui64Alignment,
    	IMG_UINT64          ui64Size
)
{
    (void)psDeviceCB; (void)ui64Alignment; (void)ui64Size; // unused
	return 0;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_FreeMemory

******************************************************************************/
IMG_VOID DEVIF_NULL_FreeMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
    (void)psDeviceCB; (void)ui64DevAddress; // unused
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_Idle

******************************************************************************/
static
IMG_VOID DEVIF_NULL_Idle (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumCycles
)
{
    (void)psDeviceCB; (void)ui32NumCycles; // unused
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_CopyDeviceToHost

******************************************************************************/
static
IMG_VOID DEVIF_NULL_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
    // unused
    (void)psDeviceCB; (void)ui64DevAddress;
    (void)pvHostAddress; (void)ui32Size;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_CopyHostToDevice

******************************************************************************/
IMG_VOID DEVIF_NULL_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
    // unused
    (void)psDeviceCB; (void)ui64DevAddress;
    (void)pvHostAddress; (void)ui32Size;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_Deinitialise

******************************************************************************/
static
IMG_VOID DEVIF_NULL_Deinitialise (
	DEVIF_sDeviceCB *   psDeviceCB
	)
{
	DEVIF_NULL_DeviceInfo * psDeviceInfo = (DEVIF_NULL_DeviceInfo *)psDeviceCB->pvDevIfInfo;
	if (psDeviceCB->pvDevIfInfo)
		delete psDeviceInfo;
	psDeviceCB->pvDevIfInfo = IMG_NULL;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_RegWriteWords

******************************************************************************/
static
IMG_VOID DEVIF_NULL_RegWriteWords (
	DEVIF_sDeviceCB		* psDeviceCB,
	IMG_VOID			* pui32MemSource,
	IMG_UINT64			ui64RegAddress,
	IMG_UINT32    		ui32WordCount
)
{
    // unused
    (void)psDeviceCB; (void)pui32MemSource;
    (void)ui64RegAddress; (void)ui32WordCount;
}

/*!
******************************************************************************

 @Function				DEVIF_NULL_Setup

******************************************************************************/
IMG_VOID DEVIF_NULL_Setup ( DEVIF_sDeviceCB *   psDeviceCB )
{
	DEVIF_NULL_DeviceInfo * psDeviceInfo = new DEVIF_NULL_DeviceInfo;
	/*
	 * Read and write memory don't use device memory. Setting them to NULL reads/ writes from host's memory instead
	 */
	psDeviceCB->pfnDeinitailise			= DEVIF_NULL_Deinitialise;
	psDeviceCB->pfnInitailise			= DEVIF_NULL_Initialise;
	psDeviceCB->pfnCopyDeviceToHost		= DEVIF_NULL_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice		= DEVIF_NULL_CopyHostToDevice;
	psDeviceCB->pfnFreeMemory			= DEVIF_NULL_FreeMemory;
	psDeviceCB->pfnIdle					= DEVIF_NULL_Idle;
	psDeviceCB->pfnMallocMemory			= DEVIF_NULL_MallocMemory;
	psDeviceCB->pfnMallocHostVirt		= DEVIF_NULL_MallocHostVirt;
	psDeviceCB->pfnReadMemory			= NULL; // does not need this function
	psDeviceCB->pfnReadRegister			= DEVIF_NULL_ReadRegister;
	psDeviceCB->pfnRegWriteWords		= DEVIF_NULL_RegWriteWords;
	psDeviceCB->pfnWriteMemory			= NULL; // does not need this function
	psDeviceCB->pfnWriteRegister		= DEVIF_NULL_WriteRegister;
	psDeviceCB->pvDevIfInfo				= psDeviceInfo;

	psDeviceCB->bInitialised = IMG_TRUE;
}

