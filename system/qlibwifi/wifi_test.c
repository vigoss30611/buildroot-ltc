#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "wifi.h"

#define MAXLINE 4096

int sta_enable_g = 0;
int sta_connect_g = 0;
int ap_enable_g = 0;

void show_results(struct wifi_config_t *config)
{
    printf("netword_id is %d\n", config->network_id);
    printf("status is %d \n", config->status);
    printf("list_type is %d \n", config->list_type);
    printf("level is %d \n", config->level);
    printf("channel is %d \n", config->channel);
    printf("ssid is %s \n", config->ssid);
    printf("bssid is %s \n", config->bssid);
    printf("key_management is %s \n", config->key_management);
    printf("pairwise_ciphers is %s \n", config->pairwise_ciphers);
    printf("psk is %s \n", config->psk);
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

void test_wifi_state_handle(char *event, void *arg)
{
    struct wifi_config_t *sta_get_scan_result = NULL, *sta_get_scan_result_prev = NULL, *sta_get_config = NULL;

    printf("func %s, event is %s, arg is %s \n", __func__, event, (char *)arg);

    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
	if (!strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
	    sta_enable_g = 1;
	    printf("sta enabled \n");
	} else if (!strncmp((char *)arg, STATE_STA_DISABLED, strlen(STATE_STA_DISABLED))) {
	    sta_enable_g = 0;
	    printf("sta disabled \n");
	} else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
	    printf("scan complete, now get scan results \n");

	    sta_get_scan_result =  wifi_sta_get_scan_result();
	    if (sta_get_scan_result != NULL) {
		show_results(sta_get_scan_result);
	    }

	    while (sta_get_scan_result != NULL) {
		sta_get_scan_result_prev = sta_get_scan_result;

		sta_get_scan_result = wifi_sta_iterate_config(sta_get_scan_result_prev);
		free(sta_get_scan_result_prev);
		if (sta_get_scan_result != NULL) {
		    show_results(sta_get_scan_result);
		}
	    }
	    
	    if (sta_get_scan_result != NULL) {
		free(sta_get_scan_result);
	    }
	} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
	    sta_connect_g = 1;
	    printf("sta connected \n");
	    sta_get_config = wifi_sta_get_config();
	    if (sta_get_config != NULL) {
		show_results(sta_get_config);
		free(sta_get_config);
	    }
	} else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
	    sta_connect_g = 0;
	    printf("sta disconnected \n");
	} else if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
	    ap_enable_g = 1;
	    printf("ap enabled \n");
	} else if (!strncmp((char *)arg, STATE_AP_DISABLED, strlen(STATE_AP_DISABLED))) {
	    ap_enable_g = 0;
	    printf("ap disabled \n");
	} else if (!strncmp((char *)arg, STATE_MONITOR_ENABLED, strlen(STATE_MONITOR_ENABLED))) {
	    printf("monitor enabled");
	} else if (!strncmp((char *)arg, STATE_MONITOR_DISABLED, strlen(STATE_MONITOR_DISABLED))) {
	    printf("monitor disabled");
	}
    } else if (!strncmp(event, STATE_AP_STA_CONNECTED, strlen(STATE_AP_STA_CONNECTED))) {
	printf("ap sta connected info \n");
	show_net_info((struct net_info_t *)arg);
    } else if (!strncmp(event, STATE_AP_STA_DISCONNECTED, strlen(STATE_AP_STA_DISCONNECTED))) {
	printf("ap sta disconected info \n");
	show_net_info((struct net_info_t *)arg);
    }
}

