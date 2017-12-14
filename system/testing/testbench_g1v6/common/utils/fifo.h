/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2012 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--                                                                            --
-- Description: Synchronized FIFO queue implementation.                       --
--                                                                            --
------------------------------------------------------------------------------*/

#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"

/* FIFO_DATATYPE must be defined to hold specific type of pointers. If it is not
 * defined, by default the datatype will be void. */
#ifndef FIFO_DATATYPE
#define FIFO_DATATYPE void
#endif  /* FIFO_DATATYPE */

typedef enum
{
    FIFO_OK,
    FIFO_ERROR_MEMALLOC
} fifo_ret;

typedef void* fifo_inst;
typedef void* fifo_object;

/* fifo_init initializes the queue.
 * |num_of_slots| defines how many slots to reserve at maximum.
 * |pInstance| is output parameter holding the instance. */
fifo_ret fifo_init(u32 num_of_slots, fifo_inst* instance);

/* fifo_push pushes an object to the back of the queue. Ownership of the
 * contained object will be moved from the caller to the queue. Returns number
 * of elements in the queue after the push.
 *
 * |inst| is the instance push to.
 * |object| holds the pointer to the object to push into queue. */
u32 fifo_push(fifo_inst inst, fifo_object* object);

/* fifo_pop returns object from the front of the queue. Ownership of the popped
 * object will be moved from the queue to the caller. Returns number of elements
 * in the queue after the pop.
 *
 * |inst| is the instance to pop from.
 * |object| holds the pointer to the object popped from the queue. */
u32 fifo_pop(fifo_inst inst, fifo_object* object);

/* Ask how many objects there are in the fifo. */
u32 fifo_count(fifo_inst inst);

/* fifo_release releases and deallocated queue. User needs to make sure the
 * queue is empty and no threads are waiting in fifo_push or fifo_pop.
 * |inst| is the instance to release. */
void fifo_release(fifo_inst inst);
#endif  /* __FIFO_H__ */

