#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<math.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

#include "spv_common.h"
#include "spv_static.h"
#include "spv_setup_dialog.h"
#include "spv_utils.h"
#include "config_manager.h"
#include "spv_item.h"
#include "spv_language_res.h"
#include "camera_spv.h"
#include "spv_folder_dialog.h"
#include "spv_app.h"
#include "spv_ui_status.h"
#include "file_dir.h"
#include "spv_debug.h"
#include "gps.h"
#include "spv_adas.h"
#include "spv_wifi.h"
#include <qsdk/event.h>

#ifndef UBUNTU
#include "qsdk/items.h"
#endif

#define FONTFILE_PATH   "/root/libra/res/"

#define CLEANUP_SECONDARYDC_TIME 10

#define TIMER_CLEANUP_SECONDARYDC 123

#define FLAG_POWERED_BY_GSENSOR 8424
#define FLAG_POWERED_BY_OTHER 8425

#define TIMER_PHOTO_EFFECT (TIMER_CLEANUP_SECONDARYDC + 1)

#define TIMER_BATTERY_FLICK (TIMER_PHOTO_EFFECT + 1)

#define TIMER_AUTO_SHUTDOWN (TIMER_BATTERY_FLICK + 1)

#define TIMER_LONGPRESS_DOWN (TIMER_AUTO_SHUTDOWN + 1)
#define TIMER_LONGPRESS_UP (TIMER_LONGPRESS_DOWN + 1)
#define TIMER_TIRED_ALARM (TIMER_LONGPRESS_UP + 1)

static HWND g_main_window;

char **g_language_res;

BITMAP *g_icon;

static PLOGFONT g_logfont_info;
static PLOGFONT g_logfont_mode;

static camera_spv* g_camera;
static wifi_p g_wifi;
//char g_wifi_sta[3][128] = {0};

PUIStatus g_status = NULL;

static BOOL g_recompute_locations = FALSE;
static BOOL g_force_cleanup_secondarydc = FALSE;

static BOOL g_sync_gps_time;

int g_fucking_volume = 0;

int CAMERA_REAR_CONNECTED = 1;

int REVERSE_IMAGE = 0;

extern RECT g_rcDesktop;

int DEVICE_WIDTH = 360;
int DEVICE_HEIGHT = 240;

int LARGE_SCREEN = 0;

int SCALE_FACTOR = 1;
int VIEWPORT_WIDTH = 360;
int VIEWPORT_HEIGHT = 240;

// main window
int MW_BAR_HEIGHT = 40;
int MW_ICON_WIDTH = 58;
int MW_ICON_HEIGHT = 36;
int MW_SMALL_ICON_WIDTH;
int MW_ICON_PADDING;
int MW_ITEM_PADDING = 4;

//setup window 
int SETUP_ITEM_HEIGHT;

//folder window
int FOLDER_DIALOG_TITLE_HEIGHT;

int FOLDER_WIDTH;
int FOLDER_HEIGHT;
int FOLDER_INFO_HEIGHT;

//playback window
int PB_BAR_HEIGHT;
int PB_PROGRESS_HEIGHT;

int PB_ICON_WIDTH;
int PB_ICON_HEIGHT;
int PB_ICON_PADDING;

int PB_FOCUS_WIDTH;
int PB_FOCUS_HEIGHT;

int SAVE_SCREEN = 0;

//window define data
typedef struct
{
    CTRLDATA ctrlData;
    HWND winHandle;
} MAIN_WIN_ITEM;

typedef struct
{
    SPV_MODE mode;
    BOOL isRecording;
    BOOL isPicturing;
    struct timeval beginTime;
    int timeLapse;
    unsigned int battery;
    BOOL charging;
    int wifiStatus;
    char locating[VALUE_SIZE];
    long availableSpace; //in kBytes
    BOOL isDaytime;
    int sdStatus;
    BOOL isKeyTone;
    unsigned int videoCount;
    unsigned int photoCount;
    BOOL isPhotoEffect;
    BOOL isLocked;
    BOOL rearCamInserted;
    BOOL reverseImage;
#if 0
    ADAS_LDWS_RESULT ldwsResult;
    struct timeval ldTime;
    ADAS_FCWS_RESULT fcwsResult;
    struct timeval fcTime;
#else
    ADAS_PROC_RESULT adasResult;
#endif
    GPS_Data gpsData;
} SPVSTATE;

static SPVSTATE g_spv_camera_state = {
    MODE_VIDEO,
    FALSE,
    FALSE,
    {0, 0},
    0,
    100,
    FALSE,
    0,
    "Off",
    0,
    TRUE,
    0,
    TRUE,
    0,
    0,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
};

typedef struct {
    MW_STYLE style;
    RECT rect;
    BOOL visibility;
    char text[VALUE_SIZE];
    int iconId;
} MWITEM;

typedef MWITEM* PMWITEM;

MWITEM g_items[MWI_MAX + 1];

static BOOL IsMainWindowActive();
static void GetLapseString(char *lstring);
static void UpdatePhotoCount(HWND hWnd, unsigned int photoCount);
static int CameraInfoCallback(int flag);
static void ShutdownProcess();
static void SpvSetCollision(HWND hWnd, camera_spv *camera);

static int DealClockTimer(HWND hWnd);

int ImmediatlyMediaFilesCount()
{
    return g_spv_camera_state.videoCount + g_spv_camera_state.photoCount;
}

int GetBatteryIcon()
{
    int batteryIcon = IC_BATTERY_FULL;
    int bt = g_spv_camera_state.battery - 1;
    if(bt > 99) bt = 99;
    if(bt < 0) bt = 0;
    if(g_spv_camera_state.charging) {
        batteryIcon = bt / 33 + IC_BATTERY_LOW_CHARGING;
    } else {
        batteryIcon = bt / 33 + IC_BATTERY_LOW;
    }

    return batteryIcon;
}

static void UpdateBatteryIcon()
{
    //battery
    int batteryIcon = GetBatteryIcon();
    g_items[MWV_BATTERY].iconId = batteryIcon;
    g_items[MWP_BATTERY].iconId = batteryIcon;
}

static void UpdateRearCameraStatus(int devInserted)
{
#ifdef GUI_CAMERA_REAR_ENABLE
    g_spv_camera_state.rearCamInserted = devInserted;
    CAMERA_REAR_CONNECTED = g_spv_camera_state.rearCamInserted; 
#ifdef GUI_WIFI_ENABLE
    SetConfigValue(g_status_keys[STATUS_REAR], 
            (g_spv_camera_state.rearCamInserted) ? 
            GETVALUE(STRING_ON) : GETVALUE(STRING_OFF), FROM_DEVICE);
#endif
#endif
}


static void GetDateTimeString(char *dtString)
{
    if(dtString == NULL)
        return;

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int year, month, day, hour, minute;
    year = timeinfo->tm_year + 1900;
    month = timeinfo->tm_mon + 1;
    day = timeinfo->tm_mday;
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;

    sprintf(dtString, "%d.%02d.%02d %02d:%02d", year, month, day, hour, minute);
}

static void InvalidateItemRect(HWND hWnd, MWI_TYPE itemId, BOOL bErase)
{
    RECT rect = g_items[itemId].rect;

    rect.left *= SCALE_FACTOR;
    rect.top *= SCALE_FACTOR;
    rect.right *= SCALE_FACTOR;
    rect.bottom *= SCALE_FACTOR;

    g_force_cleanup_secondarydc = TRUE;
    //if(g_items[itemId].style == MW_STYLE_TEXT) {
        //we need to force clean the secondarydc, otherwise the last frame will be remained on the screen
    //}

    InvalidateRect(hWnd, &rect, bErase);
}

static void UpdateModeItemsContent(SPV_MODE currentMode)
{
    if(currentMode > MODE_PHOTO)
        return;

    INFO("UpdateModeItemsContent\n");
    char value[VALUE_SIZE] = {0};
    if(currentMode == MODE_VIDEO) {
        g_items[MWV_MODE].style = MW_STYLE_ICON;
        g_items[MWV_MODE].iconId = g_spv_camera_state.isRecording ? IC_VIDEO_RECORDING: IC_VIDEO;

        g_items[MWV_LAPSE].style = MW_STYLE_TEXT;
        memset(g_items[MWV_LAPSE].text, 0, sizeof(g_items[MWV_LAPSE].text));
        GetLapseString(g_items[MWV_LAPSE].text);
        
        g_items[MWV_BATTERY].style = MW_STYLE_ICON;
        UpdateBatteryIcon();

        g_items[MWV_DATETIME].style = MW_STYLE_TEXT;
        GetDateTimeString(g_items[MWV_DATETIME].text);
        
        g_items[MWV_RESOLUTION].style = MW_STYLE_TEXT;
        GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), g_items[MWV_RESOLUTION].text);

        g_items[MWV_INTERVAL].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
        if(!strcasecmp(GETVALUE(STRING_1_MINUTE), value)) {
            g_items[MWV_INTERVAL].iconId = IC_VIDEO_1MIN;
        } else if(!strcasecmp(GETVALUE(STRING_3_MINUTES), value)) {
            g_items[MWV_INTERVAL].iconId = IC_VIDEO_3MIN;
        } else if(!strcasecmp(GETVALUE(STRING_5_MINUTES), value)) {
            g_items[MWV_INTERVAL].iconId = IC_VIDEO_5MIN;
        } else if(!strcasecmp(GETVALUE(STRING_10_MINUTES), value)) {
            g_items[MWV_INTERVAL].iconId = IC_VIDEO_10MIN;
        }

        g_items[MWV_ZOOM].style = MW_STYLE_TEXT;
        GetConfigValue(GETKEY(ID_VIDEO_ZOOM), g_items[MWV_ZOOM].text);

        g_items[MWV_LOCKED].style = MW_STYLE_ICON;
        g_items[MWV_LOCKED].iconId = IC_LOCKED;

        g_items[MWV_WDR].style = MW_STYLE_ICON;
        g_items[MWV_WDR].iconId = IC_WDR;

        //mwv_ev
        g_items[MWV_EV].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_EV), value);
        g_items[MWV_EV].iconId = (int)(atof(value) * 10 + 20) / 5 + IC_EV_NEGATIVE_2P0;

        g_items[MWV_AUDIO].style = MW_STYLE_ICON;
        g_items[MWV_AUDIO].iconId = IC_AUDIO_MUTE;

    } else if(g_spv_camera_state.mode == MODE_PHOTO){
        g_items[MWP_MODE].style = MW_STYLE_ICON;
        g_items[MWP_MODE].iconId = g_spv_camera_state.isPicturing ? IC_PHOTO_RECORDING: IC_PHOTO;

        //mwp_count
        g_items[MWP_COUNT].style = MW_STYLE_TEXT;
        GetLapseString(g_items[MWP_COUNT].text);

        //mwp_timer
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), value);
        g_items[MWP_TIMER].style = g_spv_camera_state.isPicturing ? MW_STYLE_TEXT : MW_STYLE_ICON;
        if(!strcasecmp(GETVALUE(STRING_2S_TIMER), value)) {
            g_items[MWP_TIMER].iconId = IC_PHOTO_TIMER_2S;
        } else if(!strcasecmp(GETVALUE(STRING_5S_TIMER), value)) {
            g_items[MWP_TIMER].iconId = IC_PHOTO_TIMER_5S;
        } else if(!strcasecmp(GETVALUE(STRING_10S_TIMER), value)) {
            g_items[MWP_TIMER].iconId = IC_PHOTO_TIMER_10S;
        }
        int timeRemain = atoi(value) - g_spv_camera_state.timeLapse;
        timeRemain = timeRemain < 0 ? 0 : timeRemain;
        sprintf(g_items[MWP_TIMER].text, "%d", timeRemain);
        
        //mwp_anti_shaking
        g_items[MWP_ANTI_SHAKING].style = MW_STYLE_ICON;
        g_items[MWP_ANTI_SHAKING].iconId = IC_ANTI_SHAKING;

        //mwp_resolution
        g_items[MWP_RESOLUTION].style = MW_STYLE_TEXT;
        GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), g_items[MWP_RESOLUTION].text);
       
        //mwp_quality 
        g_items[MWP_QUALITY].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_QUALITY), value);
        if(!strcasecmp(GETVALUE(STRING_FINE), value)) {
            g_items[MWP_QUALITY].iconId = IC_QUALITY_FINE;
        } else if(!strcasecmp(GETVALUE(STRING_NORMAL), value)) {
            g_items[MWP_QUALITY].iconId = IC_QUALITY_NORMAL;
        } else if(!strcasecmp(GETVALUE(STRING_ECONOMY), value)) {
            g_items[MWP_QUALITY].iconId = IC_QUALITY_ECONOMY;
        }

        //mwp_sequence
        g_items[MWP_SEQUENCE].style = MW_STYLE_ICON;
        g_items[MWP_SEQUENCE].iconId = IC_PHOTO_SEQUENCE;

        //mwp_zoom
        g_items[MWP_ZOOM].style = MW_STYLE_TEXT;
        GetConfigValue(GETKEY(ID_PHOTO_ZOOM), g_items[MWP_ZOOM].text);

        //mwp_iso
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_ISO_LIMIT), value);
        int iso = atoi(value) / 100;
        g_items[MWP_ISO].style = MW_STYLE_ICON;
        if(iso) {
            g_items[MWP_ISO].iconId = IC_ISO_100 + log2(iso);
        } else {
            g_items[MWP_ISO].iconId = IC_ISO_AUTO;
        }

        //mwp_ev
        g_items[MWP_EV].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_EV), value);
        g_items[MWP_EV].iconId = (int)(atof(value) * 10 + 20) / 5 + IC_EV_NEGATIVE_2P0;

        //mwp_wb
        g_items[MWP_WB].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_WHITE_BALANCE), value);
        if(!strcasecmp(GETVALUE(STRING_AUTO), value)) {
            g_items[MWP_WB].iconId = IC_WB_AUTO;
        } else if(!strcasecmp(GETVALUE(STRING_DAYLIGHT), value)) {
            g_items[MWP_WB].iconId = IC_WB_DAYLIGHT;
        } else if(!strcasecmp(GETVALUE(STRING_CLOUDY), value)) {
            g_items[MWP_WB].iconId = IC_WB_CLOUDY;
        } else if(!strcasecmp(GETVALUE(STRING_TUNGSTEN), value)) {
            g_items[MWP_WB].iconId = IC_WB_TUNGSTEN;
        } else if(!strcasecmp(GETVALUE(STRING_FLUORESCENT), value)) {
            g_items[MWP_WB].iconId = IC_WB_FLUORESCENT;
        }

    }

    //mwv_battery, mwp_battery
    g_items[MWP_BATTERY].style = g_items[MWV_BATTERY].style = MW_STYLE_ICON;
    UpdateBatteryIcon();

    //mwv_light, mwp_light
    g_items[MWP_LIGHT].style = g_items[MWV_LIGHT].style = MW_STYLE_ICON;
    memset(value, 0, sizeof(value));
    GetConfigValue(GETKEY(ID_SETUP_IR_LED), value);
    if(!strcasecmp(GETVALUE(STRING_AUTO), value)) {
        g_items[MWP_LIGHT].iconId = g_items[MWV_LIGHT].iconId = g_spv_camera_state.isDaytime ? IC_IR_DAYTIME : IC_IR_NIGHT;
    } else if(!strcasecmp(GETVALUE(STRING_ON), value)) {
        g_items[MWP_LIGHT].iconId = g_items[MWV_LIGHT].iconId = IC_IR_NIGHT;
    } else if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
        g_items[MWP_LIGHT].iconId = g_items[MWV_LIGHT].iconId = IC_IR_DAYTIME;
    }

    g_items[MWP_LAMP].style = g_items[MWV_LAMP].style = MW_STYLE_ICON;
    g_items[MWP_LAMP].iconId = g_items[MWV_LAMP].iconId = IC_HEADLAMP;

    g_items[MWP_WIFI].style = g_items[MWV_WIFI].style = MW_STYLE_ICON;
    g_items[MWP_WIFI].iconId = g_items[MWV_WIFI].iconId = IC_WIFI;
}

