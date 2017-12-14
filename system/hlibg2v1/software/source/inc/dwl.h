/*  Copyright 2013 Google Inc. All Rights Reserved. */

/* Decoder Wrapper Layer for operating system services. */

#ifndef __DWL_H__
#define __DWL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "basetype.h"
#include "decapicommon.h"

#ifdef __linux__
#include <pthread.h>
#else
#error "No pthread defined for the system, define prototypes."
#endif /* HAVE_PTHREAD_H */

#define DWL_OK 0
#define DWL_ERROR -1

#define DWL_HW_WAIT_OK DWL_OK
#define DWL_HW_WAIT_ERROR DWL_ERROR
#define DWL_HW_WAIT_TIMEOUT 1

#define DWL_CLIENT_TYPE_PP 4U
#define DWL_CLIENT_TYPE_VP9_DEC 11U
#define DWL_CLIENT_TYPE_HEVC_DEC 12U

#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define EXPORT  __declspec(dllexport)
#endif

/* Linear memory area descriptor */
struct DWLLinearMem {
  u32 *virtual_address;
  addr_t bus_address;
  u32 size;         /* physical size (rounded to page multiple) */
  u32 logical_size; /* requested size in bytes */
  u32 uid;
};

/* DWLInitParam is used to pass parameters when initializing the DWL */
struct DWLInitParam {
  u32 client_type;
};

/* Hardware configuration description, same as in top API */
typedef struct DecHwConfig DWLHwConfig;

struct DWLHwFuseStatus {
  u32 vp6_support_fuse;            /* HW supports VP6 */
  u32 vp7_support_fuse;            /* HW supports VP7 */
  u32 vp8_support_fuse;            /* HW supports VP8 */
  u32 vp9_support_fuse;            /* HW supports VP9 */
  u32 h264_support_fuse;           /* HW supports H.264 */
  u32 HevcSupportFuse;             /* HW supports HEVC */
  u32 mpeg4_support_fuse;          /* HW supports MPEG-4 */
  u32 mpeg2_support_fuse;          /* HW supports MPEG-2 */
  u32 sorenson_spark_support_fuse; /* HW supports Sorenson Spark */
  u32 jpeg_support_fuse;           /* HW supports JPEG */
  u32 vc1_support_fuse;            /* HW supports VC-1 Simple */
  u32 jpeg_prog_support_fuse;      /* HW supports Progressive JPEG */
  u32 pp_support_fuse;             /* HW supports post-processor */
  u32 pp_config_fuse;              /* HW post-processor functions bitmask */
  u32 max_dec_pic_width_fuse;      /* Maximum video decoding width supported  */
  u32 max_pp_out_pic_width_fuse;   /* Maximum output width of Post-Processor */
  u32 ref_buf_support_fuse;        /* HW supports reference picture buffering */
  u32 avs_support_fuse;            /* HW supports AVS */
  u32 rv_support_fuse;             /* HW supports RealVideo codec */
  u32 mvc_support_fuse;            /* HW supports MVC */
  u32 custom_mpeg4_support_fuse;   /* Fuse for custom MPEG-4 */
};

/* HW ID retrieving, static implementation */
u32 DWLReadAsicID(void);

/* HW configuration retrieving, static implementation */
void DWLReadAsicConfig(DWLHwConfig *hw_cfg);
void DWLReadMCAsicConfig(DWLHwConfig hw_cfg[MAX_ASIC_CORES]);

/* Return number of ASIC cores, static implementation */
u32 DWLReadAsicCoreCount(void);

/* HW fuse retrieving, static implementation */
void DWLReadAsicFuseStatus(struct DWLHwFuseStatus *hw_fuse_sts);

/* struct struct struct DWL initialization and release */
EXPORT const void *DWLInit(struct DWLInitParam *param);
EXPORT i32 DWLRelease(const void *instance);

/* HW sharing */
i32 DWLReserveHw(const void *instance, i32 *core_id);
i32 DWLReserveHwPipe(const void *instance, i32 *core_id);
void DWLReleaseHw(const void *instance, i32 core_id);

/* Frame buffers memory */
i32 DWLMallocRefFrm(const void *instance, u32 size, struct DWLLinearMem *info);
void DWLFreeRefFrm(const void *instance, struct DWLLinearMem *info);

