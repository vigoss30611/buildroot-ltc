/*
 * Copyright (c) 2015  INFOTMIC 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * ffmpeg muxer interface.
 *
 * Input encoded video and audio stream
 * Output a media file in any supported libavformat format. The default
 * codecs are used.
 *
 * */
#include "muxing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __USE_GNU 1
#include <fcntl.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/url.h>

typedef struct __OutputStream {
    AVStream *st;

    int64_t last_ts;
    int64_t next_ts;

    int  frame_count;
}OutputStream;

struct __AyseMuxerContext {
    AVFormatContext *oc;
    OutputStream  video_st;
    OutputStream  audio_st;

    char file_name[1024];
    int have_video;
    int have_audio;
    
    int first;
    int direct;     /* io O_DIRECT */
};

#define AYSE_CODEC_SUPPORT(name_, type_, codec_)\
 AVCodec ff_codec_## name_ = {\
        .name = #name_,\
        .type = AVMEDIA_TYPE_## type_,\
        .id = AV_CODEC_ID_## codec_,\
    };

/* register supported codec */
AYSE_CODEC_SUPPORT(pcm_s16le, AUDIO, PCM_S16LE);
AYSE_CODEC_SUPPORT(pcm_alaw, AUDIO, PCM_ALAW);
AYSE_CODEC_SUPPORT(aac, AUDIO, AAC);
AYSE_CODEC_SUPPORT(avc, VIDEO, H264);
AYSE_CODEC_SUPPORT(hevc, VIDEO, HEVC);
AYSE_CODEC_SUPPORT(mjpeg, VIDEO, MJPEG);

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
            av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
            av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
            av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
            pkt->stream_index);
}

const char * get_muxer_format(AyseMuxerType type) {
    switch(type) {
        case MUXER_MP4:
            return "mov";
        case MUXER_AVI:
            return "avi";
        case MUXER_MPEGTS:
            return "mpegts";
        case MUXER_AAC:
            return "aac";
        case MUXER_WAV:
            return "wav";
        default:
            return NULL;
    }
}

int av_muxer_open(AyseMuxerContext **muxer, AyseMuxerType type, const char *file_name, int flags) 
{
    struct __AyseMuxerContext *mux;
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    int ret;

    mux = (AyseMuxerContext *)av_mallocz(sizeof(*mux)); 
    if (!mux) {
        printf("no mem to allocate AyseMuxerContext \n");
        return -1;
    }

    /* Initialize libavcodec, and register all codecs and formats. */
    av_register_all();

    /* allocate the output media context */
    avformat_alloc_output_context2(&oc, NULL, NULL, file_name);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MuxerType.\n");
        avformat_alloc_output_context2(&oc, NULL, get_muxer_format(type), file_name);
    }

    if (!oc)
        return -1;

    fmt = oc->oformat;

    av_dump_format(oc, 0, file_name, 1);

    if (flags & AYSE_IO_FLAG_ODIRECT) {
        printf("AYSE_IO_FLAG_ODIRECT  \n");
        mux->direct = 1;
    }

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, file_name, AVIO_FLAG_WRITE | (mux->direct ? AVIO_FLAG_ODIRECT : 0));
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", file_name,
                    av_err2str(ret));
            goto ERROR;
        }
    }

    printf("oc flags:%08x  \n", oc->flags);
    oc->flags &= ~AVFMT_FLAG_FLUSH_PACKETS;
    mux->oc = oc;
    mux->first = 1;
    mux->have_audio = 0;
    mux->have_video = 0;
    *muxer = (AyseMuxerContext *)mux;

    return 0;
ERROR:
    /* free the stream */
    avformat_free_context(oc);

    /* free muxer*/
    av_free(mux);
    *muxer = NULL;
    
    return -1;
}

AVCodec *get_ff_codec(AyseCodecID codec_id)
{
    AVCodec *codec;
    switch (codec_id) {
        case AYSE_CODEC_ID_PCM_S16LE:
            codec = &ff_codec_pcm_s16le;
            break;
        case AYSE_CODEC_ID_PCM_ALAW:
            codec = &ff_codec_pcm_alaw;
            break;
        case AYSE_CODEC_ID_AAC:
            codec = &ff_codec_aac;
            break;
        case AYSE_CODEC_ID_AVC:
            codec = &ff_codec_avc;
            break;
        case AYSE_CODEC_ID_HEVC:
            codec = &ff_codec_hevc;
            break;
        case AYSE_CODEC_ID_MJPEG:
            codec = &ff_codec_mjpeg;
            break;
        default:
            codec = NULL;
    }

    return codec;
}

int av_muxer_add_stream(AyseMuxerContext *mux, AyseCodecInfo *codec_info)
{
    struct __AyseMuxerContext *muxer = (struct __AyseMuxerContext *)mux;
    AVFormatContext *oc = muxer->oc;
    OutputStream *ost;
    AVCodecContext *c;
    AVCodec *codec;

    codec = get_ff_codec(codec_info->codec_id);

    if (codec == NULL) {
        printf("now does not support this ayse codec(%08x) \n", codec_info->codec_id);
        return -1;
    }

    ost = codec_info->codec_id < AYSE_CODEC_FIRST_VIDEO ?  &muxer->audio_st : &muxer->video_st;

    ost->st = avformat_new_stream(oc, codec);

    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        return -1;
    }

    ost->st->id = oc->nb_streams-1;
    c = ost->st->codec;
    c->codec_id = codec->id;

    switch (codec->type) {
        case AVMEDIA_TYPE_AUDIO:
            //c->sample_fmt  = get_sample_fmt(codec_info->sample_fmt);
            c->bit_rate    = codec_info->bit_rate;
            c->sample_rate = codec_info->sample_rate;
            c->channels        = codec_info->channels;
            //ost->st->time_base = (AVRational){ 1, c->sample_rate };
            muxer->have_audio = 1;
            break;

        case AVMEDIA_TYPE_VIDEO:
            //c->codec_id = codec_id;

            /* timebase: This is the fundamental unit of time (in seconds) in terms
             * of which frame timestamps are represented. For fixed-fps content,
             * timebase should be 1/framerate and timestamp increments should be
             * identical to 1. */
            //ost->st->time_base = (AVRational){ 1, codec_info->frame_rate };
            //c->time_base       = ost->st->time_base;
            c->width = codec_info->width;
            c->height = codec_info->height;
            muxer->have_video = 1;
            break;

        default:
            break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return 0;    
}

