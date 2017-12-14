/*!
******************************************************************************
 @file   : tal_memspace.c

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
         Target Abrstraction Layer (TAL) C Source File for memspace and iterator access

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/

#include <tal.h>
#include <tal_intdefs.h>

#ifdef __TAL_USE_OSA__
	#include <osa.h>
#endif
#ifdef __TAL_USE_MEOS__
	#include <krn.h>
	#include <lst.h>
#endif

static TAL_psMemSpaceCB gpsMemSpaceCBs = IMG_NULL;
static IMG_UINT32		gui32UsedMemSpaces = 0;
static IMG_UINT32		gui32MaxMemSpaces = 0;

/*!
******************************************************************************

 @Function              TAL_MemSpaceGetId

******************************************************************************/
IMG_VOID TAL_MemSpaceGetId(
    const IMG_CHAR *    psMemSpaceName,
    IMG_UINT32*         pui32MemSpaceId
) 
{
    IMG_HANDLE hMemspace = TAL_GetMemSpaceHandle(psMemSpaceName);
    *(pui32MemSpaceId) = IMG_UINT64_TO_UINT32((IMG_UINT64)(IMG_UINTPTR)hMemspace);
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Iterator Functions
##############################################################################
******************************************************************************/




/*!
******************************************************************************

 @Function              TALITERATOR_GetFirst

******************************************************************************/
IMG_PVOID TALITERATOR_GetFirst(
    TALITERATOR_eTypes	eItrType,
	IMG_HANDLE *		phIterator
)
{
	TALITR_psIter	psIterator = IMG_NULL;
	IMG_PVOID		pvItem = IMG_NULL;

	switch(eItrType)
	{
	case TALITR_TYP_INTERRUPT:
		{
			TAL_sMemSpaceCB *	psMemSpaceCB;
			IMG_HANDLE			hItr;
			// Start searching the memory spaces
			psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hItr);
			while (psMemSpaceCB)
			{
                /* Ensure that interrupt check function is not used whilst being removed. */
#if defined(__TAL_USE_OSA__)
				OSA_CritSectLock(psMemSpaceCB->hCbMutex);
#elif defined(__TAL_USE_MEOS__)
				KRN_lock(&psMemSpaceCB->sCbLock, KRN_INFWAIT);
#endif
				if (psMemSpaceCB->apfnCheckInterruptFunc)
				{
					if (psMemSpaceCB->apfnCheckInterruptFunc(
											(IMG_HANDLE)psMemSpaceCB,
											psMemSpaceCB->apCallbackParameter))
					{
						psIterator = (TALITR_psIter)IMG_MALLOC(sizeof(TALITR_sIter));
						psIterator->eType = TALITR_TYP_INTERRUPT;
						psIterator->uInfo.itrInterrupt.hMemSpaceIter = hItr;
						pvItem = psMemSpaceCB;
						break;
					}
				}	
#if defined(__TAL_USE_OSA__)
				OSA_CritSectUnlock(psMemSpaceCB->hCbMutex);
#elif defined(__TAL_USE_MEOS__)
				KRN_unlock(&psMemSpaceCB->sCbLock);
#endif
				psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(hItr);
			}
		}
		break;
	case TALITR_TYP_MEMSPACE:
		// If there is at least one memory space, assign it to the returned variable (pvItem)
		if (gui32UsedMemSpaces > 0)
		{
			pvItem = (IMG_PVOID)&gpsMemSpaceCBs[0];
		}
		// If there is more than one, set up an iterator control block and assign the handle
		if (gui32UsedMemSpaces > 1)
		{
			psIterator = (TALITR_psIter)IMG_MALLOC(sizeof(TALITR_sIter));
			psIterator->eType = TALITR_TYP_MEMSPACE;
			psIterator->uInfo.itrMemSpce.psMemSpaceCB = pvItem;
		}
		break;
	default:
		IMG_ASSERT(IMG_FALSE);
	}

	*phIterator = psIterator;
			
	return pvItem;
}

