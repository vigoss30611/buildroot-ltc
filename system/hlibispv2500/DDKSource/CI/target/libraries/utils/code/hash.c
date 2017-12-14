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
* Implements simple self scaling hash tables. Hash collisions are
* handled by chaining entries together. Hash tables are increased in
* size when they become more than (50%?) full and decreased in size
* when less than (25%?) full. Hash tables are never decreased below
* their initial size.
* 
* Platform     : ALL
*
</module>
********************************************************************************/

#include "hash.h"
#include "pool.h"
#include "trace.h"
#include "img_types.h"
#include "img_defs.h"

#define private_max(a,b) ((a)>(b)?(a):(b))

/* pool of struct hash objects */
static struct pool *hash_pool = NULL;

/* pool of struct bucket objects */
static struct pool *bucket_pool = NULL;

/* Each entry in a hash table is placed into a bucket */
struct bucket
{
	/* the next bucket on the same chain */
	struct bucket *next;

	/* entry key */
	IMG_UINT64 k;

	/* entry value */
	IMG_UINTPTR v;
};

struct hash 
{
	/* the hash table array */
	struct bucket **table;
	
	/* current size of the hash table */
	unsigned int size;	

	/* number of entries currently in the hash table */
	unsigned int count;

	/* the minimum size that the hash table should be re-sized to */
	unsigned int minimum_size;
};

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Func

	PURPOSE:    Hash function intended for hashing addresses.
	                	
	PARAMETERS:	In:  p - the key to hash.
	            In:  size - the size of the hash table.
	RETURNS:	the hash value.
