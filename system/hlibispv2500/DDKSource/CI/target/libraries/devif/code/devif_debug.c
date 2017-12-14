/*!
******************************************************************************
 @file   : devif.c

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
         Device interface functions for the wrapper.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <stdio.h>
#include "devif_api.h"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#define DEVIF_DEBUG_FILENAME "devif_debug.log"
#define SHOW_MALLOC_FREE

static DEVIF_sDeviceCB	gsOriginalDeviceCB;
static FILE *			gfDebugFile = IMG_NULL;

/*!
******************************************************************************

 @Function				DEVIF_DEBUG_Initalise

******************************************************************************/
IMG_RESULT DEVIF_DEBUG_Initailise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	if (gfDebugFile == IMG_NULL)
		gfDebugFile = fopen( DEVIF_DEBUG_FILENAME, "w" );
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	if (gsOriginalDeviceCB.pfnInitailise)
		gsOriginalDeviceCB.pfnInitailise(psDeviceCB);
	return IMG_SUCCESS;
}

/*!
******************************************************************************
s
 @Function				DEVIF_DEBUG_ReadRegister

******************************************************************************/
IMG_UINT32 DEVIF_DEBUG_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	IMG_UINT32 ui32Value;
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	ui32Value =  gsOriginalDeviceCB.pfnReadRegister(psDeviceCB, ui64RegAddress);
	fprintf(gfDebugFile, "RDW :REG:0x%" IMG_I64PR "X = 0x%X\n", ui64RegAddress, ui32Value);
	fflush(gfDebugFile);
	return ui32Value;
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_WriteRegister

******************************************************************************/
IMG_VOID DEVIF_DEBUG_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "WRW :REG:0x%" IMG_I64PR "X 0x%X\n", ui64RegAddress, ui32Value);
	fflush(gfDebugFile);
	gsOriginalDeviceCB.pfnWriteRegister(psDeviceCB, ui64RegAddress, ui32Value);
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_ReadMemory

******************************************************************************/
IMG_UINT32 DEVIF_DEBUG_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_UINT32 ui32Value;
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	ui32Value = gsOriginalDeviceCB.pfnReadMemory(psDeviceCB, ui64DevAddress);
	fprintf(gfDebugFile, "RDW :MEM:0x%" IMG_I64PR "X = 0x%X\n", ui64DevAddress, ui32Value);
	fflush(gfDebugFile);
	return ui32Value;
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_WriteMemory

******************************************************************************/
IMG_VOID DEVIF_DEBUG_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "WRW :MEM:0x%" IMG_I64PR "X 0x%X\n", ui64DevAddress, ui32Value);
	fflush(gfDebugFile);
	gsOriginalDeviceCB.pfnWriteMemory(psDeviceCB, ui64DevAddress, ui32Value);
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_MallocMemory

******************************************************************************/
IMG_VOID DEVIF_DEBUG_MallocMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	IMG_ASSERT(gfDebugFile != IMG_NULL);
#ifdef SHOW_MALLOC_FREE
	fprintf(gfDebugFile, "MALLOC :MEM:0x%" IMG_I64PR "X 0x%X\n", ui64DevAddress, ui32Size);
	fflush(gfDebugFile);
#endif
	gsOriginalDeviceCB.pfnMallocMemory(psDeviceCB, ui64DevAddress, ui32Size);
}




/*!
******************************************************************************

 @Function				DEVIF_DEBUG_FreeMemory

******************************************************************************/
IMG_VOID DEVIF_DEBUG_FreeMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_ASSERT(gfDebugFile != IMG_NULL);
#ifdef SHOW_MALLOC_FREE
	fprintf(gfDebugFile, "FREE :MEM:0x%" IMG_I64PR "X\n", ui64DevAddress);
	fflush(gfDebugFile);
#endif
	gsOriginalDeviceCB.pfnFreeMemory(psDeviceCB, ui64DevAddress);
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_Idle

******************************************************************************/
IMG_VOID DEVIF_DEBUG_Idle (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumCycles
)
{
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "Idle %d\n", ui32NumCycles);
	fflush(gfDebugFile);
	gsOriginalDeviceCB.pfnIdle(psDeviceCB, ui32NumCycles);
}

/*!
******************************************************************************

 @Function				DEVIF_DEBUG_CopyDeviceToHost

******************************************************************************/
IMG_VOID DEVIF_DEBUG_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	IMG_UINT32 ui32;
	IMG_UINT8 * ui8HostData = (IMG_UINT8*)pvHostAddress;
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "SAB :MEM:0x%" IMG_I64PR "X 0x%X\n", ui64DevAddress, ui32Size);
	gsOriginalDeviceCB.pfnCopyDeviceToHost(psDeviceCB, ui64DevAddress, pvHostAddress, ui32Size);

	for (ui32 = 0; ui32 < ui32Size; ui32++)
	{
		fprintf(gfDebugFile, "%02X", *ui8HostData);
		ui8HostData ++;
		if (ui32 % 4 == 3)
			fprintf(gfDebugFile, " ");
		if (ui32 % 16 == 15)
			fprintf(gfDebugFile, "\n");
	}
	if (ui32 % 16 != 15)
		fprintf(gfDebugFile, "\n");
	fflush(gfDebugFile);
}



