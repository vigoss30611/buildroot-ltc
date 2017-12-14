#ifdef GUI_WIFI_ENABLE
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>

#include "spv_debug.h"
#include "qsdk/wifi.h"
#include "spv_wifi.h"
#include "sys_server.h"

#define MAX_ARRAY_NUM 20

static wifi_status wifi_status_g = UNKNOWN_STATUS;
wifi_p wifi_p_g = NULL;
static char ssid_g[32] = {0};
static char key_mgmt_g[32] = {0};
static char key_g[32] = {0};

int start_softap(char *ssid, char *key);
int stop_softap(void);
int is_softap_enabled(void);

int scan_ap(void);

int start_sta(char *ssid, char *key_mgmt, char *key);
int stop_sta(void);
int is_sta_enabled(void);

int get_wifi_ssid(char *ssid)
{
    struct wifi_config_t ap_config;
    struct wifi_config_t *sta_config;
    int ret = 0;
    if(ssid == NULL)
        return -1;

    switch(wifi_status_g) {
        case SOFTAP_ENABLED:
            ret = wifi_ap_get_config(&ap_config);
            if(!ret) {
                strcpy(ssid, ap_config.ssid);
            } else {
                INFO("wifi get ap config failed\n");
                return -1;
            }
            break;
        case STA_ENABLED:
            sta_config = wifi_sta_get_config();
            if(sta_config != NULL) {
                strcpy(ssid, sta_config->ssid);
            } else {
                INFO("wifi get sta config failed\n");
                return -1;
            }
            break;
        case UNKNOWN_STATUS:
        case SOFTAP_DISABLED:
        case STA_DISABLED:
        default:
            ssid[0] = 0;
            break;
    }

    return 0;
}


int start_softap(char *ssid, char *key)
{
    int ret = 0;
    INFO("start_softap, ssid: %s, key: %s\n", ssid, key);
    struct wifi_config_t ap_config = {0};
    if(ssid != NULL && strlen(ssid) > 0 && key != NULL && strlen(key) > 0) {
        strcpy(ap_config.ssid, ssid);
        strcpy(ap_config.psk, key);
    } else {
        strcpy(ap_config.ssid, "INFOTM_cardv_q360_%2m");
        strcpy(ap_config.psk, "admin888");
    }
    strcpy(ap_config.key_management, "wpa2-psk");
    ret = wifi_ap_set_config(&ap_config);
    if(ret) {
        ERROR("ap_set_config_failed\n");
        return -1;
    }

    return wifi_ap_enable();
}

int stop_softap(void)
{
    return wifi_ap_disable();
}

int is_softap_enabled(void)
{
    int ret = 0;
    struct wifi_config_t ap_config;
    ret = wifi_ap_get_config(&ap_config);
    //if(ret) {
    //    ERROR("wifi_ap_get_config failed\b");
        return wifi_status_g == SOFTAP_ENABLED;
    //}

    ret = ap_config.status == WIFI_AP_STATE_ENABLED;
    //INFO("is_softap_enabled, ret: %d, status: %d\n", ret, ap_config.status);
    return ret;
}

int scan_ap(void)
{
    int ret = 0;
    return ret;
}

int start_sta(char *ssid, char *key_mgmt, char *key)
{
    int ret = 0;
    INFO("start_sta, ssid: %s, key: %s, key_mgmt: %s\n", ssid, key, key_mgmt);
    //if(ssid != NULL && strlen(ssid) > 0 && key != NULL && strlen(key) > 0) {
        struct wifi_config_t ap_config = {0};
        strcpy(ap_config.ssid, ssid);
		strcpy(ap_config.key_management, key_mgmt);
        strcpy(ap_config.psk, key);
        ret = wifi_ap_set_config(&ap_config);
        if(ret) {
            ERROR("ap_set_config_failed\n");
            return -1;
        }
    //}

    ret = wifi_sta_enable();
    return ret;
}

int stop_sta(void)
{
    return wifi_sta_disable();
}

int is_sta_enabled(void)
{
    struct wifi_config_t *sta_config = NULL;
    sta_config = wifi_sta_get_config();
    //if(sta_config == NULL) {
    //    ERROR("wifi_ap_get_config failed\b");
        return wifi_status_g == STA_ENABLED;
    //}

    int ret = sta_config->status == WIFI_STA_STATE_ENABLED;
    free(sta_config);
    return ret;
}

void show_net_info(struct net_info_t *info)
{
    printf("hwaddr is %s\n", info->hwaddr);
    printf("ipaddr is %s\n", info->ipaddr);
    printf("gateway is %s\n", info->gateway);
    printf("mask is %s\n", info->mask);
    printf("dns1 is %s\n", info->dns1);
    printf("dns2 is %s\n", info->dns2);
    printf("server is %s\n", info->server);
    printf("lease is %s\n", info->lease);
}

