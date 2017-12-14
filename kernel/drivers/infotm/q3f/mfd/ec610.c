/*
 * driver/mfd/ec610.c
 *
 * Core driver implementation to access Elitechip EC610 power management chip.
 *
 * Copyright (C) 2012-2013 Elitechip COMPANY,LTD
 *
 * Based on code
 *	Copyright (C) 2011 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/mfd/core.h>
#include <linux/seq_file.h>
#include <mach/ec610.h>
#include <mach/hw_cfg.h>
#include <mach/pad.h>

struct ec610_reg_val {
	u8 reg_address;
	u8 reg_value;
};

static struct ec610_reg_val init_data[] = {	
/*	{0x04, 0xE6},
	{0x23, 0xE4},
	{0x24, 0xF0},
	{0x25, 0x0D},
	{0x27, 0xFC},
	{0x2A, 0xE4},
	{0x2B, 0xF0},
	{0x2C, 0x0D},
	{0x2E, 0xFC},
	{0x31, 0xE4},
	{0x32, 0xF0},
	{0x33, 0x0D},
	{0x35, 0xFC},
	{0x38, 0xE4},
	{0x39, 0xF0},
	{0x3A, 0x0F},
	{0x3B, 0xC7},
	{0x3C, 0xFC},
	{0x43, 0x1C},
	{0x45, 0x1C},
	{0x47, 0x1C},
	{0x49, 0x1C},
	{0x4B, 0x1C},
	{0x4D, 0x1C},
	{0x4F, 0x1C},*/

	{0x43, 0x1C},
	{0x45, 0x1C},
	{0x47, 0x1C},
	{0x49, 0x1C},
	{0x4b, 0x1c},
	{0x4d, 0x1c},
	{0x4F, 0x1C},
	{0x44, 0x68},/* ldo1 3.3V */
	{0x46, 0x2c},/* ldo2 1.8V */
	{0x4a, 0x54},/* ldo4 2.8V */
	{0x4c, 0x54},/* ldo5 2.8V */
	{0x07, 0x34},
	{0x08, 0x80},
};

struct i2c_client *ec610_i2c_client = NULL;

static int ec610_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ec610_remove_subdevs(struct ec610 *ec610)
{
	return device_for_each_child(ec610->dev, NULL,
				     ec610_remove_subdev);
}

static int ec610_add_subdevs(struct ec610 *ec610,
				struct ec610_platform_data *pdata)
{
	struct ec610_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ec610->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}
	return 0;

failed:
	ec610_remove_subdevs(ec610);
	return ret;
}

static inline int ec610_i2c_write(struct i2c_client *client,
					uint8_t reg, uint8_t val)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret < 0) {
		dev_err(&client->dev, "failed writing 0x%02x to 0x%02x, ret=%d\n",
				val, reg, ret);
		return ret;
	}

	return 0;
}

static inline int ec610_i2c_read(struct i2c_client *client,
					uint8_t reg, uint8_t *val)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading at 0x%02x, ret=%d\n",
				reg, ret);
		return ret;
	}

	*val = (uint8_t)ret;

	return 0;
}

int ec610_write(struct device *dev, u8 reg, uint8_t val)
{
	struct ec610 *ec610 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ec610->io_lock);
	ret = ec610_i2c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ec610->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ec610_write);

int ec610_read(struct device *dev, u8 reg, uint8_t *val)
{
	struct ec610 *ec610 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ec610->io_lock);
	ret = ec610_i2c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ec610->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ec610_read);

int ec610_set_bits(struct device *dev, u8 reg, uint8_t bit_mask)
{
	struct ec610 *ec610 = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ec610->io_lock);
	ret = ec610_i2c_read(to_i2c_client(dev), reg, &reg_val);
	if (ret)
		goto out;

	if ((reg_val & bit_mask) != bit_mask) {
		reg_val |= bit_mask;
		ret = ec610_i2c_write(to_i2c_client(dev), reg,	reg_val);
	}

out:
	mutex_unlock(&ec610->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ec610_set_bits);

int ec610_clr_bits(struct device *dev, u8 reg, uint8_t bit_mask)
{
	struct ec610 *ec610 = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ec610->io_lock);
	ret = ec610_i2c_read(to_i2c_client(dev), reg, &reg_val);
	if (ret)
		goto out;

	if (reg_val & bit_mask) {
		reg_val &= ~bit_mask;
		ret = ec610_i2c_write(to_i2c_client(dev), reg, reg_val);
	}

