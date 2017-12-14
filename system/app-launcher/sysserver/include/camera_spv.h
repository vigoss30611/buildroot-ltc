#ifndef CAMERA_SPV_H
#define CAMERA_SPV_H

#include "config_camera.h"
//#include "common_inft.h"

//#include <sys/cdef.h>

#define ERROR_NULL		-1
#define ERROR_GET_FAIL		-2
#define ERROR_SET_FAIL		-3
#define ERROR_CREATE_THREAD 	-4
#define ERROR_CANCEL_THREAD 	-5

#define FRONTIER		40
//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define SS0  "ispost0-ss0"
#define SS1  "ispost0-SS1"
#define G2CHN "dec0-stream"
#define G2FCHN "dec0-frame"
#define FSCHN "fs-out"
#define DCHN "display-osd0"
#define FCAM "isp"
#define VMARK "marker0"
#define PMARK "marker0"
#define VCHN "encavc0-stream"  //video local save channel
#define PCHN "jpeg-out"        //jpeg channel
#define PFCHN "jpegfull-out"        //jpeg channel

#define CALCHN "jpeg-out"

//video preview channel
#ifdef PREVIEW_SECOND_WAY
#define VPCHN "encavc1-stream"
#else
#define VPCHN VCHN
#endif

typedef int (*camera_error_callback_p)(int flag);

typedef struct camera_spv {
    config_camera *config;
//    char *current_mode;

    int (*init_camera)();
    int (*release_camera)();

    int (*set_mode)();
    int (*set_config)(config_camera config);

    int (*start_preview)();
    int (*stop_preview)();

    int (*start_video)();
    int (*stop_video)();

    int (*start_photo)();
    int (*stop_photo)();
    int (*take_picture)(char *filename, int width, int height);

    int (*camera_error_callback)(int flag);
}camera_spv;

camera_spv *camera_create(config_camera config);
void camera_destroy(camera_spv *camera);

void register_camera_callback(camera_spv *camera_spv_p, camera_error_callback_p callback);

int get_ambientBright(void);
void set_video_resolution(char *resolution);

int get_recorder_bitrate();

int rear_camera_inserted(void);
int confirm_p2p_stream_started();
#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
