/*!
******************************************************************************
 @file   : tal_callback.c

 @brief

 @Author Alejandro Arcos

 @date   07/09/2010

         <b>Copyright 2003 by Imagination Technologies Limited.</b>\n
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
         TAL Callback functions C Source File.

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/
/*
******************************************************************************
*/
#include "img_errors.h"
#include "tal.h"
#include "tal_internal.h"
#include "tal_intdefs.h"
#include <stdio.h>
#include <string.h>

#if defined (__TAL_CALLBACK_SUPPORT__)
#include "cbman.h"
#endif

#if defined(__TAL_NOT_THREADSAFE__)
#error "TAL must be used with threads safe on to make callback register updates appear as one operation"
#endif
// more or less the same than above but meos and osa are used for extra locking
#if !defined(__TAL_USE_MEOS__) && !defined(__TAL_USE_OSA__)
#error "TAL callback needs either MeOS or OSA to ensure thread-safety"
#endif

#if defined (WIN32) || defined (__linux__) || defined (__APPLE__)
#if !defined (__TAL_NO_OS__)
#if !defined (__TAL_CALLBACK_SUPPORT__)
#if !defined (__TAL_MAKE_THREADSAFE__)
#define __TAL_MAKE_THREADSAFE__
#endif
#endif
#endif
#endif


#if defined (__TAL_CALLBACK_SUPPORT__)


/*!
******************************************************************************

 @Function              TALCALLBACK_AddEventCallback

******************************************************************************/

