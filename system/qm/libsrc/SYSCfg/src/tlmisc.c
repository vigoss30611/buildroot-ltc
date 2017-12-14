/******************************************************************************
******************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "tl_common.h"
#include "tlsmtp.h"
#include "watchdog.h"
#include "Q3AV.h"
#include "tlmisc.h"
#include "sysconfig.h"
#include "libCamera.h"
#include "video.h"

#ifdef TL_ENCRYPT_SUPPORT
    #include "tl_authen_interface.h"
#endif
#include "QMAPIErrno.h"

extern syscfg_default_param enSysDefaultParam;

int tl_flash_erase_block(char *handle)
{
    unlink(handle);

    return 0;
}

int tl_flash_read(char *handle, unsigned char *data, int length)
{
    int ret;
    FILE *  fp;

    if (handle == NULL || data == NULL)
    {   
        printf("file(%s), line(%d):tl_flash_read invalid parameters error\n",__FILE__, __LINE__);
        return -1;
    }
    
    fp = fopen(handle, "rb+");
    if( fp == NULL )
    {
        printf("file(%s), line(%d):tl_flash_read  fopen file(%s) fail(%d)\n",__FILE__, __LINE__,handle,errno);
        err();
        return -1;
    }

    ret = fread(data, 1, length, fp);
    if( ret < 0 )
    {
        printf("file(%s), line(%d):tl_flash_read  fread fail\n",__FILE__, __LINE__);
        fclose(fp);
        return -1;
    }

    fclose(fp);

    return 0;
}

int tl_flash_write(char *handle, unsigned char *data, int length)
{
    int ret;
    int flashfd;
    tl_flash_erase_block(handle);

    flashfd = open(handle, O_RDWR|O_CREAT|O_SYNC);
    if( flashfd < 0 )
    {
        printf("Open flash fail, handle:%s\n", handle);
        close(flashfd);
        return -1;
    }

    ret = write(flashfd, data, length);
    if( ret < 0 )
    {
        printf("Flash write fail, handle:%s\n", handle);
        close(flashfd);
        return -1;
    }

    close(flashfd);

    return 0;
}

int tl_read_fatal_param( void )
{
    return tl_flash_read(TL_FLASH_FATAL_CONF_DEV,(unsigned char *)&g_fatal_param, sizeof(g_fatal_param));
}

///Read Flash Parameter

///01 Read
int tl_read_Server_Network( void )
{
    int nRes = 0;
    nRes = tl_flash_read(DMS_SERVER_NETWORK_CONFIG ,(unsigned char *)&g_tl_globel_param.stNetworkCfg , sizeof(QMAPI_NET_NETWORK_CFG));
    if(nRes < 0)
    {
        return nRes;
    }
    nRes = tl_flash_read(DMS_SERVER_DEVICE_CONFIG ,(unsigned char *)&g_tl_globel_param.stDevicInfo , sizeof(QMAPI_NET_DEVICE_INFO));
    return nRes;

}

///02 Read
int tl_read_Server_User( void )
{
    return tl_flash_read(TL_FLASH_CONF_SERVER_USER ,(unsigned char *)g_tl_globel_param.TL_Server_User , sizeof(TL_SERVER_USER)*10);
}


///03 Read
int tl_read_Server_Info( void )
{
    return tl_flash_read(TL_FLASH_CONF_SERVER_INFO ,(unsigned char *)&g_tl_globel_param.TL_Server_Info , sizeof(TLNV_SERVER_INFO));
}
///04 Read
int tl_read_Stream_Info( void )
{
    return tl_flash_read(TL_FLASH_CONF_STREAM_INFO ,(unsigned char *)g_tl_globel_param.TL_Stream_Info , sizeof(TLNV_CHANNEL_INFO)*QMAPI_MAX_CHANNUM);
}


///07 Read
int tl_read_Channel_Enhanced_Color( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_ENHANCED_COLOR ,(unsigned char *)g_tl_globel_param.TL_Channel_EnhancedColor , sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR)*QMAPI_MAX_CHANNUM);
}
///07 Read
int tl_read_Channel_Color( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_COLOR ,(unsigned char *)g_tl_globel_param.TL_Channel_Color , sizeof(QMAPI_NET_CHANNEL_COLOR)*QMAPI_MAX_CHANNUM);
}

///08 Read
int tl_read_Channel_Logo( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_LOGO ,(unsigned char *)g_tl_globel_param.TL_Channel_Logo , sizeof(TL_CHANNEL_LOGO)*QMAPI_MAX_CHANNUM);
}
///09 Read
int tl_read_Channel_Shelter( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_SHELTER ,(unsigned char *)g_tl_globel_param.TL_Channel_Shelter , sizeof(TL_CHANNEL_SHELTER)*QMAPI_MAX_CHANNUM);
}
///10 Read
int tl_read_Channel_Motion_Detect( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_MOTION_DETECT ,(unsigned char *)g_tl_globel_param.TL_Channel_Motion_Detect , sizeof(TL_CHANNEL_MOTION_DETECT)*QMAPI_MAX_CHANNUM);
}

///11 Read
int tl_read_Channel_Probe_Alarm( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_PROBE_ALARM ,(unsigned char *)g_tl_globel_param.TL_Channel_Probe_Alarm , sizeof(TL_SENSOR_ALARM)*TL_MAXSENSORNUM);
}

///12 Read
int tl_read_Channel_Video_Lost( void )
{
    return 0;
}
///13 Read
int tl_read_Channel_Osd_Info( void )
{
    tl_flash_read(TL_FLASH_CONF_TIME_OSDPOS ,(unsigned char *)g_tl_globel_param.stTimeOsdPos , sizeof(g_tl_globel_param.stTimeOsdPos));
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_OSDINFO ,(unsigned char *)g_tl_globel_param.TL_Channel_Osd_Info , sizeof(TL_CHANNEL_OSDINFO)*QMAPI_MAX_CHANNUM);
}
///14 Read
int tl_read_Server_Osd_Data( void )
{
    return tl_flash_read(TL_FLASH_CONF_CHANNEL_OSD_DATA ,(unsigned char *)g_tl_globel_param.TL_Server_Osd_Data , sizeof(TL_SERVER_OSD_DATA)*QMAPI_MAX_CHANNUM);
}
///15 Read
int tl_read_Server_Cominfo( void )
{
    return tl_flash_read(TL_FLASH_CONF_SERVER_COMINFO ,(unsigned char *)g_tl_globel_param.TL_Server_Cominfo , sizeof(TL_SERVER_COMINFO_V2)*QMAPI_MAX_CHANNUM);
}
///16 Read
int tl_read_Server_Com2info( void )
{
    return tl_flash_read(TL_FLASH_CONF_SERVER_COM2INFO , (unsigned char *)g_tl_globel_param.TL_Server_Com2info , sizeof(TL_SERVER_COM2INFO)*QMAPI_MAX_CHANNUM);
}
///17 Read
int tl_read_Upload_PTZ_Protocol( void )
{
    return tl_flash_read(TL_FLASH_CONF_PLOAD_PTZ_PROTOCOL ,(unsigned char *)g_tl_globel_param.TL_Upload_PTZ_Protocol , sizeof(TL_UPLOAD_PTZ_PROTOCOL)*QMAPI_MAX_CHANNUM);
}

///18 Read
int tl_read_FtpUpdata_Param( void )
{
    return tl_flash_read(TL_FLASH_CONF_FTPUPDATA_PARAM ,(unsigned char *)&g_tl_globel_param.TL_FtpUpdata_Param , sizeof(TL_FTPUPDATA_PARAM));
}
///19 Read
int tl_read_Notify_Servers( void )
{
    return tl_flash_read(TL_FLASH_CONF_NOTIFY_SERVER ,(unsigned char *)&g_tl_globel_param.TL_Notify_Servers , sizeof(TL_NOTIFY_SERVER));
}

int tl_read_ddns_config( void )
{
    return tl_flash_read(TL_FLASH_CONF_DDNS_INFO ,(unsigned char *)&g_tl_globel_param.stDDNSCfg, sizeof(QMAPI_NET_DDNSCFG));
}

///22 Read this is producer set information
int tl_read_producer_setting_config(void)
{
    return tl_flash_read(TL_FLASH_CONF_PRODUCER_PARAMETER_SETTING ,(unsigned char *)&g_tl_globel_param.stNetPlatCfg, sizeof(QMAPI_NET_PLATFORM_INFO_V2));
}


///24 Read defaute PTZ
int tl_read_default_PTZ_Protocol( void )
{
    int     i, ret;

    ret = tl_flash_read(TL_FLASH_CONF_DEFAULTE_PTZ_PROTOCOL ,(unsigned char *)&g_tl_globel_param.TL_Upload_PTZ_Protocol[0], sizeof(TL_UPLOAD_PTZ_PROTOCOL));
    for(i = 1; i < QMAPI_MAX_CHANNUM; i++)
    {
        memcpy(&g_tl_globel_param.TL_Upload_PTZ_Protocol[i], &g_tl_globel_param.TL_Upload_PTZ_Protocol[0], sizeof(TL_UPLOAD_PTZ_PROTOCOL));
    }

    return ret;
}

#if 0
int tl_read_default_PTZ_Protocol( void )
{
    return tl_flash_read(TL_FLASH_CONF_DEFAULTE_PTZ_PROTOCOL , &g_tl_globel_param.TL_Upload_PTZ_Protocol[0], sizeof(TL_UPLOAD_PTZ_PROTOCOL)*QMAPI_MAX_CHANNUM);
}
#endif


/// 26 read wireless 
int  tl_read_wireless_wifi_param(void)
{
    return tl_flash_read(TL_FLASH_CONF_WIRELESS_WIFI_PARAM ,(unsigned char *)&g_tl_globel_param.tl_Wifi_config, sizeof(TL_WIFI_CONFIG));
}

/// 27 read wireless 
int  tl_read_wireless_tdscdma_param(void)
{
    return tl_flash_read(TL_FLASH_CONF_WIRELESS_TDSCDMA_PARAM ,(unsigned char *)&g_tl_globel_param.tl_Tdcdma_config, sizeof(TL_TDSCDMA_CONFIG));
}

/// 28 read stream pic info 
int  tl_read_ChannelPic_param(void)
{
    int i, iRet;
    QMAPI_NET_COMPRESSION_INFO   *PicPara;
	
    iRet = tl_flash_read(TL_FLASH_CONF_CHANNEL_PIC ,(unsigned char *)&g_tl_globel_param.stChannelPicParam, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)*QMAPI_MAX_CHANNUM);
    if(iRet == 0)
    {
         for(i=0;i<QMAPI_MAX_CHANNUM;i++)
         {
			PicPara = &g_tl_globel_param.stChannelPicParam[i].stRecordPara;
			printf("Channel:%d. stRecordPara. Height;%u. Width:%u. \n",i, PicPara->wHeight,PicPara->wWidth);

			PicPara = &g_tl_globel_param.stChannelPicParam[i].stNetPara;
			printf("Channel:%d. stNetPara. Height;%u. Width:%u. \n",i, PicPara->wHeight,PicPara->wWidth);

			PicPara = &g_tl_globel_param.stChannelPicParam[i].stPhonePara;
			printf("Channel:%d. stPhonePara. Height;%u. Width:%u. \n",i, PicPara->wHeight,PicPara->wWidth);
         }
    }

    return iRet;
}

int tl_read_timezone_config(void)
{
    int iRet = tl_flash_read(TL_FLASH_CONF_TIMEZONE_PARAM , (unsigned char *)&g_tl_globel_param.stTimezoneConfig, sizeof(QMAPI_NET_ZONEANDDST));
    if(iRet != 0)
    {
        memset(&g_tl_globel_param.stTimezoneConfig, 0x00, sizeof(g_tl_globel_param.stTimezoneConfig));
        g_tl_globel_param.stTimezoneConfig.dwSize = sizeof(g_tl_globel_param.stTimezoneConfig);
    }
    if(g_tl_globel_param.stTimezoneConfig.dwSize != sizeof(g_tl_globel_param.stTimezoneConfig))
    {
        g_tl_globel_param.stTimezoneConfig.dwSize = sizeof(g_tl_globel_param.stTimezoneConfig);
    }
    return iRet;
}

int  tl_read_upnp_param(void)
{
    
     int iRet = tl_flash_read(TL_FLASH_CONF_UPNP_PARAM , (unsigned char *)&g_tl_globel_param.tl_upnp_config, sizeof(TL_UPNP_CONFIG));
     if(!g_tl_globel_param.tl_upnp_config.bEnable)
     {
         g_tl_globel_param.tl_upnp_config.dwDataPort1 = 0;
         g_tl_globel_param.tl_upnp_config.dwWebPort1 = 0;
         g_tl_globel_param.tl_upnp_config.dwMobilePort1 = 0;
     }
     g_tl_globel_param.tl_upnp_config.wDataPortOK = 0;
     g_tl_globel_param.tl_upnp_config.wMobilePortOK = 0;
     g_tl_globel_param.tl_upnp_config.wWebPortOK = 0;
     return iRet;
}

int tl_read_rtsp_param(void)
{
    int iRet =  tl_flash_read(TL_FLASH_CONF_RTSP_PARAM , (unsigned char *)&g_tl_globel_param.stRtspCfg, sizeof(QMAPI_NET_RTSP_CFG));
    if(iRet  != 0)
    {
        memset(&g_tl_globel_param.stRtspCfg, 0x00, sizeof(QMAPI_NET_RTSP_CFG));
        g_tl_globel_param.stRtspCfg.dwSize = sizeof(QMAPI_NET_RTSP_CFG);  
        g_tl_globel_param.stRtspCfg.dwPort = 554;       
    }
    if(g_tl_globel_param.stRtspCfg.dwPort <= 0)
    {
        g_tl_globel_param.stRtspCfg.dwPort = 554;       
    }
    return iRet;
}
int tl_read_snap_timer(void)
{
    int iRet = tl_flash_read(TL_FLASH_CONF_SNAP_TIMER,(unsigned char *)&g_tl_globel_param.stSnapTimer, sizeof(g_tl_globel_param.stSnapTimer));
    if(iRet != 0)
    {
        memset(&g_tl_globel_param.stSnapTimer, 0, sizeof(&g_tl_globel_param.stSnapTimer));
        g_tl_globel_param.stSnapTimer.dwSize = sizeof(g_tl_globel_param.stSnapTimer);
    }
    return iRet;
}

int tl_read_channel_reaord_param(void)
{
	return tl_flash_read(TL_FLASH_CONF_CHANNEL_RECORD ,(unsigned char *)&g_tl_globel_param.stChannelRecord, sizeof(QMAPI_NET_CHANNEL_RECORD)*QMAPI_MAX_CHANNUM);
}

int  tl_read_ntp_param(void)
{
    return tl_flash_read(TL_FLASH_CONF_NTP_PARAM,(unsigned char *)&g_tl_globel_param.stNtpConfig, sizeof(QMAPI_NET_NTP_CFG));
}

int tl_read_dms_channel_osd_config(void)
{
    int iRet = tl_flash_read(TL_FLASH_CONF_DMS_CHN_OSDINFO ,(unsigned char *)&g_tl_globel_param.stChnOsdInfo, sizeof(g_tl_globel_param.stChnOsdInfo));
    if(iRet != 0)
    {
        int i = 0;
        memset(&g_tl_globel_param.stChnOsdInfo, 0, sizeof(&g_tl_globel_param.stChnOsdInfo));
        for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
        {
            g_tl_globel_param.stChnOsdInfo[i].dwSize    = sizeof(g_tl_globel_param.stChnOsdInfo[i]);
            g_tl_globel_param.stChnOsdInfo[i].dwChannel = i;
        }
    }
    return iRet;
}
int tl_read_dms_channel_cb_detection_config(void)
{
    int iRet = tl_flash_read(DMS_SERVER_CB_DETECTION ,(unsigned char *)&g_tl_globel_param.stCBDetevtion, sizeof(g_tl_globel_param.stCBDetevtion));
    if(iRet != 0)
    {
        int i = 0;
        for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
        {
            TL_Default_CB_Detection(&g_tl_globel_param.stCBDetevtion[i], i);
        }
    }
    return iRet;
}

int  tl_read_consumer_config(void)
{
    return tl_flash_read(TL_FLASH_CONF_CONSUMER_PARAM,(unsigned char *)&g_tl_globel_param.stConsumerInfo, sizeof(g_tl_globel_param.stConsumerInfo));
}

int  tl_read_maintain_config(void)
{
    return tl_flash_read(TL_FLASH_CONF_MAINTAIN_PARAM,(unsigned char *)&g_tl_globel_param.stMaintainParam, sizeof(g_tl_globel_param.stMaintainParam));
}

int  tl_read_wifi_config(void)
{
    return tl_flash_read(TL_FLASH_CONF_WIFI_PARAM,(unsigned char *)&g_tl_globel_param.stWifiConfig, sizeof(g_tl_globel_param.stWifiConfig));
}

int tl_read_onviftestinfo_config(void)
{
    return tl_flash_read(TL_FLASH_CONF_ONVIFINFO_PARAM, (unsigned char *)&g_tl_globel_param.stONVIFInfo, sizeof(g_tl_globel_param.stONVIFInfo));
}

///TL Save 
void tl_read_all_parameter(void)
{
    int     ret;
    unsigned int uiNum1 = 0,uiNum2 = 0,uiNum3 = 0,uiNum4 = 0;

    memset(&g_tl_globel_param , 0 , sizeof(TL_FIC8120_FLASH_SAVE_GLOBEL));
    
    tl_read_Server_Network();

    tl_read_Server_User();

    //tl_read_Server_Info();
    tl_read_Stream_Info();

    tl_read_Channel_Color();

    tl_read_Channel_Enhanced_Color();
    
    tl_read_Channel_Logo();

    tl_read_Channel_Shelter();

    tl_read_Channel_Motion_Detect();

    tl_read_Channel_Probe_Alarm();

    tl_read_Channel_Osd_Info();

    tl_read_Server_Osd_Data();

    tl_read_Server_Cominfo();

    tl_read_Server_Com2info();

    tl_read_Upload_PTZ_Protocol();

    tl_read_FtpUpdata_Param();

    tl_read_Notify_Servers();

    //tl_read_pppoe_config();

    tl_read_timezone_config();

    tl_read_ddns_config();

    tl_read_producer_setting_config();

    //tl_read_record_setting();

	tl_read_channel_reaord_param();
	
    //OTTO ADD
    ret = tl_read_ChannelPic_param();

    tl_read_upnp_param();
    
    //tl_read_mobile_param();

    tl_read_rtsp_param();
    tl_read_snap_timer();
    tl_read_ntp_param();
    tl_read_dms_channel_osd_config();
    tl_read_dms_channel_cb_detection_config();
    tl_read_consumer_config();
    tl_read_maintain_config();
    tl_read_onviftestinfo_config();
//#ifdef TL_SUPPORT_WIRELESS
    tl_read_wifi_config();
    //tl_read_wireless_wifi_param();
    //tl_read_wireless_tdscdma_param();
//#endif

    TL_GetAddrNum(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4,&uiNum1,&uiNum2,&uiNum3,&uiNum4);
/*
    if( g_tl_globel_param.stNetPlatCfg.enPlatManufacture == QMAPI_NETPT_HXHT)
    {
        QMAPI_NETPT_HXHT_PRIVATE_INFO  stNetPtHxht;// = (QMAPI_NETPT_HXHT_PRIVATE_INFO*)g_tl_globel_param.stNetPlatCfg.pPrivateData;       
        int nBasePort = 0;
        memset(&stNetPtHxht,0,sizeof(QMAPI_NETPT_HXHT_PRIVATE_INFO));
        memcpy(&stNetPtHxht,g_tl_globel_param.stNetPlatCfg.pPrivateData,sizeof(QMAPI_NETPT_HXHT_PRIVATE_INFO));
        if(stNetPtHxht.nNetMode == 0)
        {           
            if(g_tl_globel_param.tl_Wifi_config.bWifiEnable)
            {
                nBasePort = 4000+(g_tl_globel_param.tl_Wifi_config.dwNetIpAddr>>24)*10;
            }
            else
            {
                nBasePort = 4000+uiNum4*10;
            }
            stNetPtHxht.nMsgdPort =  nBasePort;
            stNetPtHxht.nVideoPort = nBasePort + 1;
            stNetPtHxht.nTalkPort = nBasePort + 2;
            stNetPtHxht.nVodPort = nBasePort + 3;
        }
        stNetPtHxht.byMsgdPortOK = 0;         //MsgPort UPNP is ok
        stNetPtHxht.byVideoPortOK = 0;            //VideoPort UPNP is ok
        stNetPtHxht.byTalkPortOK = 0;             //TalkPort UPNP is ok
        stNetPtHxht.byVodPortOK = 0;              //VodPort UPNP is ok  
        memcpy(g_tl_globel_param.stNetPlatCfg.pPrivateData,&stNetPtHxht,sizeof(QMAPI_NETPT_HXHT_PRIVATE_INFO));
    }
*/
    
    ret=tl_flash_read(TL_FLASH_CONF_EMAIL_PARAM, (unsigned char *)&g_tl_globel_param.TL_Email_Param , sizeof(QMAPI_NET_EMAIL_PARAM));
    if(ret == -1)
    {
        memset(&g_tl_globel_param.TL_Email_Param,0,sizeof(QMAPI_NET_EMAIL_PARAM));
        g_tl_globel_param.TL_Email_Param.dwSize=sizeof(QMAPI_NET_EMAIL_PARAM);
    }
	
