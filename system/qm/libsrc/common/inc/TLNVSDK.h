/******************************************************************************
******************************************************************************/

#ifndef __TLNVSDK_H__
#define __TLNVSDK_H__

#define  TL_MSG_VIDEOLOST		0x20000001  
#define  TL_MSG_MOVEDETECT              0x20000002  
#define  TL_MSG_SENSOR_ALARM            0x20000003  
#define  TL_MSG_RESETSERVER             0x20000004  
#define  TL_MSG_JPEGSNAP                0x20000005  
#define  TL_MSG_UPGRADE                 0x20000006  
#define  TL_MSG_CRCERROR                0x20000007  
#define  TL_MSG_SERVER_BUSY             0x20000008  
#define  TL_MSG_SERVER_BREAK            0x20000009  
#define  TL_MSG_CHANNEL_BREAK           0x2000000A  
#define  TL_MSG_TALK_REQUEST		0x2000000B	
#define  TL_MSG_UPGRADEOK            	0x2000000C  
#define	 TL_MSG_VIDEORESUME		0x2000000D	
#define	 TL_MSG_COMDATA			0x2000000E	
#define	 TL_MSG_USERDATA		0x2000000F	
#define	 TL_MSG_MOVERESUME		0x20000010
#define  TL_MSG_ALARMSNAP                0x20000011 
#define	 TL_MSG_FILE_DATA		0x20000012
#define	 TL_MSG_DISK_ERROR		0x20000013
#define	 TL_MSG_SENSOR_RESUME			0x20000020 //̽ͷ?????ָ?
#define  TL_MSG_ITEV_ALART				0x20000022   //?ܽ????ֱ???
#define  TL_MSG_ITEV_RESUME				0x20000023	 //?ܽ????ֱ???????

#define  TL_MSG_ITEV_TRIPWIRE_ALART				0x20000024   //???߱???
#define  TL_MSG_ITEV_TRIPWIRE_RESUME				0x20000025	 //???߱???????

#define  TL_MSG_ITEV_ANOMALVIDEO_ALART				0x20000026   //??Ƶ?쳣????
#define  TL_MSG_ITEV_ANOMALVIDEO_RESUME				0x20000027	 //??Ƶ?쳣????????


#define TL_SYS_FLAG_TCP_REVERSE				0x00000001	
//#define TL_SYS_FLAG_EMAIL				0x00000002	
#define TL_SYS_FLAG_RTP					0x00000004	
#define TL_SYS_FLAG_LOGO				0x00000008	
#define TL_SYS_FLAG_PPPOE_DDNS				0x00000010	


#define	TL_SYS_FLAG_ENCODE_D1			0x00000001	
#define	TL_SYS_FLAG_HD_RECORD			0x00000002	
#define	TL_SYS_FLAG_CI_IDENTIFY			0x00000004	
#define	TL_SYS_FLAG_MD_RECORD			0x00000008	
#define TL_SYS_FLAG_DECODE_MPEG4		0x00000010	
#define TL_SYS_FLAG_DECODE_H264			0x00000020	



#define     CPUTYPE_Q3F         1006
/************************************************************************/


/************************************************************************/
#define	TL_SYS_FLAG_ENCODE_D1			0x00000001
#define	TL_SYS_FLAG_HD_RECORD			0x00000002	
#define	TL_SYS_FLAG_CI_IDENTIFY			0x00000004	
#define	TL_SYS_FLAG_MD_RECORD			0x00000008	
#define TL_SYS_FLAG_DECODE_MPEG4			0x00000010	
#define TL_SYS_FLAG_DECODE_H264			0x00000020
#define TL_SYS_FLAG_ITEV				0x00000040  //???????ܷ???
#define TL_SYS_FLAG_VIDEO_ALARM_MODE2		0x00010000  
#define TL_SYS_FLAG_FTPUP			    0x00020000 
#define TL_SYS_FLAG_EMAIL			    0x00040000
#define TL_SYS_FLAG_WIFI			    0x00080000 /* WLAN */
#define TL_SYS_FLAG_PERIPH				0x00100000	//֧??????̽????
#define TL_SYS_FLAG_TELALARM			0x00200000	//֧?ֵ绰????
#define TL_SYS_FLAG_TEMPHUM				0x00400000	//֧????ʪ?ȣ???Դ
#define TL_SYS_FLAG_UPNP				0x00800000
#define QMAPI_SYS_FLAG_PLATFORM           0x01000000 /* ֧??ƽ̨????*/

#define TL_SYS_FLAG_3G			    0x02000000 /* 3G */

#define TL_SYS_FLAG_ITEV				0x00000040  //???????ܷ???

typedef enum
{
   RRC_MODE_NOREC = 0,
   REC_MODE_REALTIME = 1,
   REC_MODE_NONREALTIME =2,
   REC_MODE_ALARM = 3,
}record_mode_t;

typedef struct  _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}SYSTEMTIME;

