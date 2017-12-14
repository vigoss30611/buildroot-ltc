#ifndef __SPV_SERVER_H__
#define __SPV_SERVER_H__

#include "spv_common.h"

#define FIFO_SERVER     "/tmp/myfifo" 

int UpdateIdleTime();

int InitInfotmServer(HMSG h);

void TerminateKeyServer();
#endif
