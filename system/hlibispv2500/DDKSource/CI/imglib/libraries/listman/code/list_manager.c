/*! 
******************************************************************************
 @file   : list_manager.c

 @brief  

 @Author Tom Whalley

 @date   05/06/2003
 
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
         List Manager Implementation File

 <b>Platform:</b>\n
	     Meson 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#include <img_defs.h>
#include <img_types.h>

#include <img_errors.h>

#include	"list_manager.h"


#define LIST_MGR_PERFORM_ERROR_CHECKING	1

/*!
******************************************************************************

 @Function				LIST_MGR_Initialise
 
******************************************************************************/
IMG_RESULT	LIST_MGR_Initialise					(	LIST_MGR_psControlBlock			psControlBlock,
													IMG_UINT32						ui32MaximumNoOfListElements,
													IMG_VOID*						psStorageArray,
													IMG_UINT32						ui32SizeOfStorageElement	)
{
	IMG_RESULT						ReturnValue = LIST_MGR_ERR_NO_ERR;
	LIST_MGR_psNavigationElement	pCurrentElement;
	IMG_UINTPTR					PointerAdder;
	IMG_UINT32						i;

	/* Do some preliminary sanity checking */
#if LIST_MGR_PERFORM_ERROR_CHECKING

	if (( psControlBlock == IMG_NULL ) ||
		( psStorageArray == IMG_NULL ))
	{
		IMG_ASSERT(IMG_FALSE);
		ReturnValue = LIST_MGR_ERR_BAD_POINTER;
	}
	else if (( ui32MaximumNoOfListElements == 0 ) ||
			 ( ui32SizeOfStorageElement <= sizeof ( LIST_MGR_sNavigationElement )))
	{
		IMG_ASSERT(IMG_FALSE);
		ReturnValue = LIST_MGR_ERR_OUT_OF_RANGE;
	}
#endif

	if ( ReturnValue == LIST_MGR_ERR_NO_ERR )
	{
		psControlBlock->ui32MaximumNoOfElements = ui32MaximumNoOfListElements;
		psControlBlock->psListHead				= IMG_NULL;
		psControlBlock->psListTail				= IMG_NULL;
		psControlBlock->psStorageArray			= (LIST_MGR_psNavigationElement) psStorageArray; 
		psControlBlock->ui32SizeOfEachElement	= ui32SizeOfStorageElement;

		pCurrentElement = psStorageArray;

		/* Navigate through all elements, setting up the navigation fields in each */
		for ( i=0; i<ui32MaximumNoOfListElements; i++ )
		{
			pCurrentElement->bElementInUse							= IMG_FALSE;
			pCurrentElement->psNext									= IMG_NULL;
			pCurrentElement->psPrevious								= IMG_NULL;

			if ( i == (ui32MaximumNoOfListElements-1) )
			{
				pCurrentElement->RESERVED_psNextNavigationElement	= IMG_NULL;
			}
			else
			{
				PointerAdder	= (IMG_UINTPTR) pCurrentElement;
				PointerAdder	+= ui32SizeOfStorageElement;
				pCurrentElement->RESERVED_psNextNavigationElement	= (LIST_MGR_psNavigationElement) PointerAdder;
			}

			pCurrentElement = (LIST_MGR_psNavigationElement) pCurrentElement->RESERVED_psNextNavigationElement;
		}
	}

	return ReturnValue;
}


/*!
******************************************************************************

 @Function				LIST_MGR_GetElement
 
******************************************************************************/
IMG_RESULT	LIST_MGR_GetElement						( 	LIST_MGR_psControlBlock			psControlBlock,														
														IMG_VOID* *						ppElementToUse		)
{
	IMG_RESULT						ReturnValue			= LIST_MGR_ERR_NO_ERR;
	LIST_MGR_psNavigationElement	psNowViewing		= IMG_NULL;
	
#if LIST_MGR_PERFORM_ERROR_CHECKING

	if ( psControlBlock == IMG_NULL )
	{
		IMG_ASSERT(IMG_FALSE);
		ReturnValue = LIST_MGR_ERR_BAD_POINTER;
	}

#endif

	if ( ReturnValue == LIST_MGR_ERR_NO_ERR )
	{
		psNowViewing = psControlBlock->psStorageArray;

		/* Search for next available table entry */
		while (( psNowViewing->bElementInUse == IMG_TRUE ) &&
			   ( psNowViewing->RESERVED_psNextNavigationElement != IMG_NULL ))
		{
			psNowViewing = (LIST_MGR_psNavigationElement) psNowViewing->RESERVED_psNextNavigationElement;
		}

		if ( psNowViewing->bElementInUse == IMG_TRUE )
		{
			ReturnValue = LIST_MGR_ERR_NO_SPACE_REMAINING;
		}
	}

	if ( ReturnValue == LIST_MGR_ERR_NO_ERR )
	{
		psNowViewing->bElementInUse = IMG_TRUE;
		psNowViewing->psNext		= IMG_NULL;

		if ( psControlBlock->psListHead == IMG_NULL )
		{
			/* List was previously empty */
			psControlBlock->psListHead				= psNowViewing;
			psNowViewing->psPrevious				= IMG_NULL;
		}
		else
		{
			psNowViewing->psPrevious				= psControlBlock->psListTail;
			(psControlBlock->psListTail)->psNext	= psNowViewing;
		}

		/* This is now the last element of the linked list		*/
		psControlBlock->psListTail	= psNowViewing;

		/* Set the handle to this callback as the address of	*/
		/* element we have used to store it.					*/
		*ppElementToUse = psNowViewing;
	}

	return ReturnValue;
}

