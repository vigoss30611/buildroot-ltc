#include "audio.h"

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
#include <linux/reboot.h>
#include <linux/input.h>
#include <stdarg.h>

#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

#include "ipc.h"
#include "looper.h"

#define LOGTAG  "INFOTM_AUDIO"
#define DEBUG_AUDIO_STREAM
#define DEBUG_AUDIO_TALK

#define TALK_STREAM_DECODE_BUFFER_SIZE 20*1024
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

enum {
     PASG711A = 0,
    //PASAAC,
     PASNUM
};
enum {
    ACDEFAULT,
    ACVOICELINK,
    ACNUM,
};

typedef struct {
    int id;
    char const *chnl;
    bool enabled;
    int handle;
    audio_fmt_t format;
    int frcnt;
    struct fr_buf_info frbuf;
} audio_capture_t;

typedef struct {
    /*!< pcm related */
    int handle;
    audio_fmt_t format;
    int sampleBundleSize;
    char *buffer;
    int offset;
    int frcnt;
} audio_play_t;

typedef struct {
    pthread_mutex_t mutex;
    void *codec;
    struct codec_info codecInfo;
    char *encbuf;
    int encbufSize;
    audio_capture_t const *acapRef;
#ifdef DEBUG_AUDIO_STREAM
    FILE *fpPCM;
    FILE *fpEncode;
#endif
} audio_stream_t;

typedef struct {
    pthread_mutex_t mutex; 
    /*!< audio decode related */
    void *codec;
    struct codec_info codecInfo;
    char *decbuf;
    int decbufSize;

    audio_play_t const *aplayRef;
#ifdef DEBUG_AUDIO_TALK
    FILE *fpPCM;
    FILE *fpEncode;
#endif
} talkback_stream_t;

typedef struct {
    pthread_mutex_t mutex;
    looper_t lp;
    audio_stream_t stream;
    audio_post_fn_t callback;
} audio_module_t;

typedef struct {
    pthread_mutex_t mutex;
    int inited;
    talkback_stream_t stream;
} talkback_module_t;

static audio_capture_t sAudioCap[ACNUM];

static audio_module_t sAudioModule;
static audio_play_t sAudioPlay;
static talkback_module_t sTalkModule = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .inited = 0
};

static void talkStrmDeinit(void);

