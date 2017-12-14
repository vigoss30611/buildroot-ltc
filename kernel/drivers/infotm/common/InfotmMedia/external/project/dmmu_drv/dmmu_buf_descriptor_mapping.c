/*
 */

#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include "dmmu_buf_descriptor_mapping.h"

#define IMAPX_PAD_INT(x) (((x) + (BITS_PER_LONG - 1)) & ~(BITS_PER_LONG - 1))

/**
 * Allocate a descriptor table capable of holding 'count' mappings
 * @param count Number of mappings in the table
 * @return Pointer to a new table, NULL on error
 */
static dmmu_buf_descriptor_table * descriptor_table_alloc(int count);

void _imapx_dmmu_buf_set_nonatomic_bit( uint32_t nr, uint32_t *addr )
{
	addr += nr >> 5; /* find the correct word */
	nr = nr & ((1 << 5)-1); /* The bit number within the word */

	(*addr) |= (1 << nr);
}

void _imapx_dmmu_buf_clear_nonatomic_bit( uint32_t nr, uint32_t *addr )
{
	addr += nr >> 5; /* find the correct word */
	nr = nr & ((1 << 5)-1); /* The bit number within the word */

	(*addr) &= ~(1 << nr);
}

uint32_t _imapx_dmmu_buf_test_bit( uint32_t nr, uint32_t *addr )
{
	addr += nr >> 5; /* find the correct word */
	nr = nr & ((1 << 5)-1); /* The bit number within the word */

	return (*addr) & (1 << nr);
}

int _imapx_dmmu_buf_internal_find_first_zero_bit( uint32_t value )
{
	uint32_t inverted;
	uint32_t negated;
	uint32_t isolated;
	uint32_t leading_zeros;

	/* Begin with xxx...x0yyy...y, where ys are 1, number of ys is in range  0..31 */
	inverted = ~value; /* zzz...z1000...0 */
	/* Using count_trailing_zeros on inverted value -
	 * See ARM System Developers Guide for details of count_trailing_zeros */

	/* Isolate the zero: it is preceeded by a run of 1s, so add 1 to it */
	negated = (uint32_t)-inverted ; /* -a == ~a + 1 (mod 2^n) for n-bit numbers */
	/* negated = xxx...x1000...0 */

	isolated = negated & inverted ; /* xxx...x1000...0 & zzz...z1000...0, zs are ~xs */
	/* And so the first zero bit is in the same position as the 1 == number of 1s that preceeded it
	 * Note that the output is zero if value was all 1s */

	leading_zeros = 32-fls( isolated );

	return 31 - leading_zeros;
}


uint32_t _imapx_dmmu_buf_find_first_zero_bit( const uint32_t *addr, uint32_t maxbit )
{
	uint32_t total;

	for ( total = 0; total < maxbit; total += 32, ++addr ) {
		int result;
		result = _imapx_dmmu_buf_internal_find_first_zero_bit( *addr );

		/* non-negative signifies the bit was found */
		if ( result >= 0 ) {
			total += (uint32_t)result;
			break;
		}
	}

	/* Now check if we reached maxbit or above */
	if ( total >= maxbit ) {
		total = maxbit;
	}

	return total; /* either the found bit nr, or maxbit if not found */
}

/**
 * Free a descriptor table
 * @param table The table to free
 */
static void descriptor_table_free(dmmu_buf_descriptor_table * table);

dmmu_buf_descriptor_mapping * dmmu_buf_descriptor_mapping_create(int init_entries, int max_entries)
{
	dmmu_buf_descriptor_mapping * map = kzalloc(sizeof(dmmu_buf_descriptor_mapping), GFP_KERNEL);

	init_entries = IMAPX_PAD_INT(init_entries);
	max_entries = IMAPX_PAD_INT(max_entries);

	if (NULL != map) {
		map->table = descriptor_table_alloc(init_entries);
		if (NULL != map->table) {
			mutex_init(&map->lock);
            _imapx_dmmu_buf_set_nonatomic_bit(0, map->table->usage);
            map->max_nr_mappings_allowed = max_entries;
            map->current_nr_mappings = init_entries;
            return map;
		}
		kfree(map);
	}
	return NULL;
}

