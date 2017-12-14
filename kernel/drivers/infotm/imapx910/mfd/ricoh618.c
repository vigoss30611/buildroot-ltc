/*
 * driver/mfd/ricoh618.c
 *
 * Core driver implementation to access RICOH R5T618 power management chip.
 *
 * Copyright (C) 2012-2013 RICOH COMPANY,LTD
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
/*#define DEBUG			1*/
/*#define VERBOSE_DEBUG		1*/
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/mfd/core.h>
#include <linux/seq_file.h>
#include <mach/ricoh618.h>
#include <mach/hw_cfg.h>


struct sleep_control_data {
	u8 reg_add;
};

static struct sleep_control_data sleep_data[] = {
	[RICOH618_DS_DC1] = {.reg_add = 0x16},
	[RICOH618_DS_DC2] = {.reg_add = 0x17},
	[RICOH618_DS_DC3] = {.reg_add = 0x18},
	[RICOH618_DS_LDO1] = {.reg_add = 0x1B},
	[RICOH618_DS_LDO2] = {.reg_add = 0x1C},
	[RICOH618_DS_LDO3] = {.reg_add = 0x1D},
	[RICOH618_DS_LDO4] = {.reg_add = 0x1E},
	[RICOH618_DS_LDO5] = {.reg_add = 0x1F},
	[RICOH618_DS_PSO0] = {.reg_add = 0x25},
	[RICOH618_DS_PSO1] = {.reg_add = 0x26},
	[RICOH618_DS_PSO2] = {.reg_add = 0x27},
	[RICOH618_DS_PSO3] = {.reg_add = 0x28},
	[RICOH618_DS_LDORTC1] = {.reg_add = 0x2A},
};

static void ricoh61x_set_sleep_slot(struct ricoh61x *ricoh61x)
{
	uint8_t val;

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO2].reg_add, &val);
	val &= 0xf0;
	val |= 0x00;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO2].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO3].reg_add, &val);
	val &= 0xf0;
	val |= 0x00;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO3].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO4].reg_add, &val);
	val &= 0xf0;
	val |= 0x00;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO4].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO5].reg_add, &val);
	val &= 0xf0;
	val |= 0x00;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO5].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO1].reg_add, &val);
	val &= 0xf0;
	val |= 0x01;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_LDO1].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC1].reg_add, &val);
	val &= 0xf0;
	val |= 0x03;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC1].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC2].reg_add, &val);
	val &= 0xf0;
	val |= 0x03;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC2].reg_add, val);

	ricoh61x_read(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC3].reg_add, &val);
	val &= 0xf0;
	val |= 0x04;
	ricoh61x_write(ricoh61x->dev,
		sleep_data[RICOH618_DS_DC3].reg_add, val);
}

static inline int __ricoh61x_read(struct i2c_client *client,
				  u8 reg, uint8_t *val)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading at 0x%02x, ret=%d\n", reg, ret);
		return ret;
	}

	*val = (uint8_t)ret;
	dev_dbg(&client->dev, "ricoh61x: reg read  reg=%x, val=%x\n",
				reg, *val);
	return 0;
}

static inline int __ricoh61x_bulk_reads(struct i2c_client *client, u8 reg,
				int len, uint8_t *val)
{
	int ret;
	int i;

	ret = i2c_smbus_read_i2c_block_data(client, reg, len, val);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading from 0x%02x\n", reg);
		return ret;
	}
	for (i = 0; i < len; ++i) {
		dev_dbg(&client->dev, "ricoh61x: reg read  reg=%x, val=%x\n",
				reg + i, *(val + i));
	}
	return 0;
}

static inline int __ricoh61x_write(struct i2c_client *client,
				 u8 reg, uint8_t val)
{
	int ret;

	dev_dbg(&client->dev, "ricoh61x: reg write  reg=%x, val=%x\n",
				reg, val);
	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret < 0) {
		dev_err(&client->dev, "failed writing 0x%02x to 0x%02x\n",
				val, reg);
		return ret;
	}

	return 0;
}

static inline int __ricoh61x_bulk_writes(struct i2c_client *client, u8 reg,
				  int len, uint8_t *val)
{
	int ret;
	int i;

	for (i = 0; i < len; ++i) {
		dev_dbg(&client->dev, "ricoh61x: reg write  reg=%x, val=%x\n",
				reg + i, *(val + i));
	}

	ret = i2c_smbus_write_i2c_block_data(client, reg, len, val);
	if (ret < 0) {
		dev_err(&client->dev, "failed writings to 0x%02x\n", reg);
		return ret;
	}

	return 0;
}

