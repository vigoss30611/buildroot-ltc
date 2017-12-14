/******************************************************************************

 @File         device_interface_master_unix.h

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  unix socket implementation the device interface master class.

******************************************************************************/
#ifndef DEVICE_INTERFACE_MASTER_UNIX
#define DEVICE_INTERFACE_MASTER_UNIX

#include <sys/socket.h>
#include <string>
#include "device_interface_master_socket.h"

class DeviceInterfaceMasterUnix : public DeviceInterfaceMasterSocket
{
	public:

	DeviceInterfaceMasterUnix(std::string name, const char* szPath, int id, bool useBuffer=IMG_TRUE, std::string parentName="");
	~DeviceInterfaceMasterUnix();
	virtual void initialise(void);

	virtual DeviceInterface* addNewDevice(std::string newName, IMG_BOOL useBuffer);
        
	private:
	DeviceInterfaceMasterUnix();	// prevent default constructor
	
	// prevent default assignment and copy constructors from being created
	DeviceInterfaceMasterUnix (const DeviceInterfaceMaster &foo);
	DeviceInterfaceMasterUnix& operator=(const DeviceInterfaceMaster &foo);

	
	string sPath_;
    int id_;

};

#endif // DEVICE_INTERFACE_MASTER_UNIX
