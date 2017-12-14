/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description :  dwl common part header file
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_linux.h,v $
--  $Revision: 1.8 $
--  $Date: 2008/11/28 12:32:33 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"

#include "dwl_linux_lock.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#ifdef USE_EFENCE
//#include "efence.h"
//#endif
//#include "InfotmMedia.h"
//#include "IM_buffallocapi.h"
//#define ION_USE
//#ifdef ION_USE
//#include "IM_devmmuapi.h"
//#endif 
#ifdef _DWL_DEBUG
#if (TARGET_SYSTEM == FS_ANDROID)
#include <cutils/log.h>
#ifndef LOGD
#define LOGD ALOGE
#endif
#define DWL_DEBUG(fmt, args...) LOGD(__FILE__ ":%d: " fmt, __LINE__ , ## args)
#else
#define DWL_DEBUG(fmt, args...) printf(__FILE__ ":%d: " fmt, __LINE__ , ## args)
#endif 
#else
#define DWL_DEBUG(fmt, args...) /* not debugging: nothing */
#endif

#ifdef INTERNAL_TEST
#define REG_DUMP_FILE "swreg.trc"
#endif

#ifndef HX170DEC_IO_BASE
#define HX170DEC_IO_BASE   0xC0000000U
#endif

#define HX170PP_REG_START       0xF0
#define HX170DEC_REG_START      0x4

#define HX170PP_SYNTH_CFG       100
#define HX170DEC_SYNTH_CFG       50
#define HX170DEC_SYNTH_CFG_2     54

#define HX170PP_FUSE_CFG         99
#define HX170DEC_FUSE_CFG        57

#define DWL_DECODER_INT ((G1DWLReadReg(dec_dwl, HX170DEC_REG_START) >> 12) & 0xFFU)
#define DWL_PP_INT      ((G1DWLReadReg(dec_dwl, HX170PP_REG_START) >> 12) & 0xFFU)

#define DWL_STREAM_ERROR_BIT    0x010000    /* 16th bit */
#define DWL_HW_TIMEOUT_BIT      0x040000    /* 18th bit */
#define DWL_HW_ENABLE_BIT       0x000001    /* 0th bit */
#define DWL_HW_PIC_RDY_BIT      0x001000    /* 12th bit */

#ifdef _DWL_HW_PERFORMANCE
/* signal that decoder/pp is enabled */
void DwlDecoderEnable(void);
void DwlPpEnable(void);
#endif

#ifndef bool
#define bool int
#endif

/* Function prototypes */

/* wrapper information */
typedef struct hX170dwl
{
    u32 clientType;
    int fd;                  /* decoder device file */
    int fd_mem;              /* /dev/mem for mapping */
    //ALCCTX alc_inst;         /* memory allocator */
    //DMMUINST dmmu_inst;      /* dev mmu inst */
    volatile u32 *pRegBase;  /* IO mem base */
    u32 regSize;             /* IO mem size */
    volatile u32 *pNV21Base;  /* nv21 mem base */
    u32 freeLinMem;          /* Start address of free linear memory */
    u32 freeRefFrmMem;       /* Start address of free reference frame memory */
    int semid;
    int sigio_needed;   
    int mmu_enable;
    int latencyMS;
    int latencyValid; 
#ifdef TIME_DEBUG
	unsigned long long total_dec_time;
	unsigned long long total_dec_count;

	unsigned long long total_pp_time;
	unsigned long long total_pp_count;
#endif
#ifdef INTERNAL_TEST
    FILE *regDump;
#endif

    unsigned int dvfsEna;
	bool  hwEna;
} hX170dwl_t;


i32 DWLWaitPpHwReady(const void *instance, u32 timeout);
i32 DWLWaitDecHwReady(const void *instance, u32 timeout);
u32 *G1DWLMapRegisters(int mem_dev, unsigned int base,
                     unsigned int regSize, u32 write);
void G1G1DWLUnmapRegisters(const void *io, unsigned int regSize);

