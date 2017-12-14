/*
*********************************************************************************************************
*
*                       Copyright (c) 2012 Hangzhou QianMing Technology Co.,Ltd
*                                     Red Bull API Library
*
*********************************************************************************************************
*/
#ifndef __NSYS_CLI_H__
#define __NSYS_CLI_H__

#include "libtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*
*       Macro Defines
*
**********************************************************************
*/

#ifdef WIN32
// Pascal compatible
#define CLI_STDCALL __stdcall
#define CLI_CALLBACK CALLBACK
#else
#define CLI_STDCALL
#define CLI_CALLBACK
#endif

#define CLI_MAX_CHNUM		64
#define CLI_MAX_ALARM_INNUM 32
#define CLI_MAX_ALARM_OUTNUM 16
#define CLI_MAX_HARDDISK_NUM 16

#define CLI_NAME_LEN		32
#define CLI_ADDR_LEN		64
#define CLI_MAX_PATH		256
#define CLI_ID_LEN			32
#define CLI_URL_LEN			128
#define CLI_USER_LEN		16
#define CLI_MAC_LEN			6
#define CLI_LOG_LEN			20
#define CLI_MANU_PREFIX_LEN	8

#define MAX_MANAGE_HOST		3 /* 最大管理主机数 */
#define MAX_UART_NUM		4 /* 最大串口数 */
#define MAX_OSD_NUM			8 /* 最大OSD数 */
#define MAX_CALLBACK_DATA_LEN 1024 /* 一次心跳传输的最大数据长度 */
#define DISK_DEV_LEN		16 /* 磁盘设备名长度 */

#define MAX_DISK_PARTNUM 4 /*最大用户分区数*/

#define MAX_USER_ACCOUNT	8 /* 最大用户数 */
#define MAX_ONLINE_USER_NUM	16 /* 最大在线用户数 */
#define MAX_IP_FILTER_NUM	20 /* 最大IP过滤条数 */
#define MAX_REC_BACKUP_NUM	64 /* 最大备份个数 */

#define VIDEO_H264_IFRAME	0x21 /* H.264 I帧 */
#define VIDEO_H264_BFRAME	0x22 /* H.264 B帧 */
#define VIDEO_H264_PFRAME	0x23 /* H.264 P帧 */

#define AUDIO_RAW_PCM		0x81 /* pcm */
#define AUDIO_G711_A		0x91 /* G711.a */
#define AUDIO_G711_U		0x92 /* G711.μ */
#define AUDIO_IMA_ADPCM		0xA1 /* IMA ADPCM */
#define AUDIO_DVI4_ADPCM	0xA2 /* DVI4 ADPCM */
#define AUDIO_G726_ADPCM	0xA3 /* G726 */

/* 设备返回的错误定义 */
#define ERR_NO_ERROR		0 /* 没有错误 */
#define ERR_NO_RIGHT		1 /* 无操作权限 */
#define ERR_NO_EXIST		2 /* 结果不存在 */
#define ERR_NO_MEMORY		3 /* 设备内存不足 */
#define ERR_NO_SUPPORT		4 /* 操作不支持 */
#define ERR_SYS_BUSYNOW		5 /* 系统正忙 */
#define ERR_MUST_REBOOT		6 /* 需要重启 */
#define ERR_CONFIG_RDWR		7 /* 配置读写错误 */
#define ERR_INVALID_CMD		8 /* 无效命令 */
#define ERR_INVALID_PARAM	9 /* 参数错误 */
#define ERR_TOO_MANY_ITEMS	10 /* 保存参数项太多 */
#define ERR_USER_PASSWORD	11 /* 无效用户 */
#define ERR_USER_NO_EXIST	12 /* 用户不存在 */
#define ERR_ADMIN_DONT_DEL	13 /* 管理员不能删除 */
#define ERR_USER_AREADY_EXIST 14 /* 用户已存在 */
#define ERR_TOO_MANY_USERS	15 /* 添加的用户超限 */
#define ERR_VERIFY_OUT_TIMES 16 /* 验证超过次数 */
#define ERR_REC_CREATE		17 /* 创建录像失败 */
#define ERR_REC_NO_DISK		18 /* 录像无可用硬盘 */
#define ERR_REC_NO_STOP		19 /* 录像还未停止 */
#define ERR_RESOURCE_NOT_ENOUGH 20 /* 资源不足 */
#define ERR_REC_IN_BACKUP	21 /* 录像还在备份 */
#define ERR_REC_NO_BACKUP	22 /* 没有进行备份 */
#define ERR_BACKUP_NO_CANCEL 23 /* 备份无法取消 */
#define ERR_DISK_NO_READY	24 /* 磁盘初始化未完成 */
#define ERR_IPPORT_IN_USE	25 /* IP端口正被使用 */

/* CLI错误，即非设备返回的错误 */
#define ERR_CLI_NO_INIT		100 /* 未调用库初始化 */
#define ERR_CLI_INACTIVE	101 /* 句柄的连接已中断 */
#define ERR_CLI_TIME_OUT	102 /* 操作超时 */
#define ERR_CLI_PARAMETER	103 /* 参数错误 */
#define ERR_CLI_OUTOF_MEM	104 /* 内存不足 */
#define ERR_CLI_SOCK_RDWR	105 /* socket访问错误 */
#define ERR_CLI_FILE_RDWR	106 /* 文件访问错误 */
#define ERR_CLI_SOCK_CLOSE	107 /* socket连接关闭 */


enum {
	EVENT_ALL_EVENTS      = 0, /* 事件掩码设置表示全部事件 */
	/* 设备事件 */
	EVENT_VIDEO_REC       = LIB_BIT00, /* 视频录像，pevent为channel_state_t*类型 */
	EVENT_VIDEO_MD        = LIB_BIT01, /* 视频动检，pevent为channel_state_t*类型 */
	EVENT_VIDEO_LOSS      = LIB_BIT02, /* 视频丢失，pevent为channel_state_t*类型 */
	EVENT_VIDEO_COVER     = LIB_BIT03, /* 视频遮挡，pevent为channel_state_t*类型 */
	EVENT_VIDEO_BITRATE   = LIB_BIT04, /* 视频码率，pevent为stream_bitrate_t*类型 */
	EVENT_ALARM_INPUT     = LIB_BIT05, /* 报警输入，pevent为channel_state_t*类型 */
	EVENT_ALARM_OUTPUT    = LIB_BIT06, /* 报警输出，pevent为channel_state_t*类型 */
	EVENT_NETWORK_CONN    = LIB_BIT07, /* 网络连接，pevent为network_state_t*类型 */
	EVENT_DISK_STATE      = LIB_BIT08, /* 磁盘状态，pevent为disk_state_t*类型 */
	EVENT_REC_BACKUP      = LIB_BIT09, /* 本机录像备份，pevent为int*类型，取值为backup_state_e */
	EVENT_REC_PLAYBACK    = LIB_BIT10, /* 本机录像回放，pevent为playback_state_t*类型 */
	EVENT_SYS_CONFIG      = LIB_BIT11, /* 系统设置，pevent为config_state_t*类型 */
	EVENT_SYS_CONTROL     = LIB_BIT12, /* 系统控制，pevent为int*类型 */
	EVENT_SYS_UPGRAND     = LIB_BIT13, /* 系统升级，pevent为progress_state_t*类型，state为0 进度通知 1 文件异常 2 固件不匹配 3 升级被取消 4 升级固件重复 */
	EVENT_SYS_EXCEPTION   = LIB_BIT14, /* 系统异常，pevent为sys_exception_t*类型 */
	EVENT_SYS_EXIT        = LIB_BIT15, /* 系统退出，pevent为int*类型，取值为sys_exit_e */
	EVENT_USER_BREAK      = LIB_BIT16, /* 用户断开该连接，pevent为NULL，收到该事件后应该关闭连接 */
	EVENT_AVC_ONLINE      = LIB_BIT17, /* 设备上线，pevent为cli_dev_t类型 */
	EVENT_AVC_CONNECT     = LIB_BIT18, /* 音视频通道连接，pevent为channel_state_t*类型，state取值为conn_state_e */
	EVENT_SYS_INTERNAL    = LIB_BIT19, /* 内部事件，用户不使用 */
	EVENT_VIDEO_LOSS_EX   = LIB_BIT20, /* 视频丢失扩展，pevent为video_state_t*类型 */
	EVENT_AVC_PLAY              = LIB_BIT21,/*本机实时流播放, pevent为playback_state_t*类型 */
	
