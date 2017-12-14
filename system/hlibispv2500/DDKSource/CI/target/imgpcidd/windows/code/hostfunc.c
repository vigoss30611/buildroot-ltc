/**************************************************************************
 * Name         : hostfunc.c
 * Title        : OS Dependant functions
 * Author       : Roger Coote
 * Created      : 8/12/97
 *
 * Copyright    : 1995 by VideoLogic Limited. All rights reserved.
 *              : No part of this software, either material or conceptual 
 *              : may be copied or distributed, transmitted, transcribed,
 *              : stored in a retrieval system or translated into any 
 *              : human or computer language in any form by any means,
 *              : electronic, mechanical, manual or other-wise, or 
 *              : disclosed to third parties without the express written
 *              : permission of VideoLogic Limited, Unit 8, HomePark
 *              : Industrial Estate, King's Langley, Hertfordshire,
 *              : WD4 8LZ, U.K.
 *
 * Description  : Functions that call non VideoPortXxxx rotuines
 
**************************************************************************/
#pragma warning (disable : 4162) // '_ReturnAddr' : no function with C linkage found
#pragma warning ( disable : 4100 4115 4201 4209 4214 4514 4390 4162)
#include <ntddk.h>
#include <windef.h>
#pragma warning ( 4 : 4001 4100 4115 4201 4209 4214 )

#include "img_types.h"
#include "hostfunc.h"

extern DWORD gdwMaxMdlSize;
extern BOOL gbUseSectionMapping;

/**************************************************************************
 * Function Name  : FreePageMapping
 * Description    : Freesd user mapped memory
 **************************************************************************/

static VOID FreePageMapping(PVOID pvUserAddr, PMDL psMDL)
{	
	if (pvUserAddr != NULL && psMDL != NULL)
	{	
		MmUnmapLockedPages(pvUserAddr, psMDL);
	}

	if (psMDL != NULL)
	{
		IoFreeMdl(psMDL);
	}
}

/**************************************************************************
 * Function Name  : UseMdlMapping
 * Description    : Returns true if we should attempt to map using an MDL
 **************************************************************************/

static BOOL UseMdlMapping(DWORD dwNumPages)
{
	return (dwNumPages * PAGE_SIZE <= gdwMaxMdlSize) && !gbUseSectionMapping;
}

/**************************************************************************
 * Function Name  : HostFreeContiguousNonPaged
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

static PVOID HostMapAddrUsingSection(PMEMORY_CONTEXT const psContext, MEMORY_CACHING_TYPE mmCacheType, BOOL bGetKrnlAddr)
{
	/* Map the memory using sections */
	UNICODE_STRING		sPhysMemUnicodeString;
	OBJECT_ATTRIBUTES	sPhsyMemObjAttr;
	NTSTATUS			ulStatus;
	HANDLE				hPhysMem;
	DWORD				dwPageAttrib = PAGE_READWRITE;
	PHYSICAL_ADDRESS	sPhysAddr;
	DWORD				dwSize = psContext->dwNumPages * PAGE_SIZE;
	ULONG				ulAttributes = OBJ_CASE_INSENSITIVE;
	PVOID				pvRtnAddr = NULL;

	if(bGetKrnlAddr)
	{
		ulAttributes |= OBJ_KERNEL_HANDLE;
	}

	psContext->bFileMapped = TRUE;

	switch (mmCacheType)
	{	
		case MmNonCached :
			dwPageAttrib |= PAGE_NOCACHE;
			break;

		case MmWriteCombined :
			dwPageAttrib |= PAGE_WRITECOMBINE;
			break;		
	}


	sPhysAddr.QuadPart = psContext->dwPhysAddress;

	/*Open a handle on physical memory*/
	RtlInitUnicodeString(&sPhysMemUnicodeString, (PCWSTR) L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&sPhsyMemObjAttr, &sPhysMemUnicodeString, ulAttributes, (HANDLE) NULL, (PSECURITY_DESCRIPTOR) NULL);
	ulStatus = ZwOpenSection(&hPhysMem, SECTION_ALL_ACCESS, &sPhsyMemObjAttr);
	if (NT_SUCCESS(ulStatus) == FALSE)
	{
		DbgPrint("Error opening physical memory section");
		return NULL;
	}

	/*Map view of physical memory*/
	if(!bGetKrnlAddr)
	{
		dwSize += BYTE_OFFSET(sPhysAddr.QuadPart);
	}
	ulStatus = ZwMapViewOfSection(hPhysMem, NtCurrentProcess(), &pvRtnAddr, 0, dwSize, &sPhysAddr, &dwSize, ViewUnmap, 0, dwPageAttrib);
	if (NT_SUCCESS(ulStatus) == FALSE)
	{
		DbgPrint("Failed to map view of physical memory into user space(0x%lx)", ulStatus);
		ZwClose(hPhysMem);
		return NULL;
	}

	/*Close section*/
	ZwClose(hPhysMem);

	return pvRtnAddr;
}

