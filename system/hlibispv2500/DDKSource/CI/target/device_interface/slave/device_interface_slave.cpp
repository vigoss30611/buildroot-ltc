/*!
******************************************************************************
 @file   : device_interface_slave.cpp

 @brief  

 @Author Imagination Technologies

 @date   15/09/2005
 
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
        this file contains the implementation of a devif_slave - a moudule that listens over tcpip and passes 
		transactions on to a DeviceInterface instance


 \n<b>Platform:</b>\n
	     PC 

******************************************************************************/
#include "device_interface_slave_tcp.h"
#include "device_interface_slave_filereader.h"
#include "img_defs.h"
#include "img_errors.h"
#include <algorithm> 

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#ifndef __APPLE__
#include <wait.h>
#endif
#endif

IMG_BOOL g_bConnected = IMG_FALSE;

// Abstract factory
DeviceInterfaceSlave* DeviceInterfaceSlave::createDevifSlave(DeviceInterface& device,
		 char*  szPortNum,
		IMG_BOOL bVerbose,
		IMG_CHAR* sDumpFile,
		IMG_BOOL bLoop,
		std::string szMasterCommandLine,
		std::string szMasterLogFile,
		std::string szRevision)
{	
	return new DevIfSlaveTCP(device, szPortNum, bVerbose, sDumpFile, bLoop, 0, 0, szMasterCommandLine, szMasterLogFile, szRevision);
}

DeviceInterfaceSlave::DeviceInterfaceSlave(DeviceInterface& device,
					   IMG_BOOL bVerbose,
					   IMG_CHAR* sDumpFile,
					   IMG_BOOL bLoop,
					   std::string szMasterCommandLine,
					   std::string szMasterLogFile,
					   std::string szRevision)
	: device(device)
	, i32ReadyCount(0)
	, i32ReadOffset(0)
	, bVerbose(bVerbose)
	, sDumpFile(sDumpFile)
	, pDumpFile(0)
	, pBinInputFile(0)
	, pBinOutputFile(0)
	, bLoop(bLoop)
    , ui32TranferMode(E_DEVIFT_32BIT)
	, ui32AutoIdleCycles(0)
	, bAllowAutoIdleOverride(IMG_TRUE)
    , ui32IdleMultiplier(1)
    , bProcessingGetData(IMG_FALSE)
    , bConnected(IMG_FALSE)
	, szMasterCommandLine(szMasterCommandLine)
	, szMasterLogFile(szMasterLogFile)
	, szRevision(szRevision)
	, bIdling(IMG_FALSE)
	, uIdleCycles(0)
#ifdef WIN32
	, lpMasterProcess(IMG_NULL)
#else
	, masterProcessPID(0)
#endif
{
   pCmdBuffer = new IMG_UINT8[i32BufSize];
   if (pCmdBuffer == NULL)
   {
		printf("Failed to create DeviceInterfaceSlave command buffer");
		throw IMG_FALSE;	// Fatal, terminate application
   }
}

DeviceInterfaceSlave::~DeviceInterfaceSlave()
{
	if (pBinInputFile) fclose(pBinInputFile);
	if (pBinOutputFile) fclose(pBinOutputFile);	
	if (lpDevIfSlaves.empty() == IMG_FALSE)
	{
		std::list<DeviceInterfaceSlave*>::iterator DeviceIter;
		for(DeviceIter = lpDevIfSlaves.begin(); DeviceIter != lpDevIfSlaves.end(); DeviceIter++)
		{
			delete (*DeviceIter);
		}
	}
#ifdef WIN32
	if(lpMasterProcess) free(lpMasterProcess);
#else
	if(masterProcessPID)
	{
		printf("Waiting for master process to complete before exiting...");
		if(waitpid(masterProcessPID, NULL, 0)!=masterProcessPID)
		{
			printf("ERROR: Unable to wait for master process to complete. Error %d\n", errno);
		}
		printf(" done\n");
	}
#endif
}


void DeviceInterfaceSlave::main(){

	do
	{
		initialise();
		while (runCommand());
	}while(bLoop);
}


