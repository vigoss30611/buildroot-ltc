#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <iconv.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <ctype.h>

#include <qsdk/event.h>
#include "wifi.h"
#include "list.h"
#include "lib_wifi.h"

#define WIFI_DEBUG		1
#define BUF_SIZE 		256
#define EVENT_BUF_SIZE  2048
#define WIFI_RPC_NUM	5
#define WIFI_SERVICE_INIT	1
#define WIFI_SERVICE_START	2

#define EVENT_PREFIX_STR "CTRL-EVENT-"
#define SCAN_RESULTS_STR "SCAN-RESULTS"
#define CONNECT_RESULTS_STR "CONNECTED"
#define DISCONNECT_RESULTS_STR "DISCONNECTED"
#define TERMINATING_STR  "TERMINATING"
#define SCAN_ADD_STR  "BSS-ADDED"
#define STATUS_CONNECTED  "COMPLETED"
#define ID_STR 		"id="
#define SSID_STR 	"ssid="
#define BSSID_STR 	"bssid="
#define FLAGS_STR 	"flags="
#define LEVEL_STR 	"level="
#define FREQ_STR 	"freq="
#define SSID_VAR_NAME 		"ssid"
#define BSSID_VAR_NAME		"bssid"
#define PSK_VAR_NAME 		"psk"
#define KEYMGMT_VAR_NAME 	"key_mgmt"

struct cmd_data_t {
	list_head_t list;
	int cmd;
	unsigned cmd_arg;
};

struct Wifi_Service_t {
	list_head_t cmd_list;
	list_head_t scan_list;
	list_head_t store_list;
    int init;
    int state;
    int net_id;
    int cmd_lock;
    int dhcp_done;
    int start_sid;
    int monitor_cmd;
    int treminate;
    struct net_info_t net_info;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t service_thread;
    pthread_mutex_t monitor_mutex;
    pthread_cond_t monitor_cond;
    pthread_t monitor_thread;
};

static const char *wifi_mode[] = {
	"AP",
	"STA",
	"MONITOR",
};

#if WIFI_DEBUG
static const char *wifi_cmd[] = {
    "exit",
    "sta enable",
    "sta disable",
    "sta scan",
    "sta connect",
    "sta forget",
    "sta disconnect",
    "ap enable",
    "ap disable",
    "monitor enable",
    "monitor disable",
    "wps connect",
    "service idle",
    "none",
};
#endif

#if WIFI_DEBUG
#define  LOGD(...)   printf(__VA_ARGS__)
#else
#define  LOGD(...)
#endif
#define  ERROR(...)   printf(__VA_ARGS__)

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

struct Wifi_Service_t *wifi_service_save = NULL;

static int docommand(int mode, const char *cmd)
{
	char replybuf[BUF_SIZE];
	int replybuflen = BUF_SIZE;
    size_t reply_len = replybuflen - 1;

    if (wifi_command(mode, cmd, replybuf, &reply_len) != 0)
        return -1;
    else {
        // Strip off trailing newline
        if (reply_len > 0 && replybuf[reply_len-1] == '\n')
            replybuf[reply_len-1] = '\0';
        else
            replybuf[reply_len] = '\0';
        return 0;
    }
}

static int dostringcommand(int mode, const char *cmd, char *replybuf)
{
	int replybuflen = EVENT_BUF_SIZE;
    size_t reply_len = replybuflen - 1;

    if (wifi_command(mode, cmd, replybuf, &reply_len) != 0)
        return -1;
    else {
        // Strip off trailing newline
        if (reply_len > 0 && replybuf[reply_len-1] == '\n')
            replybuf[reply_len-1] = '\0';
        else
            replybuf[reply_len] = '\0';
        return 0;
    }
}

static int dointcommand(int mode, const char *cmd)
{
	char replybuf[BUF_SIZE];
	int replybuflen = BUF_SIZE;
    size_t reply_len = replybuflen - 1;

    if (wifi_command(mode, cmd, replybuf, &reply_len) != 0) {
        return -1;
    } else {
        // Strip off trailing newline
        if (reply_len > 0 && replybuf[reply_len-1] == '\n')
            replybuf[reply_len-1] = '\0';
        else
            replybuf[reply_len] = '\0';
        return atoi(replybuf);
    }
}

static void wifi_service_add_cmd(int cmd, unsigned cmd_arg)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	struct cmd_data_t *cmd_data = NULL;

	pthread_mutex_lock(&wifi_service->mutex);
	cmd_data = (struct cmd_data_t *)malloc(sizeof(struct cmd_data_t));
	if (cmd_data == NULL) {
		pthread_mutex_unlock(&wifi_service->mutex);
		return;
	}
	cmd_data->cmd = cmd;
	cmd_data->cmd_arg = cmd_arg;
	list_add_tail(&cmd_data->list, &wifi_service->cmd_list);
	if (wifi_service->init == WIFI_SERVICE_START)
		pthread_cond_signal(&wifi_service->cond);
	if (cmd >= 0)
		LOGD("wifi add cmd %s \n", wifi_cmd[cmd]);

	pthread_mutex_unlock(&wifi_service->mutex);

	return;
}

static void wifi_service_state_cb(char *state)
{
	event_send(WIFI_STATE_EVENT, state, (strlen(state) + 1));
	return;
}

static struct wifi_config_t *wifi_sta_search_config(struct wifi_config_t *wifi_config, int type)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	struct wifi_config_t *wifi_save_config;
	static iconv_t chinese_it = 0;
	list_head_t *l, *n;
	list_head_t *search_list;
	char temp[32];
	int in_size, out_size;
	char *ptr1 = wifi_config->ssid;
	char *ptr2 = temp;

	memset(temp, 0, 32);
	chinese_it = iconv_open("GBK", "UTF-8");
	if (chinese_it !=  (iconv_t)(-1)) {
		in_size = strlen(wifi_config->ssid);
		out_size = 32;
		iconv(chinese_it, &ptr1, (size_t*)(&in_size), &ptr2, (size_t*)(&out_size));
		iconv_close(chinese_it);
	}

	if (type == AP_SAVE)
		search_list = &wifi_service->store_list;
	else
		search_list = &wifi_service->scan_list;
	list_for_each_safe(l, n, search_list) {
		wifi_save_config = container_of(l, struct wifi_config_t, list);
		if ((!memcmp(wifi_save_config->ssid, wifi_config->ssid, strlen(wifi_config->ssid)))
			 && (strlen(wifi_config->ssid) == strlen(wifi_save_config->ssid))) {
			return wifi_save_config;
		} else if ((!memcmp(wifi_save_config->ssid, temp, strlen(temp)))
			 && (strlen(temp) == strlen(wifi_save_config->ssid))) {
			LOGD("wifi ap GBK encode %s \n", wifi_config->ssid);
			return wifi_save_config;
		}
	}

	return NULL;
}

