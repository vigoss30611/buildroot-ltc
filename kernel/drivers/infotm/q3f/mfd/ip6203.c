/*
 * driver/mfd/ip6203.c
 *
 * Core driver implementation to access Elitechip IP6203 power management chip.
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
#include <mach/ip6203.h>
#include <mach/hw_cfg.h>
#include <mach/pad.h>

/*16bit pattern i2c read write L & H is changed, buf[0] is H, buf[1] is L, so need change H & L*/
static void ch_hl(uint16_t *val)
{
	uint16_t tmp = *val;
	tmp = ((*val >> 8) | ((*val & 0xFF) << 8));
	*val = tmp;
}

struct ip6203_reg_val {
	u8   reg_address;
	u16  reg_value;
};

static struct ip6203_reg_val init_data[] = {	
	//{0x00, 0x14c1},
	/*x2E, 0xFC},
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
	{0x4F, 0x1C},

	{0x43, 0x1C},
	{0x45, 0x1C},
	{0x47, 0x1C},
	{0x49, 0x1C},
	{0x4b, 0x1c},
	{0x4d, 0x1c},
	{0x4F, 0x1C},
	{0x44, 0x68},*/
	{0x00, 0xc310},
	{0x3a, 0x2C},	//ldo6
	{0x3b, 0x48},	//ldo7
	{0x3c, 0x48},	//ldo8
	{0x3d, 0x2C},	//ldo9
};

struct i2c_client *ip6203_i2c_client = NULL;

static int ip6203_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ip6203_remove_subdevs(struct ip6203 *ip6203)
{
	return device_for_each_child(ip6203->dev, NULL,
				     ip6203_remove_subdev);
}

static int ip6203_add_subdevs(struct ip6203 *ip6203,
				struct ip6203_platform_data *pdata)
{
	struct ip6203_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ip6203->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}
	return 0;

failed:
	ip6203_remove_subdevs(ip6203);
	return ret;
}

static inline int ip6203_i2c_write(struct i2c_client *client,
					uint8_t reg, uint16_t val)
{
	int ret;

	ch_hl(&val);
	ret = i2c_smbus_write_word_data(client, reg, val);
	if (ret < 0) {
		dev_err(&client->dev, "failed writing 0x%02x to 0x%02x, ret=%d\n",
				val, reg, ret);
		return ret;
	}

	return 0;
}

static inline int ip6203_i2c_read(struct i2c_client *client,
					uint8_t reg, uint16_t *val)
{
	int ret;
	ret = i2c_smbus_read_word_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading at 0x%02x, ret=%d\n",
				reg, ret);
		return ret;
	}
	ch_hl(&ret);
	*val = (uint16_t)ret;

	return 0;
}

int ip6203_write(struct device *dev, u8 reg, uint16_t val)
{
	struct ip6203 *ip6203 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6203->io_lock);
	ret = ip6203_i2c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6203->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6203_write);

int ip6203_read(struct device *dev, u8 reg, uint16_t *val)
{
	struct ip6203 *ip6203 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6203->io_lock);
	ret = ip6203_i2c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6203->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6203_read);

