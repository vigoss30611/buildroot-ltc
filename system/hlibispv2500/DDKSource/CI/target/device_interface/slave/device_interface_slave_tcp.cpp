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
#include "device_interface_slave_tcp.h"
#include "img_defs.h"

#include <errno.h>
#ifndef WIN32
#	include <unistd.h>
#	include <poll.h>
#endif
#include <sstream>

#define DEFAULT_MIN_PORT 4321
#define DEFAULT_MAX_PORT 4385

#define PORT_DEF_1 "[port]"
#define HOST_DEF_1 "[host]"
#define PORT_DEF_2 "portnum"
#define HOST_DEF_2 "hostname"

SOCKET_INFO CreateListeningSocketGetPort(const IMG_CHAR *pszPortNum)
{
	int	iResult;
	struct	addrinfo sAddr;
	struct	addrinfo *psResult;	
	SOCKET	listeningSock = 0;
	unsigned int uiMinPort = 0, uiMaxPort = 0, uiPortNum = 0;
	IMG_CHAR szPortNum[16];
	SOCKET_INFO sSocketInfo;
	
	sSocketInfo.ui32PortNum = 0;
	sSocketInfo.socketFd = 0;

#ifdef WIN32
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("ERROR: WSAStartup failed errno = %d", errno);
		throw IMG_FALSE;
	}
#endif

	memset(&sAddr, '\0', sizeof sAddr);
	sAddr.ai_family = AF_INET;
	sAddr.ai_socktype = SOCK_STREAM;
	sAddr.ai_protocol = IPPROTO_TCP;
	sAddr.ai_flags = AI_PASSIVE;

	if (pszPortNum == IMG_NULL)
	{
		uiMinPort = DEFAULT_MIN_PORT;
		uiMaxPort = DEFAULT_MAX_PORT;
		printf("WARNING: No Port Number defined, using default range of %d-%d\n", uiMinPort, uiMaxPort);
	}
	else
	{
        IMG_CHAR *pszEnd;
        
        uiMinPort = (IMG_UINT)strtol(pszPortNum, &pszEnd, 10);
        if (*pszEnd != '\0')
            uiMaxPort = (IMG_UINT)strtol(pszEnd + 1, NULL, 10);
        else
            uiMaxPort = uiMinPort;
	}
	
	for (uiPortNum = uiMinPort; uiPortNum <= uiMaxPort; uiPortNum++)
	{
		sprintf(szPortNum, "%d", uiPortNum);
		printf("Attempting to connect on port %s\n", szPortNum);
		iResult = getaddrinfo(NULL, szPortNum, &sAddr, &psResult);
		if (iResult != 0 )
		{
			printf("ERROR: DevIfSlave failed to getaddrinfo - %d", iResult );
			throw IMG_FALSE;
		}

		listeningSock = socket(psResult->ai_family, psResult->ai_socktype, psResult->ai_protocol);
		if (INVALID_SOCKET == listeningSock)
		{
			printf("ERROR: DevIfSlave failed to create socket on port %s: %s\n",szPortNum, strerror( errno ) );
			throw IMG_FALSE;
		}

 		int yes=1;
#ifdef WIN32
		int iSocketOption = SO_EXCLUSIVEADDRUSE;
#else
		int iSocketOption = SO_REUSEADDR;
#endif
		// lose the pesky "Address already in use" error message
		if (setsockopt(listeningSock,SOL_SOCKET,iSocketOption,(const char *)&yes,sizeof(int)) == SOCKET_ERROR) {
			printf("DevIfSlave cannot open port %s for reuse ...\n", szPortNum);
			if ( uiPortNum == uiMaxPort )
			{
				printf("ERROR: DevIfSlave cannot open any ports within this range for use %s\n", pszPortNum);
				throw IMG_FALSE;
			}
  			continue;
		}

		iResult = bind(listeningSock, psResult->ai_addr, (int)psResult->ai_addrlen);
		if (SOCKET_ERROR == iResult)
		{
			printf("DevIfSlave failed to bind to port %s ...\n", szPortNum);
			if ( uiPortNum == uiMaxPort )
			{
				printf("ERROR: DevIfSlave failed to bind to any ports within the given range\n");
				throw IMG_FALSE;
			}
  			continue;
		}

		if (SOCKET_ERROR==listen(listeningSock, SOMAXCONN))
		{
			printf("DevIfSlave failed to listen on port %d ...\n", uiPortNum);
			if ( uiPortNum == uiMaxPort )
			{
				printf("ERROR: DevIfSlave failed to listen on any ports within this range %s\n", pszPortNum);
				throw IMG_FALSE;
			}
  			continue;
		}
		break;
	}

	printf("DevIfSlave listening on port %d\n", uiPortNum);
	sSocketInfo.socketFd = listeningSock;
	sSocketInfo.ui32PortNum = uiPortNum;
	return sSocketInfo;
}