//  TL_Get_Video_Stream_Info();
}

///Write Flash Parameter

///01 Write
int tl_write_Server_Network( void )
{
    tl_flash_write(DMS_SERVER_NETWORK_CONFIG ,(unsigned char *)&g_tl_globel_param.stNetworkCfg, sizeof(QMAPI_NET_NETWORK_CFG));
	//printf("------------------------DMS_SERVER_NETWORK_CONFIG--------------------------\n");
    return tl_flash_write(DMS_SERVER_DEVICE_CONFIG ,(unsigned char *)&g_tl_globel_param.stDevicInfo, sizeof(QMAPI_NET_DEVICE_INFO));
}

///02 Write
int tl_write_Server_User( void )
{
    return tl_flash_write(TL_FLASH_CONF_SERVER_USER ,(unsigned char *)g_tl_globel_param.TL_Server_User , sizeof(TL_SERVER_USER)*10);
}


///03 Write
int tl_write_Server_Info( void )
{
    return tl_flash_write(TL_FLASH_CONF_SERVER_INFO ,(unsigned char *)&g_tl_globel_param.TL_Server_Info , sizeof(TLNV_SERVER_INFO));
}
///04 Write
int tl_write_Stream_Info( void )
{
    return tl_flash_write(TL_FLASH_CONF_STREAM_INFO ,(unsigned char *)g_tl_globel_param.TL_Stream_Info , sizeof(TLNV_CHANNEL_INFO)*QMAPI_MAX_CHANNUM);
}


