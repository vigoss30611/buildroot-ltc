#ifndef _QMAPI_NET_NETSDK_H_
#define _QMAPI_NET_NETSDK_H_
#include "QMAPICommon.h"
#include "QMAPIDataType.h"

//////////////////////////////////////////////////////////////////////////
//（1）网络配置结构体

// 语言种类
typedef enum __LANGUAGE_TYPE_E
{
    QMAPI_LANGUAGE_ENGLISH,                       // 简体中文
    QMAPI_LANGUAGE_CHINESE_SIMPLIFIED,            // 英文
    QMAPI_LANGUAGE_CHINESE_TRADITIONAL,           // 繁体中文
    QMAPI_LANGUAGE_ITALIAN,                       // 意大利文
    QMAPI_LANGUAGE_SPANISH,                       // 西班牙文
    QMAPI_LANGUAGE_JAPANESE,                      // 日文版
    QMAPI_LANGUAGE_RUSSIAN,                       // 俄文版
    QMAPI_LANGUAGE_FRENCH,                        // 法文版
    QMAPI_LANGUAGE_GERMAN,                        // 德文版
    QMAPI_LANGUAGE_PORTUGUESE,                    // 葡萄牙语
    QMAPI_LANGUAGE_TURKEY,                        // 土尔其语
    QMAPI_LANGUAGE_POLISH,                        // 波兰语
    QMAPI_LANGUAGE_ROMANIAN,                      // 罗马尼亚
    QMAPI_LANGUAGE_HUNGARIAN,                     // 匈牙利语
    QMAPI_LANGUAGE_FINNISH,                       // 芬兰语
    QMAPI_LANGUAGE_ESTONIAN,                      // 爱沙尼亚语
    QMAPI_LANGUAGE_KOREAN,                        // 韩语
    QMAPI_LANGUAGE_FARSI,                         // 波斯语
    QMAPI_LANGUAGE_DANSK,                         // 丹麦语
    QMAPI_LANGUAGE_CZECHISH,                      // 捷克文
    QMAPI_LANGUAGE_BULGARIA,                      // 保加利亚文
    QMAPI_LANGUAGE_SLOVAKIAN,                     // 斯洛伐克语
    QMAPI_LANGUAGE_SLOVENIA,                      // 斯洛文尼亚文
    QMAPI_LANGUAGE_CROATIAN,                      // 克罗地亚语
    QMAPI_LANGUAGE_DUTCH,                         // 荷兰语
    QMAPI_LANGUAGE_GREEK,                         // 希腊语
    QMAPI_LANGUAGE_UKRAINIAN,                     // 乌克兰语
    QMAPI_LANGUAGE_SWEDISH,                       // 瑞典语
    QMAPI_LANGUAGE_SERBIAN,                       // 塞尔维亚语
    QMAPI_LANGUAGE_VIETNAMESE,                    // 越南语
    QMAPI_LANGUAGE_LITHUANIAN,                    // 立陶宛语
    QMAPI_LANGUAGE_FILIPINO,                      // 菲律宾语
    QMAPI_LANGUAGE_ARABIC,                        // 阿拉伯语
    QMAPI_LANGUAGE_CATALAN,                       // 加泰罗尼亚语
    QMAPI_LANGUAGE_LATVIAN,                       // 拉脱维亚语
}QMAPI_LANGUAGE_TYPE_E;

typedef enum __QMAPI_DDNS_TYPE
{
    DDNS_3322 = 1,
    DDNS_DYNDNS,
    DDNS_NIGHTOWLDVR,
    DDNS_NOIP,
    DDNS_MYEYE,
    DDNS_PEANUTHULL,
    DDNS_ANSHIBAO,
    DDNS_BUTT,
}QMAPI_DDNS_TYPE;

typedef enum __QMAPI_PTZ_LINK_TYPE
{
    QMAPI_PTZ_LINK_GOTO_PRESET=1, //预置位
    QMAPI_PTZ_LINK_CRUISE,        //巡航
    QMAPI_PTZ_LINK_TRACK,         //轨迹
}QMAPI_PTZ_LINK_TYPE;


typedef enum
{
    QMAPI_PT_PCMU = 0,
    QMAPI_PT_1016 = 1,
    QMAPI_PT_G721 = 2,
    QMAPI_PT_GSM = 3,
    QMAPI_PT_G723 = 4,
    QMAPI_PT_DVI4_8K = 5,
    QMAPI_PT_DVI4_16K = 6,
    QMAPI_PT_LPC = 7,
    QMAPI_PT_PCMA = 8,
    QMAPI_PT_G722 = 9,
    QMAPI_PT_S16BE_STEREO,
    QMAPI_PT_S16BE_MONO = 11,
    QMAPI_PT_QCELP = 12,
    QMAPI_PT_CN = 13,
    QMAPI_PT_MPEGAUDIO = 14,
    QMAPI_PT_G728 = 15,
    QMAPI_PT_DVI4_3 = 16,
    QMAPI_PT_DVI4_4 = 17,
    QMAPI_PT_G729 = 18,
    QMAPI_PT_G711A = 19,
    QMAPI_PT_G711U = 20,
    QMAPI_PT_G726 = 21,
    QMAPI_PT_G729A = 22,
    QMAPI_PT_LPCM = 23,
    QMAPI_PT_CelB = 25,
    QMAPI_PT_JPEG = 26,
    QMAPI_PT_CUSM = 27,
    QMAPI_PT_NV = 28,
    QMAPI_PT_PICW = 29,
    QMAPI_PT_CPV = 30,
    QMAPI_PT_H261 = 31,
    QMAPI_PT_MPEGVIDEO = 32,
    QMAPI_PT_MPEG2TS = 33,
    QMAPI_PT_H263 = 34,
    QMAPI_PT_SPEG = 35,
    QMAPI_PT_MPEG2VIDEO = 36,
    QMAPI_PT_AAC = 37,
    QMAPI_PT_AMR = 0x37A,
	QMAPI_PT_WMA9STD = 38,
    QMAPI_PT_HEAAC = 39,
    QMAPI_PT_PCM_VOICE = 40,
    QMAPI_PT_PCM_AUDIO = 41,
    QMAPI_PT_AACLC = 42,
    QMAPI_PT_MP3 = 43,
    QMAPI_PT_ADPCMA = 49,
    QMAPI_PT_AEC = 50,
    QMAPI_PT_X_LD = 95,
    QMAPI_PT_H264 = 96,
	QMAPI_PT_H264_HIGHPROFILE = 0x96A,
	QMAPI_PT_H264_MAINPROFILE = 0x96B,
	QMAPI_PT_H264_BASELINE = 0x96C,
    QMAPI_PT_MJPEG = 0x96D,
    QMAPI_PT_D_GSM_HR = 200,
    QMAPI_PT_D_GSM_EFR = 201,
    QMAPI_PT_D_L8 = 202,
    QMAPI_PT_D_RED = 203,
    QMAPI_PT_D_VDVI = 204,
    QMAPI_PT_D_BT656 = 220,
    QMAPI_PT_D_H263_1998 = 221,
    QMAPI_PT_D_MP1S = 222,
    QMAPI_PT_D_MP2P = 223,
    QMAPI_PT_D_BMPEG = 224,
    QMAPI_PT_MP4VIDEO = 230,
    QMAPI_PT_MP4AUDIO = 237,
    QMAPI_PT_VC1 = 238,
    QMAPI_PT_JVC_ASF = 255,
    QMAPI_PT_D_AVI = 256,
    QMAPI_PT_MAX = 257,
    QMAPI_PT_H265 = 258,

}QMAPI_PAYLOAD_TYPE_E;

typedef enum 
{ 
    QMAPI_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    QMAPI_AUDIO_SAMPLE_RATE_BUTT,
}QMAPI_AUDIO_SAMPLE_RATE_E; 
/*
typedef enum
{
    QMAPI_NETPT_RES1 = 1, 
    QMAPI_NETPT_SHDX = 2,//上海电信
    QMAPI_NETPT_HXHT = 3,//互信互通
    QMAPI_NETPT_SHILIAN = 4, //拾联平台
    QMAPI_NETPT_RES5 = 5,//保留
    QMAPI_NETPT_LANSHI = 6,//成都朗视
    QMAPI_NETPT_RES7 = 7, //保留
    QMAPI_NETPT_DITE = 8, //迪特
    QMAPI_NETPT_RES9 = 9,//保留
    QMAPI_NETPT_RES10 = 10,//保留
    QMAPI_NETPT_SHIXUN = 11,//本公司的视讯平台
    QMAPI_NETPT_HIK = 12,//海康平台
    QMAPI_NETPT_HTZN = 13,//华拓智能
    QMAPI_NETPT_XTREAM = 14,//
    QMAPI_NETPT_JIUAN = 15, //广州九安
    QMAPI_NETPT_RES16 = 16, //保留
    QMAPI_NETPT_RES17 = 17,//保留
    QMAPI_NETPT_RES18 = 18,//保留
    QMAPI_NETPT_RES19 = 19,//保留
    QMAPI_NETPT_RES20 = 20,//保留
    ///////////////////////////////
    //以下是系统内部模块或者定制模块
    QMAPI_NETPT_ONVIF = 21, //ONVIF 网络库
    QMAPI_NETPT_CMS = 22,//JBNET 网络库
    QMAPI_NETPT_CGI = 23,//主机网络CGI
    QMAPI_NETPT_REC = 24,//录像模块
    QMAPI_NETPT_DDNS = 25,//DDNS 模块
    QMAPI_NETPT_LANGTAOMOBILE = 26, //浪涛手机库
    QMAPI_NETPT_TUTKP2P =27, //TUTK P2P 库
    QMAPI_NETPT_TLK = 28, //康力特
    QMAPI_NETPT_RTSP = 29, //RTSP 模块
    QMAPI_NETPT_BUTT,
}QMAPI_NETPT_TYPE_E;
*/
typedef enum 
{
    QMAPI_FRAME_I, /**<I帧*/
    QMAPI_FRAME_P, /**<P帧*/
    QMAPI_FRAME_B, /**<B帧*/
    QMAPI_FRAME_A, /**<音频帧*/
    QMAPI_FRAME_BUTT /**<保留值*/
}QMAPI_FRAME_TYPE_E;

typedef struct tagQMAPI_RECT
{
    int left;
    int top;
    int right;
    int bottom;
}QMAPI_RECT;

typedef struct tagQMAPI_TIME
{
  DWORD    dwYear;
  DWORD    dwMonth;
  DWORD    dwDay;
  DWORD    dwHour;
  DWORD    dwMinute;
  DWORD    dwSecond;
}QMAPI_TIME, *LPQMAPI_TIME;


typedef struct tagQMAPI_SCHEDTIME
{
	BOOL bEnable;
	BYTE res[3];
    //开始时间
    BYTE byStartHour;
    BYTE byStartMin;
    //结束时间
    BYTE byStopHour;    
    BYTE byStopMin;
}QMAPI_SCHEDTIME;

// 云台联动
typedef struct 
{
    BYTE  byType;//详见QMAPI_PTZ_LINK_TYPE，0:无效，1:预置点，2:点间巡航，3:启用轨迹
    BYTE  byValue;
    BYTE  Reserve[2];
} QMAPI_PTZ_LINK, *LPQMAPI_PTZ_LINK;

typedef struct tagQMAPI_RANGE
{
    int         nMin;
    int         nMax;
}QMAPI_RANGE, *LQMAPI_RANGE;

typedef struct tagQMAPI_SIZE
{
    int           nWidth;
    int           nHeight;
}QMAPI_SIZE, *LQMAPI_SIZE;


typedef enum
{
	QMAPI_NETPT_MAIN = 1,
    QMAPI_NETPT_ONVIF, 
	QMAPI_NETPT_REC,
	QMAPI_NETPT_RTSP,
    QMAPI_NETPT_HIK,
    QMAPI_NETPT_DH,
    QMAPI_NETPT_XM,
    QMAPI_NETPT_ZW,
    QMAPI_NETPT_JIUAN,
    QMAPI_NETPT_ZN,
    QMAPI_NETPT_BUTT,
}QMAPI_NETPT_TYPE_E;

typedef enum tagQMAPI_VIDEO_INPUT_TYPE
{
    QMAPI_VIDEO_INPUT_NET = 1,
    QMAPI_VIDEO_INPUT_SC1135,
    QMAPI_VIDEO_INPUT_SC2135,
    QMAPI_VIDEO_INPUT_BUTT,
}QMAPI_VIDEO_INPUT_TYPE;

/* 消息处理方式，可以同时多种处理方式，包括
* 0x00000000 - 无响应
* 0x00000001 - 报警上传中心
* 0x00000002 - 联动录象
* 0x00000004 - 云台联动
* 0x00000008 - 发送Email邮件，根据email配置是否带附件是否有效，决定是否图片。
* 0x00000010 - 本地轮巡（该版本不支持）
* 0x00000020 - 本地提示，监视器上警告
* 0x00000040 - 报警输出
* 0x00000080 - Ftp抓图上传
* 0x00000100 - 蜂鸣
* 0x00000200 - 语音提示
* 0x00000400 - 抓图本地保存
* 0x00000800 - 主动请求对讲
* FTP,EMAIL,抓图本地保存三个功能的抓图通道都是根据结构体成员bySnap决定。
* 如果bySnap没有指定抓图通道，则FTP，抓图本地保存功能失效；EMAIL只发送文本信息，不带图片附件。
*/
#define QMAPI_ALARM_EXCEPTION_NORESPONSE                  0x00000000  
#define QMAPI_ALARM_EXCEPTION_UPTOCENTER                  0x00000001
#define QMAPI_ALARM_EXCEPTION_TOREC                       0x00000002
#define QMAPI_ALARM_EXCEPTION_TOPTZ                       0x00000004
#define QMAPI_ALARM_EXCEPTION_TOEMAIL                     0x00000008
#define QMAPI_ALARM_EXCEPTION_TOPOLL                      0x00000010
#define QMAPI_ALARM_EXCEPTION_TOSCREENTIP                 0x00000020
#define QMAPI_ALARM_EXCEPTION_TOALARMOUT                  0x00000040
#define QMAPI_ALARM_EXCEPTION_TOFTP                       0x00000080
#define QMAPI_ALARM_EXCEPTION_TOBEEP                      0x00000100
#define QMAPI_ALARM_EXCEPTION_TOVOICE                     0x00000200
#define QMAPI_ALARM_EXCEPTION_TOSNAP                      0x00000400
#define QMAPI_ALARM_EXCEPTION_TOTALK                      0x00000800

