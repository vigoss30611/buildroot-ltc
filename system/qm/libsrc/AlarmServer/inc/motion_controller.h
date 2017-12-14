#ifndef __MOTION_CONTROLLER_H__
#define __MOTION_CONTROLLER_H__

#include <sys/time.h>

#define EVENT_MOTION_DETECT "motion_detect"

typedef enum {
    MOTDECT_END,
    MOTDECT_BEGIN
}MOTION_STATE_E;

typedef struct {
    MOTION_STATE_E motion;
    time_t t;
} motion_detect_info_t;

typedef void (*motion_detect_listen_fn_t) (int what, motion_detect_info_t *info);

int motion_controller_init(void);
int motion_controller_deinit(void);
int motion_controller_start(void);
int motion_controller_stop(void);
void motion_controller_register(motion_detect_listen_fn_t listener);
void motion_controller_unregister(motion_detect_listen_fn_t listener);

/*!< sensitivity range from 0 to 100, 0 means disable motdect */
int motion_controller_set(int sensitivity);
int motion_controller_get(void);
int motion_controller_blind(int duration);

#endif