static inline int set_bank_ricoh61x(struct device *dev, int bank)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret;

	if (bank != (bank & 1))
		return -EINVAL;
	if (bank == ricoh61x->bank_num)
		return 0;
	ret = __ricoh61x_write(to_i2c_client(dev), RICOH61x_REG_BANKSEL, bank);
	if (!ret)
		ricoh61x->bank_num = bank;

	return ret;
}

int ricoh61x_write(struct device *dev, u8 reg, uint8_t val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret)
		ret = __ricoh61x_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_write);

int ricoh61x_write_bank1(struct device *dev, u8 reg, uint8_t val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 1);
	if (!ret)
		ret = __ricoh61x_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_write_bank1);

int ricoh61x_bulk_writes(struct device *dev, u8 reg, u8 len, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret)
		ret = __ricoh61x_bulk_writes(to_i2c_client(dev), reg, len, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_bulk_writes);

int ricoh61x_bulk_writes_bank1(struct device *dev, u8 reg, u8 len, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 1);
	if (!ret)
		ret = __ricoh61x_bulk_writes(to_i2c_client(dev), reg, len, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_bulk_writes_bank1);

int ricoh61x_read(struct device *dev, u8 reg, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret)
		ret = __ricoh61x_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_read);

int ricoh61x_read_bank1(struct device *dev, u8 reg, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 1);
	if (!ret)
		ret =  __ricoh61x_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_read_bank1);

int ricoh61x_bulk_reads(struct device *dev, u8 reg, u8 len, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret)
		ret = __ricoh61x_bulk_reads(to_i2c_client(dev), reg, len, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_bulk_reads);

int ricoh61x_bulk_reads_bank1(struct device *dev, u8 reg, u8 len, uint8_t *val)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 1);
	if (!ret)
		ret = __ricoh61x_bulk_reads(to_i2c_client(dev), reg, len, val);
	mutex_unlock(&ricoh61x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_bulk_reads_bank1);

int ricoh61x_set_bits(struct device *dev, u8 reg, uint8_t bit_mask)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret) {
		ret = __ricoh61x_read(to_i2c_client(dev), reg, &reg_val);
		if (ret)
			goto out;

		if ((reg_val & bit_mask) != bit_mask) {
			reg_val |= bit_mask;
			ret = __ricoh61x_write(to_i2c_client(dev), reg,
								 reg_val);
		}
	}
out:
	mutex_unlock(&ricoh61x->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_set_bits);

int ricoh61x_clr_bits(struct device *dev, u8 reg, uint8_t bit_mask)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret) {
		ret = __ricoh61x_read(to_i2c_client(dev), reg, &reg_val);
		if (ret)
			goto out;

		if (reg_val & bit_mask) {
			reg_val &= ~bit_mask;
			ret = __ricoh61x_write(to_i2c_client(dev), reg,
								 reg_val);
		}
	}
out:
	mutex_unlock(&ricoh61x->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_clr_bits);

int ricoh61x_update(struct device *dev, u8 reg, uint8_t val, uint8_t mask)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 0);
	if (!ret) {
		ret = __ricoh61x_read(ricoh61x->client, reg, &reg_val);
		if (ret)
			goto out;

		if ((reg_val & mask) != val) {
			reg_val = (reg_val & ~mask) | (val & mask);
			ret = __ricoh61x_write(ricoh61x->client, reg, reg_val);
		}
	}
out:
	mutex_unlock(&ricoh61x->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ricoh61x_update);

int ricoh61x_update_bank1(struct device *dev, u8 reg, uint8_t val, uint8_t mask)
{
	struct ricoh61x *ricoh61x = dev_get_drvdata(dev);
	uint8_t reg_val;
	int ret = 0;

	mutex_lock(&ricoh61x->io_lock);
	ret = set_bank_ricoh61x(dev, 1);
	if (!ret) {
		ret = __ricoh61x_read(ricoh61x->client, reg, &reg_val);
		if (ret)
			goto out;

		if ((reg_val & mask) != val) {
			reg_val = (reg_val & ~mask) | (val & mask);
			ret = __ricoh61x_write(ricoh61x->client, reg, reg_val);
		}
	}
out:
	mutex_unlock(&ricoh61x->io_lock);
	return ret;
}

