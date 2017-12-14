
#ifndef _CFG_SHM_H_
#define _CFG_SHM_H_

#include "shm_util.h"
#include "av_video_common.h"
#include "av_video_display.h"
#include "wifi.h"
#include "info_log.h"
#include "common_inft.h"



#define SYS_CFG_FILE "/mnt/config/sportdv_sysenv.cfg"



#define MAX_DOMAIN_NAME			40 		///< Maximum length of domain name. Ex: www.xxx.com

#define MAX_STRING_LENGTH		256 	///< Maximum length of normal string.

#define USER_LEN				33 		///< Maximum of acount username length.
#define PASSWORD_LEN			33 		///< Maximum of acount password length.
#define IP_STR_LEN				20		///< IP string length
#define MOTION_BLK_LEN			(10) 	///< Motion block size


#define APP_ITEM_EVBBOARD_VER       "app.evbboard.ver"
#define APP_ITEM_BOARD_DISK          "board.disk"
#define APP_ITEM_BOARD_CPU          "board.cpu"

#define APP_ITEM_PRODUCT_TYPE       "app.product.type"
#define APP_ITEM_PTYPE_IPNC         "ipnc"
#define APP_ITEM_PTYPE_SPORTDV      "sportdv"
#define APP_ITEM_PTYPE_CARREC       "carrec"
#define APP_ITEM_PTYPE_Q3_SPORTDV   "q3_sportdv"

#define APP_ITEM_SENSOR_TYPE    "app.sensor.type"
#define APP_ITEM_STYPE_MIPI    "mipi"
#define APP_ITEM_STYPE_DVP    "dvp"


#define APP_ITEM_AUDIO_CODEC          "codec.model"
#define APP_ITEM_AUDIO_TQLP           "9624tqlp"
#define APP_ITEM_AUDIO_FR1023         "fr1023"

// added by linyun.xiong@2015-06-02 for add items to match sensor
#define APP_ITEM_SENSOR_MODE       "app.sensor.mode"
#define APP_ITEM_SENSOR_NAME       "app.sensor.name"
#define APP_ITEM_SENSOR_LANE       "app.sensor.lane"
#define APP_ITEM_SENSOR_FPS        "app.sensor.fps"
#define APP_ITEM_SENSOR_SETUP_FILE "app.sensor.setupfile"
#define APP_ITEM_SENSOR_BUF_SIZE   "app.sensor.buffersize"
#define APP_ITEM_SENSOR_AWBMODE    "app.sensor.awbmode"
#define APP_ITEM_MMC_DEVICE "app.mmc.device"

typedef enum
{
    AUDIO_CODEC_DEFAULT = 0,
    AUDIO_CODEC_TQLP = 1,
    AUDIO_CODEC_FR1023 = 2,

}ENUM_AUDIO_CODEC_TYPE; 



typedef enum
{
    FILE_SAVE_CLOSE = 0,
    FILE_SAVE_ALL_DAY = 1,
    FILE_SAVE_ON_ALARM = 2,
    FILE_SAVE_ALARMSNAP = 4,
}ENUM_FILE_SAVE_TYPE; //fengwu 0915



typedef struct
{
  unsigned char      motionenable;				///< motion detection enable
  unsigned char      motioncenable;				///< customized sensitivity enable
  unsigned char      motionlevel;				///< predefined sensitivity level
  unsigned char      motioncvalue;				///< customized sensitivity value
  unsigned char	 motionblock[MOTION_BLK_LEN];	///< motion detection block data
}Motion_Config_Data;


/**
* @brief temperature configuration data.
* fengwu 20140609
*/
typedef struct
{
    int minTemperature;
    int maxTemperature;
    int tempAlarmFlag;
}Temperature_Config_Data;

/**
* @brief temperature configuration data.
* fengwu 20140609
*/
typedef struct
{
    int audioLevel;
    int auAlarmFlag;
}Audio_Config_Data;


