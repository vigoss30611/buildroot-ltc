
#ifndef __LIBTYPES_H__
#define __LIBTYPES_H__

/***************************************************/
/* 基本定义                                        */
/***************************************************/

/* 返回值 */
#define LIB_OK	0
#define LIB_ERR	(-1)

/* 位定义 */
#define LIB_BIT00 (1 << 0)
#define LIB_BIT01 (1 << 1)
#define LIB_BIT02 (1 << 2)
#define LIB_BIT03 (1 << 3)
#define LIB_BIT04 (1 << 4)
#define LIB_BIT05 (1 << 5)
#define LIB_BIT06 (1 << 6)
#define LIB_BIT07 (1 << 7)
#define LIB_BIT08 (1 << 8)
#define LIB_BIT09 (1 << 9)
#define LIB_BIT10 (1 << 10)
#define LIB_BIT11 (1 << 11)
#define LIB_BIT12 (1 << 12)
#define LIB_BIT13 (1 << 13)
#define LIB_BIT14 (1 << 14)
#define LIB_BIT15 (1 << 15)
#define LIB_BIT16 (1 << 16)
#define LIB_BIT17 (1 << 17)
#define LIB_BIT18 (1 << 18)
#define LIB_BIT19 (1 << 19)
#define LIB_BIT20 (1 << 20)
#define LIB_BIT21 (1 << 21)
#define LIB_BIT22 (1 << 22)
#define LIB_BIT23 (1 << 23)
#define LIB_BIT24 (1 << 24)
#define LIB_BIT25 (1 << 25)
#define LIB_BIT26 (1 << 26)
#define LIB_BIT27 (1 << 27)
#define LIB_BIT28 (1 << 28)
#define LIB_BIT29 (1 << 29)
#define LIB_BIT30 (1 << 30)
#define LIB_BIT31 (1 << 31)

/***************************************************/
/* 类型定义                                        */
/***************************************************/

/* 尺寸 */
typedef struct {
	int width; /* 宽度 */
	int height; /* 高度 */
} lib_size_t;

/* 矩形 */
typedef struct {
	int left; /* 水平起始位置 */
	int top; /* 垂直起始位置 */
	int width; /* 宽度 */
	int height; /* 高度 */
} lib_rect_t;

/* 时间 */
typedef struct {
	int hour; /* 时 */
	int minute; /* 分 */
	int second; /* 秒 */
} lib_time_t;

/* 日期 */
typedef struct {
	int year; /* 年 */
	int month; /* 月 */
	int day; /* 日 */
} lib_date_t;

/* 时刻 */
typedef struct {
	lib_date_t momdate; /* 日期 */
	lib_time_t momtime; /* 时间 */
} lib_moment_t;

/* 时间段 */
typedef struct {
	lib_moment_t beginning; /* 开始时刻 */
	lib_moment_t ending; /* 结束时刻 */
} lib_duration_t;

typedef enum {
	DISP_DEV_NULL   = 0, /* 无 */
	DISP_DEV_SPOT   = LIB_BIT00, /* SPOT设备 */
	DISP_DEV_BT656  = LIB_BIT01, /* BT656输出 */
	DISP_DEV_BT1120 = LIB_BIT02, /* BT1120输出 */
	DISP_DEV_VGA0   = LIB_BIT03, /* VGA0设备 */
	DISP_DEV_HDMI0  = LIB_BIT04, /* HDMI0设备 */
	DISP_DEV_CVBS0  = LIB_BIT05, /* CVBS0设备 */
	DISP_DEV_VGA1   = LIB_BIT06, /* VGA2设备 */
	DISP_DEV_HDMI1  = LIB_BIT07, /* HDMI2设备 */
	DISP_DEV_CVBS1  = LIB_BIT08, /* CVBS2设备 */
	DISP_DEV_VGA2   = LIB_BIT09, /* VGA2设备 */
	DISP_DEV_HDMI2  = LIB_BIT10, /* HDMI2设备 */
	DISP_DEV_CVBS2  = LIB_BIT11  /* CVBS2设备 */
} disp_dev_e;

