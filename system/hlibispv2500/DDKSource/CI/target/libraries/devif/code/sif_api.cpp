/******************************************************************************

 @File			sif_api.cpp

 @Author		Ray Livesley

 @date			22/08/2007

 @Title			The Hardware Intermediate Debug file processing.

 @Copyright		Copyright (C) 2007 - 2008 by Imagination Technologies Limited.

          <b>Copyright 2007 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.
         
 @Platform		Independent 

 @Description	Socket interface to device.

******************************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <devif_api.h>
#include <devif_config.h>
#include "device_interface.h"
#include "img_defs.h"
#include "img_errors.h"

#include "device_interface_master_tcp.h"

static void SIF_AutoIdle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
);

static void SIF_LoadRegister(
    DEVIF_sDeviceCB *   psDeviceCB,
    void *          	pvHostAddress,
    IMG_UINT64			ui64RegAddress,
    IMG_UINT32          ui32Size
);

static void SIF_SendComment(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pcComment
);

static void SIF_DevicePrint(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pszString
);

static IMG_UINT32 SIF_GetDeviceTime(	
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64 *		pui64Time
);

/*!
******************************************************************************

 @Function				SIF_Initialise

******************************************************************************/
static IMG_RESULT SIF_Initialise(	
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	if (!psDeviceCB->bInitialised) 
	{
		DeviceInterfaceMasterTCP* pDeviceInterface = NULL, *pNewDevice;
		DEVIF_sDeviceIpInfo *	psDeviceIpInfo = &psDeviceCB->sDevIfSetup.u.sDeviceIpInfo;
		DEVIF_sBaseNameInfo *	psBaseNameInfo = &psDeviceCB->sDevIfSetup.sBaseNameInfo;
		
		IMG_ASSERT(psDeviceIpInfo);
	
		// If it is a child device, ensure the parent is started and add the device
		if (psDeviceIpInfo->psParentDevInfo != IMG_NULL)
		{
			DEVIF_sDeviceIpInfo *	psParentDevInfo = (DEVIF_sDeviceIpInfo*)psDeviceIpInfo->psParentDevInfo;
			pDeviceInterface = (DeviceInterfaceMasterTCP*)DeviceInterface::find(psParentDevInfo->pszDeviceName);
			IMG_ASSERT(pDeviceInterface);
			pDeviceInterface->initialise();
			pNewDevice = (DeviceInterfaceMasterTCP*)pDeviceInterface->addNewDevice(psDeviceIpInfo->pszDeviceName, IMG_FALSE);
			DeviceInterface::addToSystem(*pNewDevice);
		}
		
		// Initialise this device
		pDeviceInterface = (DeviceInterfaceMasterTCP*)DeviceInterface::find(psDeviceIpInfo->pszDeviceName);
		IMG_ASSERT(pDeviceInterface);
		psDeviceCB->pvDevIfInfo = (IMG_VOID*)pDeviceInterface;
		
		pDeviceInterface->initialise();
		psDeviceCB->bInitialised = IMG_TRUE;
		if (pDeviceInterface->getRemoteProtocol() >= 4)
		{
			psDeviceCB->pfnDevicePrint	= SIF_DevicePrint;
		}
		if (pDeviceInterface->getRemoteProtocol() >= 5)
		{
			psDeviceCB->pfnAutoIdle	= SIF_AutoIdle;
		}
		//psDeviceCB->pfnRegWriteWords	= SIF_LoadRegister;
		if (pDeviceInterface->getRemoteProtocol() >= 2)
		{
			psDeviceCB->pfnSendComment		= SIF_SendComment;
		}
		if (pDeviceInterface->getRemoteProtocol() >= 3)
		{
			psDeviceCB->pfnGetDeviceTime	= SIF_GetDeviceTime;
		}
		if (psBaseNameInfo)
		{
			if (psBaseNameInfo->pszRegBaseName && psBaseNameInfo->pszMemBaseName)
			{
				pDeviceInterface->setBaseNames(psBaseNameInfo->pszRegBaseName, psBaseNameInfo->pszMemBaseName);
			}
			else if (psBaseNameInfo->pszRegBaseName)
			{
				pDeviceInterface->setBaseNames(psBaseNameInfo->pszRegBaseName, "");
			}
			else if (psBaseNameInfo->pszMemBaseName)
			{
				pDeviceInterface->setBaseNames("", psBaseNameInfo->pszMemBaseName);
			}
		}
	}

	return IMG_SUCCESS;
}

