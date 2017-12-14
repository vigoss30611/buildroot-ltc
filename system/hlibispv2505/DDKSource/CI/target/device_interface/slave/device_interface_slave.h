/*!
******************************************************************************
 @file   : device_interface_slave.cpp

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
        This file contains the implementation of the DevIfSlave class -
		an abstract base class used to wrap generic devices.


 \n<b>Platform:</b>\n
	     All 

******************************************************************************/
#ifndef DEVIF_SLAVE_H
#define DEVIF_SLAVE_H

#include "device_interface.h"
#include <string.h>
#include <list>
#ifdef WIN32
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <sys/types.h>
#endif
#include <algorithm>

#include "device_interface_message.h"


/* ********************************************************************************
NOTES:

	IDLES

		Introduction
			Ideally IDLE commands should be sent over the device interface to tell
			the slave that some time has passed. In practice many masters do not do
			this, or do not send big enough idles for the slave to do much. This 
			results in very slow slave execution

		AutoIdle
			If the autoIdle time is non-zero, the slave will automatically idle for 
			the autoIdle number of cycles whenever there is not a command waiting 
			on the socket interface. This solves most speed issues, but does make
			the timing non-deterministic, which can cause problems. The exact value 
			of the autoIdle time is not terribly important - 1000 generally works 
			quite well. 

			The master can set the autoIdle over the device interface using the 
			AUTOIDLE command, for example PdumpPlayer sends IDLE commands properly 
			- so it sets autoIdle to zero as it shouldn't be required

			To set the autoIdle time call: 
			
				setAutoIdleCycles(numCycles, allowOverride)
					where
						numCylces is the autoIdle time you want (e.g. 1000)
						allowOverride - set to 0 if you don't want the master to be able
										to override this value

		IdleMult
			When an IDLE command is received over the device interface the value is
			multiplied by IdleMult. This resolves situations where proper IDLES are 
			sent, but because their value is too small the device interface connection
			still limits the speed. This can be useful when the master is pdump player

			To set the IdleMult call: 
				setIdleMultiplier(mult)

	CODESCAPE
		
		If you are using codescape with a csim you will need to have AutoIdle set to 
		a non-zero value (1000 is normally ok). In addition you can't capture 
		binaryInput when using codescape - trying to will cause codescape to loose 
		it's connection.



************************************************************************************ */

class DeviceInterfaceSlave {
public:
	static DeviceInterfaceSlave* createDevifSlave(DeviceInterface& device, 
		char*  szPortNum,
		IMG_BOOL bVerbose,
		IMG_CHAR* sDumpFile = NULL,
		IMG_BOOL bLoop = IMG_FALSE,
		std::string szMasterCommandLine = "",
		std::string szMasterLogFile = "",
		std::string szRevision= "slave"); // Abstract factory

	virtual ~DeviceInterfaceSlave();
	
	void main();
	void initialise();
	IMG_BOOL runCommand();
	virtual int  testRecv(){return 1;}
	
	void setAutoIdleCycles (IMG_UINT32 numCycles, IMG_BOOL bAllowAutoIdleOverride=IMG_TRUE) 
	{
		if(this->bAllowAutoIdleOverride)	
		{
			ui32AutoIdleCycles = numCycles;
			this->bAllowAutoIdleOverride = bAllowAutoIdleOverride;
		}
	}
	void setIdleMultiplier(IMG_UINT32 mult)
	{
		ui32IdleMultiplier = mult;
	}
	IMG_BOOL binaryCaptureEnable(const char* sInputFileName, const char* sOutputFileName);

protected:
	DeviceInterfaceSlave(DeviceInterface& device,
		IMG_BOOL bVerbose,
		IMG_CHAR* sDumpFile = NULL,
		IMG_BOOL bLoop = IMG_FALSE,
		std::string szMasterCommandLine = "",
		std::string szMasterLogFile = "",
		std::string szRevision = "slave");

