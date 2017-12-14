
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/demux.h>

#include "ipc.h"
#include "looper.h"
#include "AudioPrompt.h"
#include "QMAPINetSdk.h"

#define LOGTAG "AudioPrompt"

#define RINGFILE_NAME_LEN_MAX   (128)
#define RINGTONE_DECBUF_SIZE    (96 * 1024)
typedef struct {
    looper_t lp;
    int handle;
    audio_fmt_t format;
    int sampleBundleSize;
    void *codec;
    struct codec_info codecInfo;
    char *decbuf;
    int decbufSize;
    
    bool mute;
    char ringFile[RINGFILE_NAME_LEN_MAX];
} ringtone_play_t;

static ringtone_play_t sRingPlay;

#define AUDIO_BSIZE			1024

static pthread_mutex_t play_mutex = PTHREAD_MUTEX_INITIALIZER;

static void * ringPlayProcessor(void *p)
{
    int ret, req, i;
    ringtone_play_t *rp = (ringtone_play_t *)p;
    struct demux_t *demuxer = NULL;
    int decSize;
    struct demux_frame_t demuxFrame = {0};
    struct codec_addr codecAddr;
    char *ptr;
    pthread_attr_t attr;
    int policy = SCHED_FIFO;
    struct sched_param param;

    /*!< set high priority of ringplay thread */
    assert(pthread_attr_init(&attr) == 0);
    param.sched_priority = sched_get_priority_max(policy);
    assert(pthread_attr_setschedpolicy(&attr, policy) == 0);
    assert(pthread_attr_setschedparam(&attr, &param) == 0);
    assert(pthread_attr_getschedpolicy(&attr, &policy) == 0);
    assert(pthread_attr_getschedparam(&attr, &param) == 0);
    assert(pthread_attr_destroy(&attr) == 0);
    ipclog_debug("ring play processor, policy=%d, sched_priority=%d\n", 
            policy, param.sched_priority);

    ipclog_debug("enter ringtone play processor\n");

    /*!< loop of ringtone play processing */
    lp_lock(&rp->lp);
    lp_set_state(&rp->lp, STATE_IDLE);
    while (lp_get_req(&rp->lp) != REQ_QUIT) {
        /*!< wait for start/quit requst when in idle state */
        if (lp_get_state(&rp->lp) == STATE_IDLE) {
            req = lp_wait_request_l(&rp->lp);
            if (req == REQ_START) {
                /*!< do business */
            } else if (req == REQ_QUIT) {
                goto Quit;
            }
        } else {
            /*!< check stop request */
            if (lp_check_request_l(&rp->lp) == REQ_STOP) {
                continue;
            }
        }

        if (rp->ringFile[0] == '\0') {
            continue;
        }

        /*!< play ring file */
        while (true) {
            if (!demuxer) {
                ipclog_debug("start play ring: %s\n", rp->ringFile);
                demuxer = demux_init(rp->ringFile);
                if (!demuxer) {
                    ipclog_error("demux_init(%s) failed\n", rp->ringFile);
                    break;
                }
                /*!< FIXME: fillfull of audio(alsa) start_threshold */
                ipclog_debug("fullfill audio data for start_threshold\n");
                memset(rp->decbuf, 0, rp->decbufSize);
                for (i=0; i<4096/rp->format.sample_size; i++) {
                    if (audio_write_frame(rp->handle, rp->decbuf, rp->sampleBundleSize) < 0) {
                        ipclog_error("audio_write_frame() failed\n");
                        break;
                    }
                }
                ipclog_debug("fullfill audio data for start_threshold done\n");
            }

            ret = demux_get_frame(demuxer, &demuxFrame);
            if ((ret < 0) || (demuxFrame.data_size == 0)) {
                ipclog_warn("demux_get_frame(%s) failed, ret=%d\n", rp->ringFile, ret);
                break;
            }
            //ipclog_debug("demuxed size %d\n", demuxFrame.data_size);

            if (!rp->codec) {
                ipclog_debug("open codec, in: %d %d %d\n", demuxFrame.channels, 
                        demuxFrame.sample_rate, demuxFrame.bitwidth);
                rp->codecInfo.in.codec_type = AUDIO_CODEC_PCM;
                rp->codecInfo.in.effect = 0;
                rp->codecInfo.in.channel = demuxFrame.channels;
                rp->codecInfo.in.sample_rate = demuxFrame.sample_rate;
                rp->codecInfo.in.bitwidth = demuxFrame.bitwidth;
                rp->codecInfo.in.bit_rate = rp->codecInfo.in.channel * 
                    rp->codecInfo.in.sample_rate * GET_BITWIDTH(rp->codecInfo.in.bitwidth);
                rp->codec = codec_open(&rp->codecInfo);
                if (!rp->codec) {
                    ipclog_error("codec open for type %d failed\n", rp->codecInfo.in.codec_type);
                    break;
                }
            }

            codecAddr.in = demuxFrame.data;
            codecAddr.len_in = demuxFrame.data_size;
            codecAddr.out = rp->decbuf;
            codecAddr.len_out = rp->decbufSize;
            decSize = codec_decode(rp->codec, &codecAddr);
            if (decSize < 0) {
                ipclog_error("codec_decode() failed, ret=%d\n", decSize);
            } else {
                //ipclog_debug("decoded size %d\n", decSize);
                ptr = rp->decbuf;
                while (decSize > 0) {   /*!< FIXME: should be decoded size of non-last time same as rp->sampleBundleSize? */
                    /*!< FIXME: must fill 0 to spare space of decbuf */
                    if (decSize < rp->sampleBundleSize) {
                        memset(ptr + decSize, 0, rp->sampleBundleSize - decSize);
                    }
                    //ipclog_debug("start write\n");
                    ret = audio_write_frame(rp->handle, ptr, rp->sampleBundleSize);
                    //ret = audio_write_frame(rp->handle, rp->decbuf, (decSize < rp->sampleBundleSize) ? decSize : rp->sampleBundleSize);
                    if (ret < 0) {
                        ipclog_error("audio_write_frame() failed, ret=%d\n", ret);
                        /*!< FIXME: if write error, must reinit pcm channel */
                        audio_put_channel(rp->handle);
                        rp->handle = audio_get_channel("default", &rp->format, CHANNEL_BACKGROUND);
                        if (rp->handle < 0) {
                            ipclog_ooo("reinit audio_get_channel() failed, ret=%d\n", rp->handle);
                            ipc_panic("audio crashed");
                        }
                        break;
                    }
                    //ipclog_debug("write size %d\n", ret);
                    ptr += ret;
                    decSize -= ret;
                }
            }
            demux_put_frame(demuxer, &demuxFrame);
        }

        if (rp->codec) {
            do {
                codecAddr.out = rp->decbuf;
                codecAddr.len_out = rp->decbufSize;
                ret = codec_flush(rp->codec, &codecAddr, &decSize);
                if (ret == CODEC_FLUSH_ERR) {
                    break;
                }

                ipclog_debug("flushed size %d\n", decSize);
                ptr = rp->decbuf;
                while (decSize > 0) {   /*!< FIXME: should be decoded size of non-last time same as rp->sampleBundleSize? */
                    /*!< FIXME: must fill 0 to spare space of decbuf */
                    if (decSize < rp->sampleBundleSize) {
                        memset(ptr + decSize, 0, rp->sampleBundleSize - decSize);
                    }
                    //ipclog_debug("start write\n");
                    int l = audio_write_frame(rp->handle, ptr, rp->sampleBundleSize);
                    //ret = audio_write_frame(rp->handle, rp->decbuf, (decSize < rp->sampleBundleSize) ? decSize : rp->sampleBundleSize);
                    if (l < 0) {
                        ipclog_error("audio_write_frame() failed, ret=%d\n", l);
                        /*!< FIXME: if write error, must reinit pcm channel */
                        audio_put_channel(rp->handle);
                        rp->handle = audio_get_channel("default", &rp->format, CHANNEL_BACKGROUND);
                        if (rp->handle < 0) {
                            ipclog_ooo("reinit audio_get_channel() failed, ret=%d\n", rp->handle);
                            ipc_panic("audio crashed");
                        }
                        break;
                    }
                    //ipclog_debug("write size %d\n", l);
                    ptr += l;
                    decSize -= l;
                }
            } while (ret == CODEC_FLUSH_AGAIN);

            codec_close(rp->codec);
            rp->codec = NULL;
        }
        if (demuxer) {
            demux_deinit(demuxer);
            demuxer = NULL;
        }
        ipclog_debug("stop play ring: %s\n", rp->ringFile);
        rp->ringFile[0] = '\0';
        lp_set_state(&rp->lp, STATE_IDLE);

        /*!< for requestion checking */
        lp_unlock(&rp->lp);
        usleep(0);
        lp_lock(&rp->lp);
    }

Quit:
    lp_unlock(&rp->lp);
    ipclog_debug("leave ringtone play processor\n");
    return NULL;
}

