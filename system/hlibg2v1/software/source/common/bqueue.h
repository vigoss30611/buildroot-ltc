/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : Header file for stream decoding utilities
--
------------------------------------------------------------------------------*/

#ifndef BQUEUE_H_DEFINED
#define BQUEUE_H_DEFINED

#include "basetype.h"

struct BufferQueue {
  u32 *pic_i;
  u32 ctr;
  u32 queue_size;
  u32 prev_anchor_slot;
};

#define BQUEUE_UNUSED (u32)(0xffffffff)

u32 BqueueInit(struct BufferQueue *bq, u32 num_buffers);
void BqueueRelease(struct BufferQueue *bq);
u32 BqueueNext(struct BufferQueue *bq, u32 ref0, u32 ref1, u32 ref2, u32 b_pic);
void BqueueDiscard(struct BufferQueue *bq, u32 buffer);

#endif /* BQUEUE_H_DEFINED */
