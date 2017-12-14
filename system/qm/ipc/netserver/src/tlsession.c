/******************************************************************************
******************************************************************************/
#include "tlnetin.h"

#include "TLNetCommon.h"
#include "tlnet_header.h"
#include "tlsession.h"
#include "q3_wireless.h"

#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<openssl/err.h>

#include "qm_event.h" /* RM#2292: changed http port, restart web server. henry.li 2017/02/21 */

#define SETNETPARAM                             1
#define UPDATE_TO_FLASH                         1
#define UPDATE_TO_MINIDISK                      1
//#define UPDATE_TO_FILE                        1

#define MALLOC_BUFFER_SIZE				1024*446	//200K改成446K(最多1444个文件)

/* RM#2292: changed http port, restart web server. henry.li 2017/02/21 */
static unsigned int gHttpPortChanged = 0;

///TL Video Add New Protocol Login Right
int TL_Client_Login_Right_Add(DWORD UserRight , DWORD NetPreviewRight , DWORD UserID)
{
    PTL_RECORD_LOGIN_INFO addinfo=NULL;
    PTL_RECORD_LOGIN_INFO tmpNode=NULL;
    addinfo=(PTL_RECORD_LOGIN_INFO)malloc(sizeof(TL_RECORD_LOGIN_INFO));


    if(addinfo==NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&g_tl_record_login_mutex);
    memset(addinfo , 0 , sizeof(TL_RECORD_LOGIN_INFO));
    addinfo->dwUserRight = UserRight;
    addinfo->dwNetPreviewRight = NetPreviewRight;
    addinfo->LoginID = UserID;
    addinfo->pNext = NULL;
    tmpNode = g_record_login_info;
    if (tmpNode == NULL)
    {
        g_record_login_info=addinfo;
        g_record_login_info->pNext=NULL;
    }
    else
    {

        while(tmpNode->pNext)
        {
            if(tmpNode==tmpNode->pNext)
            {
                break;
            }
            tmpNode=tmpNode->pNext;
        }
        tmpNode->pNext = addinfo;
    }
    pthread_mutex_unlock(&g_tl_record_login_mutex);
    return 0;
}


///TL Video Add New Protocol Login Right Cut
int TL_Client_Login_Right_Cut(DWORD UserID)
{
    PTL_RECORD_LOGIN_INFO cutinfo=NULL;
    PTL_RECORD_LOGIN_INFO prvcutinfo=NULL;

    
    cutinfo=g_record_login_info;
    prvcutinfo=g_record_login_info;
    if(g_record_login_info==NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&g_tl_record_login_mutex);
    while(cutinfo)
    {
        if(cutinfo->LoginID==UserID)
        {
            //printf("[1] cutinfo->LoginID[%d] prvcutinfo[%d] cutinfo[%d] cutinfo->pNext[%d] prvcutinfo->pNext[%d]\n" , cutinfo->LoginID , prvcutinfo ,cutinfo , cutinfo->pNext , prvcutinfo->pNext);
            
            if(prvcutinfo!=cutinfo)
            {
                prvcutinfo->pNext = cutinfo->pNext;
            }
            else
            {
                g_record_login_info = cutinfo->pNext;
            }
            free(cutinfo);
            cutinfo=NULL;
            break;
        }
        prvcutinfo=cutinfo;
        cutinfo=cutinfo->pNext;
    }
    pthread_mutex_unlock(&g_tl_record_login_mutex); 
    return 0;
}

///TL Video Add New Protocol User Check Callback

int  TL_Video_CheckUser(const char *pUserName,const char *pPsw , const int pUserIp ,  const int pSocketID , const char *pUserMac , int *UserRight , int *NetPreviewRight)
{
    int i;
	QMAPI_NET_USER_INFO uinfo;
	QMAPI_NET_UPGRADE_RESP upgrade;
    BOOL bNameCheck=FALSE , bPasswordCheck=FALSE;

    ///if now update don't login
    if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_RESP, 0, &upgrade, sizeof(QMAPI_NET_UPGRADE_RESP)) == QMAPI_SUCCESS
		&& upgrade.state)
    {
        return -1;
    }
    
    //find user
    for(i=0;i<10;i++)
    {
    	memset(&uinfo, 0, sizeof(QMAPI_NET_USER_INFO));
    	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, &uinfo, sizeof(QMAPI_NET_USER_INFO));    	
		if(strcmp(uinfo.csUserName , pUserName)==0)///Check User
		{
			bNameCheck = TRUE;
			break;
		}
    	 
    }
    printf("#### %s %d,  pUserName:%s, pPsw:%s, pSocketID=%d\n",
				__FUNCTION__, __LINE__, pUserName, pPsw, pSocketID);
    if(i>=10)
    {
    	err();
        return NETCMD_NOT_USER;
    }
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    if (uinfo.bEnable)
    {
        ///Check Password
        if(strcmp(uinfo.csPassword , pPsw)==0)
        {
            bPasswordCheck = TRUE;
        }
        
        if (bNameCheck && bPasswordCheck)
        {
			*UserRight = 0xFFFFFFFF;
			*NetPreviewRight = 0xFFFFFFFF;

            ///Add Login Right
            TL_Client_Login_Right_Add(*UserRight , *NetPreviewRight , pSocketID);
            return 1;
        }
    }   
    if(!bNameCheck)
    {
        err();
        return NETCMD_NOT_USER;
    }
    else if(!bPasswordCheck)
    {
        err();
        return TLERR_PASSWORD_UNMATCH;
    }
    err();
    return NETCMD_NOT_USER;
}



///TL Video Send Right Check Message 
int TL_Video_Right_Check_Message(int userId  ,  int RightMsg)
{
    int ret;
    TL_SERVER_MSG   rightinfo;
    MSG_HEAD        toclientMsg;
    memset(&toclientMsg , 0 , sizeof(MSG_HEAD));
    memset(&rightinfo , 0 , sizeof(TL_SERVER_MSG));
    toclientMsg.nCommand = TL_STANDARD_ALARM_MSG_HEADER;
    toclientMsg.nID = userId;
    toclientMsg.nBufSize = sizeof(TL_SERVER_MSG);
    rightinfo.dwMsg=RightMsg;
    ret=TL_Video_Net_Send(&toclientMsg, (char *)&rightinfo);
    return ret;
}



///TL Video New Protocol Right Check
int TL_User_Right_Check_Func(int nCommand , int SocketID)
{
    BOOL    bRight=TRUE;
    PTL_RECORD_LOGIN_INFO info=NULL;
    pthread_mutex_lock(&g_tl_record_login_mutex);
    info=g_record_login_info;
    while(info)
    {
        if(info->LoginID==SocketID)
        {
            //printf("info->dwUserRight [0x%X]\n" , (int)info->dwUserRight);
            switch(nCommand)
            {
                case CMD_UPDATA_FLASH_FILE:
                    if(info->dwUserRight&0x00000004)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                case CMD_SEND_COMDATA:
                    if(info->dwUserRight&0x00000001)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                case CMD_RESTORE:
                case CMD_SETSYSTIME:
                case CMD_SET_OSDINFO:
                case CMD_SET_SHELTER:
                case CMD_SET_LOGO:
                case CMD_SET_STREAM:
                case CMD_SET_COLOR:
                case CMD_SET_MOTION_DETECT:
                case CMD_SET_PROBE_ALARM:
                case CMD_SET_VIDEO_LOST:
                case CMD_SET_COMINFO:
                case CMD_SET_USERINFO:
                case CMD_SET_NETWORK:
                case CMD_SET_FTPUPDATA_PARAM:
                case CMD_SET_COMINFO2:
                case CMD_SET_COMMODE:
                case CMD_SET_SENSOR_STATE:
                case CMS_SET_WIFI:
                case CMS_SET_TDSCDMA:
                case CMD_SET_EMAIL_PARAM:
                case CMD_SET_CARD_SNAP_REQUEST:
                case CMD_SET_MOBILE_CENTER:

                    if(info->dwUserRight&0x00000002)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        
                        bRight=FALSE;
                    }
                    break;
                default:
                    bRight=TRUE;
                    break;  
            }
        }
        info=info->pNext;   
    }
    if(!bRight)
    {
        TL_Video_Right_Check_Message(SocketID,nCommand);
    }
    pthread_mutex_unlock(&g_tl_record_login_mutex);
    return bRight;  
}



///TL Video New Protocol Open Channel Right Check
int TL_User_Open_Data_Channel_Check_Func(int nOpenChannel , int SocketID)
{
    BOOL    bRight=TRUE;
    PTL_RECORD_LOGIN_INFO info=NULL;
    pthread_mutex_lock(&g_tl_record_login_mutex);
    info=g_record_login_info;
    while(info)
    {
        if(info->LoginID==SocketID)
        {
            //printf("Open Channel [%d] info->dwNetPreviewRight [0x%x]\n" , nOpenChannel , info->dwNetPreviewRight);
            switch(nOpenChannel)
            {
                case 0:
                    if(info->dwNetPreviewRight&0x00000001)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                case 1:
                    if(info->dwNetPreviewRight&0x00000002)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                case 2:
                    if(info->dwNetPreviewRight&0x00000004)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                case 3:
                    if(info->dwNetPreviewRight&0x00000008)
                    {
                        bRight=TRUE;
                    }
                    else
                    {
                        bRight=FALSE;
                    }
                    break;
                default:
                    bRight=TRUE;
                    break;  
            }
        }
        info=info->pNext;   
    }
    if(!bRight)
    {
        TL_Video_Right_Check_Message(SocketID,TLERR_CHANNEL_NOT_OPEN);
    }
    pthread_mutex_unlock(&g_tl_record_login_mutex);
    return bRight;  
}


//参数pIp为xxx.xxx.xxx.xxx格式,失败返回0
unsigned long TL_IpValueFromString(const char *pIp)
{
    unsigned long value = 0;
    int ret = 0;
    unsigned long uiV1 = 0,uiV2 = 0,uiV3 = 0,uiV4 = 0;

    if(NULL == pIp)
    {
        return value;
    }
    ret = sscanf(pIp,"%ld.%ld.%ld.%ld",&uiV1,&uiV2,&uiV3,&uiV4);
    if(ret == 4)
    {
        value = (uiV4 << 24) | (uiV3 << 16) | (uiV2 << 8) | (uiV1 << 0);
    }
    return value;
}

