/***************************************************************************** 
 ** 
 ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 ** 
 ** Use of Infotm's code is governed by terms and conditions 
 ** stated in the accompanying licensing statement. 
 ** 
 ** image sensor  driver file
 **
 ** Revision History: 
 ** -----------------
 ** Davinci.Liang
 *****************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <mach/pad.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/ddk_sensor_driver.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <mach/items.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/errno.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include "bridge.h"

int bridge_log_level = B_LOG_INFO;

#define ALL_SENSOR_NUM ((MAX_SENSORS_NUM)*(MAX_BRIDGE_SENSORS_NUM))

static struct i2c_driver bridge_sensor_driver[ALL_SENSOR_NUM];
static struct i2c_device_id bridge_sensor_id[ALL_SENSOR_NUM];
static struct sensor_info bridge_sensor_info[ALL_SENSOR_NUM];

static struct _bridge_sensor_idx bridge_sensor_idx[ALL_SENSOR_NUM];
static struct _infotm_bridge infotm_bridge[MAX_SENSORS_NUM] = {{"none", 0, 0, 0}};
static struct miscdevice bridge_dev;
static struct bridge_uinfo user_info[MAX_SENSORS_NUM];

LIST_HEAD(bridge_dev_list);
static DEFINE_SPINLOCK(bridge_dev_list_lock);

//probe is core work of the ddk sensor i2c driver.
static int bridge_sensor_probe(struct i2c_client *client,
	const struct i2c_device_id *dev_id)
{
	int ret = 0, idx = 0, sensor_id = 0, bridge_id = 0;

	struct _bridge_sensor_idx* bridge_sensor_idx;
	bridge_sensor_idx = (struct _bridge_sensor_idx*)(client->dev.platform_data);

	sensor_id = bridge_sensor_idx->sensor_id;
	bridge_id = bridge_sensor_idx->bridge_id;
	idx = bridge_sensor_idx->idx;
	bridge_log(B_LOG_DEBUG,"idx = %d sensor_id = %d bridge_id=%d\n",
							idx, sensor_id, bridge_id);

	if(!strcmp(bridge_sensor_info[sensor_id].interface, "mipi") &&
		bridge_sensor_info[sensor_id].lanes > 0) {
		csi_core_init(bridge_sensor_info[sensor_id].lanes,
		bridge_sensor_info[sensor_id].csi_freq);
		csi_core_open();
	}

	if (bridge_sensor_info[sensor_id].wraper_use) {
#ifdef CONFIG_ARCH_APOLLO3
		if (sensor_id == 0) {
			isp_dvp_wrapper_probe();
			isp_dvp_wrapper_init(0x02,
				bridge_sensor_info[sensor_id].wwidth,
				bridge_sensor_info[sensor_id].wheight,
				bridge_sensor_info[sensor_id].whdly,
				bridge_sensor_info[sensor_id].wvdly);
		} else if (sensor_id == 1) {
			isp_dvp_wrapper1_probe();
			isp_dvp_wrapper1_init(0x02,
				bridge_sensor_info[sensor_id].wwidth,
				bridge_sensor_info[sensor_id].wheight,
				bridge_sensor_info[sensor_id].whdly,
				bridge_sensor_info[sensor_id].wvdly);
		}
#endif
	}

	//power up sensor
	ret = bridge_sensor_power_up(&bridge_sensor_info, bridge_id, idx, 1);
	if(ret) {
		pr_err("in bridge densor probe sensor%d power up error\n", sensor_id);
		goto err1;
	}

	return 0;
err1:
	ret = bridge_sensor_power_up(&bridge_sensor_info, bridge_id, idx, 0);

	if(!strcmp(bridge_sensor_info[sensor_id].interface, "mipi")) 
		csi_core_close();
	return ret;
}

static int bridge_sensor_remove(struct i2c_client *client)
{
	int idx = 0, sensor_id = 0, bridge_id;
	struct _bridge_sensor_idx* bridge_sensor_idx;
	bridge_sensor_idx = (struct _bridge_sensor_idx*)(client->dev.platform_data);

	bridge_log(B_LOG_DEBUG, "sensor_id = %d bridge_id=%d\n",
		bridge_sensor_idx->sensor_id, bridge_sensor_idx->bridge_id);

	sensor_id = bridge_sensor_idx->sensor_id;
	bridge_id = bridge_sensor_idx->bridge_id;
	idx = bridge_sensor_idx->idx;

	bridge_sensor_power_up(&bridge_sensor_info, bridge_id, idx, 0);

	if(!strcmp(bridge_sensor_info[sensor_id].interface, "mipi") &&
		bridge_sensor_info[sensor_id].exist)
		csi_core_close();

	bridge_log(B_LOG_INFO, "bridge sensor driver removed and exit.\n");
	return 0;
}

static void bridge_sensor_exit(int curr_sensor_id)
{
	int i = 0;

	for (i = 0; i < curr_sensor_id; i++) {
		if (bridge_sensor_info[i].exist) {
			if(bridge_sensor_info[i].client)
				i2c_unregister_device(bridge_sensor_info[i].client);

			if(bridge_sensor_info[i].adapter)
				i2c_put_adapter(bridge_sensor_info[i].adapter);

			i2c_del_driver(&(bridge_sensor_driver[i]));
			kfree(bridge_sensor_driver[i].driver.name);
		}
	}
}

static int bridge_sensor_init(void)
{
	int ret = 0, i = 0, r = 0;
	struct i2c_board_info sensor_i2c_info[ALL_SENSOR_NUM];
	char tmp[MAX_STR_LEN] = {0};

	int sensor_id = 0;
	memset((void *)bridge_sensor_id, 0, sizeof(bridge_sensor_id));

	/* memset((void *)infotm_bridge, 0, sizeof(infotm_bridge[0])*MAX_SENSORS_NUM); */
	ddk_parse_ext_item();

	for (r = 0; r < MAX_SENSORS_NUM ; r++) {

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s", "sensor", r, "bridge","sensornum");
		if(item_exist(tmp))
			infotm_bridge[r].sensor_num = item_integer(tmp, 0);
		else
			infotm_bridge[r].sensor_num = 0;

		if (infotm_bridge[r].sensor_num > MAX_BRIDGE_SENSORS_NUM)
			infotm_bridge[r].sensor_num = MAX_BRIDGE_SENSORS_NUM;

		for(i = 0; i < infotm_bridge[r].sensor_num; i++) {
			memset((void *)tmp, 0, sizeof(tmp));
			sprintf(tmp, "%s%d.%d", "bridge", r, i);

			if (sensor_id > ALL_SENSOR_NUM)
				goto err;

			infotm_bridge[r].sensor_info[i] = &bridge_sensor_info[sensor_id];
			ret = get_bridge_sensor_board_info(&bridge_sensor_info[sensor_id], r, i);
			if(-2 == ret) {
				bridge_log(B_LOG_WARN, "bridge sensor%d can't create i2c device\n", i);

				sensor_id ++;
				continue;
			}

			if (0 == strncmp(bridge_sensor_info[sensor_id].mux, "cam", 3))
				imapx_pad_init("cam");
			else {
				if (!sensor_id)
					bridge_log(B_LOG_ERROR,\
					"bridge sensor%d don't init pad: cam\n", sensor_id);
			}

			if (!strcmp(bridge_sensor_info[sensor_id].interface,  "camif") ||
				!strcmp(bridge_sensor_info[sensor_id].interface,  "camif-csi"))
				continue;

			if (0 == strncmp(bridge_sensor_info[sensor_id].mux, "cam1-mux", 8)) {
				imapx_pad_init("cam1-mux");
			} else if (0 == strncmp(bridge_sensor_info[sensor_id].mux, "cam1", 4)) {
				imapx_pad_init("cam1");
			} else {
				if (sensor_id == 1)
					bridge_log(B_LOG_WARN,\
						"bridge sensor%d don't init pad: cam1\n", sensor_id);
			}

			memset((void *)sensor_i2c_info, 0, sizeof(sensor_i2c_info));
			bridge_sensor_driver[sensor_id].driver.name =
				(const char *)kzalloc(MAX_STR_LEN, GFP_KERNEL);

			if (bridge_sensor_driver[sensor_id].driver.name  == NULL)
				return -ENOMEM;

			strcpy(sensor_i2c_info[sensor_id].type, tmp);
			bridge_sensor_info[sensor_id].i2c_addr = \
				sensor_i2c_info[sensor_id].addr = bridge_sensor_info[i].i2c_addr >> 1;
			bridge_sensor_driver[sensor_id].driver.owner = THIS_MODULE;

			strcpy((char *)bridge_sensor_driver[sensor_id].driver.name, (const char *)tmp);
			bridge_sensor_driver[sensor_id].probe = bridge_sensor_probe;
			bridge_sensor_driver[sensor_id].remove = bridge_sensor_remove;
			bridge_sensor_driver[sensor_id].id_table = &bridge_sensor_id[i];

			bridge_log(B_LOG_INFO, "this i2c add = 0x%x, bridge_sensor_dev[i].name = %s\n",
				sensor_i2c_info[sensor_id].addr, bridge_sensor_driver[sensor_id].driver.name);

			bridge_log(B_LOG_INFO, "bridge_sensor_driver[i].driver.name = %s, "\
				"bridge_sensor_id[i].name=%s \n",\
				bridge_sensor_driver[sensor_id].driver.name,\
				bridge_sensor_id[sensor_id].name);

			bridge_sensor_info[sensor_id].adapter = i2c_get_adapter(bridge_sensor_info[sensor_id].chn);
			if(!bridge_sensor_info[sensor_id].adapter) {
				bridge_log(B_LOG_ERROR, "ddk sensor get i2c adapter failed.\n");
				return -ENODEV;
			}

			bridge_sensor_info[sensor_id].client = 
				i2c_new_device(bridge_sensor_info[sensor_id].adapter, &sensor_i2c_info[i]);
			if(!bridge_sensor_info[sensor_id].client) {
				bridge_log(B_LOG_ERROR, "ddk sensor add new client devide failed.\n");
				goto err1;
			}

			/* move here for id_table set value, because i2c_new_device may call probe function. */
			strncpy(bridge_sensor_id[sensor_id].name, tmp, I2C_NAME_SIZE);
			bridge_sensor_info[sensor_id].client->flags = I2C_CLIENT_SCCB;
			((bridge_sensor_info[sensor_id].client)->dev).platform_data = 
				kzalloc(sizeof(struct _bridge_sensor_idx), GFP_KERNEL);

			bridge_sensor_idx[sensor_id].bridge_id = r;
			bridge_sensor_idx[sensor_id].sensor_id = sensor_id;
			bridge_sensor_idx[sensor_id].idx = i;

			memcpy(((bridge_sensor_info[sensor_id].client)->dev).platform_data,
				&bridge_sensor_idx[sensor_id],
				sizeof(struct _bridge_sensor_idx));

			bridge_log(B_LOG_INFO, "add i2c devices & driver \n");
			ret = i2c_add_driver(&bridge_sensor_driver[sensor_id]);
			if(ret) {
				bridge_log(B_LOG_ERROR, "bridge sensor add i2c drvier fail.\n");
				goto err2;
			}

			sensor_id++;
		}
	}

	return 0;
