#ifndef __SPV_EVENT_H__
#define __SPV_EVENT_H__

typedef enum {
    CMD_INVALID = -1,
    CMD_GET_TIME,
    CMD_SET_TIME,
    CMD_GET_STATUS,
    CMD_SET_STATUS,
    CMD_GET_RESOLUTION,
    CMD_SET_RESOLUTION,
    CMD_GET_LOOP,
    CMD_SET_LOOP,
    CMD_TAKE_PHOTO,
    CMD_GET_VERSION,
} CMD_TYPE;
extern const char* EVENT_NAME[];


typedef enum {
    STATUS_RECORDING,
    STATUS_STOP,
} STATUS_TYPE;
extern const char* STATUS_VALUE[];

typedef enum {
    RESOLUTION_FHD,
    RESOLUTION_HD,
} RESOLUTION_TYPE;
extern const char* RESOLUTION_VALUE[];

typedef enum {
    LOOP_1_MINUTE,
    LOOP_3_MINUTES,
    LOOP_5_MINUTES,
} LOOP_TYPE;
extern const char* LOOP_VALUE[];


int init_event_server();
int deinit_event_server();

#endif