///07 Write
int tl_write_Channel_Color( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_COLOR ,(unsigned char *)g_tl_globel_param.TL_Channel_Color , sizeof(QMAPI_NET_CHANNEL_COLOR)*QMAPI_MAX_CHANNUM);
}

///07 Write
int tl_write_Channel_Enhanced_Color( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_ENHANCED_COLOR,(unsigned char *)g_tl_globel_param.TL_Channel_EnhancedColor , sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR)*QMAPI_MAX_CHANNUM);
}

///08 Write
int tl_write_Channel_Logo( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_LOGO ,(unsigned char *)g_tl_globel_param.TL_Channel_Logo , sizeof(TL_CHANNEL_LOGO)*QMAPI_MAX_CHANNUM);
}
///09 Write
int tl_write_Channel_Shelter( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_SHELTER ,(unsigned char *)g_tl_globel_param.TL_Channel_Shelter , sizeof(TL_CHANNEL_SHELTER)*QMAPI_MAX_CHANNUM);
}
///10 Write
int tl_write_Channel_Motion_Detect( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_MOTION_DETECT ,(unsigned char *)g_tl_globel_param.TL_Channel_Motion_Detect , sizeof(TL_CHANNEL_MOTION_DETECT)*QMAPI_MAX_CHANNUM);
}

