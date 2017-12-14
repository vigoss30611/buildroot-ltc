#ifndef GXNETWORK_H
#define GXNETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

typedef enum tagNetCheckType {
	kCheckForRead,
	kCHeckForWrite,
	kCheckForError,
}NETWORKCHECKTYPE;

void GXPrintNetInfo(const unsigned int ip, const unsigned short port, const int socketfd);
unsigned short GXGetSocketPort(const int socketfd);

int GXCreateSocket(const char *host, const unsigned short port);
int GXCreateSocket2(const char *host, const unsigned short port, const unsigned short maxNum);
int GXCloseSocket(int socketfd);
int GXAcceptClient(int socketfd, unsigned int *addr, unsigned short *port);
int GXCheckSocket(int socketfd, long sec, int usec, NETWORKCHECKTYPE checkType);
int GXSendPacket2(int socketfd, long timeout /*second*/, const void *buffer, size_t length);
int GXRecvPacket2(int socketfd, long timeout /*second*/, void *buffer, size_t length);
int GXRecvPacket3(int socketfd, long timeout /*second*/, void *buffer, size_t length);

int GXCreateSocketBrodcast(const char *host, const unsigned short port);
int GXSendtoPacket(int socketfd, struct sockaddr_in *to, long timeout, const void *buffer, size_t length);
int GXRecvfromPacket(int socketfd, long timeout, void *buffer, size_t length, struct sockaddr_in *from);

#ifdef __cplusplus
}
#endif

#endif