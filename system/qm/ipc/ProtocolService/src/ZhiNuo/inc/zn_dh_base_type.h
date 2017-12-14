#ifndef ZN_DH_BASE_TYPE_H
#define ZN_DH_BASE_TYPE_H


//基本宏定义
#define MAX_LINK_NUM  32
#define	DVRIP_VERSION	5
#define RE_CONNECT_TIME                          (10)    //如果登录失败的重连的时间
#define BUFLEN 4024
#define SEND_TIMEOUT                              (15) //socket发送数据的超时时间
#define USER_NAME "admin"
#define PASSWD "admin"
#define MAX_VIDEO_FRAME_SIZE                     (524 * 1024)   //实时视频允许获取的帧的最大字节数
#define MAX_SEND_BUFF_SIZE                       (524 * 1024)   //send的最大发送buf的大小
#define ONE_PACKAGE_SIZE    (50*1024)  
//#define ZHINUO_ONE_PACKAGE_SIZE    (40*1024)  
#define ZHINUO_ONE_PACKAGE_SIZE    (20*1024)  
//#define DAHUA_ONE_PACKAGE_SIZE    (20*1024)  //这个不能设置超过40k不然大华的视频出不来
#define AUDIO_SIZE_PRE_FRAME    (320) //每个音频帧G711A裸流数据的实际大小
#define DEV_NAME "IPC-IPVM3150F" //"IPC_3507_720P"
#define ZHINUO_RCV_PORT 6060
#define ZHINUO_SEND_PORT 6061
#define IFNAME "eth0"
#define DAHUA_RCV_PORT 5050
#define DAHUA_SEND_PORT 5050
#define TCP_LISTEN_PORT 37777
#define ZHINUO_NVR    45
#define DAHUA_NVR    46

#define DEV_FUNCTION_SEARCH "FTP:1:Record,Snap&&SMTP:1:AlarmText,AlarmSnap&&NTP:2:AdjustSysTime&&VideoCover:1:MutiCover&&AutoRegister:1:Login&&AutoMaintain:1:Reboot,DeleteFiles,ShutDown&&UPNP:1:SearchDevice&&DHCP:1:RequestIP&&STORE POSITION:1:FTP&&DefaultQuery:1:DQuery&&ACFControl:1:ACF&&ENCODE OPTION:1:AssiCompression&&DavinciModule:1:WorkSheetCFGApart,StandardGOP&&Dahua.a4.9:1:Login&&Dahua.Device.Record.General:1:General&&IPV6:1:IPV6Config&&Log:1:PageForPageLog&&QueryURL:1:CONFIG&&DriverTypeInfo:1:DriverType&&ProtocolFramework:1:V3_1"

//结构体相关宏
#define MAX_ENC_CHIP_NR 32
#define MD_REGION_ROW 32
#define N_SYS_CH 16
#define EXTRATYPES 3
#define N_COLOR_SECTION 2

//报文命令字
#define CMD_ZHINUO_LOG_ON 0xd2    //登录
#define CMD_DAHUA_LOG_ON 0xa0    //登录
#define CMD_STATUS_SEARCH 0xa1    //查询工作及报警状态
#define CMD_DEV_EVENT_SEARCH 0x68    //查询设备事件
#define CMD_MEDIA_REQUEST 0x11    //媒体数据请求
#define CMD_MEDIA_CAPACITY_SEARCH 0x83    //查询设备媒体能力信息
#define CMD_CONFIG_SEARCH 0xa3    //查询配置参数
#define CMD_CHANNEL_NAME_SEARCH 0xa8    //查询通道名称
#define CMD_SYSTEM_INFO_SEARCH 0xa4    //查询系统信息
#define CMD_CONNECT_REQUEST 0xf1    //请求建立连接
#define CMD_I_FRAME_REQUEST 0x80    //强制I帧
#define CMD_DEV_CONTROL 0x60    //设备操作
#define CMD_SET_CONFIG 0xc1    //修改参数配置
#define CMD_TIME_MANAGE 0x24    //时间管理
#define CMD_SET_CHANNEL_NAME 0xC6    //修改通道名字
#define CMD_PTZ_CONTROL 0x12    //云台控制
#define CMD_TALK_REQUEST 0x1d    //对讲请求
#define CMD_TALK 0xc0    //对讲
#define CMD_TALK_CONTROL 0x1e    //对讲控制
#define CMD_RECORD_SEARCH 0xa5    //录像搜索
#define CMD_RECORD_PLAYBACK 0xc2    //录像回放
#define CMD_RECORD_DOWNLOAD 0xbb    //录像下载
#define CMD_DEV_SEARCH 0xa3    //设备搜索
#define CMD_UNKOWN 0xF6    //未知命令

