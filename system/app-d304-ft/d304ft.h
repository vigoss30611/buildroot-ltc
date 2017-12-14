#ifndef __SPV_APP_H__
#define __SPV_APP_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <semaphore.h>

#include <qsdk/audiobox.h>

#define BUFFER_SIZE 256
#define SERVER_PORT 17000 // define the defualt connect port id
#define LENGTH_OF_LISTEN_QUEUE 10 //length of listen queue in server

#define GREEN_LED_NAME		"green"
#define BLUE_LED_NAME		"blue"
#define RED_LED_NAME        "red"

#define IPC_MBX_MAX_SIZE 256

/*client and server*/
struct mypara
{
	int fd;
	char *addr;
	int port;
};

#define NORMAL_MODE 0
#define PCBA_MODE 1
#define BUFFER_SIZE 256

static char ipaddress[20];
static char gClose = 0;

#define D304_IRLEDGPIO 			164
#define D304_IRPWMGPIO 			85
#define D304_IRCUTAGPIO 		163
#define D304_IRCUTBGPIO 		126

#define DAYTIME_STATIC_SCENE	0
#define DAYTIME_DYNAMIC_SCENE	1
#define NIGHTTIME_STATIC_SCENE	2
#define NIGHTTIME_DYNAMIC_SCENE	3
#define DAYTIME					0
#define NIGHTTIME				1
#define STATIC_SCENE			0
#define DYNAMIC_SCENE			1
#define VIDEO_1080P_CHANNEL		0
#define VIDEO_720P_CHANNEL		1
#define VIDEO_VGA_CHANNEL		2
#define LIGHT_SENSOR_DARK		8

#define MAX_CLIENT_NUMBER    	16
#define TUTK_IPC_MODEL			"SY_MCP_IPC"
#define TUTK_IPC_VENDOR			"INFOTM"
#define TUTK_IPC_FW_VERSION		0x03100703
#define TUTK_TIMEZONE			"timezone"
#define TUTK_GMT				"gmt"
#define TUTK_DEFAULT_TIMEZONE	"ShangHai"
#define TUTK_MOTION_DECT		"motion_dect_sensitivity"
#define TUTK_RECORD_TYPE		"record_type"
#define FILE_SAVE_FULLTIME		"alltime"
#define FILE_SAVE_OFF			"off"
#define FILE_SAVE_ALARM			"alarm"
#define FILE_SAVE_ALARMSNAP		"alarmsnap"
#define REG_STATE				"REG_STATE"
#define TUTK_P2P_UID			"p2p_uid"
#define TUTK_P2P_ACC			"p2p_acc"
#define TUTK_P2P_PWD			"p2p_pwd"
#define TUTK_TFCARD_MOUNT_PATH	"/mnt/sd0"
#define LOCAL_UID_SETTING		"local_uid"
#define LOCAL_UID_NAME			"GNFZN81J5YNKA2YR111A"
#define LOCAL_ACC_SETTING		"local_acc"
#define LOCAL_ACC_NAME			"admin"
#define LOCAL_PWD_SETTING		"local_pwd"
#define LOCAL_PWD_VALUE			"aaaa8888"
#define WIFI_AP_ENABLE			"AP_ENABLE"
#define WIFI_STA_ENABLE			"STA_ENABLE"
#define DEFAULT_WIFI_SSID		"wifi.def.ssid"
#define DEFAULT_WIFI_PWD		"wifi.def.pwd"
#define COUNTRY_SETTING			"country"
#define DEFAULT_COUNTRY			"china"
#define WARNING_SOUND			"warning_sound"
#define	DEFAULT_RECORD_CHANNEL	"encrecord-stream"
#define	DEFAULT_HIGH_CHANNEL	"enc1080p-stream"
#define	DEFAULT_PREVIEW_CHANNEL	"enc720p-stream"
#define	DEFAULT_LOW_CHANNEL		"encvga-stream"
#define BT_CODEC_PLAY			"btcodec"
#define BT_CODEC_RECORD			"btcodecmic"
#define NORMAL_CODEC_PLAY		"default"
#define NORMAL_CODEC_RECORD		"default_mic"
#define CAMERA_SENSOR_NAME		"isp"

#define TUTK_BASIC_TIMER_INTERVAL	500
#define TUTK_DEFAULT_GMT		480
#define AUDIO_BUF_SIZE			1024
#define VIDEO_BUF_SIZE			1024*600
#define VIDEO_1080P_WIDTH		1920
#define VIDEO_1080P_HEIGHT		1088
#define VIDEO_720P_WIDTH		1280
#define VIDEO_720P_HEIGHT		720
#define VIDEO_VGA_WIDTH			640
#define VIDEO_VGA_HEIGHT		360

#define DAYTIME_STATIC_SCENE	0
#define DAYTIME_DYNAMIC_SCENE	1
#define NIGHTTIME_STATIC_SCENE	2
#define NIGHTTIME_DYNAMIC_SCENE	3
#define DAYTIME					0
#define NIGHTTIME				1
#define STATIC_SCENE			0
#define DYNAMIC_SCENE			1
#define VIDEO_1080P_CHANNEL		0
#define VIDEO_720P_CHANNEL		1
#define VIDEO_VGA_CHANNEL		2
#define LIGHT_SENSOR_DARK		8
#define LIGHT_SENSOR_LIGHT		40
#define ISP_LIGHT				88

