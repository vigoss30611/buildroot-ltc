/*
*********************************************************************************************************
*
*                       Copyright (c) 2012 Hangzhou QianMing Technology Co.,Ltd
*
*********************************************************************************************************
*/
#ifndef __NSYS_COMM_H__
#define __NSYS_COMM_H__

/*********************************************************************
*
*       Message
*
**********************************************************************
*/

#define VER_ID                  0x0100 /* 版本id */

/* 设备侦测 */
#define DS_TYPES				"device"
#define DS_METAVER				2014
#define DS_IPPORT				13702
#define DS_BUFLEN				512

#define DS_SERIAL				"serial"
#define DS_MODEL				"model"
#define DS_NAME				"name"
#define DS_HARDWARE			"hardware"
#define DS_SOFTWARE			"software"
#define DS_CHANNELS			"channels"
#define DS_MAC					"mac"
#define DS_IPADDR				"ipaddr"
#define DS_CLIPORT				"cliport"
#define DS_P2PID				"p2pid"
#define DS_P2PTYPE				"p2ptype"

/* 操作类型 */
#define OPERATE_MASK            0x000F /* 操作掩码 */
#define OPERATE_GET             0x0001 /* 获取 */
#define OPERATE_PUT             0x0002 /* 设置 */
#define OPERATE_START           0x0004 /* 启动(打开) */
#define OPERATE_STOP            0x0008 /* 停止(关闭) */
#define OPERATE_RETURN          0x8000 /* 返回标志 */

#define NSYS_CLI_SERVER			"/tmp/nsysserv"
#define NSYS_DISP_FBFD			"/tmp/nsysdisp"
#define NSYS_DATA_STREAM		"/tmp/nsysstream"

#define SEC_2_MS(x)             ( (x) * 1000 ) /* 秒转化为毫秒 */

/* 命令头 */
typedef struct {
	unsigned short verid;  /* 版本id */
	unsigned short cmdch;  /* 命令通道 */
	unsigned short cmdtag; /* 命令类型 */
	unsigned short cmdopt; /* 操作类型 OPERATE_GET ~ OPERATE_STOP */
	unsigned short cmdseq; /* 流水号 */
	unsigned short cmdret; /* 返回代码 */
	unsigned short cbsize; /* 附加数据长度 */
	unsigned short xorsum; /* 异或校验和 */
} cmd_header_t;

typedef struct {
	unsigned short srcmod; /* 消息源模块 */
	unsigned short destmod; /* 消息目的模块 */
	void *pconn; /* 对应的用户连接，为NULL表示内部子模块通讯 */
} msg_header_t;

typedef struct {
	msg_header_t msghdr;
	cmd_header_t cmdhdr;
} sys_header_t;

#define CMD_ISRETURN(p)         ( (p).cmdopt & OPERATE_RETURN ) /* 是否返回命令 */
#define CMD_DOMODULE(p)         CMDTAGMODULE( (p).cmdtag ) /* 命令处理模块 */
#define CMD_DOOPERATE(p)        ( (p).cmdopt & OPERATE_MASK ) /* 命令处理操作 */
#define CMD_XORSUM(cmd)         ( (cmd).cmdtag ^ (cmd).cmdch ^ (cmd).cmdopt ^ (cmd).cmdseq ^ (cmd).cmdret ^ (cmd).cbsize )

/* 命令头初始化 */
#define SET_CMDHEADER(cmd, tag, ch, opt, sz) do { \
	(cmd).verid  = VER_ID; \
	(cmd).cmdtag = tag; \
	(cmd).cmdch  = ch; \
	(cmd).cmdopt = opt; \
	(cmd).cmdseq = 0; \
	(cmd).cmdret = ERR_NO_ERROR; \
	(cmd).cbsize = sz; \
	(cmd).xorsum = CMD_XORSUM(cmd); \
} while (0)

/* 设置命令头参数 */
#define SET_CMDRET(p)           ( (p).cmdopt |= OPERATE_RETURN )
#define SET_CMDSIZE(p, cs)      ( (p).cbsize  = cs )
#define SET_CMDRETNO(p, cr)     ( (p).cmdret  = cr )
#define SET_CMDSZRET(p, cs, cr) { SET_CMDRET(p); SET_CMDSIZE(p, cs); SET_CMDRETNO(p, cr); }

/*********************************************************************
*
*       Module
*
**********************************************************************
*/

#define GENCMDTAG(m, t)         ((uint16_t) ((uint16_t)(m) | (uint8_t)(t)))
#define CMDTAGMODULE(x)			( (x) & MODULE_MASK )

