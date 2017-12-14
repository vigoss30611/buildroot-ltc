#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <qsdk/event.h>
#include <qsdk/sys.h>
#include "wifi.h"

#if WIFI_DEBUG
#define  LOGD(...)   printf(__VA_ARGS__)
#else
#define  LOGD(...)
#endif
#define  ERROR(...)   printf(__VA_ARGS__)

static void wifi_manager_add_cmd(int cmd, unsigned cmd_arg)
{
	struct wifi_config_t *wifi_config = NULL;
	struct wifi_command_t *wifi_command = NULL;

	wifi_command = (struct wifi_command_t *)malloc(sizeof(struct wifi_command_t));
	if (wifi_command == NULL)
		return;

	memset(wifi_command, 0, sizeof(struct wifi_command_t));
	switch (cmd) {
		case CMD_STA_ENABLE:
			wifi_command->cmd_type = CMD_STA_ENABLE;
			break;
		case CMD_STA_DISABLE:
			wifi_command->cmd_type = CMD_STA_DISABLE;
			break;
		case CMD_STA_SCAN:
			wifi_command->cmd_type = CMD_STA_SCAN;
			break;
		case CMD_STA_CONNECT:
			wifi_config = (struct wifi_config_t *)cmd_arg;
			sprintf(wifi_command->psk, wifi_config->psk);
			memcpy(wifi_command->ssid, wifi_config->ssid, sizeof(wifi_config->ssid));
			wifi_command->cmd_type = CMD_STA_CONNECT;
			break;
		case CMD_STA_FORGET:
			wifi_config = (struct wifi_config_t *)cmd_arg;
			sprintf(wifi_command->psk, wifi_config->psk);
			memcpy(wifi_command->ssid, wifi_config->ssid, sizeof(wifi_config->ssid));
			wifi_command->cmd_type = CMD_STA_FORGET;
			break;
		case CMD_STA_DISCONNECT:
			wifi_command->cmd_type = CMD_STA_DISCONNECT;
			break;
		case CMD_AP_ENABLE:
			wifi_command->cmd_type = CMD_AP_ENABLE;
			break;
		case CMD_AP_DISABLE:
			wifi_command->cmd_type = CMD_AP_DISABLE;
			break;
		case CMD_MONITOR_ENABLE:
			wifi_command->cmd_type = CMD_MONITOR_ENABLE;
			break;
		case CMD_MONITOR_DISABLE:
			wifi_command->cmd_type = CMD_MONITOR_DISABLE;
			break;
		case CMD_WPS_CONNECTE:
			wifi_config = (struct wifi_config_t *)cmd_arg;
			memcpy(wifi_command->psk, wifi_config->psk, sizeof(wifi_config->psk));
			memcpy(wifi_command->ssid, wifi_config->ssid, sizeof(wifi_config->ssid));
			wifi_command->cmd_type = CMD_WPS_CONNECTE;
			break;
		default:
			LOGD("wifi unknown command %d \n", cmd);
			free(wifi_command);
			return;
	}

	event_send(WIFI_CMD_EVENT, (char *)wifi_command, sizeof(struct wifi_command_t));
	free(wifi_command);
	return;
}

int wifi_sta_connect(struct wifi_config_t *wifi_config)
{
	wifi_manager_add_cmd(CMD_STA_CONNECT, (unsigned)wifi_config);
	return 0;
}

int wifi_sta_disconnect(struct wifi_config_t *wifi_config)
{
	wifi_manager_add_cmd(CMD_STA_DISCONNECT, 0);
	return 0;
}

int wifi_sta_forget(struct wifi_config_t *wifi_config)
{
	wifi_manager_add_cmd(CMD_STA_FORGET, (unsigned)wifi_config);
	return 0;
}

int wifi_sta_enable(void)
{
	wifi_manager_add_cmd(CMD_STA_ENABLE, 0);
	return 0;
}

int wifi_sta_disable(void)
{
	wifi_manager_add_cmd(CMD_STA_DISABLE, 0);
	return 0;
}

int wifi_sta_scan(void)
{
	wifi_manager_add_cmd(CMD_STA_SCAN, 0);
	return 0;
}

int wifi_ap_enable(void)
{
	wifi_manager_add_cmd(CMD_AP_ENABLE, 0);
	return 0;
}

int wifi_ap_disable(void)
{
	wifi_manager_add_cmd(CMD_AP_DISABLE, 0);
	return 0;
}

int wifi_monitor_enable(void)
{
	wifi_manager_add_cmd(CMD_MONITOR_ENABLE, 0);
	return 0;
}

