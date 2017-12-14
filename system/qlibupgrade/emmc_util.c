#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <sys/vfs.h>
#include <qsdk/sys.h>
#include "upgrade.h"
 
#define EMMC_BLK "/dev/mmcblk0"
#define	EMMC_IMAGE_OFFSET	0x200000
#define EMMC_RESERVE_SIZE	0x2000000

struct emmc_part_t {
    int fd;
    int inited;
    uint64_t offset;
    uint64_t size;
    char part_name[16];
    char fs_type[16];
		char part_type[16];
    char dev_path[32];
};

static struct emmc_part_t emmc_blk_part[IMAGE_NONE];

void emmc_creat_partitions(int part_num)
{
	struct emmc_part_t *emmc_part;
	char cmd[512], temp[64];
	uint64_t size = EMMC_RESERVE_SIZE;
	int start_cl, end_cl, i, index = 0;

	sprintf(cmd, "fdisk /dev/mmcblk0 << EOF \n d \n 1 \n d \n 2 \n d \n 3 \n d \n 4 \n ");
	for (i=0; i<IMAGE_NONE; i++) {
		emmc_part = &emmc_blk_part[i];
		if (!emmc_part->inited)
			continue;
	
		if (strncmp(emmc_part->part_type, "fs", strlen((const char*)"fs")))
			continue;

		start_cl = (int)((size + FDISK_UNIT_SIZE - 1) / FDISK_UNIT_SIZE);
		end_cl = ((emmc_part->size + FDISK_UNIT_SIZE - 1) / FDISK_UNIT_SIZE);
		sprintf(temp, "n \n p \n %d \n %d \n %d \n ", index+1, (start_cl + 1), (start_cl + end_cl));
		strcat(cmd, temp);
		size += emmc_part->size;
		index++;
	}
	start_cl = (int)((size + FDISK_UNIT_SIZE - 1) / FDISK_UNIT_SIZE);
	sprintf(temp, "n \n p \n %d \n \n ", (start_cl + 1));
	strcat(cmd, temp);
	sprintf(temp, "w \n EOF \n");
	strcat(cmd, temp);

	printf("%s \n", cmd);
	system(cmd);

	system("mdev -s");
	for (i=0; i<IMAGE_NONE; i++) {
		emmc_part = &emmc_blk_part[i];
		if (!emmc_part->inited)
			continue;
	
		if (strncmp(emmc_part->part_type, "fs", strlen((const char*)"fs")))
			continue;

		if (!strncmp(emmc_part->fs_type, "ext4", strlen("ext4"))) {
			sprintf(cmd, "mke2fs -T ext4 -L %s %s", emmc_part->part_name, emmc_part->dev_path);
			system(cmd);
		} else if (!strncmp(emmc_part->fs_type, "fat", strlen("fat"))) {
			sprintf(cmd, "mkfs.vfat -L %s %s", emmc_part->part_name, emmc_part->dev_path);
			system(cmd);
		}
	}

	return;
}

int emmc_check_part_info(struct emmc_part_t *emmc_part)
{
    struct statfs diskinfo;
    int fd, ret;
    uint64_t size;

	if (access(emmc_part->dev_path, F_OK))
		return -1;

	fd = open(emmc_part->dev_path, O_RDWR);
	if (fd < 0) {
		printf("emmc check open failed %s \n", emmc_part->dev_path);
		return FAILED;
	}

	if ((ret = ioctl(fd, BLKGETSIZE64, &size)) < 0)
    {
        printf("ioctl error %s \n", strerror(errno));
        close(fd);
        return FAILED;
    }
    close(fd);

	printf("emmc check part %lld %lld \n", size, emmc_part->size);
	if (size != emmc_part->size)
		return FAILED;

	return SUCCESS;
}

static int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}      

struct emmc_part_t *emmc_get_emmc_part_t(image_type_t image_num)
{
	struct emmc_part_t *emmc_part = NULL;
	int i;