err2:
	i2c_unregister_device(bridge_sensor_info[sensor_id].client);
err1:
	i2c_put_adapter(bridge_sensor_info[sensor_id].adapter);
err:
	bridge_sensor_exit(sensor_id);
	return -1;
}

static ssize_t bridge_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	bridge_log(B_LOG_DEBUG, "interface = %s\n",
		infotm_bridge[0].info->interface);

	if (strcmp(infotm_bridge[0].info->interface, "mipi") == 0)
		csi2_dump_regs();

	return sprintf(buf, "bridge=%s\n"
					"bridge sensor =%s\n"
					"bridge i2c exist=%d\n"
					"bridge sensor i2c exist=%d\n",
					infotm_bridge[0].name,
					infotm_bridge[0].sensor_info[0]->name,
					infotm_bridge[0].info->exist,
					infotm_bridge[0].sensor_info[0]->exist);
}

static ssize_t bridge_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	bridge_log_level = simple_strtol(buf, NULL, 10);
	printk("bridge_log_level = %d\n", bridge_log_level);
	return size;
}
DEVICE_ATTR(infotm_bridge_info, 0666, bridge_show, bridge_store);

int bridge_dev_register(struct infotm_bridge_dev *udev)
{
	struct infotm_bridge_dev *dev = NULL;
	if (!udev)
		return -1;

	spin_lock(&bridge_dev_list_lock);

	list_for_each_entry(dev, &bridge_dev_list, list)
		if(strcmp(dev->name, udev->name) == 0) {
			bridge_log(B_LOG_WARN, "the bridge %s has been registered\n", udev->name);
			spin_unlock(&bridge_dev_list_lock);
			return 0;
		}

	list_add_tail(&(udev->list), &bridge_dev_list);

	spin_unlock(&bridge_dev_list_lock);
	return 0;
}
EXPORT_SYMBOL(bridge_dev_register);

