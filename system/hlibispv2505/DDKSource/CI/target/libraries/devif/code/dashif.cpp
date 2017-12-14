/*!
******************************************************************************
 @file   : DASHIF.cpp

 @brief

 @Author Tom Hollis

 @date   11/12/2007

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
         pdump1 interface for the wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <devif_config.h>
//#include "DialConsole.h"
#include "img_defs.h"
#include "img_errors.h"


// setting this define before including DAtinyscript.h makes all the fn decls typedefs so we 
// can cast the fn types easily
#define IMG_DLL_TYPEDEF  
#include "DAtinyscript.h"


//static DIAL_ID conn;
#ifdef WIN32
#define LOAD_LIBRARY(libname) ::LoadLibraryA(libname ".dll")
#define LOAD_SYMBOL(lib, symname) ::GetProcAddress(lib, TEXT(symname))
static HMODULE lib;
#else
static void* lib;
#define LOAD_LIBRARY(libname) ::dlopen("lib" libname ".so", RTLD_LAZY)
#define LOAD_SYMBOL(lib, symname) ::dlsym(lib, symname)
#endif
static DAtiny_connection conn;

#define DATINY_DECLARE_FN(lib, name) static DAtiny_##name* conn##name

#define DATINY_LOAD_FN(lib, name)										\
	conn##name = (DAtiny_##name*) LOAD_SYMBOL(lib, "DAtiny_" #name);	\
	if (!conn##name)													\
	{																	\
		printf("Cannot Get DAtiny_" #name " Function\n");				\
        IMG_ASSERT(IMG_FALSE);											\
		return IMG_FALSE;												\
	}																	\
/**/

DATINY_DECLARE_FN(lib, Create);
DATINY_DECLARE_FN(lib, UseTarget);
DATINY_DECLARE_FN(lib, GetLastError);
DATINY_DECLARE_FN(lib, ReadMemory);
DATINY_DECLARE_FN(lib, WriteMemory);
DATINY_DECLARE_FN(lib, ReadMemoryBlock);
DATINY_DECLARE_FN(lib, WriteMemoryBlock);
DATINY_DECLARE_FN(lib, Delete);
DATINY_DECLARE_FN(lib, GetTargetFamily);


/*!
******************************************************************************

 @Function				DASHIF_Initailise

******************************************************************************/
IMG_RESULT DASHIF_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */
	
	if (!psDeviceCB->bInitialised)
	{
		lib = LOAD_LIBRARY("DAtinyscript");
		if (!lib)
		{
			printf("Cannot Find DAtinyscript, is codescape installed?\n");
			IMG_ASSERT(IMG_FALSE);
            return IMG_FALSE;
		}
        DATINY_LOAD_FN(lib, Create)
        DATINY_LOAD_FN(lib, UseTarget)
        DATINY_LOAD_FN(lib, GetLastError)
        DATINY_LOAD_FN(lib, ReadMemory)
        DATINY_LOAD_FN(lib, WriteMemory)
        DATINY_LOAD_FN(lib, ReadMemoryBlock)
        DATINY_LOAD_FN(lib, WriteMemoryBlock)
        DATINY_LOAD_FN(lib, Delete)
        DATINY_LOAD_FN(lib, GetTargetFamily)

		conn = connCreate();
		connUseTarget(conn, pDeviceInfo->pcDashName);
		if (const char * err = connGetLastError(conn))
		{
			printf("Cannot Connect to %s, Error Given: %s\n", pDeviceInfo->pcDashName, err);
			IMG_ASSERT(IMG_FALSE);
			return IMG_FALSE;
		}
		
		int family = connGetTargetFamily(conn);
		if (const char * err = connGetLastError(conn))
		{
			printf("Failed to read target family. Error Given: %s\n", err);
			IMG_ASSERT(IMG_FALSE);
		}
		else if (family != DAtiny_Meta)
		{
			printf("Expected target family of DAtiny_Meta(%d) but got %d.\n", DAtiny_Meta, family);
		}
		psDeviceCB->bInitialised = IMG_TRUE;
    }
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				DASHIF_Deinitailise

******************************************************************************/
IMG_VOID DASHIF_Deinitailise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	if (psDeviceCB->bInitialised == IMG_TRUE)
	{
		connDelete(conn);
		conn = 0;
#ifdef WIN32
		::FreeLibrary(lib);
#else
		dlclose(lib);
#endif
		psDeviceCB->bInitialised = IMG_FALSE;
	}
}





/*!
******************************************************************************

 @Function				DASHIF_ReadRegister

******************************************************************************/
IMG_UINT32 DASHIF_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */
	IMG_UINT32 ui32ReadValue = 0;

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64RegAddress & 0x3) == 0);
	IMG_ASSERT(ui64RegAddress < pDeviceInfo->ui32DeviceSize);

	ui32ReadValue = (IMG_UINT32) connReadMemory(conn, pDeviceInfo->ui32DeviceBase | (IMG_UINT32)ui64RegAddress, sizeof(IMG_UINT32), 0);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to read memory from 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | (IMG_UINT32)ui64RegAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
	printf("Read Register %08X, Got %08X\n", (IMG_UINT32)ui64RegAddress, ui32ReadValue);
	return ui32ReadValue;
}

