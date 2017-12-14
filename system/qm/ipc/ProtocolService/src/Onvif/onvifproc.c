#include <errno.h>
#include <string.h>
#include <linux/if_ether.h>
#include <dlfcn.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <iconv.h>

#include "uuid.h"
#include "onvif.h"
#include "onvifproc.h"
#include "QMAPI.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"

#define ETH_NAME "eth0"
#define WIFI_NAME "wlan0"

int Onvif_Proc(httpd_conn* hc);

static void *OnvifAlarmThread(void *para);

static void InitNotifyManager();

static int Onvif_Alarm_Out(int nChannel, int nAlarmType, int nAction, void* pParam);

//static void *StartProbeThread(void *para);

static struct onvif_config onvif_ipc_config;

static int onvif_module_deinit;
static int onvif_handle;
static pthread_t AlarmThreadId;
static int isSendBye = 0;

extern int g_nw_search_run_index;

int ConvertTimeZone_DMS2Time(int dms_tz, int *hour, int *min)
{
	*min = 0;
	switch (dms_tz)
	{
		case QMAPI_GMT_N12:
			*hour = -12;
			break;
		case QMAPI_GMT_N11:
			*hour = -11;
			break;
		case QMAPI_GMT_N10:
			*hour = -10;
			break;
		case QMAPI_GMT_N09:
			*hour = -9;
			break;
		case QMAPI_GMT_N08:
			*hour = -8;
			break;
		case QMAPI_GMT_N07:
			*hour = -7;
			break;
		case QMAPI_GMT_N06:
			*hour = -6;
			break;
		case QMAPI_GMT_N05:
			*hour = -5;
			break;
		case QMAPI_GMT_N0430:
			*hour = -4;
			*min = -30;
			break;
		case QMAPI_GMT_N04:
			*hour = -4;
			break;
		case QMAPI_GMT_N0330:
			*hour = -3;
			*min = -30;
			break;
		case QMAPI_GMT_N03:
			*hour = -3;
			break;
		case QMAPI_GMT_N02:
			*hour = -2;
			break;
		case QMAPI_GMT_N01:
			*hour = -1;
			break;
		case QMAPI_GMT_00:
			*hour = 0;
			break;
		case QMAPI_GMT_01:
			*hour = 1;
			break;
		case QMAPI_GMT_02:
			*hour = 2;
			break;
		case QMAPI_GMT_03:
			*hour = 3;
			break;
		case QMAPI_GMT_0330:
			*hour = 3;
			*min = 30;
			break;
		case QMAPI_GMT_04:
			*hour = 4;
			break;
		case QMAPI_GMT_0430:
			*hour = 4;
			*min = 30;
			break;
		case QMAPI_GMT_05:
			*hour = 5;
			break;
		case QMAPI_GMT_0530:
			*hour = 5;
			*min = 30;
			break;
		case QMAPI_GMT_0545:
			*hour = 5;
			*min = 45;
			break;
		case QMAPI_GMT_06:
			*hour = 6;
			break;
		case QMAPI_GMT_0630:
			*hour = 6;
			*min = 30;
			break;
		case QMAPI_GMT_07:
			*hour = 7;
			break;
		case QMAPI_GMT_08:
			*hour = 8;
			break;
		case QMAPI_GMT_09:
			*hour = 9;
			break;
		case QMAPI_GMT_0930:
			*hour = 9;
			*min = 30;
			break;
		case QMAPI_GMT_10:
			*hour = 10;
			break;
		case QMAPI_GMT_11:
			*hour = 11;
			break;
		case QMAPI_GMT_12:
			*hour = 12;
			break;
		case QMAPI_GMT_13:
			*hour = 13;
			break;
		default:
			return -1;
	}

	return 0;
}

void getUtcTime(time_t *ts, QMAPI_SYSTEMTIME *utcTime)
{
	QMAPI_SYSTEMTIME curTime;
	memset(&curTime, 0, sizeof(QMAPI_SYSTEMTIME));
	QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &curTime, sizeof(curTime));

	QMAPI_NET_ZONEANDDST zone_info;
	int tz_hour = 0, tz_min = 0;

	memset(&zone_info, 0, sizeof(QMAPI_NET_ZONEANDDST));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ZONEANDDSTCFG, 0, &zone_info, sizeof(QMAPI_NET_ZONEANDDST));
	ConvertTimeZone_DMS2Time(zone_info.nTimeZone, &tz_hour, &tz_min);

	struct tm tmTime = {0};
	tmTime.tm_year = curTime.wYear - 1900;
	tmTime.tm_mon = curTime.wMonth - 1;
	tmTime.tm_mday = curTime.wDay;
	tmTime.tm_hour = curTime.wHour;
	tmTime.tm_min = curTime.wMinute;
	tmTime.tm_sec = curTime.wSecond;
	tmTime.tm_isdst = zone_info.dwEnableDST;
	
	*ts = mktime(&tmTime);
	*ts += tz_hour * 60 + tz_min;

	if (utcTime)
	{
		gmtime_r(ts, &tmTime);
		
		utcTime->wYear = tmTime.tm_year + 1900;
		utcTime->wMonth = tmTime.tm_mon + 1;
		utcTime->wDay = tmTime.tm_mday;
		utcTime->wHour = tmTime.tm_hour;
		utcTime->wMinute = tmTime.tm_min;
		utcTime->wSecond = tmTime.tm_sec;
	}
}

 int code_convert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t *outlen)
 {
	 //const char**pin = (const char**)&inbuf;
	 //char **pout = &outbuf;
	 //sys_fun_iconv_convert( to_charset, from_charset, pin, &inlen, pout, outlen);
#if 1
	 iconv_t cd;
	 const char**pin = (const char**)&inbuf;
	 char **pout = &outbuf;
	 cd = iconv_open(to_charset, from_charset);
	 if((int)cd==-1){
		 printf("iconv_err:%d\n", (int)cd);
		 return -1;
	 }
	 memset(outbuf, 0, *outlen);
	 if(iconv(cd, pin, &inlen, pout, outlen)==-1)
	 {
		 iconv_close(cd);
		 return -1;
	 }
 
	 //printf("#### %s %d, pout:%s, outlen=%d\n", __FUNCTION__, __LINE__, *pout, *outlen);
	 iconv_close(cd);
#endif
	 return 0;
 }


static void *dl_handle = NULL;
static pthread_mutex_t dl_lock;
static int dl_usecount = 0;
static unsigned int dl_timer = 0;

int Onvif_Init()
{
    HI_S_ICGI_Proc struProc;
    QMAPI_NET_DEVICE_INFO stDeviceInfo;

    onvif_module_deinit = 0;

    stDeviceInfo.dwSize = sizeof(QMAPI_NET_DEVICE_INFO);
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &stDeviceInfo, sizeof(QMAPI_NET_DEVICE_INFO)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_DEVICECFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }

    QMAPI_SYSCFG_ONVIFTESTINFO stONVIFTset;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &stONVIFTset, sizeof(QMAPI_SYSCFG_ONVIFTESTINFO)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_ONVIFTESTINFO failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }

    pthread_mutex_init(&dl_lock, NULL);

    memset(&onvif_ipc_config, 0, sizeof(onvif_ipc_config));
    onvif_ipc_config.GetUtcTime = getUtcTime;
    onvif_ipc_config.CodeConvert = code_convert;
    onvif_ipc_config.ConvertTimeZone = ConvertTimeZone_DMS2Time;

    onvif_ipc_config.g_channel_num = stDeviceInfo.byVideoInNum;
    onvif_ipc_config.g_OnvifCpuType = stDeviceInfo.dwServerCPUType;
    onvif_ipc_config.system_reboot_flag = REBOOT_FLAG_IDLE;
    onvif_ipc_config.discoveryMode = stONVIFTset.dwDiscoveryMode;

    onvif_ipc_config.discoveryMode = 1;
    printf("#### %s %d, discoveryMode=%d\n", __FUNCTION__, __LINE__, onvif_ipc_config.discoveryMode);

