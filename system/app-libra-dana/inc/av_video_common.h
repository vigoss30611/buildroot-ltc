#ifndef _AV_VIDEO_COMMON_H
#define _AV_VIDEO_COMMON_H

#include "av_video_burst.h"
#include <sys/time.h>

#define CAP_SENSOR_PICTYPE  (RAW_PIC_YUV420SEM_NV12)

#define MAX_PIPELINE_NUM (8)
#define MAX_SOURCE_TSK_NUM (8)

#define C_AV_MAX_FILENAME_LEN   (128)
#define C_AV_MAX_BUFFER_LEN   (1024 * 1024 * 4)

#define C_MAX_IMG_WIDTH    1920
#define C_MAX_IMG_HEIGHT    1088

typedef enum {
    E_EN_STATUS_INVALID = -1,
    E_EN_STATUS_DISABLED = 0,
    E_EN_STATUS_ENABLED = 1,
} T_ENABLED_STATUS;

typedef enum {
    RAW_PIC_INVALID,
    RAW_PIC_YUV420SEM_NV12,
    RAW_PIC_YUV420SEM_NV21,
    RAW_PIC_RGB565,
    RAW_PIC_RGB32,
} RAW_PIC_TYPE;

typedef enum
{
    V_FRAME_TYPE_AUDIO_PLAYBACK,
    V_FRAME_TYPE_JPEG,
    V_FRAME_TYPE_SPS,
    V_FRAME_TYPE_I,
    V_FRAME_TYPE_P,
    V_FRAME_TYPE_B,
} VIDEO_FRAME_TYPE;

typedef enum {
    E_VIDEO_RESOLUTION_1080P = 0,
    E_VIDEO_RESOLUTION_720P,
    E_VIDEO_RESOLUTION_VGA,
    E_VIDEO_RESOLUTION_CNT,
} T_VIDEO_RESOLUTION;

typedef enum {
    E_STREAM_ID_INVALID = -1,
    E_STREAM_ID_STORAGE = 0,
    E_STREAM_ID_PREVIEW = 1,
    E_STREAM_ID_PLAYBACK = 2,
    E_STREAM_ID_MJPEG = 3,
    E_STREAM_ID_CNT
} T_VIDEO_STREAM_ID;

typedef enum {
    E_PIP_TYPE_INVALID = -1,
    E_PIP_TYPE_START = 0,
    E_PIP_TYPE_MAINSCREEN = E_PIP_TYPE_START,
    E_PIP_TYPE_SUBSCREEN,
    E_PIP_TYPE_CNT,
} T_DISPLAY_PIP_TYPE;

typedef enum {
    DV_MOD_INVALID = -1,
    DV_MOD_MIN = 0,
    // sport dv mode
    DV_MOD_VIDEO = DV_MOD_MIN,
    DV_MOD_PHOTO,
    DV_MOD_BURST,
    DV_MOD_CAR,
    DV_MOD_PLAY,
    // car recoder mode
    CAR_MOD_FRONTCAM,
    CAR_MOD_REARCAM,  // 5
    DV_MOD_MAX
} T_DV_MOD;


typedef struct {
    unsigned int enabled;
    unsigned int width, height;
    T_VIDEO_RESOLUTION resolutionType;
    RAW_PIC_TYPE rawPicType;
    unsigned int  frameRate;
    unsigned int gopLen;
    unsigned int bitRate;
    unsigned int fps;
//    #ifdef SURPPORT_PREVIEW
    struct timeval timestamp;
//    #endif
} AV_VIDEO_StreamInfo;

typedef struct {
    T_DV_MOD  dv_mode;
    T_BURST_TYPE burst_type;
    int isGuiRunning;
} ISHM_VIDEO_PARAMETER;

typedef struct {
    int posX, posY;
    unsigned int width, height;
    RAW_PIC_TYPE rawPicType;
    int cameraSource;
} CFG_VIDEO_DisplayStream[E_PIP_TYPE_CNT];


