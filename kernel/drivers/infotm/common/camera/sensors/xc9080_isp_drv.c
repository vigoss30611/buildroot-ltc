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
 **
 *****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <mach/items.h>
#include <linux/sysfs.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <mach/pad.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/isp_xc9080_driver.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <linux/device.h>
#include "xc9080_common.h"

#define SMODULE_NAME	("xc9080")
#define SENSOR_ADAPTER_ID (0x04)
#ifdef CONFIG_USE_PCM_CLK
#define IMAPX_PCM_MCLK_CFG       (0x0c)
#define IMAPX_PCM_CLKCTL         (0x08)
//#define AR0330_MCLK_PAD   (87)
#endif
#ifdef CONFIG_INFT_MMC0_CLK_CAMERA
uint32_t freq_sd_write(u32 slot_id , u32 req_freq);
#endif
#define ADDRESS_LENGTH	sizeof(u16)
#define DATA_LENGTH		sizeof(u8)

struct chip_resource {
	struct i2c_client *client;
	u8 *writebuf;
};

static uint i2c_addr = (0x36);
static uint mipi = 1;
static uint oclk = 24000000;
static uint lanes = 1;
static uint freq = 384;
static uint wraper = 0;
static uint wpwidth = 1920;
static uint wpheight =1088;
static uint hdly = 114;
static uint vdly = 12;
static uint i2c_chn = SENSOR_ADAPTER_ID;
static struct chip_resource *g_res;
module_param(i2c_chn, uint, S_IRUGO);
MODULE_PARM_DESC(i2c_chn, "sensor i2c channel.");
module_param(wraper, uint, S_IRUGO);
MODULE_PARM_DESC(wraper, "sensor used wraper.");
module_param(wpwidth, uint, S_IRUGO);
MODULE_PARM_DESC(wpwidth, "wraper output width.");
module_param(wpheight, uint, S_IRUGO);
MODULE_PARM_DESC(wpheight, "wraper ouput height.");
module_param(hdly, uint, S_IRUGO);
MODULE_PARM_DESC(hdly, "wraper height delay.");
module_param(vdly, uint, S_IRUGO);
MODULE_PARM_DESC(vdly, "wraper weight delay.");
module_param(i2c_addr, uint, S_IRUGO);
MODULE_PARM_DESC(i2c_addr, "sensor device address.");
module_param(mipi, uint, S_IRUGO);
MODULE_PARM_DESC(mipi, "sensor device is MIPI: 0(dvp) 1(mipi).");
module_param(oclk, uint, S_IRUGO);
MODULE_PARM_DESC(oclk, "sensor output clock.");
module_param(lanes, uint, S_IRUGO);
MODULE_PARM_DESC(lanes, "mipi lanes.");
module_param(freq, uint, S_IRUGO);
MODULE_PARM_DESC(freq, "mipi frequence.");

#define XC9080_CONFIG	XC9080_2160_1080_RAW10

static struct reginfo *xc9080_config;
static struct reginfo *hm2131_config;
static int xc9080_config_array_size;
static int hm2131_config_array_size;

static struct i2c_adapter *adapter = NULL;
static struct i2c_client *client = NULL;
static long isp_xc9080_ioctl(struct file *flip, unsigned int ioctl_cmd,
	    unsigned long ioctl_param);
static int isp_xc9080_open(struct inode *inode , struct file *filp);
static int isp_xc9080_close(struct inode *inode , struct file *filp);
static int isp_xc9080_probe(struct i2c_client *client, const struct i2c_device_id *dev_id);
static int isp_xc9080_remove(struct i2c_client *client);
static ssize_t isp_xc9080_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t isp_xc9080_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size);
static int xc9080_bypass(struct chip_resource *res, int on_off);
static void xc9080_init_sequence(struct chip_resource *res);
static int xc9080_enable(int enable);
static const struct i2c_device_id isp_xc9080_id[] = {
	{SMODULE_NAME, 0},
	{},
};
static struct i2c_driver isp_xc9080_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = SMODULE_NAME,
	},
	.probe = isp_xc9080_probe,
	.remove = isp_xc9080_remove,
	.id_table = isp_xc9080_id,
};
static struct _sensor_info {
	int oclk_pin;
	int reset_pin;
	int pwd_pin;
	struct regulator *iovdd;
	int iovol;
	struct regulator *dvdd;
	int dvol;
	struct regulator *corevdd;
	int cvol;
	struct regulator *padvdd;
	int padvol;
	struct clk *oclk;
	struct clk *csiclk;
}isp_xc9080_info;