IMG_RESULT TALCALLBACK_AddEventCallback (
    IMG_HANDLE	                hMemSpace,
    TALCALLBACK_eEvent          eEvent,
    IMG_VOID *                  pEventParameters,
    CBMAN_pfnEventCallback      pfnEventCallback,
    IMG_VOID *                  pCallbackParameter,
    CBMAN_hCallback *           phEventCallback
)
{
#if defined(__TAL_USE_MEOS__)
    KRN_IPL_T						sOldIPL;
#endif
    TALCALLBACK_sMonitorAccessCB *  psMonitorAccessCB;
    TALCALLBACK_sMonitorAccessCB *  psTmpMonitorAccessCB;
	TAL_psMemSpaceCB				psMemSpaceCB = (TAL_psMemSpaceCB)hMemSpace;
	TAL_psMemSpaceInfo				psMemSpaceInfo;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT(hMemSpace);
	psMemSpaceInfo = psMemSpaceCB->psMemSpaceInfo;

    /* Check this type of access is valid for this memory space...*/
    switch (psMemSpaceInfo->eMemSpaceType)
    {
    case TAL_MEMSPACE_REGISTER:
        break;

    case TAL_MEMSPACE_MEMORY:
        /* At some point we may want to extend support to these memory spaces */
        /* ERROR: TAL - TAL_AddEventCallback not valid for memory space "XXX" */
        printf("ERROR: TAL - TALCALLBACK_AddEventCallback not valid for memory space \"%s\"\n", psMemSpaceInfo->pszMemSpaceName);

        /* Assert */
        IMG_ASSERT(IMG_FALSE);
    default:
        /* Assert */
        IMG_ASSERT(IMG_FALSE);
    }

    /* If we have not allocate a callback slot for this memory space...*/
    if (!psMemSpaceCB->bCbSlotAllocated)
    {
        /* Initialise the Callback Manager */
        CBMAN_Initialise();

        /* Register a callback system on a slot */
        CBMAN_RegisterMyCBSystem(&psMemSpaceCB->ui32CbManSlot);

        /* Callback slot now allocated */
        psMemSpaceCB->bCbSlotAllocated = IMG_TRUE;
    }

    /* Check we have a callback function */
    IMG_ASSERT(phEventCallback != IMG_NULL);

    /* Add callback */
    CBMAN_AddCallbackToMyCBSystem ( psMemSpaceCB->ui32CbManSlot,
                                    eEvent,
                                    pfnEventCallback,
                                    pCallbackParameter,
                                    phEventCallback
                                  );

    /* Check event and event parameters */
    switch (eEvent)
    {
    case TALCALLBACK_EVENT_PRE_READ_WORD:
    case TALCALLBACK_EVENT_PRE_WRITE_WORD:
    case TALCALLBACK_EVENT_POST_READ_WORD:
    case TALCALLBACK_EVENT_POST_WRITE_WORD:
        /* Check we have an event paramater */
        IMG_ASSERT(pEventParameters != IMG_NULL);

        /* Retain the callback handle in the region structure so that
           we can remove the region from the list when the callback
           is removed */
        psMonitorAccessCB = (TALCALLBACK_sMonitorAccessCB *) pEventParameters;
        psMonitorAccessCB->hEventCallback = *phEventCallback;

        /* Save the event type associated with this region */
        psMonitorAccessCB->eEvent = eEvent;
        /* Check this CB has not been used alreary... */
#if defined(__TAL_USE_OSA__)
        OSA_CritSectLock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
        sOldIPL = KRN_raiseIPL();
#endif
        psTmpMonitorAccessCB = LST_first(&psMemSpaceCB->sMonitoredRegions);
        while (psTmpMonitorAccessCB != IMG_NULL)
        {
            IMG_ASSERT(psTmpMonitorAccessCB != psMonitorAccessCB);

            /* Check next in the list */
            psTmpMonitorAccessCB = LST_next(psTmpMonitorAccessCB);
        }
#if defined(__TAL_USE_OSA__)
        OSA_CritSectUnlock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
        KRN_restoreIPL(sOldIPL);
#endif
        
        /* Add the control block to the list of monitored regions */
#if defined(__TAL_USE_OSA__)
        OSA_CritSectLock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
        sOldIPL = KRN_raiseIPL();
#endif
        LST_add(&psMemSpaceCB->sMonitoredRegions, psMonitorAccessCB);
#if defined(__TAL_USE_OSA__)
        OSA_CritSectUnlock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
        KRN_restoreIPL(sOldIPL);
#endif
        break;
    default:
        IMG_ASSERT(IMG_FALSE);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    /* Success */
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TALCALLBACK_CallbackReadWord

******************************************************************************/

IMG_RESULT TALCALLBACK_CallbackReadWord(
    IMG_HANDLE			            hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32 *                    pui32Value
)
{
   return TALREG_ReadWord(hMemSpace, ui32Offset, pui32Value, IMG_FALSE, IMG_FALSE,0,0,IMG_TRUE,TAL_WORD_FLAGS_32BIT);
}


/*!
******************************************************************************

 @Function				TALCALLBACK_CallbackWriteWord

******************************************************************************/

IMG_RESULT  TALCALLBACK_CallbackWriteWord (
	IMG_HANDLE					    hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Value
)
{
   return TALREG_WriteWord(hMemSpace, ui32Offset, ui32Value, IMG_NULL, IMG_TRUE, TAL_WORD_FLAGS_32BIT);
}


/*!
******************************************************************************

 @Function              TALCALLBACK_RemoveEventCallback

******************************************************************************/

IMG_RESULT  TALCALLBACK_RemoveEventCallback (
    IMG_HANDLE	                hMemSpace,
    CBMAN_hCallback *           phEventCallback
)
{
#if defined(__TAL_USE_MEOS__)
    KRN_IPL_T						sOldIPL;
#endif
    TALCALLBACK_sMonitorAccessCB *  psMonitorAccessCB;
	TAL_psMemSpaceCB				psMemSpaceCB = (TAL_psMemSpaceCB)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
	IMG_ASSERT(hMemSpace);

    /* Check a callback slot has been allocated */
    IMG_ASSERT (psMemSpaceCB->bCbSlotAllocated);

    /* Check if there is an entry in the monitoryed region list associated
       with this callback...*/
#if defined(__TAL_USE_OSA__)
    OSA_CritSectLock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
    sOldIPL = KRN_raiseIPL();
#endif
    psMonitorAccessCB = LST_first(&psMemSpaceCB->sMonitoredRegions);
    while (psMonitorAccessCB != IMG_NULL)
    {
        /* Is this for this callback...*/
        if (psMonitorAccessCB->hEventCallback == *phEventCallback)
        {
            /* Yes, remove it from the list */
            LST_remove(&psMemSpaceCB->sMonitoredRegions, psMonitorAccessCB);
            break;
        }

        /* Check next in the list */
        psMonitorAccessCB = LST_next(psMonitorAccessCB);
    }
#if defined(__TAL_USE_OSA__)
    OSA_CritSectUnlock(psMemSpaceCB->hMonitoredRegionsMutex);
#elif defined(__TAL_USE_MEOS__)
    KRN_restoreIPL(sOldIPL);
#endif

    /* Remove the callback */
    CBMAN_RemoveCallbackToMyCBSystem (psMemSpaceCB->ui32CbManSlot, phEventCallback);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    /* Success */
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              TALCALLBACK_CheckMemoryAccessCallback

******************************************************************************/
static IMG_BOOL bCbActive = IMG_FALSE;

IMG_VOID TALCALLBACK_CheckMemoryAccessCallback (
    IMG_HANDLE                  hMemSpace,
    TALCALLBACK_eEvent          eEvent,
    IMG_UINT32                  ui32Offset,
    IMG_UINT64                  ui64Value
)
{
    TALCALLBACK_sMonitorAccessCB *  psMonitorAccessCB;
	TAL_psMemSpaceCB				psMemSpaceCB = (TAL_psMemSpaceCB)hMemSpace;

    /* If a callback is active...*/
    IMG_ASSERT (!bCbActive);

    /* Set callback active...*/
    bCbActive = IMG_TRUE;

    /* Check event and event parameters */
    switch (eEvent)
    {
    case TALCALLBACK_EVENT_POST_READ_WORD:
    case TALCALLBACK_EVENT_POST_WRITE_WORD:
    case TALCALLBACK_EVENT_PRE_READ_WORD:
    case TALCALLBACK_EVENT_PRE_WRITE_WORD:
        break;
    default:
        IMG_ASSERT(IMG_FALSE);
    }

    /* Check if there is a callback required for this access...*/
    psMonitorAccessCB = LST_first(&psMemSpaceCB->sMonitoredRegions);
    while (psMonitorAccessCB != IMG_NULL)
    {
        /* Is ther for this callback required...*/
        if (
                (psMonitorAccessCB->eEvent == eEvent) &&
                (ui32Offset >= psMonitorAccessCB->ui32StartOffset) &&
                (ui32Offset < (psMonitorAccessCB->ui32StartOffset + psMonitorAccessCB->ui32Size))
            )
        {
            break;
        }

        /* Check next in the list */
        psMonitorAccessCB = LST_next(psMonitorAccessCB);
    }

    /* If we found a callback was required...*/
    if (psMonitorAccessCB != IMG_NULL)
    {
        /* Invoke the callback */
        CBMAN_ExecuteCallback (psMonitorAccessCB->hEventCallback, eEvent, ui32Offset, (IMG_VOID*)(IMG_UINT64_TO_UINT32(ui64Value)));
    }

    /* Set no callback active...*/
    bCbActive = IMG_FALSE;
}

#endif



