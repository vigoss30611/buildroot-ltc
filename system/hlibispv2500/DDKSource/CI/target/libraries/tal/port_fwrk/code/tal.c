/*!
******************************************************************************
 @file   : tal.c

 @brief

 @Author Imagination Technologies

 @date   13/06/2008

         <b>Copyright 2008 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
         This file contains a subset of the TAL functions that can be
		 used with the Portablity Framework.

 \n<b>Platform:</b>\n
	     Platform Independent

******************************************************************************/

#include <tal.h>
#include <tal_intdefs.h>
#include <target.h>
#include <dman_api.h>
#include <wrap_utils.h>
#include <pftal_api.h>
#include <sysos_api_km.h>
#include <sysdev_api_km.h>
#include <rman_api.h>
#include <system.h>
#include <osa.h>

#ifdef WINCE
// IsKModeAddr - check to see if an address is a k-mode only address (From vmlayout.h)
__inline BOOL IsKModeAddr (DWORD dwAddr) { return (int) dwAddr < 0; }
#endif

#define PFTAL_TYPE_ID    (0xC0C00001)    /*!< RMAN type for global alloc.            */

static IMG_BOOL		gbInitialised = IMG_FALSE;		//!< IMG_TRUE when initilaised

//static IMG_UINT32   gui32AppFlags = 0;				//<! Application flags for controling the wrapper

/*!
******************************************************************************
 This structure contains registered device information.
******************************************************************************/
typedef struct
{
	LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/

	IMG_CHAR *			pszDeviceName;			//<! Device name
	IMG_UINT32			ui32DevFlags;			//<! Device flags
	IMG_VOID *			pvRegCpuKmAddr;			//<! Register block address
	IMG_UINT32 			ui32RegSize;			//<! Size of register block
	LST_T				sMemSpaceList;			//<! List of memory spaces
	IMG_HANDLE			hSysDevHandle;			//<! SYSDEV handle
	IMG_UINT32			ui32ConnId;				//<! Connection id
	IMG_UINT32			ui32DeviceId;			//<! Device id
	IMG_UINT32			ui32WrapUAttachId;		//<! Attachment id

} PFTAL_sRegDevInfo;

/*!
******************************************************************************
 This structure contains registered memory space information.
******************************************************************************/
typedef struct
{
	LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/

	PFTAL_sRegDevInfo *			psRegDevInfo;			//<! Pointer to the device
	IMG_CHAR *					pszMemSpaceName;		//<! Memory space name flags
	TAL_eMemSpaceType			eMemSpaceType;			//<! Type of mempry space
	IMG_UINT64					ui64BaseOffset;			//<! Base offset
	IMG_UINT64					ui64Size;				//<! Size (in bytes)
	volatile IMG_UINT32 *		pui32RegisterSpace;		//<! Pointer to register base
	IMG_UINT32					ui32IntNum;				//<! Interrupt number
	IMG_UINT32					aui32intmask[TAL_NUM_INT_MASKS];//!< The interrupt masks
	TAL_pfnCheckInterruptFunc   pfnCheckInterruptFunc;	//!< Pointer to interupt check function
	IMG_VOID *					pCallbackParameter;		//!<  Check function callback param

} PFTAL_sRegMemSpaceInfo;

static LST_T		gsRegDevList;					//<! List of devices.
#if !defined (IMG_KERNEL_MODULE)
static IMG_HANDLE ghDevListMutex;					//!< Used to lock the device list
#endif
/*!
******************************************************************************

 @Function              pftal_fnAlloc

******************************************************************************/
static
IMG_RESULT pftal_fnAlloc (
	IMG_VOID *				pvAllocInfo,
    IMG_VOID **				ppvParam
)
{
	/* Initilise the list. */
	LST_init(&gsRegDevList);

	return IMG_SUCCESS;
}
/*!
******************************************************************************

 @Function              pftal_fnFree

******************************************************************************/
static
IMG_VOID pftal_fnFree (
    IMG_VOID *                  pvParam
)
{
	PFTAL_sRegDevInfo *			psRegDevInfo;
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo;

#if !defined (IMG_KERNEL_MODULE)
    OSA_CritSectLock(ghDevListMutex);
#endif
	psRegDevInfo = LST_removeHead(&gsRegDevList);
	while (psRegDevInfo != IMG_NULL)
	{
		psRegMemSpaceInfo = LST_removeHead(&psRegDevInfo->sMemSpaceList);
		while (psRegMemSpaceInfo != IMG_NULL)
		{
			IMG_FREE(psRegMemSpaceInfo->pszMemSpaceName);
			IMG_FREE(psRegMemSpaceInfo);

			psRegMemSpaceInfo = LST_removeHead(&psRegDevInfo->sMemSpaceList);
		}

#if defined (IMG_KERNEL_MODULE)
		SYSDEVKM_CloseDevice(psRegDevInfo->hSysDevHandle);
#endif
		IMG_FREE(psRegDevInfo->pszDeviceName);
		IMG_FREE(psRegDevInfo);

		psRegDevInfo = LST_removeHead(&gsRegDevList);
	}
#if !defined (IMG_KERNEL_MODULE)
    OSA_CritSectUnlock(ghDevListMutex);
#endif
}

