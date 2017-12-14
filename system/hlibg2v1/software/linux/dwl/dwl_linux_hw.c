/*  Copyright 2013 Google Inc. All Rights Reserved. */

#include "basetype.h"
#include "dwl_linux.h"
#include "dwl.h"
#include "hantrodec.h"
#include "memalloc.h"

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

#ifdef INTERNAL_TEST
#include "internal_test.h"
#endif

u32 instances = 0;

/* the decoder device driver nod */
static const char *dec_dev = DEC_MODULE_PATH;

/* the memalloc device driver nod */
const char *mem_dev = MEMALLOC_MODULE_PATH;

/* counters for core usage statistics */
u32 core_usage_counts[MAX_ASIC_CORES] = {0};

/* a mutex protecting the wrapper init */
static pthread_mutex_t x170_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static int n_dwl_instance_count = 0;

static struct MCListenerThreadParams listener_thread_params = {0};
static pthread_t mc_listener_thread;

extern u32 dwl_shadow_regs[MAX_ASIC_CORES][264];

void *ThreadMCListener(void *args) {
  struct MCListenerThreadParams *params = (struct MCListenerThreadParams *)args;

  while (!params->b_stopped) {
    u32 id;
    struct core_desc core;

#ifdef DWL_USE_DEC_IRQ

 
    DWL_DEBUG("ioctl wait for interrupt\n");
    if (ioctl(params->fd, HANTRODEC_IOCG_CORE_WAIT, &id)) {
      DWL_DEBUG("ioctl HANTRODEC_IOCG_CORE_WAIT failed\n");
    }

    if (params->b_stopped) break;

    DWL_DEBUG("DEC IRQ by Core %d\n", id);

    core.id = id;
    core.regs = dwl_shadow_regs[id];
    core.size = 265 * 4;

    if (ioctl(params->fd, HANTRODEC_IOCS_DEC_PULL_REG, &core)) {
      DWL_DEBUG("ioctl HANTRODEC_IOCS_DEC_PULL_REG failed\n");
      assert(0);
    }

    if (params->callback[id] != NULL) {
      params->callback[id](params->callback_arg[id], id);
    } else {
      DWL_DEBUG("SINGLE CORE IRQ, core = %d\n", id);
      sem_post(params->sc_dec_rdy_sem + id);
    }
#else
    core.size = 2 * 4;

    for (id = 0; id < params->n_dec_cores; id++) {
      u32 irq_stats;
      const unsigned int usec = 1000; /* 1 ms polling interval */
      usleep(usec);

      /* Skip cores that are not part of multicore, they call directly
       * DWLWaitHwReady(), which does its own polling.
       */
      if (params->callback[id] == NULL) {
        continue;
      }

      /* Skip not enabled cores also */
      if ((dwl_shadow_regs[id][1] & 0x01) == 0) {
        continue;
      }

      core.id = id;
      core.regs = dwl_shadow_regs[id];

      if (ioctl(params->fd, HANTRODEC_IOCS_DEC_PULL_REG, &core)) {
        DWL_DEBUG("ioctl HANTRODEC_IOCS_DEC_PULL_REG failed\n");
        continue;
      }

      irq_stats = dwl_shadow_regs[id][1];

      irq_stats = (irq_stats >> 11) & 0xFF;

      if (irq_stats != 0) {
        DWL_DEBUG("DEC IRQ by Core %d\n", id);
        params->callback[id](params->callback_arg[id], id);
      }

      usleep(usec);
    }
#endif
  }

  return NULL;
}

