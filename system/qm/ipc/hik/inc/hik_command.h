#ifndef _HIK_COMMAND_H_
#define _HIK_COMMAND_H_

#include <linux/if_ether.h>  
#include <arpa/inet.h>

#include "hik_configtypes.h"
#include "QMAPIType.h"

#define CURRENT_NETSDK_VERSION	NETSDK_VERSION3_1

/* network command defines */
/* 新、老网络SDK的接口 */
#define NEW_NETSDK_INTERFACE	90
#define OLD_NETSDK_INTERFACE	60
#define CLIENT_SDK_VERSION		99

#define MAKE_NETSDK_VERSION(_major, _minor, _year, _month, _day)	\
	(	\
	(((_major)&0xff)<<24) | 	\
	(((_minor)&0xff)<<16) |	\
	((((_year)-2000)&0x3f)<<10)	|	\
	(((_month)&0xf)<<6) |	\
	((_day)&0x3f)	\
	)

#define GET_NETSDK_MAJOR(version)	(((version)>>24)&0xff)
#define GET_NETSDK_MINOR(version)	(((version)>>16)&0xff)
#define GET_NETSDK_YEAR(version)	((((version)>>10)&0x3f)+2000)
#define GET_NETSDK_MONTH(version)	(((version)>>6)&0xf)
#define GET_NETSDK_DAY(version)		((version)&0x3f)

#define GET_NETSDK_BUILDDATE(version)	((version)&0xffff)
#define RELOGIN_FLAG					0x80000000
#define NETSDK_VERSION1_0	MAKE_NETSDK_VERSION(1, 0, 2004, 10, 4)
#define NETSDK_VERSION1_2	MAKE_NETSDK_VERSION(1, 2, 2005, 3, 15)		/* 用户名/密码加密，硬盘数为16 */
#define NETSDK_VERSION1_3	MAKE_NETSDK_VERSION(1, 3, 2005, 4, 1)		/* 移动侦测/视频丢失/遮挡报警布防时间段 */
#define NETSDK_VERSION1_4	MAKE_NETSDK_VERSION(1, 4, 2005, 5, 30)		/* 预置点数目 16 -> 128 */
#define NETSDK_VERSION1_5	MAKE_NETSDK_VERSION(1, 5, 2005, 12, 28)		/* 用户的权限细化到通道(本地回放、远程回放、远程预览)*/
#define NETSDK_VERSION2_0   MAKE_NETSDK_VERSION(2, 0, 2006, 4, 27)
#define NETSDK_VERSION2_1	MAKE_NETSDK_VERSION(2, 1, 2006, 8, 14)		/* 这个版本以上的SDK需要二次认证 */
#define NETSDK_VERSION3_0	MAKE_NETSDK_VERSION(3, 0, 2008, 2, 28)		
#define NETSDK_VERSION3_1	MAKE_NETSDK_VERSION(3, 1, 2009, 7, 30)		
#define NETSDK_VERSION4_0   MAKE_NETSDK_VERSION(4, 0, 2010, 3, 1)
#define NETSDK_VERSION5_1   MAKE_NETSDK_VERSION(5, 1, 2015, 5, 11)

/* define status that return to client */
#define NETRET_QUALIFIED		1
#define NETRET_EXCHANGE			2
#define NETRET_ERRORPASSWD		3
#define NETRET_LOWPRI			4
#define NETRET_OPER_NOPERMIT	5
#define NETRET_VER_DISMATCH		6
#define NETRET_NO_CHANNEL		7
#define NETRET_NO_SERIAL		8
#define NETRET_NO_ALARMIN		9
#define NETRET_NO_ALARMOUT		10
#define NETRET_NO_DISK			11
#define NETRET_NO_HARDDISK		12
#define NETRET_NOT_SUPPORT		13
#define NETRET_ERROR_DATA		14
#define NETRET_CHAN_ERROR		15
#define NETRET_DISK_ERROR		16
#define NETRET_CMD_TIMEOUT		17
#define NETRET_MAXLINK			18
#define NETRET_NEEDRECVHEADER	19
#define NETRET_NEEDRECVDATA		20
#define NETRET_NEEDRECVDATA_V30	104
#define NETRET_IPCCFG_CHANGE	105
#define NETRET_IPCCFG_CHANGE_V31	106
#define NETRET_KEEPAUDIOTALK    NETRET_EXCHANGE
#define NETRET_RECVKEYDATA      90       	/* SETPOS后接收发过来的是关键帧数据（往前找I帧）*/
#define NETRET_RECVSETPOSDATA   91       	/* SETPOS后接收改变位置后实际位置的数据 */
#define NETRET_SAME_USER        49
#define NETRET_DEVICETYPE_ERROR 50			/*导入参数时设备型号不匹配*/
#define NETRET_LANGUAGE_ERROR   51 			/*导入参数时语音不匹配*/
#define NETRET_PARAVERSION_ERROR	52		/*导入参数时软件版本不匹配*/
#define NETRET_IPC_NOTCONNECT   53			/*IPC不在线*/
/*去除NETRET_720P_SUB_NOTSUPPORT, 需要时返回 NETRET_NOT_SUPPORT*/
//#define NETRET_720P_SUB_NOTSUPPORT         54/*720P 子码流不支持*/
#define NETRET_IPC_COUNT_OVERFLOW	54		/*IPC总数溢出*/
#define NETRET_EMAIL_TEST_ERROR		55		/*邮件测试失败 9000_1.1*/

#define NETRET_PLAY_END			21
#define NETRET_WRITEFLASHERROR	22
#define NETRET_UPGRADE_FAILED	23
#define NETRET_UPGRADING		24
#define NETRET_NEEDWAIT			25
#define NETRET_FILELISTOVER		26
#define NETRET_RECVFILEINFO		27
#define NETRET_FORMATING		28
#define NETRET_FORMAT_END		29
#define NETRET_NO_USERID		30
#define NETRET_EXCEED_MAX_USER	31
#define NETRET_DVR_NO_RESOURCE	32
#define NETRET_DVR_OPER_FAILED	33			/* operation failed */
#define NETRET_IP_MISMATCH		34
#define NETRET_SOCKIP_MISMATCH	35
#define NETRET_MAC_MISMATCH		36
#define NETRET_ENC_NOT_STARTED	37
#define NETRET_LANG_MISMATCH	38			/* 升级时语言不匹配 */
#define NETRET_NEED_RELOGIN		39			/* 需要进行密码/用户名二次认证 */
#define NETRET_HD_READONLY		48