	IMG_BOOL processCommand(EDeviceInterfaceCommand eCmd, IMG_UINT64 uArg1, IMG_UINT64 UArg2);
	virtual void waitForHost()= 0;
	virtual void recvData()= 0;

	
	void startMaster();
    virtual void setupDevice(IMG_UINT32 ui32StrLen)=0;
	virtual void sendData(IMG_INT32 i32NumBytes, const void* pData)=0;
	void sendWord(IMG_UINT32 ui32Data){
		devif_htonl(ui32Data);
		sendDataBase(sizeof(IMG_UINT32), &ui32Data);
	}
	void sendWord64(IMG_UINT64 ui64Data){
		devif_htonll(ui64Data);
		sendDataBase(sizeof(IMG_UINT64), &ui64Data);
	}
    virtual unsigned int getId()=0;
	/*
	 * Can't name these simply ntohl/htonl as some systems (e.g. Linux)
	 * sometimes define these as macros
	 */
	virtual void devif_ntohl(IMG_UINT32& val)= 0;
	virtual void devif_htonl(IMG_UINT32& val)= 0;
	virtual void devif_ntohll(IMG_UINT64& val)= 0;
	virtual void devif_htonll(IMG_UINT64& val)= 0;
	void autoIdle(IMG_UINT32 uNumCycles){
		device.idle(uNumCycles);
		if (pBinInputFile && bConnected &&!bIdling)
		{
			// Insert an idle into the log of transcations

			IMG_UINT8 myByte = static_cast<IMG_UINT8>(E_DEVIF_IDLE);
			fwrite(&myByte, sizeof(IMG_UINT8), 1, pBinInputFile);

			fflush(pBinInputFile);
			bIdling = IMG_TRUE;
			uIdleCycles = 0;
		}
		uIdleCycles += uNumCycles;
	};
	void flushAutoIdle(){
		if (pBinInputFile && bConnected && bIdling)
		{
			if  (ui32TranferMode== E_DEVIFT_32BIT)
			{
				IMG_UINT32 uTmp = 0;
				devif_htonl(uIdleCycles);
				fwrite(&uIdleCycles, sizeof(IMG_UINT32), 1, pBinInputFile);
				fwrite(&uTmp, sizeof(IMG_UINT32), 1, pBinInputFile);
			}
			else // E_DEVIFT_64BIT
			{
				IMG_UINT64 uTmp64 = 0;
				IMG_UINT64 uNumCycles64 = uIdleCycles;
				devif_htonll(uNumCycles64);
				fwrite(&uNumCycles64, sizeof(IMG_UINT64), 1, pBinInputFile);
				fwrite(&uTmp64,		  sizeof(IMG_UINT64), 1, pBinInputFile);
			}
			fflush(pBinInputFile);
		}
		bIdling = IMG_FALSE;
	}
    
	void getWord64(IMG_UINT64 &ui64Data){
		getData(sizeof(IMG_UINT64), &ui64Data);
		devif_ntohll(ui64Data);
	}

	void getWord(IMG_UINT32 &ui32Data){
		getData(sizeof(IMG_UINT32), &ui32Data);
		devif_ntohl(ui32Data);
	}
    
    void getByte(IMG_UINT8 &ui8Data) {
        getData(sizeof(IMG_UINT8), &ui8Data);
    }

	void getData(IMG_INT32 i32NumBytes, void *ptr){
		void *initial_ptr = ptr;
		IMG_INT32 i32InitialNumBytes = i32NumBytes;
		
		while (i32NumBytes)
		{
			// Copy available data into the target buffer,
			IMG_INT32 iBytesThisTime = std::min(i32NumBytes, i32ReadyCount - i32ReadOffset);
			if (iBytesThisTime)
			{
				bProcessingGetData = IMG_TRUE;
				memcpy(ptr, pCmdBuffer + i32ReadOffset, iBytesThisTime);
				i32NumBytes -= iBytesThisTime;
				i32ReadOffset += iBytesThisTime;
				ptr = static_cast<char*>(ptr) + iBytesThisTime;
			}
			if (i32NumBytes && (i32ReadyCount - i32ReadOffset) == 0)
			{
				recvData();
			}
		}
		bProcessingGetData = IMG_FALSE;
		if (pBinInputFile)
		{
			fwrite(initial_ptr, 1, i32InitialNumBytes, pBinInputFile);
		}
	}
	void sendDataBase(IMG_INT32 i32NumBytes, const void* pData)
	{
		if (pBinOutputFile)
		{
			fwrite(pData, 1, i32NumBytes, pBinOutputFile);
			fflush(pBinOutputFile);
		}
		sendData( i32NumBytes, pData);
	}
	void getCmd(EDeviceInterfaceCommand &eCmd, IMG_UINT64 &uArg1, IMG_UINT64 &uArg2);

	DeviceInterface &device;
	IMG_UINT8* pCmdBuffer; // incomming buffer
	IMG_INT32 i32ReadyCount; // Bytes in buffer ready to read 
	IMG_INT32 i32ReadOffset; // Bytes in buffer ready to read 
	static const IMG_INT32 i32BufSize = (4<<10);
	IMG_BOOL bVerbose;
	IMG_CHAR* sDumpFile;
	FILE* pDumpFile;
	FILE* pBinInputFile;
	FILE* pBinOutputFile;
	IMG_BOOL bLoop;
	IMG_UINT32 ui32TranferMode;
	IMG_UINT32 ui32AutoIdleCycles;
	IMG_BOOL bAllowAutoIdleOverride;
	IMG_UINT32 ui32IdleMultiplier;
	IMG_BOOL bProcessingGetData;
	IMG_BOOL bConnected;
	std::string szMasterCommandLine;
	std::string szMasterLogFile;
	std::string szRevision;
	IMG_BOOL bIdling;
	IMG_UINT32 uIdleCycles;
	
	std::list<DeviceInterfaceSlave*> lpDevIfSlaves;		// List of extra devices which have been added
#ifdef WIN32
	LPPROCESS_INFORMATION lpMasterProcess ;
#else
	pid_t masterProcessPID;
#endif
private:
		DeviceInterfaceSlave& operator=(const DeviceInterfaceSlave &tmp);
};


#endif
