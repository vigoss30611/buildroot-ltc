/*!
******************************************************************************
 @file   : bmem.c

 @brief

 @Author Alejandro Arcos

 @date   21/12/2010

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
         BMEM interface for the wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#ifdef WIN32
	#include <windows.h>
	#include <winioctl.h>
	#include <pciddif.h>
	#include <pbif.h>
	#ifndef WINCE
	//Win32 Only
		#define DEBUGMSG //Only supported in WinCE
		#ifdef UNICODE
			#define _T(t) L##t
			#define T(t) L##t
		#else
			#define _T(t) t
			#define T(t) t
		#endif
	#endif
#else  //Linux
	#include <fcntl.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/mman.h>
	#include <errno.h>
	#include <string.h>
	#include <sys/ioctl.h>
	#include <imgpcidd.h>
#endif

#include <stdio.h>
#include "devif_config.h"

#ifdef WIN32
/* Burn mem information */
static IMG_BOOL		                bBmemInitialised = IMG_FALSE;
static HANDLE		                hBmemDriver;

#endif


/*!
******************************************************************************
  The local device info structure.
******************************************************************************/
typedef struct
{
	/*! Pointer burn mem info from config file      */
    DEVIF_sBurnMemInfo *        psBurnMemInfo;
    /*! Host virtual address of the register base   */
    IMG_PVOID	                pvHostVirtRegisterBase;
    /*! Host virtual address of the memory base     */
    IMG_PVOID	                pvHostVirtMemoryBase;
    /*! Physical memroy address of the host virtual address of the memory base     */
    IMG_UINT32	                ui32PhysMemoryBase;
  #ifdef WIN32
    /*! Private date for burn-mem device            */
    MAP_PRIVATE_DATA	        sBmemDriverPrivateData;
  #endif

} BMEM_sLocalDeviceInfo;



#ifdef WIN32

BMEM_sLocalDeviceInfo *    gpsLocalDeviceInfo;
/*!
******************************************************************************

 @Function              BMEM_DeviceMemToHostVirtual

******************************************************************************/
static IMG_PVOID	BMEM_DeviceMemToHostVirtual(
    BMEM_sLocalDeviceInfo *        psLocalDeviceInfo,
    IMG_UINT32                      ui32DeviceAddress)
{
 	IMG_UINT32	ui32MemorySpaceOffset = (ui32DeviceAddress - psLocalDeviceInfo->ui32PhysMemoryBase);
	IMG_PVOID	pvHostVirtAddress;

    if (psLocalDeviceInfo != IMG_NULL)
    {
        ui32MemorySpaceOffset = (ui32DeviceAddress - psLocalDeviceInfo->ui32PhysMemoryBase);

        IMG_ASSERT(ui32DeviceAddress        >= psLocalDeviceInfo->ui32PhysMemoryBase);

	    IMG_ASSERT(ui32MemorySpaceOffset    <  psLocalDeviceInfo->psBurnMemInfo->ui32MemSize);

	    pvHostVirtAddress = (IMG_PVOID)((IMG_UINTPTR)psLocalDeviceInfo->pvHostVirtMemoryBase + ui32MemorySpaceOffset);
    }
    else
    {
	    pvHostVirtAddress = (IMG_PVOID)((IMG_UINTPTR)psLocalDeviceInfo->pvHostVirtMemoryBase + ui32DeviceAddress);
    }

	return(pvHostVirtAddress);
}


/*!
******************************************************************************

 @Function              BMEM_InitialiseDriver

******************************************************************************/
static IMG_BOOL BMEM_InitialiseDriver()
{
	OSVERSIONINFO				VersionInformation;
	IMG_BOOL					bIsOSWin9x = IMG_FALSE;

	/* Find out if we are running on Win9x or NT/Win2k/XP */
	VersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if(GetVersionEx(&VersionInformation))
	{
		/* The operation system is Windows NT */
		if(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			bIsOSWin9x=IMG_FALSE;
		}
		else
		{
			bIsOSWin9x=IMG_TRUE;
		}
	}
	else
	{
		printf("ERROR: Could not determine if we are running on Win9x or NT/Win2k/XP\n");
		IMG_ASSERT(IMG_FALSE);
	}

	if (bIsOSWin9x == IMG_TRUE)
	{
		hBmemDriver = CreateFile(_T("\\\\.\\imgpbdd.vxd"),
		  					GENERIC_READ | GENERIC_WRITE,
		  					FILE_SHARE_READ | FILE_SHARE_WRITE,
		  					NULL,
		  					OPEN_EXISTING,
		  					FILE_FLAG_DELETE_ON_CLOSE,
		  					NULL);
	}
	else
	{
		hBmemDriver = CreateFile(_T(PB_WIN32_DEVICE_NAME),
		  					GENERIC_READ | GENERIC_WRITE,
		  					FILE_SHARE_READ | FILE_SHARE_WRITE,
		  					NULL,
		  					OPEN_EXISTING,
		  					FILE_FLAG_DELETE_ON_CLOSE,
		  					NULL);
	}
	if (hBmemDriver == INVALID_HANDLE_VALUE)
	{
		printf("ERROR: Could not open device\n");
		IMG_ASSERT(IMG_FALSE);
	}

	return(IMG_TRUE);
}