typedef enum {
	DISP_RESO_NULL         = 0, /* 无 */
	DISP_RESO_720_480I60   = LIB_BIT00, /* 分辨率720x480，刷新频率I60表示场频60Hz，下同 */
	DISP_RESO_720_480P60   = LIB_BIT01, /* 分辨率720x480，刷新频率P60表示帧频60Hz，下同 */
	DISP_RESO_720_576I50   = LIB_BIT02,
	DISP_RESO_720_576P50   = LIB_BIT03,
	DISP_RESO_800_600P60   = LIB_BIT04,
	DISP_RESO_1024_768P60  = LIB_BIT05,
	DISP_RESO_1280_720P50  = LIB_BIT06,
	DISP_RESO_1280_720P60  = LIB_BIT07,
	DISP_RESO_1280_800P60  = LIB_BIT08,
	DISP_RESO_1280_1024P60 = LIB_BIT09,
	DISP_RESO_1366_768P60  = LIB_BIT10,
	DISP_RESO_1440_900P60  = LIB_BIT11,
	DISP_RESO_1920_1080P24 = LIB_BIT12,
	DISP_RESO_1920_1080P25 = LIB_BIT13,
	DISP_RESO_1920_1080P30 = LIB_BIT14,
	DISP_RESO_1920_1080I50 = LIB_BIT15,
	DISP_RESO_1920_1080P50 = LIB_BIT16,
	DISP_RESO_1920_1080I60 = LIB_BIT17,
	DISP_RESO_1920_1080P60 = LIB_BIT18
} disp_resolution_e;

typedef struct {
	int dispdev; /* 显示设备，disp_dev_e类型 */
	int dispreso; /* 显示分辨率，disp_resolution_e类型，枚举设备时为位属性 */
} display_dev_t;

typedef struct {
	int brightness; /* 亮度 */
	int contrast; /* 对比度 */
	int saturation; /* 饱和度 */
	int hue; /* 色度 */
	int sharpness; /* 锐利度 */
	int denoise;  /* 去噪强度 */
	int enhance; /* 增强 */
} effect_value_t;

typedef enum {
	VIDEO_PROFILE_H264_BP    = LIB_BIT00, /* h.264 base line */
	VIDEO_PROFILE_H264_MP    = LIB_BIT01, /* h.264 main profile */
	VIDEO_PROFILE_H264_HP    = LIB_BIT02, /* h.264 high profile */
	VIDEO_PROFILE_MPEG4_SP   = LIB_BIT03, /* mpeg4 simple profile */
	VIDEO_PROFILE_MPEG4_ASP  = LIB_BIT04, /* mpeg4 advance simple profile */
	VIDEO_PROFILE_MJPEG      = LIB_BIT05, /* motion jpeg */
	VIDEO_PROFILE_JPEG       = LIB_BIT06, /* jpeg */
	VIDEO_PROFILE_RAW_YUV420 = LIB_BIT07, /* yuv 420 */
	VIDEO_PROFILE_RAW_YUV422 = LIB_BIT08  /* yuv 422 */
} venc_type_e, vdec_type_e;

enum {
	ENCODE_MODE_CBR, /* 固定码流 */
	ENCODE_MODE_VBR /* 可变码流 */
};

