/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <time.h>
#include <sys/soundcard.h>
#include <sys/prctl.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/reboot.h>

#include "system_msg.h"
#include "hd_mng.h"
#include "QMAPI.h"
#include "QMAPIRecord.h"
#include "QMAPIAlarmServer.h"
#include "Q3Audio.h"
#include "QMAPIAV.h"
#include "basic.h"
#include "QMAPINetWork.h"
#include "sysconfig.h"
#include "systime.h"
#include "tl_authen_interface.h"
#include "VideoMarker.h"
#include "QMAPIUpgrade.h"
#include "libCamera.h"
#include "q3_wireless.h"
#include "checkip.h"

#include "NW_Search.h"
#include "NW_Server.h"

#include <qsdk/videobox.h>

#ifdef Q3_THTTPD_SERVER
#include "thttpd.h"
#endif

#ifdef Q3_RTSP_SERVER
#include "hi_mt_def.h"
#include "hi_mt.h"
#endif

#ifdef Q3_NET_SERVER
#include "tlnetinterface.h"
#endif

#ifdef Q3_PPCS_P2P
#include "ppcs.h"
#endif

#ifdef Q3_QVT_P2P
#include "qvt_p2p.h"
#endif

//VERSION
#ifndef SOFTWARER_VERSION_MODIFY_NUM
#define SOFTWARER_VERSION_MODIFY_NUM	0x00000090
#endif

int AVHandle, AudioHandle, AlarmHandle, RecordHandle;
int AudioPromptHandle = 0;

/*
   -------------------------------------------------------------------------
       SIGHUP        1       Term    Hangup detected on controlling terminal
                                     or death of controlling process
       SIGINT        2       Term    Interrupt from keyboard
       SIGQUIT       3       Core    Quit from keyboard
       SIGILL        4       Core    Illegal Instruction
       SIGABRT       6       Core    Abort signal from abort(3)
       SIGFPE        8       Core    Floating point exception
       SIGKILL       9       Term    Kill signal
       SIGSEGV      11       Core    Invalid memory reference
       SIGPIPE      13       Term    Broken pipe: write to pipe with no readers
       SIGALRM      14       Term    Timer signal from alarm(2)
       SIGTERM      15       Term    Termination signal
       SIGUSR1   30,10,16    Term    User-defined signal 1
       SIGUSR2   31,12,17    Term    User-defined signal 2
       SIGCHLD   20,17,18    Ign     Child stopped or terminated
       SIGCONT   19,18,25            Continue if stopped
       SIGSTOP   17,19,23    Stop    Stop process
       SIGTSTP   18,20,24    Stop    Stop typed at tty
       SIGTTIN   21,21,26    Stop    tty input for background process
       SIGTTOU   22,22,27    Stop    tty output for background process
*/
void SignalHander(int signal)
{
    static BOOL bExit = FALSE;
    char name[32];
    if(bExit == FALSE)
    {
        bExit = TRUE;
        printf("____________Application will exit by signal:%d,pid:%d\n",signal,getpid());
        prctl(PR_GET_NAME, (unsigned long)name);
        printf("____________Application exit thread name %s\n",name);
        if(signal == SIGSEGV)
        {
            SIG_DFL(signal);
        }
        else
        {
            exit(1);
        }
    }
}


/*********************************************************************
    Function:
    Description:
        捕捉所有的信号，主要用于类似死机的BUG调试
    Calls:
      Called By:
    parameter:
      Return:
      author:
            capture SIGKILL SIGINT and SIGTERM to protect disk
********************************************************************/
void CaptureAllSignal()
{
    int i = 0;
    for(i = 0; i < 32; i ++)
    {
        if ( (i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM))
        {
            signal(i, SIG_IGN);
        }
        else
        {
            signal(i, SignalHander);
        }
    }
}