typedef struct tagTL_SERVER_MSG{
	DWORD		dwMsg;  	///Message Header
	DWORD		dwChannel;	///channel
	SYSTEMTIME	st;
	DWORD		dwDataLen;
}TL_SERVER_MSG , *PTL_SERVER_MSG;	


typedef struct  tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
}RECT;

typedef struct tagTL_SERVER_STATE
{
	DWORD	dwSize;
	DWORD	dwAlarmInState;
	DWORD	dwAlarmOutState;
	DWORD	dwVideoState;
	DWORD	dwServerError;
	DWORD	dwLinkNum;
	DWORD	dwClientIP[10];
}TL_SERVER_STATE,*LPTL_SERVER_STATE;


typedef struct tagTLNV_SERVER_INFO
{
	DWORD		dwSize;			//结构大小
	DWORD		dwServerFlag;		//服务器类型
	DWORD		dwServerIp;		//服务器IP(整数表示形式)
	char			szServerIp[16];	//服务器IP(字符串表示形式)
	WORD		wServerPort;		//服务器端口
	WORD		wChannelNum;	//通道数量
	DWORD		dwVersionNum;	//版本
	char			szServerName[32];	//服务器名称
	DWORD		dwServerCPUType;	//当前CPU类型
	char			szServerSerial[48];	//服务器序列号，具有唯一标识功能
	BYTE			byMACAddr[6];	//服务器的物理地址
	DWORD		dwAlarmInCount;
	DWORD		dwAlarmOutCount;
	DWORD		dwSysFlags;		//系统支持的功能
	DWORD		dwUserRight;		//当前用户权限
	DWORD		dwNetPriviewRight;//网络观看权限
	char			csP2pID[32]; 		//P2P ID
	char			csServerRes[32];
}TLNV_SERVER_INFO,*PTLNV_SERVER_INFO;

typedef struct tagTLNV_CHANNEL_INFO
{
	DWORD   dwSize;
	DWORD   dwStream1Height;
	DWORD   dwStream1Width;
	DWORD   dwStream1CodecID;	//MPEG4Ϊ0JPEG2000Ϊ1,H264Ϊ2
	DWORD   dwStream2Height;
	DWORD   dwStream2Width;
	DWORD   dwStream2CodecID;	//MPEG4Ϊ0JPEG2000Ϊ1,H264Ϊ2
	DWORD   dwAudioChannels;
	DWORD   dwAudioBits;
	DWORD   dwAudioSamples;
	DWORD   dwAudioFormatTag;	//MP3Ϊ0x55G722Ϊ0x65
	char	csChannelName[32];
}TLNV_CHANNEL_INFO,*PTLNV_CHANNEL_INFO;

typedef struct _MSG_HEAD
{
	unsigned long	clientIP;
	unsigned long	clientPort;
	char		szUserName[32];
	char		szUserPsw[32];
	int		nLevel;
	int             nID;
	int             nCommand;
	unsigned long   nChannel;
	unsigned long   nErrorCode;//?????洦????
	int             nBufSize;
}MSG_HEAD,*PMSG_HEAD;


typedef struct tagTL_SENSOR_INFO
{
	DWORD	dwSize;
	DWORD	dwIndex;
	DWORD	dwSensorType;
	char	csSensorName[32];
}TL_SENSOR_INFO,*PTL_SENSOR_INFO;

typedef struct tagTL_CHANNEL_LOGO
{
	DWORD	dwSize;
	DWORD	dwChannel;
	BOOL	bEnable;		//
	DWORD	dwLogox;
	DWORD	dwLogoy;
	BYTE	bLogoData[20*1024];	//
}TL_CHANNEL_LOGO;

typedef struct tagTL_CHANNEL_SHELTER
{
	unsigned long	dwSize;
	unsigned long	dwChannel;
	unsigned int	bEnable;		//
	RECT	rcShelter[5];
}TL_CHANNEL_SHELTER , *PTL_CHANNEL_SHELTER;

typedef struct tagTL_SET_TIME{
	WORD	bHour;
	WORD	bMinute;
}TL_SET_TIME;

typedef struct tagTL_SCHED_TIME
{
	BOOL	bEnable;		//
	TL_SET_TIME	BeginTime1;		//
	TL_SET_TIME	EndTime1;
	TL_SET_TIME	BeginTime2;		//
	TL_SET_TIME	EndTime2;
}TL_SCHED_TIME;


typedef struct tagTL_CHANNEL_MOTION_DETECT{
	DWORD			dwSize;
	DWORD			dwChannel;				//
	BOOL			bEnable;				//
	DWORD			nDuration;				//
	BOOL			bShotSnap;				//
	BOOL			bAlarmIOOut[4];			//
	DWORD			dwSensitive;				//
	TL_SCHED_TIME	tlScheduleTime[8];		//
	BYTE			pbMotionArea[44*36];	//
}TL_CHANNEL_MOTION_DETECT,*PTL_CHANNEL_MOTION_DETECT;

