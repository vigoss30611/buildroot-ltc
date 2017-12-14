#ifndef __TS_SEARCH_PROTOCL_H__
#define __TS_SEARCH_PROTOCL_H__

#include "ts_basic.h"

int TsSearchCreateSocket(int *pSockType);
int TsSearchRequsetProc(int dmsHandle, int sockfd, int sockType);

#endif
