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
#ifndef DEVIF_SLAVE_TCP_H
#define DEVIF_SLAVE_TCP_H

#include "device_interface_slave_socket.h"

typedef struct 
{
	SOCKET		socketFd;
	IMG_UINT32	ui32PortNum;
}SOCKET_INFO;

SOCKET CreateListeningSocket(const char* szPortNum);
SOCKET_INFO CreateListeningSocketGetPort(const char* szPortNum);

class DevIfSlaveTCP: public DevIfSlaveSocket
{
public:
	DevIfSlaveTCP(DeviceInterface& device,
		const char* szPortNum,
		IMG_BOOL bVerbose = IMG_FALSE,
		IMG_CHAR* sDumpFile = NULL,
		IMG_BOOL bLoop = IMG_FALSE,
		SOCKET extListeningSockFd = 0,
		DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc = 0,
		std::string szMasterCommandLine = "",
		std::string szMasterLogFile = "",
		std::string szRevision= "slave");
	~DevIfSlaveTCP();
	

	virtual DeviceInterfaceSlave * newSlaveDevice(DeviceInterface& device, IMG_UINT32 &uiPortNum);

protected:
	void waitForHost();

    virtual void setupDevice(IMG_UINT32 ui32StrLen);
    virtual unsigned int getId(){return uiPortNum;}


	IMG_UINT32 uiPortNum;
	struct addrinfo myAddr;
	string sPortNum;
    string sPortRange;

	void cleanup();

	IMG_BOOL bExternallySuppliedListeningSocket;
};

#endif
