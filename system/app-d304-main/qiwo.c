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

#include <linux/reboot.h>
#include <linux/input.h>
#include <linux/fs.h>
#include <qsdk/sys.h>
#include <qsdk/wifi.h>
#include <qsdk/event.h>
#include <qsdk/items.h>
#include <qsdk/videobox.h>
#include <qsdk/camera.h>
#include <qsdk/vplay.h>
#include <qsdk/cep.h>

#include "d304main.h"
#include "base64.h"
#include "IOTCAPIs.h"
#include "AVAPIs.h"
#include "AVFRAMEINFO.h"
#include "AVIOCTRLDEFs.h"
#include "RDTAPIs.h"

#define QIWO_DEBUG		1

#if QIWO_DEBUG
#define  LOGD(...)   printf(__VA_ARGS__)
#else
#define  LOGD(...)
#endif
#define  ERROR(...)   printf(__VA_ARGS__)

#define RUN_BLE_NONE   	   -1
#define RUN_BLE_MESH_CMD   0x10
#define RUN_BLE_SERVER_CMD 0x11
#define BT_CONNECT_CMD     0xa2
#define BT_DISCONNECT_CMD  0xa3
#define QIWO_ALARM_INTERVAL	180
#define QIWO_RECORD_DUATION	600
#define QIWO_NTPD_SERVER_NUM	13

#define D304_IRLEDGPIO 			164
#define D304_IRPWMGPIO 			85
#define D304_IRCUTAGPIO 		163
#define D304_IRCUTBGPIO 		126

#define SETTING_PREFEX		"qiwo"
#define INIT_LED_NAME		"init_led"
#define REG_LED_NAME		"reg_led"
#define EVENT_RECORD_DIR	"qiwo_event"
#define NTPD_FAIL_EVENT		"stratum"

#define PREVIEW_SOUND		"preview_sound"
#define QIWO_CLOUD_PREFIX	"/root/qiwo/"
#define REGISTER_CMD_FORMAT "curl -E %s -d \"ipcam_mac=%s&uid=%s&version=%s&familyid=%s&bta=%s\"  https://sh.qiwocloud1.com/v1/gateway/register -k > %s"
#define REGISTER_CA_FILE "/root/qiwo/cert_all_ipcam_infotm.pem"
#define REGISTER_SERVER_HEAD "test-"
#define REGISTER_RESULT_FILE "/tmp/qiwo_reg_result_temp_file"
#define REG_RESULT_OK_HEAD "\"_status\":{\"_code\":200"
#define REG_RESULT_UID_BEGIN "\"p2p_uid\":\""
#define REG_RESULT_PWD_BEGIN "\"tutk_pwd\":"
#define REG_RESULT_PWD_PROP_END ","
#define REG_RESULT_PROP_END "\""
#define REG_VERSION		"4|534|2"
#define SD_EVENT_FILE	"/tmp/event_file"
#define WIFI_CONNECT_VOICE		"/root/qiwo/wav/connectNetwork.wav"
#define REG_SUCCESS_VOICE		"/root/qiwo/wav/online.wav"
#define BINDING_VOICE			"/root/qiwo/wav/startConfig.wav"
#define ALARM_VOICE				"/root/qiwo/wav/alarm.wav"
#define SDCARD_MOUNT_VOICE		"/root/qiwo/wav/mount.wav"
#define RESET_VOICE				"/root/qiwo/wav/reset.wav"

static const char *pQiwoChineseCloudSetVideoListUrl = "https://sh.qiwocloud1.com/v1/gateway/setVideoList";
static const char *pQiwoChineseCloudSetVideoFileUrl = "https://sh.qiwocloud1.com/v1/gateway/setVideoFile";
static const char *pQiwoChineseCloudMotionAlertUrl = "https://sh.qiwocloud1.com/v1/gateway/motionAlert";
static const char *pQiwoForeignCloudSetVideoListUrl = "https://sh.qiwocloud2.com/v1/gateway/setVideoList";
static const char *pQiwoForeignCloudSetVideoFileUrl = "https://sh.qiwocloud2.com/v1/gateway/setVideoFile";
static const char *pQiwoForeignCloudMotionAlertUrl = "https://sh.qiwocloud2.com/v1/gateway/motionAlert";

static char sNTPServerListChina[QIWO_NTPD_SERVER_NUM][64] = {{"202.120.2.101"},
                                        {"202.112.31.197"},
                                        {"202.112.29.82"},
                                        {"202.118.1.130"},
                                        {"123.204.45.116"},
                                        {"202.108.6.95"},
                                        {"110.75.190.198"},
                                        {"115.28.122.198"},
                                        {"182.92.12.11"},
                                        {"120.25.108.11"},
                                        {"110.75.186.249"},
                                        {"110.75.186.248"},
                                        {"110.75.186.247"}};
static char sNTPServerListForeign[QIWO_NTPD_SERVER_NUM][64] = {{"0.pool.ntp.org"}, 
                                        {"1.pool.ntp.org"}, 
                                        {"pool.ntp.org"}, 
                                        {"1.debian.pool.ntp.org"}, 
                                        {"asia.pool.ntp.org"},
                                        {"24.56.178.140"},
                                        {"131.107.13.100"},
                                        {"139.162.20.174"},
                                        {"103.11.143.248"},
                                        {"218.234.23.44"},
                                        {"211.233.40.78"},
                                        {"131.188.3.220"},
                                        {"133.243.238.243"}};

struct qiwo_ipc_t {
	struct tutk_common_t qiwo_tutk;
	struct list_head_t event_list;
	int booted;
	int ble_new_connect;
	int ble_recv;
	char init_led[16];
	char reg_led[16];
	char reg_uid[32];
	char reg_fam_id[32];
	char reg_time_zone[32];
	char wifi_addr[32];
	char bt_addr[32];
	char bt_scan_result[256];
	char bt_connect_save[16];
	char country[32];
	int bt_mode;
	int bt_scan_len;
	int bt_paired;
	int bt_connect_len;
	int bright_num;
	int report_running;
	int board;
	struct timeval alarm_time;
	pthread_mutex_t event_mutex;
	pthread_mutex_t bt_mutex;
	pthread_cond_t event_cond;
	pthread_t event_report_thread;
	int ntpd_server_no;
	int ntpd_timeout;
	int preview_sound;
	int ring_start_bug;
	struct tutk_timer_t *bt_start_timer;
	struct tutk_timer_t *boot_timer;
	struct tutk_timer_t *reset_key_timer;
	struct tutk_timer_t *reset_timer;
	struct tutk_timer_t *bt_timer;
	struct tutk_timer_t *motion_timer;
	struct tutk_timer_t *dark_timer;
	struct tutk_timer_t *alarm_record_timer;
	struct tutk_timer_t *preview_sound_timer;
};

static struct qiwo_ipc_t *qiwo_save;
static int qiwo_bt_start_timer(struct tutk_timer_t *tutk_timer, void *arg);

static void qiwo_signal_handler(int sig)
{
	struct qiwo_ipc_t *qiwo_ipc = qiwo_save;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	char name[32];

	if ((sig == SIGQUIT) || (sig == SIGTERM)) {
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_TUTK_QUIT, 0);
		tutk->event_report(tutk, EVENT_QUIT, NULL);
		if (item_exist("bt.model") && (access("usr/bin/d304bt", F_OK) == 0) && (system_check_process("d304bt") > 0))
			system_kill_process("d304bt");

	} else if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		LOGD("qiwo Segmentation Fault %s \n", name);
		exit(1);
	}

	return;
}

static void qiwo_scene_mode_io(struct qiwo_ipc_t *qiwo_ipc, int mode)
{
	if (mode == CAM_DAY) {
		system_set_pin_value(D304_IRLEDGPIO, 0);
		system_set_pin_value(D304_IRPWMGPIO, 0);
		system_set_pin_value(D304_IRCUTAGPIO, 0);
		system_set_pin_value(D304_IRCUTBGPIO, 0);
	} else if (mode == CAM_NIGHT)  {
		system_set_pin_value(D304_IRCUTAGPIO, 1);
		system_set_pin_value(D304_IRCUTBGPIO, 1);
		system_set_pin_value(D304_IRLEDGPIO, 1);
		system_set_pin_value(D304_IRPWMGPIO, 0);
	}

	return;
}

static void qiwo_send_bt_cmd(struct tutk_common_t *tutk, char *cmd, int len)
{
	event_send("qiwo_bt_cmd", cmd, len);
	return;
}

static int qiwo_motion_scene_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;

	if (tutk->scene_mode == DAYTIME_DYNAMIC_SCENE)
		tutk->scene_mode = DAYTIME_STATIC_SCENE;
	else if (tutk->scene_mode == NIGHTTIME_DYNAMIC_SCENE)
		tutk->scene_mode = NIGHTTIME_STATIC_SCENE;
	tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);

	return TUTK_TIMER_STOP;
}

static int qiwo_dark_scene_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	int isp_bv = 0;
	enum cam_scenario scen = 0;

	camera_get_brightnessvalue(CAMERA_SENSOR_NAME, &isp_bv);
	camera_get_scenario(CAMERA_SENSOR_NAME, &scen);
	if (scen == CAM_DAY) {
		if (isp_bv < ISP_DARK) {
			camera_monochrome_enable(CAMERA_SENSOR_NAME, 1);
			camera_set_scenario(CAMERA_SENSOR_NAME, CAM_NIGHT);
			qiwo_scene_mode_io(qiwo_ipc, CAM_NIGHT);
			return TUTK_TIMER_STOP;
		}
	} else if (scen == CAM_NIGHT) {
		if (isp_bv >= ISP_LIGHT)
			qiwo_ipc->bright_num++;
		else
			qiwo_ipc->bright_num = 0;
		if (qiwo_ipc->bright_num >= 1) {
			qiwo_scene_mode_io(qiwo_ipc, CAM_DAY);
			camera_set_scenario(CAMERA_SENSOR_NAME, CAM_DAY);
			camera_monochrome_enable(CAMERA_SENSOR_NAME, 0);
			qiwo_ipc->bright_num = 0;
			return TUTK_TIMER_STOP;
		}
	}

	return TUTK_TIMER_RESTART;
}

