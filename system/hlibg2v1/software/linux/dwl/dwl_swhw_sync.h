/* Copyright 2012 Google Inc. All Rights Reserved. */
/* Author: attilanagy@google.com (Atti Nagy) */

#ifndef SOFTWARE_LINUX_DWL_DWL_SWHW_SYNC_H_
#define SOFTWARE_LINUX_DWL_DWL_SWHW_SYNC_H_

#include <unistd.h>
#include <semaphore.h>

#include "dwl.h"

struct McListenerThreadParams {
  int fd;
  int b_stopped;
  unsigned int n_dec_cores;
  unsigned int n_ppcores;
  sem_t sc_dec_rdy_sem[MAX_ASIC_CORES];
  sem_t sc_pp_rdy_sem[MAX_ASIC_CORES];
  volatile u32 *reg_base[MAX_ASIC_CORES];
  DWLIRQCallbackFn *callback[MAX_ASIC_CORES];
  void *callback_arg[MAX_ASIC_CORES];
};

void *ThreadMcListener(void *args);

#endif /* SOFTWARE_LINUX_DWL_DWL_SWHW_SYNC_H_ */
