/******************************************************************************
 * Name         : device_interface_master.cpp
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
#include "device_interface_master.h"
#include "img_defs.h"
#include "img_errors.h"

#ifdef WINCE
	#include <wince_thread.h>
#endif


DeviceInterfaceMaster::DeviceInterfaceMaster(std::string name, IMG_BOOL useBuffer, std::string parentName)
	: DeviceInterface(name)
	, pCmdBuffer(NULL)
{
	if (parentName.empty() == IMG_FALSE)
	{
		psParentDev = (DeviceInterfaceMaster*)find(parentName);
		sParentName = parentName;
	}
	else
		psParentDev = IMG_NULL;
	bUseBuffer = useBuffer;

#ifdef OSA_MUTEX
    OSA_CritSectCreate(&hThreadSafeMutex);
#endif
}

DeviceInterfaceMaster::~DeviceInterfaceMaster()
{
#ifdef OSA_MUTEX
    OSA_CritSectDestroy(hThreadSafeMutex);
#endif
}

void DeviceInterfaceMaster::sendCmd(EDeviceInterfaceCommand eCmd, IMG_UINT64 iVal1=0, IMG_UINT64 iVal2=0)
{
	IMG_UINT32 ui32Data;

	switch (ui32TransferMode)
	{
	case E_DEVIFT_32BIT:
		// Check to see if we are getting 64bit commands, interface needs to be changed
		if ( ((iVal1 >> 32) > 0) || ((iVal2 >> 32) > 0) )
		{
			if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(1,0))
			{
				printf("Trying to send 64bit word over interface which is only capable of 32bit, slave software needs updating");
				IMG_ASSERT(IMG_FALSE);
			}
			else
			{
				sendCmd(E_DEVIF_TRANSFER_MODE, E_DEVIFT_64BIT, 0);
				ui32TransferMode = E_DEVIFT_64BIT;
				sendByte(static_cast<char>(eCmd));
				sendWord64(iVal1);
				sendWord64(iVal2);
			}
		}
		else
		{
			sendByte(static_cast<char>(eCmd));
			ui32Data = (IMG_UINT32)(iVal1);
			sendWord(ui32Data);
			ui32Data = (IMG_UINT32)(iVal2);
			sendWord(ui32Data);
		}
		break;
	case E_DEVIFT_64BIT:
		sendByte(static_cast<char>(eCmd));
		sendWord64(iVal1);
		sendWord64(iVal2);
		break;
	default:
		printf("ERROR: DEVIF Unknown Message Transfer Type %08X\n", ui32TransferMode);
		IMG_ASSERT(IMG_FALSE);
	}
	// If we have a parent device send a device message
	if (psParentDev)
	{
		psParentDev->processCommand(ui32DeviceId);
	}
	if (!bUseBuffer)
	{
		flushBuffer();
	}
}



void DeviceInterfaceMaster::initialise(void)
{
	if (!bInitialised) 
	{
		bInitialised = true;

		ui32TransferMode = E_DEVIFT_32BIT;
		pCmdBuffer = new IMG_UINT8[i32BufSize];

		if (!pCmdBuffer)
		{
			printf("ERROR: Failed to create dev_if_master command buffer\n");
			cleanup();
			abort();
		}
		i32ReadyCount = 0;
	}
}

void DeviceInterfaceMaster::deinitialise(void)
{
	if (bInitialised) 
	{
		bInitialised = IMG_FALSE;
		if (psParentDev == IMG_NULL)
			sendCmd(E_DEVIF_END, 0, 0);
		else
		{
			if (find(sParentName))
				psParentDev->deinitialise();
		}
		if (pCmdBuffer)
		{
			flushBuffer();
			delete[] pCmdBuffer;
			pCmdBuffer = IMG_NULL;
			i32ReadyCount = 0;
		}
		cleanup();
		removeFromSystem(name);
	}
}


void DeviceInterfaceMaster::processCommand(IMG_UINT32 ui32DevId)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_USE_DEVICE, ui32DevId);
	flushBuffer();
	exitMutex();
}

/*DeviceInterfaceMaster::~DeviceInterfaceMaster()
{

}*/