int main(int argc, char *argv[])
{
    struct wifi_config_t sta_connect_config, sta_disconnect_config, *sta_forget_config, *temp_sta_forget_config, ap_set_config, ap_get_config, *ap_get_config_p = &ap_get_config;
    struct net_info_t net_info;
    int i = 0, ret = 0, get_state = 0;
    char buf[MAXLINE], cmd[32], ssid[32], psk[32]; 

    if (argc > 1) {
	printf("more args, usage is: ./wifi_test \n");
	return 0;
    } else {
	printf("start to test, now enter your command, press 'q' to exit \n");
    }

    wifi_start_service(test_wifi_state_handle);

    printf("%% ");	/* print prompt (printf requires %% to print %) */
    while (fgets(buf, MAXLINE, stdin) != NULL) {
	if (buf[strlen(buf) - 1] == '\n')
	    buf[strlen(buf) - 1] = 0; /* replace newline with null */
	printf("your command is %s \n", buf);

	cmd[0] = 0;
	ssid[0] = 0;
	psk[0] = 0;

	sscanf(buf, "%s%s%s", cmd, ssid, psk);

	if (!strcmp(cmd, "sta_enable")) {
	    wifi_sta_enable();
	} else if (!strcmp(cmd, "sta_disable")) {
	    wifi_sta_disable();
	    wifi_stop_service();
	} else if (!strcmp(cmd, "sta_scan")) {
	    for (i=0; i<20; i++) {
		if (sta_enable_g) {
		    wifi_sta_scan();
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "sta_connect")) {
	    printf("cmd is %s, ssid is %s, psk is %s sta_enable_g is %d \n", cmd, ssid, psk, sta_enable_g);
	    for (i=0; i<20; i++) {
		if (sta_enable_g) {
		    strcpy(sta_connect_config.ssid, ssid);
		    strcpy(sta_connect_config.psk, psk);
		    wifi_sta_connect(&sta_connect_config);
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "sta_disconnect")) {
	    printf("cmd is %s, ssid is %s, sta_enable_g is %d \n", cmd, ssid, sta_enable_g);
	    for (i=0; i<20; i++) {
		if (sta_enable_g) {
		    strcpy(sta_disconnect_config.ssid, ssid);
		    wifi_sta_disconnect(&sta_disconnect_config);
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "sta_forget")) {
	    printf("cmd is %s, sta_enable_g is %d \n", cmd, sta_enable_g);
		if (sta_enable_g) {
			sta_forget_config = wifi_sta_get_config();
			while (sta_forget_config != NULL) {
				wifi_sta_forget(sta_forget_config);
                temp_sta_forget_config = sta_forget_config;
                sta_forget_config = wifi_sta_iterate_config(sta_forget_config);
                free(temp_sta_forget_config);
            }
		}
	} else if (!strcmp(cmd, "sta_switch_test")) {
		while (1) {
			wifi_sta_enable();
			sleep(15);
			wifi_sta_disable();
			sleep(15);
		}
	} else if (!strcmp(cmd, "ap_enable")) {
	    wifi_ap_enable();
	} else if (!strcmp(cmd, "ap_disable")) {
	    wifi_ap_disable();
	    wifi_stop_service();
	} else if (!strcmp(cmd, "ap_switch_test")) {
	    while (1) {
		wifi_ap_enable();
		sleep(10);
		wifi_ap_disable();
		sleep(10);
	    }
	} else if (!strcmp(cmd, "ap_set_config")) {
	    for (i=0; i<20; i++) {
		if (1) {
		    strcpy(ap_set_config.ssid, "ap_test");
		    strcpy(ap_set_config.key_management, "wpa2-psk");
		    strcpy(ap_set_config.psk, "11111111");
		    ret = wifi_ap_set_config(&ap_set_config);
		    if (ret) {
			printf("ap_set_config failed \n");
		    }
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "ap_get_config")) {
	    for (i=0; i<20; i++) {
		if (1) {
		    ret = wifi_ap_get_config(ap_get_config_p);
		    if (ret) {
			printf("ap_get_config failed \n");
		    }
		    if (ap_get_config_p != NULL) {
			show_results(ap_get_config_p);
		    }
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "monitor_enable")) {
	    wifi_monitor_enable();
	} else if (!strcmp(cmd, "monitor_disable")) {
	    wifi_monitor_disable();
	} else if (!strcmp(cmd, "get_state")) {
	    get_state = wifi_get_state();
	    printf("get_state is 0x%x \n", get_state);
	} else if (!strcmp(cmd, "get_net_info")) {
	    for (i=0; i<20; i++) {
		if (ap_enable_g || sta_connect_g) {
		    ret = wifi_get_net_info(&net_info);
		    if (!ret)
			show_net_info(&net_info);
		    break;
		}
		usleep(200000);
	    }
	} else if (!strcmp(cmd, "q")) {
	    exit(0);
	} else {
	    printf("invalid cmd \n");
	}

	printf("%% ");
    }

    exit(0);
}