///TL Video New Protocol Server Information Callback
int TL_Server_Info_func(char *pbuffer)
{
    QMAPI_NET_DEVICE_INFO stDeviceInfo;
    PTLNV_SERVER_INFO   pServerInfo;
//#ifdef TL_SUPPORT_WIRELESS
    //int ret;
    //int s_wlan=0, s_3g = 0;
//#endif
    if(pbuffer==NULL)
    {
        err();
        return -1;
    }
    pServerInfo=(PTLNV_SERVER_INFO)pbuffer; 
    
    pServerInfo->dwSize=sizeof(TLNV_SERVER_INFO);
    ///100001 NVS
    ///100002 NVD
    ///100003 NVR
    TLNV_SYSTEM_INFO sinfo;
	QMAPI_NET_NETWORK_CFG ninfo;
	QMAPI_NET_DEVICE_INFO dinfo;
	
    if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO)) != QMAPI_SUCCESS)
    {
		err();
        memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
        //return -1;
	}
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG)) != QMAPI_SUCCESS)
	{
		err();
        memset(&ninfo, 0, sizeof(QMAPI_NET_NETWORK_CFG));
        //return -1;
	}
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) != QMAPI_SUCCESS)
	{
		err();
        memset(&dinfo, 0, sizeof(QMAPI_NET_DEVICE_INFO));
        //return -1;
	}
    pServerInfo->dwServerFlag=IPC_FLAG;
    pServerInfo->dwServerIp=TL_IpValueFromString(ninfo.stEtherNet[0].strIPAddr.csIpV4);
    memcpy(pServerInfo->szServerIp , ninfo.stEtherNet[0].strIPAddr.csIpV4, 16);
    pServerInfo->wServerPort = ninfo.stEtherNet[0].wDataPort;

    pServerInfo->dwAlarmInCount  = sinfo.byAlarmInputNum;
    pServerInfo->dwAlarmOutCount = sinfo.byAlarmOutputNum;
    pServerInfo->wChannelNum     = sinfo.byChannelNum ? sinfo.byChannelNum : 1;


    pServerInfo->dwVersionNum = dinfo.dwSoftwareVersion;
    memset(pServerInfo->szServerName , 0 , 32);
    memcpy(pServerInfo->szServerName , dinfo.csDeviceName, 32);
    sprintf(pServerInfo->csP2pID, "%s", sinfo.byP2PID);
    sprintf(pServerInfo->szServerSerial, "%c%02X%02u-%02X%02X%02X%02X-%u%02u%lu",
            sinfo.byType, sinfo.byManufacture, sinfo.byChannelNum,
            sinfo.byHardware[0], sinfo.byHardware[1], sinfo.byHardware[2], sinfo.byHardware[3],
            (WORD)(sinfo.byIssueDate[0] << 8 | sinfo.byIssueDate[1]), (WORD)sinfo.byIssueDate[2], sinfo.dwSerialNumber);


    memcpy(pServerInfo->byMACAddr , sinfo.byMac, 6);
    memcpy(pServerInfo->csP2pID, sinfo.byP2PID, 32);
    pServerInfo->dwServerCPUType = CPUTYPE_Q3F;
    pServerInfo->dwSysFlags      = 0x0;
    pServerInfo->dwSysFlags     |= TL_SYS_FLAG_WIFI;

    memset(&stDeviceInfo, 0, sizeof(stDeviceInfo));
    memcpy(&stDeviceInfo, &dinfo,sizeof(QMAPI_NET_DEVICE_INFO));
    stDeviceInfo.dwSize = sizeof(QMAPI_NET_DEVICE_INFO);

    stDeviceInfo.byDateFormat      = 0;//xxxx-xx-xx
    stDeviceInfo.byDateSprtr       = 1;
    stDeviceInfo.byTimeFmt         = 0;
    stDeviceInfo.dwSoftwareVersion = dinfo.dwSoftwareVersion;

    stDeviceInfo.dwServerCPUType = CPUTYPE_Q3F;
    stDeviceInfo.dwSysFlags = 0;
	// need fixme yi.zhang
	stDeviceInfo.dwSysFlags &= ~(TL_SYS_FLAG_WIFI | TL_SYS_FLAG_3G);
    stDeviceInfo.dwServerType = IPC_FLAG;
    stDeviceInfo.byVideoInNum = sinfo.byChannelNum ? sinfo.byChannelNum : 1;
    stDeviceInfo.byAudioInNum = sinfo.byChannelNum ? sinfo.byChannelNum : 1;
    stDeviceInfo.byAlarmInNum = sinfo.byAlarmInputNum;
    stDeviceInfo.byAlarmOutNum = sinfo.byAlarmOutputNum;

    if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_DEVICECFG, 0, &stDeviceInfo, sizeof(QMAPI_NET_DEVICE_INFO)) != QMAPI_SUCCESS)
	{
		err();
        return -1;
	}
    return TL_SUCCESS;
}


///TL Video New Protocol Channel Information Callback
int TL_Channel_Info_func(char *pbuffer , int TL_Channel)
{
    PTLNV_CHANNEL_INFO  channelinfo=NULL;
    QMAPI_NET_CHANNEL_PIC_INFO pinfo;
    if(pbuffer==NULL)
    {
        err();
        return -1;  
    }
    if(TL_Channel<0||TL_Channel>QMAPI_MAX_CHANNUM)
    {
        err();
        return -1;  
    }
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, TL_Channel, &pinfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != QMAPI_SUCCESS)
	{
		err();
        return -1; 
	}
	
    channelinfo=(PTLNV_CHANNEL_INFO)pbuffer;
    channelinfo->dwSize=sizeof(TLNV_CHANNEL_INFO);
    channelinfo->dwStream1Height = pinfo.stRecordPara.wHeight;
    channelinfo->dwStream1Width = pinfo.stRecordPara.wWidth;;
	if (pinfo.stRecordPara.dwCompressionType == QMAPI_PT_H264)
		channelinfo->dwStream1CodecID = ENCODE_FORMAT_H264;// need fixme yi.zhang
    else if (pinfo.stRecordPara.dwCompressionType == QMAPI_PT_H265)
        channelinfo->dwStream1CodecID = ENCODE_FORMAT_H265;
	
    channelinfo->dwStream2Height = pinfo.stNetPara.wHeight;
    channelinfo->dwStream2Width  = pinfo.stNetPara.wWidth;
	if (pinfo.stNetPara.dwCompressionType == QMAPI_PT_H264)
	    channelinfo->dwStream2CodecID = ENCODE_FORMAT_H264;// need fixme yi.zhang
	else if (pinfo.stNetPara.dwCompressionType == QMAPI_PT_H265)
        channelinfo->dwStream2CodecID = ENCODE_FORMAT_H265;

    channelinfo->dwAudioChannels =  1;
    channelinfo->dwAudioBits =  16;
    channelinfo->dwAudioSamples =  8000;//8*1024
    channelinfo->dwAudioFormatTag =  WAVE_FORMAT_G711_ADPCM;//QMAPI_PT_G711A;  //WAVE_FORMAT_G722_ADPCM

	memcpy(channelinfo->csChannelName,"IPC 1",32);
#if 1
    printf("TLNV_CHANNEL_INFO dwSize %lu\n", channelinfo->dwSize);
    printf("TLNV_CHANNEL_INFO dwStream1Height %lu\n", channelinfo->dwStream1Height);
    printf("TLNV_CHANNEL_INFO dwStream1Width %lu\n", channelinfo->dwStream1Width);
    printf("TLNV_CHANNEL_INFO dwStream1CodecID %lu\n", channelinfo->dwStream1CodecID);  //MPEG4为0JPEG2000为1,H264为2
    printf("TLNV_CHANNEL_INFO dwStream2Height %lu\n", channelinfo->dwStream2Height);
    printf("TLNV_CHANNEL_INFO dwStream2Width %lu\n", channelinfo->dwStream2Width);
    printf("TLNV_CHANNEL_INFO dwStream2CodecID %lu\n", channelinfo->dwStream2CodecID);  //MPEG4为0JPEG2000为1,H264为2
    printf("TLNV_CHANNEL_INFO dwAudioChannels %lu\n", channelinfo->dwAudioChannels);
    printf("TLNV_CHANNEL_INFO dwAudioBits %lu\n", channelinfo->dwAudioBits);
    printf("TLNV_CHANNEL_INFO dwAudioSamples %lu\n", channelinfo->dwAudioSamples);
    printf("TLNV_CHANNEL_INFO dwAudioFormatTag %lu\n", channelinfo->dwAudioFormatTag);  //MP3为0x55G722为0x65
    printf("TLNV_CHANNEL_INFO csChannelName %s\n", channelinfo->csChannelName);
#endif
    return TL_SUCCESS;
}

///TL Video New Protocol Sensor Information Callback
int TL_Sensor_Info_func(char *pbuffer , int TL_Channel ,int SensorType)
{
    PTL_SENSOR_INFO     sensorinfo=NULL;
    if(pbuffer==NULL)
    {
        err();
        return -1;  
    }
    if (TL_Channel < 0 || TL_Channel >2)
    {
        err();
        return -1;  
    }   
    sensorinfo=(PTL_SENSOR_INFO)pbuffer;
    sensorinfo->dwSize=sizeof(TL_SENSOR_INFO);
    if(SensorType==0)///Alarm In
    {
      
    }
    if(SensorType==1)///Alarm In
    {
    }
    
    return TL_SUCCESS;
}

///TL Video Send Update Message 
int TL_Video_UpdateFlash_Message(int userId, int WriteSize , int UpdateMsg)
{
    int ret;
    TL_SERVER_MSG   updateInfo;
    MSG_HEAD        toclientMsg;
    memset(&toclientMsg , 0 , sizeof(MSG_HEAD));
    memset(&updateInfo , 0 , sizeof(TL_SERVER_MSG));

    toclientMsg.nCommand = TL_STANDARD_ALARM_MSG_HEADER;
    toclientMsg.nID = userId;
    toclientMsg.nBufSize = sizeof(TL_SERVER_MSG);
    updateInfo.dwMsg=UpdateMsg;

    if(UpdateMsg==TL_MSG_UPGRADE)
    {
        updateInfo.dwDataLen=WriteSize; 
    }
    else
    {
        updateInfo.dwDataLen=0; 
    }
    ret=TL_Video_Net_Send(&toclientMsg, (char *)&updateInfo);

    return ret;
}

