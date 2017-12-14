#ifndef __SPV_UTILS_H__
#define __SPV_UTILS_H__

#include<time.h>
#include<sys/time.h>

#include "wifi_softap.h"
#include "led.h"
#include "lcd_backlight.h"
#include "power.h"
#include "codec_volume.h"
#include "sd_stuff.h"
#include "gsensor.h"
#include "libramain.h"
#include "spv_media.h"

#include "common_inft.h"
#include "file_dir.h"

#define MPS_1080FHD 3.0f
#define MPS_720P_60FPS 3.0f
#define MPS_720P_30FPS 2.0f
#define MPS_WVGA 1.0f
#define MPS_VGA 0.5f

#define MPP_12M 3.6f
#define MPP_10M 3.0f
#define MPP_8M 2.4f
#define MPP_5M 1.6f
#define MPP_3M 1.1f
#define MPP_2MHD 0.7394f
#define MPP_VGA 0.1721f
#define MPP_1P3M 0.4417f


//关闭屏幕或者点亮屏幕：blank非0表示关闭屏幕，blank为0表示点亮屏幕。 	OK
//void lcd_blank(int blank);

//设置屏幕的亮度，brightness表示亮度值，范围是0-255。
//没有出现error返回0，出现error返回errno。
//int set_backlight(int brightness);

//获取屏幕的亮度值，返回值范围是0-255。
//没有出现error返回0，出现error返回errno。
//int get_backlight(void);

//关机
//void shutdown(void);

//开关led灯，sw非0表示打开led灯，sw为0表示关闭led灯。
//没有出现error返回0，出现error返回errno。 	OK
//int led_switch(int sw);

//设置led灯闪烁，times表示闪烁的次数。每一次闪烁的实现是：打开led灯，sleep 200ms，
//之后关闭led灯，sleep 200ms。
//void set_led_flicker(int times);

//（非库函数） 	使能mixer，使用的是alsa库函数，整个步骤包括：打开混音器，连接默认混音器，
//注册混音器和加载混音器。
//没有出现error返回0，出现error返回errno。
//int open_mixer(struct volume_mixer *volume_t);

//（非库函数） 	关闭mixer。
//void close_mixer(struct volume_mixer *volume_t);

//设置音量大小，volume的范围是min_volume-max_volume，这两个值会通过
//Snd_mixer_selem_get_playback_volume_range去获取。
//没有出现error返回0，出现error返回errno。
//int set_volume(int volume);

//获取音量大小，返回值的范围同上。
//int get_volume(void);

//使能softap，默认配置如下：
//ssid="INFOTM_IPC_标示符" 其实标示符是wifi mac地址后六位
//key_mgmt="WPA2PSK"
//Key="admin888"
//网关192.168.155.0，本机ip为192.168.155.1，使能udhcpd
//int start_softap(void);

//关闭softap，目前的想法是除了kill udhcpcd wpa_supplicant hostapd等后台进程，关闭wlan1，
//另外还需要把wifi模组电源关掉。
//int stop_softap(void);

//检测softap是否已经打开了，返回值0表示没有打开，1表示已经打开了。
//int is_softap_started(void);

//修改softap的ssid和key。
//没有出现error返回0，出现error返回errno。
//int set_softap(char *ssid, char *key);

//返回当前softap使用的ssid和key，通过指针传递。
//没有出现error返回0，出现error返回errno。
//int get_softap(char *ssid, char *key);

//provided by libgui
extern int gui_sdcard_status;

typedef enum {
    SD_STATUS_UMOUNT,
    SD_STATUS_MOUNTED,
    SD_STATUS_ERROR,
    SD_STATUS_NEED_FORMAT,
    SD_PREPARE,
} SD_STATUS;

typedef enum {
    CMD_UNMOUNT_END,
    CMD_UNMOUNT_BEGIN,
} SYS_COMMAND;


int SpvSetResolution();
int SetTimeToSystem(struct timeval tv, struct timezone tz);
int SpvTellSysGoingToUnmount(SYS_COMMAND cmd);
int SpvRunSystemCmd(char *cmd);

int SpvGetVideoDuration(char *videoPath);

int SpvGetWifiSSID(char *ssid);

void SpvSetBacklight();

int SpvUpdateGpsData(GPS_Data *data);
void SpvPlayAudio(const char *wav_path);

void SpvTiredAlarm(const char *wav_path);

void SpvSetIRLED(int closeLed, int isDayTime);

void SpvShutdown();

int SpvGetBattery();

int SpvAdapterConnected();

int SpvPowerByGsensor();

int SpvIsCharging();

int SpvGetSdcardStatus();
int SpvFormatSdcard();

void SpvSetParkingGuard();

unsigned long long GetAvailableSpace();

int GetVideoTimeRemain(long availableSpace, char *resolution);

int GetPictureRemain(long availableSpace, char *resolution);

void TimeToString(int time, char *ts);

void TimeToStringNoHour(int time, char *ts);

int GetPictureCount();

int SubstringIsDigital(char *string, int start, int end);

MEDIA_TYPE GetFileType(char *fileName);

int IsMediaFolder(char *folderName);

int ScanFolders(FileList *fileList, char *dirPath);

void FreeFileList(FileList *fileList);

int GetMediaCount(int *pVideoCount, int *pPictureCount);

int CompareFileNode(PFileNode node1, PFileNode node2);

int MakeExternalDir();

int copy_file(const char* src, const char* dst, int use_src_attr);
#endif //__SPV_UTILS_H__
