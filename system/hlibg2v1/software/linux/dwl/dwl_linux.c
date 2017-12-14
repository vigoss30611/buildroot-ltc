/*  Copyright 2013 Google Inc. All Rights Reserved. */

#include "basetype.h"
#include "dwl_activity_trace.h"
#include "dwl_defs.h"
#include "dwl_linux.h"
#include "dwl.h"
#include "hantrodec.h"
#include "memalloc.h"
#include "sw_util.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef INTERNAL_TEST
#include "internal_test.h"
#endif

#define DWL_PJPEG_E 22    /* 1 bit */
#define DWL_REF_BUFF_E 20 /* 1 bit */

#define DWL_JPEG_EXT_E 31        /* 1 bit */
#define DWL_REF_BUFF_ILACE_E 30  /* 1 bit */
#define DWL_MPEG4_CUSTOM_E 29    /* 1 bit */
#define DWL_REF_BUFF_DOUBLE_E 28 /* 1 bit */

#define DWL_MVC_E 20 /* 2 bits */

#define DWL_DEC_TILED_L 17   /* 2 bits */
#define DWL_DEC_PIC_W_EXT 14 /* 2 bits */
#define DWL_EC_E 12          /* 2 bits */
#define DWL_STRIDE_E 11      /* 1 bit */
#define DWL_FIELD_DPB_E 10   /* 1 bit */

#define DWL_CFG_E 24         /* 4 bits */
#define DWL_PP_IN_TILED_L 14 /* 2 bits */

#define DWL_SORENSONSPARK_E 11 /* 1 bit */

#define DWL_DOUBLEBUFFER_E 1 /* 1 bit */

#define DWL_H264_FUSE_E 31          /* 1 bit */
#define DWL_MPEG4_FUSE_E 30         /* 1 bit */
#define DWL_MPEG2_FUSE_E 29         /* 1 bit */
#define DWL_SORENSONSPARK_FUSE_E 28 /* 1 bit */
#define DWL_JPEG_FUSE_E 27          /* 1 bit */
#define DWL_VP6_FUSE_E 26           /* 1 bit */
#define DWL_VC1_FUSE_E 25           /* 1 bit */
#define DWL_PJPEG_FUSE_E 24         /* 1 bit */
#define DWL_CUSTOM_MPEG4_FUSE_E 23  /* 1 bit */
#define DWL_RV_FUSE_E 22            /* 1 bit */
#define DWL_VP7_FUSE_E 21           /* 1 bit */
#define DWL_VP8_FUSE_E 20           /* 1 bit */
#define DWL_AVS_FUSE_E 19           /* 1 bit */
#define DWL_MVC_FUSE_E 18           /* 1 bit */
#define DWL_HEVC_FUSE_E 17          /* 1 bit */
#define DWL_VP9_FUSE_E 6            /* 1 bit */

#define DWL_DEC_MAX_4K_FUSE_E 16   /* 1 bit */
#define DWL_DEC_MAX_1920_FUSE_E 15 /* 1 bit */
#define DWL_DEC_MAX_1280_FUSE_E 14 /* 1 bit */
#define DWL_DEC_MAX_720_FUSE_E 13  /* 1 bit */
#define DWL_DEC_MAX_352_FUSE_E 12  /* 1 bit */
#define DWL_REF_BUFF_FUSE_E 7      /* 1 bit */

#define DWL_PP_FUSE_E 31             /* 1 bit */
#define DWL_PP_DEINTERLACE_FUSE_E 30 /* 1 bit */
#define DWL_PP_ALPHA_BLEND_FUSE_E 29 /* 1 bit */
#define DWL_PP_MAX_4096_FUSE_E 16    /* 1 bit */
#define DWL_PP_MAX_1920_FUSE_E 15    /* 1 bit */
#define DWL_PP_MAX_1280_FUSE_E 14    /* 1 bit */
#define DWL_PP_MAX_720_FUSE_E 13     /* 1 bit */
#define DWL_PP_MAX_352_FUSE_E 12     /* 1 bit */

#define DWL_MRB_PREFETCH_E 7         /* 1 bit */
#define DWL_FORMAT_CUSTOMER1_E 6     /* 1 bit */
#define DWL_FORMAT_P010_E 5          /* 1 bit */
#define DWL_ADDR_64_E 4              /* 1 bit */

#ifdef _DWL_FAKE_HW_TIMEOUT
static void DWLFakeTimeout(u32 *status);
#endif

/* shadow HW registers */
u32 dwl_shadow_regs[MAX_ASIC_CORES][264];

static inline u32 CheckRegOffset(struct HX170DWL *dec_dwl, u32 offset) {
  if (dec_dwl->client_type == DWL_CLIENT_TYPE_PP)
    return offset < dec_dwl->reg_size && offset >= HANTRODECPP_REG_START;
  else
    return offset < dec_dwl->reg_size;
}

