/*
 * cvbs_sink.c - display cvbs sink driver
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


#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[terminal]"
#define DSS_SUB2	"[cvbs_sink]"

static int get_connect(int idsx, int *params, int num);
//extern int hdmi_state;

static struct module_attr cvbs_sink_attr[] = {
	{TRUE, ATTR_GET_CONNECT, get_connect},
	{FALSE, ATTR_END, NULL},
};

struct module_node cvbs_sink_module = {
	.name = "cvbs_sink",
	.terminal = 0,
	.attributes = cvbs_sink_attr,
};

static int get_connect(int idsx, int *params, int num)
{
	assert_num(1);

	//dss_err("cvbs %d\n", !hdmi_state);
	//*params = !hdmi_state;

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display cvbs sink driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
