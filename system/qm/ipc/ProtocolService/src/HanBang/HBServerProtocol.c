#include "HBServerProtocol.h"
#include <stdio.h>
#include <sys/prctl.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include "GXConfig.h"
#include "GXNetwork.h"
#include "GXThread.h"
#include "HBBase.h"
#include "sys_fun_interface.h"


static HBServiceSession gHBSSessionSO;

static void *g_hanbanggk_dl_handle;
static int g_hanbanggk_dl_init_flag = 0;

void* HBCloseDLRun(void *param)
{
	int nSocketFd = (int)(param);

	const char strProcessMethod[] = "HBIsActivated";
	typedef int(*HBIsActivatedCB)(int);

	HBIsActivatedCB pProcess = dlsym(g_hanbanggk_dl_handle, strProcessMethod);
	char *error = dlerror();
	if (error != NULL) {
		printf("HBCloseDLRun failed. error:%s \n", error);

// 		dlclose(g_hanbanggk_dl_handle);
//		g_hanbanggk_dl_handle = NULL;
// 		g_hanbanggk_dl_init_flag = 0;
//		return NULL;
	}

	while (1)
	{
		sleep(120);

		int nRet = pProcess(nSocketFd);
		if (nRet == 0)
		{
			//sys_fun_dlclose(g_hanbanggk_dl_handle);
			dlclose(g_hanbanggk_dl_handle);
			g_hanbanggk_dl_handle = NULL;
			g_hanbanggk_dl_init_flag = 0;

			printf("HBCloseDLRun close success. \n");
			break;
		}
	}

	return NULL;
}

void* HBSessionThreadRun_TestSo(void *arg)
{
	GX_DEBUG("HBSessionThreadRun() begin. \n");
	prctl(PR_SET_NAME, (unsigned long)"HBSessionThreadRun", 0, 0, 0);

	HBServiceSession *pSSession = (HBServiceSession *)arg;
	if (NULL == pSSession){
		GX_DEBUG("input param error. \n");
		return NULL;
	}

	int socketfd = -1;
	socketfd = GXCreateSocket2(NULL, HB_SERVER_PORT, HB_MAX_USER_NUM);
	if (socketfd < 0){
		GX_DEBUG("GXCreateSocket2() failed. result is %d", socketfd);
		return NULL;
	}
	pSSession->socketfd = socketfd;

	int nClientFd = -1;
	unsigned int nClientIpAddr = 0;
	unsigned short nClientPort = 0;
//	int nLocalIp = inet_addr("127.0.0.1");

	while (pSSession->runState == kGXWorkRun)
	{
		nClientFd = GXAcceptClient(socketfd, &nClientIpAddr, &nClientPort);
		if (nClientFd < 0){
			continue;
		}
		HBClientInfo *pClientInfo = (HBClientInfo *)malloc(sizeof(HBClientInfo));
		if (NULL == pClientInfo){
			GX_DEBUG("HBClientInfo malloc failed.");
			continue;
		}
		pClientInfo->handleID = pSSession->handleID;
		pClientInfo->clientFd = nClientFd;
		pClientInfo->clientIpAddr = nClientIpAddr;
		pClientInfo->clientPort = nClientPort;

		//filte

		// loader .so
		if (g_hanbanggk_dl_init_flag == 0)
		{
			const char strLibPath[] = "/usr/netview/app/lib/libhbgk.so";
			const char strSetParamMethod[] = "HBSetServiceParam";
			typedef void(*SetServiceParam)(HBServiceSession *pSession);

			GX_DEBUG("g_hanbanggk_dl_handle = dlopen(strLibPath, RTLD_LAZY); ..............%s", strLibPath);
			//g_hanbanggk_dl_handle = sys_fun_dlopen(strLibPath, RTLD_LAZY);
			g_hanbanggk_dl_handle = dlopen(strLibPath, RTLD_LAZY);

			SetServiceParam SetParam = dlsym(g_hanbanggk_dl_handle, strSetParamMethod);
			char *error = NULL;
			error = dlerror();
			if (error != NULL) {
				GX_DEBUG("dlsym failed. error:%s", error);
				GXCloseSocket(nClientFd);

				//sys_fun_dlclose(g_hanbanggk_dl_handle);
				dlclose(g_hanbanggk_dl_handle);
				g_hanbanggk_dl_handle = NULL;
				g_hanbanggk_dl_init_flag = 0;

				continue;
			}

			SetParam(&gHBSSessionSO);
			g_hanbanggk_dl_init_flag = 1;

			pthread_t main_thread_id;
			GXThreadCreate2(&main_thread_id, HBCloseDLRun, pClientInfo);
		}		

		const char strMethod[] = "HBRequestHandleThreadRun";
		typedef void* (*RequestHandleThreadRun)(void *arg);
		RequestHandleThreadRun pRun = dlsym(g_hanbanggk_dl_handle, strMethod);
		char *error = dlerror();
		if (error != NULL) {
			GX_DEBUG("dlsym failed. error:%s", error);
			GXCloseSocket(nClientFd);
			continue;
		}

		pClientInfo->dl_handle = g_hanbanggk_dl_handle;
		int nRet = GXThreadCreate2(&pClientInfo->clientThreadId, pRun, pClientInfo);
//		int nRet = GXThreadCreate2(&pClientInfo->clientThreadId, HBRequestHandleThreadRun, pClientInfo);
		if (0 != nRet){
			GX_DEBUG("create thread failed. socketfd:%d", nClientFd);
			GXCloseSocket(nClientFd);
			break;
		}
	}

	GXCloseSocket(pSSession->socketfd);
	GX_DEBUG("HBSessionThreadRun() stop.");
	return NULL;
}