///11 Write
int tl_write_Channel_Probe_Alarm( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_PROBE_ALARM ,(unsigned char *)g_tl_globel_param.TL_Channel_Probe_Alarm , sizeof(TL_SENSOR_ALARM)*TL_MAXSENSORNUM);
}

///12 Write
int tl_write_Channel_Video_Lost( void )
{
    return 0;
}
///13 Write
int tl_write_Channel_Osd_Info( void )
{
    tl_flash_write(TL_FLASH_CONF_TIME_OSDPOS ,(unsigned char *)g_tl_globel_param.stTimeOsdPos , sizeof(g_tl_globel_param.stTimeOsdPos));
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_OSDINFO ,(unsigned char *)g_tl_globel_param.TL_Channel_Osd_Info , sizeof(TL_CHANNEL_OSDINFO)*QMAPI_MAX_CHANNUM);
}
///14 Write
int tl_write_Server_Osd_Data( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_OSD_DATA ,(unsigned char *)g_tl_globel_param.TL_Server_Osd_Data , sizeof(TL_SERVER_OSD_DATA)*QMAPI_MAX_CHANNUM);
}
///15 Write
int tl_write_Server_Cominfo( void )
{
    return tl_flash_write(TL_FLASH_CONF_SERVER_COMINFO ,(unsigned char *)g_tl_globel_param.TL_Server_Cominfo , sizeof(TL_SERVER_COMINFO_V2)*QMAPI_MAX_CHANNUM);
}
///16 Write
int tl_write_Server_Com2info( void )
{
    return tl_flash_write(TL_FLASH_CONF_SERVER_COM2INFO ,(unsigned char *)g_tl_globel_param.TL_Server_Com2info , sizeof(TL_SERVER_COM2INFO)*QMAPI_MAX_CHANNUM);
}
///17 Write
int tl_write_Upload_PTZ_Protocol( void )
{
    return tl_flash_write(TL_FLASH_CONF_PLOAD_PTZ_PROTOCOL ,(unsigned char *)g_tl_globel_param.TL_Upload_PTZ_Protocol , sizeof(TL_UPLOAD_PTZ_PROTOCOL)*QMAPI_MAX_CHANNUM);
}