static int talkStrmInit(void)
{
    talkback_stream_t *ts = &(sTalkModule.stream);
    memset((void *)ts, 0, sizeof(talkback_stream_t));
    
    ts->aplayRef = &sAudioPlay;
    audio_play_t *aplay = ts->aplayRef;  

    ipclog_debug("talkStrmInit()\n");

    aplay->frcnt = 0;

    /*!< configure audio play channel */
    aplay->format.channels = 2;
    aplay->format.samplingrate = 8000;
    aplay->format.bitwidth = 32;
    aplay->format.sample_size = 1024;
    aplay->sampleBundleSize = 8192;    /*!< channles * bitwidth * sample_size */
    aplay->handle = audio_get_channel("default", &aplay->format, CHANNEL_BACKGROUND);
    if (aplay->handle < 0) {
        ipclog_ooo("audio_get_channel(default) failed\n");
        goto error;
    }

    /*!< configure audio codec */
    ts->codecInfo.in.codec_type = AUDIO_CODEC_G711A;
    ts->codecInfo.in.effect = 0;
    ts->codecInfo.in.channel = 1;
    ts->codecInfo.in.sample_rate = 8000;
    ts->codecInfo.in.bitwidth = 16;
    ts->codecInfo.in.bit_rate = 
        ts->codecInfo.in.channel * ts->codecInfo.in.sample_rate * GET_BITWIDTH(ts->codecInfo.in.bitwidth);
    ts->codecInfo.out.codec_type = AUDIO_CODEC_PCM;
    ts->codecInfo.out.effect = 0;
    ts->codecInfo.out.channel = aplay->format.channels;
    ts->codecInfo.out.sample_rate = aplay->format.samplingrate;
    ts->codecInfo.out.bitwidth = aplay->format.bitwidth;
    ts->codecInfo.out.bit_rate = 
        ts->codecInfo.out.channel * ts->codecInfo.out.sample_rate * GET_BITWIDTH(ts->codecInfo.out.bitwidth);
    ts->codec = codec_open(&ts->codecInfo);
    if (!ts->codec) {
        ipclog_ooo("codec open for type %d failed\n", ts->codecInfo.in.codec_type);
        goto error;
    }

    /*!< allocate pcm buffer and audio decode output buffer */
    aplay->buffer = malloc(aplay->sampleBundleSize);
    if (!aplay->buffer) {
        ipclog_ooo("alloc %d bytes of ts->buffer failed\n", aplay->sampleBundleSize);
        goto error;
    }
    aplay->offset = 0;

    ts->decbufSize = TALK_STREAM_DECODE_BUFFER_SIZE;
    ts->decbuf = malloc(ts->decbufSize);
    if (!ts->decbuf) {
        ipclog_ooo("alloc %d bytes of ts->decbuf failed\n", ts->decbufSize);
        goto error;
    }

#ifdef DEBUG_AUDIO_TALK
    ts->fpPCM = NULL;
    ts->fpEncode = NULL;

    if (!access("/mnt/sd0/audio_debug", 0)) {
        char file_name[128];
        sprintf(file_name, "%s.pcm", "/mnt/sd0/audio_play");
        ts->fpPCM = fopen(file_name, "wb"); 
        if (ts->fpPCM) { 
            printf("open %s success!\n", file_name); 
        } else { 
            printf("open %s failed!\n", file_name); 
        }

        sprintf(file_name, "%s.encode", "/mnt/sd0/audio_play");
        ts->fpEncode = fopen(file_name, "wb"); 
        if (ts->fpEncode) { 
            printf("open %s success!\n", file_name); 
        } else { 
            printf("open %s failed!\n", file_name); 
        }
    }
#endif

    ipclog_debug("%s done\n", __FUNCTION__);

    return 0;
error:
    ipclog_error("%s failed\n", __FUNCTION__);
    talkStrmDeinit();
    return -1;
}

static void talkStrmDeinit(void)
{
    talkback_module_t *tm = &(sTalkModule);    
    talkback_stream_t *ts = &tm->stream;
    audio_play_t *aplay = ts->aplayRef;

    ipclog_debug("%s \n", __FUNCTION__);

    int ret, length;
    struct codec_addr codecAddr;

    codecAddr.in      = NULL;
    codecAddr.len_in  = 0;
    codecAddr.out     = ts->decbuf;
    codecAddr.len_out = ts->decbufSize;

    if (ts->codec) { 
        do {
            ret = codec_flush(ts->codec, &codecAddr, &length);
        } while (ret == CODEC_FLUSH_AGAIN);

        codec_close(ts->codec);
        ts->codec = NULL;
    }
    
    if (ts->decbuf) {
        free(ts->decbuf);
        ts->decbuf = NULL;
    }

    if (aplay->buffer) {
        free(aplay->buffer);
        aplay->buffer = NULL;
    }
    aplay->offset = 0;
   
    if (aplay->handle >= 0) {
        audio_put_channel(aplay->handle);
    }
    
    ipclog_debug("%s done\n", __FUNCTION__);
}


