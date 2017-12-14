#include "motion_controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/reboot.h>
#include <linux/input.h>
#include <stdarg.h>

#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <qsdk/cep.h>

#include "ipc.h"
#include "looper.h"
#include "qm_timer.h"
#include "qm_event.h"

#define LOGTAG  "MOTION_CTRL"

/*!< CFG_FEATURE_MOTDECT */
#define CFG_MOTDECT_FRESH_PERIOD        120 /*!< second */

typedef struct {
    pthread_mutex_t mutex;
    int inited;
    int debug;
    int sensitivity;
    bool enabled;
    bool alive;
    bool sceneMotional;
    struct timespec blindExpire;
    motion_detect_listen_fn_t listener;
} motdect_t;

static motdect_t sMotdect = {
    .inited = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .debug = 1,
};

static void motdectNotifyBeginTimedProcess(void const *p); 

static void motdectTimedProcess(void const *p)
{
    motdect_t *mot = (motdect_t *)p;
    motion_detect_info_t info;

    ipclog_debug("motdectTimedProcess()\n");

    pthread_mutex_lock(&mot->mutex);
    if (mot->alive) {
        ipclog_info("motion detect END\n");
        time(&info.t);
        info.motion = MOTDECT_END;
        mot->alive = false;
        if (mot->listener) {
            ipclog_debug("+mot->listener()\n");
            mot->listener(MOTDECT_END, &info);
            ipclog_debug("-mot->listener()\n");
        }

        QM_Event_Send(EVENT_MOTION_DETECT, &info, sizeof(info));
        pthread_mutex_unlock(&mot->mutex);

        /*!< timer proc will stop automatically */
    } else {
        pthread_mutex_unlock(&mot->mutex);
        
        QM_Timer_StopProcessor(motdectTimedProcess, true);
    }

    mot->debug = 1;
}

static void motion_detect_handler(char *event, void *arg)
{
    motdect_t *mot = &sMotdect;
    motion_detect_info_t info;
    struct timespec ts;

    if (mot->debug) {
        ipclog_debug("+motion_detect_handler(%s)\n", event);
    }
    assert(!strcmp(event, EVENT_VAMOVE));

    pthread_mutex_lock(&mot->mutex);
    if (!mot->enabled) {
        pthread_mutex_unlock(&mot->mutex);
        goto Quit;
    }

    if ((mot->blindExpire.tv_sec != 0) && (mot->blindExpire.tv_nsec != 0)) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        if ((mot->blindExpire.tv_sec > ts.tv_sec) ||
           ((mot->blindExpire.tv_sec == ts.tv_sec) && (mot->blindExpire.tv_nsec > ts.tv_nsec))) {
            ipclog_debug("blind period\n");
            pthread_mutex_unlock(&mot->mutex);
            goto Quit;
        }
        mot->blindExpire.tv_sec = 0;
        mot->blindExpire.tv_nsec = 0;
    }

    if (!mot->alive) {
        ipclog_info("motion detect BEGIN\n");
        mot->alive = true;
        time(&info.t);
        info.motion = MOTDECT_BEGIN;
        if (mot->listener) {
            ipclog_debug("+mot->listener()\n");
            mot->listener(MOTDECT_BEGIN, &info);
            ipclog_debug("-mot->listener()\n");
        }
        
        QM_Event_Send(EVENT_MOTION_DETECT, &info, sizeof(info));
        
        pthread_mutex_unlock(&mot->mutex);

        QM_Timer_RestartProcessor(motdectTimedProcess, false);
    } else {
        //ipclog_debug("motion dectect in fresh period\n");
        pthread_mutex_unlock(&mot->mutex);
        QM_Timer_RestartProcessor(motdectTimedProcess, false);
    }

Quit:
    if (mot->debug) {
        mot->debug = 0;
        ipclog_debug("-motion_detect_handler(%s)\n", event);
    }
    return;
}

static void motdectReset(void);
static int motdectInit(void)
{
    int ret;

    motdect_t *mot = &sMotdect;

    ipclog_debug("motdectInit()\n");

    pthread_mutex_lock(&mot->mutex);
    if (mot->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return 0;
    }

    mot->sensitivity = 100;
    ipclog_debug("mot->sensitivity = %d\n", mot->sensitivity);

    mot->enabled = false;
    mot->alive = false;
    mot->listener = NULL;

    ret = event_register_handler(EVENT_VAMOVE, 0, motion_detect_handler);
    if (ret != 0) {
        ipclog_ooo("event_register_handler(EVENT_VAMOVE) failed, ret=%d\n", ret);
        return -1;
    }

    QM_Timer_RegisterProcessor(motdectTimedProcess, "motdect", 
            (void const *)mot, CFG_MOTDECT_FRESH_PERIOD * 1000, 0);
   
    mot->inited = 1; 
    pthread_mutex_unlock(&mot->mutex);
    return 0;
}

static int motdectDeinit(void)
{
    ipclog_debug("motdectDeinit()\n");
    motdect_t *mot = &sMotdect;
    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return 0;
    }
    pthread_mutex_unlock(&mot->mutex);

    motdectReset();
    QM_Timer_UnregisterProcessor(motdectTimedProcess);
    
    //TODO: fixed me
    pthread_mutex_lock(&mot->mutex);
    mot->inited = 0; 
    pthread_mutex_unlock(&mot->mutex);

    return 0;
}

