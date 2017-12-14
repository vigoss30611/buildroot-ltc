#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#include "spv_config.h"
#include "spv_debug.h"
#include "camera_spv.h"
#include "common_inft_cfg.h"
#include <qsdk/videobox.h>
#include <qsdk/vplay.h>
#include <qsdk/codecs.h>
#include <qsdk/event.h>
#include <qsdk/jpegexif.h>

#include "sys_server.h"
#include "spv_file_manager.h"

#define CAMERA_SPV_DEBUG 1


#define VIDEO_CONTAINER "mp4"

#define RESOLUTION_1920_PATH "/root/.videobox/1920.json"
#define RESOLUTION_1440_PATH  "/root/.videobox/1440.json"
#define RESOLUTION_1080_PATH "/root/.videobox/path.json"
#define RESOLUTION_720_PATH  "/root/.videobox/720.json"

#define CALIBRATION_PATH "/root/.videobox/uvc.json"

enum {
    RESOLUTION_OK,
    RESOLUTION_CHANGING,
    RESOLUTION_CHANGED,
};
static int g_resolution_changed = RESOLUTION_OK;

static int pthread_status = 0;
struct font_attr overlay_font;

camera_spv *camera_spv_g = NULL;
config_camera *config_camera_g = NULL;
Inft_PHOTO_Mode_Attr photo_attr_g;
Inft_Video_Mode_Attr video_attr_g;

static VRecorder *g_recorder =  NULL;
static VRecorderInfo recorderInfo;
vplay_event_info_t vplayState;

static int g_video_locked = 0;
int g_p2p_stream_started = 0;

int init_camera();
int release_camera();

int set_mode();
int set_config(config_camera config);

int start_preview();
int stop_preview();

int start_video();
int stop_video();

int start_photo();
int stop_photo();

int media_init();
int get_snapshot(char *filename);

/*
int display_photo(char *filename);
int start_playback(char *filename);
int stop_playback(char *filename);

int error_callback(int error);

*/
#ifdef PREVIEW_SECOND_WAY
#define DEFAULT_LOCAL_BITRATE (12*1024*1024)
struct v_bitrate_info LOCAL_MEDIA_BITRATE_INFO = {
    .bitrate = DEFAULT_LOCAL_BITRATE,
    .qp_max = 41,
    .qp_min = 10,
    .qp_delta = -5,
    .qp_hdr = 30,
    .gop_length = 60,
    .picture_skip = 0,
    .p_frame = 15,
};
#else
#define DEFAULT_LOCAL_BITRATE (10*1024*1024)
struct v_bitrate_info LOCAL_MEDIA_BITRATE_INFO = {
    .bitrate = DEFAULT_LOCAL_BITRATE,
    .qp_max = 41,
    .qp_min = 10,
    .qp_delta = -5,
    .qp_hdr = -1,
    .gop_length = 60,
    .picture_skip = 0,
    .p_frame = 30,
};
#endif

#define DEFAULT_PREVIEW_BITRATE (6*1024*1024)


#ifdef PREVIEW_SECOND_WAY
static struct v_bitrate_info WIFI_PREVIEW_BITRATE_INFO = {
    .bitrate = DEFAULT_PREVIEW_BITRATE,
    .qp_max = 51,
    .qp_min = 20,
    .qp_delta = 0,
    .qp_hdr = 30,
    .gop_length = 30,
    .picture_skip = 0,
    .p_frame = 10,
};
#else
static struct v_bitrate_info WIFI_PREVIEW_BITRATE_INFO;
#endif

int reverse_image_enabled(void);
void set_video_attr_value(Inft_Video_Mode_Attr *video_attr);
int reset_preview_stream_params(char *channel);

int get_ambientBright(void)
{
    INFO("enter func %s \n", __func__);
    int ret = -1;
    int bv = 0;

    if ((ret = camera_get_brightnessvalue(FCAM, &bv)) < 0) {
	ERROR("get brightness error \n");
	return -1;
    }

    INFO("ambientBright is %d \n", ret);
    INFO("leave func %s \n", __func__);

    return bv >= FRONTIER ? 1 : 0;
}

int init_camera()
{
    INFO("enter func %s \n", __func__);
    INFO("leave func %s \n", __func__);
    return 0;
}

int release_camera()
{
    INFO("enter func %s \n", __func__);
    INFO("leave func %s \n", __func__);
    return 0;
}

int set_mode()
{
    INFO("enter func %s \n", __func__);
    INFO("leave func %s \n", __func__);
    return 0;
}

void set_video_resolution(char *resolution)
{
#if 0 //def GUI_SET_RESOLUTION_SEPARATELY
    static int w = 1920;
    static int h = 1080;
    int width, height;
    if(resolution != NULL && (strlen(resolution) >= 4)
            && !strncasecmp(resolution, "720P", 4)) {
        width = 1280;
        height = 720;
    } else {
        width = 1920;
        height = 1080;
    }
    printf("w: %d, h: %d\n", width, height);
    if(w != width) {
        w = width;
        h = height;
        printf("Inft_cfg_setResolution, w: %d, h: %d\n", width, height);
        Inft_cfg_setResolution(width, height);
    }
#endif
}

void set_video_resolution_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL) {
	    ERROR("config or video_attr is null \n");
    }
    int default_width = 0;

#ifdef PRODUCT_LESHI_C23
    static int s_width = 1920;
    char *video_path = NULL;
    default_width = 1920;
    if(strstr(config, "736")) {
        video_attr->width = 1472;
        video_attr->height = 736;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_720_PATH;
    } else {
        video_attr->width = 1920;
        video_attr->height = 960;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_1080_PATH;
    }
#else
    static int s_width = 1088;
    char *video_path = NULL;
    default_width = 1088;
    if(strstr(config, "768")) {
        video_attr->width = 768;
        video_attr->height = 768;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_720_PATH;
    } else if(strstr(config, "1472")) {
        video_attr->width = 1472;
        video_attr->height = 1472;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_1440_PATH;
    } else if(strstr(config, "1920")) {
        video_attr->width = 1920;
        video_attr->height = 1920;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_1920_PATH;
    } else {
        video_attr->width = 1088;
        video_attr->height = 1088;
        video_attr->frameRate = 30;
        video_path = RESOLUTION_1080_PATH;
    }
#endif

    if(s_width != video_attr->width) {
        float ratio = sqrt((float)video_attr->width / default_width);
        INFO("ratio: %f, new bitrate: %dkbps\n", ratio, (int)(ratio * DEFAULT_LOCAL_BITRATE) >> 10);
        LOCAL_MEDIA_BITRATE_INFO.bitrate = (int)(ratio * DEFAULT_LOCAL_BITRATE);
#ifdef PREVIEW_SECOND_WAY
        WIFI_PREVIEW_BITRATE_INFO.bitrate = (int)(ratio * DEFAULT_PREVIEW_BITRATE);
#endif

        g_resolution_changed = RESOLUTION_CHANGING;
        int ret = videobox_repath(video_path);
        if(ret) {
            ERROR("video box repath failed, path: %s\n", video_path);
        } else {
            s_width = video_attr->width;
            //video_disable_channel(VCHN);

            Inft_Video_Mode_Attr video_attr;
            set_video_attr_value(&video_attr);
            if (cfg_setVideoAttr(&video_attr, 1)) {
                ERROR("cfg_setVideoAttr error \n");
            }

            if(!g_p2p_stream_started) {
                video_disable_channel(VPCHN);
            }
            reset_preview_stream_params(VPCHN);
        }
        g_resolution_changed = RESOLUTION_CHANGED;
    }
}

