/* Copyright 2012 Google Inc. All Rights Reserved. */
/* Author: attilanagy@google.com (Atti Nagy) */

#include "basetype.h"
#include "dwl_hw_core_array.h"
#include "dwl_swhw_sync.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _DWL_DEBUG
#define DWL_DEBUG(fmt, args...) \
  fprintf(stderr, __FILE__ ":%d:%s() " fmt, __LINE__, __func__, ##args)
#else
#define DWL_DEBUG(fmt, args...) \
  do {                          \
  } while (0)
#endif

extern HwCoreArray g_hw_core_array;

void *ThreadMcListener(void *args) {
  struct McListenerThreadParams *params = (struct McListenerThreadParams *)args;

  while (!params->b_stopped) {
    u32 i, ret;

    ret = WaitAnyCoreRdy(g_hw_core_array);
    assert(ret == 0);
    (void)ret;

    if (params->b_stopped) break;

    /* check all decoder IRQ status register */
    for (i = 0; i < params->n_dec_cores; i++) {
      Core c = GetCoreById(g_hw_core_array, i);

      /* check DEC IRQ status */
      if (HwCoreIsDecRdy(c)) {
        DWL_DEBUG("DEC IRQ by Core %d\n", i);

        if (params->callback[i] != NULL) {
          params->callback[i](params->callback_arg[i], i);
        } else {
          /* single core instance => post dec ready */
          HwCorePostDecRdy(c);
        }

        break;
      }
    }

    /* check all post-processor IRQ status register */
    for (i = 0; i < params->n_ppcores; i++) {
      Core c = GetCoreById(g_hw_core_array, i);

      /* check PP IRQ status */
      if (HwCoreIsPpRdy(c)) {
        DWL_DEBUG("PP IRQ by Core %d\n", i);

        /* single core instance => post pp ready */
        HwCorePostPpRdy(c);

        break;
      }
    }
  }

  return NULL;
}
