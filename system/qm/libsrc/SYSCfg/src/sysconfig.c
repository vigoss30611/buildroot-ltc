
#include <stdio.h>

#include <qsdk/sys.h>
#include "sysconfig.h"
#include "confile.h"
#include "tlmisc.h"
#include "tlnettools.h"
#include "Default_Set.h"

#include "QMAPINetWork.h"
#include "tl_authen_interface.h"
#include "video.h"

extern int Get_Q3_Vi_Format(int w , int h, DWORD *dwVFormat);
extern void infotm_video_info_init(void);

syscfg_default_param enSysDefaultParam;

DWORD GetSystemVersionFlag()
{
    DWORD dwQsdkVersion = 0;
    char version[16] = {0};
    int v1, v2, v3, v4;

    if(!system_get_version(version))
        dwQsdkVersion = atoi(version);

    sscanf(version, "%02d%02d%02d%02d", &v1, &v2, &v3, &v4);

    dwQsdkVersion = 1<<24 | v2<<16 | v3<<8 | v4;
    return dwQsdkVersion;
}

//参数pIp为xxx.xxx.xxx.xxx格式,失败返回0
unsigned long TL_GetIpValueFromString(const char *pIp)
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
int TL_GetAddrNum(const char *pIp, unsigned int *num1,unsigned int *num2,unsigned int *num3,unsigned int *num4)
{
    int ret = 0;
    unsigned int uiV1 = 0,uiV2 = 0,uiV3 = 0,uiV4 = 0;
    if(NULL == pIp || NULL == num1 || NULL == num2 || NULL == num3 || NULL == num4)
    {
        return -1;
    }

    sscanf(pIp,"%d.%d.%d.%d",&uiV1,&uiV2,&uiV3,&uiV4);
    if(ret == 4)
    {
        *num1 = uiV1;
        *num2 = uiV2;
        *num3 = uiV3;
        *num4 = uiV4;
        return 0;
    }
    return -2;
}
int TL_GetIP_From_CfgFile(const char *pFileName,unsigned char *ip)
{
    INI_CONFIG *config = NULL;
    char *pValue = NULL;
    char *pDefaultValue = "no value";
    if(NULL == pFileName || NULL == ip)
    {
        printf("%s: pFileName == NULL!\n",__FUNCTION__);
        return -1;
    }
    config = ini_config_create_from_file(pFileName, 0);
    if(NULL == config)
    {
        printf("%s: ini_config_create_from_file failure, name=%s!\n",__FUNCTION__,pFileName);
        return -2;
    }
    int nValue = -1;
    int nDefaultValue = -1;
    nValue = ini_config_get_int(config,"PRODUCT_IP_SET","DEFAULT",nDefaultValue);
    if(nValue == 0)
    {    
        pValue = ini_config_get_string(config,"PRODUCT_IP_SET","IP",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("%s-%d: pValue=%s!\n",__FUNCTION__,__LINE__,pValue);
            strcpy((char*)ip, pValue);
            ini_config_destroy(config);
            return 0;
        }
    }
    ini_config_destroy(config);
    return -3;
}
int TL_GetMac_From_CfgFile(const char *pFileName,unsigned char *pMac)
{
    INI_CONFIG *config = NULL;
    char *pValue = NULL;
    char *pDefaultValue = "no value";
    int szCfgMac[6] = {0,0,0,0,0,0};
    int i = 0;
    if(NULL == pFileName || NULL == pMac)
    {
        printf("%s: pFileName == NULL!\n",__FUNCTION__);
        return -1;
    }
    config = ini_config_create_from_file(pFileName, 0);
    if(NULL == config)
    {
        printf("%s: ini_config_create_from_file failure, name=%s!\n",__FUNCTION__,pFileName);
        return -2;
    }
    int nValue = -1;
    int nDefaultValue = -1;
    nValue = ini_config_get_int(config,"DEVICE_MAC","DEFAULT",nDefaultValue);
    if(nValue == 0)
    {    
        pValue = ini_config_get_string(config,"DEVICE_MAC","MAC",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("%s pValue=%s!\n",__FUNCTION__,pValue);
            sscanf(pValue,"%x:%x:%x:%x:%x:%x",
                &szCfgMac[0],&szCfgMac[1],&szCfgMac[2],&szCfgMac[3],&szCfgMac[4],&szCfgMac[5]);
            for(i = 0; i < 6; i++)
            {
                pMac[i] = szCfgMac[i];
            }
            ini_config_destroy(config);
            return 0;
        }
    }
    ini_config_destroy(config);
    return -3;
}
int TL_UpdateCfgFile(const char *pFileName,const char* section,const char* key,int key_len,const char* value,int value_len)
{
    INI_CONFIG *config = NULL;
    if(NULL == pFileName)
    {
        printf("%s: pFileName == NULL!\n",__FUNCTION__);
        return -1;
    }
    config = ini_config_create_from_file(pFileName, 0);
    if(NULL == config)
    {
        printf("%s: ini_config_create_from_file failure, name=%s!\n",__FUNCTION__,pFileName);
        return -2;
    }
    ini_config_set_string(config,section,key,key_len,value,value_len);
    ini_config_save(config, pFileName);
    ini_config_destroy(config);
    config = NULL;
    return 0;
}
int TL_GetDefaultValue_From_CfgFile(const char *pFileName)
{
    INI_CONFIG *config = NULL;
    int nValue = 0;
    int nDefaultValue = -1;
    char *pValue = NULL;
    char *pDefaultValue = "no value";
    int i = 0;
    if(NULL == pFileName)
    {
        printf("%s: pFileName == NULL!\n",__FUNCTION__);
        return -1;
    }

    config = ini_config_create_from_file(pFileName, 0);
    if(NULL == config)
    {
        printf("%s: ini_config_create_from_file failure, name=%s!\n",__FUNCTION__,pFileName);
        return -2;
    }
    
    nValue = ini_config_get_int(config,"VIDEOSTARDAD","DEFAULT",nDefaultValue);
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"VIDEOSTARDAD","INDEX", g_tl_globel_param.stDevicInfo.byVideoStandard);
        if(nValue != g_tl_globel_param.stDevicInfo.byVideoStandard)
        {
            printf("bVideoStandard -%d!\n",nValue);
            g_tl_globel_param.stDevicInfo.byVideoStandard = nValue;
        }

    }
    //设备名称是否用设备默认的参数
    nValue = ini_config_get_int(config,"NAME","DEFAULT",nDefaultValue);
    if(nValue == 0)
    {
        //获取设备名称
        pValue = ini_config_get_string(config,"NAME","DEVICENAME",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("NAME-DEVICENAME-%s!\n",pValue);
            strcpy(g_tl_globel_param.stDevicInfo.csDeviceName,pValue);
        }
    }

    //设备名称是否用设备默认的参数
    nValue = ini_config_get_int(config,"DEVICE_MAC","DEFAULT",nDefaultValue);
    printf("DEVICE_MAC-DEFAULT-%d!\n",nValue);
    if(nValue == 0)
    {
        //获取设备名称
        pValue = ini_config_get_string(config,"DEVICE_MAC","MAC",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            int szCfgMac[6] = {0,0,0,0,0,0};
            unsigned char pMac[6] = {0,0,0,0,0,0};
            
            printf("%s pValue=%s!\n",__FUNCTION__,pValue);
            sscanf(pValue,"%x:%x:%x:%x:%x:%x",&szCfgMac[0],&szCfgMac[1],&szCfgMac[2],&szCfgMac[3],&szCfgMac[4],&szCfgMac[5]);
            
            for(i = 0; i < 6; i++)
            {
                pMac[i] = szCfgMac[i];
            }  
            
            if(pMac[0] == 0x00)
            {
                memcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr,pMac, 6);
            }
            
        }
    }    

    //网络参数是否用设备默认的参数
    nValue = ini_config_get_int(config,"PRODUCT_IP_SET","DEFAULT",nDefaultValue);
    if(nValue == 0)
    {
        //获取DHCP
        nValue = ini_config_get_int(config,"PRODUCT_IP_SET","ISDHCP",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("PRODUCT_IP_SET-ISDHCP-%d!\n",nValue);
            g_tl_globel_param.stNetworkCfg.byEnableDHCP = nValue;
        }
        //获取IP
        pValue = ini_config_get_string(config,"PRODUCT_IP_SET","IP",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PRODUCT_IP_SET-IP-%s!\n",pValue);
            strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4,pValue);
        }
        //获取子网掩码
        pValue = ini_config_get_string(config,"PRODUCT_IP_SET","SUBNET",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PRODUCT_IP_SET-SUBNET-%s!\n",pValue);
            strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4,pValue);
        }
        //获取网关
        pValue = ini_config_get_string(config,"PRODUCT_IP_SET","GATE",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PRODUCT_IP_SET-GATE-%s!\n",pValue);
            strcpy(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4,pValue);
        }
        //获取DNS
        pValue = ini_config_get_string(config,"PRODUCT_IP_SET","DNS",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PRODUCT_IP_SET-DNS-%s!\n",pValue);
            strcpy(g_tl_globel_param.stNetworkCfg.stDnsServer1IpAddr.csIpV4,pValue);
        }
    }
    
    //获取视频制式-0:N制,1:P制
    nValue = ini_config_get_int(config,"VIDEOSTARDAD","DEFAULT",nDefaultValue);
    printf("VIDEOSTARDAD-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"VIDEOSTARDAD","INDEX",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("VIDEOSTARDAD-INDEX-%d!\n",nValue);
            g_tl_globel_param.stDevicInfo.byVideoStandard = nValue;
        }
    }

    //获取DDNS
    nValue = ini_config_get_int(config,"DDNS","DEFAULT",nDefaultValue);
    printf("DDNS-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"DDNS","ISDDNS",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("DDNS-ISDDNS-%d!\n",nValue);
            g_tl_globel_param.stDDNSCfg.bEnableDDNS = nValue;
        }
        //获取DDNS-enDDNSType
        nValue = ini_config_get_int(config,"DDNS","PROVIDER",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("DDNS-PROVIDER-%d!\n",nValue);
            g_tl_globel_param.stDDNSCfg.stDDNS[0].enDDNSType = nValue;
        }
        //获取DDNS-address
        pValue = ini_config_get_string(config,"DDNS","ADDRESS",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("DDNS-ADDRESS-%s!\n",pValue);
            strcpy(g_tl_globel_param.stDDNSCfg.stDDNS[0].csDDNSAddress,pValue);
        }
        //获取DDNS-port
        nValue = ini_config_get_int(config,"DDNS","PORT",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("DDNS-PORT-%d!\n",nValue);
	     g_tl_globel_param.stDDNSCfg.stDDNS[0].nDDNSPort = nValue;
        }
        //获取DDNS-domainname
        pValue = ini_config_get_string(config,"DDNS","DOMINNAME",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("DDNS-DOMINNAME-%s!\n",pValue);
            strcpy(g_tl_globel_param.stDDNSCfg.stDDNS[0].csDDNSDomain,pValue);
        }
    }
   
    //获取UPNP
    nValue = ini_config_get_int(config,"UPNP","DEFAULT",nDefaultValue);
    printf("UPNP-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"UPNP","ISUPNP",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("UPNP-ISUPNP-%d!\n",nValue);
            g_tl_globel_param.tl_upnp_config.bEnable = nValue;
        }
    }

    //获取WIFI
    nValue = ini_config_get_int(config,"WIFI","DEFAULT",nDefaultValue);
    printf("WIFI-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"WIFI","TYPE",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("WIFI-TYPE-%d!\n",nValue); 
            g_tl_globel_param.tl_Wifi_config.bWifiEnable = nValue; 
        }
        //获取WIFI-IP
        pValue = ini_config_get_string(config,"WIFI","IP",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            unsigned long ulX = TL_GetIpValueFromString((const char*)pValue);
            printf("WIFI-IP-%s!\n",pValue);
            if(0 != ulX )
            {
                g_tl_globel_param.tl_Wifi_config.dwNetIpAddr = ulX;
            }
        }
        //获取WIFI-子网掩码
        pValue = ini_config_get_string(config,"WIFI","SUBNET",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            unsigned long ulX = TL_GetIpValueFromString((const char*)pValue);
            printf("WIFI-SUBNET-%s!\n",pValue);
            if(0 != ulX)
            {
                g_tl_globel_param.tl_Wifi_config.dwNetMask = ulX;
            }
        }
        //获取WIFI-网关
        pValue = ini_config_get_string(config,"WIFI","GATE",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            unsigned long ulX = TL_GetIpValueFromString((const char*)pValue);
            printf("WIFI-GATE-%s!\n",pValue);
            if(0 != ulX )
            {
                g_tl_globel_param.tl_Wifi_config.dwGateway = ulX;
            }
        }
        //获取WIFI-DNS
        pValue = ini_config_get_string(config,"WIFI","DNS",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            unsigned long ulX = TL_GetIpValueFromString((const char*)pValue);
            printf("WIFI-DNS-%s!\n",pValue);
            if(0 != ulX )
            {
                g_tl_globel_param.tl_Wifi_config.dwDNSServer = ulX;
            }
        }
    }

    //获取PLATFORM
     nValue = ini_config_get_int(config,"PLATFORM","DEFAULT",nDefaultValue);
    printf("PLATFORM-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
        nValue = ini_config_get_int(config,"PLATFORM","ISON",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("PLATFORM-ISON-%d!\n",nValue); 
            g_tl_globel_param.stNetPlatCfg.bEnable = nValue; 
        }
        //获取PLATFORM-ip
        pValue = ini_config_get_string(config,"PLATFORM","IP",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PLATFORM-IP-%s!\n",pValue); 
            strcpy(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4,pValue); 
        }
        //获取PLATFORM-port
        nValue = ini_config_get_int(config,"PLATFORM","PORT",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("PLATFORM-PORT-%d!\n",nValue); 
            g_tl_globel_param.stNetPlatCfg.dwCenterPort = nValue; 
        }
        //获取PLATFORM-type
        pValue = ini_config_get_string(config,"PLATFORM","TYPE",pDefaultValue);
        if(0 != strcmp(pValue, pDefaultValue))
        {
            printf("PLATFORM-TYPE-%s!\n",pValue); 
            g_tl_globel_param.stNetPlatCfg.enPlatManufacture = nValue; 
        }
    }

    //颜色设置-亮度
    nValue = ini_config_get_int(config,"COLOR_PARA","DEFAULT",nDefaultValue);
    printf("COLOR_PARA-DEFAULT-%d!\n",nValue);    
    if(nValue == 0)
    {
         nValue = ini_config_get_int(config,"COLOR_PARA","BRIGHTNESS",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("COLOR_PARA-BRIGHTNESS-%d!\n",nValue);
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                g_tl_globel_param.TL_Channel_Color[i].nBrightness = nValue;
            } 
        }
        //颜色设置-对比度
        nValue = ini_config_get_int(config,"COLOR_PARA","CONTRAST",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("COLOR_PARA-CONTRAST-%d!\n",nValue); 
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                g_tl_globel_param.TL_Channel_Color[i].nContrast = nValue;
            } 
        }
        //颜色设置-饱和度
        nValue = ini_config_get_int(config,"COLOR_PARA","SATURATION",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("COLOR_PARA-SATURATION-%d!\n",nValue); 
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                g_tl_globel_param.TL_Channel_Color[i].nSaturation = nValue;
            } 
        }
        //颜色设置-色度
        nValue = ini_config_get_int(config,"COLOR_PARA","HUE",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("COLOR_PARA-HUE-%d!\n",nValue); 
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                g_tl_globel_param.TL_Channel_Color[i].nHue = nValue;
            } 
        }
        //颜色设置-锐度
        nValue = ini_config_get_int(config,"COLOR_PARA","DEFINITION",nDefaultValue);
        if(nValue != nDefaultValue)
        {
            printf("COLOR_PARA-DEFINITION-%d!\n",nValue); 
            for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
            {
                g_tl_globel_param.TL_Channel_Color[i].nDefinition = nValue;
            } 
        }
    }
   

    ini_config_destroy(config);
    config = NULL;
    return 0;
}

