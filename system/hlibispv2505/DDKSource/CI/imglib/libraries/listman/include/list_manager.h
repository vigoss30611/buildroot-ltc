/*!
******************************************************************************
 @file   : list_manager.h

 @brief

 @Author Imagination Technologies

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
         List Manager Header File

 <b>Platform:</b>\n
         Platform Independent.

 @Version
    -    1.0 First Release

******************************************************************************/
/*
******************************************************************************
*/


#if !defined (__LIST_MGR_H__)
#define __LIST_MGR_H__

#include "img_errors.h"
#include "img_types.h"
#include "img_defs.h"

/* This line just means we can write C here and C++ elsewhere when it
** picks up the header file will know that all these functions are
** mangled up C wise instead of C++ wise.  See also the } way below.
*/
#ifdef __cplusplus
    extern "C" {
#endif

/*-------------------------------------------------------------------------------*/

/*!
******************************************************************************

 Error Checking Flag

******************************************************************************/
#define	LIST_MGR_PERFORM_ERROR_CHECKING				1		/* 1 - Error checking on, 0 - Error checking off */

/*!
******************************************************************************

 These defines represent the errors that can be returned from this API

******************************************************************************/
#define LIST_MGR_ERR_NO_ERR							IMG_SUCCESS

#define LIST_MGR_ERR_BASE							((IMG_RESULT)0x10)

#define LIST_MGR_ERR_BAD_POINTER					(LIST_MGR_ERR_BASE + 0)
#define LIST_MGR_ERR_OUT_OF_RANGE					(LIST_MGR_ERR_BASE + 1)
#define LIST_MGR_ERR_NO_SPACE_REMAINING				(LIST_MGR_ERR_BASE + 2)
#define LIST_MGR_ERR_ELEMENT_EMPTY					(LIST_MGR_ERR_BASE + 3)
#define LIST_MGR_ERR_UNKNOWN_COMMAND				(LIST_MGR_ERR_BASE + 4)
#define	LIST_MGR_ERR_LIST_EMPTY						(LIST_MGR_ERR_BASE + 5)


/*!
******************************************************************************

	This structure is used to link and navigate the user defined structures
	The user's structure must include an instance of this structure as its
	first member.

******************************************************************************/
typedef struct LIST_MGR_tag_sNavigationElement
{
	/* User should use these pointers to move through the linked list		*/
	struct		LIST_MGR_tag_sNavigationElement *	psNext;
	struct		LIST_MGR_tag_sNavigationElement *	psPrevious;

	/* For internal use only - not to be used for list navigation by user	*/
	struct		LIST_MGR_tag_sNavigationElement *	RESERVED_psNextNavigationElement;

	IMG_BOOL	bElementInUse;

} LIST_MGR_sNavigationElement, * LIST_MGR_psNavigationElement;

/*!
******************************************************************************

	List Manager control block structure

******************************************************************************/
typedef struct LIST_MGR_tag_sControlBlock
{
	IMG_UINT32						ui32MaximumNoOfElements;		/* The number of storage elements available	*/
	IMG_UINT32						ui32SizeOfEachElement;			/* The size of each user defined structure	*/

	LIST_MGR_psNavigationElement	psStorageArray;					/* Pointer to the list of user defined		*/
																	/* structures								*/
	LIST_MGR_psNavigationElement	psListHead;						/* First 'full' storage element				*/
	LIST_MGR_psNavigationElement	psListTail;						/* Last 'full' storage element				*/

} LIST_MGR_sControlBlock, * LIST_MGR_psControlBlock;


/*!
******************************************************************************

 @Function				LIST_MGR_Initialise

******************************************************************************/
IMG_RESULT	LIST_MGR_Initialise					(	LIST_MGR_psControlBlock			psControlBlock,
													IMG_UINT32						ui32MaximumNoOfListElements,
													IMG_VOID*						psStorageArray,
													IMG_UINT32						ui32SizeOfStorageElement	);

/*!
******************************************************************************

 @Function				LIST_MGR_Deinitialise

******************************************************************************/
IMG_RESULT	LIST_MGR_Deinitialise				(	IMG_VOID	);


/*!
******************************************************************************

 @Function				LIST_MGR_GetElement

******************************************************************************/
IMG_RESULT	LIST_MGR_GetElement					( 	LIST_MGR_psControlBlock			psControlBlock,
													IMG_VOID**						ppElementToUse				);
/*!
******************************************************************************

 @Function				LIST_MGR_FreeElement

******************************************************************************/
IMG_RESULT	LIST_MGR_FreeElement				( 	LIST_MGR_psControlBlock			psControlBlock,
													IMG_VOID*						pElementToFree				);


/*-------------------------------------------------------------------------------*/

#ifdef __cplusplus
	}
#endif

#endif /* __LIST_MGR_H__	*/