// 报警联动结构体
typedef struct 
{
    /* 当前报警所支持的处理方式，按位掩码表示 */
    WORD                wActionMask;
    
    /* 触发动作，按位掩码表示，具体动作所需要的参数在各自的配置中体现 */
    WORD                wActionFlag;
    
    /* 报警触发的输出通道，报警触发的输出，为1表示触发该输出 */
    BYTE                byRelAlarmOut[QMAPI_MAX_ALARMOUT/8];
    
    /* 联动录象 */
    BYTE                byRecordChannel[QMAPI_MAX_CHANNUM/8]; /* 报警触发的录象通道，为1表示触发该通道 */
    
    /* 抓图通道 */
    BYTE                bySnap[QMAPI_MAX_CHANNUM/8]; /*0表示不抓拍该通道，1表示抓拍当前通道*/
    /* 轮巡通道 */
    BYTE                byTour[QMAPI_MAX_CHANNUM/8]; /* 该版本暂不生效*/
    
    /* 云台联动 */
    QMAPI_PTZ_LINK        stPtzLink[QMAPI_MAX_CHANNUM];
    WORD                wDuration;              /* 报警输出持续时间 */
    WORD                wRecTime;               /* 联动录象持续时间 */
    WORD                wSnapNum;              /* 连续抓拍图片数量*/
    BYTE                wDelayTime;             /* 报警间隔时间，s为单位*/
    BYTE                wBuzzerTime;            /* 蜂鸣器输出时间*/
     
} QMAPI_HANDLE_EXCEPTION;

/*录像参数*/
#define QMAPI_NET_RECORD_TYPE_SCHED   0x000001  //定时录像
#define QMAPI_NET_RECORD_TYPE_MOTION  0x000002  //移到侦测录像
#define QMAPI_NET_RECORD_TYPE_ALARM   0x000004  //报警录像
#define QMAPI_NET_RECORD_TYPE_CMD     0x000008  //命令录像
#define QMAPI_NET_RECORD_TYPE_MANU    0x000010  //手工录像

/** 录像计划*/
typedef struct
{
	unsigned char byUser; /* 使用 */
	unsigned char byRecordType; /* 录象类型 按位有效，见QMAPI_NET_RECORD_TYPE_*/
	
	unsigned char byStartHour;
	unsigned char byStartMin;

	unsigned char byEndHour;
	unsigned char byEndMin;

	unsigned char res[2]; /* 预留 */

} QMAPI_RECORDSCHED;

#define     CFG_NO_DEFAULT      0
#define     CFG_SOFTRESET       1
#define     CFG_HARDRESET       2

typedef struct tagQMAPI_SYSFLAG_INFO
{
    int     nConfigDefaultFlag; //复位类型标识，0不需要复位，1 软复位，2 硬件复位
    DWORD   dwConfigDefaultMask;   //软复位参数掩码标记
}QMAPI_SYSFLAG_INFO;

/* DEVICE SYSTEM INFO 设备信息*/
typedef struct tagQMAPI_NET_DEVICE_INFO {
    DWORD		dwSize;
    char		csDeviceName[QMAPI_NAME_LEN];	//DVR名称
    DWORD		dwDeviceID;					//DVR ID,用于遥控器

    BYTE		byRecordLen;				//录像文件打包时长,以分钟为单位
    BYTE		byLanguage;					//语言类型,详细见 QMAPI_LANGUAGE_TYPE
    BYTE		byRecycleRecord;			//是否循环录像,0:不是; 1:是
    BYTE		byOverWritePeriod;			//录像覆盖周期/ 以小时为单位

    BYTE		byVideoStandard;			//视频制式
    BYTE		byDateFormat;				/*日期格式(年月日格式)：
    											0－XXXX-XX-XX 年月日  1－XX-XX-XXXX 月日年 2－XX-XX-XXXX 日月年 */
                                    
    BYTE		byDateSprtr;				//日期分割符(0：":"，1："-"，2："/"). 2011-01-21 david: 无效字段
    BYTE		byTimeFmt;					//时间格式 (0-24小时，1－12小时).

	BYTE		byConfigWizard;				//是否启用了开机向导
	BYTE		byReserve[3];

    //以下不能修改
    DWORD		dwSoftwareVersion;			//软件版本号，高16位是主版本，低16位是次版本
    DWORD		dwSoftwareBuildDate;		//软件生成日期，0xYYYYMMDD
    DWORD		dwDspSoftwareVersion;
    DWORD		dwDspSoftwareBuildDate;
    DWORD		dwPanelVersion;
    DWORD		dwPanelSoftwareBuildDate;
    DWORD		dwHardwareVersion;
    DWORD		dwHardwareBuildDate;
    DWORD		dwWebVersion;
    DWORD		dwWebBuildDate;

    BYTE		csSerialNumber[QMAPI_SERIALNO_LEN];//序列号
    DWORD		dwServerCPUType;			//当前CPU类型(QMAPI_CPU_TYPE_E)
    DWORD		dwSysFlags;					//系统支持的功能
    DWORD		dwServerType;				//设备类型(QMAPI_SERVER_TYPE_E)

    BYTE		byVideoInNum;				/* 最大支持的Video Channel个数 */
    BYTE		byAudioInNum;				/* 最大支持的Audio Channel个数 */
    BYTE		byAlarmInNum;				//DVR报警输入个数
    BYTE		byAlarmOutNum;				//DVR报警输出个数
    
    BYTE		byDiskNum;					//DVR 硬盘个数
    BYTE		byRS232Num;					//DVR 232串口个数
    BYTE		byRS485Num;					//DVR 485串口个数
    BYTE		byNetworkPortNum;			//网络口个数

    BYTE		byDecordChans;				//DVR 解码路数
    BYTE		byVGANum;					//VGA口的个数
    BYTE		byUSBNum;					//USB口的个数
    BYTE		byDiskCtrlNum;				//硬盘控制器个数

    BYTE		byAuxOutNum;				//辅口的个数
    BYTE		byStreamNum;				//每路视频最大可以支持视频流数
    BYTE      byVideoInputType;
    BYTE		byResvered[1];
} QMAPI_NET_DEVICE_INFO;


//网络配置结构
typedef struct
{
    char  csIpV4[QMAPI_MAX_IP_LENGTH];    
    BYTE  byRes[QMAPI_MAX_IPV6_LENGTH];
}QMAPI_NET_IPADDR, *LPQMAPI_NET_IPADDR;

typedef struct 
{
    
    QMAPI_NET_IPADDR  strIPAddr;  
    QMAPI_NET_IPADDR  strIPMask;  //掩码地址
    DWORD dwNetInterface;       //网络接口 1-10MBase-T 2-10MBase-T全双工 3-100MBase-TX 4-100M全双工 5-10M/100M自适应
    WORD  wDataPort;             //数据端口
    WORD  wMTU;                 //MTU大小
    BYTE  byMACAddr[QMAPI_MACADDR_LEN];   //Mac地址
    char   csNetName[QMAPI_NAME_LEN]; //网卡名称，用于多网卡区别
    BYTE  byRes[2]; 
}QMAPI_NET_ETHERNET, *LPQMAPI_NET_ETHERNET;


typedef struct tagQMAPI_NET_NETWORK_CFG
{
    DWORD       dwSize;
    QMAPI_NET_ETHERNET    stEtherNet[QMAPI_MAX_ETHERNET];

    QMAPI_NET_IPADDR    stMulticastIpAddr;    
    QMAPI_NET_IPADDR    stGatewayIpAddr;
    QMAPI_NET_IPADDR    stManagerIpAddr;    //远程管理主机地址(服务器告警时，自动连接的对讲IP)
    QMAPI_NET_IPADDR    stDnsServer1IpAddr;
    QMAPI_NET_IPADDR    stDnsServer2IpAddr;                               

    BYTE        byEnableDHCP;         //    
    BYTE        byMaxConnect;         //网络最大连接数
    BYTE        byReserve[2];         //保留
    WORD            wHttpPort;            //Http端口
    WORD        wReserve1;            //david 2013-03-07 预留
    WORD            wManagerPort;         //远程管理主机端口
    WORD            wMulticastPort;       //多播端口
    //BYTE        byAutoDHCPDNS;      //david 废弃，dns和ip地址共用byEnableDHCP
    DWORD       dwReserve2;
}QMAPI_NET_NETWORK_CFG, *PQMAPI_NET_NETWORK_CFG;

typedef struct tagQMAPI_NET_NETWORK_STATUS
{
    DWORD       dwSize;
    BOOL        bNetBroke;          //0 正常,1 网络物理连接中断
    BOOL        bIPConflict;        //0 正常,1 ip地址冲突
    char        byRes[32];
}QMAPI_NET_NETWORK_STATUS, *PQMAPI_NET_NETWORK_STATUS;

typedef struct tagQMAPI_NET_PLATFORM_INFO
{
    DWORD               dwSize;
    BOOL                bEnable;
    QMAPI_NET_IPADDR      strIPAddr;                  //中心IP
    DWORD               dwCenterPort;               //中心端口
    char                    csServerNo[64];             //服务器序列号

//  int                     nPlatManufacture;                   //厂家类型
    QMAPI_NETPT_TYPE_E     nPlatManufacture;                  //厂家类型
    int                 nNetMode;                   //网络模式0: 自适应1: 被动模式 2: 主动模式
    int                 bEnableUpnp;                //是否启用upnp，被动模式有效
    int                 nMsgdPort;                  //监听命令端口，被动模式有效
    int                 nVideoPort;                 //音视频监听端口，被动模式有效
    int                 nTalkPort;                  //对讲监听端口，被动模式有效
    int                 nVodPort;                   // 音视频点播端口，被动模式有效
    BYTE                byMsgdPortOK;           //MsgPort UPNP is ok
    BYTE                byVideoPortOK;          //VideoPort UPNP is ok
    BYTE                byTalkPortOK;               //TalkPort UPNP is ok
    BYTE                byVodPortOK;                //VodPort UPNP is ok
    char                pData[252];                 //预留数据
}QMAPI_NET_PLATFORM_INFO;

typedef struct tagQMAPI_NET_PLATFORM_INFO_V2
{
    DWORD               dwSize;
    BOOL                bEnable;
    QMAPI_NET_IPADDR      strIPAddr;                  //中心地址
    DWORD               dwCenterPort;               //中心端口
    char                    csServerNo[64];             //服务器序列号
    QMAPI_NETPT_TYPE_E        enPlatManufacture;              //厂家类型
    char                pPrivateData[280];          //预留数据, 根据厂家类型决定厂家私有信息
}QMAPI_NET_PLATFORM_INFO_V2;

typedef struct
{
    DWORD dwSize;
    BOOL  bPPPoEEnable;              //0-不启用,1-启用
    char  csPPPoEUser[QMAPI_NAME_LEN]; //PPPoE用户名
    char  csPPPoEPassword[QMAPI_PASSWD_LEN]; //PPPoE密码
    QMAPI_NET_IPADDR  stPPPoEIP; //PPPoE IP地址(只读)
    DWORD dwSecurityProtocol; /**< 加密协议 值范围参考:HI_CFG_PPPOE_SECURITY_PROTOCOL_E*/
}QMAPI_NET_PPPOECFG, *LPQMAPI_NET_PPPOECFG;


typedef struct tagQMAPI_NET_DDNSCFG
{
    DWORD   dwSize;
    BOOL    bEnableDDNS;                //是否启用DDNS
    struct {
        QMAPI_DDNS_TYPE    enDDNSType;                  //DDNS服务器类型, 域名解析类型：QMAPI_DDNS_TYPE
        char    csDDNSUsername[QMAPI_NAME_LEN];
        char    csDDNSPassword[QMAPI_PASSWD_LEN];
        char    csDDNSDomain[QMAPI_MAX_DOMAIN_NAME];          //在DDNS服务器注册的域名地址
        char    csDDNSAddress[QMAPI_MAX_DOMAIN_NAME];         //DNS服务器地址，可以是IP地址或域名
        int nDDNSPort;                  //DNS服务器端口，默认为6500
        int nUpdateTime;              /*更新时间*/
        int     nStatus;                        /*状态,0:成功,其他值代表错误号*/
        
        BYTE byReserve[32];               /**< 保留字,按讨论结果预留32字节 */
    }stDDNS[QMAPI_MAX_DDNS_NUMS];
}QMAPI_NET_DDNSCFG, *PQMAPI_NET_DDNSCFG;

typedef struct QMAPI_NET_EMAIL_ADDR
{
    char  csName[QMAPI_NAME_LEN];/**< 邮件地址对应的用户名 */
    char  csAddress[QMAPI_MAX_DOMAIN_NAME];   /**< 邮件地址 如: hw@huawei.com */
    BYTE byReserve[32];               /**< 保留字,按讨论结果预留32字节 */
} QMAPI_NET_EMAIL_ADDR;

typedef struct tagQMAPI_NET_EMAIL_PARAM
{
    DWORD	dwSize;

    BOOL	bEnableEmail;                   //是否启用

    BYTE	byAttachPicture;                //是否带附件
    BYTE	bySmtpServerVerify;             //发送服务器要求身份验证
    BYTE	byMailInterval;                 //最少2s钟(1-2秒；2-3秒；3-4秒；4-5秒)
    BYTE	byReserved;

    WORD	wServicePort;					/**< 服务器端口,一般为25，用户根据具体服务器设置 */
    WORD	wEncryptionType;				/**< 加密类型 ssl*/

    char	csEMailUser[QMAPI_NAME_LEN];      //账号
    char	csEmailPass[QMAPI_PASSWD_LEN];    //密码

    char    csSmtpServer[QMAPI_MAX_DOMAIN_NAME]; //smtp服务器 //用于发送邮件
    char    csPop3Server[QMAPI_MAX_DOMAIN_NAME]; //pop3服务器 //用于接收邮件,和IMAP性质类似

    QMAPI_NET_EMAIL_ADDR stToAddrList[QMAPI_EMAIL_ADDR_MAXNUM]; /**< 收件人地址列表  */
    QMAPI_NET_EMAIL_ADDR stCcAddrList[QMAPI_EMAIL_ADDR_MAXNUM]; /**< 抄送地址列表 */
    QMAPI_NET_EMAIL_ADDR stBccAddrList[QMAPI_EMAIL_ADDR_MAXNUM];/**< 密送地址列表 */
    QMAPI_NET_EMAIL_ADDR stSendAddrList;                          /**< 发送人地址 */

	BYTE    byEmailType[1];					//复用为发送邮件类型 8 - Motion 4 - Video Loss  2 - IOAlarm  1- Other
    char    csReserved[31];

}QMAPI_NET_EMAIL_PARAM, *PQMAPI_NET_EMAIL_PARAM;