/*------------------------------------------------------------------------------
    Function name   : DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : void * param - not in use, application passes NULL
------------------------------------------------------------------------------*/
const void *DWLInit(struct DWLInitParam *param) {
  struct HX170DWL *dec_dwl;
  unsigned long multicore_base[MAX_ASIC_CORES];
  unsigned int i;

  DWL_DEBUG("INITIALIZE\n");

  dec_dwl = (struct HX170DWL *)calloc(1, sizeof(struct HX170DWL));

  if (dec_dwl == NULL) {
    DWL_DEBUG("failed to alloc struct HX170DWL struct\n");
    return NULL;
  }

  dec_dwl->client_type = param->client_type;

  pthread_mutex_lock(&x170_init_mutex);

#ifdef INTERNAL_TEST
  InternalTestInit();
#endif

  dec_dwl->fd = -1;
  dec_dwl->fd_mem = -1;
  dec_dwl->fd_memalloc = -1;

  /* open the device */
  dec_dwl->fd = open(dec_dev, O_RDWR);
  if (dec_dwl->fd == -1) {
    DWL_DEBUG("failed to open '%s'\n", dec_dev);
    goto err;
  }

#if 0
  /* Linear memories not needed in pp */
  if (dec_dwl->client_type != DWL_CLIENT_TYPE_PP) {
    /* open memalloc for linear memory allocation */
    dec_dwl->fd_memalloc = open(mem_dev, O_RDWR | O_SYNC);

    if (dec_dwl->fd_memalloc == -1) {
      DWL_DEBUG("failed to open: %s\n", mem_dev);
      goto err;
    }
  }
#endif

  dec_dwl->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);

  if (dec_dwl->fd_mem == -1) {
    DWL_DEBUG("failed to open: %s\n", "/dev/mem");
    goto err;
  }

  switch (dec_dwl->client_type) {
    case DWL_CLIENT_TYPE_HEVC_DEC:
    case DWL_CLIENT_TYPE_VP9_DEC:
    case DWL_CLIENT_TYPE_PP: { break; }
    default: {
      DWL_DEBUG("Unknown client type no. %d\n", dec_dwl->client_type);
      goto err;
    }
  }

  if (ioctl(dec_dwl->fd, HANTRODEC_IOC_MC_CORES, &dec_dwl->num_cores) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOC_MC_CORES failed\n");
    goto err;
  }

  assert(dec_dwl->num_cores <= MAX_ASIC_CORES);

  if (ioctl(dec_dwl->fd, HANTRODEC_IOC_MC_OFFSETS, multicore_base) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOC_MC_OFFSETS failed\n");
    goto err;
  }

  if (ioctl(dec_dwl->fd, HANTRODEC_IOCGHWIOSIZE, &dec_dwl->reg_size) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOCGHWIOSIZE failed\n");
    goto err;
  }

  /* Allocate the signal handling and cores just once */
  if (n_dwl_instance_count++ == 0) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    listener_thread_params.fd = dec_dwl->fd;
    listener_thread_params.n_dec_cores = dec_dwl->num_cores;

    for (i = 0; i < listener_thread_params.n_dec_cores; i++) {
      sem_init(listener_thread_params.sc_dec_rdy_sem + i, 0, 0);

      listener_thread_params.callback[i] = NULL;
    }

    listener_thread_params.b_stopped = 0;

    if (pthread_create(&mc_listener_thread, &attr, ThreadMCListener,
                       &listener_thread_params) != 0)
      goto err;
  }

  ActivityTraceInit(&dec_dwl->activity);
  dec_dwl->sync_params = &listener_thread_params;

  DWL_DEBUG("SUCCESS\n");

  pthread_mutex_unlock(&x170_init_mutex);
  return dec_dwl;

err:

  DWL_DEBUG("FAILED\n");
  pthread_mutex_unlock(&x170_init_mutex);
  DWLRelease(dec_dwl);

  return NULL;
}

