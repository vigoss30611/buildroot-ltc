/*******************************************************************************
<header>
* Name         : registry.c
* Title        : platform related function header
* Author(s)    : Imagination Technologies
* Created      : 16th Jan 2008
*
* Copyright    : 2002-2008 by Imagination Technologies Limited.
*                All rights reserved.  No part of this software, either
*                material or conceptual may be copied or distributed,
*                transmitted, transcribed, stored in a retrieval system
*                or translated into any human or computer language in any
*                form by any means, electronic, mechanical, manual or
*                other-wise, or disclosed to third parties without the
*                express written permission of Imagination Technologies
*                Limited, Unit 8, HomePark Industrial Estate,
*                King's Langley, Hertfordshire, WD4 8LZ, U.K.
*
* Description  : Registry access functions
*
* Platform     : Windows Vista
*
</header>
*******************************************************************************/
/******************************************************************************
Modifications :-

$Log: registry.c $
Revision 1.9  2010/03/05 14:58:19  Mike.Brassington
x64 support added
Revision 1.8  2009/10/20 16:23:49  richard.smithies
Build fix for basic type change
Revision 1.7  2009/05/12 14:16:56  Mike.Brassington
QAC warning fix
Revision 1.6  2009/05/11 08:27:13  Mike.Brassington
QAC warnings fixes
Revision 1.5  2009/01/26 14:46:18  Mike.Brassington
Use of C basic types replaced with IMG equivalents
Revision 1.4  2008/07/17 16:28:28  donald.scorgie
Move to new Services 4 version

*****************************************************************************/

#pragma warning (push, 3)
#include <ntddk.h>
#include <windef.h>
#include <stdlib.h>
#pragma warning (pop)

//#include "services.h"
//#include "regpaths.h"
//#include "pvr_debug.h"

#include "img_types.h"

#define MAX_REG_STRING_SIZE				 128
#define POWERVR_SERVICES_KEY			"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\imgpcidd\\"
#define PVR_ASSERT( _bye_ )
#define PVR_DPF( _bye_ )

#define strcat_s( a,b,c ) strcat( a , c )
#define strcpy_s( a,b,c ) strcpy( a , c )

/* Maximum length of registry key or value name */
#define WCE_MAX_REGNAME_SIZE MAX_PATH

/*****************************************************************************
 FUNCTION	: AToL

 PURPOSE	:

 PARAMETERS	:

 RETURNS	:
*****************************************************************************/
DWORD AtoL(IMG_CHAR *szIn)
{
	IMG_INT     iLen = 0;
	IMG_UINT32	ui32Value = 0;
	IMG_UINT32	ui32Digit=1;
	IMG_UINT32	ui32Base=10;
	IMG_INT     iPos;
	IMG_CHAR    bc;

	/*
		Get len of string
	*/
	while (szIn[iLen] > 0)
	{
		iLen ++;
	}

	if (iLen == 0)
	{
		return (0);
	}

	/*
		See if we have an 'x' or 'X' before the number to make it a hex number
	*/
	iPos=0;

	while (szIn[iPos] == '0')
	{
		iPos++;
	}

	if (szIn[iPos] == '\0')
	{
		return 0;
	}

	if (szIn[iPos] == 'x' || szIn[iPos] == 'X')
	{
		ui32Base=16;
		szIn[iPos]='0';
	}

	/*
		go through string from right (least significant) to left
	*/
	for (iPos = iLen - 1; iPos >= 0; iPos --)
	{
		bc = szIn[iPos];


		if ( (bc >= 'a') && (bc <= 'f') && ui32Base == 16)
		{
			/*
				handle lower case a-f
			*/
			bc -= 'a' - 0xa;
		}
		else
		{
			if ( (bc >= 'A') && (bc <= 'F') && ui32Base == 16)
			{
				/*
					handle upper case A-F
				*/
				bc -= 'A' - 0xa;
			}
			else
			{
				/*
					if char out of range, return 0
				*/
				if ((bc >= '0') && (bc <= '9'))
				{
					bc -= '0';
				}
				else
				{
					return (0);
				}
			}
		}

		ui32Value += (IMG_UINT32)bc  * ui32Digit;

		ui32Digit = ui32Digit * ui32Base;
	}

	return (ui32Value);
}

/**************************************************************************
 * Function Name  : OSReadRegistryBinData
 * Inputs         : pszPartialKey - partial registry key
 *				  : pszValueName - value name to read
 *				  : pvData - ptr to buf to reveive the data
 *				  : pui32DataSize - ptr to IMG_UINT32 that has size of buffer
 * Outputs        : pui32DataSize - gets size of data returned on success else size of buffer needed
 * Returns        : TRUE on success, else FALSE and pdwDataSize has size of buf needed
 * Globals Used   : None
 * Description    : Function that reads the registry
 **************************************************************************/
