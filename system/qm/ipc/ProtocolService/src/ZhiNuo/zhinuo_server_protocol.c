
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <semaphore.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <linux/unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <sys/time.h>
#include <dlfcn.h>

#include "QMAPI.h"
#include "QMAPINetSdk.h"
#include "zn_dh_base_type.h"
#include "NW_Common.h"
#include "zhinuo_server_protocol.h"

static int g_main_sock = -1;
static int g_zhinuo_server_run_flag = 0;

static void *g_zhinuo_dl_handle;
static int g_zhinuo_dl_init_flag = 0;


void* ZhiNuo_CloseDLRun(void *param)
{
	int nSocketFd = (int)(param);

	const char strProcessMethod[] = "ZhiNuo_isActivated";
	typedef int(*ZhiNuo_isActivatedCB)(int);

	ZhiNuo_isActivatedCB pProcess = dlsym(g_zhinuo_dl_handle, strProcessMethod);
	char *error = dlerror();
	if (error != NULL) {
		printf("ZhiNuo_CloseDLRun failed. error:%s \n", error);	

// 		dlclose(g_zhinuo_dl_handle);
// 		g_zhinuo_dl_init_flag = 0;
//		return NULL;
	}

	while (1)
	{
		sleep(120);

		int nRet = pProcess(nSocketFd);
		if (nRet == 0)
		{
			//sys_fun_dlclose(g_zhinuo_dl_handle);
			dlclose(g_zhinuo_dl_handle);
			g_zhinuo_dl_init_flag = 0;

			printf("ZhiNuo_CloseDLRun close success. \n");
			break;
		}		
	}	

	return NULL;
}

void* ZhiNuo_Main_Thread(void *param)
{
	printf("ZhiNuo_Main_Thread enter====23==================\n");
	prctl(PR_SET_NAME, (unsigned long)"ZhiNuo_XMain_Thread", 0, 0, 0);
	printf("ZhiNuo_Main_Thread pid:%d tid\n", getpid());

	int n_len;
//	int i = 0;
//	int n_function_flag = 0;
	struct sockaddr_in t_serv_addr, t_clnt_addr;

	//创建监听会话的socket
	g_main_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_main_sock < 0)
	{
		printf("socket error%s\n", strerror(errno));
		goto end;
	}

	/*  配置监听会话的socket */
	t_serv_addr.sin_family = AF_INET;
	t_serv_addr.sin_port = htons(37777);
	t_serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(t_serv_addr.sin_zero), 8);

	/* 绑定监听会话的socket */
	if (bind(g_main_sock, (struct sockaddr *)&t_serv_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("bind err%s\n", strerror(errno));
		goto end;
	}

	/* 主动socket转为被动监听socket */
	if (listen(g_main_sock, 1) < 0)
	{
		printf("listen error%s\n", strerror(errno));
		goto end;
	}

	/* 循环监听，等待nvr主动连接 */
	n_len = sizeof(t_clnt_addr);
	while (1)
	{
		int n_peer;
		printf("begin to accept\n");
		n_peer = accept(g_main_sock, (struct sockaddr *)&t_clnt_addr, (socklen_t *)&n_len);/* 接收连接 */

		if (0 == g_zhinuo_server_run_flag)
		{
			break;
		}

		if (n_peer < 0)
		{
			printf("Server: accept failed%s\n", strerror(errno));
			continue;
		}

// 		if (0 == n_function_flag)
// 		{
// 			ZhiNuo_Function_Thread();
// 			n_function_flag = 1;
// 		}

		printf("Entry ZhiNuo_Session_Thread ，after to accept:socket:%d============remote ip:%s,remote port:%d\n",
			  n_peer, (char *)inet_ntoa(t_clnt_addr.sin_addr), t_clnt_addr.sin_port);		

		// load .so
		int nClientFd = n_peer;

		if (g_zhinuo_dl_init_flag == 0)
		{
			const char strLibPath[] = "/usr/lib/libzhinuo.so";
			const char strInitParamMethod[] = "lib_zhinuo_start";
			typedef int(*lib_zhinuo_startCB)();

			printf("void *dl_handle = dlopen(strLibPath, RTLD_LAZY); ..............%s \n", strLibPath);
			//g_zhinuo_dl_handle = sys_fun_dlopen(strLibPath, RTLD_LAZY);
			g_zhinuo_dl_handle = dlopen(strLibPath, RTLD_LAZY);

			lib_zhinuo_startCB pInitParam = dlsym(g_zhinuo_dl_handle, strInitParamMethod);
			char *error = NULL;
			error = dlerror();
			if (error != NULL) {
				printf("dlsym failed. error:%s \n", error);
				close(nClientFd);

				//sys_fun_dlclose(g_zhinuo_dl_handle);
				dlclose(g_zhinuo_dl_handle);
				g_zhinuo_dl_handle = NULL;
				g_zhinuo_dl_init_flag = 0;

				continue;
			}

			pInitParam();
			g_zhinuo_dl_init_flag = 1;

			pthread_t main_thread_id;
			pthread_attr_t attr;
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			pthread_create(&main_thread_id, &attr, ZhiNuo_CloseDLRun, (void *)nClientFd);
		}		

		const char strProcessMethod[] = "ZhiNuo_Session_Thread";
		typedef void* (*ZhiNuo_Session_ThreadCB)(void *arg);

		ZhiNuo_Session_ThreadCB pProcess = dlsym(g_zhinuo_dl_handle, strProcessMethod);
		char *error = dlerror();
		if (error != NULL) {
			printf("dlsym failed. error:%s \n", error);
			close(nClientFd);
			continue;
		}

		//创建会话线程，每有一个nvr连接就创建一个会话线程
		pthread_t main_thread_id;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		int nRet = pthread_create(&main_thread_id, &attr, pProcess, (void *)n_peer);
		if (0 != nRet)
//		if (0 != pthread_create(&main_thread_id, &attr, ZhiNuo_Session_Thread, (void *)n_peer))
		{
			printf("erro:creat ZhiNuo_Session_Thread fail\n");
//			pthread_create(&main_thread_id, &attr, pProcess, (void *)n_peer);
			break;
		}
	}
	printf("ZhiNuo_Main_Thread end !\n");

end:
	close(g_main_sock);
	g_main_sock = -1;
	
	return NULL;
}


