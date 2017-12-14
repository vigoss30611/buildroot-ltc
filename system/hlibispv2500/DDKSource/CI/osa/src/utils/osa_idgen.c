/*! 
******************************************************************************
 @file   : idgen_api.c

 @brief  The ID Generation API.

 @Author Imagination Technologies

 @date   05/01/2011
 
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

 \n<b>Description:</b>\n
         This file contains the ID Generation API.

 \n<b>Platform:</b>\n
	     Platform Independent 

******************************************************************************/
/* 
******************************************************************************
*****************************************************************************/

#include <osa_idgen.h>


/*!
******************************************************************************
 This structure contains the ID context.
 @brief  ID Context Structure
 ******************************************************************************/
typedef struct
{
	LST_T 					sHdlBlkList;		/*!< List of handle block structures.			    */
	IMG_UINT32				ui32MaxId;			/*!< Max ID - set by IDGEN_CreateContext().		    */
	IMG_UINT32				ui32BlkSize;		/*!< The number of handle per block. In case of
                                                    incrementing ids, size of the Hash table.       */
	IMG_UINT32				ui32FreeSlot;		/*!< Next free slot.							    */
	IMG_UINT32				ui32MaxSlotPlus1;	/*!< Max slot+1 for which we have allocated blocks.	*/

    /* Incrementing IDs */
    IMG_BOOL                bIncIds;            /*!< API needed to return incrementnig IDs          */
    IMG_UINT32              ui32IncNumber;      /*!< Latest ID given back                           */
    LST_T      *            asIncIdsList;       /*!< Array of list to hold IDGEN_sHdlId             */

} OSAIDGEN_sContext;

/*!
******************************************************************************
 This structure represents internal representation of an Incrementing ID
 @brief  ID Structure
 ******************************************************************************/
typedef struct
{
    LST_LINK;                   /*!< List link (allows the structure to be part of a MeOS list).*/

    IMG_UINT32  ui32IncId;      /*!< Incrementing ID returned                                   */
    IMG_HANDLE  hId;

} OSAIDGEN_sId;

/*!
******************************************************************************
 This structure contains the ID context.
 @brief  ID Context Structure
 ******************************************************************************/
typedef struct
{
	LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/

	IMG_HANDLE			ahHandles[1];		/*!< Array of handles in this block.		*/

} OSAIDGEN_sHdlBlk;


/*!
******************************************************************************

 @Function				idgen_func

 A hashing function could go here. Currently just makes a cicular list of max 
 number of concurrent Ids (psContext->ui32BlkSize) in the system.
******************************************************************************/
static IMG_UINT32  idgen_func(
    OSAIDGEN_sContext *    psContext,
    IMG_UINT32          ui32Id)
{
    return ((ui32Id-1) % psContext->ui32BlkSize);
}

