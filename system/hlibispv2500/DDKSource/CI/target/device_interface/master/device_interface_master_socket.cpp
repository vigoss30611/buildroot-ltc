/******************************************************************************

 @File         device_interface_master_socket.cpp

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  Windows specific implementation the device interface master class.

******************************************************************************/
#include "device_interface_master_socket.h"
#include "img_defs.h"

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#endif

#ifdef WINCE
	#include <wince_env.h>	
	#include <wince_thread.h>
#endif
DeviceInterfaceMasterSocket::DeviceInterfaceMasterSocket(std::string name, bool useBuffer, std::string parentName)
	: DeviceInterfaceMaster(name, useBuffer, parentName)
{}

DeviceInterfaceMasterSocket::~DeviceInterfaceMasterSocket()
{}

void DeviceInterfaceMasterSocket::deinitialise(void)
{
	if (bInitialised) {
		DeviceInterfaceMaster::deinitialise();
		bInitialised = false;
		sRemoteName.clear();
		sRemoteVersion.clear();
	}
}

void DeviceInterfaceMasterSocket::initialise(void)
{
			
    char szDeviceName[DEVIF_PROTOCOL_MAX_STRING_LEN];
    /* Get device version */
    sendCmd(E_DEVIF_GET_DEVICENAME , 0, 0);
    if (!getString(szDeviceName, DEVIF_PROTOCOL_MAX_STRING_LEN))
    {
        printf("ERROR: DeviceInterfaceMasterSocket::initialise failed to get device name\n");
        cleanup();
        abort();
    }
    sRemoteName = szDeviceName;
    char szTagName[DEVIF_PROTOCOL_MAX_STRING_LEN];
    /* Get device version */
    sendCmd(E_DEVIF_GET_TAG , 0, 0);
    if (!getString(szTagName, DEVIF_PROTOCOL_MAX_STRING_LEN))
    {
        printf("ERROR: DeviceInterfaceMasterSocket::initialise failed to get device version\n");
        cleanup();
        abort();
    }
    sRemoteVersion = szTagName;

    /*Get Protocol Version */
    sendCmd(E_DEVIF_GET_DEVIF_VER , 0, 0);
    recvWord(ui32RemoteProtocol);
    ui32TransferMode = E_DEVIFT_32BIT;

    printf("DeviceInterfaceMasterSocket Connected to %s - version %s - protocol %08X .....\n", szDeviceName, szTagName, ui32RemoteProtocol);
    //printf("SIMVERSION=%s__%s\n", sRemoteVersion.c_str(), szCvsTagName);


}

void DeviceInterfaceMasterSocket::flushBuffer()
{
	const char *ptr = (const char *)pCmdBuffer;
    IMG_INT32 rval;
    
	while (i32ReadyCount)
	{
		rval = (IMG_INT32)send(connectingSockFd, ptr, i32ReadyCount, 0);
		if (rval == -1)
		{
#ifdef EINTR // because it is not defined in visual studio 9
            if (errno != EINTR)
            {
                printf("ERROR: DeviceInterfaceMasterSocket::flushBuffer failed to send - errno = %d\n", lastError());
                cleanup();
                abort();
            }
#endif
            continue;
		}
        
		ptr += rval;
		i32ReadyCount -= rval;
	}
}

void DeviceInterfaceMasterSocket::recvData(IMG_INT32 i32NumBytes, void* pData)
{
	char* ptr = static_cast<char*>(pData);
    IMG_INT32 rval;
    
	// Can't receive while there are commands waiting to go out.
	flushBuffer();
    
	while (i32NumBytes)
	{
		rval = (IMG_INT32)recv(connectingSockFd, ptr, i32NumBytes, 0);
		if (rval == -1)
		{
#ifdef EINTR // because it is not defined in visual studio 9
            if (errno != EINTR)
            {
                printf("ERROR: DeviceInterfaceMasterSocket::recvData failed to recv - errno = %d", lastError());
                cleanup();
                abort();
            }
#endif
            continue;
		}
		else if (rval == 0)
		{
			printf("ERROR: DeviceInterfaceMasterSocket::flushBuffer failed to recv - remote closed connection\n");
			cleanup();
			abort();
		}
        
		if (rval > i32NumBytes)
		{
			printf("ERROR: DeviceInterfaceMasterSocket::flushBuffer received more data than it expected - %d > %d",
                   rval,
                   i32NumBytes);
			cleanup();
			abort();
		}
        
		ptr += rval;
		i32NumBytes -= rval;
	}
}

int DeviceInterfaceMasterSocket::lastError()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}


void DeviceInterfaceMasterSocket::cleanup()
{
	IMG_CHAR cMessage;
	/* Shutdown Sending side of socket */
	if (shutdown(connectingSockFd, SHUT_WR) != 0)
	{
		printf("ERROR: DeviceInterfaceMasterSocket::cleanup Socket Send Shutdown Failed\n");
		return;
	}
	/* Empty Receive buffer (This should be empty) */
	while (recv(connectingSockFd, &cMessage, 1, 0) > 0);
	
#ifdef WIN32
	/* Shutdown all of socket */
	if (shutdown(connectingSockFd, SHUT_RDWR))
	{
		printf("ERROR: DeviceInterfaceMasterSocket::cleanup Socket Full Shutdown Failed\n");
		return;
	}


	/* close socket device */
	if (closesocket(connectingSockFd))
	{
		printf("ERROR: DeviceInterfaceMasterSocket::cleanup Socket Close Failed\n");
		return;
	}
	WSACleanup();
#else
	/* close socket device */
	if (close(connectingSockFd))
	{
		printf("ERROR: DeviceInterfaceMasterSocket::cleanup Socket Close Failed\n");
		return;
	}
#endif
}

void DeviceInterfaceMasterSocket::devif_ntohll(IMG_UINT64& val)
{
#if IMG_BYTE_ORDER == IMG_LITTLE_ENDIAN
	IMG_UINT32 ui32High, ui32Low;
	ui32High = (IMG_UINT32)(val >> 32);
	ui32Low = (IMG_UINT32)(val & 0xFFFFFFFF);
	ntohl(ui32High);
	ntohl(ui32Low);
	val = (((IMG_UINT64)ui32High) << 32) | ui32Low;
#endif
}

void DeviceInterfaceMasterSocket::devif_htonll(IMG_UINT64& val)
{
	devif_ntohll(val);
}