#if 0
    g_pNotifyFileName = (char *)malloc(64);
    if(g_pNotifyFileName == NULL)
    {
        printf("%s  %d, malloc failed!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
        return -1;
    }

    if(stDeviceInfo.dwServerCPUType == DMS_CPU_TI365)
    {
        strcpy(g_pNotifyFileName, "/mnt/nand/notify.conf");
    }
    else
    {
        strcpy(g_pNotifyFileName, "/jb_config/jb_rootfs/notify.conf");
    }
#endif
    InitNotifyManager();

    onvif_handle = QMapi_sys_open(QMAPI_NETPT_ONVIF);
    if(onvif_handle<0)
    {
        printf("%s  %d, QMapi_sys_open failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }

    QMapi_sys_ioctrl(onvif_handle, QMAPI_NET_REG_ALARMCALLBACK, 0, Onvif_Alarm_Out, sizeof(void *));

    memset(&struProc, 0x00, sizeof(HI_S_ICGI_Proc));
    strcpy(struProc.pfunType, "onvif");
    struProc.pfunProc = Onvif_Proc;

    HI_ICGI_Register_Proc(&struProc);

    printf("Onvif Module Build:%s %s\n", __DATE__, __TIME__);

    //pthread_create(&probeThreadId, NULL, StartProbeThread, NULL);
    return 1;
}

void Onvif_DeInit()
{
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    select(0, NULL, NULL, NULL, &tv);

    printf("Unregister onvif module~!\n");
    if (onvif_module_deinit == 1)
    {
        printf("%s uninit second times!\n", __FUNCTION__);
        return ; //防止二次反初始化
    }
    //HI_S_ICGI_Proc struProc;

    //strcpy(struProc.pfunType,"onvif");
    onvif_module_deinit = 1;
    //HI_ICGI_DeRegister_Proc(&struProc);
    QMapi_sys_ioctrl(onvif_handle, QMAPI_NET_UNREG_ALARMCALLBACK, 0, NULL, 0);
    QMapi_sys_close(onvif_handle);  

    pthread_join(AlarmThreadId, NULL);

    pthread_mutex_destroy(&onvif_ipc_config.gNotifyLock);
    pthread_mutex_destroy(&onvif_ipc_config.onvif_pullLock);
    pthread_mutex_destroy(&dl_lock);

    if (dl_handle)
    {
        void (*uninit_func)();
        uninit_func = dlsym(dl_handle, "ONVIF_EventServerUnInit");
        if (uninit_func)
        {
            uninit_func();
        }
        //sys_fun_dlclose(dl_handle);
        dlclose(dl_handle);
        dl_handle = NULL;
    }

    //if(thread_count>0)
    //  usleep(10000);
}

static void *GetDlFunction(const char *funcname)
{
    void *func;
    pthread_mutex_lock(&dl_lock);
    if (!dl_handle)
    {
        dl_handle = dlopen("/usr/lib/libonvif.so", RTLD_LAZY);	
        printf("ts:%ld open onvif dl handle:%p!\n", time(NULL), dl_handle);
        if (!dl_handle)
        {
            printf("open onvif handle fiailed!\n");
            pthread_mutex_unlock(&dl_lock);
            return NULL;
        }
    }

    ++dl_usecount;
    //printf("onvif login use count is %d\n", dl_usecount);
    pthread_mutex_unlock(&dl_lock);

    func = dlsym(dl_handle, funcname);
    if (!func)
    {
        printf("[%s] dlsym failed:%s\n", __FUNCTION__, dlerror());
    }

    return func;
}

static void ReleaseDlFunction()
{
    pthread_mutex_lock(&dl_lock);
    if (--dl_usecount < 0)
    {
        dl_usecount = 0;
    }
    //printf("onvif logout use count is %d\n", dl_usecount);
    pthread_mutex_unlock(&dl_lock);
}

static BOOL IsHandleEmpty()
{
    BOOL bEmpty = FALSE;

    pthread_mutex_lock(&dl_lock);
    if (!dl_usecount)
    {
        bEmpty = TRUE;
    }
    pthread_mutex_unlock(&dl_lock);
    return bEmpty;
}

static void CloseHandle()
{
    void (*uninit_func)();

    pthread_mutex_lock(&dl_lock);
    if (!dl_usecount && dl_handle)
    {
        uninit_func = dlsym(dl_handle, "ONVIF_EventServerUnInit");
        if (uninit_func)
        {
            uninit_func();
        }

        printf("ts:%lu close onvif dl handle:%p!\n", time(NULL), dl_handle);	
        //sys_fun_dlclose(dl_handle);
        dlclose(dl_handle);
        dl_handle = NULL;
    }
    pthread_mutex_unlock(&dl_lock);
}
 
/**
* @brief notify_thr
* @This thread sends the notify message when an event occurs.
* (This thread is initialized for every 'Subscribe' request)
* Operations Done
*  1. Loop till 'Termination Time' (Sent as a part of Subscription Request)
*  2. Intialize appropriate event handling mechanism
*  3. Wait for events
*  4. Send Notify Packet whenever an event Occurs
*/
static int connect_to_server(char *server,unsigned short port, int timeout)
{
    //int sockOptVal = 1;
    struct sockaddr_in clientAddr;
    struct timeval time = {3, 0};
    //socklen_t len = sizeof(time);

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockFd){
        //dbg(Err, DbgPerror, "socket rtspSvrSockFd error \n");
        return (-1);
    }
#if 0
    if (-1 == setsockopt(sockFd , SOL_SOCKET, SO_REUSEADDR,&sockOptVal, sizeof(int))) {
    //dbg(Err, DbgPerror, "setsockopt error \n");
    close(sockFd);
    sockFd = -1;
    return (-1);
    }
#endif

    time.tv_sec = timeout;
    if (-1 == setsockopt(sockFd , SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time))) {
        //dbg(Err, DbgPerror, "setsockopt error \n");
        //te_sock_close(sockFd);
        close(sockFd);

        sockFd = -1;
        return (-1);
    }
#if 0
    if (-1 == setsockopt(sockFd , SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time))) {
    //dbg(Err, DbgPerror, "setsockopt error \n");
    //te_sock_close(sockFd);
    close(sockFd);

    sockFd = -1;
    return (-1);
    }
#endif 
    clientAddr.sin_family		 = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(server);
    clientAddr.sin_port		 = htons(port);
    if (connect(sockFd, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == -1) {
        if (errno == EINPROGRESS) {
            fprintf(stderr, "timeout\n");
        }
        close(sockFd);
        sockFd = -1;
        return (-1);
    }

    return sockFd;
}

 
static int send_n(int fd, void *buf, int size)
{
    int n,left;
    char *pBuf;

    n = 0;
    left = size;
    pBuf = (char *)buf;
    while(left > 0)
    {
        n = send(fd, pBuf, left, MSG_NOSIGNAL);
        if (n <= 0)
        {
            /* EINTR A signal occurred before any data was transmitted. */
            if(errno == EINTR)
            {
                n = 0;
                //dbg(Err, DbgPerror, "socket send_n eintr error");
            }
            else
            {
                //dbg(Err, DbgPerror, "socket error");
                return (-1);
            }
        }
        left -= n;
        pBuf += n;
    }
    return size;
}
 

static void InitNotifyManager()
{
	//memset(&onvif_ipc_config.g_NotifyInfo, 0, sizeof(onvif_ipc_config.g_NotifyInfo) );
	//memset(onvif_ipc_config.onvif_PullNode, 0, sizeof(struct pullnode) * 10);

	pthread_mutex_init(&onvif_ipc_config.gNotifyLock, NULL);
	pthread_mutex_init(&onvif_ipc_config.onvif_pullLock, NULL);

	pthread_create(&AlarmThreadId, NULL, OnvifAlarmThread, NULL);
}

static int ParseEndPoint(const char *endpoint, char *ip, unsigned short *port, char *path)
{
	if (strncmp(endpoint, "http://", strlen("http://")))
	{
		return -1;
	}

	const char *pos = endpoint + strlen("http://");
	char *ptr = ip;
	while (*pos && (*pos != ':') && (*pos != '/'))
	{
		*ptr++ = *pos++;
	}
	
	*ptr = '\0';
	if (ptr == ip)
	{
		return -1;
	}
	
	if (*pos && (*pos == ':'))
	{
		*port = atoi(++pos);
	}
	else
	{
		*port = 80;
	}

	if (*pos && (pos = strchr(pos, '/')) && *(pos + 1))
	{
		strcpy(path, pos + 1);
		//if (path[strlen(path) -1] == '/')
		//{
		//	path[strlen(path) - 1] = '\0';
		//}
	}
	else
	{
		path[0] = '\0';
	}

	printf("path = %s\n", path);
	return 0;
}

