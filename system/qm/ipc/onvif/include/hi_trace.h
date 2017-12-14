
#ifndef __TRACE_H__
#define __TRACE_H__

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define TRACE_LEVEL_FATAL     0x00
#define TRACE_LEVEL_ERROR     0x01
#define TRACE_LEVEL_WARN      0x02
#define TRACE_LEVEL_INFO      0x03

#define TRACE_LEVEL_DEBUG     0x04
#define TRACE_LEVEL_DEBUG1    0x05
#define TRACE_LEVEL_DEBUG2    0x06

#define TRACE_LEVEL_ALL       0x07
#define TRACE_LEVEL_NONE      0x00

//int trace_level = TRACE_LEVEL_DEBUG;

#define  HI_TRACE(level,format...) \
{ \
   if(level <= TRACE_LEVEL_INFO) \
	{ \
		  printf("%s:%d  %s()", __FILE__,__LINE__,__FUNCTION__); \
          printf(format); \
	} \
}



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