//回复报文命令字
#define ACK_LOG_ON 0xb0    //登录
#define ACK_STATUS_SEARCH 0xb1    //查询工作及报警状态
#define ACK_DEV_EVENT_SEARCH 0x69    //查询设备事件
#define ACK_MEDIA_REQUEST 0xbc    //媒体数据请求
#define ACK_MEDIA_CAPACITY_SEARCH 0x83    //查询设备媒体能力信息
#define ACK_CONFIG_SEARCH 0xb3    //查询配置参数
#define ACK_CHANNEL_NAME_SEARCH 0xb8   //查询通道名称
#define ACK_SYSTEM_INFO_SEARCH 0xb4    //查询系统信息
#define ACK_CONNECT_REQUEST 0xf1    //请求建立连接
#define ACK_I_FRAME_REQUEST 0x80    //强制I帧
#define ACK_DEV_CONTROL 0x60    //设备操作
#define ACK_SET_CONFIG 0xC1    //修改参数配置
#define ACK_TIME_MANAGE 0x24    //时间管理
#define ACK_SET_CHANNEL_NAME 0xb8   //修改通道名字
#define ACK_PTZ_CONTROL 0x12    //云台控制
#define ACK_TALK_REQUEST 0xc0    //对讲请求
#define ACK_TALK 0x1d    //对讲
#define ACK_TALK_CONTROL 0x1e    //对讲控制
#define ACK_RECORD_SEARCH 0xb6   //录像搜索
#define ACK_RECORD_PLAYBACK 0xc2    //录像回放
#define ACK_RECORD_DOWNLOAD 0xbb    //录像下载
#define ACK_DEV_SEARCH 0xb3    //设备搜索
#define ACK_UNKOWN 0xF6    //未知命令

//扩展帧头命令字
#define EXPAND_PICTURE_SIZE 0x80
#define EXPAND_PLAY_BACK_TYPE 0x81
#define EXPAND_AUDIO_FORMAT 0x83
#define EXPAND_DATA_CHECK 0x88

//帧类型
#define 	FRAME_TYPE_I  0xFD  
#define 	FRAME_TYPE_P  0XFC
#define 	FRAME_TYPE_B  0XFE
#define 	AUDIO_TYPE  0XF0


/////////////////////////////////////////////////////////////////////////////
typedef struct tagZhiNuo_Dev_Search
{
	unsigned char Version[8]; // 8字节的版本信息
	char HostName[16]; // 主机名
	unsigned long HostIP; // IP 地址
	unsigned long Submask; // 子网掩码
	unsigned long GateWayIP; // 网关IP
	unsigned long DNSIP; // DNS IP 40个字节位置

	// 外部接口
	unsigned long	AlarmServerIP; // 报警中心IP
	unsigned short  AlarmServerPort; // 报警中心端口
	unsigned long	SMTPServerIP; // SMTP server IP
	unsigned short  SMTPServerPort; // SMTP server port
	unsigned long	LogServerIP; // Log server IP
	unsigned short  LogServerPort; // Log server port

	// 本机服务端口
	unsigned short  HttpPort; // HTTP服务端口号
	unsigned short  HttpsPort; // HTTPS服务端口号
	unsigned short  TCPPort; // TCP 侦听端口   68个字节位置
	unsigned short  TCPMaxConn; // TCP 最大连接数
	unsigned short  SSLPort; // SSL 侦听端口 72
	unsigned short  UDPPort; // UDP 侦听端口 76
	unsigned long	McastIP; // 组播IP  80
	unsigned short  McastPort; // 组播端口

	// 其他
	unsigned char  MonMode; // 监视协议0-TCP, 1-UDP, 2-MCAST //待确定-TCP
	unsigned char  PlayMode; // 回放协议0-TCP, 1-UDP, 2-MCAST//待确定-TCP 84
	unsigned char  AlmSvrStat; // 报警中心状态0-关闭, 1-打开 88
}ZhiNuo_Dev_Search;


