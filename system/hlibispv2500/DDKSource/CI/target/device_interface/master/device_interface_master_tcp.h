/******************************************************************************

 @File         device_interface_master_tcp.h

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  TCP/IP implementation the device interface master class.

******************************************************************************/
#ifndef DEVICE_INTERFACE_MASTER_TCP
#define DEVICE_INTERFACE_MASTER_TCP

#ifdef WIN32

#include "winsock2.h"

/* give socket shutdown parameters their linux names */
#define SHUT_RD		SD_RECEIVE
#define SHUT_WR		SD_SEND
#define SHUT_RDWR	SD_BOTH

#else /* ifdef WIN32 */

#include <sys/socket.h>

#endif /* ifdef WIN32 */

#include <string>
#include "device_interface_master_socket.h"

class DeviceInterfaceMasterTCP : public DeviceInterfaceMasterSocket
{
	public:

	DeviceInterfaceMasterTCP(std::string name, const char* szPortNum, const char* szAddress, bool useBuffer=IMG_TRUE, std::string parentName="");
	~DeviceInterfaceMasterTCP();
	virtual void initialise(void);

	virtual DeviceInterface* addNewDevice(std::string newName, IMG_BOOL useBuffer);
        
	private:
	DeviceInterfaceMasterTCP();	// prevent default constructor
	
	// prevent default assignment and copy constructors from being created
	DeviceInterfaceMasterTCP (const DeviceInterfaceMaster &foo);
	DeviceInterfaceMasterTCP& operator=(const DeviceInterfaceMaster &foo);

	void setupAddress(const char* szAddress);

	char* pszPortNum;
	struct addrinfo myAddr;
	char* szDestAddress;
    
};

#endif // DEVICE_INTERFACE_MASTER_TCP