static void PostAlarm(const NOTIFY_ITEM *pItem, unsigned int status, unsigned int ioStatus)
{
	char sendbuf[4096] = { 0 };
	
	char ip[32] = {0};
	char path[128] = {0};
	unsigned short port = 80;
	if (ParseEndPoint(pItem->endpoint, ip, &port, path))
	{
		return ;
	}

	char *p = sendbuf + 500;
	p += sprintf(p, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" "
		"xmlns:tt=\"http://www.onvif.org/ver10/schema\" "
		"xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" xmlns:wsa=\"http://www.w3.org/2005/08/addressing\" "
		"xmlns:tns1=\"http://www.onvif.org/ver10/topics\">"
		"<SOAP-ENV:Header>"
		"<wsa:To SOAP-ENV:mustUnderstand=\"true\">%s</wsa:To>"			
		"<wsa:Action>http://docs.oasis-open.org/wsn/bw-2/NotificationConsumer/Notify</wsa:Action>"
		"</SOAP-ENV:Header>"
		"<SOAP-ENV:Body>"
		"<wsnt:Notify>", pItem->endpoint);

	QMAPI_SYSTEMTIME rtcTime;
	time_t ts = 0;
	getUtcTime(&ts, &rtcTime);
	
	int i;
	for (i = 0; i < onvif_ipc_config.g_channel_num; i++)
	{
    	if (pItem->bEnableMotion && (pItem->status&(1<<i)) != (status&(1<<i)))
    	{
    		p += sprintf(p, "<wsnt:NotificationMessage>"
       			"<wsnt:Topic Dialect=\"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet\">tns1:RuleEngine/CellMotionDetector/Motion</wsnt:Topic>"
				"<wsnt:Message>"
				"<tt:Message UtcTime=\"%04d-%02d-%02dT%02d:%02d:%02dZ\" PropertyOperation=\"%s\">"
				"<tt:Source>"
				"<tt:SimpleItem Name=\"VideoSourceConfigurationToken\" Value=\"VideoSourceConfiguration%d\"/>"
				"<tt:SimpleItem Name=\"VideoAnalyticsConfigurationToken\" Value=\"VideoAnalyticsToken%d\"/>"
				"<tt:SimpleItem Name=\"Rule\" Value=\"MyMotionDetectorRule\"/>"
				"</tt:Source>"
				"<tt:Data>"
				"<tt:SimpleItem Name=\"IsMotion\" Value=\"%s\"/>"
				"</tt:Data>"
				"</tt:Message>"
				"</wsnt:Message>"
				"</wsnt:NotificationMessage>", 
        		rtcTime.wYear, rtcTime.wMonth, rtcTime.wDay, rtcTime.wHour, rtcTime.wMinute, rtcTime.wSecond, 
        		(pItem->bNoFirst ? "Changed" : "Initialized"), i, i, (status ? "true" : "false"));
    	}
    	
    	if (pItem->bEnableIO && (pItem->ioStatus&(1<<i)) != (ioStatus&(1<<i)))
    	{
    		p += sprintf(p, "<wsnt:NotificationMessage>"
        		"<wsnt:Topic Dialect=\"http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete\">tns1:Device/Trigger/DigitalInput</wsnt:Topic>"
    			"<wsnt:Message>"
        		"<tt:Message UtcTime=\"%04d-%02d-%02dT%02d:%02d:%02dZ\" PropertyOperation=\"%s\">"
        		"<tt:Source>"
        		"<tt:SimpleItem Name=\"InputToken\" Value=\"DigitInputToken0\"/>"
        		"</tt:Source>"
        		"<tt:Data>"
        		"<tt:SimpleItem Name=\"LogicalState\" Value=\"%s\"/>"
        		"</tt:Data>"
        		"</tt:Message>"
        		"</wsnt:Message>"
        		"</wsnt:NotificationMessage>", 
				rtcTime.wYear, rtcTime.wMonth, rtcTime.wDay, rtcTime.wHour, rtcTime.wMinute, rtcTime.wSecond, 
        		(pItem->bIoNoFirst ? "Changed" : "Initialized"), (ioStatus ? "true" : "false"));
    	}
	}

	p += sprintf(p, "</wsnt:Notify></SOAP-ENV:Body></SOAP-ENV:Envelope>\r\n");

	int contentLen = p - &sendbuf[500]; 
	char tempBuf[500];
	p = tempBuf;
	if (path[0])
	{
		p += sprintf(p, "POST /%s HTTP/1.1\r\n", path);
	}
	else
	{
		p += sprintf(p, "%s", "POST / HTTP/1.1\r\n");
	}	
	
	p += sprintf(p, "Host: %s:%d\r\n", ip, port); 
	p += sprintf(p, "%s\r\n", "User-Agent: gSOAP/2.8"); 
	p += sprintf(p, "%s\r\n", "Content-Type: application/soap+xml; charset=utf-8; action=\"http://docs.oasis-open.org/wsn/bw-2/NotificationConsumer/Notify\"");
	p += sprintf(p, "Content-Length: %d\r\n", contentLen);
	p += sprintf(p, "Connection: close\r\n\r\n");

	//printf("string is %s\n", tempBuf);

	//printf("header len i s%d buf len is %d\n", strlen(tempBuf), strlen(sendbuf + 500));

	int headerSize = p - tempBuf;
	contentLen += headerSize;
	p = sendbuf + (500 - headerSize);
	char *pStart = p;
	for (i = 0; i < headerSize; i++)
	{
		//if (*p)
		//	printf("error numbe: %c!\n", *p);
		*p++ = tempBuf[i];
	}

	//printf("send len: %d buf is:\n%s\n\n", contentLen, pStart);

	int sendSock;
	//int timeOutCount = 0;
	//while (timeOutCount <= 3)
	//{
		sendSock = connect_to_server(ip, port, 3);
		if (-1 != sendSock)
		{
			send_n(sendSock, pStart, contentLen);
			close(sendSock);

			//if (sendlen == contentLen)
			//{
			//	break;
			//}
			//else
			//{
			//	++timeOutCount;
			//}
		}
		//else
		//{
		//	timeOutCount++;
		//}
	//}
}

static void ProcessAalrm()
{	
	unsigned int state = onvif_ipc_config.MAlarm_status;
	unsigned int ioState = onvif_ipc_config.IOAlarm_status;
	
	fd_set write_set, error_set;
	FD_ZERO(&write_set);
	FD_ZERO(&error_set);

	int i;
	time_t curTime = time(NULL);
	
	pthread_mutex_lock(&onvif_ipc_config.gNotifyLock);
	for (i = 0; (i < MAX_NOTIFY_CLIENT) && onvif_ipc_config.g_NotifyInfo.totalActiveClient; i++)
	{
		if (onvif_ipc_config.g_NotifyInfo.notify[i].id)
		{
			if (onvif_ipc_config.g_NotifyInfo.notify[i].endTime < curTime)
			{
				onvif_ipc_config.g_NotifyInfo.notify[i].id = 0;
				if (onvif_ipc_config.g_NotifyInfo.notify[i].endpoint)
				{
					free(onvif_ipc_config.g_NotifyInfo.notify[i].endpoint);
					onvif_ipc_config.g_NotifyInfo.notify[i].endpoint = NULL;
				}
				onvif_ipc_config.g_NotifyInfo.notify[i].status = onvif_ipc_config.g_NotifyInfo.notify[i].ioStatus = 0;
				onvif_ipc_config.g_NotifyInfo.notify[i].endTime = 0;
				onvif_ipc_config.g_NotifyInfo.notify[i].bNoFirst = onvif_ipc_config.g_NotifyInfo.notify[i].bIoNoFirst = 0;
				--onvif_ipc_config.g_NotifyInfo.totalActiveClient;

				printf("id%d is timeout!!!\n", onvif_ipc_config.g_NotifyInfo.notify[i].id);
			}
			else if ((onvif_ipc_config.g_NotifyInfo.notify[i].bEnableMotion && onvif_ipc_config.g_NotifyInfo.notify[i].status != state) 
				|| (onvif_ipc_config.g_NotifyInfo.notify[i].bEnableIO && onvif_ipc_config.g_NotifyInfo.notify[i].ioStatus != ioState))
			{
				NOTIFY_ITEM temp = onvif_ipc_config.g_NotifyInfo.notify[i];
				onvif_ipc_config.g_NotifyInfo.notify[i].bIoNoFirst = onvif_ipc_config.g_NotifyInfo.notify[i].bNoFirst = 1;
				onvif_ipc_config.g_NotifyInfo.notify[i].status = state;
				onvif_ipc_config.g_NotifyInfo.notify[i].ioStatus = ioState;

				//printf("notify is %X %X io is %X %X\n", temp.status, state, temp.ioStatus, ioState);
				
				char address[256];
				strncpy(address, onvif_ipc_config.g_NotifyInfo.notify[i].endpoint, 256);
				temp.endpoint = address;
				
				printf("Post Alarm start\n");
				pthread_mutex_unlock(&onvif_ipc_config.gNotifyLock);
				PostAlarm(&temp, state, ioState);
				pthread_mutex_lock(&onvif_ipc_config.gNotifyLock);
				printf("Post Alarm end\n");
			}
		}
	}
	pthread_mutex_unlock(&onvif_ipc_config.gNotifyLock);	
}