static struct i2c_client *ricoh61x_i2c_client;
int ricoh618_power_off(void)
{
	int ret;
	uint8_t reg_val;
	reg_val = g_soc;
	reg_val &= 0x7f;

	if (!ricoh61x_i2c_client)
		return -EINVAL;

	ret = __ricoh61x_write(ricoh61x_i2c_client, RICOH61x_PSWR, reg_val);
	if (ret < 0)
		dev_err(&ricoh61x_i2c_client->dev,
					"Error in writing PSWR_REG\n");

	if (g_fg_on_mode == 0) {
		/* Clear RICOH61x_FG_CTRL 0x01 bit */
		ret = __ricoh61x_read(ricoh61x_i2c_client,
				      RICOH61x_FG_CTRL, &reg_val);
		if (reg_val & 0x01) {
			reg_val &= ~0x01;
			ret = __ricoh61x_write(ricoh61x_i2c_client,
					       RICOH61x_FG_CTRL, reg_val);
		}
		if (ret < 0)
			dev_err(&ricoh61x_i2c_client->dev,
					"Error in writing FG_CTRL\n");
	}

	/* set rapid timer 300 min */
	ret = __ricoh61x_read(ricoh61x_i2c_client,
					TIMSET_REG, &reg_val);

	reg_val |= 0x03;

	ret = __ricoh61x_write(ricoh61x_i2c_client,
					TIMSET_REG, reg_val);
	if (ret < 0)
		dev_err(&ricoh61x_i2c_client->dev,
				"Error in writing the TIMSET_Reg\n");

	/* Disable all Interrupt */
	__ricoh61x_write(ricoh61x_i2c_client, RICOH61x_INTC_INTEN, 0);

	/* Not repeat power ON after power off(Power Off/N_OE) */
	__ricoh61x_write(ricoh61x_i2c_client, RICOH61x_PWR_REP_CNT, 0x0);

	/* Power OFF */
	__ricoh61x_write(ricoh61x_i2c_client, RICOH61x_PWR_SLP_CNT, 0x1);

	return 0;
}
EXPORT_SYMBOL(ricoh618_power_off);

int ricoh61x_set_repwron(int val)
{
	uint8_t tmp = 0;

	if (val) {
		__ricoh61x_read(ricoh61x_i2c_client,
					RICOH61x_PWR_REP_CNT, &tmp);
		tmp |= 0x1;
		__ricoh61x_write(ricoh61x_i2c_client,
					RICOH61x_PWR_REP_CNT, tmp);
	} else {
		__ricoh61x_read(ricoh61x_i2c_client,
					RICOH61x_PWR_REP_CNT, &tmp);
		tmp &= ~(0x1);
		__ricoh61x_write(ricoh61x_i2c_client,
					RICOH61x_PWR_REP_CNT, tmp);
	}

	return 0;
}
EXPORT_SYMBOL(ricoh61x_set_repwron);

static int ricoh61x_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	struct ricoh61x *ricoh61x =
			container_of(gc, struct ricoh61x, gpio_chip);
	uint8_t val;
	int ret;

	ret = ricoh61x_read(ricoh61x->dev, RICOH61x_GPIO_MON_IOIN, &val);
	if (ret < 0)
		return ret;

	return ((val & (0x1 << offset)) != 0);
}

static void ricoh61x_gpio_set(struct gpio_chip *gc,
				unsigned offset, int value)
{
	struct ricoh61x *ricoh61x =
			container_of(gc, struct ricoh61x, gpio_chip);
	if (value)
		ricoh61x_set_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOOUT, 1 << offset);
	else
		ricoh61x_clr_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOOUT, 1 << offset);
}

static int ricoh61x_gpio_input(struct gpio_chip *gc, unsigned offset)
{
	struct ricoh61x *ricoh61x =
			container_of(gc, struct ricoh61x, gpio_chip);

	return ricoh61x_clr_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOSEL, 1 << offset);
}

static int ricoh61x_gpio_output(struct gpio_chip *gc,
				unsigned offset, int value)
{
	struct ricoh61x *ricoh61x =
			container_of(gc, struct ricoh61x, gpio_chip);

	ricoh61x_gpio_set(gc, offset, value);
	return ricoh61x_set_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOSEL, 1 << offset);
}

