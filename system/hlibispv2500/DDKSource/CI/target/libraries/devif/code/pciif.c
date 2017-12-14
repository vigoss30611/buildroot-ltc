/*!
******************************************************************************
 @file   : pciif.c

 @brief

 @Author Bojan Lakicevic

 @date   18/04/2006

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
         PCI interface for the wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/

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
	#define _T(x) x
	#include <fcntl.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/mman.h>
	#include <errno.h>
	#include <string.h>
	#include <sys/ioctl.h>
	#include <imgpcidd.h>
#endif

#include <tconf.h>
#include <stdio.h>
#include "devif_config.h"

#ifdef WINCE
static void pciwrite(BYTE* pciAddress, BYTE* cpuAddress, unsigned int ui32Size);
static void pciread(BYTE* cpuAddress, BYTE* pciAddress, unsigned int ui32Size);
#endif

#ifdef WIN32

#define getpagesize() ( 4*1024)

#ifndef WINCE
#define PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS (1)
#else
// use accesses through the WinCE driver using WRITE_PCI_32 etc
#endif

/* PCI Information */
static HANDLE		                hPciDriver;

//static IMG_UINT32	                ui32PciHostBuffAddress;
//static IMG_UINT32	                ui32PciHostBuffPhysAddress;
//static IMG_UINT32	                ui32PciHostBuffSize;



#else  //#ifdef WIN32

#define PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS (1)	// This is the only supported option in Linux
#define MAX_SIMPLE_REGIONS 1
#define MAX_PCI_REGIONS		(MAX_IMGPCI_MAPS)
#define LARGE_VIRTUAL_MALLOC_POOLSIZE (256*1024*1024) // virtual address allocation, sufficient for all MallocHostVirt calls.

// a fifo entry points to a single page of memory
struct fifo {
	struct fifo * next;
	void        * data;
};
static IMG_BOOL      fifo_empty(struct fifo * head);
static void          fifo_push(struct fifo * head, struct fifo * entry);
static struct fifo * fifo_pop(struct fifo * head);

static void *        pagepool_region = NULL;			// address space containing pages
static struct fifo   freepages = {.next = &freepages};	// chain of free pages
static void *        pagepool_highwatermark = NULL;		// how much of the region has been used
static void *        get_page_from_pool(void);
static void          put_page_into_pool(void * data);

/* Define the PCI_REGION structure here */
typedef struct _PCIRegion_um
{
    union
	{
		IMG_INT32		dwPhysAddress;
		IMG_INT32		dwIOAddress;
    };
	IMG_PVOID		pvLinAddress;			// User mode linear address
	IMG_PVOID		pvKrnlLinAddress;		// Kernel mode linear address
	IMG_INT32		dwSize;
	IMG_UINT8		bUsage;				// IO or memory or unused
	IMG_UINT8		Pad[3];

} PCI_REGION_UM, *PPCI_REGION_UM;

#endif	//#ifdef WIN32


static PCI_REGION_UM	            asPciRegion[MAX_PCI_REGIONS];
#ifndef WIN32
static IMG_CHAR                     acDevPath[100];   /* device: eg "/dev/imgpci0" */
#endif


typedef enum
{
	PCIIF_TYPE_PCI,
	PCIIF_TYPE_BUS,
	PCIIF_TYPE_UNKOWN,

} PCIIF_eDriverType;

static IMG_BOOL bMemMapped = IMG_FALSE;

#if defined (METAC_2_1)
static inline meta_L2_flush_ioctl(const IMG_UINT32 ui32Addr, const IMG_UINT32 ui32Size);
#endif

#ifdef WIN32


/*!
******************************************************************************

 @Function              PCIIF_InitialiseDriver

******************************************************************************/
static IMG_BOOL PCIIF_InitialiseDriver()
{
	HOSTBUFFER_INFO_OUTBLK		BufferInfoOut;
	OSVERSIONINFO				VersionInformation;
	IMG_BOOL					bIsOSWin9x;
	DWORD						dwResult;

	/* Find out if we are running on Win9x or NT/Win2k/XP */
	VersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if(GetVersionEx(&VersionInformation))
	{
		/* The operation system is Windows NT */
		if(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			bIsOSWin9x=IMG_FALSE;
		}
#ifdef WINCE
		else if(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_CE)
		{
			bIsOSWin9x=IMG_FALSE;
		}
#endif
		else
		{
			bIsOSWin9x=IMG_TRUE;
		}
	}
	else
	{
		printf("ERROR: Could not determine if we are running on Win9x or NT/Win2k/XP\n");
		IMG_ASSERT(IMG_FALSE);
		return IMG_FALSE;
	}

	if (bIsOSWin9x == IMG_TRUE)
	{
		hPciDriver = CreateFile(_T("\\\\.\\imgpcidd.vxd"),
		  					GENERIC_READ | GENERIC_WRITE,
		  					FILE_SHARE_READ | FILE_SHARE_WRITE,
		  					NULL,
		  					OPEN_EXISTING,
		  					FILE_FLAG_DELETE_ON_CLOSE,
		  					NULL);
	}
	else
	{
		hPciDriver = CreateFile(_T(PCIDD_WIN32_DEVICE_NAME),
		  					GENERIC_READ | GENERIC_WRITE,
		  					FILE_SHARE_READ | FILE_SHARE_WRITE,
		  					NULL,
		  					OPEN_EXISTING,
		  					FILE_FLAG_DELETE_ON_CLOSE,
		  					NULL);
	}

	if (hPciDriver == INVALID_HANDLE_VALUE)
	{
		return( IMG_FALSE );
	}

	/*
		Find out about static memory buffer
	*/
	if (DeviceIoControl(	hPciDriver,
							VPCI_HOSTBUFFER_INFO,
							IMG_NULL,
							0,
							&BufferInfoOut,
							sizeof(BufferInfoOut),
							&dwResult, 0)!=0
	)
	{
//		ui32PciHostBuffAddress		= BufferInfoOut.LinAddress;
//		ui32PciHostBuffPhysAddress	= BufferInfoOut.PhysAddress;
//		ui32PciHostBuffSize		    = BufferInfoOut.MemSize;
	}
	return( IMG_TRUE );
}