static int wifi_get_channel_by_freq(int freq)
{
	int channel = -1;

	switch (freq) {
		case 2412:
			channel = 1;
			break;
		case 2417:
			channel = 2;
			break;
		case 2422:
			channel = 3;
			break;
		case 2427:
			channel = 4;
			break;
		case 2432:
			channel = 5;
			break;
		case 2437:
			channel = 6;
			break;
		case 2442:
			channel = 7;
			break;
		case 2447:
			channel = 8;
			break;
		case 2452:
			channel = 9;
			break;
		case 2457:
			channel = 10;
			break;
		case 2462:
			channel = 11;
			break;
		case 2467:
			channel = 12;
			break;
		case 2472:
			channel = 13;
			break;
		case 2484:
			channel = 14;
			break;
		case 5745:
			channel = 149;
			break;
		case 5765:
			channel = 153;
			break;
		case 5785:
			channel = 157;
			break;
		case 5805:
			channel = 161;
			break;
		case 5825:
			channel = 165;
			break;
		default:
			break;
	}

	return channel;
}

static void wifi_set_scan_result(int start_sid)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	char replybuf[EVENT_BUF_SIZE];
	char cmd[BUF_SIZE];
	struct wifi_config_t *wifi_config = NULL, *wifi_save_config;
	int sid = start_sid, temp_sid = start_sid, temp, add_flag = 0, freq;
	char split[] = "\n", value[16];
	char *result = NULL, *temp_ssid = NULL, *temp_flag;
	list_head_t *l, *n;

	/*list_for_each_safe(l, n, &wifi_service->scan_list) {
		wifi_config = container_of(l, struct wifi_config_t, list);
		sid = wifi_config->network_id + 1;
	}*/
	list_for_each_safe(l, n, &wifi_service->scan_list) {
		wifi_config = container_of(l, struct wifi_config_t, list);
		list_del(&wifi_config->list);
		free(wifi_config);
	}

	while (sid != -1) {
		if (sid < temp_sid)
			break;
		sprintf(cmd, "%s%d%s", "BSS RANGE=", sid, "- MASK=0x21987");
		memset(replybuf, 0, EVENT_BUF_SIZE);
		dostringcommand(STA, cmd, replybuf);
		temp_sid = sid;

		sid = -1;
		wifi_config = NULL;
		result = strtok(replybuf, split);
		if (result == NULL)
			break;
		while (result != NULL){

			if (!memcmp(result, ID_STR, strlen((const char*)ID_STR))) {
				if (add_flag) {
					if (!wifi_sta_search_config(wifi_config, AP_SCAN)) {
						wifi_save_config = wifi_sta_search_config(wifi_config, AP_SAVE);
						if (wifi_save_config) {
							wifi_config->network_id = wifi_save_config->network_id;
							wifi_config->status = wifi_save_config->status;
						}
						list_add_tail(&wifi_config->list, &wifi_service->scan_list);
						LOGD("scan result %d %s %s %s %d \n", sid, wifi_config->ssid, wifi_config->bssid, wifi_config->key_management, wifi_config->channel);
					} else {
						free(wifi_config);
					}
				}

				sid = atoi(result + strlen((const char*)ID_STR)) + 1;
				wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
				if (wifi_config == NULL)
					return;
				memset(wifi_config, 0, sizeof(struct wifi_config_t));
				wifi_config->list_type = AP_SCAN;
				wifi_config->network_id = sid;
				add_flag = 0;
			}

			if (!memcmp(result, SSID_STR, strlen((const char*)SSID_STR))) {
				temp_ssid = result + strlen((const char*)SSID_STR);
				temp = 0;
				while (temp < 32) {
					if ((temp_ssid[temp] == 0x5c) && (temp_ssid[temp + 1] == 0x78))
						break;
					temp ++;
				}
				if (temp < 32) {
					if (temp > 0)
						memcpy(wifi_config->ssid, temp_ssid, temp);
					temp_ssid += temp;
					while ((temp_ssid[0] == 0x5c) && (temp_ssid[1] == 0x78)) {
						temp_ssid += 2;
						memcpy(value, temp_ssid, 2);
						value[2] = '\0';
						wifi_config->ssid[temp] = strtol(value, NULL, 16);
						temp++;
						temp_ssid += 2;
					}
					while ((temp_ssid[0] != '\0') && (temp < 32)) {
						wifi_config->ssid[temp] = temp_ssid[0];
						temp_ssid++;
						temp++;
					}
					wifi_config->ssid[temp] = '\0';
				} else {
					snprintf(wifi_config->ssid, sizeof(wifi_config->ssid), "%s", result + strlen((const char*)SSID_STR));
				}
				add_flag = 1;
			} else if (!memcmp(result, BSSID_STR, strlen((const char*)BSSID_STR))) {
				snprintf(wifi_config->bssid, sizeof(wifi_config->bssid), "%s", result + strlen((const char*)BSSID_STR));
			} else if (!memcmp(result, LEVEL_STR, strlen((const char*)LEVEL_STR))) {
				wifi_config->level = atoi(result + strlen((const char*)LEVEL_STR) + 1);
			} else if (!memcmp(result, FLAGS_STR, strlen((const char*)FLAGS_STR))) {
				temp_flag = (char *)(result + strlen((const char*)FLAGS_STR) + 1);
				temp = 0;
				while (temp < 64) {
					if (temp_flag[temp] == 0x5D) {
						temp_flag[temp] = '\0';
						break;
					}
					temp++;
				}
				if (temp < 64)
					snprintf(wifi_config->key_management, sizeof(wifi_config->key_management), "%s", (char *)(result + strlen((const char*)FLAGS_STR) + 1));
			} else if (!memcmp(result, FREQ_STR, strlen((const char*)FREQ_STR))) {
				freq = atoi(result + strlen((const char*)FREQ_STR));
				wifi_config->channel = wifi_get_channel_by_freq(freq);
			}

			result = strtok(NULL, split);
		}
		if (wifi_config != NULL) {
			if (add_flag) {
				if (!wifi_sta_search_config(wifi_config, AP_SCAN)) {
					wifi_save_config = wifi_sta_search_config(wifi_config, AP_SAVE);
					if (wifi_save_config) {
						wifi_config->network_id = wifi_save_config->network_id;
						wifi_config->status = wifi_save_config->status;
					}
					list_add_tail(&wifi_config->list, &wifi_service->scan_list);
					LOGD("scan result end %d %s %s %s %d \n", sid, wifi_config->ssid, wifi_config->bssid, wifi_config->key_management, wifi_config->channel);
				} else {
					free(wifi_config);
				}
			} else {
				if (sid > 1)
					sid--;
				free(wifi_config);
			}
		}
		add_flag = 0;
	}

	return;
}