///18 Write
int tl_write_FtpUpdata_Param( void )
{
    return tl_flash_write(TL_FLASH_CONF_FTPUPDATA_PARAM ,(unsigned char *)&g_tl_globel_param.TL_FtpUpdata_Param , sizeof(TL_FTPUPDATA_PARAM));
}
///19 Write
int tl_write_Notify_Servers( void )
{
    return tl_flash_write(TL_FLASH_CONF_NOTIFY_SERVER ,(unsigned char *)&g_tl_globel_param.TL_Notify_Servers , sizeof(TL_NOTIFY_SERVER));
}


int tl_write_ddns_config( void )
{
    return tl_flash_write(TL_FLASH_CONF_DDNS_INFO ,(unsigned char *)&g_tl_globel_param.stDDNSCfg, sizeof(QMAPI_NET_DDNSCFG));
}


///22 Write this is producer set information
int tl_write_producer_setting_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_PRODUCER_PARAMETER_SETTING ,(unsigned char *)&g_tl_globel_param.stNetPlatCfg, sizeof(QMAPI_NET_PLATFORM_INFO_V2));
}


int tl_write_channel_reaord_param(void)
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_RECORD ,(unsigned char *)&g_tl_globel_param.stChannelRecord, sizeof(QMAPI_NET_CHANNEL_RECORD)*QMAPI_MAX_CHANNUM);
}

///24 Write this is default ptz protocol
int tl_write_default_ptz_protocol(void)
{
    return tl_flash_write(TL_FLASH_CONF_DEFAULTE_PTZ_PROTOCOL ,(unsigned char *)g_tl_globel_param.TL_Upload_PTZ_Protocol , sizeof(TL_UPLOAD_PTZ_PROTOCOL)*QMAPI_MAX_CHANNUM);
}


/// 26 write 
int  tl_write_wireless_wifi_param(void)
{
    return tl_flash_write(TL_FLASH_CONF_WIRELESS_WIFI_PARAM ,(unsigned char *)&g_tl_globel_param.tl_Wifi_config, sizeof(TL_WIFI_CONFIG));
}


/// 27 read wireless 
int  tl_write_wireless_tdscdma_param(void)
{
    return tl_flash_write(TL_FLASH_CONF_WIRELESS_TDSCDMA_PARAM , (unsigned char *)&g_tl_globel_param.tl_Tdcdma_config, sizeof(TL_TDSCDMA_CONFIG));
}
//OTTO ADD
///28 Write
int tl_write_ChannelPic_param( void )
{
    return tl_flash_write(TL_FLASH_CONF_CHANNEL_PIC, (unsigned char *)g_tl_globel_param.stChannelPicParam, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)*QMAPI_MAX_CHANNUM);
}

int  tl_write_upnp_param(void)
{
    
    return tl_flash_write(TL_FLASH_CONF_UPNP_PARAM , (unsigned char *)&g_tl_globel_param.tl_upnp_config, sizeof(TL_UPNP_CONFIG));
    

     
}

int tl_write_timezone_param(void)
{
    return tl_flash_write(TL_FLASH_CONF_TIMEZONE_PARAM, (unsigned char *)&g_tl_globel_param.stTimezoneConfig,sizeof(QMAPI_NET_ZONEANDDST));
}

int tl_write_rtsp_param(void)
{
    int iRet =  tl_flash_write(TL_FLASH_CONF_RTSP_PARAM , (unsigned char *)&g_tl_globel_param.stRtspCfg, sizeof(QMAPI_NET_RTSP_CFG));
    return iRet;
}
int tl_write_snap_timer(void)
{
    int iRet =  tl_flash_write(TL_FLASH_CONF_SNAP_TIMER , (unsigned char *)&g_tl_globel_param.stSnapTimer, sizeof(g_tl_globel_param.stSnapTimer));
    return iRet;
}
int tl_write_ntp_timer(void)
{
    int iRet =  tl_flash_write(TL_FLASH_CONF_NTP_PARAM , (unsigned char *)&g_tl_globel_param.stNtpConfig, sizeof(g_tl_globel_param.stNtpConfig));
    return iRet;
}
int tl_write_dms_channel_osd_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_DMS_CHN_OSDINFO ,(unsigned char *)&g_tl_globel_param.stChnOsdInfo, sizeof(g_tl_globel_param.stChnOsdInfo));
}
int tl_write_dms_channel_cb_detection_config(void)
{
    return tl_flash_write(DMS_SERVER_CB_DETECTION ,(unsigned char *)&g_tl_globel_param.stCBDetevtion, sizeof(g_tl_globel_param.stCBDetevtion));
}


int  tl_write_consumer_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_CONSUMER_PARAM,(unsigned char *)&g_tl_globel_param.stConsumerInfo, sizeof(g_tl_globel_param.stConsumerInfo));
}

int  tl_write_maintain_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_MAINTAIN_PARAM,(unsigned char *)&g_tl_globel_param.stMaintainParam, sizeof(g_tl_globel_param.stMaintainParam));
}

int  tl_write_wifi_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_WIFI_PARAM,(unsigned char *)&g_tl_globel_param.stWifiConfig, sizeof(g_tl_globel_param.stWifiConfig));
}

int tl_write_onviftestinfo_config(void)
{
    return tl_flash_write(TL_FLASH_CONF_ONVIFINFO_PARAM, (unsigned char *)&g_tl_globel_param.stONVIFInfo, sizeof(g_tl_globel_param.stONVIFInfo));
}