/*!
******************************************************************************

 @Function				OSAIDGEN_CreateContext

******************************************************************************/
IMG_RESULT OSAIDGEN_CreateContext(
	IMG_UINT32					ui32MaxId,
	IMG_UINT32					ui32BlkSize,
    IMG_BOOL                    bIncId,
	IMG_HANDLE *				phIdGenHandle
)
{
	OSAIDGEN_sContext *			psContext;

	IMG_ASSERT(ui32MaxId > 0);
	IMG_ASSERT(ui32BlkSize > 0);

	/* Create context structure...*/
	psContext = IMG_MALLOC(sizeof(*psContext));
	IMG_ASSERT(psContext != IMG_NULL);
	if (psContext == IMG_NULL)
	{
		return IMG_ERROR_OUT_OF_MEMORY;
	}
	IMG_MEMSET(psContext, 0, sizeof(*psContext));

	/* Initailise the context...*/
	LST_init(&psContext->sHdlBlkList);
	psContext->ui32MaxId	= ui32MaxId;
	psContext->ui32BlkSize	= ui32BlkSize;

    /* If we need incrementing Ids */
    psContext->bIncIds = bIncId;
    psContext->ui32IncNumber = 0;
    psContext->asIncIdsList = IMG_NULL;
    if(psContext->bIncIds)
    {
        IMG_UINT32  i=0;

        // Initialise the hashtable of lists of length ui32BlkSize
        psContext->asIncIdsList = IMG_MALLOC(sizeof(*psContext->asIncIdsList) * psContext->ui32BlkSize);
        IMG_ASSERT(psContext->asIncIdsList != IMG_NULL);
        if (psContext == IMG_NULL)
        {
            return IMG_ERROR_OUT_OF_MEMORY;
        }
        IMG_MEMSET(psContext->asIncIdsList, 0, sizeof(*psContext->asIncIdsList) * psContext->ui32BlkSize);
        
        // Initialise all the lists in the hashtable
        for(i=0;i<psContext->ui32BlkSize;i++)
        {
            LST_init(&psContext->asIncIdsList[i]);
        }
    }

	/* Return context structure as handle...*/
	*phIdGenHandle = psContext;

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSAIDGEN_DestroyContext

******************************************************************************/
IMG_RESULT OSAIDGEN_DestroyContext(
	IMG_HANDLE					hIdGenHandle
)
{
	OSAIDGEN_sContext *		psContext = (OSAIDGEN_sContext *) hIdGenHandle;
	OSAIDGEN_sHdlBlk *			psHdlBlk;

	IMG_ASSERT(psContext != IMG_NULL);

    /* If incrementing Ids, free the List of Incrementing Ids */
    if(psContext->bIncIds)
    {
        OSAIDGEN_sId *  psId;
        IMG_UINT32      i=0;

        for(i=0;i<psContext->ui32BlkSize;i++)
        {
            psId = LST_removeHead(&psContext->asIncIdsList[i]);
            while(psId != IMG_NULL)
            {
                IMG_FREE(psId);
                psId = LST_removeHead(&psContext->asIncIdsList[i]);
            }
        }
        IMG_FREE(psContext->asIncIdsList);
    }

	/* Remove and free all handle blocks...*/
	psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_removeHead(&psContext->sHdlBlkList);
	while (psHdlBlk != IMG_NULL)
	{
		IMG_FREE(psHdlBlk);
		psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_removeHead(&psContext->sHdlBlkList);
	}

	/* Free context structure...*/
	IMG_FREE(psContext);

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				idgen_FindNextFreeSlot

******************************************************************************/
static IMG_VOID idgen_FindNextFreeSlot(
	IMG_HANDLE					hIdGenHandle,
	IMG_UINT32					ui32PrevFreeSlot
)
{
	OSAIDGEN_sContext *		psContext = (OSAIDGEN_sContext *) hIdGenHandle;
	OSAIDGEN_sHdlBlk *		psHdlBlk;
	IMG_UINT32				ui32FreeSlotBlk;
	IMG_UINT32				ui32FreeSlot;

	IMG_ASSERT(psContext != IMG_NULL);

	/* Find the block containing the current free slot...*/
	ui32FreeSlot	= ui32PrevFreeSlot;
	ui32FreeSlotBlk = ui32PrevFreeSlot;
	psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_first(&psContext->sHdlBlkList);
	IMG_ASSERT(psHdlBlk != IMG_NULL);
	while (ui32FreeSlotBlk >= psContext->ui32BlkSize)
	{
		ui32FreeSlotBlk -= psContext->ui32BlkSize;
		psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_next(psHdlBlk);
		IMG_ASSERT(psHdlBlk != IMG_NULL);
	}

	/* Locate the next free slot...*/
	while (psHdlBlk != IMG_NULL)
	{
		while (ui32FreeSlotBlk < psContext->ui32BlkSize)
		{
			if (psHdlBlk->ahHandles[ui32FreeSlotBlk] == IMG_NULL)
			{
				/* Found...*/
				psContext->ui32FreeSlot = ui32FreeSlot;
				return;
			}
			ui32FreeSlot++;
			ui32FreeSlotBlk++;
		}
		ui32FreeSlotBlk = 0;
		psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_next(psHdlBlk);
	}

	/* Beyond the last block...*/
	psContext->ui32FreeSlot = ui32FreeSlot;
	IMG_ASSERT(psContext->ui32FreeSlot <= psContext->ui32MaxId);
	IMG_ASSERT(psContext->ui32FreeSlot == psContext->ui32MaxSlotPlus1);
	return;
}


/*!
******************************************************************************

 @Function				idgen_GetId

 This function returns ID structure (IDGEN_sId) for ui32Id.

******************************************************************************/
static OSAIDGEN_sId * idgen_GetId(LST_T * psIdList, IMG_UINT32 ui32Id)
{
    OSAIDGEN_sId * psId;
    psId = LST_first(psIdList);
    while(psId)
    {
        if(psId->ui32IncId == ui32Id)
        {
            break;
        }
        psId = LST_next(psId);
    }

    return psId;
}


/*!
******************************************************************************

 @Function				OSAIDGEN_AllocId

******************************************************************************/
IMG_RESULT OSAIDGEN_AllocId(
	IMG_HANDLE					hIdGenHandle,
	IMG_HANDLE					hHandle,
	IMG_UINT32 *				pui32Id
)
{
    OSAIDGEN_sContext *		psContext = (OSAIDGEN_sContext *) hIdGenHandle;
    OSAIDGEN_sHdlBlk *		psHdlBlk;
	IMG_UINT32				ui32Size;
	IMG_UINT32				ui32FreeSlot;

	IMG_ASSERT(psContext != IMG_NULL);
	IMG_ASSERT(hHandle != IMG_NULL);

    if(!psContext->bIncIds)
    {
        /* If the free slot is >= to the max id...*/
        if (psContext->ui32FreeSlot >= psContext->ui32MaxId)
        {
            return IMG_ERROR_INVALID_ID;
        }

        /* If all of the allocated Ids have been used...*/
        if (psContext->ui32FreeSlot >= psContext->ui32MaxSlotPlus1)
        {
            /* Allocate a stream context...*/
            ui32Size = sizeof(*psHdlBlk)+(sizeof(IMG_HANDLE)*(psContext->ui32BlkSize-1));
            psHdlBlk = IMG_MALLOC(ui32Size);
            IMG_ASSERT(psHdlBlk != IMG_NULL);
            if (psHdlBlk == IMG_NULL)
            {
                return IMG_ERROR_OUT_OF_MEMORY;
            }
            IMG_MEMSET(psHdlBlk, 0, ui32Size);

            LST_add(&psContext->sHdlBlkList, psHdlBlk);
            psContext->ui32MaxSlotPlus1 += psContext->ui32BlkSize;
        }

        /* Find the block containing the next free slot...*/
        ui32FreeSlot = psContext->ui32FreeSlot;
        psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_first(&psContext->sHdlBlkList);
        IMG_ASSERT(psHdlBlk != IMG_NULL);
        while (ui32FreeSlot >= psContext->ui32BlkSize)
        {
            ui32FreeSlot -= psContext->ui32BlkSize;
            psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_next(psHdlBlk);
            IMG_ASSERT(psHdlBlk != IMG_NULL);
        }

        /* Put handle in the next free slot...*/
        IMG_ASSERT(psHdlBlk->ahHandles[ui32FreeSlot] == IMG_NULL);
        psHdlBlk->ahHandles[ui32FreeSlot] = hHandle;

        *pui32Id = psContext->ui32FreeSlot + 1;

        /* Find a new free slot...*/
        idgen_FindNextFreeSlot(psContext, psContext->ui32FreeSlot);
    }
    /* If incrementing IDs, just add the ID node to the correct hashtable list */
    else
    {
        OSAIDGEN_sId * psId;

        /* If incrementing IDs, increment the id for returning back, and save the ID node in the list
        of ids, indexed by hash function (idgen_func). We might want to use a better hashing function */

        /* Increment the id. Wraps if greater then Max Id */
        psContext->ui32IncNumber = (psContext->ui32IncNumber + 1) % psContext->ui32MaxId;
        if(psContext->ui32IncNumber == 0)
        {
            // Do not want to have zero as a pic id
            psContext->ui32IncNumber++;
        }


        psId = IMG_MALLOC(sizeof(*psId));
        psId->ui32IncId = psContext->ui32IncNumber;
        psId->hId = hHandle;

        /* Add to list in the correct hashtable entry */
        if( idgen_GetId(&psContext->asIncIdsList[idgen_func(psContext,psContext->ui32IncNumber)],psId->ui32IncId) == IMG_NULL )
        {            
            LST_add(&psContext->asIncIdsList[idgen_func(psContext,psContext->ui32IncNumber)],psId);
        }
        else
        {
            // We have reached a point where we have wrapped allowed Ids (MaxId) and we want to overwrite ID still not released.
            return IMG_ERROR_INVALID_ID;
        }

        *pui32Id = psId->ui32IncId;
    }

    

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSAIDGEN_FreeId

******************************************************************************/
IMG_RESULT OSAIDGEN_FreeId(
	IMG_HANDLE					hIdGenHandle,
	IMG_UINT32  				ui32Id
)
{
	OSAIDGEN_sContext *		psContext = (OSAIDGEN_sContext *) hIdGenHandle;
	OSAIDGEN_sHdlBlk *			psHdlBlk;
    IMG_UINT32              ui32OrigSlot;
	IMG_UINT32				ui32Slot;

	IMG_ASSERT(ui32Id != 0);

    if(psContext->bIncIds)
    {
        // Find the slot in the correct hashtable entry, and remove the ID
        OSAIDGEN_sId * psId;
        psId = idgen_GetId(&psContext->asIncIdsList[idgen_func(psContext,ui32Id)],ui32Id);                
        if(psId != IMG_NULL)
        {
            LST_remove(&psContext->asIncIdsList[idgen_func(psContext,ui32Id)],psId);
            IMG_FREE(psId);
        }
        else
        {
            return IMG_ERROR_INVALID_ID;
        }
    }
    else
    {
        // If not incrementing id
        ui32Slot = ui32Id - 1;
        ui32OrigSlot = ui32Slot;

        IMG_ASSERT(ui32Slot < psContext->ui32MaxSlotPlus1);
        if (ui32Slot >= psContext->ui32MaxSlotPlus1)
        {
            return IMG_ERROR_INVALID_ID;
        }

        /* Find the block containing the id...*/
        psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_first(&psContext->sHdlBlkList);
        IMG_ASSERT(psHdlBlk != IMG_NULL);
        while (ui32Slot >= psContext->ui32BlkSize)
        {
            ui32Slot -= psContext->ui32BlkSize;
            psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_next(psHdlBlk);
            IMG_ASSERT(psHdlBlk != IMG_NULL);
        }

        /* Slot should be occupied...*/
        IMG_ASSERT(psHdlBlk->ahHandles[ui32Slot] != IMG_NULL);
        if (psHdlBlk->ahHandles[ui32Slot] == IMG_NULL)
        {
            return IMG_ERROR_INVALID_ID;
        }

        /* Free slot...*/
        psHdlBlk->ahHandles[ui32Slot] = IMG_NULL;

        /* If this slot is before the previous free slot...*/
        if ((ui32OrigSlot) < psContext->ui32FreeSlot)
        {
            psContext->ui32FreeSlot = ui32OrigSlot;
        }
    }

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSAIDGEN_GetHandle

******************************************************************************/
IMG_RESULT OSAIDGEN_GetHandle(
	IMG_HANDLE					hIdGenHandle,
	IMG_UINT32  				ui32Id,
	IMG_HANDLE *				phHandle
)
{
	OSAIDGEN_sContext *		psContext = (OSAIDGEN_sContext *) hIdGenHandle;
	OSAIDGEN_sHdlBlk *			psHdlBlk;
	IMG_UINT32				ui32Slot;

	IMG_ASSERT(psContext != IMG_NULL);
	IMG_ASSERT(ui32Id != 0);


    
    if(psContext->bIncIds)
    {
        // Find the slot in the correct hashtable entry, and return the handle
        OSAIDGEN_sId * psId;
        psId = idgen_GetId(&psContext->asIncIdsList[idgen_func(psContext,ui32Id)],ui32Id); 
        if(psId != IMG_NULL)
        {
            *phHandle = psId->hId;
        }
        else
        {
            return IMG_ERROR_INVALID_ID;
        }
    }
    else
    {
        // If not incrementing IDs
        ui32Slot = ui32Id - 1;

        IMG_ASSERT(ui32Slot < psContext->ui32MaxSlotPlus1);
        if (ui32Slot >= psContext->ui32MaxSlotPlus1)
        {
            return IMG_ERROR_INVALID_ID;
        }

        /* Find the block containing the id...*/
        psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_first(&psContext->sHdlBlkList);
        IMG_ASSERT(psHdlBlk != IMG_NULL);
        while (ui32Slot >= psContext->ui32BlkSize)
        {
            ui32Slot -= psContext->ui32BlkSize;
            psHdlBlk = (OSAIDGEN_sHdlBlk *)LST_next(psHdlBlk);
            IMG_ASSERT(psHdlBlk != IMG_NULL);
        }

        /* Slot should be occupied...*/
        IMG_ASSERT(psHdlBlk->ahHandles[ui32Slot] != IMG_NULL);
        if (psHdlBlk->ahHandles[ui32Slot] == IMG_NULL)
        {
            return IMG_ERROR_INVALID_ID;
        }

        /* Return the handle...*/
        *phHandle = psHdlBlk->ahHandles[ui32Slot];
    }

	return IMG_SUCCESS;
}

