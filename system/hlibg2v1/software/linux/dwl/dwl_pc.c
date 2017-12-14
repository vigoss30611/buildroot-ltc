/* Copyright 2012 Google Inc. All Rights Reserved. */
/* Author: attilanagy@google.com (Atti Nagy) */

#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "basetype.h"
#include "dwl.h"
#include "dwl_activity_trace.h"
#include "dwl_hw_core_array.h"
#include "dwl_swhw_sync.h"
#include "sw_util.h"

#include "../../test/common/swhw/tb_cfg.h"

#ifdef INTERNAL_TEST
#include "internal_test.h"
#endif

#ifdef _DWL_DEBUG
#define DWL_DEBUG(fmt, args...) \
  fprintf(stderr, __FILE__ ":%d:%s() " fmt, __LINE__, __func__, ##args)
#else
#define DWL_DEBUG(fmt, args...) \
  do {                          \
  } while (0)
#endif

/* Constants related to registers */
#define DEC_X170_REGS 264
#define PP_FIRST_REG 60
#define DEC_STREAM_START_REG 12

#define IS_PIPELINE_ENABLED(val) ((val) & 0x02)

#ifdef DWL_PRESET_FAILING_ALLOC
#define FAIL_DURING_ALLOC DWL_PRESET_FAILING_ALLOC
#endif

extern struct TBCfg tb_cfg;
extern u32 g_hw_ver;
extern u32 h264_high_support;

#ifdef ASIC_TRACE_SUPPORT
extern u8 *dpb_base_address;
#endif
static i32 DWLTestRandomFail(void);

/* counters for core usage statistics */
u32 core_usage_counts[MAX_ASIC_CORES] = {0};

struct DWLInstance {
  u32 client_type;
  u8 *free_ref_frm_mem;
  u8 *frm_base;

  u32 b_reserved_pipe;

  /* Keep track of allocated memories */
  u32 reference_total;
  u32 reference_alloc_count;
  u32 reference_maximum;
  u32 linear_total;
  i32 linear_alloc_count;

  HwCoreArray hw_core_array;
  /* TODO(vmr): Get rid of temporary core "memory" mechanism. */
  Core current_core;
  struct ActivityTrace activity;
};

HwCoreArray g_hw_core_array;
struct McListenerThreadParams listener_thread_params;
pthread_t mc_listener_thread;

#ifdef _DWL_PERFORMANCE
u32 reference_total_max = 0;
u32 linear_total_max = 0;
u32 malloc_total_max = 0;
#endif

#ifdef FAIL_DURING_ALLOC
u32 failed_alloc_count = 0;
#endif

/* a mutex protecting the wrapper init */
pthread_mutex_t dwl_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static int n_dwl_instance_count = 0;

/* single core PP mutex */
static pthread_mutex_t pp_mutex = PTHREAD_MUTEX_INITIALIZER;

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicID
    Description     : Read the HW ID. Does not need a DWL instance to run

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 DWLReadAsicID() {
  u32 build = 0;

  /* Set HW info from TB config */

  g_hw_ver =
      tb_cfg.dec_params.hw_version ? tb_cfg.dec_params.hw_version : 10001;

  build = tb_cfg.dec_params.hw_build;

  build = (build / 1000) * 0x1000 + ((build / 100) % 10) * 0x100 +
          ((build / 10) % 10) * 0x10 + ((build) % 10) * 0x1;

  switch (g_hw_ver) {
    /* Currenty only G2 supported */
    default:
      return 0x67320000 + build;
  }
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicConfig
    Description     : Read HW configuration. Does not need a DWL instance to run

    Returns     : DWLHwConfig *hw_cfg - structure with HW configuration
------------------------------------------------------------------------------*/
void DWLReadAsicConfig(DWLHwConfig *hw_cfg) {
  assert(hw_cfg != NULL);

  /* Decoder configuration */
  memset(hw_cfg, 0, sizeof(*hw_cfg));

  TBGetHwConfig(&tb_cfg, hw_cfg);
  /* TODO(vmr): find better way to do this. */
  hw_cfg->vp9_support = 1;
  hw_cfg->hevc_support = 1;

  /* if HW config (or tb.cfg here) says that SupportH264="10" -> keep it
   * like that. This does not mean MAIN profile support but is used to
   * cut down bus load by disabling direct mv writing in certain
   * applications */
  if (hw_cfg->h264_support && hw_cfg->h264_support != 2) {
    hw_cfg->h264_support =
        h264_high_support ? H264_HIGH_PROFILE : H264_BASELINE_PROFILE;
  }

  /* Apply fuse limitations */
  {
    u32 de_interlace;
    u32 alpha_blend;
    u32 de_interlace_fuse;
    u32 alpha_blend_fuse;

    struct DWLHwFuseStatus hw_fuse_sts;
    /* check fuse status */
    DWLReadAsicFuseStatus(&hw_fuse_sts);

    /* Maximum decoding width supported by the HW */
    if (hw_cfg->max_dec_pic_width > hw_fuse_sts.max_dec_pic_width_fuse)
      hw_cfg->max_dec_pic_width = hw_fuse_sts.max_dec_pic_width_fuse;
    /* Maximum output width of Post-Processor */
    if (hw_cfg->max_pp_out_pic_width > hw_fuse_sts.max_pp_out_pic_width_fuse)
      hw_cfg->max_pp_out_pic_width = hw_fuse_sts.max_pp_out_pic_width_fuse;
    /* h264 */
    if (!hw_fuse_sts.h264_support_fuse)
      hw_cfg->h264_support = H264_NOT_SUPPORTED;
    /* mpeg-4 */
    if (!hw_fuse_sts.mpeg4_support_fuse)
      hw_cfg->mpeg4_support = MPEG4_NOT_SUPPORTED;
    /* jpeg (baseline && progressive) */
    if (!hw_fuse_sts.jpeg_support_fuse)
      hw_cfg->jpeg_support = JPEG_NOT_SUPPORTED;
    if (hw_cfg->jpeg_support == JPEG_PROGRESSIVE &&
        !hw_fuse_sts.jpeg_prog_support_fuse)
      hw_cfg->jpeg_support = JPEG_BASELINE;
    /* mpeg-2 */
    if (!hw_fuse_sts.mpeg2_support_fuse)
      hw_cfg->mpeg2_support = MPEG2_NOT_SUPPORTED;
    /* vc-1 */
    if (!hw_fuse_sts.vc1_support_fuse) hw_cfg->vc1_support = VC1_NOT_SUPPORTED;
    /* vp6 */
    if (!hw_fuse_sts.vp6_support_fuse) hw_cfg->vp6_support = VP6_NOT_SUPPORTED;
    /* vp7 */
    if (!hw_fuse_sts.vp7_support_fuse) hw_cfg->vp7_support = VP7_NOT_SUPPORTED;
    /* vp8 */
    if (!hw_fuse_sts.vp8_support_fuse) hw_cfg->vp8_support = VP8_NOT_SUPPORTED;
    /* webp */
    if (!hw_fuse_sts.vp8_support_fuse)
      hw_cfg->webp_support = WEBP_NOT_SUPPORTED;
    /* avs */
    if (!hw_fuse_sts.avs_support_fuse) hw_cfg->avs_support = AVS_NOT_SUPPORTED;
    /* rv */
    if (!hw_fuse_sts.rv_support_fuse) hw_cfg->rv_support = RV_NOT_SUPPORTED;
    /* mvc */
    if (!hw_fuse_sts.mvc_support_fuse) hw_cfg->mvc_support = MVC_NOT_SUPPORTED;
    /* pp */
    if (!hw_fuse_sts.pp_support_fuse) hw_cfg->pp_support = PP_NOT_SUPPORTED;
    /* check the pp config vs fuse status */
    if ((hw_cfg->pp_config & 0xFC000000) &&
        ((hw_fuse_sts.pp_config_fuse & 0xF0000000) >> 5)) {
      /* config */
      de_interlace = ((hw_cfg->pp_config & PP_DEINTERLACING) >> 25);
      alpha_blend = ((hw_cfg->pp_config & PP_ALPHA_BLENDING) >> 24);
      /* fuse */
      de_interlace_fuse =
          (((hw_fuse_sts.pp_config_fuse >> 5) & PP_DEINTERLACING) >> 25);
      alpha_blend_fuse =
          (((hw_fuse_sts.pp_config_fuse >> 5) & PP_ALPHA_BLENDING) >> 24);

      /* check fuse */
      if (de_interlace && !de_interlace_fuse) hw_cfg->pp_config &= 0xFD000000;
      if (alpha_blend && !alpha_blend_fuse) hw_cfg->pp_config &= 0xFE000000;
    }
    /* sorenson */
    if (!hw_fuse_sts.sorenson_spark_support_fuse)
      hw_cfg->sorenson_spark_support = SORENSON_SPARK_NOT_SUPPORTED;
    /* ref. picture buffer */
    if (!hw_fuse_sts.ref_buf_support_fuse)
      hw_cfg->ref_buf_support = REF_BUF_NOT_SUPPORTED;
  }
}

void DWLReadMCAsicConfig(DWLHwConfig hw_cfg[MAX_ASIC_CORES]) {
  u32 i, cores = DWLReadAsicCoreCount();
  assert(cores <= MAX_ASIC_CORES);

  /* read core 0  cfg */
  DWLReadAsicConfig(hw_cfg);

  /* ... and replicate first core cfg to all other cores */
  for (i = 1; i < cores; i++)
    DWLmemcpy(hw_cfg + i, hw_cfg, sizeof(DWLHwConfig));
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicFuseStatus
    Description     : Read HW fuse configuration.
                      Does not need a DWL instance to run

    Returns         : *hw_fuse_sts - structure with HW fuse configuration
------------------------------------------------------------------------------*/
void DWLReadAsicFuseStatus(struct DWLHwFuseStatus *hw_fuse_sts) {
  assert(hw_fuse_sts != NULL);
  /* Maximum decoding width supported by the HW (fuse) */
  hw_fuse_sts->max_dec_pic_width_fuse = 4096;
  /* Maximum output width of Post-Processor (fuse) */
  hw_fuse_sts->max_pp_out_pic_width_fuse = 4096;

  hw_fuse_sts->h264_support_fuse = H264_FUSE_ENABLED;   /* HW supports H264 */
  hw_fuse_sts->mpeg4_support_fuse = MPEG4_FUSE_ENABLED; /* HW supports MPEG-4 */
  /* HW supports MPEG-2/MPEG-1 */
  hw_fuse_sts->mpeg2_support_fuse = MPEG2_FUSE_ENABLED;
  /* HW supports Sorenson Spark */
  hw_fuse_sts->sorenson_spark_support_fuse = SORENSON_SPARK_ENABLED;
  /* HW supports baseline JPEG */
  hw_fuse_sts->jpeg_support_fuse = JPEG_FUSE_ENABLED;
  hw_fuse_sts->vp6_support_fuse = VP6_FUSE_ENABLED; /* HW supports VP6 */
  hw_fuse_sts->vc1_support_fuse = VC1_FUSE_ENABLED; /* HW supports VC-1 */
  /* HW supports progressive JPEG */
  hw_fuse_sts->jpeg_prog_support_fuse = JPEG_PROGRESSIVE_FUSE_ENABLED;
  hw_fuse_sts->ref_buf_support_fuse = REF_BUF_FUSE_ENABLED;
  hw_fuse_sts->avs_support_fuse = AVS_FUSE_ENABLED;
  hw_fuse_sts->rv_support_fuse = RV_FUSE_ENABLED;
  hw_fuse_sts->vp7_support_fuse = VP7_FUSE_ENABLED;
  hw_fuse_sts->vp8_support_fuse = VP8_FUSE_ENABLED;
  hw_fuse_sts->mvc_support_fuse = MVC_FUSE_ENABLED;

  /* PP fuses */
  hw_fuse_sts->pp_support_fuse = PP_FUSE_ENABLED; /* HW supports PP */
  /* PP fuse has all optional functions */
  hw_fuse_sts->pp_config_fuse = PP_FUSE_DEINTERLACING_ENABLED |
                                PP_FUSE_ALPHA_BLENDING_ENABLED |
                                MAX_PP_OUT_WIDHT_1920_FUSE_ENABLED;
}

/*------------------------------------------------------------------------------
    Function name   : DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : struct DWLInitParam * param - initialization params
------------------------------------------------------------------------------*/
const void *DWLInit(struct DWLInitParam *param) {
  struct DWLInstance *dwl_inst;
  unsigned int i;

  dwl_inst = (struct DWLInstance *)calloc(1, sizeof(struct DWLInstance));
  dwl_inst->reference_total = 0;
  dwl_inst->linear_total = 0;

  switch (param->client_type) {
    case DWL_CLIENT_TYPE_HEVC_DEC:
      printf("DWL initialized by an HEVC decoder instance...\n");
      break;
    case DWL_CLIENT_TYPE_PP:
      printf("DWL initialized by a PP instance...\n");
      break;
    case DWL_CLIENT_TYPE_VP9_DEC:
      printf("DWL initialized by a VP9 decoder instance...\n");
      break;
    default:
      printf("ERROR: DWL client type has to be always specified!\n");
      return NULL;
  }

#ifdef INTERNAL_TEST
  InternalTestInit();
#endif

  dwl_inst->client_type = param->client_type;
  dwl_inst->frm_base = NULL;
  dwl_inst->free_ref_frm_mem = NULL;

  pthread_mutex_lock(&dwl_init_mutex);
  /* Allocate cores just once */
  if (!n_dwl_instance_count) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    g_hw_core_array = InitializeCoreArray();
    if (g_hw_core_array == NULL) {
      free(dwl_inst);
      dwl_inst = NULL;
    }

    listener_thread_params.n_dec_cores = GetCoreCount();
    listener_thread_params.n_ppcores = 1; /* no multi-core support */

    for (i = 0; i < listener_thread_params.n_dec_cores; i++) {
      Core c = GetCoreById(g_hw_core_array, i);

      listener_thread_params.reg_base[i] = HwCoreGetBaseAddress(c);

      listener_thread_params.callback[i] = NULL;
    }

    listener_thread_params.b_stopped = 0;

    pthread_create(&mc_listener_thread, &attr, ThreadMcListener,
                   &listener_thread_params);

    pthread_attr_destroy(&attr);
  }

  dwl_inst->hw_core_array = g_hw_core_array;
  n_dwl_instance_count++;
  ActivityTraceInit(&dwl_inst->activity);

  pthread_mutex_unlock(&dwl_init_mutex);

  return (void *)dwl_inst;
}

/*------------------------------------------------------------------------------
    Function name   : DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 DWLRelease(const void *instance) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  unsigned int i;

  assert(dwl_inst != NULL);

  if (dwl_inst == NULL) return DWL_OK;

  assert(dwl_inst->reference_total == 0);
  assert(dwl_inst->reference_alloc_count == 0);
  assert(dwl_inst->linear_total == 0);
  assert(dwl_inst->linear_alloc_count == 0);

  pthread_mutex_lock(&dwl_init_mutex);

#ifdef INTERNAL_TEST
  InternalTestFinalize();
#endif

  n_dwl_instance_count--;

  /* Release the signal handling and cores just when
   * nobody is referencing them anymore
   */
  if (!n_dwl_instance_count) {
    listener_thread_params.b_stopped = 1;

    StopCoreArray(g_hw_core_array);

    pthread_join(mc_listener_thread, NULL);

    ReleaseCoreArray(g_hw_core_array);
  }

  pthread_mutex_unlock(&dwl_init_mutex);

  ActivityTraceRelease(&dwl_inst->activity);
  free((void *)dwl_inst);

#ifdef _DWL_PERFORMANCE
  printf("Total allocated reference mem = %8d\n", reference_total_max);
  printf("Total allocated linear mem    = %8d\n", linear_total_max);
  printf("Total allocated SWSW mem      = %8d\n", malloc_total_max);
#endif

  /* print core usage stats */
  {
    u32 total_usage = 0;
    u32 cores = DWLReadAsicCoreCount();
    for (i = 0; i < cores; i++) total_usage += core_usage_counts[i];
    /* avoid zero division */
    total_usage = total_usage ? total_usage : 1;

    printf("\nMulti-core usage statistics:\n");
    for (i = 0; i < cores; i++)
      printf("\tCore[%2d] used %6d times (%2d%%)\n", i, core_usage_counts[i],
             (core_usage_counts[i] * 100) / total_usage);

    printf("\n");
  }

  return DWL_OK;
}

void DWLSetIRQCallback(const void *instance, i32 core_id,
                       DWLIRQCallbackFn *callback_fn, void *arg) {
  listener_thread_params.callback[core_id] = callback_fn;
  listener_thread_params.callback_arg[core_id] = arg;
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : struct DWLLinearMem *info - place where the allocated
memory
                        buffer parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocRefFrm(const void *instance, u32 size, struct DWLLinearMem *info) {

  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  extern struct TBCfg tb_cfg;

  if (DWLTestRandomFail()) {
    return DWL_ERROR;
  }
#ifdef ASIC_TRACE_SUPPORT
  u32 memory_size;

  printf("DWLMallocRefFrm: %8d\n", size);

  if (dwl_inst->frm_base == NULL) {
    if (tb_cfg.tb_params.ref_frm_buffer_size == -1) {
      /* Max value based on limits set by Level 5.2 */
      /* Use tb.cfg to overwrite this limit */
      u32 cores = DWLReadAsicCoreCount();
      u32 max_frame_buffers = (36864 * (5 + 1 + cores)) * (384 + 64);
      /* TODO MIN function of "size" is not working here if we are
         not allocating all buffers in decoder init... */
      memory_size = MAX(max_frame_buffers, (16 + 1 + cores) * size);
      //memorySize = maxFrameBuffers;
      memory_size = memory_size * 2 /* Raster out requires double memory. */
                    + memory_size / 4; /* Down scaled to 1/4 (both to 1/2 in hor/ver). */
    } else {
      /* Use tb.cfg to set max size for nonconforming streams */
      memory_size = tb_cfg.tb_params.ref_frm_buffer_size;
    }
    memory_size = NEXT_MULTIPLE(memory_size, 16);
    dwl_inst->frm_base = (u8 *)memalign(16, memory_size);
    if (dwl_inst->frm_base == NULL) return DWL_ERROR;

    dwl_inst->reference_total = 0;
    dwl_inst->reference_maximum = memory_size;
    dwl_inst->free_ref_frm_mem = dwl_inst->frm_base;

    /* for DPB offset tracing */
    dpb_base_address = (u8 *)dwl_inst->frm_base;
  }

  /* Check that we have enough memory to spare */
  if (dwl_inst->free_ref_frm_mem + size > dwl_inst->frm_base + dwl_inst->reference_maximum)
    return DWL_ERROR;

  info->virtual_address = (u32 *)dwl_inst->free_ref_frm_mem;
  info->bus_address = (addr_t)info->virtual_address;
  info->size = size;
  info->logical_size = size;

  dwl_inst->free_ref_frm_mem += size;
#else
  printf("DWLMallocRefFrm: %8d\n", size);
  info->virtual_address = (u32 *)malloc(size);
  if (info->virtual_address == NULL) return DWL_ERROR;
  info->bus_address = (addr_t)info->virtual_address;
  info->size = size;
  info->logical_size = size;
#endif /* ASIC_TRACE_SUPPORT */

#ifdef _DWL_PERFORMANCE
  reference_total_max += size;
#endif /* _DWL_PERFORMANCE */

  dwl_inst->reference_total += size;
  dwl_inst->reference_alloc_count++;
  printf("DWLMallocRefFrm: memory allocated %8d bytes in %2d buffers\n",
         dwl_inst->reference_total, dwl_inst->reference_alloc_count);
  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with
                        DWLMallocRefFrm.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : struct DWLLinearMem *info - frame buffer memory
information
------------------------------------------------------------------------------*/
void DWLFreeRefFrm(const void *instance, struct DWLLinearMem *info) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  assert(dwl_inst != NULL);

  printf("DWLFreeRefFrm: %8d\n", info->size);
  dwl_inst->reference_total -= info->size;
  dwl_inst->reference_alloc_count--;
  printf("DWLFreeRefFrm: not freed %8d bytes in %2d buffers\n",
         dwl_inst->reference_total, dwl_inst->reference_alloc_count);

#ifdef ASIC_TRACE_SUPPORT
  /* Release memory when calling DWLFreeRefFrm for the last buffer */
  if (dwl_inst->frm_base && (dwl_inst->reference_alloc_count == 0)) {
    assert(dwl_inst->reference_total == 0);
    free(dwl_inst->frm_base);
    dwl_inst->frm_base = NULL;
    dpb_base_address = NULL;
  }
#else
  free(info->virtual_address);
  info->size = 0;
#endif /* ASIC_TRACE_SUPPORT */
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : struct DWLLinearMem *info - place where the allocated
                        memory buffer parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocLinear(const void *instance, u32 size, struct DWLLinearMem *info) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;

  if (DWLTestRandomFail()) {
    return DWL_ERROR;
  }
  //info->virtual_address = calloc(size, 1);
  /* allocate 16-byte aligned memory */
  size = NEXT_MULTIPLE(size, 32);
  info->virtual_address = (u32 *)memalign(16, size);
  printf("DWLMallocLinear: %8d\n", size);
  if (info->virtual_address == NULL) return DWL_ERROR;
  info->bus_address = (addr_t)info->virtual_address;
  info->logical_size = size;
  info->size = size;
  dwl_inst->linear_total += size;
  dwl_inst->linear_alloc_count++;
  printf("DWLMallocLinear: allocated total %8d bytes in %2d buffers\n",
         dwl_inst->linear_total, dwl_inst->linear_alloc_count);

#ifdef _DWL_PERFORMANCE
  linear_total_max += size;
#endif /* _DWL_PERFORMANCE */
  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with
                        DWLMallocLinear.

    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : struct DWLLinearMem *info - linear buffer memory
information
------------------------------------------------------------------------------*/
void DWLFreeLinear(const void *instance, struct DWLLinearMem *info) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  assert(dwl_inst != NULL);

  printf("DWLFreeLinear: %8d\n", info->size);
  dwl_inst->linear_total -= info->size;
  dwl_inst->linear_alloc_count--;
  printf("DWLFreeLinear: not freed %8d bytes in %2d buffers\n",
         dwl_inst->linear_total, dwl_inst->linear_alloc_count);
  free(info->virtual_address);
  info->size = 0;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWriteReg
    Description     : Write a value to a hardware IO register

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLWriteReg(const void *instance, i32 core_id, u32 offset, u32 value) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);
  u32 *core_reg_base = HwCoreGetBaseAddress(c);
#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("core[%d] swreg[%d] at offset 0x%02X = %08X\n", core_id, offset / 4,
            offset, value);
#endif
#ifdef INTERNAL_TEST
  InternalTestDumpWriteSwReg(core_id, offset >> 2, value, core_reg_base);
#endif
  assert(offset <= DEC_X170_REGS * 4);

  core_reg_base[offset >> 2] = value;
}

