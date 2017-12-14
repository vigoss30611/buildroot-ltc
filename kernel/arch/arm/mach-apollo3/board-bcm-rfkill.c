/*
 * Copyright (C) 2009 Google, Inc.
 * Copyright (C) 2009 HTC Corporation.
 * Copyright (C) 2011 Broadcom Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * founction: control wifi power state
 * usage:
 * open wifi:echo 1 > /sys/devices/platform/wifi_usb_rfkill/rfkill/rfkill0/subsystem/rfkill0/state
 * close wifi:echo 0 > /sys/devices/platform/wifi_usb_rfkill/rfkill/rfkill0/subsystem/rfkill0/state
 * log:
 * 2015-03-08: add this file to project from redtop wifi driver source code packge by linyun.xiong
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rfkill.h>

extern void wifi_power(int power);

static struct rfkill *wifi_rfk;
static const char wifi_usb_name[] = "brcm_usb_wifi_rfkill";

static int wifi_usb_set_power(void *data, bool blocked)
{

	printk(KERN_ERR "wifi_set_power set blocked=%d\n", blocked);
	if (!blocked) {
		wifi_power(1);
		msleep(300);
		printk(KERN_ERR "BRCM USB MODULE RESET HIGH!!");
	} else {
		//wifi_power(0);
		//printk(KERN_ERR "BRCM USB MODULE RESET LOW!!");
	}
	return 0;
}

static struct rfkill_ops wifi_usb_rfkill_ops = {
	.set_block = wifi_usb_set_power,
};

static int wifi_usb_rfkill_probe(struct platform_device *pdev)
{
	int rc = 0;
	bool default_state = true;  /* off */

	//wifi_usb_set_power(NULL, default_state);

	wifi_rfk = rfkill_alloc(wifi_usb_name, &pdev->dev, RFKILL_TYPE_WLAN,
			&wifi_usb_rfkill_ops, NULL);
	if (!wifi_rfk) {
		rc = -ENOMEM;
		goto err_rfkill_alloc;
	}

	rfkill_set_states(wifi_rfk, default_state, false);

	/* userspace cannot take exclusive control */

	rc = rfkill_register(wifi_rfk);
	if (rc)
		goto err_rfkill_reg;

	return 0;


err_rfkill_reg:
	rfkill_destroy(wifi_rfk);
err_rfkill_alloc:
	printk(KERN_ERR "wifi_usb_rfkill_probe error!\n");
	return rc;
}

static int wifi_usb_rfkill_remove(struct platform_device *dev)
{
	rfkill_unregister(wifi_rfk);
	rfkill_destroy(wifi_rfk);

	return 0;
}

static struct platform_device wifi_usb_rfkill_device = {
	.name = "wifi_usb_rfkill",
	.id   = -1,
};

static struct platform_driver wifi_usb_rfkill_driver = {
	.probe = wifi_usb_rfkill_probe,
	.remove = wifi_usb_rfkill_remove,
	.driver = {
		.name = "wifi_usb_rfkill",
		.owner = THIS_MODULE,
	},
};

static int __init wifi_usb_rfkill_init(void)
{
	int ret;

	ret = platform_driver_register(&wifi_usb_rfkill_driver);
	return platform_device_register(&wifi_usb_rfkill_device);;
}

static void __exit wifi_usb_rfkill_exit(void)
{
	platform_device_unregister(&wifi_usb_rfkill_device);
	platform_driver_unregister(&wifi_usb_rfkill_driver);
}

late_initcall(wifi_usb_rfkill_init);
module_exit(wifi_usb_rfkill_exit);

MODULE_DESCRIPTION("wifi usb rfkill");
MODULE_AUTHOR("Nick Pelly <npelly@google.com>");
MODULE_LICENSE("GPL");