void DeviceInterfaceMaster::allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_ALLOC, memAddress, numBytes);
	exitMutex();
}

IMG_UINT64 DeviceInterfaceMaster::allocMemOnSlave(IMG_UINT64 alignment, IMG_UINT32 numBytes)
{
	IMG_UINT64 address;
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_ALLOC_SLAVE, alignment, numBytes);
	recvWord64(address);
	exitMutex();
	return address;
}

void DeviceInterfaceMaster::freeMem(IMG_UINT64 memAddress)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_FREE, memAddress);
	exitMutex();
}

IMG_UINT32 DeviceInterfaceMaster::readMemory(IMG_UINT64 deviceAddress)
{
	IMG_UINT32 value = 0;
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_READMEM, deviceAddress);
	recvWord(value);
	exitMutex();
	return value;
}

IMG_UINT64 DeviceInterfaceMaster::readMemory64(IMG_UINT64 deviceAddress)
{
	IMG_UINT64 value = 0;
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_READMEM_64, deviceAddress);
	recvWord64(value);
	exitMutex();
	return value;
}

IMG_UINT32 DeviceInterfaceMaster::readRegister(IMG_UINT64 regOffset)
{
	IMG_UINT32 value = 0;
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_READREG, regOffset);
	recvWord(value);
	exitMutex();
	return value;
}

IMG_UINT64 DeviceInterfaceMaster::readRegister64(IMG_UINT64 regOffset)
{
	IMG_UINT64 value = 0;
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_READREG_64, regOffset);
	recvWord64(value);
	exitMutex();
	return value;
}

IMG_RESULT DeviceInterfaceMaster::getDeviceTime(IMG_UINT64 * pui64Time)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_GET_DEV_TIME);
	recvWord64(*pui64Time);
	exitMutex();
	if (*pui64Time != DEVIF_INVALID_TIME)
	{
		return IMG_SUCCESS;
	}
	else
	{
		return IMG_ERROR_GENERIC_FAILURE;
	}
}

void DeviceInterfaceMaster::writeMemory(IMG_UINT64 deviceAddress, IMG_UINT32 value)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_WRITEMEM, deviceAddress, value);
	exitMutex();
}

void DeviceInterfaceMaster::writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_WRITEMEM_64, deviceAddress, value);
	exitMutex();
}

void DeviceInterfaceMaster::writeRegister(IMG_UINT64 regOffset, IMG_UINT32 value)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_WRITEREG, regOffset, value);
	exitMutex();
}

void DeviceInterfaceMaster::writeRegister64(IMG_UINT64 regOffset, IMG_UINT64 value)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_WRITEREG_64, regOffset, value);
	exitMutex();
}

void DeviceInterfaceMaster::copyDeviceToHost(IMG_UINT64 deviceAddress, void *hostAddress, IMG_UINT32 numBytes)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_COPYDEVTOHOST, deviceAddress, numBytes);
	recvData(numBytes, hostAddress);
	exitMutex();
}

void DeviceInterfaceMaster::copyHostToDevice(const void *hostAddress, IMG_UINT64 deviceAddress, IMG_UINT32 numBytes)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_COPYHOSTTODEV, deviceAddress, numBytes);
	sendData(numBytes, hostAddress);
	exitMutex();
}

void DeviceInterfaceMaster::loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_LOADREG, regOffset, numWords);
	sendData(numWords * 4, hostAddress);
	exitMutex();
}

void DeviceInterfaceMaster::idle(IMG_UINT32 numCycles)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_IDLE, numCycles);
	exitMutex();
}
void DeviceInterfaceMaster::setAutoIdle(IMG_UINT32 numCycles)
{
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_AUTOIDLE, numCycles);
	exitMutex();
}

void DeviceInterfaceMaster::sendComment(const IMG_CHAR *pszComment)
{
	IMG_UINT32 ui32Length = (IMG_UINT32)strlen(pszComment);
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_SEND_COMMENT, ui32Length, 0);
	sendData(ui32Length, pszComment);
	exitMutex();
}

