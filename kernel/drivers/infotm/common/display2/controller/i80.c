/* display i80 driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/display_device.h>
#include <mach/pad.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"i80"


/* i80 controller will not work after disable and enable again,
  * so suggest to use manual refresh instead.
  * according to Q3 limit */


/* this driver is written by sun.sun */


static void i80_writel(uint32_t offset, uint8_t shift, uint8_t width, uint32_t data)
{
	uint32_t mask = (1 << width) - 1;
	uint32_t val;

	val = ids_readword(0, offset);
	val &= ~(mask << shift);
	val |= (data & mask) << shift;
	ids_writeword(0, offset, val);
}

static uint32_t i80_readl(int32_t offset, uint8_t shift, uint8_t width)
{
	uint32_t mask = (1 << width) - 1;
	uint32_t val;

	val = ids_readword(0, offset);
	val &= (mask << shift);
	val >>= shift;

	return val;
}

int i80_wait_idle(void)
{
	uint32_t val;

	while (1) {
		val = i80_readl(I80TRIGCON, 0, 5);
		if (!(val &= ((1 << I80IF_TRIGCON_NORMAL_CMD_ST) |
			(1 << I80IF_TRIGCON_TR_DATA) |
			(1 << I80IF_TRIGCON_TR_AUTOCMD) |
			(1 << I80IF_TRIGCON_TR_NORMALCMD))))
			break;
	}
	return 0;
}

void i80_wait_frame_end(void)
{
	uint32_t val;

	do {
		val = i80_readl(I80TRIGCON, 4, 1);
	} while (!val);
}

void i80_lcdc_enable(int en)
{
	ids_write(0, LCDCON1, LCDCON1_ENVID, 1, en);
}

void i80_enable(int flags)
{
	i80_writel(I80IFCON0, I80IF_IFCON0_EN, 1, flags);
}

void i80_set_clock_cycle(struct i80_cycles *cycle)
{
	if (!cycle)
		return;

	i80_writel(I80IFCON0, I80IF_IFCON0_CS_SETUP, 5, cycle->cs_setup);
	i80_writel(I80IFCON0, I80IF_IFCON0_WR_SETUP, 6, cycle->wr_setup);
	i80_writel(I80IFCON0, I80IF_IFCON0_WR_ACTIVE, 6, cycle->wr_act);
	i80_writel(I80IFCON0, I80IF_IFCON0_WR_HOLD, 6, cycle->wr_hold);
}

void i80_set_int_mask(enum I80_INT_TYPE type)
{
	i80_writel(I80IFCON0, I80IF_IFCON0_INTR_EN, 2, type);
}

void i80_set_main_lcd(enum MAIN_LCD type)
{
	i80_writel(I80IFCON0, I80IF_IFCON0_MAINLCD_CFG, 1, type);
}

void i80_set_date_structure(struct i80_data *data)
{
	if (!data)
		return;

	i80_writel(I80IFCON1, I80IF_IFCON1_PORTTYPE, 2, data->porttype);
	i80_writel(I80IFCON1, I80IF_IFCON1_PORTDISTRIBUTED, 2, data->portdistributed);
	i80_writel(I80IFCON1, I80IF_IFCON1_DATASTYLE, 2, data->datastyle);
	i80_writel(I80IFCON1, I80IF_IFCON1_DATALSBFIRST, 1, data->datalsb);
	i80_writel(I80IFCON1, I80IF_IFCON1_DATAUSELOWPORT, 1, data->datauselowport);
}

void i80_set_autocmd(struct i80_autocmd *cmd)
{
	if (!cmd)
		return;

	i80_writel(I80IFCON1, I80IF_IFCON1_DISAUTOCMD, 1, cmd->type);
	i80_writel(I80IFCON1, I80IF_IFCON1_AUTOCMDRATE, 4, cmd->rate);
}

void i80_set_cmd(struct i80_cmd *cmd)
{
	if (!cmd)
		return;

	i80_writel(I80CMDCON0, cmd->num * 2, 2, cmd->type);
	i80_writel(I80CMDCON1, cmd->num * 2, 1, cmd->rs_ctrl);
	i80_writel(I80CMD15 + (15 - cmd->num) * 4, 0, 18, cmd->cmd);
}

void i80_mannual_write_once(int data)
{
	i80_writel(I80MANWDAT, 0, 18, data);
	i80_mannual_ctrl(I80_MAN_DOE, I80_ACTIVE_HIGH);
	udelay(1);
	i80_mannual_ctrl(I80_MAN_WR, I80_ACTIVE_LOW);
	udelay(5);
	i80_mannual_ctrl(I80_MAN_WR, I80_ACTIVE_HIGH);
	udelay(5);
}

int i80_mannual_read_once(void)
{
	uint32_t val;

	i80_mannual_ctrl(I80_MAN_DOE, I80_ACTIVE_LOW);
	udelay(1);
	i80_mannual_ctrl(I80_MAN_RD, I80_ACTIVE_LOW);
	udelay(5);
	val = i80_readl(I80MANRDAT, 0, 18);
	i80_mannual_ctrl(I80_MAN_RD, I80_ACTIVE_HIGH);
	udelay(5);
	return val;
}

void i80_mannual_ctrl(enum SIGNAL_CTRL ctrl, int value)
{
	i80_writel(I80MANCON, ctrl, 1, value);
}

void i80_mannual_init(void)
{
	i80_writel(I80MANCON, 0, 6, 0x3f);
}

void i80_mannual_deinit(void)
{
	i80_writel(I80MANCON, 0, 6, 0x00);
}

void i80_trigger_normal_cmd(int flags)
{
	i80_writel(I80TRIGCON, 0, 1, flags);
}

void i80_clear_cmd_list(void)
{
	struct i80_cmd cmd;
	int i;

	for (i = 0; i < 16; i++) {
		cmd.num = i;
		cmd.cmd = 0;
		cmd.type = CMD_DISABLE;
		cmd.rs_ctrl = I80_ACTIVE_LOW;
		i80_set_cmd(&cmd);
	}
}

static int data_type[6][5] = {
	{ 0x0, 0x0, 0x0, 0x0, 0x0 },
	{ 0x1, 0x0, 0x0, 0x0, 0x0 },
	{ 0x2, 0x1, 0x0, 0x0, 0x0 },
	{ 0x3, 0x0, 0x0, 0x0, 0x0 },
	{ 0x2, 0x3, 0x0, 0x0, 0x3 },
	{ 0x0, 0x2, 0x0, 0x0, 0x0 },
};

void i80_default_cfg(int format)
{
	struct i80_data data;
	struct i80_autocmd autocmd;
	struct i80_cycles cycles;

	i80_wait_idle();

	memcpy(&data, data_type[format], 5 * 4);
	i80_set_date_structure(&data);

	autocmd.type = AUTOCMD_BEFORE_VIDEO;
	autocmd.rate = AUTO_RATE_PER_1;
	i80_set_autocmd(&autocmd);

	i80_clear_cmd_list();
	i80_mannual_deinit();

	cycles.cs_setup = 0;
	cycles.wr_setup = 0;
	cycles.wr_act = 1;
	cycles.wr_hold = 0;
	i80_set_clock_cycle(&cycles);

	i80_set_int_mask(FRAME_OR_CMD_OVER);
	i80_set_main_lcd(CS0_MAIN_LCD);
}

/* i80 controller seriously rely on lcd controller, so treat as a interface of lcdc */

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display i80 controller driver");
MODULE_LICENSE("GPL");