static int talkStrmWrite(void *data, int len)
{     
    talkback_stream_t *ts = &(sTalkModule.stream);
    audio_play_t *aplay = ts->aplayRef; 
    int i, copied=0;
    int ret = 0;
    struct codec_addr codecAddr;

    if (aplay->frcnt == 0) {
        aplay->offset = 0;
        /*!< FIXME: fillfull of audio(alsa) start_threshold */
        memset(aplay->buffer, 0, aplay->sampleBundleSize);
        for (i=0; i<4096/aplay->format.sample_size; i++) {
            printf("%s audio_write_frame for start_threshold :%d \n", __FUNCTION__, i);
            if (audio_write_frame(aplay->handle, aplay->buffer, aplay->sampleBundleSize) < 0) {
                ipclog_error("audio_write_frame() failed\n");
                break;
            }
            printf("%s audio_write_frame for start_threshold :%d end\n", __FUNCTION__, i);
        }
    }

    if (!(aplay->frcnt++ & 0xFF)) {
        ipclog_info("talkback: frcnt %d, size %d\n", aplay->frcnt,  len);
    }

    codecAddr.in      = data;
    codecAddr.len_in  = len;
    codecAddr.out     = ts->decbuf;
    codecAddr.len_out = ts->decbufSize;

    ret = codec_decode(ts->codec, &codecAddr);
    if (ret < 0) {
        ipclog_error("codec_decode() failed, ret=%d\n", ret);
    } else {
        //ipclog_debug("decoded size %d\n", ret);
        int len = ret;
        int decodeSize = ret;
        char *ptr = ts->decbuf; 
        while(len > 0) {
             copied = FFMIN(len, (aplay->sampleBundleSize - aplay->offset));
             memcpy(aplay->buffer + aplay->offset, ptr, copied);
             aplay->offset += copied; 
             if (aplay->offset >= aplay->sampleBundleSize) {
                 ret = audio_write_frame(aplay->handle, aplay->buffer, aplay->sampleBundleSize);
                 if (ret < 0) {
                     ipclog_error("audio_write_frame() failed, ret=%d\n", ret);
                     aplay->offset = 0;
                     /*!< FIXME: if write error, must reinit pcm channel */
                     audio_put_channel(aplay->handle);
                     aplay->handle = audio_get_channel("default", &aplay->format, CHANNEL_BACKGROUND);
                     if (aplay->handle < 0) {
                         ipclog_ooo("reinit audio_get_channel() failed, ret=%d\n", aplay->handle);
                         ipc_panic("audio crashed");
                     }
                     return -1;
                 }
                 //ipclog_debug("write size %d\n", ret);
                 aplay->offset -= aplay->sampleBundleSize;
             }
             ptr += copied;
             len -= copied;
        }
#if 0
        //TODO: need be fixed by bsp
        //      segement fault cause by size != sampleBundleSize
        if (offset) {
            audio_write_frame(aplay->handle, aplay->buffer, offset);
            offset = 0;
        }
#endif
#ifdef DEBUG_AUDIO_TALK 
        if (aplay->frcnt < 300) {
            if (ts->fpPCM) {
                fwrite(codecAddr.in, codecAddr.len_in, 1, ts->fpEncode);
            }

            if (ts->fpEncode) {
                fwrite(ts->decbuf, decodeSize, 1, ts->fpPCM);
            }
        } else {
            if (ts->fpPCM) {
                fflush(ts->fpPCM);
                fclose(ts->fpPCM);
                ts->fpPCM = NULL;
                ipclog_info("%s debug audio play stream pcm done\n", __FUNCTION__);
            }

            if (ts->fpEncode) {
                fflush(ts->fpEncode);
                fclose(ts->fpEncode);
                ts->fpEncode = NULL;
                ipclog_info("%s debug audio play stream encode done\n", __FUNCTION__);
            }
        }
#endif
    }
    
    return 0;
}

static int audio_talk_init(void) 
{
    ipclog_debug("%s \n", __FUNCTION__);
    int handle = (int)&sTalkModule;
    
    pthread_mutex_lock(&sTalkModule.mutex);
    if (sTalkModule.inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
    } else {
        if (talkStrmInit() < 0) {
            handle = 0;
        } else {    
            sTalkModule.inited = 1;
        }
    }
    pthread_mutex_unlock(&sTalkModule.mutex);
    
    ipclog_debug("%s done\n", __FUNCTION__);
    return handle;
}

static int audio_talk_deinit(int handle)
{
    ipclog_debug("%s \n", __FUNCTION__);
    int sTalkHandle = (int)&sTalkModule;    

    if (handle != sTalkHandle) {
        ipclog_error("%s invalid handle:%d\n", __FUNCTION__, handle);
        return 0;
    }

    pthread_mutex_lock(&sTalkModule.mutex);
    if (sTalkModule.inited == 0) {
        ipclog_warn("%s already\n", __FUNCTION__);
    } else {
        talkStrmDeinit();
        sTalkModule.inited = 0;
    }
    pthread_mutex_unlock(&sTalkModule.mutex);

    ipclog_debug("%s done\n", __FUNCTION__);
    return 0;
}

