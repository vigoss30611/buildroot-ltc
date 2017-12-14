#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>
#include <items.h>

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

#define	SPI_SECT_MAP_HEAD		"spi_sect_map"
#define SZ_1K				1024
#define UPGRADE_PARA_SIZE	1024
#define RESERVED_ENV_SIZE	0xfc00
#define MAX_SECTS_IN_BLOCK		128
#define HEAD_LENGTH				16

struct sect_map_head_t {
	char head[HEAD_LENGTH];
    int16_t  ec;
    int16_t   vtblk;
    int16_t        timestamp;
    int8_t    	   phy_page_map[MAX_SECTS_IN_BLOCK];
};

struct spiblk_part_t {
    int fd;
    int inited;
    int offset;
    int size;
    char part_name[16];
    char part_type[16];
    char fs_type[16];
};
static struct spiblk_part_t spiblk_part[IMAGE_NONE];

static int spiblk_init(void)
{
	struct spiblk_part_t *spi_part;
	char spi_parts[ITEM_MAX_LEN];
	char part_name[ITEM_MAX_LEN];
	char part_type[ITEM_MAX_LEN];
	char fs_type[ITEM_MAX_LEN];
	int i = 0, index = 0, fs_part_num = 0, creat_part = 0;
	uint64_t offset = 0, offset_normal = 0, offset_fs = 0, size = 0, erase_size = (512 * MAX_SECTS_IN_BLOCK);
	int part_size, fd, ret;

	spi_part = &spiblk_part[0];
	memset(spiblk_part, 0, IMAGE_NONE * sizeof(struct spiblk_part_t));
	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(spi_parts, "part%d", i);
		if (item_exist(spi_parts)) {
			item_string(part_name, spi_parts, 0);
			part_size = item_integer(spi_parts, 1);
			item_string(part_type, spi_parts, 2);
			item_string(fs_type, spi_parts, 3);

			if (!strncmp(part_type, "boot", strlen((const char*)"boot"))) {
				if (part_size == 0)
					continue;
				spi_part = &spiblk_part[i];
	            spi_part->size = SZ_1K;
	            spi_part->size *= part_size;
	            spi_part->offset = offset;
	            spi_part->inited = 1;
	            offset += spi_part->size;
	            //after item reserve UPGRADE_PARA_SIZE for upgrade
	            if (!strncmp(part_name, "item", strlen((const char*)"item")))
	            	offset += (UPGRADE_PARA_SIZE + RESERVED_ENV_SIZE);
	            snprintf(spi_part->part_name, 16, "%s", part_name);
	            snprintf(spi_part->part_type, 16, "%s", part_type);
	            printf("spiblk init %s %d \n", spi_part->part_name, spi_part->size);
			}
		}
	}

	if (offset & (erase_size - 1)) {
		offset += (erase_size - 1);
		offset &= ~(erase_size - 1);
	}
	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(spi_parts, "part%d", i);
		if (item_exist(spi_parts)) {
			item_string(part_name, spi_parts, 0);
			part_size = item_integer(spi_parts, 1);
			item_string(part_type, spi_parts, 2);
			item_string(fs_type, spi_parts, 3);

			if (!strncmp(part_name, "system", strlen((const char*)"system"))) {
				spi_part = &spiblk_part[i];
				index++;
	            spi_part->size = SZ_1K;
	            spi_part->size *= part_size;
				spi_part->offset = offset;
				spi_part->inited = 1;
				offset += spi_part->size;
				snprintf(spi_part->part_name, 16, "%s", part_name);
				snprintf(spi_part->part_type, 16, "%s", part_type);
				snprintf(spi_part->fs_type, 16, "%s", fs_type);
				printf("spiblk init %s %d \n", spi_part->part_name, spi_part->size);
			}
		}
	}

    return 0;
}