#define NETRET_IPCAMERA 		30 			/*IP 摄像机*/
#define NETRET_MEGA_IPCAMERA 	31  		/*百万像素IP 摄像机*/
#define NETRET_IPDOME 			40 			/*IP 快球*/
#define NETRET_IPMOD 			50 			/*IP 模块*/

#define ABILITY_NOT_SUPPORT		1000		//设备能力级不支持

typedef struct _MAC_FRAME_HEADER
{
	char m_cDstMacAddress[6];		//目的mac地址
	char m_cSrcMacAddress[6];		//源mac地址
	short m_cType;					//上一层协议类型，如0x0800代表上一层是IP协议，0x0806为arp
}MAC_FRAME_HEADER,*PMAC_FRAME_HEADER;

#pragma pack(2)
typedef struct
{
	int		m_ServiceType;
	int		m_Seqnum;
	int		m_CmdType;
	short	m_Checksum;

	char	m_SrcMac[6];
	int		m_SrcIp;
	char	m_DstMac[6];
	int		m_DstIp;
	char	m_res[32];
}NET_CLIENT_BROADCAST;

typedef struct
{
	int		m_ServiceType;
	int		m_Seqnum;
	int		m_CmdType;
	short	m_Checksum;

	char	m_SrcMac[6];
	int		m_SrcIp;
	char	m_DstMac[6];
	int		m_DstIp;

	int		m_DstMaskip;
	char    m_unkown[18];
	short		m_port;

	char	m_res[8];
}NET_BROADCAST_FIX_IP;


typedef struct
{
	int		m_ServiceType;
	int		m_Seqnum;
	int		m_CmdType;
	unsigned short	m_Checksum;

	char	m_SrcMac[6];
	int		m_SrcIp;
	char	m_DstMac[6];
	int		m_DstIp;
	int		m_SrcNetmask;
	char	m_DevSerialNum[48];

	int		m_unknown1;
	int		m_ServicePort;
	int		m_ChannelNum;
	int		m_unuse1;

	char	m_SoftwareVersion[48];
	char	m_DspVersion[48];
	char	m_Starttime[48];	//校验计算到此成员为止

	int		m_unknown3;
	int		m_SrcGateway;

	char	m_unuse2[32];

	int		m_unknown4;
	char	m_unuse3[28];
	int		m_unknown5;
	char	m_unuse4[32];

	char	m_DeviceType[48];
}NET_BROADCAST;

//返回给客户端状态的头 
typedef struct{
	UINT32	length;
	UINT32	checkSum;
	UINT32	retVal;
	UINT8	res[4];
}NETRET_HEADER;

//login
typedef struct{
	UINT32 	length;
	UINT8	ifVer;
	UINT8	res1[3];
	UINT32	checkSum;
	UINT32	netCmd;
	UINT32	version;
	UINT8	res2[4];
	UINT32	clientIp;
	UINT8	clientMac[6];
	UINT8	res3[2];
	UINT8	username[NAME_LEN];
	UINT8	password[PASSWD_LEN];
}NET_LOGIN_REQ;

typedef struct{
	UINT32	length;
	UINT32	checkSum;
	UINT32	retVal;
	UINT32  devSdkVer;
	DWORD	dwUserID;
	BYTE	sSerialNumber[SERIALNO_LEN];
	BYTE	byDVRType;
	BYTE	byChanNum;
	BYTE	byStartChan;
	BYTE	byAlarmInPortNum;
	BYTE	byAlarmOutPortNum;
	BYTE	byDiskNum;
	BYTE	byRes1[2];
	BYTE    byRes2[32];		//这32字节数据应该和客户端的版本有关
}NET_LOGIN_RET;

typedef struct
{
	DWORD	netCmd;
	char    CmdType[100];		//设备能力级具体的命令内容,具体大小不知，暂定
}NET_DEVABILITY_RET;

typedef struct{
	UINT32 	length;
	UINT8  	ifVer;
	UINT8	res1[3];
	UINT32	checkSum;
	UINT32	netCmd;
	UINT32	clientIp;
	UINT32	userID;	
	UINT8	clientMac[6];
	UINT8	res2[2];
	UINT8	res3[20];
	char	CmdType[100];
}NET_ISAPIABILITY;

typedef struct{
	DWORD	dwsize;
	DWORD	unknow1;
	DWORD	unknow2;
	char	data[4096];
}NET_ISAPIABILITY_RET;

typedef struct {
		UINT32	length;			/* 返回数据总长度 */
		UINT32  checkSum;		/* 数据校验和 */
		UINT32  retVal;			/* 返回状态，特定的标识表示是9000设备 */
		UINT32  devSdkVer;
		UINT8	challenge[60];	/* BASE64加密之后的挑战串 */
		//UINT8	challenge[80];	/* BASE64加密之后的挑战串 */ //抓包分析此结构体大小为96
} NET_LOGIN_CHALLENGE;

typedef struct{
	UINT32	userID;
	UINT8	serialno[SERIALNO_LEN];
	UINT8	devType;
	UINT8	channelNums;
	UINT8	firstChanNo;
	UINT8	alarmInNums;
	UINT8	alarmOutNums;
	UINT8	hdiskNums;
	UINT8	res2[2];
}NET_LOGIN_RETDATA;

/*
 * ========================================================
 *			IPC parameter 
 * ========================================================
 */
typedef struct{
	UINT32 enable;
	char username[NAME_LEN];
	char password[PASSWD_LEN];	
	U_IN_ADDR ip;
	UINT16 cmdPort;
	UINT8 factory;
	UINT8 res[33];
}SINGLE_IPDEV_CFG;

typedef struct{
	UINT8 enable;
	UINT8 factory;
	UINT8 res1[2];
	char username[NAME_LEN];
	char password[PASSWD_LEN];
	char domainname[MAX_DOMAIN_NAME];
	U_IN_ADDR ip;
	UINT16 cmdPort;
	UINT8 res2[34];
}SINGLE_IPDEV_CFG_V31;

typedef struct{
	SINGLE_IPDEV_CFG  ipDevInfo[MAX_IP_DEVICE];
}NETPARAM_IPDEV_CFG;

typedef struct{
	SINGLE_IPDEV_CFG_V31  ipDevInfo[MAX_IP_DEVICE];
}NETPARAM_IPDEV_CFG_V31; 

typedef struct{
	UINT8 enable;
	UINT8 ipID;
	UINT8 channel;
	UINT8 res[33];
}SINGLE_IPCHAN_CFG;