typedef struct tagTL_PRESET_CHANNEL{
WORD wPresetPoint1;
WORD wPresetPoint2;
}TL_PRESET_CHANNEL;

typedef struct tagTL_SENSOR_ALARM
{
	DWORD			dwSize;
	int				nSensorID;				//
	DWORD			dwSensorType;			//
	char				szSensorName[32];		//
	BOOL			bEnable;				//
	int				nDuration;				//
	BOOL			bAlarmIOOut[4];			//
	TL_PRESET_CHANNEL			bPresetChannel[4];		//
	TL_SCHED_TIME	tlScheduleTime[8];		//
	BOOL			bSnapShot;
	int				nCaptureChannel;		//
	BOOL			bReQuestTalkback;
}TL_SENSOR_ALARM,*PTL_SENSOR_ALARM;


typedef struct tagTLNV_ALARM_OUT
{
	DWORD	dwAlarmOutType;
	char	szAlarmOutName[32];
}TLNV_ALARM_OUT,*PTLNV_ALARM_OUT;

typedef struct tagTLNV_SENSOR_STATE{
	DWORD	dwSize;
	DWORD	dwSensorID;
	DWORD	dwSensorOut;
}TLNV_SENSOR_STATE;


typedef struct tagTLNV_ALARM_OUT_INFO{
	DWORD			dwSize;
	TLNV_ALARM_OUT tlao[4];			
}TLNV_ALARM_OUT_INFO,*PTLNV_ALARM_OUT_INFO;


typedef struct tagTL_CHANNEL_VIDEO_LOST
{
	DWORD			dwSize;
	DWORD			dwChannel;					//
	BOOL			bEnable;					//
	int				nDuration;
	BOOL			bAlarmIOOut[4];				//
	TL_SCHED_TIME   	tlScheduleTime[8];			//
}TL_CHANNEL_VIDEO_LOST,*PTL_CHANNEL_VIDEO_LOST;

typedef enum
{
	b_1200 = 0,
	b_2400,
	b_4800,
	b_9600,
	b_19200,
	b_38400,
	b_43000,
	b_56000,
	b_57600,
	b_115200,
}baudrate_t; 

// 485 
typedef struct tagTL_SERVER_COMINFO_V1
{
	DWORD		dwSize;
	DWORD		dwChannel;		//ͨ?\C0\BA?
	DWORD		dwBaudRate;		//?????? 1200,2400,4800,9600,19200,38400,43000,56000,57600,115200
	int			nDataBits;		//????λ
	int			nStopBits;		//ֹͣλ
	int			nParity;		//У??λ
	int			nStreamControl;	//??????
	int			nPrePos;		//??̨Ԥ??λ
	int			nCruise;		//??̨Ѳ??
	int			nTrack;			//??̨?켣
	int			nPTZSpeed;		//??̨?ٶ?
	int			nAddress;		//??̨??ַ
	char		szPTZName[32];	//??̨Э??????
}TL_SERVER_COMINFO_V1,*PTL_SERVER_COMINFO_V1;

typedef struct tagTL_SERVER_COMINFO_V2
{
	DWORD		dwSize;
	DWORD		dwChannel;		//ͨ?\C0\BA?
	DWORD		dwBaudRate;		//?????? 1200,2400,4800,9600,19200,38400,43000,56000,57600,115200
	int			nDataBits;		//????λ
	int			nStopBits;		//ֹͣλ
	int			nParity;		//У??λ
	int			nStreamControl;	//??????
	int			nPrePos;		//??̨Ԥ??λ
	int			nCruise;		//??̨Ѳ??
	int			nTrack;			//??̨?켣
	int			nHSpeed;		//??̨H?ٶ?
	int			nAddress;		//??̨??ַ
	char		szPTZName[32];	//??̨Э??????
	int			nVSpeed;		//??̨V?ٶ?
}TL_SERVER_COMINFO_V2,*PTL_SERVER_COMINFO_V2;


//232
typedef struct tagTL_SERVER_COM2INFO
{
	DWORD		dwSize;
	DWORD		dwChannel;	
	DWORD		eBaudRate;	
	int			nDataBits;	/* ?????м?λ 0??4λ??1??5λ??2??6λ??3??7λ 4??8λ*/
	int			nStopBits;	 /* ֹͣλ 0??1λ??1??2λ  */
	int			nParity;		/* У?? 0????У?飬1????У?飬2??żУ??; */
	int			nStreamControl;	 // 0: ?? 1:Ӳ?? 2:??
	BOOL		bTransferState;	//
}TL_SERVER_COM2INFO,*PTL_SERVER_COM2INFO;


typedef struct tagTL_UPLOAD_PTZ_PROTOCOL{
	DWORD		dwSize;
	DWORD		dwChannel;	
	char		szPTZName[32];
	char		szPTZData[4*1024];
}TL_UPLOAD_PTZ_PROTOCOL,*PTL_UPLOAD_PTZ_PROTOCOL;