static int qiwo_alarm_record_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;

	if (!strcmp(tutk->rec_type, FILE_SAVE_ALARM))
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_RECORD, 0);

	return TUTK_TIMER_STOP;
}

static int qiwo_preview_sound_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;

	qiwo_ipc->ring_start_bug = 0;
	return TUTK_TIMER_STOP;
}

/*static int qiwo_bt_connect_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	char cmd[32];

	if (qiwo_ipc->bt_mode == RUN_BLE_SERVER_CMD)
		return TUTK_TIMER_RESTART;
	if (qiwo_ipc->bt_connect_len) {
		memset(cmd, 0, 32);
		cmd[0] = qiwo_ipc->bt_connect_len + 3;
		cmd[4] = BT_CONNECT_CMD;
		cmd[5] = qiwo_ipc->bt_connect_len;
		memcpy(cmd + 7, qiwo_ipc->bt_connect_save, qiwo_ipc->bt_connect_len);
		qiwo_send_bt_cmd(tutk, cmd, qiwo_ipc->bt_connect_len + 7);
	}
	return TUTK_TIMER_STOP;
}*/

static int qiwo_get_reg_item(const char* result, char* begin_str, char* buf, int len)
{
	if (!result || !begin_str || !buf) 
		return -1;

	char* begin = strstr(result, begin_str);
	char*end = NULL;
	if (begin) {
		begin += strlen(begin_str);
		end = strstr(begin, REG_RESULT_PROP_END);
		if (!strcmp(begin_str, REG_RESULT_PWD_BEGIN)){
			end = strstr(begin, REG_RESULT_PWD_PROP_END);
		}
		if(end && (len > (end-begin))) {
			memcpy(buf, begin, end-begin);
			buf[end-begin] = '\0';
		} else {
			ERROR("fail %s: %s\n", begin_str, begin);
			return -2;
		}        
	} else {
		ERROR("fail find: %s\n", begin_str);
		return -3;
	}

	return 0;
}

static int qiwo_get_reg_result(struct tutk_common_t *tutk, char *uid, char *pwd)
{
	struct qiwo_ipc_t *qiwo_ipc = container_of(tutk, struct qiwo_ipc_t, qiwo_tutk);
	FILE *fp;
	char reg_cmd[512];
	int ret;

	sprintf(reg_cmd, REGISTER_CMD_FORMAT, REGISTER_CA_FILE, qiwo_ipc->wifi_addr, qiwo_ipc->reg_uid, REG_VERSION, qiwo_ipc->reg_fam_id, qiwo_ipc->bt_addr, REGISTER_RESULT_FILE);
	ret = system(reg_cmd);
	LOGD(" %s result %d \n", reg_cmd, ret);

	fp = fopen(REGISTER_RESULT_FILE, "r+");
	if (fp == NULL) {
		ERROR("open error reg result %s \n", strerror(errno));
		return -1;
	}
	if (fgets(reg_cmd, 512, fp)) {
		if (!strstr(reg_cmd, REG_RESULT_OK_HEAD)) {
			fclose(fp);
			remove(REGISTER_RESULT_FILE);
			return -2;
		}
	}
	fclose(fp);
	remove(REGISTER_RESULT_FILE);

	ret = qiwo_get_reg_item(reg_cmd, REG_RESULT_UID_BEGIN, uid, 32);
	if (ret)
		return ret;
	ret = qiwo_get_reg_item(reg_cmd, REG_RESULT_PWD_BEGIN, pwd, 32);
	if (ret)
		return ret;

	if (!tutk->reg_state) {
		if (qiwo_ipc->init_led[0] != 0)
			system_set_led(qiwo_ipc->init_led, 0, 0, 0);
		if (qiwo_ipc->reg_led[0] != 0)
			system_set_led(qiwo_ipc->reg_led, 1, 0, 0);
	}
	tutk->save_config(NULL, REG_STATE, "1");
	sprintf(tutk->reg_uid, uid);
	sprintf(tutk->reg_pwd, pwd);
	tutk->save_config(NULL, TUTK_P2P_UID, tutk->reg_uid);
	tutk->save_config(NULL, TUTK_P2P_PWD, tutk->reg_pwd);
	tutk->reg_state = 1;
	sync();
	if (!tutk->reset_flag)
		tutk->play_audio(tutk, WIFI_CONNECT_VOICE);
	/*if (qiwo_ipc->bt_paired) {
		qiwo_ipc->bt_paired = 0;
		if (!qiwo_ipc->bt_timer)
			qiwo_ipc->bt_timer = tutk->run_timer(2000, qiwo_bt_connect_timer, (void *)qiwo_ipc);
		else
			tutk->change_timer(qiwo_ipc->bt_timer, 2000);
	}*/
	return 0;
}