#define MODULE_MASK             0xFF00 /* 模块掩码 */
#define MAIN_MANAGE_MODULE      0x0100 /* 主模块 */
#define AVC_MANAGE_MODULE       0x0200 /* avc管理模块 */
#define ALARM_EXCEPTION_MODULE  0x0300 /* 报警及异常模块 */
#define RECORD_MANAGE_MODULE    0x0400 /* 录像管理模块 */
#define PLAYBACK_MANAGE_MODULE  0x0500 /* 回放管理模块 */
#define QDATA_MANAGE_MODULE     0x0600 /* 私有数据协议模块 */
#define VIDEO_ANALYTICS_MODULE  0x0700 /* 智能分析模块 */
#define SEARCH_BACKUP_MODULE    0x0800 /* 检索及备份模块 */
#define NETWORK_SERVICE_MODULE  0x0900 /* 网络服务模块 */
#define ASSIST_MANAGE_MODULE    0x0A00 /* 辅助管理模块 */
#define DEV_DISCOVERY_MODULE    0x0B00 /* 设备发现模块 */
#define P2P_MANAGE_MODULE         0x0C00 /*P2P 管理模块*/
/* 主模块命令 */
#define USER_ACCOUNT_INFO       GENCMDTAG(MAIN_MANAGE_MODULE, 1) /* 账号信息 */
#define USER_ACCOUNT_GET        GENCMDTAG(MAIN_MANAGE_MODULE, 2) /* 获取用户信息 */
#define USER_ACCOUNT_SET        GENCMDTAG(MAIN_MANAGE_MODULE, 3) /* 修改用户信息 */
#define USER_ACCOUNT_ADD        GENCMDTAG(MAIN_MANAGE_MODULE, 4) /* 增加用户 */
#define USER_ACCOUNT_DEL        GENCMDTAG(MAIN_MANAGE_MODULE, 5) /* 删除用户 */
#define ONLINE_USER_INFO        GENCMDTAG(MAIN_MANAGE_MODULE, 6) /* 在线用户 */
#define BREAK_ONLINE_USER       GENCMDTAG(MAIN_MANAGE_MODULE, 7) /* 断开用户 */
#define NET_IP_FILTER           GENCMDTAG(MAIN_MANAGE_MODULE, 8) /* ip访问过滤 */
#define ENUM_WORK_MODE          GENCMDTAG(MAIN_MANAGE_MODULE, 9) /* 枚举工作模式 */
#define WORK_MODE_CONFIG        GENCMDTAG(MAIN_MANAGE_MODULE, 10) /* 工作模式设置 */
#define VGA_OUTPUT_RANGE        GENCMDTAG(MAIN_MANAGE_MODULE, 11) /* VGA调整范围 */
#define HDMI_OUTPUT_RANGE       GENCMDTAG(MAIN_MANAGE_MODULE, 12) /* HDMI调整范围 */
#define CVBS_OUTPUT_RANGE       GENCMDTAG(MAIN_MANAGE_MODULE, 13) /* CVBS调整范围 */
#define PTZ_PROTOCOL_RANGE      GENCMDTAG(MAIN_MANAGE_MODULE, 14) /* 云台协议范围 */
#define AUDIO_SOURCE_RANGE      GENCMDTAG(MAIN_MANAGE_MODULE, 15) /* 音频源范围 */
#define AUDIO_ENCODE_RANGE      GENCMDTAG(MAIN_MANAGE_MODULE, 16) /* 音频编码范围 */
#define VIDEO_SOURCE_RANGE      GENCMDTAG(MAIN_MANAGE_MODULE, 17) /* 视频源范围 */
#define MANAGE_HOST_CONFIG      GENCMDTAG(MAIN_MANAGE_MODULE, 18) /* 管理主机参数 */
#define MANAGE_HOST_REDIRECT    GENCMDTAG(MAIN_MANAGE_MODULE, 19) /* 管理主机重定向 */
#define SYS_MODULE_COMM         GENCMDTAG(MAIN_MANAGE_MODULE, 20) /* 模块消息 */
#define SYS_REGISTER_EVENT      GENCMDTAG(MAIN_MANAGE_MODULE, 21) /* 事件注册 */
#define SYS_EVENT_NOTIFY        GENCMDTAG(MAIN_MANAGE_MODULE, 22) /* 事件触发通知 */
#define SYS_USER_LOGIN          GENCMDTAG(MAIN_MANAGE_MODULE, 23) /* 用户登入 */
#define SYS_USER_ACCEPT         GENCMDTAG(MAIN_MANAGE_MODULE, 24) /* 用户接受 */
#define SYS_USER_LOGOUT         GENCMDTAG(MAIN_MANAGE_MODULE, 25) /* 用户登出 */
#define SYS_KEEP_ALIVE          GENCMDTAG(MAIN_MANAGE_MODULE, 26) /* 系统心跳 */
#define SYS_WORK_STATE          GENCMDTAG(MAIN_MANAGE_MODULE, 27) /* 系统工作状态 */
#define SYS_DEVICE_INFO         GENCMDTAG(MAIN_MANAGE_MODULE, 28) /* 设备信息 */
#define SYS_DEVICE_LOCAL        GENCMDTAG(MAIN_MANAGE_MODULE, 29) /* 设备本地配置 */
#define SYS_CLONE_CONFIG        GENCMDTAG(MAIN_MANAGE_MODULE, 30) /* 参数复制 */
#define SYS_DEFAULT_RESET       GENCMDTAG(MAIN_MANAGE_MODULE, 31) /* 系统恢复默认 */
#define SYS_FILE_UPGRADE        GENCMDTAG(MAIN_MANAGE_MODULE, 32) /* 系统升级 */
#define SYS_UPGRADE_REQUEST     GENCMDTAG(MAIN_MANAGE_MODULE, 33) /* 升级获取文件 */
#define SYS_LOG_SEARCH          GENCMDTAG(MAIN_MANAGE_MODULE, 34) /* 日志检索 */
#define SYS_LOG_READ            GENCMDTAG(MAIN_MANAGE_MODULE, 35) /* 日志读取 */
#define SYS_LOG_CLEAR           GENCMDTAG(MAIN_MANAGE_MODULE, 36) /* 日志读取 */
#define SYS_POWER_OFF           GENCMDTAG(MAIN_MANAGE_MODULE, 37) /* 系统关机 */
#define SYS_REBOOT              GENCMDTAG(MAIN_MANAGE_MODULE, 38) /* 系统重启 */
#define AUDIO_BACK_CHANNEL      GENCMDTAG(MAIN_MANAGE_MODULE, 39) /* 音频回传 */
#define VIDEO_BACK_CHANNEL      GENCMDTAG(MAIN_MANAGE_MODULE, 40) /* 视频回传 */
#define AVC_PROTOCOL_NAME       GENCMDTAG(MAIN_MANAGE_MODULE, 41) /* avc协议名 */
#define AVC_PROTOCOL_SCAN       GENCMDTAG(MAIN_MANAGE_MODULE, 42) /* avc协议扫描 */
#define AVC_PROTOCOL_DEV        GENCMDTAG(MAIN_MANAGE_MODULE, 43) /* avc协议扫描 */
#define AVC_CHANNEL_CONFIG      GENCMDTAG(MAIN_MANAGE_MODULE, 44) /* 通道配置 */
#define JPEG_PICTURE_CAPTURE    GENCMDTAG(MAIN_MANAGE_MODULE, 45) /* 通道截图 */
#define JPEG_PICTURE_DATA       GENCMDTAG(MAIN_MANAGE_MODULE, 46) /* 截图数据 */
#define VIRTUAL_KEY_CTRL        GENCMDTAG(MAIN_MANAGE_MODULE, 47) /* 虚拟按键 */
#define SYS_WORK_STATE_EX          GENCMDTAG(MAIN_MANAGE_MODULE, 48) /* 系统工作状态 */
#define JPEG_PICTURE_CAPTURE_EX  GENCMDTAG(MAIN_MANAGE_MODULE, 49) /* 截图扩展*/
#define SYS_LOG_INQUERY GENCMDTAG(MAIN_MANAGE_MODULE, 50) /* 月日志查询*/