int TL_Default_DeviceInfo_Param(QMAPI_NET_DEVICE_INFO *pstDeviceInfo)
{
    if(NULL == pstDeviceInfo)
        return -1;
    TLNV_SYSTEM_INFO sinfo = {0};
    
    pstDeviceInfo->dwSize = sizeof(QMAPI_NET_DEVICE_INFO);
    sprintf(pstDeviceInfo->csDeviceName,"%s",DEVICE_SERVER_NAME); 
    pstDeviceInfo->byLanguage        = QMAPI_LANGUAGE_CHINESE_SIMPLIFIED;
    pstDeviceInfo->byRecycleRecord   = 1;
	pstDeviceInfo->byRecordLen       = 10;
    pstDeviceInfo->byDateFormat      = 0;//xxxx-xx-xx
    pstDeviceInfo->byDateSprtr       = 1;
    pstDeviceInfo->byTimeFmt         = 0;
    pstDeviceInfo->dwSoftwareVersion = GetSystemVersionFlag();
    pstDeviceInfo->dwSoftwareBuildDate = strtoul(__BUILD_DATE__, NULL, 16);
    pstDeviceInfo->byVideoStandard     = videoStandardPAL;
    pstDeviceInfo->byVideoInputType    = enSysDefaultParam.enInputType;
    pstDeviceInfo->dwServerCPUType     = CPUTYPE_Q3F;

    /*will be covered with info read from encrypt */
    pstDeviceInfo->dwServerType        = IPC_FLAG;
    pstDeviceInfo->byVideoInNum        = 1;
    pstDeviceInfo->byAudioInNum        = 0;
    pstDeviceInfo->byAlarmInNum        = 0;
    pstDeviceInfo->byAlarmOutNum       = 0;
    sprintf((char *)(pstDeviceInfo->csSerialNumber), "%c%02X%02u-%02X%02X%02X%02X-%u%02u%lu",
            0, 0, pstDeviceInfo->byVideoInNum,
            0, 0, 0, 0,
            0, 0);

    if (encrypt_info_read((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO)) == 0)
    {
        unsigned short woCheckSum = sinfo.woCheckSum;
        sinfo.woCheckSum = sinfo.cbSize;
        if (encrypt_info_check((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO), woCheckSum) == 0)
        {
            pstDeviceInfo->dwServerType        = IPC_FLAG;
            pstDeviceInfo->byVideoInNum        = sinfo.byChannelNum ? sinfo.byChannelNum : 1;
            pstDeviceInfo->byAudioInNum        = sinfo.byAudioInputNum;
            pstDeviceInfo->byAlarmInNum        = sinfo.byAlarmInputNum;
            pstDeviceInfo->byAlarmOutNum       = sinfo.byAlarmOutputNum;
            sprintf((char *)(pstDeviceInfo->csSerialNumber), "%c%02X%02u-%02X%02X%02X%02X-%u%02u%lu",
                sinfo.byType, sinfo.byManufacture, pstDeviceInfo->byVideoInNum,
                sinfo.byHardware[0], sinfo.byHardware[1], sinfo.byHardware[2], sinfo.byHardware[3],
                (unsigned short)(sinfo.byIssueDate[0] << 8 | sinfo.byIssueDate[1]), (unsigned short)sinfo.byIssueDate[2], sinfo.dwSerialNumber);
        }
    }
    return 0;
}