static void wifi_sta_load_store_config(void)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	char replybuf[EVENT_BUF_SIZE];
	struct wifi_config_t *wifi_config = NULL, *wifi_config_save = NULL;
	int i = 0, j, temp;
	char split[] = "\n";
	char *result = NULL;
	char *line[256], value[128], *line_node[8], *temp_ssid;
	list_head_t *l, *n;

	list_for_each_safe(l, n, &wifi_service->store_list) {
		wifi_config = container_of(l, struct wifi_config_t, list);
		list_del(&wifi_config->list);
		free(wifi_config);
	}

	dostringcommand(STA, "LIST_NETWORKS", replybuf);

	memset(line, 0, 256);
	result = strtok(replybuf, split);
	while ((result != NULL) && (i < 256)) {
		line[i++] = result;
		result = strtok(NULL, split);
	}

	i = 1;
	while (line[i] != NULL) {
		memset(line_node, 0, 8);
		wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
		if (wifi_config == NULL)
			return;

		j = 0;
		result = strtok(line[i], "\t");
		while ((result != NULL) && (j < 8)) {
			line_node[j++] = result;
			result = strtok(NULL, "\t");
		}
		if ((line_node[0] == NULL) || (line_node[1] == NULL) || (line_node[2] == NULL)) {
			free(wifi_config);
			return;
		}

		wifi_config->list_type = AP_SAVE;
		wifi_config->network_id = atoi(line_node[0]);
		temp_ssid = line_node[1];
		temp = 0;
		while (temp < 32) {
			if ((temp_ssid[temp] == 0x5c) && (temp_ssid[temp + 1] == 0x78))
				break;
			temp ++;
		}
		if (temp < 32) {
			memcpy(wifi_config->ssid, temp_ssid, temp);
			temp_ssid += temp;
			while ((temp_ssid[0] == 0x5c) && (temp_ssid[1] == 0x78)) {
				temp_ssid += 2;
				memcpy(value, temp_ssid, 2);
				value[2] = '\0';
				wifi_config->ssid[temp] = strtol(value, NULL, 16);
				temp++;
				temp_ssid += 2;
			}
			while ((temp_ssid[0] != '\0') && (temp < 32)) {
				wifi_config->ssid[temp] = temp_ssid[0];
				temp_ssid++;
				temp++;
			}
			wifi_config->ssid[temp] = '\0';
		}  else {
			snprintf(wifi_config->ssid, sizeof(wifi_config->ssid), "%s", line_node[1]);
		}

		if (line_node[3] != NULL)
			snprintf(wifi_config->bssid, sizeof(wifi_config->bssid), "%s", line_node[2]);
		else
			line_node[3] = line_node[2];

		if (!strncmp(line_node[3], "[CURRENT]", strlen("[CURRENT]")))
			wifi_config->status = CURRENT;
		else if (!strncmp(line_node[3], "[DISABLED]", strlen("[DISABLED]")))
			wifi_config->status = DISABLED;
		else
			wifi_config->status = ENABLED;
		wifi_config_save = wifi_sta_search_config(wifi_config, AP_SCAN);
		if (wifi_config_save)
			wifi_config_save->status = wifi_config->status;

		list_add_tail(&wifi_config->list, &wifi_service->store_list);
		LOGD("store ap list %d %s %d \n", wifi_config->network_id, wifi_config->ssid, wifi_config->status);

		i++;
	}

	return;
}

static int wifi_sta_check_status(char *status)
{
	char replybuf[BUF_SIZE];
	char split[] = "\n";
	char *result = NULL;

	dostringcommand(STA, "STATUS", replybuf);
	result = strtok(replybuf, split);
	while (result != NULL) {
		if (!strncmp(result, "wpa_state=", strlen("wpa_state="))) {
			sprintf(status, result + strlen("wpa_state="));
			return 0;
		} else if (!strncmp(result, "OK", strlen("OK")))  {
			sprintf(status, "OK");
			return 0;
		}
		result = strtok(NULL, split);
	}

	status[0] = '\0';
	return -1;
}

int wifi_get_dhcp_info(struct Wifi_Service_t *wifi_service)
{
	struct net_info_t *net_info = &wifi_service->net_info;
	uint32_t ipaddr = 0, gateway = 0, mask = 0;
	uint32_t dns1 = 0, dns2 = 0, server = 0, lease = 0;

	do_dhcp_request(&ipaddr, &gateway, &mask, &dns1, &dns2, &server, &lease);
	sprintf(net_info->ipaddr, "%d.%d.%d.%d", (ipaddr & 0xff), ((ipaddr >> 8) & 0xff), ((ipaddr >> 16) & 0xff), ((ipaddr >> 24) & 0xff));
	sprintf(net_info->gateway, "%d.%d.%d.%d", (gateway & 0xff), ((gateway >> 8) & 0xff), ((gateway >> 16) & 0xff), ((gateway >> 24) & 0xff));
	sprintf(net_info->mask, "%d.%d.%d.%d", (mask & 0xff), ((mask >> 8) & 0xff), ((mask >> 16) & 0xff), ((mask >> 24) & 0xff));
	sprintf(net_info->dns1, "%d.%d.%d.%d", (dns1 & 0xff), ((dns1 >> 8) & 0xff), ((dns1 >> 16) & 0xff), ((dns1 >> 24) & 0xff));
	sprintf(net_info->dns2, "%d.%d.%d.%d", (dns2 & 0xff), ((dns2 >> 8) & 0xff), ((dns2 >> 16) & 0xff), ((dns2 >> 24) & 0xff));
	sprintf(net_info->server, "%d.%d.%d.%d", (server & 0xff), ((server >> 8) & 0xff), ((server >> 16) & 0xff), ((server >> 24) & 0xff));
	sprintf(net_info->lease, "%d.%d.%d.%d", (lease & 0xff), ((lease >> 8) & 0xff), ((lease >> 16) & 0xff), ((lease >> 24) & 0xff));

	LOGD("dhcp info ip:%s gw:%s mask:%s dns1:%s dns2:%s \n", net_info->ipaddr, net_info->gateway, net_info->mask, net_info->dns1, net_info->dns2);
	return !server;
}

