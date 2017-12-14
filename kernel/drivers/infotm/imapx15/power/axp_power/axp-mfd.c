/*
 * Base driver for X-Powers AXP
 *
 * Copyright (C) 2011 X-Powers, Ltd.
 *  Zhang Donglu <zhangdonglu@x-powers.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <mach/pad.h>
#include <mach/items.h>
#include <mach/irqs.h>
#include <mach/imap-iomap.h>
#include <asm/io.h>

#include "axp-cfg.h"
#include "axp-rw.h"
#include "axp-regu.h"
#include "axp-irq.h"
#include "axp-common.h"

struct axp_mfd_chip *pchip;
 
extern void __imapx_register_pcbtest_batt_v(int (*func1)(void), int (*func2)(void));
extern void __imapx_register_pcbtest_chg_state(int (*func1)(void));
extern void imapx15_restart(char, const char *);
extern int belt_rebooton_flag(void);

int axp_get_batt_voltage(struct device *dev)
{
	int vol;
	uint8_t high, low;

	axp_read(dev, 0x78, &high);
	axp_read(dev, 0x79, &low);

	vol = (high<<4)|low;
	vol = vol*11/10;
	printk("ADC get batt voltage is %d\n", vol);

	return vol;
}
EXPORT_SYMBOL(axp_get_batt_voltage);

#define AXP_PCBTEST_ABS(X) ((X)>0?(X):-(X))
 
int axp_pcbtest_get_batt_voltage(void)
{
	int vol;
	uint8_t high=0, low=0;

	printk("axp_pcbtest_get_batt_voltage()\n");

	__axp_read(pchip->client, 0x78, &high);
	__axp_read(pchip->client, 0x79, &low);

	vol = (high<<4)|low;
	vol = vol *11/10;
	printk("axp202 get batt voltage is %d\n", vol);

	return vol;
}

int axp_pcbtest_get_batt_cap(void)
{
	uint8_t temp[8] = {0};
	uint8_t value1=0, value2=0;
	int64_t rValue1, rValue2, rValue;
	uint8_t m=0;
	int Cur_coulombCounter_tmp;

	printk("axp_pcbtest_get_batt_cap() \n");
	__axp_read(pchip->client, 0xB0, &temp[0]);
	__axp_read(pchip->client, 0xB1, &temp[1]);
	__axp_read(pchip->client, 0xB2, &temp[2]);
	__axp_read(pchip->client, 0xB3, &temp[3]);
	__axp_read(pchip->client, 0xB4, &temp[4]);
	__axp_read(pchip->client, 0xB5, &temp[5]);
	__axp_read(pchip->client, 0xB6, &temp[6]);
	__axp_read(pchip->client, 0xB7, &temp[7]);
	rValue1 = ((temp[0]<<24) + (temp[1]<<16) + (temp[2] << 8) + temp[3]);
	rValue2 = ((temp[4]<<24) + (temp[5]<<16) + (temp[6] << 8) + temp[7]);

	rValue = (AXP_PCBTEST_ABS(rValue1 - rValue2))*4369;

	__axp_read(pchip->client, 0x84, &m);
	m &= 0xc0;
	switch(m>>6)
	{
		case 0: m = 25;break;
		case 1: m = 50;break;
		case 2: m = 100;break;
		case 3: m = 200;break;
		default: return 0xff;break;
	}
	//m = axp_get_freq() * 480;
	do_div(rValue, m);
	if(rValue1 >= rValue2)
		Cur_coulombCounter_tmp = (int)rValue;
	else
		Cur_coulombCounter_tmp = (int)(0-rValue);

	Cur_coulombCounter_tmp = AXP_PCBTEST_ABS(Cur_coulombCounter_tmp);

	__axp_read(pchip->client, 0x05, &value1);
	__axp_read(pchip->client, 0xB9, &value2);
	if(AXP_PCBTEST_ABS((value1 & 0x7F)-(value2 & 0x7F))>= 10 || Cur_coulombCounter_tmp > 50)
		return (value2 & 0x7F);
	else
		return (value1 & 0x7F);
}

int axp_pcbtest_get_ac_st(void)
{
	uint8_t st;

	printk("axp_pcbtest_get_ac_st()\n");

	__axp_read(pchip->client, 0x00, &st);
	st &= 0x3f;
	if((st & 0x40) || (st & 0x80))
		return 0x01;
	else
		return 0x00;
}

static int axp_init_chip(struct axp_mfd_chip *chip)
{
	uint8_t chip_id;
	int err;
#if defined (CONFIG_AXP_DEBUG)
	uint8_t val[AXP_DATA_NUM];
	int i;
#endif
	/*read chip id*/
	err =  __axp_read(chip->client, AXP_IC_TYPE, &chip_id);
	if (err) {
		printk("[AXP-MFD] try to read chip id failed!\n");
		return err;
	}

	dev_info(chip->dev, "AXP (CHIP ID: 0x%02x) detected\n", chip_id);

	/* disable and clear all IRQs */
