#ifndef COMMON_INTF_CFG_H
#define COMMON_INTF_CFG_H

#include "common_inft_isp_tuning.h"
	
typedef struct PROTUNE_ATTR {
	WB_UI_VAL        whiteBalance;
	COLOR_UI_VAL     color;
	SHARPNESS_UI_VAL sharpness;
	EV_VAL           EVCompensation;
}Inft_tune_Attr;

typedef enum{
    VIDEO_MOD_TYPE_VIDEO,
	VIDEO_MOD_TYPE_SLOWMOTION,
	VIDEO_MOD_TYPE_TIMELAPSE,
	VIDEO_MOD_TYPE_CAR,
    VIDEO_MOD_TYPE_MAX,
}ENUM_VIDEO_MOD_TYPE;

typedef enum{
    LOW_LIGHT_OFF,
	LOW_LIGHT_ON,
    LOW_LIGHT_MAX,
}ENUM_LOW_LIGHT;


typedef enum{
	VIDEO_TIMELAPSE_3S,
	VIDEO_TIMELAPSE_5S,
	VIDEO_TIMELAPSE_10S,
	VIDEO_TIMELAPSE_60S,
    VIDEO_TIMELAPSE_MAX,
}ENUM_VIDEO_TIMELAPSE;


typedef enum{
	WDR_OFF,
	WDR_ON,
    WDR_MAX,
}ENUM_WDR;

typedef enum{
	RECORDAUDIO_OFF,
	RECORDAUDIO_ON,
    RECORDAUDIO_MAX,
}ENUM_RECORDAUDIO;

typedef enum{
	TIMESTAMP_OFF,
	TIMESTAMP_ON,
    TIMESTAMP_MAX,
}ENUM_TIMESTAMP;

typedef enum{
	GSENSOR_OFF,
	GSENSOR_LOW,
	GSENSOR_MEDIUM,
	GSENSOR_HIGH,
    GSENSOR_MAX,
}ENUM_GSENSOR;

typedef enum{
    PLATESTAMP_OFF,
	PLATESTAMP_ON,
	PLATESTAMP_MAX,
}ENUM_PLATESTAMP;

typedef enum{
    COLLIDE_OFF,
	COLLIDE_ON,
	COLLIDE_MAX,
}ENUM_COLLIDE;


typedef struct VIDEO_MODE_ATTR {
	ENUM_VIDEO_MOD_TYPE    videoModeType;	
	unsigned int           width;	
	unsigned int           height;
	unsigned int           frameRate;
	ENUM_LOW_LIGHT         lowLight;
	ENUM_VIDEO_TIMELAPSE   timelapse;
	VIDEO_ISO_LIMIT_UI_VAL ISOLimit;
	Inft_tune_Attr         tuneAtr;
	unsigned int           loopRecordTime;//minute
	ENUM_WDR               wdr;
	ENUM_RECORDAUDIO       recordaudio;
	ENUM_TIMESTAMP         dateStamp;
	ENUM_GSENSOR           gsensor;
	ENUM_PLATESTAMP        platestamp;
	unsigned char          plateNumber[10];
	int                    zoom;//(0--40)
	ENUM_COLLIDE            collide;
}Inft_Video_Mode_Attr;

typedef enum{
    PHOTO_MOD_TYPE_SINGLE,
	PHOTO_MOD_TYPE_CONTINUOUS,
	PHOTO_MOD_TYPE_NIGHT,
    PHOTO_MOD_TYPE_MAX,
}ENUM_PHOTO_MOD_TYPE;

typedef enum{
    SHUTTER_TIME_AUTO,
	SHUTTER_TIME_2S,
	SHUTTER_TIME_5S,
	SHUTTER_TIME_10S,
	SHUTTER_TIME_15S,
	SHUTTER_TIME_20S,
	SHUTTER_TIME_30S,
    SHUTTER_TIME_MAX,
}ENUM_SHUTTER_TIME;

typedef enum{
	TIME_INTERVAL_3_PER_SEC,
	TIME_INTERVAL_5_PER_SEC,
	TIME_INTERVAL_10_PER_SEC,
    TIME_INTERVAL_MAX,
}ENUM_TIME_INTERVAL;



typedef enum{
	PHOTO_TIMER_OFF,
	PHOTO_TIMER_2S,
	PHOTO_TIMER_5S,
	PHOTO_TIMER_10S,
    PHOTO_TIMER_MAX,
}ENUM_PHOTO_TIMER;

typedef enum{
	PHOTO_QUALITY_HIGH,
	PHOTO_QUALITY_MIDDLE,
	PHOTO_QUALITY_LOW,
    PHOTO_QUALITY_MAX,
}ENUM_PHOTO_QUALITY;

typedef enum{
	ANTISHAKING_OFF,
	ANTISHAKING_ON,
	ANTISHAKING_MAX,
}ENUM_ANTISHAKING;

