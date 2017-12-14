/*****************************************************************************
* Name		: pbif.h
* Title		: IOCTL Services Header File
*
* Author	: Roger Walker
* Created	: 7-Dec-2006
*
* Copyright	: 2006 by Imagination Technologies Limited. All rights reserved.
*			No part of this software, either material or conceptual
*			may be copied or distributed, transmitted, transcribed,
*			stored in a retrieval system or translated into any
*			human or computer language in any form by any means,
*			electronic, mechanical, manual or other-wise, or
*			disclosed to third parties without the express written
*			permission of Imagination Technologies Limited, Unit 8, HomePark
*			Industrial Estate, King's Langley, Hertfordshire,
*			WD4 8LZ, U.K.
*
* Description : Debug Drivers - IOCTL structs and funcs header file
*
* Program Type	: C header file
*
*/

#ifndef	_PBIF_
#define _PBIF_

#pragma warning(disable: 4201)

//#include "ioctldef.h"

/*****************************************************************************
 Constants
*****************************************************************************/

// IOCTL Interface

#define 	PB_SERVICE_IOCTL_BASE	0x990
#define		VPCI_MAP_DEVICE				CTL_CODE(FILE_DEVICE_UNKNOWN, PB_SERVICE_IOCTL_BASE + 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		VPCI_UNMAP_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, PB_SERVICE_IOCTL_BASE + 0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*****************************************************************************
 driver device name
*****************************************************************************/
#define PB_NT_DEVICE_NAME			L"\\Device\\IMGPBDRV"
#define PB_NT_SYMLINK				L"\\DosDevices\\IMGPBDRV"
#define PB_WIN32_DEVICE_NAME		"\\\\.\\IMGPBDRV"

#define PB_REGISTRY_PATH L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\ImgPB"

/*****************************************************************************
 NT exported function, used by disppatch.dll to get services table
*****************************************************************************/
#define PB_NT_IMGAGE_NAME L"\\drivers\\imgpbdd.sys"
#define PB_NT_GET_SERVICES_EXP_NAME "PBGetServices"

// IOCTL interface structures
typedef struct 
{	char		cDevice;
	unsigned	uBurnMemOffsetMeg;
	unsigned	uBurnMemSizeMeg;
} MAP_DEVICE_INBLK;

// this is the private data returned by MAP, required by unmap to unmap the block
typedef struct
{	int	iMapReg;
	int	iMapMem;
} MAP_PRIVATE_DATA;

typedef struct 
{	VOID		*pvRegisters;
	VOID		*pvMem;
	DWORD		dwMemSize;
	MAP_PRIVATE_DATA priv;
} MAP_DEVICE_OUTBLK;

typedef struct 
{	char		cDevice;
	MAP_PRIVATE_DATA priv;
} UNMAP_DEVICE_INBLK;

#endif
/****************************************************************************
	End of File PBIF.H
****************************************************************************/