void dump(int signo)
{
    char buf[1024];
    char cmd[1024] = {0};
    FILE *fh;

    snprintf(buf, sizeof(buf), "/proc/%d/cmdline", getpid());
    if(!(fh = fopen(buf, "r")))
        exit(0);
    if(!fgets(buf, sizeof(buf), fh))
        exit(0);
    fclose(fh);
    if(buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = '\0';
    snprintf(cmd, sizeof(cmd), "./gdb %s %d", buf, getpid());
    printf("%s\n", cmd);
    printf("\n");
//  system(cmd);
    SystemCall_msg(cmd,SYSTEM_MSG_BLOCK_RUN);
    exit(0);
}


SINT32 MTMNGGetCfgFrmIni(MT_CONFIG_S* pstMtCfg)
{
    memset(pstMtCfg, 0x00, sizeof(MT_CONFIG_S));

    pstMtCfg->rtspsvr.bEnable = 1;
    pstMtCfg->rtspsvr.listen_port = g_tl_globel_param.stRtspCfg.dwPort;
    pstMtCfg->rtspsvr.max_connections = 32;
    pstMtCfg->rtspsvr.udp_send_port_max = 7000;
    pstMtCfg->rtspsvr.udp_send_port_min = 6000;
    pstMtCfg->rtspOhttpsvr.bEnable = 1;
    //pstMtCfg->rtspOhttpsvr.listen_port = 553;
    pstMtCfg->rtspOhttpsvr.max_connections = 10;
    pstMtCfg->packet_len = 512;

    return 0;
}

SINT32 Rtsp_Server_Init(int enDeviceType)
{
    SINT32 s32Ret = 0;
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

    for(i=0; i<QMAPI_MAX_CHANNUM; i++)
    {
        stMtCfg.chn_cnt += QMAPI_MAX_STREAM_TYPE_NUM;
        for(j = 0;j< QMAPI_MAX_STREAM_TYPE_NUM; j++)
        {
            stMtCfg.mbuf[k].chnid = (i+1)*10+j+1;
            stMtCfg.mbuf[k].max_connect_num = 2;
            k++;
        }
    }
    /*初始化回调函数*/

    printf("#### %s %d, wChannelNum=%d. dev:%d. \n", __FUNCTION__, __LINE__, QMAPI_MAX_CHANNUM, enDeviceType);
    if((enDeviceType == DEVICE_MEGA_IPCAM) || (enDeviceType == DEVICE_MEGA_1080P) || (enDeviceType == DEVICE_MEGA_960P))
    {
    	stMtCfg.enLiveMode =  LIVE_MODE_1CHN1USER;
    	stMtCfg.maxUserNum = 2*QMAPI_MAX_CHANNUM*QMAPI_MAX_STREAM_TYPE_NUM;
        stMtCfg.maxFrameLen = 2*1024*1024;//暂时搞大
    	stMtCfg.packet_len = 4*1024;//1428;
    	//stMtCfg.packet_len = 1452;
    }
    else
    {
    	stMtCfg.enLiveMode = LIVE_MODE_1CHN1USER;
    	stMtCfg.maxUserNum = 2 * QMAPI_MAX_CHANNUM * QMAPI_MAX_STREAM_TYPE_NUM;
      	stMtCfg.maxFrameLen = 1024*1024;
    	//stMtCfg.packet_len = 4*1024;//1428;
    	stMtCfg.packet_len = 1452;
    }

    s32Ret =  HI_MT_Init(&stMtCfg);
    if(0 != s32Ret)
    {
        return s32Ret;
    }

    HI_MT_StartSvr();

    return 0;
}

SINT32 Rtsp_Server_DeInit()
{
    SINT32 s32Ret = 0;

    s32Ret =  HI_MT_Deinit();
    if(0 != s32Ret)
    {
        return s32Ret;
    }
    return 0;
}

void HDmng_Callback(HD_EVENT_TYPE_E Event, int DiskNo, int *pStatus)
{
	int i;
	printf("HDmngCallback, event:%d. diskno:%d. \n",Event,DiskNo);
#if 0
    switch(Event)
	{
		case HD_EVENT_FORMAT:
			break;
		case HD_EVENT_MOUNT:
		{
			for (i=0; i<QMAPI_MAX_CHANNUM; i++)
			{
				QMAPI_Record_Start(0, 0x0001, i);
			}
			break;
		}
		case HD_EVENT_UNMOUNT:
		{
			for (i=0; i<QMAPI_MAX_CHANNUM; i++)
			{
				QMAPI_Record_Stop(0, 0, i);
			}
			break;
		}
		case HD_EVENT_ERROR:
		case HD_EVENT_UNFORMAT:
		case HD_EVENT_BUTT:
			break;
		default:
			break;
	}
#endif
	return ;
}

int HDmng_Get_SdWriteProtectFun()
{
	printf("HDmng_GetSdWriteProtectFun. \n");
	return 0;
}


int Record_Callback(RECORD_FILE_T *file) 
{
    char top_dir[64];
    char top_chn_dir[64];
    char video_name[64];
    char new_file_name[128];
    char *filename = file->name;

    sscanf(filename, "%[0-9,a-z,/]_%s", top_dir, video_name);
    printf("filename:%s, top_dir:%s video_name:%s\n", filename,
            top_dir, video_name);

    sprintf(top_chn_dir, "%s/%d", top_dir, 0);

    if (access(top_chn_dir, F_OK) != 0) {
        printf("make rec store path %s\n", top_chn_dir);
        char cmd[128];
        sprintf(cmd, "mkdir -p %s 0644", top_chn_dir);
        system(cmd);
    }


    char *temp_name = (char *)strrchr(filename, '/');
    if (!temp_name) {
        printf("%s %s is invalid video file\n", __FUNCTION__, filename); 
        return -1;
    }
    temp_name += 1; /*!< skip '/' */
    int duration = 0; 
    struct tm tm = {0};
    char event_type[64];
    char container_type[64];    
	int scan_ret = sscanf(temp_name, "%4d%2d%2d_%2d%2d%2d_%[N,M]_%6d.%s", 
                      &tm.tm_year,&tm.tm_mon,&tm.tm_mday, 
                      &tm.tm_hour,&tm.tm_min,&tm.tm_sec, 
                      event_type, &duration, &container_type); 
    #define SSCANF_ALL_CNT 9
    if (scan_ret != SSCANF_ALL_CNT) { 
        printf("%s %s is invalid video file\n", __FUNCTION__, filename);  
        return -1; 
    } 

    printf("After sscanf:%04d%02d%02d_%02d%02d%02d_%s_%06d.%s \n", tm.tm_year, tm.tm_mon, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            event_type, duration, container_type);

    sprintf(new_file_name, "%s/%02d%02d%02d.%s", top_chn_dir, tm.tm_hour, tm.tm_min, tm.tm_sec, container_type);
    printf("new_file_name:%s\n", new_file_name);

    if (rename(filename, new_file_name) != 0) {
        printf("rename filename:%s new_file_name:%s failed\n", filename, new_file_name);
        return -1;
    }

    return 0;
}

void Record_Event_Handle(char *event, void *arg, int size)
{
    printf("%s event:%s arg:%p size:%d\n", __FUNCTION__, event, arg, size);

    RECORD_NOTIFY_T *notify =  (RECORD_NOTIFY_T *)arg;
    RECORD_FILE_T * file    = notify->arg;

    switch(notify->type)
    {
        case RECORD_NOTIFY_START:
            printf("%s trigger:%d file:%s start\n", __FUNCTION__, file->trigger, file->name);
            break;

        case RECORD_NOTIFY_FINISH:
            printf("%s trigger:%d file:%s finish\n", __FUNCTION__, file->trigger, file->name);
            break;

        case RECORD_NOTIFY_ERROR:
            break;

        default:
            break;
    }

    return 0;
}


int Net_AlarmEventCallBack(int nChannel, int nAlarmType, int nAction, void* pParam)
{
    TL_SERVER_MSG msg;
    QMAPI_SYSTEMTIME *systime = (QMAPI_SYSTEMTIME *)pParam;

    printf("## %s %d: channel=%d, alarmType=%d, action=%d\n", __FUNCTION__, __LINE__, nChannel, nAlarmType, nAction);

    switch(nAlarmType)
    {
        case QMAPI_ALARM_TYPE_VMOTION:
            msg.dwMsg = TL_MSG_MOVEDETECT;
            break;
        case QMAPI_ALARM_TYPE_VMOTION_RESUME:
            msg.dwMsg = TL_MSG_MOVERESUME;
            break;
        default:
            return -1;
    }

    msg.dwChannel = nChannel;
    msg.st.wYear = systime->wYear;
    msg.st.wMonth = systime->wMonth;
    msg.st.wDayOfWeek = systime->wDayOfWeek;
    msg.st.wDay = systime->wDay;
    msg.st.wHour = systime->wHour;
    msg.st.wMinute = systime->wMinute;
    msg.st.wSecond = systime->wSecond;
    msg.st.wMilliseconds = systime->wMilliseconds;
    msg.dwDataLen = 0;

    return TL_Video_Net_SendAllMsg((char *)&msg, sizeof(TL_SERVER_MSG));
}

int SetNetWorkConfig(char *IPAddr, char *IPMask, char *GatewayIpAddr, char *Dns1Addr, char *Dns2Addr)
{
	PQMAPI_NET_NETWORK_CFG pNetWork = &g_tl_globel_param.stNetworkCfg;

	if (IPAddr)
	    strcpy(pNetWork->stEtherNet[0].strIPAddr.csIpV4, IPAddr);
	if (IPMask)
	    strcpy(pNetWork->stEtherNet[0].strIPMask.csIpV4, IPMask);
	if (GatewayIpAddr)
	    strcpy(pNetWork->stGatewayIpAddr.csIpV4, GatewayIpAddr);
	if (Dns1Addr)
	    strcpy(pNetWork->stDnsServer1IpAddr.csIpV4, Dns1Addr);
	if (Dns2Addr)
	    strcpy(pNetWork->stDnsServer2IpAddr.csIpV4, Dns2Addr);

	return 0;
}

/*
	模块退出；目的是节省资源，方便升级

	需要考虑模块的关联性

*/
int ModuleStop(void)
{
	printf("===============================ModuleStop==========================\n");

	StopNWSearch();
	StopNWServer();

#ifdef Q3_RTSP_SERVER
	Rtsp_Server_DeInit();
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);
#endif

#ifdef Q3_PPCS_P2P
	ppcs_p2p_exit();
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);
#endif

