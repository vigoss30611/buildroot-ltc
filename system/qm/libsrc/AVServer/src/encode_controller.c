/**
  ******************************************************************************
  * @file       encode_controller.c
  * @author     InfoTM IPC Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 InfoTM</center></h2>
  *
  * This software is confidential and proprietary and may be used only as expressly
  * authorized by a licensing agreement from InfoTM, and ALL RIGHTS RESERVED.
  * 
  * The entire notice above must be reproduced on all copies and should not be
  * removed.
  ******************************************************************************
  */
#include "encode_controller.h"

#include <sys/prctl.h>
#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <qsdk/videobox.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

#include "ipc.h"
#include "looper.h"
#include "list.h"
#include "video.h"
#include "motion_controller.h"

#define LOGTAG  "ENCODE_CTRL"

typedef struct {
    int bitrate;
    int qpmax;
    int qpmin;
    //int gopsec; // gop length in seconds
} cbr_table_t;

typedef struct {
    int qpmax;
    int qpmin;
    //int gopsec; // gop length in seconds.
} vbr_table_t;

enum {
    VBR_HIGHEST=0,
    VBR_HIGH,
    VBR_NORMAL,
    VBR_LOW,
    VBR_LOWEST,

    VBR_NUM
};

typedef struct {
    struct v_bitrate_info br;
} vcap_vbr_t;

typedef struct 
{
    int bitrate;
    int rc_mode;
    int p_frame;
    int quality;
} vcap_bitrate_t;

typedef struct {
    ven_info_t *ven_info;
    
    vcap_vbr_t vbr;
    vcap_bitrate_t bitrate;

    const vbr_table_t *vbr_table;
    const cbr_table_t *cbr_table;
    int vbr_table_num;
    int cbr_table_num;

#define VCAP_STATISTIC_FRCNT_BASE   150
    struct timespec tsA;
    int sum; 
    int lastIFrame;
    int iInter;
    int frcnt;
    int realFps;
    int realKbps;
    int frcnt_base;
} encode_controller_t;

typedef struct {
    pthread_mutex_t lock;
    int inited;
    int enable;
    int num;
    int motion; /*0:static 1:move */
    int mode;   /*0:day mode 1:night mode 2:dark_mode*/
    int control;/*0:none, 1:recovery 2: control */
    encode_controller_t *ecs; 
} encode_controller_manager_t;

static encode_controller_manager_t sEncodeControllerManager = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .inited = 0,
    .num = 0,
    .motion = 1,
    .mode   = 0,
    .control = ENCODE_CONTROLLER_NONE,
    .ecs = NULL
};

#define container_of(ptr, container, member)    \
    (container *) (((char*) (ptr)) - offsetof(container, member))

static const cbr_table_t kH264MainStreamCBRTable[] = {
    {3000000, 38, 20},
    {2500000, 40, 22},
    {2000000, 42, 24},
    {1500000, 44, 26},
    {1000000, 46, 28},
    {600000, 48, 30}
};

static const cbr_table_t kH264SubStreamCBRTable[] = {
    {1000000, 32, 16},
    {600000, 35, 18},
    {250000, 38, 22}
};

static const cbr_table_t kH265MainStreamCBRTable[] = {
    {1800000, 40, 20},
    {1200000, 44, 24},
    {600000, 48, 28}
};

static const cbr_table_t kH265SubStreamCBRTable[] = {
    {1000000, 40, 12},
    {600000, 40, 16},
    {400000, 44, 20}
};

static const vbr_table_t kH264MainStreamVBRTable[VBR_NUM] = {
    {38, 30},
    {41, 33},
    {45, 37},
    {49, 39},
    {51, 41}
};

static const vbr_table_t kH264SubStreamVBRTable[VBR_NUM] = {
    {38, 30},
    {41, 33},
    {45, 37},
    {49, 39},
    {51, 41}
};

static const vbr_table_t kH265MainStreamVBRTable[VBR_NUM] = {
    {40, 20},
    {40, 24},
    {40, 28},
    {44, 28},
    {48, 28}
};

static const vbr_table_t kH265SubStreamVBRTable[VBR_NUM] = {
    {40, 12},
    {40, 16},
    {40, 20},
    {44, 24},
    {48, 28}
};

