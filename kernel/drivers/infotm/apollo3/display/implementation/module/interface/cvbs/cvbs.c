/*
 * cvbs.c - cvbs interface driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
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

#include "cvbs.h"
#include "gm7122.h"

#define DSS_LAYER   "[dss]"
#define DSS_SUB1    "[interface]"
#define DSS_SUB2    "[cvbs]"

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);
struct cvbs_ops *m_cvbs;

static int vic_config = 0;

static struct module_attr cvbs_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node cvbs_module = {
		.name = "cvbs",
		.attributes = cvbs_attr,
		.init = dss_init_module,	
};

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);

	vic_config = vic;

	return 0;
}

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;

	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	if (enable) {
		if (m_cvbs->config(vic_config)) {
			dss_err("cvbs config failed\n");
			return -1;
		}
	} else {
		if (m_cvbs->disable()) {
			dss_err("cvbs disable failed\n");
			return -1;
		}
	}

	return 0;
}

static int init = 0;
static int dss_init_module(int idsx)
{
	unsigned char name[128] = {0};

	if (init == 0) {
		if (item_exist(P_CVBS_NAME)) {
			item_string(name, P_CVBS_NAME, 0);

			m_cvbs = kzalloc(sizeof(struct cvbs_ops), GFP_KERNEL);
			if (!m_cvbs) {
				dss_err("Failed to allocate memory\n");
				return -ENOMEM;
			}

			if (strcmp(name, "gm7122") == 0) {
				dss_dbg("using gm7122 chip\n");
				if (gm7122_init()) {
					dss_err("gm7122 init failed\n");
					return -1;
				}
			} else if (strcmp(name, "cs8556") == 0) {
				dss_dbg("using cs8556 chip\n");
			} else {
				dss_err("cvbs chip not support now\n");
				return -1;
			}
		} else {
			dss_err("cvbs chip name not configed\n");
			return -1;
		}
		init = 1;
	}
	return 0;
}
