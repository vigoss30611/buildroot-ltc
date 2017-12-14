/*!
********************************************************************************
 @file   : tconf_int.c

 @brief

 @Author Jose Massada

 @date   05/10/2012

         <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
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
         Target Config File internal functions.

 <b>Platform:</b>\n
	     Platform Independent
		  
 @Version
	     1.0

*******************************************************************************/
#include "tconf.h"
#include "tconf_int.h"
#include "tconf_txt.h"
#include "devif_setup.h"
#include "img_errors.h"


/*!
******************************************************************************

 @Function				tconf_FindDevice

******************************************************************************/
TCONF_sDevItem *tconf_FindDevice(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName)
{
	TCONF_psDevItem pDevItem;

	IMG_ASSERT(pszName != NULL);
	IMG_ASSERT(strlen(pszName) > 0);

	pDevItem = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);

    while (pDevItem != IMG_NULL)
    {
		if (strcmp(pDevItem->sTalDeviceCB.pszDeviceName, pszName) == 0)
			return pDevItem;

    	pDevItem = (TCONF_psDevItem)LST_next(pDevItem);
    }

	return NULL;
}

/*!
******************************************************************************

 @Function				TCONF_AddDevice

******************************************************************************/
IMG_RESULT TCONF_AddDevice(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT64 nBaseAddr)
{
	TCONF_sDevItem *pDevItem;

	IMG_ASSERT(pszName != NULL);
	IMG_ASSERT(strlen(pszName) > 0);
	
	pDevItem = tconf_FindDevice(psTargetConf, pszName);
	if (pDevItem != NULL)
	{
		printf("WARNING: tconf, Device already exists %s\n", pszName);
		return IMG_ERROR_ALREADY_COMPLETE;
	}

	pDevItem = (TCONF_psDevItem)IMG_CALLOC(1, sizeof(TCONF_sDevItem));
	if (pDevItem != NULL)
	{
		TAL_sDeviceInfo *pDeviceInfo;

		pDeviceInfo = &pDevItem->sTalDeviceCB;
		pDeviceInfo->pszDeviceName = IMG_STRDUP(pszName);
		if (pDeviceInfo->pszDeviceName == NULL)
		{
			IMG_ASSERT(IMG_FALSE);
			IMG_FREE(pDevItem);
			return IMG_ERROR_OUT_OF_MEMORY;
		}

		pDeviceInfo->ui64DeviceBaseAddr = nBaseAddr;

		LST_add(&psTargetConf->sDevItemList, pDevItem);
	}
	return IMG_SUCCESS;
}



