/******************************************************************************
******************************************************************************/

#ifndef _DEFAULT_SET_H
#define _DEFAULT_SET_H

/// ****************************************************************************************************************************************
#ifndef TL_Q3_PLATFORM
#define TL_RECORD  /* record */
#define TL_FTP_EMAIL_SUPPORT
#define TL_SUPPORT_WIRELESS
#endif

//#define TL_AUTO_IRCUT_FUNCTION  //支持ircut自动切换
//如果是发布版本，以下宏定义由make 带参数决定
#ifndef TL_RELEASE_VESION
#define	DEFAULT_SET_GENERAL_CLIENT_PRODUCT

/* debug */
//#define TL_NETWORK_DEBUG
//#define SYS_DEFAULT

#endif
/* platform */
//#define TL_PRODUCER_SUPPORT_SNAP

#define TL_WEB_LOG_SUPPORT

/// **********************************************


/*-------------------------------------------------------*/
#define SOFTWARER_VERSION				0x03000000


//#define  CUSTERMER_SHANGHAI_VER

#ifdef TL_NETWORK_DEBUG
	#define NVSDEFAULTIP					"192.168.168.178"
#else
	#define NVSDEFAULTIP					"192.168.11.88" /* RM#1733: Deafult IP Address: 192.168.11.88 henry.li 2017/02/10 */
#endif

#define NVSDEFAULTNETMASK					"255.255.255.0"

#define NVSDEFAULTMULTIADDR				"224.1.1.1"


#ifdef TL_NETWORK_DEBUG
	#define NVSDEFAULTGATEWAY				"192.168.168.1"
#else
	#define NVSDEFAULTGATEWAY				"192.168.11.1" /* RM#1733: Deafult Gateway Address: 192.168.11.1 henry.li 2017/02/22 */
#endif


#define NVSDEFAULT_WLAN_IP				"192.168.5.202"
#define NVSDEFAULT_WLAN_GATEWAY			"192.168.5.1"
#define SER_DEFAULT_PORT					8200

#define SER_DEFAULT_WEBPORT				80

#define SER_DEFAULT_MOBILEPORT                     15961

#define ROOT_USER_NAME					"admin"

#define	ROOT_USER_PASSWORD				"admin"

#define	ROOT_USER_FUNC_RIGHT				0x8000000F

#define	ROOT_USER_NET_PREVIEW_RIGHT			0x8000000F

#define	GENERAL_USER_NAME				"user"

#define	CENERAL_USER_PASSWORD				"user"

#define	NETWORK_DEFUALT_DNS				"8.8.8.8"

#define	DEVICE_SERVER_NAME				"IPCAM"

#define	AUDIO_SAMPLING_BITS				16

#define	AUDIO_SAMPLING_RATE				8000

#define	AUDIO_SAMPLING_BLOCK_SIZE			1024

#define	AV_CHANNEL_INFO_NAME				"Channel"


#define VIDEO_ENCODE_FRAME_RATE				25

//1 PAL 0 NTSC
#define	VIDEO_ENCODE_STANDARD				1

#define VIDEO_ENCODE_STREAM_RATE			2048000

//#define VIDEO_ENCODE_STREAM_RATE			1024000


#define VIDEO_ENCODE_CBR				0

#define VIDEO_ENCODE_VBR				1

#define VIDEO_ENCODE_DEFUALT_VBR_QUALITY		0

#define VIDEO_ENCODE_KEYFRAME_INTERVAL			50


#define DEFUALT_DEFINITION				2

#define DEFUALT_LOGO_X_POS				0

#define DEFUALT_LOGO_Y_POS				0

#define DEFUALT_MOTION_DETECT_CLEAR_TIME			30

#define DEFUALT_MOTION_DETECT_SENSITIVE			4 

#define ALARM_SENSOR_OUT_NAME				"Sensor Out"

#define ALARM_SENSOR_IN_NAME				"Sensor In"

#define DEFUALT_ALARM_SENSOR_TYPE			0x0

#define	DEFUALT_ALARM_SENSOR_CLEAR_TIME			30

#define	DEFUALT_ALARM_SENSOR_SHOT_CHANNEL		0

#define	DEFUALT_VIDEO_LOST_CLEAR_TIME			30


#define DEFUALT_MASK_SHOW_TIME				1

#define DEFUALT_MASK_FONT_X_POS				550
#define DEFUALT_MASK_FONT_Y_POS				520

#define DEFUALT_COMINFO_1_BAUD				2400

#define DEFUALT_COMINFO_1_DATA_BITS			0x4

#define DEFUALT_COMINFO_1_STOP_BITS			0x0

#define DEFUALT_PTZ_SPEED				60

#define DEFUALT_PTZ_PRSET_POS				1

#define DEFUALT_PTZ_ADDRESS				1

#define DEFUALT_FTP_TRANSMIT_PORT			21

#define DEFUALT_NOTIFY_PORT				6000

#define DEFUALT_NOTIFY_TIME_INTERVAL			3

#define DEFUALT_DDNS_PORT				6500

#define DEFUALT_NXSIGHT_PRODUCER_IPADDRESS		"192.168.168.165"

#define DEFUALT_NXSIGHT_PRODUCER_PORT			8889

#define DEFUALT_NXSIGHT_PRODUCER_CENTER_NAME		"nxsight"

#define DEFUALT_COMINFO_RS232_BAUD			115200

#define DEFUALT_COMINFO_RS232_DATA_BITS			0x8

#define DEFUALT_COMINFO_RS232_STOP_BITS			0x1

#endif
