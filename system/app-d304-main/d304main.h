#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <semaphore.h>

#include <qsdk/audiobox.h>
#include "include/AVFRAMEINFO.h"

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define list_for_each(pos, head) \
	for (pos = (head)->next ; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, npos, head) \
	for (pos = (head)->next, npos = pos->next ; pos != (head); pos = npos, npos = pos->next)

#define MAX_CLIENT_NUMBER    	16
#define TUTK_IPC_MODEL			"SY_MCP_IPC"
#define TUTK_IPC_VENDOR			"INFOTM"
#define TUTK_IPC_FW_VERSION		0x03100703
#define TUTK_TIMEZONE			"timezone"
#define TUTK_GMT				"gmt"
#define TUTK_DEFAULT_TIMEZONE	"ShangHai"
#define TUTK_MOTION_DECT		"motion_dect_sensitivity"
#define TUTK_RECORD_TYPE		"record_type"
#define TUTK_RECORD_DUATION		"record_duation"
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
#define TUTK_SERVER_TIMEOUT		5
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
#define ISP_DARK				36
#define ISP_LIGHT				40

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

#define AUDIO_FORMAT		MEDIA_CODEC_AUDIO_G711A

enum {
	EVB_BOARD = 0,
	D304_BOARD = 1,
	Q3FEVB_BOARD = 2,
	APOLLO3EVB_BOARD = 3,
};

enum {
	VIDEO_CHANNEL_1080P = 0,
	VIDEO_CHANNEL_720P = 1,
	VIDEO_CHANNEL_VGA = 2,
};

enum {
	CMD_TUTK_COMMON  = 0x2001,
	CMD_TUTK_RECORD  = 0x2002,
};

enum {
	CMD_SESSION_VIDEO_SEND  = 0x1001,
	CMD_SESSION_AUDIO_SEND  = 0x1002,
	CMD_SESSION_AUDIO_RECV  = 0x1003,
	CMD_SESSION_PLAYBACK  =  0x1004,
};

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

enum {
    CMD_TUTK_QUIT  = -1,
    CMD_ENABLE_SERVER = 1,
    CMD_DISABLE_SERVER = 2,
    CMD_SERVER_START_SESSION = 3,
    CMD_SERVER_STOP_SESSION = 4,
    CMD_SERVER_START_VIDEO = 5,
    CMD_SERVER_STOP_VIDEO = 6,
    CMD_SERVER_SET_VIDEO_RESOLUTION = 7,
    CMD_SERVER_START_AUDIO = 8,
    CMD_SERVER_STOP_AUDIO = 9,
    CMD_SERVER_RESTART_AUDIO = 10,
    CMD_CLIENT_START_AUDIO = 11,
    CMD_CLIENT_STOP_AUDIO = 12,
    CMD_CLIENT_RESTART_AUDIO = 13,
    CMD_CONNECTE_AP = 14,
    CMD_START_WIFI_SCAN = 15,
    CMD_FORGET_AP = 16,
    CMD_START_REG_DEV = 17,
    CMD_SERVER_START_PLAYBACK = 18,
    CMD_SERVER_PAUSE_PLAYBACK = 19,
    CMD_SERVER_STOP_PLAYBACK = 20,
    CMD_SERVER_START_RECORD = 21,
    CMD_SERVER_STOP_RECORD = 22,
    CMD_SERVER_RESTART_RECORD = 23,
    CMD_SERVER_RECORD_CHECK = 24,
    CMD_SERVER_REPORT_EVENT = 25,
    CMD_SERVER_CHANGE_SCENE = 26,
    CMD_TUTK_NONE = 27,
};

enum {
	EVENT_QUIT  = -1,
	EVENT_MOTIONDECTED = 1,
	EVENT_FILELIST = 2,
	EVENT_ADDFILE = 3,
	EVENT_DELFILE = 4,
	EVENT_STOP= 5,
};

enum {
	TUTK_TIMER_START = 0,
	TUTK_TIMER_STOP = 1,
	TUTK_TIMER_RESTART = 2,
};

struct list_head_t {
	struct list_head_t *next, *prev;
};

struct cmd_data_t {
	struct list_head_t list;
	int session_cmd;
	unsigned cmd_arg;
};

struct record_file_t {
	struct list_head_t list;
	char file_name[64];
};

struct event_cmd_t {
	struct list_head_t list;
	int cmd_type;
	char event[64];
};