static int audio_talk_write(int handle, char *data, int len)
{
    int sTalkHandle = (int)&sTalkModule;    
    int ret = 0;
    if (handle != sTalkHandle) {
        ipclog_error("%s invalid handle:%d\n", __FUNCTION__, handle);
        return 0;
    }

    pthread_mutex_lock(&sTalkModule.mutex);
    if (sTalkModule.inited == 0) {
        ipclog_warn("%s not inited already\n", __FUNCTION__);
    } else {
        ret = talkStrmWrite(data, len);
    } 
    pthread_mutex_unlock(&sTalkModule.mutex);
    
    return ret;
}

int infotm_audio_out_init(void)
{
    ipclog_debug("%s \n", __FUNCTION__);

    int handle = audio_talk_init();
    
    ipclog_debug("%s done\n", __FUNCTION__);
    return handle;
}

int infotm_audio_out_deinit(int handle)
{
    ipclog_debug("%s \n", __FUNCTION__);

    audio_talk_deinit(handle);

    ipclog_debug("%s done\n", __FUNCTION__);
    return 0;
}

int infotm_audio_out_send(int handle, char *data, int len)
{
    //ipclog_debug("%s data:%p len:%d \n", __FUNCTION__, data, len);

    audio_talk_write(handle, data, len);
    
    //ipclog_debug("%s data:%p len:%d done\n", __FUNCTION__, data, len);
    return 0;
}

int infotm_audio_out_set_volume(int handle, int lvolume, int rvolume)
{
    ipclog_debug("%s, %d:%d \n", __FUNCTION__, lvolume, rvolume);
    return 0;
}



static int acapProcessor(audio_stream_t *as)
{
    audio_capture_t *ac = as->acapRef;
    struct codec_addr codecAddr;
    struct fr_buf_info *fr = &ac->frbuf;    
    
    int ret;

    /*!< get audio pcm frame */
    ret = audio_get_frame(ac->handle, fr);
    if ((ret < 0) || (fr->size <= 0) || (fr->virt_addr == NULL)) {
        ipclog_error("acap %d audio_get_frame() failed, ret=%d\n", ac->id, ret);
        return -1;
    }

    if (!(ac->frcnt++ & 0xFF)) {
        ipclog_info("acap[%d]: frcnt %d buf 0x%x size %d\n", ac->id, ac->frcnt, fr->buf, fr->size);
    }

    media_aframe_t mafr = {0};
    codecAddr.in = fr->virt_addr;
    codecAddr.len_in = fr->size;
    codecAddr.out = as->encbuf;
    codecAddr.len_out = as->encbufSize;
    mafr.size = codec_encode(as->codec, &codecAddr);
    if (mafr.size < 0) {
        ipclog_error("codec_encode() failed, ret=%d\n", mafr.size);
    } else if (mafr.size > 0) {
        //ipclog_debug("encoded: buf %p size %d\n", as->encbuf, mafr.size);
        mafr.buf = as->encbuf;
        mafr.ts = fr->timestamp;
        if (!(ac->frcnt & 0xFF)) {
            ipclog_info("audio capture id %p frcnt %d size:%d len:%d ts:%llu \n", 
                    ac->id, ac->frcnt, fr->size, mafr.size, mafr.ts);
        }

#ifdef DEBUG_AUDIO_STREAM 
        if (ac->frcnt < 1000) {
            if (as->fpPCM) {
                fwrite(fr->virt_addr, fr->size, 1, as->fpPCM);
            }
            
            if (as->fpEncode) {
                fwrite(mafr.buf, mafr.size, 1, as->fpEncode);
            }
        } else {
            if (as->fpPCM) {
                fflush(as->fpPCM);
                fclose(as->fpPCM);
                as->fpPCM = NULL;
                ipclog_info("%s debug audio stream pcm done\n", __FUNCTION__);
            }

            if (as->fpEncode) {
                fflush(as->fpEncode);
                fclose(as->fpEncode);
                as->fpEncode = NULL;
                ipclog_info("%s debug audio stream encode done\n", __FUNCTION__);
            }

        }
#endif
        if (sAudioModule.callback) {
            sAudioModule.callback(NULL, (void const *)&mafr); 
        }
    }

    /*!< put back audio pcm frame */
    audio_put_frame(ac->handle, fr);

    return 0;
}

