#include <unistd.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <qsdk/sys.h>
#include "upgrade.h"
#include "ius_util.h"

#define DATA_BUF_SIZE		256*1024
#ifndef PAGE_SIZE
#define PAGE_SIZE 			4096
#endif

extern int spiblk_init(struct upgrade_t *upgrade);
extern void spiblk_deinit(struct upgrade_t *upgrade);
extern void spiblk_release_part(struct upgrade_t *upgrade, image_type_t image_num);
extern int spiblk_get_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *size);
extern int spiblk_read_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data);
extern int spiblk_write_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data);

extern int emmc_init(struct upgrade_t *upgrade);
extern void emmc_deinit(struct upgrade_t *upgrade);
extern void emmc_release_part(struct upgrade_t *upgrade, image_type_t image_num);
extern int emmc_get_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *size);
extern int emmc_read_part(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data);
extern int emmc_write_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data);

static int get_boot_flag(void)
{
    int len = 0;
    int fd;
    char cmd[192];
    int ret;

    fd = open("/proc/cmdline", O_RDONLY);
    if (fd < 0) {
        // TO-DO: implement LOGE, LOGD, LOGI
        LOGE("Open /proc/cmdline failed with %s.\n", strerror(errno));
        return FAILED;
    }
    while((ret = read(fd, cmd + len, sizeof(cmd)))) {
        if (ret == -1) {
            LOGE("Read /proc/cmdline failed with %s.\n", strerror(errno));
            return FAILED;
        }
        len += ret;
        if (len >= sizeof(cmd)) {
            break;
        }
    }
    for (ret = 0; ret < len; ret++) {
        if (cmd[ret] != ' ')
        	continue;
        if (*(int *)(cmd+ret+1) == 'edom') {
            return(*(int*)(cmd+ret+5));
        }
    }

    return FAILED;
}

static void upgrade_state_callback_common(void *arg, int image_num, int state, int state_arg)
{
	struct upgrade_t *upgrade = (struct upgrade_t *)arg;
	int cost_time;

	switch (state) {
		case UPGRADE_START:
			gettimeofday(&upgrade->start_time, NULL);
			printf("upgrade start \n");
			if (system_update_check_flag() == UPGRADE_IUS)
				system_update_clear_flag_all();
			break;
		case UPGRADE_COMPLETE:
			gettimeofday(&upgrade->end_time, NULL);
			cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
			printf("upgrade sucess time is %ds \n", cost_time);
			if (system_update_check_flag() == UPGRADE_OTA)
					system_update_clear_flag(UPGRADE_FLAGS_OTA);

			printf("BURN SUCCESS, Remove ius card to enter the new system.\n");
			
			//for upgrade from usb ,not check sd card
			if (!strcmp(upgrade->src_dev_path, "/dev/fastboot_download"))
				break;

			while(!access(upgrade->src_dev_path, F_OK));
			break;
		case UPGRADE_START_WRITE:
			system_update_clear_flag(image_num);
			break;
		case UPGRADE_WRITE_STEP:
			printf("\r %s image -- %d %% complete", image_name[image_num], state_arg);
			fflush(stdout);
			break;
		case UPGRADE_VERIFY_STEP:
			if (state_arg)
				printf(" %s verify failed %x \n", image_name[image_num], state_arg);
			else {
				system_update_set_flag(image_num);
				printf(" %s verify success\n", image_name[image_num]);
			}
			break;
		case UPGRADE_FAILED:
			printf("upgrade %s failed %d \n", image_name[image_num], state_arg);
			break;
	}

	return;
}

static int upgrade_image(struct upgrade_t *upgrade, image_type_t image_num)
{
	int ret = 0;
	uint64_t unit_size, part_size = 0, write_size, image_size;
	uint64_t offset;
	char *data = NULL, *very_buf = NULL;

	image_size = 0;
	ret = upgrade->get_image(upgrade, image_num, &offset, &image_size);
	if (ret || (image_size == 0))
		goto error2;
	
	upgrade->state_cb(upgrade->priv, image_num, UPGRADE_START_WRITE, 0);

	ret = upgrade->get_part(upgrade, image_num, &part_size);
	if (ret || (image_size > part_size)) {
		printf("not enough size imagesize %llu partsize %llu \n", image_size, part_size);
		if (part_size > 0)
			ret = FAILED;
		goto error1;
	}

	ret = upgrade->check_image(upgrade, image_num);
	if (ret)
		goto error1;

	ret = posix_memalign((void **)&data, PAGE_SIZE, DATA_BUF_SIZE);
	ret = posix_memalign((void **)&very_buf, PAGE_SIZE, DATA_BUF_SIZE);
	if (ret) {
		ret = FAILED;
		goto error1;
	}

	offset = 0;
	unit_size = DATA_BUF_SIZE;
	write_size = image_size;
	do {

		unit_size = (write_size >= DATA_BUF_SIZE) ? DATA_BUF_SIZE : write_size;
		if (unit_size % 512) {
			unit_size = (unit_size + 511) / 512;
			unit_size *= 512;
		}
		ret = upgrade->read_image(upgrade, image_num, offset, unit_size, data);
		if ((ret < 0) || (ret != unit_size))
			break;

		ret = upgrade->write_image(upgrade, image_num, offset, unit_size, data);
		if ((ret < 0) || (ret != unit_size))
			break;
		offset += unit_size;
		write_size -= unit_size;

		upgrade->state_cb(upgrade->priv, image_num, UPGRADE_WRITE_STEP, (int)((offset * 100) / image_size));
	} while (offset < image_size);

	if (offset >= image_size) {
		upgrade->release_part(upgrade, image_num);
		upgrade->state_cb(upgrade->priv, image_num, UPGRADE_WRITE_STEP, 100);
		upgrade->get_part(upgrade, image_num, &part_size);
		offset = 0;
		unit_size = DATA_BUF_SIZE;
		write_size = image_size;
		do {

			unit_size = (write_size >= DATA_BUF_SIZE) ? DATA_BUF_SIZE : write_size;
			if (unit_size % 512) {
				unit_size = (unit_size + 511) / 512;
				unit_size *= 512;
			}
			upgrade->read_image(upgrade, image_num, offset, unit_size, data);
			upgrade->read_part(upgrade, image_num, offset, unit_size, very_buf);
			if (memcmp(very_buf, data, unit_size)) {
				ret = FAILED;
				upgrade->state_cb(upgrade->priv, image_num, UPGRADE_VERIFY_STEP, ret);
				break;
			}

			offset += unit_size;
			write_size -= unit_size;
		} while (offset < image_size);
	}

error1:
	upgrade->release_part(upgrade, image_num);
	
	if (offset >= image_size) {
			upgrade->state_cb(upgrade->priv, image_num, UPGRADE_VERIFY_STEP, 0);
			ret = SUCCESS;
	}
error2:
	upgrade->release_image(upgrade, image_num);
	if (data)
		free(data);
	if (very_buf)
		free(very_buf);

	sync();
	return ret;
}