struct tutk_session_t {
	struct list_head_t video_send_cmd_list;
	struct list_head_t audio_send_cmd_list;
	struct list_head_t audio_recv_cmd_list;
	struct list_head_t playback_cmd_list;
	struct list_head_t list;
	int session_id;
	int tutk_server_channel_id;
	int tutk_client_channel_id;
	int tutk_play_channel_id;
	int server_channel_id;
	int client_channel_id;
	int play_channel_id;
	int process_run;
	int ioctrl_cmd;
	int video_quality;
	int video_status;
	int local_play;
	unsigned int base_timestamp;
	pthread_t audio_thread;
	pthread_t audio_recv_thread;
	pthread_t video_thread;
	pthread_t play_thread;
	pthread_t ioctrl_thread;
    pthread_mutex_t io_mutex;
    pthread_cond_t io_cond;
    pthread_mutex_t audio_mutex;
    pthread_cond_t audio_cond;
    pthread_mutex_t audio_recv_mutex;
    pthread_cond_t audio_recv_cond;
    pthread_mutex_t video_mutex;
    pthread_cond_t video_cond;
    pthread_mutex_t play_mutex;
    pthread_cond_t play_cond;
    int thread_run_flag;
    char video_channel[16];
    char audio_send_channel[16];
    char audio_recv_channel[16];
    char dev_name[16];
    char play_file[96];
    char *video_buf;
    void *priv;
};

struct tutk_timer_t {
	struct list_head_t list;
	int base_cnt;
	int interval;
	int status;
	pthread_mutex_t mutex;
	int (*handle)(struct tutk_timer_t *tutk_timer, void *arg);
	void *priv;
};

struct tutk_common_t {
	struct list_head_t cmd_list;
	struct list_head_t rec_cmd_list;
	struct list_head_t timer_list;
	struct list_head_t record_list;
	struct list_head_t session_list;
	int wifi_ap_set;
	int wifi_sta_set;
	int wifi_sta_connected;
	int reg_state;
	int max_client_num;
	int process_run;
	int ntpd_state;
	int tf_card_state;
	int warning_sound;
	int rec_time_segment;
	int actual_session_num;
	int net_cmd_flag;
	int scene_mode;
	int main_start;
	int server_start;
	int reset_flag;
	int register_start;

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

	sem_t wifi_sem;
	sem_t thread_sem;
	pthread_t wifi_ap_thread;
	pthread_t server_login_thread;
	pthread_t server_listen_thread;
	pthread_t server_register_thread;
	pthread_t server_record_thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_t rec_mutex;
    pthread_cond_t rec_cond;
    pthread_mutex_t timer_mutex;
    pthread_mutex_t play_mutex;
    pthread_mutex_t rec_file_mutex;
    pthread_mutex_t register_mutex;

	struct tutk_session_t *play_session;
	struct tutk_timer_t *rec_timer;
	void *tutk_rec;
	void (*init)(struct tutk_common_t *tutk);
	void (*deinit)(struct tutk_common_t *tutk);
	int (*get_register_result)(struct tutk_common_t *tutk, char *uid, char *pwd);
	void (*event_report)(struct tutk_common_t *tutk, int event_type, char *event_arg);
	void (*add_cmd)(struct tutk_common_t *tutk, int cmd_type, int cmd, unsigned cmd_arg);
	void (*play_audio)(struct tutk_common_t *tutk, char *file);
	void (*event_handle)(struct tutk_common_t *tutk, char *event, void *arg);
	struct tutk_timer_t *(*run_timer)(int interval, int (*handle)(struct tutk_timer_t *tutk_timer, void *arg), void *arg);
	void (*change_timer)(struct tutk_timer_t *tutk_timer, int interval);
	void (*stop_timer)(struct tutk_timer_t *tutk_timer);

	int (*save_config)(struct tutk_session_t *tutk_session, char *name, char *value);
	int (*get_config)(struct tutk_session_t *tutk_session, char *name, char *value);
	int (*handle_ioctrl_cmd)(struct tutk_session_t *tutk_session, unsigned int ioctrl_type, char *buf);
};

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

static void __list_add(struct list_head_t * _new, struct list_head_t * prev, struct list_head_t * next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static void list_add(struct list_head_t *new, struct list_head_t *head)
{
	__list_add(new, head, head->next);
}

static void list_add_tail(struct list_head_t *_new, struct list_head_t *head)
{
	__list_add(_new, head->prev, head);
}

static void __list_del(struct list_head_t * prev, struct list_head_t * next)
{
	next->prev = prev;
	prev->next = next;
}

static void list_del(struct list_head_t *entry)
{
	__list_del(entry->prev, entry->next);
}

static int list_empty(struct list_head_t *head)
{
	return head->next == head;
}

void tutk_common_start(struct tutk_common_t *tutk);
