#ifndef __INFOTM_PLAYBACK_H__
#define __INFOTM_PLAYBACK_H__

#include <qsdk/demux.h>

#define VHDRBUF_SIZE    (128)
typedef struct {
    char vheader[VHDRBUF_SIZE];
    int vhdrlen;
    struct demux_t *demuxer;
    struct demux_frame_t demuxframe;
} playback_handle_t;

typedef struct {
    int audio_index;
    int audio_codec_id;
    
    int video_index;
    int video_codec_id;
    int video_fps;
    int video_width;
    int video_height;
    
    int time_pos;
    int time_len;
    int frame_num;
} playback_info_t;

int playback_init(char *filename);
int playback_deinit(int handle);
int playback_get_info(int handle, playback_info_t *info);
int playback_get_frame(int handle, struct demux_frame_t *demuxframe);
int playback_get_iframe(int handle, struct demux_frame_t *demuxframe);
int playback_put_frame(int handle, struct demux_frame_t *demuxframe);

#endif