int GetTextWidth(HDC hdc, const char *string)
{
    if(string == NULL || strlen(string) <= 0)
        return 0;
    SIZE size = {0, 0};
    SelectFont(hdc, g_logfont_info);
    GetTextExtent(hdc, string, -1, &size);
    return size.cx / SCALE_FACTOR;
}

int GetItemWidth(HDC hdc, MWI_TYPE type)
{
    char value[VALUE_SIZE] = {0};
    int width = 0;
    BOOL widthSet = FALSE;
    switch (type) {
        case MWV_MODE:
        case MWV_BATTERY:
        case MWP_MODE:
        case MWP_BATTERY:
            widthSet = TRUE;
            width = MW_ICON_WIDTH;
            break;
        case MWV_INTERVAL:
            GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
            widthSet = TRUE;
            width = atoi(value) ? MW_ICON_WIDTH : 0;
            break;
        case MWP_SEQUENCE:
            GetConfigValue(GETKEY(ID_PHOTO_SEQUENCE), value);
            widthSet = TRUE;
            width = strcasecmp(GETVALUE(STRING_OFF), value) ? MW_ICON_WIDTH : 0;
            break;
        case MWV_LOCKED:
            widthSet = TRUE;
            width = g_spv_camera_state.isLocked ? (MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING) : 0;
            break;
        case MWV_WDR:
            GetConfigValue(GETKEY(ID_VIDEO_WDR), value);
            widthSet = TRUE;
            width = strcasecmp(GETVALUE(STRING_OFF), value) ? MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING : 0;
            break;
        case MWV_AUDIO:
            GetConfigValue(GETKEY(ID_VIDEO_RECORD_AUDIO), value);
            widthSet = TRUE;
            width = strcasecmp(GETVALUE(STRING_OFF), value) ? 0 : MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING;
            break;
        case MWP_TIMER:
            GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), value);
            widthSet = TRUE;
            width = atoi(value) ? MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING : 0;
            break;
        case MWP_ANTI_SHAKING:
            GetConfigValue(GETKEY(ID_PHOTO_ANTI_SHAKING), value);
            widthSet = TRUE;
            width = strcasecmp(GETVALUE(STRING_OFF), value) ? MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING : 0;
            break;
        case MWV_EV:
        case MWV_LIGHT:
        case MWP_QUALITY:
        case MWP_ISO:
        case MWP_EV:
        case MWP_WB:
        case MWP_LIGHT:
            widthSet = TRUE;
            width = MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING;
            break;
        case MWV_LAMP:
        case MWP_LAMP:
            widthSet = TRUE;
            width = g_spv_camera_state.isDaytime ? 0 : (MW_SMALL_ICON_WIDTH + MW_ITEM_PADDING);
            break;
        case MWV_WIFI:
        case MWP_WIFI:
            widthSet = TRUE;
            width = (g_spv_camera_state.wifiStatus == WIFI_AP_ON || g_spv_camera_state.wifiStatus == WIFI_STA_ON) ? MW_SMALL_ICON_WIDTH : 0;
            break;

        case MWV_LAPSE:
            GetLapseString(value);
            break;
        case MWV_DATETIME:
            strcpy(value, "0000.00.00 00:00");
            break;
        case MWV_RESOLUTION:
            GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
            break;
        case MWV_ZOOM:
            GetConfigValue(GETKEY(ID_VIDEO_ZOOM), value);
            /*if(atof(value) <= 1.0f) {
                widthSet = TRUE;
                width = 0;
            }*/
            break;
        case MWP_COUNT:
            GetLapseString(value);
            break;
        case MWP_RESOLUTION:
            GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), value);
            break;
        case MWP_ZOOM:
            GetConfigValue(GETKEY(ID_PHOTO_ZOOM), value);
            /*if(atof(value) <= 1.0f) {
                widthSet = TRUE;
                width = 0;
            }*/
            break;
        default:
            widthSet = TRUE;
            width = 0;
            break;
    }

    if(!widthSet) {
        width = GetTextWidth(hdc, value) + MW_ITEM_PADDING;
    }

    return width;
}

static void ComputeItemsLocation(HDC hdc, BOOL isVideo)
{
    INFO("ComputeItemsLocation\n");
    int startx;
    BOOL alignLeft = TRUE;
    BOOL alignTop = TRUE;

    int i = 0, begin, end;
    if(isVideo) {
        begin = MWI_VIDEO_BEGIN;
        end = MWI_VIDEO_END;
    } else {
        begin = MWI_PHOTO_BEGIN;
        end = MWI_PHOTO_END;
    }

    for(i = begin; i < end + 1; i ++)
    {
        switch(i) {
            case MWV_MODE://left top corner
            case MWP_MODE:
                startx = 0;
                alignLeft = TRUE;
                alignTop = TRUE;
                break;
            case MWV_BATTERY://right top corner
            case MWP_BATTERY:
                startx = VIEWPORT_WIDTH;
                alignLeft = FALSE;
                alignTop = TRUE;
                break;
            case MWV_RESOLUTION://left top corner
            case MWP_RESOLUTION:
                startx = 0;
                alignLeft = TRUE;
                alignTop = FALSE;
                break;
            case MWV_WDR://right top corner
            case MWP_ISO:
                startx = VIEWPORT_WIDTH;
                alignLeft = FALSE;
                alignTop = FALSE;
                break;
        }

        //mwv_mode icon
        int width = GetItemWidth(hdc, i);
        if(alignLeft) {
            g_items[i].rect.left = startx;
            g_items[i].rect.right = startx + width;
            startx += width;
        } else {
            g_items[i].rect.left = startx - width;
            g_items[i].rect.right = startx;
            startx -= width;
        }

        if(alignTop) {
            g_items[i].rect.top = 0;
            g_items[i].rect.bottom = MW_BAR_HEIGHT;
        } else {
            g_items[i].rect.top = VIEWPORT_HEIGHT - MW_BAR_HEIGHT; 
            g_items[i].rect.bottom = VIEWPORT_HEIGHT;
        }
        g_items[i].visibility = width > 0 ? TRUE : FALSE;
    }
}

static void UpdateMainWindow(HWND hWnd)
{
    UpdateModeItemsContent(g_spv_camera_state.mode);
    g_recompute_locations = TRUE;
    g_force_cleanup_secondarydc = TRUE;
    UpdateWindow(hWnd, TRUE);
}

static void ChangeToMode(HWND hWnd, SPV_MODE mode)
{
    int i = 0;
    int winCount = 0;
    //SPV_MODE oldMode = g_spv_camera_state.mode;

    if(g_spv_camera_state.mode == (mode % MODE_COUNT))
        return;
    
    g_spv_camera_state.mode = mode % MODE_COUNT;

    SetIrLed();
    SetConfigValue(KEY_CURRENT_MODE, g_mode_name[g_spv_camera_state.mode], FROM_USER);

    if(g_spv_camera_state.mode == MODE_PHOTO) {
        GetMediaFileCount(&g_spv_camera_state.videoCount, &g_spv_camera_state.photoCount);
    }

    //UpdateModeItemsContent(g_spv_camera_state.mode);
    //g_recompute_locations = TRUE;

    //camera
    switch(g_spv_camera_state.mode) {
        case MODE_VIDEO:
        case MODE_PHOTO:
            SetCameraConfig(TRUE);//show preview
            break;
        case MODE_PLAYBACK:
            ShowFolderDialog(hWnd, 0);
            break;
        case MODE_SETUP:
            ShowSetupDialogByMode(hWnd, MODE_SETUP);
            break;
    }

	UpdateMainWindow(hWnd);
    //InvalidateRect(hWnd, 0, TRUE);
}

static int UpdateBatteryStatus()
{//(0-7)(low, medium, high, full, low-charging, medium-charging, high-charging, full-charing)
#ifdef GUI_WIFI_ENABLE
    int l;
    char level[16] = {0};
    if(g_spv_camera_state.charging)
    {
        l = g_spv_camera_state.battery / 26 + 4;
    } else {
        l = g_spv_camera_state.battery / 26;
    }
    sprintf(level, "%d", l);
    SetConfigValue(g_status_keys[STATUS_BATTERY], level, FROM_DEVICE);
#endif
    return 0;
}

void LoadGlobalIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;
    if(g_icon[iconId].bmWidth == 0)
    {
        LoadBitmap(HDC_SCREEN, &g_icon[iconId], SPV_ICON[iconId]);
        switch(iconId) {
            case IC_VIDEO:
            case IC_PHOTO:
            case IC_BATTERY_LOW:
            case IC_BATTERY_LOW_CHARGING:
            case IC_BATTERY_MEDIUM:
            case IC_BATTERY_MEDIUM_CHARGING:
            case IC_BATTERY_HIGH:
            case IC_BATTERY_HIGH_CHARGING:
            case IC_BATTERY_FULL:
            case IC_BATTERY_FULL_CHARGING:
                g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
                g_icon[iconId].bmColorKey = RGB2Pixel(HDC_SCREEN, 0xB8, 0x40, 0x40);
                break;
            default:
                g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
                g_icon[iconId].bmColorKey = PIXEL_black;
                break;
        }
    }
}

void LoadMainWindowIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;

    if(g_icon[iconId].bmWidth == 0)
    {
        LoadBitmap(HDC_SCREEN, &g_icon[iconId], SPV_ICON[iconId]);
        g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
        g_icon[iconId].bmColorKey = RGB2Pixel(HDC_SCREEN, 0xB8, 0x40, 0x40);//SPV_COLOR_TRANSPARENT;
    }
}

void FreeGlobalIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;

    if(g_icon[iconId].bmWidth != 0)
    {
        UnloadBitmap(&g_icon[iconId]);
        memset(&g_icon[iconId], 0, sizeof(BITMAP));
    }
}

void DrawAdasDetectionResult(HDC hdc)
{
#ifdef GUI_ADAS_ENABLE
    if(g_spv_camera_state.mode != MODE_VIDEO || !g_spv_camera_state.isRecording)
        return;

    if(GetAdasDetectionState() != ADAS_DETECTION_RUNNING)
        return;

    int i;
    struct timeval drawTime;
#if 0
    ADAS_LDWS_RESULT ldr = g_spv_camera_state.ldwsResult;
    ADAS_FCWS_RESULT fcr = g_spv_camera_state.fcwsResult;
#else
    ADAS_LDWS_RESULT ldr = g_spv_camera_state.adasResult.adas_ldws_result;
    ADAS_FCWS_RESULT fcr = g_spv_camera_state.adasResult.adas_fcws_result;
#endif

    int midRect = -1;
    int midmostGap = VIEWPORT_WIDTH;
    RECT rect;
    //FCWS
    if(fcr.target_num > 0) {

        for(i = 0; (i < fcr.target_num) && (i < ADAS_FCWS_TARGET_MAX); i ++) {
#if 0
            if(fcr.target_info[i].target_distance < 15) {
                SetPenColor(hdc, PIXEL_red);
            } else {
                SetPenColor(hdc, PIXEL_green);
            }
            Rectangle(hdc, fcr.target_info[i].target_left, fcr.target_info[i].target_top, fcr.target_info[i].target_right, fcr.target_info[i].target_bottom);
#else
            if(fcr.target_info[i].target_distance < 15) {
                SetBrushColor(hdc, PIXEL_red);
            } else {
                SetBrushColor(hdc, PIXEL_green);
            }
            rect.left = fcr.target_info[i].target_left;
            rect.top = fcr.target_info[i].target_top;
            rect.right = fcr.target_info[i].target_right;
            rect.bottom = fcr.target_info[i].target_bottom;

            FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
            SetBrushColor(hdc, SPV_COLOR_TRANSPARENT);
            FillBox(hdc, rect.left + 3, rect.top + 3, RECTW(rect) - 6, RECTH(rect) - 6);
#endif
            int gap = abs(fcr.target_info[i].target_left + fcr.target_info[i].target_right - VIEWPORT_WIDTH);
            if(gap < midmostGap) {
                midmostGap = gap;
                midRect = i;
            }
        }

        //show distance for the middle car 
        char distance[64] = {0};
        sprintf(distance, "%d m", fcr.target_info[midRect].target_distance);
        RECT r = {0, VIEWPORT_HEIGHT - 100, VIEWPORT_WIDTH, VIEWPORT_HEIGHT - 60};
        SetBkMode(hdc, BM_TRANSPARENT);
        SelectFont(hdc, g_logfont_mode);
        if(fcr.target_info[midRect].target_distance < 15) {
            SetTextColor(hdc, PIXEL_red);
        } else {
            SetTextColor(hdc, PIXEL_lightwhite);
        }
        DrawText(hdc, distance, -1, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    SetPenWidth(hdc, 5);
    //LDWS
    //if(ldr.ldws_lanemark & ADAS_LDWS_LANEMARK_LEFT_FIND) 
    {//left line
        if(ldr.veh_ldws_state == ADAS_LDWS_STATE_DEPARTURE_LEFT) {
            SetPenColor(hdc, PIXEL_red);
        } else {
            SetPenColor(hdc, PIXEL_green);
        }
        LineEx(hdc, ldr.left_lane_x1, ldr.left_lane_y1, ldr.left_lane_x2, ldr.left_lane_y2);
    }
    //if(ldr.ldws_lanemark & ADAS_LDWS_LANEMARK_RIGHT_FIND) 
    {//right line
        if(ldr.veh_ldws_state == ADAS_LDWS_STATE_DEPARTURE_RIGHT) {
            SetPenColor(hdc, PIXEL_red);
        } else {
            SetPenColor(hdc, PIXEL_green);
        }
        LineEx(hdc, ldr.right_lane_x1, ldr.right_lane_y1, ldr.right_lane_x2, ldr.right_lane_y2);
    }

#endif
}

static int ParseAdasAndWarning(ADAS_PROC_RESULT* adasResult)
{
    int ret = -1;
    static long last_ldws_warn_time = 0l;
    static long last_fcws_warn_time = 0l;
    long current_time = 0l;
    struct timeval ct;

    ADAS_LDWS_RESULT ldr = adasResult->adas_ldws_result;
    ADAS_FCWS_RESULT fcr = adasResult->adas_fcws_result;
    double currentSpeed = g_spv_camera_state.gpsData.speed;

    //hear is for video timer to update the lapse time
    DealClockTimer(g_main_window);

#ifdef GUI_GPS_ENABLE
    //If gps enable, adas warning while speed is faster than 20km/h
    if(currentSpeed < 20 || g_spv_camera_state.gpsData.speed == 0xFF)
        return ret;
#endif

    gettimeofday(&ct, 0);
    current_time = ct.tv_sec * 1000 + ct.tv_usec / 1000;
    if(ldr.veh_ldws_state == ADAS_LDWS_STATE_DEPARTURE_LEFT || ldr.veh_ldws_state == ADAS_LDWS_STATE_DEPARTURE_RIGHT) 
    {
        if(last_ldws_warn_time != 0 && (current_time - last_ldws_warn_time > 2500) //departure hold more than 2sec
                && (current_time - last_ldws_warn_time < 5000)) {//ldws warning
            last_ldws_warn_time = 0;
            SpvPlayAudio(SPV_WAV[WAV_LDWS_WARN]);
            ERROR("[ADAS--WARNING], ADAS_LDWS_STATE_DEPARTURE, %d\n", ldr.veh_ldws_state);
            ret = 0;
        } else if (last_ldws_warn_time == 0 || current_time - last_ldws_warn_time >= 5000) {
            last_ldws_warn_time = current_time;
        }
    } else {
        last_ldws_warn_time = 0;
    }

    int i = 0;
    int midRect = 0;
    int midmostGap = VIEWPORT_WIDTH;
    if(fcr.target_num <= 0)
        return ret;

    for(i = 0; (i < fcr.target_num) && (i < ADAS_FCWS_TARGET_MAX); i ++) {
        int gap = abs(fcr.target_info[i].target_left + fcr.target_info[i].target_right - VIEWPORT_WIDTH);
        if(gap < midmostGap) {
            midmostGap = gap;
            midRect = i;
        }
    }
    if((last_fcws_warn_time == 0 || (current_time - last_fcws_warn_time > 2000)) && //fcws warning every 2sec
#ifdef GUI_GPS_ENABLE
            (((double)fcr.target_info[midRect].target_distance * 3.6f/ currentSpeed) < 2.0f)//2.0sec will collide
#else
            fcr.target_info[midRect].target_distance < 15
#endif
            ) {
        ERROR("[ADAS--WARNING], ADAS_FCWS, %dm\n", fcr.target_info[midRect].target_distance);
        SpvPlayAudio(SPV_WAV[WAV_FCWS_WARN]);
        last_fcws_warn_time = current_time;
        ret = 0;
    }

    return ret;
}

BOOL effect_proc(HWND hwnd, int timerId, DWORD time)
{
    g_spv_camera_state.isPhotoEffect = FALSE;
    g_force_cleanup_secondarydc = TRUE;
    InvalidateRect(hwnd, 0, TRUE);
    return FALSE;
}

static void ShowPicturingEffect(HWND hwnd, BOOL sequence)
{
    g_spv_camera_state.isPhotoEffect = TRUE;
    SetTimerEx(hwnd, TIMER_PHOTO_EFFECT, sequence ? 60 : 30, effect_proc);
    InvalidateRect(hwnd, 0, TRUE);
}

static int UpdateCapacityStatus()
{
    char capacity[128] = {0};
    int videoCount = 0, picCount = 0;
    g_spv_camera_state.availableSpace = GetAvailableSpace();
    GetMediaFileCount(&videoCount, &picCount);
#ifdef GUI_WIFI_ENABLE
    sprintf(capacity, "%ld/%d/%d", g_spv_camera_state.availableSpace, videoCount , picCount);
    SetConfigValue(g_status_keys[STATUS_CAPACITY], capacity, FROM_DEVICE);
#endif

    return 0;
}

static int UpdateWorkingStatus()
{
#ifdef GUI_WIFI_ENABLE
    SetConfigValue(g_status_keys[STATUS_WORKING], 
            (g_spv_camera_state.isRecording || g_spv_camera_state.isPicturing) ? 
            GETVALUE(STRING_ON) : GETVALUE(STRING_OFF), FROM_DEVICE);
#endif
    return 0;
}

static void DrawMainWindow(HWND hWnd, HDC hdc)
{

    int x, y, w, h;
#if !COLORKEY_ENABLED && !defined(UBUNTU) //only do in arm with no colorkey 
    SetBrushColor (hdc, RGBA2Pixel (hdc, 0x01, 0x01, 0x01, 0x0));
    FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
    SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA,0);
    SetBkMode (hdc, BM_TRANSPARENT);
#endif

    //draw adas
    DrawAdasDetectionResult(hdc);

    //for reverse image
    if(g_spv_camera_state.reverseImage) {
        INFO("id: %d, length:%d, ic_max: %d\n", IC_REVERSE_IMAGE, IC_MAX, IC_MAX);
        LoadMainWindowIcon(IC_REVERSE_IMAGE);
        FillBoxWithBitmap(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, &g_icon[IC_REVERSE_IMAGE]);
        return;
    } else {
        FreeGlobalIcon(IC_REVERSE_IMAGE);
    }

    if(g_spv_camera_state.isPhotoEffect) {
        return;
    }
#if 0// COLORKEY_ENABLED 
    mem_dc = CreateMemDC (DEVICE_WIDTH, DEVICE_HEIGHT, 16, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
            0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0x00, 0x00, 0x00, 0x50));
    FillBox(mem_dc, 0, 0, DEVICE_WIDTH, MW_BAR_HEIGHT * SCALE_FACTOR);
    FillBox(mem_dc, 0, DEVICE_HEIGHT - MW_BAR_HEIGHT * SCALE_FACTOR, DEVICE_WIDTH, MW_BAR_HEIGHT * SCALE_FACTOR);
    BitBlt(mem_dc, 0, 0, DEVICE_WIDTH, DEVICE_HEIGHT, hdc, 0, 0, 0);
    DeleteMemDC(mem_dc);
#else
#ifdef GUI_SHOW_STATUS_BAR
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, MW_BAR_HEIGHT);
    FillBox(hdc, 0, VIEWPORT_HEIGHT - MW_BAR_HEIGHT, VIEWPORT_WIDTH, MW_BAR_HEIGHT);