static int encodeInit_l(int id)
{
    encode_controller_t *ec = &sEncodeControllerManager.ecs[id];
    struct v_bitrate_info brinfo = {0};
    char *chnl = NULL; 
    int media_type = VIDEO_MEDIA_H264;
    int fps = 0; 
    ipclog_debug("vcapInit(%d)\n", id);

    /*!< configure vcap channel */
    ven_info_t *ven_info = infotm_video_info_get(id);
    if (!ven_info) {
        return -1;
    }
    ec->ven_info = ven_info;
    chnl = ec->ven_info->bindfd;
    media_type = ec->ven_info->media_type;
    fps = ec->ven_info->fps;

    //TODO: frcnt_base should bigger than gop_length
    ec->frcnt_base = VCAP_STATISTIC_FRCNT_BASE; 

    if (media_type == VIDEO_MEDIA_H264) {
        if (id == 0) {   // Main stream
            ec->cbr_table = kH264MainStreamCBRTable;
            ec->cbr_table_num = sizeof(kH264MainStreamCBRTable) / sizeof(kH264MainStreamCBRTable[0]);

            ec->vbr_table = kH264MainStreamVBRTable;
            ec->vbr_table_num = sizeof(kH264MainStreamVBRTable) / sizeof(kH264MainStreamVBRTable[0]);
        } else {    // Sub stream
            ec->cbr_table = kH264SubStreamCBRTable;
            ec->cbr_table_num = sizeof(kH264SubStreamCBRTable) / sizeof(kH264SubStreamCBRTable[0]);

            ec->vbr_table = kH264SubStreamVBRTable;
            ec->vbr_table_num = sizeof(kH264SubStreamVBRTable) / sizeof(kH264SubStreamVBRTable[0]);
        }
    } else if (media_type == VIDEO_MEDIA_HEVC){
        if (id == 0) {   // Main stream
            ec->cbr_table = kH265MainStreamCBRTable;
            ec->cbr_table_num = sizeof(kH265MainStreamCBRTable) / sizeof(kH265MainStreamCBRTable[0]);

            ec->vbr_table = kH265MainStreamVBRTable;
            ec->vbr_table_num = sizeof(kH265MainStreamVBRTable) / sizeof(kH265MainStreamVBRTable[0]);
        } else {    // Sub stream
            ec->cbr_table = kH265SubStreamCBRTable;
            ec->cbr_table_num = sizeof(kH265SubStreamCBRTable) / sizeof(kH265SubStreamCBRTable[0]);

            ec->vbr_table = kH265SubStreamVBRTable;
            ec->vbr_table_num = sizeof(kH265SubStreamVBRTable) / sizeof(kH265SubStreamVBRTable[0]);
        }
    }

    ipclog_info("%s cbr_table:%p cbr_table:num:%d\n", __FUNCTION__, ec->cbr_table, ec->cbr_table_num);
    ipclog_info("%s vbr_table:%p vbr_table:num:%d\n", __FUNCTION__, ec->vbr_table, ec->vbr_table_num);

    const cbr_table_t *cbr_table = &ec->cbr_table[0];
    
    video_get_bitrate(chnl, &brinfo);
    brinfo.rc_mode = VENC_CBR_MODE;
    brinfo.bitrate = cbr_table->bitrate;
    brinfo.qp_max  = cbr_table->qpmax;
    brinfo.qp_min  = cbr_table->qpmin;
    brinfo.qp_delta     = -1;
    brinfo.qp_hdr       = -1;
    brinfo.picture_skip = 0;
    brinfo.p_frame      = fps * 3;
    if (brinfo.p_frame <= 2*fps) {
        brinfo.gop_length = brinfo.p_frame * 2;
    } else {
         brinfo.gop_length = brinfo.p_frame;
    }

    if (media_type == VIDEO_MEDIA_H264) {
        brinfo.mb_qp_adjustment = -1;
    } else if (media_type == VIDEO_MEDIA_HEVC) {
        brinfo.mbrc = 1;
    }     


    video_set_bitrate(chnl, &brinfo);
    ec->bitrate.rc_mode = brinfo.rc_mode;
    ec->bitrate.bitrate = brinfo.bitrate;
    ec->bitrate.p_frame = brinfo.p_frame;
    ec->bitrate.quality = VBR_HIGHEST;

    video_get_bitrate(chnl, &ec->vbr.br);
    ipclog_debug("fps=%d\n", fps);
    ipclog_debug("%s() ratectrl\n", __FUNCTION__);
    ipclog_debug("    rc_mode %d\n", ec->vbr.br.rc_mode);
    ipclog_debug("    idr_interval %d\n", ec->vbr.br.p_frame);
    ipclog_debug("    p_frame=%d\n", ec->vbr.br.p_frame);
    ipclog_debug("    picture_skip=%d\n", ec->vbr.br.picture_skip);
    ipclog_debug("    hrd %d\n", ec->vbr.br.hrd);
    ipclog_debug("    hrd_cpbsize %d\n", ec->vbr.br.hrd_cpbsize);
    ipclog_debug("    refresh_interval %d\n", ec->vbr.br.refresh_interval);
    ipclog_debug("    mbrc %d\n", ec->vbr.br.mbrc);
    ipclog_debug("    mb_qp_adjustment %d\n", ec->vbr.br.mb_qp_adjustment);
    ipclog_debug("    bitrates=%d\n", ec->vbr.br.bitrate);
    ipclog_debug("    br.qp_max=%d\n", ec->vbr.br.qp_max);
    ipclog_debug("    br.qp_min=%d\n", ec->vbr.br.qp_min);
    ipclog_debug("    br.qp_delta=%d\n", ec->vbr.br.qp_delta);
    ipclog_debug("    br.qp_hdr=%d\n", ec->vbr.br.qp_hdr);
    ipclog_debug("    br.gop_length=%d\n", ec->vbr.br.gop_length);
    return 0;
}

