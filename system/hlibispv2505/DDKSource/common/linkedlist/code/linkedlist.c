/**
***************************************************************************
 @file linkedlist.c

 @brief Template double linked list - implementation

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

#include "linkedlist.h"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

IMG_RESULT List_init(sLinkedList_T *pList)
{
	if ( pList == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
		
	pList->sAnchor.pNext = &(pList->sAnchor);
	pList->sAnchor.pPrev = &(pList->sAnchor);
	pList->ui32Elements = 0;
	
	return IMG_SUCCESS;
}

IMG_RESULT List_initPool(sLinkedList_T *pList, IMG_UINT32 ui32nelem, void* param, void* pfnConstructor(void* param))
{
	IMG_RESULT ret;
	IMG_UINT32 i;
		
	if ( (ret =	List_init(pList)) != IMG_SUCCESS )
	{
		return ret;
	}

	for ( i = 0 ; i < ui32nelem ; i++ )
	{
		if ( pfnConstructor != NULL )
		{
			ret = List_pushBackObject(pList, pfnConstructor(param));
		}
		else // no constructor given
		{
			ret = List_pushBackObject(pList, NULL); // just create an empty cell
		}

		if ( ret != IMG_SUCCESS )
		{
			return ret;
		}
	}

	return IMG_SUCCESS;
}

IMG_RESULT List_clear(sLinkedList_T* pList)
{
	sCell_T* pCurr;
	sCell_T* pNext;

	if ( pList == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	pCurr = pList->sAnchor.pNext; // head

	while ( pCurr != &(pList->sAnchor) )
	{
		pNext = pCurr->pNext;

		IMG_FREE(pCurr);

		pCurr = pNext;
	}

	pList->ui32Elements = 0;
	pList->sAnchor.pNext = &(pList->sAnchor);
	pList->sAnchor.pPrev = &(pList->sAnchor);

	return IMG_SUCCESS;
}

IMG_RESULT List_clearObjects(sLinkedList_T* pList, void pfnDestructor(void*))
{
	sCell_T* pCurr;
	sCell_T* pNext;

	if ( pList == NULL || pfnDestructor == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	pCurr = pList->sAnchor.pNext; // Head

	while ( pCurr != &(pList->sAnchor) )
	{
		pNext = pCurr->pNext;

		pfnDestructor( pCurr->object );
		IMG_FREE( pCurr );

		pCurr = pNext;
	}

	pList->ui32Elements = 0;
	pList->sAnchor.pNext = &(pList->sAnchor);
	pList->sAnchor.pPrev = &(pList->sAnchor);

	return IMG_SUCCESS;
}

IMG_RESULT List_pushBackObject(sLinkedList_T* pList, void* elem)
{
	sCell_T* pNeo;
	IMG_RESULT ret;
  
	if ( pList == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
  
	pNeo = (sCell_T*)IMG_MALLOC(sizeof(sCell_T));

	if ( pNeo == NULL )
	{
		return IMG_ERROR_MALLOC_FAILED;
	}

	pNeo->object = elem;
	pNeo->pNext = NULL;
	pNeo->pPrev = NULL;
	pNeo->pContainer = NULL;
	
	// cannot fail: pNeo is not NULL and not attached
	ret = List_pushBack(pList, pNeo);
	IMG_ASSERT(ret == IMG_SUCCESS);
	return IMG_SUCCESS;
}

IMG_RESULT List_pushFrontObject(sLinkedList_T *pList, void *elem)
{
	sCell_T* pNeo;
	IMG_RESULT ret;
  
	if ( pList == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
  
	pNeo = (sCell_T*)IMG_MALLOC(sizeof(sCell_T));
	
	if ( pNeo == NULL )
	{
		return IMG_ERROR_MALLOC_FAILED;
	}
	
	pNeo->object = elem;
	pNeo->pNext = NULL;
	pNeo->pPrev = NULL;
	pNeo->pContainer = NULL;
	      
	// cannot fail: pNeo is not NULL and does not have a container
	ret = List_pushFront(pList, pNeo);
	IMG_ASSERT(ret == IMG_SUCCESS);
	return ret;
}

IMG_RESULT List_pushBack(sLinkedList_T *pList, sCell_T *pCell)
{
	if ( pList == NULL || pCell == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( pCell->pContainer != NULL )
	{
		return IMG_ERROR_UNEXPECTED_STATE;
	}

	pCell->pNext = &(pList->sAnchor); // no next
	pCell->pPrev = pList->sAnchor.pPrev; // prev is current tail
	pCell->pContainer = pList;

	pList->sAnchor.pPrev->pNext = pCell; // old tail's next is new tail
	// if list is empty old tail's next is head
	
	pList->sAnchor.pPrev = pCell; // new tail
	pList->ui32Elements++;

	return IMG_SUCCESS;
}

IMG_RESULT List_pushFront(sLinkedList_T *pList, sCell_T *pCell)
{
	if ( pList == NULL || pCell == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( pCell->pContainer != NULL )
	{
		return IMG_ERROR_UNEXPECTED_STATE;
	}

	pCell->pNext = pList->sAnchor.pNext; // next is current head
	pCell->pPrev = &(pList->sAnchor);
	pCell->pContainer = pList;

	pList->sAnchor.pNext->pPrev = pCell; // old head's next is new one
	// if list is emtpy they old head's next is anchor, anchor's prev is tail

	pList->sAnchor.pNext = pCell; // new head
	pList->ui32Elements++;

	return IMG_SUCCESS;
}

IMG_RESULT List_detach(sCell_T* pCell)
{
	sLinkedList_T *pList;
	sCell_T *pPrev = NULL, *pNext = NULL;

	if ( pCell == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	pList = pCell->pContainer;

	if ( pList == NULL )
	{
		return IMG_ERROR_UNEXPECTED_STATE;
	}

	pPrev = pCell->pPrev;
	pNext = pCell->pNext;

	pPrev->pNext = pNext;
	pNext->pPrev = pPrev;
		
	pList->ui32Elements--;

	pCell->pContainer = NULL;
	pCell->pPrev = NULL;
	pCell->pNext = NULL;

	return IMG_SUCCESS;
}

IMG_RESULT List_remove(sCell_T* pCell)
{
	IMG_RESULT ret;

	if ( (ret = List_detach(pCell)) == IMG_SUCCESS )
	{
		IMG_FREE(pCell);
	}

	return ret;
}

IMG_RESULT List_removeObject(sCell_T *pCell, void pfnDestructor(void* param))
{
	void* obj;
	IMG_RESULT ret;

	if ( pCell == NULL || pfnDestructor == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	obj = pCell->object;
	
	if ( (ret = List_remove(pCell)) == IMG_SUCCESS )
	{
		pfnDestructor( obj );
	}
	return ret;
}

sCell_T* List_popBack(sLinkedList_T *pList)
{
	sCell_T* pTail;

	if ( pList == NULL )
	{
		return NULL;
	}

	pTail = pList->sAnchor.pPrev; // tail
	if ( pTail == &(pList->sAnchor) )
	{
		return NULL; // no tail
	}

	if ( List_detach(pTail) == IMG_SUCCESS )
	{
		return pTail;
	}
	// above should not fail
	return NULL;
}

sCell_T* List_popFront(sLinkedList_T *pList)
{
	sCell_T* pHead;

	if ( pList == NULL )
	{
		return NULL;
	}

	pHead = pList->sAnchor.pNext; // head
	if ( pHead == &(pList->sAnchor) )
	{
		return NULL; // no head
	}
	
	if ( List_detach(pHead) == IMG_SUCCESS )
	{
		return pHead;
	}
	return NULL;
}

sCell_T* List_getHead(sLinkedList_T *pList)
{
	if ( pList != NULL )
	{
		if ( pList->sAnchor.pNext != &(pList->sAnchor) )
		{
			return pList->sAnchor.pNext;
		}
	}
	return NULL;
}

sCell_T* List_getTail(sLinkedList_T *pList)
{
	if ( pList != NULL )
	{
		if ( pList->sAnchor.pPrev != &(pList->sAnchor) )
		{
			return pList->sAnchor.pPrev;
		}
	}
	return NULL;
}

sCell_T* List_getNext(sCell_T *pCell)
{
	// orphan cell or tail
	if ( pCell->pContainer == NULL || pCell->pNext == &(pCell->pContainer->sAnchor) )
	{
		return NULL;
	}

	return pCell->pNext;
}

sCell_T* List_getPrev(sCell_T *pCell)
{
	// orphan cell or head
	if ( pCell->pContainer == NULL || pCell->pPrev == &(pCell->pContainer->sAnchor) )
	{
		return NULL;
	}

	return pCell->pPrev;
}

IMG_BOOL8 List_isEmpty(sLinkedList_T *pList)
{
	if ( pList != NULL && pList->ui32Elements > 0 )
	{
		return IMG_FALSE;
	}
	return IMG_TRUE;
}

IMG_UINT32 List_nElem(sLinkedList_T *pList)
{
	if ( pList != NULL )
	{
		return pList->ui32Elements;
	}
	return 0;
}

sCell_T* List_visitor(sLinkedList_T *pList, void* param, IMG_BOOL8 pfnVisitor(void* elem, void *param))
{
	sCell_T* pCurr;

	if (pList == NULL || pfnVisitor == NULL )
	{
		return NULL;
	}

	pCurr = pList->sAnchor.pNext; // head

	while ( pCurr != &(pList->sAnchor) )
	{
		sCell_T *pNext = pCurr->pNext;

		if ( pfnVisitor(pCurr->object, param) != IMG_TRUE )
			break;

		pCurr = pNext;
	}

	if ( pCurr == &(pList->sAnchor) )
		return NULL;
	return pCurr;
}

IMG_BOOL8 ListVisitor_findPointer(void* listElem, void* lookingFor)
{
	if ( listElem == lookingFor )
	{
		return IMG_FALSE; // stop looking
	}
	return IMG_TRUE;
}
