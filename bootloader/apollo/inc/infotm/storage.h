
#ifndef _STORAGE_H_
#define _STORAGE_H_
#include <linux/types.h>
struct part_sz {
	char name[12];
	int offs;
	int size;
};
extern struct part_sz default_cfg[];
extern int partnum;
extern int has_local;
extern unsigned int sectors[];  
extern loff_t spare_area;
extern int storage_device(void);
extern int storage_part(void);
extern loff_t storage_offset(const char *p);
extern loff_t storage_size(const char *p);
extern void storage_init_part(const char *p, loff_t offset, loff_t size);

#define INFO_PART_STATE_BASE 0x14
#define DISK_PARTITIONED    0x359FC953
#define INFO_IMAGE_BASE_BUFFER 0x20
#define  INFO_PART_CFG_BASE 1
#define INFO_SPARE_IMAGES 11

extern int block_partition(struct part_sz *psz);
extern loff_t block_get_base(const char* target);
extern int block_is_parted(void);
extern int block_get_part_info(struct part_sz *psz);
#endif /* _STORAGE_H_ */

