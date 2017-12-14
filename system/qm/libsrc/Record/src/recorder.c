#include "recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/input.h>
#include <stdarg.h>

#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

#include <qsdk/vplay.h>
#include <qsdk/cep.h>

#include "ipc.h"
#include "looper.h"
#include "video.h"

#define LOGTAG  "QM_RECORD"


/* *
* note:
* {
*  {
*      name: 20160302_144815_A_000601.mkv,
*      serialno: 0,
*      start: 1464857467,
*      duration: 601,
*      property: A
*  },
*  {
*      name: 20160302_145208_N_000620.mkv,
*      serialno: 1,
*      start: 1464857467,
*      duration: 620,
*      property: N
*  },
*  ...
* }
*/
enum {
    REC_FNAME_PROP_NO = 'N',
    REC_FNAME_PROP_MOTDECT = 'M'
};

#define REC_FNMAE_SEG_DATE_LEN  8   /* !< strlen(20160302) */
#define REC_FNAME_SEG_TIME_LEN  6   /* !< strlen(144815) */
#define REC_FNAME_SEG_PROP_LEN  1   /* !< single char */

typedef struct {
    pthread_mutex_t mutex;
    int inited;
    bool reqEna;
    bool realEna;
    VRecorder *handle;
    VRecorderInfo recInfo;
    bool preCaching;
    bool recnew;
    bool motdect;

    RECORD_ATTR_T attr;
    infotm_record_callback callback;
} recorder_t;

static recorder_t sRecorder = {
    .inited = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .handle = NULL,
    .callback = NULL,
};

static int recStop_l();
static int recReset_l(void);
static void record_event_handler(char *event, void *arg);

static int initRecStorageIfNecessary(void)
{
    ipclog_debug("initRecStorageIfNecessary()\n");

    if (access(CFG_RECORD_STORE_PATH, F_OK) != 0) {
        ipclog_debug("make rec store path %s\n", CFG_RECORD_STORE_PATH);
        if (mkdir(CFG_RECORD_STORE_PATH, 0644) != 0) {
            ipclog_error("cannot create rec store diretory %s\n", CFG_RECORD_STORE_PATH);
            return -1;
        }
    }

    return 0;
}

