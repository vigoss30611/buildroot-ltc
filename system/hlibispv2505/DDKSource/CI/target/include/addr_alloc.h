/*!
******************************************************************************
 @file   : addr_alloc.h

 @brief	 Address allocation management API.

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

#if !defined (__ADDR_ALLOC_H__)
#define __ADDR_ALLOC_H__

#include <img_types.h>
#include <ra.h>

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct ADDR_tag_sRegion		 ADDR_sRegion;		/*!< Memory region structure */

/*!
******************************************************************************
  This structure contains information about the memory region.
******************************************************************************/
struct ADDR_tag_sRegion
{
	IMG_CHAR *			pszName;			/*!< A pointer to a sring containing the name of the region.
											     IMG_NULL for the default memory region.				*/
	IMG_UINT64			ui64BaseAddr;		/*!< The base address of the memory region.					*/
	IMG_UINT64			ui64Size;			/*!< The size of the memory region.							*/
	IMG_UINT32			ui32GuardBand;		/*!< The size of any guard band to be used.  Guard bands
											     can be useful in separating block allocations and allows
												 the caller to detect erroneor accesses into these
												 areas.													*/

	ADDR_sRegion *		psNextRegion;		/*!< Used internally by the ADDR API...
												 A pointer used to point to the next memory region		*/
	struct ra_arena *	psArena;			/*!< Used internally by the ADDR API...
												 A to a structure used to maintain and perform
												 address allocation.									*/
};


/*!
******************************************************************************
  This structure contains the context for allocation.
******************************************************************************/
typedef struct
{
    ADDR_sRegion *	psRegions;				    /*!< Pointer the first region in the list.	*/
    ADDR_sRegion *	psDefaultRegion;			/*!< Pointer the default region.			*/
	IMG_UINT32		ui32NoRegions;				/*!< Number of regions currently available (including default) */
	IMG_BOOL		bUseRandomBlocks;			/*!< If set, memory will be allocated in any
													random block, not necessarily the one defined */
	IMG_BOOL		bUseRandomAllocation;		/*!< If set, memory will be allocated in any
													position of the block*/
} ADDR_sContext;

/*!
******************************************************************************

 @Function				ADDR_Initialise

 @Description

 This function is used to initialise the address alocation sub-system.

 NOTE: This function may be called multiple times.  The initialisation only
 happens the first time it is called.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_Initialise(IMG_VOID);

/*!
******************************************************************************

 @Function				ADDR_Deinitialise

 @Description

 This function is used to de-initialise the address alocation sub-system.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_Deinitialise(IMG_VOID);


/*!
******************************************************************************

 @Function				ADDR_DefineMemoryRegion

 @Description

 This function is used define a memory region.

 NOTE: The region structure MUST be defined in static memory as this
 is retained and used by the ADDR sub-system.

 NOTE: Invalid parameters are trapped by asserts.

 @Input	psRegion	: A pointer to a region structure.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_DefineMemoryRegion(
	ADDR_sRegion *		psRegion
);


/*!
******************************************************************************

 @Function				ADDR_Malloc

 @Description

 This function is used allocate space within a memory region.

 NOTE: Allocation failures or invalid parameters are trapped by asserts.

 @Input	pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	ui64Size	: The size (in bytes) of the allocation.

 @Return	IMG_UINT64 : The address of the allocated space.

******************************************************************************/
extern IMG_UINT64 ADDR_Malloc(
	IMG_CHAR *			pszName,
	IMG_UINT64			ui64Size
);


/*!
******************************************************************************

 @Function				ADDR_Malloc1

 @Description

 This function is used allocate space within a memory region.

 NOTE: Allocation failures or invalid parameters are trapped by asserts.

 @Input	    pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	    ui64Size	: The size (in bytes) of the allocation.

 @Input		ui64Alignment   : The required byte alignment (1, 2, 4, 8, 16 etc).

 @Return	IMG_UINT64 : The address of the allocated space.

******************************************************************************/
extern IMG_UINT64 ADDR_Malloc1(
	IMG_CHAR *			    pszName,
	IMG_UINT32			    ui64Size,
	IMG_UINT32              ui64Alignment
);


