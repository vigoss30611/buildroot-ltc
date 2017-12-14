/*****************************************************************************
 Name     		: MAIN.C

 Title    		:

 Author 		: John Metcalfe

 Created  		: 1 Sep 2002

 Copyright		: 1997-2002 by VideoLogic Limited. All rights reserved.
                  No part of this software, either material or conceptual
                  may be copied or distributed, transmitted, transcribed,
                  stored in a retrieval system or translated into any
                  human or computer language in any form by any means,
                  electronic, mechanical, manual or other-wise, or
                  disclosed to third parties without the express written
                  permission of VideoLogic Limited, Unit 8, HomePark
                  Industrial Estate, King's Langley, Hertfordshire,
                  WD4 8LZ, U.K.

*****************************************************************************/

/*Disable warnings that system header generate*/
#pragma warning (disable : 4214)
#pragma warning (disable : 4514)
#pragma warning (disable : 4115)
#pragma warning (disable : 4201)

#include <ntddk.h>
#include <windef.h>
#include <winerror.h>

#include "img_types.h"
#include "pcidd.h"
#include "pciddif.h"
#include "hostfunc.h"
#include "ioctldef.h"

/*****************************************************************************
 PCIDD service table.
*****************************************************************************/
typedef struct _PCIDD_PROC_
{	DWORD (*function)(PVOID in,PVOID out);
	unsigned uIn;
	unsigned uOut;
} PCIDD_PROC;

PCIDD_PROC g_PCIDDProc[] =
{
	IOCFindDevice,			sizeof(FIND_DEVICE_INBLK),			sizeof(FIND_DEVICE_OUTBLK),
	IOCReadPCI8,			sizeof(CONFIG_READ_INBLK),			sizeof(CONFIG_READ_OUTBLK),
	IOCReadPCI16,			sizeof(CONFIG_READ_INBLK),			sizeof(CONFIG_READ_OUTBLK),
	IOCReadPCI32,			sizeof(CONFIG_READ_INBLK),			sizeof(CONFIG_READ_OUTBLK),
	IOCWritePCI8,			sizeof(CONFIG_WRITE_INBLK),			0,
	IOCWritePCI16,			sizeof(CONFIG_WRITE_INBLK),			0,
	IOCWritePCI32,			sizeof(CONFIG_WRITE_INBLK),			0,
	IOCRead8,				sizeof(PCI_READ_INBLK),				sizeof(PCI_READ_OUTBLK),
	IOCRead16,				sizeof(PCI_READ_INBLK),				sizeof(PCI_READ_OUTBLK),
	IOCRead32,				sizeof(PCI_READ_INBLK),				sizeof(PCI_READ_OUTBLK),
	IOCWrite8,				sizeof(PCI_WRITE_INBLK),			0,
	IOCWrite16,				sizeof(PCI_WRITE_INBLK),			0,
	IOCWrite32,				sizeof(PCI_WRITE_INBLK),			0,
	IOCWriteRead32,			sizeof(PCI_WRITE_INBLK),			sizeof(PCI_READ_OUTBLK),
	IOCReadBuffer,			sizeof(BUFFER_INBLK),				sizeof(DWORD),
	IOCWriteBuffer,			sizeof(BUFFER_INBLK),				sizeof(DWORD),
	IOCFillMem32,			sizeof(PCI_FILL_INBLK),				0,
	IOCReadMulti32,			sizeof(PCI_READM_INBLK),			sizeof(PCI_READ_OUTBLK),
	IOCBufferInfo,			0,									sizeof(HOSTBUFFER_INFO_OUTBLK),
	IOCMoveDevice,			sizeof(MOVE_DEVICE_INOUTBLK),		sizeof(MOVE_DEVICE_INOUTBLK),
	IOCAllocMemory,			sizeof(DWORD),						sizeof(CONTIG_NPP_INFO),
	IOCFreeMemory,			sizeof(DWORD),						0,
	IOCAllocMemory,			sizeof(DWORD),						sizeof(CONTIG_NPP_INFO),
	IOCFreeMemory,			sizeof(DWORD),						0,
	IOCMapBurnMem,			sizeof(DWORD),						sizeof(CONTIG_NPP_INFO),
	IOCUnmapBurnMem,		sizeof(DWORD),						0
};

