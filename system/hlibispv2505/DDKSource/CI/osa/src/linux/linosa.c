/*!
******************************************************************************
 @file   :  osa.h

 @brief    OS Abstraction Layer (OSA)

 @Author Imagination Technologies

 @date   31/01/2012

         <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         OS Adaption Layer (OSA)

******************************************************************************/

#include "osa.h"
#include <pthread.h>
#include <time.h>
#include <sys/prctl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

#include "img_errors.h"
#include "img_defs.h"

pthread_once_t          gsOneTimeInit = PTHREAD_ONCE_INIT;
pthread_key_t           gsTlsKey = 0;
/*!
******************************************************************************sThreadSyncParamssThreadSyncParams

 @Struct                 sThreadFuncParams

 @Description

 Combines OSA_pfnThreadFunc and parameters of new thread.
 Necessary to pass both those values to wrapper function(OSA_ThreadFuncWrapper)

******************************************************************************/
typedef struct
{
    OSA_pfnThreadFunc  pfnStartFunc;
    IMG_VOID *         pvTaskParam;
    IMG_CHAR *         pszName;

} sThreadFuncParams;

/*!
******************************************************************************

 @Struct                 sThreadInfo

 @Description

 Combines thread ID and its function and params.

******************************************************************************/
typedef struct
{

    pthread_t          hThread;
    sThreadFuncParams  sFuncParam;

} sThreadInfo;


/*!
******************************************************************************

 @Struct                 sThreadSyncParams

 @Description

 Structure that describes thread sync object

******************************************************************************/
typedef struct
{
    IMG_UINT32        ui32SignalCounter;/*!< Current signal state.                         */
    pthread_cond_t    hCond;             /*!< Condition variable.                           */
    pthread_mutex_t   hMutex;            /*!< Mutex correlated with cond.                   */

} sThreadSyncParams;

/*!
******************************************************************************

 @Struct                 sThreadConditionParams

 @Description

 Structure that describes thread condition object

******************************************************************************/
typedef struct
{
    pthread_cond_t    hCond;             /*!< Condition variable.                           */
    pthread_mutex_t   hMutex;            /*!< Mutex correlated with cond.                   */

} sThreadConditionParams;


/*!
******************************************************************************

 @Function                osa_ThreadFuncWrapper

 @Description

 Wrapper function that translates OSA_pfnThreadFunc to posix thread equivalent.

 @Input        pvFuncAndParams : Pair of new thread function and its params.

 @Return    IMG_VOID *        : Thread exit value.

******************************************************************************/
static IMG_VOID * osa_ThreadFuncWrapper(
    void *  pvFuncAndParams
)
{
    sThreadFuncParams * pFuncParamPair = (sThreadFuncParams*)pvFuncAndParams;

    IMG_ASSERT(pFuncParamPair != NULL);
    if(pFuncParamPair == NULL)
    {
        return NULL;
    }
#ifdef PR_SET_NAME // should be present on linux kernel > 2.6.9 but sometime is not
    prctl(PR_SET_NAME, (unsigned long)pFuncParamPair->pszName, 0, 0, 0);//set thread name, to see it use "ps -Leo comm"
#endif
    pFuncParamPair->pfnStartFunc(pFuncParamPair->pvTaskParam);

    return NULL;
}