//
typedef struct tagTL_COM_DATA
{
	DWORD		dwSize;
	DWORD		dwChannel;
	BOOL		bIs485;			//485 or 232
	char		DataBuf[128];		//
}TL_COM_DATA;



typedef struct tagTL_SERVER_NETWORK
{
	DWORD		dwSize;
	DWORD		dwNetIpAddr;			//IP??ַ
	DWORD       dwNetMask;				//????
	DWORD       dwGateway;				//????
	BYTE		bEnableDHCP;			//
	BYTE		bEnableAutoDNS;
	BYTE		bReserve;			//????
	BYTE		bVideoStandard;			//0 - NTSC, 1 - PAL
	DWORD       dwHttpPort;				//Http?˿?
	DWORD       dwDataPort;				//???ݶ˿?
	DWORD		dwDNSServer;			//DNS??????
	DWORD		dwTalkBackIp;			//???????澯ʱ???Զ?\C1\AC?ӵĶԽ?IP
	char        szMacAddr[6];			//????MAC??ַ
	char		szServerName[32];		//??????????
}TL_SERVER_NETWORK,*PTL_SERVER_NETWORK;

typedef struct tagTL_SERVER_USER{
	DWORD		dwSize;
	DWORD		dwIndex;
	BOOL		bEnable;
	char		csUserName[32];
	char		csPassword[32];
	DWORD		dwUserRight;		
	DWORD		dwNetPreviewRight;	
	DWORD		dwUserIP;			
	BYTE byMACAddr[6];				
}TL_SERVER_USER,*PTL_SERVER_USER;


///zhang added for guo's ftp upload 2009.7.8
///zhang modified 2009.7.10
/* ftp?ϴ?????*/
typedef struct tagTL_FTPUPDATA_PARAM
{
	DWORD	dwSize;
	DWORD 			dwEnableFTP; 		/*?Ƿ?????ftp?ϴ?????*/
	char 			csFTPIpAddress[32]; /*ftp ??????*/
	DWORD 			dwFTPPort; 			/*ftp?˿?*/
	BYTE 			sUserName[32]; 		/*?û???*/
	BYTE 			sPassword[32]; 		/*????*/
	WORD  			wTopDirMode; 		/*0x0 = ʹ???豸ip??ַ,0x1 = ʹ???豸??,0x2 = OFF*/
	WORD  			wSubDirMode; 		/*0x0 = ʹ??ͨ?\C0\BA? ,0x1 = ʹ??ͨ????,0x2 = OFF*/
	//  2009-7-6 Edit by yumy
	// BYTE  reservedData[28]; //????
	BOOL 			bAutoUpData; 		//?Ƿ??????Զ??ϴ?ͼƬ????
	DWORD 			dwAutoTime; 		//?Զ???ʱ?ϴ?ʱ??
	DWORD			dwChannel;			//Ҫ?ϴ???ͨ??  2009-7-10 Add yumy
	BYTE 			reservedData[16]; 	//????	
}TL_FTPUPDATA_PARAM, *LPTL_FTPUPDATA_PARAM;

typedef struct tagTL_NOTIFY_SERVER
{
	DWORD		dwSize;
	BOOL		bEnable;	
	DWORD		dwTimeDelay;
	DWORD		dwPort;		
	char        szDNS[128];		
}TL_NOTIFY_SERVER,*PTL_NOTIFY_SERVER;


#if 0
typedef struct tagTL_PPPOE_DDNS_CONFIG
{
	DWORD	dwSize;
	BOOL	bEnablePPPOE;			//0-不启???1-启用
	char	csPPPoEUserName[32];		//PPPoE用户???
	char	csPPPoEPassword[32];		//PPPoE密码
	DWORD	dwPPPoEIP;			//PPPoE IP地址(只读)
	BOOL	bEnableDDNS;			//是否启用DDNS
	char	csDDNSName[64];			//在服务器注册的DDNS名称
	char	csDNSAddress[64];		//DNS服务器地址
	DWORD	dwDNSPort;			//DNS服务器端口，默认???500
}TL_PPPOE_DDNS_CONFIG,*PTL_PPPOE_DDNS_CONFIG;
#endif
typedef struct tagTL_PPPOE_DDNS_CONFIG
{
	DWORD	dwSize;
	BOOL	bEnablePPPOE;				
	char	csPPPoEUserName[32];		
	char	csPPPoEPassword[32];		
	DWORD	dwPPPoEIP;					
	int	bEnableDDNS;				
	CHAR	csDDNSUsername[32];
	CHAR	csDDNSPassword[32];
	char	csDDNSDomain[32];			
	char	csDNSAddress[32];			
	DWORD	dwDNSPort;
}TL_PPPOE_DDNS_CONFIG,*PTL_PPPOE_DDNS_CONFIG;