#ifdef Q3_QVT_P2P
    Qvt_P2p_Stop();
#endif
	QMAPI_Record_UnInit(RecordHandle);
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);

	QMAPI_Hdmng_DeInit();
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);

	QMAPI_AlarmServer_Stop(AlarmHandle);
	QMAPI_AlarmServer_UnInit(AlarmHandle);
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);

    QMAPI_AudioPromptStop();
    QMAPI_AudioPromptUnInit();

    QMAPI_Audio_Stop(AudioHandle);
    QMAPI_Audio_UnInit(AudioHandle);
    printf("####%s..%d..\n",__FUNCTION__,__LINE__);usleep(10);

	QMAPI_Video_MarkerStop();
//	QMAPI_Video_MarkerUninit();
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);usleep(10);

	QMAPI_AVServer_Stop(AVHandle);
	QMAPI_AVServer_UnInit(AVHandle);
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);usleep(10);

	QMAPI_NET_NTP_CFG ntpinfo = {0};
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NTPCFG, 0, &ntpinfo, sizeof(QMAPI_NET_NTP_CFG));

	QMAPI_SysTime_UnInit();
	printf("####%s..%d..\n",__FUNCTION__,__LINE__);usleep(10);

    videobox_stop();

//#ifdef TL_SUPPORT_WIRELESS
	QMAPI_Wireless_Stop();
    QMAPI_Wireless_Deinit();
