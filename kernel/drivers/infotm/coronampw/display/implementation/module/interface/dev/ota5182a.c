/*
 * TXDT200LER LCD Module  driver
 * LCD Driver IC : ota5812a-c2.
 *
 * Copyright (c) 2016 
 * Author: davinci.liang<davinci.liang@infotm.com>
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
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <mach/pad.h>
#include <linux/gpio.h>

#include <dss_common.h>
#include <ids_access.h>

#include "ota5182a.h"
#include "dev.h"

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[ota5182a-spi]"

struct ota5182a *ota5182a_spidev = NULL;
static struct display_dev * ota5182a_dev = NULL;

struct ota5182a {
	struct device			*dev;
	struct spi_device		*spi;
	unsigned int			power;
	struct lcd_platform_data	*lcd_platdata;
	struct mutex			lock;
	bool  enabled;
};

static int ota5182a_spi_write(struct spi_device *spidev, unsigned short data)
{
	int ret;
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

static int ota5182a_power_on(struct ota5182a *spidev)
{
	dev_err(spidev->dev, "ota5182a_power_on\n");
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	if(core.dev_reset)
		gpio_set_value(core.dev_reset, 1);
	msleep(100);
#endif
	return 0;
}

static int ota5182a_power_off(struct ota5182a *spidev)
{
	dev_err(spidev->dev, "ota5182a_power_off\n");
#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	if(core.dev_reset)
		gpio_set_value(core.dev_reset, 0);
	msleep(100);
#endif
	return 0;
}

int ota5182a_config(char *args, int num)
{
	struct ota5182a * spidev = ota5182a_spidev;
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

	ota5182a_power_on(spidev);
#if 1
	ota5182a_spi_write(spi, 0x000F);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);
	
	ota5182a_spi_write(spi, 0x0000);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);
	
	ota5182a_spi_write(spi, 0x000F);
	mdelay(120);

	//ota5182a_spi_write(spi, 0x6007);
	ota5182a_spi_write(spi, 0x601f);
	mdelay(10);
	//ota5182a_spi_write(spi, 0x6007);//CCIR656 NTSC:6007,PAL: 601f
	ota5182a_spi_write(spi, 0x601f);
	ota5182a_spi_write(spi, 0x3008);
	ota5182a_spi_write(spi, 0x703b);
	ota5182a_spi_write(spi, 0xc005);
	ota5182a_spi_write(spi, 0xe013);
#else
	ota5182a_spi_write(spi, 0x000f);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(5);

	ota5182a_spi_write(spi, 0x000f);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(5);

	ota5182a_spi_write(spi, 0x000f);
	mdelay(5);

	ota5182a_spi_write(spi, 0x6001);
	mdelay(5);

	ota5182a_spi_write(spi, 0x6001);
	ota5182a_spi_write(spi, 0x8001);
	ota5182a_spi_write(spi, 0x3008);
	ota5182a_spi_write(spi, 0x703b);
	ota5182a_spi_write(spi, 0xc005);
	ota5182a_spi_write(spi, 0xe013);
/*

	dev_info(&spi->dev, "Init LCD module(TXDT200LER) .\n");
	mdelay(120);
	ota5182a_spi_write(spi, 0x6002);
	
	mdelay(120);
	ota5182a_spi_write(spi, 0x6002);
*/
#endif
	return 0;
}


int ota5182a_enable(int en)
{
	dss_trace("\n");
	if (en == 1)
		ota5182a_power_on(ota5182a_spidev);
	else
		ota5182a_power_off(ota5182a_spidev);

	return 0;
}

int ota5182a_init(void)
{
	static int init = 0;
	struct spi_device *spi = NULL;
	struct ota5182a * spidev = ota5182a_spidev;

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

	ota5182a_dev = kzalloc(sizeof(struct display_dev), GFP_KERNEL);
	if (!ota5182a_dev) {
		dss_err("Failed to allocate memory\n");
		return -ENOMEM;
	}

	ota5182a_dev->name = OTA5182A_SPI_DEVICE_NAME;
	ota5182a_dev->priv = (void*)spidev;

	ota5182a_dev->config = ota5182a_config;
	ota5182a_dev->enable = ota5182a_enable;

	dev_register(ota5182a_dev);
	init = 1;

	return 0;
}

static int ota5182a_probe(struct spi_device *spi)
{
	int ret = 0;
	struct ota5182a *spidev = NULL;

	spidev = devm_kzalloc(&spi->dev, sizeof(struct ota5182a), GFP_KERNEL);
	if (!spidev)
		return -ENOMEM;

	/* ota5182a lcd panel uses 3-wire 9bits SPI Mode. */
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

	/*
	 * because lcd panel was powered on automaticlly,
	 * so donnot need to power on it manually using software.
	 */
	ota5182a_spidev = spidev;

	ota5182a_init();
	spi_set_drvdata(spi, spidev);

	dev_info(&spi->dev, "ota5182a lcd driver has been probed.\n");

	return ret;
}

static int ota5182a_remove(struct spi_device *spi)
{
	struct ota5182a *spidev = spi_get_drvdata(spi);
	ota5182a_power_off(spidev);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ota5182a_suspend(struct device *dev)
{
	struct ota5182a *spidev = dev_get_drvdata(dev);
	dev_dbg(dev, "lcd suspend 0x%x\n", (u32)spidev);
	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}

static int ota5182a_resume(struct device *dev)
{
	struct ota5182a *spidev = dev_get_drvdata(dev);
	dev_dbg(dev, "lcd resume 0x%x \n", (u32)spidev);
	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ota5182a_pm_ops, ota5182a_suspend, ota5182a_resume);

/* Power down all displays on reboot, poweroff or halt. */
static void ota5182a_shutdown(struct spi_device *spi)
{
	struct ota5182a *spidev = spi_get_drvdata(spi);
	ota5182a_power_off(spidev);
}

static struct spi_driver ota5182a_driver = {
	.driver = {
		.name	= "ota5182a",
		.owner	= THIS_MODULE,
		.pm	= &ota5182a_pm_ops,
	},
	.probe		= ota5182a_probe,
	.remove		= ota5182a_remove,
	.shutdown	= ota5182a_shutdown,
};

module_spi_driver(ota5182a_driver);
MODULE_AUTHOR("davinci.liang<davinci.liang@infotm.com>");
MODULE_DESCRIPTION("ota5182a LCD Driver");
MODULE_LICENSE("GPL");
