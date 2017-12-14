/******************************************************************************
******************************************************************************/

#ifndef	_TL_NET_COMMON_H_
#define _TL_NET_COMMON_H_
#include "QMAPIType.h"

#define TL_SUCCESS                          0x00000000
#define TLERR_BASE							0x10000000
#define	TLERR_PASSWORD_UNMATCH				(TLERR_BASE|0x03)			
#define	TLERR_TIME_OVER						(TLERR_BASE|0x04)			
#define	TLERR_INVALID_PARAM					(TLERR_BASE|0x05)			
#define	TLERR_MAX_LINK						(TLERR_BASE|0x06)			
#define	TLERR_INVALID_HANDLE				(TLERR_BASE|0x07)			
#define	TLERR_INVALID_COMMAND				(TLERR_BASE|0x08)			
#define	TLERR_SENDCMD_FAILD					(TLERR_BASE|0x09)			
#define	TLERR_GETCONFIG_FAILD				(TLERR_BASE|0x0a)			
#define	TLERR_NO_LOGON						(TLERR_BASE|0x0b)		
#define	TLERR_ALLOC_FAILD					(TLERR_BASE|0x0c)		
#define	TLERR_INVALID_NETADDRESS			(TLERR_BASE|0x0d)			
#define	TLERR_FILE_CRC32					(TLERR_BASE|0x0e)			
#define TLERR_SOFTVER_ERR					(TLERR_BASE|0x10)			
#define	TLERR_CPUTYPE_ERR					(TLERR_BASE|0x11)			
#define	TLERR_ERROR_10054					(TLERR_BASE|0x12)			
#define	TLERR_ERROR_10061					(TLERR_BASE|0x13)			
#define	TLERR_ERROR_10060					(TLERR_BASE|0x14)			
#define	TLERR_ERROR_10065					(TLERR_BASE|0x15)			
#define	TLERR_INITSURFACE					(TLERR_BASE|0x16)			
#define	TLERR_UNSUPPORT						(TLERR_BASE|0x17)			
#define TLERR_TALK_REJECTED					(TLERR_BASE|0x18)			
#define TLERR_TALK_INITAUDIO				(TLERR_BASE|0x19)			
#define TLERR_OPEN_FILE						(TLERR_BASE|0x1A)			
#define TLERR_BIND_PORT						(TLERR_BASE|0x1B)
#define TLERR_ENCRYPT_IC_NO_MATCH			(TLERR_BASE|0x1C)			//加密IC不匹配
#define TLERR_NO_FILE						(TLERR_BASE|0x21)			
#define TLERR_NOMORE_FILE					(TLERR_BASE|0x22)			
#define	TLERR_FILE_FINDING					(TLERR_BASE|0x23)			
#define TLERR_DISK_NOTEXIST					(TLERR_BASE|0x24)		
#define TLERR_FILE_ERROR					(TLERR_BASE|0x25)			
#define TLERR_UNINITOBJ						(TLERR_BASE|0x26)		
#define TLERR_UNKNOW_SERVER					(TLERR_BASE|0x27)		
#define TLERR_CHANNEL_NOT_OPEN				(TLERR_BASE|0x28)			
#define TLERR_INVALID_FILE					(TLERR_BASE|0x29)	
#define TLERR_ENCRYPT_IC_NO_FIND			(TLERR_BASE|0x2A)
#define TLERR_SERVER_LIMITADDRESS			(TLERR_BASE|0x2B)

#ifndef TLNET_FLAG
#define	TLNET_FLAG			9001 //20070414
//#define	TLNET_FLAG			9000
#endif


#define NETCMD_BUF_SIZE			2048
#define PACK_SIZE					(17*1460 -28) //?ճ?1460????????һ????????????????????
#define PACKET_SND_SIZE			1024
#define NETCMD_MAX_SIZE			2048000
#define NETCMD_SNDFILE_SIZE		51200
#if 1
#define NETCMD_TALKBACK_SIZE 672	//ʹ??84?ֽڵ???????
#else
#define NETCMD_TALKBACK_SIZE 1024
#endif

/* 
 * TODO: define all command in same format
 * */

