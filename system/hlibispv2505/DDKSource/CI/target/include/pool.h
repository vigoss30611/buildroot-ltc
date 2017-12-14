/*! 
******************************************************************************
 @file   : pool.h

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
         Object Pool Memory Allocator.

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
* Name         : pool.h
* Title        : Object Pool Memory Allocator
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
*                Implements an object pool memory manager.
* 
* Platform     : ALL
*
</module>
********************************************************************************/

#ifndef _pool_h_
#define _pool_h_

#if defined (__cplusplus)
extern "C" {
#endif

struct pool;

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   pool_create

	PURPOSE:
	            Create an object pool.
	                	
	PARAMETERS:	In:  name - name of object pool for diagnostic purposes.
	            In:  size - size of each object in the pool in bytes.
	RETURNS:	NULL or object pool handle.
</function>
------------------------------------------------------------------------------*/
struct pool *
pool_create (char *name, unsigned int size);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   pool_delete

	PURPOSE:
	            Delete an object pool. All objects allocated from the pool must
	            be free'd with pool_free() before deleting the object pool.
	                	
	PARAMETERS:	In:  pool - object pool pointer.
	RETURNS:	None.
</function>
------------------------------------------------------------------------------*/
void
pool_delete (struct pool *pool);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   pool_alloc

	PURPOSE:
	            Allocate an object from an object pool.
	                	
	PARAMETERS:	In:  pool - object pool.
	RETURNS:	object pointer, or NULL.
</function>
------------------------------------------------------------------------------*/
void *
pool_alloc (struct pool *pool);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   pool_free

	PURPOSE:
	            Free an object previously allocated from an object pool.
	                	
	PARAMETERS:	In:  pool - object pool pointer.
	            In:  obj - object pointer
	RETURNS:	None.
</function>
------------------------------------------------------------------------------*/
void
pool_free (struct pool *pool, void *obj);

#if defined (__cplusplus)
}
#endif

#endif