static void qiwo_load_setting(struct tutk_common_t *tutk)
{
	struct qiwo_ipc_t *qiwo_ipc = container_of(tutk, struct qiwo_ipc_t, qiwo_tutk);
	char value[256];
	int ret;

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_P2P_UID, value, 64);
	if (value[0] != '\0')
		sprintf(tutk->reg_uid, value);

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_P2P_ACC, value, 64);
	if (value[0] == '\0') {
		sprintf(tutk->reg_acc, LOCAL_ACC_NAME);
		sprintf(value, LOCAL_ACC_NAME);
		setting_set_string(SETTING_PREFEX, TUTK_P2P_ACC, value);
	} else {
		sprintf(tutk->reg_acc, value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_P2P_PWD, value, 64);
	if (value[0] != '\0')
		sprintf(tutk->reg_pwd, value);

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, LOCAL_UID_SETTING, value, 64);
	if (value[0] == '\0') {
		sprintf(tutk->local_uid, LOCAL_UID_NAME);
		sprintf(value, LOCAL_UID_NAME);
		setting_set_string(SETTING_PREFEX, LOCAL_UID_SETTING, value);
	} else {
		sprintf(tutk->local_uid, value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, COUNTRY_SETTING, value, 64);
	if (value[0] == '\0') {
		sprintf(qiwo_ipc->country, DEFAULT_COUNTRY);
		sprintf(value, DEFAULT_COUNTRY);
		setting_set_string(SETTING_PREFEX, COUNTRY_SETTING, value);
	} else {
		sprintf(qiwo_ipc->country, value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, LOCAL_ACC_SETTING, value, 64);
	if (value[0] == '\0') {
		sprintf(tutk->local_acc, LOCAL_ACC_NAME);
		sprintf(value, LOCAL_ACC_NAME);
		setting_set_string(SETTING_PREFEX, LOCAL_ACC_SETTING, value);
	} else {
		sprintf(tutk->local_acc, value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, LOCAL_PWD_SETTING, value, 64);
	if (value[0] == '\0') {
		sprintf(tutk->local_pwd, LOCAL_PWD_VALUE);
		sprintf(value, LOCAL_PWD_VALUE);
		setting_set_string(SETTING_PREFEX, LOCAL_PWD_SETTING, value);
	} else {
		sprintf(tutk->local_pwd, value);
	}

	ret = setting_get_int(SETTING_PREFEX, WIFI_AP_ENABLE);
	if (ret == SETTING_EINT)
		tutk->wifi_ap_set = 0;
	else
		tutk->wifi_ap_set = ret;
	ret = setting_get_int(SETTING_PREFEX, WIFI_STA_ENABLE);
	if (ret == SETTING_EINT)
		tutk->wifi_sta_set = 1;
	else
		tutk->wifi_sta_set = ret;

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, REG_STATE, value, 64);
	if (value[0] == '\0')
		tutk->reg_state = 0;
	else
		tutk->reg_state = atoi(value);

	if (qiwo_ipc->board == D304_BOARD) {
		qiwo_ipc->init_led[0] = 0;
		setting_get_string(SETTING_PREFEX, INIT_LED_NAME, qiwo_ipc->init_led, 64);
		if (qiwo_ipc->init_led[0] == 0) {
			sprintf(qiwo_ipc->init_led, "green");
			setting_set_string(SETTING_PREFEX, INIT_LED_NAME, qiwo_ipc->init_led);
		}

		qiwo_ipc->reg_led[0] = 0;
		setting_get_string(SETTING_PREFEX, REG_LED_NAME, qiwo_ipc->reg_led, 64);
		if (qiwo_ipc->reg_led[0] == 0) {
			sprintf(qiwo_ipc->reg_led, "blue");
			setting_set_string(SETTING_PREFEX, REG_LED_NAME, qiwo_ipc->reg_led);
		}
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_GMT, value, 64);
	if (value[0] == '\0') {
		sprintf(value, "%d", TUTK_DEFAULT_GMT);
		setting_set_string(SETTING_PREFEX, TUTK_GMT, value);
		setting_set_string(SETTING_PREFEX, TUTK_TIMEZONE, (const char *)TUTK_DEFAULT_TIMEZONE);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_RECORD_TYPE, value, 64);
	if (!strcmp(value, FILE_SAVE_FULLTIME))
		sprintf(tutk->rec_type, "%s", FILE_SAVE_FULLTIME);
	else if (!strcmp(value, FILE_SAVE_ALARM))
		sprintf(tutk->rec_type, "%s", FILE_SAVE_ALARM);
	else {
		sprintf(tutk->rec_type, "%s", FILE_SAVE_FULLTIME);
		setting_set_string(SETTING_PREFEX, TUTK_RECORD_TYPE, (const char *)FILE_SAVE_FULLTIME);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, TUTK_RECORD_DUATION, value, 64);
	if (value[0] == '\0') {
		tutk->rec_time_segment = QIWO_RECORD_DUATION;
		sprintf(value, "%d", tutk->rec_time_segment);
		setting_set_string(SETTING_PREFEX, TUTK_RECORD_DUATION, value);
	} else {
		tutk->rec_time_segment = atoi(value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, WARNING_SOUND, value, 64);
	if (value[0] == '\0') {
		sprintf(value, "1");
		setting_set_string(SETTING_PREFEX, WARNING_SOUND, value);
		tutk->warning_sound = 1;
	} else {
		tutk->warning_sound = atoi(value);
	}

	value[0] = '\0';
	setting_get_string(SETTING_PREFEX, PREVIEW_SOUND, value, 64);
	if (value[0] == '\0') {
		qiwo_ipc->preview_sound = 0;
		setting_set_string(SETTING_PREFEX, PREVIEW_SOUND, "0");
	} else {
		qiwo_ipc->preview_sound = atoi(value);
	}

	return;
}

static void qiwo_event_report(struct tutk_common_t *tutk, int event_type, char *event_arg)
{
	struct qiwo_ipc_t *qiwo_ipc = container_of(tutk, struct qiwo_ipc_t, qiwo_tutk);
	struct event_cmd_t *event_cmd = NULL;

	if ((tutk->reset_flag) && (!((event_type == EVENT_QUIT) || (event_type == EVENT_STOP))))
		return;

	event_cmd = (struct event_cmd_t *)malloc(sizeof(struct event_cmd_t));
	if (event_cmd == NULL)
		return;

	pthread_mutex_lock(&qiwo_ipc->event_mutex);
	event_cmd->cmd_type = event_type;
	if (event_arg != NULL)
		snprintf(event_cmd->event, 64, event_arg);
	if (event_cmd->cmd_type == CMD_TUTK_QUIT)
		list_add(&event_cmd->list, &qiwo_ipc->event_list);
	else
		list_add_tail(&event_cmd->list, &qiwo_ipc->event_list);
	if (qiwo_ipc->event_report_thread)
		pthread_cond_signal(&qiwo_ipc->event_cond);
	pthread_mutex_unlock(&qiwo_ipc->event_mutex);

	return;
}

static int qiwo_report_event(struct qiwo_ipc_t *qiwo_ipc, int event_type, char *event_arg)
{
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct record_file_t *record_file;
	struct list_head_t *l, *n;
	int op_type, qiwo_event_type, duration;
	struct tm tm_current;
	FILE *fp;
	char cmd[1024], temp[128], value[128], file_type[32], file_name[32], file[128];
	char *ptr;
	int ret = 0;
	long fsize;

	switch (event_type) {
		case EVENT_MOTIONDECTED:
			sprintf(cmd, "curl ");
			if (!strcmp(qiwo_ipc->country, DEFAULT_COUNTRY))
				strcat(cmd, pQiwoChineseCloudMotionAlertUrl); /* qiwo cloud setVideoFile URL */
			else
				strcat(cmd, pQiwoForeignCloudMotionAlertUrl);
			strcat(cmd, " ");
			strcat(cmd, "-E '/root/qiwo/ca.cer' "); /* option cert path */
			strcat(cmd, "-H 'Content-type:applicaion/json' "); /* option json */
			strcat(cmd, "-X POST "); /* option json */
			sprintf(temp, "-d \'{\"did\":\"%s\",\"data\":{\"did\":\"%s\",\"capture_time\":%ld,\"sd_in\":%d}}\' ",
						qiwo_ipc->bt_addr, qiwo_ipc->bt_addr, qiwo_ipc->alarm_time.tv_sec, tutk->tf_card_state);
			strcat(cmd, temp);
			strcat(cmd, "--cacert '/root/qiwo/ca-bundle.crt' "); /* option cert path */
			sprintf(temp, "> %s", REGISTER_RESULT_FILE); /* save the response to file */
			strcat(cmd, temp);
			ret = system(cmd);
			break;
		case EVENT_FILELIST:
			if (tutk->tf_card_state) {
				fp = fopen(SD_EVENT_FILE, "a+");  
				if (fp == NULL) {
					ERROR("qiwo open error event file %s \n", strerror(errno));  
					return 0;  
				}

				pthread_mutex_lock(&tutk->rec_file_mutex);
				list_for_each_safe(l, n, &tutk->record_list) {

					record_file = container_of(l, struct record_file_t, list);
					memset(&tm_current, 0, sizeof(struct tm));
					memset(file_type, 0, 32);
					duration = 0;
					ret = sscanf(record_file->file_name, "%d_%d_%d_%d_%d_%d_%s",
							&tm_current.tm_year, &tm_current.tm_mon, &tm_current.tm_mday,
							&tm_current.tm_hour, &tm_current.tm_min, &tm_current.tm_sec, file_type);

					ptr = strstr(file_type, "_");
					if (ptr != NULL) {
						duration = atoi(ptr + 1);
						ptr[0] = '\0';
					}
					if ((ret < 0) || (duration == 0)) {
						ERROR("not valid record file %s  %s \n", record_file->file_name, file_type);
						continue;
					}

					if (!strcmp(file_type, FILE_SAVE_ALARM)) {
						qiwo_event_type = 1;
					} else if (!strcmp(file_type, FILE_SAVE_FULLTIME)) {
						qiwo_event_type = 0;
					} else {
						continue;
					}
					sprintf(file, "%04d%02d%02d-%02d%02d,%d,%d", tm_current.tm_year, tm_current.tm_mon, tm_current.tm_mday,
									tm_current.tm_hour, tm_current.tm_min, duration, qiwo_event_type);

					fsize = fseek(fp, 0, SEEK_END);
					ret = fseek(fp, fsize, SEEK_SET);
					if (ret < 0) {
						ERROR("fseek error event file %s \n", strerror(errno)); 
						break;
					}
					fprintf(fp, "%s\n", file);
				}
				pthread_mutex_unlock(&tutk->rec_file_mutex);
			} else {
				fp = fopen(SD_EVENT_FILE, "w+");  
				if (fp == NULL) {
					ERROR("qiwo open error event file %s \n", strerror(errno));  
					return 0;  
				}
			}
			fclose(fp);

			sprintf(cmd, "curl ");
			if (!strcmp(qiwo_ipc->country, DEFAULT_COUNTRY))
				strcat(cmd, pQiwoChineseCloudSetVideoFileUrl);
			else
				strcat(cmd, pQiwoForeignCloudSetVideoFileUrl);
			strcat(cmd, " ");
			strcat(cmd, "-E '/root/qiwo/ca.cer' ");
			sprintf(temp, "-F \"videofile=@%s\" ", SD_EVENT_FILE);
			strcat(cmd, temp);
			sprintf(temp, "-F \"did=%s\" ", qiwo_ipc->bt_addr);
			strcat(cmd, temp);
			strcat(cmd, "--cacert '/root/qiwo/ca-bundle.crt' ");
			sprintf(temp, "> %s", REGISTER_RESULT_FILE);
			strcat(cmd, temp);
			ret = system(cmd);
			break;
		case EVENT_ADDFILE:
		case EVENT_DELFILE:
			ptr = strstr(event_arg, "@@");
			if (ptr != NULL)
				break;
			duration = 0;
			ret = sscanf(event_arg, "%d_%d_%d_%d_%d_%d_%s",
					&tm_current.tm_year, &tm_current.tm_mon, &tm_current.tm_mday,
					&tm_current.tm_hour, &tm_current.tm_min, &tm_current.tm_sec, file_type);
			ptr = strstr(file_type, "_");
			if (ptr != NULL) {
				duration = atoi(ptr + 1);
				ptr[0] = '\0';
			}
			if ((ret < 0) || (duration < 180)) {
				ERROR("not valid file event %s \n", event_arg);
				sprintf(file, "%s/%s.mkv", tutk->rec_path, event_arg);
				remove(file);
				return 0;
			}
			sprintf(file_name, "%04d%02d%02d-%02d%02d", tm_current.tm_year, tm_current.tm_mon, tm_current.tm_mday,
						tm_current.tm_hour, tm_current.tm_min);

			if (event_type == EVENT_ADDFILE)
				op_type = 1;
			else
				op_type = 0;
			if (!strcmp(file_type, FILE_SAVE_ALARM)) {
				qiwo_event_type = 1;
			} else {
				qiwo_event_type = 0;
			}

			sprintf(cmd, "curl ");
			strcat(cmd, "-E '/root/qiwo/ca.cer' ");
			sprintf(temp, "-d \"did=%s&fileName=%s&eventType=%d&opType=%d&duration=%d\" ",
						qiwo_ipc->bt_addr, file_name, qiwo_event_type, op_type, duration);
			strcat(cmd, temp);
			if (!strcmp(qiwo_ipc->country, DEFAULT_COUNTRY))
				strcat(cmd, pQiwoChineseCloudSetVideoListUrl);
			else
				strcat(cmd, pQiwoForeignCloudSetVideoListUrl);
			strcat(cmd, " ");
			strcat(cmd, "--cacert '/root/qiwo/ca-bundle.crt' ");
			sprintf(temp, "> %s", REGISTER_RESULT_FILE);
			strcat(cmd, temp);
			ret = system(cmd);
			break;
		default:
			return -1;
			break;
	}

	fp = fopen(REGISTER_RESULT_FILE, "r+");  
	if (fp == NULL) {
		ERROR("open error reg result %s \n", strerror(errno));  
		return -1;  
	}
	if (fgets(value, 512, fp)) {
		if (!strstr(value, REG_RESULT_OK_HEAD)) {
			fclose(fp);
			ERROR("qiwo event report failed %s \n", value); 
			return -2;
		}
	}
	fclose(fp);
	remove(REGISTER_RESULT_FILE);

	return 0;
}

static int qiwo_reset_key_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	return TUTK_TIMER_STOP;
}

static int qiwo_reset_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct wifi_config_t *wifi_config, *wifi_config_forget;
	char value[16], cmd[32];
	int flag = 0, cnt = 0, pid;

	if (tutk->server_start || qiwo_ipc->report_running || tutk->register_start || qiwo_ipc->ble_recv || qiwo_ipc->bt_paired) {
		if (qiwo_ipc->report_running) {
			cnt = 0;
			do {
				pid = system_check_process("curl");
				if (pid < 0)
					break;
				sprintf(value, "kill %d", pid);
				system(value);
				usleep(10000);
				cnt++;
			} while (cnt < 5);
		}
		if (qiwo_ipc->bt_paired && qiwo_ipc->bt_connect_len) {
			memset(cmd, 0, 32);
			cmd[0] = qiwo_ipc->bt_connect_len + 3;
			cmd[4] = BT_DISCONNECT_CMD;
			cmd[5] = qiwo_ipc->bt_connect_len;
			memcpy(cmd + 7, qiwo_ipc->bt_connect_save, qiwo_ipc->bt_connect_len);
			qiwo_send_bt_cmd(tutk, cmd, qiwo_ipc->bt_connect_len + 7);
		}
		return TUTK_TIMER_RESTART;
	}
	if (item_exist("bt.model") && (access("usr/bin/d304bt", F_OK) == 0) && (qiwo_ipc->bt_mode != RUN_BLE_SERVER_CMD)) {
		pthread_mutex_lock(&qiwo_ipc->bt_mutex);
		if (system_check_process("d304bt") > 0) {
			do {
				system_kill_process("d304bt");
				usleep(20000);
				pid = system_check_process("d304bt");
				if (pid < 0)
					break;
				cnt++;
			} while (cnt < 3);
		}
		if (system_check_process("d304bt") < 0) {
			system("d304bt server &");
		} else {
			memset(value, 0, 8);
			value[0] = 3;
			value[4] = RUN_BLE_SERVER_CMD;
			qiwo_send_bt_cmd(tutk, value, 7);
		}
		qiwo_ipc->bt_mode = RUN_BLE_SERVER_CMD;
		pthread_mutex_unlock(&qiwo_ipc->bt_mutex);
	}

	wifi_config = wifi_sta_get_config();
	while (wifi_config != NULL) {
		wifi_sta_forget(wifi_config);
		wifi_config_forget = wifi_config;
		wifi_config = wifi_sta_iterate_config(wifi_config_forget);
		free(wifi_config_forget);
	}

	setting_get_string(SETTING_PREFEX, TUTK_RECORD_TYPE, value, 16);
	setting_remove_all(SETTING_PREFEX);
	qiwo_load_setting(tutk);

	if (strcmp(tutk->rec_type, value))
		flag = 1;
	if (strcmp(tutk->play_audio_channel, NORMAL_CODEC_PLAY) || strcmp(tutk->rec_audio_channel, NORMAL_CODEC_RECORD)) {
		sprintf(tutk->play_audio_channel, NORMAL_CODEC_PLAY);
		sprintf(tutk->rec_audio_channel, NORMAL_CODEC_RECORD);
		tutk->capture_fmt.channels = NORMAL_CODEC_CHANNELS;
		tutk->capture_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
		tutk->capture_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
		tutk->capture_fmt.sample_size = 1000;
		tutk->play_fmt.channels = NORMAL_CODEC_CHANNELS;
		tutk->play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
		tutk->play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
		tutk->play_fmt.sample_size = 1024;
		flag = 1;
	}
	if (flag) {
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_RECORD, 0);
		if (!strcmp(tutk->rec_type, FILE_SAVE_FULLTIME))
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
	}
	qiwo_ipc->ble_recv = 0;
	tutk->play_audio(tutk, ALARM_VOICE);

	return TUTK_TIMER_STOP;
}