typedef struct PHOTO_MODE_ATTR {
	ENUM_PHOTO_MOD_TYPE       photoModeType;
	ENUM_SHUTTER_TIME         shutterTime;	
	ENUM_TIME_INTERVAL        timeInterval;
	PHOTO_RESOLUTION_UI_VAL   megapixels;
	int						  width;
	int						  height;
	ENUM_PHOTO_TIMER          timer;
	PHOTO_ISO_LIMIT_UI_VAL    ISOLimit;
	Inft_tune_Attr            tuneAtr;
	ENUM_PHOTO_QUALITY        quality;
	ENUM_TIMESTAMP            dateStamp;
	ENUM_ANTISHAKING          antishaking;
	int                       zoom;//(0--40)
	int                       sequence;//how many photos in one burst
}Inft_PHOTO_Mode_Attr;

typedef enum{
	LANGUAGE_EN,
	LANGUAGE_CN,
	LANGUAGE_FR,
	LANGUAGE_ES,
	LANGUAGE_IT,
	LANGUAGE_PT,
	LANGUAGE_DE,
	LANGUAGE_RU,
    LANGUAGE_MAX,
}ENUM_LANGUAGE;


typedef enum{
	AUTOPOWEROFF_OFF,
	AUTOPOWEROFF_3M,
	AUTOPOWEROFF_5M,
	AUTOPOWEROFF_10M,
	AUTOPOWEROFF_MAX,
}ENUM_AUTOPOWEROFF;

typedef enum{
	BEEPSOUND_OFF,
	BEEPSOUND_70PER,
	BEEPSOUND_100PER,
	BEEPSOUND_MAX,
}ENUM_BEEPSOUND;

typedef enum{
	DELAYEDSHUTDOWN_OFF,
	DELAYEDSHUTDOWN_10S,
	DELAYEDSHUTDOWN_1MIN,
	DELAYEDSHUTDOWN_3MIN,
	DELAYEDSHUTDOWN_MAX,
}ENUM_DELAYSHUTDOWN;

typedef enum{
	DISPLAY_SLEEP_OFF,
	DISPLAY_SLEEP_3M,
	DISPLAY_SLEEP_5M,
	DISPLAY_SLEEP_10M,
	DISPLAY_SLEEP_MAX,
}ENUM_DISPLAY_SLEEP;

typedef enum{
	DISPLAY_BRIGHTNESS_HIGH,
	DISPLAY_BRIGHTNESS_MEDIUM,
	DISPLAY_BRIGHTNESS_LOW,
	DISPLAY_BRIGHTNESS_MAX,
}ENUM_DISPLAY_BRIGHTNESS;

typedef enum{
	TV_MODE_PAL,
	TV_MODE_NTSC,
	TV_MODE_MAX,
}ENUM_TV_MODE;

typedef enum{
	FREQUENCY_50HZ,
	FREQUENCY_60HZ,
	FREQUENCY_MAX,
}ENUM_FREQUENCY;

typedef enum{
    IR_LED_AUTO,
	IR_LED_ON,
	IR_LED_OFF,
	IR_LED_MAX,
}ENUM_IR_LED;

typedef enum{
    CUR_ACTION_NONE,
	CUR_ACTION_RECORD,
	CUR_ACTION_PHOTO,
	CUR_ACTION_MAX,
}ENUM_CUR_ACTION;

typedef struct CONFIG_MODE_ATTR {
	ENUM_CUR_ACTION         cur_action;
	ENUM_AUTOPOWEROFF       autopoweroff;
	ENUM_LANGUAGE           language;
	ENUM_TIMESTAMP          timestamp;
	int                     dirSeq;
	int                     fileSeq;
	int                     burstSeq;
	ENUM_BEEPSOUND          beepsound;
	ENUM_DELAYSHUTDOWN      delayshutdown;
	ENUM_DISPLAY_SLEEP      displaySleep;
	ENUM_DISPLAY_BRIGHTNESS displayBright;
	ENUM_TV_MODE            tvmode;
	ENUM_FREQUENCY          frequency;
	ENUM_IR_LED             irled;
	double                  ambientBright;
	MIRROR_FLIP_UI_VAL      mirror;
	int                     audioMsgTotal;
}Inft_CONFIG_Mode_Attr;

typedef enum{
    PLAY_STATE_BEGIN,
	PLAY_STATE_END,
	PLAY_STATE_ERROR,
	PLAY_STATE_MAX,
}ENUM_PALY_STATE;


typedef struct Play_ATTR {
	int       duration;
	int       position;
	float     speed;//speed is 0.25,0.5,2,4
	ENUM_PALY_STATE playState;
}Inft_Play_Attr;

typedef struct GPS_Data {
    double latitude;
    double longitude;
    double speed;
} GPS_Data;

typedef enum{
    TF_REMOVED,
	TF_SUCCESS,
	TF_ERROR,
	TF_FORMAT,
	TF_PREPARE,
}ENUM_SD_STATE;

typedef struct SD_Status {
    ENUM_SD_STATE sdStatus;
} SD_Status;

#endif