static void * audio_stream_thread(void *p)
{
    int req;
    audio_module_t *am = (audio_module_t *)p;
    audio_stream_t *as = &(am->stream); 
    audio_capture_t *ac = as->acapRef;
   
    ipclog_debug("%s enter\n", __FUNCTION__);

    /*!< loop of audio stream processing */
    lp_lock(&am->lp);
    lp_set_state(&am->lp, STATE_IDLE);
    while (lp_get_req(&am->lp) != REQ_QUIT) {
        /*!< wait for start/quit requst when in idle state */
        if (lp_get_state(&am->lp) == STATE_IDLE) {
            req = lp_wait_request_l(&am->lp);
            if (req == REQ_START) {
                fr_INITBUF(&ac->frbuf, NULL);
                ac->frcnt = 0;
            } else if (req == REQ_QUIT) {
                goto Quit;
            }
        } else {
            /*!< check stop request */
            if (lp_check_request_l(&am->lp) == REQ_STOP) {
                continue;
            }
        }
        lp_unlock(&am->lp);

        //TODO: audio_test_frame first
        acapProcessor(as);

        usleep(20000);

        lp_lock(&am->lp);
    }

Quit:
    lp_unlock(&am->lp);
    ipclog_debug("%s exit\n", __FUNCTION__);
    return NULL;
}


static int acapInit(int id)
{
    audio_capture_t *ac = &sAudioCap[id];
    audio_fmt_t format;
    ipclog_debug("acapInit(%d)\n", id);

    memset((void *)ac, 0, sizeof(audio_capture_t));
    ac->handle = -1;
    ac->id = id;

    /*!< configure acap channel */
    if (id == ACDEFAULT) {
        ac->chnl = "default_mic";
        ac->format.channels = 2;
        ac->format.samplingrate = 8000;
        ac->format.bitwidth = 32;
        ac->format.sample_size = 960;
    } else {
        ac->chnl = "default_mic";
        ac->format.channels = 2;
        ac->format.samplingrate = 48000;
        ac->format.bitwidth = 32;
        ac->format.sample_size = 1024;
    }

    /*!< take configuration in effect */
    ac->handle = audio_get_channel(ac->chnl, &ac->format, CHANNEL_BACKGROUND);
    if (ac->handle < 0) {
        ipclog_ooo("audio get channel of %s failed\n", ac->chnl);
        ipc_panic("audiobox crashed");
        return -1;
    }

    if (audio_get_format(ac->chnl, &format) < 0) {
        ipclog_ooo("audio get format of %s failed\n", ac->chnl);
        ipc_panic("audiobox crashed");
        return -1;
    }

    ac->enabled = true;


    fr_INITBUF(&ac->frbuf, NULL);

    return 0;
}

static void acapDeinit_l(int id)
{
    audio_capture_t *ac = &sAudioCap[id];

    ipclog_debug("acapDeinit_l(%d)\n", id);
    /*!< disable channel */
    audio_put_channel(ac->handle);
    ac->handle = -1;
    ac->enabled = false;
    ipclog_debug("acap %d stream stopped\n", id);
}

