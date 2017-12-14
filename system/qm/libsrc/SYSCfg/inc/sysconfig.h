#ifndef __SYSCOFNIG_H__
#define __SYSCOFNIG_H__

#include "QMAPIType.h"

#include "tl_common.h"

typedef struct{
	int enInputType;      //QMAPI_VIDEO_INPUT_TYPE
	int chanel;
}syscfg_default_param;

//参数pIp为xxx.xxx.xxx.xxx格式,失败返回0
unsigned long TL_GetIpValueFromString(const char *pIp);
DWORD GetSystemVersionFlag();

int TL_GetAddrNum(const char *pIp, unsigned int *num1,unsigned int *num2,unsigned int *num3,unsigned int *num4);
int TL_GetIP_From_CfgFile(const char *pFileName,unsigned char *ip);
int TL_GetMac_From_CfgFile(const char *pFileName,unsigned char *pMac);
int TL_UpdateCfgFile(const char *pFileName,const char* section,const char* key,int key_len,const char* value,int value_len);
int TL_GetDefaultValue_From_CfgFile(const char *pFileName);



int TL_Default_DeviceInfo_Param(QMAPI_NET_DEVICE_INFO *pstDeviceInfo);
int TL_Default_Channel_Encode_Param(QMAPI_NET_CHANNEL_PIC_INFO *pChnPicInfo, int Channel);
int TL_Default_MotionDetect_Param(TL_CHANNEL_MOTION_DETECT *pChnMotionDect, int Channel);
int TL_Default_SensorIn_Param(TL_SENSOR_ALARM *pSensroAlarm, int Channel);
int TL_Default_CB_Detection(QMAPI_NET_DAY_NIGHT_DETECTION_EX *pData, int Channel);
int TL_Default_Network_Param(QMAPI_NET_NETWORK_CFG *pstNetworkCfg);
int TL_Default_UserManagement_Param(TL_SERVER_USER *pstServerUser, int Index);
int TL_Default_ServerInfo_Param(TLNV_SERVER_INFO *pstServerInfo);
int TL_Default_LOGO_Param(TL_CHANNEL_LOGO *pstLOGO, int Channel);
int TL_Default_Shelter_Param(TL_CHANNEL_SHELTER *pstShelter, int Channel);
int TL_Default_RecordSched_Param(QMAPI_NET_CHANNEL_RECORD *pchn_record, int Channel);
int TL_Default_OSD_Param(TL_CHANNEL_OSDINFO *pstOSDInfo, TL_TIME_OSD_POS *pstTimeOSDPos, int Channel);
int TL_Default_OSDtypeData(TL_SERVER_OSD_DATA *pstOSDData, int Channel);
int TL_Default_RS485_Param(TL_SERVER_COMINFO_V2 *pstRS485, int Channel);
int TL_Default_RS232_Param(TL_SERVER_COM2INFO *pstRS232, int Channel);
int TL_Default_FTP_Param(TL_FTPUPDATA_PARAM *pstFtp);
int TL_Default_SMTP_Param(QMAPI_NET_EMAIL_PARAM *pstSMTP);
int TL_Default_NotifyServer_Param(TL_NOTIFY_SERVER *pstNotify);
int TL_Default_PPPOE_Param(TL_PPPOE_DDNS_CONFIG *pstpppoe);
int TL_Default_WirelessV1_Param(TL_WIFI_CONFIG *pstWifi);
int TL_Default_WirelessV2_Param(QMAPI_NET_WIFI_CONFIG *pstWifi);
int TL_Default_UpNP_Param(TL_UPNP_CONFIG *pstUPNP);
int TL_Default_RTSP_Param(QMAPI_NET_RTSP_CFG *pstrtsp);
int TL_Default_DDNS_Param(QMAPI_NET_DDNSCFG *pstDDNS);
int TL_Default_NTP_Param(QMAPI_NET_NTP_CFG *pstNTP);
int TL_Default_TIMEZONE_Param(QMAPI_NET_ZONEANDDST *pstZone);
int TL_Default_DeviceMaintain_Param(QMAPI_NET_DEVICEMAINTAIN *pstDeviceMaintain);
int TL_Default_NetPlatCfg_Param(QMAPI_NET_PLATFORM_INFO_V2 *pstPlatForm);
int TL_Default_ConsumerInfo_Param(QMAPI_NET_CONSUMER_INFO *pstConsumer);

int TL_initDefaultSet(int setFlag, int flagCode);

int QMAPI_SYSCFG_Init(syscfg_default_param *syscfg);
int QMAPI_SYSCFG_UnInit(void);


#endif