typedef enum 
{
    NETCMD_BASE             = 5000,
    NETCMD_KEEP_ALIVE       = NETCMD_BASE+1,
    NETCMD_CHANNEL_CHECK_OK = NETCMD_BASE+2,
    NETCMD_NOT_USER	        = NETCMD_BASE+3,
    NETCMD_MAX_LINK	        = NETCMD_BASE+4,
    NETCMD_NOT_TLDATA       = NETCMD_BASE+5,
    NETCMD_NOT_LOGON        = NETCMD_BASE+6,
    NETCMD_LOGON_OK	        = NETCMD_BASE+7,
    NETCMD_MULTI_OPEN       = NETCMD_BASE+8,
    NETCMD_PLAY_CLOSE       = NETCMD_BASE+9,
    NETCMD_UPDATE_FILE      = NETCMD_BASE+10,
    NETCMD_READ_FILE_LEN    = NETCMD_BASE+11,
    NETCMD_READ_FILE_DATA   = NETCMD_BASE+12,
    NETCMD_READ_FILE_CLOSE  = NETCMD_BASE+13,
    NETCMD_LOGON_FULL       = NETCMD_BASE+14,			
    //#ifdef NATTCP
    NETCMD_OPEN_REVERSE_CH  = NETCMD_BASE+16,
    NETCMD_OPEN_REVERSE_TB  = NETCMD_BASE+17,
    //#endif
    NETCMD_RECV_ERR         = NETCMD_BASE+18,  
    NETCMD_CHANNEL_INVALID  = NETCMD_BASE+19, 

    NETCMD_NUMBER           = 6000,
    NETCMD_USER_MSG         = NETCMD_NUMBER+1,
} ENUM_NET_COMMAND;

