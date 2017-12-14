/* -*- c-file-style: "img" -*-
<module>
* Name         : ra.c
* Title        : Resource Allocator
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
* Implements generic resource allocation. The resource
* allocator was originally intended to manage address spaces in
* practice the resource allocator is generic and can manages arbitrary
* sets of integers.
*
* Resources are allocated from arenas. Arena's can be created with an
* initial span of resources. Further resources spans can be added to
* arenas. A call back mechanism allows an arena to request further
* resource spans on demand.
*
* Each arena maintains an ordered list of resource segments each
* described by a boundary tag. Each boundary tag describes a segment
* of resources which are either 'free', available for allocation, or
* 'busy' currently allocated. Adjacent 'free' segments are always
* coallesced to avoid fragmentation.
*
* For allocation, all 'free' segments are kept on lists of 'free'
* segments in a table index by Log2(segment size). ie Each table index
* n holds 'free' segments in the size range 2**(n-1) -> 2**n.
*
* Allocation policy is based on an *almost* best fit
* stratedy. Choosing any segment from the appropriate table entry
* guarantees that we choose a segment which is with a power of 2 of
* the size we are allocating.
*
* Allocated segments are inserted into a self scaling hash table which
* maps the base resource of the span to the relevant boundary
* tag. This allows the code to get back to the bounary tag without
* exporting explicit boundary tag references through the API.
*
* Each arena has an associated quantum size, all allocations from the
* arena are made in multiples of the basic quantum.
*
* On resource exhaustion in an arena, a callback if provided will be
* used to request further resources. Resouces spans allocated by the
* callback mechanism are delimited by special boundary tag markers of
* zero span, 'span' markers. Span markers are never coallesced. Span
* markers are used to detect when an imported span is completely free
* and can be deallocated by the callback mechanism.
*
* Platform     : ALL
*
</module>
********************************************************************************/

/* Issues:
 * - flags, flags are passed into the resource allocator but are not currently used.
 * - determination, of import size, is currently braindead.
 * - debug code should be moved out to own module and #ifdef'd
 */

#include "hash.h"
#include "pool.h"
#include "ra.h"
#include "trace.h"
#include "img_defs.h"

/* The initial, and minimum size of the live address -> boundary tag
   structure hash table. The value 64 is a fairly arbitrary
   choice. The hash table resizes on demand so the value choosen is
   not critical. */
#define MINIMUM_HASH_SIZE (64)

//#define RA_DUMP
#ifdef RA_DUMP
void
RA_Dump (struct ra_arena *arena);
#else
#define RA_Dump(a)
#endif

#ifdef RA_STATS
void
RA_Stats (struct ra_arena *arena);
#endif

/* Defines whether sequential or random allocation is used */
enum
{
	SEQUENTIAL_ALLOCATION,
	RANDOM_ALLOCATION
};

/* boundary tags, used to describe a resource segment */
struct bt
{
	enum bt_type
	{
		btt_span,				/* span markers */
		btt_free,				/* free resource segment */
		btt_live				/* allocated resource segment */
	} type;

	/* The base resource and extent of this segment */
	IMG_UINT64 base;
	IMG_UINT64 size;

	/* doubly linked ordered list of all segments within the arena */
	struct bt *next_segment;
	struct bt *prev_segment;
	/* doubly linked un-ordered list of free segments. */
	struct bt *next_free;
	struct bt *prev_free;
	/* a user reference associated with this span, user references are
	 * currently only provided in the callback mechanism */
	void *ref;
};

/* resource allocation arena */
struct ra_arena
{
	/* arena name for diagnostics output */
	char *name;

	/* allocations within this arena are quantum sized */
	size_t quantum;

	/* index of the last position in the head_free table, with available free space*/
	IMG_UINT32 max_index;

	/* import interface, if provided */
	int (*import_alloc)(void*, IMG_UINT64 size, IMG_UINT64 *actual_size,
						void **ref, unsigned flags, IMG_UINT64 *base);
	void (*import_free) (void*, IMG_UINT64, void *ref);
	void *import_handle;

	/* head of list of free boundary tags for indexed by Log2 of the
	   boundary tag size */
#define FREE_TABLE_LIMIT 64
  
	/* power-of-two table of free lists */
	struct bt *head_free [FREE_TABLE_LIMIT];

	/* resource ordered segment list */
	struct bt *head_segment;
	struct bt *tail_segment;

	/* segment address to boundary tag hash table */
	struct hash *segment_hash;

#ifdef RA_STATS
	/* total number of segments add to the arena */
	unsigned span_count;