#if defined (CONFIG_KP_AXP20)
	chip->ops->disable_irqs(chip, 0xffffffff, 0xff);
	chip->ops->clear_irqs(chip, 0xffffffff, 0xff);
#endif

#if defined (CONFIG_KP_AXP19)
	chip->ops->disable_irqs(chip,0xffffffff, 0x0);
	chip->ops->clear_irqs(chip,0xffffffff, 0x0);
#endif

#if defined (CONFIG_AXP_DEBUG)
	axp_reads(&axp->dev,AXP_DATA_BUFFER0,AXP_DATA_NUM,val);
	for( i = 0; i < AXP_DATA_NUM; i++){
		printk("REG[0x%x] = 0x%x\n",i+AXP_DATA_BUFFER0,val[i]);	
	}
#endif
	return 0;
}

static int axp_disable_irqs(struct axp_mfd_chip *chip, uint32_t irqs_low, uint32_t irqs_high)
{
#if defined (CONFIG_KP_AXP20)
	uint8_t v[9];
#endif

#if defined (CONFIG_KP_AXP19)
	uint8_t v[7];
#endif
	int ret;

	chip->irqs_enabled_low &= ~irqs_low;
	chip->irqs_enabled_high &= ~irqs_high;

	v[0] = ((chip->irqs_enabled_low) & 0xff);
	v[1] = AXP_INTEN2;
	v[2] = ((chip->irqs_enabled_low) >> 8) & 0xff;
	v[3] = AXP_INTEN3;
	v[4] = ((chip->irqs_enabled_low) >> 16) & 0xff;
	v[5] = AXP_INTEN4;
	v[6] = ((chip->irqs_enabled_low) >> 24) & 0xff;
#if defined (CONFIG_KP_AXP20)
	v[7] = AXP_INTEN5;
	v[8] = (chip->irqs_enabled_high) & 0xff;
	ret =  __axp_writes(chip->client, AXP_INTEN1, 9, v);
#endif
#if defined (CONFIG_KP_AXP19)
	ret =  __axp_writes(chip->client, AXP_INTEN1, 7, v);
#endif
	return ret;

}

static int axp_enable_irqs(struct axp_mfd_chip *chip, uint32_t irqs_low, uint32_t irqs_high)
{
#if defined (CONFIG_KP_AXP20)
	uint8_t v[9];
#endif

#if defined (CONFIG_KP_AXP19)
	uint8_t v[7];
#endif
	int ret;

	chip->irqs_enabled_low |=  irqs_low;
	chip->irqs_enabled_high |=  irqs_high;

	v[0] = ((chip->irqs_enabled_low) & 0xff);
	v[1] = AXP_INTEN2;
	v[2] = ((chip->irqs_enabled_low) >> 8) & 0xff;
	v[3] = AXP_INTEN3;
	v[4] = ((chip->irqs_enabled_low) >> 16) & 0xff;
	v[5] = AXP_INTEN4;
	v[6] = ((chip->irqs_enabled_low) >> 24) & 0xff;
#if defined (CONFIG_KP_AXP20)
	v[7] = AXP_INTEN5;
	v[8] = (chip->irqs_enabled_high) & 0xff;
	ret =  __axp_writes(chip->client, AXP_INTEN1, 9, v);
#endif
#if defined (CONFIG_KP_AXP19)
	ret =  __axp_writes(chip->client, AXP_INTEN1, 7, v);
#endif
	return ret;
}