static void PrintIrqType(u32 core_id, u32 status) {
  if (status & DEC_IRQ_ABORT)
    printf("DEC[%d] IRQ ABORT\n", core_id);
  else if (status & DEC_IRQ_RDY)
    printf("DEC[%d] IRQ READY\n", core_id);
  else if (status & DEC_IRQ_BUS)
    printf("DEC[%d] IRQ BUS ERROR\n", core_id);
  else if (status & DEC_IRQ_BUFFER)
    printf("DEC[%d] IRQ BUFFER\n", core_id);
  else if (status & DEC_IRQ_ASO)
    printf("DEC[%d] IRQ ASO\n", core_id);
  else if (status & DEC_IRQ_ERROR)
    printf("DEC[%d] IRQ STREAM ERROR\n", core_id);
  else if (status & DEC_IRQ_SLICE)
    printf("DEC[%d] IRQ SLICE\n", core_id);
  else if (status & DEC_IRQ_TIMEOUT)
    printf("DEC[%d] IRQ TIMEOUT\n", core_id);
  else
    printf("DEC[%d] IRQ UNKNOWN 0x%08x\n", core_id, status);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMapRegisters
    Description     :

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 *DWLMapRegisters(int mem_dev, unsigned long base, unsigned int reg_size,
                     u32 write) {
  const int page_size = getpagesize();
  const int page_alignment = page_size - 1;

  size_t map_size;
  const char *io = MAP_FAILED;

  /* increase mapping size with unaligned part */
  map_size = reg_size + (base & page_alignment);

  /* map page aligned base */
  if (write)
    io = (char *)mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_dev,
                      base & ~page_alignment);
  else
    io = (char *)mmap(0, map_size, PROT_READ, MAP_SHARED, mem_dev,
                      base & ~page_alignment);

  /* add offset from alignment to the io start address */
  if (io != MAP_FAILED) io += (base & page_alignment);

  return (u32 *)io;
}

void DWLUnmapRegisters(const void *io, unsigned int reg_size) {
  const int page_size = getpagesize();
  const int page_alignment = page_size - 1;

  munmap((void *)((int)io & (~page_alignment)),
         reg_size + ((int)io & page_alignment));
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicCoreCount
    Description     : Return the number of hardware cores available
------------------------------------------------------------------------------*/
u32 DWLReadAsicCoreCount(void) {
  int fd_dec;
  unsigned int cores = 0;

  /* open driver */
  fd_dec = open(DEC_MODULE_PATH, O_RDONLY);
  if (fd_dec == -1) {
    DWL_DEBUG("failed to open %s\n", DEC_MODULE_PATH);
    return 0;
  }

  /* ask module for cores */
  if (ioctl(fd_dec, HANTRODEC_IOC_MC_CORES, &cores) == -1) {
    DWL_DEBUG("ioctl failed\n");
    cores = 0;
  }

  if (fd_dec != -1) close(fd_dec);

  return (u32)cores;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicID
    Description     : Read the HW ID. Does not need a DWL instance to run

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 DWLReadAsicID() {
  u32 *io = MAP_FAILED, id = ~0;
  int fd_dec = -1, fd;
  unsigned long base;
  unsigned int reg_size;

  DWL_DEBUG("\n");

  fd = open("/dev/mem", O_RDONLY);
  if (fd == -1) {
    DWL_DEBUG("failed to open /dev/mem\n");
    goto end;
  }

  fd_dec = open(DEC_MODULE_PATH, O_RDONLY);
  if (fd_dec == -1) {
    DWL_DEBUG("failed to open %s\n", DEC_MODULE_PATH);
    goto end;
  }

  /* ask module for base */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWOFFSET, &base) == -1) {
    DWL_DEBUG("ioctl failed\n");
    goto end;
  }

  /* ask module for reg size */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWIOSIZE, &reg_size) == -1) {
    DWL_DEBUG("ioctl failed\n");
    goto end;
  }

  io = DWLMapRegisters(fd, base, reg_size, 0);

  if (io == MAP_FAILED) {
    DWL_DEBUG("failed to mmap regs.");
    goto end;
  }

  id = io[0];

  DWLUnmapRegisters(io, reg_size);

end:
  if (fd != -1) close(fd);
  if (fd_dec != -1) close(fd_dec);

  return id;
}

