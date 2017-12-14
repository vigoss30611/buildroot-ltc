#ifndef __SYSCONF_H__
#define __SYSCONF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

//#include <linux_list.h>

#include "QMAPI.h"


#define  NORMAL_TZ_COUNT                38
#define TZ_COUNT                                54
#define MAX_USERNAME_LEN        		24
#define MAX_USERPASSWD_LEN      	64
#define MAX_SERVER_ADDR_LEN     	256
#define MAX_PATHNAME_LEN        		64
#define MAX_EMAIL_ADDR_LEN      		64
#define MAX_ALARM_RECORD_LEN    	32
#define MAX_OSD_TEXT_LEN			64
#define BUF_SZ						128
#define BUG_MAX_CFGETH0_LEN		768
#define BUG_MAX_CFGSIP_LEN		1536

#define ALARM_HOUR_TO_SECOND    	3600
#define ALARM_MINUTE_TO_SECOND  	60


#define LEN_8_SIZE		8
#define LEN_12_SIZE		12
#define LEN_16_SIZE		16
#define LEN_24_SIZE		24
#define LEN_32_SIZE		32
#define LEN_64_SIZE		64
#define LEN_128_SIZE	128
#define LEN_256_SIZE	256
#define LEN_512_SIZE	512

#define ENCRYPTION_SSID_SIZE	64
#define ENCRYPTION_KEY_SIZE	128

#define ENCRYPTION_NONE				0
#define ENCRYPTION_WEP				1
#define ENCRYPTION_WPA				2
#define ENCRYPTION_WPA2				3

#define PTZ_CONF_FILE				"/jb_config/jb_rootfs/onvif_ptz.conf"
#define IFCFG_SIP_FILE			"/data/etc/network/sip.conf"
#define IFCFG_ETH0_FILE			"/data/etc/network/ifcfg-eth0" 
#define DNSCFG_FIlE				"/etc/resolv.conf"
#define DNSCFG_FIlE_DATA		"/data/etc/resolv.conf"
#define NTPCFG_FILE				"/data/etc/time/ntp.conf"
#define TIME_ZONE_CFG_FILE		"/data/etc/TZ"
#define ETC_TZ                             "/etc/TZ"
#define DEV_MISC_I2C_8563           "/dev/misc/i2c_8563" 
#define CFG_PROVISION_FILE		"/data/etc/provision.conf"
#define CFG_SYS_LOG_FILE		"/data/etc/syslog.conf"
#define WPA_SUPPLICANT_CONF		"/data/etc/wireless/wifi.conf"
#define	WIRELESS_STATE_FIlE		"/var/run/iw_s"



#define  NUMBER_200         200
#define  NUMBER_404         404
#define  NUMBER_503         503     
#define  SUCCESSFUL   "Successful\r\n"
#define  SUCCESSFUL_REBOOT   "Successful Need Reboot\r\n "
#define  SERVICE_UNAVAILABLE   "Service Unavailable"
#define  NOFOUND_CMD               "no found command"
#define  MIX_A_B(A,B)         ((A<<16)|(B&0XFFFF))
#define  MAX_PROBE          1
#define MIN_CHANNEL      0
#define MAX_CHANNEL      1
#define MIN_REGION_ID      0
#define MAX_REGION_ID      15
#define TIME_BEFOR_RECORD      160
#define TIME_AFTER_RECORD      320
#define NTP_FILE_SIZ			320

#define PCMU_ENC				0x00
#define PCMA_ENC				0x01
#define G726_ENC				0x02
#define AUDIO_DISABLE			0x03
#define AMR_ENC					0x04

/*用户信息结构体定义*/
typedef struct 
{
	char name[MAX_USERNAME_LEN];
	char password[MAX_USERPASSWD_LEN];
	unsigned char privilege;
	unsigned char disable;
}ONVIF_USER_INFO;

/*网络设备配置结构体定义*/
typedef struct 
{
	char device[8];
	char hwaddr[18];
	char ip[LEN_16_SIZE];
	char mask[LEN_16_SIZE];
	char gateway[LEN_16_SIZE];
	char dns0[LEN_16_SIZE];
	char dns1[LEN_16_SIZE];
	unsigned char  dhcp_ip;
	unsigned char  dhcp_dns;
	char domain[LEN_32_SIZE];
	unsigned char  dhcp_ntp;
}NET_CONF;


typedef struct 
{
	float defaultptzspeed_pantilt_x;
	float defaultptzspeed_pantilt_y;
	float defaultptzspeed_zoom_x;
	char defaultptztimeout[8];
	char pantiltlimits_uri[128];
	float pantiltlimits_xrange_min;
    	float pantiltlimits_xrange_max;
	float pantiltlimits_yrange_min;
	float pantiltlimits_yrange_max;
	char zoomlimits_uri[128];
	float zoomlimits_xrange_min;
	float zoomlimits_xrange_max;
	float position_pantilt_x;
	float position_pantilt_y;
	float position_zoom_x;
	int movestatus;
	float home_position_pantilt_x;
	float home_position_pantilt_y;
	float home_position_zoom_x;
}PTZ_CONFIGURATION;

