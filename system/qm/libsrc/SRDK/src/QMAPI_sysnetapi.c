

#include "QMAPIType.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"
#include "tl_common.h"

#include "h264nalu.h"
#include "librecord.h"
#include "hd_mng.h"
//#include "ptzmng.h"
#include "QMAPIUpgrade.h"
#include "tlnettools.h"
#include "Q3AV.h"
#include "QMAPINetWork.h"
#include "QMAPIAV.h"
#include "tlmisc.h"
#include "librecord.h"
#include "QMAPIRecord.h"
#include "sysconfig.h"
#include "systime.h"
#include "q3_wireless.h"
#include "q3_gpio.h"
#include "Q3Audio.h"
#include "libCamera.h"
#include "AudioPrompt.h"
#include "logmng.h"
#include "Default_Set.h"
#include "tl_authen_interface.h"

typedef struct tagDMS_SYSNETAPI
{
	QMAPI_NETPT_TYPE_E			euNetPtType;
	int							bMainVideoStarted;
	CBVideoProc 				pfMainVideoCallBk ;
	DWORD                       dwMainVideoUser;

	int							bSecondVideoStarted ;
	CBVideoProc 				pfSecondVideoCallBk ;
	DWORD                       dwSecondVideoUser;

	int							bMobileVideoStarted ;
	CBVideoProc 				pfMobileVideoCallBk ;
	DWORD                       dwMobileVideoUser;

	CBAlarmProc 				pfAlarmCallBk;

	CBAudioProc            		pfAudioStreamCallBk;
	DWORD                       dwAudioUser;

	CBSerialDataProc            pfnSerialDataCallBk;
	struct tagDMS_SYSNETAPI		*pNext;
}DMS_SYSNETAPI, *PDMS_SYSNETAPI;

extern void saveRecordParam();//保存录像选项
extern void saveChannelRecordParam();
extern int GetNetSupportStreamFmt(int nChannel, QMAPI_NET_SUPPORT_STREAM_FMT* lpstSupportFmt);
//extern int TL_Decoder_Audio_Frame(char *data, int dataSize);
extern int SetNetWorkConfig(char *IPAddr, char *IPMask, char *GatewayIpAddr, char *Dns1Addr, char *Dns2Addr);


#define EMAIL_SERVICE_PORT 25

//static int baudrate[11] = { 600, 1200, 2400, 4800, 9600, 19200, 38400,4300,5600, 57600,115200};
static int baudrate[] = {50, 75, 110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400,4300,5600, 57600,115200};

extern TL_FIC8120_FLASH_SAVE_GLOBEL g_tl_globel_param;

//不为0时屏蔽所有DMS接口的操作
int g_dms_sysnet_stop = 0;
DMS_SYSNETAPI *g_dms_sysnetapi = NULL;
extern unsigned char g_wifistatus;
CBSystemReboot  Qmapi_System_Reboot = NULL;

extern int ftp_putfile(TL_SERVER_MSG *lpJpeg, char *pData, TL_FTPUPDATA_PARAM *lpFtpParam,
	TLNV_SERVER_INFO *lpServer,char *lpChannelName,int type);


int NetVideoSecondStreamCallBack(int nChannel, int nStreamType, char* pData, unsigned int dwSize, QMAPI_NET_FRAME_HEADER stFrameHeader, DWORD dwUserData)
{
    DMS_SYSNETAPI *pSysNetApi = g_dms_sysnetapi;
    while(pSysNetApi)
    {
        if((0x1 & (pSysNetApi->bSecondVideoStarted>>nChannel) )
                && pSysNetApi->pfSecondVideoCallBk != NULL )
        {
            pSysNetApi->pfSecondVideoCallBk(nChannel, nStreamType, pData, dwSize, stFrameHeader, dwUserData);
        }
	 pSysNetApi = pSysNetApi->pNext;
    }
    
    return 0;
}

int NetVideoMainStreamCallBack(int nChannel, int nStreamType, char* pData, unsigned int dwSize, QMAPI_NET_FRAME_HEADER stFrameHeader, DWORD dwUserData)
{
    DMS_SYSNETAPI *pSysNetApi = g_dms_sysnetapi;
    while(pSysNetApi)
    {
        if((0x1 & (pSysNetApi->bMainVideoStarted>>nChannel) )
                && pSysNetApi->pfMainVideoCallBk != NULL )
        {
            pSysNetApi->pfMainVideoCallBk(nChannel, nStreamType, pData, dwSize, stFrameHeader, dwUserData);
        }       
  	 pSysNetApi = pSysNetApi->pNext;
  }
    
    return 0;
}

int NetAudioStreamCallBack(int *pData, unsigned int dwSize, int nType, DWORD dwUserData)
{
    DMS_SYSNETAPI *pSysNetApi = g_dms_sysnetapi;
    while(pSysNetApi)
    {
        if((NULL != pSysNetApi->pfAudioStreamCallBk)
            && ((0x1 & (pSysNetApi->bMainVideoStarted>>0)) || (0x1 & (pSysNetApi->bSecondVideoStarted>>0))))
        {
            pSysNetApi->pfAudioStreamCallBk(pData, dwSize, nType, dwUserData);
        }       
        pSysNetApi = pSysNetApi->pNext;
    }
    
    return 0;
}

int QMapi_sys_open(int nFlag)
{
	DMS_SYSNETAPI *pSysNetApi = NULL;
	DMS_SYSNETAPI  *p1;
	if(g_dms_sysnet_stop != 0)
	{
		return QMAPI_FAILURE;
	}
	pSysNetApi = g_dms_sysnetapi;
	while (pSysNetApi)
	{
		if (pSysNetApi->euNetPtType == nFlag)
		{
			break;
		}
		pSysNetApi = pSysNetApi->pNext;
	}

	if (pSysNetApi == NULL)
	{
		pSysNetApi = malloc(sizeof(DMS_SYSNETAPI));
		if(pSysNetApi == NULL) 
		{
			return QMAPI_FAILURE;
		}
		memset(pSysNetApi, 0, sizeof(DMS_SYSNETAPI));
		pSysNetApi->euNetPtType = (QMAPI_NETPT_TYPE_E)nFlag;

		if(g_dms_sysnetapi == NULL)
		{
			g_dms_sysnetapi = pSysNetApi;
		}
		else
		{
			p1 = g_dms_sysnetapi;
			pSysNetApi->pNext = p1;
			g_dms_sysnetapi = pSysNetApi;
		}
	}
	else
	{
		memset(pSysNetApi, 0, sizeof(DMS_SYSNETAPI));
		pSysNetApi->euNetPtType = (QMAPI_NETPT_TYPE_E)nFlag;
	}
	printf("#####QMapi_sys_open. %p. \n",pSysNetApi);
	return (int)(pSysNetApi);
}

int QMapi_sys_close(int Handle)
{
    return 0;
}

int QMapi_sys_start(int Handle, CBSystemReboot CallBack)
{
	int ret = -1;
	DMS_SYSNETAPI *pSysNetApi = (DMS_SYSNETAPI *)Handle;
	
	if (pSysNetApi && pSysNetApi->euNetPtType == QMAPI_NETPT_MAIN)
	{
		Qmapi_System_Reboot = CallBack;
		g_dms_sysnet_stop = 0;
		Q3_SetVideoStreamCallBack(QMAPI_MAIN_STREAM, &NetVideoMainStreamCallBack);
		Q3_SetVideoStreamCallBack(QMAPI_SECOND_STREAM, &NetVideoSecondStreamCallBack);
		QMAPI_AudioStreamCallBackSet(&NetAudioStreamCallBack);
		ret = 0;
	}
	
    return ret;
}

int QMapi_sys_stop(int Handle)
{
	DMS_SYSNETAPI *pSysNetApi = (DMS_SYSNETAPI *)Handle;
	
	if (pSysNetApi && pSysNetApi->euNetPtType == QMAPI_NETPT_MAIN)
	{
		g_dms_sysnet_stop = 1;
	}
	return 0;
}

int QMapi_sys_send_alarm(int nChannel, DWORD dwMsgType, QMAPI_SYSTEMTIME *SysTime)
{
    int alarmtype;
    DMS_SYSNETAPI *pSysNetApi;

	switch(dwMsgType)
	{
		case TL_MSG_MOVEDETECT:     //motion alarm
			alarmtype = QMAPI_ALARM_TYPE_VMOTION;
			break;
		case TL_MSG_SENSOR_ALARM:       //sensor alarm
			alarmtype = QMAPI_ALARM_TYPE_ALARMIN;
			break;
		case TL_MSG_VIDEOLOST:      //video lost
			alarmtype = QMAPI_ALARM_TYPE_VLOST;
			break;
		case TL_MSG_VIDEORESUME:
			alarmtype = QMAPI_ALARM_TYPE_VLOST_RESUME;
			break;
		case TL_MSG_MOVERESUME:
			alarmtype = QMAPI_ALARM_TYPE_VMOTION_RESUME;
			break;
		case TL_MSG_DISK_ERROR:
			alarmtype = QMAPI_ALARM_TYPE_DISK_RWERR;
			break;
		default:
			return -1;
	}
    pSysNetApi = g_dms_sysnetapi;
    while (pSysNetApi)
    {
        if (pSysNetApi->pfAlarmCallBk)
        {
            if(g_tl_globel_param.stConsumerInfo.stPrivacyInfo[g_tl_globel_param.stConsumerInfo.bPrivacyMode].stMotionInfo.bPushAlarm)
                pSysNetApi->pfAlarmCallBk(nChannel, alarmtype, QMAPI_STATE_ALARMIN_WARNN, (void *)SysTime);
        }
        pSysNetApi = pSysNetApi->pNext;
    }

    return 0;
}


static int QMapi_sys_set_netplatform_config(char *buffer)
{
    TLNV_NXSIGHT_SERVER_ADDR *nxsight_parameter=NULL;
    QMAPI_NET_PLATFORM_INFO_V2        stNetPlatCfg; //
    memset(&stNetPlatCfg, 0x00, sizeof(QMAPI_NET_PLATFORM_INFO_V2));
    if (buffer == NULL)
    {
        //err();
        printf("%s  %d, Invalid buffer:%p\n",__FUNCTION__, __LINE__, buffer);
        return -1;//TLERR_INVALID_HANDLE;
    }

    nxsight_parameter=(TLNV_NXSIGHT_SERVER_ADDR *)buffer;
    if(nxsight_parameter->dwSize == sizeof(TLNV_NXSIGHT_SERVER_ADDR))
    {
        struct in_addr stInAddr;
        memcpy(&stNetPlatCfg , &g_tl_globel_param.stNetPlatCfg , sizeof(QMAPI_NET_PLATFORM_INFO_V2));
        stNetPlatCfg.bEnable = nxsight_parameter->bEnable;
        stNetPlatCfg.dwCenterPort = nxsight_parameter->wCenterPort;
        strcpy(stNetPlatCfg.csServerNo, nxsight_parameter->csServerNo);
        stInAddr.s_addr = nxsight_parameter->dwCenterIp;
        strcpy(stNetPlatCfg.strIPAddr.csIpV4, inet_ntoa(stInAddr));
    }
    else if(nxsight_parameter->dwSize == sizeof(QMAPI_NET_PLATFORM_INFO_V2))
    {
        memcpy(&stNetPlatCfg, (QMAPI_NET_PLATFORM_INFO_V2 *)buffer, sizeof(QMAPI_NET_PLATFORM_INFO_V2));
    }
    else
    {
        //err();
        printf("%s  %d, Invalid platform param size:%lu\n",__FUNCTION__, __LINE__,nxsight_parameter->dwSize);
        return -1;//TLERR_INVALID_PARAM;     
    }    
    memcpy(&g_tl_globel_param.stNetPlatCfg , &stNetPlatCfg , sizeof(QMAPI_NET_PLATFORM_INFO_V2));

    return 0;//TL_SUCCESS;  
}

static int QMapi_check_handle(int nHandle)
{
	DMS_SYSNETAPI  *pstSysNetApi = (DMS_SYSNETAPI  *)nHandle;
	if(pstSysNetApi)
	{
		DMS_SYSNETAPI  *p = g_dms_sysnetapi;
		while(p)
		{
			if(p == pstSysNetApi)
				return 0;

			p = p->pNext;
		}
	}
	else
		return 0;
	
	return -1;
}

