/*************************************************************************
 * Name         : hostfunc.h
 * Title        : OS independant functions
 * Author       : Roger Coote
 * Created      : 5/12/97
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
 * Description  : Prototypes of functions that common code can
 *				  use to access OS dependant functions
 
**************************************************************************/
#ifndef __HOSTFUNC_H__
#define __HOSTFUNC_H__

#include "Pciddif.h"

#pragma warning(disable: 4201 4121) /* Disable nameless unions */

typedef struct _MEMORY_CONTEXT_
{
	union {
		DWORD		dwPhysAddress;
		DWORD		dwIOAddress;
    } ;

	PVOID			pvKrnlAddr;		// Kernel mode linear address
	PVOID			pvUserAddr;		// Kernel mode linear address
	PMDL			psMDL;
	DWORD			dwNumPages;
	DWORD			dwUsedPages;
	DWORD			dwSize;
	BOOL			bFileMapped;
	BYTE			bUsage;			// IO or memory or unused

} MEMORY_CONTEXT, *PMEMORY_CONTEXT;


/****************************************************************************
	Some type defs
****************************************************************************/

typedef struct _CONTEXT_BUFFER_
{	
	struct _CONTEXT_BUFFER_	*	Next;

	DWORD						dwNumContexts;
	PMEMORY_CONTEXT				psContext;

} CONTEXT_BUFFER, *PCONTEXT_BUFFER;

/*****************************************************************************
 Static defines
*****************************************************************************/

#define HOSTFN_OK		0
#define HOSTFN_ERROR	1

#define	NON_CONTIGUOUS_MEM	0
#define CONTIGUOUS_MEM		1

#define DEFAULT_CONTEXTS_IN_BUFFER	0x200 /* Allocate context buffer that holds a bunch of contexts */
#define DEFAULT_ALLOCATION_SIZE		0x200 /* Allocate large chunks which are portioned off */

#ifndef NENTRIES
#define NENTRIES(a) (sizeof(a) / sizeof(a[0]))
#endif

#ifndef UNUSED
#define UNUSED(a) a
#endif

#ifndef DBG_BREAK
#ifdef DEBUG
#define DBG_BREAK __debugbreak()
#else
#define DBG_BREAK
#endif
#endif

#ifndef INTERNAL
#ifdef DEBUG
#define INTERNAL
#else
#define INTERNAL static
#endif
#endif

#define PTR_TO_DWORD(p) ((DWORD) ( ((char *)(p)) - (char *) NULL ) )
#define DWORD_TO_PTR(d) ((PVOID) ( ( ((char *) NULL) + (d) ) ) )

#define PTR_TO_DWORD64(p) ((DWORD64) ( ((char *) (p)) - (char *) NULL ) )
#define DWORD64_TO_PTR(d) ((PVOID) ( ( ((char *) NULL) + (d) ) ) )

/*****************************************************************************
 FN Prototypes..
*****************************************************************************/

#define HostGetCurrentThread()		PsGetCurrentThread()
#define HostGetCurrentThreadId()	PsGetCurrentThreadId()
#define HostGetCurrentProcess()		PsGetCurrentProcess()
#define HostGetCurrentProcessId()	PsGetCurrentProcessId()
#define HostMapLinToPhys(pvSysMem)	MmGetPhysicalAddress(pvSysMem).QuadPart

/* Mapping and phys */
BOOL HostMapPhysToLin(PMEMORY_CONTEXT const psContext, MEMORY_CACHING_TYPE mmCacheType);
VOID HostUnMapPhysToLin(PMEMORY_CONTEXT const psContext);
VOID HostUnMapLongTermMapping(PVOID pvKrnlAddr, DWORD dwSize, BOOL bFileMapping);

BOOL HostGetContext(PMEMORY_CONTEXT const psContext, DWORD dwNumPages);
VOID FreeContext(PMEMORY_CONTEXT const psContext);

VOID HostPCIWriteByte(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, BYTE byValue);
VOID HostPCIWriteWord(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, WORD wValue);
VOID HostPCIWriteDword(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg, DWORD dwValue);

BYTE HostPCIReadByte(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg);
WORD HostPCIReadWord(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg);
DWORD HostPCIReadDword(DWORD dwBus, DWORD dwFunc, DWORD dwSlot, DWORD dwReg);

void StorePhysicalAddresses(PVOID pvBase, DWORD dwNumPages);

DWORD HostGetSysRamTop(void);

#endif /* __HOSTFUNC_H__ */
/**************************************************************************
 End of file (HOSTFUNC.H)
**************************************************************************/
