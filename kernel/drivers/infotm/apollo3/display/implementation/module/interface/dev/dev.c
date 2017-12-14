/*
 * dev.c - lcd device interface driver
 *
 * Copyright (c) 2016 Davinci Liang <davinci.liang@infotm.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <dss_common.h>
#include <ids_access.h>
#include <mach/pad.h>

#include "dev.h"

#ifdef CONFIG_APOLLO3_LCD_SPI_ILI8961
#include "ili8961.h"
#endif

#ifdef CONFIG_APOLLO3_LCD_SPI_OTA5182A
#include "ota5182a.h"
#endif

#ifdef CONFIG_APOLLO3_CVBS_I2C_IP2906
#include "ip2906-cvbs.h"
#endif

LIST_HEAD(display_dev_list);
static struct display_dev *dev = NULL;
static int init = 0;

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[dev]"

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);

static int vic_config = 0;

static struct module_attr dev_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node dev_module = {
		.name = "dev",
		.attributes = dev_attr,
		.init = dss_init_module,
};

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	dss_trace("ids%d, vic %d\n", idsx, vic);
	vic_config = vic;
	return 0;
}

int dev_register(struct display_dev *udev)
{
	dss_trace("\n");
	if (!udev)
		return -1;

	list_for_each_entry(dev, &display_dev_list, link)
		if(strcmp(dev->name, udev->name) == 0)
			return 0;

	list_add_tail(&(udev->link), &display_dev_list);
	return 0;
}

static int lookup_dev(char* name)
{
	dss_trace("\n");
	list_for_each_entry(dev, &display_dev_list, link)
		if(dev&&strcmp(dev->name, name) == 0) {
			dss_dbg("srgb dev name :%s\n", dev->name);
			return 0;
		}
	return -1;
}

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;

	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	if (dev->enable(enable)) {
		dss_err("srgb config failed\n");
		return -1;
	}

	return 0;
}

void dev_config(char *args, int num)
{
	if (init)
		dev->config(args, num);
}

static void dev_init(void)
{
	dss_trace("\n");

#ifdef CONFIG_APOLLO3_LCD_SPI_ILI8961
	ili8961_init();
#endif

#ifdef CONFIG_APOLLO3_LCD_SPI_OTA5182A
	ota5182a_init();
#endif

#ifdef CONFIG_APOLLO3_CVBS_I2C_IP2906
	ip2906_init();
#endif
}

static int dss_init_module(int idsx)
{
	int ret = 0;

	if (init)
		return -1;

	dev_init();

	dss_trace("dev :%s\n", core.dev_name);
	ret = lookup_dev(core.dev_name);
	if (ret < 0){
		dss_err("can't find device : %s\n", core.dev_name);
		goto err;
	}

	//dev_config(NULL, 0);

	init = 1;
	return 0;
err:
	return -1;
}

