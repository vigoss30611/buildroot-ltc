#ifndef __SPV_COMMON_H__
#define __SPV_COMMON_H__

#include<minigui/common.h>
#include<minigui/gdi.h>
#include<minigui/window.h>

#include"spv_wifi.h"
#include"libramain.h"
#include"spv_utils.h"
#include"spv_wav_res.h"

//#define UBUNTU
#ifdef UBUNTU
#define SCANCODE_SPV_MENU 31 //'s' settings arrow up
#define SCANCODE_SPV_MODE 50 //'m' mode arrow down
#define SCANCODE_SPV_OK   24 //'o' ok arrow left
#define SCANCODE_SPV_DOWN 32 //'d' down arrow right
#define SCANCODE_SPV_UP 22 //'u'
#define SCANCODE_SPV_POWER 25 //'p'
#define SCANCODE_SPV_SHUTDOWN 20 //'t'
#define SCANCODE_SPV_RESET 19 //'r'
#define SCANCODE_SPV_SWITCH 34//'g'
#define SCANCODE_SPV_LOCK 35//'h'
#else 
/*##### self-defined scancode by Jeason @ infotm#####*/
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
/*##### self-defined scancode end #####*/
#endif

/*##### self-defined MSGs by Jeason @ infotm#####*/
#define MSG_GET_UI_STATUS (MSG_USER + 3)
#define MSG_SHUTDOWN (MSG_USER + 4)
#define MSG_IR_LED (MSG_USER + 5)
#define MSG_COLLISION (MSG_USER + 6)
#define MSG_CHARGE_STATUS (MSG_USER + 10)
#define MSG_BATTERY_STATUS (MSG_USER + 11)
#define MSG_SDCARD_MOUNT (MSG_USER + 20)
#define MSG_HDMI_HOTPLUG (MSG_USER + 30)
#define MSG_REAR_CAMERA (MSG_USER + 31)
#define MSG_REVERSE_IMAGE (MSG_USER + 32)
#define MSG_GPS (MSG_USER + 33)
#define MSG_ACC_CONNECT (MSG_USER + 34)
/*##### self-defined MSGs end #####*/

#define MSG_PAIRING_SUCCEED      MSG_USER+101
#define MSG_VALUE_CHANGED        MSG_USER+102
#define MSG_CHANGE_MODE          MSG_USER+103
#define MSG_SHUTTER_PRESSED      MSG_USER+104
#define MSG_UPDATE_WINDOW        MSG_USER+105
#define MSG_SET_FOLDER_TYPE      MSG_USER+106
#define MSG_FOCUS_FOLDER         MSG_USER+107
#define MSG_MEDIA_SCAN_FINISHED  MSG_USER+108
#define MSG_CAMERA_CALLBACK      MSG_USER+109
#define MSG_CLEAN_SECONDARY_DC   MSG_USER+110
#define MSG_RESTORE_DEFAULT      MSG_USER+111
#define MSG_SET_WIFI_STATUS      MSG_USER+112

#define MSG_LDWS MSG_USER+201
#define MSG_FCWS MSG_USER+202
#define MSG_ADAS MSG_USER+203

#define MSG_INIT_FOLDERS MSG_USER+501

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

#ifndef KEY_SIZE
#define KEY_SIZE        128 // key缓冲区大小
#define VALUE_SIZE      128 // value缓冲区大小
#define LINE_BUF_SIZE   256 // 读取配置文件中每一行的缓冲区大小
#endif

#define TIMER_CLOCK 100
#define TIMER_DATETIME 101
#define TIMER_COUNTDOWN 102
#define TIMER_DIALOG_HIDE 103
#define TIMER_DELAY_SCANNING 103

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define GET_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#ifdef UBUNTU
#define SPV_COLOR_TRANSPARENT PIXEL_blue 
#else
#define SPV_COLOR_TRANSPARENT RGB2Pixel(HDC_SCREEN, 0x01, 0x01, 0x01)
#endif

