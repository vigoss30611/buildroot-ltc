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

#include "libramain.h"
#include "camera_spv.h"
//#include "common_debug.h"
#include "codec_volume.h"
#include <qsdk/videobox.h>
#include <qsdk/vplay.h>
#include <qsdk/codecs.h>
#include <qsdk/event.h>

#define CAMERA_SPV_DEBUG 1
#define DEBUG_INFO printf
#define DEBUG_ERR printf

#define SS0  "ispost0-ss0"
#define SS1  "ispost0-ss1"
#define VCHN "encss0-stream"
#define PCHN "jpeg-out"
#define G2CHN "dec0-stream"
#define G2FCHN "dec0-frame"
#define FSCHN "fs-out"
#define DCHN "display-osd0"
#define FCAM "isp"
#define VMARK "marker0"
#define PMARK "marker0"

const char Ss0[VB_CHN_LEN] = SS0;		//video stream source
const char Ss1[VB_CHN_LEN] = SS1;		//jpeg stream source
const char Vchn[VB_CHN_LEN] = VCHN;		//video stream connect after Ss0
const char Pchn[VB_CHN_LEN] = PCHN;		//jpeg stream connect after Ss1
const char G2chn[VB_CHN_LEN] = G2CHN;	//vplayer ipu for vplayer
const char G2Fchn[VB_CHN_LEN] = G2FCHN;	//vplayer ipu output to connect to display
const char FSchn[VB_CHN_LEN] = FSCHN;	//file source ipu for vplayer
const char Dchn[VB_CHN_LEN] = DCHN;		//display ipu
const char Fcam[VB_CHN_LEN] = FCAM;		//front cam
const char Vmark[VB_CHN_LEN] = VMARK;	//video stream marker
const char Pmark[VB_CHN_LEN] = PMARK;	//jpeg stream marker

static pthread_t camera_callback_thread_id;
static int pthread_status = 0;
struct font_attr overlay_font;

camera_spv *camera_spv_g = NULL;
config_camera *config_camera_g = NULL;
VRecorder *recorder =  NULL;
VRecorderInfo recorderInfo;
vplay_event_info_t *vplayState = NULL;

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

/*
int display_photo(char *filename);
int start_playback(char *filename);
int stop_playback(char *filename);

int error_callback(int error);

*/


int reverse_image_enabled(void);

int get_ambientBright(void)
{
    DEBUG_INFO("enter func %s \n", __func__);
    int ret = -1;
    int brightness = 0;

    if ((ret = camera_get_brightness(Fcam, &brightness)) < 0) {
	DEBUG_ERR("get brightness error \n");
	return -1;
    }

    DEBUG_INFO("ambientBright is %d \n", ret);
    DEBUG_INFO("leave func %s \n", __func__);

    return brightness >= FRONTIER ? 1 : 0;
}

