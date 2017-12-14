/**
******************************************************************************
@file sys_parallel_kernel.c

@brief implementation of SYS_Lock SYS_Spinlock and SYS_Semaphore for Linux
 kernel

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "ci_internal/sys_parallel.h"

#ifndef IMG_KERNEL_MODULE
#error "including ci_parallel_kernel in sources while not using kernel"
#endif

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <linux/mutex.h>
#include <linux/semaphore.h>

IMG_RESULT SYS_LockInit(SYS_LOCK *pLock)
{
    mutex_init(pLock);

    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockDestroy(SYS_LOCK *pLock)
{
    // no real destroy
    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockAcquire(SYS_LOCK *pLock)
{
    mutex_lock(pLock);
    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockRelease(SYS_LOCK *pLock)
{
    mutex_unlock(pLock);
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SpinlockInit(SYS_SPINLOCK *pLock)
{
    spin_lock_init(&(pLock->sLock));
    pLock->flags = 0;
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SpinlockDestroy(SYS_SPINLOCK *pLock)
{
    if (pLock->flags)
    {
        printk(KERN_WARNING "destroying a locked spinlock\n");
    }
    return IMG_SUCCESS;
}

/* for some reasons we need to duplicate the acquire here otherwise
 * sparse complains that an unexpected count is met
 * while the acquires is not needed for the normal lock */
IMG_RESULT SYS_SpinlockAcquire(SYS_SPINLOCK *pLock)
    __acquires(&(pLock->sLock))
{
    spin_lock_irqsave(&(pLock->sLock), pLock->flags);
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SpinlockRelease(SYS_SPINLOCK *pLock)
    __releases(&(pLock->sLock))
{
    spin_unlock_irqrestore(&(pLock->sLock), pLock->flags);
    pLock->flags = 0;
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemInit(SYS_SEM *pSem, int i32InitValue)
{
    sema_init(pSem, i32InitValue);
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemIncrement(SYS_SEM *pSem)
{
    up(pSem);
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemDecrement(SYS_SEM *pSem)
{
    int r = 0;
    r = down_interruptible(pSem);
    if (-EINTR == r)
    {
        return IMG_ERROR_INTERRUPTED;
    }
    else if (r)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemTimedDecrement(SYS_SEM *pSem, unsigned int timeoutMs)
{
    long jiffiesToWait = timeoutMs * HZ / 1000;
    int r = 0;

    r = down_timeout(pSem, jiffiesToWait);
    if (r)
    {
        if (-ETIME == r)
        {
            return IMG_ERROR_TIMEOUT;
        }
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemTryDecrement(SYS_SEM *pSem)
{
    int r = down_trylock(pSem);
    if (r)
    {
        return IMG_ERROR_CANCELLED;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemDestroy(SYS_SEM *pSem)
{
  // no real destructor
  return IMG_SUCCESS;
}
