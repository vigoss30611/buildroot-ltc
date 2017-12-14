#ifndef __SPV_COMMON__
#define __SPV_COMMON__

#include <stdint.h>
#include <sys/time.h>
#include <semaphore.h>
//#include "common_inft.h"
#include "sys_server.h"

#define	KEY_SIZE		128
#define VALUE_SIZE		128

#define LINE_BUF_SIZE	256

typedef long 			HMSG;

typedef unsigned long	DWORD;
typedef int 			BOOL;
#define FALSE       	0
#define TRUE        	1

#define BATTERY_DIV 26

typedef enum {
    TYPE_VIDEO_SETTINGS,
    TYPE_CAR_SETTINGS,
    TYPE_PHOTO_SETTINGS,
    TYPE_SETUP_SETTINGS,
    TYPE_WIFI_SETTINGS,
    TYPE_DISPLAY_SETTINGS,
    TYPE_DATE_TIME_SETTINGS,
    TYPE_DELETE_SETTINGS,
    TYPE_CAMERA_RESET_SETTINGS,
	TYPE_SETUP_PARAM
} DIALOG_SETUP_TYPE;

typedef enum {
    WAV_BOOTING,
    WAV_BEGIN = WAV_BOOTING,
    WAV_ERROR,
    WAV_KEY,
    WAV_PHOTO,
    WAV_PHOTO3,
    WAV_LDWS_WARN,
    WAV_FCWS_WARN,
    WAV_END = WAV_FCWS_WARN,
} WAV_ENUM;

typedef struct {
	int idle;
	int highLoad;
	struct timespec idleTime;
    struct timespec dischargeTime;
    int autoSleep;
	int delayedShutdown;
	int autoPowerOff;
} SleepStatus;
typedef SleepStatus* PSleepStatus;

extern const char *SPV_WAV[];

PSleepStatus GetSleepStatus();
int IsLoopingRecord();
int SpvIsRecording();

int GetLauncherHandler();

int IsSdcardMounted();

int SpvGetChargingStatus();

int SpvIsOnLine();

void ResetCamera();

int SpvKeyToneEnabled();

int DispatchMsgToClient(int msg, int wParam, long lParam, int from);

#endif
