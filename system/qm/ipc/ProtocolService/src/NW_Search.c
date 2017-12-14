#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <string.h>
#include <unistd.h>

#include "NW_Search.h"
#include "QMAPIType.h"

//#ifdef Q3_PROTO_ONVIF
#include "onvifproc.h"
//#endif
//#ifdef FUNCTION_HIK_SUPPORT
#include "hik_search_protocol.h"
//#endif
#ifdef FUNCTION_TOPSEE_SUPPORT
#include "ts_search_protocol.h"
#endif
//#ifdef FUNCTION_XMAI_SUPPORT
#include "xmai_search_protocol.h"
//#endif
#ifdef FUNCTION_HANBANG_SUPPORT
#include "HBSearch.h"
#endif
//#ifdef FUNCTION_ZHINUO_SUPPORT
#include "zhinuo_search_protocol.h"
//#endif

typedef struct nw_search_nodetag
{
	int 	sockfd;
	int 	sockType; //0:UDP 1:raw socket
	int 	(*CreateSocket)(int * /*socktype*/);
	int 	(*ProcessRequest)(int /*dms_handle*/, int /*sockfd*/, int /*socktype*/);
	void 	(*StartProc)(); //启动时需要处理的任务，不需要的话直接放空
	void 	(*EndProc)(int /*sockfd*/); //结束时需要处理的任务(socket在外面关闭)，不需要的话直接放空
} nw_search_node;

#define MAX_SEARCH_POROTOCOL		7

static int max_search_node;
static nw_search_node search_node[MAX_SEARCH_POROTOCOL];
static int nw_search_module_deinit;

int CreateNormalThread(void* entry, void *para, pthread_t *pid);
static void *NW_Search_Service(void *para);

int g_nw_search_run_index = 0;


int CreateNormalThread(void* entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离
    if(pthread_create(&ThreadId, &attr, entry, para) == 0)//创建线程
    {
        pthread_attr_destroy(&attr);
        return 0;
    }

    pthread_attr_destroy(&attr);

    return -1;
}

static inline void NW_Search_Init()
{
	memset(search_node, 0, sizeof(search_node));
	max_search_node = 0;
	printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#ifdef Q3_PROTO_ONVIF
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = ONVIFSearchCreateSocket;
	search_node[max_search_node].ProcessRequest = OnvifSearchRequestProc;
	search_node[max_search_node].StartProc = onvif_send_hello;
	search_node[max_search_node].EndProc = onvif_send_bye;
	++max_search_node;
//#endif
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
//#ifdef FUNCTION_HIK_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = HikSearchCreateSocket;
	search_node[max_search_node].ProcessRequest = HikSearchRequsetProc;
	++max_search_node;
//#endif
#ifdef FUNCTION_TOPSEE_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = TsSearchCreateSocket;
	search_node[max_search_node].ProcessRequest = TsSearchRequsetProc;
	++max_search_node;
#endif
//#ifdef FUNCTION_XMAI_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = XMaiCreateSock;
	search_node[max_search_node].ProcessRequest = XMaiSearchRequestProc;
	search_node[max_search_node].EndProc = XMaiSearchSendEndMsg;
	++max_search_node;
//#endif
#ifdef FUNCTION_HANBANG_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = HBSearchCreateSocket;
	search_node[max_search_node].ProcessRequest = HBSearchRequestProc;
	++max_search_node;
#endif
//#ifdef FUNCTION_ZHINUO_SUPPORT
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = zhiNuo_CreateSearchSocket;
	search_node[max_search_node].ProcessRequest = zhiNuo_ProcessRequest;
	search_node[max_search_node].EndProc = zhiNuo_SendEndProc;
	++max_search_node;

	search_node[max_search_node].sockfd = -1;
	search_node[max_search_node].CreateSocket = daHua_CreateSearchSocket;
	search_node[max_search_node].ProcessRequest = daHua_ProcessRequest;
	search_node[max_search_node].EndProc = daHua_SendEndProc;
	++max_search_node;
//#endif
    printf("#### %s %d, max_search_node=%d\n", __FUNCTION__, __LINE__, max_search_node);
}

void StartNWSearch()
{
	nw_search_module_deinit = 0;
	NW_Search_Init();	
    printf("#### %s %d, max_search_node=%d\n", __FUNCTION__, __LINE__, max_search_node);
	CreateNormalThread(NW_Search_Service, NULL, NULL);
}

void StopNWSearch()
{
	nw_search_module_deinit = 1;
}

void *NW_Search_Service(void *para)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	prctl(PR_SET_NAME, (unsigned long)"NWSearchService", 0, 0, 0);

	int i;
	fd_set readSet;
	int maxfd;
	struct timeval tv;
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
	printf("[%s] start service!\n", __FUNCTION__);
	for (i = 0; i < max_search_node; i++)
	{
        printf("########################## %s %d\n", __FUNCTION__, __LINE__);
		if (search_node[i].StartProc)
		{
            printf("########################## %s %d\n", __FUNCTION__, __LINE__);
			search_node[i].StartProc();
		}
	}
	
	while (!nw_search_module_deinit)
	{
        //printf("########################## %s %d\n", __FUNCTION__, __LINE__);
		maxfd = -1;
		FD_ZERO(&readSet);
		for (i = 0; i < max_search_node; i++)
		{
			if (search_node[i].sockfd < 0)
			{
                printf("########################## %s %d\n", __FUNCTION__, __LINE__);
				search_node[i].sockfd = search_node[i].CreateSocket(&search_node[i].sockType);
			}
			
			if (search_node[i].sockfd > 0)
			{
				FD_SET(search_node[i].sockfd, &readSet);
				if (search_node[i].sockfd > maxfd)
				{
					maxfd = search_node[i].sockfd;
				}
			}
		}
        //printf("########################## %s %d\n", __FUNCTION__, __LINE__);
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		if (maxfd > 0 && select(maxfd + 1, &readSet, NULL, NULL, &tv) > 0)
		{
            //printf("########################## %s %d\n", __FUNCTION__, __LINE__);
			for (i = 0; i < max_search_node; i++)
			{
				if (search_node[i].sockfd > 0 && FD_ISSET(search_node[i].sockfd, &readSet))
				{
                    //printf("########################## %s %d\n", __FUNCTION__, __LINE__);
					if (search_node[i].ProcessRequest(0, search_node[i].sockfd, search_node[i].sockType) < 0)
					{
						close(search_node[i].sockfd);
						search_node[i].sockfd = -1;
					}
				}
			}
		}

		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		select(0, NULL, NULL, NULL, &tv);
	}

	for (i = 0; i < max_search_node; i++)
	{
		if (search_node[i].EndProc)
		{
			search_node[i].EndProc(search_node[i].sockfd);
		}

		if (search_node[i].sockfd > 0)
		{
			close(search_node[i].sockfd);
			search_node[i].sockfd = -1;
		}
	}
	
	return NULL;
}

