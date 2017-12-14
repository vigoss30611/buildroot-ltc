#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>

#include "sysconf.h"

/*
GS_TRACE LEVEL:
	LOG_DEBUG
	LOG_INFO
	LOG_WARNING
	LOG_ERR
*/

extern int * pglevel;
#define  GS_TRACE(level, fmt, args...) \
do{ \
	if((*pglevel > 0)&&(*pglevel <= 4)) \
	{ \
		syslog(level, "%s():%i: " fmt, __FUNCTION__, __LINE__, ## args); \
	} \
  } while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
