#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <fr/libfr.h>
#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/wifi.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>
#include <qsdk/vplay.h>
#include <qsdk/cep.h>

#include "d304main.h"
#include "IOTCAPIs.h"
#include "AVAPIs.h"
#include "AVFRAMEINFO.h"
#include "AVIOCTRLDEFs.h"
#include "RDTAPIs.h"

#define TUTK_DEBUG		1
#define IOTC_LISTEN_TIMEOUT		600000
#define SERVTYPE_STREAM_SERVER 	0
#define MAX_SIZE_IOCTRL_BUF        1024
#define RECV_BUF_SIZE			256

#if TUTK_DEBUG
#define  LOGD(...)   printf(__VA_ARGS__)
#else
#define  LOGD(...)
#endif
#define  ERROR(...)   printf(__VA_ARGS__)

#define MAX_ERROR_CNT 20

static struct tutk_common_t *tutk_save = NULL;

static struct v_bitrate_info record_scene_info = {
	.bitrate = 280000,
	.qp_min = 20,
	.qp_max = 40,
	.qp_hdr = -1,
	.qp_delta = -2,
	.gop_length = 15,
	.picture_skip = 0,
	.p_frame = 15,
};

static struct v_bitrate_info gernel_scene_info[2][2][3] = {
	[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 400000,
		.qp_min = 20,
		.qp_max = 45,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 160000,
		.qp_min = 20,
		.qp_max = 42,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 100000,
		.qp_min = 20,
		.qp_max = 40,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 666000,
		.qp_min = 20,
		.qp_max = 42,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 280000,
		.qp_min = 20,
		.qp_max = 40,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 130000,
		.qp_min = 20,
		.qp_max = 38,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 400000,
		.qp_min = 20,
		.qp_max = 38,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 160000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 100000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 666000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 280000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 130000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -2,
		.gop_length = 15,
		.picture_skip = 0,
		.p_frame = 15,
	}
};

static const char *device_name[] = {
	"deviceone",
    "devicetwo",
    "devicethree",
    "devicefour",
    "devicefive",
    "devicesix",
    "deviceseven",
    "deviceeight",
    "devicenine",
    "deviceten",
    "devicenone",
    "devicenone",
    "devicenone"
};

#if TUTK_DEBUG
static const char *tutk_session_cmd[] = {
    "tutk_quit",
    "enable_server",
    "disable_server",
    "server_start_session",
    "server_stop_session",
    "server_start_video",
    "server_stop_video",
    "server_set_video_resolution",
    "server_start_audio",
    "server_stop_audio",
    "server_restart_audio",
    "client_start_audio",
    "client_stop_audio",
    "client_restart_audio",
    "connect_ap",
    "start_wifi_scan",
    "forget_ap",
    "start_reg_dev",
    "server_start_playback",
    "server_pause_playback",
    "server_stop_playback",
    "server_start_record",
    "server_stop_record",
    "server_restart_record",
    "server_record_check ",
    "server_report_event ",
    "server_change_sence ",
    "event_stop_report ",
    "tutk_none"
};
#endif

static int list_get_length(struct list_head_t *head)
{
	struct list_head_t *l, *n;
	int len = 0;

	list_for_each_safe(l, n, head)
		len++;

	return len;
}

static void print_err_handling(int err_num)
{
	switch (err_num) {

		case IOTC_ER_SERVER_NOT_RESPONSE :
			ERROR("Master doesn't respond : %d\n", IOTC_ER_SERVER_NOT_RESPONSE);
        	break;
		case IOTC_ER_FAIL_RESOLVE_HOSTNAME :
			ERROR("Can't resolve hostname : %d\n", IOTC_ER_FAIL_RESOLVE_HOSTNAME);
        	break;
		case IOTC_ER_ALREADY_INITIALIZED :
			ERROR("Already initialized : %d\n", IOTC_ER_ALREADY_INITIALIZED);
        	break;
		case IOTC_ER_FAIL_CREATE_MUTEX :
			ERROR("Can't create mutex : %d\n", IOTC_ER_FAIL_CREATE_MUTEX);
			break;
		case IOTC_ER_FAIL_CREATE_THREAD :
			ERROR("Can't create thread : %d\n", IOTC_ER_FAIL_CREATE_THREAD);
        	break;
		case IOTC_ER_UNLICENSE :
			ERROR("this UID is unlicense : %d\n", IOTC_ER_UNLICENSE);
			break;
		case IOTC_ER_NOT_INITIALIZED :
			ERROR("Please initialize the IOTCAPI first : %d\n", IOTC_ER_NOT_INITIALIZED);
			break;
		case IOTC_ER_TIMEOUT :
			ERROR("IOTC timout : %d\n", IOTC_ER_TIMEOUT);
			break;
		case IOTC_ER_INVALID_SID :
			ERROR("This SID is invalid : %d\n", IOTC_ER_INVALID_SID);
			break;
		case IOTC_ER_EXCEED_MAX_SESSION :
			ERROR("The amount of session reach to the maximum : %d\n", IOTC_ER_EXCEED_MAX_SESSION);
			break;
		case IOTC_ER_CAN_NOT_FIND_DEVICE :
			ERROR("Device didn't register on server : %d\n", IOTC_ER_CAN_NOT_FIND_DEVICE);
			break;
		case IOTC_ER_SESSION_CLOSE_BY_REMOTE :
			ERROR("Session is closed by remote so we can't access : %d\n", IOTC_ER_SESSION_CLOSE_BY_REMOTE);
			break;
		case IOTC_ER_REMOTE_TIMEOUT_DISCONNECT :
			ERROR("We can't receive an acknowledgement character within a TIMEOUT : %d\n", IOTC_ER_REMOTE_TIMEOUT_DISCONNECT);
			break;
		case IOTC_ER_DEVICE_NOT_LISTENING :
			ERROR("Device doesn't listen or the sessions of device reach to maximum : %d\n", IOTC_ER_DEVICE_NOT_LISTENING);
			break;
		case IOTC_ER_CH_NOT_ON :
			ERROR("Channel isn't on : %d\n", IOTC_ER_CH_NOT_ON);
			break;
		case IOTC_ER_SESSION_NO_FREE_CHANNEL :
			ERROR("All channels are occupied : %d\n", IOTC_ER_SESSION_NO_FREE_CHANNEL);
			break;
		case IOTC_ER_TCP_TRAVEL_FAILED :
			ERROR("Device can't connect to Master : %d\n", IOTC_ER_TCP_TRAVEL_FAILED);
			break;
		case IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED :
			ERROR("Device can't connect to server by TCP : %d\n", IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED);
			break;
		case IOTC_ER_NO_PERMISSION :
			ERROR("This UID's license doesn't support TCP : %d\n", IOTC_ER_NO_PERMISSION);
			break;
		case IOTC_ER_NETWORK_UNREACHABLE :
			ERROR("Network is unreachable : %d\n", IOTC_ER_NETWORK_UNREACHABLE);
			break;
		case IOTC_ER_FAIL_SETUP_RELAY :
			ERROR("Client can't connect to a device via Lan : %d\n", IOTC_ER_FAIL_SETUP_RELAY);
			break;
		case IOTC_ER_NOT_SUPPORT_RELAY :
			ERROR("Server doesn't support UDP relay mode : %d\n", IOTC_ER_NOT_SUPPORT_RELAY);
			break;
		default :
			break;
    }
}

static void tutk_server_login_callback(unsigned long login_info)
{
	LOGD("login_callback %lx \n", login_info);
    if((login_info & 0x04))
        LOGD("I can be connected via Internet\n");
    else if((login_info & 0x08))
        LOGD("I am be banned by IOTC Server because UID multi-login\n");
}

static void *tutk_server_login_thread(void *arg)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)arg;
    int ret = IOTC_ER_NoERROR, cnt = 0;

	IOTC_Get_Login_Info_ByCallBackFn(tutk_server_login_callback);
	tutk->server_start |= (1 << 1);
	while (tutk->process_run) {
		if (tutk->reg_state)
			ret = IOTC_Device_Login((char *)tutk->reg_uid, NULL, NULL);
		else
			ret = IOTC_Device_Login((char *)tutk->local_uid, NULL, NULL);
		if (ret == IOTC_ER_NoERROR)
			break;
		print_err_handling(ret);
		sleep(1);
		cnt++;
		if (cnt >= 5)
			break;
    }

	/*cnt = 0;
	while (tutk->process_run) {
		ret = IOTC_Device_Login((char *)tutk->local_uid, NULL, NULL);
		if (ret == IOTC_ER_NoERROR)
			break;
		print_err_handling(ret);
		sleep(1);
		cnt++;
		if (cnt >= 5)
			break;
	}*/

	tutk->server_login_thread = 0;
	pthread_exit(0);
}

static int tutk_server_get_config(struct tutk_session_t *tutk_session, char *name, char *value)
{
	FILE *fp;
	char linebuf[512];
	char save_name[128];
	int offset = 0;

	fp = fopen("/etc/tutk_config", "r+");
	if (fp == NULL) {
		ERROR("open error tutk config %s \n", strerror(errno));
		return -1;
	}

	value[0] = '\0';
	sprintf(save_name, "tutk_");
	strcat(save_name, name);
	strcat(save_name, "=");
	while (fgets(linebuf, 512, fp)) {
		if (!strncmp(linebuf, save_name, strlen(save_name))) {
			snprintf(value, (strlen(linebuf) - strlen(save_name)), linebuf + strlen(save_name));
			LOGD("tutk get config %s%s\n", save_name, value);
			break;
		}

		offset += strlen(linebuf);
	}

	fclose(fp);
	return 0;
}

static int tutk_server_save_config(struct tutk_session_t *tutk_session, char *name, char *value)
{
	FILE *fp;
	char linebuf[512];
	char valuebuf[512];
	char save_name[128];
	int offset = 0, ret = -1;

	fp = fopen("/etc/tutk_config", "r+");
	if (fp == NULL) {
		ERROR("open error tutk config %s \n", strerror(errno));
		return -1;
	}

	sprintf(save_name, "tutk_");
	strcat(save_name, name);
	strcat(save_name, "=");
	while (fgets(linebuf, 512, fp)) {
		if (!strncmp(linebuf, save_name, strlen(save_name)))
			break;

		offset += strlen(linebuf);
	}
	ret = fseek(fp, offset, SEEK_SET);
	if (ret < 0) {
		ERROR("fseek error tutk config %s \n", strerror(errno));
		fclose(fp);
		return ret;
	}
	sprintf(valuebuf, "%s%s\n", save_name, value);
	fprintf(fp, "%s", valuebuf);
	LOGD("tutk save config %d  %s \n", offset, valuebuf);

	fclose(fp);
	return ret;
}

static void tutk_event_time_convert(struct tutk_session_t *tutk_session, struct tm *to_tm, STimeDay *stime_day)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	char value[128];

	tutk->get_config(tutk_session, TUTK_GMT, value);
	to_tm->tm_year = stime_day->year - 1900;
	to_tm->tm_mon = stime_day->month;
	to_tm->tm_mday = stime_day->day;
	to_tm->tm_hour = stime_day->hour;
	to_tm->tm_min = stime_day->minute;
	to_tm->tm_sec = stime_day->second;
	to_tm->tm_wday = stime_day->wday - 1;
	return;
}

static void *tutk_get_event_file(struct tutk_session_t *tutk_session, time_t start, time_t end)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct record_file_t *record_file;
	struct list_head_t *l, *n;
	SMsgAVIoctrlListEventResp *event_resp;
	char *temp = NULL, event_type[16], value[128];
	time_t current;
	struct tm tm_current;
	int ret, duration;

	tutk->get_config(tutk_session, TUTK_GMT, value);
	event_resp = (SMsgAVIoctrlListEventResp *)malloc(sizeof(SMsgAVIoctrlListEventResp) + sizeof(SAvEvent) * MAX_SEND_ALARMS);
	if (event_resp == NULL)
		return NULL;

	memset(event_resp, 0, sizeof(SMsgAVIoctrlListEventResp) + sizeof(SAvEvent) * MAX_SEND_ALARMS);
	event_resp->channel = tutk_session->tutk_server_channel_id;

	pthread_mutex_lock(&tutk->rec_file_mutex);
    list_for_each_safe(l, n, &tutk->record_list) {

		record_file = container_of(l, struct record_file_t, list);
		memset(event_type, 0, 32);
		duration = 0;
		memset(&tm_current, 0, sizeof(struct tm));
		ret = sscanf(record_file->file_name, "%d_%d_%d_%d_%d_%d_%s",
				&tm_current.tm_year, &tm_current.tm_mon, &tm_current.tm_mday,
				&tm_current.tm_hour, &tm_current.tm_min, &tm_current.tm_sec, event_type);

		temp = strstr(event_type, "_");
		if (temp != NULL) {
			duration = atoi(temp + 1);
			temp[0] = '\0';
		}
		if ((ret < 0) || (duration == 0)) {
			ERROR("not valid record file %s \n", record_file->file_name);
			continue;
		}

		if (strcmp(event_type, FILE_SAVE_ALARM) && strcmp(event_type, FILE_SAVE_FULLTIME))
			continue;
		tm_current.tm_year -= 1900;
		current = mktime(&tm_current);
		if ((current >= (start + 60 * atoi(value))) && (current <= (end + 60 * atoi(value)))) {
			event_resp->stEvent[event_resp->total].stTime.year = tm_current.tm_year + 1900;
			event_resp->stEvent[event_resp->total].stTime.month = tm_current.tm_mon;
			event_resp->stEvent[event_resp->total].stTime.day = tm_current.tm_mday;
			event_resp->stEvent[event_resp->total].stTime.wday = tm_current.tm_wday;
			event_resp->stEvent[event_resp->total].stTime.hour = tm_current.tm_hour - atoi(value) / 60;
			event_resp->stEvent[event_resp->total].stTime.minute = tm_current.tm_min;
			event_resp->stEvent[event_resp->total].stTime.second = tm_current.tm_sec;
			if (!strcmp(event_type, FILE_SAVE_FULLTIME))
				event_resp->stEvent[event_resp->total].event  = AVIOCTRL_EVENT_ALL;
			else
				event_resp->stEvent[event_resp->total].event  = AVIOCTRL_EVENT_MOTIONDECT;
			event_resp->stEvent[event_resp->total].status = EVENT_UNREADED;
			event_resp->stEvent[event_resp->total].reserved[0] = event_resp->total;
			event_resp->total++;
			event_resp->count++;
			LOGD("find event %s %s %d \n", record_file->file_name, event_type, event_resp->total);
		}
    }
    event_resp->index = 0;
    event_resp->endflag = 1;
    pthread_mutex_unlock(&tutk->rec_file_mutex);

	return event_resp;
}

static void tutk_server_ioctrl_infotm_cmd(struct tutk_session_t *tutk_session, unsigned int ioctrl_type, char *buf)
{
	//struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;

	LOGD("tutk_server_ioctrl_infotm_cmd %x  \n", ioctrl_type);
	switch (ioctrl_type) {
		case IOTYPE_INFOTM_SETCLOUD_REQ:
			break;
		default:
			break;
	}

	return;
}

static void tutk_server_ioctrl_net_cmd(struct tutk_session_t *tutk_session, unsigned int ioctrl_type, char *buf)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct wifi_config_t *wifi_config = NULL, *wifi_config_prev = NULL;
	SMsgAVIoctrlListWifiApResp *wifi_ap_resp;
	SMsgAVIoctrlSetWifiReq *wifi_req;
	SMsgAVIoctrlSetWifiResp *set_wifi_resp;
	struct timeval now_time;
	struct timespec timeout_time;
	char response[512] = {0};

	LOGD("tutk_server_ioctrl_net_cmd %s %x  \n", tutk_session->dev_name, ioctrl_type);
	switch (ioctrl_type) {
		case IOTYPE_USER_IPCAM_LISTWIFIAP_REQ:
			if (!tutk->wifi_sta_set)
				break;
			wifi_sta_scan();
			wifi_ap_resp = (SMsgAVIoctrlListWifiApResp *)malloc(sizeof(SMsgAVIoctrlListWifiApResp) + sizeof(SWifiAp)*64);
			if (!wifi_ap_resp)
				return;
			memset(wifi_ap_resp, 0, sizeof(SMsgAVIoctrlListWifiApResp) + sizeof(SWifiAp)*64);
			tutk->net_cmd_flag = WIFI_STA_STATE_SCANED;
			gettimeofday(&now_time, NULL);
			timeout_time.tv_sec = now_time.tv_sec + 5;
			timeout_time.tv_nsec = now_time.tv_usec * 1000;
			sem_timedwait(&tutk->wifi_sem, &timeout_time);

			wifi_ap_resp->number = 0;
			wifi_config = wifi_sta_get_scan_result();
			while ((wifi_config != NULL) && (wifi_ap_resp->number < 26)) {
			    strcpy(wifi_ap_resp->stWifiAp[wifi_ap_resp->number].ssid, wifi_config->ssid);
			    wifi_ap_resp->stWifiAp[wifi_ap_resp->number].mode = AVIOTC_WIFIAPMODE_MANAGED;
			    if (!memcmp(wifi_config->key_management, "WPA2-PSK-TKIP", strlen("WPA2-PSK-TKIP")))
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].enctype = AVIOTC_WIFIAPENC_WPA2_PSK_TKIP;
			    else if (!memcmp(wifi_config->key_management, "WPA-PSK-TKIP", strlen("WPA-PSK-TKIP")))
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].enctype = AVIOTC_WIFIAPENC_WPA_PSK_TKIP;
			    else if (!memcmp(wifi_config->key_management, "WPA2-PSK-CCMP", strlen("WPA2-PSK-CCMP")))
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].enctype = AVIOTC_WIFIAPENC_WPA2_PSK_AES;
			    else if (!memcmp(wifi_config->key_management, "WPA-PSK-CCMP", strlen("WPA-PSK-CCMP")))
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].enctype = AVIOTC_WIFIAPENC_WPA_PSK_AES;
			    else
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].enctype = AVIOTC_WIFIAPENC_WEP;
			    wifi_ap_resp->stWifiAp[wifi_ap_resp->number].signal = wifi_config->level;
			    if (wifi_config->status == CURRENT) {
			    	wifi_ap_resp->stWifiAp[wifi_ap_resp->number].status = 1;
			    	LOGD("tutk current connect wifi %s \n", wifi_ap_resp->stWifiAp[wifi_ap_resp->number].ssid);
			    }
			    wifi_ap_resp->number++;
			    wifi_config_prev = wifi_config;
			    wifi_config = wifi_sta_iterate_config(wifi_config_prev);
			    free(wifi_config_prev);
			}
			LOGD("tutk get wifi %d %s %d \n", wifi_ap_resp->number, wifi_ap_resp->stWifiAp[0].ssid, wifi_ap_resp->stWifiAp[0].status);
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_LISTWIFIAP_RESP, (char *)wifi_ap_resp, sizeof(SMsgAVIoctrlListWifiApResp) + sizeof(SWifiAp)*wifi_ap_resp->number);
			free(wifi_ap_resp);
			break;
		case IOTYPE_USER_IPCAM_SETWIFI_REQ:
			if (!tutk->wifi_sta_set)
				break;
			wifi_req = (SMsgAVIoctrlSetWifiReq *)buf;
			wifi_config = wifi_sta_get_scan_result();
			while (wifi_config != NULL) {
				if (!strncmp(wifi_config->ssid, (const char *)wifi_req->ssid, strlen(wifi_config->ssid))) {
					strncpy(wifi_config->psk, (const char *)wifi_req->password, sizeof(wifi_config->psk));
					break;
				}
				wifi_config_prev = wifi_config;
				wifi_config = wifi_sta_iterate_config(wifi_config_prev);
				free(wifi_config_prev);
			}
			wifi_sta_connect(wifi_config);
			free(wifi_config);
			tutk->net_cmd_flag = WIFI_STA_STATE_CONNECTED;
			gettimeofday(&now_time, NULL);
			timeout_time.tv_sec = now_time.tv_sec + 20;
			timeout_time.tv_nsec = now_time.tv_usec * 1000;
			sem_timedwait(&tutk->wifi_sem, &timeout_time);

			set_wifi_resp = (SMsgAVIoctrlSetWifiResp *)response;
			if (wifi_get_state() == WIFI_STA_STATE_CONNECTED)
				set_wifi_resp->result = 0;
			else
				set_wifi_resp->result = 1;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETWIFI_RESP, (char *)set_wifi_resp, sizeof(SMsgAVIoctrlSetWifiResp));
			break;
		default:
			break;
	}
	return;
}