	/* 连接事件 */
	EVENT_CONN_RECONNECT  = LIB_BIT24, /* 设备连接恢复 */
	EVENT_CONN_DISCONNECT = LIB_BIT25, /* 设备连接中止，pevent为NULL */
	EVENT_PLAY_RECONNECT  = LIB_BIT26, /* 播放连接恢复，phandle为播放句柄 */
	EVENT_PLAY_DISCONNECT = LIB_BIT27 /* 播放连接中止，phandle为播放句柄，pevent为NULL */
};

typedef struct {
	int ch; /* 通道 */
	int state; /* 状态 0表示关闭，否则触发 */
} channel_state_t;

typedef struct {
	int ch; /*通道*/
	int type; /*视频类型video_input_e*/
}vinput_type_t;

typedef struct {
	int ch; /* 通道 */
	int streamindex; /* 流索引号 */
	unsigned int streambps; /* 流码率 */
} stream_bitrate_t;

enum network_state_e {
	NETWORK_TYPE_NONE, /* 无 */
	NETWORK_TYPE_ETHERNET, /* 以太网 */
	NETWORK_TYPE_PPPOE, /* PPPOE拨号 */
	NETWORK_TYPE_WIFI, /* WIFI连接 */
	NETWORK_TYPE_WI3G /* 3G连接 */
};

typedef struct {
	int type; /* 类型，network_state_e定义 */
	int state; /* 状态 0表示正常，否则异常 */
} network_state_t;

enum disk_state_e {
	DISK_STATE_FORMAT, /* 格式化 */
	DISK_STATE_INSERT, /* 移动磁盘插入 */
	DISK_STATE_REMOVE, /* 移动磁盘拔除 */
	DISK_STATE_READY /* 磁盘已就绪，允许录像或回放（整体状态，忽略磁盘号），否则将返回ERR_DISK_NO_READY错误 */
};

typedef struct {
	int state; /* 状态， disk_state_e定义 */
	int diskindex; /* 磁盘号 */
	int formatprecent; /* 完成百分比，-1表示格式化失败 */
} disk_state_t;

typedef struct {
	int state; /* 状态 0表示正常，否则异常 */
	int precent; /* 完成百分比 */
} progress_state_t;

enum backup_state_e {
	BACKUP_COMPLETE, /* 备份完成 */
	BACKUP_CANCEL, /* 备份取消 */
	BACKUP_DEVICE_ERR, /* 备份设备异常 */
	BACKUP_INTERNAL_ERR, /* 备份内部错误 */
	BACKUP_FILE_OPT_ERR, /* 备份文件操作异常 */
	BACKUP_DVD_ERR /* DVD刻录异常 */
};

typedef struct {
	int ch; /* 通道 */
	int win; /* 窗口 */
	int state; /* 状态 0 播放结束，1 重新播放 */
} playback_state_t;

typedef struct {
	int ch; /* 通道 */
	int config; /* 见CONFIG_DEVICE_INFO等定义 */
} config_state_t;

enum sys_exit_e {
	EXIT_REBOOT, /* 重启退出 */
	EXIT_POWER_OFF /* 关机退出 */
};

enum sys_exception_e {
	EXP_DISK_ERR, /* 磁盘错误 异常代码 0 正常；1 录像硬盘满 */
	EXP_SIGNAL_ERR, /* 视频信号异常 异常代码 0 正常；1 信号异常 */
	EXP_NETWORK_ERR /* 网络异常 异常代码 0 正常；1 网络异常 */
};

typedef struct {
	int exp_type; /* 异常类型，sys_exception_e定义 */
	int exp_code; /* 异常代码，见具体异常类型定义 */
} sys_exception_t;

enum conn_state_e {
	CONN_STATE_DISCONNECT, /* 连接断开（未连接） */
	CONN_STATE_CONNECT, /* 连接上（已连接） */
	CONN_STATE_CONNECTING, /* 连接中 */
	CONN_STATE_REFUSED, /* 连接被拒绝 */
	CONN_STATE_AUTHORIZE_ERR, /* 用户验证错误 */
	CONN_STATE_PARAMTER_ERR /* 参数错误 */
};


typedef struct {
	char dev_model[16]; /* 产品型号，以0结尾的ASSIC字符串 */
	char serial_no[16]; /* 序列号，以0结尾的ASSIC字符串 */
	char hardware_ver[16]; /* 硬件版本号，以0结尾的ASSIC字符串 */
	char software_ver[16]; /* 软件版本号，以0结尾的ASSIC字符串 */
	char issue_date[16]; /* 固件发布日期，以0结尾的ASSIC字符串 */
	unsigned int dev_ability; /* 设备能力 */
	unsigned char total_ch_num; /* 设备通道数 */
	unsigned char analog_ch_num; /* 模拟通道数 */
	unsigned char digital_ch_num; /* 数字通道数 */
	unsigned char disk_num; /* 硬盘数 */
	unsigned char audio_num; /* 音频数 */
	unsigned char alarmin_num; /* 报警输入数 */
	unsigned char alarmout_num; /* 报警输出数 */
	unsigned char ethernet_num; /* 以太网口数 */
	char res[32]; /* 保留 */
} dev_info_t;



enum {
	RETURN_WAIT_MOST, /* 等待所有可能命令返回（默认） */
	RETURN_WAIT_NORMAL, /* 等待主要的命令返回，例如预览启动、播放启动等 */
	RETURN_WAIT_LEAST /* 等待必须的命令返回，最低限度的等待 */
};



enum {
	KEEP_ALIVE_SEND,
	KEEP_ALIVE_RECV
};

typedef struct {
	int state; /* 发送或接收状态，KEEP_ALIVE_SEND或KEEP_ALIVE_RECV  */
	void *pdata; /* 数据缓冲区 */
	size_t len; /* 数据长度 */
} keep_alive_t;


/* 通道工作状态 */
typedef struct {
	unsigned char connstate; /* 通道连接状态，conn_state_e类型 */
	unsigned char recstate; /* 通道录像状态，0表示未录像；1表示正在录像 */
	unsigned char videosignal; /* 通道视频状态，0表示无视频；1表示有视频 */
	unsigned char videodetect; /* 通道检测状态（包括智能分析），0表示无事件；1表示动检 */
} ch_work_state_t;

enum {
	HDD_STATE_NORMAL, /* 状态正常 */
	HDD_STATE_INVALID, /* 非法磁盘 */
	HDD_STATE_STANDBY, /* 休眠状态 */
	HDD_STATE_READONLY /* 磁盘只读 */
};

typedef struct {
	int ch; /* 通道 */
	int state; /* 状态 0表示关闭，否则为类型video_input_e */
} video_state_t;