int bridge_dev_unregister(struct infotm_bridge_dev *udev)
{
	struct infotm_bridge_dev *dev = NULL;
	if (!udev)
		return -1;

	//TODO, will use lock to protect this list.

	spin_lock(&bridge_dev_list_lock);

	list_for_each_entry(dev, &bridge_dev_list, list)
		if(strcmp(dev->name, udev->name) == 0) {
			list_del(&dev->list);
			break;
		}

	spin_unlock(&bridge_dev_list_lock);
	return 0;
}
EXPORT_SYMBOL(bridge_dev_unregister);

struct infotm_bridge_dev* bridge_lookup_dev(char* name)
{
	struct infotm_bridge_dev *dev = NULL;
	list_for_each_entry(dev, &bridge_dev_list, list)
		if(dev&&strcmp(dev->name, name) == 0) {
			bridge_log(B_LOG_DEBUG, "dev name :%s\n", dev->name);
			return dev;
		}
	return NULL;
}

int infotm_bridge_check_exist(void)
{
	int r = 0;
	int c = 0;
	char tmp[MAX_STR_LEN] = {0};

	for (r = 0; r < MAX_SENSORS_NUM ; r++) {

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d.%s.%s", "sensor", r, "bridge","name");
		if(item_exist(tmp)) {
			item_string(infotm_bridge[r].name, tmp, 0);
			bridge_log(B_LOG_INFO, "bridge_name = %s\n", infotm_bridge[r].name);
			infotm_bridge[r].exist = 1;
			c++;
		}else {
			strncpy(infotm_bridge[r].name, "none", MAX_STR_LEN);
			infotm_bridge[r].exist = 0;
		}
	}

	return c;
}
EXPORT_SYMBOL(infotm_bridge_check_exist);