static void qiwo_ble_handle(char *event, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = qiwo_save;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct wifi_config_t *wifi_config_connect;
	unsigned char *recv_data;
	size_t data_len, temp_len;
	char cmd[32], *temp_addr;
	char time_zone[32];

	LOGD("qiwo_ble_handle %s %d \n", (char *)arg, strlen((char *)arg) + 1);
	if (!strncmp(event, "qiwo_ble_data", strlen("qiwo_ble_data"))) {
		if (!memcmp((char *)arg, "reg_data/", strlen("reg_data/"))) {

			if (qiwo_ipc->ble_recv || (qiwo_ipc->bt_mode == RUN_BLE_NONE))
				return;

			if (qiwo_ipc->bt_mode == RUN_BLE_SERVER_CMD) {
				qiwo_ipc->ble_recv = 1;
				if (!qiwo_ipc->bt_start_timer)
					qiwo_ipc->bt_start_timer = tutk->run_timer(1000, qiwo_bt_start_timer, (void *)qiwo_ipc);
				else
					tutk->change_timer(qiwo_ipc->bt_start_timer, 1000);
			}
			if (tutk->reg_state)
				tutk->reg_state = 0;

			wifi_config_connect = (struct wifi_config_t *)malloc(sizeof(struct wifi_config_t));
			if (!wifi_config_connect)
				return;
			qiwo_ipc->ble_new_connect = 1;
			tutk->reset_flag = 0;
			temp_addr = arg + strlen("reg_data/");
			recv_data = base64_decode((const unsigned char *)temp_addr, strlen(temp_addr), &data_len);

			temp_addr = (char *)recv_data;
			memcpy(cmd, temp_addr, 2);
			cmd[2] = '\0';
			temp_len = atoi(cmd);
			snprintf(wifi_config_connect->ssid, temp_len + 1, "%s", temp_addr+2);

			temp_addr += (2 + temp_len);
			memcpy(cmd, temp_addr, 2);
			cmd[2] = '\0';
			temp_len = atoi(cmd);
			snprintf(wifi_config_connect->psk, temp_len + 1, "%s", temp_addr+2);

			temp_addr += (2 + temp_len);
			memcpy(cmd, temp_addr, 2);
			cmd[2] = '\0';
			temp_len = atoi(cmd);
			snprintf(qiwo_ipc->reg_uid, temp_len + 1, "%s", temp_addr+2);

			temp_addr += (2 + temp_len);
			memcpy(cmd, temp_addr, 2);
			cmd[2] = '\0';
			temp_len = atoi(cmd);
			snprintf(time_zone, temp_len + 1, "%s", temp_addr+2);
			time_zone[temp_len + 1] = '\0';

			temp_addr += (2 + temp_len);
			memcpy(cmd, temp_addr, 2);
			cmd[2] = '\0';
			temp_len = atoi(cmd);
			snprintf(qiwo_ipc->reg_fam_id, temp_len + 1, "%s", temp_addr+2);

			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_CONNECTE_AP, (unsigned)wifi_config_connect);
			tutk->play_audio(tutk, BINDING_VOICE);
			LOGD("%s %s %s %s %s \n", wifi_config_connect->ssid, wifi_config_connect->psk, qiwo_ipc->reg_uid, time_zone, qiwo_ipc->reg_fam_id);
		} else if (!memcmp((char *)arg, "bt_addr/", strlen("bt_addr/"))) {
			temp_addr = arg + strlen("bt_addr/");
			sscanf(temp_addr, "%x:%x:%x:%x:%x:%x", (unsigned int *)(&cmd[0]), (unsigned int *)(&cmd[4]), (unsigned int *)(&cmd[8]), (unsigned int *)(&cmd[12]), (unsigned int *)(&cmd[16]), (unsigned int *)(&cmd[20]));
			sprintf(qiwo_ipc->bt_addr, "%x%x%x%x%x%x", cmd[0], cmd[4], cmd[8], cmd[12], cmd[16], cmd[20]);
			LOGD("qiwo bt addr %s \n", qiwo_ipc->bt_addr);
		} else if (!memcmp((char *)arg, "scan_device/", strlen("scan_device/"))) {
			temp_addr = arg + strlen("scan_device/");
			qiwo_ipc->bt_scan_len = temp_addr[0];
			memcpy(qiwo_ipc->bt_scan_result, temp_addr + 1, qiwo_ipc->bt_scan_len);
			LOGD("qiwo scan result %d \n", qiwo_ipc->bt_scan_len);
		} else if (!memcmp((char *)arg, "pair_state/", strlen("pair_state/"))) {
			temp_addr = arg + strlen("pair_state/");
			qiwo_ipc->bt_scan_len = temp_addr[0];
			memcpy(qiwo_ipc->bt_scan_result, temp_addr + 1, qiwo_ipc->bt_scan_len);
			if (qiwo_ipc->bt_scan_result[3]) {
				sprintf(tutk->play_audio_channel, BT_CODEC_PLAY);
				sprintf(tutk->rec_audio_channel, BT_CODEC_RECORD);
				tutk->capture_fmt.channels = BT_CODEC_CHANNELS;
				tutk->capture_fmt.samplingrate = BT_CODEC_SAMPLERATE;
				tutk->capture_fmt.bitwidth = BT_CODEC_BITWIDTH;
				tutk->capture_fmt.sample_size = 400;
				tutk->play_fmt.channels = BT_CODEC_CHANNELS;
				tutk->play_fmt.samplingrate = BT_CODEC_SAMPLERATE;
				tutk->play_fmt.bitwidth = BT_CODEC_BITWIDTH;
				tutk->play_fmt.sample_size = 400;
				qiwo_ipc->bt_paired = 1;
				qiwo_ipc->bt_connect_len = qiwo_ipc->bt_scan_len - 4;
				memcpy(qiwo_ipc->bt_connect_save, &qiwo_ipc->bt_scan_result[4], qiwo_ipc->bt_connect_len);
			} else {
				sprintf(tutk->play_audio_channel, NORMAL_CODEC_PLAY);
				sprintf(tutk->rec_audio_channel, NORMAL_CODEC_RECORD);
				tutk->capture_fmt.channels = NORMAL_CODEC_CHANNELS;
				tutk->capture_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
				tutk->capture_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
				tutk->capture_fmt.sample_size = 1000;
				tutk->play_fmt.channels = NORMAL_CODEC_CHANNELS;
				tutk->play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
				tutk->play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
				tutk->play_fmt.sample_size = 1024;
				qiwo_ipc->bt_paired = 0;
				memset(qiwo_ipc->bt_connect_save, 0, 16);
				qiwo_ipc->bt_connect_len = 0;
			}
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_RECORD, 0);
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_AUDIO, 0);
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_CLIENT_RESTART_AUDIO, 0);
			LOGD("qiwo pair state %d %d \n", qiwo_ipc->bt_scan_len, qiwo_ipc->bt_scan_result[3]);
		} else if (!memcmp((char *)arg, "inquiry_result/", strlen("inquiry_result/"))) {
			temp_addr = arg + strlen("inquiry_result/");
			qiwo_ipc->bt_scan_len = temp_addr[0];
			memcpy(qiwo_ipc->bt_scan_result, temp_addr + 1, qiwo_ipc->bt_scan_len);
			LOGD("qiwo inquiry result %d %d \n", qiwo_ipc->bt_scan_len, qiwo_ipc->bt_scan_result[3]);
		}
	}

	return;
}