#define NUM_PCIDD_W32_API NENTRIES(g_PCIDDProc)

DWORD	gdwMaxMdlSize = 0;
BOOL	gbUseSectionMapping = FALSE;

IMG_UINT32 gui32PCIMemWriteCombine	= 1;
IMG_UINT32 gui32OSAllocWriteCombine = 0;
IMG_UINT32 gui32MaxBarSize			= 0;

DISPATCH	gasDispatch[MAX_DISPATCHS] = {0};

extern PROCESS_CONTEXT gsAllocationContext;

/*****************************************************************************
 Function prototypes
*****************************************************************************/

#if 0
#if (NTDDI_VERSION >= NTDDI_WIN2K) && (!defined PRKPROCESS)

#define PRKPROCESS PEPROCESS

typedef struct _KAPC_STATE {
    LIST_ENTRY ApcListHead[MaximumMode];
    PRKPROCESS Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE, *PRKAPC_STATE;

VOID
KeStackAttachProcess (
    PRKPROCESS PROCESS,
    PRKAPC_STATE ApcState
    );

VOID
KeUnstackDetachProcess (
    PRKAPC_STATE ApcState
    );

#endif
#endif


BOOL OSReadRegistryDWORDFromString(IMG_UINT32 ui32DevCookie, PCHAR pszKey, PCHAR pszValue, IMG_UINT32 *pui32Data);


NTSTATUS DriverEntry(DRIVER_OBJECT *psDrvObj, UNICODE_STRING *psRegistryPath);
VOID DriverUnload(DRIVER_OBJECT *psDrvObj);
NTSTATUS DispatchCreate(DEVICE_OBJECT *psDevObj, IRP *psIrp);
NTSTATUS DispatchClose(DEVICE_OBJECT *psDevObj, IRP *psIrp);
NTSTATUS DispatchCleanup(DEVICE_OBJECT *psDevObj, IRP *psIrp);
NTSTATUS DispatchIOCTL(DEVICE_OBJECT *psDevObj, IRP *psIrp);

/**************************************************************************
 * Function Name  : DriverEntry
 * Inputs         : psDrvObj - pointer to the driver object
					psRegistryPath - pointer to unicode string for the drivers
					registry key
 * Outputs        : None
 * Returns        : STATUS_SUCCESS for success else appropiate status code
 * Globals Used   : None
 * Description    : This routine is called by NT to start the driver
 **************************************************************************/

NTSTATUS DriverEntry(DRIVER_OBJECT *psDrvObj, UNICODE_STRING *psRegistryPath)
{
	UNICODE_STRING		sNTDeviceName;
	UNICODE_STRING		sWin32DeviceName;
	DEVICE_OBJECT		*psDevObj;
	NTSTATUS			dwStatus;
	
	UNUSED(psRegistryPath);

	/* Fill in driver entry points */
	psDrvObj->MajorFunction[IRP_MJ_CREATE]			= DispatchCreate;
	psDrvObj->MajorFunction[IRP_MJ_CLOSE]			= DispatchClose;
	psDrvObj->MajorFunction[IRP_MJ_CLEANUP]			= DispatchCleanup;
	psDrvObj->MajorFunction[IRP_MJ_DEVICE_CONTROL]	= DispatchIOCTL;
	psDrvObj->DriverUnload							= DriverUnload;

	/* Find out what OS we are on */
	{
		RTL_OSVERSIONINFOW	sOsVersionInfo;

		RtlZeroMemory(&sOsVersionInfo, sizeof(RTL_OSVERSIONINFOW));
		sOsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

		RtlGetVersion(&sOsVersionInfo);

		if(sOsVersionInfo.dwMajorVersion == 5) /* Windows XP */
		{
			gdwMaxMdlSize = PAGE_SIZE * (0xFFFF - sizeof(MDL)) / sizeof(ULONG_PTR);
			gbUseSectionMapping = TRUE;
		}
		else if(sOsVersionInfo.dwMajorVersion == 6) /* Windows Vista or 7 */
		{
			gdwMaxMdlSize = 0x7FFFFFFF - PAGE_SIZE;
			gbUseSectionMapping = FALSE;
		}
		else /* Assume Windows 8 */
		{
			gdwMaxMdlSize = 0xFFFFFFFF - PAGE_SIZE;
			gbUseSectionMapping = FALSE;
		}
	}

	/* Create the device object */
	RtlInitUnicodeString(&sNTDeviceName, (PCWSTR) PCIDD_NT_DEVICE_NAME);
	dwStatus = IoCreateDevice(psDrvObj, 0, &sNTDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &psDevObj);
	if (NT_SUCCESS(dwStatus) == FALSE)
	{
		return dwStatus;
	}
	psDevObj->Flags |= DO_BUFFERED_IO;

	/* Create Win32 symbolic link */
	RtlInitUnicodeString(&sWin32DeviceName, (PCWSTR) PCIDD_NT_SYMLINK);
	dwStatus = IoCreateSymbolicLink(&sWin32DeviceName, &sNTDeviceName);
	if (NT_SUCCESS(dwStatus) == FALSE)
	{
		IoDeleteDevice(psDevObj);
		return dwStatus;
	}

	OSReadRegistryDWORDFromString(0, "Parameters", "PCIMemWriteCombine",  &gui32PCIMemWriteCombine);
	KdPrint(("imgpcidd: PCIMemWriteCombine=%i\n" ,gui32PCIMemWriteCombine ));


	OSReadRegistryDWORDFromString(0, "Parameters", "OSAllocWriteCombine", &gui32OSAllocWriteCombine);
	KdPrint(("imgpcidd: gui32OSAllocWriteCombine=%i\n" ,gui32OSAllocWriteCombine ));

	OSReadRegistryDWORDFromString(0, "Parameters", "maxBarSize",  &gui32MaxBarSize);
	KdPrint(("imgpcidd: maxBarSize=%x\n" ,gui32MaxBarSize ));


	return STATUS_SUCCESS;
}

/**************************************************************************
 * Function Name  : DriverUnload
 * Inputs         : psDrvObj - pointer to the driver object
 * Outputs        : None
 * Returns        : None
 * Globals Used   : None
 * Description    : Called by IO manager when the kernel manager is to be
 *					unloaded.
 **************************************************************************/
VOID DriverUnload(DRIVER_OBJECT *psDrvObj)
{
	DEVICE_OBJECT		*psDevObj = psDrvObj->DeviceObject;
	UNICODE_STRING		sWin32DeviceName;
#ifdef PRKPROCESS
	BOOL				bAttachedToProcess = FALSE;
	IMG_UINT32			i;
	PRKAPC_STATE		pApcState = NULL;

	pApcState = ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC_STATE), 'kapc');

	/* Free all memory */
	if(gsAllocationContext.hProcessId != HostGetCurrentProcessId() && pApcState != NULL)
	{
		KeStackAttachProcess(gsAllocationContext.pProcess, pApcState);
		bAttachedToProcess = TRUE;
	}

	FreeProcessContext(&gsAllocationContext);

	if(bAttachedToProcess)
	{
		KeUnstackDetachProcess(pApcState);
		bAttachedToProcess = FALSE;
	}

	for(i=0; i < MAX_DISPATCHS; i++)
	{
		if(gasDispatch[i].hProcessId == NULL)
		{
			continue;
		}

		/* We will be unmapping usermode memory so we need to be in the same process that mapped it to begin with */
		if(gasDispatch[i].hProcessId != HostGetCurrentProcessId() && pApcState != NULL)
		{
			KeStackAttachProcess(gasDispatch[i].pProcess, pApcState);
			bAttachedToProcess = TRUE;
		}
		
		FreePciRegionAllocations(&gasDispatch[i]); //free the allocations made from this dispatch

		if(bAttachedToProcess)
		{
			KeUnstackDetachProcess(pApcState);
			bAttachedToProcess = FALSE;
		}
	}

	if( pApcState != NULL )
	{
		ExFreePool( pApcState );
	}