static struct file_operations isp_xc9080_fops = {
		.owner = THIS_MODULE,
		.open  = isp_xc9080_open,
		.release = isp_xc9080_close,
		.unlocked_ioctl = isp_xc9080_ioctl,
};
static struct miscdevice isp_xc9080_dev = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = SMODULE_NAME,
		.fops = &isp_xc9080_fops
};
DEVICE_ATTR(sensor_info, 0444, isp_xc9080_show, isp_xc9080_store);
static ssize_t isp_xc9080_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	//todo sprintf information to buf
	return sprintf(buf, "i2c_addr=0x%x\nmipi=%d\nlanes=%d\ncsi match freq=%d\noclk=%d\n\
			oclk_pin=%d\nreset_pin=%d\npwd_pin=%d\niovol=%d\ndvol=%d\ncvol=%d\n\
			", i2c_addr, mipi, lanes, freq, oclk, isp_xc9080_info.oclk_pin, isp_xc9080_info.reset_pin, \
			isp_xc9080_info.pwd_pin, isp_xc9080_info.iovol, isp_xc9080_info.dvol, \
			isp_xc9080_info.cvol);
}
static ssize_t isp_xc9080_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	return size;
}
static long isp_xc9080_ioctl(struct file *flip, unsigned int ioctl_cmd,
	    unsigned long ioctl_param)
{
	switch (ioctl_cmd) {
		case GETI2CADDR:
			if (copy_to_user((unsigned int *)ioctl_param, &i2c_addr, sizeof(i2c_addr))) {
				return -EACCES;
			}
			break;
		case BYPASSMODE:
			{
				int mode = ioctl_param ? XC9080_BYPASS_ON : XC9080_BYPASS_OFF;
				xc9080_bypass(g_res, mode);
			}
			break;
		case BRIDGEENABLE:
			return (xc9080_enable(ioctl_param) ? -EIO : 0);
			break;
		default:
			break;
	}
	return 0;
}
static int isp_xc9080_open(struct inode *inode , struct file *filp)
{
	return 0;
}
static int isp_xc9080_close(struct inode *inode , struct file *filp)
{
	return 0;
}
static int get_sensor_board_info(struct _sensor_info *pinfo , struct i2c_client *client)
{
	struct device *dev = NULL;
	char name[64];
	//initailize info 0
	memset(pinfo, 0, sizeof(*pinfo));
	if(item_exist("sensor.mclk.pads")) {
		pinfo->oclk_pin = item_integer("sensor.mclk.pads", 0);
	}
	else {
		pr_err("isp xc9080 driver get none oclk pin\n");
	}
	if(item_exist("sensor.reset.pads")) {
		pinfo->reset_pin = item_integer("sensor.reset.pads", 0);
	}
	else
	{
		pr_err("isp xc9080 driver get none reset pin\n");
	}
	if(item_exist("sensor.pwdn.pads")) {
		pinfo->pwd_pin = item_integer("sensor.pwdn.pads", 0);
	}
	if(!IS_ERR(client))
	{
		dev = &(client->dev);
	}
#ifndef CONFIG_UVC_NO_PMU
	memset(name, 0, sizeof(name));
	if(item_exist("sensor.iovdd.ldo")) {
		item_string(name, "sensor.iovdd.ldo", 0);
		pinfo->iovdd = regulator_get(dev, name);
		if(IS_ERR(pinfo->iovdd)) {
			pr_err("get iovdd regulator fail\n");
			return PTR_ERR(pinfo->iovdd);
		}
		if(item_exist("sensor.iovdd.vol")) {
			pinfo->iovol = item_integer("sensor.iovdd.vol", 0);
		}
	}
	memset(name, 0, sizeof(name));
	if(item_exist("sensor.dvdd.ldo")) {
		item_string(name, "sensor.dvdd.ldo", 0);
		pinfo->dvdd = regulator_get(dev, name);
		if(IS_ERR(pinfo->dvdd)) {
			pr_err("get dvdd regulator fail\n");
			goto dvdd_err;
		}
		if(item_exist("sensor.dvdd.vol")) {
			pinfo->dvol = item_integer("sensor.dvdd.vol", 0);
		}
	}
	memset(name, 0, sizeof(name));
	if(item_exist("sensor.padvdd.ldo")) {
		item_string(name, "sensor.padvdd.ldo", 0);
		pinfo->padvdd = regulator_get(dev, name);
		if(IS_ERR(pinfo->padvdd)) {
			pr_err("get padvdd regulator fail\n");
			goto padvdd_err;
		}
		if(item_exist("sensor.padvdd.vol")) {
			pinfo->padvol = item_integer("sensor.padvdd.vol", 0);
		}
	}
	memset(name, 0, sizeof(name));
	if(item_exist("sensor.corevdd.ldo")) {
		item_string(name, "sensor.corevdd.ldo", 0);
		pinfo->corevdd = regulator_get(dev, name);
		if(IS_ERR(pinfo->corevdd)) {
			pr_err("get corevdd regulator fail\n");
			goto corevddd_err;
		}
		if(item_exist("sensor.corevdd.vol")) {
			pinfo->cvol = item_integer("sensor.corevdd.vol", 0);
		}
	}
#endif
	return 0;
#ifndef CONFIG_UVC_NO_PMU
corevddd_err:
	if(pinfo->dvdd){
		regulator_put(pinfo->dvdd);
	}
dvdd_err:
	if(pinfo->iovdd){
		regulator_put(pinfo->iovdd);
	}
padvdd_err:
	if(pinfo->padvdd){
		regulator_put(pinfo->padvdd);
	}
	return -1;
#endif
}