//#endif

    watchdog_stop();

	printf("####%s..%d..\n",__FUNCTION__,__LINE__);usleep(10);

	//SystemCall_msg("killall audiobox", SYSTEM_MSG_BLOCK_RUN);
	return 0;
}

int ModuleStart(int handle)
{
    int enVideoInputType = 0;
    //TODO: delete it
    int enDeviceManResolution = DEVICE_MEGA_1080P;
    syscfg_default_param syscfg = {0};

    /* 初始化系统参数 */
    printf("===============================ModuleStart==========================\n");
    enVideoInputType = QMAPI_VIDEO_INPUT_SC2135;
    syscfg.chanel          = QMAPI_MAX_CHANNUM;
    syscfg.enInputType     = enVideoInputType;
    QMAPI_SYSCFG_Init(&syscfg);

    /* 启动视频 */
    QMAPI_NET_CHANNEL_PIC_INFO picinfo;
	QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_PICCFG, 0, &picinfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	AVHandle = QMAPI_AVServer_Init(&picinfo, enDeviceManResolution);
	int videocount = 1<<QMAPI_MAIN_STREAM | 1<<QMAPI_SECOND_STREAM;
    QMAPI_AVServer_Start(AVHandle, videocount);
    QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_SET_PICCFG, 0, &picinfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));

	/* 启动视频水印 */
	QMAPI_Video_MarkerInit();
	QMAPI_Video_MarkerStart();

	/* 启动音频 */
	AudioHandle = QMAPI_Audio_Init(NULL);
	QMAPI_Audio_Start(AudioHandle);

    QMAPI_AudioPromptInit();
    QMAPI_AudioPromptStart();

    /* 启动报警模块 */
    AlarmHandle = QMAPI_AlarmServer_Init(NULL);
    QMAPI_AlarmServer_Start(AlarmHandle);

	/* 启动存储设备模块*/
	QMAPI_Hdmng_Init(HDmng_Callback, HDmng_Get_SdWriteProtectFun);

	/* 启动录像模块 */
	RecordHandle = QMAPI_Record_Init(NULL, Record_Callback);
	QMAPI_Record_Start(RecordHandle, 0x0001, 0);
    QM_Event_RegisterHandler(EVENT_RECORD, 1, Record_Event_Handle); 
    
    SystemCall_msg("eventsend mount", SYSTEM_MSG_NOBLOCK_RUN);


    /* 启动wifi模块 */
    //QMAPI_NET_WIFI_CONFIG stWifiInfo;
    //QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWifiInfo, sizeof(QMAPI_NET_WIFI_CONFIG));
    //QMAPI_Wireless_Init(NULL);
    //QMAPI_Wireless_Start(&stWifiInfo);


    /* 启动网络模块 */
	// need fixme yi.zhang
	// 初始化网络时，可能会修改MAC地址
	QMAPI_NET_NETWORK_CFG netinfo;
	QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_NETCFG, 0, &netinfo, sizeof(QMAPI_NET_NETWORK_CFG));
	QMAPI_InitNetWork(&netinfo, SetNetWorkConfig);

    /* RM#2291: Init Systime and start NTP after Network inited.     henry.li    2017/03/01 */
	/* 初始化系统时间 */
	QMAPI_NET_ZONEANDDST zoneinfo;
	QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_ZONEANDDSTCFG, 0, &zoneinfo, sizeof(QMAPI_NET_ZONEANDDST));
	QMAPI_SysTime_Init(zoneinfo.nTimeZone);

	/* 启动NTP线程 */
	QMAPI_NET_NTP_CFG ntpinfo;
	QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_NTPCFG, 0, &ntpinfo, sizeof(QMAPI_NET_NTP_CFG));
	if (ntpinfo.byEnableNTP)
	{
		QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_SET_NTPCFG, 0, &ntpinfo, sizeof(QMAPI_NET_NTP_CFG));
	}

	/* 启动升级功能 */
	QMAPI_Upgrade_Init(ModuleStop);

