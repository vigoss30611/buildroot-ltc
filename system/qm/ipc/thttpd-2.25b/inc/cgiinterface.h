#ifndef _CGIINTERFACE_H_
#define _CGIINTERFACE_H_

#include "libhttpd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define MAX_CNT_CMD 4098 /*命令行的最大长度*/
#define MAX_CNT_CGI 5120 /*cgi接口中所有参数长度和的最大值*/
#define MAX_CNT_CGI_OUT 2048 /*cgi接口的返回字符串的最大长度*/
#define MAX_NUM_CMD 128  /*命令行模块的函数接口的最大参数个数*/
#define MAX_CNT_EVERYCMD 1024 /*每个参数的最大长度*/
#define MAX_CGI_INTER 64
#define MAX_CMD_INTER 128
#define MAX_LEN_URL 128

typedef int (*PTR_FUNC_ICGI_PROC)( httpd_conn* hc );


typedef struct
{
	char 	  pfunType[32];
	void		 *pfunProc;
} HI_S_ICGI_Proc;


void sendReflashToConn( httpd_conn* hc ,char* pszUrlAddr);
int ANV_CGI_Proc( httpd_conn* hc );
int HI_ICGI_Register_Proc(HI_S_ICGI_Proc* pstruProc);
int HI_ICGI_DeRegister_Proc(HI_S_ICGI_Proc* pstruProc);
void HI_CGI_strdecode( char* to, char* from );
/*解析字符串到数组*/
int HI_WEB_RequestParse_stru(char* hi_pQueryIn, int* argc1, char** argv1, 
                            int maxCnt, int maxLen);

int ANV_CGI_DumpHttpConn(httpd_conn* hc );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

