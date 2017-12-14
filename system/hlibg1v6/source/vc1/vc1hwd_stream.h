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
--  Description : Stream buffer handling
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_stream.h,v $
--  $Revision: 1.4 $
--  $Date: 2007/12/13 13:27:45 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_STREAM_H
#define VC1HWD_STREAM_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

typedef struct
{
    u8  *pStrmBuffStart;    /* pointer to start of stream buffer */
    u8  *pStrmCurrPos;      /* current read address in stream buffer */
    u32  bitPosInWord;      /* bit position in stream buffer byte */
    u32  strmBuffSize;      /* size of stream buffer (bytes) */
    u32  strmBuffReadBits;  /* number of bits read from stream buffer */
    u32  strmExhausted;     /* attempted to read more bits from the stream
                             * than available. */
    u32  removeEmulPrevBytes;
} strmData_t;

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

u32 vc1hwdGetBits(strmData_t *pStrmData, u32 numBits);

u32 vc1hwdShowBits(strmData_t *pStrmData, u32 numBits );

u32 vc1hwdFlushBits(strmData_t *pStrmData, u32 numBits);

u32 vc1hwdIsExhausted(const strmData_t * const);

#endif /* #ifndef VC1HWD_STREAM_H */