	/* number of current live segments within the arena */
	unsigned live_segment_count;

	/* number of current free segments within the arena */
	unsigned free_segment_count;

	/* number of free resource within the arena */
	unsigned free_resource_count;

	/* total number of resources allocated from the arena */
	unsigned total_allocs;

	/* total number of resources returned to the arena */
	unsigned total_frees;

	/* total number of spans allocated by the callback mechanism */
	unsigned import_count;

	/* total number of spans deallocated by the callback mechanism */
	unsigned export_count;
#endif
};

/* pool of struct arena's */
static struct pool *arena_pool = NULL;

/* pool of struct bt's */
static struct pool *bt_pool = NULL;

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RequestAllocFail

	PURPOSE:    Default callback allocator used if no callback is
  	            specified, always fails to allocate further resources to the
	            arena.
	
	PARAMETERS:	In:  _h - callback handle
	            In:  _size - requested allocation size
	            Out: _actual_size - actual allocation size
	            In:  _ref - user reference
	            In:  _flags - allocation flags
	            
	RETURNS:	allocation base, always 0
</function>
------------------------------------------------------------------------------*/
static int
RequestAllocFail (void *_h, IMG_UINT64 _size, IMG_UINT64 *_actual_size, 
				  void **_ref, unsigned _flags, IMG_UINT64 *_base)
{
	(void)_base;
	(void)_flags;
	(void)_ref;
	(void)_actual_size;
	(void)_size;
	(void)_h;

	return 0;
}