static void tutk_server_ioctrl_normal_cmd(struct tutk_session_t *tutk_session, unsigned int ioctrl_type, char *buf)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	char response[512] = {0};
	char value[256] = {0};
	storage_info_t si;

	SMsgAVIoctrlDeviceInfoResp *dev_info_resp = (SMsgAVIoctrlDeviceInfoResp *)response;
	SMsgAVIoctrlGetAudioOutFormatResp *audio_format_resp = (SMsgAVIoctrlGetAudioOutFormatResp *)response;
	SMsgAVIoctrlTimeZone *time_zone_resp = (SMsgAVIoctrlTimeZone *)response;
	SMsgAVIoctrlSetPasswdReq *set_pw_req = (SMsgAVIoctrlSetPasswdReq *)buf;
	SMsgAVIoctrlSetPasswdResp *set_pw_resp = (SMsgAVIoctrlSetPasswdResp *)response;
	SMsgAVIoctrlSetStreamCtrlResp *stream_resp = (SMsgAVIoctrlSetStreamCtrlResp *)response;
	//SMsgAVIoctrlGetAudioOutFormatResp *audio_format_res = (SMsgAVIoctrlGetAudioOutFormatResp *)response;
	SMsgAVIoctrlGetStreamCtrlResq *stream_ctrl_resp = (SMsgAVIoctrlGetStreamCtrlResq *)response;
	SMsgAVIoctrlSetMotionDetectResp *motion_dect_resp = (SMsgAVIoctrlSetMotionDetectResp *)response;
	SMsgAVIoctrlGetMotionDetectResp *momtion_get_resp = (SMsgAVIoctrlGetMotionDetectResp *)response;
	SMsgAVIoctrlGetRecordResq *get_rec_resp = (SMsgAVIoctrlGetRecordResq *)response;
	SMsgAVIoctrlSetRecordResp *set_rec_resp = (SMsgAVIoctrlSetRecordResp *)response;
	SMsgAVIoctrlFormatExtStorageResp *format_storage_resp = (SMsgAVIoctrlFormatExtStorageResp *)response;
	SMsgAVIoctrlPlayRecordResp *play_rec_resp = (SMsgAVIoctrlPlayRecordResp *)response;
	SMsgAVIoctrlSetEnvironmentReq *env_req = (SMsgAVIoctrlSetEnvironmentReq *)buf;
	SMsgAVIoctrlSetEnvironmentResp *env_resp = (SMsgAVIoctrlSetEnvironmentResp *)response;
	SMsgAVIoctrlSetVideoModeReq *vo_mode_req = (SMsgAVIoctrlSetVideoModeReq *)buf;
	SMsgAVIoctrlSetVideoModeResp *vo_mode_resp = (SMsgAVIoctrlSetVideoModeResp *)response;
	SMsgAVIoctrlListEventResp *event_resp;
	SMsgAVIoctrlSetRecordReq *rec_req;
	SMsgAVIoctrlSetStreamCtrlReq *stream_ctrl_req;
	SMsgAVIoctrlSetMotionDetectReq *motion_dect_req;
	SMsgAVIoctrlListEventReq *list_event_req;
	SMsgAVIoctrlFormatExtStorageReq *format_storage_req;
	SMsgAVIoctrlPlayRecord *play_record;
	SMsgAVIoctrlAVStream *av_stream;
	struct tm event_from, event_to;
	struct timezone tz;
	struct timeval tv;
	time_t event_start, event_end;
	int ret = -1;
	enum cam_scenario scen = 0;

	LOGD("tutk_ioctrl_normal_cmd %s %x \n", tutk_session->dev_name, ioctrl_type);
	switch (ioctrl_type) {
		case IOTYPE_USER_IPCAM_DEVINFO_REQ:
			system_get_storage_info(tutk->sd_mount_point, &si);
			snprintf((char *)dev_info_resp->vendor, sizeof(dev_info_resp->vendor), "%s", TUTK_IPC_VENDOR);
			snprintf((char *)dev_info_resp->model, sizeof(dev_info_resp->model), "%s", TUTK_IPC_MODEL);
			dev_info_resp->version = TUTK_IPC_FW_VERSION;
			dev_info_resp->total = si.total;
			dev_info_resp->total >>= 10;
			dev_info_resp->free = si.free;
			dev_info_resp->free >>= 10;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_DEVINFO_RESP, (char *)dev_info_resp, sizeof(SMsgAVIoctrlDeviceInfoResp));
			break;
		case IOTYPE_USER_IPCAM_GETSUPPORTSTREAM_REQ:
			break;
		case IOTYPE_USER_CHECK_SD:
			system_get_storage_info(tutk->sd_mount_point, &si);
			if (si.total > 0)
				response[0] = 0;
			else
				response[0] = 1;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_CHECK_SD_RESP, response, sizeof(char));
			break;
		case IOTYPE_USER_IPCAM_GETAUDIOOUTFORMAT_REQ:
			audio_format_resp->channel = tutk_session->tutk_server_channel_id;
			audio_format_resp->format = AUDIO_FORMAT;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETAUDIOOUTFORMAT_RESP, (char *)audio_format_resp, sizeof(SMsgAVIoctrlGetAudioOutFormatResp));
			break;
		case IOTYPE_USER_IPCAM_GET_TIMEZONE_REQ:
			time_zone_resp->cbSize = sizeof(SMsgAVIoctrlTimeZone);
			time_zone_resp->nIsSupportTimeZone = 1;
			tutk->get_config(tutk_session, TUTK_GMT, value);
			time_zone_resp->nGMTDiff = atoi(value);
			tutk->get_config(tutk_session, TUTK_TIMEZONE, value);
            snprintf(time_zone_resp->szTimeZoneString, sizeof(time_zone_resp->szTimeZoneString), "%s", value);
            avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GET_TIMEZONE_RESP, (char *)time_zone_resp, sizeof(SMsgAVIoctrlTimeZone));
			break;
		case IOTYPE_USER_IPCAM_SET_TIMEZONE_REQ:
			time_zone_resp = (SMsgAVIoctrlTimeZone *)buf;
			tutk->get_config(tutk_session, TUTK_GMT, value);
			ret = atoi(value);
			sprintf(response, "%d", time_zone_resp->nGMTDiff);
			tutk->save_config(tutk_session, TUTK_GMT, response);
			if (time_zone_resp->szTimeZoneString)
				tutk->save_config(tutk_session, TUTK_TIMEZONE, time_zone_resp->szTimeZoneString);
			gettimeofday(&tv, &tz);
			tv.tv_sec += 60 * (time_zone_resp->nGMTDiff - ret);
			settimeofday(&tv, &tz);
			memcpy(response, time_zone_resp, sizeof(SMsgAVIoctrlTimeZone));
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SET_TIMEZONE_RESP, (char *)response, sizeof(SMsgAVIoctrlTimeZone));
			break;
		case IOTYPE_USER_IPCAM_SETPASSWORD_REQ:
			if (!strcmp(tutk->local_pwd, set_pw_req->oldpasswd)) {
				strcpy(tutk->local_pwd, set_pw_req->newpasswd);
				ret = tutk->save_config(tutk_session, LOCAL_PWD_SETTING, tutk->local_pwd);
				if (ret < 0)
					set_pw_resp->result = 0;
				else
					set_pw_resp->result = 1;
			} else {
				set_pw_resp->result = 1;
			}
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETPASSWORD_RESP, (char *)set_pw_resp, sizeof(SMsgAVIoctrlSetPasswdResp));
			break;
		case IOTYPE_USER_IPCAM_GETSTREAMCTRL_REQ:
			stream_ctrl_resp->quality = tutk_session->video_quality;
			stream_ctrl_resp->channel = tutk_session->tutk_server_channel_id;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETSTREAMCTRL_RESP, (char *)stream_ctrl_resp, sizeof(SMsgAVIoctrlGetStreamCtrlResq));
			break;
		case IOTYPE_USER_IPCAM_SETSTREAMCTRL_REQ:
			stream_ctrl_req = (SMsgAVIoctrlSetStreamCtrlReq *)buf;
			stream_resp->result = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETSTREAMCTRL_RESP, (char *)stream_resp, sizeof(SMsgAVIoctrlSetStreamCtrlResp));

			tutk_session->video_quality = stream_ctrl_req->quality;
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_SET_VIDEO_RESOLUTION, (unsigned)tutk_session);
			break;
		case IOTYPE_USER_IPCAM_GETMOTIONDETECT_REQ:
			tutk->get_config(tutk_session, TUTK_MOTION_DECT, value);
			momtion_get_resp->sensitivity = atoi(value);
			momtion_get_resp->channel = tutk_session->tutk_server_channel_id;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETMOTIONDETECT_RESP, (char *)momtion_get_resp, sizeof(SMsgAVIoctrlGetMotionDetectResp));
			break;
		case IOTYPE_USER_IPCAM_SETMOTIONDETECT_REQ:
			motion_dect_req = (SMsgAVIoctrlSetMotionDetectReq *)buf;
			sprintf(value, "%d", motion_dect_req->sensitivity);
			if (motion_dect_req->sensitivity > 0) {
				tutk->save_config(tutk_session, TUTK_RECORD_TYPE, FILE_SAVE_ALARM);
				sprintf(tutk->rec_type, "%s", FILE_SAVE_ALARM);
			} else {
				tutk->save_config(tutk_session, TUTK_RECORD_TYPE, FILE_SAVE_OFF);
				sprintf(tutk->rec_type, "%s", FILE_SAVE_OFF);
			}
			ret = tutk->save_config(tutk_session, TUTK_MOTION_DECT, value);
			if (ret < 0)
				motion_dect_resp->result = 1;
			else
				motion_dect_resp->result = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETMOTIONDETECT_RESP, (char *)motion_dect_resp, sizeof(SMsgAVIoctrlSetMotionDetectResp));
			break;
		case IOTYPE_USER_SET_WARNING_SOUND_REQ:
			sprintf(value, "%d", buf[0]);
			tutk->save_config(tutk_session, WARNING_SOUND, value);
			tutk->warning_sound = buf[0];
			response[0] = buf[0];
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_SET_WARNING_SOUND_RESP, response, sizeof(char));
			break;
		case IOTYPE_USER_IPCAM_GETRECORD_REQ:
			tutk->get_config(tutk_session, TUTK_RECORD_TYPE, value);
			if (!strcmp(value, FILE_SAVE_FULLTIME))
				get_rec_resp->recordType = AVIOTC_RECORDTYPE_FULLTIME;
			else if (!strcmp(value, FILE_SAVE_OFF))
				get_rec_resp->recordType = AVIOTC_RECORDTYPE_OFF;
			else if (!strcmp(value, FILE_SAVE_ALARM))
				get_rec_resp->recordType = AVIOTC_RECORDTYPE_ALARM;
			else if (!strcmp(value, FILE_SAVE_ALARMSNAP))
				get_rec_resp->recordType = AVIOTC_RECORDTYPE_ALARMSNAP;
			else
				get_rec_resp->recordType = AVIOTC_RECORDTYPE_OFF;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETRECORD_RESP, (char *)get_rec_resp, sizeof(SMsgAVIoctrlGetRecordResq));
			break;
		case IOTYPE_USER_IPCAM_SETRECORD_REQ:
			rec_req = (SMsgAVIoctrlSetRecordReq *)buf;
			if (rec_req->recordType == AVIOTC_RECORDTYPE_FULLTIME)
				sprintf(value, FILE_SAVE_FULLTIME);
			else if (rec_req->recordType == AVIOTC_RECORDTYPE_OFF)
				sprintf(value, FILE_SAVE_OFF);
			else if (rec_req->recordType == AVIOTC_RECORDTYPE_ALARM)
				sprintf(value, FILE_SAVE_ALARM);
			else if (rec_req->recordType == AVIOTC_RECORDTYPE_ALARMSNAP)
				sprintf(value, FILE_SAVE_ALARMSNAP);
			else
				sprintf(value, FILE_SAVE_OFF);
			tutk->save_config(tutk_session, TUTK_RECORD_TYPE, value);
			set_rec_resp->result = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETRECORD_RESP, (char *)set_rec_resp, sizeof(SMsgAVIoctrlSetRecordResp));

			if (strcmp(tutk->rec_type, value)) {
				sprintf(tutk->rec_type, "%s", value);
				if (rec_req->recordType == AVIOTC_RECORDTYPE_FULLTIME) {
					tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_RESTART_RECORD, 0);
					tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_START_RECORD, 0);
				} else {
					tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_STOP_RECORD, 0);
					tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_RESTART_RECORD, 0);
				}
			}
			break;
		case IOTYPE_USER_IPCAM_START:
		case IOTYPE_USER_IPCAM_STOP:
		case IOTYPE_USER_IPCAM_AUDIOSTART:
		case IOTYPE_USER_IPCAM_AUDIOSTOP:
		case IOTYPE_USER_IPCAM_SPEAKERSTART:
		case IOTYPE_USER_IPCAM_SPEAKERSTOP:
			if (ioctrl_type == IOTYPE_USER_IPCAM_START) {
				ret = CMD_SERVER_START_VIDEO;
			} else if (ioctrl_type == IOTYPE_USER_IPCAM_STOP) {
				ret = CMD_SERVER_STOP_VIDEO;
			} else if (ioctrl_type == IOTYPE_USER_IPCAM_AUDIOSTART) {
				ret = CMD_SERVER_START_AUDIO;
			} else if (ioctrl_type == IOTYPE_USER_IPCAM_AUDIOSTOP) {
				ret = CMD_SERVER_STOP_AUDIO;
			} else if (ioctrl_type == IOTYPE_USER_IPCAM_SPEAKERSTART) {
				av_stream = (SMsgAVIoctrlAVStream *)buf;
				tutk_session->tutk_client_channel_id = av_stream->channel;
				ret = CMD_CLIENT_START_AUDIO;
			} else if (ioctrl_type == IOTYPE_USER_IPCAM_SPEAKERSTOP) {
				ret = CMD_CLIENT_STOP_AUDIO;
			}
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, ret, (unsigned)tutk_session);
			break;
		case IOTYPE_USER_IPCAM_LISTEVENT_REQ:
			list_event_req = (SMsgAVIoctrlListEventReq *)buf;
			LOGD("%d %d %d-%d-%d,%d,%d:%d:%d\n",
                      list_event_req->channel,list_event_req->event,list_event_req->stStartTime.year,list_event_req->stStartTime.month,
                      list_event_req->stStartTime.day,list_event_req->stStartTime.wday,
                      list_event_req->stStartTime.hour,list_event_req->stStartTime.minute,
                      list_event_req->stStartTime.second);
			LOGD("%d-%d-%d,%d,%d:%d:%d, %d, %d\n",
                      list_event_req->stEndTime.year,list_event_req->stEndTime.month,
                      list_event_req->stEndTime.day,list_event_req->stEndTime.wday,
                      list_event_req->stEndTime.hour,list_event_req->stEndTime.minute,
                      list_event_req->stEndTime.second, list_event_req->event, list_event_req->status);
			tutk_event_time_convert(tutk_session, &event_from, &list_event_req->stStartTime);
			tutk_event_time_convert(tutk_session, &event_to, &list_event_req->stEndTime);
			event_start = mktime(&event_from);
			event_end = mktime(&event_to);
			event_resp = (SMsgAVIoctrlListEventResp *)tutk_get_event_file(tutk_session, event_start, event_end);
			if (event_resp != NULL) {
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_LISTEVENT_RESP, (char *)event_resp, sizeof(SMsgAVIoctrlListEventResp) + sizeof(SAvEvent)*event_resp->count);
				free(event_resp);
			}
			break;
		case IOTYPE_USER_IPCAM_RECORD_PLAYCONTROL:
			play_record = (SMsgAVIoctrlPlayRecord *)buf;
			LOGD("%d %d-%d-%d,%d,%d:%d:%d\n",
                      play_record->channel, play_record->stTimeDay.year,play_record->stTimeDay.month,
                      play_record->stTimeDay.day,play_record->stTimeDay.wday,
                      play_record->stTimeDay.hour,play_record->stTimeDay.minute,
                      play_record->stTimeDay.second);

			if (play_record->command == AVIOCTRL_RECORD_PLAY_START) {
				if (tutk_session->local_play) {
					tutk->get_config(tutk_session, TUTK_GMT, value);
					play_record->stTimeDay.hour += atoi(value) / 60;
				}
				snprintf(tutk_session->play_file, sizeof(tutk_session->play_file), "%04d_%02d_%02d_%02d_%02d_%d", play_record->stTimeDay.year,
							play_record->stTimeDay.month, play_record->stTimeDay.day, play_record->stTimeDay.hour,
							play_record->stTimeDay.minute, play_record->stTimeDay.second);
				ret = CMD_SERVER_START_PLAYBACK;
				play_rec_resp->command = AVIOCTRL_RECORD_PLAY_START;
				if (tutk_session->tutk_play_channel_id < 0)
					tutk_session->tutk_play_channel_id = IOTC_Session_Get_Free_Channel(tutk_session->session_id);
				play_rec_resp->result = tutk_session->tutk_play_channel_id;
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_RECORD_PLAYCONTROL_RESP, (char *)play_rec_resp, sizeof(SMsgAVIoctrlPlayRecordResp));
			} else if (play_record->command == AVIOCTRL_RECORD_PLAY_PAUSE) {
				ret = CMD_SERVER_PAUSE_PLAYBACK;
				play_rec_resp->command = AVIOCTRL_RECORD_PLAY_PAUSE;
				play_rec_resp->result = 0;
			} else if (play_record->command == AVIOCTRL_RECORD_PLAY_STOP) {
				ret = CMD_SERVER_STOP_PLAYBACK;
				play_rec_resp->command = AVIOCTRL_RECORD_PLAY_END;
				play_rec_resp->result = 0;
			}
			if (ret > 0)
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, ret, (unsigned)tutk_session);
			break;
		case IOTYPE_USER_IPCAM_FORMATEXTSTORAGE_REQ:
			format_storage_req = (SMsgAVIoctrlFormatExtStorageReq *)buf;
			if (format_storage_req->storage == 0)
				system_format_storage(tutk->sd_mount_point);
			format_storage_resp->storage = format_storage_req->storage;
			format_storage_resp->result = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_FORMATEXTSTORAGE_RESP, (char *)format_storage_resp, sizeof(SMsgAVIoctrlFormatExtStorageResp));
			break;
		case IOTYPE_USER_IPCAM_SET_ENVIRONMENT_REQ:
			camera_get_scenario(CAMERA_SENSOR_NAME, &scen);
			if (env_req->mode == AVIOCTRL_ENVIRONMENT_NIGHT) {
				if (scen == CAM_DAY) {
					camera_monochrome_enable(CAMERA_SENSOR_NAME, 1);
					camera_set_scenario(CAMERA_SENSOR_NAME, CAM_NIGHT);
				}
			} else if (env_req->mode == AVIOCTRL_ENVIRONMENT_OUTDOOR) {
				if (scen == CAM_NIGHT) {
					camera_set_scenario(CAMERA_SENSOR_NAME, CAM_DAY);
					camera_monochrome_enable(CAMERA_SENSOR_NAME, 0);
				}
			}
			env_resp->result = 0;
			env_resp->channel = tutk_session->server_channel_id;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SET_ENVIRONMENT_RESP, (char *)env_resp, sizeof(SMsgAVIoctrlSetEnvironmentResp));
			break;
		case IOTYPE_USER_IPCAM_SET_VIDEOMODE_REQ:
			if (vo_mode_req->mode == AVIOCTRL_VIDEOMODE_FLIP)
				camera_set_mirror(CAMERA_SENSOR_NAME, CAM_MIRROR_V);
			else if (vo_mode_req->mode == AVIOCTRL_VIDEOMODE_MIRROR)
				camera_set_mirror(CAMERA_SENSOR_NAME, CAM_MIRROR_H);
			else if (vo_mode_req->mode == AVIOCTRL_VIDEOMODE_FLIP_MIRROR)
				camera_set_mirror(CAMERA_SENSOR_NAME, CAM_MIRROR_HV);
			else if (vo_mode_req->mode == AVIOCTRL_VIDEOMODE_NORMAL)
				camera_set_mirror(CAMERA_SENSOR_NAME, CAM_MIRROR_NONE);
			vo_mode_resp->result = 0;
			vo_mode_resp->channel = tutk_session->server_channel_id;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SET_VIDEOMODE_RESP, (char *)vo_mode_resp, sizeof(SMsgAVIoctrlSetVideoModeResp));
			break;
		default:
			break;
	}

	return;
}

