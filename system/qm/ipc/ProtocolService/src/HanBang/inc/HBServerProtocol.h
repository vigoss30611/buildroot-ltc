#ifndef HB_SERVER_PROTOCOL_H
#define HB_SERVER_PROTOCOL_H

void* HBSessionThreadRun_TestSo(void *arg);

int HBServiceInit(const int nHandle);
int HBServiceStart();
int HBServiceStop();

//
#include "NW_Common.h"

//int HBCreateServerSocket();
void HBServer_Cfg_Init(NW_Server_Cfg *para);

#endif