int ip6203_set_bits(struct device *dev, u8 reg, uint16_t bit_mask)
{
	struct ip6203 *ip6203 = dev_get_drvdata(dev);
	uint16_t reg_val;
	int ret = 0;

	mutex_lock(&ip6203->io_lock);
	ret = ip6203_i2c_read(to_i2c_client(dev), reg, &reg_val);
	if (ret)
		goto out;

	if ((reg_val & bit_mask) != bit_mask) {
		reg_val |= bit_mask;
		ret = ip6203_i2c_write(to_i2c_client(dev), reg,	reg_val);
	}

out:
	mutex_unlock(&ip6203->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6203_set_bits);

int ip6203_clr_bits(struct device *dev, u8 reg, uint16_t bit_mask)
{
	struct ip6203 *ip6203 = dev_get_drvdata(dev);
	uint16_t reg_val;
	int ret = 0;

	mutex_lock(&ip6203->io_lock);
	ret = ip6203_i2c_read(to_i2c_client(dev), reg, &reg_val);
	if (ret)
		goto out;

	if (reg_val & bit_mask) {
		reg_val &= ~bit_mask;
		ret = ip6203_i2c_write(to_i2c_client(dev), reg, reg_val);
	}

out:
	mutex_unlock(&ip6203->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6203_clr_bits);

void ip6203_chip_init(struct ip6203 *ip6203, struct pmu_cfg *ip)
{
	u16 i = 0;
	u16 value = 0;

	/*set DCDC2 voltage(core):default voltage is 1.05v*/
	if(ip->core_voltage == -1)
		value = 0x24;
	else if(ip->core_voltage >= 1050)
		value = ((ip->core_voltage * 10) - 6000)/125;
	else if(ip->core_voltage < 1050){
		printk("The core voltage must be bigger than 1.05V\n");
		return;
	}
	ip6203_write(ip6203->dev, IP6203_DC2_VSET, value);
	
	ip6203_read(ip6203->dev, IP6203_PWRON_REC, &value);
#if 0
	if (value & 0x1)
		wake_flag = 1;
	else
		wake_flag = 0;
#endif
	ip6203_read(ip6203->dev, IP6203_PSTATE_CTL, &value);
	ip6203_clr_bits(ip6203->dev, IP6203_PSTATE_CTL, 1<<7);
	ip6203_read(ip6203->dev, IP6203_PSTATE_CTL, &value);
//	ip6203_read(ip6203->dev, 0x07,	&value);
//	printk("ip6203:0x07: 0x%x\n", value);
//	ip6203_read(ip6203->dev, 0x08,	&value);
//	printk("ip6203:0x08: 0x%x\n", value);
#if 0
	IP6203_ERR("new IP6203 regs read: \n");
	for (i = 0; i <= 0xc0; i++) {
		ip6203_read(ip6203->dev, i, &value);
		IP6203_ERR("[0x%x, 0x%x]\n", i, value);
	}
#endif
	
	IP6203_ERR("IP6203 regs write: \n");
	for (i = 0; i < (sizeof(init_data)/sizeof(struct ip6203_reg_val)); i++) {
		IP6203_DBG("[0x%x, 0x%x]\n", init_data[i].reg_address, init_data[i].reg_value);
		ip6203_write(ip6203->dev, init_data[i].reg_address, init_data[i].reg_value);
	}
	
	IP6203_ERR("IP6203 regs read: \n");
	for (i = 0; i < (sizeof(init_data)/sizeof(struct ip6203_reg_val)); i++) {
		ip6203_read(ip6203->dev, init_data[i].reg_address, &value);
		IP6203_DBG("[0x%x, 0x%x]\n", init_data[i].reg_address, value);
	}
	return;
}

/*void ip6203_interrupt_init(struct ip6203 *ip6203)
{
	u8 value=0;
	
	// irq output enable.
	ip6203_read(ip6203->dev, IP6203_MFP_CTL5, &value);
	value &= 0xE0;
	value |= IP6203_IRQ_OUT|0x10;
	printk("IP6203_MFP_CTL5: value = 0x%x\n", value);
	ip6203_write(ip6203->dev, IP6203_MFP_CTL5, value);
	
	// clear flags.
	value = 0xff;
	ip6203_write(ip6203->dev, IP6203_INTS_FLAG0, value);
	ip6203_write(ip6203->dev, IP6203_INTS_FLAG1, value);
	
	// enable irq.
	value = DEFAULT_INT_MASK;
	printk("IP6203_INTS_CTL0: value = 0x%x\n", value);
	ip6203_write(ip6203->dev, IP6203_INTS_CTL0, value);
	value = 0x00;
	ip6203_write(ip6203->dev, IP6203_INTS_CTL1, value); 
}*/

void ip6203_sleep(void)
{
	u16 value=0;
#if 1	
	// set wakeup sources.
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	value |= (IP6203_WK_EN_ONOFF_S | IP6203_WK_EN_ONOFF_L | IP6203_ONOFF_SL_RST);
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);
	
	// which power must be keep when sleep.
	// state dc4 need keep voltage
	value = IP6203_SL_DC4_PWR;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_POFF_DCDC, value);
#if 0	
	ip6203_i2c_read(ip6203_i2c_client, IP6203_POFF_DCDC, &value);
	
	ip6203_i2c_read(ip6203_i2c_client, 0x0, &value);
	ip6203_i2c_read(ip6203_i2c_client, IP6203_INTS_CTL, &value);
	ip6203_i2c_read(ip6203_i2c_client, IP6203_INT_FLAG, &value);
	ip6203_i2c_read(ip6203_i2c_client, 0xa2, &value);
	ip6203_i2c_read(ip6203_i2c_client, 0x16, &value);
	ip6203_i2c_read(ip6203_i2c_client, 0x17, &value);
	
	ip6203_read(ip6203_i2c_client, 0xa0, &value);
	printk("======%s[%d], value 0x%x\n", __func__, __LINE__, value);
	ip6203_read(ip6203_i2c_client, 0xa5, &value);
	printk("======%s[%d], value 0x%x\n", __func__, __LINE__, value);
	ip6203_read(ip6203_i2c_client, 0xa2, &value);
	printk("======%s[%d], value 0x%x\n", __func__, __LINE__, value);
	// set to sleep mode.
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	printk("====%s[%d], read 0x%x\n", __func__, __LINE__, value);
	value |= IP6203_DEEP_SL_CTL;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);
	printk("====%s[%d], read 0x%x\n", __func__, __LINE__, value);
