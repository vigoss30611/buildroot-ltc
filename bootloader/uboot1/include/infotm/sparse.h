#ifndef _IROM_SPARSE_H__
#define _IROM_SPARSE_H__


extern int sparse_burn(int dev, loff_t offs, int size,
		int step, int (* get_data)(void *));
#endif /* _IROM_SPARSE_H__ */