/* 通道工作状态扩展 */
typedef struct {
	unsigned int connstate; /* 通道连接状态，conn_state_e类型 */
	unsigned int recstate; /* 通道录像状态，0表示未录像；1表示正在录像 */
	unsigned int videostate; /* 通道视频状态，0表示有视频；否则为类型video_input_e */
	unsigned int videodetect; /* 通道检测状态（包括智能分析），0表示无事件；1表示动检 */
	int res[4];
} ch_work_state_ex_t;

/* 系统状态扩展 */
typedef struct {
	unsigned char alarmin[CLI_MAX_ALARM_INNUM]; /* 报警输入状态，0表示未报警；1表示报警 */
	unsigned char alarmout[CLI_MAX_ALARM_OUTNUM]; /* 报警输出状态，0表示未启动；1表示启动 */
	unsigned char hddstate[CLI_MAX_HARDDISK_NUM]; /* 硬盘状态，HDD_STATE_NORMAL ~ HDD_STATE_READONLY */
	ch_work_state_ex_t chstate[CLI_MAX_CHNUM]; /* 通道工作状态 */
	unsigned char res[64];
} cli_work_state_ex_t;

/* 系统状态 */
typedef struct {
	unsigned char alarmin[CLI_MAX_ALARM_INNUM]; /* 报警输入状态，0表示未报警；1表示报警 */
	unsigned char alarmout[CLI_MAX_ALARM_OUTNUM]; /* 报警输出状态，0表示未启动；1表示启动 */
	unsigned char hddstate[CLI_MAX_HARDDISK_NUM]; /* 硬盘状态，HDD_STATE_NORMAL ~ HDD_STATE_READONLY */
	ch_work_state_t chstate[CLI_MAX_CHNUM]; /* 通道工作状态 */
} cli_work_state_t;


enum {
	ATTRIB_PTZ_CONFIG        = LIB_BIT00, /* 支持云台配置 */
	ATTRIB_PTZ_CONTROL       = LIB_BIT01, /* 支持云台控制 */
	ATTRIB_VIDEO_LOSS        = LIB_BIT02, /* 支持视频丢失 */
	ATTRIB_MOTION_DETECT     = LIB_BIT03, /* 支持移动侦测 */
	ATTRIB_RECORD_OSD        = LIB_BIT04, /* 支持录像OSD */
	ATTRIB_DIGITAL_WATERMARK = LIB_BIT05, /* 支持数字水印 */
	ATTRIB_VIDEO_ROTATE      = LIB_BIT06, /* 支持视频旋转 */
	ATTRIB_VIDEO_EFFECT      = LIB_BIT07, /* 支持视频效果调节 */
	ATTRIB_PRIVACY_AREA      = LIB_BIT08, /* 支持隐私区域 */
	ATTRIB_IRCUT_FILTER      = LIB_BIT09, /* 支持红外过滤 */
	ATTRIB_WIDE_DYNAMIC      = LIB_BIT10, /* 支持宽动态 */
	ATTRIB_WHITE_BANLANCE    = LIB_BIT11, /* 支持白平衡 */
	ATTRIB_TIME_COFIG        = LIB_BIT12, /* 支持时间设置，IPC时间同步 */
	ATTRIB_JPEG_SNAPSHOT     = LIB_BIT13, /* 支持jpeg快照 */
	ATTRIB_NETWORK_CONFIG    = LIB_BIT14, /* 支持网络设置，IPC网络参数 */
	ATTRIB_AUDIO_SOURCE_FROM = LIB_BIT15 /* 支持音频源选择 */
};

typedef struct {
	int audionum; /* 音频流数，为0表示无音频 */
	int videonum; /* 视频流数 */
	unsigned int chattrib; /* 通道属性 */
	char res[4];
} channel_info_t;



typedef struct {
	int win; /* 窗口号 */
	lib_rect_t rc; /* 窗口位置和尺寸，不能与其他窗口重叠（除非是隐藏的窗口） */
} win_rect_t;

typedef struct {
	int ch; /* 通道 */
	int index; /* 流的索引号 */
	int win; /* 窗口 */
} cli_preview_t;


typedef struct {
	int ch; /* 通道 */
	int win; /* 窗口 */
	int rectype; /* 录像类型，REC_TYPE_ALL等 */
	lib_duration_t playdur; /* 播放时间 */
} cli_playback_t;


enum {
	PLAY_CMD_START, /* 开始或恢复播放，param不为0时恢复正常播放状态 */
	PLAY_CMD_PAUSE, /* 暂停播放，param保留为0 */
	PLAY_CMD_STOP, /* 停止播放，释放资源，param保留为0 */
	PLAY_CMD_SEEK, /* 按时间定位，playmom为定位时间 */
	PLAY_CMD_STEP, /* 单帧连续播放，param保留为0 */
	PLAY_CMD_SLOW, /* 慢放，param为慢放倍率 */
	PLAY_CMD_FAST, /* 快放，param为快放倍率 */
	PLAY_CMD_SOUND, /* 声音控制，param为0静音，为1打开声音 */
	PLAY_CMD_GET_VOLUME, /* 音量获取，忽略窗口号，param为音量大小[OUT] */
	PLAY_CMD_SET_VOLUME, /* 音量调节，忽略窗口号，param为音量大小[0, 100] */
	PLAY_CMD_REVERSE, /* 倒放，param保留为0 */
	PLAY_CMD_ONEFRAME, /* 播放一帧后暂停，param保留为0 */
	PLAY_CMD_POSITION /* 当前播放位置，playmom为播放时间[OUT] */
						/* param对于cli_native_play_control调用传入0表示获取当前窗口播放位置，传入1表示同步模式播放位置[IN] */
						/* param对于cli_remote_play_control调用返回0表示正在播放中，返回1表示播放已结束[OUT] */
};

typedef struct {
	int win; /* 窗口 */
	int param; /* 控制参数 */
	lib_moment_t playmom; /* 播放时间 */
} native_play_control_t;


enum {
	FLAGS_FRAME_COMPLETE = LIB_BIT00, /* 帧边界，一帧可能分为多个数据块 */
	FLAGS_FRAME_FINISH   = LIB_BIT01 /* 帧结束，录像数据已结束，该数据不是实际数据（依然要释放） */
};

typedef struct {
	int datatype; /* 数据类型，VIDEO_H264_IFRAME或AUDIO_IMA_ADPCM等 */
	int dataflags; /* 数据标志，FLAGS_FRAME_COMPLETE等 */
	unsigned int timestamp; /* 时间戳(ms)，任意起始，循环递增 */
	char *databuf; /* 数据缓冲区指针 */
	size_t len; /* 数据长度 */
	void *pnode; /* 帧队列指针，数据释放接口用，不能修改 */
} stream_data_t;



enum {
	DEV_SD_CARD,    /* SD卡 */
	DEV_USB_DISK,   /* U盘 */
	DEV_ESATA_DISK, /* ESATA硬盘 */
	DEV_DVD_WRITER  /* DVD刻录机 */
};

/* 备份设备 */
typedef struct {
	int devtype; /* 设备类型 DEV_SD_CARD ~ DEV_DVD_WRITER */
	char devname[CLI_NAME_LEN]; /* 设备名称 */
	unsigned int freecapacity; /* 可用容量(MB) */
	unsigned int totalcapacity; /* 总容量(MB) */
} backup_dev_t;


typedef struct {
	int ch; /* 录像通道 */
	int type; /* 录像类型 REC_TYPE_ALL ~ REC_TYPE_ALARM */
	lib_duration_t dur; /* 录像时间段 */
} rec_duration_t;