/* avc管理 */
#define NATIVE_REAL_OPEN        GENCMDTAG(AVC_MANAGE_MODULE, 1) /* 打开实时数据通道 */
#define NATIVE_REC_OPEN         GENCMDTAG(AVC_MANAGE_MODULE, 2) /* 打开录像数据通道 */
#define NATIVE_REC_SEEK         GENCMDTAG(AVC_MANAGE_MODULE, 3) /* 录像定位 */
#define NATIVE_REC_RESTORE      GENCMDTAG(AVC_MANAGE_MODULE, 4) /* 定位后取新数据 */
#define NATIVE_DATA_CLOSE       GENCMDTAG(AVC_MANAGE_MODULE, 5) /* 关闭数据通道 */
#define NATIVE_AUDIO_STREAM     GENCMDTAG(AVC_MANAGE_MODULE, 6) /* 音频流控制 */
#define NATIVE_VIDEO_STREAM     GENCMDTAG(AVC_MANAGE_MODULE, 7) /* 视频流控制 */
#define DATE_TIME_MODE          GENCMDTAG(AVC_MANAGE_MODULE, 8) /* 时间日期模式 */
#define TIME_OSD_DISPLAY        GENCMDTAG(AVC_MANAGE_MODULE, 9) /* 时间OSD显示 */
#define NAME_OSD_CONFIG         GENCMDTAG(AVC_MANAGE_MODULE, 10) /* 通道名OSD配置 */
#define NAME_OSD_DISPLAY        GENCMDTAG(AVC_MANAGE_MODULE, 11) /* 通道名OSD显示 */
#define VIDEO_MD_RANGE          GENCMDTAG(AVC_MANAGE_MODULE, 12) /* 动检参数范围 */
#define VIDEO_EFFECT_RANGE      GENCMDTAG(AVC_MANAGE_MODULE, 13) /* 视频效果范围 */
#define VIDEO_EFFECT_CONFIG     GENCMDTAG(AVC_MANAGE_MODULE, 14) /* 视频效果设置 */
#define STREAM_MAKE_KEYFRAME    GENCMDTAG(AVC_MANAGE_MODULE, 15) /* 流产生关键帧 */
#define AUDIO_ENCODE_CONFIG     GENCMDTAG(AVC_MANAGE_MODULE, 16) /* 音频编码参数 */
#define VIDEO_ENCODE_RANGE      GENCMDTAG(AVC_MANAGE_MODULE, 17) /* 视频编码范围 */
#define VIDEO_ENCODE_CONFIG     GENCMDTAG(AVC_MANAGE_MODULE, 18) /* 视频编码参数 */
#define PRIVACY_AREA_CONFIG     GENCMDTAG(AVC_MANAGE_MODULE, 19) /* 隐私区域 */
#define AVC_STREAM_INFO         GENCMDTAG(AVC_MANAGE_MODULE, 20) /* avc流信息 */
#define AVC_PTZ_RANGE           GENCMDTAG(AVC_MANAGE_MODULE, 21) /* ptz参数范围 */
#define AVC_PTZ_CONFIG          GENCMDTAG(AVC_MANAGE_MODULE, 22) /* ptz参数设置 */
#define AVC_PTZ_CONTROL         GENCMDTAG(AVC_MANAGE_MODULE, 23) /* ptz控制 */
#define AVC_DATETIME_CONFIG     GENCMDTAG(AVC_MANAGE_MODULE, 24) /* 时间设置 */
#define AVC_NET_CONFIG          GENCMDTAG(AVC_MANAGE_MODULE, 25) /* 网络设置 */
#define AUDIO_SOURCE_FROM_CONFIG GENCMDTAG(AVC_MANAGE_MODULE, 26) /* 音频源选择 */
#define VIDEO_EFFECT_RESTORE    GENCMDTAG(AVC_MANAGE_MODULE, 27) /* 视频效果恢复默认 */
#define AVC_VIDEO_TYPE         GENCMDTAG(AVC_MANAGE_MODULE, 28) /* avc流信息 */