/* 5000 ~ 6000 reserve for ENUM_NET_COMMAND, be careful */
typedef enum
{
    CMD_REBOOT                  = 0x00000001,
    CMD_RESTORE                 = 0x00000002,
    CMD_UPDATEFLASH             = 0x00000003,
    CMD_SNAPSHOT                = 0x00000004,
    CMD_SETSYSTIME              = 0x00000005,
    CMD_SET_OSDINFO             = 0x00000006,
    CMD_SET_SHELTER             = 0x00000007,
    CMD_SET_LOGO                = 0x00000008,
    CMD_SET_STREAM              = 0x00000009,
    CMD_SET_COLOR               = 0x0000000A,
    CMD_SET_MOTION_DETECT       = 0x0000000B,
    CMD_SET_PROBE_ALARM         = 0x0000000C,
    CMD_SET_VIDEO_LOST          = 0x0000000D,
    CMD_SET_COMINFO             = 0x0000000E,		

    CMD_SET_USERINFO            = 0x0000000F,
    CMD_SET_NETWORK             = 0x00000010,
    CMD_UPLOADPTZPROTOCOL       = 0x00000011,
    CMD_SEND_COMDATA            = 0x00000012,
    CMD_SET_FTPUPDATA_PARAM     = 0x00000013,
    CMD_CLEARALARMOUT           = 0x00000014,		
    CMD_SET_COMINFO2            = 0x00000015,
    CMD_SET_ALARM_OUT           = 0x00000016,

    CMD_SET_CENTER_INFO         = 0x00000023,		
    CMD_UPDATE_CENTER_LICENCE   = 0x00000024,

    CMD_GET_CENTER_INFO         = 0x10000014,

    CMD_GET_SERVER_STATUS       = 0x11000001,

    CMD_SET_CENTER_INFO_EX      = 0x00000064,
    CMD_GET_CENTER_INFO_EX      = 0x00000065,

    CMD_GET_TL_I2C_CONFIG       = 0x11001132,
    CMD_SET_TL_I2C_CONFIG       = 0x11001133,

    CMD_SET_NOTIFY_SERVER       = 0x00000017,
    CMD_SET_PPPOE_DDNS          = 0x00000018,

    CMD_SET_EMAIL_PARAM         = 0x00000026,
    CMD_GET_EMAIL_PARAM         = 0x10000017,

    CMD_SET_COMMODE             = 0x00000027,
    CMD_GET_COMMODE             = 0x10000018,

    CMD_SET_SERVER_RECORD       = 0x00000020,
    CMD_RECORD_BEGIN            = 0x00000021,
    CMD_RECORD_STOP             = 0x00000022,
    CMD_SET_DDNS                = 0x00000028,
    CMD_SET_CARD_SNAP_REQUEST   = 0x00000049,
    CMD_SET_PTZ_CONTROL         = 0x00000050,
    CMD_REQUEST_IFRAME          = 0x00000063,

    CMD_GETSYSTIME              = 0x10000000,
    CMD_GET_OSDINFO             = 0x10000001,
    CMD_GET_STREAM              = 0x10000002,
    CMD_GET_COLOR               = 0x10000003,
    CMD_GET_MOTION_DETECT       = 0x10000004,
    CMD_GET_PROBE_ALARM         = 0x10000005,
    CMD_GET_VIDEO_LOST          = 0x10000006,
    CMD_GET_COMINFO             = 0x10000007,
    CMD_GET_USERINFO            = 0x10000008,
    CMD_GET_NETWORK             = 0x10000009,
    CMD_GET_FTPUPDATA_PARAM     = 0x1000000A,

    CMD_GET_PTZDATA             = 0x1000000B,	
    CMD_GET_COMINFO2            = 0x1000000C,
    CMD_GET_ALARM_OUT           = 0x1000000D,

    CMD_GET_NOTIFY_SERVER       = 0x1000000E,
    CMD_GET_PPPOE_DDNS          = 0x1000000F,


    CMD_SET_SENSOR_STATE        = 0x00000019,
    CMD_GET_SENSOR_STATE        = 0x10000010,

    CMD_GET_SERVER_RECORD_SET   = 0x10000011,
    CMD_GET_SERVER_RECORD_STATE = 0x10000012,
    CMD_UNLOAD_DISK             = 0x10000013,

    CMD_GET_CENTER_LICENCE      = 0x10000016,

    CMD_GET_DDNS_INFO           = 0x10000019,


    CMD_GET_SERVER_INFO         = 0x10000021,	
    CMD_GET_CHANNEL_CONFIG      = 0x10000022,

    CMS_SET_WIFI                = 0x00000046,
    CMS_SET_TDSCDMA             = 0x00000047,
    CMD_SET_PERIPH_CONFIG       = 0x00000048,

    CMS_GET_WIFI                = 0x10000046,
    CMS_GET_TDSCDMA             = 0x10000047,
    CMD_GET_PERIPH_CONFIG       = 0x10000048,

    CMD_GET_SERVER_RECORD_CONFIG= 0x1000004C,
    CMD_SET_SERVER_RECORD_CONFIG= 0x0000004C,

    CMD_GET_COMDATA             = 0x00000062,


    CMD_GET_FILELIST            = 0x1000004E,


    CMD_SET_MOBILE_CENTER       = 0x00000051,
    CMD_GET_MOBILE_CENTER       = 0x10000051,
    CMD_UPDATA_FLASH_FILE       = 0x00001000,

    CMD_NET_GET_NETCFG          = 0x020100,
    CMD_NET_SET_NETCFG          = 0x020101,
    CMD_NET_GET_PICCFG          = 0x020200,
    CMD_NET_SET_PICCFG          = 0x020201,
    CMD_NET_GET_DEF_PICCFG      = 0x020203,
    CMD_NET_GET_SUPPORT_STREAM_FMT = 0x020202,

    CMD_NET_GET_COLORCFG        = 0x020220,
    CMD_NET_SET_COLORCFG        = 0x020221,

    CMD_NET_GET_COLOR_SUPPORT   = 0x020222,
    CMD_NET_SET_COLORCFG_SINGLE = 0x020223,
    CMD_NET_GET_DEF_COLORCFG    = 0x020225,

    CMD_NET_GET_ENHANCED_COLOR_SUPPORT = 0x020224,
    CMD_NET_GET_ENHANCED_COLOR  = 0x020226,

    CMD_NET_GET_RESETSTATE      = 0x00010300,

    CMD_NET_GET_RESTORECFG      = 0x030300,
    CMD_NET_SET_RESTORECFG      = 0x030301,
    CMD_NET_GET_DEF_RESTORECFG  = 0x030302,
    CMD_NET_SET_SAVECFG         = 0x030303,
    CMD_NET_GET_RTSPCFG         = 0x030304,
    CMD_NET_SET_RTSPCFG         = 0x030305,

    CMD_NET_GET_DEVICECFG       = 0x020000,
    CMD_NET_SET_DEVICECFG       = 0x020001,

    CMD_NET_GET_WIFICFG         = 0x020160,
    CMD_NET_SET_WIFICFG         = 0x020161,
    CMD_NET_GET_WIFI_SITE_LIST  = 0x020162,
    CMD_GET_DIRECT_LIST         = 0x1000001A,	
    CMD_GET_FILE_DATA           = 0x1000001B,	    
    CMD_STOP_FILE_DOWNLOAD      = 0x00000029,
    CMD_UPDATA_DEFAULT_PTZCMD_DATA = 0x00000101,
    
    CMD_SET_UPNP				= 0x0000004D,
    CMD_GET_UPNP				= 0x1000004D,

    CMD_PLAY_FILE_BY_NAME       = 0x0000004E,
    CMD_PLAY_FILE_BY_TIME       = 0x0000004F,
    CMD_PLAY_CONTROL            = 0x00000050,

    CMD_NET_SET_ID_CIPHER	= 0x00010118
} ENUM_NET_IOCTRL;