/*!
******************************************************************************

  @Function				SIF_FakeInitialise

******************************************************************************/
static IMG_RESULT SIF_FakeInitialise(	
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	(void)psDeviceCB;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				sif_GetDeviceInterface

******************************************************************************/
static DeviceInterface* sif_GetDeviceInterface (DEVIF_sDeviceCB *   psDeviceCB)
{
	DeviceInterface* pDeviceInterface = NULL;
	
	// Initialise the socket if necessary
	if(psDeviceCB->bInitialised == IMG_FALSE)	SIF_Initialise(psDeviceCB);
	pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	return pDeviceInterface;
}

/*!
******************************************************************************

 @Function				SIF_Deinitailise

******************************************************************************/
static void SIF_Deinitailise (	
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	if(psDeviceCB->bInitialised == IMG_TRUE)
	{
		DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	
		if(pDeviceInterface)
		{
			pDeviceInterface->deinitialise();
		}
		psDeviceCB->bInitialised = IMG_FALSE;
		delete pDeviceInterface;
		psDeviceCB->pvDevIfInfo = IMG_NULL;
	}
}

/*!
******************************************************************************

 @Function				SIF_ReadRegister

******************************************************************************/
static IMG_UINT32 SIF_ReadRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readRegister(ui64RegAddress);
}

/*!
******************************************************************************

 @Function				SIF_ReadRegister64

******************************************************************************/
static IMG_UINT64 SIF_ReadRegister64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readRegister64(ui64RegAddress);
}

/*!
******************************************************************************

 @Function				SIF_WriteRegister

******************************************************************************/
static void SIF_WriteRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeRegister(ui64RegAddress, ui32Value);
}

/*!
******************************************************************************

 @Function				SIF_WriteRegister64

******************************************************************************/
static void SIF_WriteRegister64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT64          ui64Value
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeRegister64(ui64RegAddress, ui64Value);
}

/*!
******************************************************************************

 @Function				SIF_ReadMemory

******************************************************************************/
static IMG_UINT32 SIF_ReadMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readMemory(ui64DevAddress);
}

/*!
******************************************************************************

 @Function				SIF_ReadMemory64

******************************************************************************/
static IMG_UINT64 SIF_ReadMemory64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readMemory64(ui64DevAddress);
}


 
/*!
******************************************************************************

 @Function				SIF_WriteMemory

******************************************************************************/
static void SIF_WriteMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeMemory(ui64DevAddress, ui32Value);
}

/*!
******************************************************************************

 @Function				SIF_WriteMemory64

******************************************************************************/
static void SIF_WriteMemory64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT64          ui64Value
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeMemory64(ui64DevAddress, ui64Value);
}
 
/*!
******************************************************************************

 @Function				SIF_CopyDeviceToHost

******************************************************************************/
static void SIF_CopyDeviceToHost(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    void *          	pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyDeviceToHost(ui64DevAddress, pvHostAddress, ui32Size);
}


 
/*!
******************************************************************************

 @Function				SIF_CopyHostToDevice

******************************************************************************/
static void SIF_CopyHostToDevice(	
	DEVIF_sDeviceCB *   psDeviceCB,
    void *          	pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyHostToDevice(pvHostAddress, ui64DevAddress, ui32Size);
}

/*!
******************************************************************************

 @Function				SIF_LoadRegister

******************************************************************************/
static void SIF_LoadRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    void *          	pvHostAddress,
    IMG_UINT64			ui64RegAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->loadRegister(pvHostAddress, ui64RegAddress, ui32Size);
}