enum {
	CONFIG_DEVICE_INFO, /* 设备信息，pconfig为dev_info_t*类型（只读） */
	CONFIG_DEVICE_LOCAL, /* 设备本地设置，pconfig为dev_local_t*类型 */
	CONFIG_DEVICE_TIME, /* 设备时间，pconfig为lib_moment_t*类型 */
	CONFIG_DATETIME_MODE, /* 时间日期模式，pconfig为date_time_mode_t*类型 */
	CONFIG_DST_TIME, /* 夏令时时间，pconfig为cli_dst_t*类型 */
	CONFIG_USER_INFO, /* 用户账号信息，pconfig为user_info_t*类型（只读，当前为管理员才能获得账号密码） */
	CONFIG_ONLINE_USER, /* 在线用户信息，pconfig为online_user_t*类型 */
	CONFIG_MANAGE_HOST, /* 管理主机，pconfig为manage_host_t*类型 */
	CONFIG_VIDEO_SOURCE, /* 视频源参数，pconfig为video_source_t*类型 */
	CONFIG_VIDEO_EFFECT, /* 视频效果参数，pconfig为effect_value_t*类型 */
	CONFIG_VIDEO_ENCODE, /* 视频编码参数，pconfig为video_encode_t*类型，index表示视频流索引号 */
	CONFIG_PRIVACY_AREA, /* 隐私区域，pconfig为lib_rect_t*类型 */
	CONFIG_VIDEO_MD, /* 视频动检，pconfig为video_md_t*类型 */
	CONFIG_VIDEO_LOSS, /* 视频丢失，pconfig为event_trigger_t*类型 */
	CONFIG_VIDEO_COVER, /* 视频遮挡，pconfig为event_trigger_t*类型 */
	CONFIG_AUDIO_SOURCE, /* 音频源参数，pconfig为audio_source_t*类型 */
	CONFIG_AUDIO_ENCODE, /* 音频编码参数，pconfig为audio_encode_t*类型，index表示音频流索引号 */
	CONFIG_REC_CHANNEL, /* 录像设置，pconfig为rec_channel_t*类型 */
	CONFIG_ALARM_INPUT, /* 报警输入，pconfig为alarm_input_t*类型 */
	CONFIG_SYS_EXCEPTION, /* 异常处理，pconfig为exception_trigger_t*类型，index表示异常代码，sys_exception_e类型 */
	CONFIG_REC_SCHEDULE, /* 录像计划，pconfig为schedule_config_t*类型 */
	CONFIG_ALARMIN_SCHEDULE,  /* 报警输入布防计划，pconfig为schedule_config_t*类型 */
	CONFIG_ALARMOUT_SCHEDULE, /* 报警输出计划，pconfig为schedule_config_t*类型 */
	CONFIG_BUZZEROUT_SCHEDULE, /* 蜂鸣器输出计划，pconfig为schedule_config_t*类型 */
	CONFIG_EMAILSEND_SCHEDULE, /* 邮件发送计划，pconfig为schedule_config_t*类型 */
	CONFIG_NAMEOSD_INFO, /* 通道名OSD信息，pconfig为venc_osd_t*类型（Windows系统只用到osdname成员），index表示OSD索引号 */
	CONFIG_NAMEOSD_DISPLAY, /* 通道名OSD显示，pconfig为osd_display_t*类型，index表示OSD索引号 */
	CONFIG_TIMEOSD_DISPLAY, /* 时间OSD显示，pconfig为osd_display_t*类型 */
	CONFIG_DISK_MANAGE, /* 硬盘管理，pconfig为disk_manage_t*类型 */
	CONFIG_UART_INFO, /* UART信息，pconfig为uart_param_t*类型（暂未实现） */
	CONFIG_PTZ_PARAM, /* PTZ参数，pconfig为ptz_param_t*类型 */
	CONFIG_AVC_CHANNEL, /* 通道接入，pconfig为avc_channel_t*类型 */
	CONFIG_AVC_TIME, /* IPC时间，pconfig为avc_time_t*类型 */
	CONFIG_AVC_NETWORK, /* IPC网络参数，pconfig为avc_network_t*类型 */
	CONFIG_NET_IP_FILTER, /* 网络IP过滤，pconfig为cli_ip_filter_t*类型 */
	CONFIG_NET_GLOBAL, /* 网络全局参数，pconfig为cli_net_global_t*类型 */
	CONFIG_NET_ETHERNET, /* 以太网，pconfig为cli_net_ethernet_t*类型 */
	CONFIG_NET_PPPOE, /* PPPOE参数，pconfig为cli_net_pppoe_t*类型 */
	CONFIG_NET_WIFI_AP, /* WIFI接入点，pconfig为cli_wifi_ap_t*类型 */
	CONFIG_NET_WIFI_CARD, /* WIFI网卡，pconfig为cli_net_ethernet_t*类型 */
	CONFIG_NET_WIFI, /* WIFI参数，pconfig为cli_net_wifi_t*类型 */
	CONFIG_NET_WI3G, /* Wireless 3G参数，pconfig为cli_net_wi3g_t*类型 */
	CONFIG_NET_NTP, /* NTP参数，pconfig为cli_net_ntp_t*类型 */
	CONFIG_NET_P2P, /* P2P参数，pconfig为cli_net_p2p_t*类型 */
	CONFIG_NET_FTP, /* FTP参数，pconfig为cli_net_ftp_t*类型（暂未实现） */
	CONFIG_NET_EMAIL, /* EMAIL参数，pconfig为cli_net_email_t*类型 */
	CONFIG_NET_DDNS_SERVICE, /* DDNS服务列表，pconfig为cli_service_ddns_t*类型 */
	CONFIG_NET_DDNS, /* DDNS参数，pconfig为cli_net_ddns_t*类型 */
	CONFIG_AUDIO_SOURCE_FROM /* 音频源选择，pconfig为int*类型（AUDIO_FROM_CAMERA等） */
};

typedef struct {
	char devname[CLI_NAME_LEN]; /* 设备名 */
	unsigned char remoteraddr; /* 遥控地址[0, 255] */
	char res[3];
} dev_local_t;

typedef struct {
	char dstenable; /* 是否启用夏令时 */
	unsigned char dstbias; /* 夏令时偏移值（30，60，90，120分钟） */
	unsigned char beginmonth; /* 起始月份（[0, 11]表示1月到12月） */
	unsigned char beginweekno; /* 起始周数（[0, 4]表示当月第1周到最后一周） */
	unsigned char beginweekday; /* 起始星期的天数（[0, 6]表示星期天到星期六） */
	unsigned char beginhour; /* 起始小时[0, 23] */
	unsigned char beginminute; /* 起始分钟[0, 59] */
	unsigned char endmonth; /* 结束月份（[0, 11]表示1月到12月） */
	unsigned char endweekno; /* 结束周数（[0, 4]表示当月第1周到最后一周） */
	unsigned char endweekday; /* 结束星期的天数（[0, 6]表示星期天到星期六） */
	unsigned char endhour; /* 结束小时[0, 23] */
	unsigned char endminute; /* 结束分钟[0, 59] */
} cli_dst_t;

typedef struct {
	lib_rect_t rc; /* 位置及尺寸 */
	effect_value_t effect; /* 效果参数 */
} video_source_t;

enum {
	PTZ_RELATE_NONE, /* 无云台联动 */
	PTZ_RELATE_PRESET = LIB_BIT00, /* 预置点联动，此时ptzpreset有效 */
	PTZ_RELATE_TOUR   = LIB_BIT01 /* 巡航联动，此时ptztour有效 */
};

