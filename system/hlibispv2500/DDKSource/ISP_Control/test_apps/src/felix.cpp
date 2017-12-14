/**
******************************************************************************
@file felix.cpp

@brief Example on how to run ISPC library with a forever loop capturing from a
 sensor implemented in Sensor API

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/

#include <dyncmd/commandline.h>

#include <iostream>
#include <termios.h>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef INFOTM_ISP
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <math.h>
#endif //INFOTM_ISP

#include <ispc/Pipeline.h>
#include <ci/ci_api.h>
#include <mc/module_config.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>

// control algorithms
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlAF.h>
#include <ispc/TemperatureCorrection.h>

// modules access
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleWBC.h>
#include <ispc/ModuleLSH.h>
#include <ispc/ModuleESC.h>
#include "ispc/ModuleTNM.h"


#include <sstream>
#include <ispc/Save.h>

#include <sensorapi/sensorapi.h> // to get sensor modes

#ifdef EXT_DATAGEN
#include <sensors/dgsensor.h>
#endif
#include <sensors/iifdatagen.h>

#include <pthread.h>

#include "isp_av_binder.h"

#ifdef ROTATION
#include "InfotmMedia.h"
#include "IM_buffallocapi.h"
#include "ppapi.h"
#endif 

#include "semaphore.h"
#define DPF_KEY 'e'
#define LSH_KEY 'r'
#define ADAPTIVETNM_KEY 't'
#define LOCALTNM_KEY 'y'
#define AWB_KEY 'u'
#define DNSISO_KEY 'i'
#define LBC_KEY 'o'
#define AUTOFLICKER_KEY 'p'

#define BRIGHTNESSUP_KEY '+'
#define BRIGHTNESSDOWN_KEY '-'
#define FOCUSCLOSER_KEY ','
#define FOCUSFURTHER_KEY '.'
#define FOCUSTRIGGER_KEY 'm'

#define SAVE_ORIGINAL_KEY 'a'
#define SAVE_RGB_KEY 's'
#define SAVE_YUV_KEY 'd'
#define SAVE_DE_KEY 'f'
#define SAVE_RAW2D_KEY 'g'
#define SAVE_HDR_KEY 'h'

// auto white balance controls
#define AWB_RESET '0'
#define AWB_FIRST_DIGIT 1
#define AWB_LAST_DIGIT 9
// from 1 to 9 used to select one CCM

#define EXIT_KEY 'x'

#define FPS_COUNT_MULT 1.5 ///< @brief used to compute the number of frames to use for the FPS count (sensor FPS*multiplier) so that the time is more than a second

//#define debug printf
#define debug

#define MAX_LOOP_ARRAY_SIZE   (32)
#define F_NEXT_POS(p)  (((p) + 1) % (MAX_LOOP_ARRAY_SIZE+1))

int g_msg_snd_id = 0;
volatile int g_avs_exist = 0;

pthread_t g_msg_rcv_thread;

static volatile int g_fb_1_clear = 1;
static volatile int g_fb_0_reset = 1;
#define DEBUG_TIME_DIFF

#define SUPPORT_SKIP_FRAMES // added by linyun.xiong @2015-08-20 for support skip some buffer

char OV_Plate_String[8] = {0};
char OV_Longitude_String[11] = {0};
char OV_Latitude_String[11] = {0};
char OV_Speed_String[9] = {0};
int  OV_Time_Flag = 1;

static int release_index = 0;
static int acquire_index = 0;
static int buffer_count = 0;
ISPC::Shot shot[10];

#ifdef SUPPORT_SKIP_FRAMES

static pthread_mutex_t skip_frames_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned int skip_frames = 0;
extern "C" void set_skip_frames_cnt(unsigned int cnt)
{
     pthread_mutex_lock(&skip_frames_lock);
     skip_frames = cnt;
     pthread_mutex_unlock(&skip_frames_lock);
}

void reduce_frames_cnt(void)
{
     pthread_mutex_lock(&skip_frames_lock);
     if (skip_frames > 0)
     {
        skip_frames--;
     }
     pthread_mutex_unlock(&skip_frames_lock);
}

#endif

typedef struct {
    unsigned int wr;
    unsigned int rd;
    void *data[MAX_LOOP_ARRAY_SIZE+1];

    unsigned int stop; // signal to stop ISP pump
    unsigned int imgSize;
} T_LOOP_ARRAY;

static T_LOOP_ARRAY *g_shareMem = NULL;
static unsigned int g_isMipi = 1; //godspeed 0511

extern int AdasProcInit();
extern void AdasPrepareData(void *img_buf, int src_width, int src_height, int src_stride);
extern void AdasResultProc(void *img_Buf, int img_width, int img_height);

static int sharemem_loopIsEmpty(T_LOOP_ARRAY *ptr)
{
    return ((ptr->wr) == (ptr->rd));
}

static int sharemem_loopIsFull(T_LOOP_ARRAY *ptr)
{
    return (F_NEXT_POS(ptr->wr) == (ptr->rd));
}

extern "C" {
// begin added by linyun.xiong @2015-11-03
extern int cfg_shm_getSensorName(char* out_sensor_name);
extern int cfg_shm_getSensorMode(char* out_sensor_mode);
extern int cfg_shm_getSensorLane(char* out_sensor_lane);
extern int cfg_shm_getSensorFps(char* out_sensor_fps);
extern int cfg_shm_getSensorSetupFile(char* out_sensor_setupfile);
extern int cfg_shm_getSensorBufferSize(char* out_sensor_buffersize);
extern int cfg_shm_getSensorAWBMode(char* out_sensor_awbmode);

#define ITEMS_ARGC_VALUE 11
// end added by linyun.xiong @2015-11-03

}

typedef void (*buffer_report) (ISP_AV_BUFFER *isp_buffer);

static buffer_report g_bufReport = NULL;
static volatile bool felixStop = false;
static int fbfd = -1;
static pthread_mutex_t isp_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t sema;
static int release_count = 0;
static int send_count = 0;

/**
 * @return seconds for the FPS calculation
 */
double currTime()
{
    double ret;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    ret = tv.tv_sec + (tv.tv_usec/1000000.0);

    return ret;
}


// begin added by linyun.xiong @2015-07-09 for support isp tuning
//#define INFOTM_ISP_TUNING

#ifdef INFOTM_ISP_TUNING

#include "ispc/ModuleSHA.h"
#include "ispc/ModuleR2Y.h"
#include <ispc/Parameter.h>

#define INVALID_REAL_VAL (-0xA0A0A0A0)
#define CONFIG_QUEUE_SIZE 20

static pthread_mutex_t isp_tuning_lock = PTHREAD_MUTEX_INITIALIZER;

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

// added by linyun.xiong @2015-12-03
#ifdef SUPPORT_CHANGE_RESOULTION
static int sensor_reset_flag = 0;
static int msg_recv_thread_loops_flag = 1;

#if !defined(ISP_SUPPORT_SCALER)
static int cur_resolution = VIDEO_1080P;
static int cur_sensor_mode = 0;


int Inft_Isp_Init_FB();

int Inft_Isp_Init_Msg();

void *Inft_Isp_Rcv_Msg_Callback(void * args);
#endif
#endif

typedef enum __PHOTO_RESOLUTION_UI_VAL
{
    PHOTO_8MP = 0,
    PHOTO_5MP,
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

typedef struct __SettingItem
{
    int Id;
    int Val;
}SettingItem;

typedef struct __UI_ISP_ITEM_
{
    SettingItem UI_Item;
    IMG_FLOAT REAL_VAL;
}UI_ISP_ITEM;

typedef struct __SettingQueueItem
{
    UI_ISP_ITEM item;
    int WaitSettingFlag;
}SettingQueueItem;

typedef struct __SettingGroup
{
    // Resolution group
    SettingItem  VideoResolution; // 0 - 1080P, 1 - 720P, 2 - 360P
    SettingItem  PhotoExtraResolution; // 0 - 8MP, 1 - 5MP

    // White balance
    SettingItem WB; // -1 - AWB enable, 1 - 3000K, 2 - 5500K, 3 - 6500K, 0 - AWB Disable and no fixed initial value

    // Color
    SettingItem Color; // 0 - Advanced Color, 1 - Flat

    // ISO Limit
    SettingItem VideoISOLimit; // 0 - 6400, 1 - 1600, 2 - 400
    SettingItem PhotoISOLimit; // 0 - 800, 1 - 400, 2 - 200, 3 - 100

    // Sharpness
    SettingItem Sharpness; // 0 - high, 1 - Medium, 2 - low(soft)

    // Exposure compensation
    SettingItem EV; // 0 - -2.0, 0 - -1.5, 2 - -1.0, 3 - -1.0
}SettingGroup;

static const UI_ISP_ITEM UI_ISP_MAP[] =
{
    {{VIDEO_RESOLUTION_ID, VIDEO_1080P}, 1080}, 
    {{VIDEO_RESOLUTION_ID, VIDEO_720P}, 720}, 
	{{VIDEO_RESOLUTION_ID, VIDEO_480P}, 480}, 
	{{VIDEO_RESOLUTION_ID, VIDEO_400P}, 400}, 
	{{VIDEO_RESOLUTION_ID, VIDEO_320P}, 320}, 
	{{VIDEO_RESOLUTION_ID, VIDEO_320P}, 240}, 


    {{PHOTO_RESOLUTION_ID, PHOTO_8MP}, 800}, // ??? code not completed
    {{PHOTO_RESOLUTION_ID, PHOTO_5MP}, 500}, // ??? code not completed

    {{WB_ID, AWB_MODE}, 0},
    {{WB_ID, WB_3000K}, 1},
    {{WB_ID, WB_4100K}, 2},
    {{WB_ID, WB_5500K}, 3},
    {{WB_ID, WB_6500K}, 4},
    {{WB_ID, WB_NONE}, -1},
#if defined(ZT202_PROJECT) || defined(BOTAI_PROJECT)
    {{COLOR_ID, ADVANCED_COLOR_MODE}, 1.52f},
#else
    {{COLOR_ID, ADVANCED_COLOR_MODE}, 1.50f},
#endif
    {{COLOR_ID, FLAT_COLOR_MODE}, 0.0f},
    {{COLOR_ID, SEPIA_COLOR_MODE}, 1.0f},

    {{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_AUTO}, 0.0},
    {{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_6400}, 64.0},
    {{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_1600}, 16.0},
    {{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_400}, 4.0},

    {{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_AUTO}, 0.0},
    {{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_800}, 8.0},
    {{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_400}, 4.0},
    {{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_200}, 2.0},
    {{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_100}, 1.0},
#ifdef ZT202_PROJECT
    {{SHARPNESS_ID, HIGH_SHARPNESS}, 0.60},
    {{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.40},
    {{SHARPNESS_ID, LOW_SHARPNESS}, 0.25},
#else
    {{SHARPNESS_ID, HIGH_SHARPNESS}, 0.50},
    {{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.45},
    {{SHARPNESS_ID, LOW_SHARPNESS}, 0.40},
#endif
    {{EV_ID, EV_MINUS_2_0}, -0.30/*-2.0*/},
    {{EV_ID, EV_MINUS_1_5}, -0.20/*-1.5*/},
    {{EV_ID, EV_MINUS_1_0}, -0.10/*-1.0*/},
    {{EV_ID, EV_MINUS_0_5}, -0.05/*-0.5*/},
#if defined(FOR_ZHUOYING_PRJ)
    {{EV_ID, EV_0_0}, -0.05},
    {{EV_ID, EV_0_5}, 0.05/*0.5*/},
    {{EV_ID, EV_1_0}, 0.10/*1.0*/},
    {{EV_ID, EV_1_5}, 0.20/*1.5*/},
    {{EV_ID, EV_2_0}, 0.30/*2.0*/},
#elif defined(ZT202_PROJECT)
    {{EV_ID, EV_0_0}, -0.01},
    {{EV_ID, EV_0_5}, 0.10},
    {{EV_ID, EV_1_0}, 0.20},
    {{EV_ID, EV_1_5}, 0.30},
    {{EV_ID, EV_2_0}, 0.40},
#else
    {{EV_ID, EV_0_0}, 0.030},
    {{EV_ID, EV_0_5}, 0.10/*0.5*/},
    {{EV_ID, EV_1_0}, 0.15/*1.0*/},
    {{EV_ID, EV_1_5}, 0.25/*1.5*/},
    {{EV_ID, EV_2_0}, 0.35/*2.0*/},
#endif
#if defined(ZT201_PROJECT) || defined(ZT202_PROJECT)
	{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_NONE},
	{{MIRROR_FLIP_ID, MIRROR}, SENSOR_FLIP_HORIZONTAL},
	{{MIRROR_FLIP_ID, FLIP}, SENSOR_FLIP_VERTICAL},
	{{MIRROR_FLIP_ID, MIRROR_AND_FLIP}, SENSOR_FLIP_BOTH},
#else
    {{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_BOTH},
    {{MIRROR_FLIP_ID, MIRROR}, SENSOR_FLIP_HORIZONTAL},
    {{MIRROR_FLIP_ID, FLIP}, SENSOR_FLIP_VERTICAL},
    {{MIRROR_FLIP_ID, MIRROR_AND_FLIP}, SENSOR_FLIP_NONE},
#endif
};

const SettingQueueItem Ar0330DefaultSetting[CONFIG_QUEUE_SIZE] =
{
    {{{VIDEO_RESOLUTION_ID, 0}, 0}, 0},
    {{{PHOTO_RESOLUTION_ID, 0}, 0}, 0},
    {{{WB_ID, AWB_MODE}, 0}, 0},
#if defined(ZT202_PROJECT) || defined(BOTAI_PROJECT)
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.52f}, 1},
#else
	{{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.5f}, 1},
#endif
    {{{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_AUTO}, 0}, 1},
    {{{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_AUTO}, 0}, 0},
    {{{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.45}, 1},
    {{{EV_ID, EV_0_0}, 0.00}, 1},
#if defined(ZT201_PROJECT) || defined(ZT202_PROJECT)
    {{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_NONE}, 0},
#elif defined(BOTAI_PROJECT)
    {{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_BOTH}, 1},
#else
	{{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_BOTH}, 0},
#endif
    {{{OV_PLATE_ID1, 0}, 0}, 0},
    {{{OV_PLATE_ID2, 0}, 0}, 0},
    {{{OV_TIME_ID, 0}, 0}, 0},
    {{{ISP_ZOOM_ID, 0}, 0}, 0},
    {{{ISP_EN_WIDTH_ID, 0}, 0}, 0},
    {{{ISP_EN_HEIGHT_ID, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID2, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID2, 0}, 0}, 0},
    {{{ISP_SPEED_ID, 0}, 0}, 0},
};

const SettingQueueItem OvDefaultSetting[CONFIG_QUEUE_SIZE] =
{
    {{{VIDEO_RESOLUTION_ID, 0}, 0}, 0},
    {{{PHOTO_RESOLUTION_ID, 0}, 0}, 0},
    {{{WB_ID, AWB_MODE}, 0}, 0},
#if defined(ZT202_PROJECT) || defined(BOTAI_PROJECT)
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.52f}, 1},
#elif defined(SPORTDV_PROJECT)
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.60f}, 1},
#else
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.50f}, 1},
#endif
    {{{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_AUTO}, 0}, 1},
    {{{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_AUTO}, 0}, 0},
#ifdef ZT202_PROJECT
    {{{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.40}, 1},
#else
    {{{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.45}, 1},
#endif
#ifdef ZT202_PROJECT
    {{{EV_ID, EV_0_0}, -0.01}, 1},
#else
    {{{EV_ID, EV_0_0}, 0.05}, 1},
#endif
#if defined(ZT201_PROJECT) || defined(ZT202_PROJECT)
	{{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_NONE}, 0},
#else
	{{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_BOTH}, 0},
#endif
    {{{OV_PLATE_ID1, 0}, 0}, 0},
    {{{OV_PLATE_ID2, 0}, 0}, 0},
    {{{OV_TIME_ID, 0}, 0}, 0},
    {{{ISP_ZOOM_ID, 0}, 0}, 0},
    {{{ISP_EN_WIDTH_ID, 0}, 0}, 0},
    {{{ISP_EN_HEIGHT_ID, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID2, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID2, 0}, 0}, 0},
    {{{ISP_SPEED_ID, 0}, 0}, 0},
};

#if defined(FOR_ZHUOYING_PRJ)
const SettingQueueItem SonyDefaultSetting[CONFIG_QUEUE_SIZE] =
{
    {{{VIDEO_RESOLUTION_ID, 0}, 0}, 0},
    {{{PHOTO_RESOLUTION_ID, 0}, 0}, 0},
    {{{WB_ID, AWB_MODE}, 0}, 0},
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.50f}, 1},
    {{{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_AUTO}, 0}, 1},
    {{{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_AUTO}, 0}, 0},
    {{{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.45}, 1},
    {{{EV_ID, EV_0_0}, -0.05}, 1},
    {{{MIRROR_FLIP_ID, NONE}, SENSOR_FLIP_BOTH}, 0},
    {{{OV_PLATE_ID1, 0}, 0}, 0},
    {{{OV_PLATE_ID2, 0}, 0}, 0},
    {{{OV_TIME_ID, 0}, 0}, 0},
    {{{ISP_ZOOM_ID, 0}, 0}, 0},
    {{{ISP_EN_WIDTH_ID, 0}, 0}, 0},
    {{{ISP_EN_HEIGHT_ID, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID2, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID2, 0}, 0}, 0},
    {{{ISP_SPEED_ID, 0}, 0}, 0},
};
#endif

