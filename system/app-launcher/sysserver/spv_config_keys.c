#include<stdio.h>
#include<string.h>

#include "spv_config_keys.h"
#include "spv_debug.h"
#include "spv_config.h"

#ifndef GET_ARRAY_LENGTH
#define GET_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))
#endif

const char *g_mode_name[] = {
    "Video",
    "Photo",
#ifdef GUI_PLAYBACK_ENABLE
    "Playback",
#endif
#if USE_SETTINGS_MODE
    "Setup",
#endif
};

const char *g_config_key[] = {
    "current.mode",

    //video
    "video.resolution",
	"video.recordmode",
	"video.preview",
    "video.looprecording",
	"video.timelapsedimage",
	"video.highspeed",
    "video.wdr",
    "video.ev",
	"video.gsensor",
    "video.datestamp",
    "video.recordaudio",
    "",

    //photo
    "photo.resolution",
	"photo.capturemode",
	"photo.preview",
	"photo.modetimer",
	"photo.modeauto",
	"photo.modesequence",
    "photo.quality",
    "photo.sharpness",
    "photo.whitebalance",
    "photo.color",
    "photo.isolimit",
    "photo.ev",
    "photo.antishaking",
    "photo.datestamp",
    "",

    //setup
    "setup.language",
	"setup.wifimode",
	"setup.ledfreq",
	"setup.led",
	"setup.cardvmode",
    "setup.parkingguard",
    "setup.imagerotation",
    "",
	"setup.volume",
    "setup.keytone",
    "setup.brightness",
	"setup.autosleep",
	"setup.autopoweroff",
    "setup.delayedshutdown",
    "",
    "",
    "",

    //
    "",
    "",

    //external status key 
    "setup.wifi.ap.ssid",
    "setup.wifi.ap.password",
    "setup.wifi.ap.keymgmt",
    "setup.wifi.sta.ssid",
    "setup.wifi.sta.password",
    "setup.wifi.sta.keymgmt",

    "current.reverseimage",
};

int GetKeyId(const char *key)
{
    int length = sizeof(g_config_key) / sizeof(char *);
    int keyId = -1, i = 0;

    if(key == NULL || strlen(key) <= 0)
        return keyId;

    for(i = 0; i < length; i ++)
    {
        if(!strcasecmp(g_config_key[i], key))
            return i;
    }

    return keyId;
}