typedef struct{
	UINT8 analogChanEnable[MAX_ANALOG_CHANNUM/8];
	SINGLE_IPCHAN_CFG  ipChanInfo[MAX_IP_CHANNUM];
}NETPARAM_IPCHAN_CFG;

typedef struct{
	UINT32 length;
	NETPARAM_IPDEV_CFG  ipDevCfg;
	NETPARAM_IPCHAN_CFG ipChanCfg;
}NETPARAM_IPCORE_CFG;

typedef struct{
	UINT32 length;
	NETPARAM_IPDEV_CFG_V31  ipDevCfg;
	NETPARAM_IPCHAN_CFG ipChanCfg;
}NETPARAM_IPCORE_CFG_V31;



typedef struct{
	UINT8 ipID;
	UINT8 alarmIn;
	UINT8 res[18];
}SINGLE_IPALARMIN_CFG;

typedef struct{
	UINT32 length;
	SINGLE_IPALARMIN_CFG ipAlarmInInfo[MAX_IP_ALARMIN];
}NETPARAM_IPALARMIN_CFG;

typedef struct{
	UINT8 ipID;
	UINT8 alarmOut;
	UINT8 res[18];
}SINGLE_IPALARMOUT_CFG;

typedef struct{
	UINT32 length;
	SINGLE_IPALARMOUT_CFG ipAlarmOutInfo[MAX_IP_ALARMOUT];
}NETPARAM_IPALARMOUT_CFG;

typedef struct{
	NETPARAM_IPDEV_CFG  ipDevCfg;
	NETPARAM_IPCHAN_CFG ipChanCfg;
	SINGLE_IPALARMIN_CFG ipAlarmInInfo[MAX_IP_ALARMIN];
	SINGLE_IPALARMOUT_CFG ipAlarmOutInfo[MAX_IP_ALARMOUT];
}NETRET_IPC_ALARMINFO;

typedef struct{
	NETPARAM_IPDEV_CFG_V31  ipDevCfg;
	NETPARAM_IPCHAN_CFG ipChanCfg;
	SINGLE_IPALARMIN_CFG ipAlarmInInfo[MAX_IP_ALARMIN];
	SINGLE_IPALARMOUT_CFG ipAlarmOutInfo[MAX_IP_ALARMOUT];
}NETRET_IPC_ALARMINFO_V31;

typedef struct{
	UINT32	alarmType;
	UINT8	alarmInStatus[32];	    /* 按位,第0位对应第0个输入端口 */
	UINT8	alarmOutStatus[16];		/* 报警输入触发的报警输出，按位,第0位对应第0个输出端口, alarmType为0时需要 */
	UINT8   triggeredRecChan[16];		/* 报警输入触发的录象通道，按位,第0位表示第0个通道 */
	UINT8	channelNo[16];				/* 按位,第0位对应第0个通道，alarmType为2或3,6时需要设置 */
	UINT8	diskNo[16];					/* 按位,第0位对应第0个硬盘,dwAlarmType为4,5时需要设置 */
}NETRET_ALARMINFO_V30;

typedef struct{
	UINT32	alarmType;
	UINT32	alarmInNumber;			/* 按位,第0位对应第0个输入端口 */
	UINT32	triggeredAlarmOut;		/* 报警输入触发的报警输出，按位,第0位对应第0个输出端口, alarmType为0时需要 */
	UINT16	triggeredIPCAlarmOut[MAX_IPCHANNUM];		/* IPC 报警输入触发的报警输出，按位,第0位对应第0个输出端口, alarmType为0时需要 */
	UINT64	triggeredRecChan;		/* 报警输入触发的录象通道，按位,第0位表示第0个通道 */
	UINT64	channelNo;				/* 按位,第0位对应第0个通道，alarmType为2或3,6时需要设置 */
	UINT64	diskNo;					/* 按位,第0位对应第0个硬盘,dwAlarmType为4,5时需要设置 */
}NETRET_ALARMINFO;


#define MAX_ALARMINFO_MSGQ_NUM	32	
#define MAX_ALARMINFO_MSGQ_LEN	sizeof(NETRET_ALARMINFO)
#define ALARMINFO_MSG_PATH  "/alarmInfo-msg"
#if 0
typedef struct{
	NODE node;
	UINT32 userID;
	int  connfd;
	mqd_t alarmInfoMsgQ;
}ALARM_UP_CONNS;
#endif

/* DVR 工作状态 */
typedef struct{
	UINT8	bRecStarted;		/* 是否录象，1--录象/0--不录象 */
	UINT8	bViLost;			/* VI信号丢失，0--正常/1--丢失 */
	UINT8	chanStatus;			/* 通道硬件状态，0--正常/1--异常 */
	UINT8	res1;
	UINT32	bitRate;			/* 实际码率 */
	UINT32	netLinks;			/* 客户端连接数 */
	UINT32	clientIP[6];	/* 客户端IP地址 */
}ENC_CHAN_STATUS;

typedef struct{
	UINT8	bRecStarted;		/* 是否录象，1--录象/0--不录象 */
	UINT8	bViLost;			/* VI信号丢失，0--正常/1--丢失 */
	UINT8	chanStatus;			/* 通道硬件状态，0--正常/1--异常 */
	UINT8	res1;
	UINT32	bitRate;			/* 实际码率 */
	UINT32	netLinks;			/* 客户端连接数 */
	U_IN_ADDR clientIP[6];	/* 客户端IP地址 */
	UINT32	ipcNetLinks;			/* IPC 连接数 */
	UINT8  	res[12];
}ENC_CHAN_STATUS_V30;

typedef struct{
	UINT32	totalSpace;
	UINT32	freeSpace;
	UINT32	diskStatus;			/* bit 0 -- idle, bit 1 -- error */
}HDISK_STATUS;

typedef struct{
	//NODE node;
	UINT32 userID;
	int  connfd;
	//mqd_t alarmInfoMsgQ;
}ALARM_UP_CONNS;


typedef struct{
	UINT32	deviceStatus;			/* 设备的状态,0-正常,1-CPU占用率太高,超过85%,2-硬件错误,例如串口死掉 */
	HDISK_STATUS hdStatus[16];
	ENC_CHAN_STATUS chanStatus[MAX_CHANNUM_8000];
	UINT8  	alarmInStatus[MAX_ALARMIN_8000];
	UINT8	alarmOutStatus[MAX_ALARMOUT_8000];
	UINT32  localDispStatus;		/* 本地显示状态,0-正常,1-不正常 */
}DVR_WORKSTATUS;

