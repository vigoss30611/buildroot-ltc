/*
 * ILI8961 LCD Module  driver
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

#include <dss_common.h>
#include <ids_access.h>

#include "ili8961.h"
#include "dev.h"

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[ili8961-spi]"

struct ili8961 {
	struct device			*dev;
	struct spi_device		*spi;
	unsigned int			power;

	struct lcd_platform_data	*lcd_platdata;

	struct mutex			lock;
	bool  enabled;
};

static struct ili8961 *ili8961_spidev;
static struct display_dev * ili8961_dev;

extern int backlight_en_control(int enable);
static int ili8961_spi_write(struct spi_device *spidev, unsigned short data)
{
	
	u16 ret = 0;
	u16 buf[1];
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len		= 2,
		.tx_buf		= buf,
	};

	buf[0] = data;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	ret = spi_sync(spidev, &msg);
	if(ret < 0)
		dss_trace("spi write failed!\n");
	else
		dss_trace("spi write successfully\n");

	return ret;
}

static int ili8961_power_on(struct ili8961 *spidev)
{
	dev_info(spidev->dev, "%s\n",__func__);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	//gpio_set_value(core.dev_reset, 1);
#endif
	msleep(100);

	return 0;
}

 int ili8961_power_off(struct ili8961 *spidev)
{
	dev_info(spidev->dev, "%s\n",__func__);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	//gpio_set_value(core.dev_reset, 0);
#endif
	msleep(100);
	return 0;
}
EXPORT_SYMBOL(ili8961_power_off);

int ili8961_enable(int en)
{
	dss_trace("\n");
	if (en == 1)
		ili8961_power_on(ili8961_spidev);
	else
		ili8961_power_off(ili8961_spidev);

	return 0;
}

int ili8961_config(char *args, int num)
{
	struct ili8961 * spidev = ili8961_spidev;
	struct spi_device *spi = NULL;

	dss_trace("\n");
	if (!spidev) {
		dss_err( "%s.spidev is null\n", __func__);
		return -1;
	}

	spi = spidev->spi;
	if (!spi) {
		dss_err( "%s.spi is null\n", __func__);
		return -1;
	}

	ili8961_power_on(spidev);

	/*Note:0x5[6]: global reset, 1: normal operation */
	ili8961_spi_write(spi, 0x055f); 
	/*Note:0x5[6]: global reset, 0: reset all resigster to default value*/
	ili8961_spi_write(spi, 0x051f);
	ili8961_spi_write(spi, 0x055f);

	/*Note: 0x2B[0]: standby (power saving) mode control
	*0: standby mode (default) 1: normal operation
	*/
	ili8961_spi_write(spi, 0x2B01);
	ili8961_spi_write(spi, 0x0007);
	ili8961_spi_write(spi, 0x01ac);

	ili8961_spi_write(spi, 0x2f69);

	if (core.lcd_interface == DSS_INTERFACE_SRGB) {
		ili8961_spi_write(spi, 0x042B);/*8-bit Dummy RGB 360x240*/
		dss_trace("rgb bpp = 8\n");
	} else {
		ili8961_spi_write(spi, 0x044B); /*CCIR656*/
		dss_trace("rgb bpp = 32\n");
	}

	ili8961_spi_write(spi, 0x1604); /* Auto set to gamma2.2 */
	ili8961_spi_write(spi, 0x0340); /* RGB brightness level control 40h: center(0) */
	ili8961_spi_write(spi, 0x0D40);/*RGB contrast level setting */
	ili8961_spi_write(spi, 0x0e40); /*Red sub-pixel contrast level setting */
	ili8961_spi_write(spi, 0x0f40); /*Red sub-pixel brightness level setting */
	ili8961_spi_write(spi, 0x1040); /*Blue sub-pixel contrast level setting */
	ili8961_spi_write(spi, 0x1140); /*Blue sub-pixel brightness level setting */

	dev_info(&spi->dev, "Init LCD module(ILI8961) .\n");
	mdelay(20);

	return 0;
}

