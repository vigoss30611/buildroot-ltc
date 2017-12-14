#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <sys/vfs.h>
#include <qsdk/sys.h>
#include "upgrade.h"
 
#define SPI_BLK "/dev/spiblock"
#define SPI_SECT_SIZE	512
#define SPI_START_SECT	63

struct spiblk_part_t {
    int fd;
    int inited;
    uint64_t offset;
    uint64_t size;
    char part_name[16];
    char fs_type[16];
    char dev_path[32];
};

static struct spiblk_part_t spiblk_part[IMAGE_NONE];

void spiblk_creat_partitions(int part_num)
{
	struct spiblk_part_t *spi_part;
	char cmd[512], temp[64];
	int size = SPI_START_SECT * SPI_SECT_SIZE;
	int start_sect, end_sect, i;

	sprintf(cmd, "fdisk /dev/spiblock1 << EOF \n d \n 1 \n d \n 2 \n d \n 3 \n d \n 4 \n u \n");
	for (i=0; i<part_num; i++) {
		spi_part = &spiblk_part[IMAGE_SYSTEM + i];
		start_sect = (int)(size / SPI_SECT_SIZE);
		end_sect = (int)(spi_part->size / SPI_SECT_SIZE);
		sprintf(temp, " n \n p \n %d \n %d \n %d \n ", i+1, (start_sect + 1), (start_sect + end_sect));
		strcat(cmd, temp);
		size += spi_part->size;
	}
	sprintf(temp, "w \n EOF \n");
	strcat(cmd, temp);

	printf("%s \n", cmd);
	system(cmd);

	system("mdev -s");
	for (i=0; i<part_num; i++) {
		spi_part = &spiblk_part[IMAGE_SYSTEM + i];
		if (!strncmp(spi_part->fs_type, "ext4", strlen("ext4"))) {
			sprintf(cmd, "mke2fs -T ext4 -L %s %s", spi_part->part_name, spi_part->dev_path);
			system(cmd);
		} else if (!strncmp(spi_part->fs_type, "fat", strlen("fat"))) {
			sprintf(cmd, "mkfs.vfat -n %s %s", spi_part->part_name, spi_part->dev_path);
			system(cmd);
		}
	}

	return;
}

int spiblk_check_part_info(struct spiblk_part_t *spi_part)
{
    struct statfs diskinfo;
    int fd, ret;
    uint64_t size;

	if (access(spi_part->dev_path, F_OK))
		return -1;

	fd = open(spi_part->dev_path, O_RDWR);
	if (fd < 0) {
		printf("spiblk check open failed %s \n", spi_part->dev_path);
		return FAILED;
	}

	if ((ret = ioctl(fd, BLKGETSIZE64, &size)) < 0) {
        printf("ioctl error %s \n", strerror(errno));
        return FAILED;
    }
    close(fd);

	printf("spiblk check part %lld %lld \n", size, spi_part->size);
	if (size != spi_part->size)
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

struct spiblk_part_t *spiblk_get_spi_part_t(image_type_t image_num)
{
	struct spiblk_part_t *spi_part = NULL;
	int i;

	for (i = 0; i < IMAGE_NONE; i++) {
		spi_part = &spiblk_part[i];
		if (!spi_part->inited)
			continue;
#if 0
		printf("spi [%d] init part_name %s, path %s, size %d\n", i,
				spi_part->part_name, spi_part->dev_path, spi_part->size);
#endif
		if (!strncmp(spi_part->part_name, image_name[image_num],
				max(strlen(spi_part->part_name), strlen(image_name[image_num]))))
			break;
	}
	if (i == IMAGE_NONE)
		return NULL;

	return &spiblk_part[i];
}

void spiblk_format_dev(struct spiblk_part_t *spi_part)
{
	char cmd[512];

	if (!strncmp(spi_part->fs_type, "ext4", strlen("ext4")) && (access("/sbin/mke2fs", F_OK) == 0)) {
		sprintf(cmd, "mke2fs -T ext4 -L %s %s", spi_part->part_name, spi_part->dev_path);
		system(cmd);
	} else if (!strncmp(spi_part->fs_type, "fat", strlen("fat"))) {
		sprintf(cmd, "mkfs.vfat -n %s %s", spi_part->part_name, spi_part->dev_path);
		system(cmd);
	}
}

int spiblk_init(struct upgrade_t *upgrade, int clear_flag)
{
	struct spiblk_part_t *spi_part;
	char spi_parts[64];
	char part_name[32];
	char part_type[64];
	char fs_type[16];
	int i = 0, index = 0, fs_part_num = 0, creat_part = 0;
	uint64_t offset = 0, offset_normal = 0, offset_fs = 0, size = 0;
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
				spi_part = &spiblk_part[i];
	            spi_part->size = SZ_1K;
	            spi_part->size *= part_size;
	            spi_part->offset = offset;
	            spi_part->inited = 1;
	            offset += spi_part->size;
	            //after item reserve UPGRADE_PARA_SIZE for upgrade
	            if (!strncmp(part_name, "item", strlen((const char*)"item")))
	            	offset += (UPGRADE_PARA_SIZE + RESERVED_ENV_SIZE);
	            sprintf(spi_part->dev_path, "%s0", SPI_BLK);
			sprintf(spi_part->part_name, "%s", part_name);
			} else if (!strncmp(part_type, "normal", strlen((const char*)"normal"))) {
				spi_part = &spiblk_part[i];
				index++;
				sprintf(spi_part->dev_path, "%s%d", SPI_BLK, index);
				if (access(spi_part->dev_path, F_OK) == 0) {
					fd = open(spi_part->dev_path, O_RDWR);
					if (fd < 0) {
						printf("spiblk init open failed \n");
						return FAILED;
					}

					if ((ret = ioctl(fd, BLKGETSIZE64, &size)) < 0) {
				        printf("ioctl error %s \n", strerror(errno));
				        return FAILED;
				    }
				    close(fd);
				}

				spi_part->size = size;
				spi_part->offset = 0;
				spi_part->inited = 1;
				sprintf(spi_part->fs_type, "%s", fs_type);
				sprintf(spi_part->part_name, "%s", part_name);
				printf("spiblk init %s %s %lld \n", spi_part->dev_path, spi_part->fs_type, spi_part->size);
			} else if (!strncmp(part_type, "fs", strlen((const char*)"fs"))) {

				spi_part = &spiblk_part[i];
				index++;
				sprintf(spi_part->dev_path, "%s%d", SPI_BLK, index);
				if (access(spi_part->dev_path, F_OK) == 0) {
					fd = open(spi_part->dev_path, O_RDWR);
					if (fd < 0) {
						printf("spiblk init open failed \n");
						return FAILED;
					}

					if ((ret = ioctl(fd, BLKGETSIZE64, &size)) < 0) {
				        printf("ioctl error %s \n", strerror(errno));
				        return FAILED;
				    }
				    close(fd);
				}

				spi_part->size = size;
				spi_part->offset = 0;
				spi_part->inited = 1;
				sprintf(spi_part->fs_type, "%s", fs_type);
				sprintf(spi_part->part_name, "%s", part_name);
				printf("spiblk init %s %s %lld \n", spi_part->dev_path, spi_part->fs_type, spi_part->size);
				if (clear_flag || spiblk_check_part_info(spi_part))
					spiblk_format_dev(spi_part);
			}
		}
	}
	if (creat_part)
		spiblk_creat_partitions(fs_part_num);