#ifdef WINCE
void pciwrite(IMG_UINT8* pciAddress, IMG_UINT8* cpuAddress, unsigned int ui32Size)
{
	UINT32 dwResult=0;

	unsigned int bufSize = sizeof(BUFFER_INBLK);
	BUFFER_INBLK*		        pPCIBuffer = (BUFFER_INBLK*) malloc(bufSize);

	pPCIBuffer->Region	= -1;
	pPCIBuffer->Address	= (DWORD) pciAddress;
	pPCIBuffer->Size = ui32Size;
	pPCIBuffer->pBuff = cpuAddress;
	pPCIBuffer->dwErrStatus = 0;

	if (DeviceIoControl(hPciDriver, VPCI_WRITE_BUFFER,
									pPCIBuffer,
									bufSize,
									NULL,
									0,
									&dwResult, 0)==0 ||
									(pPCIBuffer->dwErrStatus != 0)
	)
	{
		IMG_ASSERT(IMG_FALSE);
	}
	//tidy up
	free(pPCIBuffer);
}

void pciread(IMG_UINT8* cpuAddress, IMG_UINT8* pciAddress, unsigned int ui32Size)
{
	UINT32 dwResult=0;
	unsigned int bufSize = sizeof(BUFFER_INBLK);
	BUFFER_INBLK*		        pPCIBuffer = (BUFFER_INBLK*) malloc(bufSize);

	IMG_UINT8* ptr = (IMG_UINT8*) pPCIBuffer;
	ptr+=sizeof(BUFFER_INBLK);

	pPCIBuffer->Region	= -1;
	pPCIBuffer->Address	= (DWORD) pciAddress;
	pPCIBuffer->Size = ui32Size;
	pPCIBuffer->pBuff = cpuAddress;
	pPCIBuffer->dwErrStatus = 0;

	if (DeviceIoControl(hPciDriver, VPCI_READ_BUFFER,
									NULL,
									0,
									pPCIBuffer,
									bufSize,
									&dwResult, 0)==0 ||
									(pPCIBuffer->dwErrStatus != 0)
	)
	{
		IMG_ASSERT(IMG_FALSE);
	}
	// tidy up
	free(pPCIBuffer);
}
#endif // wince

/*!
******************************************************************************

 @Function              PCIIF_FindDevice

******************************************************************************/
static IMG_BOOL PCIIF_FindDevice(DEVIF_sDeviceCB *   psDeviceCB)
{
    FIND_DEVICE_INBLK             FindDeviceIn;     /* Specifics of the device to look for */
    FIND_DEVICE_OUTBLK            FindDeviceOut;  /* structure containing the information on devices that have been found */
    IMG_UINT16                    ui16DeviceCount, i;
    DWORD						  dwResult;


    IMG_UINT16                  ui16Bus = 0;
    IMG_BOOL                    bSuccess = IMG_FALSE;
	DEVIF_sPciInfo *			psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;
	while ((ui16Bus <= 0xFF) && !bSuccess)
	{
		ui16DeviceCount=0;
		while ((ui16DeviceCount < 3) && !bSuccess)
		{
			/* The following have been extracted from pcilib.tcl - Emulator values */
			//FindDeviceIn.wBus           = PCI_BUS_WIDTH;
			FindDeviceIn.wVendorID        = (WORD)psPciDevInfo->ui32VendorId;
			FindDeviceIn.wDeviceID        = (WORD)psPciDevInfo->ui32DeviceId;
			FindDeviceIn.wDeviceClass     = PCI_FIND_DONT_CARE;

			FindDeviceIn.wDeviceCount = ui16DeviceCount;
			FindDeviceIn.wBus = ui16Bus;
			if (  DeviceIoControl(hPciDriver,
								VPCI_FIND_DEVICE,
								&FindDeviceIn,
								sizeof(FindDeviceIn),
								&FindDeviceOut,
								sizeof(FindDeviceOut),
								&dwResult,
								0)         !=0  &&
				(FindDeviceOut.dwBrdsFound > 0) &&
				(FindDeviceOut.wVendorID == psPciDevInfo->ui32VendorId) &&
				(FindDeviceOut.wDeviceID == psPciDevInfo->ui32DeviceId) )
			{
				for (i=0; i < MAX_PCI_REGIONS; i++)
				{
					/* Save Info */
					asPciRegion[i] = FindDeviceOut.RegionInfo[i];

					if(FindDeviceOut.RegionInfo[i].bUsage == RGN_MEMMAPPED)
					{
					}
					if(FindDeviceOut.RegionInfo[i].bUsage == RGN_ROM)
					{
					}
					if(FindDeviceOut.RegionInfo[i].bUsage == RGN_IOMAPPED)
					{
					}
				}
				bSuccess = IMG_TRUE;
				break;
			}
			ui16DeviceCount++;
		}
		ui16Bus++;
	}
	if (!bSuccess)
	{
		printf("ERROR: Failed to find the PCI device.\n");
		printf("Vendor ID 0x%04X DeviceId 0x%04X\n", psPciDevInfo->ui32VendorId, psPciDevInfo->ui32DeviceId);
		IMG_ASSERT(IMG_FALSE);		/* Did not find the PCI device */
	}
    return (bSuccess);
}