/**
 * get the bitrate of recorder, in bps
 **/
int get_recorder_bitrate()
{
    return LOCAL_MEDIA_BITRATE_INFO.bitrate;
}

void set_video_looprecording_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

/*    if (!strcasecmp(config, "off")) {
	video_attr->loopRecordTime = 0;
    } else if (!strcasecmp(config, "3 minutes")) {
	video_attr->loopRecordTime = 3;
    } else if (!strcasecmp(config, "5 minutes")) {
	video_attr->loopRecordTime = 5;
    } else if (!strcasecmp(config, "10 minutes")) {
	video_attr->loopRecordTime = 10;
    }*/
    video_attr->loopRecordTime = atoi(config);//modify by kason for more compatible settings
}

void set_video_wdr_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->wdr = WDR_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->wdr = WDR_OFF;
    }
}

void set_video_ev_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "0")) {
	video_attr->tuneAtr.EVCompensation = 0;
    } else if (!strcasecmp(config, "+0.5")) {
	video_attr->tuneAtr.EVCompensation = 1;
    } else if (!strcasecmp(config, "+1.0")) {
	video_attr->tuneAtr.EVCompensation = 2;
    } else if (!strcasecmp(config, "+1.5")) {
	video_attr->tuneAtr.EVCompensation = 3;
    } else if (!strcasecmp(config, "+2.0")) {
	video_attr->tuneAtr.EVCompensation = 4;
    } else if (!strcasecmp(config, "-0.5")) {
	video_attr->tuneAtr.EVCompensation = -1;
    } else if (!strcasecmp(config, "-1.0")) {
	video_attr->tuneAtr.EVCompensation = -2;
    } else if (!strcasecmp(config, "-1.5")) {
	video_attr->tuneAtr.EVCompensation = -3;
    } else if (!strcasecmp(config, "-2.0")) {
	video_attr->tuneAtr.EVCompensation = -4;
    }
}

void set_video_recordaudio_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->recordaudio = RECORDAUDIO_ON;
//	set_mute(1);
    } else if (!strcasecmp(config, "off")) {
	video_attr->recordaudio = RECORDAUDIO_OFF;
//	set_mute(0);
    }
}

void set_video_datestamp_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->dateStamp = TIMESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->dateStamp = TIMESTAMP_OFF;
    }
}

void set_video_gsensor_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }
    int level = 2;
    int needset = 0;

    if (!strcasecmp(config, "off")) {
	    video_attr->gsensor = GSENSOR_OFF;
    } else if (!strcasecmp(config, "low")) {
        needset = 1;
        level = 3;
	video_attr->gsensor = GSENSOR_LOW;
    } else if (!strcasecmp(config, "medium")) {
        needset = 1;
        level = 2;
	video_attr->gsensor = GSENSOR_MEDIUM;
    } else if (!strcasecmp(config, "high")) {
        needset = 1;
        level = 1;
	video_attr->gsensor = GSENSOR_HIGH;
    }
    if(needset) {
        char cmd[128] = {0};
        // GSENSOR_MODEL env is set by input_identifier
        sprintf(cmd, "echo %d > /sys/devices/virtual/misc/%s/attribute/sensitivity", level, getenv("GSENSOR_MODEL"));
        //my_system(cmd);
    }
}

void set_video_platestamp_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->platestamp = PLATESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->platestamp = PLATESTAMP_OFF;
    }
}

void set_video_platenumber_value(unsigned char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    strcpy(video_attr->plateNumber, config);
}

void set_video_zoom_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    video_attr->zoom = atof(config + 1) * 10;
}

void set_other_collide_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    if (config == NULL || video_attr == NULL)
    {
	ERROR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
        if(g_recorder != NULL) {
            INFO("collision occured, need to lock the current video\n");
            g_video_locked = 1;
        }
	video_attr->collide = COLLIDE_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->collide = COLLIDE_OFF;
    }
}

void set_video_attr_value(Inft_Video_Mode_Attr *video_attr)
{
    set_video_resolution_value(config_camera_g->video_resolution, video_attr);
    set_video_looprecording_value(config_camera_g->video_looprecording, video_attr);
    set_video_wdr_value(config_camera_g->video_wdr, video_attr);
    set_video_ev_value(config_camera_g->video_ev, video_attr);
    set_video_recordaudio_value(config_camera_g->video_recordaudio, video_attr);
    set_video_datestamp_value(config_camera_g->video_datestamp, video_attr);
    set_video_gsensor_value(config_camera_g->video_gsensor, video_attr);
    set_video_platestamp_value(config_camera_g->video_platestamp, video_attr);
    set_video_platenumber_value(config_camera_g->setup_platenumber, video_attr);
    set_video_zoom_value(config_camera_g->video_zoom, video_attr);
    set_other_collide_value(config_camera_g->other_collide, video_attr);
}

void set_photo_capturemode_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    photo_attr->photoModeType = PHOTO_MOD_TYPE_SINGLE;
}

void set_photo_resolution_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

#ifdef PRODUCT_LESHI_C23
    INFO("config: %s\n", config);
    if (!strncasecmp(config, "5M", 2)) {
        photo_attr->width = 3008;
        photo_attr->height = 1504;
    } else {
        photo_attr->width = 1920;
        photo_attr->height = 960;
    }
#else
    if (!strncasecmp(config, "16M", 3)) {
        photo_attr->width = 4096;
        photo_attr->height = 4096;
    } else if (!strncasecmp(config, "10M", 3)) {
        photo_attr->width = 3264;
        photo_attr->height = 3264;
    } else if (!strncasecmp(config, "8M", 2)) {
        photo_attr->width = 2880;
        photo_attr->height = 2880;
    } else if (!strncasecmp(config, "6M", 2)) {
        photo_attr->width = 2448;
        photo_attr->height = 2448;
    } else {
        photo_attr->width = 1088;
        photo_attr->width = 1088;
    }
#endif

}

void set_photo_sequence_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->sequence = 3;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->sequence = 1;
    }
}

void set_photo_quality_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "fine")) {
	photo_attr->quality = PHOTO_QUALITY_HIGH;
    } else if (!strcasecmp(config, "normal")) {
	photo_attr->quality = PHOTO_QUALITY_MIDDLE;
    } else if (!strcasecmp(config, "economy")) {
	photo_attr->quality = PHOTO_QUALITY_LOW;
    }
}

void set_photo_sharpness_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "strong")) {
	photo_attr->tuneAtr.sharpness = 255;
    } else if (!strcasecmp(config, "normal")) {
	photo_attr->tuneAtr.sharpness = 127;
    } else if (!strcasecmp(config, "soft")) {
	photo_attr->tuneAtr.sharpness = 0;
    }
}