#if defined (CONFIG_KP_AXP20)
static int axp_read_irqs(struct axp_mfd_chip *chip, uint32_t *irqs_low, uint32_t *irqs_high)
{
	uint8_t v[5];
	int ret;

	ret =  __axp_reads(chip->client, AXP_INTSTS1, 5, v);
	*irqs_low =(v[3] << 24) | (v[2] << 16) | (v[1]  << 8) |v[0];
	*irqs_high =  v[4];

	return ret;
}
#endif

#if defined (CONFIG_KP_AXP19)
static int axp_read_irqs(struct axp_mfd_chip *chip, uint32_t *irqs_low, uint32_t *irqs_high)
{
	uint8_t v[4];
	int ret;
	ret =  __axp_reads(chip->client, AXP_INTSTS1, 4, v);

	*irqs_low =(v[3] << 24) | (v[2] << 16) | (v[1] << 8) |v[0];
	*irqs_high = 0;

	return ret;
}
#endif

static int axp_clear_irqs(struct axp_mfd_chip *chip, uint32_t irqs_low, uint32_t irqs_high)
{
#if defined (CONFIG_KP_AXP20)
	uint8_t v[9];
#endif

#if defined (CONFIG_KP_AXP19)
	uint8_t v[7];
#endif
	int ret;
	v[0] = (irqs_low >>  0)& 0xFF;
	v[1] = AXP_INTSTS2;
	v[2] = (irqs_low >>  8) & 0xFF;
	v[3] = AXP_INTSTS3;
	v[4] = (irqs_low >> 16) & 0xFF;
	v[5] = AXP_INTSTS4;
	v[6] = (irqs_low >> 24) & 0xFF;
#if defined (CONFIG_KP_AXP20)
	v[7] = AXP_INTSTS5;
	v[8] = (irqs_high >> 0) & 0xFF;
	ret =  __axp_writes(chip->client, AXP_INTSTS1, 9, v);
#endif
#if defined (CONFIG_KP_AXP19)
	ret =  __axp_writes(chip->client, AXP_INTSTS1, 7, v);
#endif
	return ret;
}

static struct axp_mfd_chip_ops axp_mfd_ops[] = {
	[0] = {
		.init_chip		= axp_init_chip,
		.enable_irqs	= axp_enable_irqs,
		.disable_irqs	= axp_disable_irqs,
		.read_irqs		= axp_read_irqs,
		.clear_irqs		= axp_clear_irqs,
	},
};

int axp_charger_in(void)
{
	uint8_t val;

	axp_read(&axp->dev, AXP_STATUS, &val);
	return !!(((val & AXP_STATUS_USBEN) && (val & AXP_STATUS_USBVA))
			|| ((val & AXP_STATUS_ACEN) && (val & AXP_STATUS_ACVA)));
}
EXPORT_SYMBOL(axp_charger_in);

static irqreturn_t axp_mfd_irq_work(int irq, void *data)
{
	struct axp_mfd_chip *chip = (struct axp_mfd_chip *) data;
	uint32_t irqs_low = 0;
	uint32_t irqs_high = 0;
	uint32_t irqs[2] = {0};


	if (chip->ops->read_irqs(chip, &irqs_low, &irqs_high))
		goto exit;

	irqs_low &= chip->irqs_enabled_low;
	irqs_high &= chip->irqs_enabled_high;
	if ((irqs_low == 0) && (irqs_high == 0))
		goto exit;

	irqs[0] = irqs_low;
	irqs[1] = irqs_high;
	blocking_notifier_call_chain(&chip->notifier_list, 0, (void *)irqs);

exit:
	/* clear irqs */
	chip->ops->clear_irqs(chip, irqs_low, irqs_high);
	axp_rtcgp_irq_enable(1);

	return IRQ_HANDLED;
}

