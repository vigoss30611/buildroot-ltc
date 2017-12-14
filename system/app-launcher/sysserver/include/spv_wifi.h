#ifndef __SPV_WIFI_H__
#define __SPV_WIFI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISABLE_WIFI,
    ENABLE_AP,
    ENABLE_STA,
} WIFI_CMD;

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


typedef enum _wifi_status {
    UNKNOWN_STATUS = 1,
    SOFTAP_ENABLED,
    SOFTAP_DISABLED,
    SOFTAP_FAILED,
    SCAN_AP_FAILED,
    STA_ENABLED,
    STA_DISABLED,
    STA_FAILED,
    STA_SSID_NOT_EXIST = (0x1 << 4),
    STA_WRONG_KEY_MGMT = (0x1 << 5),
    STA_WRONG_KEY = (0x1 << 6),
    STA_GET_IP_FAILED,
} wifi_status;

typedef struct _router_status {
    char bssid[32]; //mac addr
    char frequency[8];
    char signal_level[6];
    char key_mgmt[64]; //flags
    char ssid[32];
    struct _router_status *next;
} router_status, *router_status_p;

typedef struct _ap_info {
    char bssid[32]; //mac addr
    int signal_level;
    char key_mgmt[32];
    char ssid[32];
} ap_info, *ap_info_p;

typedef struct _scan_results {
    int ap_num;
    ap_info ap_info_array[1];
} scan_results, *scan_results_p;

typedef struct wifi {
    int (*start_softap)(char *ssid, char *key);
    int (*stop_softap)(void);
    int (*is_softap_enabled)(void);

    int (*scan_ap)(void);

    int (*start_sta)(char *ssid, char *key_mgmt, char *key);
    int (*stop_sta)(void);
    int (*is_sta_enabled)(void);

    scan_results_p scan_results_all_g;
}wifi,*wifi_p;

wifi_p wifi_create(void);
void wifi_destroy(void);

int wifi_get_ssid(char *ssid);

#ifdef __cplusplus
}
#endif

#endif