static void *tutk_server_ioctrl_thread(void *arg)
{
	struct tutk_session_t *tutk_session = (struct tutk_session_t *)arg;
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct timeval now_time;
	struct timespec timeout_time;
	char ioctrl_buf[MAX_SIZE_IOCTRL_BUF];
    int ret, ioctrl_timeout = 1000, timeout_cnt = 0;
    unsigned int ioctrl_type;

	snprintf(ioctrl_buf, 16, "ioctrl%s", tutk_session->dev_name + strlen("device"));
	prctl(PR_SET_NAME, ioctrl_buf);
	pthread_mutex_lock(&tutk_session->io_mutex);
	tutk_session->thread_run_flag |= (1 << IOCTRL_THREAD_RUN);
	sem_post(&tutk->thread_sem);
	while (tutk_session->process_run) {

		if (tutk_session->ioctrl_cmd == CMD_SESSION_QUIT) {
			LOGD("tutk %s ioctrl thread quit \n", tutk_session->dev_name);
			break;
		}

		switch (tutk_session->ioctrl_cmd) {

			case CMD_SESSION_RECV_IOCTRL:
				memset(ioctrl_buf, 0, MAX_SIZE_IOCTRL_BUF);
				ret = avRecvIOCtrl(tutk_session->server_channel_id, &ioctrl_type, ioctrl_buf, MAX_SIZE_IOCTRL_BUF, ioctrl_timeout);
				if (ret >= 0) {
					timeout_cnt = 0;
					if (tutk->handle_ioctrl_cmd) {
						ret = tutk->handle_ioctrl_cmd(tutk_session, ioctrl_type, ioctrl_buf);
						if (ret == TUTK_CMD_DONE)
							break;
					}
					if ((ioctrl_type & 0xff001000) == 0xff001000) {
						tutk_server_ioctrl_infotm_cmd(tutk_session, ioctrl_type, ioctrl_buf);
					} else if (((ioctrl_type >= IOTYPE_USER_IPCAM_LISTWIFIAP_REQ) && (ioctrl_type <= IOTYPE_USER_IPCAM_GETWIFI_RESP_2))
								|| (ioctrl_type == IOTYPE_USER_IPCAM_BTCOMMAND_REQ) || (ioctrl_type == IOTYPE_USER_IPCAM_BTCOMMAND_RESP)
								|| (ioctrl_type == IOTYPE_USER_IPCAM_BTCOMMAND_RESULT_REQ) || (ioctrl_type == IOTYPE_USER_IPCAM_BTCOMMAND_RESULT_RESP)) {
						tutk_server_ioctrl_net_cmd(tutk_session, ioctrl_type, ioctrl_buf);
					} else {
						tutk_server_ioctrl_normal_cmd(tutk_session, ioctrl_type, ioctrl_buf);
					}
				} else {
					if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE)
						|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
						LOGD("ioctrl thread closed by remote %s %d \n", tutk_session->dev_name, ret);
						tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_SESSION, (unsigned)tutk_session);
						tutk_session->ioctrl_cmd = CMD_SESSION_NONE;
					} else if (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT) {
						timeout_cnt++;
						if (timeout_cnt > 2) {
							tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_SESSION, (unsigned)tutk_session);
							tutk_session->ioctrl_cmd = CMD_SESSION_NONE;
						}
						LOGD("ioctrl thread timeout %s %d \n", tutk_session->dev_name, timeout_cnt);
					}
				}
				break;
			default:
				break;
		}

		if (tutk_session->ioctrl_cmd == CMD_SESSION_NONE) {
			pthread_cond_wait(&tutk_session->io_cond, &tutk_session->io_mutex);
		} else {
			gettimeofday(&now_time, NULL);
			now_time.tv_usec += 50 * 1000;
			if (now_time.tv_usec >= 1000000) {
				now_time.tv_sec += now_time.tv_usec / 1000000;
				now_time.tv_usec %= 1000000;
			}
			timeout_time.tv_sec = now_time.tv_sec;
			timeout_time.tv_nsec = now_time.tv_usec * 1000;
			pthread_cond_timedwait(&tutk_session->io_cond, &tutk_session->io_mutex, &timeout_time);
		}
    }

	pthread_mutex_unlock(&tutk_session->io_mutex);
	pthread_exit(0);
}

static void *tutk_audio_recv_thread(void *arg)
{
	struct tutk_session_t *tutk_session = (struct tutk_session_t *)arg;
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	audio_fmt_t *recv_fmt = &tutk->play_fmt;
	struct cmd_data_t *cmd_data = NULL;
	struct codec_info recv_info;
	struct codec_addr recv_addr;
	struct list_head_t *l, *n;
	int frame_cnt, ret, recv_handle = -1, len, recv_delay = 0;
	int audio_recv_cmd = CMD_SESSION_NONE, req_cmd = CMD_SESSION_NONE;
	unsigned int frame_no;
	FRAMEINFO_t frame_info;
	char recv_buf[AUDIO_BUF_SIZE] = {0};
	char dec_buf[AUDIO_BUF_SIZE * 16];
	void *codec = NULL;
	int length;

	snprintf(recv_buf, 16, "au_recv%s", tutk_session->dev_name + strlen("device"));
	prctl(PR_SET_NAME, recv_buf);
	memset((char *)&recv_info, 0, sizeof(struct codec_info));
	memset((char *)&recv_addr, 0, sizeof(struct codec_addr));
	tutk_session->thread_run_flag |= (1 << AUDIO_PLAY_THREAD_RUN);
	sem_post(&tutk->thread_sem);
	while (tutk_session->process_run) {

		pthread_mutex_lock(&tutk_session->audio_recv_mutex);
		if (!list_empty(&tutk_session->audio_recv_cmd_list)) {
			l = tutk_session->audio_recv_cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			req_cmd = cmd_data->session_cmd;
			list_del(&cmd_data->list);
			free(cmd_data);
		} else {
			req_cmd = CMD_SESSION_NONE;
		}

		if ((req_cmd == CMD_SESSION_QUIT) || (req_cmd == CMD_SESSION_STOP_AUDIO) 
			|| ((req_cmd == CMD_SESSION_START_AUDIO) && (audio_recv_cmd == CMD_SESSION_NONE))
			|| (req_cmd == CMD_SESSION_RESTART_AUDIO)) {
			audio_recv_cmd = req_cmd;
			pthread_mutex_unlock(&tutk_session->audio_recv_mutex);
		} else {
			if (audio_recv_cmd == CMD_SESSION_NONE) {
				pthread_cond_wait(&tutk_session->audio_recv_cond, &tutk_session->audio_recv_mutex);
				pthread_mutex_unlock(&tutk_session->audio_recv_mutex);
				continue;
			} else {
				pthread_mutex_unlock(&tutk_session->audio_recv_mutex);
				if (recv_delay)
					usleep(recv_delay * 100);
				else
					usleep(2000);
			}
		}

		if (audio_recv_cmd == CMD_SESSION_QUIT) {
			if (recv_handle >= 0) {
				audio_put_channel(recv_handle);
				recv_handle = -1;
			}
			if (codec) {
				do {
					ret = codec_flush(codec, &recv_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				codec_close(codec);
				codec = NULL;
			}
			LOGD("tutk %s audio recv thread quit \n", tutk_session->dev_name);
			break;
		}

		switch (audio_recv_cmd) {
			case CMD_SESSION_START_AUDIO:
				if (recv_handle < 0) {
					recv_handle = audio_get_channel(tutk_session->audio_recv_channel, recv_fmt, CHANNEL_BACKGROUND);
					if (recv_handle < 0) {
						ERROR("audio recv get %s chanel failed %d \n", tutk_session->audio_recv_channel, recv_handle);
						break;
					}
					audio_get_format(tutk_session->audio_recv_channel, recv_fmt);
				}
				recv_info.out.codec_type = AUDIO_CODEC_PCM;
				recv_info.out.channel = recv_fmt->channels;
				recv_info.out.sample_rate = recv_fmt->samplingrate;
				recv_info.out.bitwidth = recv_fmt->bitwidth;
				recv_info.out.bit_rate = recv_info.out.channel * recv_info.out.sample_rate \
										* GET_BITWIDTH(recv_info.out.bitwidth);
				frame_cnt = avCheckAudioBuf(tutk_session->client_channel_id);
				if (frame_cnt >= 20) {
					audio_recv_cmd = CMD_SESSION_RECV_AUDIO;
					LOGD("tutk %s audio start recv stream %s handle %d \n", tutk_session->dev_name, tutk_session->audio_recv_channel, recv_handle);
				}
				break;
			case CMD_SESSION_STOP_AUDIO:
				if (recv_handle >= 0) {
					audio_put_channel(recv_handle);
					recv_handle = -1;
				}
				if (codec) {
					do {
						ret = codec_flush(codec, &recv_addr, &length);
						if (ret == CODEC_FLUSH_ERR)
							break;

						/* TODO you need least data or not ?*/
					} while (ret == CODEC_FLUSH_AGAIN);
					codec_close(codec);
					codec = NULL;
				}
				LOGD("tutk %s audio stop recv stream %s delay %d \n", tutk_session->dev_name, tutk_session->audio_recv_channel, recv_delay);
				recv_delay = 0;
				audio_recv_cmd = CMD_SESSION_NONE;
				break;
			case CMD_SESSION_RESTART_AUDIO:
				sprintf(tutk_session->audio_recv_channel, tutk->play_audio_channel);
				if (recv_handle >= 0) {
					audio_put_channel(recv_handle);
					recv_handle = -1;
					if (codec) {
						do {
							ret = codec_flush(codec, &recv_addr, &length);
							if (ret == CODEC_FLUSH_ERR)
								break;

							/* TODO you need least data or not ?*/
						} while (ret == CODEC_FLUSH_AGAIN);
						codec_close(codec);
						codec = NULL;
					}

					audio_recv_cmd = CMD_SESSION_START_AUDIO;
				} else {
					audio_recv_cmd = CMD_SESSION_NONE;
				}
				recv_delay = 0;
				LOGD("tutk %s audio restart recv stream %s handle %d \n", tutk_session->dev_name, tutk_session->audio_recv_channel, recv_handle);
				break;
			case CMD_SESSION_RECV_AUDIO:
				frame_cnt = avCheckAudioBuf(tutk_session->client_channel_id);
				if (frame_cnt > 0) {
					ret = avRecvAudioData(tutk_session->client_channel_id, recv_buf, AUDIO_BUF_SIZE, (char *)&frame_info, sizeof(FRAMEINFO_t), &frame_no);
					if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE) || (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
						|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
						ERROR("client had closed so stop recv %d \n",  ret);
						audio_recv_cmd = CMD_SESSION_STOP_AUDIO;
					} else if (ret == AV_ER_LOSED_THIS_FRAME) {
						ERROR("audio recv frame losed %d total %d \n", frame_no, frame_cnt);
					} else if (ret < 0) {
						ERROR("audio recv frame recive failed %d \n",  ret);
					} else {
						if (frame_info.codec_id == MEDIA_CODEC_AUDIO_G726)
							recv_info.in.codec_type = AUDIO_CODEC_G726;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_SPEEX)
							recv_info.in.codec_type = AUDIO_CODEC_SPEEX;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_ADPCM)
							recv_info.in.codec_type = AUDIO_CODEC_ADPCM;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_MP3)
							recv_info.in.codec_type = AUDIO_CODEC_MP3;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_PCM)
							recv_info.in.codec_type = AUDIO_CODEC_PCM;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_G711A)
							recv_info.in.codec_type = AUDIO_CODEC_G711A;
						else if (frame_info.codec_id == MEDIA_CODEC_AUDIO_G711U)
							recv_info.in.codec_type = AUDIO_CODEC_G711U;
						else {
							ERROR("audio recv not support encode type %x \n",  frame_info.codec_id);
							audio_recv_cmd = CMD_SESSION_STOP_AUDIO;
							break;
						}
						if ((frame_info.flags & 1) == AUDIO_CHANNEL_STERO)
							recv_info.in.channel = 2;
						else
							recv_info.in.channel = 1;
						if (((frame_info.flags >> 1) & 1) == AUDIO_DATABITS_16)
							recv_info.in.bitwidth = 16;
						else
							recv_info.in.bitwidth = 8;
						switch ((frame_info.flags >> 2) & 0xf) {
							case AUDIO_SAMPLE_48K:
								recv_info.in.sample_rate = 48000;
								break;
							case AUDIO_SAMPLE_44K:
								recv_info.in.sample_rate = 44100;
								break;
							case AUDIO_SAMPLE_8K:
								recv_info.in.sample_rate = 8000;
								break;
							default:
								recv_info.in.sample_rate = 8000;
								break;
						}
						recv_info.in.bit_rate = recv_info.in.channel * recv_info.in.sample_rate \
												* GET_BITWIDTH(recv_info.in.bitwidth);

						if (!codec) {
							codec = codec_open(&recv_info);
							if (!codec) {
								ERROR("audio recv open codec decoder failed %x \n",  frame_info.codec_id);
								break;
							}
						}
						recv_addr.in = recv_buf;
						recv_addr.len_in = ret;
						recv_addr.out = dec_buf;
						recv_addr.len_out = AUDIO_BUF_SIZE * 16;
						ret = codec_decode(codec, &recv_addr);
						if (ret < 0) {
							ERROR("audio recv codec decoder failed %d %x \n",  ret, frame_info.codec_id);
							do {
								ret = codec_flush(codec, &recv_addr, &length);
								if (ret == CODEC_FLUSH_ERR)
									break;

								/* TODO you need least data or not ?*/
							} while (ret == CODEC_FLUSH_AGAIN);
							codec_close(codec);
							codec = NULL;
							break;
						}
						len = ret;
						ret = audio_write_frame(recv_handle, recv_addr.out, len);
						if (ret < 0)
							ERROR("audio recv codec play failed %d %x \n",  ret, frame_info.codec_id);
						if (!recv_delay) {
							recv_delay = recv_info.in.sample_rate * recv_info.in.channel * (recv_info.in.bitwidth / 8);
							if (recv_info.in.codec_type == AUDIO_CODEC_ADPCM)
								recv_delay = recv_delay / recv_addr.len_in;
							else if (recv_info.in.codec_type == AUDIO_CODEC_G711A)
								recv_delay = recv_delay / (recv_addr.len_in * 2);
							else
								recv_delay = recv_delay / recv_addr.len_in;
							recv_delay = 1000 / recv_delay;
							recv_delay -= 5;
						}
						//LOGD("audio recv frame %d %d %d %x %x %d \n", temp_time, recv_addr.len_in, len, frame_info.codec_id, frame_info.timestamp, frame_cnt);
#if 0
						long fsize;
						int fd;
					    fd = open("sd/audio_stream", (O_CREAT | O_WRONLY));
					    if (fd < 0) {
					        LOGD("Cannot open file %s: %s \n", "sd/audio_stream", strerror(errno));
					    }
					    fsize = lseek(fd, 0L, SEEK_END);
					    LOGD("audio recv save %ld %d \n", fsize, len);

					    ret = lseek(fd, fsize, SEEK_SET);
					    if (ret < 0) {
					        LOGD("lseek failed with %s.\n", strerror(errno));
					    }
					    if (write(fd, recv_addr.out, len) < 0) {
					        LOGD("Cannot write to \"%s\": %s \n", "sd/audio_stream", strerror(errno));
					    }
					    close(fd);
#endif
					}
				}
				break;
			default:
				break;
		}
	}

	list_for_each_safe(l, n, &tutk_session->audio_recv_cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	tutk_session->thread_run_flag &= ~(1 << AUDIO_PLAY_THREAD_RUN);
	pthread_exit(0);
}

