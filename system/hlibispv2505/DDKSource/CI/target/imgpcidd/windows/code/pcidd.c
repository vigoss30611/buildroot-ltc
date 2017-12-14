/*****************************************************************************
 Name     		: PCIDD.C
 Title    		: PCI Services Driver
 C Author 		: John Metcalfe
 Created  		: 5/9/97

 Copyright		: 1997-2002 by Imagination Technologies Limited. All rights reserved.
                  No part of this software, either material or conceptual
                  may be copied or distributed, transmitted, transcribed,
                  stored in a retrieval system or translated into any
                  human or computer language in any form by any means,
                  electronic, mechanical, manual or other-wise, or
                  disclosed to third parties without the express written
                  permission of Imagination Technologies Limited, Unit 8, HomePark
                  Industrial Estate, King's Langley, Hertfordshire,
                  WD4 8LZ, U.K.

 Description	: Main C module for PCI IOCTL routines

*****************************************************************************/

#pragma warning(disable: 4115 4201 4214 4514)

#include <ntddk.h>
#include <windef.h>
#include <winerror.h>

#include "img_types.h"
#include "pciddif.h"
#include "hostfunc.h"
#include "pciutils.h"
#include "pcidd.h"
#pragma warning(default: 4115 4201 4214)

typedef volatile DWORD VDWORD, *PVDWORD;

// I don't reference all parameters
#pragma warning(disable:4100 4514)

#pragma warning (disable : 4201)	/* nameless struct/union */
#pragma warning (disable : 4214)	/* bit field types other than int */

/*****************************************************************************
 Globals
*****************************************************************************/

typedef struct _LONG_TERM_MAPPING_
{
	DWORD dwPhysAddress;
	DWORD dwSize;
	PVOID pvKrnlAddress;
	BOOL  bFileMapped;
} LONG_TERM_MAPPING, *PLONG_TERM_MAPPING;

static LONG_TERM_MAPPING	gasLongTermMapping[MAX_PCI_REGIONS] = {0};
static const BYTE			REGION_ADDRESS[MAX_PCI_REGIONS] = {0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x30};

PROCESS_CONTEXT gsAllocationContext = {0};

/*****************************************************************************
 Function prototypes
*****************************************************************************/

// Internal functions
INTERNAL VOID	MapPCIRegion (BYTE Bus, BYTE Device, MEMORY_CONTEXT* pRegionStr, int Region);
INTERNAL DWORD	UpdateMask(DWORD dwBus, DWORD dwFunc, DWORD dwDev, DWORD dwReg);
INTERNAL DWORD	UpdateBridgeMask(DWORD dwData);
INTERNAL BOOL	IsPhysSlotFree(DWORD dwPhysAddr, DWORD dwSizeRequired);

INTERNAL PVOID				GetUserModeMapping(PDISPATCH psCurrentDispatch, DWORD dwPhysAddr);
INTERNAL PLONG_TERM_MAPPING	GetKrnlModeMapping(DWORD dwPhysAddr);


#if SUPPORT_INT
	KINTERRUPT InterruptObject;

	/*****************************************************************************
	 Interupt Routine
	*****************************************************************************/
	BOOLEAN InterruptService(     struct _KINTERRUPT  *Interrupt,     void*  ServiceContext    )
	{
		return TRUE;
	}

#endif 




/*****************************************************************************
	IOCTL Interface Functions
*****************************************************************************/

