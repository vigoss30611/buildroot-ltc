#ifndef _NW_COMMON_H_
#define _NW_COMMON_H_

#include <netinet/in.h>

typedef void (*NW_LogoffFunc)(int id);
typedef int (*NW_CreateSockFunc)();

typedef struct NW_ClientInfotag
{
	int 				DmsHandle;
	int 				ClientSocket;
	struct sockaddr_in	ClientAddr;
	int					id;
	NW_LogoffFunc 		OnLogoff; //在客户端退出时调用这个函数
}NW_ClientInfo;

typedef struct NW_Server_Cfgtag
{
	NW_CreateSockFunc	CreateSocket;
	void *				InitPara;	   		//需要传给InitFuncName的参数
	const char *		InitFuncName;		//初始化库的函数名
	const char *		UnInitFuncName;	
	const char *		ThreadFuncName;	//创建服务的函数名
	const char *		LibPath;			//库路径
} NW_Server_Cfg;

//参考函数声明
//void NW_Server_Cfg_Init(NW_Server_Cfg *para);

#endif

