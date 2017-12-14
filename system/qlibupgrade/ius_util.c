#include "ius_util.h"
#include "upgrade.h"
#include <zlib.h>

static struct ius_t ius_util;

static int ius_check_header(struct ius_hdr* hdr)
{
    uint32_t crc_orig, crc_calc;
    if (hdr->magic != IUS_MAGIC) {
        LOGE("%s", "Check magic of ius failed.\n");
        return FAILED;
    }
    crc_orig = hdr->hcrc;
    hdr->hcrc = 0;
    crc_calc = crc32(0L, (void *)hdr, 512);
    if (crc_calc != crc_orig) {
        LOGE("CRC check failed. orig: %x, cal: %x\n", crc_orig, crc_calc);
        return FAILED;
    }
    return SUCCESS;
}

int ius_init(struct upgrade_t *upgrade, char *ius_path)
{
    int i = 0, fd = -1;
    int ret = 0;
	char path[64];
	struct ius_t *ius;

	ius = &ius_util;
	memset(ius, 0, sizeof(struct ius_t));
	if (ius_path != NULL) {
		sprintf(path, "%s", ius_path);
		fd = open(path, O_RDONLY);
		if(fd < 0) {
			printf("%s open failed with %s.\n", ius_path, strerror(errno));
			return fd;
		}
		ret = read(fd, (void *)&(ius->header), sizeof(struct ius_hdr));
		if (ret < 0) {
			printf("%s path read header failed with %s.\n", path, strerror(errno));
			close(fd);
		} else {
			ret = ius_check_header(&(ius->header));
			if(ret == FAILED) {
				printf("%s path read header check failed with %s.\n", path, strerror(errno));
				close(fd);
			} else {
				strcpy(ius->dev_path, path);
				strcpy(upgrade->src_dev_path, path);
				close(fd);
				ius->inited = 1;
				ius->is_file = 1;
				printf("find ius file %s \n", ius->dev_path);
			}
		}
	} else {
		if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0))
			i = 1;
		for (; i<4; i++) {
			sprintf(path, "/dev/mmcblk%d", i);
			if(access(path, F_OK))
				continue;
			fd = open(path, O_RDONLY);
			if(fd < 0) {
	            printf("%s exists but open failed with %s.\n", path, strerror(errno));
				continue;
	        }
			ret = lseek(fd, IUS_OFFSET, SEEK_SET);
			if (ret < 0) {
				printf("%s lseek failed with %s.\n", path, strerror(errno));
				close(fd);
				continue;
			}
			if (ret < IUS_OFFSET) {
				printf("%s open success but can't seek to %d.\n", path, IUS_OFFSET);
				close(fd);
				continue;
			}
			ret = read(fd, (void *)&(ius->header), sizeof(struct ius_hdr));
			if (ret < 0) {
				printf("%s path read header failed with %s.\n", path, strerror(errno));
				close(fd);
				continue;
			}
			ret = ius_check_header(&(ius->header));
			if(ret == FAILED) {
				printf("%s path read header check failed with %s.\n", path, strerror(errno));
				close(fd);
				continue;
			} else {
				strcpy(ius->dev_path, path);
				strcpy(upgrade->src_dev_path, path);
				close(fd);
				ius->inited = 1;
				printf("find ius card %s \n", ius->dev_path);
			}
			break;
		}
	}
	if (ius->inited)
		ret = 0;
	else
		ret = FAILED;

	return ret;
}

void ius_deinit(struct upgrade_t *upgrade)
{
	return;
}

int ius_get_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *offset, uint64_t *size)
{
	struct ius_t *ius;
	struct ius_hdr *ius_hdr;
	struct ius_desc *desc;
	int i, count, type;

	ius = &ius_util;
	ius_hdr = &(ius->header);
	*size = 0;
	*offset = 0;

	if (!ius->inited)
		return FAILED;

	ius->fd = open(ius->dev_path, O_RDONLY);
	if(ius->fd < 0) {
		printf("ius dev %s open failed %s.\n", ius->dev_path, strerror(errno));
		return FAILED;
	}
	upgrade->read_fd = ius->fd;

	count = ius_get_count(ius_hdr);
	for (i=0; i<count; i++) {
		desc = ius_get_desc(ius_hdr, i);
		desc = ius_hdr->data + i;
		type = ius_desc_offo(desc);
		if (type == (int)image_num) {
			*offset = ius_desc_offi(desc);
			upgrade->read_offset = *offset;
			if (ius->is_file)
				upgrade->read_offset -= IUS_OFFSET;
			*size = ius_desc_length(desc);
			if (*size > 0)
				break;
		}
	}
	printf("ius get image %s szie:%d \n", image_name[image_num], *size);

	return 0;
}

void ius_release_image(struct upgrade_t *upgrade, image_type_t image_num)
{
	struct ius_t *ius;

	ius = &ius_util;
	if (ius->inited && (upgrade->read_fd > 0)) {
		close(upgrade->read_fd);
		upgrade->read_fd = -1;
		upgrade->read_offset = 0;
		ius->fd = -1;
	}

	return;
}

int ius_check_image(struct upgrade_t *upgrade, image_type_t image_num)
{
	return 0;
}

int ius_read_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data)
{
    int ret;
    uint64_t read_size = 0;

	ret = lseek(upgrade->read_fd, upgrade->read_offset + offset, SEEK_SET);
	if (ret < 0) {
		LOGE("lseek ius card to offset %llx failed.\n", upgrade->read_offset + offset);
		return FAILED;
	}

	while (read_size < size) {
		ret = read(upgrade->read_fd, data + read_size, size - read_size);
		if (ret< 0) {
			LOGE("read ius card failed with %s.\n", strerror(errno));
			return FAILED;
		}
		read_size += ret;
	}
	//printf("ius read image %d %d %x \n", image_num, size, upgrade->read_offset + offset);

	return read_size;
}