typedef struct{
	UINT32		 deviceStatus;			/* 设备的状态,0-正常,1-CPU占用率太高,超过85%,2-硬件错误,例如串口死掉 */
	HDISK_STATUS hdStatus[16];
	ENC_CHAN_STATUS_V30 chanStatus[16];
	UINT8  	alarmInStatus[16];
	UINT8	alarmOutStatus[16];
	UINT32  localDispStatus;		/* 本地显示状态,0-正常,1-不正常 */
	UINT8   audioInChanStatus;
	UINT8   res[35];
}DVR_WORKSTATUS_V30;


//device parameter
typedef struct{
	UINT32 	length;
	UINT8	DVRName[NAME_LEN];
	UINT32	deviceID;
	UINT32	recycleRecord;
	UINT8	serialno[SERIALNO_LEN];
	UINT32	softwareVersion;
	UINT32	softwareBuildDate;
	UINT32	dspSoftwareVersion;
	UINT32	dspSoftwareBuildDate;
	UINT32	panelVersion;
	UINT32	hardwareVersion;
	UINT8	alarmInNums;
	UINT8	alarmOutNums;
	UINT8	rs232Nums;
	UINT8	rs485Nums;
	UINT8	netIfNums;
	UINT8	hdiskCtrlNums;
	UINT8	hdiskNums;
	UINT8	devType;
	UINT8	channelNums;
	UINT8	firstChanNo;
	UINT8	decodeChans;
	UINT8	vgaNums;
	UINT8	usbNums;
	UINT8	auxOutNum;
	UINT8	audioNum;
	UINT8 	ipChanNum;
}NETPARAM_DEVICE_CFG;

typedef struct{
    UINT32  length;
    UINT8   DVRName[NAME_LEN];
    UINT32  deviceID;
    UINT32  recycleRecord;
    UINT8   serialno[SERIALNO_LEN];
    UINT32  softwareVersion;
    UINT32  softwareBuildDate;
    UINT32  dspSoftwareVersion;
    UINT32  dspSoftwareBuildDate;
    UINT32  panelVersion;
    UINT32  hardwareVersion;
    UINT8   alarmInNums;
    UINT8   alarmOutNums;
    UINT8   rs232Nums;
    UINT8   rs485Nums;
    UINT8   netIfNums;
    UINT8   hdiskCtrlNums;
    UINT8   hdiskNums;
    UINT8   unknown1;
    UINT8   channelNums;
    UINT8   firstChanNo;
    UINT8   decodeChans;
    UINT8   vgaNums;
    UINT8   usbNums;
    UINT8   auxOutNum;
    UINT8   audioNum;
    UINT8   ipChanNum;
    UINT8   zeroChanNum;        /* 零通道编码个数 */
    UINT8   supportAbility;     /* 能力，位与结果为0表示不支持，1表示支持，
                                 supportAbility & 0x1, 表示是否支持智能搜索
                                supportAbility & 0x2, 表示是否支持一键备份
                                supportAbility & 0x4, 表示是否支持压缩参数能力获取
                                supportAbility & 0x8, 表示是否支持双网卡 
                                supportAbility & 0x10, 表示支持远程SADP 
                                supportAbility & 0x20  表示支持Raid功能*/
    
    UINT8   bESataFun;          /* eSATA的默认用途，0-默认录像，1-默认备份 */
    UINT8   bEnableIPCPnp;      /* 0-不支持即插即用，1-支持即插即用 */
    UINT8   unknown2;
    UINT8   unknown3;
    UINT8   unknown4;
	UINT8   CPUType;
    UINT8   devType[40];            /* 保留 */
}NETPARAM_DEVICE_CFG_V40;

/*
 * ====================================================================================
 *									 net data struct
 * ====================================================================================
 */
typedef struct{	//sizeof = 32
	UINT32 	length;				/* total length */
	UINT8  	ifVer;				/* version: 90 -- new interface/60 -- old interface */
	UINT8	res1[3];
	UINT32	checkSum;			/* checksum */
	UINT32	netCmd;				/* client request command */
	UINT32	clientIp;			/* clinet IP address */
	UINT32	userID;				/* user ID */
	UINT8	clientMac[6];
	UINT8	res[2];
}NETCMD_HEADER;

typedef struct{
	NETCMD_HEADER header;
	UINT32 channel;
}NETCMD_CHAN_HEADER;

typedef struct{
	UINT32 		handleType;							/* 异常处理,异常处理方式的"或"结果 */
	UINT8		alarmOutTrigStatus[16];				/* 被触发的报警输出(此处限制最大报警输出数为32) */
}NETPARAM_EXCEPTION_V30;

typedef struct{
	UINT8	startHour;
	UINT8	startMin;
	UINT8	stopHour;
	UINT8	stopMin;
}NETPARAM_TIMESEG;

typedef struct{
	UINT8		alarmInName[NAME_LEN];				/* 名称 */
	UINT8		sensorType;							/* 报警器类型：0为常开；1为常闭 */
	UINT8		bEnableAlarmIn;						/* 处理报警输入 */
	UINT8		res[2];
	NETPARAM_EXCEPTION_V30 alarmInAlarmHandleType;				/* 报警输入处理 */
	NETPARAM_TIMESEG armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* 布防时间段 */
	UINT8		recordChanTriggered[16];			/* 被触发的录像(此处限制最大通道数为128) */
	UINT8		bEnablePreset[16];					/* 是否调用预置点 */
	UINT8		presetNo[16];						/* 调用的云台预置点序号,一个报警输入可以调用
												   	   多个通道的云台预置点, 0xff表示不调用预置点。
												   	 */
	UINT8		bEnablePresetRevert[16];	
	UINT16		presetRevertDelay[16];
	UINT8       bEnablePtzCruise[16];				/* 是否调用巡航 */
	UINT8		ptzCruise[16];						/* 巡航 */
	UINT8       bEnablePtzTrack[16];				/* 是否调用轨迹 */
	UINT8		trackNo[16];						/* 云台的轨迹序号(同预置点) */
}NETPARAM_ALARMIN_V30;

typedef struct{
	UINT32	length;
	NETPARAM_ALARMIN_V30 alarmIn;
}NETPARAM_ALARMIN_CFG_V30;

/*
 * ========================================================
 *			 alarmOut parameter 
 * ========================================================
 */
typedef struct{
	UINT8	alarmOutName[NAME_LEN];				/* 名称 */
	UINT32	alarmOutDelay;
	NETPARAM_TIMESEG alarmOutTimeSeg[MAX_DAYS][MAX_TIMESEGMENT_8000];		/* 报警输出激活时间段 */
}NETPARAM_ALARMOUT;