static SettingQueueItem SettingQueue[CONFIG_QUEUE_SIZE] =
{
    {{{VIDEO_RESOLUTION_ID, 0}, 0}, 0},
    {{{PHOTO_RESOLUTION_ID, 0}, 0}, 0},
    {{{WB_ID, AWB_MODE}, 0}, 0},
    {{{COLOR_ID, ADVANCED_COLOR_MODE}, 1.50f}, 1},
    {{{VIDEO_ISO_LIMIT_ID, VIDEO_ISO_AUTO}, 0}, 1},
    {{{PHOTO_ISO_LIMIT_ID, PHOTO_ISO_AUTO}, 0}, 0},
    {{{SHARPNESS_ID, MEDIUM_SHARPNESS}, 0.45}, 1},
    {{{EV_ID, EV_0_0}, 0.05}, 1},
    {{{MIRROR_FLIP_ID, NONE}, 3}, 0},
    {{{OV_PLATE_ID1, 0}, 0}, 0},
    {{{OV_PLATE_ID2, 0}, 0}, 0},
    {{{OV_TIME_ID, 0}, 0}, 0},
    {{{ISP_ZOOM_ID, 0}, 0}, 0},
    {{{ISP_EN_WIDTH_ID, 0}, 0}, 0},
    {{{ISP_EN_HEIGHT_ID, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LONGITUDE_ID2, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID1, 0}, 0}, 0},
    {{{ISP_LATITUDE_ID2, 0}, 0}, 0},
    {{{ISP_SPEED_ID, 0}, 0}, 0},
};

#if defined(SUPPORT_CHANGE_RESOULTION)
void Resume_IQ_Setting(void)
{
	int i = 0;
	pthread_mutex_lock(&isp_tuning_lock);
	for (i = 0; i < sizeof(SettingQueue)/sizeof(SettingQueue[0]); i++)
	{
		switch (SettingQueue[i].item.UI_Item.Id)
		{
		case WB_ID:
		case COLOR_ID:
		case VIDEO_ISO_LIMIT_ID:
		case SHARPNESS_ID:
		case EV_ID:
		//case MIRROR_FLIP_ID:
        case VIDEO_RESOLUTION_ID:
			SettingQueue[i].WaitSettingFlag   = 1;
			break;
		default:
			break;
		}
	}
	pthread_mutex_unlock(&isp_tuning_lock);

}
#endif

IMG_FLOAT FindRealValueFromUI(int ConfigId, int UI_Val)
{
    int i = 0;
    IMG_FLOAT ret = INVALID_REAL_VAL;

    for (i = 0; i < sizeof(UI_ISP_MAP)/sizeof(UI_ISP_MAP[0]); i++)
    {
        if (ConfigId == UI_ISP_MAP[i].UI_Item.Id && UI_Val == UI_ISP_MAP[i].UI_Item.Val)
        {
            ret = UI_ISP_MAP[i].REAL_VAL;
            break;
        }
    }
    return ret;
}

int FindUIValueFromRealVal(int ConfigId, int Real_Val)
{
    int i = 0;
    int ret = 0;

    for (i = 0; i < sizeof(UI_ISP_MAP)/sizeof(UI_ISP_MAP[0]); i++)
    {
        if (ConfigId == UI_ISP_MAP[i].UI_Item.Id && Real_Val == UI_ISP_MAP[i].REAL_VAL)
        {
            ret = UI_ISP_MAP[i].UI_Item.Val;
            break;
        }
    }
    return ret;
}

int PushNewSettingToQueue(int ConfigId, int UI_Val)
{
    int i = 0;
    pthread_mutex_lock(&isp_tuning_lock);
    for (i = 0; i < sizeof(SettingQueue)/sizeof(SettingQueue[0]); i++)
    {
        if (ConfigId == SettingQueue[i].item.UI_Item.Id)
        {
            SettingQueue[i].item.UI_Item.Val = UI_Val;
            SettingQueue[i].item.REAL_VAL    = FindRealValueFromUI(ConfigId, UI_Val);
            SettingQueue[i].WaitSettingFlag   = 1;
            //printf("--->PushNewSettingToQueue is %f\n", SettingQueue[i].item.REAL_VAL);
            break;
        }
    }
    pthread_mutex_unlock(&isp_tuning_lock);
    return 0;
}

#ifdef SCAN_ENV_BRIGHTNESS
static double s_cur_env_brightness = 0;
extern "C" double GetIEnvBrightness(void)
{
    return s_cur_env_brightness;
}
#endif

extern "C" int GetISPConfigItemState(int ConfigId)
{
    int i = 0, ret = 0;
    pthread_mutex_lock(&isp_tuning_lock);
    for (i = 0; i < sizeof(SettingQueue)/sizeof(SettingQueue[0]); i++)
    {
        if (ConfigId == SettingQueue[i].item.UI_Item.Id)
        {
            ret = SettingQueue[i].item.UI_Item.Val;
        }
    }
    pthread_mutex_unlock(&isp_tuning_lock);
    return ret;
}

extern "C" int SetISPConfigItemState(int ConfigId, int UI_Val)
{
    //printf("-->***ConfigId = %d, UI_Val == %d\n", ConfigId, UI_Val);
    return PushNewSettingToQueue(ConfigId, UI_Val);
}
#endif
// end added by linyun.xiong @2015-07-09 for support isp tuning

void printUsage()
{
    DYNCMD_PrintUsage();

    printf("\n\nRuntime key controls:\n");
    printf("\t=== Control Modules toggle ===\n");
    printf("%c defective pixels\n", DPF_KEY);
    printf("%c use LSH matrix if available\n", LSH_KEY);
    printf("%c adaptive TNM\n", ADAPTIVETNM_KEY);
    printf("%c local TNM\n", LOCALTNM_KEY);
    printf("%c enable AWB\n", AWB_KEY);
    printf("%c enable DNS update from sensor\n", DNSISO_KEY);
    printf("%c enable LBC\n", LBC_KEY);
    printf("\t=== Auto Exposure ===\n");
    printf("%c/%c Target brightness up/down\n", BRIGHTNESSUP_KEY, BRIGHTNESSDOWN_KEY);
    printf("%c Flicker detection toggle\n", AUTOFLICKER_KEY);
    printf("\t=== Auto Focus ===\n");
    printf("%c/%c Focus closer/further\n", FOCUSCLOSER_KEY, FOCUSFURTHER_KEY);
    printf("%c auto focus trigger\n", FOCUSTRIGGER_KEY);
    printf("\t=== Multi Colour Correction Matrix ===\n");
    printf("%c force identity CCM if AWB disabled\n", AWB_RESET);
    printf("%d-%d load one of the CCM if available and AWB disabled\n", AWB_FIRST_DIGIT, AWB_LAST_DIGIT);
    printf("\t=== Other controls ===\n");
    printf("%c Save all outputs as specified in setup file\n", SAVE_ORIGINAL_KEY);
    printf("%c Save RGB output\n", SAVE_RGB_KEY);
    printf("%c Save YUV output\n", SAVE_YUV_KEY);
    printf("%c Save DE output\n", SAVE_DE_KEY);
    printf("%c Save RAW2D output (may not work on all HW versions)\n", SAVE_RAW2D_KEY);
    printf("%c Save HDR output (may not work on all HW versions)\n", SAVE_HDR_KEY);

    printf("%c Exit\n", EXIT_KEY);

    printf("\n\nAvailable sensors in sensor API:\n");

    const char **pszSensors = Sensor_ListAll();
    int s = 0;
    while(pszSensors[s])
    {
        int strn = strlen(pszSensors[s]);
        SENSOR_HANDLE hSensorHandle = NULL;

        printf("\t%d: %s \n", s, pszSensors[s]);

        if ( Sensor_Initialise(s, &hSensorHandle) == IMG_SUCCESS )
        {
            SENSOR_MODE mode;
            int m = 0;
            while ( Sensor_GetMode(hSensorHandle, m, &mode) == IMG_SUCCESS )
            {
                printf("\t\tmode %d: %5dx%5d @%.2f (vTot=%d) %s\n",
                    m, mode.ui16Width, mode.ui16Height, mode.flFrameRate, mode.ui16VerticalTotal, 
                    mode.ui8SupportFlipping==SENSOR_FLIP_NONE?"flipping=none":
                    mode.ui8SupportFlipping==SENSOR_FLIP_BOTH?"flipping=horizontal|vertical":
                    mode.ui8SupportFlipping==SENSOR_FLIP_HORIZONTAL?"flipping=horizontal":"flipping=vertical");
                m++;
            }
        }
        else
        {
            printf("\tfailed to init sensor API - no modes display availble\n");
        }

        s++;
    }
}

/**
 * @brief Represents the configuration for demo configurable parameters in running time
 */
struct DemoConfiguration
{
    // Auto Exposure
    bool controlAE;             ///< @brief Create a control AE object or not
    double targetBrightness;    ///< @brief target value for the ControlAE brightness
#ifdef INFOTM_ISP
    double oritargetBrightness;    ///< @brief target value for the ControlAE brightness
#endif
    bool flickerRejection;      ///< @brief Enable/Disable Flicker Rejection

    // Tone Mapper
    bool controlTNM;            ///< @brief Create a control TNM object or not
    bool localToneMapping;      ///< @brief Enable/Disable local tone mapping
    bool autoToneMapping;       ///< @brief Enable/Disable tone mapping auto strength control

    // Denoiser
    bool controlDNS;            ///< @brief Create a control DNS object or not
    bool autoDenoiserISO;       ///< @brief Enable/Disable denoiser ISO control

    // Light Based Controls
    bool controlLBC;            ///< @brief Create a control LBC object or not
    bool autoLBC;               ///< @brief Enable/Disable Light Based Control

    // Auto Focus
    bool controlAF;             ///< @brief Create a control AF object or not

    // White Balance
    bool controlAWB;            ///< @brief Create a control AWB object or not
    bool enableAWB;             ///< @brief Enable/Disable AWB Control
    int targetAWB;              ///< @brief forces one of the AWB as CCM (index from 1) (if 0 forces identity, if <0 does not change)
    ISPC::ControlAWB::Correction_Types AWBAlgorithm; ///< @brief choose which algorithm to use - default is combined
#ifdef INFOTM_ISP	
	bool commandAWB;
#endif //INFOTM_ISP

    // to know if we should allow other formats than RGB - changes when a key is pressed, RGB is always savable
    bool bSaveRGB;
    bool bSaveYUV;
    bool bSaveDE;
    bool bSaveHDR;
    bool bSaveRaw2D;
    // other controls
    bool bEnableDPF;
    bool bEnableLSH;

    bool bResetCapture; // to know we need to reset to original output

    // original formats loaded from the config file
    ePxlFormat originalYUV;
    ePxlFormat originalRGB; // we force RGB to be RGB_888_32 to be displayable on PDP
    ePxlFormat originalBayer;
    CI_INOUT_POINTS originalDEPoint;
    ePxlFormat originalHDR; // HDR only has 1 format
    ePxlFormat originalTiff;
    // other info from command line
    int nBuffers;
    int sensorMode;
    IMG_BOOL8 sensorFlip[2]; // horizontal and vertical
    unsigned int uiSensorInitExposure; // from parameters and also used when restarting to store current exposure
    // using IMG_FLOAT to be available in DYNCMD otherwise would be double
    IMG_FLOAT flSensorInitGain; // from parameters and also used when restarting to store current gain

    char *pszFelixSetupArgsFile;
    char *pszSensor;
    char *pszInputFLX; // for data generators only
    unsigned int gasket; // for data generator only
    unsigned int aBlanking[2]; // for data generator only

    DemoConfiguration()
    {
        // AE
        controlAE = true; //default: true 
        targetBrightness = 0.0f; //default: 0.0f
#ifdef INFOTM_ISP
        oritargetBrightness = targetBrightness;
#endif
        flickerRejection = true; //default: false 

        // TNM
#if defined(INFOTM_ENABLE_GLOBAL_TONE_MAPPER)
        controlTNM = true; //default: true 
#else
        controlTNM = false;
#endif
#if defined(INFOTM_ENABLE_LOCAL_TONE_MAPPER)
        localToneMapping = true;
#else
        localToneMapping = false; //default: false 
#endif
        autoToneMapping = false; //default: false 

        // DNS
        controlDNS = true; //default: true 
        autoDenoiserISO = true; //default: false 

        // LBC
        controlLBC = true; //default: true 
        autoLBC = false; //default: false 

        // AF
        controlAF = false; //default: true 

        // AWB
        controlAWB = true; //default: true 
        enableAWB = true; //default: false 
        AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED; //default: ISPC::ControlAWB::WB_COMBINED
        targetAWB = -1; //default: 0 
#ifdef INFOTM_ISP		
		commandAWB = false;
#endif //INFOTM_ISP		

        // savable
        bSaveRGB = false; //default: false
        bSaveYUV = false; //default: false
        bSaveDE = false; //default: false
        bSaveHDR = false; //default: false
        bSaveRaw2D = false; //default: false

        bEnableDPF = true; //default: false
#if defined(USE_ISP_LENS_SHADING_CORRECTION)
        bEnableLSH = true; //default: false
#else
        bEnableLSH = false;
#endif //INFOTM_ENABLE_LENS_SHADING_CORRECTION
        bResetCapture = false; //default: false

        originalYUV = PXL_NONE; //default: PXL_NONE
        originalRGB = PXL_NONE; //default: PXL_NONE
        originalBayer = PXL_NONE; //default: PXL_NONE
        originalDEPoint = CI_INOUT_NONE; //default: CI_INOUT_NONE
        originalHDR = PXL_NONE; //default: PXL_NONE
        originalTiff = PXL_NONE; //default: PXL_NONE

        // other information
        nBuffers = 3; //default: 2

        sensorMode = 0; //default: 0
        sensorFlip[0] = false; //default: false
        sensorFlip[1] = false; //default: false
        uiSensorInitExposure = 0; //default: 0
        flSensorInitGain = 0.0f; //default: 0.0f

        pszFelixSetupArgsFile = NULL; //default: NULL
        pszSensor = NULL; //default: NULL
        pszInputFLX = NULL; // for data generators only //default: NULL
        gasket = 0; // for data generator only //default: 0
        aBlanking[0] = FELIX_MIN_H_BLANKING; // for data generator only //default: FELIX_MIN_H_BLANKING
        aBlanking[1] = FELIX_MIN_V_BLANKING; //default: FELIX_MIN_H_BLANKING
    }

