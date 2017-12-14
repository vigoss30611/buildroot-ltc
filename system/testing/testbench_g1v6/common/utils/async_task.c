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
------------------------------------------------------------------------------*/

#include "async_task.h"
#include "dwlthread.h"
#include <stdlib.h>
#include <assert.h>

async_task run_task(task_func func, void* param)
{
    int ret;
    pthread_t* thread_handle = malloc(sizeof(pthread_t));
    ret = pthread_create(thread_handle, NULL, func, param);
    assert(ret == 0);

    if(ret != 0)
    {
        free(thread_handle);
        thread_handle = NULL;
    }

    return thread_handle;
}

void wait_for_task_completion(async_task task)
{
    int ret = pthread_join(*((pthread_t*)task), NULL);
    assert(ret == 0);
    free(task);
}