#define NORMAL_CODEC_SAMPLERATE		16000
#define NORMAL_CODEC_BITWIDTH		32
#define NORMAL_CODEC_CHANNELS		2
#define BT_CODEC_SAMPLERATE			8000
#define BT_CODEC_BITWIDTH			16
#define BT_CODEC_CHANNELS			1

#define FPS_PRINT_INTERNAL_TIME	8
#define TUTK_CMD_DONE			0x5a
#define MAX_SEND_ALARMS			4096
#define TUTK_DEL_FILE_THRESHHOLD	500
#define MAX_CHANNEL							4
#define VIDEO_THREAD_RUN					0
#define AUDIO_CAPTURE_THREAD_RUN			1
#define AUDIO_PLAY_THREAD_RUN				2
#define IOCTRL_THREAD_RUN					3
#define PLAYBACK_THREAD_RUN					4
#define ALL_THREAD_RUN_FLAGS				0xf

#define EVENT_UNREADED  			0
#define EVENT_READED 				1
#define EVENT_NORECORD  			2
#define EVENT_BAIDU_CLOUD_NOTREAD	3
#define EVENT_BAIDU_CLOUD_READ  	4
#define EVENT_PICTURE_UNREAD  		5
#define EVENT_PICTURE_READ  		6

#define AUDIO_FORMAT		MEDIA_CODEC_AUDIO_PCM

enum {
    CMD_SESSION_QUIT  = 0,
    CMD_SESSION_RECV_IOCTRL = 1,
    CMD_SESSION_START_VIDEO = 2,
    CMD_SESSION_SEND_VIDEO = 3,
    CMD_SESSION_RECV_VIDEO = 4,
    CMD_SESSION_STOP_VIDEO = 5,
    CMD_SESSION_SET_RESOLUTION = 6,
    CMD_SESSION_START_AUDIO = 7,
    CMD_SESSION_SEND_AUDIO = 8,
    CMD_SESSION_RECV_AUDIO = 9,
    CMD_SESSION_RESTART_AUDIO = 10,
    CMD_SESSION_STOP_AUDIO = 11,
    CMD_SESSION_PAUSE_VIDEO = 12,
    CMD_SESSION_RESTART_VIDEO = 13,
    CMD_SESSION_GET_FRAME = 14,
    CMD_SESSION_CHANGE_SCENE = 15,
    CMD_SESSION_NONE = 16,
};

static struct v_bitrate_info record_scene_info = {
	.bitrate = 280000,
	.qp_min = 20,
	.qp_max = 40,
	.qp_hdr = -1,
	.qp_delta = -4,
	.gop_length = 15,
	.picture_skip = 0,
};

struct v_bitrate_info gernel_scene_info[2][2][3] = {
	[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 400000,
		.qp_min = 20,
		.qp_max = 45,
		.qp_hdr = -1,
		.qp_delta = -10,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 160000,
		.qp_min = 20,
		.qp_max = 42,
		.qp_hdr = -1,
		.qp_delta = -10,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 98000,
		.qp_min = 20,
		.qp_max = 40,
		.qp_hdr = -1,
		.qp_delta = -8,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 666000,
		.qp_min = 20,
		.qp_max = 42,
		.qp_hdr = -1,
		.qp_delta = -4,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 280000,
		.qp_min = 20,
		.qp_max = 40,
		.qp_hdr = -1,
		.qp_delta = -4,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[DAYTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 130000,
		.qp_min = 20,
		.qp_max = 38,
		.qp_hdr = -1,
		.qp_delta = -4,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 400000,
		.qp_min = 20,
		.qp_max = 38,
		.qp_hdr = -1,
		.qp_delta = -10,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 160000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -10,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 98000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -10,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL] = {
		.bitrate = 666000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -6,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL] = {
		.bitrate = 280000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -7,
		.gop_length = 15,
		.picture_skip = 0,
	},
	[NIGHTTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL] = {
		.bitrate = 130000,
		.qp_min = 20,
		.qp_max = 32,
		.qp_hdr = -1,
		.qp_delta = -6,
		.gop_length = 15,
		.picture_skip = 0,
	}
};

struct test_common_t {
	int wifi_ap_set;
	int wifi_sta_set;
	int wifi_sta_connected;
	int tf_card_state;
	int ble_new_connect;
	int scene_mode;
	int reset_key;
	int gTestMode;
	int video_process_run;
	int audio_cmd;
	int video_cmd;

	char name[64];
    char ssid[64];
    char pswd[64];
    char hwaddr[32];
    char ipaddr[32];

    char bt_addr[64];
    char bt_scan_result[256];
    int bt_scan_len;

    char sd_mount_point[16];
	char local_uid[32];
	char local_acc[16];
	char local_pwd[16];
	char reg_uid[32];
	char reg_acc[16];
	char reg_pwd[16];
	char rec_path[32];
	char rec_type[16];
	char rec_video_channel[32];
	char rec_audio_channel[32];
	char play_audio_channel[32];

	audio_fmt_t capture_fmt;
	audio_fmt_t play_fmt;
	
	pthread_t socketThread;
	pthread_t ledThread;
};

struct test_common_t test;

#define FACTORY_TEST_DEBUG 1

#if FACTORY_TEST_DEBUG
#define  LOGD(...)   printf(__VA_ARGS__)
#else
#define  LOGD(...)
#endif

#define  LOGE(...)   printf(__VA_ARGS__)

#endif