/* ftp上传参数*/

typedef struct tagQMAPI_NET_FTP_PARAM
{
    DWORD   dwSize;
    BOOL    bEnableFTP;         /*是否启动ftp上传功能*/
    char        csFTPIpAddress[QMAPI_MAX_DOMAIN_NAME];        /*ftp 服务器，可以是IP地址或域名*/
    DWORD   dwFTPPort;              /*ftp端口*/
    char        csUserName[QMAPI_NAME_LEN];           /*用户名*/
    char        csPassword[QMAPI_PASSWD_LEN];         /*密码*/
    WORD    wTopDirMode;            /*0x0 = 使用设备ip地址,0x1 = 使用设备名,0x2 = OFF*/
    WORD    wSubDirMode;            /*0x0 = 使用通道号 ,0x1 = 使用通道名,0x2 = OFF*/
    BYTE    reservedData[28]; //保留

}QMAPI_NET_FTP_PARAM, *LPQMAPI_NET_FTP_PARAM;

/* 定时抓图 */
typedef struct tagQMAPI_NET_SNAP_TIMER
{
    DWORD       dwSize;
    BOOL                bEnable;
    QMAPI_SCHEDTIME stScheduleTime[7][4];/**该通道的videoloss的布防时间*/
    DWORD          dwInterval;      //单位：毫秒
    int                 nPictureQuality;//编码质量(0-4),0为最好
    int                 nImageSize;     // 画面大小；0：QCIF，1：CIF，2：D1
    BYTE        byFtpUpload;
    BYTE        byLocalSave;
    BYTE        byRes[6];
}QMAPI_NET_SNAP_TIMER, *LPQMAPI_NET_SNAP_TIMER;

/*录像文件FTP上传*/
typedef struct tagQMAPI_NET_RECORD_UPLOAD
{
    DWORD           dwSize;
    BOOL            bEanble;
    QMAPI_RECORDSCHED stScheduleTime; //上传时间段，星期一到星期天
}QMAPI_NET_RECORD_UPLOAD, *LPQMAPI_NET_RECORD_UPLOAD;

/*      WIFI   */
typedef struct tagQMAPI_NET_WIFI_SITE
{
    DWORD           dwSize;
    int             nType;
    char                csEssid[QMAPI_NAME_LEN];
    int             nRSSI;    //wifi 信号强度
    BYTE            byMac[QMAPI_MACADDR_LEN];
    BYTE                byRes1[2];
    BYTE                bySecurity;  
                            //0: none  (wifi enable)
                            // 1:web 64 assii (wifi enable)
                            // 2:web 64 hex (wifi enable)
                            // 3:web 128 assii (wifi enable)
                            // 4:web 128 hex (wifi enable)
                            //5:  WPAPSK-TKIP 
                                        //6: WPAPSK-AES
                                        //7: WPA2PSK-TKIP
                                        //8: WPA2PSK-AES 
                                        //9:
                                        //10:WPS
    BYTE                byRes2[3];
    int             nChannel;                           
}QMAPI_NET_WIFI_SITE;

typedef struct tagQMAPI_NET_WIFI_SITE_LIST
{
    DWORD           dwSize; //== sizeof(QMAPI_NET_WIFI_SITE)*nCount
    int             nCount;
    QMAPI_NET_WIFI_SITE    stWifiSite[100];// 实际传输
}QMAPI_NET_WIFI_SITE_LIST;

//wifi 状态不完全，须扩展
typedef enum tagWIRELESS_STATUS{
    WL_OK = 0,
    WL_NOT_CONNECT,     //没有在连接
    WL_DEVICE_NOT_EXIST,    //ipc的wifi硬件不存在
    WL_ESSID_NOT_EXIST, //essid不存在
    WL_DHCP_ERROR,          //dhcp获取不到ip
    WL_ENCRYPT_FAIL,        //密码认证错误
    WL_IP_CONFLICT,         //IP地址冲突
}WIRELESS_STATUS_E;

typedef struct tagQMAPI_NET_WIFI_CONFIG
{
    DWORD           dwSize;
    BYTE            bWifiEnable;
    BYTE			byWifiMode;		//0:station, 1:ap
    BYTE			byWpsStatus;	//wps状态(0:关闭,1:启用)
    BYTE            byWifiSetFlag; //0:手动配置过或成功登陆过 1:校正 2:自动配置wifi 
    QMAPI_NET_IPADDR  dwNetIpAddr;
    QMAPI_NET_IPADDR  dwNetMask;
    QMAPI_NET_IPADDR  dwGateway;
    QMAPI_NET_IPADDR  dwDNSServer;
    BYTE            byMacAddr[6];
    char            csEssid[32];
    BYTE            byRes1[32];
    BYTE            nSecurity;
						// 0: none  (wifi enable)
						// 1: wep 64 assii (wifi enable)
						// 2: wep 64 hex (wifi enable)
						// 3: wep 128 assii (wifi enable)
						// 4: wep 128 hex (wifi enable)
						// 5: WPAPSK-TKIP 
						// 6: WPAPSK-AES
						// 7: WPA2PSK-TKIP
						// 8: WPA2PSK-AES 
						// 9: AUTOMATCH
    BYTE        	byNetworkType;  // 1. Infra 2. ad-hoc
    BYTE            byEnableDHCP;
    BYTE            byStatus; //    0:  成功,其他值是错误码
    char            csWebKey[32];
    BYTE            byRes2[64];
}QMAPI_NET_WIFI_CONFIG, *LPQMAPI_NET_WIFI_CONFIG;


#define WIRELESS_TDSCDMA_ZX      0x02
#define WIRELESS_EVDO_HUAWEI    0x03
#define WIRELESS_WCDMA_HUAWEI 0x04

typedef struct tagQMAPI_NET_WANWIRELESS_CONFIG
{
    DWORD       dwSize;
    BOOL        bEnable;
    BYTE        byDevType; //WIRELESS_XXX 0x2---0x4
    BYTE        byStatus;
    BYTE        byReserve[2];
    DWORD       dwNetIpAddr;            //IP  (wifi enable)

}QMAPI_NET_WANWIRELESS_CONFIG, *LPQMAPI_NET_WANWIRELESS_CONFIG;

/**UPNP**/
typedef struct tagQMAPI_UPNP_CONFIG
{
    DWORD       dwSize;
    BOOL        bEnable;                /*是否启用upnp*/
    DWORD       dwMode;                 /*upnp工作方式.0为自动端口映射，1为指定端口映射*/
    DWORD       dwLineMode;             /*upnp网卡工作方式.0为有线网卡,1为无线网卡*/
    char            csServerIp[32];         /*upnp映射主机.即对外路由器IP*/
    DWORD       dwDataPort;             /*upnp映射数据端口*/
    DWORD       dwWebPort;              /*upnp映射网络端口*/
    DWORD       dwMobilePort;           /*upnp映射手机端口*/

    WORD        dwDataPort1;            /*upnp已映射成功的数据端口*/
    WORD        dwWebPort1;             /*upnp已映射成功的网络端口*/
    WORD        dwMobilePort1;          /*upnp映射成功的手机端口*/
    WORD            wDataPortOK;
    WORD            wWebPortOK;
    WORD            wMobilePortOK;
}QMAPI_UPNP_CONFIG, *LPQMAPI_UPNP_CONFIG;

/*--------------------------通道设置结构--------------------------------*/
//通道OSD显示以及通道名称设置

typedef struct tagQMAPI_NET_OSD_S
{
    BOOL        bOsdEnable;         /**< OSD使能*/
    DWORD   dwOsdContentType;    /**< OSD内容类型HI_CFG_OSD_TYPE_E */
    DWORD   dwLayer;             /**< 区域层次，在多个区域叠加时有用,0 ~100*/
    DWORD   dwAreaAlpha;         /**< OSD区域Alpha值*/
    DWORD   dwFgColor;           /**< OSD颜色, 象素格式RGB8888 */
    DWORD   dwBgColor;           /**< OSD背景颜色, 象素格式RGB8888 */
    int     nLeftX;       /**< 区域左上角相对于窗口左上角X坐标比例:0~255*/
    int     nLeftY;       /**< 区域左上角相对于窗口左上角Y坐标比例:0~255*/
    int     nWidth;       /**< 区域宽，为绝对值,图片时有效*/
    int     nHeight;       /**< 区域高，为绝对值,图片时有效*/
    char        csOsdCotent[QMAPI_MAX_OSD_LEN];/**< OSD为字符时，支持输入string；
                                                 OSD为图片时，支持path: "/bin/osd.bmp";
                                                 如果是图片内存，前4位为内存地址*/
}QMAPI_NET_OSD_S;

typedef struct tagQMAPI_NET_CHANNEL_OSDINFO
{
    DWORD   dwSize;
    DWORD   dwChannel;
    BOOL    bShowTime;
    BOOL    bDispWeek; /*是否显示星期*/
    DWORD   dwTimeTopLeftX; /*OSD左上角的x坐标*/
    DWORD   dwTimeTopLeftY;/*OSD左上角的y坐标*/
    BYTE    byReserve1;// 2011-01-21 david: 无效字段，直接参照tagQMAPI_NET_DEVICE_INFO 结构体
                 /*OSD类型(年月日格式)：
                */
    BYTE       byOSDAttrib; 
                /*
                OSD属性（透明/闪烁）：
                1－透明，闪烁
                2－透明，不闪烁
                3－闪烁，不透明
                4－不透明，不闪烁
                */
    BYTE    byReserve2;  // 2011-01-21 david: 无效字段，直接参照tagQMAPI_NET_DEVICE_INFO 结构体
                
    BYTE    byShowChanName ;    //预览的图象上是否显示通道名称：0-不显示，1-显示（区域大小704*576）
    DWORD   dwShowNameTopLeftX ;    //通道名称显示位置的x坐标
    DWORD   dwShowNameTopLeftY ;    //通道名称显示位置的y坐标
    char    csChannelName[QMAPI_NAME_LEN];    //通道名称
    QMAPI_NET_OSD_S stOsd[QMAPI_MAX_OSD_NUM];/**源通道编码OSD 2011-01-21 david: 字段预留*/
}QMAPI_NET_CHANNEL_OSDINFO, *LPQMAPI_NET_CHANNEL_OSDINFO;

//音频编码参数
typedef struct tagQMAPI_NET_AUDIO_FORMAT_INFO
{
    WORD    wFormatTag;        /* format type */
    WORD    wChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   dwSamplesPerSec;    /* sample rate */
    DWORD   dwAvgBytesPerSec;   /* for buffer estimation */
    WORD    wBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
}QMAPI_NET_AUDIO_FORMAT_INFO, *LPQMAPI_NET_AUDIO_FORMAT_INFO;

//视频通道压缩参数
typedef struct tagQMAPI_NET_COMPRESSION_INFO
{
    DWORD   dwCompressionType;      //
    DWORD   dwFrameRate;            //帧率 (1-25/30) PAL为25，NTSC为30
    DWORD   dwStreamFormat;         //视频分辨率 (0为CIF,1为D1,2为HALF-D1,3为QCIF) QMAPI_VIDEO_FORMAT_CIF
    WORD    wHeight;            //
    WORD    wWidth;             //
    DWORD   dwRateType;         //流模式(0为定码流，1为变码流)
    DWORD   dwBitRate;          //码率 (16000-4096000)
    DWORD   dwImageQuality;     //编码质量(0-4),0为最好
    DWORD   dwMaxKeyInterval;   //关键帧间隔(1-100)
    
    WORD    wEncodeAudio;       //是否编码音频
    WORD    wEncodeVideo;       //是否编码视频

    WORD    wFormatTag;        /* format type */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */

    BYTE    byReseved[16];
}QMAPI_NET_COMPRESSION_INFO, *LPQMAPI_NET_COMPRESSION_INFO;

typedef struct tagQMAPI_NET_CHANNEL_PIC_INFO
{
    DWORD   dwSize;
    DWORD   dwChannel;
    BYTE	csChannelName[QMAPI_NAME_LEN];    //通道名称

    QMAPI_NET_COMPRESSION_INFO   stRecordPara; /* 录像 */
    QMAPI_NET_COMPRESSION_INFO   stNetPara;    /* 网传 */
    QMAPI_NET_COMPRESSION_INFO   stPhonePara;  /* 手机监看 */
    QMAPI_NET_COMPRESSION_INFO   stEventRecordPara; /*事件触发录像压缩参数*/
}QMAPI_NET_CHANNEL_PIC_INFO, *LPQMAPI_NET_CHANNEL_PIC_INFO;


//遮挡区域设置
typedef struct tagQMAPI_NET_SHELTER_RECT
{
    DWORD   nType;          /* 遮挡使能 0-禁用, 1-编码遮挡, 2-预览遮挡, 3-全部遮挡 */
    WORD    wLeft;          /* 遮挡区域左上角的x坐标 */
    WORD    wTop;           /* 遮挡区域左上角的y坐标 */
    WORD    wWidth;         /* 遮挡区域宽度 */
    WORD    wHeight;        /* 遮挡区域高度 */
    DWORD   dwColor;        /* 遮挡颜色, 默认 0:黑色 按RGB格式*/
}QMAPI_NET_SHELTER_RECT;

typedef struct tagQMAPI_NET_CHANNEL_SHELTER
{
    DWORD   dwSize;
    DWORD   dwChannel;
    BOOL    bEnable;        //是否进行区域遮挡
    QMAPI_NET_SHELTER_RECT    strcShelter[QMAPI_MAX_VIDEO_HIDE_RECT];   //遮挡区域，最多支持5块的区域遮挡。RECT以D1为准
}QMAPI_NET_CHANNEL_SHELTER,*PQMAPI_NET_CHANNEL_SHELTER;