static void *OnvifAlarmThread(void *para)
{
	prctl(PR_SET_NAME, (unsigned long)"onvif_alarm", 0, 0, 0);

	int sockfd = -1;
	
	struct timeval tv;
	fd_set read_set, error_set;
	int (*func)(int , struct onvif_config *) = NULL;
	//unsigned int timeoutCount = 0;
	
	while (!onvif_module_deinit)
	{
		if (sockfd < 0)
		{
			if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				printf("open socket failed!\n");
		
				tv.tv_sec = 0;
				tv.tv_usec = 50000;
				select(0, NULL, NULL, NULL, &tv);
				continue;
			}
			
			fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
			
			int set = 1;
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));

			struct linger linger_opt;
			linger_opt.l_onoff = 1;
			linger_opt.l_linger = 2;
			setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));		
		
			struct sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(60987);
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
			{
				close(sockfd);
				sockfd = -1;
				
				tv.tv_sec = 0;
				tv.tv_usec = 50000;
				select(0, NULL, NULL, NULL, &tv);
				continue;
			}
		
			if (listen(sockfd, 10)) 
			{
				close(sockfd);
				sockfd = -1;
				
				printf("listen failed!\n");
				continue;
			}
		}
		
		FD_ZERO(&read_set);
		FD_SET(sockfd, &read_set);
		
		FD_ZERO(&error_set);
		FD_SET(sockfd, &error_set);
		
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		if (select(sockfd + 1, &read_set, NULL, &error_set, &tv) > 0)
		{
			if (FD_ISSET(sockfd, &error_set))
			{
				close(sockfd);
				sockfd = -1;
			}
			else
			{
				dl_timer = 0;
				//timeoutCount = 0;
				func = GetDlFunction("ONVIF_EventServer");
				if (func)
				{
					func(sockfd, &onvif_ipc_config);
				}

				ReleaseDlFunction();
			}
		}
		else if (IsHandleEmpty())
		{
			if (++dl_timer >= 500)
			{
				dl_timer = 0;
				CloseHandle();
			}
		}
		ProcessAalrm();

		tv.tv_sec = 0;
		tv.tv_usec = 30000;
		select(0, NULL, NULL, NULL, &tv);
	}
	
	if (sockfd > 0)
	{
		close(sockfd);
	}
	
	printf("stop onvif alarm thread!\n");
	return NULL;
}

void onvif_send_hello()
{
    printf("########################## %s %d, discoveryMode:%d\n", __FUNCTION__, __LINE__, onvif_ipc_config.discoveryMode);
	if (!onvif_ipc_config.discoveryMode)
	{
		return ;
	}
	
	char buf[2048] ;
	struct sockaddr_in client;
	//char ifname[32] = {0};
	char endpointId[64];
	char interfacename[2][8];
	int ifnum = 0;

    QMAPI_NET_WIFI_CONFIG stWiFiConfig;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWiFiConfig, sizeof(QMAPI_NET_WIFI_CONFIG)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_WIFICFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    QMAPI_SYSCFG_ONVIFTESTINFO stONVIFTset;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &stONVIFTset, sizeof(QMAPI_SYSCFG_ONVIFTESTINFO)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_ONVIFTESTINFO failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    QMAPI_NET_NETWORK_CFG stNetWorkCfg;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &stNetWorkCfg, sizeof(QMAPI_NET_NETWORK_CFG)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_NETCFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    QMAPI_NET_CHANNEL_PIC_INFO stChannelInfo;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, 0, &stChannelInfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_PICCFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    
    //struct ip_mreq command;
	system("route add -net 224.0.0.0 netmask 240.0.0.0 eth0");
	if (stWiFiConfig.bWifiEnable > 0)
	{
		system("route add -net 224.0.0.0 netmask 240.0.0.0 wlan0");

		if (checkdev(ETH_NAME) > 0)
		{
			strcpy(interfacename[0], WIFI_NAME);
			strcpy(interfacename[1], ETH_NAME);
			ifnum = 2;
		}
		else
		{
			strcpy(interfacename[0], WIFI_NAME);
			ifnum = 1;		
		}
	}
	else
	{
		strcpy(interfacename[0], ETH_NAME);
		ifnum = 1;
	}

	char scopes_buf[420];
	scopes_buf[0] = '\0';
	if (stONVIFTset.szDeviceScopes1[0])
	{
		sprintf(scopes_buf + strlen(scopes_buf), "onvif://www.onvif.org/%s ", stONVIFTset.szDeviceScopes1);
	}
	
	if (stONVIFTset.szDeviceScopes2[0])
	{
		sprintf(scopes_buf + strlen(scopes_buf), "onvif://www.onvif.org/%s ", stONVIFTset.szDeviceScopes2);
	}

	if (stONVIFTset.szDeviceScopes3[0])
	{
		sprintf(scopes_buf + strlen(scopes_buf), "onvif://www.onvif.org/%s ", stONVIFTset.szDeviceScopes3);
	}

	int sockfd;
	char tempAddr[QMAPI_MAX_IP_LENGTH];
	const char *ipaddr = NULL;
	struct timeval tv;
	int count, sendlen;
	uuid_t out;
	char uuid[64];
	
	int i;
	for (i = 0; i < ifnum; i++)
	{
	    memset(&client, 0, sizeof(client));
	    client.sin_family = AF_INET;
	    client.sin_port = htons(3702);
		inet_pton(AF_INET, "239.255.255.250", &client.sin_addr);

		if (interfacename[i][0] == 'e')
		{
			ipaddr = stNetWorkCfg.stEtherNet[0].strIPAddr.csIpV4;

			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
    				stNetWorkCfg.stEtherNet[0].byMACAddr[0], stNetWorkCfg.stEtherNet[0].byMACAddr[1],
    				stNetWorkCfg.stEtherNet[0].byMACAddr[2], stNetWorkCfg.stEtherNet[0].byMACAddr[3],
    				stNetWorkCfg.stEtherNet[0].byMACAddr[4], stNetWorkCfg.stEtherNet[0].byMACAddr[5],
    				stNetWorkCfg.stEtherNet[0].byMACAddr[0], stNetWorkCfg.stEtherNet[0].byMACAddr[1],
    				stNetWorkCfg.stEtherNet[0].byMACAddr[2], stNetWorkCfg.stEtherNet[0].byMACAddr[3],
    				stNetWorkCfg.stEtherNet[0].byMACAddr[4], stNetWorkCfg.stEtherNet[0].byMACAddr[5]);
		}
		else
		{
			if (get_ip_addr(WIFI_NAME, tempAddr))
			{
				continue;
			}
			
			ipaddr = tempAddr;

			char macAddr[6];
			char szMac[20];
			net_get_hwaddr(WIFI_NAME, szMac, macAddr);
			
			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
		}

		char csChannelName[32];
		size_t lenChannelName = sizeof(csChannelName);
		code_convert("GB2312", "UTF-8", stChannelInfo.csChannelName, 
			            strlen(stChannelInfo.csChannelName), csChannelName, &lenChannelName);

		for (count = 0; count < 2; ++count)
		{
			uuid_generate_random(out);
			uuid_unparse(out, uuid);
			
			sendlen = sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
				"<env:Envelope xmlns:env=\"http://www.w3.org/2003/05/soap-envelope\" "
				"xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
				"xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" "
				"xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" "
				"xmlns:wsadis=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" "
				"xmlns:wsa=\"http://www.w3.org/2005/08/addressing\">"
				"<env:Header>"
				"<wsadis:MessageID>uuid:%s</wsadis:MessageID>"
				"<wsadis:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsadis:To>"
				"<wsadis:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello</wsadis:Action>"
				"</env:Header>"
				"<env:Body><d:Hello><wsadis:EndpointReference>"
				"<wsadis:Address>urn:uuid:%s</wsadis:Address>"
				"</wsadis:EndpointReference>"
				"<d:Types>dn:NetworkVideoTransmitter tds:Device</d:Types>"
				"<d:Scopes>onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/type/audio_encoder"
				" onvif://www.onvif.org/type/ptz onvif://www.onvif.org/hardware/IPCAM"
				" onvif://www.onvif.org/Profile/Streaming onvif://www.onvif.org/name/%s"
				" onvif://www.onvif.org/type/network_video_transmitter %s</d:Scopes>"
				"<d:XAddrs>http://%s:%d/onvif/device_service</d:XAddrs>"
				"<d:MetadataVersion>1</d:MetadataVersion>"
				"</d:Hello>"
				"</env:Body>"
				"</env:Envelope>", uuid, endpointId, csChannelName, scopes_buf, ipaddr, stNetWorkCfg.wHttpPort);
			
			printf("#### %s %d, interfacename:%s, ipaddr:%s\n",  __FUNCTION__, __LINE__, interfacename[i], endpointId);

			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (sockfd < 0)
			{  
    			perror("Creatingsocket failed.");
    			return ;
			}
			
			if (ifnum == 2)
			{
				struct sockaddr_in my_addr = {0};
				my_addr.sin_family = AF_INET;
				inet_pton(AF_INET, ipaddr, &my_addr.sin_addr);
				bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
			}
			
    		if (sendto(sockfd, buf, sendlen, 0, (struct sockaddr *)&client, sizeof(client)) < 0)
    		{
    			printf("%s  %d, sendto error! errno: %d, %s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    			close(sockfd);
    			return ;
    		}
			close(sockfd);
			
			tv.tv_sec = 0;
			tv.tv_usec = 50000;
			select(0, NULL, NULL, NULL, &tv);
	    }		
	}
}

