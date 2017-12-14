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
--  Description : Locking semaphore for hardware sharing
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_linux_lock.c,v $
--  $Revision: 1.2 $
--  $Date: 2007/05/10 11:33:07 $
--
------------------------------------------------------------------------------*/

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

#include "dwl_linux_lock.h"

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

/* Obtain a binary semaphore's ID, allocating if necessary.  */

int binary_semaphore_allocation(key_t key, int sem_flags)
{
    return semget(key, 2, sem_flags);
}

/* Deallocate a binary semaphore.  All users must have finished their
   use.  Returns -1 on failure.  */

int binary_semaphore_deallocate(int semid)
{
    union semun ignored_argument;

    return semctl(semid, 1, IPC_RMID, ignored_argument);
}

/* Wait on a binary semaphore.  Block until the semaphore value is
   positive, then decrement it by one.  */

int binary_semaphore_wait(int semid, int sem_num)
{
    struct sembuf operations[1];
    int ret;
    
    /* Use 'sem_num' semaphore from the set.  */
    operations[0].sem_num = sem_num;
    /* Decrement by 1.  */
    operations[0].sem_op = -1;
    /* Permit undo'ing.  */
    operations[0].sem_flg = SEM_UNDO;
    
    /* signal safe */
    do {
        ret = semop(semid, operations, 1);
    } while (ret != 0 && errno == EINTR);
    
    return ret;
}

/* Post to a binary semaphore: increment its value by one.  This
   returns immediately.  */

int binary_semaphore_post(int semid, int sem_num)
{
    struct sembuf operations[1];
    int ret;
    
    /* Use 'sem_num' semaphore from the set.  */
    operations[0].sem_num = sem_num;
    /* Increment by 1.  */
    operations[0].sem_op = 1;
    /* Permit undo'ing.  */
    operations[0].sem_flg = SEM_UNDO;

    /* signal safe */
    do {
        ret = semop(semid, operations, 1);
    } while (ret != 0 && errno == EINTR);
    
    return ret;
}

/* Initialize a binary semaphore with a value of one.  */

int binary_semaphore_initialize(int semid)
{
    union semun argument;
    unsigned short values[2];

    values[0] = 1;
    values[1] = 1;
    argument.array = values;
    return semctl(semid, 0, SETALL, argument);
}