static void ReadCoreFuse(const u32 *io, struct DWLHwFuseStatus *hw_fuse_sts) {
  u32 config_reg, fuse_reg, fuse_reg_pp;

  /* Decoder configuration */
  config_reg = io[HANTRODEC_SYNTH_CFG];

  /* Decoder fuse configuration */
  fuse_reg = io[HANTRODEC_FUSE_CFG];

  hw_fuse_sts->vp6_support_fuse = (fuse_reg >> DWL_VP6_FUSE_E) & 0x01U;
  hw_fuse_sts->vp7_support_fuse = (fuse_reg >> DWL_VP7_FUSE_E) & 0x01U;
  hw_fuse_sts->vp8_support_fuse = (fuse_reg >> DWL_VP8_FUSE_E) & 0x01U;
  hw_fuse_sts->vp9_support_fuse = (fuse_reg >> DWL_VP9_FUSE_E) & 0x01U;
  hw_fuse_sts->h264_support_fuse = (fuse_reg >> DWL_H264_FUSE_E) & 0x01U;
  hw_fuse_sts->HevcSupportFuse = (fuse_reg >> DWL_HEVC_FUSE_E) & 0x01U;
  hw_fuse_sts->mpeg4_support_fuse = (fuse_reg >> DWL_MPEG4_FUSE_E) & 0x01U;
  hw_fuse_sts->mpeg2_support_fuse = (fuse_reg >> DWL_MPEG2_FUSE_E) & 0x01U;
  hw_fuse_sts->sorenson_spark_support_fuse =
      (fuse_reg >> DWL_SORENSONSPARK_FUSE_E) & 0x01U;
  hw_fuse_sts->jpeg_support_fuse = (fuse_reg >> DWL_JPEG_FUSE_E) & 0x01U;
  hw_fuse_sts->vc1_support_fuse = (fuse_reg >> DWL_VC1_FUSE_E) & 0x01U;
  hw_fuse_sts->jpeg_prog_support_fuse = (fuse_reg >> DWL_PJPEG_FUSE_E) & 0x01U;
  hw_fuse_sts->rv_support_fuse = (fuse_reg >> DWL_RV_FUSE_E) & 0x01U;
  hw_fuse_sts->avs_support_fuse = (fuse_reg >> DWL_AVS_FUSE_E) & 0x01U;
  hw_fuse_sts->custom_mpeg4_support_fuse =
      (fuse_reg >> DWL_CUSTOM_MPEG4_FUSE_E) & 0x01U;
  hw_fuse_sts->mvc_support_fuse = (fuse_reg >> DWL_MVC_FUSE_E) & 0x01U;

  /* check max. decoder output width */
  if (fuse_reg & 0x10000U)
    hw_fuse_sts->max_dec_pic_width_fuse = 4096;
  else if (fuse_reg & 0x8000U)
    hw_fuse_sts->max_dec_pic_width_fuse = 1920;
  else if (fuse_reg & 0x4000U)
    hw_fuse_sts->max_dec_pic_width_fuse = 1280;
  else if (fuse_reg & 0x2000U)
    hw_fuse_sts->max_dec_pic_width_fuse = 720;
  else if (fuse_reg & 0x1000U)
    hw_fuse_sts->max_dec_pic_width_fuse = 352;

  hw_fuse_sts->ref_buf_support_fuse = (fuse_reg >> DWL_REF_BUFF_FUSE_E) & 0x01U;

  /* Pp configuration */
  config_reg = io[HANTRODECPP_SYNTH_CFG];

  if ((config_reg >> DWL_PP_E) & 0x01U) {
    /* Pp fuse configuration */
    fuse_reg_pp = io[HANTRODECPP_FUSE_CFG];

    if ((fuse_reg_pp >> DWL_PP_FUSE_E) & 0x01U) {
      hw_fuse_sts->pp_support_fuse = 1;

      /* check max. pp output width */
      if (fuse_reg_pp & 0x10000U)
        hw_fuse_sts->max_pp_out_pic_width_fuse = 4096;
      else if (fuse_reg_pp & 0x8000U)
        hw_fuse_sts->max_pp_out_pic_width_fuse = 1920;
      else if (fuse_reg_pp & 0x4000U)
        hw_fuse_sts->max_pp_out_pic_width_fuse = 1280;
      else if (fuse_reg_pp & 0x2000U)
        hw_fuse_sts->max_pp_out_pic_width_fuse = 720;
      else if (fuse_reg_pp & 0x1000U)
        hw_fuse_sts->max_pp_out_pic_width_fuse = 352;

      hw_fuse_sts->pp_config_fuse = fuse_reg_pp;
    } else {
      hw_fuse_sts->pp_support_fuse = 0;
      hw_fuse_sts->max_pp_out_pic_width_fuse = 0;
      hw_fuse_sts->pp_config_fuse = 0;
    }
  }
}

