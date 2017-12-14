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
 ** v0.0.1	first version.
 ** v0.0.2  version 2017-1-10
**  v1.0.0 use power_up module
 ** sam.zhou & Davinci.Liang
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

#ifdef CONFIG_ARCH_APOLLO3
#include <linux/camera/sx2.h>
#endif

#include "items_utils.h"
#include "power_up.h"
#include "../bridge/bridge.h"

static long ddk_sensor_ioctl(struct file *flip, unsigned int ioctl_cmd,
		unsigned long ioctl_param);
static int ddk_sensor_open(struct inode *inode , struct file *filp);
static int ddk_sensor_close(struct inode *inode , struct file *filp);
static int ddk_sensor_probe(struct i2c_client *client, const struct i2c_device_id *dev_id);
static int ddk_sensor_remove(struct i2c_client *client);
static ssize_t ddk_sensor_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ddk_sensor_store(struct device *dev, \
		struct device_attribute *attr, const char *buf, size_t size);

static struct i2c_driver ddk_sensor_driver[MAX_SENSORS_NUM];
static struct i2c_device_id ddk_sensor_id[MAX_SENSORS_NUM];
static struct sensor_info ddk_sensor_info[MAX_SENSORS_NUM];

static struct file_operations ddk_sensor_fops = {
	.owner = THIS_MODULE,
	.open  = ddk_sensor_open,
	.release = ddk_sensor_close,
	.unlocked_ioctl = ddk_sensor_ioctl,
};

static struct miscdevice ddk_sensor_dev[MAX_SENSORS_NUM] ;

DEVICE_ATTR(sensor_info, 0666, ddk_sensor_show, ddk_sensor_store);
static ssize_t ddk_sensor_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int index = 0;
	index = *(int *)(dev->platform_data);
	pr_db("dev->index =%d::interface = %s\n", *(int *)(dev->platform_data),
		ddk_sensor_info[index].interface );

	if (strcmp(ddk_sensor_info[index].interface, "mipi") == 0)
		csi2_dump_regs();

	return sprintf(buf, "name=%s\ninterface=%s\ni2c_addr=0x%x\nchn=%d\n"
					"lanes=%d\ncsi_freq=%d\nmclk_src=%s\n"
					"mclk=%d\nwraper_use=%d\nwwidth=%d\nwheight=%d\n"
					"whdly=%d\nwvdly=%dimager=%d\n ", 
					ddk_sensor_info[index].name,
					ddk_sensor_info[index].interface,
					ddk_sensor_info[index].i2c_addr,
					ddk_sensor_info[index].chn,
					ddk_sensor_info[index].lanes,
					ddk_sensor_info[index].csi_freq ,
					ddk_sensor_info[index].mclk_src,
					ddk_sensor_info[index].mclk, 
					ddk_sensor_info[index].wraper_use,
					ddk_sensor_info[index].wwidth,
					ddk_sensor_info[index].wheight,
					ddk_sensor_info[index].whdly,
					ddk_sensor_info[index].wvdly,
					ddk_sensor_info[index].imager);
}

static ssize_t ddk_sensor_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	en_dbg = simple_strtol(buf, NULL, 10);
	pr_db("en_dbg = %d\n", en_dbg);
	return size;
}

static long ddk_sensor_ioctl(struct file *flip, unsigned int ioctl_cmd,
		unsigned long ioctl_param)
{
	int index;
	int phy_status = 0;
	char *item_path = NULL;
	char * path = kzalloc(MAX_STR_LEN, GFP_KERNEL);
	memset((char *)path, 0, sizeof(path));
	path = d_path((const struct path*)&(flip->f_path), path, MAX_STR_LEN);
	pr_db("path = %s\n", path);
	index = simple_strtol(&path[strlen(path) - 1], NULL, 10);
	pr_db("index = %d\n", index);

	switch (ioctl_cmd) {
	case GETI2CADDR:
		if (copy_to_user((unsigned int *)ioctl_param, &(ddk_sensor_info[index].i2c_addr), sizeof(int)))
			return -EACCES;
		break;

	case GETI2CCHN:
		if (copy_to_user((unsigned int *)ioctl_param, &(ddk_sensor_info[index].chn), sizeof(int)))
			return -EACCES;
		break;

	case GETNAME:
		if (copy_to_user((char *)ioctl_param, (ddk_sensor_info[index].name), MAX_STR_LEN))
			return -EACCES;
		break;

	case GETIMAGER:
		if (copy_to_user((unsigned int *)ioctl_param, &(ddk_sensor_info[index].imager), sizeof(int)))
			return -EACCES;
		break;

	case SETPHYENABLE:
		if (copy_from_user(&phy_status, (unsigned int *)ioctl_param, sizeof(int))) {
			pr_err("SETPHYENABLE copy from user error\n");
			return -EACCES;
		}

		if (phy_status == 1)//enable phy
			csi_core_open();
		else //disable
			csi2_phy_poweron(CSI_DISABLE);
		break;

	case GETSENSORPATH:
		item_path = ddk_get_sensor_path(index);
		if (copy_to_user((char *)ioctl_param, item_path, PATH_ITEM_MAX_LEN))
			return -EACCES;
		break;

	case GETISPPATH:
		item_path = ddk_get_isp_path(index);
		if (copy_to_user((char *)ioctl_param, item_path, PATH_ITEM_MAX_LEN))
			return -EACCES;
		break;

	default:
		break;
	}

	return 0;
}

