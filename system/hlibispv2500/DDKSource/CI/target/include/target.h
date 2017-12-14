/*!
******************************************************************************
 @file   : target.h

 @brief

 @Author Ray Livesley

 @date   03/06/2003

         <b>Copyright 2003 by Imagination Technologies Limited.</b>\n
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
         Target API Header File.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/


#if !defined (__TARGET_H__)
#define __TARGET_H__

#include <tal.h>


#if defined(__cplusplus)
extern "C" {
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Target functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Target group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function              TARGET_SetConfigFile

 @Description

 The default target config file location is hard-coded. This function will
 override the hard-coded value if it is called prior to TARGET_Initialise().

 @Input     pszFilePath : Path and file name of the target config file.

 @Return    none.

******************************************************************************/
extern IMG_VOID TARGET_SetConfigFile(const IMG_CHAR *pszFilePath);

/**
******************************************************************************
 * @brief Set the structures to use as configuration (instead of using a configuration file)
 *
 * @note If a config file is defined using TARGET_SetConfigFile() it is used in priority when using TAL Normal
 *
 * @Input pStartOfArray start of an array of memory region definitions - similar to TAL light header need
 * @Input uiNElem size of pStartOfArray
 * @Input pDevInfo (TAL_sDeviceInfo *) device information structure with the correct device configuration - copied
 * @note This is only used in TAL Normal and should be a TAL_sDeviceInfo pointer
 *
 * TCP/IP example of pDevInfo configuration:
 * @code void setDeviceInfo_IP(TAL_sDeviceInfo *pDevInfo, IMG_UINT32 port, const char *pszIP)
{
	IMG_MEMSET(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
	pDevInfo->pszDeviceName = "DEVICE_NAME";
	pDevInfo->ui64MemBaseAddr = 0x0000000000;
	pDevInfo->ui64DefMemSize = 0x10000000;

	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType = DEVIF_TYPE_SOCKET;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.ui32HostTargetCycleRatio = 20000;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName = pDevInfo->pszDeviceName;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.ui32PortId = port;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName = (char*)pszIP;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.bUseBuffer = IMG_TRUE;
} @endcode
******************************************************************************
 */
extern void TARGET_SetMemSpaceInfo(TAL_sMemSpaceInfo *pStartOfArray, IMG_SIZE uiNElem, void *pDevInfo);

/*!
******************************************************************************

 @Function				TARGET_LoadConfig

 @Description

 This function is used to load a TAL config file.  This is typeically used
 to force the loading of the config information BEFORE the TAL is used.  In
 the case of the Portablity Framework TAL then this function is used to
 setup the TAL config in the kernel mode driver.

 @Input		None.

 @Return    IMG_RESULT  : This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
extern IMG_RESULT TARGET_LoadConfig(IMG_VOID);


/*!
******************************************************************************

 @Function				TARGET_Initialise

 @Description

 This function is used to initialise the target.

 Typically this function will register the target/system memory spaces
 with the TAL using the pointer to TAL_MemSpaceRegister() passed via
 pfnTAL_MemSpaceRegister.

 @Input		pfnTAL_MemSpaceRegister : A pointer to the TAL_MemSpaceRegister
						  function.

 @Return   IMG_RESULT   : This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
extern IMG_RESULT TARGET_Initialise (
	TAL_pfnTAL_MemSpaceRegister	pfnTAL_MemSpaceRegister
);

/*!
******************************************************************************

 @Function				TARGET_Deinitialise

 @Description

 This function is used to deinitialise the target.

 Typically this function will cleanup memory allocations

 @Return	None.

******************************************************************************/
IMG_VOID TARGET_Deinitialise (IMG_VOID);


#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Target group
 *------------------------------------------------------------------------*/
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#if defined(__cplusplus)
}
#endif

#endif /* __TARGET_H__	*/


