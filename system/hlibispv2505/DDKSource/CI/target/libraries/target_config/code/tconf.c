/*!
********************************************************************************
 @file   : tconf.c

 @brief

 @Author Ray Livesley

 @date   17/10/2006

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
         Functions used to read a Target Config File.

 <b>Platform:</b>\n
	     Platform Independent
		  
 @Version
	     1.0

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gzip_fileio.h"
#include "tconf_int.h"
#include "tconf_txt.h"
#include "tconf_xml.h"
#include "tconf.h"
#include "img_errors.h"

static const IMG_CHAR* gpszOverride = IMG_NULL;



/*!
******************************************************************************

 @Function				SeperateDirFromFile

******************************************************************************/
static IMG_CHAR *SeperateDirFromFile(IMG_CHAR *pszDirWithPath)
{
	IMG_CHAR pszDelim[] = "\\/";
    IMG_CHAR * filename = IMG_NULL, *pszSubStr, *pszStr;
	IMG_UINT32 ui32DirLen;

	pszStr = IMG_STRDUP(pszDirWithPath);
	pszSubStr = strtok(pszStr, pszDelim);
	while (pszSubStr != IMG_NULL)
	{
		filename = pszSubStr;
		pszSubStr = strtok(IMG_NULL, pszDelim);
	}
	ui32DirLen = (IMG_UINT32)strlen(pszDirWithPath) - (IMG_UINT32)strlen(filename);
	IMG_FREE(pszStr);
    
    if(ui32DirLen > 0) // In case original pszDirWithPath was a filename without "\\/" delimiters, ui32DirLen is now 0. Avoid writing outside string array in this case (brn28580).
    {
	    pszDirWithPath [ ui32DirLen - 1 ] = 0;
    }
	return pszDirWithPath + ui32DirLen;
}

/*!
******************************************************************************

 @Function				TCONF_TargetConfigure

******************************************************************************/
IMG_RESULT TCONF_TargetConfigure(
	const IMG_CHAR *pszConfigFile,
	TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegister,
	IMG_HANDLE *phTargetSetup
)
{
	TCONF_sTargetConf* psTargetConf = (TCONF_sTargetConf*)IMG_CALLOC(sizeof(TCONF_sTargetConf), 1);
	IMG_UINTPTR nLength;
	
	if(psTargetConf == IMG_NULL) return IMG_ERROR_OUT_OF_MEMORY;
    /* Set global name of config file...*/
	if (pszConfigFile == IMG_NULL)
	{
		printf("ERROR: No target configuration file supplied\n");
		return IMG_ERROR_FATAL;
	}

	*phTargetSetup = psTargetConf;
	
	psTargetConf->pszConfigFile = IMG_STRDUP(pszConfigFile);
	if (psTargetConf->pszConfigFile == NULL)
	{
		printf("ERROR: not enough memory\n");
		return IMG_ERROR_OUT_OF_MEMORY;
	}
	
	psTargetConf->pszOverride = gpszOverride;
	
    printf("INFO: Using target config file \"%s\"\n", pszConfigFile);

	nLength = strlen(pszConfigFile);
#ifndef TCONF_NOXML
	if (nLength >= 4 &&
		pszConfigFile[nLength - 4] == '.' &&
		pszConfigFile[nLength - 3] == 'x' &&
		pszConfigFile[nLength - 2] == 'm' &&
		pszConfigFile[nLength - 1] == 'l')
	{
		return TCONF_XML_TargetConfigure(psTargetConf, pfnTAL_MemSpaceRegister);
	}
#endif
	return TCONF_TXT_TargetConfigure(psTargetConf, pfnTAL_MemSpaceRegister);
}

