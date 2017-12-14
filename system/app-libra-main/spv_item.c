#include<stdio.h>
#include<string.h>

#include "spv_common.h"
#include "spv_item.h"
#include "spv_language_res.h"
#include "spv_debug.h"

#ifndef GET_ARRAY_LENGTH
#define GET_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))
#endif

const char *g_mode_name[] = {
    "Video",
    "Photo",
    "Playback",
    "Setup"
};

//const char *g_current_key = "current.mode";

const char *g_item_key[] = {
    "",
    //video
    "",//header
    "video.resolution",
    "video.looprecording",
#ifdef GUI_CAMERA_REAR_ENABLE
    "video.rearcamera",
    "video.pip",
#endif
    "video.wdr",
    "video.ev",
#ifndef GUI_SPORTDV_ENABLE
    "video.recordaudio",
#endif
    "video.datestamp",
#ifndef GUI_SPORTDV_ENABLE
    "video.gsensor",
#ifdef GUI_ADAS_ENABLE
    "video.adas",
    "",
#endif
    "video.platestamp",
#endif
    "",//reset video

    //photo
    "",
    "photo.capturemode",
    "photo.resolution",
    "photo.sequence",
    "photo.quality",
    "photo.sharpness",
    "photo.whitebalance",
    "photo.color",
    "photo.isolimit",
    "photo.ev",
    "photo.antishaking",
    "photo.datestamp",
    "",//reset photo

    //setup
    "",
    "",//video settings
    "",//photo settings
#ifdef GUI_WIFI_ENABLE
    "setup.wifi",
#endif
    "",//date time
    //"setup.autopoweroff",
    "setup.beepsound",
    "setup.keytone",
    "setup.language",
    "setup.delayedshutdown",
    "",//display
#ifndef GUI_SPORTDV_ENABLE
    //"setup.tvmode",
    "setup.parkingguard",
    //"setup.frequency",
    "setup.irled",
    "setup.imagerotation",
#endif
    "",//format
    "",//camera reset
#ifndef GUI_SPORTDV_ENABLE
    "setup.platenumber",
#endif
    "",//version

    //display
    "",
    "setup.display.sleep",
    "setup.display.brightness",

    //date & time
    "",
    "",//date setting
    "",//time setting
#ifdef GUI_GPS_ENABLE
    "setup.time.syncgps",//sync gps time
#endif

    //external status key 
    "video.frontbig",
    "video.zoom",
    "photo.zoom",
    "setup.wifi.ap.ssid",
    "setup.wifi.ap.password",
    "setup.wifi.ap.keymgmt",
    "setup.wifi.sta.ssid",
    "setup.wifi.sta.password",
    "setup.wifi.sta.keymgmt",

#ifdef GUI_SPORTDV_ENABLE
    "video.recordaudio",
    "video.gsensor",
    "video.adas",
    "",
    "video.platestamp",
    "video.parkingguard",
    "setup.irled",
    "setup.imagerotation",

    "setup.platenumber",
#endif

};

