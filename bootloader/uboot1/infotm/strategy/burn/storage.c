
#include <common.h>
#include <cdbg.h>
#include <isi.h>
#include <ius.h>
#include <hash.h>
#include <crypto.h>
#include <bootlist.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <led.h>
#include <irerrno.h>
#include <items.h>
#include <storage.h>
#include <linux/types.h>

struct storage_part_t storage_part[IMAGE_NONE];

int storage_device(void)
{
	char buf[32], *devs[] = {
		"bnd", "nnd", "fnd", "eeprom",
		"flash", "hdisk", "mmc0",
		"mmc1", "mmc2", "udisk0",
		"udisk1", "udc", "eth", "ram"};
	int i;

	if (item_exist("board.disk")) {
		if (item_equal("board.disk", "emmc", 0)) {
			return DEV_MMC(0);
		} else {
			return DEV_FLASH;
		}
	}

	for(i = 0; i < ARRAY_SIZE(devs); i++) {
		if(strncmp(devs[i], buf, 20) == 0)
			return i;
	}

	/* if error hanppens, always spi flash ... */
	return DEV_FLASH;
}
int item_strtoul(char* info)
{
	char * p=info;
	int val=0;
	while(*p!='\0'){
		if((*p<'0')||(*p>'9')) break;
		val=val*10;
		val+=*p-'0';
		p++;
	}
	
	if(*p=='G'||*p=='g'){
		printf("unit %c\n",*p);
		val<<=10;
	}else if(0<val&&val<10){
		printf("treate as unit G\n");
		val<<=10;
	}

	return val;
}

int storage_number(const char *p)
{
	char stog_parts[64], item_part_name[16];
	int i = 0;

	while (i < IMAGE_NONE) {
		sprintf(stog_parts, "part%d", i);
		if (item_exist(stog_parts)) {
			item_string(item_part_name, stog_parts, 0);
			if (!strncmp(item_part_name, p, strlen(item_part_name)))
				return i;
		}
		i++;
	}

	return -1;
}

struct storage_part_t *storage_id(int id)
{
	struct storage_part_t *stog_part = NULL;

	if (id < IMAGE_NONE)
		stog_part = &storage_part[id];
	return stog_part;
}

loff_t storage_offset(const char *p)
{
	struct storage_part_t *stog_part;
	int i = 0;
	loff_t parted = 0;

	if(!p)
		return 0;

	for (i=0; i<IMAGE_NONE; i++) {
		stog_part = &storage_part[i];
		if (stog_part->inited && !strncmp(stog_part->part_name, p, strlen(p))) {
			parted = stog_part->offset;
			break;
		}
	}

	return parted;
}

uint64_t storage_size(const char *p)
{
	struct storage_part_t *stog_part;
	int i = 0;
	uint64_t size = 0;

	if(!p)
		return 0;

	for (i=0; i<IMAGE_NONE; i++) {
		stog_part = &storage_part[i];
		if (stog_part->inited && !strncmp(stog_part->part_name, p, strlen(p))) {
			size = stog_part->size;
			break;
		}
	}

	return size;
}