static void *tutk_audio_send_thread(void *arg)
{
	struct tutk_session_t *tutk_session = (struct tutk_session_t *)arg;
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	audio_fmt_t *send_fmt = &tutk->capture_fmt;
	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
	struct cmd_data_t *cmd_data = NULL;
	struct timeval internel_time, latest_time;
	struct codec_info send_info;
	struct codec_addr send_addr;
	struct list_head_t *l, *n;
	int frame_cnt = 0, ret, send_handle = -1, len = 0, frame_rate, latest_frame_cnt = 0, start_timestamp = 0;
	int audio_cmd = CMD_SESSION_NONE, req_cmd = CMD_SESSION_NONE;
	FRAMEINFO_t frame_info;
	char enc_buf[AUDIO_BUF_SIZE] = {0};
	void *codec = NULL;
	int length;

#if 0
	char send_buf[AUDIO_BUF_SIZE*4] = {0};
	long fsize;
	int fd;
#endif

	snprintf(enc_buf, 16, "au_send%s", tutk_session->dev_name + strlen("device"));
	prctl(PR_SET_NAME, enc_buf);
	memset((char *)&send_info, 0, sizeof(struct codec_info));
	memset((char *)&send_addr, 0, sizeof(struct codec_addr));
	tutk_session->thread_run_flag |= (1 << AUDIO_CAPTURE_THREAD_RUN);
	sem_post(&tutk->thread_sem);
	while (tutk_session->process_run) {

		pthread_mutex_lock(&tutk_session->audio_mutex);
		if (!list_empty(&tutk_session->audio_send_cmd_list)) {
			l = tutk_session->audio_send_cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			req_cmd = cmd_data->session_cmd;
			list_del(&cmd_data->list);
			free(cmd_data);
		} else {
			req_cmd = CMD_SESSION_NONE;
		}

		if ((req_cmd == CMD_SESSION_QUIT) || (req_cmd == CMD_SESSION_STOP_AUDIO) 
			|| ((req_cmd == CMD_SESSION_START_AUDIO) && (audio_cmd == CMD_SESSION_NONE))
			|| (req_cmd == CMD_SESSION_RESTART_AUDIO)) {
			audio_cmd = req_cmd;
			pthread_mutex_unlock(&tutk_session->audio_mutex);
		} else {
			if (audio_cmd == CMD_SESSION_NONE) {
				pthread_cond_wait(&tutk_session->audio_cond, &tutk_session->audio_mutex);
				pthread_mutex_unlock(&tutk_session->audio_mutex);
				continue;
			} else {
				pthread_mutex_unlock(&tutk_session->audio_mutex);
				usleep(3000);
			}
		}

		if (audio_cmd == CMD_SESSION_QUIT) {
			if (send_handle >= 0) {
				audio_put_channel(send_handle);
				send_handle = -1;
			}
			if (codec) {
				do {
					ret = codec_flush(codec, &send_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				codec_close(codec);
				codec = NULL;
			}
			LOGD("tutk %s audio send thread quit \n", tutk_session->dev_name);
			break;
		}

		switch (audio_cmd) {
			case CMD_SESSION_START_AUDIO:
				if (send_handle < 0) {
					send_handle = audio_get_channel(tutk_session->audio_send_channel, send_fmt, CHANNEL_BACKGROUND);
					if (send_handle < 0) {
						ERROR("audio send get %s chanel failed %d \n", tutk_session->audio_send_channel, send_handle);
						break;
					}
					audio_get_format(tutk_session->audio_send_channel, send_fmt);
				}
				fr_INITBUF(&fr_send_buf, NULL);

				send_info.in.codec_type = AUDIO_CODEC_PCM;
				send_info.in.channel = send_fmt->channels;
				send_info.in.sample_rate = send_fmt->samplingrate;
				send_info.in.bitwidth = send_fmt->bitwidth;
				send_info.in.bit_rate = send_info.in.channel * send_info.in.sample_rate \
										* GET_BITWIDTH(send_info.in.bitwidth);
				send_info.out.codec_type = AUDIO_CODEC_G711A;
				send_info.out.channel = 1;
				send_info.out.sample_rate = 8000;
				send_info.out.bitwidth = 16;
				send_info.out.bit_rate = send_info.out.channel * send_info.out.sample_rate \
										* GET_BITWIDTH(send_info.out.bitwidth);
				codec = codec_open(&send_info);
				if (!codec) {
					ERROR("audio send open codec decoder failed %x \n",  send_info.out.codec_type);
					break;
				}

				audio_cmd = CMD_SESSION_SEND_AUDIO;
				LOGD("tutk %s audio start send stream %s handle %d \n", tutk_session->dev_name, tutk_session->audio_send_channel, send_handle);
				break;
			case CMD_SESSION_STOP_AUDIO:
				if (send_handle >= 0) {
					audio_put_channel(send_handle);
					send_handle = -1;
				}
				if (codec) {
					do {
						ret = codec_flush(codec, &send_addr, &length);
						if (ret == CODEC_FLUSH_ERR)
							break;

						/* TODO you need least data or not ?*/
					} while (ret == CODEC_FLUSH_AGAIN);
					codec_close(codec);
					codec = NULL;
				}

				frame_cnt = 0;
				latest_frame_cnt = 0;
				audio_cmd = CMD_SESSION_NONE;
				LOGD("tutk %s audio stop send stream %s handle %d \n", tutk_session->dev_name, tutk_session->audio_send_channel, send_handle);
				break;
			case CMD_SESSION_RESTART_AUDIO:
				sprintf(tutk_session->audio_send_channel, tutk->rec_audio_channel);
				if (send_handle >= 0) {
					audio_put_channel(send_handle);
					send_handle = -1;
					if (codec) {
						do {
							ret = codec_flush(codec, &send_addr, &length);
							if (ret == CODEC_FLUSH_ERR)
								break;

							/* TODO you need least data or not ?*/
						} while (ret == CODEC_FLUSH_AGAIN);
						codec_close(codec);
						codec = NULL;
					}

					frame_cnt = 0;
					latest_frame_cnt = 0;
					audio_cmd = CMD_SESSION_START_AUDIO;
				} else {
					audio_cmd = CMD_SESSION_NONE;
				}
				LOGD("tutk %s audio restart send stream %s handle %d \n", tutk_session->dev_name, tutk_session->audio_send_channel, send_handle);
				break;
			case CMD_SESSION_SEND_AUDIO:
#if 0
				fd = open("/mnt/mmc/capture_stream", O_RDWR);
				if (fd < 0) {
					LOGD("Cannot open file %s: %s \n", "sd/audio_stream", strerror(errno));
				}

				fsize = lseek(fd, 0L, SEEK_END);
				ret = lseek(fd, (4000 * frame_cnt % fsize), SEEK_SET);
				if (ret < 0) {
					LOGD("lseek failed with %s.\n", strerror(errno));
				}
				if (read(fd, send_buf, 4000) < 0) {
					LOGD("Cannot read to \"%s\": %d %s \n", "sd/audio_stream", 4000 * frame_cnt, strerror(errno));
				}
				close(fd);

				frame_info.timestamp = start_timestamp + frame_cnt * 62;
				frame_info.codec_id = MEDIA_CODEC_AUDIO_G711A;
				frame_info.flags = ((AUDIO_SAMPLE_8K << 2) | (AUDIO_DATABITS_16 << 1) | AUDIO_CHANNEL_MONO);
				len = AUDIO_BUF_SIZE;

				send_addr.in = send_buf;
				send_addr.len_in = 4000;
				send_addr.out = dec_buf;
				send_addr.len_out = AUDIO_BUF_SIZE;
				ret = codec_decode(de_codec, &send_addr);
				if (ret < 0) {
					ERROR("audio recv codec decoder failed %d %x \n",  ret, frame_info.codec_id);
					break;
				}

				send_addr.in = dec_buf;
				send_addr.len_in = ret;
				send_addr.out = enc_buf;
				send_addr.len_out = ret;
				ret = codec_encode(codec, &send_addr);
				if (ret < 0) {
					ERROR("audio send codec decoder failed %d %x \n",  ret, frame_info.codec_id);
					break;
				}
				len = ret;
#else
				ret = audio_get_frame(send_handle, &fr_send_buf);
				if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
					ERROR("tutk send audio get frame failed %d %d %d \n", send_handle, ret, fr_send_buf.size);
					//audio_put_frame(send_handle, &fr_send_buf);
					break;
				}
				frame_info.timestamp = fr_send_buf.timestamp - start_timestamp;

				send_addr.in = fr_send_buf.virt_addr;
				send_addr.len_in = fr_send_buf.size;
				send_addr.out = enc_buf;
				send_addr.len_out = AUDIO_BUF_SIZE;
				ret = codec_encode(codec, &send_addr);
				if (ret < 0) {
					ERROR("audio send codec encoder failed %d \n",  ret);
				}
				len = ret;
				audio_put_frame(send_handle, &fr_send_buf);
				//LOGD("audio send frame start %d %d %d %d %d %d \n", send_handle, fr_send_buf.size, send_addr.len_in, len, frame_info.timestamp, tutk_session->base_timestamp);
#endif
				if (len == 0)
					break;
				frame_info.codec_id = MEDIA_CODEC_AUDIO_G711A;
				frame_info.flags = ((AUDIO_SAMPLE_8K << 2) | (AUDIO_DATABITS_16 << 1) | AUDIO_CHANNEL_MONO);
				ret = avSendAudioData(tutk_session->server_channel_id, enc_buf, len, &frame_info, sizeof(FRAMEINFO_t));
				if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE) || (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
				|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
					ERROR("client had closed so stop audio send %d \n",  ret);
					audio_cmd = CMD_SESSION_STOP_AUDIO;
				} else if (ret == AV_ER_EXCEED_MAX_SIZE) {
					ERROR("audio frame send tutk buf full %d \n",  ret);
				} else if (ret < 0) {
					ERROR("audio frame send failed %d \n",  ret);
				} else {
					frame_cnt++;
					gettimeofday(&internel_time, NULL);
					if ((internel_time.tv_sec - latest_time.tv_sec) >= FPS_PRINT_INTERNAL_TIME) {
						frame_rate = (frame_cnt - latest_frame_cnt) / FPS_PRINT_INTERNAL_TIME;
						printf("tutk audio %s had send %d frame to %s fps is %d timestamp %d \n", tutk_session->audio_send_channel, frame_cnt, tutk_session->dev_name, frame_rate, frame_info.timestamp);
						gettimeofday(&latest_time, NULL);
						latest_frame_cnt = frame_cnt;
					}
				}
				break;
			default:
				break;
		}
	}

	list_for_each_safe(l, n, &tutk_session->audio_send_cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	tutk_session->thread_run_flag &= ~(1 << AUDIO_CAPTURE_THREAD_RUN);
	pthread_exit(0);
}

static void tutk_session_add_cmd(struct tutk_session_t *tutk_session, int cmd_type, int cmd, unsigned cmd_arg)
{
	struct cmd_data_t *cmd_data = NULL;

	cmd_data = (struct cmd_data_t *)malloc(sizeof(struct cmd_data_t));
	if (cmd_data == NULL)
		return;

	if (cmd_type == CMD_SESSION_VIDEO_SEND) {
		pthread_mutex_lock(&tutk_session->video_mutex);
		cmd_data->session_cmd = cmd;
		cmd_data->cmd_arg = cmd_arg;
		if (cmd_data->session_cmd == CMD_SESSION_QUIT)
			list_add(&cmd_data->list, &tutk_session->video_send_cmd_list);
		else
			list_add_tail(&cmd_data->list, &tutk_session->video_send_cmd_list);
		pthread_cond_signal(&tutk_session->video_cond);
		pthread_mutex_unlock(&tutk_session->video_mutex);
	} else if (cmd_type == CMD_SESSION_AUDIO_SEND) {
		pthread_mutex_lock(&tutk_session->audio_mutex);
		cmd_data->session_cmd = cmd;
		cmd_data->cmd_arg = cmd_arg;
		if (cmd_data->session_cmd == CMD_SESSION_QUIT)
			list_add(&cmd_data->list, &tutk_session->audio_send_cmd_list);
		else
			list_add_tail(&cmd_data->list, &tutk_session->audio_send_cmd_list);
		pthread_cond_signal(&tutk_session->audio_cond);
		pthread_mutex_unlock(&tutk_session->audio_mutex);
	} else if (cmd_type == CMD_SESSION_AUDIO_RECV) {
		pthread_mutex_lock(&tutk_session->audio_recv_mutex);
		cmd_data->session_cmd = cmd;
		cmd_data->cmd_arg = cmd_arg;
		if (cmd_data->session_cmd == CMD_SESSION_QUIT)
			list_add(&cmd_data->list, &tutk_session->audio_recv_cmd_list);
		else
			list_add_tail(&cmd_data->list, &tutk_session->audio_recv_cmd_list);
		pthread_cond_signal(&tutk_session->audio_recv_cond);
		pthread_mutex_unlock(&tutk_session->audio_recv_mutex);
	} else if (cmd_type == CMD_SESSION_PLAYBACK) {
		if (tutk_session->thread_run_flag & (1 << PLAYBACK_THREAD_RUN)) {
			pthread_mutex_lock(&tutk_session->play_mutex);
			cmd_data->session_cmd = cmd;
			cmd_data->cmd_arg = cmd_arg;
			if (cmd_data->session_cmd == CMD_SESSION_QUIT)
				list_add(&cmd_data->list, &tutk_session->playback_cmd_list);
			else
				list_add_tail(&cmd_data->list, &tutk_session->playback_cmd_list);
			pthread_cond_signal(&tutk_session->play_cond);
			pthread_mutex_unlock(&tutk_session->play_mutex);
		} else {
			free(cmd_data);
		}
	}

	return;
}

static void tutk_video_change_scene(struct tutk_common_t *tutk, char *video_channel, int scene_mode)
{
	struct v_bitrate_info *bitrate_info = NULL;
	int fps = 15;

	switch (scene_mode) {
		case DAYTIME_STATIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
		case DAYTIME_DYNAMIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL];
				fps = 10;
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL];
			}
			break;
		case NIGHTTIME_STATIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
		case NIGHTTIME_DYNAMIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL];
				fps = 10;
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL];
			}
			break;
		default:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
	}

	if (bitrate_info) {
		video_set_fps(video_channel, fps);
		bitrate_info->gop_length = fps;
		video_set_bitrate(video_channel, bitrate_info);
	}

	return;
}

static void *tutk_video_send_thread(void *arg)
{
	struct tutk_session_t *tutk_session = (struct tutk_session_t *)arg;
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct cmd_data_t *cmd_data = NULL;
	struct timeval internel_time, latest_time;
	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
	struct v_basic_info video_info;
	struct list_head_t *l, *n;
	FRAMEINFO_t frame_info;
    int ret, frame_flag = VIDEO_FRAME_P, head_len = 0, send_len, frame_cnt = 0, latest_frame_cnt = 0, frame_rate = 0, send_error_cnt = 0;
    int bit_total = 0, bit_rate, video_cmd = CMD_SESSION_NONE, req_cmd = CMD_SESSION_NONE;
    char header[256], channel_name[32] = {0};
    char *video_buf = tutk_session->video_buf, *send_buf;

	snprintf(header, 16, "vd_send%s", tutk_session->dev_name + strlen("device"));
	prctl(PR_SET_NAME, header);
	tutk_session->thread_run_flag |= (1 << VIDEO_THREAD_RUN);
	sem_post(&tutk->thread_sem);
	while (tutk_session->process_run) {

		pthread_mutex_lock(&tutk_session->video_mutex);
		if (!list_empty(&tutk_session->video_send_cmd_list)) {
			l = tutk_session->video_send_cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			req_cmd = cmd_data->session_cmd;
			list_del(&cmd_data->list);
			free(cmd_data);
		} else {
			req_cmd = CMD_SESSION_NONE;
		}

		if ((req_cmd == CMD_SESSION_QUIT) || (req_cmd == CMD_SESSION_STOP_VIDEO) 
			|| ((req_cmd == CMD_SESSION_START_VIDEO) && (video_cmd == CMD_SESSION_NONE))
			|| (req_cmd == CMD_SESSION_SET_RESOLUTION)
			|| (req_cmd == CMD_SESSION_CHANGE_SCENE)) {
			ret = video_cmd;
			video_cmd = req_cmd;
			req_cmd = ret;
			pthread_mutex_unlock(&tutk_session->video_mutex);
		} else {
			if (video_cmd == CMD_SESSION_NONE) {
				pthread_cond_wait(&tutk_session->video_cond, &tutk_session->video_mutex);
				pthread_mutex_unlock(&tutk_session->video_mutex);
				continue;
			} else {
				pthread_mutex_unlock(&tutk_session->video_mutex);
				usleep(30000);
			}
		}

		if (video_cmd == CMD_SESSION_QUIT) {
			if (channel_name[0] != 0) {
				video_disable_channel(channel_name);
				memset(channel_name, 0, 32);
			}
			LOGD("tutk %s video thread quit \n", tutk_session->dev_name);
			break;
		}

		switch (video_cmd) {
			case CMD_SESSION_START_VIDEO:
				ret = video_enable_channel(tutk_session->video_channel);
				if (ret < 0) {
					video_cmd = CMD_SESSION_NONE;
					break;
				}
				video_set_frame_mode(tutk_session->video_channel, VENC_HEADER_IN_FRAME_MODE);
				tutk_video_change_scene(tutk, tutk_session->video_channel, tutk->scene_mode);

				video_get_basicinfo(tutk_session->video_channel, &video_info);
				head_len = video_get_header(tutk_session->video_channel, header, 128);
				if (head_len < 0) {
					video_cmd = CMD_SESSION_STOP_VIDEO;
					break;
				}
				memcpy(video_buf, header, head_len);
				sprintf(channel_name, tutk_session->video_channel);
				fr_INITBUF(&fr_send_buf, NULL);
				memset(&frame_info, 0, sizeof(FRAMEINFO_t));
				if (video_info.media_type == VIDEO_MEDIA_H264)
					frame_info.codec_id = MEDIA_CODEC_VIDEO_H264;
				else
					frame_info.codec_id = MEDIA_CODEC_VIDEO_H265;
				gettimeofday(&latest_time, NULL);
				video_cmd = CMD_SESSION_SEND_VIDEO;
				tutk_session->video_status = 1;
                send_error_cnt = 0;
				LOGD("%s video_send_thread %s start %d\n", tutk_session->dev_name, tutk_session->video_channel, head_len);
				break;
			case CMD_SESSION_STOP_VIDEO:
				frame_cnt = 0;
				latest_frame_cnt = 0;
				bit_total = 0;
				frame_flag = VIDEO_FRAME_P;
				if (channel_name[0] != 0) {
					video_disable_channel(channel_name);
					memset(channel_name, 0, 32);
				}
				video_cmd = CMD_SESSION_NONE;
				req_cmd = CMD_SESSION_NONE;
				tutk_session->video_status = 0;
                send_error_cnt = 0;
				LOGD("%s video_send_thread %s stop %d \n", tutk_session->dev_name, tutk_session->video_channel, head_len);
				break;
			case CMD_SESSION_SEND_VIDEO:
				ret = video_get_frame(tutk_session->video_channel, &fr_send_buf);
				if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
					LOGD("video_get_frame error %d %d \n", ret, fr_send_buf.size);
					frame_flag = VIDEO_FRAME_P;
					break;
				}
				if (fr_send_buf.size > (VIDEO_BUF_SIZE - head_len)) {
					LOGD("video_get_frame big %d \n", fr_send_buf.size);
					if (fr_send_buf.priv == VIDEO_FRAME_I) 
						frame_flag = VIDEO_FRAME_P;
					video_put_frame(tutk_session->video_channel, &fr_send_buf);
					break;
				}
				frame_info.timestamp = fr_send_buf.timestamp;
				if (frame_cnt == 0) {
					//start_timestamp = frame_info.timestamp;
					LOGD("start timestamp %d \n", frame_info.timestamp);
				}
				tutk_session->base_timestamp = frame_info.timestamp;

				frame_info.onlineNum = tutk->actual_session_num;
				if (fr_send_buf.priv == VIDEO_FRAME_I) {
					frame_info.flags = IPC_FRAME_FLAG_IFRAME;
					frame_flag = VIDEO_FRAME_I;
					//memcpy(video_buf + head_len, fr_send_buf.virt_addr, fr_send_buf.size);
					//send_buf = video_buf;
					//send_len = head_len + fr_send_buf.size;
					send_buf = fr_send_buf.virt_addr;
					send_len = fr_send_buf.size;
				} else {
					if (frame_flag == VIDEO_FRAME_P) {
						video_put_frame(tutk_session->video_channel, &fr_send_buf);
						video_trigger_key_frame(tutk_session->video_channel);
						break;
					}
					frame_info.flags = IPC_FRAME_FLAG_PBFRAME;
					send_buf = fr_send_buf.virt_addr;
					send_len = fr_send_buf.size;
				}

				ret = avSendFrameData(tutk_session->server_channel_id, send_buf, send_len, &frame_info, sizeof(FRAMEINFO_t));
				if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE) || (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
					|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
					ERROR("client %s had closed so stop video send %d \n", tutk_session->dev_name, tutk_session->server_channel_id);
					video_cmd = CMD_SESSION_STOP_VIDEO;
				} else if (ret == AV_ER_EXCEED_MAX_SIZE) {
					ERROR("%s video frame send tutk buf full %d \n", tutk_session->dev_name, tutk_session->server_channel_id);
				} else if (ret < 0) {
                    send_error_cnt ++ ;
                    ERROR("%s video frame send failed %d error cnt:%d ret:%d\n", tutk_session->dev_name, tutk_session->server_channel_id, send_error_cnt, ret);
                    if (send_error_cnt >= MAX_ERROR_CNT)
                    {
                        ERROR("errcnt:%d, reset session!\n", send_error_cnt);
                        send_error_cnt = 0;
                        video_cmd = CMD_SESSION_STOP_VIDEO;
                    }
				} else {
                    send_error_cnt = 0;
					frame_cnt++;
					bit_total += fr_send_buf.size;
					gettimeofday(&internel_time, NULL);
					if ((internel_time.tv_sec - latest_time.tv_sec) >= FPS_PRINT_INTERNAL_TIME) {
						frame_rate = (frame_cnt - latest_frame_cnt) / FPS_PRINT_INTERNAL_TIME;
						bit_rate = (bit_total * 8) / FPS_PRINT_INTERNAL_TIME;
						printf("tutk %s had send %d frame to %s fps is %d bps is %d timestamp %d \n", tutk_session->video_channel, frame_cnt, tutk_session->dev_name, frame_rate, bit_rate, frame_info.timestamp);
						gettimeofday(&latest_time, NULL);
						latest_frame_cnt = frame_cnt;
						bit_total = 0;
					}
				}
				video_put_frame(tutk_session->video_channel, &fr_send_buf);
				break;
			case CMD_SESSION_SET_RESOLUTION:
				switch (tutk_session->video_quality) {
					case AVIOCTRL_QUALITY_MAX:
						sprintf(tutk_session->video_channel, DEFAULT_HIGH_CHANNEL);
						break;
					case AVIOCTRL_QUALITY_MIDDLE:
						sprintf(tutk_session->video_channel, DEFAULT_PREVIEW_CHANNEL);
						break;
					case AVIOCTRL_QUALITY_MIN:
						sprintf(tutk_session->video_channel, DEFAULT_LOW_CHANNEL);
						break;
					default:
						sprintf(tutk_session->video_channel, DEFAULT_PREVIEW_CHANNEL);
						break;
				}

				//video_set_resolution(tutk_session->video_channel, width, height);
				if (strcmp(channel_name, tutk_session->video_channel)) {
					frame_cnt = 0;
					latest_frame_cnt = 0;
					bit_total = 0;
					frame_flag = VIDEO_FRAME_P;
					if (channel_name[0] != 0) {
						video_disable_channel(channel_name);
						memset(channel_name, 0, 32);
					}
					video_cmd = CMD_SESSION_START_VIDEO;
				} else {
					if (req_cmd == CMD_SESSION_SEND_VIDEO)
						video_cmd = req_cmd;
					else
						video_cmd = CMD_SESSION_START_VIDEO;
				}
				LOGD("tutk %s send video set resolution %s %d \n", tutk_session->dev_name, tutk_session->video_channel, video_cmd);
				break;
			case CMD_SESSION_CHANGE_SCENE:
				if (channel_name[0] != 0)
					tutk_video_change_scene(tutk, channel_name, tutk->scene_mode);
				video_cmd = req_cmd;
				break;
			default:
				break;
		}
	}

	list_for_each_safe(l, n, &tutk_session->video_send_cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	tutk_session->thread_run_flag &= ~(1 << VIDEO_THREAD_RUN);
	pthread_exit(0);
}