/*****************************************************************************
 FUNCTION	:	IOCFindDevice

 PURPOSE	:	Scans PCI bus for the nth card with correct DeviceID
				If it finds the required card it maps it's memory
				regions.

 PARAMETERS	:	pvInBuffer - ptr to FIND_DEVICE_INBLK struct
				pvOutBuffer - ptr to FIND_DEVICE_OUTBLK struct

		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:	NO_ERROR always
					outbuf[0] -

*****************************************************************************/
DWORD IOCFindDevice(PVOID pvInBuffer, PVOID pvOutBuffer)
{
    DWORD               dwConfig0, dwBrdsFound = 0;
	BYTE				i, bClass, bHeader = 0, bRevision;

	IMG_BOOL bWriteCombine = IMG_FALSE;

	MEMORY_CONTEXT		*pPCIRegion;

	FIND_DEVICE_INBLK	sInBlk;
	PFIND_DEVICE_INBLK	psInBlk = (PFIND_DEVICE_INBLK) pvInBuffer;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	FIND_DEVICE_OUTBLK			*psOutBlk = (FIND_DEVICE_OUTBLK*) pvOutBuffer;

	sInBlk = *psInBlk; // in and out buffer occupy the same memory so make a copy of the in buffer for use

	if(psCurrentDispatch == NULL)
	{
		DBG_BREAK;
		return ERROR_OUT_OF_STRUCTURES;
	}

	psCurrentDispatch->bFoundDevice = FALSE;	// Block region use if we haven't found a device

	/*
		Find a device on a bus
		either match to Vendor ID, Device ID, Class etc.
	*/
	for (i=0; i < 32; i++)
	{
		// Read devices low 2 WORDS of config space
		// Word 0 is VendorID, Word 1 is DeviceID
		dwConfig0 = PCIReadDword((BYTE) sInBlk.wBus, 0, i, 0);

		// Is there a device?
		if (dwConfig0 == 0xFFFFFFFF)
			continue;

		// Are we matching to a VendorID?
		if ((sInBlk.wVendorID != PCI_FIND_DONT_CARE) && ((dwConfig0 & 0xFFFF) != sInBlk.wVendorID))
			continue;

		// Are we matching to a DeviceID?
		if ((sInBlk.wDeviceID != PCI_FIND_DONT_CARE) && (((dwConfig0 & 0xFFFF0000) >> 16) != sInBlk.wDeviceID))
			continue;

		// Are we matching to a Class code
		bClass = (BYTE) (PCIReadDword((BYTE) sInBlk.wBus, 0, i, 0x08) >> 24);

		if ((sInBlk.wDeviceClass != PCI_FIND_DONT_CARE) && (bClass != (BYTE) sInBlk.wDeviceClass))
			continue;

		dwBrdsFound++;

		if (dwBrdsFound == sInBlk.wDeviceCount)
		{
			// Get Header Type
			bHeader = (BYTE) (PCIReadDword((BYTE) sInBlk.wBus, 0, i, 0x0C) >> 16 && 0x00FF);
			bRevision = (BYTE) (PCIReadDword((BYTE) sInBlk.wBus, 0, i, 0x08) && 0x00FF);

			// Save info about this device
			psCurrentDispatch->bDeviceNum = i;
			psCurrentDispatch->bBusNum = (BYTE) sInBlk.wBus;
			psOutBlk->bDeviceNum = i;
			psOutBlk->bBusNum = psCurrentDispatch->bBusNum;
			psOutBlk->bHeaderType = bHeader;
			psOutBlk->wVendorID = (WORD)(dwConfig0 & 0xFFFF);
			psOutBlk->wDeviceID = (WORD)((dwConfig0 & 0xFFFF0000) >> 16);
			psOutBlk->bRevision = bRevision;
			psOutBlk->bDeviceClass = bClass;

			if( psOutBlk->wVendorID==0x1010 && (  psOutBlk->wDeviceID==0x1cf1 || psOutBlk->wDeviceID ==0x1cf2 || psOutBlk->wDeviceID==0x1cf3 ) )
			{
				DWORD IntInfo =0;
				extern IMG_UINT32 gui32PCIMemWriteCombine;

				KdPrint(("imgpcidd: YAY - Found a IMG Fpga Baseboard!\n"));
				bWriteCombine = ( gui32PCIMemWriteCombine==1) ? IMG_TRUE : IMG_FALSE;

				/* Int pin [ 15:8] Int Line [ 7:0 ] */
				IntInfo =  PCIReadDword((BYTE) sInBlk.wBus, 0, i, 0x3C );
				KdPrint(("imgpcidd: IntPin: %x IntLine: %x\n" , (IntInfo>>8)&0xff ,(IntInfo)&0xff ));
			}

			KdPrint(( "imgpcidd: Found device at bus %u, device %u!\n", psCurrentDispatch->bBusNum, i));
			break;
		}
	}

	/*
		If we find device then allocate addresses for it's mem blocks.
		(Only works for Header 0 devices)
	*/
	if ( (dwBrdsFound >= sInBlk.wDeviceCount) || (sInBlk.wDeviceCount == PCI_FIND_DONT_CARE))
	{
		psCurrentDispatch->bFoundDevice = TRUE;

		if (bHeader == 0)
		{
			for (i = 0; i < MAX_PCI_REGIONS; i++)
			{
				IMG_BOOL bCapped	= IMG_FALSE;
				IMG_BOOL bMapFail	= IMG_FALSE;
				IMG_BOOL bWC		= IMG_FALSE;
				if(psCurrentDispatch->ui32CurrentPciRegion >= MAX_PCI_REGIONS)
				{
					DBG_BREAK;
					KdPrint(("imgpcidd: Too many regions to fit in bar space\n"));
					return ERROR_OUT_OF_STRUCTURES;
				}

				// Map regions - find out their type and address range etc. & initialise a linear address
				pPCIRegion = &psCurrentDispatch->PCIRegion[psCurrentDispatch->ui32CurrentPciRegion];
				RtlZeroMemory(pPCIRegion, sizeof(MEMORY_CONTEXT));

				MapPCIRegion(psCurrentDispatch->bBusNum, psCurrentDispatch->bDeviceNum, pPCIRegion, i);

				/* Have we got a max bar size */
				{
					extern IMG_UINT32 gui32MaxBarSize;	
					if(gui32MaxBarSize && pPCIRegion->dwSize > gui32MaxBarSize )
					{
						pPCIRegion->dwSize		= gui32MaxBarSize;
						pPCIRegion->dwNumPages	= BYTES_TO_PAGES(pPCIRegion->dwSize);
						bCapped = IMG_TRUE;
					}
				}

				if (pPCIRegion->bUsage == RGN_MEMMAPPED || pPCIRegion->bUsage == RGN_ROM)
				{
					/* Check if region is already mapped in this process */
					pPCIRegion->pvUserAddr = GetUserModeMapping(psCurrentDispatch, pPCIRegion->dwPhysAddress);

					if(pPCIRegion->pvUserAddr == NULL) /* Only map memory if it isn't already mapped to this process */
					{
						/* Check if region is already mapped in another process */
						PLONG_TERM_MAPPING psLongTermMapping = GetKrnlModeMapping(pPCIRegion->dwPhysAddress);

						if(psLongTermMapping == NULL)
						{
							DBG_BREAK;
							KdPrint(("imgpcidd: Failed to map kernel mode for BAR%u 0x%X+0x%X\n", i, pPCIRegion->dwPhysAddress, pPCIRegion->dwSize));
							return ERROR_OUT_OF_STRUCTURES;
						}

						pPCIRegion->pvKrnlAddr = psLongTermMapping->pvKrnlAddress;
								
						pPCIRegion->pvUserAddr = NULL;
						bWC = ((i==2) && bWriteCombine );
						if(HostMapPhysToLin(pPCIRegion, bWC ? MmWriteCombined : MmNonCached))
						{
							if(psLongTermMapping->pvKrnlAddress == NULL)
							{
								psLongTermMapping->dwPhysAddress = pPCIRegion->dwPhysAddress;
								psLongTermMapping->dwSize = pPCIRegion->dwNumPages * PAGE_SIZE;
								psLongTermMapping->pvKrnlAddress = pPCIRegion->pvKrnlAddr;
								psLongTermMapping->bFileMapped = pPCIRegion->bFileMapped;
							}
						}
						else
						{
							DBG_BREAK;
							KdPrint(("imgpcidd: Failed to map physical memory for BAR%u 0x%X+0x%X\n", i, pPCIRegion->dwPhysAddress, pPCIRegion->dwSize));
														
							bMapFail = IMG_TRUE;

							//return ERROR_OUT_OF_STRUCTURES;
						}

						psCurrentDispatch->ui32CurrentPciRegion++;
					}
					else
					{
						KdPrint(("imgpcidd: Region %i  already mapped\n",i, pPCIRegion->pvUserAddr));
					}
				}

				// return details of the regions in found device
				psOutBlk->RegionInfo[i].dwSize			= pPCIRegion->dwSize;
				psOutBlk->RegionInfo[i].dwPhysAddress	= pPCIRegion->dwPhysAddress;
				psOutBlk->RegionInfo[i].bUsage			= pPCIRegion->bUsage;
				psOutBlk->RegionInfo[i].Pad[0]			= (bWC?1:0) | (bCapped?2:0);
				psOutBlk->RegionInfo[i].Pad[1]			= bMapFail?1:0 ;
				psOutBlk->RegionInfo[i].pvLinAddress		= (void *) pPCIRegion->pvUserAddr;
				psOutBlk->RegionInfo[i].pvKrnlLinAddress	= NULL;



			}
		}
	}

	/* Send back sizes to make sure we are singing from the same song sheet */
	psOutBlk->bPad[0] = sizeof( psOutBlk->RegionInfo[0] );
	psOutBlk->bPad[1] = sizeof( *psOutBlk );

	psOutBlk->dwBrdsFound = dwBrdsFound;

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCReadPCI8

 PURPOSE	:	Read 8 bit value from PCI config space

 PARAMETERS	:	pvInBuffer - ptr to CONFIG_READ_INBLK struct
				pvOutBuffer - ptr to CONFIG_READ_OUTBLK struct

		 		*Using Buffered Mode so In and Out buffers are common*


 RETURNS	:

*****************************************************************************/
DWORD IOCReadPCI8 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_READ_INBLK		pdwIn;
	PCONFIG_READ_OUTBLK		pdwOut;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
		* Check device has been selected
		*/
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwOut = (PCONFIG_READ_OUTBLK) pvOutBuffer;
	pdwIn = (PCONFIG_READ_INBLK) pvInBuffer;

	pdwOut->RegValue8 = PCIReadByte(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress);

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCReadPCI16

 PURPOSE	:	Read 16 bit value from PCI config space

 PARAMETERS	:

		 		*Using Buffered Mode so In and Out buffers are common*


 RETURNS	:

 *****************************************************************************/
DWORD IOCReadPCI16 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_READ_INBLK		pdwIn;
	PCONFIG_READ_OUTBLK		pdwOut;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PCONFIG_READ_INBLK) pvInBuffer;
	pdwOut = (PCONFIG_READ_OUTBLK) pvOutBuffer;

	pdwOut->RegValue16 = PCIReadWord(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress);

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCReadPCI32

 PURPOSE	:	Read 32 bit value from PCI config space

 PARAMETERS	:

		 		*Using Buffered Mode so In and Out buffers are common*


 RETURNS	:

*****************************************************************************/
DWORD IOCReadPCI32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_READ_INBLK		pdwIn;
	PCONFIG_READ_OUTBLK		pdwOut;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PCONFIG_READ_INBLK) pvInBuffer;
	pdwOut = (PCONFIG_READ_OUTBLK) pvOutBuffer;

	pdwOut->RegValue32 = PCIReadDword(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress);

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCWritePCI8

 PURPOSE	:	Write 8 bit value to PCI config space

 PARAMETERS	:	pvInBuffer - ptr to CONFIG_WRITE_INBLK

		 		*Using Buffered Mode so In and Out buffers are common*


 RETURNS	:


*****************************************************************************/
DWORD IOCWritePCI8 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PCONFIG_WRITE_INBLK) pvInBuffer;

	PCIWriteByte(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress, pdwIn->RegValue8);

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCWritePCI16

 PURPOSE	:	Write 16 bit value to PCI config space

 PARAMETERS	:


		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:

