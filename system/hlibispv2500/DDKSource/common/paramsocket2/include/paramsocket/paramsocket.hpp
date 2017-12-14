/**
******************************************************************************
@file paramsocket.hpp

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
#ifndef PARAMSOCKET_HPP
#define PARAMSOCKET_HPP

#include <img_types.h>
#include <string>
#include <map>
#include <list>
#include <cstdio>

#ifdef WIN32
#include <WinSock2.h>
#else
#define SOCKET int
#define INVALID_SOCKET -1 // error code for socket()
#define SOCKET_ERROR -1 // error code for read()/write()
#endif


/**
 * @brief Maximum number of bytes sent per messages
 */
#define PARAMSOCK_MAX 512
/**
 * @brief Maximum number of u_long sent for a command including the command itself
 */
#define MAX_CMD_SIZE 20

class ParamSocket
{
protected:
    SOCKET sockfd; ///< @brief socket used for communication
    int port; ///< @brief port the connection is established on
    std::string _connectedName; ///< @brief name of the other party

    //FelixParameter::File lastFile; ///< @brief last used file

    FILE *debugRead; // used only when compiled with debug flags - save all read actions to file
    FILE *debugWrite; // used only when compiled with debug flags - save all write actions to file

    /**
     * @brief Access to last error (errno on linux, WSAGetLastError() on windows)
     */
    static long int lastError();

    /**
     * @brief Create a socket
     */
    static SOCKET openSocket(const char *add, unsigned port, struct addrinfo **addrInfoResult, bool passive=false);

public:

    static const char * cmdName(int cmd);

    ParamSocket(int port = 0);
    virtual ~ParamSocket();

    /**
     * Abstracts the usage of write() (linux) or send() (windows).
     *
     * @param[in] buff to write
     * @param[in] size in bytes
     * @param[out] written in bytes
     *
     * @return 0 on success (if >0 may not have written all data)
     * @return -1 if could not write enough information
     * @return -2 if the socket was not initialised
     */
    virtual int write(void *buff, size_t size, size_t &written);
    /**
     * @param tries if negative tries until the whole data is send
     */
    virtual int write(void *buff, size_t size, size_t &written, int tries);

    /**
     * Abstracts the usage of read() (linux) or recv() (windows)
     *
     * @param[out] buff to save into
     * @param[in] size of buff in bytes
     * @param[out] read number of bytes actually read
     *
     * @return 0 on success (may not have read all data)
     * @return -1 on failure to read
     * @return -2 if the socket was not initialised
     */
    virtual int read(void *buff, size_t size, size_t &read);
    /**
     * @param tries if negative tries until the whole data is send
     */
    virtual int read(void *buff, size_t size, size_t &read, int tries);

    /**
     * On linux flushes the socket using fsync(). On windows does nothing.
     */
    virtual void flush();
    /**
     * @brief Close the connection
     *
     * Abstracts the usage of close() (linux) or closesocket() (windows)
     */
    virtual void close();
};

class ParamSocketServer: public ParamSocket
{
private:
    SOCKET listenSock;
    int wait_for_connection(int timeout); // timeout in seconds

public:
    ParamSocketServer(int port = 0);
    ~ParamSocketServer();

    int wait_for_client(const char *appName, int timeout=0);

    static ParamSocketServer* wait_for_client(int port, const char *appName);

    std::string clientName() const;
};

class ParamSocketClient: public ParamSocket
{
private:
    const char *addr;
    bool fileAvailable; // changed to false when receiving

    ParamSocketClient(const char *addr, int port);

    int connect_to_server();

public:
    static ParamSocketClient* connect_to_server(const char *addr, int port, const char *appName);

    std::string serverName() const;
};

#endif // PARAMSOCKET_HPP