/*
	修改网络配置，IP mask gw dns mac
	修改设备名，视频制式
	
	UPNP 修改暂时不实现
*/
int TL_Network_Change(char *pRecvBuf, PQMAPI_NET_NETWORK_CFG NetWork, QMAPI_NET_DEVICE_INFO *DevInfo)
{
	struct in_addr InAddr;
	PTL_SERVER_NETWORK ServerNetCfg = (PTL_SERVER_NETWORK)pRecvBuf;

	if (ServerNetCfg->dwSize!=sizeof(TL_SERVER_NETWORK))
    {
        err();
        return TLERR_INVALID_HANDLE;
    }
	if(ServerNetCfg->dwHttpPort < 80)
    {
    	err();
        return  TLERR_INVALID_HANDLE;
    }
    if(ServerNetCfg->dwDataPort < 1024)
    {
    	err();
        return  TLERR_INVALID_HANDLE;
    }
	if(ServerNetCfg->dwDataPort < 1024)
    {
        ServerNetCfg->dwDataPort = NetWork->stEtherNet[0].wDataPort;
    }
	InAddr.s_addr = ServerNetCfg->dwNetIpAddr;
	strcpy(NetWork->stEtherNet[0].strIPAddr.csIpV4, inet_ntoa(InAddr));
	
	InAddr.s_addr = ServerNetCfg->dwNetMask;
    strcpy(NetWork->stEtherNet[0].strIPMask.csIpV4, inet_ntoa(InAddr));
	
	InAddr.s_addr  =ServerNetCfg->dwGateway;
    strcpy(NetWork->stGatewayIpAddr.csIpV4, inet_ntoa(InAddr));
	
	InAddr.s_addr = ServerNetCfg->dwDNSServer;
    strcpy(NetWork->stDnsServer1IpAddr.csIpV4, inet_ntoa(InAddr));
	
	InAddr.s_addr = ServerNetCfg->dwTalkBackIp;
    strcpy(NetWork->stManagerIpAddr.csIpV4, inet_ntoa(InAddr));
	
	memcpy(NetWork->stEtherNet[0].byMACAddr, ServerNetCfg->szMacAddr, 6);
	NetWork->stEtherNet[0].wDataPort = (WORD)ServerNetCfg->dwDataPort;

    /* RM#2292: changed http port, restart web server. henry.li 2017/02/21 */
    if (NetWork->wHttpPort != (WORD)ServerNetCfg->dwHttpPort)
    {
        gHttpPortChanged = 1;
    }
    else
    {
        gHttpPortChanged = 0;
    }

    NetWork->wHttpPort               = (WORD)ServerNetCfg->dwHttpPort;
    NetWork->byEnableDHCP            = ServerNetCfg->bEnableDHCP;

	// 修改设备名字和视频制式
    DevInfo->byVideoStandard = ServerNetCfg->bVideoStandard;
    strcpy(DevInfo->csDeviceName,ServerNetCfg->szServerName);
    
	return TL_SUCCESS;
}

int TL_find_file(char * in_buf, int clientID)
{
    int i, nFillSize, FindFile, FindFileCount = 0;
    MSG_HEAD    toclientMsg;
    TLNV_FIND_FILE_RESP FindFileResp;
    QMAPI_NET_FILECOND Find;//
    char *pSendBuf, *SendBuf;
    QMAPI_NET_FINDDATA  lpFileData;
    TLNV_FILE_DATA_INFO *pFileData;
    TLNV_FIND_FILE_REQ *lpFindFileReq = (TLNV_FIND_FILE_REQ *)in_buf;
    
    if (in_buf == NULL)
    {
        err();
        return TLERR_INVALID_PARAM;
    }
    if (lpFindFileReq->dwSize != sizeof(TLNV_FIND_FILE_REQ))
    {
        err();
        return TLERR_INVALID_PARAM;
    }

    /* 查询文件 */
    Find.dwSize      = sizeof(QMAPI_NET_FILECOND);
    Find.dwChannel   = lpFindFileReq->nChannel;
    Find.dwFileType  = lpFindFileReq->nFileType;
    Find.dwIsLocked  = 0xff;//表示所有文件（包括锁定和未锁定） 
    memcpy(&Find.stStartTime, &lpFindFileReq->BeginTime, sizeof(NET_DVR_TIME));
    memcpy(&Find.stStopTime, &lpFindFileReq->EndTime, sizeof(NET_DVR_TIME));

    FindFile = QMAPI_Record_FindFile(0, &Find);
    if (FindFile == 0)
    {
        err();
    }
    else
    {
        FindFileCount = QMAPI_Record_FindFileNumber(0, FindFile);
    }
    if (FindFileCount >= MALLOC_BUFFER_SIZE/sizeof(QMAPI_NET_FINDDATA))
    {
        printf("there are too many files\n");
        FindFileResp.nResult = 2;
    }
    if (FindFileCount > 0)
    {
        printf("there are %d files\n", FindFileCount);   
        FindFileResp.nResult = 0;
    }
    if (FindFileCount == 0)
    {
        printf("can not find files\n"); 
        FindFileResp.nResult = 1;   //can not find
    }
    FindFileResp.nCount = FindFileCount;
    FindFileResp.dwSize = sizeof(TLNV_FIND_FILE_RESP);
    nFillSize = sizeof(TL_SERVER_MSG) + sizeof(TLNV_FIND_FILE_RESP)  +  FindFileCount * sizeof(TLNV_FILE_DATA_INFO);
    SendBuf   = malloc(nFillSize);
    if (SendBuf)
    {
        pSendBuf            = SendBuf;
        TL_SERVER_MSG *pMsg = (TL_SERVER_MSG*)pSendBuf;
        pMsg->dwMsg         = TL_MSG_FILE_NAME_DATA;
        pMsg->dwChannel     = lpFindFileReq->nChannel;
        pMsg->dwDataLen     = nFillSize;
        pSendBuf            +=  sizeof(TL_SERVER_MSG);

        memcpy(pSendBuf, &FindFileResp, sizeof(TLNV_FIND_FILE_RESP));
        pSendBuf       += sizeof(TLNV_FIND_FILE_RESP);

        for (i=0; i<FindFileCount; i++)
        {
            if (QMAPI_Record_FindNextFile(0, FindFile, &lpFileData) == QMAPI_SEARCH_FILE_SUCCESS)
            {
                pFileData = (TLNV_FILE_DATA_INFO*)pSendBuf; 
                strcpy(pFileData->sFileName, lpFileData.csFileName);
                memcpy(&pFileData->BeginTime, &lpFileData.stStartTime, sizeof(NET_DVR_TIME));
                memcpy(&pFileData->EndTime, &lpFileData.stStopTime, sizeof(NET_DVR_TIME));

                pFileData->nChannel  = lpFileData.nChannel;
                pFileData->nFileSize = lpFileData.dwFileSize;
                pFileData->nState    = 0;
                pSendBuf             += sizeof(TLNV_FILE_DATA_INFO);
            }
        }
        toclientMsg.nCommand   = TL_STANDARD_ALARM_MSG_HEADER;
        toclientMsg.nBufSize   = nFillSize;
        toclientMsg.nChannel   = lpFindFileReq->nChannel;
        toclientMsg.nErrorCode = 0;
        toclientMsg.nID        = clientID;
        TL_Video_Net_Send(&toclientMsg, SendBuf);
    }
    else
    {
        err();
    }
    QMAPI_Record_FindClose(0, FindFile);
    if (SendBuf)
        free(SendBuf);
    
    return TL_SUCCESS;
}
void *TL_Update_thread(void *arg)
{
	int userId = (int)arg;
	QMAPI_NET_UPGRADE_RESP upgrade;

	pthread_detach(pthread_self()); /* 线程结束时自动清除 */
	
	while (1)
	{
		if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_RESP, 0, &upgrade, sizeof(QMAPI_NET_UPGRADE_RESP)) == QMAPI_SUCCESS)
		{
			TL_SERVER_MSG	updateInfo;
			MSG_HEAD 	   toclientMsg;
			memset(&toclientMsg , 0 , sizeof(MSG_HEAD));
			memset(&updateInfo , 0 , sizeof(TL_SERVER_MSG));
			printf("########################## %d. %d. %d. \n",upgrade.state,upgrade.nResult,upgrade.progress);
			toclientMsg.nCommand = TL_STANDARD_ALARM_MSG_HEADER;
			toclientMsg.nID      = userId;
			toclientMsg.nBufSize = sizeof(TL_SERVER_MSG);
			updateInfo.dwMsg     = upgrade.progress==100 ? TL_MSG_UPGRADEOK : TL_MSG_UPGRADE;			
			updateInfo.dwDataLen = 0; 
			TL_Video_Net_Send(&toclientMsg, (char *)&updateInfo);
			if (upgrade.progress == 100)
			{
				break;
			}
		}
		else
		{
			break;
		}
		usleep(20*1000);
	}
	QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);

	return NULL;
}

typedef struct {
	DWORD	dwCRC;
	char	csFileName[64];
	DWORD	dwFileLen;
	DWORD	dwVer;
}TL_UPDATE_FILE_HEADER;


typedef struct {
	ULONG	nFileLength;		
	ULONG	nFileOffset;		
	ULONG	nPackNum;		
	ULONG	nPackNo;		
	ULONG	nPackSize;		
	TL_UPDATE_FILE_HEADER ufh;
	BOOL	bEndFile;			
	char	reserved[180];
}TL_UPDATE_FILE_INFO;


int TL_Update_Flash(int userId, int size, char *buffer)
{
    TL_UPDATE_FILE_INFO     *update_header=NULL;
    int ret = 0;
    if (size < sizeof(TL_UPDATE_FILE_INFO))
    {
        return TLERR_INVALID_PARAM; 
    }
    update_header = (TL_UPDATE_FILE_INFO *)buffer;

    if(size == sizeof(TL_UPDATE_FILE_INFO))
    {
        QMAPI_NET_UPGRADE_REQ stUpgradeRep;
        UPGRADE_ITEM_HEADER  stItemHeader;
        memset(&stUpgradeRep, 0x00, sizeof(QMAPI_NET_UPGRADE_REQ));
        memset(&stItemHeader, 0x00, sizeof(UPGRADE_ITEM_HEADER));
        stUpgradeRep.FileHdr.dwSize       = sizeof(QMAPI_UPGRADE_FILE_HEADER);
        stUpgradeRep.FileHdr.dwFlag       = QMAPI_UPGRADE_FILE_FLAG;
        stUpgradeRep.FileHdr.dwVersion    = update_header->ufh.dwVer;      //文件版本
        stUpgradeRep.FileHdr.dwItemCount  = 1;    //包内包含的文件总数
        stUpgradeRep.FileHdr.dwPackVer    = 0;      //打包的版本
        stUpgradeRep.FileHdr.dwCPUType    = 1006;
        stUpgradeRep.FileHdr.dwChannel    = 1; 
        stUpgradeRep.FileHdr.dwServerType = 0;
        strcpy(stUpgradeRep.FileHdr.csDescrip, update_header->ufh.csFileName) ;      //描述信息
        stItemHeader.dwCRC     = update_header->ufh.dwCRC;
        stItemHeader.dwDataLen = update_header->nFileLength;    //文件大小          
        stItemHeader.dwDataPos = 0;
    
		stItemHeader.dwDataType = QMAPI_ROM_FILETYPE_ROOTFS;
        stUpgradeRep.stItemHdr[0] = stItemHeader;
 
		ret = QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_REQ, 0, &stUpgradeRep, sizeof(QMAPI_NET_UPGRADE_REQ));

        //fixme , yi.zhang
        //g_serinfo.bServerExit = TRUE;
        extern int StopAllTcpNode();
        StopAllTcpNode();
		pthread_t thd;
		pthread_create(&thd, NULL, TL_Update_thread, (void *)userId);/* 创建升级进度获取线程 */
    }
    else
    {
	    QMAPI_NET_UPGRADE_DATA stUpgradeData;
	    stUpgradeData.dwSize       = sizeof(QMAPI_NET_UPGRADE_DATA);
	    stUpgradeData.dwFileLength = update_header->nFileLength;       //升级包总文件长度
	    stUpgradeData.dwPackNo     = update_header->nPackNo;           //分包序号，从0开始
	    stUpgradeData.dwPackSize   = update_header->nPackSize;         //分包大小
	    stUpgradeData.bEndFile     = update_header->bEndFile;           //是否是最后一个分包
	    stUpgradeData.pData        = (unsigned char *)(buffer+sizeof(TL_UPDATE_FILE_INFO));
	    ret = QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_DATA, 0, &stUpgradeData, sizeof(QMAPI_NET_UPGRADE_DATA));
    }
    
    return ret; 

}