static int recInit_l(RECORD_ATTR_T *attr)
{
    recorder_t *rc = &sRecorder;
    struct v_bitrate_info brInfo;
    char value[16];
    static int registered = 0;

    ipclog_debug("recInit()\n");

    /*!< get luminancerent record mode */
    memcpy(&rc->attr, attr, sizeof(RECORD_ATTR_T));

    ipclog_info("mode:%d \n", rc->attr.mode);
    ipclog_info("duration:%d\n", rc->attr.duration); 
    ipclog_info("videoEnable:%d \n", rc->attr.videoEnable);
    ipclog_info("videoStream:%d \n", rc->attr.videoStream);
    ipclog_info("audioEnable:%d \n", rc->attr.audioEnable);
    ipclog_info("audioStream:%d \n", rc->attr.audioStream);
    ipclog_info("avForamt:%d \n",    rc->attr.avFormat);
    ipclog_info("audioFormat:%d \n", rc->attr.audioFormat);

    /*!< set audio encoder parameters */
    if (rc->attr.audioEnable) {
        strcpy(rc->recInfo.audio_channel, "default_mic");
    } else {
        strcpy(rc->recInfo.audio_channel, "");
    }

    switch (attr->audioFormat) 
    {
        case MEDIA_REC_AUDIO_FORMAT_AAC:
            rc->recInfo.audio_format.type = AUDIO_CODEC_TYPE_AAC;
            rc->recInfo.audio_format.sample_rate = 8000;
            rc->recInfo.audio_format.channels = 1;
            rc->recInfo.audio_format.sample_fmt = AUDIO_SAMPLE_FMT_S16;
            rc->recInfo.audio_format.effect = 0;
            
            rc->recInfo.stAudioChannelInfo.s32Channels = 2;
            rc->recInfo.stAudioChannelInfo.s32BitWidth = 32;
            rc->recInfo.stAudioChannelInfo.s32SamplingRate = 8000;
            rc->recInfo.stAudioChannelInfo.s32SampleSize = 1024;
            
            break;

        case MEDIA_REC_AUDIO_FORMAT_G711A:
            rc->recInfo.audio_format.type = AUDIO_CODEC_TYPE_G711A;
            rc->recInfo.audio_format.channels = 1;
            rc->recInfo.audio_format.sample_fmt = AUDIO_SAMPLE_FMT_S16;
            rc->recInfo.audio_format.sample_rate = 8000;

            rc->recInfo.stAudioChannelInfo.s32Channels = 2;
            rc->recInfo.stAudioChannelInfo.s32BitWidth = 32;
            rc->recInfo.stAudioChannelInfo.s32SamplingRate = 8000;
            rc->recInfo.stAudioChannelInfo.s32SampleSize = 1024;
            
            break;

        default:
            ipclog_error("%s unsupport audioFormat:%d \n", __FUNCTION__, rc->attr.audioFormat);
           break;
   } 

    /*!< set video parameters */
   int num = infotm_video_info_num_get();
   if (num == 0) {
       rc->attr.videoEnable = 0;
   }
   if (rc->attr.videoEnable) {
       /*!< set video channel and disable it in first because it's default enabled */
       ven_info_t * venInfo = infotm_video_info_get(rc->attr.videoStream);
       if (rc->attr.videoStream >= num) {
            rc->attr.videoStream = 0;
        }
        strcpy(rc->recInfo.video_channel, venInfo->bindfd);
        //video_disable_channel(rc->recInfo.video_channel);
    } else {
        strcpy(rc->recInfo.video_channel, "");
    }

    /*!< set recfile and contianer parameters */
    rc->recInfo.enable_gps = 0;
    if (rc->attr.duration == 0) {
        rc->attr.duration = CFG_RECORD_SLICE_DURATION;
    }
    rc->recInfo.time_segment = rc->attr.duration;
    strcpy(rc->recInfo.dir_prefix, CFG_RECORD_STORE_PATH);
    switch (rc->attr.avFormat) 
    {
        case MEDIA_REC_AV_FORMAT_MKV:
            strcpy(rc->recInfo.av_format, "mkv");
            break;

        case MEDIA_REC_AV_FORMAT_MP4:
            strcpy(rc->recInfo.av_format, "mp4");
            break;

        default:
            ipclog_error("%s unsupport avFormat:%d \n", __FUNCTION__, rc->attr.avFormat);
            break;
    }
    strcpy(rc->recInfo.time_format, "%Y%m%d_%H%M%S");

    if (rc->attr.mode == MEDIA_RECMODE_FULLTIME) {
        rc->recInfo.time_backward = 0;
        sprintf(rc->recInfo.suffix, "%%ts_%c", REC_FNAME_PROP_NO);
        strcat(rc->recInfo.suffix, "_%l");
        rc->preCaching = false;
    } else if (rc->attr.mode == MEDIA_RECMODE_MOTDECT) {
        rc->recInfo.time_backward = 10;
        sprintf(rc->recInfo.suffix, "%%ts_%c", REC_FNAME_PROP_MOTDECT);
        strcat(rc->recInfo.suffix, "_%l");
#if 0
        /*!< enable channel and new recorder to do pre-cache */
        if (video_enable_channel(rc->recInfo.video_channel) < 0) {
            ipclog_ooo("video_enable_channel(%s) failed\n", rc->recInfo.video_channel);
            ret = -1;
            goto error;
        }
#endif
        ipclog_debug("%s vplay_new_recorder\n", __FUNCTION__);
        rc->handle = vplay_new_recorder(&rc->recInfo);
        if (!rc->handle) {
            ipclog_error("vplay_new_recorder() failed, ret=%d\n", rc->handle);
            goto error;
        }
        rc->preCaching = true;
        rc->realEna = false;
    }

    /*!< register event of record handler, it must be registered in init() for it can only be called once */
    if (registered == 0) {
        if (event_register_handler(EVENT_VPLAY, 0, record_event_handler) != 0) {
            ipclog_error("event_register_handler(%s) failed\n", EVENT_VPLAY);
            goto error;
        }
        registered = 1;
    }

    rc->callback = NULL;
    ipclog_debug("recInit() done\n");
    return 0;

error:
    if (rc->handle) {
        vplay_delete_recorder(rc->handle);
        rc->handle = NULL;
    }

    return -1;
}

