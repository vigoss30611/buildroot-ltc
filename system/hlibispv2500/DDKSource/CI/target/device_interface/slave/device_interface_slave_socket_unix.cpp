/*!
******************************************************************************
 @file   : devif_slave.cpp

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
#include "device_interface_slave_socket_unix.h"
#include "img_defs.h"

#include <errno.h>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <sys/un.h>

#define PATH_DEF_1 "[path]"
#define PATH_DEF_2 "sockpath"


SOCKET CreateListeningSocketUnix(const IMG_CHAR *pszPath)
{
	int	iResult;
    struct sockaddr_un socLocal;
	SOCKET	listeningSock = 0;
    
    socLocal.sun_family = AF_UNIX;
    strcpy(socLocal.sun_path, pszPath);
    
    
	listeningSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listeningSock)
    {
        printf("ERROR: DevIfSlave failed to create socket on path %s: %s\n",pszPath, strerror( errno ) );
        throw IMG_FALSE;
        return 0;
    }

    int len = strlen(socLocal.sun_path) + sizeof(socLocal.sun_family);
	iResult = bind(listeningSock, (struct sockaddr*)&socLocal, len);
	if (SOCKET_ERROR == iResult)
	{
		printf("DevIfSlave failed to bind to path %s, %s\n", pszPath, strerror( errno ) );
        throw IMG_FALSE;
        return 0;
    }
		
	if (SOCKET_ERROR==listen(listeningSock, SOMAXCONN))
	{
        printf("DevIfSlave failed to listen on path %s, %s\n", pszPath, strerror( errno ) );
        throw IMG_FALSE;
        return 0;
    }

	printf("DevIfSlave listening on path %s\n", pszPath);
	return listeningSock;
}


DevIfSlaveUnix::DevIfSlaveUnix(DeviceInterface& device, 
					 const IMG_CHAR *pszPath,
                     IMG_UINT32 ui32Id,
					 IMG_BOOL bVerbose,
					 IMG_CHAR* sDumpFile,
					 IMG_BOOL bLoop,
					 SOCKET extListeningSockFd,
					 DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc,
					 std::string szMasterCommandLine,
					 std::string szMasterLogFile,
					 std::string szRevision)
	: DevIfSlaveSocket(device, bVerbose, sDumpFile, bLoop, extListeningSockFd, pfCBFunc, szMasterCommandLine, szMasterLogFile, szRevision)
    ,sPath_(pszPath)
    ,id_(ui32Id)
    ,subDevId_(ui32Id * 100 + 1)
    ,bExternallySuppliedListeningSocket_(extListeningSockFd==0?IMG_FALSE:IMG_TRUE)    
{}

DevIfSlaveUnix::~DevIfSlaveUnix()
{
	if (!bExternallySuppliedListeningSocket_)
	{
		cleanup();
	}
	else
	{
		if(acceptedsockFd)
		{
			shutdown(acceptedsockFd, SHUT_RDWR);
			close(acceptedsockFd);
            acceptedsockFd = 0;
		}
	}
	unlink(sPath_.c_str());
}


void DevIfSlaveUnix::waitForHost()
{


	if (!bExternallySuppliedListeningSocket_)
	{
		listeningSockFd = CreateListeningSocketUnix(sPath_.c_str());
		printf("DevIfSlave listening on path %s\n", sPath_.c_str());
	}

	if(!szMasterCommandLine.empty())
	{
		FindAndReplace(szMasterCommandLine, PATH_DEF_1, sPath_.c_str());
		FindAndReplace(szMasterCommandLine, PATH_DEF_2, sPath_.c_str());
		startMaster();
	}

	//Poll for connection attempt, and then accept it
	waitToRecv(listeningSockFd);

	acceptedsockFd = accept(listeningSockFd, NULL, NULL);
	if (INVALID_SOCKET==acceptedsockFd)
	{
		printf("ERROR: DevIfSlave failed to accept - errno = %d\n", lastError());
		cleanup();
		throw IMG_FALSE;
	}

	if (pfCallbackFunc)
	{
		DEVIFSLAVE_EVENT_CALLBACK_DATA sCallbackData;

		sCallbackData.ui32Event = DEVIFSLAVE_EVENT_CONACCEPTED;
		pfCallbackFunc(sCallbackData);
	}
}

void DevIfSlaveUnix::cleanup()
{
    if (!bExternallySuppliedListeningSocket_)
    {
        if(listeningSockFd)
        {
            shutdown(listeningSockFd, SHUT_RDWR);
            close(listeningSockFd);
            listeningSockFd = 0;
        }
        if(acceptedsockFd)
        {
            shutdown(acceptedsockFd, SHUT_RDWR);
            close(acceptedsockFd);
            acceptedsockFd = 0;
        }
        if(result) freeaddrinfo(result);
        result = NULL;
    }
}

DeviceInterfaceSlave* DevIfSlaveUnix::newSlaveDevice(DeviceInterface& device, const IMG_CHAR* pszPath)
{
    SOCKET socketFd;
    socketFd = CreateListeningSocketUnix(pszPath);
    if (socketFd == 0)
    {
        printf("ERROR: DevifSlave Cannot create new device\n");
        cleanup();
        throw IMG_TRUE;
        return NULL;
    }
    DevIfSlaveUnix * newDev = new DevIfSlaveUnix(device, pszPath, subDevId_, bVerbose, sDumpFile, bLoop, socketFd);
    return newDev;
}

void DevIfSlaveUnix::setupDevice(IMG_UINT32 ui32StrLen)
{
    IMG_CHAR* pcDevName = IMG_NULL;
    DeviceInterface * pNewDevice;
    DeviceInterfaceSlave * pNewDevIfSlave;
    string newPath = sPath_;
    // Get the RTL basename if sent
    pcDevName = new IMG_CHAR[(ui32StrLen) + 1];
    if (!pcDevName)
    {
        printf("Error: Failed to allocate temp buffers for E_DEVIF_SETUP_DEVICE\n");
        IMG_ASSERT(IMG_FALSE);
    }
    pcDevName[ui32StrLen] = 0;
    getData(ui32StrLen, pcDevName);
    // create the device
    pNewDevice = device.addNewDevice(pcDevName, IMG_FALSE);
    newPath.append("_");
    newPath.append(pcDevName);
    
    // Create new TCP slave with new device
    pNewDevIfSlave = newSlaveDevice(*pNewDevice, newPath.c_str());
    sendWord(subDevId_++);
    pNewDevIfSlave->initialise();
    lpDevIfSlaves.push_back(pNewDevIfSlave);
}