int system_update_check_flag(void)
{
	int ret = UPGRADE_NULL;
	unsigned int sys_flag;

	ret = system_check_upgrade("upgrade_flag", "ota");
	if (!ret) {
		ret = UPGRADE_OTA;
	} else {
		sys_flag = get_boot_flag();
	    switch(sys_flag) {
			case 'sui=':
				ret = UPGRADE_IUS;
				break;
			default:
				break;
		}
	}

	return ret;
}

int system_update_clear_flag_all(void)
{
	system_clearall_upgrade();
	return 0;
}

int system_update_set_flag(upgrade_flags_t image_num)
{
	switch (image_num) {
		case UPGRADE_FLAGS_OTA:
			return system_set_upgrade("upgrade_flag", "ota");
		case UPGRADE_FLAGS_RAMDISK:
		case UPGRADE_FLAGS_KERNEL:  
		case UPGRADE_FLAGS_KERENL1: 
		case UPGRADE_FLAGS_RAMDISK1:
			return system_set_upgrade(image_name[image_num], "ready");
		default:
			//printf("Now do not support this flag: %d\n", image_num);
			break;
	}
	return -1;
}

int system_update_clear_flag(upgrade_flags_t image_num)
{
	switch (image_num) {
		case UPGRADE_FLAGS_OTA:
			return system_set_upgrade("upgrade_flag", "none");
		case UPGRADE_FLAGS_RAMDISK: 
		case UPGRADE_FLAGS_KERNEL: 
		case UPGRADE_FLAGS_KERENL1: 
		case UPGRADE_FLAGS_RAMDISK1:
			return system_set_upgrade(image_name[image_num], "none");
		default:
			//printf("Now do not support this flag: %d\n", image_num);
			break;
	}
	return -1;
}

int system_update_upgrade(char *path, void (*state_cb)(void *arg, int image_type, int state, int state_arg), void *arg)
{
	unsigned int flag;
	int ret = 0, clear_flag = 0;
	int image_num = 0;

    struct upgrade_t *upgrade = (struct upgrade_t*)malloc(sizeof(struct upgrade_t));
    if (upgrade == NULL)
        return FAILED;

	if (arg != NULL)
		upgrade->priv = arg;
	else
		upgrade->priv = upgrade;
	upgrade->srcdev_init = ius_init;
	upgrade->srcdev_deinit = ius_deinit;
	upgrade->get_image = ius_get_image;
	upgrade->release_image = ius_release_image;
	upgrade->check_image = ius_check_image;
	upgrade->read_image = ius_read_image;

	if (item_exist("board.disk")) {
		if (item_equal("board.disk", "emmc", 0)) {
			upgrade->strogedev_init = emmc_init;
			upgrade->strogedev_deinit = emmc_deinit;
			upgrade->get_part = emmc_get_part;
			upgrade->read_part = emmc_read_part;
			upgrade->release_part = emmc_release_part;
			upgrade->write_image = emmc_write_image;
			printf("storage dev emmc \n");
		} else {
			upgrade->strogedev_init = spiblk_init;
			upgrade->strogedev_deinit = spiblk_deinit;
			upgrade->get_part = spiblk_get_part;
			upgrade->read_part = spiblk_read_part;
			upgrade->release_part = spiblk_release_part;
			upgrade->write_image = spiblk_write_image;
			printf("storage dev spi flash \n");
		}
	}
	if (state_cb) {
		upgrade->state_cb = state_cb;
	} else {
		upgrade->state_cb = upgrade_state_callback_common;
	}

	upgrade->state_cb(upgrade->priv, image_num, UPGRADE_START, 0);
	ret = upgrade->srcdev_init(upgrade, path);
	if (ret)
		goto error2;
	if (path == NULL)
		clear_flag = 1;
	ret = upgrade->strogedev_init(upgrade, clear_flag);
	if (ret)
		goto error1;

	while (image_num <= IMAGE_NONE) {
		ret = upgrade_image(upgrade, image_num);
		if (ret) {
			image_num++;
			continue;
		}
		image_num++;
	}

error1:
	upgrade->strogedev_deinit(upgrade);
error2:
	upgrade->srcdev_deinit(upgrade);
	if (ret)
		upgrade->state_cb(upgrade->priv, image_num, UPGRADE_FAILED, ret);
	else
		upgrade->state_cb(upgrade->priv, image_num, UPGRADE_COMPLETE, ret);
	if (upgrade)
		free(upgrade);

	return ret;
}