typedef struct
{
	DWORD	dwSize;
	BYTE byEnableDDNS;
	BYTE byDdnsType;
	char resv[2];
	char	csDDNSUserName[32];		
	char	csDDNSPassword[32];		
	char	csDNSDomain[64];		
	int	updateTime;
	DWORD dwIsMaped;
	char	reserved[28];
}TL_DDNS_CONFIG,*PTL_DDNS_CONFIG;


typedef struct tagTL_CHANNEL_OSDINFO
{
	DWORD	dwSize;
	DWORD	dwChannel;
	BOOL	bShowTime;
	DWORD	dwTimeFormat;
			
	BOOL	bShowString;		
	DWORD	dwStringx;			
	DWORD	dwStringy;
	char	csString[48];		
	char	csOsdFontName[32];
}TL_CHANNEL_OSDINFO,*LPTL_CHANNEL_OSDINFO;
typedef struct
{
    DWORD	dwTimeX;			
	DWORD	dwTimeY;
}TL_TIME_OSD_POS;

typedef struct tagTL_OSD_RECT
{
	unsigned long dwOsdWidth;
	unsigned long dwOsdHeight;
}TL_OSD_RECT;


typedef struct tagTL_SERVER_OSD_DATA{
	unsigned long		dwSize;
	unsigned long		dwChannel;
	TL_OSD_RECT		osdQCifrc;
	unsigned char		osdQCifData[10 * 10 * 24];
	TL_OSD_RECT		osdCifrc;
	unsigned char		osdCifData[16 * 16 * 24];
	TL_OSD_RECT		osd2Cifrc;
	unsigned char		osd2CifData[16 * 32 * 24];
	TL_OSD_RECT		osd4Cifrc;
	unsigned char		osd4CifData[24 * 24 * 24];
}TL_SERVER_OSD_DATA , *LTL_SERVER_OSD_DATA;


//  2009-8-8 Add 
typedef struct tagTL_MOBILE_CENTER_INFO{
	DWORD	dwSize;
	BOOL	bEnable;
	char	szIp[64];					//??????Ip
	DWORD	dwCenterPort;				//???Ķ˿?
	char	csServerNo[64];				//?????????к?
	DWORD	dwStatus;					//??????\C1\AC??״̬ 0Ϊδ\C1\AC?? 1Ϊ\C1\AC???? 2\C1\AC?ӳɹ?
	char	csUserName[32];				//?û???
	char	csPassWord[32];				//????
	BYTE	byAlarmPush;
	BYTE	reservedData[3];
	DWORD	reservedData1;				//????
}TL_MOBILE_CENTER_INFO;




typedef struct __NETVIEW_SERVER_INFO
{
	int dwPackType;
	int dwCmdPort;
	int dwServerType;
	int dwChannelNum;
	int dwVersion;
	int dwWebPort;
	int dwDSPFreq;
	int dwLogonedUser;
	int dwOpenedChannel;
	int dwChannelState;
	int dwAlarmOutState;
	char szServerIp[16];
	char szGetWay[16];
	char szMac[24];
	char szServerName[32];
	char bReserved[32];
}NETVIEW_SERVER_INFO;



typedef struct tagTLNV_SERVER_PACK
{
	char	szIp[16];		//Server Ip
	WORD	wMediaPort;		//Media Port
	WORD	wWebPort;		//Http Port
	WORD	wChannelCount;		//
	char	szServerName[32];	//
	DWORD	dwDeviceType;		//
	DWORD	dwServerVersion;	//
	WORD	wChannelStatic;		//(Is Lost Video)
	WORD	wSensorStatic;		//
	WORD	wAlarmOutStatic;	//
}TLNV_SERVER_PACK;

typedef struct tagTLNV_SERVER_MSG_DATA
{
	DWORD					dwSize; // >= sizeof(TLNV_SERVER_PACK) + 8;
	DWORD					dwPackFlag; // == SERVER_PACK_FLAG
	TLNV_SERVER_PACK			tlServerPack;
	TL_SERVER_MSG				tlMsg;
}TLNV_SERVER_MSG_DATA;

typedef struct tagTLNV_PROTECT_KEY_INFORMATION{
	BYTE byMACAddr[6];
	BYTE Product_Serial_Number[128];
}TLNV_PROTECT_KEY_INFORMATION;

typedef enum {
	COMPANY_JIABOVIDEO = 1,//????
	COMPANY_ANSHIBAO = 2   //???ݰ??ӱ?
}TLNV_COMPANY_TYPE;

typedef enum {
	Hi3510 = 1,
	Hi3511,
	Hi3512,
	Hi3515,
	Hi3515A = 5,
	Hi3520,
	Hi3520A,
	Hi3520D,
	Hi3521,
	Hi3531 = 10,
	Hi3518C,
	Hi3518C_Liu,		//????ʦ?汾??????
	Hi3516A,
	Ti365,
	Ti368 = 15
}TLNV_CPU_TYPE;

