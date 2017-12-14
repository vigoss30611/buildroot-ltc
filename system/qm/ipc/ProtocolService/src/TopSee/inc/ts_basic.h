#ifndef __TS_BASIC_H__
#define __TS_BASIC_H__

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>

#include <netinet/in.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <linux/unistd.h>
#include <errno.h>
#include <ctype.h>

#include "DMSDataType.h"
#include "DMSNetSdk.h"
#include "DMSErrno.h"
#include "Dms_sysnetapi.h"
#include "xmlparse.h"
#include "libsyslogapi.h"

#define gettid() syscall(__NR_gettid)
#define DPRINT(str,arg...)		printf("%s(%d) "str,__FUNCTION__,__LINE__,##arg)

#endif