/*!
******************************************************************************

 @Function				PFTAL_MemSpaceRegister

******************************************************************************/
IMG_RESULT PFTAL_MemSpaceRegister(
	IMG_CHAR *					pszDeviceName,
	IMG_UINT32					ui32LenDeviceName,
	IMG_UINT32					ui32DevFlags,
	IMG_CHAR *					pszMemSpaceName,
	IMG_UINT32					ui32LenMemSpaceName,
	TAL_eMemSpaceType			eMemSpaceType,
	IMG_UINT64					ui64BaseOffset,
	IMG_UINT64					ui64Size,
	IMG_UINT32					ui32IntNum
)
{
	IMG_UINT32					ui32Result = IMG_SUCCESS;
	IMG_CHAR *					pszLocDeviceName = IMG_NULL;
	IMG_CHAR *					pszLocMemSpaceName = IMG_NULL;
	PFTAL_sRegDevInfo *			psRegDevInfo;
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo;
	IMG_UINT32					ui32I;

	/* Ensure we are initialised. */
	ui32Result = TALSETUP_Initialise();
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		return ui32Result;
	}

	/* We corrently only support register memory sapces in this version of the TAL. Ignore other types. */
	if (eMemSpaceType != TAL_MEMSPACE_REGISTER)
	{
		return IMG_SUCCESS;
	}

	/* Copy the device name into kernel memory. */
	pszLocDeviceName = IMG_MALLOC(ui32LenDeviceName+1);
	IMG_ASSERT(pszLocDeviceName != IMG_NULL);
	if (pszLocDeviceName == IMG_NULL)
	{
		ui32Result = IMG_ERROR_OUT_OF_MEMORY;
		goto Exit;
	}
	ui32Result = SYSOSKM_CopyFromUser(pszLocDeviceName, pszDeviceName, (ui32LenDeviceName+1));
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		goto Exit;
	}

	/* Copy the memory space name into kernel memory. */
	pszLocMemSpaceName = IMG_MALLOC(ui32LenMemSpaceName+1);
	IMG_ASSERT(pszLocMemSpaceName != IMG_NULL);
	if (pszLocDeviceName == IMG_NULL)
	{
		ui32Result = IMG_ERROR_OUT_OF_MEMORY;
		goto Exit;
	}
	ui32Result = SYSOSKM_CopyFromUser(pszLocMemSpaceName, pszMemSpaceName, (ui32LenMemSpaceName+1));
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		goto Exit;
	}

	/* Locate this memory space. */
	SYSOSKM_DisableInt();
	psRegMemSpaceInfo = IMG_NULL;
	psRegDevInfo = LST_first(&gsRegDevList);
	while (psRegDevInfo != IMG_NULL)
	{
		psRegMemSpaceInfo = LST_first(&psRegDevInfo->sMemSpaceList);
		while (psRegMemSpaceInfo != IMG_NULL)
		{
			if (IMG_STRCMP(psRegMemSpaceInfo->pszMemSpaceName, pszLocMemSpaceName) == 0)
			{
				goto MemSpaceFound;
			}
			psRegMemSpaceInfo = LST_next(psRegMemSpaceInfo);
		}
		psRegDevInfo = LST_next(psRegDevInfo);
	}
MemSpaceFound:

	/* If memspace found. */
	if (psRegMemSpaceInfo != IMG_NULL)
	{
		/* We don't support memory spaces of teh same name on different devices. */
		if (IMG_STRCMP(psRegDevInfo->pszDeviceName, pszLocDeviceName) != 0)
		{
			ui32Result = IMG_ERROR_GENERIC_FAILURE;
			SYSOSKM_EnableInt();
			goto Exit;
		}

		/* This memory space is already defined. */
		SYSOSKM_EnableInt();
		goto Exit;
	}

	/* Locate this device. */
	psRegDevInfo = LST_first(&gsRegDevList);
	while (psRegDevInfo != IMG_NULL)
	{
		if (IMG_STRCMP(psRegDevInfo->pszDeviceName, pszLocDeviceName) == 0)
		{
			break;
		}
		psRegDevInfo = LST_next(psRegDevInfo);
	}

	/* If device not found. */
	if (psRegDevInfo == IMG_NULL)
	{
		/* Allocate a device structure...*/
		psRegDevInfo = IMG_MALLOC(sizeof(*psRegDevInfo));
		IMG_ASSERT(psRegDevInfo != IMG_NULL);
		if (psRegDevInfo == IMG_NULL)
		{
			ui32Result = IMG_ERROR_OUT_OF_MEMORY;
			SYSOSKM_EnableInt();
			goto Exit;
		}
		IMG_MEMSET(psRegDevInfo, 0, sizeof(*psRegDevInfo));
		psRegDevInfo->pszDeviceName		= pszLocDeviceName;
		pszLocDeviceName				= IMG_NULL;
		psRegDevInfo->ui32DevFlags		= ui32DevFlags;
		LST_init(&psRegDevInfo->sMemSpaceList);
#if defined (IMG_KERNEL_MODULE)
		/* Open the device...*/
		ui32Result = SYSDEVKM_OpenDevice(psRegDevInfo->pszDeviceName, &psRegDevInfo->hSysDevHandle);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			SYSOSKM_EnableInt();
			return ui32Result;
		}

		/* Map the device so that it can be accessed in kernel mode...*/
		ui32Result = SYSDEVKM_GetCpuKmAddr(psRegDevInfo->hSysDevHandle, SYSDEVKM_REGID_REGISTERS, &psRegDevInfo->pvRegCpuKmAddr, &psRegDevInfo->ui32RegSize);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			SYSOSKM_EnableInt();
			return ui32Result;
		}
