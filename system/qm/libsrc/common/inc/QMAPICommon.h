#ifndef _QMAPI_COMMON_H_
#define _QMAPI_COMMON_H_
#include "QMAPIType.h"
/************************************************************************/
/* 
本头文件定义设备层的宏定义以及结构体。
*/
/************************************************************************/

#define QMAPI_MAX_OSD_LEN				128
#define QMAPI_MAX_VIDEO_HIDE_RECT		5
#define QMAPI_MAX_OSD_NUM				5  //1路视频编码最大支持视频叠加个数

#define QMAPI_NAME_LEN					32
#define QMAPI_SERIALNO_LEN				48
#define QMAPI_MACADDR_LEN				6
#define QMAPI_MAX_ETHERNET				2
#define QMAPI_PATHNAME_LEN				128
#define QMAPI_PASSWD_LEN				32

#define QMAPI_MAX_CHANNUM				1
#define QMAPI_MAX_STREAMNUM			3
#define QMAPI_MAX_ALARMIN				16
#define QMAPI_MAX_ALARMOUT				8

#define QMAPI_MAX_DAYS				8
#define QMAPI_MAX_TIMESEGMENT			4

#define QMAPI_MAX_PRESET					255
#define QMAPI_MAX_CRUISE_GROUP_NUM	5
#define QMAPI_MAX_CRUISE_POINT_NUM		32
#define QMAPI_PHONENUMBER_LEN			32
#define QMAPI_MAX_DISKNUM				16
#define QMAPI_MAX_WINDOW				16
#define QMAPI_MAX_PLAYER_WINDOW		16
#define QMAPI_MAX_VGA					1
#define QMAPI_MAX_USERNUM				32
#define QMAPI_MAX_USERGROUPNUM		8

#define QMAPI_MAX_EXCEPTIONNUM			16
#define QMAPI_MAX_LINK					6
#define QMAPI_MAXCARD_NUM				40
#define QMAPI_MAX_SERIALNUM			3
#define QMAPI_CARDNUM_LEN				20
#define QMAPI_MAX_VIDEOOUT			2
#define QMAPI_MAX_DISPLAY_REGION		16
#define QMAPI_MAX_RIGHT				32
#define QMAPI_MAX_DOMAIN_NAME         64
#define QMAPI_MAX_PREVIEW_MODE        8
#define QMAPI_PTZ_PROTOCOL_NUM        200     /* 最大支持的云台协议数 */
#define QMAPI_MAX_HD_GROUP            16
#define QMAPI_MAX_DDNS_NUMS           3
#define QMAPI_MAX_IP_LENGTH           16
#define QMAPI_MAX_IPV6_LENGTH            128
#define QMAPI_MAX_PATH_LEN            256

/** 支持输入邮件地址最大个数 */
#define QMAPI_EMAIL_ADDR_MAXNUM (3)

/* 日志记录信息 */
#define QMAPI_LOG_INDEX_MAXNUM      1024 	//日志搜索最大条数
#define QMAPI_LOG_BLOCK_MAX         256     //这个不要定义超过一个char类型最大值，如果需要超过，需要修改DMS_LOG_INDEX里面的
#define QMAPI_LOG_BLOCK_SIZE        16
#define QMAPI_LOG_SINGLE_BLOCK_MAX  16      //单条日志最多日志条数

/* 录像文件检索数  2048*100 / sizeof(QMAPI_NET_FINDDATA)*/
#define QMAPI_FILES_SIZE 2048*100

#define QMAPI_MD_STRIDE_WIDTH     44
#define QMAPI_MD_STRIDE_HEIGHT    36
#define QMAPI_MD_STRIDE_SIZE        (QMAPI_MD_STRIDE_WIDTH*QMAPI_MD_STRIDE_HEIGHT/8)
#define QMAPI_MAX_VIDEO_PACKET_LEN              300*1024
#define QMAPI_MAX_AUDIO_PACKET_LEN              50*1024
#define QMAPI_SEND_BUFFER_SIZE				300
#define QMAPI_MAX_SYS_FD								65535

#define	QMAPI_ENCODE_FORMAT_MGPE4				0x0
#define	QMAPI_ENCODE_FORMAT_H264				0x4
#define	QMAPI_ENCODE_FORMAT_MJPEG				0x5
#define QMAPI_ENCODE_FORMAT_JPEG				0x6

