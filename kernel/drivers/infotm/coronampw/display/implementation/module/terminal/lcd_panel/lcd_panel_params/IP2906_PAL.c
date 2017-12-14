/*
 * IP2906_PAL.c - display specific lcd panel driver
 *
 * Copyright (c) 2016 davinci.liang <davinci.liang@infotm.com>
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

/* Lcd panel power sequence */
static struct lcd_power_sequence pwr_seq[] = {
	{LCDPWR_MASTER_EN, 1},
};

struct lcd_panel_param panel_IP2906_PAL = {
	.name =IP2906_I2C_DEVICE_NAME,
	.dtd = {
		.mCode = LCD_VIC_IP2906_PAL,
		.mHActive = 720,
		.mVActive = 576,
		.mHBlanking = 144,
		.mVBlanking = 49,
		.mHSyncOffset = 12,
		.mVSyncOffset = 5,
		.mHSyncPulseWidth = 64,
		.mVSyncPulseWidth = 5,
		.mHSyncPolarity =  0,
		.mVSyncPolarity = 0,
		.mInterlaced = 0,
		.mPixelClock = 2700,
	},
	.rgb_seq = SEQ_RGB,
	.rgb_bpp = RGB888,
	.power_seq = pwr_seq,
	.power_seq_num = sizeof(pwr_seq)/sizeof(struct lcd_power_sequence),

};

MODULE_DESCRIPTION("InfoTM iMAP display specific lcd panel driver");
MODULE_AUTHOR("davinci.liang <davinci.liang@infotm.com>");
MODULE_LICENSE("GPL v2");