DIALOGITEM g_video_items[] = {
    {
        ID_VIDEO_HEADER, ITEM_TYPE_HEADER | ITEM_STATUS_SECONDARY,
        STRING_VIDEO_SETTINGS, STRING_EXIT,
        {0}, 0
    },
#ifndef GUI_SET_RESOLUTION_SEPARATELY
    {
        ID_VIDEO_RESOLUTION, ITEM_TYPE_MORE,
        STRING_RESOLUTION, STRING_1080FHD,
        {STRING_1080FHD, STRING_720P_30FPS, STRING_WVGA, STRING_VGA}, 4 //STRING_720P_60FPS, 
    },
#else
    {
        ID_VIDEO_RESOLUTION, ITEM_TYPE_MORE,
        STRING_RESOLUTION, STRING_1080FHD,
        {STRING_1080FHD, STRING_720P_30FPS}, 2 //STRING_720P_60FPS, , STRING_WVGA, STRING_VGA
    },
#endif
    {
        ID_VIDEO_LOOP_RECORDING, ITEM_TYPE_MORE,
        STRING_LOOP_RECORDING, STRING_1_MINUTE,
        {STRING_OFF, STRING_1_MINUTE, STRING_3_MINUTES, STRING_5_MINUTES}, 4 //, STRING_10_MINUTES
    },
#ifdef GUI_CAMERA_REAR_ENABLE
    {
        ID_VIDEO_REAR_CAMERA, ITEM_TYPE_MORE,
        STRING_REAR_CAMERA, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_VIDEO_PIP, ITEM_TYPE_MORE,
        STRING_PIP, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
    {
        ID_VIDEO_WDR, ITEM_TYPE_MORE,
        STRING_WDR, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_VIDEO_EV, ITEM_TYPE_MORE,
        STRING_EV_COMPENSATION, STRING_ZERO,
        {STRING_POS_2P0, STRING_POS_1P5, STRING_POS_1P0,
            STRING_POS_0P5, STRING_ZERO, STRING_NEG_0P5,
            STRING_NEG_1P0, STRING_NEG_1P5, STRING_NEG_2P0
        }, 9
    },
#ifndef GUI_SPORTDV_ENABLE
    {
        ID_VIDEO_RECORD_AUDIO, ITEM_TYPE_MORE,
        STRING_RECORD_AUDIO, STRING_ON,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
    {
        ID_VIDEO_DATE_STAMP, ITEM_TYPE_MORE,
        STRING_DATE_STAMP, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
#ifndef GUI_SPORTDV_ENABLE
    {
        ID_VIDEO_GSENSOR, ITEM_TYPE_MORE,
        STRING_GSENSOR, STRING_HIGH,
        {STRING_OFF, STRING_LOW, STRING_MEDIUM, STRING_HIGH}, 4
    },
#ifdef GUI_ADAS_ENABLE
    {
        ID_VIDEO_ADAS, ITEM_TYPE_MORE,
        STRING_ADAS, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_VIDEO_ADAS_CALIBRATE, ITEM_TYPE_NORMAL,
        STRING_ADAS_CALIBRATE, 0,
        {0}, 0
    },
#endif
    {
        ID_VIDEO_PLATE_STAMP, ITEM_TYPE_MORE,
        STRING_PLATE_NUMBER_STAMP, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
    {
        ID_VIDEO_RESET, ITEM_TYPE_NORMAL,
        STRING_RESTORE_DEFAULT, 0,
        {0}, 0
    },
};

DIALOGITEM g_photo_items[] = {
    {
        ID_PHOTO_HEADER, ITEM_TYPE_HEADER | ITEM_STATUS_SECONDARY,
        STRING_PHOTO_SETTINGS, STRING_EXIT,
        {0}, 0
    },
    {
        ID_PHOTO_CAPTURE_MODE, ITEM_TYPE_MORE,
        STRING_CAPTURE_MODE, STRING_SINGLE,
        {STRING_SINGLE, STRING_2S_TIMER, STRING_5S_TIMER, STRING_10S_TIMER}, 4
    },
    {
        ID_PHOTO_RESOLUTION, ITEM_TYPE_MORE,
        STRING_RESOLUTION, STRING_8M,
#if 1
        {STRING_2MHD, STRING_VGA, STRING_1P3M}, 3//STRING_12M, STRING_10M, STRING_8M, STRING_5M, STRING_3M, 
#else
        {STRING_2MHD}, 1
#endif
    },
    {
        ID_PHOTO_SEQUENCE, ITEM_TYPE_MORE,
        STRING_SEQUENCE, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_PHOTO_QUALITY, ITEM_TYPE_MORE,
        STRING_QUALITY, STRING_FINE,
        {STRING_FINE, STRING_NORMAL, STRING_ECONOMY}, 3
    },
    {
        ID_PHOTO_SHARPNESS, ITEM_TYPE_MORE,
        STRING_SHARPNESS, STRING_STRONG,
        {STRING_STRONG, STRING_NORMAL, STRING_SOFT}, 3
    },
    {
        ID_PHOTO_WHITE_BALANCE, ITEM_TYPE_MORE,
        STRING_WHITE_BALANCE, STRING_AUTO,
        {STRING_AUTO, STRING_DAYLIGHT, STRING_CLOUDY, STRING_TUNGSTEN, STRING_FLUORESCENT}, 5
    },
    {
        ID_PHOTO_COLOR, ITEM_TYPE_MORE,
        STRING_COLOR_COLON, STRING_COLOR,
        {STRING_COLOR, STRING_BLACK_AND_WHITE, STRING_SEPIA}, 3
    },
    {
        ID_PHOTO_ISO_LIMIT, ITEM_TYPE_MORE,
        STRING_ISO_LIMIT, STRING_AUTO,
        {STRING_AUTO, STRING_800, STRING_400, STRING_200, STRING_100}, 5
    },
    {
        ID_PHOTO_EV, ITEM_TYPE_MORE,
        STRING_EV_COMPENSATION, STRING_ZERO,
        {STRING_POS_2P0, STRING_POS_1P5, STRING_POS_1P0,
            STRING_POS_0P5, STRING_ZERO, STRING_NEG_0P5,
            STRING_NEG_1P0, STRING_NEG_1P5, STRING_NEG_2P0
        }, 9
    },
    {
        ID_PHOTO_ANTI_SHAKING, ITEM_TYPE_MORE,
        STRING_ANTI_SHAKING, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_PHOTO_DATE_STAMP, ITEM_TYPE_MORE,
        STRING_DATE_STAMP, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_PHOTO_RESET, ITEM_TYPE_NORMAL,
        STRING_RESTORE_DEFAULT, 0,
        {0}, 0
    },
};

DIALOGITEM g_setup_items[] = {
    {
        ID_SETUP_HEADER, ITEM_TYPE_HEADER,
        STRING_SETUP, STRING_EXIT,
        {0}, 0
    },
    {
        ID_SETUP_VIDEO, ITEM_TYPE_NORMAL,
        STRING_VIDEO_SETTINGS, 0,
        {0}, 0
    },
    {
        ID_SETUP_PHOTO, ITEM_TYPE_NORMAL,
        STRING_PHOTO_SETTINGS, 0,
        {0}, 0
    },
#ifdef GUI_WIFI_ENABLE
    {
        ID_SETUP_WIFI, ITEM_TYPE_MORE,
        STRING_WIFI, STRING_ON,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
    {
        ID_SETUP_DATE_TIME, ITEM_TYPE_NORMAL,
        STRING_DATE_TIME, 0,
        {0}, 0
    },
    /*{
        ID_SETUP_AUTO_POWER_OFF, ITEM_TYPE_MORE,
        STRING_AUTO_POWER_OFF, STRING_OFF,
        {STRING_OFF, STRING_3_MINUTES, STRING_5_MINUTES, STRING_10_MINUTES}, 4
    },*/
    {
        ID_SETUP_BEEP_SOUND, ITEM_TYPE_MORE,
        STRING_VOLUME, STRING_100P,
        {STRING_OFF, STRING_10P, STRING_20P, STRING_30P, STRING_40P, STRING_50P, STRING_60P, STRING_70P, STRING_80P, STRING_90P, STRING_100P}, 11
    },
    {
        ID_SETUP_KEY_TONE, ITEM_TYPE_MORE,
        STRING_KEY_TONE, STRING_ON,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_SETUP_LANGUAGE, ITEM_TYPE_MORE,
        STRING_LANGUAGE, STRING_ENGLISH,
        {STRING_ENGLISH, STRING_CHINESE, STRING_CHINESE_TRADITIONAL, STRING_FRENCH, STRING_ESPANISH, STRING_ITALIAN, STRING_PORTUGUESE, STRING_GERMAN, STRING_RUSSIAN}, 9
    },
    {
        ID_SETUP_DELAYED_SHUTDOWN, ITEM_TYPE_MORE,
        STRING_DELAYED_SHUTDOWN, STRING_10_SECONDS,
        {STRING_OFF, STRING_10_SECONDS, STRING_20_SECONDS, STRING_30_SECONDS}, 4
    },
    {
        ID_SETUP_DISPLAY, ITEM_TYPE_NORMAL,
        STRING_DISPLAY, 0,
        {0}, 0
    },
#ifndef GUI_SPORTDV_ENABLE
    /*{
        ID_SETUP_TV_MODE, ITEM_TYPE_MORE,
        STRING_TV_MODE, STRING_PAL,
        {STRING_NTSC, STRING_PAL}, 2
    },*/
    {
        ID_SETUP_PARKING_GUARD, ITEM_TYPE_MORE,
        STRING_PARKING_GUARD, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    /*{
        ID_SETUP_FREQUENCY, ITEM_TYPE_MORE,
        STRING_FREQUENCY, STRING_50HZ,
        {STRING_60HZ, STRING_50HZ}, 2
    },*/
    {
        ID_SETUP_IR_LED, ITEM_TYPE_MORE,
        STRING_IR_LED, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
    {
        ID_SETUP_IMAGE_ROTATION, ITEM_TYPE_MORE,
        STRING_IMAGE_ROTATION, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
    {
        ID_SETUP_FORMAT, ITEM_TYPE_NORMAL,
        STRING_FORMAT, 0,
        {0}, 0
    },
    {
        ID_SETUP_CAMERA_RESET, ITEM_TYPE_NORMAL,
        STRING_CAMERA_RESET, 0,
        {0}, 0
    },
#ifndef GUI_SPORTDV_ENABLE
    {
        ID_SETUP_PLATE_NUMBER, ITEM_TYPE_NORMAL,
        STRING_PLATE_NUMBER_COLON, 0,
        {0}, 0
    },
#endif
    {
        ID_SETUP_VERSION, ITEM_TYPE_NORMAL,
        STRING_VERSION, 0,
        {0},0
    }
};

DIALOGITEM g_display_items[] = {
    {
        ID_DISPLAY_HEADER, ITEM_TYPE_HEADER | ITEM_STATUS_SECONDARY,
        STRING_DISPLAY, STRING_BACK,
        {0}, 0
    },
    {
        ID_DISPLAY_SLEEP, ITEM_TYPE_MORE,
        STRING_SLEEP, STRING_NEVER,
        {STRING_3_MINUTES, STRING_5_MINUTES, STRING_10_MINUTES, STRING_OFF}, 4
    },
    {
        ID_DISPLAY_BRIGHTNESS, ITEM_TYPE_MORE,
        STRING_BRIGHTNESS, STRING_HIGH,
        {STRING_HIGH, STRING_MEDIUM, STRING_LOW}, 3
    }
};

DIALOGITEM g_date_time_items[] = {
    {
        ID_DATE_TIME_HEADER, ITEM_TYPE_HEADER | ITEM_STATUS_SECONDARY,
        STRING_DATE_TIME, STRING_BACK,
        {0}, 0
    },
    {
        ID_DATE, ITEM_TYPE_NORMAL,
        STRING_DATE_SETUP, 0,
        {0}, 0
    },
    {
        ID_TIME, ITEM_TYPE_NORMAL,
        STRING_TIME_SETUP, 0,
        {0}, 0
    },
#ifdef GUI_GPS_ENABLE
    {
        ID_TIME_AUTO_SYNC, ITEM_TYPE_MORE,
        STRING_TIME_AUTO_SYNC, STRING_OFF,
        {STRING_ON, STRING_OFF}, 2
    },
#endif
};

int GetKeyId(const char *key)
{
    int length = sizeof(g_item_key) / sizeof(char *);
    int keyId = -1, i = 0;

    if(key == NULL || strlen(key) <= 0)
        return keyId;

    for(i = 1; i < length; i ++)
    {
        if(!strcasecmp(g_item_key[i], key))
            return i - 1;
    }

    return keyId;
}

void UpdateSetupDialogItemValue(const char *key, const char *value)
{
    int keyId = GetKeyId(key);

    if(keyId < 0)
        return;

    int i = 0;

    DIALOGITEM *pItems[5] = {g_video_items, g_photo_items, g_setup_items, g_display_items, g_date_time_items};

    int length[5] = {GET_ARRAY_LENGTH(g_video_items), GET_ARRAY_LENGTH(g_photo_items), GET_ARRAY_LENGTH(g_setup_items), GET_ARRAY_LENGTH(g_display_items), GET_ARRAY_LENGTH(g_date_time_items)};

    for(i = 0; i < 5; i ++) {
        if(keyId < length[i]) {//found in the list
            INFO("UpdateSetupDialogItemValue, i:%d, keyId: %d\n", i, keyId);
            pItems[i][keyId].currentValue = GetStringId(value);
            return;
        } else {
            keyId = keyId - length[i];
        }
    }
    ERROR("UpdateSetupDialogItemValue failed\n");
}

int GetSettingItemsCount(int type)
{
    int count = 0;
    switch(type) {
        case TYPE_VIDEO_SETTINGS:
            count = GET_ARRAY_LENGTH(g_video_items);
            break;
        case TYPE_PHOTO_SETTINGS:
            count = GET_ARRAY_LENGTH(g_photo_items);
            break;
        case TYPE_SETUP_SETTINGS:
            count = GET_ARRAY_LENGTH(g_setup_items);
            break;
        case TYPE_DISPLAY_SETTINGS:
            count = GET_ARRAY_LENGTH(g_display_items);
            break;
        case TYPE_DATE_TIME_SETTINGS:
            count = GET_ARRAY_LENGTH(g_date_time_items);
            break;
    }
    return count;
}