static void ReadCoreConfig(const u32 *io, DWLHwConfig *hw_cfg) {
  u32 config_reg;
  const u32 asic_id = io[0];

  /* Decoder configuration */
  config_reg = io[HANTRODEC_SYNTH_CFG];

  hw_cfg->h264_support = (config_reg >> DWL_H264_E) & 0x3U;
  /* check jpeg */
  hw_cfg->jpeg_support = (config_reg >> DWL_JPEG_E) & 0x01U;
  if (hw_cfg->jpeg_support && ((config_reg >> DWL_PJPEG_E) & 0x01U))
    hw_cfg->jpeg_support = JPEG_PROGRESSIVE;
  hw_cfg->mpeg4_support = (config_reg >> DWL_MPEG4_E) & 0x3U;
  hw_cfg->vc1_support = (config_reg >> DWL_VC1_E) & 0x3U;
  hw_cfg->mpeg2_support = (config_reg >> DWL_MPEG2_E) & 0x01U;
  hw_cfg->sorenson_spark_support = (config_reg >> DWL_SORENSONSPARK_E) & 0x01U;
#ifndef DWL_REFBUFFER_DISABLE
  hw_cfg->ref_buf_support = (config_reg >> DWL_REF_BUFF_E) & 0x01U;
#else
  hw_cfg->ref_buf_support = 0;
#endif
  hw_cfg->vp6_support = (config_reg >> DWL_VP6_E) & 0x01U;
#ifdef DEC_X170_APF_DISABLE
  if (DEC_X170_APF_DISABLE) {
    hw_cfg->tiled_mode_support = 0;
  }
#endif /* DEC_X170_APF_DISABLE */

  hw_cfg->max_dec_pic_width = config_reg & 0x07FFU;

  /* 2nd Config register */
  config_reg = io[HANTRODEC_SYNTH_CFG_2];
  if (hw_cfg->ref_buf_support) {
    if ((config_reg >> DWL_REF_BUFF_ILACE_E) & 0x01U)
      hw_cfg->ref_buf_support |= 2;
    if ((config_reg >> DWL_REF_BUFF_DOUBLE_E) & 0x01U)
      hw_cfg->ref_buf_support |= 4;
  }
  hw_cfg->vp9_support = (config_reg >> DWL_VP9_E) & 0x3U;
  hw_cfg->hevc_support = (config_reg >> DWL_HEVC_E) & 0x3U;
  hw_cfg->custom_mpeg4_support = (config_reg >> DWL_MPEG4_CUSTOM_E) & 0x01U;
  hw_cfg->vp7_support = (config_reg >> DWL_VP7_E) & 0x01U;
  hw_cfg->vp8_support = (config_reg >> DWL_VP8_E) & 0x01U;
  hw_cfg->avs_support = (config_reg >> DWL_AVS_E) & 0x01U;

  /* JPEG extensions */
  if (((asic_id >> 16) >= 0x8190U) || ((asic_id >> 16) == 0x6731U))
    hw_cfg->jpeg_esupport = (config_reg >> DWL_JPEG_EXT_E) & 0x01U;
  else
    hw_cfg->jpeg_esupport = JPEG_EXT_NOT_SUPPORTED;

  if (((asic_id >> 16) >= 0x9170U) || ((asic_id >> 16) == 0x6731U))
    hw_cfg->rv_support = (config_reg >> DWL_RV_E) & 0x03U;
  else
    hw_cfg->rv_support = RV_NOT_SUPPORTED;

  hw_cfg->mvc_support = (config_reg >> DWL_MVC_E) & 0x03U;
  hw_cfg->webp_support = (config_reg >> DWL_WEBP_E) & 0x01U;
  hw_cfg->tiled_mode_support = (config_reg >> DWL_DEC_TILED_L) & 0x03U;
  hw_cfg->max_dec_pic_width += ((config_reg >> DWL_DEC_PIC_W_EXT) & 0x03U)
                               << 11;

  hw_cfg->ec_support = (config_reg >> DWL_EC_E) & 0x03U;
  hw_cfg->stride_support = (config_reg >> DWL_STRIDE_E) & 0x01U;
  hw_cfg->field_dpb_support = (config_reg >> DWL_FIELD_DPB_E) & 0x01U;

  if (hw_cfg->ref_buf_support && ((asic_id >> 16) == 0x6731U)) {
    hw_cfg->ref_buf_support |= 8; /* enable HW support for offset */
  }

  hw_cfg->double_buffer_support = (config_reg >> DWL_DOUBLEBUFFER_E) & 0x01U;

  /* 3rd Config register */
  config_reg = io[HANTRODEC_SYNTH_CFG_3];
  hw_cfg->max_dec_pic_height = config_reg & 0x0FFFU;

  /* Pp configuration */
  config_reg = io[HANTRODECPP_SYNTH_CFG];

  if ((config_reg >> DWL_PP_E) & 0x01U) {
    hw_cfg->pp_support = 1;
    /* Theoretical max range 0...8191; actual 48...4096 */
    hw_cfg->max_pp_out_pic_width = config_reg & 0x1FFFU;
    /*hw_cfg->pp_config = (config_reg >> DWL_CFG_E) & 0x0FU; */
    hw_cfg->pp_config = config_reg;
  } else {
    hw_cfg->pp_support = 0;
    hw_cfg->max_pp_out_pic_width = 0;
    hw_cfg->pp_config = 0;
  }

  /* check the HW version */
  if (((asic_id >> 16) >= 0x8190U) || ((asic_id >> 16) == 0x6731U)) {
    u32 de_interlace;
    u32 alpha_blend;
    u32 de_interlace_fuse;
    u32 alpha_blend_fuse;
    struct DWLHwFuseStatus hw_fuse_sts;

    /* check fuse status */
    ReadCoreFuse(io, &hw_fuse_sts);

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
    /* custom mpeg-4 */
    if (!hw_fuse_sts.custom_mpeg4_support_fuse)
      hw_cfg->custom_mpeg4_support = MPEG4_CUSTOM_NOT_SUPPORTED;
    /* jpeg (baseline && progressive) */
    if (!hw_fuse_sts.jpeg_support_fuse)
      hw_cfg->jpeg_support = JPEG_NOT_SUPPORTED;
    if ((hw_cfg->jpeg_support == JPEG_PROGRESSIVE) &&
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

      /* check if */
      if (de_interlace && !de_interlace_fuse) hw_cfg->pp_config &= 0xFD000000;
      if (alpha_blend && !alpha_blend_fuse) hw_cfg->pp_config &= 0xFE000000;
    }
    /* sorenson */
    if (!hw_fuse_sts.sorenson_spark_support_fuse)
      hw_cfg->sorenson_spark_support = SORENSON_SPARK_NOT_SUPPORTED;
    /* ref. picture buffer */
    if (!hw_fuse_sts.ref_buf_support_fuse)
      hw_cfg->ref_buf_support = REF_BUF_NOT_SUPPORTED;

    /* rv */
    if (!hw_fuse_sts.rv_support_fuse) hw_cfg->rv_support = RV_NOT_SUPPORTED;
    /* avs */
    if (!hw_fuse_sts.avs_support_fuse) hw_cfg->avs_support = AVS_NOT_SUPPORTED;
    /* mvc */
    if (!hw_fuse_sts.mvc_support_fuse) hw_cfg->mvc_support = MVC_NOT_SUPPORTED;
  } else if ((asic_id >> 16) == 0x6732U) {
    /* G2 */
    config_reg = io[HANTRODEC_CFG_STAT];

    hw_cfg->hevc_support = (config_reg >> DWL_HEVC_ENA) & 0x1U;
    hw_cfg->vp9_support = (config_reg >> DWL_VP9_ENA) & 0x1U;

    hw_cfg->hevc_main10_support = hw_cfg->hevc_support && (((config_reg >> DWL_HEVC_VER) & 0xFU) == 1);
    hw_cfg->vp9_10bit_support = hw_cfg->vp9_support && (((config_reg >> DWL_VP9_PROFILE) & 0xFU) == 1);
    hw_cfg->ds_support = (config_reg >> DWL_DS_E) & 0x1U;
    hw_cfg->rfc_support = (config_reg >> DWL_RFC_E) & 0x1U;
    hw_cfg->ring_buffer_support = 1; /* Always enabled as default. */

    hw_cfg->mrb_prefetch = (config_reg >> DWL_MRB_PREFETCH_E) & 0x1U;
    hw_cfg->addr_64bit_support = (config_reg >> DWL_ADDR_64_E) & 0x1U;
    hw_cfg->fmt_p010_support = (config_reg >> DWL_FORMAT_P010_E) & 0x1U;
    hw_cfg->fmt_customer1_support = (config_reg >> DWL_FORMAT_CUSTOMER1_E) & 0x1U;
  }
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicConfig
    Description     : Read HW configuration. Does not need a DWL instance to run

    Return type     : DWLHwConfig - structure with HW configuration
------------------------------------------------------------------------------*/
void DWLReadAsicConfig(DWLHwConfig *hw_cfg) {
  const u32 *io = MAP_FAILED;
  unsigned int reg_size;
  unsigned long base;

  int fd = (-1), fd_dec = (-1);

  DWL_DEBUG("\n");

  fd = open("/dev/mem", O_RDONLY);
  if (fd == -1) {
    DWL_DEBUG("failed to open /dev/mem\n");
    goto end;
  }

  fd_dec = open(DEC_MODULE_PATH, O_RDONLY);
  if (fd_dec == -1) {
    DWL_DEBUG("failed to open %s\n", DEC_MODULE_PATH);
    goto end;
  }

  /* ask module for base */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWOFFSET, &base) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOCGHWOFFSET failed\n");
    goto end;
  }

  /* ask module for reg size */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWIOSIZE, &reg_size) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOCGHWIOSIZE failed\n");
    goto end;
  }

  io = DWLMapRegisters(fd, base, reg_size, 0);
  if (io == MAP_FAILED) {
    DWL_DEBUG("failed to mmap registers\n");
    goto end;
  }

  /* Decoder configuration */
  memset(hw_cfg, 0, sizeof(*hw_cfg));

  ReadCoreConfig(io, hw_cfg);

  DWLUnmapRegisters(io, reg_size);

