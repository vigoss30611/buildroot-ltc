/*****************************************************************************
 Name		: PCIDDIF.H
 Title		: Highlander PCI IOCTL Services Header File

 Author		: John Metcalfe
 Created	: 15/9/97

 Copyright	: 1997-2002 by Imagination Technologies Limited. All rights reserved.
		  No part of this software, either material or conceptual
		  may be copied or distributed, transmitted, transcribed,
		  stored in a retrieval system or translated into any
		  human or computer language in any form by any means,
		  electronic, mechanical, manual or other-wise, or
		  disclosed to third parties without the express written
		  permission of Imagination Technologies Limited, Unit 8, HomePark
		  Industrial Estate, King's Langley, Hertfordshire,
		  WD4 8LZ, U.K.

 Description : VXD PCI Debug Vxd - header file

 Program Type	: C header file

*/

#ifndef	_PCIDDIF_
#define _PCIDDIF_

#pragma warning(disable: 4201 4121)

//#include "winioctl.h"
//#include "ioctldef.h"
#include <basetsd.h>

#include <img_types.h>

// MinGW
#ifndef POINTER_32
#define POINTER_32
#endif

typedef IMG_UINT32      IMG_SID;

/*****************************************************************************
 Constants
*****************************************************************************/

// IOCTL Interface

#define 	PCIDD_SERVICE_IOCTL_BASE	0x900
#define		VPCI_FIND_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ_PCI8				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ_PCI16				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x03, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ_PCI32				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x04, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE_PCI8				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x05, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE_PCI16			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x06, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE_PCI32			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x07, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ8					CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x08, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ16					CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x09, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ32					CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE8					CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE16				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE32				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITEREAD32			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READ_BUFFER			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x0F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_WRITE_BUFFER			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_FILLMEM32				CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_READMULTI32			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_HOSTBUFFER_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_MOVE_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_CONTIG_NON_PAGED_ALLOC	CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_CONTIG_NON_PAGED_FREE	CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x16, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_NON_PAGED_ALLOC		CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x17, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_NON_PAGED_FREE			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_MAP_BURNMEM			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x19, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_UNMAP_BURNMEM			CTL_CODE(FILE_DEVICE_UNKNOWN, PCIDD_SERVICE_IOCTL_BASE + 0x1A, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*****************************************************************************
 driver device name
*****************************************************************************/
#define PCIDD_NT_DEVICE_NAME			L"\\Device\\IMGPCIDDDRV"
#define PCIDD_NT_SYMLINK				L"\\DosDevices\\IMGPCIDDDRV"
#define PCIDD_WIN32_DEVICE_NAME			"\\\\.\\IMGPCIDDDRV"

#define PCIDD_REGISTRY_PATH L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\ImgPciDd"

/*****************************************************************************
 NT exported function, used by disppatch.dll to get services table
*****************************************************************************/
#define PCIDD_NT_IMGAGE_NAME L"\\drivers\\imgpcidd.sys"
#define PCIDD_NT_GET_SERVICES_EXP_NAME "PCIDDGetServices"

// Memory IO region constants

#define MAX_PCI_REGIONS 7
#define PCI_HOSTMEM_REGION 7
#define PCI_HOSTIO_REGION 8

#define	RGN_UNUSED		0
#define RGN_MEMMAPPED	1
#define RGN_IOMAPPED	2
#define RGN_ROM         3

typedef struct _KMCARDINFO_ *PKMCARDINFO;

#pragma pack(push)
#pragma pack(4)

typedef struct _PCIRegion_UM_
{
    union {
		DWORD		dwPhysAddress;
		DWORD		dwIOAddress;
    };
	void * POINTER_32  pvLinAddress;	// User mode linear address
	PVOID		pvKrnlLinAddress;		// Kernel mode linear address	
	DWORD		dwSize;
	BYTE		bUsage;				// IO or memory or unused
	BYTE		Pad[3];
} PCI_REGION_UM, *PPCI_REGION_UM;

// IOCTL interface structures

#define PCI_FIND_DONT_CARE	0xFFFF


#define PAGEACCES_USERMODE 0x1
typedef struct
{
	PVOID	pvBuf;
	DWORD	dwNumPages;
	DWORD	dwAccessMode;
}SET_PAGE_ACCESS_INBLK, *PSET_PAGE_ACCESS_INBLK;


typedef struct _FIND_DEVICE_INBLK_
{
	WORD	wBus;
	WORD	wVendorID;
	WORD	wDeviceID;
	WORD	wDeviceClass;
	// If there are n similar devices - which one?
	WORD	wDeviceCount;
} FIND_DEVICE_INBLK, *PFIND_DEVICE_INBLK;

typedef struct _FIND_DEVICE_OUTBLK_
{
	DWORD			dwBrdsFound;
	BYTE			bDeviceNum;
	BYTE			bBusNum;
	BYTE			bDeviceClass;
	BYTE			bHeaderType;
	WORD			wVendorID;
	WORD			wDeviceID;
	BYTE			bRevision;
	BYTE			bPad[3];
	PCI_REGION_UM	RegionInfo[MAX_PCI_REGIONS];
} FIND_DEVICE_OUTBLK, *PFIND_DEVICE_OUTBLK;

