/*
 * KR070LB0S.c - display specific lvds lcd panel driver
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
	{LCDPWR_MASTER_EN, 1},
	{LCDPWR_DVDD_EN, 5},
	{LCDPWR_AVDD_EN, 0},
	{LCDPWR_VGH_EN, 0},
	{LCDPWR_VGL_EN, 17},
	{LCDPWR_OUTPUT_EN, 100},
	{LCDPWR_BACKLIGHT_EN, 0},
};

struct lcd_panel_param panel_KR079LA1S_768_1024 = {
	.name = "KR079LA1S_768_1024",
	.dtd = {
		.mCode = LCD_VIC_KR079LA1S_768_1024,  //3003
		.mHImageSize = 120,	// mm. Different use with HDMI
		.mVImageSize = 160,
		.mHActive = 768,
		.mVActive = 1024,
		.mHBlanking = 160,
		.mVBlanking = 41,
		.mHSyncOffset = 40,
		.mVSyncOffset = 9,
		.mHSyncPulseWidth = 40,
		.mVSyncPulseWidth = 9,
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 0,
		.mPixelClock = 5930,	// 60 fps
	},
	.rgb_seq = SEQ_RGB,
	.rgb_bpp = RGB888,
	.power_seq = pwr_seq,
	.power_seq_num = sizeof(pwr_seq)/sizeof(struct lcd_power_sequence),
};



MODULE_DESCRIPTION("InfoTM iMAP display specific lcd panel driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