typedef enum {
    IMAGE_704x576 = 0,
	IMAGE_800x600 = 1,
	IMAGE_100W,
	IMAGE_130W,
	IMAGE_200W
}TLNV_IMAGE_SIZE;

typedef enum {
	PCB_COMMON = 0x00,
	PCB_PT = 0x01,
	PCB_BUTT
}TLNV_PCB_TYPE;

typedef struct tagTLNV_SYSTEM_INFO{
    WORD cbSize;          // 结构大小 == sizeof(TLNV_SYSTEM_INFO)
    BYTE byType;          // 类型 普通IPC/全景单目IPC/全景双目IPC/NVR等
    BYTE byManufacture;   // 厂商代码
    DWORD dwSerialNumber; // 序号，顺序递增
    BYTE byHardware[4];   // SOC+MEM+FLASH+版本号
    BYTE byChannelNum;    // 通道号
    BYTE byAudioInputNum; // 音频数
    BYTE byAlarmInputNum; // 报警输入数
    BYTE byAlarmOutputNum; // 报警输出数
    DWORD dwExtendInfo;    // 扩展信息
    BYTE byIssueDate[4];   // 日期
    BYTE byP2PID[32];      // P2PID
    BYTE byMac[6];         // MAC地址
    WORD woCheckSum;       // 数据校验和
    BYTE byReserve[64];    // 保留为0
}TLNV_SYSTEM_INFO;

typedef struct {
	WORD woSize; 			//结构大小
	WORD woDataSize;		//密文长度
	BYTE	 byCipherData[256];	//密文数据
} TLNV_ID_CIPHER;

typedef struct tagTL_SERVER_RECORD_SET{
	DWORD	dwSize;
	DWORD	dwLocalRecordMode;	/*//¼??ģʽ??0 ??¼????1??ȫʵʱ¼????2????ʵʱ¼????3???澯¼??*/
	BOOL	bAutoDeleteFile;		
}TL_SERVER_RECORD_SET;

///zhang added 2009.6.9
typedef struct tagTL_SERVER_RECORD_CONFIG{
	DWORD	dwSize;
	BOOL	bTimeControl;
	BOOL	bSizeControl;
	DWORD	dwRecordTime;		///    minutes(1~600)
	DWORD	dwRecordSize;		///    Mbyte(1~50)
}TL_SERVER_RECORD_CONFIG;
typedef struct tagTL_SERVER_RECORD_STATE{
	DWORD	dwSize;
	DWORD	dwChannelRecordState;	
	DWORD	dwServerSpace;			
	DWORD	dwSpaceFree;			
}TL_SERVER_RECORD_STATE;



typedef struct tagTL_CENTER_INFO{
	DWORD	dwSize;
	BOOL	bEnable;
	DWORD	dwCenterIp;
	DWORD	wCenterPort;
	char	csServerNo[64];
}TLNV_NXSIGHT_SERVER_ADDR;


typedef struct tagTL_CENTER_INFO_EX{
	DWORD	dwSize;
	char  csCenterid[32];
	char  csUsername[32];
	char  csUserpass[32];
	char	csReserver[64];
}TLNV_NXSIGHT_SERVER_ADDR_EX;

typedef struct tagTL_CENTER_INFO_V2{
	DWORD	dwSize;
	BOOL	bEnable;
	DWORD	dwCenterIp; //?????ã?Ϊ??֮ǰ?汾???ݹʱ?????
	DWORD	wCenterPort;
	char	csServerNo[64];
	char csCenterIP[64];
}TLNV_NXSIGHT_SERVER_ADDR_V2;

typedef struct tagTL_NXSIGHT_LICENCE{
	char	csServerNo[1024];
}TLNV_NXSIGHT_LICENCE;
	



/*
typedef struct
{
	char			node_name[64];
	unsigned long		ip_addr;
	unsigned short		port;
}TLNV_NXSIGHT_SERVER_ADDR;
*/




typedef struct tagTL_I2C_CONFIG{
	DWORD	dwSize;
	DWORD	dwDeviceAddress;
	DWORD	dwPageAddress;
	DWORD	dwRegAddress;
	DWORD	dwValue;
}TL_I2C_CONFIG;



//Add 2008-05-14
typedef struct tagTLNV_EMAIL_PARAM
{
	DWORD	dwSize;
	BOOL	bEnableEmail;
	BOOL	bAttachPicture;
	char	csEmailServer[64];
	char	csEMailUser[32];
	char	csEmailPass[32];
	char	csEmailFrom[64];
	char	csEmailTo[128];
	char	csEmailCopy[128];
	char	csEmailBCopy[128];
	char	csReserved[32];
}TLNV_EMAIL_PARAM, *PTLNV_EMAIL_PARAM;


typedef struct
{
	int		sensor_addr;
	float		ftemp;
	float		fhum;
}TLNV_TEMP_HUM;