void Write_Reset_Log(int flagCode)
{
    return ;
    char *pText = NULL;
    switch(flagCode)
    {
    case 1://DMS cmd
        pText = "reset by dms network cmd!";
        break;
    case 2://3511或3515编码参数错误-param.StreamFormat err
        pText = "reset by encode para err(3511 or 3515)!";
        break;
    case 3://broadcast cmd
        pText = "reset by broadcast cmd!";
        break;
    case 4://netword cmd
        pText = "reset by netword cmd(IE or client)!";
        break;
    case 5://set ip err
        pText = "reset by set ip addr err!";
        break;
    case 6://tl_read_fatal_param failure
        pText = "reset by tl_read_fatal_param failure!";
        break;
    case 7://g_fatal_param.default_flag == fFlagSoftReset
        pText = "reset by tl_read_fatal_param-default_flag = fFlagSoftReset!";
        break;
    case 8://g_fatal_param.default_flag== fFlagDefault
        pText = "reset by tl_read_fatal_param-default_flag = fFlagDefault(IO)!";
        break;
    case 9://default_flag = unkown
        pText = "reset by tl_read_fatal_param-default_flag = unkown!";
        break;
    case 10://#ifdef SYS_DEFAULT
        pText = "reset by support SYS_DEFAULT!";
        break;
    default://不明原因--待查
        pText = "reset by unkown err!";
        break;
    }
    tl_flash_write("/mnt/config/reset.config",(unsigned char *)pText, strlen(pText));
}
//更改录像参数时，只在修改结果
void saveRecordParam()
{
	tl_write_producer_setting_config();
	//tl_write_record_setting();
}

void saveChannelRecordParam()
{
	tl_write_channel_reaord_param();
}

void tl_write_all_parameter(void)
{
    tl_write_Server_Network();

    tl_write_Server_User();

    tl_write_Server_Info();

    tl_write_Stream_Info();

    tl_write_Channel_Color();

    tl_write_Channel_Enhanced_Color();
         
    tl_write_Channel_Logo();

    tl_write_Channel_Shelter();

    tl_write_Channel_Motion_Detect();

    tl_write_Channel_Probe_Alarm();

    tl_write_Channel_Osd_Info();

    tl_write_Server_Osd_Data();

    tl_write_Server_Cominfo();

    tl_write_Server_Com2info();

    tl_write_Upload_PTZ_Protocol();

    tl_write_FtpUpdata_Param();

    tl_write_Notify_Servers();

    //tl_write_pppoe_config();

    tl_write_producer_setting_config();
    

    tl_write_channel_reaord_param();

    tl_write_default_ptz_protocol();

    tl_write_ddns_config();

    tl_write_ChannelPic_param();

    tl_write_upnp_param();

    tl_write_timezone_param();

    //tl_write_mobile_param();

    tl_write_rtsp_param();
    tl_write_snap_timer();
    tl_write_ntp_timer();
    tl_write_dms_channel_osd_config();
    tl_write_dms_channel_cb_detection_config();
    //tl_write_dms_perimeter_config();
    //tl_write_dms_itevcontrol_config();
    //tl_write_dms_tripwire_config();
    //tl_write_dms_videofool_config();
    tl_write_consumer_config();
    tl_write_maintain_config();
    tl_write_onviftestinfo_config();

//#ifdef TL_SUPPORT_WIRELESS
    tl_write_wifi_config();
    //tl_write_wireless_wifi_param();
    //tl_write_wireless_tdscdma_param();
//#endif

    tl_flash_write(TL_FLASH_CONF_EMAIL_PARAM, (unsigned char *)&g_tl_globel_param.TL_Email_Param , sizeof(QMAPI_NET_EMAIL_PARAM));

    tl_set_default_param(fFlagChange);
}

int tl_set_default_param(int flag)
{
    tl_fatal_param_t    fatal_param;

    /* save default flag to flash */
    fatal_param.fatal_param_len = sizeof(fatal_param);
    fatal_param.default_flag = flag;

    tl_write_fatal_param(fatal_param);

    return 0;
}

static pthread_mutex_t g_write_mutex = PTHREAD_MUTEX_INITIALIZER;
void* tl_write_parameter_proc(void * para)
{
    pthread_mutex_lock(&g_write_mutex);
	tl_write_all_parameter();
	if(g_bNeedRestart || g_tl_globel_param.bRtspPortChanged)
	{
		QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);
	}
    pthread_mutex_unlock(&g_write_mutex);
	return NULL;
}

void tl_write_all_parameter_asyn(void )
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(pthread_create(&ThreadId, &attr, tl_write_parameter_proc, NULL) == 0)
    {
        pthread_attr_destroy(&attr);
        return ;
    }

    pthread_attr_destroy(&attr);
}


int tl_write_fatal_param(tl_fatal_param_t fatal_param)
{
    memcpy(&g_fatal_param, &fatal_param, sizeof(g_fatal_param));
    g_fatal_param.fatal_param_len = sizeof(g_fatal_param);
    return tl_flash_write(TL_FLASH_FATAL_CONF_DEV, (unsigned char *)&g_fatal_param, sizeof(g_fatal_param));
}

void tl_init_Channel_Color(int nChannel)
{
    Camera_Get_Default_Color(&g_tl_globel_param.TL_Channel_Color[nChannel]);
    g_tl_globel_param.TL_Channel_Color[nChannel].dwChannel=nChannel;
#ifndef TL_Q3_PLATFORM
    GetVadcDefaultEnhancedColor(&g_tl_globel_param.TL_Channel_EnhancedColor[nChannel]);
    g_tl_globel_param.TL_Channel_EnhancedColor[nChannel].dwChannel = nChannel;
    TL_Default_CB_Detection(&g_tl_globel_param.stCBDetevtion[nChannel], nChannel);
#endif
}

/*********************************************************************
    Function:   
        DVR_WDT_Enable
    Description:   
        使能看门狗
    Calls:       
    Called By:     
    parameter:         
    Return:       
    author:
        l59217
 ********************************************************************/
int tl_wdt_enable()
{
	unsigned int enable_flag_tmp = WDIOS_ENABLECARD;
	signed int retvalue = 0;

	retvalue = ioctl(g_tl_wdt_fd, WDIOC_SETOPTIONS, (unsigned long)&enable_flag_tmp);
	//printf("tl_wdt_enable-g_tl_wdt_fd:%d, retvalue:%d\n",g_tl_wdt_fd, retvalue);
	return retvalue;
}

/*********************************************************************
    Function:   
        DVR_WDT_Enable
    Description:   
        使能看门狗
    Calls:       
    Called By:     
    parameter:         
    Return:       
    author:
        l59217
 ********************************************************************/
int tl_wdt_disable()
{
	unsigned int enable_flag_tmp=WDIOS_DISABLECARD;
	signed int retvalue=0;

	retvalue = ioctl(g_tl_wdt_fd,WDIOC_SETOPTIONS,(unsigned long)&enable_flag_tmp);
	printf("tl_wdt_disable-retvalue:%d\n",retvalue);
	return retvalue;
}