void onvif_send_bye(int fd)
{
	if (!isSendBye)
	{
		isSendBye = 1;
		printf("send bye!\n");
	}
	else
	{
		printf("has been send bye!\n");
		return ;
	}
	
	if (!onvif_ipc_config.discoveryMode)
	{
		return ;
	}
	
	int sockfd;
	char buf[2048];
	struct sockaddr_in client;  
	//char ifname[32] = {0};
	char endpointId[64];
	char interfacename[2][8];
	int ifnum = 0;
	uuid_t out;
	int i;

	char ipaddr[QMAPI_MAX_IP_LENGTH];

	QMAPI_NET_WIFI_CONFIG wifi_cfg;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &wifi_cfg, sizeof(wifi_cfg));
	
	if (wifi_cfg.bWifiEnable > 0)
	{
		if (checkdev(ETH_NAME) > 0)
		{
			strcpy(interfacename[0], WIFI_NAME);
			strcpy(interfacename[1], ETH_NAME);
			ifnum = 2;
		}
		else
		{
			strcpy(interfacename[0], WIFI_NAME);
			ifnum = 1;
		}
	}
	else
	{
		strcpy(interfacename[0], ETH_NAME);
		ifnum = 1;
	}

	for(i = 0; i < ifnum; i++)
	{
	#if 0
	    	command.imr_multiaddr.s_addr = inet_addr("239.255.255.250"); 
	    	command.imr_interface.s_addr = htonl(INADDR_ANY);
	   	ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command));
	    	if (ret < 0)
	    	{   
	        	printf("ret=%d\n", ret);
	        	perror("setsockopt:IP_ADD_MEMBERSHIP");
	        	close(sockfd);
	        	return (-1);
	    	}
	#endif

		if (interfacename[i][0] == 'e')
		{
			QMAPI_NET_NETWORK_CFG network_cfg;
			memset(&network_cfg, 0, sizeof(network_cfg));
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0, &network_cfg, sizeof(network_cfg));
		
			unsigned char *pMacAddr = network_cfg.stEtherNet[0].byMACAddr;
			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
				pMacAddr[0], pMacAddr[1], pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5],
				pMacAddr[0], pMacAddr[1], pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5]);
			
			strcpy(ipaddr, network_cfg.stEtherNet[0].strIPAddr.csIpV4);
			//printf("**************%s: eth addr is %s*************\n", __FUNCTION__, ipaddr);
		}
		else
		{
			char macAddr[6];
			char szMac[20];
			net_get_hwaddr(WIFI_NAME, szMac, macAddr);

			if (get_ip_addr(WIFI_NAME, ipaddr))
			{
				continue;
			}
			
			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

			//printf("**************%s: WIFI addr is %s*************\n", __FUNCTION__, ipaddr);
		}
	
    	memset(&client, 0, sizeof(client));  
    	client.sin_family = AF_INET;  
    	client.sin_port = htons(3702);  
		inet_pton(AF_INET, "239.255.255.250", &client.sin_addr);

		int count = 0;
		struct timeval tv;
		char uuid[64];
		int len;
		for (count = 0; count < 2; ++count)
		{
			uuid_generate_random(out);
			uuid_unparse(out,uuid);
			
			len = sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
				"<SOAP-ENV:Envelope"
				" xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\""
				" xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\""
				" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\""
				" xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\""
				" xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\">"
				"<SOAP-ENV:Header>"
				"<wsa:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye</wsa:Action>"
				"<wsa:MessageID>uuid:%s</wsa:MessageID>"
				"<wsa:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>"
				"</SOAP-ENV:Header>"
				"<SOAP-ENV:Body>"
				"<d:Bye>"
				"<wsa:EndpointReference>"
				"<wsa:Address>urn:uuid:%s</wsa:Address>"
				"</wsa:EndpointReference>"
				"</d:Bye>"
				"</SOAP-ENV:Body>"
				"</SOAP-ENV:Envelope>", uuid, endpointId);
		
			//printf("send bye message from %s\n", endpointId);

			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (sockfd < 0)
			{
    			perror("Creatingsocket failed.");
    			return ;
			}
	
			if (ifnum == 2)
			{
				struct sockaddr_in my_addr = {0};
				my_addr.sin_family = AF_INET;
				inet_pton(AF_INET, ipaddr, &my_addr.sin_addr);//这个IP就是我选择要发送广播包的网卡的IP
				bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
			}
			
        	if (sendto(sockfd, buf, len, 0, (struct sockaddr *)&client, sizeof(client)) < 0)
        	{
        		printf("%s  %d, sendto error! errno: %d, %s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        		close(sockfd);
				return ;
        	}
			close(sockfd);

			tv.tv_sec = 0;
			tv.tv_usec = 50000;
			select(0, NULL, NULL, NULL, &tv);
		}
	}
}

static int match_scopes(const char *scope)
{
	//printf("scope1: %s\n", scope);
	if (!*scope)
	{
		return 0;
	}
	else if (*scope != '/')
	{
		return -1;
	}
	
	++scope;
	//printf("scope2: %s\n", scope);
	
	if (!*scope || !strcmp(scope, "type") || !strcmp(scope, "name") || !strcmp(scope, "hardware") || !strcmp(scope, "Profile"))
	{
		return 0;
	}
	
	int ret = -1;
    QMAPI_SYSCFG_ONVIFTESTINFO stONVIFTset;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &stONVIFTset, sizeof(QMAPI_SYSCFG_ONVIFTESTINFO)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_ONVIFTESTINFO failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
	if (!strcmp(stONVIFTset.szDeviceScopes1, scope)
		|| !strcmp(stONVIFTset.szDeviceScopes2, scope)
		|| !strcmp(stONVIFTset.szDeviceScopes3, scope))
	{
		ret = 0;
	}
	
	return ret;
}

static int skip_ns(char **buffer, const char *ns, const char *url, int urllen)
{
	int ret = 0;

	char *p = *buffer;
	//printf("str4: %s\n", p);
	while (*p && isspace(*p))
	{
		++p;
	}

	//printf("str5: %s\n", p);
	if (!strncmp(p, ns, strlen(ns)))
	{
		p += strlen(ns);
		//printf("str1: %s\n", p);
		while (*p && (*p != '=') && (*p != '>'))
		{
			++p;
		}

		//printf("str2: %s\n", p);
		if (*p == '=')
		{
			++p;
			while (*p && (isspace(*p) || (*p == '\"')))
			{
				++p;
			}

			//printf("str3: %s\n", p);
			if (strncmp(p, url, urllen))
			{
				ret = -1;
			}
		}
		else
		{
			ret = -1;
		}
	}

	*buffer = p;
	return ret;
}

#define MAX_RECV 4096

struct client_msg
{
	struct sockaddr_in client_addr;
	char recvbuf[MAX_RECV];
	int datalen;
};