/*!
******************************************************************************

 @Function                OSA_CritSectCreate

******************************************************************************/
IMG_RESULT OSA_CritSectCreate(
    IMG_HANDLE * const  phCritSect
)
{
    pthread_mutex_t * psMutex;
    pthread_mutexattr_t sMutexAttr;

    if(pthread_mutexattr_init(&sMutexAttr) != 0)
    {
        IMG_ASSERT(0 && "Mutex attributes initialization failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_mutexattr_settype(&sMutexAttr, PTHREAD_MUTEX_RECURSIVE) != 0)//set mutex attributes to check errors
    {
        IMG_ASSERT(0 && "Mutex setting type failed");
        return IMG_ERROR_FATAL;
    }

    psMutex = IMG_MALLOC(sizeof(pthread_mutex_t));
    IMG_ASSERT(psMutex != NULL);

    if(psMutex == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    if(pthread_mutex_init(psMutex,&sMutexAttr) != 0)
    {
        IMG_FREE(psMutex);
        IMG_ASSERT(0 && "Mutex initialization failed");
        return IMG_ERROR_FATAL;
    }

    if(pthread_mutexattr_destroy(&sMutexAttr) != 0)
    {
        IMG_FREE(psMutex);
        IMG_ASSERT(0 && "Mutex attribute deinitialization failed");
        return IMG_ERROR_FATAL;
    }

    *phCritSect = (IMG_HANDLE) psMutex;//save the pointer to pthread mutex structure as a handler

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_CritSectDestroy

******************************************************************************/
IMG_VOID OSA_CritSectDestroy(
    IMG_HANDLE  hCritSect
)
{
    pthread_mutex_t * psMutex = (pthread_mutex_t *)hCritSect; //the handler is a pointer to mutex structure
	int retVal;

    IMG_ASSERT(psMutex != NULL);

    if(psMutex != NULL)
    {
		retVal = pthread_mutex_destroy(psMutex);
        if(retVal != 0)
        {
            fprintf(stderr,"Mutex destroy failed\n pthread error: %s\n", strerror(retVal));
            IMG_ASSERT(0);
            abort();
        }

        IMG_FREE(psMutex);
    }
}


/*!
******************************************************************************

 @Function                OSA_CritSectUnlock

******************************************************************************/
IMG_VOID OSA_CritSectUnlock(
    IMG_HANDLE  hCritSect
)
{
    pthread_mutex_t * psMutex = (pthread_mutex_t *)hCritSect; //the handler is a pointer to mutex structure
	int retVal;

    IMG_ASSERT(psMutex != NULL);
    if(psMutex != NULL)
    {
		retVal = pthread_mutex_unlock(psMutex);
        if(retVal != 0)
        {
            fprintf(stderr,"Mutex unlock failed\npthread error: %s\n", strerror(retVal));
            IMG_ASSERT(0);
            abort();
        }
    }

}


/*!
******************************************************************************

 @Function                OSA_CritSectLock

******************************************************************************/
IMG_VOID OSA_CritSectLock(
    IMG_HANDLE  hCritSect
)
{
    pthread_mutex_t * psMutex = (pthread_mutex_t *)hCritSect;//the handler is a pointer to mutex structure

    IMG_ASSERT(psMutex != NULL);

    if(psMutex != NULL)
    {
        if(pthread_mutex_lock(psMutex) != 0)
        {
            fprintf(stderr,"Mutex lock failed");
            IMG_ASSERT(0);
            abort();
        }
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncCreate

******************************************************************************/
IMG_RESULT OSA_ThreadSyncCreate(
    IMG_HANDLE * const  phThreadSync
)
{
    sThreadSyncParams * psThreadSync;
    pthread_mutexattr_t sMutexAttr;

    if(pthread_mutexattr_init(&sMutexAttr) != 0)
    {
        IMG_ASSERT(0 && "Internal mutex attributes initialization failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_mutexattr_settype(&sMutexAttr, PTHREAD_MUTEX_ERRORCHECK) != 0)//set mutex attributes to check errors
    {
        IMG_ASSERT(0 && "Internal mutex setting type failed");
        return IMG_ERROR_FATAL;
    }

    psThreadSync = IMG_MALLOC(sizeof(sThreadSyncParams));
    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    if(pthread_mutex_init(&psThreadSync->hMutex, NULL) != 0)//create internal mutex, used to sync access to bSignalled and ui32WaitingTasks variable
    {
        IMG_FREE(psThreadSync);
        IMG_ASSERT(0 && "Internal mutex initialization failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_cond_init(&psThreadSync->hCond, NULL) != 0)//create condition variable, used to signal thread sync object
    {
        IMG_FREE(psThreadSync);
        IMG_ASSERT(0 && "Internal condition variable initialization failed");
        return IMG_ERROR_FATAL;
    }

    //initial state is non-signalled and there are no waiting threads
    psThreadSync->ui32SignalCounter = 0;

    *phThreadSync = (IMG_HANDLE)psThreadSync;//handler is a pointer to thread sync structure

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncDestroy

******************************************************************************/
IMG_VOID OSA_ThreadSyncDestroy(
    IMG_HANDLE  hThreadSync
)
{
    sThreadSyncParams * psThreadSync = (sThreadSyncParams *)hThreadSync;//handler is a pointer to thread sync structure
	int retVal;
    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync == NULL)
    {
        return;
    }

	retVal = pthread_mutex_destroy(&psThreadSync->hMutex);
    if (retVal != 0)
    {
        fprintf(stderr,"Internal mutex deinitialization failed\n pthread error: %s\n", strerror(retVal));
        IMG_ASSERT(0);
        abort();
    }

	retVal = pthread_cond_destroy(&psThreadSync->hCond);
    if(retVal != 0)
    {
        fprintf(stderr,"Internal variable condition deinitialization failed\n pthread error: %s\n", strerror(retVal));
        IMG_ASSERT(0);
        abort();
    }

    IMG_FREE(psThreadSync);
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncSignal

******************************************************************************/
IMG_VOID OSA_ThreadSyncSignal(
    IMG_HANDLE  hThreadSync
)
{
    sThreadSyncParams * psThreadSync = (sThreadSyncParams *)hThreadSync;//handler is a pointer to thread sync structure

    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync == NULL)
    {
        fprintf(stderr,"Passed handler is not a handler to Thread Sync object");
        abort();
    }

    if( pthread_mutex_lock(&psThreadSync->hMutex) != 0)
    {
        fprintf(stderr,"Internal mutex error: probably the thread sync object is not created (or already destroyed)");
        IMG_ASSERT(0);
        abort();
    }

    ++psThreadSync->ui32SignalCounter;//always set to signalled state (even if already signalled)
    pthread_cond_signal(&psThreadSync->hCond);//sent signal that wakes one of waiting threads

    if( pthread_mutex_unlock(&psThreadSync->hMutex)!= 0)
    {
        fprintf(stderr,"Internal mutex error: probably the thread sync object is not created (or already destroyed)");
        IMG_ASSERT(0);
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncWait

******************************************************************************/
IMG_RESULT OSA_ThreadSyncWait(
    IMG_HANDLE  hThreadSync,
    IMG_UINT32  ui32TimeoutMs
)
{
    sThreadSyncParams * psThreadSync = (sThreadSyncParams *)hThreadSync;//handler is a pointer to thread sync structure
    IMG_RESULT ui32Result = IMG_SUCCESS;

    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync == NULL)
    {
        return IMG_ERROR_FATAL;
    }

    if( pthread_mutex_lock(&psThreadSync->hMutex)!= 0)
    {
        IMG_ASSERT(0 && "Internal mutex error: probably the thread sync object is not created (or already destroyed)");
        return IMG_ERROR_FATAL;
    }

    while(psThreadSync->ui32SignalCounter == 0)//if in signalled state there is no reason to wait
    {
        if(ui32TimeoutMs == OSA_NO_TIMEOUT)
        {
            pthread_cond_wait(&psThreadSync->hCond,&psThreadSync->hMutex);//wait without timeout
        }
        else
        {
            struct timespec time;
            struct timeval    tp;

            if(gettimeofday(&tp, NULL) != 0)//getting current time
            {
                IMG_ASSERT(0 && "Getting time failed");
                ui32Result = IMG_ERROR_FATAL;
            }
            else
            {
                int iWaitResult;
                //converting timeval to timespec
                time.tv_sec  = tp.tv_sec;
                time.tv_nsec = tp.tv_usec * 1000;

                //converting elapse time to absolute time
                time.tv_sec += ui32TimeoutMs/1000;
                time.tv_nsec += (ui32TimeoutMs-(ui32TimeoutMs/1000)*1000)*1000000;

                //time.tv_nsec should be less than 1000000000
                if(time.tv_nsec >= 1000000000)
                {
                    ++time.tv_sec;
                    time.tv_nsec -= 1000000000;
                }

                if( (iWaitResult = pthread_cond_timedwait(&psThreadSync->hCond, &psThreadSync->hMutex, &time)) != 0)//wait with timeout
                {
                    IMG_ASSERT(iWaitResult == ETIMEDOUT && "Waiting for signal failed");

                    if(iWaitResult == ETIMEDOUT)//timeout elapsed
                    {
                        if(psThreadSync->ui32SignalCounter != 0)//state could change in the meantime
                        {
                            ui32Result = IMG_SUCCESS;
                        }
                        else
                        {
                            ui32Result = IMG_ERROR_TIMEOUT;
                        }

                        //we souldn't wait any more on timeout
                        break;
                    }
                    else//unknown error
                    {
                        ui32Result = IMG_ERROR_FATAL;
                    }
                }
            }
        }
    }

    if(ui32Result == IMG_SUCCESS)
    {
        --psThreadSync->ui32SignalCounter;//deincrementing number of signalls after releasing thread
    }

    if( pthread_mutex_unlock(&psThreadSync->hMutex) != 0)
    {
        IMG_ASSERT(0 && "Internal mutex error: probably the thread sync object is not created (or already destroyed)");
        return IMG_ERROR_FATAL;
    }

    return ui32Result;
}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionCreate

******************************************************************************/
IMG_RESULT OSA_ThreadConditionCreate(
    IMG_HANDLE * const  phThreadCondition
)
{
    sThreadConditionParams * psThreadCondition;
    pthread_mutexattr_t sMutexAttr;

    if(pthread_mutexattr_init(&sMutexAttr) != 0)
    {
        IMG_ASSERT(0 && "Internal mutex attributes initialization failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_mutexattr_settype(&sMutexAttr, PTHREAD_MUTEX_ERRORCHECK) != 0)//set mutex attributes to check errors
    {
        IMG_ASSERT(0 && "Internal mutex setting type failed");
        return IMG_ERROR_FATAL;
    }

    psThreadCondition = IMG_MALLOC(sizeof(sThreadConditionParams));
    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    if(pthread_mutex_init(&psThreadCondition->hMutex, NULL) != 0)//create internal mutex, used to sync access to bSignalled and ui32WaitingTasks variable
    {
        IMG_FREE(psThreadCondition);
        IMG_ASSERT(0 && "Internal mutex initialization failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_cond_init(&psThreadCondition->hCond, NULL) != 0)//create condition variable, used to signal thread sync object
    {
        IMG_FREE(psThreadCondition);
        IMG_ASSERT(0 && "Internal condition variable initialization failed");
        return IMG_ERROR_FATAL;
    }

    *phThreadCondition = (IMG_HANDLE)psThreadCondition;//handler is a pointer to thread sync structure

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionDestroy

******************************************************************************/
IMG_VOID OSA_ThreadConditionDestroy(
    IMG_HANDLE  hThreadCondition
)
{
    sThreadConditionParams * psThreadCondition = (sThreadConditionParams *)hThreadCondition;//handler is a pointer to thread sync structure

    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition == NULL)
    {
        return;
    }

    if(pthread_mutex_destroy(&psThreadCondition->hMutex) != 0)
    {
        fprintf(stderr,"Internal mutex deinitialization failed");
        IMG_ASSERT(0);
        abort();
    }

    if(pthread_cond_destroy(&psThreadCondition->hCond) != 0)
    {
        fprintf(stderr,"Internal variable condition deinitialization failed");
        IMG_ASSERT(0);
        abort();
    }

    IMG_FREE(psThreadCondition);
}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionSignal

******************************************************************************/
IMG_VOID OSA_ThreadConditionSignal(
    IMG_HANDLE  hThreadCondition
)
{
    sThreadConditionParams * psThreadCondition = (sThreadConditionParams *)hThreadCondition;//handler is a pointer to thread sync structure

    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition == NULL)
    {
        fprintf(stderr,"Passed handler is not a handler to Thread Condition object");
        abort();
    }

    pthread_cond_signal(&psThreadCondition->hCond);//sent signal that wakes one of waiting threads

}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionWait

******************************************************************************/
IMG_RESULT OSA_ThreadConditionWait(
    IMG_HANDLE  hThreadCondition,
    IMG_UINT32  ui32TimeoutMs
)
{
    sThreadConditionParams * psThreadCondition = (sThreadConditionParams *)hThreadCondition;//handler is a pointer to thread sync structure
    IMG_RESULT ui32Result = IMG_SUCCESS;

    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition == NULL)
    {
        return IMG_ERROR_FATAL;
    }

    if(ui32TimeoutMs == OSA_NO_TIMEOUT)
    {
        pthread_cond_wait(&psThreadCondition->hCond,&psThreadCondition->hMutex);//wait without timeout
    }
    else
    {
        struct timespec time;
        struct timeval    tp;

        if(gettimeofday(&tp, NULL) != 0)//getting current time
        {
            IMG_ASSERT(0 && "Getting time failed");
            ui32Result = IMG_ERROR_FATAL;
        }
        else
        {
            int iWaitResult;
            //converting timeval to timespec
            time.tv_sec  = tp.tv_sec;
            time.tv_nsec = tp.tv_usec * 1000;

            //converting elapse time to absolute time
            time.tv_sec += ui32TimeoutMs/1000;
            time.tv_nsec += (ui32TimeoutMs-(ui32TimeoutMs/1000)*1000)*1000000;

            //time.tv_nsec should be less than 1000000000
            if(time.tv_nsec >= 1000000000)
            {
                ++time.tv_sec;
                time.tv_nsec -= 1000000000;
            }

            if( (iWaitResult = pthread_cond_timedwait(&psThreadCondition->hCond, &psThreadCondition->hMutex, &time)) != 0)//wait with timeout
            {
                IMG_ASSERT(iWaitResult == ETIMEDOUT && "Waiting for signal failed");

                if(iWaitResult == ETIMEDOUT)//timeout elapsed
                {
                    ui32Result = IMG_ERROR_TIMEOUT;
                }
                else//unknown error
                {
                    ui32Result = IMG_ERROR_FATAL;
                }
            }
        }
    }

    return ui32Result;
}

/*!
******************************************************************************

 @Function              OSA_ThreadConditionUnlock

******************************************************************************/
IMG_VOID OSA_ThreadConditionUnlock(
    IMG_HANDLE  hThreadCondition
)
{
    sThreadConditionParams * psThreadCondition = (sThreadConditionParams *)hThreadCondition;//handler is a pointer to thread sync structure

    if(pthread_mutex_unlock(&psThreadCondition->hMutex) != 0)
    {
        fprintf(stderr,"Thread Condition mutex unlock failed");
        IMG_ASSERT(0);
        abort();
    }

}


/*!
******************************************************************************

 @Function              OSA_ThreadConditionLock

******************************************************************************/
IMG_VOID OSA_ThreadConditionLock(
    IMG_HANDLE  hThreadCondition
)
{
    sThreadConditionParams * psThreadCondition = (sThreadConditionParams *)hThreadCondition;//handler is a pointer to thread sync structure

    if(pthread_mutex_lock(&psThreadCondition->hMutex) != 0)
    {
        fprintf(stderr,"Thread Condition mutex lock failed");
        IMG_ASSERT(0);
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadCreateAndStart

 @Description

 Creates and starts a thread.

 @Input    pfnThreadFunc    : A pointer to a #OSA_pfnThreadFunc thread function.

 @Input    pvThreadParam    : A pointer to client data passed to the thread function.

 @Input    pszThreadName    : A text string giving the name of the thread.

 @Input    eThreadPriority    : The thread priority.

 @Output   phThread            : A pointer used to return the thread handle.

 @Return   IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadCreateAndStart(
    OSA_pfnThreadFunc       pfnThreadFunc,
    IMG_VOID *              pvThreadParam,
    const IMG_CHAR * const  pszThreadName,
    OSA_eThreadPriority     eThreadPriority,
    IMG_HANDLE * const      phThread
)
{
    IMG_RESULT ui32Result = IMG_SUCCESS;
    sThreadInfo * psThreadInfo;
    pthread_attr_t        sPthreadAttr;
    struct sched_param    sSchedParam;

    //convert osa priorities to posix priorities
    switch(eThreadPriority)
        {
        case OSA_THREAD_PRIORITY_LOWEST:
            sSchedParam.sched_priority = sched_get_priority_min(SCHED_OTHER);
            break;
        case OSA_THREAD_PRIORITY_BELOW_NORMAL:
            sSchedParam.sched_priority = sched_get_priority_min(SCHED_OTHER) + (sched_get_priority_max(SCHED_OTHER)-sched_get_priority_min(SCHED_OTHER))/4;
            break;
        case OSA_THREAD_PRIORITY_NORMAL:
            sSchedParam.sched_priority = sched_get_priority_min(SCHED_OTHER) + (sched_get_priority_max(SCHED_OTHER)-sched_get_priority_min(SCHED_OTHER))/2;
            break;
        case OSA_THREAD_PRIORITY_ABOVE_NORMAL:
            sSchedParam.sched_priority = sched_get_priority_min(SCHED_OTHER) + 3*(sched_get_priority_max(SCHED_OTHER)-sched_get_priority_min(SCHED_OTHER))/4;
            break;
        case OSA_THREAD_PRIORITY_HIGHEST:
            sSchedParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
            break;
        default:
            return IMG_ERROR_INVALID_PARAMETERS;
        }

    if(pthread_attr_init(&sPthreadAttr) != 0)
    {
        IMG_ASSERT(0 && "Pthread attributes initialization failed");
        return IMG_ERROR_FATAL;
    }

    if(pthread_attr_setschedpolicy(&sPthreadAttr, SCHED_OTHER) != 0)//set default scheduler policy
    {
        IMG_ASSERT(0 && "Pthread setting schedule policy failed");
        return IMG_ERROR_FATAL;
    }
    if(pthread_attr_setschedparam(&sPthreadAttr, &sSchedParam) != 0)//set priority
    {
        IMG_ASSERT(0 && "Pthread setting schedule params failed");
        return IMG_ERROR_FATAL;
    }

    psThreadInfo = (sThreadInfo*)IMG_MALLOC(sizeof(sThreadInfo));
    IMG_ASSERT(psThreadInfo != NULL);

    if(psThreadInfo == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    //set thread parameters
    psThreadInfo->sFuncParam.pfnStartFunc = pfnThreadFunc;
    psThreadInfo->sFuncParam.pvTaskParam = pvThreadParam;
    psThreadInfo->sFuncParam.pszName = IMG_MALLOC((strlen(pszThreadName)+1) * sizeof(IMG_CHAR));

    IMG_ASSERT(psThreadInfo->sFuncParam.pszName != NULL);
    if(psThreadInfo->sFuncParam.pszName == NULL)
    {
        IMG_FREE(psThreadInfo);
        return IMG_ERROR_MALLOC_FAILED;
    }
    strcpy(psThreadInfo->sFuncParam.pszName, pszThreadName);

    //create new thread using wrapper function
    if(pthread_create(&psThreadInfo->hThread,
                      &sPthreadAttr,
                      osa_ThreadFuncWrapper,
                      &psThreadInfo->sFuncParam) == 0)
    {
        *phThread = (IMG_HANDLE)psThreadInfo;//hanlde is pointer to thread structure
    }
    else//clear memory if failed
    {
        IMG_FREE(psThreadInfo->sFuncParam.pszName);
        IMG_FREE(psThreadInfo);
        IMG_ASSERT(0 && "Pthread create failed");
        ui32Result = IMG_ERROR_FATAL;
    }

    return ui32Result;
}

/*!
******************************************************************************

 @Function                OSA_ThreadWaitExitAndDestroy

 @Description

 Waits for a thread to exit and to destroy the thread object allocated by
 the OSA layer.

 @Input     hThread      : Handle to thread

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadWaitExitAndDestroy(
    IMG_HANDLE  hThread
)
{
    sThreadInfo * psThreadInfo = (sThreadInfo *)hThread;//hanlde is pointer to thread structure

    IMG_ASSERT(psThreadInfo != NULL);
    if(psThreadInfo != NULL)
    {
        if(pthread_join(psThreadInfo->hThread,NULL) != 0)//waits for given thread
        {
            fprintf(stderr,"pthread_join failure");
            IMG_ASSERT(0);
            abort();
        }
        IMG_FREE(psThreadInfo->sFuncParam.pszName);
        IMG_FREE(psThreadInfo);
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSleep

 @Description

 Cause the calling thread to sleep for a period of time.

 NOTE: A ui32SleepMs of 0 relinquish the time-slice of current thread.

 @Input     ui32SleepMs        : Sleep period in milliseconds.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadSleep(
    IMG_UINT32  ui32SleepMs
)
{
    //convert milliseconds to timespec structure
    struct timespec time;
    time.tv_sec = ui32SleepMs/1000;
    time.tv_nsec = (ui32SleepMs-time.tv_sec*1000)*1000000;

    nanosleep(&time,NULL);
}

/*!
******************************************************************************

 @Function                osa_TLSInitFunction

 @Description

 Initialization callback function that creates the Thread Local Storage key.

 NOTE: A ui32SleepMs of 0 relinquish the time-slice of current thread.

 @Return    None.

******************************************************************************/
static void osa_TLSInitFunction(void)
{
    int iStatus;
    
    iStatus = pthread_key_create(&gsTlsKey, NULL);
    IMG_ASSERT(iStatus == 0);
}

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataSet

 @Description

 Store data in Thread Local Storage (data specific to calling thread).

 @Input     pData : Pointer to data to be set.

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataSet(
    IMG_PVOID pData
)
{
    int iStatus;

    //check input
    IMG_ASSERT(pData != NULL);
    if(pData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    //TLS once initialization
    pthread_once(&gsOneTimeInit, osa_TLSInitFunction);

    //set data
    iStatus = pthread_setspecific(gsTlsKey, pData);

    IMG_ASSERT(iStatus == 0);
    if(iStatus != 0)
    {
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataGet

 @Description

 Retrieve data from Thread Local Storage (data specific to calling thread).

 @Input     ppData : Pointer to pointer that will store retrieved data or
                    NULL if function fails

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataGet(
    IMG_PVOID * ppData
)
{
    //check input
    IMG_ASSERT(ppData != NULL);
    if(ppData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    //TLS once initialization, should be initialized in set,
    //below used to sync (wait until initFunction returns)
    pthread_once(&gsOneTimeInit, osa_TLSInitFunction);

    //get data
    *ppData  = pthread_getspecific(gsTlsKey);
    IMG_ASSERT(*ppData != NULL);
    if(*ppData == NULL)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}