static unsigned int
Log2 (IMG_UINT64 n)
{
	unsigned int l = 0;
	n>>=1;
	while (n>0)
    {
		n>>=1;
		l++;
    }
	return l;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   segment_list_insert_after

	PURPOSE:    Insert a boundary tag into an arena segment list after a specified boundary tag.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  here - the insertion point.
	            In:  bt - the boundary tag to insert.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
segment_list_insert_after (struct ra_arena *arena, struct bt *here, 
						   struct bt *bt)
{
	IMG_ASSERT (arena!=NULL);
	IMG_ASSERT (here!=NULL);

	bt->next_segment = here->next_segment;
	bt->prev_segment = here;
	if (here->next_segment==NULL)
		arena->tail_segment = bt;
	else
		here->next_segment->prev_segment = bt; 
	here->next_segment = bt;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   segment_list_insert

	PURPOSE:    Insert a boundary tag into an arena segment list at the
 	            appropriate point.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag to insert.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
segment_list_insert (struct ra_arena *arena, struct bt *bt)
{
	/* insert into the segment chain */
	if (arena->head_segment==NULL)
    {
		arena->head_segment = arena->tail_segment = bt;
		bt->next_segment = bt->prev_segment = NULL;
    }
	else
    {
		struct bt *bt_scan;
		bt_scan = arena->head_segment;
		while (bt_scan->next_segment!=NULL 
			   && bt->base >= bt_scan->next_segment->base)
			bt_scan = bt_scan->next_segment;
		segment_list_insert_after (arena, bt_scan, bt);
    }
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   segment_list_remove

	PURPOSE:    Remove a boundary tag from an arena segment list.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag to remove.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
segment_list_remove (struct ra_arena *arena, struct bt *bt)
{
	if (bt->prev_segment==NULL)
		arena->head_segment = bt->next_segment;
	else
		bt->prev_segment->next_segment = bt->next_segment;
  
	if (bt->next_segment==NULL)
		arena->tail_segment = bt->prev_segment;
	else
		bt->next_segment->prev_segment = bt->prev_segment;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   segment_split

	PURPOSE:    Split a segment into two, maintain the arena segment list. The
	            boundary tag should not be in the free table. Neither the original
	            or the new neighbour bounary tag will be in the free table.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag to split.
	            In:  size - the required segment size of boundary tag after
	                 splitting.
	            
	RETURNS:	New neighbour bounary tag.
	
</function>
------------------------------------------------------------------------------*/
static struct bt *
segment_split (struct ra_arena *arena, struct bt *bt, IMG_UINT64 size)
{
	struct bt *neighbour = pool_alloc (bt_pool);
	if (neighbour==NULL)
		return NULL;
  
	neighbour->prev_segment = bt;
	neighbour->next_segment = bt->next_segment;
	if (bt->next_segment==NULL)
		arena->tail_segment = neighbour;
	else
		bt->next_segment->prev_segment = neighbour;
	bt->next_segment = neighbour;

	neighbour->type = btt_free;
	neighbour->size = bt->size - size;
	neighbour->base = bt->base + size;
	neighbour->ref = bt->ref;
	bt->size = size;
	return neighbour;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   free_list_insert

	PURPOSE:    Insert a boundary tag into an arena free table.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag.
	            
	RETURNS:	None
	
</function>
------------------------------------------------------------------------------*/
static void
free_list_insert (struct ra_arena *arena, struct bt *bt)
{
	unsigned int index;
	index = Log2 (bt->size);
	bt->type = btt_free;
	bt->next_free = arena->head_free[index];
	bt->prev_free = NULL;
	if (arena->head_free[index]!=NULL)
		arena->head_free[index]->prev_free = bt;
	arena->head_free[index] = bt;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   free_list_remove

	PURPOSE:    Remove a boundary tag from an arena free table.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag.
	            
	RETURNS:	None
	
</function>
------------------------------------------------------------------------------*/
static void
free_list_remove (struct ra_arena *arena, struct bt *bt)
{
	unsigned int index;
	index = Log2 (bt->size);
	if (bt->next_free!=NULL)
		bt->next_free->prev_free = bt->prev_free;
	if (bt->prev_free==NULL)
		arena->head_free[index] = bt->next_free;
	else
		bt->prev_free->next_free = bt->next_free;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   build_span_marker

	PURPOSE:    Construct a span marker boundary tag.
	
	PARAMETERS:	In:  base - the base of the bounary tag.
	            
	RETURNS:	span marker boundary tag
	
</function>
------------------------------------------------------------------------------*/
static struct bt *
build_span_marker (IMG_UINT64 base)
{
	struct bt *bt = pool_alloc (bt_pool);
	if (bt != NULL)
    {
		bt->type = btt_span;
		bt->base = base;
    };
	return bt;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   build_bt

	PURPOSE:    Construct a boundary tag for a free segment.
	
	PARAMETERS:	In:  base - the base of the resource segment.
	            In:  size - the extent of the resouce segment.
	            
	RETURNS:	boundary tag
	
</function>
------------------------------------------------------------------------------*/
static struct bt *
build_bt (IMG_UINT64 base, IMG_UINT64 size)
{
	struct bt *bt;
	bt = pool_alloc (bt_pool);
	if (bt!=NULL)
    {
		bt->type = btt_free;
		bt->base = base;
		bt->size = size;
    }
	return bt;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   ra_insert_resource

	PURPOSE:    Add a free resource segment to an arena.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  base - the base of the resource segment.
	            In:  size - the extent of the resource segment.
	RETURNS:	0 failed
	            1 success
	
</function>
------------------------------------------------------------------------------*/
static int
ra_insert_resource (struct ra_arena *arena, IMG_UINT64 base, IMG_UINT64 size)
{
	struct bt *bt;
	IMG_ASSERT (arena!=NULL);
	bt = build_bt (base, size);
	if (bt==NULL)
		return 0;
	segment_list_insert (arena, bt);
	free_list_insert (arena, bt);
	arena->max_index = Log2 (size);
	if ((IMG_UINT64)1<<arena->max_index < size)
	arena->max_index++;
#ifdef RA_STATS
	arena->span_count++;
#endif
	return 1;
}
	  
/*----------------------------------------------------------------------------
<function>
	FUNCTION:   ra_insert_resource_span

	PURPOSE:    Add a free resource span to an arena, complete with span markers.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  base - the base of the resource segment.
	            In:  size - the extent of the resource segment.
	RETURNS:	the boundary tag representing the free resource segment,
	            or NULL on failure.
</function>
------------------------------------------------------------------------------*/
static struct bt *
ra_insert_resource_span (struct ra_arena *arena, IMG_UINT64 base, IMG_UINT64 size)
{
	struct bt *span_start;
	struct bt *span_end;
	struct bt *bt;

	span_start = build_span_marker (base);
	if (span_start==NULL) goto fail_start;

	span_end = build_span_marker (base + size);
	if (span_end==NULL) goto fail_end;

	bt = build_bt (base, size);
	if (bt==NULL) goto fail_bt;
	  
	segment_list_insert (arena, span_start);
	segment_list_insert_after (arena, span_start, bt);
	free_list_insert (arena, bt);
	segment_list_insert_after (arena, bt, span_end);
	return bt;

  fail_bt:
	pool_free (bt_pool, span_end);
  fail_end:
	pool_free (bt_pool, span_start);
  fail_start:
	return NULL;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   free_bt

	PURPOSE:    Free a boundary tag taking care of the segment list and the
	            boundary tag free table. 
	
	PARAMETERS:	In:  arena - the arena.
	            In:  bt - the boundary tag to free.
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
static void
free_bt (struct ra_arena *arena, struct bt *bt)
{
	struct bt *neighbour;

	IMG_ASSERT (arena!=NULL);
	IMG_ASSERT (bt!=NULL);
	
#ifdef RA_STATS
	arena->live_segment_count--;
	arena->free_segment_count++;
	arena->free_resource_count+=bt->size;
#endif

	/* try and coalesce with left neighbour */
	neighbour = bt->prev_segment;
	if (neighbour!=NULL
		&& neighbour->type == btt_free
		&& neighbour->base + neighbour->size == bt->base)
    {
		free_list_remove (arena, neighbour);
		segment_list_remove (arena, neighbour);
		bt->base = neighbour->base;
		bt->size += neighbour->size;
		pool_free (bt_pool, neighbour);
#ifdef RA_STATS
		arena->free_segment_count--;
#endif
    }

	/* try to coalesce with right neighbour */
	neighbour = bt->next_segment;
	if (neighbour!=NULL
		&& neighbour->type == btt_free
		&& bt->base + bt->size == neighbour->base)
    {
		free_list_remove (arena, neighbour);
		segment_list_remove (arena, neighbour);
		bt->size += neighbour->size;
		pool_free (bt_pool, neighbour);
#ifdef RA_STATS
		arena->free_segment_count--;
#endif
    }

	if (bt->next_segment!=NULL && bt->next_segment->type == btt_span
		&& bt->prev_segment!=NULL && bt->prev_segment->type == btt_span)
    {
		struct bt *next = bt->next_segment;
		struct bt *prev = bt->prev_segment;
		segment_list_remove (arena, next);
		segment_list_remove (arena, prev);
		segment_list_remove (arena, bt);
		arena->import_free (arena->import_handle, bt->base, bt->ref);
#ifdef RA_STATS
		arena->span_count--;
		arena->export_count++;
		arena->free_segment_count--;
		arena->free_resource_count-=bt->size;
#endif
		pool_free (bt_pool, next);
		pool_free (bt_pool, prev);
		pool_free (bt_pool, bt);
    } 
	else
		free_list_insert (arena, bt);
}


/*----------------------------------------------------------------------------
<function>
	FUNCTION:   attempt_alloc_aligned

	PURPOSE:    Attempt an allocation from an arena.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  size - the requested allocation size.
	            Out: ref - the user references associated with the allocated
	                 segment.
	            In:  alignment - required alignment, or 0
	            Out: base - allocated resource base
	RETURNS:	0 failure
	            1 success
</function>
------------------------------------------------------------------------------*/
static int
attempt_alloc_aligned (struct ra_arena *arena,
					   IMG_UINT64 size,
					   void **ref,
					   IMG_UINT64 alignment,
					   IMG_UINT64 *base)
{
	unsigned int index;
	IMG_ASSERT (arena!=NULL);

	/* search for a near fit free boundary tag, start looking at the
	   Log2 free table for our required size and work on up the
	   table. */
	index = Log2 (size);
	/* If the size required is exactly 2**n then use the n bucket, because
	   we know that every free block in that bucket is larger than 2**n,
	   otherwise start at then next bucket up. */
	if (1ull<<index < size)
		index++;

	while (index < FREE_TABLE_LIMIT && arena->head_free[index]==NULL)
		index++;
	while (index < FREE_TABLE_LIMIT)
	{
		if (arena->head_free[index]!=NULL)
		{
			/* we have a cached free boundary tag */
			struct bt *bt;

			bt = arena->head_free [index];
			while (bt!=NULL)
			{
				IMG_UINT64 aligned_base;
#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
				if (alignment>1)
				{
					aligned_base = ((bt->base + alignment - 1) / alignment ) * alignment;
				}
				else
#endif	//#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
				{
					aligned_base = bt->base;
				}
				trace (("  ra:attempt_alloc_aligned (), checking boundary tag\n"));
				if (bt->base + bt->size >= aligned_base + size)
				{
				    free_list_remove (arena, bt);
			
					IMG_ASSERT (bt->type == btt_free);

#ifdef RA_STATS
					arena->live_segment_count++;
					arena->free_segment_count--;
					arena->free_resource_count-=bt->size;
#endif

					/* with alignment we might need to discard the front of this segment */
					if (aligned_base > bt->base)
					{
						struct bt *neighbour;
						neighbour = segment_split (arena, bt, aligned_base-bt->base);
						/* partition the buffer, create a new boundary tag */
						if (neighbour==NULL)
							return 0;
						free_list_insert (arena, bt);
#ifdef RA_STATS
						arena->free_segment_count++;
						arena->free_resource_count+=bt->size;
#endif
						bt = neighbour;
					}

					/* the segment might be too big, if so, discard the back of the segment */
					if (bt->size > size)
					{
						struct bt *neighbour;
						neighbour = segment_split (arena, bt, size);
						/* partition the buffer, create a new boundary tag */
						if (neighbour==NULL)
							return 0;
						free_list_insert (arena, neighbour);
#ifdef RA_STATS
						arena->free_segment_count++;
						arena->free_resource_count+=neighbour->size;
#endif
					}

					bt->type = btt_live;

					if (!HASH_Insert (arena->segment_hash, bt->base, (IMG_UINTPTR) bt))
					{
						free_bt (arena, bt);
						return 0;
					}

					if (ref!=NULL)
						*ref = bt->ref;

					*base = bt->base;
					return 1;
				}
				bt = bt->next_free;
			}
			
		}
		index++;
	}
	
	return 0;
}


#if !defined (IMG_KERNEL_MODULE)
/*----------------------------------------------------------------------------
<function>
	FUNCTION:   attempt_random_alloc_aligned

	PURPOSE:    Attempt an allocation from an arena, selecting randomly the
				base address.
	
	PARAMETERS:	In:  arena - the arena.
	            In:  size - the requested allocation size.
	            Out: ref - the user references associated with the allocated
	                 segment.
	            In:  alignment - required alignment, or 0
	            Out: base - allocated resource base
	RETURNS:	0 failure
	            1 success
</function>
------------------------------------------------------------------------------*/
static int
attempt_random_alloc_aligned (struct ra_arena *arena,
					   IMG_UINT64 size,
					   void **ref,
					   IMG_UINT64 alignment,
					   IMG_UINT64 *base)
{
	unsigned int min_index;
	unsigned int random_index;
	unsigned int index;
	IMG_BOOL backwards = IMG_FALSE;

	IMG_ASSERT (arena!=NULL);

	/* Determine the minimum possible index for the requested size
	   in the log-based table */
	/* search for a near fit free boundary tag, start looking at the
	   Log2 free table for our required size and work on up the
	   table. */
	min_index = Log2 (size);
	/* If the size required is exactly 2**n then use the n bucket, because
	   we know that every free block in that bucket is larger than 2**n,
	   otherwise start at then next bucket up. */
	if (1ull<<min_index < size)
	min_index++;

	/* Select randomly an index between the minimun and
	   the maximum (determined by the size of the block) */
	random_index = (rand() % (arena->max_index - min_index + 1)) + min_index;
	index = random_index;

	/* From the index selected, look forwards for the first index 
	   where there are free segments */
	while (index <= arena->max_index && arena->head_free[index]==NULL)
		index++;
	if (index > arena->max_index)
	{
		backwards = IMG_TRUE;
	/* If not found forwards, look backwards from the selected index */
		index = random_index - 1;
		while (index >= min_index && arena->head_free[index]==NULL)
			index--;
	/* Not enough space in this block for the requested size */
		if(index < min_index)
			return 0;
	}

	/* Starting from the selected position of the table,
	   iterate to find suitable segments */
	while (index <= arena->max_index && index >= min_index)
	{
		IMG_UINT64 aligned_base = 0;
		if (arena->head_free[index]!=NULL)
		{
			/* we have a cached free boundary tag */
			struct bt *bt;
			IMG_UINT32 ui32NoFreeSegments = 0;
			IMG_UINT32 ui32RandomSegment;
//			IMG_UINT32 ui32i;
			IMG_UINT64 ui32RandValue;
			IMG_UINT64 ui64PosNum;
			IMG_UINT64 ui32BreakDown;

			bt = arena->head_free [index];

			/* Count for the number of free segments in the list, large enough to allocate the requested size */
			while (bt!=NULL)
			{
				if (alignment>1)
					aligned_base = ((bt->base + alignment - 1) / alignment) * alignment;
				else
					aligned_base = bt->base;
				if (bt->base + bt->size >= aligned_base + size)
				{
					ui32NoFreeSegments++;
				}
				bt = bt->next_free;
			}
			if (ui32NoFreeSegments == 0)
			{
				if (backwards == IMG_FALSE)
					index++;
				else
					index--;
				if (index > arena->max_index)
				{
					index = random_index-1;
					backwards = IMG_TRUE;
				}
				continue;
			}

			/* Select randomly one of the counted segments */
			ui32RandomSegment = (rand() % ui32NoFreeSegments) + 1;

			/* Find the selected segment */
			bt = arena->head_free [index];
			ui32NoFreeSegments = 0;
//			ui32i = 0;
			while (bt!=NULL)
			{
				if (alignment>1)
					aligned_base = ((bt->base + alignment - 1) / alignment) * alignment;
				else
					aligned_base = bt->base;
				if (bt->base + bt->size >= aligned_base + size)
				{
					ui32NoFreeSegments++;
					if (ui32NoFreeSegments == ui32RandomSegment)
					{
						break;
					}
				}
				bt = bt->next_free;
			}

			/* Select randomly a base address (properly aligned according to the alignment), within the segment */
			ui64PosNum = (bt->base + bt->size - aligned_base - size) / alignment + 1;  // number of possible base addresses
			ui32BreakDown = (IMG_UINT32) (ui64PosNum / (RAND_MAX+(IMG_UINT64)1) + 1);  // gap between possible addresses
																					   // (possibilities have to be within the range of rand() ) 
			ui32RandValue = rand() % (ui64PosNum / ui32BreakDown);					   // random value
			aligned_base += ui32RandValue * alignment * ui32BreakDown;				   // calculate the random base address
				
			free_list_remove (arena, bt);
			IMG_ASSERT (bt->type == btt_free);

#ifdef RA_STATS
			arena->live_segment_count++;
			arena->free_segment_count--;
			arena->free_resource_count-=bt->size;
#endif

			/* with alignment we might need to discard the front of this segment */
			if (aligned_base > bt->base)
			{
				struct bt *neighbour;
				neighbour = segment_split (arena, bt, aligned_base-bt->base);
				/* partition the buffer, create a new boundary tag */
				if (neighbour==NULL)
					return 0;
				free_list_insert (arena, bt);
#ifdef RA_STATS
				arena->free_segment_count++;
				arena->free_resource_count+=bt->size;
#endif
				bt = neighbour;
			}

			/* the segment might be too big, if so, discard the back of the segment */
			if (bt->size > size)
			{
				struct bt *neighbour;
				neighbour = segment_split (arena, bt, size);
				/* partition the buffer, create a new boundary tag */
				if (neighbour==NULL)
					return 0;
				free_list_insert (arena, neighbour);
#ifdef RA_STATS
				arena->free_segment_count++;
				arena->free_resource_count+=neighbour->size;
#endif
			}

			bt->type = btt_live;

			if (!HASH_Insert (arena->segment_hash, bt->base, (IMG_UINTPTR) bt))
			{
				free_bt (arena, bt);
				return 0;
			}

			if (ref!=NULL)
				*ref = bt->ref;

			*base = bt->base;
			return 1;

		}
		if (backwards == IMG_FALSE)
			index++;
		else
			index--;
		if (index > arena->max_index)
		{
			index = random_index-1;
			backwards = IMG_TRUE;
		}
	}
	
	return 0;
}
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
RA_Initialise (void)
{
	trace (("RA_Initialise()\n"));
	arena_pool = pool_create ("img-arena", sizeof (struct ra_arena));
	if (arena_pool==NULL) goto fail_arena;
	bt_pool = pool_create ("img-bt", sizeof (struct bt));
	if (bt_pool==NULL) goto fail_bt;
	return 1;
  fail_bt:
	pool_delete (arena_pool);
  fail_arena:
	return 0;
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Finalise

	PURPOSE:    To finalise the ra module.
	
	PARAMETERS:	None
	
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
RA_Finalise (void)
{
	if (arena_pool!=NULL)
    {
		pool_delete (arena_pool);
    }
	if (bt_pool!=NULL)
		pool_delete (bt_pool);
}

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
RA_Create (char *name, IMG_UINT64 base, IMG_UINT64 size, size_t quantum,
		   int (*alloc)(void *, IMG_UINT64 size, IMG_UINT64 *actual_size,
						void **ref, unsigned _flags, IMG_UINT64 *base),
		   void (*free) (void *, IMG_UINT64, void *ref),
		   void *import_handle)
{
	struct ra_arena *arena;
	int i;
	arena = pool_alloc (arena_pool);
	if (arena==NULL) goto arena_fail;
	arena->name = name;
	arena->import_alloc = alloc!=NULL ? alloc : RequestAllocFail;
	arena->import_free = free;
	arena->import_handle = import_handle;
	for (i=0; i<FREE_TABLE_LIMIT; i++)
		arena->head_free[i] = NULL;
	arena->head_segment = NULL;
	arena->tail_segment = NULL;
	arena->quantum = quantum;

#ifdef RA_STATS
	arena->span_count = 0;
	arena->live_segment_count = 0;
	arena->free_segment_count = 0;
	arena->free_resource_count = 0;
	arena->total_allocs = 0;
	arena->total_frees = 0;
	arena->import_count = 0;
	arena->export_count = 0;
#endif
  
	arena->segment_hash = HASH_Create (MINIMUM_HASH_SIZE);
	if (arena->segment_hash==NULL) goto hash_fail;
	if (size>0)
    {
#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
		size = (size + quantum - 1) / quantum * quantum;
#endif	//#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
		if (!ra_insert_resource (arena, base, size)) goto insert_fail;
    }
	return arena;

  insert_fail:
	HASH_Delete (arena->segment_hash);
  hash_fail:
	pool_free (arena_pool, arena);
  arena_fail:
	return NULL;
}

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
RA_Delete (struct ra_arena *arena)
{
	if (arena!=NULL)
    {
		int i;

		trace (("RA_Delete: name=%s\n", arena->name));

		for (i=0; i<FREE_TABLE_LIMIT; i++)
			arena->head_free[i] = NULL;

		while (arena->head_segment!=NULL)
		{
			struct bt *bt = arena->head_segment;
			IMG_ASSERT (bt->type == btt_free);
			segment_list_remove (arena, bt);
#ifdef RA_STATS
			arena->span_count--;
#endif
		}
		HASH_Delete (arena->segment_hash);
		pool_free (arena_pool, arena);
    }
}

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
RA_Add (struct ra_arena *arena, IMG_UINT64 base, size_t size)
{
#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
	size = (size + arena->quantum - 1) / arena->quantum * arena->quantum;
#endif	//#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
	return ra_insert_resource (arena, base, size);
}

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
	RETURNS:	1 - success
	            0 - failure
</function>
------------------------------------------------------------------------------*/
int
RA_Alloc (struct ra_arena *arena, IMG_UINT64 request_size, IMG_UINT64 *actual_size,
		  void **ref, unsigned flags, IMG_UINT64 alignment, IMG_UINT64 *base)
{
	int result = 0;
	IMG_UINT64 size = request_size;

	IMG_ASSERT (arena!=NULL);

#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
	size = ((size + arena->quantum - 1)/arena->quantum)*arena->quantum;
#endif	//#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
	if (actual_size!=NULL)
		*actual_size = size;

	trace (("RA_Alloc  : name='%s'  size=0x%lx(0x%lx) ...\n", 
		   arena->name, size, request_size));

	/* if allocation failed then we might have an import source which
	   can provide more resource, else we will have to fail the
	   allocation to the caller. */
	if (flags == SEQUENTIAL_ALLOCATION)
	{
		result = attempt_alloc_aligned (arena, size, ref, alignment, base);
		if (!result)
		{
			IMG_ASSERT(IMG_FALSE);
		}
	}
	else if (flags == RANDOM_ALLOCATION)
	{
#if !defined (IMG_KERNEL_MODULE)
		result = attempt_random_alloc_aligned (arena, size, ref, alignment, base);
		if (!result)
		{
			IMG_ASSERT(IMG_FALSE);
		}
#else
		IMG_ASSERT(IMG_FALSE);		/* Not supported in kernel mode. */
		return 0;
#endif
	}

	if (!result)
    {
		void *import_ref;
		IMG_UINT64 import_base;
		IMG_UINT64 import_size = size;
		
		result = 
			arena->import_alloc (arena->import_handle, import_size, &import_size,
								 &import_ref, flags, &import_base);
		if (!result)
		{
			IMG_ASSERT(IMG_FALSE);
		}
		if (result)
		{
			struct bt *bt;
			bt = ra_insert_resource_span (arena, import_base, import_size);
			/* successfully import more resource, create a span to
			   represent it and retry the allocation attempt */
			if (bt==NULL)
			{
				/* insufficient resources to insert the newly acquired span,
				   so free it back again */
				arena->import_free (arena->import_handle, import_base,
									import_ref);
				trace (("  RA_Alloc: name='%s'  size=0x%x = NULL\n", 
						arena->name, size));
				RA_Dump (arena);
				return 0;
			}
			bt->ref = import_ref;
#ifdef RA_STATS
			arena->free_segment_count++;
			arena->free_resource_count+=import_size;
			arena->import_count++;
			arena->span_count++;
#endif
			if (flags == SEQUENTIAL_ALLOCATION)
			{
				result = attempt_alloc_aligned (arena, size, ref, alignment, base);
				if (!result)
				{
					IMG_ASSERT(IMG_FALSE);
				}
			}
			else if (flags == RANDOM_ALLOCATION)
			{
#if !defined (IMG_KERNEL_MODULE)
				result = attempt_random_alloc_aligned (arena, size, ref, alignment, base);
				if (!result)
				{
					IMG_ASSERT(IMG_FALSE);
				}
#else
				IMG_ASSERT(IMG_FALSE);		/* Not supported in kernel mode. */
				return 0;
#endif
			}
		}
    }
#ifdef RA_STATS
	if (result)
		arena->total_allocs++;
#endif    
	trace (("  RA_Alloc: name=%s size=0x%lx = 0x%lx\n", arena->name, size, base));
  
	RA_Dump (arena);
	/*
	 * ra_stats (arena);
	 */

	return result;
}

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
RA_Free (struct ra_arena *arena, IMG_UINT64 base, void *ref)
{
	struct bt *bt;
	
	IMG_ASSERT (arena!=NULL);
		
	(void)ref;

	bt = (struct bt *) HASH_Remove (arena->segment_hash, base);
	IMG_ASSERT (bt->base == base);

#ifdef RA_STATS
	arena->total_frees++;
#endif
	
	free_bt (arena, bt);
	
	trace (("RA_Free: name=%s base=0x%lx\n", arena->name, base));
	RA_Dump (arena);
}

#ifdef RA_DUMP

static char *
bt_type (enum bt_type type)
{
	switch (type)
    { 
    case btt_span: return "span";
    case btt_free: return "free";
    case btt_live: return "live";
    }
	return "junk";
}

/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Dump

	PURPOSE:    To dump a readable description of an arena. Diagnostic only.
	
	PARAMETERS:	In:  arena - the arena to dump.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
RA_Dump (struct ra_arena *arena)
{
	struct bt *bt;
	IMG_ASSERT (arena!=NULL);
	fprintf (stderr, "Arena %s\n", arena->name);
	fprintf (stderr, "  alloc=%p free=%p handle=%p quantum=%d\n", 
			 arena->import_alloc, arena->import_free, arena->import_handle,
			 arena->quantum);
	fprintf (stderr, "  segment Chain:\n");
	if (arena->head_segment!=NULL && arena->head_segment->prev_segment!=NULL)
		fprintf (stderr, "  error: head boundary tag has invalid prev_segment\n");
	if (arena->tail_segment!=NULL && arena->tail_segment->next_segment!=NULL)
		fprintf (stderr, "  error: tail boundary tag has invalid next_segment\n");
    
	for (bt=arena->head_segment; bt!=NULL; bt=bt->next_segment)
    {
		fprintf (stderr, "\tbase=0x%lx size=%d type=%s  ref=%p\n", 
				 (unsigned long) bt->base, bt->size, bt_type (bt->type),
				 bt->ref);
    }

#ifdef HASH_TRACE
	HASH_Dump (arena->segment_hash);
#endif
}
#endif

#ifdef RA_STATS
/*----------------------------------------------------------------------------
<function>
	FUNCTION:   RA_Stats

	PURPOSE:    To dump the statistics associated with an arena.
	
	PARAMETERS:	In:  arena - the arena.
	            
	RETURNS:	None
</function>
------------------------------------------------------------------------------*/
void
RA_Stats (struct ra_arena *arena)
{
	IMG_ASSERT (arena!=NULL);
	fprintf (stderr, "Arena %s\n", arena->name);
	fprintf (stderr, "\tspan count = %d\n", arena->span_count);
	fprintf (stderr, "\tlive segment count = %d\n", arena->live_segment_count);
	fprintf (stderr, "\tfree segment count = %d\n", arena->free_segment_count);
	fprintf (stderr, "\tfree resource count = %d\n", arena->free_resource_count);
	fprintf (stderr, "\ttotal allocs = %d\n", arena->total_allocs);
	fprintf (stderr, "\ttotal frees = %d\n", arena->total_frees);
	fprintf (stderr, "\timport count = %d\n", arena->import_count);
	fprintf (stderr, "\texport count = %d\n", arena->export_count);
	fprintf (stderr, "\n");
}
#endif
