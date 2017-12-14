/*!
******************************************************************************
 @file   :  osa.c

 @brief  OS Abstraction Layer (OSA)

 @Author Imagination Technologies

 @date   03/10/2012

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
#include <windows.h>
#include <osa.h>

#include "img_errors.h"
#include "img_defs.h"
#include <stdlib.h>
#include <stdio.h>

// Global variable for one-time initialization
LONG gsInitOnce = 0; // Static initialization
DWORD gdwTlsIndex; //Thread Local Storage index

/*!
******************************************************************************

Code below comes from MSDN library

******************************************************************************/
#define MS_VC_EXCEPTION 0x406D1388

typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;

void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = szThreadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;
// Try doesn't work in minGW
#ifdef _MSC_VER
   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(DWORD), (const ULONG_PTR *)&info );
   }
   __except(EXCEPTION_CONTINUE_EXECUTION)
   {
   }
#endif
}

/*!
******************************************************************************

 @Struct                 sThreadFuncParams

 @Description

 Combines OSA_pfnThreadFunc and parameters of new thread.
 Necessary to pass both those values to wrapper function(OSA_ThreadFuncWrapper)

******************************************************************************/
typedef struct
{
    OSA_pfnThreadFunc  pfnStartFunc;
    IMG_VOID *         pvTaskParam;

} sThreadFuncParams;
/*!
******************************************************************************

 @Struct                 sThreadInfo

 @Description

 Combines thread ID and its function and params.

******************************************************************************/
typedef struct
{
    HANDLE             hThread;
    sThreadFuncParams  sFuncParam;

} sThreadInfo;

/*!
******************************************************************************

 @Struct                 sThreadSyncParams

 @Description

 Combines objects that are needed to implement Thread Sync object.

******************************************************************************/
typedef struct
{
    HANDLE             hSemaphore;
    IMG_INT32          ui32Counter;

} sThreadSyncParams;

/*!
******************************************************************************

 @Struct                 sThreadConditionParams

 @Description

 Combines objects that are needed to implement Thread Sync object.

******************************************************************************/
typedef struct
{

	HANDLE hMutex;			/* Synchronization mutex					*/
	HANDLE hEvent;			/* signal event (auto reset)				*/
						
	IMG_UINT32 nWaiting;	/* Number of threads waiting				*/
} sThreadConditionParams;

/*!
******************************************************************************

 @Function                osa_ThreadFuncWrapper

 @Description

 Wrapper function that translates OSA_pfnThreadFunc to posix thread equivalent.

 @Input        pvFuncAndParams : Pair of new thread function and its params.

 @Return    IMG_VOID *        : Thread exit value.

******************************************************************************/
static DWORD WINAPI osa_ThreadFuncWrapper(
    LPVOID  pvFuncAndParams
)
{
    sThreadFuncParams * pFuncParamPair = (sThreadFuncParams*)pvFuncAndParams;

    IMG_ASSERT(pFuncParamPair != NULL);
    if(pFuncParamPair == NULL)
    {
        return 1;
    }

    pFuncParamPair->pfnStartFunc(pFuncParamPair->pvTaskParam);

    return 0;
}
/*!
******************************************************************************

 @Function                OSA_CritSectCreate

******************************************************************************/
IMG_RESULT OSA_CritSectCreate(
    IMG_HANDLE * const  phCritSect
)
{
    LPCRITICAL_SECTION lpCriticalSection;

    lpCriticalSection = (LPCRITICAL_SECTION)IMG_MALLOC(sizeof(CRITICAL_SECTION));
    IMG_ASSERT(lpCriticalSection != NULL);
    
    if(lpCriticalSection==NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    //create windows critical section
    InitializeCriticalSection(lpCriticalSection);

    //save pinter to critical section as IMG_HANDLE
    *phCritSect = (IMG_HANDLE)lpCriticalSection;
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
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION)hCritSect;
    IMG_ASSERT(lpCriticalSection != NULL);

    if(lpCriticalSection != NULL)
    {
        DeleteCriticalSection(lpCriticalSection);
        IMG_FREE(lpCriticalSection);
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
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
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION)hCritSect;
    IMG_ASSERT(lpCriticalSection != NULL);

    if(lpCriticalSection == NULL)
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
    }

    LeaveCriticalSection(lpCriticalSection);
}