/*!
******************************************************************************

 @Function              PCIIF_DeviceRead32

******************************************************************************/
#ifndef PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS
IMG_UINT32 PCIIF_DeviceRead32(
    DEVIF_sDeviceCB * 				psDeviceCB,
    IMG_UINT32						ui32Address
)
{
	DWORD				        dwResult;
	PCI_READ_INBLK		        PCIReadIn;
	PCI_READ_OUTBLK		        PCIReadOut;
    DEVIF_sPciInfo *			psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

	IMG_ASSERT(bPciInitialised);
    IMG_ASSERT(ui32Address < psPciDevInfo->ui32RegSize);

	PCIReadIn.Region	= psPciDevInfo->ui32RegBar;
    PCIReadIn.Address	= ui32Address + psPciDevInfo->ui32RegOffset;

	if (DeviceIoControl(hPciDriver, VPCI_READ32,
									&PCIReadIn,
									sizeof(PCIReadIn),
									&PCIReadOut,
									sizeof(PCIReadOut),
									&dwResult, 0)==0 ||
									(PCIReadOut.dwErrStatus != 0)
	)
	{
		IMG_ASSERT(IMG_FALSE);
	}

	return (PCIReadOut.RetValue32);
}


/*!
******************************************************************************

 @Function              PCIIF_DeviceWrite32

******************************************************************************/
IMG_VOID PCIIF_DeviceWrite32(
    DEVIF_sDeviceCB * 				psDeviceCB,
    IMG_UINT32						ui32Address,
    IMG_UINT32						ui32Value
)
{
	DWORD				        dwResult;
	PCI_WRITE_INBLK		        PCIWriteIn;
	DEVIF_sPciInfo *			psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

	IMG_ASSERT(bPciInitialised);
    IMG_ASSERT(ui32Address < psPciDevInfo->ui32RegSize);

	PCIWriteIn.Region	= psPciDevInfo->ui32RegBar;
    PCIWriteIn.Address	= ui32Address + psPciDevInfo->ui32RegOffset;
    PCIWriteIn.Value32	= ui32Value;

	if (DeviceIoControl(hPciDriver, VPCI_WRITE32,
									&PCIWriteIn,
									sizeof(PCIWriteIn),
									NULL,
									0,
									&dwResult, 0)==0
	)
	{
		IMG_ASSERT(IMG_FALSE);
	}
}
#endif	// #ifndef PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS
#else	//#ifdef WIN32



/*!
******************************************************************************

 @Function              pciif_ReadStringFromFile

******************************************************************************/
static IMG_INT32 pciif_ReadStringFromFile(IMG_CHAR* pcFileName, IMG_CHAR *pcBuffer, IMG_UINT32 ui32Length)
{
	IMG_INT32			hFileHandle;
	IMG_UINT32			i32BytesRead = 0;

	hFileHandle = open(pcFileName, O_RDONLY);

	if (hFileHandle <= 0)
	{
		return -1;
	}

	i32BytesRead = read(hFileHandle, pcBuffer, ui32Length);

	close(hFileHandle);

	return i32BytesRead;
}

/*!
******************************************************************************

 @Function              PCIIF_InitialiseDriver - a stub

******************************************************************************/
#define PCIIF_InitialiseDriver() (IMG_TRUE)