//时间日期结构
typedef struct tagZhiNuo_DateTime
{
	DWORD second : 6;		//	秒	0-59
	DWORD minute : 6;		//	分	0-59
	DWORD hour : 5;		//	时	0-23
	DWORD day : 5;		//	日	1-31
	DWORD month : 4;		//	月	1-12
	DWORD year : 6;		//	年	2000-2063	
}ZhiNuo_DateTime;

//扩展帧头图像尺寸
typedef struct tagZhiNuo_Expand_Picture_Size
{
	unsigned char ch_cmd;
	unsigned char ch_coding_type; //ZhiNuo_coding_type_e
	unsigned char ch_width;
	unsigned char ch_high;
}ZhiNuo_Expand_Picture_Size;

//扩展帧头回放类型。
typedef struct tagZhiNuo_Expand_PlayBack_Type
{
	unsigned char ch_cmd;
	unsigned char ch_reserve;
	unsigned char ch_video_type; //ZhiNuo_video_type_e
	unsigned char ch_frame_rate;
}ZhiNuo_Expand_PlayBack_Type;

//扩展帧头音频格式。
typedef struct tagZhiNuo_Expand_Audio_Format
{
	unsigned char ch_cmd;
	unsigned char ch_audio_channel; //ZhiNuo_audio_channel_e
	unsigned char ch_audio_type;   //ZhiNuo_audio_type_e
	unsigned char ch_sample_frequency; //ZhiNuo_sample_sequence_e
}ZhiNuo_Expand_Audio_Format;

//扩展帧头数据校验。
typedef struct tagZhiNuo_Expand_Data_Check
{
	unsigned char ch_cmd;
	unsigned int  n_check_result;
	unsigned short s_reserve;
	unsigned char ch_check_type; //ZhiNuo_data_check_type_e
}ZhiNuo_Expand_Data_Check;

//音视频报文头结构体 24个字节
typedef struct tagZhiNuo_Media_Frame_Head
{
	unsigned char sz_tag[4];
	unsigned char ch_media_type;
	unsigned char ch_child_type;
	unsigned char ch_channel_num;
	unsigned char ch_child_sequence;
	unsigned int  n_frame_sequence;
	unsigned long   n_frame_len;
	ZhiNuo_DateTime t_date;
	unsigned short s_timestamp;
	unsigned char ch_expand_len;
	unsigned char ch_checksum;
}ZhiNuo_Media_Frame_Head;

typedef struct tagZhiNuo_Media_Frame_Tail
{
	unsigned char sz_tag[4];
	unsigned long  unl_data_len;
}ZhiNuo_Media_Frame_Tail;

//录像下载请求回复结构
typedef struct tagZhiNuo_sVideoRecordDownloadInfo
{
	DWORD  begin_seek;
	DWORD  end_seek;
	DWORD  record_id;
}ZhiNuo_sVideoRecordDownloadInfo;

typedef struct
{
	char				szIP[16];		// IP
	int					nPort;							// 端口
	char				szSubmask[16];	// 子网掩码
	char				szGateway[16];	// 网关
	char				szMac[40];			// MAC地址
	char				szDeviceType[32];	// 设备类型
	BYTE				bReserved[32];					// 保留字节
} ZhiNuo_DEVICE_NET_INFO;