static int encodeSetBitrate_l(int id, vcap_bitrate_t *v_bitrate)
{
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    encode_controller_t *ec = &sEncodeControllerManager.ecs[id];
    int ret = -1;
    int i;
    const cbr_table_t *cbrTable;
    const vbr_table_t *vbrTable;
    struct v_rate_ctrl_info rcinfo = {0};
    struct v_basic_info stBasicInfo;

    ipclog_info("%s motion:%d mode:%d control:%d \n", __FUNCTION__, 
            ecm->motion, ecm->mode, ecm->control);
    ipclog_info("vcapSetBitrate(%d, (%d, %d, %d, %d))\n",
            id, v_bitrate->rc_mode, v_bitrate->bitrate, v_bitrate->quality, v_bitrate->p_frame);

    char *chnl = ec->ven_info->bindfd; 
    int media_type = ec->ven_info->media_type;
    int fps = ec->ven_info->fps; 

    video_get_basicinfo(chnl, &stBasicInfo);
    fps = video_get_fps(chnl);
    ret = video_get_ratectrl(chnl, &rcinfo);
    if (ret<0) {
        ipclog_info("%s  video_get_ratectrl failed!!nRet:%d\n",__FUNCTION__, ret);
        return ret;
    }
    rcinfo.idr_interval = v_bitrate->p_frame;
    if (rcinfo.idr_interval <= 2*fps) {
        rcinfo.gop_length = rcinfo.idr_interval * 2;
    } else {
         rcinfo.gop_length = rcinfo.idr_interval;
    }

    if ((v_bitrate->rc_mode) == 0) {  // CBR
        cbrTable = ec->cbr_table;
        rcinfo.rc_mode = VENC_CBR_MODE;
        rcinfo.cbr.bitrate = v_bitrate->bitrate;
        rcinfo.cbr.qp_delta = -1;
        rcinfo.cbr.qp_hdr = -1;
        for (i = 0; i < ec->cbr_table_num; i++) {
            if (v_bitrate->bitrate >= cbrTable[i].bitrate) {
                break;
            }
        }
        
        /*****************encode policy***************/
        if (i == ec->cbr_table_num) {
            i -= 1;
        }

        rcinfo.cbr.qp_max = cbrTable[i].qpmax;
        rcinfo.cbr.qp_min = cbrTable[i].qpmin;

        if (ecm->enable) {
            //dec bitrate
            //inc gop & idr_interval
            if (ecm->motion == MOTION_NONE) {
                int step = (ec->cbr_table_num - i)/2;
                ipclog_debug("cbr_table_num:%d i:%d step:%d \n", ec->cbr_table_num, i, step);
                i += step;
                //rcinfo.gop_length = fps*4;
                //rcinfo.idr_interval = v_bitrate->p_frame;
                //Over write bitrate when static mode
                if (rcinfo.cbr.bitrate > cbrTable[i].bitrate) {
                    rcinfo.cbr.bitrate = cbrTable[i].bitrate;
                }
            }

            rcinfo.cbr.qp_max = cbrTable[i].qpmax;
            rcinfo.cbr.qp_min = cbrTable[i].qpmin;

            //dec qp_max
            if (ecm->mode == MODE_NIGHT) {
                rcinfo.cbr.qp_delta = -1;
                rcinfo.cbr.qp_hdr = -1;
                rcinfo.cbr.qp_max = rcinfo.cbr.qp_max - (rcinfo.cbr.qp_max - rcinfo.cbr.qp_min)/4;   
            } 

            if (ecm->mode == MODE_LOWLUX) {
                rcinfo.cbr.qp_delta = 3;
                rcinfo.cbr.qp_hdr = -1;
            }

            if (ecm->mode == MODE_DARK) {
                //rcinfo.gop_length = fps * 4;
                //rcinfo.idr_interval = v_bitrate->p_frame;
                rcinfo.cbr.bitrate = v_bitrate->bitrate;
                rcinfo.cbr.qp_delta = -1;
                rcinfo.cbr.qp_hdr = -1;
                rcinfo.cbr.qp_max = 50;
                rcinfo.cbr.qp_min = 20;
            }

            if (ecm->control == ENCODE_CONTROLLER_CONTROL) {
                //rcinfo.gop_length = fps * 4;
                //rcinfo.idr_interval = v_bitrate->p_frame;
                rcinfo.cbr.bitrate = v_bitrate->bitrate;
                rcinfo.cbr.qp_delta = -1;
                rcinfo.cbr.qp_hdr = -1;
                rcinfo.cbr.qp_max = 50;
                rcinfo.cbr.qp_min = 20;
            } else if (ecm->control == ENCODE_CONTROLLER_RECOVERY) {
                //rcinfo.gop_length = fps * 4;
                //rcinfo.idr_interval = v_bitrate->p_frame;
                rcinfo.cbr.bitrate = v_bitrate->bitrate;
                rcinfo.cbr.qp_delta = -1;
                rcinfo.cbr.qp_hdr = -1;
                rcinfo.cbr.qp_max = 50;
                rcinfo.cbr.qp_min = 20;

                if (cbrTable[i].qpmax < 50) {
                    rcinfo.cbr.qp_max = 50 - (50 - cbrTable[i].qpmax)/2;
                }

                if (cbrTable[i].qpmin > 20) {
                    rcinfo.cbr.qp_min = 20 + (cbrTable[i].qpmin - 20)/2;
                }
            }
        }

        /*****************encode policy***************/

        ipclog_info("%s() ratectrl\n", __FUNCTION__);
        ipclog_info("    rc_mode %d\n", rcinfo.rc_mode);
        ipclog_info("    gop_length %d\n", rcinfo.gop_length);
        ipclog_info("    picture_skip %d\n", rcinfo.picture_skip);
        ipclog_info("    idr_interval %d\n", rcinfo.idr_interval);
        ipclog_info("    hrd %d\n", rcinfo.hrd);
        ipclog_info("    hrd_cpbsize %d\n", rcinfo.hrd_cpbsize);
        ipclog_info("    refresh_interval %d\n", rcinfo.refresh_interval);
        ipclog_info("    mbrc %d\n", rcinfo.mbrc);
        ipclog_info("    mb_qp_adjustment %d\n", rcinfo.mb_qp_adjustment);
        ipclog_info("    bitrate %d\n", rcinfo.cbr.bitrate);
        ipclog_info("    qp_max %d\n", rcinfo.cbr.qp_max);
        ipclog_info("    qp_min %d\n", rcinfo.cbr.qp_min);
        ipclog_info("    qp_delta %d\n", rcinfo.cbr.qp_delta);
        ipclog_info("    qp_hdr %d\n", rcinfo.cbr.qp_hdr);

    }
    else {  // VBR
        vbrTable = ec->vbr_table;
        if (v_bitrate->quality >= ec->vbr_table_num) {
            v_bitrate->quality = ec->vbr_table_num - 1;
        }

        rcinfo.rc_mode = VENC_VBR_MODE;
        rcinfo.vbr.qp_max = vbrTable[v_bitrate->quality].qpmax;
        rcinfo.vbr.qp_min = vbrTable[v_bitrate->quality].qpmin;
        rcinfo.vbr.qp_delta  = -1;
        rcinfo.vbr.max_bitrate = v_bitrate->bitrate;
        rcinfo.vbr.threshold = 20;
        ipclog_info("%s() ratectrl\n", __FUNCTION__);
        ipclog_info("    rc_mode %d\n", rcinfo.rc_mode);
        ipclog_info("    gop_length %d\n", rcinfo.gop_length);
        ipclog_info("    picture_skip %d\n", rcinfo.picture_skip);
        ipclog_info("    idr_interval %d\n", rcinfo.idr_interval);
        ipclog_info("    hrd %d\n", rcinfo.hrd);
        ipclog_info("    hrd_cpbsize %d\n", rcinfo.hrd_cpbsize);
        ipclog_info("    refresh_interval %d\n", rcinfo.refresh_interval);
        ipclog_info("    mbrc %d\n", rcinfo.mbrc);
        ipclog_info("    mb_qp_adjustment %d\n", rcinfo.mb_qp_adjustment);
        ipclog_info("    vbr.qp_max %d\n", rcinfo.vbr.qp_max);
        ipclog_info("    vbr.qp_min %d\n", rcinfo.vbr.qp_min);
        ipclog_info("    vbr.qp_delta %d\n", rcinfo.vbr.qp_delta);
        ipclog_info("    vbr.max_bitrate %d\n", rcinfo.vbr.max_bitrate);
        ipclog_info("    vbr.threshold %d\n", rcinfo.vbr.threshold);
    }

    ret = video_set_ratectrl(chnl, &rcinfo);
    if (ret < 0) {
        ipclog_info("%s video_set_ratectrl failed!!nRet:%d\n",__FUNCTION__, ret);
        return ret;
    }

    memcpy(&(ec->bitrate), v_bitrate, sizeof(vcap_bitrate_t));

    /*!< FIXME: "trigger key frame" should called in the caller of this function */
    //video_trigger_key_frame(chnl);

    return 0;
}

