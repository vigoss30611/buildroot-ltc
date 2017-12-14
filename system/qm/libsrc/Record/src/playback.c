#include "playback.h"

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

#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

#include <qsdk/vplay.h>

#include "ipc.h"
#include "looper.h"

#define LOGTAG  "QM_PLAYBACK"

int playback_init(char *filename)
{
    ipclog_debug("%s filename:%s\n", __FUNCTION__, filename);

    playback_handle_t *handle = (playback_handle_t *)malloc(sizeof(playback_handle_t));
    if (!handle) {
        ipclog_error("%s OOM\n", __FUNCTION__);
        return NULL;
    }
    memset(handle, 0, sizeof(playback_handle_t));

    handle->demuxer = demux_init(filename);
    if (!handle->demuxer) {
        ipclog_error("demux_init(%s) failed\n", filename);
        goto error_exit;
    } else {
        ipclog_debug("demux_init(%s) before demux_get_head\n", filename);
        handle->vhdrlen = demux_get_head(handle->demuxer, handle->vheader, VHDRBUF_SIZE);
    }

    ipclog_debug("%s filename:%s done\n", __FUNCTION__, filename);

    return handle;

error_exit: 
    if (handle) {
        if (handle->demuxer) {
            demux_deinit(handle->demuxer);
            handle->demuxer = NULL;
        }

        free(handle);
        handle = NULL;
    }

    return NULL;
}

int playback_deinit(int handle)
{
    playback_handle_t *pbh = (playback_handle_t *)handle;
    ipclog_debug("%s handle:%p\n", __FUNCTION__, handle);

    if (pbh) {
        if (pbh->demuxer) {
            if (pbh->demuxframe.data) {
                ipclog_debug("%s handle:%p demux_put_frame\n", __FUNCTION__, handle);
                demux_put_frame(pbh->demuxer, &pbh->demuxframe);      
                pbh->demuxframe.data = NULL;
            }

            demux_deinit(pbh->demuxer);
            pbh->demuxer = NULL;
        }

        free(pbh);
        pbh = NULL;
    }

    ipclog_debug("%s handle:%p done\n", __FUNCTION__, handle);

    return 0;
}

int playback_get_info(int handle, playback_info_t *info)
{
    playback_handle_t *pbh = (playback_handle_t *)handle;
    if (!pbh) {
        ipclog_error("%s Invalid handle\n", __FUNCTION__);
        return -1;
    }

    struct demux_t *demuxer = pbh->demuxer;
    if (!demuxer) {
        ipclog_error("%s Invalid demuxer\n", __FUNCTION__);
        return -1;
    }

    info->audio_index = demuxer->audio_index;
    if ((info->audio_index != -1) && (info->audio_index < demuxer->stream_num)) {
        info->audio_codec_id = demuxer->stream_info[info->audio_index].codec_id; 
    } else {
        info->audio_index = -1;
    }

    info->video_index = demuxer->video_index;
    if ((info->video_index != -1) && (info->video_index < demuxer->stream_num)) {
        info->video_codec_id = demuxer->stream_info[info->video_index].codec_id; 
        info->video_fps      = demuxer->stream_info[info->video_index].fps; 
        info->video_width    = demuxer->stream_info[info->video_index].width; 
        info->video_height   = demuxer->stream_info[info->video_index].height; 
    } else {
        info->video_index = -1;
    }

    info->time_pos = demuxer->time_pos;
    info->time_len = demuxer->time_length;
    info->frame_num = demuxer->frame_counter;

    ipclog_info("playback:audio_index:%d \n"    , info->audio_index);
    ipclog_info("         audio_codec_id:%d \n" , info->audio_codec_id);
    ipclog_info("         video_index:%d \n"    , info->video_index);
    ipclog_info("         video_codec_id:%d \n" , info->video_codec_id);
    ipclog_info("         video_fps:%d \n"      , info->video_fps);
    ipclog_info("         video_width:%d \n"    , info->video_width);
    ipclog_info("         video_height:%d \n"   , info->video_height);
    ipclog_info("         time_pos:%d \n"       , info->time_pos);
    ipclog_info("         time_len:%d \n"       , info->time_len);
    ipclog_info("         frame_num:%d \n"      , info->frame_num);

    return 0;
}

int playback_get_frame(int handle, struct demux_frame_t *demuxframe)
{
    int ret = -1;
    playback_handle_t *pbh = (playback_handle_t *)handle;
    if (!pbh->demuxer) {
        ipclog_error("%s invalid handle\n", __FUNCTION__);
        return -1; 
    }
  
    //In fact, user should call playback_get_frame & playback_put_frame in pairs 
    if (pbh->demuxframe.data) {
        demux_put_frame(pbh->demuxer, &pbh->demuxframe);      
        pbh->demuxframe.data = NULL;
    }

    ret = demux_get_frame(pbh->demuxer, &pbh->demuxframe);
    if (ret < 0) {
        ipclog_error("demux_get_frame() failed, ret=%d\n", ret);
        demux_deinit(pbh->demuxer);
        pbh->demuxer = NULL;
        return -1;
    }

    memcpy((void*)demuxframe, (void* )(&pbh->demuxframe), sizeof(struct demux_frame_t));
#if 0
    ipclog_debug("%s handle:%d demuxframe:data:%p size:%d timestamp:%llu flags:%x code_id:%x \n", 
            __FUNCTION__, handle, 
            demuxframe->data, demuxframe->data_size, demuxframe->timestamp, demuxframe->flags, demuxframe->codec_id);
#endif
    //Modify flags for audio
    if ((demuxframe->codec_id != DEMUX_VIDEO_H264) && (demuxframe->codec_id != DEMUX_VIDEO_H265)) {
        demuxframe->flags =  AUDIO_FRAME_RAW;
    }

    return 0;
}

int playback_get_iframe(int handle, struct demux_frame_t *demuxframe)
{
    int ret = -1;
    while (playback_get_frame(handle, demuxframe) == 0) {
        if (demuxframe->flags == VIDEO_FRAME_I) {
            ipclog_info("%s handle:%p done\n", __FUNCTION__, handle);
            return 0;
        }
    }

    return -1;
}

int playback_put_frame(int handle, struct demux_frame_t *demuxframe)
{
    playback_handle_t *pbh = (playback_handle_t *)handle;
    if (!pbh->demuxer) {
        ipclog_error("%s invalid handle\n", __FUNCTION__);
        return -1; 
    }
   
    if (pbh->demuxframe.data && (pbh->demuxframe.data == demuxframe->data)) {
        demux_put_frame(pbh->demuxer, &pbh->demuxframe);      
        pbh->demuxframe.data = NULL;
    }

    return 0;
}