static Core last_dec_core = NULL;

/*------------------------------------------------------------------------------
    Function name   : DWLEnableHw
    Description     :
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLEnableHw(const void *instance, i32 core_id, u32 offset, u32 value) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);

  if (!IS_PIPELINE_ENABLED(value)) assert(c == dwl_inst->current_core);

  DWLWriteReg(dwl_inst, core_id, offset, value);

  ActivityTraceStartDec(&dwl_inst->activity);
#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("HW enabled by previous DWLWriteReg\n");
#endif

  if (dwl_inst->client_type != DWL_CLIENT_TYPE_PP) {
    HwCoreDecEnable(c);
  } else {
    /* standalone PP start */
    HwCorePpEnable(c, last_dec_core != NULL ? 0 : 1);
  }
}

/*------------------------------------------------------------------------------
    Function name   : DWLDisableHw
    Description     :
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLDisableHw(const void *instance, i32 core_id, u32 offset, u32 value) {
  DWLWriteReg(instance, core_id, offset, value);
#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("HW disabled by previous DWLWriteReg\n");
#endif
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadReg
    Description     : Read the value of a hardware IO register
    Return type     : u32 - the value stored in the register
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be read
------------------------------------------------------------------------------*/
u32 DWLReadReg(const void *instance, i32 core_id, u32 offset) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);
  u32 *core_reg_base = HwCoreGetBaseAddress(c);
  u32 val;