static void qiwo_normal_handle(char *event, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = qiwo_save;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct event_lightsence *event_brightness;
	struct event_process *event_process;
	struct tutk_timer_t *qiwo_timer;
	struct event_key *reset_key;
	struct timezone tz;
	struct timeval tv;
	char value[128];
	int pid, cnt, brightness, status;
	int check_bri = 0;
	enum cam_scenario scen = 0;

	if (!qiwo_ipc->booted)
		return;
	if (!strncmp(event, EVENT_VAMOVE, strlen(EVENT_VAMOVE))) {
		if (!strcmp(tutk->rec_type, FILE_SAVE_ALARM)) {
			if (!qiwo_ipc->alarm_record_timer) {
				gettimeofday(&qiwo_ipc->alarm_time, NULL);
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
				tutk->event_report(tutk, EVENT_MOTIONDECTED, NULL);
				qiwo_ipc->alarm_record_timer = tutk->run_timer(QIWO_ALARM_INTERVAL * 1000, qiwo_alarm_record_timer, (void *)qiwo_ipc);
				LOGD("qiwo motion alarm %s \n", (char *)event);
			} else {
				qiwo_timer = qiwo_ipc->alarm_record_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				if (qiwo_timer->status == TUTK_TIMER_STOP) {
					gettimeofday(&qiwo_ipc->alarm_time, NULL);
					tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
					tutk->event_report(tutk, EVENT_MOTIONDECTED, NULL);
					LOGD("qiwo motion alarm %s \n", (char *)event);
				}
				pthread_mutex_unlock(&qiwo_timer->mutex);
				tutk->change_timer(qiwo_timer, QIWO_ALARM_INTERVAL * 1000);
			}
		}

		if (qiwo_ipc->board == D304_BOARD) {
			if (!qiwo_ipc->motion_timer) {
				if (tutk->scene_mode == DAYTIME_STATIC_SCENE)
					tutk->scene_mode = DAYTIME_DYNAMIC_SCENE;
				else if (tutk->scene_mode == NIGHTTIME_STATIC_SCENE)
					tutk->scene_mode = NIGHTTIME_DYNAMIC_SCENE;
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
				qiwo_ipc->motion_timer = tutk->run_timer(10 * 1000, qiwo_motion_scene_timer, (void *)qiwo_ipc);
			} else {
				qiwo_timer = qiwo_ipc->motion_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				if (qiwo_timer->status == TUTK_TIMER_STOP) {
					if (tutk->scene_mode == DAYTIME_STATIC_SCENE)
						tutk->scene_mode = DAYTIME_DYNAMIC_SCENE;
					else if (tutk->scene_mode == NIGHTTIME_STATIC_SCENE)
						tutk->scene_mode = NIGHTTIME_DYNAMIC_SCENE;
					tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
				}
				pthread_mutex_unlock(&qiwo_timer->mutex);
				tutk->change_timer(qiwo_timer, 10 * 1000);
			}
		}
	} else if (!strncmp(event, EVENT_LIGHTSENCE_B, strlen(EVENT_LIGHTSENCE_B))) {

		event_brightness = (struct event_lightsence *)arg;
		brightness = event_brightness->trigger_current;
		camera_get_scenario(CAMERA_SENSOR_NAME, &scen);
		if (scen == CAM_NIGHT) {
			if (!qiwo_ipc->dark_timer) {
				qiwo_ipc->bright_num = 0;
				qiwo_ipc->dark_timer = tutk->run_timer(TUTK_BASIC_TIMER_INTERVAL, qiwo_dark_scene_timer, (void *)qiwo_ipc);
			} else {
				qiwo_timer = qiwo_ipc->dark_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				status = TUTK_TIMER_START;
				if (qiwo_timer->status == TUTK_TIMER_STOP)
					status = TUTK_TIMER_STOP;
				pthread_mutex_unlock(&qiwo_timer->mutex);
				if (status == TUTK_TIMER_STOP) {
					qiwo_ipc->bright_num = 0;
					tutk->change_timer(qiwo_timer, TUTK_BASIC_TIMER_INTERVAL);
				}
			}
		} else {
			if (qiwo_ipc->dark_timer) {
				tutk->stop_timer(qiwo_ipc->dark_timer);
				qiwo_ipc->dark_timer = NULL;
			}
		}
		if (tutk->scene_mode == NIGHTTIME_STATIC_SCENE) {
			tutk->scene_mode = DAYTIME_STATIC_SCENE;
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
		} else if (tutk->scene_mode == NIGHTTIME_DYNAMIC_SCENE) {
			tutk->scene_mode = DAYTIME_DYNAMIC_SCENE;
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
		}
		LOGD("qiwo light event %d %d %d\n", brightness, camera_get_brightness(CAMERA_SENSOR_NAME, &check_bri), check_bri);
	} else if (!strncmp(event, EVENT_LIGHTSENCE_D, strlen(EVENT_LIGHTSENCE_D))) {

		event_brightness = (struct event_lightsence *)arg;
		brightness = event_brightness->trigger_current;
		camera_get_scenario(CAMERA_SENSOR_NAME, &scen);
		if (qiwo_ipc->dark_timer) {
			tutk->stop_timer(qiwo_ipc->dark_timer);
			qiwo_ipc->dark_timer = NULL;
		}
		if (scen == CAM_DAY) {
			if (brightness < LIGHT_SENSOR_DARK) {
				camera_monochrome_enable(CAMERA_SENSOR_NAME, 1);
				camera_set_scenario(CAMERA_SENSOR_NAME, CAM_NIGHT);
				qiwo_scene_mode_io(qiwo_ipc, CAM_NIGHT);
			} else {
				if (!qiwo_ipc->dark_timer)
					qiwo_ipc->dark_timer = tutk->run_timer(1000, qiwo_dark_scene_timer, (void *)qiwo_ipc);
			}
		}

		if (tutk->scene_mode == DAYTIME_STATIC_SCENE) {
			tutk->scene_mode = NIGHTTIME_STATIC_SCENE;
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
		} else if (tutk->scene_mode == DAYTIME_DYNAMIC_SCENE) {
			tutk->scene_mode = NIGHTTIME_DYNAMIC_SCENE;
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_CHANGE_SCENE, 0);
		}
		LOGD("qiwo dark event %d \n", brightness);
	} else if (!strncmp(event, EVENT_NTPOK, strlen(EVENT_NTPOK))) {

		setting_get_string(SETTING_PREFEX, TUTK_GMT, value, 64);
		gettimeofday(&tv, &tz);
		tv.tv_sec += 60 * atoi(value);
		settimeofday(&tv, &tz);
		cnt = 0;
		do {
			pid = system_check_process("busybox ntpd");
			if (pid < 0)
				break;
			system_kill_process("busybox ntpd");
			usleep(10000);
			cnt++;
		} while (cnt < 20);

		LOGD("qiwo_normal_handle time %s %d \n", (char *)event, tz.tz_minuteswest);
		tutk->ntpd_state = 1;

	} else if (!strncmp(event, NTPD_FAIL_EVENT, strlen(NTPD_FAIL_EVENT))) {
		LOGD("qiwo_normal_handle ntp failed %s \n", (char *)event);
		system_kill_process("busybox ntpd");
	} else if (!strncmp(event, EVENT_MOUNT, strlen(EVENT_MOUNT))) {
		if (tutk->tf_card_state)
			return;
		tutk->tf_card_state = 1;
		sprintf(tutk->sd_mount_point, (char *)arg);

        /**
         *
         *
         * local upgrade test /mnt/sd0/update.zip
          param:"updata_version" NULL
		        "updata_type" 1:local
	            "updata_md5" NULL
	            #define SETTING_PREFEX		"qiwo_upgrade"
         *
         */
		sprintf(value, "%s/update.zip", tutk->sd_mount_point);
		if (access(value, F_OK) == 0) {
			setting_set_string("qiwo_upgrade", "updata_version", "1.0.0");
			setting_set_int("qiwo_upgrade", "updata_type", 1);
			setting_set_string("qiwo_upgrade", "updata_md5", "dadcfc066f842953c626a3bb77994640");
			LOGD("find /mnt/sd0/update.zip do system upgrade");
			system_set_upgrade("upgrade_flag", "none");
			system("reboot");
		}

		sprintf(tutk->rec_path, "%s/%s", tutk->sd_mount_point, EVENT_RECORD_DIR);
		if (!strcmp(tutk->rec_type, FILE_SAVE_FULLTIME) || (!strcmp(tutk->rec_type, FILE_SAVE_ALARM) && qiwo_ipc->alarm_record_timer)) {

			if (qiwo_ipc->alarm_record_timer) {
				qiwo_timer = qiwo_ipc->alarm_record_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				status = TUTK_TIMER_START;
				if (qiwo_timer->status == TUTK_TIMER_STOP)
					status = TUTK_TIMER_STOP;
				pthread_mutex_unlock(&qiwo_timer->mutex);
				if (status == TUTK_TIMER_STOP)
					tutk->change_timer(qiwo_timer, QIWO_ALARM_INTERVAL * 1000);
			}
			tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
		}
		tutk->play_audio(tutk, SDCARD_MOUNT_VOICE);
	} else if (!strncmp(event, EVENT_UNMOUNT, strlen(EVENT_UNMOUNT))) {
		tutk->tf_card_state = 0;
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_RECORD, 0);
	} else if (!strncmp(event, EVENT_KEY, strlen(EVENT_KEY))) {
		reset_key = (struct event_key *)arg;
		LOGD("qiwo reset key handle %s %d %d \n", (char *)event, reset_key->key_code, reset_key->down);
		if ((reset_key->key_code == KEY_RESTART) && (reset_key->down == 1)) {
			if (qiwo_ipc->reset_key_timer == NULL) {
				qiwo_ipc->reset_key_timer = tutk->run_timer(2000, qiwo_reset_key_timer, (void *)qiwo_ipc);
			} else {
				qiwo_timer = qiwo_ipc->reset_key_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				status = TUTK_TIMER_START;
				if (qiwo_timer->status == TUTK_TIMER_STOP)
					status = TUTK_TIMER_STOP;
				pthread_mutex_unlock(&qiwo_timer->mutex);
				if (status == TUTK_TIMER_STOP)
					tutk->change_timer(qiwo_timer, 2000);
				else
					return;
			}

			tutk->reset_flag = 1;
			tutk->play_audio(tutk, RESET_VOICE);
			if (qiwo_ipc->alarm_record_timer) {
				tutk->stop_timer(qiwo_ipc->alarm_record_timer);
				qiwo_ipc->alarm_record_timer = NULL;
			}
			if (tutk->process_run)
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_DISABLE_SERVER, 0);
			//tutk->event_report(tutk, EVENT_STOP, NULL);
			cnt = 0;
			do {
				pid = system_check_process("busybox ntpd");
				if (pid < 0)
					break;
				system_kill_process("busybox ntpd");
				usleep(10000);
				cnt++;
			} while (cnt < 20);

			if (qiwo_ipc->reset_timer == NULL) {
				qiwo_ipc->reset_timer = tutk->run_timer(1000, qiwo_reset_timer, (void *)qiwo_ipc);
			} else {
				qiwo_timer = qiwo_ipc->reset_timer;
				pthread_mutex_lock(&qiwo_timer->mutex);
				status = TUTK_TIMER_START;
				if (qiwo_timer->status == TUTK_TIMER_STOP)
					status = TUTK_TIMER_STOP;
				pthread_mutex_unlock(&qiwo_timer->mutex);
				if (status == TUTK_TIMER_STOP)
					tutk->change_timer(qiwo_timer, 1000);
			}
		}
	} else if (!strncmp(event, EVENT_PROCESS_END, strlen(EVENT_PROCESS_END))) {
		event_process = (struct event_process *)arg;
		if (!strncmp(event_process->comm, "muxer", strlen("muxer"))) {
			LOGD("event process end %s %s \n", (char *)event, event_process->comm);
		}
	}
	if (tutk->event_handle)
		tutk->event_handle(tutk, event, arg);

	return;
}