static int encodeUpdateAllBitrate_l(void)
{
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    int ret = 0;
    int i = 0;
    for (i = 0; i < ecm->num; i++) {
        encode_controller_t *ec = &ecm->ecs[i];
        vcap_bitrate_t v_bitrate;
        v_bitrate.bitrate = ec->bitrate.bitrate;
        v_bitrate.rc_mode = ec->bitrate.rc_mode;
        v_bitrate.p_frame = ec->bitrate.p_frame;
        v_bitrate.quality = ec->bitrate.quality;
        ret = encodeSetBitrate_l(i, &v_bitrate);
        //TODO: checkout ret
    }

    return ret;
}

void encodeMotionDetectListener(int what, motion_detect_info_t *info)
{
    encode_controller_set_motion(what);
}

void encodeMotionDetectHandler(char *event, void *arg, int size)
{
    if (!event) {
        ipclog_error("%s Invalid event\n", __FUNCTION__);
        return;
    }

    if (strcmp(event, EVENT_MOTION_DETECT) != 0) {
        ipclog_error("%s Invalid event:%s\n", __FUNCTION__, event);
        return;
    }

    motion_detect_info_t *info = arg;
    if (size != sizeof(motion_detect_info_t)) {
        ipclog_error("%s Invalid arg:%p size:%d \n", __FUNCTION__, arg, size);
        return;
    }

    encode_controller_set_motion(info->motion);
}