static int probe_process(struct client_msg *clientinfo)
{
	char *p, *q;
	//int sendlen;
	int reply_sock;
	//char ifname[32] = {0};
	const char *ipaddr;
	char clientMsgId[64] = {0};
	char sendbuf[2048];
	char endpointId[64];

	//printf("datalen:%d, recvbuf:%s\n", clientinfo->datalen, clientinfo->recvbuf);
	if (!strstr(clientinfo->recvbuf, "Probe>"))
	{
    	return -1;
	}

	//printf("#### %s %d, recvbuf:%s\n", __FUNCTION__, __LINE__, clientinfo->recvbuf);
	do
	{
    	p = strstr(clientinfo->recvbuf, "MessageID");
		if (p)
		{
			p += strlen("MessageID");
			if (!skip_ns(&p, "xmlns", "http://schemas.xmlsoap.org/ws/2004/08/addressing\"",
				strlen("http://schemas.xmlsoap.org/ws/2004/08/addressing\"")))
			{
				//printf("buffer is %s\n", p);
				break;
			}
		}
		//else
		//{
		//	printf("is not match 6!\n");
		//}

		//printf("not match3!\n");
	      return -1;
	} while (0);

	q = strstr(p, "uuid:");
	if (q)
	{
		strncpy(clientMsgId, q, 41);
	}
	else
	{
       p = strchr(p, '>');
       if (!p)
       {
           return -1;
       }

	 	p += 1;
        if ((*p>='0'&&*p<='9')|| (*p>='A'&&*p<='F') || (*p>='a'&&*p<='f'))
        {
        	strncpy(clientMsgId, p, 36);
        }
        else
        {
        	return -1;
        }
	}

	int i;
	if ((p = strstr(clientinfo->recvbuf, "Scopes")) != NULL)
	{
		p += strlen("Scopes");
		//printf("scope is %30s\n", p);

		while (*p && isspace(*p))
		{
			++p;
		}

		//printf("scope2 is %s\n%c %c\n", p, p[0], p[1]);

		if (!((p[0] == '/') && p[1] && (p[1] == '>')))
		{
			printf("run as ppppppppppppppppppppppppppppppp\n");
			if (!strncmp(p, "MatchBy", strlen("MatchBy")))
			{
				if (skip_ns(&p, "MatchBy", "http://schemas.xmlsoap.org/ws/2005/04/discovery/",
					strlen("http://schemas.xmlsoap.org/ws/2005/04/discovery/")))
				{
					return -1;
				}
			}
			else if (*p && *p++ == '>')
			{
				while (*p && isspace(*p))
				{
					++p;
				}

				//printf("scope3 is %s\n", p);
				if (*p && (*p != '<') )
				{
					if (strncmp(p, "onvif://www.onvif.org", strlen("onvif://www.onvif.org")))
					{
						return -1;
					}
					else
					{
						i = 0;
						char temp[200];
						while (*p && *p != '<')
						{
							//printf("string is %s\n", p);
							temp[0] = '\0';
								
							i = 0;
							p += strlen("onvif://www.onvif.org");
							while (*p && *p != ' ' && *p != '<'  && i < 199)
							{
								temp[i++] = *p++;
							}
							temp[i] = '\0';
							//printf("get scope is %s\n", temp);

							if (match_scopes(temp))
							{
								return -1;
							}

							if (*p == ' ')
							{
								while (*p && *p == ' ')
								{
									++p;
								}
							}
						}
					}
				}
			}
		}
	}

	//printf("########start send response messag###########\n");

	char interfacename[2][8];
	int ifnum = 0;
    QMAPI_NET_WIFI_CONFIG stWiFiConfig;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWiFiConfig, sizeof(QMAPI_NET_WIFI_CONFIG)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_WIFICFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
	if (stWiFiConfig.bWifiEnable > 0)
	{
		printf("checkdev: %d\n", checkdev(ETH_NAME));
		if (checkdev(ETH_NAME) > 0)
		{
 			strcpy(interfacename[0], ETH_NAME);
    		strcpy(interfacename[1], WIFI_NAME);
    		ifnum = 2;
		}
		else
		{
    		strcpy(interfacename[0], WIFI_NAME);
			ifnum = 1;		
		}
	}
	else
	{
    	strcpy(interfacename[0], ETH_NAME);
		ifnum = 1;
	}

	char tempAddr[QMAPI_MAX_IP_LENGTH] = {0};
	char uuid[64];
	uuid_t out;
	int num;
    QMAPI_NET_NETWORK_CFG stNetWorkCfg;
    if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &stNetWorkCfg, sizeof(QMAPI_NET_NETWORK_CFG)) != 0)
    {
        printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_NETCFG failed!\n",__FUNCTION__, __LINE__);
        return -1;
    }
	for (i = 0; i < ifnum; i++)
	{
	    uuid_generate_random(out);
	    uuid_unparse(out, uuid);

		if (interfacename[i][0] == 'e')
		{
			ipaddr = stNetWorkCfg.stEtherNet[0].strIPAddr.csIpV4;

			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
        				stNetWorkCfg.stEtherNet[0].byMACAddr[0], stNetWorkCfg.stEtherNet[0].byMACAddr[1],
        				stNetWorkCfg.stEtherNet[0].byMACAddr[2], stNetWorkCfg.stEtherNet[0].byMACAddr[3],
        				stNetWorkCfg.stEtherNet[0].byMACAddr[4], stNetWorkCfg.stEtherNet[0].byMACAddr[5],
        				stNetWorkCfg.stEtherNet[0].byMACAddr[0], stNetWorkCfg.stEtherNet[0].byMACAddr[1],
        				stNetWorkCfg.stEtherNet[0].byMACAddr[2], stNetWorkCfg.stEtherNet[0].byMACAddr[3],
        				stNetWorkCfg.stEtherNet[0].byMACAddr[4], stNetWorkCfg.stEtherNet[0].byMACAddr[5]);
		}
		else
		{	
			if (get_ip_addr(WIFI_NAME, tempAddr))
			{
				continue;
			}
			ipaddr = tempAddr;

			char macAddr[6];
			char szMac[20];
			net_get_hwaddr(WIFI_NAME, szMac, macAddr);
			
			sprintf(endpointId, "%02x%02x%02x%02x-%02x%02x-4adf-8702-%02x%02x%02x%02x%02x%02x",
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
				macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
		}
		
	   	p = sendbuf;
    	p += sprintf(p, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    		"<SOAP-ENV:Envelope"
    		" xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\""
    		" xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\""
    		" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""
    		" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\""
    		" xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\""
    		" xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\">"
			"<SOAP-ENV:Header>");
	    
    	p += sprintf(p, "<wsa:MessageID>uuid:%s</wsa:MessageID>", uuid);
    	p += sprintf(p, "<wsa:RelatesTo>%s</wsa:RelatesTo>", clientMsgId);
    	p += sprintf(p, "<wsa:To>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</wsa:To>");
    	p += sprintf(p, "<wsa:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches</wsa:Action>");
    	p += sprintf(p, "</SOAP-ENV:Header>");
    	p += sprintf(p, "<SOAP-ENV:Body>");
    	p += sprintf(p, "<d:ProbeMatches>");
    	p += sprintf(p, "<d:ProbeMatch>");
    	p += sprintf(p, "<wsa:EndpointReference>");
    	p += sprintf(p, "<wsa:Address>urn:uuid:%s</wsa:Address>", endpointId);
    	p += sprintf(p, "</wsa:EndpointReference>");
    	p += sprintf(p, "<d:Types>dn:NetworkVideoTransmitter</d:Types>");
    	p += sprintf(p, "<d:Scopes>");
    	p += sprintf(p, "onvif://www.onvif.org/type/video_encoder");
    	p += sprintf(p, " onvif://www.onvif.org/type/audio_encoder");
    	p += sprintf(p, " onvif://www.onvif.org/type/ptz");
    	p += sprintf(p, " onvif://www.onvif.org/hardware/IPCAM");

		p += sprintf(p, " onvif://www.onvif.org/Profile/Streaming");
		p += sprintf(p, " onvif://www.onvif.org/type/network_video_transmitter");

    	//p += sprintf(p, "%s", " onvif://www.onvif.org/location/country/china");
    	//p += sprintf(p, "%s", " onvif://www.onvif.org/location/city/shenzhen");

		printf("func: %s LINE: %d\n", __func__, __LINE__);

		char csChannelName[QMAPI_NAME_LEN * 2];
		size_t lenChannelName = sizeof(csChannelName);
        QMAPI_SYSCFG_ONVIFTESTINFO stONVIFTset;
        if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &stONVIFTset, sizeof(QMAPI_SYSCFG_ONVIFTESTINFO)) != 0)
        {
            printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_ONVIFTESTINFO failed!\n",__FUNCTION__, __LINE__);
            return -1;
        }
        QMAPI_NET_CHANNEL_PIC_INFO stChannelInfo;
        if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, 0, &stChannelInfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != 0)
        {
            printf("%s  %d, QMapi_sys_ioctrl QMAPI_SYSCFG_GET_PICCFG failed!\n",__FUNCTION__, __LINE__);
            return -1;
        }
    	
		code_convert("GB2312", "UTF-8", stChannelInfo.csChannelName, strlen(stChannelInfo.csChannelName),
			csChannelName, &lenChannelName);
    	p += sprintf(p, " onvif://www.onvif.org/name/%s", csChannelName);

		if (stONVIFTset.szDeviceScopes1[0])
		{
    		p += sprintf(p, " onvif://www.onvif.org/%s", stONVIFTset.szDeviceScopes1);
		}
		if (stONVIFTset.szDeviceScopes2[0])
		{
    		p += sprintf(p, " onvif://www.onvif.org/%s", stONVIFTset.szDeviceScopes2);
		}
		if (stONVIFTset.szDeviceScopes3[0])
		{
    		p += sprintf(p, " onvif://www.onvif.org/%s", stONVIFTset.szDeviceScopes3);
		}		
    	p += sprintf(p, "</d:Scopes>");

		//printf("#### %s %d, interfacename:%s, ipaddr:%s\n", __FUNCTION__, __LINE__, interfacename[i], ipaddr);
		if (stNetWorkCfg.wHttpPort == 80)
    	{
        	p += sprintf(p, "<d:XAddrs>http://%s/onvif/device_service</d:XAddrs>", ipaddr);
    	}
    	else
		{
			p += sprintf(p, "<d:XAddrs>http://%s:%d/onvif/device_service</d:XAddrs>", ipaddr, stNetWorkCfg.wHttpPort);
    	}
    	p += sprintf(p, "<d:MetadataVersion>1</d:MetadataVersion>");
    	p += sprintf(p, "</d:ProbeMatch>");
    	p += sprintf(p, "</d:ProbeMatches>");
    	p += sprintf(p, "</SOAP-ENV:Body>");
    	p += sprintf(p, "</SOAP-ENV:Envelope>");

    	reply_sock = socket(AF_INET, SOCK_DGRAM, 0);
    	if (reply_sock < 0)
    	{  
        	perror("Creating reply socket failed.");
        	return -1;
    	}

		int set = 1;
		if(setsockopt(reply_sock, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set)))
		{
			perror("setsockopt:SO_REUSEPORT");
			return -1;
		}

		//clientinfo->client_addr.sin_family = AF_INET;
		//printf("#### %s %d, sin_port:0x%X\n", __FUNCTION__, __LINE__, clientinfo->client_addr.sin_port);

		if (ifnum == 2)
		{
			//printf("################bind to deviceip:%s********************\n", ipaddr);
			struct sockaddr_in my_addr = {0};
			my_addr.sin_family = AF_INET;
			my_addr.sin_port = htons(3702);
			inet_pton(AF_INET, ipaddr, &my_addr.sin_addr);//这个IP就是我选择要发送广播包的网卡的IP
			bind(reply_sock, (struct sockaddr *)&my_addr, sizeof(my_addr));
		}
		else
		{
			struct sockaddr_in my_addr = {0};
			my_addr.sin_family = AF_INET;
			my_addr.sin_port = htons(3702);
			my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			bind(reply_sock, (struct sockaddr *)&my_addr, sizeof(my_addr));		
		}

    	//sendlen = strlen(sendbuf);
    	//printf("#### %s %d, %s\n",  __FUNCTION__, __LINE__, inet_ntoa(clientinfo->client_addr.sin_addr));

    	//printf("#### %s %d, sendbuf:%s\n", __FUNCTION__, __LINE__, sendbuf);
    	clientinfo->client_addr.sin_family = AF_INET;
      	num = sendto(reply_sock, sendbuf, p - sendbuf, 0, (struct sockaddr *)&clientinfo->client_addr, sizeof(struct sockaddr_in));
		close(reply_sock);

		if (num < 0)
       	{
       		printf("%s  %d, %s reply_sock=%d, errno: %d, %s\n",__FUNCTION__, __LINE__, interfacename[i], reply_sock, errno, strerror(errno));     	
            return -1;
      	}
	}

    return 0;
}