void set_photo_whitebalance_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "auto")) {
	photo_attr->tuneAtr.whiteBalance = CAM_WB_AUTO;
    } else if (!strcasecmp(config, "daylight")) {
	photo_attr->tuneAtr.whiteBalance = CAM_WB_5300K;// changed by linyun.xiong @2015-09-21
    } else if (!strcasecmp(config, "cloudy")) {
	photo_attr->tuneAtr.whiteBalance = CAM_WB_4000K;// // changed by linyun.xiong @2015-09-21
    } else if (!strcasecmp(config, "tungsten")) {
	photo_attr->tuneAtr.whiteBalance = CAM_WB_6800K;
    } else if (!strcasecmp(config, "fluorescent")) {
	photo_attr->tuneAtr.whiteBalance = CAM_WB_2500K;// changed by linyun.xiong @2015-09-21
    }
}

void set_photo_color_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "color")) {
	photo_attr->tuneAtr.color = ADVANCED_COLOR_MODE;
    } else if (!strcasecmp(config, "Black & White")) {
	photo_attr->tuneAtr.color = FLAT_COLOR_MODE;
    } else if (!strcasecmp(config, "sepia")) {
	//photo_attr->tuneAtr.color = SEPIA_COLOR_MODE;
	photo_attr->tuneAtr.color = ADVANCED_COLOR_MODE;
    }
    INFO("set_photo_color_value, color: %d\n", photo_attr->tuneAtr.color);
}

void set_photo_isolimit_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "auto")) {
	photo_attr->ISOLimit = 0;
    } else if (!strcasecmp(config, "800")) {
	photo_attr->ISOLimit = 8;
    } else if (!strcasecmp(config, "400")) {
	photo_attr->ISOLimit = 4;
    } else if (!strcasecmp(config, "200")) {
	photo_attr->ISOLimit = 2;
    } else if (!strcasecmp(config, "100")) {
	photo_attr->ISOLimit = 1;
    }
}

void set_photo_ev_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "0")) {
	photo_attr->tuneAtr.EVCompensation = 0;
    } else if (!strcasecmp(config, "+0.5")) {
	photo_attr->tuneAtr.EVCompensation = 1;
    } else if (!strcasecmp(config, "+1.0")) {
	photo_attr->tuneAtr.EVCompensation = 2;
    } else if (!strcasecmp(config, "+1.5")) {
	photo_attr->tuneAtr.EVCompensation = 3;
    } else if (!strcasecmp(config, "+2.0")) {
	photo_attr->tuneAtr.EVCompensation = 4;
    } else if (!strcasecmp(config, "-0.5")) {
	photo_attr->tuneAtr.EVCompensation = -1;
    } else if (!strcasecmp(config, "-1.0")) {
	photo_attr->tuneAtr.EVCompensation = -2;
    } else if (!strcasecmp(config, "-1.5")) {
	photo_attr->tuneAtr.EVCompensation = -3;
    } else if (!strcasecmp(config, "-2.0")) {
	photo_attr->tuneAtr.EVCompensation = -4;
    }
}

void set_photo_antishaking_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->antishaking = ANTISHAKING_ON;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->antishaking = ANTISHAKING_OFF;
    }
}

void set_photo_datestamp_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->dateStamp = TIMESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->dateStamp = TIMESTAMP_OFF;
    }
}

void set_photo_zoom_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    if (config == NULL || photo_attr == NULL)
    {
	ERROR("config or photo_attr is null \n");
    }

    photo_attr->zoom = atof(config + 1) * 10;
}

void set_photo_attr_value(Inft_PHOTO_Mode_Attr *photo_attr)
{
    set_photo_capturemode_value(config_camera_g->photo_capturemode, photo_attr);
    set_photo_resolution_value(config_camera_g->photo_resolution, photo_attr);
    set_photo_sequence_value(config_camera_g->photo_sequence, photo_attr);
    set_photo_quality_value(config_camera_g->photo_quality, photo_attr);
    set_photo_sharpness_value(config_camera_g->photo_sharpness, photo_attr);
    set_photo_whitebalance_value(config_camera_g->photo_whitebalance, photo_attr);
    set_photo_color_value(config_camera_g->photo_color, photo_attr);
    set_photo_isolimit_value(config_camera_g->photo_isolimit, photo_attr);
    set_photo_ev_value(config_camera_g->photo_ev, photo_attr);
    set_photo_antishaking_value(config_camera_g->photo_antishaking, photo_attr);
    set_photo_datestamp_value(config_camera_g->photo_datestamp, photo_attr);
    set_photo_zoom_value(config_camera_g->photo_zoom, photo_attr);
}

void show_video_attr(Inft_Video_Mode_Attr video_attr)
{
    INFO("width is %d \n", video_attr.width);
    INFO("height is %d \n", video_attr.height);
    INFO("frameRate is %d \n", video_attr.frameRate);
    INFO("loopRecordTime is %d \n", video_attr.loopRecordTime);
    INFO("wdr is %d \n", video_attr.wdr);
    INFO("EVCompensation is %d \n", video_attr.tuneAtr.EVCompensation);
    INFO("recordaudio is %d \n", video_attr.recordaudio);
    INFO("dateStamp is %d \n", video_attr.dateStamp);
    INFO("gsensor is %d \n", video_attr.gsensor);
    INFO("platestamp is %d \n", video_attr.platestamp);
    INFO("plateNumber is %s \n", video_attr.plateNumber);
    INFO("zoom is %d \n", video_attr.zoom);
    INFO("collide is %d \n", video_attr.collide);
}

void show_photo_attr(Inft_PHOTO_Mode_Attr photo_attr)
{
    INFO("photoModeType is %d \n", photo_attr.photoModeType);
    INFO("megapixels is %d \n", photo_attr.megapixels);
    INFO("sequence is %d \n", photo_attr.sequence);
    INFO("quality is %d \n", photo_attr.quality);
    INFO("sharpness is %d \n", photo_attr.tuneAtr.sharpness);
    INFO("whiteBalance is %d \n", photo_attr.tuneAtr.whiteBalance);
    INFO("color is %d \n", photo_attr.tuneAtr.color);
    INFO("ISOLimit is %d \n", photo_attr.ISOLimit);
    INFO("EVCompensation is %d \n", photo_attr.tuneAtr.EVCompensation);
    INFO("antishaking is %d \n", photo_attr.antishaking);
    INFO("dateStamp is %d \n", photo_attr.dateStamp);
    INFO("zoom is %d \n", photo_attr.zoom);
}

void show_setup_attr(Inft_CONFIG_Mode_Attr setup_attr)
{
    INFO("tvmode is %d \n", setup_attr.tvmode);
    INFO("frequency is %d \n", setup_attr.frequency);
    INFO("irled is %d \n", setup_attr.irled);
    INFO("mirror is %d \n", setup_attr.mirror);
}

void set_setup_tvmode_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    if (config == NULL || setup_attr == NULL)
    {
	ERROR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "ntsc")) {
	setup_attr->tvmode = TV_MODE_NTSC;
    } else if (!strcasecmp(config, "pal")) {
	setup_attr->tvmode = TV_MODE_PAL;
    }
}

void set_setup_frequency_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    if (config == NULL || setup_attr == NULL)
    {
	ERROR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "50hz")) {
	setup_attr->frequency = FREQUENCY_50HZ;
    } else if (!strcasecmp(config, "60hz")) {
	setup_attr->frequency = FREQUENCY_60HZ;
    }
}