static void * wifi_monitor_thread(void *arg)
{
	struct Wifi_Service_t *wifi_service = (struct Wifi_Service_t *)arg;
	struct timeval now_time;
	struct timespec timeout_time;
	char buf[EVENT_BUF_SIZE], *temp;
	int ret, cnt = 0, cmd = CMD_NONE;

	prctl(PR_SET_NAME, "wifi_monitor");
	while (1) {

		if (cmd == CMD_QUIT) {
			LOGD("wifi monitor thread quit \n");
			break;
		}

		switch (cmd) {
			case STA_STATUS_CHECK:
				ret = wifi_sta_check_status(buf);
				if (ret >= 0) {
					if (!strncmp(buf, STATUS_CONNECTED, strlen(STATUS_CONNECTED))) {
						LOGD("wifi default connect sucess %s \n", buf);
						wifi_service->state = WIFI_STA_STATE_CONNECTED;
						wifi_sta_load_store_config();
						if (!wifi_service->dhcp_done) {
							cnt = 0;
							ret = 0;
							do {
								ret = wifi_start_dhcpcd(STA, (cnt + 1) * 10);
								cnt++;
							} while ((ret > 0) && (cnt < 2));
						}

						if ((ret == 0) && !wifi_service->dhcp_done) {
							wifi_get_dhcp_info(wifi_service);
							wifi_service_state_cb(STATE_STA_CONNECTED);
							wifi_service->dhcp_done = 1;
							wifi_service->monitor_cmd = STA_WAIT_EVENT;
						}
					} else {
						cnt++;
						if (cnt >= 100) {
							ERROR("wifi default connect fail %s \n", buf);
							wifi_service->monitor_cmd = STA_WAIT_EVENT;
							cnt = 0;
						}
					}
				} else {
					ERROR("wifi wait default connect fail %d %s \n", ret, buf);
				}
				break;
			case STA_WAIT_EVENT:
				ret = wifi_wait_for_event(STA, buf, EVENT_BUF_SIZE);
				if (ret > 0) {
					if (!memcmp(buf, EVENT_PREFIX_STR, strlen((const char*)EVENT_PREFIX_STR))) {
						//LOGD("wifi event %s \n", buf);
						if (!memcmp(buf + strlen(EVENT_PREFIX_STR), TERMINATING_STR, strlen(TERMINATING_STR))) {
							wifi_stop_dhcpcd(STA);
							wifi_stop_supplicant(0);
							wifi_unload_driver(STA);
							wifi_service->state = WIFI_STA_STATE_DISABLED;
							wifi_service_state_cb(STATE_STA_DISABLED);
							cmd = CMD_NONE;
							LOGD("wifi event sta disabled \n");
						} else if (!memcmp(buf + strlen(EVENT_PREFIX_STR), SCAN_ADD_STR, strlen(SCAN_ADD_STR))) {
							temp = buf + strlen(EVENT_PREFIX_STR) + strlen(SCAN_ADD_STR) + 1;
							ret = 0;
							while (temp[ret] != 0x20) {
								ret++;
							}
							temp[ret] = '\0';
							//if (atoi(temp) < wifi_service->start_sid)
							if (wifi_service->start_sid == -1)
								wifi_service->start_sid = atoi(temp);
							//LOGD("wifi event scan add %s %d \n", buf, wifi_service->start_sid);
						} else if (!memcmp(buf + strlen(EVENT_PREFIX_STR), SCAN_RESULTS_STR, strlen(SCAN_RESULTS_STR))) {
							//wifi_set_scan_result(wifi_service->start_sid);
							wifi_set_scan_result(0);
							wifi_service_state_cb(STATE_SCAN_COMPLETE);
							wifi_service->state = WIFI_STA_STATE_SCANED;
							if (wifi_service->start_sid > 0)
								LOGD("wifi event scan complete %d \n", wifi_service->start_sid);
							wifi_service->start_sid = -1;
						} else if (!memcmp(buf + strlen(EVENT_PREFIX_STR), CONNECT_RESULTS_STR, strlen(CONNECT_RESULTS_STR))) {
							ret = 0;
							cnt = 0;
							wifi_service->state = WIFI_STA_STATE_CONNECTED;
							wifi_sta_load_store_config();
							if (!wifi_service->dhcp_done) {
								do {
									ret = wifi_start_dhcpcd(STA, (cnt + 1) * 10);
									cnt++;
								} while ((ret > 0) && (cnt < 2));
							}

							if ((ret == 0) && !wifi_service->dhcp_done) {
								wifi_get_dhcp_info(wifi_service);
								wifi_service_state_cb(STATE_STA_CONNECTED);
								wifi_service->dhcp_done = 1;
							}
							LOGD("wifi event sta connect sucess %d \n", ret);
						} else if (!memcmp(buf + strlen(EVENT_PREFIX_STR), DISCONNECT_RESULTS_STR, strlen(DISCONNECT_RESULTS_STR))) {
							if (!wifi_service->treminate)
								wifi_sta_load_store_config();
							wifi_service_state_cb(STATE_STA_DISCONNECTED);
							wifi_service->state = WIFI_STA_STATE_DISCONNECTED;
							wifi_service->dhcp_done = 0;
							LOGD("wifi event sta disconnected \n");
						}
					}
				} else {
					ERROR("wifi event fail %d %s \n", ret, buf);
				}
				break;
			default:
				break;
		}

		pthread_mutex_lock(&wifi_service->monitor_mutex);
		if (cmd != CMD_NONE) {
			gettimeofday(&now_time, NULL);
			now_time.tv_usec += 20 * 1000;
			if(now_time.tv_usec >= 1000000) {
				now_time.tv_sec += now_time.tv_usec / 1000000;
				now_time.tv_usec %= 1000000;
			}
			timeout_time.tv_sec = now_time.tv_sec;
			timeout_time.tv_nsec = now_time.tv_usec * 1000;
			pthread_cond_timedwait(&wifi_service->monitor_cond, &wifi_service->monitor_mutex, &timeout_time);
		} else {
			pthread_cond_wait(&wifi_service->monitor_cond, &wifi_service->monitor_mutex);
		}
		cmd = wifi_service->monitor_cmd;
		pthread_mutex_unlock(&wifi_service->monitor_mutex);
		
	}

	return NULL;
}