int QMapi_sys_ioctrl(int Handle, int nCmd, int nChannel, void* Param, int nSize)
{
    //if(QMAPI_SYSCFG_GET_RECORDCFG != nCmd)
  	//    printf("#### QMapi_sys_ioctrl Handle:0x%x, cmd: 0x%X, channel:%d\n", Handle, nCmd, nChannel);
    int ret = 0;
    int nResult = QMAPI_SUCCESS;
    DMS_SYSNETAPI  *pstSysNetApi = (DMS_SYSNETAPI  *)Handle;

	ret = QMapi_check_handle(Handle);
	if(ret)
	{
		printf("%s  %d, Invalid handle:0x%x\n",__FUNCTION__, __LINE__, Handle);
		return QMAPI_FAILURE;
	}
/*
    if(pstSysNetApi == NULL)
    {
        return DMS_ERR_INVALID_HANDLE;
    }
*/
    if(g_dms_sysnet_stop != 0 
		&& QMAPI_NET_STA_UPGRADE_RESP != nCmd 
		&& QMAPI_NET_STA_UPGRADE_DATA != nCmd
		&& QMAPI_NET_STA_UPGRADE_START != nCmd
		&& QMAPI_NET_STA_REBOOT != nCmd)
    {
        return 1;
    }
    
    switch(nCmd)
    {
#ifndef TL_Q3_PLATFORM

    case QMAPI_SYSCFG_GET_USERKEY:
    {
        QMAPI_NET_USER_KEY *pData = (QMAPI_NET_USER_KEY*)Param;
        if(NULL == pData)
        {
            break;
        }
        DMS_Read_Encrypt(pData->byData,(unsigned int)pData->nLen);
        break;
    }
    case QMAPI_SYSCFG_SET_USERKEY:
    {
        QMAPI_NET_USER_KEY *pData = (QMAPI_NET_USER_KEY*)Param;
        if(NULL == pData)
        {
            break;
        }
        DMS_Write_Encrypt(pData->byData,(unsigned int)pData->nLen);
        break;
    }
    case QMAPI_SYSCFG_GET_COLOR_BLACK_DETECTION:
    {
        if(nChannel < 0 || nChannel >= QMAPI_MAX_CHANNUM)
        {
            break;
        }
        memcpy(Param, &g_tl_globel_param.stCBDetevtion[nChannel], sizeof(g_tl_globel_param.stCBDetevtion[nChannel]));
        break;
    }
#endif
    case QMAPI_SYSCFG_SET_COLOR_BLACK_DETECTION:
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_DAY_NIGHT_DETECTION_EX *pDete = (QMAPI_NET_DAY_NIGHT_DETECTION_EX*)Param;
        if(NULL == pDete)
        {
            break;
        }
        memcpy(&g_tl_globel_param.stCBDetevtion[pDete->dwChannel], pDete, sizeof(*pDete));
        DMS_Vadc_DN_SetIr(g_tl_globel_param.stCBDetevtion[0].byIRCutLevel,g_tl_globel_param.stCBDetevtion[0].byLedLevel);
#endif
        break;
    }
    case QMAPI_SYSCFG_GET_DEVICECFG:         //获取设备参数
    {
    	memcpy(Param, &g_tl_globel_param.stDevicInfo, sizeof(QMAPI_NET_DEVICE_INFO));
        break;
    }
    case QMAPI_NET_STA_GET_SPSPPSENCODE_DATA:
    case QMAPI_SYSCFG_SET_ROICFG:       
    {
		QMAPI_AVServer_IOCtrl(Handle, nCmd, nChannel, Param, nSize);
    }
    break;
    case QMAPI_NET_STA_CLOSE_RS485:
    {
        break;
    }   
    case QMAPI_SYSCFG_SET_DEVICECFG:         //设置设备参数
    {
        QMAPI_NET_DEVICE_INFO *device_info = Param;
        if(0 != strcmp(g_tl_globel_param.stDevicInfo.csDeviceName,device_info->csDeviceName))
        {
            strcpy(g_tl_globel_param.stDevicInfo.csDeviceName,device_info->csDeviceName);
        }
        
        if(g_tl_globel_param.stDevicInfo.byVideoStandard != device_info->byVideoStandard)
        {
            QMAPI_NET_CHANNEL_COLOR_SINGLE pColorSingle = {0};
            QMAPI_NET_CHANNEL_COLOR *pColorParam = &g_tl_globel_param.TL_Channel_Color[nChannel];
            QMAPI_NET_CHANNEL_ENHANCED_COLOR *lpEnancedColor = &g_tl_globel_param.TL_Channel_EnhancedColor[nChannel];

            pColorSingle.dwChannel = nChannel;
            pColorSingle.dwSize    = sizeof(QMAPI_NET_CHANNEL_COLOR_SINGLE);
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_ANTIFLICKER;
            pColorSingle.nValue    = device_info->byVideoStandard;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);

            g_tl_globel_param.stDevicInfo.byVideoStandard = device_info->byVideoStandard;
        }
        memcpy(&g_tl_globel_param.stDevicInfo, Param, sizeof(QMAPI_NET_DEVICE_INFO));
        break;
    }

    case QMAPI_SYSCFG_GET_NETCFG:            //获取网络参数
    {
        memcpy(Param, &g_tl_globel_param.stNetworkCfg, sizeof(QMAPI_NET_NETWORK_CFG));
        break;
    }

    case QMAPI_SYSCFG_SET_NETCFG:            //设置网络参数
    {
        printf("#### %s %d\n",__FUNCTION__, __LINE__);
        QMAPI_NET_NETWORK_CFG *pNetworkCfg = (QMAPI_NET_NETWORK_CFG*)Param;
        if(!pNetworkCfg || pNetworkCfg->dwSize != sizeof(QMAPI_NET_NETWORK_CFG))
        {
            printf("%s  %d, Invalid network param!!\n",__FUNCTION__, __LINE__);
            nResult =  QMAPI_FAILURE;
            break;
        }

        if(pNetworkCfg->byEnableDHCP == 0)
        {
            ret = check_IP_Valid(pNetworkCfg->stEtherNet[0].strIPAddr.csIpV4);
            if(!ret)
            {
                printf("%s  %d, Invalid ethernet ip:%s\n",__FUNCTION__, __LINE__, pNetworkCfg->stEtherNet[0].strIPAddr.csIpV4);
                nResult =  QMAPI_FAILURE;
                break;
            }

            ret = IsSubnetMask(pNetworkCfg->stEtherNet[0].strIPMask.csIpV4);
            if(!ret)
            {
                printf("%s  %d, Invalid ethernet netmask:%s\n",__FUNCTION__, __LINE__, pNetworkCfg->stEtherNet[0].strIPMask.csIpV4);
                nResult =  QMAPI_FAILURE;
                break;
            }

            check_gateway(pNetworkCfg->stEtherNet[0].strIPAddr.csIpV4, 
                pNetworkCfg->stEtherNet[0].strIPMask.csIpV4, 
                pNetworkCfg->stGatewayIpAddr.csIpV4);
        }
        

        if(pNetworkCfg->byEnableDHCP == 0 && strcmp(pNetworkCfg->stEtherNet[0].strIPAddr.csIpV4, "0.0.0.0") == 0)
        {
            printf("%s  %d, Invalid ethernet ip:0.0.0.0\n",__FUNCTION__, __LINE__);
            nResult =  QMAPI_FAILURE;
            break;
        }

		if (pNetworkCfg->stEtherNet[0].wDataPort < 1024)
		{
			err();
			nResult =  QMAPI_FAILURE;
            break;
		}
        
        if(memcmp(&g_tl_globel_param.stNetworkCfg , pNetworkCfg , sizeof(QMAPI_NET_NETWORK_CFG)))
        {
            QMAPI_SetNetWork(pNetworkCfg, SetNetWorkConfig);
            memcpy(&g_tl_globel_param.stNetworkCfg, pNetworkCfg, sizeof(QMAPI_NET_NETWORK_CFG));
            tl_write_Server_Network();
        }
        
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_NETCFG:
    {
        g_tl_globel_param.stNetworkCfg.dwSize = sizeof(g_tl_globel_param.stNetworkCfg);
        strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4,NVSDEFAULTIP);
        strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4,NVSDEFAULTNETMASK);
        strcpy(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4,NVSDEFAULTGATEWAY);
        g_tl_globel_param.stNetworkCfg.byEnableDHCP = 1;
        g_tl_globel_param.stNetworkCfg.wHttpPort = SER_DEFAULT_WEBPORT;
        g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort = SER_DEFAULT_PORT;
        strcpy(g_tl_globel_param.stNetworkCfg.stDnsServer1IpAddr.csIpV4,NETWORK_DEFUALT_DNS);
        sprintf(g_tl_globel_param.stDevicInfo.csDeviceName, "%s", DEVICE_SERVER_NAME);
        g_tl_globel_param.stDevicInfo.byVideoStandard = videoStandardPAL;
        break;
    }

    case QMAPI_SYSCFG_GET_PLATFORMCFG:       //获取中心管理平台参数
    {
//          printf("QMAPI_SYSCFG_GET_PLATFORMCFG csIpV4:%s\n", g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4);
        memcpy(Param, &g_tl_globel_param.stNetPlatCfg, sizeof(QMAPI_NET_PLATFORM_INFO_V2));            
        break;
    }

    case QMAPI_SYSCFG_SET_PLATFORMCFG:       //设置中心管理平台参数      
    {
        if(!Param)
            return QMAPI_FAILURE;
        QMapi_sys_set_netplatform_config(Param);
        break;
    }

    case QMAPI_SYSCFG_GET_PPPOECFG:          //获取PPPOE参数
    {
        break;
    }

    case QMAPI_SYSCFG_SET_PPPOECFG:          //设置PPPOE参数
    {
        break;
    }

    case QMAPI_SYSCFG_GET_DDNSCFG:           //获取DDNS默认参数
    {
        memcpy(Param, &g_tl_globel_param.stDDNSCfg, sizeof(QMAPI_NET_DDNSCFG));
        break;
    }

    case QMAPI_SYSCFG_SET_DDNSCFG:           //设置DDNS参数
    {
#ifndef TL_Q3_PLATFORM
        memcpy(&g_tl_globel_param.stDDNSCfg, Param, sizeof(QMAPI_NET_DDNSCFG));
        
        extern int p2p_client_getuid_by_easyeye(char *pservername , char *psuidstr);
        QMAPI_NET_DDNSCFG *stNetDDNSCfg = (QMAPI_NET_DDNSCFG *)Param;
        if(stNetDDNSCfg->bEnableDDNS && (DDNS_MYEYE == stNetDDNSCfg->stDDNS[0].enDDNSType || DDNS_ANSHIBAO == stNetDDNSCfg->stDDNS[0].enDDNSType))
        {
            //int rvalue = 0;
            char szServer[128] = {0};
            char uidstr[64] = {0};
            sprintf(szServer, "%s.%s", stNetDDNSCfg->stDDNS[0].csDDNSDomain, stNetDDNSCfg->stDDNS[0].csDDNSAddress);
            //rvalue = p2p_client_getuid_by_easyeye(szServer, uidstr);  
            p2p_client_getuid_by_easyeye(szServer, uidstr);
        }        
#endif
        break;
    }

    case QMAPI_SYSCFG_GET_EMAILCFG:          //获取EMAIL参数
    {
        memcpy(Param, &g_tl_globel_param.TL_Email_Param, sizeof(QMAPI_NET_EMAIL_PARAM));
        break;
    }

    case QMAPI_SYSCFG_SET_EMAILCFG:          //设置EMAIL参数
    {
        QMAPI_NET_EMAIL_PARAM *email_param = Param;
        memcpy(&g_tl_globel_param.TL_Email_Param, email_param, sizeof(QMAPI_NET_EMAIL_PARAM));

        break;
    }
/*
    case QMAPI_NET_GET_FTPRECORDCFG://获取FTP录像参数
    {
        QMAPI_NET_RECORD_UPLOAD stRecordUpload;

        memset(&stRecordUpload, 0, sizeof(stRecordUpload));
        stRecordUpload.dwSize = sizeof(stRecordUpload);
        stRecordUpload.bEanble = 1;
        memset(&stRecordUpload.stScheduleTime,1,sizeof(stRecordUpload.stScheduleTime));

        memcpy(Param,&stRecordUpload,sizeof(stRecordUpload));
        break;
    }
    case QMAPI_NET_SET_FTPRECORDCFG://设置FTP录像参数
    {
        break;
    }
*/
    case QMAPI_SYSCFG_GET_FTPCFG:            //获取FTP参数
    {
        QMAPI_NET_FTP_PARAM ftpcfg;

        memset(&ftpcfg, 0, sizeof(QMAPI_NET_FTP_PARAM));
        ftpcfg.dwSize = sizeof(QMAPI_NET_FTP_PARAM);
        ftpcfg.bEnableFTP = g_tl_globel_param.TL_FtpUpdata_Param.dwEnableFTP;
        strncpy(ftpcfg.csFTPIpAddress, g_tl_globel_param.TL_FtpUpdata_Param.csFTPIpAddress, 32);
        ftpcfg.dwFTPPort = g_tl_globel_param.TL_FtpUpdata_Param.dwFTPPort;
        strncpy(ftpcfg.csUserName, (char*)g_tl_globel_param.TL_FtpUpdata_Param.sUserName, QMAPI_NAME_LEN);
        strncpy(ftpcfg.csPassword, (char*)g_tl_globel_param.TL_FtpUpdata_Param.sPassword, QMAPI_PASSWD_LEN);
        ftpcfg.wTopDirMode = g_tl_globel_param.TL_FtpUpdata_Param.wTopDirMode;
        ftpcfg.wSubDirMode = g_tl_globel_param.TL_FtpUpdata_Param.wSubDirMode;

        memcpy(Param, &ftpcfg, sizeof(QMAPI_NET_FTP_PARAM));
        break;
    }

    case QMAPI_SYSCFG_SET_FTPCFG:            //设置FTP参数
    {
        QMAPI_NET_FTP_PARAM *ftpcfg = Param;

        g_tl_globel_param.TL_FtpUpdata_Param.dwEnableFTP = ftpcfg->bEnableFTP;
        strncpy(g_tl_globel_param.TL_FtpUpdata_Param.csFTPIpAddress, ftpcfg->csFTPIpAddress, 32);
        g_tl_globel_param.TL_FtpUpdata_Param.dwFTPPort = ftpcfg->dwFTPPort;
        strncpy((char*)g_tl_globel_param.TL_FtpUpdata_Param.sUserName, ftpcfg->csUserName, QMAPI_NAME_LEN);
        strncpy((char*)g_tl_globel_param.TL_FtpUpdata_Param.sPassword, ftpcfg->csPassword, QMAPI_PASSWD_LEN);
        g_tl_globel_param.TL_FtpUpdata_Param.wTopDirMode = ftpcfg->wTopDirMode;
        g_tl_globel_param.TL_FtpUpdata_Param.wSubDirMode = ftpcfg->wSubDirMode;

        break;
    }

    case QMAPI_SYSCFG_GET_WIFICFG:           //获取WIFI参数
    {
        //printf("#### %s %d, bWifiEnable=%d\n", __FUNCTION__, __LINE__, g_tl_globel_param.stWifiConfig.bWifiEnable);
        QMAPI_Wireless_IOCtrl(Handle, QMAPI_SYSCFG_SET_WIFICFG, nChannel, &g_tl_globel_param.stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));
        QMAPI_Wireless_IOCtrl(Handle, QMAPI_SYSCFG_GET_WIFICFG, nChannel, Param, nSize);

        break;
    }
    case QMAPI_SYSCFG_SET_WIFICFG:           //设置WIFI参数
    {
        memcpy(&g_tl_globel_param.stWifiConfig, Param, sizeof(QMAPI_NET_WIFI_CONFIG));
        QMAPI_Wireless_IOCtrl(Handle, QMAPI_SYSCFG_SET_WIFICFG, nChannel, Param, nSize);
        break;
    }
    case QMAPI_SYSCFG_GET_WIFI_SITE_LIST:   //获取WIFI 站点列表
    {
        QMAPI_NET_WIFI_SITE_LIST stWifiSiteList;
        QMAPI_Wireless_GetSiteList(&stWifiSiteList);
        memcpy(Param, &stWifiSiteList, sizeof(QMAPI_NET_WIFI_SITE_LIST));
        break;
    }

    case QMAPI_SYSCFG_GET_WANWIRELESSCFG:    //获取WANWIRELESS参数
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_WANWIRELESS_CONFIG wanwireless_config;

        memset(&wanwireless_config, 0, sizeof(QMAPI_NET_WANWIRELESS_CONFIG));
        wanwireless_config.dwSize = g_tl_globel_param.tl_Tdcdma_config.dwSize;
        wanwireless_config.bEnable = g_tl_globel_param.tl_Tdcdma_config.bCdmaEnable;
        wanwireless_config.byDevType = g_tl_globel_param.tl_Tdcdma_config.byDevType;
        wanwireless_config.byStatus = g_tl_globel_param.tl_Tdcdma_config.byStatus;
        wanwireless_config.dwNetIpAddr = g_tl_globel_param.tl_Tdcdma_config.dwNetIpAddr;

        memcpy(Param, &wanwireless_config, sizeof(QMAPI_NET_WANWIRELESS_CONFIG));
