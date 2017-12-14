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
--  Abstract : Jpeg Encoder testbench
--
------------------------------------------------------------------------------*/
#ifndef _JPEGTESTBENCH_H_
#define _JPEGTESTBENCH_H_

#include "basetype.h"

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* Maximum lenght of the file path */
#ifndef MAX_PATH
#define MAX_PATH   256
#endif

#define DEFAULT -255

/* Structure for command line options */
typedef struct
{
    char input[MAX_PATH];
    char output[MAX_PATH];
    char inputThumb[MAX_PATH];
    i32 firstPic;
    i32 lastPic;
    i32 width;
    i32 height;
    i32 lumWidthSrc;
    i32 lumHeightSrc;
    i32 horOffsetSrc;
    i32 verOffsetSrc;
    i32 restartInterval;
    i32 frameType;
    i32 colorConversion;
    i32 rotation;
    i32 partialCoding;
    i32 codingMode;
    i32 markerType;
    i32 qLevel;
    i32 unitsType;
    i32 xdensity;
    i32 ydensity;
    i32 thumbnail;
    i32 widthThumb;
    i32 heightThumb;
    i32 lumWidthSrcThumb;
    i32 lumHeightSrcThumb;
    i32 horOffsetSrcThumb;
    i32 verOffsetSrcThumb;
    i32 write;
    i32 comLength;
    char com[MAX_PATH];
}
commandLine_s;

#endif
