#ifndef __Q3_WIRELESS_H__
#define __Q3_WIRELESS_H__

#include "QMAPINetSdk.h"
#include "QMAPI.h"
#include "qsdk/wifi.h"


void q3_wireless_start_swave();
int QMAPI_Wireless_Init(void (*state_cb)(char* event, void *arg));
int QMAPI_Wireless_Deinit();
int QMAPI_Wireless_Start(QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig);
int QMAPI_Wireless_Stop();
int QMAPI_Wireless_GetState(QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig);
int QMAPI_Wireless_IOCtrl(int Handle, int nCmd, int nChannel, void *Param, int nSize);
int QMAPI_Wireless_IOCtrl(int Handle, int nCmd, int nChannel, void *Param, int nSize);
int QMAPI_Wireless_SwitchMode(int nMode);
int QMAPI_Wireless_GetSiteList(QMAPI_NET_WIFI_SITE_LIST *pstWifiSiteList);



#endif