typedef enum {
	VIDEO_INPUT_NULL            = 1,  /* 无 */
		
	VIDEO_INPUT_AHD_1080P_PAL   = 2,  /* AHD 机型视频输入类型 */
	VIDEO_INPUT_AHD_1080P_NTSC  = 3,
	VIDEO_INPUT_AHD_720P_PAL    = 4,
	VIDEO_INPUT_AHD_720P_NTSC   = 5,
	VIDEO_INPUT_AHD_1080H_PAL   = 6,
	VIDEO_INPUT_AHD_1080H_NTSC  = 7,	
	
	VIDEO_INPUT_SDI_1080P_PAL   = 8,  /* SDI 机型视频输入类型 */
	VIDEO_INPUT_SDI_1080P_NTSC  = 9,
	VIDEO_INPUT_SDI_720P_PAL    = 10,	
	VIDEO_INPUT_SDI_720P_NTSC   = 11,
	VIDEO_INPUT_SDI_1080H_PAL   = 12,
	VIDEO_INPUT_SDI_1080H_NTSC  = 13,	
	
	VIDEO_INPUT_CVI_1080P_PAL   = 14,  /* CVI 机型视频输入类型 */
	VIDEO_INPUT_CVI_1080P_NTSC  = 15,
	VIDEO_INPUT_CVI_720P_PAL    = 16,
	VIDEO_INPUT_CVI_720P_NTSC   = 17,
	VIDEO_INPUT_CVI_1080H_PAL   = 18,
	VIDEO_INPUT_CVI_1080H_NTSC  = 19,	
	
	VIDEO_INPUT_CVBS            = 20,  /* 普通960H\D1机型视频输入类型 */	
	VIDEO_INPUT_CVBS_PAL        = 21,
	VIDEO_INPUT_CVBS_NTSC       = 22,

    VIDEO_INPUT_TVI_1080P_NTSC  = 23,  /* TVI 机型视频输入类型 */
    VIDEO_INPUT_TVI_1080P_PAL   = 24,
    VIDEO_INPUT_TVI_720P_NTSC   = 25,
    VIDEO_INPUT_TVI_720P_PAL   = 26,
    
	VIDEO_INPUT_IPC_720P             = 27,  /* IPC 720P 视频输入类型 */	
	VIDEO_INPUT_IPC_1080P             = 28,  /* IPC 1080P 视频输入类型 */	
} video_input_e;

typedef struct {
	int encformat; /* 编码格式 */
	int framerate; /* 帧率 */
	int bitrate; /* 码率 */
	int gop; /* 关键帧间隔 */
	int quality; /* 编码质量,VBR有效 */
	int encmode; /* 编码模式 ENCODE_MODE_CBR或ENCODE_MODE_VBR */
	lib_size_t picsz; /* 图像尺寸 */
} video_encode_t;

typedef enum {
	AUDIO_PROFILE_PCM        = LIB_BIT00,
	AUDIO_PROFILE_G711A      = LIB_BIT01,
	AUDIO_PROFILE_G711U      = LIB_BIT02,
	AUDIO_PROFILE_IMA_ADPCM  = LIB_BIT03, /* sample为奇数 */
	AUDIO_PROFILE_DVI4_ADPCM = LIB_BIT04, /* sample为偶数 */
	AUDIO_PROFILE_G726_ADPCM = LIB_BIT05,
	AUDIO_PROFILE_AMR        = LIB_BIT06,
	AUDIO_PROFILE_VORBIS     = LIB_BIT07,
	AUDIO_PROFILE_AAC        = LIB_BIT08,
	AUDIO_PROFILE_HEAAC      = LIB_BIT09,
	AUDIO_PROFILE_MP3        = LIB_BIT10
} aenc_type_e, adec_type_e;

typedef struct {
	int samplerate; /* 音频源采样频率 */
	int samplebits; /* 音频源采样精度 */
	int audiochs; /* 音频源通道数 */
} asrc_param_t, audio_source_t;

enum {
	AUDIO_FROM_CAMERA = LIB_BIT00, /* 音频从摄像机获取 */
	AUDIO_FROM_BOARD  = LIB_BIT01 /* 音频从板载输入获取 */
};

typedef struct {
	int profile; /* 音频编码算法，类型见aenc_type_e定义 */
	int encbitrate; /* 音频编码码率 */
} aenc_param_t, audio_encode_t;

enum {
	PIC_QUALITY_HIGH, /* 高质量 */
	PIC_QUALITY_NORMAL, /* 一般质量 */
	PIC_QUALITY_LOW /* 低质量 */
};