/*!
******************************************************************************

 @Function				SIF_AllocMem

******************************************************************************/
static void SIF_AllocMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->allocMem(ui64Address, ui32Size);
}

/*!
******************************************************************************

 @Function				SIF_AllocHostVirt

******************************************************************************/
static IMG_UINT64 SIF_AllocHostVirt(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address,
    IMG_UINT64          ui64Size
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	return pDeviceInterface->allocMemOnSlave(ui64Address, IMG_UINT64_TO_UINT32(ui64Size));
}


/*!
******************************************************************************

 @Function				SIF_FreeMem

******************************************************************************/
static void SIF_FreeMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->freeMem(ui64Address);
}

/*!
******************************************************************************

 @Function				SIF_Idle

******************************************************************************/
static void SIF_Idle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->idle(ui32NumBytes);
}

/*!
******************************************************************************

 @Function				SIF_AutoIdle

******************************************************************************/
static void SIF_AutoIdle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->setAutoIdle(ui32NumBytes);
}

/*!
******************************************************************************

 @Function				SIF_SendComment

******************************************************************************/
static void SIF_SendComment(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pcComment
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->sendComment(pcComment);
}

/*!
******************************************************************************

 @Function				SIF_DevicePrint

******************************************************************************/
static void SIF_DevicePrint(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pszString
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->devicePrint(pszString);
}

/*!
******************************************************************************

 @Function				SIF_GetDeviceTime

******************************************************************************/
static IMG_UINT32 SIF_GetDeviceTime(	
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64 *		pui64Time
)
{
	DeviceInterface* pDeviceInterface = sif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	return pDeviceInterface->getDeviceTime(pui64Time);
}

/*!
******************************************************************************

 @Function				SIF_FakeDevicePrint

******************************************************************************/
static void SIF_FakeDevicePrint(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pszString
)
{
	// Initialise device and check to see if we need to run this command properly.
	sif_GetDeviceInterface(psDeviceCB);
	if (psDeviceCB->pfnDevicePrint == SIF_DevicePrint)
	{
		SIF_DevicePrint(psDeviceCB, pszString);
	}
}

/*!
******************************************************************************

 @Function				SIF_FakeAutoIdle

******************************************************************************/
static void SIF_FakeAutoIdle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
)
{
	// Initialise device and check to see if we need to run this command properly.
	sif_GetDeviceInterface(psDeviceCB);
	if (psDeviceCB->pfnAutoIdle == SIF_AutoIdle)
	{
		SIF_AutoIdle(psDeviceCB, ui32NumBytes);
	}
}
/*!
******************************************************************************

 @Function				SIF_FakeSendComment

******************************************************************************/
static void SIF_FakeSendComment(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pcComment
)
{
	// Initialise device and check to see if we need to run this command properly.
	sif_GetDeviceInterface(psDeviceCB);
	if (psDeviceCB->pfnSendComment == SIF_SendComment)
	{
		SIF_SendComment(psDeviceCB, pcComment);
	}
}
/*!
******************************************************************************

 @Function				SIF_FakeGetDeviceTime

******************************************************************************/
static IMG_UINT32 SIF_FakeGetDeviceTime(	
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64 *		pui64Time
)
{
	// Initialise device and check to see if we need to run this command properly.
	sif_GetDeviceInterface(psDeviceCB);
	if (psDeviceCB->pfnGetDeviceTime == SIF_GetDeviceTime)
	{
		return SIF_GetDeviceTime(psDeviceCB, pui64Time);
	}
	*pui64Time = 0;
	return IMG_ERROR_GENERIC_FAILURE;
}

