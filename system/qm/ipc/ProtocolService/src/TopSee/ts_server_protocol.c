#include "ts_server_protocol.h"
#include "sys_fun_interface.h"

static int g_ts_handle = -1;

int ts_server_stop()
{
	void *handle = NULL;
	int (*pTSModuleExit)( );

	handle = dlopen("/usr/lib/libts.so",RTLD_LAZY);
	if (!handle) 
	{	
		SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
		return -1;
	}  
	
	pTSModuleExit = dlsym(handle,"TSModuleExit");
	if(dlerror() != NULL)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
		//sys_fun_dlclose(handle);
		dlclose(handle);
		return -1;
	}
	
	int ret = (*pTSModuleExit)( );
	if(ret != 0)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d,search handle err:%s\n", __FUNCTION__, __LINE__, dlerror());
	}
	
	//sys_fun_dlclose(handle);
	dlclose(handle);

	return 0;
}

static int ts_CreateServerSocket()
{
	int fListenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (fListenfd < 0) 
	{
		SYSTEMLOG(SLOG_ERROR, "socket error%s\n", strerror(errno));
		return -1;
	}
			
	struct sockaddr_in t_serv_addr;	
	bzero(&t_serv_addr, sizeof(t_serv_addr));
	t_serv_addr.sin_family = AF_INET;
	t_serv_addr.sin_port = htons(8091); 	//ÔÝÊ±¹Ì¶¨¶Ë¿Ú
	t_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fListenfd, (struct sockaddr *)&t_serv_addr, sizeof(struct sockaddr)) < 0) 
	{
		SYSTEMLOG(SLOG_ERROR, "bind err%s\n", strerror(errno));
		goto end;
	}
		
	if (listen(fListenfd, 10) < 0)		
	{
		SYSTEMLOG(SLOG_ERROR, "listen error%s\n", strerror(errno));
		goto end;
	}

	return fListenfd;
end:
	close(fListenfd);
	fListenfd = -1;

	return -1;
}

static int ts_ServiceInit()
{
	g_ts_handle = dms_sysnetapi_open(DMS_NETPT_TOPSEE);
	DPRINT("handle111:%d\n",g_ts_handle);

	return 0;
}

int ts_Server_Cfg_Init(NW_Server_Cfg *para)
{
	ts_ServiceInit();

	para->CreateSocket = ts_CreateServerSocket;
	para->InitPara = &g_ts_handle;
	para->InitFuncName = "lib_ts_init";
	para->UnInitFuncName = "lib_ts_uninit";
	para->ThreadFuncName = "ts_Session_Thread";
	para->LibPath = "/usr/lib/libts.so";	

	return 0;
}