static int ringPlayInit(void)
{
    ringtone_play_t *rp = &sRingPlay;

    ipclog_debug("ringPlayInit()\n");

    memset((void *)rp, 0, sizeof(ringtone_play_t));

    /*!< create ringtone play looper */
    if (lp_init(&rp->lp, "ringplay", ringPlayProcessor, (void *)rp) != 0) {
        ipclog_ooo("create ringtone play looper failed\n");
        return -1;
    }

    /*!< configure audio play channel */
    rp->format.channels = 2;
    rp->format.samplingrate = 8000;
    rp->format.bitwidth = 32;
    rp->format.sample_size = 1024;
    rp->sampleBundleSize = 8192;    /*!< channles * bitwidth * sample_size */
    rp->handle = audio_get_channel("default", &rp->format, CHANNEL_BACKGROUND);
    if (rp->handle < 0) {
        ipclog_ooo("audio_get_channel() failed, ret=%d\n", rp->handle);
        return -1;
    }
    ipclog_debug("rp format: %d %d %d %d\n", rp->format.channels, rp->format.samplingrate, rp->format.bitwidth, rp->format.sample_size);

    /*!< configure audio codec */
    rp->codecInfo.out.codec_type = AUDIO_CODEC_PCM;
    rp->codecInfo.out.effect = 0;
    rp->codecInfo.out.channel = rp->format.channels;
    rp->codecInfo.out.sample_rate = rp->format.samplingrate;
    rp->codecInfo.out.bitwidth = rp->format.bitwidth;
    rp->codecInfo.out.bit_rate = rp->codecInfo.out.channel * rp->codecInfo.out.sample_rate * GET_BITWIDTH(rp->codecInfo.out.bitwidth);
    ipclog_debug("rp codec out: %d %d %d\n", rp->codecInfo.out.channel, rp->codecInfo.out.sample_rate, rp->codecInfo.out.bitwidth);

    rp->decbufSize = RINGTONE_DECBUF_SIZE;
    rp->decbuf = malloc(rp->decbufSize);
    if (rp->decbuf == NULL) {
        ipclog_ooo("alloc %d bytes of rp->decbuf failed\n", rp->decbufSize);
        return -1;
    }

    return 0;
}

