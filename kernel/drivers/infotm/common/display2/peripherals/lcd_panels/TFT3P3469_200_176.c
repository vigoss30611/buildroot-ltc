/* display lcd panel params */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/display_device.h>
#include <mach/pad.h>

#include "register_apollo_i80.h"

#define DISPLAY_MODULE_LOG		"tft3p3469"


static struct i80_init_param reg_init[] = {
#if 1
	{ I80_CMD,		0x0000 },
	{ I80_CMD,		0x0000 },
	{ I80_CMD,		0x0000 },
	{ I80_CMD,		0x0000 },
	{ I80_CMD,		0x00A4 },
	{ I80_PARA,		0x0001 },
	{ I80_DELAY,	0x0010 },
	{ I80_CMD,      0x0060 },
	{ I80_PARA,     0x2700 },
	{ I80_CMD,      0x0008 },
	{ I80_PARA,     0x8080 },	//0x8080
	{ I80_CMD,      0x0030 },
	{ I80_PARA,     0x0900 },
	{ I80_CMD,      0x0031 },
	{ I80_PARA,     0x5b15 },
	{ I80_CMD,      0x0032 },
	{ I80_PARA,     0x0603 },
	{ I80_CMD,      0x0033 },
	{ I80_PARA,     0x1a01 },
	{ I80_CMD,      0x0034 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0035 },
	{ I80_PARA,     0x011a },
	{ I80_CMD,      0x0036 },
	{ I80_PARA,     0x5306 },
	{ I80_CMD,      0x0037 },
	{ I80_PARA,     0x150b },
	{ I80_CMD,      0x0038 },
	{ I80_PARA,     0x0009 },
	{ I80_CMD,      0x0039 },
	{ I80_PARA,     0x3333 },
	{ I80_CMD,      0x0090 },
	{ I80_PARA,     0x0017 },
	{ I80_CMD,      0x000a },
	{ I80_PARA,     0x0008 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0010 },
	{ I80_PARA,     0x15b0 },
	{ I80_DELAY,    0x0100 },
	{ I80_CMD,      0x0011 },
	{ I80_PARA,     0x0247 },
	{ I80_DELAY,    0x0100 },
	{ I80_CMD,      0x000e },
	{ I80_PARA,     0x0020 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0010 },
	{ I80_PARA,     0x15b0 },
	{ I80_DELAY,    0x0100 },
	{ I80_CMD,      0x0011 },
	{ I80_PARA,     0x0247 },
	{ I80_DELAY,    0x0100 },
	{ I80_CMD,      0x000e },
	{ I80_PARA,     0x0020 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0013 },
	{ I80_PARA,     0x1a00 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x002a },
	{ I80_PARA,     0x005a },
	{ I80_CMD,      0x0012 },
	{ I80_PARA,     0x019a },
	{ I80_DELAY,    0x0050 },
	{ I80_CMD,      0x0012 },
	{ I80_PARA,     0x01b8 },
	{ I80_DELAY,    0x0200 },
	{ I80_CMD,      0x0050 },
	{ I80_PARA,     0x000a },
	{ I80_CMD,      0x0051 },
	{ I80_PARA,     0x00e5 },
	{ I80_CMD,      0x0052 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0053 },
	{ I80_PARA,     0x00af },
	{ I80_CMD,      0x0061 },
	{ I80_PARA,     0x0001 },
	{ I80_CMD,      0x006a },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0080 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0081 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0082 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0092 },
	{ I80_PARA,     0x0300 },
	{ I80_CMD,      0x0093 },
	{ I80_PARA,     0x0005 },
	{ I80_CMD,      0x0001 },
	{ I80_PARA,     0x0100 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0002 },
	{ I80_PARA,     0x0200 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0003 },
	{ I80_PARA,     0x1030 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x000c },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x000f },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,      0x0020 },
	{ I80_PARA,     0x000a },
	{ I80_CMD,      0x0021 },
	{ I80_PARA,     0x0000 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x0007 },
	{ I80_PARA,     0x0100 },
	{ I80_CMD,      0x00ab },
	{ I80_PARA,     0x0010 },
	{ I80_CMD,      0x0013 },
	{ I80_PARA,     0x1500 },
	{ I80_DELAY,    0x0020 },
	{ I80_CMD,      0x002a },
	{ I80_PARA,     0x0058 },
	{ I80_DELAY,    0x0080 },
	{ I80_CMD,      0x0022 },