typedef struct
{
    int server_port;
	char		servier_ip[MAX_DOMAIN_NAME+1]; ///< SMTP server address	        
	char		username[USER_LEN]; ///< SMTP username
	char		password[PASSWORD_LEN]; ///< SMTP password
	unsigned char		authentication; ///< SMTP authentication
	char		sender_email[MAX_STRING_LENGTH]; ///< sender E-mail address
	char		receiver_email[MAX_STRING_LENGTH]; ///< receiver E-mail address
	char		CC[MAX_STRING_LENGTH]; ///< CC E-mail address
	char		subject[MAX_STRING_LENGTH]; ///< mail subject
	char		text[MAX_STRING_LENGTH]; ///< mail text
	unsigned char		attachments; ///< mail attachment
	unsigned char		view; ///< smtp view
	unsigned char		asmtpattach; ///< attatched file numbers
	unsigned char        attfileformat; ///< attachment file format
	unsigned char        sendMailFlag; //201406069 fengwu
} Smtp_Config_Data;


typedef enum{
    CLOUD_TP_CLOSE,
    CLOUD_TP_FTP,
    CLOUD_TP_BAIDU_PCS,
    CLOUD_TP_BAIDU_BCS,
}ENUM_CLOUD_TP;


typedef struct {
    ENUM_CLOUD_TP cloudType;
    char ftpSvrIp[IP_STR_LEN];
    char ftpUsrName[USER_LEN];
    char ftpUsrPass[PASSWORD_LEN];
    char baiduAPIKey[PASSWORD_LEN];
    char baiduSecKey[PASSWORD_LEN];
    char baiduPcsAppName[USER_LEN];
    char baiduPcsAccessCode[MAX_STRING_LENGTH];
}Cloud_Config_Data;


typedef struct
{
/**  __u8			sdrecordtype;
  __u8			sdcount;
  __u8			sdrate;
  __u8			sdduration;
  __u8			aviprealarm;*/
   //__u8         sdformat;
   unsigned char			sdfileformat;	///< file format saved into SD card
   unsigned char			sdrenable;		// ENUM_FILE_SAVE_TYPE
   unsigned char			sdvideomode;    ///< 0:720, 1:CIF, 2:VGA
   unsigned char			sdinsert;		///< SD card inserted
   unsigned int sd_free_space;    //MB
   unsigned int sd_total_space;   //MB
}Sdcard_Config_Data;

typedef struct P2P_Config_Data {
	unsigned char maxSessionCount;		//最大的session 数目
	//unsigned short port;				//外接的端口
	char netInterface[10];		//接口eth0 wlan0等
	char deviceID[32];			//设备ID号
    char password[PASSWORD_LEN];//登录p2p服务器的密码
}P2P_Config_Data;

typedef struct System_Config_Data {
	unsigned char model[16];	// IPCam mode
	unsigned char vendor[16];	// IPCam manufacturer
	unsigned int version;		// IPCam firmware version	ex. v1.2.3.4 => 0x01020304;  v1.0.0.2 => 0x01000002
}System_Config_Data;



typedef struct {
    int updateFileFlag;
    AV_VIDEO_StreamInfo videoStrmInfo[E_STREAM_ID_CNT];
    CFG_VIDEO_DisplayStream videoDisplayStrm;
    WIFI_CONFIG_DATA wifiCfg;
	Cloud_Config_Data cloud_config;
	Smtp_Config_Data smtp_config;
	Motion_Config_Data motion_config;
	Temperature_Config_Data temperatureAlm; 
    Audio_Config_Data audioAlm;
	ENUM_FILE_SAVE_TYPE fileSaveType;
	P2P_Config_Data p2p_config;		//p2p data
	System_Config_Data sys_config;
	Inft_CONFIG_Mode_Attr config_mode_addr;
	Inft_Play_Attr        play_addr;
	Inft_Video_Mode_Attr  video_mode_addr[INFT_CAMERA_FRONT_REAR];
	Inft_PHOTO_Mode_Attr  photo_mode_addr[INFT_CAMERA_FRONT_REAR];
    struct Inft_Action_Attr action_attr[INFT_ACTION_MODE_MAX];
    struct Inft_Wifi_Attr wifi_attr[INFT_WIFI_MODE_MAX];
    GPS_Data  gps_data;
} T_SYS_CFG;


