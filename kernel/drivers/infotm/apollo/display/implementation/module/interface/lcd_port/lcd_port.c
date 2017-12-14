/*
 * lcd_port.c - display lcd port driver
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


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[interface]"
#define DSS_SUB2	"[lcd port]"


int lcd_exist = 1;

static int get_rgb_bpp(int idsx, int *params, int num);
static int get_connect(int idsx, int *params, int num);




static int lcd_port_max_bpp[2] = {RGB888, RGB565};



static struct module_attr lcd_port_attr[] = {
	{TRUE, ATTR_GET_RGB_BPP, get_rgb_bpp},
	{TRUE, ATTR_GET_CONNECT, get_connect},
	{FALSE, ATTR_END, NULL},
};

struct module_node lcd_port_module = {
	.name = "lcd_port",
	.attributes = lcd_port_attr,
	.idsx = 1,
};

static int get_connect(int idsx, int *params, int num)
{
	assert_num(1);

	*params = lcd_exist; //always on

	return 0;
}

static int get_rgb_bpp(int idsx, int *params, int num)
{
	assert_num(1);
	dss_trace("ids%d\n", idsx);
	
	*params = lcd_port_max_bpp[idsx];
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display lcd port driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