#endif
#endif
//	// disable irq.
//	ip6203_i2c_read(ip6203_i2c_client, IP6203_MFP_CTL5, &value);
//	value &=~IP6203_IRQ_CTL;
//	ip6203_i2c_write(ip6203_i2c_client, IP6203_MFP_CTL5, value);
	
//	// goto sleep.
//	ip6203_i2c_read(ip6203_i2c_client, IP6203_STATUS_CTL, &value);
//	value &= ~IP6203_SLEEP_EN_n;
//	ip6203_i2c_write(ip6203_i2c_client, IP6203_STATUS_CTL, &value);
}

void ip6203_deep_sleep(void)
{
	u16 value=0;
#if 1
	// set wakeup sources.
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	value |= (IP6203_WK_EN_ONOFF_S | IP6203_WK_EN_ONOFF_L | IP6203_ONOFF_SL_RST);
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);
	
	// which power must be keep when sleep.
	/*ip6203_i2c_read(ip6203_i2c_client, IP6203_SL_PWR_CTL0, &value);
	value &= (~0x7F);
	value |= IP6203_SL_OFF_SCAL_2MS |	IP6203_SL_OFF_SCH;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_SL_PWR_CTL0, value);*/
	
	// set to deep sleep mode.
	value = 0x00;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_POFF_DCDC, value);
	ip6203_i2c_write(ip6203_i2c_client, IP6203_POFF_LDO, value);
	
	// disable irq.
#if 0
	ip6203_i2c_read(ip6203_i2c_client, IP6203_MFP_CTL5, &value);
	value &=~IP6203_IRQ_CTL;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_MFP_CTL5, value);
#endif
	// goto deep sleep.
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	value |= (IP6203_DEEP_SL_CTL | IP6203_DC_EN);	//DC_EN control adapter & powerdown
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);
#endif
}
EXPORT_SYMBOL(ip6203_deep_sleep);

void ip6203_reset(void)
{
	u16 value=0;
	
	// set wakeup sources.
	/*ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	value |= (IP6203_WK_EN_ONOFF_S | IP6203_WK_EN_ONOFF_L);
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);*/
	
	// which power must be keep when sleep.
	/*ip6203_i2c_read(ip6203_i2c_client, IP6203_SL_PWR_CTL0, &value);
	value &= (~0x7F);
	value |= IP6203_SL_OFF_SCAL_2MS |	IP6203_SL_OFF_SCH;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_SL_PWR_CTL0, value);*/
	
	// set to deep sleep mode.
#if 1
	value = 0x00;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_POFF_DCDC, value);
	ip6203_i2c_write(ip6203_i2c_client, IP6203_POFF_LDO, value);
	
	// disable irq.
#if 0
	ip6203_i2c_read(ip6203_i2c_client, IP6203_MFP_CTL5, &value);
	printk("====%s[%d], read 0x%x\n", __func__, __LINE__, value);
	value &=~IP6203_IRQ_CTL;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_MFP_CTL5, value);
	printk("====%s[%d], read 0x%x\n", __func__, __LINE__, value);
#endif
	// now reset. effect after 2 seconds.
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
	value |= 0x200;
	ip6203_i2c_write(ip6203_i2c_client, IP6203_PSTATE_CTL, value);
	ip6203_i2c_read(ip6203_i2c_client, IP6203_PSTATE_CTL, &value);
#endif
	ip6203_i2c_write(ip6203_i2c_client, IP6203_WDOG_CTL, 0x8);
}
EXPORT_SYMBOL(ip6203_reset);

