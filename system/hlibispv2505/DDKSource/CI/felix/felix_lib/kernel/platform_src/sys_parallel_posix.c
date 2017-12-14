/**
*******************************************************************************
@file sys_parallel_posix.c

@brief

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

/* using posix */

#ifndef FELIX_FAKE
#error "including ci_parallel_posix in sources while not using fake device"
#endif

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

/*#define debug_lock(...) fprintf(stderr, "%s: (thd=%lx)-", __FUNCTION__,\
    get_tid()); fprintf(stderr, __VA_ARGS__); fflush(stderr);*/
#define debug_lock(...)
#define fatal_lock(...) fprintf(stderr, "%s: (thd=0x%"IMG_PTRDPR"x)-",\
    __FUNCTION__, get_tid()); fprintf(stderr, __VA_ARGS__); fflush(stderr);

#ifdef __GNUC__
#include <unistd.h>
#include <execinfo.h> // to have back trace information

#define PRINT_BT(N)                                     \
    {                                                   \
        int n;                                          \
        void *btElem[N];                                \
        n = backtrace(btElem, N);                       \
        backtrace_symbols_fd(btElem, n, STDERR_FILENO); \
    }
#else
#define PRINT_BT(...)
#endif

IMG_UINTPTR get_tid()
{
// using pthread for WIN32 pthread_t has 2 elements, one of which is a pointer
#ifdef WIN32 
    return (IMG_UINTPTR)pthread_self().p;
#else
    return (IMG_UINTPTR)pthread_self();
#endif
}

/*
 * The elements defined using bLocked and pthread_t are only used when in
 * debug (i.e. when NDEBUG is not defined).
 * Their access are not thread safe but can help debugging potential deadlocks.
 */

IMG_RESULT SYS_LockInit(SYS_LOCK *pLock)
{
    int r;
    debug_lock("%p\n", *pLock);
    pLock->bLocked = IMG_FALSE;
    // manual says: always returns 0 but we check anyway
    r = pthread_mutex_init(&(pLock->mutex), NULL);
    if (r)
    {
        r = errno;
        fatal_lock("Failed to create posix mutex (errno %d)\n", r);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockDestroy(SYS_LOCK *pLock)
{
    int r;
    debug_lock("%p\n", get_tid(), __FUNCTION__, pLock);

    if (pLock->bLocked)
    {
    fatal_lock("deleting locked lock\n");
    PRINT_BT(10);
    }

    r = pthread_mutex_destroy(&(pLock->mutex));
    if (r)
    {
        r = errno;
        fatal_lock("Failed to destroy a lock (errno %d)\n", r);
        PRINT_BT(10);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockAcquire(SYS_LOCK *pLock)
{
    int r;
    debug_lock("%p\n", pLock);

    if(pLock->bLocked && pthread_equal(pthread_self(), pLock->owner))
    {
        fatal_lock("relocking owned lock: DEADLOCK\n");
        PRINT_BT(10);
        //return IMG_ERROR_FATAL;
    }

    r = pthread_mutex_lock(&(pLock->mutex));
    if (r)
    {
        r = errno;
        fatal_lock("Failed to lock a posix mutex (errno %d)\n", r);
        return IMG_ERROR_FATAL;
    }
    pLock->owner=pthread_self();
    pLock->bLocked = IMG_TRUE;
    return IMG_SUCCESS;
}

IMG_RESULT SYS_LockRelease(SYS_LOCK *pLock)
{
    int r;
    debug_lock("%p\n", pLock);

    if(pLock->bLocked!=IMG_TRUE)
    {
        fatal_lock("release a non-locked lock\n");
        PRINT_BT(10);
        return IMG_ERROR_FATAL;
    }
    if (!pthread_equal(pthread_self(), pLock->owner))
    {
        fatal_lock("releasing some other thread's lock\n");
        PRINT_BT(10);
        //return IMG_ERROR_FATAL; // fast mutex should not check ownership
    }

    pLock->bLocked = IMG_FALSE;
    r = pthread_mutex_unlock(&(pLock->mutex));
    if (r)
    {
        pLock->bLocked = IMG_TRUE;
        r = errno;
        fatal_lock("Failed to unlock a posix mutex (errno %d)\n", r);
        return IMG_ERROR_FATAL;
    }
    

    return IMG_SUCCESS;
}

IMG_RESULT SYS_SpinlockInit(SYS_SPINLOCK *pLock)
{
    return SYS_LockInit(&(pLock->sLock));
}

IMG_RESULT SYS_SpinlockDestroy(SYS_SPINLOCK *pLock)
{
    return SYS_LockDestroy(&(pLock->sLock));
}

IMG_RESULT SYS_SpinlockAcquire(SYS_SPINLOCK *pLock)
{
    return SYS_LockAcquire(&(pLock->sLock));
}

IMG_RESULT SYS_SpinlockRelease(SYS_SPINLOCK *pLock)
{
    return SYS_LockRelease(&(pLock->sLock));
}

IMG_RESULT SYS_SemInit(SYS_SEM *pLock, int i32InitVal)
{
    int r = 0;
    r = sem_init(pLock, 0, i32InitVal);
    if (r)
    {
        // init value is too big
        return r == EINVAL ? IMG_ERROR_INVALID_PARAMETERS : IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemIncrement(SYS_SEM *pSem)
{
    int r = 0;
    r = sem_post(pSem);
    if (r)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemDecrement(SYS_SEM *pSem)
{
    int r = 0;
    r = sem_wait(pSem);
    if (r)
    {
        r = errno;
        fatal_lock("Failed to decrement sem (errno = %d)\n", r);
        if (-EINTR == r)
        {
            return IMG_ERROR_INTERRUPTED;
        }
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemTimedDecrement(SYS_SEM *pSem, unsigned int timeoutMs)
{
    int r = 0;
    struct timespec timeout;

    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;//timeoutMs*1000000;
    while ( timeoutMs >= 1000 )
    {
        timeout.tv_sec++;
        timeoutMs -= 1000;
    }
    timeout.tv_nsec = timeoutMs*1000000;

    r = sem_timedwait(pSem, &timeout);
    if (r)
    {
        r = errno;
        if ( r != ETIMEDOUT )
        {
            //fatal_lock("Failed to decrement sem (errno = %d)\n", r);
            return IMG_ERROR_FATAL;
        }
        return IMG_ERROR_TIMEOUT;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemTryDecrement(SYS_SEM *pSem)
{
    int r = sem_trywait(pSem);
    if (r)
    {
        return IMG_ERROR_CANCELLED;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_SemDestroy(SYS_SEM *pSem)
{
    int r = 0;
    r = sem_destroy(pSem);
    if (r)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}