#ifndef CONFIG_UVC_NO_PMU
static int isp_xc9080_set_voltage(struct _sensor_info *pinfo)
{
	int ret = 0;
	if(pinfo->iovdd) {
		ret = regulator_set_voltage(pinfo->iovdd, pinfo->iovol, pinfo->iovol);
		if(ret) {
			pr_err("settting iovdd voltage failed\n");
			return ret;
		}
	}
	if(pinfo->dvdd) {
		ret = regulator_set_voltage(pinfo->dvdd, pinfo->dvol, pinfo->dvol);
		if(ret) {
			pr_err("settting dvdd voltage failed\n");
			return ret;
		}
	}
	if(pinfo->padvdd) {
		ret = regulator_set_voltage(pinfo->padvdd, pinfo->padvol, pinfo->padvol);
		if(ret) {
			pr_err("settting padvdd voltage failed\n");
			return ret;
		}
	}
	if(pinfo->corevdd) {
		ret = regulator_set_voltage(pinfo->corevdd, pinfo->cvol, pinfo->cvol);
		if(ret) {
			pr_err("settting corevdd voltage failed\n");
			return ret;
		}
	}
	//if have power down pin
	return 0;
}
#endif

static int isp_xc9080_power_up(struct _sensor_info *pinfo, int up)
{
	int ret = 0, tmp =0;
	if (up) {
#ifndef CONFIG_UVC_NO_PMU
		if (pinfo->iovdd) {
			ret = regulator_enable(pinfo->iovdd);
		}
		if (pinfo->dvdd) {
			ret = regulator_enable(pinfo->dvdd);
		}
		if (pinfo->corevdd) {
			ret = regulator_enable(pinfo->corevdd);
		}
		if (pinfo->padvdd) {
			ret = regulator_enable(pinfo->padvdd);
		}
#endif
		if(pinfo->pwd_pin) {
			tmp = (pinfo->pwd_pin > 0) ? pinfo->pwd_pin : -(pinfo->pwd_pin);
			ret = gpio_request(tmp, "pwd_pin");
			if(ret) {
				pr_err("gpio pwd_pin:%x request failed\n ", pinfo->pwd_pin);
				return ret;
			}
			if(pinfo->pwd_pin < 0) {
				gpio_direction_output(tmp, 1);
				gpio_set_value(tmp, 0);
			}
			else
			{
				gpio_direction_output(tmp, 0);
				gpio_set_value(tmp, 1);
			}
		}
	}
	else
	{
#ifndef CONFIG_UVC_NO_PMU
		if (pinfo->iovdd) {
			regulator_disable(pinfo->iovdd);
		}
		if (pinfo->dvdd) {
			regulator_disable(pinfo->dvdd);
		}
		if (pinfo->corevdd) {
			regulator_disable(pinfo->corevdd);
		}
		if (pinfo->padvdd) {
			regulator_disable(pinfo->padvdd);
		}
#endif
		if(pinfo->pwd_pin) {
//			ret = gpio_request(pinfo->pwd_pin, "pwd_pin");
//			if(ret) {
//				pr_err("gpio pwd_pin:%x request failed\n ", pinfo->pwd_pin);
//				return ret;
//			}
			tmp = (pinfo->pwd_pin > 0) ? pinfo->pwd_pin : -(pinfo->pwd_pin);
			if(pinfo->pwd_pin > 0) {
				gpio_direction_output(tmp, 0);
			}
			else
			{
				gpio_direction_output(tmp, 1);
			}
		}
	}
	return 0;
}

