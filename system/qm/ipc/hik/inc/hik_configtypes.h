#ifndef _HIK_CONFIGTYPES_H_
#define _HIK_CONFIGTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "QMAPIType.h"
#include "hik_lst.h"
//#include "hik_netfun.h"


#define NAME_LEN				32				/* 用户名长度 */
#define PASSWD_LEN				16				/* 密码长度 */
#define MAX_DOMAIN_NAME			64				/* 最大域名长度 */
#define MACADDR_LEN				6				/* MAC地址长度 */
#define PATHNAME_LEN			128				/* 路径名长度 */
#define PRODUCT_MODEL_LEN		64				/* 产品型号长度(国标型号>16位) */

#define MAX_VIDEOOUT			4				/* 最大CVBS个数 */
#define MAX_VIDEOOUT_8000		2				/* 最大CVBS个数 */
#define MAX_VGA					4				/* 最大VGA个数 */
#define MAX_VGA_8000			1				/* 最大VGA个数 */
#define MAX_AUXOUT				16				/* 最大模拟矩阵输出个数 */
#define MAX_ETHERNET			2				/* 网口个数 */
#define MAX_ANALOG_CHANNUM		32				/* maximum analog channel count */
#define MAX_CHANNUM				32				/* 最大输入通道数 */
#define MAX_CHANNUM_8000	    16				/* 最大输入通道数 */
#define	MAX_DECODE_CHAN     	16				/* 最大解码通道数 */
#define MAX_ALARMIN				32				/* 最大报警输入 */
#define MAX_ALARMIN_8000	    16				/* 最大报警输入 */
#define MAX_ALARMOUT			32				/* 最大报警输出 */
#define MAX_ALARMOUT_8000		4				/* 最大报警输出 */
#define MAX_SERIAL_PORT			8				/* 最大串口个数 */
#define MAX_USB_NUM         	2				/* USB外设最大个数 */

#define MAX_IPCHANNUM       	32				/* IPC最大个数 */
#define MAX_IPC_ALARMIN     	6				/* IPC最大报警输入个数 */
#define MAX_IPC_ENCCHAN     	4				/* IPC最大报编码通道个数 */
#define MAX_IPC_ALARMOUT    	2				/* IPC最大报警输出个数 */

#define MAX_USERNUM				32				/* 最大用户数 */
#define MAX_USERNUM_8000		16				/* 最大用户数 */
#define MAX_EXCEPTIONNUM		32				/* 最大异常个数 */
#define MAX_EXCEPTIONNUM_8000	16				/* 最大异常个数 */
// fix by tw
//#define MAX_TIMESEGMENT			8				/* 一天最大8个时间段 */
#define MAX_TIMESEGMENT			4				/* 一天最大8个时间段 */
#define MAX_TIMESEGMENT_8000    4				/* 8000一天最大4个时间段 */
#define MAX_DAYS				7				/* 1周7天 */
#define PHONENUMBER_LEN			32				/* 电话号码长度 */

#define MAX_HD_COUNT			33				/* 最多33个硬盘(包括16个内置SATA硬盘、1个eSATA硬盘和16个NFS盘) */
#define MAX_HD_COUNT_8000	    16				/* 8000最多16个硬盘(包括16个内置SATA硬盘、1个eSATA硬盘和16个NFS盘) */
#define MAX_NFS_DISK			16				/* 最多16个NFS盘 */
#define MAX_NFS_DISK_8000	    8				/* 8000最多8个NFS盘 */
#define MAX_HD_GROUP			16				/* 最多16个盘组 */

#define MAX_AUXOUT_COUNT		4				/* 最大辅助输出口个数 */

#define MAX_OSD_LINE			8				/* 最大OSD字符行数(不包括通道名、日期) */
#define MAX_OSD_LINE_8000		4				/* 最大OSD字符行数(不包括通道名、日期) */
#define MAX_MASK_REGION_NUM		4				/* 最大遮挡区域个数 */
#define MAX_PREV_WINDOWS		32				/* 最大本地预览窗口数 */
#define MAX_PREV_WINDOWS_8000	16				/* 最大本地预览窗口数 */
#define MAX_PREV_MOD			8				/* 最大预览模式数目*/

#define MAX_PRESET				256				/* 云台的预置点 */
#define MAX_PRESET_8000			128				/* 云台的预置点 */
#define MAX_TRACK				32				/* 云台的轨迹 */
#define MAX_TRACK_8000			16				/* 云台的轨迹 */
#define MAX_CRUISE				32				/* 云台的巡航 */
#define MAX_CRUISE_8000			16				/* 云台的巡航 */
#define MAX_KEYPOINT			64				/* 一条巡航最多的关键点 */

#define PRODDATELEN         8			/* 生产日期 */
#define PRODNUMLEN          9			/* 9位数的序列号 */
#define SERIALNO_LEN        48			/* 序列号长度 */


#define PARAVER_071122			0x071122			/* V1.0.0的参数结构 */
#define PARAVER_090309			0x090309			/* V1.0.1的参数结构 */
#define PARAVER_091203			0x091203			/* V1.0.3 */
#define PARAVER_100316			0x100316			/* V3.1.0 */
#define PARAVER_101010			0x101010			/* V3.2.0 */
#define PARAVER_100511			0x100511			/* 81st的参数结构 */
#ifdef DZ_BEITONG_VER
#define PARAVER_110108          0x110108            /* 贝通协议的补丁 */
#endif

#ifdef HI3515
#define CURRENT_PARAVER	PARAVER_100511
#else
#ifndef DZ_BEITONG_VER
#define CURRENT_PARAVER	PARAVER_101010
#else
#define CURRENT_PARAVER	PARAVER_110108
#endif
#endif

#define MAX_VO_CHAN				MAX_VIDEOOUT	/*最大输出通道个数*/
#define MAX_VO_CHAN_8000		MAX_VIDEOOUT_8000/*最大输出通道个数*/
#define CHANNEL_MAIN			1				/*主口*/
#define CHANNEL_SEC				2				/*辅口*/
#define CHANNEL_VGA				3				/*VGA */
#define CHANNEL_AUX			    0				/*辅助输出*/

#define MAX_AO_CHAN				2				/* maximum audio out ports */
/*
  预览模式
  */
#define PREVIEW_MODE_1		1
#define PREVIEW_MODE_2		2
#define PREVIEW_MODE_4		4
#define PREVIEW_MODE_6		6
#define PREVIEW_MODE_7		7
#define PREVIEW_MODE_8		8
#define PREVIEW_MODE_9		9
#define PREVIEW_MODE_10		10
#define PREVIEW_MODE_12     12
#define PREVIEW_MODE_13		13
#define PREVIEW_MODE_16		16
#define PREVIEW_MODE_402	0x42

/*
 * Definitions for OSD
 */
#define OSD_DISABLE					0		/* 不显示OSD */
#define OSD_TRANS_WINK				1		/* 透明，闪烁 */
#define OSD_TRANS_NO_WINK			2		/* 透明，不闪烁 */
#define OSD_NO_TRANS_WINK			3		/* 不透明，闪烁 */
#define OSD_NO_TRANS_NO_WINK		4		/* 不透明，不闪烁 */

#define OSD_TYPE_ENGYYMMDD			0		/* XXXX-XX-XX 年月日 */
#define OSD_TYPE_ENGMMDDYY			1		/* XX-XX-XXXX 月日年 */
#define OSD_TYPE_ENGDDMMYY			2		/* XX-XX-XXXX 日月年 */
#define OSD_TYPE_CHNYYMMDD			3		/* XXXX年XX月XX日 */
#define OSD_TYPE_CHNMMDDYY			4		/* XX月XX日XXXX年 */
#define OSD_TYPE_CHNDDMMYY			5		/* XX日XX月XXXX年 */

#define OSD_24H_TYPE			    0		/* 24 小时制*/
#define OSD_12H_TYPE			    1		/* 12 小时制*/

/*
 * Definitions for DEVTime
 */
#define DATE_TYPE_YYMMDD			0		/* XXXX-XX-XX 年月日 */
#define DATE_TYPE_MMDDYY			1		/* XX-XX-XXXX 月日年 */
#define DATE_TYPE_DDMMYY			2		/* XX-XX-XXXX 日月年 */

/*
 * Definitions for LOGO
 */
#define LOGO_DISABLE				0		/* 不显示LOGO */
#define LOGO_TRANS					1		/* 透明 */
#define LOGO_NO_TRANS				2		/* 不透明 */

/*
 * Definitions for streamtype and resolution。
 */
#define STREAM_TYPE_VO		(STREAM_VIDEO-CONST_NUMBER_BASE)	/* Video only */
#define STREAM_TYPE_AO		(STREAM_AUDIO-CONST_NUMBER_BASE)	/* Audio only */
#define STREAM_TYPE_AV		(STREAM_MULTI-CONST_NUMBER_BASE)	/* Audio & video */

#define RESOLUTION_CIF		(CIF_FORMAT-CONST_NUMBER_BASE)
#define RESOLUTION_QCIF		(QCIF_FORMAT-CONST_NUMBER_BASE)
#define RESOLUTION_2CIF		(HCIF_FORMAT-CONST_NUMBER_BASE)
#define RESOLUTION_4CIF		(FCIF_FORMAT-CONST_NUMBER_BASE)
#define RESOLUTION_QQCIF	(QQCIF_FORMAT-CONST_NUMBER_BASE)	/* PAL:96*80, NTSC:96*64 */
#define RESOLUTION_NCIF		(NCIF_FORMAT-CONST_NUMBER_BASE)		/* 320*240 */
#define RESOLUTION_DCIF		(DCIF_FORMAT-CONST_NUMBER_BASE)		/* PAL:528*384 NTSC:528*320 */
#define RESOLUTION_UXGA		(UXGA_FORMAT-CONST_NUMBER_BASE)  	/* 1600*1200 !!!与QQCIF复用*/
#define RESOLUTION_VGA		(VGA_FORMAT-CONST_NUMBER_BASE)		/*VGA format added by lj 2007-1-27*/
#define RESOLUTION_HD720P  	(HD720p_FORMAT-CONST_NUMBER_BASE)

/*
 *definitions for videoStandard, 与 DSP/common.h中的VIDEO_STANDARD一致
 */
#define VIDEOS_NON		0
#define VIDEOS_NTSC 	1			/* NTSC制式 */
#define VIDEOS_PAL		2			/* PAL 制式 */