static void tutk_record_file_list(struct tutk_common_t *tutk, int mounted)
{
	struct record_file_t *record_file;
	struct list_head_t *l, *n;
	DIR *dir;
	struct dirent *ptr;
	struct tm tm_current;
	char event_type[32], file[128], *temp;
	int duration, ret;

	if (mounted) {
		dir = opendir(tutk->rec_path);
		if (dir == NULL) {
			ERROR("tutk have not record dir %s \n", tutk->rec_path);
			return; 
		} else {

			pthread_mutex_lock(&tutk->rec_file_mutex);
			while ((ptr = readdir(dir)) != NULL) {
		        if (strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
		            continue;

				temp = strstr(ptr->d_name, ".");
				if (temp == NULL)
					continue;

				snprintf(file, ((unsigned)temp - (unsigned)ptr->d_name + 1), ptr->d_name);
				temp = strstr(ptr->d_name, "@@@@");
				if (temp != NULL) {
					sprintf(file, "%s/%s", tutk->rec_path, ptr->d_name);
					remove(file);
					continue;
				}

				memset(&tm_current, 0, sizeof(struct tm));
				memset(event_type, 0, 32);
				duration = 0;
				ret = sscanf(file, "%d_%d_%d_%d_%d_%d_%s",
						&tm_current.tm_year, &tm_current.tm_mon, &tm_current.tm_mday,
						&tm_current.tm_hour, &tm_current.tm_min, &tm_current.tm_sec, event_type);

				temp = strstr(event_type, "_");
				if (temp != NULL) {
					duration = atoi(temp + 1);
					temp[0] = '\0';
				}
				if ((ret < 0) || (duration == 0)) {
					//ERROR("not valid record file %s  %s \n", file, event_type);
					continue;
				}
				if (duration < 180) {
					sprintf(file, "%s/%s", tutk->rec_path, ptr->d_name);
				    remove(file);
				    LOGD("remove record file %s duration %d \n", file, duration);
					continue;
				}

				record_file = malloc(sizeof(struct record_file_t));
				if (record_file == NULL)
					continue;

				snprintf(record_file->file_name, 64, "%s", file);
				list_add_tail(&record_file->list, &tutk->record_list);
			}
			pthread_mutex_unlock(&tutk->rec_file_mutex);
		}
		closedir(dir);
	} else {
		pthread_mutex_lock(&tutk->rec_file_mutex);
		list_for_each_safe(l, n, &tutk->record_list) {
			record_file = container_of(l, struct record_file_t, list);
			list_del(&record_file->list);
			free(record_file);
		}
		pthread_mutex_unlock(&tutk->rec_file_mutex);
	}

	return;
}

static void tutk_record_del_old(struct tutk_common_t *tutk)
{
	struct record_file_t *record_file, *del_record_file = NULL;
	struct list_head_t *l, *n;
	char file[256], del_file[256], report_file[128], *temp = NULL, event_type[16];
	time_t current = {0}, prev = {0};
	struct tm tm_current;
	int ret, duration = 0;

	pthread_mutex_lock(&tutk->rec_file_mutex);
    list_for_each_safe(l, n, &tutk->record_list) {

		record_file = container_of(l, struct record_file_t, list);
		memset(&tm_current, 0, sizeof(struct tm));
		ret = sscanf(record_file->file_name, "%d_%d_%d_%d_%d_%d_%s",
				&tm_current.tm_year, &tm_current.tm_mon, &tm_current.tm_mday,
				&tm_current.tm_hour, &tm_current.tm_min, &tm_current.tm_sec, event_type);
		if (ret < 0) {
			ERROR("not valid record file %s \n", record_file->file_name);
			continue;
		}

		temp = strstr(event_type, "_");
		if (temp != NULL) {
			duration = atoi(temp + 1);
			temp[0] = '\0';
		}
		if (duration < 180) {
			sprintf(file, "%s/%s.mkv", tutk->rec_path, record_file->file_name);
			remove(file);
			list_del(&record_file->list);
			free(record_file);
			continue;
		}
		//LOGD(" %d_%d_%d_%d_%d_%d_%s \n", tm_current.tm_year, tm_current.tm_mon, tm_current.tm_mday,
					//tm_current.tm_hour, tm_current.tm_min, tm_current.tm_sec, event_type);

		tm_current.tm_year -= 1900;
		current = mktime(&tm_current);
		if ((current < prev) || (prev == 0)) {
			prev = current;
			sprintf(del_file, "%s/%s.mkv", tutk->rec_path, record_file->file_name);
			sprintf(report_file, record_file->file_name);
			del_record_file = record_file;
		}
    }
    if (!memcmp(del_file, tutk->rec_path, strlen(tutk->rec_path))) {
    	LOGD("tutk del event file %s %s \n", del_file, report_file);
    	remove(del_file);
    	if (del_record_file != NULL) {
			list_del(&del_record_file->list);
			free(del_record_file);
		}
		if (tutk->event_report)
			tutk->event_report(tutk, EVENT_DELFILE, report_file);
	}
	pthread_mutex_unlock(&tutk->rec_file_mutex);

	return;
}

static void tutk_record_handle(char *event, void *arg)
{
	struct tutk_common_t *tutk = tutk_save;
	vplay_event_info_t *tutk_vplay = (vplay_event_info_t *)arg;
	struct record_file_t *record_file;
	char file[256], *temp;

	if (!strncmp(event, EVENT_VPLAY, strlen(EVENT_VPLAY))) {
		LOGD("tutk_record_handle %s %s %d \n", event, (char *)tutk_vplay->buf, tutk->tf_card_state);
		if (tutk_vplay->type == VPLAY_EVENT_NEWFILE_FINISH) {
			if (!tutk->tf_card_state)
				return;

			temp = strstr((char *)tutk_vplay->buf, ".");
			if (temp == NULL)
				return;

			snprintf(file, ((unsigned)temp - (unsigned)tutk_vplay->buf + 1), (char *)tutk_vplay->buf);
			do {
				temp = strstr(file, "/");
				if (temp != NULL)
					sprintf(file, temp + 1);
			} while (temp != NULL);

			record_file = malloc(sizeof(struct record_file_t));
			if (record_file == NULL)
				return;
			snprintf(record_file->file_name, 64, "%s", file);
			pthread_mutex_lock(&tutk->rec_file_mutex);
			list_add_tail(&record_file->list, &tutk->record_list);
			if (tutk->event_report)
				tutk->event_report(tutk, EVENT_ADDFILE, file);
			pthread_mutex_unlock(&tutk->rec_file_mutex);
		} else if (tutk_vplay->type == VPLAY_EVENT_DISKIO_ERROR) {
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_RECORD, 0);
		}
	}

	return;
}

static void tutk_record_init(struct tutk_common_t *tutk)
{
	struct v_basic_info video_info;
	VRecorderInfo record_info;
	int ret = 0;

	ret = video_get_basicinfo(tutk->rec_video_channel, &video_info);
	if (ret)
		return;
	memset(&record_info, 0, sizeof(VRecorderInfo));
	sprintf(record_info.audio_channel, tutk->rec_audio_channel);
	sprintf(record_info.video_channel, tutk->rec_video_channel);
	record_info.audio_format.type = AUDIO_CODEC_TYPE_G711A;
	record_info.audio_format.channels = 1;
	record_info.audio_format.sample_fmt = AUDIO_SAMPLE_FMT_S16;
	record_info.audio_format.sample_rate = 8000;
	record_info.audio_format.effect = 0;
	record_info.enable_gps = 0;
	record_info.keep_temp = 0;
	record_info.time_segment = tutk->rec_time_segment;
	if (!strcmp(tutk->rec_type, FILE_SAVE_ALARM)) {
		video_enable_channel(tutk->rec_video_channel);
		video_set_bitrate(record_info.video_channel, &record_scene_info);
		record_info.time_backward = 30;
	} else {
		record_info.time_backward = 0;
	}
	sprintf(record_info.dir_prefix, "%s/", tutk->rec_path);
	sprintf(record_info.av_format, "mkv");
	strcat(record_info.time_format, "%Y_%m_%d_%H_%M");
	strcat(record_info.suffix, "%ts_0_");
	strcat(record_info.suffix, tutk->rec_type);
	strcat(record_info.suffix, "_%l");
	tutk->tutk_rec = vplay_new_recorder(&record_info);

	return;
}

static void tutk_record_deinit(struct tutk_common_t *tutk)
{
	if (tutk->tutk_rec) {
		vplay_delete_recorder(tutk->tutk_rec);
		if (!strcmp(tutk->rec_type, FILE_SAVE_ALARM))
			video_disable_channel(tutk->rec_video_channel);
	}
	tutk->tutk_rec = NULL;

	return;
}

static void *tutk_record_thread(void *arg)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)arg;
	struct cmd_data_t *cmd_data = NULL;
	storage_info_t si;
	struct timeval now_time;
	struct timespec timeout_time;
	struct list_head_t *l, *n;
	unsigned int total_size, free_size;
	int req_cmd, rec_cmd = CMD_TUTK_NONE, i, rec_start = 0;

	prctl(PR_SET_NAME, "tutk_record");
	if (tutk->tf_card_state) {
		if (!strcmp(tutk->rec_type, FILE_SAVE_FULLTIME))
			rec_cmd = CMD_SERVER_START_RECORD;
	} else {
		rec_cmd = CMD_TUTK_NONE;
	}
	while (1) {

		pthread_mutex_lock(&tutk->rec_mutex);
		if (list_empty(&tutk->rec_cmd_list)) {
			req_cmd = CMD_TUTK_NONE;		
		} else {
			l = tutk->rec_cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			req_cmd = cmd_data->session_cmd;
			list_del(&cmd_data->list);
			free(cmd_data);
		}

		if ((req_cmd == CMD_TUTK_QUIT) || (req_cmd == CMD_SERVER_STOP_RECORD) 
			|| ((req_cmd == CMD_SERVER_START_RECORD) && (rec_cmd == CMD_TUTK_NONE))
			|| (req_cmd == CMD_SERVER_RESTART_RECORD)
			|| (req_cmd == CMD_SERVER_CHANGE_SCENE)) {
			i = rec_cmd;
			rec_cmd = req_cmd;
			req_cmd = i;
		}

		if (rec_cmd == CMD_TUTK_NONE) {
			pthread_cond_wait(&tutk->rec_cond, &tutk->rec_mutex);
		} else if (rec_cmd == CMD_SERVER_RECORD_CHECK) {
			gettimeofday(&now_time, NULL);
			now_time.tv_sec += 240;
			timeout_time.tv_sec = now_time.tv_sec;
			timeout_time.tv_nsec = now_time.tv_usec * 1000;
			pthread_cond_timedwait(&tutk->rec_cond, &tutk->rec_mutex, &timeout_time);
		}
		pthread_mutex_unlock(&tutk->rec_mutex);

		if (rec_cmd == CMD_TUTK_QUIT) {
			LOGD("tutk %s record thread quit \n", tutk->rec_type);
			break;
		}

		switch (rec_cmd) {
			case CMD_SERVER_START_RECORD:
				LOGD("tutk %s record thread start \n", tutk->rec_type);
				if (tutk->tf_card_state && (access(tutk->sd_mount_point, F_OK) == 0) && (access(tutk->rec_path, F_OK) != 0))
					mkdir(tutk->rec_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if ((access(tutk->rec_path, F_OK) != 0) || (!tutk->tf_card_state)) {
					rec_cmd = CMD_TUTK_NONE;
					break;
				}

				if (tutk->tutk_rec == NULL) {
					rec_cmd = CMD_TUTK_NONE;
				} else {
					if (!rec_start) {
						if (strcmp(tutk->rec_type, FILE_SAVE_ALARM)) {
							video_enable_channel(tutk->rec_video_channel);
							video_set_bitrate(tutk->rec_video_channel, &record_scene_info);
						}
						vplay_start_recorder(tutk->tutk_rec);
						rec_start = 1;
					}
					rec_cmd = CMD_SERVER_RECORD_CHECK;
				}
				LOGD("tutk %s record thread start vplay recorder \n", tutk->rec_type);
				break;
			case CMD_SERVER_RESTART_RECORD:
				if (tutk->tutk_rec) {
					if (rec_start) {
						vplay_stop_recorder(tutk->tutk_rec);
						if (strcmp(tutk->rec_type, FILE_SAVE_ALARM))
							video_disable_channel(tutk->rec_video_channel);
						rec_start = 0;
						rec_cmd = CMD_SERVER_START_RECORD;
					} else {
						rec_cmd = CMD_TUTK_NONE;
					}
					tutk_record_deinit(tutk);
					tutk_record_init(tutk);
				} else {
					rec_cmd = CMD_TUTK_NONE;
				}
				LOGD("tutk %s record thread restart \n", tutk->rec_type);
				break;
			case CMD_SERVER_STOP_RECORD:
				if (rec_start) {
					vplay_stop_recorder(tutk->tutk_rec);
					if (strcmp(tutk->rec_type, FILE_SAVE_ALARM))
						video_disable_channel(tutk->rec_video_channel);
					rec_start = 0;
				}
				rec_cmd = CMD_TUTK_NONE;
				LOGD("tutk %s record thread stop \n", tutk->rec_type);
				break;
			case CMD_SERVER_CHANGE_SCENE:
				if (tutk->tutk_rec) {
					tutk_video_change_scene(tutk, tutk->rec_video_channel, tutk->scene_mode);
					rec_cmd = req_cmd;
				} else {
					rec_cmd = CMD_TUTK_NONE;
				}
				LOGD("tutk %s record thread scene change %d \n", tutk->rec_type, tutk->scene_mode);
				break;
			case CMD_SERVER_RECORD_CHECK:
				free_size = 0;
				i = 0;
				while ((free_size < TUTK_DEL_FILE_THRESHHOLD) && tutk->tf_card_state && (i < 5)) {
					system_get_storage_info(tutk->sd_mount_point, &si);
					total_size = si.total;
					total_size >>= 10;
					free_size = si.free;
					free_size >>= 10;

					if (free_size < TUTK_DEL_FILE_THRESHHOLD) {
						LOGD("tutk %s record thread not enough storage size total: %d free:%d \n", tutk->rec_type, total_size, free_size);
						tutk_record_del_old(tutk);
					}
					i++;
					usleep(20000);
				}
				break;
			default:
				break;
		}
	}

	pthread_mutex_lock(&tutk->rec_mutex);
	list_for_each_safe(l, n, &tutk->rec_cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	pthread_mutex_unlock(&tutk->rec_mutex);
	pthread_exit(0);
}

static void *tutk_playback_thread(void *arg)
{
	struct tutk_session_t *tutk_session = (struct tutk_session_t *)arg;
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct cmd_data_t *cmd_data = NULL;
	struct list_head_t *l, *n;
	DIR *dir;
	struct dirent *ptr;
	SMsgAVIoctrlPlayRecordResp play_rec_resp;
	struct timeval internel_video_time, latest_video_time, internel_audio_time, latest_audio_time;
	struct demux_t *demux = NULL;
	struct demux_frame_t demux_frame = {0};
	struct codec_info send_info;
	struct codec_addr send_addr;
	void *codec = NULL;
	char enc_buf[AUDIO_BUF_SIZE * 4] = {0};
	FRAMEINFO_t video_info, audio_info;
    int ret, head_len = 0, send_len = 0, frame_flag = VIDEO_FRAME_P, start_timestamp = 0, offset = 0, temp_len = 0;
    int video_frame_cnt = 0, audio_frame_cnt = 0, latest_video_frame_cnt = 0, latest_audio_frame_cnt = 0, frame_rate, play_delay = 20;
    int play_cmd = CMD_SESSION_NONE, req_cmd = CMD_SESSION_NONE, pause_cmd = CMD_SESSION_NONE;
    int ms_timestamp = 0, file_fps = 30;
    char play_file[96] = {0}, header[128];
    char *video_buf = tutk_session->video_buf, *send_buf = NULL;
	int length;

	snprintf(header, 16, "py_back%s", tutk_session->dev_name + strlen("device"));
	prctl(PR_SET_NAME, header);
	tutk_session->thread_run_flag |= (1 << PLAYBACK_THREAD_RUN);
	sem_post(&tutk->thread_sem);
	memset(video_buf, 0, VIDEO_BUF_SIZE);
	while (tutk_session->process_run) {

		pthread_mutex_lock(&tutk_session->play_mutex);
		if (!list_empty(&tutk_session->playback_cmd_list)) {
			l = tutk_session->playback_cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			req_cmd = cmd_data->session_cmd;
			list_del(&cmd_data->list);
			free(cmd_data);
		} else {
			req_cmd = CMD_SESSION_NONE;
		}

		if (play_cmd == CMD_SESSION_PAUSE_VIDEO) {
			if (req_cmd == CMD_SESSION_NONE) {
				pthread_cond_wait(&tutk_session->play_cond, &tutk_session->play_mutex);
				//LOGD("play back pause cmd wake  \n");
				pthread_mutex_unlock(&tutk_session->play_mutex);
				continue;
			} else if (req_cmd == CMD_SESSION_PAUSE_VIDEO) {
				play_cmd = pause_cmd;
				pause_cmd = CMD_SESSION_NONE;
			} else if (req_cmd == CMD_SESSION_START_VIDEO) {
				play_cmd = CMD_SESSION_RESTART_VIDEO;
				pause_cmd = CMD_SESSION_NONE;
			} else {
				play_cmd = req_cmd;
				pause_cmd = CMD_SESSION_NONE;
			}
		} else if ((req_cmd == CMD_SESSION_QUIT) || (req_cmd == CMD_SESSION_STOP_VIDEO) 
			|| ((req_cmd == CMD_SESSION_START_VIDEO) && (play_cmd == CMD_SESSION_NONE))
			|| (req_cmd == CMD_SESSION_PAUSE_VIDEO)) {
			if (req_cmd == CMD_SESSION_PAUSE_VIDEO)
				pause_cmd = play_cmd;
			play_cmd = req_cmd;
		}

		if (play_cmd == CMD_SESSION_NONE) {
			pthread_cond_wait(&tutk_session->play_cond, &tutk_session->play_mutex);
			pthread_mutex_unlock(&tutk_session->play_mutex);
			continue;
		} else {
			pthread_mutex_unlock(&tutk_session->play_mutex);
		}

		if (play_cmd == CMD_SESSION_QUIT) {
			if (demux) {
				demux_deinit(demux);
				demux = NULL;
			}
			if (codec) {
				do {
					ret = codec_flush(codec, &send_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				codec_close(codec);
				codec = NULL;
			}
			LOGD("tutk %s playback thread quit \n", tutk_session->dev_name);
			break;
		}

		switch (play_cmd) {
			case CMD_SESSION_START_VIDEO:
				if (strncmp(tutk_session->play_file, play_file, 96)) {
					dir = opendir(tutk->rec_path);
					if (dir == NULL) {
						play_cmd = CMD_SESSION_NONE;
						LOGD("tutk %s playback thread dir null \n", tutk_session->dev_name);
						break;
					}
					ret = 0;
					while ((ptr = readdir(dir)) != NULL) {
						if (strstr(ptr->d_name, tutk_session->play_file) != NULL) {
							ret = 1;
							sprintf(play_file, "%s/%s", tutk->rec_path, ptr->d_name);
							break;
						}
					}
					closedir(dir);
					if (!ret) {
						ERROR("not valid play file %s \n", tutk_session->play_file);
						play_cmd = CMD_SESSION_NONE;
						break;
					}

					if (access(play_file, F_OK) != 0) {
						play_cmd = CMD_SESSION_NONE;
						break;
					}
					demux = demux_init(play_file);
					head_len = demux_get_head(demux, header, 128);
					memcpy(video_buf, header, head_len);
				}

				memset(&video_info, 0, sizeof(FRAMEINFO_t));
				memset(&audio_info, 0, sizeof(FRAMEINFO_t));
				gettimeofday(&latest_video_time, NULL);
				gettimeofday(&latest_audio_time, NULL);
				play_cmd = CMD_SESSION_GET_FRAME;
				LOGD("%s video_playback_thread %s start %d \n", tutk_session->dev_name, play_file, head_len);
				break;
			case CMD_SESSION_GET_FRAME:
				ret = demux_get_frame(demux, &demux_frame);
				if (ret < 0) {
					play_cmd = CMD_SESSION_STOP_VIDEO;
				} else {
					if ((demux_frame.codec_id == DEMUX_VIDEO_H265) || (demux_frame.codec_id == DEMUX_VIDEO_H264)) {
						if (demux_frame.codec_id == DEMUX_VIDEO_H264)
							video_info.codec_id = MEDIA_CODEC_VIDEO_H264;
						else
							video_info.codec_id = MEDIA_CODEC_VIDEO_H265;
						if (demux_frame.data_size > (VIDEO_BUF_SIZE - head_len)) {
							LOGD("demux_get_frame big %d \n", demux_frame.data_size);
							if (demux_frame.flags == VIDEO_FRAME_I) 
								frame_flag = VIDEO_FRAME_P;
							demux_put_frame(demux, &demux_frame);
							break;
						}
						if (demux_frame.flags == VIDEO_FRAME_I) {
							video_info.flags = IPC_FRAME_FLAG_IFRAME;
							frame_flag = VIDEO_FRAME_I;
							memcpy(video_buf + head_len, demux_frame.data, demux_frame.data_size);
							send_buf = video_buf;
							send_len = head_len + demux_frame.data_size;
						} else {
							if (frame_flag == VIDEO_FRAME_P) {
								demux_put_frame(demux, &demux_frame);
								break;
							}
							video_info.flags = IPC_FRAME_FLAG_PBFRAME;
							send_buf = (char *)demux_frame.data;
							send_len = demux_frame.data_size;
						}
						video_info.timestamp = demux_frame.timestamp;
						play_cmd = CMD_SESSION_SEND_VIDEO;
					} else {
						if ((demux_frame.codec_id == DEMUX_AUDIO_PCM_ALAW) || (demux_frame.sample_rate == 8000)) {
							send_buf = (char *)demux_frame.data;
							send_len = demux_frame.data_size;
						} else {
							if (!codec) {
								send_info.in.codec_type = AUDIO_CODEC_PCM;
								send_info.in.channel = demux_frame.channels;
								send_info.in.sample_rate = demux_frame.sample_rate;
								send_info.in.bitwidth = demux_frame.bitwidth;
								send_info.in.bit_rate = send_info.in.channel * send_info.in.sample_rate \
														* GET_BITWIDTH(send_info.in.bitwidth);
								send_info.out.codec_type = AUDIO_CODEC_G711A;
								send_info.out.channel = 1;
								send_info.out.sample_rate = 8000;
								send_info.out.bitwidth = 16;
								send_info.out.bit_rate = send_info.out.channel * send_info.out.sample_rate \
														* GET_BITWIDTH(send_info.out.bitwidth);
								codec = codec_open(&send_info);
								if (!codec) {
									ERROR("audio play open codec encoder failed %x \n",  send_info.out.codec_type);
									break;
								}
							}

							send_addr.in = demux_frame.data;
							send_addr.len_in = demux_frame.data_size;
							send_addr.out = enc_buf;
							send_addr.len_out = AUDIO_BUF_SIZE * 4;
							ret = codec_encode(codec, &send_addr);
							if (ret < 0) {
								ERROR("audio play codec decoder failed %d %x \n",  ret, audio_info.codec_id);
							}
							send_buf = enc_buf;
							send_len = ret;
						}
						audio_info.codec_id = MEDIA_CODEC_AUDIO_G711A;
						if (demux_frame.sample_rate == 16000)
							audio_info.flags = ((AUDIO_SAMPLE_16K << 2) | (AUDIO_DATABITS_16 << 1) | AUDIO_CHANNEL_MONO);
						else
							audio_info.flags = ((AUDIO_SAMPLE_8K << 2) | (AUDIO_DATABITS_16 << 1) | AUDIO_CHANNEL_MONO);
						audio_info.timestamp = demux_frame.timestamp;
						play_cmd = CMD_SESSION_SEND_AUDIO;
					}
				}
				break;
			case CMD_SESSION_PAUSE_VIDEO:
				play_rec_resp.command = AVIOCTRL_RECORD_PLAY_PAUSE;
				play_rec_resp.result = 0;
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_RECORD_PLAYCONTROL_RESP, (char *)&play_rec_resp, sizeof(SMsgAVIoctrlPlayRecordResp));
				sprintf(play_file, tutk_session->play_file);
				break;
			case CMD_SESSION_STOP_VIDEO:
				play_rec_resp.command = AVIOCTRL_RECORD_PLAY_END;
				play_rec_resp.result = 0;
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_RECORD_PLAYCONTROL_RESP, (char *)&play_rec_resp, sizeof(SMsgAVIoctrlPlayRecordResp));
				if (demux) {
					demux_deinit(demux);
					demux = NULL;
				}
				if (codec) {
					do {
						ret = codec_flush(codec, &send_addr, &length);
						if (ret == CODEC_FLUSH_ERR)
							break;

						/* TODO you need least data or not ?*/
					} while (ret == CODEC_FLUSH_AGAIN);
					codec_close(codec);
					codec = NULL;
				}
				video_frame_cnt = 0;
				audio_frame_cnt = 0;
				latest_video_frame_cnt = 0;
				latest_audio_frame_cnt = 0;
				start_timestamp = 0;
				play_delay = 0;
				frame_flag = VIDEO_FRAME_P;
				memset(play_file, 0, 96);
				pause_cmd = CMD_SESSION_NONE;
				play_cmd = CMD_SESSION_NONE;
				LOGD("%s video_playback_thread %s stop %d \n", tutk_session->dev_name, tutk_session->play_file, head_len);
				break;
			case CMD_SESSION_RESTART_VIDEO:
				if (demux) {
					demux_deinit(demux);
					demux = NULL;
				}
				if (codec) {
					do {
						ret = codec_flush(codec, &send_addr, &length);
						if (ret == CODEC_FLUSH_ERR)
							break;

						/* TODO you need least data or not ?*/
					} while (ret == CODEC_FLUSH_AGAIN);
					codec_close(codec);
					codec = NULL;
				}
				video_frame_cnt = 0;
				audio_frame_cnt = 0;
				latest_video_frame_cnt = 0;
				latest_audio_frame_cnt = 0;
				start_timestamp = 0;
				play_delay = 0;
				frame_flag = VIDEO_FRAME_P;
				memset(play_file, 0, 96);
				play_cmd = CMD_SESSION_START_VIDEO;
				LOGD("%s video_playback_thread %s restart %d \n", tutk_session->dev_name, tutk_session->play_file, head_len);
				break;
			case CMD_SESSION_SEND_VIDEO:
				ret = avSendFrameData(tutk_session->play_channel_id, send_buf, send_len, &video_info, sizeof(FRAMEINFO_t));
				if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE) || (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
					|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
					ERROR("client had closed so stop playback video send %d \n",  ret);
					play_cmd = CMD_SESSION_STOP_VIDEO;
				} else if (ret == AV_ER_EXCEED_MAX_SIZE) {
					ERROR("video frame send tutk buf full %d \n",  ret);
					usleep(200000);
					play_cmd = CMD_SESSION_GET_FRAME;
				} else if (ret < 0) {
					ERROR("video frame send failed %d \n",  ret);
					play_cmd = CMD_SESSION_STOP_VIDEO;
				} else {
					video_frame_cnt++;
					gettimeofday(&internel_video_time, NULL);
					if ((internel_video_time.tv_sec - latest_video_time.tv_sec) >= FPS_PRINT_INTERNAL_TIME) {
						frame_rate = (video_frame_cnt - latest_video_frame_cnt) / FPS_PRINT_INTERNAL_TIME;
						printf("tutk video %s had send %d frame to %s fps is %d timestamp %d \n", tutk_session->play_file, video_frame_cnt, tutk_session->dev_name, frame_rate, video_info.timestamp);
						gettimeofday(&latest_video_time, NULL);
						latest_video_frame_cnt = video_frame_cnt;
					}
					play_cmd = CMD_SESSION_GET_FRAME;
				}
				demux_put_frame(demux, &demux_frame);
				usleep((play_delay / 2) * 1000);
				break;
			case CMD_SESSION_SEND_AUDIO:
				offset = 0;
				temp_len = 0;
				do {
					temp_len = ((send_len - offset) >= AUDIO_BUF_SIZE) ? AUDIO_BUF_SIZE : (send_len - offset);
					ret = avSendAudioData(tutk_session->play_channel_id, send_buf + offset, temp_len, &audio_info, sizeof(FRAMEINFO_t));
					if ((ret == AV_ER_SESSION_CLOSE_BY_REMOTE) || (ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT) 
						|| (ret == IOTC_ER_INVALID_SID) || (ret == AV_ER_CLIENT_NO_AVLOGIN)) {
						ERROR("play had closed so stop playback audio send %d \n",  ret);
						play_cmd = CMD_SESSION_STOP_VIDEO;
					} else if (ret == AV_ER_EXCEED_MAX_SIZE) {
						ERROR("play frame send tutk buf full %d \n",  ret);
						usleep(20000);
						play_cmd = CMD_SESSION_GET_FRAME;
					} else if (ret < 0) {
						ERROR("play frame send failed %d \n",  ret);
						play_cmd = CMD_SESSION_STOP_VIDEO;
					} else {
						audio_frame_cnt++;
						if (start_timestamp == 0) {
							start_timestamp = audio_info.timestamp;
							file_fps = (8000 / send_len);
							play_delay = 1000 / file_fps;
							if (play_delay > 10)
								play_delay -= 10;
						}
						gettimeofday(&internel_audio_time, NULL);
						if ((internel_audio_time.tv_sec - latest_audio_time.tv_sec) >= FPS_PRINT_INTERNAL_TIME) {

							frame_rate = (audio_frame_cnt - latest_audio_frame_cnt) / FPS_PRINT_INTERNAL_TIME;
							ms_timestamp = (internel_audio_time.tv_sec - latest_audio_time.tv_sec) * 1000;
							if (internel_audio_time.tv_usec >= latest_audio_time.tv_usec) {
								ms_timestamp += (internel_audio_time.tv_usec - latest_audio_time.tv_usec) / 1000;
							} else {
								ms_timestamp -= (latest_audio_time.tv_usec - internel_audio_time.tv_usec) / 1000;
							}
							start_timestamp = (audio_info.timestamp - start_timestamp);
							if (start_timestamp > ms_timestamp) {
								start_timestamp = (start_timestamp - ms_timestamp) / FPS_PRINT_INTERNAL_TIME;
								play_delay += (start_timestamp / file_fps);
							} else {
								start_timestamp = (ms_timestamp - start_timestamp) / FPS_PRINT_INTERNAL_TIME;
								play_delay -= (start_timestamp / file_fps);
							}

							LOGD("tutk audio %s had send %d frame to %s fps is %d timestamp %d next delay %d \n", tutk_session->play_file, audio_frame_cnt, tutk_session->dev_name, frame_rate, audio_info.timestamp, play_delay);
							gettimeofday(&latest_audio_time, NULL);
							latest_audio_frame_cnt = audio_frame_cnt;
							start_timestamp = audio_info.timestamp;
							if (play_delay > 4)
								play_delay -= 4;
						}
						play_cmd = CMD_SESSION_GET_FRAME;
					}
					offset += temp_len;
				} while (offset < send_len);
				demux_put_frame(demux, &demux_frame);
				usleep((play_delay / 2) * 1000);
				break;
			default:
				break;
		}
	}

	list_for_each_safe(l, n, &tutk_session->playback_cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	tutk_session->thread_run_flag &= ~(1 << PLAYBACK_THREAD_RUN);
	pthread_exit(0);
}

static int recv_message(int socketfd, char * buff, int buffLen, int is_tcp,
                       char* from_ip, unsigned int* from_port, unsigned int timeout_secs)
{
	struct sockaddr_in client_addr = {0};
	struct timeval tv;
	fd_set rfds;
	int ret = 0;

	memset(buff, 0, buffLen);
	FD_ZERO(&rfds);
	FD_SET(socketfd, &rfds);
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = timeout_secs;
	tv.tv_usec = 0;

	ret = select(socketfd + 1, &rfds, NULL, NULL, &tv);
	if (ret == 0) {
        return -1;
	} else if (ret < 0) {
		ERROR("ERROR: ret: %d, errno: [ %s]",  ret,  strerror(errno));
        return -1;
    }

	if (FD_ISSET(socketfd, &rfds)) {
		if (is_tcp) {
			ret = read(socketfd, buff, buffLen);
		} else {
            socklen_t addrLen = sizeof(struct sockaddr_in);
            ret = recvfrom(socketfd, buff, buffLen, 0, (struct sockaddr *)&client_addr, &addrLen);
            if (from_ip)
                strcpy(from_ip, (char*)inet_ntoa(client_addr.sin_addr));
            if (from_port)
                *from_port = ntohs(client_addr.sin_port);
        }
    }

    return ret;
}

static void *tutk_wifi_ap_listen_thread(void *arg)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)arg;
	struct net_info_t ap_info;
	struct wifi_config_t wifi_config;
    unsigned short listen_port = 8000;
    int ap_socket_fd = -1, ret;
    struct sockaddr_in localaddr = {0};
    char client_ip[64] = {0};
    char buf[RECV_BUF_SIZE];
    char ssid[64];
    char psk[16];
    unsigned int client_port = 0;

	//need get from wifi lib
	wifi_get_net_info(&ap_info);
	//sprintf(ap_info.ipaddr, "192.168.0.1");

    ap_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ap_socket_fd < 0)
    	goto exit;

	bzero(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(listen_port);
	if (inet_pton(AF_INET, ap_info.ipaddr, &localaddr.sin_addr) < 0) {
        ERROR("ERROR inet_pton: errono[%s]", strerror(errno));
		goto exit;
    }

	if (bind(ap_socket_fd, (struct sockaddr *) &localaddr, sizeof(localaddr)) < 0) {
        ERROR("bind error port:%d error:[%s]", listen_port, strerror(errno));
		goto exit;
    }

	while (tutk->process_run) {
		ret = recv_message(ap_socket_fd, buf, RECV_BUF_SIZE - 1, 0, client_ip, &client_port, 1);
		if (ret < 0) {
			ERROR("wait recv ssid from dev \n");
		} else {
			LOGD("recv wifi ssid %s \n", buf);
			close(ap_socket_fd);
			wifi_ap_disable();
			wifi_sta_enable();
			sprintf(wifi_config.ssid, ssid);
			sprintf(wifi_config.psk, psk);
			wifi_sta_connect(&wifi_config);
			tutk->save_config(NULL, WIFI_AP_ENABLE, "0");
			tutk->wifi_ap_set = 0;
			tutk->save_config(NULL, WIFI_STA_ENABLE, "1");
			tutk->wifi_sta_set = 1;
			break;
		}
		sleep(2);
    }

exit:
	pthread_exit(0);
}