typedef struct {
	unsigned char ptzrelate[CLI_MAX_CHNUM]; /* 云台联动类型 PTZ_RELATE_NONE ~ PTZ_RELATE_TOUR */
	unsigned char ptzpreset[CLI_MAX_CHNUM]; /* 云台预置点 */
	unsigned char ptztour[CLI_MAX_CHNUM]; /* 云台巡航线 */
} ptz_relate_t;

typedef struct {
	unsigned char recch[CLI_MAX_CHNUM]; /* 录像通道 */
	unsigned char alarmout[CLI_MAX_ALARM_OUTNUM]; /* 报警输出 */
	unsigned char beepout; /* 蜂鸣器 */
	unsigned char emailsend; /* 邮件发送 */
	char res[2]; /* 保留 */
} event_trigger_t;

typedef struct {
	int mdsensitive; /* 动检灵敏度 */
	ptz_relate_t mdrelate; /* 动检联动 */
	event_trigger_t mdtrigger; /* 动检触发 */
	unsigned int mdregion[24]; /* 动检区域，按位代表区域 */
} video_md_t;

typedef struct {
	int streamindex; /* 录像视频流索引号，从0起始 */
	unsigned char audiorec; /* 录音，0 不录音；1 录音 */
	unsigned char prerectime; /* 报警预录时间[0, 30](秒)，为0表示不预录 */
	char res[2]; /* 保留 */
} rec_channel_t;

enum {
	ALARM_TYPE_NO, /* 常开(Normal Open) */
	ALARM_TYPE_NC  /* 常闭(Normal Close) */
};

typedef struct {
	int alarmtype; /* 报警类型 */
	int autoclear; /* 自动清除[0, 900](秒钟)，0表示必须手动清除 */
	ptz_relate_t alarmrelate; /* 报警联动 */
	event_trigger_t alarmtrigger; /* 报警触发 */
} alarm_input_t;

typedef struct {
	unsigned char alarmout[CLI_MAX_ALARM_OUTNUM]; /* 报警输出 */
	unsigned char beepout; /* 蜂鸣器 */
	unsigned char emailsend; /* 邮件发送 */
	char res[2]; /* 保留 */
} exception_trigger_t;

typedef struct {
	unsigned short xpos; /* 水平位置 */
	unsigned short ypos; /* 垂直位置 */
	char osddisp; /* 是否显示 0 不显示，1 显示 */
	char res[3];
} osd_display_t;

typedef struct {
	char recoverwrite; /* 录像覆盖 0 不覆盖，1 自动覆盖 */
	unsigned char recpacktime; /* 录像打包时间[5, 120](分钟) */
	char res[2];
} disk_manage_t;

enum {
	OUTPUT_TYPE_ALARM, /* 报警输出 */
	OUTPUT_TYPE_EMAIL, /* 电子邮件 */
	OUTPUT_TYPE_BEEPER /* 蜂鸣器输出 */
};

typedef struct {
	int type; /* 类型 录像计划为REC_TYPE_NORMAL ~ REC_TYPE_VIDEODETECT，其他为0 */
	int wday; /* 星期 [0, 6]（表示从星期天到星期六） */
	lib_time_t beginning; /* 开始时间 */
	lib_time_t ending; /* 结束时间 */
} schedule_config_t;

/* AVC通道参数 */
typedef struct {
	char protocol[DEV_PROTOCOL_LEN]; /* 协议名 */
	char username[CLI_USER_LEN]; /* 用户名 */
	char password[CLI_USER_LEN]; /* 密码 */
	char ipaddr[DEV_ADDR_LEN]; /* ip或域名 */
	unsigned short ipport; /* ip端口 */
	unsigned char inport; /* 输入号，区分同一设备的多个输入 */
	unsigned char linkmode; /* 连接模式，LINK_MODE_AUTO、LINK_MODE_UDP等 */
	unsigned char enable; /* 使能 */
	char res[31]; /* 保留 */
} avc_channel_t;

/* ip访问参数 */
typedef struct {
	int ipenable; /* ip段是否允许，0 不允许；1 允许 */
	char ipbegin[CLI_ADDR_LEN]; /* 起始ip */
	char ipend[CLI_ADDR_LEN]; /* 结束ip */
} cli_ip_filter_t;

/* 网络全局参数 */
typedef struct {
	char upnpenable; /* UPNP是否启用，0 不启用；1 启用 */
	char multiipenable; /* 多IP功能是否启用，0 不启用；1 启用 */
	char netpriority[6]; /* 网络类型优先级高到低，表示优先使用对应类型的网络，NETWORK_TYPE_ETHERNET等，NETWORK_TYPE_NONE表示结束 */
	unsigned short cmdport; /* 命令端口，设备连接端口号 */
	unsigned short qdataport; /* 数据端口，设备数据传输端口号 */
	unsigned short httpport; /* http端口 */
	unsigned short rtspport; /* rtsp端口（暂未使用） */
	unsigned short mobileport; /* 移动设备端口（暂未使用） */
	unsigned short multicastport; /* 组播端口（暂未使用） */
	char res[64]; /* 保留 */
} cli_net_global_t;

/* 以太网/WIFI参数 */
typedef struct {
	char ipaddr[CLI_ADDR_LEN]; /* IP地址 */
	char ipmask[CLI_ADDR_LEN]; /* 子网掩码 */
	char gatewayaddr[CLI_ADDR_LEN]; /* 网关地址 */
	char dnsaddr[2][CLI_ADDR_LEN]; /* DNS服务器地址 */
	char dhcpenable; /* 是否启用dhcp */
	char autodns; /* 是否自动获取dns */
	unsigned char macaddr[CLI_MAC_LEN]; /* mac地址 */
} cli_net_ethernet_t;

/* 以太网状态 */
typedef struct {
	int etherstate; /* 以太网状态，0 未连接； 1 已连接（此时ipaddr有效） */
	char ipaddr[CLI_ADDR_LEN]; /* 以太网IP地址 */
} cli_status_ethernet_t;

/* PPPOE参数 */
typedef struct {
	int pppoeenable; /* PPPoE是否启用，0 不启用；1 启用 */
	char username[CLI_NAME_LEN]; /* PPPoE拨号用户名 */
	char password[CLI_NAME_LEN]; /* PPPoE拨号密码 */
} cli_net_pppoe_t;

/* PPPOE状态 */
typedef struct {
	int pppoestate; /* PPPoE状态，0 未连接； 1 已连接（此时ipaddr有效）； 2 正在连接 */
	char ipaddr[CLI_ADDR_LEN]; /* PPPoE拨号IP地址 */
} cli_status_pppoe_t;

enum {
	SECURITY_NONE, /* 无加密 */
	SECURITY_WEP, /* WEP，目前支持自动和共享密钥两种，开放系统暂不支持 */
	SECURITY_WPA, /* WPA */
	SECURITY_WPA2, /* WPA2 */
	SECURITY_WPA_AND_WPA2, /* WPA+WPA2 */
	SECURITY_WPA_PSK, /* WPA-PSK */
	SECURITY_WPA2_PSK, /* WPA2-PSK */
	SECURITY_WPA_AND_WPA2_PSK, /* WPA-PSK+WPA2-PSK */
	SECURITY_WPA_EAP, /* WPA-EAP */
	SECURITY_WPA2_EAP, /* WPA2-EAP */
	SECURITY_WPA_AND_WPA2_EAP, /* WPA-EAP+WPA2-EAP */
	SECURITY_WPS /* WPS */
};