#else
		/* We have to temporaily enable interrupts to allwow the following to "work" */
		SYSOSKM_EnableInt();

		/* Initialise the device manager...*/
		ui32Result = DMAN_Initialise();
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			return ui32Result;
		}

		/* Open the "NULL" device...*/
		ui32Result = DMAN_OpenDevice(NULL_DEV_NAME, DMAN_OMODE_SHARED, &psRegDevInfo->ui32ConnId);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			return ui32Result;
		}

		/* Open the device...*/
		ui32Result = DMAN_GetDeviceId(psRegDevInfo->ui32ConnId, &psRegDevInfo->ui32DeviceId);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			return ui32Result;
		}

		/* Attach to the wrapper utilities to this device connection...*/
		ui32Result = WRAPU_AttachToConnection(psRegDevInfo->ui32ConnId, &psRegDevInfo->ui32WrapUAttachId);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			return ui32Result;
		}

		/* Get access to register and slave ports...*/
		ui32Result = WRAPU_GetCpuUmAddr(psRegDevInfo->ui32WrapUAttachId, WRAPU_REGID_REGISTERS, &psRegDevInfo->pvRegCpuKmAddr);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			return ui32Result;
		}

		SYSOSKM_DisableInt();
#endif

		LST_add(&gsRegDevList, psRegDevInfo);
	}

	/* Allocate a mem space structure...*/
	psRegMemSpaceInfo = IMG_MALLOC(sizeof(*psRegMemSpaceInfo));
	IMG_ASSERT(psRegMemSpaceInfo != IMG_NULL);
	if (psRegDevInfo == IMG_NULL)
	{
		ui32Result = IMG_ERROR_OUT_OF_MEMORY;
		SYSOSKM_EnableInt();
		goto Exit;

	}
	IMG_MEMSET(psRegMemSpaceInfo, 0, sizeof(*psRegMemSpaceInfo));
	psRegMemSpaceInfo->psRegDevInfo		= psRegDevInfo;
	psRegMemSpaceInfo->eMemSpaceType	= eMemSpaceType;
	psRegMemSpaceInfo->pszMemSpaceName	= pszLocMemSpaceName;
	pszLocMemSpaceName					= IMG_NULL;
	psRegMemSpaceInfo->ui64BaseOffset	= ui64BaseOffset;
	psRegMemSpaceInfo->ui64Size			= ui64Size;
	psRegMemSpaceInfo->ui32IntNum		= ui32IntNum;
//	IMG_ASSERT((ui64BaseOffset+ui64Size) <= psRegDevInfo->ui32RegSize);
	psRegMemSpaceInfo->pui32RegisterSpace = (IMG_UINT32 *) ((char*)psRegDevInfo->pvRegCpuKmAddr + psRegMemSpaceInfo->ui64BaseOffset);
	for (ui32I=0; ui32I< TAL_NUM_INT_MASKS; ui32I++ )
	{
		psRegMemSpaceInfo->aui32intmask[ui32I] = 0;
	}
	ui32I = (psRegMemSpaceInfo->ui32IntNum & ~(0x20-1))>>5;
	if (ui32I < TAL_NUM_INT_MASKS)
	{
		psRegMemSpaceInfo->aui32intmask[ui32I] = (1<< ((psRegMemSpaceInfo->ui32IntNum)&(0x20-1)) );
	}
	LST_add(&psRegDevInfo->sMemSpaceList, psRegMemSpaceInfo);

	SYSOSKM_EnableInt();

Exit:
	/* Free local copies of the names that are notr null. */
	if (pszLocDeviceName != IMG_NULL)
	{
		IMG_FREE(pszLocDeviceName);
	}
	if (pszLocMemSpaceName != IMG_NULL)
	{
		IMG_FREE(pszLocMemSpaceName);
	}

	return ui32Result;
}


