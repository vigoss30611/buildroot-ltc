#include <dlfcn.h> 
#include <pthread.h>
#include "Rtsp_server_protocol.h"
#include "jb_common.h"
#include "cgiinterface.h"
#include "sys_fun_interface.h"





//MT_CONFIG_S  g_MTConfig;
    
static MT_CONFIG_S *g_pMTConfig = NULL;//&g_MTConfig;


static int rtsp_CreateServerSocket()
{
	int ret;
	int Listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Listenfd < 0) 
	{
		SYSTEMLOG(SLOG_ERROR, "socket error%s\n", strerror(errno));
		return -1;
	}
			
	struct sockaddr_in serv_addr;	
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(554); 	//暂时固定端口
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int opt = 1;
	ret = setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (0 != ret)
	{
		//HPRINT_ERR("\n");
		goto end;
	}

	if (bind(Listenfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{
		SYSTEMLOG(SLOG_ERROR, "bind err%s\n", strerror(errno));
		goto end;
	}
		
	if (listen(Listenfd, 10) < 0)		
	{
		SYSTEMLOG(SLOG_ERROR, "listen error%s\n", strerror(errno));
		goto end;
	}
	return Listenfd;
end:
	close(Listenfd);
	Listenfd = -1;

	return -1;
}


#if 0
int MTMNGGetCfgFrmIni(MT_CONFIG_S* pstMtCfg)
{
    memset(pstMtCfg, 0x00, sizeof(MT_CONFIG_S));

    pstMtCfg->rtspsvr.bEnable = 1;
    pstMtCfg->rtspsvr.listen_port = g_jb_globel_param.stRtspCfg.dwPort;
    pstMtCfg->rtspsvr.max_connections = 32;
    pstMtCfg->rtspsvr.udp_send_port_max = 7000;
    pstMtCfg->rtspsvr.udp_send_port_min = 6000;
    pstMtCfg->rtspOhttpsvr.bEnable = 1;
    //pstMtCfg->rtspOhttpsvr.listen_port = 553;
    pstMtCfg->rtspOhttpsvr.max_connections = 10;
    pstMtCfg->packet_len = 512;

    return 0;
}

int HI_MTMng_Init()
{
    int s32Ret = 0;
    MT_CONFIG_S stMtCfg;
    int i = 0, j = 0,k = 0;
    
    memset(&stMtCfg,0,sizeof(MT_CONFIG_S));

    s32Ret = MTMNGGetCfgFrmIni(&stMtCfg);
    if(0 != s32Ret)
    {
        return s32Ret;
    }

    stMtCfg.chn_cnt = 0;
    k = 0;
    for(i=0; i<g_ChannelCount; i++)//g_jb_globel_param.Jb_Server_Info.wChannelNum
    {
        stMtCfg.chn_cnt += DMS_MAX_STREAM_TYPE_NUM;
        for(j = 0;j< DMS_MAX_STREAM_TYPE_NUM; j++)
        {
            stMtCfg.mbuf[k].chnid = (i+1)*10+j+1;
            stMtCfg.mbuf[k].max_connect_num = 2;
            k++;
        }
    }
    /*初始化回调函数*/
    SYSTEMLOG(SLOG_TRACE, "MTMNT Init cfg chnid %d , max connect num %d, bufsize %d,wChannelNum:%d\n ",
					        stMtCfg.mbuf[0].chnid,stMtCfg.mbuf[0].max_connect_num,
					        stMtCfg.mbuf[0].buf_size,g_ChannelCount);//g_jb_globel_param.Jb_Server_Info.wChannelNum);
    DEVICE_TYPE_E enDeviceType = jb_get_device_type();
    if((enDeviceType == DEVICE_MEGA_5M) || (enDeviceType == DEVICE_MEGA_300M) || (enDeviceType == DEVICE_MEGA_IPCAM)
        || (enDeviceType == DEVICE_MEGA_1080P) || (enDeviceType == DEVICE_MEGA_960P)
        || (enDeviceType == DEVICE_MEGA_4M))
    {
    	stMtCfg.enLiveMode =  LIVE_MODE_1CHN1USER;
    	stMtCfg.maxUserNum = 2*g_ChannelCount*DMS_MAX_STREAM_TYPE_NUM;//g_jb_globel_param.Jb_Server_Info.wChannelNum
        stMtCfg.maxFrameLen = 2*1024*1024;//暂时搞大
        if(CHIP_Hi3518E == DMS_DEV_GetChipType())
        {
            stMtCfg.maxFrameLen = 768*1024;
        }
		if(CHIP_Hi3518EV200 == DMS_DEV_GetChipType())
        {
            stMtCfg.maxFrameLen = 768*1024;
        }
    	//stMtCfg.packet_len = 4*1024;//1428;
    	stMtCfg.packet_len = 1452;
    }
    else
    {
    	stMtCfg.enLiveMode = LIVE_MODE_1CHN1USER;
    	stMtCfg.maxUserNum = 2*g_ChannelCount*DMS_MAX_STREAM_TYPE_NUM;//g_jb_globel_param.Jb_Server_Info.wChannelNum
      	stMtCfg.maxFrameLen = 1024*1024;
    	//stMtCfg.packet_len = 4*1024;//1428;
    	stMtCfg.packet_len = 1452;
    }
    SYSTEMLOG(SLOG_TRACE, "the live mode is:%d\n",stMtCfg.enLiveMode);
     
    s32Ret =  HI_MT_Init(&stMtCfg);
    if(0 != s32Ret)
    {
        return s32Ret;
    }

    HI_MT_StartSvr();

    return 0;
}

#endif

static int Rtsp_ServiceInit()
{
	g_pMTConfig = (MT_CONFIG_S *)malloc(sizeof(MT_CONFIG_S));
	memset(g_pMTConfig ,0, sizeof(MT_CONFIG_S));
	
	
	g_pMTConfig->rtspsvr.bEnable = 1;
	g_pMTConfig->rtspsvr.listen_port = 554;//g_jb_globel_param.stRtspCfg.dwPort;
	g_pMTConfig->rtspsvr.max_connections = 32;
	g_pMTConfig->rtspsvr.udp_send_port_max = 7000;
	g_pMTConfig->rtspsvr.udp_send_port_min = 6000;
	g_pMTConfig->rtspOhttpsvr.bEnable = 1;
		//pstMtCfg->rtspOhttpsvr.listen_port = 553;
	g_pMTConfig->rtspOhttpsvr.max_connections = 10;
	g_pMTConfig->packet_len = 512;

	g_pMTConfig->chn_cnt = 0;
    int k = 0;
	int i,j;
	
    for(i=0; i<g_ChannelCount; i++)//g_jb_globel_param.Jb_Server_Info.wChannelNum
    {
        g_pMTConfig->chn_cnt += DMS_MAX_STREAM_TYPE_NUM;
        for(j = 0;j< DMS_MAX_STREAM_TYPE_NUM; j++)
        {
            g_pMTConfig->mbuf[k].chnid = (i+1)*10+j+1;
            g_pMTConfig->mbuf[k].max_connect_num = 2;
            k++;
        }
    }
/*
	DEVICE_TYPE_E enDeviceType = jb_get_device_type();
    if((enDeviceType == DEVICE_MEGA_5M) || (enDeviceType == DEVICE_MEGA_300M) || (enDeviceType == DEVICE_MEGA_IPCAM)
        || (enDeviceType == DEVICE_MEGA_1080P) || (enDeviceType == DEVICE_MEGA_960P)
        || (enDeviceType == DEVICE_MEGA_4M))
    {
    	g_pMTConfig->enLiveMode =  LIVE_MODE_1CHN1USER;
    	g_pMTConfig->maxUserNum = 2*g_ChannelCount*DMS_MAX_STREAM_TYPE_NUM;//g_jb_globel_param.Jb_Server_Info.wChannelNum
        g_pMTConfig->maxFrameLen = 2*1024*1024;//暂时搞大
        if(CHIP_Hi3518E == DMS_DEV_GetChipType())
        {
            g_pMTConfig->maxFrameLen = 768*1024;
        }
		if(CHIP_Hi3518EV200 == DMS_DEV_GetChipType())
        {
            g_pMTConfig->maxFrameLen = 768*1024;
        }
    	//stMtCfg.packet_len = 4*1024;//1428;
    	g_pMTConfig->packet_len = 1452;
    }
    else
*/		
    {
    	g_pMTConfig->enLiveMode = LIVE_MODE_1CHN1USER;
    	g_pMTConfig->maxUserNum = 2*g_ChannelCount*DMS_MAX_STREAM_TYPE_NUM;//g_jb_globel_param.Jb_Server_Info.wChannelNum
      	g_pMTConfig->maxFrameLen = 1024*1024;
    	//stMtCfg.packet_len = 4*1024;//1428;
    	g_pMTConfig->packet_len = 1452;
    }
	return 0;
}



int rtsp_Server_Cfg_Init(NW_Server_Cfg *para)
{
	
	printf("%s,%d rtsp init \n",__FILE__,__LINE__);
	Rtsp_ServiceInit();
	printf("%s,%d rtsp init end\n",__FILE__,__LINE__);
	para->CreateSocket = rtsp_CreateServerSocket;
	printf("%s,%d rtsp createServersocket end %d \n",__FILE__,__LINE__,(int)g_pMTConfig);
	para->InitPara = g_pMTConfig;
	para->InitFuncName = "lib_rtsp_init";
	para->UnInitFuncName = "lib_rtsp_uninit";
	para->ThreadFuncName = "rtsp_Session_Thread";
	para->LibPath = "/usr/lib/libmtran.so";	

	return 0;
}

int rtsp_ServiceDeinit()
{
	void *handle = NULL;
	int (*lib_rtsp_stopsvr)( );
	//printf("strat rtsp_ServiceDeinit\n");
	handle = dlopen("/usr/lib/libmtran.so",RTLD_LAZY);
	//printf("strat rtsp_ServiceDeinit step1  handle=%d\n",(int)handle);
	if (!handle) 
	{	
		SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
		return -1;
	}  
	
	lib_rtsp_stopsvr = dlsym(handle,"lib_rtsp_stopsvr");
	//printf("strat rtsp_ServiceDeinit step2  lib_rtsp_stopsvr=%d\n",(int)lib_rtsp_stopsvr);
	if(dlerror() != NULL)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
		//sys_fun_dlclose(handle);
		dlclose(handle);
		return -1;
	}
	
	
	int ret = (*lib_rtsp_stopsvr)( );
	//printf("strat rtsp_ServiceDeinit step3  ret=%d\n",ret);
	if(ret != 0)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d,search handle err:%s\n", __FUNCTION__, __LINE__, dlerror());
	}
	
	//sys_fun_dlclose(handle);
	dlclose(handle);
	return 0;
}