void wifi_state_handle(char *event, void *arg)
{
    struct wifi_config_t *sta_get_scan_result = NULL, *sta_get_scan_result_prev = NULL, *sta_get_config = NULL;
    int sta_connect_g = 0;
    static int ap_connected_count = 0;

    INFO("func %s, event is %s, arg is %s \n", __func__, event, (char *)arg);

    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
        if (!strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
            wifi_status_g = STA_ENABLED;
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_STATUS, WIFI_STA_ON, 0);
            INFO("sta enabled \n");
        } else if (!strncmp((char *)arg, STATE_STA_DISABLED, strlen(STATE_STA_DISABLED))) {
            wifi_status_g = STA_DISABLED;
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_STATUS, WIFI_OFF, 0);
            INFO("sta disabled \n");
        } else if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
            wifi_status_g = SOFTAP_ENABLED;
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_STATUS, WIFI_AP_ON, 0);
            INFO("ap enabled \n");
        } else if (!strncmp((char *)arg, STATE_AP_DISABLED, strlen(STATE_AP_DISABLED))) {
            wifi_status_g = SOFTAP_DISABLED;
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_STATUS, WIFI_OFF, 0);
            INFO("ap disabled \n");
        } else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
            INFO("scan complete, now get scan results \n");
            sta_get_scan_result =  wifi_sta_get_scan_result();
            if (sta_get_scan_result != NULL) {
                //show_results(sta_get_scan_result);
            }

            while (sta_get_scan_result != NULL) {
                sta_get_scan_result_prev = sta_get_scan_result;

                sta_get_scan_result = wifi_sta_iterate_config(sta_get_scan_result_prev);
                free(sta_get_scan_result_prev);
                if (sta_get_scan_result != NULL) {
                    //show_results(sta_get_scan_result);
                }
            }

            if (sta_get_scan_result != NULL) {
                free(sta_get_scan_result);
            }
        } else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_CONNECTION, WIFI_STA_CONNECTED, 1);
            sta_connect_g = 1;
            INFO("sta connected \n");
            sta_get_config = wifi_sta_get_config();
            if (sta_get_config != NULL) {
                //show_results(sta_get_config);
                free(sta_get_config);
            }
        } else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
            sta_connect_g = 0;
            INFO("sta disconnected \n");
            SpvPostMessage(GetLauncherHandler(), MSG_WIFI_CONNECTION, WIFI_STA_DISCONNECTED, 0);
        } else if (!strncmp((char *)arg, STATE_MONITOR_ENABLED, strlen(STATE_MONITOR_ENABLED))) {
            INFO("monitor enabled");
        } else if (!strncmp((char *)arg, STATE_MONITOR_DISABLED, strlen(STATE_MONITOR_DISABLED))) {
            INFO("monitor disabled");
        }
    } else if (!strncmp(event, STATE_AP_STA_CONNECTED, strlen(STATE_AP_STA_CONNECTED))) {
        if(ap_connected_count <= 0) {
            ap_connected_count = 1;
        } else {
            ap_connected_count++;
        }
        printf("ap sta connected info \n");
        //show_net_info((struct net_info_t *)arg);
        SpvPostMessage(GetLauncherHandler(), MSG_WIFI_CONNECTION, WIFI_AP_CONNECTED, ap_connected_count);
    } else if (!strncmp(event, STATE_AP_STA_DISCONNECTED, strlen(STATE_AP_STA_DISCONNECTED))) {
        if(ap_connected_count <= 0) {
            ap_connected_count = 0;
        } else {
            ap_connected_count--;
        }
        printf("ap sta disconected info \n");
        SpvPostMessage(GetLauncherHandler(), MSG_WIFI_CONNECTION, WIFI_AP_DISCONNECTED, ap_connected_count);
        //show_net_info((struct net_info_t *)arg);
    }
}

wifi_p wifi_create()
{
    INFO("enter func %s \n", __func__);
    wifi_start_service(wifi_state_handle);

    wifi_p_g = (wifi_p)malloc(sizeof(wifi));
    wifi_p_g->scan_results_all_g = (scan_results_p)malloc(sizeof(scan_results) + sizeof(ap_info)*MAX_ARRAY_NUM);
    if (NULL == wifi_p_g->scan_results_all_g) {
        ERROR("scan_results_all_g malloc failed \n");
    } else {
        memset(wifi_p_g->scan_results_all_g, 0, sizeof(scan_results) + sizeof(ap_info)*MAX_ARRAY_NUM);
        wifi_p_g->scan_results_all_g->ap_num = 0;
    }

    wifi_p_g->start_softap = start_softap;
    wifi_p_g->stop_softap = stop_softap;
    wifi_p_g->is_softap_enabled = is_softap_enabled;
    wifi_p_g->scan_ap = scan_ap;
    wifi_p_g->start_sta = start_sta;
    wifi_p_g->stop_sta = stop_sta;
    wifi_p_g->is_sta_enabled = is_sta_enabled;

    INFO("leave %s \n", __func__);
    return wifi_p_g;
}

void wifi_destroy()
{
    INFO("enter func %s \n", __func__);
    free(wifi_p_g->scan_results_all_g);
    free(wifi_p_g);

    wifi_p_g = NULL;
    INFO("leave %s \n", __func__);
}
#endif