#define PUB07_PEM "/root/qm_app/pub07.pem"

int TL_Check_Cipher(unsigned char *byCipherData, TLNV_SYSTEM_INFO *pInfo)
{
    int r = 0;
    int len;
    FILE *pf = NULL;
    RSA *prsa = NULL;

    do {
        pf = fopen(PUB07_PEM, "r");
        if (pf == NULL)
        {
            r = -1;
            break;
        }

        prsa = PEM_read_RSA_PUBKEY(pf, NULL, NULL, NULL); // 加载公钥
        if (prsa == NULL)
        {
            r = -2;
            break;
        }

        len = RSA_size(prsa);
        if (len > 0)
        {
            unsigned char data[len]; // 正常为256字节
            len = RSA_public_decrypt(len, byCipherData, data, prsa, RSA_NO_PADDING); // 解密
            if (len < 0)
            {
                r = -3;
                break;
            }

            unsigned short checksum, val = 0;
            TLNV_SYSTEM_INFO *pinfo = (TLNV_SYSTEM_INFO*)data;

            if (pinfo->cbSize != sizeof(TLNV_SYSTEM_INFO))
            {
                r = -4;
                break;
            }
            len = sizeof(TLNV_SYSTEM_INFO);
            checksum = pinfo->woCheckSum;
            pinfo->woCheckSum = pinfo->cbSize;
            while (len > 0)
            {
                val += data[--len];
            }
            if (checksum != val) // 数据校验
            {
                r = -5;
                break;
            }
            pinfo->woCheckSum = checksum;
            memcpy(pInfo, pinfo, sizeof(TLNV_SYSTEM_INFO));
        }
        else
        {
            r = -6;
        }
    } while(0);
    
    if (prsa)
    {
        RSA_free(prsa);
    }
    if (pf)
    {
        fclose(pf);
    }
    
    return r;
}

int TL_Set_Cipher(TLNV_ID_CIPHER *pCipher)
{
    int r;
    TLNV_SYSTEM_INFO sinfo;
    TLNV_SYSTEM_INFO sysinfo;

    if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO)) != QMAPI_SUCCESS)
    {
        printf("%s.%d\n",__FUNCTION__,__LINE__);
        memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
    }
    else
    {
        if (strlen((char *)sinfo.byP2PID) || sinfo.dwSerialNumber==0)
        {
            printf("%s.%d..pid:%s..Serial:%lu.\n",__FUNCTION__,__LINE__,sinfo.byP2PID,sinfo.dwSerialNumber);
            return TLERR_INVALID_COMMAND;
        }
    }

    if ((access(PUB07_PEM, F_OK)) ==-1)
    {
        printf("%s.%d\n",__FUNCTION__,__LINE__);
        return TLERR_INVALID_FILE;
    }

    if (pCipher->woSize!=sizeof(TLNV_ID_CIPHER) || pCipher->woDataSize > sizeof(pCipher->byCipherData))
    {
        printf("%s.%d. %u. %u.\n",__FUNCTION__,__LINE__,pCipher->woSize,pCipher->woDataSize);
        return TLERR_INVALID_PARAM;
    }

    r = TL_Check_Cipher(pCipher->byCipherData, &sinfo);
    if (r)
    {
        printf("%s.%d..%d\n",__FUNCTION__,__LINE__,r);
        return TLERR_ENCRYPT_IC_NO_MATCH;
    }

    if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO)) != QMAPI_SUCCESS)
    {
        return TLERR_ALLOC_FAILD;
    }

    if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sysinfo, sizeof(TLNV_SYSTEM_INFO)) != QMAPI_SUCCESS
        || strcmp((char *)sinfo.byP2PID, (char *)sysinfo.byP2PID) != 0
        || sinfo.dwSerialNumber != sysinfo.dwSerialNumber)
    {
        return TLERR_ENCRYPT_IC_NO_FIND;
    }
    
    return TL_SUCCESS;
}


void SendUnsupportMsg(MSG_HEAD *lpMsg)
{
    lpMsg->nBufSize = 0;
    lpMsg->nErrorCode = TLERR_UNSUPPORT;
    TL_Video_Net_Send(lpMsg,0);
}

static int getNameByPid(char * pidpath, char *task_name)
{
    char proc_pid_path[1024]={0};
    char buf[1024]={0};
    int ret;
    int pid;
    FILE* fp;

    fp = fopen(pidpath, "r");
    if (NULL != fp)
    {
        if( fgets(buf, 1024-1, fp)== NULL)
        {
            fclose(fp);
            return 0;
        }

        fclose(fp);
        ret = sscanf(buf, "%d", &pid);
        if (ret != 1)
            return 0;
	}
	
    snprintf(proc_pid_path,sizeof(proc_pid_path), "/proc/%d/status", pid);
    fp = fopen(proc_pid_path, "r");
    if(NULL != fp)
    {
        if(fgets(buf, 1024-1, fp) == NULL)
        {
            fclose(fp);
            return 0;
        }
        fclose(fp);
        ret = sscanf(buf, "%*s %s", task_name);
    }
    else 
        ret = 0;

    return ret;
}

int check_isdhcppid(char * pidpath)
{
    char tmp[128]={0};
    int res = 0;
    if (1 == getNameByPid(pidpath, tmp))
    {
        if (strncmp(tmp, "udhcpc", strlen("udhcpc")) == 0)
        {
            res = 1;	
        }
        else
        {
            res = 0;
            memset(tmp, 0, sizeof(tmp));
            snprintf(tmp, sizeof(tmp), "rm %s", pidpath);  //删除无效的PID文件
            //SystemCall_msg(tmp, SYSTEM_MSG_BLOCK_RUN);
        }
    }
    else
        res = 1;

    return res;
}