void set_setup_irled_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    if (config == NULL || setup_attr == NULL)
    {
	ERROR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "auto")) {
	setup_attr->irled = IR_LED_AUTO;
    } else if (!strcasecmp(config, "on")) {
	setup_attr->irled = IR_LED_ON;
    } else if (!strcasecmp(config, "off")) {
	setup_attr->irled = IR_LED_OFF;
    }
}

void set_setup_imagerotation_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    if (config == NULL || setup_attr == NULL)
    {
	ERROR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "off")) {
	setup_attr->mirror = NONE;
    } else if (!strcasecmp(config, "on")) {
	setup_attr->mirror = FLIP;//MIRROR;
    }
}

void set_setup_attr_value(Inft_CONFIG_Mode_Attr *setup_attr)
{
    set_setup_tvmode_value(config_camera_g->setup_tvmode, setup_attr);
    set_setup_frequency_value(config_camera_g->setup_frequency, setup_attr);
    set_setup_irled_value(config_camera_g->setup_irled, setup_attr);
    set_setup_imagerotation_value(config_camera_g->setup_imagerotation, setup_attr);
}

int cfg_setVideoAttr(const Inft_Video_Mode_Attr* pstvideo_attr, int force)
{
    INFO("enter func %s \n", __func__);
    static Inft_Video_Mode_Attr s_video_attr;

    struct cam_zoom_info zoom_info;

    if( NULL == pstvideo_attr){
        INFO("pstvideo_attr is NULL");
        return ERROR_NULL;
    }

    memcpy(&video_attr_g, pstvideo_attr, sizeof(Inft_Video_Mode_Attr));

    //show_video_attr(*pstvideo_attr);

    if(force || s_video_attr.width != pstvideo_attr->width || s_video_attr.height != pstvideo_attr->height) {
        /*if(video_set_resolution(VCHN, pstvideo_attr->width, pstvideo_attr->height)) {
            INFO("%s: video set resolution %dx%d error\n", __func__, pstvideo_attr->width, pstvideo_attr->height);
            return ERROR_SET_FAIL;
        }*/
        s_video_attr.width = pstvideo_attr->width;
        s_video_attr.height = pstvideo_attr->height;
    }

    if(force) {
        if(camera_set_wb(FCAM, CAM_WB_AUTO)) {
            INFO("%s: camera_awb_enable error\n", __func__);
            //return ERROR_SET_FAIL;
        }

        if(camera_monochrome_enable(FCAM, 0)) {
            INFO("%s: camera_monochrome_enable error\n", __func__);
            //return ERROR_SET_FAIL;
        }

        if (camera_set_sharpness(FCAM, 127)) {
            INFO("%s: camera_set_sharpness error\n", __func__);
            //return ERROR_SET_FAIL;
        }

        if (camera_set_ISOLimit(FCAM, 1)) {
            INFO("%s: camera_set_ISOLimit error\n", __func__);
            //return ERROR_SET_FAIL;
        }

    }

    if(force || s_video_attr.tuneAtr.EVCompensation != pstvideo_attr->tuneAtr.EVCompensation) {
        if (camera_ae_enable(FCAM, 1) || camera_set_exposure(FCAM, pstvideo_attr->tuneAtr.EVCompensation)) {
            INFO("%s: camera_set_ae error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_video_attr.tuneAtr.EVCompensation = pstvideo_attr->tuneAtr.EVCompensation;
    }

    if(force || s_video_attr.wdr != pstvideo_attr->wdr) {
        if (camera_wdr_enable(FCAM, pstvideo_attr->wdr)) {
            INFO("%s: camera_set_wdr error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_video_attr.wdr = pstvideo_attr->wdr;
    }

#ifdef GUI_MARKER_ENABLE
    if(force || s_video_attr.dateStamp != pstvideo_attr->dateStamp) {
        if (pstvideo_attr->dateStamp == TIMESTAMP_ON) {
            INFO("enable video marker\n");
            marker_enable(VMARK);
            sprintf((char *)overlay_font.ttf_name, "arial");
            overlay_font.font_size = 8;
            overlay_font.font_color = 0x00ffffff;
            marker_set_mode(VMARK, "auto", "%t%Y/%M/%D  %H:%m:%S", &overlay_font);
        } else {
            INFO("diable video marker\n");
            marker_disable(VMARK);
        }
        s_video_attr.dateStamp = pstvideo_attr->dateStamp;
    }
#endif

    /*reverse for plateNumber*/

	/*zoom setting*/
	/*
	zoom_info.a_multiply = pstvideo_attr->zoom;
	zoom_info.d_multiply = pstvideo_attr->zoom;
	if (camera_set_zoom(FCAM, &zoom_info)) {
		INFO("%s: camera_set_zoom error\n", __func__);
		return ERROR_SET_FAIL;
	}
	*/

	INFO("leave func %s \n", __func__);
	return 0;
}

int cfg_setPhotoAttr(const Inft_PHOTO_Mode_Attr *pstPhotoAttr, int force)
{
    INFO("enter func %s \n", __func__);
    static Inft_PHOTO_Mode_Attr s_photo_attr;

    if( NULL == pstPhotoAttr){
        INFO("pstPhotoAttr is NULL");
        return ERROR_NULL;
    }


    memcpy(&photo_attr_g, pstPhotoAttr, sizeof(Inft_PHOTO_Mode_Attr));
    //show_photo_attr(*pstPhotoAttr);

    if(force || s_photo_attr.tuneAtr.whiteBalance != pstPhotoAttr->tuneAtr.whiteBalance) {
        if (camera_set_wb(FCAM, pstPhotoAttr->tuneAtr.whiteBalance)) {
            INFO("%s: camera_awb_enable error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.tuneAtr.whiteBalance = pstPhotoAttr->tuneAtr.whiteBalance;
    }

    if(force || s_photo_attr.tuneAtr.color != pstPhotoAttr->tuneAtr.color) {
        if (camera_monochrome_enable(FCAM, pstPhotoAttr->tuneAtr.color)) {
            INFO("%s: camera_monochrome_enable error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.tuneAtr.color = pstPhotoAttr->tuneAtr.color;
    }

    if(force || s_photo_attr.tuneAtr.sharpness != pstPhotoAttr->tuneAtr.sharpness) {
        if (camera_set_sharpness(FCAM, pstPhotoAttr->tuneAtr.sharpness)) {
            INFO("%s: camera_set_sharpness error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.tuneAtr.sharpness = pstPhotoAttr->tuneAtr.sharpness;
    }

    if(force || s_photo_attr.ISOLimit != pstPhotoAttr->ISOLimit) {
        if (camera_set_ISOLimit(FCAM, pstPhotoAttr->ISOLimit)) {
            INFO("%s: camera_set_ISOLimit error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.ISOLimit = pstPhotoAttr->ISOLimit;
    }

    if(force || s_photo_attr.tuneAtr.EVCompensation != pstPhotoAttr->tuneAtr.EVCompensation) {
        if (camera_ae_enable(FCAM, 1) || camera_set_exposure(FCAM, pstPhotoAttr->tuneAtr.EVCompensation)) {
            INFO("%s: camera_set_ae error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.tuneAtr.EVCompensation = pstPhotoAttr->tuneAtr.EVCompensation;
    }

    if(force || s_photo_attr.width != pstPhotoAttr->width || s_photo_attr.height != pstPhotoAttr->height) {
        /*if (video_set_resolution(PCHN, pstPhotoAttr->width, pstPhotoAttr->height)) {
            INFO("%s: photo set resolution %dx%d error\n", __func__, pstPhotoAttr->width, pstPhotoAttr->height);
            return ERROR_SET_FAIL;
        }*/
        s_photo_attr.width = pstPhotoAttr->width;
        s_photo_attr.height = pstPhotoAttr->height;
    }

    if(force || s_photo_attr.quality != pstPhotoAttr->quality) {
        if (!video_set_snap_quality(PCHN, pstPhotoAttr->quality)) {
            INFO("%s: video_set_snap_quality error\n", __func__);
            //return ERROR_SET_FAIL;
        }
        s_photo_attr.quality = pstPhotoAttr->quality;
    }

#ifdef GUI_MARKER_ENABLE
    if(force || s_photo_attr.dateStamp != pstPhotoAttr->dateStamp) {
        if (pstPhotoAttr->dateStamp == TIMESTAMP_ON) {
            INFO("enable photo marker\n");
            marker_enable(PMARK);
            sprintf((char *)overlay_font.ttf_name, "arial");
            overlay_font.font_color = 0x00ffffff;
            marker_set_mode(PMARK, "auto", "%t%Y/%M/%D  %H:%m:%S", &overlay_font);
        } else {
            INFO("diable photo marker\n");
            marker_disable(PMARK);
        }
        s_photo_attr.dateStamp = pstPhotoAttr->dateStamp;
    }
#endif

    INFO("leave func %s \n", __func__);
    return 0;
}

int cfg_setConfigAttr(const Inft_CONFIG_Mode_Attr *pstConfigAttr)
{
    INFO("enter func %s \n", __func__);
    static Inft_CONFIG_Mode_Attr s_config_attr;

    if( NULL == pstConfigAttr){
        INFO("pstConfigAttr is NULL");
        return ERROR_NULL;
    }

    if(s_config_attr.mirror != pstConfigAttr->mirror) {
        if (camera_set_mirror(FCAM, (enum cam_mirror_state)(pstConfigAttr->mirror))) {
            INFO("%s: camera_set_mirror error\n");
            //return ERROR_SET_FAIL;
        }
        s_config_attr.mirror = pstConfigAttr->mirror;
    }

    INFO("leave func %s \n", __func__);
    return 0;
}

int set_config(config_camera config)
{

    Inft_tune_Attr tune_attr;

    int mode_change = strcasecmp(config.current_mode, config_camera_g->current_mode) != 0;
    INFO("enter func %s, mode_change: %d, mode: %s\n", __func__, mode_change, config.current_mode);

    memcpy(config_camera_g, &config, sizeof(config_camera));

    if ((config_camera_g == NULL) || (camera_spv_g == NULL)) {
        ERROR("config_camera_g or camera_spv_g is NULL \n");
        return ERROR_NULL;
    }

    if (!strcasecmp(config_camera_g->current_mode, "video") ) {
        Inft_Video_Mode_Attr video_attr;
        //videobox_rebind(DCHN, SS1);
        set_video_attr_value(&video_attr);

        if (cfg_setVideoAttr(&video_attr, mode_change)) {
            ERROR("cfg_setVideoAttr error \n");
            return ERROR_SET_FAIL;
        }
        //videobox_rebind(DCHN, SS0);
    } else if (!strcasecmp(config_camera_g->current_mode, "photo")) {
        Inft_PHOTO_Mode_Attr photo_attr;
        //videobox_rebind(DCHN, SS0);
        set_photo_attr_value(&photo_attr);

        if (cfg_setPhotoAttr(&photo_attr, mode_change)) {
            ERROR("cfg_setPhotoAttr error \n");
            return ERROR_SET_FAIL;
        }
        //videobox_rebind(DCHN, SS1);
    } 

    /*static int balance = -1;
    Inft_PHOTO_Mode_Attr photo_attr1;
    set_photo_whitebalance_value(config_camera_g->photo_whitebalance, &photo_attr1);
    if(balance != photo_attr1.tuneAtr.whiteBalance) {
        camera_set_wb(FCAM, photo_attr1.tuneAtr.whiteBalance);
        balance = photo_attr1.tuneAtr.whiteBalance;
    }*/


    Inft_CONFIG_Mode_Attr setup_attr;
    set_setup_attr_value(&setup_attr);

    if (cfg_setConfigAttr(&setup_attr)) {
        ERROR("cfg_setConfigAttr error \n");
        return ERROR_SET_FAIL;
    }

    INFO("leave func %s \n", __func__);
    return 0;

}

int start_preview()
{
    INFO("enter func %s \n", __func__);
    //videobox_rebind(DCHN, SS1);
    //video_enable_channel(DCHN);
    INFO("leave func %s \n", __func__);
    return 0;
}

int stop_preview()
{
    INFO("enter func %s \n", __func__);
	//videobox_rebind(DCHN, G2FCHN);
	//video_disable_channel(PCHN);
	//video_disable_channel(VCHN);
	//video_disable_channel(DCHN);
    INFO("leave func %s \n", __func__);
    return 0;
}

int g_begin_camera = INFT_CAMERA_FRONT;


void set_video_recorderInfo(VRecorderInfo *recorderInfo)
{
    INFO("enter func %s \n", __func__);
	recorderInfo->enable_gps = 0 ;
    recorderInfo->keep_temp = 1;
	//recorderInfo->audio_format.type = AUDIO_CODEC_TYPE_G711A ;
	recorderInfo->audio_format.type = AUDIO_CODEC_TYPE_AAC;

	recorderInfo->audio_format.sample_rate = 16000;
	recorderInfo->audio_format.sample_fmt  = AUDIO_SAMPLE_FMT_S16 ;
	recorderInfo->audio_format.channels    = 2;
//	recorderInfo->audio_format.bit_rate    = 0;

    //if(IsLoopingRecord()) {
    if(strlen(config_camera_g->video_looprecording) <= 0 || !strcasecmp(config_camera_g->video_looprecording, "Off")) {
        recorderInfo->time_segment = 60 * 30;
    } else {
	    recorderInfo->time_segment = 60 * atoi(config_camera_g->video_looprecording);
    }
    //} else {
    //}
	recorderInfo->time_backward = 0;
	
	strcpy(recorderInfo->av_format, VIDEO_CONTAINER);

	strcpy(recorderInfo->suffix, "%ts_%l_F");
	strcpy(recorderInfo->time_format , "%Y_%m%d_%H%M%S");

	if (access(EXTERNAL_MEDIA_DIR"Video", 0) == -1)
		system("mkdir -p "EXTERNAL_MEDIA_DIR"Video");
	strcpy(recorderInfo->dir_prefix,EXTERNAL_MEDIA_DIR"Video/");

	sprintf(recorderInfo->audio_channel,"default_mic" );
	sprintf(recorderInfo->video_channel, "%s", VCHN );

    INFO("leave %s \n", __func__);
}

int start_video()
{
    INFO("enter func %s \n", __func__);

    video_enable_channel(VCHN);
    //set bitrate control
    video_set_bitrate(VCHN, &LOCAL_MEDIA_BITRATE_INFO);
#if 1
    if(media_init()) {
        ERROR("init g_recorder failed\n");
        return -1;
    }
    g_video_locked = 0;

    vplay_mute_recorder(g_recorder, VPLAY_MEDIA_TYPE_AUDIO, strcasecmp(config_camera_g->video_recordaudio, "On"));

	vplayState.type = VPLAY_EVENT_NONE;
	vplay_start_recorder(g_recorder);
	INFO("recoder ready+_+++\n");	
#endif
    INFO("leave func %s \n", __func__);
    return 0;
}

int stop_video()
{
    INFO("enter func %s \n", __func__);
#if 1
    if(g_recorder != NULL) {
        vplay_stop_recorder(g_recorder);
        vplay_delete_recorder(g_recorder);
        g_recorder = NULL;
    }
#endif
    video_disable_channel(VCHN);

    //g_video_locked = 0;
    struct timeval  tp;
	struct font_attr overlay_font;
	gettimeofday(&tp,NULL);
	INFO("stop_video1 [%u.%u]\n",tp.tv_sec,tp.tv_usec/1000);

    INFO("leave func %s \n", __func__);
    return 0;
}

int start_photo()
{
    INFO("enter func %s, photo:(%dx%d)\n", __func__, photo_attr_g.width, photo_attr_g.height);
    static unsigned int picCount=0;
	static  time_t time_last=0;
	struct tm *tm_p;
	time_t time_t_photo;
	char filename[128];

	int file_fd = -1;
	int ret = 0;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

	time_t_photo = time(NULL);
	tm_p = localtime(&time_t_photo);
	if (access(EXTERNAL_MEDIA_DIR"Photo", 0) == -1) {
		system("mkdir -p "EXTERNAL_MEDIA_DIR"Photo");
    }
	if(time_last==time_t_photo)
	{
	  picCount++;
	}else
	{
	   picCount=0;
	   time_last=time_t_photo;
	}
	
	sprintf(filename, EXTERNAL_MEDIA_DIR"Photo/%04d_%02d%02d_%02d%02d%02d_%04d.jpg", tm_p->tm_year+1900,
	        tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec,picCount);

    ret = take_picture(filename, photo_attr_g.width, photo_attr_g.height);
    INFO("leave func %s \n", __func__);
    return ret;
}

int take_picture(char *filename, int width, int height)
{
    INFO("enter func %s ,photo: (%dx%d) %s\n", __func__, width, height, filename);
#ifdef FULL_RESOLUTION_PHOTO_ENABLE
    if(strcasecmp(config_camera_g->current_mode, "photo") || 
            (photo_attr_g.width == video_attr_g.width && photo_attr_g.height == video_attr_g.height))
#endif
    {
        return get_snapshot(filename);
    }

	int file_fd = -1;
	int ret = 0;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

	if(filename == NULL) {
		ERROR("filename is NULL\n");
		return -1;
    }

    //video_get_resolution("jpeg-in", &h, &w);
    //INFO("Origin width = %d, height = %d\n ", h, w);
    video_set_resolution(PFCHN, width, height);
    ret = camera_snap_one_shot(FCAM);
    INFO("camera_snap_one_shot, ret: %d\n", ret);
    ret = video_get_snap(PFCHN, &buf);
    if(ret < 0) {
        ERROR("video_get_snap failed, ret: %d\n", ret);
        return ret;
    }

	file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if(-1 != file_fd) {
		int writesize = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
		if(writesize <0) {
			ERROR("write snapshot data to file fail, %d\n",ret );
            ret = -1;
		}
		if(ret >= 0) {
			if(access(CALIBRATION_FILE, F_OK) == 0) {
				jpeg_set_exif(filename, CALIBRATION_FILE, 0);
			}
		}
		close(file_fd);
	} else {
		ERROR("create %s file failed  \n", filename);
        ret = -1;
    }

    video_put_snap(PFCHN, &buf);
    usleep(50000);//for one frame not correct when continue snap no delay.
    ret = camera_snap_exit(FCAM);

    if(!ret && !strstr(filename, "/.")) {//should not include hidden files
        SpvAddFile(filename);
    }

    INFO("leave func %s , ret: %d\n", __func__, ret);
    return ret;
}

int get_snapshot(char *filename)
{
    INFO("enter func %s ,photo: %s\n", __func__, filename);

	int file_fd = -1;
	int ret = 0;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

	if(filename == NULL) {
		ERROR("filename is NULL\n");
		return -1;
	}

	ret = video_get_snap(PCHN, &buf);
    if(ret < 0) {
        ERROR("video_get_snap failed, ret: %d\n", ret);
        return ret;
    }

	file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if(-1 != file_fd) {
		int writesize = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
		if(writesize <0) {
			ERROR("write snapshot data to file fail, %d\n",ret );
            ret = -1;
		}
		if(ret >= 0) {
			if(access(CALIBRATION_FILE, F_OK) == 0) {
				jpeg_set_exif(filename, CALIBRATION_FILE, 0);
			}
		}
		close(file_fd);
	} else {
		ERROR("create %s file failed  \n", filename);
        ret = -1;
	}

    if(!ret && !strstr(filename, "/.")) {//should not include hidden files
        SpvAddFile(filename);
    }


    INFO("leave func %s , ret: %d\n", __func__, ret);
    return ret;
}

int stop_photo()
{
    INFO("enter func %s \n", __func__);

    INFO("leave func %s \n", __func__);
    return 0;
}

/*
int start_burst()
{
    INFO("enter func %s \n", __func__);

    int nRet=0;
    struct Inft_Action_Attr stActionAttr;
    memset(&stActionAttr, 0, sizeof(struct Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_BURST;
    stActionAttr.enable = 1;
    nRet = Inft_action_setAttr(&stActionAttr);

    return 0;
}

int display_photo(char *filename)
{
    INFO("enter func %s \n", __func__);

    int nRet=0;
    struct Inft_Action_Attr stActionAttr;
    memset(&stActionAttr, 0, sizeof(struct Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_DISPPIC;
    stActionAttr.enable = 1;
    strcpy(stActionAttr.args.startTime, filename);
    nRet = Inft_action_setAttr(&stActionAttr);

    return 0;
}

int start_playback(char *filename)
{
    INFO("enter func %s \n", __func__);

    int nRet=0;
    struct Inft_Action_Attr stActionAttr;
    memset(&stActionAttr, 0, sizeof(struct Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_PLAYBACK;
    stActionAttr.enable = 1;
    strcpy(stActionAttr.args.startTime, filename);
    nRet = Inft_action_setAttr(&stActionAttr);

    return 0;
}

int stop_playback(char *filename)
{
    INFO("enter func %s \n", __func__);

    int nRet=0;
    struct Inft_Action_Attr stActionAttr;
    memset(&stActionAttr, 0, sizeof(struct Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_PLAYBACK;
    stActionAttr.enable = 0;
    strcpy(stActionAttr.args.startTime, filename);
    nRet = Inft_action_setAttr(&stActionAttr);

    return 0;
}

int error_callback(int error)
{
    INFO("enter func %s \n", __func__);

    INFO("leave func %s \n", __func__);
    return 0;
}
*/

static int rename_thumbnail(char *pathname)
{
	int index = 0;
	char *filename = NULL;
    char folder[256] = {0};
	char videoname[128] = {0};
	char newname[256] = {0};
	if (!access(EXTERNAL_MEDIA_DIR"Video/.thumbnail.jpg", F_OK)) {
		filename = strrchr(pathname, '/') + 1;
        strncpy(folder, pathname, filename - pathname);
        strncpy(videoname, filename, strlen(filename) - strlen(".jpg"));
		sprintf(newname, "%s/.%s.jpg", folder, videoname);
		INFO("video thumbnail name is %s\n", newname);
		if(rename(EXTERNAL_MEDIA_DIR"Video/.thumbnail.jpg", newname)) {
			INFO("rename thumbnail fail, errno: %s\n", strerror(errno));
			return -1;
		}
	} else {
		INFO("access thumbnail.jpg error\n");
		return -1;
	}

	return 0;
}

static int collision_rename(char *originname, char *newpathname)
{
    int ret = 0;
    char tmpname[256] = {0};
    if(realpath(originname, tmpname) == NULL) {
        ERROR("invalid name, %s\n", originname);
        return -1;
    }

    if(g_video_locked) {
        g_video_locked = 0;
        char *filename = strrchr(tmpname, '/') + 1;
        char foldername[256] = {0};
        if(filename == NULL) {
            ERROR("invalid name, file: %s\n", tmpname);
            return -1;
        }
        sprintf(foldername, "%s%s/", EXTERNAL_MEDIA_DIR, LOCK_DIR);
        sprintf(newpathname, "%sLOCK_%s", foldername, filename);
        INFO("lock video name: %s\n", newpathname);
        if(access(foldername, F_OK)) {
            ret = MakeDirRecursive(foldername);
        }
        ret = rename(originname, newpathname);
        if(ret) {
            ERROR("rename locked video failed, d: %s, errno: %s\n", newpathname, strerror(errno));
            strcpy(newpathname, tmpname);
            ret = -1;
        }
    } else {
        strcpy(newpathname, tmpname);
    }
    return ret;
}

void vplay_event_handler(char *name, void *args)
{

	if((NULL == name) || (NULL == args)) {
		INFO("invalid parameter!\n");
		return;
	}

    if(!strcmp(name, EVENT_VPLAY)) {
        memcpy(&vplayState, (vplay_event_info_t *)args, sizeof(vplay_event_info_t));
        INFO("vplay event type: %d, buf:%s\n", vplayState.type, vplayState.buf);

        switch (vplayState.type) {
            case VPLAY_EVENT_NEWFILE_START:
                vplayState.type = VPLAY_EVENT_NONE;
                if (!access(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg", F_OK))
                    remove(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg");
                get_snapshot(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg");
                break;

            case VPLAY_EVENT_NEWFILE_FINISH:
                {
                    char pathname[256] = {0};
                    vplayState.type = VPLAY_EVENT_NONE;
                    strcpy(pathname, vplayState.buf);
                    collision_rename(vplayState.buf, pathname);
                    rename_thumbnail(pathname);
                    SpvAddFile(pathname);
                    break;
                }
            case VPLAY_EVENT_VIDEO_ERROR:
            case VPLAY_EVENT_AUDIO_ERROR:
            case VPLAY_EVENT_EXTRA_ERROR:
            case VPLAY_EVENT_INTERNAL_ERROR:
                if(camera_spv_g != NULL && camera_spv_g->camera_error_callback != NULL) {
                    camera_spv_g->camera_error_callback(CAMERA_ERROR_INTERNAL);
                }
                break;
            case VPLAY_EVENT_DISKIO_ERROR:
                if(camera_spv_g != NULL && camera_spv_g->camera_error_callback != NULL) {
                    camera_spv_g->camera_error_callback(CAMERA_ERROR_DISKIO);
                }
                break;
            default:
                break;
        }
    }
}

int media_init()
{
    if(g_recorder != NULL) {
        vplay_delete_recorder(g_recorder);
        g_recorder = NULL;
    }

    memset(&recorderInfo ,0,sizeof(VRecorderInfo));
    set_video_recorderInfo(&recorderInfo);
    g_recorder = vplay_new_recorder(&recorderInfo);
    if(g_recorder == NULL){
        ERROR("create g_recorder handler error\n");
        return -1;
    }

	if(access(CALIBRATION_FILE, F_OK) == 0) {
		INFO("####set recorder udta data ->%s\n",CALIBRATION_FILE);
		vplay_set_recorder_udta(g_recorder, CALIBRATION_FILE, 0);
	}


    INFO("media_init succeed\n");
	return 0;
}

int disable_all_channel()
{
    video_disable_channel(VCHN);
    video_disable_channel(VPCHN);
    //video_disable_channel(PCHN);
}

camera_spv *camera_create(config_camera config)
{
    INFO("enter func %s \n", __func__);
    camera_spv_g = (camera_spv*)malloc(sizeof(camera_spv));
    memset(camera_spv_g, 0, sizeof(camera_spv));
    config_camera_g = (config_camera*)malloc(sizeof(config_camera));
    memset(config_camera_g, 0, sizeof(config_camera));
    //memcpy(config_camera_g, &config, sizeof(config_camera));

    camera_spv_g->config = config_camera_g;

    camera_spv_g->init_camera = init_camera;
    camera_spv_g->release_camera = release_camera;

    camera_spv_g->set_mode = set_mode;
    camera_spv_g->set_config = set_config;

    camera_spv_g->start_preview = start_preview;
    camera_spv_g->stop_preview = stop_preview;

    camera_spv_g->start_video = start_video;
    camera_spv_g->stop_video = stop_video;

    camera_spv_g->start_photo = start_photo;
    camera_spv_g->take_picture = take_picture;
    camera_spv_g->stop_photo = stop_photo;
    camera_spv_g->camera_error_callback = NULL;

    disable_all_channel();
    set_config(config);

	memset(&vplayState, 0, sizeof(vplay_event_info_t));
	event_register_handler(EVENT_VPLAY, 0, vplay_event_handler);

    INFO("leave func %s \n", __func__);
    return camera_spv_g;
}

void camera_destroy(camera_spv *camera)
{
    INFO("enter func %s \n", __func__);
    free(config_camera_g);
    free(camera_spv_g);

    config_camera_g = NULL;
    camera_spv_g = NULL;
    INFO("leave func %s \n", __func__);
}

void register_camera_callback(camera_spv *camera_spv_p, camera_error_callback_p callback)
{
    INFO("enter func %s \n", __func__);
    camera_spv_p->camera_error_callback = callback;
    INFO("leave %s \n", __func__);
}

int rear_camera_inserted(void)
{
#ifdef GUI_CAMERA_REAR_ENABLE
    if(!access("/dev/video0", F_OK))
        return 1;
#endif

    return 0;
}

int reverse_image_enabled(void)
{
    int ri_fd = open("/sys/devices/virtual/switch/p2g/state", O_RDONLY);
    char buf[16] = {0};
    int reverse_iamge_status = 0;
    if(ri_fd > 0) {
        int size = read(ri_fd, buf, 16);
        if(size > 0) {
            reverse_iamge_status = atoi(buf) == 1 ? 1 : 0;
        }
        close(ri_fd);
    }
    return reverse_iamge_status;
}

int reset_preview_stream_params(char *channel)
{
    //set fps
#ifndef PREVIEW_SECOND_WAY
    video_set_bitrate(VPCHN, &LOCAL_MEDIA_BITRATE_INFO);
    //video_set_fps(VPCHN, 25);
#else
    video_set_fps(VPCHN, 20);
    //set bitrate control
    video_set_bitrate(VPCHN, &WIFI_PREVIEW_BITRATE_INFO);
#endif
}

int start_p2p_stream()
{
    if(g_p2p_stream_started) {
        INFO("p2p stream started yet\n");
        return 0;
    }

    //video_set_fps(VPCHN, 20);

    int ret = video_enable_channel(VPCHN);
    if(ret) {
        ERROR("video_enable_channel failed, ret: %d\n", ret);
    }

    //set bitrate control
    //video_set_bitrate(VPCHN, &WIFI_PREVIEW_BITRATE_INFO);
    reset_preview_stream_params(VPCHN);

    g_p2p_stream_started = 1;
    return 0;
}

int stop_p2p_stream()
{
    if(!g_p2p_stream_started) {
        INFO("p2p stream not started\n");
        return 0;
    }
    video_disable_channel(VPCHN);
    g_p2p_stream_started = 0;
    return 0;
}



int get_p2p_video_frame_with_header(void *data, int *size, int *key_frame, int *timestamp, int reset)
{
    if(data == NULL) {
        INFO("data == NULL, return\n");
        return -1;
    }

    if(!g_p2p_stream_started) {
        INFO("p2p stream not started yet, started p2p stream first\n");
        start_p2p_stream();
    }

    static char header[256] = {0};
    static int headersize = 0;

    int ret;
    static struct fr_buf_info framebuf = {0};
    if(g_resolution_changed == RESOLUTION_CHANGING) {
        //INFO("resolution is changing\n");
        usleep(30000);
        return -1;
    }
    if(g_resolution_changed == RESOLUTION_CHANGED || reset || headersize == 0) {
        g_resolution_changed = RESOLUTION_OK;
        fr_INITBUF(&framebuf, NULL);

        //query I frame for the stream
        video_trigger_key_frame(VPCHN);

        ret = get_p2p_video_header(header, &headersize);
        if(ret) {
            ERROR("get p2p header failed, ret: %d\n", ret);
            return -1;
        }
    }
    ret = video_get_frame(VPCHN, &framebuf);
    //INFO("video_get_frame, ret: %d, frame, size: %d, key: %2X, %lld\n", ret, framebuf.size, framebuf.priv, framebuf.timestamp);
    if(ret < 0) {
        INFO("get frame faild, ret: %d\n", ret);
        usleep(2000);
        return -1;
    }

    if(framebuf.priv == VIDEO_FRAME_I) {
        memcpy(data, header, headersize);
    }
    memcpy(data + (framebuf.priv == VIDEO_FRAME_I ? headersize : 0), framebuf.virt_addr, framebuf.size);
    *size = framebuf.size + (framebuf.priv == VIDEO_FRAME_I ? headersize : 0);
    *key_frame = framebuf.priv == VIDEO_FRAME_I;
    *timestamp = framebuf.timestamp;

    video_put_frame(VPCHN, &framebuf);
    return 0;
}

/**
 * 0 means H.264, otherwise means H.265
 **/
int get_p2p_video_stream_type()
{
    int type = P2P_MEDIA_H264;
    if(strstr(VPCHN, "avc")) {
        type = P2P_MEDIA_H264;
    } else if(strstr(VPCHN, "evc")) {
        type = P2P_MEDIA_H265;
    } else if(strstr(VPCHN, "mjpeg")) {
        type = P2P_MEDIA_MJPEG;
    } else {
        type = P2P_MEDIA_H264;
    }
    return type;
}

int confirm_p2p_stream_started()
{
    if(!g_p2p_stream_started) {
        INFO("p2p stream not started yet, started p2p stream first\n");
        return start_p2p_stream();
    }
	return 0;
}

int get_p2p_video_frame(void **data, int *size, int *key_frame, int *timestamp, int reset)
{
    if(!g_p2p_stream_started) {
        INFO("p2p stream not started yet, started p2p stream first\n");
        start_p2p_stream();
    }
    int ret;
    static struct fr_buf_info framebuf = {0};
    static uint64_t last_ts = 0;
    ret = video_get_frame(VPCHN, &framebuf);
    //INFO("video_get_frame, ret: %d, frame, size: %d, key: %d\n", ret, framebuf.size, framebuf.priv);
    if(ret || last_ts == framebuf.timestamp) {
        if(ret < 0)
            INFO("get frame faild, ret: %d\n", ret);
        return -1;
    }
    last_ts = framebuf.timestamp;

    *data = malloc(framebuf.size);
    if(*data == NULL) {
        INFO("malloc data failed, data == NULL\n");
        return -1;
    }
    memcpy(*data, framebuf.virt_addr, framebuf.size);
    *size = framebuf.size;
    *key_frame = framebuf.priv == VIDEO_FRAME_I;
    *timestamp = framebuf.timestamp;

    video_put_frame(VPCHN, &framebuf);
    return 0;
}

int get_p2p_video_header(void *data, int *size)
{
    if(!g_p2p_stream_started) {
        INFO("p2p stream not started yet, started p2p stream first\n");
        start_p2p_stream();
    }


    int headersize = video_get_header(VPCHN, data, 180);
    if(headersize <= 0) {
        ERROR("p2p stream get header failed, ret: %d\n", headersize);
        return -1;
    }

    *size = headersize;

    return 0;
}
extern FileList g_media_files[MEDIA_COUNT];

 int findPlaybackFile(char* request, char* fileName,MEDIA_TYPE fileType)
{
   int i = 0;
   
   if (!request || !fileName) {
       INFO("p2p request: %x, fileName:%x\n", 
            request, fileName);
       return 0;
   }


       PFileNode pItem = g_media_files[fileType].pHead;
       int j = 0;
       for (j = 0; j<g_media_files[fileType].fileCount; j++) {
          if (NULL == pItem) return 0;
          
          if (strstr(pItem->fileName, request)){
		  	   sprintf(fileName,"%s%s/%s",EXTERNAL_MEDIA_DIR,pItem->folderName,pItem->fileName); 
              return 1;
          }
          pItem=pItem->next;
        }
   

   return 0;
}


int enable_calibration()
{
    //int ret = videobox_repath(CALIBRATION_PATH);
    video_set_snap_quality(CALCHN, 9);
    //video_set_fps(CALCHN, 10);

    /*char url[128] = {0};
    int id = 0;
	int file_fd = -1;
	struct fr_buf_info buf = FR_BUFINITIALIZER;
    char filename[128];
    INFO("enable_calibration\n");
    while(1) {
        sprintf(filename, "/mnt/http/mmc/%d.jpg", id);
        sprintf(url, "http://172.3.0.2/mmc/%d.jpg", id);
        ret = video_get_snap(CALCHN, &buf);
        if(ret < 0) {
            ERROR("video_get_snap failed, ret: %d\n", ret);
            continue;
        }

        id++;
        id = id%10;

        file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if(-1 != file_fd) {
            int writesize = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
            if(writesize <0) {
                ERROR("write snapshot data to file fail, %d\n",ret );
                ret = -1;
            }
            close(file_fd);
        } else {
            ERROR("create %s file failed  \n", filename);
            ret = -1;
        }

        SpvSendSocketMsg("172.3.0.1", 6001, url, strlen(url));
        usleep(100000);
    }*/
    SpvCalibrate("172.3.0.1", 6001, NULL, 0);
    return 0;
}

