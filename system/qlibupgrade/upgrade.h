/* dlimage.h
 *
 * Image type handler ops and flash type handler ops defination, download
 * image flow control and UI output functions.
 *
 * Copyright © 2014 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 * 1.0  Created.
 *      xecle, 03/15/2014 03:02:06 PM
 *
 */

#ifndef _UPGRADE_H_
#define _UPGRADE_H_

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#define __USE_GNU 1
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/reboot.h>

#ifndef TAG
#define TAG "upgrade"
#endif

#define LOGE(fmt, arg...)   printf("ERROR: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#ifdef SHOW_WARNING
#define LOGW(fmt, arg...)   printf("WARNING: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define LOGW while(0);
#endif
#ifdef DEBUG
#define LOGD(fmt, arg...)   printf("DEBUG: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#define LOGI(fmt, arg...)   printf("INFOR: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define LOGD while(0);
#define LOGI while(0);
#endif

#define SZ_1K	0x400
#define BL_DEFAULT_LOCATION		0x1000000
#define FDISK_UNIT_SIZE		32768

enum {
    FAILED = -1,
    SUCCESS
};

typedef enum {
	UPGRADE_START = 0,
	UPGRADE_START_WRITE = 1,
	UPGRADE_WRITE_STEP = 2,
	UPGRADE_VERIFY_STEP = 3,
	UPGRADE_FAILED = 4,
	UPGRADE_COMPLETE = 5
} upgrade_state_t;

typedef enum {
	IMAGE_UBOOT = 0,
	IMAGE_ITEM = 1,
	IMAGE_RAMDISK = 2,
	IMAGE_KERNEL = 3,
	IMAGE_KERNEL_UPGRADE = 4,
	IMAGE_SYSTEM = 5,
	IMAGE_RAMDISK_UPGRADE = 6,
	IMAGE_UBOOT1 = 16,
	IMAGE_RESERVED = 17,
	IMAGE_NONE = 18,
}image_type_t;

typedef enum {
	UPGRADE_NULL = 0,
	UPGRADE_IUS = 1,
	UPGRADE_OTA = 2,
} upgrade_mode_t;

typedef enum {
	UPGRADE_FLAGS_OTA = -1,		// do not equal anyone in image_type_t
	UPGRADE_FLAGS_RAMDISK = IMAGE_RAMDISK,
	UPGRADE_FLAGS_KERNEL = IMAGE_KERNEL,
	UPGRADE_FLAGS_KERENL1 = IMAGE_KERNEL_UPGRADE,
	UPGRADE_FLAGS_RAMDISK1 = IMAGE_RAMDISK_UPGRADE,
	UPGRADE_FLAGS_NONE,
} upgrade_flags_t;

static const char *image_name[] = {
	"uboot", 
    "item", 
    "ramdisk", 
    "kernel", 
    "kernel1", 
    "system" ,
    "ramdisk1",
    "none",
    "none",
    "none",
    "none",
    "none",
    "none",
    "none",
    "none",
    "none",
    "none",
    "uboot1",
    "reserved",
    "none",
    "none",
    "none",
    "none"
};

struct upgrade_t;

struct upgrade_t {
	char src_dev_path[32];
	int read_fd;
	int write_fd;
	uint64_t read_offset;
	struct timeval start_time;
	struct timeval end_time;
	void *priv;
	int (*srcdev_init)(struct upgrade_t *upgrade, char *path);
	int (*strogedev_init)(struct upgrade_t *upgrade, int clear_flag);
	void (*srcdev_deinit)(struct upgrade_t *upgrade);
	void (*strogedev_deinit)(struct upgrade_t *upgrade);
	int (*get_image)(struct upgrade_t *upgrade, image_type_t image_type, uint64_t *offset, uint64_t *size);
	void (*release_image)(struct upgrade_t *upgrade, image_type_t image_type);
	int (*get_part)(struct upgrade_t *upgrade, image_type_t image_type, uint64_t *size);
	int (*read_part)(struct upgrade_t *upgrade, image_type_t image_type, uint64_t offset, uint64_t size, char *data);
	void (*release_part)(struct upgrade_t *upgrade, image_type_t image_type);
	int (*check_image)(struct upgrade_t *upgrade, image_type_t image_type);
	int (*read_image)(struct upgrade_t *upgrade, image_type_t image_type, uint64_t offset, uint64_t size, char *data);
	int (*write_image)(struct upgrade_t *upgrade, image_type_t image_type, uint64_t offset, uint64_t size, char *data);
	void (*state_cb)(void *arg, int image_type, int state, int state_arg);
};

/*
	path: ius file path such as /mnt/mmc/update.ius
	state_cb: callback of upgrade state 
		image_type is current upgrade image such as IMAGE_KERNEL
		state is current state UPGRADE_START UPGRADE_FAILED and so on
		state_arg arg of state such as percent of wirted image
	void *arg: just send back pointer to uplayer
*/
int system_update_upgrade(char *path, void (*state_cb)(void *arg, int image_type, int state, int state_arg), void *arg);

/*
	check update flag
	if return < 0 check failed
	others return upgrade_mode_t enum now just ota and ius
*/
int system_update_check_flag(void);

/* clear update all flags 
	 return 0 if success
	 other failed
*/
int system_update_clear_flag_all(void);

/*
	for set flags about whether ramdisk ramdisk1 kernel kernel1 ready or not
	return 0 if ready
	other failed
*/
int system_update_set_flag(upgrade_flags_t image_num);

/*
	clear flags about whether ramdisk ramdisk1 kernel kernel1 ready or not
	return 0  if success
	other failed
*/
int system_update_clear_flag(upgrade_flags_t image_num);

#endif