/**************************************************************************
 * Function Name  : HostUnMapAddrUsingSection
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

static VOID HostUnMapAddrUsingSection(PVOID pvLinAddr)
{
	NTSTATUS	ntStatus;

	if(pvLinAddr != NULL)
	{
		/* Unmap view of phys mem */
		ntStatus = ZwUnmapViewOfSection(NtCurrentProcess(), pvLinAddr);

		if (NT_SUCCESS(ntStatus) == FALSE)
		{	
			KdPrint(("imgpcidd: HostUnMapAddrUsingSection: Failed to unmap 0x%lx (error 0x%lX)\n", pvLinAddr, (LONG) ntStatus));
		}
	}
}

/**************************************************************************
 * Function Name  : HostFreeContiguousNonPaged
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

static BOOL HostMapUserMode(PMEMORY_CONTEXT const psContext, MEMORY_CACHING_TYPE mmCacheType)
{	
	if(psContext == NULL)
	{
		DBG_BREAK;
		return FALSE;
	}

	if(UseMdlMapping(psContext->dwNumPages))
	{
		psContext->bFileMapped = FALSE;

		// Map data area into user mode, non-cached
		if((psContext->psMDL = IoAllocateMdl(psContext->pvKrnlAddr, psContext->dwNumPages * PAGE_SIZE, FALSE, FALSE, NULL)) == NULL)
		{
			DBG_BREAK;
			goto MapSection;
		}
	
		MmBuildMdlForNonPagedPool(psContext->psMDL);

		if (MmGetMdlByteCount(psContext->psMDL) != psContext->dwNumPages * PAGE_SIZE)
		{
			DBG_BREAK;
			IoFreeMdl(psContext->psMDL);
			goto MapSection;
		}

		/* Set MDL_MAPPING_CAN_FAIL otherwise Driver Verifier on WinXP blue screen says it needs to be set */
		// psContext->psMDL->MdlFlags |= MDL_MAPPING_CAN_FAIL;

		__try
		{
			psContext->pvUserAddr = (PVOID) PAGE_ALIGN((ULONG) MmMapLockedPagesSpecifyCache(psContext->psMDL, UserMode, mmCacheType, NULL, FALSE, NormalPagePriority) + MmGetMdlByteOffset(psContext->psMDL));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			NTSTATUS code = GetExceptionCode();
			DbgPrint("Call to MmMapLocked failed due to exception 0x%0x\n", code);
			IoFreeMdl(psContext->psMDL);
			goto MapSection;
		}

		if (psContext->pvUserAddr == NULL)
		{
			DBG_BREAK;
			IoFreeMdl(psContext->psMDL);
			goto MapSection;
		}
	}
	else
MapSection:
	psContext->pvUserAddr = HostMapAddrUsingSection(psContext, mmCacheType, FALSE);

	return (psContext->pvUserAddr != NULL ? TRUE : FALSE);
}

/**************************************************************************
 * Function Name  : HostUnmapUserMem
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

static VOID HostUnmapUserMem(PMEMORY_CONTEXT const psContext)
{
	if(psContext == NULL)
	{
		DBG_BREAK;
		return;
	}

	if(psContext->bFileMapped)
	{
		HostUnMapAddrUsingSection(psContext->pvUserAddr);
	}
	else
	{
		FreePageMapping(psContext->pvUserAddr, psContext->psMDL);
	}
}

/**************************************************************************
 * Function Name  : HostMapPhysToLin
 * Inputs         : dwPhysBase - phys memory to map
 *					swPages - number of pages to map
 *					dwCacheType - required cacheing type of mapping (CACHETYPE_UNCACHED, CACHETYPE_CACHED or CACHETYPE_WRITECOMBINED)
 * Outputs        : None
 * Returns        : Linear addr of mapping on successful, else NULL
 * Globals Used   : None
 * Description    : Maps the physical memory into linear addr range
 **************************************************************************/