/*!
******************************************************************************
##############################################################################
 Category:              TAL Setup Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

 @Function              TALSETUP_Initialise

******************************************************************************/
IMG_RESULT TALSETUP_Initialise(void)
{
	IMG_HANDLE				hResBHandle;
	IMG_UINT32				ui32Result;
    IMG_VOID *				pvParam;

	/* Check if we are inilialised. */
	SYSOSKM_DisableInt();
	if (!gbInitialised )
	{
#if !defined (IMG_KERNEL_MODULE)
		OSA_CritSectCreate(&ghDevListMutex);
#endif
		/* Get handle to global resource bucket. */
		hResBHandle = RMAN_GetGlobalBucket();

		/* Ensure we have a "free" function register to free the resources when the KM is uninstalled. */
		ui32Result = RMAN_GetNamedResource("PFTAL", pftal_fnAlloc, IMG_NULL, hResBHandle, PFTAL_TYPE_ID, pftal_fnFree, &pvParam, IMG_NULL, IMG_NULL);
		IMG_ASSERT(ui32Result == IMG_SUCCESS);
		if (ui32Result != IMG_SUCCESS)
		{
			SYSOSKM_EnableInt();
			return ui32Result;
		}

		gbInitialised = IMG_TRUE;
	}
	SYSOSKM_EnableInt();

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALSETUP_Reset

******************************************************************************/
IMG_RESULT TALSETUP_Reset (void)
{
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALSETUP_Deinitialise

******************************************************************************/
IMG_RESULT TALSETUP_Deinitialise (void)
{
	if (gbInitialised )
	{
#if !defined (IMG_KERNEL_MODULE)
		OSA_CritSectDestroy(ghDevListMutex);
#endif
	}
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALSETUP_SetTargetApplicationFlags

******************************************************************************/
IMG_VOID TALSETUP_SetTargetApplicationFlags(
	IMG_UINT32					ui32TargetAppFlags
)
{
}

/*!
******************************************************************************

 @Function				TALSETUP_RegisterPollFailFunc

******************************************************************************/
IMG_VOID TALSETUP_RegisterPollFailFunc (
	TAL_pfnPollFailFunc         pfnPollFailFunc
)
{
    //Shouldn't be using polls inside portability framework TAL
	IMG_ASSERT(IMG_FALSE);
}


/*!
******************************************************************************

 @Function				TAL_GetMemSpaceHandle

******************************************************************************/
IMG_HANDLE TAL_GetMemSpaceHandle (
	const IMG_CHAR *		psMemSpaceName
)
{
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo;
	IMG_HANDLE					hMemSpItr;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	IMG_ASSERT(psMemSpaceName != IMG_NULL);

	psRegMemSpaceInfo = TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
	while (psRegMemSpaceInfo)
	{
        if (strcmp(psRegMemSpaceInfo->pszMemSpaceName, psMemSpaceName) == 0)
        {
            break;
        }
        psRegMemSpaceInfo = TALITERATOR_GetNext(hMemSpItr);
    }

	if (psRegMemSpaceInfo != IMG_NULL)
	{
		TALITERATOR_Destroy(hMemSpItr);
	}

	IMG_ASSERT(psRegMemSpaceInfo != IMG_NULL);

    return (IMG_HANDLE)psRegMemSpaceInfo;
}


/*!
******************************************************************************

 @Function              TALINTERRUPT_RegisterCheckFunc

*******************************************************************************/
IMG_RESULT TALINTERRUPT_RegisterCheckFunc(
    IMG_HANDLE                      hMemSpace,
    TAL_pfnCheckInterruptFunc       pfnCheckInterruptFunc,
    IMG_VOID *                      pCallbackParameter
)
{
#if !defined (IMG_KERNEL_MODULE)
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo = hMemSpace;
#endif

#if defined (IMG_KERNEL_MODULE)
	IMG_ASSERT(IMG_FALSE);			/* We don't support this in kernel mode...*/
	return IMG_ERROR_FATAL;
#else
	IMG_ASSERT(gbInitialised == IMG_TRUE);
	IMG_ASSERT(psRegMemSpaceInfo->pfnCheckInterruptFunc == IMG_NULL);

	psRegMemSpaceInfo->pfnCheckInterruptFunc = pfnCheckInterruptFunc;
	psRegMemSpaceInfo->pCallbackParameter	 = pCallbackParameter;

	return IMG_SUCCESS;
#endif
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Register Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

 @Function              TALREG_WriteWord32

******************************************************************************/
IMG_RESULT TALREG_WriteWord32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32		ui32Value)
{
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo = hMemSpace;
#ifdef WINCE
    IMG_UINT32 *                    pvKernelSpaceAddress;
#endif

	IMG_ASSERT(gbInitialised == IMG_TRUE);
    IMG_ASSERT((ui32Offset & 0x3) == 0);

	/* Check memory space ID is valid...*/
	IMG_ASSERT(hMemSpace);
	if (hMemSpace == IMG_NULL)
	{
		return IMG_ERROR_INVALID_ID;
	}

    IMG_ASSERT (psRegMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);
	IMG_ASSERT(psRegMemSpaceInfo->pui32RegisterSpace	!= IMG_NULL);

#ifdef WINCE
    /* Always write to the Kernel-space memory addresses as Bridge cannot guarantee the   *
     * user process is in cpu context, so write may not succeed, causing access violation */
    pvKernelSpaceAddress = (IMG_UINT32 *)SYSOSKM_CpuKmAddrToCpuPAddr((IMG_VOID *)psMemSpaceCBxx->pui32RegisterSpace);

    /* Write register...*/
    pvKernelSpaceAddress[ui32Offset>>2] = ui32Value;
#else //Linux:
	/* Write register...*/
    psRegMemSpaceInfo->pui32RegisterSpace[ui32Offset>>2] = ui32Value;
#endif
	/* Return success...*/
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALREG_WriteWords32

******************************************************************************/
IMG_RESULT TALREG_WriteWords32(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32WordCount,
	IMG_UINT32 *					pui32Value
)
{
	// To Do: Implement this
	// Function not yet supported in portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              TALREG_ReadWord32

******************************************************************************/
IMG_RESULT TALREG_ReadWord32(
    IMG_HANDLE              hMemSpace,
    IMG_UINT32              ui32Offset,
    IMG_UINT32 *            pui32Value
)
{
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo = hMemSpace;
#ifdef WINCE
	IMG_UINT32 *                      pui32KernelSpaceAddress;
	IMG_UINT32 *                      pui32ValidAddress;
#endif

	IMG_ASSERT(gbInitialised == IMG_TRUE);
    IMG_ASSERT((ui32Offset & 0x3) == 0);

	// Check memory space is valid...
    IMG_ASSERT(hMemSpace);
	if (hMemSpace == IMG_NULL)
	{
		return IMG_ERROR_INVALID_ID;
	}

    IMG_ASSERT(psRegMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);
	IMG_ASSERT(psRegMemSpaceInfo->pui32RegisterSpace	!= IMG_NULL);

#ifdef WINCE
	/* Always read from the Kernel-space memory addresses as Bridge cannot guarantee the *
	 * user process is in cpu context, so read may not succeed, causing access violation */
	pui32KernelSpaceAddress = (IMG_UINT32 *) SYSOSKM_CpuKmAddrToCpuPAddr((IMG_VOID *)psRegMemSpaceInfo->pui32RegisterSpace);

	/* Identify the address range of pui32Value (User/Kernel) */
	if(IsKModeAddr((DWORD)pui32Value))
	{ //Address already in Kernel space, ok to use:
		pui32ValidAddress = pui32Value;
	} else { //Address in User-space, convert to Kernel space:
		pui32ValidAddress = (IMG_UINT32 *)SYSOSKM_CpuKmAddrToCpuPAddr(pui32Value);
	}
	
	/* Read register...*/
	*pui32ValidAddress = pui32KernelSpaceAddress[ui32Offset>>2];
		
#else //Linux (Existing addresses are valid):
	*pui32Value = psRegMemSpaceInfo->pui32RegisterSpace[ui32Offset>>2];
#endif
	/* Return success...*/
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALREG_ReadWords32

******************************************************************************/
IMG_RESULT TALREG_ReadWords32(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32WordCount,
	IMG_UINT32 *					pui32Value
)
{
	// To Do: Implement this
	// Function not yet supported in portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALREG_Poll32

******************************************************************************/
IMG_RESULT TALREG_Poll32(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
)
{
#if defined (IMG_KERNEL_MODULE)
	IMG_ASSERT(IMG_FALSE);			/* We don't want to be POLing in the kernel...*/
	return IMG_ERROR_FATAL;
#else
	IMG_UINT32					ui32Count, ui32ReadVal;
	IMG_UINT32					ui32Result = IMG_SUCCESS;
//	IMG_BOOL					bPass = IMG_FALSE;

	/* Test the value */
	for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
	{
		// Read from the device
		ui32Result = TALREG_ReadWord32(hMemSpace, ui32Offset, &ui32ReadVal);
		// Check result
		if (IMG_SUCCESS != ui32Result) break;
		if(TAL_TestWord(ui32ReadVal, ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt))
			break;
	}
	if (ui32Count >= ui32PollCount)	ui32Result = IMG_ERROR_TIMEOUT;

	return ui32Result;
#endif
}


/*!
******************************************************************************

 @Function              TALREG_CircBufPoll32

******************************************************************************/
IMG_RESULT TALREG_CircBufPoll32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT32                      ui32Offset,
	IMG_UINT32						ui32WriteOffsetVal,
	IMG_UINT32						ui32PacketSize,
	IMG_UINT32						ui32BufferSize,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
)
{
#if defined (IMG_KERNEL_MODULE)
	IMG_ASSERT(IMG_FALSE);			/* We don't want to be POLing in the kernel...*/
	return IMG_ERROR_FATAL;
#else
	IMG_UINT32					ui32Count, ui32ReadVal;
	IMG_RESULT					ui32Result = IMG_SUCCESS;
	IMG_BOOL					bPass = IMG_FALSE;

	/* Test the value */
	for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
	{
		// Read from the device
		ui32Result = TALREG_ReadWord32(hMemSpace, ui32Offset, &ui32ReadVal);
		// Check result
		if (IMG_SUCCESS != ui32Result) break;
//		if(bPass = TAL_TestWord(ui32ReadVal, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, TAL_CHECKFUNC_CBP))
		bPass = TAL_TestWord(ui32ReadVal, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, TAL_CHECKFUNC_CBP);
		if (bPass)
			break;
	}
	if (ui32Count >= ui32PollCount)	ui32Result = IMG_ERROR_TIMEOUT;

	return ui32Result;
#endif
}

/*!
******************************************************************************

 @Function				TALDEBUG_DevComment

******************************************************************************/
IMG_RESULT TALDEBUG_DevComment(
    IMG_HANDLE                      hMemSpace,
    const IMG_CHAR *				psCommentText
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALDEBUG_DevPrint

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrint(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALDEBUG_DevPrintf

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrintf(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText,
	...
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              TALDEBUG_GetDevTime

******************************************************************************/
IMG_RESULT TALDEBUG_GetDevTime(
	IMG_HANDLE                      hMemSpace,
    IMG_UINT64 *                    pui64Time
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALDEBUG_AutoIdle

******************************************************************************/
IMG_RESULT TALDEBUG_AutoIdle(
	IMG_HANDLE                      hMemSpace,
	IMG_UINT32						ui32Time
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				TALDEBUG_ReadMem

******************************************************************************/
IMG_RESULT TALDEBUG_ReadMem(
	IMG_UINT64				ui64MemoryAddress,
	IMG_UINT32				ui32Size,
	IMG_PUINT8				pui8Buffer
)
{
	// Function not supported portability framework
	IMG_ASSERT(IMG_FALSE);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				TALPDUMP_SetPdump1MemName

******************************************************************************/
IMG_VOID TALPDUMP_SetPdump1MemName (
	IMG_CHAR*					pszMemName
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_Comment

******************************************************************************/
IMG_VOID TALPDUMP_Comment(
	IMG_HANDLE				hMemSpace,
	const IMG_CHAR *	    psCommentText
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_MiscOutput

******************************************************************************/
IMG_VOID TALPDUMP_MiscOutput(
	IMG_HANDLE				hMemSpace,
	IMG_CHAR *				pOutputText
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_ConsoleMessage

******************************************************************************/
IMG_VOID TALPDUMP_ConsoleMessage(
	IMG_HANDLE				hMemSpace,
    IMG_CHAR *              psCommentText
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_SetConversionSourceData

******************************************************************************/
IMG_VOID TALPDUMP_SetConversionSourceData(
    IMG_UINT32				ui32SourceFileID,
    IMG_UINT32				ui32SourceLineNum
)
{
}



/*!
******************************************************************************

 @Function				TALPDUMP_MemSpceCaptureEnable

******************************************************************************/
IMG_VOID TALPDUMP_MemSpceCaptureEnable (
	IMG_HANDLE						hMemSpace,
	IMG_BOOL						bEnable,
	IMG_BOOL *						pbPrevState
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_DisableCmds

******************************************************************************/
IMG_VOID TALPDUMP_DisableCmds(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32DisableFlags
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_SetFlags

******************************************************************************/
IMG_VOID TALPDUMP_SetFlags( 
	IMG_UINT32						ui32Flags
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_ClearFlags

******************************************************************************/
IMG_VOID TALPDUMP_ClearFlags( 
	IMG_UINT32		ui32Flags

)
{
}



/*!
******************************************************************************

 @Function				TALPDUMP_GetFlags

******************************************************************************/
IMG_UINT32 TALPDUMP_GetFlags(void)
{
	return 0;
}

/*!
******************************************************************************

 @Function				TALPDUMP_SetFilename

******************************************************************************/
IMG_VOID TALPDUMP_SetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_CHAR *				        psFileName
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_GetFilename

******************************************************************************/
IMG_CHAR * TALPDUMP_GetFilename (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename
)
{
	return IMG_NULL;
}

/*!
******************************************************************************

 @Function				TALPDUMP_SetFileoffset

******************************************************************************/
IMG_VOID TALPDUMP_SetFileoffset (
    IMG_HANDLE                      hMemSpace,
	TAL_eSetFilename		        eSetFilename,
	IMG_UINT64				        ui64FileOffset
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_DefineContext

******************************************************************************/
IMG_VOID TALPDUMP_DefineContext (
	IMG_CHAR *	    pszContextName,
	IMG_CHAR **	    ppszMemSpaceNames
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_EnableContextCapture

******************************************************************************/

IMG_VOID TALPDUMP_EnableContextCapture (
	IMG_CHAR *	    pszContextName
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_GetNoContexts

******************************************************************************/
IMG_UINT32 TALPDUMP_GetNoContexts (void)
{
	return 0; 
}

/*!
******************************************************************************

 @Function				TALPDUMP_AddSyncToTDF

******************************************************************************/
IMG_VOID TALPDUMP_AddSyncToTDF (
	IMG_HANDLE			hMemSpace,
	IMG_UINT32			ui32SyncId
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_AddCommentToTDF

******************************************************************************/
IMG_VOID TALPDUMP_AddCommentToTDF (
	IMG_CHAR *			pcComment
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_AddTargetConfigToTDF

******************************************************************************/
IMG_VOID TALPDUMP_AddTargetConfigToTDF (
	IMG_CHAR *			pszFilePath
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_GenerateTDF

******************************************************************************/
IMG_VOID TALPDUMP_GenerateTDF (
	IMG_CHAR *			psFilename)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_CaptureStart

******************************************************************************/
IMG_VOID TALPDUMP_CaptureStart(
	IMG_CHAR *							psTestDirectory
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_CaptureStop

******************************************************************************/
IMG_VOID TALPDUMP_CaptureStop(void)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_ChangeResFileName

******************************************************************************/
IMG_VOID TALPDUMP_ChangeResFileName(
	IMG_HANDLE		hMemSpace,
	IMG_CHAR *		sFileName
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_ChangePrmFileName

******************************************************************************/
IMG_VOID TALPDUMP_ChangePrmFileName(
	IMG_HANDLE		hMemSpace,
	IMG_CHAR *		sFileName
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_ResetResFileName

******************************************************************************/
IMG_VOID TALPDUMP_ResetResFileName(
	IMG_HANDLE		hMemSpace
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_ResetPrmFileName

******************************************************************************/
IMG_VOID TALPDUMP_ResetPrmFileName(
	IMG_HANDLE		hMemSpace
)
{
}



/*!
******************************************************************************

 @Function				TALPDUMP_IsCaptureEnabled

******************************************************************************/

IMG_BOOL TALPDUMP_IsCaptureEnabled(void)
{
	return IMG_FALSE;
}

/*!
******************************************************************************

 @Function				TALPDUMP_GetSemaphoreId

******************************************************************************/
IMG_UINT32 TALPDUMP_GetSemaphoreId(
	IMG_HANDLE						hSrcMemSpace,
	IMG_HANDLE						hDestMemSpace
)
{
	return 1;
}

/*!
******************************************************************************

 @Function				TALPDUMP_Lock

******************************************************************************/
IMG_VOID TALPDUMP_Lock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_Unlock

******************************************************************************/
IMG_VOID TALPDUMP_Unlock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
)
{
}

/*!
******************************************************************************

 @Function				TALPDUMP_SyncWithId

******************************************************************************/
IMG_RESULT TALPDUMP_SyncWithId(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SyncId
)
{
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALPDUMP_Sync

******************************************************************************/
IMG_VOID TALPDUMP_Sync(
	IMG_HANDLE						hMemSpace
)
{
}


/*!
******************************************************************************

 @Function				TALPDUMP_SetDefineStrings

******************************************************************************/
IMG_VOID TALPDUMP_SetDefineStrings(
    TAL_Strings *					psStrings
)
{
	// Not currently supported in Portability Framework
	IMG_ASSERT(IMG_FALSE);
}

/*!
******************************************************************************

 @Function				TALPDUMP_If

******************************************************************************/
IMG_VOID TALPDUMP_If(
    IMG_HANDLE						hMemSpace,
	IMG_CHAR*						pszDefine
)
{
	// Not currently supported in Portability Framework
	IMG_ASSERT(IMG_FALSE);
}

/*!
******************************************************************************

 @Function				TALPDUMP_Else

******************************************************************************/
IMG_VOID TALPDUMP_Else(
    IMG_HANDLE						hMemSpace
)
{
	// Not currently supported in Portability Framework
	IMG_ASSERT(IMG_FALSE);
}

/*!
******************************************************************************

 @Function				TALPDUMP_EndIf

******************************************************************************/
IMG_VOID TALPDUMP_EndIf(
    IMG_HANDLE						hMemSpace
)
{
	// Not currently supported in Portability Framework
	IMG_ASSERT(IMG_FALSE);
}

/*!
******************************************************************************

 @Function              taliterator_GetNext

******************************************************************************/
static
IMG_PVOID taliterator_GetNext(
	IMG_HANDLE			hIterator,
	IMG_BOOL			bUnLock
	)
{
	TALITR_psIter				psIterator = (TALITR_psIter)hIterator;
	PFTAL_sRegDevInfo *			psRegDevInfo;
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo;

	if (hIterator == IMG_NULL)
	{
		return IMG_NULL;
	}

	switch(psIterator->eType)
	{
	case TALITR_TYP_NONE:
		break;
	case TALITR_TYP_INTERRUPT:
#if defined (IMG_KERNEL_MODULE)
	IMG_ASSERT(IMG_FALSE);			/* Not supported in kernel...*/
	return IMG_NULL;
#else
	IMG_ASSERT(0);
#if 0
		{
			TAL_sMemSpaceCBxx *	psMemSpaceCBxx;

			// Start searching the memory spaces
			psMemSpaceCBxx = (TAL_sMemSpaceCBxx *)TALITERATOR_GetNext(psIterator->uInfo.itrInterrupt.hMemSpaceIter);
			while (psMemSpaceCBxx)
			{
				if (psMemSpaceCBxx->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER && psMemSpaceCBxx->apfnCheckInterruptFunc)
				{
					if (psMemSpaceCBxx->apfnCheckInterruptFunc(
											(IMG_HANDLE)psMemSpaceCBxx,
											psMemSpaceCBxx->apCallbackParameter))
					{
						return psMemSpaceCBxx;
					}
				}
				psMemSpaceCBxx = (TAL_sMemSpaceCBxx *)TALITERATOR_GetNext(psIterator->uInfo.itrInterrupt.hMemSpaceIter);
			}
		}
#endif
#endif
		break;
	case TALITR_TYP_MEMSPACE:
		psRegMemSpaceInfo = (PFTAL_sRegMemSpaceInfo *) psIterator->uInfo.itrMemSpce.psMemSpaceCB;
		psRegDevInfo = psRegMemSpaceInfo->psRegDevInfo;
		psRegMemSpaceInfo = LST_next(psRegMemSpaceInfo);
		if (psRegMemSpaceInfo == IMG_NULL)
		{
			psRegDevInfo = LST_next(psRegDevInfo);
			if (psRegDevInfo != IMG_NULL)
			{
				psRegMemSpaceInfo = LST_first(&psRegDevInfo->sMemSpaceList);
			}
		}
		if (psRegMemSpaceInfo == IMG_NULL)
		{
#if !defined (IMG_KERNEL_MODULE)
			if (bUnLock)
			{
				OSA_CritSectUnlock(ghDevListMutex);
			}
#endif
			IMG_FREE(psIterator);
			return IMG_NULL;
		}
		else
		{
			psIterator->uInfo.itrMemSpce.psMemSpaceCB = (TAL_psMemSpaceCB)psRegMemSpaceInfo;
		}
		return psRegMemSpaceInfo;
		break;
	default:
		IMG_ASSERT(IMG_FALSE);
		break;
	}
	return IMG_NULL;
}

/*!
******************************************************************************

 @Function              taliterator_GetFirst

******************************************************************************/
static
IMG_PVOID taliterator_GetFirst(
    TALITERATOR_eTypes	eItrType,
	IMG_HANDLE *		phIterator,
	IMG_BOOL			bLock
)
{
	TALITR_psIter				psIterator = IMG_NULL;
	IMG_PVOID					pvItem = IMG_NULL;
	PFTAL_sRegDevInfo *			psRegDevInfo;
	PFTAL_sRegMemSpaceInfo *	psRegMemSpaceInfo;
#if !defined (IMG_KERNEL_MODULE)
	IMG_HANDLE					hItr;
#endif

	IMG_ASSERT(gbInitialised == IMG_TRUE);

#if !defined (IMG_KERNEL_MODULE)
	if (bLock)
	{
		OSA_CritSectLock(ghDevListMutex);
	}
#endif
	switch(eItrType)
	{
	case TALITR_TYP_INTERRUPT:
#if defined (IMG_KERNEL_MODULE)
	IMG_ASSERT(IMG_FALSE);			/* Not supported in kernel...*/
	return IMG_NULL;
#else
		// Start searching the memory spaces
		psRegMemSpaceInfo = (PFTAL_sRegMemSpaceInfo *)taliterator_GetFirst(TALITR_TYP_MEMSPACE, &hItr, IMG_FALSE);
		while (psRegMemSpaceInfo)
		{
			if (psRegMemSpaceInfo->pfnCheckInterruptFunc)
			{
				if (psRegMemSpaceInfo->pfnCheckInterruptFunc(
										(IMG_HANDLE)psRegMemSpaceInfo,
										psRegMemSpaceInfo->pCallbackParameter))
				{
					psIterator = (TALITR_psIter)IMG_MALLOC(sizeof(TALITR_sIter));
					psIterator->eType = TALITR_TYP_INTERRUPT;
					psIterator->uInfo.itrInterrupt.hMemSpaceIter = hItr;
					pvItem = psRegMemSpaceInfo;
					break;
				}
			}
			psRegMemSpaceInfo = (PFTAL_sRegMemSpaceInfo *)taliterator_GetNext(hItr, IMG_FALSE);
		}
#endif
		break;
	case TALITR_TYP_MEMSPACE:
		psRegDevInfo = LST_first(&gsRegDevList);
		if (psRegDevInfo != IMG_NULL)
		{
			psRegMemSpaceInfo = LST_first(&psRegDevInfo->sMemSpaceList);
			if (psRegMemSpaceInfo != IMG_NULL)
			{
				psIterator = (TALITR_psIter)IMG_MALLOC(sizeof(TALITR_sIter));
				psIterator->eType = TALITR_TYP_MEMSPACE;
				psIterator->uInfo.itrMemSpce.psMemSpaceCB = (TAL_psMemSpaceCB) psRegMemSpaceInfo;
				pvItem = psRegMemSpaceInfo;
			}
		}
		break;
	default:
		IMG_ASSERT(IMG_FALSE);
	}
#if !defined (IMG_KERNEL_MODULE)
	if ( (psIterator == IMG_NULL) && (bLock) )
	{
		OSA_CritSectUnlock(ghDevListMutex);
	}
#endif
	*phIterator = psIterator;
	return pvItem;
}

/*!
******************************************************************************

 @Function              TALITERATOR_GetFirst

******************************************************************************/
IMG_PVOID TALITERATOR_GetFirst(
    TALITERATOR_eTypes	eItrType,
	IMG_HANDLE *		phIterator
)
{
	return taliterator_GetFirst(eItrType, phIterator, IMG_TRUE);
}

/*!
******************************************************************************

 @Function              TALITERATOR_GetNext

******************************************************************************/
IMG_PVOID TALITERATOR_GetNext(IMG_HANDLE hIterator)
{
	return taliterator_GetNext(hIterator, IMG_TRUE);
}

/*!
******************************************************************************

 @Function              TALITERATOR_Destroy

******************************************************************************/
IMG_VOID TALITERATOR_Destroy(IMG_HANDLE hIterator)
{
	if(hIterator)
	{
#if !defined (IMG_KERNEL_MODULE)
		OSA_CritSectUnlock(ghDevListMutex);
#endif
		IMG_FREE(hIterator);
	}
}


/*!
******************************************************************************
******************************************************************************
******************************************************************************
******************************************************************************/