static int ricoh61x_gpio_to_irq(struct gpio_chip *gc, unsigned off)
{
	struct ricoh61x *ricoh61x =
			container_of(gc, struct ricoh61x, gpio_chip);

	if ((off >= 0) && (off < 8))
		return ricoh61x->irq_base + RICOH61x_IRQ_GPIO0 + off;

	return -EIO;
}

static void ricoh61x_gpio_init(struct ricoh61x *ricoh61x,
				struct ricoh618_platform_data *pdata)
{
	int ret;
	int i;
	struct ricoh618_gpio_init_data *ginit;

	if (pdata->gpio_base  <= 0)
		return;

	for (i = 0; i < pdata->num_gpioinit_data; ++i) {
		ginit = &pdata->gpio_init_data[i];

		if (!ginit->init_apply)
			continue;

		if (ginit->output_mode_en) {
			/* GPIO output mode */
			if (ginit->output_val)
				/* output H */
				ret = ricoh61x_set_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOOUT, 1 << i);
			else
				/* output L */
				ret = ricoh61x_clr_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOOUT, 1 << i);
			if (!ret)
				ret = ricoh61x_set_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOSEL, 1 << i);
		} else
			/* GPIO input mode */
			ret = ricoh61x_clr_bits(ricoh61x->dev,
					RICOH61x_GPIO_IOSEL, 1 << i);

		/* if LED function enabled in OTP */
		if (ginit->led_mode) {
			/* LED Mode 1 */
			if (i == 0)	/* GP0 */
				ret = ricoh61x_set_bits(ricoh61x->dev,
					 RICOH61x_GPIO_LED_FUNC,
					 0x04 | (ginit->led_func & 0x03));
			if (i == 1)	/* GP1 */
				ret = ricoh61x_set_bits(ricoh61x->dev,
					 RICOH61x_GPIO_LED_FUNC,
					 0x40 | (ginit->led_func & 0x03) << 4);

		}


		if (ret < 0)
			dev_err(ricoh61x->dev,
				"Gpio %d init dir configuration failed: %d\n",
				i, ret);

	}

	ricoh61x->gpio_chip.owner		= THIS_MODULE;
	ricoh61x->gpio_chip.label		= ricoh61x->client->name;
	ricoh61x->gpio_chip.dev			= ricoh61x->dev;
	ricoh61x->gpio_chip.base		= pdata->gpio_base;
	ricoh61x->gpio_chip.ngpio		= RICOH61x_NR_GPIO;
	ricoh61x->gpio_chip.can_sleep	= 1;

	ricoh61x->gpio_chip.direction_input	= ricoh61x_gpio_input;
	ricoh61x->gpio_chip.direction_output	= ricoh61x_gpio_output;
	ricoh61x->gpio_chip.set			= ricoh61x_gpio_set;
	ricoh61x->gpio_chip.get			= ricoh61x_gpio_get;
	ricoh61x->gpio_chip.to_irq		= ricoh61x_gpio_to_irq;

	ret = gpiochip_add(&ricoh61x->gpio_chip);
	if (ret)
		dev_warn(ricoh61x->dev, "GPIO registration failed: %d\n", ret);
}

static int ricoh61x_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ricoh61x_remove_subdevs(struct ricoh61x *ricoh61x)
{
	return device_for_each_child(ricoh61x->dev, NULL,
				     ricoh61x_remove_subdev);
}

static int ricoh61x_add_subdevs(struct ricoh61x *ricoh61x,
				struct ricoh618_platform_data *pdata)
{
	struct ricoh618_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ricoh61x->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}
	return 0;

failed:
	ricoh61x_remove_subdevs(ricoh61x);
	return ret;
}

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
static void print_regs(const char *header, struct seq_file *s,
		struct i2c_client *client, int start_offset,
		int end_offset)
{
	uint8_t reg_val;
	int i;
	int ret;

	seq_printf(s, "%s\n", header);
	for (i = start_offset; i <= end_offset; ++i) {
		ret = __ricoh61x_read(client, i, &reg_val);
		if (ret >= 0)
			seq_printf(s, "Reg 0x%02x Value 0x%02x\n", i, reg_val);
	}
	seq_puts(s, "------------------\n");
}