int ili8961_init(void)
{
	static int init = 0;
	struct spi_device *spi = NULL;
	struct ili8961 * spidev = ili8961_spidev;

	dss_trace("\n");

	if (init) {
		dss_err( "%s.spidev has been inited\n", __func__);
		return 0;
	}

	if (!spidev) {
		dss_err( "%s.spidev is null\n", __func__);
		return -1;
	}

	spi = spidev->spi;
	if (!spi) {
		dss_err( "%s.spi is null\n", __func__);
		return -1;
	}

	ili8961_dev = kzalloc(sizeof(struct display_dev), GFP_KERNEL);
	if (!ili8961_dev) {
		dss_err("Failed to allocate memory\n");
		return -ENOMEM;
	}

	ili8961_dev->name = ILI8961_SPI_DEVICE_NAME;
	ili8961_dev->priv = (void*)spidev;

	ili8961_dev->config = ili8961_config;
	ili8961_dev->enable = ili8961_enable;

	dev_register(ili8961_dev);
	init = 1;

	return 0;
}

static int ili8961_probe(struct spi_device *spi)
{
	int ret = 0;
	struct ili8961 *spidev = NULL;
	dss_trace("\n");

	spidev = devm_kzalloc(&spi->dev, sizeof(struct ili8961), GFP_KERNEL);
	if (!spidev)
		return -ENOMEM;

	/* ili8961 lcd panel uses 3-wire 9bits SPI Mode. */
	spi->bits_per_word = 16;

	ret = spi_setup(spi);
	if (ret < 0) {
		dev_err(&spi->dev, "spi setup failed.\n");
		return ret;
	}

	spidev->spi = spi;
	spidev->dev = &spi->dev;

	spidev->lcd_platdata = spi->dev.platform_data;
	if (!spidev->lcd_platdata) {
		dev_err(&spi->dev, "platform data is NULL.\n");
		//return -EINVAL;
	}

	ili8961_spidev = spidev;

	spi_set_drvdata(spi, spidev);

	dev_info(&spi->dev, "ili8961 lcd spi driver has been probed.\n");

	return ret;
}

static int ili8961_remove(struct spi_device *spi)
{
	struct ili8961 *spidev = spi_get_drvdata(spi);

	ili8961_power_off(spidev);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	//gpio_free(core.dev_reset);
#endif
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ili8961_suspend(struct device *dev)
{
	struct ili8961 *spidev = dev_get_drvdata(dev);
	dev_dbg(dev, " %s \n", __func__);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	backlight_en_control(0);
#endif
	msleep(10);
	ili8961_power_off(spidev);

	return 0;
}

static int ili8961_resume(struct device *dev)
{
	struct ili8961 *spidev = dev_get_drvdata(dev);
	dev_dbg(dev, " %s \n", __func__);

	ili8961_power_on(spidev);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	backlight_en_control(1);
#endif
	msleep(10);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ili8961_pm_ops, ili8961_suspend, ili8961_resume);
extern int backlight_en_control(int enable);

/* Power down all displays on reboot, poweroff or halt. */
static void ili8961_shutdown(struct spi_device *spi)
{
	struct ili8961 *spidev = spi_get_drvdata(spi);

	dev_info(spidev->dev, "%s\n", __func__);
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	backlight_en_control(0);
#endif
	msleep(10);
	ili8961_power_off(spidev);
}

static struct spi_driver ili8961_driver = {
	.driver = {
		.name	= "ili8961",
		.owner	= THIS_MODULE,
		.pm	= &ili8961_pm_ops,
	},
	.probe		= ili8961_probe,
	.remove		= ili8961_remove,
	.shutdown	= ili8961_shutdown,
};

module_spi_driver(ili8961_driver);

MODULE_AUTHOR(" Davinci Liang<davinci.liang@infotm.com>");
MODULE_DESCRIPTION("ili8961 LCD Driver");
MODULE_LICENSE("GPL");