/* wireless EX*/
typedef struct tagTLTDSCDMAConfig{
	
	DWORD			dwSize;
	unsigned char	     bCdmaEnable;
	unsigned char      byDevType;
	unsigned char      byStatus;
	unsigned char      byReserve;
	DWORD		    dwNetIpAddr;			//IP  (wifi enable)
} TL_TDSCDMA_CONFIG;

typedef struct tagTLWifiConfig
{
	
	DWORD		    dwSize;
	BYTE            bWifiEnable;            // 0: disable 1: stiatic ip 2:dhcp
    BYTE			byWifiMode;		//0:station, 1:ap
    BYTE			byWpsStatus;	//wps״̬(0:?ر?,1:????)
    BYTE            byWifiSetFlag; //0:?ֶ????ù????ɹ???½?? 1:У?? 2:?Զ?????wifi 
	DWORD		    dwNetIpAddr;			//IP  (wifi enable)
	DWORD       	dwNetMask;			
	DWORD       	dwGateway;			
	DWORD		dwDNSServer;			//DNS  (wifi enable)
	char		szEssid[32];	             // (wifi enable)
	unsigned char        nSecurity; 		//0: none  (wifi enable)
						 	// 1:web 64 assii (wifi enable)
						 	// 2:web 64 hex (wifi enable)
						 	// 3:web 128 assii (wifi enable)
						 	// 4:web 128 hex (wifi enable)
						 	//5:  WPAPSK-TKIP 
                            //6: WPAPSK-AES
                            //7: WPA2PSK-TKIP
                            //8: WPA2PSK-AES  
	unsigned char		byNetworkType;  // 1. managed 2. ad-hoc
	BYTE          		byStatus; //	0:	?ɹ?,????ֵ?Ǵ?????
	BYTE          		byRes;	
	char            	szWebKey[32]; //(wifi enable)					 	
}TL_WIFI_CONFIG;

typedef struct
{
	char    		sUserName[32];  //name
	char    		sBirthDay[12];  //birthday
	char    		sNation[128];  //nation
	char    		sCertificateNo[256]; //certificate No.
	char    		sSignDept[256]; //certificate department
	char    		sRegistered[256]; //certificate area
	unsigned short 	nSex;  //sex 
	unsigned short 	nCertificateType; //certificate type
	unsigned short 	nCertificatePeriod; // certificate period of validity
	unsigned short 	nReserve;    //Reserved
}IDENTITY_CARD_INFO;

typedef struct 
{	
	DWORD			dwSize;       //sizeof(TL_CARD_SNAP_PIC_MSG), must be valid
	unsigned  int   		nChannelNO;   //need to link ,0 ~ 3
	IDENTITY_CARD_INFO 	IdInfo;	
} TL_CARD_SNAP_PIC_MSG;



typedef struct tagTL_CHANNEL_CONFIG
{
	DWORD	dwSize;
	DWORD	dwChannel;			//
	//
	DWORD	nFrameRate;		//
	DWORD	nStandard; 			//(0:NTSC 1:PAL)
	DWORD	dwRateType;           	//0:CBR 1:VBR
	DWORD	dwStreamFormat;	// (0ΪCIF,1ΪD1,2ΪHALF-D1,3ΪQCIF)
	DWORD	dwBitRate;			//
	DWORD	dwImageQuality;		//
	DWORD	nMaxKeyInterval;	//
	BOOL	bEncodeAudio;		//
	char	csChannelName[32];		//
}TL_CHANNEL_CONFIG,*LPTL_CHANNEL_CONFIG;
//Add 2008-07-28

///zhang added for guo's uPnP, 2009.6.18
typedef struct	tagTLUPnPConfig
{
	DWORD		dwSize;
	BOOL		bEnable;				/*?Ƿ?????upnp*/
	DWORD		dwMode;				/*upnp??????ʽ.0Ϊ?Զ??˿?ӳ?䣬1Ϊָ???˿?ӳ??*/
	DWORD		dwLineMode;			/*upnp?\F8\BF\A8\B9\A4????ʽ.0Ϊ????????,1Ϊ????????*/
	char		csServerIp[32];			/*upnpӳ??????.??????·????IP*/
	DWORD		dwDataPort;				/*upnpӳ?????ݶ˿?*/
	DWORD		dwWebPort;				/*upnpӳ???????˿?*/
	DWORD		dwMobilePort;         /*upnpӳ???ֻ??˿?*/  
	WORD		dwDataPort1;			/*upnp??ӳ???ɹ??????ݶ˿?*/
	WORD		dwWebPort1;			/*upnp??ӳ???ɹ????????˿?*/
	WORD		dwMobilePort1;      /*upnpӳ???ɹ????ֻ??˿?*/  
	WORD        wDataPortOK;
	WORD        wWebPortOK;
	WORD        wMobilePortOK;
}TL_UPNP_CONFIG;