#else
	{ I80_CMD,       0x0600 },
	{ I80_PARA,       0x0001 },
	{ I80_DELAY,      0x0150 },
	{ I80_CMD,      0x0150 },
	{ I80_PARA,      0x0000 },
	{ I80_CMD,      0x0001 },
	{ I80_PARA,     0x0100 },
	{ I80_CMD,     0x0002 },
	{ I80_PARA,     0x0100 },
	{ I80_CMD,     0x0003 },
	{ I80_PARA,     0x1230 },
	{ I80_CMD,     0x0006 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,     0x0008 },
	{ I80_PARA,     0x0202 },
	{ I80_CMD,     0x0009 },
	{ I80_PARA,     0x0001 },
	{ I80_CMD,     0x000B },
	{ I80_PARA,     0x0010 },
	{ I80_CMD,     0x000C },
	{ I80_PARA,     0x0111 },
	{ I80_CMD,     0x000F },
	{ I80_PARA,     0x0003 },
	{ I80_CMD,     0x0010 },
	{ I80_PARA,     0x0016 },
	{ I80_CMD,     0x0020 },
	{ I80_PARA,     0x0110 },
	{ I80_CMD,     0x0011 },
	{ I80_PARA,     0x0101 },
	{ I80_CMD,     0x0012 },
	{ I80_PARA,     0x0307 },
	{ I80_CMD,     0x0400 },
	{ I80_PARA,     0x3500 },
	{ I80_CMD,     0x0401 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,     0x0300 },
	{ I80_PARA,     0x0706 },
	{ I80_CMD,     0x0301 },
	{ I80_PARA,     0x0206 },
	{ I80_CMD,     0x0302 },
	{ I80_PARA,     0x0007 },
	{ I80_CMD,     0x0303 },
	{ I80_PARA,     0x0201 },
	{ I80_CMD,     0x0304 },
	{ I80_PARA,     0x0306 },
	{ I80_CMD,     0x0305 },
	{ I80_PARA,     0x0405 },
	{ I80_CMD,     0x0306 },
	{ I80_PARA,     0x1F1F },
	{ I80_CMD,     0x0307 },
	{ I80_PARA,     0x0506 },
	{ I80_CMD,     0x0308 },
	{ I80_PARA,     0x0206 },
	{ I80_CMD,     0x0309 },
	{ I80_PARA,     0x0002 },
	{ I80_CMD,     0x030A },
	{ I80_PARA,     0x0301 },
	{ I80_CMD,     0x030B },
	{ I80_PARA,     0x0300 },
	{ I80_CMD,     0x030C },
	{ I80_PARA,     0x0207 },
	{ I80_CMD,     0x030D },
	{ I80_PARA,     0x090B },
	{ I80_CMD,     0x0210 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,     0x0211 },
	{ I80_PARA,     0x00EF },
	{ I80_CMD,     0x0212 },
	{ I80_PARA,     0x0000 },
	{ I80_CMD,     0x0213 },
	{ I80_PARA,     0x01AF },
	{ I80_CMD,     0x0007 },
	{ I80_PARA,     0x0001 },
	{ I80_CMD,     0x0110 },
	{ I80_PARA,     0x0001 },
	{ I80_CMD,     0x0112 },
	{ I80_PARA,     0x0060 },
	{ I80_CMD,    0x0100 },
	{ I80_PARA,     0x10B0 },
	{ I80_DELAY,     0x0100 },
	{ I80_CMD,     0x0101 },
	{ I80_PARA,     0x0007 },
	{ I80_DELAY,     0x0100 },
	{ I80_CMD,     0x0102 },
	{ I80_PARA,     0x011D },
	{ I80_CMD,     0x0103 },
	{ I80_PARA,     0x3900 },
	{ I80_CMD,     0x0282 },
	{ I80_PARA,     0x0000 },
	{ I80_DELAY,     0x0100 },
	{ I80_CMD,     0x0281 },
	{ I80_PARA,     0x001A },
	{ I80_CMD,     0x0110 },
	{ I80_PARA,     0x0001 },
	{ I80_DELAY,     0x0010 },
	{ I80_CMD,     0x0102 },
	{ I80_PARA,     0x01BD },
	{ I80_DELAY,     0x0100 },
	{ I80_CMD,     0x0100 },
	{ I80_PARA,     0x10B0 },
	{ I80_CMD,     0x0101 },
	{ I80_PARA,     0x0007 },
	{ I80_CMD,     0x0007 },
	{ I80_PARA,     0x0061 },
	{ I80_DELAY,     0x0300 },
	{ I80_CMD,     0x0173 },
	{ I80_PARA,     0x0061 },
	{ I80_DELAY,     0x0300 },
	{ I80_CMD,     0x0202 },