/* 录象类型 */
#define TIMING_REC				0x0				/* 定时录象 */
#define MOTION_DETECT_REC		0x1				/* 移动侦测录象 */
#define ALARM_REC				0x2				/* 报警录象 *//* 接近报警 */
#define ALARMORMOTION_REC		0x3				/* 报警 | 移动侦测录象 */ /* 出钞报警 */
#define ALARMANDMOTION_REC		0x4				/* 报警 & 移动侦测录象 */ /* 装钞报警 */
#define COMMAND_REC				0x5				/* 命令触发 */
#define MANUAL_REC				0x6				/* 手动录象 */
#define SHAKEALARM_REC			0x7				/* 震动报警录像 */
#define ENVIRONMENT_ALARM_REC 	0x8				/* 环境报警触发录像 */
#define ALL_RECORD_TYPE			0xff			/* 所有录象类型 */


/*异常处理类型*/
#define NO_HARDDISK_FLAG				0x80000000

#define EXCEPTIONCOUNT					5		/*异常类型种类*/
#define HARDDISKFULL_EXCEPTION			0x0		/*盘满*/
#define HARDDISKERROR_EXCEPTION			0x1		/*盘错*/
#define ETHERNETBROKEN_EXCEPTION		0x2		/*网线断*/
#define IPADDRCONFLICT_EXCEPTION		0x3		/*IP地址冲突*/
#define ILLEGALACCESS_EXCEPTION			0x4		/*非法访问*/
#define VI_EXCEPTION					0x5		/*视频信号异常*/
#define VS_MISMATCH_EXCEPTION			0x6		/*输入/输出视频制式不匹配 */
#define VIDEOCABLELOSE_EXCEPTION		0x7		/*视频无信号*/
#define AUDIOCABLELOSE_EXCEPTION        0x8		/*音频无信号*/
#define ALARMIN_EXCEPTION				0x9		/*报警输入*/
#define MASKALARM_EXCEPTION				0xa		/*遮挡报警*/
#define MOTDET_EXCEPTION				0xb		/*移动侦测*/
#define RECORDING_EXCEPTION				0xc		/*录像异常--申请不到文件或录像过程中出现异常*/
#define IPC_IP_CONFLICT_EXCEPTION       0Xd     /*ipc ip 地址冲突*/
#define MAX_EXCEPTION_TYPE				(RECORDING_EXCEPTION+1)

#define MANUAL_TRIGGER_ALARMOUT			0x10
#define MANUAL_CLEAR_ALARMOUT			0x11
#define IPC_INNERCHAN_NOTEXIST          0x20
/*异常处理方式*/
#define NOACTION						0x0		/*无响应*/
#define WARNONMONITOR					0x1		/*监视器上警告*/
#define WARNONAUDIOOUT					0x2		/*声音警告*/
#define UPTOCENTER						0x4		/*上传中心*/
#define TRIGGERALARMOUT					0x8		/*触发报警输出*/
#define SENDEMAIL						0x10	/*Send Email*/

/* RS232口使用模式 */
#define SER_MODE_PPP		0			/* PPP 模式 */
#define SER_MODE_CONSOLE	1			/* 控制口模式 dvrShell */
#define SER_MODE_TRANS		2			/* 透明传输的数据通道 */
#define SER_MODE_DIRECT		3			/* 串口直连 */

/* baud rate defines */
#define S50     	0
#define S75     	1
#define S110    	2
#define S150    	3
#define S300    	4
#define S600    	5
#define S1200   	6
#define S2400   	7
#define S4800   	8
#define S9600   	9
#define S19200  	10
#define S38400  	11
#define S57600  	12
#define S76800		13
#define S115200 	14

/* data bits defines */
#define DATAB5     	0
#define DATAB6     	1
#define DATAB7     	2
#define DATAB8     	3

/* stop bits defines */
#define STOPB1		0
#define STOPB2		1

/* parity defines */
#define NOPARITY	0
#define ODDPARITY	1
#define EVENPARITY	2

/* flow control defines */
#define	NOCTRL		0
#define SOFTCTRL	1		/* xon/xoff flow control */
#define HARDCTRL	2		/* RTS/CTS flow control */

#define SER_SPEED_SHIFT		0
#define SER_SPEED_MASK		0xf
#define SER_DATAB_SHIFT		4
#define SER_DATAB_MASK		0x3
#define SER_STOPB_SHIFT		6
#define SER_STOPB_MASK		0x1
#define SER_PARITY_SHIFT	7
#define SER_PARITY_MASK 	0x3
#define SER_FLOWCTRL_SHIFT	9
#define SER_FLOWCTRL_MASK	0x3

#ifdef EVDO
#define EVDO_MAXCELLNUM 5
#define EVDO_CELLNUMLEN 16
#endif

/*
  用户管理
 */
#define LOGIN_NO_LOGIN		0			/*未登录*/
#define LOGIN_DEFAULT_RIGHT	0x00014014
				/* 默认权限:包括本地和远程回放,本地和远程查看日志和状态,本地和远程关机/重启
					LOCALPLAYBACK|LOCALSHOWSTATUS|REMOTEPLAYBACK|REMOTESHOWSTATUS
				*/
#define LOGIN_SUPERUSER		0x000d7117
				/* 高级操作员:包括本地和远程控制云台,本地和远程手动录像,本地和远程回放,语音对讲和远程预览
				    本地备份,本地/远程关机/重启
					LOCALPTZCONTROL|LOCALMANUALREC|LOCALPLAYBACK|REMOTEPTZCONTROL
					|REMOTEMANUALREC|REMOTEPLAYBACK|REMOTEVOICETALK|REMOTEPREVIEW
				*/
#define LOGIN_ADMIN			0xffffffff	/*管理员*/

/*用户权限等级*/
#define USER_PRIO_HIGH     	2
#define USER_PRIO_MIDDLE	1
#define USER_PRIO_LOW		0


/*
 * Definitions for user permission
 */
#define LOCALPTZCONTROL		0x1					/* 本地控制云台 */
#define LOCALMANUALREC		0x2					/* 本地手动录象 */
#define LOCALPLAYBACK		0x4					/* 本地回放 */
#define LOCALSETPARAMTER	0x8					/* 本地设置参数 */
#define LOCALSHOWSTATUS		0x10				/* 本地查看状态、日志 */
#define LOCALHIGHOP			0x20				/* 本地高级操作(升级，格式化) */
#define LOCALSHOWPARAMETER	0x40				/* 本地查看参数 */
#define LOCALMANAGECAMERA	0x80				/* 本地管理模拟和IP camera */
#define LOCALBACKUP		    0x100				/* 本地备份 */
#define LOCALREBOOT		    0x200				/* 本地关机/重启 */

#define REMOTEPTZCONTROL	0x1000				/* 远程控制云台 */
#define REMOTEMANUALREC		0x2000				/* 远程手动录象 */
#define REMOTEPLAYBACK		0x4000				/* 远程回放 */
#define REMOTESETPARAMETER	0x8000				/* 远程设置参数 */
#define REMOTESHOWSTATUS	0x10000				/* 远程查看状态、日志 */
#define REMOTEHIGHOP		0x20000				/* 远程高级操作(升级，格式化) */
#define REMOTEVOICETALK		0x40000				/* 远程发起语音对讲 */
#define REMOTEPREVIEW		0x80000				/* 远程预览 */
#define REMOTEALARM			0x100000			/* 远程请求报警上传、报警输出 */
#define REMOTECTRLLOCALOUT	0x200000			/* 远程控制，本地输出 */
#define REMOTESERIALCTRL	0x400000			/* 远程控制串口 */
#define REMOTESHOWPARAMETER	0x800000			/* 远程查看参数 */
#define REMOTEMANAGECAMERA	0x1000000			/* 远程管理模拟和IP camera */
#define REMOTEREBOOT		0x2000000			/* 远程关机/重启 */

/*参数类型*/
#define PARA_VIDEOOUT	0x1
#define PARA_IMAGE		0x2
#define PARA_ENCODE		0x4
#define PARA_NETWORK	0x8
#define PARA_ALARM		0x10
#define PARA_EXCEPTION	0x20
#define PARA_DECODER	0x40				/*解码器*/
#define PARA_RS232		0x80
#define PARA_PREVIEW	0x100
#define PARA_SECURITY	0x200
#define PARA_DATETIME	0x400
#define PARA_FRAMETYPE	0x800				/*帧格式*/
#define PARA_HARDDISK   0x1000
#define PARA_AUTOBOOT   0x2000
#define PARA_HOLIDAY    0x4000
#define PARA_IPC	    0x8000 				/*IP通道配置*/
#define PARA_DEVICE     0X10000				/*设备配置信息*/

/*ppp 属性*/
#define PPPMODE_MASK		0x1					/* PPP模式：0为主动；1为被动 */
#define CALLBACK_MASK		0x2					/* 回拨：0为不需要；1为需要 */
#define CALLBACKMODE_MASK	0x4					/* 回拨模式：0为固定号码；1为用户指定 */
#define ENCRYPTION_MASK		0x8					/* 加密：0为不需要；1：为需要 */

/* 输入方法 */
#define NET_INTERCEPTION	0
#define NET_RECEIVE			1
#define SERIAL_DIRECT_INPUT	2
#define SERIAL_ATM_INPUT	3

//#define NET_INTERCEPTION		0
#define SERIAL_INTERCEPTION 	1
#define NET_PROTOCOL			2
#define SERIAL_PROTOCOL		3
#define SERIAL_SERVER			4

#define OLD_ATM_ALARMIN_NUMS		5
/* ATM机类型 */
#define ATM_NCR				0
#define ATM_DIEBOLD			1
#define ATM_WINCOR_NIXDORF	2
#define ATM_SIEMENS			3
#define ATM_OLIVETTI		4
#define ATM_FUJITSU			5
#define ATM_HITACHI			6
#define ATM_SMI				7
#define ATM_IBM				8
#define ATM_BULL			9
#define ATM_YIHUA			10
#define ATM_LIDE			11
#define ATM_GUANGDIAN		12
#define ATM_MINIBANL		13
#define ATM_GUANGLI			14
#define ATM_EASTCOM			15
#define ATM_CHENTONG		16
#define ATM_NANTIAN			17
#define ATM_XIAOXING		18
#define ATM_YUYIN			19
#define ATM_TAILITE			20
#define ATM_DRS918			21

#define ATM_KALATEL			22
#define ATM_NCR_2			23

#define ATM_NXS				24

#define MAX_ATM_TYPE		ATM_NXS

/* IP camera factory */
#define FAC_HIKVISION       0
#define FAC_PANASONIC		1
#define FAC_SONY			2	
#define FAC_AXIS	       	3
#define FAC_INFINOVA      	4
#define FAC_SANYO			5
#define FAC_ZAVIO           6