SOCKET CreateListeningSocket(const IMG_CHAR *pszPortNum)
{
	SOCKET_INFO sSocketInfo = CreateListeningSocketGetPort(pszPortNum);
	return sSocketInfo.socketFd;
}

DevIfSlaveTCP::DevIfSlaveTCP(DeviceInterface& device, 
					 const IMG_CHAR *pszPortNum,
					 IMG_BOOL bVerbose,
					 IMG_CHAR* sDumpFile,
					 IMG_BOOL bLoop,
					 SOCKET extListeningSockFd,
					 DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc,
					 std::string szMasterCommandLine,
					 std::string szMasterLogFile,
					 std::string szRevision)
	: DevIfSlaveSocket(device, bVerbose, sDumpFile, bLoop, extListeningSockFd, pfCBFunc, szMasterCommandLine, szMasterLogFile, szRevision)
    , uiPortNum(0)
    ,bExternallySuppliedListeningSocket(extListeningSockFd==0?IMG_FALSE:IMG_TRUE)
{
	sPortRange = pszPortNum;
}

DevIfSlaveTCP::~DevIfSlaveTCP()
{
	if (!bExternallySuppliedListeningSocket)
	{
		cleanup();
#ifdef WIN32
		WSACleanup();
#endif
	}
	else
	{
#ifdef WIN32
		if(acceptedsockFd) closesocket(acceptedsockFd);
#else
		if(acceptedsockFd)
		{
			shutdown(acceptedsockFd, SHUT_RDWR);
			close(acceptedsockFd);
		}
#endif
	}
}


void DevIfSlaveTCP::waitForHost()
{


	char szHostName[256];

	if (!bExternallySuppliedListeningSocket)
	{
		SOCKET_INFO sSocketInfo = CreateListeningSocketGetPort(sPortRange.c_str());
		listeningSockFd = sSocketInfo.socketFd;
		uiPortNum = sSocketInfo.ui32PortNum;
		printf("DevIfSlave listening on port %d\n", uiPortNum);
	}

	if(gethostname(szHostName, 256))
	{
		printf("ERROR: DevIfSlave failed to get hostname - errno = %d\n", lastError());
		cleanup();
		throw IMG_FALSE;
	}


	if(!szMasterCommandLine.empty())
	{
		std::ostringstream szPortNum;
		szPortNum << uiPortNum;

		FindAndReplace(szMasterCommandLine, PORT_DEF_1, szPortNum.str());
		FindAndReplace(szMasterCommandLine, PORT_DEF_2, szPortNum.str());
		FindAndReplace(szMasterCommandLine, HOST_DEF_1, szHostName);
		FindAndReplace(szMasterCommandLine, HOST_DEF_2, szHostName);

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

void DevIfSlaveTCP::cleanup()
{
    if (!bExternallySuppliedListeningSocket)
    {
#ifdef WIN32
    
        if(acceptedsockFd) 
        {
            closesocket(acceptedsockFd);
            acceptedsockFd = 0;
        }
        if(result) freeaddrinfo(result);
        //Note WSACleanup() called in destructor
#else
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
#endif
    }
}

DeviceInterfaceSlave* DevIfSlaveTCP::newSlaveDevice(DeviceInterface& device, IMG_UINT32 &uiPortNum)
{
    SOCKET_INFO sSocketInfo;
    sSocketInfo = CreateListeningSocketGetPort(sPortRange.c_str());
    if (sSocketInfo.socketFd == 0)
    {
        printf("ERROR: DevifSlave Cannot create new device\n");
        cleanup();
        throw IMG_TRUE;
    }
    uiPortNum = sSocketInfo.ui32PortNum;
    DevIfSlaveTCP * newDev = new DevIfSlaveTCP(device, sPortRange.c_str(), bVerbose, sDumpFile, bLoop, sSocketInfo.socketFd);
    newDev->uiPortNum = sSocketInfo.ui32PortNum;
    return newDev;
}

void DevIfSlaveTCP::setupDevice(IMG_UINT32 ui32StrLen)
{
    IMG_CHAR* pcDevName = IMG_NULL;
    IMG_UINT32 ui32PortNum;
    DeviceInterface * pNewDevice;
    DeviceInterfaceSlave * pNewDevIfSlave;
    IMG_CHAR* pszPortRange = strdup(sPortRange.c_str());
    IMG_CHAR * pcPortSeperator = strchr (pszPortRange, '-');
    IMG_UINT32 uiMaxPort = atoi(++pcPortSeperator);
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

    // Create new TCP slave with new device
    pNewDevIfSlave = newSlaveDevice(*pNewDevice, ui32PortNum);
    sprintf(pszPortRange, "%d-%d", ui32PortNum+1, uiMaxPort);
    sPortRange = pszPortRange;
    free(pszPortRange);
    sendWord(ui32PortNum);
    pNewDevIfSlave->initialise();
    lpDevIfSlaves.push_back(pNewDevIfSlave);
}

