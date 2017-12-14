/*!
******************************************************************************
 @file   : device_interface_slave_socket_unix.h

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
        this file contains the implementation of a devif_slave for unix sockets

 \n<b>Platform:</b>\n
	     PC 

******************************************************************************/
#ifndef DEVIF_SLAVE_SOCKET_UNIX_H
#define DEVIF_SLAVE_SOCKET_UNIX_H

#include "device_interface_slave_socket.h"

SOCKET CreateListeningSocketUnix(const char* szPortNum);

class DevIfSlaveUnix: public DevIfSlaveSocket
{
public:
	DevIfSlaveUnix(DeviceInterface& device,
		const char* pszPath,
        IMG_UINT32 ui32Id = 0,
		IMG_BOOL bVerbose = IMG_FALSE,
		IMG_CHAR* sDumpFile = NULL,
		IMG_BOOL bLoop = IMG_FALSE,
		SOCKET extListeningSockFd = 0,
		DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc = 0,
		std::string szMasterCommandLine = "",
		std::string szMasterLogFile = "",
		std::string szRevision= "slave");
	~DevIfSlaveUnix();
	

	virtual DeviceInterfaceSlave * newSlaveDevice(DeviceInterface& device, const IMG_CHAR* pszPath);


protected:
	void waitForHost();

    virtual void setupDevice(IMG_UINT32 ui32StrLen);
    virtual unsigned int getId(){return id_;}


	string sPath_;
    IMG_UINT32 id_;
    IMG_UINT32 subDevId_;
	//struct addrinfo myAddr;
	//string sPortNum;
    //string sPortRange;

	void cleanup();

	IMG_BOOL bExternallySuppliedListeningSocket_;
};

#endif
