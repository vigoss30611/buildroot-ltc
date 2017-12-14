/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Description : Internal traces
--
------------------------------------------------------------------------------*/

#ifndef __ENCTRACE_H__
#define __ENCTRACE_H__

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "base_type.h"
#include "hevcencapi.h"
#include "sw_put_bits.h"
#include "sw_picture.h"
//#include "encpreprocess.h"
#include "encasiccontroller.h"

#include <string.h>


/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/
i32 Enc_test_data_init(void);

void Enc_test_data_release(void);

i32 Enc_open_stream_trace(struct buffer *b);

i32 Enc_close_stream_trace(void);

void Enc_add_comment(struct buffer *b, i32 value, i32 number, char *comment);

// Update frame count and set trace flag accordingly. called when one one frame is done.
void EncTraceUpdateStatus();

// Write reconstruct data.
void EncTraceReconEnd();
int EncTraceRecon(HEVCEncInst inst, i32 poc, char *f_recon);

// TOP trace file generation routines
void EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum);

void EncTraceReferences(struct container *c, struct sw_picture *pic);


#if 0
void EncTraceAsicParameters(asicData_s *asic);
void EncTracePreProcess(preProcess_s *preProcess);
void EncTraceSegments(u32 *map, i32 bytes, i32 enable, i32 mapModified,
                      i32 *cnt, i32 *qp);
void EncTraceProbs(u16 *ptr, i32 bytes);

void EncDumpMem(u32 *ptr, u32 bytes, char *desc);
void EncDumpRecon(asicData_s *asic);

void EncTraceCloseAll(void);
#endif

#endif