static int dbg_ricoh_show(struct seq_file *s, void *unused)
{
	struct ricoh61x *ricoh = s->private;
	struct i2c_client *client = ricoh->client;

	seq_puts(s, "RICOH61x Registers\n");
	seq_puts(s, "------------------\n");

	print_regs("System Regs",		s, client, 0x0, 0x05);
	print_regs("Power Control Regs",	s, client, 0x07, 0x2B);
	print_regs("DCDC  Regs",		s, client, 0x2C, 0x43);
	print_regs("LDO   Regs",		s, client, 0x44, 0x5C);
	print_regs("ADC   Regs",		s, client, 0x64, 0x8F);
	print_regs("GPIO  Regs",		s, client, 0x90, 0x9B);
	print_regs("INTC  Regs",		s, client, 0x9C, 0x9E);
	print_regs("OPT   Regs",		s, client, 0xB0, 0xB1);
	print_regs("CHG   Regs",		s, client, 0xB2, 0xDF);
	print_regs("FUEL  Regs",		s, client, 0xE0, 0xFC);
	return 0;
}

static int dbg_ricoh_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_ricoh_show, inode->i_private);
}

static const struct file_operations debug_fops = {
	.open		= dbg_ricoh_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
static void __init ricoh61x_debuginit(struct ricoh61x *ricoh)
{
	(void)debugfs_create_file("ricoh61x",
		S_IRUGO, NULL, ricoh, &debug_fops);
}
#else
static void print_regs(const char *header, struct i2c_client *client,
		int start_offset, int end_offset)
{
	uint8_t reg_val;
	int i;
	int ret;
	struct seq_file *s;

	seq_printf(s, "%s\n", header);
	for (i = start_offset; i <= end_offset; ++i) {
		ret = __ricoh61x_read(client, i, &reg_val);
		if (ret >= 0)
			dev_info(&client->dev,
				"Reg 0x%02x Value 0x%02x\n", i, reg_val);
	}
	seq_puts(s, "------------------\n");
}
static void __init ricoh61x_debuginit(struct ricoh61x *ricoh)
{
	struct i2c_client *client = ricoh->client;
	struct seq_file *s;

	seq_puts(s, "RICOH61x Registers\n");
	seq_puts(s, "------------------\n");

	print_regs("System Regs",		client, 0x0, 0x05);
	print_regs("Power Control Regs",	client, 0x07, 0x2B);
	print_regs("DCDC  Regs",		client, 0x2C, 0x43);
	print_regs("LDO   Regs",		client, 0x44, 0x5C);
	print_regs("ADC   Regs",		client, 0x64, 0x8F);
	print_regs("GPIO  Regs",		client, 0x90, 0x9B);
	print_regs("INTC  Regs",		client, 0x9C, 0x9E);
	print_regs("OPT   Regs",		client, 0xB0, 0xB1);
	print_regs("CHG   Regs",		client, 0xB2, 0xDF);
	print_regs("FUEL  Regs",		client, 0xE0, 0xFC);

	return;
}
#endif

static void ricoh61x_noe_init(struct ricoh61x *ricoh)
{
	struct i2c_client *client = ricoh->client;

	/* N_OE timer setting to 128mS */
	__ricoh61x_write(client, RICOH61x_PWR_NOE_TIMSET, 0x0);
	/* Keep REPWRON off in default */
	__ricoh61x_write(client, RICOH61x_PWR_REP_CNT, 0x0);
}

static int ricoh61x_i2c_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	struct ricoh61x *ricoh61x;
	struct ricoh618_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	int ret;
	RICOH_ERR("PMU: %s:\n", __func__);

	if (strcmp(ip->name, "ricoh618")) {
		pr_err("PMU is not ricoh618\n");
		return -1;
	}

	ricoh61x = kzalloc(sizeof(struct ricoh61x), GFP_KERNEL);
	if (ricoh61x == NULL)
		return -ENOMEM;

	ricoh61x->client = client;
	ricoh61x->dev = &client->dev;
	i2c_set_clientdata(client, ricoh61x);

	mutex_init(&ricoh61x->io_lock);

	ricoh61x->bank_num = 0;

	/* For init PMIC_IRQ port */
	/* ret = pdata->init_port(client->irq); */

	if (client->irq) {
		ret = ricoh61x_irq_init(ricoh61x, client->irq, pdata->irq_base);
		if (ret) {
			dev_err(&client->dev, "IRQ init failed: %d\n", ret);
			goto err_irq_init;
		}
	}

	ret = ricoh61x_add_subdevs(ricoh61x, pdata);
	if (ret) {
		dev_err(&client->dev, "add devices failed: %d\n", ret);
		goto err_add_devs;
	}

	ricoh61x_noe_init(ricoh61x);

	ricoh61x_gpio_init(ricoh61x, pdata);

	ricoh61x_set_sleep_slot(ricoh61x);

#if (RICOH_DEBUG_FLAG)
	ricoh61x_debuginit(ricoh61x);
#endif

	ricoh61x_i2c_client = client;

    ricoh61x_write(ricoh61x->dev, 0x37, 0x24);

	return 0;

err_add_devs:
	if (client->irq)
		ricoh61x_irq_exit(ricoh61x);
err_irq_init:
	kfree(ricoh61x);
	RICOH_ERR("%s: error !\n", __func__);
	return ret;
}