/* SW/HW shared memory */
i32 DWLMallocLinear(const void *instance, u32 size, struct DWLLinearMem *info);
void DWLFreeLinear(const void *instance, struct DWLLinearMem *info);

/* Register access */
void DWLWriteReg(const void *instance, i32 core_id, u32 offset, u32 value);
u32 DWLReadReg(const void *instance, i32 core_id, u32 offset);

/* HW starting/stopping */
void DWLEnableHw(const void *instance, i32 core_id, u32 offset, u32 value);
void DWLDisableHw(const void *instance, i32 core_id, u32 offset, u32 value);

/* HW synchronization */
i32 DWLWaitHwReady(const void *instance, i32 core_id, u32 timeout);

typedef void DWLIRQCallbackFn(void *arg, i32 core_id);

void DWLSetIRQCallback(const void *instance, i32 core_id,
                       DWLIRQCallbackFn *callback_fn, void *arg);

/* SW/SW shared memory */
void *DWLmalloc(u32 n);
void DWLfree(void *p);
void *DWLcalloc(u32 n, u32 s);
void *DWLmemcpy(void *d, const void *s, u32 n);
void *DWLmemset(void *d, i32 c, u32 n);

/* SW/HW shared memory access*/
u8 DWLPrivateAreaReadByte(const u8 *p);
void DWLPrivateAreaWriteByte(u8 *p, u8 data);
void * DWLPrivateAreaMemcpy(void *d,  const void *s,  u32 n);
void * DWLPrivateAreaMemset(void *p,  i32 c, u32 n);
/* Decoder wrapper layer functionality. */
struct DWL {
  /* HW sharing */
  i32 (*ReserveHw)(const void *instance, i32 *core_id);
  i32 (*ReserveHwPipe)(const void *instance, i32 *core_id);
  void (*ReleaseHw)(const void *instance, i32 core_id);
  /* Physical, linear memory functions */
  i32 (*MallocLinear)(const void *instance, u32 size,
                      struct DWLLinearMem *info);
  void (*FreeLinear)(const void *instance, struct DWLLinearMem *info);
  /* Register access */
  void (*WriteReg)(const void *instance, i32 core_id, u32 offset, u32 value);
  u32 (*ReadReg)(const void *instance, i32 core_id, u32 offset);
  /* HW starting/stopping */
  void (*EnableHw)(const void *instance, i32 core_id, u32 offset, u32 value);
  void (*DisableHw)(const void *instance, i32 core_id, u32 offset, u32 value);
  /* HW synchronization */
  i32 (*WaitHwReady)(const void *instance, i32 core_id, u32 timeout);
  void (*SetIRQCallback)(const void *instance, i32 core_id,
                         DWLIRQCallbackFn *callback_fn, void *arg);
  /* Virtual memory functions. */
  void *(*malloc)(u32 n);
  void (*free)(void *p);
  void *(*calloc)(u32 n, u32 s);
  void *(*memcpy)(void *d, const void *s, u32 n);
  void *(*memset)(void *d, i32 c, u32 n);

  /* POSIX compatible threading functions. */
  i32 (*pthread_create)(pthread_t *tid, const pthread_attr_t *attr,
                        void *(*start)(void *), void *arg);
  void (*pthread_exit)(void *value_ptr);
  i32 (*pthread_join)(pthread_t thread, void **value_ptr);
  i32 (*pthread_mutex_init)(pthread_mutex_t *mutex,
                            const pthread_mutexattr_t *attr);
  i32 (*pthread_mutex_destroy)(pthread_mutex_t *mutex);
  i32 (*pthread_mutex_lock)(pthread_mutex_t *mutex);
  i32 (*pthread_mutex_unlock)(pthread_mutex_t *mutex);
  i32 (*pthread_cond_init)(pthread_cond_t *cond,
                           const pthread_condattr_t *attr);
  i32 (*pthread_cond_destroy)(pthread_cond_t *cond);
  i32 (*pthread_cond_wait)(pthread_cond_t *cond, pthread_mutex_t *mutex);
  i32 (*pthread_cond_signal)(pthread_cond_t *cond);
  /* API trace function. Set to NULL if no trace wanted. */
  int (*printf)(const char *string, ...);
};

#ifdef __cplusplus
}
#endif

#endif /* __DWL_H__ */