static int isp_xc9080_set_clock_enable(struct _sensor_info *pinfo, int enable)
{
	int ret = 0;

	if(enable) {
#if defined(CONFIG_USE_PCM_CLK)
#elif defined(CONFIG_INFT_MMC0_CLK_CAMERA)
#elif defined(CONFIG_ARCH_APOLLO)
		ret = clk_prepare_enable(pinfo->oclk);
#else
		if (!mipi) {
			ret = clk_prepare_enable(pinfo->oclk);
		} else {
#if defined(CONFIG_INFOTM_LANCHOU_CAMERA)
			ret = clk_prepare_enable(pinfo->oclk);
#else
			ret = clk_prepare_enable(pinfo->oclk);
#endif
		}
#endif
		if (pinfo->reset_pin) {
			gpio_set_value(pinfo->reset_pin, 0);
			usleep_range(5, 10);
			gpio_set_value(pinfo->reset_pin, 1);
			mdelay(30);
		}
	} else {
		if (pinfo->reset_pin) {
			gpio_set_value(pinfo->reset_pin, 0);
			usleep_range(5, 10);
		}
#if defined(CONFIG_USE_PCM_CLK)
#elif defined(CONFIG_INFT_MMC0_CLK_CAMERA)
#elif defined(CONFIG_ARCH_APOLLO)
		clk_disable_unprepare(pinfo->oclk);
#else
		if (!mipi) {
			clk_disable_unprepare(pinfo->oclk);
		} else {
#if defined(CONFIG_INFOTM_LANCHOU_CAMERA)
			clk_disable_unprepare(pinfo->oclk);
#else
			clk_disable_unprepare(pinfo->oclk);
#endif
		}
#endif
	}

	pr_info("%s called, enable=%d ret=%d\n", __func__, enable, ret);
	return ret;
}