#if 0
static void ReadProbeConfig(struct onvif_probe_para *pConfig)
{
	memset(pConfig, 0, sizeof(struct onvif_probe_para));

	DMS_NET_ONVIFTEST_INFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	QMapi_sys_ioctrl(0, DMS_NET_GET_ONVIFTEST_INFO, 0, &info, sizeof(info));

	onvif_ipc_config.discoveryMode = info.dwDiscoveryMode;
	strcpy(pConfig->szDeviceScopes[0], info.szDeviceScopes1);
	strcpy(pConfig->szDeviceScopes[1], info.szDeviceScopes2);
	strcpy(pConfig->szDeviceScopes[2], info.szDeviceScopes3);
	
	QMAPI_NET_WIFI_CONFIG wifi_cfg;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &wifi_cfg, sizeof(wifi_cfg));
	pConfig->bWifiEnable = wifi_cfg.bWifiEnable;

	QMAPI_NET_NETWORK_CFG network_cfg;
	memset(&network_cfg, 0, sizeof(network_cfg));
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0, &network_cfg, sizeof(QMAPI_NET_NETWORK_CFG));
	memcpy(pConfig->byEthMACAddr, network_cfg.stEtherNet[0].byMACAddr, DMS_MACADDR_LEN);
	strcpy(pConfig->csEthIpAddr, network_cfg.stEtherNet[0].strIPAddr.csIpV4);
	pConfig->httpPort = network_cfg.wHttpPort;
	
	DMS_NET_CHANNEL_OSDINFO osd_info;
	memset(&osd_info, 0, sizeof(osd_info));
	QMapi_sys_ioctrl(0, DMS_NET_GET_OSDCFG, 0, &osd_info, sizeof(osd_info));
	size_t lenChannelName = sizeof(pConfig->csChannelName);
	code_convert("GB2312", "UTF-8", osd_info.csChannelName, strlen(osd_info.csChannelName), 
		pConfig->csChannelName, &lenChannelName);
}
#endif

typedef struct _iphdr //定义IP首部 
{ 
    unsigned char h_verlen; //4位首部长度+4位IP版本号 
    unsigned char tos; //8位服务类型TOS 
    unsigned short total_len; //16位总长度（字节） 
    unsigned short ident; //16位标识 
    unsigned short frag_and_flags; //3位标志位 
    unsigned char ttl; //8位生存时间 TTL 
    unsigned char proto; //8位协议 (TCP, UDP 或其他) 
    unsigned short checksum; //16位IP首部校验和 
    unsigned int sourceIP; //32位源IP地址 
    unsigned int destIP; //32位目的IP地址 
} IP_HEADER; 

typedef struct _udphdr //定义UDP首部
{
    unsigned short uh_sport;    //16位源端口
    unsigned short uh_dport;    //16位目的端口
    unsigned int uh_len;//16位UDP包长度
    unsigned int uh_sum;//16位校验和
} UDP_HEADER;

int ONVIFSearchCreateSocket(int *pSocketType)
{
	int sockfd;
#if 0
    QMAPI_NET_WIFI_CONFIG stWifiConfig;
    QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));
	if (stWifiConfig.bWifiEnable > 0)
	{
		sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
		if (sockfd < 0)
		{ 
			perror("Creatingsocket failed.");
			return -1; 
		}

		if (pSocketType)
		{
			*pSocketType = 1;
		}
	}
	else
#endif		
	{
		struct sockaddr_in server;
		struct ip_mreq command;
		
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0)
		{
			perror("Creatingsocket failed.");
			return -1;
		}

		printf("new sockfd is %d\n", sockfd);

		memset(&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(3702);
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)	
		{
			perror("Bind()error.");
			close(sockfd);
			return -1;
		}

		int set = 1;
		int ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &set, sizeof(set));
		if (ret < 0)
		{
			printf("%s %d, setsockopt error!err:%s(%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
			close(sockfd);
			return -1;
		}

		inet_pton(AF_INET, "239.255.255.250", &server.sin_addr);
		command.imr_multiaddr.s_addr = server.sin_addr.s_addr;
		command.imr_interface.s_addr = htonl(INADDR_ANY);
		ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command));
		if (ret < 0)
		{
			perror("setsockopt:IP_ADD_MEMBERSHIP");
			close(sockfd);
			return -1;
		}

		set = 1;
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set)))
		{
			perror("setsockopt:SO_REUSEPORT");
			return -1;
		}

		if (pSocketType)
		{
			*pSocketType = 0;
		}		
	}

	return sockfd;
}