static void * wifi_service_thread(void *arg)
{
	struct Wifi_Service_t *wifi_service = (struct Wifi_Service_t *)arg;
	struct net_info_t *net_info = &wifi_service->net_info;
	struct wifi_config_t *wifi_config = NULL, *wifi_config_save;
	struct cmd_data_t *cmd_data;
	int cmd, ret = WIFI_STATE_FAILED, cnt = 0, connect_scan = 0, state = WIFI_STATE_FAILED, timeout_cnt = 0;
	unsigned cmd_arg, hwaddr1, hwaddr2;
	char hwaddr[16], temp_cmd[BUF_SIZE];
	list_head_t *l;

	prctl(PR_SET_NAME, "wifi_service");
	wifi_service->init = WIFI_SERVICE_START;
	wifi_service->start_sid = -1;
	for (;;) {

		pthread_mutex_lock(&wifi_service->mutex);
		if (!list_empty(&wifi_service->cmd_list)) {
			l = wifi_service->cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			cmd = cmd_data->cmd;
			cmd_arg = cmd_data->cmd_arg;
			list_del(&cmd_data->list);
			free(cmd_data);
		} else {
			cmd = CMD_NONE;
			cmd_arg = 0;
		}

		if (cmd == CMD_NONE) {
			pthread_cond_wait(&wifi_service->cond, &wifi_service->mutex);
			LOGD("wifi service cmd wake %d \n", wifi_service->state);
			pthread_mutex_unlock(&wifi_service->mutex);
			continue;
		}
		pthread_mutex_unlock(&wifi_service->mutex);

		if (cmd == CMD_QUIT) {
			LOGD("wifi service thread quit \n");
			wifi_service->init = WIFI_SERVICE_INIT;
			break;
		}

		//LOGD("wifi current cmd start %s \n", wifi_cmd[cmd]);
		switch (cmd) {
			case CMD_STA_ENABLE:
				if ((wifi_service->state > 0) && ((wifi_service->state & 0xf) > 1)) {
					wifi_service_state_cb(STATE_STA_ENABLED);
					if (wifi_service->dhcp_done)
						wifi_service_state_cb(STATE_STA_CONNECTED);
					break;
				}
				if (wifi_load_driver(STA, hwaddr) < 0) {
					ERROR("wifi load driver failed %s \n", wifi_mode[STA]);
					wifi_service->state = WIFI_STATE_FAILED;
					break;
				}
				hwaddr1 = (hwaddr[0] | (hwaddr[1] << 8) | (hwaddr[2] << 16) | (hwaddr[3] << 24));
				hwaddr2 = *(unsigned *)(hwaddr + sizeof(unsigned));
				sprintf(net_info->hwaddr, "%x:%x:%x:%x:%x:%x", (hwaddr1 & 0xff), ((hwaddr1 >> 8) & 0xff), ((hwaddr1 >> 16) & 0xff), ((hwaddr1 >> 24) & 0xff), (hwaddr2 & 0xff), ((hwaddr2 >> 8) & 0xff));
				wifi_start_supplicant(0);
				cnt = 0;
				do {
					ret = wifi_connect_to_supplicant(STA);
					if (ret == 0) {
						LOGD("connect suplicant success %d \n", wifi_service->cmd_lock);
						wifi_service->treminate = 0;
						wifi_service->state = WIFI_STA_STATE_ENABLED;
						wifi_sta_load_store_config();
						if (list_empty(&wifi_service->store_list)) {
							wifi_service_add_cmd(CMD_STA_SCAN, 0);
						} else {
							wifi_service->cmd_lock = 1;
							state = WIFI_STA_STATE_CONNECTED;
							ret = wifi_service->state;
							timeout_cnt = 100;
						}
						wifi_service_state_cb(STATE_STA_ENABLED);
						break;
					} else {
						sleep(1);
						cnt++;
					}
				} while (ret && (cnt < 10));
				if (ret && (cnt >= 10)) {
					wifi_service->state = WIFI_STATE_FAILED;
					wifi_service->cmd_lock = 0;
				}
				cnt = 0;
				break;
			case CMD_STA_DISABLE:
				if (!((wifi_service->state > 0) && ((wifi_service->state & 0xf) > 1)))
					break;
				wifi_service->cmd_lock = 1;
				wifi_service->dhcp_done = 0;
				state = WIFI_STA_STATE_DISABLED;
				ret = wifi_service->state;
				timeout_cnt = 100;
				wifi_service->treminate = 1;
				wifi_stop_dhcpcd(STA);
				docommand(STA, "TERMINATE");
				break;
			case CMD_STA_SCAN:
				if (!((wifi_service->state > 0) && ((wifi_service->state & 0xf) > 1)))
					break;
				docommand(STA, "SCAN");
				wifi_service->cmd_lock = 1;
				if (cmd_arg)
					connect_scan++;
				state = WIFI_STA_STATE_SCANED;
				ret = wifi_service->state;
				timeout_cnt = 400;
				break;
			case CMD_STA_CONNECT:
				if (!((wifi_service->state > 0) && ((wifi_service->state & 0xf) > 1)))
					break;
				wifi_config = (struct wifi_config_t *)cmd_arg;
				if (wifi_config) {
					wifi_sta_load_store_config();
					wifi_config_save = wifi_sta_search_config(wifi_config, AP_SAVE);
					if (wifi_config_save == NULL) {
						wifi_config_save = wifi_sta_search_config(wifi_config, AP_SCAN);
						if (wifi_config_save == NULL) {
							if (connect_scan > 5) {
								LOGD("wifi have not scan request ap %s \n", wifi_config->ssid);
								wifi_service->cmd_lock = 0;
								connect_scan = 0;
								free(wifi_config);
								wifi_config = NULL;
								break;
							} else {
								wifi_service_add_cmd(CMD_STA_SCAN, (unsigned)wifi_config);
								wifi_service_add_cmd(CMD_STA_CONNECT, (unsigned)wifi_config);
								wifi_service->cmd_lock = 0;
								continue;
							}
						} else {
							sprintf(wifi_config_save->psk, wifi_config->psk);
							wifi_config_save->network_id = dointcommand(STA, "ADD_NETWORK");
							memcpy(wifi_config, wifi_config_save, sizeof(struct wifi_config_t));
							wifi_config->list_type = AP_SAVE;
							list_add_tail(&wifi_config->list, &wifi_service->store_list);
							connect_scan = 0;
							LOGD("wifi add network %d %s \n", wifi_config_save->network_id, wifi_config_save->ssid);
						}
					} else {
						sprintf(wifi_config_save->psk, wifi_config->psk);
						free(wifi_config);
						wifi_config = NULL;
					}
					sprintf(temp_cmd, "%s%d%s%s%s\"%s\"", "SET_NETWORK ", wifi_config_save->network_id, " ", SSID_VAR_NAME, " ", wifi_config_save->ssid);
					docommand(STA, temp_cmd);
					sprintf(temp_cmd, "%s%d%s%s%s%s", "SET_NETWORK ", wifi_config_save->network_id, " ", BSSID_VAR_NAME, " ", wifi_config_save->bssid);
					docommand(STA, temp_cmd);
					if (wifi_config_save->psk[0] != 0) {
						sprintf(temp_cmd, "%s%d%s%s%s\"%s\"", "SET_NETWORK ", wifi_config_save->network_id, " ", PSK_VAR_NAME, " ", wifi_config_save->psk);
						docommand(STA, temp_cmd);
					} else {
						sprintf(temp_cmd, "%s%d%s%s%s%s", "SET_NETWORK ", wifi_config_save->network_id, " ", KEYMGMT_VAR_NAME, " ", "NONE");
						docommand(STA, temp_cmd);
					}
					sprintf(temp_cmd, "%s%d", "SELECT_NETWORK ", wifi_config_save->network_id);
					docommand(STA, temp_cmd);
					docommand(STA, "SAVE_CONFIG");
				}
				docommand(STA, "RECONNECT");
				wifi_service->cmd_lock = 1;
				state = WIFI_STA_STATE_CONNECTED;
				ret = wifi_service->state;
				timeout_cnt = 1000;
				break;
			case CMD_STA_FORGET:
				if (!((wifi_service->state > 0) && ((wifi_service->state & 0xf) > 1)))
					break;
				sprintf(temp_cmd, "%s%d", "REMOVE_NETWORK ", cmd_arg);
				docommand(STA, temp_cmd);
				docommand(STA, "SAVE_CONFIG");
				wifi_stop_dhcpcd(STA);
				wifi_service->dhcp_done = 0;
				wifi_service->cmd_lock = 1;
				state = WIFI_STA_STATE_DISCONNECTED;
				ret = wifi_service->state;
				timeout_cnt = 100;
				break;
			case CMD_STA_DISCONNECT:
				if ((wifi_service->state > 0) && (wifi_service->state == WIFI_STA_STATE_CONNECTED)) {
					wifi_service->dhcp_done = 0;
					wifi_service->cmd_lock = 1;
					state = WIFI_STA_STATE_DISCONNECTED;
					ret = wifi_service->state;
					timeout_cnt = 100;
					docommand(STA, "DISCONNECT");
				}
				break;
			case CMD_AP_ENABLE:
				if ((wifi_service->state > 0) && ((wifi_service->state & 0xf0) == WIFI_AP_STATE_ENABLED)) {
					wifi_service_state_cb(STATE_AP_ENABLED);
					break;
				}
				if (wifi_load_driver(AP, hwaddr) < 0) {
					ERROR("wifi load driver failed %s \n", wifi_mode[AP]);
					wifi_service->state = WIFI_STATE_FAILED;
				} else {
					hwaddr1 = (hwaddr[0] | (hwaddr[1] << 8) | (hwaddr[2] << 16) | (hwaddr[3] << 24));
					hwaddr2 = *(unsigned *)(hwaddr + sizeof(unsigned));
					sprintf(net_info->hwaddr, "%x:%x:%x:%x:%x:%x", (hwaddr1 & 0xff), ((hwaddr1 >> 8) & 0xff), ((hwaddr1 >> 16) & 0xff), ((hwaddr1 >> 24) & 0xff), (hwaddr2 & 0xff), ((hwaddr2 >> 8) & 0xff));
					wifi_ap_start_hostapd();
					wifi_ap_start_dns();
					wifi_service->state = WIFI_AP_STATE_ENABLED;
					wifi_service_state_cb(STATE_AP_ENABLED);
				}
				break;
			case CMD_AP_DISABLE:
				if ((wifi_service->state > 0) && ((wifi_service->state & 0xf0) == WIFI_AP_STATE_ENABLED)) {
					wifi_set_if_down(AP);
					wifi_ap_stop_dns();
					wifi_ap_stop_hostapd();
					wifi_unload_driver(AP);
					wifi_service->state = WIFI_AP_STATE_DISABLED;
					wifi_service_state_cb(STATE_AP_DISABLED);
				}
				break;
			case CMD_MONITOR_ENABLE:
				if ((wifi_service->state > 0) && ((wifi_service->state & 0xf00) == WIFI_MONITOR_STATE_ENABLED)) {
					wifi_service_state_cb(STATE_MONITOR_ENABLED);
					break;
				}
				if (wifi_load_driver(MONITOR, hwaddr) < 0) {
					ERROR("wifi load driver failed %s \n", wifi_mode[MONITOR]);
					wifi_service->state = WIFI_STATE_FAILED;
				} else {
					hwaddr1 = (hwaddr[0] | (hwaddr[1] << 8) | (hwaddr[2] << 16) | (hwaddr[3] << 24));
					hwaddr2 = *(unsigned *)(hwaddr + sizeof(unsigned));
					sprintf(net_info->hwaddr, "%x:%x:%x:%x:%x:%x", (hwaddr1 & 0xff), ((hwaddr1 >> 8) & 0xff), ((hwaddr1 >> 16) & 0xff), ((hwaddr1 >> 24) & 0xff), (hwaddr2 & 0xff), ((hwaddr2 >> 8) & 0xff));
					wifi_service->state = WIFI_MONITOR_STATE_ENABLED;
					wifi_service_state_cb(STATE_MONITOR_ENABLED);
				}
				break;
			case CMD_MONITOR_DISABLE:
				if ((wifi_service->state > 0) && ((wifi_service->state & 0xf00) == WIFI_MONITOR_STATE_ENABLED)) {
					wifi_unload_driver(MONITOR);
					wifi_service->state = WIFI_MONITOR_STATE_DISABLED;
					wifi_service_state_cb(STATE_MONITOR_DISABLED);
				}
				break;
			case CMD_WPS_CONNECTE:
				wifi_config = (struct wifi_config_t *)cmd_arg;
				if (wifi_config) {
					if (wifi_config->psk[0] != '\0') {
						sprintf(temp_cmd, "%s%s", "WPS_PIN any ", wifi_config->psk);
						docommand(STA, temp_cmd);
					} else if (wifi_config->ssid[0] != '\0') {
						wifi_sta_load_store_config();
						wifi_config_save = wifi_sta_search_config(wifi_config, AP_SAVE);
						if (wifi_config_save == NULL) {
							wifi_config_save = wifi_sta_search_config(wifi_config, AP_SCAN);
							if (wifi_config_save == NULL) {
								if (connect_scan > 5) {
									LOGD("wifi have not scan request ap %s \n", wifi_config->ssid);
									wifi_service->cmd_lock = 0;
									connect_scan = 0;
									free(wifi_config);
									wifi_config = NULL;
									break;
								} else {
									wifi_service_add_cmd(CMD_STA_SCAN, (unsigned)wifi_config);
									wifi_service_add_cmd(CMD_WPS_CONNECTE, (unsigned)wifi_config);
									wifi_service->cmd_lock = 0;
									continue;
								}
							} else {
								memcpy(wifi_config, wifi_config_save, sizeof(struct wifi_config_t));
								wifi_config->list_type = AP_SAVE;
								list_add_tail(&wifi_config->list, &wifi_service->store_list);
								connect_scan = 0;
								LOGD("wifi add network %d %s \n", wifi_config_save->network_id, wifi_config_save->ssid);
							}
						} else {
							sprintf(wifi_config_save->psk, wifi_config->psk);
							free(wifi_config);
							wifi_config = NULL;
						}
						sprintf(temp_cmd, "%s%s", "WPS_PBC ", wifi_config_save->bssid);
						docommand(STA, temp_cmd);
					} else {
						docommand(STA, "WPS_PBC");
					}
				} else {
					docommand(STA, "WPS_PBC");
				}
				wifi_service->cmd_lock = 1;
				state = WIFI_STA_STATE_CONNECTED;
				ret = wifi_service->state;
				timeout_cnt = 3000;
				break;
			default:
				break;
		}

		if ((wifi_service->cmd_lock) && (wifi_service->state != WIFI_STATE_FAILED)) {

			pthread_mutex_lock(&wifi_service->monitor_mutex);
			if (cmd == CMD_STA_ENABLE) {
				wifi_service->monitor_cmd = STA_STATUS_CHECK;
			} else {
				wifi_service->monitor_cmd = STA_WAIT_EVENT;
			}
			wifi_service->state = WIFI_STATE_UNKNOWN;
			pthread_cond_signal(&wifi_service->monitor_cond);
			pthread_mutex_unlock(&wifi_service->monitor_mutex);
			cnt = 0;
			LOGD("wifi service cmd wait start %d %d \n", state, timeout_cnt);
			while (((wifi_service->state != state) || (wifi_service->state == WIFI_STATE_UNKNOWN)) && (cnt < timeout_cnt)) {
				if (!wifi_service->cmd_lock)
					break;
				usleep(20000);
				cnt++;
			}
			LOGD("wifi service cmd wait end %d \n", wifi_service->state);
			if (cnt >= timeout_cnt || !wifi_service->cmd_lock)
				wifi_service->state = ret;
			wifi_service->cmd_lock = 0;
		}
	}

	return NULL;
}