static void *tutk_server_register_thread(void *arg)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)arg;
	int ret, cnt = 0;
	char reg_uid[32], reg_pwd[32];

	pthread_mutex_lock(&tutk->register_mutex);
	tutk->register_start = 1;
	while (1) {
		if (tutk->reset_flag || !tutk->wifi_sta_connected)
			break;
		ret = tutk->get_register_result(tutk, reg_uid, reg_pwd);
		if (ret == -1) {
			ERROR("stop server register for disconnect or new connect \n");
			break;
		} else if (ret) {
			ERROR("server register faile %d \n", ret);
			sleep(1);
			cnt++;
			if (cnt >= 5)
				break;
		} else {
			if (!tutk->reset_flag) {
				if (tutk->process_run)
					tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_DISABLE_SERVER, 0);
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_ENABLE_SERVER, 0);
				if (tutk->event_report)
					tutk->event_report(tutk, EVENT_FILELIST, NULL);
			}
			LOGD("tutk server register complete %d uid:%s pwd:%s\n", ret, reg_uid, reg_pwd);
			break;
		}
	}

	tutk->register_start = 0;
	pthread_mutex_unlock(&tutk->register_mutex);
	pthread_exit(0);
}

static int tutk_p2p_auth_cb(char *account, char *pwd)
{
	struct tutk_common_t *tutk = tutk_save;
	struct tutk_session_t *tutk_session = tutk->play_session;
	int ret = 0;

	LOGD("tutk_p2p_auth_cb %s %s \n", account, pwd);
	if (((strcmp(account, tutk->local_acc) == 0) && (strcmp(pwd, tutk->local_pwd) == 0))
		|| ((strcmp(account, tutk->reg_acc) == 0) && (strcmp(pwd, tutk->reg_pwd) == 0))) {
		ret = 1;
		if ((strcmp(account, tutk->local_acc) == 0) && (strcmp(pwd, tutk->local_pwd) == 0)) {
			if (tutk_session)
				tutk_session->local_play = 1;
		} else {
			if (tutk_session)
				tutk_session->local_play = 0;
		}
	}

	return ret;
}

static int tutk_enable_server_session(struct tutk_common_t *tutk, int session_id)
{
	//unsigned long server_type = SERVTYPE_STREAM_SERVER;
	struct tutk_session_t *tutk_session, *tutk_session_temp;
	int server_channel_id = 0, tutk_channel_id = 0, ret, i = 0;
	struct st_SInfo info;
	struct list_head_t *l, *n;

	tutk_session = malloc(sizeof(struct tutk_session_t));
	if (!tutk_session)
		return -1;

	memset(tutk_session, 0, sizeof(struct tutk_session_t));
	tutk_session->tutk_client_channel_id = -1;
	tutk_session->tutk_server_channel_id = -1;
	tutk_session->tutk_play_channel_id = -1;
	tutk_session->play_channel_id = -1;
	ret = IOTC_Session_Check(session_id, &info);
	if (ret != IOTC_ER_NoERROR) {
		ERROR("tutk session id check failed %d \n", session_id);
        IOTC_Session_Close(session_id);
        free(tutk_session);
        return -2;
	}
	LOGD("tutk enable session start %d client %s %d %d:%d:%d\n", session_id, info.RemoteIP, info.RemotePort, info.VID, info.PID, info.GID);

    tutk_session->session_id = session_id;
    tutk_session->tutk_server_channel_id = tutk_channel_id;

	IOTC_Session_Channel_ON(session_id, tutk_channel_id);
	tutk->play_session = tutk_session;
	server_channel_id = avServStart2(session_id, tutk_p2p_auth_cb, TUTK_SERVER_TIMEOUT, SERVTYPE_STREAM_SERVER, tutk_channel_id);
	tutk->play_session = NULL;;
	if (server_channel_id < 0) {
		ERROR("tutk av channel start failed av:%d free:%d \n", server_channel_id, tutk_channel_id);
        IOTC_Session_Close(session_id);
        free(tutk_session);
        return -2;
    }
    sprintf(tutk_session->video_channel, DEFAULT_PREVIEW_CHANNEL);
    for (i=0; i<10; i++) {
    	ret = 0;
	    list_for_each_safe(l, n, &tutk->session_list) {
	    	tutk_session_temp = container_of(l, struct tutk_session_t, list);
	    	if (!strcmp(tutk_session_temp->dev_name, device_name[i])) {
	    		ret = 1;
	    		break;
	    	}
	    }
	    if (!ret)
	    	break;
	}
    snprintf(tutk_session->dev_name, sizeof(tutk_session->dev_name), "%s", device_name[i]);
    tutk_session->server_channel_id = server_channel_id;

	tutk_session->process_run = 1;
	tutk_session->priv = (void *)tutk;
	tutk_session->video_buf = (char *)malloc(VIDEO_BUF_SIZE);
	if (tutk_session->video_buf == NULL) {
		ERROR("video thread malloc failed \n");
        free(tutk_session);
        return -1;
	}
	memset(tutk_session->video_buf, 0, VIDEO_BUF_SIZE);
	INIT_LIST_HEAD(&tutk_session->video_send_cmd_list);
	tutk_session->video_quality = AVIOCTRL_QUALITY_MIDDLE;
	pthread_mutex_init(&tutk_session->video_mutex, NULL);
	pthread_cond_init(&tutk_session->video_cond, NULL);
	ret = pthread_create(&tutk_session->video_thread, NULL, &tutk_video_send_thread, (void *)tutk_session);
	if (ret) {
        ERROR("video thread creat failed %d \n", ret);
        free(tutk_session);
        return -1;
    }
    sem_wait(&tutk->thread_sem);

	sprintf(tutk_session->audio_send_channel, tutk->rec_audio_channel);
	INIT_LIST_HEAD(&tutk_session->audio_send_cmd_list);
	pthread_mutex_init(&tutk_session->audio_mutex, NULL);
	pthread_cond_init(&tutk_session->audio_cond, NULL);
	ret = pthread_create(&tutk_session->audio_thread, NULL, &tutk_audio_send_thread, (void *)tutk_session);
	if (ret) {
        ERROR("audio thread creat failed %d \n", ret);
        free(tutk_session);
        return -1;
    }
    sem_wait(&tutk->thread_sem);

	sprintf(tutk_session->audio_recv_channel, tutk->play_audio_channel);
	INIT_LIST_HEAD(&tutk_session->audio_recv_cmd_list);
	pthread_mutex_init(&tutk_session->audio_recv_mutex, NULL);
	pthread_cond_init(&tutk_session->audio_recv_cond, NULL);
	ret = pthread_create(&tutk_session->audio_recv_thread, NULL, &tutk_audio_recv_thread, (void *)tutk_session);
	if (ret) {
        ERROR("audio recv thread creat failed %d \n", ret);
        return -1;
    }
    sem_wait(&tutk->thread_sem);

	pthread_mutex_init(&tutk_session->io_mutex, NULL);
	pthread_cond_init(&tutk_session->io_cond, NULL);
	tutk_session->ioctrl_cmd = CMD_SESSION_NONE;
	ret = pthread_create(&tutk_session->ioctrl_thread, NULL, &tutk_server_ioctrl_thread, (void *)tutk_session);
	if (ret) {
        ERROR("ioctrl thread creat failed %d \n", ret);
        free(tutk_session);
        return -1;
    }
    sem_wait(&tutk->thread_sem);

	list_add_tail(&tutk_session->list, &tutk->session_list);
	tutk->actual_session_num = list_get_length(&tutk->session_list);
	pthread_mutex_lock(&tutk_session->io_mutex);
	tutk_session->ioctrl_cmd = CMD_SESSION_RECV_IOCTRL;
	pthread_cond_signal(&tutk_session->io_cond);
	pthread_mutex_unlock(&tutk_session->io_mutex);
	LOGD("tutk %s enable session complete %d %d %x \n", tutk_session->dev_name, tutk_session->server_channel_id, tutk->actual_session_num, tutk_session->thread_run_flag);
	return 0;
}

static void tutk_disable_server_session(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	void *ioctrl, *audio, *video;

	LOGD("tutk_disable_server_session %s start %d \n", tutk_session->dev_name, tutk_session->session_id);
	if (tutk_session->thread_run_flag & (1 << AUDIO_PLAY_THREAD_RUN)) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_RECV, CMD_SESSION_QUIT, 0);
		pthread_join(tutk_session->audio_recv_thread, &audio);
	}
	pthread_mutex_destroy(&tutk_session->audio_recv_mutex);
	pthread_cond_destroy(&tutk_session->audio_recv_cond);

	if (tutk_session->client_channel_id >= 0) {
		avClientExit(tutk_session->session_id, tutk_session->tutk_client_channel_id);
		avClientStop(tutk_session->client_channel_id);
		tutk_session->client_channel_id = -1;
	}

	if (tutk_session->thread_run_flag & (1 << IOCTRL_THREAD_RUN)) {
		pthread_mutex_lock(&tutk_session->io_mutex);
		tutk_session->ioctrl_cmd = CMD_SESSION_QUIT;
		pthread_cond_signal(&tutk_session->io_cond);
		pthread_mutex_unlock(&tutk_session->io_mutex);
		pthread_join(tutk_session->ioctrl_thread, &ioctrl);
	}
	pthread_mutex_destroy(&tutk_session->io_mutex);
	pthread_cond_destroy(&tutk_session->io_cond);

	if (tutk_session->thread_run_flag & (1 << AUDIO_CAPTURE_THREAD_RUN)) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_SEND, CMD_SESSION_QUIT, 0);
		pthread_join(tutk_session->audio_thread, &audio);
	}
	pthread_mutex_destroy(&tutk_session->audio_mutex);
	pthread_cond_destroy(&tutk_session->audio_cond);

	if (tutk_session->thread_run_flag & (1 << VIDEO_THREAD_RUN)) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_VIDEO_SEND, CMD_SESSION_QUIT, 0);
		pthread_join(tutk_session->video_thread, &video);
	}
	pthread_mutex_destroy(&tutk_session->video_mutex);
	pthread_cond_destroy(&tutk_session->video_cond);
	if (tutk_session->video_buf)
		free(tutk_session->video_buf);
	tutk_session->process_run = 0;

	if (tutk_session->tutk_server_channel_id >= 0) {
		avServExit(tutk_session->session_id, tutk_session->tutk_server_channel_id);
		avServStop(tutk_session->server_channel_id);
		IOTC_Session_Channel_OFF(tutk_session->session_id, tutk_session->tutk_server_channel_id);
	}

	IOTC_Session_Close(tutk_session->session_id);
	list_del(&tutk_session->list);
	tutk->actual_session_num = list_get_length(&tutk->session_list);
	LOGD("tutk_disable_session %s end %d \n", tutk_session->dev_name, tutk->actual_session_num);
	free(tutk_session);

	return;
}

