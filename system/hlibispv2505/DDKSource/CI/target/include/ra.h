/*! 
******************************************************************************
 @file   : ra.h

 @brief  

 @Author Marcus Shawcroft

 @date   114 May 2003
 
         <b>Copyright 2004 by Imagination Technologies Limited.</b>\n
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
         Resource Allocator API.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/
/* -*- c-file-style: "img" -*-
<module>
* Name         : ra.h
* Title        : Resource Allocator API
* Author       : Marcus Shawcroft
* Created      : 14 May 2003
*
* Copyright    : 2003 by Imagination Technologies Limited.
*                All rights reserved.  No part of this software, either
*                material or conceptual may be copied or distributed,
*                transmitted, transcribed, stored in a retrieval system
*                or translated into any human or computer language in any
*                form by any means, electronic, mechanical, manual or
*                other-wise, or disclosed to third parties without the
*                express written permission of Imagination Technologies
*                Limited, Unit 8, HomePark Industrial Estate,
*                King's Langley, Hertfordshire, WD4 8LZ, U.K.
*
* Description :
*
* Implements generic resource allocation.
* 
* Platform     : ALL
*
</module>
********************************************************************************/

#ifndef _ra_h_
#define _ra_h_

/* C99 integer types, specifically IMG_UINT64 */
#include <img_types.h>

/* system types, specifically size_t */
#ifndef WINCE
#if !defined(IMG_KERNEL_MODULE)
#include "sys/types.h"
#endif
#endif
#include <stddef.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Initialise

	PURPOSE:    To initialise the ra module. Must be called before any
 	            other ra API function.
	
	PARAMETERS:	None
	
	RETURNS:	0 failure
	            1 success
</function>
------------------------------------------------------------------------------*/
int
RA_Initialise (void);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Finalise

	PURPOSE:    To finalise the ra module.
	
	PARAMETERS:	None
	
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
RA_Finalise (void);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Create

	PURPOSE:    To create a resource arena.
	
	PARAMETERS:	In: name - the name of the arena for diagnostic purposes.
	            In: base - the base of an initial resource span or 0.
	            In: size - the size of an initial resource span or 0.
	            In: quantum - the arena allocation quantum.
	            In: alloc - a resource allocation callback or 0.
	            In: free - a resource de-allocation callback or 0.
	            In: import_handle - handle passed to alloc and free or 0.
	RETURNS:	arena handle, or NULL.
</function>
------------------------------------------------------------------------------*/
struct ra_arena *
RA_Create (char *name,
		   IMG_UINT64 base,
		   IMG_UINT64 size,		
		   size_t quantum,	
		   int (*alloc)(void *_h, IMG_UINT64 size, IMG_UINT64 *actual_size,
						void **ref, unsigned flags, IMG_UINT64 *base),
		   void (*free) (void *, IMG_UINT64, void *ref),
		   void *import_handle);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Delete

	PURPOSE:    To delete a resource arena. All resources allocated from
 	            the arena must be freed before deleting the arena.
	                	
	PARAMETERS:	In: arena - the arena to delete.
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
RA_Delete (struct ra_arena *arena);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Add

	PURPOSE:    To add a resource span to an arena. The span must not
 	            overlapp with any span previously added to the arena.
	
	PARAMETERS:	In: arena - the arena to add a span into.
	            In: base - the base of the span.
	            In: size - the extent of the span.
	RETURNS:	1 - success
	            0 - failure
</function>
------------------------------------------------------------------------------*/
int
RA_Add (struct ra_arena *arena, IMG_UINT64 base, size_t size);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Alloc

	PURPOSE:    To allocate resource from an arena.
	
	PARAMETERS:	In:  arena - the arena
	            In:  request_size - the size of resource segment requested.
	            Out: actual_size - the actual_size of resource segment
	                 allocated, typcially rounded up by quantum.
	            Out: ref - the user reference associated with allocated resource span.
	            In:  flags - flags influencing allocation policy.
	            In: alignment - the alignment constraint required for the
	                allocated segment, use 0 if alignment not required.
	            Out: base - allocated base resource
	RETURNS:    1 - success
	            0 - failure
</function>
------------------------------------------------------------------------------*/
int
RA_Alloc (struct ra_arena *arena, 
		  IMG_UINT64 size,
		  IMG_UINT64 *actual_size,
		  void **ref, 
		  unsigned flags,
		  IMG_UINT64 alignment,
		  IMG_UINT64 *base);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Free

	PURPOSE:    To free a resource segment.
	
	PARAMETERS:	In:  arena - the arena the segment was originally allocated from.
	            In:  base - the base of the resource span to free.
	            In:  ref - could be removed.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void 
RA_Free (struct ra_arena *arena, IMG_UINT64 base, void *ref);

#if defined (__cplusplus)
}
#endif

#endif