static struct wifi_config_t *wifi_get_scan_result(struct Wifi_Service_t *wifi_service)
{
	struct wifi_config_t *wifi_config;
	list_head_t *l;

	if (list_empty(&wifi_service->scan_list))
		return NULL;

	l = wifi_service->scan_list.next;
	wifi_config = container_of(l, struct wifi_config_t, list);
	return wifi_config;
}

static struct wifi_config_t *wifi_get_sta_config(struct Wifi_Service_t *wifi_service)
{
	struct wifi_config_t *wifi_config;
	list_head_t *l;

	if (list_empty(&wifi_service->store_list))
		return NULL;

	l = wifi_service->store_list.next;
	wifi_config = container_of(l, struct wifi_config_t, list);
	return wifi_config;
}

static struct wifi_config_t *wifi_sta_next_config(struct Wifi_Service_t *wifi_service, struct wifi_config_t *wifi_config)
{
	struct wifi_config_t *wifi_config_next = NULL;
	list_head_t *l;

	wifi_config_next = wifi_sta_search_config(wifi_config, wifi_config->list_type);
	if (wifi_config_next == NULL)
		return NULL;

	if ((wifi_config_next->list.next == &wifi_service->store_list) 
		|| (wifi_config_next->list.next == &wifi_service->scan_list))
		return NULL;