    // generateDefault if format is PXL_NONE
    void loadFormats(const ISPC::ParameterList &parameters, bool generateDefault=true)
    {
        originalYUV = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::ENCODER_TYPE));
        debug("loaded YVU=%s\n", FormatString(originalYUV));
        if ( generateDefault && originalYUV == PXL_NONE )
            originalYUV = YUV_420_PL12_8;

        originalRGB = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DISPLAY_TYPE));
        debug("loaded RGB=%s (forced)\n", FormatString(RGB_888_32));
        if ( generateDefault && originalRGB == PXL_NONE )
            originalRGB = RGB_888_32;


        originalBayer = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_TYPE));
        debug("loaded DE=%s\n", FormatString(originalBayer));
        if ( generateDefault && originalBayer == PXL_NONE )
            originalBayer = BAYER_RGGB_12;

        originalDEPoint = (CI_INOUT_POINTS)(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_POINT)-1);
        debug("loaded DE point=%d\n", (int)originalDEPoint);
        if ( generateDefault && originalDEPoint != CI_INOUT_BLACK_LEVEL && originalDEPoint != CI_INOUT_FILTER_LINESTORE )
            originalDEPoint = CI_INOUT_BLACK_LEVEL;

        originalHDR = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::HDREXTRA_TYPE));
        debug("loaded HDR=%s (forced)\n", FormatString(BGR_101010_32));
        if ( generateDefault && originalHDR == PXL_NONE )
            originalHDR = BGR_101010_32;


        originalTiff = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::RAW2DEXTRA_TYPE));
        debug("loaded TIFF=%s\n", FormatString(originalTiff));
        if ( generateDefault && originalTiff == PXL_NONE )
            originalTiff = BAYER_TIFF_12;

    }

    /**
     * @brief Apply a configuration to the control loop algorithms
     * @param
     */
    void applyConfiguration(ISPC::Camera &cam)
    {
        //
        // control part
        //
        ISPC::ControlAE *pAE = cam.getControlModule<ISPC::ControlAE>();
        ISPC::ControlAWB_PID *pAWB = cam.getControlModule<ISPC::ControlAWB_PID>();
        ISPC::ControlTNM *pTNM = cam.getControlModule<ISPC::ControlTNM>();
        ISPC::ControlDNS *pDNS = cam.getControlModule<ISPC::ControlDNS>();
        ISPC::ControlLBC *pLBC = cam.getControlModule<ISPC::ControlLBC>();

        if ( pAE )
        {
#if defined(INFOTM_ISP) && defined(ENABLE_AE_DEBUG_MSG)
        	printf("felix::applyConfiguration targetBrightness = %6.3f\n", this->targetBrightness);
#endif //INFOTM_ISP
#ifndef INFOTM_ISP
            pAE->setTargetBrightness(this->targetBrightness);
#else
			if (this->oritargetBrightness != pAE->getOriTargetBrightness())
			{
				pAE->setTargetBrightness(this->targetBrightness);
				pAE->setOriTargetBrightness(this->oritargetBrightness);
			}
#endif

            //Enable/disable flicker rejection in the auto-exposure algorithm
            pAE->enableFlickerRejection(this->flickerRejection);
        }

        if ( pTNM )
        {
            // parameters loaded from config file
            pTNM->enableControl(this->localToneMapping||this->autoToneMapping);

            //Enable/disable local tone mapping
            pTNM->enableLocalTNM(this->localToneMapping);

            //Enable/disable automatic tone mapping strength control
            pTNM->enableAdaptiveTNM(this->autoToneMapping);

            if (!pAE || !pAE->isEnabled())
            {
                // TNM needs global HIS, normally configured by AE, if AE not present allow TNM to configure it
                pTNM->setAllowHISConfig(true);
            }
        }

        if ( pDNS )
        {
            //Enable disable denoiser ISO parameter automatic update
            pDNS->enableControl(this->autoDenoiserISO);
        }

        if ( pLBC )
        {
            //Enable disable light based control to set up certain pipeline characteristics based on light level
            pLBC->enableControl(this->autoLBC);

            if ((!pTNM || !pTNM->isEnabled()) && (!pAE || !pAE->isEnabled()))
            {
                // LBC needs global HIS to estimate illumination, normally configured by AE or TNM, if both not present allow LBC to configure it
                pLBC->setAllowHISConfig(true);
            }
        }

        if ( pAWB )
        {
            pAWB->enableControl(this->enableAWB);

            if ( this->targetAWB >= 0 )
            {
#if defined(INFOTM_ISP) && defined(ENABLE_AWB_DEBUG_MSG)
                printf("targetAWB = %d\n", this->targetAWB);
#endif //INFOTM_ISP
                ISPC::ModuleCCM *pCCM = cam.getModule<ISPC::ModuleCCM>();
                ISPC::ModuleWBC *pWBC = cam.getModule<ISPC::ModuleWBC>();
                const ISPC::TemperatureCorrection &tempCorr = pAWB->getTemperatureCorrections();

                if ( this->targetAWB == 0 )
                {
                    ISPC::ParameterList defaultParams;
                    pCCM->load(defaultParams); // load defaults
                    pCCM->requestUpdate();

                    pWBC->load(defaultParams);
                    pWBC->requestUpdate();
                }
                else if ( this->targetAWB-1 < tempCorr.size() )
                {
                    ISPC::ColorCorrection currentCCM = tempCorr.getCorrection(this->targetAWB-1);

                    for(int i=0;i<3;i++)
                    {
                        for(int j=0;j<3;j++)
                        {
                            pCCM->aMatrix[i*3+j] = currentCCM.coefficients[i][j];
                        }
                    }

                    //apply correction offset
                    pCCM->aOffset[0] = currentCCM.offsets[0][0];
                    pCCM->aOffset[1] = currentCCM.offsets[0][1];
                    pCCM->aOffset[2] = currentCCM.offsets[0][2];

                    pCCM->requestUpdate();

                    pWBC->aWBGain[0] = currentCCM.gains[0][0];
                    pWBC->aWBGain[1] = currentCCM.gains[0][1];
                    pWBC->aWBGain[2] = currentCCM.gains[0][2];
                    pWBC->aWBGain[3] = currentCCM.gains[0][3];

                    pWBC->requestUpdate();
                }
            } // if targetAWB >= 0
            this->targetAWB = -1;
        }

        //
        // module part
        //

        ISPC::ModuleDPF *pDPF = cam.getModule<ISPC::ModuleDPF>();
        ISPC::ModuleLSH *pLSH = cam.getModule<ISPC::ModuleLSH>();
        ISPC::ModuleOUT *glb = cam.getModule<ISPC::ModuleOUT>();

        this->bResetCapture = false; // we assume we don't need to reset

        ePxlFormat prev = glb->encoderType;
        if ( this->bSaveYUV )
        {
            glb->encoderType = this->originalYUV;
        }
        else
        {
            glb->encoderType = PXL_NONE;
        }
		#ifdef INFOTM_ISP
		glb->encoderType = this->originalYUV;
		#endif //INFOTM_ISP
        this->bResetCapture |= glb->encoderType != prev;
        debug("Configure YUV output %s (prev=%s)\n", FormatString(glb->encoderType), FormatString(prev));

        prev = glb->dataExtractionType;
        if ( this->bSaveDE )
        {
            glb->dataExtractionType = this->originalBayer;
            glb->dataExtractionPoint = this->originalDEPoint;
            glb->displayType = this->originalRGB;
        }
        else
        {
            glb->dataExtractionType = PXL_NONE;
            glb->dataExtractionPoint = CI_INOUT_NONE;
            glb->displayType = PXL_NONE;
        }
        this->bResetCapture |= glb->dataExtractionType != prev;
        debug("Configure DE output %s at %d (prev=%s)\n", FormatString(glb->dataExtractionType), (int)glb->dataExtractionPoint, FormatString(prev));
        debug("Configure RGB output %s (prev=%s)\n", FormatString(glb->displayType), FormatString(prev==PXL_NONE?RGB_888_32:PXL_NONE));

        prev = glb->hdrExtractionType;
        if ( this->bSaveHDR ) // assumes the possibility has been checked
        {
            glb->hdrExtractionType = this->originalHDR;
            //this->bResetCapture = true;
        }
        else
        {
            glb->hdrExtractionType = PXL_NONE;
        }
        this->bResetCapture |= glb->hdrExtractionType != prev;
        debug("Configure HDR output %s (prev=%s)\n", FormatString(glb->hdrExtractionType), FormatString(prev));

        prev = glb->raw2DExtractionType;
        if ( this->bSaveRaw2D )
        {
            glb->raw2DExtractionType = this->originalTiff;
            this->bResetCapture = true;
        }
        else
        {
            glb->raw2DExtractionType = PXL_NONE;
        }
        this->bResetCapture |= glb->raw2DExtractionType != prev;
        debug("Configure RAW2D output %s (prev=%s)\n", FormatString(glb->raw2DExtractionType), FormatString(prev));

        debug("Configure needs to reset camera? %d\n", this->bResetCapture);

        if ( pLSH )
        {
            if ( pLSH->hasGrid() )
            {
                pLSH->bEnableMatrix = this->bEnableLSH;
#if defined(INFOTM_ISP) && defined(ENABLE_LSH_DEBUG_MSG)
                if ( pLSH->bEnableMatrix )
                {
                    printf("LSH matrix from %s\n", pLSH->lshFilename.c_str());
                }
#endif //ENABLE_LSH_DEBUG_MSG
                pLSH->requestUpdate();
            }
        }

        if ( pDPF )
        {
            pDPF->bDetect = this->bEnableDPF;
            pDPF->bWrite = this->bEnableDPF;

            pDPF->requestUpdate();
        }
        else
        {
            this->bEnableDPF = false; // so that stats are not read
        }

        if(this->bResetCapture && pAE)
        {
            // if we reset and have Auto Exposure we save current values in initial exposure and gains
            // so that the output does not re-run the loop
            this->uiSensorInitExposure = cam.getSensor()->getExposure();
            this->flSensorInitGain = cam.getSensor()->getGain();
        }
    }

    int loadParameters(int argc, char *argv[])
    {
        int ret;
        int missing = 0;

        //command line parameter loading:
        ret = DYNCMD_AddCommandLine(argc, argv, "-source");
        if (IMG_SUCCESS != ret)
        {
            fprintf(stderr, "ERROR unable to parse command line\n");
            DYNCMD_ReleaseParameters();
            return EXIT_FAILURE;
        }

        ret=DYNCMD_RegisterParameter("-setupFile", DYNCMDTYPE_STRING, 
                "FelixSetupArgs file to use to configure the Pipeline", 
                &(this->pszFelixSetupArgsFile));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error while parsing parameter -setupFile\n");
            }
            else
            {
                fprintf(stderr, "Error: No setup file provided\n");
            }
            missing++;
        }

        std::ostringstream os;
        os.str("");
        os << "Sensor to use for the run. If '" << IIFDG_SENSOR_INFO_NAME;
#ifdef EXT_DATAGEN
        os << "' or '" << EXTDG_SENSOR_INFO_NAME;
#endif
        os << "' is used then the sensor will be considered a data-generator.";
        static std::string sensorHelp = os.str(); // static otherwise object is lost and c_str() points to invalid memory

        ret=DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING, 
                (sensorHelp.c_str()),
                &(this->pszSensor));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error while parsing parameter -sensor\n");
            }
            else
            {
                fprintf(stderr, "Error: no sensor provided\n");
            }
            missing++;
        }

        ret=DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_INT, 
                "Sensor mode to use",
                &(this->sensorMode));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -modenum\n");
            }
        }
#if defined(SUPPORT_CHANGE_RESOULTION) && !defined(ISP_SUPPORT_SCALER)
		if (strcmp(this->pszSensor, "OV2710") == 0)
		{
			switch (cur_resolution)
			{
			case VIDEO_1080P:
				cur_sensor_mode = 0;
				break;
			case VIDEO_720P:
				cur_sensor_mode = 1;
				break;
			default:
				cur_sensor_mode = 0;
				break;
			}
			
			if (cur_sensor_mode != this->sensorMode)
			{
				this->sensorMode = cur_sensor_mode;
			}
		}
#endif
        
        ret=DYNCMD_RegisterParameter("-sensorExposure", DYNCMDTYPE_UINT, 
                "[optional] initial exposure to use in us", 
                &(this->uiSensorInitExposure));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                fprintf(stderr, "Error while parsing parameter -sensorExposure\n");
                missing++;
            }
            // parameter is optional
        }

        ret=DYNCMD_RegisterParameter("-sensorGain", DYNCMDTYPE_FLOAT, 
                "[optional] initial gain to use", 
                &(this->flSensorInitGain));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                fprintf(stderr, "Error while parsing parameter -sensorGain\n");
                missing++;
            }
            // parameter is optional
        }

        ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8, 
                "[optional] Set 1 to flip the sensor Horizontally and Vertically. If only 1 option is given it is assumed to be the horizontal one\n",
                this->sensorFlip, 2);
        if (RET_FOUND != ret)
        {
            if (RET_INCOMPLETE == ret)
            {
                fprintf(stderr, "Warning: incomplete -sensorFlip, flipping is H=%d V=%d\n", 
                        (int)sensorFlip[0], (int)sensorFlip[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error: while parsing parameter -sensorFlip\n");
                missing++;
            }
        }

        int algo = 0;
        ret=DYNCMD_RegisterParameter("-chooseAWB", DYNCMDTYPE_UINT, 
                "Enable usage of White Balance Control algorithm and loading of the multi-ccm parameters - values are mapped ISPC::Correction_Types (1=AC, 2=WP, 3=HLW, 4=COMBINED)", 
                &(algo));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -chooseAWB\n");
            }
        }
        else
        {
            switch(algo)
            {
                case ISPC::ControlAWB::WB_AC:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_AC;
                    break;

                case ISPC::ControlAWB::WB_WP:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_WP;
                    break;

                case ISPC::ControlAWB::WB_HLW:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_HLW;
                    break;

                case ISPC::ControlAWB::WB_COMBINED:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED;
                    break;

                case ISPC::ControlAWB::WB_NONE:
                default:
                    missing++;
                    fprintf(stderr, "Unsuported AWB algorithm %d given\n", algo);
            }
#ifdef INFOTM_ISP			
			this->commandAWB = true;
#endif //INFOTM_ISP			
        }

        ret = DYNCMD_RegisterParameter("-disableAE", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Auto Exposure control object",
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlAE = false;
        }
        ret = DYNCMD_RegisterParameter("-disableAWB", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Auto White Balance control object",
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlAWB = false;
        }
        ret = DYNCMD_RegisterParameter("-disableAF", DYNCMDTYPE_COMMAND,
                "Disable construction of the Auto Focus control object", 
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlAF = false;
        }
        ret = DYNCMD_RegisterParameter("-disableTNMC", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Tone Mapper control object", 
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlTNM = false;
        }
        ret = DYNCMD_RegisterParameter("-disableDNS", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Denoiser control object", 
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlDNS = false;
        }
        DYNCMD_RegisterParameter("-disableLBC", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Light Base Control object", 
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlLBC = false;
        }

        ret=DYNCMD_RegisterParameter("-nBuffers", DYNCMDTYPE_UINT, 
                "Number of buffers to run with",
                &(this->nBuffers));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -nBuffers\n");
            }
        }

        ret=DYNCMD_RegisterParameter("-DG_inputFLX", DYNCMDTYPE_STRING, 
                "FLX file to use as an input of the data-generator (only if specified sensor is a data generator)",
                &(this->pszInputFLX));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_inputFLX\n");
            }
        }

        ret=DYNCMD_RegisterParameter("-DG_gasket", DYNCMDTYPE_UINT, 
                "Gasket to use for data-generator (only if specified sensor is a data generator)", 
                &(this->gasket));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_gasket\n");
            }
        }

        ret=DYNCMD_RegisterArray("-DG_blanking", DYNCMDTYPE_UINT,
                "Horizontal and vertical blanking in pixels (only if specified sensor is data generator)", 
                &(this->aBlanking), 2);
        if (RET_FOUND != ret)
        {
            if (RET_INCOMPLETE == ret)
            {
                fprintf(stderr, "-DG_blanking incomplete, second value for blanking is assumed to be %u\n", this->aBlanking[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_blanking\n");
            }
        }

        /*
         * must be last action
         */
        ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "Usage information", NULL);
        if(RET_FOUND == ret || missing > 0)
        {
            printUsage();
            ret = EXIT_FAILURE;
        }
        else
        {
            ret = EXIT_SUCCESS;
        }

        DYNCMD_ReleaseParameters();
        return ret;
    }

};

struct LoopCamera
{
    ISPC::Camera *camera;
    std::list<IMG_UINT32> buffersIds;
    const DemoConfiguration &config;
    unsigned int missedFrames;
    bool mStart;

    enum IsDataGenerator { NO_DG = 0, INT_DG, EXT_DG };
    enum IsDataGenerator isDatagen;

    LoopCamera(DemoConfiguration &_config): config(_config)
    {
        ISPC::Sensor *sensor = 0;
        if(strncmp(config.pszSensor, IIFDG_SENSOR_INFO_NAME, strlen(config.pszSensor))==0)
        {
            isDatagen = INT_DG;

            if (!ISPC::DGCamera::supportIntDG())
            {
                std::cerr << "ERROR trying to use internal data generator but it is not supported by HW!" << std::endl;
                return;
            }

            camera = new ISPC::DGCamera(0, config.pszInputFLX, config.gasket, true);

            sensor = camera->getSensor();

            // apply additional parameters
            //sensor->setExtendedParam(EXTENDED_IIFDG_CONTEXT, &(testparams.aIndexDG[gasket]));
            IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), config.nBuffers);

            IIFDG_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

            sensor = 0;// sensor should be 0 for populateCameraFromHWVersion
        }
#ifdef EXT_DATAGEN
        else if (strncmp(config.pszSensor, EXTDG_SENSOR_INFO_NAME, strlen(EXTDG_SENSOR_INFO_NAME))==0)
        {

            isDatagen = EXT_DG;

            if (!ISPC::DGCamera::supportExtDG())
            {
                std::cerr << "ERROR trying to use external data generator but it is not supported by HW!" << std::endl;
                return;
            }

            camera = new ISPC::DGCamera(0, config.pszInputFLX, config.gasket, false);

            sensor = camera->getSensor();

            // apply additional parameters
            DGCam_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

            sensor = 0;// sensor should be 0 for populateCameraFromHWVersion
        }