/* Wifi接入点信息 */
typedef struct {
	char ssid[CLI_NAME_LEN]; /* SSID名称 */
	int netspeed; /* 网络速率(Kbps) */
	int signalch; /* 信道 */
	int workmode; /* 工作模式：0 Unknown；1 ad-hoc模式；2 mange模式 */
	int signalstrength; /* 信号强度，[0, 100] */
	int netsecurity; /* 网络加密类型，SECURITY_NONE等 */
} cli_wifi_ap_t;

/* Wifi参数 */
typedef struct {
	int wifienable; /* wifi是否启用，0 不启用；1 启用 */
	char ssid[CLI_NAME_LEN]; /* SSID名称 */
	char password[CLI_NAME_LEN]; /* 密码 */
	int netsecurity; /* 网络加密类型，SECURITY_NONE等 */
} cli_net_wifi_t;

/* Wifi连接状态 */
typedef struct {
	int wifistate; /* WIFI状态，0 未连接； 1 已连接（此时ipaddr有效）； 2 正在连接 */
	int signalch; /* 信道 */
	int signalstrength; /* 连接质量 */
	int netspeed; /* 网络速率(Kbps) */
	char ipaddr[CLI_ADDR_LEN]; /* WIFI IP地址 */
} cli_status_wifi_t;

/* Wireless 3G参数 */
typedef struct {
	int wi3genable; /* 3G是否启用，0 不启用；1 启用 */
	char apn[CLI_NAME_LEN]; /* 拨号接入点名称 */
	char dialnumber[CLI_NAME_LEN]; /* 拨号号码 */
	char username[CLI_NAME_LEN]; /* 3G拨号用户名 */
	char password[CLI_NAME_LEN]; /* 3G拨号密码 */
} cli_net_wi3g_t;

/* Wireless 3G连接状态 */
typedef struct {
	int wi3gstate; /* 3G状态，0 未连接； 1 已连接（此时ipaddr有效）； 2 正在连接 */
	int strength; /* 3G信号强度 */
	char network_mode[CLI_NAME_LEN]; /* 3G网络系统模式 */
	char network_type[CLI_NAME_LEN]; /* 3G网络类型 */
	char ipaddr[CLI_ADDR_LEN]; /* 3G拨号IP地址 */
} cli_status_wi3g_t;

/* NTP参数 */
typedef struct {
	int ntpenable; /* NTP是否启用，0 不启用；1 启用 */
	int zonebias; /* 当前时区与UTC时间差(分钟) */
	char ntpaddr[CLI_ADDR_LEN]; /* NTP服务器域名或地址 */
	unsigned short ntpport; /* NTP端口 */
	unsigned short checkperiod; /* 检查周期(分钟)，至少5分钟以上 */
} cli_net_ntp_t;

/* P2P参数 */
typedef struct {
	int p2penable; /* P2P是否启用，0 不启用；1 启用 */
	char p2pid[CLI_ID_LEN]; /* P2P ID号（只读） */
	char p2paddr[CLI_ADDR_LEN]; /* P2P访问域名或地址（只读），为空表示无效 */
	unsigned short remoteport; /* P2P远程服务端口号，搭配P2P域名或地址使用，0 无效 */
	unsigned short localport; /* P2P设备本地加载端口号 */
	char iosappurl[CLI_URL_LEN]; /* P2P iOS应用下载地址（只读），为空表示无效 */
	char androidappurl[CLI_URL_LEN]; /* P2P Android应用下载地址（只读），为空表示无效 */
	char res[112]; /* 保留为空 */
} cli_net_p2p_t;

/* P2P扩展参数 */
typedef struct {
	unsigned int p2ptype; /* P2P类型 */
	//char p2pmanuprefix[CLI_MANU_PREFIX_LEN]; /* P2P厂商前缀 */
	char p2pid[CLI_ID_LEN]; /* P2P ID号（只读） */
	char res[128]; /* 保留为空 */
} cli_net_p2p_extra_t;

/* EMAIL参数 */
typedef struct {
	char username[CLI_NAME_LEN]; /* 用户名 */
	char password[CLI_NAME_LEN]; /* 密码 */
	char smtpaddr[CLI_ADDR_LEN]; /* SMTP服务器地址 */
	unsigned short smtpport; /* SMTP端口 */
	char sslenable; /* SSL是否启用，0 不启用；1 启用 */
	char smtpverify; /* SMTP是否需验证，0 不验证；1 验证 */
	char senderaddr[CLI_ADDR_LEN]; /* 邮件发送者地址 */
	char receiveraddr[2][CLI_ADDR_LEN]; /* 邮件接收者地址 */
} cli_net_email_t;

enum {
	DDNS_DOMAIN_SUBMIT  = LIB_BIT00, /* 设备支持DDNS域名注册 */
	DDNS_DOMAIN_FIXED   = LIB_BIT01, /* DDNS域名固定，不可修改 */
	DDNS_POSTFIX_FIXED  = LIB_BIT02, /* 域名后缀固定（即DDNS服务名），不可修改 */
	DDNS_ACCOUNT_FIXED  = LIB_BIT03, /* 域名账号固定，不可修改 */
	DDNS_ACCOUNT_HIDDEN = LIB_BIT04, /* 域名账号隐藏，不用显示 */
	DDNS_ALIASNAME_SET  = LIB_BIT05 /* 域名支持设置别名，通常配合DDNS_DOMAIN_FIXED使用 */
};

/* DDNS可用服务列表（只读） */
typedef struct {
	char ddnsname[CLI_NAME_LEN]; /* 服务名 */
	char ddnsaddr[CLI_ADDR_LEN]; /* 服务器IP或域名 */
	unsigned short ddnsport; /* 服务器端口 */
	unsigned short ddnscapabilties; /* DDNS属性，DDNS_DOMAIN_SUBMIT等 */
	char ddnsdomain[CLI_ADDR_LEN]; /* DDNS固定域名，属性DDNS_DOMAIN_FIXED有效 */
} cli_service_ddns_t;

/* DDNS参数 */
typedef struct {
	int ddnsenable; /* DDNS是否启用，0 不启用；1 启用 */
	char ddnsname[CLI_NAME_LEN]; /* 对应的DDNS服务名 */
	char ddnsdomain[CLI_ADDR_LEN]; /* DDNS可变域名或别名，无DDNS_DOMAIN_FIXED属性或有DDNS_ALIASNAME_SET属性有效 */
	char username[CLI_NAME_LEN]; /* DDNS用户名 */
	char password[CLI_NAME_LEN]; /* DDNS密码 */
} cli_net_ddns_t;

/* DDNS状态 */
typedef struct {
	int ddnsstate; /* DDNS状态 */
	char ipaddr[CLI_ADDR_LEN]; /* 域名对应IP地址 */
} cli_status_ddns_t;

enum {
	P2P_STATUS_UNCONNECTED, /* 未连接 */
	P2P_STATUS_CONNECTING, /* 正在连接 */
	P2P_STATUS_CONNECTED, /* 已连接 */
	P2P_STATUS_CONNECT_FAIL, /* 连接失败 */
	P2P_STATUS_CONNECT_UNKNOW /* 连接状态未知 */
};

/* P2P状态 */
typedef struct {
	char p2pid[CLI_ID_LEN]; /* P2P ID号（只读） */
	unsigned short connstate; /* 连接状态(只读),P2P_STATUS_* */
	unsigned short connerrnum; /* 连接失败错误码(只读) */
} cli_status_p2p_t;