	for (i = 0; i < IMAGE_NONE; i++) {
		emmc_part = &emmc_blk_part[i];
		if (!emmc_part->inited)
			continue;
#if 0
		printf("emmc [%d] init part_name %s, path %s, size %d\n", i,
				emmc_part->part_name, emmc_part->dev_path, emmc_part->size);
#endif
		if (!strncmp(emmc_part->part_name, image_name[image_num],
					max(strlen(emmc_part->part_name), strlen(image_name[image_num]))))
			break;
	}
	if (i == IMAGE_NONE)
		return NULL;

	return &emmc_blk_part[i];
}


int emmc_init(struct upgrade_t *upgrade, int clear_flag)
{
	struct emmc_part_t *emmc_part;
	char emmc_parts[64];
	char part_name[32];
	char part_type[64];
	char fs_type[16];
	int i = 0, index = 0, creat_part = 0, fs_part_num = 0;
	uint64_t offset = EMMC_IMAGE_OFFSET, offset_normal = 0, offset_fs = EMMC_IMAGE_OFFSET, size = 0;
	int part_size, fd, ret;

	if (access(EMMC_BLK, F_OK) == 0) {
		fd = open(EMMC_BLK, O_RDWR);
		if (fd < 0) {
			printf("emmcblk init open failed \n");
			return FAILED;
		}
	
		if ((ret = ioctl(fd, BLKGETSIZE64, &size)) < 0) {
	        printf("emmcblk ioctl error %s \n", strerror(errno));
	        return FAILED;
	    }
	    close(fd);
	}

	emmc_part = &emmc_blk_part[0];
	memset(emmc_part, 0, IMAGE_NONE * sizeof(struct emmc_part_t));
	for (i=0; i<IMAGE_NONE; i++) {
		sprintf(emmc_parts, "part%d", i);
		if (item_exist(emmc_parts)) {
			item_string(part_name, emmc_parts, 0);
			part_size = item_integer(emmc_parts, 1);
			item_string(part_type, emmc_parts, 2);

			if (!strncmp(part_type, "boot", strlen((const char*)"boot"))) {
				emmc_part = &emmc_blk_part[i];
	            emmc_part->size = SZ_1K;
	            emmc_part->size *= part_size;
	            emmc_part->offset = offset;
	            emmc_part->inited = 1;
	            offset += emmc_part->size;
	            //after item reserve UPGRADE_PARA_SIZE for upgrade
	            if (!strncmp(part_name, "item", strlen((const char*)"item")))
	            	offset += (UPGRADE_PARA_SIZE + RESERVED_ENV_SIZE);
	            sprintf(emmc_part->dev_path, EMMC_BLK);
							sprintf(emmc_part->part_name, "%s", part_name);
							sprintf(emmc_part->part_type, "%s", part_type);
			} else if (!strncmp(part_type, "normal", strlen((const char*)"normal"))) {
				index++;
				offset_normal += (part_size * SZ_1K);
			} else if (!strncmp(part_type, "fs", strlen((const char*)"fs"))) {

				item_string(fs_type, emmc_parts, 3);
				emmc_part = &emmc_blk_part[i];
				if (part_size == -1) {
					emmc_part->size = (size - offset_fs - 2 * FDISK_UNIT_SIZE);
				} else {
					emmc_part->size = SZ_1K;
					emmc_part->size *= part_size;
					if (emmc_part->size > (size - offset_fs - 2 * FDISK_UNIT_SIZE))
						emmc_part->size = (size - offset_fs- 2 * FDISK_UNIT_SIZE);
				}
				emmc_part->size = ((emmc_part->size + FDISK_UNIT_SIZE - 1) / FDISK_UNIT_SIZE);
				emmc_part->size *= FDISK_UNIT_SIZE;
				emmc_part->offset = 0;
				emmc_part->inited = 1;
				offset_fs += emmc_part->size;
				index++;
				fs_part_num++;
				sprintf(emmc_part->dev_path, "%sp%d", EMMC_BLK, index);
				sprintf(emmc_part->fs_type, "%s", fs_type);
				sprintf(emmc_part->part_name, "%s", part_name);
				sprintf(emmc_part->part_type, "%s", part_type);
				printf("emmc init %s %s %lld \n", emmc_part->dev_path, emmc_part->fs_type, emmc_part->size);
				if (clear_flag || emmc_check_part_info(emmc_part))
					creat_part = 1;
			}
		}
	}
	if (creat_part)
		emmc_creat_partitions(fs_part_num);
#if 0
	for (i = 0; i < IMAGE_NONE; i++) {
		emmc_part = &emmc_blk_part[i];
		if (!emmc_part->inited)
			continue;
		printf("spi [%d] init part_name %s, path %s, size %d\n", i,
				emmc_part->part_name, emmc_part->dev_path, emmc_part->size);
	}
#endif
    return SUCCESS;
}