#endif
#endif
//#define GET_BMP
#ifdef GET_BMP
    int width = 300;//200
    int height = 128;//72
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, (VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, width, height);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, (VIEWPORT_WIDTH - width )/2 + 4, (VIEWPORT_HEIGHT - height )/2 + 4, width - 8, height - 8);
    RECT r = {(VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, VIEWPORT_WIDTH / 2 + width / 2, VIEWPORT_HEIGHT / 2 + height / 2};
    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkColor(hdc, PIXEL_transparent);
#if 1
    DrawText(hdc, "准备存储卡中", -1, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
#else
    r.top += 32;//16
    DrawText(hdc, "请插入存储卡,", -1, &r, DT_SINGLELINE | DT_CENTER | DT_TOP);
    r.top += 32;//22
    DrawText(hdc, "新卡将会格式化", -1, &r, DT_SINGLELINE | DT_CENTER | DT_TOP);
#endif
    SetBrushColor(hdc, PIXEL_black);
    Rectangle(hdc, (VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, (VIEWPORT_WIDTH + width) / 2 - 1, (VIEWPORT_HEIGHT + height) / 2 - 1);
#endif

    if(g_recompute_locations) {
        g_recompute_locations = FALSE;
        ComputeItemsLocation(hdc, g_spv_camera_state.mode == MODE_VIDEO);
    }   
    int i = 0, begin, end;
    if(g_spv_camera_state.mode == MODE_VIDEO) {
        begin = MWI_VIDEO_BEGIN;
        end = MWI_VIDEO_END;
    } else {
        begin = MWI_PHOTO_BEGIN;
        end = MWI_PHOTO_END;
    }

    for(i = begin; i < end + 1; i ++)
    {
        PMWITEM pItem = &g_items[i];
        if(!(pItem->visibility))
            continue;

        switch(pItem->style) {
            case MW_STYLE_ICON:
                {
                    LoadMainWindowIcon(pItem->iconId);
                    w = g_icon[pItem->iconId].bmWidth;
                    h = g_icon[pItem->iconId].bmHeight;
                    x = (pItem->rect).left + (RECTW(pItem->rect) - w) / 2;
                    y = (pItem->rect).top + (RECTH(pItem->rect) - h) / 2;
                    FillBoxWithBitmap(hdc, x, y, w, h, &g_icon[pItem->iconId]);
                    break;
                }
            case MW_STYLE_TEXT:
                if(i == MWV_ZOOM || i == MWP_ZOOM) {
                    if(atof(pItem->text + 1) <= 1.0f)
                        continue;
                }
                SelectFont(hdc, g_logfont_info);
                SetTextColor(hdc, PIXEL_lightwhite);
                SetBkMode(hdc, BM_TRANSPARENT);
                SetBkColor(hdc, PIXEL_black);

                DrawText(hdc, pItem->text, -1, &(pItem->rect), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                break;
        }
    }
    /*if(g_spv_camera_state.isPhotoEffect) {
        SetBrushColor(hdc, PIXEL_red);
        int border = 5;
        FillBox(hdc, 0, 0, VIEWPORT_WIDTH, border);
        FillBox(hdc, 0, 0, border, VIEWPORT_HEIGHT);
        FillBox(hdc, VIEWPORT_WIDTH - border, 0, border, VIEWPORT_HEIGHT);
        FillBox(hdc, 0, VIEWPORT_HEIGHT - border, VIEWPORT_WIDTH, border);
//        return;
    }*/
    if(!g_spv_camera_state.isRecording)
        INFO("DrawMainWindow\n");
}

static void DrawPlaybackOrSetup(HWND hWnd, HDC hdc)
{
    int iconid, stringid;
    int textHeight;
    RECT rect;
    int x, y;
    SIZE size;
    if(g_spv_camera_state.mode == MODE_PLAYBACK) {
        iconid = IC_PLAYBACK_MODE;
        stringid = STRING_PLAYBACK;
    } else if(g_spv_camera_state.mode == MODE_SETUP) {
        iconid = IC_SETUP_MODE;
        stringid = STRING_SETUP;
    } else {
        return;
    }

    if(g_icon[iconid].bmWidth == 0) {
        LoadBitmap(hdc, &g_icon[iconid], SPV_ICON[iconid]);
    }

    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, 60, 40, VIEWPORT_WIDTH - 60 * 2, VIEWPORT_HEIGHT - 40 * 2);

    SelectFont(hdc, g_logfont_mode);
    GetTextExtent(hdc, GETSTRING(stringid), -1, &size);

    x = (VIEWPORT_WIDTH - g_icon[iconid].bmWidth) >> 1;
    y = (VIEWPORT_HEIGHT - (g_icon[iconid].bmHeight + size.cy + 5)) >> 1;
    FillBoxWithBitmap(hdc, x, y, g_icon[iconid].bmWidth, g_icon[iconid].bmHeight, &g_icon[iconid]);

    rect.left = 0;
    rect.right = VIEWPORT_WIDTH;
    rect.top = y + g_icon[iconid].bmHeight + 5;
    rect.bottom = VIEWPORT_HEIGHT;
    //INFO("rect: (%d, %d, %d, %d), bm:(%d, %d)\n", rect.left, rect.top, rect.right, rect.bottom, g_icon[iconid].bmWidth, g_icon[iconid].bmHeight);
    DrawText(hdc, GETSTRING(stringid), -1, &rect, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
}

static BOOL IsTimeLapseUpdated()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);
    int timeLapse = ((currentTime.tv_sec- g_spv_camera_state.beginTime.tv_sec) * 10 + 
            (currentTime.tv_usec - g_spv_camera_state.beginTime.tv_usec) / 100000) / 10;
    if(timeLapse != g_spv_camera_state.timeLapse) {
        g_spv_camera_state.timeLapse = timeLapse;
        return TRUE;
    }
    
    return FALSE;
}

static void GetLapseString(char *lstring)
{
    if(lstring == NULL)
        return;

    int timeRemain;
    int timeLapse;
    int countLapse;
    char value[VALUE_SIZE] = {0};
    
    if(g_spv_camera_state.mode == MODE_VIDEO) {
        if(!IsSdcardMounted()) {
            strcpy(lstring, "00:00:00");
            return;
        }
        if(g_spv_camera_state.isRecording) {
            GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
            int loopTime = atoi(value) * 60;
            if(loopTime == 0) {//here if the loop recording is off, change to 5 minutes
                loopTime = 5 * 60;
            }
            timeLapse = g_spv_camera_state.timeLapse;
            if(loopTime != 0) {
                if(timeLapse != 0 && (timeLapse % loopTime == 0)) {
                    g_spv_camera_state.videoCount += 1;
                    if(g_spv_camera_state.isLocked) {//loop record next video, kill the lock icon
                        INFO("reset isLocked\n");
                        g_spv_camera_state.isLocked = FALSE;
                        g_items[MWV_LOCKED].visibility = FALSE;
                        InvalidateItemRect(g_main_window, MWV_LOCKED, TRUE);
                    }
                }
#ifdef GUI_TIMER_RESET
                timeLapse = timeLapse % loopTime;
#endif
            }
            TimeToString(timeLapse, lstring);
        } else {
#if SD_VOLUME_ESTIMATE_ENABLED
            GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
            timeRemain = GetVideoTimeRemain(g_spv_camera_state.availableSpace, value);
            TimeToString(timeRemain, lstring);
#else
            strcpy(lstring, "00:00:00");
#endif
        }
    } else if(g_spv_camera_state.mode == MODE_PHOTO) {
#if SD_VOLUME_ESTIMATE_ENABLED
        if(!IsSdcardMounted()) {
            strcpy(lstring, "0 | 0");
            return;
        }
        GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), value);
        sprintf(lstring, "%d | %d", g_spv_camera_state.photoCount, GetPictureRemain(g_spv_camera_state.availableSpace, value));
#else
        if(!IsSdcardMounted()) {
            strcpy(lstring, "0");
            return;
        }
        sprintf(lstring, "%d", g_spv_camera_state.photoCount);
#endif
    }
}

static BOOL DelayScanning(HWND hwnd, int timerId, DWORD time)
{
    INFO("timerproc\n");
    ScanSdcardMediaFiles(IsSdcardMounted());
    return FALSE;
}

void GoingToScanMedias(HWND hWnd, int delay)
{
    if(delay > 0) {
        INFO("GoingToScanMedias\n");
        KillTimer(hWnd, TIMER_DELAY_SCANNING);
        SetTimerEx(hWnd, TIMER_DELAY_SCANNING, delay, DelayScanning);
    } else {
        ScanSdcardMediaFiles(IsSdcardMounted());
    }
}


static BOOL IsSdcardFull()
{
    if(IsSdcardMounted()) {
        if(g_spv_camera_state.mode == MODE_VIDEO) {
            if(g_spv_camera_state.availableSpace < SDCARD_FREE_SPACE_NOT_ENOUGH_VALUE * 1024) {//60M at least for video recording
                INFO("sdcard is full!, availableSpace: %ldKB\n", g_spv_camera_state.availableSpace);
                return TRUE;
            }
        } else if(g_spv_camera_state.mode == MODE_PHOTO) {
            if(g_spv_camera_state.availableSpace < SDCARD_FREE_SPACE_PHOTO_NOT_ENOUGH_VALUE * 1024) {//10M at least for photo taking
                INFO("sdcard is full!, availableSpace: %ldKB\n", g_spv_camera_state.availableSpace);
                return TRUE;
            }
        }
    }

    return FALSE;
}

HWND ShowSDErrorToast(HWND hWnd) {
    HWND toastWnd = HWND_INVALID;
    switch(g_spv_camera_state.sdStatus) {
        case SD_STATUS_UMOUNT:
            toastWnd = ShowToast(hWnd, TOAST_SD_NO, 160);
            break;
        case SD_STATUS_ERROR:
            toastWnd = ShowToast(hWnd, TOAST_SD_ERROR, 160);
            break;
        case SD_STATUS_NEED_FORMAT:
            toastWnd = ShowToast(hWnd, TOAST_SD_FORMAT, 160);
            break;
        default:
            break;
    }
    return toastWnd;
}

static int StartVideoRecord(HWND hWnd)
{
    if(g_spv_camera_state.isRecording || (g_spv_camera_state.sdStatus != SD_STATUS_MOUNTED))
        return 0;

    SetCameraConfig(TRUE);//make sure all the config has been set before recording
    INFO("StartVideoRecord\n");
    if(!g_camera->start_video()) {//start recording
        SetHighLoadStatus(1);
        g_status->idle = 0;
        g_spv_camera_state.isRecording = TRUE;
        g_spv_camera_state.videoCount += 1;
        gettimeofday(&g_spv_camera_state.beginTime, 0);
        g_spv_camera_state.timeLapse = 0;
        GetLapseString(g_items[MWV_LAPSE].text);
        SetTimer(hWnd, TIMER_CLOCK, 10);
        g_items[MWV_MODE].iconId = IC_VIDEO_RECORDING;
#ifdef GUI_ADAS_ENABLE
        char value[VALUE_SIZE] = {0};
        GetConfigValue(GETKEY(ID_VIDEO_ADAS), value);
        if(!strcasecmp(value, GETVALUE(STRING_ON)))
            StartAdasDetection();
#endif
    } else {
        ShowToast(hWnd, TOAST_CAMERA_BUSY, 160);
        return ERROR_CAMERA_BUSY;
    }

    return 0;
}

