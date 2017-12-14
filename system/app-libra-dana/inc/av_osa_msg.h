#ifndef _AV_OSA_MSG_H
#define _AV_OSA_MSG_H

#include "av_video_common.h"
#include "media_buffer.h"
#include <sys/time.h>

typedef enum {
    E_MSG_PROD_INVALID = 0x0000,
    
    E_MSG_PROD_FRONTCAM = 0x1000,
    E_MSG_PROD_REARCAM = 0x2000,
    
    E_MSG_PROD_CAP = 0x0001,
    E_MSG_PROD_AVI_INPUT = 0x0002,
    E_MSG_PROD_JPG_INPUT = 0x0004,
    E_MSG_PROD_REAR_CAP = 0x0008,
    E_MSG_PROD_DEMUXER_INPUT = 0x0010
} T_MSG_PRODUCER;

typedef struct {
    int   size;
    void  *physAddr;
    void  *virtAddr;
    T_VIDEO_STREAM_ID videoStreamId;
    VIDEO_FRAME_TYPE   frameType;  // I/P/B frame
    struct timeval timestamp;
    long long videotime;
    // display related
    T_MSG_PRODUCER msgProd;
    // sensor output format
    unsigned int raw_imageWidth;
    unsigned int raw_imageHeight;
    unsigned int valid_width;
    unsigned int valid_height;
    RAW_PIC_TYPE raw_imageType;
//    #ifdef SURPPORT_PREVIEW
    unsigned int dst_imageWidth;
    unsigned int dst_imageHeight;
//    #endif
    // pipeline related
    unsigned int pipelineId;
    // send ack related
    unsigned int needAck;
    unsigned int srcTaskId;
    //Now, only be used by pp task
    //Indicate how many times funcProcData be called
    //After all procCnt equal to nextTask count
    unsigned int procCnt; 
    int jpeg_encoding;
    //buffer for sync
    media_buffer *buffer;
} AV_OSA_MsgBufInfo;


#ifdef __cplusplus 
extern "C" {
#endif

int av_osa_msg_init() ;

int av_osa_new_msg(unsigned int *msgId, AV_OSA_MsgBufInfo *bufInfo);

int av_osa_free_msg(unsigned int msgId);

AV_OSA_MsgBufInfo* av_osa_get_msgBufInfo(unsigned int msgId);

#define av_osa_set_msgBufInfo(vir, phy, s, need_ack, buf) \
{\
    (buf)->virtAddr = (vir);\
    (buf)->physAddr = (phy);\
    (buf)->size = (s);\
    (buf)->needAck = (need_ack);\
}

void av_osa_msg_print_msgInfo();

char* av_osa_frame_type_to_name(VIDEO_FRAME_TYPE tp);

#ifdef __cplusplus 
}
#endif


#endif
 