static int qiwo_bt_start_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	char cmd[32];
	int ret = TUTK_TIMER_STOP;

	pthread_mutex_lock(&qiwo_ipc->bt_mutex);
	if (qiwo_ipc->ble_recv) {
		if (qiwo_ipc->bt_mode == RUN_BLE_SERVER_CMD) {
			memset(cmd, 0, 32);
			cmd[0] = 3;
			cmd[4] = RUN_BLE_MESH_CMD;
			qiwo_send_bt_cmd(tutk, cmd, 7);
			qiwo_ipc->bt_mode = RUN_BLE_MESH_CMD;
		}
		qiwo_ipc->ble_recv = 0;
	} else {
		if (item_exist("bt.model") && (access("usr/bin/d304bt", F_OK) == 0)) {
			if (system_check_process("d304bt") < 0) {
				event_register_handler("qiwo_ble_data", 0, qiwo_ble_handle);
				system("d304bt server &");
				if (!tutk->reg_state) {
					qiwo_ipc->bt_mode = RUN_BLE_SERVER_CMD;
					tutk->play_audio(tutk, ALARM_VOICE);
				} else {
					qiwo_ipc->bt_mode = RUN_BLE_NONE;
					ret = TUTK_TIMER_START;
				}
			} else {
				if (qiwo_ipc->bt_mode != RUN_BLE_MESH_CMD) {
					memset(cmd, 0, 32);
					cmd[0] = 3;
					cmd[4] = RUN_BLE_MESH_CMD;
					qiwo_send_bt_cmd(tutk, cmd, 7);
					qiwo_ipc->bt_mode = RUN_BLE_MESH_CMD;
				}
			}
		}
	}
	pthread_mutex_unlock(&qiwo_ipc->bt_mutex);
	return ret;
}

static int qiwo_boot_timer(struct tutk_timer_t *tutk_timer, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	storage_info_t si;
	int tf_card_state = 0;
	char value[32];

	if (access(tutk->sd_mount_point, F_OK) == 0) {
		system_get_storage_info(tutk->sd_mount_point, &si);
		if (si.total > 0) {
			if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
				if (access("/dev/mmcblk1", F_OK) == 0)
					tf_card_state = 1;
			} else {
				if (access("/dev/mmcblk0", F_OK) == 0)
					tf_card_state = 1;
			}
		}
	}
	if (tf_card_state)
		event_send(EVENT_MOUNT, tutk->sd_mount_point, strlen(tutk->sd_mount_point) + 1);

	setting_get_string(SETTING_PREFEX, TUTK_RECORD_TYPE, value, 16);
	if (strcmp(tutk->rec_type, value) && strcmp(value, FILE_SAVE_ALARM)) {
		sprintf(tutk->rec_type, "%s", value);
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_RECORD, 0);
		tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
	}
	qiwo_ipc->booted = 1;

	return TUTK_TIMER_STOP;
}

static void qiwo_wifi_state_handle(char *event, void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = qiwo_save;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct net_info_t net_info;
	struct wifi_config_t *wifi_config_connect;
	struct tutk_session_t *tutk_session;
	char ssid[32], pwd[32];
	char value[128], cmd[128];
	int i, pid, cnt;
	struct list_head_t *l, *n;

	if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
		if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))
			 || !strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
			wifi_get_net_info(&net_info);
			sscanf(net_info.hwaddr, "%x:%x:%x:%x:%x:%x", (unsigned int *)(&value[0]), (unsigned int *)(&value[4]), (unsigned int *)(&value[8]), (unsigned int *)(&value[12]), (unsigned int *)(&value[16]), (unsigned int *)(&value[20]));
			sprintf(qiwo_ipc->wifi_addr, "%x%x%x%x%x%x", value[0], value[4], value[8], value[12], value[16], value[20]);
			if (item_exist("bt.model") && item_equal("bt.model", "bcm6212", 0)) {
				wifi_config_connect = wifi_sta_get_config();
				if (wifi_config_connect == NULL) {
					setting_remove_all(SETTING_PREFEX);
					qiwo_load_setting(tutk);
				} else {
					free(wifi_config_connect);
				}
				if (system_check_process("bsa_server") < 0)
				{
					LOGD("--->start bsa_server and d304bt \n");
					 if (access("/usr/bin/bsa_server", F_OK) == 0)
					 	system("start_bsa_server &");
				}
				if (!qiwo_ipc->bt_start_timer)
					qiwo_ipc->bt_start_timer = tutk->run_timer(4000, qiwo_bt_start_timer, (void *)qiwo_ipc);
				else
					tutk->change_timer(qiwo_ipc->bt_start_timer, 4000);
			}

		} else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
			wifi_config_connect = wifi_sta_get_config();
			if (wifi_config_connect == NULL) {
				if (!item_exist("bt.model")) {
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
		} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {

			if (tutk->wifi_sta_connected)
				return;

			if (!tutk->ntpd_state) {
				for (i=0; i<QIWO_NTPD_SERVER_NUM; i++) {
					sprintf(cmd, "busybox ntpd -nN");
					strcat(cmd, " -p ");
					if (!strcmp(qiwo_ipc->country, DEFAULT_COUNTRY))
						strcat(cmd, sNTPServerListChina[i]);
					else
						strcat(cmd, sNTPServerListForeign[i]);
					strcat(cmd, " -S eventsend &");
					system(cmd);
				}
			}
			tutk->wifi_sta_connected = 1;

			if (qiwo_ipc->ble_new_connect && !tutk->reset_flag && !tutk->reg_state && (qiwo_ipc->bt_mode == RUN_BLE_MESH_CMD)) {
				tutk->register_start = 1;
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_START_REG_DEV, 0);
			} else if (!tutk->process_run) {
				if (tutk->reg_state) {
					pthread_mutex_lock(&qiwo_ipc->bt_mutex);
					pthread_mutex_unlock(&qiwo_ipc->bt_mutex);
					tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_ENABLE_SERVER, 0);
					tutk->play_audio(tutk, WIFI_CONNECT_VOICE);
				} else {
					if (qiwo_ipc->board == EVB_BOARD) {
						tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_ENABLE_SERVER, 0);
						tutk->play_audio(tutk, WIFI_CONNECT_VOICE);
					}
				}
			}

			if  (tutk->reg_state) {
				if (qiwo_ipc->init_led[0] != 0)
					system_set_led(qiwo_ipc->init_led, 0, 0, 0);
				if (qiwo_ipc->reg_led[0] != 0) {
					i = 0;
					list_for_each_safe(l, n, &tutk->session_list) {
						tutk_session = container_of(l, struct tutk_session_t, list);
						if (tutk_session->video_status) {
							i = 1;
							break;
						}
					}
					if (i)
						system_set_led(qiwo_ipc->reg_led, 800, 500, 1);
					else
						system_set_led(qiwo_ipc->reg_led, 1, 0, 0);
				}
			}
			LOGD("qiwo_wifi_state_handle connect %d %x \n", tutk->reset_flag, qiwo_ipc->bt_mode);
		} else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {

			tutk->wifi_sta_connected = 0;
			if (qiwo_ipc->reg_led[0] != 0)
				system_set_led(qiwo_ipc->reg_led, 0, 0, 0);
			if (qiwo_ipc->init_led[0] != 0)
				system_set_led(qiwo_ipc->init_led, 1000, 800, 1);

			cnt = 0;
			do {
				pid = system_check_process("busybox ntpd");
				if (pid < 0)
					break;
				sprintf(cmd, "kill %d", pid);
				system(cmd);
				usleep(10000);
				cnt++;
			} while (cnt < 20);
			cnt = 0;
			do {
				pid = system_check_process("curl");
				if (pid < 0)
					break;

				sprintf(cmd, "kill %d", pid);
				system(cmd);
				usleep(20000);
				cnt++;
			} while (cnt < 20);
			LOGD("qiwo_wifi_state_handle disconnect %d \n", tutk->reg_state);
		}
	}
	if (tutk->event_handle)
		tutk->event_handle(tutk, event, arg);

	return;
}

