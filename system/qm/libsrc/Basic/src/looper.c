/**
  ******************************************************************************
  * @file       looper.c
  * @author     InfoTM IPC Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 InfoTM</center></h2>
  *
  * This software is confidential and proprietary and may be used only as expressly
  * authorized by a licensing agreement from InfoTM, and ALL RIGHTS RESERVED.
  * 
  * The entire notice above must be reproduced on all copies and should not be
  * removed.
  *
  ******************************************************************************
  *
  * Revision History:
  * ----------------------------------------------------------------------------
  * v0.0.1  2016/06/17  leo.zhang   First created.
  * v1.0.0  2016/06/30  leo.zhang   -
  *
  ******************************************************************************
  */


#define LOGTAG  "LP"
#include "ipc.h"
#include "looper.h"

int lp_init(looper_t *lp, char const *name, void *(*processor)(void *p), void *p)
{
    lp->request = REQ_UNKNOWN;
    lp->state = STATE_UNKNOWN;
    strncpy(lp->name, name, 15);
    pthread_cond_init(&lp->cond, NULL);
    pthread_mutex_init(&lp->mutex, NULL);
    if (pthread_create(&lp->thread, NULL/*attr*/, processor, p) != 0) {
        ipclog_ooo("create thread of %s failed\n", name);
        return -1;
    }
#if 0
    if (pthread_setname_np(lp->thread, name) != 0) {
        ipclog_ooo("pthread_setname_np() failed\n");
        return -1;
    }
#endif
    while (lp->state != STATE_IDLE) {
        usleep(2000);
    }
    ipclog_debug("looper %s inited\n", lp->name);
    return 0;
}

void lp_start(looper_t *lp, bool withlock)
{
    if (!withlock) {
        pthread_mutex_lock(&lp->mutex);
    }
    assert(lp->state != STATE_UNKNOWN);
    if (lp->state != STATE_RUN) {
        lp->request = REQ_START;
        pthread_cond_signal(&lp->cond);
#if 0
        do {
            if (lp->state == STATE_RUN) {
                ipclog_debug("looper %s started\n", lp->name);
                break;
            } else {
                pthread_mutex_unlock(&lp->mutex);
                usleep(20000);
                pthread_mutex_lock(&lp->mutex);
            }
        } while (true);
#endif
    }
    if (!withlock) {
        pthread_mutex_unlock(&lp->mutex);
    }
}

void lp_stop(looper_t *lp, bool withlock)
{
    if (!withlock) {
        pthread_mutex_lock(&lp->mutex);
    }
    assert(lp->state != STATE_UNKNOWN);
    if (lp->state == STATE_RUN) {
        lp->request = REQ_STOP;
        pthread_cond_signal(&lp->cond);
        while (lp->state != STATE_IDLE) {
            pthread_mutex_unlock(&lp->mutex);
            usleep(20000);
            pthread_mutex_lock(&lp->mutex);
        }
        ipclog_debug("looper %s stopped\n", lp->name);
    }
    if (!withlock) {
        pthread_mutex_unlock(&lp->mutex);
    }
}

void lp_deinit(looper_t *lp)
{
    assert(lp->state != STATE_UNKNOWN);

    /*!< first stop */
    lp_stop(lp, false);

    /*!< request and wait to quit */
    pthread_mutex_lock(&lp->mutex);
    lp->request = REQ_QUIT;
    pthread_cond_signal(&lp->cond);
    pthread_mutex_unlock(&lp->mutex);
    pthread_join(lp->thread, NULL);
    ipclog_debug("looper %s deinited\n", lp->name);
}

int lp_wait_request_l(looper_t *lp)
{
    int req = REQ_UNKNOWN;

    if (lp->request == REQ_UNKNOWN) {
        ipclog_debug("looper %s wait request\n", lp->name);
        pthread_cond_wait(&lp->cond, &lp->mutex);
        if (lp->request == REQ_QUIT) {
            ipclog_debug("looper %s get REQ_QUIT\n", lp->name);
            req = lp->request;
            lp->state = STATE_UNKNOWN;
            lp->request = REQ_UNKNOWN;
        } else if (lp->request == REQ_START) {
            ipclog_debug("looper %s get REQ_START\n", lp->name);
            req = lp->request;
            lp->state = STATE_RUN;
            lp->request = REQ_UNKNOWN;
        } else if (lp->request == REQ_STOP) {
            ipclog_debug("looper %s get REQ_STOP\n", lp->name);
            req = lp->request;
            lp->state = STATE_IDLE;
            lp->request = REQ_UNKNOWN;
        } else {
            assert(false);
        }
    } else {
        req = lp->request;
        lp->request = REQ_UNKNOWN;
    }
    return req;
}

int lp_check_request_l(looper_t *lp)
{
    int req = lp->request;
    if (req == REQ_START) {
        lp->state = STATE_RUN;
        lp->request = REQ_UNKNOWN;
    } else if (req == REQ_STOP) {
        lp->state = STATE_IDLE;
        lp->request = REQ_UNKNOWN;
    } else if (req == REQ_QUIT) {
        lp->state = STATE_IDLE;
        lp->request = REQ_UNKNOWN;
    }
    return req;
}

