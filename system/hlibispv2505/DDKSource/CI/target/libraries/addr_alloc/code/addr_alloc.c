/*!
******************************************************************************
 @file   : addr_alloc.c

 @brief

 @Author Imagination Technologies

 @date   19/10/2005

         <b>Copyright 2005 by Imagination Technologies Limited.</b>\n
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
         Address allocation APIs - used to manage address allocation
		 with a number of predefined regions.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <addr_alloc.h>
#include <hash.h>

#if !defined (IMG_KERNEL_MODULE)
#include <stdio.h> // printf
#endif

#ifdef __cplusplus
#define PREF extern "C"
#else
#define PREF
#endif

/* Defines whether sequential or random allocation is used */
enum
{
	SEQUENTIAL_ALLOCATION,
	RANDOM_ALLOCATION
};

#define RANDOM_BLOCK_SEED 0x82647868
static ADDR_sContext        gsContext;                  /*!< Global contaxt.        				*/

static IMG_BOOL	            bInitalised = IMG_FALSE;    /*!< Sub-system initialised.				*/
static IMG_UINT32           gui32NoContext = 0;         /*!< Count of contexts.     				*/

/*!
******************************************************************************

 @Function				ADDR_Initialise

******************************************************************************/
PREF IMG_VOID ADDR_Initialise()
{
	/* If we are not initialised...*/
	if (!bInitalised)
	{
        ADDR_CxInitialise(&gsContext);
    }
}
/*!
******************************************************************************

 @Function				ADDR_CxInitialise

******************************************************************************/
PREF IMG_VOID ADDR_CxInitialise(
    ADDR_sContext *        psContext
)
{
	/* If we are not initialised...*/
	if (!bInitalised)
	{
		/* Initialise the hash functions..*/
		if (!HASH_Initialise ())
		{
			IMG_ASSERT(IMG_FALSE);
		}

		/* Initialise the arena functions...*/
		if (!RA_Initialise ())
		{
			IMG_ASSERT(IMG_FALSE);
		}
#if !defined (IMG_KERNEL_MODULE)
		/* Seed the random number generator */
		srand(RANDOM_BLOCK_SEED);
#endif
		/* We are now initalised */
		bInitalised = IMG_TRUE;
	}

    /* Initialise context...*/
    psContext->psDefaultRegion  = IMG_NULL;
    psContext->psRegions        = IMG_NULL;
	psContext->bUseRandomBlocks = IMG_FALSE;
	psContext->bUseRandomAllocation = IMG_FALSE;
	psContext->ui32NoRegions    = 0;
    gui32NoContext++;
}

/*!
******************************************************************************

 @Function				ADDR_Initialise

******************************************************************************/
PREF IMG_VOID ADDR_Deinitialise()
{
    ADDR_CxDeinitialise(&gsContext);
}
/*!
******************************************************************************

 @Function				ADDR_CxDeinitialise

 ******************************************************************************/
IMG_VOID ADDR_CxDeinitialise(
    ADDR_sContext *        psContext
)
{
	ADDR_sRegion *		psTmpRegion = psContext->psRegions;

    IMG_ASSERT(gui32NoContext != 0);
    /* Delete all arena structure...*/
    if (psContext->psDefaultRegion != IMG_NULL)
    {
        RA_Delete(psContext->psDefaultRegion->psArena);
    }
    while (psTmpRegion != IMG_NULL)
    {
        RA_Delete(psTmpRegion->psArena);
        psTmpRegion = psTmpRegion->psNextRegion;
    }
    gui32NoContext--;

    /* If initailised...*/
    if ( (bInitalised) && (gui32NoContext == 0) )
    {
        /* Free off resources...*/
        HASH_Finalise();
        RA_Finalise();

        bInitalised = IMG_FALSE;
    }
}