#endif
        else
        {
            int flipping = (_config.sensorFlip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE)
                | (_config.sensorFlip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE);
            isDatagen = NO_DG;
            camera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_config.pszSensor), _config.sensorMode, flipping);
            sensor = camera->getSensor();
        }

        if(camera->state==ISPC::CAM_ERROR)
        {
            std::cerr << "ERROR failed to create camera correctly" << std::endl;
            delete camera;
            camera = 0;
            return;
        }

        ISPC::CameraFactory::populateCameraFromHWVersion(*camera, sensor);
        missedFrames = 0;
        mStart = false;
        buffersIds.clear();
    }

    ~LoopCamera()
    {
        if(camera)
        {
            //camera->control.clearModules();
            delete camera;
            camera = 0;
        }
    }

	void IspMakeupLongitudeString(int step, int Value)
	{
		int data = abs(Value);
		if(ISP_LONGITUDE_ID1 == step)
		{
			if(0xFF == Value)
			{
				memset(OV_Longitude_String, 0, sizeof(OV_Longitude_String));
				return;
			}
			
			if(Value > 0)
			{
				OV_Longitude_String[0] = 'E';
			}
			else
			{
				OV_Longitude_String[0] = 'W';
			}
			OV_Longitude_String[3] = data%10 + 48;
			data /= 10;
			OV_Longitude_String[2] = data%10 + 48;
			data /= 10;
			OV_Longitude_String[1] = data%10 + 48;
		}
	
		if(ISP_LONGITUDE_ID2 == step && OV_Longitude_String[0] != 0)
		{
			data /= 100;//wangkai first 4 bit
			OV_Longitude_String[8] = data%10 + 48;
			data /= 10;
			OV_Longitude_String[7] = data%10 + 48;
			data /= 10;
			OV_Longitude_String[6] = data%10 + 48;
			data /= 10;
			OV_Longitude_String[5] = data%10 + 48;
			OV_Longitude_String[4] = '.';
			OV_Longitude_String[9] = '`';
			OV_Longitude_String[10] = '\0';//'`';
		}
	}

	void IspMakeupLatitudeString(int step, int Value)
	{
		int data = abs(Value);
		if(ISP_LATITUDE_ID1 == step)
		{
			if(0xFF == Value)
			{
				memset(OV_Latitude_String, 0, sizeof(OV_Latitude_String));
				return;
			}
			
			if(Value > 0)
			{
				OV_Latitude_String[0] = 'N';
			}
			else
			{
				OV_Latitude_String[0] = 'S';
			}
			OV_Latitude_String[2] = data%10 + 48;
			data /= 10;
			OV_Latitude_String[1] = data%10 + 48;
		}
	
		if(ISP_LATITUDE_ID2 == step && OV_Latitude_String[0] != 0)
		{
			data /= 100;//wangkai first 4 bit
			OV_Latitude_String[7] = data%10 + 48;
			data /= 10;
			OV_Latitude_String[6] = data%10 + 48;
			data /= 10;
			OV_Latitude_String[5] = data%10 + 48;
			data /= 10;
			OV_Latitude_String[4] = data%10 + 48;
			OV_Latitude_String[3] = '.';
			OV_Latitude_String[8] = '`';
			OV_Latitude_String[9] = 32;//'`';
			OV_Latitude_String[10] = '\0';//'`';
		}
	}

	void IspMakeupSpeedString(int step, int Value)
	{
		int data = abs(Value);
		if(ISP_SPEED_ID == step)
		{
			data/=1000;
			if(0xFF == data)
			{
				memset(OV_Speed_String, 0, sizeof(OV_Speed_String));
				return;
			}
			OV_Speed_String[2] = data%10 + 48;
			data/=10;
			OV_Speed_String[1] = data%10 + 48;
			data/=10;
			OV_Speed_String[0] = data%10 + 48;
			OV_Speed_String[3] = 'k';
			OV_Speed_String[4] = 'm';
			OV_Speed_String[5] = '/';
			OV_Speed_String[6] = 'h';
			OV_Speed_String[7] = 32;
			OV_Speed_String[8] = '\0';
		}
	}

    // begin added by linyun.xiong @2015-07-09 for support isp tuning
    #ifdef INFOTM_ISP_TUNING
        // return:
        // 1 == update no need reset sensor
        // 2 == update need reset sensor
        int IspTuningCheck(DemoConfiguration &config)
        {
            int i = 0, ret = 0, flag = 0;
			int ResolutionSetFlag = 0;
			int PlateSetFlag = 0;
			int TargetWidth = 0, TargetHeight = 0;
            ISPC::ParameterList parameters;

#ifdef SCAN_ENV_BRIGHTNESS
            ISPC::ControlAE* pAE = camera->getControlModule<ISPC::ControlAE>();
            s_cur_env_brightness = (pAE->getCurrentBrightness() - pAE->getTargetBrightness());
//#if 0 // ??? for test brightness
//            printf("--->getCurrentBrightness() is %f, getTargetBrightness() is %f\n", pAE->getCurrentBrightness(), pAE->getTargetBrightness());
//#else
//            printf("--->Current brightness is %f\n", s_cur_env_brightness);
//#endif
#endif

            pthread_mutex_lock(&isp_tuning_lock);
            for (i = 0; i < sizeof(SettingQueue)/sizeof(SettingQueue[0]); i++)
            {
                if (SettingQueue[i].WaitSettingFlag == 1)
                {
                    switch (SettingQueue[i].item.UI_Item.Id)
                    {
                    case VIDEO_RESOLUTION_ID:
                        {
#if defined(SUPPORT_CHANGE_RESOULTION)
#if defined(ISP_SUPPORT_SCALER)
							int video_width = 0, video_height = 0;
                            pthread_mutex_lock(&isp_lock);
                            g_fb_0_reset = 1;
                            pthread_mutex_unlock(&isp_lock);

#endif

                            switch ((int)SettingQueue[i].item.REAL_VAL)
                            {
                            case 1080:
#if !defined(ISP_SUPPORT_SCALER)
								if (cur_resolution == VIDEO_1080P)
								{
									flag |= 0x1;
									break;
								}
								cur_resolution = VIDEO_1080P;
								flag |= 0x2;
#else
								video_width  = 1920;
								video_height = 1088;
								flag |= 0x1;
#endif
                                break;
                            case 720:
#if !defined(ISP_SUPPORT_SCALER)
								if (cur_resolution == VIDEO_720P)
								{
									flag |= 0x1;
									break;
								}

								cur_resolution = VIDEO_720P;
								flag |= 0x2;
#else
								video_width  = 1280;
								video_height = 720;		
								flag |= 0x1;
#endif
                                break;
							case VIDEO_480P:
#if defined(ISP_SUPPORT_SCALER)
								video_width  = 800;
								video_height = 480;
#endif

								flag |= 0x1;
								break;
							case VIDEO_400P:
#if defined(ISP_SUPPORT_SCALER)
								video_width  = 800;
								video_height = 400;
#endif

								flag |= 0x1;
								break;
							case VIDEO_320P:
#if defined(ISP_SUPPORT_SCALER)
								video_width  = 720;
								video_height = 320;
#endif

								flag |= 0x1;
								break;
							case VIDEO_240P:
#if defined(ISP_SUPPORT_SCALER)
								video_width  = 360;
								video_height = 240;
#endif

								flag |= 0x1;
                                break;
							default:
#if defined(ISP_SUPPORT_SCALER)
								// INVALID_REAL_VAL
								{
								    video_width = (SettingQueue[i].item.UI_Item.Val>>16);
								    video_height = (SettingQueue[i].item.UI_Item.Val&0x0000FFFF);                                    
								}
#endif
								flag |= 0x1;
								break;
                            }
#if defined(ISP_SUPPORT_SCALER)
                            if (video_width > 0 && video_height > 0)
							{
								ISPC::ModuleESC* esc = camera->getModule<ISPC::ModuleESC>();
								if (esc != NULL)
								{
									printf("--> width = %d, height = %d\n", video_width, video_height);
#if defined(FOR_ZHUOYING_PRJ)
									esc->aPitch[0] = (IMG_FLOAT)1;
									esc->aPitch[1] = (IMG_FLOAT)1;

									esc->aRect[0] = 120;
#else
									esc->aPitch[0] = (IMG_FLOAT)0;
									esc->aPitch[1] = (IMG_FLOAT)0;

									esc->aRect[0] = 0;
#endif
									esc->aRect[1] = 0;
									esc->aRect[2] = video_width;
									esc->aRect[3] = video_height;
									esc->eRectType = ISPC::SCALER_RECT_SIZE;
									//esc->requestUpdate();
									esc->setup();
								}
							}
#endif
#endif

                        }
                        
                        break;
                    case PHOTO_RESOLUTION_ID:
                        flag |= 0x1;
                        break;
                    case WB_ID:
                        {
                            //ISPC::ControlAWB* pAWB = camera->getControlModule<ISPC::ControlAWB>();
                            static int lastVal = 0;
                            ISPC::ControlAWB_PID* pAWB = camera->getControlModule<ISPC::ControlAWB_PID>();

                            if (SettingQueue[i].item.REAL_VAL == lastVal)
                            {
                                break;
                            }
                            if (SettingQueue[i].item.REAL_VAL == 0)
                            {
                                 config.enableAWB = true;
                                config.AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED;
                                config.targetAWB = 0;
                                pAWB->setCorrectionMode(config.AWBAlgorithm);
                                pAWB->setTargetTemperature(0);
                            }
                            else if (SettingQueue[i].item.REAL_VAL > 0)
                            {
                                 config.enableAWB = false;
                                config.AWBAlgorithm = ISPC::ControlAWB::WB_NONE;
                                config.targetAWB = SettingQueue[i].item.REAL_VAL;
                                pAWB->setCorrectionMode(config.AWBAlgorithm);
                                //pAWB->setTargetTemperature(SettingQueue[i].item.REAL_VAL);
                            }
                            else
                            {
#ifdef SPORTDV_PROJECT
								printf("###invalid wb setting, so use default awb mode###\n");
								config.enableAWB = true;
								config.AWBAlgorithm = ISPC::WB_COMBINED;
								config.targetAWB = 0;
								pAWB->setCorrectionMode(config.AWBAlgorithm);
								pAWB->setTargetTemperature(0);
#else
                                //config.AWBAlgorithm = ISPC::WB_AC;
                                config.enableAWB = false;
                                //config.targetAWB = SettingQueue[i].item.REAL_VAL;
                                //pAWB->getTemperatureCorrections()
                                config.targetAWB = 0;
                                pAWB->setTargetTemperature(0);
#endif
                            }
                            
                            lastVal = SettingQueue[i].item.REAL_VAL;
                            
                        }
                        flag |= 0x1;
                        break;
                    case COLOR_ID:
                        {
                            ISPC::ModuleR2Y *pR2Y = camera->pipeline->getModule<ISPC::ModuleR2Y>();
                            ISPC::ModuleTNM *pTNM = camera->pipeline->getModule<ISPC::ModuleTNM>();

                            if (ADVANCED_COLOR_MODE == SettingQueue[i].item.UI_Item.Val)
                            {
                                printf("-->***ADVANCED_COLOR_MODE %d\n", SettingQueue[i].item.REAL_VAL);
                                pR2Y->aRangeMult[0] = 1;
                                pR2Y->aRangeMult[1] = 1;
                                pR2Y->aRangeMult[2] = 1;

                                pR2Y->fOffsetU = 0;
                                pR2Y->fOffsetV = 0;

                                pTNM->fColourSaturation = SettingQueue[i].item.REAL_VAL;
            
                                //pR2Y->fSaturation = SettingQueue[i].item.REAL_VAL;                            
                                //pR2Y->setup();
                            }
                            else if (FLAT_COLOR_MODE == SettingQueue[i].item.UI_Item.Val)
                            {
                                printf("-->***FLAT_COLOR_MODE\n");
                                pR2Y->aRangeMult[0] = 1;
                                pR2Y->aRangeMult[1] = 0;
                                pR2Y->aRangeMult[2] = 0;

                                pR2Y->fOffsetU = 0;
                                pR2Y->fOffsetV = 0;
                            
                                pTNM->fColourSaturation = SettingQueue[i].item.REAL_VAL;
                            }
                            else if (SEPIA_COLOR_MODE == SettingQueue[i].item.UI_Item.Val)
                            {
                                printf("-->***SEPIA_COLOR_MODE %d\n", SettingQueue[i].item.UI_Item.Val);
                                pR2Y->aRangeMult[0] = 1;
                                pR2Y->aRangeMult[1] = 0;
                                pR2Y->aRangeMult[2] = 0;

                                pR2Y->fOffsetU = -61;//-30.7436;	//cb
                                pR2Y->fOffsetV = 50;//26.7304;		//cr
                                pTNM->fColourSaturation = SettingQueue[i].item.REAL_VAL;
                            }
                            else
                            {
                                printf("-->invalid color mode value\n");
                            }
                            
                            pTNM->requestUpdate();
                            pR2Y->requestUpdate();
                        }
                        flag |= 0x1;
                        break;
                    case VIDEO_ISO_LIMIT_ID:
                        if (SettingQueue[i].item.REAL_VAL > 0)
                        {
                            camera->sensor->setMinGain(1.0);
                            camera->sensor->setMaxGain(SettingQueue[i].item.REAL_VAL);
                            //camera->sensor->setGain(SettingQueue[i].item.REAL_VAL);
                            printf("-->*set video iso min = %f, max = %f\n", 1.0, SettingQueue[i].item.REAL_VAL);
                        }
                        else
                        {
                            double flMin, flMax;
                            IMG_UINT8 uiContexts;
                            Sensor_GetGainRange(camera->sensor->getHandle(), &flMin, &flMax, &uiContexts);

                            camera->sensor->setMinGain(flMin);
                            camera->sensor->setMaxGain(flMax);
                            printf("-->set video iso min = %f, max = %f\n", flMin, flMax);
                        }
                        flag |= 0x1;
                        break;
                    case PHOTO_ISO_LIMIT_ID:
                        if (SettingQueue[i].item.REAL_VAL > 0)
                        {
                            camera->sensor->setMinGain(1.0);
                            camera->sensor->setMaxGain(SettingQueue[i].item.REAL_VAL);
                            //camera->sensor->setGain(SettingQueue[i].item.REAL_VAL);
                        }
                        else
                        {
                            double flMin, flMax;
                            IMG_UINT8 uiContexts;
                            Sensor_GetGainRange(camera->sensor->getHandle(), &flMin, &flMax, &uiContexts);

                            camera->sensor->setMinGain(flMin);
                            camera->sensor->setMaxGain(flMax);
                        }
                        flag |= 0x1;
                        break;
                    case SHARPNESS_ID:
                        {
#if 0
                            ISPC::ControlLBC *pLBC = (ISPC::ControlLBC *)camera->getControlModule<const ISPC::ControlLBC>();
                            //pLBC->defSharpness = SettingQueue[i].item.REAL_VAL;
                            //Parameter TmpParameter();
                            parameters.addParameter(ISPC::Parameter("SHA_STRENGTH", ISPC::toString(SettingQueue[i].item.REAL_VAL)));
                            pLBC->change_def_sharpness(SettingQueue[i].item.REAL_VAL);
                            pLBC->setUpdateSpeed(0.1);
#else
                            ISPC::ModuleSHA *pSHA = camera->pipeline->getModule<ISPC::ModuleSHA>();
                            pSHA->fStrength = SettingQueue[i].item.REAL_VAL;
                            pSHA->setup();
#endif
                            flag |= 0x1;
                        }
                        break;
                    case EV_ID:
                    {
                        static int lastVal = 0;
                        if (SettingQueue[i].item.REAL_VAL == lastVal)
                        {
                            break;
                        }
                        lastVal = SettingQueue[i].item.REAL_VAL;
                        
                        config.targetBrightness = SettingQueue[i].item.REAL_VAL;
#if defined(INFOTM_ISP)
                        config.oritargetBrightness = config.targetBrightness;
#endif //INFOTM_ISP
                        flag |= 0x1;
                        break;
                    }
                    case MIRROR_FLIP_ID:
					{
                        int ret_flag = 0;
					    printf("-->mirror flip val is %d\n", (IMG_UINT8)SettingQueue[i].item.REAL_VAL);		
						ret_flag = camera->sensor->setFlipMirror((IMG_UINT8)SettingQueue[i].item.REAL_VAL);
#ifdef SUPPORT_SKIP_FRAMES
                        //set_skip_frames_cnt(8);
#endif
						printf("--->ret_flag = %x\n", ret_flag);

						if (ret_flag == 0xfe)
						{
                        	flag |= 0x2;
						}
						else
						{
                        	flag |= 0x1;
						}
                    }
                        break;
					case OV_PLATE_ID1:
						if(0 == SettingQueue[i].item.UI_Item.Val)
						{
							OV_Plate_String[7] = 0;
							break;
						}
						OV_Plate_String[7] = 1;
						OV_Plate_String[0] = (SettingQueue[i].item.UI_Item.Val >> 24)&0xFF;
						OV_Plate_String[1] = (SettingQueue[i].item.UI_Item.Val >> 16)&0xFF;
						OV_Plate_String[2] = (SettingQueue[i].item.UI_Item.Val >> 8)&0xFF;
						OV_Plate_String[3] = SettingQueue[i].item.UI_Item.Val&0xFF;
						break;
					case OV_PLATE_ID2:
						if(OV_Plate_String[7] == 1)
						{
							OV_Plate_String[4] = (SettingQueue[i].item.UI_Item.Val >> 24)&0xFF;
							OV_Plate_String[5] = (SettingQueue[i].item.UI_Item.Val >> 16)&0xFF;
							OV_Plate_String[6] = (SettingQueue[i].item.UI_Item.Val >> 8)&0xFF;
							OV_Plate_String[7] = 2;
						}
						break;
					case OV_TIME_ID:
#if 0 // ??? for debug no ov first

							printf("---->OV_TIME_ID: %d\n", SettingQueue[i].item.UI_Item.Val);
							if(0 == SettingQueue[i].item.UI_Item.Val)
							{
								OV_Time_Flag = 0;
							}else
							{
								OV_Time_Flag = 1;
							}

#endif
						break;

					case ISP_LONGITUDE_ID1:
						IspMakeupLongitudeString(ISP_LONGITUDE_ID1, SettingQueue[i].item.UI_Item.Val);
						break;
					case ISP_LONGITUDE_ID2:
						IspMakeupLongitudeString(ISP_LONGITUDE_ID2, SettingQueue[i].item.UI_Item.Val);
						break;
					case ISP_LATITUDE_ID1:
						IspMakeupLatitudeString(ISP_LATITUDE_ID1, SettingQueue[i].item.UI_Item.Val);
						break;
					case ISP_LATITUDE_ID2:
						IspMakeupLatitudeString(ISP_LATITUDE_ID2, SettingQueue[i].item.UI_Item.Val);
						break;
					case ISP_SPEED_ID:
						IspMakeupSpeedString(ISP_SPEED_ID, SettingQueue[i].item.UI_Item.Val);
						break;
             		default:
                        break;
                    }
                    SettingQueue[i].WaitSettingFlag   = 0;
                    break;
                }
            }
            if (flag)
            {
#if 0 // unused now
                if ( camera->saveParameters(parameters) == IMG_SUCCESS )
                {
                    ISPC::ParameterFileParser::saveGrouped(parameters, config.pszFelixSetupArgsFile);
                }
#endif
                if (flag & 0x2)
                {
                     // save param list & reset sensor
                    ret = 2;
                }
                else
                {
                     // save param list
                    ret = 1;
                }
            }
            pthread_mutex_unlock(&isp_tuning_lock);
            return ret;
        }

        int InitIspTuningQueue(DemoConfiguration &config)
        {
            int i = 0;
            ISPC::ParameterList parameters;
            pthread_mutex_lock(&isp_tuning_lock);

            if ((config.pszSensor[0] == 'A' && config.pszSensor[1] == 'R')) 
            // aptina
            {
                memcpy(&SettingQueue, &Ar0330DefaultSetting, sizeof(Ar0330DefaultSetting));
            }
#if defined(FOR_ZHUOYING_PRJ)
            else if ((config.pszSensor[0] == 'I' && config.pszSensor[1] == 'M')) // 
            // sony
            {
                memcpy(&SettingQueue, &SonyDefaultSetting, sizeof(SonyDefaultSetting));
            }
#endif
	      else
		{
		    memcpy(&SettingQueue, &OvDefaultSetting, sizeof(OvDefaultSetting));
		}
            
            for (i = 0; i < sizeof(SettingQueue)/sizeof(SettingQueue[0]); i++)
            {
                switch (SettingQueue[i].item.UI_Item.Id)
                {
                case VIDEO_RESOLUTION_ID:
#ifdef SUPPORT_ISP_TUNING
				   {
					   extern int get_config(const char *key, char *value);
					   char value[64] = {0};
					   {
						   unsigned int width = 0, height = 0;
						   
						   get_config("video.resolution", value);
						   if(!strcasecmp(value, "1080FHD"))
						   {
							   width  = 1920;
							   height = 1088;
						   } 
						   else if(!strncasecmp(value, "720P", 4))
						   {
							   width  = 1280;
							   height = 720;
						   } 
						   else if(!strcasecmp(value, "WVGA")) 
						   {
							   width  = 800;
							   height = 480;
						   } 
						   else if(!strcasecmp(value, "VGA")) 
						   {
							   width  = 640;
							   height = 480;
						   } 
						   else 
						   {
							   width  = 1920;
							   height = 1088;
						   }
						   printf("-->init video.resolution is %s, width is %d, height is %d\n", value, width, height);
						   SettingQueue[i].item.UI_Item.Val = ((width<<16)|height);
						   SettingQueue[i].WaitSettingFlag = 1;
						   //SettingQueue[i].item.REAL_VAL = FindRealValueFromUI(VIDEO_RESOLUTION_ID, SettingQueue[i].item.UI_Item.Val)
					   }
				   }
#endif

                    break;
                case PHOTO_RESOLUTION_ID:

                    break;
				case MIRROR_FLIP_ID:
#if defined(SUPPORT_CHANGE_RESOULTION) && defined(SUPPORT_ISP_TUNING)
					{
						extern int get_config(const char *key, char *value);
						char value[64] = {0};
						get_config("setup.imagerotation", value);
						if(!strcasecmp(value, "On"))
						{
							SettingQueue[i].item.UI_Item.Val = MIRROR_AND_FLIP;
						}
						else
						{
							SettingQueue[i].item.UI_Item.Val = NONE;
						}
						SettingQueue[i].item.REAL_VAL = FindRealValueFromUI(MIRROR_FLIP_ID, SettingQueue[i].item.UI_Item.Val);
						printf("-->init setup.imagerotation %d, %d\n", SettingQueue[i].item.UI_Item.Val, (IMG_UINT8)SettingQueue[i].item.REAL_VAL);
						if (config.pszSensor[0] == 'O' && config.pszSensor[1] == 'V' && config.pszSensor[2] == '2' && config.pszSensor[3] == '7')
						{ 
							camera->sensor->setFlipMirror((IMG_UINT8)SettingQueue[i].item.REAL_VAL);
						}
						else
						{
							SettingQueue[i].WaitSettingFlag = 1;
						}
					}
#endif	

                    break;
                case WB_ID:
                    SettingQueue[i].item.REAL_VAL = config.targetAWB ;
                    SettingQueue[i].item.UI_Item.Val = FindUIValueFromRealVal(SettingQueue[i].item.UI_Item.Id, SettingQueue[i].item.REAL_VAL);
                    break;
                case COLOR_ID:
                    break;
                case VIDEO_ISO_LIMIT_ID:
                    break;
                case PHOTO_ISO_LIMIT_ID:
                    break;
                case SHARPNESS_ID:
                    break;
                case EV_ID:
                    break;
                default:
                    break;
                }
#if 0
                if (!(config.pszSensor[0] == 'O' && config.pszSensor[1] == 'V'))
                { // just support ov sensor use optimized default values, so other sensor no use default setting
                    SettingQueue[i].WaitSettingFlag   = 0;
                }
#endif
            }
            pthread_mutex_unlock(&isp_tuning_lock);
        }
    #endif
    // end added by linyun.xiong @2015-07-09 for support isp tuning


    /* Encapsulation for camera api */
    int enqueueShot() {
        IMG_RESULT ret = camera->enqueueShot();
        return 0;
    }

    int acquireShot(ISPC::Shot &shot) {
        IMG_RESULT ret = camera->acquireShot(shot);
        return ret;
    }

    int releaseShot(ISPC::Shot &shot) {
        return camera->releaseShot(shot);
    }

    int startCapture()
    {
        if ( buffersIds.size() > 0 )
        {
            fprintf(stderr, "ERROR: Buffers still exists - camera was not stopped beforehand\n");
            return EXIT_FAILURE;
        }

        // allocate all types of buffers needed
        //printf("### allocateBufferPool %d\n", config.nBuffers);

		if(camera->allocateBufferPool(config.nBuffers, buffersIds)!= IMG_SUCCESS)

        {
            fprintf(stderr, "ERROR: allocating buffers.\n");
            return EXIT_FAILURE;
        }

        missedFrames = 0;

        if(camera->startCapture() !=IMG_SUCCESS)
        {
            fprintf(stderr, "ERROR: starting capture.\n");
            return EXIT_FAILURE;
        }
        mStart = true;

        //
        // pre-enqueue shots to have multi-buffering
        //
        int preEnqueueBuffers = config.nBuffers;

        for ( int i = 0 ; i < preEnqueueBuffers ; i++ )
        {
            if(camera->enqueueShot() !=IMG_SUCCESS)
            {
                printf("---->startCapture fail: enqueueShot error!\n");
                fprintf(stderr, "ERROR: pre-enqueuing shot %d/%d.", i+1, preEnqueueBuffers);
                return EXIT_FAILURE;
            }
        }      
        if(config.uiSensorInitExposure>0)
        {
            if(camera->getSensor()->setExposure(config.uiSensorInitExposure) != IMG_SUCCESS)
            {
                fprintf(stderr, "WARNING: failed to set sensor to initial exposure of %u us\n", config.uiSensorInitExposure);
            }
        }
        if(config.flSensorInitGain>1.0)
        {
            if (camera->getSensor()->setGain(config.flSensorInitGain) != IMG_SUCCESS)
            {
                fprintf(stderr, "WARNING: failed to set sensor to initial gain of %lf\n", config.flSensorInitGain);
            }
        }

        return EXIT_SUCCESS;
    }

    int stopCapture(int count)
    {
        if (!mStart) {
            fprintf(stderr,"camera not start already\n");
            return 0;
        }

        // we need to acquire the buffers we already pushed otherwise the HW hangs...
        /// @ investigate that
        for ( int i = 0 ; i < count ; i++ )
        {
            ISPC::Shot shot;
            if(camera->acquireShot(shot) !=IMG_SUCCESS)
            {
                fprintf(stderr, "ERROR: acquire shot %d/%d before stopping.", i+1, count);
                //return EXIT_FAILURE;
                break;
            }
            camera->releaseShot(shot);
        }

        if ( camera->stopCapture() != IMG_SUCCESS )
        {
            fprintf(stderr, "Failed to Stop the capture!\n");
            return EXIT_FAILURE;
        }

        std::list<IMG_UINT32>::iterator it;
        for ( it = buffersIds.begin() ; it != buffersIds.end() ; it++ )
        {
            debug("deregister buffer %d\n", *it);
            if ( camera->deregisterBuffer(*it) != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to deregister buffer ID=%d!\n", *it);
                return EXIT_FAILURE;
            }
        }
        buffersIds.clear();

        printf("total number of missed frames: %d\n", missedFrames);

        return EXIT_SUCCESS;
    }

    void saveResults(const ISPC::Shot &shot, DemoConfiguration &config)
    {
        std::stringstream outputName;
        bool saved = false;
        static int savedFrames = 0;

        debug("Shot RGB=%s YUV=%s DE=%s HDR=%s TIFF=%s\n",
                FormatString(shot.RGB.pxlFormat),
                FormatString(shot.YUV.pxlFormat),
                FormatString(shot.BAYER.pxlFormat),
                FormatString(shot.HDREXT.pxlFormat),
                FormatString(shot.RAW2DEXT.pxlFormat)
        );

        if ( shot.RGB.pxlFormat != PXL_NONE && config.bSaveRGB )
        {
            outputName.str("");
            outputName << "display"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::RGB, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving RGB output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveRGB=false;
        }

        if ( shot.YUV.pxlFormat != PXL_NONE && config.bSaveYUV )
        {
            ISPC::Global_Setup globalSetup = camera->getPipeline()->getGlobalSetup();
            PIXELTYPE fmt;

            PixelTransformYUV(&fmt, shot.YUV.pxlFormat);

            outputName.str("");
            outputName << "/mnt/sd0/encoder"<<savedFrames
                << "-" << globalSetup.ui32EncWidth << "x" << globalSetup.ui32EncHeight
                << "-" << FormatString(shot.YUV.pxlFormat)
                << "-align" << (unsigned int)fmt.ui8PackedStride
                << ".yuv";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::YUV, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving YUV output to " << outputName.str() << std::endl;
                //printf("-->INFO: saving YUV output to %s\n", outputName.str());
                saved = true;
            }
            config.bSaveYUV = false;
        }

        if ( shot.BAYER.pxlFormat != PXL_NONE && config.bSaveDE )
        {
            outputName.str("");
            outputName << "dataExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Bayer, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving Bayer output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveDE = false;
        }

        if ( shot.HDREXT.pxlFormat != PXL_NONE && config.bSaveHDR )
        {
            outputName.str("");
            outputName << "hdrExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::RGB_EXT, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving HDR Extraction output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveHDR = false;
        }

        if ( shot.RAW2DEXT.pxlFormat != PXL_NONE && config.bSaveRaw2D )
        {
            outputName.str("");
            outputName << "raw2DExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Bayer_TIFF, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving RAW 2D Extraction output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveRaw2D = false;
        }
        
        if ( saved )
        {
			#if 0 //No Save FelixSetupArgs.txt file
            // we saved a file so we save the parameters it used too
            ISPC::ParameterList parameters;

            outputName.str("");
            outputName << "/mnt/sd0/FelixSetupArgs_"<<savedFrames<<".txt";

            if ( camera->saveParameters(parameters) == IMG_SUCCESS )
            {
                ISPC::ParameterFileParser::saveGrouped(parameters, outputName.str());
                std::cout << "INFO: saving parameters to " << outputName.str() << std::endl;
            }
            else
            {
                std::cerr << "WARNING: failed to save parameters" << std::endl;
            }
			#endif //0

            savedFrames++;
        }
    }

    void printInfo(const ISPC::Shot &shot, const DemoConfiguration &config) // cannot be const because modifies missedFrames
    {
        if ( shot.bFrameError )
        {
            printf("=== Frame is erroneous! ===\n");
        }

        if ( shot.iMissedFrames != 0 )
        {
            missedFrames += shot.iMissedFrames;
            printf("=== Missed %d frames before acquiring that one! ===\n",
                    shot.iMissedFrames
                  );
        }

        //
        // control modules information
        //
        const ISPC::ControlAE *pAE = camera->getControlModule<const ISPC::ControlAE>();
        const ISPC::ControlAF *pAF = camera->getControlModule<const ISPC::ControlAF>();
        const ISPC::ControlAWB_PID *pAWB = camera->getControlModule<const ISPC::ControlAWB_PID>();
        const ISPC::ControlTNM *pTNM = camera->getControlModule<const ISPC::ControlTNM>();
        const ISPC::ControlDNS *pDNS = camera->getControlModule<const ISPC::ControlDNS>();
        const ISPC::ControlLBC *pLBC = camera->getControlModule<const ISPC::ControlLBC>();
        const ISPC::ModuleDPF *pDPF = camera->getModule<const ISPC::ModuleDPF>();
        const ISPC::ModuleLSH *pLSH = camera->getModule<const ISPC::ModuleLSH>();

        printf("Sensor: exposure %.4lfms - gain %.2f\n",
                camera->getSensor()->getExposure()/1000.0, camera->getSensor()->getGain()
              );

        if ( pAE )
        {
            printf("AE %d: Metered/target brightness %f/%f (flicker %d)\n",
                    pAE->isEnabled(),
                    pAE->getCurrentBrightness(),
                    pAE->getTargetBrightness(),
                    pAE->getFlickerRejection()
                  );
        }
        if ( pAF )
        {
            printf("AF %d: state %s current distance %umm\n",
                    pAF->isEnabled(),
                    ISPC::ControlAF::StateName(pAF->getState()),
                    pAF->getBestFocusDistance()
                  );
        }
        if ( pAWB )
        {
            printf("AWB_PID %d: mode %s target temp %.0lfK - measured temp %.0lfK - correction temp %.0lfK\n",
                    pAWB->isEnabled(),
                    ISPC::ControlAWB::CorrectionName(pAWB->getCorrectionMode()),
                    pAWB->getTargetTemperature(),
                    pAWB->getMeasuredTemperature(),
                    pAWB->getCorrectionTemperature()
                  );
        }
        if ( pTNM )
        {
            printf("TNMC %d: adaptive %d local %d\n",
                    pTNM->isEnabled(),
                    pTNM->getAdaptiveTNM(),
                    pTNM->getLocalTNM()
                  );
        }
        if ( pDNS )
        {
            printf("DNSC %d\n",
                    pDNS->isEnabled()
                  );
        }
        if ( pLBC )
        {
            printf("LBC %d: correction %.0f measured %.0f\n",
                    pLBC->isEnabled(),
                    pLBC->getCurrentCorrection().lightLevel,
                    pLBC->getLightLevel()
                  );
        }
        if ( pDPF )
        {
            printf("DPF: detect %d\n",
                    pDPF->bDetect
                  );
            if ( config.bEnableDPF )
            {
                const MC_STATS_DPF &dpf = shot.metadata.defectiveStats;

                printf("    fixed=%d written=%d (threshold=%d weight=%f)\n",
                        dpf.ui32FixedPixels, dpf.ui32NOutCorrection,
                        pDPF->ui32Threshold, pDPF->fWeight
                      );
            }
        }
        if ( pLSH )
        {
            printf("LSH: matrix %d from '%s'\n",
                    pLSH->bEnableMatrix,
                    pLSH->lshFilename.c_str()
                  );
        }

    }
};