typedef struct _MOVE_DEVICE_INBLK_
{
	DWORD	dwBarNum;			/* The mem BAR of the device that we want to move */
	DWORD	dwNewPhys;
	BOOL	bMovedOk;
	BOOL	bDevFound;
	WORD	wBus;
	WORD	wVendorID;
	WORD	wDeviceID;
} MOVE_DEVICE_INOUTBLK, *PMOVE_DEVICE_INOUTBLK;

typedef struct _MOVE_PVRAGP_DEVICE_INOUTBLK_
{
	DWORD	dwBarNum;			/* The mem BAR of the device that we want to move */
	DWORD	dwNewFBPhys;
	DWORD	dwRetCode;
	WORD	wBus;
	WORD	wVendorID;
	WORD	wDeviceID;
}MOVE_PVRAGP_DEVICE_INOUTBLK, *PMOVE_PVRAGP_DEVICE_INOUTBLK;

#define MOVE_PVRAGP_DEVICE_OK		0
#define MOVE_PVRAGP_DEVICE_NO_DEV	1
#define MOVE_PVRAGP_DEVICE_CONFLICT 2

typedef struct _CONFIG_READ_INBLK_
{
	WORD	RegAddress;
} CONFIG_READ_INBLK, *PCONFIG_READ_INBLK;


typedef struct _CONFIG_READ_OUTBLK_
{
	union {
		BYTE	RegValue8, RV8Pad[3];
		WORD	RegValue16, RV16Pad;
		DWORD	RegValue32;
	};
} CONFIG_READ_OUTBLK, *PCONFIG_READ_OUTBLK;


typedef struct _CONFIG_WRITE_INBLK_
{
	WORD	RegAddress;
	union {
		BYTE	RegValue8, WV8Pad[3];
		WORD	RegValue16, WV16Pad;
		DWORD	RegValue32;
	};
} CONFIG_WRITE_INBLK, *PCONFIG_WRITE_INBLK;


typedef struct _PCI_READ_INBLK_
{
	WORD	Region;
	DWORD	Address;
} PCI_READ_INBLK, *PPCI_READ_INBLK;


typedef struct _PCI_READM_INBLK_
{
	WORD	Region;
	DWORD	Address;
	DWORD	dwSize;		// In Bytes - # of bytes to read
} PCI_READM_INBLK, *PPCI_READM_INBLK;


typedef struct _PCI_READ_OUTBLK_
{
	DWORD	dwErrStatus;
	union {
		BYTE	RetValue8, RV8Pad[3];
		WORD	RetValue16, RV16Pad;
		DWORD	RetValue32;
	};
} PCI_READ_OUTBLK, *PPCI_READ_OUTBLK;


typedef struct _PCI_WRITE_INBLK_
{
	WORD	Region;
	DWORD	Address;
	union {
		BYTE	Value8, V8Pad[3];
		WORD	Value16, V16Pad;
		DWORD	Value32;
	};
} PCI_WRITE_INBLK, *PPCI_WRITE_INBLK;


typedef struct _PCI_FILL_INBLK_
{
	WORD	Region;
	DWORD	Address;
	DWORD	dwSize;		// In bytes
	union {
		BYTE	Value8, V8Pad[3];
		WORD	Value16, V16Pad;
		DWORD	Value32;
	};
} PCI_FILL_INBLK, *PPCI_FILL_INBLK;


typedef struct _BUFFER_INBLK_
{
	DWORD	dwErrStatus;
	DWORD	Address;
	DWORD	dwSize;		// In bytes
	WORD	Region;
	BYTE	*pBuff;
	BYTE	byPad;
} BUFFER_INBLK, *PBUFFER_INBLK;

typedef struct _HOSTBUFFER_INFO_OUTBLK_
{
	void * POINTER_32 pv32LinAddress;
	DWORD64           dw64PhysAddress;
	DWORD	          MemSize;

	WORD              wMajorRev;
	WORD              wMinorRev;
} HOSTBUFFER_INFO_OUTBLK, *PHOSTBUFFER_INFO_OUTBLK;


typedef struct _MAPPED_ADDRS_
{
	DWORD                  PhysAddr;
	PVOID                  LinearAddr;
	struct _MAPPED_ADDRS_ *pNext;
} MAPPED_ADDRS, *PMAPPED_ADDRS;

typedef struct
{
	PVOID	pvNTAGPSrv;
} GET_SET_NT_AGPSRV, *PGET_SET_NT_AGPSRV;

typedef struct
{
	IMG_SID           hContextID;
	DWORD             dwNumPages;
	void * POINTER_32 pv32UserMode;
	DWORD64           dw64PhysAddr;
}CONTIG_NPP_INFO, *PCONTIG_NPP_INFO;

#pragma warning(default: 4201 4121)
	#pragma pack(pop)

#endif
/****************************************************************************
	End of File PCIDDIF.H
****************************************************************************/
