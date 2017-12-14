/*! 
******************************************************************************
 @file   : hash.h

 @brief  

 @Author Marcus Shawcroft

 @date   14 May 2003
 
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
         Self scaling hash tables.

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
* Name         : hash.c
* Title        : Self scaling hash tables.
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
* Implements simple self scaling hash tables.
* 
* Platform     : ALL
*
</module>
********************************************************************************/

#ifndef _hash_h_
#define _hash_h_

/* C99 integer types, specifically IMG_UINTPTR */
#include <img_types.h>

#if defined (__cplusplus)
extern "C" {
#endif


struct hash;

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Initialise

	PURPOSE:    To initialise the hash module.
	                	
	PARAMETERS:	None
	RETURNS:	1 Success
	            0 Failed
</function>
------------------------------------------------------------------------------*/
int
HASH_Initialise (void);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Finalise

	PURPOSE:    To finalise the hash module. All allocated hash tables should
	            be deleted before calling this function.
	                	
	PARAMETERS:	None
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
HASH_Finalise (void);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Create

	PURPOSE:    Create a self scaling hash table.
	                	
	PARAMETERS:	In:  initial_size - initial and minimum size of the hash table.
	RETURNS:	NULL or hash table handle.
</function>
------------------------------------------------------------------------------*/
struct hash *
HASH_Create (unsigned int initial_size);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Delete

	PURPOSE:    To delete a hash table, all entries in the table should be
	            removed before calling this function.
	                	
	PARAMETERS:	In:  hash - hash table
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
HASH_Delete (struct hash *hash);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Insert

	PURPOSE:    To insert a key value pair into a hash table.
	                	
	PARAMETERS:	In:  hash - the hash table.
	            In:  k - the key value.
	            In:  v - the value associated with the key.
	RETURNS:	1 Success
	            0 Failed
</function>
------------------------------------------------------------------------------*/
int
HASH_Insert (struct hash *hash, IMG_UINT64 k, IMG_UINTPTR v);

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Remove

	PURPOSE:    To remove a key value pair from a hash table.
	                	
	PARAMETERS:	In:  hash - the hash table
	            In:  k - the key
	RETURNS:	0 if the key is missing or the value associated with the key.
</function>
------------------------------------------------------------------------------*/
IMG_UINTPTR
HASH_Remove (struct hash *hash, IMG_UINT64 k);

#if defined (__cplusplus)
}
#endif

#endif
