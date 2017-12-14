/**
  ******************************************************************************
  * @file       video.h
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

#ifndef __INFOTM_VIDEO_H__
#define __INTOFM_VIDEO_H__

#include <stdint.h>

#define VIDEO_STREAM_NAME_MAX 64
typedef struct 
{
    char bindfd[VIDEO_STREAM_NAME_MAX];
    int  width;
    int  height;
    int  fps;
    int  media_type;
} ven_info_t;

#define MEDIA_VFRAME_FLAG_EOS   (1<<0)
#define MEDIA_VFRAME_FLAG_KEY   (1<<1)
typedef struct {
    int flag;
    char *header;
    int hdrlen;
    void *buf;
    int size;
    int64_t ts;
} media_vframe_t;

typedef void (*video_post_fn_t) (void const *client, media_vframe_t const *vfr);

int infotm_video_preview_start(int id, void const *client, video_post_fn_t vfrpost);
int infotm_video_preview_stop(int id, void const *client);

void infotm_video_init(void);
void infotm_video_deinit(void);
void infotm_video_reset(void);

int infotm_video_set_bitrate(int id, int bitrate, int rc_mode, int p_frame, int quality);
int infotm_video_set_resolution(int id, int w, int h);
int infotm_video_set_fps(int id, int fps);
int infotm_video_set_viewmode(int id, int viewmode);

ven_info_t *infotm_video_info_get(int id);
char * infotm_video_get_name(int id);

int infotm_video_info_num_get(void);

#endif
