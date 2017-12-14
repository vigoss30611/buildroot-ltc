/*
 * TXDT200LER LCD Module  driver
 * LCD Driver IC : ota5812a-c2.
 *
 * Copyright (c) 2015 
 * Author: shuyu.li  <394056202@qq.com>
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

#include "ota5182a.h"

struct ota5182a *ota5182a_spidev;

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
	u16 buf[1];
	struct spi_message msg;

	struct spi_transfer xfer = {
		.len		= 2,
		.tx_buf		= buf,
	};

	buf[0] = data;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	return spi_sync(spidev, &msg);
}


int ota5182a_init(struct ota5182a *spidev)
{
	int ret=0;
    
    struct spi_device *spi;
	spi = spidev->spi;
#if 0 
	ota5182a_spi_write(spi, 0x000F);  
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);
	
	ota5182a_spi_write(spi, 0x0000);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);
	
	ota5182a_spi_write(spi, 0x000F);
	mdelay(120);

	ota5182a_spi_write(spi, 0x6007);
	mdelay(10);
	ota5182a_spi_write(spi, 0x6007);//CCIR656 NTSC 6007,PAL 601f
	ota5182a_spi_write(spi, 0x3008);
	ota5182a_spi_write(spi, 0x703b);
	ota5182a_spi_write(spi, 0xc005);   
	ota5182a_spi_write(spi, 0xe013); 
#else
	ota5182a_spi_write(spi, 0x000f);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);

	ota5182a_spi_write(spi, 0x000f);
	ota5182a_spi_write(spi, 0x0005);
	mdelay(80);

	ota5182a_spi_write(spi, 0x000f);
	mdelay(120);

	ota5182a_spi_write(spi, 0x6001);
	mdelay(10);

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
	return ret;
}
EXPORT_SYMBOL(ota5182a_init);

static int ota5182a_power_on(struct ota5182a *spidev)
{
	int ret = 0;

    dev_err(spidev->dev, "lcd setting failed.\n");
	
	return 0;
}

static int ota5182a_power_off(struct ota5182a *spidev)
{
	int ret;

    dev_err(spidev->dev, "lcd setting failed.\n");

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


    ota5182a_init(spidev);
    spi_set_drvdata(spi, spidev);

	ota5182a_spidev = spidev;
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

	dev_dbg(dev, "lcd suspend \n");

	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}

static int ota5182a_resume(struct device *dev)
{

	struct ota5182a *spidev = dev_get_drvdata(dev);

	dev_dbg(dev, "lcd suspend \n");

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

	return 0;
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

MODULE_AUTHOR("shuyu.li	<394056202@qq.com>");
MODULE_DESCRIPTION("ota5182a LCD Driver");
MODULE_LICENSE("GPL");