int infotm_bridge_init(struct sensor_info *pinfo)
{
	int i = 0, ret = 0;
	struct infotm_bridge_dev* dev = NULL;

	ret = bridge_sensor_init();
	if (ret < 0)
		goto err;

	for (i =0; i < MAX_SENSORS_NUM; i++) {
		if (!infotm_bridge[i].exist)
			continue;

		infotm_bridge[i].dev =bridge_lookup_dev(infotm_bridge[i].name);
		infotm_bridge[i].info = &pinfo[i];

		bridge_log(B_LOG_DEBUG, "sensor_info%d exist=%d name=%s "
					"interface=%s i2c_addr=0x%x chn=%d\n"
					"lanes=%d csi_freq=%d mipi_pixclk=%d "
					"mclk_src=%s i2c_client=%p i2c_adapter=%p reset_pin=%d\n",
					i, pinfo[i].exist, pinfo[i].name, pinfo[i].interface,
					pinfo[i].i2c_addr, pinfo[i].chn, pinfo[i].lanes,
					pinfo[i].csi_freq, pinfo[i].mipi_pixclk,
					pinfo[i].mclk_src, pinfo[i].client, pinfo[i].adapter, pinfo[i].reset_pin);

		dev = infotm_bridge[i].dev;
		if (dev && dev->init) {
			ret = dev->init(&infotm_bridge[i], 0);
			if (ret < 0)
				bridge_log(B_LOG_ERROR, "%s init failed\n", dev->name);
		}
	}

	return 0;
err:
	bridge_sensor_exit(ALL_SENSOR_NUM);
	return -1;
}
EXPORT_SYMBOL(infotm_bridge_init);

void infotm_bridge_exit()
{
	int i = 0;
	struct infotm_bridge_dev* dev = NULL;

	for (i =0; i < MAX_SENSORS_NUM; i++) {
		if (!infotm_bridge[i].exist)
			continue;

		dev = infotm_bridge[i].dev;
		if (dev && dev->exit)
			dev->exit(0);
	}

	bridge_sensor_exit(ALL_SENSOR_NUM);
}
EXPORT_SYMBOL(infotm_bridge_exit);

static void bridge_user_info_init(
	struct bridge_uinfo*user_info, struct _infotm_bridge* bridge)
{
	int i = 0;

	struct sensor_info* pinfo= NULL;

	strncpy(user_info->name, bridge->name, MAX_STR_LEN);
	user_info->sensor_num = bridge->sensor_num;

