/*
 * implementation.c - display implementation layer driver
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
#include "implementation.h"


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	""
#define DSS_SUB2	""



struct list_head path;


int set_attr(struct module_node * module, int * types, int num)
{
	int i = 0, j;
	dss_trace("~\n");

	if (module->attributes == NULL)
		return 0;
	
	while(module->attributes[i].type != ATTR_END) {
		for (j = 0; j < num; j++) {
			if (module->attributes[i].type == types[j]) {
				module->attributes[i].exist = TRUE;
				break;
			}
		}
		/* Leave default setting if not found */
		i++;
		
		if (i >= 1024) {
			dss_err("Missing ATTR_END.\n");
			return -1;
		}
	}
	return 0;
}

static int modules_obtain(void)
{
	struct module_node *ms, *mp, *mss;

	dss_trace("%s\n", __func__);
	list_for_each_entry(ms, &path, serial) {
		if (ms->obtain_info)
			ms->obtain_info(1);
		if (!list_empty(&ms->parallel))
			list_for_each_entry(mp, &ms->parallel, parallel) {
				if (mp->obtain_info)
					mp->obtain_info(1);
				if (!list_empty(&mp->serial))
					list_for_each_entry(mss, &mp->serial, serial) {
						if (mss->obtain_info)
							mss->obtain_info(1);
					}
			}
	}
	return 0;
}


static int modules_init(void)
{
	struct module_node *ms, *mp, *mss;
		
	dss_dbg("%s\n", __func__);
	list_for_each_entry(ms, &path, serial) {
		dss_dbg("serial iterate module %s\n", ms->name);
		if (ms->init)
			ms->init(1);
		if (!list_empty(&ms->parallel))
			dss_dbg("non empty parallel.\n");
		list_for_each_entry(mp, &ms->parallel, parallel) {
			dss_dbg("parallel iterate module %s\n", mp->name);
			if (mp->init)
				mp->init(1);
			if (!list_empty(&mp->serial))
				dss_dbg("non empty serial.\n");
			list_for_each_entry(mss, &mp->serial, serial) {
				dss_dbg("serial iterate module %s\n", mss->name);
				if (mss->init)
					mss->init(1);
			}
		}
	}
	return 0;
}


/* @return	whether found */
int module_attr_find(struct module_node *module, int attr, struct module_attr **m_attr)
{
	int i = 0;
	dss_trace("module %s, attr %d\n", module->name, attr);
	
	if (!module->attributes)
		return 0;

	while(module->attributes[i].type != ATTR_END) {
		if (module->attributes[i].exist && module->attributes[i].type == attr) {
			*m_attr = &module->attributes[i];
			return 1;
		}
		i++;
		if (i >= 1024) {
			dss_err("Missing ATTR_END.\n");
			break;
		}
	}

	return 0;
}

int module_attr_ctrl(struct module_node *module, int attr, int *param, int num)
{
	struct module_attr *m_attr;
	struct module_node *mp, *ms;
	int ret;
	dss_trace("module %s, attr %d\n", module->name, attr);
	
	if (module_attr_find(module, attr, &m_attr)) {
		ret = m_attr->func(module->idsx, param, num);
		if (ret)
			return -1;
	}
	if (!list_empty(&module->parallel)) {
		list_for_each_entry(mp, &module->parallel, parallel) {
			if (strcmp(mp->name, module->parallel_select) == 0) {
				if (module_attr_find(mp, attr, &m_attr)) {
					ret = m_attr->func(mp->idsx, param, num);
					if (ret)
						return -1;
				}
				if (!list_empty(&mp->serial))
					list_for_each_entry(ms, &mp->serial, serial) {
						if (module_attr_find(ms, attr, &m_attr)) {
							ret = m_attr->func(ms->idsx, param, num);
							if (ret)
								return -1;
						}
					}
			}
		}
	}
	
	return 0;
}

int implementation_init(void)
{
	dss_trace("%s\n", __func__);
	platform_init();
	product_init();
	board_init();
	modules_init();
	modules_obtain();
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display implementation layer driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
