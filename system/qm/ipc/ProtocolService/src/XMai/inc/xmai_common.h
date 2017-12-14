#ifndef _XMAI_COMMON_H_
#define _XMAI_COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "QMAPI.h"
#include "QMAPIDataType.h"
#include "cJSON.h"

//提示信息可根据需要是否打印
#define XMAI_DEBUG 1
#if XMAI_DEBUG
#define XMAINFO(fmt, args...)  printf("[XMAINFO:%s-%s(%d)] "fmt,__FILE__,__FUNCTION__, __LINE__,##args)
#else
#define XMAINFO(fmt, args...) {}
#endif
//使用XMAIERR打印错误信息时，无须加'\n'
#define XMAIERR(fmt, args...)   printf("[XMAIERR:%s-%s(%d)] " fmt "\n", __FILE__,__FUNCTION__,__LINE__, ##args)


//基本宏定义
#define XMAI_MSG_SEND_BUF_SIZE (16*1024)
#define XMAI_STREAM_DATA_SEND_BUF_SIZE (1*1024*1024)
#define XMAI_PACKET_SEND_LEN_ONCE (4*1460-20)	//(8*1024)
#define XMAI_MAX_LINK_NUM  10
#define XMAI_TCP_LISTEN_PORT 8005
#define XMAI_BROADCAST_RCV_PORT 34569
#define XMAI_BROADCAST_SEND_PORT 34569
#define XMAI_SSL_PORT  8443
//#define XMAI_HTTP_PORT  80
#define XMAI_UDP_PORT  34568
#define XMAI_MSG_RECV_BUF_SIZE (16*1024)
#define ETH_DEV "eth0"
//每个音频帧G711A裸流数据的实际大小
#define XMAI_AUDIO_SIZE_PRE_FRAME (320) 

typedef enum{								//REQ为请求，RSP为相应的响应
    LOGIN_REQ = 1000, 						//登录请求
    LOGIN_RSP = 1001, 		
    KEEPALIVE_REQ = 1006,					//保活请求
    KEEPALIVE_RSP = 1007,					
    SYSINFO_REQ	= 1020,						//获取系统信息请求
    SYSINFO_RSP	= 1021,
    CONFIG_SET = 1040,						//设置配置请求
    CONFIG_SET_RSP = 1041,
    CONFIG_GET	= 1042,						//获取设置请求
    CONFIG_GET_RSP	= 1043,
    CONFIG_CHANNELTILE_SET = 1046,			//设置通道名称请求
    CONFIG_CHANNELTILE_SET_RSP = 1047,
    CONFIG_CHANNELTILE_GET = 1048,			//获取通道名称请求
    CONFIG_CHANNELTILE_GET_RSP = 1049,
    CONFIG_CHANNELTILE_DOT_SET = 1050,		//设置通道名称点阵请求
    CONFIG_CHANNELTILE_DOT_SET_RSP = 1051,
    ABILITY_GET	= 1360,						//能力级获取请求
    ABILITY_GET_RSP = 1361,
    PTZ_REQ = 1400,							//云台控制请求
    PTZ_RSP = 1401,
    MONITOR_REQ = 1410,						//实时监测请求
    MONITOR_RSP = 1411,
    MONITOR_DATA = 1412,					//实时监测数据
    MONITOR_CLAIM = 1413,					//监视认领请求
    MONITOR_CLAIM_RSP = 1414,				
    TALK_REQ = 1430,						//对讲请求
    TALK_RSP = 1431,
    TALK_CU_PU_DATA = 1432,					//CU到PU的对讲数据
    TALK_PU_CU_DATA = 1433,					//PU到CU的对讲数据
    TALK_CLAIM = 1434,						//对讲认领请求
    TALK_CLAIM_RSP = 1435,
    SYSMANAGER_REQ = 1450,					//系统管理请求
    SYSMANAGER_RSP = 1451,
    TIMEQUERY_REQ = 1452,					//获取PU系统时间
    TIMEQUERY_RSP = 1453,
    FULLAUTHORITYLIST_GET = 1470,			//获取所有权限的列表	
    FULLAUTHORITYLIST_GET_RSP = 1471,
    USERS_GET = 1472,						//获取所有用户的权限列表
    USERS_GET_RSP = 1473,
    GROUPS_GET = 1474,						//获取所有组的权限列表
    GROUPS_GET_RSP = 1475,
    GUARD_REQ = 1500,						//布警请求
    GUARD_RSP = 1501,
    UNGUARD_REQ	= 1502,						//撤警请求
    UNGUARD_RSP	= 1503,
    ALARM_REQ = 1504,						//告警请求，唯一一个PU主动上报给CU的消息
    IPSEARCH_REQ = 1530,					//IP自动搜索请求
    IPSEARCH_RSP = 1531,
    IP_SET_REQ = 1532,						//IP设置请求
    IP_SET_RSP = 1533,
    SET_IFRAME_REQ = 1562,					//设置框架请求
    SET_IFRAME_RSP = 1563,
    SYNC_TIME_REQ = 1590,					//网络登录时间同步请求
    SYNC_TIME_RSP = 1591,
}XMAI_MESSAGE_ID;

