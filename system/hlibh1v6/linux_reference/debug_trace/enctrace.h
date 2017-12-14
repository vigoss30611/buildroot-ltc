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
--  Description : Internal traces
--
------------------------------------------------------------------------------*/

#ifndef __ENCTRACE_H__
#define __ENCTRACE_H__

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "encpreprocess.h"
#include "encasiccontroller.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

void EncTraceAsicParameters(asicData_s * asic);
void EncTracePreProcess(preProcess_s * preProcess);
void EncTraceSegments(u32 *map, i32 bytes, i32 enable, i32 mapModified,
        i32* cnt, i32 *qp);
void EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum);
void EncTraceProbs(u16 *ptr, i32 bytes);

void EncDumpMem(u32 * ptr, u32 bytes, char *desc);
void EncDumpRecon(asicData_s * asic);

void EncTraceCloseAll(void);

#endif
