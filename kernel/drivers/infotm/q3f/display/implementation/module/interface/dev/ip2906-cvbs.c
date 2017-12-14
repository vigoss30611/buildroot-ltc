/*
 * IP2906-CVBS Module  driver
 * LCD Driver IC :
 *
 * Copyright (c) 2016 
 * Author: Davinci Liang<davinci.liang@infotm.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/lcd.h>
#include <linux/module.h>
#include <mach/items.h>
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <dss_common.h>
#include <mach/pad.h>
#include <linux/gpio.h>
#include <linux/clk.h>

#include <mach/items.h>

#include <dss_common.h>
#include <ids_access.h>

#include "dev.h"
#include "ip2906-cvbs.h"

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[ip2906]"

static uint8_t cvbs_regs[] = {
	IP2906_INT_CTRL,
	IP2906_MFP_PAD,
	IP2906_CMU_ANALOG,
	IP2906_CMU_CLKEN,
	IP2906_DEV_RST,
	/*cvbs*/
	IP2906_CVBS_CTRL,
	IP2906_CVBS_COLOR_CTRL,
	IP2906_CVBS_BRIGHT_CTRL,
	IP2906_CVBS_HUE_CTRL,
	IP2906_CVBS_SBCP_CTRL,
	IP2906_CVBS_SBCF_CTRL,
	IP2906_CVBS_APTD_CTRL,
	IP2906_CVBS_SIN_CTRL,
	IP2906_CVBS_HDAC_TEST,
	IP2906_CVBS_ROM_TEST,
	IP2906_CVBS_ANALOG_REG,
	IP2906_CVBS_CLK_CTRL
};

struct ip2906_setting {
	uint8_t saturation;
	uint8_t contrast;
	uint8_t bright;
	uint8_t hue;
	uint16_t sub_carrier_phase;
	uint8_t sub_carrier_freq;
	uint8_t color_burst_amp_level;
	uint8_t sync_pedestal_amp_level;
};

struct ip2906_cvbs {
	int format;
	int colorbar;
	int gpio;
	int irq;
	struct ip2906_setting setting;

	spinlock_t lock;
	struct work_struct _work;
	struct delayed_work delay_work;
};

static struct ip2906_cvbs* cvbs = NULL;
static struct display_dev* ip2906_dev = NULL;

static int ip2906_dump(void)
{
	int i,j;
	int ret = 0;
	uint16_t reg;

	printk("Reg  Hex      D7  D6  D5  D4  D3  D2  D1  D0  d7  d6  d5  d4  d3  d2  d1  d0\n");
	printk("     ------------------------------------------------------------------------\n");
	for (i = 0; i < sizeof(cvbs_regs); i++) {
		printk("%02XH| ", cvbs_regs[i]);
		ret = ip2906_read(cvbs_regs[i], &reg);
		if (ret < 0) {
			printk("ip2906_read 0x%x 0x%x\n", cvbs_regs[i], reg);
			break;
		}

		printk(" %04X  - ", reg);
		for (j = 0; j < 16; j++)
			printk(" %d  ", (reg >> (15-j)) & 0x01);
		printk("\n");
	}
	printk("     ------------------------------------------------------------------------\n");
	
	return 0;
}

int ip2906_set_color_saturation(uint8_t saturation)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_COLOR_CTRL, &val);

	val &= 0x00ff;
	val |= saturation << 8;
	(void)ip2906_write(IP2906_CVBS_COLOR_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_COLOR_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", saturation);
		return -1;
	}
	return 0;
}

int ip2906_get_color_saturation(uint8_t *saturation)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_COLOR_CTRL, &val);

	*saturation = (val >> 8) & 0x0f;
	return ret;
}

int ip2906_set_contrast(uint8_t contrast)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_COLOR_CTRL, &val);

	val &= 0xff00;
	val |= contrast;
	(void)ip2906_write(IP2906_CVBS_COLOR_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_COLOR_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", contrast);
		return -1;
	}
	return 0;
}

int ip2906_get_contrast(uint8_t *contrast)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_COLOR_CTRL, &val);

	*contrast = val & 0x0f;
	return ret;
}


int ip2906_set_bright(uint8_t bright)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_BRIGHT_CTRL, &val);

	val &= ~(0xff);
	val |= bright;
	(void)ip2906_write(IP2906_CVBS_BRIGHT_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_BRIGHT_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", bright);
		return -1;
	}
	return 0;
}

int ip2906_get_bright(uint8_t *bright)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_BRIGHT_CTRL, &val);

	*bright = val & 0xff;
	return ret;
}

int ip2906_set_hue(uint8_t hue)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_HUE_CTRL, &val);

	val &= ~(0xff);
	val |= hue;
	(void)ip2906_write(IP2906_CVBS_HUE_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_HUE_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", hue);
		return -1;
	}
	return 0;
}