int HBServiceInit(const int nHandle)
{
	memset(&gHBSSessionSO, 0, sizeof(HBServiceSession));
	gHBSSessionSO.handleID = nHandle;
	GX_DEBUG("HBServiceInit():%d", nHandle);

	return kGXErrorCodeOK;
}


int HBServiceStart()
{
//	HBRegisterAlarm(GetHBSSessionInstance());

	signal(SIGPIPE, SIG_IGN);

	gHBSSessionSO.runState = kGXWorkRun;

	int nRet = GXThreadCreate2(&(gHBSSessionSO.threadId), HBSessionThreadRun_TestSo, &gHBSSessionSO);
	return nRet;
}

int HBServiceStop()
{
//	HBUnRegisterAlarm(&gHBSSessionSO);
	gHBSSessionSO.runState = kGXWorkClose;

	int index;
	for (index = 0; index < HB_MAX_USER_NUM; ++index)
	{
		HBCliecntSession *pCSession = &(gHBSSessionSO.clientSessions[index]);
		if (pCSession->token == 1){
			pCSession->runState = kGXWorkClose;
			pCSession->netStreams[0].runState = kGXWorkClose;
			pCSession->netStreams[1].runState = kGXWorkClose;
		}
	}

	return kGXErrorCodeOK;
}



static int HBCreateServerSocket()
{
	int socketfd = -1;
	socketfd = GXCreateSocket2(NULL, HB_SERVER_PORT, HB_MAX_USER_NUM);
	if (socketfd < 0){
		GX_DEBUG("GXCreateSocket2() failed. result is %d", socketfd);
		return -1;
	}
	return socketfd;
}

void HBServer_Cfg_Init(NW_Server_Cfg *para)
{
	HBServiceInit(0);

	para->CreateSocket = HBCreateServerSocket;
	para->InitPara = &gHBSSessionSO;
	para->InitFuncName = "HBSetServiceParam";	
	para->UnInitFuncName = "HBServiceStop";
	para->ThreadFuncName = "HBRequestHandleThreadRun";
	para->LibPath = "/usr/lib/libhbgk.so";	

	return;
}