/*!
******************************************************************************

 @Function				DASHIF_WriteRegister

******************************************************************************/
IMG_VOID DASHIF_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64RegAddress & 0x3) == 0);
	IMG_ASSERT(ui64RegAddress < pDeviceInfo->ui32DeviceSize);

	connWriteMemory(conn, pDeviceInfo->ui32DeviceBase | (IMG_UINT32)ui64RegAddress, ui32Value, sizeof(IMG_UINT32), 0);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to write to memory at 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | (IMG_UINT32)ui64RegAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function				DASHIF_ReadMemory

******************************************************************************/
IMG_UINT32 DASHIF_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_UINT32			ui32DevAddress = IMG_UINT64_TO_UINT32(ui64DevAddress);
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */
	IMG_UINT32			ui32ReadValue = 0;

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui32DevAddress & 0x3) == 0);
	IMG_ASSERT(ui32DevAddress < pDeviceInfo->ui32MemorySize);

	ui32ReadValue = connReadMemory(conn, pDeviceInfo->ui32MemoryBase | ui32DevAddress, sizeof(IMG_UINT32), 0);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to read memory from 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | ui32DevAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
	return ui32ReadValue;
}

/*!
******************************************************************************

 @Function				DASHIF_ReadMemory64

******************************************************************************/

IMG_UINT64 DASHIF_ReadMemory64 (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_UINT32			ui32DevAddress = IMG_UINT64_TO_UINT32(ui64DevAddress);
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */
	IMG_UINT32			ui32ReadValues[2] = {0, 0};
	IMG_UINT64			ui64ReadValue;

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui32DevAddress & 0x3) == 0);
	IMG_ASSERT(ui32DevAddress < pDeviceInfo->ui32MemorySize);

	connReadMemoryBlock(conn, pDeviceInfo->ui32MemoryBase | ui32DevAddress, 2, sizeof(IMG_UINT32), 0, ui32ReadValues);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to read 64-bit value from 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | ui32DevAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
	ui64ReadValue = (IMG_UINT64)ui32ReadValues[0] + (((IMG_UINT64)ui32ReadValues[0]) << 32);
	return ui64ReadValue;
}


/*!
******************************************************************************

 @Function				DASHIF_WriteMemory

******************************************************************************/
IMG_VOID DASHIF_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	IMG_UINT32		ui32DevAddress = IMG_UINT64_TO_UINT32(ui64DevAddress);
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui32DevAddress & 0x3) == 0);
	IMG_ASSERT(ui32DevAddress < pDeviceInfo->ui32MemorySize);

	connWriteMemory(conn, pDeviceInfo->ui32MemoryBase | ui32DevAddress, ui32Value, sizeof(IMG_UINT32), 2);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to write to memory at 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | ui32DevAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
}



/*!
******************************************************************************

 @Function				DASHIF_CopyDeviceToHost

******************************************************************************/
IMG_VOID DASHIF_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	IMG_UINT32			ui32DevAddress = IMG_UINT64_TO_UINT32(ui64DevAddress);
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT(ui32DevAddress < pDeviceInfo->ui32MemorySize);
	IMG_ASSERT((ui32DevAddress + ui32Size) < pDeviceInfo->ui32MemorySize);

	connReadMemoryBlock(conn, pDeviceInfo->ui32MemoryBase | ui32DevAddress, ui32Size, 1, 0, pvHostAddress);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to read memory block from 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | ui32DevAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function				DASHIF_CopyHostToDevice

******************************************************************************/
IMG_VOID DASHIF_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	IMG_UINT32			ui32DevAddress = IMG_UINT64_TO_UINT32(ui64DevAddress);
	DEVIF_sDashIFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sDashIFInfo;		/* Info on interface */
	
	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT(ui32DevAddress < pDeviceInfo->ui32MemorySize);
	IMG_ASSERT((ui32DevAddress + ui32Size) < pDeviceInfo->ui32MemorySize);

	connWriteMemoryBlock(conn, pDeviceInfo->ui32MemoryBase | ui32DevAddress, 
													ui32Size, 1, 0, pvHostAddress);
	if (const char * err = connGetLastError(conn))
	{
		printf("Failed to write memory block to 0x%08x. Error Given: %s\n", pDeviceInfo->ui32MemoryBase | ui32DevAddress, err);
		IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function				DASHIF_DevIfConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input     pszDevName      : Pointer to device name.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
IMG_VOID
DASHIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB)
{   

	psDeviceCB->bInitialised        = IMG_FALSE;
	psDeviceCB->pfnInitailise       = DASHIF_Initialise;
	psDeviceCB->pfnReadRegister     = DASHIF_ReadRegister;
	psDeviceCB->pfnWriteRegister    = DASHIF_WriteRegister;
	psDeviceCB->pfnReadMemory       = DASHIF_ReadMemory;
	psDeviceCB->pfnReadMemory64     = DASHIF_ReadMemory64;
	psDeviceCB->pfnWriteMemory      = DASHIF_WriteMemory;
	//psDeviceCB->pfnWriteMemory64    = DASHIF_WriteMemory64;
	psDeviceCB->pfnCopyDeviceToHost = DASHIF_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice = DASHIF_CopyHostToDevice;
    psDeviceCB->pfnDeinitailise     = DASHIF_Deinitailise;
}






