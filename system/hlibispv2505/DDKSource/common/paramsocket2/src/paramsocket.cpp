/**
******************************************************************************
@file paramsocket.cpp

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
#include "paramsocket/demoguicommands.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <img_errors.h>
#include <img_defs.h>

#ifdef WIN32
#include <WS2tcpip.h> // getaddrinfo()

#pragma comment(lib, "Ws2_32.lib")
static int nSock = 0;

#else

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
}

#endif

//#define SOCK_DBG

#if defined(SOCK_DBG)
#define PARAMSOCK_DBG(...) {printf("ParamSocket::%s() ", __FUNCTION__); printf(__VA_ARGS__);}
#else
#define PARAMSOCK_DBG
#endif

#define N_TRIES 5

//#define USE_ACK // define it to make the transfert of file more "secure" (1 ack per parameter) but also very slow

//
// protected static
//

long int ParamSocket::lastError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

SOCKET ParamSocket::openSocket(const char *add, unsigned port, struct addrinfo **addrInfoResult, bool passive)
{
    struct addrinfo sAddr;
    int ret;
    char pszport[8];
    SOCKET resultSocket = INVALID_SOCKET;
	int trueInt = 1;
	
    memset(&sAddr, 0, sizeof sAddr);
    sAddr.ai_family = AF_INET;
    sAddr.ai_socktype = SOCK_STREAM;
    sAddr.ai_protocol = IPPROTO_TCP;
    if ( passive )
    {
        sAddr.ai_flags = AI_PASSIVE;
    }
	
    printf("Open socket on %s:%d\n", add?add:"null", port);

    sprintf(pszport, "%d", port);
    if ( (ret=getaddrinfo(add, pszport, &sAddr, addrInfoResult)) != 0 )
    {
        fprintf(stderr, "failed to get address information (returned %d)\n", ret);
        return resultSocket;
    }

    if ( (resultSocket = socket((*addrInfoResult)->ai_family, (*addrInfoResult)->ai_socktype, (*addrInfoResult)->ai_protocol)) == INVALID_SOCKET )
    {
        fprintf(stderr, "Failed to create master socket (errno %ld)\n", lastError());
        return INVALID_SOCKET;
    }

	if(setsockopt(resultSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&trueInt, sizeof(trueInt)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Failed to set socket option SO_REUSEADDR (errno %ld)\n", lastError());
        return INVALID_SOCKET;
	}

    return resultSocket;
}

//
// public static
//

const char * ParamSocket::cmdName(int cmd)
{
    switch(cmd)
    {
	case GUICMD_QUIT: return "GUICMD_QUIT";

	// HWINFO
	case GUICMD_GET_HWINFO: return "GUICMD_GET_HWINFO";

	// PARAMETERlIST
	case GUICMD_SET_PARAMETER_LIST: return "GUICMD_SET_PARAMETER_LIST";
	case GUICMD_SET_PARAMETER_NAME: return "GUICMD_SET_PARAMETER_NAME";
	case GUICMD_SET_PARAMETER_VALUE: return "GUICMD_SET_PARAMETER_VALUE";
	case GUICMD_SET_PARAMETER_END: return "GUICMD_SET_PARAMETER_END";
	case GUICMD_SET_PARAMETER_LIST_END: return "GUICMD_SET_PARAMETER_LIST_END";
	case GUICMD_GET_PARAMETER_LIST: return "GUICMD_GET_PARAMETER_LIST";

	// IMAGE
	case GUICMD_SET_IMAGE: return "GUICMD_SET_IMAGE";
	case GUICMD_GET_IMAGE: return "GUICMD_GET_IMAGE";
	case GUICMD_SET_IMAGE_RECORD: return "GUICMD_SET_IMAGE_RECORD";

	// SENSOR
	case GUICMD_GET_SENSOR: return "GUICMD_GET_SENSOR";

	// AE
	case GUICMD_GET_AE: return "GUICMD_GET_AE";
	case GUICMD_SET_AE_ENABLE: return "GUICMD_SET_AE_ENABLE";

	// AWB
	case GUICMD_GET_AWB: return "GUICMD_GET_AWB";
	case GUICMD_SET_AWB_MODE: return "GUICMD_SET_AWB_MODE";

	// AF
	case GUICMD_GET_AF: return "GUICMD_GET_AF";
	case GUICMD_SET_AF_ENABLE: return "GUICMD_SET_AF_ENABLE";
	case GUICMD_SET_AF_TRIG: return "GUICMD_SET_AF_TRIG";

	// TNM
	case GUICMD_GET_TNM: return "GUICMD_GET_TNM";
	case GUICMD_SET_TNM_ENABLE: return "GUICMD_SET_TNM_ENABLE";

	// LIVEFEED
	case GUICMD_SET_LF_ENABLE: return "GUICMD_SET_LF_ENABLE";
	case GUICMD_SET_LF_FORMAT: return "GUICMD_SET_LF_FORMAT";
	case GUICMD_GET_LF: return "GUICMD_GET_LF";
	case GUICMD_SET_LF_REFRESH: return "GUICMD_SET_LF_REFRESH";

	// DPF
	case GUICMD_GET_DPF: return "GUICMD_GET_DPF";
	case GUICMD_GET_DPF_MAP: return "GUICMD_GET_DPF_MAP";
	case GUICMD_SET_DPF_MAP: return "GUICMD_SET_DPF_MAP";

	// LSH
	case GUICMD_ADD_LSH_GRID: return "GUICMD_ADD_LSH_GRID";
    case GUICMD_REMOVE_LSH_GRID: return "GUICMD_REMOVE_LSH_GRID";
    case GUICMD_SET_LSH_GRID: return "GUICMD_SET_LSH_GRID";
    case GUICMD_SET_LSH_ENABLE: return "GUICMD_SET_LSH_ENABLE";
    case GUICMD_GET_LSH: return "GUICMD_GET_LSH";

	// LBC
	case GUICMD_GET_LBC: return "GUICMD_GET_LBC";
	case GUICMD_SET_LBC_ENABLE: return "GUICMD_SET_LBC_ENABLE";

	// DNS
	case GUICMD_SET_DNS_ENABLE: return "GUICMD_SET_DNS_ENABLE";

	// ENS
	case GUICMD_GET_ENS: return "GUICMD_GET_ENS";

	// GMA
	case GUICMD_GET_GMA: return "GUICMD_GET_GMA";
	case GUICMD_SET_GMA: return "GUICMD_SET_GMA";

    // PDP
    case GUICMD_SET_PDP_ENABLE: return "GUICMD_SET_PDP_ENABLE";

	// TEST
	case GUICMD_GET_FB: return "GUICMD_GET_FB";
    }

    return "NOT A COMMAND";
}

//
// public
//

ParamSocket::ParamSocket(int port): sockfd(INVALID_SOCKET), port(port)
{
#ifdef WIN32
    if ( nSock == 0 )
    {
        int ret;
        WSADATA wsaData;
        if ( (ret=WSAStartup(MAKEWORD(2, 2), &wsaData)) != NOERROR )
        {
            fprintf(stderr, "Failed to initialise WSA! returned %d\n", ret);
            throw ret;
        }
        nSock++;
    }
#endif
    this->sockfd = INVALID_SOCKET;

#ifdef SOCK_DBG
    {
        char tmp[32];
        sprintf(tmp, "read%d_dbg.txt", port);
        debugRead = fopen(tmp, "w");
        sprintf(tmp, "write%d_dbg.txt", port);
        debugWrite = fopen(tmp, "w");
    }
#else
    debugRead = NULL;
    debugWrite = NULL;
#endif
}

ParamSocket::~ParamSocket()
{
    close();
#ifdef WIN32
    nSock--;
    if ( nSock == 0 )
    {
        WSACleanup();
    }
#endif

    if ( debugRead )
    {
        fclose(debugRead);
        debugRead = NULL;
    }
    if ( debugWrite )
    {
        fclose(debugWrite);
        debugWrite = NULL;
    }
}

int ParamSocket::write(void *buff, size_t size, size_t &written)
{
    if ( sockfd != INVALID_SOCKET )
    {
        if ( size >= sizeof(IMG_UINT32) )
        {
            PARAMSOCK_DBG("trying for %ld bytes (first %ld = 0x%lx<%s>)\n", size, sizeof(IMG_UINT32), ntohl(*((IMG_UINT32*)buff)), ParamSocket::cmdName(ntohl(*((IMG_UINT32*)buff))));
        }
        else
        {
            PARAMSOCK_DBG("trying for %ld bytes (first = 0x%x)\n", size, ntohs(*((unsigned char*)buff)));
        }

#ifdef WIN32
        written = send(sockfd, (const char*)buff, (int)size, 0); // see WinError.h for error numbers
#else
        written = ::write(sockfd, (char*)buff, (int)size);
#endif
        PARAMSOCK_DBG("written %ld/%ld\n", written, size);

#ifdef SOCK_DBG
        if ( debugWrite && written >= sizeof(char) )
        {
            int b;
            size_t wrtLg = written/sizeof(char);
            char *bufL = (char*)buff;
            fprintf(debugWrite, "%s %ld/%ld bytes:", __FUNCTION__, written, size);
            for ( b = 0 ; b < wrtLg ; b++ )
            {
                fprintf(debugWrite, " 0x%x", bufL[b]);
            }
            fprintf(debugWrite, "\n");
            fflush(debugWrite);
        }
#endif


        if ( written != size )
        {
            fprintf(stderr, "Failed to send all data on socket, %zu/%zu sent (errno %ld)\n", written, size, lastError());
#ifndef WIN32
            perror(__FUNCTION__);
#endif
            return 1;
        }
        return 0;
    }
    fprintf(stderr, "Socket is not initialised\n");
    return -2;
}

int ParamSocket::write(void *buff, size_t size, size_t &written, int tries)
{
    int ret = 1;
    int ch = -1;
    size_t wr = 0;
    if ( tries < 0 )
    {
        ch = 0;
        tries = 1;
    }
    written = 0;
    while ( ret > 0 )
    {
        PARAMSOCK_DBG("try=%d wrriten=%ld/%ld\n", tries, written, size);

        wr = 0;
        ret = this->write(&(((char*)buff)[written]), size-written, wr);
        written += wr;

        if ( written == size )
        {
            return ret;
        }
        tries += ch;
        if ( tries == 0 )
        {
            return -2;
        }
    }
    return ret;
}

int ParamSocket::read(void *buff, size_t size, size_t &_read)
{
    if ( sockfd != INVALID_SOCKET )
    {
        PARAMSOCK_DBG("trying for %d bytes...\n", size);

#ifdef WIN32
        _read = recv(sockfd, (char*)buff, (int)size, 0);
#else
        _read = ::read(sockfd, (char*)buff, (int)size);
#endif
        if ( _read >= sizeof(IMG_UINT32) )
        {
            PARAMSOCK_DBG("read %ld/%ld bytes (first %ld = 0x%lx<%s>)\n", _read, size, sizeof(IMG_UINT32), ntohl(*((IMG_UINT32*)buff)), ParamSocket::cmdName(ntohl(*((IMG_UINT32*)buff))));
        }
        else if ( _read > 0 )
        {
            PARAMSOCK_DBG("read %ld/%ld bytes (first = 0x%x)\n", size, _read, ntohs(*((unsigned char*)buff)));
        }
        else
        {
            PARAMSOCK_DBG("couldn't read!\n");
        }

#ifdef SOCK_DBG
        if ( debugRead && _read >= sizeof(char) )
        {
            int b;
            size_t wrtLg = _read/sizeof(char);
            char *bufL = (char*)buff;
            fprintf(debugRead, "%s %ld/%ld bytes:", __FUNCTION__, _read, size);
            for ( b = 0 ; b < wrtLg ; b++ )
            {
                fprintf(debugRead, " 0x%x", bufL[b]);
            }
            fprintf(debugRead, "\n");
            fflush(debugRead);
        }
#endif

        if ( _read == SOCKET_ERROR )
        {
            fprintf(stderr, "Failed to read on socket (errno %ld)\n", lastError());
#ifndef WIN32
            perror(__FUNCTION__);
#endif
            return -1;
        }
        if ( _read < size )
        {
            return 1;
        }
        return 0;
    }
    fprintf(stderr, "Socket is not initialised\n");
    return -2;
}

int ParamSocket::read(void *buff, size_t size, size_t &_read, int tries)
{
    int ret = 1;
    int ch = -1;
    size_t rd = 0;
    if ( tries < 0 )
    {
        ch = 0;
        tries = 1;
    }
    _read = 0;
    while ( tries > 0 && ret > 0 )
    {
        PARAMSOCK_DBG("try=%d read %ld/%ld\n", tries, _read, size);

        ret = this->read(&(((char*)buff)[_read]), size-_read, rd);
        _read += rd;
        if ( _read == size )
        {
            return ret;
        }
        tries += ch;
        if ( tries == 0 )
        {
            return -2;
        }
    }
    return ret;
}

void ParamSocket::flush()
{
#ifndef WIN32
    if ( sockfd != INVALID_SOCKET )
    {
        fsync(sockfd);
    }
#endif
}

void ParamSocket::close()
{
    if ( sockfd != INVALID_SOCKET )
    {
#ifdef WIN32
        closesocket(sockfd);
#else
        ::close(sockfd);
#endif
        sockfd = INVALID_SOCKET;
    }
}