static void * gs_handle_rtspdll = NULL;
static int rtspdllCount = 0;
pthread_mutex_t  rtspdllcount_mutex;

int init_rtspdll_overhttp()
{
	return pthread_mutex_init(&rtspdllcount_mutex,NULL);
}
int DeInit_rtspdll_overhttp()
{
	return pthread_mutex_destroy(&rtspdllcount_mutex);
}

static int RTSPoHTTP_Link(httpd_conn* hc)
{
	char *error; 

	int (*lib_rtspOhttpsvr_init)(int);
	int (*lib_rtspOhttpsvr_start)(httpd_conn*);
	int (*lib_rtspOhttpsvr_DeInit)();
	if(NULL == gs_handle_rtspdll)
	{      
		gs_handle_rtspdll = dlopen("/usr/lib/libmtran.so", RTLD_LAZY);
		if (!gs_handle_rtspdll)
		{
			fprintf(stderr, "%s\n", dlerror());
			return -1;
		}		
		
		dlerror();
		lib_rtspOhttpsvr_init = dlsym(gs_handle_rtspdll, "lib_rtspOhttpsvr_init");
		error = dlerror();
		if(error)
		{
			printf("lib load error:%s\n", error);
			//sys_fun_dlclose(gs_handle_rtspdll);
			dlclose(gs_handle_rtspdll);
			gs_handle_rtspdll = NULL;
			return -1;
		}
		lib_rtspOhttpsvr_init(32);
	}
	
	dlerror();
	
	lib_rtspOhttpsvr_start = dlsym(gs_handle_rtspdll ,"lib_rtspOhttpsvr_start");
	error = dlerror();
	if (error)
	{
		printf("lib load error %s\n",error);
		//sys_fun_dlclose(gs_handle_rtspdll);
		dlclose(gs_handle_rtspdll);
		gs_handle_rtspdll = NULL;
		return -1;
	}

	dlerror();

	lib_rtspOhttpsvr_DeInit = dlsym(gs_handle_rtspdll,"lib_rtspOhttpsvr_DeInit");
	error = dlerror();
	if (error)
	{
		printf("lib load error %s\n",error);
		//sys_fun_dlclose(gs_handle_rtspdll);
		dlclose(gs_handle_rtspdll);
		gs_handle_rtspdll = NULL;
		return -1;	
	}
	pthread_mutex_lock(&rtspdllcount_mutex);
	rtspdllCount ++;
	pthread_mutex_unlock(&rtspdllcount_mutex);
	lib_rtspOhttpsvr_start(hc);
	pthread_mutex_lock(&rtspdllcount_mutex);
	rtspdllCount--;
	pthread_mutex_unlock(&rtspdllcount_mutex);
	if (rtspdllCount <=0) 
		lib_rtspOhttpsvr_DeInit();
	
	return 0;
}

void rtsp_overhttp_reg()
{
	HI_S_ICGI_Proc struProc;
	init_rtspdll_overhttp();
    memset(&struProc, 0, sizeof(HI_S_ICGI_Proc));
    strcpy(struProc.pfunType,"rtsp");
    struProc.pfunProc = RTSPoHTTP_Link;//MT_RTSPoHTTP_DistribLink;

    HI_ICGI_Register_Proc(&struProc);
	
}