//命令报文头结构体
typedef struct dvrip
{
	unsigned char	cmd;			/* command  */
	unsigned char	dvrip_r0;		/* reserved */
	unsigned char	dvrip_r1;		/* reserved */
	unsigned char	dvrip_hl : 4,		/* header length */
	dvrip_v : 4;		/* version */
	unsigned int	dvrip_extlen;	/* ext data length */
	unsigned char	dvrip_p[24];	/* inter params */
}DVRIP;

typedef union
{
	struct dvrip	dvrip; /* struct def */
	unsigned char	c[32]; /* 1 byte def */
	unsigned short	s[16]; /* 2 byte def */
	unsigned int	l[8];  /* 4 byte def */
}DVRIP_HEAD_T;

#define DVRIP_HEAD_T_SIZE sizeof(DVRIP)
#define ZERO_DVRIP_HEAD_T(X) memset((X), 0, DVRIP_HEAD_T_SIZE);	\
	(X)->dvrip.dvrip_hl = DVRIP_HEAD_T_SIZE / 4; \
	(X)->dvrip.dvrip_v = DVRIP_VERSION;

//编码类型
typedef enum
{
	ONE_FRAME,  //0:编码时只有一场(帧)
	MIX_FRAME,  //1:编码时两场交织
	TWO_FRAME,	 //2:编码时分两场
}ZhiNuo_coding_type_e;

//视频帧类型
typedef enum
{
	MPEG4 = 1,
	H264,
	MPEG4_LB,
	H264_GBE,
	JPEG,
	JPEG2000,
	AVS,
}ZhiNuo_video_type_e;

//音频帧类型
typedef enum
{
	PCM8 = 7,
	G729,
	IMA_ADPCM,
	G711U,
	G721,
	PCM8_VWIS,
	MS_ADPCM,
	G711A,
	PCM16,
}ZhiNuo_audio_type_e;

//音频声道类型
typedef enum
{
	SINGLE_CHANNEL = 1, //单声道
	DOUBLE_CHANNEL,//双声道     
}ZhiNuo_audio_channel_e;

//音频采样率
typedef enum
{
	SAMPLE_FREQ_4000 = 1,
	SAMPLE_FREQ_8000,
	SAMPLE_FREQ_11025,
	SAMPLE_FREQ_16000,
	SAMPLE_FREQ_20000,
	SAMPLE_FREQ_22050,
	SAMPLE_FREQ_32000,
	SAMPLE_FREQ_44100,
	SAMPLE_FREQ_48000,
}ZhiNuo_sample_sequence_e;

typedef enum
{
	check_type_add,
	check_type_xor,
	check_type_CRC32,
}ZhiNuo_data_check_type_e;

typedef enum
{
	ZhiNuo_PTZ_CMD_UP = 0x00, // 上
	ZhiNuo_PTZ_CMD_DOWN, // 下
	ZhiNuo_PTZ_CMD_LEFT, // 左
	ZhiNuo_PTZ_CMD_RIGHT, // 右
	ZhiNuo_PTZ_CMD_ZOOM_ADD, // 变倍大     
	ZhiNuo_PTZ_CMD_ZOOM_SUB, // 变倍小 
	ZhiNuo_PTZ_CMD_FOCUS_ADD = 0x07,    // 变焦大    
	ZhiNuo_PTZ_CMD_FOCUS_SUB, // 变焦小
	ZhiNuo_PTZ_CMD_IRIS_ADD, // 光圈+
	ZhiNuo_PTZ_CMD_IRIS_SUB, // 光圈-
	ZhiNuo_PTZ_CMD_LIGHT_BRUSH = 0x0e,//   灯光 
	ZhiNuo_PTZ_CMD_AUTO = 0x0f, // 巡航 
	ZhiNuo_PTZ_CMD_PRESET_GO = 0X10,//跳到预置点   
	ZhiNuo_PTZ_CMD_PRESET_SET = 0X11,//设置预置点  
	ZhiNuo_PTZ_CMD_PRESET_CLR = 0X12,//清除预置点  
	ZhiNuo_PTZ_CMD_LEFT_UP = 0x20, // 左上
	ZhiNuo_PTZ_CMD_RIGHT_UP, // 右上
	ZhiNuo_PTZ_CMD_LEFT_DOWN, // 左下
	ZhiNuo_PTZ_CMD_RIGHT_DOWN, // 右下 
	ZhiNuo_PTZ_CMD_STOP = 0xff, //停止
	ZhiNuo_PTZ_CMD_ADD_POS_CRU = 0x24,
	ZhiNuo_PTZ_CMD_DEL_POS_CRU,
	ZhiNuo_PTZ_CMD_CLR_POS_CRU,
} ZhiNuo_PT_PTZCMD_E;

enum capture_size_t {
	CAPTURE_SIZE_D1,		///< 704*576(PAL)	704*480(NTSC)
	CAPTURE_SIZE_HD1,		///< 352*576(PAL)	352*480(NTSC)
	CAPTURE_SIZE_BCIF,		///< 704*288(PAL)	704*240(NTSC)
	CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	CAPTURE_SIZE_SVCD,		///< 480*480(PAL)	480*480(NTSC)
	CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	CAPTURE_SIZE_SVGA,		///< 800*592
	CAPTURE_SIZE_XVGA,		///< 1024*768
	CAPTURE_SIZE_WXGA,		///< 1280*800
	CAPTURE_SIZE_SXGA,		///< 1280*1024 
	CAPTURE_SIZE_WSXGA,		///< 1600*1024 
	CAPTURE_SIZE_UXGA,		///< 1600*1200
	CAPTURE_SIZE_WUXGA,		///< 1920*1200
	CAPTURE_SIZE_LTF,		///< 240*192(PAL);浪涛平台使用
	CAPTURE_SIZE_720p,		///< 1280*720
	CAPTURE_SIZE_1080p,		///< 1920*1080
	CAPTURE_SIZE_1_3M,      ////<1280*960
	CAPTURE_SIZE_NR			///< 枚举的图形大小种类的数目。
};

/// 捕获压缩格式类型
enum capture_comp_t {
	CAPTURE_COMP_DIVX_MPEG4,	///< DIVX MPEG4。
	CAPTURE_COMP_MS_MPEG4,		///< MS MPEG4。
	CAPTURE_COMP_MPEG2,			///< MPEG2。
	CAPTURE_COMP_MPEG1,			///< MPEG1。
	CAPTURE_COMP_H263,			///< H.263
	CAPTURE_COMP_MJPG,			///< MJPG
	CAPTURE_COMP_FCC_MPEG4,		///< FCC MPEG4
	CAPTURE_COMP_H264,			///< H.264
	CAPTURE_COMP_NR				///< 枚举的压缩标准数目。
};

enum rec_type
{
	REC_TYP_TIM = 0,		/*定时录像*/
	REC_TYP_MTD,
	REC_TYP_ALM,
	REC_TYP_NUM,
};

enum capture_brc_t {
	CAPTURE_BRC_CBR,
	CAPTURE_BRC_VBR,
	CAPTURE_BRC_MBR,
	CAPTURE_BRC_NR,
};

typedef struct tagAv_Connect_Info
{
	int n_socket;
	//	int n_flag;	//连接标识//DAHUA_NVR 或 ZHINUO_NVR
	int n_cnntsocket;//登录时的那个socket，码流请求时的l[8]，根据这个n_cnntsocket来设置 n_active
	int n_streamtype;//1:主码流 / 2:子码流
	int n_active;//1:监听状态,0:屏蔽状态
}Av_Connect_Info;

typedef struct __frame_caps
{
	DWORD Compression; //!压缩模式,  支持的压缩标准的掩码，位序号对应枚举类型						
	//!capture_comp_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	DWORD ImageSize;	//!分辨率, /// 支持的图像大小类型的掩码，位序号对应枚举类型						
	//!capture_size_t的每一个值。
	///! 置1表示支持该枚举值对应的特性，置0表示不支持
}FRAME_CAPS;