	if ((wifi_config_next->list.next)) {
		l = wifi_config_next->list.next;
		wifi_config_next = container_of(l, struct wifi_config_t, list);
	}
	return wifi_config_next;
}

static void wifi_cmd_rpc_event_handle(char *event, void *arg)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	struct wifi_config_t *wifi_config = NULL;
	struct wifi_config_t *wifi_config_next = NULL;

	if (!wifi_service->init) {
		LOGD("wifi service not start %s \n", event);
		return;
	}

	if (!strncmp(event, WIFI_GET_STATE, strlen(WIFI_GET_STATE))) {
		*(int *)arg = wifi_service->state;
	} else if (!strncmp(event, WIFI_GET_NET_INFO, strlen(WIFI_GET_NET_INFO))) {
		memcpy(arg, &wifi_service->net_info, sizeof(struct net_info_t));
	} else if (!strncmp(event, WIFI_STA_GET_CONFIG, strlen(WIFI_STA_GET_CONFIG))) {
		wifi_config = (struct wifi_config_t *)arg;
		wifi_config_next = wifi_get_sta_config(wifi_service);
		if (wifi_config_next)
			memcpy(wifi_config, wifi_config_next, sizeof(struct wifi_config_t));
		else
			wifi_config->status = -1;
	} else if (!strncmp(event, WIFI_STA_ITERATE_CONFIG, strlen(WIFI_STA_ITERATE_CONFIG))) {
		wifi_config = (struct wifi_config_t *)arg;
		wifi_config_next = wifi_sta_next_config(wifi_service, wifi_config);
		if (wifi_config_next)
			memcpy(wifi_config, wifi_config_next, sizeof(struct wifi_config_t));
		else
			wifi_config->status = -1;
	} else if (!strncmp(event, WIFI_STA_GET_SCAN_RESL, strlen(WIFI_STA_GET_SCAN_RESL))) {
		wifi_config = (struct wifi_config_t *)arg;
		wifi_config_next = wifi_get_scan_result(wifi_service);
		if (wifi_config_next)
			memcpy(wifi_config, wifi_config_next, sizeof(struct wifi_config_t));
		else
			wifi_config->status = -1;
	} else if (!strncmp(event, WIFI_AP_GET_CONFIG, strlen(WIFI_AP_GET_CONFIG))) {
		wifi_config = (struct wifi_config_t *)arg;
		wifi_get_config(NULL, wifi_config->ssid, wifi_config->key_management, wifi_config->psk, &wifi_config->channel);
	} else if (!strncmp(event, WIFI_AP_SET_CONFIG, strlen(WIFI_AP_SET_CONFIG))) {
		wifi_config = (struct wifi_config_t *)arg;
		wifi_set_config(NULL, wifi_config->ssid, wifi_config->key_management, wifi_config->psk, wifi_config->channel);
	}

	return;
}

