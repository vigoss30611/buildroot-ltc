/*!
******************************************************************************
 @file   :  osa.h

 @brief OS Abstraction Layer (OSA)

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

#ifndef OSA_H
#define OSA_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "img_types.h"

#define OSA_NO_TIMEOUT  (0xFFFFFFFF)  /*!< No timeout specified - indefinite wait.  */

/*!
******************************************************************************

 @Function              OSA_pfnThreadFunc

 @Description

 This is the prototype for thread function.

 @Input     pvTaskParam : A pointer client data provided to OSA_ThreadCreate().

 @Return    None.

******************************************************************************/
typedef IMG_VOID (* OSA_pfnThreadFunc) (
    IMG_VOID *  pvTaskParam
);


/*!
******************************************************************************
 This type defines the thread priority level.
 @brief  OSA thread priority level
******************************************************************************/
typedef enum
{
    OSA_THREAD_PRIORITY_LOWEST = 0,    /*!< Lowest priority.        */
    OSA_THREAD_PRIORITY_BELOW_NORMAL,  /*!< Below normal priority.  */
    OSA_THREAD_PRIORITY_NORMAL,        /*!< Normal priority.        */
    OSA_THREAD_PRIORITY_ABOVE_NORMAL,  /*!< Above normal priority.  */
    OSA_THREAD_PRIORITY_HIGHEST        /*!< Highest priority.       */

} OSA_eThreadPriority;


