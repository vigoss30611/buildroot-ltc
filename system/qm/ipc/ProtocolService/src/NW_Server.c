#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "NW_Server.h"
#include "QMAPIType.h"
#include "NW_Common.h"
//#ifdef FUNCTION_HIK_SUPPORT
#include "hik_server_protocol.h"
//#endif
#ifdef FUNCTION_TOPSEE_SUPPORT
#include "ts_server_protocol.h"
#endif
//#ifdef FUNCTION_XMAI_SUPPORT
#include "xmai_server_protocol.h"
//#endif
#ifdef FUNCTION_HANBANG_SUPPORT
#include "HBServerProtocol.h"
#endif
//#ifdef FUNCTION_ZHINUO_SUPPORT
#include "zhinuo_server_protocol.h"
//#endif
#ifdef FUNCTION_RTSP_DLL_CONFIG
#include "rtsp_server_protocol.h"
#endif
//#include "sys_fun_interface.h"

#include "Onvif/inc/onvifproc.h"

typedef void *(*ThreadBodyFunc)(void *);
 
typedef struct NW_Server_Nodetag
{
	int 				sockfd;
	NW_CreateSockFunc	CreateSocket;

	pthread_mutex_t 	CountLock;
	int					UseCount;

	void *				Handle;
	void *				InitPara;
	const char *		InitFuncName;
	const char *		UnInitFuncName;	
	const char *		ThreadFucName;
	const char *		LibPath;
	ThreadBodyFunc		pThreadFunc;
} NW_Server_Node;

#define MAX_SERVER_NODE		6

static int nw_server_module_deinit;

static int g_MaxServerNode = 0;
static NW_Server_Node g_ServerNode[MAX_SERVER_NODE];
static DWORD g_dl_Timer[MAX_SERVER_NODE];

static inline void InitNWServer()
{
	g_MaxServerNode = 0;
	memset(g_ServerNode, 0, sizeof(g_ServerNode));
	memset(g_dl_Timer, 0, sizeof(g_dl_Timer));

	int i;
	for (i = 0; i < MAX_SERVER_NODE; i++)
	{
		pthread_mutex_init(&g_ServerNode[i].CountLock, NULL);
	}
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    Onvif_Init();
    printf("#### %s %d\n", __FUNCTION__, __LINE__);

    printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#if (defined(FUNCTION_RTSP_DLL_CONFIG) || defined(FUNCTION_HIK_SUPPORT) || defined(FUNCTION_TOPSEE_SUPPORT) || defined(FUNCTION_XMAI_SUPPORT) || defined(FUNCTION_HANBANG_SUPPORT) || defined(FUNCTION_ZHINUO_SUPPORT))
	NW_Server_Cfg cfg;
//#endif
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#ifdef FUNCTION_HIK_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	Hik_Server_Cfg_Init(&cfg);
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
//#endif

#ifdef FUNCTION_TOPSEE_SUPPORT
	ts_Server_Cfg_Init(&cfg);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
#endif
printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#ifdef FUNCTION_XMAI_SUPPORT
	xmai_Server_Cfg_Init(&cfg);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
//#endif
printf("#### %s %d\n", __FUNCTION__, __LINE__);
#ifdef FUNCTION_HANBANG_SUPPORT
	HBServer_Cfg_Init(&cfg);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
#endif
printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#ifdef FUNCTION_ZHINUO_SUPPORT
	zhiNuo_Server_Cfg_Init(&cfg);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
//#endif
printf("#### %s %d\n", __FUNCTION__, __LINE__);
#ifdef FUNCTION_RTSP_DLL_CONFIG
	printf("%s,%d rtsp step1 \n",__FILE__,__LINE__);
	rtsp_Server_Cfg_Init(&cfg);
	printf("%s,%d rtsp step2\n",__FILE__,__LINE__);
	g_ServerNode[g_MaxServerNode].sockfd = -1;
	g_ServerNode[g_MaxServerNode].CreateSocket = cfg.CreateSocket;
	g_ServerNode[g_MaxServerNode].InitPara = cfg.InitPara;
	g_ServerNode[g_MaxServerNode].InitFuncName = cfg.InitFuncName;
	g_ServerNode[g_MaxServerNode].UnInitFuncName = cfg.UnInitFuncName;	
	g_ServerNode[g_MaxServerNode].ThreadFucName = cfg.ThreadFuncName;
	g_ServerNode[g_MaxServerNode].LibPath = cfg.LibPath;
	++g_MaxServerNode;
#endif
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
}

