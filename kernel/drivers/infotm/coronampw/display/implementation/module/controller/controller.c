/*
 * controller.c - display controller driver
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
#include "controller.h"

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[controller]"
#define DSS_SUB2	""

static int set_bt656(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);

static int terminal_bt656[2] = {0};
enum controller_name_type {
	CTRL_NAME_LCDC = 0,
	CTRL_NAME_TVIF,
	CTRL_NAME_I80,
};
static char * controller_name[] = {"lcdc", "tvif", "i80"};

static struct module_attr controller_attr[] = {
	{TRUE, ATTR_SET_BT656, set_bt656},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node controller_module = {
	.name = "controller",
	.attr_ctrl = module_attr_ctrl,
	.attributes = controller_attr,
	.idsx = 0,
	.init = dss_init_module,
	.parallel_select = "lcdc"
};

//choose controller tyoe
//0 means lcdc 1 means tvif for now
void set_controller(int con)
{
	switch(con){
	case DSS_INTERFACE_LCDC:
		controller_module.parallel_select = controller_name[CTRL_NAME_LCDC];
		break;
	case DSS_INTERFACE_SRGB:
		controller_module.parallel_select = controller_name[CTRL_NAME_LCDC];
		break;
	case DSS_INTERFACE_TVIF:
		controller_module.parallel_select = controller_name[CTRL_NAME_TVIF];
		break;
	case DSS_INTERFACE_I80:
		controller_module.parallel_select = controller_name[CTRL_NAME_I80];
		break;
	default:
		break;
	}
	dss_info("Set controller -%s , con=%d\n", con == DSS_INTERFACE_LCDC ?"LCDC":\
		(con == DSS_INTERFACE_TVIF ?"TVIF":\
		(con == DSS_INTERFACE_I80 ?"I80":"ERROR")\
		),con);

}

static int set_bt656(int idsx, int *params, int num)
{
	assert_num(1);
	dss_trace("ids%d, bt656 %d\n", idsx, *params);
	terminal_bt656[idsx] = *params;
	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	dtd_t dtd;
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);
	
	vic_fill(&dtd, vic, 60000);

	if (terminal_bt656[idsx] && dtd.mInterlaced) {
		dss_err("bt656 terminal does not support interlace vic.\n");
		return -1;
	}
	return 0;
}

static int dss_init_module(int idsx)
{
	int t = 0;
	dss_trace("ids%d\n", idsx);
	ids_irq_initialize(idsx);
	
	if(item_exist("lcd.interface")) {
		t = item_integer("lcd.interface",0); 
		printk(KERN_INFO "get lcd.interface %d!\n",t);
	}else{
		printk(KERN_INFO "error lcd.interface is not exist \n");
	}

	switch(t){
	case DSS_INTERFACE_LCDC://lcdc rgb_parallel
		controller_module.parallel_select = controller_name[CTRL_NAME_LCDC];
		core.lcd_interface = DSS_INTERFACE_LCDC;
		printk(KERN_INFO "Init controller interface %s ,lcd_interface=%d\n",
			"LCDC",core.lcd_interface);
		break;

	case DSS_INTERFACE_SRGB://lcdc srgb
		controller_module.parallel_select = controller_name[CTRL_NAME_LCDC];
		core.lcd_interface = DSS_INTERFACE_SRGB;
		printk(KERN_INFO "Init controller interface %s ,lcd_interface=%d\n",
			"SRGB",core.lcd_interface);
		break;

	case DSS_INTERFACE_TVIF://tvif
		controller_module.parallel_select = controller_name[CTRL_NAME_TVIF];
		core.lcd_interface = DSS_INTERFACE_TVIF;
		printk(KERN_INFO "Init controller interface %s ,lcd_interface=%d\n",
			"TVIF",core.lcd_interface);
		break;

	case DSS_INTERFACE_I80://I80
		controller_module.parallel_select = controller_name[CTRL_NAME_I80];
		core.lcd_interface = DSS_INTERFACE_I80;
		printk(KERN_INFO "Init controller interface %s ,lcd_interface=%d\n",
			"I80",core.lcd_interface);
		break;

	case DSS_INTERFACE_HDMI://hdmi
	case DSS_INTERFACE_DSI://mipi_dsi
		/* TODO */
	default:
		dss_err("Init controller  interlace error,lcd_interface = %d.\n",t);
		break;
	}

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