/*!
******************************************************************************

 @Function              TALITERATOR_GetNext

******************************************************************************/
IMG_PVOID TALITERATOR_GetNext(IMG_HANDLE hIterator)
{
	TALITR_psIter	psIterator = (TALITR_psIter)hIterator;
	IMG_PVOID		pvNextItem = NULL;

	TAL_THREADSAFE_INIT;
	TAL_THREADSAFE_LOCK;

	if (hIterator == IMG_NULL)
	{
		TAL_THREADSAFE_UNLOCK;
		return IMG_NULL;
	}

	switch(psIterator->eType)
	{
	case TALITR_TYP_NONE:
		break;
	case TALITR_TYP_INTERRUPT:
		{
			TAL_sMemSpaceCB *	psMemSpaceCB;

			// Start searching the memory spaces
			psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(psIterator->uInfo.itrInterrupt.hMemSpaceIter);
			while (psMemSpaceCB)
			{
				if (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER && psMemSpaceCB->apfnCheckInterruptFunc)
				{
					if (psMemSpaceCB->apfnCheckInterruptFunc(
											(IMG_HANDLE)psMemSpaceCB,
											psMemSpaceCB->apCallbackParameter))
					{
						pvNextItem = psMemSpaceCB;  
						break;
					}
				}
				psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(psIterator->uInfo.itrInterrupt.hMemSpaceIter);
			}
		}
		break;
	case TALITR_TYP_MEMSPACE:
		psIterator->uInfo.itrMemSpce.psMemSpaceCB++;
		if (psIterator->uInfo.itrMemSpce.psMemSpaceCB >= &gpsMemSpaceCBs[gui32UsedMemSpaces])
		{
//			IMG_FREE(psIterator);
			break;
		}
		pvNextItem = psIterator->uInfo.itrMemSpce.psMemSpaceCB;
		break;
	default:
		IMG_ASSERT(IMG_FALSE);
		break;
	}  			

	TAL_THREADSAFE_UNLOCK;

	return pvNextItem;
}

/*!
******************************************************************************

 @Function              TALITERATOR_Destroy

******************************************************************************/
IMG_VOID TALITERATOR_Destroy(IMG_HANDLE hIterator)
{
	if(hIterator)
		IMG_FREE(hIterator);
}


/*!
******************************************************************************
 @Function              TAL_DestroyMemSpaceCBs

******************************************************************************/
IMG_VOID TAL_DestroyMemSpaceCBs()
{
	IMG_FREE(gpsMemSpaceCBs);
	gpsMemSpaceCBs = IMG_NULL;
	gui32UsedMemSpaces = 0;
	gui32MaxMemSpaces = 0;
}

/*!
******************************************************************************

 @Function              TAL_GetFreeMemSpaceCB

******************************************************************************/
TAL_psMemSpaceCB TAL_GetFreeMemSpaceCB()
{
	TAL_psMemSpaceCB psMemSpaceCB;
	if(gui32UsedMemSpaces == gui32MaxMemSpaces) // This means there are none free and we need to make some
	{
		if(gpsMemSpaceCBs == IMG_NULL)	// This means it is the first memspace and we need to do an initial malloc
		{
			gui32MaxMemSpaces = TAL_MAX_MEMSPACES;
			gpsMemSpaceCBs = IMG_MALLOC(sizeof(TAL_sMemSpaceCB) * gui32MaxMemSpaces);
		}
		else
		{
			gui32MaxMemSpaces *= 2;
			gpsMemSpaceCBs = IMG_REALLOC(gpsMemSpaceCBs, sizeof(TAL_sMemSpaceCB) * gui32MaxMemSpaces);
		}
	}
	psMemSpaceCB = &gpsMemSpaceCBs[gui32UsedMemSpaces];
    IMG_MEMSET(psMemSpaceCB, 0, sizeof(TAL_sMemSpaceCB));
	gui32UsedMemSpaces++;
	return psMemSpaceCB;
}

/*!
******************************************************************************

 @Function              TAL_GetMemSpaceHandle

******************************************************************************/
IMG_HANDLE TAL_GetMemSpaceHandle (
    const IMG_CHAR *                 psMemSpaceName
)
{
	TAL_sMemSpaceCB *	psMemSpaceCB;
	IMG_HANDLE			hMemSpItr;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(TALSETUP_IsInitialised());

	IMG_ASSERT(psMemSpaceName != IMG_NULL);

	psMemSpaceCB = TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
	while (psMemSpaceCB)
	{
        if (strcmp(psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName, psMemSpaceName) == 0)
        {
            break;
        }
        psMemSpaceCB = TALITERATOR_GetNext(hMemSpItr);
    }

	if (psMemSpaceCB != IMG_NULL)
	{
		TALITERATOR_Destroy(hMemSpItr);
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	IMG_ASSERT(psMemSpaceCB != IMG_NULL);

    return (IMG_HANDLE)psMemSpaceCB;
}