extern int CreateNormalThread(void *entry, void *para, pthread_t *pid);
static void *NW_Server_Thread(void *para);

void StartNWServer()
{
	nw_server_module_deinit = 0;
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	InitNWServer();
	printf("%s,%d InitNWServer end\n",__FILE__,__LINE__);
	CreateNormalThread(NW_Server_Thread, NULL, NULL);
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
}

void StopNWServer()
{
	nw_server_module_deinit = 1;
	sleep(3);
}

void NW_Client_Logoff(int id)
{
	if (id >= 0 && id < g_MaxServerNode)
	{
		NW_Server_Node *pNode = &g_ServerNode[id];
		pthread_mutex_lock(&pNode->CountLock);
		if (--pNode->UseCount < 0)
		{
			pNode->UseCount = 0;
		}

		//printf("id:%d logoff, count is %d\n", id, pNode->UseCount);
		pthread_mutex_unlock(&pNode->CountLock);
	}
}

static BOOL NW_Is_Empty(int id)
{
	BOOL bIsEmpty = FALSE;
	
	pthread_mutex_lock(&g_ServerNode[id].CountLock);
	if (!g_ServerNode[id].UseCount && g_ServerNode[id].Handle)
	{
		bIsEmpty = TRUE;
	}
	pthread_mutex_unlock(&g_ServerNode[id].CountLock);

	return bIsEmpty;
}

static void NW_CloseDl(int id)
{
	pthread_mutex_lock(&g_ServerNode[id].CountLock);
	if (!g_ServerNode[id].UseCount && g_ServerNode[id].Handle)
	{
		if (g_ServerNode[id].UnInitFuncName)
		{
			dlerror();
			int (*uninit_func)();
			uninit_func = dlsym(g_ServerNode[id].Handle, g_ServerNode[id].UnInitFuncName);

			const char *error = dlerror();
			if (!error)
			{
				uninit_func();
			}
		}
		
		//int ret = sys_fun_dlclose(g_ServerNode[id].Handle);
		int ret = dlclose(g_ServerNode[id].Handle);
		g_ServerNode[id].Handle = NULL;
		g_ServerNode[id].pThreadFunc = NULL;

		if (!ret)
		{
			printf("[%s] dlclose id:%d ts:%ld handle success\n", __FUNCTION__, id, time(NULL));
		}
		else
		{
			printf("[%s] dlclose id:%d ts:%ld handle failed: %s\n", __FUNCTION__, id, time(NULL), dlerror());
		}
	}
	else
	{
		printf("[%s] dlclose type%d handle failed. count:%d handle:%p\n", __FUNCTION__, id, g_ServerNode[id].UseCount, g_ServerNode[id].Handle);
	}
    
	pthread_mutex_unlock(&g_ServerNode[id].CountLock);
}

