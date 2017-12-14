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
        this file contains the implementation of a devif_slave - a moudule that listens over a socket and passes 
		transactions on to a DeviceInterface instance


 \n<b>Platform:</b>\n
	     PC 

******************************************************************************/
#include "device_interface_slave_socket.h"
#include "img_defs.h"

#include <errno.h>
#ifndef WIN32
#	include <unistd.h>
#	include <poll.h>
#endif
#include <sstream>


IMG_VOID CleanupListeningSocket(SOCKET socket)
{
    /* any cleaning of the listening socket here? */
#ifdef WIN32
        if(socket) closesocket(socket);
#else
        if(socket)
        {
            shutdown(socket, SHUT_RDWR);
            close(socket);
        }
#endif

#ifdef WIN32
    WSACleanup();
#endif
}

DevIfSlaveSocket::DevIfSlaveSocket(DeviceInterface& device, 
					 IMG_BOOL bVerbose,
					 IMG_CHAR* sDumpFile,
					 IMG_BOOL bLoop,
					 SOCKET extListeningSockFd,
					 DEVIFSLAVE_EVENT_CALLBACK_FUNCTION* pfCBFunc,
					 std::string szMasterCommandLine,
					 std::string szMasterLogFile,
					 std::string szRevision)
	: DeviceInterfaceSlave(device, bVerbose, sDumpFile, bLoop, szMasterCommandLine, szMasterLogFile, szRevision)
	, acceptedsockFd(0)
	, listeningSockFd(extListeningSockFd)
	, result(NULL)
{
	pfCallbackFunc = pfCBFunc;
}

DevIfSlaveSocket::~DevIfSlaveSocket()
{}


void DevIfSlaveSocket::waitForHost()
{
	//Poll for connection attempt, and then accept it
	waitToRecv(listeningSockFd);

	acceptedsockFd = accept(listeningSockFd, NULL, NULL);
	if (INVALID_SOCKET==acceptedsockFd)
	{
		printf("ERROR: DevIfSlave failed to accept - errno = %d\n", lastError());
		cleanup();
		throw IMG_FALSE;
	}

	if (pfCallbackFunc)
	{
		DEVIFSLAVE_EVENT_CALLBACK_DATA sCallbackData;

		sCallbackData.ui32Event = DEVIFSLAVE_EVENT_CONACCEPTED;
		pfCallbackFunc(sCallbackData);
	}
}

void DevIfSlaveSocket::recvData()
{
	static int iTotalBytes=0;

	//Poll for data to be recieved before doing blocking read on socket
	if (!bProcessingGetData || !pBinInputFile) {
		// waitToRecv might idle - we can't allow this to happen mid fetch if we are 
		// capturing the input to file binaryInput - since the autoIdle would be inserted
		// to binaryInput in the middle of the fetch. BUT - do allow idle if we aren't 
		// capturing the input as we need it if using codescape.
		waitToRecv(acceptedsockFd);
	}
	int rval = recv(acceptedsockFd, (char*)pCmdBuffer, i32BufSize, 0);
	if (rval == -1)
	{
		printf("ERROR: DevIfSlaveSocket::recv - errno = %d", lastError());
		cleanup();
		throw IMG_TRUE;
	} 
	else if (rval == 0)
	{
		printf("ERROR: DevIfSlaveSocket::recv - theother party went away\n");
		cleanup();
		throw IMG_TRUE;

	}
	iTotalBytes += rval;

	i32ReadyCount = rval;
	i32ReadOffset = 0;
}

void DevIfSlaveSocket::sendData(IMG_INT32 i32NumBytes, const void* pData)
{
	const char* ptr = static_cast<const char*>(pData);
	while (i32NumBytes)
	{	
		IMG_INT32 rval = send(acceptedsockFd, ptr, i32NumBytes, 0);
		if (rval == -1)
		{
			printf("ERROR: DevIfSlaveSocket::sendData failed to send - errno = %d\n", lastError());
			cleanup();
			throw IMG_TRUE;
		}
		ptr += rval;
		i32NumBytes -= rval;
	}
}
extern IMG_BOOL g_bConnected;

void DevIfSlaveSocket::waitToRecv(SOCKET sock)
{
#ifdef WIN32
	if(g_bConnected==IMG_FALSE && !szMasterCommandLine.empty())
	{
		struct timeval timeout = {0, 100};
		fd_set select_set;
		int return_val;
		do {
			//select_set needs to be reset between each call to select
			FD_ZERO(&select_set);

#ifdef _MSC_VER // Visual Studio macro for FD_SET issues a warning (C4127: conditional expression is constant)
#pragma warning(push)
#pragma warning(disable:4127)
			FD_SET(sock, &select_set);
#pragma warning(pop)
#else
			FD_SET(sock, &select_set);
#endif /* _MSC_VER */

			return_val = select((int)(sock + 1), &select_set, NULL, NULL, &timeout);
			if(SOCKET_ERROR == return_val)
			{
				printf("ERROR: DevIfSlave failed to wait for incoming data/connection - errno = %d", lastError());
				cleanup();
				throw IMG_TRUE;
			}

			if(return_val != 0)
			{
				// Check to see if child is still alive or not
				DWORD exitCode;
				if(GetExitCodeProcess(lpMasterProcess->hProcess, &exitCode)==0) 
				{
					printf("ERROR: DevIfSlave failed to to get status of master process - errno = %d", GetLastError());
					cleanup();
					throw IMG_TRUE;
				}
				if(exitCode != STILL_ACTIVE)
				{
					printf("ERROR: External App has exited before connection has been established, killing simulator ...\n");
					fflush(stdout);
					IMG_ASSERT(IMG_FALSE);
				}
			}
		} while(return_val==0);
	}
#endif
	
	if (ui32AutoIdleCycles != 0)
	{
	
		struct timeval timeout = {0, 0};
		fd_set select_set;
		int return_val;
		IMG_BOOL bHasIdled=IMG_FALSE;
		do {
			//select_set needs to be reset between each call to select
			FD_ZERO(&select_set);

#ifdef _MSC_VER // Visual Studio macro for FD_SET issues a warning (C4127: conditional expression is constant)
#pragma warning(push)
#pragma warning(disable:4127)
			FD_SET(sock, &select_set);
#pragma warning(pop)
#else
			FD_SET(sock, &select_set);
#endif /* _MSC_VER */

			return_val = select((int)(sock + 1), &select_set, NULL, NULL, &timeout);
			if(SOCKET_ERROR == return_val)
			{
				printf("ERROR: DevIfSlave failed to wait for incoming data/connection - errno = %d", lastError());
				cleanup();
				throw IMG_TRUE;
			}
			if(0 == return_val) 
			{
				autoIdle(ui32AutoIdleCycles);
			}
		} while (return_val==0);

		flushAutoIdle();
		if (pDumpFile) 
		{
			static IMG_UINT64 last_time=0;
			IMG_UINT64 cur_time;
			if(bHasIdled) fprintf(pDumpFile, "\n");
			if(device.getDeviceTime(&cur_time)){
				fprintf(pDumpFile, "Time Passed: %" IMG_I64PR "d\n", cur_time - last_time);
			}
			last_time = cur_time;
		}
		if(0 == return_val) device.idle(ui32AutoIdleCycles);
	}
}



int DevIfSlaveSocket::testRecv()
{
	struct timeval timeout = {0, 0};
	fd_set select_set;
	int return_val;

	if (i32ReadyCount - i32ReadOffset > 0)
	{
		return IMG_TRUE;
	}

	do
	{
		//select_set needs to be reset between each call to select
		FD_ZERO(&select_set);

#ifdef _MSC_VER // Visual Studio macro for FD_SET issues a warning (C4127: conditional expression is constant)
#pragma warning(push)
#pragma warning(disable:4127)
		FD_SET(acceptedsockFd, &select_set);
#pragma warning(pop)
#else
		FD_SET(acceptedsockFd, &select_set);
#endif /* _MSC_VER */		

		//Check for input
		return_val = select((int)(acceptedsockFd + 1), &select_set, NULL, NULL, &timeout);
	} while (return_val == SOCKET_ERROR && lastError() == EINTR);
	if(SOCKET_ERROR == return_val)
	{
		printf("ERROR: DevIfSlave failed to wait for incoming data/connection - errno = %d", lastError());
		cleanup();
		throw IMG_TRUE;
	}
	return return_val;
}

int DevIfSlaveSocket::lastError()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void DevIfSlaveSocket::devif_ntohll(IMG_UINT64& val)
{
#if IMG_BYTE_ORDER == IMG_LITTLE_ENDIAN
	IMG_UINT32 ui32High, ui32Low;
	ui32High = (IMG_UINT32)(val >> 32);
	ui32Low = (IMG_UINT32)(val & 0xFFFFFFFF);
	ui32High = ntohl(ui32High);
	ui32Low = ntohl(ui32Low);
	val = (((IMG_UINT64)ui32High) << 32) | ui32Low;
#endif
}

void DevIfSlaveSocket::devif_htonll(IMG_UINT64& val)
{
	devif_ntohll(val);
}



void DevIfSlaveSocket::FindAndReplace( std::string& tInput, std::string tFind, std::string tReplace ) 
{ 
	size_t uPos = 0; 
	size_t uFindLen = tFind.length(); 
	size_t uReplaceLen = tReplace.length();

	if( uFindLen == 0 )
	{
		return;
	}

	for( ;(uPos = tInput.find( tFind, uPos )) != std::string::npos; )
	{
		tInput.replace( uPos, uFindLen, tReplace );
		uPos += uReplaceLen;
	}

}


