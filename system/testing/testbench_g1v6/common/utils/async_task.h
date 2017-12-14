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
--  Description : Generic interface to perform asynchronous tasks and wait for
--                completion.
--
------------------------------------------------------------------------------*/
#ifndef __ASYNC_TASK_H__
#define __ASYNC_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"

typedef void* async_task;           /* Handle type to the task. */
typedef void* (*task_func)(void*);  /* Function prototype to run. */

/* Function to run arbitrary |func| in own thread with |param|. Returns
   handle to the user to wait for the task completion. User must call
   wait_for_task_completion for all tasks successfully dispatched through this
   function. */
async_task run_task(task_func func, void* param);

/* Function to wait task completion. Blocks until the |task| is done. */
void wait_for_task_completion(async_task task);

#ifdef __cplusplus
}
#endif

#endif  /* __ASYNC_TASK_H__ */