/* backup device type */
#define BACKUP_USB_DEV		1
#define BACKUP_USB_HD_DEV	2
#define BACKUP_USB_CDR_DEV	3
#define BACKUP_IDE_CDR_DEV	4
#define BACKUP_SATA_DEV		5
#define BACKUP_SATA_CDR_DEV	6
/*复制文件*/
#define ITEM_FILE_COPY  30		/*最大可复制文件数，也用于最大可复制文件片断*/

/* parameter of searching file */
#define MAX_DATE_TIME	(0x7fffffff)
#define MIN_DATE_TIME	(0)
#define MAX_DATE_YEAR	(2037)
#define NO_MATCH_CHAN	(-1)
#define NO_MATCH_TYPE	(0xff)
#define NO_MATCH_TIME	(0)

/* NIC type */
#define NET_IF_10M_HALF		1					/* 10M ethernet */
#define NET_IF_10M_FULL		2
#define NET_IF_100M_HALF	3					/* 100M ethernet */
#define NET_IF_100M_FULL	4
#define NET_IF_1000M_FULL	5					/* 1000M ethernet */
#define NET_IF_AUTO			6


/* minimum and maximum value of MTU */
#define MIN_MTU_VALUE		500
#define MAX_MTU_VALUE		1500

/* 通道支持的最大分辨率 */
#define MAX_CIF_FORMAT		0			/* QCIF, CIF */
#define MAX_DCIF_FORMAT 	1			/* QCIF, CIF, 2CIF, DCIF */
#define MAX_4CIF_FORMAT 	2			/* QCIF, CIF, 2CIF, DCIF, 4CIF */
#define MAX_2CIF_FORMAT 	3			/* QCIF, CIF, 2CIF */
#define MAX_QCIF_FORMAT 	4			/* QCIF */

/*
  结构的成员必须注意对齐: UINT64必须8字节对齐，UINT32必须4字节对齐。
*/
#define CFG_MAGIC				0x484b5753		/* "HKWS", */
#define PARAM_START_OFFSET 		12

/*
 	消息队列默认模式
*/
#define DEFAULT_MSG_MODE   		0600

/*
	禁止移动侦测
*/
#define DISABLE_MOTDET	0xff

#ifdef DZ_BEITONG_VER
#define MAX_BT_CARD_LEN      160                //定义贝通卡号的长度      
#endif


/*
	坐标
*/
typedef struct
{		/* 4 bytes */
	UINT16	x;									/* 横坐标 */
	UINT16	y;									/* 纵坐标 */
} COORDINATE;

/*
	尺寸
*/
typedef struct
{		/* 4 bytes */
	UINT16		width;							/* 宽度 */
	UINT16		height;							/* 高度 */
} SIZE;

/*
	矩形区域
*/
typedef struct
{		/* 8 bytes */
	COORDINATE	upLeft;							/* 左上角坐标 */
	SIZE		size;							/* 大小(宽度和高度) */
} ZONE;


/*
	矩形区域
*/
typedef struct
{		/* 12 bytes */
	COORDINATE	upLeft;							/* 左上角坐标 */
	COORDINATE	dwRight;						/* 右下角坐标*/
	UINT32		enableWork;						/* 是否启用*/
} ZONE_EX;


/*
	发送邮件信息
*/
typedef struct{
	UINT32	bSendEmail;
	UINT32  haveSendEmail;
	UINT32	exceptionType;
	UINT64	inputChannel;
	UINT64  trigChannel;
	UINT64	saveInputChannel;
	UINT16	IPCInputAlarmNo[MAX_IPCHANNUM];
	UINT16	IPCsaveInputAlarmNo[MAX_IPCHANNUM];
	UINT64	saveTrigChannel;
	time_t  eventTime;
} CTRL_EMAILINFO;

/*
 * 报警提示信息结构
 */
typedef struct
{
	NODE node;
	int alarmType;
	UINT32 channel;				/* channel no. */
	UINT8 ipStr[MAX_DOMAIN_NAME];   //域名或IP地址
}ALARM_TIP;

/* HD_INFO: 用于本地显示硬盘状态
*/
#define HD_STAT_OK			0	/* 正常 */
#define HD_STAT_UNFORMATTED		1	/* 未格式化 */
#define HD_STAT_ERROR			2	/* 错误 */
#define HD_STAT_SMART_FAILED		3	/* SMART状态 */
#define HD_STAT_MISMATCH			4	/* 不匹配 */
#define HD_STAT_IDLE				5	/* 休眠*/
#define HD_STAT_NOT_ONLINE			6	/* 网络硬盘不在线*/

typedef struct
{	/* 24 bytes */
	UINT16 ctrl;
	UINT16 drive;
	UINT32 capacity;
	UINT32 freeSpace;
	UINT32 hdStatus;
	UINT16 bIdle;
	UINT16 bReadOnly;
	UINT16 bRedund;
	UINT16 hdGroup;
	UINT32 hdType;
}HD_INFO;

/*
	时间段
*/
#define MAKE_HM(hour, min) 		(((hour)<<8) | (min))
#define GET_HOUR_FROM_HM(hm)	(((hm)>>8) & 0xff)
#define GET_MIN_FROM_HM(hm)		((hm) & 0xff)
typedef struct
{		/* 4 bytes */
	UINT16		startTime;						/* 开始时间 */
	UINT16		stopTime;						/* 停止时间 */
} TIMESEGMENT;

typedef struct
{		/* 104 bytes */
	COORDINATE	upLeft;							/* 左上角坐标 */
	UINT8		dispLine;						/* 是否显示该行 */
	UINT8		charCnt;						/* 字符个数 */
	UINT8		osdCode[88];					/* OSD字符，考虑百万像素的IP Camera，增加每行的最大字符数为88 */
	UINT8		res[10];
}OSD_LINE;

/*
	OSD数据结构
*/
typedef struct
{		/* 848 bytes */
	UINT8       osdAttrib;						/* OSD属性:透明，闪烁 */
	UINT8		hourOsdType;					/* 24小时制或者12小时制*/
	UINT8		res[2];	
	/* 日期/通道名 */
	UINT8		dispDate;						/* 是否显示日期 */
	UINT8		dateOsdType;					/* 年月日格式类型 */
	UINT8		dispWeek;						/* 是否显示星期 */
	UINT8		dispNameOsd;					/* 是否显示通道名称 */
	COORDINATE	dateUpLeft;						/* 日期左上角坐标 */
	COORDINATE	nameOsdUpLeft;					/* 通道名称显示位置 */
	/* 其它OSD字符 */
	OSD_LINE	additOsdLine[MAX_OSD_LINE];		/* 其它OSD字符 */
} OSDPARA;

/*
	LOGO参数
*/
typedef struct
{		/* 8 bytes */
	UINT8		dispLogo;						/* 是否叠加LOGO */
	UINT8		logoAttrib;						/* LOGO属性:透明，闪烁 */
	UINT8		res[2];
	COORDINATE	logoUpLeft;						/* LOGO位置 */
}LOGOPARA;

/*
 * Compression parameters
 */
typedef struct
{		/* 32 bytes */
	UINT8		streamType;						/* 码流类型: 复合、视频、音频 */
	UINT8		resolution;						/* 分辨率：QCIF、CIF、2CIF、DCIF、4CIF、D1 */
	UINT8		bitrateType;					/* 定码率、变码率 */
	UINT8		videoEncType;					/* 视频编码标准 */
	UINT8		audioEncType;					/* 音频编码标准 */
	UINT8		BFrameNum;						/* B帧个数: 0:BBP, 1:BP, 2:P */
	UINT8		EFNumber;						/* E帧个数 */
	UINT8		res[13];
	UINT16	 	intervalFrameI;					/* I帧间隔 */
	UINT16		quality;						/* 图象质量 */
	UINT32 		maxVideoBitrate;				/* 视频码率上限(单位：bps) */
	UINT32 		videoFrameRate;					/* 视频帧率，PAL：1/16-25；NTCS：1/16-30 */
} COMPRESSION;

/*
	录像计划数据结构
*/
typedef struct
{		/* 8 bytes */
	TIMESEGMENT	recActTime;						/* 时间段 */
	UINT8		type;							/* 类型(不同时间段可有不同的类型) */
	UINT8		compParaType;					/* 压缩参数类型 */
	UINT8		res[2];
} RECORDSCHED;

/*
	全天录像数据结构
*/
typedef struct
{		/* 8 bytes */
	UINT8		bAllDayRecord;					/* 是否全天录像 */
	UINT8		recType;						/* 录象类型 */
	UINT8		compParaType;					/* 压缩参数类型 */
	UINT8		res[5];
}RECORDDAY;
#if 0
/*
*IPcamrea record parameters
*/
typedef struct
{	/*546 bytes*/
	UINT8			enableRecord;				/* 是否录象 */
	UINT8			enableRedundancyRec;		/* 是否冗余录像 */
	UINT8			enableAudioRec;				/* 复合流编码时是否记录音频数据 */
	UINT8			res[17];
	RECORDDAY		recordDay[MAX_DAYS];
	RECORDSCHED		recordSched[MAX_DAYS][MAX_TIMESEGMENT];	/* 记录参数 */
	UINT32			recordDelay;				/* 移动侦测/报警录象的延时 */
	UINT32          preRecordTime;				/* 预录时间*/
	UINT32			recorderDuration;			/* 录像保存的最长时间 */
} IPCRECORD;
#endif

/* backup device type */
#define BACKUP_USB_DEV		1
#define BACKUP_USB_HD_DEV	2
#define BACKUP_USB_CDR_DEV	3
#define BACKUP_IDE_CDR_DEV	4
#define BACKUP_SATA_DEV		5
#define BACKUP_SATA_CDR_DEV	6
#define ITEM_FILE_COPY  	30		/*最大可复制文件数，也用于最大可复制文件片断*/
typedef struct
{	/*368 bytes*/
	UINT32 saveDeviceType;	                /*存储设备类型*/
	UINT32 frameNumber;						/*文件片断个数*/
	UINT32 file[ITEM_FILE_COPY];          	/*文件序号*/
	UINT32 startTime[ITEM_FILE_COPY];		/*复制片断的开始时间，用于片断复制，单位为秒*/
	UINT32 endTime[ITEM_FILE_COPY];			/*复制片断的结束时间，用于片断复制，单位为秒*/
}copyFile_t;

/*
	节假日数据结构
*/
typedef struct
{		/* 32 bytes */
	UINT8		weekendStart;					/* 周末起始日 */
	UINT8		weekendDays;					/* 天数 */
	UINT16		res[15];						/* 可用于设置其他节假日 */
}HOLIDAYPARA;

/*
	异常处理数据结构
*/
typedef struct
{		/* 40 bytes */
	UINT32 		handleType;						/* 异常处理,异常处理方式的"或"结果 */
	UINT32		alarmOutTriggered;				/* 被触发的报警输出(此处限制最大报警输出数为32) */
	UINT8		IPCalarmOutTriggered[MAX_IPCHANNUM]; /* 被触发的IPC 报警输出 */
}EXCEPTION;

