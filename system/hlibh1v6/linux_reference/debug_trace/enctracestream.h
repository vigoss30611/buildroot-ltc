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
--  Abstract  : Stream tracing
--
------------------------------------------------------------------------------*/
#ifndef __ENCTRACESTREAM_H__
#define __ENCTRACESTREAM_H__

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"

#include <stdio.h>

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

typedef struct {
    FILE *file;		    /* File where the trace is written */
    i32 bitCnt;		    /* Stream bit count from the beginning of file */
    i32 frameNum;	    /* Frame number */
    i32 disableStreamTrace; /* Don't write stream trace when disabled */
    i32 id;		    /* ID of stream generation, eg. 0=SW, 1=HW */
} traceStream_s;

extern traceStream_s traceStream;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

i32 EncOpenStreamTrace(const char *filename);
void EncCloseStreamTrace(void);

void EncTraceStream(i32 value, i32 numberOfBits);
void EncComment(const char *comment);
void EncCommentMbType(const char *comment, i32 mbNum);

#endif
