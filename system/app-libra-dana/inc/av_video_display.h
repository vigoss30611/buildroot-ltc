#ifndef _AV_VIDEO_DISPLAY_H_
#define _AV_VIDEO_DISPLAY_H_

#include "av_osa_msg.h"
#include "av_video_common.h"

enum {
    DISPLAY_NONE = 0x0,
    DISPLAY_FRONT = 0x101,
    DISPLAY_REAR = 0x1001,  /*   */
    DISPLAY_FRONT_REAR = 0x111,
    DISPLAY_REAR_FRONT = 0x1011,
};

void video_display_init(int  tskId, int bufferId);
void video_display_set_mode(int mode);
int video_display_get_mode();
void video_get_display_size(int *width, int *height);

int video_display_getScreenInfo
    (T_MSG_PRODUCER cameraSrc, unsigned int *width_out, unsigned int *height_out, 
    RAW_PIC_TYPE *pictype_out, T_DISPLAY_PIP_TYPE *piptype_out);

void video_display_setPipScreen(T_MSG_PRODUCER cameraSrc, T_DISPLAY_PIP_TYPE pipType);

    
#endif


