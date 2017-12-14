#include <linux/kernel.h>
#include <linux/module.h>
#include <dss_common.h>


/* Lcd panel power sequence */
static struct lcd_power_sequence pwr_seq[] = {
	{LCDPWR_MASTER_EN, 1},		 // LCD_BEN, K17, GPIO119, controls main power of LCD.
	{LCDPWR_BACKLIGHT_EN, 30},	// LCD_BL_EN, GPIO109, controls LED+ and LED- power of LCD
};

struct lcd_panel_param panel_ILI8961_360_240 = {
	.name =ILI8961_SPI_DEVICE_NAME,
	.dtd = {
		.mCode = LCD_VIC_ILI8961_360_240,
		.mPixelClock = 2708,
		.mHActive = 360,
		.mVActive = 240,
		.mHBlanking = 276,
		.mVBlanking = 23,
		.mHSyncOffset = 35,
		.mVSyncOffset = 2,
		.mHSyncPulseWidth = 126,
		.mVSyncPulseWidth = 8,

		/*Note: set ! LCDCON5 value in lcdc module, so set zero in here, i don't know why do that.*/
		.mHSyncPolarity = 0,
		.mVSyncPolarity = 0,
		.mVclkInverse = 0,
	},
	.rgb_seq = SEQ_RGB, /*Note: rgb order */
	.rgb_bpp = RGB888,
	.power_seq = pwr_seq,
	.power_seq_num = sizeof(pwr_seq)/sizeof(struct lcd_power_sequence),
};

MODULE_DESCRIPTION("InfoTM iMAP display specific lcd panel driver");
MODULE_AUTHOR("Davinci Liang, <davinci.liang@infotm.com>");
MODULE_LICENSE("GPL v2");