typedef struct tagQMAPI_NET_MOTION_DETECT
{
    DWORD           dwSize;
    DWORD           dwChannel;
    BOOL            bEnable;                //是否进行布防
    DWORD           dwSensitive;            //灵敏度 取值0 - 5, 越小越灵敏*/
    BOOL                bManualDefence;     //(david 20130203 废弃) 手动布防标志，==YES(1)启动，==NO(0)按定时判断
    BYTE            byMotionArea[QMAPI_MD_STRIDE_SIZE];   //布防区域块组.布防区域共有44*36个块,数据以行BIT 为单位的内存宽度。
    QMAPI_HANDLE_EXCEPTION   stHandle;    
    QMAPI_SCHEDTIME stScheduleTime[8][4];/**该通道的videoloss的布防时间*/
}QMAPI_NET_CHANNEL_MOTION_DETECT, *LPQMAPI_NET_CHANNEL_MOTION_DETECT;

//信号丢失报警
typedef struct{
    DWORD    dwSize;
    DWORD   dwChannel;
    BOOL     bEnable; /* 是否处理信号丢失报警 */
    QMAPI_HANDLE_EXCEPTION   stHandle; /* 处理方式 */
    QMAPI_SCHEDTIME stScheduleTime[7][4];/**该通道的videoloss的布防时间*/
}QMAPI_NET_CHANNEL_VILOST,*LPQMAPI_NET_CHANNEL_VILOST;

//遮挡报警区域大小704*576
typedef struct
{
    DWORD dwSize;
    DWORD   dwChannel;
    BOOL bEnable; /* 是否启动遮挡报警 ,0-否,1-低灵敏度 2-中灵敏度 3-高灵敏度*/
    WORD wHideAlarmAreaTopLeftX; /* 遮挡区域的x坐标 */
    WORD wHideAlarmAreaTopLeftY; /* 遮挡区域的y坐标 */
    WORD wHideAlarmAreaWidth; /* 遮挡区域的宽 */
    WORD wHideAlarmAreaHeight; /*遮挡区域的高*/
    QMAPI_HANDLE_EXCEPTION stHandle; /* 处理方式 */
    QMAPI_SCHEDTIME stScheduleTime[7][4];/**该通道的videoloss的布防时间*/
}QMAPI_NET_CHANNEL_HIDEALARM,*LPQMAPI_NET_CHANNEL_HIDEALARM;

typedef struct tagQMAPI_NET_TRHD_DETECT
{
	BYTE			byTUplimit;					//温度报警上限
	BYTE			byTDownlimit;				//温度报警下限
	BYTE			byHUplimit;					//湿度报警上限
	BYTE			byHDownlimit;				//湿度报警下限
	QMAPI_SCHEDTIME	stTRScheduleTime[7][4];		//温度报警布防时间
	QMAPI_SCHEDTIME	stHDScheduleTime[7][4];		//湿度报警布防时间
}QMAPI_NET_CHANNEL_TRHD_DETECT, *LPQMAPI_NET_CHANNEL_TRHD_DETECT;


#define MAX_REC_SCHED 8

typedef struct 
{
    DWORD			dwSize;    			/* 此结构的大小 */
    DWORD			dwChannel;
    DWORD			dwRecord;  			/*是否录像 0-否 1-是*/
    QMAPI_RECORDSCHED stRecordSched[7][MAX_REC_SCHED];		/*录像时间段，星期一到星期天*/
    DWORD			dwPreRecordTime;	/*预录时间，单位是s，0表示不预录*/
    DWORD			dwRecorderDuration; /*录像保存的最长时间*/
    BYTE			byRedundancyRec;	/*是否冗余录像,重要数据双备份：0/1, 默认为不启用*/
    BYTE			byAudioRec;			/*录像时复合流编码时是否记录音频数据：国外有此法规,目前版本不支持*/
    BYTE			byRecordMode;		//0:自动模式（按定时录像配置） 1:手动录像模式（全天侯录像），2:禁止所有方式触发录像
    BYTE  			byStreamType;		// 录像码流类型 0:主码流 1:子码流
	BYTE  			byReserve[8];
}QMAPI_NET_CHANNEL_RECORD, *LPQMAPI_NET_CHANNEL_RECORD;

typedef struct tagQMAPI_NET_RECORD_STREAMMODE
{
    DWORD         dwSize;
    DWORD         dwStreamType;//0:first stream,1:second stream,2,third stream
    unsigned char byRes[16];
}QMAPI_NET_RECORD_STREAMMODE;

#define QMAPI_ENC_ENCRYPT_KEY_LEN 128
#define QMAPI_ENC_WATERMARK_CONTENT_LEN 16
#define QMAPI_ENC_WATERMARK_KEY_LEN  16
#define QMAPI_ENC_MAX_OSD_NUM 4

typedef struct 
{
    DWORD u32EncryptEnable;                 /**< 码流加密使能: 0 关闭; 1 开启 */
    char    as8Key[QMAPI_ENC_ENCRYPT_KEY_LEN];   /**< 码流加密密钥*/
}QMAPI_NET_CHANNEL_ENCRYPT;


typedef struct
{
    int     s32WmEnable;                                /**< 水印开启标志: 0 关闭; 1 开启*/
    char    as8WaterMark[QMAPI_ENC_WATERMARK_CONTENT_LEN]; /**< 水印内容 */
    char    as8Key[QMAPI_ENC_WATERMARK_KEY_LEN];           /**< 水印密钥 */
}QMAPI_NET_CHANNEL_WATERMARK;


/*报警输入*/
typedef struct
{
    DWORD   dwSize;                 /* 此结构的大小 */
    DWORD   dwChannel;
    char    csAlarmInName[QMAPI_NAME_LEN];    /* 名称 */
    BYTE    byAlarmType;                /* 报警器类型（目前版本保留）,0：常开,1：常闭 */
    BYTE    byAlarmInHandle;            /* 是否处理 0：不处理,1：处理*/
    char    reserve[2];
    QMAPI_HANDLE_EXCEPTION stHandle; /* 处理方式 */
    QMAPI_SCHEDTIME stScheduleTime[7][4];/**该通道的videoloss的布防时间*/
}QMAPI_NET_ALARMINCFG, *LPQMAPI_NET_ALARMINCFG;

/*报警输出*/
typedef struct
{
    DWORD   dwSize; /* 此结构的大小 */
    DWORD   dwChannel;
    BYTE    csAlarmOutName[QMAPI_NAME_LEN]; /* 名称 */
    QMAPI_SCHEDTIME stScheduleTime[7][4];/**该通道的videoloss的布防时间*/
}QMAPI_NET_ALARMOUTCFG, *LPQMAPI_NET_ALARMOUTCFG;


typedef struct tagQMAPI_NET_ALARM_SERVER
{
    QMAPI_ALARM_TYPE_E eAlarmType;
    char    *pAlarmEntity;
    struct tagDMS_SYSNETAPI *pNext;
    char    reserve[8];
}QMAPI_NET_ALARM_SERVER, *LPQMAPI_NET_ALARM_SERVER;


/*云台解码器485串口参数*/
typedef struct
{
    DWORD dwSize; /* 此结构的大小 */
    DWORD   dwChannel;
    DWORD dwBaudRate; /* 波特率(bps)，0－50；1－75；2－110；3－150；4－300；5－600；6－1200；7－2400；8－4800；9－9600；10－19200；11－38400；12－57600；13－76800；14－115.2k */
    BYTE byDataBit; /* 数据有几位 0－5位，1－6位，2－7位，3－8位 */
    BYTE byStopBit; /* 停止位 0－1位，1－2位  */
    BYTE byParity; /* 校验 0－无校验，1－奇校验，2－偶校验; */
    BYTE byFlowcontrol; /* 0－无，1－软流控,2-硬流控 */
    char  csDecoderType[QMAPI_NAME_LEN]; /* 解码器类型, 见下表*/
    WORD wDecoderAddress; /*解码器地址:0 - 255*/
    BYTE byHSpeed;      //云台H速度
    BYTE byVSpeed;      //云台V速度
    DWORD dwRes;
}QMAPI_NET_DECODERCFG,*LPQMAPI_NET_DECODERCFG;

//云台协议表结构配置
typedef struct
{ 
    DWORD dwType;               /*解码器类型值，从1开始连续递增*/
    char  csDescribe[QMAPI_NAME_LEN]; /*解码器的描述符*/
}QMAPI_PTZ_PROTOCOL;

typedef struct
{
    DWORD   dwSize;    
    QMAPI_PTZ_PROTOCOL stPtz[QMAPI_PTZ_PROTOCOL_NUM];/*最大200中PTZ协议*/
    DWORD   dwPtzNum;           /*有效的ptz协议数目*/
    BYTE    byRes[8];           // 保留参数
}QMAPI_NET_PTZ_PROTOCOLCFG, *LQMAPI_NET_PTZ_PROTOCOLCFG;

//球机位置信息
typedef struct
{
    DWORD   dwSize;
    int     nAction;        //
    float   nPanPos;        //水平参数
    float   nTiltPos;       //垂直参数
    float   nZoomPos;       //变倍参数
    BYTE    byRes[8];       // 保留参数	
}QMAPI_NET_PTZ_POS;

/*上传云台协议*/
typedef struct 
{
    DWORD dwSize;                   /* 包含pData的数据实际长度*/
    BYTE  byDescribe[QMAPI_NAME_LEN]; /*解码器的描述符*/
    BYTE  pData[1];                
}QMAPI_NET_PTZ_PROTOCOL_DATA;

/*232串口参数*/
typedef struct
{
    DWORD dwSize; /* 此结构的大小 */
    DWORD dwBaudRate; /* 波特率(bps) */
    BYTE byDataBit; /* 数据有几位 0－5位，1－6位，2－7位，3－8位; */
    BYTE byStopBit; /* 停止位 0－1位，1－2位; */
    BYTE byParity; /* 校验 0－无校验，1－奇校验，2－偶校验; */
    BYTE byFlowcontrol; /* 0－无，1－软流控,2-硬流控 */
    DWORD dwWorkMode; /* 工作模式，0－窄带传输（232串口用于PPP拨号），1－控制台（232串口用于参数控制），2－透明通道 */
}QMAPI_NET_RS232CFG, *LPQMAPI_NET_RS232CFG;

/************************************************************************/
/* 用户权限管理、认证                                                   */
/************************************************************************/
#define RIGHT_LOCAL_PTZ             0x00000001  //本地控制云台
#define RIGHT_LOCAL_MANUAL_RECORD   0x00000002  //本地手动录象
#define RIGHT_LOCAL_PLAYBACK        0x00000004  //本地回放
#define RIGHT_LOCAL_PARAMSET        0x00000008  //本地设置参数
#define RIGHT_LOCAL_QUERYLOG        0x00000010  //本地查看状态、日志
#define RIGHT_LOCAL_ADVANCED        0x00000020  //本地高级操作:升级，格式化，恢复出厂
#define RIGHT_LOCAL_VIEWPARAM       0x00000040  //本地查看参数
#define RIGHT_LOCAL_MANAGE_CHANNEL  0x00000080  //本地通道管理
#define RIGHT_LOCAL_BACKUP          0x00000100  //本地备份
#define RIGHT_LOCAL_POWER           0x00000200  //本地关机/重启
#define RIGHT_LOCAL_FORMAT          0x00000400  //本地格式化
#define RIGHT_LOCAL_USRMNG          0x00000800  //本地用户管理
#define RIGHT_LOCAL_ACCESS          0x00002000  //本地访问权限
#define RIGHT_LOCAL_MAINTAIN		0x00004000  //本地系统维护
#define RIGHT_REMOTE_USRMNG         0x00000800  //远程用户管理
#define RIGHT_REMOTE_ACCESS         0x00002000  //远程访问权限
#define RIGHT_REMOTE_MAINTAIN		0x00004000  //远程系统维护

#define RIGHT_REMOTE_PTZ            0x00010000  //远程控制云台
#define RIGHT_REMOTE_MANUAL_RECORD  0x00020000  //远程手动录象
#define RIGHT_REMOTE_PLAYBACK       0x00040000  //远程回放
#define RIGHT_REMOTE_PARAMSET       0x00080000  //远程设置参数
#define RIGHT_REMOTE_QUERYLOG       0x00100000  //远程查看状态、日志
#define RIGHT_REMOTE_ADVANCED       0x00200000  //远程高级操作:升级，格式化，恢复出厂
#define RIGHT_REMOTE_TALK           0x00400000  //远程发起语音对讲
#define RIGHT_REMOTE_ALARM_SEND     0x00800000  //远程请求报警上传、报警输出
#define RIGHT_REMOTE_CONTROL        0x01000000  //远程控制，本地输出
#define RIGHT_REMOTE_SERIAL_PORT    0x02000000  //远程控制串口
#define RIGHT_REMOTE_VIEWPARAM      0x04000000  //远程查看参数
#define RIGHT_REMOTE_MANAGE_CHANNEL 0x08000000  //远程管理模拟
#define RIGHT_REMOTE_POWER          0x10000000  //远程关机/重启
#define RIGHT_REMOTE_FORMAT         0x20000000  //远程格式化
#define RIGHT_REMOTE_BACKUP         0x40000000  //远程备份