static int motdectStart(void)
{
    motdect_t *mot = &sMotdect;

    ipclog_debug("motdectStart()\n");

    /*!< TODO: set sensitivity and start motdect */
    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return 0;
    }

    if (!mot->enabled) { 
        mot->enabled = true;
    }

    QM_Timer_RegisterProcessor(motdectNotifyBeginTimedProcess, "motdect-n", 
            (void const *)mot, 100, TIMER_PROC_FLAG_ONESHOT | TIMER_PROC_FLAG_AUTOSTART);
    pthread_mutex_unlock(&mot->mutex);

    return 0;
}

static int motdectStop(void)
{
    motdect_t *mot = &sMotdect;
    motion_detect_info_t info;

    ipclog_debug("motdectStop()\n");

    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return 0;
    }

    if (mot->enabled) {
        /*!< TODO: motdect cannot be stopped */
        mot->enabled = false;
        if (mot->alive) {
            ipclog_info("motion detect stop\n");
            time(&info.t);
            mot->alive = false;
            info.motion = MOTDECT_END;
            if (mot->listener) {
                ipclog_debug("+mot->listener()\n");
                mot->listener(MOTDECT_END, &info);
                ipclog_debug("-mot->listener()\n");
            }

            QM_Event_Send(EVENT_MOTION_DETECT, &info, sizeof(info));
            pthread_mutex_unlock(&mot->mutex);

            QM_Timer_StopProcessor(motdectTimedProcess, false);
            
            return 0;
        }
    }
    pthread_mutex_unlock(&mot->mutex);

    return 0;
}

static void motdectReset(void)
{
    motdect_t *mot = &sMotdect;

    ipclog_debug("motdectReset()\n");

    motdectStop();
    pthread_mutex_lock(&mot->mutex);
    mot->listener = NULL;
    mot->blindExpire.tv_sec = 0;
    mot->blindExpire.tv_nsec = 0;
    mot->sensitivity = 0;
    pthread_mutex_unlock(&mot->mutex);
}

static void motdectNotifyBeginTimedProcess(void const *p)
{
    motdect_t *mot = (motdect_t *)p;
    motion_detect_info_t info;

    ipclog_debug("motdectNotifyBeginTimedProcess(%p)\n", p);

    pthread_mutex_lock(&mot->mutex);

    time(&info.t);
    ipclog_debug("+mot->listener()\n");
    if (mot->alive) {
        info.motion = MOTDECT_BEGIN;
        if (mot->listener) { 
            mot->listener(MOTDECT_BEGIN, &info);
        }
    } else {
        info.motion = MOTDECT_END;
        if (mot->listener) { 
            mot->listener(MOTDECT_END, &info);
        }
    }
    ipclog_debug("-mot->listener()\n");

    QM_Event_Send(EVENT_MOTION_DETECT, &info, sizeof(info));
    pthread_mutex_unlock(&mot->mutex);
}

int motion_controller_init(void)
{
    return motdectInit();
}

int motion_controller_deinit(void)
{
    return motdectDeinit(); 
}

int motion_controller_start(void)
{
    return motdectStart();
}

int motion_controller_stop(void)
{
    return motdectStop();
}

void motion_controller_register(motion_detect_listen_fn_t listener)
{
    motdect_t *mot = &sMotdect;

    ipclog_debug("%s(%p)\n", __FUNCTION__, listener);
    assert(listener);
    assert(mot->listener == NULL);  /*!< TODO: should able to support multi listener */

    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return;
    }

    mot->listener = listener;
    //if (mot->alive) {
        QM_Timer_RegisterProcessor(motdectNotifyBeginTimedProcess, "motdect-n", 
                (void const *)mot, 100, TIMER_PROC_FLAG_ONESHOT | TIMER_PROC_FLAG_AUTOSTART);
    //}
    pthread_mutex_unlock(&mot->mutex);
}

void motion_controller_unregister(motion_detect_listen_fn_t listener)
{
    motdect_t *mot = &sMotdect;

    ipclog_debug("%s(%p)\n", __FUNCTION__, listener);

    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return;
    }

    assert(mot->listener == listener);  
    mot->listener = NULL;
    pthread_mutex_unlock(&mot->mutex);
}

int motion_controller_set(int sensitivity)
{
    motdect_t *mot = &sMotdect;
    char value[16];

    ipclog_debug("%s(%d)\n", __FUNCTION__, sensitivity);
    assert(sensitivity >= 0);
    assert(sensitivity <= 100);

    /*!< save sensitivity */
    sprintf(value, "%d", sensitivity);
    mot->sensitivity = sensitivity;
 
    if (sensitivity == 0) {
        motdectStop();
    } else {
        motdectStart();
    }

    return 0;
}

int motion_controller_get(void)
{
    motdect_t *mot = &sMotdect;
    ipclog_debug("%s \n", __FUNCTION__);
    return mot->sensitivity;
}

int motion_controller_blind(int duration)
{
    motdect_t *mot = &sMotdect;
    struct timespec ts;

    ipclog_debug("%s (%d)\n", __FUNCTION__, duration);
    assert(duration > 0);

    pthread_mutex_lock(&mot->mutex);
    if (mot->inited == 0) {
        ipclog_warn("%s not inited\n", __FUNCTION__);
        pthread_mutex_unlock(&mot->mutex);
        return 0;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts);
    mot->blindExpire.tv_sec = ts.tv_sec + (duration / 1000);
    mot->blindExpire.tv_nsec = ts.tv_nsec + ((duration % 1000) * 1E6);
    if (mot->blindExpire.tv_nsec >= 1E9) {
        mot->blindExpire.tv_sec += mot->blindExpire.tv_nsec / 1E9;
        mot->blindExpire.tv_nsec %= 1000000000;
    }
    pthread_mutex_unlock(&mot->mutex);
    return 0;
}