static int qiwo_get_config(struct tutk_session_t *tutk_session, char *name, char *value)
{
	setting_get_string(SETTING_PREFEX, name, value, 64);
	return 0;
}

static int qiwo_save_config(struct tutk_session_t *tutk_session, char *name, char *value)
{
	int ret;

	ret = setting_set_string(SETTING_PREFEX, name, value);
	if (ret < 0)
		return ret;

	return 0;
}

static int qiwo_handle_ioctrl_cmd(struct tutk_session_t *tutk_session, unsigned int ioctrl_type, char *buf)
{
	struct tutk_common_t *tutk = (struct tutk_common_t *)tutk_session->priv;
	struct qiwo_ipc_t *qiwo_ipc = container_of(tutk, struct qiwo_ipc_t, qiwo_tutk);
	char response[512] = {0}, value[128];
	SMsgAVIoctrlSetEnvironmentReq *env_req = (SMsgAVIoctrlSetEnvironmentReq *)buf;
	SMsgAVIoctrlSetEnvironmentResp *env_resp = (SMsgAVIoctrlSetEnvironmentResp *)response;
	SMsgAVIoctrlSetMotionDetectResp *motion_dect_resp = (SMsgAVIoctrlSetMotionDetectResp *)response;
	SMsgAVIoctrlGetMotionDetectResp *momtion_get_resp = (SMsgAVIoctrlGetMotionDetectResp *)response;
	SMsgAVIoctrlSetMotionDetectReq *motion_dect_req;
	int ret = 0;
	enum cam_scenario scen = 0;

	switch (ioctrl_type) {
		case IOTYPE_USER_IPCAM_GETSPEAKERSTATUS_REQ:
			response[0] = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETSPEAKERSTATUS_RESP, response, sizeof(char));
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_BTCOMMAND_REQ:
			ret = (buf[1] << 8) | buf[2];
			ret += 3;
			memcpy(response, &ret, sizeof(int));
			memcpy(response + sizeof(int), buf, ret);
			qiwo_send_bt_cmd(tutk, response, ret + sizeof(int));
			sprintf(response, "1");
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_BTCOMMAND_RESP, response, sizeof(char));
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_BTCOMMAND_RESULT_REQ:
			if (qiwo_ipc->bt_scan_len) {
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_BTCOMMAND_RESULT_RESP, qiwo_ipc->bt_scan_result, qiwo_ipc->bt_scan_len);
				qiwo_ipc->bt_scan_len = 0;
			} else {
				sprintf(response, "0");
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_BTCOMMAND_RESULT_RESP, response, sizeof(char));
			}
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_GETMOTIONDETECT_REQ:
			tutk->get_config(tutk_session, TUTK_MOTION_DECT, value);
			momtion_get_resp->sensitivity = atoi(value);
			momtion_get_resp->channel = tutk_session->tutk_server_channel_id;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_GETMOTIONDETECT_RESP, (char *)momtion_get_resp, sizeof(SMsgAVIoctrlGetMotionDetectResp));
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_SETMOTIONDETECT_REQ:
			motion_dect_req = (SMsgAVIoctrlSetMotionDetectReq *)buf;
			if (motion_dect_req->sensitivity > 0) {
				tutk->save_config(tutk_session, TUTK_RECORD_TYPE, FILE_SAVE_ALARM);
				sprintf(tutk->rec_type, "%s", FILE_SAVE_ALARM);
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_STOP_RECORD, 0);
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_RECORD, 0);
			} else {
				tutk->save_config(tutk_session, TUTK_RECORD_TYPE, FILE_SAVE_FULLTIME);
				sprintf(tutk->rec_type, "%s", FILE_SAVE_FULLTIME);
				if (qiwo_ipc->alarm_record_timer) {
					tutk->stop_timer(qiwo_ipc->alarm_record_timer);
					qiwo_ipc->alarm_record_timer = NULL;
				}
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_RESTART_RECORD, 0);
				tutk->add_cmd(tutk, CMD_TUTK_COMMON, CMD_SERVER_START_RECORD, 0);
			}

			sprintf(value, "%d", motion_dect_req->sensitivity);
			ret = tutk->save_config(tutk_session, TUTK_MOTION_DECT, value);
			if (ret < 0)
				motion_dect_resp->result = 1;
			else
				motion_dect_resp->result = 0;
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SETMOTIONDETECT_RESP, (char *)motion_dect_resp, sizeof(SMsgAVIoctrlSetMotionDetectResp));
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_SET_WARNING_SOUND_REQ:
			if ((buf[0] == 0) || (buf[0] == 1)) {
				sprintf(value, "%d", buf[0]);
				tutk->save_config(tutk_session, PREVIEW_SOUND, value);
				qiwo_ipc->preview_sound = buf[0];
			} else {
				tutk->get_config(tutk_session, PREVIEW_SOUND, value);
				response[0] = atoi(value);
				qiwo_ipc->preview_sound = response[0];
			}
			avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_SET_WARNING_SOUND_RESP, response, sizeof(char));
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_START_RING:
			if (qiwo_ipc->preview_sound && !qiwo_ipc->ring_start_bug) {
				qiwo_ipc->ring_start_bug = 1;
				tutk->play_audio(tutk, ALARM_VOICE);
				if (!qiwo_ipc->preview_sound_timer)
					qiwo_ipc->preview_sound_timer = tutk->run_timer(1000, qiwo_preview_sound_timer, (void *)qiwo_ipc);
				else
					tutk->change_timer(qiwo_ipc->preview_sound_timer, 1000);
			}
			ret = TUTK_CMD_DONE;
			break;
		case IOTYPE_USER_IPCAM_START:
			if (qiwo_ipc->reg_led[0] != 0)
				system_set_led(qiwo_ipc->reg_led, 800, 500, 1);
			break;
		case IOTYPE_USER_IPCAM_STOP:
			if (qiwo_ipc->reg_led[0] != 0)
				system_set_led(qiwo_ipc->reg_led, 1, 0, 0);
			break;
		case IOTYPE_USER_IPCAM_SET_ENVIRONMENT_REQ:
			if (qiwo_ipc->board == D304_BOARD) {
				camera_get_scenario(CAMERA_SENSOR_NAME, &scen);
				if (env_req->mode == AVIOCTRL_ENVIRONMENT_NIGHT) {
					if (scen == CAM_DAY) {
						camera_monochrome_enable(CAMERA_SENSOR_NAME, 1);
						camera_set_scenario(CAMERA_SENSOR_NAME, CAM_NIGHT);
						qiwo_scene_mode_io(qiwo_ipc, CAM_NIGHT);
					}
				} else if (env_req->mode == AVIOCTRL_ENVIRONMENT_OUTDOOR) {
					if (scen == CAM_NIGHT) {
						qiwo_scene_mode_io(qiwo_ipc, CAM_DAY);
						camera_set_scenario(CAMERA_SENSOR_NAME, CAM_DAY);
						camera_monochrome_enable(CAMERA_SENSOR_NAME, 0);
					}
				}
				env_resp->result = 0;
				env_resp->channel = tutk_session->server_channel_id;
				avSendIOCtrl(tutk_session->server_channel_id, IOTYPE_USER_IPCAM_SET_ENVIRONMENT_RESP, (char *)env_resp, sizeof(SMsgAVIoctrlSetEnvironmentResp));
				ret = TUTK_CMD_DONE;
			}
			break;
		default:
			break;
	}

	return ret;
}