static int recDeinit_l(void)
{
    recorder_t *rc = &sRecorder;

    ipclog_debug("recDeinit()\n");

    recStop_l(true);
    
    if (rc->preCaching) {
        if (rc->handle) {
            ipclog_debug("%s vplay_delete_recorder\n", __FUNCTION__);
            vplay_delete_recorder(rc->handle);
            rc->handle = NULL;
        }
        //video_disable_channel(rc->recInfo.video_channel);
        rc->preCaching = false;
    }

    //event_unregister_handler(EVENT_VPLAY, record_event_handler);
    
    ipclog_debug("recDeinit() done\n");
    return 0;
}

static int recStart_l()
{
    int ret = 0;
    recorder_t *rc = &sRecorder;
    ipclog_debug("recStart\n");

    if (rc->realEna) {
        ipclog_warn("%s already\n", __FUNCTION__);
        goto Quit;
    }

    /*!< prepare record store */
    if (initRecStorageIfNecessary() != 0) {
        ipclog_error("initRecStorageIfNecessary() failed\n");
        goto Quit;
    }

#if 0
    /*!< check and shrink storage space */
    while (recCheckStoreAvalible() != 0) {
        if (recShrinkStore() != 0) {
            ipclog_alert("no rec space to record\n");
            ret = -1;
            goto Quit;
        }
    }
#endif

    /*!< enable channel */
    if (!rc->preCaching) {
        #if 0
        if (video_enable_channel(rc->recInfo.video_channel) < 0) {
            ipclog_ooo("video_enable_channel(%s) failed\n", rc->recInfo.video_channel);
            ret = -1;
            goto Quit;
        }
        #endif

        ipclog_debug("%s vplay_new_recorder\n", __FUNCTION__);
        rc->handle = vplay_new_recorder(&rc->recInfo);
        if (!rc->handle) {
            ipclog_ooo("vplay_new_recorder() failed, ret=%d\n", rc->handle);
            ret = -1;
            goto Quit;
        }
    }

    /*!< start record */
    if (rc->handle) {
        ipclog_debug("%s vplay_new_recorder\n", __FUNCTION__);
        vplay_start_recorder(rc->handle);
    }
    rc->realEna = true;

    ipclog_debug("%s done\n", __FUNCTION__);
Quit:
    return ret;
}

static int recStop_l(void)
{
    recorder_t *rc = &sRecorder;
    ipclog_debug("recStop\n");
    
    if (rc->realEna) {
        if (rc->handle) {
            ipclog_debug("%s vplay_stop_recorder\n", __FUNCTION__);
            vplay_stop_recorder(rc->handle);
        }
        
        if (!rc->preCaching) {
            if (rc->handle) {
                ipclog_debug("%s vplay_delete_recorder\n", __FUNCTION__);
                vplay_delete_recorder(rc->handle);
                rc->handle = NULL;
            }
            //video_disable_channel(rc->recInfo.video_channel);
        }
        rc->realEna = false;
    }

    ipclog_debug("%s done\n", __FUNCTION__);

    return 0;
}