void DeviceInterfaceSlave::initialise(){

	waitForHost();
	device.initialise();
	printf("Accepted connection\n");
	if (sDumpFile)
	{
		pDumpFile = fopen(sDumpFile, "w");
		if (pDumpFile == NULL)
		{
				printf("Failed to create file for dumping %s\n", sDumpFile);
				throw IMG_FALSE; // Fatal, terminate application;
		}
	}
	bConnected = g_bConnected = IMG_TRUE;
}

IMG_BOOL DeviceInterfaceSlave::binaryCaptureEnable(const char* sInputFileName, const char* sOutputFileName)
{
	if (!(sInputFileName && sOutputFileName)){
		printf("Bad filenames specified for logging devif slave data\n");
		return false;
	}
	printf("Opening files for logging devif transactions %s and %s\n",  sInputFileName, sOutputFileName);
	pBinInputFile = fopen(sInputFileName, "wb");
	pBinOutputFile = fopen(sOutputFileName, "wb");
	if (!(pBinInputFile && pBinOutputFile)){
		printf("Failed to open devif slave binary capture files\n");
		return false;		
	}
	return true;
}

IMG_BOOL DeviceInterfaceSlave::runCommand()
{
	EDeviceInterfaceCommand eCmd;
	IMG_UINT64 arg1;
	IMG_UINT64 arg2;
	getCmd(eCmd, arg1, arg2);
	return processCommand(eCmd, arg1, arg2);
}