/*!
******************************************************************************

 @Function              BMEM_MapMemForPCIDevice

******************************************************************************/
static IMG_BOOL	BMEM_MapMemForPCIDevice(
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo
)
{
	DWORD				dwResult;
	MAP_DEVICE_INBLK	sMapDevInBlock;
	MAP_DEVICE_OUTBLK	sMapDevOutBlock;
    IMG_UINT32          ui32BurnmemOffset = psLocalDeviceInfo->psBurnMemInfo->ui32MemStartOffset;
    IMG_UINT32          ui32BurnmemSize = psLocalDeviceInfo->psBurnMemInfo->ui32MemSize;

	/* Enforce the requirement that the device memory size value has to be a whole number of Megabytes */
	IMG_ASSERT((ui32BurnmemSize & 0x000FFFFF) == 0);
	IMG_ASSERT((ui32BurnmemOffset & 0x000FFFFF) == 0);
    ui32BurnmemOffset = ui32BurnmemOffset>>20;
    ui32BurnmemSize = ui32BurnmemSize>>20;

	/* Make a call to the PCI driver to obtain linear register and memory addresses */
    sMapDevInBlock.cDevice				= psLocalDeviceInfo->psBurnMemInfo->cDeviceId;
	sMapDevInBlock.uBurnMemOffsetMeg	= ui32BurnmemOffset;
	sMapDevInBlock.uBurnMemSizeMeg		= ui32BurnmemSize;

	if (DeviceIoControl(hBmemDriver, VPCI_MAP_DEVICE,
									&sMapDevInBlock,
									sizeof(sMapDevInBlock),
									&sMapDevOutBlock,
									sizeof(sMapDevOutBlock),
									&dwResult, 0)==0
	)
	{
		printf("ERROR: Problem mapping device %s\n", psLocalDeviceInfo->psBurnMemInfo->pszDeviceName);
		IMG_ASSERT(IMG_FALSE);
		return IMG_FALSE;
	}

	psLocalDeviceInfo->pvHostVirtMemoryBase	    = sMapDevOutBlock.pvMem;			/* Get the linear address pointer to the memory */
	psLocalDeviceInfo->pvHostVirtRegisterBase	= sMapDevOutBlock.pvRegisters;		/* Get the linear address pointer to the registers */

	printf("Map %d MB starting at %d MB into burnmemory\n", ui32BurnmemSize, ui32BurnmemOffset);
	printf("Memory mapped at CPU address 0x%p\n",       psLocalDeviceInfo->pvHostVirtMemoryBase);
	printf("Registers mapped at CPU address 0x%p\n",    psLocalDeviceInfo->pvHostVirtRegisterBase);

	/* Check validity of the linear address pointers to memory and registers */
	IMG_ASSERT(psLocalDeviceInfo->pvHostVirtMemoryBase != 0);
	IMG_ASSERT(psLocalDeviceInfo->pvHostVirtRegisterBase != 0);

	psLocalDeviceInfo->sBmemDriverPrivateData	= sMapDevOutBlock.priv;							/* Remember the private driver data */
	psLocalDeviceInfo->ui32PhysMemoryBase  	= *((IMG_UINT32 *) psLocalDeviceInfo->pvHostVirtMemoryBase);		/* Obtain the physical address of memory */

	if (sMapDevOutBlock.dwMemSize != (ui32BurnmemSize<<20))
	{
		printf("Memory size available though the driver is 0x%X bytes... \n", sMapDevOutBlock.dwMemSize);
	}

	return IMG_TRUE;
}