int av_muxer_write_frame(AyseMuxerContext *mux,  AyseFrame *frame, AyseCodecID id)
{
    struct __AyseMuxerContext *muxer = (struct __AyseMuxerContext *)mux;
    AVFormatContext *oc = muxer->oc;
    OutputStream *ost;
    int ret = 0; 
    ost = id < AYSE_CODEC_FIRST_VIDEO ?  &muxer->audio_st : &muxer->video_st;

    if(muxer->first) {
        AVDictionary *opt = NULL;
#ifdef FRAG_DURATION
        printf("set frag duration :5s  \n");
        av_dict_set(&opt, "frag_duration", "5000000", 0);
#endif 
        ret = avformat_write_header(oc, &opt);
        if (ret < 0) {
            fprintf(stderr, "Error occurred when opening output file: %s\n",
                    av_err2str(ret));
#ifdef FRAG_DURATION
            if(opt != NULL) {
                av_dict_free(&opt);
            }
#endif 
            return -1;
        }
        muxer->first = 0;

        if (muxer->direct) {
            URLContext *h = oc->pb->opaque;
            int fd = h->prot->url_get_file_handle(h);
            int old_flag = fcntl(fd, F_GETFL);
            int new_flag = old_flag | O_DIRECT;
            int ret = fcntl(fd, F_SETFL, new_flag);
            int real_flag = fcntl(fd, F_GETFL);

            printf("%s fcntl oldFlag:%x newFlag:%x realFlag:%x ret :%x\n", __FUNCTION__,
                    old_flag, new_flag, real_flag , ret);
        }

#ifdef FRAG_DURATION
        if(opt != NULL) {
            av_dict_free(&opt);
        }
#endif 
    }

    if(frame == NULL || frame->data_size <= 0) { 
        printf("frame(%p) data error, size=%d \n", frame, frame != NULL ? frame->data_size : 0);
        return -1;
    }

    if(frame->type == AYSE_FRAME_TYPE_EXTRADATA) {
        if (!ost->st->codec->extradata_size && frame->data_size > 0) {
            ost->st->codec->extradata = av_mallocz(frame->data_size + FF_INPUT_BUFFER_PADDING_SIZE);                                                                             
            if (ost->st->codec->extradata) {        
                memcpy(ost->st->codec->extradata, frame->data, frame->data_size);
                ost->st->codec->extradata_size = frame->data_size;                                                                                                               
            }
        }
    } else {
        AVPacket pkt;

        /* check timestamp */
        if(frame->timestamp < ost->last_ts + 5000ll) {
            printf("timestamp(%lld, %lld) error, workaround  \n", frame->timestamp, ost->last_ts);
            frame->timestamp += 5000ll;
            if(frame->timestamp <= ost->last_ts) {
                frame->timestamp = ost->last_ts  + 5000ll;
            }
        } 
        
        ost->last_ts = frame->timestamp;
        
        av_init_packet(&pkt);

        pkt.pts = pkt.dts = av_rescale_q(frame->timestamp, AV_TIME_BASE_Q, ost->st->time_base);
        if (frame->type == AYSE_FRAME_TYPE_I) {
            pkt.flags |= AV_PKT_FLAG_KEY;
        }

        pkt.stream_index = ost->st->index;
        pkt.data = (uint8_t *)frame->data;
        pkt.size = frame->data_size;

        struct timeval time1, time2;
        gettimeofday(&time1, NULL);
        /* write the compressed frame in the media file */
        ret = av_write_frame(oc, &pkt);
        gettimeofday(&time2, NULL);
        int interval = (time2.tv_sec - time1.tv_sec) * 1000 + (time2.tv_usec - time1.tv_usec)/1000;
        if(interval > 100) {
            printf("oc:%p, write time:%ldms \n ", oc, interval);
        } 
    }
    return ret;
}

int av_muxer_close(AyseMuxerContext *mux, int sync) 
{
    struct __AyseMuxerContext *muxer = (struct __AyseMuxerContext *)mux;
    AVFormatContext *oc = muxer->oc;

    if (muxer->direct) {
        URLContext *h = oc->pb->opaque;
        int fd = h->prot->url_get_file_handle(h);
        int old_flag = fcntl(fd, F_GETFL);
        int new_flag = old_flag & ~O_DIRECT;
        int ret = fcntl(fd, F_SETFL, new_flag);
        int real_flag = fcntl(fd, F_GETFL);

        printf("%s fcntl oldFlag:%x newFlag:%x realFlag:%x ret :%x\n", __FUNCTION__,
                old_flag, new_flag, real_flag , ret);
    }

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    if (oc->pb) {
        av_write_trailer(oc);
    }

    if (sync) {
        URLContext *h = oc->pb->opaque;
        fsync(h->prot->url_get_file_handle(h));
    }

    if (!(oc->oformat->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&oc->pb);

    /* free the stream */
    avformat_free_context(oc);

    /* free muxer */
    av_free(muxer);
    muxer = NULL;
    return 0;
}
