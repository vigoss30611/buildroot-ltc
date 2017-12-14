#ifndef __HIK_SEARCH_PROTOCOL_H__
#define __HIK_SEARCH_PROTOCOL_H__

#include <dlfcn.h>
//#include "hik_netfun.h"

int HikSearchCreateSocket(int *pSockType);
int HikSearchRequsetProc(int dmsHandle, int sockfd, int sockType);

#endif