typedef struct{
	UINT32 length;
	NETPARAM_ALARMOUT alarmOut;
}NETPARAM_ALARMOUT_CFG;

typedef struct{
	UINT8	alarmOutName[NAME_LEN];				/* 名称 */
	UINT32	alarmOutDelay;
	NETPARAM_TIMESEG alarmOutTimeSeg[MAX_DAYS][MAX_TIMESEGMENT];		/* 报警输出激活时间段 */
}NETPARAM_ALARMOUT_V30;

typedef struct{
	UINT32 length;
	NETPARAM_ALARMOUT_V30 alarmOut;
	UINT8 res[16];
}NETPARAM_ALARMOUT_CFG_V30;


#define CLOSE_ALL		0	/* 关闭(或断开)所有开关 */
#define CAMERA_PWRON	1	/* 接通摄像机电源 */
#define LIGHT_PWRON		2	/* 接通灯光电源 */
#define WIPER_PWRON		3	/* 接通雨刷开关 */
#define FAN_PWRON		4	/* 接通风扇开关 */
#define HEATER_PWRON	5	/* 接通加热器开关 */
#define AUX_PWRON1		6	/* 接通辅助设备开关 */
#define AUX_PWRON2		7	/* 接通辅助设备开关 */
#define SET_PRESET		8	/* 设置预置点 */
#define CLE_PRESET		9	/* 清除预置点 */

#define STOP_ALL		10	/* 停止所有连续量(镜头, 云台)动作 */
#define ZOOM_IN			11	/* 焦距以速度SS变大(倍率变大) */
#define ZOOM_OUT		12	/* 焦距以速度SS变小(倍率变小) */
#define FOCUS_IN		13	/* 焦点以速度SS前调 */
#define FOCUS_OUT		14	/* 焦点以速度SS后调 */
#define IRIS_ENLARGE	15	/* 光圈以速度SS扩大 */
#define IRIS_SHRINK		16	/* 光圈以速度SS缩小 */
#define AUTO_ZOOM		17	/* 开自动焦距(自动倍率) */
#define AUTO_FOCUS		18	/* 开自动调焦 */
#define AUTO_IRIS		19	/* 开自动光圈 */

#define TILT_UP			21	/* 云台以SS的速度上仰 */
#define TILT_DOWN		22	/* 云台以SS的速度下俯 */
#define PAN_LEFT		23	/* 云台以SS的速度左转 */
#define PAN_RIGHT		24	/* 云台以SS的速度右转 */
#define UP_LEFT			25	/* 云台以SS的速度上仰和左转 */
#define UP_RIGHT		26	/* 云台以SS的速度上仰和右转 */
#define DOWN_LEFT		27	/* 云台以SS的速度下俯和左转 */
#define DOWN_RIGHT		28	/* 云台以SS的速度下俯和右转 */
#define AUTO_PAN		29	/* 云台以SS的速度左右自动扫描 */

#define FILL_PRE_SEQ	30	/* 将预置点加入巡航序列 */
#define SET_SEQ_DWELL	31	/* 设置巡航点停顿时间 */
#define SET_SEQ_SPEED	32	/* 设置巡航速度 */
#define CLE_PRE_SEQ		33	/* 将预置点从巡航速度中删除 */
#define STA_MEM_CRUISE	34	/* Set Cruise start memorize */
#define STO_MEM_CRUISE	35	/* Set Cruise stop memorize */
#define RUN_CRUISE		36	/* 开始轨迹 */
#define RUN_SEQ			37	/* 开始巡航 */
#define STOP_SEQ		38	/* 停止巡航 */
#define GOTO_PRESET		39  /* 快球转到预置点 */
#define SYSTEM_RESET	40	/* 系统复位 */

#define CLICK_ZOOM		41	/*鼠标点击事件*/
#define BEG_ADD_KEYPT	42	/*开始添加关键点*/
#define END_ADD_KEYPT	43	/*结束添加关键点*/
#define DEL_CRUISE     	44 	/*删除巡航路径*/
#define MENU_MODE       45 	/*	进入菜单模式*/
#define Clean_All_PreSet  46 /*	清除所有的预置点命令*/
/* PTZ control when preview*/
typedef struct{
	NETCMD_HEADER header;
	UINT32 channel;
	UINT32 command;
	union{
	UINT32 presetNo;
	UINT32 speed;
	};
}NET_PTZ_CTRL_DATA;


/*
 * ========================================================
 *					network parameter
 * ========================================================
 */
//8000 network parameter 
typedef struct{
	UINT32	devIp;
	UINT32	devIpMask;
	UINT32	mediaType;		/* network interface type */
	UINT16	ipPortNo;		/* command port */
	UINT16	mtu;			/* MTU */
	UINT8	macAddr[6];
	UINT8	res2[2];
}NETPARAM_ETHER_CFG;

typedef struct{
	UINT32	length;
	NETPARAM_ETHER_CFG etherCfg[MAX_ETHERNET];
	UINT32	manageHostIp;
	UINT16	manageHostPort;
	UINT16	httpPort;
	UINT32	ipResolverIpAddr;
	UINT32	mcastAddr;
	UINT32	gatewayIp;
	UINT32	nfsIp;
	UINT8	nfsDirectory[PATHNAME_LEN];
	UINT32	bEnablePPPoE;
	UINT8	pppoeName[NAME_LEN];
	UINT8	pppoePassword[PASSWD_LEN];
	UINT8	res2[4];
	UINT32	pppoeIp;
}NETPARAM_NETWORK_CFG;



typedef struct{
	UINT8	streamType;
	UINT8	resolution;
	UINT8	bitrateType;
	UINT8	quality;
	UINT32	maxBitRate;
	UINT32	videoFrameRate;
	UINT16  intervalFrameI;
	UINT8	BFrameNum;			/* B帧个数: 0:BBP, 1:BP, 2:P */
	UINT8   EFrameNum;
	UINT8   videoEncType; //视频编码格式
	UINT8   audioEncType;
	UINT8   res[10];
}NETPARAM_COMP_PARA_V30;

typedef struct{
	UINT32	length;
	NETPARAM_COMP_PARA_V30 normHighCompPara;
	NETPARAM_COMP_PARA_V30 normLowCompPara;
	NETPARAM_COMP_PARA_V30 eventCompPara;
	NETPARAM_COMP_PARA_V30 netCompPara;
}NETPARAM_COMPRESS_CFG_V30;