#endif

	/* Free long term memory mappings */
	FreeLongTermMappings();

	/*Remove Win32 symbolic link*/
	RtlInitUnicodeString(&sWin32DeviceName, (PCWSTR)PCIDD_NT_SYMLINK);
	IoDeleteSymbolicLink(&sWin32DeviceName);

	/*Remove device object*/
	IoDeleteDevice(psDevObj);
}

/**************************************************************************
 * Function Name  : DispatchCreate
 * Inputs         : psDevObj - pointer to device object NOT USED
 *					psIrp - pointer to the requested IRP
 * Outputs        : None
 * Returns        : STATUS_SUCCESS, or fails if no more memory
 * Globals Used   : psDispatchInfoEntry
 * Description    : Routine called by IO manager when call the create a 
					file object is received
 **************************************************************************/
NTSTATUS DispatchCreate(DEVICE_OBJECT *psDevObj, IRP *psIrp)
{
	PDISPATCH psDispatch;
	NTSTATUS errStatus = STATUS_SUCCESS;

	UNUSED(psDevObj);

	psDispatch = FindCurrentDispatch();

	if(psDispatch == NULL) //it's a new process
	{
		psDispatch = FindFreeDispatch();

		if(psDispatch == NULL)
		{
			DBG_BREAK;
			errStatus = STATUS_ALLOTTED_SPACE_EXCEEDED;
		}
		else
		{
			RtlZeroMemory(psDispatch, sizeof(DISPATCH));
			psDispatch->hProcessId = HostGetCurrentProcessId();
			psDispatch->pProcess = HostGetCurrentProcess();
		}
	}

	if(psDispatch != NULL)
	{
		psDispatch->ui32FileOpenCount++;
	}

	/*Complete IRP*/
	psIrp->IoStatus.Status		= errStatus;
	psIrp->IoStatus.Information	= 0;
	IoCompleteRequest(psIrp, IO_NO_INCREMENT);

	return errStatus;
}