static int StopVideoRecord(HWND hWnd)
{
    if(!g_spv_camera_state.isRecording)
        return 0;

    INFO("StopVideoRecord\n");
    int ret = g_camera->stop_video();
    SetHighLoadStatus(0);
	UpdateCapacityStatus();
    g_status->idle = 1;
    gettimeofday(&(g_status->idleTime), 0);
    g_spv_camera_state.isRecording = FALSE;
    g_spv_camera_state.isLocked = FALSE;
    g_items[MWV_LOCKED].visibility = FALSE;
    GetLapseString(g_items[MWV_LAPSE].text);
    KillTimer(hWnd, TIMER_CLOCK);
    g_items[MWV_MODE].iconId = IC_VIDEO;
#ifdef GUI_ADAS_ENABLE
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(ID_VIDEO_ADAS), value);
    if(!strcasecmp(value, GETVALUE(STRING_ON)))
        StopAdasDetection();
#endif
    GoingToScanMedias(hWnd, 250);
    if(ret) {
        ShowToast(hWnd, TOAST_CAMERA_BUSY, 160);
        return ERROR_CAMERA_BUSY;
    }

    return 0;
}

static int TakePicture(HWND hWnd)
{
    INFO("TakePicture\n");
    char value[VALUE_SIZE] = {0};
    if(!g_camera->start_photo()) {
        g_camera->stop_photo();
        //UpdateCapacityStatus();
        GetConfigValue(GETKEY(ID_PHOTO_SEQUENCE), value);
        BOOL sequence = !strcasecmp(value, GETVALUE(STRING_ON));
        ShowPicturingEffect(hWnd, sequence);
        UpdatePhotoCount(hWnd, g_spv_camera_state.photoCount + (sequence ? 3 : 1));
        SpvPlayAudio(SPV_WAV[sequence ? WAV_PHOTO3 : WAV_PHOTO]);
        GoingToScanMedias(hWnd, sequence ? 150 : 100);
    } else {
        ShowToast(hWnd, TOAST_CAMERA_BUSY, 160);
        return ERROR_CAMERA_BUSY;
    }

    return 0;
}

static int DoShutterAction(HWND hWnd)
{
    if(g_spv_camera_state.mode == MODE_SETUP) {
        ShowSetupDialogByMode(hWnd, g_spv_camera_state.mode);
        return 0;
    }

    if(g_spv_camera_state.mode == MODE_PLAYBACK) {
        ShowFolderDialog(hWnd, 0);
        return 0;
    }

    if(!IsSdcardMounted() && !g_spv_camera_state.isPicturing && !g_spv_camera_state.isRecording) {
        ShowSDErrorToast(hWnd);
        if(g_spv_camera_state.sdStatus == SD_PREPARE) {
            ShowToast(hWnd, TOAST_SD_PREPARE, 60);
        }
        return 1;
    }

    //g_spv_camera_state.availableSpace = GetAvailableSpace();

    //if(!g_spv_camera_state.isPicturing && !g_spv_camera_state.isRecording && IsSdcardFull()) {
/*    if(!g_spv_camera_state.isPicturing && !g_spv_camera_state.isRecording) {
		//printf("[UICHANGES]:changes by fred!\n");
    	g_spv_camera_state.availableSpace = GetAvailableSpace();
		if(IsSdcardFull()){
        	ShowToast(hWnd, TOAST_SD_FULL, 160);
        	return 1;
		}
    }*/

    int ret = 0;
    if(g_spv_camera_state.mode == MODE_VIDEO) {
        if(g_spv_camera_state.isRecording) {//stop recording
            ret = StopVideoRecord(hWnd);

        } else {	
			//printf("[UICHANGES]:changes by fred!\n");
    		g_spv_camera_state.availableSpace = GetAvailableSpace();
            ret = StartVideoRecord(hWnd);
        }
    } else if(g_spv_camera_state.mode == MODE_PHOTO) {
		//printf("[UICHANGES]:changes by fred!\n");
    	g_spv_camera_state.availableSpace = GetAvailableSpace();
        if(g_spv_camera_state.isPicturing)
            return 0;

        //SetCameraConfig(FALSE);
        char value[VALUE_SIZE] = {0};
        GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), value);
        if(atoi(value)) {
            g_status->idle = 0;
            g_spv_camera_state.isPicturing = TRUE;
            SetTimer(hWnd, TIMER_COUNTDOWN, 10);
            sprintf(g_items[MWP_TIMER].text, "%d", atoi(value));
            g_items[MWP_TIMER].style = MW_STYLE_TEXT;
            gettimeofday(&g_spv_camera_state.beginTime, 0);
            g_spv_camera_state.timeLapse = 0;
            g_items[MWP_MODE].iconId = IC_PHOTO_RECORDING;
        } else {
            ret = TakePicture(hWnd);
        }
    }

    if(ret) {
        ERROR("DoShutterAction, ret: %d\n", ret);
        return ret;
    }

    UpdateWorkingStatus();

    UpdateMainWindow(hWnd);
    return 0;
}

static BOOL LongPressInterpolation(HWND hWnd, int timerId, DWORD time)
{
    static int interTimes;
    if(timerId == TIMER_LONGPRESS_UP || timerId == TIMER_LONGPRESS_DOWN) {
        interTimes ++;
        PostMessage(hWnd, MSG_KEYUP, (timerId == TIMER_LONGPRESS_DOWN) ? SCANCODE_SPV_DOWN : SCANCODE_SPV_UP, 0);
        if(interTimes % 3 == 0)
            return FALSE;
    }
    return TRUE;
}

static void ElectronicZoom(HWND hWnd, BOOL increased, LPARAM longPress)
{
    if(g_spv_camera_state.mode > MODE_PHOTO)
        return;

    if(!IsMainWindowActive())
        return;

    if(g_spv_camera_state.isPicturing) //g_spv_camera_state.isRecording || 
        return;

    char value[VALUE_SIZE] = {0};
    float zoom = 1.0f;
    float oldZoom = 1.0f;
    BOOL valueChanged = FALSE;
    if(g_spv_camera_state.mode == MODE_VIDEO) {
        GetConfigValue(GETKEY(ID_VIDEO_ZOOM), value);
        //printf("video.zoom: %s\n", value);
        oldZoom = atof(value + 1);
        zoom = oldZoom + (increased ? 0.1f : -0.1f);
        if(zoom < 1.0f)
            zoom = 1.0f;
        if(zoom > 3.0f)
            zoom = 3.0f;
        if(oldZoom == zoom)
            return;
        valueChanged = TRUE;
        memset(value, 0, sizeof(value));
        sprintf(value, "x%.1f", zoom);
        SetConfigValue(GETKEY(ID_VIDEO_ZOOM), value, FROM_USER);
        strcpy(g_items[MWV_ZOOM].text, value);
        InvalidateItemRect(hWnd, MWV_ZOOM, TRUE);
    } else if(g_spv_camera_state.mode == MODE_PHOTO) {
        GetConfigValue(GETKEY(ID_PHOTO_ZOOM), value);
        oldZoom = atof(value + 1);
        zoom = oldZoom + (increased ? 0.1f : -0.1f);
        if(zoom < 1.0f)
            zoom = 1.0f;
        if(zoom > 3.0f)
            zoom = 3.0f;
        if(oldZoom == zoom)
            return;
        valueChanged = TRUE;
        memset(value, 0, sizeof(value));
        sprintf(value, "x%.1f", zoom);
        SetConfigValue(GETKEY(ID_PHOTO_ZOOM), value, FROM_USER);
        strcpy(g_items[MWP_ZOOM].text, value);
        InvalidateItemRect(hWnd, MWP_ZOOM, TRUE);
    }
    
    if(valueChanged) {
        struct timeval start, end;
        gettimeofday(&start, 0);
        SetCameraConfig(TRUE);
        gettimeofday(&end, 0);
        //INFO("elctronic time waste: %ld\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
    }

    if(longPress) {//double fast the keys
        //PostMessage(hWnd, MSG_KEYUP, increased ? SCANCODE_SPV_UP : SCANCODE_SPV_DOWN, 0);
        static BOOL timerCreated = FALSE;
        if(timerCreated) {
            ResetTimerEx(hWnd, increased ? TIMER_LONGPRESS_UP : TIMER_LONGPRESS_DOWN, 7, LongPressInterpolation);
        } else {
            SetTimerEx(hWnd, increased ? TIMER_LONGPRESS_UP : TIMER_LONGPRESS_DOWN, 7, LongPressInterpolation);
        }
    }

}

static void OnKeyDown(HWND hWnd, int keyCode, LPARAM lParam) 
{
}

static void OnKeyUp(HWND hWnd, int keyCode, LPARAM lParam) 
{
    INFO("OnKeyUp, keyCode: %d, lParam: %ld\n", keyCode, lParam);
    char value[VALUE_SIZE] = {0};
    switch(keyCode) {
        case SCANCODE_SPV_MENU:
            if(g_spv_camera_state.mode == MODE_VIDEO && g_spv_camera_state.isRecording) 
            {
                if(!g_camera->start_photo()) {
                    g_camera->stop_photo();
                    UpdatePhotoCount(hWnd, g_spv_camera_state.photoCount + 1);
                    GoingToScanMedias(hWnd, 200);
                }
				return;
            }

            if(g_spv_camera_state.mode == MODE_PHOTO && g_spv_camera_state.isPicturing)
                return;
            if(g_spv_camera_state.mode == MODE_PLAYBACK) {
                //ShowFolderDialog(hWnd, 0);
            } else {
                ShowSetupDialogByMode(hWnd, g_spv_camera_state.mode);
            }
            //InitDialogBox(hWnd, 0);
            break;
        case SCANCODE_SPV_MODE:
            if(g_spv_camera_state.mode == MODE_PHOTO && g_spv_camera_state.isPicturing)
                return;
            if(g_spv_camera_state.mode == MODE_VIDEO && g_spv_camera_state.isRecording) {
                GetConfigValue(GETKEY(ID_SETUP_IR_LED), value);
                if(!strcasecmp(value, GETVALUE(STRING_ON))) {
                    SetConfigValue(GETKEY(ID_SETUP_IR_LED), GETVALUE(STRING_OFF), FROM_USER);
                    g_items[MWV_LIGHT].iconId = IC_IR_DAYTIME;
                } else {
                    SetConfigValue(GETKEY(ID_SETUP_IR_LED), GETVALUE(STRING_ON), FROM_USER);
                    g_items[MWV_LIGHT].iconId = IC_IR_NIGHT;
                }
                InvalidateItemRect(hWnd, MWV_LIGHT, TRUE);
                SetIrLed();
                return;
            }

            ChangeToMode(hWnd, (g_spv_camera_state.mode + 1) % MODE_COUNT);
            break;
        case SCANCODE_SPV_OK:
            if(lParam > 0) {
                //here is a long press event, ok key should be ignored
                return;
            }
            DoShutterAction(hWnd);
            break;
        case SCANCODE_SPV_UP:
//#define GUI_DEBUG_FB1
#ifdef GUI_DEBUG_FB1
            if(lParam == 0)
            {
                SAVE_SCREEN = 1;
                struct timeval tm;
                gettimeofday(&tm, NULL);
                char cmd[128] = {0};
                sprintf(cmd, "dd if=/dev/fb1 of=/mnt/sd0/fb1.%ld.data; sync", tm.tv_sec);
                system(cmd);
            }
#endif 
#ifndef GUI_ROTATE_SCREEN
            //ElectronicZoom(hWnd, TRUE, lParam);
#endif
            break;
        case SCANCODE_SPV_DOWN:
#ifdef GUI_DEBUG_FB1
            if(lParam == 0)
            {
                SAVE_SCREEN = 1;
                struct timeval tm;
                gettimeofday(&tm, NULL);
                char cmd[128] = {0};
                sprintf(cmd, "dd if=/dev/fb1 of=/mnt/sd0/fb1.%ld.data; sync", tm.tv_sec);
                system(cmd);
            }
#endif
#ifndef GUI_ROTATE_SCREEN
            //ElectronicZoom(hWnd, FALSE, lParam);
#endif
            break;
        case SCANCODE_SPV_SWITCH:
#ifndef GUI_CAMERA_REAR_ENABLE
            OnKeyUp(hWnd, SCANCODE_SPV_OK, lParam);
            return;
#endif
            if(g_spv_camera_state.mode != MODE_VIDEO || !IsMainWindowActive())
                return;
            GetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), value);
            INFO("SCANCODE_SPV_SWITCH, frontbig: %s\n", value);
            if(!strcasecmp(value, GETVALUE(STRING_ON))) {
                SetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_OFF), FROM_USER);
            } else {
                SetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_ON), FROM_USER);
            }
            SetCameraConfig(TRUE);
            break;
        case SCANCODE_SPV_LOCK:
            if(g_spv_camera_state.mode == MODE_VIDEO && g_spv_camera_state.isRecording) {
                SpvSetCollision(hWnd, g_camera);
            }
            break;
        case SCANCODE_SPV_PHOTO:
            if(!g_camera->start_photo()) {
                g_camera->stop_photo();
                UpdatePhotoCount(hWnd, g_spv_camera_state.photoCount + 1);
                GoingToScanMedias(hWnd, 200);
            }
            break;
        default:
            break;
    }
}

static void OnLongKeyPress(int keyCode, int duration)
{
    INFO("onLongKeyPress, keyCode: %d, duration: %d\n", keyCode, duration);
}

static void UpdatePhotoCount(HWND hWnd, unsigned int photoCount)
{
    INFO("UpdatePhotoCount: %d\n", photoCount);
    if(IsMainWindowActive() && (g_spv_camera_state.mode == MODE_PHOTO)) {
        int oldCount = g_spv_camera_state.photoCount;
        if(photoCount == oldCount) {
            return;
        } 
        g_spv_camera_state.photoCount = photoCount;
        GetLapseString(g_items[MWP_COUNT].text);

        if((photoCount == 0 && oldCount > 9) ||
            (photoCount > 9 && oldCount == 0) || 
            (log10(photoCount) != log10(oldCount))) { // strlen change
            UpdateMainWindow(hWnd);
            /*RECT rect = g_items[MWP_COUNT].rect;
            rect.right = g_items[MWP_BATTERY].rect.left;
            InvalidateRect(hWnd, &rect, TRUE);//invalidate top pannel*/
        } else {
            InvalidateItemRect(hWnd, MWP_COUNT, TRUE);
        }
    } else  {
        g_spv_camera_state.photoCount = photoCount;
        GetLapseString(g_items[MWP_COUNT].text);
    }
}


static int DealClockTimer(HWND hWnd)
{
    char lapseString[VALUE_SIZE]  = {0};
    if(!g_spv_camera_state.isRecording)
        return 0;

    if(IsTimeLapseUpdated()) {
        GetLapseString(g_items[MWV_LAPSE].text);
        InvalidateItemRect(hWnd, MWV_LAPSE, TRUE);
    }
    return 0;
}

static int DealCountdownTimer(HWND hWnd)
{
    struct timeval currentTime;
    int timeLapse, timeCountdown;
    char value[VALUE_SIZE] = {0};

    if(!g_spv_camera_state.isPicturing || !IsTimeLapseUpdated()) {
        return 0;
    }

    GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), value);
    int countTime = atoi(value);
    if(g_spv_camera_state.timeLapse >= countTime) {
        INFO("time countdown ready, take photo\n");
        KillTimer(hWnd, TIMER_COUNTDOWN);
        g_status->idle = 1;
        gettimeofday(&(g_status->idleTime), 0);
        g_spv_camera_state.isPicturing = FALSE;
        g_items[MWP_TIMER].style = MW_STYLE_ICON;
        g_items[MWP_MODE].iconId = IC_PHOTO;
        int ret = TakePicture(hWnd);
        UpdateMainWindow(hWnd);
        UpdateWorkingStatus();
        return ret;
    } else {
        SpvPlayAudio(SPV_WAV[WAV_KEY]);
        sprintf(g_items[MWP_TIMER].text, "%d", (atoi(value) - g_spv_camera_state.timeLapse));
        InvalidateItemRect(hWnd, MWP_TIMER, TRUE);
    }

    return 0;
}

