#ifndef __INFOTM_BRIDGE_H
#define __INFOTM_BRIDGE_H

#include "../sensors/items_utils.h"
#include "../sensors/power_up.h"
#include <linux/list.h>

#define B_LOG_DEBUG 0
#define B_LOG_INFO 1
#define B_LOG_WARN 2
#define B_LOG_ERROR 3

extern int bridge_log_level;

#define bridge_log(level,fmt, arg...) \
	do { \
		if (level >= bridge_log_level) \
			printk("[bridge] %s:"fmt, __func__, ##arg); \
	}while(0)

struct _infotm_bridge {
	char name[MAX_STR_LEN];
	int sensor_num;
	int exist;
	struct sensor_info* info;
	struct sensor_info* sensor_info[MAX_BRIDGE_SENSORS_NUM];

	struct infotm_bridge_dev *dev;
};

struct infotm_bridge_dev {
	char name[MAX_STR_LEN];
	int (*control)(unsigned int ,unsigned int );
	int (*init)(struct _infotm_bridge*, unsigned int );
	int(*exit)(unsigned int );

	struct list_head list;
};

struct _bridge_sensor_idx {
	int bridge_id;
	int sensor_id;
	int idx;
};

struct bridge_init_config {
	void* arg;
	int len;
};

struct bsensor_uinfo {
	char name[MAX_STR_LEN];
	int i2c_addr;
	int chn;
	int exist;
	int mipi_lanes;
	/* char config[128]; */
};

struct bridge_uinfo {
	char name[MAX_STR_LEN];
	int i2c_addr;
	int chn;
	int imager;
	int mipi_lanes;
	int sensor_num;
	int i2c_exist;
	struct bsensor_uinfo bsinfo[MAX_BRIDGE_SENSORS_NUM];
};

#define XC9080_USER_INIT_MODE 0
/*Note: xc9080 will used kernel space i2c setting and will speed less time*/
#define XC9080_KERNAL_INIT_MODE 1

struct bridge_hw_cfg {
	int mode;
	int enable;
};

#define BRIDGE_CTRL_MAGIC	('B')

#define BRIDGE_G_INFO	_IOW(BRIDGE_CTRL_MAGIC, 0x31, struct bridge_uinfo)
#define BRIDGE_G_CFG	_IOW(BRIDGE_CTRL_MAGIC, 0x32, unsigned int)
#define BRIDGE_S_CFG	_IOW(BRIDGE_CTRL_MAGIC, 0x33, unsigned int)

extern int infotm_bridge_init(struct sensor_info *pinfo);
extern void infotm_bridge_exit(void);
extern int infotm_bridge_check_exist(void);

extern int bridge_dev_register(struct infotm_bridge_dev* dev);
extern int bridge_dev_unregister(struct infotm_bridge_dev *udev);

#endif