#define	QMAPI_WAVE_FORMAT_MPEGLAYER3				0x0055
#define	QMAPI_WAVE_FORMAT_G722_ADPCM				0x0065
#define	QMAPI_WAVE_FORMAT_G711_ADPCM				0x003e
#define	QMAPI_WAVE_FORMAT_ADPCM					0x0002
#define QMAPI_WAVE_FORMAT_ARM                     0x0003

#define QMAPI_WITHOUT_TIME					0xFFFFFFFF

#define QMAPI_STREAM_CBR						0
#define QMAPI_STREAM_VBR						1

//#define   SUPPORT_FULLD1

#ifdef SUPPORT_FULLD1
#define QMAPI_PAL_D1_FORMAT_WIDTH						720
#define QMAPI_PAL_HD1_FORMAT_WIDTH					720
#else
#define QMAPI_PAL_D1_FORMAT_WIDTH						704
#define QMAPI_PAL_HD1_FORMAT_WIDTH					704
#endif

#define QMAPI_PAL_D1_FORMAT_HEIGHT					576

#define QMAPI_PAL_HD1_FORMAT_HEIGHT					288
#define QMAPI_PAL_CIF_FORMAT_WIDTH					352
#define QMAPI_PAL_CIF_FORMAT_HEIGHT					288
#define QMAPI_PAL_QCIF_FORMAT_WIDTH					176
#define QMAPI_PAL_QCIF_FORMAT_HEIGHT					144

#ifdef SUPPORT_FULLD1
#define QMAPI_NTSC_D1_FORMAT_WIDTH				720
#define QMAPI_NTSC_HD1_FORMAT_WIDTH				720
#else
#define QMAPI_NTSC_D1_FORMAT_WIDTH				704
#define QMAPI_NTSC_HD1_FORMAT_WIDTH				704
#endif

#define QMAPI_NTSC_D1_FORMAT_HEIGHT				480

#define QMAPI_NTSC_HD1_FORMAT_HEIGHT				240
#define QMAPI_NTSC_CIF_FORMAT_WIDTH				352
#define QMAPI_NTSC_CIF_FORMAT_HEIGHT				240
#define QMAPI_NTSC_QCIF_FORMAT_WIDTH				176
#define QMAPI_NTSC_QCIF_FORMAT_HEIGHT				120

#define QMAPI_QQVGA_FORMAT_WIDTH					160
#define QMAPI_QQVGA_FORMAT_HEIGHT					112

#define QMAPI_QVGA_FORMAT_WIDTH					320
#define QMAPI_QVGA_FORMAT_HEIGHT					240

#define QMAPI_VGA_FORMAT_WIDTH						640
#define QMAPI_VGA_FORMAT_HEIGHT					480

#define QMAPI_SVGA_FORMAT_WIDTH					800
#define QMAPI_SVGA_FORMAT_HEIGHT					600

#define QMAPI_UXGA_FORMAT_WIDTH					1600
#define QMAPI_UXGA_FORMAT_HEIGHT					1200


#define QMAPI_HH720P_FORMAT_WIDTH					1280
#define QMAPI_HH720P_FORMAT_HEIGHT				720

#define QMAPI_960P_FORMAT_WIDTH                   1280
#define QMAPI_960P_FORMAT_HEIGHT                  960

#define QMAPI_HH130H_FORMAT_WIDTH					1280
#define QMAPI_HH130H_FORMAT_HEIGHT				1024

#define QMAPI_HH300H_FORMAT_WIDTH					2048
#define QMAPI_HH300H_FORMAT_HEIGHT				1536

#define QMAPI_960H_FORMAT_WIDTH					960
#define QMAPI_960H_FORMAT_HEIGHT					576

//视频制式
#define QMAPI_HH1080P_FORMAT_WIDTH			       1920
#define QMAPI_HH1080P_FORMAT_HEIGHT				1080
#define QMAPI_NTSC								0
#define QMAPI_PAL									1
#define videoStandardPAL								1
#define videoStandardNTSC							0

