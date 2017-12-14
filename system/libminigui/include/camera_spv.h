#ifndef CAMERA_SPV_H
#define CAMERA_SPV_H

#include "config_camera.h"
#include "common_inft.h"

//#include <sys/cdef.h>

#define ERROR_NULL		-1
#define ERROR_GET_FAIL		-2
#define ERROR_SET_FAIL		-3
#define ERROR_CREATE_THREAD 	-4
#define ERROR_CANCEL_THREAD 	-5

#define STATUS_SD_FULL    	1
#define STATUS_NO_SD    	2
#define STATUS_ERROR    	3

#define FRONTIER		40
//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
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
    int (*start_photo_with_name)(char *filename);
    int (*stop_photo)();

    int (*camera_error_callback)(int flag);
}camera_spv;

camera_spv *camera_create(config_camera config);
void camera_destroy(camera_spv *camera);

void register_camera_callback(camera_spv *camera_spv_p, camera_error_callback_p callback);

int get_ambientBright(void);
void set_video_resolution(char *resolution);

int rear_camera_inserted(void);
#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
