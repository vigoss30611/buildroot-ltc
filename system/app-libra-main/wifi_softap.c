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
//#include <common_debug.h>
#include "wifi_softap.h"

#define DEBUG_INFO   printf
#define DEBUG_ERR  printf

#define START_SOFTAP_TIMEOUT_SEC 10
#define STOP_SOFTAP_TIMEOUT_SEC 3

#define START_STA_TIMEOUT_SEC 20
#define STOP_STA_TIMEOUT_SEC 3

#define MAX_ARRAY_NUM 50
#define MAX_DATA_SIZE_BYTES (1024*4)

#define GUI_WIFI_8189ES
#ifdef GUI_WIFI_ESP8089
static char const*const WIFI_TOOL_PATH = "/root/libra/wifi/";
static char const*const WIFI_SETUP_FILE = "esp8089_setup.sh";
static char const*const WIFI_DRIVER_NAME = "esp8089";
#elif defined(GUI_WIFI_8189ES)
static char const*const WIFI_TOOL_PATH = "/root/libra/wifi/";
static char const*const WIFI_SETUP_FILE = "rtl8189es_setup.sh";
static char const*const WIFI_DRIVER_NAME = "8189es";
#else
static char const*const WIFI_TOOL_PATH = "/root/libra/wifi/";
static char const*const WIFI_SETUP_FILE = "ap6210_setup.sh";
static char const*const WIFI_DRIVER_NAME = "bcmdhd";
#endif

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

int popen_wifi_cmd(char *cmd)
{
    FILE *pf;
    char pf_stream[1] = {0};
    int wc_result_lines = 0;

    pf = popen(cmd, "r");
    if (NULL == pf) {
	DEBUG_ERR("popen %s failed \n", cmd);
    } else {
	if (1 != fread(pf_stream, 1, 1, pf)) {
	    DEBUG_ERR("fread pf error \n");
	}

	wc_result_lines = atoi(pf_stream);

	if (pclose(pf)) {
	    DEBUG_ERR("pclose %s failed, error is %d \n", cmd, errno);
	}
    }

    return wc_result_lines;
}

static int insmod_net_ko()
{
    static int insmoded = 0;
    if(!insmoded) {
        system("/root/libra/wifi/insmod_net_ko.sh");
        insmoded = 1;
    }
    return 0;
}

int start_softap(char *ssid, char *key)
{
    char cmd[128];
    int ret = 0, i = 0;
    DEBUG_INFO("enter func %s \n", __func__);
    insmod_net_ko();
    if (SOFTAP_ENABLED == is_softap_enabled()) {
	stop_softap();
    }
    
    if (NULL != ssid && NULL != key) {
	sprintf(cmd, "%s/%s AP %s WPA-PSK %s &", WIFI_TOOL_PATH, WIFI_SETUP_FILE, ssid, key);
    } else {
	sprintf(cmd, "%s/%s AP &", WIFI_TOOL_PATH, WIFI_SETUP_FILE);
    }

    system(cmd);

    for (i=0; i<START_SOFTAP_TIMEOUT_SEC; i++) {
	if (SOFTAP_ENABLED == is_softap_enabled()) {
	    break;
	}

	sleep(1);
    }

    if (i >= START_SOFTAP_TIMEOUT_SEC) {
	wifi_status_g = SOFTAP_FAILED;
	ret = wifi_status_g;
    }

    DEBUG_INFO("leave %s \n", __func__);

    return ret;
}

int stop_softap(void)
{
    int ret = 0, i = 0;
    char popen_cmd[128] = {0}, rm_wifi_cmd[32] = {0};
    DEBUG_INFO("enter func %s \n", __func__);

    system("killall -9 hostapd");

    system("killall -9 udhcpd");

    system("ifconfig wlan0 down");
    
    sprintf(rm_wifi_cmd, "rmmod %s", WIFI_DRIVER_NAME);
    system(rm_wifi_cmd);

    sprintf(popen_cmd, "lsmod | grep %s | wc -l", WIFI_DRIVER_NAME);

    for (i=0; i<STOP_SOFTAP_TIMEOUT_SEC; i++) {
	if (0 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = SOFTAP_DISABLED;
	    break;
	}

	sleep(1);
    }
    
    if (i >= STOP_SOFTAP_TIMEOUT_SEC) {
	wifi_status_g = SOFTAP_FAILED;
	ret = wifi_status_g;
    }

    DEBUG_INFO("leave %s \n", __func__);
    return ret;
}

