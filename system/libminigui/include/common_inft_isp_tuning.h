#ifndef COMMON_INTF_ISP_TUNING_H
#define COMMON_INTF_ISP_TUNING_H


typedef enum __CONFIG_ID
{
    VIDEO_RESOLUTION_ID = 0,
    PHOTO_RESOLUTION_ID,
    WB_ID,
    COLOR_ID,
    VIDEO_ISO_LIMIT_ID,
    PHOTO_ISO_LIMIT_ID,
    SHARPNESS_ID,
    EV_ID,
    MIRROR_FLIP_ID,
    OV_PLATE_ID1,
    OV_PLATE_ID2,
    OV_TIME_ID,
    ISP_ZOOM_ID,
    ISP_EN_WIDTH_ID,
    ISP_EN_HEIGHT_ID,
    ISP_LONGITUDE_ID1,	//jingdu
    ISP_LONGITUDE_ID2,  //jingdu
    ISP_LATITUDE_ID1,	//weidu
    ISP_LATITUDE_ID2,	//weidu
    ISP_SPEED_ID,
}CONFIG_ID;

typedef enum __VIDEO_RESOLUTION_UI_VAL
{
    VIDEO_1080P = 0,
    VIDEO_720P,
    VIDEO_480P,//800X480
    VIDEO_400P,//800X400
    VIDEO_320P,//720X320
    VIDEO_240P,//320X240
}VIDEO_RESOLUTION_UI_VAL;

typedef enum __PHOTO_RESOLUTION_UI_VAL
{
    PHOTO_8MP = 0,
    PHOTO_5MP,
    PHOTO_1080P,
    PHOTO_720P,
    PHOTO_WVGA,
    PHOTO_VGA,
}PHOTO_RESOLUTION_UI_VAL;

typedef enum __WB_UI_VAL
{
    AWB_MODE = 0,
    WB_3000K,
    WB_4100K,
    WB_5500K,
    WB_6500K,
    WB_NONE,
}WB_UI_VAL;

typedef enum __COLOR_UI_VAL
{
    ADVANCED_COLOR_MODE = 0,
    FLAT_COLOR_MODE,
    SEPIA_COLOR_MODE,
}COLOR_UI_VAL;

typedef enum __VIDEO_ISO_LIMIT_UI_VAL
{
    VIDEO_ISO_AUTO = 0,
    VIDEO_ISO_6400,
    VIDEO_ISO_1600,
    VIDEO_ISO_400,
}VIDEO_ISO_LIMIT_UI_VAL;

typedef enum __PHOTO_ISO_LIMIT_UI_VAL
{
    PHOTO_ISO_AUTO = 0,
    PHOTO_ISO_800,
    PHOTO_ISO_400,
    PHOTO_ISO_200,
    PHOTO_ISO_100,
}PHOTO_ISO_LIMIT_UI_VAL;

typedef enum __SHARPNESS_UI_VAL
{
    HIGH_SHARPNESS = 0,
    MEDIUM_SHARPNESS,
    LOW_SHARPNESS,
}SHARPNESS_UI_VAL;

typedef enum __EV_UI_VAL
{
    EV_MINUS_2_0 = 0,
    EV_MINUS_1_5,
    EV_MINUS_1_0,
    EV_MINUS_0_5,
    EV_0_0,
    EV_0_5,
    EV_1_0,
    EV_1_5,
    EV_2_0,
}EV_VAL;

typedef enum __MIRROR_FLIP_UI_VAL
{
    NONE = 0,
    MIRROR,
    FLIP,
    MIRROR_AND_FLIP,
}MIRROR_FLIP_UI_VAL;

#ifdef __cplusplus
extern "C"
{
#endif

int GetISPConfigItemState(int ConfigId);
int SetISPConfigItemState(int ConfigId, int UI_Val);
double GetIEnvBrightness(void);
void set_skip_frames_cnt(unsigned int cnt);
#ifdef __cplusplus
}
#endif


#endif