/**************************************************************************
 * Function Name  : DispatchClose
 * Inputs         : psDevObj - pointer to device object NOT USED
 *					psIrp - pointer to the requested IRP
 * Outputs        : None
 * Returns        : STATUS_SUCCESS, never fails
 * Globals Used   : None
 * Description    : Routine called by IO manager when call the create
 *					or close to a file object is received
 **************************************************************************/
NTSTATUS DispatchClose(DEVICE_OBJECT *psDevObj, IRP *psIrp)
{
	UNUSED(psDevObj);

	/*Complete IRP*/
	psIrp->IoStatus.Status		= STATUS_SUCCESS;
	psIrp->IoStatus.Information	= 0;
	IoCompleteRequest(psIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

/**************************************************************************
 * Function Name  : DispatchCleanup
 * Inputs         : psDevObj - pointer to device object NOT USED
 *					psIrp - pointer to the requested IRP
 * Outputs        : None
 * Returns        : STATUS_SUCCESS, never fails
 * Globals Used   : None
 * Description    : Routine called by IO manager when call the create
 *					or close to a file object is received
 **************************************************************************/
NTSTATUS DispatchCleanup(DEVICE_OBJECT *psDevObj, IRP *psIrp)
{
	PDISPATCH psDispatch;
	NTSTATUS errStatus = STATUS_SUCCESS;

	UNUSED(psDevObj);

	if( (psDispatch = FindCurrentDispatch()) != NULL )
	{
		psDispatch->ui32FileOpenCount--;

		if( psDispatch->ui32FileOpenCount == 0 )
		{
			FreePciRegionAllocations(psDispatch); /* free all the allocations made from this process */
			RtlZeroMemory(psDispatch, sizeof(DISPATCH));
		}
	}
	else
	{
		//Memory has not been freed!
		DBG_BREAK;
		errStatus = STATUS_ALLOTTED_SPACE_EXCEEDED;
	}


	/*Complete IRP*/
	psIrp->IoStatus.Status		= STATUS_SUCCESS;
	psIrp->IoStatus.Information	= 0;
	IoCompleteRequest(psIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
/**************************************************************************
 * Function Name  : DispatchIOCTL
 * Inputs         : psDevObj - pointer to device object NOT USED
 *					psIrp - pointer to the requested IRP
 * Outputs        : None
 * Returns        : STATUS_SUCCESS if ok else appropiate error
 * Globals Used   : None
 * Description    : Routine called by IO manager when IOCTL is sent
 *					to open file handle on file object
 **************************************************************************/
NTSTATUS DispatchIOCTL(DEVICE_OBJECT *psDevObj, IRP *psIrp)
{
	IO_STACK_LOCATION	*psIrpStack;
	NTSTATUS			ulStatus;
	DWORD				dwService;

	UNUSED(psDevObj);

	/*Get current stack clocation*/
	psIrpStack = IoGetCurrentIrpStackLocation(psIrp);

	/*Check IOCTL code*/
	dwService = MAKEIOCTLINDEX(psIrpStack->Parameters.DeviceIoControl.IoControlCode) - PCIDD_SERVICE_IOCTL_BASE;
	if (dwService >= NUM_PCIDD_W32_API ||
		psIrpStack->Parameters.DeviceIoControl.InputBufferLength<g_PCIDDProc[dwService-1].uIn ||
		psIrpStack->Parameters.DeviceIoControl.OutputBufferLength<g_PCIDDProc[dwService-1].uOut)
	{
		ulStatus = STATUS_INVALID_DEVICE_REQUEST;
		psIrp->IoStatus.Information = 0;
	}
	else
	{
		if ((g_PCIDDProc[dwService-1].function)(psIrp->AssociatedIrp.SystemBuffer, psIrp->AssociatedIrp.SystemBuffer) == NO_ERROR)
		{
			ulStatus = STATUS_SUCCESS;
			psIrp->IoStatus.Information = psIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
		}
		else
		{
			ulStatus = STATUS_INVALID_DEVICE_REQUEST;
			psIrp->IoStatus.Information = 0;
		}
	}

	/*Complete IRP*/
	psIrp->IoStatus.Status = ulStatus;
	IoCompleteRequest(psIrp, IO_NO_INCREMENT);
	return ulStatus;
}

/**************************************************************************
 * Function Name  : FindThreadContext
 * Description    : Walks through the linked list of dispatch infos to
					find the one associated with the given process
 **************************************************************************/
PTHREAD_CONTEXT FindThreadContext(const PPROCESS_CONTEXT psProcessContext, const HANDLE hThreadId)
{
	int i;

	if( psProcessContext == NULL )
	{
		return NULL;
	}

	for( i=0; i < MAX_THREAD_CONTEXTS; i++ )
	{
		if(psProcessContext->asThreadContext[i].hThreadId == hThreadId)
		{
			return &psProcessContext->asThreadContext[i];
		}
	}

	return NULL;
}

/**************************************************************************
 * Function Name  : FindDispatch
 * Description    : Walks through the linked list of dispatch infos to
					find the one associated with the given process
 **************************************************************************/
PDISPATCH FindDispatch(PEPROCESS hCurrentProcessId)
{
	IMG_UINT32 i;

	for(i=0; i < MAX_DISPATCHS; i++)
	{
		if(gasDispatch[i].hProcessId == hCurrentProcessId)
		{
			return &gasDispatch[i];
		}
	}

	return NULL;
}

/*****************************************************************************
 End of file (MAIN.C)
*****************************************************************************/
