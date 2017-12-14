#ifndef __SPV_UTILS_H__
#define __SPV_UTILS_H__

#include<time.h>
#include<sys/time.h>
#include "spv_common.h"
#include "spv_wifi.h"
#include "spv_config.h"
#include "spv_file_manager.h"


#define MPS_1080FHD 1.0f
#define MPS_720P_60FPS 1.0f
#define MPS_720P_30FPS 1.0f
#define MPS_WVGA 1.0f
#define MPS_VGA 0.5f
#define MPS_2448_30FPS 1.0f
#define MPS_1080P_60FPS 1.0f
#define MPS_1080P_30FPS 1.0f
#define MPS_3264 2.0f
#define MPS_2880 1.5f
#define MPS_2048 1.0f

#define MPP_16M 4.8f
#define MPP_12M 3.6f
#define MPP_10M 3.0f
#define MPP_8M 2.4f
#define MPP_5M 1.6f
#define MPP_4M 1.35f
#define MPP_3M 1.1f
#define MPP_2MHD 0.7394f
#define MPP_VGA 0.1721f
#define MPP_1P3M 0.4417f

typedef enum {
    CMD_UNMOUNT_END,
    CMD_UNMOUNT_BEGIN,
} SYS_COMMAND;

typedef enum {
    LED_WORK,
    LED_WORK1,
    LED_WIFI,
    LED_BATTERY,
    LED_BATTERY_G,
    LED_BATTERY_B,
} LED_TYPE;

typedef struct __BuzzerUnit {
    int freq;
    int duration;
    int interval;
    int times;
} BuzzerUnit;


int SpvRunSystemCmd(char *cmd);
int SetTimeByRawData(time_t rawtime);
int SetTimeToSystem(struct timeval tv, struct timezone tz);

int SpvGetWifiSSID(char *ssid);

int SpvSetBacklight();
int SpvGetBacklight();
int SpvLcdBlank(int blank);
int SpvSetLed(LED_TYPE led, int time_on, int time_off, int blink_on);

int SpvGetBattery();

int SpvAdapterConnected();

int SpvPowerByGsensor();

int SpvIsCharging();


void SpvSetIRLED(int closeLed, int isDayTime);
int SpvUpdateGpsData(GPS_Data *data);

int SpvSetVolume(int volume);
int SpvGetVolume(void);
void SpvPlayAudio(int param);

void SpvTiredAlarm(const char *wav_path);

unsigned long int SpvCreateServerThread(void (*func(void *arg)), void *args);
void SpvShutdown();

int SpvGetSdcardStatus();
int SpvFormatSdcard();

int SpvSetParkingGuard();

unsigned long long GetAvailableSpace();

int GetVideoTimeRemain(long availableSpace, char *resolution);

int GetPictureRemain(long availableSpace, char *resolution);

void TimeToString(int time, char *ts);

void TimeToStringNoHour(int time, char *ts);

int GetPictureCount();

int SubstringIsDigital(char *string, int start, int end);

MEDIA_TYPE GetFileType(char *fileName);

MEDIA_TYPE GetMediaFolderType(char *folderName);
int IsMediaFolder(char *folderName);

int ScanFolders(FileList *fileList, char *dirPath);

void FreeFileList(FileList *fileList);

int GetMediaCount(int *pVideoCount, int *pPictureCount);

int CompareFileNode(PFileNode node1, PFileNode node2);

int MakeDirRecursive(char *dirpath);

int copy_file(const char* src, const char* dst, int use_src_attr);

int SpvGetVersion(char *version);

int SpvEnterCalibrationMode();
#endif //__SPV_UTILS_H__