typedef struct tagCAPTURE_EXT_STREAM
{
	DWORD ExtraStream;							//用channel_t的位来表示支持的功能
	DWORD CaptureSizeMask[64];	//每一个值表示对应分辨率支持的辅助码流。
}CAPTURE_EXT_STREAM, *pCAPTURE_EXT_STREAM;

typedef struct {	// 定时时段
	BYTE	StartHour;
	BYTE	StartMin;
	BYTE	StartSec;
	BYTE	EndHour;
	BYTE    EndMin;
	BYTE	EndSec;
	BYTE	State;			//第二位是定时，第三位是动态检测，第四位是报警
	BYTE	Reserve;		/*!< Reserve已经被使用，更改的话请通知录像模块 */
} TSECT;

//! 颜色设置内容
typedef struct
{
	TSECT 	Sector;				/*!< 对应的时间段*/
	BYTE	Brightness;			/*!< 亮度	0-100		*/
	BYTE	Contrast;			/*!< 对比度	0-100		*/
	BYTE	Saturation;			/*!< 饱和度	0-100		*/
	BYTE	Hue;				/*!< 色度	0-100		*/
	BYTE	Gain;				/*!< 增益	0-100		*/
	BYTE	Reserve[3];
}COLOR_PARAM;

//! 颜色结构
typedef struct
{
	BYTE ColorVersion[8];
	COLOR_PARAM Color[N_COLOR_SECTION];
}CONFIG_COLOR_OLD;

//! 编码选项
typedef struct
{
	BYTE ImageSize;   /*!< 分辨率 参照枚举capture_size_t(DVRAPI.H) */
	BYTE BitRateControl;  /*!< 码流控制 参照枚举capture_brc_t(DVRAPI.H) */
	BYTE ImgQlty;   /*!< 码流的画质 档次1-6  */
	BYTE Frames;    /*!< 帧率　档次N制1-6,P制1-5 */
	BYTE AVEnable;   /*!< 音视频使能 1位为视频，2位为音频。ON为打开，OFF为关闭 */
	BYTE IFrameInterval;  /*!< I帧间隔帧数量，描述两个I帧之间的P帧个数，0-149, 255表示此功能不支持设置*/
	WORD usBitRate;
}ENCODE_OPTION;

//! 标题结构
typedef struct
{
	DWORD	TitlefgRGBA;			/*!< 标题的前景RGB，和透明度 */
	DWORD	TitlebgRGBA;		/*!< 标题的后景RGB，和透明度*/
	WORD	TitleLeft;			/*!< 标题距左边的距离与整长的比例*8192 */
	WORD	TitleTop;			/*!< 标题的上边的距离与整长的比例*8192 */
	WORD	TitleRight;			/*!< 标题的右边的距离与整长的比例*8192 */
	WORD	TitleBottom;			/*!< 标题的下边的距离与整长的比例*8192 */
	BYTE	TitleEnable;			/*!< 标题使能 */
	BYTE	Reserved[3];
}ENCODE_TITLE;

typedef struct {
	int left;
	int top;
	int right;
	int bottom;
}RECT, *PRECT;

//! 编码信息结构(双码流在用)
typedef struct {
	BYTE				CapVersion[8];				/*!< 版本号			*/
	ENCODE_OPTION		MainOption[REC_TYP_NUM];	/*!< 主码流，REC_TYP_NUM不同录像类型*/
	ENCODE_OPTION		AssiOption[EXTRATYPES];	/*!< 支持3 路辅码流 */
	BYTE				Compression;				/*!< 压缩模式 */;
	BYTE    			CoverEnable;				/*!< 区域遮盖开关　0x00不使能遮盖，0x01仅遮盖预览，0x10仅遮盖录像，0x11都遮盖	*/
	BYTE 				alignres[2];			/*!< 保留对齐用 */
	RECT				Cover;						/*!< 区域遮盖范围	*/

	ENCODE_TITLE 		TimeTitle;					/*!< 时间标题*/
	ENCODE_TITLE 		ChannelTitle;				/*!< 通道标题*/

	ENCODE_OPTION		PicOption[2];				//定时抓图、触发抓图

	BYTE	Volume;				/*!< 保存音量的阀值 */
	BYTE    VolumeEnable;       /*!< 音量阀值使能*/
	BYTE	 Reserved[46];
}CONFIG_CAPTURE_OLD;