/* 报警及异常处理 */
#define VIDEO_MD_CONFIG         GENCMDTAG(ALARM_EXCEPTION_MODULE, 1) /* 视频动检设置 */
#define VIDEO_LOSS_CONFIG       GENCMDTAG(ALARM_EXCEPTION_MODULE, 2) /* 视频丢失设置 */
#define VIDEO_COVER_CONFIG      GENCMDTAG(ALARM_EXCEPTION_MODULE, 3) /* 视频遮挡设置 */
#define ALARM_INPUT_CONFIG      GENCMDTAG(ALARM_EXCEPTION_MODULE, 4) /* 报警输入设置 */
#define ALARMIN_SCHEDULE_CONFIG GENCMDTAG(ALARM_EXCEPTION_MODULE, 5) /* 报警输入计划 */
#define ALARMOUT_SCHEDULE_CONFIG GENCMDTAG(ALARM_EXCEPTION_MODULE, 6) /* 报警输出计划 */
#define BUZZEROUT_SCHEDULE_CONFIG GENCMDTAG(ALARM_EXCEPTION_MODULE, 7) /* 报警输出计划 */
#define EMAILSEND_SCHEDULE_CONFIG GENCMDTAG(ALARM_EXCEPTION_MODULE, 8) /* 报警输出计划 */
#define ALARMIN_SCHEDULE_CTRL   GENCMDTAG(ALARM_EXCEPTION_MODULE, 9) /* 报警输入计划控制 */
#define ALARMOUT_SCHEDULE_CTRL  GENCMDTAG(ALARM_EXCEPTION_MODULE, 10) /* 报警输出计划控制 */
#define BUZZEROUT_SCHEDULE_CTRL GENCMDTAG(ALARM_EXCEPTION_MODULE, 11) /* 蜂鸣器输出计划控制 */
#define EMAILSEND_SCHEDULE_CTRL GENCMDTAG(ALARM_EXCEPTION_MODULE, 12) /* 邮件发送计划控制 */
#define ALARMIN_MODE_CONFIG     GENCMDTAG(ALARM_EXCEPTION_MODULE, 13) /* 报警输入控制 */
#define ALARMOUT_MODE_CONFIG    GENCMDTAG(ALARM_EXCEPTION_MODULE, 14) /* 报警输出控制 */
#define SYS_EXCEPTION_CONFIG    GENCMDTAG(ALARM_EXCEPTION_MODULE, 15) /* 系统异常设置 */
#define ALARMIN_TRIGGER_CLEAR   GENCMDTAG(ALARM_EXCEPTION_MODULE, 16) /* 报警输入触发清除 */
#define BUZZER_BEEP_OUT         GENCMDTAG(ALARM_EXCEPTION_MODULE, 17) /* 蜂鸣器输出 */

/* 录像管理 */
#define REC_DISK_INFO           GENCMDTAG(RECORD_MANAGE_MODULE, 1) /* 录像磁盘信息 */
#define REC_DISK_FORMAT         GENCMDTAG(RECORD_MANAGE_MODULE, 2) /* 录像磁盘格式化 */
#define REC_DISK_MANAGE         GENCMDTAG(RECORD_MANAGE_MODULE, 3) /* 录像磁盘管理 */
#define CHANNEL_REC_CONFIG      GENCMDTAG(RECORD_MANAGE_MODULE, 4) /* 录像设置 */
#define CHANNEL_REC_CONTROL     GENCMDTAG(RECORD_MANAGE_MODULE, 5) /* 录像控制 */
#define CHANNEL_PREREC_CONTROL  GENCMDTAG(RECORD_MANAGE_MODULE, 6) /* 预录像控制 */
#define REC_MODE_CONFIG         GENCMDTAG(RECORD_MANAGE_MODULE, 7) /* 录像模式 */
#define REC_STREAM_BITRATE      GENCMDTAG(RECORD_MANAGE_MODULE, 8) /* 录像流码率 */
#define REC_SCHEDULE_CONFIG     GENCMDTAG(RECORD_MANAGE_MODULE, 9) /* 录像计划配置 */
#define REC_SCHEDULE_NORMAL     GENCMDTAG(RECORD_MANAGE_MODULE, 10) /* 普通录像计划控制 */
#define REC_SCHEDULE_DETECT     GENCMDTAG(RECORD_MANAGE_MODULE, 11) /* 视频检测录像计划控制 */
#define REC_DISK_REMOVE         GENCMDTAG(RECORD_MANAGE_MODULE, 12) /* 录像磁盘移除 */

