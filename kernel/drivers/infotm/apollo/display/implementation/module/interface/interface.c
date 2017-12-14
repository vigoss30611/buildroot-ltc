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
extern int board_type;
static char * interface_name[] = {"lcd_port", "cvbs", "hdmi","i80"};

enum interface_name_type {
	INTERF_NAME_LCD_PORT = 0,
	INTERF_NAME_CVBS,
	INTERF_NAME_HDMI,
	INTERF_NAME_I80,
};

struct module_node interface_module = {
	.name = "interface",
	.idsx = 1,
	.attr_ctrl = module_attr_ctrl,
	.init = dss_init_module,
	.parallel_select = "lcd_port",
};

//0 means lcd 1 means hdmi for now
void set_interface(int intf)
{
   int a=intf;
   switch(a){
	   case INTERF_NAME_LCD_PORT:	   
		interface_module.parallel_select = interface_name[INTERF_NAME_LCD_PORT];
	break;
	   case  INTERF_NAME_CVBS:
		interface_module.parallel_select = interface_name[INTERF_NAME_CVBS];
	break;
	   case INTERF_NAME_HDMI:
		interface_module.parallel_select = interface_name[INTERF_NAME_HDMI];
	break;	
	 case INTERF_NAME_I80:
		interface_module.parallel_select = interface_name[INTERF_NAME_I80];
	break;	
	   default:
		interface_module.parallel_select = interface_name[INTERF_NAME_LCD_PORT];
	break;
   }
   dss_info("Set interface -%s\n", intf == INTERF_NAME_LCD_PORT ?"LCD":\
		                          (intf == INTERF_NAME_CVBS ?"CVBS":\
								   (intf == INTERF_NAME_HDMI ?"HDMI":"ERROR")\
								  )\
								  );
}

static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	if (pt == PRODUCT_MID) {
		if (board_type == BOARD_I80)
			interface_module.parallel_select = interface_name[INTERF_NAME_I80];
		else if (board_type == BOARD_LCD1)
			interface_module.parallel_select = interface_name[INTERF_NAME_LCD_PORT];
//		interface_module[1].parallel_select = interface_name[INTERF_NAME_HDMI];
	}
	else if (pt == PRODUCT_BOX)
		interface_module.parallel_select = interface_name[INTERF_NAME_CVBS];
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display interface driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