///TL Video Add New Protocol Main Task Callback from Client get Command
int ReceiveFun(MSG_HEAD *pMsgHead, char *pRecvBuf)
{
    static int upgrade_flage = 0;
    MSG_HEAD                        toclientMsg;
	//QMAPI_NET_UPGRADE_RESP upgrade;

    bzero(&toclientMsg, sizeof(toclientMsg));
    toclientMsg.nCommand = pMsgHead->nCommand;
    toclientMsg.nID = pMsgHead->nID;
    toclientMsg.nBufSize = 4;
    toclientMsg.nChannel = pMsgHead->nChannel;

    if(CMD_UPDATA_FLASH_FILE != pMsgHead->nCommand)
    	printf("############ 0x%x %s ############\n" , pMsgHead->nCommand , __TIME__);
	
    ///Right Check If This a User Right 
    if(!TL_User_Right_Check_Func(pMsgHead->nCommand , pMsgHead->nID))
    {
        printf("Not Right !\n");
        return 0;
    }
    
    if(upgrade_flage != 0
        && CMD_UPDATA_FLASH_FILE != pMsgHead->nCommand
        && QMAPI_NET_STA_UPGRADE_REQ != pMsgHead->nCommand
        && QMAPI_NET_STA_UPGRADE_DATA != pMsgHead->nCommand)
    {
        printf("%s-%d: it was upgrading system!\n",__FUNCTION__,__LINE__);
        return 0;
    }
	
    switch(pMsgHead->nCommand)
    {
    ///TL New Protocol Set System time
    case CMD_SETSYSTIME:
    {
		QMapi_sys_ioctrl(0, QMAPI_NET_STA_SET_SYSTIME, 0, pRecvBuf, sizeof(SYSTEMTIME));
        break;
    }
    ///TL New Protocol Reboot
    case CMD_REBOOT:
    {
        QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);
        break;
    }
    ///TL New Protocol Reset default configure
    case CMD_RESTORE:
    {
		QMAPI_NET_RESTORECFG res;

		res.dwSize = sizeof(QMAPI_NET_RESTORECFG);
		res.dwMask = 0;
		if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_RESTORECFG, 0, &res, sizeof(QMAPI_NET_RESTORECFG)) != QMAPI_SUCCESS)
		{
			err();
		}
        break;
    }
	// upgrade file
	case CMD_UPDATA_FLASH_FILE:
	{
		upgrade_flage = 1;
		TL_Update_Flash(pMsgHead->nID , pMsgHead->nBufSize , pRecvBuf);
		break;
	}
	case QMAPI_NET_STA_UPGRADE_REQ:
    {
        QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_REQ, 0, pRecvBuf, pMsgHead->nBufSize);
        break;
    }
    case QMAPI_NET_STA_UPGRADE_DATA:
    {
        QMAPI_NET_UPGRADE_DATA *pUpgData = (QMAPI_NET_UPGRADE_DATA *)pRecvBuf;
        pUpgData->pData = (unsigned char *)(pRecvBuf+sizeof(QMAPI_NET_UPGRADE_DATA));
        QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_DATA, 0, &pUpgData, sizeof(QMAPI_NET_UPGRADE_DATA));
        break;
    }
	case CMD_UPDATEFLASH:
	{
		toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG, 0, NULL, 0);
		toclientMsg.nBufSize = 0;
		break;
	}
    case CMD_SET_SERVER_RECORD:
    {
        QMAPI_NET_CHANNEL_RECORD rinfo;
        QMAPI_NET_DEVICE_INFO dinfo;
        TL_SERVER_RECORD_SET *record_set = (TL_SERVER_RECORD_SET *)pRecvBuf;
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_RECORDCFG, 0, &rinfo, sizeof(QMAPI_NET_CHANNEL_RECORD)) == QMAPI_SUCCESS)
        {
            rinfo.dwRecord = record_set->dwLocalRecordMode;
            QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_RECORDCFG, 0, &rinfo, sizeof(QMAPI_NET_CHANNEL_RECORD));
        }
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) == QMAPI_SUCCESS)
        {
            dinfo.byRecycleRecord = record_set->bAutoDeleteFile;
            QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO));
        }
        break;
    }
   
    ///TL Video Get Return Parameter Define
    case CMD_GETSYSTIME:
    {
        QMAPI_SYSTEMTIME stSystemTime;

        if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &stSystemTime, sizeof(QMAPI_SYSTEMTIME)) == QMAPI_SUCCESS)
        {
            toclientMsg.nCommand = CMD_GETSYSTIME;
            toclientMsg.nBufSize = sizeof(QMAPI_SYSTEMTIME);
            TL_Video_Net_Send(&toclientMsg, (Char *)&stSystemTime);
        }
        break;
    }
    ///TL Video Get Return NVS Network Set
    case CMD_GET_NETWORK:
    {
        TL_SERVER_NETWORK network_par;
        QMAPI_NET_NETWORK_CFG ninfo;
        QMAPI_NET_DEVICE_INFO dinfo;
        ///copy network parameter
        memset(&network_par, 0, sizeof(network_par));
        network_par.dwSize = sizeof(network_par);
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG)) == QMAPI_SUCCESS
            && QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) == QMAPI_SUCCESS)
        {
            network_par.dwNetIpAddr   = TL_IpValueFromString(ninfo.stEtherNet[0].strIPAddr.csIpV4);
            network_par.dwNetMask     = TL_IpValueFromString(ninfo.stEtherNet[0].strIPMask.csIpV4);
            network_par.dwGateway     = TL_IpValueFromString(ninfo.stGatewayIpAddr.csIpV4);
            network_par.bEnableDHCP   = ninfo.byEnableDHCP;
            network_par.bEnableAutoDNS = ninfo.byEnableDHCP;
            network_par.bVideoStandard = dinfo.byVideoStandard;
            network_par.dwHttpPort     = ninfo.wHttpPort;
            network_par.dwDataPort     = ninfo.stEtherNet[0].wDataPort;
            network_par.dwDNSServer    = TL_IpValueFromString(ninfo.stDnsServer1IpAddr.csIpV4);
            network_par.dwTalkBackIp   = TL_IpValueFromString(ninfo.stManagerIpAddr.csIpV4);
            memcpy(network_par.szMacAddr, ninfo.stEtherNet[0].byMACAddr, 6);
            strcpy(network_par.szServerName,dinfo.csDeviceName);
            toclientMsg.nCommand = CMD_GET_NETWORK;
            toclientMsg.nBufSize = sizeof(TL_SERVER_NETWORK);
            TL_Video_Net_Send(&toclientMsg, (Char *)&network_par);
        }
        break;
    }
	///TL New Protocol Set Network
    case CMD_SET_NETWORK:
    {
        QMAPI_NET_NETWORK_CFG ninfo;
        QMAPI_NET_DEVICE_INFO dinfo;

        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG)) == QMAPI_SUCCESS
            	&& QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) == QMAPI_SUCCESS)
        {
            if (TL_Network_Change(pRecvBuf, &ninfo, &dinfo) == TL_SUCCESS)
            {
                toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO));
                toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG));

                /* RM#2292: changed http port, restart web server. henry.li 2017/02/21 */
                if (gHttpPortChanged == 1)
                {
                    QM_Event_Send("http_port_changed", (void *)(ninfo.wHttpPort), sizeof(ninfo.wHttpPort));
                }
            }
        }
        break;
    }
	case CMD_NET_GET_DEVICECFG:
	{
        QMAPI_NET_DEVICE_INFO dinfo;

        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) == QMAPI_SUCCESS)
        {
            toclientMsg.nCommand = CMD_NET_GET_DEVICECFG;      
            toclientMsg.nBufSize = sizeof(QMAPI_NET_DEVICE_INFO);
            TL_Video_Net_Send(&toclientMsg, (Char *)&dinfo);
        }
        break;
	}
	case CMD_NET_SET_DEVICECFG:
	{
		toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_DEVICECFG, 0, &pRecvBuf, sizeof(QMAPI_NET_DEVICE_INFO));
		break;
	}
    ///TL Video Get Return Video Main Setting
    case CMD_GET_STREAM:
    {
        int iCh = pMsgHead->nChannel;
        TL_CHANNEL_CONFIG chinnelCfg = {0};
        QMAPI_NET_CHANNEL_PIC_INFO stPicInfo;

        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, pMsgHead->nChannel, &stPicInfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) == QMAPI_SUCCESS)
        {
            toclientMsg.nCommand  = CMD_GET_STREAM;      
            toclientMsg.nBufSize  = sizeof(TL_CHANNEL_CONFIG);

            chinnelCfg.bEncodeAudio  = stPicInfo.stEventRecordPara.wEncodeAudio;
            strcpy(chinnelCfg.csChannelName, (char *)stPicInfo.csChannelName);
            chinnelCfg.dwBitRate       = stPicInfo.stRecordPara.dwBitRate;
            chinnelCfg.dwChannel       = iCh;
            chinnelCfg.dwImageQuality  = stPicInfo.stRecordPara.dwImageQuality;
            chinnelCfg.dwRateType      = stPicInfo.stRecordPara.dwRateType;
            chinnelCfg.dwSize          = sizeof(TL_CHANNEL_CONFIG);
            chinnelCfg.dwStreamFormat  = stPicInfo.stRecordPara.dwStreamFormat;
            chinnelCfg.nFrameRate      = stPicInfo.stRecordPara.dwFrameRate;
            chinnelCfg.nMaxKeyInterval = stPicInfo.stRecordPara.dwMaxKeyInterval;
            chinnelCfg.nStandard       = 1;//need fixme yi.zhang

            TL_Video_Net_Send(&toclientMsg, (Char *)&chinnelCfg);
        }
        break;
    }
    case CMD_SET_MOTION_DETECT:
    {
        int ret;
        int day, i;
        int nChannel;
        TL_CHANNEL_MOTION_DETECT *pstMotionDetectCfg = (TL_CHANNEL_MOTION_DETECT *)pRecvBuf;
        QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetectCfg;

        printf("#### %s %d, CMD_SET_MOTION_DETECT\n", __FUNCTION__, __LINE__);
        nChannel = pstMotionDetectCfg->dwChannel;

        ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, pstMotionDetectCfg->dwChannel, &stMotionDetectCfg, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
        if (!ret)
        {
            stMotionDetectCfg.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
            stMotionDetectCfg.dwChannel = pstMotionDetectCfg->dwChannel;
            stMotionDetectCfg.bEnable = pstMotionDetectCfg->bEnable;
            //printf("#### %s %d, stMotionDetectCfg.bEnable = [%d]\n", __FUNCTION__, __LINE__, stMotionDetectCfg.bEnable);
            stMotionDetectCfg.dwSensitive = pstMotionDetectCfg->dwSensitive;

            for(i = 0;i<44*36;i++)
            {
                if(pstMotionDetectCfg->pbMotionArea[i])
                {
                    SET_BIT(stMotionDetectCfg.byMotionArea,i);
                }
                else
                {
                    CLR_BIT(stMotionDetectCfg.byMotionArea,i);
                }
            }

            stMotionDetectCfg.stHandle.wDuration = pstMotionDetectCfg->nDuration;

            if(pstMotionDetectCfg->bShotSnap)
            {
                SET_BIT(&stMotionDetectCfg.stHandle.bySnap[nChannel%8], nChannel);
                stMotionDetectCfg.stHandle.wSnapNum = 1;
                stMotionDetectCfg.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOSNAP;
            }
            else
            {
                CLR_BIT(&stMotionDetectCfg.stHandle.bySnap[nChannel%8], nChannel);
                stMotionDetectCfg.stHandle.wSnapNum = 0;
                stMotionDetectCfg.stHandle.wActionFlag &= ~QMAPI_ALARM_EXCEPTION_TOSNAP;
            }

            for(i = 0; i < 4; i++)
            {
                if(pstMotionDetectCfg->bAlarmIOOut[i])
                {
                    SET_BIT(&stMotionDetectCfg.stHandle.byRelAlarmOut[0], i);
                    stMotionDetectCfg.stHandle.wActionFlag |= QMAPI_ALARM_EXCEPTION_TOALARMOUT;
                }
                else
                {
                    CLR_BIT(&stMotionDetectCfg.stHandle.byRelAlarmOut[0], i);
                    stMotionDetectCfg.stHandle.wActionFlag &= ~QMAPI_ALARM_EXCEPTION_TOALARMOUT;
                }
            }

            for(day = 0; day < 8; day++)
            {
                if(pstMotionDetectCfg->tlScheduleTime[day].bEnable)
                {
                    stMotionDetectCfg.bEnable = 1;
                }

                stMotionDetectCfg.stScheduleTime[day][0].bEnable = pstMotionDetectCfg->tlScheduleTime[day].bEnable;
                stMotionDetectCfg.stScheduleTime[day][0].byStartHour = pstMotionDetectCfg->tlScheduleTime[day].BeginTime1.bHour;
                stMotionDetectCfg.stScheduleTime[day][0].byStartMin = pstMotionDetectCfg->tlScheduleTime[day].BeginTime1.bMinute;
                stMotionDetectCfg.stScheduleTime[day][0].byStopHour = pstMotionDetectCfg->tlScheduleTime[day].EndTime1.bHour;
                stMotionDetectCfg.stScheduleTime[day][0].byStopMin = pstMotionDetectCfg->tlScheduleTime[day].EndTime1.bMinute;

                stMotionDetectCfg.stScheduleTime[day][1].bEnable = pstMotionDetectCfg->tlScheduleTime[day].bEnable;
                stMotionDetectCfg.stScheduleTime[day][1].byStartHour = pstMotionDetectCfg->tlScheduleTime[day].BeginTime2.bHour;
                stMotionDetectCfg.stScheduleTime[day][1].byStartMin = pstMotionDetectCfg->tlScheduleTime[day].BeginTime2.bMinute;
                stMotionDetectCfg.stScheduleTime[day][1].byStopHour = pstMotionDetectCfg->tlScheduleTime[day].EndTime2.bHour;
                stMotionDetectCfg.stScheduleTime[day][1].byStopMin = pstMotionDetectCfg->tlScheduleTime[day].EndTime2.bMinute;
            }
#if 0
            if(pstMotionDetectCfg->tlScheduleTime[7].bEnable)
            {
                stMotionDetectCfg.bEnable = 1;
                for(day = 0; day < 7; day++)
                {
                    stMotionDetectCfg.stScheduleTime[day][0].bEnable = pstMotionDetectCfg->tlScheduleTime[7].bEnable;
                    stMotionDetectCfg.stScheduleTime[day][0].byStartHour = pstMotionDetectCfg->tlScheduleTime[7].BeginTime1.bHour;
                    stMotionDetectCfg.stScheduleTime[day][0].byStartMin = pstMotionDetectCfg->tlScheduleTime[7].BeginTime1.bMinute;
                    stMotionDetectCfg.stScheduleTime[day][0].byStopHour = pstMotionDetectCfg->tlScheduleTime[7].EndTime1.bHour;
                    stMotionDetectCfg.stScheduleTime[day][0].byStopMin = pstMotionDetectCfg->tlScheduleTime[7].EndTime1.bMinute;

                    stMotionDetectCfg.stScheduleTime[day][1].bEnable = pstMotionDetectCfg->tlScheduleTime[7].bEnable;
                    stMotionDetectCfg.stScheduleTime[day][1].byStartHour = pstMotionDetectCfg->tlScheduleTime[7].BeginTime2.bHour;
                    stMotionDetectCfg.stScheduleTime[day][1].byStartMin = pstMotionDetectCfg->tlScheduleTime[7].BeginTime2.bMinute;
                    stMotionDetectCfg.stScheduleTime[day][1].byStopHour = pstMotionDetectCfg->tlScheduleTime[7].EndTime2.bHour;
                    stMotionDetectCfg.stScheduleTime[day][1].byStopMin = pstMotionDetectCfg->tlScheduleTime[7].EndTime2.bMinute;
                }
            }
#endif

            ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_MOTIONCFG, pstMotionDetectCfg->dwChannel, &stMotionDetectCfg, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
        }

        if (ret)
        {
            toclientMsg.nErrorCode = ret;
            toclientMsg.nBufSize = 0;
            TL_Video_Net_Send(&toclientMsg, NULL);
        }
        break;
    }
    ///TL Video Get Return Motion Alarm Setting
    case CMD_GET_MOTION_DETECT:
    {
        int i, day;
        QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetectCfg;
        TL_CHANNEL_MOTION_DETECT stTLMotionDetectCfg;

        printf("#### %s %d, CMD_GET_MOTION_DETECT\n", __FUNCTION__, __LINE__);
        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, 0, &stMotionDetectCfg, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));

        {
            stTLMotionDetectCfg.dwSize = sizeof(TL_CHANNEL_MOTION_DETECT);
            stTLMotionDetectCfg.dwChannel = stMotionDetectCfg.dwChannel;
            stTLMotionDetectCfg.bEnable = stMotionDetectCfg.bEnable;
            //printf("stTLMotionDetectCfg.bEnable = [%d]\n", stTLMotionDetectCfg.bEnable);
            stTLMotionDetectCfg.nDuration = stMotionDetectCfg.stHandle.wDuration;
            stTLMotionDetectCfg.bShotSnap = stMotionDetectCfg.stHandle.bySnap[0];
            stTLMotionDetectCfg.dwSensitive = stMotionDetectCfg.dwSensitive;

            if (stMotionDetectCfg.stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOALARMOUT)
            {
                for (i=0; i<4; i++)
                {
                    stTLMotionDetectCfg.bAlarmIOOut[i] = CHK_BIT(&stMotionDetectCfg.stHandle.byRelAlarmOut[i/8],i);
                }
            }
            else
            {
                for (i=0; i<4; i++)
                {
                    stTLMotionDetectCfg.bAlarmIOOut[i] = 0;
                }
            }

            if (stMotionDetectCfg.stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOSNAP)
            {
                stTLMotionDetectCfg.bShotSnap = 1;
            }
            else
            {
                stTLMotionDetectCfg.bShotSnap = 0;
            }

            for(i = 0;i<44*36;i++)
            {
                if(CHK_BIT(stMotionDetectCfg.byMotionArea,i))
                {
                    stTLMotionDetectCfg.pbMotionArea[i] = 1;
                }
                else
                {
                    stTLMotionDetectCfg.pbMotionArea[i] = 0;
                }
            }

            for(day = 0; day < 8; day++)
            {
                TL_SCHED_TIME   *pstScheduleTime = &stTLMotionDetectCfg.tlScheduleTime[day];

                pstScheduleTime->bEnable = stMotionDetectCfg.stScheduleTime[day][0].bEnable;
                //printf("day[%d] pstScheduleTime enable = [%d]\n", day, pstScheduleTime->bEnable);

                pstScheduleTime->BeginTime1.bHour = stMotionDetectCfg.stScheduleTime[day][0].byStartHour;
                pstScheduleTime->BeginTime1.bMinute = stMotionDetectCfg.stScheduleTime[day][0].byStartMin;
                pstScheduleTime->EndTime1.bHour = stMotionDetectCfg.stScheduleTime[day][0].byStopHour;
                pstScheduleTime->EndTime1.bMinute = stMotionDetectCfg.stScheduleTime[day][0].byStopMin;

                pstScheduleTime->BeginTime2.bHour = stMotionDetectCfg.stScheduleTime[day][1].byStartHour;
                pstScheduleTime->BeginTime2.bMinute = stMotionDetectCfg.stScheduleTime[day][1].byStartMin;
                pstScheduleTime->EndTime2.bHour = stMotionDetectCfg.stScheduleTime[day][1].byStopHour;
                pstScheduleTime->EndTime2.bMinute = stMotionDetectCfg.stScheduleTime[day][1].byStopMin;
            }

            toclientMsg.nCommand = CMD_GET_MOTION_DETECT;
            toclientMsg.nBufSize = sizeof(TL_CHANNEL_MOTION_DETECT);
            //printf("At end: stTLMotionDetectCfg.bEnable = [%d]\n", stTLMotionDetectCfg.bEnable);
            TL_Video_Net_Send(&toclientMsg, (Char *)&stTLMotionDetectCfg);
        }
        break;
    }
    case CMD_SET_OSDINFO:
    {
        int ret;
        TL_CHANNEL_OSDINFO *pstChannelOsd = (TL_CHANNEL_OSDINFO *)pRecvBuf;

        printf("####  %s %d, dwSize=%lu, channel=%lu, showtime:%d, dwTimeFormat=%lu, bShowString=%d, x:y=%lu:%lu, string:%s, name:%s\n",
                __FUNCTION__, __LINE__, pstChannelOsd->dwSize, pstChannelOsd->dwChannel,
                pstChannelOsd->bShowTime, pstChannelOsd->dwTimeFormat, pstChannelOsd->bShowString,
                pstChannelOsd->dwStringx, pstChannelOsd->dwStringy, pstChannelOsd->csString, pstChannelOsd->csOsdFontName);
        
        QMAPI_NET_CHANNEL_OSDINFO stChnOsd;
        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_OSDCFG, 0, &stChnOsd, sizeof(QMAPI_NET_CHANNEL_OSDINFO));
        stChnOsd.bShowTime = pstChannelOsd->bShowTime;
        stChnOsd.byReserve1 = pstChannelOsd->dwTimeFormat;
        stChnOsd.byShowChanName = pstChannelOsd->bShowString;
        stChnOsd.dwShowNameTopLeftX = pstChannelOsd->dwStringx;
        stChnOsd.dwShowNameTopLeftY = pstChannelOsd->dwStringy;
        memcpy(stChnOsd.csChannelName, pstChannelOsd->csString, QMAPI_NAME_LEN);
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
		if ((ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_OSDCFG, 0, &stChnOsd, sizeof(QMAPI_NET_CHANNEL_OSDINFO))))
        {
    		toclientMsg.nErrorCode = ret;
    		toclientMsg.nBufSize = 0;
    		TL_Video_Net_Send(&toclientMsg, NULL);
		}

        break;
    }
    case CMD_GET_FILELIST:
    {
        TL_find_file(pRecvBuf, toclientMsg.nID);
        break;
    }
	case CMD_GET_OSDINFO:
    {
        TL_CHANNEL_OSDINFO stTLChnOsd;
        QMAPI_NET_CHANNEL_OSDINFO stChnOsd;
        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_OSDCFG, 0, &stChnOsd, sizeof(QMAPI_NET_CHANNEL_OSDINFO));

        stTLChnOsd.dwSize = sizeof(TL_CHANNEL_OSDINFO);
        stTLChnOsd.dwChannel = stChnOsd.dwChannel;
        stTLChnOsd.bShowTime = stChnOsd.bShowTime;
        stTLChnOsd.dwTimeFormat = stChnOsd.byReserve1;
        stTLChnOsd.bShowString = stChnOsd.byShowChanName;
        stTLChnOsd.dwStringx = stChnOsd.dwShowNameTopLeftX;
        stTLChnOsd.dwStringy = stChnOsd.dwShowNameTopLeftY;

        memcpy(stTLChnOsd.csString, stChnOsd.csChannelName, QMAPI_NAME_LEN);

        toclientMsg.nCommand = CMD_GET_OSDINFO;
        toclientMsg.nBufSize = sizeof(TL_CHANNEL_OSDINFO);
        TL_Video_Net_Send(&toclientMsg, (Char *)&stTLChnOsd);
        break;
    }
    case CMS_GET_WIFI://CMD_NET_GET_WIFICFG:
    {
        TL_WIFI_CONFIG stTlWifiConfig;
        QMAPI_NET_WIFI_CONFIG stWifiConfig;
        QMAPI_Wireless_IOCtrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));

        memset(&stTlWifiConfig, 0, sizeof(TL_WIFI_CONFIG));
        stTlWifiConfig.dwSize = sizeof(TL_WIFI_CONFIG);

        stTlWifiConfig.bWifiEnable = stWifiConfig.bWifiEnable;

        if (stWifiConfig.bWifiEnable == 1)
        {
            stTlWifiConfig.byWifiMode= stWifiConfig.byWifiMode;
            stTlWifiConfig.byWpsStatus = stWifiConfig.byWpsStatus;
            stTlWifiConfig.byWifiSetFlag = stWifiConfig.byWifiSetFlag;
            
            stTlWifiConfig.dwNetIpAddr = inet_addr(stWifiConfig.dwNetIpAddr.csIpV4);
            stTlWifiConfig.dwNetMask = inet_addr(stWifiConfig.dwNetMask.csIpV4);
            stTlWifiConfig.dwGateway = inet_addr(stWifiConfig.dwGateway.csIpV4);
            stTlWifiConfig.dwDNSServer = inet_addr(stWifiConfig.dwDNSServer.csIpV4);
            
            strcpy(stTlWifiConfig.szEssid, stWifiConfig.csEssid);
            strcpy(stTlWifiConfig.szWebKey, stWifiConfig.csWebKey);
            stTlWifiConfig.nSecurity = stWifiConfig.nSecurity;
            stTlWifiConfig.byNetworkType = stWifiConfig.byNetworkType;
            stTlWifiConfig.bWifiEnable = (1 == stWifiConfig.byEnableDHCP) ? 2 : stWifiConfig.bWifiEnable;
        }

        toclientMsg.nCommand = CMS_GET_WIFI;
        toclientMsg.nBufSize = sizeof(QMAPI_NET_WIFI_CONFIG);
        TL_Video_Net_Send(&toclientMsg, (Char *)&stTlWifiConfig);
        break;
    }
    case CMS_SET_WIFI://CMD_NET_SET_WIFICFG:
    {
        TL_WIFI_CONFIG *pstTlWifiConfig = (TL_WIFI_CONFIG *)pRecvBuf;
        struct in_addr inp;

        QMAPI_NET_WIFI_CONFIG stWifiConfig;
        QMAPI_Wireless_IOCtrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));
        stWifiConfig.bWifiEnable = (0 == pstTlWifiConfig->bWifiEnable) ? 0 : 1;
        stWifiConfig.byWifiMode= pstTlWifiConfig->byWifiMode;
        stWifiConfig.byWpsStatus = pstTlWifiConfig->byWpsStatus;
        stWifiConfig.byWifiSetFlag = pstTlWifiConfig->byWifiSetFlag;

        inp.s_addr = pstTlWifiConfig->dwNetIpAddr;
        strcpy(stWifiConfig.dwNetIpAddr.csIpV4, inet_ntoa(inp));
        inp.s_addr = pstTlWifiConfig->dwNetMask;
        strcpy(stWifiConfig.dwNetMask.csIpV4, inet_ntoa(inp));
        inp.s_addr = pstTlWifiConfig->dwGateway;
        strcpy(stWifiConfig.dwGateway.csIpV4, inet_ntoa(inp));
        inp.s_addr = pstTlWifiConfig->dwDNSServer;
        strcpy(stWifiConfig.dwDNSServer.csIpV4, inet_ntoa(inp));

        strcpy(stWifiConfig.csEssid, pstTlWifiConfig->szEssid);
        strcpy(stWifiConfig.csWebKey, pstTlWifiConfig->szWebKey);
        stWifiConfig.nSecurity = pstTlWifiConfig->nSecurity;
        stWifiConfig.byNetworkType = pstTlWifiConfig->byNetworkType;
        stWifiConfig.byEnableDHCP = (pstTlWifiConfig->bWifiEnable == 2) ? 1 : 0;

        printf("--\n TlSession SET WiFi: WifiEnable=[%d]; webKey=[%s]; security=[%d]\n",
                stWifiConfig.bWifiEnable, stWifiConfig.csWebKey, stWifiConfig.nSecurity);

        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_WIFICFG, 0, &stWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));

		toclientMsg.nErrorCode = 0;
		toclientMsg.nBufSize = 0;
		TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }
    case CMD_NET_GET_WIFI_SITE_LIST:
    {
        QMAPI_NET_WIFI_SITE_LIST stWifiSiteList;
        QMAPI_Wireless_GetSiteList(&stWifiSiteList);

        toclientMsg.nCommand = CMD_NET_GET_WIFI_SITE_LIST;
        toclientMsg.nBufSize = sizeof(QMAPI_NET_WIFI_SITE_LIST);
        TL_Video_Net_Send(&toclientMsg, (Char *)&stWifiSiteList);
        break;
    }
	case CMD_GET_USERINFO:
	    /* RM#3189: unsupport get/set userInfo.  henry.li   2017/04/06 */
        SendUnsupportMsg(&toclientMsg);
	    break;
    case CMD_SNAPSHOT:
    case CMD_SET_CARD_SNAP_REQUEST:
    case CMD_SET_SHELTER:
    {
        int i;
        int ret;
#if 0
        struct v_roi_info roi;
        struct v_roi_info roi_tmp;
        TL_CHANNEL_SHELTER *pShelter = (TL_CHANNEL_SHELTER *)pRecvBuf;
        printf("%s dwSize:%ld dwChannel:%ld bEnable:%ld \n", __FUNCTION__, pShelter->dwSize, pShelter->dwChannel, pShelter->bEnable);
        for ( i = 0; i<2; i++)
        {
            printf("rcShelter[%d]:left:%ld right:%ld top:%ld bottom:%ld\n", 
                    i, pShelter->rcShelter[i].left, pShelter->rcShelter[i].right, pShelter->rcShelter[i].top, pShelter->rcShelter[i].bottom);
        }

        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ROICFG, 0, pRecvBuf, sizeof(TL_CHANNEL_SHELTER));