/*!
******************************************************************************

 @Function				DEVIF_DEBUG_CopyHostToDevice

******************************************************************************/
IMG_VOID DEVIF_DEBUG_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	IMG_UINT32 ui32;
	IMG_UINT8 * ui8HostData = (IMG_UINT8*)pvHostAddress;
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "LDB :MEM:0x%" IMG_I64PR "X 0x%X\n", ui64DevAddress, ui32Size);
	for (ui32 = 0; ui32 < ui32Size; ui32++)
	{
		fprintf(gfDebugFile, "%02X", *ui8HostData);
		ui8HostData ++;
		if ((ui32 & 3) == 3)
			fprintf(gfDebugFile, " ");
		if ((ui32 & 0xF) == 0xF)
			fprintf(gfDebugFile, "\n");
	}
	if ((ui32 & 0xF) != 0xF)
		fprintf(gfDebugFile, "\n");
	fflush(gfDebugFile);
	gsOriginalDeviceCB.pfnCopyHostToDevice(psDeviceCB, pvHostAddress, ui64DevAddress, ui32Size);
}


/*!
******************************************************************************

 @Function				DEVIF_DEBUG_Deinitailise

******************************************************************************/
IMG_VOID DEVIF_DEBUG_Deinitailise (
	DEVIF_sDeviceCB *   psDeviceCB
	)
{
	if(gfDebugFile == IMG_NULL) return;
	fclose(gfDebugFile);
	gfDebugFile = IMG_NULL;
	if (gsOriginalDeviceCB.pfnDeinitailise)
		gsOriginalDeviceCB.pfnDeinitailise(psDeviceCB);
}

/*!
******************************************************************************

 @Function				DEVIF_DEBUG_RegWriteWords

******************************************************************************/
IMG_VOID DEVIF_DEBUG_RegWriteWords (
	DEVIF_sDeviceCB		* psDeviceCB,
	IMG_VOID			* pui32MemSource,
	IMG_UINT64			ui64RegAddress,
	IMG_UINT32    		ui32WordCount
)
{
	IMG_UINT32 ui32;
	IMG_UINT32 * pui32HostData = (IMG_UINT32*)pui32MemSource;
	IMG_ASSERT(gfDebugFile != IMG_NULL);

	fprintf(gfDebugFile, "LDW :REG:0x%" IMG_I64PR "X 0x%X\n", ui64RegAddress, ui32WordCount);
	for (ui32 = 0; ui32 < ui32WordCount; ui32++)
	{
		fprintf(gfDebugFile, "%08X ", *pui32HostData);
		pui32HostData ++;
		if (ui32 % 4 == 3)
			fprintf(gfDebugFile, "\n");
	}
	if (ui32 % 4 != 3)
		fprintf(gfDebugFile, "\n");
	fflush(gfDebugFile);
	gsOriginalDeviceCB.pfnRegWriteWords(psDeviceCB, pui32MemSource, ui64RegAddress, ui32WordCount);

}


/*!
******************************************************************************

 @Function				DEVIF_AddDebugOutput

******************************************************************************/
IMG_VOID DEVIF_AddDebugOutput ( DEVIF_sDeviceCB *   psDeviceCB )
{
    gsOriginalDeviceCB = *psDeviceCB;
	psDeviceCB->bInitialised			= gsOriginalDeviceCB.bInitialised;
	psDeviceCB->pvDevIfInfo				= gsOriginalDeviceCB.pvDevIfInfo;
	psDeviceCB->pfnSendComment			= gsOriginalDeviceCB.pfnSendComment;
	psDeviceCB->pfnDevicePrint			= gsOriginalDeviceCB.pfnDevicePrint;
	psDeviceCB->pfnGetDeviceTime		= gsOriginalDeviceCB.pfnGetDeviceTime;
	psDeviceCB->pfnGetDeviceMapping		= gsOriginalDeviceCB.pfnGetDeviceMapping;
	psDeviceCB->pfnAutoIdle				= gsOriginalDeviceCB.pfnAutoIdle;
	psDeviceCB->pfnDeinitailise			= DEVIF_DEBUG_Deinitailise;
	psDeviceCB->pfnInitailise			= DEVIF_DEBUG_Initailise;

	if (gsOriginalDeviceCB.pfnCopyDeviceToHost)
		psDeviceCB->pfnCopyDeviceToHost		= DEVIF_DEBUG_CopyDeviceToHost;
	if (gsOriginalDeviceCB.pfnCopyHostToDevice)
		psDeviceCB->pfnCopyHostToDevice		= DEVIF_DEBUG_CopyHostToDevice;
	if (gsOriginalDeviceCB.pfnFreeMemory)
		psDeviceCB->pfnFreeMemory			= DEVIF_DEBUG_FreeMemory;
	if (gsOriginalDeviceCB.pfnIdle)
		psDeviceCB->pfnIdle					= DEVIF_DEBUG_Idle;
	if (gsOriginalDeviceCB.pfnMallocMemory)
		psDeviceCB->pfnMallocMemory			= DEVIF_DEBUG_MallocMemory;
	if (gsOriginalDeviceCB.pfnReadMemory)
		psDeviceCB->pfnReadMemory			= DEVIF_DEBUG_ReadMemory;
	if (gsOriginalDeviceCB.pfnReadRegister)
		psDeviceCB->pfnReadRegister			= DEVIF_DEBUG_ReadRegister;
	if (gsOriginalDeviceCB.pfnRegWriteWords)
		psDeviceCB->pfnRegWriteWords		= DEVIF_DEBUG_RegWriteWords;
	if (gsOriginalDeviceCB.pfnWriteMemory)
		psDeviceCB->pfnWriteMemory			= DEVIF_DEBUG_WriteMemory;
	if (gsOriginalDeviceCB.pfnWriteRegister)
		psDeviceCB->pfnWriteRegister		= DEVIF_DEBUG_WriteRegister;
}