/*!
******************************************************************************

 @Function				ADDR_DefineMemoryRegion

******************************************************************************/
IMG_VOID ADDR_DefineMemoryRegion(
	ADDR_sRegion *		psRegion
)
{
    ADDR_CxDefineMemoryRegion(&gsContext, psRegion);
}
/*!
******************************************************************************

 @Function				ADDR_CxDefineMemoryRegion

******************************************************************************/
IMG_VOID ADDR_CxDefineMemoryRegion(
    ADDR_sContext *         psContext,
	ADDR_sRegion *		    psRegion
)
{
	ADDR_sRegion *		psTmpRegion = psContext->psRegions;

	/* Ensure the link to the next is NULL...*/
	psRegion->psNextRegion = IMG_NULL;

	/* If this is the default memory region...*/
	if (psRegion->pszName == IMG_NULL)
	{
		/* Should not previously have been defined...*/
		IMG_ASSERT(psContext->psDefaultRegion == IMG_NULL);
		psContext->psDefaultRegion = psRegion;
		psContext->ui32NoRegions ++;

		/* Create an arena for memory allocation */
		psRegion->psArena = RA_Create (
			"memory",					/* name of resource arena for debug */
			psRegion->ui64BaseAddr,		/* start of resource */
			psRegion->ui64Size,			/* size of resource */
			1,							/* allocation quantum */
			IMG_NULL,					/* import allocator */
			IMG_NULL,					/* import deallocator */
			IMG_NULL);					/* import handle */
	}
	else
	{
		/* Run down the list of existing named regions to check if
		   there is a region with this name...*/
		while (
				(psTmpRegion != IMG_NULL) &&
				(IMG_STRCMP(psRegion->pszName, psTmpRegion->pszName) != 0) &&
				(psTmpRegion->psNextRegion != IMG_NULL)
			)
		{
			psTmpRegion = psTmpRegion->psNextRegion;
		}

		/* If we have items in the list...*/
		if (psTmpRegion != IMG_NULL)
		{
			/* Check we didn't stop because the name clashes with one already defined..*/
			IMG_ASSERT(IMG_STRCMP(psRegion->pszName, psTmpRegion->pszName) != 0);
			IMG_ASSERT(psTmpRegion->psNextRegion == IMG_NULL);

			/* Add to end of list...*/
			psTmpRegion->psNextRegion = psRegion;
		}
		else
		{
			/* Add to head of list */
			psContext->psRegions = psRegion;
		}
		psContext->ui32NoRegions ++;
		/* Create an arena for memory allocation */
		psRegion->psArena = RA_Create (
			psRegion->pszName,			/* name of resource arena for debug */
			psRegion->ui64BaseAddr,		/* start of resource */
			psRegion->ui64Size,			/* size of resource */
			1,							/* allocation quantum */
			IMG_NULL,					/* import allocator */
			IMG_NULL,					/* import deallocator */
			IMG_NULL);					/* import handle */
	}

	/* Check the arean was created OK...*/
	if (psRegion->psArena==IMG_NULL)
	{
		IMG_ASSERT(IMG_FALSE);
	}

}


/*!
******************************************************************************

 @Function				ADDR_Malloc

******************************************************************************/
IMG_UINT64 ADDR_Malloc(
	IMG_CHAR *			pszName,
	IMG_UINT64			ui64Size
)
{
    return ADDR_CxMalloc(&gsContext, pszName, ui64Size);
}


/*!
******************************************************************************

 @Function				ADDR_Malloc1

******************************************************************************/
IMG_UINT64 ADDR_Malloc1(
	IMG_CHAR *			    pszName,
	IMG_UINT32			    ui32Size,
	IMG_UINT32              ui32Alignment
)
{
    return ADDR_CxMalloc1(&gsContext, pszName, ui32Size, ui32Alignment);
}


/*!
******************************************************************************

 @Function				addr_CxMalloc1

******************************************************************************/
static IMG_RESULT addr_CxMalloc1(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64              ui64Alignment,
	IMG_UINT64 *			pui64Base
)
{
	ADDR_sRegion *		    psTmpRegion;
	IMG_UINT32				ui32Flags;

	for(;;)
	{
		psTmpRegion = psContext->psRegions;
		if (psContext->bUseRandomBlocks) /* If we are using random blocks, ignore the given blockname */
		{
#if !defined (IMG_KERNEL_MODULE)
			IMG_UINT32 ui32BlockNo;
			/* Select a random number from 0 to num blocks -1 */
			while (psContext->ui32NoRegions <= (ui32BlockNo = rand() / (RAND_MAX/psContext->ui32NoRegions)));

			/* 0 counts as default region */
			if (0 == ui32BlockNo)
			{
				IMG_ASSERT(psContext->psDefaultRegion != IMG_NULL);
				psTmpRegion = psContext->psDefaultRegion;
			}
			else
			{
				/* Count through the region list to the random one which has been selected */
				while (ui32BlockNo > 1)
				{
					IMG_ASSERT(psTmpRegion->psNextRegion);
					psTmpRegion = psTmpRegion->psNextRegion;
					ui32BlockNo --;
				}
			}
#else
			IMG_ASSERT(IMG_FALSE);	/* Not supported in kernel mode */
			return IMG_ERROR_OUT_OF_MEMORY;
#endif
		}
		else if (pszName == IMG_NULL) /* If the allocation is for the default region...*/
		{
			IMG_ASSERT(psContext->psDefaultRegion != IMG_NULL);
			psTmpRegion = psContext->psDefaultRegion;
		}
		else
		{
			/* Run down the list of existing named regions to locate this...*/
			while (
					(psTmpRegion != IMG_NULL) &&
					(IMG_STRCMP(pszName, psTmpRegion->pszName) != 0) &&
					(psTmpRegion->psNextRegion)
				)
			{
				psTmpRegion = psTmpRegion->psNextRegion;
			}

			/* If there was no match....*/
			if (
					(psTmpRegion == IMG_NULL) ||
					(IMG_STRCMP(pszName, psTmpRegion->pszName) != 0)
				)
			{
				/* Use the default...*/
				IMG_ASSERT(psContext->psDefaultRegion != IMG_NULL);
				psTmpRegion = psContext->psDefaultRegion;
			}
		}
		
		if (psContext->bUseRandomAllocation)
			ui32Flags = RANDOM_ALLOCATION;
		else
			ui32Flags = SEQUENTIAL_ALLOCATION;

		/* Allocate size + guard band...*/
		if (!RA_Alloc (psTmpRegion->psArena, ui64Size + psTmpRegion->ui32GuardBand, IMG_NULL, IMG_NULL, ui32Flags, ui64Alignment, pui64Base))
		{
			if (psContext->bUseRandomBlocks)
			{
				continue; /* If we have overrun, pick a different region */
			}
#if !defined (IMG_KERNEL_MODULE)
			if (psTmpRegion->pszName)
			{
				printf("ERROR: Memory Region %s is full\n", psTmpRegion->pszName);
			}
			else
			{
				printf("ERROR: Default Memory Region is full\n");
			}
#endif			
			IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_OUT_OF_MEMORY;
		}
		break;
	}

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				ADDR_CxMalloc

******************************************************************************/
IMG_UINT64 ADDR_CxMalloc(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size
)
{
	IMG_RESULT ui32Result;
	IMG_UINT64 ui64Base = (IMG_UINT64)-1LL;

	ui32Result = addr_CxMalloc1(psContext, pszName, ui64Size, 1, &ui64Base);

	if (ui32Result != IMG_SUCCESS)
	{
		IMG_ASSERT(IMG_FALSE);
	}

	return ui64Base;
}


/*!
******************************************************************************

 @Function				ADDR_CxMalloc1

******************************************************************************/
IMG_UINT64 ADDR_CxMalloc1(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64              ui64Alignment
)
{
	IMG_RESULT ui32Result;
	IMG_UINT64 ui64Base = (IMG_UINT64)-1LL;

	ui32Result = addr_CxMalloc1(psContext, pszName, ui64Size, ui64Alignment, &ui64Base);

	if (ui32Result != IMG_SUCCESS)
	{
		IMG_ASSERT(IMG_FALSE);
	}

	return ui64Base;
}


/*!
******************************************************************************

 @Function				ADDR_CxMallocRes

******************************************************************************/
IMG_RESULT ADDR_CxMallocRes(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64 *			pui64Base
)
{
	return addr_CxMalloc1(psContext, pszName, ui64Size, 1, pui64Base);
}


/*!
******************************************************************************

 @Function				ADDR_CxMalloc1Res

******************************************************************************/
IMG_RESULT ADDR_CxMalloc1Res(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64              ui64Alignment,
	IMG_UINT64 *			pui64Base
)
{
	return addr_CxMalloc1(psContext, pszName, ui64Size, ui64Alignment, pui64Base);
}



/*!
******************************************************************************

 @Function				ADDR_Free

******************************************************************************/
IMG_VOID ADDR_Free(
	IMG_CHAR *			pszName,
	IMG_UINT64			ui64Addr
)
{
    ADDR_CxFree(&gsContext, pszName, ui64Addr);
}
/*!
******************************************************************************

 @Function				ADDR_Free

******************************************************************************/
IMG_VOID ADDR_CxFree(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Addr
)
{
	ADDR_sRegion *		psTmpRegion = psContext->psRegions;

	if (psContext->bUseRandomBlocks) /* Ignore given name, find using the address */
	{
		/* Try Default Region */
		psTmpRegion = psContext->psDefaultRegion;
		if ( (ui64Addr >= psTmpRegion->ui64BaseAddr)
			&& (ui64Addr < psTmpRegion->ui64BaseAddr + psTmpRegion->ui64Size) )
		{ /* Memory is on the default region */}
		else
		{
			psTmpRegion = psContext->psRegions;
			/* Continue looping while address is not inside current region */
			while ( (ui64Addr < psTmpRegion->ui64BaseAddr) 
				|| (ui64Addr >= psTmpRegion->ui64BaseAddr + psTmpRegion->ui64Size) )
			{
				IMG_ASSERT(psTmpRegion->psNextRegion);
				psTmpRegion = psTmpRegion->psNextRegion;
			}
		}

	}
	else if (pszName == IMG_NULL) /* If the allocation is for the default region...*/
	{
		IMG_ASSERT(psContext->psDefaultRegion != IMG_NULL);
		psTmpRegion = psContext->psDefaultRegion;
	}
	else
	{
		/* Run down the list of existing named regions to locate this...*/
		while (
				(psTmpRegion != IMG_NULL) &&
				(IMG_STRCMP(pszName, psTmpRegion->pszName) != 0) &&
				(psTmpRegion->psNextRegion)
			)
		{
			psTmpRegion = psTmpRegion->psNextRegion;
		}

		/* If there was no match....*/
		if (
				(psTmpRegion == IMG_NULL) ||
				(IMG_STRCMP(pszName, psTmpRegion->pszName) != 0)
			)
		{
			/* Use the default...*/
			IMG_ASSERT(psContext->psDefaultRegion != IMG_NULL);
			psTmpRegion = psContext->psDefaultRegion;
		}
	}

	/* Free the address...*/
	RA_Free (psTmpRegion->psArena, ui64Addr, IMG_NULL);
}
