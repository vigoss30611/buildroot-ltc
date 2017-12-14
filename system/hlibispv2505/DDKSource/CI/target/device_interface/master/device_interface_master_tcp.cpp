/******************************************************************************

 @File         device_interface_master_winsock.cpp

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  Windows specific implementation the device interface master class.

******************************************************************************/
#include "device_interface_master_tcp.h"
#include "img_defs.h"

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#endif

#ifdef WINCE
	#include <wince_env.h>	
	#include <wince_thread.h>
#endif
DeviceInterfaceMasterTCP::DeviceInterfaceMasterTCP(std::string name, const char* szPortNum, const char* szAddress, bool useBuffer, std::string parentName)
	: DeviceInterfaceMasterSocket(name, useBuffer, parentName)
	, pszPortNum(IMG_STRDUP(szPortNum))
{
	setupAddress(szAddress);
	ui32DeviceId = atoi(szPortNum);
}

DeviceInterfaceMasterTCP::~DeviceInterfaceMasterTCP()
{
	IMG_FREE(pszPortNum);
	IMG_FREE(szDestAddress);
}

void DeviceInterfaceMasterTCP::setupAddress(const char* szAddress)
{
#ifdef WIN32
    WSADATA wsaData;
#endif
    
    if (szAddress != NULL)
    {
        szDestAddress = IMG_STRDUP(szAddress);
    }
    else
    {
        szDestAddress = NULL;
    }
#ifdef WIN32
	if(WSAStartup(MAKEWORD(1,1), &wsaData) != 0)
	{
		#ifdef WINCE
			printf("ERROR: WSAStartup failed errno = %d", GetLastError());
		#else
		printf("ERROR: WSAStartup failed errno = %d", errno);
		#endif
		abort();
	}
#endif
}


void DeviceInterfaceMasterTCP::initialise(void)
{
	int iResult;
	struct addrinfo *result;

	if (!bInitialised) {
		DeviceInterfaceMaster::initialise();
		bInitialised = true;

		connectingSockFd = (int)socket(PF_INET, SOCK_STREAM, 0);
		if (connectingSockFd==-1)
		{
			printf("ERROR: DeviceInterfaceMasterTCP failed to open socket errno = %d", lastError());
			abort();
		}
		memset(&myAddr, '\0', sizeof myAddr);
		myAddr.ai_family = PF_INET;//AF_UNSPEC;
		myAddr.ai_socktype = SOCK_STREAM;
		myAddr.ai_protocol = IPPROTO_TCP;

		iResult = getaddrinfo(szDestAddress, pszPortNum, &myAddr, &result);
		if (iResult != 0 ) 
		{
			printf("ERROR: DeviceInterfaceMasterTCP failed to getaddrinfo - %d", iResult );
			cleanup();
			abort();
		}
		connectingSockFd = (int)socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		
		//linger my_linger;
		//my_linger.l_onoff = 1;
		//my_linger.l_linger = 10; // seconds

		//setsockopt(connectingSockFd, SOL_SOCKET, SO_LINGER, &my_linger, sizeof(my_linger));

		struct linger linger;
		linger.l_onoff = 1; 
			/*0 = off (l_linger ignored), nonzero = on */
		linger.l_linger =10; 
			/*0 = discard data, nonzero = wait for data sent */
		iResult = setsockopt(connectingSockFd, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
		if (iResult != 0 ) 
		{
			printf("ERROR: DeviceInterfaceMasterTCP failed to setsockopt to SO_LINGER - %d", iResult );
			cleanup();
			abort();
		}

		if (INVALID_SOCKET == connectingSockFd)
		{
			printf("ERROR: DeviceInterfaceMasterTCP failed to create socket - %d", lastError());
			freeaddrinfo(result);
			cleanup();
			abort();
		}

		iResult = connect(connectingSockFd, result->ai_addr, (int)result->ai_addrlen);
		if (SOCKET_ERROR == iResult)
		{
			printf("ERROR: DeviceInterfaceMasterTCP failed to connect - errno = %d\n", lastError());
			cleanup();
			abort();
		}
		DeviceInterfaceMasterSocket::initialise();
		//printf("SIMVERSION=%s__%s\n", sRemoteVersion.c_str(), szCvsTagName);
		freeaddrinfo(result);

	}
}

DeviceInterface* DeviceInterfaceMasterTCP::addNewDevice(std::string newName, IMG_BOOL useBuffer)
{
	IMG_UINT32 ui32PortNum;
	IMG_CHAR acPortNum[20];
	DeviceInterface* pNewDevice;

	checkInitialised();
	if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2,0))
	{
		printf("ERROR: DeviceInterfaceMasterTCP::addNewDevice Remote protocol too old for this functionality.");
		cleanup();
		abort();
	}
	enterMutex();
	// Device name to send
	sendCmd(E_DEVIF_SETUP_DEVICE, newName.length(), 0);
	sendData((IMG_UINT32)newName.length(), (void*)newName.c_str());
	
	recvWord(ui32PortNum);
	exitMutex();
	sprintf(acPortNum, "%d", ui32PortNum);

	pNewDevice = new DeviceInterfaceMasterTCP(newName, acPortNum, szDestAddress, (useBuffer != IMG_FALSE), name);
	//pNewDevice->initialise();
	return pNewDevice;
}