/*!
******************************************************************************

 @Function              PCIIF_FindDevice

******************************************************************************/
static IMG_BOOL PCIIF_FindDevice(DEVIF_sDeviceCB *   psDeviceCB)
{
	IMG_UINT32						ui32UIODeviceNumber = 0;
	IMG_CHAR						acPath[100];
	IMG_CHAR						acString[100];
	IMG_INT32						iRes;
	IMG_INT32						hDeviceFileHandle;
	PCIIF_eDriverType				eDriverType = PCIIF_TYPE_UNKOWN;
	IMG_BOOL						bNameTest = IMG_FALSE;
	IMG_UINT32						ui32VendorID, ui32DeviceID;
	DEVIF_sPciInfo *				psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;
	while (1)
	{
		// Check to see if we have either a pci or a simple bus driver installed
		sprintf(&acPath[0], "/sys/class/imgpci/imgpci%d/name", ui32UIODeviceNumber);
		iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));
		if (iRes<0)
		{
			sprintf(&acPath[0], "/sys/class/img/img%d/name", ui32UIODeviceNumber);
			iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));
			if (iRes<0)
			{
				printf("ERROR: Cannot open img driver file: %s\n", acPath);
				/* Exit as soon as we do not find a /sys/class/imgpci/imgpci#/name or /sys/class/img/img#/name */
				return IMG_FALSE;
			}
			else
			{
				eDriverType = PCIIF_TYPE_BUS;
			}
		}
		else
		{
			eDriverType = PCIIF_TYPE_PCI;
		}

		// Check the string read from the name file to see if it is a valid driver
		switch (eDriverType)
		{
		case PCIIF_TYPE_PCI:
			bNameTest = ((iRes >= 6) && (strncmp(acString, "imgpci", 6) == 0));
			break;
		case PCIIF_TYPE_BUS:
			bNameTest = ((iRes >= 6) && (strncmp(acString, "img", 3) == 0));
			break;
		default:
			return IMG_FALSE;
		}

		if (bNameTest)
		{
			if (eDriverType == PCIIF_TYPE_PCI)
			{
				/* we have found a imgpci device. Next match it up against the vendor and device ID numbers supplied */

				/* Vendor ID */
				sprintf(&acPath[0], "/sys/class/imgpci/imgpci%d/device/vendor", ui32UIODeviceNumber);
				iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));

				if (iRes<0)
				{
					printf("ERROR: Problem looking up vendor ID at %s", &acPath[0]);
					return IMG_FALSE;
				}
				ui32VendorID = strtoul(acString, IMG_NULL, 0);


				/* Device ID */
				sprintf(&acPath[0], "/sys/class/imgpci/imgpci%d/device/device", ui32UIODeviceNumber);
				iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));

				if (iRes<0)
				{
					printf("ERROR: Problem looking up device ID at %s", &acPath[0]);
					return IMG_FALSE;
				}
				ui32DeviceID = strtoul(acString, IMG_NULL, 0);
			}


			switch(eDriverType)
			{
			case PCIIF_TYPE_PCI:
				/* Does the device found match the provided vendor and device IDs? */
				if ((ui32VendorID == psPciDevInfo->ui32VendorId) &&
					(ui32DeviceID == psPciDevInfo->ui32DeviceId))
				{
					IMG_UINT32	ui32PciRegionNumber;


					/* Open the device file for this device */
					sprintf(&acDevPath[0], "/dev/imgpci%d", ui32UIODeviceNumber);
					hDeviceFileHandle = open(acDevPath, O_RDWR);

					if (hDeviceFileHandle < 0)
					{
						printf("ERROR: Cannot open device %s ", &acDevPath[0]);

						if (errno == EACCES)
						{
							printf("(Permission denied)\n");
						}
						else
						{
							printf("(errno %d)\n", errno);
						}
						return IMG_FALSE;
					}

					/* Map all the available regions */
					for (ui32PciRegionNumber=0; ui32PciRegionNumber < MAX_PCI_REGIONS; ui32PciRegionNumber++)
					{
						IMG_UINT32		ui32RegionSize;
						IMG_PVOID		pvLinearAddress;

						sprintf(&acPath[0], "/sys/class/imgpci/imgpci%d/maps/map%d/size", ui32UIODeviceNumber, ui32PciRegionNumber);
						iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));

						if (iRes < 1)
						{
							printf("WARNING: Cannot read BAR%d size for the device\n", ui32PciRegionNumber);
							asPciRegion[ui32PciRegionNumber].dwSize = 0;
							asPciRegion[ui32PciRegionNumber].pvLinAddress = IMG_NULL;
							continue;
						}
						ui32RegionSize = strtoul(acString, IMG_NULL, 0);

						pvLinearAddress = mmap(IMG_NULL, ui32RegionSize, (PROT_READ | PROT_WRITE) , MAP_SHARED, hDeviceFileHandle, (ui32PciRegionNumber * getpagesize()));

						if (pvLinearAddress == MAP_FAILED)
						{
							printf("WARNING: Memory mapping of BAR%d to user space not successful (errno %d)\n", ui32PciRegionNumber, errno);
							ui32RegionSize = 0;
							pvLinearAddress = IMG_NULL;
						}

						printf("0x%08X bytes from BAR%d mapped to user address 0x%p\n", ui32RegionSize, ui32PciRegionNumber, pvLinearAddress);

						/* Save Info */
						asPciRegion[ui32PciRegionNumber].dwSize = ui32RegionSize;
						asPciRegion[ui32PciRegionNumber].pvLinAddress = pvLinearAddress;
					}

					/* Close the device file handle */
					close(hDeviceFileHandle);
					return IMG_TRUE;
				}
				break;
			case PCIIF_TYPE_BUS:
			{
				IMG_UINT32	ui32RegionNumber;

				/* Open the device file for this device */
				sprintf(&acDevPath[0], "/dev/img%d", ui32UIODeviceNumber);
				hDeviceFileHandle = open(acDevPath, O_RDWR);

				if (hDeviceFileHandle < 0)
				{
					printf("ERROR: Cannot open device %s ", &acDevPath[0]);

					if (errno == EACCES)
					{
						printf("(Permission denied)\n");
					}
					else
					{
						printf("(errno %d)\n", errno);
					}
					return IMG_FALSE;
				}

				/* Map all the available regions */
				for (ui32RegionNumber=0; ui32RegionNumber < MAX_SIMPLE_REGIONS; ui32RegionNumber++)
				{
					IMG_UINT32		ui32RegionSize;
					IMG_PVOID		pvLinearAddress;

					sprintf(&acPath[0], "/sys/class/img/img%d/maps/map%d/size", ui32UIODeviceNumber, ui32RegionNumber);
					iRes = pciif_ReadStringFromFile(acPath, acString, sizeof(acString));

					if (iRes < 1)
					{
						printf("WARNING: Cannot read BAR%d size for the device\n", ui32RegionNumber);
						asPciRegion[ui32RegionNumber].dwSize = 0;
						asPciRegion[ui32RegionNumber].pvLinAddress = IMG_NULL;
						continue;
					}
					ui32RegionSize = strtoul(acString, IMG_NULL, 0);

					pvLinearAddress = mmap(IMG_NULL, ui32RegionSize, (PROT_READ | PROT_WRITE) , MAP_SHARED, hDeviceFileHandle, (ui32RegionNumber * getpagesize()));

					if (pvLinearAddress == MAP_FAILED)
					{
						printf("WARNING: Memory mapping of BAR%d to user space not successful (errno %d)\n", ui32RegionNumber, errno);
						ui32RegionSize = 0;
						pvLinearAddress = IMG_NULL;
					}

					printf("0x%08X bytes from BAR%d mapped to user address 0x%p\n", ui32RegionSize, ui32RegionNumber, pvLinearAddress);

					/* Save Info */
					asPciRegion[ui32RegionNumber].dwSize = ui32RegionSize;
					asPciRegion[ui32RegionNumber].pvLinAddress = pvLinearAddress;
				}

				/* Close the device file handle */
				close(hDeviceFileHandle);
				return IMG_TRUE;
			}
				break;
			default:
				IMG_ASSERT(IMG_FALSE);
			}
		}
		ui32UIODeviceNumber++;
	}


	return IMG_TRUE;
}

#endif	//#ifdef WIN32


