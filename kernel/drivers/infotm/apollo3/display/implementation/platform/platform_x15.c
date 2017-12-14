/*
 * platform_x15.c - x15 display platform driver
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
#define DSS_SUB1	"[platform]"
#define DSS_SUB2	""

extern int pt;
extern struct list_head path;

extern struct module_node osd_module;
extern struct module_node controller_module;
extern struct module_node lcdc_module;
extern struct module_node tvif_module;
extern struct module_node interface_module;
extern struct module_node cvbs_module;
extern struct module_node lcd_port_module;
extern struct module_node dev_module;

static int osd_types[] = {ATTR_MULTI_LAYER};

/* Only config optional attributes */
static void platform_attributes(void)
{
	set_attr(&osd_module,osd_types, sizeof(osd_types)/sizeof(int));
}

static void platform_link(void)
{
	/* INIT */
	INIT_LIST_HEAD(&path);
	INIT_LIST_HEAD(&osd_module.serial);
	INIT_LIST_HEAD(&osd_module.parallel);
	INIT_LIST_HEAD(&controller_module.serial);
	INIT_LIST_HEAD(&controller_module.parallel);
	INIT_LIST_HEAD(&interface_module.serial);
	INIT_LIST_HEAD(&interface_module.parallel);
	INIT_LIST_HEAD(&lcdc_module.serial);
	INIT_LIST_HEAD(&lcdc_module.parallel);
	INIT_LIST_HEAD(&tvif_module.serial);
	INIT_LIST_HEAD(&tvif_module.parallel);
	INIT_LIST_HEAD(&lcd_port_module.serial);
	INIT_LIST_HEAD(&lcd_port_module.parallel);

	/* serial */
	list_add_tail(&osd_module.serial, &path);
	list_add_tail(&controller_module.serial, &path);
	list_add_tail(&interface_module.serial, &path);

	/* parallel */
	list_add_tail(&lcdc_module.parallel, &controller_module.parallel);
	list_add_tail(&tvif_module.parallel, &controller_module.parallel);
	list_add_tail(&lcd_port_module.parallel, &interface_module.parallel);

#if 0
	INIT_LIST_HEAD(&cvbs_module.serial);
	INIT_LIST_HEAD(&cvbs_module.parallel);
	list_add_tail(&cvbs_module.parallel, &interface_module.parallel);
#endif

	INIT_LIST_HEAD(&dev_module.serial);
	INIT_LIST_HEAD(&dev_module.parallel);
	list_add_tail(&dev_module.parallel, &interface_module.parallel);

}

int platform_init(void)
{
	dss_trace("%s\n", __func__);
	
	platform_link();
	platform_attributes();
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display platform driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