typedef struct {
	int picquality; /* 图片质量 PIC_QUALITY_HIGH等 */
	lib_size_t picsz; /* 图片尺寸 */
} pic_param_t;

enum {
	LINK_MODE_AUTO, /* 自动 */
	LINK_MODE_UDP, /* udp连接模式 */
	LINK_MODE_TCP /* tcp连接模式 */
};

#define DEV_PROTOCOL_LEN	16
#define DEV_SN_LEN			48
#define DEV_NAME_LEN		32
#define DEV_VER_LEN			16
#define DEV_ADDR_LEN		64
#define DEV_MAC_LEN			6
#define DEV_MANU_PREFIX		8
#define DEV_P2P_ID			20

typedef struct {
	char protocol[DEV_PROTOCOL_LEN]; /* 设备协议名（对于cli_dev_scan为设备型号） */
	char sn[DEV_SN_LEN]; /* 设备序号 */
	char name[DEV_NAME_LEN]; /* 设备名 */
	char softwarever[DEV_VER_LEN]; /* 软件型号 */
	char hardwarever[DEV_VER_LEN]; /* 硬件型号 */
	char ipaddr[DEV_ADDR_LEN]; /* ip或域名 */
	unsigned short ipport; /* 连接端口号 */
	unsigned short httpport; /* http端口号，0表示无该信息 */
	unsigned short chnum; /* 通道数，0表示无通道信息 */
	unsigned char mac[DEV_MAC_LEN]; /* 物理地址,0表示无该信息 */
	char manuprefix[DEV_MANU_PREFIX]; /* p2p厂商前缀 */
	char p2pid[DEV_P2P_ID]; /* p2p id */
	unsigned char p2ptype; /* p2p类型 */
	char res[3]; /* 保留 */
} dev_param_t;

/* 系统模式 */
typedef struct {
	unsigned char analogcif; /* 模拟cif通道数，为0表示无，下同 */
	unsigned char digitalcif; /* 数字cif通道数 */
	unsigned char analoghd1; /* 模拟half d1通道数 */
	unsigned char digitalhd1; /* 数字half d1通道数 */
	unsigned char analogd1; /* 模拟d1通道数 */
	unsigned char digitald1; /* 数字d1通道数 */
	unsigned char analog960h; /* 模拟960h通道数 */
	unsigned char digital960h; /* 数字960h通道数 */
	unsigned char analog720p; /* 模拟720p通道数 */
	unsigned char digital720p; /* 数字720p通道数 */
	unsigned char analog960p; /* 模拟960p通道数 */
	unsigned char digital960p; /* 数字960p通道数 */
	unsigned char analog1080p; /* 模拟1080p通道数 */
	unsigned char digital1080p; /* 数字1080p通道数 */
	unsigned char previewwinnum; /* 最大预览窗口数 */
	unsigned char playbackwinnum; /* 最大回放窗口数 */
	unsigned char previewdigitalmainstreamnum; /* 预览数字主码流数（同时可能存在其他的子码流预览） */
	unsigned char previewalldigitalmainstreamnum; /* 预览数字全主码流数（表示不预览子码流或子码流全部隐藏并冻结时可预览的主码流数） */
	unsigned char previewmainstreamhiddenfreeze; /* 预览数字主码流时冻结隐藏通道，0 不冻结 1 自动冻结 */
	unsigned char suggeststreammanagememsz; /* 码流管理器建议使用的内存大小，为0默认使用64MB */
	unsigned char externallivestreamnum; /* 外部访问实时码流数 */
	unsigned char externalrecordstreamnum; /* 外部访问录像码流数 */
	char analogmodename[24]; /* 模拟通道模式显示名（以0结尾的字符串），空串表示按照原规则显示 */
	char digitalmodename[24]; /* 数字通道模式显示名（以0结尾的字符串），空串表示按照原规则显示 */
	unsigned char analog1080h; /* 模拟1080h通道数 */
	unsigned char digital1080h; /* 数字1080h通道数 */
	char res[28];
} cli_work_mode_t;