int encode_controller_init(void)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_info("%s \n", __FUNCTION__); 
    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        ret = 0;
        goto exit;
    }    

    int num = infotm_video_info_num_get();
    if (!num) {
        ipclog_error("%s Invalid num:0\n", __FUNCTION__);
        ret = -1;
        goto exit;
    }
    ecm->num = num;

    ecm->ecs = (encode_controller_t *)malloc(sizeof(encode_controller_t)*num);
    if (!ecm->ecs) {
        ipclog_error("%s OOM ecs\n", __FUNCTION__);
        ret = -1; 
        goto exit;
    }
    memset(ecm->ecs, 0 , sizeof(encode_controller_t)*num);

#ifdef CONFIG_ENCODE_CONTROL_ENABLE
    ecm->enable = 1;
#pragma  message("encode control enable!")  
#else
    ecm->enable = 0;
 #pragma  message("encode control disable!")  
#endif

    int i = 0;
    for (i = 0; i < num; i++) {
        encodeInit_l(i);
    }

    ecm->inited = 1;

    ret = QM_Event_RegisterHandler(EVENT_MOTION_DETECT, 0, encodeMotionDetectHandler);
    if (ret < 0) {
        ipclog_error("%s register handler failed\n", __FUNCTION__);
    }

exit:
    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret; 
}

