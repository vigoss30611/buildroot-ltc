#ifndef _HI_MT_SOCKET_H_
#define _HI_MT_SOCKET_H_

#if HI_OS_TYPE == HI_OS_LINUX
#include <netinet/in.h>
#elif HI_OS_TYPE == HI_OS_WIN32
#include "Winsock2.h"
#endif



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef char HI_IP_ADDR[64];
typedef unsigned short HI_PORT;
typedef int  HI_SOCKET;

int      MT_SOCKET_GetPeerIPPort(int s, char* ip, unsigned short *port);
int      MT_SOCKET_GetPeerSockAddr(char* ip, unsigned short port,struct sockaddr_in *pSockAddr);

int      MT_SOCKET_GetHostIP(int s, char* ip);
int      MT_SOCKET_Udp_Open(unsigned short port);

int   MT_SOCKET_Multicast_Open(char *multicastAddr, unsigned short port);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