/**
 * @brief Enable mode to allow proper key capture in the application
 */
void enableRawMode(struct termios &orig_term_attr)
{
    struct termios new_term_attr;

    debug("Enable Console RAW mode\n");

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;

    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
}

#ifdef INFOTM_ISP
char getch(void)
{
    static char line[2];
    if(read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}
#endif //INFOTM_ISP

/**
 * @brief Disable mode to allow proper key capture in the application
 */
void disableRawMode(struct termios &orig_term_attr)
{
    debug("Disable Console RAW mode\n");
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
}

//====20151204 kai.wang add====
int ispGetoverlayIndex(char* pChar)
{
	int Index = 0xFFFFFFFF;
	
    if (*pChar >= '0' && *pChar <= '9')
    {
        Index = *pChar - '0' + 1;
    }
	else if(*pChar >= 65 && *pChar <= 90)
	{
		Index = *pChar - 52;
	}
	else if(*pChar >= 128 && *pChar <= 174)
	{
		Index = *pChar - 89;
	}
    else if (*pChar == '/')
    {
    	Index = 11;
    }
    else if (*pChar == ':')
    {
        Index = 12;
    }
	else if (*pChar == '.')
    {
        Index = 89;
    }
	else if (*pChar == '`')
    {
        Index = 90;
    }
	else if (*pChar == ',')
    {
        Index = 88;
    }
	else if (*pChar == 'k')
    {
        Index = 86;
    }
	else if (*pChar == 'm')
    {
        Index = 87;
    }
	else if (*pChar == 'h')
    {
        Index = 85;
    }
    else if(*pChar == 32)
    {
		Index = 0;
	}

	return Index;
}
void ispSetImgOverlay(void* ImgAddr, int ImgWidth, int ImgStride, char overlayString[])
{
	static int flag = 1;
	FILE *fp;
	int i, j;
	int count, Index, startx;
	char* pChar;
	static IMG_UINT8 imgOverlayBuf[176640];
	static IMG_UINT32 pBufSize;

	char* imgBuf = (char*)ImgAddr;
	if(flag)
	{
		fp = fopen("/etc/ovimg_timestamp.bin", "rb");
		if(NULL != fp)
		{
			pBufSize = fread(imgOverlayBuf, 1, 176640, fp);
			fclose(fp);
			flag = 0;
		}
	}
	if(0 != flag)
	{
		return;
	}

	count = 0;
	startx = 100;
	if(strlen(overlayString) < 30)
	{
		if(ImgWidth == 1920)
		{
			startx = 500;
		}
		if(ImgWidth == 1280)
		{
			startx = 200;
		}
	}
	pChar = overlayString;
	while(*pChar != '\0')
	{
		Index = ispGetoverlayIndex(pChar);
		for(i = 0; i < 40; i++)
		{
			for(j = 0; j < 32; j++)
			{	
				
				if(imgOverlayBuf[32*60*Index + i*32 + j] > 200)
				{
					*(imgBuf + (i + 0) * ImgStride + count * 32 + j + startx) = 250;//imgOverlayBuf[32*60 + i*32 + j];
				}
			}
		}

		count++;
		pChar++;
	}
}
//======================

DemoConfiguration config;
LoopCamera *loopCam = NULL;

//#define AYSNC_RELEASE
#ifdef DEBUG_TIME_DIFF
#define TIME_DIFF(start, end) (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec-start.tv_usec)/1000
#endif


#if 0
int run(int argc, char *argv[])
{
    int savedFrames = 0;

    int ret;

    //Default configuration, everything disabled but control objects are created
    ISPC::ParameterFileParser pFileParser;
/////////////////////////////
    //ISPC::Shot shot;
    ISPC::ControlAE *pAE = 0;
    ISPC::ControlAWB_PID *pAWB = 0;
    ISPC::ControlTNM *pTNM = 0;
    ISPC::ControlDNS *pDNS = 0;
    ISPC::ControlLBC *pLBC = 0;
    ISPC::ControlAF *pAF = 0;
    ISPC::Sensor *sensor = 0;//loopCam->camera->getSensor();
    ISPC::ParameterList parameters;
    	
    bool loopActive=true;
    
    //bool bSaveOriginals=false; // when saving all saves original formats from config file
    int frameCount = 0;
    double estimatedFPS = 0.0;
    
    double FPSstartTime = 0;//currTime();
    int neededFPSFrames = 0;//(int)floor(loopCam->camera.sensor->flFrameRate*FPS_COUNT_MULT);
    //struct timeval timeStart, timeEnd;
	
    //config.applyConfiguration(loopCam->camera);
    //config.bResetCapture = true; // first time we force the reset
    
    /////////////////////////////
#ifdef DEBUG_TIME_DIFF
    struct timeval time_start, time_end; 
#endif

    int sema_inited = 0;

retry_loop:   

#ifdef SUPPORT_CHANGE_RESOULTION
	if (sensor_reset_flag != 0)
	{
		Resume_IQ_Setting();
		sensor_reset_flag = 0;
	}
#endif

    if ( config.loadParameters(argc, argv) != EXIT_SUCCESS )
    {
        return EXIT_FAILURE;
    }

    //printf("-->Using sensor %s\n", config.pszSensor);
    //printf("-->Using config file %s\n", config.pszFelixSetupArgsFile);

    //load parameter file
    //ISPC::ParameterFileParser pFileParser;
    //ISPC::ParameterList parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);       
    parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);       
    if(parameters.validFlag == false) //Something went wrong parsing the parameters
    {
        std::cout <<"ERROR parsing parameter file \'"<<config.pszFelixSetupArgsFile<<"\'"<<std::endl;
        return EXIT_FAILURE;
    }
    
	loopCam = new LoopCamera(config);
    
    if(!loopCam->camera)
    {
        std::cerr << "ERROR when creating camera" << std::endl;
        return EXIT_FAILURE;
    }
    if(loopCam->camera->loadParameters(parameters) != IMG_SUCCESS)
    {
        std::cerr << "ERROR loading pipeline setup parameters from file: "<< config.pszFelixSetupArgsFile<<std::endl;
        return EXIT_FAILURE;
    }

	
    config.loadFormats(parameters);

    if(loopCam->camera->program() != IMG_SUCCESS)
    {
        std::cerr <<"ERROR programming pipeline."<<std::endl;
        return EXIT_FAILURE;
    }

#if defined(INFOTM_DISABLE_AE)
    config.controlAE = false;
#endif

#if defined(INFOTM_DISABLE_AWB)
    config.controlAWB = false;
    config.enableAWB = false;
#endif

#if !defined(INFOTM_DISABLE_AF)
    config.controlAF = false;
#endif

	sensor = loopCam->camera->getSensor();
    if ( config.controlAE/* && pAE == 0*/)
    {
        if (pAE != 0)
        {
            //pAE->~ControlAE();
            delete(pAE);
            pAE = 0;
            //printf("****(delete pAE done)*****\n");
        }
        
        pAE = new ISPC::ControlAE();

        loopCam->camera->registerControlModule(pAE);
        //printf("****(register AE done)*****\n");
    }
    if ( config.controlAWB/* && pAWB == 0*/)
    {
        if (pAWB != 0)
        {
            delete(pAWB);
            pAWB = 0;
            //printf("****(delete pAWB done)*****\n");
        }
        //if (pAWB == 0)
        {
            pAWB = new ISPC::ControlAWB_PID();
        }
        loopCam->camera->registerControlModule(pAWB);
        //printf("****(register AWB done)*****\n");
    }
    if ( config.controlTNM/*&& pTNM == 0*/) 
    {
        if (pTNM != 0)
        {
            delete(pTNM);
            pTNM = 0;
            //printf("****(delete pTNM done)*****\n");
        }
        //if (pTNM == 0)
        {
            pTNM = new ISPC::ControlTNM();
        }
        loopCam->camera->registerControlModule(pTNM);
        //printf("****(register TNM done)*****\n");
    }
    if ( config.controlDNS/* && pDNS == 0*/) 
    {
        if (pDNS != 0)
        {
            delete(pDNS);
            pDNS = 0;
            //printf("****(delete pDNS done)*****\n");
        }
        //if (pDNS == 0)
        {
            pDNS = new ISPC::ControlDNS(); 
        }
        loopCam->camera->registerControlModule(pDNS);
        //printf("****(register DNS done)*****\n");
    }
    if ( config.controlLBC/* && pLBC == 0*/)
    {
        if (pLBC != 0)
        {
            delete(pLBC);
            pLBC = 0;
            //printf("****(delete pLBC done)*****\n");
        }
        //if (pLBC == 0)
        {
            pLBC = new ISPC::ControlLBC();
        }
        loopCam->camera->registerControlModule(pLBC);
        //printf("****(register LBC done)*****\n");
    }
    if ( config.controlAF/* && pAF == 0*/)
    {
        if (pAF != 0)
        {
            delete(pAF);
            pAF = 0;
            //printf("****(delete pAF done)*****\n");
        }
        //if (pAF == 0)
        {
            pAF = new ISPC::ControlAF();
        }
        loopCam->camera->registerControlModule(pAF);
        //printf("****(register AF done)*****\n");
    }

    loopCam->camera->loadControlParameters(parameters); // load all registered controls setup

    if ( pAE )
    {
#ifdef INFOTM_ISP
		//use setting file as initial targetBrightness value. 20150819.Fred
		config.targetBrightness = pAE->getTargetBrightness();
		config.oritargetBrightness = pAE->getOriTargetBrightness();
#endif //INFOTM_ISP

        pAE->setnBuffers(config.nBuffers);
        pAE->setTargetBrightness(config.targetBrightness);
    }

    if ( pAWB )
    {
#ifdef INFOTM_ISP	
		if (!config.commandAWB)
		{
			//use setting file as initial AWBAlgorithm value. 20150819.Fred
			config.AWBAlgorithm = pAWB->getCorrectionMode();
		}
#endif //INFOTM_ISP		

        pAWB->setCorrectionMode(config.AWBAlgorithm);
    }
    //--------------------------------------------------------------

    FPSstartTime = currTime();
    neededFPSFrames = (int)floor(loopCam->camera->sensor->flFrameRate*FPS_COUNT_MULT);

    config.applyConfiguration(*(loopCam->camera));
    config.bResetCapture = true; // first time we force the reset

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
    // Initial GPIO 147 for the frame rate check.
    static int index=0;

	system ("echo 147 > /sys/class/gpio/export");
	system ("echo out > /sys/class/gpio/gpio147/direction");

#endif

    // begin added by linyun.xiong @2015-07-09 for support isp tuning
#ifdef INFOTM_ISP_TUNING
    loopCam->InitIspTuningQueue(config);
#endif
   // end added by linyun.xiong @2015-07-09 for support isp tuning

    while(!felixStop)   //Capture loop
    {
        if (frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            double cT = currTime();

            estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
            FPSstartTime = cT;
        }


#ifdef DEBUG_TIME_DIFF
        time_t t;
        struct tm tm;

        t = time(NULL); // to print info
        tm = *localtime(&t);
#endif

        if (frameCount % 1000 == 0) {
#ifdef DEBUG_TIME_DIFF
            printf("[%d/%d %02d:%02d.%02d]\n",
                tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
                );
#endif
            printf("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
                frameCount,
                sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate, config.sensorMode,
                (int)sensor->eBayerFormat,
                estimatedFPS, neededFPSFrames, config.nBuffers
                );
            
        }
        // begin added by linyun.xiong @2015-07-09 for support isp tuning
#ifdef INFOTM_ISP_TUNING
        if (1)
	 {
            switch(loopCam->IspTuningCheck(config))
            {
            case 2:
				{
#ifdef SUPPORT_CHANGE_RESOULTION

                printf("*******reset sensor *******\n");
                loopCam->camera->control.clearModules();
                pAE = 0;
                pAWB = 0;
                pTNM = 0;
                pDNS = 0;
                pLBC = 0;
                pAF = 0;
			    config.bResetCapture = true;
				frameCount = 0;
			    sensor_reset_flag = 1;

                goto retry_loop;
#endif
                break;
                }
            default:
                break;
            }
        }
#endif
        // end added by linyun.xiong @2015-07-09 for support isp tuning  
        if ( config.bResetCapture )
        {
            printf("INFO: re-starting capture to enable alternative output\n");

           	loopCam->stopCapture(config.nBuffers);

            if ( loopCam->camera->setupModules() != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to re-setup the pipeline!\n");
                return EXIT_FAILURE;
            }

            if( loopCam->camera->program() != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }

            if (loopCam->startCapture() != IMG_SUCCESS)
            {
                loopCam->camera->control.clearModules();
                pAE = 0;
                pAWB = 0;
                pTNM = 0;
                pDNS = 0;
                pLBC = 0;
                pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
				sensor_reset_flag = 1;
#endif
                goto retry_loop;
            }

            release_index = 0;
            release_count = 0;
            acquire_index = 0;
            // reset flags
            config.bResetCapture = false;
        } 
       
       if(loopCam->camera->acquireShot(shot[acquire_index]) != IMG_SUCCESS)
       {
            printf("****ERROR acquireShot. \n");
            printf("*******retry happened*******\n");
            loopCam->camera->control.clearModules();
            pAE = 0;
            pAWB = 0;
            pTNM = 0;
            pDNS = 0;
            pLBC = 0;
            pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
			sensor_reset_flag = 1;
#endif
            goto retry_loop;
        }

#if 0//def SUPPORT_SKIP_FRAMES
		if (skip_frames > 0)
        {
            reduce_frames_cnt();
            acquire_index++;
            acquire_index = acquire_index >= config.nBuffers ? 0 : acquire_index;
#ifdef INFOTM_ISP
            if (loopCam->camera->IsUpdateSelfLBC() != IMG_SUCCESS)
            {
                printf("update self LBC fail.\n");
                return EXIT_FAILURE;
            }
#endif //INFOTM_ISP
    		config.applyConfiguration(*(loopCam->camera));
    		continue;
        }
#endif

#ifdef INFOTM_ISP
        if (loopCam->camera->IsUpdateSelfLBC() != IMG_SUCCESS)
        {
            printf("update self LBC fail.\n");
            return EXIT_FAILURE;
        }
#endif //INFOTM_ISP

#ifdef DEBUG_TIME_DIFF        
        gettimeofday(&time_start, NULL);
#endif	

        acquire_index++;
        acquire_index = acquire_index >= config.nBuffers ? 0 : acquire_index;
        //release_count++;

        if(loopCam->camera->releaseShot(shot[release_index]) !=IMG_SUCCESS)
        {
            printf("ERR: Error releasing shot.\n");
        }
        
        release_index++;
        release_index = release_index >= config.nBuffers ? 0 : release_index;
        
        if(loopCam->camera->enqueueShot() !=IMG_SUCCESS)
        {
            printf("****ERROR enqueueShot. \n");
            printf("*******retry happened*******\n");
            loopCam->camera->control.clearModules();
            pAE = 0;
            pAWB = 0;
            pTNM = 0;
            pDNS = 0;
            pLBC = 0;
            pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
            sensor_reset_flag = 1;
#endif
            goto retry_loop;

        }

	
        //Update Camera loop controls configuration
        // before the release because may use the statistics
        config.applyConfiguration(*(loopCam->camera));
		
        // done after apply configuration as we already did all changes we needed
        //if (bSaveOriginals)
        //{
        //    config.loadFormats(parameters, true); // reload with defaults
        //    bSaveOriginals=false;
        //}


#ifdef DEBUG_TIME_DIFF        
        gettimeofday(&time_end, NULL);
        long time_start_ms = time_start.tv_sec*1000L + time_start.tv_usec/1000L;
        long time_end_ms   = time_end.tv_sec*1000L + time_end.tv_usec/1000L;
        long time_used     = time_end_ms - time_start_ms;
        
        if (time_used > 33) {
            printf("Use time:%ldms\n", time_used);
        }
#endif

        frameCount++;
        //usleep(10);
    }

    debug("Exiting loop\n");

    loopCam->stopCapture(0);

    return EXIT_SUCCESS;
}