/*
	本地监视器警告数据结构
*/
typedef struct
{		/* 32 bytes */
	UINT32		warnOutChan;					/* 0:主输出, 1:辅输出, 2:矩阵输出1, 3:矩阵输出2, 4:矩阵输出3, 5:矩阵输出4 */
	UINT32		warnChanSwitchTime;				/* 报警通道单画面预览切换时间 */
	UINT8		res[24];
}LOCALWARNPARA;

/*
	移动侦测异常处理结构
*/
typedef struct
{
	TIMESEGMENT	armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 布防时间 */
	EXCEPTION	handleType;							/* 异常处理方式 */
	UINT64		motTriggerRecChans;					/* 移动侦测触发的录象通道 */
	UINT8		enableMotDetAlarm;					/* 移动侦测报警 */
	UINT8		precision;							/* 移动侦测算法的进度: 0--16*16, 1--32*32, 2--64*64 ... */
	UINT8		res[6];
}MOTDETEXECPARA;

/*
	移动侦测数据结构
*/
typedef struct
{		/* 1024 bytes */
#if 0
	UINT64		motionLine[36];					/* 36行44/45列方阵，每个方块表示16*16象素。*/
												/* 每个64位长整型数据的0-43/44位分别对应每行中从左到右的44/45个块。*/
#endif
	UINT32		motionLine[64][3];				/* 考虑到百万像素的IP Camera，整个屏幕最多分为(96*64)个移动侦测‘块’，
												   每个块的大小由‘precision’确定。
												   这样，即使是500万像素(2592*1944)的IP Camera，如果每个块是32*32像素，
												   整个屏幕分为81*61块。
												 */
	UINT8		motionLevel;					/* 动态检测灵敏度，取0xff时关闭;(6最低;0最高) -> (0最低;5最高) */
				/* 移动侦测算法的进度: 0--16*16, 1--32*32, 2--64*64 ... */
	UINT8		res[7];
	MOTDETEXECPARA motdetExec;
} MOTDETECTPARA;

/*
	移动侦测数据结构扩展(用于其他厂家的IPC)
*/
typedef struct
{		/* 1024 bytes */

	ZONE_EX     motionArea[8];					/* 考虑到后续扩展，暂定为8个区域*/
	UINT8       res2[672];
	UINT8		motionLevel;					/* 动态检测灵敏度，取0xff时关闭;(6最低;0最高) -> (0最低;5最高) */
				/* 移动侦测算法的进度: 0--16*16, 1--32*32, 2--64*64 ... */
	UINT8		res[7];
	MOTDETEXECPARA motdetExec;
} MOTDETECTPARA_EX;

/*
	遮挡报警数据结构
*/
typedef struct
{		/* 256 bytes */
	ZONE		area;							/* 遮挡报警区域 */
	TIMESEGMENT	armTime[MAX_DAYS][MAX_TIMESEGMENT];/* 遮挡报警布防时间 */
	EXCEPTION	handleType;						/* 遮挡处理 */
	UINT8		maskAlarmLevel;					/* 灵敏度 */
	UINT8		enableMaskAlarm;				/* 遮挡报警 */
	UINT8		res[14];
}MASKALARMPARA;

/*
	视频/音频信号丢失数据结构
*/
typedef struct
{		/* 240 bytes */
	TIMESEGMENT	armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 视频信号丢失布防时间 */
	EXCEPTION	handleType;						/* 视频信号丢失处理 */
	UINT8		enableAlarm;					/* 视频信号丢失报警 */
	UINT8		res[7];
}SIGNALLOSTPARA;

/*
	遮挡数据结构
*/
typedef struct
{		/* 40 bytes */
	ZONE		hideArea[MAX_MASK_REGION_NUM];		/* 遮挡区域 */
	UINT8		enableHide;						/* 是否启动遮挡 */
	UINT8		res[7];
}VIHIDEPARA;

/*
	RS232 parameter
*/
typedef struct
{		/* 8 bytes */
	UINT8		speed;							/* 波特率 */
	UINT8		databits;						/* 数据位 */
	UINT8		stopbits;						/* 停止位 */
	UINT8		parity;							/* 校验 */
	UINT8		flowctrl;						/* 流控 */
	UINT8		rs232Usage;						/* RS232口使用方式 */
	UINT8		res[2];
} RS232PARA;

/*
	DVR实现巡航数据结构
*/
#define CRUISE_MAX_PRESET_NUMS	32
typedef struct
{		/* 192 bytes */
	UINT8 		presetNo[CRUISE_MAX_PRESET_NUMS];	/* 预置点号 */
	UINT8 		cruiseSpeed[CRUISE_MAX_PRESET_NUMS];/* 巡航速度 */
	UINT16 		dwellTime[CRUISE_MAX_PRESET_NUMS];	/* 停留时间 */
	UINT8		enableThisCruise;					/* 是否启用 */
	UINT8		res[63];
}CRUISE_PARA;

/*
	PTZ数据结构
*/
typedef struct
{		/* 344 bytes */
	UINT8		speed;							/* 波特率 */
	UINT8		databits;						/* 数据位 */
	UINT8		stopbits;						/* 停止位 */
	UINT8		parity;							/* 校验 */
	UINT8		flowctrl;						/* 流控 */
	UINT8		res1[3];
    UINT16		decoderModel;					/* 解码器类型 */
	UINT16		decoderAddress;					/* 解码器地址 */
	UINT8 		ptzPreset[MAX_PRESET/8];		/* 预置点是否设置: 0--没有设置， 1--已设置 */
	UINT8   	ptzCruise[MAX_CRUISE];			/* 巡航是否设置  : 0--没有设置， 1--已设置 */
	UINT8		ptzTrack[MAX_TRACK];			/* 轨迹是否设置  : 0--没有设置， 1--已设置 */
	CRUISE_PARA	ptzCruisePara;					/* DVR实现PTZ巡航功能参数 */
	UINT8		res2[44];
} PANTILTZOOM;

/*
* 视频颜色结构
*/
typedef struct
{	/*4 bytes*/
	UINT8		brightness; 				/* 亮度   (0-255) */
	UINT8		contrast;					/* 对比度 (0-255) */
	UINT8		tonality;					/* 色调   (0-255) */
	UINT8		saturation; 				/* 饱和度 (0-255) */
}COLOR;

/*
* 视频颜色配置结构
*/
typedef struct
{	/*64 bytes*/
	COLOR color[MAX_TIMESEGMENT];
	TIMESEGMENT handleTime[MAX_TIMESEGMENT];
}VICOLOR;

/*
* 录像配置结构
*/
typedef struct
{	/*528 bytes*/
	UINT8			enableRecord;				/* 是否录象 */
	UINT8			enableRedundancyRec;		/* 是否冗余录像 */
	UINT8			enableAudioRec;				/* 复合流编码时是否记录音频数据 */
#ifdef HI3515
	UINT8 			res1;						/* 加一个字节*/
#endif
	RECORDDAY		recordDay[MAX_DAYS];		/* 全天录像参数 */
	RECORDSCHED		recordSched[MAX_DAYS][MAX_TIMESEGMENT];	/* 录像参数参数 */
	UINT32			preRecordTime;				/* 预录时间 */
	UINT32			recordDelay;				/* 移动侦测/报警录象的延时 */
	UINT32			recorderDuration;			/* 录像保存的最长时间 */
	UINT8		    res2[12];					/* 补为12字节*/
}RECORDPARA;

/*
 * Video in(camera) parameters
 */
typedef struct
{		/* 3904 bytes */
	UINT8			channelName[NAME_LEN];		/* 通道名称 */
	VICOLOR 		viColor;					/* 视频颜色配置结构*/
	OSDPARA			osdPara;					/* OSD参数 */
	LOGOPARA		logoPara;					/* LOGO参数 */

	UINT8			videoStandard;				/* 视频制式*/
	UINT8			enableWork;					/* 是否使能该模拟通道*/
	UINT8			res1[6];

	RECORDPARA		recPara;					/* 录像参数 */

	COMPRESSION		normalHighCompPara;			/* 定时高压缩参数 */
	COMPRESSION		normalLowCompPara;			/* 定时低压缩参数 */
	COMPRESSION		eventCompPara;				/* 事件压缩参数 */
	COMPRESSION		netCompPara;				/* 网传压缩参数 */

	SIGNALLOSTPARA	viLostPara;					/* 视频信号丢失参数 */
	SIGNALLOSTPARA	aiLostPara;					/* 音频信号丢失参数 */
	union
	{
		MOTDETECTPARA	para;					/* 移动侦测参数 */
		MOTDETECTPARA_EX para_ex;				
	}motDetPara;
	MASKALARMPARA	maskAlarmPara;				/* 遮挡报警参数 */
	VIHIDEPARA		videoHidePara;				/* 视频遮挡参数 */

	PANTILTZOOM		ptzPara;					/* 解码器参数 */
	UINT8			res2[240];
} VIDEOIN;

/*
 * Video out数据结构
 */
typedef struct
{		/* 32 bytes */
	UINT8		videoStandard;					/* 输出制式	*/
	UINT8		menuAlphaValue;					/* 菜单与背景图象对比度 */
	UINT16		idleTime;						/* 屏幕保护时间 */
	UINT16		voOffset;						/* 视频输出偏移 */
	UINT16		voBrightnessLevel;				/* 视频输出亮度 */
	UINT8		voMode;							/* 启动后视频输出模式(0--菜单/1--预览) */
	UINT8		enableScaler;					/* 启动缩放 */
	UINT8		res[22];
} VIDEOOUT;

/*
	VGA数据结构
*/
typedef struct
{		/* 16 bytes */
	UINT8		resolution;						/* 分辨率 */
	UINT8		freq;							/* 刷新频率 */
	UINT16		brightness;						/* 亮度 */
	UINT8		res[12];
} VGAPARA;

/*
 * MATRIX输出参数结构
 */
typedef struct
{		/* 80 bytes */
	UINT16		order[MAX_CHANNUM];				/* 预览顺序, 0xff表示相应的窗口不预览 */
	UINT16		switchTime;						/* 预览切换时间 */
	UINT8		res[14];
}MATRIXPARA;

/*
 * Preview parameters
 */
