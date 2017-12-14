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
#ifndef DEVIF_SLAVE_FILEREADER_H
#define DEVIF_SLAVE_FILEREADER_H


#ifdef WIN32

// This lets us use winsock2 instead of winsock
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "winsock2.h"
#include <ws2tcpip.h>
#include <iphlpapi.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#endif
#include "device_interface_slave.h"
class DevIfSlaveFilereader: public DeviceInterfaceSlave
{
public:
	DevIfSlaveFilereader(DeviceInterface& device,
		const char* pInputFilename,
		const char* pResultsFilename = NULL,
		IMG_BOOL bVerbose = IMG_FALSE,
		IMG_CHAR* sDumpFile = NULL
		);
	~DevIfSlaveFilereader();

protected:
	virtual void recvData();
	virtual void sendData(IMG_INT32 i32NumBytes, const void* pData);

	virtual int  testRecv(){return IMG_TRUE;}// Always data.}
	virtual void devif_ntohl(IMG_UINT32& val){val = ntohl(val);}
	virtual void devif_htonl(IMG_UINT32& val){val = htonl(val);}
	virtual void devif_ntohll(IMG_UINT64& val);
	virtual void devif_htonll(IMG_UINT64& val);
	virtual void waitForHost(void){}

	FILE* pInputFile;
	FILE* pResultsFile;
	IMG_UINT32 uResultsOffset;
};
#endif