#ifdef INTERNAL_TEST
  InternalTestDumpReadSwReg(core_id, offset >> 2, core_reg_base[offset >> 2],
                            core_reg_base);
#endif

  assert(offset <= DEC_X170_REGS * 4);

  val = core_reg_base[offset >> 2];

#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("core[%d] swreg[%d] at offset 0x%02X = %08X\n", core_id, offset / 4,
            offset, val);
#endif

  return val;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitDecHwReady
    Description     : Wait until decoder hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.
    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR
    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitDecHwReady(const void *instance, i32 core_id, u32 timeout) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);

  assert(c == dwl_inst->current_core);

  if (HwCoreWaitDecRdy(c) != 0) {
    return (i32)DWL_HW_WAIT_ERROR;
  }

  return (i32)DWL_HW_WAIT_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitPpHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.
    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR
    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitPpHwReady(const void *instance, i32 core_id, u32 timeout) {

  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);

  assert(c == dwl_inst->current_core);

  if (HwCoreWaitPpRdy(c) != 0) {
    return (i32)DWL_HW_WAIT_ERROR;
  }

#ifdef ASIC_TRACE_SUPPORT
/* update swregister_accesses.trc */
/* TODO pp is currently not used, so put the trace function into comments
   to disable compiler warning.
TraceWaitPpEnd(); */
#endif

  return (i32)DWL_HW_WAIT_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.
    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR
    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitHwReady(const void *instance, i32 core_id, u32 timeout) {
  struct DWLInstance *dec_dwl = (struct DWLInstance *)instance;

  i32 ret;
  assert(dec_dwl);
  switch (dec_dwl->client_type) {
    case DWL_CLIENT_TYPE_HEVC_DEC:
    case DWL_CLIENT_TYPE_VP9_DEC:
      ret = DWLWaitDecHwReady(dec_dwl, core_id, timeout);
      break;
    case DWL_CLIENT_TYPE_PP:
      ret = DWLWaitPpHwReady(dec_dwl, core_id, timeout);
      break;
    default:
      assert(0); /* should not happen */
      ret = DWL_HW_WAIT_ERROR;
      break;
  }

  ActivityTraceStopDec(&dec_dwl->activity);
  return ret;
}

