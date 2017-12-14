/*
 * ILI8961 LCD Module  driver
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
#include <mach/items.h>
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <dss_common.h>

#include "ili8961.h"

struct ili8961 *ili8961_spidev;
struct ili8961 {
	struct device			*dev;
	struct spi_device		*spi;
	unsigned int			power;

	struct lcd_platform_data	*lcd_platdata;

	struct mutex			lock;
	bool  enabled;
};

static int rgb_bpp = 32;

static int ili8961_spi_write(struct spi_device *spidev, unsigned short data)
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


int ili8961_init(struct ili8961 *spidev)
{
	int ret=0;
    int i=0;
    struct spi_device *spi;
	spi = spidev->spi;
   
   ili8961_spi_write(spi, 0x055f);
   ili8961_spi_write(spi, 0x051f);
   ili8961_spi_write(spi, 0x055f);
   ili8961_spi_write(spi, 0x2B01);
   ili8961_spi_write(spi, 0x0007);
   ili8961_spi_write(spi, 0x01ac);
   ili8961_spi_write(spi, 0x2f69);
   if (rgb_bpp == 8)
	   ili8961_spi_write(spi, 0x042B);
   else
	   ili8961_spi_write(spi, 0x044B);
   ili8961_spi_write(spi, 0x1604);
   ili8961_spi_write(spi, 0x0340);
   ili8961_spi_write(spi, 0x0D40);
   ili8961_spi_write(spi, 0x0e40);
   ili8961_spi_write(spi, 0x0f40);
   ili8961_spi_write(spi, 0x1040);
   ili8961_spi_write(spi, 0x1140);
   dev_info(&spi->dev, "Init LCD module(ILI8961) .\n");
   mdelay(20);
	
   return ret;
}
EXPORT_SYMBOL(ili8961_init);

static int ili8961_power_on(struct ili8961 *spidev)
{
	int ret = 0;

    dev_err(spidev->dev, "lcd setting failed.\n");
	
	return 0;
}

static int ili8961_power_off(struct ili8961 *spidev)
{
	int ret;

    dev_err(spidev->dev, "lcd setting failed.\n");

	return 0;
}


static int ili8961_probe(struct spi_device *spi)
{
	int ret = 0;
	struct ili8961 *spidev = NULL;

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

	/*
	 * because lcd panel was powered on automaticlly,
	 * so donnot need to power on it manually using software.
	 */


    ili8961_init(spidev);
    spi_set_drvdata(spi, spidev);

	ili8961_spidev = spidev;

	if (item_exist(P_LCD_RGBBPP))
		rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
	else
		rgb_bpp = 32;

	dev_info(&spi->dev, "ili8961 lcd spi driver has been probed.\n");

	return ret;
}

static int ili8961_remove(struct spi_device *spi)
{
	struct ili8961 *spidev = spi_get_drvdata(spi);

	ili8961_power_off(spidev);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ili8961_suspend(struct device *dev)
{
	struct ili8961 *spidev = dev_get_drvdata(dev);

	dev_dbg(dev, "lcd suspend \n");

	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}

static int ili8961_resume(struct device *dev)
{

	struct ili8961 *spidev = dev_get_drvdata(dev);

	dev_dbg(dev, "lcd suspend \n");

	/*
	 * when lcd panel is suspend, lcd panel becomes off
	 * regardless of status.
	 */
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ili8961_pm_ops, ili8961_suspend, ili8961_resume);

/* Power down all displays on reboot, poweroff or halt. */
static void ili8961_shutdown(struct spi_device *spi)
{

	struct ili8961 *spidev = spi_get_drvdata(spi);

	ili8961_power_off(spidev);

	return 0;
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

MODULE_AUTHOR("shuyu.li	<394056202@qq.com>");
MODULE_DESCRIPTION("ili8961 LCD Driver");
MODULE_LICENSE("GPL");
