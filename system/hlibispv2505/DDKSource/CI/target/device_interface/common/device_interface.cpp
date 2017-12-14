/*! 
******************************************************************************
 @file   : device_interface.cpp

 @brief  

 @Author Imagination Technologies

 @date   15/08/2007
 
         <b>Copyright 2005 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
        This file contains the implementation of the DeviceInterfaec class -
		an abstract base class used to wrap generic devices.


 \n<b>Platform:</b>\n
	     All 

******************************************************************************/
#include "device_interface.h"
#include "img_defs.h"
#include "img_errors.h"

using std::pair;

DeviceInterface::DeviceMap DeviceInterface::devices;

void DeviceInterface::addToSystem(DeviceInterface& device)
{
	if (find(device.name)) {
		IMG_ASSERT(false);
	}
	else {
		pair<string, DeviceInterface*> item(device.name, &device);
		devices.insert(item);
	}
}

void DeviceInterface::removeFromSystem(string deviceName)
{
	if (find(deviceName))
		devices.erase(deviceName);
}

DeviceInterface* DeviceInterface::find(string name)
{
	DeviceMap::iterator iterFind = devices.find(name);

	return iterFind != devices.end() ? iterFind->second : NULL;
}

DeviceInterface::DeviceInterface(string name)
	: context_(), bInitialised(false)
{
	this->name = name;
}

IMG_UINT64 DeviceInterface::allocMemOnSlave(IMG_UINT64 alignment, IMG_UINT32 numBytes)
{
	(void)alignment;(void)numBytes;
	return 0;
}
void DeviceInterface::loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords)
{
	(void)hostAddress;(void)regOffset;(void)numWords;
}

void DeviceInterface::writeRegister64(IMG_UINT64 deviceAddress, IMG_UINT64 value)
{
	writeRegister(deviceAddress, value & 0xFFFFFFFF);
	writeRegister(deviceAddress + 4, value >> 32);
}

void DeviceInterface::writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value)
{
	writeMemory(deviceAddress, value & 0xFFFFFFFF);
	writeMemory(deviceAddress + 4, value >> 32);
}

IMG_RESULT DeviceInterface::getDeviceTime(IMG_UINT64 * pui64Time)
{
	(void)pui64Time;
	return IMG_ERROR_GENERIC_FAILURE;
}

void DeviceInterface::allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes)
{
	(void)memAddress;
	(void)numBytes;
}

void DeviceInterface::freeMem(IMG_UINT64 memAddress)
{
	(void)memAddress;
}

void DeviceInterface::idle(IMG_UINT32 numCycles)
{
	(void)numCycles;
}


IMG_UINT64 DeviceInterface::readRegister64(IMG_UINT64 deviceAddress)
{
	IMG_UINT64 ui64Val;
	ui64Val = readRegister(deviceAddress + 4);
	ui64Val <<= 32;
	ui64Val |= readRegister(deviceAddress);
	return ui64Val;
}

IMG_UINT64 DeviceInterface::readMemory64(IMG_UINT64 deviceAddress)
{
	IMG_UINT64 ui64Val;
	ui64Val = readMemory(deviceAddress + 4);
	ui64Val <<= 32;
	ui64Val |= readMemory(deviceAddress);
	return ui64Val;
}

IMG_RESULT DeviceInterface::pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
    IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount)
{
    (void)ui64RegAddr; (void)eCmpOpType; (void)ui32ReqVal; (void)ui32EnableMask;
    (void)ui32PollCount; (void)ui32CycleCount;
    
    return IMG_ERROR_NOT_SUPPORTED;
}
    
IMG_RESULT DeviceInterface::pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
    IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount)
{
    (void)ui64RegAddr; (void)eCmpOpType; (void)ui64ReqVal; (void)ui64EnableMask;
    (void)ui32PollCount; (void)ui32CycleCount;

    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT DeviceInterface::pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
    IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount)
{
    (void)ui64MemAddr; (void)eCmpOpType; (void)ui32ReqVal; (void)ui32EnableMask;
    (void)ui32PollCount; (void)ui32CycleCount;
    
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT DeviceInterface::pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
    IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount)
{
    (void)ui64MemAddr; (void)eCmpOpType; (void)ui64ReqVal; (void)ui64EnableMask;
    (void)ui32PollCount; (void)ui32CycleCount;
    
    return IMG_ERROR_NOT_SUPPORTED;
}