out:
	mutex_unlock(&ec610->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ec610_clr_bits);

void ec610_chip_init(struct ec610 *ec610, struct pmu_cfg *ip)
{
	u8 i = 0;
	u8 value = 0;

	/* set DCDC0 voltage*/
	if(ip->core_voltage == -1)
		value = 0x24;
	else if(ip->core_voltage >= 1050)
		value = (ip->core_voltage - 600)/25;
	else if(ip->core_voltage < 1050){
		printk("The core voltage must be bigger than 1.05V\n");
		return;
	}
	ec610_write(ec610->dev, 0x21, value);
	
//	ec610_read(ec610->dev, 0x07,	&value);
//	printk("ec610:0x07: 0x%x\n", value);
//	ec610_read(ec610->dev, 0x08,	&value);
//	printk("ec610:0x08: 0x%x\n", value);
	
	EC610_DBG("EC610 regs write: \n");
	for (i = 0; i < (sizeof(init_data)/sizeof(struct ec610_reg_val)); i++) {
		EC610_DBG("[0x%x, 0x%x]\n", init_data[i].reg_address, init_data[i].reg_value);
		ec610_write(ec610->dev, init_data[i].reg_address, init_data[i].reg_value);
	}
	
	/*EC610_DBG("EC610 regs read: \n");
	for (i = 0; i < (sizeof(init_data)/sizeof(struct ec610_reg_val)); i++) {
		ec610_read(ec610->dev, init_data[i].reg_address, &value);
		EC610_DBG("[0x%x, 0x%x]\n", init_data[i].reg_address, value);
	}*/

	return;
}

/*void ec610_interrupt_init(struct ec610 *ec610)
{
	u8 value=0;
	
	// irq output enable.
	ec610_read(ec610->dev, EC610_MFP_CTL5, &value);
	value &= 0xE0;
	value |= EC610_IRQ_OUT|0x10;
	printk("EC610_MFP_CTL5: value = 0x%x\n", value);
	ec610_write(ec610->dev, EC610_MFP_CTL5, value);
	
	// clear flags.
	value = 0xff;
	ec610_write(ec610->dev, EC610_INTS_FLAG0, value);
	ec610_write(ec610->dev, EC610_INTS_FLAG1, value);
	
	// enable irq.
	value = DEFAULT_INT_MASK;
	printk("EC610_INTS_CTL0: value = 0x%x\n", value);
	ec610_write(ec610->dev, EC610_INTS_CTL0, value);
	value = 0x00;
	ec610_write(ec610->dev, EC610_INTS_CTL1, value); 
}*/

void ec610_sleep(void)
{
	u8 value=0;
	
	// set wakeup sources.
	ec610_i2c_read(ec610_i2c_client, EC610_WAKE_CTL, &value);
	value |= (EC610_WK_EN_ONOFF_S | EC610_WK_EN_ONOFF_L | EC610_ONOFF_SL_RST);
	value &= ~EC610_WK_EN_VINOK;
	ec610_i2c_write(ec610_i2c_client, EC610_WAKE_CTL, value);
	
	// which power must be keep when sleep.
	value = EC610_SL_OFF_SCAL_2MS | EC610_SL_OFF_SCH | EC610_SL_DC2_PWR;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL0, value);
	
	// set to sleep mode.
	value = EC610_DEEP_SL_CTL;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL1, value);
	
//	// disable irq.
//	ec610_i2c_read(ec610_i2c_client, EC610_MFP_CTL5, &value);
//	value &=~EC610_IRQ_CTL;
//	ec610_i2c_write(ec610_i2c_client, EC610_MFP_CTL5, value);
	
//	// goto sleep.
//	ec610_i2c_read(ec610_i2c_client, EC610_STATUS_CTL, &value);
//	value &= ~EC610_SLEEP_EN_n;
//	ec610_i2c_write(ec610_i2c_client, EC610_STATUS_CTL, &value);
}