end:
  if (fd != -1) close(fd);
  if (fd_dec != -1) close(fd_dec);
}

void DWLReadMCAsicConfig(DWLHwConfig hw_cfg[MAX_ASIC_CORES]) {
  const u32 *io = MAP_FAILED;
  unsigned int reg_size;
  unsigned int n_cores, i;
  unsigned long mc_reg_base[MAX_ASIC_CORES];

  int fd = (-1), fd_dec = (-1);

  DWL_DEBUG("\n");

  fd = open("/dev/mem", O_RDONLY);
  if (fd == -1) {
    DWL_DEBUG("failed to open /dev/mem\n");
    goto end;
  }

  fd_dec = open(DEC_MODULE_PATH, O_RDONLY);
  if (fd_dec == -1) {
    DWL_DEBUG("failed to open %s\n", DEC_MODULE_PATH);
    goto end;
  }

  if (ioctl(fd_dec, HANTRODEC_IOC_MC_CORES, &n_cores) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOC_MC_CORES failed\n");
    goto end;
  }

  assert(n_cores <= MAX_ASIC_CORES);

  if (ioctl(fd_dec, HANTRODEC_IOC_MC_OFFSETS, mc_reg_base) == -1) {
    DWL_DEBUG("ioctl HANTRODEC_IOC_MC_OFFSETS failed\n");
    goto end;
  }

  /* ask module for reg size */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWIOSIZE, &reg_size) == -1) {
    DWL_DEBUG("ioctl failed\n");
    goto end;
  }

  /* Decoder configuration */
  memset(hw_cfg, 0, MAX_ASIC_CORES * sizeof(*hw_cfg));

  for (i = 0; i < n_cores; i++) {
    io = DWLMapRegisters(fd, mc_reg_base[i], reg_size, 0);
    if (io == MAP_FAILED) {
      DWL_DEBUG("failed to mmap registers\n");
      goto end;
    }

    ReadCoreConfig(io, hw_cfg + i);

    DWLUnmapRegisters(io, reg_size);
  }

