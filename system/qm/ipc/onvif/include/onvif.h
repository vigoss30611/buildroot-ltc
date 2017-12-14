#ifndef _ONVIF_H
#define _ONVIF_H

#include <time.h>
#include <pthread.h>
#include "cgiinterface.h"
#include "QMAPICommon.h"

enum Reboot_Flag
{
	NORMAL_REBOOT = 1, 
	RESTORE_REBOOT = 2,
	RESET_REBOOT = 3,
	AVSERVER_REBOOT = 4,
	DISCOVERY_REBOOT=5,
	REBOOT_FLAG_IDLE=6
};

struct onvif_profile_config
{
	int isVaild;
	char token[64];
	char name[64];
	char audioSoureToken[64];
	char audioEncoderToken[64];
	char videoSoureToken[64];
	char videoEncoderToken[64];
};

#define MAX_NOTIFY_CLIENT 10

typedef struct _NOTIFY_ITEM
{
 	unsigned int id;
 	unsigned int status;
	unsigned int ioStatus;
 	char *endpoint;
	time_t endTime;
	unsigned char bNoFirst;
	unsigned char bIoNoFirst;
	unsigned char bEnableMotion;
	unsigned char bEnableIO;
}NOTIFY_ITEM;

typedef struct _NOTIFY_INFO
{
   NOTIFY_ITEM notify[MAX_NOTIFY_CLIENT];
   int totalActiveClient;
}NOTIFY_INFO;

struct pullnode
{
	int id;
	time_t endTime;
	unsigned int status;
	unsigned int ioStatus;
	unsigned char bNoFirst;
	unsigned char bNoIoFirst;
	unsigned char bEnableMotion;
	unsigned char bEnableIO;
};

typedef void (*onGetUtcTime)(time_t *, QMAPI_SYSTEMTIME *);
typedef int (*onCodeConvert)(const char *, const char *, char *, size_t , char *, size_t *);
typedef int (*onConvertTimeZone)(int , int *, int *);

struct onvif_config
{
	onGetUtcTime GetUtcTime;
	onCodeConvert CodeConvert;
	onConvertTimeZone ConvertTimeZone;

	char domainname[20];
	unsigned int system_reboot_flag;
	int g_channel_num;

	unsigned int MAlarm_status;
	unsigned int IOAlarm_status;

	int g_OnvifCpuType;
	
	struct onvif_profile_config profile_cfg;

	char sys_time_zone[128];

	char onvif_username[3][32];
	char onvif_password[3][32];
	int onvif_userLevel[3];

	int isNTPFromDHCP;
	int networkProtocol[2];

	unsigned int relayMode;
	unsigned int relayIdleState;
	int relayDelayTime;

	int videosource_width;
	int videosource_height;

	int outputLevel;

	struct pullnode onvif_PullNode[MAX_NOTIFY_CLIENT];

	pthread_mutex_t onvif_pullLock;
	unsigned int pullInex;

	NOTIFY_INFO g_NotifyInfo;

	pthread_mutex_t	gNotifyLock;
	unsigned int SubscriptionIndex;
	int discoveryMode;
};

__attribute__ ((visibility("default"))) int Onvif_Server(httpd_conn* hc, struct onvif_config *pConfig);

__attribute__ ((visibility("default"))) int ONVIF_EventServer(int sockfd, struct onvif_config *pConfig);

__attribute__ ((visibility("default"))) void ONVIF_EventServerUnInit();

#endif