static int NW_Server_Proc(int id, int sockfd, const struct sockaddr_in *pAddr)
{
	NW_Server_Node *pNode = &g_ServerNode[id];
	BOOL bInit = FALSE;
	
	char ipaddr_str[32] = {0};
	inet_ntop(AF_INET, &pAddr->sin_addr, ipaddr_str, 32);
	
	pthread_mutex_lock(&pNode->CountLock);
	++pNode->UseCount;
	printf("[%s] user login %s:%d. count is %d\n", __FUNCTION__, ipaddr_str, ntohs(pAddr->sin_port), pNode->UseCount);
	if (!pNode->Handle)
	{
		//pNode->Handle = sys_fun_dlopen(pNode->LibPath, RTLD_LAZY);
		pNode->Handle = dlopen(pNode->LibPath, RTLD_LAZY);
		printf("[%s] dlopen:%s %p\n", __FUNCTION__, pNode->LibPath, pNode->Handle);		
		if (!pNode->Handle)
		{
			if (--pNode->UseCount < 0)
			{
				pNode->UseCount = 0;
			}
			pthread_mutex_unlock(&pNode->CountLock);
			close(sockfd);
			return -1;
		}

		bInit = TRUE;
	}
	pthread_mutex_unlock(&pNode->CountLock);
	
	NW_ClientInfo *pInfo = NULL;
	do
	{
		char *error;
		if (bInit)
		{
			printf("[%s] ts:%lu dl init!\n", __FUNCTION__, time(NULL));
			int (*init_func)(void *);
			dlerror();
			init_func = dlsym(pNode->Handle, pNode->InitFuncName);
			
			error = dlerror();
			if (error != NULL)
			{
				//sys_fun_dlclose(pNode->Handle);
				dlclose(pNode->Handle);
				pNode->Handle = NULL;
				pNode->pThreadFunc = NULL;
			
				printf("[%s:%d] %s dlsym failed: %s %p.\n", __FUNCTION__, __LINE__, pNode->InitFuncName, error, init_func);
				break;
			}
			
			if (init_func(pNode->InitPara) < 0)
			{
				printf("[%s] lib:%s init failed!\n", __FUNCTION__, pNode->LibPath);

				//sys_fun_dlclose(pNode->Handle);
				dlclose(pNode->Handle);
				pNode->Handle = NULL;
				pNode->pThreadFunc = NULL;					
				break;
			}

			dlerror();
			pNode->pThreadFunc = dlsym(pNode->Handle, pNode->ThreadFucName);
			error = dlerror();
			if (error != NULL)
			{
				//sys_fun_dlclose(pNode->Handle);
				dlclose(pNode->Handle);
				pNode->Handle = NULL;
				pNode->pThreadFunc = NULL;
				
				printf("[%s:%d] %s dlsym failed: %s.\n", __FUNCTION__, __LINE__, pNode->LibPath, error);
				break;
			}			
		}
		
		pInfo = (NW_ClientInfo *)malloc(sizeof(NW_ClientInfo));
		if (!pInfo)
		{
			printf("[%s:%d] Malloc failed!\n", __FUNCTION__, __LINE__);
			break;
		}
		
		pInfo->DmsHandle = 0;
		pInfo->ClientSocket = sockfd;
		memcpy(&pInfo->ClientAddr, pAddr, sizeof(pInfo->ClientAddr));
		pInfo->id = id;
		pInfo->OnLogoff = NW_Client_Logoff;

	#if 0 //accept返回的套接字此属性不会被继承
		int val = fcntl(sockfd, F_GETFL);
		if (val & O_NONBLOCK)
		{
			printf("[%s] set noblock!!!\n", __FUNCTION__);
			val &= ~O_NONBLOCK;
			fcntl(sockfd, F_SETFL, val);//设置成默认的阻塞型
		}
	#endif
		
		if (CreateNormalThread(pNode->pThreadFunc, pInfo, NULL) != 0)
		{
			printf("[%s:%d] create pthread failed.\n", __FUNCTION__, __LINE__);
			break;
		}

		//success
		
		return 0;
	} while (0);
	
	close(sockfd);
	if (pInfo)
	{
		free(pInfo);
	}
	
	pthread_mutex_lock(&pNode->CountLock);
	if (--pNode->UseCount < 0)
	{
		pNode->UseCount = 0;
	}
	pthread_mutex_unlock(&pNode->CountLock);
	
	return -1;
}

void *NW_Server_Thread(void *para)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	prctl(PR_SET_NAME, (unsigned long)"NWServerThread", 0, 0, 0);
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	int i;
	fd_set readSet;
	int maxfd;
	struct timeval tv;
	struct sockaddr_in addr;
	socklen_t addrlen;
#ifndef __linux__
	struct timeval startTime, endTime;
#endif
	int timeTick;

	int sockfd;
	int ret;

	printf("[%s] start service!\n", __FUNCTION__);
	while (!nw_server_module_deinit)
	{
		maxfd = -1;
		FD_ZERO(&readSet);
        //printf("#### %s %d, g_MaxServerNode=%d\n", __FUNCTION__, __LINE__, g_MaxServerNode);
		for (i = 0; i < g_MaxServerNode; i++)
		{
            if (g_ServerNode[i].sockfd < 0)
            {
                printf("#### %s %d\n", __FUNCTION__, __LINE__);
                g_ServerNode[i].sockfd = g_ServerNode[i].CreateSocket();
                if (g_ServerNode[i].sockfd < 0)
                {
                    continue;
                }

                //新创建的监听socket需要设置为非阻塞模式
                fcntl(g_ServerNode[i].sockfd, F_SETFL, fcntl(g_ServerNode[i].sockfd, F_GETFL) | O_NONBLOCK);
            }

            FD_SET(g_ServerNode[i].sockfd, &readSet);
            if (g_ServerNode[i].sockfd > maxfd)
            {
                maxfd = g_ServerNode[i].sockfd;
            }
		}

		ret = 0;
		timeTick = 1;
		if (maxfd > 0)
		{
#ifndef __linux__
            gettimeofday(&startTime, NULL);
#endif

			tv.tv_sec = 3;
			tv.tv_usec = 0;		
			ret = select(maxfd + 1, &readSet, NULL, NULL, &tv);

#ifndef __linux__
            gettimeofday(&endTime, NULL);

            if ((endTime.tv_sec > startTime.tv_sec) || (endTime.tv_usec > startTime.tv_usec))
            {
                timeTick += (endTime.tv_sec * 1000 + endTime.tv_usec / 1000 - startTime.tv_sec * 1000 - startTime.tv_usec / 1000) / 10;
                printf("[%s]:%d timetick is %d\n", __FUNCTION__, __LINE__, timeTick);
            }
#else
            timeTick += (3000 - tv.tv_sec * 1000 - tv.tv_usec / 1000) / 10;
            //printf("[%s]:%d timetick is %d\n", __FUNCTION__, __LINE__, timeTick);
#endif		
		}
		
		for (i = 0; i < g_MaxServerNode; i++)
		{
			//printf("#### %s %d\n", __FUNCTION__, __LINE__);
			if (ret > 0 && g_ServerNode[i].sockfd > 0 && FD_ISSET(g_ServerNode[i].sockfd, &readSet))
			{
				g_dl_Timer[i] = 0;
				addrlen = sizeof(addr);
				sockfd = accept(g_ServerNode[i].sockfd, (struct sockaddr *)&addr, &addrlen);
				printf("accept-sockfd =%d i=%d\n",sockfd,i);
				if (sockfd > 0)
				{
					NW_Server_Proc(i, sockfd, &addr);
				}
				else if (errno != EINTR)
				{
					printf("[%s:%d] close socket:%d\nid:%d socket:%d errno:%d\n", __FUNCTION__, __LINE__, sockfd, i, g_ServerNode[i].sockfd, errno);
					close(g_ServerNode[i].sockfd);
					g_ServerNode[i].sockfd = -1;
				}
			}
			else if (NW_Is_Empty(i))
			{
				g_dl_Timer[i] += timeTick;
				if (g_dl_Timer[i] >= 3000)
				{
					NW_CloseDl(i);
					g_dl_Timer[i] = 0;
				}
			}
			else
			{
				g_dl_Timer[i] = 0;
			}
		}

		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		select(0, NULL, NULL, NULL, &tv);
	}

	for (i = 0; i < g_MaxServerNode; i++)
	{
		if (g_ServerNode[i].sockfd > 0)
		{
			close(g_ServerNode[i].sockfd);
			g_ServerNode[i].sockfd = -1;
		}
	}
	
	return NULL;
}