static irqreturn_t axp_mfd_irq_handler(int irq, void *data)
{
	//struct axp_mfd_chip *chip = data;

	axp_rtcgp_irq_disable(1);
	axp_rtcgp_irq_clr(1);

	return IRQ_WAKE_THREAD;
}

static const struct i2c_device_id axp_mfd_id_table[] = {
	{ "axp_mfd", 0 },
	{},
};
MODULE_DEVICE_TABLE(i2c, axp_mfd_id_table);

static int __remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int axp_mfd_remove_subdevs(struct axp_mfd_chip *chip)
{
	return device_for_each_child(chip->dev, NULL, __remove_subdev);
}

static int axp_mfd_add_subdevs(struct axp_mfd_chip *chip,
					struct axp_platform_data *pdata)
{
	struct axp_funcdev_info *regl_dev;
	struct axp_funcdev_info *sply_dev;
	struct platform_device *pdev;
	int i, ret = 0;


	/* register for regultors */
	for (i = 0; i < pdata->num_regl_devs; i++) {
		regl_dev = &pdata->regl_devs[i];
		pdev = platform_device_alloc(regl_dev->name, regl_dev->id);
		pdev->dev.parent = chip->dev;
		pdev->dev.platform_data = regl_dev->platform_data;
		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}

	/* register for power supply */
	for (i = 0; i < pdata->num_sply_devs; i++) {
		sply_dev = &pdata->sply_devs[i];
		pdev = platform_device_alloc(sply_dev->name, sply_dev->id);
		pdev->dev.parent = chip->dev;
		pdev->dev.platform_data = sply_dev->platform_data;
		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}

	return 0;

failed:
	axp_mfd_remove_subdevs(chip);
	return ret;
}

/* power off system */
void axp_power_off(void)
{
	//uint8_t val;


	/* clear coulomb */
#if defined (CONFIG_USE_OCV)
	axp_read(&axp->dev, AXP_COULOMB_CTL, &val);
	val &= 0x3f;
	axp_write(&axp->dev, AXP_COULOMB_CTL, val);
	val |= 0x80;
	val &= 0xbf;
	axp_write(&axp->dev, AXP_COULOMB_CTL, val);
#endif

	printk("[axp] send power-off command!\n");
	mdelay(20);


	/* as knowing: there are two bug during shuting down
	 *
	 * 1: charger power mask on && charger in
	 *    enter bug state whether soft or force shutting down
	 * 2: charger power mask on && charger not in
	 *    enter bug state if force shutting down
	 *
	 * so we must use a trick to avoid the machine from entering
	 * bug state if user wants to shut down in these situations.
	 *
	 * charger.pwron is to be depracated as it's not a standard
	 * method to perform charging
	 */
	if(item_exist("power.acboot")
			&& item_equal("power.acboot", "1", 0)
			&& axp_charger_in()) {  /* use axp202 to get charger status */
		imapx15_restart(0, "charger");
	} else {
		/*if (1 == belt_rebooton_flag()) {
			imapx_pad_set_mode(1, 1, 25);
			imapx_pad_set_dir(0, 1, 25);
			imapx_pad_set_outdat(1, 1, 25);
			mdelay(100);
			imapx_pad_set_outdat(0, 1, 25);
			printk("rebooton power off & power on\n");
		}*/
		axp_write(&axp->dev, AXP_DATA_BUFFERB, 0x00); /* record status of shutdown, distinguish between shutdown and sleep */
		mdelay(1);
		axp_set_bits(&axp->dev, AXP_OFF_CTL, 0x1 << 2);
		mdelay(1);
		axp_set_bits(&axp->dev, AXP_OFF_CTL, 0x1 << 7);
		mdelay(20);
		printk("[axp] warning!!! axp can't power-off, maybe some error happend!\n");
	}
}
EXPORT_SYMBOL(axp_power_off);