static void ringPlayDeinit(void)
{
    ringtone_play_t *rp = &sRingPlay;

    ipclog_debug("ringPlayDeinit()\n");

    /*!< stop and deinit looper */
    lp_stop(&rp->lp, false);
    lp_deinit(&rp->lp);

    if (rp->decbuf) {
        free(rp->decbuf);
        rp->decbuf = NULL;
    }
    
    codec_close(rp->codec);
    audio_put_channel(rp->handle);
}

static void ringPlayMute(bool mute)
{
    ringtone_play_t *rp = &sRingPlay;

    ipclog_debug("ringPlayMute(%d)\n", mute);
    
    lp_lock(&rp->lp);
    rp->mute = mute;
    lp_unlock(&rp->lp);
}

void ringSetMute(bool mute)
{
    ipclog_debug("ms_ringtone_set_mute(%d)\n", mute);
    ringPlayMute(mute);
}

static ringPlayFile(char const *file)
{
    ringtone_play_t *rp = &sRingPlay;

    ipclog_debug("ms_ringtone_play(%s)\n", file);
    assert(strlen(file) < RINGFILE_NAME_LEN_MAX);
    //lp_stop(&rp->lp, false);
    lp_lock(&rp->lp);
    if (!rp->mute) {
        while (rp->ringFile[0] != '\0') {
            lp_unlock(&rp->lp);
            usleep(100000);
            lp_lock(&rp->lp);
        }
        strcpy(rp->ringFile, file);
        lp_start(&rp->lp, true);
    } else {
        ipclog_debug("skip play ringtone %s for mute set\n", file);
    }
    lp_unlock(&rp->lp);
}

int QMAPI_AudioPromptInit()
{   
    return ringPlayInit();
}

int QMAPI_AudioPromptUnInit()
{
    ringPlayDeinit();
    return 0;
}

int QMAPI_AudioPromptStart(void)
{   
    return 0;
}

int QMAPI_AudioPromptStop(void)
{
    return 0;
}

int QMAPI_AudioPromptPlayFile(const char *fileName, int block)
{
    ringPlayFile(fileName);

    return 0;
}