static int ddk_sensor_open(struct inode *inode , struct file *filp)
{
	return 0;
}

static int ddk_sensor_close(struct inode *inode , struct file *filp)
{
	return 0;
}

//probe is core work of the ddk sensor i2c driver.
static int ddk_sensor_probe(struct i2c_client *client, const struct i2c_device_id *dev_id)
{
	int ret = 0, index = 0;

	index =*(int *)(client->dev.platform_data);
	pr_db("index = %d \n", index);
	if(!strcmp(ddk_sensor_info[index].interface, "mipi") &&
		ddk_sensor_info[index].lanes > 0) {
		csi_core_init(ddk_sensor_info[index].lanes, ddk_sensor_info[index].csi_freq);
		csi_core_open();
	}

	if (ddk_sensor_info[index].wraper_use) {

#ifdef CONFIG_ARCH_Q3F
		isp_dvp_wrapper_probe();
		isp_dvp_wrapper_init(0x02,
			ddk_sensor_info[index].wwidth,
			ddk_sensor_info[index].wheight,
			ddk_sensor_info[index].whdly,
			ddk_sensor_info[index].wvdly);

#elif CONFIG_ARCH_APOLLO3
		if (index == 0) {
			isp_dvp_wrapper_probe();
			isp_dvp_wrapper_init(0x02,
				ddk_sensor_info[index].wwidth,
				ddk_sensor_info[index].wheight,
				ddk_sensor_info[index].whdly,
				ddk_sensor_info[index].wvdly);
		} else if (index == 1) {
			isp_dvp_wrapper1_probe();
			isp_dvp_wrapper1_init(0x02,
				ddk_sensor_info[index].wwidth,
				ddk_sensor_info[index].wheight,
				ddk_sensor_info[index].whdly,
				ddk_sensor_info[index].wvdly);
		}
#endif

	}

	//power up sensor
	ret = ddk_sensor_power_up(&ddk_sensor_info, index, 1);
	if(ret) {
		pr_err("in ddk densor probe sensor%d power up error\n", index);
		goto err1;
	}

	return 0;

err1:
	ddk_sensor_power_up(&ddk_sensor_info, index, 0);
	csi_core_close();

	return ret;
}

static int ddk_sensor_remove(struct i2c_client *client)
{
	int index = 0;
	index =*(int *)(client->dev.platform_data);
	pr_db("index = %d \n", index);

	ddk_sensor_power_up(&ddk_sensor_info, index, 0);
	if(!strcmp(ddk_sensor_info[index].interface, "mipi"))
		csi_core_close();

	pr_db("ddk sensor driver removed and exit.\n");
	return 0;
}