static int axp_mfd_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct axp_platform_data *pdata = client->dev.platform_data;
	struct axp_mfd_chip *chip;
	int ret;
	int irq_used;

	chip = kzalloc(sizeof(struct axp_mfd_chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;
	pchip = chip;
	printk("[AXP]: axp_mfd_probe !\n");

	axp_mode_scan(&client->dev);
	irq_used = axp_irq_used();

	axp = client;

	chip->client = client;
	chip->dev = &client->dev;
	chip->ops = &axp_mfd_ops[id->driver_data];

	mutex_init(&chip->lock);

	i2c_set_clientdata(client, chip);

	ret = chip->ops->init_chip(chip);
	if (ret)
		goto out_free_chip;

	if(irq_used) {
	    client->irq = GIC_RTCGP1_ID;
	    axp_rtcgp_irq_config(1);
	    BLOCKING_INIT_NOTIFIER_HEAD(&chip->notifier_list);
	    ret = request_threaded_irq(client->irq, axp_mfd_irq_handler, axp_mfd_irq_work,
		    IRQF_SHARED, "axp_mfd", chip);
	    if(ret) {
		dev_err(&client->dev, "failed to request irq %d\n",
			client->irq);
		goto out_free_chip;
	    }
	}

	axp_mfd_minit(&client->dev);

	ret = axp_mfd_add_subdevs(chip, pdata);
	if (ret)
		goto out_free_irq;

	printk("axp202 register function for pcbtest!\n");

//	__imapx_register_pcbtest_batt_v(axp_pcbtest_get_batt_voltage, axp_pcbtest_get_batt_cap);
//	__imapx_register_pcbtest_chg_state(axp_pcbtest_get_ac_st);

	printk("axp202 register function for pcbtest success!!!");

	return ret;

out_free_irq:
	if(client->irq) {
	    free_irq(client->irq, chip);
	}

out_free_chip:
	printk("error exit: out_free_chip\n");
	i2c_set_clientdata(client, NULL);
	kfree(chip);

	return ret;
}

static int axp_mfd_remove(struct i2c_client *client)
{
	struct axp_mfd_chip *chip = i2c_get_clientdata(client);

	axp = NULL;

	axp_mfd_remove_subdevs(chip);
	kfree(chip);
	return 0;
}

static int axp_mfd_suspend(struct i2c_client *client, pm_message_t mesg)
{
	axp_mfd_msuspend(&client->dev);

	return 0;
}

static int axp_mfd_resume(struct i2c_client *client)
{
	//struct axp_mfd_chip *chip = i2c_get_clientdata(client);
	//uint8_t val;

	axp_mfd_mresume(&client->dev);

	return 0;
}
/*
static const unsigned short axp_i2c[] = {
	AXP_DEVICES_ADDR, I2C_CLIENT_END };
*/
static struct i2c_driver axp_mfd_driver = {
	//.class = I2C_CLASS_HWMON,
	.driver	= {
		.name	= "axp_mfd",
		.owner	= THIS_MODULE,
	},
	.probe		= axp_mfd_probe,
	.remove		= axp_mfd_remove,
	.suspend		= axp_mfd_suspend,
	.resume		= axp_mfd_resume,
	.id_table	= axp_mfd_id_table,
	//.detect         = axp_detect,
	//.address_list   = axp_i2c,
};

static int __init axp_mfd_init(void)
{
	//char buf[ITEM_MAX_LEN];

	if(item_equal("pmu.model", "axp202", 0)) {
		return i2c_add_driver(&axp_mfd_driver);
	}else {
		printk("pmu is not axp202!\n");
	}

	return -1;
}
subsys_initcall(axp_mfd_init);

static void __exit axp_mfd_exit(void)
{
	i2c_del_driver(&axp_mfd_driver);
}
module_exit(axp_mfd_exit);

MODULE_DESCRIPTION("MFD Driver for X-Powers AXP PMIC");
MODULE_AUTHOR("Donglu Zhang, <zhangdonglu@x-powers.com>");
MODULE_LICENSE("GPL");