void dmmu_buf_descriptor_mapping_destroy(dmmu_buf_descriptor_mapping * map)
{
	descriptor_table_free(map->table);
	kfree(map);
}

int dmmu_buf_descriptor_mapping_allocate_mapping(dmmu_buf_descriptor_mapping * map, void * target)
{
	int descriptor = -1;/*-EFAULT;*/
	mutex_lock(&map->lock);
	descriptor = _imapx_dmmu_buf_find_first_zero_bit(map->table->usage, map->current_nr_mappings);
	if (descriptor == map->current_nr_mappings) {
		int nr_mappings_new;
		/* no free descriptor, try to expand the table */
		dmmu_buf_descriptor_table * new_table;
		dmmu_buf_descriptor_table * old_table = map->table;
		nr_mappings_new= map->current_nr_mappings *2;

		if (map->current_nr_mappings >= map->max_nr_mappings_allowed) {
			descriptor = -1;
			goto unlock_and_exit;
		}

		new_table = descriptor_table_alloc(nr_mappings_new);
		if (NULL == new_table) {
			descriptor = -1;
			goto unlock_and_exit;
		}

		memcpy(new_table->usage, old_table->usage, (sizeof(unsigned long)*map->current_nr_mappings) / BITS_PER_LONG);
		memcpy(new_table->mappings, old_table->mappings, map->current_nr_mappings * sizeof(void*));
		map->table = new_table;
		map->current_nr_mappings = nr_mappings_new;
		descriptor_table_free(old_table);
	}

	/* we have found a valid descriptor, set the value and usage bit */
	_imapx_dmmu_buf_set_nonatomic_bit(descriptor, map->table->usage);
	map->table->mappings[descriptor] = target;

unlock_and_exit:
	mutex_unlock(&map->lock);
	return descriptor;
}

int dmmu_buf_descriptor_mapping_get(dmmu_buf_descriptor_mapping * map, int descriptor, void** target)
{
	int result = -1;/*-EFAULT;*/
	BUG_ON(!map);
	mutex_lock(&map->lock);
	if ((descriptor > 0) && (descriptor < map->current_nr_mappings) && _imapx_dmmu_buf_test_bit(descriptor, map->table->usage)) {
		*target = map->table->mappings[descriptor];
		result = 0;
	} else *target = NULL;
	mutex_unlock(&map->lock);
	return result;
}

int dmmu_buf_descriptor_mapping_set(dmmu_buf_descriptor_mapping * map, int descriptor, void * target)
{
	int result = -1;/*-EFAULT;*/
	mutex_lock(&map->lock);
	if ((descriptor > 0) && (descriptor < map->current_nr_mappings) && _imapx_dmmu_buf_test_bit(descriptor, map->table->usage)) {
		map->table->mappings[descriptor] = target;
		result = 0;
	}
	mutex_unlock(&map->lock);
	return result;
}

void dmmu_buf_descriptor_mapping_free(dmmu_buf_descriptor_mapping * map, int descriptor)
{
	mutex_lock(&map->lock);
	if ((descriptor > 0) && (descriptor < map->current_nr_mappings) && _imapx_dmmu_buf_test_bit(descriptor, map->table->usage)) {
		map->table->mappings[descriptor] = NULL;
		_imapx_dmmu_buf_clear_nonatomic_bit(descriptor, map->table->usage);
	}
	mutex_unlock(&map->lock);
}

static dmmu_buf_descriptor_table * descriptor_table_alloc(int count)
{
	dmmu_buf_descriptor_table * table;

	table = kzalloc(sizeof(dmmu_buf_descriptor_table) + ((sizeof(unsigned long) * count)/BITS_PER_LONG) + (sizeof(void*) * count), GFP_KERNEL);

	if (NULL != table) {
		table->usage = (uint32_t*)((u8*)table + sizeof(dmmu_buf_descriptor_table));
		table->mappings = (void**)((u8*)table + sizeof(dmmu_buf_descriptor_table) + ((sizeof(unsigned long) * count)/BITS_PER_LONG));
	}

	return table;
}

static void descriptor_table_free(dmmu_buf_descriptor_table * table)
{
	kfree(table);
}

