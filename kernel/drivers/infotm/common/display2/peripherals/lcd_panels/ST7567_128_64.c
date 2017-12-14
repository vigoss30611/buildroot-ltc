/* display lcd panel params */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/display_device.h>
#include <mach/pad.h>

#include "register_apollo_i80.h"

#define DISPLAY_MODULE_LOG		"st7567"


static struct i80_init_param reg_init[] = {
	{ I80_CMD,     0xA2},
	{ I80_CMD,     0xC0},
	{ I80_CMD,     0xA0},
	{ I80_CMD,     0xA6},
	{ I80_CMD,     0x2F},
	{ I80_CMD,     0x27},
	{ I80_CMD,     0x81},
	{ I80_CMD,     0x23},
	{ I80_CMD,     0x40},
	{ I80_CMD,     0xAF},
	{ I80_DELAY,     10},
};

static uint32_t st7567_data_switch(uint32_t data, int mode)
{
	uint32_t tmp;

	switch (mode) {
		case I80IF_BUS8_1x_00_0_0:
			tmp = data;
			break;
		default:
			printk("Currently, not support this mode: %d\n", mode);
			break;
	}
	return tmp;
}

static void st7567_write_cmd(uint32_t cmd)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	cmd = st7567_data_switch(cmd, I80IF_BUS8_1x_00_0_0);
	i80_mannual_write_once(cmd);
	udelay(5);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static void st7567_write_param(uint32_t parameter)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	parameter = st7567_data_switch(parameter, I80IF_BUS8_1x_00_0_0);
	i80_mannual_write_once(parameter);
	udelay(5);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static int st7567_init(struct display_device * pdev)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(reg_init); i++)
		if (reg_init[i].type == I80_CMD)
			st7567_write_cmd(reg_init[i].data);
		else if (reg_init[i].type == I80_PARA)
			st7567_write_param(reg_init[i].data);
		else if (reg_init[i].type == I80_DELAY)
			mdelay(reg_init[i].data);

	return 0;	
}

static int st7567_enable(struct display_device *pdev, int en)
{
	i80_enable(1);
	if (en) {
		st7567_write_cmd(0xA4);
		st7567_write_cmd(0xAF);
	} else {
		st7567_write_cmd(0xAE);
		st7567_write_cmd(0xA5);
	}
	i80_enable(0);

	return 0;
}

static int st7567_manual_refresh(struct display_device *pdev, int mem)
{
	int i, j, k;
	char data;
	bool bit[8];
	char *buffer = (char *)mem;
//	struct timeval s, e;

	i80_enable(1);
//	do_gettimeofday(&s);
	for (i = 0; i < 8; i++) {
		st7567_write_cmd(0xb0+i);
		st7567_write_cmd(0x10);
		st7567_write_cmd(0x00);

		for (j = 0; j < 128;j++) {
			for (k = 0; k < 8; k++)
				bit[k] = (buffer[16*(i*8+k)+j/8] >> (7-j%8)) & 0x1;
			data = bit[0]|(bit[1]<<1)|(bit[2]<<2)|(bit[3]<<3)|(bit[4]<<4)|(bit[5]<<5)|(bit[6]<<6)|(bit[7]<<7);
			st7567_write_param(data);
		}

	}
	i80_enable(0);
//	do_gettimeofday(&e);
//	printk("use time %d ms\n", ((e.tv_sec*1000000+e.tv_usec)-(s.tv_sec*1000000+s.tv_usec))/1000);

	return 0;
}

struct peripheral_param_config ST7567_I80_800_480 = {
	.name = "ST7567_I80_800_480",
	.interface_type = DISPLAY_INTERFACE_TYPE_I80,
	.rgb_order = DISPLAY_LCD_RGB_ORDER_RGB,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 128,
		.mVActive = 64,
		.mHBlanking = 51, //HBP+HFP+HSPW 
		.mVBlanking = 16, 
		.mHSyncOffset = 2, //HFP 
		.mVSyncOffset = 2, //VFP 
		.mHSyncPulseWidth = 43, //HSPW 
		.mVSyncPulseWidth = 12, //VSPW 
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 1,
		.mPixelClock = 900.9,	// 60 fps, 60*(480+41+2+2)*(272+2+2+10)=60*525*286=9009000
		.mInterlaced = 0,
	},
	.i80_format = 5,
	.init = st7567_init,
	.enable = st7567_enable,
	.i80_refresh = st7567_manual_refresh,
};

MODULE_DESCRIPTION("display specific lcd panel param driver");
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_LICENSE("GPL v2");
