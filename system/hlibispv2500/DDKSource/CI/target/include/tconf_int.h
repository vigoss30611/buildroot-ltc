/*! 
******************************************************************************
 @file   : tconf_int.h

 @brief	 API for the target config file parser

 @Author Jose Massada

 @date   05/10/2012
 
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
         Target Config File internal functions.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0

******************************************************************************/
#ifndef __TCONF_INT_H__
#define __TCONF_INT_H__

#include "lst.h"
#include "target.h"
#include "tconf.h"
#include "devif_setup.h"
#include "img_types.h"
#include "tal_defs.h"


// Device List
typedef struct TCONF_sDevItem_tag
{	
	LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/
	TAL_sDeviceInfo sTalDeviceCB;
} TCONF_sDevItem, *TCONF_psDevItem;

// Memory Space List
typedef struct TCONF_sMemSpItem_tag
{
	LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/
	TAL_sMemSpaceInfo sTalMemSpaceCB;
} TCONF_sMemSpItem, *TCONF_psMemSpItem;

typedef struct TCONF_sTargetConf_tag
{
	LST_T sDevItemList;			//!< List of device items
	LST_T sMemSpItemList;		//!< List of memspace items

	const IMG_CHAR *pszConfigFile;

	IMG_UINT32 ui32LineNo;
	const IMG_CHAR * pszOverride;

	IMG_BOOL bWrapperControlInfo;
	IMG_BOOL bPciCardInfo;
} TCONF_sTargetConf;

/*!
******************************************************************************

 @Function				TCONF_AddDevice

******************************************************************************/
IMG_RESULT TCONF_AddDevice(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT64 nBaseAddr);

/*!
******************************************************************************

 @Function				TCONF_SetDeviceDefaultMem

******************************************************************************/
IMG_RESULT TCONF_SetDeviceDefaultMem(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName,
	IMG_UINT64 nBaseAddr,
	IMG_UINT64 nSize,
	IMG_UINT64 nGuardBand);

/*!
******************************************************************************

 @Function				TCONF_SetDeviceFlags

******************************************************************************/
IMG_RESULT TCONF_SetDeviceFlags(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT32 nFlags);

/*!
******************************************************************************

 @Function				TCONF_ClearDeviceFlags

******************************************************************************/
IMG_RESULT TCONF_ClearDeviceFlags(TCONF_sTargetConf* psTargetConf, const IMG_CHAR *pszName, IMG_UINT32 nFlags);

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
	IMG_UINT32 nIRQ);

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
	IMG_UINT64 nGuardBand);

/*!
******************************************************************************

 @Function				TCONF_AddDeviceSubDevice

******************************************************************************/
IMG_RESULT TCONF_AddDeviceSubDevice(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDevName,
	const IMG_CHAR *pszSubName,
	IMG_CHAR chType,
	IMG_UINT64 nBaseAddr, 
	IMG_UINT64 nSize);

/*!
******************************************************************************

 @Function				TCONF_AddPciInterface

******************************************************************************/
IMG_RESULT TCONF_AddPciInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName, 
	IMG_UINT32 nVendorId, 
	IMG_UINT32 nDeviceId);

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
	IMG_BOOL   bAddDevBaseAddr);

/*!
******************************************************************************

 @Function				TCONF_AddPciInterfaceMemoryBase

******************************************************************************/
IMG_RESULT TCONF_AddPciInterfaceMemoryBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_UINT32 nBar,
	IMG_UINT32 nBaseAddr,
	IMG_UINT32 nSize);

/*!
******************************************************************************

 @Function				TCONF_AddBurnMem

******************************************************************************/
IMG_RESULT TCONF_AddBurnMem(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_CHAR chDeviceId,
	IMG_UINT32 nOffset,
	IMG_UINT32 nSize);

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
	IMG_BOOL bBuffering);

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
	const IMG_CHAR *pszReceiveFile);

/*!
******************************************************************************

 @Function				TCONF_AddDashInterface

******************************************************************************/
IMG_RESULT TCONF_AddDashInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	IMG_UINT32 nId);


/*!
******************************************************************************

 @Function				TCONF_SetDashInterfaceDeviceBase

******************************************************************************/
IMG_RESULT TCONF_SetDashInterfaceDeviceBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName, 
	IMG_UINT32 nBaseAddr, 
	IMG_UINT32 nSize);

/*!
******************************************************************************

 @Function				TCONF_SetDashInterfaceMemoryBase

******************************************************************************/
IMG_RESULT TCONF_SetDashInterfaceMemoryBase(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszName, 
	IMG_UINT32 nBaseAddr, 
	IMG_UINT32 nSize);

/*!
******************************************************************************

 @Function				TCONF_AddDirectInterface

******************************************************************************/
IMG_RESULT TCONF_AddDirectInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszParentDeviceName);

/*!
******************************************************************************

 @Function				TCONF_AddPostedInterface

******************************************************************************/
IMG_RESULT TCONF_AddPostedInterface(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName, 
	IMG_UINT32 nOffset);

/*!
******************************************************************************

 @Function				TCONF_SetWrapperControlInfo

******************************************************************************/
IMG_RESULT TCONF_SetWrapperControlInfo(
	TCONF_sTargetConf* psTargetConf,
	const DEVIF_sWrapperControlInfo *psWrapSetup,
	eTAL_Twiddle eMemTwiddle,
	IMG_BOOL bVerifyMemWrites);


/*!
******************************************************************************

 @Function				TCONF_AddBaseName

******************************************************************************/
IMG_RESULT TCONF_AddBaseName(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszDeviceName,
	const IMG_CHAR *pszRegisterName,
	const IMG_CHAR *pszMemoryName);

/*!
******************************************************************************

 @Function				TCONF_AddPdumpContext

******************************************************************************/
IMG_RESULT TCONF_AddPdumpContext(
	TCONF_sTargetConf* psTargetConf,
	const IMG_CHAR *pszMemSpaceName,
	const IMG_CHAR *pszContextName);

/*!
******************************************************************************

 @Function				TCONF_WriteWord32

******************************************************************************/
IMG_BOOL TCONF_WriteWord32(const IMG_CHAR *pszMemSpace, IMG_UINT32 nOffset, IMG_UINT32 nValue);

#endif /* __TCONF_INT_H__ */