typedef struct{
	BYTE byStreamType; //码流类型 1-视频流,3-复合流
	BYTE byResolution; //分辨率0-DCIF, 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF
	BYTE byBitrateType; //码率类型 0:定码率， 1:变码率
	BYTE byPicQuality; //图象质量 0-最好 1-次好 2-较好 3-一般 4-较差 5-差
	DWORD dwVideoBitrate; //视频码率 0-无限制 1-16K 2-32K 3-48k 4-64K 5-80K
							//6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
							//13-384K 14-448K 15-512K 16-640K 17-768K 18-896K
							//19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
							//最高位(32 位)置成 1 表示是自定义码流, 0-31 位表示码流值(最小值 16k，最大值 8192k)。
	DWORD dwVideoFrameRate; //帧率 0-全部; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4; 8-6;
							//9-8; 10-10; 11-12; 12-16; 13-20
}INTER_COMPRESSION_INFO, *LPINTER_COMPRESSION_INFO;

typedef struct{
	DWORD dwSize;
	INTER_COMPRESSION_INFO struRecordPara; /* 录像或者事件触发录像参数 */
	INTER_COMPRESSION_INFO struNetPara; /* 网传（事件触发录像参数时该项保留） */
}INTER_COMPRESSIONCFG, *LPINTER_COMPRESSIONCFG;


typedef struct
{
	BYTE byStreamType; //码流类型 1-视频流,3-复合流
	BYTE byResolution; //分辨率0-DCIF, 1-CIF, 2-QCIF, 3-4CIF, 4-2CIF
						//16-VGA, 17-UXGA, 18-SVGA, 19-HD720p (16-19高清分
						//辨率)
	BYTE byBitrateType; //码率类型 0:定码率， 1:变码率
	BYTE byPicQuality; //图象质量 0-最好 1-次好 2-较好 3-一般 4-较差 5-差
	DWORD dwVideoBitrate; //视频码率 0-保留 1-16K 2-32K 3-48k 4-64K 5-80K 6-96K
							//7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
							// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K
							//19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
							//最高位(31位)置成1表示是自定义码流, 0-30位表示码流值。
	DWORD dwVideoFrameRate; //帧率 0-全部; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4;
							//8 - 6; 9 - 8; 10 - 10; 11 - 12; 12 - 16; 13 - 20; V2.0版本中新加14 - 15; 15 - 18; 16 - 22;
	WORD wIntervalFrameI; //I帧间隔
							//BYTE byRes[2];
							//2006-08-11 增加单P帧的配置接口，可以改善实时流延时问题
	BYTE byIntervalBPFrame;//0-BBP帧; 1-BP帧; 2-单P帧
	BYTE byENumber;//E帧数量
}INTER_COMPRESSION_INFO_EX, *LPINTER_COMPRESSION_INFO_EX;

typedef struct
{
	DWORD dwSize;
	INTER_COMPRESSION_INFO_EX struRecordPara; //录像
	INTER_COMPRESSION_INFO_EX struNetPara;//网传
}INTER_COMPRESSIONCFG_EX, *LPINTER_COMPRESSIONCFG_EX;


////////////////////////////////////////////////////////////////
typedef struct{
	DWORD dwAlarmType;/*0-信号量报警,1-硬盘满,2-信号丢失， 3－移动侦测， 4－硬盘未
					  格式化,5-写硬盘出错,6-遮挡报警， 7-制式不匹配, 8-非法访问*/
	DWORD dwAlarmInputNumber;/*按位,第 0 位对应第 0 个输入端口,dwAlarmType 为 0 时
							 需要设置*/
	DWORD dwAlarmOutputNumber;/*按位,第 0 位对应第 0 个输出端口, */
	DWORD dwAlarmRelateChannel;/*按位，第 0 位对应第 0 个通道*/
	DWORD dwChannel;/*按位,第 0 位对应第 0 个通道， dwAlarmType 为 2 或 3,6 时需要设
					置, */
	DWORD dwDiskNumber;/*按位,第 0 位对应第 0 个硬盘,dwAlarmType 为 4,5 时需要设置
					   */
}INTER_ALARMINFO, *LPINTER_ALARMINFO;


///////////////////////////////////////////////////////////////
#define SDK_V13
typedef struct{
	//开始时间
	BYTE byStartHour;
	BYTE byStartMin;
	//结束时间
	BYTE byStopHour;
	BYTE byStopMin;
} INTER_SCHEDTIME,*LPINTER_SCHEDTIME;
/*异常处理方式*/
//#define NOACTION 0x0 /*无响应*/
//#define WARNONMONITOR 0x1 /*监视器上警告*/
//#define WARNONAUDIOOUT 0x2 /*声音警告*/
//#define UPTOCENTER 0x4 /*上传中心*/
//#define TRIGGERALARMOUT 0x8 /*触发报警输出*/
typedef struct
{
	DWORD dwHandleType; //异常处理,异常处理方式的"或"结果
	DWORD dwAlarmOutTriggered; //触发的报警输出，按位，最大 32 个输出
}INTER_HANDLEEXCEPTION;

typedef struct { /* Each day shares the same motion detection parameter */
	DWORD dwMotionScope[18]; /*侦测区域,0-21 位,表示 22 行,共有 22*18 个小宏
	块,为 1 表示是移动侦测区域,0-表示不是*/
	BYTE byMotionSenstive; /*移动侦测灵敏度, 0 - 5,越高越灵敏,oxff 关闭*/
	BYTE byEnableHandleMotion; /* 是否处理移动侦测 0－否 1－是*/
	char reservedData[2];
	INTER_HANDLEEXCEPTION struMotionHandleType; /* 处理方式 */
#ifdef SDK_V13
	INTER_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT]; /*布防时
	间*/
#endif
	DWORD dwRelRecordChan; /* 报警触发的录象通道 按位 */
	}INTER_MOTION,*LPINTER_MOTION;
	typedef struct{
	DWORD dwEnableHideAlarm; /* 是否启动遮挡报警 ,0-否,1-是*/
	WORD wHideAlarmAreaTopLeftX; /* 遮挡区域的 x 坐标 */
	WORD wHideAlarmAreaTopLeftY; /* 遮挡区域的 y 坐标 */
	WORD wHideAlarmAreaWidth; /* 遮挡区域的宽 */
	WORD wHideAlarmAreaHeight; /*遮挡区域的高*/
	INTER_HANDLEEXCEPTION strHideAlarmHandleType; /* 处理方式 */
#ifdef SDK_V13
	INTER_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT]; /*布防时间*/