int TL_Default_Channel_Encode_Param(QMAPI_NET_CHANNEL_PIC_INFO *pChnPicInfo, int Channel)
{
    QMAPI_NET_COMPRESSION_INFO  *strCompressionPara = NULL;

    if(NULL == pChnPicInfo)
        return -1;
    {
        int width = 0;
        int height = 0;
        int fps = 0;
        DWORD dwVFormat = QMAPI_VIDEO_FORMAT_1080P;
        
        memset(pChnPicInfo, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
        pChnPicInfo->dwSize 	= sizeof(QMAPI_NET_CHANNEL_PIC_INFO);
        pChnPicInfo->dwChannel	= Channel;
        sprintf((char *)pChnPicInfo->csChannelName, "%s%02d", AV_CHANNEL_INFO_NAME, Channel+1); 

        strCompressionPara = &pChnPicInfo->stRecordPara;
        strCompressionPara->dwCompressionType = QMAPI_PT_H264;//
        //strCompressionPara->dwCompressionType = QMAPI_PT_H265;//

        ven_info_t *ven_info = infotm_video_info_get(0);
        if (!ven_info) {
            printf("%s cannot get ven_info for sub stream\n", __FUNCTION__);
            width   = 1920;
            height  = 1080;
            fps     = 25; 
        } else {
            width   = ven_info->width;
            height  = ven_info->height;
            fps     = ven_info->fps;
            if (Get_Q3_Vi_Format(width, height, &dwVFormat) < 0) { 
                printf("%s cannot Get vi format\n", __FUNCTION__);
            } 
        }

        strCompressionPara->dwStreamFormat = dwVFormat;
        strCompressionPara->wWidth = width;
        strCompressionPara->wHeight = height;
        strCompressionPara->dwFrameRate = fps;//帧率 (1-25/30) PAL为25，NTSC为30
        strCompressionPara->dwBitRate = 3*1000*1000;

        printf("+++++++++++++++++ dwStreamFormat:%ld\n", strCompressionPara->dwStreamFormat);
        strCompressionPara->dwRateType = VIDEO_ENCODE_CBR;			//流模式(0为定码流，1为变码流)
        strCompressionPara->dwImageQuality = VIDEO_ENCODE_DEFUALT_VBR_QUALITY;		//编码质量(0-4),0为最好
        strCompressionPara->dwMaxKeyInterval = fps*5;	//关键帧间隔(1-100)

        strCompressionPara->wEncodeAudio = 1;		//是否编码音频
        strCompressionPara->wEncodeVideo = 1;		//是否编码视频
        strCompressionPara->wFormatTag = QMAPI_PT_G711A;		  /* format type */
        strCompressionPara->wBitsPerSample = QMAPI_AUDIO_SAMPLE_RATE_8000;	/* Number of bits per sample of mono data */
        ////////////////////////////////////

        strCompressionPara = &pChnPicInfo->stNetPara;
        strCompressionPara->dwCompressionType = QMAPI_PT_H264;//
        //strCompressionPara->dwCompressionType = QMAPI_PT_H265;//

        dwVFormat = QMAPI_VIDEO_FORMAT_VGA;
        ven_info = infotm_video_info_get(1);
        if (!ven_info) {
            printf("%s cannot get ven_info for sub stream\n", __FUNCTION__);
            width   = 1920;
            height  = 1080; 
            fps     = 25;
        } else {
            width   = ven_info->width;
            height  = ven_info->height;
            fps     = ven_info->fps;
            if (Get_Q3_Vi_Format(width, height, &dwVFormat) < 0) { 
                printf("%s cannot Get vi format %d*%d\n", __FUNCTION__, width, height);
            } 
        }

        strCompressionPara->dwStreamFormat = dwVFormat;
        strCompressionPara->wWidth = width;
        strCompressionPara->wHeight = height;
        strCompressionPara->dwFrameRate = fps;//帧率 (1-25/30) PAL为25，NTSC为30 
        strCompressionPara->dwBitRate = 600*1000;//码率 (16000-4096000)

        strCompressionPara->dwRateType = VIDEO_ENCODE_CBR;//流模式(0为定码流，1为变码流)
        strCompressionPara->dwImageQuality = VIDEO_ENCODE_DEFUALT_VBR_QUALITY;//编码质量(0-4),0为最好
        strCompressionPara->dwMaxKeyInterval = fps*5;//关键帧间隔(1-100)

        strCompressionPara->wEncodeAudio = 1;//是否编码音频
        strCompressionPara->wEncodeVideo = 1;//是否编码视频

        strCompressionPara->wFormatTag = QMAPI_PT_G711A;/* format type */
        strCompressionPara->wBitsPerSample = QMAPI_AUDIO_SAMPLE_RATE_8000;/* Number of bits per sample of mono data */
    }
    
    return 0;
}

int TL_Default_MotionDetect_Param(TL_CHANNEL_MOTION_DETECT *pChnMotionDect, int Channel)
{
    int j;
    if(NULL == pChnMotionDect)
        return -1;

/*RM#2836: default motion detect is disabled.    henry.li    2017/05/09*/
#if 0
    //默认全时段/全区域开启侦测
    {
        pChnMotionDect->dwSize      = sizeof(TL_CHANNEL_MOTION_DETECT);
        pChnMotionDect->dwChannel   = Channel;
        pChnMotionDect->bEnable     = 1;    //默认开启
        pChnMotionDect->nDuration   = DEFUALT_MOTION_DETECT_CLEAR_TIME;
        pChnMotionDect->dwSensitive = DEFUALT_MOTION_DETECT_SENSITIVE;
        for(j=0;j<8;j++)
        {
            pChnMotionDect->tlScheduleTime[j].bEnable=0;

            pChnMotionDect->tlScheduleTime[j].BeginTime1.bHour=0;
            pChnMotionDect->tlScheduleTime[j].BeginTime1.bMinute=0;
            pChnMotionDect->tlScheduleTime[j].EndTime1.bHour=23;
            pChnMotionDect->tlScheduleTime[j].EndTime1.bMinute=59;

            pChnMotionDect->tlScheduleTime[j].BeginTime2.bHour=0;
            pChnMotionDect->tlScheduleTime[j].BeginTime2.bMinute=0;
            pChnMotionDect->tlScheduleTime[j].EndTime2.bHour=23;
            pChnMotionDect->tlScheduleTime[j].EndTime2.bMinute=59;
        }

        pChnMotionDect->tlScheduleTime[7].bEnable=1; /*RM#2836: default ALL DAYS schedule time is Enabled.    henry.li    2017/05/09*/
        memset(pChnMotionDect->pbMotionArea,0x1,sizeof(pChnMotionDect->pbMotionArea));
    }
#else
    {
        pChnMotionDect->dwSize      = sizeof(TL_CHANNEL_MOTION_DETECT);
        pChnMotionDect->dwChannel   = Channel;
        pChnMotionDect->bEnable     = 0;    //默认关闭
        pChnMotionDect->nDuration   = DEFUALT_MOTION_DETECT_CLEAR_TIME;
        pChnMotionDect->dwSensitive = DEFUALT_MOTION_DETECT_SENSITIVE;
        for(j=0;j<8;j++)
        {
            pChnMotionDect->tlScheduleTime[j].bEnable=0;

            pChnMotionDect->tlScheduleTime[j].BeginTime1.bHour=0;
            pChnMotionDect->tlScheduleTime[j].BeginTime1.bMinute=0;
            pChnMotionDect->tlScheduleTime[j].EndTime1.bHour=23;
            pChnMotionDect->tlScheduleTime[j].EndTime1.bMinute=59;

            pChnMotionDect->tlScheduleTime[j].BeginTime2.bHour=0;
            pChnMotionDect->tlScheduleTime[j].BeginTime2.bMinute=0;
            pChnMotionDect->tlScheduleTime[j].EndTime2.bHour=23;
            pChnMotionDect->tlScheduleTime[j].EndTime2.bMinute=59;
        }
        memset(pChnMotionDect->pbMotionArea,0x1,sizeof(pChnMotionDect->pbMotionArea));
    }

#endif

    return 0;
}

int TL_Default_SensorIn_Param(TL_SENSOR_ALARM *pSensroAlarm, int Channel)
{
    int j;
    if (NULL == pSensroAlarm)
        return -1;
    {
        pSensroAlarm->dwSize          = sizeof(TL_SENSOR_ALARM);
        pSensroAlarm->nSensorID       = Channel;
        pSensroAlarm->dwSensorType    = DEFUALT_ALARM_SENSOR_TYPE;
        pSensroAlarm->nDuration       = DEFUALT_ALARM_SENSOR_CLEAR_TIME;
        pSensroAlarm->nCaptureChannel = DEFUALT_ALARM_SENSOR_SHOT_CHANNEL;
        pSensroAlarm->bEnable         = TRUE;
        if(Channel<(TL_MAXSENSORNUM/2))
        {
            sprintf(pSensroAlarm->szSensorName , "%s-%d", ALARM_SENSOR_IN_NAME ,Channel+1);
        }
        else
        {
            sprintf(pSensroAlarm->szSensorName , "%s-%d",ALARM_SENSOR_OUT_NAME,Channel+1-(TL_MAXSENSORNUM/2));  
        }
        for(j=0;j<8;j++)
        {
            pSensroAlarm->tlScheduleTime[j].BeginTime1.bHour=0;
            pSensroAlarm->tlScheduleTime[j].BeginTime1.bMinute=0;

            pSensroAlarm->tlScheduleTime[j].EndTime1.bHour=23;
            pSensroAlarm->tlScheduleTime[j].EndTime1.bMinute=59;

            pSensroAlarm->tlScheduleTime[j].BeginTime2.bHour=0;
            pSensroAlarm->tlScheduleTime[j].BeginTime2.bMinute=0;
            
            pSensroAlarm->tlScheduleTime[j].EndTime2.bHour=23;
            pSensroAlarm->tlScheduleTime[j].EndTime2.bMinute=59;
        }
    }

    return 0;
}


int TL_Default_CB_Detection(QMAPI_NET_DAY_NIGHT_DETECTION_EX *pData, int Channel)
{
    if(NULL == pData || Channel < 0 || Channel >= QMAPI_MAX_CHANNUM)
    {
        return -1;
    }
    memset(pData, 0, sizeof(*pData));
    pData->dwSize = sizeof(*pData);
    pData->dwChannel = Channel;
    pData->byMode = QMAPI_DN_DT_MODE_IR;//默认
    pData->byTrigger = 0;//低电平有效
    pData->byIRCutLevel= 1;//高电平有效
    pData->byLedLevel = 0;//低电平有效
    pData->byAGCSensitivity = 3;//灵敏度
    pData->byDelay = 3;//默认延时3秒
    pData->stColorTime.byStartHour = 7;
    pData->stColorTime.byStartMin  = 0;
    pData->stColorTime.byStopHour  = 18;
    pData->stColorTime.byStopMin   = 0;
    return 0;
}

int TL_Default_Network_Param(QMAPI_NET_NETWORK_CFG *pstNetworkCfg)
{
    if(NULL == pstNetworkCfg)
        return -1;
    TLNV_SYSTEM_INFO sinfo;

    /*will be covered with info read from encrypt */
    char mac[8] = {0};
    char mac_addr[24] ={0};
    get_net_phyaddr(QMAPI_ETH_DEV, mac_addr, mac);
    printf("get mac:%02x %02x %02x %02x %02x %02x \n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    memcpy(pstNetworkCfg->stEtherNet[0].byMACAddr, mac, 6);

    if (encrypt_info_read((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO)) == 0)
    {
        unsigned short woCheckSum = sinfo.woCheckSum;
        sinfo.woCheckSum = sinfo.cbSize;
        if (encrypt_info_check((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO), woCheckSum) == 0)
        {
            if (sinfo.byMac[0]==0 && sinfo.byMac[1]==0 && sinfo.byMac[2]==0
                && sinfo.byMac[3]==0 && sinfo.byMac[4]==0 && sinfo.byMac[5]==0)
            {
                unsigned char byMac[6];
                
                byMac[0] = 0x48;
                byMac[1] = sinfo.byManufacture;
                byMac[2] = (BYTE)(sinfo.dwSerialNumber >> 24 & 0xff); 
                byMac[3] = (BYTE)(sinfo.dwSerialNumber >> 16 & 0xff); 
                byMac[4] = (BYTE)(sinfo.dwSerialNumber >> 8 & 0xff);
                byMac[5] = (BYTE)(sinfo.dwSerialNumber & 0xff);
                memcpy(pstNetworkCfg->stEtherNet[0].byMACAddr, byMac, 6);
            }
            else
            {
                memcpy(pstNetworkCfg->stEtherNet[0].byMACAddr, sinfo.byMac, 6);
            }
            printf("@@@@ %s. %d.  %02x-%02x-%02x-%02x-%02x-%02x. \n",__FUNCTION__,__LINE__,pstNetworkCfg->stEtherNet[0].byMACAddr[0],
                pstNetworkCfg->stEtherNet[0].byMACAddr[1],pstNetworkCfg->stEtherNet[0].byMACAddr[2],pstNetworkCfg->stEtherNet[0].byMACAddr[3],
                pstNetworkCfg->stEtherNet[0].byMACAddr[4],pstNetworkCfg->stEtherNet[0].byMACAddr[6]);
        }
    }
    else
    {
        char mac[8] = {0};
        char mac_addr[24] ={0};
        get_net_phyaddr(QMAPI_ETH_DEV, mac_addr, mac);
        printf("get mac:%02x %02x %02x %02x %02x %02x \n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        memcpy(pstNetworkCfg->stEtherNet[0].byMACAddr, mac, 6);
    }
    
    pstNetworkCfg->dwSize = sizeof(QMAPI_NET_NETWORK_CFG);
    strcpy(pstNetworkCfg->stEtherNet[0].strIPAddr.csIpV4,NVSDEFAULTIP);
    strcpy(pstNetworkCfg->stEtherNet[0].strIPMask.csIpV4,NVSDEFAULTNETMASK);
    strcpy(pstNetworkCfg->stGatewayIpAddr.csIpV4,NVSDEFAULTGATEWAY);

    /* RM#1733: default use Static IP. henry.li 2017/02/10 */
    /* pstNetworkCfg->byEnableDHCP = 1; */
    pstNetworkCfg->byEnableDHCP = 0;
    pstNetworkCfg->wHttpPort = SER_DEFAULT_WEBPORT;
    pstNetworkCfg->stEtherNet[0].wDataPort = SER_DEFAULT_PORT;
    pstNetworkCfg->stEtherNet[0].dwNetInterface = 5;
    strcpy(pstNetworkCfg->stEtherNet[0].csNetName,ETH_DEV);
    pstNetworkCfg->stEtherNet[0].wMTU = 1500;
    strcpy(pstNetworkCfg->stDnsServer1IpAddr.csIpV4,NETWORK_DEFUALT_DNS);

    return 0;
}

int TL_Default_UserManagement_Param(TL_SERVER_USER *pstServerUser, int Index)
{
    ///TL_SERVER_USER 02
    //admin Super User
    if(NULL == pstServerUser)
        return -1;

	if (Index)
	{
		pstServerUser->dwSize  = sizeof(TL_SERVER_USER);
        pstServerUser->dwIndex = Index;
        pstServerUser->bEnable = 0;
        //sprintf(pstServerUser[i].csUserName,"%s%d",GENERAL_USER_NAME , i+1);
        //sprintf(pstServerUser[i].csPassword,"%s%d",CENERAL_USER_PASSWORD , i+1);
        pstServerUser->dwUserRight=0x0;
        pstServerUser->dwNetPreviewRight=0x0;
	}
	else
	{
		pstServerUser->dwSize=sizeof(TL_SERVER_USER);
		pstServerUser->dwIndex = Index;
		pstServerUser->bEnable =1;
		sprintf(pstServerUser->csUserName,"%s",ROOT_USER_NAME);
		strcpy(pstServerUser->csPassword,ROOT_USER_PASSWORD);
		//sprintf(g_tl_globel_param.TL_Server_User[0].csPassword,"%s",ROOT_USER_PASSWORD);
		pstServerUser->dwUserRight=ROOT_USER_FUNC_RIGHT;
		pstServerUser->dwNetPreviewRight=ROOT_USER_NET_PREVIEW_RIGHT;
	}
   
    return 0;
}

int TL_Default_ServerInfo_Param(TLNV_SERVER_INFO *pstServerInfo)
{
    if(NULL == pstServerInfo)
        return -1;

    pstServerInfo->dwSize=sizeof(TLNV_SERVER_INFO);
    pstServerInfo->dwServerFlag=IPC_FLAG;
    pstServerInfo->dwServerIp=inet_addr(NVSDEFAULTIP);
    sprintf(pstServerInfo->szServerIp , "%s" , NVSDEFAULTIP);
    pstServerInfo->wServerPort = SER_DEFAULT_WEBPORT;

    pstServerInfo->wChannelNum = 4;
    printf("#### %s %d, wChannelNum=%d\n", __FUNCTION__, __LINE__, pstServerInfo->wChannelNum);
    DEVICE_TYPE_E enDeviceType = DEVICE_MEGA_1080P;

    if ((enDeviceType == DEVICE_MEGA_IPCAM) 
        || (enDeviceType == DEVICE_MEGA_1080P) || (enDeviceType == DEVICE_MEGA_300M)
        || (enDeviceType == DEVICE_MEGA_960P)|| (enDeviceType == DEVICE_IPCAM_SVGA))
    {
        pstServerInfo->wChannelNum = 1;
    }

    printf("#### %s %d, wChannelNum=%d\n", __FUNCTION__, __LINE__, pstServerInfo->wChannelNum);

    pstServerInfo->dwVersionNum = GetSystemVersionFlag();
    sprintf(pstServerInfo->szServerName , "%s" , DEVICE_SERVER_NAME);

    pstServerInfo->dwServerCPUType   = CPUTYPE_Q3F;
    pstServerInfo->dwAlarmInCount    = 0;
    pstServerInfo->dwAlarmOutCount   = 0;
    pstServerInfo->dwSysFlags        = 9001;
    pstServerInfo->dwUserRight       = 0x0;
    pstServerInfo->dwNetPriviewRight = 0x0;

    return 0;
}

int TL_Default_LOGO_Param(TL_CHANNEL_LOGO *pstLOGO, int Channel)
{
    if(NULL == pstLOGO)
        return -1;
    {
        pstLOGO->dwSize    = sizeof(TL_CHANNEL_LOGO);
        pstLOGO->dwChannel = Channel;
        pstLOGO->bEnable   = 0;
        pstLOGO->dwLogox   = DEFUALT_LOGO_X_POS;
        pstLOGO->dwLogoy   =  DEFUALT_LOGO_Y_POS;

    }
    
    return 0;
}

int TL_Default_Shelter_Param(TL_CHANNEL_SHELTER *pstShelter, int Channel)
{
    if(NULL == pstShelter)
        return -1;
    {
        pstShelter->dwSize    = sizeof(TL_CHANNEL_SHELTER);
        pstShelter->dwChannel = Channel;
        pstShelter->bEnable   = 0;

    }
    
    return 0;
}

int TL_Default_RecordSched_Param(QMAPI_NET_CHANNEL_RECORD *pchn_record, int Channel)
{
    int k, z;
    if(NULL == pchn_record)
        return -1;

    {
        pchn_record->dwSize = sizeof(QMAPI_NET_CHANNEL_RECORD);
        pchn_record->dwChannel = Channel;
        pchn_record->dwRecord = 1;
        for (k=0; k<7; k++)
        {
            for (z=0; z<MAX_REC_SCHED; z++)
            {
                pchn_record->stRecordSched[k][z].byUser = 100;
                pchn_record->stRecordSched[k][z].byRecordType = QMAPI_NET_RECORD_TYPE_SCHED;
                pchn_record->stRecordSched[k][z].byStartHour = 0;
                pchn_record->stRecordSched[k][z].byStartMin = 0;
                pchn_record->stRecordSched[k][z].byEndHour = 23;
                pchn_record->stRecordSched[k][z].byEndMin = 59;
            }
        }
    }

    return 0;
}

int TL_Default_OSD_Param(TL_CHANNEL_OSDINFO *pstOSDInfo, TL_TIME_OSD_POS *pstTimeOSDPos, int Channel)
{
    if(pstOSDInfo)
    {
        pstOSDInfo->dwSize       = sizeof(TL_CHANNEL_OSDINFO);
        pstOSDInfo->dwChannel    = Channel;
        pstOSDInfo->bShowTime    = DEFUALT_MASK_SHOW_TIME;
        pstOSDInfo->dwTimeFormat = 0;

        if (g_tl_globel_param.stChannelPicParam[0].stRecordPara.dwStreamFormat == QMAPI_VIDEO_FORMAT_1080P)
        {
            pstOSDInfo->dwStringx    = 1920;
            pstOSDInfo->dwStringy    = 1080;
        }
        else if (g_tl_globel_param.stChannelPicParam[0].stRecordPara.dwStreamFormat == QMAPI_VIDEO_FORMAT_960P)
        {
            pstOSDInfo->dwStringx    = 1280;
            pstOSDInfo->dwStringy    = 960;
        }
        else if (g_tl_globel_param.stChannelPicParam[0].stRecordPara.dwStreamFormat == QMAPI_VIDEO_FORMAT_720P)
        {
            pstOSDInfo->dwStringx    = 1280;
            pstOSDInfo->dwStringy    = 720;
        }
        pstOSDInfo->bShowString = 1;
        sprintf(pstOSDInfo->csString, "IPCamera");
    }

    if(pstTimeOSDPos)
    {
        pstTimeOSDPos->dwTimeX = 10;
        pstTimeOSDPos->dwTimeY = 10;
    }

    return 0;
}

int TL_Default_OSDtypeData(TL_SERVER_OSD_DATA *pstOSDData, int Channel)
{
    if(NULL == pstOSDData)
        return -1;

    pstOSDData->dwSize    = sizeof(TL_SERVER_OSD_DATA);
    pstOSDData->dwChannel = Channel;

    return 0;
}

int TL_Default_RS485_Param(TL_SERVER_COMINFO_V2 *pstRS485, int Channel)
{
    if(NULL == pstRS485)
        return -1;
    {
        ///TL_SERVER_COMINFO 15
        pstRS485->dwSize     = sizeof(TL_SERVER_COMINFO_V2);
        pstRS485->dwChannel  = Channel;
        pstRS485->dwBaudRate = DEFUALT_COMINFO_1_BAUD;
        pstRS485->nHSpeed    = DEFUALT_PTZ_SPEED;
        pstRS485->nVSpeed    = DEFUALT_PTZ_SPEED;
        pstRS485->nDataBits  = DEFUALT_COMINFO_1_DATA_BITS;
        pstRS485->nStopBits  = DEFUALT_COMINFO_1_STOP_BITS;
        pstRS485->nPrePos    = DEFUALT_PTZ_PRSET_POS;
        pstRS485->nAddress   = DEFUALT_PTZ_ADDRESS;
        memcpy(pstRS485->szPTZName, "pelco-d", strlen("pelco-d"));
    }

    return 0;
}

int TL_Default_RS232_Param(TL_SERVER_COM2INFO *pstRS232, int Channel)
{
    if(NULL == pstRS232)
        return -1;

    {
        pstRS232->dwSize    = sizeof(TL_SERVER_COM2INFO);
        pstRS232->dwChannel = Channel;
        pstRS232->eBaudRate = DEFUALT_COMINFO_RS232_BAUD;
        pstRS232->nDataBits = DEFUALT_COMINFO_RS232_DATA_BITS;
        pstRS232->nStopBits = DEFUALT_COMINFO_RS232_STOP_BITS;
    }

    return 0;
}

int TL_Default_FTP_Param(TL_FTPUPDATA_PARAM *pstFtp)
{
    if(NULL == pstFtp)
        return -1;

    memset(pstFtp, 0, sizeof(TL_FTPUPDATA_PARAM));
    pstFtp->dwSize=sizeof(TL_FTPUPDATA_PARAM);
    pstFtp->dwFTPPort=DEFUALT_FTP_TRANSMIT_PORT;

    return 0;
}

int TL_Default_SMTP_Param(QMAPI_NET_EMAIL_PARAM *pstSMTP)
{
    if(NULL == pstSMTP)
        return -1;

    memset(pstSMTP, 0, sizeof(QMAPI_NET_EMAIL_PARAM));
    pstSMTP->dwSize = sizeof(QMAPI_NET_EMAIL_PARAM);
    pstSMTP->wServicePort = 25;

    return 0;
}

int TL_Default_NotifyServer_Param(TL_NOTIFY_SERVER *pstNotify)
{
    if(NULL == pstNotify)
        return -1;

    pstNotify->dwSize=sizeof(TL_NOTIFY_SERVER);
    pstNotify->dwTimeDelay=DEFUALT_NOTIFY_TIME_INTERVAL;
    pstNotify->dwPort=DEFUALT_NOTIFY_PORT;

    return 0;
}

int TL_Default_WirelessV1_Param(TL_WIFI_CONFIG *pstWifi)
{
    if(NULL == pstWifi)
        return -1;

    memset(pstWifi, 0, sizeof(TL_WIFI_CONFIG));
    pstWifi->dwSize = sizeof(TL_WIFI_CONFIG);
    pstWifi->byNetworkType = 1;
    pstWifi->bWifiEnable = 2;
    pstWifi->byWifiMode = 0;

    return 0;
}

int TL_Default_Wireless_Param(QMAPI_NET_WIFI_CONFIG *pstWifi)
{
    if(NULL == pstWifi)
        return -1;

    memset(pstWifi, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
    pstWifi->dwSize = sizeof(QMAPI_NET_WIFI_CONFIG);
    pstWifi->byNetworkType = 1;
    pstWifi->bWifiEnable = 1;
    pstWifi->byWifiMode = 0;
    pstWifi->byEnableDHCP = 1;

    return 0;
}

int TL_Default_UpNP_Param(TL_UPNP_CONFIG *pstUPNP)
{
    if(NULL == pstUPNP)
        return -1;

    memset(pstUPNP,0,sizeof(TL_UPNP_CONFIG));
    pstUPNP->bEnable = FALSE;
    pstUPNP->dwDataPort = SER_DEFAULT_PORT;
    pstUPNP->dwWebPort = SER_DEFAULT_WEBPORT;
    pstUPNP->dwMobilePort = SER_DEFAULT_MOBILEPORT;
    pstUPNP->dwSize = sizeof(TL_UPNP_CONFIG);

    return 0;
}

int TL_Default_RTSP_Param(QMAPI_NET_RTSP_CFG *pstrtsp)
{
    if(NULL == pstrtsp)
        return -1;

    memset(pstrtsp, 0, sizeof(QMAPI_NET_RTSP_CFG));
    pstrtsp->dwPort = 554;
    pstrtsp->dwSize = sizeof(QMAPI_NET_RTSP_CFG);

    return 0;
}

int TL_Default_DDNS_Param(QMAPI_NET_DDNSCFG *pstDDNS)
{
    int i;
    TLNV_SYSTEM_INFO sinfo;
    
    if(NULL == pstDDNS)
        return -1;
    
    memset(pstDDNS, 0, sizeof(QMAPI_NET_DDNSCFG));
    for(i=0; i<QMAPI_MAX_DDNS_NUMS; i++)
    {
        pstDDNS->stDDNS[i].enDDNSType = -1;
    }

    //TODO: set default value,then covered by info read from encrypt
    do {
        if (encrypt_info_read((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO)) == 0)
        {
            unsigned short woCheckSum = sinfo.woCheckSum;
            sinfo.woCheckSum = sinfo.cbSize;
            if (encrypt_info_check((char *)&sinfo, sizeof(TLNV_SYSTEM_INFO), woCheckSum) == 0)
            {
                break;
            }
        }
        memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
    }while(0);

    pstDDNS->dwSize = sizeof(QMAPI_NET_DDNSCFG);
    if(COMPANY_ANSHIBAO == sinfo.byManufacture)
    {
        pstDDNS->stDDNS[0].enDDNSType = DDNS_ANSHIBAO;
        strcpy(pstDDNS->stDDNS[0].csDDNSAddress, "see-world.cc");
        pstDDNS->stDDNS[0].nDDNSPort = 80;
    }
    else
    {
        pstDDNS->stDDNS[0].enDDNSType = DDNS_NOIP;
        strcpy(pstDDNS->stDDNS[0].csDDNSAddress, "no-ip.com");
		//strcpy(g_tl_globel_param.stDDNSCfg.stDDNS[0].csDDNSAddress, "ipcdvr.com");//安士佳//"dvripc.cn");
    }
    pstDDNS->stDDNS[0].nUpdateTime = 60*30; 
    unsigned char        *lpMac;
    lpMac = (unsigned char *)sinfo.byMac;
    sprintf(pstDDNS->stDDNS[0].csDDNSDomain, "%02X%02X%02X", lpMac[3],lpMac[4],lpMac[5]);
    pstDDNS->bEnableDDNS = TRUE;

    return 0;
}

int TL_Default_NTP_Param(QMAPI_NET_NTP_CFG *pstNTP)
{
    if(NULL == pstNTP)
        return -1;

    memset(pstNTP, 0, sizeof(QMAPI_NET_NTP_CFG));
    pstNTP->dwSize = sizeof(QMAPI_NET_NTP_CFG);
    pstNTP->wInterval = 3;
    pstNTP->byEnableNTP = 1;
    pstNTP->wNtpPort = 123;
/*  RM#1525: default NTP Server:  cn.pool.ntp.org
    strcpy((char*)pstNTP->csNTPServer, "time.nist.gov");
*/
    strcpy((char*)pstNTP->csNTPServer, "cn.pool.ntp.org");

    return 0;
}

int TL_Default_TIMEZONE_Param(QMAPI_NET_ZONEANDDST *pstZone)
{
    if(NULL == pstZone)
        return -1;

    memset(pstZone, 0, sizeof(QMAPI_NET_ZONEANDDST));
    pstZone->dwSize = sizeof(QMAPI_NET_ZONEANDDST);
    pstZone->nTimeZone = QMAPI_GMT_08;

    return 0;
}

int TL_Default_DeviceMaintain_Param(QMAPI_NET_DEVICEMAINTAIN *pstDeviceMaintain)
{
    if(NULL == pstDeviceMaintain)
        return -1;
    
    //设备维护
    memset(pstDeviceMaintain, 0, sizeof(QMAPI_NET_DEVICEMAINTAIN));
    pstDeviceMaintain->dwSize = sizeof(QMAPI_NET_DEVICEMAINTAIN);
#ifdef JIUAN_SUPPORT
    pstDeviceMaintain->byEnable = QMAPI_TRUE;
    pstDeviceMaintain->byIndex = 7;//每天
    pstDeviceMaintain->stDeviceMaintain.wHour = 2;
    pstDeviceMaintain->stDeviceMaintain.wMinute = 0;
#endif

    return 0;
}

int TL_Default_NetPlatCfg_Param(QMAPI_NET_PLATFORM_INFO_V2 *pstPlatForm)
{
    if(NULL == pstPlatForm)
        return -1;
    
    pstPlatForm->dwSize = sizeof(QMAPI_NET_PLATFORM_INFO_V2);
    pstPlatForm->bEnable = 1;
#ifdef JIUAN_SUPPORT
    pstPlatForm->enPlatManufacture = QMAPI_NETPT_JIUAN;
#endif

    return 0;
}


int TL_Default_ConsumerInfo_Param(QMAPI_NET_CONSUMER_INFO *pstConsumer)
{
    int i;
    if(NULL == pstConsumer)
        return -1;

    //消费类配置参数，该类配置为全功能配置的简化版。依赖于全功能配置
    memset(pstConsumer, 0, sizeof(QMAPI_NET_CONSUMER_INFO));
    pstConsumer->dwSize = sizeof(QMAPI_NET_CONSUMER_INFO);
    for(i = 0; i < 2; i++)  //0 public, 1 private
    {
        pstConsumer->stPrivacyInfo[i].dwSize = sizeof(QMAPI_CONSUMER_PRIVACY_INFO);
        pstConsumer->stPrivacyInfo[i].enBitrateType = QMAPI_BITRATE_NORMAL;
        pstConsumer->stPrivacyInfo[i].bSpeakerMute = QMAPI_FALSE;
        pstConsumer->stPrivacyInfo[i].bPreviewAudioEnable = QMAPI_TRUE;
        pstConsumer->stPrivacyInfo[i].bMotdectReportEnable = QMAPI_TRUE;
        pstConsumer->stPrivacyInfo[i].dwSceneMode = 1;  //默认均为室内模式
        pstConsumer->stPrivacyInfo[i].enBitrateType = QMAPI_BITRATE_AUTO;

        pstConsumer->stPrivacyInfo[i].stRecordInfo.bEnable = g_tl_globel_param.stChannelRecord[0].dwRecord;
        memcpy(pstConsumer->stPrivacyInfo[i].stRecordInfo.stRecordSched,
                    g_tl_globel_param.stChannelRecord[0].stRecordSched, sizeof(g_tl_globel_param.stChannelRecord[0].stRecordSched));

        pstConsumer->stPrivacyInfo[i].stMotionInfo.bEnable = g_tl_globel_param.TL_Channel_Motion_Detect[0].bEnable;
        pstConsumer->stPrivacyInfo[i].stMotionInfo.dwSensitive = g_tl_globel_param.TL_Channel_Motion_Detect[0].dwSensitive;
        pstConsumer->stPrivacyInfo[i].stMotionInfo.bPushAlarm = 1;  //待定
        memcpy(pstConsumer->stPrivacyInfo[i].stMotionInfo.stSchedTime,
                    g_tl_globel_param.TL_Channel_Motion_Detect[0].tlScheduleTime, sizeof(g_tl_globel_param.TL_Channel_Motion_Detect[0].tlScheduleTime));

        if(1 == i)
        {
            pstConsumer->stPrivacyInfo[i].stRecordInfo.bEnable = 0;
            pstConsumer->stPrivacyInfo[i].stMotionInfo.bPushAlarm = 0;
            memset(pstConsumer->stPrivacyInfo[i].stRecordInfo.stRecordSched, 0, sizeof(QMAPI_RECORDSCHED)*7*8);

            pstConsumer->stPrivacyInfo[i].stMotionInfo.bEnable = 0;
            memset(pstConsumer->stPrivacyInfo[i].stMotionInfo.stSchedTime, 0, sizeof(QMAPI_SCHED_TIME)*8);
            pstConsumer->stPrivacyInfo[i].bMotdectReportEnable = 0;
        }
    }

    pstConsumer->dwN1ListenPort = 80;

    return 0;
}

int TL_Default_ONVIFTestInfo_Param(QMAPI_SYSCFG_ONVIFTESTINFO *pstONVIFInfo)
{
    if(NULL == pstONVIFInfo)
        return -1;

    memset(pstONVIFInfo, 0, sizeof(QMAPI_SYSCFG_ONVIFTESTINFO));
    pstONVIFInfo->dwSize = sizeof(QMAPI_SYSCFG_ONVIFTESTINFO);
    pstONVIFInfo->dwDiscoveryMode = 1;

    return 0;
}


int TL_initDefaultSet(int setFlag, int flagCode)
{
    int i=0;

    Write_Reset_Log(flagCode);
    ///Clear All Parameter
    memset(&g_tl_globel_param , 0 , sizeof(TL_FIC8120_FLASH_SAVE_GLOBEL));
    if(setFlag == HARDRESET)
    {
        ///TL_SERVER_NETWORK 01 
        TL_Default_Network_Param(&g_tl_globel_param.stNetworkCfg);

        TL_Default_DeviceInfo_Param(&g_tl_globel_param.stDevicInfo);

        for (i=0; i<10; i++)
        {
            TL_Default_UserManagement_Param(&g_tl_globel_param.TL_Server_User[i], i);
        }
    }

    ///if software reset don't change network set
    if(setFlag==SOFTRESET)
    {
        tl_read_Server_Network();
        sprintf(g_tl_globel_param.stDevicInfo.csDeviceName,"%s",DEVICE_SERVER_NAME);
        tl_read_Server_User();
    }

    //TLNV_SERVER_INFO 03
    TL_Default_ServerInfo_Param(&g_tl_globel_param.TL_Server_Info);

    ///TL_CHANNEL_CONFIG 06
    for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
    {
        TL_Default_Channel_Encode_Param(&g_tl_globel_param.stChannelPicParam[i], i);
    }

    //  TL_Get_Video_Stream_Info();

    ///TL_CHANNEL_COLOR 07
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        tl_init_Channel_Color(i);
        TL_Default_CB_Detection(&g_tl_globel_param.stCBDetevtion[i], i);
    }
    ///TL_CHANNEL_LOGO 08
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_LOGO_Param(&g_tl_globel_param.TL_Channel_Logo[i], i);
    }

    ///TL_CHANNEL_SHELTER 09
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_Shelter_Param(&g_tl_globel_param.TL_Channel_Shelter[i], i);
    }

    ///TL_CHANNEL_MOTION_DETECT 10
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_MotionDetect_Param(&g_tl_globel_param.TL_Channel_Motion_Detect[i], i);
    }

    ///TL_CHANNEL_SENSOR_ALARM 11
    for(i=0;i<TL_MAXSENSORNUM;i++)
    {
        TL_Default_SensorIn_Param(&g_tl_globel_param.TL_Channel_Probe_Alarm[i], i);
    }

    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_RecordSched_Param(&g_tl_globel_param.stChannelRecord[i], i);
    }

    ///TL_CHANNEL_OSDINFO 13
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_OSD_Param(&g_tl_globel_param.TL_Channel_Osd_Info[i], &g_tl_globel_param.stTimeOsdPos[i], i);
    }

    ///TL_SERVER_OSD_DATA 14
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_OSDtypeData(&g_tl_globel_param.TL_Server_Osd_Data[i], i);
    }

    ///TL_UPLOAD_PTZ_PROTOCOL 17
    tl_read_default_PTZ_Protocol();

    ///TL_SERVER_COMINFO 15
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_RS485_Param(&g_tl_globel_param.TL_Server_Cominfo[i], i);
    }

    ///TL_SERVER_COM2INFO 16 dwChannel
    for(i=0;i<QMAPI_MAX_CHANNUM;i++)
    {
        TL_Default_RS232_Param(&g_tl_globel_param.TL_Server_Com2info[i], i);
    }

    ///TL_FTPUPDATA_PARAM 18
    TL_Default_FTP_Param(&g_tl_globel_param.TL_FtpUpdata_Param);

    TL_Default_SMTP_Param(&g_tl_globel_param.TL_Email_Param);

    ///TL_NOTIFY_SERVER 19
    TL_Default_NotifyServer_Param(&g_tl_globel_param.TL_Notify_Servers);

    ///init wireless
	TL_Default_Wireless_Param(&g_tl_globel_param.stWifiConfig);

    TL_Default_UpNP_Param(&g_tl_globel_param.tl_upnp_config);

    TL_Default_RTSP_Param(&g_tl_globel_param.stRtspCfg);

    TL_Default_DDNS_Param(&g_tl_globel_param.stDDNSCfg);

    TL_Default_NTP_Param(&g_tl_globel_param.stNtpConfig);

    TL_Default_TIMEZONE_Param(&g_tl_globel_param.stTimezoneConfig);

    TL_Default_DeviceMaintain_Param(&g_tl_globel_param.stMaintainParam);

    TL_Default_ConsumerInfo_Param(&g_tl_globel_param.stConsumerInfo);

    TL_Default_ONVIFTestInfo_Param(&g_tl_globel_param.stONVIFInfo);

    //该成员需从全局结构体中删除
    memset(&g_tl_globel_param.stNetworkStatus, 0, sizeof(QMAPI_NET_NETWORK_STATUS));
    g_tl_globel_param.stNetworkStatus.dwSize = sizeof(QMAPI_NET_NETWORK_STATUS);

    //----------------------------------------------------------------
    TL_Default_NetPlatCfg_Param(&g_tl_globel_param.stNetPlatCfg);

    TL_GetDefaultValue_From_CfgFile(TL_FLASH_CONF_DEFAULT_PARAM);

    tl_write_all_parameter();

