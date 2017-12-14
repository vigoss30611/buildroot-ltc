#ifdef GUI_EVENT_HTTP_ENABLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/time.h>

#include <event1.h>
#include <evhttp.h>

#include "filesave.h"

#include "spv_config_keys.h"
#include "spv_debug.h"
#include "spv_event.h"
#include "spv_config.h"
#include "spv_language_res.h"


#define OK_CMD "OK"
#define ERROR_CMD "ERROR"
#define ERROR_INVALIDE_ARGUMENT "ERROR: invalid argument"
#define ERROR_SET_FAILED "ERROR: set failed"

const char* EVENT_NAME[] = {
    "getDateTime",
    "setDateTime",
    "getDeviceStatus",
    "setDeviceStatus",
    "getResolution",
    "setResolution",
    "getLoop",
    "setLoop",
    "takePhoto",
    "getVersion",
};

const char* STATUS_VALUE[] = {
    "recording",
    "stop",
};

const char* RESOLUTION_VALUE[] = {
    "1080",
    "720",
};

const char* LOOP_VALUE[] = {
    "1",
    "3",
    "5",
};

struct evhttp *httpd = NULL;

void root_handler(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf;

    buf = evbuffer_new();
    if (buf == NULL)
        err(1, "failed to create response buffer");
    evbuffer_add_printf(buf, "Hello World!/n");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

static int get_time_string(char *replyBuf)
{
    static time_t rawtime;
    static struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    int y = 1900+timeinfo->tm_year;
    int M = timeinfo->tm_mon + 1;
    int d = timeinfo->tm_mday;
    int h = timeinfo->tm_hour;
    int m = timeinfo->tm_min;
    int s = timeinfo->tm_sec;
    sprintf(replyBuf, "%04d%02d%02d%02d%02d%02d", y, M, d, h, m, s);

    return 0;
}

static int set_time_string(char *replyBuf, char *value)
{
    if(value == NULL) {
        strcpy(replyBuf, ERROR_CMD": invalid argument");
        return -1;
    }

    int y = -1, M = -1, d = -1, h = -1, m = -1, s = 0; 
    sscanf(value, "%4d%2d%2d%2d%2d%2d", &y, &M, &d, &h, &m, &s);
    if(y == -1 || M == -1 || d == -1 || h == -1 || m == -1) {
        strcpy(replyBuf, ERROR_CMD": invalid argument");
        return -1;
    }

    int ret = SetTimeByApp(y, M, d, h, m, s);
    if(!ret) {
        strcpy(replyBuf, OK_CMD);
    } else {
        strcpy(replyBuf, ERROR_CMD);
    }

    return ret;
}

static int get_status(char *replyBuf)
{
    strcpy(replyBuf, isCameraRecording() ? STATUS_VALUE[STATUS_RECORDING] : STATUS_VALUE[STATUS_STOP]);
    return 0;
}

static int set_status(char *replyBuf, char *value) {
    int status = -1;
    if(!strncasecmp(value, STATUS_VALUE[STATUS_RECORDING], strlen(STATUS_VALUE[STATUS_RECORDING]))) {
        status = 1;
    } else if(!strncasecmp(value, STATUS_VALUE[STATUS_STOP], strlen(STATUS_VALUE[STATUS_STOP]))) {
        status = 0;
    }

    if(status == -1) {
        strcpy(replyBuf, ERROR_INVALIDE_ARGUMENT);
    } else {
        setRecordingStatus(status);
        strcpy(replyBuf, OK_CMD);
    }

    return status < 0;
}

static int get_resolution(char *replyBuf)
{
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
    if(!strcasecmp(value, GETVALUE(STRING_1080FHD))) {
        strcpy(replyBuf, RESOLUTION_VALUE[RESOLUTION_FHD]);
    } else {
        strcpy(replyBuf, RESOLUTION_VALUE[RESOLUTION_HD]);
    }
    return 0;
}

static int set_resolution(char *replyBuf, char *value)
{
    int valueId = -1;

    if(!strncasecmp(value, RESOLUTION_VALUE[RESOLUTION_FHD], strlen(RESOLUTION_VALUE[RESOLUTION_FHD]))) {
        valueId = STRING_1080FHD;
    } else if(!strncasecmp(value, RESOLUTION_VALUE[RESOLUTION_HD], strlen(RESOLUTION_VALUE[RESOLUTION_HD]))) {
        valueId = STRING_720P_30FPS;
    } else {
        strcpy(replyBuf, ERROR_CMD": invalid argument");
        return -1;
    }

    int ret = HandleConfigChangedByHttp(ID_VIDEO_RESOLUTION, valueId);
    if(!ret) {
        strcpy(replyBuf, OK_CMD);
    } else {
        strcpy(replyBuf, ERROR_SET_FAILED);
    }

    return ret;
}

static int get_loop_time(char *replyBuf)
{
    char value[128] = {0};
    int bufId = -1;
    GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
    if(!strcasecmp(value, GETVALUE(STRING_1_MINUTE))) {
        bufId = LOOP_1_MINUTE;
    } else if(!strcasecmp(value, GETVALUE(STRING_3_MINUTES))) {
        bufId = LOOP_3_MINUTES;
    } else {
        bufId = LOOP_5_MINUTES;
    }

    strcpy(replyBuf, LOOP_VALUE[bufId]);
    return 0;
}

static int set_loop_time(char *replyBuf, char *value)
{
    int valueId = -1;
    if(!strncasecmp(value, LOOP_VALUE[LOOP_1_MINUTE], strlen(LOOP_VALUE[LOOP_1_MINUTE]))) {
        valueId = STRING_1_MINUTE;
    } else if(!strncasecmp(value, LOOP_VALUE[LOOP_3_MINUTES], strlen(LOOP_VALUE[LOOP_3_MINUTES]))) {
        valueId = STRING_3_MINUTES;
    } else if(!strncasecmp(value, LOOP_VALUE[LOOP_5_MINUTES], strlen(LOOP_VALUE[LOOP_5_MINUTES]))) {
        valueId = STRING_5_MINUTES;
    } else {
        strcpy(replyBuf, ERROR_INVALIDE_ARGUMENT);
        return -1;
    } 

    int ret = HandleConfigChangedByHttp(ID_VIDEO_LOOP_RECORDING, valueId);
    if(!ret) {
        strcpy(replyBuf, OK_CMD);
    } else {
        strcpy(replyBuf, ERROR_SET_FAILED);
    }

    return ret;
}

static int take_photo(char *replyBuf) {
    char name[128] = {0};
    int ret = get_jpg_serial_name(0,name);
    INFO("take_photo, name: %s\n", name);
    if(ret) {
        goto error_failed;
    }

    ret = take_photo_with_name(0,name);
    if(ret)
        goto error_failed;

    //block until the real file is generated.
    int timeout = 50;//5 seconds
    do {
        if(!access(name, F_OK)) {
            INFO("file exist, %s\n", name);
            break;
        }

        usleep(100 * 1000);
    }while(timeout-- > 0);

    if(timeout <= 0) {
        printf("time out for 5 seconds to take photo\n");
        goto error_failed;
    }
    usleep(500 * 1000);//wait 500ms for data written

    strcpy(replyBuf, name);
    return 0;

error_failed:
    strcpy(replyBuf, ERROR_SET_FAILED);
    return -1;
}

static int get_version(char *replyBuf)
{
    char version[128] = {0};
    SpvGetVersion(version);
    strcpy(replyBuf, version);
    return 0;
}

int handle_message(const char* cmd, char *replyBuf)
{
    int ret = 0;
    if(cmd == NULL) {
        goto error_msg;
    }

    int cmdlen = strlen(cmd);
    if(cmdlen <= 0) {
        goto error_msg;
    }

    int max_event_count = sizeof(EVENT_NAME) / sizeof(EVENT_NAME[0]);
    int cmdId = CMD_INVALID;
    int i = 0;
    for(i = 0; i < max_event_count; i++) {
        if(!strncasecmp(cmd, EVENT_NAME[i], strlen(EVENT_NAME[i]))) {
            cmdId = i;
            break;
        }
    }

    if(cmdId == CMD_INVALID) {
        goto error_msg;
    }

    char *value = NULL;
    if(strlen(cmd) > (strlen(EVENT_NAME[cmdId]) + 1))
        value = cmd+(strlen(EVENT_NAME[cmdId]) + 1);
    INFO("handle_message, cmd: %s, value: %s\n", cmd, value);
    switch(cmdId) {
        case CMD_GET_TIME:
            ret = get_time_string(replyBuf);
            break;
        case CMD_SET_TIME:
            ret = set_time_string(replyBuf, value);
            break;
        case CMD_GET_STATUS:
            ret = get_status(replyBuf);
            break;
        case CMD_SET_STATUS:
            ret = set_status(replyBuf, value);
            break;
        case CMD_GET_RESOLUTION:
            ret = get_resolution(replyBuf);
            break;
        case CMD_SET_RESOLUTION:
            ret = set_resolution(replyBuf, value);
            break;
        case CMD_GET_LOOP:
            ret = get_loop_time(replyBuf);
            break;
        case CMD_SET_LOOP:
            ret = set_loop_time(replyBuf, value);
            break;
        case CMD_TAKE_PHOTO:
            ret = take_photo(replyBuf);
            break;
        case CMD_GET_VERSION:
            ret = get_version(replyBuf);
            break;
        default:
            goto error_msg;
            break;
    }

    return ret;

error_msg:
    sprintf(replyBuf, ERROR_CMD);
    return -1;

}

    void
generic_handler(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf;
    INFO("got request: %s\n", evhttp_request_uri(req));

    buf = evbuffer_new();
    if (buf == NULL) {
        ERROR("failed to create response buffer\n");
        return;
    }
    char replyBuf[128] = {0};
    int ret = handle_message(evhttp_request_uri(req) + 1, replyBuf);
    INFO("handle_message, ret: %d, cmd: %s, replyBuf: %s\n", ret, evhttp_request_uri(req), replyBuf);
    evbuffer_add_printf(buf, "%s", replyBuf);
    evhttp_send_reply(req, ret ? HTTP_BADREQUEST : HTTP_OK, "OK", buf);
}

int deinit_event_server()
{
    if(httpd != NULL) {
        INFO("deinit_event_server");
        evhttp_free(httpd);
        httpd = NULL;
    }
}

int init_event_server()
{
    deinit_event_server();
    event_init();
     httpd = evhttp_start("0.0.0.0", 8280);

    /* Set a callback for requests to "/". */
    evhttp_set_cb(httpd, "/", root_handler, NULL);

    /* Set a callback for all other requests. */
    evhttp_set_gencb(httpd, generic_handler, NULL);

    event_dispatch();

    return 0;
}

#endif
