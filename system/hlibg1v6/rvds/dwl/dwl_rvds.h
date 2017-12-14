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
#ifndef _DWL_RVDS_H_
#define _DWL_RVDS_H_
#include <imapx_system.h>
#include <cmnlib.h>

#include "basetype.h"
#include "dwl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif

#ifdef _DWL_DEBUG
#define DWL_DEBUG(fmt, args...) 
#else
#define DWL_DEBUG(fmt, args...) /* not debugging: nothing */
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

/* Function prototypes */

/* wrapper information */
typedef struct hX170dwl
{
    u32 clientType;
    int fd;                  /* decoder device file */
    int fd_mem;              /* /dev/mem for mapping */
    int fd_memalloc;         /* linear memory allocator */
    volatile u32 *pRegBase;  /* IO mem base */
    u32 regSize;             /* IO mem size */
    u32 freeLinMem;          /* Start address of free linear memory */
    u32 freeRefFrmMem;       /* Start address of free reference frame memory */
    int semid;
    int sigio_needed;
#ifdef INTERNAL_TEST
    FILE *regDump;
#endif
} hX170dwl_t;

i32 DWLWaitPpHwReady(const void *instance, u32 timeout);
i32 DWLWaitDecHwReady(const void *instance, u32 timeout);
#endif