#endif

    TL_CHANNEL_SHELTER *pShelter = (TL_CHANNEL_SHELTER *)pRecvBuf;
        printf("%s dwSize:%ld dwChannel:%ld bEnable:%ld \n", __FUNCTION__, pShelter->dwSize, pShelter->dwChannel, pShelter->bEnable);
        for ( i = 0; i<2; i++)
        {
            printf("rcShelter[%d]:left:%ld right:%ld top:%ld bottom:%ld\n", 
                    i, pShelter->rcShelter[i].left, pShelter->rcShelter[i].right, pShelter->rcShelter[i].top, pShelter->rcShelter[i].bottom);
        }

        QMAPI_NET_CHANNEL_SHELTER shelter;
        shelter.bEnable     = pShelter->bEnable;
        shelter.dwSize      = sizeof(QMAPI_NET_CHANNEL_SHELTER);
        shelter.dwChannel   = pShelter->dwChannel;
		
		/*workaround for web wrong setting*/
		if(shelter.bEnable > 1)
			return 0;
		
        for(i = 0; i <QMAPI_MAX_VIDEO_HIDE_RECT; i++ )
        {
            shelter.strcShelter[i].nType    = 5;
            shelter.strcShelter[i].dwColor  = 0 ;
            shelter.strcShelter[i].wLeft    = pShelter->rcShelter[i].left;
            shelter.strcShelter[i].wTop     = pShelter->rcShelter[i].top;
            shelter.strcShelter[i].wWidth   = pShelter->rcShelter[i].right - pShelter->rcShelter[i].left;
            shelter.strcShelter[i].wHeight  = pShelter->rcShelter[i].bottom - pShelter->rcShelter[i].top;
        }

        QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SHELTERCFG, 0, &shelter, sizeof(QMAPI_NET_CHANNEL_SHELTER));
        break;
    }
    case CMD_SET_STREAM:
    case CMD_SET_COMINFO:
    case CMD_SET_PROBE_ALARM:
    case CMD_SET_COMINFO2:
    case CMD_SEND_COMDATA:
	case CMD_GET_COMDATA:
    case CMD_SET_COMMODE:
    case CMD_SET_VIDEO_LOST:
    case CMD_SET_USERINFO:
    case CMD_UPLOADPTZPROTOCOL:
    case CMD_SET_ALARM_OUT:
    case CMD_SET_NOTIFY_SERVER:
    case CMD_SET_PPPOE_DDNS:
    case QMAPI_SYSCFG_SET_DDNSCFG:
    case CMD_SET_SENSOR_STATE:
    case CMD_SET_CENTER_INFO:
    case QMAPI_SYSCFG_SET_WIFI_WPS_START:
    case CMS_SET_TDSCDMA:
    case CMS_GET_TDSCDMA:
    case CMD_RECORD_BEGIN:
    case CMD_RECORD_STOP:
    case CMD_GET_PROBE_ALARM:
    case CMD_GET_VIDEO_LOST:
    case CMD_GET_COMINFO:
    case CMD_GET_COMINFO2:
    case CMD_GET_COMMODE:
    case CMD_GET_PTZDATA:
    case CMD_GET_ALARM_OUT:
    case CMD_GET_SENSOR_STATE:
    case CMD_GET_PPPOE_DDNS:
    case CMD_GET_CENTER_INFO:
    case CMD_GET_SERVER_STATUS:
    case CMD_GET_CENTER_LICENCE:
    case CMD_UPDATE_CENTER_LICENCE:
    case CMD_GET_SERVER_RECORD_SET:
    case CMD_GET_SERVER_RECORD_CONFIG:
    case CMD_SET_SERVER_RECORD_CONFIG:
    case CMD_GET_SERVER_RECORD_STATE:
    case CMD_UNLOAD_DISK:
    case CMD_GET_NOTIFY_SERVER:
    case CMD_SET_UPNP:
    case CMD_SET_PTZ_CONTROL:
    {
        toclientMsg.nCommand = pMsgHead->nCommand;
        toclientMsg.nBufSize = 0;
        toclientMsg.nErrorCode = TLERR_UNSUPPORT;
        printf("#### %s %d, unsupport cmd :0x%X\n", __FUNCTION__, __LINE__, pMsgHead->nCommand);
        TL_Video_Net_Send(&toclientMsg,0);
        break;
    }
    case CMD_SET_FTPUPDATA_PARAM:
    {
        printf("\n==========LiQ test: CMD_SET_FTPUPDATA_PARAM\n");
        TL_FTPUPDATA_PARAM *pFtpUpDataParam = (TL_FTPUPDATA_PARAM *)pRecvBuf;
        QMAPI_NET_FTP_PARAM finfo;

        printf("EnableFTP=%lu, IP=%s, Port=%lu, user=%s, pwd=%s\n",
                pFtpUpDataParam->dwEnableFTP, 
                pFtpUpDataParam->csFTPIpAddress, 
                pFtpUpDataParam->dwFTPPort,
                pFtpUpDataParam->sUserName,
                pFtpUpDataParam->sPassword);

        memset(&finfo, 0, sizeof(QMAPI_NET_FTP_PARAM));
        finfo.dwSize      = sizeof(QMAPI_NET_FTP_PARAM);
        finfo.bEnableFTP = pFtpUpDataParam->dwEnableFTP;
        finfo.dwFTPPort   = pFtpUpDataParam->dwFTPPort;
        finfo.wTopDirMode = pFtpUpDataParam->wTopDirMode;
        finfo.wSubDirMode = pFtpUpDataParam->wSubDirMode;
        strncpy(finfo.csFTPIpAddress, pFtpUpDataParam->csFTPIpAddress, 32);
        strncpy((char *)finfo.csUserName, (char *)pFtpUpDataParam->sUserName, QMAPI_NAME_LEN);
        strncpy((char *)finfo.csPassword, (char *)pFtpUpDataParam->sPassword, QMAPI_PASSWD_LEN);
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_FTPCFG, 0, &finfo, sizeof(QMAPI_NET_FTP_PARAM));
        break;
    }
    case CMD_GET_FTPUPDATA_PARAM:
    {
        TL_FTPUPDATA_PARAM fparam;
        QMAPI_NET_FTP_PARAM finfo;
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_FTPCFG, 0, &finfo, sizeof(QMAPI_NET_FTP_PARAM)) == QMAPI_SUCCESS)
        {
            memset(&fparam, 0, sizeof(TL_FTPUPDATA_PARAM));
            fparam.dwSize      = sizeof(TL_FTPUPDATA_PARAM);
            fparam.dwEnableFTP = finfo.bEnableFTP;
            fparam.dwFTPPort   = finfo.dwFTPPort;
            fparam.wTopDirMode = finfo.wTopDirMode;
            fparam.wSubDirMode = finfo.wSubDirMode;
            strncpy(fparam.csFTPIpAddress, finfo.csFTPIpAddress, 32);
            strncpy((char *)fparam.sUserName, finfo.csUserName, QMAPI_NAME_LEN);
            strncpy((char *)fparam.sPassword, finfo.csPassword, QMAPI_PASSWD_LEN);
            toclientMsg.nCommand = CMD_GET_FTPUPDATA_PARAM;
            toclientMsg.nBufSize = sizeof(TL_FTPUPDATA_PARAM);
            TL_Video_Net_Send(&toclientMsg, (Char *)&fparam);
        }
        break;
    }
    case CMD_GET_DIRECT_LIST:
    {
        SendUnsupportMsg(&toclientMsg);
        break;
    }
   
    case CMD_GET_FILE_DATA:
    {
        SendUnsupportMsg(&toclientMsg);
        break;
    }
    case CMD_STOP_FILE_DOWNLOAD:
    {
        SendUnsupportMsg(&toclientMsg);
        break;
    }
    case CMD_GET_UPNP:
    {
        QMAPI_UPNP_CONFIG uinfo;
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_UPNPCFG, 0, &uinfo, sizeof(QMAPI_UPNP_CONFIG)) == QMAPI_SUCCESS)
        {
            toclientMsg.nCommand = CMD_GET_UPNP;
            toclientMsg.nBufSize = sizeof(QMAPI_UPNP_CONFIG);
            TL_Video_Net_Send(&toclientMsg, (Char *)&uinfo);
        }
        break;
    }
    case CMD_NET_GET_RTSPCFG:
    {
        QMAPI_NET_RTSP_CFG rinfo;
        if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_RTSPCFG, 0, &rinfo, sizeof(QMAPI_NET_RTSP_CFG)) == QMAPI_SUCCESS)
        {
            toclientMsg.nCommand = CMD_NET_GET_RTSPCFG;
            toclientMsg.nBufSize = sizeof(QMAPI_NET_RTSP_CFG);
            TL_Video_Net_Send(&toclientMsg, (Char *)&rinfo);
        }
        break;
    }
    case CMD_NET_SET_RTSPCFG:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_RTSPCFG, 0, pRecvBuf, sizeof(QMAPI_NET_RTSP_CFG));
        break;
    }
    case CMD_NET_GET_NETCFG:
    {
        QMAPI_NET_NETWORK_CFG info;

        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, toclientMsg.nChannel, &info, sizeof(QMAPI_NET_NETWORK_CFG));
        toclientMsg.nBufSize = sizeof(QMAPI_NET_NETWORK_CFG);
        TL_Video_Net_Send(&toclientMsg, (char *)&info);
        break;
    }
    case CMD_NET_SET_NETCFG:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NETCFG, toclientMsg.nChannel, pRecvBuf, sizeof(QMAPI_NET_NETWORK_CFG));
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }
    case CMD_NET_SET_SAVECFG:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG, toclientMsg.nChannel, NULL, 0);
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }

    case CMD_REQUEST_IFRAME:
    {
        TL_CHANNEL_FRAMEREQ *PFrame = (TL_CHANNEL_FRAMEREQ *)pRecvBuf;    
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_NET_STA_IFRAME_REQUEST,PFrame->dwChannel,(void *)&(PFrame->dwStreamType), sizeof(void *));
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }   
    case CMD_NET_GET_PICCFG:
    {
        QMAPI_NET_CHANNEL_PIC_INFO info;

        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, toclientMsg.nChannel, &info,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        toclientMsg.nBufSize = sizeof(QMAPI_NET_CHANNEL_PIC_INFO);
        TL_Video_Net_Send(&toclientMsg, (char *)&info);
        break;
    }
    case CMD_NET_SET_PICCFG:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_PICCFG, toclientMsg.nChannel, pRecvBuf,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }
    case CMD_NET_GET_DEF_PICCFG:
    {
        QMAPI_NET_CHANNEL_PIC_INFO info;
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEF_PICCFG, toclientMsg.nChannel, &info,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        toclientMsg.nBufSize = sizeof(QMAPI_NET_CHANNEL_PIC_INFO);
        TL_Video_Net_Send(&toclientMsg, (char *)&info);
        break;
    }
    case CMD_NET_GET_SUPPORT_STREAM_FMT:
    {
        QMAPI_NET_SUPPORT_STREAM_FMT info;
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_SUPPORT_STREAM_FMT, toclientMsg.nChannel, &info,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        toclientMsg.nBufSize = sizeof(QMAPI_NET_SUPPORT_STREAM_FMT);
        TL_Video_Net_Send(&toclientMsg, (char *)&info);
        break;
    }
    case CMD_GET_COLOR:
    case CMD_NET_GET_COLORCFG:
    {
        QMAPI_NET_CHANNEL_COLOR color  = {0};
        color.dwSize           = sizeof(QMAPI_NET_CHANNEL_COLOR);
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_COLORCFG, toclientMsg.nChannel, &color,sizeof(QMAPI_NET_CHANNEL_COLOR));
        toclientMsg.nBufSize   = sizeof(QMAPI_NET_CHANNEL_COLOR);
        printf("nBrightness:%d. nContrast:%d. nDefinition:%d. nHue:%d. nSaturation:%d.\n", 
            color.nBrightness, color.nContrast, color.nDefinition, color.nHue, color.nSaturation);
        TL_Video_Net_Send(&toclientMsg, (char *)&color);
        break;
    }
    case CMD_SET_COLOR:
    case CMD_NET_SET_COLORCFG:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_COLORCFG, toclientMsg.nChannel, pRecvBuf, sizeof(QMAPI_NET_CHANNEL_COLOR));
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }
    case CMD_NET_GET_COLOR_SUPPORT:
    {
        QMAPI_NET_COLOR_SUPPORT color  = {0};
        color.dwSize           = sizeof(QMAPI_NET_COLOR_SUPPORT);
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_COLOR_SUPPORT, toclientMsg.nChannel, &color,sizeof(QMAPI_NET_COLOR_SUPPORT));
        toclientMsg.nBufSize   = sizeof(QMAPI_NET_COLOR_SUPPORT);
        printf("nBrightness:%d~%d. nContrast:%d~%d. nDefinition:%d~%d. nHue:%d~%d. nSaturation:%d~%d.\n", 
            color.strBrightness.nMin, color.strBrightness.nMax, color.strContrast.nMin,color.strContrast.nMax, 
            color.strDefinition.nMin,color.strDefinition.nMax, color.strHue.nMin,color.strHue.nMax, color.strSaturation.nMin,color.strSaturation.nMax);
        TL_Video_Net_Send(&toclientMsg, (char *)&color);
        break;
    }
    case CMD_NET_GET_ENHANCED_COLOR:
    {
        QMAPI_NET_CHANNEL_ENHANCED_COLOR color = {0};
        color.dwSize           = sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR);
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENHANCED_COLOR, toclientMsg.nChannel, &color,sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR));
        toclientMsg.nBufSize   = sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR);
        TL_Video_Net_Send(&toclientMsg, (char *)&color);
        break;
    }
    case CMD_NET_SET_COLORCFG_SINGLE:
    {
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_COLORCFG_SINGLE, toclientMsg.nChannel, pRecvBuf, sizeof(QMAPI_NET_CHANNEL_COLOR_SINGLE));
        toclientMsg.nBufSize   = 0;
        TL_Video_Net_Send(&toclientMsg, NULL);
        break;
    }
    case CMD_NET_GET_RESETSTATE:
    {
        int param = 0;
        toclientMsg.nErrorCode = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_RESETSTATE, toclientMsg.nChannel, &param, sizeof(int));
        toclientMsg.nBufSize   = sizeof(int);
        TL_Video_Net_Send(&toclientMsg, (char *)&param);
        break;
    }
    case CMD_NET_SET_ID_CIPHER:
    {
        toclientMsg.nErrorCode = TL_Set_Cipher((TLNV_ID_CIPHER *)pRecvBuf);
        if (toclientMsg.nErrorCode == TL_SUCCESS)
        {
            TLNV_SERVER_INFO sinfo;
            TLNV_ID_CIPHER cipher;
            TL_Server_Info_func((char *)&sinfo);
            cipher.woSize          = sizeof(TLNV_ID_CIPHER);
            cipher.woDataSize      = sizeof(TLNV_SERVER_INFO);
            memcpy(cipher.byCipherData, &sinfo, sizeof(TLNV_SERVER_INFO));
            toclientMsg.nBufSize   = sizeof(TLNV_ID_CIPHER);
            TL_Video_Net_Send(&toclientMsg, (char *)&cipher);
        }
        else
        {
            toclientMsg.nBufSize   = 0;
            TL_Video_Net_Send(&toclientMsg, NULL);
        }
        break;
    }
    case CMD_UPDATA_DEFAULT_PTZCMD_DATA:
    case CMD_NET_GET_DEF_COLORCFG:
    case CMD_NET_GET_ENHANCED_COLOR_SUPPORT:
    default:
        toclientMsg.nCommand = pMsgHead->nCommand;
        toclientMsg.nBufSize = 0;
        toclientMsg.nErrorCode = TLERR_UNSUPPORT;
        TL_Video_Net_Send(&toclientMsg,0);
        printf("#### %s %d, unsupport cmd :0x%X\n", __FUNCTION__, __LINE__, pMsgHead->nCommand);
        break;
    }

    return 1;
}