#endif
        break;
    }

    case QMAPI_SYSCFG_SET_WANWIRELESSCFG:    //设置WANWIRELESS参数
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_WANWIRELESS_CONFIG *wanwireless_config = Param;

        g_tl_globel_param.tl_Tdcdma_config.dwSize = wanwireless_config->dwSize;
        g_tl_globel_param.tl_Tdcdma_config.bCdmaEnable = wanwireless_config->bEnable;
        g_tl_globel_param.tl_Tdcdma_config.byDevType = wanwireless_config->byDevType;
        g_tl_globel_param.tl_Tdcdma_config.byStatus = wanwireless_config->byStatus;
        g_tl_globel_param.tl_Tdcdma_config.dwNetIpAddr = wanwireless_config->dwNetIpAddr;
#endif
        break;
    }

    case QMAPI_SYSCFG_GET_UPNPCFG:
    {
        QMAPI_UPNP_CONFIG upnp_config;        /*QMAPI_UPNP_CONFIG 和 TL_UPNP_CONFIG 是一样的，
                                            仅仅是名称不一样,所以在此就不一一赋值，而是直接拷贝结构体*/

        memset(&upnp_config, 0, sizeof(QMAPI_UPNP_CONFIG));
        memcpy(&upnp_config, &g_tl_globel_param.tl_upnp_config, sizeof(TL_UPNP_CONFIG));
        memcpy(Param, &upnp_config, sizeof(QMAPI_UPNP_CONFIG));
        break;
    }

    case QMAPI_SYSCFG_SET_UPNPCFG:
    {
        QMAPI_UPNP_CONFIG *upnp_config = Param;

        memcpy(&g_tl_globel_param.tl_upnp_config, upnp_config, sizeof(QMAPI_UPNP_CONFIG));
        break;
    }
#if 0
    case QMAPI_GET_MOBILE_CENTER_INFO:
    {
        QMAPI_MOBILE_CENTER_INFO mobile_center_info;
        /*QMAPI_MOBILE_CENTER_INFO 和 TL_MOBILE_CENTER_INFO 是一样的，
        仅仅是名称不一样,所以在此就不一一赋值，而是直接拷贝结构体*/

        memset(&mobile_center_info, 0, sizeof(QMAPI_MOBILE_CENTER_INFO));
        memcpy(&mobile_center_info, &g_tl_globel_param.tl_mobile_config, sizeof(TL_MOBILE_CENTER_INFO));
        memcpy(Param, &mobile_center_info, sizeof(QMAPI_MOBILE_CENTER_INFO));         
        break;
    }

    case QMAPI_SET_MOBILE_CENTER_INFO:
    {
        QMAPI_MOBILE_CENTER_INFO *mobile_center_info = Param;
        
        memcpy(&g_tl_globel_param.tl_mobile_config, mobile_center_info, sizeof(QMAPI_MOBILE_CENTER_INFO));
        break;
    }