void ec610_deep_sleep(void)
{
	u8 value=0;
	
	// set wakeup sources.
	ec610_i2c_read(ec610_i2c_client, EC610_WAKE_CTL, &value);
	value |= (EC610_WK_EN_ONOFF_S | EC610_WK_EN_ONOFF_L | EC610_ONOFF_SL_RST);
	value &= ~EC610_WK_EN_VINOK;
	ec610_i2c_write(ec610_i2c_client, EC610_WAKE_CTL, value);
	
	// which power must be keep when sleep.
	/*ec610_i2c_read(ec610_i2c_client, EC610_SL_PWR_CTL0, &value);
	value &= (~0x7F);
	value |= EC610_SL_OFF_SCAL_2MS |	EC610_SL_OFF_SCH;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL0, value);*/
	
	// set to deep sleep mode.
	value = 0x00;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL1, value);
	
	// disable irq.
	ec610_i2c_read(ec610_i2c_client, EC610_MFP_CTL5, &value);
	value &=~EC610_IRQ_CTL;
	ec610_i2c_write(ec610_i2c_client, EC610_MFP_CTL5, value);
	
	// goto deep sleep.
	ec610_i2c_read(ec610_i2c_client, EC610_STATUS_CTL, &value);
	value &= ~EC610_SLEEP_EN_n;
	ec610_i2c_write(ec610_i2c_client, EC610_STATUS_CTL, value);
}
EXPORT_SYMBOL(ec610_deep_sleep);

void ec610_reset(void)
{
	u8 value=0;
	
	// set wakeup sources.
	/*ec610_i2c_read(ec610_i2c_client, EC610_WAKE_CTL, &value);
	value |= (EC610_WK_EN_ONOFF_S | EC610_WK_EN_ONOFF_L);
	ec610_i2c_write(ec610_i2c_client, EC610_WAKE_CTL, value);*/
	
	// which power must be keep when sleep.
	/*ec610_i2c_read(ec610_i2c_client, EC610_SL_PWR_CTL0, &value);
	value &= (~0x7F);
	value |= EC610_SL_OFF_SCAL_2MS |	EC610_SL_OFF_SCH;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL0, value);*/
	
	// set to deep sleep mode.
	value = 0x00;
	ec610_i2c_write(ec610_i2c_client, EC610_SL_PWR_CTL1, value);
	
	// disable irq.
	ec610_i2c_read(ec610_i2c_client, EC610_MFP_CTL5, &value);
	value &=~EC610_IRQ_CTL;
	ec610_i2c_write(ec610_i2c_client, EC610_MFP_CTL5, value);
	
	// now reset. effect after 2 seconds.
	ec610_i2c_read(ec610_i2c_client, EC610_WAKE_CTL, &value);
	value |= 0x08;
	ec610_i2c_write(ec610_i2c_client, EC610_WAKE_CTL, value);
}
EXPORT_SYMBOL(ec610_reset);

/*
	IRQ functions 
*/
/*#define base    IO_ADDRESS(SYSMGR_RTC_BASE)
int ec610_gpio_irq_enable(int io, int en)
{
	uint8_t val;

	if (io > 3)
		return -EINVAL;

	val = readl(base + 0x5c);

	if (en)
		val |= 1 << io;
	else
		val &= ~(1 << io);

	writel(val, base + 0x5c);

	return 0;
}
EXPORT_SYMBOL(ec610_gpio_irq_enable);

int ec610_gpio_irq_clr(int io)
{
	uint8_t val;

	if (io > 3)
		return -EINVAL;

	val = readl(base + 0x8);
	if (val & (1 << (io+3)))
		writel((1 << (io+3)), base + 0x4);

	return 0;
}
EXPORT_SYMBOL(ec610_gpio_irq_clr);

int ec610_gpio_set_dir(int io, int dir)
{
	uint8_t val;

	if (io > 5)
		return -EINVAL;

	val = readl(base + 0x4c);
	if (dir)
		val |= 1 << io;
	else
		val &= ~(1 << io);

	writel(val, base + 0x4c);

	return 0;
}
EXPORT_SYMBOL(ec610_gpio_set_dir);

int ec610_gpio_set_pulldown(int io, int en)
{
	uint8_t val;

	if (io > 5)
		return -EINVAL;

	val = readl(base + 0x54);
	if (en)
		val |= 1<<io;
	else
		val &= ~(1<<io);

	writel(val, base + 0x54);

	return 0;
}
EXPORT_SYMBOL(ec610_gpio_set_pulldown);

int ec610_gpio_irq_polarity(int io, int polar)
{
	uint8_t val;

	if (io > 3)
		return -EINVAL;

	val = readl(base + 0x5c);
	if (polar)
		val |= 1<<(4+io);
	else
		val &= ~(1<<(4+io));

	writel(val, base + 0x5c);

	return 0;
}
EXPORT_SYMBOL(ec610_gpio_irq_polarity);*/
#if 0
static irqreturn_t ec610_irq_handler(int irq, void *data)
{
	struct ec610 *ec610 = data;
	u8 int_status = 0;
	int ret;
	u8 irqmask = INT_MASK_ALL;
	u8 key_pressed = -1;
	u8 key_changed = -1;
	u8 key_shortpress = -1;
	u8 key_longpress = -1;

	printk("PMU: %s: irq=%d\n", __func__, irq);
	/* disable_irq_nosync(irq); */

	/* disable rtc_gp1 irq */
	ec610_gpio_irq_enable(1, 0);
	ec610_gpio_irq_clr(1);

	ret = ec610_read(ec610->dev, EC610_INTS_FLAG0,	&int_status);
	pr_err("%s: int_status=0x%x\n", __func__, int_status);
	if (ret < 0) {
		dev_err(ec610->dev,
			"Error in reading reg 0x%02x, error: %d\n",
			EC610_INTS_FLAG0, ret);
		return IRQ_HANDLED;
	}
	
	/* clear irq flags */
	ec610_write(ec610->dev, EC610_INTS_FLAG0,	irqmask);
	
	key_changed = int_status & 0x01;
	key_pressed = int_status & 0x20;
	key_shortpress = int_status & 0x02;
	key_longpress = int_status & 0x04;
	
	if (key_changed && key_pressed) {
		pr_err("key down\n");
		input_event(ec610->ec610_pwr, EV_KEY, KEY_POWER, 1);
		input_sync(ec610->ec610_pwr);
	} else if (key_changed && !key_pressed) {
		pr_err("key up\n");
		input_event(ec610->ec610_pwr, EV_KEY, KEY_POWER, 0);
		input_sync(ec610->ec610_pwr);
	}
	
	/* enable rtc_gp1 irq */
	ec610_gpio_irq_enable(1, 1);

	return IRQ_HANDLED;
}