typedef struct
{	/* 280 bytes */
	UINT16		switchInterval;					/* 预览切换画面间隔时间(单位：秒)。0为不切换 */
	UINT8		mode;							/* 通道预览模式 1: 1画面 2: 4画面 3: 6画面 4: 8画面
												                                     5: 9画面 6: 12画面 7: 16画面 */
	UINT8		enableAudio;					/* 是否预览音频 */
	UINT8  		order[MAX_PREV_MOD][MAX_PREV_WINDOWS];				/* 预览顺序, 0xff表示相应的窗口不预览 */
	UINT8		res[20];
} PREVIEW;

/*
 * Monitoralarm parameters
 */
typedef struct
{   /*24 bytes*/
	UINT16      alarmChanSwitchTime;			/* 报警通道切换时间 */
	UINT16 		alarmOutChan;					/* 选择报警弹出大画面的输出通道: 1:主输出/ 2:辅输出/3:VGA*/
	UINT8		res[20];
}MONITORALARM;

/*
 * 硬盘数据结构
 */
typedef struct
{		/* 640 bytes */
	UINT64		chansUseHD[MAX_HD_COUNT];		/* 盘组:使用该硬盘的通道 */
	UINT8		HDProperty[MAX_HD_COUNT];		/* 硬盘属性:正常盘--0/只读盘--1/冗余盘--2*/
	UINT8		res1[MAX_HD_COUNT];		
	UINT8		checkHDMatch[MAX_HD_COUNT];		/* 是否检测硬盘更换 */
	UINT8		cyclicRecord[MAX_HD_COUNT];		/* 硬盘循环写入 */
	UINT32		HDGroup[MAX_HD_COUNT];		    /* 盘组分布情况，对应bit 位数值为该硬盘盘组号*/
	UINT8		sataFunc;						/* eSATA的用法:盘库/备份 */
	UINT8		hdSpaceThreshold;				/* 0-100，硬盘剩余空间不足报警 */
	UINT8		bEnableSMART;					/* S.M.A.R.T开关 */
	UINT8  		res2[109];
}HDPARA;

/*
	IP地址：包括IPv4和IPv6格式的地址
*/
typedef struct
{
	/* 24 bytes */
	struct in_addr	v4;							/* IPv4地址 */
	struct in6_addr	v6;							/* IPv6地址 */
	UINT8			res[4];
}U_IN_ADDR;

/* 网盘的IP 地址与类型
结构体的长度与U_IN_ADDR 保持一致*/
typedef struct
{
	struct in_addr	v4;							/* IPv4地址 */
	struct in6_addr	v6;							/* IPv6地址 */
	UINT16			port;						/* 端口号*/
	UINT8			type;						/* 网盘的类型*/
	UINT8			res;
}NETHD_ADDR_TYPE;

/*
	网络接口数据结构
*/
typedef struct
{		/* 64 bytes */
	UINT32		mtu;							/* MTU */
   	U_IN_ADDR	ipAddress;						/* IP地址 */
   	U_IN_ADDR	ipMask;							/* IP掩码 */
	UINT16		ipPortNo;						/* IP端口号 */
	UINT8		macAddr[MACADDR_LEN];			/* 物理地址，只用于显示 */
	UINT8		mediaType;						/* 介质类型 */
	UINT8		res2[23];
} ETHERNET;

/*
	NTP数据结构
*/
typedef struct
{		/* 80 bytes */
	UINT8		ntpServer[64];					/* Domain Name or IP addr of NTP server */
	UINT16		interval;						/* adjust time interval(hours) */
	UINT8		enableNTP;						/* enable NPT client */
	UINT8       res1;
	UINT16      ntpPort;							/*IP port for NTP server*/
	UINT8	    res2[10];
}NTPPARA;

/*
	DDNS数据结构
*/
typedef struct{
	UINT8		username[NAME_LEN];				/* DDNS账号用户名/密码 */
	UINT8		password[PASSWD_LEN];
	UINT8		domainName[MAX_DOMAIN_NAME];		/* 域名 */
	UINT8		serverName[MAX_DOMAIN_NAME];	/* DDNS server 域名/IP */
	UINT16		ddnsPort;                       /* DDNS port*/
	UINT8       res[10];
}SINGLE_DDNS;

#define MAX_DDNS_NUMS        10
typedef struct
{		/* 144 bytes */
	UINT8		enableDDNS;						/* 是否启用 */
	UINT8		hostIdx;								/* 选用那种DNS 协议*/
	UINT8       res[2];
	SINGLE_DDNS ddns[MAX_DDNS_NUMS];
}DDNSPARA;

/*
	SMTP数据结构
*/
typedef struct
{		/* 640 bytes */
	UINT8		account[NAME_LEN];				/* 账号/密码 */
	UINT8		password[NAME_LEN];
	struct
	{
		UINT8	name[NAME_LEN];					/* 发件人姓名 */
		UINT8	address[MAX_DOMAIN_NAME];		/* 发件人地址 */
	}sender;
	UINT8		smtpServer[MAX_DOMAIN_NAME];	/* smtp服务器 */
	UINT8		pop3Server[MAX_DOMAIN_NAME];	/* pop3服务器 */
	struct
	{
		UINT8	name[NAME_LEN];					/* 收件人姓名 */
		UINT8	address[MAX_DOMAIN_NAME];		/* 收件人地址 */
	}receiver[3];								/* 最多可以设置3个收件人 */
	UINT8		bAttachment;					/* 是否带附件 */
	UINT8		bSmtpServerVerify;				/* 发送服务器要求身份验证 */
	UINT8       mailinterval;					/* 时间间隔 */
	UINT8 	bSslServerVerify;                   /*是否启用ssl发送邮件*/
	UINT32    smtpServerPort;                      /*ssl 服务器端口*/
	UINT8   res[56];
}EMAILPARA;

/*
	NFS数据结构
	attention:
	caofeng 2009.6 新增ISCSI 网络硬盘，为了与NAS 保持一致并最小改动，名称依然使用nfs
*/
typedef struct
{		/* 152 bytes */
	NETHD_ADDR_TYPE nfsHostIPaddr;				/* 网盘的IP 地址与类型(NAS, ISCSI...)*/
	UINT8		nfsDirectory[128];				/* NFS主机献出的目录名 */
}NFSPARA;

typedef NFSPARA ISCSIPARA;

/*
	网络数据结构
*/
typedef struct
{		/* 3936 bytes */
	ETHERNET	etherNet[MAX_ETHERNET];			/* 以太网口 */
	U_IN_ADDR	manageHost1IpAddr;				/* 主管理主机IP地址 */
	U_IN_ADDR	manageHost2IpAddr;				/* 辅管理主机IP地址 */
	U_IN_ADDR	alarmHostIpAddr;				/* 报警主机IP地址 */
	UINT16 		manageHost1Port;				/* 主管理主机端口号 */
	UINT16 		manageHost2Port;				/* 辅管理主机端口号 */
	UINT16		alarmHostIpPort;				/* 报警主机端口号 */
	UINT8       	bUseDhcp;						/* 是否启用DHCP */
	UINT8		res1[9];
	U_IN_ADDR	dnsServer1IpAddr;				/* 域名服务器1的IP地址 */
	U_IN_ADDR	dnsServer2IpAddr;				/* 域名服务器2的IP地址 */
	UINT8		ipResolver[MAX_DOMAIN_NAME];	/* IP解析服务器域名或IP地址 */
	U_IN_ADDR	ipResolverAddr;                 /* IP解析服务器IP地址 */
	UINT16		ipResolverPort;					/* IP解析服务器端口号 */
	UINT16		httpPortNo;						/* HTTP端口号 */
	UINT8		res2[4];
	U_IN_ADDR	multicastIpAddr;				/* 多播组地址 */
	U_IN_ADDR	gatewayIpAddr;					/* 网关地址 */
	NFSPARA		nfsDiskParam[MAX_NFS_DISK]; 	/* 网络硬盘(NAS, ISCSI ...) */
	NTPPARA		ntpClientParam;					/* NTP参数 */
	DDNSPARA	ddnsClientParam;				/* DDNS参数 */
	EMAILPARA	emailParam;						/* EMAIL参数 */
	UINT8		res[256];						/* !!! SNMP是否需要参数 */
} NETWORK;

/*
	PPP数据结构
*/
typedef struct
{		/* 160 bytes */
	U_IN_ADDR	remoteIpAddr;					/* 远程IP地址 */
	U_IN_ADDR	localIpAddr;					/* 本机IP地址 */
	UINT32 		localIpMask;					/* 本机子网掩码 */
	UINT8		username[NAME_LEN];				/* 用户名 */
	UINT8		password[PASSWD_LEN];			/* 密码 */
	UINT16		mru;							/* 最大接收长度 */
	UINT16		pppAttribute;					/* PPP模式，回拨，回拨模式，加密 */
	UINT8		phoneNumber[PHONENUMBER_LEN];	/* 电话号码/回拨号码 */
	UINT8		res[24];
} PPPPARA;

/*
	PPPoE数据结构
*/
typedef struct
{		/* 80 bytes */
	UINT8		username[NAME_LEN];				/* 用户名 */
	UINT8		password[PASSWD_LEN];			/* 密码 */
	UINT8		enablePPPoE;					/* 启用PPPoE */
	UINT8		res[31];
} PPPOEPARA;

/*
	报警输入数据结构
*/
typedef struct
{		/* 632 bytes */
	UINT8		alarmInName[NAME_LEN];			/* 名称 */
	UINT8		sensorType;						/* 报警器类型：0为常开；1为常闭 */
	UINT8		bEnableAlarmIn;					/* 处理报警输入 */
	UINT8		res1[6];
	UINT64		alarmInTriggerRecChans;			/* 报警输入触发的录象通道 */
	TIMESEGMENT armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 布防时间段 */
	EXCEPTION	alarmInAlarmHandleType;			/* 报警输入处理 */
	UINT8		enablePreset[MAX_CHANNUM];		/* 是否调用预置点 */
	UINT8		presetNo[MAX_CHANNUM];			/* 调用的云台预置点序号 */
	UINT8		enablePresetRevert[MAX_CHANNUM];/* 是否恢复到调用预置点前的位置 */
	UINT16		presetRevertDelay[MAX_CHANNUM];	/* 恢复预置点延时 */
	UINT8      	enablePtzCruise[MAX_CHANNUM];	/* 是否调用巡航 */
	UINT8		ptzCruise[MAX_CHANNUM];			/* 巡航 */
	UINT8      	enablePtzTrack[MAX_CHANNUM];	/* 是否调用轨迹 */
	UINT8		trackNo[MAX_CHANNUM];			/* 云台的轨迹序号(同预置点) */
	UINT8		res2[64];
} ALARMIN;

/*
	报警输出数据结构
*/
typedef struct
{		/* 272 bytes */
	UINT8		alarmOutName[NAME_LEN];			/* 名称 */
	TIMESEGMENT	alarmOutTimeSeg[MAX_DAYS][MAX_TIMESEGMENT];	/* 报警输出激活时间段 */
	UINT32		alarmOutDelay;					/* 报警输出延时 */
	UINT8		res[12];
} ALARMOUT;

