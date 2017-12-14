#ifndef 	__LOG_H__
#define 	__LOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum TagSYSLOGLEVEL
{
	SLOG_ERROR,
	SLOG_WARNING,
	SLOG_TRACE,	
	SLOG_DEBUG,  
}SYSLOGLEVEL_E;

/*
函数：日志模块初始化，在一个应用程序中只要初始化一次。
变量：
		server_ip:日志服务器的ip地址，值等于"0.0.0.0" 或者 "127.0.0.1" 或者为空指针都表示本地
*/
int SystemLogInit(char *server_ip);
/*
函数：输出日志
变量：
	level：日志等级,取值范围SYSLOGLEVEL_E
	format:日志输出信息，格式同printf
*/
int SYSTEMLOG(int level, const char *format, ...);
/*
 日志模块反初始化
*/
int SystemLogUnInit();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

