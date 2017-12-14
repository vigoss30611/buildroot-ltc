
#ifndef _STORAGE_H_
#define _STORAGE_H_
#include <linux/types.h>

#define SPI_ERASE_SIZE	0x10000
#define SZ_1K		1024
#define DEFAULT_UBOOT0_SIZE 	0xC000
#define DEFAULT_ITEM_SIZE 		0x4000
#define DEFAULT_UBOOT1_SIZE 	0x80000
#define UPGRADE_PARA_SIZE		0x400
#define RESERVED_ENV_SIZE		0xfc00
struct part_sz {
	char name[12];
	int offs;
	int size;
};

struct storage_part_t {
    int inited;
    uint64_t offset;
    uint64_t size;
    char part_name[16];
    char fs_type[16];
    char dev_path[32];
};

typedef enum {
	IMAGE_UBOOT = 0,
	IMAGE_ITEM = 1,
	IMAGE_RAMDISK = 2,
	IMAGE_KERNEL = 3,
	IMAGE_KERNEL_UPGRADE = 4,
	IMAGE_SYSTEM = 5,
	IMAGE_UBOOT1 = 16,
	IMAGE_RESERVED = 17,
	IMAGE_NONE = 18,
}image_type_t;

extern struct part_sz default_cfg[];
extern int partnum;
extern int has_local;
extern unsigned int sectors[];  
extern loff_t spare_area;
extern int storage_device(void);
extern loff_t storage_offset(const char *p);
extern uint64_t storage_size(const char *p);
extern void storage_init_part(const char *p, loff_t offset, loff_t size);
extern int storage_number(const char *p);
extern void storage_init(void);
extern struct storage_part_t *storage_id(int id);

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