int OnvifSearchRequestProc(int dmsHandle, int sockfd, int socketype)
{
	int num;
	struct client_msg ClientMsg;

	//printf("bwifi enable is %d\n", probe_cfg.bWifiEnable);
	if (socketype == 1)
	{
		char buf[42];

		IP_HEADER *ip;
		//addrlen = sizeof(struct sockaddr_in);

		struct iovec vec[2];
		//printf("start recv.\n");
		vec[0].iov_base = buf;
		vec[0].iov_len = 42;
		vec[1].iov_base = ClientMsg.recvbuf;
		vec[1].iov_len = MAX_RECV - 1;//预留一个字节做空字符
		
		struct msghdr header = {0};
		header.msg_iov = vec;
		header.msg_iovlen = 2;
		num = recvmsg(sockfd, &header, 0);
		if (num == -1)
		{
			printf("recv error!\n");
			return -1;
		}
		else if (!num || (num <= 42))
		{
			return 0;
		}

		//接收数据不包括数据链路帧头
		ip = (IP_HEADER *)(buf+14);

		//analyseIP(ip);
		if (ip->proto == IPPROTO_UDP)
		{
			unsigned char *p = (unsigned char*)&ip->destIP;
			size_t iplen =	(ip->h_verlen&0x0f)*4;
			UDP_HEADER *udp = (UDP_HEADER *)(buf+14 + iplen);
			if (3702 == ntohs(udp->uh_dport) && (p[0] == 239 && p[1] == 255 && p[2] == 255 && p[3] == 250))
			{
				char tmp[16];

				//analyseIP(ip);
				//analyseUDP(udp);
				memset(&ClientMsg.client_addr, 0, sizeof(ClientMsg.client_addr));

				p = (unsigned char*)&ip->sourceIP;
				sprintf(tmp, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
				//printf("from:%s\n", tmp);
				ClientMsg.datalen = num - 42;
				ClientMsg.recvbuf[ClientMsg.datalen] = '\0';
				//memcpy(ClientMsg.recvbuf, buf+42, num-42);
				ClientMsg.client_addr.sin_family = AF_INET;
				ClientMsg.client_addr.sin_port = udp->uh_sport;
				inet_pton(AF_INET, tmp, &ClientMsg.client_addr.sin_addr);

				//printf("recvfrom: %s", tmp);

				if (onvif_ipc_config.discoveryMode)
				{
					probe_process(&ClientMsg);
				}
				else
				{
					printf("no response!\n");
				}
			}
		}
	}
	else
	{
		socklen_t addrlen = sizeof(struct sockaddr_in);

		g_nw_search_run_index = 100;
		//printf("\n start recv.\n");
		//memset(&ClientMsg.client_addr, 0, sizeof(ClientMsg.client_addr));

		num = recvfrom(sockfd, ClientMsg.recvbuf, 4095, 0, (struct sockaddr*)&ClientMsg.client_addr, &addrlen);								
		if (num < 0)  
		{  
			if (errno == EINTR || errno == EAGAIN)
			{
				return 0;
			}
		
			printf("%s	%d, recvfrom error: %s(%d)\n",__FUNCTION__, __LINE__, strerror(errno),errno);
			return -1;  
		}
		g_nw_search_run_index = 101;

		ClientMsg.recvbuf[num] = '\0';
		ClientMsg.datalen = num;
#if 1
        char tmp[40];
        inet_ntop(AF_INET, &ClientMsg.client_addr.sin_addr, tmp, 40);
        printf("from:%s\n", tmp);
#endif
		g_nw_search_run_index = 102;
		if (onvif_ipc_config.discoveryMode)
		{
			probe_process(&ClientMsg);
		}
		else
		{
			printf("no response!\n");
		}
	}

	return 0;
}

void Onvif_ResetLinkState()
{
	printf("**********%s*******************\n", __FUNCTION__);
	onvif_send_hello();
}

typedef	unsigned int	__u32;
typedef	unsigned short	__u16;
typedef	unsigned char	__u8;

struct uuid 
{
    __u32   time_low;
    __u16   time_mid;
    __u16   time_hi_and_version;
    __u16   clock_seq;
    __u8    node[6];
};

void uuid_pack(const struct uuid *uu, uuid_t ptr)
{
    __u32   tmp;
    unsigned char   *out = ptr;
    tmp = uu->time_low;
    out[3] = (unsigned char) tmp;
    tmp >>= 8;
    out[2] = (unsigned char) tmp;
    tmp >>= 8;
    out[1] = (unsigned char) tmp;
    tmp >>= 8;
    out[0] = (unsigned char) tmp;
    tmp = uu->time_mid;
    out[5] = (unsigned char) tmp;
    tmp >>= 8;
    out[4] = (unsigned char) tmp;
    tmp = uu->time_hi_and_version;
    out[7] = (unsigned char) tmp;
    tmp >>= 8;
    out[6] = (unsigned char) tmp;
    tmp = uu->clock_seq;
    out[9] = (unsigned char) tmp;
    tmp >>= 8;
    out[8] = (unsigned char) tmp;
    memcpy(out+10, uu->node, 6);
}

void uuid_unpack(const uuid_t in, struct uuid *uu)
{
    const __u8  *ptr = in;
    __u32       tmp;
    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    tmp = (tmp << 8) | *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_low = tmp;
    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_mid = tmp;
    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_hi_and_version = tmp;
    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->clock_seq = tmp;
    memcpy(uu->node, ptr, 6);
}

void uuid_unparse(const uuid_t uu, char *out)
{
    struct uuid uuid;
    uuid_unpack(uu, &uuid);
    sprintf(out,"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,uuid.clock_seq >> 8,
        uuid.clock_seq & 0xFF,uuid.node[0], uuid.node[1], uuid.node[2],uuid.node[3],
        uuid.node[4], uuid.node[5]);
}

#ifdef HAVE_SRANDOM
#define srand(x) 	srandom(x)
#define rand() 		random()
#endif
static int get_random_fd(void)
{
    struct timeval	tv;
    static int	fd = -2;
    int		i;
    if (fd == -2)
    {
        gettimeofday(&tv, 0);
        fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1)
            fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
        srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);
    }
    /* Crank the random number generator a few times */
    gettimeofday(&tv, 0);
    for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
        rand(); 
    return fd;
}
static void get_random_bytes(void *buf, int nbytes)
{
    int i, n = nbytes, fd = get_random_fd();
    int lose_counter = 0;
    unsigned char *cp = (unsigned char *) buf;
    if (fd >= 0) 
    {
        while (n > 0)
        {
            i = read(fd, cp, n);
            if (i <= 0) 
            {
                if (lose_counter++ > 16)
                    break;
                continue;
            }
            n -= i;
            cp += i;
            lose_counter = 0;
            }
        }
    /*   
    * We do this all the time, but this is the only source of   
    * randomness if /dev/random/urandom is out to lunch.    
    */ 
    for (cp = buf, i = 0; i < nbytes; i++)
        *cp++ ^= (rand() >> 7) & 0xFF;
    return;
}

void uuid_generate_random(uuid_t out)
{
    uuid_t  buf;
    struct uuid uu;

    get_random_bytes(buf, sizeof(buf));
    uuid_unpack(buf, &uu);
    uu.clock_seq = (uu.clock_seq & 0x3FFF) | 0x8000;
    uu.time_hi_and_version = (uu.time_hi_and_version & 0x0FFF) | 0x4000;
    uuid_pack(&uu, out);
}

int Onvif_Alarm_Out(int nChannel, int nAlarmType, int nAction, void* pParam)
{
	if(onvif_module_deinit == 1)
		return 0;
	
	if(nAlarmType == QMAPI_ALARM_TYPE_VMOTION)
	{
		onvif_ipc_config.MAlarm_status |= (1<<nChannel);
		
		//printf("time:%ld ch%d is motion alarm!\n", time(NULL), nChannel);
	}
	else if((nAlarmType == QMAPI_ALARM_TYPE_VMOTION_RESUME)&&(onvif_ipc_config.MAlarm_status&(1<<nChannel)))
	{
		onvif_ipc_config.MAlarm_status &= ~(1<<nChannel);
		//printf("time:%ld ch%d is motion alarm clear!\n", time(NULL), nChannel);
	}

	if(nAlarmType == QMAPI_ALARM_TYPE_ALARMIN)
	{
		onvif_ipc_config.IOAlarm_status |= (1<<nChannel);
	}
	else if((nAlarmType == QMAPI_ALARM_TYPE_ALARMIN_RESUME)&&(onvif_ipc_config.IOAlarm_status&(1<<nChannel)))
	{
		onvif_ipc_config.IOAlarm_status &= ~(1<<nChannel);
	}

	return 0;
}

int Onvif_Proc(httpd_conn* hc)
{
	if(onvif_module_deinit)
		return -1;

	int (*func)(httpd_conn *, struct onvif_config *);
	func = GetDlFunction("Onvif_Server");	
	if (func)
	{
		(*func)(hc, &onvif_ipc_config);
	}

	ReleaseDlFunction();
	
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	switch(onvif_ipc_config.system_reboot_flag)
	{
	case NORMAL_REBOOT:
		onvif_send_bye(0);
		select(0, NULL, NULL, NULL, &tv);
		
		QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, 0, 0);
		break;
	case RESTORE_REBOOT:
		{
			onvif_send_bye(0);
			select(0, NULL, NULL, NULL, &tv);

			QMAPI_NET_RESTORECFG stRestorecfg;
			memset(&stRestorecfg, 0, sizeof(stRestorecfg));
			stRestorecfg.dwSize = sizeof(stRestorecfg);
			stRestorecfg.dwMask = NORMAL_CFG | VENCODER_CFG | RECORD_CFG | RS232_CFG | ALARM_CFG | VALARM_CFG | PTZ_CFG
								| PREVIEW_CFG;// | TVMODE_CFG | COLOR_CFG;
			
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_RESTORECFG, 0, &stRestorecfg, sizeof(stRestorecfg));
		}
		break;
	case RESET_REBOOT:
		{
			onvif_send_bye(0);
			select(0, NULL, NULL, NULL, &tv);
		
			QMAPI_NET_RESTORECFG stRestorecfg;
			memset(&stRestorecfg, 0, sizeof(stRestorecfg));
			stRestorecfg.dwSize = sizeof(stRestorecfg);
			stRestorecfg.dwMask = NORMAL_CFG | VENCODER_CFG | RECORD_CFG | RS232_CFG | ALARM_CFG | VALARM_CFG | PTZ_CFG 
								| PREVIEW_CFG/* | TVMODE_CFG | COLOR_CFG*/ | USER_CFG | NETWORK_CFG;
			
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_RESTORECFG, 0, &stRestorecfg, sizeof(stRestorecfg));
		}
		break;
	case AVSERVER_REBOOT:
		//system("killall avserver");
		break;
	case DISCOVERY_REBOOT:
		printf("%s set discovery reboot!\n", __FUNCTION__);
		Onvif_ResetLinkState();
		onvif_ipc_config.system_reboot_flag = REBOOT_FLAG_IDLE;
		break;
	default:
		break;
	}
	
	return -1;
}