typedef struct
{
    DWORD   dwSize;
    int             bEnable;              /*0:无效用户，禁用, 1:启用*/
    DWORD   dwIndex;          /*用户编号*/

    char        csUserName[QMAPI_NAME_LEN]; /* 用户名最大32字节*/
    char        csPassword[QMAPI_PASSWD_LEN]; /* 密码 */
    
    DWORD   dwUserRight; /* 权限 */
    
    BYTE  byLocalPreviewRight[QMAPI_MAX_CHANNUM]; //本地可以预览的通道 1-有权限，0-无权限
    
    BYTE  byNetPreviewRight[QMAPI_MAX_CHANNUM]; //远程可以预览的通道 1-有权限，0-无权限
    
    BYTE  byLocalPlaybackRight[QMAPI_MAX_CHANNUM]; //本地可以回放的通道 1-有权限，0-无权限
    
    BYTE  byNetPlaybackRight[QMAPI_MAX_CHANNUM]; //远程可以回放的通道 1-有权限，0-无权限
    
    BYTE  byLocalRecordRight[QMAPI_MAX_CHANNUM]; //本地可以录像的通道 1-有权限，0-无权限
    
    BYTE  byNetRecordRight[QMAPI_MAX_CHANNUM];//远程可以录像的通道 1-有权限，0-无权限
    
    BYTE  byLocalPTZRight[QMAPI_MAX_CHANNUM];//本地可以PTZ的通道 1-有权限，0-无权限
    
    BYTE  byNetPTZRight[QMAPI_MAX_CHANNUM];//远程可以PTZ的通道 1-有权限，0-无权限
    
    BYTE  byLocalBackupRight[QMAPI_MAX_CHANNUM];//本地备份权限通道 1-有权限，0-无权限

    BYTE  byNetBackupRight[QMAPI_MAX_CHANNUM];//远程备份权限通道 1-有权限，0-无权限
  
    QMAPI_NET_IPADDR   stIPAddr;
    BYTE    byMACAddr[QMAPI_MACADDR_LEN]; /* 物理地址 */
    BYTE    byPriority;                                 /* 优先级，0xff-无，0--低，1--中，2--高，3--最高 */
                                                                    /*
                                                                    无……表示不支持优先级的设置
                                                                    低……默认权限:包括本地和远程回放,本地和远程查看日志和状态,本地和远程关机/重启
                                                                    中……包括本地和远程控制云台,本地和远程手动录像,本地和远程回放,语音对讲和远程预览
                                                                                    本地备份,本地/远程关机/重启
                                                                    高……可以执行除了为 Administrators 组保留的任务外的其他任何操作系统任务
                                                                    最高……管理员
                                                                    */

    BYTE    byRes[17];  
}QMAPI_NET_USER_INFO,*LPQMAPI_NET_USER_INFO;


typedef struct
{
    DWORD dwSize;
    QMAPI_NET_USER_INFO stUser[QMAPI_MAX_USERNUM];
}QMAPI_NET_USER,*LPQMAPI_NET_USER;

typedef enum __QMAPI_EXCEPTION_TYPE_E
{
    QMAPI_DISK_FULL = 0,
    QMAPI_DISK_ERROR,
    QMAPI_NETWORK_DISCONNECT,
    QMAPI_IPADDR_CONFLICT,
    QMAPI_ILLEGAL_ACCESS,
    QMAPI_VMODE_NOT_MATCH,
}QMAPI_EXCEPTION_TYPE_E;
/*异常参数配置*/
typedef struct
{
    DWORD dwSize;
    QMAPI_HANDLE_EXCEPTION stExceptionHandleType[QMAPI_MAX_EXCEPTIONNUM];
    /*数组0-盘满,1- 硬盘出错,2-网线断,3-局域网内IP 地址冲突,4-非法访问, 5-输入/输出视频制式不匹配*/
}QMAPI_NET_EXCEPTION,*LPQMAPI_NET_EXCEPTION;

//本地输出、预览
typedef struct 
{
    DWORD dwSize;
    BYTE  byEnableAudio;		//是否声音预览：0－不预览；1－预览
    BYTE  byVolume;				//预览音量大小
    BYTE  byMenuTransparency;	//菜单与背景图象对比度
    BYTE  byTimeOsd;			//是否显示时间
    BYTE  byChOsd;				//是否显示通道标题
	BYTE  byAlarmFullScreenTime;//报警是否全屏显示及显示时长. 0 不全屏显示，1-60 全屏显示时间 单位为秒
	BYTE  byScreenSaveTime;		//屏保时间 GUI 隐藏时间	. 0 从不，1-60 屏幕保护时间设置 单位为分钟
	BYTE  byVgaType;			// VGA输出类型:0-VGA, 1-BT1120
	BYTE  byPreview[QMAPI_MAX_CHANNUM];
								//预览开关
    BYTE  byRes[20];
}QMAPI_NET_PREVIEWCFG, *LPQMAPI_NET_PREVIEWCFG;

//TV调节,视频输出
typedef struct 
{
    DWORD dwSize;
    DWORD dwBrightness;			/* 亮度 */
    WORD  wResolution;			/* 分辨率 SVGA(800*600), XGA(1024*768), SXGA(1280*1024)*/
    BYTE  byVideoStandard;		/* 输出制式,1-PAL,0-N */
    BYTE  byIsStartPoll;		/* 是否开机轮询 */
    BYTE  byPollMode;			/* 开机轮循画面，0-单画面，1-4画面，2-9画面，3-16画面，4-25画面，5-32画面 */
	BYTE  byPollTime;			/* 输出轮询时间间隔  单位：1秒，0表示不轮询 */
	BYTE  byStaticOrDynamic;	/* 0-static，1-dynamic(用户手动设置) */
	BYTE  byOutputMode;			/* 主输出预览模式:0-一画面，1-四画面，2-九画面，3-十六画面 */
	BYTE  byMargin[4];	
								/* byMargin[0]-OD_VOUT, byMargin[1]-OD_VSPOT, byMargin[2]-OD_VGA*/
								/* 输出边距调整 0:左边距  1:右边距  2:上边距  3:下边距 单位为像素 */
	BYTE  byPic32Chn[QMAPI_MAX_CHANNUM];
								/* 保存通道位置调换结果 */
	BYTE  byRes[4];
}QMAPI_NET_VIDEOOUTCFG, *LPQMAPI_NET_VIDEOOUTCFG;


typedef enum __QMAPI_HDSTATUS_TYPE_E
{
    QMAPI_HDSTATUS_OK = 0,		//正常
    QMAPI_HDSTATUS_UNFORMAT,		//未格式化
    QMAPI_HDSTATUS_ERROR,			//错误
    QMAPI_HDSTATUS_SMART,			//S.M.A.R.T状态
    QMAPI_HDSTATUS_UNMATCH,		//不匹配
    QMAPI_HDSTATUS_SLEEP,			//休眠
    QMAPI_HDSTATUS_NONE,			//暂定无
    QMAPI_HDSTATUS_BUTT,
}QMAPI_HDSTATUS_TYPE_E;

/*硬盘信息*/
/*单个硬盘信息*/
typedef struct
{
    DWORD  dwSize;
    DWORD  dwHDNo;      //硬盘号, 取值0～QMAPI_MAX_DISKNUM-1
    DWORD  dwHDType;    //硬盘类型(不可设置) 0:SD卡,1:U盘,2:硬盘
    DWORD  dwCapacity;  //硬盘容量(不可设置)
    DWORD  dwFreeSpace; //硬盘剩余空间(不可设置)
    DWORD  dwHdStatus;  //硬盘状态(不可设置)： 0－正常；1－未格式化；2－错误；3－S.M.A.R.T状态；4－不匹配；5－休眠
    BYTE  byHDAttr;     //0－默认；1－冗余；2－只读
    BYTE  byRecStatus;  //是否正在录像--0:空闲,1:正在录像
    BYTE  byRes1[2];    //保留参数
    DWORD  dwHdGroup;   //属于哪个盘组，取值 0～QMAPI_MAX_HD_GROUP-1
    BYTE   byPath[64];  //硬盘mount之后的路径
    BYTE  byRes2[56];   //保留
}QMAPI_NET_SINGLE_HD, *LPQMAPI_NET_SINGLE_HD;
/*本地硬盘信息配置*/
typedef struct
{    
    DWORD  dwSize;  
    DWORD  dwHDCount; //硬盘数
    QMAPI_NET_SINGLE_HD  stHDInfo[QMAPI_MAX_DISKNUM]; 
}QMAPI_NET_HDCFG, *LPQMAPI_NET_HDCFG;
/*格式化磁盘命令*/
typedef struct
{
    DWORD  dwSize;  
    DWORD  dwHDNo; //硬盘号
    BYTE  byRes2[32];//保留
}QMAPI_NET_DISK_FORMAT, *LPQMAPI_NET_DISK_FORMAT;
//格式化硬盘状态以及进度
typedef struct
{
    DWORD  dwSize;  
    DWORD  dwHDNo; //硬盘号
    DWORD  dwHdStatus;//硬盘状态： 0－格式化开始；1－正在格式化磁盘；2－格式化完成
    DWORD  dwProcess;//格式化进度 0-100
    DWORD  dwResult;//格式化结果 0：成功，1：失败
    BYTE  byRes[16];//保留
}QMAPI_NET_DISK_FORMAT_STATUS, *LPQMAPI_NET_DISK_FORMAT_STATUS;


/*单个盘组信息 第一版本不支持*/
typedef struct
{
    
    DWORD dwHDGroupNo;          //盘组号(不可设置) 0～QMAPI_MAX_HD_GROUP-1
    BYTE  byHDGroupChans[64];  //盘组对应的录像通道, 0－表示该通道不录象到该盘组；1－表示录象到该盘组
    BYTE  byRes[8];
}QMAPI_NET_SINGLE_HDGROUP, *LPQMAPI_NET_SINGLE_HDGROUP;

/*本地盘组信息配置 第一版本不支持*/
typedef struct
{
    DWORD  dwSize;
    DWORD  dwHDGroupCount;
    QMAPI_NET_SINGLE_HDGROUP  stHDGroupAttr[QMAPI_MAX_HD_GROUP];    
}QMAPI_NET_HDGROUP_CFG, *LPQMAPI_NET_HDGROUP_CFG;


/*系统时间*/
typedef struct
{
    DWORD  dwYear;
    DWORD  dwMonth;
    DWORD  dwDay;
    DWORD  dwHour;
    DWORD  dwMinute;
    DWORD  dwSecond;
}QMAPI_NET_TIME, *LPQMAPI_NET_TIME;

/*时区和夏时制参数*/
typedef struct 
{
    DWORD dwMonth;  //月 0-11表示1-12个月
    DWORD dwWeekNo;  //第几周：0－第1周；1－第2周；2－第3周； 3－第4周；4－最后一周
    DWORD dwWeekDate;  //星期几：0－星期日；1－星期一；2－星期二；3－星期三；4－星期四；5－星期五；6－星期六
    DWORD dwHour; //小时，开始时间取值范围0－23； 结束时间取值范围1－23
    DWORD dwMin; //分0－59
}QMAPI_NET_TIMEPOINT;

typedef enum __QMAPI_TIME_ZONE_E
{
    QMAPI_GMT_N12 = 0, //(GMT-12:00) International Date Line West
    QMAPI_GMT_N11, //(GMT-11:00) Midway Island,Samoa
    QMAPI_GMT_N10, //(GMT-10:00) Hawaii
    QMAPI_GMT_N09, //(GMT-09:00) Alaska
    QMAPI_GMT_N08, //(GMT-08:00) Pacific Time (US&amp;Canada)
    QMAPI_GMT_N07, //(GMT-07:00) Mountain Time (US&amp;Canada)
    QMAPI_GMT_N06, //(GMT-06:00) Central Time (US&amp;Canada)
    QMAPI_GMT_N05, //(GMT-05:00) Eastern Time(US&amp;Canada)
    QMAPI_GMT_N0430, //(GMT-04:30) Caracas
    QMAPI_GMT_N04, //(GMT-04:00) Atlantic Time (Canada)
    QMAPI_GMT_N0330, //(GMT-03:30) Newfoundland
    QMAPI_GMT_N03, //(GMT-03:00) Georgetown, Brasilia
    QMAPI_GMT_N02, //(GMT-02:00) Mid-Atlantic
    QMAPI_GMT_N01, //(GMT-01:00) Cape Verde Islands, Azores
    QMAPI_GMT_00, //(GMT+00:00) Dublin, Edinburgh, London
    QMAPI_GMT_01, //(GMT+01:00) Amsterdam, Berlin, Rome, Paris
    QMAPI_GMT_02, //(GMT+02:00) Athens, Jerusalem, Istanbul
    QMAPI_GMT_03, //(GMT+03:00) Baghdad, Kuwait, Moscow
    QMAPI_GMT_0330, //(GMT+03:30) Tehran
    QMAPI_GMT_04, //(GMT+04:00) Caucasus Standard Time
    QMAPI_GMT_0430, //(GMT+04:30) Kabul
    QMAPI_GMT_05, //(GMT+05:00) Islamabad, Karachi, Tashkent
    QMAPI_GMT_0530, //(GMT+05:30) Madras, Bombay, New Delhi
    QMAPI_GMT_0545, //(GMT+05:45) Kathmandu
    QMAPI_GMT_06, //(GMT+06:00) Almaty, Novosibirsk, Dhaka
    QMAPI_GMT_0630, //(GMT+06:30) Yangon
    QMAPI_GMT_07, //(GMT+07:00) Bangkok, Hanoi, Jakarta
    QMAPI_GMT_08, //(GMT+08:00) Beijing, Urumqi, Singapore
    QMAPI_GMT_09, //(GMT+09:00) Seoul, Tokyo, Osaka, Sapporo
    QMAPI_GMT_0930, //(GMT+09:30) Adelaide, Darwin
    QMAPI_GMT_10, //(GMT+10:00) Melbourne, Sydney, Canberra
    QMAPI_GMT_11, //(GMT+11:00) Magadan, Solomon Islands
    QMAPI_GMT_12, //(GMT+12:00) Auckland, Wellington, Fiji
    QMAPI_GMT_13, //(GMT+13:00) Nuku'alofa
}QMAPI_TIME_ZONE_E;


typedef struct 
{
    DWORD				dwSize;			//本结构大小
    int					nTimeZone;		//时区
    BYTE				byRes1[12];		//保留
    DWORD				dwEnableDST;	//是否启用夏时制 0－不启用 1－启用
    BYTE				byDSTBias;		//夏令时偏移值，30min, 60min, 90min, 120min, 以分钟计，传递原始数值
    BYTE				byRes2[3];		//保留
    QMAPI_NET_TIMEPOINT	stBeginPoint;	//夏时制开始时间
    QMAPI_NET_TIMEPOINT	stEndPoint;		//夏时制停止时间
}QMAPI_NET_ZONEANDDST, *LPQMAPI_NET_ZONEANDDST;