/***********************************以下是外部服务********************************************************/
#ifdef Q3_NET_SERVER
	int     serverMediaPort, webServerPort;
	serverMediaPort = netinfo.stEtherNet[0].wDataPort;
    webServerPort = netinfo.wHttpPort;

    /* 启动 web 服务*/
    TL_net_server_init(serverMediaPort, DEVICE_MEGA_1080P);
    TL_net_server_start();

    QMapi_sys_ioctrl(handle, QMAPI_NET_REG_ALARMCALLBACK, 0, Net_AlarmEventCallBack, 0);

    /* 启动局域网广播服务 */
    startServerInfoBroadcast();

#ifdef  Q3_THTTPD_SERVER
    pthread_t thread_thttpd_id;
    pthread_create(&thread_thttpd_id , NULL, (void *)thttpd_start_main , (void*)webServerPort);
#endif
#endif

#ifdef Q3_RTSP_SERVER
	/* 启动 RTSP 服务*/
	Rtsp_Server_Init(enDeviceManResolution);
#endif

#ifdef Q3_PPCS_P2P
	/*启动 P2P 服务*/
	ppcs_p2p_init();
#endif

#ifdef Q3_QVT_P2P
    Qvt_P2p_Start();
#endif

	StartNWSearch();
	StartNWServer();

    return 0;
}