IMG_BOOL OSReadRegistryBinData(IMG_UINT32 ui32DevCookie, PCHAR pszPartialKey, PCHAR pszValueName, PVOID pvData, IMG_UINT32 *pui32DataSize)
{
	IMG_UINT32						ui32CallsBufSize;
	HANDLE							hKey;
	NTSTATUS						sErr;
	OBJECT_ATTRIBUTES				sObjAttribs;
	PKEY_VALUE_PARTIAL_INFORMATION	psInfo;
	UNICODE_STRING					sUKey;
	UNICODE_STRING					sUValue;
	ANSI_STRING						sAnsiKey;
	ANSI_STRING						sAnsiValue;
	CHAR							szKey[200] = POWERVR_SERVICES_KEY;
	IMG_BOOL						bRetCode = IMG_FALSE;
	KIRQL eOldIRQ, eNewIRQL;
	IMG_BOOL bRetIRQL = IMG_FALSE;

	eOldIRQ = KeGetCurrentIrql();

	if (eOldIRQ != PASSIVE_LEVEL)
	{
		/*
		 * Lower the IRQL if we are at APC level (which can happen when
		 * creating the device with a mutex grabbed
		 */
		PVR_ASSERT(eOldIRQ == APC_LEVEL);
		bRetIRQL = IMG_TRUE;
		KeLowerIrql(PASSIVE_LEVEL);
	}

	PVR_UNREFERENCED_PARAMETER(ui32DevCookie);

	// Null pointer here not allowed
	PVR_ASSERT(pui32DataSize);

	if (pvData)
	{
		// Caller is requesting data
		PVR_ASSERT(*pui32DataSize);			/* PRQA S 0505 */ /* ignore apparent nul ptr */
		ui32CallsBufSize = *pui32DataSize;
	}
	else
	{
		// Caller is requesting size
		ui32CallsBufSize = 0;
	}

	/* Zero output buffer size just in case there is any error */
	*pui32DataSize=0;

	/* Construct full registry path */
	if (strlen(pszPartialKey) + strlen(szKey) >= sizeof(szKey))
	{
		KdPrint(("OSReadRegistryBinData : 'szKey' buffer not big enough"));
		if (bRetIRQL)
		{
			KeRaiseIrql(eOldIRQ, &eNewIRQL);
		}
		return IMG_FALSE;
	}
	strcat_s(szKey, 200, pszPartialKey);
	RtlInitAnsiString(&sAnsiKey, szKey);
	sErr = RtlAnsiStringToUnicodeString( &sUKey, &sAnsiKey, TRUE );
	if (NT_SUCCESS(sErr) == FALSE)
	{
		if (bRetIRQL)
		{
			KeRaiseIrql(eOldIRQ, &eNewIRQL);
		}
		return IMG_FALSE;
	}

	RtlInitAnsiString( &sAnsiValue, pszValueName );
	sErr = RtlAnsiStringToUnicodeString( &sUValue, &sAnsiValue, TRUE );
	if (NT_SUCCESS(sErr) == FALSE)
	{
		RtlFreeUnicodeString( &sUKey );
		if (bRetIRQL)
		{
			KeRaiseIrql(eOldIRQ, &eNewIRQL);
		}
		return IMG_FALSE;
	}

	psInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + ui32CallsBufSize);

	if (psInfo == NULL)
	{
		RtlFreeUnicodeString( &sUKey );
		RtlFreeUnicodeString( &sUValue );
		if (bRetIRQL)
		{
			KeRaiseIrql(eOldIRQ, &eNewIRQL);
		}
		return IMG_FALSE;
	}

	/* QAC misunderstands this function */
	/* PRQA S 3198 1 */
	InitializeObjectAttributes( &sObjAttribs, &sUKey, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );
	
	/* override QAC warning about KEY_READ */
	sErr = ZwOpenKey( &hKey, KEY_READ, &sObjAttribs );	/* PRQA S 4130 */
	if (NT_SUCCESS(sErr) == TRUE)
	{
		ULONG ulResultLength = 0;
		// Read data into our locally allocd buffer
		sErr = ZwQueryValueKey( hKey, &sUValue, KeyValuePartialInformation, psInfo, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + ui32CallsBufSize, &ulResultLength);
		ZwClose(hKey);

		if ((NT_SUCCESS(sErr) == TRUE) && (ui32CallsBufSize >= psInfo->DataLength))
		{
			// Success and enough space for result
			PVR_ASSERT(pvData);
			memcpy(pvData, psInfo->Data, psInfo->DataLength);
			*pui32DataSize = psInfo->DataLength;
			bRetCode = IMG_TRUE;
		}
		else if ((sErr == STATUS_BUFFER_OVERFLOW) && (psInfo->DataLength > ui32CallsBufSize))		// PRQA S 0274
		{
			// Callers buffer too small, so advise of correct size
			*pui32DataSize = psInfo->DataLength;
		}
	}

	ExFreePool( psInfo );
	RtlFreeUnicodeString( &sUKey );
	RtlFreeUnicodeString( &sUValue );
	if (bRetIRQL)
	{
		KeRaiseIrql(eOldIRQ, &eNewIRQL);
	}
	return bRetCode;
}

