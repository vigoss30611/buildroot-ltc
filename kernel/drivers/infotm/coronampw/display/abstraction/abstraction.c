/*
 * abstraction.c - display abstraction layer driver
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
#include <linux/slab.h>
#include <dss_common.h>
#include <implementation.h>

#define DSS_LAYER	"[abs]"
#define DSS_SUB1	""
#define DSS_SUB2	""

extern struct list_head path;
extern int pt;

static int abstraction_set_attribute(struct abstract_terminal * terminal,
		int attr, int *param, int num)
{
	struct module_node * module;
	int idsx = terminal->idsx;
	struct list_head * head = &path;
	int ret;
	dss_trace("terminal %s, ids%d, attr %d\n", terminal->name, idsx, attr);
	
	list_for_each_entry(module, head, serial) {
		dss_trace("serial iterate module %s\n", module->name);
		if (module->attr_ctrl) {
			ret = module->attr_ctrl(module, attr, param, num);
			if (ret) {
				dss_err("module %s attr %d set failed! \n", module->name, attr);
				return ret;
			}
		}
		else {
			dss_err("module %s null attr_ctrl.\n", module->name);
			return -1;
		}
	}

	return 0;
}
/* pesto code:

	abstraction layer function:
		* set ATTR_XXX with related terminal device.

*/

static struct abstract_terminal abstract_terminals[32] = {{0}};
static int abstract_terminals_num = 0;
static int on_ed = 0;

static int find_terminal_attributes(struct module_node * terminal, 
		struct abstract_terminal * ab_terminal)
{
	struct list_head * head = &path;
	struct module_node * ms;
	int attrs[128] = {0};
	int i, n = 0;
	dss_trace("terminal %s\n", terminal->name);

	/* collect attributes before controller module */
	list_for_each_entry(ms, head, serial) {
		if (strcmp(ms->name, "controller") == 0)
			break;
		if (ms->attributes) {
			for (i = 0; ms->attributes[i].type != ATTR_END; i++) {
				attrs[n] = ms->attributes[i].type;
				n++;
			}
		}
	}
	/* collect terminal device attributes */
	if (terminal->attributes) {
		for (i = 0; terminal->attributes[i].type != ATTR_END; i++) {
			attrs[n] = terminal->attributes[i].type;
			n++;
		}
	}

	/* allocate attrs memory */
	ab_terminal->attrs = kzalloc(sizeof(int) * n, GFP_KERNEL);
	for(i = 0; i < n; i++)
		ab_terminal->attrs[i] = attrs[i];

	ab_terminal->attr_num = n;

	return n;
}

static int find_terminals(struct module_node * intf, 
		struct module_node ** terminals)
{
	struct module_node * mp, *ms;
	int i = 0;
	dss_trace("ids%d, inft %s\n", intf->idsx, intf->name);
	
	list_for_each_entry(mp, &intf->parallel, parallel) {
		if (!list_empty(&mp->serial))
			list_for_each_entry(ms, &mp->serial, serial) {
				if (ms->terminal) {
					terminals[i] = ms;
					i++;
				}
			}
	}

	return i;
}

static int find_interface(struct list_head * head, struct module_node ** intf)
{
	struct module_node *ms;
	int intf_num = 0;
	dss_trace("~\n");
	
	list_for_each_entry(ms, head, serial) {
		if (strcmp(ms->name, "interface") == 0) {
			*intf = ms;
			intf_num++;
			intf++;
	//		return 0;
		}
	} 

	if (intf_num == 0){
		dss_err("interface module not found.\n");
		return -1;
	} else
		return intf_num;
}