/*
	系统退出回调
	需要退出录像模块，对T卡的保护
	可添加用户自己的需要退出的模块
*/
int System_Reboot(void )
{
	QMAPI_Record_UnInit(RecordHandle);

    /* RM#2936: Stop wifi (forget the connected AP)   henry.li  20170330 */
	QMAPI_Wireless_Stop();
	QMAPI_Wireless_Deinit();

	QMAPI_Hdmng_DeInit();

	QMAPI_SYSCFG_UnInit();
	// need fixme yi.zhang
	// 需要保持系统时间
	SystemCall_msg("umount /config", SYSTEM_MSG_NOBLOCK_RUN);
	printf("%s  %d, reboot ....\n",__FUNCTION__, __LINE__);
	sync();
    reboot(RB_AUTOBOOT);

	printf("%s  %d, reboot failed!!set watchdog time out..\n",__FUNCTION__, __LINE__);
	//watchdog_set_timeout(5);
	return 0;
}
int QMAPI_CleanVM()
{
    //当脏数据超过系统总体内存5%时,触发pdflush写脏数据到磁盘
    SystemCall_msg("echo 5 > /proc/sys/vm/dirty_ratio", SYSTEM_MSG_NOBLOCK_RUN);
    //当脏数据在内存驻留超过3s后,触发pdflush背脏数据到磁盘
    SystemCall_msg("echo 300 > /proc/sys/vm/dirty_expire_centisecs", SYSTEM_MSG_NOBLOCK_RUN);
    //pdflush会已5s为周期写脏数据到磁盘
    SystemCall_msg("echo 500 > /proc/sys/vm/dirty_writeback_centisecs", SYSTEM_MSG_NOBLOCK_RUN);

    //内核倾向于回收用于directory和inode cache内存
    SystemCall_msg("echo 101 > /proc/sys/vm/vfs_cache_pressure", SYSTEM_MSG_NOBLOCK_RUN);

    return 0;
}


/* RM#2292: changed http port, restart web server.  henry.li 2017/02/21 */
#include "qm_event.h"
static unsigned int gHttpPortNew = 0;


/**
 * @brief
 * @param
 * @return
 *
 */
static void HttpPortChanged(char* event, void *arg, int size)
{
    gHttpPortNew = (unsigned int)arg;
}


/* RM#2770: Do dhcp start/stop when Ethernet Link UP/DOWN.  henry.li 2017/03/17 */
/**
 * @brief
 * @param
 * @return
 *
 */
static void UpIfEthernet(int handle)
{
    QMAPI_NET_NETWORK_CFG netinfo;

    QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_NETCFG, 0, &netinfo, sizeof(QMAPI_NET_NETWORK_CFG));
    //QMAPI_InitNetWork(&netinfo, SetNetWorkConfig);
    QMAPI_SetNetWork(&netinfo, SetNetWorkConfig);
}


/**
 * @brief
 * @param
 * @return
 *
 */
static void DownIfEthernet(int handle)
{
    QMAPI_NET_NETWORK_CFG netinfo;
    QMapi_sys_ioctrl(handle, QMAPI_SYSCFG_GET_NETCFG, 0, &netinfo, sizeof(QMAPI_NET_NETWORK_CFG));
    //QMAPI_DeinitNetWork(&netinfo);

    if (netinfo.byEnableDHCP)
    {
        dhcpc_stop();
    }
    else
    {
    }

    SystemCall_msg("ifconfig eth0 0.0.0.0", SYSTEM_MSG_BLOCK_RUN);
    SystemCall_msg("rm -f /tmp/gw.eth0", SYSTEM_MSG_BLOCK_RUN);
}


/**
 * @brief
 * @param
 * @return
 *
 */