/*!
******************************************************************************

 @Function              OSA_CritSectCreate

 @Description

 Creates a critical section object.

 @Output    phCritSect : A pointer used to return the critical section object handle.

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_CritSectCreate(
    IMG_HANDLE * const  phCritSect
);


/*!
******************************************************************************

 @Function              OSA_CritSectDestroy

 @Description

 Destroys a critical section object.

 NOTE: The state of any thread waiting on the critical section when it is
 destroyed is undefined.

 @Input     hCritSect : Handle to critical section object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_CritSectDestroy(
    IMG_HANDLE  hCritSect
);


/*!
******************************************************************************

 @Function              OSA_CritSectUnlock

 @Description

 Releases ownership of the specified critical section object.

 @Input     hCritSect : Handle to critical section object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_CritSectUnlock(
    IMG_HANDLE  hCritSect
);


/*!
******************************************************************************

 @Function              OSA_CritSectLock

 @Description

 Waits until the critical section object can be acquired.

 NOTE: The state of any thread waiting on the critical section when it is
 destroyed is undefined.

 @Input     hCritSect : Handle to critical section object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_CritSectLock(
    IMG_HANDLE  hCritSect
);


/*!
******************************************************************************

 @Function              OSA_ThreadSyncCreate

 @Description

 Creates an inter-thread synchronisation object. Initial state is non-signalled.

 @Output    phThreadSync : A pointer used to return the inter-thread synchronisation
                        object handle.

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadSyncCreate(
    IMG_HANDLE * const  phThreadSync
);


/*!
******************************************************************************

 @Function              OSA_ThreadSyncDestroy

 @Description

 Destroys an inter-thread synchronisation object.

 NOTE: The state of any thread waiting on the inter-thread synchronisation object
 when it is destroyed is undefined.

 @Input     hThreadSync : Handle to inter-thread synchronisation object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadSyncDestroy(
    IMG_HANDLE  hThreadSync
);


/*!
******************************************************************************

 @Function              OSA_ThreadSyncSignal

 @Description

 Sets the specified inter-thread synchronisation object to the signalled state.
 Each OSA_ThreadSyncSignal() call is counted.

 The inter-thread synchronisation object remains signalled until a number
 of waiting threads, equal to OSA_ThreadSyncSignal() call count, is awaken. After
 that the inter-thread synchronisation object will become non-signalled
 automatically.

 If no threads are waiting, the object's state remains signalled.

 @Input     hThreadSync : Handle to inter-thread synchronisation object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadSyncSignal(
    IMG_HANDLE  hThreadSync
);


/*!
******************************************************************************

 @Function              OSA_ThreadSyncWait

 @Description

 Waits until the inter-thread synchronisation object is in the signalled state
 or the time-out interval elapses.

 If the signalled state of the inter-thread synchronisation object causes
 the thread to wake up and there were more than one OSA_ThreadSyncSignal() calls
 preceding the wake up, the inter-thread synchronisation object will remain signalled.
 The number of thread wake ups has to match the number of OSA_ThreadSyncSignal() calls.

 NOTE: The state of any thread waiting on the inter-thread synchronisation object
 when it is destroyed is undefined.

 @Input     hThreadSync      : Handle to inter-thread synchronisation object.

 @Input     ui32TimeoutMs : Timeout in milliseconds or #OSA_NO_TIMEOUT.

 @Return    IMG_RESULT : This function returns IMG_TIMEOUT if the timeout
                         interval is reached, IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadSyncWait(
    IMG_HANDLE  hThreadSync,
    IMG_UINT32  ui32TimeoutMs
);

/*!
******************************************************************************

 @Function              OSA_ThreadConditionCreate

 @Description

 Creates an inter-thread coniditional wait object. Initial state is non-signalled.

 @Output    phThreadCondition : A pointer used to return the inter-thread conditional wait
                        object handle.

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadConditionCreate(
    IMG_HANDLE * const  phThreadCondition
);


/*!
******************************************************************************

 @Function              OSA_ThreadConditionDestroy

 @Description

 Destroys an inter-thread conditional wait object.

 NOTE: The state of any thread waiting on the inter-thread synchronisation object
 when it is destroyed is undefined.

 @Input     hThreadCondition : Handle to inter-thread synchronisation object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadConditionDestroy(
    IMG_HANDLE  hThreadCondition
);


/*!
******************************************************************************

 @Function              OSA_ThreadConditionSignal

 @Description

 Releases in the internal lock and sets the specified inter-thread synchronisation
 object to the signalled state.

 If no threads are waiting, the object's state remains signalled.

 @Input     hThreadCondition : Handle to inter-thread conditional wait object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadConditionSignal(
    IMG_HANDLE  hThreadCondition
);


/*!
******************************************************************************

 @Function              OSA_ThreadConditionWait

 @Description

 Releases the internal lock and waits until the inter-thread conditional
 wait object is in the signalled state or the time-out interval elapses.

 NOTE: The state of any thread waiting on the inter-thread conditional wait object
 when it is destroyed is undefined.

 @Input     hThreadCondition      : Handle to inter-thread conditional wait object.

 @Input     ui32TimeoutMs : Timeout in milliseconds or #OSA_NO_TIMEOUT.

 @Return    IMG_RESULT : This function returns IMG_TIMEOUT if the timeout
                         interval is reached, IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadConditionWait(
    IMG_HANDLE  hThreadCondition,
    IMG_UINT32  ui32TimeoutMs
);

/*!
******************************************************************************

 @Function              OSA_ThreadConditionUnlock

 @Description

 Releases ownership of the sync lock.

 @Input     hThreadCondition : Handle to inter-thread conditional wait object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadConditionUnlock(
    IMG_HANDLE  hThreadCondition
);


/*!
******************************************************************************

 @Function              OSA_ThreadConditionLock

 @Description

 Waits until the conditional wait critical section object can be acquired.

 @Input     hThreadCondition : Handle to inter-thread conditional wait object.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadConditionLock(
    IMG_HANDLE  hThreadCondition
);


/*!
******************************************************************************

 @Function              OSA_ThreadCreateAndStart

 @Description

 Creates and starts a thread.

 @Input    pfnThreadFunc   : A pointer to a #OSA_pfnThreadFunc thread function.

 @Input    pvThreadParam   : A pointer to client data passed to the thread function.

 @Input    pszThreadName   : A text string giving the name of the thread.

 @Input    eThreadPriority : The thread priority.

 @Output   phThread        : A pointer used to return the thread handle.

 @Return   IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadCreateAndStart(
    OSA_pfnThreadFunc       pfnThreadFunc,
    IMG_VOID *              pvThreadParam,
    const IMG_CHAR * const  pszThreadName,
    OSA_eThreadPriority     eThreadPriority,
    IMG_HANDLE * const      phThread
);


/*!
******************************************************************************

 @Function              OSA_ThreadWaitExitAndDestroy

 @Description

 Waits for a thread to exit and to destroy the thread object allocated
 by the OSA layer.

 @Input     hThread : Handle to thread.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadWaitExitAndDestroy(
    IMG_HANDLE  hThread
);


/*!
******************************************************************************

 @Function              OSA_ThreadSleep

 @Description

 Cause the calling thread to sleep for a period of time.

 NOTE: A ui32SleepMs of 0 relinquish the time-slice of current thread.

 @Input     ui32SleepMs : Sleep period in milliseconds.

 @Return    None.

******************************************************************************/
IMG_VOID OSA_ThreadSleep(
    IMG_UINT32  ui32SleepMs
);


/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataSet

 @Description

 Store data in Thread Local Storage (data specific to calling thread).

 @Input     pData : Pointer to data to be set.

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataSet(
    IMG_PVOID  pData
);


/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataGet

 @Description

 Retrieve data from Thread Local Storage (data specific to calling thread).

 @Input     ppData : Pointer to pointer that will store retrieved data or NULL
                     if function fails

 @Return    IMG_RESULT : This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataGet(
    IMG_PVOID *  ppData
);


#if defined(__cplusplus)
}
#endif


#endif /* OSA_H */




