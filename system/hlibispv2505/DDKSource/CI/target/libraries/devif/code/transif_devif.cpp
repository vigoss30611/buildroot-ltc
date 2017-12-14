/*! 
******************************************************************************
 @file   : transif_devif.c

 @brief  

 @Author James Brodie

 @date   09/03/2010
 
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

#include "transif_devif.h"
#include "device_interface_master_transif.h"

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_Initialise

******************************************************************************/
static IMG_RESULT TRANSIF_DEVIF_Initialise(	
	DEVIF_sDeviceCB * psDeviceCB
)
{
	DeviceInterface* pDeviceInterface = NULL;

	pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);

    if (pDeviceInterface)
    {
	    if (!psDeviceCB->bInitialised) 
        {
		    pDeviceInterface->initialise();
		    psDeviceCB->bInitialised = IMG_TRUE;
	    }
    }

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_DeInitialise

******************************************************************************/
static void TRANSIF_DEVIF_DeInitialise(	
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DeviceInterface* pDeviceInterface = NULL;

	pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	if (!psDeviceCB->bInitialised) {
		pDeviceInterface->deinitialise();
		psDeviceCB->bInitialised = IMG_FALSE;
	}
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_ReadRegister

******************************************************************************/
static IMG_UINT32 TRANSIF_DEVIF_ReadRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32RegAddress
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readRegister(ui32RegAddress);
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_WriteRegister

******************************************************************************/
static void TRANSIF_DEVIF_WriteRegister(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32RegAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeRegister(ui32RegAddress, ui32Value);
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_ReadMemory

******************************************************************************/
static IMG_UINT32 TRANSIF_DEVIF_ReadMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

    return pDeviceInterface->readMemory(ui64DevAddress);
}


 
/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_WriteMemory

******************************************************************************/
static void TRANSIF_DEVIF_WriteMemory(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->writeMemory(ui64DevAddress, ui32Value);
}

 
/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_CopyDeviceToHost

******************************************************************************/
static void TRANSIF_DEVIF_CopyDeviceToHost(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    void *          	pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyDeviceToHost(ui64DevAddress, pvHostAddress, ui32Size);
}


 
/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_CopyHostToDevice

******************************************************************************/
static void TRANSIF_DEVIF_CopyHostToDevice(	
	DEVIF_sDeviceCB *   psDeviceCB,
    void *          	pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->copyHostToDevice(pvHostAddress, ui64DevAddress, ui32Size);
}


/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_AllocMem

******************************************************************************/
static void TRANSIF_DEVIF_AllocMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address,
    IMG_UINT32          ui32Size
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->allocMem(ui64Address, ui32Size);
}


/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_FreeMem

******************************************************************************/
static void TRANSIF_DEVIF_FreeMem(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64Address
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->freeMem(ui64Address);
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_Idle

******************************************************************************/
static void TRANSIF_DEVIF_Idle(	
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumBytes
)
{
	DeviceInterface* pDeviceInterface = static_cast<DeviceInterface*> (psDeviceCB->pvDevIfInfo);
	IMG_ASSERT(pDeviceInterface);

	pDeviceInterface->idle(ui32NumBytes);
}

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_CopyAbsToHost

******************************************************************************/
void TRANSIF_DEVIF_CopyAbsToHost(	
    IMG_UINT32          ui32AbsAddress,
    void *          	pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	printf("TRANSIF_DEVIF_CopyAbsToHost() should only be called on FPGA builds\n");
	IMG_ASSERT(IMG_FALSE);
}


/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_ConfigureDevice

******************************************************************************/
IMG_VOID
TRANSIF_DEVIF_ConfigureDevice(
	IMG_CHAR *				pszDevName,
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	DeviceInterfaceMasterTransif * pDeviceInterfaceMasterTransif = IMG_NULL;
	DEVIF_sFullInfo* psDevSetup = psDeviceCB->psDevIfSetup;

    pDeviceInterfaceMasterTransif = new DeviceInterfaceMasterTransif(pszDevName, false, "parent_name");
    DeviceInterface::addToSystem(*pDeviceInterfaceMasterTransif);

	DeviceInterface* pDeviceInterface = DeviceInterface::find(pszDevName);

	
	memset(psDeviceCB, 0, sizeof(DEVIF_sDeviceCB));
	psDeviceCB->psDevIfSetup = psDevSetup;

	psDeviceCB->bInitialised        = IMG_FALSE;  
	psDeviceCB->pfnInitailise       = TRANSIF_DEVIF_Initialise;
	psDeviceCB->pfnDeinitailise     = TRANSIF_DEVIF_DeInitialise;
	psDeviceCB->pfnReadRegister     = TRANSIF_DEVIF_ReadRegister;
	psDeviceCB->pfnWriteRegister    = TRANSIF_DEVIF_WriteRegister;
	psDeviceCB->pfnReadMemory       = TRANSIF_DEVIF_ReadMemory;
	psDeviceCB->pfnWriteMemory      = TRANSIF_DEVIF_WriteMemory;
	psDeviceCB->pfnCopyDeviceToHost = TRANSIF_DEVIF_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice = TRANSIF_DEVIF_CopyHostToDevice;
	psDeviceCB->pfnMallocMemory		= TRANSIF_DEVIF_AllocMem;
	psDeviceCB->pfnFreeMemory		= TRANSIF_DEVIF_FreeMem;
	psDeviceCB->pfnIdle				= TRANSIF_DEVIF_Idle;
	psDeviceCB->pvDevIfInfo			= pDeviceInterface;
}
