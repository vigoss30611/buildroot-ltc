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
--  Abstract : Header file for stream decoding utilities
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: bqueue.h,v $
--  $Date: 2010/02/25 12:18:56 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef BQUEUE_H_DEFINED
#define BQUEUE_H_DEFINED

#include "basetype.h"

typedef struct
{
    u32 *picI;
    u32 ctr;
    u32 queueSize;
    u32 prevAnchorSlot;
} bufferQueue_t;

#define BQUEUE_UNUSED (u32)(0xffffffff)

u32  BqueueInit( bufferQueue_t *bq, u32 numBuffers );
void BqueueRelease( bufferQueue_t *bq );
u32  BqueueNext( bufferQueue_t *bq, u32 ref0, u32 ref1, u32 ref2, u32 bPic );
void BqueueDiscard( bufferQueue_t *bq, u32 buffer );

#endif /* BQUEUE_H_DEFINED */
