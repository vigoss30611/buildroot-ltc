
#ifndef __SMART_RC_H__
#define __SMART_RC_H__

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <log.h>
#include <qsdk/videobox.h>
#include <Json.h>

typedef struct  {
    pthread_mutex_t mutex;

    unsigned int mode;  // 00: day-move 01:day-still  10: night-move 11: night-still
    struct timeval movement_record; // last motion detected 

}stSmartRCCtx;

typedef void (*msg_handler) (char *event, void *arg);

typedef struct {
    char msg[32];
    msg_handler handler;
}stSmartRCMsgHandler;

typedef struct  {
    int stream_num;
    int level_num;
    int mode_num;
    int time_gap;
    int bps[4];
    int s32ThrOverNumer;
    int s32ThrOverDenom;
    int s32ThrNormalNumer;
    int s32ThrNormalDenom;
}stSmartRCInfo;

typedef struct
{
    int bps;
    int intraQpDelta;
    int qpMin;
    int qpMax;
}stSmartRCSet;

typedef struct {
    int t; //top
    int b; //bottom
    int l; //left
    int r; //right
    int cnt;
}stAdjoin;

typedef struct {
    char stream[32];
    stSmartRCSet  *pSet;
    bool change_flag;
    int s32Index;
    int s32State;  //0: normal   1: bps over  2: bps over rollback
    struct v_rate_ctrl_info  user_set;
    struct v_rate_ctrl_info  new_set;
    struct timeval stTimeLast;
}stSmartRCDefaultSetting;

#define TIME_GAP_2S  (2)
#define TIME_GAP_5S  (5)
#define TIME_GAP_7S  (7)
#define TIME_GAP_10S (10)
#define TIME_GAP_30S (30)
#define TIME_GAP_1M  (60)
#define TIME_GAP_2M  (120)

#define STAT_RC_NORMAL   (0)
#define STAT_RC_ROLLBACK (1)
#define STAT_RC_OVERRATE (2)

#define RC_QP_MIN  (20)
#define RC_QP_MAX  (50)

extern stSmartRCDefaultSetting *smartrc_default_setting;
extern const stSmartRCMsgHandler smartrc_msg_handler_cb[];
extern const int smartrc_msg_handler_num;
extern stSmartRCInfo smartrc_info;
extern stSmartRCCtx gSmartRCContext;
extern bool g_tosetroi;
extern struct v_roi_info g_roi;

int smartrc_defaultsetting_init(Json *js);
int smartrc_register_msg_handler_cb();
int smartrc_unregister_msg_handler_cb();
void smartrc_monitor_system_status();
int smartrc_defaultsetting_deinit();

int getcnt(char raw[7][7], int fix, int min, int max, stAdjoin *src);
int adjoin(char raw[7][7], stAdjoin *src, int i, int j);
int injoin(int i, int j, stAdjoin *src);
struct v_roi_info get_roi_info(struct event_vamovement *efd);
int setroi(struct v_roi_info r);
void roi_alarm_handler(int signal);

/*
#ifdef __cplusplus
}
#endif
*/
#endif