static int prevAudioStrmInit(int id)
{
    ipclog_debug("prevAudioStrmInit(%d)\n", id);
    
    int acapId;
    audio_stream_t *as = &(sAudioModule.stream);
    memset((void *)as, 0, sizeof(audio_stream_t));
    
    /*!< bind PAS to AC */
    acapId = ACDEFAULT;
    audio_capture_t *ac = (audio_capture_t *)&sAudioCap[acapId];
    as->acapRef = ac;

    if (id == PASG711A) {
        as->codecInfo.in.codec_type = AUDIO_CODEC_PCM;
        as->codecInfo.in.effect = 0;
        as->codecInfo.in.channel = ac->format.channels;
        as->codecInfo.in.sample_rate = ac->format.samplingrate;
        as->codecInfo.in.bitwidth = ac->format.bitwidth;
        as->codecInfo.in.bit_rate = as->codecInfo.in.channel * as->codecInfo.in.sample_rate * GET_BITWIDTH(as->codecInfo.in.bitwidth);
        as->codecInfo.out.codec_type = AUDIO_CODEC_G711A;
        as->codecInfo.out.effect = 0;
        as->codecInfo.out.channel = 1;
        as->codecInfo.out.sample_rate = 8000;
        as->codecInfo.out.bitwidth = 16;
        as->codecInfo.out.bit_rate = as->codecInfo.out.channel * as->codecInfo.out.sample_rate * GET_BITWIDTH(as->codecInfo.out.bitwidth);
        as->codec = codec_open(&as->codecInfo);
        if (as->codec == NULL) {
            ipclog_ooo("codec open for type %d failed\n", as->codecInfo.out.codec_type);
            return -1;
        }
        as->encbufSize = 1024;
    }

    as->encbuf = malloc(as->encbufSize);
    if (as->encbuf == NULL) {
        ipclog_ooo("alloc %d bytes of as->encbuf failed\n", as->encbufSize);
        return -1;
    }

#ifdef DEBUG_AUDIO_STREAM
    as->fpPCM = NULL;
    as->fpEncode = NULL;
    
    if (!access("/mnt/sd0/audio_debug", 0)) {
        char file_name[128];
        sprintf(file_name, "%s_%d.pcm", "/mnt/sd0/audio_stream", id);
        as->fpPCM = fopen(file_name, "wb"); 
        if (as->fpPCM) { 
            printf("open %s success!\n", file_name); 
        } else { 
            printf("open %s failed!\n", file_name); 
        }

        sprintf(file_name, "%s_%d.encode", "/mnt/sd0/audio_stream", id);
        as->fpEncode = fopen(file_name, "wb"); 
        if (as->fpEncode) { 
            printf("open %s success!\n", file_name); 
        } else { 
            printf("open %s failed!\n", file_name); 
        }
    }
#endif


    return 0;
}

static void prevAudioStrmDeinit(int id)
{
    int ret, length;
    struct codec_addr codecAddr;
    audio_stream_t *as = &(sAudioModule.stream);

    ipclog_debug("prevAudioStrmDeinit(%d)\n", id);

    if (as->codec) {
        do {
            ret = codec_flush(as->codec, &codecAddr, &length);
        } while (ret == CODEC_FLUSH_AGAIN);

        codec_close(as->codec);
        as->codec = NULL;
    }

    if (as->encbuf) {
        free(as->encbuf);
        as->encbuf = NULL;
    }
}

int infotm_audio_in_init(audio_post_fn_t callback)
{
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_init(&sAudioModule.mutex, NULL);

    sAudioModule.callback = callback;
 
    acapInit(0);
    prevAudioStrmInit(0);

    /*!< create acap looper */
    if (lp_init(&sAudioModule.lp, "audio_stream", audio_stream_thread, (void *)&sAudioModule) != 0) {
        ipclog_ooo("create audio preview looper failed\n");
        return -1;
    }

    ipclog_info("%s done\n", __FUNCTION__);
    return 0;
}

int infotm_audio_in_deinit(void)
{
    ipclog_info("%s \n", __FUNCTION__);

    lp_deinit(&sAudioModule.lp);

    prevAudioStrmDeinit(0);
    acapDeinit_l(0);

    pthread_mutex_destroy(&sAudioModule.mutex);

    ipclog_info("%s done\n", __FUNCTION__);

    return 0;
}

int infotm_audio_in_start(void)
{
    lp_start(&sAudioModule.lp, false);
    return 0;
}

int infotm_audio_in_stop(void)
{
    lp_stop(&sAudioModule.lp, false);
    return 0;
}

