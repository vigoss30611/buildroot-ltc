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
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <mach/pad.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/isp_xc9080_driver.h>
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
#define SENSOR_ADAPTER_ID (0x04)
#define SMODULE_NAME	("xc9080")
#define XC9080_ID_HIGH	0x71
#define XC9080_ID_LOW	0x60
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

//#define ENABLE_9080_AEC
//#define ENABLE_9080_AWB

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
	return 0;
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
}
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
static int isp_xc9080_power_up(struct _sensor_info *pinfo, int up)
{
	int ret = 0, tmp =0;
	if (up) {
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
	clk_prepare_enable(pinfo->oclk);
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
		clk_prepare_enable(pinfo->oclk);
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
		clk_prepare_enable(pinfo->oclk);
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
		clk_prepare_enable(pinfo->oclk);
#endif
	}
#endif
	if (pinfo->reset_pin) {
		tmp = (pinfo->reset_pin > 0) ? pinfo->reset_pin : -(pinfo->reset_pin);
		ret = gpio_request(tmp, "rst_pin");
		if(ret != 0 )
		{
			printk("gpio_request reset pin :%x error!\n", pinfo->reset_pin);
			return ret;
		}
		gpio_direction_output(tmp, 0);
		usleep_range(5, 10);
		gpio_set_value(tmp, 1);
		mdelay(30);
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
#ifdef CONFIG_VIDEO_STK3855
static int enable_status = 0;
static ssize_t isp_xc9080_enable_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
    printk("%s : hello !\n", __func__);
	return sprintf(buf, "%d\n", enable_status);
}
static int sensor_status = 0;
static ssize_t isp_xc9080_enable_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
    printk("%s : hello !\n", __func__);
	int tmp, ret;
    tmp = simple_strtol(buf, NULL, 10);
#if 1
    if (tmp != enable_status)
    {
        if (tmp == 1)
        {
            gpio_set_value(isp_xc9080_info.reset_pin, 0);
            //gpio_set_value(isp_xc9080_info.pwd_pin, 0);
            mdelay(10);
            //gpio_set_value(isp_xc9080_info.pwd_pin, 1);
            mdelay(10);
            gpio_set_value(isp_xc9080_info.reset_pin, 1);
            mdelay(20);
            //nt99142_dvp_initial(nt99142_dvp_client);
            enable_status = 1;
        } else if (tmp == 0)
        {
            gpio_set_value(isp_xc9080_info.reset_pin, 0);
            mdelay(10);
            //gpio_set_value(isp_xc9080_info.pwd_pin, 1);
            mdelay(10);
            enable_status = 0;
        } else
        {
            printk("%s : %d value invaild\n", __func__, tmp);
        }
    }
#elif 0
	static int ct = 0;
	if (ct > 1)
		return count;
	tmp = simple_strtol(buf, NULL, 10);
	printk("%s tmp %d\n", __func__, tmp);
	if ((tmp == 1) && (tmp != sensor_status)) {
		//set power on and set clock
		ret = isp_xc9080_power_up(&isp_xc9080_info, 1);
		if(ret)
		{
			pr_err("in xc9080 probe sensor power up error\n");
			return -1;
		}
		ret = isp_xc9080_set_clock(&isp_xc9080_info);
		if (ret) {
			pr_err("in xc9080 probe sensor set clock error\n");
			return -1;
		}
		//set clock
	} else if ((tmp == 0) && (tmp != sensor_status)){
		//set power on and set clock
		ret = isp_xc9080_power_up(&isp_xc9080_info, 0);
		if(ret)
		{
			pr_err("in xc9080 probe sensor power down error\n");
			return -1;
		}
	}
	ct++;
#else
	//csi_core_open();
#endif
	return count;
}
static struct class_attribute isp_xc9080_class_attrs[] = {
	__ATTR(enable, 0644, isp_xc9080_enable_show, isp_xc9080_enable_store),
	__ATTR_NULL
};
static struct class isp_xc9080_class = {
	.name = "isp_xc9080_debug",
	.class_attrs = isp_xc9080_class_attrs,
};
#endif
static int xc9080_i2c_single_read(struct chip_resource *res, const u16 addr, u8 *value)
{
	struct i2c_msg msg[2];
	struct i2c_client *client = res->client;
	int retval;
	memset(msg, 0, sizeof(msg));
	msg[0].addr = client->addr;
	msg[0].buf = res->writebuf;
	msg[0].buf[0] = (u8)(addr >> 8);
	msg[0].buf[1] = (u8)addr;
	msg[0].len = 2;
	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = value;
	msg[1].len = 1;
	retval = i2c_transfer(client->adapter, msg, 2);
	if (retval == 2) {
		retval = 0;
	}
	return retval;
}
static int xc9080_i2c_single_write(struct chip_resource *res, const u16 addr, const u8 value)
{
	struct i2c_msg msg;
	struct i2c_client *client = res->client;
	int retval;
	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = res->writebuf;
	msg.buf[0] = (u8)(addr >> 8);
	msg.buf[1] = (u8)addr;
	msg.buf[2] = value;
	msg.len = 3;
	retval = i2c_transfer(client->adapter, &msg, 1);
	if(retval == 1) {
		retval = 0;
	}
	return retval;
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
	int retval = -1;
	u8 buf[2] = {0};
	XC9080_I2C_READ(0xfffb, &buf[0]);
	XC9080_I2C_READ(0xfffc, &buf[1]);
	printk(KERN_ERR "xc9080: ID-0x%X%X read-0x%X%X\n", XC9080_ID_HIGH, XC9080_ID_LOW, buf[0], buf[1]);
	if(buf[0] == XC9080_ID_HIGH && buf[1] == XC9080_ID_LOW) {
		retval = 0;
	}
	return retval;
}
static int xc9080_bypass(struct chip_resource *res, int on_off)
{
	int retval = 0;
	if(on_off == XC9080_BYPASS_ON) {
		//on
		XC9080_I2C_WRITE(0xfffd, 0x80);
		XC9080_I2C_WRITE(0xfffe, 0x50);
		XC9080_I2C_WRITE(0x004d, 0x03);
	} else if (on_off == XC9080_BYPASS_OFF){
		//off
		XC9080_I2C_WRITE(0xfffd, 0x80);
		XC9080_I2C_WRITE(0xfffe, 0x50);
		XC9080_I2C_WRITE(0x004d, 0x00);
	}
	return retval;
}
static void xc9080_init_sequence(struct chip_resource *res)
{
	int retval = 0;
#if 0
	XC9080_I2C_WRITE(0xfffd, 0x80);
	XC9080_I2C_WRITE(0xfffe, 0x50);
	/* RESET&PLL */
	XC9080_I2C_WRITE(0x001c, 0xff);
	XC9080_I2C_WRITE(0x001d, 0xff);
	XC9080_I2C_WRITE(0x001e, 0xff);
	XC9080_I2C_WRITE(0x001f, 0xff);
	XC9080_I2C_WRITE(0x0018, 0x00);
	XC9080_I2C_WRITE(0x0019, 0x00);
	XC9080_I2C_WRITE(0x001a, 0x00);
	XC9080_I2C_WRITE(0x001b, 0x00);
	XC9080_I2C_WRITE(0x0030, 0x44);
	XC9080_I2C_WRITE(0x0031, 0x04);
	XC9080_I2C_WRITE(0x0032, 0x32);
	XC9080_I2C_WRITE(0x0033, 0x31);
	XC9080_I2C_WRITE(0x0020, 0x03);
	XC9080_I2C_WRITE(0x0021, 0x0d);
	XC9080_I2C_WRITE(0x0022, 0x01);
	XC9080_I2C_WRITE(0x0023, 0x86);
	XC9080_I2C_WRITE(0x0024, 0x01);//0x0c
	XC9080_I2C_WRITE(0x0025, 0x04);//0x00
	XC9080_I2C_WRITE(0x0026, 0x02);
	XC9080_I2C_WRITE(0x0027, 0x06);
	XC9080_I2C_WRITE(0x0028, 0x01);
	XC9080_I2C_WRITE(0x0029, 0x00);
	XC9080_I2C_WRITE(0x002a, 0x02);
	XC9080_I2C_WRITE(0x002b, 0x05);
	XC9080_I2C_WRITE(0xfffe, 0x50);
	XC9080_I2C_WRITE(0x0050, 0x0f);
	XC9080_I2C_WRITE(0x0054, 0x0f);
	XC9080_I2C_WRITE(0x0058, 0x00);
	XC9080_I2C_WRITE(0x0058, 0x0f);
	/* PAD&DATASEL */
	XC9080_I2C_WRITE(0x00bc, 0x19);
	XC9080_I2C_WRITE(0x0090, 0x38);//0x3a colorbar
	XC9080_I2C_WRITE(0x00a8, 0x09);//0x0a colorbar
	XC9080_I2C_WRITE(0x0200, 0x0f);	//mipi_rx1_pad_en
	XC9080_I2C_WRITE(0x0201, 0x00);
	XC9080_I2C_WRITE(0x0202, 0x80);
	XC9080_I2C_WRITE(0x0203, 0x00);
	XC9080_I2C_WRITE(0x0214, 0x0f);	//mipi_rx2_pad_en
	XC9080_I2C_WRITE(0x0215, 0x00);
	XC9080_I2C_WRITE(0x0216, 0x80);
	XC9080_I2C_WRITE(0x0217, 0x00);
	/* COLORBAR&CROP */
	XC9080_I2C_WRITE(0xfffe, 0x26);
	XC9080_I2C_WRITE(0x8001, 0x88);	//cb1
	XC9080_I2C_WRITE(0x8002, 0x07);
	XC9080_I2C_WRITE(0x8003, 0x40);
	XC9080_I2C_WRITE(0x8004, 0x04);
	XC9080_I2C_WRITE(0x8005, 0x40);
	XC9080_I2C_WRITE(0x8006, 0x40);
	XC9080_I2C_WRITE(0x8007, 0x10);
	XC9080_I2C_WRITE(0x8008, 0xf0);
	XC9080_I2C_WRITE(0x800b, 0x00);
	XC9080_I2C_WRITE(0x8000, 0x0d);
	XC9080_I2C_WRITE(0x8041, 0x88);	//cb2
	XC9080_I2C_WRITE(0x8042, 0x07);
	XC9080_I2C_WRITE(0x8043, 0x40);
	XC9080_I2C_WRITE(0x8044, 0x04);
	XC9080_I2C_WRITE(0x8045, 0x03);
	XC9080_I2C_WRITE(0x8046, 0x10);
	XC9080_I2C_WRITE(0x8047, 0x10);
	XC9080_I2C_WRITE(0x8048, 0xff);
	XC9080_I2C_WRITE(0x804b, 0x00);
	XC9080_I2C_WRITE(0x8040, 0x0d);
	XC9080_I2C_WRITE(0x8010, 0x05);	//crop1
	XC9080_I2C_WRITE(0x8012, 0x38);
	XC9080_I2C_WRITE(0x8013, 0x04);
	XC9080_I2C_WRITE(0x8014, 0x38);
	XC9080_I2C_WRITE(0x8015, 0x04);
	XC9080_I2C_WRITE(0x8016, 0xa5);
	XC9080_I2C_WRITE(0x8017, 0x01);
	XC9080_I2C_WRITE(0x8018, 0x01);
	XC9080_I2C_WRITE(0x8019, 0x00);
	XC9080_I2C_WRITE(0x8050, 0x05);	//crop2
	XC9080_I2C_WRITE(0x8052, 0x38);
	XC9080_I2C_WRITE(0x8053, 0x04);
	XC9080_I2C_WRITE(0x8054, 0x38);
	XC9080_I2C_WRITE(0x8055, 0x04);
	XC9080_I2C_WRITE(0x8056, 0xa5);
	XC9080_I2C_WRITE(0x8057, 0x01);
	XC9080_I2C_WRITE(0x8058, 0x01);
	XC9080_I2C_WRITE(0x8059, 0x00);
	XC9080_I2C_WRITE(0xfffe, 0x2e);	//retiming_crop
	XC9080_I2C_WRITE(0x0026, 0xc0);
	XC9080_I2C_WRITE(0x0100, 0x00);	//crop1
	XC9080_I2C_WRITE(0x0101, 0x01);
	XC9080_I2C_WRITE(0x0102, 0x00);
	XC9080_I2C_WRITE(0x0103, 0x05);
	XC9080_I2C_WRITE(0x0104, 0x07);
	XC9080_I2C_WRITE(0x0105, 0x88);
	XC9080_I2C_WRITE(0x0106, 0x04);
	XC9080_I2C_WRITE(0x0107, 0x40);
	XC9080_I2C_WRITE(0x0108, 0x01);
	XC9080_I2C_WRITE(0x0200, 0x00);	//crop1
	XC9080_I2C_WRITE(0x0201, 0x01);
	XC9080_I2C_WRITE(0x0202, 0x00);
	XC9080_I2C_WRITE(0x0203, 0x05);
	XC9080_I2C_WRITE(0x0204, 0x07);
	XC9080_I2C_WRITE(0x0205, 0x88);
	XC9080_I2C_WRITE(0x0206, 0x04);
	XC9080_I2C_WRITE(0x0207, 0x40);
	XC9080_I2C_WRITE(0x0208, 0x01);
	/* ISPdatapath */
	XC9080_I2C_WRITE(0xfffe, 0x30);	//isp1
	XC9080_I2C_WRITE(0x0000, 0x01);
	XC9080_I2C_WRITE(0x0001, 0x00);
	XC9080_I2C_WRITE(0x0002, 0x10);
	XC9080_I2C_WRITE(0x0003, 0x20);
	XC9080_I2C_WRITE(0x0004, 0x00);
	XC9080_I2C_WRITE(0x0019, 0x01);
	XC9080_I2C_WRITE(0x0050, 0x20);
	XC9080_I2C_WRITE(0x005e, 0x37);
	XC9080_I2C_WRITE(0x005f, 0x04);
	XC9080_I2C_WRITE(0x0060, 0x37);
	XC9080_I2C_WRITE(0x0061, 0x04);
	XC9080_I2C_WRITE(0x0064, 0x38);
	XC9080_I2C_WRITE(0x0065, 0x04);
	XC9080_I2C_WRITE(0x0066, 0x38);
	XC9080_I2C_WRITE(0x0067, 0x04);
	XC9080_I2C_WRITE(0x0006, 0x04);
	XC9080_I2C_WRITE(0x0007, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x04);
	XC9080_I2C_WRITE(0x0009, 0x38);
	XC9080_I2C_WRITE(0x000a, 0x04);
	XC9080_I2C_WRITE(0x000b, 0x38);
	XC9080_I2C_WRITE(0x000c, 0x04);
	XC9080_I2C_WRITE(0x000d, 0x38);
	XC9080_I2C_WRITE(0xfffe, 0x31);	//isp2
	XC9080_I2C_WRITE(0x0000, 0x01);
	XC9080_I2C_WRITE(0x0001, 0x00);
	XC9080_I2C_WRITE(0x0002, 0x10);
	XC9080_I2C_WRITE(0x0003, 0x20);
	XC9080_I2C_WRITE(0x0004, 0x00);
	XC9080_I2C_WRITE(0x0019, 0x01);
	XC9080_I2C_WRITE(0x0050, 0x20);
	XC9080_I2C_WRITE(0x005e, 0x37);
	XC9080_I2C_WRITE(0x005f, 0x04);
	XC9080_I2C_WRITE(0x0060, 0x37);
	XC9080_I2C_WRITE(0x0061, 0x04);
	XC9080_I2C_WRITE(0x0064, 0x38);
	XC9080_I2C_WRITE(0x0065, 0x04);
	XC9080_I2C_WRITE(0x0066, 0x38);
	XC9080_I2C_WRITE(0x0067, 0x04);
	XC9080_I2C_WRITE(0x0006, 0x04);
	XC9080_I2C_WRITE(0x0007, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x04);
	XC9080_I2C_WRITE(0x0009, 0x38);
	XC9080_I2C_WRITE(0x000a, 0x04);
	XC9080_I2C_WRITE(0x000b, 0x38);
	XC9080_I2C_WRITE(0x000c, 0x04);
	XC9080_I2C_WRITE(0x000d, 0x38);
	/* MIPI & FIFO STITCH */
	XC9080_I2C_WRITE(0xfffe, 0x26);
	XC9080_I2C_WRITE(0x0000, 0x20);	//mipi_rx1_lane
	XC9080_I2C_WRITE(0x0009, 0xc4);	//mipi_rx1_set
	XC9080_I2C_WRITE(0x1000, 0x20);	//mipi_rx2_lane
	XC9080_I2C_WRITE(0x1009, 0xc4);	//mipi_rx2_set
	XC9080_I2C_WRITE(0x2019, 0x08);	//MIPI_TX 
	XC9080_I2C_WRITE(0x201a, 0x70);
	XC9080_I2C_WRITE(0x201b, 0x04);
	XC9080_I2C_WRITE(0x201c, 0x38);
	XC9080_I2C_WRITE(0x201d, 0x08);
	XC9080_I2C_WRITE(0x201e, 0x70);
	XC9080_I2C_WRITE(0x201f, 0x04);
	XC9080_I2C_WRITE(0x2020, 0x38);
	XC9080_I2C_WRITE(0x2015, 0x83);
	XC9080_I2C_WRITE(0x2017, 0x2b);
	XC9080_I2C_WRITE(0x2018, 0x2b);
	XC9080_I2C_WRITE(0x2023, 0x0f);	//mipi_tx_set
	XC9080_I2C_WRITE(0xfffe, 0x2c);	//stitch
	XC9080_I2C_WRITE(0x0000, 0x40);
	XC9080_I2C_WRITE(0x0001, 0x08);
	XC9080_I2C_WRITE(0x0002, 0x70);
	XC9080_I2C_WRITE(0x0004, 0x04);
	XC9080_I2C_WRITE(0x0005, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x10);
	XC9080_I2C_WRITE(0x0044, 0x0a); //fifo1
	XC9080_I2C_WRITE(0x0045, 0x04);
	XC9080_I2C_WRITE(0x0048, 0x08);//0x08
	XC9080_I2C_WRITE(0x0049, 0x60);
	XC9080_I2C_WRITE(0x0084, 0x0a); //fifo2
	XC9080_I2C_WRITE(0x0085, 0x04);
	XC9080_I2C_WRITE(0x0088, 0x08);//0x08
	XC9080_I2C_WRITE(0x0089, 0x60);
	/* RETIMMNG&MERGE */
	XC9080_I2C_WRITE(0xfffe, 0x2e);	//re-timing
	XC9080_I2C_WRITE(0x0000, 0x42);
	XC9080_I2C_WRITE(0x0001, 0xee); //0xee
	XC9080_I2C_WRITE(0x0003, 0x01);
	XC9080_I2C_WRITE(0x0004, 0xa0);
	XC9080_I2C_WRITE(0x0006, 0x01);
	XC9080_I2C_WRITE(0x0007, 0xa0);
	XC9080_I2C_WRITE(0x000a,0x13);	//retiming_bypass
	XC9080_I2C_WRITE(0x000b,0x04);
	XC9080_I2C_WRITE(0x000c,0x38);	//retimming_size
	XC9080_I2C_WRITE(0x000d,0x04);
	XC9080_I2C_WRITE(0x1000,0x0a);	//Merge
	XC9080_I2C_WRITE(0x1001,0x70);
	XC9080_I2C_WRITE(0x1002,0x08);
	XC9080_I2C_WRITE(0x1003,0x38);
	XC9080_I2C_WRITE(0x1004,0x04);
	XC9080_I2C_WRITE(0x1005,0x70);
	XC9080_I2C_WRITE(0x1006,0x08);
	XC9080_I2C_WRITE(0x1007,0x38);
	XC9080_I2C_WRITE(0x1008,0x04);
	XC9080_I2C_WRITE(0x1009,0x70);
	XC9080_I2C_WRITE(0x100a,0x08);
	XC9080_I2C_WRITE(0x100b,0x00);
	XC9080_I2C_WRITE(0x100e,0x00);
	XC9080_I2C_WRITE(0x100f,0x01);
	XC9080_I2C_WRITE(0xfffe,0x26);
	XC9080_I2C_WRITE(0x200b,0x25);
	XC9080_I2C_WRITE(0x200c,0x04);
	XC9080_I2C_WRITE(0xfffe,0x50);
#elif 0 // v1.1 disable awb&AE
	XC9080_I2C_WRITE(0xfffd, 0x80);
	//Initial
	//////////RESET&PLL//////////
	XC9080_I2C_WRITE(0xfffe, 0x50);
	XC9080_I2C_WRITE(0x001c, 0xff);
	XC9080_I2C_WRITE(0x001d, 0xff);
	XC9080_I2C_WRITE(0x001e, 0xff);
	XC9080_I2C_WRITE(0x001f, 0xff);
	XC9080_I2C_WRITE(0x0018, 0x00);
	XC9080_I2C_WRITE(0x0019, 0x00);
	XC9080_I2C_WRITE(0x001a, 0x00);
	XC9080_I2C_WRITE(0x001b, 0x00);
	XC9080_I2C_WRITE(0x0030, 0x44);
	XC9080_I2C_WRITE(0x0031, 0x04);
	XC9080_I2C_WRITE(0x0032, 0x32);  
	XC9080_I2C_WRITE(0x0033, 0x31);   
	XC9080_I2C_WRITE(0x0020, 0x03);
	XC9080_I2C_WRITE(0x0021, 0x0d);
	XC9080_I2C_WRITE(0x0022, 0x01);
	XC9080_I2C_WRITE(0x0023, 0x86);
	XC9080_I2C_WRITE(0x0024, 0x01);  
	XC9080_I2C_WRITE(0x0025, 0x04); 
	XC9080_I2C_WRITE(0x0026, 0x02);
	XC9080_I2C_WRITE(0x0027, 0x06);
	XC9080_I2C_WRITE(0x0028, 0x01);
	XC9080_I2C_WRITE(0x0029, 0x00);
	XC9080_I2C_WRITE(0x002a, 0x02);
	XC9080_I2C_WRITE(0x002b, 0x05);
	XC9080_I2C_WRITE(0xfffe, 0x50);
	XC9080_I2C_WRITE(0x0050, 0x0f);  
	XC9080_I2C_WRITE(0x0054, 0x0f);  
	XC9080_I2C_WRITE(0x0058, 0x00);
	XC9080_I2C_WRITE(0x0058, 0x0a);  
	//////////PAD&DATASEL//////////
	XC9080_I2C_WRITE(0xfffe, 0x50);
	XC9080_I2C_WRITE(0x00bc, 0x19);
	XC9080_I2C_WRITE(0x0090, 0x38);
	XC9080_I2C_WRITE(0x00a8, 0x09);  //XC9080_I2C_WRITE(0x09
	XC9080_I2C_WRITE(0x0200, 0x0f);   //mipi_rx1_pad_en
	XC9080_I2C_WRITE(0x0201, 0x00);
	XC9080_I2C_WRITE(0x0202, 0x80);
	XC9080_I2C_WRITE(0x0203, 0x00);
	XC9080_I2C_WRITE(0x0214, 0x0f);   //mipi_rx2_pad_en
	XC9080_I2C_WRITE(0x0215, 0x00);
	XC9080_I2C_WRITE(0x0216, 0x80);
	XC9080_I2C_WRITE(0x0217, 0x00);
	//////////COLORBAR&CROP////////
	XC9080_I2C_WRITE(0xfffe, 0x26);
	XC9080_I2C_WRITE(0x8001, 0x88);   //cb0
	XC9080_I2C_WRITE(0x8002, 0x07);
	XC9080_I2C_WRITE(0x8003, 0x40);
	XC9080_I2C_WRITE(0x8004, 0x04);
	XC9080_I2C_WRITE(0x8005, 0x40);
	XC9080_I2C_WRITE(0x8006, 0x40);
	XC9080_I2C_WRITE(0x8007, 0x10);
	XC9080_I2C_WRITE(0x8008, 0xf0);
	XC9080_I2C_WRITE(0x800b, 0x00);
	XC9080_I2C_WRITE(0x8000, 0x0d);   //XC9080_I2C_WRITE(0x1d
	XC9080_I2C_WRITE(0x8041, 0x88);   //cb1
	XC9080_I2C_WRITE(0x8042, 0x07);
	XC9080_I2C_WRITE(0x8043, 0x40);
	XC9080_I2C_WRITE(0x8044, 0x04);
	XC9080_I2C_WRITE(0x8045, 0x03);
	XC9080_I2C_WRITE(0x8046, 0x10);
	XC9080_I2C_WRITE(0x8047, 0x10);
	XC9080_I2C_WRITE(0x8048, 0xff);
	XC9080_I2C_WRITE(0x804b, 0x00);
	XC9080_I2C_WRITE(0x8040, 0x0d);
	XC9080_I2C_WRITE(0x8010, 0x05);   //crop0
	XC9080_I2C_WRITE(0x8012, 0x38);
	XC9080_I2C_WRITE(0x8013, 0x04);
	XC9080_I2C_WRITE(0x8014, 0x38);
	XC9080_I2C_WRITE(0x8015, 0x04);
	XC9080_I2C_WRITE(0x8016, 0xa5);
	XC9080_I2C_WRITE(0x8017, 0x01);
	XC9080_I2C_WRITE(0x8018, 0x01);
	XC9080_I2C_WRITE(0x8019, 0x00);
	XC9080_I2C_WRITE(0x8050, 0x05);   //crop1
	XC9080_I2C_WRITE(0x8052, 0x38);
	XC9080_I2C_WRITE(0x8053, 0x04);
	XC9080_I2C_WRITE(0x8054, 0x38);
	XC9080_I2C_WRITE(0x8055, 0x04);
	XC9080_I2C_WRITE(0x8056, 0xa5);
	XC9080_I2C_WRITE(0x8057, 0x01);
	XC9080_I2C_WRITE(0x8058, 0x01);
	XC9080_I2C_WRITE(0x8059, 0x00);
	XC9080_I2C_WRITE(0xfffe, 0x2e);    //retiming_crop
	XC9080_I2C_WRITE(0x0026, 0xc0);
	XC9080_I2C_WRITE(0x0100, 0x00);   //crop0
	XC9080_I2C_WRITE(0x0101, 0x01);
	XC9080_I2C_WRITE(0x0102, 0x00);
	XC9080_I2C_WRITE(0x0103, 0x05);
	XC9080_I2C_WRITE(0x0104, 0x07);
	XC9080_I2C_WRITE(0x0105, 0x88);
	XC9080_I2C_WRITE(0x0106, 0x04);
	XC9080_I2C_WRITE(0x0107, 0x40);
	XC9080_I2C_WRITE(0x0108, 0x01);
	XC9080_I2C_WRITE(0x0200, 0x00);   //crop1
	XC9080_I2C_WRITE(0x0201, 0x01);
	XC9080_I2C_WRITE(0x0202, 0x00);
	XC9080_I2C_WRITE(0x0203, 0x05);
	XC9080_I2C_WRITE(0x0204, 0x07);
	XC9080_I2C_WRITE(0x0205, 0x88);
	XC9080_I2C_WRITE(0x0206, 0x04);
	XC9080_I2C_WRITE(0x0207, 0x40);
	XC9080_I2C_WRITE(0x0208, 0x01);
	//////////ISPdatapath////////
	XC9080_I2C_WRITE(0xfffe, 0x30);   //isp0
	XC9080_I2C_WRITE(0x0019, 0x01);
	XC9080_I2C_WRITE(0x0050, 0x20);
	XC9080_I2C_WRITE(0x0004, 0x00);
	XC9080_I2C_WRITE(0x005e, 0x37);
	XC9080_I2C_WRITE(0x005f, 0x04);
	XC9080_I2C_WRITE(0x0060, 0x37);
	XC9080_I2C_WRITE(0x0061, 0x04);
	XC9080_I2C_WRITE(0x0064, 0x38);
	XC9080_I2C_WRITE(0x0065, 0x04);
	XC9080_I2C_WRITE(0x0066, 0x38);
	XC9080_I2C_WRITE(0x0067, 0x04);
	XC9080_I2C_WRITE(0x0006, 0x04);
	XC9080_I2C_WRITE(0x0007, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x04);
	XC9080_I2C_WRITE(0x0009, 0x38);
	XC9080_I2C_WRITE(0x000a, 0x04);
	XC9080_I2C_WRITE(0x000b, 0x38);
	XC9080_I2C_WRITE(0x000c, 0x04);
	XC9080_I2C_WRITE(0x000d, 0x38);
	XC9080_I2C_WRITE(0xfffe, 0x31);   //isp1
	XC9080_I2C_WRITE(0x0019, 0x01);
	XC9080_I2C_WRITE(0x0050, 0x20);
	XC9080_I2C_WRITE(0x0004, 0x00);
	XC9080_I2C_WRITE(0x005e, 0x37);
	XC9080_I2C_WRITE(0x005f, 0x04);
	XC9080_I2C_WRITE(0x0060, 0x37);
	XC9080_I2C_WRITE(0x0061, 0x04);
	XC9080_I2C_WRITE(0x0064, 0x38);
	XC9080_I2C_WRITE(0x0065, 0x04);
	XC9080_I2C_WRITE(0x0066, 0x38);
	XC9080_I2C_WRITE(0x0067, 0x04);
	XC9080_I2C_WRITE(0x0006, 0x04);
	XC9080_I2C_WRITE(0x0007, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x04);
	XC9080_I2C_WRITE(0x0009, 0x38);
	XC9080_I2C_WRITE(0x000a, 0x04);
	XC9080_I2C_WRITE(0x000b, 0x38);
	XC9080_I2C_WRITE(0x000c, 0x04);
	XC9080_I2C_WRITE(0x000d, 0x38);
	////////MIPI & FIFO STITCH/////////
	XC9080_I2C_WRITE(0xfffe, 0x26);
	XC9080_I2C_WRITE(0x0000, 0x20);   //mipi_rx1_lane
	XC9080_I2C_WRITE(0x0009, 0xc4);   //mipi_rx1_set
	XC9080_I2C_WRITE(0x1000, 0x20);   //mipi_rx2_lane
	XC9080_I2C_WRITE(0x1009, 0xc4);   //mipi_rx2_set
	XC9080_I2C_WRITE(0x2019, 0x08);   //MIPI_TX 
	XC9080_I2C_WRITE(0x201a, 0x70);
	XC9080_I2C_WRITE(0x201b, 0x04);
	XC9080_I2C_WRITE(0x201c, 0x38);
	XC9080_I2C_WRITE(0x201d, 0x08);
	XC9080_I2C_WRITE(0x201e, 0x70);
	XC9080_I2C_WRITE(0x201f, 0x04);
	XC9080_I2C_WRITE(0x2020, 0x38);
	XC9080_I2C_WRITE(0x2015, 0x80); 
	XC9080_I2C_WRITE(0x2017, 0x2b);
	XC9080_I2C_WRITE(0x2018, 0x2b);
	XC9080_I2C_WRITE(0x2023, 0x0f);   //mipi_tx_set
	XC9080_I2C_WRITE(0xfffe, 0x2c);   //stitch
	XC9080_I2C_WRITE(0x0000, 0x40);
	XC9080_I2C_WRITE(0x0001, 0x08);
	XC9080_I2C_WRITE(0x0002, 0x70);
	XC9080_I2C_WRITE(0x0004, 0x04);
	XC9080_I2C_WRITE(0x0005, 0x38);
	XC9080_I2C_WRITE(0x0008, 0x10);  
	XC9080_I2C_WRITE(0x0044, 0x0a);	//fifo0
	XC9080_I2C_WRITE(0x0045, 0x04);
	XC9080_I2C_WRITE(0x0048, 0x08);   
	XC9080_I2C_WRITE(0x0049, 0x60);
	XC9080_I2C_WRITE(0x0084, 0x0a);	//fifo1
	XC9080_I2C_WRITE(0x0085, 0x04);
	XC9080_I2C_WRITE(0x0088, 0x08);   
	XC9080_I2C_WRITE(0x0089, 0x60);
	////////RETIMMNG&MERGE/////////
	XC9080_I2C_WRITE(0xfffe, 0x2e);   //retiming
	XC9080_I2C_WRITE(0x0000, 0x42);
	XC9080_I2C_WRITE(0x0001, 0xee);
	XC9080_I2C_WRITE(0x0003, 0x01);
	XC9080_I2C_WRITE(0x0004, 0xa0);
	XC9080_I2C_WRITE(0x0006, 0x01);
	XC9080_I2C_WRITE(0x0007, 0xa0);
	XC9080_I2C_WRITE(0x000a, 0x13);   //retiming_bypass
	XC9080_I2C_WRITE(0x000b, 0x04);  
	XC9080_I2C_WRITE(0x000c, 0x38);   //retimming_size
	XC9080_I2C_WRITE(0x000d, 0x04);
	XC9080_I2C_WRITE(0x1000, 0x0a);   //Merge
	XC9080_I2C_WRITE(0x1001, 0x70);
	XC9080_I2C_WRITE(0x1002, 0x08);
	XC9080_I2C_WRITE(0x1003, 0x38);
	XC9080_I2C_WRITE(0x1004, 0x04);
	XC9080_I2C_WRITE(0x1005, 0x70);
	XC9080_I2C_WRITE(0x1006, 0x08);
	XC9080_I2C_WRITE(0x1007, 0x38);
	XC9080_I2C_WRITE(0x1008, 0x04);
	XC9080_I2C_WRITE(0x1009, 0x70);
	XC9080_I2C_WRITE(0x100a, 0x08);
	XC9080_I2C_WRITE(0x100b, 0x00);
	XC9080_I2C_WRITE(0x100e, 0x00);  
	XC9080_I2C_WRITE(0x100f, 0x01);
	XC9080_I2C_WRITE(0xfffe, 0x26);  //HS prepare
	XC9080_I2C_WRITE(0x200b, 0x25);  //XC9080_I2C_WRITE(0x45
	XC9080_I2C_WRITE(0x200c, 0x04);
	//ISP0 AE
	//// 1.  AE statistical //////
	//////////////////////////////
	XC9080_I2C_WRITE(0xfffe, 0x30);   // AE_avg
	XC9080_I2C_WRITE(0x1f00, 0x00);   // WIN start X
	XC9080_I2C_WRITE(0x1f01, 0x9f);
	XC9080_I2C_WRITE(0x1f02, 0x00);   // WIN stat Y
	XC9080_I2C_WRITE(0x1f03, 0x9f);
	XC9080_I2C_WRITE(0x1f04, 0x02);   // WIN width
	XC9080_I2C_WRITE(0x1f05, 0xee);
	XC9080_I2C_WRITE(0x1f06, 0x02);   // WIN height
	XC9080_I2C_WRITE(0x1f07, 0xee);
	XC9080_I2C_WRITE(0x1f08, 0x03);
	XC9080_I2C_WRITE(0x0051, 0x01);  
	//////////// 2.  AE SENSOR     //////
	/////////////////////////////////////
	XC9080_I2C_WRITE(0xfffe, 0x14); 
	XC9080_I2C_WRITE(0x000e, 0x00);   //Isp0 Used I2c
	XC9080_I2C_WRITE(0x010e, 0x48);  	//camera i2c id
	XC9080_I2C_WRITE(0x010f, 0x01);  	//camera i2c bits
	XC9080_I2C_WRITE(0x0110, 0x0f);  	//sensor type gain
	XC9080_I2C_WRITE(0x0111, 0x0f);  	//sensor type exposure
	//exposure
	XC9080_I2C_WRITE(0x0114, 0x02);  //write camera exposure variable [19:16]
	XC9080_I2C_WRITE(0x0115, 0x02);
	XC9080_I2C_WRITE(0x0116, 0x02);  //write camera exposure variable [15:8]
	XC9080_I2C_WRITE(0x0117, 0x03);
	XC9080_I2C_WRITE(0x011c, 0x00);  //camera exposure addr mask 1
	XC9080_I2C_WRITE(0x011d, 0xff);      
	XC9080_I2C_WRITE(0x011e, 0x00);  //camera exposure addr mask 2
	XC9080_I2C_WRITE(0x011f, 0xff);
	//gain
	XC9080_I2C_WRITE(0x0134, 0x02);  //camera gain addr
	XC9080_I2C_WRITE(0x0135, 0x05);
	XC9080_I2C_WRITE(0x013c, 0x00);  //camera gain addr mask 1
	XC9080_I2C_WRITE(0x013d, 0xff);
	XC9080_I2C_WRITE(0x0145, 0x00);  //gain delay frame
	XC9080_I2C_WRITE(0x0031, 0x01);    //effect exposure mode
	XC9080_I2C_WRITE(0x0032, 0x01);    //effect gain mode
	///////// 3.  AE NORMAL     //////
	////////////////////////////////// 
	XC9080_I2C_WRITE(0xfffe, 0x14);  
	XC9080_I2C_WRITE(0x004c, 0x0);    //AEC mode
#ifdef ENABLE_9080_AEC
	XC9080_I2C_WRITE(0x002b, 0x01);    //ae enable
#else
        XC9080_I2C_WRITE(0x002b, 0x00);    //ae enable
#endif
	XC9080_I2C_WRITE(0x004d, 0x1);    //ae Force write 
	XC9080_I2C_WRITE(0x00fa, 0x01);
	XC9080_I2C_WRITE(0x00fb, 0xf0);   //max gain
	XC9080_I2C_WRITE(0x00fc, 0x00);
	XC9080_I2C_WRITE(0x00fd, 0x20);   //min gain
	XC9080_I2C_WRITE(0x00e2, 0x42);   //max exposure lines
	XC9080_I2C_WRITE(0x00e3, 0x00);   
	XC9080_I2C_WRITE(0x00de, 0x0);
	XC9080_I2C_WRITE(0x00df, 0x10);   //min exp
	XC9080_I2C_WRITE(0x00a0, 0x01);   //day target
	XC9080_I2C_WRITE(0x00a1, 0xc0);
	//flicker
	XC9080_I2C_WRITE(0x0104, 0x02);  
	XC9080_I2C_WRITE(0x0105, 0x01);  //Min FlickerLines Enable
	XC9080_I2C_WRITE(0x0108, 0x14);   
	XC9080_I2C_WRITE(0x0109, 0xc0);   
	//weight
	XC9080_I2C_WRITE(0x0055, 0x04);
	XC9080_I2C_WRITE(0x0056, 0x04);
	XC9080_I2C_WRITE(0x0057, 0x04);
	XC9080_I2C_WRITE(0x0058, 0x04);
	XC9080_I2C_WRITE(0x0059, 0x04);
		    
	XC9080_I2C_WRITE(0x005a, 0x04);
	XC9080_I2C_WRITE(0x005b, 0x04);
	XC9080_I2C_WRITE(0x005c, 0x04);
	XC9080_I2C_WRITE(0x005d, 0x04);
	XC9080_I2C_WRITE(0x005e, 0x04);
		    
	XC9080_I2C_WRITE(0x005f, 0x04);                                                             
	XC9080_I2C_WRITE(0x0060, 0x0a);
	XC9080_I2C_WRITE(0x0061, 0x10);
	XC9080_I2C_WRITE(0x0062, 0x0a);
	XC9080_I2C_WRITE(0x0063, 0x04);
		    
	XC9080_I2C_WRITE(0x0064, 0x08);
	XC9080_I2C_WRITE(0x0065, 0x08);
	XC9080_I2C_WRITE(0x0066, 0x08);
	XC9080_I2C_WRITE(0x0067, 0x08);
	XC9080_I2C_WRITE(0x0068, 0x08);
		    
	XC9080_I2C_WRITE(0x0069, 0x04);
	XC9080_I2C_WRITE(0x006a, 0x04);
	XC9080_I2C_WRITE(0x006b, 0x04);
	XC9080_I2C_WRITE(0x006c, 0x04);
	XC9080_I2C_WRITE(0x006d, 0x04);   //权重for CurAvg
		  
	XC9080_I2C_WRITE(0x0088, 0x00);
	XC9080_I2C_WRITE(0x0089, 0xe7);
	XC9080_I2C_WRITE(0x008a, 0x38);
	XC9080_I2C_WRITE(0x008b, 0x00);  //关注区域 for 亮块
		  
	XC9080_I2C_WRITE(0x0050, 0x01);   //refresh
	//////////  4.AE SPEED       /////
	//////////////////////////////////
	//XC9080_I2C_WRITE(0x00c6, 0x01);     // delay frame
        XC9080_I2C_WRITE(0x00ca, 0x00); //1. threshold low
	XC9080_I2C_WRITE(0x00cb, 0x80);
		   
	XC9080_I2C_WRITE(0x01bc, 0x00);  //thr_l_all
	XC9080_I2C_WRITE(0x01bd, 0xa0);  //thr_l_all
		   
	XC9080_I2C_WRITE(0x01be, 0x00);  //thr_l_avg
	XC9080_I2C_WRITE(0x01bf, 0x60);  //thr_l_avg
		   
	XC9080_I2C_WRITE(0x00cc, 0x00);    //2. threshold high
	XC9080_I2C_WRITE(0x00cd, 0x50);
		   
	XC9080_I2C_WRITE(0x00c7, 0x20);    //3. finally thr
	XC9080_I2C_WRITE(0x00d8, 0x80);    //4. stable thr_h
		   
	XC9080_I2C_WRITE(0x00c8, 0x01);    //total speed
	XC9080_I2C_WRITE(0x0208, 0x02);    //speed limit factor
		   
	XC9080_I2C_WRITE(0x00da, 0x00);    // LumaDiffThrLow
	XC9080_I2C_WRITE(0x00db, 0xa0);
		   
	XC9080_I2C_WRITE(0x00dc, 0x03);    // LumaDiffThrHigh
	XC9080_I2C_WRITE(0x00dd, 0x00);
	//////////  5.AE SMART       ////
	/////////////////////////////////
	XC9080_I2C_WRITE(0x0092, 0x01);  //smart mode
	XC9080_I2C_WRITE(0x0093, 0x07);  //XC9080_I2C_WRITE(0x15  //analysis mode+zhifangtu
	XC9080_I2C_WRITE(0x0094, 0x00);  //smart speed limit
		   
	XC9080_I2C_WRITE(0x00ad, 0x02);   //ATT block Cnt   关注区域亮块数算平均
	////////////table reftarget/////////
	XC9080_I2C_WRITE(0x0022, 0x1e);  //use current fps //isp0、isp1 使用同一寄存器
	XC9080_I2C_WRITE(0x01e4, 0x00);   // Exp value Table 
	XC9080_I2C_WRITE(0x01e5, 0x00);
	XC9080_I2C_WRITE(0x01e6, 0x04);
	XC9080_I2C_WRITE(0x01e7, 0x00);  //table0
		 
	XC9080_I2C_WRITE(0x01e8, 0x00);                                        
	XC9080_I2C_WRITE(0x01e9, 0x00);         
	XC9080_I2C_WRITE(0x01ea, 0x0c);         
	XC9080_I2C_WRITE(0x01eb, 0x00);   //table1      
		             
	XC9080_I2C_WRITE(0x01ec, 0x00);         
	XC9080_I2C_WRITE(0x01ed, 0x00);         
	XC9080_I2C_WRITE(0x01ee, 0x24);         
	XC9080_I2C_WRITE(0x01ef, 0x00);    //table2     
		             
	XC9080_I2C_WRITE(0x01f0, 0x00);         
	XC9080_I2C_WRITE(0x01f1, 0x00);         
	XC9080_I2C_WRITE(0x01f2, 0x6c);          
	XC9080_I2C_WRITE(0x01f3, 0x00);    //table3       
		             
	XC9080_I2C_WRITE(0x01f4, 0x00);         
	XC9080_I2C_WRITE(0x01f5, 0x01);         
	XC9080_I2C_WRITE(0x01f6, 0x44);         
	XC9080_I2C_WRITE(0x01f7, 0x00);   //table4      
		             
	XC9080_I2C_WRITE(0x01f8, 0x00);           
	XC9080_I2C_WRITE(0x01f9, 0x04);         
	XC9080_I2C_WRITE(0x01fa, 0x00);         
	XC9080_I2C_WRITE(0x01fb, 0x00);  //table5   
	//////////reftarget///////////////
	XC9080_I2C_WRITE(0x00b2, 0x02);   // ref target table  0可更改的目标亮度
	XC9080_I2C_WRITE(0x00b3, 0x80); 
	XC9080_I2C_WRITE(0x00b4, 0x02);   // ref target table  1
	XC9080_I2C_WRITE(0x00b5, 0x60); 
	XC9080_I2C_WRITE(0x00b6, 0x02);   // ref target table  2
	XC9080_I2C_WRITE(0x00b7, 0x40); 
	XC9080_I2C_WRITE(0x00b8, 0x01);   // ref target table  3
	XC9080_I2C_WRITE(0x00b9, 0xf0); 
	XC9080_I2C_WRITE(0x00ba, 0x01);   // ref target table  4
	XC9080_I2C_WRITE(0x00bb, 0xb0); 
	XC9080_I2C_WRITE(0x00bc, 0x00);   // ref target table  5
	XC9080_I2C_WRITE(0x00bd, 0xf0); 
	//基于平均亮度维度分? 
	XC9080_I2C_WRITE(0x01cd, 0x28);//avg affect val
	XC9080_I2C_WRITE(0x01cb, 0x00);//avg thr_l
	XC9080_I2C_WRITE(0x01cc, 0x40);  //avg thr_h
	///////////over exposure offest//////
	XC9080_I2C_WRITE(0x01d6, 0x00);   
	XC9080_I2C_WRITE(0x01d7, 0x04); 
	XC9080_I2C_WRITE(0x01d8, 0x14); 
	XC9080_I2C_WRITE(0x01d9, 0x50); 
	XC9080_I2C_WRITE(0x01da, 0x70); 
	XC9080_I2C_WRITE(0x01db, 0x90);
	XC9080_I2C_WRITE(0x01c0, 0x08);
	XC9080_I2C_WRITE(0x01c7, 0x06);
	XC9080_I2C_WRITE(0x01c9, 0x02);
	XC9080_I2C_WRITE(0x01ca, 0x30);
	XC9080_I2C_WRITE(0x01d1, 0x40);
	//基于方差维度分析模式/Variance  /////////////////
	XC9080_I2C_WRITE(0x01b1, 0x30);  // variance affect
	XC9080_I2C_WRITE(0x01b2, 0x01);  // thr l
	XC9080_I2C_WRITE(0x01b3, 0x00);
	XC9080_I2C_WRITE(0x01b4, 0x06);  // thr h
	XC9080_I2C_WRITE(0x01b5, 0x00);
	//基于关注区亮度压制
	XC9080_I2C_WRITE(0x016e, 0x02);
	XC9080_I2C_WRITE(0x016f, 0x80);  //Max ATT thrshold关注区域的门限值
		   
	XC9080_I2C_WRITE(0x01d3, 0x00);   // no use Att limit ratio L
	XC9080_I2C_WRITE(0x01d4, 0x30);   //Att limit ratio H关注区域的调整值
		   
	XC9080_I2C_WRITE(0x016c, 0x00);   //min att thr
	XC9080_I2C_WRITE(0x016d, 0x00);   //min att thr
	XC9080_I2C_WRITE(0x01ba, 0x8);   //Bright Ratio2
	////////////smart ae end /////////
	XC9080_I2C_WRITE(0x1a74, 0x02);//2frame write
	XC9080_I2C_WRITE(0x0212, 0x00);//ae chufa menxian
	/////////////AE  END/////////////////
	////////////////////////////////////
	//ISP0 BLC              
	XC9080_I2C_WRITE(0xfffe, 0x30);         
	XC9080_I2C_WRITE(0x0013, 0x20);//bias   
	XC9080_I2C_WRITE(0x0014, 0x00);         
	XC9080_I2C_WRITE(0x071b, 0x80);//blc   
	//ISP0 LENC                 
	XC9080_I2C_WRITE(0xfffe, 0x30);	 
	//XC9080_I2C_WRITE(0x0002, 0x80);      //bit[7]:lenc_en
	XC9080_I2C_WRITE(0x03ca, 0x16);        
	XC9080_I2C_WRITE(0x03cb, 0xC1);	      
	XC9080_I2C_WRITE(0x03cc, 0x16);	      
	XC9080_I2C_WRITE(0x03cd, 0xC1);	      
	XC9080_I2C_WRITE(0x03ce, 0x16);        
	XC9080_I2C_WRITE(0x03cf, 0xC1);	      
	XC9080_I2C_WRITE(0x03d0, 0x0B);        
	XC9080_I2C_WRITE(0x03d1, 0x60);                  
		 
	XC9080_I2C_WRITE(0xfffe, 0x14);                 
	XC9080_I2C_WRITE(0x002c, 0x01);        
	XC9080_I2C_WRITE(0x0030, 0x01);        
	XC9080_I2C_WRITE(0x0620, 0x01);	      
	XC9080_I2C_WRITE(0x0621, 0x01);	      
		 
	XC9080_I2C_WRITE(0x0928, 0x00);	//ct  
	XC9080_I2C_WRITE(0x0929, 0x67);        
	XC9080_I2C_WRITE(0x092a, 0x00);	      
	XC9080_I2C_WRITE(0x092b, 0xb3);        
	XC9080_I2C_WRITE(0x092c, 0x01);	      
	XC9080_I2C_WRITE(0x092d, 0x32);        
		 
	XC9080_I2C_WRITE(0x06e5, 0x3c);  //A _9);20161101_v2
	XC9080_I2C_WRITE(0x06e6, 0x1e);        
	XC9080_I2C_WRITE(0x06e7, 0x15);        
	XC9080_I2C_WRITE(0x06e8, 0x11);        
	XC9080_I2C_WRITE(0x06e9, 0x11);        
	XC9080_I2C_WRITE(0x06ea, 0x13);        
	XC9080_I2C_WRITE(0x06eb, 0x1c);        
	XC9080_I2C_WRITE(0x06ec, 0x32);        
	XC9080_I2C_WRITE(0x06ed, 0x8 );        
	XC9080_I2C_WRITE(0x06ee, 0x5 );        
	XC9080_I2C_WRITE(0x06ef, 0x3 );        
	XC9080_I2C_WRITE(0x06f0, 0x3 );        
	XC9080_I2C_WRITE(0x06f1, 0x3 );        
	XC9080_I2C_WRITE(0x06f2, 0x3 );        
	XC9080_I2C_WRITE(0x06f3, 0x5 );        
	XC9080_I2C_WRITE(0x06f4, 0x9 );        
	XC9080_I2C_WRITE(0x06f5, 0x3 );        
	XC9080_I2C_WRITE(0x06f6, 0x1 );        
	XC9080_I2C_WRITE(0x06f7, 0x1 );        
	XC9080_I2C_WRITE(0x06f8, 0x1 );        
	XC9080_I2C_WRITE(0x06f9, 0x1 );        
	XC9080_I2C_WRITE(0x06fa, 0x1 );        
	XC9080_I2C_WRITE(0x06fb, 0x2 );        
	XC9080_I2C_WRITE(0x06fc, 0x4 );        
	XC9080_I2C_WRITE(0x06fd, 0x1 );        
	XC9080_I2C_WRITE(0x06fe, 0x0 );        
	XC9080_I2C_WRITE(0x06ff, 0x0 );        
	XC9080_I2C_WRITE(0x0700, 0x0 );        
	XC9080_I2C_WRITE(0x0701, 0x0 );        
	XC9080_I2C_WRITE(0x0702, 0x0 );        
	XC9080_I2C_WRITE(0x0703, 0x1 );        
	XC9080_I2C_WRITE(0x0704, 0x1 );        
	XC9080_I2C_WRITE(0x0705, 0x1 );        
	XC9080_I2C_WRITE(0x0706, 0x0 );        
	XC9080_I2C_WRITE(0x0707, 0x0 );        
	XC9080_I2C_WRITE(0x0708, 0x0 );        
	XC9080_I2C_WRITE(0x0709, 0x0 );        
	XC9080_I2C_WRITE(0x070a, 0x0 );        
	XC9080_I2C_WRITE(0x070b, 0x1 );        
	XC9080_I2C_WRITE(0x070c, 0x2 );        
	XC9080_I2C_WRITE(0x070d, 0x3 );        
	XC9080_I2C_WRITE(0x070e, 0x1 );        
	XC9080_I2C_WRITE(0x070f, 0x1 );        
	XC9080_I2C_WRITE(0x0710, 0x1 );        
	XC9080_I2C_WRITE(0x0711, 0x1 );        
	XC9080_I2C_WRITE(0x0712, 0x1 );        
	XC9080_I2C_WRITE(0x0713, 0x2 );        
	XC9080_I2C_WRITE(0x0714, 0x3 );        
	XC9080_I2C_WRITE(0x0715, 0x9 );        
	XC9080_I2C_WRITE(0x0716, 0x6 );        
	XC9080_I2C_WRITE(0x0717, 0x3 );        
	XC9080_I2C_WRITE(0x0718, 0x3 );        
	XC9080_I2C_WRITE(0x0719, 0x3 );        
	XC9080_I2C_WRITE(0x071a, 0x4 );        
	XC9080_I2C_WRITE(0x071b, 0x5 );        
	XC9080_I2C_WRITE(0x071c, 0xb );        
	XC9080_I2C_WRITE(0x071d, 0x3f);        
	XC9080_I2C_WRITE(0x071e, 0x21);        
	XC9080_I2C_WRITE(0x071f, 0x16);        
	XC9080_I2C_WRITE(0x0720, 0x12);        
	XC9080_I2C_WRITE(0x0721, 0x11);        
	XC9080_I2C_WRITE(0x0722, 0x14);        
	XC9080_I2C_WRITE(0x0723, 0x20);        
	XC9080_I2C_WRITE(0x0724, 0x28);        
	XC9080_I2C_WRITE(0x0725, 0xf );        
	XC9080_I2C_WRITE(0x0726, 0x1a);        
	XC9080_I2C_WRITE(0x0727, 0x1b);        
	XC9080_I2C_WRITE(0x0728, 0x1c);        
	XC9080_I2C_WRITE(0x0729, 0x1c);        
	XC9080_I2C_WRITE(0x072a, 0x1a);        
	XC9080_I2C_WRITE(0x072b, 0x18);        
	XC9080_I2C_WRITE(0x072c, 0x14);        
	XC9080_I2C_WRITE(0x072d, 0x1d);        
	XC9080_I2C_WRITE(0x072e, 0x1e);        
	XC9080_I2C_WRITE(0x072f, 0x1f);        
	XC9080_I2C_WRITE(0x0730, 0x1f);        
	XC9080_I2C_WRITE(0x0731, 0x1f);        
	XC9080_I2C_WRITE(0x0732, 0x20);        
	XC9080_I2C_WRITE(0x0733, 0x1f);        
	XC9080_I2C_WRITE(0x0734, 0x1e);        
	XC9080_I2C_WRITE(0x0735, 0x1d);        
	XC9080_I2C_WRITE(0x0736, 0x20);        
	XC9080_I2C_WRITE(0x0737, 0x1f);        
	XC9080_I2C_WRITE(0x0738, 0x20);        
	XC9080_I2C_WRITE(0x0739, 0x20);        
	XC9080_I2C_WRITE(0x073a, 0x20);        
	XC9080_I2C_WRITE(0x073b, 0x1f);        
	XC9080_I2C_WRITE(0x073c, 0x20);        
	XC9080_I2C_WRITE(0x073d, 0x1e);        
	XC9080_I2C_WRITE(0x073e, 0x20);        
	XC9080_I2C_WRITE(0x073f, 0x20);        
	XC9080_I2C_WRITE(0x0740, 0x20);        
	XC9080_I2C_WRITE(0x0741, 0x20);        
	XC9080_I2C_WRITE(0x0742, 0x20);        
	XC9080_I2C_WRITE(0x0743, 0x20);        
	XC9080_I2C_WRITE(0x0744, 0x21);        
	XC9080_I2C_WRITE(0x0745, 0x1e);        
	XC9080_I2C_WRITE(0x0746, 0x20);        
	XC9080_I2C_WRITE(0x0747, 0x20);        
	XC9080_I2C_WRITE(0x0748, 0x20);        
	XC9080_I2C_WRITE(0x0749, 0x20);        
	XC9080_I2C_WRITE(0x074a, 0x21);        
	XC9080_I2C_WRITE(0x074b, 0x21);        
	XC9080_I2C_WRITE(0x074c, 0x20);        
	XC9080_I2C_WRITE(0x074d, 0x1d);        
	XC9080_I2C_WRITE(0x074e, 0x20);        
	XC9080_I2C_WRITE(0x074f, 0x20);        
	XC9080_I2C_WRITE(0x0750, 0x20);        
	XC9080_I2C_WRITE(0x0751, 0x20);        
	XC9080_I2C_WRITE(0x0752, 0x20);        
	XC9080_I2C_WRITE(0x0753, 0x20);        
	XC9080_I2C_WRITE(0x0754, 0x20);        
	XC9080_I2C_WRITE(0x0755, 0x1c);        
	XC9080_I2C_WRITE(0x0756, 0x1f);        
	XC9080_I2C_WRITE(0x0757, 0x1f);        
	XC9080_I2C_WRITE(0x0758, 0x20);        
	XC9080_I2C_WRITE(0x0759, 0x20);        
	XC9080_I2C_WRITE(0x075a, 0x1f);        
	XC9080_I2C_WRITE(0x075b, 0x20);        
	XC9080_I2C_WRITE(0x075c, 0x1d);        
	XC9080_I2C_WRITE(0x075d, 0xe );        
	XC9080_I2C_WRITE(0x075e, 0x18);        
	XC9080_I2C_WRITE(0x075f, 0x1a);        
	XC9080_I2C_WRITE(0x0760, 0x1b);        
	XC9080_I2C_WRITE(0x0761, 0x1b);        
	XC9080_I2C_WRITE(0x0762, 0x1b);        
	XC9080_I2C_WRITE(0x0763, 0x17);        
	XC9080_I2C_WRITE(0x0764, 0x1b);        
	XC9080_I2C_WRITE(0x0765, 0x1f);        
	XC9080_I2C_WRITE(0x0766, 0x22);        
	XC9080_I2C_WRITE(0x0767, 0x22);        
	XC9080_I2C_WRITE(0x0768, 0x23);        
	XC9080_I2C_WRITE(0x0769, 0x24);        
	XC9080_I2C_WRITE(0x076a, 0x24);        
	XC9080_I2C_WRITE(0x076b, 0x23);        
	XC9080_I2C_WRITE(0x076c, 0x22);        
	XC9080_I2C_WRITE(0x076d, 0x25);        
	XC9080_I2C_WRITE(0x076e, 0x23);        
	XC9080_I2C_WRITE(0x076f, 0x23);        
	XC9080_I2C_WRITE(0x0770, 0x22);        
	XC9080_I2C_WRITE(0x0771, 0x22);        
	XC9080_I2C_WRITE(0x0772, 0x22);        
	XC9080_I2C_WRITE(0x0773, 0x22);        
	XC9080_I2C_WRITE(0x0774, 0x23);        
	XC9080_I2C_WRITE(0x0775, 0x24);        
	XC9080_I2C_WRITE(0x0776, 0x21);        
	XC9080_I2C_WRITE(0x0777, 0x21);        
	XC9080_I2C_WRITE(0x0778, 0x21);        
	XC9080_I2C_WRITE(0x0779, 0x21);        
	XC9080_I2C_WRITE(0x077a, 0x21);        
	XC9080_I2C_WRITE(0x077b, 0x21);        
	XC9080_I2C_WRITE(0x077c, 0x22);        
	XC9080_I2C_WRITE(0x077d, 0x23);        
	XC9080_I2C_WRITE(0x077e, 0x21);        
	XC9080_I2C_WRITE(0x077f, 0x21);        
	XC9080_I2C_WRITE(0x0780, 0x20);        
	XC9080_I2C_WRITE(0x0781, 0x20);        
	XC9080_I2C_WRITE(0x0782, 0x20);        
	XC9080_I2C_WRITE(0x0783, 0x20);        
	XC9080_I2C_WRITE(0x0784, 0x21);        
	XC9080_I2C_WRITE(0x0785, 0x23);        
	XC9080_I2C_WRITE(0x0786, 0x21);        
	XC9080_I2C_WRITE(0x0787, 0x20);        
	XC9080_I2C_WRITE(0x0788, 0x20);        
	XC9080_I2C_WRITE(0x0789, 0x20);        
	XC9080_I2C_WRITE(0x078a, 0x20);        
	XC9080_I2C_WRITE(0x078b, 0x20);        
	XC9080_I2C_WRITE(0x078c, 0x21);        
	XC9080_I2C_WRITE(0x078d, 0x24);        
	XC9080_I2C_WRITE(0x078e, 0x21);        
	XC9080_I2C_WRITE(0x078f, 0x21);        
	XC9080_I2C_WRITE(0x0790, 0x21);        
	XC9080_I2C_WRITE(0x0791, 0x21);        
	XC9080_I2C_WRITE(0x0792, 0x21);        
	XC9080_I2C_WRITE(0x0793, 0x21);        
	XC9080_I2C_WRITE(0x0794, 0x22);        
	XC9080_I2C_WRITE(0x0795, 0x24);        
	XC9080_I2C_WRITE(0x0796, 0x22);        
	XC9080_I2C_WRITE(0x0797, 0x22);        
	XC9080_I2C_WRITE(0x0798, 0x22);        
	XC9080_I2C_WRITE(0x0799, 0x22);        
	XC9080_I2C_WRITE(0x079a, 0x22);        
	XC9080_I2C_WRITE(0x079b, 0x22);        
	XC9080_I2C_WRITE(0x079c, 0x23);        
	XC9080_I2C_WRITE(0x079d, 0x22);        
	XC9080_I2C_WRITE(0x079e, 0x22);        
	XC9080_I2C_WRITE(0x079f, 0x24);        
	XC9080_I2C_WRITE(0x07a0, 0x24);        
	XC9080_I2C_WRITE(0x07a1, 0x24);        
	XC9080_I2C_WRITE(0x07a2, 0x24);        
	XC9080_I2C_WRITE(0x07a3, 0x23);        
	XC9080_I2C_WRITE(0x07a4, 0x21);        
		 
	XC9080_I2C_WRITE(0x07a5, 0x3d);        //c_95);20161101_v2
	XC9080_I2C_WRITE(0x07a6, 0x1e);        
	XC9080_I2C_WRITE(0x07a7, 0x15);        
	XC9080_I2C_WRITE(0x07a8, 0x11);        
	XC9080_I2C_WRITE(0x07a9, 0x10);        
	XC9080_I2C_WRITE(0x07aa, 0x13);        
	XC9080_I2C_WRITE(0x07ab, 0x1c);        
	XC9080_I2C_WRITE(0x07ac, 0x33);        
	XC9080_I2C_WRITE(0x07ad, 0x8 );        
	XC9080_I2C_WRITE(0x07ae, 0x5 );        
	XC9080_I2C_WRITE(0x07af, 0x3 );        
	XC9080_I2C_WRITE(0x07b0, 0x3 );        
	XC9080_I2C_WRITE(0x07b1, 0x3 );        
	XC9080_I2C_WRITE(0x07b2, 0x3 );        
	XC9080_I2C_WRITE(0x07b3, 0x5 );        
	XC9080_I2C_WRITE(0x07b4, 0x9 );        
	XC9080_I2C_WRITE(0x07b5, 0x3 );        
	XC9080_I2C_WRITE(0x07b6, 0x1 );        
	XC9080_I2C_WRITE(0x07b7, 0x0 );        
	XC9080_I2C_WRITE(0x07b8, 0x1 );        
	XC9080_I2C_WRITE(0x07b9, 0x1 );        
	XC9080_I2C_WRITE(0x07ba, 0x1 );        
	XC9080_I2C_WRITE(0x07bb, 0x2 );        
	XC9080_I2C_WRITE(0x07bc, 0x3 );        
	XC9080_I2C_WRITE(0x07bd, 0x1 );        
	XC9080_I2C_WRITE(0x07be, 0x0 );        
	XC9080_I2C_WRITE(0x07bf, 0x0 );        
	XC9080_I2C_WRITE(0x07c0, 0x0 );        
	XC9080_I2C_WRITE(0x07c1, 0x0 );        
	XC9080_I2C_WRITE(0x07c2, 0x0 );        
	XC9080_I2C_WRITE(0x07c3, 0x1 );        
	XC9080_I2C_WRITE(0x07c4, 0x1 );        
	XC9080_I2C_WRITE(0x07c5, 0x1 );        
	XC9080_I2C_WRITE(0x07c6, 0x0 );        
	XC9080_I2C_WRITE(0x07c7, 0x0 );        
	XC9080_I2C_WRITE(0x07c8, 0x0 );        
	XC9080_I2C_WRITE(0x07c9, 0x0 );        
	XC9080_I2C_WRITE(0x07ca, 0x0 );        
	XC9080_I2C_WRITE(0x07cb, 0x1 );        
	XC9080_I2C_WRITE(0x07cc, 0x2 );        
	XC9080_I2C_WRITE(0x07cd, 0x3 );        
	XC9080_I2C_WRITE(0x07ce, 0x1 );        
	XC9080_I2C_WRITE(0x07cf, 0x1 );        
	XC9080_I2C_WRITE(0x07d0, 0x1 );        
	XC9080_I2C_WRITE(0x07d1, 0x1 );        
	XC9080_I2C_WRITE(0x07d2, 0x1 );        
	XC9080_I2C_WRITE(0x07d3, 0x2 );        
	XC9080_I2C_WRITE(0x07d4, 0x3 );        
	XC9080_I2C_WRITE(0x07d5, 0x9 );        
	XC9080_I2C_WRITE(0x07d6, 0x6 );        
	XC9080_I2C_WRITE(0x07d7, 0x3 );        
	XC9080_I2C_WRITE(0x07d8, 0x3 );        
	XC9080_I2C_WRITE(0x07d9, 0x3 );        
	XC9080_I2C_WRITE(0x07da, 0x4 );        
	XC9080_I2C_WRITE(0x07db, 0x5 );        
	XC9080_I2C_WRITE(0x07dc, 0xa );        
	XC9080_I2C_WRITE(0x07dd, 0x3f);        
	XC9080_I2C_WRITE(0x07de, 0x21);        
	XC9080_I2C_WRITE(0x07df, 0x16);        
	XC9080_I2C_WRITE(0x07e0, 0x12);        
	XC9080_I2C_WRITE(0x07e1, 0x11);        
	XC9080_I2C_WRITE(0x07e2, 0x14);        
	XC9080_I2C_WRITE(0x07e3, 0x1f);        
	XC9080_I2C_WRITE(0x07e4, 0x2e);        
	XC9080_I2C_WRITE(0x07e5, 0x13);        
	XC9080_I2C_WRITE(0x07e6, 0x1a);        
	XC9080_I2C_WRITE(0x07e7, 0x1b);        
	XC9080_I2C_WRITE(0x07e8, 0x1c);        
	XC9080_I2C_WRITE(0x07e9, 0x1c);        
	XC9080_I2C_WRITE(0x07ea, 0x1a);        
	XC9080_I2C_WRITE(0x07eb, 0x18);        
	XC9080_I2C_WRITE(0x07ec, 0x17);        
	XC9080_I2C_WRITE(0x07ed, 0x1c);        
	XC9080_I2C_WRITE(0x07ee, 0x1e);        
	XC9080_I2C_WRITE(0x07ef, 0x1f);        
	XC9080_I2C_WRITE(0x07f0, 0x1f);        
	XC9080_I2C_WRITE(0x07f1, 0x1f);        
	XC9080_I2C_WRITE(0x07f2, 0x1f);        
	XC9080_I2C_WRITE(0x07f3, 0x1f);        
	XC9080_I2C_WRITE(0x07f4, 0x1d);        
	XC9080_I2C_WRITE(0x07f5, 0x1d);        
	XC9080_I2C_WRITE(0x07f6, 0x1f);        
	XC9080_I2C_WRITE(0x07f7, 0x1f);        
	XC9080_I2C_WRITE(0x07f8, 0x20);        
	XC9080_I2C_WRITE(0x07f9, 0x20);        
	XC9080_I2C_WRITE(0x07fa, 0x20);        
	XC9080_I2C_WRITE(0x07fb, 0x1f);        
	XC9080_I2C_WRITE(0x07fc, 0x1f);        
	XC9080_I2C_WRITE(0x07fd, 0x1e);        
	XC9080_I2C_WRITE(0x07fe, 0x20);        
	XC9080_I2C_WRITE(0x07ff, 0x20);        
	XC9080_I2C_WRITE(0x0800, 0x21);        
	XC9080_I2C_WRITE(0x0801, 0x20);        
	XC9080_I2C_WRITE(0x0802, 0x21);        
	XC9080_I2C_WRITE(0x0803, 0x20);        
	XC9080_I2C_WRITE(0x0804, 0x20);        
	XC9080_I2C_WRITE(0x0805, 0x1e);        
	XC9080_I2C_WRITE(0x0806, 0x20);        
	XC9080_I2C_WRITE(0x0807, 0x20);        
	XC9080_I2C_WRITE(0x0808, 0x21);        
	XC9080_I2C_WRITE(0x0809, 0x21);        
	XC9080_I2C_WRITE(0x080a, 0x21);        
	XC9080_I2C_WRITE(0x080b, 0x20);        
	XC9080_I2C_WRITE(0x080c, 0x20);        
	XC9080_I2C_WRITE(0x080d, 0x1d);        
	XC9080_I2C_WRITE(0x080e, 0x1f);        
	XC9080_I2C_WRITE(0x080f, 0x20);        
	XC9080_I2C_WRITE(0x0810, 0x20);        
	XC9080_I2C_WRITE(0x0811, 0x20);        
	XC9080_I2C_WRITE(0x0812, 0x20);        
	XC9080_I2C_WRITE(0x0813, 0x20);        
	XC9080_I2C_WRITE(0x0814, 0x1f);        
	XC9080_I2C_WRITE(0x0815, 0x1c);        
	XC9080_I2C_WRITE(0x0816, 0x1e);        
	XC9080_I2C_WRITE(0x0817, 0x1f);        
	XC9080_I2C_WRITE(0x0818, 0x1f);        
	XC9080_I2C_WRITE(0x0819, 0x1f);        
	XC9080_I2C_WRITE(0x081a, 0x1f);        
	XC9080_I2C_WRITE(0x081b, 0x1f);        
	XC9080_I2C_WRITE(0x081c, 0x1d);        
	XC9080_I2C_WRITE(0x081d, 0x11);        
	XC9080_I2C_WRITE(0x081e, 0x18);        
	XC9080_I2C_WRITE(0x081f, 0x1a);        
	XC9080_I2C_WRITE(0x0820, 0x1b);        
	XC9080_I2C_WRITE(0x0821, 0x1b);        
	XC9080_I2C_WRITE(0x0822, 0x1b);        
	XC9080_I2C_WRITE(0x0823, 0x18);        
	XC9080_I2C_WRITE(0x0824, 0x19);        
	XC9080_I2C_WRITE(0x0825, 0x1a);        
	XC9080_I2C_WRITE(0x0826, 0x1e);        
	XC9080_I2C_WRITE(0x0827, 0x1f);        
	XC9080_I2C_WRITE(0x0828, 0x20);        
	XC9080_I2C_WRITE(0x0829, 0x20);        
	XC9080_I2C_WRITE(0x082a, 0x20);        
	XC9080_I2C_WRITE(0x082b, 0x1f);        
	XC9080_I2C_WRITE(0x082c, 0x1e);        
	XC9080_I2C_WRITE(0x082d, 0x22);        
	XC9080_I2C_WRITE(0x082e, 0x21);        
	XC9080_I2C_WRITE(0x082f, 0x21);        
	XC9080_I2C_WRITE(0x0830, 0x21);        
	XC9080_I2C_WRITE(0x0831, 0x21);        
	XC9080_I2C_WRITE(0x0832, 0x21);        
	XC9080_I2C_WRITE(0x0833, 0x21);        
	XC9080_I2C_WRITE(0x0834, 0x21);        
	XC9080_I2C_WRITE(0x0835, 0x22);        
	XC9080_I2C_WRITE(0x0836, 0x21);        
	XC9080_I2C_WRITE(0x0837, 0x21);        
	XC9080_I2C_WRITE(0x0838, 0x21);        
	XC9080_I2C_WRITE(0x0839, 0x21);        
	XC9080_I2C_WRITE(0x083a, 0x21);        
	XC9080_I2C_WRITE(0x083b, 0x21);        
	XC9080_I2C_WRITE(0x083c, 0x21);        
	XC9080_I2C_WRITE(0x083d, 0x22);        
	XC9080_I2C_WRITE(0x083e, 0x20);        
	XC9080_I2C_WRITE(0x083f, 0x20);        
	XC9080_I2C_WRITE(0x0840, 0x20);        
	XC9080_I2C_WRITE(0x0841, 0x20);        
	XC9080_I2C_WRITE(0x0842, 0x20);        
	XC9080_I2C_WRITE(0x0843, 0x20);        
	XC9080_I2C_WRITE(0x0844, 0x21);        
	XC9080_I2C_WRITE(0x0845, 0x22);        
	XC9080_I2C_WRITE(0x0846, 0x20);        
	XC9080_I2C_WRITE(0x0847, 0x20);        
	XC9080_I2C_WRITE(0x0848, 0x20);        
	XC9080_I2C_WRITE(0x0849, 0x20);        
	XC9080_I2C_WRITE(0x084a, 0x20);        
	XC9080_I2C_WRITE(0x084b, 0x20);        
	XC9080_I2C_WRITE(0x084c, 0x21);        
	XC9080_I2C_WRITE(0x084d, 0x22);        
	XC9080_I2C_WRITE(0x084e, 0x20);        
	XC9080_I2C_WRITE(0x084f, 0x20);        
	XC9080_I2C_WRITE(0x0850, 0x20);        
	XC9080_I2C_WRITE(0x0851, 0x21);        
	XC9080_I2C_WRITE(0x0852, 0x20);        
	XC9080_I2C_WRITE(0x0853, 0x21);        
	XC9080_I2C_WRITE(0x0854, 0x20);        
	XC9080_I2C_WRITE(0x0855, 0x21);        
	XC9080_I2C_WRITE(0x0856, 0x20);        
	XC9080_I2C_WRITE(0x0857, 0x21);        
	XC9080_I2C_WRITE(0x0858, 0x21);        
	XC9080_I2C_WRITE(0x0859, 0x21);        
	XC9080_I2C_WRITE(0x085a, 0x21);        
	XC9080_I2C_WRITE(0x085b, 0x21);        
	XC9080_I2C_WRITE(0x085c, 0x22);        
	XC9080_I2C_WRITE(0x085d, 0x1c);        
	XC9080_I2C_WRITE(0x085e, 0x1e);        
	XC9080_I2C_WRITE(0x085f, 0x20);        
	XC9080_I2C_WRITE(0x0860, 0x20);        
	XC9080_I2C_WRITE(0x0861, 0x21);        
	XC9080_I2C_WRITE(0x0862, 0x20);        
	XC9080_I2C_WRITE(0x0863, 0x20);        
	XC9080_I2C_WRITE(0x0864, 0x1a);        
		 
	XC9080_I2C_WRITE(0x0865, 0x3c);        //d_95);20161101_v2
	XC9080_I2C_WRITE(0x0866, 0x1e);        
	XC9080_I2C_WRITE(0x0867, 0x15);        
	XC9080_I2C_WRITE(0x0868, 0x10);        
	XC9080_I2C_WRITE(0x0869, 0x10);        
	XC9080_I2C_WRITE(0x086a, 0x13);        
	XC9080_I2C_WRITE(0x086b, 0x1b);        
	XC9080_I2C_WRITE(0x086c, 0x32);        
	XC9080_I2C_WRITE(0x086d, 0x8 );        
	XC9080_I2C_WRITE(0x086e, 0x5 );        
	XC9080_I2C_WRITE(0x086f, 0x3 );        
	XC9080_I2C_WRITE(0x0870, 0x3 );        
	XC9080_I2C_WRITE(0x0871, 0x3 );        
	XC9080_I2C_WRITE(0x0872, 0x3 );        
	XC9080_I2C_WRITE(0x0873, 0x5 );        
	XC9080_I2C_WRITE(0x0874, 0x9 );        
	XC9080_I2C_WRITE(0x0875, 0x3 );        
	XC9080_I2C_WRITE(0x0876, 0x1 );        
	XC9080_I2C_WRITE(0x0877, 0x0 );        
	XC9080_I2C_WRITE(0x0878, 0x1 );        
	XC9080_I2C_WRITE(0x0879, 0x1 );        
	XC9080_I2C_WRITE(0x087a, 0x1 );        
	XC9080_I2C_WRITE(0x087b, 0x2 );        
	XC9080_I2C_WRITE(0x087c, 0x3 );        
	XC9080_I2C_WRITE(0x087d, 0x1 );        
	XC9080_I2C_WRITE(0x087e, 0x0 );        
	XC9080_I2C_WRITE(0x087f, 0x0 );        
	XC9080_I2C_WRITE(0x0880, 0x0 );        
	XC9080_I2C_WRITE(0x0881, 0x0 );        
	XC9080_I2C_WRITE(0x0882, 0x1 );        
	XC9080_I2C_WRITE(0x0883, 0x1 );        
	XC9080_I2C_WRITE(0x0884, 0x1 );        
	XC9080_I2C_WRITE(0x0885, 0x1 );        
	XC9080_I2C_WRITE(0x0886, 0x0 );        
	XC9080_I2C_WRITE(0x0887, 0x0 );        
	XC9080_I2C_WRITE(0x0888, 0x0 );        
	XC9080_I2C_WRITE(0x0889, 0x0 );        
	XC9080_I2C_WRITE(0x088a, 0x1 );        
	XC9080_I2C_WRITE(0x088b, 0x1 );        
	XC9080_I2C_WRITE(0x088c, 0x2 );        
	XC9080_I2C_WRITE(0x088d, 0x3 );        
	XC9080_I2C_WRITE(0x088e, 0x1 );        
	XC9080_I2C_WRITE(0x088f, 0x1 );        
	XC9080_I2C_WRITE(0x0890, 0x1 );        
	XC9080_I2C_WRITE(0x0891, 0x1 );        
	XC9080_I2C_WRITE(0x0892, 0x1 );        
	XC9080_I2C_WRITE(0x0893, 0x2 );        
	XC9080_I2C_WRITE(0x0894, 0x3 );        
	XC9080_I2C_WRITE(0x0895, 0x9 );        
	XC9080_I2C_WRITE(0x0896, 0x5 );        
	XC9080_I2C_WRITE(0x0897, 0x3 );        
	XC9080_I2C_WRITE(0x0898, 0x3 );        
	XC9080_I2C_WRITE(0x0899, 0x3 );        
	XC9080_I2C_WRITE(0x089a, 0x4 );        
	XC9080_I2C_WRITE(0x089b, 0x5 );        
	XC9080_I2C_WRITE(0x089c, 0xa );        
	XC9080_I2C_WRITE(0x089d, 0x3f);        
	XC9080_I2C_WRITE(0x089e, 0x20);        
	XC9080_I2C_WRITE(0x089f, 0x16);        
	XC9080_I2C_WRITE(0x08a0, 0x12);        
	XC9080_I2C_WRITE(0x08a1, 0x11);        
	XC9080_I2C_WRITE(0x08a2, 0x14);        
	XC9080_I2C_WRITE(0x08a3, 0x1f);        
	XC9080_I2C_WRITE(0x08a4, 0x2e);        
	XC9080_I2C_WRITE(0x08a5, 0x18);        
	XC9080_I2C_WRITE(0x08a6, 0x1c);        
	XC9080_I2C_WRITE(0x08a7, 0x1d);        
	XC9080_I2C_WRITE(0x08a8, 0x1d);        
	XC9080_I2C_WRITE(0x08a9, 0x1d);        
	XC9080_I2C_WRITE(0x08aa, 0x1c);        
	XC9080_I2C_WRITE(0x08ab, 0x1b);        
	XC9080_I2C_WRITE(0x08ac, 0x1b);        
	XC9080_I2C_WRITE(0x08ad, 0x1d);        
	XC9080_I2C_WRITE(0x08ae, 0x1e);        
	XC9080_I2C_WRITE(0x08af, 0x1f);        
	XC9080_I2C_WRITE(0x08b0, 0x1f);        
	XC9080_I2C_WRITE(0x08b1, 0x1f);        
	XC9080_I2C_WRITE(0x08b2, 0x1f);        
	XC9080_I2C_WRITE(0x08b3, 0x1f);        
	XC9080_I2C_WRITE(0x08b4, 0x1e);        
	XC9080_I2C_WRITE(0x08b5, 0x1e);        
	XC9080_I2C_WRITE(0x08b6, 0x1f);        
	XC9080_I2C_WRITE(0x08b7, 0x1f);        
	XC9080_I2C_WRITE(0x08b8, 0x20);        
	XC9080_I2C_WRITE(0x08b9, 0x20);        
	XC9080_I2C_WRITE(0x08ba, 0x20);        
	XC9080_I2C_WRITE(0x08bb, 0x1f);        
	XC9080_I2C_WRITE(0x08bc, 0x1f);        
	XC9080_I2C_WRITE(0x08bd, 0x1e);        
	XC9080_I2C_WRITE(0x08be, 0x20);        
	XC9080_I2C_WRITE(0x08bf, 0x20);        
	XC9080_I2C_WRITE(0x08c0, 0x20);        
	XC9080_I2C_WRITE(0x08c1, 0x20);        
	XC9080_I2C_WRITE(0x08c2, 0x20);        
	XC9080_I2C_WRITE(0x08c3, 0x20);        
	XC9080_I2C_WRITE(0x08c4, 0x1f);        
	XC9080_I2C_WRITE(0x08c5, 0x1f);        
	XC9080_I2C_WRITE(0x08c6, 0x1f);        
	XC9080_I2C_WRITE(0x08c7, 0x20);        
	XC9080_I2C_WRITE(0x08c8, 0x20);        
	XC9080_I2C_WRITE(0x08c9, 0x20);        
	XC9080_I2C_WRITE(0x08ca, 0x20);        
	XC9080_I2C_WRITE(0x08cb, 0x20);        
	XC9080_I2C_WRITE(0x08cc, 0x20);        
	XC9080_I2C_WRITE(0x08cd, 0x1e);        
	XC9080_I2C_WRITE(0x08ce, 0x1f);        
	XC9080_I2C_WRITE(0x08cf, 0x1f);        
	XC9080_I2C_WRITE(0x08d0, 0x20);        
	XC9080_I2C_WRITE(0x08d1, 0x20);        
	XC9080_I2C_WRITE(0x08d2, 0x1f);        
	XC9080_I2C_WRITE(0x08d3, 0x1f);        
	XC9080_I2C_WRITE(0x08d4, 0x1e);        
	XC9080_I2C_WRITE(0x08d5, 0x1c);        
	XC9080_I2C_WRITE(0x08d6, 0x1e);        
	XC9080_I2C_WRITE(0x08d7, 0x1e);        
	XC9080_I2C_WRITE(0x08d8, 0x1f);        
	XC9080_I2C_WRITE(0x08d9, 0x1f);        
	XC9080_I2C_WRITE(0x08da, 0x1f);        
	XC9080_I2C_WRITE(0x08db, 0x1e);        
	XC9080_I2C_WRITE(0x08dc, 0x1e);        
	XC9080_I2C_WRITE(0x08dd, 0x17);        
	XC9080_I2C_WRITE(0x08de, 0x1b);        
	XC9080_I2C_WRITE(0x08df, 0x1c);        
	XC9080_I2C_WRITE(0x08e0, 0x1c);        
	XC9080_I2C_WRITE(0x08e1, 0x1c);        
	XC9080_I2C_WRITE(0x08e2, 0x1c);        
	XC9080_I2C_WRITE(0x08e3, 0x1b);        
	XC9080_I2C_WRITE(0x08e4, 0x1d);        
	XC9080_I2C_WRITE(0x08e5, 0x19);        
	XC9080_I2C_WRITE(0x08e6, 0x1f);        
	XC9080_I2C_WRITE(0x08e7, 0x1f);        
	XC9080_I2C_WRITE(0x08e8, 0x21);        
	XC9080_I2C_WRITE(0x08e9, 0x21);        
	XC9080_I2C_WRITE(0x08ea, 0x21);        
	XC9080_I2C_WRITE(0x08eb, 0x1f);        
	XC9080_I2C_WRITE(0x08ec, 0x1e);        
	XC9080_I2C_WRITE(0x08ed, 0x22);        
	XC9080_I2C_WRITE(0x08ee, 0x21);        
	XC9080_I2C_WRITE(0x08ef, 0x22);        
	XC9080_I2C_WRITE(0x08f0, 0x22);        
	XC9080_I2C_WRITE(0x08f1, 0x22);        
	XC9080_I2C_WRITE(0x08f2, 0x22);        
	XC9080_I2C_WRITE(0x08f3, 0x22);        
	XC9080_I2C_WRITE(0x08f4, 0x21);        
	XC9080_I2C_WRITE(0x08f5, 0x23);        
	XC9080_I2C_WRITE(0x08f6, 0x21);        
	XC9080_I2C_WRITE(0x08f7, 0x21);        
	XC9080_I2C_WRITE(0x08f8, 0x21);        
	XC9080_I2C_WRITE(0x08f9, 0x21);        
	XC9080_I2C_WRITE(0x08fa, 0x21);        
	XC9080_I2C_WRITE(0x08fb, 0x21);        
	XC9080_I2C_WRITE(0x08fc, 0x22);        
	XC9080_I2C_WRITE(0x08fd, 0x23);        
	XC9080_I2C_WRITE(0x08fe, 0x20);        
	XC9080_I2C_WRITE(0x08ff, 0x21);        
	XC9080_I2C_WRITE(0x0900, 0x20);        
	XC9080_I2C_WRITE(0x0901, 0x20);        
	XC9080_I2C_WRITE(0x0902, 0x20);        
	XC9080_I2C_WRITE(0x0903, 0x21);        
	XC9080_I2C_WRITE(0x0904, 0x21);        
	XC9080_I2C_WRITE(0x0905, 0x22);        
	XC9080_I2C_WRITE(0x0906, 0x20);        
	XC9080_I2C_WRITE(0x0907, 0x20);        
	XC9080_I2C_WRITE(0x0908, 0x20);        
	XC9080_I2C_WRITE(0x0909, 0x20);        
	XC9080_I2C_WRITE(0x090a, 0x20);        
	XC9080_I2C_WRITE(0x090b, 0x20);        
	XC9080_I2C_WRITE(0x090c, 0x21);        
	XC9080_I2C_WRITE(0x090d, 0x23);        
	XC9080_I2C_WRITE(0x090e, 0x21);        
	XC9080_I2C_WRITE(0x090f, 0x21);        
	XC9080_I2C_WRITE(0x0910, 0x21);        
	XC9080_I2C_WRITE(0x0911, 0x21);        
	XC9080_I2C_WRITE(0x0912, 0x21);        
	XC9080_I2C_WRITE(0x0913, 0x21);        
	XC9080_I2C_WRITE(0x0914, 0x20);        
	XC9080_I2C_WRITE(0x0915, 0x22);        
	XC9080_I2C_WRITE(0x0916, 0x21);        
	XC9080_I2C_WRITE(0x0917, 0x21);        
	XC9080_I2C_WRITE(0x0918, 0x21);        
	XC9080_I2C_WRITE(0x0919, 0x21);        
	XC9080_I2C_WRITE(0x091a, 0x21);        
	XC9080_I2C_WRITE(0x091b, 0x21);        
	XC9080_I2C_WRITE(0x091c, 0x22);        
	XC9080_I2C_WRITE(0x091d, 0x1b);        
	XC9080_I2C_WRITE(0x091e, 0x1e);        
	XC9080_I2C_WRITE(0x091f, 0x20);        
	XC9080_I2C_WRITE(0x0920, 0x21);        
	XC9080_I2C_WRITE(0x0921, 0x22);        
	XC9080_I2C_WRITE(0x0922, 0x20);        
	XC9080_I2C_WRITE(0x0923, 0x20);        
	XC9080_I2C_WRITE(0x0924, 0x1b);        
		 
	//ISP0 AWB
	XC9080_I2C_WRITE(0xfffe, 0x14); //0805
	XC9080_I2C_WRITE(0x0248, 0x02);  //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
	XC9080_I2C_WRITE(0x0282, 0x06);   //int B gain
	XC9080_I2C_WRITE(0x0283, 0x00);  
	XC9080_I2C_WRITE(0x0286, 0x04);   //int Gb gain
	XC9080_I2C_WRITE(0x0287, 0x00); 
	XC9080_I2C_WRITE(0x028a, 0x04);   //int Gr gain
	XC9080_I2C_WRITE(0x028b, 0x00);
	XC9080_I2C_WRITE(0x028e, 0x04);   //int R gain
	XC9080_I2C_WRITE(0x028f, 0x04);
	XC9080_I2C_WRITE(0x02b6, 0x06);    //B_temp      
	XC9080_I2C_WRITE(0x02b7, 0x00);
	XC9080_I2C_WRITE(0x02ba, 0x04);    //G_ temp          
	XC9080_I2C_WRITE(0x02bb, 0x00);      
	XC9080_I2C_WRITE(0x02be, 0x04);   //R_temp      
	XC9080_I2C_WRITE(0x02bf, 0x04);
	XC9080_I2C_WRITE(0xfffe, 0x14);
	XC9080_I2C_WRITE(0x0248, 0x01);  //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
	XC9080_I2C_WRITE(0x0249, 0x01);  //AWBFlexiMap_en
	XC9080_I2C_WRITE(0x024a, 0x00);  //AWBMove_en
	XC9080_I2C_WRITE(0x027a, 0x01);  //nCTBasedMinNum    //20
	XC9080_I2C_WRITE(0x027b, 0x00);
	XC9080_I2C_WRITE(0x027c, 0x0f);
	XC9080_I2C_WRITE(0x027d, 0xff);//nMaxAWBGain
	XC9080_I2C_WRITE(0xfffe, 0x30);
	XC9080_I2C_WRITE(0x0708, 0x02); 
	XC9080_I2C_WRITE(0x0709, 0xa0); 
	XC9080_I2C_WRITE(0x070a, 0x00); 
	XC9080_I2C_WRITE(0x070b, 0x0c); 
	XC9080_I2C_WRITE(0x071c, 0x0a); //simple awb
	//XC9080_I2C_WRITE(0x0003, 0x11);   //bit[4]:awb_en  bit[0]:awb_gain_en
	XC9080_I2C_WRITE(0xfffe, 0x30);
	XC9080_I2C_WRITE(0x0730, 0x8e); 
	XC9080_I2C_WRITE(0x0731, 0xb4); 
	XC9080_I2C_WRITE(0x0732, 0x40); 
	XC9080_I2C_WRITE(0x0733, 0x52); 
	XC9080_I2C_WRITE(0x0734, 0x70); 
	XC9080_I2C_WRITE(0x0735, 0x98); 
	XC9080_I2C_WRITE(0x0736, 0x60); 
	XC9080_I2C_WRITE(0x0737, 0x70); 
	XC9080_I2C_WRITE(0x0738, 0x4f); 
	XC9080_I2C_WRITE(0x0739, 0x5d); 
	XC9080_I2C_WRITE(0x073a, 0x6e); 
	XC9080_I2C_WRITE(0x073b, 0x80); 
	XC9080_I2C_WRITE(0x073c, 0x74); 
	XC9080_I2C_WRITE(0x073d, 0x93); 
	XC9080_I2C_WRITE(0x073e, 0x50); 
	XC9080_I2C_WRITE(0x073f, 0x61); 
	XC9080_I2C_WRITE(0x0740, 0x5c); 
	XC9080_I2C_WRITE(0x0741, 0x72); 
	XC9080_I2C_WRITE(0x0742, 0x63); 
	XC9080_I2C_WRITE(0x0743, 0x85); 
	XC9080_I2C_WRITE(0x0744, 0x00); 
	XC9080_I2C_WRITE(0x0745, 0x00); 
	XC9080_I2C_WRITE(0x0746, 0x00); 
	XC9080_I2C_WRITE(0x0747, 0x00); 
	XC9080_I2C_WRITE(0x0748, 0x00); 
	XC9080_I2C_WRITE(0x0749, 0x00); 
	XC9080_I2C_WRITE(0x074a, 0x00); 
	XC9080_I2C_WRITE(0x074b, 0x00); 
	XC9080_I2C_WRITE(0x074c, 0x00); 
	XC9080_I2C_WRITE(0x074d, 0x00); 
	XC9080_I2C_WRITE(0x074e, 0x00); 
	XC9080_I2C_WRITE(0x074f, 0x00); 
	XC9080_I2C_WRITE(0x0750, 0x00); 
	XC9080_I2C_WRITE(0x0751, 0x00); 
	XC9080_I2C_WRITE(0x0752, 0x00); 
	XC9080_I2C_WRITE(0x0753, 0x00); 
	XC9080_I2C_WRITE(0x0754, 0x00); 
	XC9080_I2C_WRITE(0x0755, 0x00); 
	XC9080_I2C_WRITE(0x0756, 0x00); 
	XC9080_I2C_WRITE(0x0757, 0x00); 
	XC9080_I2C_WRITE(0x0758, 0x00); 
	XC9080_I2C_WRITE(0x0759, 0x00); 
	XC9080_I2C_WRITE(0x075a, 0x00); 
	XC9080_I2C_WRITE(0x075b, 0x00); 
	XC9080_I2C_WRITE(0x075c, 0x00); 
	XC9080_I2C_WRITE(0x075d, 0x00); 
	XC9080_I2C_WRITE(0x075e, 0x00); 
	XC9080_I2C_WRITE(0x075f, 0x00); 
	XC9080_I2C_WRITE(0x0760, 0x00); 
	XC9080_I2C_WRITE(0x0761, 0x00); 
	XC9080_I2C_WRITE(0x0762, 0x00); 
	XC9080_I2C_WRITE(0x0763, 0x00); 
	XC9080_I2C_WRITE(0x0764, 0x00); 
	XC9080_I2C_WRITE(0x0765, 0x00); 
	XC9080_I2C_WRITE(0x0766, 0x00); 
	XC9080_I2C_WRITE(0x0767, 0x00); 
	XC9080_I2C_WRITE(0x0768, 0x00); 
	XC9080_I2C_WRITE(0x0769, 0x00); 
	XC9080_I2C_WRITE(0x076a, 0x00); 
	XC9080_I2C_WRITE(0x076b, 0x00); 
	XC9080_I2C_WRITE(0x076c, 0x00); 
	XC9080_I2C_WRITE(0x076d, 0x00); 
	XC9080_I2C_WRITE(0x076e, 0x00); 
	XC9080_I2C_WRITE(0x076f, 0x00); 
	XC9080_I2C_WRITE(0x0770, 0x22); 
	XC9080_I2C_WRITE(0x0771, 0x21); 
	XC9080_I2C_WRITE(0x0772, 0x10); 
	XC9080_I2C_WRITE(0x0773, 0x00); 
	XC9080_I2C_WRITE(0x0774, 0x00); 
	XC9080_I2C_WRITE(0x0775, 0x00); 
	XC9080_I2C_WRITE(0x0776, 0x00); 
	XC9080_I2C_WRITE(0x0777, 0x00); 
	//ISP1 AE
	//// 1.  AE statistical //////
	//////////////////////////////
	XC9080_I2C_WRITE(0xfffe, 0x31);   // AE_avg
	XC9080_I2C_WRITE(0x1f00, 0x00);   // WIN start X
	XC9080_I2C_WRITE(0x1f01, 0x9f);
	XC9080_I2C_WRITE(0x1f02, 0x00);   // WIN stat Y
	XC9080_I2C_WRITE(0x1f03, 0x9f);
	XC9080_I2C_WRITE(0x1f04, 0x02);   // WIN width
	XC9080_I2C_WRITE(0x1f05, 0xee);
	XC9080_I2C_WRITE(0x1f06, 0x02);   // WIN height
	XC9080_I2C_WRITE(0x1f07, 0xee);
	XC9080_I2C_WRITE(0x1f08, 0x03);
	XC9080_I2C_WRITE(0x0051, 0x01);  
	//////////// 2.  AE SENSOR     //////
	/////////////////////////////////////
	XC9080_I2C_WRITE(0xfffe, 0x14); 
	XC9080_I2C_WRITE(0x000f, 0x01);  //Isp1 Used I2c
	XC9080_I2C_WRITE(0x0ac2, 0x48);  	//camera i2c id
	XC9080_I2C_WRITE(0x0ac3, 0x01);  	//camera i2c bits
	XC9080_I2C_WRITE(0x0ac4, 0x0f);  	//sensor type gain
	XC9080_I2C_WRITE(0x0ac5, 0x0f);  	//sensor type exposure
	//exposure
	XC9080_I2C_WRITE(0x0ac8, 0x02);  //write camera exposure variable [19:16]
	XC9080_I2C_WRITE(0x0ac9, 0x02);
	XC9080_I2C_WRITE(0x0aca, 0x02);  //write camera exposure variable [15:8]
	XC9080_I2C_WRITE(0x0acb, 0x03);
	XC9080_I2C_WRITE(0x0ad0, 0x00);  //camera exposure addr mask 1
	XC9080_I2C_WRITE(0x0ad1, 0xff);      
	XC9080_I2C_WRITE(0x0ad2, 0x00);  //camera exposure addr mask 2
	XC9080_I2C_WRITE(0x0ad3, 0xff);
	//gain
	XC9080_I2C_WRITE(0x0ae8, 0x02);  //camera gain addr
	XC9080_I2C_WRITE(0x0ae9, 0x05);
	XC9080_I2C_WRITE(0x0af0, 0x00);  //camera gain addr mask 1
	XC9080_I2C_WRITE(0x0af1, 0xff);
	XC9080_I2C_WRITE(0x0af9, 0x00);  //gain delay frame
	XC9080_I2C_WRITE(0x0042, 0x01);    //effect exposure mode
	XC9080_I2C_WRITE(0x0043, 0x01);    //effect gain mode
	///////// 3.  AE NORMAL     //////
	////////////////////////////////// 
	XC9080_I2C_WRITE(0xfffe, 0x14);  
	XC9080_I2C_WRITE(0x0a00, 0x0);   //AEC mode
#ifdef ENABLE_9080_AEC
	XC9080_I2C_WRITE(0x003c, 0x01);    //ae enable
#else
	XC9080_I2C_WRITE(0x003c, 0x00/*0x1*/);    //ae enable
#endif
	XC9080_I2C_WRITE(0x0a01, 0x1);    //ae Force write 
	XC9080_I2C_WRITE(0x0aae, 0x01);
	XC9080_I2C_WRITE(0x0aaf, 0xf0);   //max gain
	XC9080_I2C_WRITE(0x0ab0, 0x00);
	XC9080_I2C_WRITE(0x0ab1, 0x20);   //min gain
	XC9080_I2C_WRITE(0x0a96, 0x42);   //max exposure lines
	XC9080_I2C_WRITE(0x0a97, 0x00);   
	XC9080_I2C_WRITE(0x0a92, 0x0);
	XC9080_I2C_WRITE(0x0a93, 0x10);   //min exp
	XC9080_I2C_WRITE(0x0a54, 0x01);   //day target
	XC9080_I2C_WRITE(0x0a55, 0xc0);
	//flicker
	XC9080_I2C_WRITE(0x0ab8, 0x00);  
	XC9080_I2C_WRITE(0x0ab9, 0x01);  //Min FlickerLines Enable
	XC9080_I2C_WRITE(0x0abc, 0x15);   
	XC9080_I2C_WRITE(0x0abd, 0xa0);   
	//weight
	XC9080_I2C_WRITE(0x0a09, 0x04);
	XC9080_I2C_WRITE(0x0a0a, 0x04);
	XC9080_I2C_WRITE(0x0a0b, 0x04);
	XC9080_I2C_WRITE(0x0a0c, 0x04);
	XC9080_I2C_WRITE(0x0a0d, 0x04);
		    
	XC9080_I2C_WRITE(0x0a0e, 0x04);
	XC9080_I2C_WRITE(0x0a0f, 0x04);
	XC9080_I2C_WRITE(0x0a10, 0x04);
	XC9080_I2C_WRITE(0x0a11, 0x04);
	XC9080_I2C_WRITE(0x0a12, 0x04);
		    
	XC9080_I2C_WRITE(0x0a13, 0x04);
	XC9080_I2C_WRITE(0x0a14, 0x0a);
	XC9080_I2C_WRITE(0x0a15, 0x10);
	XC9080_I2C_WRITE(0x0a16, 0x0a);
	XC9080_I2C_WRITE(0x0a17, 0x04);
		    
	XC9080_I2C_WRITE(0x0a18, 0x08);
	XC9080_I2C_WRITE(0x0a19, 0x08);
	XC9080_I2C_WRITE(0x0a1a, 0x08);
	XC9080_I2C_WRITE(0x0a1b, 0x08);
	XC9080_I2C_WRITE(0x0a1c, 0x08);
		    
	XC9080_I2C_WRITE(0x0a1d, 0x04);
	XC9080_I2C_WRITE(0x0a1e, 0x04);
	XC9080_I2C_WRITE(0x0a1f, 0x04);
	XC9080_I2C_WRITE(0x0a20, 0x04);
	XC9080_I2C_WRITE(0x0a21, 0x04);  //权重for CurAvg
	XC9080_I2C_WRITE(0x0a3c, 0x00);
	XC9080_I2C_WRITE(0x0a3d, 0xe7);
	XC9080_I2C_WRITE(0x0a3e, 0x38);
	XC9080_I2C_WRITE(0x0a3f, 0x00);//关注区域 for 亮块
	XC9080_I2C_WRITE(0x0a04, 0x01);   //refresh
	//////////  4.AE SPEED       /////
	//////////////////////////////////
	//XC9080_I2C_WRITE(0x0a7a, 0x1);     // delay frame
	XC9080_I2C_WRITE(0x0a7e, 0x00);    //1. threshold low
	XC9080_I2C_WRITE(0x0a7f, 0x80);
		   
	XC9080_I2C_WRITE(0x0b70, 0x00);  //thr_l_all
	XC9080_I2C_WRITE(0x0b71, 0xa0);  //thr_l_all
		   
	XC9080_I2C_WRITE(0x0b72, 0x00);  //thr_l_avg
	XC9080_I2C_WRITE(0x0b73, 0x60);  //thr_l_avg
		   
	XC9080_I2C_WRITE(0x0a80, 0x00);    //2. threshold high
	XC9080_I2C_WRITE(0x0a81, 0x50);
		   
	XC9080_I2C_WRITE(0x0a7b, 0x20);    //3. finally thr
	XC9080_I2C_WRITE(0x0a8c, 0x80);    //4. stable thr_h
		   
	XC9080_I2C_WRITE(0x0a7c, 0x01);    //total speed
	XC9080_I2C_WRITE(0x0bbc, 0x02);    //speed limit factor
		   
	XC9080_I2C_WRITE(0x0a8e, 0x00);    // LumaDiffThrLow
	XC9080_I2C_WRITE(0x0a8f, 0xa0);
		   
	XC9080_I2C_WRITE(0x0a90, 0x03);    // LumaDiffThrHigh
	XC9080_I2C_WRITE(0x0a91, 0x00);
	//////////  5.AE SMART       ////
	/////////////////////////////////
	XC9080_I2C_WRITE(0x0a46, 0x01);  //smart mode
	XC9080_I2C_WRITE(0x0a47, 0x07);  //XC9080_I2C_WRITE(0x15  //analysis mode+zhifangtu
	XC9080_I2C_WRITE(0x0a48, 0x00);  //smart speed limit
		   
	XC9080_I2C_WRITE(0x0a61, 0x02);   //ATT block Cnt   关注区域亮块数算平均
	////////table reftarget//////////
	XC9080_I2C_WRITE(0x0022, 0x1e);   //use current fps //isp0、isp1 使用同一寄存器
	XC9080_I2C_WRITE(0x0b98, 0x00);   // Exp value Table 
	XC9080_I2C_WRITE(0x0b99, 0x00);
	XC9080_I2C_WRITE(0x0b9a, 0x04);
	XC9080_I2C_WRITE(0x0b9b, 0x00);  //table0
		 
	XC9080_I2C_WRITE(0x0b9c, 0x00);                                        
	XC9080_I2C_WRITE(0x0b9d, 0x00);         
	XC9080_I2C_WRITE(0x0b9e, 0x0c);         
	XC9080_I2C_WRITE(0x0b9f, 0x00);   //table1      
		             
	XC9080_I2C_WRITE(0x0ba0, 0x00);         
	XC9080_I2C_WRITE(0x0ba1, 0x00);         
	XC9080_I2C_WRITE(0x0ba2, 0x24);         
	XC9080_I2C_WRITE(0x0ba3, 0x00);    //table2     
		             
	XC9080_I2C_WRITE(0x0ba4, 0x00);         
	XC9080_I2C_WRITE(0x0ba5, 0x00);         
	XC9080_I2C_WRITE(0x0ba6, 0x6c);          
	XC9080_I2C_WRITE(0x0ba7, 0x00);    //table3       
		             
	XC9080_I2C_WRITE(0x0ba8, 0x00);         
	XC9080_I2C_WRITE(0x0ba9, 0x01);         
	XC9080_I2C_WRITE(0x0baa, 0x44);         
	XC9080_I2C_WRITE(0x0bab, 0x00);   //table4      
		             
	XC9080_I2C_WRITE(0x0bac, 0x00);           
	XC9080_I2C_WRITE(0x0bad, 0x04);         
	XC9080_I2C_WRITE(0x0bae, 0x00);         
	XC9080_I2C_WRITE(0x0baf, 0x00);  //table5   
	//////////reftarget///////////////
	XC9080_I2C_WRITE(0x0a66, 0x02);   // ref target table  0可更改的目标亮度
	XC9080_I2C_WRITE(0x0a67, 0x80); 
	XC9080_I2C_WRITE(0x0a68, 0x02);   // ref target table  1
	XC9080_I2C_WRITE(0x0a69, 0x60); 
	XC9080_I2C_WRITE(0x0a6a, 0x02);   // ref target table  2
	XC9080_I2C_WRITE(0x0a6b, 0x40); 
	XC9080_I2C_WRITE(0x0a6c, 0x01);   // ref target table  3
	XC9080_I2C_WRITE(0x0a6d, 0xf0); 
	XC9080_I2C_WRITE(0x0a6e, 0x01);   // ref target table  4
	XC9080_I2C_WRITE(0x0a6f, 0xb0); 
	XC9080_I2C_WRITE(0x0a70, 0x00);   // ref target table  5
	XC9080_I2C_WRITE(0x0a71, 0xf0); 
	//基于平均亮度维度分? 
	XC9080_I2C_WRITE(0x0b81, 0x28);//avg affect val
	XC9080_I2C_WRITE(0x0b7f, 0x00);//avg thr_l
	XC9080_I2C_WRITE(0x0b80, 0x40);  //avg thr_h
	///////////over exposure offest//////
	XC9080_I2C_WRITE(0x0b8a, 0x00);   
	XC9080_I2C_WRITE(0x0b8b, 0x04); 
	XC9080_I2C_WRITE(0x0b8c, 0x14); 
	XC9080_I2C_WRITE(0x0b8d, 0x50); 
	XC9080_I2C_WRITE(0x0b8e, 0x70); 
	XC9080_I2C_WRITE(0x0b8f, 0x90);
	XC9080_I2C_WRITE(0x0b74, 0x08);
	XC9080_I2C_WRITE(0x0b7b, 0x06);
	XC9080_I2C_WRITE(0x0b7d, 0x02);
	XC9080_I2C_WRITE(0x0b7e, 0x30);
	XC9080_I2C_WRITE(0x0b85, 0x40);
	//基于方差维度分析模式/Variance  /////////////////
	XC9080_I2C_WRITE(0x0b65, 0x30);  // variance affect
	XC9080_I2C_WRITE(0x0b66, 0x01);  // thr l
	XC9080_I2C_WRITE(0x0b67, 0x00);
	XC9080_I2C_WRITE(0x0b68, 0x06);  // thr h
	XC9080_I2C_WRITE(0x0b69, 0x00);
	//基于关注区亮度压制
	XC9080_I2C_WRITE(0x0b22, 0x02);
	XC9080_I2C_WRITE(0x0b23, 0x80);  //Max ATT thrshold关注区域的门限值
		   
	XC9080_I2C_WRITE(0x0b87, 0x00);   // no use Att limit ratio L
	XC9080_I2C_WRITE(0x0b88, 0x30);   // Att limit ratio H关注区域的调整值
		   
	XC9080_I2C_WRITE(0x0b20, 0x00);   //min att thr
	XC9080_I2C_WRITE(0x0b21, 0x00);   //min att thr
		   
	XC9080_I2C_WRITE(0x0b6e, 0x08);   //Bright Ratio2
	////////////smart ae end /////////
	XC9080_I2C_WRITE(0x1a7e, 0x02);//2frame write
	XC9080_I2C_WRITE(0x0bc6, 0x00);//ae chufa menxian
	/////////////AE  END/////////////////
	////////////////////////////////////
	//ISP1 BLC           
	XC9080_I2C_WRITE(0xfffe, 0x31);         
	XC9080_I2C_WRITE(0x0013, 0x20);//bias   
	XC9080_I2C_WRITE(0x0014, 0x00);         
	XC9080_I2C_WRITE(0x071b, 0x80);//blc   
	//ISP1 LENC           
	XC9080_I2C_WRITE(0xfffe, 0x31);
	//XC9080_I2C_WRITE(0x0002, 0x80);      //bit[7]:lenc_en
	XC9080_I2C_WRITE(0x03ca, 0x16);	        
	XC9080_I2C_WRITE(0x03cb, 0xC1);	      
	XC9080_I2C_WRITE(0x03cc, 0x16);	      
	XC9080_I2C_WRITE(0x03cd, 0xC1);	      
	XC9080_I2C_WRITE(0x03ce, 0x16);	        
	XC9080_I2C_WRITE(0x03cf, 0xC1);	      
	XC9080_I2C_WRITE(0x03d0, 0x0B);	        
	XC9080_I2C_WRITE(0x03d1, 0x60);                  
		 
	XC9080_I2C_WRITE(0xfffe, 0x14);                 
	XC9080_I2C_WRITE(0x003d, 0x01);        
	XC9080_I2C_WRITE(0x0041, 0x01);        
	XC9080_I2C_WRITE(0x0fd4, 0x01);	      
	XC9080_I2C_WRITE(0x0fd5, 0x01);	      
		
	XC9080_I2C_WRITE(0x12dc, 0x00);	//ct  
	XC9080_I2C_WRITE(0x12dd, 0x67);        
	XC9080_I2C_WRITE(0x12de, 0x00);	      
	XC9080_I2C_WRITE(0x12df, 0xb3);        
	XC9080_I2C_WRITE(0x12e0, 0x01);	      
	XC9080_I2C_WRITE(0x12e1, 0x32);        
		 
	XC9080_I2C_WRITE(0x1099, 0x3c);  //A _9);20161101_v2
	XC9080_I2C_WRITE(0x109a, 0x1e);        
	XC9080_I2C_WRITE(0x109b, 0x15);        
	XC9080_I2C_WRITE(0x109c, 0x11);        
	XC9080_I2C_WRITE(0x109d, 0x11);        
	XC9080_I2C_WRITE(0x109e, 0x13);        
	XC9080_I2C_WRITE(0x109f, 0x1c);        
	XC9080_I2C_WRITE(0x10a0, 0x32);        
	XC9080_I2C_WRITE(0x10a1, 0x8 );        
	XC9080_I2C_WRITE(0x10a2, 0x5 );        
	XC9080_I2C_WRITE(0x10a3, 0x3 );        
	XC9080_I2C_WRITE(0x10a4, 0x3 );        
	XC9080_I2C_WRITE(0x10a5, 0x3 );        
	XC9080_I2C_WRITE(0x10a6, 0x3 );        
	XC9080_I2C_WRITE(0x10a7, 0x5 );        
	XC9080_I2C_WRITE(0x10a8, 0x9 );        
	XC9080_I2C_WRITE(0x10a9, 0x3 );        
	XC9080_I2C_WRITE(0x10aa, 0x1 );        
	XC9080_I2C_WRITE(0x10ab, 0x1 );        
	XC9080_I2C_WRITE(0x10ac, 0x1 );        
	XC9080_I2C_WRITE(0x10ad, 0x1 );        
	XC9080_I2C_WRITE(0x10ae, 0x1 );        
	XC9080_I2C_WRITE(0x10af, 0x2 );        
	XC9080_I2C_WRITE(0x10b0, 0x4 );        
	XC9080_I2C_WRITE(0x10b1, 0x1 );        
	XC9080_I2C_WRITE(0x10b2, 0x0 );        
	XC9080_I2C_WRITE(0x10b3, 0x0 );        
	XC9080_I2C_WRITE(0x10b4, 0x0 );        
	XC9080_I2C_WRITE(0x10b5, 0x0 );        
	XC9080_I2C_WRITE(0x10b6, 0x0 );        
	XC9080_I2C_WRITE(0x10b7, 0x1 );        
	XC9080_I2C_WRITE(0x10b8, 0x1 );        
	XC9080_I2C_WRITE(0x10b9, 0x1 );        
	XC9080_I2C_WRITE(0x10ba, 0x0 );        
	XC9080_I2C_WRITE(0x10bb, 0x0 );        
	XC9080_I2C_WRITE(0x10bc, 0x0 );        
	XC9080_I2C_WRITE(0x10bd, 0x0 );        
	XC9080_I2C_WRITE(0x10be, 0x0 );        
	XC9080_I2C_WRITE(0x10bf, 0x1 );        
	XC9080_I2C_WRITE(0x10c0, 0x2 );        
	XC9080_I2C_WRITE(0x10c1, 0x3 );        
	XC9080_I2C_WRITE(0x10c2, 0x1 );        
	XC9080_I2C_WRITE(0x10c3, 0x1 );        
	XC9080_I2C_WRITE(0x10c4, 0x1 );        
	XC9080_I2C_WRITE(0x10c5, 0x1 );        
	XC9080_I2C_WRITE(0x10c6, 0x1 );        
	XC9080_I2C_WRITE(0x10c7, 0x2 );        
	XC9080_I2C_WRITE(0x10c8, 0x3 );        
	XC9080_I2C_WRITE(0x10c9, 0x9 );        
	XC9080_I2C_WRITE(0x10ca, 0x6 );        
	XC9080_I2C_WRITE(0x10cb, 0x3 );        
	XC9080_I2C_WRITE(0x10cc, 0x3 );        
	XC9080_I2C_WRITE(0x10cd, 0x3 );        
	XC9080_I2C_WRITE(0x10ce, 0x4 );        
	XC9080_I2C_WRITE(0x10cf, 0x5 );        
	XC9080_I2C_WRITE(0x10d0, 0xb );        
	XC9080_I2C_WRITE(0x10d1, 0x3f);        
	XC9080_I2C_WRITE(0x10d2, 0x21);        
	XC9080_I2C_WRITE(0x10d3, 0x16);        
	XC9080_I2C_WRITE(0x10d4, 0x12);        
	XC9080_I2C_WRITE(0x10d5, 0x11);        
	XC9080_I2C_WRITE(0x10d6, 0x14);        
	XC9080_I2C_WRITE(0x10d7, 0x20);        
	XC9080_I2C_WRITE(0x10d8, 0x28);        
	XC9080_I2C_WRITE(0x10d9, 0xf );        
	XC9080_I2C_WRITE(0x10da, 0x1a);        
	XC9080_I2C_WRITE(0x10db, 0x1b);        
	XC9080_I2C_WRITE(0x10dc, 0x1c);        
	XC9080_I2C_WRITE(0x10dd, 0x1c);        
	XC9080_I2C_WRITE(0x10de, 0x1a);        
	XC9080_I2C_WRITE(0x10df, 0x18);        
	XC9080_I2C_WRITE(0x10e0, 0x14);        
	XC9080_I2C_WRITE(0x10e1, 0x1d);        
	XC9080_I2C_WRITE(0x10e2, 0x1e);        
	XC9080_I2C_WRITE(0x10e3, 0x1f);        
	XC9080_I2C_WRITE(0x10e4, 0x1f);        
	XC9080_I2C_WRITE(0x10e5, 0x1f);        
	XC9080_I2C_WRITE(0x10e6, 0x20);        
	XC9080_I2C_WRITE(0x10e7, 0x1f);        
	XC9080_I2C_WRITE(0x10e8, 0x1e);        
	XC9080_I2C_WRITE(0x10e9, 0x1d);        
	XC9080_I2C_WRITE(0x10ea, 0x20);        
	XC9080_I2C_WRITE(0x10eb, 0x1f);        
	XC9080_I2C_WRITE(0x10ec, 0x20);        
	XC9080_I2C_WRITE(0x10ed, 0x20);        
	XC9080_I2C_WRITE(0x10ee, 0x20);        
	XC9080_I2C_WRITE(0x10ef, 0x1f);        
	XC9080_I2C_WRITE(0x10f0, 0x20);        
	XC9080_I2C_WRITE(0x10f1, 0x1e);        
	XC9080_I2C_WRITE(0x10f2, 0x20);        
	XC9080_I2C_WRITE(0x10f3, 0x20);        
	XC9080_I2C_WRITE(0x10f4, 0x20);        
	XC9080_I2C_WRITE(0x10f5, 0x20);        
	XC9080_I2C_WRITE(0x10f6, 0x20);        
	XC9080_I2C_WRITE(0x10f7, 0x20);        
	XC9080_I2C_WRITE(0x10f8, 0x21);        
	XC9080_I2C_WRITE(0x10f9, 0x1e);        
	XC9080_I2C_WRITE(0x10fa, 0x20);        
	XC9080_I2C_WRITE(0x10fb, 0x20);        
	XC9080_I2C_WRITE(0x10fc, 0x20);        
	XC9080_I2C_WRITE(0x10fd, 0x20);        
	XC9080_I2C_WRITE(0x10fe, 0x21);        
	XC9080_I2C_WRITE(0x10ff, 0x21);        
	XC9080_I2C_WRITE(0x1100, 0x20);        
	XC9080_I2C_WRITE(0x1101, 0x1d);        
	XC9080_I2C_WRITE(0x1102, 0x20);        
	XC9080_I2C_WRITE(0x1103, 0x20);        
	XC9080_I2C_WRITE(0x1104, 0x20);        
	XC9080_I2C_WRITE(0x1105, 0x20);        
	XC9080_I2C_WRITE(0x1106, 0x20);        
	XC9080_I2C_WRITE(0x1107, 0x20);        
	XC9080_I2C_WRITE(0x1108, 0x20);        
	XC9080_I2C_WRITE(0x1109, 0x1c);        
	XC9080_I2C_WRITE(0x110a, 0x1f);        
	XC9080_I2C_WRITE(0x110b, 0x1f);        
	XC9080_I2C_WRITE(0x110c, 0x20);        
	XC9080_I2C_WRITE(0x110d, 0x20);        
	XC9080_I2C_WRITE(0x110e, 0x1f);        
	XC9080_I2C_WRITE(0x110f, 0x20);        
	XC9080_I2C_WRITE(0x1110, 0x1d);        
	XC9080_I2C_WRITE(0x1111, 0xe );        
	XC9080_I2C_WRITE(0x1112, 0x18);        
	XC9080_I2C_WRITE(0x1113, 0x1a);        
	XC9080_I2C_WRITE(0x1114, 0x1b);        
	XC9080_I2C_WRITE(0x1115, 0x1b);        
	XC9080_I2C_WRITE(0x1116, 0x1b);        
	XC9080_I2C_WRITE(0x1117, 0x17);        
	XC9080_I2C_WRITE(0x1118, 0x1b);        
	XC9080_I2C_WRITE(0x1119, 0x1f);        
	XC9080_I2C_WRITE(0x111a, 0x22);        
	XC9080_I2C_WRITE(0x111b, 0x22);        
	XC9080_I2C_WRITE(0x111c, 0x23);        
	XC9080_I2C_WRITE(0x111d, 0x24);        
	XC9080_I2C_WRITE(0x111e, 0x24);        
	XC9080_I2C_WRITE(0x111f, 0x23);        
	XC9080_I2C_WRITE(0x1120, 0x22);        
	XC9080_I2C_WRITE(0x1121, 0x25);        
	XC9080_I2C_WRITE(0x1122, 0x23);        
	XC9080_I2C_WRITE(0x1123, 0x23);        
	XC9080_I2C_WRITE(0x1124, 0x22);        
	XC9080_I2C_WRITE(0x1125, 0x22);        
	XC9080_I2C_WRITE(0x1126, 0x22);        
	XC9080_I2C_WRITE(0x1127, 0x22);        
	XC9080_I2C_WRITE(0x1128, 0x23);        
	XC9080_I2C_WRITE(0x1129, 0x24);        
	XC9080_I2C_WRITE(0x112a, 0x21);        
	XC9080_I2C_WRITE(0x112b, 0x21);        
	XC9080_I2C_WRITE(0x112c, 0x21);        
	XC9080_I2C_WRITE(0x112d, 0x21);        
	XC9080_I2C_WRITE(0x112e, 0x21);        
	XC9080_I2C_WRITE(0x112f, 0x21);        
	XC9080_I2C_WRITE(0x1130, 0x22);        
	XC9080_I2C_WRITE(0x1131, 0x23);        
	XC9080_I2C_WRITE(0x1132, 0x21);        
	XC9080_I2C_WRITE(0x1133, 0x21);        
	XC9080_I2C_WRITE(0x1134, 0x20);        
	XC9080_I2C_WRITE(0x1135, 0x20);        
	XC9080_I2C_WRITE(0x1136, 0x20);        
	XC9080_I2C_WRITE(0x1137, 0x20);        
	XC9080_I2C_WRITE(0x1138, 0x21);        
	XC9080_I2C_WRITE(0x1139, 0x23);        
	XC9080_I2C_WRITE(0x113a, 0x21);        
	XC9080_I2C_WRITE(0x113b, 0x20);        
	XC9080_I2C_WRITE(0x113c, 0x20);        
	XC9080_I2C_WRITE(0x113d, 0x20);        
	XC9080_I2C_WRITE(0x113e, 0x20);        
	XC9080_I2C_WRITE(0x113f, 0x20);        
	XC9080_I2C_WRITE(0x1140, 0x21);        
	XC9080_I2C_WRITE(0x1141, 0x24);        
	XC9080_I2C_WRITE(0x1142, 0x21);        
	XC9080_I2C_WRITE(0x1143, 0x21);        
	XC9080_I2C_WRITE(0x1144, 0x21);        
	XC9080_I2C_WRITE(0x1145, 0x21);        
	XC9080_I2C_WRITE(0x1146, 0x21);        
	XC9080_I2C_WRITE(0x1147, 0x21);        
	XC9080_I2C_WRITE(0x1148, 0x22);        
	XC9080_I2C_WRITE(0x1149, 0x24);        
	XC9080_I2C_WRITE(0x114a, 0x22);        
	XC9080_I2C_WRITE(0x114b, 0x22);        
	XC9080_I2C_WRITE(0x114c, 0x22);        
	XC9080_I2C_WRITE(0x114d, 0x22);        
	XC9080_I2C_WRITE(0x114e, 0x22);        
	XC9080_I2C_WRITE(0x114f, 0x22);        
	XC9080_I2C_WRITE(0x1150, 0x23);        
	XC9080_I2C_WRITE(0x1151, 0x22);        
	XC9080_I2C_WRITE(0x1152, 0x22);        
	XC9080_I2C_WRITE(0x1153, 0x24);        
	XC9080_I2C_WRITE(0x1154, 0x24);        
	XC9080_I2C_WRITE(0x1155, 0x24);        
	XC9080_I2C_WRITE(0x1156, 0x24);        
	XC9080_I2C_WRITE(0x1157, 0x23);        
	XC9080_I2C_WRITE(0x1158, 0x21);        
		 
	XC9080_I2C_WRITE(0x1159, 0x3d);        //c_95);20161101_v2
	XC9080_I2C_WRITE(0x115a, 0x1e);        
	XC9080_I2C_WRITE(0x115b, 0x15);        
	XC9080_I2C_WRITE(0x115c, 0x11);        
	XC9080_I2C_WRITE(0x115d, 0x10);        
	XC9080_I2C_WRITE(0x115e, 0x13);        
	XC9080_I2C_WRITE(0x115f, 0x1c);        
	XC9080_I2C_WRITE(0x1160, 0x33);        
	XC9080_I2C_WRITE(0x1161, 0x8 );        
	XC9080_I2C_WRITE(0x1162, 0x5 );        
	XC9080_I2C_WRITE(0x1163, 0x3 );        
	XC9080_I2C_WRITE(0x1164, 0x3 );        
	XC9080_I2C_WRITE(0x1165, 0x3 );        
	XC9080_I2C_WRITE(0x1166, 0x3 );        
	XC9080_I2C_WRITE(0x1167, 0x5 );        
	XC9080_I2C_WRITE(0x1168, 0x9 );        
	XC9080_I2C_WRITE(0x1169, 0x3 );        
	XC9080_I2C_WRITE(0x116a, 0x1 );        
	XC9080_I2C_WRITE(0x116b, 0x0 );        
	XC9080_I2C_WRITE(0x116c, 0x1 );        
	XC9080_I2C_WRITE(0x116d, 0x1 );        
	XC9080_I2C_WRITE(0x116e, 0x1 );        
	XC9080_I2C_WRITE(0x116f, 0x2 );        
	XC9080_I2C_WRITE(0x1170, 0x3 );        
	XC9080_I2C_WRITE(0x1171, 0x1 );        
	XC9080_I2C_WRITE(0x1172, 0x0 );        
	XC9080_I2C_WRITE(0x1173, 0x0 );        
	XC9080_I2C_WRITE(0x1174, 0x0 );        
	XC9080_I2C_WRITE(0x1175, 0x0 );        
	XC9080_I2C_WRITE(0x1176, 0x0 );        
	XC9080_I2C_WRITE(0x1177, 0x1 );        
	XC9080_I2C_WRITE(0x1178, 0x1 );        
	XC9080_I2C_WRITE(0x1179, 0x1 );        
	XC9080_I2C_WRITE(0x117a, 0x0 );        
	XC9080_I2C_WRITE(0x117b, 0x0 );        
	XC9080_I2C_WRITE(0x117c, 0x0 );        
	XC9080_I2C_WRITE(0x117d, 0x0 );        
	XC9080_I2C_WRITE(0x117e, 0x0 );        
	XC9080_I2C_WRITE(0x117f, 0x1 );        
	XC9080_I2C_WRITE(0x1180, 0x2 );        
	XC9080_I2C_WRITE(0x1181, 0x3 );        
	XC9080_I2C_WRITE(0x1182, 0x1 );        
	XC9080_I2C_WRITE(0x1183, 0x1 );        
	XC9080_I2C_WRITE(0x1184, 0x1 );        
	XC9080_I2C_WRITE(0x1185, 0x1 );        
	XC9080_I2C_WRITE(0x1186, 0x1 );        
	XC9080_I2C_WRITE(0x1187, 0x2 );        
	XC9080_I2C_WRITE(0x1188, 0x3 );        
	XC9080_I2C_WRITE(0x1189, 0x9 );        
	XC9080_I2C_WRITE(0x118a, 0x6 );        
	XC9080_I2C_WRITE(0x118b, 0x3 );        
	XC9080_I2C_WRITE(0x118c, 0x3 );        
	XC9080_I2C_WRITE(0x118d, 0x3 );        
	XC9080_I2C_WRITE(0x118e, 0x4 );        
	XC9080_I2C_WRITE(0x118f, 0x5 );        
	XC9080_I2C_WRITE(0x1190, 0xa );        
	XC9080_I2C_WRITE(0x1191, 0x3f);        
	XC9080_I2C_WRITE(0x1192, 0x21);        
	XC9080_I2C_WRITE(0x1193, 0x16);        
	XC9080_I2C_WRITE(0x1194, 0x12);        
	XC9080_I2C_WRITE(0x1195, 0x11);        
	XC9080_I2C_WRITE(0x1196, 0x14);        
	XC9080_I2C_WRITE(0x1197, 0x1f);        
	XC9080_I2C_WRITE(0x1198, 0x2e);        
	XC9080_I2C_WRITE(0x1199, 0x13);        
	XC9080_I2C_WRITE(0x119a, 0x1a);        
	XC9080_I2C_WRITE(0x119b, 0x1b);        
	XC9080_I2C_WRITE(0x119c, 0x1c);        
	XC9080_I2C_WRITE(0x119d, 0x1c);        
	XC9080_I2C_WRITE(0x119e, 0x1a);        
	XC9080_I2C_WRITE(0x119f, 0x18);        
	XC9080_I2C_WRITE(0x11a0, 0x17);        
	XC9080_I2C_WRITE(0x11a1, 0x1c);        
	XC9080_I2C_WRITE(0x11a2, 0x1e);        
	XC9080_I2C_WRITE(0x11a3, 0x1f);        
	XC9080_I2C_WRITE(0x11a4, 0x1f);        
	XC9080_I2C_WRITE(0x11a5, 0x1f);        
	XC9080_I2C_WRITE(0x11a6, 0x1f);        
	XC9080_I2C_WRITE(0x11a7, 0x1f);        
	XC9080_I2C_WRITE(0x11a8, 0x1d);        
	XC9080_I2C_WRITE(0x11a9, 0x1d);        
	XC9080_I2C_WRITE(0x11aa, 0x1f);        
	XC9080_I2C_WRITE(0x11ab, 0x1f);        
	XC9080_I2C_WRITE(0x11ac, 0x20);        
	XC9080_I2C_WRITE(0x11ad, 0x20);        
	XC9080_I2C_WRITE(0x11ae, 0x20);        
	XC9080_I2C_WRITE(0x11af, 0x1f);        
	XC9080_I2C_WRITE(0x11b0, 0x1f);        
	XC9080_I2C_WRITE(0x11b1, 0x1e);        
	XC9080_I2C_WRITE(0x11b2, 0x20);        
	XC9080_I2C_WRITE(0x11b3, 0x20);        
	XC9080_I2C_WRITE(0x11b4, 0x21);        
	XC9080_I2C_WRITE(0x11b5, 0x20);        
	XC9080_I2C_WRITE(0x11b6, 0x21);        
	XC9080_I2C_WRITE(0x11b7, 0x20);        
	XC9080_I2C_WRITE(0x11b8, 0x20);        
	XC9080_I2C_WRITE(0x11b9, 0x1e);        
	XC9080_I2C_WRITE(0x11ba, 0x20);        
	XC9080_I2C_WRITE(0x11bb, 0x20);        
	XC9080_I2C_WRITE(0x11bc, 0x21);        
	XC9080_I2C_WRITE(0x11bd, 0x21);        
	XC9080_I2C_WRITE(0x11be, 0x21);        
	XC9080_I2C_WRITE(0x11bf, 0x20);        
	XC9080_I2C_WRITE(0x11c0, 0x20);        
	XC9080_I2C_WRITE(0x11c1, 0x1d);        
	XC9080_I2C_WRITE(0x11c2, 0x1f);        
	XC9080_I2C_WRITE(0x11c3, 0x20);        
	XC9080_I2C_WRITE(0x11c4, 0x20);        
	XC9080_I2C_WRITE(0x11c5, 0x20);        
	XC9080_I2C_WRITE(0x11c6, 0x20);        
	XC9080_I2C_WRITE(0x11c7, 0x20);        
	XC9080_I2C_WRITE(0x11c8, 0x1f);        
	XC9080_I2C_WRITE(0x11c9, 0x1c);        
	XC9080_I2C_WRITE(0x11ca, 0x1e);        
	XC9080_I2C_WRITE(0x11cb, 0x1f);        
	XC9080_I2C_WRITE(0x11cc, 0x1f);        
	XC9080_I2C_WRITE(0x11cd, 0x1f);        
	XC9080_I2C_WRITE(0x11ce, 0x1f);        
	XC9080_I2C_WRITE(0x11cf, 0x1f);        
	XC9080_I2C_WRITE(0x11d0, 0x1d);        
	XC9080_I2C_WRITE(0x11d1, 0x11);        
	XC9080_I2C_WRITE(0x11d2, 0x18);        
	XC9080_I2C_WRITE(0x11d3, 0x1a);        
	XC9080_I2C_WRITE(0x11d4, 0x1b);        
	XC9080_I2C_WRITE(0x11d5, 0x1b);        
	XC9080_I2C_WRITE(0x11d6, 0x1b);        
	XC9080_I2C_WRITE(0x11d7, 0x18);        
	XC9080_I2C_WRITE(0x11d8, 0x19);        
	XC9080_I2C_WRITE(0x11d9, 0x1a);        
	XC9080_I2C_WRITE(0x11da, 0x1e);        
	XC9080_I2C_WRITE(0x11db, 0x1f);        
	XC9080_I2C_WRITE(0x11dc, 0x20);        
	XC9080_I2C_WRITE(0x11dd, 0x20);        
	XC9080_I2C_WRITE(0x11de, 0x20);        
	XC9080_I2C_WRITE(0x11df, 0x1f);        
	XC9080_I2C_WRITE(0x11e0, 0x1e);        
	XC9080_I2C_WRITE(0x11e1, 0x22);        
	XC9080_I2C_WRITE(0x11e2, 0x21);        
	XC9080_I2C_WRITE(0x11e3, 0x21);        
	XC9080_I2C_WRITE(0x11e4, 0x21);        
	XC9080_I2C_WRITE(0x11e5, 0x21);        
	XC9080_I2C_WRITE(0x11e6, 0x21);        
	XC9080_I2C_WRITE(0x11e7, 0x21);        
	XC9080_I2C_WRITE(0x11e8, 0x21);        
	XC9080_I2C_WRITE(0x11e9, 0x22);        
	XC9080_I2C_WRITE(0x11ea, 0x21);        
	XC9080_I2C_WRITE(0x11eb, 0x21);        
	XC9080_I2C_WRITE(0x11ec, 0x21);        
	XC9080_I2C_WRITE(0x11ed, 0x21);        
	XC9080_I2C_WRITE(0x11ee, 0x21);        
	XC9080_I2C_WRITE(0x11ef, 0x21);        
	XC9080_I2C_WRITE(0x11f0, 0x21);        
	XC9080_I2C_WRITE(0x11f1, 0x22);        
	XC9080_I2C_WRITE(0x11f2, 0x20);        
	XC9080_I2C_WRITE(0x11f3, 0x20);        
	XC9080_I2C_WRITE(0x11f4, 0x20);        
	XC9080_I2C_WRITE(0x11f5, 0x20);        
	XC9080_I2C_WRITE(0x11f6, 0x20);        
	XC9080_I2C_WRITE(0x11f7, 0x20);        
	XC9080_I2C_WRITE(0x11f8, 0x21);        
	XC9080_I2C_WRITE(0x11f9, 0x22);        
	XC9080_I2C_WRITE(0x11fa, 0x20);        
	XC9080_I2C_WRITE(0x11fb, 0x20);        
	XC9080_I2C_WRITE(0x11fc, 0x20);        
	XC9080_I2C_WRITE(0x11fd, 0x20);        
	XC9080_I2C_WRITE(0x11fe, 0x20);        
	XC9080_I2C_WRITE(0x11ff, 0x20);        
	XC9080_I2C_WRITE(0x1200, 0x21);        
	XC9080_I2C_WRITE(0x1201, 0x22);        
	XC9080_I2C_WRITE(0x1202, 0x20);        
	XC9080_I2C_WRITE(0x1203, 0x20);        
	XC9080_I2C_WRITE(0x1204, 0x20);        
	XC9080_I2C_WRITE(0x1205, 0x21);        
	XC9080_I2C_WRITE(0x1206, 0x20);        
	XC9080_I2C_WRITE(0x1207, 0x21);        
	XC9080_I2C_WRITE(0x1208, 0x20);        
	XC9080_I2C_WRITE(0x1209, 0x21);        
	XC9080_I2C_WRITE(0x120a, 0x20);        
	XC9080_I2C_WRITE(0x120b, 0x21);        
	XC9080_I2C_WRITE(0x120c, 0x21);        
	XC9080_I2C_WRITE(0x120d, 0x21);        
	XC9080_I2C_WRITE(0x120e, 0x21);        
	XC9080_I2C_WRITE(0x120f, 0x21);        
	XC9080_I2C_WRITE(0x1210, 0x22);        
	XC9080_I2C_WRITE(0x1211, 0x1c);        
	XC9080_I2C_WRITE(0x1212, 0x1e);        
	XC9080_I2C_WRITE(0x1213, 0x20);        
	XC9080_I2C_WRITE(0x1214, 0x20);        
	XC9080_I2C_WRITE(0x1215, 0x21);        
	XC9080_I2C_WRITE(0x1216, 0x20);        
	XC9080_I2C_WRITE(0x1217, 0x20);        
	XC9080_I2C_WRITE(0x1218, 0x1a);        
		 
	XC9080_I2C_WRITE(0x1219, 0x3c);        //d_95);20161101_v2
	XC9080_I2C_WRITE(0x121a, 0x1e);        
	XC9080_I2C_WRITE(0x121b, 0x15);        
	XC9080_I2C_WRITE(0x121c, 0x10);        
	XC9080_I2C_WRITE(0x121d, 0x10);        
	XC9080_I2C_WRITE(0x121e, 0x13);        
	XC9080_I2C_WRITE(0x121f, 0x1b);        
	XC9080_I2C_WRITE(0x1220, 0x32);        
	XC9080_I2C_WRITE(0x1221, 0x8 );        
	XC9080_I2C_WRITE(0x1222, 0x5 );        
	XC9080_I2C_WRITE(0x1223, 0x3 );        
	XC9080_I2C_WRITE(0x1224, 0x3 );        
	XC9080_I2C_WRITE(0x1225, 0x3 );        
	XC9080_I2C_WRITE(0x1226, 0x3 );        
	XC9080_I2C_WRITE(0x1227, 0x5 );        
	XC9080_I2C_WRITE(0x1228, 0x9 );        
	XC9080_I2C_WRITE(0x1229, 0x3 );        
	XC9080_I2C_WRITE(0x122a, 0x1 );        
	XC9080_I2C_WRITE(0x122b, 0x0 );        
	XC9080_I2C_WRITE(0x122c, 0x1 );        
	XC9080_I2C_WRITE(0x122d, 0x1 );        
	XC9080_I2C_WRITE(0x122e, 0x1 );        
	XC9080_I2C_WRITE(0x122f, 0x2 );        
	XC9080_I2C_WRITE(0x1230, 0x3 );        
	XC9080_I2C_WRITE(0x1231, 0x1 );        
	XC9080_I2C_WRITE(0x1232, 0x0 );        
	XC9080_I2C_WRITE(0x1233, 0x0 );        
	XC9080_I2C_WRITE(0x1234, 0x0 );        
	XC9080_I2C_WRITE(0x1235, 0x0 );        
	XC9080_I2C_WRITE(0x1236, 0x1 );        
	XC9080_I2C_WRITE(0x1237, 0x1 );        
	XC9080_I2C_WRITE(0x1238, 0x1 );        
	XC9080_I2C_WRITE(0x1239, 0x1 );        
	XC9080_I2C_WRITE(0x123a, 0x0 );        
	XC9080_I2C_WRITE(0x123b, 0x0 );        
	XC9080_I2C_WRITE(0x123c, 0x0 );        
	XC9080_I2C_WRITE(0x123d, 0x0 );        
	XC9080_I2C_WRITE(0x123e, 0x1 );        
	XC9080_I2C_WRITE(0x123f, 0x1 );        
	XC9080_I2C_WRITE(0x1240, 0x2 );        
	XC9080_I2C_WRITE(0x1241, 0x3 );        
	XC9080_I2C_WRITE(0x1242, 0x1 );        
	XC9080_I2C_WRITE(0x1243, 0x1 );        
	XC9080_I2C_WRITE(0x1244, 0x1 );        
	XC9080_I2C_WRITE(0x1245, 0x1 );        
	XC9080_I2C_WRITE(0x1246, 0x1 );        
	XC9080_I2C_WRITE(0x1247, 0x2 );        
	XC9080_I2C_WRITE(0x1248, 0x3 );        
	XC9080_I2C_WRITE(0x1249, 0x9 );        
	XC9080_I2C_WRITE(0x124a, 0x5 );        
	XC9080_I2C_WRITE(0x124b, 0x3 );        
	XC9080_I2C_WRITE(0x124c, 0x3 );        
	XC9080_I2C_WRITE(0x124d, 0x3 );        
	XC9080_I2C_WRITE(0x124e, 0x4 );        
	XC9080_I2C_WRITE(0x124f, 0x5 );        
	XC9080_I2C_WRITE(0x1250, 0xa );        
	XC9080_I2C_WRITE(0x1251, 0x3f);        
	XC9080_I2C_WRITE(0x1252, 0x20);        
	XC9080_I2C_WRITE(0x1253, 0x16);        
	XC9080_I2C_WRITE(0x1254, 0x12);        
	XC9080_I2C_WRITE(0x1255, 0x11);        
	XC9080_I2C_WRITE(0x1256, 0x14);        
	XC9080_I2C_WRITE(0x1257, 0x1f);        
	XC9080_I2C_WRITE(0x1258, 0x2e);        
	XC9080_I2C_WRITE(0x1259, 0x18);        
	XC9080_I2C_WRITE(0x125a, 0x1c);        
	XC9080_I2C_WRITE(0x125b, 0x1d);        
	XC9080_I2C_WRITE(0x125c, 0x1d);        
	XC9080_I2C_WRITE(0x125d, 0x1d);        
	XC9080_I2C_WRITE(0x125e, 0x1c);        
	XC9080_I2C_WRITE(0x125f, 0x1b);        
	XC9080_I2C_WRITE(0x1260, 0x1b);        
	XC9080_I2C_WRITE(0x1261, 0x1d);        
	XC9080_I2C_WRITE(0x1262, 0x1e);        
	XC9080_I2C_WRITE(0x1263, 0x1f);        
	XC9080_I2C_WRITE(0x1264, 0x1f);        
	XC9080_I2C_WRITE(0x1265, 0x1f);        
	XC9080_I2C_WRITE(0x1266, 0x1f);        
	XC9080_I2C_WRITE(0x1267, 0x1f);        
	XC9080_I2C_WRITE(0x1268, 0x1e);        
	XC9080_I2C_WRITE(0x1269, 0x1e);        
	XC9080_I2C_WRITE(0x126a, 0x1f);        
	XC9080_I2C_WRITE(0x126b, 0x1f);        
	XC9080_I2C_WRITE(0x126c, 0x20);        
	XC9080_I2C_WRITE(0x126d, 0x20);        
	XC9080_I2C_WRITE(0x126e, 0x20);        
	XC9080_I2C_WRITE(0x126f, 0x1f);        
	XC9080_I2C_WRITE(0x1270, 0x1f);        
	XC9080_I2C_WRITE(0x1271, 0x1e);        
	XC9080_I2C_WRITE(0x1272, 0x20);        
	XC9080_I2C_WRITE(0x1273, 0x20);        
	XC9080_I2C_WRITE(0x1274, 0x20);        
	XC9080_I2C_WRITE(0x1275, 0x20);        
	XC9080_I2C_WRITE(0x1276, 0x20);        
	XC9080_I2C_WRITE(0x1277, 0x20);        
	XC9080_I2C_WRITE(0x1278, 0x1f);        
	XC9080_I2C_WRITE(0x1279, 0x1f);        
	XC9080_I2C_WRITE(0x127a, 0x1f);        
	XC9080_I2C_WRITE(0x127b, 0x20);        
	XC9080_I2C_WRITE(0x127c, 0x20);        
	XC9080_I2C_WRITE(0x127d, 0x20);        
	XC9080_I2C_WRITE(0x127e, 0x20);        
	XC9080_I2C_WRITE(0x127f, 0x20);        
	XC9080_I2C_WRITE(0x1280, 0x20);        
	XC9080_I2C_WRITE(0x1281, 0x1e);        
	XC9080_I2C_WRITE(0x1282, 0x1f);        
	XC9080_I2C_WRITE(0x1283, 0x1f);        
	XC9080_I2C_WRITE(0x1284, 0x20);        
	XC9080_I2C_WRITE(0x1285, 0x20);        
	XC9080_I2C_WRITE(0x1286, 0x1f);        
	XC9080_I2C_WRITE(0x1287, 0x1f);        
	XC9080_I2C_WRITE(0x1288, 0x1e);        
	XC9080_I2C_WRITE(0x1289, 0x1c);        
	XC9080_I2C_WRITE(0x128a, 0x1e);        
	XC9080_I2C_WRITE(0x128b, 0x1e);        
	XC9080_I2C_WRITE(0x128c, 0x1f);        
	XC9080_I2C_WRITE(0x128d, 0x1f);        
	XC9080_I2C_WRITE(0x128e, 0x1f);        
	XC9080_I2C_WRITE(0x128f, 0x1e);        
	XC9080_I2C_WRITE(0x1290, 0x1e);        
	XC9080_I2C_WRITE(0x1291, 0x17);        
	XC9080_I2C_WRITE(0x1292, 0x1b);        
	XC9080_I2C_WRITE(0x1293, 0x1c);        
	XC9080_I2C_WRITE(0x1294, 0x1c);        
	XC9080_I2C_WRITE(0x1295, 0x1c);        
	XC9080_I2C_WRITE(0x1296, 0x1c);        
	XC9080_I2C_WRITE(0x1297, 0x1b);        
	XC9080_I2C_WRITE(0x1298, 0x1d);        
	XC9080_I2C_WRITE(0x1299, 0x19);        
	XC9080_I2C_WRITE(0x129a, 0x1f);        
	XC9080_I2C_WRITE(0x129b, 0x1f);        
	XC9080_I2C_WRITE(0x129c, 0x21);        
	XC9080_I2C_WRITE(0x129d, 0x21);        
	XC9080_I2C_WRITE(0x129e, 0x21);        
	XC9080_I2C_WRITE(0x129f, 0x1f);        
	XC9080_I2C_WRITE(0x12a0, 0x1e);        
	XC9080_I2C_WRITE(0x12a1, 0x22);        
	XC9080_I2C_WRITE(0x12a2, 0x21);        
	XC9080_I2C_WRITE(0x12a3, 0x22);        
	XC9080_I2C_WRITE(0x12a4, 0x22);        
	XC9080_I2C_WRITE(0x12a5, 0x22);        
	XC9080_I2C_WRITE(0x12a6, 0x22);        
	XC9080_I2C_WRITE(0x12a7, 0x22);        
	XC9080_I2C_WRITE(0x12a8, 0x21);        
	XC9080_I2C_WRITE(0x12a9, 0x23);        
	XC9080_I2C_WRITE(0x12aa, 0x21);        
	XC9080_I2C_WRITE(0x12ab, 0x21);        
	XC9080_I2C_WRITE(0x12ac, 0x21);        
	XC9080_I2C_WRITE(0x12ad, 0x21);        
	XC9080_I2C_WRITE(0x12ae, 0x21);        
	XC9080_I2C_WRITE(0x12af, 0x21);        
	XC9080_I2C_WRITE(0x12b0, 0x22);        
	XC9080_I2C_WRITE(0x12b1, 0x23);        
	XC9080_I2C_WRITE(0x12b2, 0x20);        
	XC9080_I2C_WRITE(0x12b3, 0x21);        
	XC9080_I2C_WRITE(0x12b4, 0x20);        
	XC9080_I2C_WRITE(0x12b5, 0x20);        
	XC9080_I2C_WRITE(0x12b6, 0x20);        
	XC9080_I2C_WRITE(0x12b7, 0x21);        
	XC9080_I2C_WRITE(0x12b8, 0x21);        
	XC9080_I2C_WRITE(0x12b9, 0x22);        
	XC9080_I2C_WRITE(0x12ba, 0x20);        
	XC9080_I2C_WRITE(0x12bb, 0x20);        
	XC9080_I2C_WRITE(0x12bc, 0x20);        
	XC9080_I2C_WRITE(0x12bd, 0x20);        
	XC9080_I2C_WRITE(0x12be, 0x20);        
	XC9080_I2C_WRITE(0x12bf, 0x20);        
	XC9080_I2C_WRITE(0x12c0, 0x21);        
	XC9080_I2C_WRITE(0x12c1, 0x23);        
	XC9080_I2C_WRITE(0x12c2, 0x21);        
	XC9080_I2C_WRITE(0x12c3, 0x21);        
	XC9080_I2C_WRITE(0x12c4, 0x21);        
	XC9080_I2C_WRITE(0x12c5, 0x21);        
	XC9080_I2C_WRITE(0x12c6, 0x21);        
	XC9080_I2C_WRITE(0x12c7, 0x21);        
	XC9080_I2C_WRITE(0x12c8, 0x20);        
	XC9080_I2C_WRITE(0x12c9, 0x22);        
	XC9080_I2C_WRITE(0x12ca, 0x21);        
	XC9080_I2C_WRITE(0x12cb, 0x21);        
	XC9080_I2C_WRITE(0x12cc, 0x21);        
	XC9080_I2C_WRITE(0x12cd, 0x21);        
	XC9080_I2C_WRITE(0x12ce, 0x21);        
	XC9080_I2C_WRITE(0x12cf, 0x21);        
	XC9080_I2C_WRITE(0x12d0, 0x22);        
	XC9080_I2C_WRITE(0x12d1, 0x1b);        
	XC9080_I2C_WRITE(0x12d2, 0x1e);        
	XC9080_I2C_WRITE(0x12d3, 0x20);        
	XC9080_I2C_WRITE(0x12d4, 0x21);        
	XC9080_I2C_WRITE(0x12d5, 0x22);        
	XC9080_I2C_WRITE(0x12d6, 0x20);        
	XC9080_I2C_WRITE(0x12d7, 0x20);        
	XC9080_I2C_WRITE(0x12d8, 0x1b);        
		
	//ISP1 AWB
	XC9080_I2C_WRITE(0xfffe, 0x14); //0805
	XC9080_I2C_WRITE(0x0bfc, 0x02);   //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
	XC9080_I2C_WRITE(0x0c36, 0x06);   //int B gain
	XC9080_I2C_WRITE(0x0c37, 0x00);  
	XC9080_I2C_WRITE(0x0c3a, 0x04);   //int Gb gain
	XC9080_I2C_WRITE(0x0c3b, 0x00); 
	XC9080_I2C_WRITE(0x0c3e, 0x04);   //int Gr gain
	XC9080_I2C_WRITE(0x0c3f, 0x00);
	XC9080_I2C_WRITE(0x0c42, 0x04);   //int R gain
	XC9080_I2C_WRITE(0x0c43, 0x04);
	XC9080_I2C_WRITE(0x0c6a, 0x06);    //B_temp      
	XC9080_I2C_WRITE(0x0c6b, 0x00);
	XC9080_I2C_WRITE(0x0c6e, 0x04);    //G_ temp          
	XC9080_I2C_WRITE(0x0c6f, 0x00);      
	XC9080_I2C_WRITE(0x0c72, 0x04);   //R_temp      
	XC9080_I2C_WRITE(0x0c73, 0x04);
	XC9080_I2C_WRITE(0xfffe, 0x14);
	XC9080_I2C_WRITE(0x0bfc, 0x01);  //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
	XC9080_I2C_WRITE(0x0bfd, 0x01);  //AWBFlexiMap_en
	XC9080_I2C_WRITE(0x0bfe, 0x00);  //AWBMove_en
	XC9080_I2C_WRITE(0x0c2e, 0x01);  //nCTBasedMinNum    //20
	XC9080_I2C_WRITE(0x0c2f, 0x00);
	XC9080_I2C_WRITE(0x0c30, 0x0f);
	XC9080_I2C_WRITE(0x0c31, 0xff); //nMaxAWBGain
	XC9080_I2C_WRITE(0xfffe, 0x31);
	XC9080_I2C_WRITE(0x0708, 0x02);
	XC9080_I2C_WRITE(0x0709, 0xa0);
	XC9080_I2C_WRITE(0x070a, 0x00);
	XC9080_I2C_WRITE(0x070b, 0x0c);
	XC9080_I2C_WRITE(0x071c, 0x0a); //simple awb
	//XC9080_I2C_WRITE(0x0003, 0x11);   //bit[4]:awb_en  bit[0]:awb_gain_en
	XC9080_I2C_WRITE(0xfffe, 0x31);
	XC9080_I2C_WRITE(0x0730, 0x8e);  
	XC9080_I2C_WRITE(0x0731, 0xb4);  
	XC9080_I2C_WRITE(0x0732, 0x40);  
	XC9080_I2C_WRITE(0x0733, 0x52);  
	XC9080_I2C_WRITE(0x0734, 0x70);  
	XC9080_I2C_WRITE(0x0735, 0x98);  
	XC9080_I2C_WRITE(0x0736, 0x60);  
	XC9080_I2C_WRITE(0x0737, 0x70);  
	XC9080_I2C_WRITE(0x0738, 0x4f);  
	XC9080_I2C_WRITE(0x0739, 0x5d);  
	XC9080_I2C_WRITE(0x073a, 0x6e);  
	XC9080_I2C_WRITE(0x073b, 0x80);  
	XC9080_I2C_WRITE(0x073c, 0x74);  
	XC9080_I2C_WRITE(0x073d, 0x93);  
	XC9080_I2C_WRITE(0x073e, 0x50);  
	XC9080_I2C_WRITE(0x073f, 0x61);  
	XC9080_I2C_WRITE(0x0740, 0x5c);  
	XC9080_I2C_WRITE(0x0741, 0x72);  
	XC9080_I2C_WRITE(0x0742, 0x63);  
	XC9080_I2C_WRITE(0x0743, 0x85);  
	XC9080_I2C_WRITE(0x0744, 0x00);  
	XC9080_I2C_WRITE(0x0745, 0x00);  
	XC9080_I2C_WRITE(0x0746, 0x00);  
	XC9080_I2C_WRITE(0x0747, 0x00);  
	XC9080_I2C_WRITE(0x0748, 0x00);  
	XC9080_I2C_WRITE(0x0749, 0x00);  
	XC9080_I2C_WRITE(0x074a, 0x00);  
	XC9080_I2C_WRITE(0x074b, 0x00);  
	XC9080_I2C_WRITE(0x074c, 0x00);  
	XC9080_I2C_WRITE(0x074d, 0x00);  
	XC9080_I2C_WRITE(0x074e, 0x00);  
	XC9080_I2C_WRITE(0x074f, 0x00);  
	XC9080_I2C_WRITE(0x0750, 0x00);  
	XC9080_I2C_WRITE(0x0751, 0x00);  
	XC9080_I2C_WRITE(0x0752, 0x00);  
	XC9080_I2C_WRITE(0x0753, 0x00);  
	XC9080_I2C_WRITE(0x0754, 0x00);  
	XC9080_I2C_WRITE(0x0755, 0x00);  
	XC9080_I2C_WRITE(0x0756, 0x00);  
	XC9080_I2C_WRITE(0x0757, 0x00);  
	XC9080_I2C_WRITE(0x0758, 0x00);  
	XC9080_I2C_WRITE(0x0759, 0x00);  
	XC9080_I2C_WRITE(0x075a, 0x00);  
	XC9080_I2C_WRITE(0x075b, 0x00);  
	XC9080_I2C_WRITE(0x075c, 0x00);  
	XC9080_I2C_WRITE(0x075d, 0x00);  
	XC9080_I2C_WRITE(0x075e, 0x00);  
	XC9080_I2C_WRITE(0x075f, 0x00);  
	XC9080_I2C_WRITE(0x0760, 0x00);  
	XC9080_I2C_WRITE(0x0761, 0x00);  
	XC9080_I2C_WRITE(0x0762, 0x00);  
	XC9080_I2C_WRITE(0x0763, 0x00);  
	XC9080_I2C_WRITE(0x0764, 0x00);  
	XC9080_I2C_WRITE(0x0765, 0x00);  
	XC9080_I2C_WRITE(0x0766, 0x00);  
	XC9080_I2C_WRITE(0x0767, 0x00);  
	XC9080_I2C_WRITE(0x0768, 0x00);  
	XC9080_I2C_WRITE(0x0769, 0x00);  
	XC9080_I2C_WRITE(0x076a, 0x00);  
	XC9080_I2C_WRITE(0x076b, 0x00);  
	XC9080_I2C_WRITE(0x076c, 0x00);  
	XC9080_I2C_WRITE(0x076d, 0x00);  
	XC9080_I2C_WRITE(0x076e, 0x00);  
	XC9080_I2C_WRITE(0x076f, 0x00);  
	XC9080_I2C_WRITE(0x0770, 0x22);  
	XC9080_I2C_WRITE(0x0771, 0x21);  
	XC9080_I2C_WRITE(0x0772, 0x10);  
	XC9080_I2C_WRITE(0x0773, 0x00);  
	XC9080_I2C_WRITE(0x0774, 0x00);  
	XC9080_I2C_WRITE(0x0775, 0x00);  
	XC9080_I2C_WRITE(0x0776, 0x00);  
	XC9080_I2C_WRITE(0x0777, 0x00);         
		                                    
	//TOP                                   
	//isp0 top
	XC9080_I2C_WRITE(0xfffe, 0x14);
#ifdef ENABLE_9080_AEC
	XC9080_I2C_WRITE(0x002b, 0x01);    //ae enable
#else
	XC9080_I2C_WRITE(0x002b, 0x00/*0x1*/);    //ae enable
#endif
	XC9080_I2C_WRITE(0x002c, 0x01);   //isp0 awb_en
	XC9080_I2C_WRITE(0x0030, 0x01);   //isp0 lenc_en
	XC9080_I2C_WRITE(0x0620, 0x01);   //lenc mode
	XC9080_I2C_WRITE(0x0621, 0x01);  
	XC9080_I2C_WRITE(0xfffe, 0x30);
	XC9080_I2C_WRITE(0x0000, 0x06); 
	XC9080_I2C_WRITE(0x0001, 0x00);                                   
	XC9080_I2C_WRITE(0x0002, 0x82); 
#ifndef ENABLE_9080_AWB//disable AWB
	XC9080_I2C_WRITE(0x0003, 0x20/*0x31*/); // awb enable/disable 
#else
	XC9080_I2C_WRITE(0x0003, 0x31); // awb enable/disable 
#endif
	XC9080_I2C_WRITE(0x0004, 0x00);
	XC9080_I2C_WRITE(0x0019, 0x0b);
#ifndef ENABLE_9080_AWB
	XC9080_I2C_WRITE(0x0051, 0x02/*0x01*/);
#else
	XC9080_I2C_WRITE(0x0051, 0x01);
#endif
	//isp1 top
	XC9080_I2C_WRITE(0xfffe,0x14);
#ifdef ENABLE_9080_AEC
	XC9080_I2C_WRITE(0x003c,0x01);   //isp1 AE enable
#else
	XC9080_I2C_WRITE(0x003c,0x00);   //isp1 AE enable
#endif
	XC9080_I2C_WRITE(0x003d,0x01);   //isp1 awb_en
	XC9080_I2C_WRITE(0x0041,0x01);   //isp1 lenc_en
	XC9080_I2C_WRITE(0x0fd4,0x01);   //lenc mode   
	XC9080_I2C_WRITE(0x0fd5,0x01);   
	XC9080_I2C_WRITE(0xfffe,0x31);
	XC9080_I2C_WRITE(0x0000,0x06); 
	XC9080_I2C_WRITE(0x0001,0x00); 
	//XC9080_I2C_WRITE(0x0002,0x00); 
        XC9080_I2C_WRITE(0x0002,0x82); 
#ifndef ENABLE_9080_AWB
	XC9080_I2C_WRITE(0x0003,0x20);
#else
	XC9080_I2C_WRITE(0x0003,0x31);    
#endif       
	XC9080_I2C_WRITE(0x0004,0x00);

	XC9080_I2C_WRITE(0x0019,0x0b);
#ifndef ENABLE_9080_AWB
	XC9080_I2C_WRITE(0x0051, 0x02/*0x01*/);
#else
	XC9080_I2C_WRITE(0x0051, 0x01);
#endif

#else // v1.2 enable AE&AWB
XC9080_I2C_WRITE(0xfffd, 0x80);

//Initial
//////////RESET&PLL//////////
XC9080_I2C_WRITE(0xfffe, 0x50);
XC9080_I2C_WRITE(0x001c, 0xff);
XC9080_I2C_WRITE(0x001d, 0xff);
XC9080_I2C_WRITE(0x001e, 0xff);
XC9080_I2C_WRITE(0x001f, 0xff);
XC9080_I2C_WRITE(0x0018, 0x00);
XC9080_I2C_WRITE(0x0019, 0x00);
XC9080_I2C_WRITE(0x001a, 0x00);
XC9080_I2C_WRITE(0x001b, 0x00);

XC9080_I2C_WRITE(0x0030, 0x44);
XC9080_I2C_WRITE(0x0031, 0x04);
XC9080_I2C_WRITE(0x0032, 0x32);
XC9080_I2C_WRITE(0x0033, 0x31);

XC9080_I2C_WRITE(0x0020, 0x03);
XC9080_I2C_WRITE(0x0021, 0x0d);
XC9080_I2C_WRITE(0x0022, 0x01);
XC9080_I2C_WRITE(0x0023, 0x86);
XC9080_I2C_WRITE(0x0024, 0x01);
XC9080_I2C_WRITE(0x0025, 0x04);
XC9080_I2C_WRITE(0x0026, 0x02);
XC9080_I2C_WRITE(0x0027, 0x06);
XC9080_I2C_WRITE(0x0028, 0x01);
XC9080_I2C_WRITE(0x0029, 0x00);
XC9080_I2C_WRITE(0x002a, 0x02);
XC9080_I2C_WRITE(0x002b, 0x05);

XC9080_I2C_WRITE(0xfffe, 0x50);
XC9080_I2C_WRITE(0x0050, 0x0f);
XC9080_I2C_WRITE(0x0054, 0x0f);
XC9080_I2C_WRITE(0x0058, 0x00);
XC9080_I2C_WRITE(0x0058, 0x0a);

//////////PAD&DATASEL//////////
XC9080_I2C_WRITE(0xfffe, 0x50);
XC9080_I2C_WRITE(0x00bc, 0x19);
XC9080_I2C_WRITE(0x0090, 0x38);
XC9080_I2C_WRITE(0x00a8, 0x09);

XC9080_I2C_WRITE(0x0200, 0x0f);
XC9080_I2C_WRITE(0x0201, 0x00);
XC9080_I2C_WRITE(0x0202, 0x80);
XC9080_I2C_WRITE(0x0203, 0x00);

XC9080_I2C_WRITE(0x0214, 0x0f);
XC9080_I2C_WRITE(0x0215, 0x00);
XC9080_I2C_WRITE(0x0216, 0x80);
XC9080_I2C_WRITE(0x0217, 0x00);

//////////COLORBAR&CROP////////
XC9080_I2C_WRITE(0xfffe, 0x26);
XC9080_I2C_WRITE(0x8001, 0x88);
XC9080_I2C_WRITE(0x8002, 0x07);
XC9080_I2C_WRITE(0x8003, 0x40);
XC9080_I2C_WRITE(0x8004, 0x04);
XC9080_I2C_WRITE(0x8005, 0x40);
XC9080_I2C_WRITE(0x8006, 0x40);
XC9080_I2C_WRITE(0x8007, 0x10);
XC9080_I2C_WRITE(0x8008, 0xf0);
XC9080_I2C_WRITE(0x800b, 0x00);
XC9080_I2C_WRITE(0x8000, 0x0d);

XC9080_I2C_WRITE(0x8041, 0x88);
XC9080_I2C_WRITE(0x8042, 0x07);
XC9080_I2C_WRITE(0x8043, 0x40);
XC9080_I2C_WRITE(0x8044, 0x04);
XC9080_I2C_WRITE(0x8045, 0x03);
XC9080_I2C_WRITE(0x8046, 0x10);
XC9080_I2C_WRITE(0x8047, 0x10);
XC9080_I2C_WRITE(0x8048, 0xff);
XC9080_I2C_WRITE(0x804b, 0x00);
XC9080_I2C_WRITE(0x8040, 0x0d);

XC9080_I2C_WRITE(0x8010, 0x05);
XC9080_I2C_WRITE(0x8012, 0x38);
XC9080_I2C_WRITE(0x8013, 0x04);
XC9080_I2C_WRITE(0x8014, 0x38);
XC9080_I2C_WRITE(0x8015, 0x04);
XC9080_I2C_WRITE(0x8016, 0xa5);
XC9080_I2C_WRITE(0x8017, 0x01);
XC9080_I2C_WRITE(0x8018, 0x01);
XC9080_I2C_WRITE(0x8019, 0x00);

XC9080_I2C_WRITE(0x8050, 0x05);
XC9080_I2C_WRITE(0x8052, 0x38);
XC9080_I2C_WRITE(0x8053, 0x04);
XC9080_I2C_WRITE(0x8054, 0x38);
XC9080_I2C_WRITE(0x8055, 0x04);
XC9080_I2C_WRITE(0x8056, 0xa5);
XC9080_I2C_WRITE(0x8057, 0x01);
XC9080_I2C_WRITE(0x8058, 0x01);
XC9080_I2C_WRITE(0x8059, 0x00);

XC9080_I2C_WRITE(0xfffe, 0x2e);
XC9080_I2C_WRITE(0x0026, 0xc0);

XC9080_I2C_WRITE(0x0100, 0x00);
XC9080_I2C_WRITE(0x0101, 0x01);
XC9080_I2C_WRITE(0x0102, 0x00);
XC9080_I2C_WRITE(0x0103, 0x05);
XC9080_I2C_WRITE(0x0104, 0x07);
XC9080_I2C_WRITE(0x0105, 0x88);
XC9080_I2C_WRITE(0x0106, 0x04);
XC9080_I2C_WRITE(0x0107, 0x40);
XC9080_I2C_WRITE(0x0108, 0x01);

XC9080_I2C_WRITE(0x0200, 0x00);
XC9080_I2C_WRITE(0x0201, 0x01);
XC9080_I2C_WRITE(0x0202, 0x00);
XC9080_I2C_WRITE(0x0203, 0x05);
XC9080_I2C_WRITE(0x0204, 0x07);
XC9080_I2C_WRITE(0x0205, 0x88);
XC9080_I2C_WRITE(0x0206, 0x04);
XC9080_I2C_WRITE(0x0207, 0x40);
XC9080_I2C_WRITE(0x0208, 0x01);

//////////ISPdatapath////////

XC9080_I2C_WRITE(0xfffe, 0x30);
XC9080_I2C_WRITE(0x0019, 0x01);
XC9080_I2C_WRITE(0x0050, 0x20);
XC9080_I2C_WRITE(0x0004, 0x00);

XC9080_I2C_WRITE(0x005e, 0x37);
XC9080_I2C_WRITE(0x005f, 0x04);
XC9080_I2C_WRITE(0x0060, 0x37);
XC9080_I2C_WRITE(0x0061, 0x04);
XC9080_I2C_WRITE(0x0064, 0x38);
XC9080_I2C_WRITE(0x0065, 0x04);
XC9080_I2C_WRITE(0x0066, 0x38);
XC9080_I2C_WRITE(0x0067, 0x04);

XC9080_I2C_WRITE(0x0006, 0x04);
XC9080_I2C_WRITE(0x0007, 0x38);
XC9080_I2C_WRITE(0x0008, 0x04);
XC9080_I2C_WRITE(0x0009, 0x38);
XC9080_I2C_WRITE(0x000a, 0x04);
XC9080_I2C_WRITE(0x000b, 0x38);
XC9080_I2C_WRITE(0x000c, 0x04);
XC9080_I2C_WRITE(0x000d, 0x38);

XC9080_I2C_WRITE(0xfffe, 0x31);
XC9080_I2C_WRITE(0x0019, 0x01);
XC9080_I2C_WRITE(0x0050, 0x20);
XC9080_I2C_WRITE(0x0004, 0x00);

XC9080_I2C_WRITE(0x005e, 0x37);
XC9080_I2C_WRITE(0x005f, 0x04);
XC9080_I2C_WRITE(0x0060, 0x37);
XC9080_I2C_WRITE(0x0061, 0x04);
XC9080_I2C_WRITE(0x0064, 0x38);
XC9080_I2C_WRITE(0x0065, 0x04);
XC9080_I2C_WRITE(0x0066, 0x38);
XC9080_I2C_WRITE(0x0067, 0x04);

XC9080_I2C_WRITE(0x0006, 0x04);
XC9080_I2C_WRITE(0x0007, 0x38);
XC9080_I2C_WRITE(0x0008, 0x04);
XC9080_I2C_WRITE(0x0009, 0x38);
XC9080_I2C_WRITE(0x000a, 0x04);
XC9080_I2C_WRITE(0x000b, 0x38);
XC9080_I2C_WRITE(0x000c, 0x04);
XC9080_I2C_WRITE(0x000d, 0x38);

////////MIPI & FIFO STITCH/////////

XC9080_I2C_WRITE(0xfffe, 0x26);
XC9080_I2C_WRITE(0x0000, 0x20);
XC9080_I2C_WRITE(0x0009, 0xc4);

XC9080_I2C_WRITE(0x1000, 0x20);
XC9080_I2C_WRITE(0x1009, 0xc4);

XC9080_I2C_WRITE(0x2019, 0x08);
XC9080_I2C_WRITE(0x201a, 0x70);
XC9080_I2C_WRITE(0x201b, 0x04);
XC9080_I2C_WRITE(0x201c, 0x38);
XC9080_I2C_WRITE(0x201d, 0x08);
XC9080_I2C_WRITE(0x201e, 0x70);
XC9080_I2C_WRITE(0x201f, 0x04);
XC9080_I2C_WRITE(0x2020, 0x38);

XC9080_I2C_WRITE(0x2015, 0x80);
XC9080_I2C_WRITE(0x2017, 0x2b);
XC9080_I2C_WRITE(0x2018, 0x2b);
XC9080_I2C_WRITE(0x2023, 0x0f);

XC9080_I2C_WRITE(0xfffe, 0x2c);
XC9080_I2C_WRITE(0x0000, 0x40);
XC9080_I2C_WRITE(0x0001, 0x08);
XC9080_I2C_WRITE(0x0002, 0x70);
XC9080_I2C_WRITE(0x0004, 0x04);
XC9080_I2C_WRITE(0x0005, 0x38);
XC9080_I2C_WRITE(0x0008, 0x10);

XC9080_I2C_WRITE(0x0044, 0x0a);	
XC9080_I2C_WRITE(0x0045, 0x04);
XC9080_I2C_WRITE(0x0048, 0x08);
XC9080_I2C_WRITE(0x0049, 0x60);

XC9080_I2C_WRITE(0x0084, 0x0a);	
XC9080_I2C_WRITE(0x0085, 0x04);
XC9080_I2C_WRITE(0x0088, 0x08);
XC9080_I2C_WRITE(0x0089, 0x60);

////////RETIMMNG&MERGE/////////
XC9080_I2C_WRITE(0xfffe, 0x2e);
XC9080_I2C_WRITE(0x0000, 0x42);

XC9080_I2C_WRITE(0x0001, 0xee);
XC9080_I2C_WRITE(0x0003, 0x01);
XC9080_I2C_WRITE(0x0004, 0xa0);

XC9080_I2C_WRITE(0x0006, 0x01);
XC9080_I2C_WRITE(0x0007, 0xa0);

XC9080_I2C_WRITE(0x000a, 0x13);
XC9080_I2C_WRITE(0x000b, 0x04);
XC9080_I2C_WRITE(0x000c, 0x38);
XC9080_I2C_WRITE(0x000d, 0x04);

XC9080_I2C_WRITE(0x1000, 0x0a);
XC9080_I2C_WRITE(0x1001, 0x70);
XC9080_I2C_WRITE(0x1002, 0x08);
XC9080_I2C_WRITE(0x1003, 0x38);
XC9080_I2C_WRITE(0x1004, 0x04);
XC9080_I2C_WRITE(0x1005, 0x70);
XC9080_I2C_WRITE(0x1006, 0x08);
XC9080_I2C_WRITE(0x1007, 0x38);
XC9080_I2C_WRITE(0x1008, 0x04);
XC9080_I2C_WRITE(0x1009, 0x70);
XC9080_I2C_WRITE(0x100a, 0x08);
XC9080_I2C_WRITE(0x100b, 0x00);

XC9080_I2C_WRITE(0x100e, 0x00);
XC9080_I2C_WRITE(0x100f, 0x01);

XC9080_I2C_WRITE(0xfffe, 0x26);
XC9080_I2C_WRITE(0x200b, 0x25);
XC9080_I2C_WRITE(0x200c, 0x04);

//ISP0 AE
//// 1.  AE statistical //////
//////////////////////////////
XC9080_I2C_WRITE(0xfffe, 0x30); // AE_avg

XC9080_I2C_WRITE(0x1f00, 0x00);
XC9080_I2C_WRITE(0x1f01, 0x9f);
XC9080_I2C_WRITE(0x1f02, 0x00);
XC9080_I2C_WRITE(0x1f03, 0x9f);
XC9080_I2C_WRITE(0x1f04, 0x02);
XC9080_I2C_WRITE(0x1f05, 0xf0);
XC9080_I2C_WRITE(0x1f06, 0x02);
XC9080_I2C_WRITE(0x1f07, 0xf0);
XC9080_I2C_WRITE(0x1f08, 0x03);

XC9080_I2C_WRITE(0x0051, 0x01);

//////////// 2.  AE SENSOR     //////
/////////////////////////////////////
XC9080_I2C_WRITE(0xfffe, 0x14);
XC9080_I2C_WRITE(0x000e, 0x00);
XC9080_I2C_WRITE(0x010e, 0x48);
XC9080_I2C_WRITE(0x010f, 0x01);
XC9080_I2C_WRITE(0x0110, 0x0f);
XC9080_I2C_WRITE(0x0111, 0x0f);

//exposure
XC9080_I2C_WRITE(0x0114, 0x02);
XC9080_I2C_WRITE(0x0115, 0x02);
XC9080_I2C_WRITE(0x0116, 0x02);
XC9080_I2C_WRITE(0x0117, 0x03);

XC9080_I2C_WRITE(0x011c, 0x00);
XC9080_I2C_WRITE(0x011d, 0xff);
XC9080_I2C_WRITE(0x011e, 0x00);
XC9080_I2C_WRITE(0x011f, 0xff);

//gain
XC9080_I2C_WRITE(0x0134, 0x02);
XC9080_I2C_WRITE(0x0135, 0x05);

XC9080_I2C_WRITE(0x013c, 0x00);
XC9080_I2C_WRITE(0x013d, 0xff);

XC9080_I2C_WRITE(0x0145, 0x00);

XC9080_I2C_WRITE(0x0031, 0x01);
XC9080_I2C_WRITE(0x0032, 0x01);

///////// 3.  AE NORMAL     //////
////////////////////////////////// 
XC9080_I2C_WRITE(0xfffe, 0x14);
XC9080_I2C_WRITE(0x004c, 0x0);
#ifdef ENABLE_9080_AEC
XC9080_I2C_WRITE(0x002b, 0x1);
#else
XC9080_I2C_WRITE(0x002b, 0x0);
#endif
XC9080_I2C_WRITE(0x004d, 0x1);

XC9080_I2C_WRITE(0x00fa, 0x01);
XC9080_I2C_WRITE(0x00fb, 0xf0);

XC9080_I2C_WRITE(0x00fc, 0x00);
XC9080_I2C_WRITE(0x00fd, 0x20);

XC9080_I2C_WRITE(0x00e2, 0x42);
XC9080_I2C_WRITE(0x00e3, 0x00);

XC9080_I2C_WRITE(0x00de, 0x0);
XC9080_I2C_WRITE(0x00df, 0x10);

XC9080_I2C_WRITE(0x00a0, 0x01);
XC9080_I2C_WRITE(0x00a1, 0xc0);

//flicker
XC9080_I2C_WRITE(0x0104, 0x02);
XC9080_I2C_WRITE(0x0105, 0x01);
XC9080_I2C_WRITE(0x0108, 0x14);
XC9080_I2C_WRITE(0x0109, 0xc0);

//weight
XC9080_I2C_WRITE(0x0055, 0x04);
XC9080_I2C_WRITE(0x0056, 0x04);
XC9080_I2C_WRITE(0x0057, 0x04);
XC9080_I2C_WRITE(0x0058, 0x04);
XC9080_I2C_WRITE(0x0059, 0x04);
 
XC9080_I2C_WRITE(0x005a, 0x04);
XC9080_I2C_WRITE(0x005b, 0x08);
XC9080_I2C_WRITE(0x005c, 0x08);
XC9080_I2C_WRITE(0x005d, 0x08);
XC9080_I2C_WRITE(0x005e, 0x04);
 
XC9080_I2C_WRITE(0x005f, 0x04);
                                         
XC9080_I2C_WRITE(0x0060, 0x08);
XC9080_I2C_WRITE(0x0061, 0x08);
XC9080_I2C_WRITE(0x0062, 0x08);
XC9080_I2C_WRITE(0x0063, 0x04);
 
XC9080_I2C_WRITE(0x0064, 0x04);
XC9080_I2C_WRITE(0x0065, 0x08);
XC9080_I2C_WRITE(0x0066, 0x08);
XC9080_I2C_WRITE(0x0067, 0x08);
XC9080_I2C_WRITE(0x0068, 0x04);
 
XC9080_I2C_WRITE(0x0069, 0x04);
XC9080_I2C_WRITE(0x006a, 0x04);
XC9080_I2C_WRITE(0x006b, 0x04);
XC9080_I2C_WRITE(0x006c, 0x04);
XC9080_I2C_WRITE(0x006d, 0x04);
          
XC9080_I2C_WRITE(0x0088, 0x00);
XC9080_I2C_WRITE(0x0089, 0x07);
XC9080_I2C_WRITE(0x008a, 0x39);
XC9080_I2C_WRITE(0x008b, 0xc0);
          
XC9080_I2C_WRITE(0x0050, 0x01);

//////////  4.AE SPEED       /////
//////////////////////////////////
//XC9080_I2C_WRITE(0x00c6, 0x01);

XC9080_I2C_WRITE(0x00ca, 0x00);
XC9080_I2C_WRITE(0x00cb, 0x80);

XC9080_I2C_WRITE(0x01bc, 0x00);
XC9080_I2C_WRITE(0x01bd, 0xa0);

XC9080_I2C_WRITE(0x01be, 0x00);
XC9080_I2C_WRITE(0x01bf, 0x60);

XC9080_I2C_WRITE(0x00cc, 0x00);
XC9080_I2C_WRITE(0x00cd, 0x50);

XC9080_I2C_WRITE(0x00c7, 0x20);
XC9080_I2C_WRITE(0x00d8, 0x80);

XC9080_I2C_WRITE(0x00c8, 0x01);
XC9080_I2C_WRITE(0x0208, 0x03);

XC9080_I2C_WRITE(0x00da, 0x00);
XC9080_I2C_WRITE(0x00db, 0xa0);
XC9080_I2C_WRITE(0x00dc, 0x03);
XC9080_I2C_WRITE(0x00dd, 0x00);
//////////  5.AE SMART       ////
/////////////////////////////////
XC9080_I2C_WRITE(0x0092, 0x01);
XC9080_I2C_WRITE(0x0093, 0x07);
XC9080_I2C_WRITE(0x0094, 0x00);

XC9080_I2C_WRITE(0x00ad, 0x02);

////////////table reftarget/////////
XC9080_I2C_WRITE(0x0022, 0x1e);
XC9080_I2C_WRITE(0x01e4, 0x00);
XC9080_I2C_WRITE(0x01e5, 0x00);
XC9080_I2C_WRITE(0x01e6, 0x04);
XC9080_I2C_WRITE(0x01e7, 0x00);
XC9080_I2C_WRITE(0x01e8, 0x00);
XC9080_I2C_WRITE(0x01e9, 0x00);
XC9080_I2C_WRITE(0x01ea, 0x0c);
XC9080_I2C_WRITE(0x01eb, 0x00);
XC9080_I2C_WRITE(0x01ec, 0x00);
XC9080_I2C_WRITE(0x01ed, 0x00);
XC9080_I2C_WRITE(0x01ee, 0x24);
XC9080_I2C_WRITE(0x01ef, 0x00);
XC9080_I2C_WRITE(0x01f0, 0x00);
XC9080_I2C_WRITE(0x01f1, 0x00);
XC9080_I2C_WRITE(0x01f2, 0x6c);
XC9080_I2C_WRITE(0x01f3, 0x00);
XC9080_I2C_WRITE(0x01f4, 0x00);
XC9080_I2C_WRITE(0x01f5, 0x01);
XC9080_I2C_WRITE(0x01f6, 0x44);
XC9080_I2C_WRITE(0x01f7, 0x00);
XC9080_I2C_WRITE(0x01f8, 0x00);
XC9080_I2C_WRITE(0x01f9, 0x04);
XC9080_I2C_WRITE(0x01fa, 0x00);
XC9080_I2C_WRITE(0x01fb, 0x00);

//////////reftarget///////////////
XC9080_I2C_WRITE(0x00b2, 0x02);
XC9080_I2C_WRITE(0x00b3, 0x80);
XC9080_I2C_WRITE(0x00b4, 0x02);
XC9080_I2C_WRITE(0x00b5, 0x60);
XC9080_I2C_WRITE(0x00b6, 0x02);
XC9080_I2C_WRITE(0x00b7, 0x40);
XC9080_I2C_WRITE(0x00b8, 0x01);
XC9080_I2C_WRITE(0x00b9, 0xf0);
XC9080_I2C_WRITE(0x00ba, 0x01);
XC9080_I2C_WRITE(0x00bb, 0xb0);
XC9080_I2C_WRITE(0x00bc, 0x00);
XC9080_I2C_WRITE(0x00bd, 0xf0);

//基于平均亮度维度分? 
XC9080_I2C_WRITE(0x01cd, 0x28);
XC9080_I2C_WRITE(0x01cb, 0x00);
XC9080_I2C_WRITE(0x01cc, 0x40);

///////////over exposure offest//////
XC9080_I2C_WRITE(0x01d6, 0x00);
XC9080_I2C_WRITE(0x01d7, 0x04);
XC9080_I2C_WRITE(0x01d8, 0x14);
XC9080_I2C_WRITE(0x01d9, 0x50);
XC9080_I2C_WRITE(0x01da, 0x70);
XC9080_I2C_WRITE(0x01db, 0x90);

XC9080_I2C_WRITE(0x01c0, 0x08);
XC9080_I2C_WRITE(0x01c7, 0x06);
XC9080_I2C_WRITE(0x01c9, 0x02);
XC9080_I2C_WRITE(0x01ca, 0x30);
XC9080_I2C_WRITE(0x01d1, 0x40);

//基于方差维度分析模式/Variance  /////////////////
XC9080_I2C_WRITE(0x01b1, 0x30);

XC9080_I2C_WRITE(0x01b2, 0x01);
XC9080_I2C_WRITE(0x01b3, 0x00);
XC9080_I2C_WRITE(0x01b4, 0x06);
XC9080_I2C_WRITE(0x01b5, 0x00);

//基于关注区亮度压制

XC9080_I2C_WRITE(0x016e, 0x02);
XC9080_I2C_WRITE(0x016f, 0x80);

XC9080_I2C_WRITE(0x01d3, 0x00);
XC9080_I2C_WRITE(0x01d4, 0x30);

XC9080_I2C_WRITE(0x016c, 0x00);
XC9080_I2C_WRITE(0x016d, 0x00);

XC9080_I2C_WRITE(0x01ba, 0x8);

////////////smart ae end /////////
XC9080_I2C_WRITE(0x1a74, 0x02);
XC9080_I2C_WRITE(0x0212, 0x00);

/////////////AE  END/////////////////
////////////////////////////////////

//ISP0 BLC              
XC9080_I2C_WRITE(0xfffe, 0x30);

XC9080_I2C_WRITE(0x0013, 0x20); //bias   
XC9080_I2C_WRITE(0x0014, 0x00);

XC9080_I2C_WRITE(0x071b, 0x80); //blc   

//ISP0 LENC                 
XC9080_I2C_WRITE(0xfffe, 0x30);
//XC9080_I2C_WRITE(0x0002, 0x80);//bit[7]:lenc_en
XC9080_I2C_WRITE(0x03ca, 0x16);
XC9080_I2C_WRITE(0x03cb, 0xC1);
XC9080_I2C_WRITE(0x03cc, 0x16);
XC9080_I2C_WRITE(0x03cd, 0xC1);
XC9080_I2C_WRITE(0x03ce, 0x16);
XC9080_I2C_WRITE(0x03cf, 0xC1);
XC9080_I2C_WRITE(0x03d0, 0x0B);
XC9080_I2C_WRITE(0x03d1, 0x60);
         
         
XC9080_I2C_WRITE(0xfffe, 0x14);
        
XC9080_I2C_WRITE(0x002c, 0x01);
XC9080_I2C_WRITE(0x0030, 0x01);
XC9080_I2C_WRITE(0x0620, 0x01);
XC9080_I2C_WRITE(0x0621, 0x01);
         
XC9080_I2C_WRITE(0x0928, 0x00);	//ct  
XC9080_I2C_WRITE(0x0929, 0x67);
XC9080_I2C_WRITE(0x092a, 0x00);
XC9080_I2C_WRITE(0x092b, 0xb3);
XC9080_I2C_WRITE(0x092c, 0x01);
XC9080_I2C_WRITE(0x092d, 0x32);
         
XC9080_I2C_WRITE(0x06e5, 0x3c); //A _9,20161101_v2
XC9080_I2C_WRITE(0x06e6, 0x1e);
XC9080_I2C_WRITE(0x06e7, 0x15);
XC9080_I2C_WRITE(0x06e8, 0x11);
XC9080_I2C_WRITE(0x06e9, 0x11);
XC9080_I2C_WRITE(0x06ea, 0x13);
XC9080_I2C_WRITE(0x06eb, 0x1c);
XC9080_I2C_WRITE(0x06ec, 0x32);
XC9080_I2C_WRITE(0x06ed, 0x8 );
XC9080_I2C_WRITE(0x06ee, 0x5 );
XC9080_I2C_WRITE(0x06ef, 0x3 );
XC9080_I2C_WRITE(0x06f0, 0x3 );
XC9080_I2C_WRITE(0x06f1, 0x3 );
XC9080_I2C_WRITE(0x06f2, 0x3 );
XC9080_I2C_WRITE(0x06f3, 0x5 );
XC9080_I2C_WRITE(0x06f4, 0x9 );
XC9080_I2C_WRITE(0x06f5, 0x3 );
XC9080_I2C_WRITE(0x06f6, 0x1 );
XC9080_I2C_WRITE(0x06f7, 0x1 );
XC9080_I2C_WRITE(0x06f8, 0x1 );
XC9080_I2C_WRITE(0x06f9, 0x1 );
XC9080_I2C_WRITE(0x06fa, 0x1 );
XC9080_I2C_WRITE(0x06fb, 0x2 );
XC9080_I2C_WRITE(0x06fc, 0x4 );
XC9080_I2C_WRITE(0x06fd, 0x1 );
XC9080_I2C_WRITE(0x06fe, 0x0 );
XC9080_I2C_WRITE(0x06ff, 0x0 );
XC9080_I2C_WRITE(0x0700, 0x0 );
XC9080_I2C_WRITE(0x0701, 0x0 );
XC9080_I2C_WRITE(0x0702, 0x0 );
XC9080_I2C_WRITE(0x0703, 0x1 );
XC9080_I2C_WRITE(0x0704, 0x1 );
XC9080_I2C_WRITE(0x0705, 0x1 );
XC9080_I2C_WRITE(0x0706, 0x0 );
XC9080_I2C_WRITE(0x0707, 0x0 );
XC9080_I2C_WRITE(0x0708, 0x0 );
XC9080_I2C_WRITE(0x0709, 0x0 );
XC9080_I2C_WRITE(0x070a, 0x0 );
XC9080_I2C_WRITE(0x070b, 0x1 );
XC9080_I2C_WRITE(0x070c, 0x2 );
XC9080_I2C_WRITE(0x070d, 0x3 );
XC9080_I2C_WRITE(0x070e, 0x1 );
XC9080_I2C_WRITE(0x070f, 0x1 );
XC9080_I2C_WRITE(0x0710, 0x1 );
XC9080_I2C_WRITE(0x0711, 0x1 );
XC9080_I2C_WRITE(0x0712, 0x1 );
XC9080_I2C_WRITE(0x0713, 0x2 );
XC9080_I2C_WRITE(0x0714, 0x3 );
XC9080_I2C_WRITE(0x0715, 0x9 );
XC9080_I2C_WRITE(0x0716, 0x6 );
XC9080_I2C_WRITE(0x0717, 0x3 );
XC9080_I2C_WRITE(0x0718, 0x3 );
XC9080_I2C_WRITE(0x0719, 0x3 );
XC9080_I2C_WRITE(0x071a, 0x4 );
XC9080_I2C_WRITE(0x071b, 0x5 );
XC9080_I2C_WRITE(0x071c, 0xb );
XC9080_I2C_WRITE(0x071d, 0x3f);
XC9080_I2C_WRITE(0x071e, 0x21);
XC9080_I2C_WRITE(0x071f, 0x16);
XC9080_I2C_WRITE(0x0720, 0x12);
XC9080_I2C_WRITE(0x0721, 0x11);
XC9080_I2C_WRITE(0x0722, 0x14);
XC9080_I2C_WRITE(0x0723, 0x20);
XC9080_I2C_WRITE(0x0724, 0x28);
XC9080_I2C_WRITE(0x0725, 0xf );
XC9080_I2C_WRITE(0x0726, 0x1a);
XC9080_I2C_WRITE(0x0727, 0x1b);
XC9080_I2C_WRITE(0x0728, 0x1c);
XC9080_I2C_WRITE(0x0729, 0x1c);
XC9080_I2C_WRITE(0x072a, 0x1a);
XC9080_I2C_WRITE(0x072b, 0x18);
XC9080_I2C_WRITE(0x072c, 0x14);
XC9080_I2C_WRITE(0x072d, 0x1d);
XC9080_I2C_WRITE(0x072e, 0x1e);
XC9080_I2C_WRITE(0x072f, 0x1f);
XC9080_I2C_WRITE(0x0730, 0x1f);
XC9080_I2C_WRITE(0x0731, 0x1f);
XC9080_I2C_WRITE(0x0732, 0x20);
XC9080_I2C_WRITE(0x0733, 0x1f);
XC9080_I2C_WRITE(0x0734, 0x1e);
XC9080_I2C_WRITE(0x0735, 0x1d);
XC9080_I2C_WRITE(0x0736, 0x20);
XC9080_I2C_WRITE(0x0737, 0x1f);
XC9080_I2C_WRITE(0x0738, 0x20);
XC9080_I2C_WRITE(0x0739, 0x20);
XC9080_I2C_WRITE(0x073a, 0x20);
XC9080_I2C_WRITE(0x073b, 0x1f);
XC9080_I2C_WRITE(0x073c, 0x20);
XC9080_I2C_WRITE(0x073d, 0x1e);
XC9080_I2C_WRITE(0x073e, 0x20);
XC9080_I2C_WRITE(0x073f, 0x20);
XC9080_I2C_WRITE(0x0740, 0x20);
XC9080_I2C_WRITE(0x0741, 0x20);
XC9080_I2C_WRITE(0x0742, 0x20);
XC9080_I2C_WRITE(0x0743, 0x20);
XC9080_I2C_WRITE(0x0744, 0x21);
XC9080_I2C_WRITE(0x0745, 0x1e);
XC9080_I2C_WRITE(0x0746, 0x20);
XC9080_I2C_WRITE(0x0747, 0x20);
XC9080_I2C_WRITE(0x0748, 0x20);
XC9080_I2C_WRITE(0x0749, 0x20);
XC9080_I2C_WRITE(0x074a, 0x21);
XC9080_I2C_WRITE(0x074b, 0x21);
XC9080_I2C_WRITE(0x074c, 0x20);
XC9080_I2C_WRITE(0x074d, 0x1d);
XC9080_I2C_WRITE(0x074e, 0x20);
XC9080_I2C_WRITE(0x074f, 0x20);
XC9080_I2C_WRITE(0x0750, 0x20);
XC9080_I2C_WRITE(0x0751, 0x20);
XC9080_I2C_WRITE(0x0752, 0x20);
XC9080_I2C_WRITE(0x0753, 0x20);
XC9080_I2C_WRITE(0x0754, 0x20);
XC9080_I2C_WRITE(0x0755, 0x1c);
XC9080_I2C_WRITE(0x0756, 0x1f);
XC9080_I2C_WRITE(0x0757, 0x1f);
XC9080_I2C_WRITE(0x0758, 0x20);
XC9080_I2C_WRITE(0x0759, 0x20);
XC9080_I2C_WRITE(0x075a, 0x1f);
XC9080_I2C_WRITE(0x075b, 0x20);
XC9080_I2C_WRITE(0x075c, 0x1d);
XC9080_I2C_WRITE(0x075d, 0xe );
XC9080_I2C_WRITE(0x075e, 0x18);
XC9080_I2C_WRITE(0x075f, 0x1a);
XC9080_I2C_WRITE(0x0760, 0x1b);
XC9080_I2C_WRITE(0x0761, 0x1b);
XC9080_I2C_WRITE(0x0762, 0x1b);
XC9080_I2C_WRITE(0x0763, 0x17);
XC9080_I2C_WRITE(0x0764, 0x1b);
XC9080_I2C_WRITE(0x0765, 0x1f);
XC9080_I2C_WRITE(0x0766, 0x22);
XC9080_I2C_WRITE(0x0767, 0x22);
XC9080_I2C_WRITE(0x0768, 0x23);
XC9080_I2C_WRITE(0x0769, 0x24);
XC9080_I2C_WRITE(0x076a, 0x24);
XC9080_I2C_WRITE(0x076b, 0x23);
XC9080_I2C_WRITE(0x076c, 0x22);
XC9080_I2C_WRITE(0x076d, 0x25);
XC9080_I2C_WRITE(0x076e, 0x23);
XC9080_I2C_WRITE(0x076f, 0x23);
XC9080_I2C_WRITE(0x0770, 0x22);
XC9080_I2C_WRITE(0x0771, 0x22);
XC9080_I2C_WRITE(0x0772, 0x22);
XC9080_I2C_WRITE(0x0773, 0x22);
XC9080_I2C_WRITE(0x0774, 0x23);
XC9080_I2C_WRITE(0x0775, 0x24);
XC9080_I2C_WRITE(0x0776, 0x21);
XC9080_I2C_WRITE(0x0777, 0x21);
XC9080_I2C_WRITE(0x0778, 0x21);
XC9080_I2C_WRITE(0x0779, 0x21);
XC9080_I2C_WRITE(0x077a, 0x21);
XC9080_I2C_WRITE(0x077b, 0x21);
XC9080_I2C_WRITE(0x077c, 0x22);
XC9080_I2C_WRITE(0x077d, 0x23);
XC9080_I2C_WRITE(0x077e, 0x21);
XC9080_I2C_WRITE(0x077f, 0x21);
XC9080_I2C_WRITE(0x0780, 0x20);
XC9080_I2C_WRITE(0x0781, 0x20);
XC9080_I2C_WRITE(0x0782, 0x20);
XC9080_I2C_WRITE(0x0783, 0x20);
XC9080_I2C_WRITE(0x0784, 0x21);
XC9080_I2C_WRITE(0x0785, 0x23);
XC9080_I2C_WRITE(0x0786, 0x21);
XC9080_I2C_WRITE(0x0787, 0x20);
XC9080_I2C_WRITE(0x0788, 0x20);
XC9080_I2C_WRITE(0x0789, 0x20);
XC9080_I2C_WRITE(0x078a, 0x20);
XC9080_I2C_WRITE(0x078b, 0x20);
XC9080_I2C_WRITE(0x078c, 0x21);
XC9080_I2C_WRITE(0x078d, 0x24);
XC9080_I2C_WRITE(0x078e, 0x21);
XC9080_I2C_WRITE(0x078f, 0x21);
XC9080_I2C_WRITE(0x0790, 0x21);
XC9080_I2C_WRITE(0x0791, 0x21);
XC9080_I2C_WRITE(0x0792, 0x21);
XC9080_I2C_WRITE(0x0793, 0x21);
XC9080_I2C_WRITE(0x0794, 0x22);
XC9080_I2C_WRITE(0x0795, 0x24);
XC9080_I2C_WRITE(0x0796, 0x22);
XC9080_I2C_WRITE(0x0797, 0x22);
XC9080_I2C_WRITE(0x0798, 0x22);
XC9080_I2C_WRITE(0x0799, 0x22);
XC9080_I2C_WRITE(0x079a, 0x22);
XC9080_I2C_WRITE(0x079b, 0x22);
XC9080_I2C_WRITE(0x079c, 0x23);
XC9080_I2C_WRITE(0x079d, 0x22);
XC9080_I2C_WRITE(0x079e, 0x22);
XC9080_I2C_WRITE(0x079f, 0x24);
XC9080_I2C_WRITE(0x07a0, 0x24);
XC9080_I2C_WRITE(0x07a1, 0x24);
XC9080_I2C_WRITE(0x07a2, 0x24);
XC9080_I2C_WRITE(0x07a3, 0x23);
XC9080_I2C_WRITE(0x07a4, 0x21);
         
XC9080_I2C_WRITE(0x07a5, 0x3d); //c_95,20161101_v2
XC9080_I2C_WRITE(0x07a6, 0x1e);
XC9080_I2C_WRITE(0x07a7, 0x15);
XC9080_I2C_WRITE(0x07a8, 0x11);
XC9080_I2C_WRITE(0x07a9, 0x10);
XC9080_I2C_WRITE(0x07aa, 0x13);
XC9080_I2C_WRITE(0x07ab, 0x1c);
XC9080_I2C_WRITE(0x07ac, 0x33);
XC9080_I2C_WRITE(0x07ad, 0x8 );
XC9080_I2C_WRITE(0x07ae, 0x5 );
XC9080_I2C_WRITE(0x07af, 0x3 );
XC9080_I2C_WRITE(0x07b0, 0x3 );
XC9080_I2C_WRITE(0x07b1, 0x3 );
XC9080_I2C_WRITE(0x07b2, 0x3 );
XC9080_I2C_WRITE(0x07b3, 0x5 );
XC9080_I2C_WRITE(0x07b4, 0x9 );
XC9080_I2C_WRITE(0x07b5, 0x3 );
XC9080_I2C_WRITE(0x07b6, 0x1 );
XC9080_I2C_WRITE(0x07b7, 0x0 );
XC9080_I2C_WRITE(0x07b8, 0x1 );
XC9080_I2C_WRITE(0x07b9, 0x1 );
XC9080_I2C_WRITE(0x07ba, 0x1 );
XC9080_I2C_WRITE(0x07bb, 0x2 );
XC9080_I2C_WRITE(0x07bc, 0x3 );
XC9080_I2C_WRITE(0x07bd, 0x1 );
XC9080_I2C_WRITE(0x07be, 0x0 );
XC9080_I2C_WRITE(0x07bf, 0x0 );
XC9080_I2C_WRITE(0x07c0, 0x0 );
XC9080_I2C_WRITE(0x07c1, 0x0 );
XC9080_I2C_WRITE(0x07c2, 0x0 );
XC9080_I2C_WRITE(0x07c3, 0x1 );
XC9080_I2C_WRITE(0x07c4, 0x1 );
XC9080_I2C_WRITE(0x07c5, 0x1 );
XC9080_I2C_WRITE(0x07c6, 0x0 );
XC9080_I2C_WRITE(0x07c7, 0x0 );
XC9080_I2C_WRITE(0x07c8, 0x0 );
XC9080_I2C_WRITE(0x07c9, 0x0 );
XC9080_I2C_WRITE(0x07ca, 0x0 );
XC9080_I2C_WRITE(0x07cb, 0x1 );
XC9080_I2C_WRITE(0x07cc, 0x2 );
XC9080_I2C_WRITE(0x07cd, 0x3 );
XC9080_I2C_WRITE(0x07ce, 0x1 );
XC9080_I2C_WRITE(0x07cf, 0x1 );
XC9080_I2C_WRITE(0x07d0, 0x1 );
XC9080_I2C_WRITE(0x07d1, 0x1 );
XC9080_I2C_WRITE(0x07d2, 0x1 );
XC9080_I2C_WRITE(0x07d3, 0x2 );
XC9080_I2C_WRITE(0x07d4, 0x3 );
XC9080_I2C_WRITE(0x07d5, 0x9 );
XC9080_I2C_WRITE(0x07d6, 0x6 );
XC9080_I2C_WRITE(0x07d7, 0x3 );
XC9080_I2C_WRITE(0x07d8, 0x3 );
XC9080_I2C_WRITE(0x07d9, 0x3 );
XC9080_I2C_WRITE(0x07da, 0x4 );
XC9080_I2C_WRITE(0x07db, 0x5 );
XC9080_I2C_WRITE(0x07dc, 0xa );
XC9080_I2C_WRITE(0x07dd, 0x3f);
XC9080_I2C_WRITE(0x07de, 0x21);
XC9080_I2C_WRITE(0x07df, 0x16);
XC9080_I2C_WRITE(0x07e0, 0x12);
XC9080_I2C_WRITE(0x07e1, 0x11);
XC9080_I2C_WRITE(0x07e2, 0x14);
XC9080_I2C_WRITE(0x07e3, 0x1f);
XC9080_I2C_WRITE(0x07e4, 0x2e);
XC9080_I2C_WRITE(0x07e5, 0x13);
XC9080_I2C_WRITE(0x07e6, 0x1a);
XC9080_I2C_WRITE(0x07e7, 0x1b);
XC9080_I2C_WRITE(0x07e8, 0x1c);
XC9080_I2C_WRITE(0x07e9, 0x1c);
XC9080_I2C_WRITE(0x07ea, 0x1a);
XC9080_I2C_WRITE(0x07eb, 0x18);
XC9080_I2C_WRITE(0x07ec, 0x17);
XC9080_I2C_WRITE(0x07ed, 0x1c);
XC9080_I2C_WRITE(0x07ee, 0x1e);
XC9080_I2C_WRITE(0x07ef, 0x1f);
XC9080_I2C_WRITE(0x07f0, 0x1f);
XC9080_I2C_WRITE(0x07f1, 0x1f);
XC9080_I2C_WRITE(0x07f2, 0x1f);
XC9080_I2C_WRITE(0x07f3, 0x1f);
XC9080_I2C_WRITE(0x07f4, 0x1d);
XC9080_I2C_WRITE(0x07f5, 0x1d);
XC9080_I2C_WRITE(0x07f6, 0x1f);
XC9080_I2C_WRITE(0x07f7, 0x1f);
XC9080_I2C_WRITE(0x07f8, 0x20);
XC9080_I2C_WRITE(0x07f9, 0x20);
XC9080_I2C_WRITE(0x07fa, 0x20);
XC9080_I2C_WRITE(0x07fb, 0x1f);
XC9080_I2C_WRITE(0x07fc, 0x1f);
XC9080_I2C_WRITE(0x07fd, 0x1e);
XC9080_I2C_WRITE(0x07fe, 0x20);
XC9080_I2C_WRITE(0x07ff, 0x20);
XC9080_I2C_WRITE(0x0800, 0x21);
XC9080_I2C_WRITE(0x0801, 0x20);
XC9080_I2C_WRITE(0x0802, 0x21);
XC9080_I2C_WRITE(0x0803, 0x20);
XC9080_I2C_WRITE(0x0804, 0x20);
XC9080_I2C_WRITE(0x0805, 0x1e);
XC9080_I2C_WRITE(0x0806, 0x20);
XC9080_I2C_WRITE(0x0807, 0x20);
XC9080_I2C_WRITE(0x0808, 0x21);
XC9080_I2C_WRITE(0x0809, 0x21);
XC9080_I2C_WRITE(0x080a, 0x21);
XC9080_I2C_WRITE(0x080b, 0x20);
XC9080_I2C_WRITE(0x080c, 0x20);
XC9080_I2C_WRITE(0x080d, 0x1d);
XC9080_I2C_WRITE(0x080e, 0x1f);
XC9080_I2C_WRITE(0x080f, 0x20);
XC9080_I2C_WRITE(0x0810, 0x20);
XC9080_I2C_WRITE(0x0811, 0x20);
XC9080_I2C_WRITE(0x0812, 0x20);
XC9080_I2C_WRITE(0x0813, 0x20);
XC9080_I2C_WRITE(0x0814, 0x1f);
XC9080_I2C_WRITE(0x0815, 0x1c);
XC9080_I2C_WRITE(0x0816, 0x1e);
XC9080_I2C_WRITE(0x0817, 0x1f);
XC9080_I2C_WRITE(0x0818, 0x1f);
XC9080_I2C_WRITE(0x0819, 0x1f);
XC9080_I2C_WRITE(0x081a, 0x1f);
XC9080_I2C_WRITE(0x081b, 0x1f);
XC9080_I2C_WRITE(0x081c, 0x1d);
XC9080_I2C_WRITE(0x081d, 0x11);
XC9080_I2C_WRITE(0x081e, 0x18);
XC9080_I2C_WRITE(0x081f, 0x1a);
XC9080_I2C_WRITE(0x0820, 0x1b);
XC9080_I2C_WRITE(0x0821, 0x1b);
XC9080_I2C_WRITE(0x0822, 0x1b);
XC9080_I2C_WRITE(0x0823, 0x18);
XC9080_I2C_WRITE(0x0824, 0x19);
XC9080_I2C_WRITE(0x0825, 0x1a);
XC9080_I2C_WRITE(0x0826, 0x1e);
XC9080_I2C_WRITE(0x0827, 0x1f);
XC9080_I2C_WRITE(0x0828, 0x20);
XC9080_I2C_WRITE(0x0829, 0x20);
XC9080_I2C_WRITE(0x082a, 0x20);
XC9080_I2C_WRITE(0x082b, 0x1f);
XC9080_I2C_WRITE(0x082c, 0x1e);
XC9080_I2C_WRITE(0x082d, 0x22);
XC9080_I2C_WRITE(0x082e, 0x21);
XC9080_I2C_WRITE(0x082f, 0x21);
XC9080_I2C_WRITE(0x0830, 0x21);
XC9080_I2C_WRITE(0x0831, 0x21);
XC9080_I2C_WRITE(0x0832, 0x21);
XC9080_I2C_WRITE(0x0833, 0x21);
XC9080_I2C_WRITE(0x0834, 0x21);
XC9080_I2C_WRITE(0x0835, 0x22);
XC9080_I2C_WRITE(0x0836, 0x21);
XC9080_I2C_WRITE(0x0837, 0x21);
XC9080_I2C_WRITE(0x0838, 0x21);
XC9080_I2C_WRITE(0x0839, 0x21);
XC9080_I2C_WRITE(0x083a, 0x21);
XC9080_I2C_WRITE(0x083b, 0x21);
XC9080_I2C_WRITE(0x083c, 0x21);
XC9080_I2C_WRITE(0x083d, 0x22);
XC9080_I2C_WRITE(0x083e, 0x20);
XC9080_I2C_WRITE(0x083f, 0x20);
XC9080_I2C_WRITE(0x0840, 0x20);
XC9080_I2C_WRITE(0x0841, 0x20);
XC9080_I2C_WRITE(0x0842, 0x20);
XC9080_I2C_WRITE(0x0843, 0x20);
XC9080_I2C_WRITE(0x0844, 0x21);
XC9080_I2C_WRITE(0x0845, 0x22);
XC9080_I2C_WRITE(0x0846, 0x20);
XC9080_I2C_WRITE(0x0847, 0x20);
XC9080_I2C_WRITE(0x0848, 0x20);
XC9080_I2C_WRITE(0x0849, 0x20);
XC9080_I2C_WRITE(0x084a, 0x20);
XC9080_I2C_WRITE(0x084b, 0x20);
XC9080_I2C_WRITE(0x084c, 0x21);
XC9080_I2C_WRITE(0x084d, 0x22);
XC9080_I2C_WRITE(0x084e, 0x20);
XC9080_I2C_WRITE(0x084f, 0x20);
XC9080_I2C_WRITE(0x0850, 0x20);
XC9080_I2C_WRITE(0x0851, 0x21);
XC9080_I2C_WRITE(0x0852, 0x20);
XC9080_I2C_WRITE(0x0853, 0x21);
XC9080_I2C_WRITE(0x0854, 0x20);
XC9080_I2C_WRITE(0x0855, 0x21);
XC9080_I2C_WRITE(0x0856, 0x20);
XC9080_I2C_WRITE(0x0857, 0x21);
XC9080_I2C_WRITE(0x0858, 0x21);
XC9080_I2C_WRITE(0x0859, 0x21);
XC9080_I2C_WRITE(0x085a, 0x21);
XC9080_I2C_WRITE(0x085b, 0x21);
XC9080_I2C_WRITE(0x085c, 0x22);
XC9080_I2C_WRITE(0x085d, 0x1c);
XC9080_I2C_WRITE(0x085e, 0x1e);
XC9080_I2C_WRITE(0x085f, 0x20);
XC9080_I2C_WRITE(0x0860, 0x20);
XC9080_I2C_WRITE(0x0861, 0x21);
XC9080_I2C_WRITE(0x0862, 0x20);
XC9080_I2C_WRITE(0x0863, 0x20);
XC9080_I2C_WRITE(0x0864, 0x1a);
         
XC9080_I2C_WRITE(0x0865, 0x3c); //d_95,20161101_v2
XC9080_I2C_WRITE(0x0866, 0x1e);
XC9080_I2C_WRITE(0x0867, 0x15);
XC9080_I2C_WRITE(0x0868, 0x10);
XC9080_I2C_WRITE(0x0869, 0x10);
XC9080_I2C_WRITE(0x086a, 0x13);
XC9080_I2C_WRITE(0x086b, 0x1b);
XC9080_I2C_WRITE(0x086c, 0x32);
XC9080_I2C_WRITE(0x086d, 0x8 );
XC9080_I2C_WRITE(0x086e, 0x5 );
XC9080_I2C_WRITE(0x086f, 0x3 );
XC9080_I2C_WRITE(0x0870, 0x3 );
XC9080_I2C_WRITE(0x0871, 0x3 );
XC9080_I2C_WRITE(0x0872, 0x3 );
XC9080_I2C_WRITE(0x0873, 0x5 );
XC9080_I2C_WRITE(0x0874, 0x9 );
XC9080_I2C_WRITE(0x0875, 0x3 );
XC9080_I2C_WRITE(0x0876, 0x1 );
XC9080_I2C_WRITE(0x0877, 0x0 );
XC9080_I2C_WRITE(0x0878, 0x1 );
XC9080_I2C_WRITE(0x0879, 0x1 );
XC9080_I2C_WRITE(0x087a, 0x1 );
XC9080_I2C_WRITE(0x087b, 0x2 );
XC9080_I2C_WRITE(0x087c, 0x3 );
XC9080_I2C_WRITE(0x087d, 0x1 );
XC9080_I2C_WRITE(0x087e, 0x0 );
XC9080_I2C_WRITE(0x087f, 0x0 );
XC9080_I2C_WRITE(0x0880, 0x0 );
XC9080_I2C_WRITE(0x0881, 0x0 );
XC9080_I2C_WRITE(0x0882, 0x1 );
XC9080_I2C_WRITE(0x0883, 0x1 );
XC9080_I2C_WRITE(0x0884, 0x1 );
XC9080_I2C_WRITE(0x0885, 0x1 );
XC9080_I2C_WRITE(0x0886, 0x0 );
XC9080_I2C_WRITE(0x0887, 0x0 );
XC9080_I2C_WRITE(0x0888, 0x0 );
XC9080_I2C_WRITE(0x0889, 0x0 );
XC9080_I2C_WRITE(0x088a, 0x1 );
XC9080_I2C_WRITE(0x088b, 0x1 );
XC9080_I2C_WRITE(0x088c, 0x2 );
XC9080_I2C_WRITE(0x088d, 0x3 );
XC9080_I2C_WRITE(0x088e, 0x1 );
XC9080_I2C_WRITE(0x088f, 0x1 );
XC9080_I2C_WRITE(0x0890, 0x1 );
XC9080_I2C_WRITE(0x0891, 0x1 );
XC9080_I2C_WRITE(0x0892, 0x1 );
XC9080_I2C_WRITE(0x0893, 0x2 );
XC9080_I2C_WRITE(0x0894, 0x3 );
XC9080_I2C_WRITE(0x0895, 0x9 );
XC9080_I2C_WRITE(0x0896, 0x5 );
XC9080_I2C_WRITE(0x0897, 0x3 );
XC9080_I2C_WRITE(0x0898, 0x3 );
XC9080_I2C_WRITE(0x0899, 0x3 );
XC9080_I2C_WRITE(0x089a, 0x4 );
XC9080_I2C_WRITE(0x089b, 0x5 );
XC9080_I2C_WRITE(0x089c, 0xa );
XC9080_I2C_WRITE(0x089d, 0x3f);
XC9080_I2C_WRITE(0x089e, 0x20);
XC9080_I2C_WRITE(0x089f, 0x16);
XC9080_I2C_WRITE(0x08a0, 0x12);
XC9080_I2C_WRITE(0x08a1, 0x11);
XC9080_I2C_WRITE(0x08a2, 0x14);
XC9080_I2C_WRITE(0x08a3, 0x1f);
XC9080_I2C_WRITE(0x08a4, 0x2e);
XC9080_I2C_WRITE(0x08a5, 0x18);
XC9080_I2C_WRITE(0x08a6, 0x1c);
XC9080_I2C_WRITE(0x08a7, 0x1d);
XC9080_I2C_WRITE(0x08a8, 0x1d);
XC9080_I2C_WRITE(0x08a9, 0x1d);
XC9080_I2C_WRITE(0x08aa, 0x1c);
XC9080_I2C_WRITE(0x08ab, 0x1b);
XC9080_I2C_WRITE(0x08ac, 0x1b);
XC9080_I2C_WRITE(0x08ad, 0x1d);
XC9080_I2C_WRITE(0x08ae, 0x1e);
XC9080_I2C_WRITE(0x08af, 0x1f);
XC9080_I2C_WRITE(0x08b0, 0x1f);
XC9080_I2C_WRITE(0x08b1, 0x1f);
XC9080_I2C_WRITE(0x08b2, 0x1f);
XC9080_I2C_WRITE(0x08b3, 0x1f);
XC9080_I2C_WRITE(0x08b4, 0x1e);
XC9080_I2C_WRITE(0x08b5, 0x1e);
XC9080_I2C_WRITE(0x08b6, 0x1f);
XC9080_I2C_WRITE(0x08b7, 0x1f);
XC9080_I2C_WRITE(0x08b8, 0x20);
XC9080_I2C_WRITE(0x08b9, 0x20);
XC9080_I2C_WRITE(0x08ba, 0x20);
XC9080_I2C_WRITE(0x08bb, 0x1f);
XC9080_I2C_WRITE(0x08bc, 0x1f);
XC9080_I2C_WRITE(0x08bd, 0x1e);
XC9080_I2C_WRITE(0x08be, 0x20);
XC9080_I2C_WRITE(0x08bf, 0x20);
XC9080_I2C_WRITE(0x08c0, 0x20);
XC9080_I2C_WRITE(0x08c1, 0x20);
XC9080_I2C_WRITE(0x08c2, 0x20);
XC9080_I2C_WRITE(0x08c3, 0x20);
XC9080_I2C_WRITE(0x08c4, 0x1f);
XC9080_I2C_WRITE(0x08c5, 0x1f);
XC9080_I2C_WRITE(0x08c6, 0x1f);
XC9080_I2C_WRITE(0x08c7, 0x20);
XC9080_I2C_WRITE(0x08c8, 0x20);
XC9080_I2C_WRITE(0x08c9, 0x20);
XC9080_I2C_WRITE(0x08ca, 0x20);
XC9080_I2C_WRITE(0x08cb, 0x20);
XC9080_I2C_WRITE(0x08cc, 0x20);
XC9080_I2C_WRITE(0x08cd, 0x1e);
XC9080_I2C_WRITE(0x08ce, 0x1f);
XC9080_I2C_WRITE(0x08cf, 0x1f);
XC9080_I2C_WRITE(0x08d0, 0x20);
XC9080_I2C_WRITE(0x08d1, 0x20);
XC9080_I2C_WRITE(0x08d2, 0x1f);
XC9080_I2C_WRITE(0x08d3, 0x1f);
XC9080_I2C_WRITE(0x08d4, 0x1e);
XC9080_I2C_WRITE(0x08d5, 0x1c);
XC9080_I2C_WRITE(0x08d6, 0x1e);
XC9080_I2C_WRITE(0x08d7, 0x1e);
XC9080_I2C_WRITE(0x08d8, 0x1f);
XC9080_I2C_WRITE(0x08d9, 0x1f);
XC9080_I2C_WRITE(0x08da, 0x1f);
XC9080_I2C_WRITE(0x08db, 0x1e);
XC9080_I2C_WRITE(0x08dc, 0x1e);
XC9080_I2C_WRITE(0x08dd, 0x17);
XC9080_I2C_WRITE(0x08de, 0x1b);
XC9080_I2C_WRITE(0x08df, 0x1c);
XC9080_I2C_WRITE(0x08e0, 0x1c);
XC9080_I2C_WRITE(0x08e1, 0x1c);
XC9080_I2C_WRITE(0x08e2, 0x1c);
XC9080_I2C_WRITE(0x08e3, 0x1b);
XC9080_I2C_WRITE(0x08e4, 0x1d);
XC9080_I2C_WRITE(0x08e5, 0x19);
XC9080_I2C_WRITE(0x08e6, 0x1f);
XC9080_I2C_WRITE(0x08e7, 0x1f);
XC9080_I2C_WRITE(0x08e8, 0x21);
XC9080_I2C_WRITE(0x08e9, 0x21);
XC9080_I2C_WRITE(0x08ea, 0x21);
XC9080_I2C_WRITE(0x08eb, 0x1f);
XC9080_I2C_WRITE(0x08ec, 0x1e);
XC9080_I2C_WRITE(0x08ed, 0x22);
XC9080_I2C_WRITE(0x08ee, 0x21);
XC9080_I2C_WRITE(0x08ef, 0x22);
XC9080_I2C_WRITE(0x08f0, 0x22);
XC9080_I2C_WRITE(0x08f1, 0x22);
XC9080_I2C_WRITE(0x08f2, 0x22);
XC9080_I2C_WRITE(0x08f3, 0x22);
XC9080_I2C_WRITE(0x08f4, 0x21);
XC9080_I2C_WRITE(0x08f5, 0x23);
XC9080_I2C_WRITE(0x08f6, 0x21);
XC9080_I2C_WRITE(0x08f7, 0x21);
XC9080_I2C_WRITE(0x08f8, 0x21);
XC9080_I2C_WRITE(0x08f9, 0x21);
XC9080_I2C_WRITE(0x08fa, 0x21);
XC9080_I2C_WRITE(0x08fb, 0x21);
XC9080_I2C_WRITE(0x08fc, 0x22);
XC9080_I2C_WRITE(0x08fd, 0x23);
XC9080_I2C_WRITE(0x08fe, 0x20);
XC9080_I2C_WRITE(0x08ff, 0x21);
XC9080_I2C_WRITE(0x0900, 0x20);
XC9080_I2C_WRITE(0x0901, 0x20);
XC9080_I2C_WRITE(0x0902, 0x20);
XC9080_I2C_WRITE(0x0903, 0x21);
XC9080_I2C_WRITE(0x0904, 0x21);
XC9080_I2C_WRITE(0x0905, 0x22);
XC9080_I2C_WRITE(0x0906, 0x20);
XC9080_I2C_WRITE(0x0907, 0x20);
XC9080_I2C_WRITE(0x0908, 0x20);
XC9080_I2C_WRITE(0x0909, 0x20);
XC9080_I2C_WRITE(0x090a, 0x20);
XC9080_I2C_WRITE(0x090b, 0x20);
XC9080_I2C_WRITE(0x090c, 0x21);
XC9080_I2C_WRITE(0x090d, 0x23);
XC9080_I2C_WRITE(0x090e, 0x21);
XC9080_I2C_WRITE(0x090f, 0x21);
XC9080_I2C_WRITE(0x0910, 0x21);
XC9080_I2C_WRITE(0x0911, 0x21);
XC9080_I2C_WRITE(0x0912, 0x21);
XC9080_I2C_WRITE(0x0913, 0x21);
XC9080_I2C_WRITE(0x0914, 0x20);
XC9080_I2C_WRITE(0x0915, 0x22);
XC9080_I2C_WRITE(0x0916, 0x21);
XC9080_I2C_WRITE(0x0917, 0x21);
XC9080_I2C_WRITE(0x0918, 0x21);
XC9080_I2C_WRITE(0x0919, 0x21);
XC9080_I2C_WRITE(0x091a, 0x21);
XC9080_I2C_WRITE(0x091b, 0x21);
XC9080_I2C_WRITE(0x091c, 0x22);
XC9080_I2C_WRITE(0x091d, 0x1b);
XC9080_I2C_WRITE(0x091e, 0x1e);
XC9080_I2C_WRITE(0x091f, 0x20);
XC9080_I2C_WRITE(0x0920, 0x21);
XC9080_I2C_WRITE(0x0921, 0x22);
XC9080_I2C_WRITE(0x0922, 0x20);
XC9080_I2C_WRITE(0x0923, 0x20);
XC9080_I2C_WRITE(0x0924, 0x1b);
         
//ISP0 AWB
XC9080_I2C_WRITE(0xfffe, 0x14); //0805
XC9080_I2C_WRITE(0x0248, 0x02);  //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
XC9080_I2C_WRITE(0x0282, 0x06);   //int B gain
XC9080_I2C_WRITE(0x0283, 0x00);
XC9080_I2C_WRITE(0x0286, 0x04);   //int Gb gain
XC9080_I2C_WRITE(0x0287, 0x00);
XC9080_I2C_WRITE(0x028a, 0x04);   //int Gr gain
XC9080_I2C_WRITE(0x028b, 0x00);
XC9080_I2C_WRITE(0x028e, 0x04);   //int R gain
XC9080_I2C_WRITE(0x028f, 0x04);
XC9080_I2C_WRITE(0x02b6, 0x06);    //B_temp      
XC9080_I2C_WRITE(0x02b7, 0x00);
XC9080_I2C_WRITE(0x02ba, 0x04);    //G_ temp          
XC9080_I2C_WRITE(0x02bb, 0x00);      
XC9080_I2C_WRITE(0x02be, 0x04);   //R_temp      
XC9080_I2C_WRITE(0x02bf, 0x04);

XC9080_I2C_WRITE(0xfffe, 0x14);
XC9080_I2C_WRITE(0x0248, 0x01);  //XC9080_I2C_WRITE(0x00:AWB_ARITH_ORIGIN21 XC9080_I2C_WRITE(0x01:AWB_ARITH_SW_PRO XC9080_I2C_WRITE(0x02:AWB_ARITH_M
XC9080_I2C_WRITE(0x0249, 0x01);  //AWBFlexiMap_en
XC9080_I2C_WRITE(0x024a, 0x00);  //AWBMove_en
XC9080_I2C_WRITE(0x027a, 0x01);  //nCTBasedMinNum    //20
XC9080_I2C_WRITE(0x027b, 0x00);
XC9080_I2C_WRITE(0x027c, 0x0f);
XC9080_I2C_WRITE(0x027d, 0xff);//nMaxAWBGain

XC9080_I2C_WRITE(0xfffe, 0x30);
XC9080_I2C_WRITE(0x0708, 0x02);
XC9080_I2C_WRITE(0x0709, 0xa0);
XC9080_I2C_WRITE(0x070a, 0x00);
XC9080_I2C_WRITE(0x070b, 0x0c);
XC9080_I2C_WRITE(0x071c, 0x0a); //simple awb
//XC9080_I2C_WRITE(0x0003, 0x11);   //bit[4]:awb_en  bit[0]:awb_gain_en

XC9080_I2C_WRITE(0xfffe, 0x30);
XC9080_I2C_WRITE(0x0730, 0x8e);
XC9080_I2C_WRITE(0x0731, 0xb4);
XC9080_I2C_WRITE(0x0732, 0x40);
XC9080_I2C_WRITE(0x0733, 0x52);
XC9080_I2C_WRITE(0x0734, 0x70);
XC9080_I2C_WRITE(0x0735, 0x98);
XC9080_I2C_WRITE(0x0736, 0x60);
XC9080_I2C_WRITE(0x0737, 0x70);
XC9080_I2C_WRITE(0x0738, 0x4f);
XC9080_I2C_WRITE(0x0739, 0x5d);
XC9080_I2C_WRITE(0x073a, 0x6e);
XC9080_I2C_WRITE(0x073b, 0x80);
XC9080_I2C_WRITE(0x073c, 0x74);
XC9080_I2C_WRITE(0x073d, 0x93);
XC9080_I2C_WRITE(0x073e, 0x50);
XC9080_I2C_WRITE(0x073f, 0x61);
XC9080_I2C_WRITE(0x0740, 0x5c);
XC9080_I2C_WRITE(0x0741, 0x72);
XC9080_I2C_WRITE(0x0742, 0x63);
XC9080_I2C_WRITE(0x0743, 0x85);
XC9080_I2C_WRITE(0x0744, 0x00);
XC9080_I2C_WRITE(0x0745, 0x00);
XC9080_I2C_WRITE(0x0746, 0x00);
XC9080_I2C_WRITE(0x0747, 0x00);
XC9080_I2C_WRITE(0x0748, 0x00);
XC9080_I2C_WRITE(0x0749, 0x00);
XC9080_I2C_WRITE(0x074a, 0x00);
XC9080_I2C_WRITE(0x074b, 0x00);
XC9080_I2C_WRITE(0x074c, 0x00);
XC9080_I2C_WRITE(0x074d, 0x00);
XC9080_I2C_WRITE(0x074e, 0x00);
XC9080_I2C_WRITE(0x074f, 0x00);
XC9080_I2C_WRITE(0x0750, 0x00);
XC9080_I2C_WRITE(0x0751, 0x00);
XC9080_I2C_WRITE(0x0752, 0x00);
XC9080_I2C_WRITE(0x0753, 0x00);
XC9080_I2C_WRITE(0x0754, 0x00);
XC9080_I2C_WRITE(0x0755, 0x00);
XC9080_I2C_WRITE(0x0756, 0x00);
XC9080_I2C_WRITE(0x0757, 0x00);
XC9080_I2C_WRITE(0x0758, 0x00);
XC9080_I2C_WRITE(0x0759, 0x00);
XC9080_I2C_WRITE(0x075a, 0x00);
XC9080_I2C_WRITE(0x075b, 0x00);
XC9080_I2C_WRITE(0x075c, 0x00);
XC9080_I2C_WRITE(0x075d, 0x00);
XC9080_I2C_WRITE(0x075e, 0x00);
XC9080_I2C_WRITE(0x075f, 0x00);
XC9080_I2C_WRITE(0x0760, 0x00);
XC9080_I2C_WRITE(0x0761, 0x00);
XC9080_I2C_WRITE(0x0762, 0x00);
XC9080_I2C_WRITE(0x0763, 0x00);
XC9080_I2C_WRITE(0x0764, 0x00);
XC9080_I2C_WRITE(0x0765, 0x00);
XC9080_I2C_WRITE(0x0766, 0x00);
XC9080_I2C_WRITE(0x0767, 0x00);
XC9080_I2C_WRITE(0x0768, 0x00);
XC9080_I2C_WRITE(0x0769, 0x00);
XC9080_I2C_WRITE(0x076a, 0x00);
XC9080_I2C_WRITE(0x076b, 0x00);
XC9080_I2C_WRITE(0x076c, 0x00);
XC9080_I2C_WRITE(0x076d, 0x00);
XC9080_I2C_WRITE(0x076e, 0x00);
XC9080_I2C_WRITE(0x076f, 0x00);
XC9080_I2C_WRITE(0x0770, 0x22);
XC9080_I2C_WRITE(0x0771, 0x21);
XC9080_I2C_WRITE(0x0772, 0x10);
XC9080_I2C_WRITE(0x0773, 0x00);
XC9080_I2C_WRITE(0x0774, 0x00);
XC9080_I2C_WRITE(0x0775, 0x00);
XC9080_I2C_WRITE(0x0776, 0x00);
XC9080_I2C_WRITE(0x0777, 0x00);

//ISP1 AE
//// 1.  AE statistical //////
//////////////////////////////
XC9080_I2C_WRITE(0xfffe, 0x31);
 // AE_avg

XC9080_I2C_WRITE(0x1f00, 0x00);
XC9080_I2C_WRITE(0x1f01, 0x9f);
XC9080_I2C_WRITE(0x1f02, 0x00);
XC9080_I2C_WRITE(0x1f03, 0x9f);
XC9080_I2C_WRITE(0x1f04, 0x02);
XC9080_I2C_WRITE(0x1f05, 0xf0);
XC9080_I2C_WRITE(0x1f06, 0x02);
XC9080_I2C_WRITE(0x1f07, 0xf0);
XC9080_I2C_WRITE(0x1f08, 0x03);

XC9080_I2C_WRITE(0x0051, 0x01);

//////////// 2.  AE SENSOR     //////
/////////////////////////////////////
XC9080_I2C_WRITE(0xfffe, 0x14);
XC9080_I2C_WRITE(0x000f, 0x01);
XC9080_I2C_WRITE(0x0ac2, 0x48);
XC9080_I2C_WRITE(0x0ac3, 0x01);
XC9080_I2C_WRITE(0x0ac4, 0x0f);
XC9080_I2C_WRITE(0x0ac5, 0x0f);

//exposure
XC9080_I2C_WRITE(0x0ac8, 0x02);
XC9080_I2C_WRITE(0x0ac9, 0x02);
XC9080_I2C_WRITE(0x0aca, 0x02);
XC9080_I2C_WRITE(0x0acb, 0x03);

XC9080_I2C_WRITE(0x0ad0, 0x00);
XC9080_I2C_WRITE(0x0ad1, 0xff);
XC9080_I2C_WRITE(0x0ad2, 0x00);
XC9080_I2C_WRITE(0x0ad3, 0xff);

//gain
XC9080_I2C_WRITE(0x0ae8, 0x02);
XC9080_I2C_WRITE(0x0ae9, 0x05);

XC9080_I2C_WRITE(0x0af0, 0x00);
XC9080_I2C_WRITE(0x0af1, 0xff);

XC9080_I2C_WRITE(0x0af9, 0x00);

XC9080_I2C_WRITE(0x0042, 0x01);
XC9080_I2C_WRITE(0x0043, 0x01);

///////// 3.  AE NORMAL     //////
////////////////////////////////// 
XC9080_I2C_WRITE(0xfffe, 0x14);

XC9080_I2C_WRITE(0x0a00, 0x0);
#ifdef ENABLE_9080_AEC
XC9080_I2C_WRITE(0x003c, 0x1);
#else
XC9080_I2C_WRITE(0x003c, 0x0);
#endif
XC9080_I2C_WRITE(0x0a01, 0x1);

XC9080_I2C_WRITE(0x0aae, 0x01);
XC9080_I2C_WRITE(0x0aaf, 0xf0);

XC9080_I2C_WRITE(0x0ab0, 0x00);
XC9080_I2C_WRITE(0x0ab1, 0x20);

XC9080_I2C_WRITE(0x0a96, 0x42);
XC9080_I2C_WRITE(0x0a97, 0x00);

XC9080_I2C_WRITE(0x0a92, 0x0);
XC9080_I2C_WRITE(0x0a93, 0x10);

XC9080_I2C_WRITE(0x0a54, 0x01);
XC9080_I2C_WRITE(0x0a55, 0xc0);

//flicker
XC9080_I2C_WRITE(0x0ab8, 0x02);
XC9080_I2C_WRITE(0x0ab9, 0x01);
XC9080_I2C_WRITE(0x0abc, 0x14);
XC9080_I2C_WRITE(0x0abd, 0xc0);

//weight
XC9080_I2C_WRITE(0x0a09, 0x04);
XC9080_I2C_WRITE(0x0a0a, 0x04);
XC9080_I2C_WRITE(0x0a0b, 0x04);
XC9080_I2C_WRITE(0x0a0c, 0x04);
XC9080_I2C_WRITE(0x0a0d, 0x04);
 
XC9080_I2C_WRITE(0x0a0e, 0x04);
XC9080_I2C_WRITE(0x0a0f, 0x08);
XC9080_I2C_WRITE(0x0a10, 0x08);
XC9080_I2C_WRITE(0x0a11, 0x08);
XC9080_I2C_WRITE(0x0a12, 0x04);
 
XC9080_I2C_WRITE(0x0a13, 0x04);
XC9080_I2C_WRITE(0x0a14, 0x08);
XC9080_I2C_WRITE(0x0a15, 0x08);
XC9080_I2C_WRITE(0x0a16, 0x08);
XC9080_I2C_WRITE(0x0a17, 0x04);
 
XC9080_I2C_WRITE(0x0a18, 0x04);
XC9080_I2C_WRITE(0x0a19, 0x08);
XC9080_I2C_WRITE(0x0a1a, 0x08);
XC9080_I2C_WRITE(0x0a1b, 0x08);
XC9080_I2C_WRITE(0x0a1c, 0x04);
 
XC9080_I2C_WRITE(0x0a1d, 0x04);
XC9080_I2C_WRITE(0x0a1e, 0x04);
XC9080_I2C_WRITE(0x0a1f, 0x04);
XC9080_I2C_WRITE(0x0a20, 0x04);
XC9080_I2C_WRITE(0x0a21, 0x04); //权重for CurAvg

XC9080_I2C_WRITE(0x0a3c, 0x00);
XC9080_I2C_WRITE(0x0a3d, 0x07);
XC9080_I2C_WRITE(0x0a3e, 0x39);
XC9080_I2C_WRITE(0x0a3f, 0xc0);

XC9080_I2C_WRITE(0x0a04, 0x01); //refresh

//////////  4.AE SPEED       /////
//////////////////////////////////
//XC9080_I2C_WRITE(0x0a7a, 0x1);     // delay frame

XC9080_I2C_WRITE(0x0a7e, 0x00);
XC9080_I2C_WRITE(0x0a7f, 0x80);

XC9080_I2C_WRITE(0x0b70, 0x00);
XC9080_I2C_WRITE(0x0b71, 0xa0);

XC9080_I2C_WRITE(0x0b72, 0x00);
XC9080_I2C_WRITE(0x0b73, 0x60);

XC9080_I2C_WRITE(0x0a80, 0x00);
XC9080_I2C_WRITE(0x0a81, 0x50);

XC9080_I2C_WRITE(0x0a7b, 0x20);
XC9080_I2C_WRITE(0x0a8c, 0x80);

XC9080_I2C_WRITE(0x0a7c, 0x01);
XC9080_I2C_WRITE(0x0bbc, 0x03);

XC9080_I2C_WRITE(0x0a8e, 0x00);
XC9080_I2C_WRITE(0x0a8f, 0xa0);

XC9080_I2C_WRITE(0x0a90, 0x03);
XC9080_I2C_WRITE(0x0a91, 0x00);

//////////  5.AE SMART       ////
/////////////////////////////////
XC9080_I2C_WRITE(0x0a46, 0x01);
XC9080_I2C_WRITE(0x0a47, 0x07);
XC9080_I2C_WRITE(0x0a48, 0x00);

XC9080_I2C_WRITE(0x0a61, 0x02);

////////table reftarget//////////
XC9080_I2C_WRITE(0x0022, 0x1e);

XC9080_I2C_WRITE(0x0b98, 0x00);
XC9080_I2C_WRITE(0x0b99, 0x00);
XC9080_I2C_WRITE(0x0b9a, 0x04);
XC9080_I2C_WRITE(0x0b9b, 0x00);
         
XC9080_I2C_WRITE(0x0b9c, 0x00);
                    
XC9080_I2C_WRITE(0x0b9d, 0x00);

XC9080_I2C_WRITE(0x0b9e, 0x0c);

XC9080_I2C_WRITE(0x0b9f, 0x00);

      
XC9080_I2C_WRITE(0x0ba0, 0x00);
XC9080_I2C_WRITE(0x0ba1, 0x00);
XC9080_I2C_WRITE(0x0ba2, 0x24);
XC9080_I2C_WRITE(0x0ba3, 0x00);

      
XC9080_I2C_WRITE(0x0ba4, 0x00);
XC9080_I2C_WRITE(0x0ba5, 0x00);
XC9080_I2C_WRITE(0x0ba6, 0x6c);

XC9080_I2C_WRITE(0x0ba7, 0x00);
  
                     
XC9080_I2C_WRITE(0x0ba8, 0x00);

XC9080_I2C_WRITE(0x0ba9, 0x01);

XC9080_I2C_WRITE(0x0baa, 0x44);

XC9080_I2C_WRITE(0x0bab, 0x00);

      
XC9080_I2C_WRITE(0x0bac, 0x00);
 
XC9080_I2C_WRITE(0x0bad, 0x04);
XC9080_I2C_WRITE(0x0bae, 0x00);
XC9080_I2C_WRITE(0x0baf, 0x00);

//////////reftarget///////////////
XC9080_I2C_WRITE(0x0a66, 0x02);
XC9080_I2C_WRITE(0x0a67, 0x80);
XC9080_I2C_WRITE(0x0a68, 0x02);
XC9080_I2C_WRITE(0x0a69, 0x60);
XC9080_I2C_WRITE(0x0a6a, 0x02);
XC9080_I2C_WRITE(0x0a6b, 0x40);
XC9080_I2C_WRITE(0x0a6c, 0x01);
XC9080_I2C_WRITE(0x0a6d, 0xf0);
XC9080_I2C_WRITE(0x0a6e, 0x01);
XC9080_I2C_WRITE(0x0a6f, 0xb0);
XC9080_I2C_WRITE(0x0a70, 0x00);
XC9080_I2C_WRITE(0x0a71, 0xf0);

//基于平均亮度维度分
XC9080_I2C_WRITE(0x0b81, 0x28);
XC9080_I2C_WRITE(0x0b7f, 0x00);
XC9080_I2C_WRITE(0x0b80, 0x40);

///////////over exposure offest//////
XC9080_I2C_WRITE(0x0b8a, 0x00);
XC9080_I2C_WRITE(0x0b8b, 0x04);
XC9080_I2C_WRITE(0x0b8c, 0x14);
XC9080_I2C_WRITE(0x0b8d, 0x50);
XC9080_I2C_WRITE(0x0b8e, 0x70);
XC9080_I2C_WRITE(0x0b8f, 0x90);

XC9080_I2C_WRITE(0x0b74, 0x08);
XC9080_I2C_WRITE(0x0b7b, 0x06);
XC9080_I2C_WRITE(0x0b7d, 0x02);
XC9080_I2C_WRITE(0x0b7e, 0x30);
XC9080_I2C_WRITE(0x0b85, 0x40);

//基于方差维度分析模式/Variance  /////////////////
XC9080_I2C_WRITE(0x0b65, 0x30);
XC9080_I2C_WRITE(0x0b66, 0x01);
XC9080_I2C_WRITE(0x0b67, 0x00);
XC9080_I2C_WRITE(0x0b68, 0x06);
XC9080_I2C_WRITE(0x0b69, 0x00);

//基于关注区亮度压制

XC9080_I2C_WRITE(0x0b22, 0x02);
XC9080_I2C_WRITE(0x0b23, 0x80);

XC9080_I2C_WRITE(0x0b87, 0x00);
XC9080_I2C_WRITE(0x0b88, 0x30);

XC9080_I2C_WRITE(0x0b20, 0x00);
XC9080_I2C_WRITE(0x0b21, 0x00);

XC9080_I2C_WRITE(0x0b6e, 0x08);

////////////smart ae end /////////

XC9080_I2C_WRITE(0x1a7e, 0x02); //2frame write
XC9080_I2C_WRITE(0x0bc6, 0x00); //ae chufa menxian

/////////////AE  END/////////////////
////////////////////////////////////

//ISP1 BLC           
XC9080_I2C_WRITE(0xfffe, 0x31);         
XC9080_I2C_WRITE(0x0013, 0x20); //bias   
XC9080_I2C_WRITE(0x0014, 0x00);         
XC9080_I2C_WRITE(0x071b, 0x80); //blc   

//ISP1 LENC           
XC9080_I2C_WRITE(0xfffe, 0x31);
//XC9080_I2C_WRITE(0x0002, 0x80); //bit[7]:lenc_en
XC9080_I2C_WRITE(0x03ca, 0x16);	        
XC9080_I2C_WRITE(0x03cb, 0xC1);
XC9080_I2C_WRITE(0x03cc, 0x16);
XC9080_I2C_WRITE(0x03cd, 0xC1);
XC9080_I2C_WRITE(0x03ce, 0x16);	        
XC9080_I2C_WRITE(0x03cf, 0xC1);
XC9080_I2C_WRITE(0x03d0, 0x0B);	        
XC9080_I2C_WRITE(0x03d1, 0x60);                 
         
XC9080_I2C_WRITE(0xfffe, 0x14);                
XC9080_I2C_WRITE(0x003d, 0x01);
XC9080_I2C_WRITE(0x0041, 0x01);
XC9080_I2C_WRITE(0x0fd4, 0x01);
XC9080_I2C_WRITE(0x0fd5, 0x01);
        
XC9080_I2C_WRITE(0x12dc, 0x00);	//ct  
XC9080_I2C_WRITE(0x12dd, 0x67);
XC9080_I2C_WRITE(0x12de, 0x00);
XC9080_I2C_WRITE(0x12df, 0xb3);
XC9080_I2C_WRITE(0x12e0, 0x01);
XC9080_I2C_WRITE(0x12e1, 0x32);
         
XC9080_I2C_WRITE(0x1099, 0x3c); //A _9,20161101_v2
XC9080_I2C_WRITE(0x109a, 0x1e);
XC9080_I2C_WRITE(0x109b, 0x15);
XC9080_I2C_WRITE(0x109c, 0x11);
XC9080_I2C_WRITE(0x109d, 0x11);
XC9080_I2C_WRITE(0x109e, 0x13);
XC9080_I2C_WRITE(0x109f, 0x1c);
XC9080_I2C_WRITE(0x10a0, 0x32);
XC9080_I2C_WRITE(0x10a1, 0x8 );
XC9080_I2C_WRITE(0x10a2, 0x5 );
XC9080_I2C_WRITE(0x10a3, 0x3 );
XC9080_I2C_WRITE(0x10a4, 0x3 );
XC9080_I2C_WRITE(0x10a5, 0x3 );
XC9080_I2C_WRITE(0x10a6, 0x3 );
XC9080_I2C_WRITE(0x10a7, 0x5 );
XC9080_I2C_WRITE(0x10a8, 0x9 );
XC9080_I2C_WRITE(0x10a9, 0x3 );
XC9080_I2C_WRITE(0x10aa, 0x1 );
XC9080_I2C_WRITE(0x10ab, 0x1 );
XC9080_I2C_WRITE(0x10ac, 0x1 );
XC9080_I2C_WRITE(0x10ad, 0x1 );
XC9080_I2C_WRITE(0x10ae, 0x1 );
XC9080_I2C_WRITE(0x10af, 0x2 );
XC9080_I2C_WRITE(0x10b0, 0x4 );
XC9080_I2C_WRITE(0x10b1, 0x1 );
XC9080_I2C_WRITE(0x10b2, 0x0 );
XC9080_I2C_WRITE(0x10b3, 0x0 );
XC9080_I2C_WRITE(0x10b4, 0x0 );
XC9080_I2C_WRITE(0x10b5, 0x0 );
XC9080_I2C_WRITE(0x10b6, 0x0 );
XC9080_I2C_WRITE(0x10b7, 0x1 );
XC9080_I2C_WRITE(0x10b8, 0x1 );
XC9080_I2C_WRITE(0x10b9, 0x1 );
XC9080_I2C_WRITE(0x10ba, 0x0 );
XC9080_I2C_WRITE(0x10bb, 0x0 );
XC9080_I2C_WRITE(0x10bc, 0x0 );
XC9080_I2C_WRITE(0x10bd, 0x0 );
XC9080_I2C_WRITE(0x10be, 0x0 );
XC9080_I2C_WRITE(0x10bf, 0x1 );
XC9080_I2C_WRITE(0x10c0, 0x2 );
XC9080_I2C_WRITE(0x10c1, 0x3 );
XC9080_I2C_WRITE(0x10c2, 0x1 );
XC9080_I2C_WRITE(0x10c3, 0x1 );
XC9080_I2C_WRITE(0x10c4, 0x1 );
XC9080_I2C_WRITE(0x10c5, 0x1 );
XC9080_I2C_WRITE(0x10c6, 0x1 );
XC9080_I2C_WRITE(0x10c7, 0x2 );
XC9080_I2C_WRITE(0x10c8, 0x3 );
XC9080_I2C_WRITE(0x10c9, 0x9 );
XC9080_I2C_WRITE(0x10ca, 0x6 );
XC9080_I2C_WRITE(0x10cb, 0x3 );
XC9080_I2C_WRITE(0x10cc, 0x3 );
XC9080_I2C_WRITE(0x10cd, 0x3 );
XC9080_I2C_WRITE(0x10ce, 0x4 );
XC9080_I2C_WRITE(0x10cf, 0x5 );
XC9080_I2C_WRITE(0x10d0, 0xb );
XC9080_I2C_WRITE(0x10d1, 0x3f);
XC9080_I2C_WRITE(0x10d2, 0x21);
XC9080_I2C_WRITE(0x10d3, 0x16);
XC9080_I2C_WRITE(0x10d4, 0x12);
XC9080_I2C_WRITE(0x10d5, 0x11);
XC9080_I2C_WRITE(0x10d6, 0x14);
XC9080_I2C_WRITE(0x10d7, 0x20);
XC9080_I2C_WRITE(0x10d8, 0x28);
XC9080_I2C_WRITE(0x10d9, 0xf );
XC9080_I2C_WRITE(0x10da, 0x1a);
XC9080_I2C_WRITE(0x10db, 0x1b);
XC9080_I2C_WRITE(0x10dc, 0x1c);
XC9080_I2C_WRITE(0x10dd, 0x1c);
XC9080_I2C_WRITE(0x10de, 0x1a);
XC9080_I2C_WRITE(0x10df, 0x18);
XC9080_I2C_WRITE(0x10e0, 0x14);
XC9080_I2C_WRITE(0x10e1, 0x1d);
XC9080_I2C_WRITE(0x10e2, 0x1e);
XC9080_I2C_WRITE(0x10e3, 0x1f);
XC9080_I2C_WRITE(0x10e4, 0x1f);
XC9080_I2C_WRITE(0x10e5, 0x1f);
XC9080_I2C_WRITE(0x10e6, 0x20);
XC9080_I2C_WRITE(0x10e7, 0x1f);
XC9080_I2C_WRITE(0x10e8, 0x1e);
XC9080_I2C_WRITE(0x10e9, 0x1d);
XC9080_I2C_WRITE(0x10ea, 0x20);
XC9080_I2C_WRITE(0x10eb, 0x1f);
XC9080_I2C_WRITE(0x10ec, 0x20);
XC9080_I2C_WRITE(0x10ed, 0x20);
XC9080_I2C_WRITE(0x10ee, 0x20);
XC9080_I2C_WRITE(0x10ef, 0x1f);
XC9080_I2C_WRITE(0x10f0, 0x20);
XC9080_I2C_WRITE(0x10f1, 0x1e);
XC9080_I2C_WRITE(0x10f2, 0x20);
XC9080_I2C_WRITE(0x10f3, 0x20);
XC9080_I2C_WRITE(0x10f4, 0x20);
XC9080_I2C_WRITE(0x10f5, 0x20);
XC9080_I2C_WRITE(0x10f6, 0x20);
XC9080_I2C_WRITE(0x10f7, 0x20);
XC9080_I2C_WRITE(0x10f8, 0x21);
XC9080_I2C_WRITE(0x10f9, 0x1e);
XC9080_I2C_WRITE(0x10fa, 0x20);
XC9080_I2C_WRITE(0x10fb, 0x20);
XC9080_I2C_WRITE(0x10fc, 0x20);
XC9080_I2C_WRITE(0x10fd, 0x20);
XC9080_I2C_WRITE(0x10fe, 0x21);
XC9080_I2C_WRITE(0x10ff, 0x21);
XC9080_I2C_WRITE(0x1100, 0x20);
XC9080_I2C_WRITE(0x1101, 0x1d);
XC9080_I2C_WRITE(0x1102, 0x20);
XC9080_I2C_WRITE(0x1103, 0x20);
XC9080_I2C_WRITE(0x1104, 0x20);
XC9080_I2C_WRITE(0x1105, 0x20);
XC9080_I2C_WRITE(0x1106, 0x20);
XC9080_I2C_WRITE(0x1107, 0x20);
XC9080_I2C_WRITE(0x1108, 0x20);
XC9080_I2C_WRITE(0x1109, 0x1c);
XC9080_I2C_WRITE(0x110a, 0x1f);
XC9080_I2C_WRITE(0x110b, 0x1f);
XC9080_I2C_WRITE(0x110c, 0x20);
XC9080_I2C_WRITE(0x110d, 0x20);
XC9080_I2C_WRITE(0x110e, 0x1f);
XC9080_I2C_WRITE(0x110f, 0x20);
XC9080_I2C_WRITE(0x1110, 0x1d);
XC9080_I2C_WRITE(0x1111, 0xe );
XC9080_I2C_WRITE(0x1112, 0x18);
XC9080_I2C_WRITE(0x1113, 0x1a);
XC9080_I2C_WRITE(0x1114, 0x1b);
XC9080_I2C_WRITE(0x1115, 0x1b);
XC9080_I2C_WRITE(0x1116, 0x1b);
XC9080_I2C_WRITE(0x1117, 0x17);
XC9080_I2C_WRITE(0x1118, 0x1b);
XC9080_I2C_WRITE(0x1119, 0x1f);
XC9080_I2C_WRITE(0x111a, 0x22);
XC9080_I2C_WRITE(0x111b, 0x22);
XC9080_I2C_WRITE(0x111c, 0x23);
XC9080_I2C_WRITE(0x111d, 0x24);
XC9080_I2C_WRITE(0x111e, 0x24);
XC9080_I2C_WRITE(0x111f, 0x23);
XC9080_I2C_WRITE(0x1120, 0x22);
XC9080_I2C_WRITE(0x1121, 0x25);
XC9080_I2C_WRITE(0x1122, 0x23);
XC9080_I2C_WRITE(0x1123, 0x23);
XC9080_I2C_WRITE(0x1124, 0x22);
XC9080_I2C_WRITE(0x1125, 0x22);
XC9080_I2C_WRITE(0x1126, 0x22);
XC9080_I2C_WRITE(0x1127, 0x22);
XC9080_I2C_WRITE(0x1128, 0x23);
XC9080_I2C_WRITE(0x1129, 0x24);
XC9080_I2C_WRITE(0x112a, 0x21);
XC9080_I2C_WRITE(0x112b, 0x21);
XC9080_I2C_WRITE(0x112c, 0x21);
XC9080_I2C_WRITE(0x112d, 0x21);
XC9080_I2C_WRITE(0x112e, 0x21);
XC9080_I2C_WRITE(0x112f, 0x21);
XC9080_I2C_WRITE(0x1130, 0x22);
XC9080_I2C_WRITE(0x1131, 0x23);
XC9080_I2C_WRITE(0x1132, 0x21);
XC9080_I2C_WRITE(0x1133, 0x21);
XC9080_I2C_WRITE(0x1134, 0x20);
XC9080_I2C_WRITE(0x1135, 0x20);
XC9080_I2C_WRITE(0x1136, 0x20);
XC9080_I2C_WRITE(0x1137, 0x20);
XC9080_I2C_WRITE(0x1138, 0x21);
XC9080_I2C_WRITE(0x1139, 0x23);
XC9080_I2C_WRITE(0x113a, 0x21);
XC9080_I2C_WRITE(0x113b, 0x20);
XC9080_I2C_WRITE(0x113c, 0x20);
XC9080_I2C_WRITE(0x113d, 0x20);
XC9080_I2C_WRITE(0x113e, 0x20);
XC9080_I2C_WRITE(0x113f, 0x20);
XC9080_I2C_WRITE(0x1140, 0x21);
XC9080_I2C_WRITE(0x1141, 0x24);
XC9080_I2C_WRITE(0x1142, 0x21);
XC9080_I2C_WRITE(0x1143, 0x21);
XC9080_I2C_WRITE(0x1144, 0x21);
XC9080_I2C_WRITE(0x1145, 0x21);
XC9080_I2C_WRITE(0x1146, 0x21);
XC9080_I2C_WRITE(0x1147, 0x21);
XC9080_I2C_WRITE(0x1148, 0x22);
XC9080_I2C_WRITE(0x1149, 0x24);
XC9080_I2C_WRITE(0x114a, 0x22);
XC9080_I2C_WRITE(0x114b, 0x22);
XC9080_I2C_WRITE(0x114c, 0x22);
XC9080_I2C_WRITE(0x114d, 0x22);
XC9080_I2C_WRITE(0x114e, 0x22);
XC9080_I2C_WRITE(0x114f, 0x22);
XC9080_I2C_WRITE(0x1150, 0x23);
XC9080_I2C_WRITE(0x1151, 0x22);
XC9080_I2C_WRITE(0x1152, 0x22);
XC9080_I2C_WRITE(0x1153, 0x24);
XC9080_I2C_WRITE(0x1154, 0x24);
XC9080_I2C_WRITE(0x1155, 0x24);
XC9080_I2C_WRITE(0x1156, 0x24);
XC9080_I2C_WRITE(0x1157, 0x23);
XC9080_I2C_WRITE(0x1158, 0x21);
         
XC9080_I2C_WRITE(0x1159, 0x3d);        //c_95,20161101_v2
XC9080_I2C_WRITE(0x115a, 0x1e);
XC9080_I2C_WRITE(0x115b, 0x15);
XC9080_I2C_WRITE(0x115c, 0x11);
XC9080_I2C_WRITE(0x115d, 0x10);
XC9080_I2C_WRITE(0x115e, 0x13);
XC9080_I2C_WRITE(0x115f, 0x1c);
XC9080_I2C_WRITE(0x1160, 0x33);
XC9080_I2C_WRITE(0x1161, 0x8 );
XC9080_I2C_WRITE(0x1162, 0x5 );
XC9080_I2C_WRITE(0x1163, 0x3 );
XC9080_I2C_WRITE(0x1164, 0x3 );
XC9080_I2C_WRITE(0x1165, 0x3 );
XC9080_I2C_WRITE(0x1166, 0x3 );
XC9080_I2C_WRITE(0x1167, 0x5 );
XC9080_I2C_WRITE(0x1168, 0x9 );
XC9080_I2C_WRITE(0x1169, 0x3 );
XC9080_I2C_WRITE(0x116a, 0x1 );
XC9080_I2C_WRITE(0x116b, 0x0 );
XC9080_I2C_WRITE(0x116c, 0x1 );
XC9080_I2C_WRITE(0x116d, 0x1 );
XC9080_I2C_WRITE(0x116e, 0x1 );
XC9080_I2C_WRITE(0x116f, 0x2 );
XC9080_I2C_WRITE(0x1170, 0x3 );
XC9080_I2C_WRITE(0x1171, 0x1 );
XC9080_I2C_WRITE(0x1172, 0x0 );
XC9080_I2C_WRITE(0x1173, 0x0 );
XC9080_I2C_WRITE(0x1174, 0x0 );
XC9080_I2C_WRITE(0x1175, 0x0 );
XC9080_I2C_WRITE(0x1176, 0x0 );
XC9080_I2C_WRITE(0x1177, 0x1 );
XC9080_I2C_WRITE(0x1178, 0x1 );
XC9080_I2C_WRITE(0x1179, 0x1 );
XC9080_I2C_WRITE(0x117a, 0x0 );
XC9080_I2C_WRITE(0x117b, 0x0 );
XC9080_I2C_WRITE(0x117c, 0x0 );
XC9080_I2C_WRITE(0x117d, 0x0 );
XC9080_I2C_WRITE(0x117e, 0x0 );
XC9080_I2C_WRITE(0x117f, 0x1 );
XC9080_I2C_WRITE(0x1180, 0x2 );
XC9080_I2C_WRITE(0x1181, 0x3 );
XC9080_I2C_WRITE(0x1182, 0x1 );
XC9080_I2C_WRITE(0x1183, 0x1 );
XC9080_I2C_WRITE(0x1184, 0x1 );
XC9080_I2C_WRITE(0x1185, 0x1 );
XC9080_I2C_WRITE(0x1186, 0x1 );
XC9080_I2C_WRITE(0x1187, 0x2 );
XC9080_I2C_WRITE(0x1188, 0x3 );
XC9080_I2C_WRITE(0x1189, 0x9 );
XC9080_I2C_WRITE(0x118a, 0x6 );
XC9080_I2C_WRITE(0x118b, 0x3 );
XC9080_I2C_WRITE(0x118c, 0x3 );
XC9080_I2C_WRITE(0x118d, 0x3 );
XC9080_I2C_WRITE(0x118e, 0x4 );
XC9080_I2C_WRITE(0x118f, 0x5 );
XC9080_I2C_WRITE(0x1190, 0xa );
XC9080_I2C_WRITE(0x1191, 0x3f);
XC9080_I2C_WRITE(0x1192, 0x21);
XC9080_I2C_WRITE(0x1193, 0x16);
XC9080_I2C_WRITE(0x1194, 0x12);
XC9080_I2C_WRITE(0x1195, 0x11);
XC9080_I2C_WRITE(0x1196, 0x14);
XC9080_I2C_WRITE(0x1197, 0x1f);
XC9080_I2C_WRITE(0x1198, 0x2e);
XC9080_I2C_WRITE(0x1199, 0x13);
XC9080_I2C_WRITE(0x119a, 0x1a);
XC9080_I2C_WRITE(0x119b, 0x1b);
XC9080_I2C_WRITE(0x119c, 0x1c);
XC9080_I2C_WRITE(0x119d, 0x1c);
XC9080_I2C_WRITE(0x119e, 0x1a);
XC9080_I2C_WRITE(0x119f, 0x18);
XC9080_I2C_WRITE(0x11a0, 0x17);
XC9080_I2C_WRITE(0x11a1, 0x1c);
XC9080_I2C_WRITE(0x11a2, 0x1e);
XC9080_I2C_WRITE(0x11a3, 0x1f);
XC9080_I2C_WRITE(0x11a4, 0x1f);
XC9080_I2C_WRITE(0x11a5, 0x1f);
XC9080_I2C_WRITE(0x11a6, 0x1f);
XC9080_I2C_WRITE(0x11a7, 0x1f);
XC9080_I2C_WRITE(0x11a8, 0x1d);
XC9080_I2C_WRITE(0x11a9, 0x1d);
XC9080_I2C_WRITE(0x11aa, 0x1f);
XC9080_I2C_WRITE(0x11ab, 0x1f);
XC9080_I2C_WRITE(0x11ac, 0x20);
XC9080_I2C_WRITE(0x11ad, 0x20);
XC9080_I2C_WRITE(0x11ae, 0x20);
XC9080_I2C_WRITE(0x11af, 0x1f);
XC9080_I2C_WRITE(0x11b0, 0x1f);
XC9080_I2C_WRITE(0x11b1, 0x1e);
XC9080_I2C_WRITE(0x11b2, 0x20);
XC9080_I2C_WRITE(0x11b3, 0x20);
XC9080_I2C_WRITE(0x11b4, 0x21);
XC9080_I2C_WRITE(0x11b5, 0x20);
XC9080_I2C_WRITE(0x11b6, 0x21);
XC9080_I2C_WRITE(0x11b7, 0x20);
XC9080_I2C_WRITE(0x11b8, 0x20);
XC9080_I2C_WRITE(0x11b9, 0x1e);
XC9080_I2C_WRITE(0x11ba, 0x20);
XC9080_I2C_WRITE(0x11bb, 0x20);
XC9080_I2C_WRITE(0x11bc, 0x21);
XC9080_I2C_WRITE(0x11bd, 0x21);
XC9080_I2C_WRITE(0x11be, 0x21);
XC9080_I2C_WRITE(0x11bf, 0x20);
XC9080_I2C_WRITE(0x11c0, 0x20);
XC9080_I2C_WRITE(0x11c1, 0x1d);
XC9080_I2C_WRITE(0x11c2, 0x1f);
XC9080_I2C_WRITE(0x11c3, 0x20);
XC9080_I2C_WRITE(0x11c4, 0x20);
XC9080_I2C_WRITE(0x11c5, 0x20);
XC9080_I2C_WRITE(0x11c6, 0x20);
XC9080_I2C_WRITE(0x11c7, 0x20);
XC9080_I2C_WRITE(0x11c8, 0x1f);
XC9080_I2C_WRITE(0x11c9, 0x1c);
XC9080_I2C_WRITE(0x11ca, 0x1e);
XC9080_I2C_WRITE(0x11cb, 0x1f);
XC9080_I2C_WRITE(0x11cc, 0x1f);
XC9080_I2C_WRITE(0x11cd, 0x1f);
XC9080_I2C_WRITE(0x11ce, 0x1f);
XC9080_I2C_WRITE(0x11cf, 0x1f);
XC9080_I2C_WRITE(0x11d0, 0x1d);
XC9080_I2C_WRITE(0x11d1, 0x11);
XC9080_I2C_WRITE(0x11d2, 0x18);
XC9080_I2C_WRITE(0x11d3, 0x1a);
XC9080_I2C_WRITE(0x11d4, 0x1b);
XC9080_I2C_WRITE(0x11d5, 0x1b);
XC9080_I2C_WRITE(0x11d6, 0x1b);
XC9080_I2C_WRITE(0x11d7, 0x18);
XC9080_I2C_WRITE(0x11d8, 0x19);
XC9080_I2C_WRITE(0x11d9, 0x1a);
XC9080_I2C_WRITE(0x11da, 0x1e);
XC9080_I2C_WRITE(0x11db, 0x1f);
XC9080_I2C_WRITE(0x11dc, 0x20);
XC9080_I2C_WRITE(0x11dd, 0x20);
XC9080_I2C_WRITE(0x11de, 0x20);
XC9080_I2C_WRITE(0x11df, 0x1f);
XC9080_I2C_WRITE(0x11e0, 0x1e);
XC9080_I2C_WRITE(0x11e1, 0x22);
XC9080_I2C_WRITE(0x11e2, 0x21);
XC9080_I2C_WRITE(0x11e3, 0x21);
XC9080_I2C_WRITE(0x11e4, 0x21);
XC9080_I2C_WRITE(0x11e5, 0x21);
XC9080_I2C_WRITE(0x11e6, 0x21);
XC9080_I2C_WRITE(0x11e7, 0x21);
XC9080_I2C_WRITE(0x11e8, 0x21);
XC9080_I2C_WRITE(0x11e9, 0x22);
XC9080_I2C_WRITE(0x11ea, 0x21);
XC9080_I2C_WRITE(0x11eb, 0x21);
XC9080_I2C_WRITE(0x11ec, 0x21);
XC9080_I2C_WRITE(0x11ed, 0x21);
XC9080_I2C_WRITE(0x11ee, 0x21);
XC9080_I2C_WRITE(0x11ef, 0x21);
XC9080_I2C_WRITE(0x11f0, 0x21);
XC9080_I2C_WRITE(0x11f1, 0x22);
XC9080_I2C_WRITE(0x11f2, 0x20);
XC9080_I2C_WRITE(0x11f3, 0x20);
XC9080_I2C_WRITE(0x11f4, 0x20);
XC9080_I2C_WRITE(0x11f5, 0x20);
XC9080_I2C_WRITE(0x11f6, 0x20);
XC9080_I2C_WRITE(0x11f7, 0x20);
XC9080_I2C_WRITE(0x11f8, 0x21);
XC9080_I2C_WRITE(0x11f9, 0x22);
XC9080_I2C_WRITE(0x11fa, 0x20);
XC9080_I2C_WRITE(0x11fb, 0x20);
XC9080_I2C_WRITE(0x11fc, 0x20);
XC9080_I2C_WRITE(0x11fd, 0x20);
XC9080_I2C_WRITE(0x11fe, 0x20);
XC9080_I2C_WRITE(0x11ff, 0x20);
XC9080_I2C_WRITE(0x1200, 0x21);
XC9080_I2C_WRITE(0x1201, 0x22);
XC9080_I2C_WRITE(0x1202, 0x20);
XC9080_I2C_WRITE(0x1203, 0x20);
XC9080_I2C_WRITE(0x1204, 0x20);
XC9080_I2C_WRITE(0x1205, 0x21);
XC9080_I2C_WRITE(0x1206, 0x20);
XC9080_I2C_WRITE(0x1207, 0x21);
XC9080_I2C_WRITE(0x1208, 0x20);
XC9080_I2C_WRITE(0x1209, 0x21);
XC9080_I2C_WRITE(0x120a, 0x20);
XC9080_I2C_WRITE(0x120b, 0x21);
XC9080_I2C_WRITE(0x120c, 0x21);
XC9080_I2C_WRITE(0x120d, 0x21);
XC9080_I2C_WRITE(0x120e, 0x21);
XC9080_I2C_WRITE(0x120f, 0x21);
XC9080_I2C_WRITE(0x1210, 0x22);
XC9080_I2C_WRITE(0x1211, 0x1c);
XC9080_I2C_WRITE(0x1212, 0x1e);
XC9080_I2C_WRITE(0x1213, 0x20);
XC9080_I2C_WRITE(0x1214, 0x20);
XC9080_I2C_WRITE(0x1215, 0x21);
XC9080_I2C_WRITE(0x1216, 0x20);
XC9080_I2C_WRITE(0x1217, 0x20);
XC9080_I2C_WRITE(0x1218, 0x1a);
         
XC9080_I2C_WRITE(0x1219, 0x3c);
  //d_95,20161101_v2
XC9080_I2C_WRITE(0x121a, 0x1e);
XC9080_I2C_WRITE(0x121b, 0x15);
XC9080_I2C_WRITE(0x121c, 0x10);
XC9080_I2C_WRITE(0x121d, 0x10);
XC9080_I2C_WRITE(0x121e, 0x13);
XC9080_I2C_WRITE(0x121f, 0x1b);
XC9080_I2C_WRITE(0x1220, 0x32);
XC9080_I2C_WRITE(0x1221, 0x8 );
XC9080_I2C_WRITE(0x1222, 0x5 );
XC9080_I2C_WRITE(0x1223, 0x3 );
XC9080_I2C_WRITE(0x1224, 0x3 );
XC9080_I2C_WRITE(0x1225, 0x3 );
XC9080_I2C_WRITE(0x1226, 0x3 );
XC9080_I2C_WRITE(0x1227, 0x5 );
XC9080_I2C_WRITE(0x1228, 0x9 );
XC9080_I2C_WRITE(0x1229, 0x3 );
XC9080_I2C_WRITE(0x122a, 0x1 );
XC9080_I2C_WRITE(0x122b, 0x0 );
XC9080_I2C_WRITE(0x122c, 0x1 );
XC9080_I2C_WRITE(0x122d, 0x1 );
XC9080_I2C_WRITE(0x122e, 0x1 );
XC9080_I2C_WRITE(0x122f, 0x2 );
XC9080_I2C_WRITE(0x1230, 0x3 );
XC9080_I2C_WRITE(0x1231, 0x1 );
XC9080_I2C_WRITE(0x1232, 0x0 );
XC9080_I2C_WRITE(0x1233, 0x0 );
XC9080_I2C_WRITE(0x1234, 0x0 );
XC9080_I2C_WRITE(0x1235, 0x0 );
XC9080_I2C_WRITE(0x1236, 0x1 );
XC9080_I2C_WRITE(0x1237, 0x1 );
XC9080_I2C_WRITE(0x1238, 0x1 );
XC9080_I2C_WRITE(0x1239, 0x1 );
XC9080_I2C_WRITE(0x123a, 0x0 );
XC9080_I2C_WRITE(0x123b, 0x0 );
XC9080_I2C_WRITE(0x123c, 0x0 );
XC9080_I2C_WRITE(0x123d, 0x0 );
XC9080_I2C_WRITE(0x123e, 0x1 );
XC9080_I2C_WRITE(0x123f, 0x1 );
XC9080_I2C_WRITE(0x1240, 0x2 );
XC9080_I2C_WRITE(0x1241, 0x3 );
XC9080_I2C_WRITE(0x1242, 0x1 );
XC9080_I2C_WRITE(0x1243, 0x1 );
XC9080_I2C_WRITE(0x1244, 0x1 );
XC9080_I2C_WRITE(0x1245, 0x1 );
XC9080_I2C_WRITE(0x1246, 0x1 );
XC9080_I2C_WRITE(0x1247, 0x2 );
XC9080_I2C_WRITE(0x1248, 0x3 );
XC9080_I2C_WRITE(0x1249, 0x9 );
XC9080_I2C_WRITE(0x124a, 0x5 );
XC9080_I2C_WRITE(0x124b, 0x3 );
XC9080_I2C_WRITE(0x124c, 0x3 );
XC9080_I2C_WRITE(0x124d, 0x3 );
XC9080_I2C_WRITE(0x124e, 0x4 );
XC9080_I2C_WRITE(0x124f, 0x5 );
XC9080_I2C_WRITE(0x1250, 0xa );
XC9080_I2C_WRITE(0x1251, 0x3f);
XC9080_I2C_WRITE(0x1252, 0x20);
XC9080_I2C_WRITE(0x1253, 0x16);
XC9080_I2C_WRITE(0x1254, 0x12);
XC9080_I2C_WRITE(0x1255, 0x11);
XC9080_I2C_WRITE(0x1256, 0x14);
XC9080_I2C_WRITE(0x1257, 0x1f);
XC9080_I2C_WRITE(0x1258, 0x2e);
XC9080_I2C_WRITE(0x1259, 0x18);
XC9080_I2C_WRITE(0x125a, 0x1c);
XC9080_I2C_WRITE(0x125b, 0x1d);
XC9080_I2C_WRITE(0x125c, 0x1d);
XC9080_I2C_WRITE(0x125d, 0x1d);
XC9080_I2C_WRITE(0x125e, 0x1c);
XC9080_I2C_WRITE(0x125f, 0x1b);
XC9080_I2C_WRITE(0x1260, 0x1b);
XC9080_I2C_WRITE(0x1261, 0x1d);
XC9080_I2C_WRITE(0x1262, 0x1e);
XC9080_I2C_WRITE(0x1263, 0x1f);
XC9080_I2C_WRITE(0x1264, 0x1f);
XC9080_I2C_WRITE(0x1265, 0x1f);
XC9080_I2C_WRITE(0x1266, 0x1f);
XC9080_I2C_WRITE(0x1267, 0x1f);
XC9080_I2C_WRITE(0x1268, 0x1e);
XC9080_I2C_WRITE(0x1269, 0x1e);
XC9080_I2C_WRITE(0x126a, 0x1f);
XC9080_I2C_WRITE(0x126b, 0x1f);
XC9080_I2C_WRITE(0x126c, 0x20);
XC9080_I2C_WRITE(0x126d, 0x20);
XC9080_I2C_WRITE(0x126e, 0x20);
XC9080_I2C_WRITE(0x126f, 0x1f);
XC9080_I2C_WRITE(0x1270, 0x1f);
XC9080_I2C_WRITE(0x1271, 0x1e);
XC9080_I2C_WRITE(0x1272, 0x20);
XC9080_I2C_WRITE(0x1273, 0x20);
XC9080_I2C_WRITE(0x1274, 0x20);
XC9080_I2C_WRITE(0x1275, 0x20);
XC9080_I2C_WRITE(0x1276, 0x20);
XC9080_I2C_WRITE(0x1277, 0x20);
XC9080_I2C_WRITE(0x1278, 0x1f);
XC9080_I2C_WRITE(0x1279, 0x1f);
XC9080_I2C_WRITE(0x127a, 0x1f);
XC9080_I2C_WRITE(0x127b, 0x20);
XC9080_I2C_WRITE(0x127c, 0x20);
XC9080_I2C_WRITE(0x127d, 0x20);
XC9080_I2C_WRITE(0x127e, 0x20);
XC9080_I2C_WRITE(0x127f, 0x20);
XC9080_I2C_WRITE(0x1280, 0x20);
XC9080_I2C_WRITE(0x1281, 0x1e);
XC9080_I2C_WRITE(0x1282, 0x1f);
XC9080_I2C_WRITE(0x1283, 0x1f);
XC9080_I2C_WRITE(0x1284, 0x20);
XC9080_I2C_WRITE(0x1285, 0x20);
XC9080_I2C_WRITE(0x1286, 0x1f);
XC9080_I2C_WRITE(0x1287, 0x1f);
XC9080_I2C_WRITE(0x1288, 0x1e);
XC9080_I2C_WRITE(0x1289, 0x1c);
XC9080_I2C_WRITE(0x128a, 0x1e);
XC9080_I2C_WRITE(0x128b, 0x1e);
XC9080_I2C_WRITE(0x128c, 0x1f);
XC9080_I2C_WRITE(0x128d, 0x1f);
XC9080_I2C_WRITE(0x128e, 0x1f);
XC9080_I2C_WRITE(0x128f, 0x1e);
XC9080_I2C_WRITE(0x1290, 0x1e);
XC9080_I2C_WRITE(0x1291, 0x17);
XC9080_I2C_WRITE(0x1292, 0x1b);
XC9080_I2C_WRITE(0x1293, 0x1c);
XC9080_I2C_WRITE(0x1294, 0x1c);
XC9080_I2C_WRITE(0x1295, 0x1c);
XC9080_I2C_WRITE(0x1296, 0x1c);
XC9080_I2C_WRITE(0x1297, 0x1b);
XC9080_I2C_WRITE(0x1298, 0x1d);
XC9080_I2C_WRITE(0x1299, 0x19);
XC9080_I2C_WRITE(0x129a, 0x1f);
XC9080_I2C_WRITE(0x129b, 0x1f);
XC9080_I2C_WRITE(0x129c, 0x21);
XC9080_I2C_WRITE(0x129d, 0x21);
XC9080_I2C_WRITE(0x129e, 0x21);
XC9080_I2C_WRITE(0x129f, 0x1f);
XC9080_I2C_WRITE(0x12a0, 0x1e);
XC9080_I2C_WRITE(0x12a1, 0x22);
XC9080_I2C_WRITE(0x12a2, 0x21);
XC9080_I2C_WRITE(0x12a3, 0x22);
XC9080_I2C_WRITE(0x12a4, 0x22);
XC9080_I2C_WRITE(0x12a5, 0x22);
XC9080_I2C_WRITE(0x12a6, 0x22);
XC9080_I2C_WRITE(0x12a7, 0x22);
XC9080_I2C_WRITE(0x12a8, 0x21);
XC9080_I2C_WRITE(0x12a9, 0x23);
XC9080_I2C_WRITE(0x12aa, 0x21);
XC9080_I2C_WRITE(0x12ab, 0x21);
XC9080_I2C_WRITE(0x12ac, 0x21);
XC9080_I2C_WRITE(0x12ad, 0x21);
XC9080_I2C_WRITE(0x12ae, 0x21);
XC9080_I2C_WRITE(0x12af, 0x21);
XC9080_I2C_WRITE(0x12b0, 0x22);
XC9080_I2C_WRITE(0x12b1, 0x23);
XC9080_I2C_WRITE(0x12b2, 0x20);
XC9080_I2C_WRITE(0x12b3, 0x21);
XC9080_I2C_WRITE(0x12b4, 0x20);
XC9080_I2C_WRITE(0x12b5, 0x20);
XC9080_I2C_WRITE(0x12b6, 0x20);
XC9080_I2C_WRITE(0x12b7, 0x21);
XC9080_I2C_WRITE(0x12b8, 0x21);
XC9080_I2C_WRITE(0x12b9, 0x22);
XC9080_I2C_WRITE(0x12ba, 0x20);
XC9080_I2C_WRITE(0x12bb, 0x20);
XC9080_I2C_WRITE(0x12bc, 0x20);
XC9080_I2C_WRITE(0x12bd, 0x20);
XC9080_I2C_WRITE(0x12be, 0x20);
XC9080_I2C_WRITE(0x12bf, 0x20);
XC9080_I2C_WRITE(0x12c0, 0x21);
XC9080_I2C_WRITE(0x12c1, 0x23);
XC9080_I2C_WRITE(0x12c2, 0x21);
XC9080_I2C_WRITE(0x12c3, 0x21);
XC9080_I2C_WRITE(0x12c4, 0x21);
XC9080_I2C_WRITE(0x12c5, 0x21);
XC9080_I2C_WRITE(0x12c6, 0x21);
XC9080_I2C_WRITE(0x12c7, 0x21);
XC9080_I2C_WRITE(0x12c8, 0x20);
XC9080_I2C_WRITE(0x12c9, 0x22);
XC9080_I2C_WRITE(0x12ca, 0x21);
XC9080_I2C_WRITE(0x12cb, 0x21);
XC9080_I2C_WRITE(0x12cc, 0x21);
XC9080_I2C_WRITE(0x12cd, 0x21);
XC9080_I2C_WRITE(0x12ce, 0x21);
XC9080_I2C_WRITE(0x12cf, 0x21);
XC9080_I2C_WRITE(0x12d0, 0x22);
XC9080_I2C_WRITE(0x12d1, 0x1b);
XC9080_I2C_WRITE(0x12d2, 0x1e);
XC9080_I2C_WRITE(0x12d3, 0x20);
XC9080_I2C_WRITE(0x12d4, 0x21);
XC9080_I2C_WRITE(0x12d5, 0x22);
XC9080_I2C_WRITE(0x12d6, 0x20);
XC9080_I2C_WRITE(0x12d7, 0x20);
XC9080_I2C_WRITE(0x12d8, 0x1b);
        
//ISP1 AWB
XC9080_I2C_WRITE(0xfffe, 0x14); //0805
XC9080_I2C_WRITE(0x0bfc, 0x02); //0x00:AWB_ARITH_ORIGIN21 0x01:AWB_ARITH_SW_PRO 0x02:AWB_ARITH_M
XC9080_I2C_WRITE(0x0c36, 0x06); //int B gain
XC9080_I2C_WRITE(0x0c37, 0x00);
XC9080_I2C_WRITE(0x0c3a, 0x04); //int Gb gain
XC9080_I2C_WRITE(0x0c3b, 0x00);
XC9080_I2C_WRITE(0x0c3e, 0x04); //int Gr gain
XC9080_I2C_WRITE(0x0c3f, 0x00);
XC9080_I2C_WRITE(0x0c42, 0x04); //int R gain
XC9080_I2C_WRITE(0x0c43, 0x04);
XC9080_I2C_WRITE(0x0c6a, 0x06); //B_temp      
XC9080_I2C_WRITE(0x0c6b, 0x00);
XC9080_I2C_WRITE(0x0c6e, 0x04); //G_ temp          
XC9080_I2C_WRITE(0x0c6f, 0x00);      
XC9080_I2C_WRITE(0x0c72, 0x04); //R_temp      
XC9080_I2C_WRITE(0x0c73, 0x04);

XC9080_I2C_WRITE(0xfffe, 0x14);
XC9080_I2C_WRITE(0x0bfc, 0x01); //0x00:AWB_ARITH_ORIGIN21 0x01:AWB_ARITH_SW_PRO 0x02:AWB_ARITH_M
XC9080_I2C_WRITE(0x0bfd, 0x01); //AWBFlexiMap_en
XC9080_I2C_WRITE(0x0bfe, 0x00); //AWBMove_en
XC9080_I2C_WRITE(0x0c2e, 0x01); //nCTBasedMinNum    //20
XC9080_I2C_WRITE(0x0c2f, 0x00);
XC9080_I2C_WRITE(0x0c30, 0x0f);
XC9080_I2C_WRITE(0x0c31, 0xff); //nMaxAWBGain

XC9080_I2C_WRITE(0xfffe, 0x31);
XC9080_I2C_WRITE(0x0708, 0x02);
XC9080_I2C_WRITE(0x0709, 0xa0);
XC9080_I2C_WRITE(0x070a, 0x00);
XC9080_I2C_WRITE(0x070b, 0x0c);
XC9080_I2C_WRITE(0x071c, 0x0a); //simple awb
//XC9080_I2C_WRITE(0x0003, 0x11);
 //bit[4]:awb_en  bit[0]:awb_gain_en

XC9080_I2C_WRITE(0xfffe, 0x31);
XC9080_I2C_WRITE(0x0730, 0x8e);
XC9080_I2C_WRITE(0x0731, 0xb4);
XC9080_I2C_WRITE(0x0732, 0x40);
XC9080_I2C_WRITE(0x0733, 0x52);
XC9080_I2C_WRITE(0x0734, 0x70);
XC9080_I2C_WRITE(0x0735, 0x98);
XC9080_I2C_WRITE(0x0736, 0x60);
XC9080_I2C_WRITE(0x0737, 0x70);
XC9080_I2C_WRITE(0x0738, 0x4f);
XC9080_I2C_WRITE(0x0739, 0x5d);
XC9080_I2C_WRITE(0x073a, 0x6e);
XC9080_I2C_WRITE(0x073b, 0x80);
XC9080_I2C_WRITE(0x073c, 0x74);
XC9080_I2C_WRITE(0x073d, 0x93);
XC9080_I2C_WRITE(0x073e, 0x50);
XC9080_I2C_WRITE(0x073f, 0x61);
XC9080_I2C_WRITE(0x0740, 0x5c);
XC9080_I2C_WRITE(0x0741, 0x72);
XC9080_I2C_WRITE(0x0742, 0x63);
XC9080_I2C_WRITE(0x0743, 0x85);
XC9080_I2C_WRITE(0x0744, 0x00);
XC9080_I2C_WRITE(0x0745, 0x00);
XC9080_I2C_WRITE(0x0746, 0x00);
XC9080_I2C_WRITE(0x0747, 0x00);
XC9080_I2C_WRITE(0x0748, 0x00);
XC9080_I2C_WRITE(0x0749, 0x00);
XC9080_I2C_WRITE(0x074a, 0x00);
XC9080_I2C_WRITE(0x074b, 0x00);
XC9080_I2C_WRITE(0x074c, 0x00);
XC9080_I2C_WRITE(0x074d, 0x00);
XC9080_I2C_WRITE(0x074e, 0x00);
XC9080_I2C_WRITE(0x074f, 0x00);
XC9080_I2C_WRITE(0x0750, 0x00);
XC9080_I2C_WRITE(0x0751, 0x00);
XC9080_I2C_WRITE(0x0752, 0x00);
XC9080_I2C_WRITE(0x0753, 0x00);
XC9080_I2C_WRITE(0x0754, 0x00);
XC9080_I2C_WRITE(0x0755, 0x00);
XC9080_I2C_WRITE(0x0756, 0x00);
XC9080_I2C_WRITE(0x0757, 0x00);
XC9080_I2C_WRITE(0x0758, 0x00);
XC9080_I2C_WRITE(0x0759, 0x00);
XC9080_I2C_WRITE(0x075a, 0x00);
XC9080_I2C_WRITE(0x075b, 0x00);
XC9080_I2C_WRITE(0x075c, 0x00);
XC9080_I2C_WRITE(0x075d, 0x00);
XC9080_I2C_WRITE(0x075e, 0x00);
XC9080_I2C_WRITE(0x075f, 0x00);
XC9080_I2C_WRITE(0x0760, 0x00);
XC9080_I2C_WRITE(0x0761, 0x00);
XC9080_I2C_WRITE(0x0762, 0x00);
XC9080_I2C_WRITE(0x0763, 0x00);
XC9080_I2C_WRITE(0x0764, 0x00);
XC9080_I2C_WRITE(0x0765, 0x00);
XC9080_I2C_WRITE(0x0766, 0x00);
XC9080_I2C_WRITE(0x0767, 0x00);
XC9080_I2C_WRITE(0x0768, 0x00);
XC9080_I2C_WRITE(0x0769, 0x00);
XC9080_I2C_WRITE(0x076a, 0x00);
XC9080_I2C_WRITE(0x076b, 0x00);
XC9080_I2C_WRITE(0x076c, 0x00);
XC9080_I2C_WRITE(0x076d, 0x00);
XC9080_I2C_WRITE(0x076e, 0x00);
XC9080_I2C_WRITE(0x076f, 0x00);
XC9080_I2C_WRITE(0x0770, 0x22);
XC9080_I2C_WRITE(0x0771, 0x21);
XC9080_I2C_WRITE(0x0772, 0x10);
XC9080_I2C_WRITE(0x0773, 0x00);
XC9080_I2C_WRITE(0x0774, 0x00);
XC9080_I2C_WRITE(0x0775, 0x00);
XC9080_I2C_WRITE(0x0776, 0x00);
XC9080_I2C_WRITE(0x0777, 0x00);
   
                                            
//TOP                                   
//isp0 top
XC9080_I2C_WRITE(0xfffe, 0x14);
#ifdef ENABLE_9080_AEC
XC9080_I2C_WRITE(0x002b, 0x01); //isp0 AE_en
#else
XC9080_I2C_WRITE(0x002b, 0x00); //isp0 AE_en
#endif
XC9080_I2C_WRITE(0x002c, 0x01); //isp0 awb_en
XC9080_I2C_WRITE(0x0030, 0x01); //isp0 lenc_en
XC9080_I2C_WRITE(0x0620, 0x01); //lenc mode
XC9080_I2C_WRITE(0x0621, 0x01);

XC9080_I2C_WRITE(0xfffe, 0x30);
XC9080_I2C_WRITE(0x0000, 0x06);
XC9080_I2C_WRITE(0x0001, 0x00);                                   
XC9080_I2C_WRITE(0x0002, 0x80);
#ifdef ENABLE_9080_AWB
XC9080_I2C_WRITE(0x0003, 0x31);
#else
XC9080_I2C_WRITE(0x0003, 0x20);
#endif
XC9080_I2C_WRITE(0x0004, 0x00);

XC9080_I2C_WRITE(0x0019, 0x0b);
#ifdef ENABLE_9080_AWB
XC9080_I2C_WRITE(0x0051, 0x01);
#else
XC9080_I2C_WRITE(0x0051, 0x02);
#endif
//isp1 top
XC9080_I2C_WRITE(0xfffe, 0x14);
#ifdef ENABLE_9080_AEC
XC9080_I2C_WRITE(0x003c, 0x01); //isp1 AE_en
#else
XC9080_I2C_WRITE(0x003c, 0x00); //isp1 AE_en
#endif
XC9080_I2C_WRITE(0x003d, 0x01); //isp1 awb_en
XC9080_I2C_WRITE(0x0041, 0x01); //isp1 lenc_en
XC9080_I2C_WRITE(0x0fd4, 0x01); //lenc mode   
XC9080_I2C_WRITE(0x0fd5, 0x01);

XC9080_I2C_WRITE(0xfffe, 0x31);
XC9080_I2C_WRITE(0x0000, 0x06); 
XC9080_I2C_WRITE(0x0001, 0x00); 
XC9080_I2C_WRITE(0x0002, 0x80);
#ifdef ENABLE_9080_AWB 
XC9080_I2C_WRITE(0x0003, 0x31);
#else
XC9080_I2C_WRITE(0x0003, 0x20);
#endif
XC9080_I2C_WRITE(0x0004, 0x00);

XC9080_I2C_WRITE(0x0019, 0x0b);
#ifdef ENABLE_9080_AWB
XC9080_I2C_WRITE(0x0051, 0x01);
#else
XC9080_I2C_WRITE(0x0051, 0x02);
#endif
#endif
}
//probe is core work of the i2c driver.
static int isp_xc9080_probe(struct i2c_client *client, const struct i2c_device_id *dev_id)
{
	int ret = 0;
	struct chip_resource *res;
	u8 buf;
	int i;
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
		return ret;
	}
	if(mipi) {
		csi_core_init(lanes, freq);
		csi_core_open();
	}
	ret = isp_xc9080_set_voltage(&isp_xc9080_info);
	if(ret) {
		pr_err("in isp xc9080 probe sensor voltage error\n");
		goto err1;
	}
	//set power on and set clock
	ret = isp_xc9080_power_up(&isp_xc9080_info, 1);
	if(ret)
	{
		pr_err("in isp xc9080 probe sensor power up error\n");
		goto err1;
	}
	//set clock
	ret = isp_xc9080_set_clock(&isp_xc9080_info);
	if (ret) {
		pr_err("in isp xc9080 probe sensor set clock error\n");
		goto err2;
	}
#ifdef CONFIG_ARCH_Q3F
	if (wraper) {
		 isp_dvp_wrapper_probe();
		 isp_dvp_wrapper_init(0x02, wpwidth, wpheight, hdly, vdly);
	}
#endif
#ifdef CONFIG_VIDEO_STK3855
	class_register(&isp_xc9080_class);
    enable_status = 1;
#endif
    ret = xc9080_read_id(res);
    if(ret) {
    	goto err3;
    }
    xc9080_init_sequence(res);
    //xc9080_bypass(res, XC9080_BYPASS_ON);
	//xc9080_bypass(res, XC9080_BYPASS_OFF);
    g_res = res;
	return 0;
err4:
	xc9080_bypass(res, XC9080_BYPASS_OFF);
err3:
	isp_xc9080_remove_clock(&isp_xc9080_info);
err2:
	isp_xc9080_power_up(&isp_xc9080_info, 0);
err1:
	csi_core_close();
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
#ifdef CONFIG_VIDEO_STK3855
	class_unregister(&isp_xc9080_class);
#endif
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
MODULE_AUTHOR("sam");
MODULE_DESCRIPTION("driver for xc9080.");
MODULE_LICENSE("Dual BSD/GPL");