int is_softap_enabled(void)
{
    char popen_cmd[128] = {0};

    sprintf(popen_cmd, "ifconfig | grep wlan0 | wc -l");
    if (1 == popen_wifi_cmd(popen_cmd)) {
	wifi_status_g = SOFTAP_ENABLED;
    } else {
	wifi_status_g = SOFTAP_DISABLED;
    }

    return (int)wifi_status_g;

    if (SOFTAP_ENABLED == wifi_status_g) {
	sprintf(popen_cmd, "hostapd_cli status | grep ENABLED | wc -l");
	if (1 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = SOFTAP_ENABLED;
	} else {
	    wifi_status_g = SOFTAP_DISABLED;
	}
    }

    if (SOFTAP_ENABLED == wifi_status_g) {
	sprintf(popen_cmd, "ps | awk `{printf $5}` | grep udhcpd | wc -l");
	if (1 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = SOFTAP_ENABLED;
	} else {
	    wifi_status_g = SOFTAP_DISABLED;
	}
    }

    return (int)wifi_status_g;
}

int scan_ap(void)
{
    router_status router_status_l = {{0},{0},{0},{0},{0}};
    router_status_p router_status_p_l = &router_status_l;
    FILE *fp;
    char buf[256] = {0};
    int index = 0, size_w, ret = 0;

    DEBUG_INFO("enter func %s \n", __func__);

    if (NULL != wifi_p_g->scan_results_all_g && SOFTAP_ENABLED == is_softap_enabled()) {
	char cmd[128];
	sprintf(cmd, "wpa_supplicant -B -Dwext -iwlan0 -c%s/wpa_supplicant.conf &", WIFI_TOOL_PATH);
	system(cmd);

	sleep(2);

	sprintf(cmd, "wpa_cli scan_results &");

	fp = popen(cmd, "r");
	if (NULL == fp) {
	    DEBUG_INFO("popen scan_results failed \n");
	    return SCAN_AP_FAILED;
	}

	wifi_p_g->scan_results_all_g->ap_num = 0;

	while (NULL != fgets(buf, 256, fp)) {
	    DEBUG_INFO("buf %s \n", buf);
	    if (NULL == strstr(buf, "interface") && NULL == strstr(buf, "bssid")) {
		sscanf(buf, "%s%s%s%s%s",
			router_status_p_l->bssid, router_status_p_l->frequency,
			router_status_p_l->signal_level,  router_status_p_l->key_mgmt,
			router_status_p_l->ssid);

		index = wifi_p_g->scan_results_all_g->ap_num;
		strcpy(wifi_p_g->scan_results_all_g->ap_info_array[index].bssid, router_status_p_l->bssid);
		strcpy(wifi_p_g->scan_results_all_g->ap_info_array[index].ssid, router_status_p_l->ssid);
		wifi_p_g->scan_results_all_g->ap_info_array[index].signal_level = atoi(router_status_p_l->signal_level);
		if (NULL != strstr(buf, "WPA-") && NULL != strstr(buf, "WPA2")) {
		    sprintf(wifi_p_g->scan_results_all_g->ap_info_array[index].key_mgmt, "WPA/WPA2");
		} else if (NULL != strstr(buf, "WPA-")) {
		    sprintf(wifi_p_g->scan_results_all_g->ap_info_array[index].key_mgmt, "WPA");
		} else if (NULL != strstr(buf, "WPA2")) {
		    sprintf(wifi_p_g->scan_results_all_g->ap_info_array[index].key_mgmt, "WPA2");
		} else if (NULL != strstr(buf, "WEP")) {
		    sprintf(wifi_p_g->scan_results_all_g->ap_info_array[index].key_mgmt, "WEP");
		} else {
		    sprintf(wifi_p_g->scan_results_all_g->ap_info_array[index].key_mgmt, "NONE");
		}

		wifi_p_g->scan_results_all_g->ap_num++;
		if (wifi_p_g->scan_results_all_g->ap_num >= MAX_ARRAY_NUM) {
		    break;
		}
	    }
	}
	pclose(fp);
	system("killall -9 wpa_supplicant");
    } else {
	ret = SCAN_AP_FAILED;
    }

    DEBUG_INFO("leave %s \n", __func__);
    return ret;
}

int start_sta(char *ssid, char *key_mgmt, char *key)
{
    char cmd[128];
    int ret = 0, i = 0;
    DEBUG_INFO("enter func %s \n", __func__);

    if (NULL == ssid) {
	DEBUG_ERR("ssid is NULL. \n");
	return -1;
    }

    insmod_net_ko();

    stop_sta();

    strcpy(ssid_g, ssid);
    if (NULL == key) {
	key_g[0] = '\0';
    } else {
	strcpy(key_g, key);
    }
   
    if (NULL == key_mgmt || NULL == key) {
	sprintf(cmd, "%s/%s STA %s NONE &", WIFI_TOOL_PATH, WIFI_SETUP_FILE, ssid);
	strcpy(key_mgmt_g, "NONE");
    } else {
	sprintf(cmd, "%s/%s STA %s %s %s &", WIFI_TOOL_PATH, WIFI_SETUP_FILE, ssid, key_mgmt, key);
	strcpy(key_mgmt_g, key_mgmt);
    }

    system(cmd);

    for (i=0; i<START_STA_TIMEOUT_SEC; i++) {
	if (STA_ENABLED == is_sta_enabled()) {
	    break;
	}

	sleep(1);
    }

    if (i >= START_STA_TIMEOUT_SEC) {
	ret = STA_FAILED;
    }

    DEBUG_INFO("leave %s \n", __func__);

    return ret;
}