#else

int run(int argc, char *argv[])
{
    int savedFrames = 0;

    int ret;

    //Default configuration, everything disabled but control objects are created
    ISPC::ParameterFileParser pFileParser;
/////////////////////////////
    //ISPC::Shot shot;
    ISPC::ControlAE *pAE = 0;
    ISPC::ControlAWB_PID *pAWB = 0;
    ISPC::ControlTNM *pTNM = 0;
    ISPC::ControlDNS *pDNS = 0;
    ISPC::ControlLBC *pLBC = 0;
    ISPC::ControlAF *pAF = 0;
    ISPC::Sensor *sensor = 0;//loopCam->camera->getSensor();
    ISPC::ParameterList parameters;
    	
    bool loopActive=true;
    
    //bool bSaveOriginals=false; // when saving all saves original formats from config file
    int frameCount = 0;
    double estimatedFPS = 0.0;
    
    double FPSstartTime = 0;//currTime();
    int neededFPSFrames = 0;//(int)floor(loopCam->camera.sensor->flFrameRate*FPS_COUNT_MULT);
    //struct timeval timeStart, timeEnd;

	int PrevSec = 0;
	char TmString[60];
	
    //config.applyConfiguration(loopCam->camera);
    //config.bResetCapture = true; // first time we force the reset
    
    /////////////////////////////
#ifdef DEBUG_TIME_DIFF
    struct timeval time_start, time_end; 
#endif

    int sema_inited = 0;

 retry_loop:   

#ifdef SUPPORT_CHANGE_RESOULTION
	if (sensor_reset_flag != 0)
	{
		Resume_IQ_Setting();
		sensor_reset_flag = 0;
	}
#endif

    if ( config.loadParameters(argc, argv) != EXIT_SUCCESS )
    {
        return EXIT_FAILURE;
    }

    //printf("-->Using sensor %s\n", config.pszSensor);
    //printf("-->Using config file %s\n", config.pszFelixSetupArgsFile);

    //load parameter file
    //ISPC::ParameterFileParser pFileParser;
    //ISPC::ParameterList parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);       
    parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);       
    if(parameters.validFlag == false) //Something went wrong parsing the parameters
    {
        std::cout <<"ERROR parsing parameter file \'"<<config.pszFelixSetupArgsFile<<"\'"<<std::endl;
        return EXIT_FAILURE;
    }
	
    buffer_count = config.nBuffers;
	if (loopCam != NULL)
	{
         int value = 0;
         int sleep_count = 0;
         sem_getvalue(&sema, &value);
         printf("value:%d, acquire_index:%d, release_index:%d,  release_count:%d, send_count:%d\n", value, acquire_index, release_index, release_count,send_count);
         if(value + release_count != config.nBuffers) {
         }
        /* wait all buffer return */;
        while(!sem_getvalue(&sema, &value) && value != config.nBuffers) {
            //sem_wait(&sema);
            if(sleep_count > 10) {
                printf("sleep timeout  \n");
                break;
            }
            usleep(20000);
            sleep_count++;
        }

        printf("release count 22:%d  \n", release_count);
        int acquire_count = config.nBuffers - release_count;
        while(release_count > 0) {
            //printf("acquire:%d, release %d shot:%p   \n",acquire_index, release_index,shot[release_index].YUV.phyAddr);
            if(loopCam->camera->releaseShot(shot[release_index]) !=IMG_SUCCESS)
            {
                printf("ERR: Error releasing shot[%d].\n", release_index);
            }
            release_count--;
            release_index++;
            release_index = release_index >= config.nBuffers ? 0 : release_index;
            
        }

        loopCam->stopCapture(acquire_count);

        pAE = 0;pAWB = 0;pTNM = 0;pDNS = 0;pLBC = 0;pAF = 0;

 
		delete (loopCam);
		loopCam = NULL;
        acquire_index = 0;
        release_index = 0;
        release_count = 0;

	}


    if (sema_inited == 0) {
        sem_init(&sema, 0, config.nBuffers);
        sema_inited = 1;
    }
	loopCam = new LoopCamera(config);


    if(!loopCam->camera)
    {
        std::cerr << "ERROR when creating camera" << std::endl;
        return EXIT_FAILURE;
    }
    if(loopCam->camera->loadParameters(parameters) != IMG_SUCCESS)
    {
        std::cerr << "ERROR loading pipeline setup parameters from file: "<< config.pszFelixSetupArgsFile<<std::endl;
        return EXIT_FAILURE;
    }

	
    config.loadFormats(parameters);

    if(loopCam->camera->program() != IMG_SUCCESS)
    {
        std::cerr <<"ERROR programming pipeline."<<std::endl;
        return EXIT_FAILURE;
    }

#if defined(INFOTM_DISABLE_AE)
    config.controlAE = false;
#endif

#if defined(INFOTM_DISABLE_AWB)
    config.controlAWB = false;
    config.enableAWB = false;
#endif

#if defined(INFOTM_DISABLE_AF)
    config.controlAF = false;