/*!
******************************************************************************

 @Function              BMEM_UnMapMemForPCIDevice

******************************************************************************/
static IMG_BOOL	BMEM_UnMapMemForPCIDevice(
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo
)
{
    DWORD					dwResult;
	UNMAP_DEVICE_INBLK		sUnmapDevInBlock;

    sUnmapDevInBlock.cDevice	= psLocalDeviceInfo->psBurnMemInfo->cDeviceId;
	sUnmapDevInBlock.priv		= psLocalDeviceInfo->sBmemDriverPrivateData;

	if (DeviceIoControl(hBmemDriver, VPCI_UNMAP_DEVICE,
									&sUnmapDevInBlock,
									sizeof(sUnmapDevInBlock),
									NULL,
									0,
									&dwResult, 0)==0
	)
	{
		printf("ERROR: Problem unmapping the device %s\n", psLocalDeviceInfo->psBurnMemInfo->pszDeviceName);
		IMG_ASSERT(IMG_FALSE);
	}

	return IMG_TRUE;
}

/*!
******************************************************************************

 @Function				BMEM_Initialise

******************************************************************************/
IMG_RESULT BMEM_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = (BMEM_sLocalDeviceInfo *)psDeviceCB->pvDevIfInfo;

    /* If the PCI interface has not been initialised..*/
    if (!bBmemInitialised)
    {
	    printf("Initialising PCI driver...\n");
	    if (!BMEM_InitialiseDriver())
	    {
		    printf("ERROR: Driver initialisation failed!\n");
		    IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_FATAL;
	    }

		bBmemInitialised = IMG_TRUE;
    }

    /* If the device has not been initialised...*/
    if (!psDeviceCB->bInitialised)
    {
	    printf("Mapping memory...\n");

		/* Map memory...*/
		if( !BMEM_MapMemForPCIDevice(psLocalDeviceInfo) )
		{
		    printf("ERROR: Failed to map memory!\n");
			return IMG_ERROR_FATAL;
		}
		
		psDeviceCB->bInitialised = IMG_TRUE;

    }
	gpsLocalDeviceInfo = psLocalDeviceInfo;
	return IMG_SUCCESS;
}
/*!
******************************************************************************

 @Function				BMEM_Deinitailise

******************************************************************************/
IMG_VOID BMEM_Deinitailise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = psDeviceCB->pvDevIfInfo;

	if (psLocalDeviceInfo != IMG_NULL)
	{
		BMEM_UnMapMemForPCIDevice(psLocalDeviceInfo);
	}

    if (psDeviceCB->pvDevIfInfo != IMG_NULL)
    {
        IMG_FREE(psLocalDeviceInfo);
        psDeviceCB->pvDevIfInfo = IMG_NULL;
    }
}

/*!
******************************************************************************

 @Function				BMEM_ReadRegister

******************************************************************************/
IMG_UINT32 BMEM_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = (BMEM_sLocalDeviceInfo *)psDeviceCB->pvDevIfInfo;
	volatile IMG_UINT32 *			pui32Register = (IMG_PUINT32)((IMG_UINTPTR)psLocalDeviceInfo->pvHostVirtRegisterBase + IMG_UINT64_TO_UINT32(ui64RegAddress));

    IMG_ASSERT(psDeviceCB->bInitialised);

    return *pui32Register;
}

/*!
******************************************************************************

 @Function				BMEM_WriteRegister

******************************************************************************/
IMG_VOID BMEM_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = psDeviceCB->pvDevIfInfo;
	volatile IMG_UINT32 *			pui32Register = (IMG_PUINT32)((IMG_UINTPTR)psLocalDeviceInfo->pvHostVirtRegisterBase + IMG_UINT64_TO_UINT32(ui64RegAddress));

    IMG_ASSERT(psDeviceCB->bInitialised);


// SGX and MSVDX Customisations deprecated
/*	if ( (psLocalDeviceInfo->psBurnMemInfo->cDeviceId == 'S') && ((psDeviceCB->sDevIfSetup.sWrapCtrlInfo.ui32Flags & TAL_WRAPFLAG_SGX_CLKGATECTL_OVERWRITE) != 0) )
	{
		if ((ui64RegAddress == 0x0))
		{
			ui32Value = 0x00111111;
		}
	}
	else if ( (psLocalDeviceInfo->psBurnMemInfo->cDeviceId == 'M') && ((psDeviceCB->sDevIfSetup.sWrapCtrlInfo.ui32Flags & TAL_WRAPFLAG_MSVDX_POL_RENDEC_SPACE) != 0) )
	{
		if ((ui64RegAddress == 0x00000894))
		{
			IMG_UINT32	ui32Count;
			volatile IMG_UINT32 *			pui32TempRegister = (IMG_PUINT32)((IMG_UINTPTR)psLocalDeviceInfo->pvHostVirtRegisterBase + 0x62C);
			volatile IMG_UINT32				ui32StartTime;

			// Need to poll register 0x62C bit0 for value of 1
			for (ui32Count = 0; !(*pui32TempRegister & 0x00000001) && (ui32Count < 0x9C40); ui32Count++)
			{
				// Wait some time
				ui32StartTime = 0x64;
				while (ui32StartTime != 0)
				{
					ui32StartTime--;
				}
			}

			if (!(*pui32TempRegister & 0x00000001))
			{
				printf("ERROR: BMEM_WriteRegister - timeout polling 0x62C\n");
				IMG_ASSERT(0);
			}
		}
	}
	*/
	/* Write register value */
	*pui32Register = ui32Value;
}

