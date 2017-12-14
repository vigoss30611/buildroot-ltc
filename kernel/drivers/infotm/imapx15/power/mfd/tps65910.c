/*
 * tps65910.c  --  TI TPS65910
 *
 * Copyright 2012 Shanghai Infotm Inc.
 *
 * Author: zhanglei <lei_zhang@infotm.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 */

//#define TPS65910_DEBUG

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/mfd/core.h>
#include <linux/mfd/tps65910.h>
#include <linux/mfd/tps65910_imapx800.h>
#include <mach/items.h>

//#define CONFIG_PMIC_I2C	(0)

struct tps65910 *ptps65910;

static struct mfd_cell tps65910s[] = {
	{
		.name = "tps65910-pmic",
	},
};

static int tps65910_i2c_read(struct tps65910 *tps65910, u8 reg,
				  int bytes, void *dest)
{
	struct i2c_client *i2c = tps65910->i2c_client;
	struct i2c_msg xfer[2];
	int ret;

	//printk("tps65910_i2c_read()  run !\n");

	/* Write register */
	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 1;
	xfer[0].buf = &reg;

	/* Read data */
	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = bytes;
	xfer[1].buf = dest;

	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret == 2)
		ret = 0;
	else if (ret < 0)
		ret = -EIO;

	return ret;
}

static int tps65910_i2c_write(struct tps65910 *tps65910, u8 reg,
				   int bytes, void *src)
{
	struct i2c_client *i2c = tps65910->i2c_client;
	/* we add 1 byte for device register */
	u8 msg[TPS65910_MAX_REGISTER + 1];
	int ret;

	if (bytes > TPS65910_MAX_REGISTER)
		return -EINVAL;

	//printk("tps65910_i2c_write()  run !\n");

	msg[0] = reg;
	memcpy(&msg[1], src, bytes);

	ret = i2c_master_send(i2c, msg, bytes + 1);
	if (ret < 0)
		return ret;
	if (ret != bytes + 1)
		return -EIO;
	return 0;
}

#ifdef TPS65910_DEBUG
static u8 tps_vreg[] = {
		0x00, 0x01, 0x02, 0x03, 0x04,
		0x05, 0x06, 0x08, 0x09, 0x0a,
		0x0b, 0x0c, 0x0d, 0x10, 0x11,
		0x12, 0x13, 0x14, 0x15, 0x16,
		0x17, 0x18, 0x19, 0x1a, 0x1b,
		0x1c, 0x1d, 0x1e, 0x20, 0x21,
		0x22, 0x23, 0x24, 0x25, 0x26,
		0x27, 0x30, 0x31, 0x32, 0x33,
		0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3e, 0x3f, 0x40, 0x41, 
		0x42, 0x43, 0x44, 0x45, 0x46, 
		0x47, 0x48, 0x49, 0x4a, 0x50, 
		0x51, 0x52, 0x53, 0x60, 0x80,
	};
#endif

static u8 tps_reg_init[][2] = {
	{TPS65910_DEVCTRL, 0x32},// 0011 0010
	{TPS65910_DEVCTRL2, 0x34},
	
	{TPS65910_SLEEP_KEEP_LDO_ON, 0x00},
	{TPS65910_SLEEP_KEEP_RES_ON, 0x00},
	{TPS65910_SLEEP_SET_LDO_OFF, 0xfc},
	{TPS65910_SLEEP_SET_RES_OFF, 0x0e},//sleep config
	{TPS65910_EN1_LDO_ASS, 0x00},
	{TPS65910_EN1_SMPS_ASS, 0x00},
	{TPS65910_EN2_LDO_ASS, 0x00},
	{TPS65910_EN2_SMPS_ASS, 0x00},

	{TPS65910_INT_MSK, 0xff},
	{TPS65910_INT_MSK2, 0x03},
	{TPS65910_INT_STS, 0xff},
	{TPS65910_INT_STS2, 0xff},

	//{TPS65910_VDIG1, 0xD},
	//{TPS65910_VDIG2, 0xD},
	//{TPS65910_VAUX1, 0x9},
	//{TPS65910_VAUX2, 0xD},
	//{TPS65910_VAUX33, 0xD},
	//{TPS65910_VMMC, 0xD},
	//{TPS65910_VDAC, 0x9},

};

const u8 tps_reg_init_num = ARRAY_SIZE(tps_reg_init);

int tps65910_set_bits(struct tps65910 *tps65910, u8 reg, u8 mask)
{
	u8 data;
	int err;

	mutex_lock(&tps65910->io_mutex);
	err = tps65910_i2c_read(tps65910, reg, 1, &data);
	if (err) {
		dev_err(tps65910->dev, "read from reg %x failed\n", reg);
		goto out;
	}

	data |= mask;
	err = tps65910_i2c_write(tps65910, reg, 1, &data);
	if (err)
		dev_err(tps65910->dev, "write to reg %x failed\n", reg);

out:
	mutex_unlock(&tps65910->io_mutex);
	return err;
}
EXPORT_SYMBOL_GPL(tps65910_set_bits);