typedef enum
{
    //video
    //top left
    MWV_MODE,
    MWI_VIDEO_BEGIN = MWV_MODE,
    MWV_LAPSE,

    //top right
    MWV_BATTERY,
    MWV_WIFI,
#ifndef GUI_SPORTDV_ENABLE
    MWV_DATETIME,
#endif

    //bottom left
    MWV_RESOLUTION,
    MWV_INTERVAL,
    MWV_ZOOM,

    //bottom right
    MWV_WDR,
    MWV_EV,
#ifdef GUI_SPORTDV_ENABLE
    MWI_VIDEO_END = MWV_EV,
#else
    MWV_LIGHT,
    MWV_AUDIO,
    MWV_LAMP,
    MWV_LOCKED,
    MWI_VIDEO_END = MWV_LOCKED,
#endif

    //photo
    //top left
    MWP_MODE,
    MWI_PHOTO_BEGIN = MWP_MODE,
    MWP_COUNT,
    MWP_TIMER,

    //top right
    MWP_BATTERY,
    MWP_WIFI,
    MWP_ANTI_SHAKING,

    //bottom left
    MWP_RESOLUTION,
    MWP_QUALITY,
    MWP_SEQUENCE,
    MWP_ZOOM,

    //bottom right
    MWP_ISO,
    MWP_EV,
    MWP_WB,
#ifdef GUI_SPORTDV_ENABLE
    MWI_PHOTO_END = MWP_WB,
#else
    MWP_LIGHT,
    MWP_LAMP,
    MWI_PHOTO_END = MWP_LAMP,
#endif

#ifndef GUI_SPORTDV_ENABLE
    MWI_MAX = MWI_PHOTO_END,
#else
    MWV_DATETIME,
    MWV_LIGHT,
    MWV_AUDIO,
    MWV_LAMP,
    MWV_LOCKED,
    MWP_LIGHT,
    MWP_LAMP,
    MWI_MAX = MWP_LAMP,
#endif

} MWI_TYPE;

typedef enum {
    MW_STYLE_TEXT,
    MW_STYLE_ICON,
} MW_STYLE;

typedef enum
{
    MODE_VIDEO,
    MODE_PHOTO,
    MODE_PLAYBACK,
#if USE_SETTINGS_MODE
    MODE_SETUP,
    MODE_COUNT,
#else
    MODE_COUNT,
    MODE_SETUP,
#endif
} SPV_MODE;

typedef enum {
    TYPE_VIDEO_SETTINGS,
    TYPE_CAR_SETTINGS,
    TYPE_PHOTO_SETTINGS,
    TYPE_SETUP_SETTINGS,
    TYPE_WIFI_SETTINGS,
    TYPE_DISPLAY_SETTINGS,
    TYPE_DATE_TIME_SETTINGS,
    TYPE_DELETE_SETTINGS,
    TYPE_CAMERA_RESET_SETTINGS
} DIALOG_SETUP_TYPE;

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
    INFO_DIALOG_PAIRING,
    INFO_DIALOG_PAIRING_STOPPED,
    INFO_DIALOG_DELETE_LAST,
    INFO_DIALOG_DELETE_ALL,
    INFO_DIALOG_DELETE_PROG,
    INFO_DIALOG_RESET_CAMERA,
    INFO_DIALOG_RESET_WIFI,
    INFO_DIALOG_RESET_WIFI_PRGO,
    INFO_DIALOG_SET_DATE,
    INFO_DIALOG_SET_TIME,
    INFO_DIALOG_SET_PLATE,
    INFO_DIALOG_SHUTDOWN,
    INFO_DIALOG_FORMAT,
    INFO_DIALOG_FORMAT_PROG,
    INFO_DIALOG_ADAS_CALIBRATE,
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
    WIFI_OFF,
    WIFI_CLOSING,
    WIFI_OPEN_AP,
    WIFI_OPEN_STA,
    WIFI_AP_ON,
    WIFI_STA_ON,
} WIFI_STATUS;