static int ricoh61x_i2c_remove(struct i2c_client *client)
{
	struct ricoh61x *ricoh61x = i2c_get_clientdata(client);

	if (client->irq)
		ricoh61x_irq_exit(ricoh61x);

	ricoh61x_remove_subdevs(ricoh61x);
	kfree(ricoh61x);
	return 0;
}

static void ricoh61x_i2c_shutdown(struct i2c_client *client)
{
	struct ricoh61x *ricoh61x = i2c_get_clientdata(client);

	if (client->irq)
		ricoh61x_irq_exit(ricoh61x);
}

#ifdef CONFIG_PM
static int ricoh61x_i2c_suspend(struct i2c_client *client, pm_message_t state)
{
	RICOH_DBG("PMU: %s:\n", __func__);
	if (client->irq)
		disable_irq(client->irq);
	return 0;
}

int pwrkey_wakeup = 0;
int charger_wakeup = 0;
static int ricoh61x_i2c_resume(struct i2c_client *client)
{
	uint8_t reg_val = 0;
	int ret;

	RICOH_DBG("PMU: %s:\n", __func__);

	/* Disable all Interrupt */
	__ricoh61x_write(client, RICOH61x_INTC_INTEN, 0x0);

	ret = __ricoh61x_read(client, RICOH61x_INT_IR_SYS, &reg_val);
	if (reg_val & 0x01) { /* If PWR_KEY wakeup */
		RICOH_DBG("PMU: %s: PWR_KEY Wakeup\n", __func__);
		pwrkey_wakeup = 1;
		/* Clear PWR_KEY IRQ */
		__ricoh61x_write(client, RICOH61x_INT_IR_SYS, 0x0);
	} else {
		ret = __ricoh61x_read(client, RICOH61x_INT_IR_CHGCTR, &reg_val);
		if (reg_val & 0x03) {
			RICOH_DBG("%s: CHARGER Wakeup\n", __func__);
			charger_wakeup = 1;
			/* Clear ADPDET/USBDET IRQ */
			__ricoh61x_write(client, RICOH61x_INT_IR_CHGCTR, 0x0);
		}
	}
	enable_irq(client->irq);

	/* Enable all Interrupt */
	__ricoh61x_write(client, RICOH61x_INTC_INTEN, 0xff);

	return 0;
}

#endif

static const struct i2c_device_id ricoh61x_i2c_id[] = {
	{"ricoh618", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, ricoh61x_i2c_id);

static struct i2c_driver ricoh61x_i2c_driver = {
	.driver = {
		   .name = "ricoh618",
		   .owner = THIS_MODULE,
		   },
	.probe = ricoh61x_i2c_probe,
	.remove = ricoh61x_i2c_remove,
	.shutdown = ricoh61x_i2c_shutdown,
#ifdef CONFIG_PM
	.suspend = ricoh61x_i2c_suspend,
	.resume = ricoh61x_i2c_resume,
#endif
	.id_table = ricoh61x_i2c_id,
};


static int __init ricoh61x_i2c_init(void)
{
	int ret = -ENODEV;

	RICOH_INFO("PMU: %s:\n", __func__);

	ret = i2c_add_driver(&ricoh61x_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ricoh61x_i2c_init);

static void __exit ricoh61x_i2c_exit(void)
{
	i2c_del_driver(&ricoh61x_i2c_driver);
}

module_exit(ricoh61x_i2c_exit);

MODULE_DESCRIPTION("RICOH R5T618 PMU multi-function core driver");
MODULE_LICENSE("GPL");