//分辨率   (0为CIF,1为D1,2为HALF-D1,3为QCIF)
#define QMAPI_VIDEO_FORMAT_CIF                   0
#define QMAPI_VIDEO_FORMAT_D1                    1
#define QMAPI_VIDEO_FORMAT_HD1                   2
#define QMAPI_VIDEO_FORMAT_QCIF                  3
#define QMAPI_VIDEO_FORMAT_VGA				   4
#define QMAPI_VIDEO_FORMAT_QVGA                  5
#define QMAPI_VIDEO_FORMAT_720P                  6
#define QMAPI_VIDEO_FORMAT_130H                  7
#define QMAPI_VIDEO_FORMAT_300H                  8
#define QMAPI_VIDEO_FORMAT_QQVGA				   9
#define QMAPI_VIDEO_FORMAT_UXGA                  10
#define QMAPI_VIDEO_FORMAT_SVGA                  11
#define QMAPI_VIDEO_FORMAT_1080P				   12
#define QMAPI_VIDEO_FORMAT_XGA                   13 /*1024*768*/
#define QMAPI_VIDEO_FORMAT_960P                  14 /*1280*960*/
#define QMAPI_VIDEO_FORMAT_640x360               15
#define QMAPI_VIDEO_FORMAT_960H                  16 /*960*576 (928*576)*/
#define QMAPI_VIDEO_FORMAT_4M					   17
#define QMAPI_VIDEO_FORMAT_2560x1440			   18
#define QMAPI_VIDEO_FORMAT_5M					   19
#define	QMAPI_VIDEO_FORMAT_2048x1520			   20




//视频色彩设置标识
//Set Flag
#define		QMAPI_COLOR_SET_BRIGHTNESS			0x00000001 //亮度
#define		QMAPI_COLOR_SET_HUE					0x00000002 //色调
#define		QMAPI_COLOR_SET_SATURATION			0x00000004 //饱和度，颜色
#define		QMAPI_COLOR_SET_CONTRAST				0x00000008 //对比度
#define		QMAPI_COLOR_SET_IRISBASIC				0x00000010 //自动光圈基准值
#define		QMAPI_COLOR_SET_RED					0x00000020 //
#define		QMAPI_COLOR_SET_BLUE					0x00000040
#define		QMAPI_COLOR_SET_GREEN					0x00000080
#define		QMAPI_COLOR_SET_GAMMA					0x00000100
#define  	QMAPI_COLOR_SET_DEFINITION                     	0x00000200 //清晰度
#define   	QMAPI_COLOR_SET_SCENE                             	0x00000400 //场景环境
#define  	QMAPI_COLOR_SET_ADNSWITCH                     	0x00000800 //开启降噪
#define   	QMAPI_COLOR_SET_DN			                     0x00001000 //降噪阀值,开启降噪有效
#define    	QMAPI_COLOR_SET_AWBSWITCH                   	0x00002000 //自动白平衡切换
#define    	QMAPI_COLOR_SET_AECSWITCH                    	0x00004000 //自动曝光切换
#define    	QMAPI_COLOR_SET_EC                    			0x00008000 //曝光阀值
#define    	QMAPI_COLOR_SET_AGCSWITCH                    	0x00010000 //自动增益切换
#define    	QMAPI_COLOR_SET_GC                    			0x00020000 //增益阀值
#define    	QMAPI_COLOR_SET_MIRROR                   		0x00040000 //镜像
#define    	QMAPI_COLOR_SET_BAW                     		0x00080000 //彩色转黑白
#define    	QMAPI_COLOR_SET_EWD		       		0x00100000 //打开宽动态
#define    	QMAPI_COLOR_SET_WD					0x00200000 //宽动态档次
#define   	QMAPI_COLOR_SET_AIRIS                                0x00400000 //自动光圈
#define  	QMAPI_COLOR_SET_BLC                                    0x00800000 // 背光补偿
#define    	QMAPI_COLOR_SET_ROTATE							0x01000000 //旋转
#define    	QMAPI_COLOR_SET_ANTIFLICKER					0x02000000 //抗闪烁