#endif

	sensor = loopCam->camera->getSensor();
    if ( config.controlAE/* && pAE == 0*/)
    {
        if (pAE != 0)
        {
            //pAE->~ControlAE();
            delete(pAE);
            pAE = 0;
            //printf("****(delete pAE done)*****\n");
        }
        
        pAE = new ISPC::ControlAE();

        loopCam->camera->registerControlModule(pAE);
        //printf("****(register AE done)*****\n");
    }
    if ( config.controlAWB/* && pAWB == 0*/)
    {
        if (pAWB != 0)
        {
            delete(pAWB);
            pAWB = 0;
            //printf("****(delete pAWB done)*****\n");
        }
        //if (pAWB == 0)
        {
            pAWB = new ISPC::ControlAWB_PID();
        }
        loopCam->camera->registerControlModule(pAWB);
        //printf("****(register AWB done)*****\n");
    }
    if ( config.controlTNM/*&& pTNM == 0*/) 
    {
        if (pTNM != 0)
        {
            delete(pTNM);
            pTNM = 0;
            //printf("****(delete pTNM done)*****\n");
        }
        //if (pTNM == 0)
        {
            pTNM = new ISPC::ControlTNM();
        }
        loopCam->camera->registerControlModule(pTNM);
        //printf("****(register TNM done)*****\n");
    }
    if ( config.controlDNS/* && pDNS == 0*/) 
    {
        if (pDNS != 0)
        {
            delete(pDNS);
            pDNS = 0;
            //printf("****(delete pDNS done)*****\n");
        }
        //if (pDNS == 0)
        {
            pDNS = new ISPC::ControlDNS(); 
        }
        loopCam->camera->registerControlModule(pDNS);
        //printf("****(register DNS done)*****\n");
    }
    if ( config.controlLBC/* && pLBC == 0*/)
    {
        if (pLBC != 0)
        {
            delete(pLBC);
            pLBC = 0;
            //printf("****(delete pLBC done)*****\n");
        }
        //if (pLBC == 0)
        {
            pLBC = new ISPC::ControlLBC();
        }
        loopCam->camera->registerControlModule(pLBC);
        //printf("****(register LBC done)*****\n");
    }
    if ( config.controlAF/* && pAF == 0*/)
    {
        if (pAF != 0)
        {
            delete(pAF);
            pAF = 0;
            //printf("****(delete pAF done)*****\n");
        }
        //if (pAF == 0)
        {
            pAF = new ISPC::ControlAF();
        }
        loopCam->camera->registerControlModule(pAF);
        //printf("****(register AF done)*****\n");
    }

    loopCam->camera->loadControlParameters(parameters); // load all registered controls setup

    if ( pAE )
    {
#ifdef INFOTM_ISP
		//use setting file as initial targetBrightness value. 20150819.Fred
		config.targetBrightness = pAE->getTargetBrightness();
		config.oritargetBrightness = pAE->getOriTargetBrightness();
#endif //INFOTM_ISP

        //pAE->setnBuffers(config.nBuffers);
        pAE->setTargetBrightness(config.targetBrightness);
    }

    if ( pAWB )
    {
#ifdef INFOTM_ISP	
		if (!config.commandAWB)
		{
			//use setting file as initial AWBAlgorithm value. 20150819.Fred
			config.AWBAlgorithm = pAWB->getCorrectionMode();
		}
#endif //INFOTM_ISP		

        pAWB->setCorrectionMode(config.AWBAlgorithm);
    }
    //--------------------------------------------------------------

    FPSstartTime = currTime();
    neededFPSFrames = (int)floor(loopCam->camera->sensor->flFrameRate*FPS_COUNT_MULT);

    config.applyConfiguration(*(loopCam->camera));
    config.bResetCapture = true; // first time we force the reset

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
    // Initial GPIO 147 for the frame rate check.
    static int index=0;

	system ("echo 147 > /sys/class/gpio/export");
	system ("echo out > /sys/class/gpio/gpio147/direction");

#endif
    // begin added by linyun.xiong @2015-07-09 for support isp tuning
#ifdef INFOTM_ISP_TUNING
    loopCam->InitIspTuningQueue(config);
#endif
   // end added by linyun.xiong @2015-07-09 for support isp tuning
    while(!felixStop)   //Capture loop
    {
        if (frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            double cT = currTime();

            estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
            FPSstartTime = cT;
        }

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
		// Set this GPIO pin as high to count frame rate.
		system ("echo 1 > /sys/class/gpio/gpio147/value");

#endif

#ifdef DEBUG_TIME_DIFF
        time_t t;
        struct tm tm;

        t = time(NULL); // to print info
        tm = *localtime(&t);
#endif

        if (frameCount % 1000 == 0) {
#ifdef DEBUG_TIME_DIFF
            printf("[%d/%d %02d:%02d.%02d]\n",
                tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
                );
#endif
            printf("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
                frameCount,
                sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate, config.sensorMode,
                (int)sensor->eBayerFormat,
                estimatedFPS, neededFPSFrames, config.nBuffers
                );
            
        }
        // begin added by linyun.xiong @2015-07-09 for support isp tuning
#ifdef INFOTM_ISP_TUNING
        if (1)
	 {
            switch(loopCam->IspTuningCheck(config))
            {
            case 2:
				{
#ifdef SUPPORT_CHANGE_RESOULTION

                printf("*******reset sensor *******\n");
                loopCam->camera->control.clearModules();
                pAE = 0;
                pAWB = 0;
                pTNM = 0;
                pDNS = 0;
                pLBC = 0;
                pAF = 0;
			config.bResetCapture = true;
				frameCount = 0;
			sensor_reset_flag = 1;
                goto retry_loop;
#endif
                break;
                }
            default:
                break;
            }
        }
#endif
        // end added by linyun.xiong @2015-07-09 for support isp tuning  
        if ( config.bResetCapture )
        {
            printf("INFO: re-starting capture to enable alternative output\n");

           	loopCam->stopCapture(config.nBuffers);

            if ( loopCam->camera->setupModules() != IMG_SUCCESS )
            {
                printf("Failed to re-setup the pipeline!\n");
                return EXIT_FAILURE;
            }
            if( loopCam->camera->program() != IMG_SUCCESS )
            {
                printf("Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }
            if (loopCam->startCapture() != IMG_SUCCESS)
            {
                loopCam->camera->control.clearModules();
                pAE = 0;
                pAWB = 0;
                pTNM = 0;
                pDNS = 0;
                pLBC = 0;
                pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
				sensor_reset_flag = 1;
#endif
                goto retry_loop;
            }

            // reset flags
            config.bResetCapture = false;
        } 
        int value = 0;
        sem_getvalue(&sema, &value);
        if(value > 0)
		{
			usleep(10);
		}
		else
		{
            printf("no buffer used \n");
        }
        sem_wait(&sema);
#ifndef AYSNC_RELEASE 
        pthread_mutex_lock(&isp_lock);
        //if(release_count > 1)
       // printf("release count:%d  \n", release_count);
        struct timeval time1, time2;
		gettimeofday(&time1, NULL);

        while(release_count > 0) {
            //printf("acquire:%d, release %d send_count:%d   \n",acquire_index, release_index, send_count);
#if 0
            loopCam->camera->releaseShot(shot[release_index]);
#else
			if(loopCam->camera->releaseShot(shot[release_index]) !=IMG_SUCCESS)
            {
                printf("ERR: Error releasing shot[%d].\n", release_index);
                pthread_mutex_unlock(&isp_lock);
                sem_post(&sema);
	            loopCam->camera->control.clearModules();
	            pAE = 0;
	            pAWB = 0;
	            pTNM = 0;
	            pDNS = 0;
	            pLBC = 0;
	            pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
				sensor_reset_flag = 1;
#endif
                goto retry_loop;
            }
#endif
            release_count--;
           
			if(loopCam->camera->enqueueShot() !=IMG_SUCCESS)
			{
				printf("ERROR enqueuing shot.  \n");
				pthread_mutex_unlock(&isp_lock);
				sem_post(&sema);
				loopCam->camera->control.clearModules();
				pAE = 0;
				pAWB = 0;
				pTNM = 0;
				pDNS = 0;
				pLBC = 0;
				pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
				sensor_reset_flag = 1;
#endif
				goto retry_loop;
			}

            release_index++;
            release_index = release_index >= config.nBuffers ? 0 : release_index;
        }

        pthread_mutex_unlock(&isp_lock);
#endif 

        if(tm.tm_sec != PrevSec)
        {
            sprintf(TmString, "%04d/%02d/%02d %02d:%02d:%02d ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            if(OV_Longitude_String[0]!=0 && OV_Latitude_String[0]!=0 && OV_Speed_String[0]!=0)
            {
                strcat(TmString, OV_Longitude_String);
                strcat(TmString, OV_Latitude_String);
				strcat(TmString, OV_Speed_String);
            }

            if(OV_Plate_String[7]>0)
            {
                strcat(TmString, OV_Plate_String);
				TmString[strlen(TmString)-1] = 0;
            }
            memset((TmString+55), 0, 5*sizeof(char));
            PrevSec = tm.tm_sec;
        }
       if(loopCam->camera->acquireShot(shot[acquire_index]) != IMG_SUCCESS)
       {
            printf("****ERROR acquireShot. \n");
            printf("*******retry happened*******\n");
            sem_post(&sema);
            loopCam->camera->control.clearModules();
            pAE = 0;
            pAWB = 0;
            pTNM = 0;
            pDNS = 0;
            pLBC = 0;
            pAF = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
			sensor_reset_flag = 1;
#endif
            goto retry_loop;
        }

#ifdef SUPPORT_SKIP_FRAMES
		if (skip_frames > 0)
        {
            reduce_frames_cnt();
            acquire_index++;
            acquire_index = acquire_index >= config.nBuffers ? 0 : acquire_index;
            release_count++;
            sem_post(&sema);
            usleep(200);
#ifdef INFOTM_ISP
            if (loopCam->camera->IsUpdateSelfLBC() != IMG_SUCCESS)
            {
                printf("update self LBC fail.\n");
                sem_post(&sema);
                return EXIT_FAILURE;
            }
#endif //INFOTM_ISP
    		config.applyConfiguration(*(loopCam->camera));
    		continue;
        }
#endif

#ifdef INFOTM_ISP
        if (loopCam->camera->IsUpdateSelfLBC() != IMG_SUCCESS)
        {
            printf("update self LBC fail.\n");
            sem_post(&sema);
            return EXIT_FAILURE;
        }
#endif //INFOTM_ISP

#ifdef DEBUG_TIME_DIFF        
        gettimeofday(&time_start, NULL);
#endif		

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
        // Set this GPIO pin as low to count frame rate.
        system ("echo 0 > /sys/class/gpio/gpio147/value");
#endif
	
        //Update Camera loop controls configuration
        // before the release because may use the statistics
        config.applyConfiguration(*(loopCam->camera));
		
        // done after apply configuration as we already did all changes we needed
        //if (bSaveOriginals)
        //{
        //    config.loadFormats(parameters, true); // reload with defaults
        //    bSaveOriginals=false;
        //}
        
        void *virAddr = 0;
        void *phyAddr = 0;
        int size      = 0;
        int width     = 0;
        int height    = 0;
        int stride    = 0;
        int valid_width = 0;
        int valid_height = 0;

#if defined(SUPPORT_CHANGE_RESOULTION) && defined(ISP_SUPPORT_SCALER) // added by linyun.xiong @2015-12-03

        virAddr = const_cast<IMG_UINT8 *>(shot[acquire_index].YUV.data);
        phyAddr = shot[acquire_index].YUV.phyAddr;
        size    = shot[acquire_index].YUV.stride * shot[acquire_index].YUV.height *1.5;
        width   = shot[acquire_index].YUV.stride;
        height  = shot[acquire_index].YUV.vstride;
 
        /* get scaler size */
        ISPC::ScalerImg EscImg;
        if (ISPC::Save::GetEscImg(&EscImg, *(loopCam->camera->getPipeline()), shot[acquire_index]) == IMG_SUCCESS)
        {		
            valid_width   = EscImg.width;
            valid_height  = EscImg.height;
            //printf("-->ecs width %d, height %d\n", EscImg.width, EscImg.height);

        }
        else 
        {
            valid_width   = shot[acquire_index].YUV.width;
            valid_height  = shot[acquire_index].YUV.height;
        }
#else
        virAddr = const_cast<IMG_UINT8 *>(shot[acquire_index].YUV.data);
        phyAddr = shot[acquire_index].YUV.phyAddr;
        size    = shot[acquire_index].YUV.stride * shot[acquire_index].YUV.height *1.5;
        width   = shot[acquire_index].YUV.width;
        height  = shot[acquire_index].YUV.height;

        valid_width   = shot[acquire_index].YUV.width;
        valid_height  = shot[acquire_index].YUV.height;
#endif
        if (OV_Time_Flag)
        {
//            ispSetImgOverlay(virAddr, valid_width, shot[acquire_index].YUV.stride, TmString); 
        }
        
		AdasPrepareData(virAddr, width, height, shot[acquire_index].YUV.stride);
		AdasResultProc(virAddr, width, height);

        acquire_index++;
        acquire_index = acquire_index >= config.nBuffers ? 0 : acquire_index;

        ISP_AV_BUFFER isp_buffer;
        
        struct timeval timestamp; 
        gettimeofday(&timestamp, NULL);

        isp_buffer.width     = width;
        isp_buffer.height    = height;

        isp_buffer.valid_width     = valid_width;
        isp_buffer.valid_height    = valid_height;
        isp_buffer.virt_addr = virAddr;
        isp_buffer.phy_addr = phyAddr;
        isp_buffer.size      = size;
        isp_buffer.timestamp = timestamp;
        isp_buffer.buffer    = NULL;

        isp_buffer.image_type = ISP_IMAGE_YUV_420SP;
        isp_buffer.env_brightness = GetIEnvBrightness();

        g_bufReport(&isp_buffer);
#if 0
            static int64_t last_time = -1ll;
            if(last_time > 0) {
                long interval = timestamp.tv_sec * 1000ll + timestamp.tv_usec /1000 - last_time;
                if(interval > 35 || interval < 25) {
                    //printf("interval frame time:%ldms \n", interval);
                }
            }

           last_time = timestamp.tv_sec * 1000ll + timestamp.tv_usec /1000;
#endif

#ifdef DEBUG_TIME_DIFF        
        gettimeofday(&time_end, NULL);
        long time_start_ms = time_start.tv_sec*1000L + time_start.tv_usec/1000L;
        long time_end_ms   = time_end.tv_sec*1000L + time_end.tv_usec/1000L;
        long time_used     = time_end_ms - time_start_ms;
        
        if (time_used > 33) {
            printf("Use time:%ldms\n", time_used);
        }
#endif

        frameCount++;
        //usleep(1000);
    }

    debug("Exiting loop\n");

    loopCam->stopCapture(0);
    sem_destroy(&sema);
    sema_inited = 0;

    return EXIT_SUCCESS;
}
#endif

#define MAX_FELIX_PARAM_CNT 15
#define MAX_FELIX_PARAM_LEN 64
/*
    sensorMode: 
        0 - 720P  (1280x720)
        1 - 1080P (1920x1080)
 */

extern "C" int felix_stop(void) 
{
    felixStop = true;
    return 0;
}


#if 1
//extern "C" int felix_start(buffer_report bufReport, int argc, char*argv[])
//extern "C" int felix_start(buffer_report bufReport, char* param_list, int param_cnt)
extern "C" int felix_start(buffer_report bufReport)

#else
extern "C" int felix_start(T_LOOP_ARRAY *shareMem, int sensorMode, unsigned int isMipi)
#endif
{
    int ret;
    struct termios orig_term_attr;

    felixStop = false;
    g_bufReport = bufReport;
    
    // begin added by linyun.xiong @2015-11-03     
    int items_argc = ITEMS_ARGC_VALUE;  
    char items_argv[ITEMS_ARGC_VALUE][MAX_FELIX_PARAM_LEN] = {0};   
    char* ptr_argv[MAX_FELIX_PARAM_CNT + 3];

    memcpy(items_argv[0], "fleix_lib", 9); // don't change it
    memcpy(items_argv[1], "-sensor", 7); // don't change it     
    cfg_shm_getSensorName(items_argv[2]);   

    if (strcmp(items_argv[2], "OV2710") == 0)   
    {   
       system("insmod /ov2710_mipi.ko");   
    }   
    else if (strcmp(items_argv[2], "AR330MIPI") == 0)   
    {   
       system("insmod /ar0330_mipi.ko");   
    }   
    else if (strcmp(items_argv[2], "OV4689") == 0)  
    {   
       system("insmod /ov4689_mipi.ko");   
    }   
    else if (strcmp(items_argv[2], "AR330DVP") == 0)    
    {   
       system("insmod /ar0330_dvp.ko");    
    }   
#if defined(FOR_ZHUOYING_PRJ)
    else if (strcmp(items_argv[2], "IMX322DVP") == 0)    
    {   
       system("insmod /imx322_dvp.ko");    
    }   
#endif
    else    
    {   
       printf("-->not found app.sensor.name\n");   
       return EXIT_FAILURE;    
    }   

    memcpy(items_argv[3], "-setupFile", 10); // don't change it     
    cfg_shm_getSensorSetupFile(items_argv[4]);  

    memcpy(items_argv[5], "-nBuffers", 9); // don't change it   
    cfg_shm_getSensorBufferSize(items_argv[6]); 

    memcpy(items_argv[7], "-sensorMode", 11); // don't change it    
    cfg_shm_getSensorMode(items_argv[8]);   

    memcpy(items_argv[9], "-chooseAWB", 10); // don't change it     
    cfg_shm_getSensorAWBMode(items_argv[10]);

#if defined(SUPPORT_CHANGE_RESOULTION) && !defined(ISP_SUPPORT_SCALER) // added by linyun.xiong @2015-12-03
    if (strcmp(items_argv[2], "OV2710") == 0)
    {
    	switch (atoi(items_argv[8]))
    	{
    	case 0:
    		cur_resolution = VIDEO_1080P;
    		break;
    	case 1:
    		cur_resolution = VIDEO_720P;
    		break;
    	default:
    		printf("invalid sensorMode id\n");
    		break;
    	}
    }
    cur_sensor_mode = atoi(items_argv[8]);
#endif

    if (0) // ??? for test
    {
       int i;
       for (i = 1; i < items_argc; i+= 2) {
           printf("-->%10s %10s\n", items_argv[i], items_argv[i+1]);
       }
    }

    int i;
    ptr_argv[0] = items_argv[0];
    for (i = 1; i < items_argc; i+= 2) {
    	//printf("\t%10s %10s\n", argv[i], items_argv[i+1]);
    	ptr_argv[i] = items_argv[i];
    	ptr_argv[i+1] = items_argv[i+1];
    }
    // end added by linyun.xiong @2015-11-03 

	ret = run(items_argc, ptr_argv);

    return EXIT_SUCCESS;
}

int Inft_Isp_Clear_FB_1(void)
{
    //Clear fb1 first
    struct fb_var_screeninfo vinfo;
    long int screensize = 0;

    int fb1fd = open("/dev/fb1", O_RDWR);
    if (!fb1fd) {
        printf("Error: cannot open framebuffer device.\n");
        return -1;
    }    

    if (ioctl(fb1fd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error: reading variable information.\n");
        close(fb1fd);
        return -1;
    }    

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8; 

    unsigned int *data = (unsigned int *) malloc(screensize);
    //memset(data, 0, screensize);
    int i = 0; 
    for(i = 0; i < screensize / 4; i++) {
        data[i] = 0xFF010101;
    }

    //map object fb1
    char *fbp = (char *)mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fb1fd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        close(fb1fd);
        return -1;
    }

    //push data to screen
    memcpy(fbp, data, screensize);

    free(data);
    munmap(fbp, screensize);
    close(fb1fd);
}


int Inft_Isp_Init_FB()
{
    fbfd = open(MAINSCREEN_FB_NAME, O_RDWR);
    struct fb_var_screeninfo vinfo; 
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.nonstd = 2;
    //vinfo.xres = 1920; //the dss driver has already hack the res to 1920x1088;
    //vinfo.yres = 1088;
    ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);

    return 0;
}


static int Inft_Isp_Display_SetInputConfig(int width, int height, int valid_width, int valid_height, void *phy_addr, int format)
{
#define IMAPFB_CONFIG_VIDEO_SIZE   20005
    typedef struct {
        unsigned int width;
        unsigned int height;
        unsigned int win_width;
        unsigned int win_height;
        void *   phy_addr;
        int      format;
    } imapfb_video_size;

    static imapfb_video_size last_video_size = {
        .width    = 0,
        .height   = 0,
        .win_width    = 0,
        .win_height   = 0,
        .phy_addr = NULL,//Invalid first
        .format   = -1, //Invalid first
    };

    if(!g_fb_0_reset && last_video_size.width == width && 
            last_video_size.height == height &&
            last_video_size.win_width == valid_width &&
            last_video_size.win_height == valid_height &&
            last_video_size.format == format) {
        return 1;
    }
        
    
    imapfb_video_size videoSize;
    videoSize.width    = width;
    videoSize.height   = height; 
    videoSize.win_width    = valid_width;
    videoSize.win_height   = valid_height; 
    videoSize.phy_addr = phy_addr;
    videoSize.format   = format;
    memcpy(&last_video_size, &videoSize, sizeof(last_video_size));

    if (ioctl(fbfd, IMAPFB_CONFIG_VIDEO_SIZE, &videoSize) == -1) {
        printf("%s resolution:%d*%d win_size:%d*%d format:%d addr:%p  failed\n",__FUNCTION__, width, 
        height, valid_width, valid_height, format, phy_addr);
        return -1;
    }

    pthread_mutex_lock(&isp_lock);
    if(g_fb_0_reset == 1) {
        g_fb_0_reset = 0;
    }
    pthread_mutex_unlock(&isp_lock);
    printf("%s resolution:%d*%d win_size:%d*%d format:%d addr:%p\n",__FUNCTION__, width, 
    height, valid_width, valid_height, format, phy_addr);
    return 0;
}


int Inft_Isp_Buffer_Display(ISP_AV_BUFFER *isp_buffer)
{
    Inft_Isp_Display_SetInputConfig(isp_buffer->width, isp_buffer->height, isp_buffer->valid_width, isp_buffer->valid_height,isp_buffer->phy_addr, 2);

    struct fb_var_screeninfo vinfo;
#if 1
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.reserved[0] = (unsigned int )isp_buffer->phy_addr;
    ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);
#endif
    static int i=0;
    if(i++%1000==0)
        printf("%s done. phy addr:%p \n",__FUNCTION__, isp_buffer->phy_addr);

	if (i == 1)
	{
		//SetISPConfigItemState(WB_ID, WB_NONE);
		SetISPConfigItemState(EV_ID, EV_MINUS_2_0);
	}
    if (g_fb_1_clear && i >= 25) {
		g_fb_1_clear = 0;
#ifdef SUPPORT_CHANGE_RESOULTION
		if (sensor_reset_flag == 0)
#endif
		{
			//SetISPConfigItemState(WB_ID, AWB_MODE);
			SetISPConfigItemState(EV_ID, EV_0_0);
			Inft_Isp_Clear_FB_1();
	        system("cd /small_ui;./small_ui&");
		}
    }
    return 0;
}