void DeviceInterfaceMaster::devicePrint(const IMG_CHAR *pszString)
{
	IMG_UINT32 ui32Length = (IMG_UINT32)strlen(pszString);
	checkInitialised();
	enterMutex();
	sendCmd(E_DEVIF_PRINT, ui32Length, 0);
	sendData(ui32Length, pszString);
	flushBuffer();
	exitMutex();
}

void DeviceInterfaceMaster::setBaseNames(std::string sRegName, std::string sMemName)
{
	enterMutex();
	sendCmd( E_DEVIF_SET_BASENAMES, sRegName.length(), sMemName.length() );
	sendData( (IMG_UINT32)sRegName.length(), sRegName.c_str() );
	sendData( (IMG_UINT32)sMemName.length(), sMemName.c_str() );
	flushBuffer();
	exitMutex();
}

IMG_RESULT DeviceInterfaceMaster::pollRegister(IMG_UINT64 ui64RegAddr,
    CompareOperationType eCmpOpType, IMG_UINT32 ui32ReqVal,
    IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount,
    IMG_UINT32 ui32CycleCount)
{
    IMG_UINT32 value = IMG_ERROR_FATAL;
    
    checkInitialised();
    
    if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2, 1))
        return IMG_ERROR_NOT_SUPPORTED;
    
    enterMutex();
	sendCmd(E_DEVIF_POLLREG, ui64RegAddr);
    sendByte((IMG_UINT8)eCmpOpType);
    sendWord(ui32ReqVal);
    sendWord(ui32EnableMask);
    sendWord(ui32PollCount);
    sendWord(ui32CycleCount);
    
    recvWord(value);
	exitMutex();
    
    return (IMG_RESULT)value;
}

IMG_RESULT DeviceInterfaceMaster::pollRegister(IMG_UINT64 ui64RegAddr,
    CompareOperationType eCmpOpType, IMG_UINT64 ui64ReqVal,
    IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount,
    IMG_UINT32 ui32CycleCount)
{
    IMG_UINT32 value = IMG_ERROR_FATAL;
    
    checkInitialised();
    
    if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2, 1))
        return IMG_ERROR_NOT_SUPPORTED;

    enterMutex();
	sendCmd(E_DEVIF_POLLREG_64, ui64RegAddr);
    sendByte((IMG_UINT8)eCmpOpType);
    sendWord64(ui64ReqVal);
    sendWord64(ui64EnableMask);
    sendWord(ui32PollCount);
    sendWord(ui32CycleCount);
    
    recvWord(value);
	exitMutex();
    
    return (IMG_RESULT)value;
}

IMG_RESULT DeviceInterfaceMaster::pollMemory(IMG_UINT64 ui64MemAddr,
    CompareOperationType eCmpOpType, IMG_UINT32 ui32ReqVal,
    IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount,
    IMG_UINT32 ui32CycleCount)
{
    IMG_UINT32 value = IMG_ERROR_FATAL;
    
    checkInitialised();
    
    if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2, 1))
        return IMG_ERROR_NOT_SUPPORTED;

    enterMutex();
	sendCmd(E_DEVIF_POLLMEM, ui64MemAddr);
    sendByte((IMG_UINT8)eCmpOpType);
    sendWord(ui32ReqVal);
    sendWord(ui32EnableMask);
    sendWord(ui32PollCount);
    sendWord(ui32CycleCount);
    
    recvWord(value);
	exitMutex();
    
    return (IMG_RESULT)value;
}

IMG_RESULT DeviceInterfaceMaster::pollMemory(IMG_UINT64 ui64MemAddr,
    CompareOperationType eCmpOpType, IMG_UINT64 ui64ReqVal,
    IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount,
    IMG_UINT32 ui32CycleCount)
{
    IMG_UINT32 value = IMG_ERROR_FATAL;
    
    checkInitialised();
    
    if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2, 1))
        return IMG_ERROR_NOT_SUPPORTED;

    enterMutex();
	sendCmd(E_DEVIF_POLLMEM_64, ui64MemAddr);
    sendByte((IMG_UINT8)eCmpOpType);
    sendWord64(ui64ReqVal);
    sendWord64(ui64EnableMask);
    sendWord(ui32PollCount);
    sendWord(ui32CycleCount);
    
    recvWord(value);
	exitMutex();
    
    return (IMG_RESULT)value;
}