void emmc_deinit(struct upgrade_t *upgrade)
{
	return;
}

void emmc_release_part(struct upgrade_t *upgrade, image_type_t image_num)
{
	struct emmc_part_t *emmc_part;

	emmc_part = emmc_get_emmc_part_t(image_num);
	if (!emmc_part || !emmc_part->inited)
		return;

	close(emmc_part->fd);
	upgrade->write_fd = -1;

	return;
}

int emmc_get_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *size)
{
	struct emmc_part_t *emmc_part;

	emmc_part = emmc_get_emmc_part_t(image_num);
	if (!emmc_part || !emmc_part->inited) {
		*size = 0;
		return SUCCESS;
	}

	emmc_part->fd = open(emmc_part->dev_path, (O_RDWR | O_SYNC | O_DIRECT));
    if (emmc_part->fd < 0) {
        printf("Can not open device %s %s \n", emmc_part->dev_path, strerror(errno));
        *size = 0;
        return SUCCESS;
    }
    upgrade->write_fd = emmc_part->fd;
	*size = emmc_part->size;
	printf("emmc get %s part %s offset:%llx size:%lld \n", emmc_part->dev_path, image_name[image_num], emmc_part->offset, *size);

	return SUCCESS;
}

int emmc_read_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data)
{
    int ret;
    int read_size = 0;
	struct emmc_part_t *emmc_part;

	emmc_part = emmc_get_emmc_part_t(image_num);
	if (!emmc_part || !emmc_part->inited)
		return FAILED;

    ret = lseek(emmc_part->fd, emmc_part->offset + offset, SEEK_SET);
    if (ret < 0) {
        printf("lseek %lld offset failed with %s.\n", offset, strerror(errno));
        return FAILED;
    }

	while(read_size < size) {
		ret = read(emmc_part->fd, data + read_size, size - read_size);
		//printf("emmc read part %x %d %d\n", offset, ret, emmc_part->fd);
		if (ret < 0) {
			LOGE("write emmc failed with %s.\n", strerror(errno));
			return FAILED;
		}
		read_size += ret;
	}

	return read_size;
}

int emmc_write_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data)
{
    int ret;
    int write_size = 0;
	struct emmc_part_t *emmc_part;

	emmc_part = emmc_get_emmc_part_t(image_num);
	if (!emmc_part || !emmc_part->inited)
		return FAILED;

    ret = lseek(emmc_part->fd, emmc_part->offset + offset, SEEK_SET);
    if (ret < 0) {
        printf("lseek %lld offset failed with %s.\n", offset, strerror(errno));
        return FAILED;
    }

	while(write_size < size) {
		ret = write(emmc_part->fd, data + write_size, size - write_size);
		if (ret < 0) {
			LOGE("write emmc failed with %s.\n", strerror(errno));
			return FAILED;
		}
		write_size += ret;
	}
	//printf("emmc write image %d %d %x \n", image_num, size, emmc_part->offset + offset);

	return write_size;
}