/// 捕获压缩格式类型
typedef enum {
	SDK_CAPTURE_COMP_DIVX_MPEG4,	///< DIVX MPEG4。
	SDK_CAPTURE_COMP_MS_MPEG4,		///< MS MPEG4。
	SDK_CAPTURE_COMP_MPEG2,			///< MPEG2。
	SDK_CAPTURE_COMP_MPEG1,			///< MPEG1。
	SDK_CAPTURE_COMP_H263,			///< H.263
	SDK_CAPTURE_COMP_MJPG,			///< MJPG
	SDK_CAPTURE_COMP_FCC_MPEG4,		///< FCC MPEG4
	SDK_CAPTURE_COMP_H264,			///< H.264
	SDK_CAPTURE_COMP_H265,			///< H.265
	SDK_CAPTURE_COMP_NR				///< 枚举的压缩标准数目。
}SDK_CAPTURE_COMP_t;

typedef enum {
	SDK_CAPTURE_SIZE_D1,		///< 720*576(PAL)	720*480(NTSC)
	SDK_CAPTURE_SIZE_HD1,		///< 352*576(PAL)	352*480(NTSC)
	SDK_CAPTURE_SIZE_BCIF,		///< 720*288(PAL)	720*240(NTSC)
	SDK_CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	SDK_CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	SDK_CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	SDK_CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	SDK_CAPTURE_SIZE_SVCD,		///< 1024*768
	SDK_CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	SDK_CAPTURE_SIZE_ND1 = 9,     ///< 240*192
	SDK_CAPTURE_SIZE_960H,      ///< 960*576 (960H)
	SDK_CAPTURE_SIZE_720P,        ///< 1280*720
	SDK_CAPTURE_SIZE_960P,        ///< 1280*960  (960P)
	SDK_CAPTURE_SIZE_UXGA ,       ///< 1600*1200
	SDK_CAPTURE_SIZE_1080P,       ///< 1920*1080
	SDK_CAPTURE_SIZE_WUXGA,       ///< 1920*1200
	SDK_CAPTURE_SIZE_2_5M,        ///< 1872*1408
	SDK_CAPTURE_SIZE_3M,          ///< 2048*1536
	SDK_CAPTURE_SIZE_5M,          ///< 3744*1408
	SDK_CAPTURE_SIZE_1080N = 19,
	SDK_CAPTURE_SIZE_4M,
	SDK_CAPTURE_SIZE_6M,
	SDK_CAPTURE_SIZE_8M,
	SDK_CAPTURE_SIZE_12M,
	SDK_CAPTURE_SIZE_4K,
	SDK_CAPTURE_SIZE_720N,
}SDK_CAPTURE_SIZE_t;

///////////////////////////
typedef struct tagVideoIFrameHeader{
    char flag1;
    char flag2;
    char flag3;
    char flag4;
    char flagT;
    char framebit;
    char width;
    char height;
    int  time;
    int  len;
} VideoIFrameHeader;

typedef struct tagVideoPFrameHeader{
    char flag1;
    char flag2;
    char flag3;
    char flag4;
    int  len;
}VideoPFrameHeader;

typedef struct tagAudioDataHeader{
    char flag1;
    char flag2;
    char flag3;
    char flag4;
    char flagT;
    char sampleRate;
    short  len;
}AudioDataHeader;

typedef struct tagXMaiMsgHeader{
    char headFlag;
    char version;
    char reserved1;
    char reserved2;
    int  sessionId;
    int  sequenceNum;
    char totalPacket;
    char curPacket;
    short messageId;
    int dataLen;
} XMaiMsgHeader;

typedef struct tagXMaiMediaStreamMsgHeader{
    char headFlag;
    char version;
    char reserved1;
    char reserved2;
    int  sessionId;
    int  sequenceNum;
    char channel;
    char endflag;
    short messageId;
    int dataLen;
} XMaiMediaStreamMsgHeader;

typedef enum{
    UNINIT_SESSION = 0,
    CMD_SESSION,
    CONNECT_SESSION,
} SESSION_TYPE;

typedef struct tagXMaiSessionCtrl{
	//int flag;
	int fSessionInt;
	char fSessionId[16];
	int accept_sock;
	//XMaiLvSocket lv_sock;
	//int lv_sub_stream_sock;
	int fPeerSeqNum;
	int fSequenceNum;
	char fSendbuf[XMAI_MSG_SEND_BUF_SIZE];
	SESSION_TYPE fSessionType;
} XMaiSessionCtrl;	

typedef enum{
    XMAI_MAIN_STREAM = 0,
    XMAI_SUB_STREAM,
    XMAI_UNKNOWN,
} XMAI_STREAM_TYPE;

typedef struct tagXMaiLvSocketInfo{
	int n_socket; 					
	int n_flag;	// 0关闭，1打开
} XMaiLvSocketInfo;

typedef struct tagXMaiLvSocket{
	XMaiLvSocketInfo main_stream_socket;
	XMaiLvSocketInfo sub_stream_socket;
} XMaiLvSocket;

typedef struct tagXMaiTalkSocket{
	int n_socket; 					
} XMaiTalkSocket;

typedef struct tagSessionGlobal{
	int flag;
	int fSessionInt;
	int main_sock;
	XMaiLvSocket lv_sock;	
	XMaiTalkSocket talk_sock;	
} XMaiSessionGlobal;

typedef struct tagXMaiTimetick
{
	DWORD second            :6;  // 秒      1-60
	DWORD minute            :6;  // 分      1-60
	DWORD hour              :5;  // 时      1-24
	DWORD day               :5;  // 日      1-31
	DWORD month             :4;  // 月      1-12
	DWORD year              :6;  // 年      2000为基准，0-63
} XMaiTimetick;


typedef	void *(*ThreadEntryPtrType)(void *);

#endif