#endif
}INTER_HIDEALARM,*LPINTER_HIDEALARM;

//信号丢失报警
typedef struct{
	DWORD dwEnableVILostAlarm; /* 是否启动信号丢失报警 ,0-否,1-是*/
	INTER_HANDLEEXCEPTION strVILostAlarmHandleType; /* 处理方式 */
#ifdef SDK_V13
	INTER_SCHEDTIME struAlarmTime[MAX_DAYS][MAX_TIMESEGMENT]; /*布防时 间*/
#endif
}INTER_VILOST,*LPINTER_VILOST;

typedef struct
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN]; /* 通道名 */
	DWORD dwVideoFormat; /* 视频制式 */
	BYTE byBrightness; /*亮度,传索引*/
	BYTE byContrast; /*对比度,传索引*/
	BYTE bySaturation; /*饱和度,传索引 */
	BYTE byHue; /*色调,传索引*/
	//显示通道名
	DWORD dwShowChanName; /* 预览的图象上是否显示通道名称,0-不显示,1-显示 */
	WORD wShowNameTopLeftX; /* 通道名称显示位置的 x 坐标 */
	WORD wShowNameTopLeftY; /* 通道名称显示位置的 y 坐标 */
	//信号丢失报警
	INTER_VILOST struVILost;
	//移动侦测
	INTER_MOTION struMotion;
	//遮挡报警
	INTER_HIDEALARM struHideAlarm;
	//遮挡
	DWORD dwEnableHide; /* 是否启动遮挡 ,0-否,1-是*/
	WORD wHideAreaTopLeftX; /* 遮挡区域的 x 坐标 */
	WORD wHideAreaTopLeftY; /* 遮挡区域的 y 坐标 */
	WORD wHideAreaWidth; /* 遮挡区域的宽 */
	WORD wHideAreaHeight; /*遮挡区域的高*/
	//OSD
	DWORD dwShowOsd; /* 预览的图象上是否显示 OSD,0-不显示,1-显示 */
	WORD wOSDTopLeftX; /* OSD 的 x 坐标 */
	WORD wOSDTopLeftY; /* OSD 的 y 坐标 */
	BYTE byOSDType; /* OSD类型(主要是年月日格式) */
					/* 0: XXXX-XX-XX 年月日 */
					/* 1: XX-XX-XXXX 月日年 */
					/* 2: XXXX年XX月XX日 */
					/* 3: XX月XX日XXXX年 */
					/* 4: XX-XX-XXXX 日月年*/
					/* 5: XX 日 XX 月 XXXX 年 */
	BYTE byDispWeek; /* 是否显示星期 */
	BYTE byOSDAttrib; /* OSD 属性:透明，闪烁 */
	/*
#define OSD_DISABLE 0 //不显示 OSD
#define OSD_TRANS_WINK 1 //透明，闪烁
#define OSD_TRANS_NO_WINK 2 //透明，不闪烁
#define OSD_NO_TRANS_WINK 3 //不透明，闪烁
#define OSD_NO_TRANS_NO_WINK 4 //不透明，不闪烁
	*/
	char reservedData2;
}INTER_PICCFG,*LPINTER_PICCFG;

#define MAX_SHELTERNUM 4	

typedef struct _INTER_SHELTER
{
	WORD wHideAreaTopLeftX; /* 遮挡区域的x坐标 */
	WORD wHideAreaTopLeftY; /* 遮挡区域的y坐标 */
	WORD wHideAreaWidth; /* 遮挡区域的宽 */
	WORD wHideAreaHeight; /*遮挡区域的高*/
}INTER_SHELTER,*LPINTER_SHELTER;

typedef struct _INTER_PICCFG_EX
{
	DWORD dwSize;
	BYTE sChanName[NAME_LEN]; 
	DWORD dwVideoFormat; /*只读 视频制式 1-NTSC 2-PAL */
	BYTE byBrightness; /*亮度,0-255*/
	BYTE byContrast; /*对比度,0-255*/
	BYTE bySaturation; /*饱和度,0-255 */
	BYTE byHue; /*色调,0-255*/
	//显示通道名
	DWORD dwShowChanName; // 预览的图象上是否显示通道名称,0-不显示,1-显示
	WORD wShowNameTopLeftX; /* 通道名称显示位置的x坐标 */
	WORD wShowNameTopLeftY; /* 通道名称显示位置的y坐标 */
	//信号丢失报警
	INTER_VILOST struVILost;
	//移动侦测
	INTER_MOTION struMotion;
	//遮挡报警
	INTER_HIDEALARM struHideAlarm;
	//遮挡
	DWORD dwEnableHide; /* 是否启动遮挡 ,0-否,1-是*/
	INTER_SHELTER struShelter[MAX_SHELTERNUM];
	//OSD
	DWORD dwShowDate;// 预览的图象上是否显示日期
	WORD wShowDateX; /* 时期显示的x坐标 */
	WORD wShowDateY; /* 时期显示的y坐标 */
	BYTE byDateType; /* 日期格式(主要是年月日格式) */
					/* 0: XXXX-XX-XX 年月日 */
					/* 1: XX-XX-XXXX 月日年 */
					/* 2: XXXX年XX月XX日 */
					/* 3: XX月XX日XXXX年 */
					/* 4: XX-XX-XXXX 日月年*/
					/* 5: XX日XX月XXXX年 */
	BYTE byDispWeek; /* 是否显示星期 */
	BYTE byOSDAttrib; /* OSD属性:透明，闪烁 */
						/* 1: 透明，闪烁 */
						/* 2: 透明，不闪烁 */
						/* 3: 不透明，闪烁 */
						/* 4: 不透明，不闪烁 */
	char byTimeFmt;	//时间格式0-24,1-12
}INTER_PICCFG_EX,*LPINTER_PICCFG_EX;


////////////////////////////实时图像参数///////////////////////////////////////
typedef struct {
	BYTE byBrightness; /*亮度,传索引*/
	BYTE byContrast; /*对比度,传索引*/
	BYTE bySaturation; /*饱和度,传索引 */
	BYTE byHue; /*色调,传索引*/
}INTER_VIDEOPARA, *LPINTER_VIDEOPARA;

