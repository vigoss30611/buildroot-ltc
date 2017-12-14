#ifndef _WIFI_H_
#define _WIFI_H_

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#define WIFI_CMD_EVENT 			"wifi_cmd"
#define WIFI_STATE_EVENT		"wifi_state"
#define STATE_STA_CONNECTED 	"sta_connected"
#define STATE_STA_DISCONNECTED  "sta_disconnected"
#define STATE_STA_ENABLED 		"sta_enabled"
#define STATE_AP_ENABLED 		"ap_enabled"
#define STATE_STA_DISABLED 		"sta_disabled"
#define STATE_AP_DISABLED		"ap_disabled"
#define STATE_MONITOR_ENABLED	"monitor_enabled"
#define STATE_MONITOR_DISABLED	"monitor_disabled"
#define STATE_SCAN_COMPLETE 	"scan_complete"
#define STATE_AP_STA_CONNECTED 	"ap_sta_connected"
#define STATE_AP_STA_DISCONNECTED 	"ap_sta_disconnected"
#define WIFI_GET_STATE 			"get_state"
#define WIFI_GET_NET_INFO 		"get_net_info"
#define WIFI_STA_GET_CONFIG		"sta_get_config"
#define WIFI_STA_ITERATE_CONFIG	"sta_iterate_config"
#define WIFI_STA_GET_SCAN_RESL	"sta_get_scan_result"
#define WIFI_AP_GET_CONFIG		"ap_get_config"
#define WIFI_AP_SET_CONFIG		"ap_set_config"

struct wifi_list_head {
	struct wifi_list_head *next, *prev;
};

#define AP_SAVE				0
#define AP_SCAN				1
#define STA_STATUS_CHECK	1
#define STA_WAIT_EVENT		2
enum {
    DISABLED  = 0,
    ENABLED = 1,
    CURRENT  = 2
};

typedef enum {
	WIFI_STATE_FAILED = -1,
	WIFI_STA_STATE_DISABLED = 0x1,
	WIFI_STA_STATE_ENABLED = 0x2,
	WIFI_STA_STATE_SCANED = 0x3,
	WIFI_STA_STATE_CONNECTED = 0x4,
	WIFI_STA_STATE_DISCONNECTED = 0x5,
	WIFI_AP_STATE_DISABLED = 0x10,
	WIFI_AP_STATE_ENABLED = 0x20,
	WIFI_MONITOR_STATE_DISABLED = 0x100,
	WIFI_MONITOR_STATE_ENABLED = 0x200,
	WIFI_STATE_UNKNOWN = 0xf000000,
} Wifi_State_t;

enum {
    CMD_QUIT  = 0,
    CMD_STA_ENABLE = 1,
    CMD_STA_DISABLE  = 2,
    CMD_STA_SCAN = 3,
    CMD_STA_CONNECT = 4,
    CMD_STA_FORGET = 5,
    CMD_STA_DISCONNECT = 6,
    CMD_AP_ENABLE = 7,
    CMD_AP_DISABLE = 8,
    CMD_MONITOR_ENABLE = 9,
    CMD_MONITOR_DISABLE = 10,
    CMD_WPS_CONNECTE = 11,
    CMD_IDLE = 12,
	CMD_NONE = 13,
};

struct net_info_t {
	char hwaddr[32];
	char ipaddr[32];
	char gateway[32];
	char mask[32];
	char dns1[32];
	char dns2[32];
	char server[32];
	char lease[32];
};

struct wifi_config_t {
	struct wifi_list_head 	list;
	int network_id;
	int status;
	int list_type;
	int level;
	int channel;
	char ssid[32];
	char bssid[32];
	char key_management[64];
	char pairwise_ciphers[16];
	char psk[16];
};

struct wifi_command_t {
	int cmd_type;
	char ssid[32];
	char psk[16];
};

extern int wifi_start_service(void (* state_cb)(char* event, void *arg));
extern int wifi_stop_service(void);
extern int wifi_sta_enable(void);
extern int wifi_sta_disable(void);
extern int wifi_sta_scan(void);
extern struct wifi_config_t *wifi_sta_get_scan_result(void);
extern int wifi_sta_connect(struct wifi_config_t *wifi_config);
extern int wifi_sta_disconnect(struct wifi_config_t *wifi_config);
extern int wifi_sta_forget(struct wifi_config_t *wifi_config);
extern struct wifi_config_t *wifi_sta_get_config(void);
extern struct wifi_config_t *wifi_sta_iterate_config(struct wifi_config_t *wifi_config);
extern int wifi_ap_enable(void);
extern int wifi_ap_disable(void);
extern int wifi_ap_set_config(struct wifi_config_t *wifi_config);
extern int wifi_ap_get_config(struct wifi_config_t *wifi_config);
extern int wifi_get_state(void);
extern int wifi_get_net_info(struct net_info_t *net_info);
extern int wifi_monitor_enable(void);
extern int wifi_monitor_disable(void);
extern int wifi_wps_connect(struct wifi_config_t *wifi_config);
#endif
