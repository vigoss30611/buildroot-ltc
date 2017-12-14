#ifndef __SPV_WIFI_H__
#define __SPV_WIFI_H__

enum {
    DISABLE_WIFI,
    ENABLE_AP,
    ENABLE_STA,
};

enum {
    INFO_SSID,
    INFO_PSWD,
    INFO_KEYMGMT,
    INFO_WIFI_COUNT,
};

#define INFO_LENGTH 128

typedef struct _WifiOpt {
    int operation;
    char wifiInfo[INFO_WIFI_COUNT][INFO_LENGTH];
} WifiOpt;

typedef WifiOpt* PWifiOpt;

#endif