/*
	IRQ functions 
*/
/*#define base    IO_ADDRESS(SYSMGR_RTC_BASE)
int ip6203_gpio_irq_enable(int io, int en)
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
EXPORT_SYMBOL(ip6203_gpio_irq_enable);

int ip6203_gpio_irq_clr(int io)
{
	uint8_t val;

	if (io > 3)
		return -EINVAL;

	val = readl(base + 0x8);
	if (val & (1 << (io+3)))
		writel((1 << (io+3)), base + 0x4);

	return 0;
}
EXPORT_SYMBOL(ip6203_gpio_irq_clr);

int ip6203_gpio_set_dir(int io, int dir)
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
EXPORT_SYMBOL(ip6203_gpio_set_dir);

int ip6203_gpio_set_pulldown(int io, int en)
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
EXPORT_SYMBOL(ip6203_gpio_set_pulldown);

int ip6203_gpio_irq_polarity(int io, int polar)
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
EXPORT_SYMBOL(ip6203_gpio_irq_polarity);*/
#if 0
static irqreturn_t ip6203_irq_handler(int irq, void *data)
{
	struct ip6203 *ip6203 = data;
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
	ip6203_gpio_irq_enable(1, 0);
	ip6203_gpio_irq_clr(1);

	ret = ip6203_read(ip6203->dev, IP6203_INTS_FLAG0,	&int_status);
	pr_err("%s: int_status=0x%x\n", __func__, int_status);
	if (ret < 0) {
		dev_err(ip6203->dev,
			"Error in reading reg 0x%02x, error: %d\n",
			IP6203_INTS_FLAG0, ret);
		return IRQ_HANDLED;
	}
	
	/* clear irq flags */
	ip6203_write(ip6203->dev, IP6203_INTS_FLAG0,	irqmask);
	
	key_changed = int_status & 0x01;
	key_pressed = int_status & 0x20;
	key_shortpress = int_status & 0x02;
	key_longpress = int_status & 0x04;
	
	if (key_changed && key_pressed) {
		pr_err("key down\n");
		input_event(ip6203->ip6203_pwr, EV_KEY, KEY_POWER, 1);
		input_sync(ip6203->ip6203_pwr);
	} else if (key_changed && !key_pressed) {
		pr_err("key up\n");
		input_event(ip6203->ip6203_pwr, EV_KEY, KEY_POWER, 0);
		input_sync(ip6203->ip6203_pwr);
	}
	
	/* enable rtc_gp1 irq */
	ip6203_gpio_irq_enable(1, 1);

	return IRQ_HANDLED;
}

static int ip6203_irq_init(struct ip6203 *ip6203, int irq)
{
	int ret;
	
	/* disable rtc gpio1 irq first */
	ip6203_gpio_irq_enable(1, 0);
	/* set rtc gpio1 to input */
	ip6203_gpio_set_dir(1, 1);
	/* rtc gpio1 pulldown disable */
	ip6203_gpio_set_pulldown(1, 0);
	/* low valid */
	ip6203_gpio_irq_polarity(1, 0);
	
	ip6203->irq = irq;

	ret = request_threaded_irq(ip6203->irq, NULL, ip6203_irq_handler,
				IRQF_DISABLED | IRQF_ONESHOT, "ip6203", ip6203);
	if (ret < 0) {
		dev_err(ip6203->dev,
			"Error in registering interrupt error: %d\n", ret);
		goto exit;
	}

	device_init_wakeup(ip6203->dev, 1);
	enable_irq_wake(ip6203->irq);
	
	/* enable rtc gpio1 irq */
	ip6203_gpio_irq_enable(1, 1);
exit:	
	return ret;
}

static int ip6203_irq_exit(struct ip6203 *ip6203)
{
	if (ip6203->irq)
		free_irq(ip6203->irq, ip6203);
	return 0;
}
#endif

/*static int ip6203_input_init(struct ip6203 *ip6203)
{
	struct input_dev *input;
	int ret;
	
	input = input_allocate_device();
	if (!input) {
		dev_err(ip6203->dev, "Can't allocate power button input device\n");
		return -ENOMEM;
	}
	
	input_set_capability(input, EV_KEY, KEY_POWER);
	input->name = "ip6203_pwrkey";
	input->phys = "ip6203_pwrkey/input0";
	input->dev.parent = &ip6203->client->dev;

	ret = input_register_device(input);
	if (ret) {
		dev_err(ip6203->dev, "Can't register power key: %d\n", ret);
		input_free_device(input);
		return -1;
	}
	
	ip6203->ip6203_pwr = input;
	
	return 0;
}

static int ip6203_input_exit(struct ip6203 *ip6203)
{
	input_unregister_device(ip6203->ip6203_pwr);
	return 0;
}*/

#ifdef CONFIG_PMIC_DEBUG_IF
static uint8_t ip6203_reg_addr;

static ssize_t ip6203_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint16_t val;
	ip6203_read(&ip6203_i2c_client->dev, ip6203_reg_addr, &val);
	return sprintf(buf, "REG[0x%x]=0x%x\n", ip6203_reg_addr, val);
}

static ssize_t ip6203_reg_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t count)
{
	int tmp;
	uint16_t val;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp < 256) {
		ip6203_reg_addr = tmp;
	} else {
		val = tmp & 0x00FF;
		ip6203_reg_addr = (tmp >> 8) & 0x00FF;
		ip6203_write(&ip6203_i2c_client->dev, ip6203_reg_addr, val);
	}

	return count;
}

