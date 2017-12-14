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
        this file contains the implementation of a devif_slave for tcpip

 \n<b>Platform:</b>\n
	     PC 

******************************************************************************/
#ifndef DEVIF_SLAVE_SOCKET_H
#define DEVIF_SLAVE_SOCKET_H

#include "device_interface_slave.h"

#ifdef WIN32

// This lets us use winsock2 instead of winsock
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "winsock2.h"

#else

#include <sys/socket.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

#define DEVIFSLAVE_EVENT_CONACCEPTED (0x00000001)

typedef struct
{
	IMG_UINT32      ui32Event;

} DEVIFSLAVE_EVENT_CALLBACK_DATA;

typedef IMG_VOID DEVIFSLAVE_EVENT_CALLBACK_FUNCTION ( DEVIFSLAVE_EVENT_CALLBACK_DATA	sCallbackData );

IMG_VOID CleanupListeningSocket(SOCKET socket);

class DevIfSlaveSocket: public DeviceInterfaceSlave
{
public:
	DevIfSlaveSocket(DeviceInterface& device,
		IMG_BOOL bVerbose = IMG_FALSE,
		IMG_CHAR* sDumpFile = NULL,
		IMG_BOOL bLoop = IMG_FALSE,
		SOCKET extListeningSockFd = 0,
		DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc = 0,
		std::string szMasterCommandLine = "",
		std::string szMasterLogFile = "",
		std::string szRevision= "slave");
	~DevIfSlaveSocket();
	
	
	virtual int  testRecv();

protected:
	void waitForHost();
	virtual void recvData();
	virtual void sendData(IMG_INT32 i32NumBytes, const void* pData);

	virtual void waitToRecv(SOCKET sock);
    virtual void setupDevice(IMG_UINT32 ui32StrLen)=0;
    virtual unsigned int getId()=0;
    
	virtual void devif_ntohl(IMG_UINT32& val){val = ntohl(val);}
	virtual void devif_htonl(IMG_UINT32& val){val = htonl(val);}
	virtual void devif_ntohll(IMG_UINT64& val);
	virtual void devif_htonll(IMG_UINT64& val);

	struct addrinfo myAddr;
	SOCKET acceptedsockFd;
	SOCKET listeningSockFd;
	struct addrinfo *result;
	char * szPortNum;

	int lastError();
	virtual void cleanup()=0;

	DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCallbackFunc;
	void FindAndReplace( std::string& tInput, std::string tFind, std::string tReplace ) ;
};

#endif
