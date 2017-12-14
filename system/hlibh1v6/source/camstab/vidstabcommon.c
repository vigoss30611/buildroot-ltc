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
-
-  Description : Video stabilization common stuff for standalone and pipeline
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vidstabcommon.h"

/*------------------------------------------------------------------------------
    Function name   : VSReadStabData
    Description     : 
    Return type     : void 
    Argument        : const u32 * regMirror
    Argument        : HWStabData * hwStabData
------------------------------------------------------------------------------*/
void VSReadStabData(const u32 * regMirror, HWStabData * hwStabData)
{
    i32 i;
    u32 *matrix;
    const u32 *reg;

    hwStabData->rMotionMin = (regMirror[40] & ((1 << 24) - 1));
    hwStabData->rMotionSum = regMirror[41];
    if (hwStabData->rMotionSum*8 > hwStabData->rMotionSum)
        hwStabData->rMotionSum *= 8;
    else
        hwStabData->rMotionSum = 0xFFFFFFFF;

    hwStabData->rGmvX = ((i32) (regMirror[42]) >> 26);
    hwStabData->rGmvY = ((i32) (regMirror[43]) >> 26);

#ifdef TRACE_VIDEOSTAB_INTERNAL
    DEBUG_PRINT(("%8d %6d %4d %4d", hwStabData->rMotionSum,
                 hwStabData->rMotionMin, hwStabData->rGmvX, hwStabData->rGmvY));
#endif

    matrix = hwStabData->rMatrixVal;
    reg = &regMirror[42];

    for(i = 9; i > 0; i--)
    {
        *matrix++ = (*reg++) & ((1 << 24) - 1);

#ifdef TRACE_VIDEOSTAB_INTERNAL
        DEBUG_PRINT((" %6d", matrix[-1]));
#endif

    }

#ifdef TRACE_VIDEOSTAB_INTERNAL
    DEBUG_PRINT(("\n"));
#endif

}
