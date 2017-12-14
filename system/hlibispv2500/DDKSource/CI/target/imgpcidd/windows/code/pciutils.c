/*****************************************************************************
 Name			: PCIUTILS.C
 Title			: PCI Read / Write Utilities
 C Author		: John Metcalfe
 Created		: 20/9/2002

 Copyright		:	1996-2002 by Imagination Technologies Limited. All rights reserved.
					No part of this software, either material or conceptual
					may be copied or distributed, transmitted, transcribed,
					stored in a retrieval system or translated into any
					human or computer language in any form by any means,
					electronic, mechanical, manual or other-wise, or
					disclosed to third parties without the express written
					permission of Imagination Technologies Limited, Unit 8, HomePark
					Industrial Estate, King's Langley, Hertfordshire,
					WD4 8LZ, U.K.

 Description	: PCI read write utilities

*****************************************************************************/

#pragma warning(disable: 4115 4201 4214 4514)

#include <ntddk.h>
#include <windef.h>
#include <winerror.h>

#pragma warning(default: 4115 4201 4214)

#include "pciutils.h"

/**************************************************************************
	Function Name	: PCIReadByte
	Inputs			: PCI bus, function, device and register
	Returns			: Data from PCI config register
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
BYTE PCIReadByte(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	return (BYTE)(_inp(0xCFC));
}


/**************************************************************************
	Function Name	: PCIReadWord
	Inputs			: PCI bus, function, device and register
	Returns			: Data from PCI config register
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
WORD PCIReadWord(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	return (_inpw(0xCFC));
}


/**************************************************************************
	Function Name	: PCIReadDword
	Inputs			: PCI bus, function, device and register
	Returns			: Data from PCI config register
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
DWORD PCIReadDword(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	return (_inpd(0xCFC));
}


/**************************************************************************
	Function Name	: PCIWriteDword
	Inputs			: PCI bus, function, device, register and data
	Returns			: None
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
void PCIWriteDword(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg, DWORD dwData)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	_outpd(0xCFC, dwData);
	return;
}


/**************************************************************************
	Function Name	: PCIWriteWord
	Inputs			: PCI bus, function, device, register and data
	Returns			: None
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
void PCIWriteWord(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg, WORD wData)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	_outpw(0xCFC, wData);
	return;
}



/**************************************************************************
	Function Name	: PCIWriteByte
	Inputs			: PCI bus, function, device, register and data
	Returns			: None
	Globals Used	: None
	Description		: Reads PCI configuration register
***************************************************************************/
void PCIWriteByte(BYTE bBus, BYTE bFunc, BYTE bDevice, BYTE bReg, BYTE bData)
{
	DWORD	dwAddr;

	dwAddr = 0x80000000 | (bBus << 16) | ((bDevice & 0x1F) << 11) | ((bFunc & 0x07) << 8) | (bReg & 0xFC);

	_outpd(0xCF8, dwAddr);
	_outp(0xCFC, bData);
	return;
}


/*****************************************************************************
	End of file (PCIUTILS.C)
*****************************************************************************/