int abstraction_init(void)
{
	struct module_node * intf[4];
	struct module_node * terminals[32] = {0};
	int i, j, tn = 0;
	int path_terminal_num;
	int intf_num = 0;
	struct module_attr * m_attr;
	dss_trace("~\n");

	intf_num = find_interface(&path, intf);
	if (intf_num == -1)
		return -1;

	for (i = 0; i < intf_num; i++) {
		path_terminal_num = find_terminals(intf[i], terminals);
		abstract_terminals_num += path_terminal_num;
		//if (path_terminal_num == 1 && !intf->parallel_select)
		//	intf->parallel_select = terminals[0]->name;			!!! Wrong !!!
		for (j = 0; j < path_terminal_num; j++) {
			abstract_terminals[tn].name = terminals[j]->name;
			abstract_terminals[tn].idsx = terminals[j]->idsx;
			/* set attributes */
			find_terminal_attributes(terminals[j], &abstract_terminals[tn]);
			/* rgb seq */
			if (module_attr_find(terminals[j], ATTR_GET_RGB_SEQ, &m_attr))
				m_attr->func(terminals[j]->idsx, &abstract_terminals[tn].rgb_seq, 1);
			else
				abstract_terminals[tn].rgb_seq = SEQ_RGB;
			/* rgb bpp */
			if (module_attr_find(terminals[j], ATTR_GET_RGB_BPP, &m_attr))
				m_attr->func(terminals[j]->idsx, &abstract_terminals[tn].rgb_bpp, 1);
			else
				abstract_terminals[tn].rgb_bpp = RGB888;
			/* rgb bt656 */
			if (module_attr_find(terminals[j], ATTR_GET_BT656, &m_attr))
				m_attr->func(terminals[j]->idsx, &abstract_terminals[tn].bt656, 1);
			else
				abstract_terminals[tn].bt656= 0;

			tn++;
		} 
	}

	return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++
 *					Temp  Code
 * +++++++++++++++++++++++++++++++++++++++++++++++++++
 */
static int enable_terminal(struct abstract_terminal * terminal, int vic)
{
	int param[8] = {0};
	dtd_t dtd;

	if (terminal->enabled && (terminal->vic == vic))
		return 0;

	param[0] = vic;
	if (abstraction_set_attribute(terminal, ATTR_VIC, param, 1))	// First call must be ATTR_VIC
		return -1;

	param[0] = 100;
	vic_fill(&dtd, vic, 60000);
	param[1] = dtd.mHActive/2;
	param[2] = dtd.mVActive/2;
	if (abstraction_set_attribute(terminal, ATTR_POSITION, param, 3))
		return -1;

	param[0] = RGB888;
	if (abstraction_set_attribute(terminal, ATTR_RGB_BPP, param, 1))
		return -1;

	//abstraction_set_attribute(terminal, ATTR_SET_RGB_SEQ, &terminal->rgb_seq, 1);
	//abstraction_set_attribute(terminal, ATTR_SET_RGB_BPP, &terminal->rgb_bpp, 1);
	//abstraction_set_attribute(terminal, ATTR_SET_BT656, &terminal->bt656, 1);

	/* enable */
	param[0] = 1;
	if (abstraction_set_attribute(terminal, ATTR_ENABLE, param, 1))
		return -1;

	on_ed = 1;
	terminal->enabled = 1;
	return 0;
}


static int disable_terminal(struct abstract_terminal * terminal, int vic)
{
	int param[8] = {0};

	if (!terminal->enabled)
		return 0;

	/* disable */
	param[0] = 0;
	if (abstraction_set_attribute(terminal, ATTR_ENABLE, param, 1))
		return -1;

	on_ed = 1;
	terminal->enabled = 0;
	return 0;
}


// TODO: Temp for lighten LCD panel
int terminal_configure(char * terminal_name, int vic, int enable)
{
	struct abstract_terminal * terminal_des;
	int i;
	dss_trace("%s~\n", (enable?"enable":"disable"));

	for (i = 0; i < abstract_terminals_num; i++) {
		if (strcmp(abstract_terminals[i].name, terminal_name) == 0) {
			terminal_des = &abstract_terminals[i];
			if (enable) 
				enable_terminal(terminal_des, vic);
			else
				disable_terminal(terminal_des, vic);
			terminal_des->vic = vic;
			dss_info("Terminal %s %s\n", terminal_name, (enable?"enabled":"disabled"));
			return 0;
		}
	}
	dss_err("Terminal %s not found.\n", terminal_name);
	return -1;
}

// temp for interaction with others
int get_gLCDStateOned(void)
{
	return on_ed;
}

MODULE_DESCRIPTION("InfoTM iMAP display abstraction layer driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
