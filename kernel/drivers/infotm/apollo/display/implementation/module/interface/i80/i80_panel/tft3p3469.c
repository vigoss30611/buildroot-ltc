#include <linux/printk.h>
#include <mach/items.h>
#include <asm/delay.h>
#include <dss_item.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include "i80.h"
#include <dss_common.h>
#define DSS_LAYER       "[implementation]"
#define DSS_SUB1        "[interface]"
#define DSS_SUB2        "[tft3p3469]"


static struct dev_init reg_init[] = {
#if 0
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

static int tft3p3469_open(void)
{
	int reset, ret;

	dss_err("~~~~~ \n");
	if (item_exist(P_I80_RESET)) {
		reset = item_integer(P_I80_RESET, 0);                     
		if (gpio_is_valid(reset)) {                                
			ret = gpio_request(reset, "i80_reset");               
			if (ret)
			   return -1;	
		}                                                               

	}
	dss_err("~~~~~ reset pin %d\n", reset);
	printk("tft3p3469_open reset\n");	
	gpio_direction_output(reset, 1);
	mdelay(10);
	gpio_set_value(reset, 0);
	mdelay(20);
	gpio_set_value(reset, 1);
	return 0;
}

static int tft3p3469_close(void)
{
	return 0;
}

int tft3p3469_mannual_write(uint32_t *mem, int length, int mode)
{
	uint32_t i;
#if 0
	uint32_t tmp;
	uint32_t red, green, green1, blue;
#else
	uint32_t buf[2];
#endif

	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	switch (mode) {
		case I80IF_BUS16_10_11_x_x:
			i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
#if 0
			for (i = 0; i < length; i++) {
				red = (mem[i] & 0xff0000) >> (16 + 3);
				green = (mem[i] & 0x001f00) >> (8 + 2);
				green1 = (mem[i] & 0x00e000) >> (8 + 5);
				blue = (mem[i] & 0x0000ff) >> (0 + 3);
				tmp = (red << 13) | (green1 << 10)  | (green << 6) | (blue << 1);
				i80_mannual_write_once(tmp);
			}
#else
			for (i = 0; i < length; i++) {
				buf[1] = (mem[i] & 0xffff0000) >> 16;
				buf[0] = (mem[i] & 0x0000ffff);
				buf[1] = tft3p3469_data_switch(buf[1], I80IF_BUS16_10_11_x_x);
				buf[0] = tft3p3469_data_switch(buf[0], I80IF_BUS16_10_11_x_x);
				i80_mannual_write_once(buf[0]);
				i80_mannual_write_once(buf[1]);
			}
#endif
			break;
		default:
			printk("Now, not support this mode: %d\n", mode);
			break;
	}
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
	return 0;
}

static int tft3p3469_config(uint8_t *mem)
{
	int i;
	dss_err("~~~~~\n");
	printk("tft3p3469_config\n");
	for (i = 0; i < ARRAY_SIZE(reg_init); i++)
		if (reg_init[i].type == I80_CMD)
			tft3p3469_write_cmd(reg_init[i].data);
		else if (reg_init[i].type == I80_PARA)
			tft3p3469_write_param(reg_init[i].data);
		else if (reg_init[i].type == I80_DELAY)
			mdelay(reg_init[i].data);
	return 0;	
}

int tft3p3469_init(void)
{
	struct i80_dev *tft3p3469;

	dss_err("~~~~~\n");
	tft3p3469 = (struct i80_dev *)kmalloc(sizeof(struct i80_dev), GFP_KERNEL);
	if (!tft3p3469)
		return -1;

	tft3p3469->name = "TFT3P3469_220_176";
	tft3p3469->config = tft3p3469_config;
	tft3p3469->open = tft3p3469_open;
	tft3p3469->close = tft3p3469_close;
	tft3p3469->write = tft3p3469_mannual_write;

	return i80_register(tft3p3469);
}
