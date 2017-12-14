
/***********************************************************************
* Copyright (c) 2014, infoTMIC.
* All rights reserved.
************************************************************************
* AUTHOR: LiQiang
* DATE: 2014/08/13
************************************************************************
* DESCRIPTION: Main routine for WiFi Control
*
************************************************************************
* History
************************************************************************
Date		Revised By	Item		Description
20140813	LiQiang		RM#NONE	Creat
20140813	LiQiang		RM#11723	Make a WiFi part from BOA module
************************************************************************/
#ifndef _WIFI_API_H
#define _WIFI_API_H

#include "wifi_operation.h"


/*RM#NONE add by LiQiang 20140506, add mac address to ssid string when use default value*/
#define WIFI_POINT_SSID_MAC

/*RM#NONE add by LiQiang 20140515, enable or disable WiFi STA mode*/
#define WIFI_STA_ENABLE

/*RM#NONE add by LiQiang 20140515, enable or disable WiFi AP mode*/
#define WIFI_POINT_ENABLE


/*RM#10541 add by LiQiang 20140612, enable or disable WiFi AP quit*/
#define WIFI_POINT_QUIT
#define MAX_CHARS_IN_LINE 1024
#define SSID_MAC_MAX_LEN	64
#define MAX_LINE_SIZE		255
#define MAX_DATA_SIZE		(1024 * 2)
#define MAX_WIFI_AP_LIST	28

#define MAX_WIFIAP_LIST_ITEM_NUM 28


#define MTK7601_AP_MODE_SH "/wifi/mtk7601/mt7601_up_ap.sh"
#define MTK7601_STA_MODE_SH "/wifi/mtk7601/mt7601_up_sta.sh"

#define RTL8188_AP_MODE_SH "rtl8188_up_ap.sh"
#define RTL8188_STA_MODE_SH "rtl8188_up_sta.sh"


#define DBG(fmt, args...)	fprintf(stderr,"\033[40;34mWifi: Debug\033[0m\t" fmt, ##args)
#define WIFI_STATUS_CHANGED(x, y) ((x) == (y)) ? 0 : 1

#define WIFI_RUN_WPA_CMD(cmd, ...) \
{\
    snprintf(cmd,255, __VA_ARGS__);\
    DBG("WPA:%s\n",cmd);\
    wpa_cmd(cmd); \
}

#define WIFI_RUN_SYS_CMD(cmd, ...) \
{\
    snprintf(cmd,255, __VA_ARGS__);\
    DBG("SYS:%s\n",cmd);\
    system(cmd); \
}

enum WIFI_MODE {
	WIFI_MODE_NO_WIFI,	//û???κ?wifi
	WIFI_MODE_WIFI,		//wifiģʽ
	WIFI_MODE_HOTPOINT	//?ȵ?ģʽ
	
};

typedef struct WifiStatus {
	int isInited;		//?Ƿ??Ѿ?????
	int initNeed;
	int scan;			//?Ƿ??Ѿ?scan??
	int isHotPointInited;	//?ȵ㹦???Ƿ??Ѿ?????
	
	enum WIFI_MODE	wifiMode;
}WifiStatus;

typedef enum
{
	AP_START,
	AP_QUIT
}WIFI_AP_LAST_ACTION;

typedef enum
{
	WIFI_DAEMON_WPA_NONE,
	WIFI_DAEMON_WPA_SETTING_WIFI,
	WIFI_DAEMON_WPA_SCANNING,
	TEST_LED
}WIFI_DAEMON_STATE;


typedef struct WIFIINF {
    char Address[50];            //物理地址
    char ESSID[50];                //信号???
    char Protocol[50];            //标准
    char Mode[50];                //模式
    char Frequency[50];            //频率，单位GHz
    char Encryption_key;        //是否加密
    float Bit_Rates;            //速率
    int    Quality;                //质量
    struct WIFIINF* next;
}WIFIINF,*pWIFIINF;


/* config related */
typedef struct Wifi_Config_Data {
	char	wifiselect[255];
	char	wifiuser[50];
	char	wifipwd[50];
	unsigned char	wifienciphertype;
	unsigned char	wifistaticipset;
	unsigned char	wifienable;
} WIFI_CONFIG_DATA;


/* ipc related */
typedef struct
{
    int number;
    WifiApInfoT wifiAps[MAX_WIFIAP_LIST_ITEM_NUM];
}WifiApListT;

typedef union
{
    WifiSettingResultT wifiSettingResult;
    WifiApListT wifiApListResult;
}WifiOpResultU;

typedef struct
{
    char wifiRequestCmd; // refer to WifiRequestTypeE
    char wifiOpRespCode; // 0: invalid, 1: complete lastest request.
    char reseved[2];
    WifiSettingParamT wifSettingParam;
    WifiOpResultU wifiOpResult;
}WifiOperationT;


#ifdef __cplusplus 
extern "C" {
#endif


extern int initWPA();
extern int uninitWPA();
extern int wpa_cmd(char* cmd);
extern int wpa_set_outbuf(char* buf );
//extern int initAP(void);
//extern int uninitAP(void);
//extern int ap_set_outbuf(char *buf);
//extern int hostapd_cli_cmd_connect(void);
extern void *WifiPointDaemonProc(void *arg);

#ifdef __cplusplus 
}
#endif

#endif

