/*!
******************************************************************************
 @file   : pagemem.h

 @brief

 @Author Ray Livesley

 @date   28/11/2006

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
         Page memory allocation functions for the wrapper for PDUMP tools..

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#if !defined (__PAGEMEM_H__)
#define __PAGEMEM_H__

#include <img_types.h>

#if (__cplusplus)
extern "C" {
#endif


/*!
******************************************************************************
  This structure contains the context for allocation.
******************************************************************************/
typedef struct
{
    IMG_UINT32	    ui32MaxNumberOfPages;   /*!< Total number of pages available	    */
    IMG_UINT32	    ui32DeviceMemoryBase;   /*!< Virtual address memory base, in bytes	*/
    IMG_UINT32      ui32SizeOfMemory;       /*!< Size of memory, in bytes           	*/
    IMG_UINT32 *    pui32PageTable;         /*!< Dynamically allocated Page Table	    */
    IMG_UINT32	    ui32NextFreeSlot;       /*!< Next free slot in memory	            */
    IMG_BOOL	    bMemoryFull;            /*!< IMG_TRUE when the memory is full       */
    IMG_UINT32      ui32PageSize;           /*!< The page size                          */
    IMG_UINT32      ui32PageNumberMask;     /*!< The page number mask                   */
    IMG_UINT32      ui32PageNumberShift;    /*!< The page number shift                  */

} PAGEMEM_sContext;

/*!
******************************************************************************

 @Function              PAGEMEM_Initialise

 @Description

 This function is used to initialise the page memory system.

 @Input     ui32MemoryBase : The base address of device memory
                             allocations.

 @Input     ui32MemorySize : The max. size of the device memory.

 @Input     ui32PageSize   : Page size.

 @Return    None.

******************************************************************************/
extern IMG_VOID PAGEMEM_Initialise (
    IMG_UINT32          ui32MemoryBase,
    IMG_UINT32          ui32MemorySize,
    IMG_UINT32          ui32PageSize
);

/*!
******************************************************************************

 @Function              PAGEMEM_Deinitialise

 @Description

 This function is used to de-initialise the page memory system.

 @Input     None.

 @Return    None.

******************************************************************************/
extern IMG_VOID PAGEMEM_Deinitialise ();


/*!
******************************************************************************

 @Function              PAGEMEM_AllocateMemoryPage

 @Description

 Allocate a page in the device address space and return the address as seen
 by the device.

 @Input     ui32Size        : The size of the allocation.

 @Input     bRandomAlloc    : IMG_TRUE for randon allocation.

 @Return	The device address of the allocated page.

******************************************************************************/
extern IMG_UINT32 PAGEMEM_AllocateMemoryPage (
    IMG_UINT32          ui32Size,
    IMG_BOOL            bRandomAlloc
);

/*!
******************************************************************************

 @Function              PAGEMEM_FreeMemoryPage

 @Description

 Free a page in the device address space at the device address specified.

 @Input     ui32DeviceAddress : The device address of the page to be freed.

 @Return	None.

******************************************************************************/
extern IMG_VOID    PAGEMEM_FreeMemoryPage(
    IMG_UINT32      ui32DeviceAddress
);

/*!
******************************************************************************

 @Function              PAGEMEM_CxInitialise

 @Description

 This function is used to initialise the page memory system with
 an external context structure.

 @Input     psContext       : Pointer to context structure.

 @Input     ui32MemoryBase  : The base address of device memory
                              allocations.

 @Input     ui32MemorySize  : The max. size of the device memory.

 @Input     ui32PageSize    : Page size

 @Return    None

******************************************************************************/
extern IMG_VOID PAGEMEM_CxInitialise (
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32MemoryBase,
    IMG_UINT32          ui32MemorySize,
    IMG_UINT32          ui32PageSize
);

/*!
******************************************************************************

 @Function              PAGEMEM_CxDeinitialise

 @Description

 This function is used to de-initialise the page memory system with
 an external context structure.

 @Input     psContext       : Pointer to context structure.

 @Return    None

******************************************************************************/
extern IMG_VOID PAGEMEM_CxDeinitialise (
    PAGEMEM_sContext *   psContext
);


/*!
******************************************************************************

 @Function              PAGEMEM_CxAllocateMemoryPage

 @Description

 Allocate a page in the device address space and return the address as seen
 by the device with an external context structure.

 @Input     psContext       : Pointer to context structure.

 @Input     ui32Size        : The size of the allocation.

 @Input     bRandomAlloc    : IMG_TRUE for randon allocation.

 @Return	The device address of the allocated page.

******************************************************************************/
extern IMG_UINT32 PAGEMEM_CxAllocateMemoryPage (
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32Size,
    IMG_BOOL            bRandomAlloc
);

/*!
******************************************************************************

 @Function              PAGEMEM_CxFreeMemoryPage

 @Description

 Free a page in the device address space at the device address specified with
 an external context structure.

 @Input     psContext       : Pointer to context structure.

 @Input     ui32DeviceAddress : The device address of the page to be freed.

 @Return	None.

******************************************************************************/
extern IMG_VOID    PAGEMEM_CxFreeMemoryPage(
    PAGEMEM_sContext *  psContext,
    IMG_UINT32          ui32DeviceAddress
);

#if (__cplusplus)
}
#endif

#endif /* __PAGEMEM_H__	*/