int init_camera()
{
    DEBUG_INFO("enter func %s \n", __func__);
    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int release_camera()
{
    DEBUG_INFO("enter func %s \n", __func__);
    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int set_mode()
{
    DEBUG_INFO("enter func %s \n", __func__);
    DEBUG_INFO("leave func %s \n", __func__);
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
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "1080fhd")) {
	video_attr->width = 1920;
	video_attr->height = 1088;
	video_attr->frameRate = 30;
    } else if (!strcasecmp(config, "720p 60fps")) {
	video_attr->width = 1280;
	video_attr->height = 720;
	video_attr->frameRate = 60;
    } else if (!strcasecmp(config, "720p 30fps")) {
	video_attr->width = 1280;
	video_attr->height = 720;
	video_attr->frameRate = 30;
    } else if (!strcasecmp(config, "wvga")) {
	video_attr->width = 800;
	video_attr->height = 480;
	video_attr->frameRate = 30;
    } else if (!strcasecmp(config, "vga")) {
	video_attr->width = 640;
	video_attr->height = 480;
	video_attr->frameRate = 30;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_looprecording_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_wdr_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->wdr = WDR_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->wdr = WDR_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_ev_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_recordaudio_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->recordaudio = RECORDAUDIO_ON;
//	set_mute(1);
    } else if (!strcasecmp(config, "off")) {
	video_attr->recordaudio = RECORDAUDIO_OFF;
//	set_mute(0);
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_datestamp_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->dateStamp = TIMESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->dateStamp = TIMESTAMP_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_gsensor_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_platestamp_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->platestamp = PLATESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->platestamp = PLATESTAMP_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_platenumber_value(unsigned char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    strcpy(video_attr->plateNumber, config);
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_zoom_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    video_attr->zoom = atof(config + 1) * 10;
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_other_collide_value(char *config, Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || video_attr == NULL)
    {
	DEBUG_ERR("config or video_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	video_attr->collide = COLLIDE_ON;
    } else if (!strcasecmp(config, "off")) {
	video_attr->collide = COLLIDE_OFF;
    }

    DEBUG_INFO("leave func %s \n", __func__);
}

void set_video_attr_value(Inft_Video_Mode_Attr *video_attr)
{
    DEBUG_INFO("enter func %s \n", __func__);
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_capturemode_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    photo_attr->photoModeType = PHOTO_MOD_TYPE_SINGLE;
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_resolution_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

	 if (!strcasecmp(config, "12M")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "10M")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "8M")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "5M")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "3M")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "2MHD")) {
	photo_attr->width = 1920;
	photo_attr->height = 1088;
	} else if (!strcasecmp(config, "VGA")) {
	photo_attr->width = 640;
	photo_attr->height = 480;
	} else if (!strcasecmp(config, "1.3M")) {
	photo_attr->width = 1280;
	photo_attr->height = 720;
	}

    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_sequence_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->sequence = 3;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->sequence = 1;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_quality_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "fine")) {
	photo_attr->quality = PHOTO_QUALITY_HIGH;
    } else if (!strcasecmp(config, "normal")) {
	photo_attr->quality = PHOTO_QUALITY_MIDDLE;
    } else if (!strcasecmp(config, "economy")) {
	photo_attr->quality = PHOTO_QUALITY_LOW;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_sharpness_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "strong")) {
	photo_attr->tuneAtr.sharpness = 255;
    } else if (!strcasecmp(config, "normal")) {
	photo_attr->tuneAtr.sharpness = 128;
    } else if (!strcasecmp(config, "soft")) {
	photo_attr->tuneAtr.sharpness = 0;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_whitebalance_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_color_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "color")) {
	photo_attr->tuneAtr.color = ADVANCED_COLOR_MODE;
    } else if (!strcasecmp(config, "black&white")) {
	photo_attr->tuneAtr.color = FLAT_COLOR_MODE;
    } else if (!strcasecmp(config, "sepia")) {
	photo_attr->tuneAtr.color = SEPIA_COLOR_MODE;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_isolimit_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_ev_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_antishaking_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->antishaking = ANTISHAKING_ON;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->antishaking = ANTISHAKING_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_datestamp_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    if (!strcasecmp(config, "on")) {
	photo_attr->dateStamp = TIMESTAMP_ON;
    } else if (!strcasecmp(config, "off")) {
	photo_attr->dateStamp = TIMESTAMP_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_zoom_value(char *config, Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || photo_attr == NULL)
    {
	DEBUG_ERR("config or photo_attr is null \n");
    }

    photo_attr->zoom = atof(config + 1) * 10;
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_photo_attr_value(Inft_PHOTO_Mode_Attr *photo_attr)
{
    DEBUG_INFO("enter func %s \n", __func__);
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
    DEBUG_INFO("leave func %s \n", __func__);
}

void show_video_attr(Inft_Video_Mode_Attr video_attr)
{
    DEBUG_INFO("width is %d \n", video_attr.width);
    DEBUG_INFO("height is %d \n", video_attr.height);
    DEBUG_INFO("frameRate is %d \n", video_attr.frameRate);
    DEBUG_INFO("loopRecordTime is %d \n", video_attr.loopRecordTime);
    DEBUG_INFO("wdr is %d \n", video_attr.wdr);
    DEBUG_INFO("EVCompensation is %d \n", video_attr.tuneAtr.EVCompensation);
    DEBUG_INFO("recordaudio is %d \n", video_attr.recordaudio);
    DEBUG_INFO("dateStamp is %d \n", video_attr.dateStamp);
    DEBUG_INFO("gsensor is %d \n", video_attr.gsensor);
    DEBUG_INFO("platestamp is %d \n", video_attr.platestamp);
    DEBUG_INFO("plateNumber is %s \n", video_attr.plateNumber);
    DEBUG_INFO("zoom is %d \n", video_attr.zoom);
    DEBUG_INFO("collide is %d \n", video_attr.collide);
}

void show_photo_attr(Inft_PHOTO_Mode_Attr photo_attr)
{
    DEBUG_INFO("photoModeType is %d \n", photo_attr.photoModeType);
    DEBUG_INFO("megapixels is %d \n", photo_attr.megapixels);
    DEBUG_INFO("sequence is %d \n", photo_attr.sequence);
    DEBUG_INFO("quality is %d \n", photo_attr.quality);
    DEBUG_INFO("sharpness is %d \n", photo_attr.tuneAtr.sharpness);
    DEBUG_INFO("whiteBalance is %d \n", photo_attr.tuneAtr.whiteBalance);
    DEBUG_INFO("color is %d \n", photo_attr.tuneAtr.color);
    DEBUG_INFO("ISOLimit is %d \n", photo_attr.ISOLimit);
    DEBUG_INFO("EVCompensation is %d \n", photo_attr.tuneAtr.EVCompensation);
    DEBUG_INFO("antishaking is %d \n", photo_attr.antishaking);
    DEBUG_INFO("dateStamp is %d \n", photo_attr.dateStamp);
    DEBUG_INFO("zoom is %d \n", photo_attr.zoom);
}

void show_setup_attr(Inft_CONFIG_Mode_Attr setup_attr)
{
    DEBUG_INFO("tvmode is %d \n", setup_attr.tvmode);
    DEBUG_INFO("frequency is %d \n", setup_attr.frequency);
    DEBUG_INFO("irled is %d \n", setup_attr.irled);
    DEBUG_INFO("mirror is %d \n", setup_attr.mirror);
}

void set_setup_tvmode_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || setup_attr == NULL)
    {
	DEBUG_ERR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "ntsc")) {
	setup_attr->tvmode = TV_MODE_NTSC;
    } else if (!strcasecmp(config, "pal")) {
	setup_attr->tvmode = TV_MODE_PAL;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_setup_frequency_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || setup_attr == NULL)
    {
	DEBUG_ERR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "50hz")) {
	setup_attr->frequency = FREQUENCY_50HZ;
    } else if (!strcasecmp(config, "60hz")) {
	setup_attr->frequency = FREQUENCY_60HZ;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_setup_irled_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || setup_attr == NULL)
    {
	DEBUG_ERR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "auto")) {
	setup_attr->irled = IR_LED_AUTO;
    } else if (!strcasecmp(config, "on")) {
	setup_attr->irled = IR_LED_ON;
    } else if (!strcasecmp(config, "off")) {
	setup_attr->irled = IR_LED_OFF;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_setup_imagerotation_value(char *config, Inft_CONFIG_Mode_Attr *setup_attr)
{
    DEBUG_INFO("enter func %s, config is %s \n", __func__, config);
    if (config == NULL || setup_attr == NULL)
    {
	DEBUG_ERR("config or setup_attr is null \n");
    }

    if (!strcasecmp(config, "off")) {
	setup_attr->mirror = NONE;
    } else if (!strcasecmp(config, "on")) {
	setup_attr->mirror = FLIP;//MIRROR;
    }
    DEBUG_INFO("leave func %s \n", __func__);
}

void set_setup_attr_value(Inft_CONFIG_Mode_Attr *setup_attr)
{
    DEBUG_INFO("enter func %s \n", __func__);
    set_setup_tvmode_value(config_camera_g->setup_tvmode, setup_attr);
    set_setup_frequency_value(config_camera_g->setup_frequency, setup_attr);
    set_setup_irled_value(config_camera_g->setup_irled, setup_attr);
    set_setup_imagerotation_value(config_camera_g->setup_imagerotation, setup_attr);
    DEBUG_INFO("leave func %s \n", __func__);
}

int cfg_setVideoAttr(const Inft_Video_Mode_Attr* pstvideo_attr)
{
    	DEBUG_INFO("enter func %s \n", __func__);

	struct cam_zoom_info zoom_info;

	if( NULL == pstvideo_attr){
		DEBUG_INFO("pstvideo_attr is NULL");
		return ERROR_NULL;
	}

	DEBUG_INFO("video set resolution %dx%d \n", pstvideo_attr->width, pstvideo_attr->height);
	if (video_set_resolution(Vchn, pstvideo_attr->width, pstvideo_attr->height)) {
		DEBUG_INFO("%s: video set resolution %dx%d error\n", __func__, pstvideo_attr->width, pstvideo_attr->height);
		return ERROR_SET_FAIL;
	}
	
/*
	if (camera_awb_enable(Fcam, pstvideo_attr->tuneAtr.whiteBalance)) {
		DEBUG_INFO("%s: camera_awb_enable error\n", __func__);
		return ERROR_SET_FAIL;
	}
*/
	
	DEBUG_INFO("color is %d\n", pstvideo_attr->tuneAtr.color);
	if (camera_monochrome_enable(Fcam, pstvideo_attr->tuneAtr.color)) {
		DEBUG_INFO("%s: camera_monochrome_enable error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("sharpness is %d\n", pstvideo_attr->tuneAtr.sharpness);
	if (camera_set_sharpness(Fcam, pstvideo_attr->tuneAtr.sharpness)) {
		DEBUG_INFO("%s: camera_set_sharpness error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("EVCompensation is %d\n", pstvideo_attr->tuneAtr.EVCompensation);
	if (camera_ae_enable(Fcam, 1) || camera_set_exposure(Fcam, pstvideo_attr->tuneAtr.EVCompensation)) {
		DEBUG_INFO("%s: camera_set_ae error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("wdr is %d\n", pstvideo_attr->wdr);
	if (camera_wdr_enable(Fcam, pstvideo_attr->wdr)) {
		DEBUG_INFO("%s: camera_set_wdr error\n", __func__);
		return ERROR_SET_FAIL;
	}

	if (pstvideo_attr->dateStamp == TIMESTAMP_ON) {
		DEBUG_INFO("enable video marker\n");
		marker_enable(Vmark);
		sprintf((char *)overlay_font.ttf_name, "arial");
		overlay_font.font_color = 0x00ffffff;
		marker_set_mode(Vmark, "auto", "%t%Y/%M/%D  %H:%m:%S", &overlay_font);
	} else {
		DEBUG_INFO("diable video marker\n");
		marker_disable(Vmark);
	}
	/*ISOLimit*/

	/*reverse for plateNumber*/

	/*reverse for timestamp*/

	/*zoom setting*/
	/*
	zoom_info.a_multiply = pstvideo_attr->zoom;
	zoom_info.d_multiply = pstvideo_attr->zoom;
	if (camera_set_zoom(Fcam, &zoom_info)) {
		DEBUG_INFO("%s: camera_set_zoom error\n", __func__);
		return ERROR_SET_FAIL;
	}
	*/

	DEBUG_INFO("leave func %s \n", __func__);
	return 0;
}

int cfg_setPhotoAttr(const Inft_PHOTO_Mode_Attr *pstPhotoAttr)
{
	DEBUG_INFO("enter func %s \n", __func__);

	if( NULL == pstPhotoAttr){
		DEBUG_INFO("pstPhotoAttr is NULL");
		return ERROR_NULL;
	}

	if (camera_set_wb(Fcam, pstPhotoAttr->tuneAtr.whiteBalance)) {
		DEBUG_INFO("%s: camera_awb_enable error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("color is %d\n", pstPhotoAttr->tuneAtr.color);
	if (camera_monochrome_enable(Fcam, pstPhotoAttr->tuneAtr.color)) {
		DEBUG_INFO("%s: camera_monochrome_enable error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("sharpness is %d \n", pstPhotoAttr->tuneAtr.sharpness);
	if (camera_set_sharpness(Fcam, pstPhotoAttr->tuneAtr.sharpness)) {
		DEBUG_INFO("%s: camera_set_sharpness error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("ISOLimit is %d \n", pstPhotoAttr->ISOLimit);
	if (camera_set_ISOLimit(Fcam, pstPhotoAttr->ISOLimit)) {
		DEBUG_INFO("%s: camera_set_ISOLimit error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("EVCompensation is %d \n", pstPhotoAttr->tuneAtr.EVCompensation);
	if (camera_ae_enable(Fcam, 1) || camera_set_exposure(Fcam, pstPhotoAttr->tuneAtr.EVCompensation)) {
		DEBUG_INFO("%s: camera_set_ae error\n", __func__);
		return ERROR_SET_FAIL;
	}

	DEBUG_INFO("photo set resolution %dx%d \n", pstPhotoAttr->width, pstPhotoAttr->height);
	if (video_set_resolution(Pchn, pstPhotoAttr->width, pstPhotoAttr->height)) {
		DEBUG_INFO("%s: photo set resolution %dx%d error\n", __func__, pstPhotoAttr->width, pstPhotoAttr->height);
		return ERROR_SET_FAIL;
	}

	if (!video_set_snap_quality(Pchn, pstPhotoAttr->quality)) {
		DEBUG_INFO("%s: video_set_snap_quality error\n", __func__);
		return ERROR_SET_FAIL;
	}


	if (pstPhotoAttr->dateStamp == TIMESTAMP_ON) {
		DEBUG_INFO("enable photo marker\n");
		marker_enable(Pmark);
		sprintf((char *)overlay_font.ttf_name, "arial");
		overlay_font.font_color = 0x00ffffff;
		marker_set_mode(Pmark, "auto", "%t%Y/%M/%D  %H:%m:%S", &overlay_font);
	} else {
		DEBUG_INFO("diable photo marker\n");
		marker_disable(Pmark);
	}

    	DEBUG_INFO("leave func %s \n", __func__);
	return 0;
}

int cfg_setConfigAttr(const Inft_CONFIG_Mode_Attr *pstConfigAttr)
{
    	DEBUG_INFO("enter func %s \n", __func__);

	if( NULL == pstConfigAttr){
		DEBUG_INFO("pstConfigAttr is NULL");
		return ERROR_NULL;
	}

	if (camera_set_mirror(Fcam, (enum cam_mirror_state)(pstConfigAttr->mirror))) {
		DEBUG_INFO("%s: camera_set_mirror error\n");
		return ERROR_SET_FAIL;
	}

    	DEBUG_INFO("leave func %s \n", __func__);
	return 0;
}

int set_config(config_camera config)
{

    Inft_tune_Attr tune_attr;
    DEBUG_INFO("enter func %s \n", __func__);


    memcpy(config_camera_g, &config, sizeof(config_camera));

    if ((config_camera_g == NULL) || (camera_spv_g == NULL)) {
	DEBUG_ERR("config_camera_g or camera_spv_g is NULL \n");
	return ERROR_NULL;
    }

    if (!strcasecmp(config_camera_g->current_mode, "video") ) {
		Inft_Video_Mode_Attr video_attr;
		videobox_rebind(Dchn, Ss1);
		set_video_attr_value(&video_attr);
	
	if (cfg_setVideoAttr(&video_attr)) {
	    DEBUG_ERR("cfg_setVideoAttr error \n");
	    return ERROR_SET_FAIL;
	}
		videobox_rebind(Dchn, Ss0);
#if CAMERA_SPV_DEBUG
	    DEBUG_INFO("after set_video_attr_value \n");
	    show_video_attr(video_attr);
#endif
    } else if (!strcasecmp(config_camera_g->current_mode, "photo")) {
		Inft_PHOTO_Mode_Attr photo_attr;
		videobox_rebind(Dchn, Ss0);
	    set_photo_attr_value(&photo_attr);

	if (cfg_setPhotoAttr(&photo_attr)) {
	    DEBUG_ERR("cfg_setPhotoAttr error \n");
	    return ERROR_SET_FAIL;
	}
		videobox_rebind(Dchn, Ss1);
#if CAMERA_SPV_DEBUG
	    DEBUG_INFO("after set_photo_attr_value \n");
	    show_photo_attr(photo_attr);
#endif
    } 

	Inft_CONFIG_Mode_Attr setup_attr;
	set_setup_attr_value(&setup_attr);
	
	if (cfg_setConfigAttr(&setup_attr)) {
	    DEBUG_ERR("cfg_setConfigAttr error \n");
	    return ERROR_SET_FAIL;
	}

#if CAMERA_SPV_DEBUG
	DEBUG_INFO("after set_setup_attr_value \n");
	show_setup_attr(setup_attr);
#endif

    DEBUG_INFO("leave func %s \n", __func__);
    return 0;

}

int start_preview()
{
    DEBUG_INFO("enter func %s \n", __func__);
	videobox_rebind(Dchn, Ss1);
	//video_enable_channel(Dchn);
    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int stop_preview()
{
    DEBUG_INFO("enter func %s \n", __func__);
	videobox_rebind(Dchn, G2Fchn);
	video_disable_channel(Pchn);
	video_disable_channel(Vchn);
	video_disable_channel(Dchn);
    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

void *camera_callback_thread(void *args)
{
    DEBUG_INFO("enter func %s \n", __func__);

    Inft_Action_Attr action_attr;
    int state_changed = 0;

    memset(&action_attr, 0, sizeof(action_attr));
    action_attr.mode = INFT_ACTION_MODE_RECORD;


    while(1) {
		/*Inft_action_getAttr(&action_attr);
		if ((ACTION_STATE_ERROR_SD_FULL == action_attr.state) && !state_changed) {
		    DEBUG_INFO("ACTION_STATE_ERROR_SD_FULL \n");
		    state_changed = 1;
		    if (NULL != camera_spv_g->camera_error_callback)
			camera_spv_g->camera_error_callback(STATUS_SD_FULL);
		} else if((ACTION_STATE_ERROR_NO_SD == action_attr.state) && !state_changed) {
		    DEBUG_INFO("ACTION_STATE_ERROR_NO_SD \n");
		    state_changed = 1;
		    if (NULL != camera_spv_g->camera_error_callback)
			camera_spv_g->camera_error_callback(STATUS_NO_SD);
		} else if((ACTION_STATE_ERROR == action_attr.state) && !state_changed) {
		    DEBUG_INFO("ACTION_STATE_ERROR \n");
		    state_changed = 1;
		    if (NULL != camera_spv_g->camera_error_callback)
			camera_spv_g->camera_error_callback(STATUS_ERROR);*/

		pthread_testcancel();
		usleep(100*1000);
    }
    DEBUG_INFO("leave %s \n", __func__);
}

int g_begin_camera = INFT_CAMERA_FRONT;


void set_video_recorderInfo(VRecorderInfo *recorderInfo)
{
	struct tm *tm_p;
	time_t time_t_video;
	char temp[] =  "%ts_%c";
	time_t_video= time(NULL);
	tm_p = localtime(&time_t_video);
    	DEBUG_INFO("enter func %s \n", __func__);
	recorderInfo->enable_gps = 0 ;
	recorderInfo->audio_format.type = AUDIO_CODEC_G711A ;
//	recorderInfo->audio_format.type = AUDIO_CODEC_PCM ;

	recorderInfo->audio_format.sample_rate = 8000;
	recorderInfo->audio_format.sample_fmt   = AUDIO_SAMPLE_FMT_S16 ;
	recorderInfo->audio_format.channels    = 2;
//	recorderInfo->audio_format.bit_rate    = 0;

	recorderInfo->time_segment = 60;
	recorderInfo->time_backward = 0;
	
	strcat(recorderInfo->av_format,"mkv");
	sprintf(recorderInfo->suffix, "%04d_", tm_p->tm_year+1900);
	strcat(recorderInfo->suffix,temp);
	if (access(EXTERNAL_MEDIA_DIR"/Video", 0) == -1)
		system("mkdir -p "EXTERNAL_MEDIA_DIR"/Video");
	strcat(recorderInfo->dir_prefix,EXTERNAL_MEDIA_DIR"/Video");
	printf("use local para\n");

	sprintf(recorderInfo->video_channel, "%s", Vchn );
	sprintf(recorderInfo->audio_channel,"default_mic" );
	strcat(recorderInfo->time_format , "%m%d_%H%M%S");

    	DEBUG_INFO("leave %s \n", __func__);
}

int start_video()
{
    	DEBUG_INFO("enter func %s \n", __func__);

	vplayState->type = VPLAY_EVENT_NONE;
	vplay_start_recorder(recorder);
	printf("recoder ready+_+++\n");	
	//vplay_show_statistics(recorder);

    int ret = 0;
    pthread_attr_t attr;
    if (!pthread_status) {
	pthread_status = 1;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	DEBUG_INFO("pthread_create, start_video\n");
	ret = pthread_create(&camera_callback_thread_id, &attr, camera_callback_thread, NULL);



	pthread_attr_destroy(&attr);

	if (0 != ret) {
	    DEBUG_ERR("pthread_create error \n");
	    return ERROR_CREATE_THREAD;
	}

	usleep(1000);
    }
    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int stop_video()
{
    	DEBUG_INFO("enter func %s \n", __func__);
	vplay_stop_recorder(recorder);

    	struct timeval  tp;
	struct font_attr overlay_font;
	gettimeofday(&tp,NULL);
	printf("stop_video1 [%u.%u]\n",tp.tv_sec,tp.tv_usec/1000);

    int ret = 0;
    if (pthread_status) {
		pthread_status = 0;
		ret =  pthread_cancel(camera_callback_thread_id);
		if (0 != ret) {
		    DEBUG_ERR("pthread_cancel error \n");
		    return ERROR_CANCEL_THREAD;
		}

    }

    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int start_photo()
{
    DEBUG_INFO("enter func %s \n", __func__);

	struct tm *tm_p;
	time_t time_t_photo;
	char cmd[128];

	int file_fd = -1;
	int ret;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

	time_t_photo = time(NULL);
	tm_p = localtime(&time_t_photo);
	if (access(EXTERNAL_MEDIA_DIR"/Photo", 0) == -1)
		system("mkdir -p "EXTERNAL_MEDIA_DIR"/Photo");
	sprintf(cmd, EXTERNAL_MEDIA_DIR"/Photo/%04d_%02d%02d_%02d%02d%02d_%04d.jpg", tm_p->tm_year+1900,
	tm_p->tm_mon, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, GetPhotoCount());

	video_get_snap(Pchn, &buf);
	
	file_fd = open(cmd, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	
	if(-1 != file_fd) {
		ret = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
		if(ret <0) {
			printf("write snapshot data to file fail, %d\n",ret );
		}
		close(file_fd);
	} else {
		printf("create %s file failed\n", cmd);
	}


    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int start_photo_with_name(char *filename)
{
    DEBUG_INFO("enter func %s \n", __func__);

	int file_fd = -1;
	int ret;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

	if(filename == NULL) {
		printf("filename is NULL\n");
		return -1;
	}

	video_get_snap(Pchn, &buf);
	file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	
	if(-1 != file_fd) {
		ret = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
		if(ret <0) {
			printf("write snapshot data to file fail, %d\n",ret );
		}
		close(file_fd);
	} else {
		printf("create %s file failed  \n", filename);
	}

    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

int stop_photo()
{
    DEBUG_INFO("enter func %s \n", __func__);

    /*
    int nRet=0;
    struct Inft_Action_Attr stActionAttr;
    memset(&stActionAttr, 0, sizeof(struct Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_SINGLE;
    stActionAttr.enable = 0;
    nRet = Inft_action_setAttr(&stActionAttr);
    */

    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}

/*
int start_burst()
{
    DEBUG_INFO("enter func %s \n", __func__);

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
    DEBUG_INFO("enter func %s \n", __func__);

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
    DEBUG_INFO("enter func %s \n", __func__);

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
    DEBUG_INFO("enter func %s \n", __func__);

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
    DEBUG_INFO("enter func %s \n", __func__);

    DEBUG_INFO("leave func %s \n", __func__);
    return 0;
}
*/
static int rename_thumbnail()
{
	int index = 0;
	char *thumbnail = NULL;
	char videoname[128];
	char buf[64];

	if (!access(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg", F_OK)) {
		thumbnail = strstr(vplayState->buf, "/Video");
		index = strstr(thumbnail, ".mkv") - thumbnail - 7;
		memset(buf, 0, 64);
		memcpy(buf, thumbnail+7, index);
		sprintf(videoname, EXTERNAL_MEDIA_DIR"/Video/.%s.jpg", buf);
		DEBUG_INFO("video thumbnail name is %s\n", videoname);
		if(rename(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg", videoname)) {
			DEBUG_INFO("rename thumbnail fail\n");
			return -1;
		}
	} else {
		DEBUG_INFO("access thumbnail.jpg error\n");
		return -1;
	}

	return 0;
}

void vplay_event_handler(char *name, void *args)
{

	if((NULL == name) || (NULL == args)) {
		DEBUG_INFO("invalid parameter!\n");
		return;
	}

	if(!strcmp(name, EVENT_VPLAY)) {
		memcpy(vplayState, (vplay_event_info_t *)args, sizeof(vplay_event_info_t));
		DEBUG_INFO("vplay event type: %d, buf:%s\n",
					vplayState->type, vplayState->buf);
	}
	switch (vplayState->type) {
		case VPLAY_EVENT_NEWFILE_START:
			vplayState->type = VPLAY_EVENT_NONE;
			if (!access(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg", F_OK))
				remove(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg");
			start_photo_with_name(EXTERNAL_MEDIA_DIR"/Video/.thumbnail.jpg");
			break;

		case VPLAY_EVENT_NEWFILE_FINISH:
			vplayState->type = VPLAY_EVENT_NONE;
			rename_thumbnail();
			break;

		default:
			break;
	}
}

int media_init()
{
    memset(&recorderInfo ,0,sizeof(VRecorderInfo));
    set_video_recorderInfo(&recorderInfo);
    recorder = vplay_new_recorder(&recorderInfo);
    if(recorder == NULL){
		printf("create recorder handler error\n");
		return -1;
    }

	vplayState = (vplay_event_info_t *)malloc(sizeof(vplay_event_info_t));
	memset(vplayState, 0, sizeof(vplay_event_info_t));
	event_register_handler(EVENT_VPLAY, 0, vplay_event_handler);

	return 0;
}

camera_spv *camera_create(config_camera config)
{
    DEBUG_INFO("enter func %s \n", __func__);
    camera_spv_g = (camera_spv*)malloc(sizeof(camera_spv));
    config_camera_g = (config_camera*)malloc(sizeof(config_camera));
    memcpy(config_camera_g, &config, sizeof(config_camera));

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
    camera_spv_g->start_photo_with_name = start_photo_with_name;
    camera_spv_g->stop_photo = stop_photo;
    camera_spv_g->camera_error_callback = NULL;

	if(media_init()) {
		DEBUG_INFO("media_init fail\n");
	}

    DEBUG_INFO("leave func %s \n", __func__);
    return camera_spv_g;
}

void camera_destroy(camera_spv *camera)
{
    DEBUG_INFO("enter func %s \n", __func__);
    free(config_camera_g);
    free(camera_spv_g);
	free(vplayState);

    config_camera_g = NULL;
    camera_spv_g = NULL;
	vplayState = NULL;
    DEBUG_INFO("leave func %s \n", __func__);
}

void register_camera_callback(camera_spv *camera_spv_p, camera_error_callback_p callback)
{
    DEBUG_INFO("enter func %s \n", __func__);
    camera_spv_p->camera_error_callback = callback;
    DEBUG_INFO("leave %s \n", __func__);
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
