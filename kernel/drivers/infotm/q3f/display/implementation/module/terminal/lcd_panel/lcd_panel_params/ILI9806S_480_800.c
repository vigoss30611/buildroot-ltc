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
	{LCDPWR_MASTER_EN, 1},		 // LCD_BEN, K17, GPIO119, controls main power of LCD.
//	{LCDPWR_DVDD_EN, 30},
//	{LCDPWR_AVDD_EN, 20},
//	{LCDPWR_OUTPUT_EN, 1},		// 10 VSD, 317ms
//	{LCDPWR_BACKLIGHT_EN, 250},
	{LCDPWR_BACKLIGHT_EN, 30},	// LCD_BL_EN, GPIO109, controls LED+ and LED- power of LCD
};

struct lcd_panel_param panel_ILI9806S_480_800 = {
	.name = "ILI9806S_480_800",
	.dtd = {
		.mCode = LCD_VIC_ILI9806S_480_800,
		.mHImageSize = 86,//154,	// mm. Different use with HDMI
		.mVImageSize = 154,//86,
		.mHActive = 480,//800,
		.mVActive = 800,//480,
		.mHBlanking = 47,//45,//256,
		.mVBlanking = 156,//256,//45,
		.mHSyncOffset = 24,//22,//210,
		.mVSyncOffset = 108,//210,//22,
		.mHSyncPulseWidth = 20,//30,
		.mVSyncPulseWidth = 29,//13,
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 1,
		.mPixelClock = 3326,//3000,//3493,	// 60 fps
	},
	.rgb_seq = SEQ_RGB,
	.rgb_bpp = RGB888,
	.power_seq = pwr_seq,
	.power_seq_num = sizeof(pwr_seq)/sizeof(struct lcd_power_sequence),
};



MODULE_DESCRIPTION("InfoTM iMAP display specific lcd panel driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