static int ec610_irq_init(struct ec610 *ec610, int irq)
{
	int ret;
	
	/* disable rtc gpio1 irq first */
	ec610_gpio_irq_enable(1, 0);
	/* set rtc gpio1 to input */
	ec610_gpio_set_dir(1, 1);
	/* rtc gpio1 pulldown disable */
	ec610_gpio_set_pulldown(1, 0);
	/* low valid */
	ec610_gpio_irq_polarity(1, 0);
	
	ec610->irq = irq;

	ret = request_threaded_irq(ec610->irq, NULL, ec610_irq_handler,
				IRQF_DISABLED | IRQF_ONESHOT, "ec610", ec610);
	if (ret < 0) {
		dev_err(ec610->dev,
			"Error in registering interrupt error: %d\n", ret);
		goto exit;
	}

	device_init_wakeup(ec610->dev, 1);
	enable_irq_wake(ec610->irq);
	
	/* enable rtc gpio1 irq */
	ec610_gpio_irq_enable(1, 1);
exit:	
	return ret;
}

static int ec610_irq_exit(struct ec610 *ec610)
{
	if (ec610->irq)
		free_irq(ec610->irq, ec610);
	return 0;
}
#endif

/*static int ec610_input_init(struct ec610 *ec610)
{
	struct input_dev *input;
	int ret;
	
	input = input_allocate_device();
	if (!input) {
		dev_err(ec610->dev, "Can't allocate power button input device\n");
		return -ENOMEM;
	}
	
	input_set_capability(input, EV_KEY, KEY_POWER);
	input->name = "ec610_pwrkey";
	input->phys = "ec610_pwrkey/input0";
	input->dev.parent = &ec610->client->dev;

	ret = input_register_device(input);
	if (ret) {
		dev_err(ec610->dev, "Can't register power key: %d\n", ret);
		input_free_device(input);
		return -1;
	}
	
	ec610->ec610_pwr = input;
	
	return 0;
}

static int ec610_input_exit(struct ec610 *ec610)
{
	input_unregister_device(ec610->ec610_pwr);
	return 0;
}*/

#ifdef CONFIG_PMIC_DEBUG_IF
static uint8_t ec610_reg_addr;

static ssize_t ec610_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint8_t val;
	ec610_read(&ec610_i2c_client->dev, ec610_reg_addr, &val);
	return sprintf(buf, "REG[0x%x]=0x%x\n", ec610_reg_addr, val);
}

static ssize_t ec610_reg_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
	int tmp;
	uint8_t val;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp < 256) {
		ec610_reg_addr = tmp;
	} else {
		val = tmp & 0x00FF;
		ec610_reg_addr = (tmp >> 8) & 0x00FF;
		ec610_write(&ec610_i2c_client->dev, ec610_reg_addr, val);
	}

	return count;
}