/*!
******************************************************************************

 @Function				LIST_MGR_FreeElement
 
******************************************************************************/
IMG_RESULT	LIST_MGR_FreeElement					( 	LIST_MGR_psControlBlock			psControlBlock,
														IMG_VOID*						pElementToFree	)
{
	IMG_RESULT						ReturnValue				= LIST_MGR_ERR_NO_ERR;
	IMG_UINTPTR					uiLastValidAddress;
	IMG_UINTPTR					uiRelativeAddress;
	LIST_MGR_psNavigationElement	sElementToRemove;

#if LIST_MGR_PERFORM_ERROR_CHECKING
	if ( psControlBlock == IMG_NULL )
	{
		IMG_ASSERT(IMG_FALSE);
		ReturnValue = LIST_MGR_ERR_BAD_POINTER;
	}
	else
	{
		uiLastValidAddress = ((IMG_UINTPTR) psControlBlock->psStorageArray) + 
								(psControlBlock->ui32MaximumNoOfElements * (psControlBlock->ui32SizeOfEachElement) );

		if (( (IMG_UINTPTR) pElementToFree < (IMG_UINTPTR) psControlBlock->psStorageArray ) ||
			( (IMG_UINTPTR) pElementToFree > uiLastValidAddress ))
		{
			IMG_ASSERT(IMG_FALSE);
			ReturnValue = LIST_MGR_ERR_BAD_POINTER;
		}
		else
		{
			uiRelativeAddress = (IMG_UINTPTR) pElementToFree - (IMG_UINTPTR) (psControlBlock->psStorageArray);

			if ( uiRelativeAddress % (psControlBlock->ui32SizeOfEachElement) != 0 )
			{
				IMG_ASSERT(IMG_FALSE);
				ReturnValue = LIST_MGR_ERR_BAD_POINTER;
			}
		}
	}

#endif

	if ( ReturnValue == LIST_MGR_ERR_NO_ERR )
	{
		sElementToRemove = (LIST_MGR_psNavigationElement) pElementToFree;

		/* Check that the referenced element isn't empty */
		if ( sElementToRemove->bElementInUse != IMG_FALSE )
		{
			/* Remove the indicated element */
			if ( sElementToRemove->psNext == IMG_NULL )
			{
				if ( sElementToRemove->psPrevious == IMG_NULL )
				{
					/* No forward or backward linking - this is the only element in the list */
					psControlBlock->psListHead = IMG_NULL;
					psControlBlock->psListTail = IMG_NULL;
				}
				else
				{
					/* No forward linking, but there is backward - this is the last element in the list */
					(sElementToRemove->psPrevious)->psNext = IMG_NULL;
					psControlBlock->psListTail = sElementToRemove->psPrevious;
				}

			}
			else
			{
				if ( sElementToRemove->psPrevious == IMG_NULL )
				{
					/* Forward linking, but no backward - this is the first element in the list */
					(sElementToRemove->psNext)->psPrevious = IMG_NULL;
					psControlBlock->psListHead = sElementToRemove->psNext;
				}
				else
				{
					/* Forward and backward linking - this element is in the middle of the list */
					(sElementToRemove->psNext)->psPrevious = sElementToRemove->psPrevious;
					(sElementToRemove->psPrevious)->psNext = sElementToRemove->psNext;
				}

			}

			/* Clear all fields of the element to remove */
			sElementToRemove->bElementInUse	= IMG_FALSE;
		}
		else
		{
			IMG_ASSERT(IMG_FALSE);
			ReturnValue = LIST_MGR_ERR_ELEMENT_EMPTY;
		}
	}

	return ReturnValue;
}

/*****************************************************************************/
/*	END OF FILE */
/*****************************************************************************/