int tps65910_clear_bits(struct tps65910 *tps65910, u8 reg, u8 mask)
{
	u8 data;
	int err;

	mutex_lock(&tps65910->io_mutex);
	err = tps65910_i2c_read(tps65910, reg, 1, &data);
	if (err) {
		dev_err(tps65910->dev, "read from reg %x failed\n", reg);
		goto out;
	}

	data &= ~mask;
	err = tps65910_i2c_write(tps65910, reg, 1, &data);
	if (err)
		dev_err(tps65910->dev, "write to reg %x failed\n", reg);

out:
	mutex_unlock(&tps65910->io_mutex);
	return err;
}
EXPORT_SYMBOL_GPL(tps65910_clear_bits);

int tps65910_bkreg_wr(int reg, int val)
{
	
	if((reg < 1)||(reg > 5))
		return -ENOMEM;

	reg += 0x16;// reg address is (0x17 - 0x1b)
	tps65910_i2c_write(ptps65910, reg, 1, &val);
	printk("%s, reg = 0x%x, val = %d\n", __func__, reg, val);
	return 0;
}
EXPORT_SYMBOL(tps65910_bkreg_wr);

int tps65910_bkreg_rd(int reg)
{
	int val;
	if((reg < 1)||(reg > 5))
		return -ENOMEM;

	reg += 0x16;// reg address is (0x17 - 0x1b)
	tps65910_i2c_read(ptps65910, reg, 1, &val);
	printk("%s, reg = 0x%x, val = %d\n", __func__, reg, val);
	return val;
}
EXPORT_SYMBOL(tps65910_bkreg_rd);

struct tps_channel {
	const char *name;
	int found;
	uint8_t reg;
	uint8_t val;
};

static struct tps_channel channel_info[] = {
	{
		.name = "vdig1",
		.reg = TPS65910_VDIG1,
		.val = 0xD,
	},
	{
		.name = "vdig2",
		.reg = TPS65910_VDIG2,
		.val = 0xD,
	},
	{
		.name = "vdac",
		.reg = TPS65910_VDAC,
		.val = 0x9,
	},
	{
		.name = "vaux1",
		.reg = TPS65910_VAUX1,
		.val = 0x9,
	},
	{
		.name = "vaux2",
		.reg = TPS65910_VAUX2,
		.val = 0xD,
	},
	{
		.name = "vaux33",
		.reg = TPS65910_VAUX33,
		.val = 0xD,
	},
	{
		.name = "vmmc",
		.reg = TPS65910_VMMC,
		.val = 0xD,
	},
	{
		.name = "vaux2",
		.reg = TPS65910_VAUX2,
		.val = 0xD,
	}
};

static int ch_num = ARRAY_SIZE(channel_info);

static const char *dev_item[5] = {
	"camera.front.power_iovdd",
	"camera.front.power_dvdd",
	"camera.rear.power_iovdd",
	"camera.rear.power_dvdd",
	"codec.power",
};

static int check_reg_num(const char *buf)
{
	int i;
	int ret = -1;

	for(i=0; i<ch_num; i++) {
		if(!strcmp(buf, channel_info[i].name)) {
			ret = i;
			break;
		}
	}

	return ret;
}

static int tps65910_default_output(struct tps65910 *tps65910)
{
	int i;
	int num;
	int ret = 0;
	char buf[ITEM_MAX_LEN];

	for(i=0; i<ch_num; i++) {
		channel_info[i].found = 0;
		printk("found == %d\n", channel_info[i].found);
	}

	for(i=0;i<5;i++) {
		if(item_exist(dev_item[i])) {
			item_string(buf, dev_item[i], 1);
			num = check_reg_num(buf);
			if(num != -1) {
				if(channel_info[num].found == 0){
					ret |= tps65910_i2c_write(tps65910, channel_info[num].reg,
													0x1, &channel_info[num].val);
					channel_info[num].found = 1;
				}else
					continue;
			}else
				continue;
		}
	}

	ret |= tps65910_i2c_write(tps65910, channel_info[ch_num-1].reg,
									0x1, &channel_info[ch_num-1].val);

	return ret;
}