int encode_controller_deinit(void)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_info("%s \n", __FUNCTION__); 

    QM_Event_UnregisterHandler(EVENT_MOTION_DETECT , encodeMotionDetectHandler);
    
    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s already\n", __FUNCTION__);
        ret = 0;
        goto exit;
    }

    if (ecm->ecs) {
        free(ecm->ecs);
        ecm->ecs = NULL;
    }

    ecm->inited = 0;

exit:
    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;    
}

int encode_controller_set_encode_param(int id, int bitrate, int rc_mode, int p_frame, int quality)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s id:%d\n", __FUNCTION__, id);
    pthread_mutex_lock(&ecm->lock);

    vcap_bitrate_t v_bitrate;
    v_bitrate.bitrate = bitrate;
    v_bitrate.rc_mode = rc_mode;
    v_bitrate.p_frame = p_frame;
    v_bitrate.quality = quality;

    ret = encodeSetBitrate_l(id, &v_bitrate);

    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;
}

int encode_controller_set_bitrate(int id, int bitrate)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s id:%d\n", __FUNCTION__, id);
    pthread_mutex_lock(&ecm->lock);
    
    encode_controller_t *ec = &ecm->ecs[id];

    vcap_bitrate_t v_bitrate;
    v_bitrate.bitrate = bitrate;
    v_bitrate.rc_mode = ec->bitrate.rc_mode;
    v_bitrate.p_frame = ec->bitrate.p_frame;
    v_bitrate.quality = ec->bitrate.quality;

    ret = encodeSetBitrate_l(id, &v_bitrate);

    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;
}

int encode_controller_refresh_bitrate(int id)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s id:%d\n", __FUNCTION__, id);
    pthread_mutex_lock(&ecm->lock);

    encode_controller_t *ec = &ecm->ecs[id];
    vcap_bitrate_t v_bitrate;
    v_bitrate.bitrate = ec->bitrate.bitrate;
    v_bitrate.rc_mode = ec->bitrate.rc_mode;
    v_bitrate.p_frame = ec->bitrate.p_frame;
    v_bitrate.quality = ec->bitrate.quality;

    ret = encodeSetBitrate_l(id, &v_bitrate);

    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;
}


int encode_controller_set_motion(int motion)
{	
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s motion:%d \n", __FUNCTION__, motion);

    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&ecm->lock);
        return -1;
    }

    if (!ecm->enable) {
        pthread_mutex_unlock(&ecm->lock);
        return 0;
    }

    ecm->motion = motion;
    
    ret = encodeUpdateAllBitrate_l();
    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return 0;
}

static int encodeSetMode_l(int id, int mode)
{
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    encode_controller_t *ec = &sEncodeControllerManager.ecs[id];
    int ret = -1;
    ipclog_info("%s mode:%d\n", __FUNCTION__, mode);
    char *chnl = ec->ven_info->bindfd; 
    if (mode == MODE_DAY) {
        ret = video_set_scenario(chnl, VIDEO_DAY_MODE);
    } else if(mode == MODE_NIGHT) {
        ret = video_set_scenario(chnl, VIDEO_NIGHT_MODE);
    }

    return 0;
}

static int encodeUpdataAllMode_l()
{
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    int ret = 0;
    int i = 0;
    for (i = 0; i < ecm->num; i++) {
        ret = encodeSetMode_l(i, ecm->mode);
    }
    
    return 0;
}

