/*!
******************************************************************************
 @file   : tal_interrupt.c

 @brief

 @Author Tom Hollis

 @date   23/03/2011

         <b>Copyright 2011 by Imagination Technologies Limited.</b>\n
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
         Target Abrstraction Layer (TAL) C Source File for interrupts

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/

#include <img_errors.h>

#include <tal.h>
#include <tal_intdefs.h>


/*!
******************************************************************************
##############################################################################
 Category:              TAL Interrupt Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

 @Function              TALINTERRUPT_RegisterCheckFunc

******************************************************************************/
IMG_RESULT TALINTERRUPT_RegisterCheckFunc(
    IMG_HANDLE                      hMemSpace,
    TAL_pfnCheckInterruptFunc       pfnCheckInterruptFunc,
    IMG_VOID *                      pCallbackParameter
)
{
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(TALSETUP_IsInitialised());
	IMG_ASSERT(hMemSpace);


    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

    /* Check a function has not been registered for this ID */
    IMG_ASSERT(psMemSpaceCB->apfnCheckInterruptFunc == IMG_NULL);

    /* Retain interrupt check function */
    psMemSpaceCB->apfnCheckInterruptFunc = pfnCheckInterruptFunc;
    psMemSpaceCB->apCallbackParameter = pCallbackParameter;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    /* Success */
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              TALINTERRUPT_UnRegisterCheckFunc

******************************************************************************/
IMG_RESULT TALINTERRUPT_UnRegisterCheckFunc(IMG_HANDLE hMemSpace)
{
	TAL_sMemSpaceCB *			psMemSpaceCB =  (TAL_sMemSpaceCB *)hMemSpace;

	TAL_THREADSAFE_INIT;

    /* Ensure that interrupt check function is not removed whilst being used. */
#if defined(__TAL_USE_OSA__)
    OSA_CritSectLock(psMemSpaceCB->hCbMutex);
#elif defined(__TAL_USE_MEOS__)
    KRN_lock(&psMemSpaceCB->sCbLock, KRN_INFWAIT);
#endif
    

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(TALSETUP_IsInitialised());
	IMG_ASSERT(hMemSpace);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

    psMemSpaceCB->apfnCheckInterruptFunc = IMG_NULL;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

#if defined(__TAL_USE_OSA__)
    OSA_CritSectUnlock(psMemSpaceCB->hCbMutex);
#elif defined(__TAL_USE_MEOS__)
    KRN_unlock(&psMemSpaceCB->sCbLock);
#endif
   
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALINTERRUPT_GetMask

******************************************************************************/
IMG_RESULT TALINTERRUPT_GetMask(
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						aui32IntMasks[]
)
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	IMG_UINT32					ui32I;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(TALSETUP_IsInitialised());
	IMG_ASSERT(hMemSpace);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

	for (ui32I = 0; ui32I < TAL_NUM_INT_MASKS; ui32I++)
	{
		aui32IntMasks[ui32I] = psMemSpaceCB->aui32intmask[ui32I];
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

 @Function              TALINTERRUPT_GetNumber

******************************************************************************/
IMG_RESULT TALINTERRUPT_GetNumber(
    IMG_HANDLE						hMemSpace,
    IMG_UINT32 *                    pui32IntNum
)
{
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(TALSETUP_IsInitialised());
	IMG_ASSERT(hMemSpace);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

    *pui32IntNum = psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32intnum;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return IMG_SUCCESS;
}