static int isp_xc9080_set_clock(struct _sensor_info *pinfo)
{
	int ret = 0, tmp = 0;
#ifdef CONFIG_USE_PCM_CLK
    struct resource *ioarea = NULL;
    void __iomem *pcm_reg = NULL;
#endif
	imapx_pad_init("cam");
	if(pinfo->oclk_pin) {
		gpio_request(pinfo->oclk_pin, "oclk_pin");
		imapx_pad_set_mode(pinfo->oclk_pin, "function");
	}
#if defined(CONFIG_USE_PCM_CLK)
 	//power on and enable pcm module
	module_power_on(SYSMGR_PCM_BASE);
	//set pcm0 pad io function
	imapx_pad_init("pcm0");
	ioarea = request_mem_region(IMAP_PCM0_BASE, IMAP_PCM0_SIZE, "pcm0-camif");
	if(IS_ERR(ioarea)) {
		pr_err("failed to request pcm0 register area in camif driver\n");
		return -EBUSY;
	}
	pcm_reg = ioremap(IMAP_PCM0_BASE, IMAP_PCM0_SIZE);
	if(IS_ERR(pcm_reg)) {
		pr_err("failed to ioremap pcm0 register in camif driver\n");
		return PTR_ERR(pcm_reg);
	}
	//use what frequence you want to do
	writel(0x13, pcm_reg + IMAPX_PCM_CLKCTL);
	writel(4, pcm_reg + IMAPX_PCM_MCLK_CFG);
#elif defined(CONFIG_INFT_MMC0_CLK_CAMERA)
	freq_sd_write(0, 24000000);
#elif defined(CONFIG_ARCH_APOLLO)
	pinfo->oclk = clk_get_sys("imap-clk.0", "imap-clk.0");
	if (IS_ERR(pinfo->oclk)) {
		pr_err("fail to get clk 'imap-clk.0'\n");
		return PTR_ERR(pinfo->oclk);
	}
	ret = clk_set_rate(pinfo->oclk, oclk);
	if (0 > ret){
		pr_err("fail to set camera output %d clock\n", oclk);
		return ret;
	}
#else
	if (!mipi) {
		pinfo->oclk = clk_get_sys("imap-camo", "imap-camo");
		if (IS_ERR(pinfo->oclk)) {
			pr_err("fail to get clk 'imap-camo'\n");
			return PTR_ERR(pinfo->oclk);
		}
		ret = clk_set_rate(pinfo->oclk, oclk);
		if (0 > ret){
			pr_err("fail to set camera output %d clock\n", oclk);
			return ret;
		}
	}
	else {
#if defined(CONFIG_INFOTM_LANCHOU_CAMERA)
		pinfo->oclk = clk_get_sys("imap-camo","imap-camo");
		if(IS_ERR(pinfo->oclk)){
			pr_err("fail to get clk 'imap-camo'\n");
			return PTR_ERR(pinfo->oclk);
		}
		ret = clk_set_rate(pinfo->oclk, oclk);
		if (0 > ret){
			pr_err("fail to set camera output %d clock\n", oclk);
			return ret;
		}
#else
		pinfo->oclk = clk_get_sys("imap-clk.1", "imap-clk.1");
		if (IS_ERR(pinfo->oclk)) {
			pr_err("fail to get clk 'clk-out1'\n");
			return PTR_ERR(pinfo->oclk);
		}
		ret = clk_set_rate(pinfo->oclk, oclk);
		if (0 > ret){
			pr_err("fail to set camera output %d clock\n", oclk);
			return ret;
		}
#endif
	}
#endif
	if (pinfo->reset_pin) {
		tmp = (pinfo->reset_pin > 0) ? pinfo->reset_pin : -(pinfo->reset_pin);
		pinfo->reset_pin = tmp;
		ret = gpio_request(tmp, "rst_pin");
		if(ret != 0 )
		{
			printk("gpio_request reset pin :%x error!\n", pinfo->reset_pin);
			return ret;
		}
		gpio_direction_output(tmp, 0);
	}
	return 0;
}
static int isp_xc9080_remove_clock(struct _sensor_info *pinfo)
{
	if(pinfo->oclk){
		clk_disable_unprepare(pinfo->oclk);
	}
	#if defined(CONFIG_USE_PCM_CLK)
		module_power_down(SYSMGR_PCM_BASE);
	#endif
	return 0;
}

static int xc9080_i2c_single_read(struct chip_resource *res, const u16 addr, u8 *value)
{
	struct i2c_client *client = res->client;
	return xc9080_read(client, addr, value);
}
static int xc9080_i2c_single_write(struct chip_resource *res, const u16 addr, const u8 value)
{
	struct i2c_client *client = res->client;
	return xc9080_write(client, addr, value);
}
#define XC9080_I2C_READ(addr, value)	xc9080_i2c_single_read(res, addr, value)
#define XC9080_I2C_WRITE(addr, value)	xc9080_i2c_single_write(res, addr, value)
static void xc9080_power_up(void)
{
	return;
}
static void xc9080_power_down(void)
{
	return;
}
static int xc9080_read_id(struct chip_resource *res)
{
	struct i2c_client *client = res->client;
	return xc9080_get_id(client);
}
static int xc9080_bypass(struct chip_resource *res, int on_off)
{
	struct i2c_client *client = res->client;
	xc9080_bypass_change(client, on_off);
	return 0;
}
static void xc9080_init_sequence(struct chip_resource *res)
{
	struct i2c_client *client = res->client;
	xc9080_write_array2(client, xc9080_config, xc9080_config_array_size);
}