/*
	用户数据结构
*/
typedef struct
{		/* 184 bytes */
	UINT8		username[NAME_LEN];				/* 用户名 */
	UINT8		password[PASSWD_LEN];			/* 密码 */
	UINT32		permission;						/* 权限 */
	U_IN_ADDR	ipAddr;							/* 用户IP地址(为0时表示允许任何地址) */
	UINT8		macAddr[MACADDR_LEN];			/* MAC address */
	UINT8		priority;						/* 优先级，0--低，1--中，2--高 */
	UINT8		res1[13];
	UINT64		localPrevPermission;			/* 预览权限 */
	UINT64		netPrevPermission;
	UINT64		localRecordPermission;			/* 录像权限 */
	UINT64		netRecordPermission;
	UINT64		localPlayPermission;			/* 回放权限 */
	UINT64		netPlayPermission;
	UINT64		localPtzCtrlPermission;			/* PTZ控制权限 */
	UINT64		netPtzCtrlPermission;
	UINT64		localBackupPermission;			/* 本地备份权限 */
	UINT8		res2[16];
} USER;

/* ATM卡号捕捉数据结构
*/
typedef struct
{
	UINT8 code[12];
}FRAMECODE;

typedef struct
{		/* 240 bytes */
	UINT32 		inputMode;						/* 输入方式 */
	U_IN_ADDR   atmip;							/* ATM机IP */
	UINT32 		atmtype;						/* ATM机类型 */
	UINT32 		frameSignBeginPos;				/* 报文标志位的起始位置 */
	UINT32 		frameSignLength;				/* 报文标志位的长度 */
	UINT8  		frameSignContent[12];			/* 报文标志位的内容 */
	UINT32 		cardLengthInfoBeginPos;			/* 卡号长度信息的起始位置 */
	UINT32 		cardLengthInfoLength;			/* 卡号长度信息的长度 */
	UINT32 		cardNumberInfoBeginPos;			/* 卡号的起始位置 */
	UINT32 		cardNumberInfoLength;			/* 卡号的长度 */
	UINT32 		businessTypeBeginPos;			/* 交易类型的起始位置 */
	UINT32 		businessTypeLength;				/* 交易类型的长度 */
	FRAMECODE	frameTypeCode[10];				/* 码 */
	UINT16		ATMPort;						/* 卡号捕捉端口号(网络协议方式) */
	UINT16		protocolType;					/* 网络协议类型 */
	UINT8		res[60];
}ATM_FRAMETYPE;

/*
	DM6467 ATM卡号捕捉数据结构
*/
#define MAX_ATM				4
#define MAX_ATM_NUM		1
#define MAX_ACTION_TYPE	12	
#if 0
typedef struct
{
	UINT8					code[12];
}FRAMECODE;
#endif
typedef struct	/*筛选设置*/
{	/*32 bytes*/
	UINT8					benable;					/*0,不启用;1,启用*/
	UINT8					mode;					/*0,ASCII;1,HEX*/	
	UINT8					textPos;					/*字符串位置，位置固定时使用*/
	UINT8					res1[1];
	UINT8 					filterText[16];			/*包含字符串*/
	UINT8					res[12];
}FILTER;

typedef struct	/*起始标识设置*/
{	/*40 bytes*/
	UINT8					startMode;				/*0,ASCII;1,HEX*/
	UINT8					endMode;				/*0,ASCII;1,HEX*/
	UINT8					res1[2];
	FRAMECODE				startCode;				/*起始字符*/
	FRAMECODE				endCode;				/*结束字符*/
	UINT8					res[12];
}IDENTIFICAT;

typedef struct	/*报文信息位置*/
{	/*32 bytes*/
	UINT32 					offsetMode;				/*0,token;1,fix*/
	UINT32  					offsetPos;				/*mode为1的时候使用*/
	FRAMECODE				tokenCode;				/*标志位*/
	UINT8					multiplierValue;			/*标志位多少次出现*/
	UINT8					externOffset;			/*附加的偏移量*/
	UINT8					codeMode;				/*0,ASCII;1,HEX*/
	UINT8 					res[9];					
}PACKAGE_LOCATION;

typedef struct	/*报文信息长度*/
{	/*48 bytes*/
	UINT32					lengthMode;				/*长度类型，0,variable;1,fix;2,get from package(设置卡号长度使用)*/
	UINT32  					fixLength;				/*mode为1的时候使用*/
	UINT32					maxLength;
	UINT32					minLength;
	UINT32					endMode;				/*终结符0,ASCII;1,HEX*/	
	FRAMECODE				endCode;				/*终结符*/
	UINT32					lengthPos;				/*lengthMode为2的时候使用，卡号长度在报文中的位置*/
	UINT32					lengthLen;				/*lengthMode为2的时候使用，卡号长度的长度*/
	UINT8					res[8];
}PACKAGE_LENGTH;

typedef struct	/*OSD 叠加的位置*/
{	/*20 bytes*/
	UINT32					positionMode;			/*叠加风格，共2种；0，不显示；1，Custom*/
	UINT32 					pos_x;					/*x坐标，positionmode为Custom时使用*/
	UINT32					pos_y;					/*y坐标，positionmode为Custom时使用*/
	UINT8					res[8];
}OSD_POSITION;

typedef struct	/*日期显示格式*/
{	/*60 bytes*/
	UINT8					item1;					/*Month,0.mm;1.mmm;2.mmmm*/							
	UINT8 					item2;					/*Day,0.dd;*/									
	UINT8 					item3;					/*Year,0.yy;1.yyyy*/	
	UINT8					dateform;				/*0~5，3个item的排列组合*/
	UINT8					res1[20];
	char						seprator[4];				/*分隔符*/
	char	    					displayseprator[4];		/*显示分隔符*/
	UINT32					displayform;				/*0~5，3个item的排列组合*/
	UINT8					res[24];
}DATE_FORMAT;
typedef struct	/*时间显示格式*/
{	/*60 bytes*/
	UINT32					timeform;				/*1. HH MM SS;0. HH MM*/
	char 					res1[20];
	UINT32					hourmode;				/*0,12;1,24*/
	char						seprator[4]; 				/*报文分隔符，暂时没用*/
	char						displayseprator[4];		/*显示分隔符*/
	UINT32	    				displayform;				/*0~5，3个item的排列组合*/
	UINT32					displayhourmode;			/*0,12;1,24*/
	UINT8					res[16];
}TIME_FORMAT;

typedef struct
{	/*24 bytes*/
	UINT64					channel;					/*叠加的通道*/
	UINT32					delayTime;				/*叠加延时时间*/
	UINT8					benableDelayTime;		/*是否启用叠加延时，在无退卡命令时启用*/
	UINT8					res[11];
}OVERLAY_CHANNAL;

typedef struct
{	/*84 bytes*/
	PACKAGE_LOCATION		location;
	OSD_POSITION			position;
	FRAMECODE				actionCode;				/*交易类型等对应的码*/
	FRAMECODE				preCode;				/*叠加字符前的字符*/
	UINT8					actionCodeMode;			/*交易类型等对应的码0,ASCII;1,HEX*/
	UINT8					res[7];
}ATM_PACKAGE_ACTION;

typedef struct
{	/*120 bytes*/
	PACKAGE_LOCATION		location;
	DATE_FORMAT			dateForm;
	OSD_POSITION			position;
	UINT8					res[8];
}ATM_PACKAGE_DATE;

typedef struct
{	/*120 bytes*/
	PACKAGE_LOCATION		location;
	TIME_FORMAT				timeForm;
	OSD_POSITION			position;
	UINT8					res[8];
}ATM_PACKAGE_TIME;

typedef struct
{	/*120 bytes*/
	PACKAGE_LOCATION		location;
	PACKAGE_LENGTH			length;
	OSD_POSITION			position;
	FRAMECODE				preCode;				/*叠加字符前的字符*/
	UINT8					res[8];
}ATM_PACKAGE_OTHERS;

typedef struct
{		/* 1804 bytes*/
	UINT8					benable;					/*是否启用0,不启用;1,启用*/
	UINT8					inputMode;				/*输入方式:网络监听、串口监听、网络协议、串口协议、串口服务器*/
	UINT8					res1[2];
	UINT8					atmName[32];			/*ATM 名称*/
	U_IN_ADDR				atmIp;					/*ATM 机IP  */
	UINT16					atmPort;					/* 卡号捕捉端口号(网络协议方式) 或串口服务器端口号*/
	UINT8					res2[2];
	UINT32					atmType;				/*ATM 机类型*/
	IDENTIFICAT				identification;				/*报文标志*/
	FILTER					filter;					/*过滤设置*/
	ATM_PACKAGE_OTHERS		cardNoPara;				/*叠加卡号设置*/
	ATM_PACKAGE_ACTION		tradeActionPara[MAX_ACTION_TYPE];	/*叠加交易行为设置*/
	ATM_PACKAGE_OTHERS		amountPara;				/*叠加交易金额设置*/
	ATM_PACKAGE_OTHERS		serialNoPara;			/*叠加交易序号设置*/
	OVERLAY_CHANNAL		overlayChan;				/*叠加通道设置*/
	ATM_PACKAGE_DATE		datePara;				/*叠加日期设置，暂时保留*/
	ATM_PACKAGE_TIME		timePara;				/*叠加时间设置，暂时保留*/
	UINT8					res[32];
}ATM_FRAMETYPE_NEW;


/*
	自动备份数据结构，可以按通道设置时间段
*/
typedef struct
{		/* 208 bytes */
	UINT8		enableAutoBackup;				/* 是否开启自动备份 */
	UINT8		backupDelay;					/* 备份延时 */
	UINT8		days;							/* 备份天数,1表示当天 */
	UINT8		res[5];
	UINT64  	channels;						/* 需要备份的通道,按位表示 */
	UINT8		startHour[MAX_CHANNUM];			/* 开始时间-小时 */
	UINT8		startMinute[MAX_CHANNUM];		/* 开始时间-分钟 */
	UINT8		startSecond[MAX_CHANNUM];		/* 开始时间-秒 */
	UINT8		endHour[MAX_CHANNUM];			/* 结束时间-小时 */
	UINT8		endMinute[MAX_CHANNUM];			/* 结束时间-分钟 */
	UINT8		endSecond[MAX_CHANNUM];			/* 结束时间-秒 */
}AUTOBACKUP;