/* command push to web */
#define TL_MSG_FILE_NAME_DATA           0x2000001D
#define	TL_STANDARD_ALARM_MSG_HEADER	0x100000
#define TL_SYS_FLAG_WIRELESS		    0x00080000

#define SERVER_PACK_FLAG		        0x03404324

typedef enum
{
    WAVE_FORMAT_ADPCM	    =	0x0002,
    WAVE_FORMAT_G711_ADPCM  =	0x003e,
    WAVE_FORMAT_MPEGLAYER3  =	0x0055,
    WAVE_FORMAT_G722_ADPCM	=	0x0065,
}ENUM_WAVE_FORMAT;

typedef enum 
{
    ENCODE_FORMAT_MGPE4	=   0,
    ENCODE_FORMAT_H264  =	4,
    ENCODE_FORMAT_MJPEG	=   5,
    ENCODE_FORMAT_H265	=	265,
} ENUM_ENCODE_FORMAT;

typedef enum
{
    TL_PLAY_CMD_PLAY	    =	1,
    TL_PLAY_CMD_PLAYPREV    =	2,
    TL_PLAY_CMD_STEPIN	    =	3,	
    TL_PLAY_CMD_STEPOUT	    =	4,
    TL_PLAY_CMD_PAUSE	    =	5,	
    TL_PLAY_CMD_RESUME	    =	6,	
    TL_PLAY_CMD_FASTPLAY    =	7,
    TL_PLAY_CMD_FASTBACK    =	8,
    TL_PLAY_CMD_STOP	    = 	9,	
    TL_PLAY_CMD_SLOWPLAY    =	10,	
    TL_PLAY_CMD_FIRST	    =	11,
    TL_PLAY_CMD_LAST	    = 	12,
    TL_PLAY_CMD_SEEK	    =	13,
    TL_PLAY_CMD_GET_INFO    =	14,
} PLAY_BACK_CMD;

typedef enum _SOCK_TYPE
{
	SOCK_LOGON = 0,
	SOCK_DATA = 1,
	SOCK_FILE = 2,
	SOCK_TALKBACK = 3,
	LINK_REMOTE_HOST = 4,
	SOCK_ALARM = 5, 	//????ͨ??
}SOCK_TYPE;

typedef enum _PROTOCOL_TYPE
{
    PROTOCOL_TCP = 0,
    PROTOCOL_UDP = 1,
    PROTOCOL_MULTI = 2
}PROTOCOL_TYPE;

/*????\C1\AC??ע?????ݽṹ??*/
typedef struct _ACCEPT_HEAD
{
	unsigned long	nFlag;  //Э???汾??ʶ
	unsigned long	nSockType;
}ACCEPT_HEAD,*PACCEPT_HEAD;

/*??Ƶ???\F7\A3\A8\B4򿪣??رգ??????ṹ??*/
typedef struct _OPEN_HEAD
{
	unsigned long	nFlag;    //Э???汾??ʶ
	unsigned long	nID;      //?û?ID,???û???¼??ʱ????ǰ?˷???
	PROTOCOL_TYPE   nProtocolType; //????Э??
	unsigned long	nStreamType; //??Ƶ?????ͣ???????????????
	unsigned long	nSerChannel; //??Ƶͨ?\C0\BA?
	unsigned long	nClientChannel; //??ʶ?????ͻ??˵???ʾ???ں?
}OPEN_HEAD,*POPEN_HEAD;

