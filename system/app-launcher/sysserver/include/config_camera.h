#ifndef CONFIG_CAMERA_H
#define CONFIG_CAMERA_H

#include <stdint.h>
//#include <sys/cdef.h>

//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define ITEM_SISE 128

typedef struct config_camera {
    //current mode:video,photo,setup,other
    char current_mode[ITEM_SISE];
    char current_reverseimage[ITEM_SISE];
    //video 
    char video_resolution[ITEM_SISE];
    char video_looprecording[ITEM_SISE];
    char video_rearcamera[ITEM_SISE];
    char video_pip[ITEM_SISE];
    char video_frontbig[ITEM_SISE];
    char video_wdr[ITEM_SISE];
    char video_ev[ITEM_SISE];
    char video_recordaudio[ITEM_SISE];
    char video_datestamp[ITEM_SISE];
    char video_gsensor[ITEM_SISE];
    char video_platestamp[ITEM_SISE];
    char video_zoom[ITEM_SISE];
    //photo
    char photo_capturemode[ITEM_SISE];
    char photo_resolution[ITEM_SISE];
    char photo_sequence[ITEM_SISE];
    char photo_quality[ITEM_SISE];
    char photo_sharpness[ITEM_SISE];
    char photo_whitebalance[ITEM_SISE];
    char photo_color[ITEM_SISE];
    char photo_isolimit[ITEM_SISE];
    char photo_ev[ITEM_SISE];
    char photo_antishaking[ITEM_SISE];
    char photo_datestamp[ITEM_SISE];
    char photo_zoom[ITEM_SISE];
    //setup
    char setup_tvmode[ITEM_SISE];
    char setup_frequency[ITEM_SISE];
    char setup_irled[ITEM_SISE];
    char setup_imagerotation[ITEM_SISE];
    unsigned char setup_platenumber[10];
    //other
    char other_collide[ITEM_SISE];
}config_camera;

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
