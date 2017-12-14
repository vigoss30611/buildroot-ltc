#include <linux/printk.h>
#include <mach/items.h>
#include <asm/delay.h>
#include <dss_item.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include "i80.h"
#include <dss_common.h>
#include <ids_access.h>
#define DSS_LAYER       "[implementation]"
#define DSS_SUB1        "[interface]"
#define DSS_SUB2        "[st7789s]"


static struct dev_init reg_init[] = {
	{ I80_CMD,       0x11},
	{ I80_DELAY,      0x120 },
	{ I80_CMD,       0x36},
	{ I80_PARA,      0x00},
	{ I80_CMD,      0x3A},
	{ I80_PARA,      0x06 },
	{ I80_CMD,      0xB2 },
	{ I80_PARA,     0x0C },
	{ I80_PARA,     0x0C },
	{ I80_PARA,     0x00 },
	{ I80_PARA,     0x33 },
	{ I80_PARA,     0x33 },
	{ I80_CMD,     0xB7},
	{ I80_PARA,     0x35},	//VCOM
	{ I80_CMD,     0xC0 },
	{ I80_PARA,     0x2C},
	{ I80_CMD,     0xC2 },
	{ I80_PARA,     0x01 },
	{ I80_CMD,     0xC3 },
	{ I80_PARA,     0x11 },
	{ I80_CMD,     0xC4 },
	{ I80_PARA,     0x20 },
	{ I80_CMD,     0xC6 },
	{ I80_PARA,     0x0F },
	{ I80_CMD,     0xD0 },
	{ I80_PARA,     0xA7 },
	{ I80_PARA,     0xA1 },

	{ I80_CMD,     0xE0},
	{ I80_PARA,     0xD0 },
	{ I80_PARA,     0x00 },
	{ I80_PARA,     0x06 },
	{ I80_PARA,     0x09 },
	{ I80_PARA,     0x0B },
	{ I80_PARA,     0x2A },
	{ I80_PARA,     0x3C },
	{ I80_PARA,     0x55 },
	{ I80_PARA,     0x4B },
	{ I80_PARA,     0x08 },
	{ I80_PARA,     0x16 },
	{ I80_PARA,     0x14 },
	{ I80_PARA,     0x19 },
	{ I80_PARA,     0x20 },

	{ I80_CMD,     0xE1},
	{ I80_PARA,     0xD0},
	{ I80_PARA,     0x00},
	{ I80_PARA,     0x06},
	{ I80_PARA,     0x09},
	{ I80_PARA,     0x0B},
	{ I80_PARA,     0x29},
	{ I80_PARA,     0x36},
	{ I80_PARA,     0x54},
	{ I80_PARA,     0x4B},
	{ I80_PARA,     0x0D},
	{ I80_PARA,     0x16},
	{ I80_PARA,     0x14},
	{ I80_PARA,     0x21},
	{ I80_PARA,     0x20},

	{ I80_CMD,     0x29},
	{ I80_CMD,     0x2C},
};

static uint32_t st7789s_data_switch(uint32_t data, int mode)
{
	uint32_t tmp = -1;
	uint32_t highbyte, lowbyte;

	switch (mode) {
		case I80IF_BUS8_1x_00_0_0:
			tmp = data;
			break;
		default:
			printk("Now, not support this mode: %d\n", mode);
			break;
	}
	return tmp;
}

static void st7789s_write_cmd(uint32_t cmd)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	cmd = st7789s_data_switch(cmd, I80IF_BUS8_1x_00_0_0);
	i80_mannual_write_once(cmd);
	udelay(10);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static void st7789s_write_param(uint32_t parameter)
{
	i80_mannual_init();
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_HIGH);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_LOW);
	parameter = st7789s_data_switch(parameter, I80IF_BUS8_1x_00_0_0);
	i80_mannual_write_once(parameter);
	udelay(10);
	i80_mannual_ctrl(I80_MAN_RS, I80_ACTIVE_LOW);
	i80_mannual_ctrl(I80_MAN_CS0, I80_ACTIVE_HIGH);
	i80_mannual_deinit();
}

static int st7789s_open(void)
{
	int reset, ret;

	if (item_exist(P_I80_RESET)) {
		reset = item_integer(P_I80_RESET, 0);                     
		if (gpio_is_valid(reset)) {                                
			ret = gpio_request(reset, "i80_reset");               
			if (ret)
			   return -1;	
		}                                                               

	}
	gpio_direction_output(reset, 1);
	mdelay(10);
	gpio_set_value(reset, 0);
	mdelay(50);
	gpio_set_value(reset, 1);
	mdelay(120);
	return 0;
}

static int st7789s_close(void)
{
	return 0;
}

int st7789s_mannual_write(uint32_t *mem, int length, int mode)
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
		case I80IF_BUS8_1x_00_0_0:
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
				buf[0] = st7789s_data_switch(buf[0], I80IF_BUS8_1x_00_0_0);
				i80_mannual_write_once(buf[0]);
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

static int st7789s_config(uint8_t *mem)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(reg_init); i++)
		if (reg_init[i].type == I80_CMD)
			st7789s_write_cmd(reg_init[i].data);
		else if (reg_init[i].type == I80_PARA)
			st7789s_write_param(reg_init[i].data);
		else if (reg_init[i].type == I80_DELAY)
			mdelay(reg_init[i].data);
	return 0;	
}

int st7789s_init(void)
{
	struct i80_dev *st7789s;

	st7789s= (struct i80_dev *)kmalloc(sizeof(struct i80_dev), GFP_KERNEL);
	if (!st7789s)
		return -1;

	st7789s->name = "ST7789S_240_320";
	st7789s->config = st7789s_config;
	st7789s->open = st7789s_open;
	st7789s->close = st7789s_close;
	st7789s->write = st7789s_mannual_write;

	return i80_register(st7789s);
}