/*系统时间结构体*/
typedef struct 
{
	int year;
	unsigned char month;
	unsigned char date;
	unsigned char hour;
	unsigned char min;
    	unsigned char sec;
	unsigned char zone;
}DATETIME;

/*定义系统服务配置结构体*/
typedef struct 
{
	char name[16];
    	char *buf;
	unsigned char enable;
}SERVICE_CONF;

/*系统时间区结构体*/
typedef struct 
{
	unsigned char  zone;
	unsigned char enable;
	char    server[MAX_SERVER_ADDR_LEN];      /*ntp服务器名字长度限制为64*/
	char	   definetimezone[LEN_128_SIZE];
}TIME_ZONE_CONF;

/*ftp 服务配置结构体*/
typedef struct 
{
	char server[LEN_256_SIZE];
	char auth_user[MAX_USERNAME_LEN];
	char auth_passwd[MAX_USERPASSWD_LEN];
	char path_name[MAX_PATHNAME_LEN];
	unsigned int port;
	unsigned char enable;
}FTP_CONF;

/*pppoe 配置结构体*/
typedef struct 
{
	char auth_user[MAX_USERNAME_LEN];
	char auth_passwd[MAX_USERPASSWD_LEN];
	unsigned char enable;
	int  mtu;
}PPPOE_CONF;

/*smtp 配置结构体*/
typedef struct 
{
	char mail_hub[LEN_256_SIZE];     /*smtp 服务器地址*/
	char mail_addr[MAX_EMAIL_ADDR_LEN];    /*发送邮件 地址*/
	char auth_user[MAX_EMAIL_ADDR_LEN];    /*发送邮件 账号*/ 
	char auth_passwd[MAX_USERPASSWD_LEN];  
	char rcpt1[MAX_EMAIL_ADDR_LEN];         /*接受邮件 地址1*/
	char rcpt2[MAX_EMAIL_ADDR_LEN];         /*接受邮件 地址2*/
	char rcpt3[MAX_EMAIL_ADDR_LEN];         /*接受邮件 地址3*/
	unsigned char  use_tls;                /*使用安全传输SSL*/
	unsigned char enable;
	int  port;
}SSMTP_CONF;

/*http 配置结构体*/
typedef struct 
{
	int port;
    	unsigned char use_tls;
}HTTP_CONF;


/*alarm time配置结构体*/
typedef struct 
{
	unsigned int  alarm_device;
	unsigned int  week;
	unsigned int  start_time;
	unsigned int  end_time;
}ALARM_TIME_CONF;

/*wificonf 配置结构体*/

typedef struct 
{
	char id;
	char type;
	char channel;
	char ssid[LEN_128_SIZE];
	char dhcp;
	char ip[LEN_16_SIZE];
	char mask[LEN_16_SIZE];
	char gateway[LEN_16_SIZE];
	char enabled;
}WIFI_CONF;

/*wifiencrypt 配置结构体*/

typedef struct 
{
	char id;	  
	char encrypt;
	char arythmetic;
	char key[LEN_64_SIZE];
	char enabled;
}WIFI_ENCRYPT;

/*sip 配置结构体*/
typedef struct 
{
	int  id;
	unsigned int enabled;
	unsigned int state;
	unsigned int registered;
	char name[LEN_128_SIZE];
	char proxy[LEN_256_SIZE];
	char server[LEN_256_SIZE];
	char nat[LEN_64_SIZE];
	char sipid[LEN_128_SIZE];
	char authenticate[LEN_128_SIZE];
	char pwd[LEN_128_SIZE];
	char expires[LEN_32_SIZE];
	char vocoder[LEN_16_SIZE];
	unsigned int port;
	unsigned int rtpport;
}SIP_CONF;

/*sip phone 配置结构体*/
typedef struct 
{
	int  id;
	char tel[20];
	char remark[LEN_128_SIZE];
	char enabled;
}PHONE_CONF;

/*video 配置结构体*/
typedef struct
{
	unsigned char chn;
	unsigned char catype;
	unsigned char piclevel;
	unsigned char cbr;
	int entype;
	int width;
	int height;
	int bitrate;
	int fps;
	int gop;
	int AudioEnable;
}VIDEO_CONF;

/*移动侦测时间结构体*/
typedef struct md_time_conf
{
	unsigned int chn;
	unsigned int region_id;
	unsigned int week;
	unsigned int start_time;
	unsigned int end_time;
}MD_TIME_CONF;

/*侦测区域结构体*/
typedef struct md_region_conf
{
	unsigned int chn;
	unsigned int region_id;
	unsigned int md_x1;
	unsigned int md_y1;
	unsigned int md_x2;
	unsigned int md_y2;
	unsigned int sensitive;
	unsigned int enable;
}MD_REGION_CONF;

/*报警联动录像结构体*/
typedef struct chn_record_conf
{
	int storage_type;
	int per_second;
	int end_second;
}RECORD_CONF;