#endif
    case QMAPI_SYSCFG_GET_PICCFG:            //获取图象压缩参数
    {
		//printf("######### size %d sizeof %d \n" , nSize , sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        memcpy(Param, &g_tl_globel_param.stChannelPicParam[nChannel], sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        break;
    }

    case QMAPI_SYSCFG_GET_SUPPORT_STREAM_FMT://获取系统支持的图像能力
    {
        QMAPI_NET_SUPPORT_STREAM_FMT stSupportFmt;

        memset(&stSupportFmt, 0, sizeof(QMAPI_NET_SUPPORT_STREAM_FMT));
        GetNetSupportStreamFmt(nChannel, &stSupportFmt);

        memcpy(Param, &stSupportFmt, sizeof(QMAPI_NET_SUPPORT_STREAM_FMT));
        break;
    }

    case QMAPI_SYSCFG_SET_PICCFG:            //设置图象压缩参数
    {
		LPQMAPI_NET_CHANNEL_PIC_INFO lpPICCfg = (LPQMAPI_NET_CHANNEL_PIC_INFO)Param;
		if(lpPICCfg->dwSize != sizeof(QMAPI_NET_CHANNEL_PIC_INFO)
			|| lpPICCfg->dwChannel < 0
			|| lpPICCfg->dwChannel > QMAPI_MAX_CHANNUM)
		{
			err();
			ret = QMAPI_ERR_INVALID_PARAM;
		}
		else
		{
			ret = QMAPI_AVServer_IOCtrl(Handle, nCmd, nChannel, Param, nSize);
			if (ret == QMAPI_SUCCESS)
			{
				memcpy(&g_tl_globel_param.stChannelPicParam[nChannel], Param, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
				tl_write_ChannelPic_param();
			}
		}
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_PICCFG:
    {
        QMAPI_NET_CHANNEL_PIC_INFO stChnPicInfo;
        TL_Default_Channel_Encode_Param(&stChnPicInfo, nChannel);

        memcpy(Param, &stChnPicInfo, sizeof(stChnPicInfo));
        break;
    }

    case QMAPI_SYSCFG_GET_OSDCFG:            //获取图象字符叠加参数
    {
        QMAPI_NET_CHANNEL_OSDINFO *channel_osdinfo = Param;

        memset(channel_osdinfo, 0, sizeof(QMAPI_NET_CHANNEL_OSDINFO));
        channel_osdinfo->dwSize = sizeof(QMAPI_NET_CHANNEL_OSDINFO);
        channel_osdinfo->dwChannel = nChannel;
        channel_osdinfo->bShowTime = g_tl_globel_param.TL_Channel_Osd_Info[nChannel].bShowTime;
        channel_osdinfo->bDispWeek = 0; //不显示星期，ipc无星期显示
        channel_osdinfo->dwTimeTopLeftX = g_tl_globel_param.stTimeOsdPos[nChannel].dwTimeX;
        channel_osdinfo->dwTimeTopLeftY = g_tl_globel_param.stTimeOsdPos[nChannel].dwTimeY;
        channel_osdinfo->byReserve1 = g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwTimeFormat;
                                        //OSD类型(年月日格式)：
        //channel_osdinfo->byOSDAttrib;   //OSD透明属性
        //channel_osdinfo->byReserve2;        //
        channel_osdinfo->byShowChanName = g_tl_globel_param.TL_Channel_Osd_Info[nChannel].bShowString;
        channel_osdinfo->dwShowNameTopLeftX = g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwStringx;
        channel_osdinfo->dwShowNameTopLeftY = g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwStringy;
        memcpy(channel_osdinfo->csChannelName, g_tl_globel_param.TL_Channel_Osd_Info[nChannel].csString, QMAPI_NAME_LEN);
        //channel_osdinfo->stOsd[0].bOsdEnable;   //

        printf("channel_osdinfo.byShowChanName = %d\n",g_tl_globel_param.TL_Channel_Osd_Info[nChannel].bShowString);
        printf("channel_osdinfo.dwShowNameTopLeftX = %ld\n",channel_osdinfo->dwShowNameTopLeftX);
        printf("channel_osdinfo.dwShowNameTopLeftY = %ld\n",channel_osdinfo->dwShowNameTopLeftY);
        printf("channel_osdinfo.csChannelName = %s\n",channel_osdinfo->csChannelName);
        
        break;
    }

    case QMAPI_SYSCFG_SET_OSDCFG:            //设置图象字符叠加参数
    {
        QMAPI_NET_CHANNEL_OSDINFO *channel_osdinfo = Param;
        if(NULL == channel_osdinfo || channel_osdinfo->dwSize != sizeof(QMAPI_NET_CHANNEL_OSDINFO))
            return QMAPI_FAILURE;

        printf("%s %d\n", __FUNCTION__, nCmd);
         int ret = QMAPI_Video_Marker_IOCtrl(Handle, nCmd, nChannel, Param, nSize);
             if (ret == QMAPI_SUCCESS) {
        g_tl_globel_param.TL_Channel_Osd_Info[nChannel].bShowTime = channel_osdinfo->bShowTime;
        g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwTimeFormat = channel_osdinfo->byReserve1;
        g_tl_globel_param.TL_Channel_Osd_Info[nChannel].bShowString = channel_osdinfo->byShowChanName;
        g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwStringx = channel_osdinfo->dwShowNameTopLeftX;
        g_tl_globel_param.TL_Channel_Osd_Info[nChannel].dwStringy = channel_osdinfo->dwShowNameTopLeftY;
        g_tl_globel_param.stTimeOsdPos[nChannel].dwTimeX = channel_osdinfo->dwTimeTopLeftX;
        g_tl_globel_param.stTimeOsdPos[nChannel].dwTimeY = channel_osdinfo->dwTimeTopLeftY;
        memcpy(g_tl_globel_param.TL_Channel_Osd_Info[nChannel].csString, channel_osdinfo->csChannelName, QMAPI_NAME_LEN);
        }

	printf("%s cmd:%d ret:%d \n", __FUNCTION__, nCmd, ret);
        break;
    }

    case QMAPI_SYSCFG_GET_COLORCFG:          //获取图象色彩参数
    {
        memcpy(Param, &g_tl_globel_param.TL_Channel_Color[nChannel], sizeof(QMAPI_NET_CHANNEL_COLOR));
        
        Camera_Get_Default_Color((QMAPI_NET_CHANNEL_COLOR*)Param);
        break;
    }

    case QMAPI_SYSCFG_SET_COLORCFG:          //设置图象色彩参数
    {
        QMAPI_NET_CHANNEL_COLOR_SINGLE pColorSingle = {0};
        QMAPI_NET_CHANNEL_COLOR *pColorSet   = (QMAPI_NET_CHANNEL_COLOR *)Param;
        QMAPI_NET_CHANNEL_COLOR *pColorParam = &g_tl_globel_param.TL_Channel_Color[pColorSet->dwChannel];
        QMAPI_NET_CHANNEL_ENHANCED_COLOR *lpEnancedColor = &g_tl_globel_param.TL_Channel_EnhancedColor[pColorSet->dwChannel];

        pColorSingle.dwChannel = pColorSet->dwChannel;
        pColorSingle.dwSize    = sizeof(QMAPI_NET_CHANNEL_COLOR_SINGLE);
        if (pColorSet->nBrightness != pColorParam->nBrightness)/* */
        {
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_BRIGHTNESS;
            pColorSingle.nValue    = pColorSet->nBrightness;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);
        }
        if (pColorSet->nHue != pColorParam->nHue)/* */
        {
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_HUE;
            pColorSingle.nValue    = pColorSet->nHue;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);
        }
        if (pColorSet->nContrast != pColorParam->nContrast) /* */
        {
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_CONTRAST;
            pColorSingle.nValue    = pColorSet->nContrast;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);
        }
        if (pColorSet->nDefinition != pColorParam->nDefinition) /* */
        {
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_DEFINITION;
            pColorSingle.nValue    = pColorSet->nDefinition;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);
        }
        if (pColorSet->nSaturation != pColorParam->nSaturation)/* */
        {
            pColorSingle.dwSetFlag = QMAPI_COLOR_SET_SATURATION;
            pColorSingle.nValue    = pColorSet->nSaturation;
            Camera_Set_Video_Color(&pColorSingle, pColorParam, lpEnancedColor);
        }
        switch(pColorSet->dwSetFlag)
        {
            case QMAPI_COLOR_SAVE:
                tl_write_Channel_Color();
                tl_write_Channel_Enhanced_Color();
                break;
            case QMAPI_COLOR_SET_DEF:
                tl_init_Channel_Color(pColorSet->dwChannel);
                break;
            default:
                break;
        }
        break;
    }

    case QMAPI_SYSCFG_GET_ENHANCED_COLOR:  //设置增强图象色彩参数
    {   
        memcpy(Param,&g_tl_globel_param.TL_Channel_EnhancedColor[nChannel],sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR));
        break;
    }
    
    case QMAPI_SYSCFG_GET_COLOR_SUPPORT:     //获取图象色彩参数
    {
        QMAPI_NET_COLOR_SUPPORT *pColorSupport = (QMAPI_NET_COLOR_SUPPORT *)Param;
        if(!pColorSupport || nSize != sizeof(QMAPI_NET_COLOR_SUPPORT))
            return QMAPI_FAILURE;
        Camera_Get_Color_Support((QMAPI_NET_COLOR_SUPPORT*)Param);
        break;
    }

    case QMAPI_SYSCFG_SET_COLORCFG_SINGLE:   //单独设置某个图象色彩参数
    {
        QMAPI_NET_CHANNEL_COLOR_SINGLE *lpClrSingleCfg = (QMAPI_NET_CHANNEL_COLOR_SINGLE *)Param;
		QMAPI_NET_CHANNEL_COLOR    *lpColorParam = &g_tl_globel_param.TL_Channel_Color[lpClrSingleCfg->dwChannel];
		QMAPI_NET_CHANNEL_ENHANCED_COLOR *lpEnancedColor = &g_tl_globel_param.TL_Channel_EnhancedColor[lpClrSingleCfg->dwChannel];
        if(!lpClrSingleCfg || lpClrSingleCfg->dwSize != sizeof(QMAPI_NET_CHANNEL_COLOR_SINGLE))
            return QMAPI_FAILURE;
        Camera_Set_Video_Color(lpClrSingleCfg, lpColorParam, lpEnancedColor);
		switch(lpClrSingleCfg->dwSaveFlag)
		{
			case QMAPI_COLOR_SAVE:
				tl_write_Channel_Color();
				tl_write_Channel_Enhanced_Color();
				break;
			case QMAPI_COLOR_SET_DEF:
				tl_init_Channel_Color(lpClrSingleCfg->dwChannel);
				break;
			default:
				break;
		}
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_COLORCFG:
    {
        Camera_Get_Default_Color((QMAPI_NET_CHANNEL_COLOR*)Param);
        break;
    }

    case QMAPI_SYSCFG_GET_SHELTERCFG:        //获取图象遮挡参数
    {
        unsigned char i;
        QMAPI_NET_CHANNEL_SHELTER * shelter = NULL;
        shelter = (QMAPI_NET_CHANNEL_SHELTER * )Param;
        shelter->bEnable     = g_tl_globel_param.TL_Channel_Shelter[nChannel].bEnable;
        shelter->dwSize      = sizeof(QMAPI_NET_CHANNEL_SHELTER);
        shelter->dwChannel   = g_tl_globel_param.TL_Channel_Shelter[nChannel].dwChannel;
        for(i = 0 ; i <QMAPI_MAX_VIDEO_HIDE_RECT; i++ )
        {
            shelter->strcShelter[i].nType    = 5;
            shelter->strcShelter[i].dwColor  = 0 ;
            shelter->strcShelter[i].wLeft    = g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].left;
            shelter->strcShelter[i].wTop     = g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].top;
            shelter->strcShelter[i].wWidth   = g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].right - g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].left;
            shelter->strcShelter[i].wHeight  = g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].bottom - g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].top;
        }
        break;
    }

    case QMAPI_SYSCFG_SET_SHELTERCFG:        //设置图象遮挡参数
    {
        printf("QMAPI_SYSCFG_SET_SHELTERCFG\n");
        {
            QMAPI_NET_CHANNEL_SHELTER *shelter = Param;
            if(NULL == shelter || shelter->dwSize != sizeof(QMAPI_NET_CHANNEL_SHELTER))
                return QMAPI_FAILURE;

            printf("%s %d\n", __FUNCTION__, nCmd);
            int ret = QMAPI_Video_Marker_IOCtrl(Handle, nCmd, nChannel, Param, nSize);
            if (ret == QMAPI_SUCCESS) 
            {
                g_tl_globel_param.TL_Channel_Shelter[nChannel].bEnable      = shelter->bEnable;
                g_tl_globel_param.TL_Channel_Shelter[nChannel].dwSize       = shelter->dwSize;
                g_tl_globel_param.TL_Channel_Shelter[nChannel].dwChannel    = shelter->dwChannel;
                int i = 0;
                for (i = 0 ; i <QMAPI_MAX_VIDEO_HIDE_RECT; i++ )
                {
                    g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].left    = shelter->strcShelter[i].wLeft;
                    g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].top     = shelter->strcShelter[i].wTop;
                    g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].right   = shelter->strcShelter[i].wLeft + shelter->strcShelter[i].wWidth;
                    g_tl_globel_param.TL_Channel_Shelter[nChannel].rcShelter[i].bottom  = shelter->strcShelter[i].wTop  + shelter->strcShelter[i].wHeight;
                }
            }

            printf("%s cmd:%d ret:%d \n", __FUNCTION__, nCmd, ret);
        }

        break;
    }

    case QMAPI_SYSCFG_GET_MOTIONCFG:         //获取图象移动侦测参数
    {
        QMAPI_NET_CHANNEL_MOTION_DETECT channel_motion_detect;
        int day, i;

        memset(&channel_motion_detect, 0, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
        channel_motion_detect.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
        channel_motion_detect.dwChannel = nChannel;
        channel_motion_detect.bEnable = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bEnable;
        channel_motion_detect.dwSensitive = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].dwSensitive;
        //channel_motion_detect.bManualDefence;   //手动布防标志，==YES(1)启动，==NO(0)按定时判断 

        for(i = 0;i<44*36;i++)
        {
            if(g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].pbMotionArea[i])
            {
                SET_BIT(channel_motion_detect.byMotionArea,i);
            }
            else
            {
                CLR_BIT(channel_motion_detect.byMotionArea,i);
            }
        }

        //channel_motion_detect.stHandle.wActionMask;         /* 当前报警所支持的处理方式，按位掩码表示 */
        //channel_motion_detect.stHandle.wActionFlag;         /* 触发动作，按位掩码表示，具体动作所需要的参数在各自的配置中体现 */

        /* 报警触发的输出通道，报警触发的输出，为1表示触发该输出 */ 
        channel_motion_detect.stHandle.wDuration = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].nDuration;
        if(g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bShotSnap)
        {
            SET_BIT(&channel_motion_detect.stHandle.bySnap[nChannel%8], nChannel);
            channel_motion_detect.stHandle.wSnapNum = 1;
            channel_motion_detect.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOSNAP;
        }

        //alarm out
        for(i = 0; i < 4; i++)
        {
            if(g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bAlarmIOOut[i])
            {
                SET_BIT(&channel_motion_detect.stHandle.byRelAlarmOut[0], i);
                channel_motion_detect.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOALARMOUT;
            }
        }

        for(day = 0; day < 8; day++)
        {
            //  if(!g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].bEnable)
            //     continue;

            //  channel_motion_detect.bEnable = 1;
            channel_motion_detect.stScheduleTime[day][0].bEnable = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].bEnable;
            channel_motion_detect.stScheduleTime[day][0].byStartHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].BeginTime1.bHour;
            channel_motion_detect.stScheduleTime[day][0].byStartMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].BeginTime1.bMinute;
            channel_motion_detect.stScheduleTime[day][0].byStopHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].EndTime1.bHour;
            channel_motion_detect.stScheduleTime[day][0].byStopMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].EndTime1.bMinute;

            channel_motion_detect.stScheduleTime[day][1].bEnable = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].bEnable;
            channel_motion_detect.stScheduleTime[day][1].byStartHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].BeginTime2.bHour;
            channel_motion_detect.stScheduleTime[day][1].byStartMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].BeginTime2.bMinute;
            channel_motion_detect.stScheduleTime[day][1].byStopHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].EndTime2.bHour;
            channel_motion_detect.stScheduleTime[day][1].byStopMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day].EndTime2.bMinute;
        }
#if 0
        if(g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].bEnable)
        {
            channel_motion_detect.bEnable = 1;
            for(day = 0; day < 7; day++)
            {
                channel_motion_detect.stScheduleTime[day][0].byStartHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].BeginTime1.bHour;
                channel_motion_detect.stScheduleTime[day][0].byStartMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].BeginTime1.bMinute;
                channel_motion_detect.stScheduleTime[day][0].byStopHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].EndTime1.bHour;
                channel_motion_detect.stScheduleTime[day][0].byStopMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].EndTime1.bMinute;

                channel_motion_detect.stScheduleTime[day][1].byStartHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].BeginTime2.bHour;
                channel_motion_detect.stScheduleTime[day][1].byStartMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].BeginTime2.bMinute;
                channel_motion_detect.stScheduleTime[day][1].byStopHour = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].EndTime2.bHour;
                channel_motion_detect.stScheduleTime[day][1].byStopMin = g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[7].EndTime2.bMinute;
            }
        }
#endif

        memcpy(Param, &channel_motion_detect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
        break;
    }

    case QMAPI_SYSCFG_SET_MOTIONCFG:         //设置图象移动侦测参数
    {
        QMAPI_NET_CHANNEL_MOTION_DETECT *channel_motion_detect = Param;
        printf("Set QMAPI_SYSCFG_SET_MOTIONCFG: enable=%d\n", channel_motion_detect->bEnable);
        int day, i;
        int sensitive = channel_motion_detect->dwSensitive;

        if (sensitive<0 || sensitive>100)
        {
            printf("Set motion sensitive(%d) out of range\n", sensitive);
            return QMAPI_ERR_INVALID_PARAM;
        }

        if (sensitive < 35)
        {
            sensitive = va_move_set_sensitivity("vam", 2);
        }
        else if (sensitive < 70)
        {
            sensitive = va_move_set_sensitivity("vam", 1);
        }
        else
        {
            sensitive = va_move_set_sensitivity("vam", 0);
        }
        if (sensitive<0)
        {
            printf("Set motion sensitive failed, ret=%d\n", sensitive);
            return QMAPI_FAILURE;
        }

        g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bEnable = channel_motion_detect->bEnable;
        g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].dwSensitive = channel_motion_detect->dwSensitive;
        g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].nDuration = channel_motion_detect->stHandle.wDuration;
        g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bShotSnap = channel_motion_detect->stHandle.bySnap[nChannel];
        //ALARM OUT
        if(channel_motion_detect->stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOALARMOUT)
        {
            for(i = 0; i < 4; i++)
            {
                g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bAlarmIOOut[i] = CHK_BIT(&channel_motion_detect->stHandle.byRelAlarmOut[i/8], i);
            }                           
        }
        else
        {
            for(i = 0; i < 4; i++)
            {
                g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bAlarmIOOut[i] = 0;
            }
        }

        // Snap 
        if(channel_motion_detect->stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOSNAP)
        {
            g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bShotSnap = 1;             
        }
        else
        {
            g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].bShotSnap = 0; 
        }

        for(i = 0;i<44*36;i++)
        {
            if(CHK_BIT(channel_motion_detect->byMotionArea,i))
            {
                g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].pbMotionArea[i] = 1;
            }
            else
            {
                g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].pbMotionArea[i] = 0;
            }
        }
/*
        if(channel_motion_detect->bEnable == 0)
        {
            for(day = 0; day < 7; day++)
            {
                TL_SCHED_TIME   *pstScheduleTime = &g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day];
                pstScheduleTime->bEnable = 0;
            }
        }
        else
*/
        {
            for(day = 0; day < 8; day++)
            {
                TL_SCHED_TIME   *pstScheduleTime = &g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime[day];

                pstScheduleTime->bEnable = channel_motion_detect->stScheduleTime[day][0].bEnable;

                pstScheduleTime->BeginTime1.bHour = channel_motion_detect->stScheduleTime[day][0].byStartHour;
                pstScheduleTime->BeginTime1.bMinute = channel_motion_detect->stScheduleTime[day][0].byStartMin;
                pstScheduleTime->EndTime1.bHour = channel_motion_detect->stScheduleTime[day][0].byStopHour;
                pstScheduleTime->EndTime1.bMinute = channel_motion_detect->stScheduleTime[day][0].byStopMin;

                pstScheduleTime->BeginTime2.bHour = channel_motion_detect->stScheduleTime[day][1].byStartHour;
                pstScheduleTime->BeginTime2.bMinute = channel_motion_detect->stScheduleTime[day][1].byStartMin;
                pstScheduleTime->EndTime2.bHour = channel_motion_detect->stScheduleTime[day][1].byStopHour;
                pstScheduleTime->EndTime2.bMinute = channel_motion_detect->stScheduleTime[day][1].byStopMin;
#if 0
                if((pstScheduleTime->BeginTime1.bHour*60+pstScheduleTime->BeginTime1.bMinute 
                    < pstScheduleTime->EndTime1.bHour*60 + pstScheduleTime->EndTime1.bMinute) ||
                    (pstScheduleTime->BeginTime2.bHour*60+pstScheduleTime->BeginTime2.bMinute 
                    < pstScheduleTime->EndTime2.bHour*60 + pstScheduleTime->EndTime2.bMinute))
                {
                    pstScheduleTime->bEnable = 1;
                }
#endif

                printf("day = %d, %d:%d-%d:%d %d:%d-%d:%d\n",
                    day, 
                    pstScheduleTime->BeginTime1.bHour, pstScheduleTime->BeginTime1.bMinute,
                    pstScheduleTime->EndTime1.bHour, pstScheduleTime->EndTime1.bMinute,
                    pstScheduleTime->BeginTime2.bHour, pstScheduleTime->BeginTime2.bMinute,
                    pstScheduleTime->EndTime2.bHour, pstScheduleTime->EndTime2.bMinute);
            }
        }   
#ifdef JIUAN_SUPPORT
		QMAPI_CONSUMER_PRIVACY_INFO *ConsumerInfo = &g_tl_globel_param.stConsumerInfo.stPrivacyInfo[g_tl_globel_param.stConsumerInfo.bPrivacyMode];
		ConsumerInfo->stMotionInfo.bEnable = channel_motion_detect->bEnable;
		ConsumerInfo->stMotionInfo.dwSensitive = channel_motion_detect->dwSensitive;
		memcpy(ConsumerInfo->stMotionInfo.stSchedTime, g_tl_globel_param.TL_Channel_Motion_Detect[nChannel].tlScheduleTime, sizeof(ConsumerInfo->stMotionInfo.stSchedTime));
#endif
        break;
    }

    case QMAPI_SYSCFG_DEF_MOTIONCFG:
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_VLOSTCFG:          //获取图象视频丢失参数
    {
         break;
    }
    case QMAPI_SYSCFG_SET_VLOSTCFG:          //设置图象视频丢失参数
    {
        break;
    }

    case QMAPI_SYSCFG_GET_HIDEALARMCFG:      //获取图象遮挡报警参数
    {
        return -1;                      //ipc暂不支持遮挡报警
        break;
    }

    case QMAPI_SYSCFG_SET_HIDEALARMCFG:      //设置图象遮挡报警参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_DEF_HIDEALARMCFG:
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_RECORDCFG:         //获取录像参数
    {
        QMAPI_NET_CHANNEL_RECORD *pchn_record = (QMAPI_NET_CHANNEL_RECORD*)Param;

        memcpy(pchn_record, &g_tl_globel_param.stChannelRecord[nChannel], sizeof(QMAPI_NET_CHANNEL_RECORD));
        break;
    }

    case QMAPI_SYSCFG_SET_RECORDCFG:         //设置图象录像参数
    {
        QMAPI_NET_CHANNEL_RECORD *pchn_record = (QMAPI_NET_CHANNEL_RECORD*)Param;
		
        saveRecordParam();//tl_write_all_parameter();

        memcpy(&g_tl_globel_param.stChannelRecord[nChannel], pchn_record, sizeof(QMAPI_NET_CHANNEL_RECORD));
        saveChannelRecordParam();
		
#ifdef JIUAN_SUPPORT
        QMAPI_CONSUMER_PRIVACY_INFO *ConsumerInfo = &g_tl_globel_param.stConsumerInfo.stPrivacyInfo[g_tl_globel_param.stConsumerInfo.bPrivacyMode];
        ConsumerInfo->stRecordInfo.bEnable = pchn_record->dwRecord;
        memcpy(ConsumerInfo->stRecordInfo.stRecordSched, pchn_record->stRecordSched, sizeof(pchn_record->stRecordSched));/*同步更新当前模式下录像计划*/
#endif
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_RECORDCFG:
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_RECORDMODECFG:     //获取图象手动录像参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_SET_RECORDMODECFG:     //设置图象手动录像参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_DECODERCFG:        //获取解码器参数
    {
        QMAPI_NET_DECODERCFG *decodercfg =Param;
        int i;

        memset(decodercfg, 0, sizeof(QMAPI_NET_DECODERCFG));
        decodercfg->dwSize = sizeof(QMAPI_NET_DECODERCFG);
        decodercfg->dwChannel = nChannel;

        for(i = 0; i < 16; i++)
        {
            if(g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate == baudrate[i])
            {
                printf("g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate = %ld\n",g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate);
                decodercfg->dwBaudRate = i;
                break;
            }
        }
        
        decodercfg->byDataBit = g_tl_globel_param.TL_Server_Cominfo[nChannel].nDataBits -1;
        decodercfg->byStopBit = g_tl_globel_param.TL_Server_Cominfo[nChannel].nStopBits;
        decodercfg->byParity = g_tl_globel_param.TL_Server_Cominfo[nChannel].nParity;
        decodercfg->byFlowcontrol = g_tl_globel_param.TL_Server_Cominfo[nChannel].nStreamControl;
        memcpy(decodercfg->csDecoderType, g_tl_globel_param.TL_Server_Cominfo[nChannel].szPTZName, QMAPI_NAME_LEN);
        decodercfg->wDecoderAddress = g_tl_globel_param.TL_Server_Cominfo[nChannel].nAddress;
        decodercfg->byHSpeed = g_tl_globel_param.TL_Server_Cominfo[nChannel].nHSpeed;
        decodercfg->byVSpeed = g_tl_globel_param.TL_Server_Cominfo[nChannel].nVSpeed;
        strcpy(decodercfg->csDecoderType ,g_tl_globel_param.TL_Server_Cominfo[nChannel].szPTZName);

        //    memcpy(Param, &decodercfg, sizeof(QMAPI_NET_DECODERCFG));
        break;
    }

    case QMAPI_SYSCFG_SET_DECODERCFG:        //设置解码器参数
    {
        QMAPI_NET_DECODERCFG *decodercfg = Param;
        
        if(decodercfg->dwBaudRate < 0 || decodercfg->dwBaudRate > 10)
        {
            decodercfg->dwBaudRate = 10;
        }

        printf("decodercfg->dwBaudRate = %ld\n",decodercfg->dwBaudRate);
        g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate = baudrate[decodercfg->dwBaudRate];
        
        printf("g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate = %ld\n",g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate);
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nDataBits = decodercfg->byDataBit + 1;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nStopBits = decodercfg->byStopBit;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nParity = decodercfg->byParity;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nStreamControl = decodercfg->byFlowcontrol;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nAddress = decodercfg->wDecoderAddress;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nHSpeed = decodercfg->byHSpeed;
        g_tl_globel_param.TL_Server_Cominfo[nChannel].nVSpeed = decodercfg->byVSpeed;
        memcpy(g_tl_globel_param.TL_Server_Cominfo[nChannel].szPTZName, decodercfg->csDecoderType, QMAPI_NAME_LEN);
        printf("g_tl_globel_param.TL_Server_Cominfo[%d].eBaudRate = %ld, hspeed:%d, vspeed:%d\n",
                    nChannel, g_tl_globel_param.TL_Server_Cominfo[nChannel].dwBaudRate,
                    g_tl_globel_param.TL_Server_Cominfo[nChannel].nHSpeed,
                    g_tl_globel_param.TL_Server_Cominfo[nChannel].nVSpeed);
#ifndef TL_Q3_PLATFORM

        ptzmng_setdecoderinfo(nChannel, (int)decodercfg->wDecoderAddress, decodercfg->csDecoderType, 
            (unsigned char)decodercfg->byHSpeed, (unsigned char)decodercfg->byVSpeed);
#endif
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_DECODERCFG:
    {
        QMAPI_NET_DECODERCFG decodercfg;

        memset(&decodercfg, 0, sizeof(QMAPI_NET_DECODERCFG));
        decodercfg.dwSize = sizeof(QMAPI_NET_DECODERCFG);
        decodercfg.dwChannel = nChannel;
        decodercfg.dwBaudRate = DEFUALT_COMINFO_1_BAUD;
        decodercfg.byDataBit = DEFUALT_COMINFO_1_DATA_BITS;
        decodercfg.byStopBit = DEFUALT_COMINFO_1_STOP_BITS;
        decodercfg.byParity = 0;
        decodercfg.byFlowcontrol = 0;
        strcpy(decodercfg.csDecoderType, "pelco-d");
        decodercfg.wDecoderAddress = DEFUALT_PTZ_ADDRESS;
        decodercfg.byHSpeed = DEFUALT_PTZ_SPEED;
        decodercfg.byVSpeed = DEFUALT_PTZ_SPEED;

        memcpy(Param, &decodercfg, sizeof(QMAPI_NET_DECODERCFG));
        break;
    }

    case QMAPI_SYSCFG_GET_SNAPTIMERCFG:      //获取图像定时抓拍参数
    {
        memcpy(Param, &g_tl_globel_param.stSnapTimer, sizeof(g_tl_globel_param.stSnapTimer));
        break;
    }

    case QMAPI_SYSCFG_SET_SNAPTIMERCFG:      //设置图像定时抓拍参数
    {
        memcpy(&g_tl_globel_param.stSnapTimer, Param, sizeof(g_tl_globel_param.stSnapTimer));
        break;
    }

    case QMAPI_SYSCFG_GET_RS232CFG:          //获取232串口参数
    {
        QMAPI_NET_RS232CFG rs232cfg;
        int i;

        memset(&rs232cfg, 0, sizeof(QMAPI_NET_RS232CFG));

        for(i = 0; i < 11; i++)
        {
            if(g_tl_globel_param.TL_Server_Com2info[nChannel].eBaudRate == baudrate[i])
            {
                rs232cfg.dwBaudRate = i;
                break;
            }
        }
        if(15 == i)
            rs232cfg.dwBaudRate = 15;

        rs232cfg.dwSize = sizeof(QMAPI_NET_RS232CFG);
        rs232cfg.dwBaudRate = g_tl_globel_param.TL_Server_Com2info[nChannel].eBaudRate;
        rs232cfg.byDataBit = g_tl_globel_param.TL_Server_Com2info[nChannel].nDataBits;
        rs232cfg.byStopBit = g_tl_globel_param.TL_Server_Com2info[nChannel].nStopBits;
        rs232cfg.byParity = g_tl_globel_param.TL_Server_Com2info[nChannel].nParity;
        rs232cfg.byFlowcontrol = g_tl_globel_param.TL_Server_Com2info[nChannel].nStreamControl;
        //rs232cfg.dwWorkMode;    /* 工作模式，0－窄带传输（232串口用于PPP拨号），1－控制台（232串口用于参数控制），2－透明通道 */
        break;
    }

    case QMAPI_SYSCFG_SET_RS232CFG:          //设置232串口参数
    {
        QMAPI_NET_RS232CFG *rs232cfg = Param;

        if(rs232cfg->dwBaudRate < 0 || rs232cfg->dwBaudRate > 14)
            rs232cfg->dwBaudRate = 15;

        g_tl_globel_param.TL_Server_Com2info[nChannel].eBaudRate = baudrate[rs232cfg->dwBaudRate];
        g_tl_globel_param.TL_Server_Com2info[nChannel].eBaudRate = rs232cfg->dwBaudRate;
        g_tl_globel_param.TL_Server_Com2info[nChannel].nDataBits = rs232cfg->byDataBit;
        g_tl_globel_param.TL_Server_Com2info[nChannel].nStopBits = rs232cfg->byStopBit;
        g_tl_globel_param.TL_Server_Com2info[nChannel].nParity = rs232cfg->byParity;
        g_tl_globel_param.TL_Server_Com2info[nChannel].nStreamControl = rs232cfg->byFlowcontrol;
        
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_SERIAL:
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_PTZ_PROTOCOLCFG:   //获取485串口参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_ADD_PTZ_PROTOCOL:      //添加云台协议
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_ALARMINCFG:        //获取报警输入参数
    {            
        QMAPI_NET_ALARMINCFG alarmincfg;
        int i, day;

        memset(&alarmincfg, 0, sizeof(QMAPI_NET_ALARMINCFG));
        alarmincfg.dwSize = sizeof(QMAPI_NET_ALARMINCFG);
        alarmincfg.dwChannel = nChannel;
        memcpy(alarmincfg.csAlarmInName, g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].szSensorName, QMAPI_NAME_LEN);
        alarmincfg.byAlarmType = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].dwSensorType;
        alarmincfg.stHandle.wDuration = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nDuration;
        // Snap 
        if(g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bSnapShot)
        {
            if(g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nCaptureChannel > 0)
            {
                SET_BIT(&alarmincfg.stHandle.bySnap[0], g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nCaptureChannel-1);
            }
            alarmincfg.stHandle.wSnapNum = 1;
            alarmincfg.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOSNAP;
        }   
        // Alarm Out 
        for (i = 0; i < 4; ++i)
        {
            if(g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bAlarmIOOut[i])
            {
                SET_BIT(&alarmincfg.stHandle.byRelAlarmOut[0], i);
                alarmincfg.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOALARMOUT;
            }
        }
        //Preset
        for (i = 0; i < 4; ++i)
        {
            if(g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bPresetChannel[i].wPresetPoint1
                || g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bPresetChannel[i].wPresetPoint2)
            {
                alarmincfg.stHandle.stPtzLink[i].byType = 1;
                alarmincfg.stHandle.stPtzLink[i].byValue = g_tl_globel_param.TL_Server_Cominfo[i].nPrePos;
                alarmincfg.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOPTZ;
            }
        }

        for(day = 0; day < 7; day++)
        {
            if(!g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].bEnable)
                continue;

            alarmincfg.byAlarmInHandle = 1;
			alarmincfg.stScheduleTime[day][0].bEnable = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].bEnable;
            alarmincfg.stScheduleTime[day][0].byStartHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].BeginTime1.bHour;
            alarmincfg.stScheduleTime[day][0].byStartMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].BeginTime1.bMinute;
            alarmincfg.stScheduleTime[day][0].byStopHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].EndTime1.bHour;
            alarmincfg.stScheduleTime[day][0].byStopMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].EndTime1.bMinute;

			alarmincfg.stScheduleTime[day][1].bEnable = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].bEnable;
            alarmincfg.stScheduleTime[day][1].byStartHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].BeginTime2.bHour;
            alarmincfg.stScheduleTime[day][1].byStartMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].BeginTime2.bMinute;
            alarmincfg.stScheduleTime[day][1].byStopHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].EndTime2.bHour;
            alarmincfg.stScheduleTime[day][1].byStopMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day].EndTime2.bMinute;
        }
        if(g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].bEnable)
        {
            alarmincfg.byAlarmInHandle = 1;
            for(day = 0; day < 7; day++)
            {                   
                alarmincfg.stScheduleTime[day][0].byStartHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].BeginTime1.bHour;
                alarmincfg.stScheduleTime[day][0].byStartMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].BeginTime1.bMinute;
                alarmincfg.stScheduleTime[day][0].byStopHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].EndTime1.bHour;
                alarmincfg.stScheduleTime[day][0].byStopMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].EndTime1.bMinute;

                alarmincfg.stScheduleTime[day][1].byStartHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].BeginTime2.bHour;
                alarmincfg.stScheduleTime[day][1].byStartMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].BeginTime2.bMinute;
                alarmincfg.stScheduleTime[day][1].byStopHour = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].EndTime2.bHour;
                alarmincfg.stScheduleTime[day][1].byStopMin = g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[7].EndTime2.bMinute;
            }   
        }

        memcpy(Param, &alarmincfg, sizeof(QMAPI_NET_ALARMINCFG));

        break;
    }

    case QMAPI_SYSCFG_SET_ALARMINCFG:        //设置报警输入参数
    {
        QMAPI_NET_ALARMINCFG *alarmincfg = Param;
        int i, day;

        g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nSensorID = nChannel;
        g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nDuration = alarmincfg->stHandle.wDuration;
        memcpy( g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].szSensorName,alarmincfg->csAlarmInName, QMAPI_NAME_LEN);
        g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].dwSensorType = alarmincfg->byAlarmType;
        g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bEnable = alarmincfg->byAlarmInHandle;  

        //ALARM OUT
        if(alarmincfg->stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOALARMOUT)
        {
            for(i = 0; i < 4; i++)
            {
                g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bAlarmIOOut[i] = CHK_BIT(&alarmincfg->stHandle.byRelAlarmOut[i/8], i);
            }                           
        }
        else
        {
            for(i=0; i<4; i++)
            {
                g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bAlarmIOOut[i] = 0;
            }
        }

        // Snap 
        if(alarmincfg->stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOSNAP)
        {
            g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bSnapShot = 1;
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                if(CHK_BIT(&alarmincfg->stHandle.bySnap[i/8], i))
                {
                    g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].nCaptureChannel = i + 1;
                    break;
                }
            }               
        }
        else
        {
            g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bSnapShot = 0;   
        }
        
        //Preset
        if(alarmincfg->stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOPTZ)
        {               
            for (i = 0; i < 4; ++i)
            {
                if(alarmincfg->stHandle.stPtzLink[i].byType ==1)                        
                {                   
                    g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bPresetChannel[i].wPresetPoint1 = 1;
                    g_tl_globel_param.TL_Server_Cominfo[i].nPrePos = alarmincfg->stHandle.stPtzLink[i].byValue;
                }
            }
        }
        else
        {
            g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bPresetChannel[i].wPresetPoint1 = 0;
            g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].bPresetChannel[i].wPresetPoint2 = 0;
        }
        if(alarmincfg->byAlarmInHandle == 0)
        {
            for(day = 0; day < 7; day++)
            {
                TL_SCHED_TIME   *pstScheduleTime = &g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day];
                pstScheduleTime->bEnable = 0;
            }
        }
        else
        {
            for(day = 0; day < 7; day++)
            {
                TL_SCHED_TIME   *pstScheduleTime = &g_tl_globel_param.TL_Channel_Probe_Alarm[nChannel].tlScheduleTime[day];
				pstScheduleTime->bEnable = alarmincfg->stScheduleTime[day][0].bEnable;
                pstScheduleTime->BeginTime1.bHour = alarmincfg->stScheduleTime[day][0].byStartHour;
                pstScheduleTime->BeginTime1.bMinute = alarmincfg->stScheduleTime[day][0].byStartMin;
                pstScheduleTime->EndTime1.bHour = alarmincfg->stScheduleTime[day][0].byStopHour;
                pstScheduleTime->EndTime1.bMinute = alarmincfg->stScheduleTime[day][0].byStopMin;

                pstScheduleTime->BeginTime2.bHour = alarmincfg->stScheduleTime[day][1].byStartHour;
                pstScheduleTime->BeginTime2.bMinute = alarmincfg->stScheduleTime[day][1].byStartMin;
                pstScheduleTime->EndTime2.bHour = alarmincfg->stScheduleTime[day][1].byStopHour;
                pstScheduleTime->EndTime2.bMinute = alarmincfg->stScheduleTime[day][1].byStopMin;
#if 0
                if((pstScheduleTime->BeginTime1.bHour*24+pstScheduleTime->BeginTime1.bMinute 
                    < pstScheduleTime->EndTime1.bHour*24 + pstScheduleTime->EndTime1.bMinute) ||
                    (pstScheduleTime->BeginTime2.bHour*24+pstScheduleTime->BeginTime2.bMinute 
                    < pstScheduleTime->EndTime2.bHour*24 + pstScheduleTime->EndTime2.bMinute))
                {
                    pstScheduleTime->bEnable = 1;
                }
#endif
            }
        }   
        break;
    }

    case QMAPI_SYSCFG_DEF_ALARMINCFG:
    {
        break;
    }

    case QMAPI_SYSCFG_GET_ALARMOUTCFG:       //获取报警输出参数
    {
        QMAPI_NET_ALARMOUTCFG alarmoutcfg;

        memset(&alarmoutcfg, 0, sizeof(QMAPI_NET_ALARMOUTCFG));
        alarmoutcfg.dwSize = sizeof(QMAPI_NET_ALARMOUTCFG);
        //alarmoutcfg.csAlarmOutName;
        //alarmoutcfg.stScheduleTime;
        break;
    }

    case QMAPI_SYSCFG_SET_ALARMOUTCFG:       //设置报警输出参数
    {
        break;
    }

    case QMAPI_SYSCFG_GET_ZONEANDDSTCFG:     //获取时区和夏时制参数
    {
        if(NULL == Param)
        {
            return QMAPI_FAILURE;
        }
        memcpy(Param,&g_tl_globel_param.stTimezoneConfig,sizeof(QMAPI_NET_ZONEANDDST));
        break;
    }

    case QMAPI_SYSCFG_SET_ZONEANDDSTCFG:     //设置时区和夏时制参数
    {
        if(NULL == Param)
        {
            return QMAPI_FAILURE;
        }
        int oldtz = g_tl_globel_param.stTimezoneConfig.nTimeZone;
        QMAPI_NET_ZONEANDDST *pTZ_info = (QMAPI_NET_ZONEANDDST *)Param;
        memcpy(&g_tl_globel_param.stTimezoneConfig,Param,sizeof(QMAPI_NET_ZONEANDDST));
        if(pTZ_info->nTimeZone != oldtz)
        {
			QMapi_SysTime_ioctrl(Handle, nCmd, nChannel, Param, nSize);
        }
        //TL_rtc_to_system_time();
        break;
    }

    case QMAPI_SYSCFG_GET_PREVIEWCFG:        //获取预览参数
    {
        return -1;                      //ipc 无此设置
        break;
    }

    case QMAPI_SYSCFG_SET_PREVIEWCFG:        //设置预览参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_PREVIEWCFG:
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_VIDEOOUTCFG:       //获取视频输出参数
    {
        return -1;                      //ipc 无此设置
        break;
    }

    case QMAPI_SYSCFG_SET_VIDEOOUTCFG:       //设置视频输出参数
    {
        return -1;                      //ipc 无此设置
        break;
    }

    case QMAPI_SYSCFG_GET_DEF_VIDEOOUTCFG:   //设置视频输出参数
    {
        return -1;                      //ipc 无此设置
        break;
    }

    case QMAPI_SYSCFG_GET_USERCFG:           //获取用户参数
    {
        QMAPI_NET_USER_INFO user_info;    //nChannel 表示编号
        struct in_addr stInAddr;
        if(nChannel < 0  || nChannel >= 10)
        {
            return QMAPI_ERR_ILLEGAL_PARAM;
        }
        memset(&user_info, 0, sizeof(QMAPI_NET_USER_INFO));
        stInAddr.s_addr = g_tl_globel_param.TL_Server_User[nChannel].dwUserIP;
        user_info.dwSize = sizeof(QMAPI_NET_USER_INFO);
        user_info.bEnable = g_tl_globel_param.TL_Server_User[nChannel].bEnable;
        user_info.dwIndex = g_tl_globel_param.TL_Server_User[nChannel].dwIndex;
        user_info.byPriority = g_tl_globel_param.TL_Server_User[nChannel].dwIndex; 
        user_info.dwUserRight = g_tl_globel_param.TL_Server_User[nChannel].dwUserRight;
        memcpy(user_info.csUserName, g_tl_globel_param.TL_Server_User[nChannel].csUserName, QMAPI_NAME_LEN);
        memcpy(user_info.csPassword, g_tl_globel_param.TL_Server_User[nChannel].csPassword, QMAPI_PASSWD_LEN);
        memcpy(user_info.stIPAddr.csIpV4, inet_ntoa(stInAddr), QMAPI_PASSWD_LEN);
        memcpy(user_info.byMACAddr, g_tl_globel_param.TL_Server_User[nChannel].byMACAddr, QMAPI_MACADDR_LEN);

        memcpy(Param, &user_info, sizeof(QMAPI_NET_USER_INFO));
        break;
    }

    case QMAPI_SYSCFG_SET_USERCFG:           //设置用户参数
    {
        QMAPI_NET_USER_INFO *user_info = Param;   //nChannel 表示编号
        if(nChannel < 0  || nChannel >= 10)
        {
            return QMAPI_ERR_ILLEGAL_PARAM;
        }
        g_tl_globel_param.TL_Server_User[nChannel].bEnable = user_info->bEnable;
        g_tl_globel_param.TL_Server_User[nChannel].dwIndex = user_info->dwIndex;
        g_tl_globel_param.TL_Server_User[nChannel].dwUserRight = user_info->dwUserRight;
        g_tl_globel_param.TL_Server_User[nChannel].dwNetPreviewRight = ROOT_USER_NET_PREVIEW_RIGHT;
        memcpy(g_tl_globel_param.TL_Server_User[nChannel].csUserName, user_info->csUserName, QMAPI_NAME_LEN);
        memcpy(g_tl_globel_param.TL_Server_User[nChannel].csPassword, user_info->csPassword, QMAPI_PASSWD_LEN);
        inet_aton(user_info->stIPAddr.csIpV4, (struct in_addr *)(&g_tl_globel_param.TL_Server_User[nChannel].dwUserIP));
        memcpy(user_info->byMACAddr, g_tl_globel_param.TL_Server_User[nChannel].byMACAddr, QMAPI_MACADDR_LEN);

        break;
    }

    case QMAPI_SYSCFG_GET_USERGROUPCFG:      //获取用户组参数
    {
        return -1;                      //ipc 无用户组概念
        break;
    }

    case QMAPI_SYSCFG_SET_USERGROUPCFG:      //设置用户组参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_EXCEPTIONCFG:      //获取异常参数
    {
        return -1;                      //ipc 无异常报警
        break;
    }

    case QMAPI_SYSCFG_SET_EXCEPTIONCFG:      //设置异常参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_HDCFG:             //获取硬盘参数
    {
        QMAPI_NET_HDCFG hd_info;
        memset(&hd_info, 0, sizeof(QMAPI_NET_HDCFG));
        if (HdGetDiskInfo(&hd_info))
        {
            nResult = QMAPI_FAILURE;
        }
        memcpy(Param, &hd_info, sizeof(QMAPI_NET_HDCFG));
        break;
    }

    case QMAPI_SYSCFG_SET_HDCFG:             //设置(单个)硬盘参数
    {
        
        break;
    }

    case QMAPI_SYSCFG_HD_FORMAT:             //格式化硬盘
    {
        nResult = HdFormat(Param);
        if(nResult)
        {
            printf("%s: QMAPI_SYSCFG_HD_FORMAT fail!ret=%d\n",__FUNCTION__, nResult);
            return QMAPI_FAILURE;
        }
        break;
    }

    case QMAPI_SYSCFG_GET_HD_FORMAT_STATUS:  //格式化硬盘状态以及进度
    {
        QMAPI_NET_DISK_FORMAT_STATUS FormatStatus;
        memset(&FormatStatus, 0, sizeof(QMAPI_NET_DISK_FORMAT_STATUS));
        FormatStatus.dwSize = sizeof(QMAPI_NET_DISK_FORMAT_STATUS);

        if(HdGetFormatStatus(&FormatStatus))
            return QMAPI_FAILURE;

        memcpy(Param, &FormatStatus, sizeof(QMAPI_NET_DISK_FORMAT_STATUS));
        printf("%s: QMAPI_SYSCFG_GET_HD_FORMAT_STATUS!\n",__FUNCTION__);
        break;
    }
	case QMAPI_SYSCFG_UNLOAD_DISK:
    {
		HdUnloadAllDisks();
		break;
	}
    case QMAPI_SYSCFG_GET_HDGROUPCFG:        //获取硬盘组参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_SET_HDGROUPCFG:        //设置硬盘组参数
    {
        return -1;
        break;
    }

    case QMAPI_SYSCFG_GET_RESTORECFG:        //恢复出厂值配置
    {
        
        break;
    }
    case QMAPI_SYSCFG_SET_RESTORECFG:
    {
        QMAPI_NET_RESTORECFG *pstRestorecfg = Param;
        if(pstRestorecfg->dwMask & NETWORK_CFG)
        {
            TL_initDefaultSet(HARDRESET, 1);
        }
        else
        {
            TL_initDefaultSet(SOFTRESET, 1);
        }
		Qmapi_System_Reboot();
        break;
    }
/*
    case DMS_NET_GET_DEF_RESTORECFG:
    {
        break;
    }
*/
    // 配置音频对讲数据回调函数(OnNetAudioStreamCallBack)
    case QMAPI_NET_REG_AUDIOCALLBACK:
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }

        pstSysNetApi->pfAudioStreamCallBk = Param;
        break;
    }

    // 配置视频数据回调函数(QMAPI_NET_VIDEOCALLBACK)
    // 每个通道的不通码流可以分别配置不同的回调函数接口
    case QMAPI_NET_REG_VIDEOCALLBACK:
    {
        QMAPI_NET_VIDEOCALLBACK *pNetVideoCallback = (QMAPI_NET_VIDEOCALLBACK *)Param;
        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }

        if(pNetVideoCallback->nStreamType == QMAPI_MAIN_STREAM)
        {
            pstSysNetApi->pfMainVideoCallBk = pNetVideoCallback->pCallBack;
            pstSysNetApi->dwMainVideoUser = pNetVideoCallback->dwUserData;
        }
        else if(pNetVideoCallback->nStreamType == QMAPI_SECOND_STREAM)
        {
            pstSysNetApi->pfSecondVideoCallBk = pNetVideoCallback->pCallBack;
            pstSysNetApi->dwSecondVideoUser = pNetVideoCallback->dwUserData;                
        }
        break;
    }
    // 配置报警消息回调函数(OnNetAlarmCallback)
    case QMAPI_NET_REG_ALARMCALLBACK:
    {
        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }

        pstSysNetApi->pfAlarmCallBk = Param;
        break;
    }