int encode_controller_set_night(int mode)
{
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s mode:%d \n", __FUNCTION__, mode);

    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&ecm->lock);
        return -1;
    }
    ecm->mode = mode;
    
    if (ecm->enable) {
        ret = encodeUpdateAllBitrate_l();
    }

    //update encoder mode, trigger blackwhite color mode, base on mode
    ret = encodeUpdataAllMode_l();
    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;    
}

int encode_controller_check_control(void)
{
    int control = ENCODE_CONTROLLER_NONE;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;

    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&ecm->lock);
        return control;
    }
    
    control = ecm->control;
    pthread_mutex_unlock(&ecm->lock);

    return control;    
}

int encode_controller_set_control(int control)
{
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    ipclog_debug("%s control:%d \n", __FUNCTION__, control);

    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&ecm->lock);
        return -1;
    }

    if (!ecm->enable) {
        pthread_mutex_unlock(&ecm->lock);
        return 0;
    }

    ecm->control = control;
    
    ret = encodeUpdateAllBitrate_l();

    pthread_mutex_unlock(&ecm->lock);
    ipclog_info("%s ret:%d \n", __FUNCTION__, ret); 

    return ret;    
}

int encode_controller_count_bitrate(int id, int frcnt, int priv, int size)
{
    struct timespec tsB;
    int tsdiff = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    pthread_mutex_lock(&ecm->lock);

    encode_controller_t *ec = &ecm->ecs[id];
    ec->frcnt = frcnt;
    if (ec->frcnt == 0) {
        clock_gettime(CLOCK_MONOTONIC, &ec->tsA);
        ec->sum = 0;
    }

    if (priv == VIDEO_FRAME_I) {
        ec->iInter = ec->frcnt - ec->lastIFrame;
        ec->lastIFrame = ec->frcnt;
    }

    ec->sum += size;
   if ((ec->frcnt != 0) && !(ec->frcnt % ec->frcnt_base)){
        clock_gettime(CLOCK_MONOTONIC, &tsB);
        tsdiff = (tsB.tv_sec * 1000 + tsB.tv_nsec / 1E6) - (ec->tsA.tv_sec * 1000 + ec->tsA.tv_nsec / 1E6);
        //ipclog_debug("vcap[%d]: frcnt %d buf 0x%x, size %d\n", id, ec->frcnt, size);
        //ipclog_debug("sum %d, tsdiff %d\n", ec->sum, tsdiff);
        ec->realFps = (ec->frcnt_base * 1000) / tsdiff;
        ec->realKbps = (ec->sum * 8) / tsdiff;
        ipclog_info("vcap[%d]: frcnt %d, fps %d, kbps %d iInter %d\n",
                    id, ec->frcnt, ec->realFps, ec->realKbps, ec->iInter);
        memcpy((void *)&ec->tsA, (void *)&tsB, sizeof(struct timespec));
        ec->sum = 0;
    }

     pthread_mutex_unlock(&ecm->lock);
}

int encode_controller_check_bitrate(int id)
{
    int ret = 0;
    encode_controller_manager_t *ecm = &sEncodeControllerManager;
    pthread_mutex_lock(&ecm->lock);
    if (ecm->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&ecm->lock);
        return 0;
    }

    if (!ecm->enable) {
        pthread_mutex_unlock(&ecm->lock);
        return 0;
    }

    if (id >= ecm->num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, ecm->num);
        pthread_mutex_unlock(&ecm->lock);
        return 0;
    }

    encode_controller_t *ec = &ecm->ecs[id];
    
    int realKbps = ec->realKbps * 1000;
    if (realKbps >= (ec->bitrate.bitrate + (ec->bitrate.bitrate/3))) {
        ret = ENCODE_BITRATE_HIGH;
    } else if (realKbps <= (ec->bitrate.bitrate - (ec->bitrate.bitrate/3))) {
        ret = ENCODE_BITRATE_LOW;
    } else {
        ret = ENCODE_BITRATE_NORMAL;
    }

    //ipclog_debug("%s realKbps:%d bitrate:%d ret:%d \n", __FUNCTION__, realKbps, ec->bitrate.bitrate, ret);
    
    pthread_mutex_unlock(&ecm->lock);

    return ret;
}