#ifndef TL_Q3_PLATFORM
    TL_SetDataPort_Script();
#endif

    return 0;
}

#ifndef TL_Q3_PLATFORM
int TL_SetDataPort_Script( void )
{
    FILE *fp;
    char csXmlBuf[1024];
    char csXmlBuf2[1024] = {0};
    char *p2 = csXmlBuf2;
    const char *p1 = g_tl_globel_param.stDevicInfo.csDeviceName;
    int size1 = 0, size2 = 1023;
    iconv_t g_iconv = (iconv_t)(-1);
   // need fixme yi.zhang
   // if (g_tl_updateflash.bUpdatestatus)
   //     return 0;

    g_iconv = iconv_open("UTF-8","GB2312");
    if( g_iconv == (iconv_t)-1)
    {
        printf("%s(%d) iconv_open UTF-8 GB2312 failed:%s\n", __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    
    fp = fopen("/usr/netview/web/a.js", "wb");
    if( fp == NULL)
    {
        iconv_close(g_iconv);
        g_iconv = (iconv_t)-1;

        printf("%s(%d) fopen:/usr/netview/web/a.js failed:%s\n", __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    DWORD dwUPnPDataPort = g_serverPort;
    if(g_tl_globel_param.tl_upnp_config.wDataPortOK == 1)
    {
        dwUPnPDataPort = g_tl_globel_param.tl_upnp_config.dwDataPort1;
    }

    int timer = 0;
    int writebytes = 0;
    int totalbytes = 0;
    int outputbytes = 0;
    char *pTmp = csXmlBuf;

    size1 = strlen(p1);
    iconv(g_iconv,&p1,(size_t*)&size1,&p2,(size_t*)&size2);
    p2 = strdup(csXmlBuf2);

    sprintf(csXmlBuf, "<!--\nvar vPort = %d;\nvar vUPnPPort = %ld;\nvar vDevIP = \"%s\";\nvar ServerName = \"%s\";\n-->\n",
        g_serverPort, 
        dwUPnPDataPort,
        g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4,
        p2);

    totalbytes = strlen(csXmlBuf);
    while(writebytes < totalbytes && timer < 10)
    {
        char *pWrite = pTmp+writebytes;
        outputbytes = fwrite(pWrite,1,totalbytes-writebytes,fp);
        if(outputbytes > 0)
        {
            writebytes += outputbytes;
        }
        else
        {
            printf("write a.js failure!\n");
            break;
        }
        
        timer++;
    }
    fclose(fp);
    fp = NULL;
    free(p2);   
    iconv_close(g_iconv);
    g_iconv = (iconv_t)-1;

    return 0;
}
#endif

void TL_WebServerPort_Limit(void)
{ 
    QMAPI_NET_NETWORK_CFG *netinfo = &g_tl_globel_param.stNetworkCfg;
    
    int serverMediaPort = netinfo->stEtherNet[0].wDataPort;
    if (serverMediaPort < 1024 || serverMediaPort > 65535)
    {
        serverMediaPort = SER_DEFAULT_PORT;
    }

    int webServerPort = netinfo->wHttpPort;
    if (webServerPort < 80 || webServerPort > 65535)
    {
        webServerPort = SER_DEFAULT_WEBPORT;
    }

    if (webServerPort == serverMediaPort)
    {
        serverMediaPort = SER_DEFAULT_PORT;
        webServerPort   = SER_DEFAULT_WEBPORT;
    }

    netinfo->stEtherNet[0].wDataPort = serverMediaPort;
    netinfo->wHttpPort = webServerPort;
    
    return ;
}


int QMAPI_SYSCFG_Init(syscfg_default_param *syscfg)
{
    int ret;
    int i = 0;

    //添加初始化默认配置文件
    //该文件可通过用户自定义并保存至flash,做硬恢复时此参数可生效
    //TLTK_DefaultConfigFile();
    infotm_video_info_init();

    ///read initial information
    ret = tl_read_fatal_param();

    memcpy(&enSysDefaultParam, syscfg, sizeof(syscfg_default_param));
    printf("#### %s %d, ret=%d, g_fatal_param.default_flag=%d\n", __FUNCTION__, __LINE__, 
            ret, g_fatal_param.default_flag);
    if (ret < 0)
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        printf("LINE:%d HARDRESET default value.\n" , __LINE__);
        TL_initDefaultSet(HARDRESET, 6);
    }
    else if (g_fatal_param.default_flag == fFlagSoftReset)      // 182
    {
        printf("SoftReset default value.\n");
        TL_initDefaultSet( SOFTRESET, 7);
    }
    else if (g_fatal_param.default_flag == fFlagChange) // 181
    {
        tl_read_all_parameter();
    }
    else if(g_fatal_param.default_flag== fFlagDefault) //180 Hardware reset
    {
        printf("LINE:%d HARDRESET default value.\n" , __LINE__);
        TL_initDefaultSet(HARDRESET, 8);
        //QMAPI_AudioPromptPlay(AUDIO_PROMPT_RESET_SUCCESS, 0);
    }
    else
    {
        printf("LINE:%d HARDRESET default value.\n" , __LINE__);
        TL_initDefaultSet( HARDRESET, 9);
    }
    g_tl_globel_param.stDevicInfo.dwSoftwareVersion = GetSystemVersionFlag();
    g_tl_globel_param.stDevicInfo.dwSoftwareBuildDate = strtoul(__BUILD_DATE__, NULL, 16);

    TL_WebServerPort_Limit();   

    //always reinit encode param every time, workaround for channel pic config missing 
    for(i = 0; i < QMAPI_MAX_CHANNUM; i++)
    {
        TL_Default_Channel_Encode_Param(&g_tl_globel_param.stChannelPicParam[i], i);
    }
  
    tl_write_ChannelPic_param();

    return 0;
}

int QMAPI_SYSCFG_UnInit(void)
{
    tl_write_all_parameter();
    return 0;
}