/*!
******************************************************************************

 @Function				BMEM_ReadMemory

******************************************************************************/
IMG_UINT32 BMEM_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
    BMEM_sLocalDeviceInfo *        psLocalDeviceInfo = (BMEM_sLocalDeviceInfo *)psDeviceCB->pvDevIfInfo;
    volatile IMG_UINT32 *           pui32HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui32HostVirt = (IMG_UINT32 *)BMEM_DeviceMemToHostVirtual(psLocalDeviceInfo, IMG_UINT64_TO_UINT32(ui64DevAddress));
    return *pui32HostVirt;
}



/*!
******************************************************************************

 @Function				BMEM_WriteMemory

******************************************************************************/
IMG_VOID BMEM_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = psDeviceCB->pvDevIfInfo;
    volatile IMG_UINT32 *           pui32HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui32HostVirt = (IMG_UINT32 *)BMEM_DeviceMemToHostVirtual(psLocalDeviceInfo, IMG_UINT64_TO_UINT32(ui64DevAddress));
    *pui32HostVirt = ui32Value;
}


/*!
******************************************************************************

 @Function				BMEM_CopyDeviceToHost

******************************************************************************/
IMG_VOID BMEM_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = (BMEM_sLocalDeviceInfo *)psDeviceCB->pvDevIfInfo;
    IMG_UINT8  *                    pui8HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui8HostVirt = (IMG_UINT8 *)BMEM_DeviceMemToHostVirtual(psLocalDeviceInfo, IMG_UINT64_TO_UINT32(ui64DevAddress));
	memcpy(pvHostAddress, pui8HostVirt, ui32Size);
}



/*!
******************************************************************************

 @Function				BMEM_CopyHostToDevice

******************************************************************************/
IMG_VOID BMEM_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
    BMEM_sLocalDeviceInfo *       psLocalDeviceInfo = psDeviceCB->pvDevIfInfo;
    IMG_UINT8  *                    pui8HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui8HostVirt = (IMG_UINT8 *)BMEM_DeviceMemToHostVirtual(psLocalDeviceInfo, IMG_UINT64_TO_UINT32(ui64DevAddress));
	memcpy(pui8HostVirt, pvHostAddress, ui32Size);
}

#endif  //#ifdef WIN32


/*!
******************************************************************************

 @Function				BMEM_DevIfConfigureDevice

******************************************************************************/
IMG_VOID
BMEM_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB
	)
{
    BMEM_sLocalDeviceInfo *        psLocalDeviceInfo;
	
	/* Create a device info block for this device...*/
	psLocalDeviceInfo = (BMEM_sLocalDeviceInfo *)malloc (sizeof(BMEM_sLocalDeviceInfo));
	memset(psLocalDeviceInfo, 0, sizeof(BMEM_sLocalDeviceInfo));
	psDeviceCB->bInitialised        = IMG_FALSE;
	psDeviceCB->pvDevIfInfo         = psLocalDeviceInfo;

    /* Get the burn-mem info...*/
    psLocalDeviceInfo->psBurnMemInfo = &psDeviceCB->sDevIfSetup.u.sBurnMemInfo;

#ifdef WIN32
    /* Use "burn-mem" driver...*/
    psDeviceCB->pfnInitailise       = BMEM_Initialise;
    psDeviceCB->pfnReadRegister     = BMEM_ReadRegister;
    psDeviceCB->pfnWriteRegister    = BMEM_WriteRegister;
    psDeviceCB->pfnReadMemory       = BMEM_ReadMemory;
    psDeviceCB->pfnWriteMemory      = BMEM_WriteMemory;
    psDeviceCB->pfnCopyDeviceToHost = BMEM_CopyDeviceToHost;
    psDeviceCB->pfnCopyHostToDevice = BMEM_CopyHostToDevice;
    psDeviceCB->pfnDeinitailise     = BMEM_Deinitailise;
#else
	printf("Burnmem interface not available in the Linux build\n");
	IMG_ASSERT(IMG_FALSE);
#endif

}