//Get Flag
#define	QMAPI_COLOR_GET_BRIGHTNESS			0x00000001 //亮度
#define	QMAPI_COLOR_GET_HUE					0x00000002 //色调
#define	QMAPI_COLOR_GET_SATURATION			0x00000004 //饱和度，颜色
#define	QMAPI_COLOR_GET_CONTRAST				0x00000008 //对比度
#define	QMAPI_COLOR_GET_ACUTANCE				0x00000010 //锐度
#define	QMAPI_COLOR_GET_RED					0x00000020 //
#define	QMAPI_COLOR_GET_BLUE					0x00000040
#define	QMAPI_COLOR_GET_GREEN					0x00000080
#define	QMAPI_COLOR_GET_GAMMA					0x00000100
#define   QMAPI_COLOR_GET_DEFINITION                     	0x00000200 //清晰度
#define   QMAPI_COLOR_GET_SCENE                             	0x00000400 //场景环境
#define   QMAPI_COLOR_GET_ADNSWITCH                     	0x00000800 //自动降噪切换
#define   QMAPI_COLOR_GET_DN			                     0x00001000 //降噪阀值
#define    QMAPI_COLOR_GET_AWBSWITCH                   	0x00002000 //自动白平衡切换
#define    QMAPI_COLOR_GET_AECSWITCH                    	0x00004000 //自动曝光切换
#define    QMAPI_COLOR_GET_EC                    			0x00008000 //曝光阀值
#define    QMAPI_COLOR_GET_AGCSWITCH                    	0x00010000 //自动增益切换
#define    QMAPI_COLOR_GET_GC                    			0x00020000 //增益阀值
#define    QMAPI_COLOR_GET_MIRROR                   		0x00040000 //镜像
#define    QMAPI_COLOR_GET_BAW                     		0x00080000 //彩色转黑白
#define    QMAPI_COLOR_GET_EWD		       				0x00100000 //打开宽动态
#define    QMAPI_COLOR_GET_WD								0x00200000 //宽动态档次
#define   QMAPI_COLOR_GET_AIRIS                           0x00400000 //自动光圈
#define   QMAPI_COLOR_GET_BLC                             0x00800000 // 背光补偿
#define    QMAPI_COLOR_GET_ROTATE							0x01000000 //旋转


//智能分析
#define   QMAPI_MAX_EVT_CNT				32
#define   QMAPI_MAX_TGT_CNT				32
#define   QMAPI_BUFLEN_EVT_DATA		64
#define   QMAPI_URP_NAME_BUF_SIZE	32
#define 	QMAPI_IVMD_MAX_REGION_NUM 32
#define    QMAPI_MAX_POLYGON_NUM 32
#define  	QMAPI_ITEV_CHAN					32
#define  	QMAPI_URP_MAX_TRIPWIRE_CNT     5


//系统退出类型
#define QMAPI_SYS_EXITCODE_REBOOT            1
#define QMAPI_SYS_EXITCODE_SHUTDOWN          2

#define QMAPI_SENSOR_EVEROPEN               0
#define QMAPI_SENSOR_EVERCLOSE              1

typedef enum __QMAPI_STREAM_TYPE_E
{
	QMAPI_MAIN_STREAM = 0, // 主码流
	QMAPI_SECOND_STREAM, //子码流
	QMAPI_MOBILE_STREAM, // 手机码流
	QMAPI_MAX_STREAM_TYPE_NUM
}QMAPI_STREAM_TYPE_E;

typedef enum __DMS_SERVER_TYPE_E
{
	NVS_FLAG=100001,
	NVD_FLAG=100002,
	NVR_FLAG=100003,
	IPC_FLAG=100004,

	//IPC
	IPC_3420P	=0xA1010001,
	IPC_3520P	=0xA1010002,

	//DVR
	DVR_Q3I     =0xA2010001,
}QMAPI_SERVER_TYPE_E;

typedef enum __QMAPI_CPU_TYPE_E
{
	QMAPI_CPU_Q3F = 1006,
    QMAPI_CPU_3420P = 3420,
    QMAPI_CPU_3520P = 3520,
}QMAPI_CPU_TYPE_E;


typedef struct  _tagQMAPI_SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;    
	WORD wDayOfWeek;
	WORD wDay;    
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}QMAPI_SYSTEMTIME;

typedef struct tagQMAPI_SYSTEM_MEMINFO
{
	DWORD		dwTotal;
	DWORD		dwFree;
	DWORD		dwBuffers;
	DWORD		dwCached;
}QMAPI_SYSTEM_MEMINFO;

typedef struct tagQMAPI_FORMAT_STATUS
{
	DWORD dwSize;
	DWORD dwDiskID;
	DWORD dwPartitionIndex;
	DWORD dwFormatState;
	DWORD dwParam1;
	DWORD dwParam2;
}QMAPI_FORMAT_STATUS;

#endif //_QMAPI_COMMON_H_


