/*
 * hdmi.c - display hdmi controller driver
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
#include <linux/switch.h>

static int hdmi_state= 0;

static ssize_t
hdmi_audio_name(struct switch_dev *dev, char *buf) {
	return sprintf(buf, "%s\n", "hdmi_audio");
}

static ssize_t
hdmi_name(struct switch_dev *dev, char *buf) {
	return sprintf(buf, "%s\n", "hdmi");
}

static ssize_t
hdmi_get_state(struct switch_dev *dev, char *buf) {
	return sprintf(buf, "%d\n", hdmi_state);
}

static struct switch_dev hdmi_switch = {
	.name  = "hdmi",
	.print_name = hdmi_name,
	.print_state = hdmi_get_state,
}, hdmi_audio_switch = {
	.name  = "hdmi_audio",
	.print_name = hdmi_audio_name,
	.print_state = hdmi_get_state,
};

int __init hdmi_sysif_init(void) {
	switch_dev_register(&hdmi_switch);
	switch_dev_register(&hdmi_audio_switch);

	return 0;
}

void hdmi_update_state(int state) {
	hdmi_state = !!state;
	switch_set_state(&hdmi_switch, hdmi_state);
//	switch_set_state(&hdmi_audio_switch, hdmi_state);
}
EXPORT_SYMBOL(hdmi_update_state);

void hdmi_audio_update_state(int state) {
	switch_set_state(&hdmi_audio_switch, hdmi_state);
}
EXPORT_SYMBOL(hdmi_audio_update_state);

module_init(hdmi_sysif_init);

MODULE_DESCRIPTION("InfoTMIC HDMI audio /sys interface");
MODULE_LICENSE("GPL v2");

