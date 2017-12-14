/******************************************************************************

 @File         device_interface_master_socket.h

 @Title        The Hardware Intermediate Debug file processing.

 @Copyright    Copyright (C) 2003 - 2008 by Imagination Technologies Limited.

 @Platform     </b>\n

 @Description  SOCKET implementation the device interface master class.

******************************************************************************/
#ifndef DEVICE_INTERFACE_MASTER_SOCKET
#define DEVICE_INTERFACE_MASTER_SOCKET

#ifdef WIN32

#include <ws2tcpip.h>
#include <iphlpapi.h>

/* give socket shutdown parameters their linux names */
#define SHUT_RD		SD_RECEIVE
#define SHUT_WR		SD_SEND
#define SHUT_RDWR	SD_BOTH

#else /* ifdef WIN32 */

#include <sys/types.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif /* ifdef WIN32 */

#include <string>
#include "device_interface_master.h"
#include "device_interface_message.h"

class DeviceInterfaceMasterSocket : public DeviceInterfaceMaster
{
	public:

	DeviceInterfaceMasterSocket(std::string name, bool useBuffer=IMG_TRUE, std::string parentName="");
	~DeviceInterfaceMasterSocket();
	virtual void initialise(void);
	virtual void deinitialise(void);

	virtual DeviceInterface* addNewDevice(std::string newName, IMG_BOOL useBuffer) = 0;
        
	protected:
	DeviceInterfaceMasterSocket();	// prevent default constructor
	
	// prevent default assignment and copy constructors from being created
	DeviceInterfaceMasterSocket (const DeviceInterfaceMaster &foo);
	DeviceInterfaceMasterSocket& operator=(const DeviceInterfaceMaster &foo);

	virtual void cleanup();
	virtual void flushBuffer(); // Send any pending data
	virtual void recvData(IMG_INT32 uNumBytes, void* pData);
	virtual void devif_ntohl(IMG_UINT32& val){val = ntohl(val);}
	virtual void devif_htonl(IMG_UINT32& val){val = htonl(val);}
	virtual void devif_ntohll(IMG_UINT64& val);
	virtual void devif_htonll(IMG_UINT64& val);

	int lastError();
	int connectingSockFd;

};

#endif // DEVICE_INTERFACE_MASTER_TCP