/*!
******************************************************************************

 @Function				TCONF_Deinitialise

******************************************************************************/
IMG_VOID TCONF_Deinitialise(IMG_HANDLE hTargetSetup)
{
	TCONF_psDevItem					psDeviceItem;
	TCONF_psMemSpItem				psMemSpItem;
	TAL_psSubDeviceCB				psSubDevList, psSubDevNext;
	TCONF_sTargetConf *				psTargetConf = (TCONF_sTargetConf*)hTargetSetup;
	
	if (psTargetConf->pszConfigFile != IMG_NULL)
		IMG_FREE((void*)psTargetConf->pszConfigFile);

	/* Locate device CB...*/
	psDeviceItem = (TCONF_psDevItem)LST_removeHead(&psTargetConf->sDevItemList);
    while (psDeviceItem != IMG_NULL)
    {
		// free all sub-device blocks
		psSubDevList = psDeviceItem->sTalDeviceCB.psSubDeviceCB;
		while(psSubDevList)
		{
			psSubDevNext = psSubDevList->pNext;
			if (psSubDevList->pszMemName) IMG_FREE(psSubDevList->pszMemName);
			if (psSubDevList->pszRegName) IMG_FREE(psSubDevList->pszRegName);
			IMG_FREE(psSubDevList);
			psSubDevList = psSubDevNext;
		}
		// Free strings setup in devif control blocks
		if(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName)
			IMG_FREE(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName);
		switch(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType)
		{
		case DEVIF_TYPE_PCI:
		case DEVIF_TYPE_BMEM:
		case DEVIF_TYPE_PDUMP1:
		case DEVIF_TYPE_DIRECT:
		case DEVIF_TYPE_NULL:
		case DEVIF_TYPE_POSTED:
			break;
		case DEVIF_TYPE_SOCKET:
			if(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName != IMG_NULL)
			  IMG_FREE(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName);
			break;
		
		case DEVIF_TYPE_DASH:
			if(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDashIFInfo.pcDashName != IMG_NULL)
			  IMG_FREE(psDeviceItem->sTalDeviceCB.sDevIfDeviceCB.sDevIfSetup.u.sDashIFInfo.pcDashName);
			break;
		default:
			IMG_ASSERT(IMG_FALSE);
		}

		IMG_FREE(psDeviceItem->sTalDeviceCB.pszDeviceName);
		IMG_FREE(psDeviceItem);

       psDeviceItem = (TCONF_psDevItem)LST_removeHead(&psTargetConf->sDevItemList);
    }

    psMemSpItem = (TCONF_psMemSpItem)LST_removeHead(&psTargetConf->sMemSpItemList);
    while (psMemSpItem != IMG_NULL)
    {
		// free all sub-device blocks
    	IMG_FREE(psMemSpItem->sTalMemSpaceCB.pszMemSpaceName);
		IMG_FREE(psMemSpItem);
    	psMemSpItem = (TCONF_psMemSpItem)LST_removeHead(&psTargetConf->sMemSpItemList);
    }

	if(gpszOverride)
		IMG_FREE((void*)gpszOverride);
	IMG_FREE(psTargetConf);
}

/*!
******************************************************************************

 @Function				TCONF_SaveConfig
 
******************************************************************************/
IMG_RESULT TCONF_SaveConfig(IMG_HANDLE hTargetSetup, 
							IMG_CHAR *pszPath)
{
	IMG_HANDLE from, to;
	IMG_CHAR ch;
	IMG_CHAR * pszConfigFile, *pszFileName, szNewFile[256];
	TCONF_sTargetConf *	psTargetConf = (TCONF_sTargetConf*)hTargetSetup;
	
	pszConfigFile = IMG_STRDUP(psTargetConf->pszConfigFile);
	pszFileName = SeperateDirFromFile(pszConfigFile);
	sprintf(szNewFile, "%s/%s", pszPath, pszFileName);

	/* open source file */
	if (IMG_FILEIO_OpenFile(psTargetConf->pszConfigFile, "rb", &from, IMG_FALSE) != IMG_SUCCESS);
	{
		// First check for gzipped version of file
		IMG_CHAR* pszZippedFile;
		pszZippedFile = IMG_MALLOC(strlen(psTargetConf->pszConfigFile) + 4);
		sprintf(pszZippedFile, "%s.gz", psTargetConf->pszConfigFile);
		if (IMG_FILEIO_OpenFile(pszZippedFile, "rb", &from, IMG_TRUE) != IMG_SUCCESS)
		{
			printf("ERROR: Failed to open config file ""%s""\n", psTargetConf->pszConfigFile);
			return IMG_ERROR_GENERIC_FAILURE;
		}
		IMG_FREE(pszZippedFile);
	}

	/* open destination file */
	IMG_FILEIO_OpenFile(szNewFile, "wb", &to, IMG_FALSE);
	if( to == NULL )
	{
		printf("ERROR: Failed to open destination config file ""%s""\n", szNewFile);
		return IMG_ERROR_GENERIC_FAILURE;
	}

	/* copy the file */
	while(!IMG_FILEIO_Eof(from))
	{
		ch = (IMG_CHAR)IMG_FILEIO_Getc(from);
		if(IMG_FILEIO_Error(from))
		{
			printf("ERROR: Reading config file ""%s""\n", psTargetConf->pszConfigFile);
			return IMG_ERROR_GENERIC_FAILURE;
		}
		if(!IMG_FILEIO_Eof(from)) IMG_FILEIO_Putc(to, ch);
		
		if(IMG_FILEIO_Error(to)) 
		{
			printf("ERROR: Writing destinations config file ""%s""\n", szNewFile);
			return IMG_ERROR_GENERIC_FAILURE;
		}
	}

	// Close Files
	IMG_FILEIO_CloseFile(from);
	IMG_FILEIO_CloseFile(to);

	TALPDUMP_AddTargetConfigToTDF(pszFileName);
	IMG_FREE(pszConfigFile);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_OverrideWrapper

 ******************************************************************************/
IMG_RESULT TCONF_OverrideWrapper(IMG_CHAR *pcArgs)
{
	
    gpszOverride = IMG_STRDUP (pcArgs);
	if(gpszOverride)
		return IMG_SUCCESS;
	return IMG_ERROR_OUT_OF_MEMORY;
}