	pinfo = bridge->info;
	if (pinfo) {
		user_info->chn = pinfo->chn;
		user_info->i2c_addr = pinfo->i2c_addr;
		user_info->imager = pinfo->imager;
		user_info->mipi_lanes = pinfo->lanes;
		user_info->i2c_exist = pinfo->exist;

		bridge_log(B_LOG_INFO, "bridge:%s chn:%d i2c_addr:0x%x "
			"imager:%d mipi_lanes:%d i2c_exist:%d\n",
			user_info->name,
			user_info->chn,
			user_info->i2c_addr,
			user_info->imager,
			user_info->mipi_lanes ,
			user_info->i2c_exist);
	}

	for (i =0; i < MAX_BRIDGE_SENSORS_NUM; i++) {

		pinfo = bridge->sensor_info[i];
		if (!pinfo) {
			bridge_log(B_LOG_ERROR, "bridge_sensor_info%d is not exist\n", i);
			continue;
		}

		strncpy(user_info->bsinfo[i].name,
			pinfo->name, MAX_STR_LEN);

		user_info->bsinfo[i].chn = pinfo->chn;
		user_info->bsinfo[i].i2c_addr = pinfo->i2c_addr;
		user_info->bsinfo[i].mipi_lanes = pinfo->lanes;
		user_info->bsinfo[i].exist = pinfo->exist;

		if (user_info->bsinfo[i].i2c_addr)
			bridge_log(B_LOG_INFO, "bridge-sensor:%s chn:%d i2c_addr:0x%x "
				" mipi_lanes:%d i2c_exist:%d\n",
				user_info->bsinfo[i].name,
				user_info->bsinfo[i].chn,
				user_info->bsinfo[i].i2c_addr,
				user_info->bsinfo[i].mipi_lanes,
				user_info->bsinfo[i].exist);

		/* TODO: add sensor relating file */
	}
}

static long bridge_ioctl(struct file *flip, unsigned int ioctl_cmd,
	unsigned long ioctl_param)
{
	int index;
	struct infotm_bridge_dev* dev = NULL;

	index = 0; /*Note: the case only support one bridge */

	if (!infotm_bridge[index].info || !infotm_bridge[index].exist)
		return -EACCES;

	switch (ioctl_cmd) {
	case BRIDGE_G_INFO:
		bridge_user_info_init(&user_info[index], &infotm_bridge[index]);

		if (copy_to_user((struct bridge_uinfo *)ioctl_param,
			&(user_info[index]), sizeof(struct bridge_uinfo)))
		break;

	case BRIDGE_G_CFG:
	case BRIDGE_S_CFG:
		dev = infotm_bridge[index].dev;
		if (dev && dev->control)
			return dev->control(ioctl_cmd, ioctl_param);
		else
			return 0;

	default:
		break;
	}

	return 0;
}

static int bridge_open(struct inode *inode , struct file *filp)
{
	return 0;
}

static int bridge_close(struct inode *inode , struct file *filp)
{
	return 0;
}
static struct file_operations bridge_fops = {
	.owner = THIS_MODULE,
	.open  = bridge_open,
	.release = bridge_close,
	.unlocked_ioctl = bridge_ioctl,
};

static int __init bridge_driver_init(void)
{
	int ret = 0;

	bridge_dev.name =
		(const char *)kzalloc(MAX_STR_LEN, GFP_KERNEL);

	bridge_dev.minor = MISC_DYNAMIC_MINOR;
	strncpy((char *)bridge_dev.name, (const char *)"bridge0", MAX_STR_LEN);
	bridge_dev.fops = &bridge_fops;

	if (bridge_dev.name == NULL)
		return -ENOMEM;

	ret = misc_register(&bridge_dev);
	if(ret) {
		pr_err("bridge sensor misc regbister fail.\n");
		goto err1;
	}

	(bridge_dev.this_device)->platform_data = kzalloc(sizeof(int), GFP_KERNEL);
	ret = device_create_file(bridge_dev.this_device, &dev_attr_infotm_bridge_info);
	if(ret) {
		pr_err("ddk sensor device create file fail.\n");
		goto err2;
	}

	return 0;

err2:
	misc_deregister(&bridge_dev);
err1:
	kfree(bridge_dev.name);
	return -ENODEV;
}

static void __exit bridge_driver_exit(void)
{
	device_remove_file(bridge_dev.this_device, &dev_attr_infotm_bridge_info);
	misc_deregister(&(bridge_dev));
}
module_init(bridge_driver_init);
module_exit(bridge_driver_exit);
MODULE_AUTHOR("Davinci.Liang");
MODULE_DESCRIPTION("infotm bridge core driver.");
MODULE_LICENSE("Dual BSD/GPL");
