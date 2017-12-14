#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#include "spv_file_manager.h"

#define SCANCODE_SPV_MENU (SCANCODE_USER + 1)
#define SCANCODE_SPV_MODE (SCANCODE_USER + 2)
#define SCANCODE_SPV_OK (SCANCODE_USER + 3)
#define SCANCODE_SPV_DOWN (SCANCODE_USER + 4)
#define SCANCODE_SPV_UP (SCANCODE_USER + 5)
#define SCANCODE_SPV_POWER (SCANCODE_USER + 6)
#define SCANCODE_SPV_SHUTDOWN (SCANCODE_USER + 7)
#define SCANCODE_SPV_RESET (SCANCODE_USER + 8)
#define SCANCODE_SPV_SWITCH (SCANCODE_USER + 9)
#define SCANCODE_SPV_LOCK (SCANCODE_USER + 10)
#define SCANCODE_SPV_PHOTO (SCANCODE_USER + 11)


#define MSG_VALUE_CHANGED        MSG_USER+102
#define MSG_CHANGE_MODE          MSG_USER+103
#define MSG_SHUTTER_PRESSED      MSG_USER+104
#define MSG_UPDATE_WINDOW        MSG_USER+105
#define MSG_SET_FOLDER_TYPE      MSG_USER+106
#define MSG_FOCUS_FOLDER         MSG_USER+107
#define MSG_CLEAN_SECONDARY_DC   MSG_USER+110
#define MSG_RESTORE_DEFAULT      MSG_USER+111
#define MSG_SET_WIFI_STATUS      MSG_USER+112
#define MSG_INIT_FOLDERS MSG_USER+501

#define MSG_LDWS MSG_USER+201
#define MSG_FCWS MSG_USER+202
#define MSG_ADAS MSG_USER+203

#define TIMER_CLOCK 100
#define TIMER_DATETIME 101
#define TIMER_COUNTDOWN 102
#define TIMER_DIALOG_HIDE 103
#define TIMER_DELAY_SCANNING 104

#define GET_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))
#define SPV_COLOR_TRANSPARENT RGB2Pixel(HDC_SCREEN, 0x01, 0x01, 0x01)

typedef enum
{
    INFO_DIALOG_CAMERA_BUSY,
    INFO_DIALOG_SD_ERROR,
    INFO_DIALOG_SD_EMPTY,
    INFO_DIALOG_SD_FULL,
    INFO_DIALOG_SD_NO,
    INFO_DIALOG_RESET_VIDEO,
    INFO_DIALOG_RESET_PHOTO,
    INFO_DIALOG_VOLUME,
    INFO_DIALOG_WIFI_ENABLED,
    INFO_DIALOG_WIFI_DISABLE,
    INFO_DIALOG_WIFI_INFO,
    INFO_DIALOG_PAIRING,
    INFO_DIALOG_PAIRING_STOPPED,
    INFO_DIALOG_DELETE_LAST,
    INFO_DIALOG_DELETE_ALL,
    INFO_DIALOG_DELETE_PROG,
    INFO_DIALOG_RESET_CAMERA,
    INFO_DIALOG_RESET_WIFI,
    INFO_DIALOG_RESET_WIFI_PRGO,
	INFO_DIALOG_SET_DATETIME,
    INFO_DIALOG_SET_DATE,
    INFO_DIALOG_SET_TIME,
    INFO_DIALOG_SET_PLATE,
    INFO_DIALOG_SHUTDOWN,
    INFO_DIALOG_FORMAT,
    INFO_DIALOG_FORMAT_PROG,
    INFO_DIALOG_ADAS_CALIBRATE,
	INFO_DIALOG_BATTERY_LOW,
	INFO_DIALOG_VERSION,
} INFO_DIALOG_TYPE;

typedef enum {
    TOAST_SD_ERROR,
    TOAST_SD_EMPTY,
    TOAST_SD_FULL,
    TOAST_SD_NO,
    TOAST_SD_FORMAT,
    TOAST_SD_PREPARE,
    TOAST_CAMERA_BUSY,
    TOAST_PLAYBACK_ERROR,
} TOAST_TYPE;

typedef enum {
    LANG_EN,
    LANG_ZH,
    LANG_TW,
    LANG_FR,
    LANG_ES,
    LANG_IT,
    LANG_PT,
    LANG_DE,
    LANG_RU,
	LANG_KR,
	LANG_JP
} LANGUAGE_TYPE;

typedef enum {
    MW_STYLE_TEXT,
    MW_STYLE_ICON,
} MW_STYLE;

typedef enum {
	GUI_FONTSIZE_SMALL,
	GUI_FONTSIZE_MIDDLE,
	GUI_FONTSIZE_LARGE,
} GUI_FONTSIZE;

//window size define
extern int DEVICE_WIDTH;
extern int DEVICE_HEIGHT;
extern int VIEWPORT_WIDTH;
extern int VIEWPORT_HEIGHT;

extern int LARGE_SCREEN;

extern int SCALE_FACTOR;

//main window
extern int MW_BAR_HEIGHT;

extern int MW_ICON_WIDTH;
extern int MW_ICON_HEIGHT;
extern int MW_SMALL_ICON_WIDTH;
extern int MW_ICON_PADDING;
extern int MW_ITEM_PADDING;

//setup window 
extern int SETUP_ITEM_HEIGHT;

//folder window
extern int FOLDER_DIALOG_TITLE_HEIGHT;

extern int FOLDER_WIDTH;
extern int FOLDER_HEIGHT;
extern int FOLDER_INFO_HEIGHT;

//playback window
extern int PB_BAR_HEIGHT;
extern int PB_PROGRESS_HEIGHT;

extern int PB_ICON_WIDTH;
extern int PB_ICON_HEIGHT;
extern int PB_ICON_PADDING;

extern int PB_FOCUS_WIDTH;
extern int PB_FOCUS_HEIGHT;

extern const char *SPV_ICON[];

extern const char *SPV_WAV[];

extern BITMAP *g_icon;

extern FileList g_media_files[MEDIA_COUNT];

extern int g_fucking_volume;

extern int AUTO_PLAY;
extern int CAMERA_REAR_ENABLED;
extern int CAMERA_REAR_CONNECTED;
extern int WIFI_ENABLED;

int ImmediatlyMediaFilesCount();
int GetBatteryIcon();
void LoadGlobalIcon(int iconId);
HDC BeginSpvPaint(HWND hWnd);
void CleanupSecondaryDC(HWND hWnd);

void GoingToScanMedias(HWND hwnd, int delay);
void SetDelayedShutdwonStatus();
void SetAutoPowerOffStatus();
void SetDisplaySleepStatus();
void LightLcdStatus();
void SetHighLoadStatus(int highLoad);
void SetKeyTone();
void SetIrLed();
void GetWifiApInfo(char *ssid, char *pswd);
//void SpvSetWifiStatus(PWifiOpt pWifi); 
void DealSdEvent(HWND hWnd, WPARAM wParam);
int SpvGetWifiStatus(); 
int IsSdcardMounted();
int GetSdcardStatus();
int GetPhotoCount();
int ResetConfig(char *filepath, char *resetkey);

HWND ShowSDErrorToast(HWND hWnd);

HWND GetMainWindow();

void ScanSdcardMediaFiles(BOOL sdInserted);
void GetMediaFileCount(int *videoCount, int *photoCount);

void SyncMediaList(FileList *list);

void ResetCamera();
BOOL SetCameraConfig(BOOL directly);

#endif