/**************************************************************************
 * Function Name  : OSWriteRegistryBinData
 * Inputs         : pszPartialKey - partial registry key
 *				  : pszValueName - value name to read
 *				  : pvData - ptr to buf to reveive the data
 *				  : ui32DataSize - ptr to IMG_UINT32 that has size of buffer
 * Outputs        : None
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Function that writes the registry
 **************************************************************************/
IMG_BOOL OSWriteRegistryBinData(IMG_UINT32 ui32DevCookie, PCHAR pszPartialKey, PCHAR pszValueName, PVOID pvData, IMG_UINT32 ui32DataSize)
{
	HANDLE							hKey;
	NTSTATUS						sErr;
	OBJECT_ATTRIBUTES				sObjAttribs;
	PKEY_VALUE_PARTIAL_INFORMATION	psInfo;
	UNICODE_STRING					sUKey;
	UNICODE_STRING					sUValue;
	ANSI_STRING						sAnsiKey;
	ANSI_STRING						sAnsiValue;
	IMG_BOOL						bRetCode = IMG_FALSE;
	IMG_CHAR						szKey[200] = POWERVR_SERVICES_KEY;

	if (ui32DevCookie);

	/* Construct full key path */
	if (strlen(pszPartialKey) + strlen(szKey) >= sizeof(szKey))
	{
		KdPrint(("OSReadRegistryString : 'szKey' buffer not big enough"));
		return IMG_FALSE;
	}
	strcat_s(szKey, 200, pszPartialKey);

	/* Convert key, value and output string into unicode */
	RtlInitAnsiString(&sAnsiKey, szKey);
	sErr = RtlAnsiStringToUnicodeString(&sUKey, &sAnsiKey, TRUE);
	if (NT_SUCCESS(sErr) == FALSE)
	{
		return IMG_FALSE;
	}

	RtlInitAnsiString(&sAnsiValue, pszValueName);
	sErr = RtlAnsiStringToUnicodeString(&sUValue, &sAnsiValue, TRUE);
	if (NT_SUCCESS(sErr) == FALSE)
	{
		RtlFreeUnicodeString(&sUKey);
		return IMG_FALSE;
	}

	psInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + MAX_REG_STRING_SIZE);
	if (psInfo == NULL)
	{
		RtlFreeUnicodeString(&sUKey);
		RtlFreeUnicodeString(&sUValue);
		return IMG_FALSE;
	}

	/* QAC misunderstands this function */
	/* PRQA S 3198 1 */
	InitializeObjectAttributes(&sObjAttribs, &sUKey, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	sErr=ZwCreateKey(&hKey,
	                 /* override QAC warning about MASKS */
					 GENERIC_ALL | KEY_ALL_ACCESS | KEY_READ | KEY_WRITE,	/* PRQA S 4130 */
					 &sObjAttribs,
                     0,
                     (PUNICODE_STRING) NULL,
                     REG_OPTION_NON_VOLATILE,
                     NULL);

	if (NT_SUCCESS(sErr) == TRUE)
	{
		sErr=ZwSetValueKey(hKey, &sUValue, 0, REG_BINARY, pvData, ui32DataSize);
		if (NT_SUCCESS(sErr) == TRUE)
		{
			bRetCode=IMG_TRUE;
		}

		ZwClose(hKey);
	}

	ExFreePool(psInfo);
	RtlFreeUnicodeString(&sUKey);
	RtlFreeUnicodeString(&sUValue);
	return bRetCode;
}

/**************************************************************************
 * Function Name  : OSReadRegistryString
 * Inputs         : pszKey - registry key to open
					pszValue - registry value to read
					dwOutBufSize - size of output buffer in bytes
 * Outputs        : pszOutBuf - buf to reveive the registry string
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Read the registry
 **************************************************************************/
BOOL OSReadRegistryString(IMG_UINT32 ui32DevCookie, PCHAR pszKey, PCHAR pszValue, PCHAR pszOutBuf, IMG_UINT32 ui32OutBufSize)
{
	ANSI_STRING		sAnsiRegStr;
	UNICODE_STRING	sUnicodeRegStr;
	PWCHAR			pszwString;
	IMG_UINT32		ui32Size=ui32OutBufSize*2;
	NTSTATUS		sErr;
	BOOL			bRet=FALSE;

	/* Alloc buffer for unicode string that */
	if ((pszwString=ExAllocatePoolWithTag(PagedPool, ui32OutBufSize*2, 'gPMK')) == NULL)
	{
		KdPrint(("OSReadRegistryString : Failed to alloc buffer for unicode string\n"));
		return FALSE;
	}

	/* Read registry */
	if (OSReadRegistryBinData(ui32DevCookie, pszKey, pszValue, (PVOID)pszwString, &ui32Size) == TRUE)
	{
		/* Now convert unicode string to ansi string */
		RtlInitUnicodeString(&sUnicodeRegStr, (PCWSTR)pszwString);
		sErr=RtlUnicodeStringToAnsiString(&sAnsiRegStr, &sUnicodeRegStr, TRUE);
		if (NT_SUCCESS(sErr) == TRUE)
		{
			strcpy_s(pszOutBuf, ui32OutBufSize, sAnsiRegStr.Buffer);
			RtlFreeAnsiString(&sAnsiRegStr);
			bRet=TRUE;
		}
	}

	ExFreePool(pszwString);
	return bRet;
}

/**************************************************************************
 * Function Name  : OSWriteRegistryString
 * Inputs         : pszKey - registry key to open
					pszValue - registry value to writen
					pszBuf - buf to reveive the registry string
 * Outputs        : None
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Write the registry
 **************************************************************************/
IMG_BOOL OSWriteRegistryString(IMG_UINT32 ui32DevCookie, PCHAR pszKey, PCHAR pszValue, PCHAR pszBuf)
{
	UNICODE_STRING	sUData;
	ANSI_STRING		sAnsiData;
	NTSTATUS		sErr;
	IMG_BOOL		bRet;

	/* We need to write unicode string to the registry as the read expects it */
	RtlInitAnsiString(&sAnsiData, pszBuf);
	sErr = RtlAnsiStringToUnicodeString(&sUData, &sAnsiData, TRUE);
	if (NT_SUCCESS(sErr) == FALSE)
	{
		return IMG_FALSE;
	}

	/* Write reagisty */
	bRet=OSWriteRegistryBinData(ui32DevCookie,pszKey, pszValue, sUData.Buffer, (IMG_UINT32)((strlen(pszBuf) +1) * 2));

	/* Free string */
	RtlFreeUnicodeString(&sUData);

	return bRet;
}

/**************************************************************************
 * Function Name  : OSReadRegistryDWORDFromString
 * Inputs         : pszKey - registry key to open
					pszValue - registry value to read
					dwOutBufSize - size of output buffer in bytes
 * Outputs        : pszOutBuf - buf to reveive the registry string
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Read the registry
 **************************************************************************/
BOOL OSReadRegistryDWORDFromString(IMG_UINT32 ui32DevCookie, PCHAR pszKey, PCHAR pszValue, IMG_UINT32 *pui32Data)
{
	ANSI_STRING		sAnsiRegStr;
	UNICODE_STRING	sUnicodeRegStr;
	PWCHAR			pszwString;
	IMG_UINT32		ui32Size=60;
	NTSTATUS		sErr;
	BOOL			bRet=FALSE;

	/* Alloc buffer for unicode string that */
	if ((pszwString=ExAllocatePoolWithTag(PagedPool, 60, 'gPMK')) == NULL)
	{
		KdPrint(("OSReadRegistryString : Failed to alloc buffer for unicode string\n"));
		return FALSE;
	}

	/* Read registry */
	if (OSReadRegistryBinData(ui32DevCookie, pszKey, pszValue, (PVOID)pszwString, &ui32Size) == TRUE)
	{
		/* Now convert unicode string to ansi string */
		RtlInitUnicodeString(&sUnicodeRegStr, (PCWSTR)pszwString);
		sErr=RtlUnicodeStringToAnsiString(&sAnsiRegStr, &sUnicodeRegStr, TRUE);
		if (NT_SUCCESS(sErr) == TRUE)
		{
			*pui32Data = AtoL(sAnsiRegStr.Buffer);
			RtlFreeAnsiString(&sAnsiRegStr);
			bRet=TRUE;
		}
	}

	ExFreePool(pszwString);
	return bRet;
}

/*****************************************************************************
 End of file (registry.c)
*****************************************************************************/