static BOOL BatteryFlickTimer(HWND hWnd, int id, DWORD time)
{
    static BOOL iconVisible;
    g_items[MWV_BATTERY].visibility = g_items[MWP_BATTERY].visibility = iconVisible = !iconVisible;
    if(g_spv_camera_state.mode == MODE_VIDEO) {
        InvalidateItemRect(hWnd, MWV_BATTERY, TRUE);
    } else {
        InvalidateItemRect(hWnd, MWP_BATTERY, TRUE);
    }
    return TRUE;    
}

static void SpvSetCollision(HWND hWnd, camera_spv *camera) {
    if(camera == NULL) {
        return;
    }
    INFO("SpvSetCollision\n");
    g_spv_camera_state.isLocked = TRUE;
    g_items[MWV_LOCKED].visibility = TRUE;
    g_recompute_locations = TRUE;
    InvalidateRect(hWnd, 0, TRUE);

    config_camera config;
    get_config_camera(&config);
    strcpy(config.other_collide, "On");
    camera->set_config(config);
}


void GetWifiApInfo(char *ssid, char *pswd)
{
    GetConfigValue(GETKEY(ID_WIFI_AP_SSID), ssid);
    GetConfigValue(GETKEY(ID_WIFI_AP_PSWD), pswd);
    if(!strcmp(ssid, DEFAULT)) {
        memset(ssid, 0, strlen(DEFAULT));
    }
    if(strlen(pswd) < 8) {
        memset(pswd, 0, 8);
    }
}

pthread_mutex_t gWifiMutex = PTHREAD_MUTEX_INITIALIZER;

static void SetWifiStatusThread(PWifiOpt pWifi)
{
#ifdef GUI_WIFI_ENABLE
    int ret = -1;

    if(pWifi == NULL)
        return;

    char oldinfo[INFO_WIFI_COUNT][INFO_LENGTH] = {0};

    char *ssid = pWifi->wifiInfo[INFO_SSID];
    if(strlen(ssid) == 0)
        ssid = NULL;
    char *pswd = pWifi->wifiInfo[INFO_PSWD];
    if(strlen(pswd) == 0)
        pswd = NULL;
    char *keymgmt = pWifi->wifiInfo[INFO_KEYMGMT];
    if(strlen(keymgmt) == 0)
        keymgmt = NULL;

    INFO("set wifi status, action:%d, ssid:%s, pswd:%s, keymgmt:%s\n", pWifi->operation, 
            pWifi->wifiInfo[INFO_SSID], pWifi->wifiInfo[INFO_PSWD], pWifi->wifiInfo[INFO_KEYMGMT]);

    pthread_mutex_lock(&gWifiMutex);
    switch(pWifi->operation) {
        case DISABLE_WIFI:
            if(g_spv_camera_state.wifiStatus == WIFI_OFF)
                break;

            g_spv_camera_state.wifiStatus = WIFI_CLOSING;
            SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            if(g_spv_camera_state.wifiStatus == WIFI_AP_ON) {
                ret = g_wifi->stop_softap();
            } else {
                ret = g_wifi->stop_sta();
            }
            if(!ret) {
                g_spv_camera_state.wifiStatus = WIFI_OFF;
                SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            } else {
                ERROR("----shutdown wifi error\n");
            }
            break;
        case ENABLE_AP:
            if(pswd != NULL && strlen(pswd) < 8) {//makesure the pswd is valid
                pswd = NULL;
            }
            GetWifiApInfo(oldinfo[INFO_SSID], oldinfo[INFO_PSWD]);
            if(g_spv_camera_state.wifiStatus == WIFI_AP_ON) {
                INFO("current wifi is in ap mode\n");
                if((ssid == NULL || pswd == NULL) && (!strcmp(oldinfo[INFO_SSID], DEFAULT) && strlen(oldinfo[INFO_PSWD]) < 8)) {
                    //default ssid & password, no need to change
                    break;
                }
                if(((ssid == NULL && strlen(oldinfo[INFO_SSID]) == 0) || !strcmp(ssid, oldinfo[INFO_SSID])) && 
                        ((ssid == NULL && strlen(oldinfo[INFO_SSID]) == 0) || !strcmp(pswd, oldinfo[INFO_PSWD]))) {
                    //same info
                    break;
                }
            }

            g_spv_camera_state.wifiStatus = WIFI_OPEN_AP;
            SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            if(GetActiveWindow() == g_main_window) {
                PostMessage(g_main_window, MSG_UPDATE_WINDOW, 0, 0);
            }
            ret = g_wifi->start_softap(ssid, pswd);
            if(!ret) {
                SetConfigValue(GETKEY(ID_WIFI_AP_SSID), ssid == NULL ? DEFAULT : ssid, FROM_USER);
                SetConfigValue(GETKEY(ID_WIFI_AP_PSWD), pswd == NULL ? DEFAULT : pswd, FROM_USER);
            }
            g_spv_camera_state.wifiStatus = !ret ? WIFI_AP_ON : WIFI_OFF;
            SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            break;
        case ENABLE_STA:
            if(g_spv_camera_state.wifiStatus == WIFI_STA_ON)
                break;

            g_spv_camera_state.wifiStatus = WIFI_OPEN_STA;
            SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            if(GetActiveWindow() == g_main_window) {
                PostMessage(g_main_window, MSG_UPDATE_WINDOW, 0, 0);
            }
            if(ssid == NULL) {
                ERROR("invalid ssid\n");
                break;
            }
            if(keymgmt == NULL) {
                keymgmt = "WPA-PSK";
            }
			if(pswd!=NULL)
			{
	            if(strlen(pswd) < 8 && !strcasecmp(keymgmt, "WPA-PSK")) {
	                ERROR("invalid password: %s\n", pswd);
	                break;
	            }
			}
            int retry = 0;
            while(retry++ < 3) {
                ret = g_wifi->start_sta(ssid, keymgmt, pswd);
                if(!ret) {
                    /*strcpy(g_wifi_sta[0], ssid);
					if(pswd!=NULL)
					{
                      strcpy(g_wifi_sta[1], pswd);
					}
                    strcpy(g_wifi_sta[2], keymgmt);*/
                    SetConfigValue(GETKEY(ID_WIFI_STA_SSID), ssid != NULL ? ssid : DEFAULT, FROM_USER);
                    SetConfigValue(GETKEY(ID_WIFI_STA_PSWD), pswd != NULL ? pswd : DEFAULT, FROM_USER);
                    SetConfigValue(GETKEY(ID_WIFI_STA_KEYMGMT), keymgmt != NULL ? keymgmt : DEFAULT, FROM_USER);
                    break;
                }
                sleep(1);
            }
            g_spv_camera_state.wifiStatus = !ret ? WIFI_STA_ON : WIFI_OFF;
            SendNotifyMessage(GetActiveWindow(), MSG_SET_WIFI_STATUS, g_spv_camera_state.wifiStatus, 0);
            break;
        default:
            break;
    }

    free(pWifi);
    pWifi = NULL;
    pthread_mutex_unlock(&gWifiMutex);

    if(IsMainWindowActive()) {
        PostMessage(g_main_window, MSG_UPDATE_WINDOW, 0, 0);
    }

#endif
}

void SpvSetWifiStatus(PWifiOpt pWifi) 
{
#ifdef GUI_WIFI_ENABLE
    INFO("SpvSetWifiStatus, enable: %d, status: %d\n", pWifi->operation, g_spv_camera_state.wifiStatus);
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    INFO("pthread_create, SpvSetWifiStatus\n");
    pthread_create(&t, &attr, (void *)SetWifiStatusThread, pWifi);
    pthread_attr_destroy (&attr);
#endif
}

int SpvGetWifiStatus() 
{
    return g_spv_camera_state.wifiStatus;
}

static void StartThttpdThread()
{
#ifdef GUI_WIFI_ENABLE
	while(g_wifi->is_softap_enabled() != 2) {
		sleep(1);
	}
	printf("======try to start thttpd=====\n");
	system("/mnt1/usr/sbin/thttpd -C /etc/thttpd.conf &");

#ifdef GUI_EVENT_HTTP_ENABLE
    init_event_server();
#endif
#endif
}

void SpvStartThttpd() 
{
#ifdef GUI_WIFI_ENABLE
    INFO("SpvStartThttpd, try to start thttpd\n");
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    INFO("pthread_create, SpvSetWifiStatus\n");
    pthread_create(&t, &attr, (void *)StartThttpdThread, NULL);
    pthread_attr_destroy (&attr);
#endif
}

static BOOL AutoShutdownTimerProc(HWND hWnd, int timerId, DWORD lapseTime)
{
    static struct timeval sBeginTime = {0, 0};
    if(sBeginTime.tv_sec == 0) {
        gettimeofday(&sBeginTime, 0);
    }
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);
    int lapsetime = currentTime.tv_sec - sBeginTime.tv_sec;
    if((g_spv_camera_state.isRecording && g_spv_camera_state.timeLapse >= 59) ||
            (!g_spv_camera_state.isRecording && lapsetime >= 10)) {
        INFO("AutoShutdownTimerProc, condition satisfied\n");
        ShutdownProcess();
        return FALSE;
    }
    return TRUE;
}

void DelayedShutdownThread(int delay)
{
    usleep(delay * 1000);
    pthread_testcancel();
    ShutdownProcess();
}

/**
 *
 * Delay time and then shutdown,
 * if delay < 0, shutdown process will be canceled.
 **/
void SpvDelayedShutdown(int delay) 
{
    INFO("SpvDelayedShutdown, delay: %d\n", delay);
    static pthread_t t = 0;
    if(delay >= 0) {
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&t, &attr, (void *)DelayedShutdownThread, (void *)delay);
        pthread_attr_destroy (&attr);
    } else {
        if(t > 0) {
            int ret = pthread_cancel(t);
            INFO("cancel pthread, thread: %ld, ret: %d\n", t, ret);
        }
        t = 0;
    }
}

static int SetTimeByData(gps_utc_time timedata){
    INFO("SetTimeByData, y: %d, M: %d, d: %d, h: %d, m: %d, s: %lf\n", timedata.year, timedata.month, 
            timedata.day, timedata.hour, timedata.minute, timedata.second);

	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_year = timedata.year - 1900;
    timeinfo->tm_mon = timedata.month - 1;
    timeinfo->tm_mday = timedata.day;
    timeinfo->tm_hour = timedata.hour;
    timeinfo->tm_min = timedata.minute;
    timeinfo->tm_sec = timedata.second;

    rawtime = mktime(timeinfo);

    struct timeval tv;
    struct timezone tz;
    
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = rawtime + 8 * 60 * 60;
    tv.tv_usec = 0;

    int result = SetTimeToSystem(tv, tz);
    return result;
}

int SetTimeByApp(int y, int M, int d, int h, int m, int s)
{
    int currentIsRecording = g_spv_camera_state.isRecording;
    int sleeptime = 0;

	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_year = y - 1900;
    timeinfo->tm_mon = M - 1;
    timeinfo->tm_mday = d;
    timeinfo->tm_hour = h;
    timeinfo->tm_min = m;
    timeinfo->tm_sec = s;

    rawtime = mktime(timeinfo);

    struct timeval tv;
    struct timezone tz;
    
    gettimeofday(&tv, &tz);

    if(currentIsRecording && abs(tv.tv_sec - rawtime) < 10) {
        INFO("current time: %ld, time to set: %ld, abs < 5, abort it\n", tv.tv_sec, rawtime);
        return 0;
    }

    if(currentIsRecording) {
        StopVideoRecord(g_main_window);
        sleeptime = 1000 * 1000;
        usleep(sleeptime);
    }

    tv.tv_sec = rawtime + sleeptime / 1000000;
    tv.tv_usec = sleeptime % 1000000;

    int ret = SetTimeToSystem(tv, tz);

    if(currentIsRecording) {
        usleep(1500 * 1000);
        StartVideoRecord(g_main_window);
    }
    UpdateMainWindow(g_main_window);
    
    return ret;
}

int isCameraRecording()
{
    return g_spv_camera_state.isRecording == TRUE;
}

int setRecordingStatus(int recording)
{
    if(recording == g_spv_camera_state.isRecording)
        return 0;

    if(recording) {
        StartVideoRecord(g_main_window);
    } else {
        StopVideoRecord(g_main_window);
    }
    UpdateMainWindow(g_main_window);

    return 0;
}

