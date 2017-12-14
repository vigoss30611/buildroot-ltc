/*****************************************************************************
 Name		: VPCIDD.H
 Title		: Highlander PCI Debug Vxd - Header File

 Author		: John Metcalfe
 Created	: 15/9/97

 Copyright	: 1997 by VideoLogic Limited. All rights reserved.
		  No part of this software, either material or conceptual
		  may be copied or distributed, transmitted, transcribed,
		  stored in a retrieval system or translated into any
		  human or computer language in any form by any means,
		  electronic, mechanical, manual or other-wise, or
		  disclosed to third parties without the express written
		  permission of VideoLogic Limited, Unit 8, HomePark
		  Industrial Estate, King's Langley, Hertfordshire,
		  WD4 8LZ, U.K.

 Description : VXD PCI Debug Vxd - header file

*****************************************************************************/

#ifndef	_VPCIDD_
#define _VPCIDD_

#include "pciddif.h"
#include "hostfunc.h"


/*****************************************************************************
 Constants
*****************************************************************************/

/* Make sure we can hold enough for all use cases */
#define MAX_DISPATCHS 8
#define MAX_THREAD_CONTEXTS	16 /* Make sure we can hold enough for all use cases */	

typedef struct _THREAD_CONTEXT_
{
	HANDLE				hThreadId;
	DWORD				dwNumAllocations;

	PCONTEXT_BUFFER		psContextBuffer; /* linked list of allocation contexts */
	PMEMORY_CONTEXT		psNonContigContext; /* pointer to a non contiguous allocation context that has free space */
} THREAD_CONTEXT, *PTHREAD_CONTEXT;

typedef struct _PROCESS_CONTEXT_
{
	HANDLE				hProcessId;
	PEPROCESS			pProcess;

	THREAD_CONTEXT		asThreadContext[MAX_THREAD_CONTEXTS];
} PROCESS_CONTEXT, *PPROCESS_CONTEXT;

typedef struct _DISPATCH_
{
	HANDLE				hProcessId;
	PEPROCESS			pProcess;

	MEMORY_CONTEXT		PCIRegion[MAX_PCI_REGIONS]; /* Struct to hold mapped address regions */
	IMG_UINT32			ui32CurrentPciRegion;

	IMG_UINT32			ui32FileOpenCount;

	IMG_BOOL			bFoundDevice;
	IMG_BYTE			bBusNum;
	IMG_BYTE			bDeviceNum;
} DISPATCH, *PDISPATCH;

extern DISPATCH		gasDispatch[MAX_DISPATCHS];

/*****************************************************************************
    PCI Config register addresses.
*****************************************************************************/
#define PCI_MAX_BUSES 255
#define PCI_MAX_DEVS  32
#define PCI_MAX_FUNCS 8

#define PCI_CMD_ENABLE_MEM_SPACE   2
#define PCI_HEADER_MULTIFUNC       0x80

#define PCI_CONFIG_VENDOR_ID       0x0000
#define PCI_CONFIG_DEVICE_ID       0x0002
#define PCI_CONFIG_COMMAND         0x0004
#define PCI_CONFIG_STATUS          0x0006
#define PCI_CONFIG_HW_REV          0x0008
#define PCI_CONFIG_FB_BASE         0x0010
#define PCI_CONFIG_HEADER_TYPE     0x000E
#define PCI_CONFIG_REG_BASE        0x0014
#define PCI_CONFIG_VGAFB_BASE      0x0018
#define PCI_CONFIG_VGAREG_BASE     0x001C
#define PCI_CONFIG_SUBVENDOR_ID    0x002C
#define PCI_CONFIG_SUBSYS_ID       0x002E
#define PCI_CONFIG_ROM_BASE        0x0030
#define PCI_CONFIG_CAPS_POINTER    0x0034

#define PCI_CONFIG_CODEBASE        0x00F0
#define PCI_CONFIG_DOS_BUFFBASE    0x00FF

#define PCI_CONFIG_CLASS_BRIDGE_DEV       0x6
#define PCI_AGP_CAP_REG_ID         0x0002

#define PCI_CONFIG_T16	0x50

/*****************************************************************************
 IOCTL Function prototypes
*****************************************************************************/
DWORD	IOCFindDevice			(PVOID, PVOID);
DWORD	IOCReadPCI8   			(PVOID, PVOID);
DWORD	IOCReadPCI16  			(PVOID, PVOID);
DWORD	IOCReadPCI32  			(PVOID, PVOID);
DWORD	IOCWritePCI8  			(PVOID, PVOID);
DWORD	IOCWritePCI16 			(PVOID, PVOID);
DWORD	IOCWritePCI32 			(PVOID, PVOID);
DWORD	IOCRead8 				(PVOID, PVOID);
DWORD	IOCRead16 				(PVOID, PVOID);
DWORD	IOCRead32 				(PVOID, PVOID);
DWORD	IOCWrite8 				(PVOID, PVOID);
DWORD	IOCWrite16				(PVOID, PVOID);
DWORD	IOCWrite32				(PVOID, PVOID);
DWORD	IOCWriteRead32			(PVOID, PVOID);
DWORD	IOCReadBuffer			(PVOID, PVOID);
DWORD	IOCWriteBuffer 			(PVOID, PVOID);
DWORD	IOCFillMem32 			(PVOID, PVOID);
DWORD	IOCReadMulti32			(PVOID, PVOID);
DWORD	IOCBufferInfo 			(PVOID, PVOID);
DWORD	IOCMoveDevice			(PVOID, PVOID);
DWORD	IOCAllocMemory			(PVOID, PVOID);
DWORD	IOCFreeMemory			(PVOID, PVOID);
DWORD	IOCMapBurnMem			(PVOID, PVOID);
DWORD	IOCUnmapBurnMem			(PVOID, PVOID);

void HostNonContigHeapAlloc(DWORD dwNumPages);
void FreeThreadAllocations(PTHREAD_CONTEXT const psThreadContext);
void FreePciRegionAllocations(PDISPATCH const psDispatch);
void FreeLongTermMappings();
void FreeProcessContext(PPROCESS_CONTEXT psProcessContext);

PTHREAD_CONTEXT FindThreadContext(const PPROCESS_CONTEXT psProcessContext, const HANDLE hThreadId);
#define FindFreeThreadContext(psProcessContext) FindThreadContext(psProcessContext, NULL)

PDISPATCH FindDispatch(HANDLE hProcessId);
#define FindFreeDispatch() FindDispatch(NULL)
#define FindCurrentDispatch() FindDispatch(HostGetCurrentProcessId())

#endif
/****************************************************************************
	End of File VPCIDD.H
****************************************************************************/