typedef struct{
	DWORD	unknown0;
	BYTE	unknown1;
	BYTE	unknown2;
	BYTE	unknown3;
	BYTE	unknown4;
	BYTE	unknown5;
	BYTE	res1[3];
	BYTE	unknown6;
	BYTE	res2[7];
	BYTE	unknown7;
	BYTE	res3[7];
	DWORD	unknown8;
	DWORD	unknown9;
	BYTE	res4[16];
	DWORD	unknown10;
	BYTE	res5[16];
	BYTE	unknown11;
	BYTE	unknown12; 
	BYTE	unknown13;
	BYTE	unknown14;
	DWORD	unknown15;
	BYTE	res6[4];
	DWORD	unknown16;
	BYTE	res7[20];
	DWORD	unknown17;
	BYTE	res8[6];
	BYTE	Mirror;
	BYTE	res9;
	DWORD	unknown18;
	BYTE	res10[28];
	DWORD	unknown19;
	DWORD	unknown20;
	BYTE	res11[12];
	BYTE	spin;
	BYTE	res12[3];
	BYTE	res13[12];
	DWORD	unknown21;
	DWORD	unknown22;
	BYTE	res14[4];
	DWORD	unknown23;
	DWORD	unknown24;
	BYTE	res15[288];
}INTER_COLORENHANCE, *LINTER_COLORENHANCE;

typedef struct { 
	BYTE sNTPServer[64];    	/* Domain Name or IP addr of NTP server */ 
	WORD wInterval;    		/* adjust time interval(hours) */ 
	BYTE byEnableNTP; 			/*enable NPT client */ 
	signed char cTimeDifferenceH; /* 与国际标准时间的时差 -12 ... +13 */ 
	signed char cTimeDifferenceM; 
    BYTE res1;
    WORD wNtpPort;			//NTP服务器端口，设备默认为123 
    BYTE res2[8];
}INTER_NTPPARA, *LPINTER_NTPPARA; 

/*时区和夏时制参数*/
typedef struct 
{
    DWORD dwMonth;  //月 0-11表示1-12个月
    DWORD dwWeekNo;  //第几周：0－第1周；1－第2周；2－第3周； 3－第4周；4－最后一周 
    DWORD dwWeekDate;  //星期几：0－星期日；1－星期一；2－星期二；3－星期三；4－星期四；5－星期五；6－星期六
    DWORD dwHour; //小时，开始时间取值范围0－23； 结束时间取值范围1－23 
    DWORD dwMin; //分0－59
}INTER_TIMEPOINT;

typedef struct 
{
    DWORD				dwSize;			//本结构大小
    int					nTimeZone;		//时区
    BYTE                byDstMode;		//夏令时模式，0-默认，1-自定义
    BYTE                byStartDst;     //是否已经开始执行Dst;
    BYTE				byRes1[10];		//保留
    DWORD				dwEnableDST;	//是否启用夏时制 0－不启用 1－启用
    BYTE				byDSTBias;		//夏令时偏移值，30min, 60min, 90min, 120min, 以分钟计，传递原始数值
    BYTE				byRes2[3];		//保留
    INTER_TIMEPOINT		stBeginPoint;	//夏时制开始时间
    INTER_TIMEPOINT		stEndPoint;		//夏时制停止时间
}INTER_DSTPARA, *LPINTER_DSTPARA;

typedef struct{
	UINT32	U32IpV4;
	BYTE  	byRes[20]; 
}INTER_NET_IPADDR,*LINTER_NET_IPADDR;

typedef struct{
	INTER_NET_IPADDR strIPAddr;
	INTER_NET_IPADDR strIPMask;
	DWORD	dwNetInterface;
	WORD	wDataPort;
    WORD	wMTU;
	BYTE	macAddr[6];
	BYTE	res2[2];
}INTER_NET_ETHERNET;

typedef struct{
	DWORD	dwSize;
    INTER_NET_ETHERNET    stEtherNet[MAX_ETHERNET];
	BYTE	res1[64];
	BYTE	res2[14];
    BYTE    byIpV4Enable;          
    BYTE    byIpV6Enable;  
	WORD	wMulticastSearchEnable; //0x0101启用,0x0202关闭
	BYTE	res3[2];
	BYTE	res4[4];
    INTER_NET_IPADDR    stDnsServer1IpAddr;
    INTER_NET_IPADDR    stDnsServer2IpAddr;                               
	BYTE	res5[64];
	BYTE	res6[2];
    WORD    wHttpPort;	//Http端口    
	BYTE	res7[4];
    INTER_NET_IPADDR    stMulticastIpAddr;    
    INTER_NET_IPADDR    stGatewayIpAddr;
	BOOL	dwPPPoEEnable;
	char 	csPPPoEUser[32];
	char	csPPPoEPassword[32];
	BYTE	res8[36];
}INTER_NET_ETHER_CFG, *LINTER_NET_ETHER_CFG;

typedef struct{
	DWORD	dwSize;
	BYTE	byEnableFTP;
	char 	res1[2];
	BYTE	byFTPPort;
	char	csFTPIpAddress[MAX_DOMAIN_NAME];
	char    csUserName[NAME_LEN];
	char    csPassword[PASSWD_LEN];
	char	byCustomTopDirMode[MAX_DOMAIN_NAME]; //自定义一级目录
	char	byCustomSubDirMode[MAX_DOMAIN_NAME]; //自定义二级目录
	BYTE	byDirMode;		//目录结构,0:保存在根目录,1:使用一级目录,2:使用二级目录
	BYTE	byTopDirMode;	//一级目录,1:使用设备名,2:使用设备号,3:使用设备IP,6:自定义
	BYTE	bySubDirMode;	//二级目录,1:使用通道名,2:使用通道号,3:自定义
	char 	res3;
	BYTE	byIsAnonymity;	//是否匿名
	BYTE	byServerType;	//服务器类型
	char 	res4[2];
	char 	res5[252];	
}INTER_NET_FTP_CFG, *LINTER_NET_FTP_CFG;

typedef struct{
	DWORD	dwSize;
	WORD	wPort;			//RTSP端口
	BYTE	byReserve[54];
}INTER_NET_RTSP_CFG,*LINTER_NET_RTSP_CFG;

typedef struct{
	DWORD	length;	//文件头的长度
	BYTE	byFileHead[40]; //文件头
	BYTE	byReserve[4];
}INTER_NET_P2PTCP_CFG,*LINTER_NET_P2PTCP_CFG;

typedef struct{
	DWORD	dwSize;
	BYTE	res1;
	BYTE	res2[3];
	BYTE	res3;
	BYTE	res4;
	BYTE	res5[2];
	BYTE	res6[64];
}NET_UNKNOWN113305;


#endif