/*!
******************************************************************************

 @Function                OSA_CritSectLock

******************************************************************************/
IMG_VOID OSA_CritSectLock(
    IMG_HANDLE  hCritSect
)
{
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION)hCritSect;
    IMG_ASSERT(lpCriticalSection != NULL);

    if(lpCriticalSection == NULL)
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
    }

    EnterCriticalSection(lpCriticalSection);
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncCreate

******************************************************************************/
IMG_RESULT OSA_ThreadSyncCreate(
    IMG_HANDLE * const  phThreadSync
)
{
    sThreadSyncParams *psThreadSync;

    psThreadSync = (sThreadSyncParams *) IMG_MALLOC(sizeof(sThreadSyncParams));
    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    //create windows auto event in non-siganlled state
    if((psThreadSync->hSemaphore = CreateSemaphore(NULL,
                                              0,
                                              LONG_MAX,
                                              NULL))== NULL)
    {
        IMG_ASSERT(psThreadSync->hSemaphore != NULL);
        return IMG_ERROR_FATAL;
    }

    psThreadSync->ui32Counter = 0;

    *phThreadSync = psThreadSync;

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
    sThreadSyncParams *psThreadSync = (sThreadSyncParams *)hThreadSync;
    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync != NULL)
    {

        if(CloseHandle(psThreadSync->hSemaphore) != TRUE)
        {
            fprintf(stderr, "Destroy iteral semaphore failed\n");
            IMG_ASSERT(0);
            abort();
        }

        IMG_FREE(psThreadSync);
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadSync\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncSignal

******************************************************************************/
IMG_VOID OSA_ThreadSyncSignal(
    IMG_HANDLE  hThreadSync
)
{
    sThreadSyncParams *psThreadSync = (sThreadSyncParams *)hThreadSync;
    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync != NULL)
    {
        if(ReleaseSemaphore(psThreadSync->hSemaphore,1,NULL) != TRUE)
        {
            IMG_ASSERT(0 && "Event signal failed");
        }
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadSync\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncWait

******************************************************************************/
IMG_RESULT OSA_ThreadSyncWait(
    IMG_HANDLE  phThreadSync,
    IMG_UINT32  ui32TimeoutMs
)
{
    DWORD ret = WAIT_OBJECT_0;
    sThreadSyncParams *psThreadSync = (sThreadSyncParams *)phThreadSync;
    IMG_RESULT ui32Result = IMG_SUCCESS;

    IMG_ASSERT(psThreadSync != NULL);

    if(psThreadSync != NULL)
    {
        ret = WaitForSingleObject(psThreadSync->hSemaphore,
                                  ui32TimeoutMs == OSA_NO_TIMEOUT ? INFINITE : ui32TimeoutMs);
    }
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(ret == WAIT_TIMEOUT || ret == WAIT_OBJECT_0);
    if (ret == WAIT_TIMEOUT)
    {
        ui32Result = IMG_ERROR_TIMEOUT;
    }
    else if (ret != WAIT_OBJECT_0)
    {
        ui32Result = IMG_ERROR_FATAL;
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
    sThreadConditionParams *psThreadCondition;

    psThreadCondition = (sThreadConditionParams *) IMG_MALLOC(sizeof(sThreadConditionParams));
    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }
	
	psThreadCondition->hMutex = CreateMutex(NULL, FALSE, NULL);
	psThreadCondition->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	
	psThreadCondition->nWaiting = 0;

	if (psThreadCondition->hMutex == NULL ||
		psThreadCondition->hEvent == NULL)
	{
		OSA_ThreadConditionDestroy(psThreadCondition);
		return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;;
	}

    *phThreadCondition = psThreadCondition;

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
    sThreadConditionParams *psThreadCondition = (sThreadConditionParams *)hThreadCondition;
    IMG_ASSERT(psThreadCondition != NULL);

	if(psThreadCondition != NULL)
    {
		if (psThreadCondition->hEvent != NULL)
			CloseHandle(psThreadCondition->hEvent);

		if (psThreadCondition->hMutex != NULL)
			CloseHandle(psThreadCondition->hMutex);

		IMG_FREE(psThreadCondition);
	}
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadCondition\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionSignal

******************************************************************************/
IMG_VOID OSA_ThreadConditionSignal(
    IMG_HANDLE  hThreadCondition
)
{
    sThreadConditionParams *psThreadCondition = (sThreadConditionParams *)hThreadCondition;
    IMG_ASSERT(psThreadCondition != NULL);

    if(psThreadCondition != NULL)
    {
		if (psThreadCondition->nWaiting > 0)
			SetEvent(psThreadCondition->hEvent);  
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadCondition\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadConditionWait

******************************************************************************/
IMG_RESULT OSA_ThreadConditionWait(
    IMG_HANDLE  phThreadCondition,
    IMG_UINT32  ui32TimeoutMs
)
{
    DWORD dwResult = WAIT_OBJECT_0;
    sThreadConditionParams *psThreadCondition = (sThreadConditionParams *)phThreadCondition;
    IMG_RESULT ui32Result = IMG_SUCCESS;

    IMG_ASSERT(psThreadCondition != NULL);
	if(psThreadCondition != NULL)
	{
		psThreadCondition->nWaiting++;

		/* Unlock the mutex */
		ReleaseMutex(psThreadCondition->hMutex);

		/* Wait for signal or broadcast */
		dwResult = WaitForSingleObject(psThreadCondition->hEvent, ui32TimeoutMs == OSA_NO_TIMEOUT ? INFINITE : ui32TimeoutMs);

		/* Lock the mutex */
		WaitForSingleObject(psThreadCondition->hMutex, INFINITE);

		if(dwResult == WAIT_TIMEOUT)
			ui32Result = IMG_ERROR_TIMEOUT;

		psThreadCondition->nWaiting--;

	}
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(dwResult == WAIT_TIMEOUT || dwResult == WAIT_OBJECT_0);
    if (dwResult == WAIT_TIMEOUT)
    {
        ui32Result = IMG_ERROR_TIMEOUT;
    }
    else if (dwResult != WAIT_OBJECT_0)
    {
        ui32Result = IMG_ERROR_FATAL;
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
    
	if(psThreadCondition == NULL)
    {
        fprintf(stderr, "Passed handler is not a Condition handle\n");
        abort();
    }
	ReleaseMutex(psThreadCondition->hMutex);
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

	if(psThreadCondition == NULL)
    {
        fprintf(stderr, "Passed handler is not a Condition handle\n");
        abort();
    }
	WaitForSingleObject(psThreadCondition->hMutex, INFINITE);
}

/*!
******************************************************************************

 @Function                OSA_ThreadCreateAndStart

******************************************************************************/
IMG_RESULT OSA_ThreadCreateAndStart(
    OSA_pfnThreadFunc       pfnThreadFunc,
    IMG_VOID *              pvThreadParam,
    const IMG_CHAR * const  pszThreadName,
    OSA_eThreadPriority     eThreadPriority,
    IMG_HANDLE * const      phThread
)
{
    sThreadInfo * psThreadInfo;
    DWORD dwThreadId;

    psThreadInfo = (sThreadInfo*)IMG_MALLOC(sizeof(sThreadInfo));
    IMG_ASSERT(psThreadInfo != NULL);

    if(psThreadInfo == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    psThreadInfo->sFuncParam.pfnStartFunc = pfnThreadFunc;
    psThreadInfo->sFuncParam.pvTaskParam = pvThreadParam;

    //create new thread and use HANDLE as IMG_HANDLE
    if((psThreadInfo->hThread = CreateThread(NULL,
                                 0,
                                 osa_ThreadFuncWrapper,
                                 &psThreadInfo->sFuncParam,
                                 0,
                                 &dwThreadId)) == NULL)
    {
        IMG_ASSERT(psThreadInfo->hThread != NULL);
        return  IMG_ERROR_FATAL;
    }
    else
    {
        int iPriority;

        //set thread name
        SetThreadName(dwThreadId, pszThreadName);

        //mapping windows priorities to osa priorities
        switch(eThreadPriority)
        {
        case OSA_THREAD_PRIORITY_LOWEST:
            iPriority = THREAD_PRIORITY_LOWEST;
            break;
        case OSA_THREAD_PRIORITY_BELOW_NORMAL:
            iPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case OSA_THREAD_PRIORITY_NORMAL:
            iPriority = THREAD_PRIORITY_NORMAL;
            break;
        case OSA_THREAD_PRIORITY_ABOVE_NORMAL:
            iPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case OSA_THREAD_PRIORITY_HIGHEST:
            iPriority = THREAD_PRIORITY_HIGHEST;
            break;
        default:
            if(CloseHandle(phThread) != TRUE)
            {
                IMG_ASSERT(0 && "Cannot close thread after setting priority failure");
            }
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        //setting priority
        if(SetThreadPriority(phThread, iPriority) != 0)
        {
            if(CloseHandle(phThread) != TRUE)
            {
                IMG_ASSERT(0 && "Cannot close thread after setting priority failure");
            }
            return  IMG_ERROR_FATAL;
        }
    }

    *phThread = psThreadInfo;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                OSA_ThreadWaitExitAndDestroy

******************************************************************************/
IMG_VOID OSA_ThreadWaitExitAndDestroy(
    IMG_HANDLE  hThread
)
{
    sThreadInfo * psThreadInfo = (sThreadInfo *)hThread;

    IMG_ASSERT(psThreadInfo != NULL);
    if(psThreadInfo != NULL)
    {
        if(WaitForSingleObject(psThreadInfo->hThread, INFINITE) == WAIT_FAILED)
        {
            fprintf(stderr, "Waiting for thread failed\n");
            IMG_ASSERT(0);
            abort();            
        }
        if(CloseHandle(psThreadInfo->hThread) != TRUE)
        {
            fprintf(stderr, "Cannot close thread\n");
            IMG_ASSERT(0);
            abort();  
        }
        IMG_FREE(psThreadInfo);
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to OSA Thread\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSleep

******************************************************************************/
IMG_VOID OSA_ThreadSleep(
    IMG_UINT32  ui32SleepMs
)
{
    Sleep(ui32SleepMs);
}

/*!
******************************************************************************

 @Function                osa_InitTLSOnce

 @Description

 Initialization function that creates the Thread Local Storage index 

 @Return    TRUE if success, otherwise FALSE.

******************************************************************************/
static BOOL osa_InitTLSOnce()                 
{
    //gsInitOnce is initially 0
    //when it is called first time its value is campared to third argument (which is 0)
    //the values are equal, so value of second argument is assigned to gsInitOnce (now 1)
    //function returns gsInitOnce initial value, so in that case it is 0, and initilaization should be done
    //when function is called next time it campares gsInitOnce (now 1) to 0
    //the values are diffirent and gsInitOnce stay unchanged
    //function returns initial value (1), so initialization is omitted (as it should be)
    //It works like InitOnceInitialize that cannot be used because of no win xp support...
    if( InterlockedCompareExchange(&gsInitOnce,1,0) == 0 )
	{
    	if ((gdwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
    	{
       		IMG_ASSERT( 0 && "TlsAlloc failed");
        	return FALSE;
    	}
    }

    return TRUE;
}

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataSet

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataSet(
    IMG_PVOID pData
)
{
    BOOL bStatus;//status returned by winapi function

    //check input
    IMG_ASSERT(pData != NULL);
    if(pData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    //TLS once initialization
    bStatus = osa_InitTLSOnce();

    IMG_ASSERT(bStatus == TRUE);
    if(bStatus == FALSE)
    {
        return IMG_ERROR_FATAL;
    }

    bStatus = TlsSetValue(gdwTlsIndex, pData);

    IMG_ASSERT(bStatus == TRUE);
    if ( bStatus == FALSE)
    {
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataGet

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataGet(
    IMG_PVOID * ppData
)
{
    BOOL bStatus;//status returned by winapi function

    //check input
    IMG_ASSERT(ppData != NULL);
    if(ppData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    //TLS once initialization, should be initialized in set,
    //below used to sync (wait until initFunction returns)
    bStatus = osa_InitTLSOnce();

    IMG_ASSERT(bStatus == TRUE);
    if(bStatus == FALSE)
    {
        *ppData = NULL;
        return IMG_ERROR_FATAL;
    }
 
    // Retrieve a data pointer for the current thread. 
    *ppData = (IMG_PVOID) TlsGetValue(gdwTlsIndex);
    
    IMG_ASSERT(*ppData != NULL);
    if(*ppData == NULL)
    {
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}