</function>
------------------------------------------------------------------------------*/
static unsigned
HASH_Func (IMG_UINT64 p, unsigned size)
{ 
	unsigned hash = (unsigned) p;
	hash += (hash << 12);
	hash ^= (hash >> 22);
	hash += (hash << 4);
	hash ^= (hash >> 9);
	hash += (hash << 10);
	hash ^= (hash >> 2);
	hash += (hash << 7);
	hash ^= (hash >> 12);
	hash &= (size-1);
	return hash;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   chain_insert

	PURPOSE:    Insert a bucket into the appropriate hash table chain.
	                	
	PARAMETERS:	In:  b - the bucket
	            In:  table - the hash table
	            In:  size - the size of the hash table
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
chain_insert (struct bucket *b, struct bucket **table, unsigned int size)
{
	unsigned int index;

	IMG_ASSERT (b!=NULL);
	IMG_ASSERT (table!=NULL);
	IMG_ASSERT (size!=0);

	index = HASH_Func (b->k, size);
	b->next = table[index];
	table[index] = b;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   rehash

	PURPOSE:    Iterate over every entry in an old hash table and rehash into
	            the new table.
	                	
	PARAMETERS:	In:  old_table - the old hash table
	            In:  old_size - the size of the old hash table
	            In:  new_table - the new hash table
	            In:  new_size - the size of the new hash table
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
rehash (struct bucket **old_table, unsigned int old_size,
		struct bucket **new_table, unsigned int new_size)
{
	unsigned int i;
	for (i=0; i< old_size; i++)
    {
		struct bucket *b;
		b = old_table[i];
		while (b!=NULL)
		{
			struct bucket *n = b->next;
			chain_insert (b, new_table, new_size);
			b = n;
		}
    }
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   resize

	PURPOSE:    Attempt to resize a hash table, failure to allocate a new larger
	            hash table is not considered a hard failure. We simply continue
	            and allow the table to fill up, the effect is to allow hash
	            chains to become longer.
	                	
	PARAMETERS:	None
	RETURNS:	1 Success
	            0 Failed
</function>
------------------------------------------------------------------------------*/
static void
resize (struct hash *hash, unsigned int new_size)
{
	if (new_size != hash->size)
    {
		struct bucket **new_table;

		trace (("hash resize: oldsize=%d  newsize=%d  count=%d\n",
				hash->size, new_size, hash->count));

		new_table = IMG_MALLOC(sizeof (struct bucket *) * new_size);
		if (new_table!=NULL)
		{
			unsigned int i;
			for (i=0; i<new_size; i++)
				new_table[i] = NULL;
			rehash (hash->table, hash->size, new_table, new_size);
			IMG_FREE(hash->table);
			hash->table = new_table;
			hash->size = new_size;
		}
    }
}

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
HASH_Initialise (void)
{
	trace (("HASH_Initialise ()\n"));
	hash_pool = pool_create ("img-hash", sizeof (struct hash));
	if (hash_pool==NULL) goto hash_fail;
	bucket_pool = pool_create ("img-bucket", sizeof (struct bucket));
	if (hash_pool==NULL) goto bucket_fail;
	return 1;

  bucket_fail:
	pool_delete (hash_pool);
  hash_fail:
	return 0;
}

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
HASH_Finalise (void)
{
	trace (("HASH_Finalise ()\n"));
	pool_delete (hash_pool);
	hash_pool = NULL;

	pool_delete (bucket_pool);
	bucket_pool = NULL;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Create

	PURPOSE:    Create a self scaling hash table.
	                	
	PARAMETERS:	In:  initial_size - initial and minimum size of the hash table.
	RETURNS:	NULL or hash table handle.
</function>
------------------------------------------------------------------------------*/
struct hash *
HASH_Create (unsigned int initial_size)
{
	struct hash *hash;
	unsigned int i;

	trace (("HASH_Create (%d)\n", initial_size));

	hash = pool_alloc (hash_pool);
	if (hash==NULL)
		return NULL;
	hash->count = 0;
	hash->size = initial_size;
	hash->minimum_size = initial_size;
	hash->table = IMG_MALLOC(sizeof (struct bucket *) * hash->size);
	if (hash->table == NULL)
    {
		pool_free (hash_pool, hash);
		return NULL;
    }

	for (i=0; i<hash->size; i++)
		hash->table[i] = NULL;
	return hash;
}

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
HASH_Delete (struct hash *hash)
{
	if (hash!=NULL)
    {
		IMG_ASSERT (hash->count==0);
		IMG_FREE (hash->table);
		pool_free (hash_pool, hash);
    }
}

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
HASH_Insert (struct hash *hash, IMG_UINT64 k, IMG_UINTPTR v)
{
	struct bucket *b;
	b = pool_alloc (bucket_pool);
	if (b==NULL)
		return 0;
	b->k = k;
	b->v = v;
	chain_insert (b, hash->table, hash->size);
	hash->count++;

	/* check if we need to think about re-balencing */
	if (hash->count << 1 > hash->size)
		resize (hash, hash->size << 1);
	return 1;
}

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
HASH_Remove (struct hash *hash, IMG_UINT64 k)
{
	struct bucket **bp;
	unsigned int index;
	index = HASH_Func (k, hash->size);
  
	for (bp = &(hash->table[index]); *bp!=NULL; bp = &((*bp)->next))
		if ((*bp)->k == k)
		{
			struct bucket *b = *bp;
			IMG_UINTPTR v = b->v;
			(*bp) = b->next;
			pool_free (bucket_pool, b);
			hash->count--;

			/* check if we need to think about re-balencing */
			if (hash->size > (hash->count << 2) && hash->size > hash->minimum_size)
				resize (hash, private_max (hash->size >> 1, hash->minimum_size));

			return v;
		}
	return 0;
}

#ifdef HASH_TRACE
/*----------------------------------------------------------------------------
<function>
	FUNCTION:   HASH_Dump

	PURPOSE:    To dump the contents of a hash table in human readable form.
	                	
	PARAMETERS:	In:  hash - the hash table
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
HASH_Dump (struct hash *hash)
{
	int i;
	int max_length=0;
	int empty_count=0;

	IMG_ASSERT (hash!=NULL);
	for (i=0; i<hash->size; i++)
    {
		struct bucket *b;
		int length = 0;
		if (hash->table[i]==NULL)
			empty_count++;
		for (b=hash->table[i]; b!=NULL; b=b->next)
			length++;
		max_length = private_max (max_length, length);
    }

	printf ("hash table: minimum_size=%d  size=%d  count=%d\n",
			hash->minimum_size, hash->size, hash->count);
	printf ("  empty=%d  max=%d\n", empty_count, max_length);
}
#endif