typedef struct tagPTZ_LINK
{
	int iType;				/*!< 联动的类型 */
	int iValue;				/*!< 联动的类型对应的值 */
}PTZ_LINK;


//! 事件处理结构
typedef struct tagEVENT_HANDLER
{
	unsigned long	dwRecord;
	int		iRecordLatch;
	unsigned long	dwTour;
	unsigned long	dwSnapShot;
	unsigned long	dwAlarmOut;
	int		iAOLatch;
	PTZ_LINK	PtzLink[16];

	int 	bRecordEn;
	int 	bTourEn;
	int 	bSnapEn;
	int 	bAlarmOutEn;
	int 	bPtzEn;
	int 	bTip;
	int 	bMail;
	int 	bMessage;
	int 	bBeep;
	int 	bVoice;
	int 	bFtp;

	unsigned long	dwWorkSheet;

	unsigned long	dwMatrix;				/*!< 矩阵掩码 */
	int 	bMatrixEn;				/*!< 矩阵使能 */
	int 	bLog;					/*!< 日志使能，目前只有在WTN动态检测中使用 */
	int		iEventLatch;			/*!< 联动开始延时时间，s为单位 范围是0－－15 默认值是0*/
	int 	bMessagetoNet;			/*!< 消息上传给网络使能 */
	unsigned long	wiAlarmOut; 			/*!< 无线报警输出 */
	unsigned char	bMMSEn;					/*!< 短信报警使能  */
	unsigned char	SnapshotTimes;          /*!< 抓图张数 */
	char	dReserved[22];			/*!< 保留字节 */
} EVENT_HANDLER, *LPEVENT_HANDLER;

//!动态检测设置
typedef struct tagCONFIG_MOTIONDETECT
{
	BOOL bEnable;					/*!< 动态检测开启 */
	int iLevel;						/*!< 灵敏度 */
	DWORD mRegion[MD_REGION_ROW];	/*!< 区域，每一行使用一个二进制串 */
	EVENT_HANDLER hEvent;			/*!< 动态检测联动 */
} CONFIG_MOTIONDETECT;

typedef struct CAPTURE_DSPINFO
{
	unsigned int	nMaxEncodePower;	///< DSP 支持的最高编码能力。
	unsigned short	nMaxSupportChannel;	///< DSP 支持最多输入视频通道数。
	unsigned short	bChannelMaxSetSync;	///< DSP 每通道的最大编码设置是否同步 0-不同步, 1 -同步。
	unsigned short	nExpandChannel;		///< DSP 支持的扩展通道数，主要是多路回放使用，目前只是一个
	unsigned short  rev;
}CAPTURE_DSPINFO, *PCAPTURE_DSPINFO;

typedef struct CAPTURE_ENCCHIPCAPABILITY
{
	int EncChipNR;
	CAPTURE_DSPINFO EncChipInfo[MAX_ENC_CHIP_NR];
}CAPTURE_ENCCHIPCAPABILITY, *PCAPTURE_ENCCHIPCAPABILITY;

#define ZHINUO_SET_BIT(addr, index)\
	(addr) |= (1 << ((index)-1));



/////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

#if 0
	#define X6_PRINTF(fmt...) fprintf(stderr,##fmt)

	#define X6_TRACE(fmt...) do{ \
		X6_PRINTF("**[FILE]:%s [Line]:%d[Info]:====", __FILE__, __LINE__); \
		X6_PRINTF(fmt); \
	}while (0)

	#define TRACE(fmt...) {}
#else
	#define X6_TRACE(fmt...) {}
	#define TRACE(fmt...) {}
#endif

#endif