/*!
******************************************************************************

 @Function				PCIIF_Initialise

******************************************************************************/
IMG_RESULT PCIIF_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
    DEVIF_sPciInfo *				 psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

    /* If the PCI interface has not been initialised..*/
    if (!psDeviceCB->bInitialised)
    {
	    printf("Initialising PCI driver...\n");

	    if ( ! PCIIF_InitialiseDriver() )
	    {
		    printf("ERROR: Driver initialisation failed!\n");
		    IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_FATAL;
	    }

	    printf("Finding PCI card...\n");
	    if ( ! PCIIF_FindDevice(psDeviceCB) )
	    {
		    printf("ERROR: Did not find PCI card!\n");
		    IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_FATAL;
	    }

		psDeviceCB->bInitialised = IMG_TRUE;
#ifdef FILL_MEMORY
		// Fill 256M as the new FPGAs and emulator have 256.
		DEVIF_FillAbs(0x10000, 0x00, 0xfff0000);
#endif
    }

    IMG_ASSERT(psPciDevInfo->ui32RegBar < MAX_PCI_REGIONS);
	if ((psPciDevInfo->ui32RegOffset + psPciDevInfo->ui32RegSize) > asPciRegion[psPciDevInfo->ui32RegBar].dwSize)
	{		 
		fprintf(stderr, "Given PCI Device Offset 0x%08X + PCI Device Size 0x%08X > PCI Bar Size 0x%08X (reported from PCI)\n"
						,psPciDevInfo->ui32RegOffset, psPciDevInfo->ui32RegSize, asPciRegion[psPciDevInfo->ui32RegBar].dwSize);
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_FATAL;
	}
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              PCIIF_DeviceMemToHostVirtual

 Translate the device memory address into the virtual address as seen by the host

******************************************************************************/
IMG_VOID*	PCIIF_DeviceMemToHostVirtual(
	DEVIF_sDeviceCB *  				 psDeviceCB,
    IMG_UINT64                      ui64DeviceAddress)
{
    IMG_UINT32	                ui32MemorySpaceOffset;
	IMG_VOID *                  pvVirtAddress;
	DEVIF_sPciInfo *			 psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;
	
	if(bMemMapped)
	{
#ifdef WIN32
		pvVirtAddress = DEVIF_FindPhysToVirtEntry(ui64DeviceAddress);
		IMG_ASSERT(pvVirtAddress);
#else
			// it is not actually device memory, but memory mapped kernel memory.
			// its virtual address is stored in a lookup table
			// We are dependant on allocations up to 1 page in size.
			IMG_UINT64 pagemask = getpagesize()-1;
			IMG_UINT64 ui64DevicePageAddr = ui64DeviceAddress & ~pagemask;
			pvVirtAddress = DEVIF_FindPhysToVirtEntry(ui64DevicePageAddr);
			pvVirtAddress += ui64DeviceAddress & pagemask;
			IMG_ASSERT(pvVirtAddress);
#endif
	}
   	else
	{
		if (psDeviceCB != IMG_NULL)
		{
			IMG_ASSERT(psPciDevInfo->ui32MemBar < MAX_PCI_REGIONS);
			IMG_ASSERT((psPciDevInfo->ui32MemOffset + psPciDevInfo->ui32MemSize) <= asPciRegion[psPciDevInfo->ui32MemBar].dwSize);
			
			if (psPciDevInfo->bPCIAddDevOffset)
				ui32MemorySpaceOffset = IMG_UINT64_TO_UINT32(ui64DeviceAddress);
			else
			    ui32MemorySpaceOffset = IMG_UINT64_TO_UINT32(ui64DeviceAddress - psPciDevInfo->ui64BaseAddr);

		    IMG_ASSERT(ui32MemorySpaceOffset < psPciDevInfo->ui32MemSize);

		    pvVirtAddress = (IMG_PVOID)((IMG_UINTPTR)asPciRegion[psPciDevInfo->ui32MemBar].pvLinAddress + psPciDevInfo->ui32MemOffset + ui32MemorySpaceOffset);
		}
		else
		{
		//  ui32VirtAddress = ui32DeviceAddress + psPciMemBaseInfo->ui32Offset + (IMG_UINT32) asPciRegion[psPciMemBaseInfo->ui32Bar].LinAddress;
		    pvVirtAddress = (IMG_PVOID)((IMG_UINTPTR)asPciRegion[2].pvLinAddress + IMG_UINT64_TO_UINT32(ui64DeviceAddress) + 0);
		}
	}
	return (pvVirtAddress);
}

/*!
******************************************************************************

 @Function				PCIIF_ReadRegister

******************************************************************************/
IMG_UINT32 PCIIF_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
#ifdef PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS
	volatile IMG_UINT32 *		    pui32VirtAddress;
    DEVIF_sPciInfo *			 	psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

    IMG_ASSERT(psDeviceCB->bInitialised);
	IMG_ASSERT(ui64RegAddress < psPciDevInfo->ui32RegSize);

	pui32VirtAddress = (IMG_UINT32 *)((IMG_UINTPTR)asPciRegion[psPciDevInfo->ui32RegBar].pvLinAddress + IMG_UINT64_TO_UINT32(ui64RegAddress) + psPciDevInfo->ui32RegOffset);
    return *pui32VirtAddress;
#else
    IMG_ASSERT(psDeviceCB->bInitialised);
    return PCIIF_DeviceRead32(psDeviceCB, ui32RegAddress);
#endif
}