#define 		TL_SYS_FLAG_UPNP			0x00800000		/*֧??upnp*/
typedef struct{
	uint32_t dwYear;	
	uint32_t dwMonth;
	uint32_t dwDay;
	uint32_t dwHour;
	uint32_t dwMinute;
	uint32_t dwSecond;	
}NET_DVR_TIME,*LPNET_DVR_TIME;

typedef struct tagTLNV_FindFileReq{	/*??ѯ????*/
	DWORD		dwSize;
	DWORD		nChannel;				/*0xff??ȫ??ͨ?\C0\A3?0??1??2 ......*/
	DWORD		nFileType;				/*?ļ????? ??0xff ?? ȫ????0 ?? ??ʱ¼????1 - ?ƶ????⣬2 ?? ??????????3  ?? ?ֶ?¼??*/
	NET_DVR_TIME	BeginTime;
	NET_DVR_TIME	EndTime;			/*StartTime StopTime ??ֵ??Ϊ0000-00-00 00:00:00??ʾ????????ʱ??*/
}TLNV_FIND_FILE_REQ;

typedef struct tagTLNV_FILE_DATA_INFO{
	char		sFileName[256];				/*?ļ???*/
	NET_DVR_TIME	BeginTime;
	NET_DVR_TIME	EndTime;
	DWORD			nChannel;
	DWORD			nFileSize;			/*?ļ??Ĵ?С*/
	DWORD			nState				/*?ļ?ת??״̬*/;		
}TLNV_FILE_DATA_INFO;

typedef struct tagTLNV_FindFileResp{
	DWORD		dwSize;
	DWORD		nResult;		//0:success ;1:find file error ; 2:the number of file more than the memory size, and the result contains part of the data
	DWORD		nCount;			/*???ص??ļ???Ŀ*/
	//char		filedata[1];	/*	TLNV_FILE_DATA[];Ҫ?ú?????,???????Խ??????ṹ?崮??һ??????*/
    DWORD       resv[3];
}TLNV_FIND_FILE_RESP;

typedef struct{
    uint32_t dwYear;	
    uint32_t dwMonth;
	uint32_t dwDay;
	uint32_t dwHour;
	uint32_t dwMinute;
	uint32_t dwSecond;	
}TLNV_TIME,*LPTLNV_TIME;


typedef struct tagTLOPEN_FILE
{	
	DWORD		nFileType;     // //1 open by filename ,2 open by time
	char			csFileName[256];
	DWORD		nChannel;
	DWORD		nProtocolType;
	DWORD		nStreamType;

	TLNV_TIME	struStartTime;
	TLNV_TIME	struStopTime;
}TLNV_OPEN_FILE,*TLNV_POPEN_FILE;
#define TL_VCRACTION_REQUEST     0x10
typedef struct tagTLNV_VODActionReq{
	DWORD		dwSize;
       DWORD          dwVersion; 	//==1
	DWORD		dwCMD; 		//VCRACTION_REQUEST == 0X10
	DWORD          dwAction;	 //1.2......
	DWORD		dwData;
}TLNV_VODACTION_REQ;


#define VIDEO_XVID	0x58564944
#define VIDEO_H264	0x34363248
#define VIDEO_MJPEG 0x47504A4D


typedef struct tagFilePlay_INFO
{
	DWORD   dwSize;
	DWORD   dwStream1Height;			//??Ƶ??(1)
	DWORD   dwStream1Width;			//??Ƶ??
	DWORD   dwStream1CodecID;			//??Ƶ???????ͺţ?MPEG4Ϊ0??JPEG2000Ϊ1,H264Ϊ2??
	DWORD   dwAudioChannels;			//??Ƶͨ????
	DWORD   dwAudioBits;				//??Ƶ??????
	DWORD   dwAudioSamples;			//??Ƶ??????
	DWORD   dwWaveFormatTag;			//??Ƶ???????ͺ?
	char    csChannelName[32];			//ͨ??????
	DWORD	dwFileSize;
	DWORD	dwTotalTime;			/*?ļ?????ʱ?䳤??*/
	DWORD	dwPlayStamp;			/*ʱ????*/
}FilePlay_INFO,*PFilePlay_INFO;

//2009.8.3 zhang add for top-saf DDNS init
#define TOP_SAF_IP_ADDR "203.86.26.156"

typedef struct tagTLNV_PTZ_CONTROL
{
	DWORD		dwSize;
	DWORD		dwChannel;
	DWORD         dwPTZCommand;//PTZCMD_UP .....
	int         		nPTZParam;  // ???ݾ????????ֶ???
	BYTE              byReserve[32];
}TLNV_PTZ_CONTROL;

//?????ؼ?֡
typedef struct tagTL_CHANNEL_FRAMEREQ
{
	DWORD dwSize;
	DWORD dwChannel;
	DWORD dwStreamType;
	DWORD dwFrameType; // 0 - I?????ౣ??
}TL_CHANNEL_FRAMEREQ;


#endif
