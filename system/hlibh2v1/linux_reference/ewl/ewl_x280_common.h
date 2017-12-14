/*------------------------------------------------------------------------------
--                                                                                                                     
--       This software is confidential and proprietary and may be used                            
--        only as expressly authorized by a licensing agreement from                             
--                                                                                                                           
--                            Verisilicon.                                                                                   
--                                                                                                                             
--                   (C) COPYRIGHT 2014 VERISILICON                                                          
--                            ALL RIGHTS RESERVED                                                                  
--                                                                                                                             
--                 The entire notice above must be reproduced                                               
--                  on all copies and should not be removed.                                                  
--                                                                                                                             
--------------------------------------------------------------------------------
--
--  Abstract : H2 Encoder Wrapper Layer for OS services
--
------------------------------------------------------------------------------*/
#ifndef __H2_EWL_X280_COMMON_H__
#define __H2_EWL_X280_COMMON_H__

#include <stdio.h>
#include <signal.h>
#include <semaphore.h>

//#include "InfotmMedia.h"
//#include "IM_buffallocapi.h"

extern FILE *h2_fEwl;

/* Macro for debug printing */
#undef PTRACE
#ifdef TRACE_EWL
#   include <stdio.h>
#   define PTRACE(...) if (h2_fEwl) {fprintf(h2_fEwl,"%s:%d:",__FILE__,__LINE__);fprintf(h2_fEwl,__VA_ARGS__);}
#else
#   define PTRACE(...)  //printf(__VA_ARGS__);/* no trace */
#endif

#define ASIC_STATUS_SLICE_READY         0x100
#define ASIC_STATUS_TIMEOUT             0x40
#define ASIC_STATUS_BUFF_FULL           0x20
#define ASIC_STATUS_RESET               0x10
#define ASIC_STATUS_ERROR               0x08
#define ASIC_STATUS_FRAME_READY         0x04
#define ASIC_IRQ_LINE                   0x01

#define ASIC_STATUS_ALL     (ASIC_STATUS_SLICE_READY |\
                             ASIC_STATUS_TIMEOUT |\
                             ASIC_STATUS_FRAME_READY | \
                             ASIC_STATUS_BUFF_FULL | \
                             ASIC_STATUS_RESET | \
                             ASIC_STATUS_ERROR)

/* the encoder device driver nod */
#ifndef MEMALLOC_MODULE_PATH
#define MEMALLOC_MODULE_PATH        "/tmp/dev/memalloc"
#endif

#define ENC_MODULE_PATH             "/dev/HX280_HEVC_ENC"

#ifndef SDRAM_LM_BASE
#define SDRAM_LM_BASE               0x00000000
#endif

/* EWL internal information for Linux */
typedef struct
{
    u32 clientType;
    int fd_mem;              /* /dev/mem */
    int fd_enc;              /* /dev/hx280 */
    int fd_memalloc;         /* /dev/memalloc */
    //ALCCTX alc_inst;          /* infotm linear allocator */
    u32 regSize;             /* IO mem size */
    u32 regBase;
    volatile u32 *pRegBase;  /* IO mem base */
    int semid;
    int sigio_needed;
    int mmu_used;
} hx280ewl_t;

#endif /* __EWLX280_COMMON_H__ */