/*!
******************************************************************************

 @Function				PCIIF_WriteRegister

******************************************************************************/
IMG_VOID PCIIF_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
#ifdef PCI_USE_MEMORY_MAPPED_REGISTER_ACCESS
	volatile IMG_UINT32 *			pui32VirtAddress;
    DEVIF_sPciInfo *			 	psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

    IMG_ASSERT(psDeviceCB->bInitialised);
    IMG_ASSERT(ui64RegAddress < psPciDevInfo->ui32RegSize);

	pui32VirtAddress = (IMG_UINT32 *)((IMG_UINTPTR) asPciRegion[psPciDevInfo->ui32RegBar].pvLinAddress + IMG_UINT64_TO_UINT32(ui64RegAddress) + psPciDevInfo->ui32RegOffset);
   *pui32VirtAddress = ui32Value;
#else
    IMG_ASSERT(psDeviceCB->bInitialised);
    PCIIF_DeviceWrite32(psDeviceCB, ui32RegAddress, ui32Value);
#endif
}

/*!
******************************************************************************

 @Function				PCIIF_ReadMemory

******************************************************************************/
IMG_UINT32 PCIIF_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_UINT32 ret=0;
    volatile IMG_UINT32 *           pui32HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui32HostVirt = (IMG_UINT32 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), sizeof(IMG_UINT32));
#endif

#ifdef WINCE
	pciread(&ret, pui32HostVirt, sizeof(ret));
#else
	ret =*pui32HostVirt;
#endif

	return ret;
}

/*!
******************************************************************************

 @Function				PCIIF_ReadMemory64

******************************************************************************/
IMG_UINT64 PCIIF_ReadMemory64 (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	IMG_UINT64 ret=0;
    volatile IMG_UINT64 *           pui64HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui64HostVirt = (IMG_UINT64 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), sizeof(IMG_UINT64));
#endif

#ifdef WINCE
	pciread(&ret, pui64HostVirt, sizeof(ret));
#else
	ret =*pui64HostVirt;
#endif

	return ret;
}


/*!
******************************************************************************

 @Function				PCIIF_WriteMemory

******************************************************************************/
IMG_VOID PCIIF_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
    volatile IMG_UINT32 *           pui32HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui32HostVirt = (IMG_UINT32 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));

#ifdef WINCE
	pciwrite(pui32HostVirt, &ui32Value, sizeof(ui32Value));
#else
    *pui32HostVirt = ui32Value;
#endif

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), sizeof(IMG_UINT32));
#endif /* defined (METAC_2_1) */
}

/*!
******************************************************************************

 @Function				PCIIF_WriteMemory64

******************************************************************************/
IMG_VOID PCIIF_WriteMemory64 (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT64          ui64Value
)
{
    volatile IMG_UINT64 *           pui64HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui64HostVirt = (IMG_UINT64 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));

#ifdef WINCE
	pciwrite(pui64HostVirt, &ui64Value, sizeof(ui64Value));
#else
    *pui64HostVirt = ui64Value;
#endif

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), sizeof(IMG_UINT64));
#endif /* defined (METAC_2_1) */
}




/*!
******************************************************************************

 @Function				PCIIF_CopyDeviceToHost

******************************************************************************/
IMG_VOID PCIIF_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
    IMG_UINT8  *                    pui8HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui8HostVirt = (IMG_UINT8 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), ui32Size);
#endif /* defined (METAC_2_1) */

#ifdef WINCE
	pciread(pvHostAddress, pui8HostVirt, ui32Size);
#else
	memcpy(pvHostAddress, pui8HostVirt, ui32Size);
#endif
}


/*!
******************************************************************************

 @Function				PCIIF_CopyHostToDevice

******************************************************************************/
IMG_VOID PCIIF_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
    IMG_UINT8  *                    pui8HostVirt;

    IMG_ASSERT(psDeviceCB->bInitialised);
	pui8HostVirt = (IMG_UINT8 *)PCIIF_DeviceMemToHostVirtual(psDeviceCB, IMG_UINT64_TO_UINT32(ui64DevAddress));


#ifdef WINCE
	pciwrite(pui8HostVirt, pvHostAddress, ui32Size);
#else
	memcpy(pui8HostVirt, pvHostAddress, ui32Size);
#endif

	/* Now flush the cache */
#if defined (METAC_2_1)
	meta_L2_flush_ioctl(IMG_UINT64_TO_UINT32(ui64DevAddress), ui32Size);
#endif /* defined (METAC_2_1) */
}


/*!
******************************************************************************

 @Function				PCIIF_Deinitialise

******************************************************************************/
IMG_VOID PCIIF_Deinitailise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
    if (psDeviceCB->pvDevIfInfo != IMG_NULL)
    {
        IMG_FREE(psDeviceCB->pvDevIfInfo);
        psDeviceCB->pvDevIfInfo = IMG_NULL;
    }
}

/*!
******************************************************************************

 @Function				PCIIF_GetDeviceMapping

******************************************************************************/
IMG_VOID PCIIF_GetDeviceMapping (
	DEVIF_sDeviceCB	*			psDeviceCB,
	DEVIF_sDevMapping *			psDevMapping
)
{
    DEVIF_sPciInfo *			 	psPciDevInfo = &psDeviceCB->sDevIfSetup.u.sPciInfo;

	IMG_MEMSET(psDevMapping, 0, sizeof(*psDevMapping));
	psDevMapping->pui32Registers = (IMG_UINT32 *)((IMG_UINTPTR) asPciRegion[psPciDevInfo->ui32RegBar].pvLinAddress + psPciDevInfo->ui32RegOffset);
	psDevMapping->ui32RegSize	 = psPciDevInfo->ui32RegSize;
	psDevMapping->pui32Memory	 = (IMG_PVOID)((IMG_UINTPTR)asPciRegion[psPciDevInfo->ui32MemBar].pvLinAddress + psPciDevInfo->ui32MemOffset);
	psDevMapping->ui32MemSize	 = psPciDevInfo->ui32MemSize;
	psDevMapping->ui32DevMemoryBase =  IMG_UINT64_TO_UINT32(psPciDevInfo->ui64BaseAddr);
}