/*********************************************************************
    Function:   
        DVR_WDT_Reset
    Description:   
        复位看门狗
    Calls:       
    Called By:     
    parameter:         
    Return:       
    author:
        l59217
 ********************************************************************/
int tl_wdt_reset()
{
	tl_set_wdt_timeout(1000);    
	sleep(1);
	return 0;    
}

int tl_wdt_startup(void)
{
    g_tl_wdt_fd = open(TL_WDT_DEV, O_WRONLY); 
    if (g_tl_wdt_fd < 0)
    {
        printf("file(%s), line(%d), tl_wdt_startup(%s) open fail, errno(%d)\n", 
            __FILE__, __LINE__, TL_WDT_DEV, errno);
        return -1;
    }

    tl_wdt_enable();
    
    printf("LDEV OPEN WDT OK#################\n");

    return 0;
}
/*********************************************************************
    Function:   
        DVR_WDT_Open
    Description:   
        关闭看门狗
    Calls:       
    Called By:     
    parameter:         
    Return:       
    author:
        l59217
 ********************************************************************/
int tl_wdt_close()
{
    if(tl_wdt_disable() != 0)
    {
       printf("before close, disable failed!\n");
       return -1;
    }

    if(g_tl_wdt_fd <= 0)
    {
        printf("warning: file closed already!\n");
    }
    else
    {
		close( g_tl_wdt_fd );
		g_tl_wdt_fd = -1;
    }

    return 0;
}

int tl_wdt_keep_alive( void )
{
    //int value;
    int ret;
    
    //value = 1;
/*/home/hi3510/Hi3510_VSSDK_V1.3.1.0/code/linux/kernel/linux-2.6.14/drivers/char/watchdog/hi3510_wdt.c*/
    ret = ioctl(g_tl_wdt_fd, WDIOC_KEEPALIVE, NULL);
    if (ret < 0)
    {
        printf("tl_wdt_keep_alive error ret = %d, errno = %d\n", ret, errno);
        return -1;
    }
    return 0;
}


int tl_get_wdt_status(void)
{
    int     status = 0;
    int     ret;

/*/home/hi3510/Hi3510_VSSDK_V1.3.1.0/code/linux/kernel/linux-2.6.14/drivers/char/watchdog/hi3510_wdt.c*/
    ret = ioctl(g_tl_wdt_fd, WDIOC_GETSTATUS, &status);
    if (ret < 0)
    {
        printf("tl_get_wdt_status error ret = %d, errno = %d\n", ret, errno);
        return -1;
    }
    printf("tl_get_wdt_status status is %x\n", status);

    return status;
}


int tl_get_wdt_timeout( void )
{
    int     ret;
    int     cur_margin;

/*/home/hi3510/Hi3510_VSSDK_V1.3.1.0/code/linux/kernel/linux-2.6.14/drivers/char/watchdog/hi3510_wdt.c*/
    ret = ioctl(g_tl_wdt_fd, WDIOC_GETTIMEOUT, &cur_margin);
    if (ret < 0)
    {
        printf("tl_get_wdt_status error ret = %d, errno = %d\n", ret, errno);
        return -1;
    }

    return cur_margin;
}


int tl_set_wdt_timeout(int cur_margin)
{
    int     ret;

/*/home/hi3510/Hi3510_VSSDK_V1.3.1.0/code/linux/kernel/linux-2.6.14/drivers/char/watchdog/hi3510_wdt.c*/
    ret = ioctl(g_tl_wdt_fd, WDIOC_SETTIMEOUT, &cur_margin);
    if (ret < 0)
    {
        printf("tl_set_wdt_timeout error ret = %d, errno = %d\n", ret, errno);
        return -1;
    }

    return cur_margin;
}


/*
struct rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
*/





#define BUTCHNGMASK 0x38
#define MOVEMENT 0x40
#define BUTSTATMASK 7
#define BUT3STAT 1
#define BUT2STAT 2
#define BUT1STAT 4
#define BUT3CHNG 8
#define BUT2CHNG 0x10
#define BUT1CHNG 0x20

#define MSE_BUTTON 0
#define MSE_MOTION 1

typedef struct 
{
    unsigned char   status;
    char        xmotion, ymotion;
}DEV_MOUSEINFO;

/* Ioctl Command definitions */
#define MOUSEIOC ('M'<<8)
#define MOUSEIOCREAD (MOUSEIOC| 60)
#define MOUSEIOCNDELAY (MOUSEIOC| 81)
/* mouse */
void DMS_MOUSE_TEST()
{
    DEV_MOUSEINFO   mseinfo;
    int         xpos=1,ypos=1;
    int     fd, ret;

    fd=open("/dev/input/mouse0", O_RDWR);
    if (fd < 0)
    {
        printf("cann't open dev!!!");
        return ;
    }
    printf("open mouse dev ok\n");

    //ioctl(fd, MOUSEIOCNDELAY, 0);
    /*set the mouse dev to nodelay */
    while(1)
    {
        ret = ioctl(fd,MOUSEIOCREAD, &mseinfo); /*read the mouse status*/
        if (ret < 0)
        {
            sleep(1);
            printf("MOUSEIOCREAD error\n");
            err();
        }
        if(mseinfo.status&MOVEMENT)
        {
            xpos=mseinfo.xmotion;
            if(xpos <=0)
                xpos=1;
            else if(xpos >80)
                xpos=80;
            ypos=mseinfo.ymotion;
            if(ypos <=0)
                ypos=1;
            else
            if(ypos > 25)
                ypos=25;
            printf("033[%d;%dH",ypos,xpos);/*adjust the curse position*/
            //fflush(stdout); /*flush*/
        }
        if(mseinfo.status&BUTCHNGMASK)
        {
            if(mseinfo.status&BUT1CHNG)
            {
                if(mseinfo.status&BUT1STAT)
                {
                    printf("Left Depressed");/*left key push*/
                    //fflush(stdout);
                }
                else 
                {
                    printf("Left Relessed");
                    //fflush(stdout);
                }
            }
            if(mseinfo.status&BUT3CHNG)
            {
                if(mseinfo.status&BUT3STAT)
                {
                    printf("Right Depressed"); 
                    //fflush(stdout);
                }
                else 
                {
                    printf("Right Relessed");
                    //fflush(stdout);
                }
            }
        }
    }
}