typedef struct
{
    DWORD			dwSize;				//本结构大小
    BYTE			csNTPServer[64];	//NTP服务器域名或者IP地址
    WORD			wInterval;			//校时间隔时间（以小时为单位）
    BYTE			byEnableNTP;		//NTP校时是否启用：0－否，1－是
    signed char		cTimeDifferenceH;	//与国际标准时间的时差（小时），-12 ... +13
    signed char		cTimeDifferenceM;	//与国际标准时间的时差（分钟），0, 30, 45
    BYTE			res1;
    WORD			wNtpPort;			//NTP服务器端口，设备默认为123
    BYTE			res2[8];
}QMAPI_NET_NTP_CFG,*LQMAPI_NET_NTP_CFG;


typedef enum __RESTORE_MASK_E
{
    NORMAL_CFG = 0x00000001,       //设备基本参数
    VENCODER_CFG = 0x00000002,     //视频编码参数
    RECORD_CFG = 0x00000004,           //录像参数
    RS232_CFG = 0x00000008,            //RS232参数
    NETWORK_CFG = 0x00000010,          //网络参数
    ALARM_CFG = 0x00000020,          //报警输入/输出参数
    VALARM_CFG = 0x00000040,           //视频报警检测参数
    PTZ_CFG = 0x00000080,              //云台参数
    PREVIEW_CFG = 0x00000100,          //本地输出参数
    USER_CFG = 0x00000200,            //用户参数
}RESTORE_MASK_E;

typedef struct  
{
    DWORD   dwSize;           //本结构大小
    DWORD   dwMask;           //掩码，按位有效
    BYTE        byRes[16];
}QMAPI_NET_RESTORECFG, *LPQMAPI_NET_RESTORECFG;

/* 
2013-04-10 david: 
修改: 增加当前支持的码流个数byStreamCount 成员.
说明: 结构体大小不变

*/
typedef struct tagQMAPI_NET_SUPPORT_STREAM_FMT
{
    DWORD       dwSize;                     //struct size
    DWORD           dwChannel;
    DWORD       dwVideoSupportFmt[QMAPI_MAX_STREAMNUM][4];        // Video Format.
    QMAPI_RANGE   stVideoBitRate[QMAPI_MAX_STREAMNUM];
    QMAPI_SIZE    stVideoSize[QMAPI_MAX_STREAMNUM][10];     // Video Size(height,width)
    DWORD       dwAudioFmt[4];              //Audio Format
    DWORD       dwAudioSampleRate[4];       //Audio Sample Rate
    BOOL        bSupportAudioAEC;           //b Support Audio Echo Cancellation
    BYTE        byStreamCount;              //max is QMAPI_MAX_STREAMNUM
    BYTE   		stMAXFrameRate[QMAPI_MAX_STREAMNUM];
    BYTE   		stMINFrameRate[QMAPI_MAX_STREAMNUM];
	BYTE		dwEncodeCapability;
	BYTE            byReserve[25];
}QMAPI_NET_SUPPORT_STREAM_FMT;

//Save Flag
#define QMAPI_COLOR_NO_SAVE               0
#define QMAPI_COLOR_SAVE                  1
#define QMAPI_COLOR_SET_DEF               2
typedef struct tagQMAPI_NET_CHANNEL_COLOR_SINGLE
{
    DWORD       dwSize;                     //struct size
    DWORD           dwChannel;
    DWORD       dwSetFlag;
    DWORD       dwSaveFlag;
    int             nValue;             
    int             nParam;             
}QMAPI_NET_CHANNEL_COLOR_SINGLE;

//通道颜色设置
typedef struct tagQMAPI_NET_CHANNEL_COLOR
{
    DWORD   dwSize;
    DWORD   dwChannel;
    DWORD   dwSetFlag;      //0,设置但不保存;1,保存参数;2,恢复上一次保存的
    int     nHue;           //色调 0-255
    int     nSaturation;        //饱和度 0-255
    int     nContrast;      //对比度 0-255
    int     nBrightness;    //亮度 0-255
    int     nDefinition;    //清晰度 0-255
}QMAPI_NET_CHANNEL_COLOR, *PQMAPI_NET_CHANNEL_COLOR;