end:
  if (fd != -1) close(fd);
  if (fd_dec != -1) close(fd_dec);
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicFuseStatus
    Description     : Read HW fuse configuration. Does not need a DWL instance
to run

    Returns     : struct DWLHwFuseStatus * hw_fuse_sts - structure with HW fuse
configuration
------------------------------------------------------------------------------*/
void DWLReadAsicFuseStatus(struct DWLHwFuseStatus *hw_fuse_sts) {
  const u32 *io = MAP_FAILED;

  unsigned long base;
  unsigned int reg_size;

  int fd = (-1), fd_dec = (-1);

  DWL_DEBUG("\n");

  memset(hw_fuse_sts, 0, sizeof(*hw_fuse_sts));

  fd = open("/dev/mem", O_RDONLY);
  if (fd == -1) {
    DWL_DEBUG("failed to open /dev/mem\n");
    goto end;
  }

  fd_dec = open(DEC_MODULE_PATH, O_RDONLY);
  if (fd_dec == -1) {
    DWL_DEBUG("failed to open %s\n", DEC_MODULE_PATH);
    goto end;
  }

  /* ask module for base */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWOFFSET, &base) == -1) {
    DWL_DEBUG("ioctl failed\n");
    goto end;
  }

  /* ask module for reg size */
  if (ioctl(fd_dec, HANTRODEC_IOCGHWIOSIZE, &reg_size) == -1) {
    DWL_DEBUG("ioctl failed\n");
    goto end;
  }

  io = DWLMapRegisters(fd, base, reg_size, 0);

  if (io == MAP_FAILED) {
    DWL_DEBUG("failed to mmap\n");
    goto end;
  }

  /* Decoder fuse configuration */
  ReadCoreFuse(io, hw_fuse_sts);

  DWLUnmapRegisters(io, reg_size);

