#ifndef WIFI_OPERATION_H
#define WIFI_OPERATION_H

/*
 * Copyright (c) 2014 Infotm, Inc.  All rights reserved.
 * 20140814 Mason Xiao create.
 */

#include <stdio.h>

typedef enum
{
    INVALID_WIFI_CMD       = 0, // there is not any request command.
    SETTING_WIFI_CMD       = 1,
    ASKING_STATUS_WIFI_CMD = 2,
    TESTING_LED_CMD        = 3,
}WifiRequestTypeE;

typedef enum
{
    WIFIAPMODE_NULL           = 0x00,
    WIFIAPMODE_MANAGED        = 0x01,
    WIFIAPMODE_ADHOC          = 0x02,
    WIFI_CANCEL_SETTING       = 0x04,
}WifiApModeE;

typedef enum
{
    WIFIAPENC_INVALID         = 0x00,
    WIFIAPENC_NONE            = 0x01,
    WIFIAPENC_WEP             = 0x02, //WEP, for no password
    WIFIAPENC_WPA_TKIP        = 0x03,
    WIFIAPENC_WPA_AES         = 0x04,
    WIFIAPENC_WPA2_TKIP       = 0x05,
    WIFIAPENC_WPA2_AES        = 0x06,
    WIFIAPENC_WPA_PSK_TKIP    = 0x07,
    WIFIAPENC_WPA_PSK_AES     = 0x08,
    WIFIAPENC_WPA2_PSK_TKIP   = 0x09,
    WIFIAPENC_WPA2_PSK_AES    = 0x0A,
}WifiApEnctypeE;

typedef struct
{
    char ssid[32];                 // WiFi ssid
    char mode;                       // refer to WifiApModeE
    char enctype;                  // refer to WifiApEnctypeE
    char signal;                   // signal intensity 0--100%
    char status;                   // 0 : invalid ssid or disconnected
                                // 1 : connected with default gateway
                                // 2 : unmatched password
                                // 3 : weak signal and connected
                                // 4 : selected:
                                //        - password matched and
                                //        - disconnected or connected but not default gateway
}WifiApInfoT;

typedef struct
{
    unsigned char ssid[32];            //WiFi ssid
    unsigned char password[32];        //if exist, WiFi password
    unsigned char mode;                //refer to WifiApModeE
    unsigned char enctype;            //refer to WifiApEnctypeE
    unsigned char reserved[10];
}WifiSettingParamT;

typedef struct
{
    int result; //0: wifi connected; 1: failed to connect
    unsigned char reserved[4];
}WifiSettingResultT;

typedef struct
{
    int number; // number of WifiApInfoT
    WifiApInfoT pApList[1]; // arrary of WifiApInfoT
}WifiStatusResultT;

//following is interface used by boa
#ifdef __cplusplus 
extern "C" { 
#endif

/**
 * wifiOpInit: interface for boa and P2P below.
 */
void wifiOpInit(void);

/**
 * wifiOpDeinit: interface for boa and P2P below.
 */
void wifiOpDeinit(void);

/**
 *  askWifiRequest:to polling if there is a wifi operation request.
 *                 NOTE, 1:should polling once every 100 miniseconds.
 *                       2:this function is not thread safe.
 *
 *  pParam [OUT]:address of request parameter.
 *               SETTING_WIFI_CMD: address of share memory of WifiSettingParamT
 *               ASKING_STATUS_WIFI_CMD: *pParam = 0;
 *
 * return:  WifiRequestTypeE
 */
int askWifiRequest(void* pParam);

/**
 *  answerWifiRequest: once receive a request, should call this function to response.
 *                     NOTE: this function is not thread safe.
 *                     
 *   command [IN]: WifiRequestTypeE to response.
 * 
 *   pResult [IN]: address of response result.
 *                  SETTING_WIFI_CMD: WifiSettingResultT*   
 *                  ASKING_STATUS_WIFI_CMD: WifiStatusResultT*
 *                                               
 *
 * return: 0: fail(maybe state machine error), 1: ok.
 */
int answerWifiRequest(const int command, void* pResult);

/**
 * requestWifiOp: interface for P2P below.
 * return 0: fail, 1: ok.
 */
int requestWifiOp(const int command, void* param);

/**
 * checkWifiOpResult: interface for P2P below.
 * return 0: fail, 1: ok.
 */
int checkWifiOpResult(const int command, void* pResult);

#ifdef __cplusplus 
} 
#endif

#endif // WIFI_OPERATION_H