/* 网络状态 */
typedef struct {
	union {
		cli_status_ethernet_t ethernetstate; /* 以太网状态，CONFIG_NET_ETHRENET有效 */
		cli_status_pppoe_t pppoestate; /* PPPOE状态，CONFIG_NET_PPPOE有效 */
		cli_status_wifi_t wifistate; /* WIFI状态，CONFIG_NET_WIFI有效 */
		cli_status_wi3g_t wi3gstate; /* 3G状态，CONFIG_NET_WI3G有效 */
		cli_status_ddns_t ddnsstate; /* DDNS状态，CONFIG_NET_DDNS有效 */
		cli_status_p2p_t p2pstate; /* P2P状态，CONFIG_NET_P2P有效 */
	};
} cli_net_status_t;


enum {
	RANGE_VGA_OUTPUT, /* VGA输出参数范围，display_range_t指针 */
	RANGE_HDMI_OUTPUT, /* HDMI输出参数范围，display_range_t指针 */
	RANGE_CVBS_OUTPUT, /* CVBS输出参数范围，display_range_t指针 */
	RANGE_VIDEO_MD, /* 动检参数范围，md_range_t指针 */
	RANGE_PTZ_PROTOCOL, /* 云台协议范围，ptz_param_t指针（位属性） */
	RANGE_AUDIO_SOURCE, /* 音频源参数范围，adsrc_range_t指针 */
	RANGE_VIDEO_SOURCE, /* 视频源参数范围，effect_range_t指针 */
	RANGE_VIDEO_EFFECT, /* 视频效果参数范围，effect_range_t指针 */
	RANGE_VIDEO_ENCODE /* 视频编码参数范围，vdenc_range_t指针，index为对应流索引号 */
};

typedef struct {
	effect_value_t minval; /* 最小值 */
	effect_value_t maxval; /* 最大值 */
} effect_range_t;

typedef struct {
	int dispreso; /* 分辨率支持，0表示不支持调整 */
	int refreshfreq; /* 刷新频率支持，0表示不支持调整 */
	effect_range_t dispeffect; /* 显示效果范围 */
} display_range_t;

typedef struct {
	int horznum; /* 水平块数 */
	int vertnum; /* 垂直块数 */
	int minsensitive; /* 最小灵敏度 */
	int maxsensitive; /* 最大灵敏度 */
} md_range_t;

typedef struct {
	audio_source_t minval; /* 最小值 */
	audio_source_t maxval; /* 最大值 */
} adsrc_range_t;

typedef struct {
	video_encode_t minenc; /* 最小值 */
	video_encode_t maxenc; /* 最大值 */
} vdenc_range_t;



typedef struct {
	int ch; /* 通道 */
	int type; /* 类型 */
	unsigned int ts; /* 相对时间戳 */
	size_t len; /* 长度 */
	char *pdata; /* 数据 */
} data_info_t;


typedef struct {
	void* hplaywnd; /* 播放窗口 */
	int streamindex; /* 视频流索引号，从0起始 */
	int autoplay; /* 自动播放，例如等待IPC连接后启动通道预览 */
} real_play_info_t;


typedef struct {
	void* hplaywnd; /* 播放窗口 */
	int rectype; /* 录像类型，REC_TYPE_ALL等 */
	lib_duration_t recdur; /* 录像时间段 */
} rec_play_info_t;



typedef struct {
	int recvbps; /* 码流接收速率 */
	int recvjitter; /* 码流接收抖动 */
} play_recv_info_t;


typedef struct {
	int param; /* 控制参数 */
	lib_moment_t playmom; /* 播放时间 */
} remote_play_control_t;



enum {
	RIGHT_DATE_TIME        = LIB_BIT00, /* 包括系统时间、时间格式和夏令时设置权限 */
	RIGHT_NETWORK_CONFIG   = LIB_BIT01, /* 包括以太网、3G、WIFI、各种网络服务、IP过滤等参数设置权限 */
	RIGHT_ENCODE_CONFIG    = LIB_BIT02, /* 包括音视频编码参数设置权限 */
	RIGHT_RECODE_CONFIG    = LIB_BIT03, /* 包括录像参数、录像计划设置 */
	RIGHT_RECODE_CONTROL   = LIB_BIT04, /* 录像控制权限 */
	RIGHT_RECORD_BACKUP    = LIB_BIT05, /* 录像备份权限 */
	RIGHT_ALARM_CONFIG     = LIB_BIT06, /* 包括报警输入、输出设置权限 */
	RIGHT_ALARM_CONTROL    = LIB_BIT07, /* 包括报警输入、报警清除、报警输出控制权限 */
	RIGHT_REMOTE_CHANNEL   = LIB_BIT08, /* 远程IP通道设置权限 */
	RIGHT_CHANNEL_OSD      = LIB_BIT09, /* 包括通道时间、名称OSD设置权限 */
	RIGHT_PRIVACY_CONFIG   = LIB_BIT10, /* 隐私区域设置权限 */
	RIGHT_EXCEPTION_CONFIG = LIB_BIT11, /* 异常处理设置权限 */
	RIGHT_VIDEO_DETECT     = LIB_BIT12, /* 包括视频丢失、动检、遮挡设置权限 */
	RIGHT_VIDEO_EFFECT     = LIB_BIT13, /* 视频效果设置权限 */
	RIGHT_DISK_MANAGE      = LIB_BIT14, /* 包括硬盘使用参数、格式化权限 */
	RIGHT_LOG_VIEW         = LIB_BIT15, /* 日志查看权限 */
	RIGHT_WORK_MODE        = LIB_BIT16, /* 通道工作模式、视频制式设置权限 */
	RIGHT_REMOTE_LOGIN     = LIB_BIT17, /* 用户远程登录权限 */
	RIGHT_ONLINE_USER      = LIB_BIT18, /* 在线用户管理权限 */
	RIGHT_SYSTEM_UPGRADE   = LIB_BIT19, /* 系统升级权限 */
	RIGHT_REBOOT_POWEROFF  = LIB_BIT20 /* 系统重启、关机权限 */
};

typedef struct {
	char username[CLI_USER_LEN]; /* 用户名 */
	char password[CLI_USER_LEN]; /* 密码 */
	unsigned int userright; /* 用户权限（位属性），RIGHT_DATE_TIME等 */
	unsigned char ptzch[CLI_MAX_CHNUM]; /* 云台控制通道权限 */
	unsigned char previewch[CLI_MAX_CHNUM]; /* 预览通道权限 */
	unsigned char playbackch[CLI_MAX_CHNUM]; /* 回放通道权限 */
	unsigned char remotech[CLI_MAX_CHNUM]; /* 远程通道权限，结合其他通道权限使用 */
	char ipaddr[CLI_ADDR_LEN]; /* ip地址，表示该用户只允许特定地址，为空任意地址 */
	unsigned char macaddr[6]; /* mac地址，表示该用户只允许特定地址，0任意地址 */
	char res[18]; /* 保留 */
} user_info_t;


typedef struct {
	char username[CLI_USER_LEN]; /* 在线用户名 */
	char ipaddr[CLI_ADDR_LEN]; /* 在线ip地址 */
} online_user_t;


enum {
	ALARMIN_MODE_DISARMING, /* 撤防 */
	ALARMIN_MODE_SCHEDULE, /* 计划布防 */
	ALARMIN_MODE_ARMING /* 手动布防 */
};


enum {
	REC_MODE_STOP, /* 停止录像 */
	REC_MODE_SCHEDULE, /* 计划录像 */
	REC_MODE_MANUAL /* 手动录像 */
};



/* PTZ主控制命令 */
enum {
	CAMERA_MENU_CONTROL, /* 菜单控制 */
	PTZ_DIR_CONTROL, /* 方向控制 */
	PTZ_LENS_CONTROL, /* 镜头控制 */
	PTZ_PRESET_CONTROL, /* 预置位控制 */
	PTZ_TOUR_CONTROL, /* 巡航控制 */
	PTZ_AUX_CONTROL, /* 辅助开关控制 */
	PTZ_HOME_POSITION /* 看守位控制 */
};