static void SetDefaultRoute()
{
	char cmdbuf[128] = {0};
	char gateway[32] = {0};

    /* Check wireless status */
    if (access("/tmp/gw.wlan0", F_OK) == 0)
    {
        printf("==[%s] WLAN is enabled!\n", __FUNCTION__);
        memset(gateway, 0, 32);

        if (GetIPGateWay("wlan0", gateway) == 0)
        {
            printf("gateway=[%s]\n", gateway);
            sprintf(cmdbuf, "ip route replace default via %s dev %s", gateway, "wlan0");
            SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
        }
    }

#if 0
    /* Check ethernet status */
    if (access("/tmp/gw.eth0", F_OK) == 0)
    {
        printf("==[%s] Eth is enabled!\n", __FUNCTION__);
        memset(gateway, 0, 32);

        if (GetIPGateWay("eth0", gateway) == 0)
        {
            printf("gateway=[%s]\n", gateway);
            sprintf(cmdbuf, "ip route replace default via %s dev %s", gateway, "eth0");
            SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
        }
    }
#endif
}


/**
 * @brief
 * @param
 * @return
 *
 */
void* mgmt_main(void* param)
{
    int netLinkStatus = -1;
    int netLinkStatusBak = -1;
    int handle;

    /* get the handle */
    handle = (int)param;

    /* register handler for web server restart */
    QM_Event_RegisterHandler("http_port_changed", 1, HttpPortChanged);

    while (1)
    {
        /* handle event: http port changed! restart web server */
        if (gHttpPortNew != 0)
        {
            printf("\n==[%s] Restart WebServer, port = %d ==\n", __FUNCTION__, (gHttpPortNew));
#ifdef  Q3_THTTPD_SERVER
            thttpd_stop_main();
            pthread_t thread_thttpd_id;
            pthread_create(&thread_thttpd_id , NULL, (void *)thttpd_start_main , (void*)gHttpPortNew);
#endif
            gHttpPortNew = 0;
        }

        /* RM#2770: Do dhcp start/stop when Ethernet Link UP/DOWN.  henry.li 2017/03/17 */
        /* Check Ethernet Link UP/DOWN ? */
        netLinkStatus = checkdev("eth0");

        /* Ethernet Link UP */
        if ((netLinkStatus == 1) && (netLinkStatus != netLinkStatusBak))
        {
            printf("==[%s] Ethernet Link UP!\n", __FUNCTION__);
            UpIfEthernet(handle);
        }
        /* Ethernet Link Down */
        else if ((netLinkStatus == 0) && (netLinkStatus != netLinkStatusBak))
        {
            printf("==[%s] Ethernet Link DOWN!\n", __FUNCTION__);
            DownIfEthernet(handle);
            SetDefaultRoute();
        }
        else if ((netLinkStatus == -1) && (netLinkStatus != netLinkStatusBak))
        {
            printf("==[%s] Ethernet Link ERROR!\n", __FUNCTION__);
        }
        else
        {
        }

        netLinkStatusBak = netLinkStatus;

        sleep(1);
    }
}


int main(int argc, char **argv)
{
	int QmapiHandel;
    prctl(PR_SET_NAME, (unsigned long)"main", 0,0,0);

    CaptureAllSignal();

	//watchdog_start();

    QmapiHandel = QMapi_sys_open(QMAPI_NETPT_MAIN);
    if (QmapiHandel == 0)
	{
		err();
		return -1;
	}
	QMapi_sys_start(QmapiHandel, System_Reboot);

	ModuleStart(QmapiHandel);

    QMAPI_CleanVM();

    /* RM#2292: create a management thread.  henry.li 2017/02/22 */
    pthread_t thread_mgmt_id;
    pthread_create(&thread_mgmt_id , NULL, (void *)mgmt_main , (void *)QmapiHandel);

    /* RM#3483: Add the alarm ring after QM started.  henry.li 2017/04/21 */
    QMAPI_AudioPromptPlayFile("/root/qm_app/res/sound/alarm.wav", 0);

    while(1)
    {
        SystemCall_msg("echo 2 >/proc/sys/vm/drop_caches", SYSTEM_MSG_NOBLOCK_RUN);
        sleep(3);
    }

    return -1;
}


