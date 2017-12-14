#ifndef _DISK_FULL_MNG_H_
#define _DISK_FULL_MNG_H_

#include "QMAPIErrno.h"

int GetFreeSpaceDisk(char *pPath);
void *HDFullDetectThread(void *param);


int disk_monitor_start(void);
int disk_monitor_stop(void);


#endif