static int __init ddk_sensor_driver_init(void)
{
	char tmp[MAX_STR_LEN];
	struct i2c_board_info sensor_i2c_info[MAX_SENSORS_NUM];
	int ret = 0, i = 0;
	int sensor_num = 0;

	imapx_pad_init("cam");
	memset((void *)ddk_sensor_id, 0, sizeof(ddk_sensor_id));

	ddk_parse_ext_item();

	for(i = 0; i < MAX_SENSORS_NUM; i++) {
		memset((void *)tmp, 0, sizeof(tmp));
		sprintf(tmp, "%s%d", "ddk_sensor", i);
		//not to be so early
		//strncpy(ddk_sensor_id[i].name, tmp, I2C_NAME_SIZE);
		ret = get_sensor_board_info(&ddk_sensor_info[i], i);
		if(-2 == ret) {
			pr_err("ddk sensor%d interface item not exist!\n", i);
			continue;
		}

		if (!strcmp(ddk_sensor_info[i].interface,  "camif") ||
			!strcmp(ddk_sensor_info[i].interface,  "camif-csi"))
			continue;

		if (0 == strncmp(ddk_sensor_info[i].mux, "cam1-mux", 8)) {
			imapx_pad_init("cam1-mux");
		} else if (0 == strncmp(ddk_sensor_info[i].mux, "cam1", 4)) {
			imapx_pad_init("cam1");
		} else {
			if (i == 1) imapx_pad_init("cam1-mux");
		}
		sensor_num++;

		memset((void *)sensor_i2c_info, 0, sizeof(sensor_i2c_info));
		ddk_sensor_dev[i].name = ddk_sensor_driver[i].driver.name \
		    = (const char *)kzalloc(MAX_STR_LEN, GFP_KERNEL);
		if (ddk_sensor_dev[i].name == NULL) return -ENOMEM;
		strcpy(sensor_i2c_info[i].type, tmp);
		ddk_sensor_info[i].i2c_addr = sensor_i2c_info[i].addr = ddk_sensor_info[i].i2c_addr >> 1;
		ddk_sensor_driver[i].driver.owner = THIS_MODULE;
		strcpy((char *)ddk_sensor_driver[i].driver.name, (const char *)tmp);
		ddk_sensor_driver[i].probe = ddk_sensor_probe;
		ddk_sensor_driver[i].remove = ddk_sensor_remove;
		ddk_sensor_driver[i].id_table = &ddk_sensor_id[i];
		ddk_sensor_dev[i].minor = MISC_DYNAMIC_MINOR;
		strcpy((char *)ddk_sensor_dev[i].name, (const char *)tmp);
		ddk_sensor_dev[i].fops = &ddk_sensor_fops;

		pr_db("this i2c add = 0x%x, ddk_sensor_dev[i].name = %s\n",
			sensor_i2c_info[i].addr, ddk_sensor_dev[i].name);
		pr_db("ddk_sensor_driver[i].driver.name = %s, ddk_sensor_id[i].name=%s \n",
			ddk_sensor_driver[i].driver.name, ddk_sensor_id[i].name);
		//get adapter
		ddk_sensor_info[i].adapter = i2c_get_adapter(ddk_sensor_info[i].chn);

		if(!ddk_sensor_info[i].adapter) {
			pr_err("ddk sensor get i2c adapter failed.\n");
			return -ENODEV;
		}

		ddk_sensor_info[i].client = i2c_new_device(ddk_sensor_info[i].adapter, &sensor_i2c_info[i]);
		if(!ddk_sensor_info[i].client) {
			pr_err("ddk sensor add new client devide failed.\n");
			goto err1;
		}

		//move here for id_table set value, because i2c_new_device may call probe function.
		strncpy(ddk_sensor_id[i].name, tmp, I2C_NAME_SIZE);
		ddk_sensor_info[i].client->flags = I2C_CLIENT_SCCB;
		((ddk_sensor_info[i].client)->dev).platform_data = kzalloc(sizeof(int), GFP_KERNEL);
		*(int *) (((ddk_sensor_info[i].client)->dev).platform_data) = i;
		//add i2c_driver
		//int ret = 0;
		printk("after i2c new device\n");
		ret = i2c_add_driver(&ddk_sensor_driver[i]);
		if(ret) {
			pr_err("ddk sensor add i2c drvier fail.\n");
			goto err2;
		}

		ret = misc_register(&ddk_sensor_dev[i]);
		if(ret) {
			pr_err("ddk sensor misc regbister fail.\n");
			goto err3;
		}

		//for sys filesystem used.
		(ddk_sensor_dev[i].this_device)->platform_data = kzalloc(sizeof(int), GFP_KERNEL);
		*((int *)((ddk_sensor_dev[i].this_device)->platform_data)) = i;
		ret = device_create_file(ddk_sensor_dev[i].this_device, &dev_attr_sensor_info);
		if(ret) {
			pr_err("ddk sensor device create file fail.\n");
			goto err4;
		}
	}

	ret = infotm_bridge_check_exist();
	if (ret > 0) {
		infotm_bridge_init(ddk_sensor_info);
	} else {
#ifdef CONFIG_ARCH_APOLLO3
		if (sensor_num == 1){
			if (!strncmp(ddk_sensor_info[0].interface, "dvp", MAX_STR_LEN))
				daul_dvp_bridge_init(SX2_SENSOR_DVP0);

			if (!strncmp(ddk_sensor_info[1].interface, "dvp", MAX_STR_LEN))
				daul_dvp_bridge_init(SX2_SENSOR_DVP1);
		}
#endif
	}
	return 0;

err4:
	misc_deregister(&(ddk_sensor_dev[i]));
err3:
	i2c_del_driver(&(ddk_sensor_driver[i]));
err2:
	i2c_unregister_device(ddk_sensor_info[i].client);
err1:
	i2c_put_adapter(ddk_sensor_info[i].adapter);
	return -ENODEV;
}

static void __exit ddk_sensor_driver_exit(void)
{
	int i = 0;
	for (i = 0; i < MAX_SENSORS_NUM; i++) {
		if (ddk_sensor_info[i].exist) {
			device_remove_file(ddk_sensor_dev[i].this_device, &dev_attr_sensor_info);
			misc_deregister(&(ddk_sensor_dev[i]));
			if(ddk_sensor_info[i].client) {
				i2c_unregister_device(ddk_sensor_info[i].client);
			}

			if(ddk_sensor_info[i].adapter) {
				i2c_put_adapter(ddk_sensor_info[i].adapter);
			}
			i2c_del_driver(&(ddk_sensor_driver[i]));
		}
	}
}
module_init(ddk_sensor_driver_init);
module_exit(ddk_sensor_driver_exit);

MODULE_AUTHOR("sam");
MODULE_DESCRIPTION("imagination ddk sensor driver.");
MODULE_LICENSE("Dual BSD/GPL");