end:
  if (fd != -1) close(fd);
  if (fd_dec != -1) close(fd_dec);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocRefFrm(const void *instance, u32 size, struct DWLLinearMem *info) {

#ifdef MEMORY_USAGE_TRACE
  printf("DWLMallocRefFrm\t%8d bytes\n", size);
#endif

  return DWLMallocLinear(instance, size, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with
                        DWLMallocRefFrm.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - frame buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeRefFrm(const void *instance, struct DWLLinearMem *info) {
  DWLFreeLinear(instance, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocLinear(const void *instance, u32 size, struct DWLLinearMem *info) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  u32 pgsize = getpagesize();
  MemallocParams params;

  assert(dec_dwl != NULL);
  assert(info != NULL);

#ifdef MEMORY_USAGE_TRACE
  printf("DWLMallocLinear\t%8d bytes \n", size);
#endif

  info->logical_size = size;
  info->size = NEXT_MULTIPLE(size, pgsize);
  info->virtual_address = MAP_FAILED;
  info->bus_address = 0;

#if 0
  params.size = info->size;

  /* get memory linear memory buffers */
  ioctl(dec_dwl->fd_memalloc, MEMALLOC_IOCXGETBUFFER, &params);
  if (params.bus_address == 0) {
    DWL_DEBUG("ERROR! No linear buffer available\n");
    return DWL_ERROR;
  }

  /* The bus address for mmap and HW may be different. translation_offset
   * is used to calculate the bus address for HW access. If no translation is
   * needed memalloc-driver sets it to 0 */
  info->bus_address = params.bus_address - params.translation_offset;

  /* Map the bus address to virtual address */
  info->virtual_address =
      (u32 *)mmap(0, info->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                  dec_dwl->fd_mem, params.bus_address);

#ifdef MEMORY_USAGE_TRACE
  printf("DWLMallocLinear 0x%08x virtual_address: 0x%08x\n", info->bus_address,
         (unsigned)info->virtual_address);
#endif

  if (info->virtual_address == MAP_FAILED) return DWL_ERROR;
#else
  info->virtual_address = fr_alloc_single("dwl_g2",size, &info->bus_address);
  if(!info->virtual_address) {
	  printf("unable to allocat memory anymore\n");
	  return DWL_ERROR  ;
  }
  printf("alloc linear buf %d->v0x%X b0x%X\n",size ,info->virtual_address,info->bus_address);
#endif

  return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with
                        DWLMallocLinear.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - linear buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeLinear(const void *instance, struct DWLLinearMem *info) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

  assert(dec_dwl != NULL);
  assert(info != NULL);

#if 0
  if (info->bus_address != 0)
    ioctl(dec_dwl->fd_memalloc, MEMALLOC_IOCSFREEBUFFER, &info->bus_address);

  if (info->virtual_address != MAP_FAILED)
    munmap(info->virtual_address, info->size);
#else
  fr_free_single(info->virtual_address, info->bus_address);
  DWL_DEBUG("G2DWLFreeLinear::mmu(%d) uid= %d, phy= %x, vir =%p \n",
    dec_dwl->mmu_enable, info->uid, info->bus_address,info->virtual_address);
#endif
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
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;

#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("core[%d] swreg[%d] at offset 0x%02X = %08X\n", core_id, offset / 4,
            offset, value);
#endif

  assert(dec_dwl != NULL);
  assert(CheckRegOffset(dec_dwl, offset));
  assert(core_id < (i32)dec_dwl->num_cores);

  offset = offset / 4;

  dwl_shadow_regs[core_id][offset] = value;

#ifdef INTERNAL_TEST
  InternalTestDumpWriteSwReg(core_id, offset, value, dwl_shadow_regs[core_id]);
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
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;
  u32 val;

  assert(dec_dwl != NULL);

  assert(CheckRegOffset(dec_dwl, offset));
  assert(core_id < (i32)dec_dwl->num_cores);

  offset = offset / 4;

  val = dwl_shadow_regs[core_id][offset];

#ifndef DWL_DISABLE_REG_PRINTS
  DWL_DEBUG("core[%d] swreg[%d] at offset 0x%02X = %08X\n", core_id, offset,
            offset * 4, val);
#endif

#ifdef INTERNAL_TEST
  InternalTestDumpReadSwReg(core_id, offset, val, dwl_shadow_regs[core_id]);
#endif

  return val;
}

/*------------------------------------------------------------------------------
    Function name   : DWLEnableHw
    Description     : Enable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLEnableHw(const void *instance, i32 core_id, u32 offset, u32 value) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;
  struct core_desc core;
  int ioctl_req;

  assert(dec_dwl);

  ioctl_req = HANTRODEC_IOCS_DEC_PUSH_REG;

  DWLWriteReg(dec_dwl, core_id, offset, value);

  DWL_DEBUG("%s %d enabled by previous DWLWriteReg\n", "DEC", core_id);

  core.id = core_id;
  core.regs = dwl_shadow_regs[core_id];
  core.size = dec_dwl->reg_size;

  ActivityTraceStartDec(&dec_dwl->activity);

  if (ioctl(dec_dwl->fd, ioctl_req, &core)) {
    DWL_DEBUG("ioctl HANTRODEC_IOCS_*_PUSH_REG failed\n");
    assert(0);
  }
}

/*------------------------------------------------------------------------------
    Function name   : DWLDisableHw
    Description     : Disable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLDisableHw(const void *instance, i32 core_id, u32 offset, u32 value) {
  struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;
  struct core_desc core;
  int ioctl_req;

  assert(dec_dwl);

  ioctl_req = HANTRODEC_IOCS_DEC_PUSH_REG;

  DWLWriteReg(dec_dwl, core_id, offset, value);

  DWL_DEBUG("%s %d disabled by previous DWLWriteReg\n", "DEC", core_id);

  core.id = core_id;
  core.regs = dwl_shadow_regs[core_id];
  core.size = dec_dwl->reg_size;

  if (ioctl(dec_dwl->fd, ioctl_req, &core)) {
    DWL_DEBUG("ioctl HANTRODEC_IOCS_*_PUSH_REG failed\n");
    assert(0);
  }
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
  const struct HX170DWL *dec_dwl = (struct HX170DWL *)instance;
  struct core_desc core;
  int ioctl_req;
  i32 ret = DWL_HW_WAIT_OK;

#ifndef DWL_USE_DEC_IRQ
  int max_wait_time = 10000; /* 10s in ms */
#endif

  assert(dec_dwl);

  DWL_DEBUG("%s %d\n", "DEC", core_id);

  core.id = core_id;
  core.regs = dwl_shadow_regs[core_id];
  core.size = dec_dwl->reg_size;

#ifdef DWL_USE_DEC_IRQ
#if 0
  ioctl_req = HANTRODEC_IOCX_DEC_WAIT;

  if (ioctl(dec_dwl->fd, ioctl_req, &core)) {
    DWL_DEBUG("ioctl HANTRODEC_IOCG_*_WAIT failed\n");
    ret = DWL_HW_WAIT_ERROR;
  }
#else

  sem_wait(dec_dwl->sync_params->sc_dec_rdy_sem + core_id);
#endif
#else /* Polling */

  ret = DWL_HW_WAIT_TIMEOUT;

  ioctl_req = HANTRODEC_IOCS_DEC_PULL_REG;

  do {
    u32 irq_stats;
    const unsigned int usec = 1000; /* 1 ms polling interval */

    if (ioctl(dec_dwl->fd, ioctl_req, &core)) {
      DWL_DEBUG("ioctl HANTRODEC_IOCS_*_PULL_REG failed\n");
      ret = DWL_HW_WAIT_ERROR;
      break;
    }
    usleep(usec);
    irq_stats =  dwl_shadow_regs[core_id][HANTRODEC_IRQ_STAT_DEC];

    irq_stats = (irq_stats >> 11) & 0xFF;

    if (irq_stats != 0) {
      ret = DWL_HW_WAIT_OK;
      break;
    }

    max_wait_time--;
  } while (max_wait_time > 0);

#endif

#ifdef _DWL_DEBUG
  {
    u32 irq_stats =  dwl_shadow_regs[core_id][HANTRODEC_IRQ_STAT_DEC];

    PrintIrqType(core_id, irq_stats);
  }
#endif

  ActivityTraceStopDec(&dec_dwl->activity);

  DWL_DEBUG("%s %d done\n", "DEC", core_id);

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
#ifdef MEMORY_USAGE_TRACE
  printf("DWLmalloc\t%8d bytes\n", n);
#endif
  return malloc((size_t)n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()

    Return type     : void

    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void DWLfree(void *p) {
  if (p != NULL) free(p);
}

/*------------------------------------------------------------------------------
    Function name   : DWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

}
    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *DWLcalloc(u32 n, u32 s) {
#ifdef MEMORY_USAGE_TRACE
  printf("DWLcalloc\t%8d bytes\n", n * s);
#endif
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
    Function name   : DWLFakeTimeout
    Description     : Testing help function that changes HW stream errors info
                        HW timeouts. You can check how the SW behaves or not.
    Return type     : void
    Argument        : void
------------------------------------------------------------------------------*/

#ifdef _DWL_FAKE_HW_TIMEOUT
void DWLFakeTimeout(u32 *status) {

  if ((*status) & DEC_IRQ_ERROR) {
    *status &= ~DEC_IRQ_ERROR;
    *status |= DEC_IRQ_TIMEOUT;
    printf("\n_dwl: Change stream error to hw timeout\n");
  }
}
#endif
