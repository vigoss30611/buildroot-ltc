/******************************************************************************
* Name         : device_interface_master_transif.cpp
* Author       : PowerVR
*
* Copyright    : 1997 by VideoLogic Limited. All rights reserved.
* 				  No part of this software, either material or conceptual 
* 				  may be copied or distributed, transmitted, transcribed,
* 				  stored in a retrieval system or translated into any 
* 				  human or computer language in any form by any means,
* 				  electronic, mechanical, manual or other-wise, or 
* 				  disclosed to third parties without the express written
* 				  permission of VideoLogic Limited, Unit 8, HomePark
* 				  Industrial Estate, King's Langley, Hertfordshire,
* 				  WD4 8LZ, U.K.
*
* Description  : Abstrasct class providing a DeviceInterface to network interface.
*
******************************************************************************/
#include <iostream>
#include "device_interface_transif.h"
#include "img_defs.h"
#include "img_errors.h"

DeviceInterfaceTransif::DeviceInterfaceTransif(string devifName, SimpleDevifMemoryModel* pMemModel, TransifDevifModel* pSimulator)
: DeviceInterface(devifName), m_pSimulator(pSimulator), m_pMemModel(pMemModel)
{
	addToSystem(*this);
}


void DeviceInterfaceTransif::initialise(void)
{
	if (bInitialised) return;
	m_pSimulator->InitDevice();	
	bInitialised = true;
}

void DeviceInterfaceTransif::deinitialise(void)
{
	if (bInitialised) 
	{
		m_pSimulator->DeinitDevice();
		bInitialised = false;
	}
}

DeviceInterfaceTransif::~DeviceInterfaceTransif()
{
	// does not delete memory or C-sim, that is up to application
  removeFromSystem(name);
}

void DeviceInterfaceTransif::allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes)
{
	checkInitialised();
	if (!(m_pMemModel->alloc(memAddress, numBytes)))
	{
		std::cerr << "ERROR DeviceInterfaceTransif::allocMemory, address ";
		std::cerr << memAddress << ", size " << numBytes << std::endl;
		abort();
	}
}

void DeviceInterfaceTransif::freeMem(IMG_UINT64 memAddress)
{
	checkInitialised();
	m_pMemModel->free(memAddress);
}

IMG_UINT32 DeviceInterfaceTransif::readMemory(IMG_UINT64 deviceAddress)
{
	checkInitialised();
	return m_pMemModel->read(deviceAddress);
}

IMG_UINT32 DeviceInterfaceTransif::readRegister(IMG_UINT64 regOffset)
{
	checkInitialised();
	IMG_UINT32 busWidth = m_pSimulator->GetTXBusWidth();
	IMGBUS_DATA_TYPE d(busWidth);
	d.data[0] = 0; // reg read, data not used
	
	return m_pSimulator->RegisterIO(regOffset, d, true);
}

IMG_UINT64 DeviceInterfaceTransif::readMemory64(IMG_UINT64 deviceAddress)
{
	IMG_UINT64 ui64Val;
	checkInitialised();
	
	// Assuming that this is little endian
	ui64Val = m_pMemModel->read(deviceAddress + 4 );
	ui64Val <<= 32;
	ui64Val += m_pMemModel->read(deviceAddress);
	return ui64Val;
}

IMG_UINT64 DeviceInterfaceTransif::readRegister64(IMG_UINT64 regOffset)
{
	IMG_UINT64 value;

	value = readRegister(regOffset + 4);
	value <<= 32;
	value += readRegister(regOffset);
	return value;
}

IMG_RESULT DeviceInterfaceTransif::getDeviceTime(IMG_UINT64 * pui64Time)
{
	(void)pui64Time; // unused
	return IMG_SUCCESS;
}

void DeviceInterfaceTransif::writeMemory(IMG_UINT64 deviceAddress, IMG_UINT32 value)
{
	checkInitialised();
	m_pMemModel->write(deviceAddress, value);
}

void DeviceInterfaceTransif::writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value)
{
	checkInitialised();
	
	// Assuming little endian
	m_pMemModel->write(deviceAddress, (IMG_UINT32)(value & 0xFFFFFFFF));
	m_pMemModel->write(deviceAddress + 4, (IMG_UINT32)((value >> 32) & 0xFFFFFFFF));
}

void DeviceInterfaceTransif::writeRegister(IMG_UINT64 regOffset, IMG_UINT32 value)
{
	checkInitialised();
	IMG_UINT32 busWidth = m_pSimulator->GetTXBusWidth();
	IMGBUS_DATA_TYPE d(busWidth);
	d.data[0] = value;

	m_pSimulator->RegisterIO(regOffset, d, false);
}

void DeviceInterfaceTransif::writeRegister64(IMG_UINT64 regOffset, IMG_UINT64 value)
{
	writeRegister(regOffset, (IMG_UINT32)(value & 0xFFFFFFFF));
	writeRegister(regOffset + 4, (IMG_UINT32)((value >> 32) & 0xFFFFFFFF));
}


void DeviceInterfaceTransif::copyDeviceToHost(IMG_UINT64 deviceAddress, void *hostAddress, IMG_UINT32 numBytes)
{
	checkInitialised();
	m_pMemModel->block_read(deviceAddress, (IMG_BYTE*) hostAddress, numBytes);
}

void DeviceInterfaceTransif::copyHostToDevice(const void *hostAddress, IMG_UINT64 deviceAddress, IMG_UINT32 numBytes)
{			
	checkInitialised();
	m_pMemModel->block_write(deviceAddress, (IMG_BYTE*) hostAddress, numBytes);
}

void DeviceInterfaceTransif::loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords)
{
	(void)hostAddress; (void)regOffset; (void)numWords; // unused

	checkInitialised();
}

void DeviceInterfaceTransif::idle(IMG_UINT32 numCycles)
{
	checkInitialised();
	m_pSimulator->Idle(numCycles);
}

void DeviceInterfaceTransif::setAutoIdle(IMG_UINT32 numCycles)
{
	(void)numCycles; // unused

	checkInitialised();
}

void DeviceInterfaceTransif::sendComment(const IMG_CHAR* comment)
{
	//IMG_UINT32 ui32Length = (IMG_UINT32)strlen(comment);
	(void)comment; // unused

	checkInitialised();
}

void DeviceInterfaceTransif::devicePrint(const IMG_CHAR* string)
{
	//IMG_UINT32 ui32Length = (IMG_UINT32)strlen(string);
	(void)string; // unused

	checkInitialised();
}

void DeviceInterfaceTransif::setBaseNames(std::string sRegName, std::string sMemName)
{
	//IMG_UINT32 ui32Len = (IMG_UINT32)sRegName.length() + (IMG_UINT32)sMemName.length() + 2;
	(void)sRegName; (void)sMemName; // unused
}