typedef void (*T_SrcTskEnable)(unsigned int enable);
typedef struct {
    unsigned int srcTsk;
    T_SrcTskEnable fncEnableSrcTsk;
    unsigned int enabledFlag;
} T_PipelineSrcTsk;

enum {
    PIPELINE_TYPE_UNKNOWN,
    PIPELINE_TYPE_RECORD,
    PIPELINE_TYPE_PLAYBACK,
    PIPELINE_TYPE_PICPREVIEW,
};

typedef struct {
    unsigned int pipelineId;
    int created;   /* pipeline is created or not */    
    int enabled;   /* is enabled or not */
    int  active;   /* for PIPELINE_RECORD, when pipeline switch, 
                    * this pipeline is still enabled, but only 
                    * source task is working, other task will be 
                    * unactive ,memory resources will be destroyed
                    * */    
    int pipeline_type;

    T_PipelineSrcTsk *srcTskInfo;
    T_VIDEO_STREAM_ID activeStrms[E_STREAM_ID_CNT];
    unsigned int activeStrmCnt;
} IPC_VIDEO_PipelineInfo;

typedef struct {
    IPC_VIDEO_PipelineInfo pipeline[MAX_PIPELINE_NUM];
    unsigned int pipelineCnt;
} IPC_VIDEO_ActivePipeline;


#define AV_CAL_RESOLUTION(w, h, size) \
    (size) = ((w) * (h))

#define AV_CAL_YUV_RESOLUTION(w, h, size) \
    (size) = (((w) + 15)&(~0x0f)) * (((h) + 15)&(~0x0f))


#define AV_CAL_PIC_SIZE(w, h, type, size)\
do{\
    switch(type) { \
    case RAW_PIC_YUV420SEM_NV12: \
    case RAW_PIC_YUV420SEM_NV21: \
        AV_CAL_YUV_SIZE(w, h, size); break;\
    case RAW_PIC_RGB565:\
        AV_CAL_RGB565_SIZE(w, h, size); break;\
    case RAW_PIC_RGB32:\
        AV_CAL_RGB32_SIZE(w, h, size); break;\
    default: size = 0; break;\
    }\
}while(0)

#define AV_CAL_YUV_SIZE(w, h, size) \
do{\
    int pic;\
    AV_CAL_YUV_RESOLUTION(w, h, pic);\
    size = pic + (pic >> 1);\
}while(0)

#define AV_CAL_RGB565_SIZE(w, h, size) \
do{\
    int pic;\
    AV_CAL_RESOLUTION(w, h, pic);\
    size = pic << 1;\
}while(0)

#define AV_CAL_RGB32_SIZE(w, h, size) \
do{\
    int pic;\
    AV_CAL_RESOLUTION(w, h, pic);\
    size = pic << 2;\
}while(0)

// please use streamIndex for each loop
#define FOR_ALL_STREAMS \
    unsigned int streamIndex;\
    for (streamIndex = 0;\
          (streamIndex < E_STREAM_ID_CNT);\
          streamIndex++)



extern const AV_VIDEO_StreamInfo g_default_streamInfo[E_STREAM_ID_CNT];
extern const CFG_VIDEO_DisplayStream g_default_displayStrm;


#ifdef __cplusplus 
extern "C" {
#endif

void video_pp_init(int tskId, int bufferId);
void video_encode_init(int tskId, int bufferId);
void encode_h265_init(int tskId, int bufferId);
#ifdef SURPPORT_PREVIEW
void encode_hevc_init(int tskId, int bufferId);
#endif
void video_decode_init(int tskId, int bufferId);
void jpeg_decode_init(int tskId, int bufferId);
void video_stream_init(int tskId, int bufferId);
void h264_input_init(int tskId, int bufferId);

void video_stream_procVideoHead(unsigned int streamIndex, unsigned char *headData, int size);

#ifdef __cplusplus 
}
#endif

#endif

