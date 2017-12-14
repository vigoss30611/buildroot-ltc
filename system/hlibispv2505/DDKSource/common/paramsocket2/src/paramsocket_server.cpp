/**
******************************************************************************
@file paramsocket_server.cpp

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
#include <cstring>
#include <algorithm>

//
// private
//

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h> // getaddrinfo()

#else
extern "C" {
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
}
#define closesocket ::close

#endif

#ifdef SOCK_DBG
#define PARAMSOCK_DBG printf
#else
#define PARAMSOCK_DBG
#endif

ParamSocketServer::ParamSocketServer(int port): ParamSocket(port)
{
    this->listenSock = INVALID_SOCKET;
}

ParamSocketServer::~ParamSocketServer()
{
    if ( listenSock != INVALID_SOCKET )
    {
        closesocket(listenSock);
        listenSock = INVALID_SOCKET;
    }
}

int ParamSocketServer::wait_for_connection(int timeout)
{
    if ( sockfd == INVALID_SOCKET && listenSock == INVALID_SOCKET )
    {
        struct addrinfo *pAddrInfo = NULL;
        socklen_t addrLen;

        if ( (listenSock = openSocket(NULL, port, &pAddrInfo, true)) == INVALID_SOCKET )
        {
            fprintf(stderr, "Failed to create master socket (errno %ld)\n", lastError());
            return -1;
        }

        if ( bind(listenSock, pAddrInfo->ai_addr, (int)pAddrInfo->ai_addrlen) == SOCKET_ERROR )
        {
            fprintf(stderr, "Failed to bind master socket (errno %ld)\n", lastError());
            closesocket(listenSock);
            listenSock = INVALID_SOCKET;
            return -1;
        }

        printf("Wait for connection on port %d...\n", port);
        if ( listen(listenSock, SOMAXCONN) != 0 ) // mark the socket as passive
        {
            fprintf(stderr, "Failed to listen on master socket (errno %ld)\n", lastError());
            closesocket(listenSock);
            listenSock = INVALID_SOCKET;
            return -1;
        }

        if ( timeout > 0 )
        {
            struct timeval tv;
            fd_set rfds;
	    int ret;
	    int selectOn = listenSock;
            FD_ZERO(&rfds);
            FD_SET(listenSock, &rfds);

            tv.tv_sec = (long)timeout;
            tv.tv_usec = 0;

#ifndef WIN32
	    selectOn++; // on linux the file descriptor needs a +1
#endif

            if ( (ret=select(selectOn, &rfds, NULL, NULL, &tv)) <= 0 )
            {
		if ( ret == SOCKET_ERROR )
		{
		    fprintf(stderr, "Failed to wait for connections on port %d (errno %ld)\n", port, lastError());
#ifndef WIN32
		    perror(__FUNCTION__);
#endif
		    ret = -2; // error
		}
		else
		{
		    fprintf(stderr, "Timeout waiting for connections on port %d after %d sec\n", port, timeout);
		    ret = -3; // timeout
		}
                closesocket(listenSock);
                listenSock = INVALID_SOCKET;
                return ret;
            }
        }

        addrLen = pAddrInfo->ai_addrlen;
        if ( (this->sockfd = accept(listenSock, pAddrInfo->ai_addr, &addrLen)) == INVALID_SOCKET )
        {
            fprintf(stderr, "Failed to listen on accepted client socket (errno %ld)\n", lastError());
            closesocket(listenSock);
            listenSock = INVALID_SOCKET;
            return -1;
        }

        closesocket(listenSock);
        listenSock = INVALID_SOCKET;
        return 0;
    }
    return -2;
}

//
// public
//
int ParamSocketServer::wait_for_client(const char *appName, int timeout)
{
    char name[PARAMSOCK_MAX];
    size_t nameSize, written;
    int ret;

    if ( (ret=wait_for_connection(timeout)) != 0 )
    {
        return ret;
    }

    if ( read(name, PARAMSOCK_MAX, nameSize) >= 0 )
    {
        name[nameSize] = 0;
        printf("Connection from %s accepted on port %d\n", name, port);
        _connectedName = name;
    }
    else
    {
        fprintf(stderr, "Failed to read connectee name\n");
        return -1;
    }

    nameSize = std::min((size_t)PARAMSOCK_MAX-1, strlen(appName));
    strncpy(name, appName, nameSize);
    name[nameSize] = 0;
    if ( write(name, nameSize, written) != 0 )
    {
        fprintf(stderr, "Failed to write name of master\n");
        return -1;
    }

    return 0;
}

ParamSocketServer* ParamSocketServer::wait_for_client(int port, const char *appName)
{
    ParamSocketServer *neo = new ParamSocketServer(port);


    if (!neo || neo->wait_for_client(appName) != 0)
    {
        if ( neo )
        {
            delete neo;
        }
        return 0;
    }

    return neo;
}

std::string ParamSocketServer::clientName() const
{
    return _connectedName;
}