#ifndef WIN32
/*******************************************************************************

 @Function				a simple FIFO implementation

******************************************************************************/
static IMG_BOOL fifo_empty(struct fifo * head)
{
	return head->next == head;
}
/* add an entry to a FIFO */
static void fifo_push(struct fifo * head, struct fifo * entry)
{
	entry->next = head->next;
	head->next  = entry;
}
/* pop an entry from a FIFO */
static struct fifo * fifo_pop(struct fifo * head)
{
	struct fifo * n = NULL;
	if(fifo_empty(head))
		return NULL;
	n = head->next;
	head->next = n->next;
	n->next = n;			// unneccessary, but tidier
	return n;
}
/* get next available page from the pool.
 * Either pop from the freepages fifo, or move the high water mark along
 */
static void * get_page_from_pool(void)
{
	void * data = NULL;
	struct fifo * page = fifo_pop(&freepages);

	if(page == NULL) {
		IMG_ASSERT(pagepool_highwatermark - pagepool_region < LARGE_VIRTUAL_MALLOC_POOLSIZE);
		data = pagepool_highwatermark;
		pagepool_highwatermark += getpagesize();
	}
	else {
		data = page->data;
		IMG_FREE(page);
	}
	return data;
}
/* return a previously allocated page to the free pages fifo */
static void put_page_into_pool(void * data)
{
	struct fifo * page = IMG_MALLOC(sizeof(struct fifo));
	IMG_ASSERT(page);
	page->data = data;
	fifo_push(&freepages, page);
}

/*!
******************************************************************************

 @Function				PCIIF_MallocHostVirt

 @Description: this function asks the kernel to allocate memory, and then
               maps the memory into user space.

			   Potentially, there could be many thousands of mallocs. Rather
			   than individually map them into the memory space,
			   allocate a pool of virtual address space,
			   and take lumps out of it as required using IMGPCI_VIRTUAL_MAP.

			   This function returns the physical address

******************************************************************************/
static IMG_UINT64 PCIIF_MallocHostVirt(
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64          ui64Alignment,
	IMG_UINT64          ui64Size)
{
	IMG_RESULT res = IMG_SUCCESS;
	IMG_UINT64 ui64PhysAddress = 0;

	int fd = -1;
	int ret;

	bMemMapped = IMG_TRUE;

	/* at the moment, we only handle single pages at a time */
	IMG_ASSERT(ui64Size <= getpagesize());

	IMG_ASSERT(ui64Alignment <= getpagesize());
	fd = open(acDevPath, O_RDWR);
	if(fd < 0)
	{
		perror("open");
		IMG_ASSERT(fd >= 0);
		res = IMG_ERROR_INVALID_PARAMETERS;
	}
	if(res == IMG_SUCCESS && !pagepool_region)
	{
		/* memory map a nice big region. This does not actually reserve memory (see imgpcidd.c).
		   Then, successive calls to this function ping real memory into the region */
		pagepool_region = mmap(IMG_NULL,
							   LARGE_VIRTUAL_MALLOC_POOLSIZE,
							   PROT_READ | PROT_WRITE,
							   MAP_SHARED,
							   fd,
							   IMGPCI_VIRTUAL_MAP * getpagesize());
		if(pagepool_region == IMG_NULL)
		{
			IMG_ASSERT(!"unable to memory map");
			res = IMG_ERROR_OUT_OF_MEMORY;
		}
		pagepool_highwatermark = pagepool_region;
	}

	if(res == IMG_SUCCESS)
	{
		struct imgpci_get_virt2phys virt2phys;
		virt2phys.virt = get_page_from_pool();
		if(virt2phys.virt == NULL)
		{
			IMG_ASSERT(virt2phys.virt);
			res = IMG_ERROR_OUT_OF_MEMORY;
		}
		virt2phys.dma32 = IMG_TRUE;

		/* ask the kernel for the physical address of the page */
		ret = ioctl(fd, IMGPCI_IOCTL_GET_VIRT2PHYS, &virt2phys);
		if(ret)
		{
			IMG_ASSERT(ret == 0);
			res = IMG_ERROR_GENERIC_FAILURE;
		}
		else
		{
			ui64PhysAddress = virt2phys.phys;
			if(!DEVIF_AddPhysToVirtEntry(virt2phys.phys, virt2phys.virt))
			{
				IMG_ASSERT(!"could not track physical to virtual address");
				res = IMG_ERROR_GENERIC_FAILURE;
			}
		}
	}

	if(fd >= 0)
		close(fd);
	return ui64PhysAddress;
}
#endif // WIN32

/*!
******************************************************************************

 @Function				PCIIF_FreeMemory

******************************************************************************/
#ifndef WIN32
IMG_VOID PCIIF_FreeMemory(
    DEVIF_sDeviceCB *		psDeviceCB,
	IMG_UINT64          	ui64Address
)
{

	IMG_VOID * virt = DEVIF_FindPhysToVirtEntry(ui64Address);

	// Free virtual address.
	put_page_into_pool(virt);

}
#endif



#ifdef WIN32

