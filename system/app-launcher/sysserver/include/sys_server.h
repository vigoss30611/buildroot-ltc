#ifndef __SYS_SERVER_H__
#define __SYS_SERVER_H__
#include "spv_config.h"

typedef enum {
    P2P_MEDIA_H264 = 1,
    P2P_MEDIA_H265,
    P2P_MEDIA_MJPEG,
} P2P_MEDIA_TYPE;

typedef enum
{
    MODE_VIDEO,
    MODE_PHOTO,
#ifdef GUI_PLAYBACK_ENABLE
    MODE_PLAYBACK,
#endif
#if USE_SETTINGS_MODE
    MODE_SETUP,
#endif
    MODE_COUNT,
#ifndef GUI_PLAYBACK_ENABLE
    MODE_PLAYBACK,
#endif
#if !USE_SETTINGS_MODE
    MODE_SETUP,
#endif
    MODE_CARDV,
} SPV_MODE;

typedef enum {
	WIFI_OFF,
	WIFI_CLOSING,
	WIFI_OPEN_AP,
	WIFI_OPEN_STA,
	WIFI_AP_ON,
	WIFI_STA_ON,
} WIFI_STATUS;

typedef enum {
    WIFI_AP_DISCONNECTED,
    WIFI_AP_CONNECTED,
    WIFI_STA_DISCONNECTED,
    WIFI_STA_CONNECTED,
} WIFI_CONNECTED_STATUS;

typedef enum {
    SD_STATUS_UMOUNT,
    SD_STATUS_MOUNTED,
    SD_STATUS_ERROR,
    SD_STATUS_NEED_FORMAT,
    SD_PREPARE,
} SD_STATUS;


typedef enum {
    SHUTDOWN_UNKNOWN = -1,
    SHUTDOWN_KEY,
    SHUTDOWN_DELAY,
    SHUTDOWN_AUTO,
	SHUTDOWN_POWER_LOW,
    SHUTDOWN_ACC,
    SHUTDOWN_ERROR,
} SHUTDOWN_REASON;

typedef enum {
    CAMERA_ERROR_INTERNAL = 10,
    CAMERA_ERROR_DISKIO,
    CAMERA_ERROR_SD_FULL,
} CAMERA_CALLBACK_ERROR;

typedef enum {
    BATTERY_CHARGING,
    BATTERY_DISCHARGING,
    BATTERY_FULL,
} BATTERY_STATUS;

typedef struct GPS_Data {
    double latitude;
    double longitude;
    double speed;
} GPS_Data;

typedef struct
{
    int online;
    int initialized;
	SPV_MODE mode;//camera mode
    int isRecording;//recording?
    int isPicturing;//picturing?
    struct timeval beginTime;//video/photo begin time
    int timeLapse;//video/photo lapse time
    unsigned int battery;//battery volume
    int charging;//charging or not
    WIFI_STATUS wifiStatus;
    long availableSpace; //in kBytes
    int isDaytime;//is in day time
    int sdStatus;
    int isKeyTone;//key press tone enabled
    unsigned int videoCount;//video count in sdcard
    unsigned int photoCount;//phoot count in sdcard
    int isPhotoEffect;//is in photo effect
    int isLocked;//collision occured
    int rearCamInserted;
    int reverseImage;
    GPS_Data gpsData;//The newest gps data
    int calibration;
} SPVSTATE;

  
typedef enum {          //wParam     lParam
    MSG_INVALID = -1,
    MSG_USER_DESTROY = 0,
    MSG_USER_KEYDOWN = 0x0010,       //keyvalue
    MSG_USER_KEYUP = 0x0012,         //keyvalue
    MSG_USER_BEGIN = 0x0800,

    MSG_COLLISION,
    MSG_CHARGE_STATUS,        //status     volume
    MSG_BATTERY_STATUS,        //status     volume
    MSG_SDCARD_MOUNT,         //status
    MSG_WIFI_STATUS,    //status      value
    MSG_WIFI_CONNECTION, //connect status value
    MSG_IR_LED,         //isDayTime
    MSG_UMS,
    MSG_GPS,            //GPS_Data
    MSG_HDMI_HOTPLUG,
    MSG_REAR_CAMERA,
    MSG_REVERSE_IMAGE,
    MSG_ACC_CONNECT,
    MSG_SHUTDOWN,

    MSG_GET_UI_STATUS,
    MSG_CONFIG_CHANGED, //key        value
    MSG_SHUTTER_ACTION, //ok key pressed in main window
    MSG_TAKE_SNAPSHOT,  //take photo while recording
    MSG_FORMAT_SDCARD,
	MSG_FORMAT_ERROR,
    MSG_RESTORE_SETTINGS, //settings type

    MSG_MEDIA_UPDATED,
    MSG_CAMERA_CALLBACK,
    MSG_PAIRING_SUCCEED,
    MSG_USER_KEYLONGPRESS,

    MSG_CALIBRATION,
	MSG_SWITCH_TO_UMS,
	MSG_SWITCH_TO_UVC,
    MSG_ONLINE,
} GUI_MSG;

typedef enum {
    TYPE_FROM_GUI,
    TYPE_FROM_P2P_DANA,
	TYPE_FROM_UVC_HOST = TYPE_FROM_P2P_DANA,

    TYPE_FROM_DEVICE,
    TYPE_FROM_COUNT = TYPE_FROM_DEVICE,
} FROM_TYPE;

typedef struct {
    //client type
    int type;
    /**
     * App/Gui need to implment this function, to recevie the system event
     **/
    int (*SystemMsgListener)(int message, int wParam, long lParam);
} SpvClient;


/**
 * Register the client listener to sysserver to recevie message
 **/
int RegisterSpvClient(int type, int (*SystemMsgListener)(int message, int wParam, long lParam));

/**
 * Send msg to sysserver, implemented by sysserver
 **/
int SendMsgToServer(int message, int wParam, long lParam, int from);

int GetConfigValue(const char *key, char *value);

SPVSTATE* GetSpvState();

#endif
