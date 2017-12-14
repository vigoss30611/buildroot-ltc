/******************************************************************************

 @File         device_interface_master_winsock.cpp

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  Windows specific implementation the device interface master class.

******************************************************************************/
#include "device_interface_master_unix.h"
#include "img_defs.h"

#include <errno.h>
#include <unistd.h>
#include <sys/un.h>

DeviceInterfaceMasterUnix::DeviceInterfaceMasterUnix(std::string name, const char* szPath, int id, bool useBuffer, std::string parentName)
	: DeviceInterfaceMasterSocket(name, useBuffer, parentName)
	, sPath_(szPath)
{}

DeviceInterfaceMasterUnix::~DeviceInterfaceMasterUnix()
{}


void DeviceInterfaceMasterUnix::initialise(void)
{
    int iResult;
	struct sockaddr_un remote;

	if (!bInitialised) {
		DeviceInterfaceMaster::initialise();
		bInitialised = true;

		connectingSockFd = (int)socket(AF_UNIX, SOCK_STREAM, 0);
		if (connectingSockFd==-1)
		{
			printf("ERROR: DeviceInterfaceMasterUnix failed to open socket errno = %d", lastError());
			abort();
		}
        remote.sun_family = AF_UNIX;
        strcpy(remote.sun_path, sPath_.c_str());
        int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
		iResult = connect(connectingSockFd, (struct sockaddr *)&remote, len);
		if (SOCKET_ERROR == iResult)
		{
			printf("ERROR: DeviceInterfaceMasterUnix failed to connect - errno = %d\n", lastError());
			cleanup();
			abort();
		}
		DeviceInterfaceMasterSocket::initialise();
	}
}

DeviceInterface* DeviceInterfaceMasterUnix::addNewDevice(std::string newName, IMG_BOOL useBuffer)
{
	DeviceInterface* pNewDevice;
    unsigned int id;
    string newPath = sPath_;

	checkInitialised();
	if (ui32RemoteProtocol < DEVIF_PROTOCOL_MK_VERSION(2,0))
	{
		printf("ERROR: DeviceInterfaceMasterUnix::addNewDevice Remote protocol too old for this functionality.");
		cleanup();
		abort();
	}
	enterMutex();
	// Device name to send
	sendCmd(E_DEVIF_SETUP_DEVICE, newName.length(), 0);
	sendData((IMG_UINT32)newName.length(), (void*)newName.c_str());
	
	recvWord(id);
    
    newPath.append("_");
    newPath.append(newName);
    
	exitMutex();
	
	pNewDevice = new DeviceInterfaceMasterUnix(newName, newPath.c_str(), id, (useBuffer != IMG_FALSE), name);
	//pNewDevice->initialise();
	return pNewDevice;
}