static int MainWindowProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, mem_dc;
    static HWND textHandle = 0;
    RECT rect = {60, 40, 300, 200};
    unsigned int startTick, endTick;
    char mode[VALUE_SIZE] = {0};
    int ret = -1;
    //printf("MainWindowProc, MSG: %d\n", message);

    switch(message) {
        case MSG_CREATE:
        {
            DEVFONT *devfontBold = LoadDevFontFromFile("ttf-fb-rrncnn-0-0-UTF-8", RESOURCE_PATH"./res/SpvfontBold.ttf");
            DEVFONT *devfont = LoadDevFontFromFile("ttf-simsun-rrncnn-0-0-UTF-8", RESOURCE_PATH"./res/Spvfont.ttf");
            g_recompute_locations = TRUE;
            //INFO("devfont: %x\n", devfont);
            int fontsize = LARGE_SCREEN ? 26 : (17 * SCALE_FACTOR);
            g_logfont_info = CreateLogFont("ttf", "simsun", "UTF-8",
                    FONT_WEIGHT_LIGHT, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    fontsize, 0);
            g_logfont_mode = CreateLogFont("ttf", "fb", "UTF-8",
                    FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    fontsize + 6, 0);
            SetWindowFont(hWnd, g_logfont_info);
            SetTimer(hWnd, TIMER_DATETIME, 100);
            //SetTimer(hWnd, TIMER_TIRED_ALARM,  50 * 60 * 100);//60 minutes alarm
            if(!IsSdcardMounted()) {
                ShowSDErrorToast(hWnd);
            }
            //SendNotifyMessage(hWnd, MSG_BATTERY_STATUS, 0, 0);
            UpdateMainWindow(hWnd);
#ifdef GUI_ADAS_ENABLE
            InitAdasDetection();
#endif
            break;
        }
        case MSG_ACTIVE:
            INFO("MSG_ACTIVE, wparam:%d, lparam:%ld\n", wParam, lParam);
            if(wParam) {
                CleanupSecondaryDC(hWnd);
            }
            if(wParam && g_spv_camera_state.mode < MODE_PLAYBACK) {
                if(!SetCameraConfig(TRUE))
                    g_camera->start_preview();
            } else {
                g_camera->stop_preview();
            }
            break;
        case MSG_BATTERY_STATUS:
        case MSG_CHARGE_STATUS:
        {
            INFO("MSG_BATTERY_STATUS, MSG_CHARGE_STATUS, msg: %d, wParam:%d, lParam:%ld\n", message, wParam, lParam);
            if(message == MSG_BATTERY_STATUS) {
                g_spv_camera_state.battery = wParam;//SpvGetBattery();
            } else if(message == MSG_CHARGE_STATUS) {
                g_spv_camera_state.charging = (wParam == BATTERY_CHARGING);//SpvIsCharging();
            }
            if(MSG_CHARGE_STATUS == message && SpvAdapterConnected()) {
                if(IsTimerInstalled(hWnd, TIMER_AUTO_SHUTDOWN)) {
                    KillTimer(hWnd, TIMER_AUTO_SHUTDOWN);
                }
            }
            if(g_spv_camera_state.battery <= 25 && !g_spv_camera_state.charging && !IsTimerInstalled(hWnd, TIMER_BATTERY_FLICK)) {
                SetTimerEx(hWnd, TIMER_BATTERY_FLICK, 50, BatteryFlickTimer);
            } else if(g_spv_camera_state.battery > 25 || g_spv_camera_state.charging) {
                KillTimer(hWnd, TIMER_BATTERY_FLICK);
                g_items[MWV_BATTERY].visibility = g_items[MWP_BATTERY].visibility = TRUE;
            }
            UpdateBatteryIcon();
            UpdateBatteryStatus();
            if(IsMainWindowActive()) {
                InvalidateItemRect(hWnd, MWV_BATTERY, TRUE);
            } else {
                SendNotifyMessage(GetActiveWindow(), message, wParam, lParam);
            }
            return 0;
        }
        case MSG_HDMI_HOTPLUG:
            INFO("MSG_HDMI_HOTPLUG, wParam:%d, lParam:%ld\n", wParam, lParam);
            return 0;
        case MSG_SDCARD_MOUNT:
#ifndef GUI_SMALL_SYSTEM_ENABLE
            INFO("MSG_SDCARD_MOUNT, wParam:%d, lParam:%ld\n", wParam, lParam);
            gettimeofday(&(g_status->idleTime), 0);
            UpdateCapacityStatus();
#ifdef GUI_LCD_DISABLE
            if(wParam == SD_STATUS_NEED_FORMAT) {
                //we formated sdcard when sdcard inserted in hidden style
                SpvFormatSdcard();
                g_spv_camera_state.sdStatus = wParam;
                return 0;
            }
#endif
            if(wParam == 1 && g_spv_camera_state.availableSpace <= 0) {
                ERROR("Outdated message, sdcard status has been changed\n");
                return 0;
            }

            if(g_spv_camera_state.sdStatus == wParam)
                return 0;
            g_spv_camera_state.sdStatus = wParam;
#if SYSTEM_IS_READ_ONLY
            if(IsSdcardMounted()) {
                SaveConfigToDisk();
            }
#endif
            if(IsMainWindowActive()) {
                if(!IsSdcardMounted()) {//sdcard removed
                    if(g_spv_camera_state.isRecording) {
                        g_status->idle = 1;
                        gettimeofday(&(g_status->idleTime), 0);
                        g_spv_camera_state.isRecording = FALSE;
                        g_spv_camera_state.isLocked = FALSE;
                        g_items[MWV_LOCKED].visibility = FALSE;
                        g_camera->stop_video();
                        g_spv_camera_state.timeLapse = 0;
                        KillTimer(hWnd, TIMER_CLOCK);
                    } else if(g_spv_camera_state.isPicturing) {
                        g_status->idle = 1;
                        gettimeofday(&(g_status->idleTime), 0);
                        g_spv_camera_state.isPicturing = FALSE;
                        KillTimer(hWnd, TIMER_COUNTDOWN);
                        g_spv_camera_state.timeLapse = 0;
                    }
                    ShowSDErrorToast(hWnd);
                } else if(IsSdcardFull()) {
                    ShowToast(hWnd, TOAST_SD_FULL, 160);
                }
#ifdef GUI_LCD_DISABLE
                else {//we start record when sdcard inserted in hidden style
                    DoShutterAction(hWnd);
                }
#endif
                UpdateMainWindow(hWnd);
            } else {
                SendMessage(GetActiveWindow(), MSG_SDCARD_MOUNT, wParam, lParam);
            }
            GoingToScanMedias(hWnd, 0);
#endif
            return 0;
        case MSG_MEDIA_SCAN_FINISHED:
            {
                int vc, pc;
                INFO("MSG_MEDIA_SCAN_FINISHED\n");
                GetMediaFileCount(&vc, &pc);
                UpdatePhotoCount(hWnd, pc);
#ifdef GUI_WIFI_ENABLE
                UpdateCapacityStatus();
#endif
                return 0;
            }
        case MSG_CHANGE_MODE:
            ChangeToMode(hWnd, wParam);
            if(!lParam && !IsMainWindowActive()) {
                do {
                    INFO("current active window: %d, g_main_window: %d\n", GetActiveWindow(), g_main_window);
                    SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
                } while(!IsMainWindowActive());
            }
            break;
        case MSG_SHUTTER_PRESSED:
            {
                if(lParam == 1 && g_spv_camera_state.sdStatus != SD_MOUNTED)
                    return 0;
                int ret = DoShutterAction(hWnd);
                if(g_spv_camera_state.mode != MODE_VIDEO || g_spv_camera_state.isRecording != TRUE) {
                    ERROR("MSG_SHUTTER_PRESSED, start shutter pressed\n");
                    return ret;
                }
                if(FLAG_POWERED_BY_GSENSOR != wParam)
                    return ret;
                INFO("FLAG_POWERED_BY_GSENSOR detected\n");
                SpvSetCollision(hWnd, g_camera);
                if(!SpvAdapterConnected()) {
                    SetTimerEx(hWnd, TIMER_AUTO_SHUTDOWN, 10, AutoShutdownTimerProc);
                }
                return ret;
            }
        case MSG_SHUTDOWN:
            {
                INFO("MSG_SHUTDOWN: shutdown\n");
                HWND activeWindow = GetActiveWindow();
                if(activeWindow == hWnd) {
                    StopVideoRecord(hWnd);
                } else {
                    SendMessage(activeWindow, MSG_SHUTDOWN, 0, 0);
                }
                UpdateMainWindow(hWnd);
				system("touch /mnt/sd0/sdcard.cfg");
                SaveConfigToDisk();
                ShowInfoDialog(GetActiveWindow(), INFO_DIALOG_SHUTDOWN);
                break;
            }
        case MSG_GET_UI_STATUS:
            return (int)g_status;
        case MSG_IR_LED:
            INFO("MSG_IR_LED: wParam: %d\n", wParam);
            if(g_spv_camera_state.isDaytime == wParam)
                return 0;

            g_spv_camera_state.isDaytime = wParam;//get_ambientBright();
            if(g_spv_camera_state.mode <= MODE_PHOTO && IsMainWindowActive()) {
                MWI_TYPE mwi = (g_spv_camera_state.mode == MODE_VIDEO) ? MWV_LAMP : MWP_LAMP;
                g_items[mwi].visibility = g_spv_camera_state.isDaytime ? FALSE : TRUE;
                /*g_recompute_locations = TRUE;
                InvalidateItemRect(hWnd, mwi, TRUE);
                InvalidateItemRect(hWnd, IC_LOCKED, TRUE);*/
                UpdateMainWindow(hWnd);
            }
            return 0;
        case MSG_COLLISION:
        {
            INFO("MSG_COLLISION\n");
            char value[VALUE_SIZE] = {0};
            GetConfigValue(GETKEY(ID_VIDEO_GSENSOR), value);
            if(!strcasecmp(value, GETVALUE(STRING_OFF))) {
                return 0;
            }

            if(g_spv_camera_state.mode == MODE_VIDEO && g_spv_camera_state.isRecording) {
                SpvSetCollision(hWnd, g_camera);
                return 0;
            }
#if 0
            if(!IsMainWindowActive()) {
                do {
                    INFO("current active window: %d, g_main_window: %d\n", GetActiveWindow(), g_main_window);
                    SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
                } while(!IsMainWindowActive());
            }
            if(g_spv_camera_state.mode == MODE_PHOTO && g_spv_camera_state.isPicturing) {
                KillTimer(hWnd, TIMER_COUNTDOWN);
                g_status->idle = 1;
                gettimeofday(&(g_status->idleTime), 0);
                g_spv_camera_state.isPicturing = FALSE;
                g_items[MWP_TIMER].style = MW_STYLE_ICON;
                g_items[MWP_MODE].iconId = IC_PHOTO;
            }
            if(g_spv_camera_state.mode != MODE_VIDEO) {
                ChangeToMode(hWnd, MODE_VIDEO);
            }
#else
            if(!IsMainWindowActive() || g_spv_camera_state.mode != MODE_VIDEO)
                return 0;
#endif
            DoShutterAction(hWnd);
            if(g_spv_camera_state.mode == MODE_VIDEO && g_spv_camera_state.isRecording) {
                SpvPlayAudio(SPV_WAV[WAV_KEY]);
                SpvSetCollision(hWnd, g_camera);
            }
            return 0;
        }
        case MSG_REAR_CAMERA:
#ifdef GUI_CAMERA_REAR_ENABLE
            INFO("MSG_REAR_CAMERA, device inserted:%d\n", wParam);
            UpdateRearCameraStatus(wParam);
            if(IsMainWindowActive()) {
                SetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_ON), FROM_USER);
                if(!SetCameraConfig(TRUE))
                    g_camera->start_preview();
                if(g_spv_camera_state.isRecording && wParam) {
                    g_camera->start_video();
                }
            } else {
                PostMessage(GetActiveWindow(), MSG_REAR_CAMERA, wParam, lParam);
            }
#endif
            break;
        case MSG_REVERSE_IMAGE:
        {
#ifdef GUI_CAMERA_REAR_ENABLE
            INFO("MSG_REVERSE_IMAGE, wParam: %d\n", wParam);
            if(g_spv_camera_state.reverseImage == (BOOL)wParam) 
                return 0;

            REVERSE_IMAGE = g_spv_camera_state.reverseImage = (BOOL)wParam;

            if(wParam == 1) {
                if(!IsMainWindowActive()) {
                    INFO("close the current non-main active window: \n");
                    do {
                        INFO("\tcurrent active window: %d, g_main_window: %d\n", GetActiveWindow(), g_main_window);
                        SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
                    } while(!IsMainWindowActive());
                }
            }

            if(IsMainWindowActive()) {
				g_camera->start_preview();
                UpdateMainWindow(hWnd);
            }
#endif
            return 0;
        }
        case MSG_ACC_CONNECT:
        {
            INFO("MSG_ACC_CONNECT, wParam: %d\n", wParam);
            SpvDelayedShutdown(wParam ? -1 : 8000);
            break;
        }
#ifdef GUI_ADAS_ENABLE
#if 0
        case MSG_LDWS:
            gettimeofday(&g_spv_camera_state.ldTime, NULL);
            memcpy(&(g_spv_camera_state.ldwsResult), (ADAS_LDWS_RESULT *)lParam, sizeof(ADAS_LDWS_RESULT));
            InvalidateRect(hWnd, 0, TRUE);
            break;
        case MSG_FCWS:
            gettimeofday(&g_spv_camera_state.fcTime, NULL);
            memcpy(&(g_spv_camera_state.fcwsResult), (ADAS_FCWS_RESULT *)lParam, sizeof(ADAS_FCWS_RESULT));
            InvalidateRect(hWnd, 0, TRUE);
            break;
#else
        case MSG_ADAS:
        {
            INFO("MSG_ADAS: lParam: %ld\n", lParam);
            struct timeval ct;
            gettimeofday(&ct, 0);
            static long last_update_time = 0;
            long current = ct.tv_sec * 1000 + ct.tv_usec / 1000;
            int warn = !ParseAdasAndWarning((ADAS_PROC_RESULT*)lParam);
            if(warn || (current - last_update_time) > 300) {
                InvalidateRect(hWnd, 0, TRUE);
                last_update_time = current;
            }
            memcpy(&(g_spv_camera_state.adasResult), (ADAS_PROC_RESULT*)lParam, sizeof(ADAS_PROC_RESULT));
            break;
        }
#endif
#endif
        case MSG_GPS:
        {
#ifdef GUI_GPS_ENABLE
            INFO("MSG_GPS: lParam: %ld, g_sync: %d\n", lParam, g_sync_gps_time);
            gps_data *gd = (gps_data *) lParam;
            if(lParam == 0)
                return;

            GPS_Data data;
            data.latitude = gd->latitude;
            data.longitude = gd->longitude;
            data.speed = gd->speed * 1.852f;
            if(data.latitude == 0.0f || data.longitude == 0.0f) {
                data.latitude = 0xFF;
                data.longitude = 0xFF;
                data.speed = 0xFF;
            }
            SpvUpdateGpsData(&data);
            memcpy(&(g_spv_camera_state.gpsData), &data, sizeof(GPS_Data));

            static BOOL time_updated = FALSE;
            if(!g_spv_camera_state.isRecording && g_sync_gps_time && !time_updated) {
                if(!SetTimeByData(gd->gut))
                    time_updated = TRUE;
            }
            free(gd);
#endif
            break;
        }
        case MSG_UPDATE_WINDOW:
            UpdateMainWindow(hWnd);
            break;
        case MSG_CAMERA_CALLBACK:
            INFO("MSG_CAMERA_CALLBACK, wParam: %d, isRecording: %d\n", wParam, g_spv_camera_state.isRecording);
            switch(wParam) {
                case STATUS_SD_FULL:
                    if(g_spv_camera_state.isRecording && g_spv_camera_state.mode == MODE_VIDEO) {
                        StopVideoRecord(hWnd);
                        UpdateMainWindow(hWnd);
                    }
                    ShowToast(hWnd, TOAST_SD_FULL, 160);
                    break;
                case STATUS_NO_SD:
                    if(g_spv_camera_state.isRecording && g_spv_camera_state.mode == MODE_VIDEO) {
                        /*g_status->idle = 1;
                        gettimeofday(&(g_status->idleTime), 0);
                        g_spv_camera_state.isRecording = FALSE;
                        g_spv_camera_state.isLocked = FALSE;
                        g_camera->stop_video();
                        g_spv_camera_state.timeLapse = 0;
                        KillTimer(hWnd, TIMER_CLOCK);*/
                        //DoShutterAction(hWnd);
                        StopVideoRecord(hWnd);
                        UpdateMainWindow(hWnd);
                    }
                    //ShowToast(hWnd, TOAST_SD_NO, 160);
                    ShowSDErrorToast(hWnd);
                    break;
                case STATUS_ERROR:
                    if(g_spv_camera_state.isRecording && g_spv_camera_state.mode == MODE_VIDEO) {
                        StopVideoRecord(hWnd);
                        UpdateMainWindow(hWnd);
                    }
                    ShowToast(hWnd, TOAST_CAMERA_BUSY, 160);
                    break;
            }
            break;
        case MSG_CLEAN_SECONDARY_DC:
            if(IsMainWindowActive()) {
                g_force_cleanup_secondarydc = TRUE;
                InvalidateRect(hWnd, 0, TRUE);
            }
            break;
        case MSG_TIMER:
            switch(wParam)
            {
                case TIMER_CLOCK:
                    DealClockTimer(hWnd);
                    break;
                case TIMER_COUNTDOWN:
                    DealCountdownTimer(hWnd);
                    break;
                case TIMER_DATETIME:
                    if(g_spv_camera_state.mode != MODE_VIDEO || !IsMainWindowActive())
                        break;
                    char timestring[32] = {0};
                    GetDateTimeString(timestring);
                    if(strcasecmp(timestring, g_items[MWV_DATETIME].text)) {
                        strcpy(g_items[MWV_DATETIME].text, timestring);
                        InvalidateItemRect(hWnd, MWV_DATETIME, TRUE);
                    }
                    break;
                /*case TIMER_TIRED_ALARM:
                    {
                        static int count = 1;
                        if(count < 2)
                            count ++;
                        if(count == 2)
                            ResetTimer(hWnd, TIMER_TIRED_ALARM, 25 * 60 * 100);//8 hours later, alarm every 20 minutes
                        INFO("TIMER_TIRED_ALARM\n");
                        SpvTiredAlarm(SPV_WAV[WAV_KEY]);
                        break;
                    }*/
                default:
                    break;
            }
            break;
        case MSG_PAINT:
            hdc = BeginSpvPaint(hWnd);
            if(g_spv_camera_state.mode > MODE_PHOTO) {
                //DrawPlaybackOrSetup(hWnd, hdc);
            } else {
                DrawMainWindow(hWnd, hdc);
            }
            EndPaint(hWnd, hdc);

            if(g_force_cleanup_secondarydc) {
                //INFO("msg_paint, g_force_cleanup_secondarydc\n");
                CleanupSecondaryDC(hWnd);
                g_force_cleanup_secondarydc = FALSE;
            }
            return 0;
        case MSG_KEYUP:
            OnKeyUp(hWnd, LOWORD(wParam), lParam);
            break;
        case MSG_CLOSE:
            INFO("MSG_CLOSE, spv_main!\n");
            DestroyWindow(hWnd);
            DestroyLogFont(g_logfont_info);
            DestroyLogFont(g_logfont_mode);
            PostQuitMessage(hWnd);
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void ShutdownProcess()
{
    static BOOL shitdown;
    if(shitdown)
        return;
    INFO("[UI]device goint to SHUTDOWN!!!\n");
    SendNotifyMessage(g_main_window, MSG_SHUTDOWN, 0, 0);
    shitdown = TRUE;
}