/*!
******************************************************************************

 @Function				sif_SetupFunctions

******************************************************************************/
static IMG_VOID	sif_SetupFunctions(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	psDeviceCB->bInitialised        = IMG_FALSE;  
	psDeviceCB->pfnInitailise       = SIF_FakeInitialise;
	psDeviceCB->pfnReadRegister     = SIF_ReadRegister;
	psDeviceCB->pfnReadRegister64   = SIF_ReadRegister64;
	psDeviceCB->pfnWriteRegister    = SIF_WriteRegister;
	psDeviceCB->pfnWriteRegister64  = SIF_WriteRegister64;
	psDeviceCB->pfnRegWriteWords	= SIF_LoadRegister;
	psDeviceCB->pfnReadMemory       = SIF_ReadMemory;
	psDeviceCB->pfnReadMemory64     = SIF_ReadMemory64;
	psDeviceCB->pfnWriteMemory      = SIF_WriteMemory;
	psDeviceCB->pfnWriteMemory64    = SIF_WriteMemory64;
	psDeviceCB->pfnCopyDeviceToHost = SIF_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice = SIF_CopyHostToDevice;
	psDeviceCB->pfnMallocMemory		= SIF_AllocMem;
	psDeviceCB->pfnMallocHostVirt	= SIF_AllocHostVirt;
	psDeviceCB->pfnFreeMemory		= SIF_FreeMem;
	psDeviceCB->pfnIdle				= SIF_Idle;
	psDeviceCB->pfnDevicePrint		= SIF_FakeDevicePrint;
	psDeviceCB->pfnAutoIdle			= SIF_FakeAutoIdle;
	psDeviceCB->pfnSendComment		= SIF_FakeSendComment;
	psDeviceCB->pfnGetDeviceTime	= SIF_FakeGetDeviceTime;
  psDeviceCB->pfnDeinitailise     = SIF_Deinitailise;
}

/*!
******************************************************************************

 @Function				SIF_DevIfConfigureDevice

******************************************************************************/
IMG_VOID	SIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
  
	DEVIF_sDeviceIpInfo *	psDeviceIpInfo = &psDeviceCB->sDevIfSetup.u.sDeviceIpInfo;

	DeviceInterfaceMasterTCP * pDeviceInterfaceMasterTCP = IMG_NULL;

	IMG_CHAR    acPortId[100], *pszPortId;
	IMG_CHAR    acIpAddr[100], *pszRemoteMach;

	// Get port number or set string to false if there isn't one
	if (psDeviceIpInfo->ui32PortId == 0)
	{
		pszPortId = IMG_NULL;
	}
	else
	{
		sprintf(acPortId, "%d", psDeviceIpInfo->ui32PortId);
		pszPortId = acPortId; 
	}
            
	// Get remote name or Ip address
    if (psDeviceIpInfo->pszRemoteName != IMG_NULL)
	{
		pszRemoteMach = psDeviceIpInfo->pszRemoteName;
	}
	else if (
            (psDeviceIpInfo->ui32IpAddr1 == 0) &&
            (psDeviceIpInfo->ui32IpAddr2 == 0) &&
            (psDeviceIpInfo->ui32IpAddr3 == 0) &&
            (psDeviceIpInfo->ui32IpAddr4 == 0)
            )
    {
		pszRemoteMach = IMG_NULL; // No Remote Machine Defined
    }
    else
    {
        sprintf(acIpAddr, "%d.%d.%d.%d", psDeviceIpInfo->ui32IpAddr1, psDeviceIpInfo->ui32IpAddr2, psDeviceIpInfo->ui32IpAddr3, psDeviceIpInfo->ui32IpAddr4);
        pszRemoteMach = acIpAddr;
    }

    /* If we are not using another connection, make a new connection ...*/
    if (psDeviceIpInfo->psParentDevInfo == IMG_NULL)
    {
		IMG_ASSERT(pszPortId != IMG_NULL);
        pDeviceInterfaceMasterTCP = new DeviceInterfaceMasterTCP(psDeviceIpInfo->pszDeviceName, pszPortId, pszRemoteMach, (psDeviceIpInfo->bUseBuffer != IMG_FALSE));
        DeviceInterface::addToSystem(*pDeviceInterfaceMasterTCP);
	}
	else
	{
		IMG_ASSERT(pszPortId == IMG_NULL && pszRemoteMach == IMG_NULL);
	}
	sif_SetupFunctions(psDeviceCB);
}