static int recSection_l()
{
    int ret = 0;
    recorder_t *rc = &sRecorder;
    ipclog_debug("recSection\n");
    if (!rc->realEna) {
        ipclog_warn("%s not realEna \n", __FUNCTION__);
        goto Quit;
    }

    /*!< start record */
    if (rc->handle) {
        ret = vplay_control_recorder(rc->handle, VPLAY_RECORDER_NEW_SEGMENT, NULL);
    }

    ipclog_debug("%s done\n", __FUNCTION__);

Quit:
    return ret;
}

static void record_event_handler(char *event, void *arg)
{
    recorder_t *rc = &sRecorder;
    vplay_event_info_t *veinfo = (vplay_event_info_t *)arg;
    char *fname = NULL;
    infotm_record_callback callback = rc->callback;

    pthread_mutex_lock(&rc->mutex); 
    if (!rc->inited) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }
    pthread_mutex_unlock(&rc->mutex); 

    ipclog_debug("+record_event_handler(%s) type:%d \n", event, veinfo->type);
    
    ipclog_info("rec a file %s\n", (char *)veinfo->buf);
    //fname = strrchr((char *)veinfo->buf, '/');
    //fname += 1; /*!< skip '/' */
    fname = (char *)veinfo->buf;
    if (callback) {
        callback(veinfo->type, fname);
    }

    ipclog_debug("-record_event_handler(%s)\n", event);
}

int infotm_record_init(RECORD_ATTR_T *attr, infotm_record_callback callback)
{
    int ret = 0; 
    recorder_t *rc = &sRecorder;
    ipclog_debug("%s start\n", __FUNCTION__); 
    pthread_mutex_lock(&rc->mutex);
    if (rc->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }
    ret = recInit_l(attr);

    rc->callback = callback;

    rc->inited = 1;
    pthread_mutex_unlock(&rc->mutex);
    ipclog_debug("%s done\n", __FUNCTION__); 
    return ret;
}

int infotm_record_deinit(void)
{
    int ret;
    recorder_t *rc = &sRecorder;
    ipclog_debug("%s start\n", __FUNCTION__);

    pthread_mutex_lock(&rc->mutex);
    if (rc->inited == 0) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }
    
    ret = recDeinit_l();

    rc->callback = NULL;
    rc->inited = 0;

    pthread_mutex_unlock(&rc->mutex); 
    ipclog_debug("%s done\n", __FUNCTION__);
    
    return ret;
}

int infotm_record_start(void)
{
    int ret = 0;
    recorder_t *rc = &sRecorder;
    ipclog_debug("%s start\n", __FUNCTION__);

    pthread_mutex_lock(&rc->mutex);
    if (rc->inited == 0) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }

    ret = recStart_l();
    pthread_mutex_unlock(&rc->mutex);

    ipclog_debug("%s done\n", __FUNCTION__);
    
    return ret;
}

int infotm_record_stop(void)
{
    int ret = 0;
    recorder_t *rc = &sRecorder;
    ipclog_debug("%s start\n", __FUNCTION__);
    pthread_mutex_lock(&rc->mutex);
    if (rc->inited == 0) {
        ipclog_warn("%s deinited already \n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }

    ret = recStop_l();

    pthread_mutex_unlock(&rc->mutex);

    ipclog_debug("%s done\n", __FUNCTION__);
    
    return ret;
}

int infotm_record_section(void)
{
    int ret = 0;
    recorder_t *rc = &sRecorder;
    ipclog_debug("%s start\n", __FUNCTION__);
    pthread_mutex_lock(&rc->mutex);
    if (rc->inited == 0) {
        ipclog_warn("%s deinited already \n", __FUNCTION__);
        pthread_mutex_unlock(&rc->mutex); 
        return 0;
    }

    ret = recSection_l();

    pthread_mutex_unlock(&rc->mutex);

    ipclog_debug("%s done\n", __FUNCTION__);
    
    return ret;
}