BOOL HostMapPhysToLin(PMEMORY_CONTEXT const psContext, MEMORY_CACHING_TYPE mmCacheType)
{	
	if (psContext == NULL || psContext->dwNumPages == 0)
	{	
		DBG_BREAK;
		return FALSE;
	}

	if(UseMdlMapping(psContext->dwNumPages) && psContext->pvKrnlAddr == NULL)
	{
		PHYSICAL_ADDRESS	sPhysAddr;
		sPhysAddr.QuadPart = psContext->dwPhysAddress;

		psContext->pvKrnlAddr = MmMapIoSpace(sPhysAddr, psContext->dwNumPages * PAGE_SIZE, mmCacheType);
	}
	else if (psContext->pvKrnlAddr == NULL)
	{
		psContext->pvKrnlAddr = HostMapAddrUsingSection(psContext, mmCacheType, TRUE);
	}

	if (psContext->pvKrnlAddr == NULL)
	{	
		DBG_BREAK;
		return FALSE;
	}

	return HostMapUserMode(psContext, mmCacheType);
}

/**************************************************************************
 * Function Name  : HostUnMapPhysToLin
 * Inputs         : pvLinAddr - linear addr of mapping
 *					swPages - number of pages that were mapped
 * Outputs        : None
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Unmaps memory that was mapped with HostMapPhysToLin
 **************************************************************************/
VOID HostUnMapPhysToLin(PMEMORY_CONTEXT const psContext)
{	
	HostUnmapUserMem(psContext);
}

/**************************************************************************
 * Function Name  : HostUnMapPhysToLin
 * Inputs         : pvLinAddr - linear addr of mapping
 *					swPages - number of pages that were mapped
 * Outputs        : None
 * Returns        : TRUE on success, else FALSE
 * Globals Used   : None
 * Description    : Unmaps memory that was mapped with HostMapPhysToLin
 **************************************************************************/
VOID HostUnMapLongTermMapping(PVOID pvKrnlAddr, DWORD dwSize, BOOL bFileMapping)
{	
	if(pvKrnlAddr != NULL)
	{
		if(bFileMapping)
		{
			HostUnMapAddrUsingSection(pvKrnlAddr);
		}
		else
		{
			MmUnmapIoSpace(pvKrnlAddr, dwSize);
		}
	}
}

/**************************************************************************
 * Function Name  : HostGetContext
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

BOOL HostGetContext(PMEMORY_CONTEXT const psContext, DWORD dwNumPages)
{
	static const PHYSICAL_ADDRESS sLowAddr		= {0};
	static const PHYSICAL_ADDRESS sHighAddr		= {0xFFFFFFFF}; //Limit to 32-bit range
	static const PHYSICAL_ADDRESS sBoundaryAddr = {0};

extern IMG_UINT32 gui32OSAllocWriteCombine;

	// Clear in case we try to free it later
	psContext->psMDL = NULL;
	psContext->pvUserAddr = NULL;
	
	// Do allocation, add an extra page at start to store context in
	if(dwNumPages > 1)
	{
		psContext->pvKrnlAddr = MmAllocateContiguousMemorySpecifyCache(dwNumPages * PAGE_SIZE, sLowAddr, sHighAddr, sBoundaryAddr, MmNonCached);
		psContext->dwNumPages = dwNumPages;
		psContext->dwUsedPages = dwNumPages;
		psContext->bUsage = CONTIGUOUS_MEM;
	}
	else
	{
		if(UseMdlMapping(psContext->dwNumPages))
		{
			psContext->pvKrnlAddr = MmAllocateNonCachedMemory(PAGE_SIZE * DEFAULT_ALLOCATION_SIZE);
			psContext->dwNumPages = DEFAULT_ALLOCATION_SIZE;
		}
		else
		{
			psContext->pvKrnlAddr = MmAllocateNonCachedMemory(PAGE_SIZE);
			psContext->dwNumPages = 1;
		}
		psContext->dwUsedPages = 1;
		psContext->bUsage = NON_CONTIGUOUS_MEM;
	}

	if (psContext->pvKrnlAddr == NULL || (PTR_TO_DWORD(psContext->pvKrnlAddr) & (PAGE_SIZE-1)) != 0) // It should be non null and page aligned, if not bail out
	{
		DBG_BREAK;
		return FALSE;
	}

	StorePhysicalAddresses(psContext->pvKrnlAddr, dwNumPages);
	psContext->dwPhysAddress = (DWORD) HostMapLinToPhys(psContext->pvKrnlAddr);

	if(!HostMapUserMode(psContext, (gui32OSAllocWriteCombine==1)? MmWriteCombined : MmNonCached))
	{
		FreeContext(psContext);
		return FALSE;
	}

	return TRUE;
}

/**************************************************************************
 * Function Name  : HostFreeContiguousNonPaged
 * Inputs         : psContext - Pointer to kernel mapped structure with info in
 **************************************************************************/