static IMG_UINT64 PCIIF_MallocHostVirt(
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64          ui64Alignment,
	IMG_UINT64          ui64Size)
{
	IMG_UINT32 ui32Result;
	IMG_UINT64 uiPhys = 0;

	CONTIG_NPP_INFO sLastNonPagedPoolInfo;
	DWORD dwPages = (DWORD)((ui64Size + (getpagesize()-1) )/ getpagesize());

	(void)ui64Alignment;
	(void*)psDeviceCB;

	/* dirty  but this seems to be how its been done before*/
	bMemMapped = IMG_TRUE;

	memset( &sLastNonPagedPoolInfo , 0, sizeof( sLastNonPagedPoolInfo ) );
	if (DeviceIoControl(	hPciDriver,
							VPCI_NON_PAGED_ALLOC ,//(dwPages==1) ? VPCI_NON_PAGED_ALLOC : VPCI_CONTIG_NON_PAGED_ALLOC,
							&dwPages,
							sizeof(DWORD),
							&sLastNonPagedPoolInfo,
							sizeof(sLastNonPagedPoolInfo),
							(DWORD*)&ui32Result, NULL )!=0	)
	{
		uiPhys = sLastNonPagedPoolInfo.dw64PhysAddr;
		if(!DEVIF_AddPhysToVirtEntry(	sLastNonPagedPoolInfo.dw64PhysAddr,
										sLastNonPagedPoolInfo.dwNumPages * getpagesize(),
										sLastNonPagedPoolInfo.pv32UserMode))
		{
			IMG_ASSERT(!"Could not track physical to virtual address.");
		}
	}
	else
	{
		/* Failed to allocate memory - not an error and an apropriate return code should be returned
		 however the interface is deficient and not allow this.   Did I mention, THIS IS NOT AN ERROR,
		 rather a real work use case */
	}

	return uiPhys;
}


IMG_VOID PCIIF_FreeMemory(
    DEVIF_sDeviceCB *		psDeviceCB,
	IMG_UINT64          	ui64Address
)
{
	IMG_UINT32 ui32Result;
	DWORD dwUnused = 0;
	(void*)psDeviceCB;

	if (DeviceIoControl(	hPciDriver,
							VPCI_NON_PAGED_FREE, /* The PCI Driver handles contiguous and non-contiguous memory in the same function */
							&dwUnused,
							sizeof(DWORD),
							NULL,
							0,
							(DWORD*)&ui32Result, NULL ) !=0	)
	{
		DEVIF_RemovePhysToVirtEntry(ui64Address);
	}
}

#endif



/*!
******************************************************************************

 @Function				PCIIF_DevIfConfigureDevice

******************************************************************************/
IMG_VOID PCIIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	psDeviceCB->bInitialised        = IMG_FALSE;
	psDeviceCB->pvDevIfInfo         = IMG_NULL;
	
    /* Use "normal" PCI driver...*/
    psDeviceCB->pfnInitailise       = PCIIF_Initialise;
    psDeviceCB->pfnReadRegister     = PCIIF_ReadRegister;
    psDeviceCB->pfnWriteRegister    = PCIIF_WriteRegister;
    psDeviceCB->pfnReadMemory       = PCIIF_ReadMemory;
    psDeviceCB->pfnReadMemory64     = PCIIF_ReadMemory64;
    psDeviceCB->pfnWriteMemory      = PCIIF_WriteMemory;
    psDeviceCB->pfnWriteMemory64    = PCIIF_WriteMemory64;
    psDeviceCB->pfnCopyDeviceToHost = PCIIF_CopyDeviceToHost;
    psDeviceCB->pfnCopyHostToDevice = PCIIF_CopyHostToDevice;
    psDeviceCB->pfnDeinitailise     = PCIIF_Deinitailise;
    psDeviceCB->pfnGetDeviceMapping = PCIIF_GetDeviceMapping;

    psDeviceCB->pfnMallocHostVirt   = PCIIF_MallocHostVirt;
    psDeviceCB->pfnFreeMemory		= PCIIF_FreeMemory;

}


/*!
******************************************************************************

 @Function				PCIIF_CopyAbsToHost

******************************************************************************/
IMG_VOID PCIIF_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32
	IMG_UINT8  *        pui8HostVirt;

	pui8HostVirt = (IMG_UINT8 *)PCIIF_DeviceMemToHostVirtual(IMG_NULL, IMG_UINT64_TO_UINT32(ui64AbsAddress));
#ifdef WINCE
	pciread(pvHostAddress, pui8HostVirt, ui32Size);
#else
	memcpy(pvHostAddress, pui8HostVirt, ui32Size);
#endif
#else
	IMG_ASSERT(IMG_FALSE);
#endif
}


/*!
******************************************************************************

 @Function              PCIIF_FillAbs

******************************************************************************/
IMG_VOID PCIIF_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32
	IMG_UINT8 *pui8HostVirt;

	pui8HostVirt = (IMG_UINT8 *)PCIIF_DeviceMemToHostVirtual(IMG_NULL, IMG_UINT64_TO_UINT32(ui64AbsAddress));
#ifdef WINCE
	{
		IMG_UINT8* buf = (IMG_UINT8*) malloc(ui32Size);
		memset(buf, ui8Value, ui32Size);
		pciwrite(pui8HostVirt, buf, ui32Size);
		free(buf);
	}
#else
	memset(pui8HostVirt, ui8Value, ui32Size);
#endif
#else
	IMG_ASSERT(IMG_FALSE);
#endif
}

#if defined (METAC_2_1)
/*!
******************************************************************************

 @Function              meta_L2_flush_ioctl

******************************************************************************/
static IMG_INLINE meta_L2_flush_ioctl(const IMG_UINT32 ui32Addr, const IMG_UINT32 ui32Size)
{
	struct imgpci_cache_flush sCacheFlush;
	int 					  fd = -1;

	fd = open(acDevPath, O_RDWR);
	if(fd < 0)
	{
		perror("open");
		IMG_ASSERT(fd >= 0); // Failed to open the imgpci device
	}
	else
	{
		sCacheFlush.ui32PhysStartAddr = ui32Addr;
		sCacheFlush.ui32SizeInBytes   = ui32Size;

		if(ioctl(fd, IMGPCI_IOCTL_META_L2_CACHE_FLUSH, &sCacheFlush))
		{
			IMG_ASSERT(0); // Failed L2 Cache flush IOCTL
		}
		close(fd);
	}
}
#endif

