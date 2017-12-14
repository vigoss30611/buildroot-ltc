/* -*- c-file-style: "img" -*-
<module>
* Name         : pool.c
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
* Platform     : all bareboards
*
</module>
********************************************************************************/

#include <img_defs.h>
#include <img_types.h>

#include "pool.h"

struct pool
{
  char *name;
  size_t size;
  unsigned grow;
  struct buffer *buffers;
  struct object *objects;
};

struct buffer
{
  struct buffer *next;
};

struct object
{
  struct object *next;
};

static void
allocate_buffer (struct pool *pool)
{
  struct buffer *buf;
  unsigned int i;
  const int align_size = sizeof(long long) - 1; /*64 bit*/

  buf = IMG_MALLOC((pool->size + align_size) * pool->grow + sizeof (struct buffer));
  buf->next = pool->buffers;
  pool->buffers = buf;
  for (i=0; i<pool->grow; i++)
  {
      struct object *obj;
      unsigned char* temp_ptr;
      obj = (struct object *)(((unsigned char*)(buf+1)) + i * (pool->size + align_size));
      /*align to 64bit*/
      temp_ptr = (unsigned char*) obj;
      if ((IMG_UINTPTR)temp_ptr & align_size)
      {
    	  temp_ptr+= ((align_size + 1) - ((IMG_UINTPTR)temp_ptr & align_size));
    	  obj = (struct object*)temp_ptr;
      }

      obj->next = pool->objects;
      pool->objects = obj;
  }
}

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
pool_create (char *name, unsigned int size)
{
  struct pool *pool;
  pool = IMG_MALLOC (sizeof (*pool));
  if (pool==NULL)
    return NULL;
  pool->name = name;
  pool->size = size;
  pool->buffers = NULL;
  pool->objects = NULL;
  pool->grow = 32 /* grow */;
  return pool;
}

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
pool_delete (struct pool *pool)
{
  struct buffer *   pBuffer;

  IMG_ASSERT (pool!=NULL);

  pBuffer = pool->buffers;
  while (pBuffer != 0)
  {
      pBuffer = pBuffer->next;
      IMG_FREE(pool->buffers);
      pool->buffers = pBuffer;

  }
  IMG_FREE(pool);
}

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
pool_alloc (struct pool *pool)
{
  struct object *obj;
  IMG_ASSERT (pool!=NULL);
  if (pool->objects==NULL)
    allocate_buffer (pool);
  if (pool->objects==NULL)
    return NULL;
  obj = pool->objects;
  pool->objects = obj->next;
  return obj;
}

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
pool_free (struct pool *pool, void *o)
{
  struct object *obj;

  IMG_ASSERT (pool!=NULL);
  IMG_ASSERT (o!=NULL);
  obj = o;
  obj->next = pool->objects;
  pool->objects = obj;
}