/*------------------------------------------------------------------------------
    Function name   : DWLmalloc
    Description     : Allocate a memory block. Same functionality as
                      the ANSI C malloc()
    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available
    Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *DWLmalloc(u32 n) {
  if (DWLTestRandomFail()) {
    return NULL;
  }
  DWL_DEBUG("%8d\n", n);

#ifdef _DWL_PERFORMANCE
  malloc_total_max += n;
#endif /* _DWL_PERFORMANCE */

  return malloc((size_t)n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()
    Return type     : void
    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void DWLfree(void *p) { free(p); }

/*------------------------------------------------------------------------------
    Function name   : DWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()
    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available
    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *DWLcalloc(u32 n, u32 s) {
  if (DWLTestRandomFail()) {
    return NULL;
  }
  DWL_DEBUG("%8d\n", n * s);
#ifdef _DWL_PERFORMANCE
  malloc_total_max += n * s;
#endif /* _DWL_PERFORMANCE */

  return calloc((size_t)n, (size_t)s);
}

/*------------------------------------------------------------------------------
    Function name   : DWLmemcpy
    Description     : Copies characters between buffers. Same functionality as
                      the ANSI C memcpy()
    Return type     : The value of destination d
    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *DWLmemcpy(void *d, const void *s, u32 n) {
  return memcpy(d, s, (size_t)n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLmemset
    Description     : Sets buffers to a specified character. Same functionality
                      as the ANSI C memset()
    Return type     : The value of destination d
    Argument        : void *d - Pointer to destination
    Argument        : i32 c - Character to set
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *DWLmemset(void *d, i32 c, u32 n) { return memset(d, (int)c, (size_t)n); }

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 DWLReserveHw(const void *instance, i32 *core_id) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;

  if (dwl_inst->client_type == DWL_CLIENT_TYPE_PP) {
    pthread_mutex_lock(&pp_mutex);

    if (last_dec_core == NULL) /* Blocks until core available. */
      dwl_inst->current_core = BorrowHwCore(dwl_inst->hw_core_array);
    else
      /* We rely on the fact that in combined mode the PP is always reserved
       * after the decoder
       */
      dwl_inst->current_core = last_dec_core;
  } else {
    /* Blocks until core available. */
    dwl_inst->current_core = BorrowHwCore(dwl_inst->hw_core_array);
    last_dec_core = dwl_inst->current_core;
  }

  *core_id = HwCoreGetid(dwl_inst->current_core);
  DWL_DEBUG("Reserved %s core %d\n",
            dwl_inst->client_type == DWL_CLIENT_TYPE_PP ? "PP" : "DEC",
            *core_id);

  core_usage_counts[*core_id]++;

  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHwPipe
    Description     : Reserve both DEC and PP on same core for pipeline
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 DWLReserveHwPipe(const void *instance, i32 *core_id) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;

  /* only decoder can reserve a DEC+PP hardware for pipelined operation */
  assert(dwl_inst->client_type != DWL_CLIENT_TYPE_PP);

  /* Blocks until core available. */
  dwl_inst->current_core = BorrowHwCore(dwl_inst->hw_core_array);
  last_dec_core = dwl_inst->current_core;

  /* lock PP also */
  pthread_mutex_lock(&pp_mutex);

  dwl_inst->b_reserved_pipe = 1;

  *core_id = HwCoreGetid(dwl_inst->current_core);
  DWL_DEBUG("Reserved DEC+PP core %d\n", *core_id);

  core_usage_counts[*core_id]++;

  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReleaseHw
    Description     :
    Return type     : void
    Argument        : const void *instance