static void wifi_cmd_event_handle(char *event, void *arg)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	struct wifi_command_t *wifi_command = (struct wifi_command_t *)arg;
	struct wifi_config_t *wifi_config;
	list_head_t *l, *n;
	int network_id = -1;

	if (!wifi_service->init) {
		LOGD("wifi service not start %d \n", wifi_command->cmd_type);
		return;
	}

	if (!strncmp(event, WIFI_CMD_EVENT, strlen(WIFI_CMD_EVENT))) {
		switch (wifi_command->cmd_type) {
			case CMD_STA_ENABLE:
				wifi_service_add_cmd(CMD_STA_ENABLE, 0);
				break;
			case CMD_STA_DISABLE:
				wifi_service_add_cmd(CMD_STA_DISABLE, 0);
				wifi_service->cmd_lock = 0;
				break;
			case CMD_STA_SCAN:
				wifi_service_add_cmd(CMD_STA_SCAN, 0);
				break;
			case CMD_STA_CONNECT:
				wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
				if (wifi_config == NULL)
					break;
				sprintf(wifi_config->psk, wifi_command->psk);
				memcpy(wifi_config->ssid, wifi_command->ssid, sizeof(wifi_command->ssid));
				wifi_service_add_cmd(CMD_STA_CONNECT, (unsigned)wifi_config);
				wifi_service->cmd_lock = 0;
				break;
			case CMD_STA_FORGET:
				list_for_each_safe(l, n, &wifi_service->store_list) {
					wifi_config = container_of(l, struct wifi_config_t, list);
					if (!memcmp(wifi_config->ssid, wifi_command->ssid, sizeof(wifi_config->ssid))) {
						network_id = wifi_config->network_id;
						break;
					}
				}
				if (network_id >= 0) {
					wifi_service_add_cmd(CMD_STA_FORGET, network_id);
					wifi_service->cmd_lock = 0;
				}
				break;
			case CMD_STA_DISCONNECT:
				wifi_service_add_cmd(CMD_STA_DISCONNECT, 0);
				break;
			case CMD_AP_ENABLE:
				wifi_service_add_cmd(CMD_AP_ENABLE, 0);
				break;
			case CMD_AP_DISABLE:
				wifi_service_add_cmd(CMD_AP_DISABLE, 0);
				break;
			case CMD_MONITOR_ENABLE:
				wifi_service_add_cmd(CMD_MONITOR_ENABLE, 0);
				break;
			case CMD_MONITOR_DISABLE:
				wifi_service_add_cmd(CMD_MONITOR_DISABLE, 0);
				break;
			case CMD_WPS_CONNECTE:
				wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
				if (wifi_config == NULL)
					break;
				memset(wifi_config, 0, sizeof(struct wifi_config_t));
				memcpy(wifi_config->psk, wifi_command->psk, sizeof(wifi_command->psk));
				memcpy(wifi_config->ssid, wifi_command->ssid, sizeof(wifi_command->ssid));
				wifi_service_add_cmd(CMD_WPS_CONNECTE, (unsigned)wifi_config);
				wifi_service->cmd_lock = 0;
				break;
			default:
				LOGD("wifi unknown command %d \n", wifi_command->cmd_type);
				break;
		}
	}

	return;
}

static int wifi_init_service(struct Wifi_Service_t *wifi_service)
{
	if (wifi_service->init)
		return 0;

	wifi_service->state = WIFI_STATE_FAILED;
	wifi_service->init = WIFI_SERVICE_INIT;
	INIT_LIST_HEAD(&wifi_service->cmd_list);
	INIT_LIST_HEAD(&wifi_service->scan_list);
	INIT_LIST_HEAD(&wifi_service->store_list);
	pthread_mutex_init(&wifi_service->mutex, NULL);
	pthread_cond_init(&wifi_service->cond, NULL);
	if (pthread_create(&wifi_service->service_thread, NULL, wifi_service_thread, (void *)wifi_service) != 0) {
		ERROR("could not create wifi service thread: %s \n", strerror(errno));
		return -1;
	}
	pthread_mutex_init(&wifi_service->monitor_mutex, NULL);
	pthread_cond_init(&wifi_service->monitor_cond, NULL);
	wifi_service->monitor_cmd = CMD_NONE;
	if (pthread_create(&wifi_service->monitor_thread, NULL, wifi_monitor_thread, (void *)wifi_service) != 0) {
		ERROR("could not create wifi monitor thread: %s \n", strerror(errno));
		return -1;
	}
	event_register_handler(WIFI_CMD_EVENT, 0, wifi_cmd_event_handle);
	event_register_handler(WIFI_GET_STATE, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_GET_NET_INFO, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_STA_GET_CONFIG, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_STA_ITERATE_CONFIG, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_STA_GET_SCAN_RESL, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_AP_GET_CONFIG, EVENT_RPC, wifi_cmd_rpc_event_handle);
	event_register_handler(WIFI_AP_SET_CONFIG, EVENT_RPC, wifi_cmd_rpc_event_handle);
	wifi_normal_init();

	return 0;
}

static int wifi_deinit_service(struct Wifi_Service_t *wifi_service)
{
	struct wifi_config_t *wifi_config;
	struct cmd_data_t *cmd_data;
	list_head_t *l, *n;
	void *dummy;

	if (!wifi_service->init)
		return -1;

	wifi_service_add_cmd(CMD_STA_DISABLE, 0);
	wifi_service_add_cmd(CMD_AP_DISABLE, 0);
	wifi_service_add_cmd(CMD_MONITOR_DISABLE, 0);
	wifi_service_add_cmd(CMD_QUIT, 0);
	pthread_join(wifi_service->service_thread, &dummy);

	pthread_mutex_lock(&wifi_service->monitor_mutex);
	wifi_service->monitor_cmd = CMD_QUIT;
	pthread_cond_signal(&wifi_service->monitor_cond);
	pthread_mutex_unlock(&wifi_service->monitor_mutex);
	pthread_join(wifi_service->monitor_thread, &dummy);

	list_for_each_safe(l, n, &wifi_service->cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}

	list_for_each_safe(l, n, &wifi_service->store_list) {
		wifi_config = container_of(l, struct wifi_config_t, list);
		list_del(&wifi_config->list);
		free(wifi_config);
	}

	list_for_each_safe(l, n, &wifi_service->scan_list) {
		wifi_config = container_of(l, struct wifi_config_t, list);
		list_del(&wifi_config->list);
		free(wifi_config);
	}

	pthread_cond_destroy(&wifi_service->cond);
	pthread_mutex_destroy(&wifi_service->mutex);
	pthread_cond_destroy(&wifi_service->monitor_cond);
	pthread_mutex_destroy(&wifi_service->monitor_mutex);
	wifi_service->init = 0;
	wifi_normal_deinit();
	LOGD("wifi service stoped\n");

	return 0;
}

static void wifi_signal_handler(int sig)
{
	struct Wifi_Service_t *wifi_service = wifi_service_save;
	char name[32];

	if ((sig == SIGQUIT) || (sig == SIGTERM)) {
		wifi_deinit_service(wifi_service);
	} else if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		LOGD("wifi Segmentation Fault %s \n", name);
		exit(1);
	}

	return;
}

static void wifi_init_signal(void)
{
    signal(SIGTERM, wifi_signal_handler);
    signal(SIGQUIT, wifi_signal_handler);
    signal(SIGTSTP, wifi_signal_handler);
    signal(SIGSEGV, wifi_signal_handler);
}

int main(int argc, char** argv)
{
	struct Wifi_Service_t *wifi_service = NULL;

	wifi_service = malloc(sizeof(struct Wifi_Service_t));
	if (!wifi_service)
		return 0;

	memset(wifi_service, 0, sizeof(struct Wifi_Service_t));
	wifi_init_signal();
	wifi_service_save = wifi_service;
	wifi_init_service(wifi_service);
	while (1) {
		if (!wifi_service->init)
			break;
		sleep(1);
	}

	free(wifi_service);
	return 0;
}