/*!
******************************************************************************

 @Function				TCONF_SetDeviceDefaultMem

******************************************************************************/
IMG_RESULT TCONF_SetDeviceDefaultMem(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	IMG_UINT64 nBaseAddr,
	IMG_UINT64 nSize,
	IMG_UINT64 nGuardBand)
{
	TAL_sDeviceInfo *pDeviceInfo;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszName);
	
	if (pDevItem == NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	pDeviceInfo = &pDevItem->sTalDeviceCB;
	pDeviceInfo->ui64MemBaseAddr = nBaseAddr;
	pDeviceInfo->ui64DefMemSize = nSize;
	pDeviceInfo->ui64MemGuardBand = nGuardBand;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_SetDeviceFlags

******************************************************************************/
IMG_RESULT TCONF_SetDeviceFlags(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT32 nFlags)
{
	TAL_sDeviceInfo *pDeviceInfo;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszName);

	if (pDevItem == NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	pDeviceInfo = &pDevItem->sTalDeviceCB;
	pDeviceInfo->ui32DevFlags |= nFlags;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_ClearDeviceFlags

******************************************************************************/
IMG_RESULT TCONF_ClearDeviceFlags(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT32 nFlags)
{
	TAL_sDeviceInfo *pDeviceInfo;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszName);
	
	if (pDevItem == NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	pDeviceInfo = &pDevItem->sTalDeviceCB;
	pDeviceInfo->ui32DevFlags &= ~nFlags;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddDeviceRegister

******************************************************************************/
IMG_RESULT TCONF_AddDeviceRegister(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDevName, 
	const IMG_CHAR *pszRegName, 
	IMG_UINT64 nOffset, 
	IMG_UINT32 nSize, 
	IMG_UINT32 nIRQ)
{
	TAL_sMemSpaceInfo *pMemSpaceInfo;
	TCONF_sMemSpItem *pMemSpItem;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszDevName);
	
	if (pDevItem == NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszDevName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	IMG_ASSERT(pszRegName != NULL);
	IMG_ASSERT(strlen(pszRegName) > 0);

	pMemSpItem = (TCONF_sMemSpItem *)IMG_CALLOC(1, sizeof(TCONF_sMemSpItem));
	if (pMemSpItem == NULL)
		return IMG_ERROR_OUT_OF_MEMORY;

	pMemSpaceInfo = &pMemSpItem->sTalMemSpaceCB;
	pMemSpaceInfo->pszMemSpaceName = IMG_STRDUP(pszRegName);
	if (pMemSpaceInfo->pszMemSpaceName == NULL)
		return IMG_ERROR_OUT_OF_MEMORY;


	pMemSpaceInfo->psDevInfo = &pDevItem->sTalDeviceCB;
	pMemSpaceInfo->eMemSpaceType = TAL_MEMSPACE_REGISTER;
	pMemSpaceInfo->sRegInfo.ui64RegBaseAddr = nOffset;
    pMemSpaceInfo->sRegInfo.ui32RegSize = nSize;
    pMemSpaceInfo->sRegInfo.ui32intnum = nIRQ;
    
    LST_add(&psTargetConf->sMemSpItemList, pMemSpItem);
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddDeviceMemory

******************************************************************************/
IMG_RESULT TCONF_AddDeviceMemory(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDevName, 
	const IMG_CHAR *pszMemName, 
	IMG_UINT64 nBaseAddr, 
	IMG_UINT64 nSize, 
	IMG_UINT64 nGuardBand)
{
	TAL_sMemSpaceInfo *pMemSpaceInfo;
	TCONF_sMemSpItem *pMemSpItem;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszDevName);

	if (pDevItem == NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszDevName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	IMG_ASSERT(pszMemName != NULL);
	IMG_ASSERT(strlen(pszMemName) > 0);

    pMemSpItem = (TCONF_sMemSpItem *)IMG_CALLOC(1, sizeof(TCONF_sMemSpItem));
	if (pMemSpItem == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_FALSE;
	}

	pMemSpaceInfo = &pMemSpItem->sTalMemSpaceCB;
	pMemSpaceInfo->pszMemSpaceName = IMG_STRDUP(pszMemName);
	if (pMemSpaceInfo->pszMemSpaceName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		IMG_FREE(pMemSpItem);
		return IMG_FALSE;
	}

	pMemSpaceInfo->eMemSpaceType = TAL_MEMSPACE_MEMORY;
	pMemSpaceInfo->psDevInfo = &pDevItem->sTalDeviceCB;
    pMemSpaceInfo->sMemInfo.ui64MemBaseAddr	= nBaseAddr;
    pMemSpaceInfo->sMemInfo.ui64MemGuardBand = nGuardBand;
    pMemSpaceInfo->sMemInfo.ui64MemSize = nSize;
	
    LST_add(&psTargetConf->sMemSpItemList, pMemSpItem);
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddDeviceSubDevice

******************************************************************************/
IMG_BOOL TCONF_AddDeviceSubDevice(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDevName,
	const IMG_CHAR *pszSubName,
	IMG_CHAR chType,
	IMG_UINT64 nBaseAddr, 
	IMG_UINT64 nSize)
{
	TAL_sSubDeviceCB *pSubDevCB;
	TAL_sDeviceInfo *pDeviceCB;
	TCONF_psDevItem pDevItem = tconf_FindDevice(psTargetConf, pszDevName);

	IMG_ASSERT(pDevItem != NULL);
	IMG_ASSERT(pszSubName != NULL);
	IMG_ASSERT(strlen(pszSubName) > 0);
	IMG_ASSERT(chType == 'M' || chType == 'R');

	pDeviceCB = &pDevItem->sTalDeviceCB;
	IMG_ASSERT(pDeviceCB != NULL);

	/* Find spare sub device */
	pSubDevCB = pDeviceCB->psSubDeviceCB;
	while (pSubDevCB != IMG_NULL)
	{
		if (pSubDevCB->pszMemName == IMG_NULL || pSubDevCB->pszRegName == IMG_NULL)
			break;

		pSubDevCB = pSubDevCB->pNext;
	}

	/* Create sub device */
	if (pSubDevCB == IMG_NULL)
	{
		pSubDevCB = (TAL_sSubDeviceCB *)IMG_CALLOC(1, sizeof(TAL_sSubDeviceCB));
		if (pSubDevCB == NULL)
			return IMG_FALSE;

		/* Add to list */
		pSubDevCB->pNext = pDeviceCB->psSubDeviceCB;
		pDeviceCB->psSubDeviceCB = pSubDevCB;
	}

	/* Fill in the details */
	if (chType == 'M')
	{
		pSubDevCB->pszMemName = IMG_STRDUP(pszSubName);
		if (pSubDevCB->pszMemName == NULL)
		{
			pDeviceCB->psSubDeviceCB = pSubDevCB->pNext;
			IMG_FREE(pSubDevCB);
			return IMG_FALSE;
		}

		pSubDevCB->ui64MemStartAddress = nBaseAddr;
		pSubDevCB->ui64MemSize = nSize;

		/* Basenames need to be regname:memname */
		TCONF_AddBaseName(psTargetConf, pszDevName, NULL, pszSubName);
	}
	else /* (chType == 'R') */
	{
		pSubDevCB->pszRegName = IMG_STRDUP(pszSubName);
		if (pSubDevCB->pszRegName == NULL)
		{
			pDeviceCB->psSubDeviceCB = pSubDevCB->pNext;
			IMG_FREE(pSubDevCB);
			return IMG_FALSE;
		}

		pSubDevCB->ui64RegStartAddress = nBaseAddr;
		pSubDevCB->ui32RegSize = IMG_UINT64_TO_UINT32(nSize);

		/* Basenames need to be regname:memname */
		TCONF_AddBaseName(psTargetConf, pszDevName, pszSubName, NULL);
	}
	return IMG_TRUE;
}

/*!
******************************************************************************

 @Function				tconf_FindPciInterface

******************************************************************************/
IMG_RESULT tconf_FindPciInterface(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, DEVIF_sPciInfo ** ppsPciInfo)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsPciInfo = IMG_NULL;
	
	if(*pszName  == '\0')  // If there is no name, default to the first available device
	{
		psDevInfo = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);
		psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	}
	else
	{
		psDevInfo = tconf_FindDevice(psTargetConf, pszName);
		if (psDevInfo == IMG_NULL)
		{
			printf("WARNING: Cannot find device %s in config file\n", pszName);
			return IMG_ERROR_DEVICE_NOT_FOUND;
		}
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	switch (psDevifInfo->sWrapCtrlInfo.eDevifType)
	{
	case DEVIF_TYPE_PCI:
	case DEVIF_TYPE_BMEM:
	case DEVIF_TYPE_POSTED:
		*ppsPciInfo = &psDevifInfo->u.sPciInfo;
		return IMG_SUCCESS;
	case DEVIF_TYPE_SOCKET:
	case DEVIF_TYPE_PDUMP1:
	case DEVIF_TYPE_DASH:
	case DEVIF_TYPE_DIRECT:
	case DEVIF_TYPE_NULL:
		return IMG_ERROR_NOT_SUPPORTED;
	default:
		return IMG_ERROR_INVALID_PARAMETERS;
	}
}

/*!
******************************************************************************

 @Function				TCONF_AddPciInterface

******************************************************************************/
IMG_RESULT TCONF_AddPciInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName, 
	IMG_UINT32 nVendorId, 
	IMG_UINT32 nDeviceId)
{
	DEVIF_sPciInfo *pPciInterfaceInfo;
	IMG_RESULT rResult;
	
	rResult = tconf_FindPciInterface(psTargetConf, pszName, &pPciInterfaceInfo);
	
	if (rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS;	// We are not using PCI so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	if(pszName[0] == '\0') // If no name, setup all devices
	{
		TCONF_sDevItem * psDevInfo;
		DEVIF_sFullInfo * psDevifInfo;
		psDevInfo = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);
		
		while (psDevInfo != IMG_NULL)
		{
			psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
			pPciInterfaceInfo = &psDevifInfo->u.sPciInfo;
			pPciInterfaceInfo->pszDeviceName = IMG_STRDUP(psDevInfo->sTalDeviceCB.pszDeviceName);
			pPciInterfaceInfo->ui32VendorId = nVendorId;
			pPciInterfaceInfo->ui32DeviceId = nDeviceId;
			psDevInfo = (TCONF_psDevItem)LST_next(psDevInfo);
		}	

	}
	else
	{
		pPciInterfaceInfo->pszDeviceName = IMG_STRDUP(pszName);
		pPciInterfaceInfo->ui32VendorId = nVendorId;
		pPciInterfaceInfo->ui32DeviceId = nDeviceId;
		
		if (pPciInterfaceInfo->pszDeviceName == NULL)
		{
			IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_OUT_OF_MEMORY;
		}
	}
	

	
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddPciInterfaceDeviceBase

******************************************************************************/
IMG_RESULT TCONF_AddPciInterfaceDeviceBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_UINT32 nBar,
	IMG_UINT32 nBaseAddr,
	IMG_UINT32 nSize,
	IMG_BOOL   bAddDevBaseAddr
)
{
	DEVIF_sPciInfo *pPciInterfaceInfo;
	IMG_RESULT rResult;
	
	rResult = tconf_FindPciInterface(psTargetConf, pszDeviceName, &pPciInterfaceInfo);
	
	if (rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS;	// We are not using PCI so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	if(pszDeviceName[0] == '\0') // If no name, setup all devices
	{
		TCONF_sDevItem * psDevInfo;
		DEVIF_sFullInfo * psDevifInfo;
		psDevInfo = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);
		
		while (psDevInfo != IMG_NULL)
		{
			psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
			pPciInterfaceInfo = &psDevifInfo->u.sPciInfo;
			pPciInterfaceInfo->ui32RegBar = nBar;
			pPciInterfaceInfo->ui32RegOffset = nBaseAddr;
			pPciInterfaceInfo->ui32RegSize = nSize;
			pPciInterfaceInfo->bPCIAddDevOffset = bAddDevBaseAddr;
			psDevInfo = (TCONF_psDevItem)LST_next(psDevInfo);
		}	

	}
	else
	{
		pPciInterfaceInfo->ui32RegBar = nBar;
		pPciInterfaceInfo->ui32RegOffset = nBaseAddr;
		pPciInterfaceInfo->ui32RegSize = nSize;
		pPciInterfaceInfo->bPCIAddDevOffset = bAddDevBaseAddr;
	}
	


	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddPciInterfaceMemoryBase

******************************************************************************/
IMG_RESULT TCONF_AddPciInterfaceMemoryBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_UINT32 nBar,
	IMG_UINT32 nBaseAddr,
	IMG_UINT32 nSize)
{
	DEVIF_sPciInfo *pPciInterfaceInfo;
	IMG_RESULT rResult;
	TCONF_sDevItem * psDevInfo;
	
	rResult = tconf_FindPciInterface(psTargetConf, pszDeviceName, &pPciInterfaceInfo);
		
	if (rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS;	// We are not using PCI so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	// Use the device name from the interface in case the given one is empty
	psDevInfo = tconf_FindDevice(psTargetConf, pPciInterfaceInfo->pszDeviceName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pPciInterfaceInfo->pszDeviceName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	if(pszDeviceName[0] == '\0') // If no name, setup all devices
	{
		TCONF_sDevItem * psDevInfo;
		DEVIF_sFullInfo * psDevifInfo;
		psDevInfo = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);
		
		while (psDevInfo != IMG_NULL)
		{
			psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
			pPciInterfaceInfo = &psDevifInfo->u.sPciInfo;
			pPciInterfaceInfo->ui32MemBar = nBar;
			pPciInterfaceInfo->ui32MemOffset = nBaseAddr;
			pPciInterfaceInfo->ui32MemSize = nSize;
			pPciInterfaceInfo->ui64BaseAddr = psDevInfo->sTalDeviceCB.ui64DeviceBaseAddr;
			psDevInfo = (TCONF_psDevItem)LST_next(psDevInfo);
		}	

	}
	else
	{
		pPciInterfaceInfo->ui32MemBar = nBar;
		pPciInterfaceInfo->ui32MemOffset = nBaseAddr;
		pPciInterfaceInfo->ui32MemSize = nSize;
		pPciInterfaceInfo->ui64BaseAddr = psDevInfo->sTalDeviceCB.ui64DeviceBaseAddr;
	}
	


	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddBurnMem

******************************************************************************/
IMG_RESULT TCONF_AddBurnMem(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_CHAR chDeviceId,
	IMG_UINT32 nOffset,
	IMG_UINT32 nSize)
{
	(void)pszDeviceName;
	(void)chDeviceId;
	(void)nOffset;
	(void)nSize;
	(void)psTargetConf;
// 	TCONF_sBurnMemInfo *pBurnMemInfo;
// 
// 	IMG_ASSERT(pszDeviceName != NULL);
// 
// 	/* Create burn-mem */
//     pBurnMemInfo = (TCONF_sBurnMemInfo *)IMG_CALLOC(1, sizeof(TCONF_sBurnMemInfo));
// 	if (pBurnMemInfo != NULL)
// 	{
// 	    pBurnMemInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
// 		if (pBurnMemInfo->pszDeviceName == NULL)
// 		{
// 			IMG_ASSERT(IMG_FALSE);
// 			IMG_FREE(pBurnMemInfo);
// 			return NULL;
// 		}
// 
// 		pBurnMemInfo->cDeviceId = chDeviceId;
// 		pBurnMemInfo->ui32MemStartOffset = nOffset;
// 		pBurnMemInfo->ui32MemSize = nSize;
// 
// 		/* Add to the list */
// 		pBurnMemInfo->psNextBurnMemInfo = gsTargetConf.psBurnMemInfo;
// 		gsTargetConf.psBurnMemInfo = pBurnMemInfo;
// 	}
	printf("WARNING: BurnMem setup in config file, BurnMem has been disabled.\nContact Imagination Pdump Tools Team to request re-activation\n");
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				tconf_FindSocketInterface

******************************************************************************/
IMG_RESULT tconf_FindSocketInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	DEVIF_sDeviceIpInfo ** ppsIpInfo
)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsIpInfo = IMG_NULL;
	
	psDevInfo = tconf_FindDevice(psTargetConf, pszName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	switch (psDevifInfo->sWrapCtrlInfo.eDevifType)
	{
	case DEVIF_TYPE_SOCKET:
		*ppsIpInfo = &psDevifInfo->u.sDeviceIpInfo;
		return IMG_SUCCESS;
	case DEVIF_TYPE_DIRECT:
		// We are relying on the structure being the same in the union
		*ppsIpInfo = &psDevifInfo->u.sDeviceIpInfo;
		return IMG_ERROR_DEVICE_UNAVAILABLE;
	case DEVIF_TYPE_PCI:
	case DEVIF_TYPE_BMEM:
	case DEVIF_TYPE_POSTED:	  
	case DEVIF_TYPE_PDUMP1:
	case DEVIF_TYPE_DASH:
	case DEVIF_TYPE_NULL:
		return IMG_ERROR_NOT_SUPPORTED;
	default:
		return IMG_ERROR_INVALID_PARAMETERS;
	}
}

/*!
******************************************************************************

 @Function				TCONF_AddDeviceIp

******************************************************************************/
IMG_RESULT TCONF_AddDeviceIp(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszParentDeviceName,
	const IMG_CHAR *pszHostname,
	IMG_UINT16 nPort,
	IMG_BOOL bBuffering)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sDeviceIpInfo *pDeviceIpInfo;
	IMG_RESULT rResult;
	
	rResult =  tconf_FindSocketInterface(psTargetConf, pszDeviceName, &pDeviceIpInfo);
	if (rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS;	// We are not using Socket so ignore
	if (rResult != IMG_SUCCESS && !(rResult == IMG_ERROR_DEVICE_UNAVAILABLE))
		return rResult;
	
	// Get the socket buffereing setting from the device info
	psDevInfo = tconf_FindDevice(psTargetConf, pszDeviceName);
	IMG_ASSERT(psDevInfo);
	
	if(pDeviceIpInfo->pszDeviceName != IMG_NULL)
	{
		// This device has been overriden, ignore these commands
		return IMG_SUCCESS;
	}
	pDeviceIpInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
	if (pDeviceIpInfo->pszDeviceName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_OUT_OF_MEMORY;
	}

	/* Connect using parent device */
	if (pszParentDeviceName != NULL)
	{
		rResult = tconf_FindSocketInterface(psTargetConf, pszParentDeviceName, &pDeviceIpInfo->psParentDevInfo);
		if (rResult != IMG_SUCCESS && !(rResult == IMG_ERROR_DEVICE_UNAVAILABLE))
			return rResult;
	}
	else /* Connect using hostname and port */
	{
		/* If pszHostname is NULL, it defaults to local machine (ui32IpAddrX = 0) */
		if (pszHostname != NULL)
		{
			IMG_BYTE ch1, ch2, ch3, ch4;

			/* Everytime someone writes code like this socket info implementation a dolphin dies */
#ifdef _MSC_VER
			if (sscanf(pszHostname, "%hc.%hc.%hc.%hc", &ch1, &ch2, &ch3, &ch4) == 4)
#else
			if (sscanf(pszHostname, "%hhu.%hhu.%hhu.%hhu", &ch1, &ch2, &ch3, &ch4) == 4)
#endif
			{
				/* Using ip address */
				pDeviceIpInfo->ui32IpAddr1 = (IMG_UINT32)ch1;
				pDeviceIpInfo->ui32IpAddr2 = (IMG_UINT32)ch2;
				pDeviceIpInfo->ui32IpAddr3 = (IMG_UINT32)ch3;
				pDeviceIpInfo->ui32IpAddr4 = (IMG_UINT32)ch4;
			}
			else
			{
				/* Using remote machine name */
				pDeviceIpInfo->pszRemoteName = IMG_STRDUP(pszHostname);
				if (pDeviceIpInfo->pszRemoteName == NULL)
				{
					IMG_ASSERT(IMG_FALSE);
					IMG_FREE(pDeviceIpInfo->pszDeviceName);
					return IMG_ERROR_OUT_OF_MEMORY;
				}
			}
		}

		pDeviceIpInfo->ui32PortId = nPort;
	}

	pDeviceIpInfo->bUseBuffer = bBuffering;


	return IMG_SUCCESS;	
}

/*!
******************************************************************************

 @Function				TCONF_AddPdump1IF

******************************************************************************/
IMG_RESULT TCONF_AddPdump1IF(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszInputDir,
	const IMG_CHAR *pszOutputDir,
	const IMG_CHAR *pszCommandFile,
	const IMG_CHAR *pszSendFile,
	const IMG_CHAR *pszReceiveFile)
{
	(void)psTargetConf;
	(void)pszDeviceName;
	(void)pszInputDir;
	(void)pszOutputDir;
	(void)pszCommandFile;
	(void)pszSendFile;
	(void)pszReceiveFile;
// 	TCONF_sPDump1IFInfo *pPDump1IFInfo;
// 
// 	IMG_ASSERT(pszDeviceName != NULL && *pszDeviceName != '\0');
// 	IMG_ASSERT(pszInputDir != NULL && *pszInputDir != '\0');
// 	IMG_ASSERT(pszOutputDir != NULL && *pszOutputDir != '\0');
// 	IMG_ASSERT(pszCommandFile != NULL && *pszCommandFile != '\0');
// 	IMG_ASSERT(pszSendFile != NULL && *pszSendFile != '\0');
// 	IMG_ASSERT(pszReceiveFile != NULL && *pszReceiveFile != '\0');
// 
// 	/* Create PDump1IF */
//     pPDump1IFInfo = (TCONF_sPDump1IFInfo *)IMG_CALLOC(1, sizeof(TCONF_sPDump1IFInfo));
// 	if (pPDump1IFInfo != NULL)
// 	{
// 
// 		pPDump1IFInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
// 		pPDump1IFInfo->cpInputDirectory = IMG_STRDUP(pszInputDir);
// 		pPDump1IFInfo->cpOutputDirectory = IMG_STRDUP(pszOutputDir);
// 		pPDump1IFInfo->cpCommandFile = IMG_STRDUP(pszCommandFile);
// 		pPDump1IFInfo->cpSendData = IMG_STRDUP(pszSendFile);
// 		pPDump1IFInfo->cpReturnData = IMG_STRDUP(pszReceiveFile);
// 
// 		if (pPDump1IFInfo->pszDeviceName == NULL ||
// 			pPDump1IFInfo->cpInputDirectory == NULL ||
// 			pPDump1IFInfo->cpOutputDirectory == NULL ||
// 			pPDump1IFInfo->cpCommandFile == NULL ||
// 			pPDump1IFInfo->cpSendData == NULL ||
// 			pPDump1IFInfo->cpReturnData == NULL)
// 		{
// 			IMG_ASSERT(IMG_FALSE);
// 			IMG_FREE(pPDump1IFInfo);
// 			return NULL;
// 		}
// 
// 	    /* Add to the the list */
// 	    pPDump1IFInfo->psNextPDump1IFInfo = gsTargetConf.psPDump1IFInfo;
// 	    gsTargetConf.psPDump1IFInfo = pPDump1IFInfo;
// 	}
// 	return pPDump1IFInfo;
		printf("WARNING: Pdump1 interface setup in config file, Pdump1 interface has been disabled.\nContact Imagination Pdump Tools Team to request re-activation\n");
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				tconf_FindDashInterface

******************************************************************************/
IMG_RESULT tconf_FindDashInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	DEVIF_sDashIFInfo ** ppsDashInfo)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsDashInfo = IMG_NULL;
	
	psDevInfo = tconf_FindDevice(psTargetConf, pszName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	switch (psDevifInfo->sWrapCtrlInfo.eDevifType)
	{
	case DEVIF_TYPE_DASH:
		*ppsDashInfo = &psDevifInfo->u.sDashIFInfo;
		return IMG_SUCCESS;
	case DEVIF_TYPE_SOCKET:
	case DEVIF_TYPE_DIRECT:
	case DEVIF_TYPE_PCI:
	case DEVIF_TYPE_BMEM:
	case DEVIF_TYPE_POSTED:	  
	case DEVIF_TYPE_PDUMP1:
	case DEVIF_TYPE_NULL:
		return IMG_ERROR_NOT_SUPPORTED;
	default:
		return IMG_ERROR_INVALID_PARAMETERS;
	}
}

/*!
******************************************************************************

 @Function				TCONF_AddDashInterface

******************************************************************************/
IMG_RESULT TCONF_AddDashInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_UINT32 nId)
{
	DEVIF_sDashIFInfo *pDeviceDashInfo;
	IMG_RESULT rResult;
	IMG_CHAR szDashId[256];
	
	rResult =  tconf_FindDashInterface(psTargetConf, pszDeviceName, &pDeviceDashInfo);
	if(rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS; // We are not using Dash so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;

	
	sprintf(szDashId, "%u", nId);

	pDeviceDashInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
	pDeviceDashInfo->pcDashName = IMG_STRDUP(szDashId);

	if (pDeviceDashInfo->pszDeviceName == NULL ||
		pDeviceDashInfo->pcDashName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_OUT_OF_MEMORY;
	}

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_SetDashInterfaceDeviceBase

******************************************************************************/
IMG_RESULT TCONF_SetDashInterfaceDeviceBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	IMG_UINT32 nBaseAddr, 
	IMG_UINT32 nSize)
{
  	DEVIF_sDashIFInfo *pDeviceDashInfo;
	IMG_RESULT rResult;
	
	rResult =  tconf_FindDashInterface(psTargetConf, pszName, &pDeviceDashInfo);
	if(rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS; // We are not using Dash so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	pDeviceDashInfo->ui32DeviceBase = nBaseAddr;
	pDeviceDashInfo->ui32DeviceSize = nSize;
	return rResult;
}

/*!
******************************************************************************

 @Function				TCONF_SetDashInterfaceMemoryBase

******************************************************************************/
IMG_RESULT TCONF_SetDashInterfaceMemoryBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	IMG_UINT32 nBaseAddr, 
	IMG_UINT32 nSize)
{
  	DEVIF_sDashIFInfo *pDeviceDashInfo;
	IMG_RESULT rResult;
	
	rResult =  tconf_FindDashInterface(psTargetConf, pszName, &pDeviceDashInfo);
	if(rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS; // We are not using Dash so ignore
	if (rResult != IMG_SUCCESS)
		return rResult;

	pDeviceDashInfo->ui32MemoryBase = nBaseAddr;
	pDeviceDashInfo->ui32MemorySize = nSize;
	return rResult;
}

/*!
******************************************************************************

 @Function				tconf_FindDirectInterface

******************************************************************************/
IMG_RESULT tconf_FindDirectInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	DEVIF_sDirectIFInfo ** ppsDirectInfo
)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsDirectInfo = IMG_NULL;
	
	psDevInfo = tconf_FindDevice(psTargetConf, pszName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	switch (psDevifInfo->sWrapCtrlInfo.eDevifType)
	{
	case DEVIF_TYPE_DIRECT:
		*ppsDirectInfo = &psDevifInfo->u.sDirectIFInfo;
		return IMG_SUCCESS;
	case DEVIF_TYPE_SOCKET:	
		*ppsDirectInfo = &psDevifInfo->u.sDirectIFInfo;
		return IMG_ERROR_DEVICE_UNAVAILABLE;
	case DEVIF_TYPE_DASH:
	case DEVIF_TYPE_PCI:
	case DEVIF_TYPE_BMEM:
	case DEVIF_TYPE_POSTED:	  
	case DEVIF_TYPE_PDUMP1:
	case DEVIF_TYPE_NULL:
		return IMG_ERROR_NOT_SUPPORTED;
	default:
		return IMG_ERROR_INVALID_PARAMETERS;
	}
}

/*!
******************************************************************************

 @Function				TCONF_AddDirectInterface

******************************************************************************/
IMG_RESULT TCONF_AddDirectInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszParentDeviceName)
{
    DEVIF_sDirectIFInfo *psDirectInfo;
	IMG_RESULT rResult;
	
	rResult =  tconf_FindDirectInterface(psTargetConf, pszDeviceName, &psDirectInfo);
	if(rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS; // Direct interface not in use so ignore
	if (rResult != IMG_SUCCESS && !(rResult == IMG_ERROR_DEVICE_UNAVAILABLE))
		return rResult;
	
	if(psDirectInfo->pszDeviceName != IMG_NULL)
	{
		// This device has been overriden, ignore these commands
		return IMG_SUCCESS;
	}
	psDirectInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
	if (psDirectInfo->pszDeviceName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_OUT_OF_MEMORY;
	}

	/* Connect using parent device */
	if (pszParentDeviceName != NULL)
	{
		rResult = tconf_FindDirectInterface(psTargetConf, pszParentDeviceName, &psDirectInfo->psParentDevInfo);
		if (rResult != IMG_SUCCESS && !(rResult == IMG_ERROR_DEVICE_UNAVAILABLE))
			return rResult;
	}
	
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				tconf_FindPostedInterface

******************************************************************************/
IMG_RESULT tconf_FindPostedInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	DEVIF_sPostIFInfo ** ppsPostedInfo
)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsPostedInfo = IMG_NULL;
	
	psDevInfo = tconf_FindDevice(psTargetConf, pszName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	switch (psDevifInfo->sWrapCtrlInfo.eDevifType)
	{
	case DEVIF_TYPE_POSTED:
		*ppsPostedInfo = &psDevifInfo->u.sPostIFInfo;
		return IMG_SUCCESS;
	case DEVIF_TYPE_DIRECT:
	case DEVIF_TYPE_SOCKET:	
	case DEVIF_TYPE_DASH:
	case DEVIF_TYPE_PCI:
	case DEVIF_TYPE_BMEM:
	case DEVIF_TYPE_PDUMP1:
	case DEVIF_TYPE_NULL:
		return IMG_ERROR_NOT_SUPPORTED;
	default:
		return IMG_ERROR_INVALID_PARAMETERS;
	}
}

/*!
******************************************************************************

 @Function				TCONF_AddPostedInterface

******************************************************************************/
IMG_RESULT TCONF_AddPostedInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName, 
	IMG_UINT32 nOffset)
{
    DEVIF_sPostIFInfo *psPostedInfo;
	IMG_RESULT rResult;
	
	rResult =  tconf_FindPostedInterface(psTargetConf, pszDeviceName, &psPostedInfo);
	if(rResult == IMG_ERROR_NOT_SUPPORTED)
		return IMG_SUCCESS; // Not using Posted interface so can ignore
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	psPostedInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
	if (psPostedInfo->pszDeviceName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_OUT_OF_MEMORY;
	}
	
	psPostedInfo->ui32PostedIfOffset = nOffset;
	return IMG_SUCCESS;
}

#define BUFFER_SIZE (1024)


/*!
******************************************************************************

 @Function				TCONF_SetWrapperControlInfo

******************************************************************************/
IMG_RESULT TCONF_SetWrapperControlInfo(
	TCONF_sTargetConf* psTargetConf,
	const DEVIF_sWrapperControlInfo *psWrapSetup,
	eTAL_Twiddle eMemTwiddle,
	IMG_BOOL bVerifyMemWrites
)
{
	TCONF_sDevItem * psDevItem;
	DEVIF_sWrapperControlInfo* psWrapInfo;
	IMG_RESULT rResult = IMG_SUCCESS;

	// Loops setting up all devices
	psDevItem = (TCONF_psDevItem)LST_first(&psTargetConf->sDevItemList);


	while (psDevItem != IMG_NULL)
    {
		psWrapInfo = &psDevItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo;
		IMG_MEMCPY(psWrapInfo, psWrapSetup, sizeof(DEVIF_sWrapperControlInfo));
		if (psTargetConf->pszOverride)
		{
			// Set the overriden interface
			rResult = TCONF_ProcessOverride(psTargetConf, &psWrapInfo->eDevifType);
			if (rResult != IMG_SUCCESS)
				return rResult;
		}
		psDevItem->sTalDeviceCB.eTALTwiddle = eMemTwiddle;
		psDevItem->sTalDeviceCB.bVerifyMemWrites = bVerifyMemWrites;
    	psDevItem = (TCONF_psDevItem)LST_next(psDevItem);
    }
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				tconf_FindBaseNameInfo

******************************************************************************/
IMG_RESULT tconf_FindBaseNameInfo(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	DEVIF_sBaseNameInfo ** ppsBaseNameInfo)
{
	TCONF_sDevItem * psDevInfo;
	DEVIF_sFullInfo * psDevifInfo;

	*ppsBaseNameInfo = IMG_NULL;
	
	psDevInfo = tconf_FindDevice(psTargetConf, pszName);
	if (psDevInfo == IMG_NULL)
	{
		printf("WARNING: Cannot find device %s in config file\n", pszName);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	
	psDevifInfo = &psDevInfo->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup;
	*ppsBaseNameInfo = &psDevifInfo->sBaseNameInfo;
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_AddBaseName

******************************************************************************/
IMG_RESULT TCONF_AddBaseName(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszRegisterName,
	const IMG_CHAR *pszMemoryName)
{
  
	DEVIF_sBaseNameInfo *psBaseNameInfo;
	IMG_RESULT rResult;
	
	IMG_ASSERT(pszDeviceName != NULL && *pszDeviceName != '\0');
	IMG_ASSERT(
		(pszRegisterName != NULL && *pszRegisterName != '\0') ||
		(pszMemoryName != NULL && *pszMemoryName != '\0')
	);
	
	rResult =  tconf_FindBaseNameInfo(psTargetConf, pszDeviceName, &psBaseNameInfo);
	if (rResult != IMG_SUCCESS)
		return rResult;

	psBaseNameInfo->pszDeviceName = IMG_STRDUP(pszDeviceName);
	if (psBaseNameInfo->pszDeviceName == NULL)
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_OUT_OF_MEMORY;
	}
	
	if (pszMemoryName)
	{
		if (psBaseNameInfo->pszMemBaseName)
			return IMG_ERROR_ALREADY_COMPLETE;
		psBaseNameInfo->pszMemBaseName = IMG_STRDUP(pszMemoryName);
		if (psBaseNameInfo->pszMemBaseName == NULL)
			return IMG_ERROR_OUT_OF_MEMORY;
	}

	if (pszRegisterName)
	{
		if (psBaseNameInfo->pszRegBaseName)
			return IMG_ERROR_ALREADY_COMPLETE;
		psBaseNameInfo->pszRegBaseName = IMG_STRDUP(pszRegisterName);
		if (psBaseNameInfo->pszRegBaseName == NULL)
			return IMG_ERROR_OUT_OF_MEMORY;
	}

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				tconf_FindMemSpace

******************************************************************************/
TAL_sMemSpaceInfo* tconf_FindMemSpace(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR* pszMemSpaceName
)
{
	TCONF_sMemSpItem *pMemSpItem;
	pMemSpItem = LST_first(&psTargetConf->sMemSpItemList);
	while (pMemSpItem)
	{
		if (strcmp(pMemSpItem->sTalMemSpaceCB.pszMemSpaceName, pszMemSpaceName) == 0)
			return &pMemSpItem->sTalMemSpaceCB;
		pMemSpItem = (TCONF_sMemSpItem *)LST_next(pMemSpItem);
	}
	return IMG_NULL;
}

/*!
******************************************************************************

 @Function				TCONF_AddPdumpContext

******************************************************************************/
IMG_RESULT TCONF_AddPdumpContext(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszMemSpaceName,
	const IMG_CHAR *pszContextName)
{
	TAL_sMemSpaceInfo *pMemSpaceInfo = tconf_FindMemSpace(psTargetConf, pszMemSpaceName);
	if(pMemSpaceInfo)
	{
		pMemSpaceInfo->pszPdumpContext = IMG_STRDUP(pszContextName);
		return IMG_SUCCESS;
	}
	printf("WARNING Memspace not found %s\n", pszMemSpaceName);
	return IMG_ERROR_DEVICE_NOT_FOUND;
}

/*!
******************************************************************************

 @Function				TCONF_WriteWord32

******************************************************************************/
IMG_BOOL TCONF_WriteWord32(const IMG_CHAR *pszMemSpace, IMG_UINT32 nOffset, IMG_UINT32 nValue)
{
	IMG_HANDLE hMemSpace;

	IMG_ASSERT(pszMemSpace != NULL && *pszMemSpace != '\0');

	hMemSpace = TAL_GetMemSpaceHandle(pszMemSpace);
	if (hMemSpace != NULL)
		return (TALREG_WriteWord32(hMemSpace, nOffset, nValue) == IMG_SUCCESS);

	return IMG_FALSE;
}
