#include <linux/printk.h>
#include <mach/items.h>
#include <asm/delay.h>
#include <dss_item.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/slab.h>
#include "i80.h"
#include <dss_common.h>
#include <ids_access.h>
#define DSS_LAYER       "[implementation]"
#define DSS_SUB1        "[interface]"
#define DSS_SUB2        "[st7567]"
#include "kernel_logo.h"


static struct dev_init reg_init[] = {
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

static int st7567_open(void)
{
	int reset, ret;

	if (item_exist(P_I80_RESET)) {
		reset = item_integer(P_I80_RESET, 0);                     
		if (gpio_is_valid(reset)) {                                
			ret = gpio_request(reset, "i80_reset");               
			if (ret)
			   return -1;	
		}                                                               
		gpio_direction_output(reset, 1);
		mdelay(5);
		gpio_set_value(reset, 0);
		mdelay(10);
		gpio_set_value(reset, 1);
		mdelay(5);
		gpio_free(reset);
	}
	return 0;
}

static int st7567_close(void)
{
	return 0;
}

static int st7567_enable(int en)
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
}

int st7567_mannual_write(uint32_t *mem, int length, int mode)
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
				buf[0] = st7567_data_switch(buf[0], I80IF_BUS8_1x_00_0_0);
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

static int *mem_addr = NULL;
void manual_sync(char *mem)
{
	int i, j, k, m;
	char data;
	bool bit[8];
	struct timeval s, e;

	i80_enable(1);
//	do_gettimeofday(&s);
	for (i = 0; i < 8; i++) {
		st7567_write_cmd(0xb0+i);
		st7567_write_cmd(0x10);
		st7567_write_cmd(0x00);

		for (j = 0; j < 128;j++) {
			for (k = 0; k < 8; k++)
				bit[k] = (mem[16*(i*8+k)+j/8] >> (7-j%8)) & 0x1;
			data = bit[0]|(bit[1]<<1)|(bit[2]<<2)|(bit[3]<<3)|(bit[4]<<4)|(bit[5]<<5)|(bit[6]<<6)|(bit[7]<<7);
			st7567_write_param(data);
		}

	}
	i80_enable(0);
	mem_addr = mem;
//	do_gettimeofday(&e);
//	printk("use time %d ms\n", ((e.tv_sec*1000000+e.tv_usec)-(s.tv_sec*1000000+s.tv_usec))/1000);
}

static int st7567_config(uint8_t *mem)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(reg_init); i++)
		if (reg_init[i].type == I80_CMD)
			st7567_write_cmd(reg_init[i].data);
		else if (reg_init[i].type == I80_PARA)
			st7567_write_param(reg_init[i].data);
		else if (reg_init[i].type == I80_DELAY)
			mdelay(reg_init[i].data);
	if (mem_addr != NULL)
		manual_sync(mem_addr);
	else
		manual_sync(antaur_h);
	return 0;	
}

int st7567_init(void)
{
	struct i80_dev *st7567;

	st7567= (struct i80_dev *)kmalloc(sizeof(struct i80_dev), GFP_KERNEL);
	if (!st7567)
		return -1;

	st7567->name = "ST7567_132_65";
	st7567->config = st7567_config;
	st7567->open = st7567_open;
	st7567->close = st7567_close;
	st7567->write = st7567_mannual_write;
	st7567->enable = st7567_enable;
	st7567->display_manual = manual_sync;

	return i80_register(st7567);
}