/* 回放控制 */
#define DISPLAY_DEV_ENUM        GENCMDTAG(PLAYBACK_MANAGE_MODULE, 1) /* 枚举显示设备 */
#define DISPLAY_DEV_OPEN        GENCMDTAG(PLAYBACK_MANAGE_MODULE, 2) /* 打开显示设备 */
#define DISPLAY_DEV_CLOSE       GENCMDTAG(PLAYBACK_MANAGE_MODULE, 3) /* 关闭显示设备 */
#define DISPLAY_DEV_EFFECT      GENCMDTAG(PLAYBACK_MANAGE_MODULE, 4) /* 显示设备效果调节 */
#define DISPLAY_WINDOW_CREATE   GENCMDTAG(PLAYBACK_MANAGE_MODULE, 5) /* 创建窗口 */
#define DISPLAY_WINDOW_DESTROY  GENCMDTAG(PLAYBACK_MANAGE_MODULE, 6) /* 销毁窗口 */
#define DISPLAY_WINDOW_ZOOM     GENCMDTAG(PLAYBACK_MANAGE_MODULE, 7) /* 电子放大 */
#define DISPLAY_WINDOW_SHOW     GENCMDTAG(PLAYBACK_MANAGE_MODULE, 8) /* 窗口显示 */
#define DISPLAY_WINDOW_SET      GENCMDTAG(PLAYBACK_MANAGE_MODULE, 9) /* 窗口位置设置 */
#define DISPLAY_WINDOW_SWAP     GENCMDTAG(PLAYBACK_MANAGE_MODULE, 10) /* 窗口交换 */
#define NATIVE_PREVIEW          GENCMDTAG(PLAYBACK_MANAGE_MODULE, 11) /* 本地预览 */
#define NATIVE_PLAYBACK         GENCMDTAG(PLAYBACK_MANAGE_MODULE, 12) /* 本地回放 */
#define NATIVE_PLAYBACK_CONTROL GENCMDTAG(PLAYBACK_MANAGE_MODULE, 13) /* 回放控制 */
#define RESTORE_DEV_EFFECT      GENCMDTAG(PLAYBACK_MANAGE_MODULE, 14) /* 设备效果恢复默认 */
#define DISPLAY_JPEG_FILE       GENCMDTAG(PLAYBACK_MANAGE_MODULE, 15) /* 显示jpeg文件 */

/* 私有数据协议 */
#define QDATA_SEND_REPORT       GENCMDTAG(QDATA_MANAGE_MODULE, 1) /* 发送报告 */
#define QDATA_RECV_REPORT       GENCMDTAG(QDATA_MANAGE_MODULE, 2) /* 接收报告 */
#define QDATA_REAL_PLAY         GENCMDTAG(QDATA_MANAGE_MODULE, 3) /* 实时播放 */
#define QDATA_REC_PLAY          GENCMDTAG(QDATA_MANAGE_MODULE, 4) /* 录像播放 */
#define QDATA_PLAY_CONTROL      GENCMDTAG(QDATA_MANAGE_MODULE, 5) /* 播放控制 */
#define QDATA_REC_TSSYNC        GENCMDTAG(QDATA_MANAGE_MODULE, 6) /* 录像时间戳同步实际时间 */
#define QDATA_REC_GET_FRAME     GENCMDTAG(QDATA_MANAGE_MODULE, 7) /* 读取录像数据 */
#define QDATA_VOICE_COM         GENCMDTAG(QDATA_MANAGE_MODULE, 8) /* 语音对讲 */
#define QDATA_HEAVY_CONDING         GENCMDTAG(QDATA_MANAGE_MODULE, 9) /* 重编码 */


/* 录像检索和备份 */
#define REC_INFO_SEARCH         GENCMDTAG(SEARCH_BACKUP_MODULE, 1) /* 录像信息检索 */
#define REC_INFO_READ           GENCMDTAG(SEARCH_BACKUP_MODULE, 2) /* 录像信息读取*/
#define REC_BACKUP_DEV          GENCMDTAG(SEARCH_BACKUP_MODULE, 3) /* 备份设备 */
#define REC_BACKUP_INFO         GENCMDTAG(SEARCH_BACKUP_MODULE, 4) /* 录像备份信息 */
#define REC_BACKUP_START        GENCMDTAG(SEARCH_BACKUP_MODULE, 5) /* 录像备份启动 */
#define REC_BACKUP_PROGRESS     GENCMDTAG(SEARCH_BACKUP_MODULE, 6) /* 录像备份进度 */
#define REC_BACKUP_CANCEL       GENCMDTAG(SEARCH_BACKUP_MODULE, 7) /* 录像备份取消 */
#define REC_STREAM_OPEN         GENCMDTAG(SEARCH_BACKUP_MODULE, 8) /* 打开录像 */
#define REC_STREAM_SEEK         GENCMDTAG(SEARCH_BACKUP_MODULE, 9) /* 录像定位 */
#define REC_MONTH_INFO          GENCMDTAG(SEARCH_BACKUP_MODULE, 10) /* 录像月信息 */

