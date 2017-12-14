/**
  ******************************************************************************
  * @file       looper.h
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


#ifndef __LOOPER_H__
#define __LOOPER_H__

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <pthread.h>

enum {
    STATE_UNKNOWN,
    STATE_IDLE,
    STATE_RUN
};

enum {
    REQ_UNKNOWN = 0,
    REQ_START,
    REQ_STOP,
    REQ_QUIT
};

typedef struct {
    char name[16];
    pthread_t thread;
    int request;
    int state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} looper_t;

#define lp_get_state(lp)        (lp)->state
#define lp_set_state(lp, v)     (lp)->state = v
#define lp_get_req(lp)          (lp)->request
#define lp_set_req(lp, req)     (lp)->request = req
#define lp_lock(lp)             pthread_mutex_lock(&(lp)->mutex)
#define lp_unlock(lp)           pthread_mutex_unlock(&(lp)->mutex)
#define lp_signal(lp)           pthread_cond_signal(&(lp)->cond)
#define lp_wait(lp)             pthread_cond_wait(&(lp)->cond, NULL)
#define lp_wait_l(lp)           pthread_cond_wait(&(lp)->cond, &(lp)->mutex)
#define lp_timedwait(lp, t)     pthread_cond_timedwait(&(lp)->cond, NULL, t)
#define lp_timedwait_l(lp, t)   pthread_cond_timedwait(&(lp)->cond, &(lp)->mutex, t)

int lp_init(looper_t *lp, char const *name, void *(*processor)(void *p), void *p);
void lp_start(looper_t *lp, bool withlock);
void lp_stop(looper_t *lp, bool withlock);
void lp_deinit(looper_t *lp);
int lp_wait_request_l(looper_t *lp);
int lp_check_request_l(looper_t *lp);


#endif  // __LOOPER_H__