#ifdef __cplusplus 
extern "C" {
#endif

int cfg_shm_init();

int cfg_shm_loadDefCfg();

void  cfg_shm_free();

void* cfg_shm_getShm();
unsigned int cfg_shm_getStorageSize();


/*-------------------------------------------------------------------------------------
            GET/SET operations
-------------------------------------------------------------------------------------*/
int cfg_shm_get_updateFileFlag();
void cfg_shm_set_updateFileFlag(int flag);
T_CFG_PRODUCE_TYPE cfg_shm_getProdType();

T_CFG_SENSOR_TYPE cfg_shm_getSensorType();

int cfg_shm_is_audio_tqlp();


void cfg_shm_initVideoStrmInfo();
AV_VIDEO_StreamInfo *cfg_shm_getVideoStrmInfo();
AV_VIDEO_StreamInfo *cfg_shm_getVideoStrmInfoByIndex(T_VIDEO_STREAM_ID id);
void cfg_shm_enableVideoStrm(T_VIDEO_STREAM_ID id, unsigned int enabled);
void cfg_shm_setVideoStrmResolution(T_VIDEO_STREAM_ID id, T_VIDEO_RESOLUTION rtype);
//void cfg_shm_setVideoBitrate(T_VIDEO_STREAM_ID id, unsigned int bitRate);

CFG_VIDEO_DisplayStream* cfg_shm_getVideoDisplayStrm();
void cfg_shm_setVideoDisplayStrm(CFG_VIDEO_DisplayStream *disStrm);

WIFI_CONFIG_DATA* cfg_shm_getWifiConfig();
void cfg_shm_setWifiConfig(WIFI_CONFIG_DATA* wifiConfig);

Cloud_Config_Data* cfg_shm_getCloudConfig();
void cfg_shm_setCloudConfig(Cloud_Config_Data* cloudConfig);

Smtp_Config_Data* cfg_shm_getSmtpConfig();
void cfg_shm_setSmtpConfig(Smtp_Config_Data* smtpConfig);

Motion_Config_Data* cfg_shm_getMotionConfig();
void cfg_shm_setMotionConfig(Motion_Config_Data* motionConfig);

Temperature_Config_Data* cfg_shm_getTempConfig();
void cfg_shm_setTempConfig(Temperature_Config_Data* tempConfig);


Audio_Config_Data* cfg_shm_getAudioConfig();
void cfg_shm_setAudioConfig(Audio_Config_Data* audioConfig);

ENUM_FILE_SAVE_TYPE cfg_shm_getFileSaveType();
void cfg_shm_setFileSaveType(ENUM_FILE_SAVE_TYPE type);

P2P_Config_Data* cfg_shm_getP2PConfig();
void cfg_shm_setP2PConfig(P2P_Config_Data* p2pConfig);

System_Config_Data* cfg_shm_getSystemConfig();
void cfg_shm_setSystemConfig(System_Config_Data* systemConfig);

Inft_PHOTO_Mode_Attr* cfg_shm_getphotoModeAttr(ENUM_INFT_CAMERA cameraId);
void cfg_shm_setphotoModeAttr(Inft_PHOTO_Mode_Attr* photo_mode_addr,ENUM_INFT_CAMERA cameraId);

Inft_Video_Mode_Attr* cfg_shm_getvideoModeAttr(ENUM_INFT_CAMERA cameraId);

void cfg_shm_setvideoModeAttr(Inft_Video_Mode_Attr* video_mode_addr,ENUM_INFT_CAMERA cameraId);

Inft_CONFIG_Mode_Attr* cfg_shm_getconfigModeAttr();


void cfg_shm_setconfigModeAttr(Inft_CONFIG_Mode_Attr* config_mode_addr);




//add by linyun.xiong@2015-06-02
int cfg_shm_getSensorName(char* out_sensor_name);
int cfg_shm_getSensorMode(char* out_sensor_mode);
int cfg_shm_getSensorLane(char* out_sensor_lane);
int cfg_shm_getSensorFps(char* out_sensor_fps);
int cfg_shm_getSensorSetupFile(char* out_sensor_setupfile);
int cfg_shm_getSensorBufferSize(char* out_sensor_buffersize);
int cfg_shm_getSensorAWBMode(char* out_sensor_awbmode);

#ifdef __cplusplus 
}
#endif


#endif