#define REC_DISK_FORMAT_EX   GENCMDTAG(RECORD_MANAGE_MODULE, 13) /* 录像磁盘格式化扩展*/
#define REC_DISK_FORMAT_PARTTION GENCMDTAG(RECORD_MANAGE_MODULE, 14) /* 录像磁盘分区格式化*/
#define REC_DISK_PARTTION_INFO GENCMDTAG(RECORD_MANAGE_MODULE, 15) /* 录像磁盘分区信息获取*/

/* 网络服务控制*/
#define NET_GLOBAL_CONFIG       GENCMDTAG(NETWORK_SERVICE_MODULE, 1) /* 获取网络全局设置 */
#define NET_ETHERNET_CONFIG     GENCMDTAG(NETWORK_SERVICE_MODULE, 2) /* 以太网参数 */
#define NET_3G_CONFIG           GENCMDTAG(NETWORK_SERVICE_MODULE, 3) /* 3G参数 */
#define NET_WIFI_CONFIG         GENCMDTAG(NETWORK_SERVICE_MODULE, 4) /* WIFI参数 */
#define NET_WIFI_CARD           GENCMDTAG(NETWORK_SERVICE_MODULE, 5) /* WIFI参数 */
#define NET_WIFI_AP             GENCMDTAG(NETWORK_SERVICE_MODULE, 6) /* WIFI参数 */
#define NET_NTP_CONFIG          GENCMDTAG(NETWORK_SERVICE_MODULE, 7) /* NTP参数 */
#define NET_P2P_CONFIG          GENCMDTAG(NETWORK_SERVICE_MODULE, 8) /* P2P参数 */
#define NET_FTP_CONFIG          GENCMDTAG(NETWORK_SERVICE_MODULE, 9) /* FTP参数 */
#define NET_RTP_CONFIG          GENCMDTAG(NETWORK_SERVICE_MODULE, 10) /* RTP参数 */
#define NET_DHCP_CONFIG         GENCMDTAG(NETWORK_SERVICE_MODULE, 11) /* DHCP设置 */
#define NET_UPNP_CONFIG         GENCMDTAG(NETWORK_SERVICE_MODULE, 12) /* UPNP设置 */
#define NET_PPPOE_CONFIG        GENCMDTAG(NETWORK_SERVICE_MODULE, 13) /* PPPOE设置 */
#define NET_DDNS_CONFIG         GENCMDTAG(NETWORK_SERVICE_MODULE, 14) /* DDNS设置 */
#define NET_DDNS_SERVICE        GENCMDTAG(NETWORK_SERVICE_MODULE, 15) /* DDNS服务器 */
#define NET_EMAIL_CONFIG        GENCMDTAG(NETWORK_SERVICE_MODULE, 16) /* EMAIL设置 */
#define NET_STATUS_GET          GENCMDTAG(NETWORK_SERVICE_MODULE, 17) /* 获取网络状态 */

/* 其他功能 */
#define SYS_DEVICE_TIME         GENCMDTAG(ASSIST_MANAGE_MODULE, 1) /* 设备时间 */
#define DST_SCHEDULE_CONFIG     GENCMDTAG(ASSIST_MANAGE_MODULE, 2) /* 夏令时设置 */


#define P2P_SERVE_CONFIG          GENCMDTAG(P2P_MANAGE_MODULE, 1) /* P2P参数 */
#define P2P_EXTRA_CONFIG        GENCMDTAG(P2P_MANAGE_MODULE, 2) /* P2P扩展参数 */

/*********************************************************************
*
*       Struct
*
**********************************************************************
*/

enum {
	MODULE_CONNECT, /* 模块连接 */
	CLIENT_CONNECT, /* 客户连接 */
	MODULE_DISCONNECT, /* 模块断开 */
	CLIENT_DISCONNECT, /* 客户断开 */
	MODULE_READY, /* 模块就绪 */
	SYSTEM_READY /* 系统就绪 */
};

typedef struct {
	int msg; /* 模块消息 */
	union {
		int data; /* 消息数据 */
		void *pdata; /* 消息指针 */
	};
} comm_module_msg_t;

enum {
	AVC_DISCONNECT_ACK, /* avc断开响应 */
	AVC_STREAM_CLEAR, /* avc流全部关闭 */
};

typedef struct {
	int eventcode; /* 事件代码 */
	int eventch; /* 事件通道 */
} internal_state_t;

