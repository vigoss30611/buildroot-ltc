/******************************************************************************
 * Name         : hidebug.c
 * Title        : The Hardware Intermediate Debug file processing.
 * Author       : Steve Morphet/Peter Leaback
 * Created      : 10/03/2003
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
#ifndef DEVICE_INTERFACE_MASTER
#define DEVICE_INTERFACE_MASTER

#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))

#include "device_interface.h"
#include <string>
#include "device_interface_message.h"
#include <string.h>

#ifdef MEOS_MUTEX
#include "krn.h"
#endif

#ifdef OSA_MUTEX
#include "osa.h"
#endif

class DeviceInterfaceMaster : public DeviceInterface
{
	public:
	DeviceInterfaceMaster(std::string name, IMG_BOOL useBuffer=IMG_TRUE, std::string parentName="");
	~DeviceInterfaceMaster();

	virtual void initialise(void);
	virtual void deinitialise(void);
	virtual IMG_UINT32 readMemory(IMG_UINT64 deviceAddress);
	virtual IMG_UINT64 readMemory64(IMG_UINT64 deviceAddress);
	virtual IMG_UINT32 readRegister(IMG_UINT64 deviceAddress);
	virtual IMG_UINT64 readRegister64(IMG_UINT64 deviceAddress);
	virtual IMG_RESULT getDeviceTime(IMG_UINT64 * pui64Time);
	virtual void writeMemory(IMG_UINT64 deviceAddress, IMG_UINT32 value);
	virtual void writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	virtual void writeRegister(IMG_UINT64 deviceAddress, IMG_UINT32 value);
	virtual void writeRegister64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	virtual void copyDeviceToHost(IMG_UINT64 deviceAddress, void *hostAddress, IMG_UINT32 numBytes);
	virtual void copyHostToDevice(const void *hostAddress, IMG_UINT64 deviceAddress, IMG_UINT32 numBytes);
	virtual void loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords);
	virtual void allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes);
	virtual IMG_UINT64 allocMemOnSlave(IMG_UINT64 alignment, IMG_UINT32 numBytes);
	virtual void freeMem(IMG_UINT64 memAddress);
	virtual void idle(IMG_UINT32 numCycles);
	virtual void setAutoIdle(IMG_UINT32 numCycles);
	virtual void sendComment(const IMG_CHAR *pszComment);
	virtual void devicePrint(const IMG_CHAR *pszString);
	virtual void processCommand(IMG_UINT32 ui32DevId);

    virtual IMG_RESULT pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
        IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    virtual IMG_RESULT pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
        IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    virtual IMG_RESULT pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
        IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    virtual IMG_RESULT pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
        IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);

	virtual DeviceInterface* addNewDevice(std::string name, IMG_BOOL useBuffer)
    {
        (void)name; (void)useBuffer; // unused
        return IMG_NULL;
    }
	virtual void setBaseNames(std::string sRegName, std::string sMemName);

	string&		getRemoteName(){return sRemoteName;}
	string&		getRemoteVersion(){return sRemoteVersion;}
	IMG_UINT32	getRemoteProtocol(){return ui32RemoteProtocol;} // This is protocol version
	
	private:
	
	DeviceInterfaceMaster();	// prevent default constructor

	// prevent default assignment and copy constructors from being created
	DeviceInterfaceMaster (const DeviceInterfaceMaster &foo);
	DeviceInterfaceMaster& operator=(const DeviceInterfaceMaster &foo);

protected:
	virtual void flushBuffer() = 0;
	virtual void recvData(IMG_INT32 i32NumBytes, void* pData)=0;
	virtual void cleanup() = 0;
	virtual void devif_ntohl(IMG_UINT32& val) = 0;
	virtual void devif_htonl(IMG_UINT32& val) = 0;
	virtual void devif_ntohll(IMG_UINT64& val) = 0;
	virtual void devif_htonll(IMG_UINT64& val) = 0;
	
	void sendCmd(EDeviceInterfaceCommand eCmd, IMG_UINT64 iVal1, IMG_UINT64 iVal2);

	IMG_BOOL getString(char* pszString, IMG_UINT32 uBufferSize)
	{
		if (pszString == NULL || uBufferSize == 0) return IMG_FALSE;

		char* pTmp = pszString;
		do
		{
			recvData(1, pTmp);
		} while (--uBufferSize &&  *pTmp++);
		if ( uBufferSize ) return IMG_TRUE;
		return IMG_FALSE;
	}
	void sendByte(IMG_UINT8 iVal)
	{
		pCmdBuffer[i32ReadyCount++] = iVal;
		if (i32ReadyCount == i32BufSize)
		{
			flushBuffer();
		}
	}
	void sendData(IMG_INT32 i32NumBytes, const void*  pData)
	{ 
		// Send any data we have space for in this buffer.
		while(i32NumBytes)
		{

			IMG_INT32 i32BytesThisTime = MIN(i32BufSize-i32ReadyCount , i32NumBytes);
			if (i32BytesThisTime)
			{
				memcpy(pCmdBuffer + i32ReadyCount, pData, (IMG_UINT32)i32BytesThisTime);
				i32ReadyCount += i32BytesThisTime;
				pData = static_cast<const char*>(pData) + i32BytesThisTime;
				i32NumBytes -= i32BytesThisTime;
			}
			if ((i32ReadyCount == i32BufSize) || (!bUseBuffer))
			{
				flushBuffer();
			}
		}
	}
	

	void recvWord(IMG_UINT32 &val){
		recvData(sizeof(IMG_UINT32), &val);
		devif_ntohl(val);
	}
	void sendWord(IMG_UINT32 &val){
		devif_htonl(val);
		sendData(sizeof(IMG_UINT32), &val);
	}
	void recvWord64(IMG_UINT64 &val){
		recvData(sizeof(IMG_UINT64), &val);
		devif_ntohll(val);
	}
	void sendWord64(IMG_UINT64 &val){
		devif_htonll(val);
		sendData(sizeof(IMG_UINT64), &val);
	}

	void enterMutex()
	{
#ifdef MEOS_MUTEX
		threadSafeOldPriority = KRN_raiseIPL();
#endif
#ifdef OSA_MUTEX
        OSA_CritSectLock(hThreadSafeMutex);
#endif
	}

	void exitMutex()
	{
#ifdef MEOS_MUTEX
		KRN_restoreIPL(threadSafeOldPriority);
#endif
#ifdef OSA_MUTEX
        OSA_CritSectUnlock(hThreadSafeMutex);
#endif
	}

	IMG_BOOL bUseBuffer;	// switch for buffer use
	IMG_UINT8* pCmdBuffer; // outgoing buffer
	IMG_INT32 i32ReadyCount; // Bytes in buffer ready to send 
	static const IMG_INT32 i32BufSize = (4<<10);
	IMG_UINT32 ui32TransferMode;
	DeviceInterfaceMaster * psParentDev;
	string sParentName;
	string		sRemoteName;			//!< Device name reported by device
	string		sRemoteVersion;			//!< Version string for device
	IMG_UINT32	ui32RemoteProtocol;		//!< Protocol version of remote device
	IMG_UINT32	ui32DeviceId;			//!< Number used to identify remote device

#ifdef MEOS_MUTEX
	KRN_IPL_T threadSafeOldPriority;
#endif


};

#endif // DEVICE_INTERFACE_MASTER