static void *tutk_server_listen_thread(void *arg)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)arg;
    int timout = 2000, session_id;

	tutk->server_start |= (1 << 2);
	while (tutk->process_run) {
		session_id = IOTC_Listen(timout);
		if (session_id < 0) {
			if (session_id != IOTC_ER_TIMEOUT) {
		        ERROR("session IOTC listen failed %d \n", session_id);
		        print_err_handling(session_id);
		    }
	    } else {
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_SESSION, session_id);
		}
	}

	LOGD("tutk_listen_thread end \n");
	pthread_exit(0);
}

int tutk_enable_client(struct tutk_common_t *tutk)
{
	struct st_LanSearchInfo *psLanSearchInfo;
	int dev_num, i;

	psLanSearchInfo = (struct st_LanSearchInfo *)malloc(sizeof(struct st_LanSearchInfo)*8);
	if (psLanSearchInfo == NULL)
		return -1;

	dev_num = IOTC_Lan_Search(psLanSearchInfo, 8, 5000);
	for (i=0; i<dev_num; i++)
		LOGD("UID[%s] Addr[%s:%d]\n", psLanSearchInfo[i].UID, psLanSearchInfo[i].IP, psLanSearchInfo[i].port);
	free(psLanSearchInfo);

	return 0;
}

static int tutk_server_enable_playback(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	int ret;
	if (tutk_session->play_channel_id >= 0)
		return 0;

	IOTC_Session_Channel_ON(tutk_session->session_id, tutk_session->tutk_play_channel_id);
	tutk_session->play_channel_id = avServStart2(tutk_session->session_id, tutk_p2p_auth_cb, TUTK_SERVER_TIMEOUT, SERVTYPE_STREAM_SERVER, tutk_session->tutk_play_channel_id);
	if (tutk_session->play_channel_id < 0) {
		ERROR("tutk play start failed %d %d \n", tutk_session->play_channel_id, tutk_session->tutk_play_channel_id);
		return tutk_session->play_channel_id;
	}

	pthread_mutex_init(&tutk_session->play_mutex, NULL);
	pthread_cond_init(&tutk_session->play_cond, NULL);
	INIT_LIST_HEAD(&tutk_session->playback_cmd_list);
	ret = pthread_create(&tutk_session->play_thread, NULL, &tutk_playback_thread, (void *)tutk_session);
	if (ret) {
        ERROR("playback thread creat failed %d \n", ret);
        free(tutk_session);
        return -1;
    }
    sem_wait(&tutk->thread_sem);

    LOGD("tutk %s server playback %d \n", tutk_session->dev_name, tutk_session->play_channel_id);
	return 0;
}

static void tutk_server_disable_playback(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	void *play_arg;

	if (tutk_session->thread_run_flag & (1 << PLAYBACK_THREAD_RUN)) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_PLAYBACK, CMD_SESSION_QUIT, 0);
		pthread_join(tutk_session->play_thread, &play_arg);
		tutk_session->play_thread = 0;
		tutk_session->thread_run_flag &= ~(1 << PLAYBACK_THREAD_RUN);
	}

	if (tutk_session->play_channel_id >= 0) {
		avServExit(tutk_session->session_id, tutk_session->tutk_play_channel_id);
		avServStop(tutk_session->play_channel_id);
		tutk_session->play_channel_id = -1;
	}

	if (tutk_session->tutk_play_channel_id >= 0) {
		IOTC_Session_Channel_OFF(tutk_session->session_id, tutk_session->tutk_play_channel_id);
		tutk_session->tutk_play_channel_id = -1;
	}

	return;
}

static int tutk_server_enable_client(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	unsigned long server_type = SERVTYPE_STREAM_SERVER;
	int tutk_channel_id;

	tutk_channel_id = IOTC_Session_Get_Free_Channel(tutk_session->session_id);
	if (tutk_channel_id < 0) {
		ERROR("tutk have not free channel %d \n", tutk_channel_id);
		return tutk_channel_id;
	}

	tutk_session->tutk_client_channel_id = tutk_channel_id;
	IOTC_Session_Channel_ON(tutk_session->session_id, tutk_channel_id);
	tutk_session->client_channel_id = avClientStart(tutk_session->session_id, NULL, NULL, 2, &server_type, tutk_channel_id);
	if (tutk_session->client_channel_id < 0) {
		ERROR("tutk client start failed %d %d \n", tutk_session->client_channel_id, tutk_channel_id);
		return tutk_session->client_channel_id;
	}
	avClientCleanAudioBuf(tutk_session->client_channel_id);

	return 0;
}

static void tutk_server_disable_client(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	if (tutk_session->client_channel_id >= 0) {
		avClientCleanAudioBuf(tutk_session->client_channel_id);
		avClientExit(tutk_session->session_id, tutk_session->tutk_client_channel_id);
		avClientStop(tutk_session->client_channel_id);
		tutk_session->client_channel_id = -1;
	}

	if (tutk_session->tutk_client_channel_id >= 0) {
		IOTC_Session_Channel_OFF(tutk_session->session_id, tutk_session->tutk_client_channel_id);
		tutk_session->tutk_client_channel_id = -1;
	}

	return;
}

int tutk_enable_server(struct tutk_common_t *tutk)
{
	int ret = 0;

	tutk->process_run = 1;
	tutk->server_start = 1;
	IOTC_Set_Max_Session_Number(MAX_CLIENT_NUMBER);
	ret = IOTC_Initialize2(0);
	if(ret != IOTC_ER_NoERROR) {
		ERROR("IOTC_Initialize failed %d \n", ret);
		print_err_handling(ret);
		tutk->process_run = 0;
		tutk->server_start = 0;
		return 0;
    }

	/*ret = RDT_Initialize();
	if (ret <= 0) {
        ERROR("RDT_Initialize error!!\n");
		tutk->process_run = 0;
		tutk->server_start = 0;
        return 0;
	}*/

	//IOTC_Get_Version(&iotc_ver);
	//av_ver = avGetAVApiVer();

	avInitialize(MAX_CLIENT_NUMBER*3);
	ret = pthread_create(&tutk->server_login_thread, NULL, &tutk_server_login_thread, (void *)tutk);
	if (ret) {
        ERROR("login thread creat failed %d \n", ret);
        return -1;
    }

	ret = pthread_create(&tutk->server_listen_thread, NULL, &tutk_server_listen_thread, (void *)tutk);
	if (ret) {
        ERROR("listen thread creat failed %d \n", ret);
        return -1;
    }
    ret = 0;
	while (((tutk->server_start & 0x7) != 0x7) && (ret++ < 50)) {
		usleep(20000);
	}

    return 0;
}

static void tutk_disable_server(struct tutk_common_t *tutk)
{
	struct tutk_session_t *tutk_session;
	struct list_head_t *l, *n;
	void *arg_login, *arg_listen;

	LOGD("tutk_disable_server start %d %d \n", tutk->process_run, tutk->actual_session_num);
	if (!tutk->process_run)
		return;
	tutk->process_run = 0;
	if (tutk->server_listen_thread) {
		IOTC_Listen_Exit();
		pthread_join(tutk->server_listen_thread, &arg_login);
	}
	if (tutk->server_login_thread)
		pthread_join(tutk->server_login_thread, &arg_listen);

	list_for_each_safe(l, n, &tutk->session_list) {
		tutk_session = container_of(l, struct tutk_session_t, list);
		tutk_server_disable_playback(tutk, tutk_session);
		tutk_disable_server_session(tutk, tutk_session);
	}

    avDeInitialize();
    //RDT_DeInitialize();
    IOTC_DeInitialize();
    tutk->server_start = 0;
    LOGD("tutk_disable_server end \n");

	return;
}

static void tutk_server_start_stream(struct tutk_common_t *tutk, int type, struct tutk_session_t *tutk_session)
{
	if (type == CMD_SERVER_START_VIDEO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_VIDEO_SEND, CMD_SESSION_START_VIDEO, 0);
	} else if (type == CMD_SERVER_START_PLAYBACK) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_PLAYBACK, CMD_SESSION_START_VIDEO, 0);
	} else if (type == CMD_SERVER_START_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_SEND, CMD_SESSION_START_AUDIO, 0);
	} else if (type == CMD_CLIENT_START_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_RECV, CMD_SESSION_START_AUDIO, 0);
	} else if (type == CMD_SERVER_RESTART_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_SEND, CMD_SESSION_RESTART_AUDIO, 0);
	} else if (type == CMD_CLIENT_RESTART_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_RECV, CMD_SESSION_RESTART_AUDIO, 0);
	}

	return;
}

static void tutk_server_stop_stream(struct tutk_common_t *tutk, int type, struct tutk_session_t *tutk_session)
{
	if (type == CMD_SERVER_STOP_VIDEO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_VIDEO_SEND, CMD_SESSION_STOP_VIDEO, 0);
	} else if (type == CMD_SERVER_STOP_PLAYBACK) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_PLAYBACK, CMD_SESSION_STOP_VIDEO, 0);
	} else if (type == CMD_SERVER_PAUSE_PLAYBACK) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_PLAYBACK, CMD_SESSION_PAUSE_VIDEO, 0);
	} else if (type == CMD_SERVER_STOP_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_SEND, CMD_SESSION_STOP_AUDIO, 0);
	} else if (type == CMD_CLIENT_STOP_AUDIO) {
		tutk_session_add_cmd(tutk_session, CMD_SESSION_AUDIO_RECV, CMD_SESSION_STOP_AUDIO, 0);
	}

	return;
}

static void tutk_server_set_resolution(struct tutk_common_t *tutk, int type, struct tutk_session_t *tutk_session)
{
	tutk_session_add_cmd(tutk_session, CMD_SESSION_VIDEO_SEND, CMD_SESSION_SET_RESOLUTION, 0);

	return;
}

static void tutk_server_change_scene(struct tutk_common_t *tutk, int type, struct tutk_session_t *tutk_session)
{
	tutk_session_add_cmd(tutk_session, CMD_SESSION_VIDEO_SEND, CMD_SESSION_CHANGE_SCENE, 0);

	return;
}

static void tutk_wifi_state_handle(char *event, void *arg)
{
	struct tutk_common_t *tutk = tutk_save;
	struct wifi_config_t *wifi_config_connect;
	char ssid[32], pwd[32];
	int ret;

	LOGD("wifi_state handle  %s %d \n", (char *)arg, tutk->wifi_sta_connected);
	if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {

		if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
			ret = pthread_create(&tutk->wifi_ap_thread, NULL, &tutk_wifi_ap_listen_thread, (void *)tutk);
			if (ret) {
		        ERROR("wifi ap thread creat failed %d \n", ret);
		        return;
		    }
		    pthread_detach(tutk->wifi_ap_thread);
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_ENABLE_SERVER, 0);

		} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {

			if (!tutk->wifi_sta_connected) {
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_ENABLE_SERVER, 0);
				tutk->wifi_sta_connected = 1;
			}

		    LOGD("wifi_state changed %s \n", (char *)arg);
		} else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
			if (!tutk->wifi_sta_connected) {
				if (item_exist(DEFAULT_WIFI_SSID) && item_exist(DEFAULT_WIFI_PWD)) {

					item_string(ssid, DEFAULT_WIFI_SSID, 0);
					item_string(pwd, DEFAULT_WIFI_PWD, 0);
					wifi_config_connect = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
					if (!wifi_config_connect)
						return;

					sprintf(wifi_config_connect->ssid, ssid);
					sprintf(wifi_config_connect->psk, pwd);
					tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_CONNECTE_AP, (unsigned)wifi_config_connect);
				}
			}
		}
	}

	return;
}

static void tutk_common_event_handle(struct tutk_common_t *tutk, char *event, void *arg)
{
	struct event_process *event_process;
	struct font_attr tutk_attr;

	if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
		if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
			if (tutk->net_cmd_flag == WIFI_STA_STATE_SCANED) {
				sem_post(&tutk->wifi_sem);
				tutk->net_cmd_flag = 0;
			}
		} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
			if (tutk->net_cmd_flag == WIFI_STA_STATE_CONNECTED) {
				sem_post(&tutk->wifi_sem);
				tutk->net_cmd_flag = 0;
			}
		}
	} else if (!strncmp(event, EVENT_NTPOK, strlen(EVENT_NTPOK))) {

		memset(&tutk_attr, 0, sizeof(struct font_attr));
		sprintf((char *)tutk_attr.ttf_name, "arial");
		tutk_attr.font_color = 0x00ffffff;
		tutk_attr.back_color = 0x20000000;
		marker_set_mode("marker0", "auto", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);

		if (tutk->tf_card_state) {
			tutk_record_file_list(tutk, 1);
		} else {
			tutk_record_file_list(tutk, 0);
		}
		if (tutk->event_report)
			tutk->event_report(tutk, EVENT_FILELIST, NULL);
	} else if (!strncmp(event, EVENT_MOUNT, strlen(EVENT_MOUNT))) {

		tutk_record_file_list(tutk, 1);
		if (tutk->event_report)
			tutk->event_report(tutk, EVENT_FILELIST, NULL);
	} else if (!strncmp(event, EVENT_UNMOUNT, strlen(EVENT_UNMOUNT))) {

		tutk_record_file_list(tutk, 0);
		if (tutk->event_report)
			tutk->event_report(tutk, EVENT_FILELIST, NULL);
	} else if (!strncmp(event, EVENT_PROCESS_END, strlen(EVENT_PROCESS_END))) {
		event_process = (struct event_process *)arg;
		if (!strncmp(event_process->comm, "play_audio", strlen("play_audio"))) {
			LOGD("audio play complete \n");
		}
	}

	return;
}

static void tutk_common_init(struct tutk_common_t *tutk)
{
	char value[256];

	tutk->get_config(NULL, LOCAL_UID_SETTING, value);
	if (value[0] == '\0') {
		sprintf(tutk->local_uid, LOCAL_UID_NAME);
		sprintf(value, LOCAL_UID_NAME);
		tutk->save_config(NULL, LOCAL_UID_SETTING, value);
	}
	tutk->get_config(NULL, LOCAL_ACC_SETTING, value);
	if (value[0] == '\0') {
		sprintf(tutk->local_acc, LOCAL_ACC_NAME);
		sprintf(value, LOCAL_ACC_NAME);
		tutk->save_config(NULL, LOCAL_ACC_SETTING, value);
	}
	tutk->get_config(NULL, LOCAL_PWD_SETTING, value);
	if (value[0] == '\0') {
		sprintf(tutk->local_pwd, LOCAL_PWD_VALUE);
		sprintf(value, LOCAL_PWD_VALUE);
		tutk->save_config(NULL, LOCAL_PWD_SETTING, value);
	}
	tutk->get_config(NULL, "AP_ENABLE", value);
	tutk->wifi_ap_set = atoi(value);
	tutk->get_config(NULL, "STA_ENABLE", value);
	tutk->wifi_sta_set = atoi(value);
	tutk->get_config(NULL, "REG_STATE", value);
	tutk->reg_state = atoi(value);
	tutk->get_config(NULL, TUTK_GMT, value);
	if (value[0] == '\0') {
		sprintf(value, "%d", TUTK_DEFAULT_GMT);
		tutk->save_config(NULL, TUTK_GMT, value);
		tutk->save_config(NULL, TUTK_TIMEZONE, TUTK_DEFAULT_TIMEZONE);
	}

	if (!tutk->wifi_ap_set && !tutk->wifi_sta_set) {
		sprintf(value, "1");
		tutk->save_config(NULL, "STA_ENABLE", value);
		tutk->wifi_sta_set = 1;
	}
	event_register_handler(WIFI_STATE_EVENT, 0, tutk_wifi_state_handle);
	if (tutk->wifi_ap_set) {
		wifi_start_service(tutk_wifi_state_handle);
		wifi_ap_enable();
	} else if (tutk->wifi_sta_set) {
		wifi_start_service(tutk_wifi_state_handle);
		wifi_sta_enable();
	}

	return;
}

static void tutk_common_deinit(struct tutk_common_t *tutk)
{
	if (tutk->wifi_ap_set) {
		wifi_ap_disable();
		wifi_stop_service();
	} else if (tutk->wifi_sta_set) {
		if (tutk->wifi_sta_connected)
			wifi_sta_disconnect(NULL);
		wifi_sta_disable();
		wifi_stop_service();
	}
}

static int tutk_common_get_reg_result(struct tutk_common_t *tutk, char *uid, char *pwd)
{
	return 0;
}