/* 系统事件 */
typedef struct {
	int eventtype; /* 事件类型 */
	union {
		int eventstate; /* 通用事件状态 */
		internal_state_t internalstate; /* 内部事件 */
		channel_state_t chstate; /* 通道状态 */
		stream_bitrate_t streambitrate; /* 流码率 */
		network_state_t netstate; /* 网络状态 */
		disk_state_t diskstate; /* 磁盘状态 */
		progress_state_t progstate; /* 进度状态 */
		playback_state_t playstate; /* 播放状态 */
		config_state_t configstate; /* 配置状态 */
		sys_exception_t sysexcption; /* 异常状态 */
		video_state_t type;/*视频输入类型*/
	};
} comm_sys_event_t;


#define MAX_NET_CARDS		4
#define MAC_ADDR_LEN		6

/* 用户登录 */
typedef struct {
	int libver; /* 版本号 */
	char username[CLI_USER_LEN]; /* 用户名 */
	char password[CLI_USER_LEN]; /* 密码 */
	unsigned char macaddr[MAX_NET_CARDS][MAC_ADDR_LEN]; /* 用户MAC地址 */
} comm_user_login_t;

/* 用户接受 */
typedef struct {
	int libver; /* 版本号 */
	unsigned short tcpport; /* TCP数据端口 */
	unsigned char res[2]; /* 保留 */
} comm_user_accept_t;

/* 用户信息 */
typedef struct {
	char username[CLI_USER_LEN]; /* 用户名 */
	char password[CLI_USER_LEN]; /* 密码 */
	unsigned int userright; /* 设置权限 */
	unsigned int ptzchlow; /* 云台控制低通道 */
	unsigned int ptzchhigh; /* 云台控制高通道 */
	unsigned int previewchlow; /* 预览低通道 */
	unsigned int previewchhigh; /* 预览高通道 */
	unsigned int playbackchlow; /* 回放低通道 */
	unsigned int playbackchhigh; /* 回放高通道 */
	unsigned int remotechlow; /* 远程低通道 */
	unsigned int remotechhigh; /* 远程高通道 */
	char ipaddr[CLI_ADDR_LEN]; /* ip地址，表示该用户只允许特定地址，0任意地址 */
	unsigned char macaddr[6]; /* mac地址，表示该用户只允许特定地址，0任意地址 */
	char res[18]; /* 保留 */
} comm_user_info_t;

/* 时间 */
typedef struct {
	unsigned char hour; /* 时 */
	unsigned char minute; /* 分 */
	unsigned char second; /* 秒 */
	unsigned char res; /* 保留 */
} comm_time_t;

/* 日期 */
typedef struct {
	unsigned short year; /* 年 */
	unsigned char month; /* 月 */
	unsigned char day; /* 日 */
} comm_date_t;

/* 时刻 */
typedef struct {
	comm_date_t momdate; /* 日期 */
	comm_time_t momtime; /* 时间 */
} comm_moment_t;

/* 时间段 */
typedef struct {
	comm_moment_t beginning; /* 开始时刻 */
	comm_moment_t ending; /* 结束时刻 */
} comm_duration_t;

typedef struct {
	int ch; /* 通道 */
	int win; /* 窗口 */
	int type; /* 类型 */
	comm_duration_t playdur; /* 起止时间 */
} comm_playback_t;

typedef struct {
	unsigned short type; /* 类型 录像任务 REC_TYPE_NORMAL ~ REC_TYPE_VIDEODETECT，其他为0 */
	unsigned short wday; /* 星期 0 ~ 6（星期天 ~ 星期六） */
	comm_time_t beginning; /* 开始时间 */
	comm_time_t ending; /* 结束时间 */
} comm_schedule_config_t;

typedef struct {
	int ch; /* 录像通道 */
	int type; /* 录像类型 */
	comm_duration_t dur; /* 起止时间 */
} comm_rec_duration_t;

typedef struct {
	unsigned int samplenum; /* 总帧数 */
	unsigned int samplelen; /* 总长度 */
	comm_rec_duration_t recdur; /* 录像段 */
} comm_rec_info_t;

typedef struct {
	char logtype; /* 日志命令 */
	char logopt; /* 日志操作 */
	char logch; /* 日志通道 */
	char res; /* 保留为0 */
	int logcmd; /* 日志类型 */
	comm_moment_t logmom; /* 记录时间 */
	char logtext[CLI_LOG_LEN]; /* 日志内容 */
} comm_log_info_t;

#define UPGRADE_NAME_LEN	64

typedef struct {
	unsigned short connid; /* 主机连接ID */
	unsigned short dataid; /* 数据传输ID */
} comm_link_id_t;

typedef struct {
	comm_link_id_t linkid; /* 连接ID */
	union {
		int streamindex; /* 音视频流索引号 */
		struct {
			comm_duration_t recdur; /* 录像回放时间段 */
			int rectype; /* 录像类型 */
			char sendrequest; /* 数据发送请求 0 不请求（发送数据直到结束） 1 请求 */
		} recch;
		struct {
			audio_source_t src;
			audio_encode_t enc;
		} audioch; /* 音频回传参数 */
		struct {
			int win;
			video_encode_t enc;
		} videoch; /* 视频回传参数 */
		struct {
			unsigned short tcpport; /* TCP数据端口 */
			unsigned short udpport; /* UDP数据端口 */
			unsigned short httpport; /* HTTP端口 */
			unsigned short rtspport; /* RTSP端口 */
		} portin; /* 端口信息 */
	};
} comm_link_info_t;

