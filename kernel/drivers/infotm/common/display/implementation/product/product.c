/*
 * product.c - display product driver
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
#include <linux/string.h>
#include <dss_common.h>
#include "product.h"


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[product]"
#define DSS_SUB2	""



int pt;
int cvbs_style = 0;

int product_init(void)
{
	char name[64] = {0};
	dss_trace("~\n");

	if (!item_exist(P_PRODUCT_TYPE)) {
		dss_err("#item# Lack of product type item.\n");
		return -1;
	}
	
//ifdef CONFIG_MACH_IMAPX800
//	dss_dbg("CONFIG_MACH_IMAPX800 is TRUE.\n");
	
	item_string(name, P_PRODUCT_TYPE, 0);
	if (strcmp(name, "mid") == 0) {
		pt = PRODUCT_MID;
		product_x15_mid_init();
	}
	else if (strcmp(name, "box") == 0) {
		pt = PRODUCT_BOX;
		if (!item_exist(P_CVBS_STYLE)) {
			dss_err("#item# Lack of cvbs type item.\n");
			return -1;
		} else {
			item_string(name, P_CVBS_STYLE, 0);
			if (strcmp("pal", name) == 0) {
				cvbs_style = 0;
			} else if (strcmp("ntsc", name) == 0) {
				cvbs_style = 1;
			}
		}
		//	product_x15_box_init();
	}
	else if (strcmp(name, "dongle") == 0) {
		pt = PRODUCT_DONGLE;
		//product_x15_dongle_init();
	}
	else {
		dss_err("#item# Invalid product type item %s.\n", name);
		return -1;
	}

//endif

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display product driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
