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
--  Abstract : Stream decoding utilities
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "bqueue.h"
#include "dwl.h"
#ifndef HANTRO_OK
#define HANTRO_OK (0)
#endif /* HANTRO_TRUE */

#ifndef HANTRO_NOK
#define HANTRO_NOK (1)
#endif /* HANTRO_FALSE*/

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    BqueueInit
        Initialize buffer queue
------------------------------------------------------------------------------*/
u32 BqueueInit(struct BufferQueue *bq, u32 num_buffers) {
  u32 i;

  if (num_buffers == 0) return HANTRO_OK;

  bq->pic_i = (u32 *)DWLmalloc(sizeof(u32) * num_buffers);
  if (bq->pic_i == NULL) {
    return HANTRO_NOK;
  }
  for (i = 0; i < num_buffers; ++i) {
    bq->pic_i[i] = 0;
  }
  bq->queue_size = num_buffers;
  bq->ctr = 1;

  return HANTRO_OK;
}

/*------------------------------------------------------------------------------
    BqueueRelease
------------------------------------------------------------------------------*/
void BqueueRelease(struct BufferQueue *bq) {
  if (bq->pic_i) {
    DWLfree(bq->pic_i);
    bq->pic_i = NULL;
  }
  bq->prev_anchor_slot = 0;
  bq->queue_size = 0;
}

/*------------------------------------------------------------------------------
    BqueueNext
        Return "oldest" available buffer.
------------------------------------------------------------------------------*/
u32 BqueueNext(struct BufferQueue *bq, u32 ref0, u32 ref1, u32 ref2,
               u32 b_pic) {
  u32 min_pic_i = 1 << 30;
  u32 next_out = (u32)0xFFFFFFFFU;
  u32 i;
  /* Find available buffer with smallest index number  */
  i = 0;

  while (i < bq->queue_size) {
    if (i == ref0 || i == ref1 || i == ref2) /* Skip reserved anchor pictures */
    {
      i++;
      continue;
    }
    if (bq->pic_i[i] < min_pic_i) {
      min_pic_i = bq->pic_i[i];
      next_out = i;
    }
    i++;
  }

  if (next_out == (u32)0xFFFFFFFFU) {
    return 0; /* No buffers available, shouldn't happen */
  }

  /* Update queue state */
  if (b_pic) {
    bq->pic_i[next_out] = bq->ctr - 1;
    bq->pic_i[bq->prev_anchor_slot]++;
  } else {
    bq->pic_i[next_out] = bq->ctr;
  }
  bq->ctr++;
  if (!b_pic) {
    bq->prev_anchor_slot = next_out;
  }

  return next_out;
}

/*------------------------------------------------------------------------------
    BqueueDiscard
        "Discard" output buffer, e.g. if error concealment used and buffer
        at issue is never going out.
------------------------------------------------------------------------------*/
void BqueueDiscard(struct BufferQueue *bq, u32 buffer) {
  bq->pic_i[buffer] = 0;
}
