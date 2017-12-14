/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#include "asic.h"
#include "dwl_hw_core.h"

#include <assert.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

struct HwCore {
  const void* hw_core;
  int id;
  int reserved;
  sem_t hw_enable;
  sem_t* mc_hw_rdy;
  sem_t dec_rdy;
  sem_t pp_rdy;
  int b_dec_ena;
  int b_dec_rdy;
  int b_pp_ena;
  int b_pp_rdy;
  pthread_t core_thread;
  int b_stopped;
};

static void* HwCoreRunAsic(void* param);

Core HwCoreInit(void) {
  pthread_attr_t attr;
  struct HwCore* core = NULL;

  core = calloc(1, sizeof(struct HwCore));

  if (core == NULL) return NULL;

  core->hw_core = AsicHwCoreInit();
  assert(core->hw_core);

  sem_init(&core->hw_enable, 0, 0);
  sem_init(&core->dec_rdy, 0, 0);
  sem_init(&core->pp_rdy, 0, 0);

  core->b_dec_ena = core->b_pp_ena = 0;
  core->b_dec_rdy = core->b_pp_rdy = 0;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  core->b_stopped = 0;

  pthread_create(&core->core_thread, &attr, HwCoreRunAsic, (void*)core);

  pthread_attr_destroy(&attr);

  return core;
}

void HwCoreRelease(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  core->b_stopped = 1;

  sem_post(&core->hw_enable);

  pthread_join(core->core_thread, NULL);

  AsicHwCoreRelease((void*)core->hw_core);

  sem_destroy(&core->hw_enable);
  sem_destroy(&core->dec_rdy);
  sem_destroy(&core->pp_rdy);

  free(core);
}

u32* HwCoreGetBaseAddress(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;
  return AsicHwCoreGetBase((void*)core->hw_core);
}

void HwCoreDecEnable(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;
  core->b_dec_ena = 1;
  sem_post(&core->hw_enable);
}

void HwCorePpEnable(Core instance, int start) {
  struct HwCore* core = (struct HwCore*)instance;

  volatile u32* reg = AsicHwCoreGetBase((void*)core->hw_core);

  /* check standalone pp enabled */
  if (reg[60] & 0x02) {
    /* dec+pp pipeline enabled, we only wait for dec ready */
    fprintf(stderr, "\n_pipeline\n");
  } else {
    core->b_pp_ena = 1;
  }

  /* if standalone PP then start hw core */
  if (start && core->b_pp_ena) sem_post(&core->hw_enable);
}

int HwCoreWaitDecRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  return sem_wait(&core->dec_rdy);
}

int HwCoreIsDecRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  int rdy = core->b_dec_rdy;

  if (core->b_dec_rdy) core->b_dec_rdy = 0;

  return rdy;
}

int HwCoreIsPpRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;
  int rdy = core->b_pp_rdy;

  if (core->b_pp_rdy) core->b_pp_rdy = 0;

  return rdy;
}

int HwCoreWaitPpRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  return sem_wait(&core->pp_rdy);
}

int HwCorePostDecRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  return sem_post(&core->dec_rdy);
}

int HwCorePostPpRdy(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;

  return sem_post(&core->pp_rdy);
}

void HwCoreSetid(Core instance, int id) {
  struct HwCore* core = (struct HwCore*)instance;
  core->id = id;
}

int HwCoreGetid(Core instance) {
  struct HwCore* core = (struct HwCore*)instance;
  return core->id;
}

void HwCoreSetHwRdySem(Core instance, sem_t* rdy) {
  struct HwCore* core = (struct HwCore*)instance;
  core->mc_hw_rdy = rdy;
}

static pthread_mutex_t core_stat_lock = PTHREAD_MUTEX_INITIALIZER;

int HwCoreTryLock(Core inst) {
  struct HwCore* core = (struct HwCore*)inst;
  int success = 0;

  pthread_mutex_lock(&core_stat_lock);
  if (!core->reserved) {
    core->reserved = 1;
    success = 1;
  }
  pthread_mutex_unlock(&core_stat_lock);

  return success;
}

void HwCoreUnlock(Core inst) {
  struct HwCore* core = (struct HwCore*)inst;

  pthread_mutex_lock(&core_stat_lock);
  core->reserved = 0;
  pthread_mutex_unlock(&core_stat_lock);
}

void* HwCoreRunAsic(void* param) {
  struct HwCore* core = (struct HwCore*)param;

  volatile u32* reg = AsicHwCoreGetBase((void*)core->hw_core);

  while (!core->b_stopped) {
    while (sem_wait(&core->hw_enable) != 0 && errno == EINTR) {
    }

    if (core->b_stopped) /* thread terminating */
      break;

    AsicRun((void*)core->hw_core);

    /* clear IRQ line; in real life this is done in the ISR! */
    reg[1] &= ~(1 << 8);  /* decoder IRQ */
    reg[60] &= ~(1 << 8); /* PP IRQ */

    if (core->b_dec_ena) core->b_dec_rdy = 1;
    if (core->b_pp_ena) core->b_pp_rdy = 1;

    core->b_dec_ena = 0;
    core->b_pp_ena = 0;

    sem_post(core->mc_hw_rdy);
  }

  return NULL;
}