static struct class_attribute ip6203_class_attrs[] = {
	__ATTR(reg, 0644, ip6203_reg_show, ip6203_reg_store),
	__ATTR_NULL
};

static struct class ip6203_class = {
	.name = "pmu-debug",
	.class_attrs = ip6203_class_attrs,
};
#endif

static int ip6203_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	struct ip6203 *ip6203;
	struct ip6203_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	int ret;
	
	IP6203_ERR("PMU: %s:\n", __func__);

	if (strcmp(ip->name, "ip6203")) {
		pr_err("PMU is not ip6203, ip->name %s\n", ip->name);
		return -1;
	}

	ip6203 = kzalloc(sizeof(struct ip6203), GFP_KERNEL);
	if (ip6203 == NULL)
		return -ENOMEM;

	ip6203->client = client;
	ip6203->dev = &client->dev;
	i2c_set_clientdata(client, ip6203);

	mutex_init(&ip6203->io_lock);
	
	/* store to global i2c client. */
	ip6203_i2c_client = client;
	
	ip6203_chip_init(ip6203, ip);

	/*ret = ip6203_input_init(ip6203);
	if (ret) {
		dev_err(ip6203->dev, "input regist failed: %d\n", ret);
		goto err_input_init;
	}*/
	
	ret = ip6203_add_subdevs(ip6203, pdata);
	if (ret) {
		dev_err(ip6203->dev, "add sub devices failed: %d\n", ret);
		goto err_add_subdevs;
	}
	
	/*ip6203_interrupt_init(ip6203);*/

	/*ret = ip6203_irq_init(ip6203, IP6203_PMIC_IRQ);*/

#ifdef CONFIG_PMIC_DEBUG_IF
	class_register(&ip6203_class);
#endif
	
	IP6203_ERR("PMU: %s done!\n", __func__);

	return 0;

err_add_subdevs:
	/*ip6203_input_exit(ip6203);*/
/*err_input_init:*/
	/*if (ip6203->irq)
		ip6203_irq_exit(ip6203);*/

	kfree(ip6203);
	IP6203_ERR("%s: error !\n", __func__);
	return ret;
}

static int ip6203_remove(struct i2c_client *client)
{
	struct ip6203 *ip6203 = i2c_get_clientdata(client);

	/*if (ip6203->irq)
		ip6203_irq_exit(ip6203);*/
	
	/*ip6203_input_exit(ip6203);*/
	
	ip6203_remove_subdevs(ip6203);
	
	kfree(ip6203);
	return 0;
}

static void ip6203_shutdown(struct i2c_client *client)
{
	/*struct ip6203 *ip6203 = i2c_get_clientdata(client);

	if (ip6203->irq)
		ip6203_irq_exit(ip6203);*/
}

#ifdef CONFIG_PM
static int ip6203_suspend(struct i2c_client *client, pm_message_t state)
{
	struct ip6203 *ip6203 = i2c_get_clientdata(client);
	
	IP6203_DBG("PMU: %s:\n", __func__);
	
	if (ip6203->irq)
		disable_irq(ip6203->irq);
	
	ip6203_sleep();
	
	return 0;
}

static int ip6203_resume(struct i2c_client *client)
{
	struct ip6203 *ip6203 = i2c_get_clientdata(client);
	struct ip6203_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;

	IP6203_DBG("PMU: %s:\n", __func__);

	ip6203_chip_init(ip6203, ip);

	if (ip6203->irq)
		enable_irq(ip6203->irq);

	return 0;
}

#endif

static const struct i2c_device_id ip6203_i2c_id[] = {
	{"ip6203", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, ip6203_i2c_id);

static struct i2c_driver ip6203_i2c_driver = {
	.driver = {
			.name = "ip6203",
			.owner = THIS_MODULE,
	},
	.probe = ip6203_probe,
	.remove = ip6203_remove,
	.shutdown = ip6203_shutdown,
#ifdef CONFIG_PM
	.suspend = ip6203_suspend,
	.resume = ip6203_resume,
#endif
	.id_table = ip6203_i2c_id,
};


static int __init ip6203_init(void)
{
	int ret = -ENODEV;
	
	
	ret = i2c_add_driver(&ip6203_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ip6203_init);

static void __exit ip6203_exit(void)
{
	i2c_del_driver(&ip6203_i2c_driver);
}
module_exit(ip6203_exit);

MODULE_DESCRIPTION("Elitechip IP6203 PMU driver");
MODULE_LICENSE("GPL");