int GetNetSupportStreamFmt(int nChannel, QMAPI_NET_SUPPORT_STREAM_FMT   *lpstSupportFmt)
{
    memset(lpstSupportFmt, 0x00, sizeof(QMAPI_NET_SUPPORT_STREAM_FMT));
    lpstSupportFmt->dwSize = sizeof(QMAPI_NET_SUPPORT_STREAM_FMT);                        //struct size
    lpstSupportFmt->dwChannel = nChannel;
    QMAPI_SIZE stMainVideoSize[10];
    int i = 0;
    int width;
    int height;
    memset(stMainVideoSize, 0, sizeof(stMainVideoSize));

    extern int Get_Q3_Vi_Format(int w , int h, DWORD *dwVFormat);
    
    DWORD dwVFormat = QMAPI_VIDEO_FORMAT_1080P;
    ven_info_t *ven_info = infotm_video_info_get(0);
    if (!ven_info) {
        printf("%s cannot get ven_info for main stream\n", __FUNCTION__);
        width   = 1920;
        height  = 1080; 
    } else {
        width   = ven_info->width;
        height  = ven_info->height;
    }
    stMainVideoSize[i].nWidth	 = width;
    stMainVideoSize[i++].nHeight = height;

    //TODO: should not report to web
    QMAPI_SIZE stMirorVideoSize[10];
    i = 0;
    memset(stMirorVideoSize, 0, sizeof(stMirorVideoSize));
    dwVFormat = QMAPI_VIDEO_FORMAT_VGA;
    ven_info = infotm_video_info_get(1);
    if (!ven_info) {
        printf("%s cannot get ven_info for sub stream\n", __FUNCTION__);
        width   = 640;
        height  = 480; 
    } else {
        width   = ven_info->width;
        height  = ven_info->height;
        if (Get_Q3_Vi_Format(width, height, &dwVFormat) < 0) { 
            printf("%s cannot Get vi format for %d*%d\n", __FUNCTION__, width, height);
        }
    } 

    if (dwVFormat == QMAPI_VIDEO_FORMAT_CIF)
    {
        stMirorVideoSize[i].nWidth = 352;
        stMirorVideoSize[i++].nHeight = 288;
    }
    else if (dwVFormat == QMAPI_VIDEO_FORMAT_VGA)
    {
        stMirorVideoSize[i].nWidth = 640;
        stMirorVideoSize[i++].nHeight = 480;
        stMirorVideoSize[i].nWidth  = 640;
        stMirorVideoSize[i++].nHeight = 360;
    }
    else if (dwVFormat == QMAPI_VIDEO_FORMAT_640x360)
    {
        stMirorVideoSize[i].nWidth = 640;
        stMirorVideoSize[i++].nHeight = 360;
        stMirorVideoSize[i].nWidth  = 640;
        stMirorVideoSize[i++].nHeight = 480;
    }
    else if (dwVFormat == QMAPI_VIDEO_FORMAT_D1)
    {
        stMirorVideoSize[i].nWidth = 720;
        stMirorVideoSize[i++].nHeight = 576;
    } else {
        stMirorVideoSize[i].nWidth = width;
        stMirorVideoSize[i++].nHeight = height;
    }

    lpstSupportFmt->dwVideoSupportFmt[0][0] = QMAPI_PT_H264;		//Video Format.
    lpstSupportFmt->dwVideoSupportFmt[0][1] = QMAPI_PT_H264_BASELINE;
    lpstSupportFmt->dwVideoSupportFmt[0][2] = QMAPI_PT_MJPEG; 	//Video Format.
    lpstSupportFmt->dwVideoSupportFmt[0][3] = QMAPI_PT_H265; 	//Video Format.

    lpstSupportFmt->stVideoBitRate[0].nMin = 16000;
    lpstSupportFmt->stVideoBitRate[0].nMax = 20000000;
    memcpy(lpstSupportFmt->stVideoSize[0], stMainVideoSize, sizeof(stMainVideoSize));		//Video Size(height,width)
    lpstSupportFmt->dwVideoSupportFmt[1][0] = QMAPI_PT_H264;		//Video Format.
    lpstSupportFmt->dwVideoSupportFmt[1][1] = QMAPI_PT_H264_BASELINE;
    lpstSupportFmt->dwVideoSupportFmt[1][2] = QMAPI_PT_MJPEG; 	//Video Format.
    lpstSupportFmt->dwVideoSupportFmt[1][3] = QMAPI_PT_H265; 	//Video Format.

    lpstSupportFmt->stVideoBitRate[1].nMin = 16000;
    lpstSupportFmt->stVideoBitRate[1].nMax = 1024000;
    memcpy(lpstSupportFmt->stVideoSize[1], stMirorVideoSize, sizeof(stMirorVideoSize)); 	//Video Size(height,width)

    QMAPI_SIZE stMobileVideoSize;
    stMobileVideoSize.nWidth = 640;
    stMobileVideoSize.nHeight = 360;

    lpstSupportFmt->dwVideoSupportFmt[2][0] = QMAPI_PT_H264;		//Video Format.
    lpstSupportFmt->stVideoBitRate[2].nMin = 16000;
    lpstSupportFmt->stVideoBitRate[2].nMax = 512000;
    memcpy(lpstSupportFmt->stVideoSize[2], &stMobileVideoSize, sizeof(stMobileVideoSize));		//Video Size(height,width)


    lpstSupportFmt->dwAudioFmt[0] = QMAPI_PT_G711A;
    lpstSupportFmt->dwAudioSampleRate[0] = QMAPI_AUDIO_SAMPLE_RATE_8000;
    i = 0;
    for(i = 0 ; i < QMAPI_MAX_STREAMNUM ; i ++)
    {
        lpstSupportFmt->stMAXFrameRate[i] = 25;
        lpstSupportFmt->stMINFrameRate[i] = 1;
    }
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return 0;
}   
//format ipaddr from "010" to "10"   --vinsent
int tl_ipaddr_format( char * addr)
{

	int ret = 0;
	struct in_addr inaddr ;
	memset(&inaddr , 0 , sizeof(struct in_addr));
	ret = inet_aton(addr , &inaddr);
	if( 0 == ret)
	{
		printf("###@@@FUN:%s LINE: %d Addr Bad " , __FUNCTION__ , __LINE__ );
		return 1;
	}
	char *tmpaddr = NULL;
	tmpaddr = inet_ntoa(inaddr);
	if(NULL == tmpaddr)
	{
		printf("###@@@FUN:%s LINE: %d Addr NULL " , __FUNCTION__ , __LINE__);
		return 2;
	}
	strcpy(addr , tmpaddr);
	
	return 0;
}