static struct class_attribute ec610_class_attrs[] = {
	__ATTR(reg, 0644, ec610_reg_show, ec610_reg_store),
	__ATTR_NULL
};

static struct class ec610_class = {
	.name = "ec610-debug",
	.class_attrs = ec610_class_attrs,
};
#endif

static int ec610_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	struct ec610 *ec610;
	struct ec610_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	int ret;
	
	EC610_ERR("PMU: %s:\n", __func__);

	if (strcmp(ip->name, "ec610")) {
		pr_err("PMU is not ec610\n");
		return -1;
	}

	ec610 = kzalloc(sizeof(struct ec610), GFP_KERNEL);
	if (ec610 == NULL)
		return -ENOMEM;

	ec610->client = client;
	ec610->dev = &client->dev;
	i2c_set_clientdata(client, ec610);

	mutex_init(&ec610->io_lock);
	
	/* store to global i2c client. */
	ec610_i2c_client = client;
	
	ec610_chip_init(ec610, ip);

	/*ret = ec610_input_init(ec610);
	if (ret) {
		dev_err(ec610->dev, "input regist failed: %d\n", ret);
		goto err_input_init;
	}*/
	
	ret = ec610_add_subdevs(ec610, pdata);
	if (ret) {
		dev_err(ec610->dev, "add sub devices failed: %d\n", ret);
		goto err_add_subdevs;
	}
	
	/*ec610_interrupt_init(ec610);*/

	/*ret = ec610_irq_init(ec610, EC610_PMIC_IRQ);*/

#ifdef CONFIG_PMIC_DEBUG_IF
	class_register(&ec610_class);
#endif
	
	EC610_ERR("PMU: %s done!\n", __func__);

	return 0;

err_add_subdevs:
	/*ec610_input_exit(ec610);*/
/*err_input_init:*/
	/*if (ec610->irq)
		ec610_irq_exit(ec610);*/

	kfree(ec610);
	EC610_ERR("%s: error !\n", __func__);
	return ret;
}

static int ec610_remove(struct i2c_client *client)
{
	struct ec610 *ec610 = i2c_get_clientdata(client);

	/*if (ec610->irq)
		ec610_irq_exit(ec610);*/
	
	/*ec610_input_exit(ec610);*/
	
	ec610_remove_subdevs(ec610);
	
	kfree(ec610);
	return 0;
}

static void ec610_shutdown(struct i2c_client *client)
{
	/*struct ec610 *ec610 = i2c_get_clientdata(client);

	if (ec610->irq)
		ec610_irq_exit(ec610);*/
}

#ifdef CONFIG_PM
static int ec610_suspend(struct i2c_client *client, pm_message_t state)
{
	struct ec610 *ec610 = i2c_get_clientdata(client);
	
	EC610_DBG("PMU: %s:\n", __func__);
	
	if (ec610->irq)
		disable_irq(ec610->irq);
	
	ec610_sleep();
	
	return 0;
}

static int ec610_resume(struct i2c_client *client)
{
	struct ec610 *ec610 = i2c_get_clientdata(client);
	struct ec610_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;

	EC610_DBG("PMU: %s:\n", __func__);

	ec610_chip_init(ec610, ip);

	if (ec610->irq)
		enable_irq(ec610->irq);

	return 0;
}

#endif

static const struct i2c_device_id ec610_i2c_id[] = {
	{"ec610", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, ec610_i2c_id);

static struct i2c_driver ec610_i2c_driver = {
	.driver = {
			.name = "ec610",
			.owner = THIS_MODULE,
	},
	.probe = ec610_probe,
	.remove = ec610_remove,
	.shutdown = ec610_shutdown,
#ifdef CONFIG_PM
	.suspend = ec610_suspend,
	.resume = ec610_resume,
#endif
	.id_table = ec610_i2c_id,
};


static int __init ec610_init(void)
{
	int ret = -ENODEV;
	
	EC610_INFO("PMU: %s:\n", __func__);
	
	ret = i2c_add_driver(&ec610_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ec610_init);

static void __exit ec610_exit(void)
{
	i2c_del_driver(&ec610_i2c_driver);
}
module_exit(ec610_exit);

MODULE_DESCRIPTION("Elitechip EC610 PMU driver");
MODULE_LICENSE("GPL");
