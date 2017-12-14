/*  Copyright 2013 Google Inc. All Rights Reserved. */

#include "basetype.h"
#include "dwl_defs.h"
#include "dwl.h"
#include "dwl_activity_trace.h"
#include "memalloc.h"
#include <assert.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>

#ifdef _DWL_DEBUG
#define DWL_DEBUG(fmt, args...) \
  printf(__FILE__ ":%d:%s() " fmt, __LINE__, __func__, ##args)
#else
#define DWL_DEBUG(fmt, args...) \
  do {                          \
  } while (0); /* not debugging: nothing */
#endif

#ifndef DEC_MODULE_PATH
#define DEC_MODULE_PATH "/dev/vdec_g2"
#endif

#ifndef MEMALLOC_MODULE_PATH
#define MEMALLOC_MODULE_PATH "/dev/memalloc"
#endif

#define HANTRODECPP_REG_START 0x400
#define HANTRODEC_REG_START 0x4

#define HANTRODECPP_FUSE_CFG 99
#define HANTRODEC_FUSE_CFG 57

#define DWL_DECODER_INT \
  ((DWLReadReg(dec_dwl, HANTRODEC_REG_START) >> 11) & 0xFFU)

#define DEC_IRQ_ABORT (1 << 11)
#define DEC_IRQ_RDY (1 << 12)
#define DEC_IRQ_BUS (1 << 13)
#define DEC_IRQ_BUFFER (1 << 14)
#define DEC_IRQ_ASO (1 << 15)
#define DEC_IRQ_ERROR (1 << 16)
#define DEC_IRQ_SLICE (1 << 17)
#define DEC_IRQ_TIMEOUT (1 << 18)

#define DWL_HW_ENABLE_BIT 0x000001 /* 0th bit */

#ifdef _DWL_HW_PERFORMANCE
/* signal that decoder/pp is enabled */
void DwlDecoderEnable(void);
#endif

struct MCListenerThreadParams {
  int fd;
  int b_stopped;
  unsigned int n_dec_cores;
  sem_t sc_dec_rdy_sem[MAX_ASIC_CORES];
  DWLIRQCallbackFn *callback[MAX_ASIC_CORES];
  void *callback_arg[MAX_ASIC_CORES];
};

/* wrapper information */
struct HX170DWL {
  u32 client_type;
  int fd;          /* decoder device file */
  int fd_mem;      /* /dev/mem for mapping */
  int fd_memalloc; /* linear memory allocator */
  u32 num_cores;
  u32 reg_size;         /* IO mem size */
  addr_t free_lin_mem;     /* Start address of free linear memory */
  addr_t free_ref_frm_mem; /* Start address of free reference frame memory */
  int semid;
  struct MCListenerThreadParams *sync_params;
  struct ActivityTrace activity;
  int mmu_enable;
 };

i32 DWLWaitDecHwReady(const void *instance, i32 core_id, u32 timeout);
u32 *DWLMapRegisters(int mem_dev, unsigned long base, unsigned int reg_size,
                     u32 write);
void DWLUnmapRegisters(const void *io, unsigned int reg_size);