enum {
	STANDARD_AUTO, /* 自动识别 */
	STANDARD_PAL, /* pal制 */
	STANDARD_NTSC /* ntsc制 */
};

enum {
	DATE_MMDDYY, /* 月/日/年 */
	DATE_DDMMYY, /* 日/月/年 */
	DATE_YYMMDD /* 年/月/日 */
};

enum {
	TIME_HOUR12, /* 12小时制 */
	TIME_HOUR24 /* 24小时制 */
};

typedef struct {
	unsigned char datemode; /* 日期格式 DATE_MMDDYY ~ DATE_YYMMDD */
	unsigned char timemode; /* 时间格式 TIME_HOUR12 ~ TIME_HOUR24 */
	char res[2];
} date_time_mode_t;

#define OSD_NAME_SZ 32
#define OSD_DATA_SZ 1024

/* 编码OSD设置，库可根据需要选取字符串或点阵方式，Windows系统只用到osdname成员 */
typedef struct {
	char osdname[OSD_NAME_SZ]; /* utf8字符串 */
	lib_size_t sz; /* 点阵尺寸 */
	unsigned short linebytes; /* 每行的字节数 */
	unsigned char pixelbits; /* 每个点的位数 */
	unsigned char osdisempty; /* 读参数为1表示当前的参数未设置（写参数忽略） */
	unsigned char osddata[OSD_DATA_SZ]; /* 点阵数据，按照显示模式排列 */
} venc_osd_t;

enum {
	BAUD_RATE_1200   = LIB_BIT00, /* 波特率 */
	BAUD_RATE_2400   = LIB_BIT01, /* 波特率 */
	BAUD_RATE_4800   = LIB_BIT02, /* 波特率 */
	BAUD_RATE_9600   = LIB_BIT03, /* 波特率 */
	BAUD_RATE_14400  = LIB_BIT04, /* 波特率 */
	BAUD_RATE_19200  = LIB_BIT05, /* 波特率 */
	BAUD_RATE_38400  = LIB_BIT06, /* 波特率 */
	BAUD_RATE_57600  = LIB_BIT07, /* 波特率 */
	BAUD_RATE_115200 = LIB_BIT08 /* 波特率 */
};

enum {
	DATA_BIT_4 = LIB_BIT00, /* 数据位数 */
	DATA_BIT_5 = LIB_BIT01, /* 数据位数 */
	DATA_BIT_6 = LIB_BIT02, /* 数据位数 */
	DATA_BIT_7 = LIB_BIT03, /* 数据位数 */
	DATA_BIT_8 = LIB_BIT04 /* 数据位数 */
};

enum {
	DATA_CHECK_NULL  = LIB_BIT00, /* 无校验 */
	DATA_CHECK_ODD   = LIB_BIT01, /* 奇校验 */
	DATA_CHECK_EVEN  = LIB_BIT02, /* 偶校验 */
	DATA_CHECK_FLAG  = LIB_BIT03, /* 标志,始终为1 */
	DATA_CHECK_SPACE = LIB_BIT04 /* 空格,始终为0 */
};

enum {
	FLOW_CTRL_NONE = LIB_BIT00, /* 无 */
	FLOW_CTRL_SW   = LIB_BIT01, /* 软件，Xon/Xoff */
	FLOW_CTRL_HW   = LIB_BIT02 /* 硬件 */
};

typedef struct {
	int baudrate; /* 波特率(位属性)，BAUD_RATE_1200等 */
	int databits; /* 数据位(位属性)，DATA_BIT_4等 */
	int datacheck; /* 数据校验(位属性)，DATA_CHECK_NULL等 */
	int flowctrl; /* 数据流控(位属性)，FLOW_CTRL_NONE等 */
} uart_param_t;

#define PTZ_PROTOCOL_LEN 16

typedef struct {
	char protocol[PTZ_PROTOCOL_LEN]; /* 云台协议名 */
	int ptzaddr; /* 云台地址 */
	uart_param_t uartpara; /* 串口参数 */
} ptz_param_t;