typedef struct {
	comm_link_id_t linkid; /* 连接ID */
	comm_duration_t recdur; /* 录像回放时间段 */
	int rectype; /* 录像类型 */
	video_encode_t enc;/*重编码需要指定的参数
						若采用现有参数值为0，其中宽和高必须指定*/
	char res[36];/*预留*/
}comm_heavy_conding_t;


/* 播放控制 */
typedef struct {
	union {
		int win; /* 窗口号（本地） */
		comm_link_id_t id; /* 连接id（远程） */
	};
	int param; /* 命令参数 */
	comm_moment_t playmom; /* 播放时间 */
} comm_play_control_t;

/* 录像时间戳同步 */
typedef struct {
	unsigned int ts; /* 数据时间戳 */
	comm_moment_t mom; /* 对应时间 */
} comm_ts_time_t;

/* 元数据类型,定义值不能超过15 */
#define DATA_TYPE_NULL			0 /* 空数据类型 */
#define DATA_CONN_AUTH			1 /* 数据连接验证 */
#define DATA_SEND_REPORT		2 /* 数据发送报告 */
#define DATA_RECV_REPORT		3 /* 数据接收报告 */
#define DATA_UPGRADE_FILE		4 /* 数据升级文件 */

enum {
	RECV_STATE_ACTIVE      = LIB_BIT00, /* 活动状态 */
	RECV_STATE_INIT_BUFFER = LIB_BIT01, /* 缓冲初始化 */
	RECV_STATE_INIT_REPORT = LIB_BIT02, /* 统计报告初始化 */
	RECV_STATE_LIST_FULL   = LIB_BIT03, /* 队列数据满 */
	RECV_STATE_DATA_HEADER = LIB_BIT08, /* 接收数据头 */
	RECV_STATE_EXT_HEADER  = LIB_BIT09, /* 接收扩展头 */
	RECV_STATE_EXT_DATA    = LIB_BIT10, /* 接收扩展数据 */
	RECV_STATE_DATA_DATA   = LIB_BIT11  /* 接收负载数据 */
};

typedef struct {
	unsigned char vf:2; /* 版本标记 */
	unsigned char cf:1; /* 校验标记 */
	unsigned char mf:1; /* 帧边界标记 */
	unsigned char xf:1; /* 扩展标记 */
	unsigned char tf:1; /* 传输控制标记 */
	unsigned char wf:1; /* 视频宽度进位标记，表示宽度不小于4096 */
	unsigned char hf:1; /* 视频高度进位标记，表示高度不小于2048 */
	unsigned char dt; /* 数据类型，见前述定义 */
	union {
		unsigned char any0; /* 数据定义 */
		struct {
			unsigned char fm:1; /* 视频交错 */
			unsigned char fs:1; /* 交错方式 */
			unsigned char fps:6; /* 视频帧率 */
		} vdo;
		struct {
			unsigned char chs:2; /* 声道数 */
			unsigned char KBps:6; /* 音频编码平均码率(KBytes每秒) */
		} ado;
	};
	unsigned char eb; /* 数据末尾字节 */
	unsigned short sn; /* 包序列号 */
	unsigned short sz; /* 数据包大小 */
	unsigned short id; /* 数据流id */
	union {
		unsigned short any1; /* 用户定义 */
		unsigned short hz; /* 音频采样率 */
		struct {
			unsigned char w; /* 视频宽度(以16像素为单位计算) */
			unsigned char h; /* 视频高度(以8像素为单位计算) */
		} pic;
	};
	unsigned int ts; /* 数据时间戳 */
} comm_data_header_t;

#define INIT_DATAHEADER(hdr) do { \
	memset(&hdr, 0, sizeof(comm_data_header_t)); \
	(hdr).vf = 1; \
} while (0)

#define SET_DATAHEADER(hdr, type, size)	do { \
	INIT_DATAHEADER(hdr); \
	(hdr).dt = type; \
	(hdr).sz = size; \
} while (0)

#define GET_PICWIDTH(hdr) ( ((hdr).wf == 0) ? (hdr).pic.w * 16 : (hdr).pic.w * 16 + 4096 )
#define GET_PICHEIGHT(hdr) ( ((hdr).hf == 0) ? (hdr).pic.h * 8 : (hdr).pic.h * 8 + 2048 )

#define SET_DATAENDBYTE(hdr, x) { (hdr).cf = 1; (hdr).eb = x; }
#define SET_TRANSFEREND(hdr) { (hdr).tf = 1; }

typedef struct {
	unsigned short xt; /* 扩展头类型 */
	unsigned short sz; /* 扩展头长度 */
} comm_extension_header_t;

typedef struct {
	int bps; /* 报告期内平均码率 */
	int jit; /* 报告期内数据抖动 */
	unsigned short recvpkt; /* 报告期内接收包数 */
	unsigned short losspkt; /* 报告期内丢失包数 */
	unsigned int lastts; /* 报告期内最后的时间戳 */
} comm_recv_report_t;

#endif /* #ifndef __NSYS_COMM_H__ */