------------------------------------------------------------------------------*/
void DWLReleaseHw(const void *instance, i32 core_id) {
  struct DWLInstance *dwl_inst = (struct DWLInstance *)instance;
  Core c = GetCoreById(dwl_inst->hw_core_array, core_id);

  if (dwl_inst->client_type == DWL_CLIENT_TYPE_PP) {
    DWL_DEBUG("Released PP core %d\n", core_id);

    pthread_mutex_unlock(&pp_mutex);

    /* core will be released by decoder */
    if (last_dec_core != NULL) return;
  }

  /* PP reserved by decoder in DWLReserveHwPipe */
  if (dwl_inst->b_reserved_pipe) pthread_mutex_unlock(&pp_mutex);

  dwl_inst->b_reserved_pipe = 0;

  ReturnHwCore(dwl_inst->hw_core_array, c);
  last_dec_core = NULL;
  DWL_DEBUG("Released %s core %d\n",
            dwl_inst->client_type == DWL_CLIENT_TYPE_PP ? "PP" : "DEC",
            core_id);
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicCoreCount
    Description     : Return number of ASIC cores, static implementation
    Return type     : u32
    Argument        : void
------------------------------------------------------------------------------*/
u32 DWLReadAsicCoreCount(void) { return GetCoreCount(); }

i32 DWLTestRandomFail(void) {
#ifdef FAIL_DURING_ALLOC
  if (!failed_alloc_count) {
    srand(time(NULL));
  }
  failed_alloc_count++;

  /* If fail preset to this alloc occurance, failt it */
  if (failed_alloc_count == FAIL_DURING_ALLOC) {
    printf("DWL: Preset allocation fail during alloc %d\n", failed_alloc_count);
    return DWL_ERROR;
  }
  /* If failing point is preset, no randomization */
  if (FAIL_DURING_ALLOC > 0) return DWL_OK;

  if ((rand() % 100) > 90) {
    printf("DWL: Testing a failure in memory allocation number %d\n",
           failed_alloc_count);
    return DWL_ERROR;
  } else {
    return DWL_OK;
  }
#endif
  return DWL_OK;
}
