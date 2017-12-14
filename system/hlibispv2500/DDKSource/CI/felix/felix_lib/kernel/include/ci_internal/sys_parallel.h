/**
*******************************************************************************
 @file sys_parallel.h

 @brief Define the lock and semaphore abstraction classes

 Each function is implemented in ci_parallel_kernel.c or ci_parallel_posix.c

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
#ifndef SYS_PARALLEL_H_
#define SYS_PARALLEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <img_types.h>

#if defined(FELIX_FAKE) && defined(IMG_KERNEL_MODULE)
#error "Posix and kernel cannot be both used at the same time!"
#endif

#ifdef FELIX_FAKE

#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    pthread_mutex_t mutex;
    /** @brief only used if NDEBUG is not defined */
    IMG_BOOL8 bLocked;
    /**
     * @brief use to check on recursive locking only when NDEBUG is not
     * defined
     */
    pthread_t owner;
} SYS_LOCK;

typedef sem_t SYS_SEM;

// not the same structure to have type checking
typedef struct
{
    SYS_LOCK sLock;
} SYS_SPINLOCK;

#define __acquires(A)
#define __releases(A)

#elif IMG_KERNEL_MODULE

#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>

typedef struct mutex SYS_LOCK;
typedef struct semaphore SYS_SEM;
typedef struct
{
    spinlock_t sLock;
    unsigned long flags;
} SYS_SPINLOCK;

#else
#error "no alternative to posix or kernel"
#endif  // FELIX_FAKE

// CI_KERNEL_TOOLS defined in sys_mem.h
/**
 * @addtogroup CI_KERNEL_TOOLS
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_KERNEL_TOOLS (defined in sys_mem.h)
 *---------------------------------------------------------------------------*/

/**
 * @name Synchronisation (not interrupt-safe)
 * @brief Lock and semaphore that may sleep (not safe to use for ressources
 * shared with interrupt handler).
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Synchronisation documentation module
 *---------------------------------------------------------------------------*/

/**
 * @brief Initialise a lock object
 *
 * @warning does NOT creates a re-entrant lock
 */
IMG_RESULT SYS_LockInit(SYS_LOCK *pLock);

/**
 * @brief Destroys a lock object
 * @warning if LockCreate was called use need to call IMG_FREE on it
 */
IMG_RESULT SYS_LockDestroy(SYS_LOCK *pLock);

/**
 * @brief Acquire the lock
 *
 * @warning does NOT uses re-entrant lock
 */
IMG_RESULT SYS_LockAcquire(SYS_LOCK *pLock) __acquires(pLock);

/**
 * @brief Release the lock
 *
 * @warning does NOT uses re-entrant lock
 */
IMG_RESULT SYS_LockRelease(SYS_LOCK *pLock) __releases(pLock);

/**
 * @brief Create a semaphore object and initialise it
 */
IMG_RESULT SYS_SemInit(SYS_SEM *ppSem, int i32InitValue);

/**
 * @brief Increments the value of the semaphore (and wakes up waiting
 * processes)
 */
IMG_RESULT SYS_SemIncrement(SYS_SEM *pSem);

/**
 * @brief Sleeps until a semaphore value can be tacken
 */
IMG_RESULT SYS_SemDecrement(SYS_SEM *pSem);

/**
 * @brief Sleeps until a semaphore value can be taken or timeoutMs ms passed
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_TIMEOUT on timeout
 * @return IMG_ERROR_FATAL on other reasons the decrement failed
 */
IMG_RESULT SYS_SemTimedDecrement(SYS_SEM *pSem, unsigned int timeoutMs);

/**
 * @brief Try to decrement the semaphore but does not sleep.
 *
 * @return IMG_SUCCESS if semaphore was decremented
 * @return IMG_ERROR_CANCELLED if the semaphore could not be acquired
 */
IMG_RESULT SYS_SemTryDecrement(SYS_SEM *pSem);

/**
 * @brief Destroys the semaphore
 * @warning destroying a semaphore some processes are waiting on has
 * undefined effect
 */
IMG_RESULT SYS_SemDestroy(SYS_SEM *pSem);

/**
 * @}
 * @name Synchronisation (interrupt-safe)
 * @brief Locks that are safe to use when ressource is shared with the
 * interrupt handler (spinlock)
 * @{
 */

/**
 * @brief Initialise a lock object
 *
 * @warning does NOT creates a re-entrant lock
 */
IMG_RESULT SYS_SpinlockInit(SYS_SPINLOCK *pLock);

/**
 * @brief Destroys a lock object
 * @warning if LockCreate was called use need to call IMG_FREE on it
 */
IMG_RESULT SYS_SpinlockDestroy(SYS_SPINLOCK *pLock);

/**
 * @brief Acquire the lock - interrupts are safely disabled beforehand
 *
 * @warning does NOT uses re-entrant lock
 */
IMG_RESULT SYS_SpinlockAcquire(SYS_SPINLOCK *pLock)
    __acquires(&(pLock->sLock));

/**
 * @brief Release the lock - interrupts are restored after release
 *
 * @warning does NOT uses re-entrant lock
 */
IMG_RESULT SYS_SpinlockRelease(SYS_SPINLOCK *pLock)
    __releases(&(pLock->sLock));



/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Syncrhonisation block
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_KERNEL_TOOLS documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SYS_PARALLEL_H_ */