int key_press_hook(void *context, HWND dstWnd, int message, WPARAM wParam, LPARAM lParam)
{
    gettimeofday(&(g_status->idleTime), 0);
    switch(message) {
        case MSG_KEYDOWN:
            if(IsTimerInstalled(g_main_window, TIMER_AUTO_SHUTDOWN)) {
                KillTimer(g_main_window, TIMER_AUTO_SHUTDOWN);
            }
#ifdef UBUNTU
            if(wParam == 30) {//for debug 'a'
                SendNotifyMessage(g_main_window, MSG_COLLISION, 0, 0);
                return;
            } else if(wParam == 48) { //'b'
                SendNotifyMessage(g_main_window, MSG_BATTERY_STATUS, 0, 0);
                return;
            } else if(wParam == 23) { //'i'
                CameraInfoCallback(STATUS_SD_FULL);
                return;
            } else if(wParam == 18) {
                SendNotifyMessage(GetActiveWindow(), MSG_USER + 202, 0, 0);
                return;
            } else if(wParam == 47) {//'v'
                SendNotifyMessage(g_main_window, MSG_REAR_CAMERA, 1, 0);
                return;
            } else if(wParam == 46) {//'c'
                static BOOL v = FALSE;
                v = !v;
                SendNotifyMessage(g_main_window, MSG_REVERSE_IMAGE, v, 0);
                return;
            }
#endif
            if(!g_spv_camera_state.isKeyTone || g_fucking_volume == 0)
                return;
#if 0
            if((g_spv_camera_state.mode == MODE_VIDEO) && g_spv_camera_state.isRecording){//mute in recording
                return;
            } else if (g_spv_camera_state.mode == MODE_PHOTO && IsMainWindowActive() && SCANCODE_SPV_OK == LOWORD(wParam)) {
                char value[VALUE_SIZE] = {0};
                GetConfigValue(GETKEY(ID_PHOTO_CAPTURE_MODE), value);
                if(!atoi(value)) {//mute in capture photo
                    return;
                }
            }
#endif
            SpvPlayAudio(SPV_WAV[WAV_KEY]);
            break;
        case MSG_KEYUP:
            if(SCANCODE_SPV_SHUTDOWN == LOWORD(wParam)) {
                ShutdownProcess();
#ifdef DEBUG_WIFI_AP_STA
            } else if(wParam == SCANCODE_SPV_UP) {
                static int count = 0;
                char *key[1];
                char *value[1];
                key[0] = "action.wifi.set";
                if(count % 3 == 0) {
                    value[0] = "off";
                } else if(count % 3 == 1) {
                    value[0] = "ap;;";
                } else if(count % 3 == 2) {
                    value[0] = "sta;;Test_Test;;12348765;;WPA-PSK";
                }
                send_message_to_gui(MSG_DO_ACTION, key, value, 1);
                count ++;
                /*
                static int count = 0;
                PWifiOpt pWifi = malloc(sizeof(WifiOpt));
                memset(pWifi, 0, sizeof(WifiOpt));
                if(count % 3 == 0) {
                    pWifi->operation = DISABLE_WIFI;
                } else if(count % 3 == 1) {
                    pWifi->operation = ENABLE_AP;
                } else if(count % 3 == 2) {
                    pWifi->operation = ENABLE_STA;
                    strcpy(pWifi->wifiInfo[0], "Test_Test");
                    strcpy(pWifi->wifiInfo[1], "12348765");
                    strcpy(pWifi->wifiInfo[2], "WPA-PSK");

                }
                count++;
                SpvSetWifiStatus(pWifi);*/
#endif
            }
            break;
        default:
            break;
    }
    return HOOK_GOON;
}

static BOOL IsMainWindowActive()
{
    return g_main_window == GetActiveWindow();
}

BOOL IsKeyBelongsCurrentMode(const char *key, SPV_MODE mode)
{
    if(key == NULL)
        return FALSE;

    switch(mode) {
        case MODE_VIDEO:
            if(!strncasecmp("video.", key, 6))
                return TRUE;
            break;
        case MODE_PHOTO:
            if(!strncasecmp("photo.", key, 6))
                return TRUE;
            break;
        case MODE_SETUP:
            if(!strncasecmp("setup.", key, 6))
                return TRUE;
            break;
    }

    if(strstr(key, "irled") || strstr(key, "imagerotation"))
        return TRUE;

    return FALSE;
}

int take_photo_with_name(char *filename)
{
    if(g_camera == NULL)
        return -1;

    if(filename == NULL || strlen(filename) <= 0) {
        g_camera->start_photo();
    } else {
        g_camera->start_photo_with_name(filename);
    }

    stop_photo();

    return 0;
}

int HandleConfigChangedByHttp(int keyId, int valueId)
{
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(keyId), value);
    if(!strcasecmp(value, GETVALUE(valueId))) {
        return 0;
    }
    int recording = g_spv_camera_state.isRecording;
    if(recording) {
        StopVideoRecord(g_main_window);
    } 
    SetConfigValue(GETKEY(keyId), GETVALUE(valueId), FROM_CLIENT);
    //SetCameraConfig(g_camera);
    if(recording) {
        if(keyId == ID_VIDEO_LOOP_RECORDING || keyId == ID_VIDEO_RESOLUTION)
        usleep(1000 * 1000);
        StartVideoRecord(g_main_window);
    }
    UpdateMainWindow(g_main_window);

    return 0;
}

int HandleConfigChangedByClient(char *keys[], char *values[], int argc)
{
    int i = 0;
    char existValue[VALUE_SIZE] = {0};
    BOOL reConfigCamera = FALSE;
    BOOL updateWindow = FALSE;
    BOOL mainActive = IsMainWindowActive();

    for(i = 0; i < argc; i ++)
    {
        if(keys[i] == NULL || strlen(keys[i]) <= 0 || values[i] == NULL || strlen(values[i]) <= 0)
            return ERROR_INVALID_ARGS;

        GetConfigValue(keys[i], existValue);
        if(!strcasecmp(existValue, values[i]))
        {
            INFO("same value, key: %s, value:%s\n", keys[i], values[i]);
            continue;
        }


        if(!strcasecmp(keys[i], KEY_CURRENT_MODE)) {
            if(g_spv_camera_state.mode > MODE_PHOTO)
                return ERROR_INVALID_STATUS;

            SetConfigValue(keys[i], values[i], FROM_CLIENT);
            SendNotifyMessage(g_main_window, MSG_CHANGE_MODE, GetCurrentMode(), 0);
            if(!mainActive) {
                SendNotifyMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
            }
            return 0;
        }

        SetConfigValue(keys[i], values[i], FROM_CLIENT);

        if(IsKeyBelongsCurrentMode(keys[i], g_spv_camera_state.mode))
        {
            if(mainActive && g_spv_camera_state.mode < MODE_PLAYBACK)
            {
                reConfigCamera = TRUE;
                SendNotifyMessage(g_main_window, MSG_UPDATE_WINDOW, 0, 0);
            } else if (!mainActive) {
                UpdateSetupDialogItemValue(keys[i], values[i]);
                updateWindow = TRUE;
            }
        }
    }

    if(updateWindow) 
    {
        UpdateWindow(GetActiveWindow(), TRUE);
    }

    if(reConfigCamera)
    {
        SetCameraConfig(TRUE);
    }
    return 0;
}

/**
 * Called by socket thread, key / value MUST be correct, verified in the socket thread!!
 **/
int send_message_to_gui(MSG_TYPE msg, char *keys[], char *values[], int argc)
{
    INFO("MainWindow, got message from socket,argc:%d\n", argc);
    if(msg == MSG_PIN_SUCCEED) {
        SendMessage(GetActiveWindow(), MSG_PAIRING_SUCCEED, 0, 0);
        return 0;
    }

    if(argc <= 0)
        return ERROR_INVALID_ARGS;

    if(msg == MSG_DO_ACTION) {
        if(!strcasecmp(keys[0], g_action_keys[ACTION_SHUTTER_PRESS])) {//shutter action
            if(!strcasecmp(values[0], "photo")) {
                int ret = g_camera->start_photo();
                if(!ret) {
                    g_camera->stop_photo();
                    //UpdateCapacityStatus();
                    UpdatePhotoCount(g_main_window, g_spv_camera_state.photoCount + 1);
                    GoingToScanMedias(g_main_window, 200);
                }
                return ret;
            }

            if(g_spv_camera_state.mode > MODE_PHOTO)
            {// In playback or setup, discard the mode value
                return ERROR_INVALID_STATUS;
            }

            if(!IsMainWindowActive())
            {
                SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
            }
            return SendMessage(g_main_window, MSG_SHUTTER_PRESSED, 0, 0);
        } else if(!strcasecmp(keys[0], g_action_keys[ACTION_FILE_DELETE])) {
            UpdateCapacityStatus();
            GoingToScanMedias(g_main_window, 150);
        } else if(!strcasecmp(keys[0], g_action_keys[ACTION_WIFI_SET])) {
            INFO("Got Message: %s:%s\n", keys[0], values[0]);
            PWifiOpt pWifi = (PWifiOpt) malloc(sizeof(WifiOpt));
            if(pWifi == NULL) {
                ERROR("malloc for PWifiOpt failed\n");
                return -1;
            }
            memset(pWifi, 0, sizeof(WifiOpt));
            char *valueString = values[0];
            char opt[128] = {0};
            char *tmpBegin = NULL;
            char *tmpEnd = NULL;
            tmpBegin = strstr(valueString, ";;");
            if(tmpBegin == NULL) {
                if(!strncasecmp(valueString, "off", 3)) {
                    pWifi->operation = DISABLE_WIFI;
                    SpvSetWifiStatus(pWifi);
                    return 0;
                } else if(!strncasecmp(valueString, "ap", 2)) {
                    pWifi->operation = ENABLE_AP;
                    SpvSetWifiStatus(pWifi);
                    return 0;
                }
                ERROR("invalide values, %s\n", values[0]);
                return -1;
           }
            strncpy(opt, valueString, tmpBegin - valueString);
            if(!strncasecmp(opt, "ap", 2)) {
                pWifi->operation = ENABLE_AP;
            } else if(!strncasecmp(opt, "sta", 3)) {
                pWifi->operation = ENABLE_STA;
            } else if(!strncasecmp(opt, "off", 3)) {
                pWifi->operation = DISABLE_WIFI;
                SpvSetWifiStatus(pWifi);
                return 0;
            } else {
                ERROR("invalide values, %s\n", values[0]);
                return -1;
            }

            int i = 0;
            while (i < INFO_WIFI_COUNT) {
                tmpBegin += 2;
                tmpEnd = strstr(tmpBegin, ";;");
                if(tmpEnd == NULL) {//the end
                    INFO("i: %d, info: %s\n", i, pWifi->wifiInfo[i]);
                    strcpy(pWifi->wifiInfo[i], tmpBegin);
                    break;
                }
                strncpy(pWifi->wifiInfo[i], tmpBegin, tmpEnd - tmpBegin);
                INFO("i: %d, info: %s\n", i, pWifi->wifiInfo[i]);
                tmpBegin = tmpEnd;
                i++;
            }
            SpvSetWifiStatus(pWifi);
            return 0;
        }
        return 0;
    }

    int ret = HandleConfigChangedByClient(keys, values, argc);
    if(!ret) {
        notify_app_value_updated(keys, values, argc, FROM_CLIENT);
    }
    return ret;

}

HDC BeginSpvPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    if(SCALE_FACTOR == 1) {
        SetMapMode(hdc, MM_TEXT);
    } else {
        SetMapMode(hdc, MM_ANISOTROPIC);
        POINT p;
        p.x = VIEWPORT_WIDTH; 
        p.y = VIEWPORT_HEIGHT; 
        SetWindowExt(hdc, &p); 
        p.x = DEVICE_WIDTH; 
        p.y = DEVICE_HEIGHT; 
        SetViewportExt(hdc, &p); 
    }

    return hdc;
}

void CleanupSecondaryDC(HWND hWnd)
{
    if(GetActiveWindow() != hWnd)
        return;
#if !COLORKEY_ENABLED
    HDC hdc;
    hdc = GetSecondaryDC(hWnd);
    if(hdc == 0)
        return;
    SetMemDCAlpha (hdc, 0, 0);
    BitBlt(hdc, 0, 0, DEVICE_WIDTH, DEVICE_HEIGHT, HDC_SCREEN, 0, 0, 0);
#endif
}

void SetDelayedShutdwonStatus()
{
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(ID_SETUP_DELAYED_SHUTDOWN), value);
    if(atoi(value)) {
        g_status->delayedShutdown = atoi(value);
    } else {
        g_status->delayedShutdown = -1;
    }
}

void SetAutoPowerOffStatus()
{
    char value[VALUE_SIZE] = {0};
    //GetConfigValue(GETKEY(ID_SETUP_AUTO_POWER_OFF), value);
    if(atoi(value)) {
        g_status->autoPowerOff = atoi(value) * 60;
    } else {
        g_status->autoPowerOff = -1;
    }
}

void SetDisplaySleepStatus()
{
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(ID_DISPLAY_SLEEP), value);
    if(atoi(value)) {
        g_status->displaySleep = atoi(value) * 60;
    } else {
        g_status->displaySleep = -1;
    }
}

/**
 * request the libgui light the lcd
 **/
void LightLcdStatus()
{
    g_status->iWantLightLcd = 1;
    gettimeofday(&g_status->idleTime, 0);
}

/**
 * In recording/playing, we set the system to highload status
 **/
void SetHighLoadStatus(int highLoad)
{
    g_status->highLoad = highLoad;
}

void SetIrLed()
{
    SpvSetIRLED(g_spv_camera_state.mode < MODE_PLAYBACK ? 0 : 1, g_spv_camera_state.isDaytime);
}

void SetKeyTone()
{
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(ID_SETUP_KEY_TONE), value);
    g_spv_camera_state.isKeyTone = strcasecmp(value, GETVALUE(STRING_OFF));
}


int sd_format(void)
{
	INFO("[LIBRA_MAIN] %s\n", __func__);

	FILE *stream;
	char buf[1024];

	memset(buf, 0, sizeof(buf));

	if (IsSdcardMounted()) {
		INFO("[LIBRA_MAIN] sd mounted\n");
		stream = popen("/usr/bin/sd_format.sh", "r");
		if(NULL == stream) {
			ERROR("popen file failed\n");
			return SD_FORMAT_ERROR;
		} else {
			while(NULL != fgets(buf, sizeof(buf), stream)) {
				ERROR("sd_format failed, buf is %s\n", buf);
				pclose(stream);
				return SD_FORMAT_ERROR;
			}
		}

		pclose(stream);

		return SD_FORMAT_OK;
	}
	else
	{
		return SD_FORMAT_ERROR;
	}
}

int IsSdcardMounted()
{
    return g_spv_camera_state.sdStatus == SD_STATUS_MOUNTED;
}

int GetSdcardStatus()
{
    return g_spv_camera_state.sdStatus;
}

int GetPhotoCount()
{
	return g_spv_camera_state.photoCount;
}

void SetIdleTime()
{
    gettimeofday(&(g_status->idleTime), 0);
}

HWND GetMainWindow()
{
    //INFO("get main window\n");
    return g_main_window;
}