typedef struct {
	int autodst; /* 当前时区夏令时自动调整 */
	int tzbias; /* 当前时区与UTC时间差(分钟) */
	lib_moment_t utctime; /* UTC时间 */
} avc_time_t;

typedef struct {
	char dhcpenable; /* 是否启用dhcp */
	char isipv6; /* 是否ipv6，当前应为0表示使用ipv4 */
	unsigned short httpport; /* http端口 */
	unsigned short httpsport; /* https端口 */
	unsigned short rtspport; /* rtsp端口 */
	char ipaddr[DEV_ADDR_LEN]; /* ip地址 */
	char netmask[DEV_ADDR_LEN]; /* 子网掩码 */
	char gateway[DEV_ADDR_LEN]; /* 默认网关 */
	char dnsaddr[DEV_ADDR_LEN]; /* dns地址 */
} avc_network_t;

enum key_state_e {
	KEY_STATE_UP, /* key up */
	KEY_STATE_DOWN, /* key down */
	KEY_STATE_CLICK /* key down & up */
};

enum key_value_e {
	KEY_VALUE_ESCAPE, /* ESC */
	KEY_VALUE_ENTER, /* enter */
	KEY_VALUE_BACKSPACE, /* backspace */
	KEY_VALUE_FN, /* FN */

	KEY_VALUE_UP, /* up */
	KEY_VALUE_DOWN, /* down */
	KEY_VALUE_LEFT, /* left */
	KEY_VALUE_RIGHT, /* right */

	KEY_VALUE_NUM1, /* 1 */
	KEY_VALUE_NUM2, /* 2 */
	KEY_VALUE_NUM3, /* 3 */
	KEY_VALUE_NUM4, /* 4 */
	KEY_VALUE_NUM5, /* 5 */
	KEY_VALUE_NUM6, /* 6 */
	KEY_VALUE_NUM7, /* 7 */
	KEY_VALUE_NUM8, /* 8 */
	KEY_VALUE_NUM9, /* 9 */
	KEY_VALUE_NUM0, /* 0 */

	KEY_VALUE_INC, /* + */
	KEY_VALUE_DEC, /* - */

	KEY_VALUE_MENU, /* menu */
	KEY_VALUE_WIZARD, /* wizard */
	KEY_VALUE_REC, /* rec menu */
	KEY_VALUE_PTZ, /* ptz menu */
	KEY_VALUE_BACKUP, /* backup menu */
	KEY_VALUE_ALARM, /* alarm menu */
	KEY_VALUE_CLRALARM, /* alarm clear menu */
	KEY_VALUE_SEQ, /* seq display */
	KEY_VALUE_DISPLAY, /* display menu */
	KEY_VALUE_PLAYBACK, /* playback menu */
	KEY_VALUE_SEARCH, /* rec search menu */

	KEY_VALUE_PLAY, /* play start or restore */
	KEY_VALUE_PAUSE, /* play pause */
	KEY_VALUE_STOP, /* play stop */
	KEY_VALUE_MUTE, /* mute or voice */
	KEY_VALUE_SLOW, /* play speed slow */
	KEY_VALUE_FAST, /* play speed fast */
	KEY_VALUE_FORWARD, /* play forward */
	KEY_VALUE_BACKWRAD, /* play backward */

	KEY_VALUE_WIN1, /* win 1 */
	KEY_VALUE_WIN4, /* win 4 */
	KEY_VALUE_WIN8, /* win 8 */
	KEY_VALUE_WIN9, /* win 9 */
	KEY_VALUE_WIN16, /* win 16 */
	KEY_VALUE_WIN25, /* win 25 */
	KEY_VALUE_WIN36, /* win 36 */
	KEY_VALUE_WINMAX, /* win max */
	KEY_VALUE_WINSW /* win 1/4/8/9... switch */
};

#endif /* #ifndef __LIBTYPES_H__ */
