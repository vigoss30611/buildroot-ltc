#ifndef __DEMUXER_INTERFACE_H__
#define __DEMUXER_INTERFACE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
//#include <string.h>

typedef  void* DemuxerContext;

enum {
    AV_AUDIO,
    AV_VIDEO,
    AV_SUBTITLE,
};

typedef enum __StreamType {
    AV_STREAM_AUDIO,
    AV_STREAM_VIDEO,
    AV_STREAM_SUBTITLE
}StreamType;

typedef enum __IM_CodecID{
    IM_STREAM_NONE,
    IM_STREAM_AUDIO_AAC =  0x00000001,
    IM_STREAM_AUDIO_PCM_S16LE =  0x00000002,
    IM_STREAM_AUDIO_PCM_ALAW =  0x00000003,
    IM_STREAM_VIDEO_AVC =  0x00001000,
    IM_STREAM_VIDEO_HEVC = 0x00001001,
    IM_STREAM_VIDEO_MJPEG = 0x00001002,
}IM_CodecID;

typedef struct __KeyFrameIndexEntry {
    int64_t pos;
    int64_t timestamp;
}KeyFrameIndexEntry;


typedef struct __AudioInfo {
   IM_CodecID codec_id;
   int index;
   int sample_rate;
   int channels;
   int sample_format;
   
}AudioInfo;

typedef struct __VideoInfo {
    IM_CodecID codec_id;
    int index;
    int width;
    int height;
    int keyframe_count;
    int frame_count;
    int64_t frame_duration;

    KeyFrameIndexEntry *keyframe_index_entries;  /* must free when stream close */
}VideoInfo;

typedef struct __StreamInfo {
    int nb_streams;
    int64_t duration;
    int64_t file_size;
    char iformat[64];
    
    AudioInfo audio_info; 
    VideoInfo video_info;
}StreamInfo;


enum {
    AV_FRAME_FLAG_NONE,
    AV_FRAME_FLAG_KEY,
};

typedef struct __AV_BUFFER {
    void    *data;       /* data ptr */
    int      data_size;  /* data size */
    int64_t  timestamp;  /* timestamp */
    int64_t  frame_duration;  /* frame_duration */

    int      stream_index; /* stream index */
    int      flags;        /* indict keyframe or not */
    void    *opaque;       /* buffer context. set by demuxer interface */
} AV_BUFFER;

/**
 *function: open mediafile
 *
 * return:
 *  0           - ok
 *  -1          - failed  
 */
int  stream_open(const char *name, DemuxerContext *demuxer_context);

/**
 *function: get media info 
 *
 *return:   
 *   0   -  ok
 *   -1  -  fail
 */
int stream_get_info(DemuxerContext demuxer_context, StreamInfo *stream_info);

/**
 *function:   read a packet from demuxer lib
 *   
 *   return:
 *   0        ok
 *   1        end of stream
 *   -1       failed
 **/
int stream_read(DemuxerContext demuxer_context,AV_BUFFER *av_buffer);

/**
 *  function:  seek file to target us
 *
 *  return 
 *   0        seek ok
 *   -1       seek failed
 */
int stream_seek(DemuxerContext demuxer_context, int64_t target_us);

/**
 * function: close media file 
 * 
 * return :
 *   0                 ok
 *   -1                failed
 * */
int stream_close(DemuxerContext demuxer_context);

/**
 *function : free buffer
 *
 * void *opaque   --- av_buffer
 *
 *return:
 *   0                ok
 *   -1               failed
 **/
int stream_free_buffer(AV_BUFFER *buffer);


#endif //