/*------------------------------------------------------------------------------
    Function name   : DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 DWLRelease(const void *instance) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;
  unsigned int i = 0;

  DWL_DEBUG("RELEASE\n");

  assert(dec_dwl != NULL);

  if (dec_dwl == NULL) return DWL_OK;

  pthread_mutex_lock(&x170_init_mutex);

  n_dwl_instance_count--;

  /* Release the signal handling and cores just when
   * nobody is referencing them anymore
   */
  if (!n_dwl_instance_count) {
    listener_thread_params.b_stopped = 1;
  }

  for (i = 0; i < dec_dwl->num_cores; i++) {
    sem_destroy(listener_thread_params.sc_dec_rdy_sem + i);
  }

  if (dec_dwl->fd_mem != -1) close(dec_dwl->fd_mem);

  if (dec_dwl->fd != -1) close(dec_dwl->fd);

  /* linear memory allocator */
  //if (dec_dwl->fd_memalloc != -1) close(dec_dwl->fd_memalloc);

  /* print core usage stats */
  if (dec_dwl->client_type != DWL_CLIENT_TYPE_PP) {
    u32 total_usage = 0;
    u32 cores = dec_dwl->num_cores;
    for (i = 0; i < cores; i++) total_usage += core_usage_counts[i];
    /* avoid zero division */
    total_usage = total_usage ? total_usage : 1;

    printf("\nMulti-core usage statistics:\n");
    for (i = 0; i < cores; i++)
      printf("\tCore[%2d] used %6d times (%2d%%)\n", i, core_usage_counts[i],
             (core_usage_counts[i] * 100) / total_usage);

    printf("\n");
  }

#ifdef INTERNAL_TEST
  InternalTestFinalize();
#endif

  ActivityTraceRelease(&dec_dwl->activity);
  free(dec_dwl);

  pthread_mutex_unlock(&x170_init_mutex);

  DWL_DEBUG("SUCCESS\n");

  return (DWL_OK);
}

/* HW locking */

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHwPipe
    Description     :
    Return type     : i32
    Argument        : const void *instance
    Argument        : i32 *core_id - ID of the reserved HW core
------------------------------------------------------------------------------*/
i32 DWLReserveHwPipe(const void *instance, i32 *core_id) {
  i32 ret;
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  assert(dec_dwl != NULL);
  assert(dec_dwl->client_type != DWL_CLIENT_TYPE_PP);

  DWL_DEBUG("Start\n");

  /* reserve decoder */
  *core_id =
      ioctl(dec_dwl->fd, HANTRODEC_IOCH_DEC_RESERVE, dec_dwl->client_type);

  if (*core_id != 0) {
    return DWL_ERROR;
  }

  core_usage_counts[*core_id]++;

  DWL_DEBUG("Reserved DEC core %d\n", *core_id);

  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
    Argument        : i32 *core_id - ID of the reserved HW core
------------------------------------------------------------------------------*/
i32 DWLReserveHw(const void *instance, i32 *core_id) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  assert(dec_dwl != NULL);

  DWL_DEBUG(" %s\n", "DEC");

  *core_id =
        ioctl(dec_dwl->fd, HANTRODEC_IOCH_DEC_RESERVE, dec_dwl->client_type);


  /* negative value signals an error */
  if (*core_id < 0) {
    DWL_DEBUG("ioctl HANTRODEC_IOCS_%s_reserve failed, %d\n",
              "DEC", *core_id);
    return DWL_ERROR;
  }

  core_usage_counts[*core_id]++;

  DWL_DEBUG("Reserved %s core %d\n", "DEC", *core_id);

  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReleaseHw
    Description     :
    Return type     : void
    Argument        : const void *instance
------------------------------------------------------------------------------*/
void DWLReleaseHw(const void *instance, i32 core_id) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  assert((u32)core_id < dec_dwl->num_cores);
  assert(dec_dwl != NULL);

  if ((u32)core_id >= dec_dwl->num_cores) {
    assert(0);
    return;
  }

  DWL_DEBUG(" %s core %d\n", "DEC", core_id);

  ioctl(dec_dwl->fd, HANTRODEC_IOCT_DEC_RELEASE, core_id);
}

void DWLSetIRQCallback(const void *instance, i32 core_id,
                       DWLIRQCallbackFn *callback_fn, void *arg) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  dec_dwl->sync_params->callback[core_id] = callback_fn;
  dec_dwl->sync_params->callback_arg[core_id] = arg;
}