////////////////////////////////////////////////////////////////////////////
int zhiNuo_server_init()
{
	return 0;
}

int zhiNuo_server_uninit()
{
	return 0;
}

int zhiNuo_server_start()
{	
	printf("%s line:%d ======= Entry lib_zhinuo_start(%s %s) \n", __FUNCTION__, __LINE__, __DATE__, __TIME__);

	g_zhinuo_server_run_flag = 1;

	pthread_t main_thread_id;
	pthread_attr_t attr;	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (0 != pthread_create(&main_thread_id, &attr, ZhiNuo_Main_Thread, NULL))
	{
		printf("erro:creat zhinuo_main_thread fail\n");
		return 0;
	}
	
	return 0;
}

int zhiNuo_server_stop()
{
	g_zhinuo_server_run_flag = 0;
	
	return 0;
}

int zhiNuo_get_use_count()
{
	return 0;
}

int zhiNuo_default_use_mem_size()
{
	return 65536; //64*1024;
}



////////////////////////////////////////////////////////////////////////
static int zhiNuo_CreateServerSocket()
{
	printf(".............zhiNuo_CreateSearchSocket()..........\n");
	
	struct sockaddr_in t_serv_addr;

	//创建监听会话的socket
	int g_main_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_main_sock < 0)
	{
		printf("socket error%s\n", strerror(errno));
		return -1;
	}

	int opt = 1;
	int nRet = setsockopt(g_main_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (nRet < 0){
		goto end_v2;
	}

	/*  配置监听会话的socket */
	t_serv_addr.sin_family = AF_INET;
	t_serv_addr.sin_port = htons(37777);
	t_serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(t_serv_addr.sin_zero), 8);

	/* 绑定监听会话的socket */
	if (bind(g_main_sock, (struct sockaddr *)&t_serv_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("bind err%s\n", strerror(errno));
		goto end_v2;
	}

	/* 主动socket转为被动监听socket */
	if (listen(g_main_sock, 1) < 0)
	{
		printf("listen error%s\n", strerror(errno));
		goto end_v2;
	}
	return g_main_sock;

end_v2:
	close(g_main_sock);
	g_main_sock = -1;

	return -1;
}


void zhiNuo_Server_Cfg_Init(NW_Server_Cfg *para)
{
	para->CreateSocket = zhiNuo_CreateServerSocket;
	para->InitPara = NULL;
	para->InitFuncName = "lib_zhinuo_init";
	para->UnInitFuncName = "lib_zhinuo_stop";	
	para->ThreadFuncName = "ZhiNuo_Session_Thread";
	para->LibPath = "/usr/lib/libzhinuo.so";	
}