int wifi_monitor_disable(void)
{
	wifi_manager_add_cmd(CMD_MONITOR_DISABLE, 0);
	return 0;
}

struct wifi_config_t *wifi_sta_get_scan_result(void)
{
	struct wifi_config_t *wifi_config = NULL;
	int ret;

	wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
	if (wifi_config == NULL)
		return NULL;
	memset(wifi_config, 0, sizeof(struct wifi_config_t));
	ret = event_rpc_call(WIFI_STA_GET_SCAN_RESL, (char *)wifi_config, sizeof(struct wifi_config_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_STA_GET_SCAN_RESL, ret);
		free(wifi_config);
		return NULL;
	}
	if (wifi_config->status == -1) {
		free(wifi_config);
		return NULL;
	}

	return wifi_config;
}

struct wifi_config_t *wifi_sta_get_config(void)
{
	struct wifi_config_t *wifi_config = NULL;
	int ret;

	wifi_config = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
	if (wifi_config == NULL)
		return NULL;
	memset(wifi_config, 0, sizeof(struct wifi_config_t));
	ret = event_rpc_call(WIFI_STA_GET_CONFIG, (char *)wifi_config, sizeof(struct wifi_config_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_STA_GET_CONFIG, ret);
		free(wifi_config);
		return NULL;
	}
	if (wifi_config->status == -1) {
		free(wifi_config);
		return NULL;
	}

	return wifi_config;
}

struct wifi_config_t *wifi_sta_iterate_config(struct wifi_config_t *wifi_config)
{
	struct wifi_config_t *wifi_config_next = NULL;
	int ret;

	wifi_config_next = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
	if (wifi_config_next == NULL)
		return NULL;
	memset(wifi_config_next, 0, sizeof(struct wifi_config_t));
	memcpy(wifi_config_next, wifi_config, sizeof(struct wifi_config_t));
	ret = event_rpc_call(WIFI_STA_ITERATE_CONFIG, (char *)wifi_config_next, sizeof(struct wifi_config_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_STA_ITERATE_CONFIG, ret);
		free(wifi_config_next);
		return NULL;
	}
	if (wifi_config_next->status == -1) {
		free(wifi_config_next);
		return NULL;
	}

	return wifi_config_next;
}

int wifi_ap_get_config(struct wifi_config_t *wifi_config)
{
	int ret;

	ret = event_rpc_call(WIFI_AP_GET_CONFIG, (char *)wifi_config, sizeof(struct wifi_config_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_AP_GET_CONFIG, ret);
		return ret;
	}

	return 0;
}

int wifi_ap_set_config(struct wifi_config_t *wifi_config)
{
	int ret;

	ret = event_rpc_call(WIFI_AP_SET_CONFIG, (char *)wifi_config, sizeof(struct wifi_config_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_AP_SET_CONFIG, ret);
		return ret;
	}

	return 0;
}

int wifi_get_state(void)
{
	int state = WIFI_STATE_FAILED, ret;

	ret = event_rpc_call(WIFI_GET_STATE,(char *)(&state), sizeof(int));
	if (ret < 0)
		ERROR("wifi RPC %s event failed %d\n", WIFI_GET_STATE, ret);

	return state;
}

int wifi_get_net_info(struct net_info_t *net_info)
{
	int ret;

	ret = event_rpc_call(WIFI_GET_NET_INFO, (char *)net_info, sizeof(struct net_info_t));
	if (ret < 0) {
		ERROR("wifi RPC %s event failed %d\n", WIFI_GET_NET_INFO, ret);
		return ret;
	}

	return 0;
}

int wifi_wps_connect(struct wifi_config_t *wifi_config)
{
	wifi_manager_add_cmd(CMD_WPS_CONNECTE, (unsigned)wifi_config);
	return 0;
}

int wifi_start_service(void (*state_cb)(char* event, void *arg))
{
	int pid, cnt = 0;

	do {
		pid = system_check_process("wifi");
		if (pid > 0)
			break;

		usleep(20000);
		cnt++;
	} while (cnt < 20);

	if (cnt >= 20)
		printf("have not found wifi process please run wifi \n");
	if (state_cb) {
		event_register_handler(WIFI_STATE_EVENT, 0, state_cb);
		event_register_handler(STATE_AP_STA_CONNECTED, 0, state_cb);
		event_register_handler(STATE_AP_STA_DISCONNECTED, 0, state_cb);
	}

	return 0;
}

int wifi_stop_service(void)
{
	LOGD("wifi manager stoped\n");
	return 0;
}