#endif
};

static uint32_t tft3p3469_data_switch(uint32_t data, int mode)
{
	uint32_t tmp = -1;
	uint32_t highbyte, lowbyte;

	switch (mode) {
		case I80IF_BUS16_10_11_x_x:
			highbyte = (data & 0xff00) << 2;
			lowbyte = (data & 0x00ff) << 1;
			tmp = highbyte | lowbyte;
			break;
		default:
			printk("Now, not support this mode: %d\n", mode);
			break;
	}
	return tmp;
}

static void tft3p3469_write_cmd(uint32_t cmd)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	cmd = tft3p3469_data_switch(cmd, I80IF_BUS16_10_11_x_x);
	i80_mannual_write_once(cmd);
	udelay(10);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static void tft3p3469_write_param(uint32_t parameter)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	parameter = tft3p3469_data_switch(parameter, I80IF_BUS16_10_11_x_x);
	i80_mannual_write_once(parameter);
	udelay(10);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static int tft3p3469_init(struct display_device * pdev)
{
	struct i80_cmd cmd;
	int i;
	dlog_trace();
	for (i = 0; i < ARRAY_SIZE(reg_init); i++)
		if (reg_init[i].type == I80_CMD)
			tft3p3469_write_cmd(reg_init[i].data);
		else if (reg_init[i].type == I80_PARA)
			tft3p3469_write_param(reg_init[i].data);
		else if (reg_init[i].type == I80_DELAY)
			mdelay(reg_init[i].data);

	i80_enable(0);
	cmd.num = 0;
	cmd.cmd = 0x0022 << 1;
	cmd.type = CMD_NORMAL_AUTO;
	cmd.rs_ctrl = I80_ACTIVE_LOW;
	i80_set_cmd(&cmd);
	i80_enable(1);

	return 0;	
}

struct peripheral_param_config TFT3P3469_220_176 = {
	.name = "TFT3P3469_220_176",
	.interface_type = DISPLAY_INTERFACE_TYPE_I80,
	.rgb_order = DISPLAY_LCD_RGB_ORDER_RGB,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 220,
		.mVActive = 176,
		.mHBlanking = 160,
		.mVBlanking = 106,
		.mHSyncOffset = 16,
		.mVSyncOffset = 4,
		.mHSyncPulseWidth = 96,
		.mVSyncPulseWidth = 2,
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 1,
		.mPixelClock = 6430,	// 60 fps
		.mInterlaced = 0,
	},
	.i80_format = 4,
	.init = tft3p3469_init,
};

MODULE_DESCRIPTION("display specific lcd panel param driver");
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_LICENSE("GPL v2");