static void *qiwo_event_report_thread(void *arg)
{
	struct qiwo_ipc_t *qiwo_ipc = (struct qiwo_ipc_t *)arg;
	struct tutk_common_t *tutk = &qiwo_ipc->qiwo_tutk;
	struct event_cmd_t *event_cmd = NULL;
	struct list_head_t *l, *n;
	int ret, cnt;

	prctl(PR_SET_NAME, "qiwo_report");
	while (1) {

		pthread_mutex_lock(&qiwo_ipc->event_mutex);
		if (!list_empty(&qiwo_ipc->event_list)) {
			if (!tutk->ntpd_state || !tutk->wifi_sta_connected || !tutk->reg_state || tutk->reset_flag) {
				l = qiwo_ipc->event_list.next;
				event_cmd = container_of(l, struct event_cmd_t, list);
				if ((event_cmd->cmd_type == CMD_TUTK_QUIT) || (event_cmd->cmd_type == EVENT_STOP)) {
					list_del(&event_cmd->list);
				} else {
					pthread_mutex_unlock(&qiwo_ipc->event_mutex);
					usleep(100000);
					continue;
				}
			} else {
				l = qiwo_ipc->event_list.next;
				event_cmd = container_of(l, struct event_cmd_t, list);
				list_del(&event_cmd->list);
			}
		} else {
			event_cmd = NULL;
			pthread_cond_wait(&qiwo_ipc->event_cond, &qiwo_ipc->event_mutex);
		}
		pthread_mutex_unlock(&qiwo_ipc->event_mutex);

		if (event_cmd) {
			if (event_cmd->cmd_type == EVENT_QUIT) {
				free(event_cmd);
				LOGD("qiwo event report thread quit \n");
				break;
			} else if (event_cmd->cmd_type == EVENT_STOP) {
				system_kill_process("curl");
				free(event_cmd);
				event_cmd = NULL;
			}
		}

		cnt = 0;
		while (event_cmd != NULL) {
			if (!tutk->ntpd_state || !tutk->wifi_sta_connected || !tutk->reg_state) {
				usleep(100000);
				if (cnt >= 10) {
					free(event_cmd);
					event_cmd = NULL;
				}
			} else {
				qiwo_ipc->report_running = 1;
				ret = qiwo_report_event(qiwo_ipc, event_cmd->cmd_type, event_cmd->event);
				if (!ret || (cnt >= 3)) {
					free(event_cmd);
					event_cmd = NULL;
				}
			}
			cnt++;
		}
		qiwo_ipc->report_running = 0;
	}

	pthread_mutex_destroy(&qiwo_ipc->event_mutex);
	pthread_cond_destroy(&qiwo_ipc->event_cond);
	list_for_each_safe(l, n, &qiwo_ipc->event_list) {
		event_cmd = container_of(l, struct event_cmd_t, list);
		list_del(&event_cmd->list);
		free(event_cmd);
	}

	pthread_exit(0);
}

static void qiwo_common_init(struct tutk_common_t *tutk)
{
	struct qiwo_ipc_t *qiwo_ipc = container_of(tutk, struct qiwo_ipc_t, qiwo_tutk);
	struct timeval tv;
	time_t default_time;
	struct tm tm_default;
	int ret;

	if (item_exist("board.string.device") && item_equal("board.string.device", "qiwo304", 0))
		qiwo_ipc->board = D304_BOARD;
	else if (item_exist("board.string.device") && item_equal("board.string.device", "Q3FEVB", 0))
		qiwo_ipc->board = Q3FEVB_BOARD;
	else if (item_exist("board.string.device") && item_equal("board.string.device", "APOLLO3EVB", 0))
		qiwo_ipc->board = APOLLO3EVB_BOARD;
	else
		qiwo_ipc->board = EVB_BOARD;

	qiwo_load_setting(tutk);
	tm_default.tm_year = 2016 - 1900;
	tm_default.tm_mon = 0;
	tm_default.tm_mday = 1;
	tm_default.tm_hour = 8;
	tm_default.tm_min = 0;
	tm_default.tm_sec = 0;
	default_time = mktime(&tm_default);
	tv.tv_sec = (long)default_time;
	tv.tv_usec = 0;
	settimeofday(&tv, NULL);

	pthread_mutex_init(&qiwo_ipc->bt_mutex, NULL);
	sprintf(tutk->sd_mount_point, TUTK_TFCARD_MOUNT_PATH);
	sprintf(tutk->rec_path, "%s/%s", tutk->sd_mount_point, EVENT_RECORD_DIR);
	if (qiwo_ipc->board == D304_BOARD) {
		tutk->event_report = qiwo_event_report;
		tutk->scene_mode = DAYTIME_STATIC_SCENE;
		system_request_pin(D304_IRLEDGPIO);
		system_request_pin(D304_IRCUTAGPIO);
		system_request_pin(D304_IRCUTBGPIO);
		system_request_pin(D304_IRPWMGPIO);

		sprintf(tutk->rec_video_channel, DEFAULT_RECORD_CHANNEL);
		//video_disable_channel(DEFAULT_RECORD_CHANNEL);
		video_disable_channel(DEFAULT_HIGH_CHANNEL);
		video_disable_channel(DEFAULT_PREVIEW_CHANNEL);
		video_disable_channel(DEFAULT_LOW_CHANNEL);
	} else if (qiwo_ipc->board == Q3FEVB_BOARD || qiwo_ipc->board == APOLLO3EVB_BOARD) {
		tutk->scene_mode = DAYTIME_DYNAMIC_SCENE;
		sprintf(tutk->rec_video_channel, DEFAULT_RECORD_CHANNEL);
	} else {
		tutk->scene_mode = DAYTIME_DYNAMIC_SCENE;
		sprintf(tutk->rec_video_channel, DEFAULT_RECORD_CHANNEL);
		video_disable_channel(DEFAULT_RECORD_CHANNEL);
		video_disable_channel(DEFAULT_HIGH_CHANNEL);
		video_disable_channel(DEFAULT_PREVIEW_CHANNEL);
		video_disable_channel(DEFAULT_LOW_CHANNEL);
	}
	sprintf(tutk->play_audio_channel, NORMAL_CODEC_PLAY);
	sprintf(tutk->rec_audio_channel, NORMAL_CODEC_RECORD);
	tutk->capture_fmt.channels = NORMAL_CODEC_CHANNELS;
	tutk->capture_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	tutk->capture_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	tutk->capture_fmt.sample_size = 1000;
	tutk->play_fmt.channels = NORMAL_CODEC_CHANNELS;
	tutk->play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	tutk->play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	tutk->play_fmt.sample_size = 1024;

	tutk->play_audio(tutk, NULL);

	if (tutk->wifi_ap_set) {
		wifi_start_service(qiwo_wifi_state_handle);
		wifi_ap_enable();
	} else if (tutk->wifi_sta_set) {
		wifi_start_service(qiwo_wifi_state_handle);
		wifi_sta_enable();
	}

	//if (strcmp(tutk->rec_type, FILE_SAVE_ALARM))
		//sprintf(tutk->rec_type, FILE_SAVE_ALARM);

	INIT_LIST_HEAD(&qiwo_ipc->event_list);
	pthread_mutex_init(&qiwo_ipc->event_mutex, NULL);
	pthread_cond_init(&qiwo_ipc->event_cond, NULL);
	ret = pthread_create(&qiwo_ipc->event_report_thread, NULL, &qiwo_event_report_thread, (void *)qiwo_ipc);
	if (ret)
		ERROR("qiwo event_report_thread creat failed %d \n", ret);
	else
		pthread_detach(qiwo_ipc->event_report_thread);
	qiwo_ipc->boot_timer = tutk->run_timer(2 * 1000, qiwo_boot_timer, (void *)qiwo_ipc);

	event_register_handler(EVENT_VAMOVE, 0, qiwo_normal_handle);
	event_register_handler(EVENT_NTPOK, 0, qiwo_normal_handle);
	event_register_handler(NTPD_FAIL_EVENT, 0, qiwo_normal_handle);
	event_register_handler(EVENT_MOUNT, 0, qiwo_normal_handle);
	event_register_handler(EVENT_UNMOUNT, 0, qiwo_normal_handle);
	event_register_handler(EVENT_KEY, 0, qiwo_normal_handle);
	event_register_handler(EVENT_PROCESS_END, 0, qiwo_normal_handle);
	if (qiwo_ipc->board == D304_BOARD) {
		event_register_handler(EVENT_LIGHTSENCE_B, 0, qiwo_normal_handle);
		event_register_handler(EVENT_LIGHTSENCE_D, 0, qiwo_normal_handle);
	}
	event_send(EVENT_BOOT_COMPLETE, "d304main", strlen("d304main") + 1);

	return;
}

static void qiwo_common_deinit(struct tutk_common_t *tutk)
{
	if (item_exist("bt.model") && (access("usr/bin/d304bt", F_OK) == 0))
		system_kill_process("d304bt");

	return;
	if (tutk->wifi_ap_set) {
		wifi_ap_disable();
		wifi_stop_service();
	} else if (tutk->wifi_sta_set) {
		wifi_sta_disable();
		wifi_stop_service();
	}
}

static void qiwo_init_signal(void)
{
	struct sigaction action;

    action.sa_handler = qiwo_signal_handler;
    sigemptyset(&action.sa_mask);
    sigfillset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGTSTP, &action, NULL);
	sigaction(SIGSEGV, &action, NULL);
	//sigaction(SIGCHLD, &action, NULL);
	//sigaction(SIGBUS, &action, NULL);
	//sigaction(SIGINT, &action, NULL);

	return;
}

int main(int argc, char** argv)
{
	struct tutk_common_t *tutk;
	struct qiwo_ipc_t *qiwo_ipc;

	qiwo_init_signal();
	qiwo_ipc = malloc(sizeof(struct qiwo_ipc_t));
	if (!qiwo_ipc)
		return 0;

	memset(qiwo_ipc, 0, sizeof(struct qiwo_ipc_t));
	qiwo_save = qiwo_ipc;
	tutk = &qiwo_ipc->qiwo_tutk;

	tutk->init = qiwo_common_init;
	tutk->deinit = qiwo_common_deinit;
	tutk->get_config = qiwo_get_config;
	tutk->save_config = qiwo_save_config;
	tutk->get_register_result = qiwo_get_reg_result;
	tutk->handle_ioctrl_cmd = qiwo_handle_ioctrl_cmd;

	tutk_common_start(tutk);
	free(qiwo_ipc);

	return 0;
}