int Inft_Isp_Init_Msg()
{
    g_msg_snd_id = msgget(ISP_AVS_BINDER_KEY,0);
    if ( g_msg_snd_id > -1)
    {
        msgctl(g_msg_snd_id, IPC_RMID, 0);
    }  
    
    g_msg_snd_id = msgget(ISP_AVS_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
    printf("Inft_Isp_Snd_Msg_Callback id:0x%x, key:0x%x.\n", g_msg_snd_id, ISP_AVS_BINDER_KEY);

    if(g_msg_snd_id < 0)
    { 
        return -1;
    }

    return 0;
}

void Inft_Isp_Snd_Msg_Callback(ISP_AV_BUFFER * isp_buffer)
{
    ISP_AV_MSG_INFO msg_info;
    memcpy(&msg_info.isp_buffer, isp_buffer, sizeof(ISP_AV_BUFFER));

    static int i=0;
    if(i++%1000==0)
        printf("%s buffer:%p %p size:%d\n", __FUNCTION__, 
                isp_buffer->virt_addr, isp_buffer->phy_addr, isp_buffer->size);

    if(g_msg_snd_id < 0)
    {
        printf("Inft_Isp_Snd_Msg handle is error.\n");
    }

    pthread_mutex_lock(&isp_lock);
    if(g_avs_exist) {
        //printf("send buffer to avserver \n");
        send_count++;
        msg_info.isp_av_msg_to   = AV_PORT;
        msg_info.isp_av_msg_from = ISP_PORT;
        msg_info.isp_av_command = ISP_AV_CMD_SEND_BUFFER;
    } else {
        msg_info.isp_av_msg_to   = ISP_PORT;
        msg_info.isp_av_msg_from = ISP_PORT;
        msg_info.isp_av_command = ISP_AV_CMD_RELEASE_BUFFER;
    }

    msgsnd(g_msg_snd_id, &msg_info, ISP_AV_MSG_SIZE, 0);
    pthread_mutex_unlock(&isp_lock);
} 

#ifdef ROTATION
static int isp_data_rotation(PPInst ppInst, unsigned int dst_addr, unsigned int src_addr, int width, int height, int valid_width, int valid_height)
{
    PPResult ppRet;
    PPConfig pPpConf;

#ifdef TIME_DEBUG
    struct timeval time;
    gettimeofday(&time, NULL);
#endif

    /*  First get the default PP settings  */
    ppRet = PPGetConfig(ppInst, &pPpConf);
    if(ppRet != PP_OK){
        /*  Handle errors here  */
        printf("PPGetConfig error(%d)\n", ppRet);
        goto PP_ERR;
    }

    /*  setup PP  */
    pPpConf.ppInImg.width      = width;
    pPpConf.ppInImg.height     = height;
    pPpConf.ppInImg.videoRange = 1;
    pPpConf.ppInImg.bufferBusAddr   = src_addr;
    pPpConf.ppInImg.bufferCbBusAddr = pPpConf.ppInImg.bufferBusAddr + width * height;
    pPpConf.ppInImg.bufferCrBusAddr = 0;//pPpConf.ppInImg.bufferCbBusAddr + (pPpConf.ppInImg.width *  pPpConf.ppInImg.height / 4);
    pPpConf.ppInImg.pixFormat       = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;

    if (valid_width != width ||
            valid_height != height) {
        pPpConf.ppInCrop.enable = 1;
        pPpConf.ppInCrop.originX = 0;
        pPpConf.ppInCrop.originY = 0;
        pPpConf.ppInCrop.width  = valid_width;
        pPpConf.ppInCrop.height = valid_height;
    } else {
        pPpConf.ppInCrop.enable = 0;
    }  

    pPpConf.ppInRotation.rotation = PP_ROTATION_LEFT_90;

    pPpConf.ppOutImg.width     = valid_height;
    pPpConf.ppOutImg.height    = valid_width;
    pPpConf.ppOutImg.bufferBusAddr  = dst_addr;
    pPpConf.ppOutImg.bufferChromaBusAddr = pPpConf.ppOutImg.bufferBusAddr + valid_width * valid_height;
    pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_YCRCB_4_2_0_SEMIPLANAR;
    pPpConf.ppOutRgb.alpha     = 0xFF; /* */

    ppRet = PPSetConfig(ppInst, &pPpConf);
    if(ppRet != PP_OK){
        /*  Handle errors here  */
        goto PP_ERR;
    }

    /* do PP  */
    ppRet = PPGetResult(ppInst);
    if(ppRet != PP_OK){
        /*  Handle errors here  */
        goto PP_ERR;
    }

#ifdef TIME_DEBUG
    struct timeval end;
    gettimeofday(&end, NULL);
    fprintf(stderr, "do pp (%d) time:%ldms \n", flag, (end.tv_sec - time.tv_sec) * 1000 + (end.tv_usec - time.tv_usec)/1000);
#endif

    return 0;

PP_ERR:
    printf("isp do pp error:%d \n", ppRet);
    return -1;
}
#endif 

void *Inft_Isp_Rcv_Msg_Callback(void * args)
{

#ifdef ROTATION

#define DISPLAY_BUFFER_COUNT  3

    ALCCTX  alc_inst = NULL; 
    IM_Buffer display_buffer[DISPLAY_BUFFER_COUNT];
    PPInst ppInst = NULL;
    int free_count = 0; //use to free buffer
    int flags =  ALC_FLAG_ALIGN_64BYTES | ALC_FLAG_PHYADDR;
    int size = 1920 * 1088 * 3 / 2;
    int display_index = 0;
    int i;
    if(alc_open(&alc_inst, "isp bin") != IM_RET_OK) {
        printf("isp.bin alc_open failed  \n");
        return NULL;
    }

    memset(display_buffer, 0, DISPLAY_BUFFER_COUNT * sizeof(IM_Buffer));
    for (i = 0; i < DISPLAY_BUFFER_COUNT; i++) 
    {
        if(alc_alloc(alc_inst, size,  flags, &display_buffer[i]) != IM_RET_OK){
            printf("alc_alloc(in) failed");
            goto ERROR;
        }
    }
    display_index = 0;

    printf("init rotation resource \n");
    if (PPInit(&ppInst, 0) != PP_OK) {
        printf("PP init failed  \n");
        goto ERROR;
    }
#endif 

#ifdef SUPPORT_CHANGE_RESOULTION
	msg_recv_thread_loops_flag = 1;
    while (msg_recv_thread_loops_flag)
#else
	while (1)
#endif
    {
        ISP_AV_MSG_INFO msg_info;
        msg_info.isp_av_msg_to = ISP_PORT;
        int ret = msgrcv(g_msg_snd_id, &msg_info, ISP_AV_MSG_SIZE, msg_info.isp_av_msg_to, 0);
        if(ret < 0)
        {
            usleep(/*10000*/1000);
            continue;
        }

        switch(msg_info.isp_av_command) 
        {
            case ISP_AV_CMD_CONNECT:
                printf("ISP Connect From:%ld,send_count:%d\n", msg_info.isp_av_msg_from,send_count);
#ifdef ROTATION
                free_count = 0;
#endif 
                pthread_mutex_lock(&isp_lock);
                g_avs_exist = 1;
                pthread_mutex_unlock(&isp_lock);

                break;
            case ISP_AV_CMD_DISCONNECT:
                printf("ISP Disconnect From:%ld\n", msg_info.isp_av_msg_from);

                pthread_mutex_lock(&isp_lock);

                printf("send to avserver count :%d ,acquire_index:%d, release_index:%d  \n", send_count, acquire_index, release_index);
                while(send_count > 0) {
#ifdef AYSNC_RELEASE


                    if(loopCam->camera->releaseShot(shot[release_index]) !=IMG_SUCCESS)
                    {
                        printf("ERR: Error releasing shot.\n");
                    }
                    release_index++;
                    release_index = release_index >= 5 ? 0 : release_index;

                    if(loopCam->camera->enqueueShot() !=IMG_SUCCESS)
                    {
                        std::cerr<<"ERROR enqueuing shot."<<std::endl;
                    }

#else

					release_count++;

#endif
                    send_count--;

					sem_post(&sema);


                }

                if (g_avs_exist) {
                    g_fb_0_reset = 1;
                    g_avs_exist = 0;
                }

                pthread_mutex_unlock(&isp_lock);

                break;
            case ISP_AV_CMD_RELEASE_BUFFER:
                if (msg_info.isp_av_msg_from == ISP_PORT) {
                    if(g_avs_exist == 1) {

                        pthread_mutex_lock(&isp_lock);
                        release_count++;
	                    sem_post(&sema);
                        pthread_mutex_unlock(&isp_lock);
                        printf("isp is connected to avserver \n");
                        break;
                    }
#ifdef ROTATION
                    for(i = 0; i < DISPLAY_BUFFER_COUNT; i++) 
                    {
                        if(display_buffer[i].vir_addr == NULL) {
                            if(alc_alloc(alc_inst, size, flags, &display_buffer[i]) != IM_RET_OK){
                                printf("alc_alloc(in) failed");
                                goto ERROR;
                            }
                        }
                    }

                    //ROTATION
                    //printf("isp picture size:%dx%d  \n",  msg_info.isp_buffer.width, msg_info.isp_buffer.height);
                    isp_data_rotation(ppInst, display_buffer[display_index].phy_addr, \
                            (unsigned int )msg_info.isp_buffer.phy_addr, msg_info.isp_buffer.width, msg_info.isp_buffer.height, \
                            msg_info.isp_buffer.valid_width, msg_info.isp_buffer.valid_height);

                    {
                        int temp = msg_info.isp_buffer.valid_width;
                        msg_info.isp_buffer.valid_width = msg_info.isp_buffer.valid_height;
                        msg_info.isp_buffer.valid_height = temp;

                        msg_info.isp_buffer.width = msg_info.isp_buffer.valid_width;
                        msg_info.isp_buffer.height = msg_info.isp_buffer.valid_height;
                        msg_info.isp_buffer.phy_addr = (void *)display_buffer[display_index].phy_addr;
                    }
#endif 
                        Inft_Isp_Buffer_Display(&msg_info.isp_buffer);
#ifdef ROTATION
                    display_index++;
                    display_index = display_index >= DISPLAY_BUFFER_COUNT ? 0 : display_index; 
#endif 
                } else {
               }

               pthread_mutex_lock(&isp_lock);

                if(msg_info.isp_av_msg_from != ISP_PORT) {
                    if(g_avs_exist == 0) {
                        printf("avserver is disconnected from isp \n");

                        pthread_mutex_unlock(&isp_lock);

                        break;
                    } else {
#ifdef ROTATION
                        free_count++;
                        if(free_count > 10) {
                            for(i = 0; i < DISPLAY_BUFFER_COUNT; i++) 
                            {
                                if(display_buffer[i].vir_addr != NULL) {
                                    printf("free isp buffer:%p   \n", display_buffer[i].vir_addr);
                                    alc_free(alc_inst, &display_buffer[i]);
                                    memset(&display_buffer[i], 0, sizeof(IM_Buffer));
                                }
                            }
                        }
#endif 
 
                        send_count--;
                    }
                }
#ifdef AYSNC_RELEASE
                struct timeval time1;

                gettimeofday(&time1, NULL);
                printf("release %d shot:%p   \n",release_index,shot[release_index].YUV.phyAddr);

                if(loopCam->camera->releaseShot(shot[release_index]) !=IMG_SUCCESS)
                {
                    printf("ERR: Error releasing shot.\n");
                }
                release_index++;
                release_index = release_index >= buffer_count ? 0 : release_index;

                if(loopCam->camera->enqueueShot() !=IMG_SUCCESS)
                {
                    std::cerr<<"ERROR enqueuing shot."<<std::endl;
                }

                struct timeval time2;
                gettimeofday(&time2, NULL);
                //printf("release time:%ld ms", (time2.tv_sec - time1.tv_sec) * 1000 + (time2.tv_usec - time1.tv_usec) /1000);
#else


				release_count++;

#endif 

				sem_post(&sema);
				pthread_mutex_unlock(&isp_lock);

                static int i=0;
                if(i++%1000==0)
                    printf("ISP_AV_CMD_RELEASE_BUFFER from:%ld\n", msg_info.isp_av_msg_from);

                break;
            case ISP_AV_CMD_ISP_CONFIG:
                {
                    int id    = msg_info.isp_config.id;
                    int value = msg_info.isp_config.value;
                    SetISPConfigItemState(id, value);
                }
                break;

            default:
                printf("%s does not deal this command(%d) \n", __FUNCTION__, msg_info.isp_av_command);
                break;
        }
    }
#ifdef ROTATION
ERROR:
    for(i = 0; i < 2; i++) 
    {
        if(display_buffer[i].vir_addr != NULL) {
            alc_free(alc_inst, &display_buffer[i]);

            memset(&display_buffer[i], 0, sizeof(IM_Buffer));
        }
    }
    
    if (alc_inst != NULL)
        alc_close(alc_inst);
    alc_inst = NULL;

    if(ppInst != NULL) 
         PPRelease(ppInst);
    ppInst = NULL;
#endif

#ifdef SUPPORT_CHANGE_RESOULTION
	msg_recv_thread_loops_flag = 1;
#endif

}

int main(int argc, char *argv[])
{
    int ret;

    Inft_Isp_Init_FB();

    Inft_Isp_Init_Msg();

	AdasProcInit();

    pthread_create(&g_msg_rcv_thread, NULL, Inft_Isp_Rcv_Msg_Callback, NULL);

    felix_start(Inft_Isp_Snd_Msg_Callback/*, argc, argv*/);

    if (fbfd > 0) {
        close(fbfd);
        fbfd = -1;
    }

    return EXIT_SUCCESS;
}