static int xc9080_enable(int enable)
{
	int ret = 0;

	if(enable) {
		ret = -1;

		if(mipi) {
			if(csi_core_init(lanes, freq) != SUCCESS) {
				pr_err("in isp xc9080 fail to init csi core\n");
				return ret;
			}
			if(csi_core_open() != SUCCESS) {
				pr_err("in isp xc9080 fail to open csi core\n");
				goto err1;
			}
		}
		//set power on and set clock
		ret = isp_xc9080_power_up(&isp_xc9080_info, 1);
		if(ret) {
			pr_err("in isp xc9080 probe power up error\n");
			goto err1;
		}

		ret = isp_xc9080_set_clock_enable(&isp_xc9080_info, 1);
		if(ret) {
			pr_err("in isp xc9080 clock enable error\n");
			goto err2;
		}
		ret = xc9080_read_id(g_res);
		if(ret) {
			pr_err("in isp xc9080 sensor ID is not matched\n");
			goto err3;
		}
		xc9080_init_sequence(g_res);
		return 0;
	}

err3:
	isp_xc9080_set_clock_enable(&isp_xc9080_info, 0);
err2:
	isp_xc9080_power_up(&isp_xc9080_info, 0);
err1:
	csi_core_close();
	return ret;
}

#ifndef CONFIG_UVC_NO_PMU
static void xc9080_free_vlotage(void)
{
	pr_info("%s called\n", __func__);
	if(isp_xc9080_info.corevdd) {
		regulator_put(isp_xc9080_info.corevdd);
	}
	if(isp_xc9080_info.dvdd) {
		regulator_put(isp_xc9080_info.dvdd);
	}
	if(isp_xc9080_info.iovdd) {
		regulator_put(isp_xc9080_info.iovdd);
	}
	if(isp_xc9080_info.padvdd) {
		regulator_put(isp_xc9080_info.padvdd);
	}
}
#endif

//probe is core work of the i2c driver.
static int isp_xc9080_probe(struct i2c_client *client, const struct i2c_device_id *dev_id)
{
	int ret = 0;
	struct chip_resource *res;
	u8 buf;
	int i;

#if (XC9080_CONFIG == XC9080_1920_1080_RAW10)
	xc9080_config = xc9080_1920_1080_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_1920_1080_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_RAW10)
	xc9080_config = xc9080_2160_1080_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_2160_1080_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_3840_1920_RAW10)
	xc9080_config = xc9080_3840_1920_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_3840_1920_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_1920_1080_YUV)
	xc9080_config = xc9080_1920_1080_yuv;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_1920_1080_yuv_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV)
	xc9080_config = xc9080_2160_1080_yuv;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_2160_1080_yuv_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#endif

	res = kzalloc(sizeof(*res), GFP_KERNEL);
	if(res == NULL) {
		ret = -ENOMEM;
		goto err_out;
	}
	res->client = client;
	res->writebuf = kzalloc(ADDRESS_LENGTH + DATA_LENGTH, GFP_KERNEL);
	if(res->writebuf == NULL) {
		ret = -ENOMEM;
		goto err_struct;
	}
	i2c_set_clientdata(client, res);

	//get platform board information
	ret = get_sensor_board_info(&isp_xc9080_info, client);
	if(ret) {
		pr_err("in isp xc9080 probe get board information error\n");
		goto err_writebuf;
	}

#ifndef CONFIG_UVC_NO_PMU
	//set voltage
	ret = isp_xc9080_set_voltage(&isp_xc9080_info);
	if(ret) {
		pr_err("in isp xc9080 probe sensor voltage error\n");
		goto err_writebuf;
	}
