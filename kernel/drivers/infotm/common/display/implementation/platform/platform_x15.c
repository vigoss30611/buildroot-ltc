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

#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[platform]"
#define DSS_SUB2	""


extern int pt;
extern struct list_head path[2];

extern struct module_node osd_module[2];
extern struct module_node controller_module[2];
extern struct module_node lcdc_module[2];
extern struct module_node tvif_module[2];
extern struct module_node interface_module[2];
extern struct module_node hdmi_module;
extern struct module_node cvbs_module;
extern struct module_node lcd_port_module[2];


static int osd_types[] = {ATTR_MULTI_LAYER};


/* Only config optional attributes */
static int platform_attributes(void)
{
	int idsx;

	for (idsx = 0; idsx < 2; idsx++)
		set_attr(&osd_module[idsx],osd_types, sizeof(osd_types)/sizeof(int));

	return 0;
}

static int platform_link(void)
{
	int idsx;

	for (idsx = 0; idsx < 2; idsx++) {
		/* INIT */
		INIT_LIST_HEAD(&path[idsx]);
		INIT_LIST_HEAD(&osd_module[idsx].serial);
		INIT_LIST_HEAD(&osd_module[idsx].parallel);
		INIT_LIST_HEAD(&controller_module[idsx].serial);
		INIT_LIST_HEAD(&controller_module[idsx].parallel);
		INIT_LIST_HEAD(&interface_module[idsx].serial);
		INIT_LIST_HEAD(&interface_module[idsx].parallel);
		INIT_LIST_HEAD(&lcdc_module[idsx].serial);
		INIT_LIST_HEAD(&lcdc_module[idsx].parallel);
		INIT_LIST_HEAD(&tvif_module[idsx].serial);
		INIT_LIST_HEAD(&tvif_module[idsx].parallel);
		INIT_LIST_HEAD(&lcd_port_module[idsx].serial);
		INIT_LIST_HEAD(&lcd_port_module[idsx].parallel);
		/* serial */
		list_add_tail(&osd_module[idsx].serial, &path[idsx]);
		list_add_tail(&controller_module[idsx].serial, &path[idsx]);
		list_add_tail(&interface_module[idsx].serial, &path[idsx]);
		/* parallel */
		list_add_tail(&lcdc_module[idsx].parallel, &controller_module[idsx].parallel);
		list_add_tail(&tvif_module[idsx].parallel, &controller_module[idsx].parallel);
		list_add_tail(&lcd_port_module[idsx].parallel, &interface_module[idsx].parallel);
	}

	INIT_LIST_HEAD(&cvbs_module.serial);
	INIT_LIST_HEAD(&cvbs_module.parallel);
	list_add_tail(&cvbs_module.parallel, &interface_module[0].parallel);
	INIT_LIST_HEAD(&hdmi_module.serial);
	INIT_LIST_HEAD(&hdmi_module.parallel);
	list_add_tail(&hdmi_module.parallel, &interface_module[1].parallel);
	
	return 0;
}

int platform_init(void)
{
	dss_trace("~\n");
	
	platform_link();
	platform_attributes();
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display platform driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