/*
	保存IPcamera 对于 视频信号丢失,遮挡报警,移动侦测
	异常处理的数据结构
*/
typedef struct
{
	SIGNALLOSTPARA  viLostPara;					/* 视频信号丢失参数 */
	MASKALARMPARA   maskAlarmPara;				/* 遮挡报警参数 */
	MOTDETEXECPARA	motDetPara;					/* 移动侦测参数 */
}IPCPICPARA;

/*
	IPcamrea 参数结构
*/
#define HD_IPC 			31
#define HD_IPC_862F 	32
#define IP_DOME     	40
#define IP_DOME_HD     	41
#define IP_DOME_HD_B    42

typedef struct
{
	UINT32 bExist;
	UINT32 bValid;
	UINT32 ip; 			/* host byte ordered */
	UINT8 mac[MACADDR_LEN];
	UINT16 port; 	/* rtsp port */
	UINT16 cmdPort;		/* http port */
	UINT16 factory;
	UINT32 channel;
	UINT8 mode;			/*0-main stream; 1-sub stream*/
	UINT8 devType;
	UINT8 alarmInNums;
	UINT8 alarmOutNums;
	UINT8 usr[NAME_LEN];
	UINT8 pwd[PASSWD_LEN];
	VIDEOIN	ipcVideo;
	ALARMIN  	alarmInPara[MAX_IPC_ALARMIN];
	ALARMOUT	alarmOutPara[MAX_IPC_ALARMOUT];
	UINT8 ipcDomain[MAX_DOMAIN_NAME];   /*IPC 域名*/
	UINT8 res2[64];
}IPCAMERA_PARA;

/*
	夏令时数据结构
*/
typedef struct
{		/* 8 bytes */
	UINT8 		mon;
	UINT8 		weekNo;
	UINT8 		date;
	UINT8 		hour;
	UINT8 		min;
	UINT8		res[3];
}TIMEPOINT;

typedef struct
{		/* 40 bytes */
	UINT32		zoneIndex;  					/* 0:PST, 1:MST, 2:CST, 3:EST, 4:HST, 5:AST, 6:AKST */
	UINT8		res1[12];
	UINT32		DSTBias;						/* 时间调整值 */
	UINT8		enableDST;						/* 启用夏令时 */
	UINT8		res[3];
	TIMEPOINT	startpoint;						/* 夏令时起止时间 */
	TIMEPOINT   endpoint;
}ZONEANDDST;

/*
	定时开关机数据结构
*/
typedef struct
{		/* 96 bytes */
	TIMESEGMENT autoBootShut[MAX_DAYS];			/* 开机/关机时间 */
	UINT8		enableAutoBootShut;				/* 是否自动开关机 */
	UINT8		res[67];
}AUTOBOOT;

/*
	智能视频参数
*/
/* 截图格式 */
typedef enum _VCA_PICTURE_FORMAT_
{
  	VCA_PICTURE_JPEG	=  0,					/* JPEG格式 */
 	VCA_PICTURE_YUV420  =  1					/* YUV420 格式 */
}VCA_PICTURE_FORMAT;

/**** 车牌识别 ****/
/* 识别类型 */
#define  VCA_IMAGE_RECOGNIZE	0				/* 静态图像识别 */
#define  VCA_VIDEO_RECOGNIZE	1				/* 动态视频识别 */

/* 识别场景 */
#define  VCA_LOW_SPEED_SCENE	0				/* 低速通过场景（收费站、小区门口、停车场）*/
#define  VCA_HIGH_SPEED_SCENE	1				/* 高速通过场景（卡口、高速公路、移动稽查）*/

/* 识别结果标志 */
	/* 图像识别模式 */
#define  VCA_IMAGE_RECOGNIZE_FAILURE	0		/* 识别失败 */
#define  VCA_IMAGE_RECOGNIZE_SUCCESS	1		/* 识别成功 */
	/* 视频识别模式 */
#define  VCA_VIDEO_RECOGNIZE_FAILURE					0	/* 识别失败 */
#define  VCA_VIDEO_RECOGNIZE_SUCCESS_OF_BEST_LICENSE	1	/* 识别到比上次准确性更高的车牌 */
#define  VCA_VIDEO_RECOGNIZE_SUCCESS_OF_NEW_LICENSE		2	/* 识别到新的车牌 */

/* 车牌位置*/
typedef struct _VCA_PLATE_RECT_
{	/* 16 bytes */
	int		left;
	int		top;
	int		right;
	int		bottom;
} VCA_PLATE_RECT;

/* 车牌类型 */
typedef enum _VCA_PLATE_TYPE_
{
	VCA_STANDARD92_PLATE,						/* 标准民用车与军车 */
	VCA_STANDARD02_PLATE,						/* 02 式民用车牌  */
	VCA_WJPOLICE_PLATE,							/* 武警车 */
	VCA_JINGCHE_PLATE							/* 警车 */
}VCA_PLATE_TYPE;

/* 车牌颜色 */
typedef enum _VCA_PLATE_COLOR_
{
	VCA_BLUE_PLATE,								/* 蓝色车牌 */
	VCA_YELLOW_PLATE,							/* 黄色车牌 */
	VCA_WHITE_PLATE,							/* 白色车牌 */
	VCA_BLACK_PLATE								/* 黑色车牌 */
}VCA_PLATE_COLOR;

/* 车牌识别参数 */
typedef struct _VCA_PLATE_PARAM_
{	/* 320 bytes */
	UINT8		bEnable;						/* 开启车牌识别 */
	UINT8		bCapturePic;					/* 是否截图 */
	UINT8 		picFormat;						/* 截图的压缩格式,VCA_PICTURE_FORMAT */
	UINT8		res1;
	UINT16		jpegSize;						/* JPEG图片的大小 */
	UINT16		jpegQuality;					/* JPEG图片的质量 */
	UINT64		vcaTriggerRecChans;				/* 车牌识别触发的录象通道 */
	TIMESEGMENT	armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 车牌识别布防时间 */
	EXCEPTION	handleType;						/* 车牌识别处理 */
	UINT32		frameInterval;					/* 间隔 frmInterval 做一次识别,对图像识别模式有效,建议最小为 20 */
	VCA_PLATE_RECT	search_rect;				/* 搜索区域 */
	VCA_PLATE_RECT	invalidate_rect;			/* 无效区域，在搜索区域内部 */
	UINT32		minPlateWidth;					/* 最小车牌宽度，建议设为 80 */
	UINT8		recogniseScene;					/* 识别场景(低速和高速) */
	UINT8		recogniseType;					/* 识别类型(静态和动态) */
	UINT16		firstChar;						/* 识别应用对应的当地省份简称,如"浙"。*/
	UINT8		res2[28];
}VCA_PLATE_PARAM;

/* 车牌识别结果 */
typedef struct _VCA_PLATE_RESULT_
{
	VCA_PLATE_RECT	plate_rect;					/* 车牌位置 */
	UINT8	plate_class;						/* 车牌类型, VCA_PLATE_TYPE */
	UINT8	plate_color;						/* 车牌颜色, VCA_PLATE_COLOR */
	UINT	res[2];
	UINT8	license[16];						/* 车牌号码 */
	UINT8	believe[16];						/* 各个识别字符的置信度，如检测到车牌"浙 A12345",置信
                                 				   为 10,20,30,40,50,60,70，则表示"浙"字正确的可能性
                                 				   只有 10%，"A"字的正确的可能性是 20% */
}VCA_PLATE_RESULT;
/*********/

/**** 行为分析 ****/
#define  VCA_MAX_ALARM_LINE_NUM   	5			/* 最多支持五条警戒线 */
#define  VCA_MAX_ALARM_AREA_NUM  	5			/* 最多支持五个警戒区域 */

/* 坐标体 */
typedef struct _VCA_POINT_
{	/* 8 bytes */
 	UINT32		x;								/* X轴坐标 */
 	UINT32		y;								/* Y轴坐标 */
}VCA_POINT;

/* 报警目标类型 */
typedef enum _VCA_TARGET_CLASS_
{
  	VCA_TARGET_NOTHING	= 0,					/* 无效目标 */
  	VCA_TARGET_HUMAN	= 1,					/* 人 */
  	VCA_TARGET_VEHICLE	= 2,					/* 车 */
  	VCA_TARGET_ANYTHING	= 3						/* 人或车 */
}VCA_TARGET_CLASS;

/* 警戒线报警方向;注意：如果警戒线为水平线，则以"自上而下"视为"由左向右"  */
typedef enum _VCA_CROSS_DIRECTION_
{
  	VCA_LEFT_GO_RIGHT	= 0,					/* 由左至右 */
  	VCA_RIGHT_GO_LEFT	= 1,					/* 由右至左 */
 	VCA_BOTH_DIRECTION  = 2						/* 双向 */
}VCA_CROSS_DIRECTION;

/* 警戒线 */
typedef struct _VCA_ALARM_LINE_
{	/* 24 bytes */
  	UINT8		bActive;						/* 此警戒线是否有效 */
   	UINT8		alarm_target;					/* 报警目标类型, VCA_TARGET_CLASS */
  	UINT8		alarm_direction;				/* 警戒线报警方向, VCA_CROSS_DIRECTION */
	UINT8		res[5];
  	VCA_POINT	start;							/* 警戒线起点 */
  	VCA_POINT	end;							/* 警戒线终点 */
}VCA_ALARM_LINE;

/* 警戒区报警类型 */
typedef enum _VCA_AREA_ALARM_CLASS_
{
 	VCA_ENTER_AREA	= 0,						/* 目标进入区域 */
  	VCA_EXIT_AREA	= 1,						/* 目标离开区域 */
  	VCA_INSIDE_AREA = 2							/* 区域内有目标 */
}VCA_AREA_ALARM_CLASS;

/* 警戒区 */
typedef struct _VCA_ALARM_AREA_
{	/* 88 bytes */
	UINT8		bActive;						/* 此警戒区域是否有效 */
	UINT8		alarm_target;					/* 报警目标类型, VCA_TARGET_CLASS */
	UINT8  		alarm_class;					/* 警戒区报警类型, VCA_AREA_ALARM_CLASS */
	UINT8		res;
	UINT32		pointNum;						/* 顶点个数，10 个以内 */
	VCA_POINT	pos[10];						/* 顶点数组 */
}VCA_ALARM_AREA;