int mkburn(int argc, char * argv[])
{
	char *images_path = NULL, *out_file = NULL, path[128];
	uint8_t *item_buf;
	int fd, fo, file_size, flash_size = 16, ret, i, j, logic_blk_addr;
	struct spiblk_part_t *spi_part;
	struct sect_map_head_t *sect_map_head;

	if(!images_path && argv[1]) {
		images_path = argv[1];
		item_buf = malloc(0x100000);
		if(!item_buf) {
			printf("no buffer.\n");
			return -1;
		}
		sprintf(path, "%s/items.itm", images_path);
		fd = open(path, O_RDONLY);
		if(fd < 0) {
			printf("invalid item file: %s\n", path);
			free(item_buf);
			return -1;
		}
		file_size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		ret = read(fd, item_buf, file_size);
		if (ret < 0) {
			free(item_buf);
			return -1;
		}
		item_init(item_buf, ITEM_SIZE_NORMAL);
		free(item_buf);
		close(fd);
	}
	if (item_exist("board.disk") && (!item_equal("board.disk", "flash", 0))) {
		printf("we just support spi flahs device \n");
		return 0;
	}

	for(;;)
	{
		int option_index = 0;
		static const char *short_options = "o:p:s";
		static const struct option long_options[] = {
			{"out", 0, NULL, 'o'},
			{"size", 0, NULL, 'p'},
			{"aa", 0, NULL, 's'},
			{0,0,0,0},
		};

		int c = getopt_long(argc, argv, short_options,
					        long_options, &option_index);

		if (c == EOF) {
	    	break;
		}

		switch (c) {
			case 'o':
				if(!out_file)
					out_file = optarg;
				break;
			case 'p':
				flash_size = strtoul(optarg, NULL, 0);
				break;
			default:
				break;
		}
	}
	printf("\nGenerating spi burnimg %s ...\n", out_file);
	ret = spiblk_init();
	if (ret < 0)
		return 0;

	item_buf = malloc(0x100000);
	if(!item_buf) {
		printf("no buffer.\n");
		return -1;
	}
	memset(item_buf, 0xff, 0x100000);
	fo = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if(fo < 0) {
		printf("create burn file: %s\n", out_file);
		free(item_buf);
		return -1;
	}
	lseek(fo, 0, SEEK_SET);
	for (i=0; i<flash_size; i++)
		write(fo, item_buf, 0x100000);

	for (i=0; i<IMAGE_NONE; i++) {
		spi_part = &spiblk_part[i];
		if (spi_part->inited) {
			if (strcmp(spi_part->part_name, "uboot") == 0) {
				sprintf(path, "%s/uboot0.isi", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "item") == 0) {
				sprintf(path, "%s/items.itm", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "ramdisk") == 0) {
				sprintf(path, "%s/ramdisk.img", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "kernel0") == 0) {
				sprintf(path, "%s/uImage", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "kernel1") == 0) {
				sprintf(path, "%s/uImage", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "system") == 0) {
				sprintf(path, "%s/rootfs.squashfs", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else if (strcmp(spi_part->part_name, "uboot1") == 0) {
				sprintf(path, "%s/uboot1.isi", images_path);
				fd = open(path, O_RDONLY);
				if(fd < 0) {
					printf("invalid item file: %s\n", path);
					continue;
				}
			} else {
				continue;
			}
			//file_size = lseek(fd, 0, SEEK_END);
			//printf("write %s %llx \n", path, spi_part->offset);sect_map_head = (struct sect_map_head_t *)head_buf;
			lseek(fo, spi_part->offset, SEEK_SET);
			logic_blk_addr = 0;
			for(;;) {
				if ((strcmp(spi_part->part_name, "system") == 0) 
					&& (!strncmp(spi_part->part_type, "fs", strlen((const char*)"fs")))) {
						ret = read(fd, item_buf + 512, (MAX_SECTS_IN_BLOCK - 1) * 512);
						if(ret > 0) {
							memset(item_buf, 0xff, 512);
							sect_map_head = (struct sect_map_head_t *)item_buf;
							memcpy(item_buf, SPI_SECT_MAP_HEAD, strlen(SPI_SECT_MAP_HEAD));
							sect_map_head->ec = 0;
							sect_map_head->vtblk = logic_blk_addr;
							sect_map_head->timestamp = 0;
							for (j=0; j<(ret / 512); j++)
								sect_map_head->phy_page_map[j+1] = j;
							write(fo, item_buf, (ret + 512));
							logic_blk_addr++;
						} else {
							break;
						}
				} else {
					ret = read(fd, item_buf, 0x100000);
					if(ret > 0)
						write(fo, item_buf, ret);
	
					//printf("write step %x \n", ret);
					if(ret != 0x100000)
						break;
				}
			}
			close(fd);
		}
	}
	close(fo);
	if (item_buf)
		free(item_buf);
	printf("generated!\n");

	return 0;
}
