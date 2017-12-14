#ifndef __SPV_ITEM_H__
#define __SPV_ITEM_H__

#define GETKEY(id) g_item_key[id]
#define GETVALUE(id) language_en[id - 1]

#define MAX_VALUE_COUNT 12

#define ITEM_TYPE_NORMAL 0x00000001L
#define ITEM_TYPE_HEADER 0x000000002L
#define ITEM_TYPE_MORE 0x00000003L
#define ITEM_TYPE_SWITCH 0x00000004L

#define ITEM_TYPE_MASK 0x00000007L

#define ITEM_STATUS_SECONDARY 0x00000010L
#define ITEM_STATUS_DISABLE   0x00000020L
#define ITEM_STATUS_INVISIBLE   0x00000040L
#define ITEM_STATUS_OFF        0x00000080L

enum ITEM_ID {
    //video
    ID_VIDEO_HEADER = 1,
    ID_VIDEO_RESOLUTION,
    ID_VIDEO_LOOP_RECORDING,
#ifdef GUI_CAMERA_REAR_ENABLE
    ID_VIDEO_REAR_CAMERA,
    ID_VIDEO_PIP,
#endif
    ID_VIDEO_WDR,
    ID_VIDEO_EV,
#ifndef GUI_SPORTDV_ENABLE
    ID_VIDEO_RECORD_AUDIO,
#endif
    ID_VIDEO_DATE_STAMP,
#ifndef GUI_SPORTDV_ENABLE
    ID_VIDEO_GSENSOR,
#ifdef GUI_ADAS_ENABLE
    ID_VIDEO_ADAS,
    ID_VIDEO_ADAS_CALIBRATE,
#endif
    ID_VIDEO_PLATE_STAMP,
#endif
    ID_VIDEO_RESET,

    //photo
    ID_PHOTO_HEADER,
    ID_PHOTO_CAPTURE_MODE,
    ID_PHOTO_RESOLUTION,
    ID_PHOTO_SEQUENCE,
    ID_PHOTO_QUALITY,
    ID_PHOTO_SHARPNESS,
    ID_PHOTO_WHITE_BALANCE,
    ID_PHOTO_COLOR,
    ID_PHOTO_ISO_LIMIT,
    ID_PHOTO_EV,
    ID_PHOTO_ANTI_SHAKING,
    ID_PHOTO_DATE_STAMP,
    ID_PHOTO_RESET,

    //setup
    ID_SETUP_HEADER,
    ID_SETUP_VIDEO,
    ID_SETUP_PHOTO,
#ifdef GUI_WIFI_ENABLE
    ID_SETUP_WIFI,
#endif
    ID_SETUP_DATE_TIME,
    //ID_SETUP_AUTO_POWER_OFF,
    ID_SETUP_BEEP_SOUND,
    ID_SETUP_KEY_TONE,
    ID_SETUP_LANGUAGE,
    ID_SETUP_DELAYED_SHUTDOWN,
    ID_SETUP_DISPLAY,
#ifndef GUI_SPORTDV_ENABLE
    //ID_SETUP_TV_MODE,
    ID_SETUP_PARKING_GUARD,
    //ID_SETUP_FREQUENCY,
    ID_SETUP_IR_LED,
    ID_SETUP_IMAGE_ROTATION,
#endif
    ID_SETUP_FORMAT,
    ID_SETUP_CAMERA_RESET,
#ifndef GUI_SPORTDV_ENABLE
    ID_SETUP_PLATE_NUMBER,
#endif
    ID_SETUP_VERSION,

    //display
    ID_DISPLAY_HEADER,
    ID_DISPLAY_SLEEP,
    ID_DISPLAY_BRIGHTNESS,

    //data & time
    ID_DATE_TIME_HEADER,
    ID_DATE,
    ID_TIME,
#ifdef GUI_GPS_ENABLE
    ID_TIME_AUTO_SYNC,
#endif

    //external status key
    ID_VIDEO_FRONTBIG,
    ID_VIDEO_ZOOM, 
    ID_PHOTO_ZOOM,
    ID_WIFI_AP_SSID,
    ID_WIFI_AP_PSWD,
    ID_WIFI_AP_KEYMGMT,
    ID_WIFI_STA_SSID,
    ID_WIFI_STA_PSWD,
    ID_WIFI_STA_KEYMGMT,
    
#ifdef GUI_SPORTDV_ENABLE
    ID_VIDEO_RECORD_AUDIO,
    ID_VIDEO_GSENSOR,
    ID_VIDEO_ADAS,
    ID_VIDEO_ADAS_CALIBRATE,
    ID_VIDEO_PLATE_STAMP,
    ID_SETUP_PARKING_GUARD,
    ID_SETUP_IR_LED,
    ID_SETUP_IMAGE_ROTATION,

    ID_SETUP_PLATE_NUMBER,
#endif
};


typedef struct _DIALOGITEM{
    int itemId;
    int style;
    int caption;
    int currentValue;
    int valuelist[MAX_VALUE_COUNT];
    int vc;
} DIALOGITEM;

typedef struct {
    int type;
    DIALOGITEM *pItems;
    int itemCount;
} DIALOGPARAM;

extern const int g_video_1080_fps_value[MAX_VALUE_COUNT];
extern const int g_video_720_fps_value[MAX_VALUE_COUNT];
extern const int g_video_360_fps_value[MAX_VALUE_COUNT];

extern DIALOGITEM g_video_items[];
extern DIALOGITEM g_photo_items[];
extern DIALOGITEM g_setup_items[];
extern DIALOGITEM g_display_items[];
extern DIALOGITEM g_date_time_items[];
//const extern DIALOGITEM *g_all_item[];

extern const char *g_mode_name[];
//extern const char *g_current_key;
extern const char *g_item_key[];

void UpdateSetupDialogItemValue(const char *key, const char *value);

int GetSettingItemsCount(int type);

#endif //__SPV_ITEM_H__