static void InitSPV() {
    struct timeval t1, t2;
    gettimeofday(&t1, 0);
    char value[VALUE_SIZE] = {0};

    g_spv_camera_state.sdStatus = SpvGetSdcardStatus();
    INFO("sd mounted status: %d\n", g_spv_camera_state.sdStatus);
    char *configPath = NULL;
#if SYSTEM_IS_READ_ONLY
    INFO("Is read-only system\n");
    configPath = RESOURCE_PATH"./config/libramain.config";
    if(IsSdcardMounted()) {
        if(access(EXTERNAL_CONFIG, F_OK)) {//config file not exist, copy it
            if(!MakeExternalDir()) {
                INFO("re copy default file to sd\n");
                copy_file(RESOURCE_PATH"./config/libramain.config.bak", EXTERNAL_CONFIG, TRUE);
                configPath = EXTERNAL_CONFIG;
            } else {
                INFO("[ERROR]: mkdir %s failed\n", EXTERNAL_MEDIA_DIR);
            }
        } else {
            INFO("sdcard external config found!!!\n");
            configPath = EXTERNAL_CONFIG;
        }
    }
#else
    configPath = CONFIG_PATH"./libramain.config";
    if(access(CONFIG_PATH"./libramain.config", F_OK)) {
        int ret = copy_file(RESOURCE_PATH"./config/libramain.config.bak", CONFIG_PATH"./libramain.config", TRUE);
        INFO("config file not exist, copy it, ret: %d\n", ret);
        if(ret) {
            ERROR("Can not get config file:%s, use the default!!!\n", CONFIG_PATH"./libramain.config");
            configPath = RESOURCE_PATH"./config/libramain.config";
        }
    }
#endif
    InitConifgManager(configPath);

    g_spv_camera_state.mode = MODE_VIDEO;
    SetConfigValue(KEY_CURRENT_MODE, g_mode_name[MODE_VIDEO], FROM_USER);

#ifdef GUI_SET_RESOLUTION_SEPARATELY
    //SpvSetResolution();
#endif

#ifdef GUI_CAMERA_REAR_ENABLE
    UpdateRearCameraStatus(rear_camera_inserted());
#ifndef GUI_CAMERA_PIP_ENABLE
    SetConfigValue(GETKEY(ID_VIDEO_PIP), "Off", FROM_USER);
#endif
#endif

    SetConfigValue(GETKEY(ID_VIDEO_ZOOM), "x1.0", FROM_USER);
    SetConfigValue(GETKEY(ID_PHOTO_ZOOM), "x1.0", FROM_USER);
    //init status
    UpdateBatteryStatus();

#ifdef GUI_WIFI_ENABLE
#ifdef GUI_SPORTDV_ENABLE
    SetConfigValue(GETKEY(ID_SETUP_WIFI), GETVALUE(STRING_OFF), FROM_USER);
#endif
    //wifi status
    GetConfigValue(GETKEY(ID_SETUP_WIFI), value);
    PWifiOpt pWifi = (PWifiOpt) malloc(sizeof(WifiOpt));
    if(pWifi != NULL) {
        memset(pWifi, 0, sizeof(WifiOpt));
        if(!strcasecmp(GETVALUE(STRING_ON), value)) {
            pWifi->operation = ENABLE_AP;
        } else {
            pWifi->operation = DISABLE_WIFI;
        }
        GetWifiApInfo(pWifi->wifiInfo[INFO_SSID], pWifi->wifiInfo[INFO_PSWD]);
        SpvSetWifiStatus(pWifi);
    } else {
        ERROR("malloc pWifi failed\n");
    }

    //g_spv_camera_state.wifiStatus = is_softap_started();
#ifdef GUI_WIFI_ENABLE
	SpvStartThttpd();
#endif

#endif

    strcpy(g_spv_camera_state.locating, "Off");
    UpdateCapacityStatus();
    UpdateWorkingStatus();

    ScanSdcardMediaFiles(IsSdcardMounted());

    SpvSetBacklight();

    SpvSetParkingGuard();

    GetConfigValue(GETKEY(ID_SETUP_BEEP_SOUND), value);
    set_volume(atoi(value) * CODEC_VOLUME_MAX / 100);
    g_fucking_volume = atoi(value);

    SetKeyTone();

    g_spv_camera_state.isDaytime = get_ambientBright();
    SetIrLed();

    memset(g_status, 0, sizeof(UIStatus));
    g_status->idle = 1;
    gettimeofday(&(g_status->idleTime), 0);

    SetDelayedShutdwonStatus();
    SetAutoPowerOffStatus();
    SetDisplaySleepStatus();
   
    //init language
    SetLanguageConfig(GetLanguageType(), FROM_OTHER);

    gettimeofday(&t2, 0);
    INFO("InitSPV end, time lapse:%ldms\n", (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000);
}

void ResetCamera()
{
    DeinitConfigManager();
#if SYSTEM_IS_READ_ONLY
    if(IsSdcardMounted()) {
        if(!MakeExternalDir()) {
            copy_file(RESOURCE_PATH"./config/libramain.config.bak", EXTERNAL_CONFIG, TRUE);
        } else {
            ERROR("[ERROR]: dir make failed\n");
        }
    }
#else
    copy_file(RESOURCE_PATH"./config/libramain.config.bak", CONFIG_PATH"./libramain.config", TRUE);
#endif
    InitSPV();
    SetCameraConfig(TRUE);

    if(IsMainWindow(g_main_window))
        UpdateMainWindow(g_main_window);
}

static int CameraInfoCallback(int flag)
{
    if(g_main_window == 0 || g_main_window == HWND_INVALID)
        return -1;

    SendNotifyMessage(g_main_window, MSG_CAMERA_CALLBACK, flag, 0L);
    return 0;
}

static BOOL CameraConfigIsSame(config_camera config1, config_camera config2)
{
    return memcmp(&config1, &config2, sizeof(config_camera)) == 0;
    /*
    if(strcasecmp(config1.current_mode, config2.current_mode))
        return FALSE;
    if(strcasecmp(config1.video_resolution,config2.video_resolution))
        return FALSE;
    if(strcasecmp(config1.video_looprecording,config2.video_looprecording))
        return FALSE;
    if(strcasecmp(config1.video_wdr,config2.video_wdr))
        return FALSE;
    if(strcasecmp(config1.video_ev,config2.video_ev))
        return FALSE;
    if(strcasecmp(config1.video_recordaudio,config2.video_recordaudio))
        return FALSE;
    if(strcasecmp(config1.video_datestamp,config2.video_datestamp))
        return FALSE;
    if(strcasecmp(config1.video_gsensor,config2.video_gsensor))
        return FALSE;
    if(strcasecmp(config1.video_platestamp,config2.video_platestamp))
        return FALSE;
    if(strcasecmp(config1.video_zoom,config2.video_zoom))
        return FALSE;
    if(strcasecmp(config1.photo_capturemode,config2.photo_capturemode))
        return FALSE;
    if(strcasecmp(config1.photo_resolution,config2.photo_resolution))
        return FALSE;
    if(strcasecmp(config1.photo_sequence,config2.photo_sequence))
        return FALSE;
    if(strcasecmp(config1.photo_quality,config2.photo_quality))
        return FALSE;
    if(strcasecmp(config1.photo_sharpness,config2.photo_sharpness))
        return FALSE;
    if(strcasecmp(config1.photo_whitebalance,config2.photo_whitebalance))
        return FALSE;
    if(strcasecmp(config1.photo_color,config2.photo_color))
        return FALSE;
    if(strcasecmp(config1.photo_isolimit,config2.photo_isolimit))
        return FALSE;
    if(strcasecmp(config1.photo_ev,config2.photo_ev))
        return FALSE;
    if(strcasecmp(config1.photo_antishaking,config2.photo_antishaking))
        return FALSE;
    if(strcasecmp(config1.photo_datestamp,config2.photo_datestamp))
        return FALSE;
    if(strcasecmp(config1.photo_zoom,config2.photo_zoom))
        return FALSE;
    if(strcasecmp(config1.setup_tvmode,config2.setup_tvmode))
        return FALSE;
    if(strcasecmp(config1.setup_frequency,config2.setup_frequency))
        return FALSE;
    if(strcasecmp(config1.setup_irled,config2.setup_irled))
        return FALSE;
    if(strcasecmp(config1.setup_imagerotation,config2.setup_imagerotation))
        return FALSE;
    if(strcasecmp(config1.setup_platenumber,config2.setup_platenumber))
        return FALSE;
    if(strcasecmp(config1.other_collide,config2.other_collide))
        return FALSE;

    return TRUE;*/
}

BOOL SetCameraConfig(BOOL directly)
{
    static config_camera sCurrentConfig;
    config_camera config;
    memset(&config, 0, sizeof(config_camera));
    get_config_camera(&config);

    if(CameraConfigIsSame(sCurrentConfig, config)) {
        INFO("config not changed, no need to set\n");
        return FALSE;
    }

    memcpy(&sCurrentConfig, &config, sizeof(config_camera));

    if(!directly) {
        g_camera->stop_preview();
    }

    g_camera->set_config(config);

    if(!directly) {
        g_camera->start_preview();
    }
    return TRUE;
}

static void MeasureWindowSize()
{
    DEVICE_WIDTH = VIEWPORT_WIDTH = g_rcDesktop.right;
    DEVICE_HEIGHT = VIEWPORT_HEIGHT = g_rcDesktop.bottom;

    SCALE_FACTOR = 1;
#if SCALE_ENABLED
    while(VIEWPORT_HEIGHT > 300) {
        SCALE_FACTOR ++;
        VIEWPORT_WIDTH = DEVICE_WIDTH / SCALE_FACTOR;
        VIEWPORT_HEIGHT = DEVICE_HEIGHT / SCALE_FACTOR;
    }
#endif
    LARGE_SCREEN = VIEWPORT_HEIGHT >= 400;
    ERROR("window infomation, w: %d, h: %d, vw: %d, vh: %d, sf: %d, large: %d\n", DEVICE_WIDTH, DEVICE_HEIGHT, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, SCALE_FACTOR, LARGE_SCREEN);
    // main window
    MW_BAR_HEIGHT = 40;
    MW_ICON_WIDTH = 58;
    MW_ICON_HEIGHT = 36;
    MW_SMALL_ICON_WIDTH = MW_ICON_HEIGHT;
    MW_ICON_PADDING = ((MW_BAR_HEIGHT - MW_ICON_HEIGHT)>>1);
    MW_ITEM_PADDING = 4;

    //setup window 
    SETUP_ITEM_HEIGHT = DEVICE_HEIGHT / (LARGE_SCREEN ? 6 : 4);

    //folder window
    FOLDER_DIALOG_TITLE_HEIGHT = SETUP_ITEM_HEIGHT / SCALE_FACTOR;

    FOLDER_WIDTH = VIEWPORT_WIDTH / FOLDER_COLUMN;
    FOLDER_HEIGHT = (VIEWPORT_HEIGHT - FOLDER_DIALOG_TITLE_HEIGHT) / FOLDER_ROW; 
    FOLDER_INFO_HEIGHT = LARGE_SCREEN ? 40 : 25;

    //playback window
    PB_BAR_HEIGHT = (MW_BAR_HEIGHT + 4);
    PB_PROGRESS_HEIGHT = 6;

    PB_ICON_WIDTH = MW_ICON_WIDTH;
    PB_ICON_HEIGHT = MW_ICON_HEIGHT;
    PB_ICON_PADDING = ((PB_BAR_HEIGHT - PB_ICON_HEIGHT) >> 2);

    PB_FOCUS_WIDTH = (PB_ICON_WIDTH - 12);
    PB_FOCUS_HEIGHT = PB_PROGRESS_HEIGHT;
}

static void InitSystemTime()
{
    //setup the time
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
    if((1900+timeinfo->tm_year < 2016) || (1900+timeinfo->tm_year > 2025)) {
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        gps_utc_time time = {2016, 1, 1, 0, 0, 0};
        SetTimeByData(time);
    }
}

void event_handler(char *name, void *args)
{
	INFO("enter func %s\n", __func__);
	if ((NULL == name) || (NULL == args)) {
		INFO("parameter is NULL\n");
		return;
	}

	if (!strcmp(name, EVENT_MOUNT)) {
		g_spv_camera_state.sdStatus = 1;
		if (access(EXTERNAL_MEDIA_DIR"/Photo", 0) == -1)
			system("mkdir -p "EXTERNAL_MEDIA_DIR"/Photo");
		if (access(EXTERNAL_MEDIA_DIR"/Video", 0) == -1)
			system("mkdir -p "EXTERNAL_MEDIA_DIR"/Video");
		INFO("[event] sd mount\n");
	} else if (!strcmp(name, EVENT_UNMOUNT)) {
		g_spv_camera_state.sdStatus = 0;
		DoShutterAction(g_main_window);
		INFO("[event] sd unmount\n");
	} else if (!strcmp(name, EVENT_BATTERY)) {
		INFO("[event] battery\n");
	} else if (!strcmp(name, EVENT_CHARGING)) {
		INFO("[event] charging\n");
	}
	INFO("leave func %s\n", __func__);
}

int MiniGUIMain(int argc, const char* argv[])
{
    MSG msg;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "SportDV GUI", 0, 0);
#endif

    g_icon = (BITMAP *)malloc(sizeof(BITMAP) * IC_MAX);
    memset(g_icon, 0, sizeof(BITMAP) * IC_MAX);
	
    //RegisterSetupButtonControl();
    RegisterSPVStaticControl();
    RegisterKeyMsgHook(0, key_press_hook);

    //init the time
    InitSystemTime();

    //register event handler
    event_register_handler(EVENT_UNMOUNT, 0, event_handler);
    event_register_handler(EVENT_MOUNT, 0, event_handler);
    event_register_handler(EVENT_BATTERY, 0, event_handler);
    event_register_handler(EVENT_CHARGING, 0, event_handler);

    //here we just init the battery once in startup
    g_spv_camera_state.battery = SpvGetBattery();
    g_spv_camera_state.charging = SpvIsCharging();

#ifdef GUI_WIFI_ENABLE
    g_wifi = wifi_create();
    INFO("g_wifi: 0x%x\n", g_wifi);
    //init socket server
    init_socket_server();
#endif

#ifndef GUI_SPORTDV_ENABLE
    //init tired alarm thread
    SpvTiredAlarm(SPV_WAV[WAV_KEY]);
#endif

    g_status = (PUIStatus) malloc(sizeof(UIStatus));
    if(g_status == NULL) {
        ERROR("malloc g_status failed\n");
        exit(1);
        //return;
    }
    memset(g_status, 0, sizeof(UIStatus));

    InitSPV();

#ifdef GUI_GPS_ENABLE
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(ID_TIME_AUTO_SYNC), value);
    if(!strcasecmp(value, GETVALUE(STRING_ON))) {
        g_sync_gps_time = TRUE;
    } else {
        g_sync_gps_time = FALSE;
    }
#endif

    MeasureWindowSize();

    CreateInfo.dwStyle = WS_VISIBLE;// | WS_EX_TRANSPARENT;
#if SECONDARYDC_ENABLED
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT; //WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT;
#else
    CreateInfo.dwExStyle = WS_EX_TRANSPARENT; //WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT;
#endif
    CreateInfo.spCaption = "SportDV GUI";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = 0;
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainWindowProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = DEVICE_WIDTH;
    CreateInfo.by = DEVICE_HEIGHT;
#if COLORKEY_ENABLED || defined(UBUNTU)
    CreateInfo.iBkColor = SPV_COLOR_TRANSPARENT;
#else
    CreateInfo.iBkColor = COLOR_transparent;
#endif
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    config_camera config;
    get_config_camera(&config);
    g_camera = camera_create(config);
    g_camera->init_camera();
    
    g_main_window = CreateMainWindow(&CreateInfo);
    if(g_main_window == HWND_INVALID) {
        ERROR("MiniGUI MAIN window create failed\n");
        return -1;
    }

	printf("start to killall small_ui\n");
	system("killall small_ui");

    ShowWindow(g_main_window, SW_SHOWNORMAL);
    ShowCursor(FALSE);

    register_camera_callback(g_camera, CameraInfoCallback);

#ifndef GUI_SPORTDV_ENABLE
    PostMessage(g_main_window, MSG_SHUTTER_PRESSED, SpvPowerByGsensor() ? FLAG_POWERED_BY_GSENSOR : FLAG_POWERED_BY_OTHER, 1);
#endif

    while(GetMessage(&msg, g_main_window)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MainWindowCleanup(g_main_window);
#ifdef GUI_WIFI_ENABLE
    wifi_destroy();
    g_wifi = NULL;
#endif
    INFO("main window exit!\n");

    return 0;
}