/*!
******************************************************************************

 @Function				BMEM_BurnMemInfo

******************************************************************************/
IMG_VOID BMEM_BurnMemInfo (
    IMG_UINT32          ui32BurnmemSize
)
{
#ifdef WIN32
 	DWORD				dwResult;
	MAP_DEVICE_INBLK	sMapDevInBlock;
	MAP_DEVICE_OUTBLK	sMapDevOutBlock;
    IMG_UINT32          ui32BurnmemOffset = 0;

    IMG_PVOID           pvHostVirtMemoryBase;
    IMG_PVOID           pvHostVirtRegisterBase;
    IMG_UINT32          ui32PhysMemoryBase;
	UNMAP_DEVICE_INBLK	sUnmapDevInBlock;

    /* Initailise the driver...*/
    BMEM_InitialiseDriver();

	/* Enforce the requirement that the device memory size value has to be a whole number of Megabytes */
	IMG_ASSERT((ui32BurnmemSize & 0x000FFFFF) == 0);
	IMG_ASSERT((ui32BurnmemOffset & 0x000FFFFF) == 0);
    ui32BurnmemOffset = ui32BurnmemOffset>>20;
    ui32BurnmemSize = ui32BurnmemSize>>20;

	/* Map the device...*/
    sMapDevInBlock.cDevice				= 'M';
	sMapDevInBlock.uBurnMemOffsetMeg	= ui32BurnmemOffset;
	sMapDevInBlock.uBurnMemSizeMeg		= ui32BurnmemSize;
	if (DeviceIoControl(hBmemDriver, VPCI_MAP_DEVICE,
									&sMapDevInBlock,
									sizeof(sMapDevInBlock),
									&sMapDevOutBlock,
									sizeof(sMapDevOutBlock),
									&dwResult, 0)==0
	)
	{
		printf("ERROR: Problem mapping device %\nNOTE: The size of the requested burn-mem could be larger than the available region\n");
		IMG_ASSERT(IMG_FALSE);
		return ;
	}

    /* Get the host virtual address mapping...*/
	pvHostVirtMemoryBase	    = sMapDevOutBlock.pvMem;
	pvHostVirtRegisterBase	= sMapDevOutBlock.pvRegisters;

	/* Check validity of the mappings...*/
	IMG_ASSERT(pvHostVirtMemoryBase != 0);
	IMG_ASSERT(pvHostVirtRegisterBase != 0);

	/* Get the physical adderss of the burn-mem...*/
	ui32PhysMemoryBase  	= *((IMG_UINT32 *) pvHostVirtMemoryBase);

    /* Display display burn-mem info...*/
    printf("Burn-mem base address: 0x%08X (physical), size 0x%08X (bytes)\n", ui32PhysMemoryBase, sMapDevOutBlock.dwMemSize);

    /* Unmap the device...*/
    sUnmapDevInBlock.cDevice	= 'M';
	sUnmapDevInBlock.priv		= sMapDevOutBlock.priv;
	if (DeviceIoControl(hBmemDriver, VPCI_UNMAP_DEVICE,
									&sUnmapDevInBlock,
									sizeof(sUnmapDevInBlock),
									NULL,
									0,
									&dwResult, 0)==0
	)
	{
		printf("ERROR: Problem unmapping the device M\n");
		IMG_ASSERT(IMG_FALSE);
	}
#else
		IMG_ASSERT(IMG_FALSE);
#endif
}


/*!
******************************************************************************

 @Function				BMEM_CopyAbsToHost

******************************************************************************/
IMG_VOID BMEM_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32
	IMG_UINT8  *        pui8HostVirt = 0;

    IMG_ASSERT(bBmemInitialised);
	pui8HostVirt = (IMG_UINT8 *)BMEM_DeviceMemToHostVirtual(gpsLocalDeviceInfo, IMG_UINT64_TO_UINT32(ui64AbsAddress));
	memcpy(pvHostAddress, pui8HostVirt, ui32Size);
#else
	IMG_ASSERT(IMG_FALSE);
#endif
}

/*!
******************************************************************************

 @Function              BMEM_FillAbs

******************************************************************************/
IMG_VOID BMEM_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32
	IMG_UINT8 *pui8HostVirt;

	IMG_ASSERT(bBmemInitialised);
    pui8HostVirt = (IMG_UINT8 *)BMEM_DeviceMemToHostVirtual(IMG_NULL, IMG_UINT64_TO_UINT32(ui64AbsAddress));
    memset(pui8HostVirt, ui8Value, ui32Size);
#else
	IMG_ASSERT(IMG_FALSE);
#endif
}