/* 用户设定报警规则 */
typedef struct _VCA_RULE_PARAM_
{	/* 832 bytes */
	UINT8			bEnable;					/* 开启行为分析 */
	UINT8			bCapturePic;				/* 是否截图 */
	UINT8 			picFormat;					/* 截图的压缩格式,VCA_PICTURE_FORMAT */
	UINT8			res1;
	UINT16			jpegSize;					/* JPEG图片的大小 */
	UINT16			jpegQuality;				/* JPEG图片的质量 */
	UINT64			vcaTriggerRecChans;			/* 行为分析触发的录象通道 */
	TIMESEGMENT		armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 行为分析布防时间 */
	EXCEPTION		handleType;					/* 行为分析处理 */
	UINT16 			human_width; 				/* 人的大致宽度 */
	UINT16 			human_height; 				/* 人的大致高度 */
	UINT16			vehicle_width; 				/* 车的大致宽度 */
	UINT16			vehicle_height;				/* 车的大致高度 */
 	VCA_ALARM_AREA	alarm_area[VCA_MAX_ALARM_AREA_NUM];	/* 警戒线规则 */
 	VCA_ALARM_LINE	alarm_line[VCA_MAX_ALARM_LINE_NUM];	/* 警戒区域规则 */
	UINT8			res2[16];
}VCA_RULE_PARAM;

/* 行为分析结果 */
typedef struct _VCA_BEHAVIOR_ANALYSE_RESULT_
{
	struct{
		UINT8		bAlert;						/* 1-报警 */
		UINT8		alarm_target;				/* 报警目标类型, VCA_TARGET_CLASS */
		UINT8		alarm_direction;			/* 警戒线报警方向, VCA_CROSS_DIRECTION */
		UINT8		res;
		COORDINATE 	start;
		COORDINATE 	end;
	}line_alert[VCA_MAX_ALARM_LINE_NUM];
	struct{
		UINT8		bAlert;						/* 1-报警 */
		UINT8		alarm_target;				/* 报警目标类型, VCA_TARGET_CLASS */
		UINT8		alarm_class;				/* 警戒区报警类型, VCA_AREA_ALARM_CLASS */
		UINT8		pointNum;					/* 定点个数 */
		COORDINATE	pos[10];					/* 顶点数组 */
	}area_alert[VCA_MAX_ALARM_AREA_NUM];
} VCA_BEHAVIOR_ANALYSE_RESULT;
/********/

/**** 人脸检测 ****/
#define VCA_MAX_FACE_CNT         10              /*每次检测最多的人脸个数*/
typedef struct
{	/* 8 bytes */
	short x,y,w,h;
}VCA_FACE_AREA;

/* 人脸检测参数 */
typedef struct _VCA_FD_PARAM_
{	/*  272 bytes */
	UINT8		bEnable;						/* 开启人脸识别 */
	UINT8		bCapturePic;					/* 是否截图 */
	UINT8 		picFormat;						/* 截图的压缩格式,VCA_PICTURE_FORMAT */
	UINT8		res;
	UINT16		jpegSize;						/* JPEG图片的大小 */
	UINT16		jpegQuality;					/* JPEG图片的质量 */
	UINT64		vcaTriggerRecChans;				/* 人脸检测触发的录象通道 */
	TIMESEGMENT	armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 人脸检测布防时间 */
	EXCEPTION	handleType;						/* 人脸检测处理 */
	UINT32		frameInterval;					/* 帧间隔 */
	UINT8		bLocateEyePos;					/* 是否定位人眼 */
	UINT8		bClusterProcess;				/* 是否进行聚类分析(过滤重复人脸) */
	UINT8		res1[18];
}VCA_FD_PARAM;
/********/

/*
   时区信息
*/
typedef struct
{
	SINT16		zoneHour;  						/*时区小时信息*/
	SINT16      	zoneMin;						/*时区分钟信息*/
}TIMEZONE;


/*
   设备时间
*/
typedef struct
{
	TIMEZONE    timeZone;
	UINT8       dateType;
	UINT8       zoneno;	
	UINT8       res[2];
}TIMEPARA;

/*
	网络IPC配置结构
*/
#define MAX_IP_DEVICE		32
#define MAX_IP_CHANNUM      32
#define MAX_IP_ALARMIN		128
#define MAX_IP_ALARMOUT		64
typedef struct{
	UINT32 enable;
	char username[NAME_LEN];
	char password[PASSWD_LEN];
	U_IN_ADDR ip;
	UINT16 cmdPort;
	UINT8 devType;
	UINT8 factory;
	UINT8 res[32];
}NET_IP_DEV_PARA;

typedef struct{
	UINT8 enable;
	UINT8 ipID;
	UINT8 channel;
	UINT8 factory;
	UINT8 res[32];
}NET_IP_CHAN_INFO;

typedef struct{
	UINT8 analogChanEnable[MAX_ANALOG_CHANNUM/8];
	NET_IP_CHAN_INFO  ipChanInfo[MAX_IP_CHANNUM];
}NET_IP_CHAN_PARA;

typedef struct{
	UINT8 ipID;
	UINT8 alarmIn;
	UINT8 res[18];
}NET_IP_ALARMIN_PARA;

typedef struct{
	UINT8 ipID;
	UINT8 alarmOut;
	UINT8 res[18];
}NET_IP_ALARMOUT_PARA;

typedef struct{
	NET_IP_DEV_PARA  ipDevPara[MAX_IP_DEVICE];
	NET_IP_CHAN_PARA ipChanPara;
	NET_IP_ALARMIN_PARA ipAlarmInPara[MAX_IP_ALARMIN];
	NET_IP_ALARMOUT_PARA ipAlarmOutPara[MAX_IP_ALARMOUT];
}NET_IP_PARA;

typedef struct{
	UINT16 scale;             /* 比例 */
	UINT16 offset;            /* 偏移量*/
}MOUSEPARA;

typedef struct{
	UINT8 bUseWizard;
	UINT8 res[15];
}GUIPARA;

#ifdef PIC_PART_MANAGE
/**@struct PICPARA	  
 * @brief  图片参数
 */
typedef struct
{
    UINT16 picResolution;   /* 抓图分辨率 设备端定义:2=D1,3=CIF,4=QCIF, sdk端定义:0=CIF, 1=QCIF, 2=D1 */
    UINT16 picQuality;      /* 抓图质量   设备端定义:80,60,40, sdk端定义:0-最好，1-较好，2-一般 */ 

}PICPARA;
#endif
/*
	设备参数结构
*/
typedef struct
{
	//UINT32		magicNumber;					/* 幻数 */
	//UINT32		paraChecksum;					/* 校验和 */
	//UINT32		paraLength;						/* 结构长度* 校验和、长度从'paraVersion'开始计算 */
	//UINT32		paraVersion;					/* 参数文件的版本号 */

	//UINT16		deviceType;						/* 设备型号：用于导入设备参数时检查是否兼容 */
	//UINT8		res1[6];							/* 保留6字节，可用于扩展参数的兼容性检查项 */

	//UINT32		language;						/* 设备语言 */
	//UINT8		voutAndTip;						/* bit0:(是否显示事件提示)*/
												/*    bit1:(是否从VGA 启动)
												    bit2:(是否显示main/aux/spot信息)*/
	//UINT8		res2[3];

	/* VGA、CVBS输出、串口参数放在最前面，供内核启动时初始化这些外设 */
	//VGAPARA		vgaPara[MAX_VGA];				/* VGA参数 */
	//VIDEOOUT	videoOut[MAX_VIDEOOUT];			/* 输出视频参数 */
	//RS232PARA	rs232Ppara[MAX_SERIAL_PORT];	/* RS232口参数 */

	//UINT8		deviceName[NAME_LEN];			/* 设备名称 */
	//UINT32		deviceId;						/* 设备号, 用于遥控器 */
	//UINT32		bEnablePassword;				/* 是否启动本地密码登陆 */

	//HOLIDAYPARA	holidayPara;						/* 工作日/节假日参数 */
	//ZONEANDDST	zoneDst;						/* 夏令时参数 */
	//TIMEPARA    devTime;							/* 设备时间相关信息*/
	//MOUSEPARA   mousePara;                      			/* 鼠标参数*/
	//VIDEOIN		videoIn[MAX_CHANNUM];			/* 输入视频参数 */
	//MATRIXPARA 	matrixPara[MAX_AUXOUT];			/* MATRIX输出预览参数 */
	//PREVIEW		previewPara[MAX_VIDEOOUT];		/* 预览参数 */
	//MONITORALARM monitorAlarm;					/* 监视器告警参数*/

	//HDPARA		hdiskPara;						/* 硬盘相关参数 */
	NETWORK	networkPara;					/* 网络参数 */
	//PPPPARA		pppPara;						/* PPP参数 */
	//PPPOEPARA	pppoePara;						/* PPPoE参数 */

	//ALARMIN		alarmInPara[MAX_ALARMIN];		/* 报警输入参数 */
	//ALARMOUT	alarmOutPara[MAX_ALARMOUT];	/* 报警输出参数 */
	//EXCEPTION	exceptionHandleType[MAX_EXCEPTIONNUM];	/* 异常处理方式 */

	//USER		user[MAX_USERNUM];				/* 用户参数 */
	//ATM_FRAMETYPE	frameType;					/* ATM卡号捕捉参数 */
	//AUTOBOOT	autoBootShutPara;				/* 定时开关机参数 */
	//AUTOBACKUP	autoBackupPara;					/* 自动备份参数 */
	//IPCAMERA_PARA ipCamera[MAX_IPCHANNUM];	/* IPcamera 设备参数 */
#if 0
	NET_IP_PARA netIpPara;                     /* 网络IP设备参数配置*/
	GUIPARA   guiPara;
#ifdef EVDO
      UINT32 enableEvdo;
      UINT32 enableSms;
      UINT32 EvdoOffLineTime;
      UINT8   cellNumber[EVDO_MAXCELLNUM][EVDO_CELLNUMLEN];
#endif
	/* 智能 */
#ifdef EHOME
	struct in_addr ehServerIpaddr;
	UINT16 ehServerPort;
	UINT16 ehAlarmEnable;
	UINT8 ehRes[16];		
	ZONE motionArea[MAX_CHANNUM][MASK_MAX_REGION]; 	/*移动侦测四个区域组合，增加后需加补丁*/
	UINT8 eHFrontId[32];
#endif
#if 1
	ATM_FRAMETYPE_NEW frameTypeNew[MAX_ATM]; /*新ATM卡号捕捉参数*/
#endif
#if defined(DM6467) || defined(HI3515)
	UINT32 colorSharpness[MAX_CHANNUM][MAX_TIMESEGMENT];
#endif

#ifdef PIC_PART_MANAGE
    PICPARA picPara[MAX_CHANNUM];
#endif
#ifdef DZ_BEITONG_VER
    int delayTime;
#endif
#endif
} DEVICECONFIG;

/* extern globals */
//extern DEVICECONFIG *pDevCfgParam;
//extern DEV_HARD_INFO devHardInfo;
//extern BOOT_PARMS bootParms;
extern pthread_mutex_t globalMSem;

#ifdef __cplusplus
}
#endif


#endif

