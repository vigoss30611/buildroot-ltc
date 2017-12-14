/*
 * infotm_hid.c -- HID Composite driver
 *
 *
 * Copyright (C) 2016 ivan <ivan.zhuang@infotm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb/composite.h>

#include "gadget_chips.h"

#ifdef ERROR
#undef ERROR
#define ERROR(d, fmt, args...) \
        dev_err(&(d)->gadget->dev , fmt , ## args)
#endif
/*-------------------------------------------------------------------------*/

/*
 * kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "f_hid.c"

#define INFOTM_JOYSTICK
#ifdef INFOTM_KEYBOARD
/* hid descriptor for a keyboard */
static struct hidg_func_descriptor infotm_hid_data = {
    .subclass       = 0, /* No subclass */
    .protocol       = 1, /* Keyboard */
    .report_length      = 8,
    .report_desc_length = 63, 
    .report_desc        = {
        0x05, 0x01, /* USAGE_PAGE (Generic Desktop)           */
        0x09, 0x06, /* USAGE (Keyboard)                       */
        0xa1, 0x01, /* COLLECTION (Application)               */
        0x05, 0x07, /*   USAGE_PAGE (Keyboard)                */
        0x19, 0xe0, /*   USAGE_MINIMUM (Keyboard LeftControl) */
        0x29, 0xe7, /*   USAGE_MAXIMUM (Keyboard Right GUI)   */
        0x15, 0x00, /*   LOGICAL_MINIMUM (0)                  */
        0x25, 0x01, /*   LOGICAL_MAXIMUM (1)                  */
        0x75, 0x01, /*   REPORT_SIZE (1)                      */
        0x95, 0x08, /*   REPORT_COUNT (8)                     */
        0x81, 0x02, /*   INPUT (Data,Var,Abs)                 */
        0x95, 0x01, /*   REPORT_COUNT (1)                     */
        0x75, 0x08, /*   REPORT_SIZE (8)                      */
        0x81, 0x03, /*   INPUT (Cnst,Var,Abs)                 */
        0x95, 0x05, /*   REPORT_COUNT (5)                     */
        0x75, 0x01, /*   REPORT_SIZE (1)                      */
        0x05, 0x08, /*   USAGE_PAGE (LEDs)                    */
        0x19, 0x01, /*   USAGE_MINIMUM (Num Lock)             */
        0x29, 0x05, /*   USAGE_MAXIMUM (Kana)                 */
        0x91, 0x02, /*   OUTPUT (Data,Var,Abs)                */
        0x95, 0x01, /*   REPORT_COUNT (1)                     */
        0x75, 0x03, /*   REPORT_SIZE (3)                      */
        0x91, 0x03, /*   OUTPUT (Cnst,Var,Abs)                */
        0x95, 0x06, /*   REPORT_COUNT (6)                     */
        0x75, 0x08, /*   REPORT_SIZE (8)                      */
        0x15, 0x00, /*   LOGICAL_MINIMUM (0)                  */
        0x25, 0x65, /*   LOGICAL_MAXIMUM (101)                */
        0x05, 0x07, /*   USAGE_PAGE (Keyboard)                */
        0x19, 0x00, /*   USAGE_MINIMUM (Reserved)             */
        0x29, 0x65, /*   USAGE_MAXIMUM (Keyboard Application) */
        0x81, 0x00, /*   INPUT (Data,Ary,Abs)                 */
        0xc0        /* END_COLLECTION                         */
    }
};
#endif

#ifdef INFOTM_JOYSTICK
/* hid descriptor for a joystick */
static struct hidg_func_descriptor infotm_hid_data = {
    .subclass       = 0, /* No subclass */
    .protocol       = 0, /* None */
    .report_length      = 4,
    .report_desc_length = 37, 
    .report_desc        = {
        0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)$
        0x09, 0x04,                    // USAGE (Joystick)$
        0xa1, 0x01,                    // COLLECTION (Application)$
        0x75, 0x08,                    //     REPORT_SIZE (8)$
        0x95, 0x03,                    //     REPORT_COUNT (3)$
        0x81, 0x03,                    //     INPUT (Cnst,Var,Abs) 
        0x75, 0x01,                    //     REPORT_SIZE (1)$
        0x95, 0x04,                    //     REPORT_COUNT (4)$
        0x81, 0x03,                    //     INPUT (Cnst,Var,Abs) 
        0x05, 0x09,                    //     USAGE_PAGE (Button)$
        0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)$
        0x29, 0x04,                    //     USAGE_MAXIMUM (Button 4)$
        0x15, 0x00,                    //     LOGICAL_MINIMUM (0)$
        0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)$
        0x75, 0x01,                    //     REPORT_SIZE (1)$
        0x95, 0x04,                    //     REPORT_COUNT (4)$
        0x65, 0x00,                    //     UNIT (None)$
        0x81, 0x02,                    //     INPUT (Data,Var,Abs)$
        0xc0                           // END_COLLECTION    
    }
};

#endif

static void infotm_hid_release(struct device *dev)
{
}

static struct platform_device infotm_hid = {
    .name           = "hidg",
    .id         = 0,
    .num_resources      = 0,
    .resource       = 0,
    .dev.platform_data  = &infotm_hid_data,
	.dev.release = infotm_hid_release,
};


struct hidg_func_node {
	struct list_head node;
	struct hidg_func_descriptor *func;
};

static LIST_HEAD(hidg_func_list);

/****************************** Gadget Bind ******************************/

int infotm_hid_bind_config(struct usb_composite_dev *cdev, struct usb_configuration *c)
{
	struct hidg_func_node *e;
	struct list_head *tmp;
	int status, funcs = 0;

	
	list_for_each(tmp, &hidg_func_list)
		funcs++;

	if (!funcs)
		return -ENODEV;

	/* set up HID */
	status = ghid_setup(cdev->gadget, funcs);
	if (status < 0)
		return status;
	funcs = 0;
    
	list_for_each_entry(e, &hidg_func_list, node) {
		status = hidg_bind_config(c, e->func, funcs++);
		if (status)
			break;
	}

	return status;
}


static int __init hidg_plat_driver_probe(struct platform_device *pdev)
{
	struct hidg_func_descriptor *func = pdev->dev.platform_data;
	struct hidg_func_node *entry;

	if (!func) {
		dev_err(&pdev->dev, "Platform data missing\n");
		return -ENODEV;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->func = func;
	list_add_tail(&entry->node, &hidg_func_list);

	return 0;
}

static int hidg_plat_driver_remove(struct platform_device *pdev)
{
	struct hidg_func_node *e, *n;

	list_for_each_entry_safe(e, n, &hidg_func_list, node) {
		list_del(&e->node);
		kfree(e);
	}

	return 0;
}

/****************************** Some noise ******************************/
static struct platform_driver hidg_plat_driver = {
	.remove		= hidg_plat_driver_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "hidg",
	},
};

int __init infotm_hid_setup(void)
{
	int status;
	status = platform_device_register(&infotm_hid);
	if (status)
	{
		printk("register hid device error\n");
		return status;
	}
	status = platform_driver_probe(&hidg_plat_driver,
				hidg_plat_driver_probe);
	if (status < 0)
	{
		platform_device_unregister(&infotm_hid);
	}
	return status;
}

void infotm_hid_cleanup(void)
{
	ghid_cleanup();
	platform_device_unregister(&infotm_hid);
	platform_driver_unregister(&hidg_plat_driver);
}