int ip2906_get_hue(uint8_t *hue)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_HUE_CTRL, &val);

	*hue = val & 0xff;
	return ret;
}

int ip2906_set_sub_carrier_phase(uint16_t phase)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_SBCP_CTRL, &val);

	val &= ~(0xfff);
	val |= phase;
	(void)ip2906_write(IP2906_CVBS_SBCP_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_SBCP_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", phase);
		return -1;
	}
	return 0;
}

int ip2906_get_sub_carrier_phase(uint16_t* phase)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_SBCP_CTRL, &val);

	*phase = val & 0xfff;
	return ret;
}

int ip2906_set_sub_carrier_freq(uint8_t freq)
{
	uint16_t val, val_r;
	(void)ip2906_read(IP2906_CVBS_SBCF_CTRL, &val);

	val &= ~(0x1f);
	val |= freq;
	(void)ip2906_write(IP2906_CVBS_SBCF_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_SBCF_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u\n failed\n ", freq);
		return -1;
	}
	return 0;
}

int ip2906_get_sub_carrier_freq(uint8_t* freq)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_SBCF_CTRL, &val);

	*freq = val & 0x1f;
	return ret;
}

int ip2906_set_amplitude_level(
		uint8_t color_burst_amp_level, uint8_t sync_pedestal_amp_level)
{
	uint16_t val, val_r;

	(void)ip2906_read(IP2906_CVBS_APTD_CTRL, &val);

	val &= ~(0x0);
	val |= color_burst_amp_level << 8| sync_pedestal_amp_level;

	(void)ip2906_write(IP2906_CVBS_APTD_CTRL,val);

	(void)ip2906_read(IP2906_CVBS_APTD_CTRL, &val_r);

	if (val != val_r) {
		dss_err("val = %u %u\n failed\n ", color_burst_amp_level,
			sync_pedestal_amp_level);
		return -1;
	}
	return 0;
}

int ip2906_get_amplitude_level(uint8_t* color_burst_amp_level,
		uint8_t* sync_pedestal_amp_level)
{
	int ret = 0;
	uint16_t val = 0;
	ret = ip2906_read(IP2906_CVBS_APTD_CTRL, &val);

	*color_burst_amp_level = (val >> 8) | 0xf;
	*sync_pedestal_amp_level = val & 0xf;
	return ret;
}

static void ip2906_cvbs_draw_plug_handler(void)
{
	uint16_t val = 0;

	(void)ip2906_read(IP2906_CVBS_ANALOG_REG, &val);
	printk("# ip2906 cvbs irq val = 0x%x\n", val);

	if (val & (1 << 13)) {
		printk("# ip2906 cvbs draw ......\n");
		ip2906_write(IP2906_CVBS_ANALOG_REG,val | (1 << 13));
	}

	if (val & (1 << 12)) {
		printk("# ip2906 cvbs plug ......\n");
		ip2906_write(IP2906_CVBS_ANALOG_REG,val | (1 << 12));
	}
}

static int omac_irq_setup(void);
int ip2906_cvbs_config(uint8_t colorbar, uint8_t format)
{
	int ret = 0;
	uint16_t reg = 0;

	/* ip2906_dump(); */
	ret = omac_irq_setup();
	if (ret < 0)
		dss_err("omac_irq setup failed, gpio = %d irq = %d\n", cvbs->gpio, cvbs->irq);

	ret = ip2906_read(IP2906_INT_CTRL, &reg);
	if (reg & 0x04) {
		dss_trace("ip2906 is ready\n");
	} else {
		dss_err("ip2906 is not ready\n");
		return -1;
	}

	/*select bt656 pin ,Reg0x01 bit6/[3-0]: */
	ip2906_write(IP2906_MFP_PAD,0x4A);

	/*config CVBS PLL*/
	ip2906_write(IP2906_CMU_ANALOG, 0x0a53);

	(void)ip2906_read(IP2906_CMU_CLKEN, &reg);
	/* cvbs Clock Enable?Reg0x04 bit2=1 */
	ip2906_write(IP2906_CMU_CLKEN,reg | 0x04); /* add cvbs  */

	/* cvbs reset: Reg0x05 bit2=0-->1 */
	ip2906_write(IP2906_DEV_RST,0x00); /* 000b */
	ip2906_write(IP2906_DEV_RST,0x07); /* 111b*/

	/* config BT clockphase and delay time */
	ip2906_write(IP2906_CVBS_CLK_CTRL, 0x00);

	(void)ip2906_read(IP2906_CVBS_ANALOG_REG, &reg);
	ip2906_write(IP2906_CVBS_ANALOG_REG,reg | 0x03 | 0x3 << 12); /* EN_DRAW and EN_PLUG */

	/* setting */

	if (format > 0x01) /*if format is PAL */
		format |= 1 << 5; /* CVBS_CTL[5] CLK_SEL = 0x1*/

	if (!colorbar) {
		/* enable HDAC and CVBS module enable */
		reg = 0x88 << 8 | format;
		ip2906_write(IP2906_CVBS_CTRL, reg);
	} else {
		reg = 0x8A << 8 | format;
		ip2906_write(IP2906_CVBS_CTRL, reg);/* color Bar */
	}

	ip2906_dump();
	return 0;
}