/*!
******************************************************************************

 @Function				ADDR_Free

 @Description

 This function is used free a previously allocate space within a memory region.

 NOTE: Invalid parameters are trapped by asserts.

 @Input	pszName		: Is a pointer the name of the memory region.
 						  IMG_NULL is used to free space from the
						  default memory region.

 @Input	ui64Addr	: The address allocated.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_Free(
	IMG_CHAR *			pszName,
	IMG_UINT64			ui64Addr
);


/*!
******************************************************************************

 @Function				ADDR_CxInitialise

 @Description

 This function is used to initialise the address alocation sub-system with
 an external context structure.

 NOTE: This function should be call only once for the context.

 @Input	    psContext   : Pointer to context structure.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_CxInitialise(
    ADDR_sContext *        psContext
);

/*!
******************************************************************************

 @Function				ADDR_CxDeinitialise

 @Description

 This function is used to de-initialise the address alocation sub-system with
 an external context structure.


 @Input	    psContext   : Pointer to context structure.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_CxDeinitialise(
    ADDR_sContext *        psContext
);

/*!
******************************************************************************

 @Function				ADDR_CxDefineMemoryRegion

 @Description

 This function is used define a memory region with an external context structure.

 NOTE: The region structure MUST be defined in static memory as this
 is retained and used by the ADDR sub-system.

 NOTE: Invalid parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    psRegion	: A pointer to a region structure.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_CxDefineMemoryRegion(
    ADDR_sContext *         psContext,
	ADDR_sRegion *		    psRegion
);


/*!
******************************************************************************

 @Function				ADDR_CxMalloc

 @Description

 This function is used allocate space within a memory region with
 an external context structure.

 NOTE: Allocation failures or invalid parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	    ui64Size	: The size (in bytes) of the allocation.

 @Return	IMG_UINT64 : The address of the allocated space.

******************************************************************************/
extern IMG_UINT64 ADDR_CxMalloc(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size
);


/*!
******************************************************************************

 @Function				ADDR_CxMalloc1

 @Description

 This function is used allocate space within a memory region with
 an external context structure.

 NOTE: Allocation failures or invalid parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	    ui64Size	: The size (in bytes) of the allocation.

 @Input		ui64Alignment   : The required byte alignment (1, 2, 4, 8, 16 etc).

 @Return	IMG_UINT64 : The address of the allocated space.

******************************************************************************/
extern IMG_UINT64 ADDR_CxMalloc1(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64              ui64Alignment
);


/*!
******************************************************************************

 @Function				ADDR_CxMallocRes

 @Description

 This function is used allocate space within a memory region with
 an external context structure. 

 NOTE: Allocation failures are returned in IMG_RESULT, however invalid 
 parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	    ui64Size	: The size (in bytes) of the allocation.

 @Input		pui64Base   : Pointer to the address of the allocated space.

 @Return	IMG_RESULT  : IMG_SUCCESS or an error code.

******************************************************************************/
extern IMG_RESULT ADDR_CxMallocRes(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64 *			pui64Base
);


/*!
******************************************************************************

 @Function				ADDR_CxMalloc1Res

 @Description

 This function is used allocate space within a memory region with
 an external context structure.

 NOTE: Allocation failures are returned in IMG_RESULT, however invalid 
 parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    pszName		: Is a pointer the name of the memory region.
						  IMG_NULL can be used to allocate space from the
						  default memory region.

 @Input	    ui64Size	: The size (in bytes) of the allocation.

 @Input		ui64Alignment   : The required byte alignment (1, 2, 4, 8, 16 etc).

 @Input		pui64Base   : Pointer to the address of the allocated space.

 @Return	IMG_RESULT  : IMG_SUCCESS or an error code.

******************************************************************************/
extern IMG_RESULT ADDR_CxMalloc1Res(
    ADDR_sContext *         psContext,
	IMG_CHAR *			    pszName,
	IMG_UINT64			    ui64Size,
	IMG_UINT64              ui64Alignment,
	IMG_UINT64 *			pui64Base
);


/*!
******************************************************************************

 @Function				ADDR_CxFree

 @Description

 This function is used free a previously allocate space within a memory region
 with an external context structure.

 NOTE: Invalid parameters are trapped by asserts.

 @Input	    psContext   : Pointer to context structure.

 @Input	    pszName		: Is a pointer the name of the memory region.
 						  IMG_NULL is used to free space from the
						  default memory region.

 @Input	    ui64Addr	: The address allocated.

 @Return	None.

******************************************************************************/
extern IMG_VOID ADDR_CxFree(
    ADDR_sContext *     psContext,
	IMG_CHAR *			pszName,
	IMG_UINT64			ui64Addr
);


#if defined (__cplusplus)
}
#endif

#endif /* __ADDR_ALLOC_H__	*/


