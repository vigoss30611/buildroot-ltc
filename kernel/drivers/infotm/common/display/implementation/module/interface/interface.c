/*
 * interface.c - display interface driver
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
#include <dss_common.h>
#include <implementation.h>


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[interface]"
#define DSS_SUB2	""


static int dss_init_module(int idsx);
extern int pt;
static char * interface_name[] = {"lcd_port", "cvbs", "hdmi"};

enum interface_name_type {
	INTERF_NAME_LCD_PORT = 0,
	INTERF_NAME_CVBS,
	INTERF_NAME_HDMI,
};

static struct module_attr interface_attr[] = {
//	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node interface_module[2] = {
	{	.name = "interface",
		.idsx = 0,
		.attr_ctrl = module_attr_ctrl,
//		.attributes = interface_attr,
		.init = dss_init_module,
		.parallel_select = "lcd_port",
	},
	{
		.name = "interface",
		.idsx = 1,
		.attr_ctrl = module_attr_ctrl,
//		.attributes = interface_attr,
		.init = dss_init_module,
		.parallel_select = "hdmi",
	},
};

static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	if (pt == PRODUCT_MID) {
		if (idsx == 0)
			interface_module[idsx].parallel_select = interface_name[INTERF_NAME_LCD_PORT];
		else
			interface_module[idsx].parallel_select = interface_name[INTERF_NAME_HDMI];
	} else if (pt == PRODUCT_BOX) {
		if (idsx == 0)
			interface_module[idsx].parallel_select = interface_name[INTERF_NAME_CVBS];
		else
			interface_module[idsx].parallel_select = interface_name[INTERF_NAME_HDMI];
	}

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display interface driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