*****************************************************************************/
DWORD IOCWritePCI16 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PCONFIG_WRITE_INBLK) pvInBuffer;
	PCIWriteWord(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress, pdwIn->RegValue16);

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCWritePCI32

 PURPOSE	:	Write 32 bit value to PCI config space

 PARAMETERS	:
		 		*Using Buffered Mode so In and Out buffers are common*
 RETURNS	:

*****************************************************************************/
DWORD IOCWritePCI32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONFIG_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PCONFIG_WRITE_INBLK) pvInBuffer;

	PCIWriteDword(psCurrentDispatch->bBusNum, 0, psCurrentDispatch->bDeviceNum, (BYTE)pdwIn->RegAddress, pdwIn->RegValue32);

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCRead8

 PURPOSE	:	Read 8 bit value from PCI device region
				If the region is IO an IO cycle is performed
				If the region is Memory a memory read is performed

 PARAMETERS	:	pvInBuffer - ptr to PCI_READ_INBLK
				pvOutBuffer - ptr to PCI_READ_OUTBLK
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCRead8 (PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	PPCI_READ_OUTBLK	pdwOut;
	PCI_READ_INBLK		sdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwOut = (PPCI_READ_OUTBLK) pvOutBuffer;
	sdwIn = *((PPCI_READ_INBLK) pvInBuffer);					// copy input params to local

	/*
		Check for system IO
	*/
	if (sdwIn.Region == PCI_HOSTIO_REGION)
	{
		if (sdwIn.Address==0 || sdwIn.Address>=0x10000)
		{
			DBG_BREAK;
			pdwOut->dwErrStatus = 2;
			return ERROR_INVALID_ADDRESS;
		}

		pdwOut->RetValue8 = (BYTE)_inp(sdwIn.Address);
		pdwOut->dwErrStatus = 0;
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (sdwIn.Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 1;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[sdwIn.Region].dwPhysAddress ==0 ||
		psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_UNUSED ||
	    sdwIn.Address > psCurrentDispatch->PCIRegion[sdwIn.Region].dwSize)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 2;
		return ERROR_INVALID_ADDRESS;
	}


	switch (psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage)
	{
		case RGN_IOMAPPED:
			pdwOut->RetValue8 = (BYTE)_inp((WORD)(psCurrentDispatch->PCIRegion[sdwIn.Region].dwIOAddress + sdwIn.Address));
			pdwOut->dwErrStatus = 0;
			break;

		case RGN_MEMMAPPED:
        case RGN_ROM:
			pdwOut->RetValue8 = *(BYTE *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[sdwIn.Region].pvKrnlAddr + sdwIn.Address);
			pdwOut->dwErrStatus = 0;
			break;

		default:
			pdwOut->dwErrStatus = 1;
			break;
	}

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCRead16

 PURPOSE	:	Read 16 bit value from PCI device region
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCRead16 (PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	PPCI_READ_OUTBLK	pdwOut;
	PCI_READ_INBLK		sdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwOut = (PPCI_READ_OUTBLK) pvOutBuffer;
	sdwIn = *((PPCI_READ_INBLK) pvInBuffer);					// copy input params to local

	/*
		Check for system IO
	*/
	if (sdwIn.Region == PCI_HOSTIO_REGION)
	{
		if (sdwIn.Address==0 || sdwIn.Address>=0x10000)
		{
			DBG_BREAK;
			pdwOut->dwErrStatus = 2;
			return ERROR_INVALID_ADDRESS;
		}

		pdwOut->RetValue16 = (WORD)_inpw(sdwIn.Address);
		pdwOut->dwErrStatus = 0;
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{	
		DBG_BREAK;
		pdwOut->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (sdwIn.Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 1;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[sdwIn.Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_UNUSED ||
	    sdwIn.Address > psCurrentDispatch->PCIRegion[sdwIn.Region].dwSize)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 2;
		return ERROR_INVALID_ADDRESS;
	}


	switch (psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage)
	{
		case RGN_IOMAPPED:
			pdwOut->RetValue16 = _inpw((WORD)(psCurrentDispatch->PCIRegion[sdwIn.Region].dwIOAddress + sdwIn.Address));
			pdwOut->dwErrStatus = 0;
			break;

		case RGN_MEMMAPPED:
                case RGN_ROM:
			pdwOut->RetValue16 = *(WORD *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[sdwIn.Region].pvKrnlAddr + sdwIn.Address);
			pdwOut->dwErrStatus = 0;
			break;

		default:
			pdwOut->dwErrStatus = 1;
			return ERROR_INVALID_DATA;
	}

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCRead32

 PURPOSE	:	Read 32 bit value from PCI device region
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCRead32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	PPCI_READ_OUTBLK	pdwOut;
	PCI_READ_INBLK		sdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwOut = (PPCI_READ_OUTBLK) pvOutBuffer;
	sdwIn = *((PPCI_READ_INBLK) pvInBuffer);					// copy input params to local

	/*
		Check for system IO
	*/
	if (sdwIn.Region == PCI_HOSTIO_REGION)
	{
		if (sdwIn.Address==0 || sdwIn.Address>=0x10000)
		{
			DBG_BREAK;
			pdwOut->dwErrStatus = 2;
			return ERROR_INVALID_ADDRESS;
		}

		pdwOut->RetValue32 = (DWORD)_inpd(sdwIn.Address);
		pdwOut->dwErrStatus = 0;
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{	
		DBG_BREAK;
		pdwOut->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (sdwIn.Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 1;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[sdwIn.Region].dwPhysAddress ==0 ||
		psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_UNUSED ||
	    sdwIn.Address > psCurrentDispatch->PCIRegion[sdwIn.Region].dwSize)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 2;
		return ERROR_INVALID_ADDRESS;
	}


	switch (psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage)
	{
		case RGN_IOMAPPED:
			pdwOut->RetValue32 = _inpd(psCurrentDispatch->PCIRegion[sdwIn.Region].dwIOAddress + sdwIn.Address);
			pdwOut->dwErrStatus = 0;
			break;

		case RGN_MEMMAPPED:
                case RGN_ROM:
			pdwOut->RetValue32 = *(DWORD *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[sdwIn.Region].pvKrnlAddr + sdwIn.Address);
			pdwOut->dwErrStatus = 0;
			break;

		default:
			pdwOut->dwErrStatus = 1;
			return ERROR_INVALID_DATA;
	}

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCWrite8

 PURPOSE	:	Write 8 bit value to a PCI device region

 PARAMETERS	:	pvInBuffer - ptr to PCI_WRITE_INBLK
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCWrite8 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PPCI_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwIn = (PPCI_WRITE_INBLK) pvInBuffer;

	/*
		Check for system IO
	*/
	if (pdwIn->Region == PCI_HOSTIO_REGION)
	{
		if (pdwIn->Address==0 || pdwIn->Address>=0x10000)
		{
			DBG_BREAK;
			return ERROR_INVALID_ADDRESS;
		}

		_outp(pdwIn->Address, pdwIn->Value8);
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (pdwIn->Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    pdwIn->Address > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		return ERROR_INVALID_ADDRESS;
	}

	switch (psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage)
	{
		case RGN_IOMAPPED:
			_outp((WORD)(psCurrentDispatch->PCIRegion[pdwIn->Region].dwIOAddress+pdwIn->Address), pdwIn->Value8);
			break;

		case RGN_MEMMAPPED:
                case RGN_ROM:
			*(BYTE *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address) = pdwIn->Value8;
			break;

		default:
			break;
	}

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCWrite16

 PURPOSE	:	Write 16 bit value to a PCI device region
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCWrite16 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PPCI_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwIn = (PPCI_WRITE_INBLK) pvInBuffer;

	/*
		Check for system IO
	*/
	if (pdwIn->Region == PCI_HOSTIO_REGION)
	{
		if (pdwIn->Address==0 || pdwIn->Address >= 0x10000)
		{
			DBG_BREAK;
			return ERROR_INVALID_ADDRESS;
		}

		_outpw(pdwIn->Address, pdwIn->Value16);
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (pdwIn->Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    pdwIn->Address > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		return ERROR_INVALID_ADDRESS;
	}

	switch (psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage)
	{
		case RGN_IOMAPPED:
			_outpw((WORD)(psCurrentDispatch->PCIRegion[pdwIn->Region].dwIOAddress+pdwIn->Address), pdwIn->Value16);
			break;

		case RGN_MEMMAPPED:
                case RGN_ROM:
			*(WORD *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address) = pdwIn->Value16;
			break;

		default:
			break;
	}

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCWrite32

 PURPOSE	:	Write 32 bit value to a PCI device region
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:

*****************************************************************************/
DWORD IOCWrite32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PPCI_WRITE_INBLK		pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwIn = (PPCI_WRITE_INBLK) pvInBuffer;

	/*
		Check for system IO
	*/
	if (pdwIn->Region == PCI_HOSTIO_REGION)
	{
		if (pdwIn->Address==0 || pdwIn->Address >= 0x10000)
		{
			DBG_BREAK;
			return ERROR_INVALID_ADDRESS;
		}

		_outpd(pdwIn->Address, pdwIn->Value32);
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (pdwIn->Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    pdwIn->Address > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		return ERROR_INVALID_ADDRESS;
	}

	switch (psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage)
	{
		case RGN_IOMAPPED:
			_outpd(psCurrentDispatch->PCIRegion[pdwIn->Region].dwIOAddress+pdwIn->Address, pdwIn->Value32);
			break;

		case RGN_MEMMAPPED:
                case RGN_ROM:
			*(DWORD *)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address) = pdwIn->Value32;
			break;

		default:
			break;
	}

	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCWriteRead32

 PURPOSE	:	Write a 32 bit value to PCI device region
				and read it back again ASAP
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCWriteRead32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	PPCI_READ_OUTBLK	pdwOut;
	PCI_WRITE_INBLK		sdwIn;
	PDWORD  pdwAddress;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */
	
	pdwOut = (PPCI_READ_OUTBLK) pvOutBuffer;
	sdwIn = *((PPCI_WRITE_INBLK) pvInBuffer);					// copy input params to local

	/*
		Check for system IO
	*/
	if (sdwIn.Region == PCI_HOSTIO_REGION)
	{
		if (sdwIn.Address==0 || sdwIn.Address>=0x10000)
		{
			DBG_BREAK;
			return ERROR_INVALID_ADDRESS;
		}

		_outpd(sdwIn.Address, sdwIn.Value32);
		pdwOut->RetValue32 = _inpd(sdwIn.Address);
		pdwOut->dwErrStatus = 0;
		return NO_ERROR;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{	
		DBG_BREAK;
		pdwOut->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if (sdwIn.Region >= MAX_PCI_REGIONS)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[sdwIn.Region].dwPhysAddress ==0 ||
		psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_UNUSED ||
	    sdwIn.Address > psCurrentDispatch->PCIRegion[sdwIn.Region].dwSize)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 1;
		return ERROR_INVALID_ADDRESS;
	}

	switch (psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage)
	{
		case RGN_IOMAPPED:
			_outpd((WORD)(psCurrentDispatch->PCIRegion[sdwIn.Region].dwIOAddress + sdwIn.Address), sdwIn.Value32);
			pdwOut->RetValue32 = _inpd((WORD)(psCurrentDispatch->PCIRegion[sdwIn.Region].dwIOAddress + sdwIn.Address));
			pdwOut->dwErrStatus = 0;
			break;

		case RGN_MEMMAPPED:
        case RGN_ROM:
			pdwAddress = (PDWORD)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[sdwIn.Region].pvKrnlAddr + sdwIn.Address);
			*pdwAddress = sdwIn.Value32;
			pdwOut->RetValue32 = *pdwAddress;
			pdwOut->dwErrStatus = 0;
			break;

		default:
			pdwOut->dwErrStatus = 1;
			return ERROR_INVALID_DATA;
	}

	return NO_ERROR;
}



/*****************************************************************************
 FUNCTION	:	IOCFillMem32

 PURPOSE	:	Fill region with 32 bit value
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:

*****************************************************************************/
DWORD IOCFillMem32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PPCI_FILL_INBLK			pdwIn;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		return ERROR_DEV_NOT_EXIST;
	}

	pdwIn = (PPCI_FILL_INBLK) pvInBuffer;

	/*
	 *	Check if it's a valid request
	 */
	if ((pdwIn->Region >= MAX_PCI_REGIONS) ||
		(pdwIn->dwSize == 0) ||
		(pdwIn->dwSize&3))
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_IOMAPPED ||
	    (pdwIn->Address + pdwIn->dwSize) > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		return ERROR_INVALID_ADDRESS;
	}


	memset((PVOID)((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address),
			pdwIn->Value32,
			pdwIn->dwSize);

	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCReadMulti32

 PURPOSE	:	Read repeatedly sequential addresses from PCI device region
 				return last value read
 				only used for test purposes

 				IO Reads are not supported
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:


*****************************************************************************/
DWORD IOCReadMulti32 (PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	PPCI_READ_OUTBLK	pdwOut;
	PCI_READM_INBLK   	sdwIn;

	DWORD*	pdwMem;
	DWORD	dwDwordCount;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwOut = (PPCI_READ_OUTBLK) pvOutBuffer;
	sdwIn = *((PPCI_READM_INBLK) pvInBuffer);					// copy input params to local

	/*
		Check for system IO
	*/
	if (sdwIn.Region == PCI_HOSTIO_REGION)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 3;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{	
		DBG_BREAK;
		pdwOut->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	/*
	 *	Check if it's a valid request
	 */
	if ((sdwIn.Region >= MAX_PCI_REGIONS) ||
		(sdwIn.dwSize==0) ||
		(sdwIn.dwSize&3))
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 1;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[sdwIn.Region].dwPhysAddress ==0 ||
		psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_UNUSED ||
	    psCurrentDispatch->PCIRegion[sdwIn.Region].bUsage == RGN_IOMAPPED ||
	    (sdwIn.Address + sdwIn.dwSize) > psCurrentDispatch->PCIRegion[sdwIn.Region].dwSize)
	{
		DBG_BREAK;
		pdwOut->dwErrStatus = 2;
		return ERROR_INVALID_ADDRESS;
	}

	pdwMem = (DWORD*) ((IMG_UINTPTR)psCurrentDispatch->PCIRegion[sdwIn.Region].pvKrnlAddr + sdwIn.Address);
	dwDwordCount = sdwIn.dwSize >> 2;
	while (dwDwordCount--)
	{
		pdwOut->RetValue32 = *pdwMem;
	}
	pdwOut->dwErrStatus = 0;
	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCWriteBuffer

 PURPOSE	:	Write a buffer to a PCI memory area

 PARAMETERS	:	pvInBuffer - ptr to BUFFER_WRITE_INBLK struct
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:

*****************************************************************************/
DWORD IOCWriteBuffer (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PBUFFER_INBLK			pdwIn;
	BYTE*					pMemSrc;
	BYTE*					pMemDest;
	DWORD*					pdwMemSrc;
	DWORD*					pdwMemDest;
	DWORD                   i, dwByteCount;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwIn = (PBUFFER_INBLK) pvInBuffer;

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{	
		DBG_BREAK;
		pdwIn->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	dwByteCount = pdwIn->dwSize;

	/*
	 *	Check if it's a valid request
	 */
	if ((pdwIn->Region >= MAX_PCI_REGIONS) ||
		(dwByteCount==0))
	{
		pdwIn->dwErrStatus = 1;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress ==0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_IOMAPPED ||
	    (pdwIn->Address + dwByteCount) > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		pdwIn->dwErrStatus = 2;
		return ERROR_INVALID_ADDRESS;
	}

	pMemSrc =  (BYTE*) (pdwIn->pBuff);
	pMemDest = (BYTE*) ((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address);

	/*
		Align Dest buffer to DWORDS
		Notice the fall through on the switch is intentional
	*/
	switch (((IMG_UINTPTR)pMemDest) & 0x03)
	{
		case 1:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;
			if (dwByteCount==0) break;

		case 2:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;
			if (dwByteCount==0) break;

		case 3:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;

		default:
			break;
	}

	pdwMemDest = (DWORD *) pMemDest;
	pdwMemSrc = (DWORD *) pMemSrc;

	// Copy a dword at a time
	for (i=0; i < (dwByteCount >> 2); i++)
	{
		*pdwMemDest++ = *pdwMemSrc++;
	}

	/*
		finish off with bytes
	*/
	dwByteCount = dwByteCount & 0x03;
	pMemDest = (BYTE *) pdwMemDest;
	pMemSrc = (BYTE *) pdwMemSrc;

	for (i=0; i < dwByteCount; i++)
	{
		*pMemDest++ = *pMemSrc++;
	}

	pdwIn->dwErrStatus = 0;
	return NO_ERROR;
}

/*****************************************************************************
 FUNCTION	:	IOCReadBuffer

 PURPOSE	:	Read PCI memory to buffer

 PARAMETERS	:	pvInBuffer - ptr to BUFFER_READ_INBLK struct
				pvOutBuffer - ptr to BUFFER_READ_OUTBLK struct
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:

*****************************************************************************/
DWORD IOCReadBuffer (PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PBUFFER_INBLK			pdwIn;
	BYTE*					pMemSrc;
	BYTE*					pMemDest;
	DWORD*					pdwMemSrc;
	DWORD*					pdwMemDest;

	DWORD                   i, dwByteCount;

	const PDISPATCH psCurrentDispatch = FindCurrentDispatch(); /* Find the dispatch info associated with this IOCTL call  */

	pdwIn = (PBUFFER_INBLK) pvInBuffer;

	/*
	 * Check device has been selected
	 */
	if (psCurrentDispatch == NULL || !psCurrentDispatch->bFoundDevice)
	{
		DBG_BREAK;
		pdwIn->dwErrStatus = 99;
		return ERROR_DEV_NOT_EXIST;
	}

	dwByteCount = pdwIn->dwSize;

	/*
	 *	Check if it's a valid request
	 */
	if ((pdwIn->Region >= MAX_PCI_REGIONS) ||
		(dwByteCount==0))
	{
		DBG_BREAK;
		pdwIn->dwErrStatus = 2;
		return ERROR_INVALID_DATA;
	}

	/*
	 * Check it's valid for this region
	 */
	if (psCurrentDispatch->PCIRegion[pdwIn->Region].dwPhysAddress == 0 ||
		psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_UNUSED ||
	    psCurrentDispatch->PCIRegion[pdwIn->Region].bUsage == RGN_IOMAPPED ||
	    (pdwIn->Address + dwByteCount) > psCurrentDispatch->PCIRegion[pdwIn->Region].dwSize)
	{
		DBG_BREAK;
		pdwIn->dwErrStatus = 1;
		return ERROR_INVALID_ADDRESS;
	}

	pMemDest =  (BYTE *) (pdwIn->pBuff);
	pMemSrc = (BYTE *) ((IMG_UINTPTR)psCurrentDispatch->PCIRegion[pdwIn->Region].pvKrnlAddr + pdwIn->Address);

	/*
		Align Source buffer to DWORDS
		Notice the fall through on the switch is intentional
	*/
	switch (((IMG_UINTPTR)pMemSrc) & 0x03)
	{
		case 1:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;
			if (dwByteCount==0) break;

		case 2:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;
			if (dwByteCount==0) break;

		case 3:
			*pMemDest++ = *pMemSrc++;
			dwByteCount--;

		default:
			break;
	}

	pdwMemDest = (DWORD *) pMemDest;
	pdwMemSrc = (DWORD *) pMemSrc;

	// Copy a dword at a time
	for (i=0; i < (dwByteCount >> 2); i++)
	{
		*pdwMemDest++ = *pdwMemSrc++;
	}


	/*
		finish off with bytes
	*/
	dwByteCount = dwByteCount & 0x03;
	pMemDest = (BYTE *) pdwMemDest;
	pMemSrc = (BYTE *) pdwMemSrc;

	for (i=0; i < dwByteCount; i++)
	{
		*pMemDest++ = *pMemSrc++;
	}

	pdwIn->dwErrStatus = 0;
	return NO_ERROR;
}


/*****************************************************************************
 FUNCTION	:	IOCBufferInfo

 PURPOSE	:	Returns Information on VxD reserved buffer

 PARAMETERS	:	pvOutBuffer - ptr to HOSTBUFFER_INFO_OUTBLK struct
		 		*Using Buffered Mode so In and Out buffers are common*

 RETURNS	:	NO_ERROR always
*****************************************************************************/
DWORD IOCBufferInfo(PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PHOSTBUFFER_INFO_OUTBLK		 pdwOut;

	pdwOut = (PHOSTBUFFER_INFO_OUTBLK) pvOutBuffer;

	/*
		Fill out the info required
	*/
	pdwOut->dw64PhysAddress = 0;
	pdwOut->MemSize         = 0;
	
	pdwOut->wMajorRev = 0;
	pdwOut->wMinorRev = 0;

	pdwOut->pv32LinAddress = NULL;

	return NO_ERROR;
}


/*****************************************************************************
	Internal Functions
*****************************************************************************/


/*****************************************************************************
 FUNCTION	:	MapPCIRegion

 PURPOSE	:	Looks at a specific region defined in a PCI devices
				config space and records whether it's memory, IO or unused
				and it's limits
				The results get stored in the PCI region array

 PARAMETERS	:	Region to map

 RETURNS        :       Type of region - memory, ROM, IO or unused


*****************************************************************************/
INTERNAL VOID MapPCIRegion (BYTE Bus, BYTE Device, MEMORY_CONTEXT* pRegionStr, int Region)
{
	DWORD	dwBaseAddress;
	DWORD	dwAddressRange;

	dwBaseAddress = PCIReadDword(Bus, 0, Device, REGION_ADDRESS[Region]);

	if (dwBaseAddress == 0)
	{
		pRegionStr->bUsage = RGN_UNUSED;
		pRegionStr->dwSize = 0;
		return;
	}
	else if (REGION_ADDRESS[Region] == 0x30)
	{
		pRegionStr->bUsage = RGN_ROM;
		pRegionStr->dwPhysAddress = dwBaseAddress & 0xFFFFF800;
	}
	else if (dwBaseAddress & 0x01)
	{
		pRegionStr->bUsage = RGN_IOMAPPED;
		pRegionStr->dwIOAddress = dwBaseAddress & 0xFFFC;
	}
	else
	{
		pRegionStr->bUsage = RGN_MEMMAPPED;
		pRegionStr->dwPhysAddress = dwBaseAddress & 0xFFFFFFF0;
	}

	/*
		Find size of region
	*/
//	_asm{cli};

	PCIWriteDword(Bus, 0, Device, REGION_ADDRESS[Region], 0xFFFFFFFF);
	dwAddressRange = PCIReadDword(Bus, 0, Device, REGION_ADDRESS[Region]);
	PCIWriteDword(Bus, 0, Device, REGION_ADDRESS[Region], dwBaseAddress);

//	_asm{sti};

	pRegionStr->dwSize		= (~(dwAddressRange & 0xFFFFFFF0))+1;
	pRegionStr->dwNumPages	= BYTES_TO_PAGES(pRegionStr->dwSize);
}


/***********************************************************************************
 Function Name 		: IOCMoveDevice
 Inputs         	:
 Outputs        	:
 Returns        	:
 Globals Used    	:
 Description    	:
************************************************************************************/
DWORD IOCMoveDevice(PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PMOVE_DEVICE_INOUTBLK	psIn=(PMOVE_DEVICE_INOUTBLK)pvInBuffer;
	BYTE					byDev;
	DWORD					dwVenDevID;
	DWORD					dwOrgBARVal;
	DWORD					dwNewBARVal;
	DWORD					dwBARSize;
	DWORD					dwBARSizeMask;
	BYTE					byBARAddr;

	if (pvOutBuffer);

	/* First set failure in the ret struct */
	psIn->bMovedOk=FALSE;
	psIn->bDevFound=FALSE;

	/* First find the device */
	for (byDev=0; byDev < 32; byDev++)
	{
		// Read devices low 2 WORDS of config space
		// Word 0 is VendorID, Word 1 is DeviceID
		dwVenDevID = PCIReadDword((BYTE)psIn->wBus, 0, byDev, 0);

		// Are we matching to a Vendor and Device ID
		if (dwVenDevID == (DWORD)(psIn->wVendorID + (psIn->wDeviceID << 16)))
		{
			psIn->bDevFound = TRUE;
			break;
		}
	}

	/* Return if we did not find the device */
	if (psIn->bDevFound == FALSE)
	{
		DBG_BREAK;
		return ERROR_NOT_FOUND;
	}

	/* Now find size of mem BAR */
	byBARAddr = (BYTE)(0x10 + (sizeof(DWORD) * psIn->dwBarNum));
	dwOrgBARVal = PCIReadDword((BYTE)psIn->wBus, 0, byDev, byBARAddr);
	PCIWriteDword( (BYTE) psIn->wBus, 0, byDev, byBARAddr, 0xffffffff );
	dwBARSizeMask = PCIReadDword((BYTE)psIn->wBus, 0, byDev, byBARAddr);
	PCIWriteDword( (BYTE) psIn->wBus, 0, byDev, byBARAddr, dwOrgBARVal );
	dwBARSizeMask |= 0xF;				// Overwrite flags with bits
	dwBARSize = (~dwBARSizeMask) + 1;	// Convert to memory size

	/* Check that new location is on a multiple of the BAR size */
	if ((psIn->dwNewPhys&dwBARSizeMask) != psIn->dwNewPhys)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/* Check if hw is already at new location */
	if ((dwOrgBARVal & 0xfffffff0) == psIn->dwNewPhys)
	{
		psIn->bMovedOk=TRUE;
		return NO_ERROR;
	}

	/* Check if slot is free */
	if (IsPhysSlotFree(psIn->dwNewPhys, dwBARSize) == FALSE)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/* Move the BAR */
	dwNewBARVal=(psIn->dwNewPhys & 0xfffffff0) + (dwOrgBARVal & 0xf);
	PCIWriteDword((BYTE)psIn->wBus, 0, byDev, byBARAddr, dwNewBARVal);

	/* Now indicate we have done it */
	psIn->bMovedOk = TRUE;

	return NO_ERROR;
}

INTERNAL BOOL IsPhysSlotFree(DWORD dwPhysAddr, DWORD dwSizeRequired)
{
	DWORD	dwBus;
	DWORD	dwFunc;
	DWORD	dwDev;
	DWORD	dwMemMask=0;	//Each bit in the dwUsedMask reprosents a 64Mb region of phyical memory starting 0x80000000
	DWORD	dwData;
	DWORD	dwDeviceID;
	DWORD	dwVendorID;
	DWORD	dwIdx;
	DWORD	dwRquMask;
	DWORD	dwRquSlots;
	DWORD	dwRquBitPos;

	/*Check all the busses*/
	for (dwBus=0; dwBus < PCI_MAX_BUSES; dwBus++)
	{
		/*Check all devices on that bus*/
		for (dwDev=0; dwDev < PCI_MAX_DEVS; dwDev++)
		{
			/*Check all functions on that device*/
			for (dwFunc=0; dwFunc < PCI_MAX_FUNCS; dwFunc++)
			{
				/* Get the deviceID and vendor ID */
				dwVendorID=HostPCIReadWord(dwBus, dwFunc, dwDev, PCI_CONFIG_VENDOR_ID);
				dwDeviceID=HostPCIReadWord(dwBus, dwFunc, dwDev, PCI_CONFIG_DEVICE_ID);

				if (dwFunc == 0)
				{
					switch (HostPCIReadWord(dwBus, dwFunc, dwDev, 0x0A))
					{
						case 0x0600:				/* HOST 2 PCI BRIDGE CLASS */
							dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x10);
							break;

						case 0x0604:				/* PCI 2 PCI BRIDGE CLASS */
							dwData = HostPCIReadDword(dwBus, dwFunc, dwDev, 0x20);
							if (dwData != 0xFFF0)
							{
								dwMemMask |= UpdateBridgeMask(dwData);
							}
							dwData = HostPCIReadDword(dwBus, dwFunc, dwDev, 0x24);
							if (dwData != 0xFFF0)
							{
								dwMemMask |= UpdateBridgeMask(dwData);
							}
							break;

						default:
							if ((HostPCIReadByte(dwBus, dwFunc, dwDev, PCI_CONFIG_HEADER_TYPE) & 0x7F) == 0)
							{
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x10);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x14);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x18);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x1C);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x20);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x24);
								dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x30);
							}
							break;
					}

					/*
						If this is first function on this device and it is not a multi-function device,
						We must move on to next device
					*/
					if ((HostPCIReadByte(dwBus, dwFunc, dwDev, PCI_CONFIG_HEADER_TYPE) & PCI_HEADER_MULTIFUNC) == 0 )
					{
	                    break;	/* out of dwFunc for loop */
					}
				}
				else
				{
					if ((HostPCIReadByte(dwBus, dwFunc, dwDev, PCI_CONFIG_HEADER_TYPE) & 0x7F) == 0)
					{
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x10);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x14);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x18);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x1C);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x20);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x24);
						dwMemMask |= UpdateMask(dwBus, dwFunc, dwDev, 0x30);
					}
				}

				if (dwDeviceID == 0x7110 && dwVendorID == 0x8086)
				{
					/*****************  ALERT ***************
					Ok, the reserved bits on sSlotNum should be zero but if they are the machine
					hangs when we call HalGetBusData() for Bus 0, dev 7 fun 3 on a Dell XPS R400
					which is a Intel PIIX4 Power management function.
					But if sSlotNum.u.bits.Reserved=1 it does not hang, but HalGetBusData() always
					return 2.

					So what we will do is detect when we are looking at function zero of the
					Intel device that has the Intel PIIX4 Power management function on it and then
					skip that device.

					It is very unlikely that a SGX is on an Intel PCI to ISA bridge and if it is
					then this is why we cannot find it.
					*/
					break;		/* out of dwFunc for loop */
				}
			}/*End loop for each function*/
		}/*End loop for each device*/
	}/*End loop for each bus*/

	/* How many 64Mb slots do we need */
	dwRquSlots = (dwSizeRequired + (64*1024*1024) - 1) / (64*1024*1024);
	dwRquMask = 0;
	dwRquBitPos = (dwPhysAddr-0x80000000) / (64*1024*1024);
	for (dwIdx=0; dwIdx < dwRquSlots; dwIdx++)
	{
		dwRquMask|=1 << (dwRquBitPos + dwIdx);
	}

	if ((dwMemMask & dwRquMask) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

INTERNAL DWORD UpdateMask(DWORD dwBus, DWORD dwFunc, DWORD dwDev, DWORD dwReg)
{
	DWORD dwData, dwBase;
	DWORD dwSize, dwMask;

	dwData = HostPCIReadDword(dwBus, dwFunc, dwDev, dwReg);

	if (dwData == 0)
	{
		return (0);
	}

	if (dwReg == 0x30)
	{
		dwBase = dwData & 0xFFFFF800;
	}
	else
	{
		if (((dwData & 0x01) != 0) ||
		    ((dwData & 0x06) == 0x04))
		{
			return (0);
		}

		dwBase = dwData & 0xFFFFFFF0;
	}

	if ((dwBase & 0x80000000) == 0)
	{
		return (0);
	}

	dwBase = (dwBase & 0x7FFFFFFF) >> 26;
	HostPCIWriteDword(dwBus, dwFunc, dwDev, dwReg, 0xFFFFFFFF);
	dwSize = !(HostPCIReadDword(dwBus, dwFunc, dwDev, dwReg) * 0xFFFFFFF0);

	dwSize = (dwSize >> 26) + 1;
	dwMask = (1 << (WORD)dwSize) - 1;


	HostPCIWriteDword(dwBus, dwFunc, dwDev, dwReg, dwData);

	return (dwMask << dwBase);
}

INTERNAL DWORD UpdateBridgeMask(DWORD dwData)
{
	DWORD dwSize, dwMask, dwBase;

	if ((dwData & 0x8000) == 0)
	{
		return (0);
	}

	dwBase = (dwData & 0x7FFF) >> 10;				/* subtract 2GB and scale in 64MB units */

	dwSize = (dwData >> 16) - (dwData & 0xFFFF);	/* get size of allocation */
	dwSize = (dwSize >> 10) + 1;
/*
 * Convert This to a number of bits
 * e.g.  64MB  -> 0001
 *      128MB  -> 0003
 *	..
 *	512MB  -> 00FF
 */
	dwMask = (1 << (WORD)dwSize) - 1;

/*
 * Now use this and shift for base address
 * as update for mem mask
 */
	return (dwMask << dwBase);
}

/*****************************************************************************
 FUNCTION	:	GetUserModeMapping
 PURPOSE	:	Looks over all dispatches to see if we have already mapped a
				kernel address to the same physical location.
*****************************************************************************/
INTERNAL PVOID GetUserModeMapping(PDISPATCH psCurrentDispatch, DWORD dwPhysAddr)
{
	IMG_UINT32 i;
	
	for(i=0; i < MAX_PCI_REGIONS; i++)
	{
		if(	psCurrentDispatch->PCIRegion[i].dwPhysAddress == dwPhysAddr)
		{
			return psCurrentDispatch->PCIRegion[i].pvUserAddr;
		}
	}

	return NULL;
}

/*****************************************************************************
 FUNCTION	:	GetKrnlModeMapping
 PURPOSE	:	Looks over all dispatches to see if we have already mapped a
				kernel address to the same physical location.
*****************************************************************************/
INTERNAL PLONG_TERM_MAPPING GetKrnlModeMapping(DWORD dwPhysAddr)
{
	IMG_UINT32 i;
	
	for(i=0; i < MAX_PCI_REGIONS; i++)
	{
		if(	gasLongTermMapping[i].dwPhysAddress == dwPhysAddr || gasLongTermMapping[i].dwPhysAddress == 0 )
		{
			return &gasLongTermMapping[i];
		}
	}

	return NULL;
}

/***********************************************************************************
 Function Name 		: FreeLongTermMappings
 Description    	: Frees allocations made for long term mappings to physical mem
************************************************************************************/

void FreeLongTermMappings()
{
	IMG_UINT32 i;

	for(i=0; i < MAX_PCI_REGIONS; i++)
	{
		if(	gasLongTermMapping[i].dwPhysAddress != 0 )
		{
			HostUnMapLongTermMapping(	gasLongTermMapping[i].pvKrnlAddress, 
										gasLongTermMapping[i].dwSize, 
										gasLongTermMapping[i].bFileMapped);
		}
	}
}

/***********************************************************************************
 Function Name 		: FreeThreadAllocations
 Description    	: Frees allocations made for current thread
************************************************************************************/

void FreeThreadAllocations(PTHREAD_CONTEXT const psThreadContext)
{
	IMG_UINT32 i;
	PCONTEXT_BUFFER psContextBuffer, psNextContextBuffer;

	if(psThreadContext == NULL)
	{
		return;
	}

	psContextBuffer = psThreadContext->psContextBuffer;

	while(psContextBuffer != NULL)
	{
		psNextContextBuffer = psContextBuffer->Next;

		for(i=0; i < psContextBuffer->dwNumContexts; i++)
		{
			FreeContext(&psContextBuffer->psContext[i]);
		}

		ExFreePoolWithTag(psContextBuffer, 'dICP');
		psContextBuffer = psNextContextBuffer;
	}

	/* Make safe to use again */
	RtlZeroMemory(psThreadContext, sizeof(THREAD_CONTEXT));
}

/***********************************************************************************
 Function Name 		: FreePciRegionAllocations
 Description    	: Frees allocations made for current pci region
************************************************************************************/

void FreePciRegionAllocations(PDISPATCH const psDispatch)
{
	IMG_UINT32 i;

	if(psDispatch == NULL)
	{
		return;
	}

	for(i=0; i < psDispatch->ui32CurrentPciRegion; i++)
	{
		HostUnMapPhysToLin(	&psDispatch->PCIRegion[i] ); //Free Kernel Mapping
	}
}

/***********************************************************************************
 Function Name 		: FreeDispatch
 Description    	: Frees allocations associated with a dispatch
************************************************************************************/

void FreeProcessContext(PPROCESS_CONTEXT psProcessContext)
{
	int i;

	for( i=0; i < MAX_THREAD_CONTEXTS; i++ )
	{
		if( psProcessContext->asThreadContext[i].hThreadId != NULL )
		{
			FreeThreadAllocations(&psProcessContext->asThreadContext[i]);
		}
	}

	RtlZeroMemory(psProcessContext, sizeof(PROCESS_CONTEXT));
}


/***********************************************************************************
 Function Name 		: IOCAllocNonPaged
 Returns        	: psContext pointer, user mode address, and number of pages allocated
					  The user mode data area contains the physical addresses of the pages
************************************************************************************/
DWORD IOCAllocMemory(PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PMEMORY_CONTEXT psContext;
	PVOID pvUserAddr;
	PCONTIG_NPP_INFO const pOutVals = pvOutBuffer;
	DWORD dwNumPages;

	const HANDLE hThisProcessId = HostGetCurrentProcessId(); /* Find the process ID associated with this IOCTL call  */
	PTHREAD_CONTEXT psThreadContext;

	if( pvInBuffer == NULL || pvOutBuffer == NULL || hThisProcessId == NULL ) 
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	/* Don't allow different processes to allocate memory at the same time */
	if (gsAllocationContext.hProcessId != NULL && gsAllocationContext.hProcessId != hThisProcessId)
	{
		FreeProcessContext(&gsAllocationContext); /* Free last used process memory. Slower but safe? */
	}
	
	if (gsAllocationContext.hProcessId == NULL)
	{
		const PDISPATCH psDispatch = FindDispatch(hThisProcessId); /* Find the dispatch info associated with this IOCTL call  */

		if( psDispatch == NULL ) 
		{
			DBG_BREAK;
			return ERROR_INVALID_DATA;
		}

		gsAllocationContext.hProcessId = hThisProcessId;
		gsAllocationContext.pProcess = HostGetCurrentProcess();
	}

	psThreadContext = FindThreadContext(&gsAllocationContext, HostGetCurrentThreadId()); /* Get a thread context for this allocation */

	if(psThreadContext == NULL)
	{
		/* Get a new thread context */
		psThreadContext = FindFreeThreadContext(&gsAllocationContext);

		if(psThreadContext != NULL)
		{
			psThreadContext->hThreadId = HostGetCurrentThreadId();
		}
		else
		{
			DBG_BREAK;
			return ERROR_INVALID_DATA;
		}
	}

	dwNumPages = *(DWORD *) pvInBuffer;

	if (dwNumPages == 0 || dwNumPages > 0x100000)	/* Check for invalid values */
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}
	else if(psThreadContext->psNonContigContext != NULL && dwNumPages == 1) /* If we have memory left in our non contiguous allocation use this */
	{
		/* This is fast and uses much less virtual memory mappings */
		psContext = psThreadContext->psNonContigContext;
		pvUserAddr = (PVOID) (((char *) psContext->pvUserAddr) + psContext->dwUsedPages * PAGE_SIZE);

		psContext->dwUsedPages++;

		if(psContext->dwUsedPages >= psContext->dwNumPages)
		{
			psThreadContext->psNonContigContext = NULL;
		}
	}
	else /* Allocate memory as per normal */
	{
		if(psThreadContext->psContextBuffer == NULL || psThreadContext->psContextBuffer->dwNumContexts >= DEFAULT_CONTEXTS_IN_BUFFER)
		{
			PCONTEXT_BUFFER psNextBuffer = psThreadContext->psContextBuffer;
			SIZE_T BufferSize = sizeof(CONTEXT_BUFFER) + (sizeof(MEMORY_CONTEXT) * DEFAULT_CONTEXTS_IN_BUFFER);

			if((psThreadContext->psContextBuffer = ExAllocatePoolWithTag(PagedPool, BufferSize, 'dICP')) == NULL)
			{
				DBG_BREAK;
				return ERROR_NOT_ENOUGH_MEMORY;
			}

			psThreadContext->psContextBuffer->Next = psNextBuffer;
			psThreadContext->psContextBuffer->dwNumContexts = 0;
			psThreadContext->psContextBuffer->psContext = (void *) (((char *) psThreadContext->psContextBuffer) + sizeof(CONTEXT_BUFFER));
		}

		psContext = &psThreadContext->psContextBuffer->psContext[psThreadContext->psContextBuffer->dwNumContexts];
		if (psContext == NULL)
		{
			DBG_BREAK;
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		if(!HostGetContext(psContext, dwNumPages)) //Allocate pages
		{
			DBG_BREAK;
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		if(psContext->dwUsedPages < psContext->dwNumPages)
		{
			psThreadContext->psNonContigContext = psContext; /*see if we can still keep using this context */
		}

		psThreadContext->psContextBuffer->dwNumContexts++;

		pvUserAddr = psContext->pvUserAddr;
	}

	psThreadContext->dwNumAllocations++;

	pOutVals->hContextID	= (IMG_SID) psThreadContext->hThreadId;
	pOutVals->pv32UserMode	= (void * POINTER_32) pvUserAddr;
	pOutVals->dw64PhysAddr	= MmGetPhysicalAddress(pvUserAddr).QuadPart;
	pOutVals->dwNumPages	= dwNumPages;
	
	return NO_ERROR;
}

/***********************************************************************************
 Function Name 		: IOCFreeNonPaged
 Inputs         	:
 Outputs        	:
 Returns        	:
 Globals Used    	:
 Description    	: We do our own garbage collection, but if the process doesn't
					  close (i.e. IPCServer) then we still need to track our allocs.
************************************************************************************/
DWORD IOCFreeMemory(PVOID pvInBuffer, PVOID pvOutBuffer)
{
	const HANDLE hThisProcessId = HostGetCurrentProcessId(); /* Find the process ID associated with this IOCTL call  */
	PTHREAD_CONTEXT psThreadContext;
	DWORD dwThreadId;

	/* If we don't know what thread this allocation belongs to just return and do garbage collection later */
	if(pvInBuffer == NULL)
	{
		return NO_ERROR;
	}

	dwThreadId = *(DWORD *) pvInBuffer;
	if(dwThreadId == 0) 
	{
		return NO_ERROR;
	}

	/* Don't allow a different process to free memory */
	if (gsAllocationContext.hProcessId != hThisProcessId || gsAllocationContext.hProcessId == NULL)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	psThreadContext = FindThreadContext(&gsAllocationContext, (HANDLE) dwThreadId);

	if (psThreadContext == NULL) 
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	if(psThreadContext->dwNumAllocations <= 1)
	{
		/* If something goes wrong and we free memory more than once this is still safe */
		FreeThreadAllocations(psThreadContext);
		psThreadContext->dwNumAllocations = 0;
	}
	else
	{
		psThreadContext->dwNumAllocations--;
	}
	return NO_ERROR;
}

// Use weird maths to avoid 32-bit overflow problems with big values
// If a and b are multiples of 1024, then a/(b/1024) == a*1024/b
#define MEG_TO_PAGES(m) ((m)*1024/(PAGE_SIZE/1024))

// We can only have one burnmem active, so cache info locally, keeps interface the same as other non-paged pool allocs
static MEMORY_CONTEXT gsBurnMemInfo;

/***********************************************************************************
 Function Name 		: IOCMapBurnMem
 Inputs         	:
 Outputs        	:
 Returns        	: psContext pointer, user mode address, and number of pages allocated
					  The user mode data area contains the physical addresses of the pages
 Globals Used    	: gsBurnMemInfo
 Description    	: Map a user mode linear pointer to the burnmem region
************************************************************************************/
DWORD IOCMapBurnMem(PVOID pvInBuffer, PVOID pvOutBuffer)
{
	PCONTIG_NPP_INFO const pOutVals = pvOutBuffer;
	DWORD dwTopOfRam;

	if (pvInBuffer == NULL || pvOutBuffer == NULL) 
		return ERROR_INVALID_DATA;

	// If get top of mem fails it returns 0
	dwTopOfRam = HostGetSysRamTop();
	if (dwTopOfRam == 0)
	{
		DBG_BREAK;
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	// If we've allocated it but not freed it then fail
	if (gsBurnMemInfo.dwPhysAddress == dwTopOfRam)
	{
		DBG_BREAK;
		return ERROR_ALREADY_ASSIGNED;
	}

	// Don't allow burn mem of 0 or more than 1536 Meg, on a 2Gig machine this would only leave 512M...
	gsBurnMemInfo.dwNumPages = MEG_TO_PAGES(*(DWORD *) pvInBuffer);

	if (gsBurnMemInfo.dwNumPages == 0 || gsBurnMemInfo.dwNumPages >= MEG_TO_PAGES(1536)) 
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	// Map to user mode somehow
	if(!HostMapPhysToLin(&gsBurnMemInfo, MmWriteCombined))
	{
		DBG_BREAK;
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	// Store the physical address in the pages, to be consistent with other non-page allocs
	StorePhysicalAddresses(gsBurnMemInfo.pvUserAddr, gsBurnMemInfo.dwNumPages);

	// Save this away, is checked on free to make sure that the struture is valid
	gsBurnMemInfo.dwPhysAddress = dwTopOfRam;

	// Copy the local data into the output structure so that the callee has the info it needs
	pOutVals->hContextID	= 0;
	pOutVals->pv32UserMode	= (void * POINTER_32) gsBurnMemInfo.pvUserAddr;
	pOutVals->dw64PhysAddr  = gsBurnMemInfo.dwPhysAddress;
	pOutVals->dwNumPages	= gsBurnMemInfo.dwNumPages;

	return NO_ERROR;
}

/***********************************************************************************
 Function Name 		: IOCUnBurnMem
 Inputs         	:
 Outputs        	:
 Returns        	: psContext pointer
 Globals Used    	: gsBurnMemInfo
 Description    	: Free the linear user mode memory map to the burnmem region
************************************************************************************/
DWORD IOCUnmapBurnMem(PVOID pvInBuffer, PVOID pvOutBuffer)
{	
	DWORD dwTopOfRam;

	// Validate input pointer, and sBurnMemInfo structure contents
	if (pvInBuffer == NULL)
	{
		return ERROR_INVALID_DATA;
	}

	// If get top of mem fails it returns 0
	dwTopOfRam = HostGetSysRamTop();
	if (dwTopOfRam == 0)
	{
		DBG_BREAK;
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	// Check it tallies with the struct
	if (gsBurnMemInfo.dwPhysAddress != dwTopOfRam)
	{
		DBG_BREAK;
		return ERROR_INVALID_DATA;
	}

	// Unmap the memory
	HostUnMapPhysToLin(&gsBurnMemInfo);

	// Mark structure as freed
	memset(&gsBurnMemInfo, 0, sizeof(gsBurnMemInfo));

	return NO_ERROR;
}

/****************************************************************************
 End of file pcidd.c
****************************************************************************/