typedef enum
{
    IC_VIDEO = 1,
    IC_VIDEO_RECORDING,
    IC_VIDEO_FOCUS,
    IC_VIDEO_FOLDER,
    IC_VIDEO_1MIN,
    IC_VIDEO_3MIN,
    IC_VIDEO_5MIN,
    IC_VIDEO_10MIN,
    IC_LOCKED,
    IC_VIDEO_LOCKED_FOLDER,
    IC_PHOTO,
    IC_PHOTO_RECORDING,
    IC_PHOTO_FOCUS,
    IC_PHOTO_FOLDER,
    IC_PHOTO_SEQUENCE,
    IC_PHOTO_TIMER_2S,
    IC_PHOTO_TIMER_5S,
    IC_PHOTO_TIMER_10S,
    IC_PLAYBACK_MODE,
    IC_PLAYBACK_FOLDER,
    IC_GRID,
    IC_PLAY,
    IC_PAUSE,
    IC_PREV,
    IC_NEXT,
    IC_REWIND,
    IC_FAST_FORWARD,
    IC_VOLUME,
    IC_VOLUME_MUTE,
    IC_VOLUME_MAX,
    IC_SETUP,
    IC_SETUP_MODE,
    IC_WIFI,
    IC_BATTERY_LOW,
    IC_BATTERY_MEDIUM,
    IC_BATTERY_HIGH,
    IC_BATTERY_FULL,
    IC_BATTERY_LOW_CHARGING,
    IC_BATTERY_MEDIUM_CHARGING,
    IC_BATTERY_HIGH_CHARGING,
    IC_BATTERY_FULL_CHARGING,
    IC_WDR,
    IC_AUDIO_RECORD,
    IC_AUDIO_MUTE,
    IC_EV_NEGATIVE_2P0,
    IC_EV_NEGATIVE_1P5,
    IC_EV_NEGATIVE_1P0,
    IC_EV_NEGATIVE_0P5,
    IC_EV_ZERO,
    IC_EV_POSITIVE_0P5,
    IC_EV_POSITIVE_1P0,
    IC_EV_POSITIVE_1P5,
    IC_EV_POSITIVE_2P0,
    IC_QUALITY_FINE,
    IC_QUALITY_NORMAL,
    IC_QUALITY_ECONOMY,
    IC_WB_AUTO,
    IC_WB_DAYLIGHT,
    IC_WB_CLOUDY,
    IC_WB_TUNGSTEN,
    IC_WB_FLUORESCENT,
    IC_ISO_AUTO,
    IC_ISO_100,
    IC_ISO_200,
    IC_ISO_400,
    IC_ISO_800,
    IC_IR_DAYTIME,
    IC_IR_NIGHT,
    IC_HEADLAMP,
    IC_ANTI_SHAKING,
    IC_SETTINGS,
    IC_EXIT,
    IC_BACK,
    IC_BRIGHTNESS,
    IC_DATE_TIME,
    IC_DELETE,
    IC_DELETED,
    IC_DELETING,
    IC_MORE,
    IC_CARD,
    IC_CAMERA_BUSY,
    IC_DEFAULT_THUMB,
    IC_SHUTDOWN,
    IC_REVERSE_IMAGE,
    IC_MAX, 
} ICON_ENUM;

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
} LANGUAGE_TYPE;

//icon res, file path
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
void SpvSetWifiStatus(PWifiOpt pWifi); 
int SpvGetWifiStatus(); 
int IsSdcardMounted();
int GetSdcardStatus();
int GetPhotoCount();

HWND ShowSDErrorToast(HWND hWnd);

HWND GetMainWindow();

void ScanSdcardMediaFiles(BOOL sdInserted);
void GetMediaFileCount(int *videoCount, int *photoCount);

void SyncMediaList(FileList *list);

void ResetCamera();
BOOL SetCameraConfig(BOOL directly);
#endif //_SPV_COMMON_H
