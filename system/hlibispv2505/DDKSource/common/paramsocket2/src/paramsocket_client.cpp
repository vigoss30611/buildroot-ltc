/**
******************************************************************************
@file paramsocket_client.cpp

@brief 

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "paramsocket/paramsocket.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#ifdef SOCK_DBG
#define PARAMSOCK_DBG printf
#else
#define PARAMSOCK_DBG
#endif

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h> // getaddrinfo()
#define MIN min
#else
extern "C" {
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
}
#define MIN(A, B) std::min(long(A), long(B))
#endif

//
// private
//
ParamSocketClient::ParamSocketClient(const char *addr, int port): ParamSocket(port), fileAvailable(false)
{
	this->addr = addr;
}

int ParamSocketClient::connect_to_server()
{
	if ( sockfd == INVALID_SOCKET )
	{
		struct addrinfo *pAddrInfo;
		
		if ( (sockfd = openSocket(addr, port, &pAddrInfo)) == INVALID_SOCKET )
		{
			fprintf(stderr, "failed to create master socket (errno %ld)\n", lastError());
			return -1;
		}

		if ( connect(sockfd, pAddrInfo->ai_addr, (int)pAddrInfo->ai_addrlen) != 0 )
		{
			fprintf(stderr, "failed to connect to master (errno %ld)\n", lastError());
			close();
			return -1;
		}
		
		return 0;
	}
	return -2;
}

//
// public
//

ParamSocketClient* ParamSocketClient::connect_to_server(const char *addr, int port, const char *appName)
{
	ParamSocketClient *neo = new ParamSocketClient(addr, port);
	char name[PARAMSOCK_MAX];
	size_t nameSize, written;

	if ( neo && neo->connect_to_server() != 0 )
	{
		delete neo;
		return 0;
	}
	
	nameSize = std::min((size_t)PARAMSOCK_MAX-1, strlen(appName));
	strncpy(name, appName, nameSize);
	name[nameSize] = 0;
	
	if ( neo->write(name, nameSize, written) != 0 )
	{
		fprintf(stderr, "Failed to send application name\n");
		delete neo;
		return 0;
	}

	if ( neo->read(name, PARAMSOCK_MAX, nameSize) >= 0 )
	{
		name[nameSize] = 0;
		printf("Connected to %s on port %d\n", name, port);
        neo->_connectedName = name;
	}
	else
	{
		fprintf(stderr, "Failed to receive master name\n");
		delete neo;
		neo = 0;
	}

	return neo;
}

std::string ParamSocketClient::serverName() const
{
    return _connectedName;
}
