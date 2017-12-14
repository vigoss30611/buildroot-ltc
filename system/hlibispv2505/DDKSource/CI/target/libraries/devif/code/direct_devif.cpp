/*! 
******************************************************************************
 @file   : devif.c

 @brief  

 @Author Ray Livesley

 @date   15/05/2007
 
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

 <b>Description:</b>\n
         This file contains the System C implemenation of the DEVIF.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#include "devif_config.h"
#include "device_interface.h"
#include "img_defs.h"
#include "img_errors.h"

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_Initialise

******************************************************************************/
static IMG_RESULT DIRECT_DEVIF_Initialise(	
	DEVIF_sDeviceCB * psDeviceCB
)
{
	DEVIF_sDirectIFInfo *	psDirectIFInfo = &psDeviceCB->sDevIfSetup.u.sDirectIFInfo;
	DEVIF_sDirectIFInfo *	psParentIFInfo;
	DeviceInterface * pDeviceInterface, * pParentDevice = IMG_NULL;
	DEVIF_sBaseNameInfo *	psBaseNameInfo = &psDeviceCB->sDevIfSetup.sBaseNameInfo;

	if (psDeviceCB->bInitialised) return IMG_SUCCESS;


	pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
    if (pDeviceInterface)
    {
	    if (!psDeviceCB->bInitialised) 
        {
		    pDeviceInterface->initialise();
		    psDeviceCB->bInitialised = IMG_TRUE;
	    }
    }
    else
    {
    	// If the device wasn't found during the configure phase then a check is done to see if it can 
    	// be created through a parent device
    	if (psDirectIFInfo->psParentDevInfo)
    	{
    		psParentIFInfo = (DEVIF_sDirectIFInfo *)psDirectIFInfo->psParentDevInfo;
    		pDeviceInterface = DeviceInterface::find(psParentIFInfo->pszDeviceName);
    		pParentDevice->addNewDevice(psDirectIFInfo->pszDeviceName, IMG_FALSE);
			pDeviceInterface = DeviceInterface::find(psDirectIFInfo->pszDeviceName);
			psDeviceCB->pvDevIfInfo = (IMG_VOID*)pDeviceInterface;
		}
    	else
    	{
    		printf("ERROR No direct connection available for device %s\n", psDirectIFInfo->pszDeviceName);
    		return IMG_ERROR_FATAL;
    	}
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

	return IMG_SUCCESS;
}

static IMG_RESULT DIRECT_DEVIF_FakeInitialise(DEVIF_sDeviceCB * psDeviceCB)
{
	(void)psDeviceCB;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				direct_devif_GetDeviceInterface

******************************************************************************/
static
DeviceInterface *direct_devif_GetDeviceInterface(DEVIF_sDeviceCB *psDeviceCB)
{
	DeviceInterface *pDeviceInterface = NULL;
	
	// Initialise if necessary
	if (psDeviceCB->bInitialised == IMG_FALSE)
		DIRECT_DEVIF_Initialise(psDeviceCB);

	pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	return pDeviceInterface;
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_DeInitialise

******************************************************************************/
static void DIRECT_DEVIF_DeInitialise(	
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	if (psDeviceCB->bInitialised == IMG_TRUE) {
		DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
		pDeviceInterface->deinitialise();
		psDeviceCB->pvDevIfInfo = IMG_NULL;
		psDeviceCB->bInitialised = IMG_FALSE;
	}
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ReadRegister

******************************************************************************/
static IMG_UINT32 DIRECT_DEVIF_ReadRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readRegister(ui64RegAddress);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ReadRegister64

******************************************************************************/
static IMG_UINT64 DIRECT_DEVIF_ReadRegister64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readRegister64(ui64RegAddress);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_WriteRegister

******************************************************************************/
static void DIRECT_DEVIF_WriteRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeRegister(ui64RegAddress, ui32Value);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_WriteRegister64

******************************************************************************/
static void DIRECT_DEVIF_WriteRegister64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT64          ui64Value
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeRegister64(ui64RegAddress, ui64Value);
}


/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ReadMemory

******************************************************************************/
static IMG_UINT32 DIRECT_DEVIF_ReadMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readMemory(ui64DevAddress);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ReadMemory64

******************************************************************************/
static IMG_UINT64 DIRECT_DEVIF_ReadMemory64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readMemory64(ui64DevAddress);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_WriteMemory

******************************************************************************/
static void DIRECT_DEVIF_WriteMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeMemory(ui64DevAddress, ui32Value);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_WriteMemory64

******************************************************************************/
static void DIRECT_DEVIF_WriteMemory64(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT64          ui64Value
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeMemory64(ui64DevAddress, ui64Value);
}

 
/*!
******************************************************************************

 @Function				DIRECT_DEVIF_CopyDeviceToHost

******************************************************************************/
static void DIRECT_DEVIF_CopyDeviceToHost(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    void *          	pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyDeviceToHost(ui64DevAddress, pvHostAddress, ui32Size);
}


 
/*!
******************************************************************************

 @Function				DIRECT_DEVIF_CopyHostToDevice

******************************************************************************/
static void DIRECT_DEVIF_CopyHostToDevice(	
	DEVIF_sDeviceCB *   psDeviceCB,
    void *          	pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyHostToDevice(pvHostAddress, ui64DevAddress, ui32Size);
}


/*!
******************************************************************************

 @Function				DIRECT_DEVIF_AllocMem

******************************************************************************/
static void DIRECT_DEVIF_AllocMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->allocMem(ui64Address, ui32Size);
}


/*!
******************************************************************************

 @Function				DIRECT_DEVIF_FreeMem

******************************************************************************/
static void DIRECT_DEVIF_FreeMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->freeMem(ui64Address);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_Idle

******************************************************************************/
static void DIRECT_DEVIF_Idle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->idle(ui32NumBytes);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_DevicePrint

******************************************************************************/
static void DIRECT_DEVIF_DevicePrint(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pszString
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->devicePrint(pszString);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_SendComment

******************************************************************************/
static void DIRECT_DEVIF_SendComment(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pcComment
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->sendComment(pcComment);
}

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_GetDeviceTime

******************************************************************************/
static IMG_UINT32 DIRECT_DEVIF_GetDeviceTime(	
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64 *		pui64Time
)
{
	DeviceInterface* pDeviceInterface = direct_devif_GetDeviceInterface(psDeviceCB);
	IMG_ASSERT(pDeviceInterface);

	return pDeviceInterface->getDeviceTime(pui64Time);
}

/*!
******************************************************************************

 @Function				direct_devif_SetupFunctions

******************************************************************************/
static IMG_VOID	direct_devif_SetupFunctions(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	psDeviceCB->bInitialised        = IMG_FALSE;  
	psDeviceCB->pfnInitailise       = DIRECT_DEVIF_FakeInitialise;
	psDeviceCB->pfnDeinitailise     = DIRECT_DEVIF_DeInitialise;
	psDeviceCB->pfnReadRegister     = DIRECT_DEVIF_ReadRegister;
	psDeviceCB->pfnReadRegister64   = DIRECT_DEVIF_ReadRegister64;
	psDeviceCB->pfnWriteRegister    = DIRECT_DEVIF_WriteRegister;
	psDeviceCB->pfnWriteRegister64  = DIRECT_DEVIF_WriteRegister64;
	psDeviceCB->pfnReadMemory       = DIRECT_DEVIF_ReadMemory;
	psDeviceCB->pfnReadMemory64     = DIRECT_DEVIF_ReadMemory64;
	psDeviceCB->pfnWriteMemory      = DIRECT_DEVIF_WriteMemory;
	psDeviceCB->pfnWriteMemory64    = DIRECT_DEVIF_WriteMemory64;
	psDeviceCB->pfnCopyDeviceToHost = DIRECT_DEVIF_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice = DIRECT_DEVIF_CopyHostToDevice;
	psDeviceCB->pfnMallocMemory		= DIRECT_DEVIF_AllocMem;
	psDeviceCB->pfnFreeMemory		= DIRECT_DEVIF_FreeMem;
	psDeviceCB->pfnIdle				= DIRECT_DEVIF_Idle;
	psDeviceCB->pfnDevicePrint		= DIRECT_DEVIF_DevicePrint;
	psDeviceCB->pfnSendComment		= DIRECT_DEVIF_SendComment;
	psDeviceCB->pfnGetDeviceTime	= DIRECT_DEVIF_GetDeviceTime;
}
/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ConfigureDevice

******************************************************************************/
IMG_VOID
DIRECT_DEVIF_ConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	DEVIF_sDirectIFInfo *	psDirectIFInfo = &psDeviceCB->sDevIfSetup.u.sDirectIFInfo;

	psDeviceCB->pvDevIfInfo = DeviceInterface::find(psDirectIFInfo->pszDeviceName);

	direct_devif_SetupFunctions(psDeviceCB);

}
