/*
 * dss_item.h - display common defination
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_ITEM_HEADER__
#define __DISPLAY_ITEM_HEADER__

#define IDS_LCD_INTERFACE		"lcd.interface"
#define IDS_PRESCALER_VIC		"prescaler.vic"
#define IDS_LCD_VIC				"lcd.vic"

#define IDS_CONFIG_UBOOT_LOGO	"config.uboot.logo"

#define IDS_LOGO_FB		"idslogofb"

#define IDS_INPUTFORMAT		"ids.inputformat"
#define IDS_PORT				"ids.port"

#define P_PRODUCT_TYPE		"dss.product.type"
#define P_BOARD_TYPE			"dss.board.type"
#define P_MAINFB_VIC			"dss.framebuffer.vic"

#define P_OSD0_RBEXG		"dss.osd0.rbexg"
#define P_OSD1_RBEXG		"dss.osd1.rbexg"
#define P_OSD1_COLORKEY		"dss.osd1.colorkey"
#define P_OSD1_ALPHA			"dss.osd1.alpha"

#define P_DEV_NAME		"dss.dev.name"
#define P_DEV_RESET	"dss.dev.reset"
#define P_SRGB_IFTYPE			"dss.srgb.iftype"
#define P_BT656_PAD_MODE "dss.bt656.padmode"

#define P_CVBS_NAME			"dss.cvbs.name"
#define P_CVBS_RESET			"dss.cvbs.reset"
#define P_CVBS_STYLE			"dss.cvbs.style"
#define P_CVBS_PA_EN			"dss.cvbs.pa.en"
#define P_CVBS_I2C_CTL			"dss.cvbs.i2c.ctl"

#define P_LCD_NAME				"dss.lcdpanel.name"
#define P_LCD_RGBSEQ			"dss.lcdpanel.rgbseq"
#define P_IDS0_RGB_ORDER		"dss.lcdpanel.ids0.rgborder"
#define P_IDS1_RGB_ORDER		"dss.lcdpanel.ids1.rgborder"
#define P_LCD_RGBBPP			"dss.lcdpanel.rgbbpp"

#define P_LCD_BL_GPIO			"dss.lcdpanel.bl.gpio"
#define P_LCD_BL_POLARITY		"dss.lcdpanel.bl.polarity"
#define P_LCD_BL_PWM			"dss.lcdpanel.bl.pwm"

#define P_LCD_PWREN_POLARITY	"dss.lcdpanel.pwren.polarity"

#define P_LCD_MASTER_GPIO		"dss.lcdpanel.master.gpio"
#define P_LCD_MASTER_POLARITY	"dss.lcdpanel.master.polarity"
#define P_LCD_MASTER_PMU		"dss.lcdpanel.master.pmu"

#define P_LCD_DVDD_GPIO		"dss.lcdpanel.dvdd.gpio"
#define P_LCD_DVDD_POLARITY	"dss.lcdpanel.dvdd.polarity"

#define P_LCD_AVDD_GPIO		"dss.lcdpanel.avdd.gpio"
#define P_LCD_AVDD_POLARITY	"dss.lcdpanel.avdd.polarity"

#define P_LCD_VGH_GPIO			"dss.lcdpanel.vgh.gpio"
#define P_LCD_VGH_POLARITY	"dss.lcdpanel.vgh.polarity"

#define P_LCD_VGL_GPIO			"dss.lcdpanel.vgl.gpio"
#define P_LCD_VGL_POLARITY	"dss.lcdpanel.vgl.polarity"

#endif