static void *tutk_play_audio_thread(void *arg)
{
	struct tutk_common_t *tutk = tutk_save;
	char *file = (char *)arg;
	audio_fmt_t *play_fmt = &tutk->play_fmt;
	struct demux_frame_t demux_frame = {0};
	struct demux_t *demux = NULL;
	struct codec_info play_info;
	struct codec_addr play_addr;
	int play_handle = -1, ret, len, frame_size, offset;
	char play_buf[AUDIO_BUF_SIZE * 16 * 6];
	void *codec = NULL;
	int length;

	pthread_mutex_lock(&tutk->play_mutex);
	prctl(PR_SET_NAME, "play_audio");
	frame_size = play_fmt->sample_size;
	if (file == NULL) {
		memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
		play_handle = audio_get_channel(tutk->play_audio_channel, play_fmt, CHANNEL_BACKGROUND);
		if (play_handle < 0) {
			ERROR("audio play get %s chanel failed %d \n", tutk->play_audio_channel, play_handle);
		} else {
			if (!strcmp(tutk->play_audio_channel, BT_CODEC_PLAY))
				frame_size = play_fmt->sample_size;
			else
				frame_size = play_fmt->sample_size * 4;
			for (len=0; len<8; len++)
				audio_write_frame(play_handle, play_buf, frame_size);
			usleep(100000);
			audio_put_channel(play_handle);
		}
	} else {
		demux = demux_init(file);
		if (demux) {

			do {
				ret = demux_get_frame(demux, &demux_frame);
				if (((ret < 0) || (demux_frame.data_size == 0)) && (play_handle >= 0)) {
					memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
					if (!strcmp(tutk->play_audio_channel, BT_CODEC_PLAY)) {
						for (len=0; len<16; len++) {
							audio_write_frame(play_handle, play_buf, frame_size);
							usleep(30000);
						}
					} else {
						for (len=0; len<4; len++) {
							audio_write_frame(play_handle, play_buf, frame_size);
							usleep(30000);
						}
					}
					break;
				}
				if ((demux_frame.codec_id == DEMUX_VIDEO_H265) || (demux_frame.codec_id == DEMUX_VIDEO_H264)) {
					demux_put_frame(demux, &demux_frame);
					continue;
				}

				if (play_handle < 0) {
					printf("tutk audio play %s %d %d %d \n", file, play_fmt->channels, play_fmt->samplingrate, play_fmt->bitwidth);
					frame_size = play_fmt->sample_size;
					if (demux_frame.bitwidth == 16)
						frame_size = play_fmt->sample_size * 2;
					if (play_fmt->channels > demux_frame.channels)
						frame_size *= (play_fmt->channels / demux_frame.channels);
					if (play_fmt->samplingrate > demux_frame.sample_rate)
						frame_size *= (play_fmt->samplingrate / demux_frame.sample_rate);
					play_handle = audio_get_channel(tutk->play_audio_channel, play_fmt, CHANNEL_BACKGROUND);
					if (play_handle < 0) {
						ERROR("audio play get %s chanel failed %d \n", tutk->play_audio_channel, play_handle);
						break;
					}
				}

				if (!codec) {
					if (demux_frame.codec_id == DEMUX_AUDIO_PCM_ALAW)
						play_info.in.codec_type = AUDIO_CODEC_G711A;
					else
						play_info.in.codec_type = AUDIO_CODEC_PCM;
					play_info.in.channel = demux_frame.channels;
					play_info.in.sample_rate = demux_frame.sample_rate;
					play_info.in.bitwidth = demux_frame.bitwidth;
					play_info.in.bit_rate = play_info.in.channel * play_info.in.sample_rate \
											* GET_BITWIDTH(play_info.in.bitwidth);
					play_info.out.codec_type = AUDIO_CODEC_PCM;
					play_info.out.channel = play_fmt->channels;
					play_info.out.sample_rate = play_fmt->samplingrate;
					play_info.out.bitwidth = play_fmt->bitwidth;
					play_info.out.bit_rate = play_info.out.channel * play_info.out.sample_rate \
											* GET_BITWIDTH(play_info.out.bitwidth);
					codec = codec_open(&play_info);
					if (!codec) {
						ERROR("audio play open codec decoder failed \n");
						break;
					}
				}
				memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
				play_addr.in = demux_frame.data;
				play_addr.len_in = demux_frame.data_size;
				play_addr.out = play_buf;
				play_addr.len_out = AUDIO_BUF_SIZE * 16 * 6;
				ret = codec_decode(codec, &play_addr);
				demux_put_frame(demux, &demux_frame);
				if (ret < 0) {
					ERROR("audio play codec decoder failed %d \n",  ret);
					break;
				}
				len = ret;
				if (len < frame_size)
					len = frame_size;
				offset = 0;
				do {
					ret = audio_write_frame(play_handle, play_buf + offset, frame_size);
					if (ret < 0)
						ERROR("audio play codec failed %d \n",  ret);
					offset += frame_size;
				} while (offset < len);

				usleep(30000);
			} while (ret >= 0);

			if (codec != NULL) {
				do {
					play_addr.out = play_buf;
					play_addr.len_out = AUDIO_BUF_SIZE * 16 * 6;
					ret = codec_flush(codec, &play_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					if (length > 0) {
						ret = audio_write_frame(play_handle, play_buf, length);
						if (ret < 0)
							ERROR("audio play codec failed %d \n",  ret);
					}
				} while (ret == CODEC_FLUSH_AGAIN);
				codec_close(codec);
				codec = NULL;
			}
			if (play_handle >= 0) {
				usleep(150000);
				audio_put_channel(play_handle);
			}
			demux_deinit(demux);
		}
	}

	if (arg)
		free(arg);
	pthread_mutex_unlock(&tutk->play_mutex);
	pthread_exit(0);
}

static void tutk_common_play_audio(struct tutk_common_t *tutk, char *file)
{
	int ret;
	void *arg = NULL;
	pthread_t play_audio_thread;

	if (!tutk->warning_sound)
		return;

	if (file != NULL) {
		arg = (void *)malloc(256);
		if (file == NULL)
			return;

		sprintf((char *)arg, file);
	}
	ret = pthread_create(&play_audio_thread, NULL, &tutk_play_audio_thread, (void *)arg);
	if (ret)
		ERROR("play_audio_thread creat failed %d \n", ret);
	else
		pthread_detach(play_audio_thread);

	return;
}

static struct tutk_timer_t *tutk_common_run_timer(int interval, int (*handle)(struct tutk_timer_t *tutk_timer, void *arg), void *arg)
{
	struct tutk_common_t *tutk = tutk_save;
	struct tutk_timer_t *tutk_timer;

	tutk_timer = malloc(sizeof(struct tutk_timer_t));
	if (tutk_timer == NULL)
		return NULL;

	pthread_mutex_lock(&tutk->timer_mutex);
	tutk_timer->base_cnt = 0;
	tutk_timer->interval = interval / TUTK_BASIC_TIMER_INTERVAL;
	tutk_timer->status = TUTK_TIMER_START;
	tutk_timer->handle = handle;
	tutk_timer->priv = arg;
	pthread_mutex_init(&tutk_timer->mutex, NULL);
	list_add_tail(&tutk_timer->list, &tutk->timer_list);
	pthread_mutex_unlock(&tutk->timer_mutex);

	return tutk_timer;
}

static void tutk_common_change_timer(struct tutk_timer_t *tutk_timer, int interval)
{
	struct tutk_common_t *tutk = tutk_save;

	pthread_mutex_lock(&tutk->timer_mutex);
	tutk_timer->base_cnt = 0;
	tutk_timer->interval = interval / TUTK_BASIC_TIMER_INTERVAL;
	tutk_timer->status = TUTK_TIMER_START;
	pthread_mutex_unlock(&tutk->timer_mutex);

	return;
}

static void tutk_common_stop_timer(struct tutk_timer_t *tutk_timer)
{
	struct tutk_common_t *tutk = tutk_save;

	pthread_mutex_lock(&tutk->timer_mutex);
	list_del(&tutk_timer->list);
	pthread_mutex_destroy(&tutk_timer->mutex);
	free(tutk_timer);
	pthread_mutex_unlock(&tutk->timer_mutex);

	return;
}

static void tutk_alarm_signal_handler(int sig)
{
	struct tutk_common_t *tutk = tutk_save;
	struct tutk_timer_t *tutk_timer;
	struct list_head_t *l, *n;

	pthread_mutex_lock(&tutk->timer_mutex);
	list_for_each_safe(l, n, &tutk->timer_list) {
		tutk_timer = container_of(l, struct tutk_timer_t, list);
		pthread_mutex_lock(&tutk_timer->mutex);
		if (tutk_timer->status == TUTK_TIMER_STOP) {
			pthread_mutex_unlock(&tutk_timer->mutex);
			continue;
		} else {
			tutk_timer->base_cnt++;
			if (tutk_timer->base_cnt == tutk_timer->interval) {
				tutk_timer->status = tutk_timer->handle(tutk_timer, tutk_timer->priv);
				tutk_timer->base_cnt = 0;
			}
		}
		pthread_mutex_unlock(&tutk_timer->mutex);
	}
	pthread_mutex_unlock(&tutk->timer_mutex);

	return;
}

static void tutk_common_add_cmd(struct tutk_common_t *tutk, int cmd_type, int cmd, unsigned cmd_arg)
{
	struct cmd_data_t *cmd_data = NULL;
	struct list_head_t *l, *n;

	if (cmd_type == CMD_TUTK_COMMON) {
		if (tutk->main_start < 0)
			return;

		cmd_data = (struct cmd_data_t *)malloc(sizeof(struct cmd_data_t));
		if (cmd_data == NULL)
			return;
		pthread_mutex_lock(&tutk->mutex);
		cmd_data->session_cmd = cmd;
		cmd_data->cmd_arg = cmd_arg;
		if ((cmd == CMD_TUTK_QUIT) || (cmd == CMD_DISABLE_SERVER) || (cmd == CMD_SERVER_STOP_SESSION))
			list_add(&cmd_data->list, &tutk->cmd_list);
		else
			list_add_tail(&cmd_data->list, &tutk->cmd_list);
		if (cmd >= 0)
			LOGD("tutk add cmd %s \n", tutk_session_cmd[cmd]);

		if ((cmd == CMD_DISABLE_SERVER) || (cmd == CMD_SERVER_STOP_SESSION)) {
			list_for_each_safe(l, n, &tutk->cmd_list) {
				cmd_data = container_of(l, struct cmd_data_t, list);
				if ((cmd_data->session_cmd == CMD_SERVER_START_VIDEO) || (cmd_data->session_cmd == CMD_SERVER_STOP_VIDEO)
					|| (cmd_data->session_cmd == CMD_SERVER_SET_VIDEO_RESOLUTION) || (cmd_data->session_cmd == CMD_SERVER_START_AUDIO)
					|| (cmd_data->session_cmd == CMD_SERVER_STOP_AUDIO) || (cmd_data->session_cmd == CMD_SERVER_RESTART_AUDIO)
					|| (cmd_data->session_cmd == CMD_CLIENT_START_AUDIO) || (cmd_data->session_cmd == CMD_CLIENT_STOP_AUDIO)
					|| (cmd_data->session_cmd == CMD_CLIENT_RESTART_AUDIO) || (cmd_data->session_cmd == CMD_SERVER_START_PLAYBACK)
					|| (cmd_data->session_cmd == CMD_SERVER_PAUSE_PLAYBACK) || (cmd_data->session_cmd == CMD_SERVER_STOP_PLAYBACK)) {

					if ((cmd == CMD_DISABLE_SERVER) || ((cmd == CMD_SERVER_STOP_SESSION) && (cmd_data->cmd_arg == cmd_arg))) {
						list_del(&cmd_data->list);
						free(cmd_data);
					}
				} else if (cmd_data->session_cmd == CMD_SERVER_STOP_SESSION) {
					if (cmd == CMD_DISABLE_SERVER) {
						list_del(&cmd_data->list);
						free(cmd_data);
					}
				}
			}
		}
		if (tutk->main_start > 0)
			pthread_cond_signal(&tutk->cond);
		pthread_mutex_unlock(&tutk->mutex);
	} else if (cmd_type == CMD_TUTK_RECORD) {
		cmd_data = (struct cmd_data_t *)malloc(sizeof(struct cmd_data_t));
		if (cmd_data == NULL)
			return;
		pthread_mutex_lock(&tutk->rec_mutex);
		cmd_data->session_cmd = cmd;
		cmd_data->cmd_arg = cmd_arg;
		if (cmd_data->session_cmd == CMD_TUTK_QUIT)
			list_add(&cmd_data->list, &tutk->rec_cmd_list);
		else
			list_add_tail(&cmd_data->list, &tutk->rec_cmd_list);
		if (tutk->server_record_thread)
			pthread_cond_signal(&tutk->rec_cond);
		pthread_mutex_unlock(&tutk->rec_mutex);
	}

	return;
}

static int tutk_common_check_session(struct tutk_common_t *tutk, struct tutk_session_t *tutk_session)
{
	struct tutk_session_t *tutk_session_list = NULL;
	struct list_head_t *l, *n;

	list_for_each_safe(l, n, &tutk->session_list) {
		tutk_session_list = container_of(l, struct tutk_session_t, list);
		if (tutk_session_list == tutk_session)
			return 1;
	}

	return 0;
}

static void tutk_common_event_report(struct tutk_common_t *tutk, int event_type, char *event_arg)
{
	return;
}

void tutk_common_start(struct tutk_common_t *tutk)
{
	struct tutk_session_t *tutk_session = NULL;
	struct cmd_data_t *cmd_data = NULL;
	struct tutk_timer_t *tutk_timer = NULL;
	struct wifi_config_t *wifi_config;
	struct list_head_t *l, *n;
	struct itimerval tick;
	int ret, session_cmd = CMD_TUTK_NONE;
	unsigned cmd_arg = 0;
	void *arg_rec;

	tutk_save = tutk;
	if (!tutk->init)
		tutk->init = tutk_common_init;
	if (!tutk->deinit)
		tutk->deinit = tutk_common_deinit;
	if (!tutk->get_register_result)
		tutk->get_register_result = tutk_common_get_reg_result;
	if (!tutk->get_config)
		tutk->get_config = tutk_server_get_config;
	if (!tutk->save_config)
		tutk->save_config = tutk_server_save_config;
	if (!tutk->add_cmd)
		tutk->add_cmd = tutk_common_add_cmd;
	if (!tutk->play_audio)
		tutk->play_audio = tutk_common_play_audio;
	if (!tutk->event_report)
		tutk->event_report = tutk_common_event_report;
	tutk->run_timer = tutk_common_run_timer;
	tutk->stop_timer = tutk_common_stop_timer;
	tutk->change_timer = tutk_common_change_timer;
	tutk->event_handle = tutk_common_event_handle;

	INIT_LIST_HEAD(&tutk->timer_list);
	INIT_LIST_HEAD(&tutk->cmd_list);
	INIT_LIST_HEAD(&tutk->rec_cmd_list);
	INIT_LIST_HEAD(&tutk->record_list);
	INIT_LIST_HEAD(&tutk->session_list);
	sem_init(&tutk->wifi_sem, 0, 0);
	sem_init(&tutk->thread_sem, 0, 0);
	pthread_mutex_init(&tutk->timer_mutex, NULL);
	pthread_mutex_init(&tutk->play_mutex, NULL);
	pthread_mutex_init(&tutk->rec_file_mutex, NULL);
	pthread_mutex_init(&tutk->register_mutex, NULL);
	signal(SIGALRM, tutk_alarm_signal_handler);
	tick.it_value.tv_sec = 0;
	tick.it_value.tv_usec = 1;
	tick.it_interval.tv_sec = 0;
	tick.it_interval.tv_usec = TUTK_BASIC_TIMER_INTERVAL * 1000;
    setitimer(ITIMER_REAL, &tick, NULL);

	pthread_mutex_init(&tutk->mutex, NULL);
	pthread_cond_init(&tutk->cond, NULL);
	pthread_mutex_init(&tutk->rec_mutex, NULL);
	pthread_cond_init(&tutk->rec_cond, NULL);

	tutk->init(tutk);

	event_register_handler(EVENT_VPLAY, 0, tutk_record_handle);
	tutk_record_init(tutk);
	ret = pthread_create(&tutk->server_record_thread, NULL, &tutk_record_thread, (void *)tutk);
	if (ret) {
		tutk->server_record_thread = 0;
		ERROR("record_thread creat failed %d \n", ret);
	}
	
	while (1) {

		pthread_mutex_lock(&tutk->mutex);
		if (!tutk->main_start)
			tutk->main_start = 1;
		if (list_empty(&tutk->cmd_list)) {
			session_cmd = CMD_TUTK_NONE;
			cmd_arg = 0;
		} else {
			l = tutk->cmd_list.next;
			cmd_data = container_of(l, struct cmd_data_t, list);
			session_cmd = cmd_data->session_cmd;
			cmd_arg = cmd_data->cmd_arg;
			list_del(&cmd_data->list);
			free(cmd_data);
		}
		if (session_cmd == CMD_TUTK_NONE) {
			pthread_cond_wait(&tutk->cond, &tutk->mutex);
			//LOGD("tutk main thread cmd wake  \n");
		}
		pthread_mutex_unlock(&tutk->mutex);

		if (session_cmd == CMD_TUTK_QUIT) {
			tutk->main_start = -1;
			if (tutk->process_run)
				tutk_disable_server(tutk);
			LOGD("tutk main thread quit \n");
			break;
		}

		//LOGD("tutk current cmd start %s \n", tutk_session_cmd[session_cmd]);
		switch (session_cmd) {
			case CMD_ENABLE_SERVER:
				tutk_enable_server(tutk);
				break;
			case CMD_DISABLE_SERVER:
				tutk_disable_server(tutk);
				break;
			case CMD_SERVER_START_SESSION:
				ret = tutk_enable_server_session(tutk, cmd_arg);
				if (ret)
					IOTC_Session_Close(cmd_arg);
				break;
			case CMD_SERVER_STOP_SESSION:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) {
					tutk_server_disable_playback(tutk, tutk_session);
					tutk_disable_server_session(tutk, tutk_session);
				}
				break;
			case CMD_START_WIFI_SCAN:
			    wifi_sta_scan();
				break;
			case CMD_CONNECTE_AP:
			    wifi_config = (struct wifi_config_t *)cmd_arg;
				if (wifi_config) {
					if (tutk->wifi_sta_connected)
						wifi_sta_disconnect(NULL);
			    	wifi_sta_connect(wifi_config);
			    	free(wifi_config);
			    }
				break;
			case CMD_FORGET_AP:
			    wifi_config = (struct wifi_config_t *)cmd_arg;
				if (wifi_config) {
			    	wifi_sta_forget(wifi_config);
			    	free(wifi_config);
			    }
				break;
			case CMD_START_REG_DEV:
				ret = pthread_create(&tutk->server_register_thread, NULL, &tutk_server_register_thread, (void *)tutk);
				if (ret)
					ERROR("register_thread creat failed %d \n", ret);
				else
			    	pthread_detach(tutk->server_register_thread);
				break;
			case CMD_SERVER_START_VIDEO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) 
					tutk_server_start_stream(tutk, CMD_SERVER_START_VIDEO, tutk_session);
				break;
			case CMD_SERVER_STOP_VIDEO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session))
					tutk_server_stop_stream(tutk, CMD_SERVER_STOP_VIDEO, tutk_session);
				break;
			case CMD_SERVER_START_PLAYBACK:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) {
					ret = tutk_server_enable_playback(tutk, tutk_session);
					if (!ret)
						tutk_server_start_stream(tutk, CMD_SERVER_START_PLAYBACK, tutk_session);
				}
				break;
			case CMD_SERVER_PAUSE_PLAYBACK:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session))
					tutk_server_stop_stream(tutk, CMD_SERVER_PAUSE_PLAYBACK, tutk_session);
				break;
			case CMD_SERVER_STOP_PLAYBACK:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) {
					tutk_server_stop_stream(tutk, CMD_SERVER_STOP_PLAYBACK, tutk_session);
					tutk_server_disable_playback(tutk, tutk_session);
				}
				break;
			case CMD_SERVER_SET_VIDEO_RESOLUTION:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session))
					tutk_server_set_resolution(tutk, CMD_SERVER_SET_VIDEO_RESOLUTION, tutk_session);
				break;
			case CMD_SERVER_START_AUDIO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session))
					tutk_server_start_stream(tutk, CMD_SERVER_START_AUDIO, tutk_session);
				break;
			case CMD_SERVER_STOP_AUDIO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session))
					tutk_server_stop_stream(tutk, CMD_SERVER_STOP_AUDIO, tutk_session);
				break;
			case CMD_SERVER_RESTART_AUDIO:
				list_for_each_safe(l, n, &tutk->session_list) {
					tutk_session = container_of(l, struct tutk_session_t, list);
					tutk_server_start_stream(tutk, CMD_SERVER_RESTART_AUDIO, tutk_session);
				}
				break;
			case CMD_CLIENT_START_AUDIO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) {
					ret = tutk_server_enable_client(tutk, tutk_session);
					if (!ret)
						tutk_server_start_stream(tutk, CMD_CLIENT_START_AUDIO, tutk_session);
				}
				break;
			case CMD_CLIENT_STOP_AUDIO:
				tutk_session = (struct tutk_session_t *)cmd_arg;
				if (tutk_common_check_session(tutk, tutk_session)) {
					tutk_server_stop_stream(tutk, CMD_CLIENT_STOP_AUDIO, tutk_session);
					tutk_server_disable_client(tutk, tutk_session);
				}
				break;
			case CMD_CLIENT_RESTART_AUDIO:
				list_for_each_safe(l, n, &tutk->session_list) {
					tutk_session = container_of(l, struct tutk_session_t, list);
					tutk_server_start_stream(tutk, CMD_CLIENT_RESTART_AUDIO, tutk_session);
				}
				break;
			case CMD_SERVER_START_RECORD:
				tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_START_RECORD, 0);
				break;
			case CMD_SERVER_STOP_RECORD:
				tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_STOP_RECORD, 0);
				break;
			case CMD_SERVER_RESTART_RECORD:
				tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_RESTART_RECORD, 0);
				break;
			case CMD_SERVER_CHANGE_SCENE:
				list_for_each_safe(l, n, &tutk->session_list) {
					tutk_session = container_of(l, struct tutk_session_t, list);
					tutk_server_change_scene(tutk, CMD_SERVER_CHANGE_SCENE, tutk_session);
				}
				break;
			default:
				break;
		}
		if (session_cmd != CMD_TUTK_NONE)
			LOGD("tutk current cmd end %s \n", tutk_session_cmd[session_cmd]);
	}

	if (tutk->server_record_thread) {
		tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_SERVER_STOP_RECORD, 0);
		tutk->add_cmd(tutk, CMD_TUTK_RECORD, CMD_TUTK_QUIT, 0);
		pthread_join(tutk->server_record_thread, &arg_rec);
		tutk->server_record_thread = 0;
		tutk_record_deinit(tutk);
	}
	pthread_mutex_destroy(&tutk->rec_mutex);
	pthread_cond_destroy(&tutk->rec_cond);

	tutk->deinit(tutk);
	sem_destroy(&tutk->wifi_sem);
	sem_destroy(&tutk->thread_sem);

	pthread_mutex_lock(&tutk->timer_mutex);
	list_for_each_safe(l, n, &tutk->timer_list) {
		tutk_timer = container_of(l, struct tutk_timer_t, list);
		list_del(&tutk_timer->list);
		free(tutk_timer);
	}
	pthread_mutex_unlock(&tutk->timer_mutex);
	pthread_mutex_destroy(&tutk->timer_mutex);
	pthread_mutex_destroy(&tutk->play_mutex);
	pthread_mutex_destroy(&tutk->register_mutex);

	pthread_mutex_lock(&tutk->mutex);
	list_for_each_safe(l, n, &tutk->cmd_list) {
		cmd_data = container_of(l, struct cmd_data_t, list);
		list_del(&cmd_data->list);
		free(cmd_data);
	}
	pthread_mutex_unlock(&tutk->mutex);
	pthread_mutex_destroy(&tutk->mutex);
	pthread_cond_destroy(&tutk->cond);

	return;
}
