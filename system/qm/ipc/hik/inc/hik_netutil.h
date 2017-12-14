#ifndef _HIK_NETUTIL_H_
#define _HIK_NETUTIL_H_


#include "hik_command.h"
#include "QMAPIType.h"


int verifyNetClientOperation(NETCMD_HEADER *pHeader, struct sockaddr_in *pClientSockAddr, UINT32 operation);
int checkAlarmInNumValid(int alarmInNum);
int checkAlarmOutNumValid(int alarmOutNum);
BOOL isAnalogAlarmInNum(int alarmInNum);
BOOL isAnalogAlarmOutNum(int alarmOutNum);
BOOL isNetIpAlarmInNum(int alarmInNum);
BOOL isNetIpAlarmOutNum(int alarmOutNum);


#endif