#ifndef TL_Q3_PLATFORM

    // 配置串口数据回调函数(OnNetSerialDataCallback)
    case QMAPI_NET_REG_SERIALDATACALLBACK:
    {
        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }

        pstSysNetApi->pfnSerialDataCallBk = Param;
        break;
    }

    // 打开透明串口传输
    case QMAPI_NET_STA_OPEN_SERIALTRANSP:
    {
        break;
    }

    // 关闭透明串口传输
    case QMAPI_NET_STA_CLOSE_SERIALTRANSP:
    {
        break;
    }

    // 云台控制( QMAPI_NET_PTZ_CONTROL 结构体)
    case QMAPI_NET_STA_PTZ_CONTROL:
    {
        QMAPI_NET_PTZ_CONTROL *ptz_control = Param;
        #if 0 //tmp stop by david
        if(0 == DMS_Check_YT())
        {
            DMS_YT_ACtion(ptz_control->dwCommand,(unsigned char)ptz_control->dwParam);
            return QMAPI_SUCCESS;
        }
        #endif
        nResult = ptzmng_control(ptz_control->nChannel, ptz_control->dwCommand, ptz_control->dwParam, ptz_control->byRes);
        if(nResult<0)
            return QMAPI_FAILURE;

        break;
    }

    case QMAPI_SYSCFG_GET_ALL_PRESET:
    {
        QMAPI_NET_PRESET_INFO *preset = (QMAPI_NET_PRESET_INFO*)Param;

        nResult = ptzmng_getallpreset(nChannel, preset);
        if(nResult<0)
            return QMAPI_FAILURE;
        break;
    }
    case QMAPI_SYSCFG_GET_CRUISE_CFG:
    {
        nResult = ptzmng_getcruisecfg(nChannel, Param);
        if(nResult<0)
            return QMAPI_FAILURE;
        break;
    }
    case QMAPI_SYSCFG_SET_CRUISE_INFO:
    {
        nResult = ptzmng_setcruise(nChannel, Param);
        if(nResult<0)
            return QMAPI_FAILURE;
        break;
    }
    case QMAPI_SYSCFG_GET_PTZ_PROTOCOL_DATA:
    {
        QMAPI_NET_PTZ_PROTOCOL_DATA *ptz_data = (QMAPI_NET_PTZ_PROTOCOL_DATA *)Param;
        nResult = ptzmng_getprotocol(ptz_data);
        if(nResult!=QMAPI_SUCCESS)
            return QMAPI_FAILURE;
        break;
    }
#endif
    // 报警输出控制(命令模式QMAPI_NET_ALARMOUT_CONTROL)
    case QMAPI_NET_STA_ALARMOUT_CONTROL:
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_ALARMOUT_CONTROL *alarm_control = Param;
        DMS_DEV_GPIO_SetAlarmOutState(alarm_control->nChannel, alarm_control->wAction);
#endif
        break;
    }

    // 获取报警输出状态(QMAPI_NET_SENSOR_STATE)
    case QMAPI_NET_GET_ALARMOUT_STATE:
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_SENSOR_STATE *sensor_state = Param;
        DWORD dwState;
        int max_io_in, max_io_out;
        int i;
        tl_get_io_alarm_num(&max_io_in, &max_io_out);
        DMS_DEV_GPIO_GetAlarmOutState(&dwState);
        printf("#### %s %d, max_io_in=%d, max_io_out=%d, dwSensorID=0x%X, dwState=0x%X\n", 
                    __FUNCTION__, __LINE__, max_io_in, max_io_out, (int)sensor_state->dwSensorID, (int)dwState);
        sensor_state->dwSensorID = 0;
        sensor_state->dwSensorOut = 0;
        for(i = 0; i < max_io_out; i++)
        {
            sensor_state->dwSensorID += 1<<(i+16);
            if(1<<i & dwState)
            {
                sensor_state->dwSensorOut += 1<<(i+16);
            }
        }
#endif
        break;
    }

    // 获取报警输入状态(QMAPI_NET_SENSOR_STATE)
    case QMAPI_NET_GET_ALARMIN_STATE:
    {
#ifndef TL_Q3_PLATFORM
        QMAPI_NET_SENSOR_STATE *sensor_state = Param;
        int max_io_in, max_io_out;
        int i;

        tl_get_io_alarm_num(&max_io_in, &max_io_out);
        sensor_state->dwSensorID = 0;
        sensor_state->dwSensorOut = 0;
        for(i = 0; i < max_io_in; i++)
        {
            sensor_state->dwSensorID += 1<<i;
        }
#endif

        break;
    }
    case QMAPI_NET_STA_GET_VIDEO_STATE:
    {
        QMAPI_NET_VIDEO_STATE *pstVideoState = (QMAPI_NET_VIDEO_STATE *)Param;
        memset(pstVideoState, 0x00, sizeof(QMAPI_NET_VIDEO_STATE));
        pstVideoState->dwSize = sizeof(QMAPI_NET_VIDEO_STATE);
		pstVideoState->nChannelNum = 1;
		pstVideoState->nState[0] = QMAPI_ALARM_TYPE_BUTT;
    }
    break;
    // 请求视频流关键帧 
    case QMAPI_NET_STA_IFRAME_REQUEST:
    {
		// need fixme yi.zhang
		Q3_Encode_Intra_FrameEx(nChannel, *(int *)Param);
        break;
    }

    // 重启设备
    case QMAPI_NET_STA_REBOOT:
    {
		Qmapi_System_Reboot();
        break;
    }

    // 设备关机
    case QMAPI_NET_STA_SHUTDOWN:
    {
        return -1;          //ipc 无关机功能
        break;
    }

    // 抓拍图片(QMAPI_NET_SNAP_DATA)
    /* 数据Buffer内存必须由上层自己分配，dwBufSize是内存大小*/
    case QMAPI_NET_STA_SNAPSHOT:
    {
        QMAPI_NET_SNAP_DATA *snap_data = Param;
        nResult = Q3_Video_SnapShot(nChannel, &snap_data->pDataBuffer, (int*)&snap_data->dwBufSize);
        break;
    }
    //录像控制(QMAPI_NET_REC_CONTROL)
    case QMAPI_NET_STA_REC_CONTROL:
    {

        break;
    }

    //注册回放回调函数
    case QMAPI_NET_REG_PLAYBACKCALLBACK:
    {
        return -1;
        break;
    }
#if 0
    //开始音频对讲
    case QMAPI_NET_STA_START_TALKAUDIO:
    {
        QMAPI_NET_AUDIOTALK   *pNetAudioTalk = (QMAPI_NET_AUDIOTALK   *)Param;
        if(pstSysNetApi == NULL)
        {
            return DMS_ERR_INVALID_HANDLE;
        }
        pstSysNetApi->dwAudioUser = pNetAudioTalk->dwUser;
		DMS_DEV_GPIO_SetSpeakerOnOff(1);
        printf("function(%s) QMAPI_NET_STA_START_TALKAUDIO enFormatTag:%d, nNumPerFrm:%d\n",  __FUNCTION__, pNetAudioTalk->enFormatTag, pNetAudioTalk->nNumPerFrm);
        nResult = startAudiotalk(pNetAudioTalk->enFormatTag, pNetAudioTalk->nNumPerFrm, AUDIOTALK_NETPT_DMS,  pstSysNetApi->pfAudioStreamCallBk, pstSysNetApi->dwAudioUser);
#ifdef TL_Q3_PLATFORM
		if(nResult)
			DMS_DEV_GPIO_SetSpeakerOnOff(0);
#endif
        break;
    }

    //停止音频对讲
    case QMAPI_NET_STA_STOP_TALKAUDIO:
    {
        nResult = stopAudiotalk();
#ifdef TL_Q3_PLATFORM
		DMS_DEV_GPIO_SetSpeakerOnOff(0);
#endif
        break;
    }