/*联动报警结构体*/
typedef struct alarm_action_conf
{
	int chn;
	int alarm_out;
	int info_center;
	int ftp;
	int email;
	int sip;
	RECORD_CONF record;
}ALARM_ACTION_CONF;

/*osd 配置结构体*/
typedef struct
{
	unsigned char id;
	unsigned char style;
	unsigned char chn;
	unsigned char alpha;
	unsigned char data[MAX_OSD_TEXT_LEN];
	unsigned char enable;
	char color[LEN_16_SIZE];
	int xpos;
	int ypos;
}OSD_CONF;

/*audio 配置结构体*/
typedef struct
{
	unsigned char chn;
	unsigned char cpress;
	unsigned char dformat;
}AUDIO_CONF;


/*双向语音输入/输出设置结构体*/
typedef struct 
{
     int  linein;				/*1外置MIC输入 0内置MIC输入*/
     int  lineout;			/*1外置音箱输出 0内置音箱输出*/
     int  micro_adjust;		/*输入音量调节*/
     int  speaker_adjust;	/*输出音量调节*/
}AUDIO_IN_OUT;

/*video audio osd speed up buffer*/
typedef struct 
{
	VIDEO_CONF main_video;
	VIDEO_CONF sub_video;
	OSD_CONF osd_time;
	OSD_CONF osd_string;
	AUDIO_CONF audio;
	AUDIO_IN_OUT audio_in_out;
}SPEEDUP_VIDAUD_S;

/*ptz setting配置结构体*/
typedef struct 
{
	unsigned int  id;	
	unsigned int  protocol;				
	unsigned int  baudrate;
}PTZ_CONF;

/* alias 配置结构体*/
typedef struct 
{
	char id;
	char devname[LEN_64_SIZE];
}ALIAS_CONF;

/* dynadns 配置结构体*/
typedef struct 
{
	char enabled;
	char dns[LEN_256_SIZE];	
	char nameserver[LEN_256_SIZE];				
	char port[5];
	char user[LEN_128_SIZE];
	char pwd[LEN_128_SIZE];
}DYNADNS_CONF;

/*upgrade 配置结构体*/
typedef struct 
{
	int  enabled;
	int	 transport;
	int  interval;
	char warepath[LEN_256_SIZE];	
	char configpath[LEN_256_SIZE];		
}UPGRADE;

/* system log 配置结构体 */
typedef struct
{
	int 	level;
	char	server[LEN_128_SIZE];
}SYS_LOG;

/*wireless 认证结构体定义*/
typedef struct 
{
	int encryption;
	int shared;
	int key_index;
	char ssid[ENCRYPTION_SSID_SIZE];
	char key[ENCRYPTION_KEY_SIZE];
}WIFI_AUTH_CONF;

/*ccd 认证结构体定义*/
typedef struct 
{
	char colortoblack;		//彩转黑模式
	char mirrormode;		//镜像模式	
	char blc;				//背光补偿
	char shutter;			//快门设置
	char shutterspeed;		//快门速度
	char wbc;				//白平衡控制
	int autosqure;			//自动光圈控制
	int agc;				//自动增益控制
}CCD_CONF;

/****************onvif*********************/

#define   MG_INDEX			3
#define   SCOP_NUM			7
#define 	RESET_SCRIPT		"/app/bin/reset.sh"
#define	VIRT_MSG_PATH		"/tmp/"
#define 	RESTORE_SCRIPT  		"/app/bin/restore.sh"

#define	DISCOV_MSG_TYPE	7

enum dis_moth
{
	dis_hello = 1, 
	dis_probe,
	dis_noprobe,
	dis_bye,
	dis_scope,
	dis_mode,
	system_restore,
	hard_reset
};

typedef struct{
	char  mg[MG_INDEX][LEN_32_SIZE];
	char reason_txt[LEN_128_SIZE];
}FAULT_MG;

typedef struct{
	char  mg[2][LEN_32_SIZE];
	char reason_txt[LEN_128_SIZE];
}FAULT_MG_TWO;

enum set_t_mod
{
	manual = 0, 
	NTP = 1
};
enum isdaylight
{
	no = 0, 
	yes = 1
};

typedef struct{
	enum set_t_mod set_mod;
	enum isdaylight dlight;
	char time_zone[LEN_128_SIZE];
	int hour;	
	int minute;	
	int second;	
	int year;
	int month;	
	int day;
}SYSDATE_TIME;


typedef struct{
	long iMsgType;
	unsigned long ulModPid;
	unsigned long ulMsgType;
	unsigned long ulPara0;
	unsigned long channel;
	char acBuf[LEN_512_SIZE];
}MSG_TO_DIS;

enum scope_type
{
	scope_add,
	scope_del,
};

enum scope_def 
{
	fixed = 0, 
	config = 1
};

typedef struct gs_scopes
{
	enum scope_def s_def;
	char scope[LEN_64_SIZE];
} GS_SCOPES;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif




