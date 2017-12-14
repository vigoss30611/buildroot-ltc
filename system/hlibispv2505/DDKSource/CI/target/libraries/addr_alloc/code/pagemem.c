/*! 
******************************************************************************
 @file   : pagemen.c

 @brief  

 @Author Bojan Lakicevic

 @date   18/04/2006
 
         <b>Copyright 2006 by Imagination Technologies Limited.</b>\n
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
         SGX MMU code for wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#include <pagemem.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "img_defs.h"

static IMG_BOOL		bInitialised = IMG_FALSE;

static PAGEMEM_sContext gsContext;


/*!
******************************************************************************

 @Function              PAGEMEM_Initialise

******************************************************************************/
IMG_VOID PAGEMEM_Initialise (
    IMG_UINT32          ui32MemoryBase,
    IMG_UINT32          ui32MemorySize,
    IMG_UINT32          ui32PageSize
)
{
    /* Only allow this to be called once...*/
    IMG_ASSERT (!bInitialised);
    bInitialised = IMG_TRUE;
    PAGEMEM_CxInitialise(&gsContext, ui32MemoryBase, ui32MemorySize, ui32PageSize);
}
/*!
******************************************************************************

 @Function              PAGEMEM_CxInitialise

******************************************************************************/
IMG_VOID PAGEMEM_CxInitialise (
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32MemoryBase,
    IMG_UINT32          ui32MemorySize,
    IMG_UINT32          ui32PageSize
)
{
    /* Initialise the random number generator...*/
    srand(0x5A5A5A5A);

    /* Calculate things related to the page size...*/
    psContext->ui32PageSize        = ui32PageSize;
    psContext->ui32PageNumberMask  = ~(ui32PageSize-1);
    psContext->ui32PageNumberShift = 1;
    while (
        ((ui32PageSize >> psContext->ui32PageNumberShift) != 1) &&
        (psContext->ui32PageNumberShift < 32)
        )
    {
        psContext->ui32PageNumberShift++;
    }
    IMG_ASSERT(psContext->ui32PageNumberShift < 32);

	psContext->ui32DeviceMemoryBase	= ui32MemoryBase;
	psContext->ui32SizeOfMemory		= ui32MemorySize;	

	/* Ensure that the memory base is aligned to the page boundary */
	IMG_ASSERT((psContext->ui32DeviceMemoryBase & ~psContext->ui32PageNumberMask) == 0x00000000);

	if ( psContext->ui32SizeOfMemory % psContext->ui32PageSize != 0)
	{	
		printf("ERROR: Wrapper - Memory size 0x%X is not a multiple of page size 0x%X \n", psContext->ui32SizeOfMemory, (IMG_UINT32) psContext->ui32PageSize);
		IMG_ASSERT(0);
	}

	/* Initialise the memory page array */
	psContext->ui32MaxNumberOfPages = (psContext->ui32SizeOfMemory / psContext->ui32PageSize);

	/* Allocate the necessary memory and set to 0 */
	psContext->pui32PageTable = IMG_MALLOC( psContext->ui32MaxNumberOfPages * sizeof(IMG_UINT32));
	memset((void*) psContext->pui32PageTable, 0, psContext->ui32MaxNumberOfPages * sizeof(IMG_UINT32) );

	psContext->ui32NextFreeSlot = 0;
	psContext->bMemoryFull = IMG_FALSE;
}

/*!
******************************************************************************

 @Function              PAGEMEM_Deinitialise

******************************************************************************/
IMG_VOID PAGEMEM_Deinitialise(void)
{
    if (bInitialised)
    {
        PAGEMEM_CxDeinitialise(&gsContext);
        bInitialised = IMG_FALSE;
    }
}

/*!
******************************************************************************

 @Function              PAGEMEM_CxDeinitialise

******************************************************************************/
IMG_VOID PAGEMEM_CxDeinitialise (
    PAGEMEM_sContext *  psContext
)
{
    IMG_FREE(psContext->pui32PageTable);
}

/*!
******************************************************************************

 @Function              PAGEMEM_AllocateMemoryPage

******************************************************************************/
IMG_UINT32 PAGEMEM_AllocateMemoryPage (
    IMG_UINT32      ui32Size,
    IMG_BOOL        bRandomAlloc
)
{
    IMG_ASSERT (bInitialised);
    return PAGEMEM_CxAllocateMemoryPage(&gsContext, ui32Size, bRandomAlloc);
}
/*!
******************************************************************************

 @Function              PAGEMEM_AllocateMemoryPage

******************************************************************************/
IMG_UINT32 PAGEMEM_CxAllocateMemoryPage (
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32Size,
    IMG_BOOL            bRandomAlloc
)
{
	IMG_UINT32	ui32PageNumber		 = psContext->ui32NextFreeSlot;
	IMG_UINT32	ui32DeviceAddress;

	if (psContext->bMemoryFull)
	{
		printf("ERROR: Available memory is full\n");
		printf("Check Target Config File memory map is correct for this test\n");
		IMG_ASSERT(IMG_FALSE);
	}
    if (ui32Size != psContext->ui32PageSize)
	{
		printf("ERROR: Allocation %d not equal to page size %d\n", ui32Size, psContext->ui32PageSize);
		printf("When using paged allocations the allocation in the test MUST be the size of the page\n");
		printf("Check Target Config File DEVICE flags are correct for this test - random sized allocation may be required\n");
		IMG_ASSERT(IMG_FALSE);
	}

    /* If random allocation...*/
    if (bRandomAlloc)
    {
        /* Get random number...*/
        IMG_UINT64      ui64RandomNo = (IMG_UINT64)rand();

        /* Calculate potential page no based on randon number...*/
        ui32PageNumber = (IMG_UINT32)((ui64RandomNo * (IMG_UINT64)psContext->ui32MaxNumberOfPages) / (IMG_UINT64)RAND_MAX);

        /* If the page is allocate, search for the next free page...*/
	    while (
                (*(psContext->pui32PageTable + ui32PageNumber) != 0) &&
                (ui32PageNumber < psContext->ui32MaxNumberOfPages)
                )
	    {	
		    ui32PageNumber++;
	    }

        /* If we reached the end...*/
        if (ui32PageNumber >= psContext->ui32MaxNumberOfPages)
        {
            /* Seeach for a free page from the start...*/
            ui32PageNumber = 0;
	        while (
                    (*(psContext->pui32PageTable + ui32PageNumber) != 0) &&
                    (ui32PageNumber < psContext->ui32MaxNumberOfPages)
                    )
	        {	
		        ui32PageNumber++;
	        }

            /* If we still haven't found one...*/
            if (ui32PageNumber >= psContext->ui32MaxNumberOfPages)
            {
 		        psContext->bMemoryFull = IMG_TRUE;
            }
        }

        /* Check thee memory is not full...*/
	    IMG_ASSERT(!psContext->bMemoryFull);
    }

	/* Annotate the appropriate entry in the page table as being allocated. For now just write 1 to the location */
	*(psContext->pui32PageTable + ui32PageNumber) = 0x00000001;
	
    /* If not random allocation...*/
    if (!bRandomAlloc)
    {
	    /* Work out where to move the next free slot */
	    while (psContext->ui32NextFreeSlot < psContext->ui32MaxNumberOfPages)
	    {	
		    if ( *(psContext->pui32PageTable + psContext->ui32NextFreeSlot) == 0)
		    {
			    break;
		    }
		    psContext->ui32NextFreeSlot ++;
	    }
	    if (psContext->ui32NextFreeSlot >= psContext->ui32MaxNumberOfPages)
	    {
		    psContext->bMemoryFull = IMG_TRUE;
	    }
    }
	
	/* Work out the device address */
	ui32DeviceAddress = psContext->ui32DeviceMemoryBase + (ui32PageNumber << psContext->ui32PageNumberShift);	

	/*	Address wrap-around check. This would occur if the memory base was set up such that the 
		virtual memory space does not fit into the 32bit address range
	*/
	IMG_ASSERT(ui32DeviceAddress >= psContext->ui32DeviceMemoryBase);

	/* Return the device address */
	return (ui32DeviceAddress);
}


/*!
******************************************************************************

 @Function              PAGEMEM_FreeMemoryPage

******************************************************************************/
IMG_VOID PAGEMEM_FreeMemoryPage (
    IMG_UINT32 ui32DeviceAddress
)
{
    IMG_ASSERT (bInitialised);
    PAGEMEM_CxFreeMemoryPage(&gsContext, ui32DeviceAddress);
}
/*!
******************************************************************************

 @Function              PAGEMEM_CxFreeMemoryPage

******************************************************************************/
IMG_VOID    PAGEMEM_CxFreeMemoryPage(
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32DeviceAddress
)
{
	IMG_UINT32	ui32PageNumber = 0;

	IMG_ASSERT( ui32DeviceAddress >= psContext->ui32DeviceMemoryBase);

	/* Work out the page index from the device address */
	ui32PageNumber = (ui32DeviceAddress - psContext->ui32DeviceMemoryBase)>>psContext->ui32PageNumberShift;

	IMG_ASSERT(ui32PageNumber < psContext->ui32MaxNumberOfPages);

	/* Set the memory at page index to 0 */
	*(psContext->pui32PageTable + ui32PageNumber) = 0;
	
	/* Set the next free slot to the current page if this will move it lower in the array */
	if (ui32PageNumber < psContext->ui32NextFreeSlot)
	{
		psContext->ui32NextFreeSlot = ui32PageNumber;
	}

	/* Mark the memory as not being full */
	psContext->bMemoryFull = IMG_FALSE;
}
