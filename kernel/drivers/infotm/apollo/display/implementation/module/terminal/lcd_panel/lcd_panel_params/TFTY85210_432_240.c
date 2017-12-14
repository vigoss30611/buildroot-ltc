/*
 * KR070PB2S.c - display specific lcd panel driver
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

/* Lcd panel power sequence */
static struct lcd_power_sequence pwr_seq[] = {
	{LCDPWR_MASTER_EN, 0},
{LCDPWR_BACKLIGHT_EN, 30},
};

struct lcd_panel_param panel_Y85210_432_240 = {
	.name = "Y85210_432_240",
	.dtd = {
		.mCode = LCD_VIC_Y85210_432_240,
		.mHImageSize = 154,	// mm. Different use with HDMI
		.mVImageSize = 86,
		.mHActive = 432,
		.mVActive = 240,
		.mHBlanking = 3,//160,
		.mVBlanking = 3,//106,
		.mHSyncOffset = 1,//16,
		.mVSyncOffset = 1,//4,
		.mHSyncPulseWidth = 1,//96,
		.mVSyncPulseWidth = 1,//2,
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 1,
		.mPixelClock = 4800,//643,	// 60 fps
	},
	.rgb_seq = SEQ_RGB,
	.rgb_bpp = RGB888,
	.power_seq = pwr_seq,
	.power_seq_num = sizeof(pwr_seq)/sizeof(struct lcd_power_sequence),
};

MODULE_DESCRIPTION("InfoTM iMAP display specific lcd panel driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