typedef struct _OPEN_FILE
{
    unsigned long   nFlag;
    unsigned long   nID;
	int				hTransferSock;
    unsigned long   nClientOpenID;
	
	
	unsigned long	lChannel;
    unsigned long	lYear;
    unsigned long	lMonth;
    unsigned long	lDay;
	unsigned long	lDisk;
    unsigned long   nStartHour;
    unsigned long   nStartMin;
	unsigned long	nStartSecond;
    unsigned long   nEndHour;

    unsigned long   nEndMin;
	unsigned long	nEndSecond;
}OPEN_FILE,*POPEN_FILE;

typedef struct _FILES_NUM
{
    unsigned long   nFlag;
    unsigned long   nFileNum;
    char            szFileName[60][16];
}FILES_NUM;

typedef struct _READ_HEAD
{
    unsigned long   nFlag;
    unsigned long   nCommand;
    unsigned long   nIndex;
    unsigned long   nFileLen;
    unsigned long   nReadPos;
    unsigned long   nReadLen;
    unsigned long   nReadResult;
}READ_HEAD;

typedef struct _FILE_DATA
{
    unsigned long   nFilePos;
    unsigned long   nDataLen;
}FILE_DATA,*PFILE_DATA;

#define USER_NAME_LEN   32 
#define USER_PASSWD_LEN 32

typedef struct _USER_INFO
{
	char	szUserName[USER_NAME_LEN];
	char	szUserPsw[USER_PASSWD_LEN];
	char	MacCheck[36];
}USER_INFO,*PUSER_INFO;

/*??????????Ϣͷ*/
typedef struct _COMM_HEAD
{
	unsigned long	nFlag;
	unsigned long	nCommand;
	unsigned long	nChannel;
	unsigned long	nErrorCode;
	unsigned long	nBufSize;
}COMM_HEAD,*PCOMM_HEAD;

typedef struct _COMM_NODE
{
    COMM_HEAD   commHead;
    char        pBuf[NETCMD_BUF_SIZE];
}COMM_NODE,*PCOMM_NODE;

typedef struct _UPDATE_FILE_PARTITION
{
	unsigned long	nFileLength;
	unsigned long	nFileOffset;
	unsigned long	nPartitionNum;
	unsigned long	nPartitionNo;
	unsigned long	nPartitionSize;
	char			szFileName[260];
}UPDATE_FILE_PARTITION;

typedef struct _TALK_PAR
{
    unsigned long   nFlag;
    unsigned long   nChannel;
    unsigned long   nBits;
    unsigned long   nSamples;
}TALK_PAR;



///Video New Protocol Frame Header
typedef struct tagFRAME_HEADER
{
	WORD	wMotionDetect;
	WORD	wFrameIndex;
	DWORD	dwVideoSize;
	DWORD	dwTimeTick;
	WORD	wAudioSize;
	BYTE	byKeyFrame;
	BYTE	byPackIndex;
}FRAME_HEADER,*PFRAME_HEADER;


///Video New Protocol DATAPACK
typedef struct tagDATA_PACKET
{
	WORD			wIsSampleHead;
	WORD			wBufSize;
	FRAME_HEADER	stFrameHeader;
	unsigned char		byPackData[PACK_SIZE];
}DATA_PACKET,*PDATA_PACKET;

//
typedef struct _NET_DATA_HEAD
{
	unsigned long	nFlag;
	unsigned long	nSize;
}NET_DATA_HEAD,*PNET_DATA_HEAD;

//
typedef struct _NET_DATA_PACKET
{
    NET_DATA_HEAD       stPackHead;
    DATA_PACKET         stPackData;
}NET_DATA_PACKET,*PNET_DATA_PACKET;


typedef struct _PARAM_HEAD
{
	int				nTotalDataLen;
	int				nTotalPackNum;
	int				nPacketSize;
	int				nCurrentPackNo;
	int				nIndexNo;
}PARAM_HEAD,*PPARAM_HEAD;



#endif
