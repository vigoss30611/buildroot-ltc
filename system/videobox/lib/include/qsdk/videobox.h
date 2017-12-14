// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __VIDEOBOX_API_H__
#define __VIDEOBOX_API_H__
#include <time.h>
#include <qsdk/event.h>
#include <fr/libfr.h>


#ifdef __cplusplus
extern "C" {
#endif

#define VB_RPC_EVENT "vb_request"
#define VB_RPC_INFO "vb_debug_info"
#define VB_CHN_LEN 32

typedef struct {
    int w;
    int h;
} vbres_t;

typedef struct {
    int code;
    int para;
} vbctrl_t;

enum VBCTRL {
    VBCTRL_INDEX,
    VBCTRL_GET,
};

typedef struct {
    char func[VB_CHN_LEN];
    char chn[VB_CHN_LEN];
    int ret;
} vbrpc_t;

typedef struct timespec timepin_t;
static inline void tp_poke(timepin_t *tp)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, tp);
}

static inline int tp_peek(timepin_t *tp)
{
    struct timespec t2;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);

    return t2.tv_sec * 10000 + t2.tv_nsec / 100000 -
        tp->tv_sec * 10000 - tp->tv_nsec / 100000;
}

#define VB_SC_N 2
#define videobox_launch_rpc_l(c, b, l)   \
    struct event_scatter _sc[VB_SC_N];  \
    vbrpc_t _rpc = {.func = {0}, .chn = {0}, .ret = 0}; int _ret; do {  \
    strncpy(_rpc.func, __func__, VB_CHN_LEN);\
    if(c) strncpy(_rpc.chn, c, VB_CHN_LEN);\
    _sc[0].buf = (char *)&_rpc;  \
    _sc[0].len = sizeof((_rpc));  \
    _sc[1].buf = (char *)b;  \
    _sc[1].len = l;  \
    _ret = event_rpc_call_scatter(VB_RPC_EVENT, _sc, VB_SC_N);  \
    if(_ret < 0) return _ret;  \
} while(0)
#define videobox_launch_rpc(c, b) videobox_launch_rpc_l(c, b, sizeof(*b))

#define VBOK 0
#define VBEFAILED -1
#define VBENOFUNC -2
#define VBEBADPARAM -3
#define VBEMEMERROR -4
#define VBEALREADYUP -5

#define EVENT_VIDEOBOX "videobox"

/* common API */
extern int videobox_start(const char *js);
extern int videobox_stop(void);
extern int videobox_rebind(const char *p0, const char *p2);
extern int videobox_repath(const char *js);
extern int videobox_control(const char *js, int code, int para);
extern int videobox_mod_offset(const char *ipu, int x, int y);
extern int videobox_get_jsonpath(void *buf, int len);

/*
 * @brief exchange data with videobox
 * @param ps8Type                  -IN: the channel of encoder
 * @param ps8SubType               -IN: the sei data buffer
 * @param pInfo                    -IN/OUT: the buffer for exchange
 * @param pInfo                    -IN: the buffer length
 * @return 0: Success
 * @return <0: Failure
 */
extern int videobox_exchange_data(const char *ps8Type, const char *ps8SubType, void *pInfo, const int s32MemSize);

/*
 * @brief get debug info from videobox
 * @param ps8SubType           -IN: the type of debug info
 * @param pInfo                -IN/OUT: the debug info
 * @param pInfo                -IN: the buffer length
 * @return 0: Success
 * @return <0: Failure
 */
extern int videobox_get_debug_info(const char *ps8SubType, void *pInfo, const int s32MemSize);

/* sub-API to inlcude */
#include <qsdk/video.h>
#include <qsdk/camera.h>
#include <qsdk/vam.h>
#include <qsdk/marker.h>
#include <qsdk/vaqr.h>
#include <qsdk/fodet.h>
#include <qsdk/IPUInfo.h>
#ifdef __cplusplus
}
#endif
#endif /* __VIDEOBOX_API_H__ */