typedef struct 
{
    QMAPI_NET_CHANNEL_OSDINFO         stOsdInfo[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_PIC_INFO        stPicInfo[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_COLOR           stColor[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_SHELTER         stShelter[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_MOTION_DETECT           stMotionDetect[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_VILOST                  stVLost[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_HIDEALARM               stHideAlarm[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_RECORD                  stRecord[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_ENCRYPT         stEncrypt;         /**< 码流加密特性 */
    QMAPI_NET_CHANNEL_WATERMARK       stWaterMark;     /**< 水印 */
}QMAPI_NET_CHANNEL_CFG, *LPQMAPI_NET_CHANNEL_CFG;

typedef enum 
{   
    QMAPI_SHUTTER_AUTO=0, 
    QMAPI_SHUTTER_1_200000,       // 1/200000s
    QMAPI_SHUTTER_1_100000,       // 1/100000s
    QMAPI_SHUTTER_1_50000,        // 1/50000s
    QMAPI_SHUTTER_1_20000,        // 1/20000s
    QMAPI_SHUTTER_1_10000,        // 1/10000s
    QMAPI_SHUTTER_1_5000,     // 1/5000s
    QMAPI_SHUTTER_1_2000,     // 1/2000s
    QMAPI_SHUTTER_1_1000,     // 1/1000s
    QMAPI_SHUTTER_1_500,      // 1/500s
    QMAPI_SHUTTER_1_250,      // 1/250s
    QMAPI_SHUTTER_1_200,      // 1/200s
    QMAPI_SHUTTER_1_175,      // 1/175s
    QMAPI_SHUTTER_1_150,      // 1/150s
    QMAPI_SHUTTER_1_125,      // 1/125s
    QMAPI_SHUTTER_1_100,      // 1/100s || 1/120s
    QMAPI_SHUTTER_1_50,           // 1/50s   || 1/60s
    QMAPI_SHUTTER_1_25,           // 1/25s   || 1/30s
    QMAPI_SHUTTER_1_20,           // 1/20s   || 1/20s
    QMAPI_SHUTTER_1_15,           // 1/17s   || 1/15s
    QMAPI_SHUTTER_1_13,           // 1/13s
    QMAPI_SHUTTER_1_10,           // 1/10s
    QMAPI_SHUTTER_1_5,            // 1/5s
    QMAPI_SHUTTER_Max
} QMAPI_SHUTTER_MODE_E;


//Color QMAPI_RANGE
typedef struct tagQMAPI_NET_COLOR_SUPPORT
{
    DWORD           dwSize;                     //struct size
    DWORD           dwMask;              //按位有效(QMAPI_COLOR_SET_BRIGHTNESS)
    
    QMAPI_RANGE       strBrightness;          
    QMAPI_RANGE       strHue;                 
    QMAPI_RANGE       strSaturation;          
    QMAPI_RANGE       strContrast;                
    QMAPI_RANGE       strDefinition;
    BYTE                byReseved[32];
}QMAPI_NET_COLOR_SUPPORT;

typedef struct tagQMAPI_NET_ENHANCED_COLOR_SUPPORT
{
    DWORD           dwSize;                     //struct size
    DWORD           dwMask;              //按位有效(QMAPI_COLOR_SET_BRIGHTNESS)

    QMAPI_RANGE       strDenoise; //降噪
    QMAPI_RANGE       strIrisBasic;
    QMAPI_RANGE       strRed;
    QMAPI_RANGE       strBlue;
    QMAPI_RANGE       strGreen;
    QMAPI_RANGE       strGamma;

    QMAPI_RANGE       strEC;
    QMAPI_RANGE       strGC;
    QMAPI_RANGE       strWD;
    BYTE                byReseved[32];
}QMAPI_NET_ENHANCED_COLOR_SUPPORT;

typedef enum
{
    DN_MODE_AUTO = 0,
    DN_MODE_NIGHT,
    DN_MODE_DAY,
    DN_MODE_BUTT
}QMAPI_DN_MODE_E;

//通道颜色设置
typedef struct tagQMAPI_NET_CHANNEL_ENHANCED_COLOR
{
    DWORD   dwSize;
    DWORD   dwChannel;
    DWORD   dwSetFlag;              //0,设置但不保存;1,保存参数;2,恢复上一次保存的

    BOOL    bEnableAutoDenoise;     //自动降噪使能
    int     nDenoise;               //降噪阀值

    BOOL    bEnableAWB;             //自动白平衡
    int     nRed;
    int     nBlue;
    int     nGreen; 

    BOOL    bEnableAEC;             //自动曝光(电子快门)使能
    int     nEC;                    //自动曝光(电子快门)阀值

    BOOL    bEnableAGC;             //自动增益/手动增益
    int     GC;                     //自动增益/手动增益阀值

    int     nMirror;                //0:不镜像, 1: 上下镜像, 2: 左右镜像  3: 上下左右镜像
    BOOL    bEnableBAW;             //手动彩转黑使能0,自动1,夜视,2 白天

    int     nIrisBasic;             //锐度
    int     nGamma;
    BOOL    bWideDynamic;           //宽动态使能
    int     nWDLevel;               //宽动态等级

    int     nSceneMode;             //场景模式
                                    //0x00    /*scene，outdoor*/
                                    //0x01    /*scene，indoor*/
                                    //0x02    /*scene，manual*/
                                    //0x03    /*scene，auto*/
    BOOL    bEnableAIris;           //自动光圈
    BOOL    bEnableBLC;             //背光控制使能

    char	nRotate;				//0 none ,1 90度 ,2 180度 ,3 270度

    BYTE    byReseved[27];
}QMAPI_NET_CHANNEL_ENHANCED_COLOR, *PQMAPI_NET_CHANNEL_ENHANCED_COLOR;


//////////////////////////////////////////////////////////////////////////
//（2）网络通信结构体

//传感器输出控制
typedef struct tagQMAPI_NET_ALARMOUT_CONTROL
{
    DWORD   dwSize;
    int     nChannel;
    WORD    wAction;     //0:停止输出 1:开始输出
    WORD    wDuration;   //持续时间以秒为单位，0xFFFFFFFF表示不停止，0表示立即停止
}QMAPI_NET_ALARMOUT_CONTROL;

//云台控制
typedef struct tagQMAPI_NET_PTZ_CONTROL
{
    DWORD   dwSize;
    int     nChannel;		//通道号
    DWORD  	dwCommand;		//云台控制命令字QMAPI_PTZ_CMD_UP
    DWORD  	dwParam;		//云台控制参数(巡航编号)
    BYTE    byRes[64];		//预置点名称  (设置巡航点的时候为QMAPI_NET_CRUISE_POINT参数)
}QMAPI_NET_PTZ_CONTROL;

typedef struct
{
	BYTE 	byPointIndex;	//巡航组中的下标,如果值大于QMAPI_MAX_CRUISE_POINT_NUM表示添加到末尾
	BYTE 	byPresetNo;	//预置点编号
	BYTE 	byRemainTime;	//预置点滞留时间
	BYTE 	bySpeed;		//到预置点速度
}QMAPI_NET_CRUISE_POINT;

typedef struct
{
	BYTE byPointNum; 		//预置点数量
	BYTE byCruiseIndex;	//本巡航的序号
	BYTE byRes[2];
	QMAPI_NET_CRUISE_POINT struCruisePoint[QMAPI_MAX_CRUISE_POINT_NUM];
}QMAPI_NET_CRUISE_INFO;

typedef struct
{
	DWORD dwSize;
	int     nChannel;
	BYTE     byIsCruising;		//是否在巡航
	BYTE     byCruisingIndex;	//正在巡航的巡航编号
	BYTE     byPointIndex;		//正在巡航的预置点序号(数组下标)
	BYTE     byEnableCruise;;	//是否开启巡航
	QMAPI_NET_CRUISE_INFO struCruise[QMAPI_MAX_CRUISE_GROUP_NUM];
}QMAPI_NET_CRUISE_CFG;

typedef struct tagQMAPI_NET_PRESET_INFO
{
    DWORD 		dwSize;
    unsigned short   nChannel;
    unsigned short   nPresetNum;                   //预置点个数
    unsigned int   no[QMAPI_MAX_PRESET];        //预置点序号
    char           csName[QMAPI_MAX_PRESET][64];//预置点名称
}QMAPI_NET_PRESET_INFO;

#define REC_ACTION_STOP   0
#define REC_ACTION_START  1
//录像控制
typedef struct tagQMAPI_NET_REC_CONTROL
{
    DWORD   dwSize;
    int     nChannel;
    int     nRecordType; //录像类型，参见QMAPI_NET_RECORD_TYPE_
    WORD        wAction;     //动作：0 停止录像，1 开始录像
    WORD     wDuration;  //0xFFFFFFFF表示不停止，直到发送再次停止命令。
}QMAPI_NET_REC_CONTROL;

//传感器状态获取
typedef struct tagQMAPI_NET_SENSOR_STATE
{
    DWORD   dwSize;
    //0xfffffff标识所有报警输入输出通道，按位有效，
    //从低到高前16位表示报警输入通道号位索引，后16位表示报警输出通道号索引
    DWORD   dwSensorID; 
    //0xfffffff标识所有报警输入输出通道状态，按位有效，
    //从低到高前16位表示报警输入通道状态位，后16位表示报警输出通道状态索引
    DWORD   dwSensorOut;
}QMAPI_NET_SENSOR_STATE;

typedef struct tagQMAPI_NET_VIDEO_STATE
{
    DWORD   dwSize;
    int     nChannelNum;
    int     nState[QMAPI_MAX_CHANNUM];
}QMAPI_NET_VIDEO_STATE;


//报警上传中心
typedef struct tagQMAPI_NET_ALARM_NOTIFY
{
    DWORD   dwSize;
    DWORD   dwAlarmType;        //报警类型（详见QMAPI_ALARM_TYPE_E定义）
    DWORD   dwAlarmInput;       //报警通道号，报警类型是1/4/5时，按位有效，每位表示硬盘号
    BYTE    byRes[32];
}QMAPI_NET_ALARM_NOTIFY;

//串口透明传输
typedef struct tagQMAPI_NET_SERIAL_DATA
{
    DWORD   dwSize;
    int         nPort;  //1-232, 2-485
    DWORD  dwBufSize;     //数据长度
    char    *pDataBuffer;     //数据
}QMAPI_NET_SERIAL_DATA;

//上传抓拍图片
typedef struct tagQMAPI_NET_SNAP_DATA
{
    DWORD   dwSize;
    int          nChannel;
    int          nLevel;          // 0-5 :0 最高
    DWORD  dwFileType;    // 0:jpg图片，1:bmp图片
    DWORD  dwBufSize;     // 数据长度
    char    *pDataBuffer;     //数据
}QMAPI_NET_SNAP_DATA;
typedef struct tagQMAPI_NET_SNAP_DATA_V2
{
    DWORD   dwSize;
    int     nChannel;
    int     nLevel;       // 0-5 :0 最高
    DWORD   dwFileType;   // 0:jpg图片，1:bmp图片
    DWORD   dwBufSize;    // 数据长度
    char   *pDataBuffer;  //数据
    int     nWidth;
    int     nHeight;
}QMAPI_NET_SNAP_DATA_V2;

typedef struct tagQMAPI_NET_VIDEOCALLBACK
{
    DWORD       dwSize;
    int                 nChannel;//通道号
    int                 nStreamType;//码流类型，0,主码流，1,子码流
    void            *pCallBack; //回调函数指针
    DWORD       dwUserData; // 用户数据
}QMAPI_NET_VIDEOCALLBACK;


//用户登陆
typedef struct tagQMAPI_NET_USER_LOGON
{
    DWORD   dwSize;
    char        csUserName[QMAPI_NAME_LEN];
    char        csPassword[QMAPI_PASSWD_LEN];
    char        csUserIP[QMAPI_MAX_IP_LENGTH]; /*用户IP地址(为0时表示允许任何地址) */
    BYTE    byMACAddr[QMAPI_MACADDR_LEN]; /* 物理地址 */
}QMAPI_NET_USER_LOGON;

//
#define QMAPI_UPGRADE_FILE_FLAG               0x38291231
#define QMAPI_UPGRADE_FILE_FLAG2             0x38291232
#define QMAPI_UPGRADE_FILE_FLAG3             0x6851e685

typedef enum
{
	QMAPI_ROM_FILETYPE_ARMBOOT = 0,
	QMAPI_ROM_FILETYPE_KERNEL =  1,
	QMAPI_ROM_FILETYPE_ROOTFS = 2,
	QMAPI_ROM_FILETYPE_USERDATA =  3,
	QMAPI_ROM_FILETYPE_SETTING = 4,
	QMAPI_ROM_FILETYPE_APP = 5,
	QMAPI_ROM_FILETYPE_WEB  =  6,
	QMAPI_ROM_FILETYPE_LOGO = 7,
	QMAPI_ROM_FILETYPE_FACTORY_PARA =8,
	QMAPI_ROM_FILETYPE_BUTT,
}QMAPI_ROM_FILETYPE_E;

//升级文件数据头信息
typedef struct tagUPGRADE_ITEM_HEADER
{
    DWORD   dwSize;
    DWORD   dwDataType;    //数据区类型
    DWORD   dwDataLen;     //数据长度
    DWORD   dwCRC;         //数据CRC校验
    DWORD   dwDataPos;     //数据位置
    DWORD   dwVersion;
}UPGRADE_ITEM_HEADER;

//升级文件头信息
typedef struct tagQMAPI_UPGRADE_FILE_HEADER
{
    DWORD   dwFlag;         //升级文件类型(QMAPI_UPGRADE_FILE_FLAG)
    DWORD   dwSize;         //本结构体大小
    DWORD   dwVersion;      //文件版本
    DWORD   dwItemCount;    //包内包含的文件总数
    DWORD   dwPackVer;      //打包的版本
    DWORD   dwCPUType;      //CPU类型
    DWORD   dwChannel;      //服务器通道数量0x01 | 0x04 | 0x08
    DWORD   dwServerType;   //服务器类型
    char        csDescrip[64];      //描述信息
    BYTE    byReserved[32]; 
}QMAPI_UPGRADE_FILE_HEADER, *LPQMAPI_UPGRADE_FILE_HEADER;

//升级程序请求
typedef struct tagQMAPI_NET_UPGRADE_REQ
{
    QMAPI_UPGRADE_FILE_HEADER  FileHdr;
    UPGRADE_ITEM_HEADER         stItemHdr[10];
}QMAPI_NET_UPGRADE_REQ;

//升级程序应答
typedef struct tagQMAPI_NET_UPGRADE_RESP
{
    DWORD   dwSize;
    int     nResult;        //升级应答结果
    char 	state;			//升级状态0 普通状态1 开始升级2 升级中3升级完成
    char 	progress;		//升级进度0 - 100%
    char        reserved[30];
}QMAPI_NET_UPGRADE_RESP;

//升级程序
typedef struct tagQMAPI_NET_UPGRADE_DATA
{
    DWORD   dwSize;
    DWORD   dwFileLength;       //升级包总文件长度
    DWORD   dwPackNo;           //分包序号，从0开始
    DWORD   dwPackSize;         //分包大小
    BOOL    bEndFile;           //是否是最后一个分包
    char        reserved[32];
    BYTE    *pData;
}QMAPI_NET_UPGRADE_DATA;

///Video New Protocol Frame Header
typedef struct QMAPI_NET_FRAME_HEADER
{
    DWORD  dwSize;
    DWORD   dwFrameIndex;
    DWORD   dwVideoSize;
    DWORD   dwTimeTick;
    WORD    wAudioSize;
    BYTE    byFrameType; //0：I帧；1：P帧；2：B帧，只针对视频
    BYTE    byVideoCode;
    BYTE    byAudioCode;
    BYTE    byReserved1;// 按4位对齐
    BYTE    byReserved2; // 按4位对齐
    BYTE    byReserved3; // 按4位对齐
    WORD    wVideoWidth;
    WORD    wVideoHeight;
}QMAPI_NET_FRAME_HEADER;

///Video New Protocol DATAPACK
typedef struct tagQMAPI_NET_DATA_PACKET
{
    QMAPI_NET_FRAME_HEADER    stFrameHeader;
    DWORD                   dwPacketNO;         //包序号, 0 开始
    DWORD                   dwPacketSize;   //包大小
    unsigned char               *pData;
}QMAPI_NET_DATA_PACKET,*PQMAPI_NET_DATA_PACKET;

typedef  struct
{
    DWORD            dwSize;
    DWORD            dwChannel;     // 0xff 代表所有通道
    DWORD            dwFileType; // 取值范围 QMAPI_NET_RECORD_TYPE_
    DWORD            dwIsLocked;// 是否锁定：0-未锁定文件，1-锁定文件，0xff表示所有文件（包括锁定和未锁定）
    QMAPI_TIME       stStartTime;
    QMAPI_TIME       stStopTime;
}QMAPI_NET_FILECOND,*LPQMAPI_NET_FILECOND;

typedef  struct
{
    DWORD          dwSize;
    char               csFileName[48];
    DWORD          dwFileType; // 取值范围 QMAPI_NET_RECORD_TYPE_
    QMAPI_TIME     stStartTime;
    QMAPI_TIME     stStopTime;
    DWORD          dwFileSize;
    BYTE              byLocked;// 是否锁定：0-未锁定文件，1-锁定文件，0xff表示所有文件（包括锁定和未锁定）
    BYTE              nChannel;	//0731添加
    BYTE              byRes[2];
}QMAPI_NET_FINDDATA,*LPQMAPI_NET_FINDDATA;

typedef  struct _qmapi_net_finddata_list
{
    QMAPI_NET_FINDDATA data;
	struct _qmapi_net_finddata_list *pPre;
	struct _qmapi_net_finddata_list *pNext;
}QMAPI_NET_FINDDATA_LIST,*LPQMAPI_NET_FINDDATA_LIST;


typedef struct
{
    char szBuf[QMAPI_FILES_SIZE];
    int TotalNum;
    int CurNum;
    int StartIndex;
    int EndIndex;
		/*
			文件回放的开始时间, 目前只在数据推送模式下使用 表示最开始时间点播放
		*/
	QMAPI_TIME StartTime; 
}QMAPI_FILES_INFO, *LPQMAPI_FILES_INFO;

//回放数据回调注册结构体
typedef struct tagQMAPI_NET_PLAYDATACALLBACK
{
    DWORD       dwSize;
    int             PlayHandle;// 回放句柄
    DWORD       dwUser;//用户数据，保留，数据回调时赋值返回
    void            *pCallback; //回调函数指针
}QMAPI_NET_PLAYDATACALLBACK;


//音频对讲请求结构体
typedef struct tagQMAPI_NET_AUDIOTALK
{
    DWORD                   dwSize;
    QMAPI_PAYLOAD_TYPE_E      enFormatTag;// 音频编码类型(QMAPI_PT_)
    int                                 nNumPerFrm; //每次采集音频数据字节长度
    DWORD                   dwUser;//用户数据，保留，数据回调时赋值返回
}QMAPI_NET_AUDIOTALK;
//david add 2012-05-11---获取SPSPPS base64编码后的数据
typedef struct tagQMAPI_NET_SPSPPS_BASE64ENCODE_DATA
{
    DWORD dwSize;
    WORD  chn;
    WORD  streamtype;
    int SPS_LEN;
    int PPS_LEN;
    int SEL_LEN;
    int SPS_PPS_BASE64_LEN;
//  unsigned char SPS_VALUE[64];
    unsigned char SPS_VALUE[128]; //david 2013-03-19:为了支持TI 方案编码出来的数据长度
    unsigned char PPS_VALUE[64];
//  unsigned char SEL_VALUE[64];

    unsigned char SEI_VALUE[64];
    char SPS_PPS_BASE64[256];
    unsigned char profile_level_id[8];
}QMAPI_NET_SPSPPS_BASE64ENCODE_DATA;

/**获取读指针方式*/
typedef enum tagQMAPI_MBUF_POS_POLICY_E
{
    QMAPI_MBUF_POS_CUR_READ = 0,/**<当前读指针的位置*/
    QMAPI_MBUF_POS_LAST_READ_nIFRAME,/**<离当前读指针最近的第n个I帧所在的块的起始位置*/
    QMAPI_MBUF_POS_CUR_WRITE,/**<当前写指针的位置*/
    QMAPI_MBUF_POS_LAST_WRITE_nBLOCK,/**<离当前写指针最近的第n个块的起始位置*/
    QMAPI_MBUF_POS_LAST_WRITE_nIFRAME,/**<离当前写指针最近的第n个I帧所在的块的起始位置, n=0表示最近的I帧*/
    QMAPI_MBUF_POS_BUTT
}QMAPI_MBUF_POS_POLICY_E;
/*录像文件的信息--初步定义以后慢慢添加其它信息*/
typedef struct tagQMAPI_RECFILE_INFO
{
    UINT16  u16Width;           //视频帧宽
    UINT16  u16Height;          //视频帧高
    UINT16  u16FrameRate;       //视频流帧率
    UINT16  u16VideoTag;        //视频流标识   QMAPI_PT_H264
    DWORD   u32TotalTime;		  //视频总时长
    UINT16  u16HaveAudio;       //是否有音频流
    UINT16  u16AudioTag;        //音频流标识   QMAPI_PT_G711A
    UINT16  u16AudioBitWidth;   //音频采样
    UINT16  u16AudioSampleRate; //
    DWORD   u32TotalSize;       // 文件大小
}QMAPI_RECFILE_INFO;

//RTSP设置
typedef struct tagQMAPI_NET_RTSP_CFG
{
	DWORD		dwSize;
	DWORD		dwPort;			//RTSP端口
	int			nAuthenticate;	//0 不认证 1 basic64 2 digest 
	BYTE		byReserve[28];
}QMAPI_NET_RTSP_CFG,*PQMAPI_NET_RTSP_CFG;

//
typedef struct tagQMAPI_NET_SPRCIAL_OSD
{
    int nIndex;         //OSD 索引
    BOOL bShow;         //FALSE,隐藏
    int nStartX;        //起始x坐标
    int nStartY;        //起始y坐标
    char csText[64];    //需要显示的文本
    int  nLen;          //文本的长度
}QMAPI_NET_SPECIAL_OSD,*PQMAPI_NET_SPECIAL_OSD;

typedef struct tagQMAPI_NET_UPDATE_FLASH
{
    DWORD dwSize;
    char *pBuff;
    DWORD dwBuffSize;
}QMAPI_NET_UPDATE_FLASH;

typedef struct tagQMAPI_NET_P2P_CFG
{
    DWORD dwSize;
    DWORD dwEnable;
    char  csID[64];
    char  csRes[64];
}QMAPI_NET_P2P_CFG;

typedef struct tagQMAPI_NET_COM_DATA
{
    DWORD       dwSize;
    DWORD       dwChannel;
    BOOL        bIs485;         //485 or 232
    char            byData[128];        //
}QMAPI_NET_COM_DATA;

typedef enum __QMAPI_AUTO_TEST_TYPE_E
{
    CMD_TEST_PTZ         = 0x00000001,
    CMD_TEST_ALARM_IN  = 0x00000002,
    CMD_TEST_ALARM_OUT = 0x00000003,
    CMD_TEST_LED         = 0x00000005,
    CMD_TEST_DISK      = 0x00000006,
    CMD_TEST_CLOCK     = 0x00000007,
    CMD_TEST_REBOOT    = 0x0000000A,
    CMD_TEST_DEFAULT   = 0x0000000B,
    CMD_TEST_AUDIO     = 0x0000000C,
	CMD_TEST_WIFI		= 0x0000000D,
	CMD_TEST_3G		= 0x0000000E,   
	CMD_TEST_RUN_STATUS = 0x0000000F, //最近4条的运行时间
	CMD_TEST_WRITE_MACADDR = 0x00000010, 
	CMD_TEST_WRITE_DOMAIN = 0x00000011	
}QMAPI_AUTO_TEST_TYPE_E;
typedef struct tagQMAPI_NET_AUTO_TEST
{
	DWORD	dwSize;
	DWORD	dwCommand;
	DWORD	dwChannel;
	DWORD	dwErrorCode;
	DWORD	dwBufSize;
    char szPrivateBuffer[256];
}QMAPI_NET_AUTO_TEST;

typedef struct tagQMAPI_NET_LOG_INFO
{
    QMAPI_TIME   stSysStartTime;
    QMAPI_TIME   stSysRunTime;
    char       szReserve[16];
}QMAPI_NET_LOG_INFO;

typedef enum __QMAPI_DN_MODE_E
{
    QMAPI_DN_DT_MODE_AUTO = 0,
    QMAPI_DN_DT_MODE_IR,
    QMAPI_DN_DT_MODE_VIDEO,
    QMAPI_DN_DT_MODE_TIME,
    QMAPI_DN_DT_MODE_BUTT
}QMAPI_DN_DT_MODE_E;
typedef struct tagQMAPI_NET_DAY_NIGHT_DETECTION_EX
{
    DWORD dwSize;
    DWORD dwChannel;
    BYTE byMode;      // 0：自动检测，1：光敏电阻检测，2：视频检测，3：时间检测
    //光敏电阻检测
    BYTE byTrigger;    // 光敏电阻检测-0：低电平有效，1：高电平有效
    //视频检测
    BYTE byAGCSensitivity;// 灵敏度,0-5
    BYTE byDelay;      // 切换延时时间0-10s。
	
    BYTE byIRCutLevel;// IRCUT控制-0：低电平有效，1：高电平有效
    BYTE byLedLevel; // 红外灯控制: 0:低电平 1:高电平
    BYTE reserve[2];
    //时间检测
    QMAPI_SCHEDTIME stColorTime; //彩色的时间段，不在此时间段内认为是黑白。
}QMAPI_NET_DAY_NIGHT_DETECTION_EX;

typedef struct tagQMAPI_NET_USER_KEY
{
    DWORD dwSize;
    int   nLen;
    BYTE  byData[128];
}QMAPI_NET_USER_KEY;


typedef enum __QMAPILogMainType
{
	QMAPI_LOG_TYPE_ALL = 1,         /* 所有日志类型 */
	QMAPI_LOG_TYPE_OPER,            /* 操作类型 */
	QMAPI_LOG_TYPE_ALARM,           /* 告警类型 */
	QMAPI_LOG_TYPE_SMART,           /* 智能分析类型 */
	QMAPI_LOG_TYPE_ACCESS,	        /* 访问类型 */
	QMAPI_LOG_TYPE_INFO,            /* 提示信息类型 */
	QMAPI_LOG_TYPE_ABNORMAL,        /* 异常类型 */
	QMAPI_LOG_TYPE_BUTT,
}QMAPI_LOG_MAIN_TYPE_E;

typedef enum __QMAPILogOperType
{
	QMAPI_LOG_OPER_ALL = 1,   	    /* 所有操作类型 */
	QMAPI_LOG_OPER_POWERON,		    /* 开机操作 */
	QMAPI_LOG_OPER_POWEROFF,     	/* 关机操作 */
	QMAPI_LOG_OPER_REBOOT,		    /* 重启操作 */
	QMAPI_LOG_OPER_SAVEPARA,        /* 保存参数操作 */
	QMAPI_LOG_OPER_RECMANUAL,  	    /* 手动录像操作 */
	QMAPI_LOG_OPER_PTZCONTROL,	    /* 云台操作操作 */
	QMAPI_LOG_OPER_STARTCRUISE,	    /* 开启巡航操作 */
	QMAPI_LOG_OPER_STOPCRUISE,	    /* 停止巡航操作 */
	QMAPI_LOG_OPER_SNAPMAUNAL,	    /* 手动抓拍操作 */
	QMAPI_LOG_OPER_STARTTALKBACK,	/* 开启对讲操作 */
	QMAPI_LOG_OPER_STOPTALKBACK,	/* 停止对讲操作 */
	QMAPI_LOG_OPER_UPGRADEAPP,	    /* 升级APP操作 */
	QMAPI_LOG_OPER_UPGRADEKERNEL,	/* 升级KERNEL操作 */
	QMAPI_LOG_OPER_UPGRADEWEB,	    /* 升级WEB操作 */
	QMAPI_LOG_OPER_FORMAT,		    /* 格式化磁盘操作 */
	QMAPI_LOG_OPER_UNINSTALLDISK,	/* 卸载磁盘操作 */
 	QMAPI_LOG_OPER_BUTT,
}QMAPI_LOG_OPER_TYPE_E;
	
typedef enum __QMAPILogAlarmType
{
	QMAPI_LOG_ALARM_ALL = 1,		/* 所有报警类型 */
	QMAPI_LOG_ALARM_SENSOR,			/* 传感器报警*/
	QMAPI_LOG_ALARM_MOTION,			/* 移动侦测报警 */
	QMAPI_LOG_ALARM_VLOST,			/* 视频丢失报警 */
	QMAPI_LOG_ALARM_SHELTER,		/* 视频遮挡报警*/
	QMAPI_LOG_ALARM_NET_BROKEN,	    /* 网络断开 */
	QMAPI_LOG_ALARM_IP_CONFLICT, 	/* 网络冲突*/
	QMAPI_LOG_ALARM_BUTT,
}QMAPI_LOG_ALARM_TYPE_E;

typedef enum __QMAPILogSmartType
{
    QMAPI_LOG_SMART_ALL = 1,        /* 所有只能分析类型 */
    QMAPI_LOG_SMART_AUDIO_ABNORMAL, /* 音频异常侦测 */
    QMAPI_LOG_SMART_DEFOCUS,        /* 虚焦侦测 */
    QMAPI_LOG_SMART_SCENE_CHANGE,   /* 场景变换*/
    QMAPI_LOG_SMART_FACE_DETECTION, /* 人脸侦测 */
    QMAPI_LOG_SMART_BORDER_DETECTION,       /* 越界侦测 */
    QMAPI_LOG_SMART_REGIONAL_INVASION,      /* 区域入侵 */
    QMAPI_LOG_SMART_ENTER_REGION,   /* 进入区域 */
    QMAPI_LOG_SMART_LEAVE_REGION,   /* 离开区域 */
    QMAPI_LOG_SMART_LINGER_DETECTION,       /* 徘徊侦测 */
    QMAPI_LOG_SMART_PERSONNEL_GATHERS,      /* 人员聚集 */
    QMAPI_LOG_SMART_FAST_MOVING,            /* 快速移动 */
    QMAPI_LOG_SMART_PARKING_DETECTION,      /* 停车侦测 */
    QMAPI_LOG_SMART_ARTICLES_LEFT,  /* 物品遗留 */
    QMAPI_LOG_SMART_ARTICLES_TAKE,  /* 物品拿取 */
    QMAPI_LOG_SMART_BUTT,
}QMAPI_LOG_SMART_TYPE_E;

typedef enum __QMAPILogAccessType
{
	QMAPI_LOG_ACCESS_ALL = 1,		/* 所有访问类型 */
	QMAPI_LOG_ACCESS_CLIENT,		/* 客户端访问 */
	QMAPI_LOG_ACCESS_MOBILE,		/* 手机访问 */
	QMAPI_LOG_ACCESS_RTSP,		    /* RTSP访问 */
	QMAPI_LOG_ACCESS_ONVIF,		    /* ONVIF访问 */
	QMAPI_LOG_ACCESS_G28181,		/* 国标访问 */
	QMAPI_LOG_ACCESS_ZN,			/* 智诺NVR访问 */
	QMAPI_LOG_ACCESS_BUTT,
}QMAPI_LOG_ACCESS_TYPE_E;

typedef enum __QMAPILogInfoType
{
	QMAPI_LOG_INFO_ALL = 1,		    /* 所有提示信息 */
	QMAPI_LOG_INFO_RECSCH,		    /* 按计划录像 */
	QMAPI_LOG_INFO_RECALARM,		/* 报警联动录像 */
	QMAPI_LOG_INFO_SNAPFTP,		    /* 抓拍FTP上传 */
	QMAPI_LOG_INFO_EMAIL,			/* 报警信息EMAIL上传 */
	QMAPI_LOG_INFO_UPNP_SUCC,		/* UPNP映射成功 */
	QMAPI_LOG_INFO_DDNS_SUCC,		/* DDNS注册成功 */
	QMAPI_LOG_INFO_PPPOE_SUCC,	    /* PPPOE拨号成功 */
	QMAPI_LOG_INFO_3G_SUCC,		    /* 3G拨号成功 */
	QMAPI_LOG_INFO_NTP,			    /* NTP校时成功 */
	QMAPI_LOG_INFO_BUTT,
}QMAPI_LOG_INFO_TYPE_E;

typedef enum __QMAPILogErrorType
{
	QMAPI_LOG_ERROR_ALL = 1,		/* 所有错误信息 */
	QMAPI_LOG_ERROR_MEM,			/* 内存错误 */
	QMAPI_LOG_ERROR_BUTT,
}QMAPI_LOG_ERROR_TYPE_E;

typedef struct __QMAPI_LOG_INDEX
{
	BYTE byUsed;                    //对应QMAPI_LOG_HEADER的currUsedFlag
	BYTE byMainType;                //日志主类型//QMAPI_LOG_MAIN_TYPE_E
	BYTE bySubType;                 //详细类型
	BYTE byChnn;
	
	unsigned long time;             //秒数，发生时间
	unsigned long ip;
	
	union
        {
            BYTE byUserID;
            BYTE byAlarmEnd;
        };
	BYTE byBlockID;                 //从1开始0表示不扩展
	BYTE byBlockNum;                //使用的块个数
	BYTE byRecv[1];                 //1//预留
}QMAPI_LOG_INDEX;


typedef struct _QMAPI_LOG_QUERY_S
{
    DWORD dwSize;
    UINT32 u32SearchMainType;
    UINT32 u32SearchSubType;
    UINT32 u32Num;
    QMAPI_LOG_INDEX logIndex[QMAPI_LOG_INDEX_MAXNUM];//最近的一条在后面
    char logExtInfo[QMAPI_LOG_BLOCK_MAX][QMAPI_LOG_BLOCK_SIZE];
}QMAPI_LOG_QUERY_S;


typedef struct tagQMAPI_NET_DEVICEMAINTAIN
{
    DWORD   dwSize;
    BYTE    byEnable;   //设备维护功能开关
    BYTE    byIndex;	 //0-6 代表星期天至星期六的设备维护时间，7代表每天设备维护时间
    BYTE    byReserve1[2];
    QMAPI_SYSTEMTIME stDeviceMaintain; 
}QMAPI_NET_DEVICEMAINTAIN;


typedef struct tagQMAPI_NET_NOTIFY_USER_INFO
{
    DWORD	dwSize;
    UINT32	u32Type; //提示类型 QMAPI_NOTIFY_TYPE
    char 	szText[32];//提示文本信息
}QMAPI_NET_NOTIFY_USER_INFO;


typedef enum
{
    QMAPI_BITRATE_AUTO,
    QMAPI_BITRATE_SMOOTH, //
    QMAPI_BITRATE_NORMAL, //
    QMAPI_BITRATE_HD,     //high definition
}QMAPI_BITRATE_CONTROL_TYPE;

typedef struct tagQMAPI_SIMPLIFYL_RECORD
{
    BOOL                bEnable;                            /*是否录像 0-否 1-是*/
    QMAPI_RECORDSCHED     stRecordSched[7][MAX_REC_SCHED];    /*录像时间段，星期一到星期天*/
    BYTE                byReserve[8];
}QMAPI_SIMPLIFYL_RECORD, *LPDMS_SIMPLIFY_RECORD; 

/*临时使用*/
typedef struct tagQMAPI_SET_TIME{
	WORD	bHour;
	WORD	bMinute;
}QMAPI_SET_TIME;
/*临时使用*/
typedef struct tagQMAPI_SCHED_TIME
{
	BOOL            bEnable;		//
	QMAPI_SET_TIME	BeginTime1;		//
	QMAPI_SET_TIME	EndTime1;
	QMAPI_SET_TIME	BeginTime2;		//
	QMAPI_SET_TIME	EndTime2;
}QMAPI_SCHED_TIME;

typedef struct tagQMAPI_SIMPLIFY_MOTION
{
    BOOL                bEnable;                //是否进行布防
    DWORD               dwSensitive;            //灵敏度 取值0 - 5, 越小越灵敏*/
    BOOL                bPushAlarm;             //移动侦测消息推送开关
    QMAPI_SCHED_TIME      stSchedTime[8];         /*布防时间*/    /*临时使用，之后调整*/
}QMAPI_SIMPLIFY_MOTION, *LPQMAPI_SIMPLIFY_MOTION;

typedef struct tagQMAPI_CONSUMER_PRIVACY_INFO
{
    DWORD   dwSize;
    BOOL    bSpeakerMute;           //1:mute
    BOOL    bPreviewAudioEnable;    // 是否使能实时查看音频
    BOOL    bMotdectReportEnable;   // 是否使能移动侦测上报
    DWORD   dwSceneMode;            //场景模式，0:outdoor, 1:indoor, 2:manual, 3:auto
    QMAPI_BITRATE_CONTROL_TYPE        enBitrateType;
    QMAPI_SIMPLIFYL_RECORD            stRecordInfo;
    QMAPI_SIMPLIFY_MOTION             stMotionInfo;
    char    byRes[12];
}QMAPI_CONSUMER_PRIVACY_INFO;

typedef struct tagQMAPI_NET_CONSUMER_INFO
{
    DWORD   dwSize;
    BOOL    bPrivacyMode;   //0 public, 1 private
    QMAPI_CONSUMER_PRIVACY_INFO   stPrivacyInfo[2]; //0 public, 1 private
    DWORD   dwN1ListenPort; // 保存N1协议要使用的端口号，出厂默认值是8089
    char    byRes[56];
}QMAPI_NET_CONSUMER_INFO;


typedef struct tagQMAPI_SYSCFG_ONVIFTESTINFO
{
	DWORD	dwSize;
	DWORD	dwDiscoveryMode;		//0 关闭,1开启(默认)
	char	szDeviceScopes1[64];
	char	szDeviceScopes2[64];
	char	szDeviceScopes3[64];
	BOOL	bAuthEnable;
	BYTE	byReserve[124];
}QMAPI_SYSCFG_ONVIFTESTINFO, *LPQMAPI_SYSCFG_ONVIFTESTINFO;


#endif //_QMAPI_NET_NETSDK_H_