void storage_init_item(void)
{
	struct storage_part_t *stog_part;
	char stog_parts[64];
	char part_name[32];
	char part_type[64];
	char fs_type[16];
	int i = 0;
	uint64_t offset = 0;
	int part_size;

	stog_part = &storage_part[0];
	memset(storage_part, 0, IMAGE_NONE * sizeof(struct storage_part_t));
	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(stog_parts, "part%d", i);
		if (item_exist(stog_parts)) {
			item_string(part_name, stog_parts, 0);
			part_size = item_integer(stog_parts, 1);
			item_string(part_type, stog_parts, 2);
			item_string(fs_type, stog_parts, 3);

			if (!strncmp(part_type, "boot", strlen((const char*)"boot"))) {
				stog_part = &storage_part[i];
	            stog_part->size = SZ_1K;
	            stog_part->size *= part_size;
	            stog_part->offset = offset;
	            stog_part->inited = 1;
	            offset += stog_part->size;
	            sprintf(stog_part->part_name, "%s", part_name);
	            //after item reserve UPGRADE_PARA_SIZE for upgrade
	            if (!strncmp(part_name, "item", strlen((const char*)"item")))
	            	offset += (UPGRADE_PARA_SIZE + RESERVED_ENV_SIZE);
			} 
		}
	}

	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(stog_parts, "part%d", i);
		if (item_exist(stog_parts)) {
			item_string(part_name, stog_parts, 0);
			part_size = item_integer(stog_parts, 1);
			item_string(part_type, stog_parts, 2);
			item_string(fs_type, stog_parts, 3);

			if (!strncmp(part_type, "normal", strlen((const char*)"normal"))) {
				stog_part = &storage_part[i];
	            stog_part->size = SZ_1K;
	            stog_part->size *= part_size;
				if (item_exist("board.disk") && item_equal("board.disk", "flash", 0)) {
					if (offset & (SPI_ERASE_SIZE - 1)) {
						offset += (SPI_ERASE_SIZE - 1);
						offset &= ~(SPI_ERASE_SIZE - 1);
					}
					if (stog_part->size & (SPI_ERASE_SIZE - 1)) {
						stog_part->size += (SPI_ERASE_SIZE - 1);
						stog_part->size &= ~(SPI_ERASE_SIZE - 1);
					}
				}
				stog_part->offset = offset;
				stog_part->inited = 1;
				offset += stog_part->size;
				sprintf(stog_part->fs_type, "%s", fs_type);
				sprintf(stog_part->part_name, "%s", part_name);
			}
		}
	}

	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(stog_parts, "part%d", i);
		if (item_exist(stog_parts)) {
			item_string(part_name, stog_parts, 0);
			part_size = item_integer(stog_parts, 1);
			item_string(part_type, stog_parts, 2);
			item_string(fs_type, stog_parts, 3);

			if (!strncmp(part_type, "fs", strlen((const char*)"fs"))) {

				stog_part = &storage_part[i];
	            stog_part->size = SZ_1K;
	            stog_part->size *= part_size;
				stog_part->offset = offset;
				stog_part->inited = 1;
				offset += stog_part->size;
				sprintf(stog_part->fs_type, "%s", fs_type);
				sprintf(stog_part->part_name, "%s", part_name);
				printf("storage init %s %s %lld %lld \n", stog_part->part_name, stog_part->fs_type, stog_part->offset, stog_part->size);
			}
		}
	}
	stog_part = &storage_part[IMAGE_RESERVED];
	if (!stog_part->inited) {
		stog_part->offset = storage_part[IMAGE_ITEM].offset + storage_part[IMAGE_ITEM].size;
		stog_part->offset += UPGRADE_PARA_SIZE;
		stog_part->size = RESERVED_ENV_SIZE;
		stog_part->inited = 1;
		sprintf(stog_part->part_name, "%s", "reserved");
	}

	return;
}

void storage_init_env(void)
{
	struct storage_part_t *stog_part;

	stog_part = &storage_part[0];
	memset(storage_part, 0, IMAGE_NONE * sizeof(struct storage_part_t));
	stog_part = &storage_part[IMAGE_UBOOT];
	if (!stog_part->inited) {
		stog_part->offset = 0;
		stog_part->size = DEFAULT_UBOOT0_SIZE;
		stog_part->inited = 1;
		sprintf(stog_part->part_name, "%s", "uboot");
	}

	stog_part = &storage_part[IMAGE_ITEM];
	if (!stog_part->inited) {
		stog_part->offset = storage_part[IMAGE_UBOOT].size;
		stog_part->size = DEFAULT_ITEM_SIZE;
		stog_part->inited = 1;
		sprintf(stog_part->part_name, "%s", "item");
	}

	stog_part = &storage_part[IMAGE_RESERVED];
	if (!stog_part->inited) {
		stog_part->offset = storage_part[IMAGE_ITEM].offset + storage_part[IMAGE_ITEM].size;
		stog_part->size = RESERVED_ENV_SIZE + UPGRADE_PARA_SIZE;
		stog_part->inited = 1;
		sprintf(stog_part->part_name, "%s", "reserved");
	}

	stog_part = &storage_part[IMAGE_UBOOT1];
	if (!stog_part->inited) {
		stog_part->offset = storage_part[IMAGE_RESERVED].offset + storage_part[IMAGE_RESERVED].size;
		stog_part->size = DEFAULT_UBOOT1_SIZE;
		stog_part->inited = 1;
		sprintf(stog_part->part_name, "%s", "uboot1");
	}

	return;
}

void storage_init(void)
{
	if (item_exist("part0"))
		storage_init_item();
	else
		storage_init_env();

	if (item_exist("board.disk")) {
		if (item_equal("board.disk", "emmc", 0)) {
			vs_assign_by_id(DEV_MMC(0), 1);
			printf("storage dev emmc \n");
		} else {
			vs_assign_by_id(DEV_FLASH, 1);
			printf("storage dev spi flash \n");
		}
	}

	return;
}
