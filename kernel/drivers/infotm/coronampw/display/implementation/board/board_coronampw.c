/*
 * board_mid_lcd1.c - x15 display mid board driver
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

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[board]"
#define DSS_SUB2	""

extern struct module_node lcd_panel_module;
extern struct module_node lcd_port_module;

static int board_attributes(void)
{
	return 0;
}

static int board_link(void)
{
	INIT_LIST_HEAD(&lcd_panel_module.serial);
	INIT_LIST_HEAD(&lcd_panel_module.parallel);

	/* serial */
	list_add_tail(&lcd_panel_module.serial, &lcd_port_module.serial);
	lcd_panel_module.idsx = 0;
	return 0;
}

int board_coronampw_init(void)
{
	dss_trace("%s\n", __func__);
	
	board_link();
	board_attributes();
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display mid board lcd1 driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