IMG_BOOL DeviceInterfaceSlave::processCommand(EDeviceInterfaceCommand eCmd, IMG_UINT64 arg1, IMG_UINT64 arg2)
{
	IMG_BOOL run = IMG_TRUE;
	IMG_UINT32 ui32rval;
	IMG_UINT64 ui64rval;
	
	switch(eCmd)
	{
	case E_DEVIF_READMEM:
		ui32rval = device.readMemory(arg1);
		if (bVerbose) printf("E_DEVIF_READMEM %016" IMG_I64PR "x %08x\n", arg1, ui32rval);
		if (pDumpFile) fprintf(pDumpFile, "RM %016" IMG_I64PR "x %08x\n",arg1,ui32rval);
		sendWord(ui32rval);
		break;
	case E_DEVIF_READMEM_64:
		ui64rval = device.readMemory64(arg1);
		if (bVerbose) printf("E_DEVIF_READMEM %016" IMG_I64PR "x %016" IMG_I64PR "x\n", arg1, ui64rval);
		if (pDumpFile) fprintf(pDumpFile, "RM64 %016" IMG_I64PR "x %016" IMG_I64PR "x\n",arg1,ui64rval);
		sendWord64(ui64rval);
		break;
	case E_DEVIF_READREG:
		ui32rval = device.readRegister(arg1);
		if (bVerbose) printf("E_DEVIF_READREG %016" IMG_I64PR "x %08x\n", arg1, ui32rval);
		if (pDumpFile) fprintf(pDumpFile, "RR %016" IMG_I64PR "x %08x\n",arg1,ui32rval);
		sendWord(ui32rval);
		break;
	case E_DEVIF_READREG_64:
		ui64rval = device.readRegister64(arg1);
		if (bVerbose) printf("E_DEVIF_READREG %016" IMG_I64PR "x %016" IMG_I64PR "x\n", arg1, ui64rval);
		if (pDumpFile) fprintf(pDumpFile, "RR64 %016" IMG_I64PR "x %016" IMG_I64PR "x\n",arg1, ui64rval);
		sendWord64(ui64rval);
		break;
	case E_DEVIF_WRITEMEM:
		if (bVerbose) printf("E_DEVIF_WRITEMEM %016" IMG_I64PR "x %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
		if (pDumpFile) fprintf(pDumpFile, "WM %016" IMG_I64PR "x %08x\n",arg1,IMG_UINT64_TO_UINT32(arg2));
		device.writeMemory(arg1, IMG_UINT64_TO_UINT32(arg2));
		break;
	case E_DEVIF_WRITEMEM_64:
		if (bVerbose) printf("E_DEVIF_WRITEMEM %016" IMG_I64PR "x %016" IMG_I64PR "x\n", arg1, arg2);
		if (pDumpFile) fprintf(pDumpFile, "WM64 %016" IMG_I64PR "x %016" IMG_I64PR "x\n",arg1,arg2);
		device.writeMemory64(arg1, arg2);
		break;
	case E_DEVIF_WRITEREG:
		if (bVerbose) printf("E_DEVIF_WRITEREG %016" IMG_I64PR "x %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
		if (pDumpFile) fprintf(pDumpFile, "WR %016" IMG_I64PR "x %08x\n",arg1,IMG_UINT64_TO_UINT32(arg2));
		device.writeRegister(arg1, IMG_UINT64_TO_UINT32(arg2));
		break;
	case E_DEVIF_WRITEREG_64:
		if (bVerbose) printf("E_DEVIF_WRITEREG %016" IMG_I64PR "x %016" IMG_I64PR "x\n", arg1, arg2);
		if (pDumpFile) fprintf(pDumpFile, "WR64 %016" IMG_I64PR "x %016" IMG_I64PR "x\n",arg1,arg2);
		device.writeRegister64(arg1, arg2);
		break;
	case E_DEVIF_COPYHOSTTODEV:
		{
			if (bVerbose) printf("E_DEVIF_COPYHOSTTODEV %016" IMG_I64PR "X %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
			IMG_UINT8* buffer = new IMG_UINT8[(IMG_UINTPTR)arg2];
			if (!buffer)
			{
				printf("Error: Failed to allocate temp buffer for E_DEVIF_COPYHOSTTODEV\n");
				throw IMG_TRUE; 
			}
			if (pDumpFile) fprintf(pDumpFile, "H2D %016" IMG_I64PR "x %08x\n",arg1, IMG_UINT64_TO_UINT32(arg2));
			getData(IMG_UINT64_TO_UINT32(arg2), buffer);
			device.copyHostToDevice(buffer, arg1,  IMG_UINT64_TO_UINT32(arg2));
			delete [] buffer;
		}
		break;
	case E_DEVIF_LOADREG:
		{
			if (bVerbose) printf("E_DEVIF_LOADREG %08x %08x\n", IMG_UINT64_TO_UINT32(arg1), IMG_UINT64_TO_UINT32(arg2));
			IMG_UINT8* buffer = new IMG_UINT8[IMG_UINT64_TO_UINT32(arg2) * 4];
			if (!buffer)
			{
				printf("Error: Failed to allocate temp buffer for E_DEVIF_LOADREG\n");
				throw IMG_TRUE;
			}
			if (pDumpFile) fprintf(pDumpFile, "H2D %08x %08x\n",IMG_UINT64_TO_UINT32(arg1), IMG_UINT64_TO_UINT32(arg2));
			getData(IMG_UINT64_TO_UINT32(arg2) * 4, buffer);
			device.loadRegister(buffer, IMG_UINT64_TO_UINT32(arg1),  IMG_UINT64_TO_UINT32(arg2));
			delete [] buffer;
		}
		break;
	case E_DEVIF_COPYDEVTOHOST:
		{
			if (bVerbose) printf("E_DEVIF_COPYDEVTOHOST %016" IMG_I64PR "X %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
			IMG_UINT8* buffer = new IMG_UINT8[(IMG_UINTPTR)arg2];
			if (!buffer)
			{
				printf("Error: Failed to allocate temp buffer for E_DEVIF_COPYHOSTTODEV\n");
				throw IMG_TRUE;
			}
			if (pDumpFile) fprintf(pDumpFile, "D2H %016" IMG_I64PR "x %08x\n",arg1, IMG_UINT64_TO_UINT32(arg2));
			device.copyDeviceToHost(arg1, buffer, IMG_UINT64_TO_UINT32(arg2));
			sendDataBase(IMG_UINT64_TO_UINT32(arg2), buffer);
			delete [] buffer;
		}
		break;
    case E_DEVIF_POLLREG:
        {
            IMG_UINT32 ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount;
            IMG_UINT8 ui8CmpOpType;
            
            getByte(ui8CmpOpType);
            getWord(ui32ReqVal);
            getWord(ui32EnableMask);
            getWord(ui32PollCount);
            getWord(ui32CycleCount);
            
            ui32rval = device.pollRegister(arg1,
                (DeviceInterface::CompareOperationType)ui8CmpOpType, ui32ReqVal,
                ui32EnableMask, ui32PollCount, ui32CycleCount);

		    if (bVerbose)
            {
                printf("E_DEVIF_POLLREG %016" IMG_I64PR "x %02x %08x %08x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    if (pDumpFile)
            {
                fprintf(pDumpFile, "PR %016" IMG_I64PR "x %02x %08x %08x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    sendWord(ui32rval);
		}
        break;
    case E_DEVIF_POLLREG_64:
        {
            IMG_UINT32 ui32PollCount, ui32CycleCount;
            IMG_UINT64 ui64ReqVal, ui64EnableMask;
            IMG_UINT8 ui8CmpOpType;
            
            getByte(ui8CmpOpType);
            getWord64(ui64ReqVal);
            getWord64(ui64EnableMask);
            getWord(ui32PollCount);
            getWord(ui32CycleCount);
            
            ui32rval = device.pollRegister(arg1,
                (DeviceInterface::CompareOperationType)ui8CmpOpType, ui64ReqVal,
                ui64EnableMask, ui32PollCount, ui32CycleCount);

		    if (bVerbose)
            {
                printf("E_DEVIF_POLLREG_64 %016" IMG_I64PR "x %02x %016" IMG_I64PR "x %016" IMG_I64PR "x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui64ReqVal, ui64EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    if (pDumpFile)
            {
                fprintf(pDumpFile, "PR64 %016" IMG_I64PR "x %02x %016" IMG_I64PR "x %016" IMG_I64PR "x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui64ReqVal, ui64EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    sendWord(ui32rval);
		}
        break;
    case E_DEVIF_POLLMEM:
        {
            IMG_UINT32 ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount;
            IMG_UINT8 ui8CmpOpType;
            
            getByte(ui8CmpOpType);
            getWord(ui32ReqVal);
            getWord(ui32EnableMask);
            getWord(ui32PollCount);
            getWord(ui32CycleCount);
            
            ui32rval = device.pollMemory(arg1,
                (DeviceInterface::CompareOperationType)ui8CmpOpType, ui32ReqVal,
                ui32EnableMask, ui32PollCount, ui32CycleCount);

		    if (bVerbose)
            {
                printf("E_DEVIF_POLLMEM %016" IMG_I64PR "x %02x %08x %08x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    if (pDumpFile)
            {
                fprintf(pDumpFile, "PM %016" IMG_I64PR "x %02x %08x %08x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui32ReqVal, ui32EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    sendWord(ui32rval);
		}
        break;
    case E_DEVIF_POLLMEM_64:
        {
            IMG_UINT32 ui32PollCount, ui32CycleCount;
            IMG_UINT64 ui64ReqVal, ui64EnableMask;
            IMG_UINT8 ui8CmpOpType;
            
            getByte(ui8CmpOpType);
            getWord64(ui64ReqVal);
            getWord64(ui64EnableMask);
            getWord(ui32PollCount);
            getWord(ui32CycleCount);
            
            ui32rval = device.pollMemory(arg1,
                (DeviceInterface::CompareOperationType)ui8CmpOpType, ui64ReqVal,
                ui64EnableMask, ui32PollCount, ui32CycleCount);

		    if (bVerbose)
            {
                printf("E_DEVIF_POLLMEM_64 %016" IMG_I64PR "x %02x %016" IMG_I64PR "x %016" IMG_I64PR "x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui64ReqVal, ui64EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    if (pDumpFile)
            {
                fprintf(pDumpFile, "PM64 %016" IMG_I64PR "x %02x %016" IMG_I64PR "x %016" IMG_I64PR "x %08x %08x %08x\n",
                    arg1, ui8CmpOpType, ui64ReqVal, ui64EnableMask, ui32PollCount, ui32CycleCount, ui32rval);
            }
            
		    sendWord(ui32rval);
		}
        break;
	case E_DEVIF_WRITESLAVE:
		if (bVerbose) printf("E_DEVIF_WRITESLAVE %08x %08x\n", IMG_UINT64_TO_UINT32(arg1), IMG_UINT64_TO_UINT32(arg2));
		printf("DeviceInterfaceSlave - Unsupported command E_DEVIF_WRITESLAVE\n");
		throw IMG_TRUE;
		break;
	case E_DEVIF_ALLOC:
		if (bVerbose) printf("E_DEVIF_ALLOC %016" IMG_I64PR "x %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
		if (pDumpFile) fprintf(pDumpFile, "ALLOC %016" IMG_I64PR "x %08x\n",arg1, IMG_UINT64_TO_UINT32(arg2));
		device.allocMem(arg1, IMG_UINT64_TO_UINT32(arg2));
		break;
	case E_DEVIF_ALLOC_SLAVE:
		IMG_UINT64 ui64Address;
		if (bVerbose) printf("E_DEVIF_ALLOC_SLAVE %016" IMG_I64PR "x %08x\n", arg1, IMG_UINT64_TO_UINT32(arg2));
		if (pDumpFile) fprintf(pDumpFile, "ALLOC_SLAVE %016" IMG_I64PR "x %08x\n",arg1, IMG_UINT64_TO_UINT32(arg2));
		ui64Address = device.allocMemOnSlave(arg1, IMG_UINT64_TO_UINT32(arg2));
		sendWord64(ui64Address);
		break;
	case E_DEVIF_FREE:
		if (bVerbose) printf("E_DEVIF_FREE %016" IMG_I64PR "X\n", arg1);
		if (pDumpFile) fprintf(pDumpFile, "FREE %016" IMG_I64PR "x\n",arg1);
		device.freeMem(arg1);
		break;
	case E_DEVIF_IDLE:
		if (bVerbose) printf("E_DEVIF_IDLE %08x\n", IMG_UINT64_TO_UINT32(arg1));
		if (pDumpFile) fprintf(pDumpFile, "ID %08x\n",IMG_UINT64_TO_UINT32(arg1));
		device.idle(IMG_UINT64_TO_UINT32(std::min(arg1*ui32IdleMultiplier, (IMG_UINT64)0xffffffff)));
		break;
	case E_DEVIF_AUTOIDLE:
		if (bVerbose) printf("E_DEVIF_AUTOIDLE %08x\n", IMG_UINT64_TO_UINT32(arg1));
		if (pDumpFile) fprintf(pDumpFile, "AUTOIDLE %x\n",IMG_UINT64_TO_UINT32(arg1));
		setAutoIdleCycles(IMG_UINT64_TO_UINT32(arg1));
		device.setAutoIdle(IMG_UINT64_TO_UINT32(arg1));
		break;
	case E_DEVIF_SEND_COMMENT:
		{
			if (bVerbose) printf("E_DEVIF_SEND_COMMENT, %08x\n", IMG_UINT64_TO_UINT32(arg1));
			if (pDumpFile) fprintf(pDumpFile, "COMMENT %08x\n",IMG_UINT64_TO_UINT32(arg1));
			IMG_CHAR* buffer = new IMG_CHAR[IMG_UINT64_TO_UINT32(arg1) + 1];
			if (!buffer)
			{
				printf("Error: Failed to allocate temp buffer for E_DEVIF_SEND_COMMENT\n");
				abort();
			}
			buffer[arg1] = 0;
			getData(IMG_UINT64_TO_UINT32(arg1), buffer);
			device.sendComment(buffer);
			delete [] buffer;
			break;
		}
	case E_DEVIF_PRINT:
		{
			if (bVerbose) printf("E_DEVIF_PRINT, %08x\n", IMG_UINT64_TO_UINT32(arg1));
			IMG_CHAR* buffer = new IMG_CHAR[IMG_UINT64_TO_UINT32(arg1) + 1];
			if (!buffer)
			{
				printf("Error: Failed to allocate temp buffer for E_DEVIF_PRINT\n");
				abort();
			}
			buffer[arg1] = 0;
			getData(IMG_UINT64_TO_UINT32(arg1), buffer);
			device.devicePrint(buffer);
			delete [] buffer;
			break;
		}
	case E_DEVIF_GET_DEV_TIME:
		IMG_UINT64 ui64Time;
		if (bVerbose) printf("E_DEVIF_GET_DEV_TIME\n");
		if (device.getDeviceTime(&ui64Time) == IMG_SUCCESS)
			sendWord64(ui64Time);
		else
			sendWord64(DEVIF_INVALID_TIME);
		break;
	case E_DEVIF_TRANSFER_MODE:
		if (bVerbose) printf("E_DEVIF_TRANSFER_MODE %08x\n", IMG_UINT64_TO_UINT32(arg1));
		ui32TranferMode = IMG_UINT64_TO_UINT32(arg1);
		break;
	case E_DEVIF_GET_TAG:
		if (bVerbose) printf("E_DEVIF_GET_TAG %08x\n", IMG_UINT64_TO_UINT32(arg1));
		sendDataBase((IMG_INT32)szRevision.length(), szRevision.c_str());
		sendDataBase(1, ""); // Null terminate
		break;
	case E_DEVIF_GET_DEVICENAME:
		if (bVerbose) printf("E_DEVIF_GET_DEVICENAME %08x\n", IMG_UINT64_TO_UINT32(arg1));
		sendDataBase((IMG_INT32)(device.getName().length() + 1), device.getName().c_str());
		break;
	case E_DEVIF_GET_DEVIF_VER:
		if (bVerbose) printf("E_DEVIF_GET_DEVIF_VER %08x\n", IMG_UINT64_TO_UINT32(arg1));
		ui32rval=DEVIF_PROTOCOL_VERSION;
		sendWord(ui32rval); // Null terminate
		break;
	case E_DEVIF_END:
		if (bVerbose) printf("E_DEVIF_END\n");
		if (pDumpFile) fprintf(pDumpFile, "END\n"); 
		printf("DeviceInterfaceSlave - End command received\n");
		run = IMG_FALSE; //goto END;
		break;
	case E_DEVIF_SETUP_DEVICE:
		// Get the Device Name
		if (bVerbose) printf("E_DEVIF_SETUP_DEVICE\n");
        if (arg1 > 0)
        	setupDevice(IMG_UINT64_TO_UINT32(arg1));
		break;
	case E_DEVIF_USE_DEVICE:
		{
			if (bVerbose) printf("E_DEVIF_USE_DEVICE\n");
			std::list<DeviceInterfaceSlave*>::iterator DeviceIter;
			IMG_BOOL bFoundDev = IMG_FALSE;
			if (lpDevIfSlaves.empty() != IMG_FALSE)
			{
				printf("ERROR: E_DEVIF_USE_DEVICE, device id %d is not know\n", IMG_UINT64_TO_UINT32(arg1)); 
				IMG_ASSERT(IMG_FALSE);
			}
			for(DeviceIter = lpDevIfSlaves.begin(); DeviceIter != lpDevIfSlaves.end(); DeviceIter++)
			{
				// Look for the appropriate Device
				if ((*DeviceIter)->getId() == arg1)
				{
					(*DeviceIter)->runCommand();
					bFoundDev = IMG_TRUE;
					break;
				}
			}
			IMG_ASSERT(bFoundDev == IMG_TRUE);
			
		}
		break;
	case E_DEVIF_SET_BASENAMES:
		if (bVerbose) printf("E_DEVIF_SET_BASENAMES\n");
		{
			IMG_CHAR * regName = new IMG_CHAR[IMG_UINT64_TO_UINT32(arg1) + 1];
			IMG_CHAR * memName = new IMG_CHAR[IMG_UINT64_TO_UINT32(arg2) + 1];
			regName[arg1] = 0;
			memName[arg2] = 0;
			getData(IMG_UINT64_TO_UINT32(arg1), regName);
			getData(IMG_UINT64_TO_UINT32(arg2), memName);
			device.setBaseNames(regName, memName);
			delete [] regName;
			delete [] memName;
		}
		break;
	default:
		printf("DeviceInterfaceSlave - Unknown command received - %d\n", eCmd);
		throw IMG_TRUE;
	}
		 // Cmd(1 byte), Data(4 byte), Data(4Byte)
	return run;
}

void DeviceInterfaceSlave::getCmd(EDeviceInterfaceCommand &eCmd, IMG_UINT64 &uArg1, IMG_UINT64 &uArg2)
{
		IMG_UINT8 myByte;
		getData(sizeof(IMG_UINT8), &myByte);
			eCmd = static_cast<EDeviceInterfaceCommand>(myByte);
		IMG_UINT32 ui32Arg1, ui32Arg2;
		switch (ui32TranferMode)
		{
		case E_DEVIFT_32BIT:
			getWord(ui32Arg1);
			getWord(ui32Arg2);
			uArg1 = ui32Arg1;
			uArg2 = ui32Arg2;
		break;
		case E_DEVIFT_64BIT:
			getWord64(uArg1);
			getWord64(uArg2);
			break;
		default:
			printf("ERROR: DEVIF Unknown Message Type %08X\n", eCmd);
			IMG_ASSERT(IMG_FALSE);
		}
}




using namespace std;
#ifndef WIN32	
	void fChildDied(int signal);
#endif

void DeviceInterfaceSlave::startMaster(){

	if(!szMasterCommandLine.empty())
	{

		printf("\nLaunching DevIf Master with command line:\n%s\n\n", szMasterCommandLine.c_str());
		if(!szMasterLogFile.empty()) printf("Logging DevIf Master transcript to %s\n", szMasterLogFile.c_str());

#ifdef WIN32
		HANDLE hOutput;
		SECURITY_ATTRIBUTES SecurityDesc;
		memset(&SecurityDesc, 0, sizeof(SecurityDesc));
		SecurityDesc.bInheritHandle = IMG_TRUE;
		LPSTARTUPINFO lpStartupInfo = (LPSTARTUPINFO) malloc(sizeof(*lpStartupInfo));
		GetStartupInfo(lpStartupInfo);
		lpStartupInfo->lpTitle = NULL;
		if(!szMasterLogFile.empty()) 
		{
			hOutput = CreateFile(szMasterLogFile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,&SecurityDesc,CREATE_ALWAYS,NULL,NULL);
			lpStartupInfo->hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			lpStartupInfo->hStdOutput = hOutput;
			lpStartupInfo->hStdError = hOutput;
			lpStartupInfo->dwFlags |= STARTF_USESTDHANDLES;

		}

		lpMasterProcess = (LPPROCESS_INFORMATION) malloc(sizeof(*lpMasterProcess));

		CreateProcess(NULL, (LPSTR)(szMasterCommandLine.c_str()),NULL, NULL, true, NULL, NULL, NULL, lpStartupInfo, lpMasterProcess);


#else

		/* We are going to try and run master, so fork program */
		masterProcessPID = fork();
		if (masterProcessPID > 0)
		{
			// Child Process started, this is the parent process
		    IMG_ASSERT( signal(SIGCHLD, fChildDied) != SIG_ERR );
		}
		else if(masterProcessPID < 0)
		{
			// Child Process not started
			printf("ERROR: Unable to start master process. Error %d\n", errno);
			IMG_ASSERT(IMG_FALSE);
		}
		else
		{
			// Only the child gets here
			if(!szMasterLogFile.empty())
			{
				close(2);	//close stderr
				freopen(szMasterLogFile.c_str(), "w", stdout);	// redirect stdout to log file
				dup(1);		//dup stdout, so stderr now points to stdout
			}
			system(szMasterCommandLine.c_str());
			if(!szMasterLogFile.empty())
			{
			  fclose(stderr);
			  fclose(stdout);
			}
			// this is the child process - so we can now exit
			exit(0);
		}
#endif

	}
}

#ifndef WIN32
void fChildDied(int signal)
{
	IMG_ASSERT(signal == SIGCHLD);
    
	if (g_bConnected == IMG_FALSE)
	{
		printf("ERROR: External App has exited before connection has been established, killing simulator ...\n");
		fflush(stdout);
		IMG_ASSERT(IMG_FALSE);
	}

}
#endif