static int cvbs_config(char *args, int num)
{
	dss_dbg("\n");
	if (num ==2) {
		cvbs->format = *((int*)args[0]);
		cvbs->colorbar = *((int*)args[1]);
	}

	/* wait for audio codec probe and i2c.1 init*/
	schedule_delayed_work(&cvbs->delay_work, msecs_to_jiffies(2000));
	return 0;
}

static int cvbs_enable(int en)
{
	dss_dbg("\n");
	//TODO
	return 0;
}

static void _delay_work(struct work_struct *data)
{
	(void)ip2906_cvbs_config(cvbs->colorbar, cvbs->format);
}

static void omac_irq_work(struct work_struct *data)
{
	dss_info("\n");
	ip2906_cvbs_draw_plug_handler();
}

static irqreturn_t omac_irq_isr(int irq, void *dev_id)
{
	struct ip2906_cvbs*bdata = dev_id;
	dss_info("\n");
	BUG_ON(irq != bdata->irq);
	schedule_work(&bdata->_work);

	return IRQ_HANDLED;
}

static int omac_irq_setup(void)
{
	char *desc = "omac_irq";
	irq_handler_t isr;
	unsigned long irqflags;
	int irq, error;

	spin_lock_init(&cvbs->lock);

	if (gpio_is_valid(cvbs->gpio)) {

		error = gpio_request_one(cvbs->gpio, GPIOF_IN, desc);
		if (error < 0) {
			dss_err("Failed to request GPIO %d, error %d\n",
				cvbs->gpio, error);
			return error;
		}

		irq = gpio_to_irq(cvbs->gpio);
		if (irq < 0) {
			error = irq;
			dss_err(
				"Unable to get irq number for GPIO %d, error %d\n",
				cvbs->gpio, error);
			goto fail;
		}
		cvbs->irq = irq;

		isr = omac_irq_isr;
		irqflags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;

	}

	error = request_any_context_irq(cvbs->irq, isr, irqflags, desc, cvbs);
	if (error < 0) {
		dss_err( "Unable to claim irq %d; error %d\n",
			cvbs->irq, error);
		goto fail;
	}

	return 0;

fail:
	if (gpio_is_valid(cvbs->gpio))
		gpio_free(cvbs->gpio);

	return error;
}

static int ip2906_register(void)
{
	uint32_t val;
	//struct clk* clk_out0 = NULL;

	dss_info("ip2096 register\n");
	cvbs  = kzalloc(sizeof(struct ip2906_cvbs), GFP_KERNEL);
	if (!cvbs) {
		dss_err("Failed to allocate memory\n");
		return -ENOMEM;
	}

	ip2906_dev = kzalloc(sizeof(struct display_dev), GFP_KERNEL);
	if (!ip2906_dev) {
		dss_err("Failed to allocate memory\n");
		return -ENOMEM;
	}

	ip2906_dev->name = IP2906_I2C_DEVICE_NAME;
	ip2906_dev->priv = (void*)NULL;

	ip2906_dev->config = cvbs_config;
	ip2906_dev->enable = cvbs_enable;

	val = readb(IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);
	writeb(val | (1 << 3), IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);

	writeb(0x01, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x02E0);
	writeb(0x3f, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x02E4);
	writeb(0x00, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x02E8);
	writeb(0x00, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x02Ec);

#if 0
	clk_out0 = clk_get_sys("imap-camo", "imap-camo");
	if (!clk_out0) {
		dss_err("clk_get(clk_out1) failed\n");
		return -1;
	}

	clk_prepare_enable(clk_out0);
	clk_set_rate(clk_out0, 24000000); /* 24MHz */
#endif

	dev_register(ip2906_dev);

	cvbs->format = TV_NTSC_M;
	cvbs->colorbar = false;
	cvbs->gpio = 123;
	INIT_DELAYED_WORK(&cvbs->delay_work, _delay_work);
	INIT_WORK(&cvbs->_work, omac_irq_work);

	return 0;
}

int ip2906_init(void)
{
	static int init = 0;
	dss_dbg("\n");

	if (init == 0) {
		ip2906_register();
		init = 1;
	}

	return 0;
}