/* CAMERA_MENU_CONTROL子命令 */
enum {
	MENU_OPEN, /* 菜单打开 */
	MENU_CLOSE, /* 菜单关闭 */
	MENU_MOVE_UP, /* 上移 */
	MENU_MOVE_DOWN, /* 下移 */
	MENU_MOVE_LEFT, /* 左移 */
	MENU_MOVE_RIGHT, /* 右移 */
	MENU_UPPER_LEVEL, /* 上一级菜单 */
	MENU_LOWER_LEVEL, /* 下一级菜单 */
	MENU_SEL_CONFIRM /* 菜单选择确认 */
};

/* PTZ_DIR_CONTROL子命令，majorparam表示转动速度[0, 15] */
enum {
	DIR_STOP, /* 停止 */
	DIR_UP, /* 向上 */
	DIR_DOWN, /* 向下 */
	DIR_LEFT, /* 向左 */
	DIR_RIGHT, /* 向右 */
	DIR_UP_LEFT, /* 左上 */
	DIR_UP_RIGHT, /* 右上 */
	DIR_DOWN_LEFT, /* 左下 */
	DIR_DOWN_RIGHT /* 右下 */
};

/* PTZ_LENS_CONTROL子命令，majorparam表示调焦速度[0, 15] */
enum {
	LENS_STOP, /* 停止 */
	IRIS_CLOSE, /* 光圈小 */
	IRIS_OPEN, /* 光圈大 */
	FOCUS_NEAR, /* 聚焦近 */
	FOCUS_FAR, /* 聚焦远 */
	ZOOM_WIDE, /* 焦距广 */
	ZOOM_TELE /* 焦距窄 */
};

/* PTZ_PRESET_CONTROL子命令，majorparam表示对应的预置点 */
enum {
	PRESET_SET, /* 设定预置位 */
	PRESET_GOTO, /* 调用预置位 */
	PRESET_REMOVE /* 移除预置位 */
};

/* PTZ_TOUR_CONTROL子命令，majorparam表示对应的巡航号 */
/* 添加或删除时minorparam表示对应的预置点，添加时staytime表示该预置点停留时间 */
enum {
	TOUR_PRESET_ADD, /* 增加预置点 */
	TOUR_PRESET_DEL, /* 删除预置点 */
	TOUR_CLEAR, /* 清除巡航线 */
	TOUR_START, /* 开始巡航 */
	TOUR_STOP /* 停止巡航 */
};

/* PTZ_AUX_CONTROL子命令 */
enum {
	AUX_LIGHT_ON, /* 灯光控制开 */
	AUX_LIGHT_OFF, /* 灯光控制关 */
	AUX_WIPER_ON, /* 雨刷控制开 */
	AUX_WIPER_OFF, /* 雨刷控制关 */
	AUX_WASHER_ON, /* 清洗控制开 */
	AUX_WASHER_OFF, /* 清洗控制关 */
	AUX_HEATER_ON, /* 加热控制开 */
	AUX_HEATER_OFF /* 加热控制关 */
};

/* PTZ_HOME_POSITION子命令 */
enum {
	HOME_POSITION_SET, /* 设置看守位 */
	HOME_POSITION_GOTO /* 调用看守位 */
};

/* 云台控制 */
typedef struct {
	unsigned char majorcmd; /* 主控制命令 PTZ_DIRECTION_CONTROL ~ PTZ_TOUR_CONFIG */
	unsigned char minorcmd; /* 次控制命令，主命令对应的子命令 */
	unsigned char majorparam; /* 主控制参数 */
	unsigned char minorparam; /* 次控制参数 */
	unsigned short staytime; /* 预置点巡航的停留时间(秒) */
	char res[2]; /* 保留 */
} ptz_control_t;


enum {
	REC_TYPE_ALL         = 0, /* 全部类型 */
	REC_TYPE_NORMAL      = LIB_BIT00, /* 普通录像 */
	REC_TYPE_VIDEODETECT = LIB_BIT01, /* 视频检测录像，包括视频动检、丢失和遮挡导致的录像 */
	REC_TYPE_ALARM       = LIB_BIT02 /* 报警录像 */
};

typedef struct {
	unsigned int samplenum; /* 总帧数 */
	unsigned int samplelen; /* 总长度(Bytes) */
	rec_duration_t recdur; /* 录像段 */
} rec_info_t;


enum {
	LOG_TYPE_ALL       = 0, /* 全部类型 */
	LOG_SYSTEM_CONTROL = LIB_BIT00, /* 系统操作（见CONTROL_USER_ADD等定义） */
	LOG_SYSTEM_CONFIG  = LIB_BIT01, /* 系统配置（见CONFIG_DEVICE_INFO等定义） */
	LOG_SYSTEM_EVENT   = LIB_BIT02 /* 系统事件（见EVENT_VIDEO_REC等定义），与LOG_SYSTEM_CONTROL重合的除外 */
};

enum {
	CONTROL_USER_ADD, /* 添加用户 */
	CONTROL_USER_SET, /* 修改用户 */
	CONTROL_USER_DEL, /* 删除用户 */
	CONTROL_USER_BREAK, /* 断开在线用户 */
	CONTROL_USER_LOGIN, /* 用户登录，日志内容为用户名 */
	CONTROL_USER_LOGOUT, /* 用户登出，日志内容为用户名 */
	CONTROL_ALARM_CLEAR, /* 手动清除报警 */
	CONTROL_ALARM_INPUT, /* 报警输入布防 */
	CONTROL_ALARM_OUTPUT, /* 报警输出控制 */
	CONTROL_PTZ, /* 云台控制 */
	CONTROL_REC, /* 录像控制 */
	CONTROL_REC_BACKUP, /* 录像备份 */
	CONTROL_REC_PLAY, /* 录像播放 */
	CONTROL_REAL_PLAY, /* 实时播放 */
	CONTROL_WORK_MODE, /* 工作模式设置 */
	CONTROL_FORMAT_DISK, /* 硬盘格式化 */
	CONTROL_DEFAULT_RESET, /* 恢复默认设置 */
	CONTROL_LOG_CLEAR, /* 日志清除 */
	CONTROL_DEV_UPGRADE, /* 设备升级 */
	CONTROL_DEV_BOOTUP, /* 设备开机 */
	CONTROL_DEV_REBOOT, /* 设备重启 */
	CONTROL_DEV_SHUTDOWN /* 设备关机 */
};

enum {
	OPERATE_CONTROL_START, /* 控制开始 */
	OPERATE_CONTROL_STOP, /* 控制结束 */
	OPERATE_CONFIG_GET, /* 参数获取 */
	OPERATE_CONFIG_PUT, /* 参数设置 */
	OPERATE_EVENT_START, /* 事件开始 */
	OPERATE_EVENT_STOP /* 事件结束 */
};

typedef struct {
	int logtype; /* 日志类型，LOG_SYSTEM_CONTROL等 */
	int logcmd; /* 日志命令，见日志类型相关定义 */
	int logopt; /* 日志操作，OPERATE_CONTROL_START等 */
	int logch; /* 日志通道 */
	lib_moment_t logmom; /* 记录时间 */
	char logtext[CLI_LOG_LEN]; /* 附加信息，例如用户名 */
} log_info_t;


enum {
	UPGRADE_FROM_FILE, /* 文件升级，使用升级文件路径 */
	UPGRADE_FROM_URL /* URL升级 */
};


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __NSYS_CLI_H__ */