static int tps65910_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct tps65910 *tps65910;
	//struct tps65910_platform_data *pmic_plat_data;
	int ret = 0;
	int i;
	//int dat;
		
	//printk(KERN_INFO "****** %s run! ******\n", __func__);

	//pmic_plat_data = kzalloc(sizeof(struct tps65910_platform_data), GFP_KERNEL);
	//if (pmic_plat_data == NULL)
	//	return -ENOMEM;

	//pmic_plat_data = dev_get_platdata(i2c->dev.parent);
	//if (pmic_plat_data == NULL)
	//	return -EINVAL;

	tps65910 = kzalloc(sizeof(struct tps65910), GFP_KERNEL);
	if (tps65910 == NULL) {
		//kfree(pmic_plat_data);
		return -ENOMEM;
	}

	ptps65910 = tps65910;
	i2c_set_clientdata(i2c, tps65910);
	tps65910->dev = &i2c->dev;
	tps65910->i2c_client = i2c;
	tps65910->id = id->driver_data;
	tps65910->read = tps65910_i2c_read;
	tps65910->write = tps65910_i2c_write;
	mutex_init(&tps65910->io_mutex);

	#ifdef TPS65910_DEBUG
	printk("for testing pmic, read all register\n");
	for(i= 0; i<65 ; i++)
	{
		ret  = tps65910_i2c_read(tps65910, tps_vreg[i], 0x01, &dat);
		if(ret == 0)
			printk("tps65910 read reg 0x%x =0x%x OK\n", tps_vreg[i], dat);
		else
			printk("tps65910 read reg 0x%x =0x%x  Err\n", tps_vreg[i], dat);
	}
	#endif

	//printk("tps_reg_init run!\n");
	for(i = 0; i< tps_reg_init_num; i++){
		ret = tps65910_i2c_write(tps65910, tps_reg_init[i][0], 0x01, &tps_reg_init[i][1]);
		if(ret < 0)
		{
			printk("tps65910 register init fail, regaddr = 0x%x\n", tps_reg_init[i][0]);
			goto err2;
		}
		//ret = tps65910_i2c_read(tps65910, tps_reg_init[i][0], 0x01, &dat);
		//printk("tps65910 read result of register init, 0x%x = 0x%x\n", tps_reg_init[i][0], dat);
	}
	//printk("tps_reg_init_run finish\n");

	if(!tps65910_default_output(tps65910))
		printk("tps65910_default_output OK!\n");
	else
		printk("tps65910_default_output ERROR!\n");

	ret = mfd_add_devices(tps65910->dev, -1,
			      tps65910s, ARRAY_SIZE(tps65910s),
			      NULL, 0, NULL);
	//printk("mfd_add_devices() return %d\n", ret);
	if (ret < 0)
		goto err;

	//tps65910_gpio_init(tps65910, pmic_plat_data->gpio_base);

	//ret = tps65910_irq_init(tps65910, pmic_plat_data->irq, pmic_plat_data);
	//if (ret < 0)
	//	goto err;
	//printk("****** %s End! ******\n", __func__);

	return ret;

err:
	printk("%s err\n", __func__);
	mfd_remove_devices(tps65910->dev);
err2:
	kfree(tps65910);
	//kfree(pmic_plat_data);
	return ret;
}

static int tps65910_i2c_remove(struct i2c_client *i2c)
{
	struct tps65910 *tps65910 = i2c_get_clientdata(i2c);
	u8 reg_dat;
	int ret;

	reg_dat = 0x8f;
	ret = tps65910_i2c_write(tps65910, TPS65910_SLEEP_SET_RES_OFF, 0x01, &reg_dat);

	printk("%s run !!!\n", __func__);
	mfd_remove_devices(tps65910->dev);
	//tps65910_irq_exit(tps65910);
	ptps65910 = NULL;
	kfree(tps65910);

	return 0;
}

static int tps65910_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
	return 0;
}

static int tps65910_i2c_resume(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id tps65910_i2c_id[] = {
	{ "tps65910", TPS65910 },
       { }
};
MODULE_DEVICE_TABLE(i2c, tps65910_i2c_id);


static struct i2c_driver tps65910_i2c_driver = {
	.driver = {
		   .name = "tps65910",
		   .owner = THIS_MODULE,
	},
	.probe = tps65910_i2c_probe,
	.remove = tps65910_i2c_remove,
	.suspend = tps65910_i2c_suspend,
	.resume = tps65910_i2c_resume,
	.id_table = tps65910_i2c_id,
};

static int __init tps65910_i2c_init(void)
{
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	printk(KERN_ERR "****** %s run ! ******\n", __func__);

	if(item_exist("pmu.model"))
	{
		if(item_equal("pmu.model", "tps65910", 0))
		{
			memset(&info, 0, sizeof(struct i2c_board_info));
			info.addr = 0x2d;
			strlcpy(info.type, "tps65910", I2C_NAME_SIZE);
			info.flags = I2C_CLIENT_WAKE;
			info.platform_data = &tps65910_platform_data;

			adapter = i2c_get_adapter(item_integer("pmu.ctrl", 1));
			if(!adapter)
			printk(KERN_ERR "******i2c_get_adapter error!******\n");

			client = i2c_new_device(adapter, &info);

			return i2c_add_driver(&tps65910_i2c_driver);
		}
		else
			printk(KERN_ERR "%s: pmu model is not tps65910\n", __func__);
	}
	else
		printk(KERN_ERR "%s: pmu model is not exist\n", __func__);

	return -1;
}
/* init early so consumer devices can complete system boot */
//module_init(tps65910_i2c_init);
//late_initcall(tps65910_i2c_init);
subsys_initcall(tps65910_i2c_init);

static void __exit tps65910_i2c_exit(void)
{

	i2c_del_driver(&tps65910_i2c_driver);
}
module_exit(tps65910_i2c_exit);

MODULE_AUTHOR("zhanglei <lei_zhang@infotm.com>");
MODULE_DESCRIPTION("TPS65910 chip multi-function driver");
MODULE_LICENSE("GPL");