#if 0	
		for (i = 0; i < IMAGE_NONE; i++) {
			spi_part = &spiblk_part[i];
			if (!spi_part->inited)
				continue;
			printf("spi [%d] init part_name %s, path %s, size %d\n", i,
						spi_part->part_name, spi_part->dev_path, spi_part->size);
		}
#endif
    return SUCCESS;
}

void spiblk_deinit(struct upgrade_t *upgrade)
{
	return;
}

void spiblk_release_part(struct upgrade_t *upgrade, image_type_t image_num)
{
	struct spiblk_part_t *spi_part;

	spi_part = spiblk_get_spi_part_t(image_num);
	if (!spi_part || !spi_part->inited)
		return;

	close(spi_part->fd);
	upgrade->write_fd = -1;

	return;
}

int spiblk_get_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *size)
{
	struct spiblk_part_t *spi_part;
	
	spi_part = spiblk_get_spi_part_t(image_num);
	if (!spi_part || !spi_part->inited) {
		*size = 0;
		return SUCCESS;
	}

	spi_part->fd = open(spi_part->dev_path, (O_RDWR | O_SYNC | O_DIRECT));
    if (spi_part->fd < 0) {
        printf("Can not open device %s\n", spi_part->dev_path);
        *size = 0;
        return SUCCESS;
    }
    upgrade->write_fd = spi_part->fd;
	*size = spi_part->size;
	printf("spiblk get %s part %s %d offset:%llx size:%llx \n", spi_part->dev_path, image_name[image_num], spi_part->fd, spi_part->offset, *size);

	return SUCCESS;
}

int spiblk_read_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data)
{
    int ret;
    int read_size = 0;
	struct spiblk_part_t *spi_part;
	
	spi_part = spiblk_get_spi_part_t(image_num);
	if (!spi_part || !spi_part->inited)
		return FAILED;

    ret = lseek(spi_part->fd, spi_part->offset + offset, SEEK_SET);
    if (ret < 0) {
        printf("lseek %ld offset failed with %s.\n", offset, strerror(errno));
        return FAILED;
    }

	while(read_size < size) {
		ret = read(spi_part->fd, data + read_size, size - read_size);
		//printf("spiblk read part %x %d %d\n", offset, ret, spi_part->fd);
		if (ret < 0) {
			LOGE("read spiblk failed with %s.\n", strerror(errno));
			return FAILED;
		}
		read_size += ret;
	}

	return read_size;
}

int spiblk_write_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data)
{
    int ret;
    int write_size = 0;
	struct spiblk_part_t *spi_part;
	
	spi_part = spiblk_get_spi_part_t(image_num);
	if (!spi_part || !spi_part->inited)
		return FAILED;

    ret = lseek(spi_part->fd, spi_part->offset + offset, SEEK_SET);
    if (ret < 0) {
        printf(" %s %d lseek %lld offset failed with %s.\n", image_name[image_num], spi_part->fd, spi_part->offset + offset, strerror(errno));
        return FAILED;
    }

	while(write_size < size) {
		ret = write(spi_part->fd, data + write_size, size - write_size);
		if (ret < 0) {
			LOGE("write spiblk failed with %s.\n", strerror(errno));
			return FAILED;
		}
		write_size += ret;
	}
	//printf("spiblk write image %d %d %x \n", image_num, size, spi_part->offset + offset);

	return write_size;
}