#endif
    //获取系统时间
    case QMAPI_NET_STA_GET_SYSTIME:
    {
		QMapi_SysTime_ioctrl(Handle, nCmd, nChannel, Param, nSize);
        break;
    }

    //设置系统时间
    case QMAPI_NET_STA_SET_SYSTIME:
    {
		QMapi_SysTime_ioctrl(Handle, nCmd, nChannel, Param, nSize);
        // need fixme yi.zhang
		Q3_Encode_Intra_FrameEx(0, QMAPI_MAIN_STREAM);
        break;
    }

    //打开某通道视频
    case QMAPI_NET_STA_START_VIDEO:
    {
        int stream = *(int *)Param;

        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }
        
        if(QMAPI_MAIN_STREAM == stream)
        {
            pstSysNetApi->bMainVideoStarted |= (1<<nChannel);
        }
        else if(QMAPI_SECOND_STREAM == stream)
        {
            pstSysNetApi->bSecondVideoStarted |= (1<<nChannel);
        }
        else if(QMAPI_MOBILE_STREAM == stream)
        {
            pstSysNetApi->bMobileVideoStarted |= (1<<nChannel);
        }
        break;
    }

    //关闭某通道视频
    case QMAPI_NET_STA_STOP_VIDEO:
    {
        int stream = *(int *)Param;
        if(pstSysNetApi == NULL)
        {
            printf("### %s(%d) ERROR: pstSysNetApi == NULL\n",__FUNCTION__,__LINE__);
            return QMAPI_ERR_INVALID_HANDLE;
        }
        
        if(QMAPI_MAIN_STREAM == stream)
        {
            pstSysNetApi->bMainVideoStarted &= ~(1<<nChannel);
        }
        else if(QMAPI_SECOND_STREAM== stream)
        {
            pstSysNetApi->bSecondVideoStarted &= ~(1<<nChannel);
        }
        else if(QMAPI_MOBILE_STREAM== stream)
        {
            pstSysNetApi->bMobileVideoStarted &= ~(1<<nChannel);
        }
        break;
    }
    //保存设置参数
    case QMAPI_SYSCFG_SET_SAVECFG:
    {
        tl_write_all_parameter();
        printf("QMAPI_SYSCFG_SET_SAVECFG\n");
        if(g_bNeedRestart || g_tl_globel_param.bRtspPortChanged)
        {
        	Qmapi_System_Reboot();
        }
        break;
    }
    case QMAPI_SYSCFG_SET_SAVECFG_ASYN:
    {
	   	tl_write_all_parameter_asyn();
		break;
    }
    case QMAPI_SYSCFG_GET_RTSPCFG:
    {
		if(!Param || nSize <sizeof(QMAPI_NET_RTSP_CFG))
            return QMAPI_FAILURE;
        memcpy(Param, &g_tl_globel_param.stRtspCfg, nSize);
    }
    break;
    case QMAPI_SYSCFG_SET_RTSPCFG:
    {
        QMAPI_NET_RTSP_CFG *lpstRtspCfg = (QMAPI_NET_RTSP_CFG*)Param;
        if(lpstRtspCfg->dwPort <= 0 )
        {
            printf("%s:QMAPI_SYSCFG_SET_RTSPCFG Port is Invalid\n",__FUNCTION__);
            break;
        }
        if(g_tl_globel_param.stRtspCfg.dwPort != lpstRtspCfg->dwPort)
        {
            g_tl_globel_param.bRtspPortChanged = 1;
        }
        else
        {
            g_tl_globel_param.bRtspPortChanged = 0;
        }
        memcpy(&g_tl_globel_param.stRtspCfg, lpstRtspCfg, sizeof(g_tl_globel_param.stRtspCfg));
    }
    break;
    case QMAPI_SYSCFG_GET_NTPCFG:
    {
        if(!Param || nSize <sizeof(QMAPI_NET_NTP_CFG))
            return QMAPI_FAILURE;
        memcpy(Param, &g_tl_globel_param.stNtpConfig, nSize);
        break;
    }
    case QMAPI_SYSCFG_SET_NTPCFG://设置NTP
    {
        //printf("%s: DMS_NET_SET_NTFCFG!\n",__FUNCTION__);
        if(!Param || nSize <sizeof(QMAPI_NET_NTP_CFG))
            return QMAPI_FAILURE;

        QMAPI_NET_NTP_CFG *ntpinfo = (QMAPI_NET_NTP_CFG *)Param;
        if (ntpinfo->wInterval<1)
			ntpinfo->wInterval = 1;
		if (ntpinfo->byEnableNTP)
		{
			QMapi_SysTime_ioctrl(Handle, nCmd, nChannel, Param, nSize);
		}
        memcpy(&g_tl_globel_param.stNtpConfig, ntpinfo, nSize);
        break;
    }
 
    case QMAPI_NET_STA_UPGRADE_REQ:
    {
        QMAPI_NET_UPGRADE_REQ *pUpgReq = (QMAPI_NET_UPGRADE_REQ *)Param;
        nResult = QMAPI_UpgradeFlashReq(-1, pUpgReq);
        break;
    }
    case QMAPI_NET_STA_UPGRADE_DATA:
    {
        QMAPI_NET_UPGRADE_DATA *pUpgData = (QMAPI_NET_UPGRADE_DATA *)Param;
        nResult = QMAPI_UpgradeFlashData(-1, pUpgData);
        break;
    }
    case QMAPI_NET_STA_UPGRADE_RESP:
    {
        nResult = QMAPI_UpgradeFlashGetState((QMAPI_NET_UPGRADE_RESP * )Param);
        break;
    }
    case QMAPI_NET_STA_UPGRADE_PREPARE:
    {
        nResult = QMAPI_PrepareUpgrade((CBUpgradeProc)Param);
        break;
    }
    case QMAPI_NET_STA_UPGRADE_START:
    {
        nResult = QMAPI_StartUpgrade((char *)Param);
        break;
    }
    case QMAPI_SYSCFG_NOTIFY_USER:
    {
        if(!Param || nSize < sizeof(QMAPI_NET_NOTIFY_USER_INFO))
            return QMAPI_FAILURE;
        
        break;
    }

    case QMAPI_SYSCFG_SET_CONSUMER:
    {
        if(!Param || nSize != sizeof(QMAPI_NET_CONSUMER_INFO))
            return QMAPI_FAILURE;

        QMAPI_NET_CONSUMER_INFO *pstConsumerInfo = (QMAPI_NET_CONSUMER_INFO *)Param;
        int i = pstConsumerInfo->bPrivacyMode;

        memcpy(&g_tl_globel_param.stConsumerInfo, pstConsumerInfo, nSize);

        g_tl_globel_param.TL_Channel_Motion_Detect[0].bEnable = pstConsumerInfo->stPrivacyInfo[i].stMotionInfo.bEnable;
        g_tl_globel_param.TL_Channel_Motion_Detect[0].dwSensitive = pstConsumerInfo->stPrivacyInfo[i].stMotionInfo.dwSensitive;
        memcpy(g_tl_globel_param.TL_Channel_Motion_Detect[0].tlScheduleTime,
                pstConsumerInfo->stPrivacyInfo[i].stMotionInfo.stSchedTime, sizeof(QMAPI_SCHED_TIME)*8);

        g_tl_globel_param.stChannelRecord[0].dwRecord = pstConsumerInfo->stPrivacyInfo[i].stRecordInfo.bEnable;
        memcpy(g_tl_globel_param.stChannelRecord[0].stRecordSched,
                pstConsumerInfo->stPrivacyInfo[i].stRecordInfo.stRecordSched, sizeof(QMAPI_RECORDSCHED)*8*7);

        QMAPI_CONSUMER_PRIVACY_INFO p = pstConsumerInfo->stPrivacyInfo[i];
        printf("#### %s %d, bPrivacyMode=%d, mute=%d, audio=%d, md_report=%d, mode=%ld, bittype=%d, RecEnable=%d, MDEnable=%d, sen=%ld, pushAlarm=%d\n",
                __FUNCTION__, __LINE__, i, p.bSpeakerMute, p.bPreviewAudioEnable, p.bMotdectReportEnable, p.dwSceneMode, p.enBitrateType,
                p.stRecordInfo.bEnable, p.stMotionInfo.bEnable, p.stMotionInfo.dwSensitive, p.stMotionInfo.bPushAlarm);
        break;
    }

    case QMAPI_SYSCFG_GET_CONSUMER:
    {
        if(!Param || nSize != sizeof(QMAPI_NET_CONSUMER_INFO))
            return QMAPI_FAILURE;

        QMAPI_NET_CONSUMER_INFO *pstConsumerInfo = (QMAPI_NET_CONSUMER_INFO *)Param;

        memcpy(pstConsumerInfo, &g_tl_globel_param.stConsumerInfo, nSize);
        break;
    }

    case QMAPI_SYSCFG_GET_NETSTATUS:
    {
        if(!Param || nSize != sizeof(QMAPI_NET_NETWORK_STATUS))
            return QMAPI_FAILURE;

        QMAPI_NET_NETWORK_STATUS *pstNetworkStatus = (QMAPI_NET_NETWORK_STATUS *)Param;
        memcpy(pstNetworkStatus, &g_tl_globel_param.stNetworkStatus, nSize);
        break;
    }
#if 0
    case QMAPI_NET_GET_WIFICFG_V2:           //获取WIFI参数
    {
        QMAPI_NET_WIFI_CONFIG_V2 *pWifiCfg = (QMAPI_NET_WIFI_CONFIG_V2 *)Param;
        if(!pWifiCfg)
            return QMAPI_FAILURE;
#ifdef TL_Q3_PLATFORM
        q3_Wireless_GetConfig(pWifiCfg);
#else
        memcpy(Param, &g_tl_globel_param.stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG_V2));
#endif
        break;
    }

    case QMAPI_NET_SET_WIFICFG_V2:           //设置WIFI参数
    {
        QMAPI_NET_WIFI_CONFIG_V2 *pWifiConfig = Param;
        if(!pWifiConfig || pWifiConfig->dwSize != sizeof(QMAPI_NET_WIFI_CONFIG_V2))
            return QMAPI_FAILURE;

        unsigned char byMac[6] = {0};
        memcpy(byMac, g_tl_globel_param.stWifiConfig.byMacAddr, 6);
        memcpy(&g_tl_globel_param.stWifiConfig, pWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG_V2));
        memcpy(g_tl_globel_param.stWifiConfig.byMacAddr, byMac, 6);
#ifdef TL_Q3_PLATFORM
        q3_Wireless_SetConfig(&g_tl_globel_param.stWifiConfig);
#endif
        break;
    }
#endif

    case QMAPI_SYSCFG_GET_RESETSTATE:
    {
        if(!Param)
            return QMAPI_FAILURE;

        DMS_DEV_GPIO_GetResetState((DWORD *)Param);
        break;
    }

    case QMAPI_DEV_GET_DEVICEMAINTAINCFG:
        if(!Param)
			return QMAPI_FAILURE;

        memcpy(Param, &g_tl_globel_param.stMaintainParam, sizeof(QMAPI_NET_DEVICEMAINTAIN));
        break;
    case QMAPI_DEV_SET_DEVICEMAINTAINCFG:
        if(!Param || nSize != sizeof(QMAPI_NET_CONSUMER_INFO))
			return QMAPI_FAILURE;

        memcpy(&g_tl_globel_param.stMaintainParam, Param, sizeof(QMAPI_NET_DEVICEMAINTAIN));
        break;
	case QMAPI_SYSCFG_GET_ENCRYPT:
	{
        TLNV_SYSTEM_INFO *pinfo;
        unsigned short checksum;

        if(!Param || nSize < sizeof(TLNV_SYSTEM_INFO))
            return QMAPI_FAILURE;
			
        if (encrypt_info_read((char *)Param, sizeof(TLNV_SYSTEM_INFO)))
        {
            return QMAPI_FAILURE;
        }
        pinfo = (TLNV_SYSTEM_INFO *)Param;
        checksum = pinfo->woCheckSum;
        pinfo->woCheckSum = pinfo->cbSize;
        if (encrypt_info_check((char *)pinfo, sizeof(TLNV_SYSTEM_INFO), checksum))
        {
            return QMAPI_FAILURE;
        }
        pinfo->woCheckSum = checksum;
        break;
	}
     case QMAPI_SYSCFG_SET_ENCRYPT:
     {
        if(!Param || nSize < sizeof(TLNV_SYSTEM_INFO))
            return QMAPI_FAILURE;
        if (encrypt_info_write(Param, sizeof(TLNV_SYSTEM_INFO)))
        {
            return QMAPI_FAILURE;
        }
        break;
     }
	case QMAPI_SYSCFG_GET_TLSERVER:
	{
		if(!Param || nSize < sizeof(TLNV_SERVER_INFO))
			return QMAPI_FAILURE;
		memcpy(Param, &g_tl_globel_param.TL_Server_Info, nSize);
		break;
	}
	case QMAPI_SYSCFG_SET_TLSERVER:
	{
		if(!Param || nSize < sizeof(TLNV_SERVER_INFO))
			return QMAPI_FAILURE;
		memcpy(&g_tl_globel_param.TL_Server_Info, Param, nSize);
		break;
	}
    case QMAPI_SYSCFG_GET_ONVIFTESTINFO:
	{
		if(!Param || nSize < sizeof(QMAPI_SYSCFG_ONVIFTESTINFO))
			return QMAPI_FAILURE;
        
		memcpy(Param, &g_tl_globel_param.stONVIFInfo, nSize);
		break;
	}
    case QMAPI_SYSCFG_SET_ONVIFTESTINFO:
	{
		if(!Param || nSize < sizeof(QMAPI_SYSCFG_ONVIFTESTINFO))
			return QMAPI_FAILURE;
        
		memcpy(&g_tl_globel_param.stONVIFInfo, Param, nSize);
		break;
	}
	case QMAPI_SYSCFG_GET_LOGINFO:
	{
		if(!Param || nSize < sizeof(QMAPI_LOG_QUERY_S))
		    return QMAPI_FAILURE;

		QMAPI_LOG_QUERY_S *pLogResult = (QMAPI_LOG_QUERY_S *)Param;
		pLogResult->dwSize = sizeof(QMAPI_LOG_QUERY_S);
		pLogResult->u32Num = QMAPI_LOG_INDEX_MAXNUM;
		printf("==============Search_Log begin\n");
		printf("%s  %d, input:mainType=%u, subType=%u\n",
                __FUNCTION__, __LINE__, pLogResult->u32SearchMainType, pLogResult->u32SearchSubType);
		Search_Log(pLogResult->u32SearchMainType, pLogResult->u32SearchSubType, pLogResult->logIndex, &(pLogResult->u32Num), pLogResult->logExtInfo, sizeof(pLogResult->logExtInfo));

		//int i=0;
		//for(i=0; i<pLogQuery->u32Num; i++)
		//{
			//SYSTEMLOG(SLOG_TRACE, "......i=%04d, bUsed=%d, mainType=%d, subType=%d,time=%lu, blockID=%d\n", i, pLogQuery->logIndex[i].byUsed, pLogQuery->logIndex[i].byMainType, pLogQuery->logIndex[i].bySubType,pLogQuery->logIndex[i].time, pLogQuery->logIndex[i].byBlockID);
			//printf("111 i=%04d, bUsed=%d, mainType=%d, subType=%d,time=%lu, blockID=%d\n", i, pLogQuery->logIndex[i].byUsed, pLogQuery->logIndex[i].byMainType, pLogQuery->logIndex[i].bySubType,pLogQuery->logIndex[i].time, pLogQuery->logIndex[i].byBlockID);
			//if(0 != pLogQuery->logIndex[i].byBlockID)
			//{
				//SYSTEMLOG(SLOG_TRACE, "====ID:%d,  ExtInfo:%p,    %s\n", pLogQuery->logIndex[i].byBlockID, pLogQuery->logExtInfo[pLogQuery->logIndex[i].byBlockID], pLogQuery->logExtInfo[pLogQuery->logIndex[i].byBlockID]);
			//}
		//}

        break;
	}    
    case QMAPI_SYSCFG_GET_PTZPOS:
    case QMAPI_SYSCFG_SET_PTZPOS:
    default:
        printf("Unsupport cmd:0x%x!\n", nCmd);
        break;
    }

    return nResult;
}

//#endif