int stop_sta(void)
{
    int ret = 0, i = 0;
    char popen_cmd[128] = {0}, rm_wifi_cmd[32] = {0};
    DEBUG_INFO("enter func %s \n", __func__);

    system("killall -9 wpa_supplicant");

    system("killall -9 dhcpcd");

    system("ifconfig wlan0 down");
    
    sprintf(rm_wifi_cmd, "rmmod %s", WIFI_DRIVER_NAME);
    system(rm_wifi_cmd);

    sprintf(popen_cmd, "lsmod | grep %s | wc -l", WIFI_DRIVER_NAME);


    for (i=0; i<STOP_STA_TIMEOUT_SEC; i++) {
	if (0 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = STA_DISABLED;
	    break;
	}

	sleep(1);
    }
    
    if (i >= STOP_STA_TIMEOUT_SEC) {
	wifi_status_g = STA_FAILED;
	ret = wifi_status_g;
    }

    DEBUG_INFO("leave %s \n", __func__);
    return ret;
}

int is_sta_enabled(void)
{
    char popen_cmd[128] = {0};
    DEBUG_INFO("enter func %s \n", __func__);

    sprintf(popen_cmd, "ifconfig | grep wlan0 | wc -l");
    if (1 == popen_wifi_cmd(popen_cmd)) {
	wifi_status_g = STA_ENABLED;
    } else {
	wifi_status_g = STA_DISABLED;
    }

    if (STA_ENABLED == wifi_status_g) {
	sprintf(popen_cmd, "wpa_cli status | grep wpa_state | wc -l");
	if (1 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = STA_ENABLED;
	} else {
	    wifi_status_g = STA_WRONG_KEY_MGMT | STA_WRONG_KEY;
	}
    }

    if (STA_ENABLED == wifi_status_g) {
	sprintf(popen_cmd, "wpa_cli status | grep COMPLETED | wc -l");
	if (1 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = STA_ENABLED;
	} else {
	    sprintf(popen_cmd, "wpa_cli scan_result \
		    | grep -w %s | wc -l", ssid_g);
	    if (0 == popen_wifi_cmd(popen_cmd)) {
		wifi_status_g = STA_SSID_NOT_EXIST; 
	    } else {
		if (NULL != strstr(key_mgmt_g, "WPA-PSK")) {
		    /*if key_mgmt_g='WPA-PSK', it supports both WPA-PSK and WPA2-PSK mode 
		      set by router, so we intercept 'PSK' from key_mgmt_g to compare them.*/
		    char key_mgmt_intercept[16] = {0};
		    sscanf(key_mgmt_g, "%*[^-]-%s", key_mgmt_intercept);
		    sprintf(popen_cmd, "wpa_cli scan_result \
			    | grep -w %s | grep %s | wc -l", ssid_g, key_mgmt_intercept);
		} else if (NULL != strstr(key_mgmt_g, "NONE") && '\0' != key_g[0]) {
		    sprintf(popen_cmd, "wpa_cli scan_result \
			    | grep -w %s | grep WEP | wc -l", ssid_g);
		} else {
		    sprintf(popen_cmd, "wpa_cli scan_result \
			    | grep -w %s | wc -l", ssid_g);
		}
		if (0 == popen_wifi_cmd(popen_cmd)) {
		    wifi_status_g = STA_WRONG_KEY_MGMT; 
		} else {
		    wifi_status_g = STA_WRONG_KEY; 
		}
	    }
	}
    }

    if (STA_ENABLED == wifi_status_g) {
	sprintf(popen_cmd, "ifconfig wlan0 | grep 'inet addr' | wc -l");
	if (1 == popen_wifi_cmd(popen_cmd)) {
	    wifi_status_g = STA_ENABLED;
	} else {
	    wifi_status_g = STA_GET_IP_FAILED;
	}
    }

    DEBUG_INFO("leave %s \n", __func__);
    return (int)wifi_status_g;
}

wifi_p wifi_create()
{
    DEBUG_INFO("enter func %s \n", __func__);

    wifi_p_g = (wifi_p)malloc(sizeof(wifi));
    wifi_p_g->scan_results_all_g = (scan_results_p)malloc(sizeof(scan_results) + sizeof(ap_info)*MAX_ARRAY_NUM);
    if (NULL == wifi_p_g->scan_results_all_g) {
	DEBUG_ERR("scan_results_all_g malloc failed \n");
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

    DEBUG_INFO("leave %s \n", __func__);
    return wifi_p_g;
}

void wifi_destroy()
{
    DEBUG_INFO("enter func %s \n", __func__);
    free(wifi_p_g->scan_results_all_g);
    free(wifi_p_g);

    wifi_p_g = NULL;
    DEBUG_INFO("leave %s \n", __func__);
}