#endif
	//set clock
	ret = isp_xc9080_set_clock(&isp_xc9080_info);
	if (ret) {
		pr_err("in isp xc9080 probe sensor set clock error\n");
		goto err1;
	}

    g_res = res;
	return 0;

err1:
#ifndef CONFIG_UVC_NO_PMU
	xc9080_free_vlotage();
#endif
err_writebuf:
	kfree(res->writebuf);
err_struct:
	kfree(res);
err_out:
	return ret;
}
static int isp_xc9080_remove(struct i2c_client *client)
{
	int tmp = 0;
	struct chip_resource *res;
	isp_xc9080_power_up(&isp_xc9080_info, 0);
	if(mipi) {
		csi_core_close();
	}
	isp_xc9080_remove_clock(&isp_xc9080_info);
#ifndef CONFIG_UVC_NO_PMU
	xc9080_free_vlotage();
#endif
	if(isp_xc9080_info.pwd_pin) {
		tmp = (isp_xc9080_info.pwd_pin > 0) ? isp_xc9080_info.pwd_pin : -(isp_xc9080_info.pwd_pin);
		gpio_free(tmp);
	}
	if(isp_xc9080_info.reset_pin)
	{
		tmp = (isp_xc9080_info.reset_pin > 0) ? isp_xc9080_info.reset_pin : -(isp_xc9080_info.reset_pin);
		gpio_free(tmp);
	}
	if(isp_xc9080_info.oclk_pin)
	{
		gpio_free(isp_xc9080_info.oclk_pin);
	}

	res = i2c_get_clientdata(client);
	kfree(res->writebuf);
	kfree(res);
	pr_err("xc9080 driver removed and exit.\n");
	return 0;
}
static int __init isp_xc9080_driver_init(void)
{
	int ret = 0;
	struct i2c_board_info sensor_i2c_info = {
		.type = SMODULE_NAME,
		.addr = (i2c_addr >> 1),
	};
	i2c_addr = i2c_addr >> 1;
//	struct i2c_board_info isp_xc9080_info;
//	isp_xc9080_info.type = SMODULE_NAME;
	printk("this i2c add = 0x%x\n", sensor_i2c_info.addr);
	//get adapter
	adapter = i2c_get_adapter(i2c_chn);
	if(!adapter) {
		pr_err("isp xc9080 get i2c adapter failed.\n");
		return -ENODEV;
	}
	client = i2c_new_device(adapter, &sensor_i2c_info);
	if(!client) {
		pr_err("isp xc9080 add new client devide failed.\n");
		goto err1;
	}
	client->flags = I2C_CLIENT_SCCB;
	//add i2c_driver
	//int ret = 0;
	ret = i2c_add_driver(&isp_xc9080_driver);
	if(ret) {
		pr_err("isp xc9080 add i2c drvier fail.\n");
		goto err2;
	}
	ret = misc_register(&isp_xc9080_dev);
	if(ret) {
		goto err3;
	}
	ret = device_create_file(isp_xc9080_dev.this_device, &dev_attr_sensor_info);
	if(ret) {
		goto err4;
	}
	return ret;
err4:
	misc_deregister(&isp_xc9080_dev);
err3:
i2c_del_driver(&isp_xc9080_driver);
err2:
	i2c_unregister_device(client);
err1:
	i2c_put_adapter(adapter);
	return -ENODEV;
}
static void __exit isp_xc9080_driver_exit(void)
{
	//isp_xc9080_remove(client);
	device_remove_file(isp_xc9080_dev.this_device, &dev_attr_sensor_info);
	misc_deregister(&isp_xc9080_dev);
	if(client)
	{
		i2c_unregister_device(client);
	}
	if(adapter)
	{
		i2c_put_adapter(adapter);
	}
	i2c_del_driver(&isp_xc9080_driver);
}
module_init(isp_xc9080_driver_init);
module_exit(isp_xc9080_driver_exit);

MODULE_DESCRIPTION("driver for xc9080.");
MODULE_LICENSE("Dual BSD/GPL");
