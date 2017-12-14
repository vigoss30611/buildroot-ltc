/**
******************************************************************************
 @file talbase.cpp

 @brief Implementation of the TALBase test class

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 

******************************************************************************/
#include "tal_test.h"

#include <target.h>

#ifdef __TAL_USE_MEOS__
#include <krn.h>

static KRN_SCHEDULE_T g_sKrnScheduler;

#include <system.h>
#endif

extern "C" {

void setDeviceInfo_NULL(TAL_sDeviceInfo *pDevInfo)
{
	memset(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
	pDevInfo->pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->ui64MemBaseAddr = TAL_DEV_OFF;
	pDevInfo->ui64DefMemSize = TAL_DEV_DEF_MEM_SIZE;
	pDevInfo->ui64MemGuardBand = TAL_DEV_DEF_MEM_GUARD;

	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType = DEVIF_TYPE_NULL;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.ui32HostTargetCycleRatio = 20000;
}

void setDeviceInfo_IP(TAL_sDeviceInfo *pDevInfo, IMG_UINT32 port, const char *pszIP)
{
	memset(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
	pDevInfo->pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->ui64MemBaseAddr = TAL_DEV_OFF;
	pDevInfo->ui64DefMemSize = TAL_DEV_DEF_MEM_SIZE;
	pDevInfo->ui64MemGuardBand = TAL_DEV_DEF_MEM_GUARD;

	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType = DEVIF_TYPE_SOCKET;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.ui32HostTargetCycleRatio = 20000;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.ui32PortId = port;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName = (char*)pszIP;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.bUseBuffer = IMG_TRUE;
}

}

/*
 * protected
 */
void TALBase::init(TAL_sMemSpaceInfo *aInfo, size_t nElem)
{
	this->nRegBanks = 0;
	this->nMemory = 0;
	if ( aRegBanks )
	{
	    free(aRegBanks);
	}
	this->aRegBanks = (IMG_HANDLE*)calloc(nElem, sizeof(IMG_HANDLE)); // not optimal but who cares it's testing
	if ( aMemory )
	{
	    free(aMemory);
	}
	this->aMemory = (IMG_HANDLE*)calloc(nElem, sizeof(IMG_HANDLE));
	for ( size_t i = 0 ; i < nElem ; i++ )
	{
		if ( aInfo[i].eMemSpaceType == TAL_MEMSPACE_REGISTER )
		{
			aRegBanks[nRegBanks] = TAL_GetMemSpaceHandle( aInfo[i].pszMemSpaceName );
			nRegBanks++;
		}
		else // memory
		{
			aMemory[nMemory] = TAL_GetMemSpaceHandle( aInfo[i].pszMemSpaceName );
			nMemory++;
		}
	}
}
 
/*
 * public
 */ 
 
TALBase::TALBase(eDevifTypes eInterface)
{
	switch(eInterface)
	{
	case DEVIF_TYPE_NULL:
		setDeviceInfo_NULL(&sDeviceInfo);
		break;
		
	case DEVIF_TYPE_SOCKET:
		setDeviceInfo_IP(&sDeviceInfo, 1234, "localhost");
	}
	nRegBanks = 0;
	nMemory = 0;
	aRegBanks = 0;
	aMemory = 0;
}

TALBase::~TALBase()
{
	finalise(); // in case TAL was not deinit
	if ( aRegBanks )
	{
		free(aRegBanks);
		aRegBanks = 0;
	}
	if ( aMemory )
	{
		free(aMemory);
		aMemory = 0;
	}
}

IMG_RESULT TALBase::initialise(TAL_sMemSpaceInfo *aInfo, size_t nElem)
{
	IMG_RESULT ret;
	setDeviceInfo_NULL(&sDeviceInfo);

#ifdef __TAL_USE_MEOS__
	KRN_reset(&g_sKrnScheduler, NULL, KRN_NO_PRIORITY_LEVELS-1, 0, NULL, 0);
	KRN_startOS("TAL test");
#endif

	TARGET_SetMemSpaceInfo(aInfo, nElem, &sDeviceInfo);
	
	if ( (ret = TALSETUP_Initialise()) != IMG_SUCCESS )
	{
		return ret;
	}
	
	init(aInfo, nElem);
	return ret;
}

IMG_RESULT TALBase::initialise(const char *pszFilename)
{
	TARGET_SetConfigFile(pszFilename);
	/// @ add something to find all registers and memory
	//init();
	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT TALBase::finalise()
{
    IMG_RESULT ret;
    ret = TALSETUP_Deinitialise();

#if defined(__TAL_USE_MEOS__) 
    //KRN_stopOS(); // deadlocks 
#endif

    return ret;
}
 