VOID FreeContext(PMEMORY_CONTEXT const psContext)
{	
	if(psContext == NULL)
	{
		DBG_BREAK;
		return;
	}

	HostUnmapUserMem(psContext);

	if(psContext->pvKrnlAddr != NULL)
	{
		if(psContext->bUsage == CONTIGUOUS_MEM)
		{
			MmFreeContiguousMemory(psContext->pvKrnlAddr);
		}
		else
		{
			MmFreeNonCachedMemory(psContext->pvKrnlAddr, PAGE_SIZE * psContext->dwNumPages);
		}
	}
}

void HostPCIWriteByte(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, UCHAR ucValue)
{
	PCI_SLOT_NUMBER sSlot;
	UCHAR			pucBuf[4];

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	pucBuf[0] = ucValue;

	HalSetBusDataByOffset(PCIConfiguration, dwBus, sSlot.u.AsULONG, pucBuf, dwReg, 1);
}

void HostPCIWriteWord(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, WORD wValue)
{
	PCI_SLOT_NUMBER sSlot;
	WORD			pwBuf[2];

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	pwBuf[0] = wValue;

	HalSetBusDataByOffset(PCIConfiguration, dwBus, sSlot.u.AsULONG, pwBuf, dwReg, 2);
}

void HostPCIWriteDword(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, DWORD dwValue)
{
	PCI_SLOT_NUMBER sSlot;
	DWORD			pdwBuf[1];

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	pdwBuf[0] = dwValue;

	HalSetBusDataByOffset(PCIConfiguration, dwBus, sSlot.u.AsULONG, pdwBuf, dwReg, 4);
}

/*
	These return the actual data, not an error indication
*/
BYTE HostPCIReadByte(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg)
{
	PCI_SLOT_NUMBER sSlot;
	UCHAR			pucBuf[4] = {0};

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	HalGetBusDataByOffset( PCIConfiguration, dwBus, sSlot.u.AsULONG, pucBuf, dwReg, 1 );

	return pucBuf[0];
}

WORD HostPCIReadWord(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg)
{
	PCI_SLOT_NUMBER sSlot;
	WORD			pwBuf[2] = {0};

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	HalGetBusDataByOffset( PCIConfiguration, dwBus, sSlot.u.AsULONG, pwBuf, dwReg, 2 );

	return pwBuf[0];
}

DWORD HostPCIReadDword(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg)
{
	PCI_SLOT_NUMBER sSlot;
	DWORD			pdwBuf[1] = {0};

	sSlot.u.bits.DeviceNumber = dwSlot;
	sSlot.u.bits.FunctionNumber = dwFunc;
	sSlot.u.bits.Reserved = 0;

	HalGetBusDataByOffset( PCIConfiguration, dwBus, sSlot.u.AsULONG, pdwBuf, dwReg, 4 );

	return pdwBuf[0];
}

/***********************************************************************************
 Function Name 		: StorePhysicalAddresses
 Inputs         	: Kernel mode address, number of bytes in block
 Outputs        	:
 Returns        	:
 Globals Used    	:
 Description    	: Walk through allocated block a paged at a time, storing each page's physical address
************************************************************************************/
void StorePhysicalAddresses(PVOID pvBase, DWORD dwNumPages)
{	
	DWORD dwPage;

	for (dwPage=0; dwPage < dwNumPages; dwPage++)
	{
		*((LONGLONG *) pvBase) = HostMapLinToPhys(pvBase);
		pvBase = (void *) (((char *) pvBase) + PAGE_SIZE);
	}
}

/*!
******************************************************************************

 @Function		HostGetSysMemSize
 
 @Description 	Gets the total amount of system RAM in system

 @Input
 @Output		pui32Bytes - bytes of RAM
 
 @Return		PVRSRV_ERROR :

******************************************************************************/
DWORD HostGetSysRamTop(void)
{
#if 1
	DBG_BREAK;
	
	return 0;
#else	
	SYSTEM_BASIC_INFORMATION	sSysInfo;
	NTSTATUS					sStatus;
	DWORD						dwTopOfRAM;

	/* Get basic system info */
	sStatus = ZwQuerySystemInformation (SystemBasicInformation, 
											&sSysInfo, 
											sizeof(sSysInfo), 
											NULL);
	if (NT_SUCCESS(sStatus) == FALSE)
	{
		KdPrint(("imgpcidd: HostGetSysRamTop : Failed to get basic system info (0x%lX)\n", sStatus));
		return 0;
	}

	/* Convert pages to bytes */
	dwTopOfRAM = sSysInfo.ulHighestPhysPage * PAGE_SIZE;
	/* Round up to next MB as this is the start addr of last page */
	dwTopOfRAM = (dwTopOfRAM + 0xfffff) & ~0xfffff;

	return dwTopOfRAM;
#endif	
}

/*--------------------------- End of File --------------------------------*/
