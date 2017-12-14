/******************************************************************************
 *  I2C adapter for the IMG Serial Control Bus IP block.

 @copyright Imagination Technologies Ltd. All Rights Reserved.

 @license Strictly Confidential.
 No part of this software, either material or conceptual may be copied or
 distributed, transmitted, transcribed, stored in a retrieval system or
 translated into any human or computer language in any form by any means,
 electronic, mechanical, manual or other-wise, or disclosed to third
 parties without the express written permission of
 Imagination Technologies Limited,
 Unit 8, HomePark Industrial Estate,
 King's Langley, Hertfordshire,
 WD4 8LZ, U.K.

 ******************************************************************************/
//#define DEBUG	1
//#define CONFIG_I2C_IMG_DEBUG_BUFFER 1
#ifdef DEBUG
#define CONFIG_I2C_DEBUG_BUS    1
#define SYS_DBG printk
#endif

#ifndef IMG_SCB_POLLING_MODE
#include <ci_kernel/ci_kernel.h>
#include <ci_internal/sys_device.h>
#endif
#include <ci_internal/i2c-img.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/of_platform.h>
#include <linux/completion.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/version.h>

#ifndef CONFIG_DANUBE_MDC_DMA
#undef CONFIG_I2C_DMA_SUPPORT
#endif

#ifdef CONFIG_I2C_DMA_SUPPORT
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/img_mdc_dma.h>
#endif //CONFIG_I2C_DMA_SUPPORT

#define SCB_STATUS_REG			0x0
#define SCB_CLEAR_REG			0x98
#define SCB_OVERRIDE_REG		0x4
#define SCB_READ_ADDR_REG		0x8
#define SCB_READ_COUNT_REG		0xc
#define SCB_WRITE_ADDR_REG		0x10
#define SCB_READ_XADDR_REG		0x74
#define SCB_WRITE_XADDR_REG		0x78
#define SCB_WRITE_COUNT_REG		0x7c
#define SCB_READ_DATA_REG		0x14
#define SCB_READ_FIFO_REG		0x94
#define SCB_WRITE_DATA_REG		0x18
#define SCB_FIFO_STATUS_REG		0x1c
#define SCB_FIFO_FLUSH_REG		0x8c
#define SCB_CLK_SET_REG			0x3c
#define SCB_INT_STATUS_REG		0x40
#define SCB_INT_CLEAR_REG		0x44
#define SCB_INT_MASK_REG		0x48
#define SCB_CONTROL_REG			0x4c

#define SCB_TIME_TPL_REG		0x50
#define SCB_TIME_TPH_REG		0x54
#define SCB_TIME_TP2S_REG		0x58
#define SCB_TIME_TBI_REG		0x60
#define SCB_TIME_TSL_REG		0x64
#define SCB_TIME_TDL_REG		0x68
#define SCB_TIME_TSDL_REG		0x6c
#define SCB_TIME_TSDH_REG		0x70
#define SCB_TIME_TCKH_REG		0x84
#define SCB_TIME_TCKL_REG		0x88

#define SCB_CORE_REV_REG		0x80

#define SCB_CONTROL_TRANSACTION_HALT	0x200
#define SCB_CONTROL_CLK_ENABLE		0x1e0
#define SCB_CONTROL_SOFT_RESET		0x01f

#define FIFO_READ_FULL			0x1
#define FIFO_READ_EMPTY			0x2
#define FIFO_WRITE_FULL			0x4
#define FIFO_WRITE_EMPTY		0x8

#define INT_BUS_INACTIVE		0x00001
#define INT_UNEXPECTED_START		0x00002
#define INT_SCLK_LOW_TIMEOUT		0x00004
#define INT_SDAT_LOW_TIMEOUT		0x00008
#define INT_WRITE_ACK_ERR		0x00010
#define INT_ADDR_ACK_ERR		0x00020
#define INT_READ_FIFO_FULL		0x00200
#define INT_READ_FIFO_FILLING		0x00400
#define INT_WRITE_FIFO_EMPTY		0x00800
#define INT_WRITE_FIFO_EMPTYING		0x01000
#define INT_TRANSACTION_DONE		0x08000
#define INT_SLAVE_EVENT			0x10000
#define INT_TIMING			0x40000

/* level interrupts need clearing after handling instead of before */
#define INT_LEVEL			0x01e00

/* don't allow any interrupts while the clock may be off */
#define INT_ENABLE_MASK_INACTIVE	0x00000
#define INT_ENABLE_MASK_RAW		0x40000
#define INT_ENABLE_MASK_ATOMIC		0x18034
#define INT_ENABLE_MASK_AUTOMATIC	0x01e34
#define INT_ENABLE_MASK_WAITSTOP	0x10030

#define MODE_INACTIVE			0
#define MODE_RAW			1
#define MODE_ATOMIC			2
#define MODE_AUTOMATIC			3
#define MODE_SEQUENCE			4
#define MODE_FATAL			5
#define MODE_WAITSTOP			6
#define MODE_SUSPEND			7

#define LINESTAT_SCLK_LINE_STATUS	0x00000001
#define LINESTAT_SCLK_EN		0x00000002
#define LINESTAT_SDAT_LINE_STATUS	0x00000004
#define LINESTAT_SDAT_EN		0x00000008
#define LINESTAT_DET_START_STATUS	0x00000010
#define LINESTAT_DET_STOP_STATUS	0x00000020
#define LINESTAT_DET_ACK_STATUS		0x00000040
#define LINESTAT_DET_NACK_STATUS	0x00000080
#define LINESTAT_BUS_IDLE		0x00000100
#define LINESTAT_T_DONE_STATUS		0x00000200
#define LINESTAT_SCLK_OUT_STATUS	0x00000400
#define LINESTAT_SDAT_OUT_STATUS	0x00000800
#define LINESTAT_GEN_LINE_MASK_STATUS	0x00001000
#define LINESTAT_START_BIT_DET		0x00002000
#define LINESTAT_STOP_BIT_DET		0x00004000
#define LINESTAT_ACK_DET		0x00008000
#define LINESTAT_NACK_DET		0x00010000
#define LINESTAT_INPUT_HELD_V		0x00020000
#define LINESTAT_ABORT_DET		0x00040000
#define LINESTAT_ACK_OR_NACK_DET	(LINESTAT_ACK_DET | LINESTAT_NACK_DET)
#define LINESTAT_INPUT_DATA		0xff000000
#define LINESTAT_INPUT_DATA_SHIFT	24

#define LINESTAT_CLEAR_SHIFT		13
#define LINESTAT_LATCHED		(0x3f << LINESTAT_CLEAR_SHIFT)

#define OVERRIDE_SCLK_OVR		0x001
#define OVERRIDE_SCLKEN_OVR		0x002
#define OVERRIDE_SDAT_OVR		0x004
#define OVERRIDE_SDATEN_OVR		0x008
#define OVERRIDE_MASTER			0x200
#define OVERRIDE_LINE_OVR_EN		0x400
#define OVERRIDE_DIRECT			0x800
#define OVERRIDE_CMD_SHIFT		4
#define OVERRIDE_DATA_SHIFT		24

#define OVERRIDE_SCLK_DOWN		(OVERRIDE_LINE_OVR_EN | \
					 OVERRIDE_SCLKEN_OVR)
#define OVERRIDE_SCLK_UP		(OVERRIDE_LINE_OVR_EN | \
					 OVERRIDE_SCLKEN_OVR | \
					 OVERRIDE_SCLK_OVR)
#define OVERRIDE_SDAT_DOWN		(OVERRIDE_LINE_OVR_EN | \
					 OVERRIDE_SDATEN_OVR)
#define OVERRIDE_SDAT_UP		(OVERRIDE_LINE_OVR_EN | \
					 OVERRIDE_SDATEN_OVR | \
					 OVERRIDE_SDAT_OVR)

#define CMD_PAUSE			0x00
#define CMD_GEN_DATA			0x01
#define CMD_GEN_START			0x02
#define CMD_GEN_STOP			0x03
#define CMD_GEN_ACK			0x04
#define CMD_GEN_NACK			0x05
#define CMD_RET_DATA			0x08
#define CMD_RET_ACK			0x09

#define ATSTATE_ADDRESSING		0
#define ATSTATE_DATA_XFERED		1

#define TIMEOUT_TBI			0x0
#define TIMEOUT_TSL			0xffff
#define TIMEOUT_TDL			0x0

/*
 * Hardware quirks
 */

/*
 * Do 2 dummy writes to ensure a subsequent read reflects the effects of a prior
 * write. This is necessary due to clock domain crossing in the SCB.
 */
#define QUIRK_WR_RD_FENCE	0x00000001
/*
 * Automatic mode reads and writes are unreliable when using different clock
 * domains, e.g. on Comet using XTAL1 in the SCB clock switch. This can be
 * worked around by using atomic mode (slower), or the same clock domain.
 */
#define QUIRK_ATOMIC_ONLY	0x00000002

/*
 * Bits to return from isr handler functions for different modes.
 * This delays completion until we've finished with the registers, so that the
 * function waiting for completion can safely disable the clock to save power.
 */
#define ISR_COMPLETE_M		0x80000000
#define ISR_FATAL_M		0x40000000
#define ISR_WAITSTOP		0x20000000
#define ISR_ATDATA_M		0x0ff00000
#define ISR_ATDATA_S		20
#define ISR_ATCMD_M		0x000f0000
#define ISR_ATCMD_S		16
#define ISR_STATUS_M		0x0000ffff	/* contains +ve errno */
#define ISR_COMPLETE(ERR)	(ISR_COMPLETE_M | (ISR_STATUS_M & (ERR)))
#define ISR_FATAL(ERR)		(ISR_COMPLETE(ERR) | ISR_FATAL_M)
#define ISR_ATOMIC(CMD, DATA)	((ISR_ATCMD_M & ((CMD) << ISR_ATCMD_S)) \
				| (ISR_ATDATA_M & ((DATA) << ISR_ATDATA_S)))

/* Timing parameters for i2c modes (in ns) */
static struct {
	char *name;
	unsigned int max_bitrate;	/* bps */
	unsigned int tckh, tckl, tsdh, tsdl;
	unsigned int tp2s, tpl, tph;
} img_i2c_timings[] = {
	/* Standard mode */
	{
		.name = "standard",
		.max_bitrate = 100000,
		.tckh = 4000,
		.tckl = 4700,
		.tsdh = 4700,
		.tsdl = 8700,
		.tp2s = 4700,
		.tpl = 4700,
		.tph = 4000,
	},
	/* Fast mode */
	{
		.name = "fast",
		.max_bitrate = 400000,
		.tckh = 600,
		.tckl = 1300,
		.tsdh = 600,
		.tsdl = 1200,
		.tp2s = 1300,
		.tpl = 600,
		.tph = 600,
	},
};

/* Reset dance */
static u8 img_i2c_reset_seq[] = { CMD_GEN_START,
				  CMD_GEN_DATA, 0xff,
				  CMD_RET_ACK,
				  CMD_GEN_START,
				  CMD_GEN_STOP,
				  0 };
#ifndef IMG_SCB_POLLING_MODE
/* Just issue a stop (after an abort condition) */
static u8 img_i2c_stop_seq[] = {  CMD_GEN_STOP,
				  0 };

/* We're interested in different interrupts depending on the mode */
static unsigned int img_i2c_int_enable_by_mode[] = {
	[MODE_INACTIVE]  = INT_ENABLE_MASK_INACTIVE,
	[MODE_RAW]       = INT_ENABLE_MASK_RAW,
	[MODE_ATOMIC]    = INT_ENABLE_MASK_ATOMIC,
	[MODE_AUTOMATIC] = INT_ENABLE_MASK_AUTOMATIC,
	[MODE_SEQUENCE]  = INT_ENABLE_MASK_ATOMIC,
	[MODE_FATAL]     = 0,
	[MODE_WAITSTOP]  = INT_ENABLE_MASK_WAITSTOP,
	[MODE_SUSPEND]   = 0,
};
#endif

/* Mode names */
static const char * const img_scb_mode_names[] = {
	[MODE_INACTIVE]   = "INACTIVE",
	[MODE_RAW]		  = "RAW",
	[MODE_ATOMIC]     = "ATOMIC",
	[MODE_AUTOMATIC]  = "AUTOMATIC",
	[MODE_SEQUENCE]   = "SEQUENCE",
	[MODE_FATAL]      = "FATAL",
	[MODE_WAITSTOP]   = "WAITSTOP",
	[MODE_SUSPEND]    = "SUSPEND",
};

/* Atomic command names */
static const char * const img_scb_atomic_cmd_names[] = {
	[CMD_PAUSE]	= "PAUSE",
	[CMD_GEN_DATA]	= "GEN_DATA",
	[CMD_GEN_START]	= "GEN_START",
	[CMD_GEN_STOP]	= "GEN_STOP",
	[CMD_GEN_ACK]	= "GEN_ACK",
	[CMD_GEN_NACK]	= "GEN_NACK",
	[CMD_RET_DATA]	= "RET_DATA",
	[CMD_RET_ACK]	= "RET_ACK",
};

struct img_i2c;
typedef unsigned int img_scb_raw_handler(struct img_i2c *i2c,
					 unsigned int int_status,
					 unsigned int line_status);

struct img_i2c {
	struct i2c_adapter adap;

	void __iomem *reg_base;

	int irq;

	/*
	 * The clock is used to get the input frequency, and to disable it
	 * after every set of transactions to save some power.
	 */
	struct clk *clk;
	unsigned int bitrate;
	unsigned int busdelay;
	unsigned int quirks;

	/* state */
	struct completion xfer_done;
	spinlock_t main_lock;	/* lock before doing anything with the state */
	struct i2c_msg msg;
	int last_msg;	/* wait for a stop bit after this transaction */
	int status;

	int mode;			/* MODE_* */
	unsigned int int_enable;	/* depends on mode */
	unsigned int line_status;	/* line status over command */

	/*
	 * To avoid slave event interrupts in automatic mode, use a timer to
	 * poll the abort condition if we don't get an interrupt for too long.
	 */
	struct timer_list check_timer;
	int t_halt;

	/* atomic mode state */
	int at_t_done;
	int at_slave_event;
	int at_cur_cmd;
	u8 at_cur_data;
	int at_state;	/* ATSTATE_* */

	u8 *seq;	/* see img_scb_sequence_handle_irq */

	/* raw mode */
	unsigned int raw_timeout;
	img_scb_raw_handler *raw_handler;
	unsigned int data_setup_cycles;

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
	/* for minimal overhead debugging */
	struct {
		unsigned int time;
		unsigned int irq_stat;
		unsigned int line_stat;
		unsigned int cmd;
		unsigned int result;
	} irq_buf[64];
	unsigned int start_time;
	unsigned int irq_buf_index;
#endif
};

#ifdef CONFIG_I2C_DMA_SUPPORT
typedef enum
{
	SCB_DMA_READ=0,
	SCB_DMA_WRITE,
	SCB_DMA_RESERVED1,
	SCB_DMA_RESERVED2
} scb_dma_mode;
#endif //CONFIG_I2C_DMA_SUPPORT

static void scb_write_reg(void __iomem *reg_base, unsigned int regno,
			  unsigned int value)
{
	writel(value, reg_base + regno);
}

static unsigned int scb_read_reg(void __iomem *reg_base, unsigned int regno)
{
	return readl(reg_base + regno);
}

static void scb_wr_rd_fence(struct img_i2c *i2c)
{
	if (i2c->quirks & QUIRK_WR_RD_FENCE) {
		scb_write_reg(i2c->reg_base, SCB_CORE_REV_REG, 0);
		scb_write_reg(i2c->reg_base, SCB_CORE_REV_REG, 0);
	}
}

static void img_scb_switch_mode(struct img_i2c *i2c, int mode)
{
	i2c->mode = mode;
#ifndef IMG_SCB_POLLING_MODE
	i2c->int_enable = img_i2c_int_enable_by_mode[mode];
#else
    i2c->int_enable = 0;
#endif
	i2c->line_status = 0;
}

/* delay the check timeout for a bit longer */
static void img_scb_delay_check(struct img_i2c *i2c)
{
	mod_timer(&i2c->check_timer, jiffies + msecs_to_jiffies(1));
}

static void img_scb_raw_op(struct img_i2c *i2c,
			   int force_sdat, int sdat,
			   int force_sclk, int sclk,
			   int timeout,
			   img_scb_raw_handler *handler)
{
	i2c->raw_timeout = timeout;
	i2c->raw_handler = handler;
	scb_write_reg(i2c->reg_base, SCB_OVERRIDE_REG,
		(sclk		? OVERRIDE_SCLK_OVR	: 0) |
		(force_sclk	? OVERRIDE_SCLKEN_OVR	: 0) |
		(sdat		? OVERRIDE_SDAT_OVR	: 0) |
		(force_sdat	? OVERRIDE_SDATEN_OVR	: 0) |
		OVERRIDE_MASTER |
		OVERRIDE_LINE_OVR_EN |
		OVERRIDE_DIRECT |
		((i2c->at_cur_cmd & 0x1f) << OVERRIDE_CMD_SHIFT) |
		(i2c->at_cur_data << OVERRIDE_DATA_SHIFT));
}

static const char *img_scb_atomic_op_name(unsigned int cmd)
{
	if (unlikely(cmd >= ARRAY_SIZE(img_scb_atomic_cmd_names)))
		return "UNKNOWN";
	return img_scb_atomic_cmd_names[cmd];
}

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
static const char *img_scb_mode_name(int mode)
{
	if (unlikely(mode >= ARRAY_SIZE(img_scb_mode_names)))
		return "UNKNOWN";
	return img_scb_mode_names[mode];
}
#endif

static void img_scb_atomic_op(struct img_i2c *i2c, int cmd, u8 data);

static unsigned int img_scb_raw_atomic_delay_handler(struct img_i2c *i2c,
					      unsigned int int_status,
					      unsigned int line_status)
{
	/* stay in raw mode for this, so we don't just loop infinitely */
	img_scb_atomic_op(i2c, i2c->at_cur_cmd, i2c->at_cur_data);
	img_scb_switch_mode(i2c, MODE_ATOMIC);
	return ISR_ATOMIC(i2c->at_cur_cmd, i2c->at_cur_data);
}

static void img_scb_atomic_op(struct img_i2c *i2c, int cmd, u8 data)
{
	unsigned int line_status;

	i2c->at_cur_cmd = cmd;
	i2c->at_cur_data = data;

	/* work around lack of data setup time when generating data */
	if (cmd == CMD_GEN_DATA && i2c->mode == MODE_ATOMIC) {
		line_status = scb_read_reg(i2c->reg_base, SCB_STATUS_REG);
		if (line_status & LINESTAT_SDAT_LINE_STATUS && !(data & 0x80)) {
			/* hold the data line down for a moment */
			img_scb_switch_mode(i2c, MODE_RAW);
			img_scb_raw_op(i2c, 1, 0, 1, 0,
					0, &img_scb_raw_atomic_delay_handler);
			return;
		}
	}

	dev_dbg(i2c->adap.dev.parent,
		"atomic cmd=%s (%d) data=%#x\n",
		img_scb_atomic_op_name(cmd), cmd,
		data);
	i2c->at_t_done = (cmd == CMD_RET_DATA || cmd == CMD_RET_ACK);
	i2c->at_slave_event = 0;
	i2c->line_status = 0;

	scb_write_reg(i2c->reg_base, SCB_OVERRIDE_REG,
		((cmd & 0x1f) << OVERRIDE_CMD_SHIFT) |
		OVERRIDE_MASTER |
		OVERRIDE_DIRECT |
		(data << OVERRIDE_DATA_SHIFT));
}

static void img_scb_atomic_start(struct img_i2c *i2c)
{
	img_scb_switch_mode(i2c, MODE_ATOMIC);
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);
	i2c->at_state = ATSTATE_ADDRESSING;
	img_scb_atomic_op(i2c, CMD_GEN_START, 0x00);
}

static void img_scb_soft_reset(struct img_i2c *i2c)
{
	i2c->t_halt = 0;
	scb_write_reg(i2c->reg_base, SCB_CONTROL_REG, 0);

	scb_write_reg(i2c->reg_base, SCB_CONTROL_REG,
		      SCB_CONTROL_CLK_ENABLE | SCB_CONTROL_SOFT_RESET);
}

/* enable or release transaction halt for control of repeated starts */
static void img_scb_transaction_halt(struct img_i2c *i2c, int t_halt)
{
	unsigned int tmp;

	if (i2c->t_halt == t_halt)
		return;

	i2c->t_halt = t_halt;
	tmp = scb_read_reg(i2c->reg_base, SCB_CONTROL_REG);
	if (t_halt)
		tmp |= SCB_CONTROL_TRANSACTION_HALT;
	else
		tmp &= ~SCB_CONTROL_TRANSACTION_HALT;
	scb_write_reg(i2c->reg_base, SCB_CONTROL_REG, tmp);
}

#ifdef CONFIG_I2C_DMA_SUPPORT
static int img_scb_dma(struct img_i2c *i2c, scb_dma_mode mode)
{
	dma_cap_mask_t mask;
	struct dma_chan *chan;
	struct mdc_dma_cookie cookie;
	dma_cookie_t dma_cookie;
	struct dma_async_tx_descriptor *xdesc;
	struct dma_slave_config conf;
	struct mdc_dma_tx_control tx_control;

	dma_cap_zero(mask);
	dma_cap_set(DMA_MEMCPY, mask);

	/* Request the channel */
	if (mode == SCB_DMA_READ)
		cookie.periph = 25;
	else if (mode == SCB_DMA_WRITE)
		cookie.periph = 24;
	cookie.req_channel = -1;
	chan = dma_request_channel(mask, &mdc_dma_filter_fn, &cookie);

	if(!chan)
	{
		printk(KERN_ERR "Failed to allocate DMA channel\n");
		return -EFAULT;
	}

	tx_control.flags = MDC_PRIORITY;
	tx_control.prio = IMG_DMA_PRIO_BULK;

	memset(&conf, 0, sizeof(struct dma_slave_config));

	if (mode == SCB_DMA_READ) {
		conf.direction = DMA_DEV_TO_MEM;
		conf.src_addr = (dma_addr_t)(i2c->reg_base + SCB_READ_DATA_REG);
		conf.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
		conf.dst_addr = *i2c->msg.buf;
		conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	} else if (mode == SCB_DMA_WRITE)
	{
		conf.direction = DMA_MEM_TO_DEV;
		conf.src_addr = *i2c->msg.buf;
		conf.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		conf.dst_addr = (dma_addr_t)(i2c->reg_base + SCB_WRITE_DATA_REG);
		conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
	}
	conf.src_maxburst = 16;
	conf.dst_maxburst = 16;

	chan->private = (void *)&tx_control;

	/* Configure the channel */
	dmaengine_slave_config(chan, &conf);

	/* Prepare the transfer */
	xdesc = dmaengine_prep_slave_single(chan, *i2c->msg.buf,
			i2c->msg.len, conf.direction,
			DMA_CTRL_ACK | DMA_PREP_INTERRUPT);

	if (!xdesc)
	{
		printk(KERN_ERR "Failed to allocate a dma descriptor\n");
		return -EFAULT;
	}

	/* Set the complete callback information */
	//xdesc->callback = img_dma_callback; //FIXME
	//xdesc->callback_param = &listener->xfers[i];

	/* Submit the operation */
	dma_cookie = dmaengine_submit(xdesc);

	if(dma_submit_error(dma_cookie))
	{
		printk(KERN_ERR "Failed to submit DMA operation\n");
		return -EFAULT;
	}

	/* Start all pending operations */
	dma_async_issue_pending(chan);

	scb_wr_rd_fence(i2c);

	i2c->msg.len = 0;

	dma_release_channel(chan);

	return 0;
}
#endif //CONFIG_I2C_DMA_SUPPORT

static void img_scb_read_fifo(struct img_i2c *i2c)
{
	while (i2c->msg.len) {
		unsigned int fifo_status;
		char data;

		/* Get FIFO status. */
		fifo_status = scb_read_reg(i2c->reg_base, SCB_FIFO_STATUS_REG);

		if (fifo_status & FIFO_READ_EMPTY)
			break;

#ifdef CONFIG_I2C_DMA_SUPPORT
		if (img_scb_dma(i2c, SCB_DMA_READ) == 0)
			break;
#endif //CONFIG_I2C_DMA_SUPPORT

		/* Read data from FIFO. */
		data = scb_read_reg(i2c->reg_base, SCB_READ_DATA_REG);

		*i2c->msg.buf = data;

		/* Advance FIFO. */
		scb_write_reg(i2c->reg_base, SCB_READ_FIFO_REG, 0xff);
		scb_wr_rd_fence(i2c);
		i2c->msg.len--;
		i2c->msg.buf++;
	}
}

static void img_scb_write_fifo(struct img_i2c *i2c)
{
	while (i2c->msg.len) {
		unsigned int fifo_status;

		/* Get FIFO status. */
		fifo_status = scb_read_reg(i2c->reg_base, SCB_FIFO_STATUS_REG);

		if (fifo_status & FIFO_WRITE_FULL)
		{
#ifdef IMG_SCB_POLLING_MODE
			/* Wait little bit in polling mode and try again later */
			schedule();
			continue;
#else
			break;
#endif
		}

#ifdef CONFIG_I2C_DMA_SUPPORT
		if (img_scb_dma(i2c, SCB_DMA_WRITE) == 0)
			break;
#endif //CONFIG_I2C_DMA_SUPPORT

		/* Write data into FIFO. */
		scb_write_reg(i2c->reg_base, SCB_WRITE_DATA_REG,
				*i2c->msg.buf);
		scb_wr_rd_fence(i2c);
		i2c->msg.len--;
		i2c->msg.buf++;

	}
	if (!i2c->msg.len)
		/* Disable fifo emptying interrupt if nothing more to write. */
		i2c->int_enable &= ~INT_WRITE_FIFO_EMPTYING;
}

static void img_scb_read(struct img_i2c *i2c)
{
#ifdef IMG_SCB_POLLING_MODE
	unsigned int int_status = 0;
#endif

	img_scb_switch_mode(i2c, MODE_AUTOMATIC);
#ifndef IMG_SCB_POLLING_MODE
	if (!i2c->last_msg)
		i2c->int_enable |= INT_SLAVE_EVENT;
#endif
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);

	/* Set address to read from. */
	scb_write_reg(i2c->reg_base, SCB_READ_ADDR_REG, i2c->msg.addr);
	/* Set number of bytes to read. */
	scb_write_reg(i2c->reg_base, SCB_READ_COUNT_REG, i2c->msg.len);

	/* Release transaction halt */
	img_scb_transaction_halt(i2c, 0);

	/* start check timer if not already going */
	img_scb_delay_check(i2c);

#ifdef IMG_SCB_POLLING_MODE
	do {
		/* Give some time to raise interrupt from slave */
		/*
		 * : During tests it was not needed but maybe it's good
		 * to preempt here. Remember that we're in atomic operation!
		 */
		//schedule();
		/* Read interrupt status register. */
		int_status = scb_read_reg(i2c->reg_base, SCB_INT_STATUS_REG);
		dev_dbg(i2c->adap.dev.parent, "Waiting for read interrupt, int_status = %#x\n", int_status);
	} while (!(int_status & (INT_READ_FIFO_FULL | INT_READ_FIFO_FILLING)));
	/* Clear detected interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_CLEAR_REG, int_status);

	while (i2c->msg.len) {
		dev_dbg(i2c->adap.dev.parent, "Reading I2C slave data: left = %#x\n", i2c->msg.len);
		img_scb_read_fifo(i2c);
		/*
		 * : During tests it was not needed but maybe it's good
		 * to preempt here. Remember that we're in atomic operation!
		 */
		//if (i2c->msg.len > 0)
			/* Give some time to gather more data */
		//	schedule();
	}
#endif
}

static void img_scb_write(struct img_i2c *i2c)
{
	img_scb_switch_mode(i2c, MODE_AUTOMATIC);
#ifndef IMG_SCB_POLLING_MODE
	if (!i2c->last_msg)
		i2c->int_enable |= INT_SLAVE_EVENT;
#endif

	/* Set address to write to. */
	scb_write_reg(i2c->reg_base, SCB_WRITE_ADDR_REG, i2c->msg.addr);
	/* Set number of bytes to write. */
	scb_write_reg(i2c->reg_base, SCB_WRITE_COUNT_REG, i2c->msg.len);

	/* Release transaction halt */
	img_scb_transaction_halt(i2c, 0);

	/* start check timer if not already going */
	img_scb_delay_check(i2c);

	/* Start filling fifo right away */
	img_scb_write_fifo(i2c);

	/* img_scb_write_fifo() may modify int_enable */
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);
}

/* After calling this, the ISR must not access any more SCB registers. */
static void img_scb_complete_transaction(struct img_i2c *i2c, int status)
{
	img_scb_switch_mode(i2c, MODE_INACTIVE);
	if (status) {
		i2c->status = status;
		img_scb_transaction_halt(i2c, 0);
	}
	complete(&i2c->xfer_done);
}

#ifndef IMG_SCB_POLLING_MODE
static unsigned int img_scb_raw_handle_irq(struct img_i2c *i2c,
					   unsigned int int_status,
					   unsigned int line_status)
{
	if (int_status & INT_TIMING) {
		if (!i2c->raw_timeout)
			return i2c->raw_handler(i2c, int_status, line_status);
		--i2c->raw_timeout;
	}
	return 0;
}
#endif

static unsigned int img_scb_sequence_handle_irq(struct img_i2c *i2c,
						unsigned int int_status)
{
	static const unsigned int continue_bits[] = {
		[CMD_GEN_START] = LINESTAT_START_BIT_DET,
		[CMD_GEN_DATA]  = LINESTAT_INPUT_HELD_V,
		[CMD_RET_ACK]   = LINESTAT_ACK_DET | LINESTAT_NACK_DET,
		[CMD_RET_DATA]  = LINESTAT_INPUT_HELD_V,
		[CMD_GEN_STOP]  = LINESTAT_STOP_BIT_DET,
	};
	int next_cmd = -1;
	u8 next_data = 0x00;

	if (int_status & INT_SLAVE_EVENT)
		i2c->at_slave_event = 1;
	if (int_status & INT_TRANSACTION_DONE)
		i2c->at_t_done = 1;

	if (!i2c->at_slave_event || !i2c->at_t_done)
		return 0;

	/* wait if no continue bits are set */
	if (i2c->at_cur_cmd >= 0 && i2c->at_cur_cmd
					< ARRAY_SIZE(continue_bits)) {
		unsigned int cont_bits = continue_bits[i2c->at_cur_cmd];
		if (cont_bits) {
			cont_bits |= LINESTAT_ABORT_DET;
			if (!(i2c->line_status & cont_bits))
				return 0;
		}
	}

	/* follow the sequence of commands in i2c->seq */
	next_cmd = *i2c->seq;
	/* stop on a nil */
	if (!next_cmd) {
		scb_write_reg(i2c->reg_base, SCB_OVERRIDE_REG, 0);
		return ISR_COMPLETE(0);
	}
	/* when generating data, the next byte is the data */
	if (next_cmd == CMD_GEN_DATA) {
		++i2c->seq;
		next_data = *i2c->seq;
	}
	++i2c->seq;
	img_scb_atomic_op(i2c, next_cmd, next_data);

	return ISR_ATOMIC(next_cmd, next_data);
}

static void img_scb_reset_start(struct img_i2c *i2c)
{
	/* Initiate the magic dance */
	img_scb_switch_mode(i2c, MODE_SEQUENCE);
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);
	i2c->seq = img_i2c_reset_seq;
	i2c->at_slave_event = 1;
	i2c->at_t_done = 1;
	i2c->at_cur_cmd = -1;
	/* img_i2c_reset_seq isn't empty so the following won't fail */
	img_scb_sequence_handle_irq(i2c, 0);
}

#ifndef IMG_SCB_POLLING_MODE
static void img_scb_stop_start(struct img_i2c *i2c)
{
	/* Initiate a stop bit sequence */
	img_scb_switch_mode(i2c, MODE_SEQUENCE);
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);
	i2c->seq = img_i2c_stop_seq;
	i2c->at_slave_event = 1;
	i2c->at_t_done = 1;
	i2c->at_cur_cmd = -1;
	/* img_i2c_stop_seq isn't empty so the following won't fail */
	img_scb_sequence_handle_irq(i2c, 0);
}

static unsigned int img_scb_atomic_handle_irq(struct img_i2c *i2c,
					      unsigned int int_status,
					      unsigned int line_status)
{
	int next_cmd = -1;
	u8 next_data = 0x00;

	if (int_status & INT_SLAVE_EVENT)
		i2c->at_slave_event = 1;
	if (int_status & INT_TRANSACTION_DONE)
		i2c->at_t_done = 1;

	if (!i2c->at_slave_event || !i2c->at_t_done)
		goto next_atomic_cmd;
	if (i2c->line_status & LINESTAT_ABORT_DET) {
		dev_dbg(i2c->adap.dev.parent, "abort condition detected\n");
		next_cmd = CMD_GEN_STOP;
		i2c->status = -EIO;
		goto next_atomic_cmd;
	}

	/* i2c->at_cur_cmd may have completed */
	switch (i2c->at_cur_cmd) {
	case CMD_GEN_START:
		next_cmd = CMD_GEN_DATA;
		next_data = (i2c->msg.addr << 1);
		if (i2c->msg.flags & I2C_M_RD)
			next_data |= 0x1;
		break;
	case CMD_GEN_DATA:
		if (i2c->line_status & LINESTAT_INPUT_HELD_V)
			next_cmd = CMD_RET_ACK;
		break;
	case CMD_RET_ACK:
		if (i2c->line_status & LINESTAT_ACK_DET) {
			if (i2c->msg.len == 0)
				next_cmd = CMD_GEN_STOP;
			else if (i2c->msg.flags & I2C_M_RD)
				next_cmd = CMD_RET_DATA;
			else {
				next_cmd = CMD_GEN_DATA;
				next_data = *i2c->msg.buf;
				--i2c->msg.len;
				++i2c->msg.buf;
				i2c->at_state = ATSTATE_DATA_XFERED;
			}
		} else if (i2c->line_status & LINESTAT_NACK_DET) {
			i2c->status = -EIO;
			next_cmd = CMD_GEN_STOP;
		}
		break;
	case CMD_RET_DATA:
		if (i2c->line_status & LINESTAT_INPUT_HELD_V) {
			*i2c->msg.buf = (i2c->line_status &
						LINESTAT_INPUT_DATA)
					>> LINESTAT_INPUT_DATA_SHIFT;
			--i2c->msg.len;
			++i2c->msg.buf;
			if (i2c->msg.len)
				next_cmd = CMD_GEN_ACK;
			else
				next_cmd = CMD_GEN_NACK;
		}
		break;
	case CMD_GEN_ACK:
		if (i2c->line_status & LINESTAT_ACK_DET) {
			next_cmd = CMD_RET_DATA;
		} else {
			i2c->status = -EIO;
			next_cmd = CMD_GEN_STOP;
		}
		i2c->at_state = ATSTATE_DATA_XFERED;
		break;
	case CMD_GEN_NACK:
		next_cmd = CMD_GEN_STOP;
		break;
	case CMD_GEN_STOP:
		scb_write_reg(i2c->reg_base, SCB_OVERRIDE_REG, 0);
		return ISR_COMPLETE(0);
	default:
		dev_err(i2c->adap.dev.parent, "bad atomic command %d\n",
			i2c->at_cur_cmd);
		i2c->status = -EIO;
		next_cmd = CMD_GEN_STOP;
		break;
	}

next_atomic_cmd:
	if (next_cmd != -1) {
		/* don't actually stop unless we're the last transaction */
		if (next_cmd == CMD_GEN_STOP && !i2c->status && !i2c->last_msg)
			return ISR_COMPLETE(0);
		img_scb_atomic_op(i2c, next_cmd, next_data);
	}
	return ISR_ATOMIC(next_cmd, next_data);
}
#endif

/*
 * Timer function to check if something has gone wrong in automatic mode (so we
 * don't have to handle so many interrupts just to catch an exception).
 */
static void img_scb_check_timer(unsigned long arg)
{
	struct img_i2c *i2c = (struct img_i2c *)arg;
	unsigned long flags;
	unsigned int line_status;

	spin_lock_irqsave(&i2c->main_lock, flags);
	line_status = scb_read_reg(i2c->reg_base, SCB_STATUS_REG);

	/* check for an abort condition */
	if (line_status & LINESTAT_ABORT_DET) {
		dev_dbg(i2c->adap.dev.parent,
			"abort condition detected by check timer\n");
#ifndef IMG_SCB_POLLING_MODE
		/* enable slave event interrupt mask to trigger irq */
		scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG,
			      i2c->int_enable | INT_SLAVE_EVENT);
#endif
	}

	spin_unlock_irqrestore(&i2c->main_lock, flags);
}

#ifndef IMG_SCB_POLLING_MODE
static unsigned int img_scb_automatic_handle_irq(struct img_i2c *i2c,
						 unsigned int int_status,
						 unsigned int line_status)
{
	if (int_status & (INT_WRITE_ACK_ERR |
			  INT_ADDR_ACK_ERR))
		return ISR_COMPLETE(EIO);
	if (line_status & LINESTAT_ABORT_DET) {
		dev_dbg(i2c->adap.dev.parent, "abort condition detected\n");
		/* empty the read fifo */
		if (i2c->msg.flags & I2C_M_RD && int_status &
				(INT_READ_FIFO_FULL | INT_READ_FIFO_FILLING))
			img_scb_read_fifo(i2c);
		/* use atomic mode and try to force a stop bit */
		i2c->status = -EIO;
		img_scb_stop_start(i2c);
		return 0;
	}
	/* Enable transaction halt on start bit */
	if (!i2c->last_msg && i2c->line_status & LINESTAT_START_BIT_DET) {
		img_scb_transaction_halt(i2c, 1);
		/* we're no longer interested in the slave event */
		i2c->int_enable &= ~INT_SLAVE_EVENT;
	}


	/* push back check timer */
	img_scb_delay_check(i2c);

	if (i2c->msg.flags & I2C_M_RD) {
		if (int_status & (INT_READ_FIFO_FULL |
				  INT_READ_FIFO_FILLING)) {
			img_scb_read_fifo(i2c);
			if (i2c->msg.len == 0)
				return ISR_WAITSTOP;
		}
	} else {
		if (int_status & (INT_WRITE_FIFO_EMPTY |
				  INT_WRITE_FIFO_EMPTYING)) {
			/*
			 * The write fifo empty indicates that we're in the
			 * last byte so it's safe to start a new write
			 * transaction without losing any bytes from the
			 * previous one.
			 * see 2.3.7 Repeated Start Transactions.
			 */
			if ((int_status & INT_WRITE_FIFO_EMPTY) &&
			    i2c->msg.len == 0)
				return ISR_WAITSTOP;
			img_scb_write_fifo(i2c);
		}
	}

	return 0;
}
#endif

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
/* get the name of a command in the debug irq_buf */
static const char *img_scb_cmd_name(unsigned int cmd)
{
	switch (cmd >> 16) {
	case MODE_ATOMIC:
	case MODE_SEQUENCE:
		return img_scb_atomic_op_name(cmd & 0xffff);
	default:
		return "UNKNOWN";
	}
}
#endif

#ifndef IMG_SCB_POLLING_MODE
/* dump info about the last few interrupts */
static void img_scb_dump_debug(struct img_i2c *i2c)
{
	static const struct {
		unsigned int addr;
		const char * const name;
	} interesting_regs[] = {
		{ SCB_STATUS_REG, "scb_line_status" },
		{ SCB_OVERRIDE_REG, "scb_line_override" },
		{ SCB_FIFO_STATUS_REG, "scb_master_fill_status" },
		{ SCB_INT_STATUS_REG, "scb_interrupt_status" },
		{ SCB_INT_MASK_REG, "scb_interrupt_mask" },
		{ SCB_CONTROL_REG, "scb_general_control" },
	};
	int i;

	/* print out some useful registers */
	dev_info(i2c->adap.dev.parent, "SCB registers:\n");
	for (i = 0; i < ARRAY_SIZE(interesting_regs); ++i)
		dev_info(i2c->adap.dev.parent, " %s=%#x\n",
			 interesting_regs[i].name,
			 scb_read_reg(i2c->reg_base, interesting_regs[i].addr));

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
	/* print out the contents of the debug buffer */
	if (!i2c->irq_buf_index)
		dev_info(i2c->adap.dev.parent,
			 "No interrupts in debug buffer\n");

	dev_info(i2c->adap.dev.parent,
		 "%d interrupts in debug buffer:\n",
		 i2c->irq_buf_index);
	for (i = 0; i < i2c->irq_buf_index; ++i)
		dev_info(i2c->adap.dev.parent,
			 " #%d after %uus: irq=%#x, stat=%#x, cmd=%s.%s (%#x), hret=%#x\n",
			 i,
			 (i2c->irq_buf[i].time - i2c->start_time) >> 10,
			 i2c->irq_buf[i].irq_stat,
			 i2c->irq_buf[i].line_stat,
			 img_scb_mode_name(i2c->irq_buf[i].cmd >> 16),
			 img_scb_cmd_name(i2c->irq_buf[i].cmd),
			 i2c->irq_buf[i].cmd,
			 i2c->irq_buf[i].result);
#endif
}
#endif

#if 0
static void img_scb_dump_regs(struct img_i2c *i2c)
{
	unsigned i;

	for (i = 0; i < 0xa0; i += 16)
	{
		printk(KERN_ERR "%#x:\t%#08x %#08x %#08x %#08x\n", i,
				*((unsigned int __iomem*)(i2c->reg_base) + i/4),
				*((unsigned int __iomem*)(i2c->reg_base) + i/4 + 1),
				*((unsigned int __iomem*)(i2c->reg_base) + i/4 + 2),
				*((unsigned int __iomem*)(i2c->reg_base) + i/4 + 3));
	}
}
#endif

#ifndef IMG_SCB_POLLING_MODE
static irqreturn_t img_scb_irq_handler(int irq, void *dev_id)
{
	struct img_i2c *i2c = (struct img_i2c *)dev_id;
	unsigned int int_status;
	unsigned int line_status;
	/* We handle transaction completion AFTER accessing registers */
	unsigned int hret;
	irqreturn_t irq_ret = IRQ_HANDLED;

	/* Read interrupt status register. */
	int_status = scb_read_reg(i2c->reg_base, SCB_INT_STATUS_REG);
	/* we share IRQ with Felix - don't handle it if it's not our */
	if ( (int_status & i2c->int_enable) == 0 ) {
	    irq_ret = IRQ_NONE;
	    goto exit;
	}
	/* Clear detected interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_CLEAR_REG, int_status);

	/*
	 * Read line status and clear it until it actually is clear.  We have
	 * to be careful not to lose any line status bits that get latched.
	 */
	line_status = scb_read_reg(i2c->reg_base, SCB_STATUS_REG);
	if (line_status & LINESTAT_LATCHED) {
		scb_write_reg(i2c->reg_base, SCB_CLEAR_REG,
			      (line_status & LINESTAT_LATCHED)
				>> LINESTAT_CLEAR_SHIFT);
		scb_wr_rd_fence(i2c);
	}

	spin_lock(&i2c->main_lock);

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
	if (i2c->irq_buf_index < ARRAY_SIZE(i2c->irq_buf)) {
		i2c->irq_buf[i2c->irq_buf_index].time =
						(unsigned int)local_clock();
		i2c->irq_buf[i2c->irq_buf_index].irq_stat = int_status;
		i2c->irq_buf[i2c->irq_buf_index].line_stat = line_status;
		i2c->irq_buf[i2c->irq_buf_index].cmd = (i2c->mode << 16)
							| i2c->at_cur_cmd;
	}
#endif

	/* Keep track of line status bits received */
	i2c->line_status &= ~LINESTAT_INPUT_DATA;
	i2c->line_status |= line_status;

	/*
	 * Certain interrupts indicate that sclk low timeout is not
	 * a problem. If any of these are set, just continue.
	 */
	if ((int_status & INT_SCLK_LOW_TIMEOUT) &&
	    !(int_status & (INT_SLAVE_EVENT |
			    INT_WRITE_FIFO_EMPTY |
			    INT_READ_FIFO_FULL))) {
		dev_crit(i2c->adap.dev.parent,
			 "fatal: clock low timeout occurred"
			 " %s addr 0x%02x\n",
			 (i2c->msg.flags & I2C_M_RD) ? "reading"
						      : "writing",
			 i2c->msg.addr);
		hret = ISR_FATAL(EIO);
		goto out;
	}

	if (i2c->mode == MODE_ATOMIC)
		hret = img_scb_atomic_handle_irq(i2c, int_status, line_status);
	else if (i2c->mode == MODE_AUTOMATIC)
		hret = img_scb_automatic_handle_irq(i2c, int_status,
						    line_status);
	else if (i2c->mode == MODE_SEQUENCE)
		hret = img_scb_sequence_handle_irq(i2c, int_status);
	else if (i2c->mode == MODE_WAITSTOP && (int_status & INT_SLAVE_EVENT) &&
			 (line_status & LINESTAT_STOP_BIT_DET))
		hret = ISR_COMPLETE(0);
	else if (i2c->mode == MODE_RAW)
		hret = img_scb_raw_handle_irq(i2c, int_status, line_status);
	else
		hret = 0;

	/* Clear detected level interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_CLEAR_REG, int_status & INT_LEVEL);
out:
#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
	if (i2c->irq_buf_index < ARRAY_SIZE(i2c->irq_buf)) {
		i2c->irq_buf[i2c->irq_buf_index].result = hret;
		++i2c->irq_buf_index;
	}
#endif

#ifdef CONFIG_I2C_DEBUG_BUS
	dev_dbg(i2c->adap.dev.parent,
		"serviced irq: %x (ls=%x,ret=%x) %s%s%s%s%s%s%s%s%s%s%s%s\n",
		int_status,
		line_status,
		hret,
		(int_status & INT_BUS_INACTIVE)
			? " | INT_BUS_INACTIVE" : "",
		(int_status & INT_UNEXPECTED_START)
			? " | INT_UNEXPECTED_START" : "",
		(int_status & INT_SCLK_LOW_TIMEOUT)
			? " | INT_SCLK_LOW_TIMEOUT" : "",
		(int_status & INT_SDAT_LOW_TIMEOUT)
			? " | INT_SDAT_LOW_TIMEOUT" : "",
		(int_status & INT_WRITE_ACK_ERR)
			? " | INT_WRITE_ACK_ERR" : "",
		(int_status & INT_ADDR_ACK_ERR)
			? " | INT_ADDR_ACK_ERR" : "",
		(int_status & INT_READ_FIFO_FULL)
			? " | INT_READ_FIFO_FULL" : "",
		(int_status & INT_READ_FIFO_FILLING)
			? " | INT_READ_FIFO_FILLING" : "",
		(int_status & INT_WRITE_FIFO_EMPTY)
			? " | INT_WRITE_FIFO_EMPTY" : "",
		(int_status & INT_WRITE_FIFO_EMPTYING)
			? " | INT_WRITE_FIFO_EMPTYING" : "",
		(int_status & INT_TRANSACTION_DONE)
			? " | INT_TRANSACTION_DONE" : "",
		(int_status & INT_SLAVE_EVENT)
			? " | INT_SLAVE_EVENT" : "");
#endif

	if (hret & ISR_WAITSTOP) {
		/*
		 * Only wait for stop on last message.
		 * Also we may already have detected the stop bit.
		 */
		if (!i2c->last_msg || i2c->line_status & LINESTAT_STOP_BIT_DET)
			hret = ISR_COMPLETE(0);
		else
			img_scb_switch_mode(i2c, MODE_WAITSTOP);
	}

	/* now we've finished using regs, handle transaction completion */
	if (hret & ISR_COMPLETE_M) {
		int status = -(hret & ISR_STATUS_M);
		img_scb_complete_transaction(i2c, status);
		if (hret & ISR_FATAL_M) {
			img_scb_switch_mode(i2c, MODE_FATAL);
			img_scb_dump_debug(i2c);
		}
	}

	/* Enable interrupts (int_enable may be altered by changing mode) */
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);

	spin_unlock(&i2c->main_lock);
exit:
	SYS_DevClearInterrupts(g_psCIDriver->pDevice);
	return irq_ret;
}
#endif

#ifdef IMG_SCB_POLLING_MODE
static unsigned int check_sequence_line_status(struct img_i2c *i2c)
{
	static const unsigned int continue_bits[] = {
		[CMD_GEN_START] = LINESTAT_START_BIT_DET,
		[CMD_GEN_DATA]  = LINESTAT_INPUT_HELD_V,
		[CMD_RET_ACK]   = LINESTAT_ACK_DET | LINESTAT_NACK_DET,
		[CMD_RET_DATA]  = LINESTAT_INPUT_HELD_V,
		[CMD_GEN_STOP]  = LINESTAT_STOP_BIT_DET,
	};
	int next_cmd = -1;
	u8 next_data = 0x00;

	/* wait for proper line status for relevant command */
	if (i2c->at_cur_cmd >= 0 && i2c->at_cur_cmd
			< ARRAY_SIZE(continue_bits)) {
		unsigned int cont_bits = continue_bits[i2c->at_cur_cmd];
		if (cont_bits) {
			cont_bits |= LINESTAT_ABORT_DET;
			/* Try again in case line status is wrong */
			if (!(i2c->line_status & cont_bits))
				return EAGAIN;
		}
	}

	/* follow the sequence of commands in i2c->seq */
	next_cmd = *i2c->seq;
	/* stop on a nil */
	if (!next_cmd) {
		scb_write_reg(i2c->reg_base, SCB_OVERRIDE_REG, 0);
		return ISR_COMPLETE(0);
	}
	/* when generating data, the next byte is the data */
	if (next_cmd == CMD_GEN_DATA) {
		++i2c->seq;
		next_data = *i2c->seq;
	}
	++i2c->seq;
	img_scb_atomic_op(i2c, next_cmd, next_data);

	return ISR_ATOMIC(next_cmd, next_data);
}

static int poll_i2c_status(struct img_i2c *i2c)
{
	unsigned int line_status;
	unsigned int hret = EAGAIN;

	/*
	 * Read line status and clear it until it actually is clear.  We have
	 * to be careful not to lose any line status bits that get latched.
	 */
	line_status = scb_read_reg(i2c->reg_base, SCB_STATUS_REG);
	if (line_status & LINESTAT_LATCHED) {
		scb_write_reg(i2c->reg_base, SCB_CLEAR_REG,
			      (line_status & LINESTAT_LATCHED)
				>> LINESTAT_CLEAR_SHIFT);
		scb_wr_rd_fence(i2c);
	}

	/* Keep track of line status bits received */
	i2c->line_status = line_status;
	i2c->line_status &= ~LINESTAT_INPUT_DATA;

	if (i2c->mode == MODE_AUTOMATIC)
	{
		unsigned int fifo_status;
		fifo_status = scb_read_reg(i2c->reg_base, SCB_FIFO_STATUS_REG);
		/* In automatic mode, finish when FIFO is empty */
		if (fifo_status & FIFO_WRITE_EMPTY)
			hret = ISR_COMPLETE_M;
	}
	else if (i2c->mode == MODE_SEQUENCE)
	{
		/* In sequence mode poll for line status */
		hret = check_sequence_line_status(i2c);
	}
	else
	{
		/* Polling mode is currently supported only for sequence and automatic modes */
		dev_crit(i2c->adap.dev.parent, "Mode %s is not supported in polling mode",
				img_scb_mode_names[i2c->mode]);
		BUG();
		hret = EINVAL;
	}

	/* now we've finished using regs, handle transaction completion */
	if (hret & ISR_COMPLETE_M) {
		int status = -(hret & ISR_STATUS_M);
		img_scb_complete_transaction(i2c, status);
		hret = 0;
	}

	return hret;
}

static void wait_until_transfer_done(struct img_i2c *i2c)
{
	/* In case of sequence mode (manual), give some time to stabilize I2C lines */
	if (i2c->mode == MODE_SEQUENCE)
		schedule();

	/* Wait until transfer is finished */
	while (poll_i2c_status(i2c))
	{
		schedule();
	}
}
#endif

static u32 img_scb_func(struct i2c_adapter *i2c_adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

// used to be __init but we already have one in felix
static int i2c_img_reset_bus(struct img_i2c *i2c)
{
	unsigned long flags;

	spin_lock_irqsave(&i2c->main_lock, flags);

	i2c->status = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
    INIT_COMPLETION(i2c->xfer_done);
#else
	reinit_completion(&i2c->xfer_done);
#endif
	img_scb_reset_start(i2c);

	spin_unlock_irqrestore(&i2c->main_lock, flags);

#ifdef IMG_SCB_POLLING_MODE
	wait_until_transfer_done(i2c);
#endif
	if ( wait_for_completion_timeout(&i2c->xfer_done, HZ) == 0)
	{
	    dev_err(i2c->adap.dev.parent, "Timeout - waiting for completion\n");
	    dev_err(i2c->adap.dev.parent, "i2c->mode 0x%X\n", i2c->mode);
	    img_scb_switch_mode(i2c, MODE_SUSPEND); //so next img_scb_xfer()
	                                              //reinit it
	}

	return i2c->status;
}

static int img_scb_init(struct img_i2c *i2c)
{
	int clk_khz;
	int bitrate_khz = i2c->bitrate / 1000;
	int opt_inc;
	int data, prescale, inc, filt, clk_period, int_bitrate;
	int tckh, tckl, tsdh;
	int mode, i;
	unsigned int revision;
	int ret = 0;

	revision = scb_read_reg(i2c->reg_base, SCB_CORE_REV_REG);
	if ((revision & 0x00ffffff) < 0x00020200) {
		dev_info(i2c->adap.dev.parent,
			 "Unknown hardware revision (%d.%d.%d.%d)\n",
			 (revision >> 24) & 0xff, (revision >> 16) & 0xff,
			 (revision >> 8) & 0xff, revision & 0xff);
		ret = -EINVAL;
		goto out;
	}

	clk_khz = SCB_REF_CLK * 1000 * 1000 / 1000;

	/* Determine what mode we're in from the bitrate */
	mode = ARRAY_SIZE(img_i2c_timings) - 1;
	for (i = 0; i < ARRAY_SIZE(img_i2c_timings); ++i)
		if (i2c->bitrate <= img_i2c_timings[i].max_bitrate) {
			mode = i;
			break;
		}

	/*
	 * Worst incs are 1 (innacurate) and 16*256 (irregular).
	 * So a sensible inc is the logarithmic mean: 64 (2^6), which is
	 * in the middle of the valid range (0-127).
	 */
	opt_inc = 64;

	/* Find the prescale that would give us that inc (approx delay = 0) */
	prescale = opt_inc * clk_khz / (256 * 16 * bitrate_khz);
	if (prescale > 8)
		prescale = 8;
	else if (prescale < 1)
		prescale = 1;
	clk_khz /= prescale;

	/* Setup the clock increment value. */
	inc = ((256 * 16 * bitrate_khz) /
	       (clk_khz - (16 * bitrate_khz * (clk_khz / 1000) *
			   i2c->busdelay) / 10000));
	/* Setup the filter clock value. */
	if (clk_khz < 20000) {
		/* Filter disable. */
		filt = 0x8000;
	} else if (clk_khz < 40000) {
		/* Filter bypass. */
		filt = 0x4000;
	} else {
		/* Calculate filter clock. */
		filt = ((640000) / ((clk_khz / 1000) *
				    (250 - i2c->busdelay)));
		if ((640000) % ((clk_khz / 1000) *
				(250 - i2c->busdelay))) {
			/* Scale up. */
			inc++;
		}
		if (filt > 0x7f)
			filt = 0x7f;
	}
	data = ((filt & 0xffff) << 16) | ((inc & 0x7f) << 8) | (prescale - 1);
//	scb_write_reg(i2c->reg_base, SCB_CLK_SET_REG, data);
	scb_write_reg(i2c->reg_base, SCB_CLK_SET_REG, 0x40004105);

	/* Obtain the clock period of the fx16 clock in ns. */
	clk_period = (256 * 1000000) / (clk_khz * inc) + i2c->busdelay;

	/* Calculate the bitrate in terms of internal clock pulses. */
	int_bitrate = 1000000 / (bitrate_khz * clk_period);
	if ((1000000 % (bitrate_khz * clk_period)) >=
	    ((bitrate_khz * clk_period) / 2))
		int_bitrate++;

	/* Setup TCKH value. */
	tckh = img_i2c_timings[mode].tckh / clk_period;
	if (img_i2c_timings[mode].tckh % clk_period)
		tckh++;

	if (tckh > 0)
		data = tckh - 1;
	else
		data = 0;

//	scb_write_reg(i2c->reg_base, SCB_TIME_TCKH_REG, data);
	scb_write_reg(i2c->reg_base, SCB_TIME_TCKH_REG, 0x07);

	/* Setup TCKL value. */
	tckl = int_bitrate - tckh;

	if (tckl > 0)
		data = tckl - 1;
	else
		data = 0;

//	scb_write_reg(i2c->reg_base, SCB_TIME_TCKL_REG, data);
	scb_write_reg(i2c->reg_base, SCB_TIME_TCKL_REG, 0x07);

	/* Setup TSDH value. */
	tsdh = img_i2c_timings[mode].tsdh / clk_period;
	if (img_i2c_timings[mode].tsdh % clk_period)
		tsdh++;

	if (tsdh > 1)
		data = tsdh - 1;
	else
		data = 0x01;
	scb_write_reg(i2c->reg_base, SCB_TIME_TSDH_REG, data);

	/* This value is used later. */
	tsdh = data;

	/* Setup TPL value. */
	data = img_i2c_timings[mode].tpl / clk_period;
	if (data > 0)
		--data;
	scb_write_reg(i2c->reg_base, SCB_TIME_TPL_REG, data);

	/* Setup TPH value. */
	data = img_i2c_timings[mode].tph / clk_period;
	if (data > 0)
		--data;
	scb_write_reg(i2c->reg_base, SCB_TIME_TPH_REG, data);

	/* Setup TSDL value to TPL + TSDH + 2. */
	scb_write_reg(i2c->reg_base, SCB_TIME_TSDL_REG, data + tsdh + 2);

	/* Setup TP2S value. */
	data = img_i2c_timings[mode].tp2s / clk_period;
	if (data > 0)
		--data;
	scb_write_reg(i2c->reg_base, SCB_TIME_TP2S_REG, data);

	scb_write_reg(i2c->reg_base, SCB_TIME_TBI_REG, TIMEOUT_TBI);
	scb_write_reg(i2c->reg_base, SCB_TIME_TSL_REG, TIMEOUT_TSL);
	scb_write_reg(i2c->reg_base, SCB_TIME_TDL_REG, TIMEOUT_TDL);

	/* Take module out of soft reset and enable clocks. */
	img_scb_soft_reset(i2c);

	/* Disable all interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, 0);

	/* Clear all interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_CLEAR_REG, 0xffffffff);

	/* Clear the scb_line_status events. */
	scb_write_reg(i2c->reg_base, SCB_CLEAR_REG, 0xffffffff);

	/* Enable interrupts */
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, i2c->int_enable);

	dev_info(i2c->adap.dev.parent,
		 "IMG I2C adapter (%d.%d.%d.%d) probed successfully"
			" (%s %d bps)\n",
		 (revision >> 24) & 0xff, (revision >> 16) & 0xff,
		 (revision >> 8) & 0xff, revision & 0xff,
		 img_i2c_timings[mode].name,
		 1000000000/(int_bitrate*clk_period));

	/* Reset the bus */
	ret = i2c_img_reset_bus(i2c);
out:
	return ret;
}

#if 0 //def CONFIG_I2C_DMA_SUPPORT
static int img_scb_init_dma(struct platform_device *pdev,
			    struct spfi_driver *drv_data,
			    struct device *dev) {
	int status;

	drv_data->rxchan = dma_request_slave_channel(&pdev->dev,
						     "rx");

	if (!drv_data->rxchan) {
		dev_err(dev, "Failed to get scb DMA rx channel\n");
		status = -EBUSY;
		goto out;
	}

	drv_data->txchan = dma_request_slave_channel(&pdev->dev,
						     "tx");

	if (!drv_data->txchan) {
		dev_err(dev, "Failed to get scb DMA tx channel\n");
		goto free_rx;
	}

	/* Allocate necessary coherent buffers */

	drv_data->rx_buf = dma_alloc_coherent(dev, IMG_SPFI_MAX_TRANSFER,
					      &drv_data->rx_dma_start,
					      GFP_KERNEL);

	if (!drv_data->rx_buf) {
		dev_err(dev, "failed to alloc read dma buffer\n");
		status = -ENOMEM;
		goto free_tx;
	}

	drv_data->tx_buf = dma_alloc_coherent(dev, IMG_SPFI_MAX_TRANSFER,
					      &drv_data->tx_dma_start,
					      GFP_KERNEL);

	if (!drv_data->tx_buf) {
		dev_err(dev, "failed to alloc write dma buffer\n");
		status = -ENOMEM;
		goto free_buf;
	}


	return 0;

free_buf:
	dma_free_coherent(dev, IMG_SPFI_MAX_TRANSFER, drv_data->rx_buf,
			  drv_data->rx_dma_start);
free_tx:
	dma_release_channel(drv_data->txchan);
free_rx:
	dma_release_channel(drv_data->rxchan);
out:
	return status;
}
#endif //CONFIG_I2C_DMA_SUPPORT


static int img_scb_xfer(struct i2c_adapter *i2c_adap, struct i2c_msg *msgs,
			int num)
{
	struct img_i2c *i2c = i2c_get_adapdata(i2c_adap);
	int ret = num;
	int i;
	int atomic;

	if (i2c->mode == MODE_FATAL)
		return -EIO;

	/*
	 * Decide whether to use automatic or atomic mode.
	 * Due to repeated starts we can't really mix and match.
	 */
	atomic = (i2c->quirks & QUIRK_ATOMIC_ONLY);
	for (i = 0; i < num; i++) {
		if (likely(msgs[i].len))
			continue;
		/*
		 * 0 byte reads are not possible because the slave could try
		 * and pull the data line low, preventing a stop bit.
		 */
		if (unlikely(msgs[i].flags & I2C_M_RD))
			return -EIO;
		/*
		 * 0 byte writes are possible and used for probing, but we
		 * cannot do them in automatic mode, so use atomic mode
		 * instead.
		 */
		atomic = 1;
	}

	/* re-initialise the device */
	if (i2c->mode == MODE_SUSPEND)
		img_scb_init(i2c);

#ifdef CONFIG_I2C_IMG_DEBUG_BUFFER
	i2c->irq_buf_index = 0;
	i2c->start_time = (unsigned int)local_clock();
#endif

	for (i = 0; i < num; i++) {
		struct i2c_msg *msg = &msgs[i];
		unsigned long flags;

		spin_lock_irqsave(&i2c->main_lock, flags);

		/*
		 * Make a copy of the message struct. We mustn't modify the
		 * original or we'll confuse drivers and i2c-dev.
		 */
		i2c->msg = *msg;
		i2c->status = 0;
		/*
		 * After the last message we must have waited for a stop bit.
		 * Not waiting can cause problems when the clock is disabled
		 * before the stop bit is sent, and the linux I2C interface
		 * requires separate transfers not to joined with repeated
		 * start.
		 */
		i2c->last_msg = (i == num-1);
		dev_dbg(i2c->adap.dev.parent,
			"msg %c %#x %d (%d/%d last=%d)\n",
			(msg->flags & I2C_M_RD) ? 'R' : 'W',
			(int)msg->addr, (int)msg->len,
			i, num, i2c->last_msg);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
        INIT_COMPLETION(i2c->xfer_done);
#else
		reinit_completion(&i2c->xfer_done);
#endif

		if (atomic)
			img_scb_atomic_start(i2c);
		else if (msg->flags & I2C_M_RD)
			img_scb_read(i2c);
		else
			img_scb_write(i2c);

		spin_unlock_irqrestore(&i2c->main_lock, flags);

#ifdef IMG_SCB_POLLING_MODE
		wait_until_transfer_done(i2c);
#endif
		if ( wait_for_completion_timeout(&i2c->xfer_done, HZ) == 0)
		{
		    dev_err(i2c->adap.dev.parent, "Timeout - waiting for completion\n");
		    dev_err(i2c->adap.dev.parent, "i2c->mode 0x%X\n", i2c->mode);
		    img_scb_switch_mode(i2c, MODE_SUSPEND); //so next img_scb_xfer()
		                                              //reinit it
		}

		del_timer_sync(&i2c->check_timer);

		if (i2c->status) {
			ret = i2c->status;
			break;
		}
	}

	return ret;
}

static const struct i2c_algorithm i2c_img_algorithm = {
	.master_xfer = img_scb_xfer,
	.functionality = img_scb_func,
};

int i2c_img_probe(struct pci_dev *dev, uintptr_t i2c_base_addr, int irq)
{
	struct img_i2c *i2c;
	int ret;
	struct device_node *node = dev->dev.of_node;
	u32 val;

	i2c = kzalloc(sizeof(struct img_i2c), GFP_KERNEL);
	if (!i2c) {
		ret = -ENOMEM;
		goto out_error_kmalloc;
	}

	i2c->adap.owner = THIS_MODULE;
	i2c->adap.class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c->adap.algo = &i2c_img_algorithm;
	i2c->adap.retries = 5;
	i2c->adap.timeout = msecs_to_jiffies(5000);

	/*
	 * Get the bus number from the device tree. Other I2C adapters may be
	 * reserved for non-Linux cores.
	 */
	ret = of_property_read_u32(node, "linux,i2c-index", &val);
	if (!ret)
		i2c->adap.nr = val;
	else
		i2c->adap.nr = -1;
	snprintf(i2c->adap.name, sizeof(i2c->adap.name),
		 "ImgTec SCB I2C %d", i2c->adap.nr);

	i2c->reg_base = (void __iomem*)i2c_base_addr;

#if 0 //def CONFIG_I2C_DMA_SUPPORT
	if (img_scb_init_dma(pdev, drv_data, dev) < 0)
	{
		dev_err(dev,
				"Failed to allocated tx/rx DMA channels\n");
		status = -ENOMEM;
		goto out_error_queue_alloc;
	}
#endif  //CONFIG_I2C_DMA_SUPPORT

#ifndef IMG_SCB_POLLING_MODE
	ret = request_irq(irq, img_scb_irq_handler, IRQF_SHARED, i2c->adap.name, i2c);
	if (ret)
		goto out_error_irq;
#endif

	/* Set up the exception check timer */
	init_timer(&i2c->check_timer);
	i2c->check_timer.function = img_scb_check_timer;
	i2c->check_timer.data = (unsigned long)i2c;

#ifdef IMG_SCB_POLLING_MODE
	i2c->irq = -1;
#else
	i2c->irq = irq;
#endif

	i2c->bitrate = 400000; /* fast mode */
	if (!of_property_read_u32(node, "bit-rate", &val))
		i2c->bitrate = val;
	i2c->busdelay = 0;
	if (!of_property_read_u32(node, "bus-delay", &val))
		i2c->busdelay = val;
	i2c->quirks = 0;
	if (!of_property_read_u32(node, "quirks", &val))
		i2c->quirks = val;

	i2c_set_adapdata(&i2c->adap, i2c);
	i2c->adap.dev.parent = &dev->dev;
	i2c->adap.dev.of_node = node;

	img_scb_switch_mode(i2c, MODE_INACTIVE);
	spin_lock_init(&i2c->main_lock);
	init_completion(&i2c->xfer_done);

	pci_set_drvdata(dev, i2c);

	ret = img_scb_init(i2c);
	if (ret)
		goto out_error_i2c;

	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret < 0) {
		dev_err(&dev->dev, "failed to add bus\n");
		goto out_error_i2c;
	}

	return 0;
out_error_i2c:
#ifndef IMG_SCB_POLLING_MODE
	free_irq(i2c->irq, i2c);
out_error_irq:
#endif
	kfree(i2c);
out_error_kmalloc:
	return ret;
}

int i2c_img_remove(struct pci_dev *dev)
{
	struct img_i2c *i2c = pci_get_drvdata(dev);

	i2c_del_adapter(&i2c->adap);

	/* Release DMA */
#if 0 //def CONFIG_I2C_DMA_SUPPORT
	dma_release_channel(drv_data->rxchan);
	dma_release_channel(drv_data->txchan);
#endif // CONFIG_I2C_DMA_SUPPORT

	free_irq(i2c->irq, i2c);
	kfree(i2c);

	return 0;
}

int i2c_img_suspend_noirq(struct pci_dev *dev)
{
	struct img_i2c *i2c = pci_get_drvdata(dev);
	if ( i2c == NULL )
	{
		dev_err(&dev->dev, "img_i2c not allocated yet\n");
	 	return -EFAULT;
	}

	/*
	 * Go into suspend state. The device will be reinitialised on the next
	 * transfer, or on resume.
	 */
	i2c_lock_adapter(&i2c->adap);
	/* Disable all interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_MASK_REG, 0);

	/* Clear all interrupts. */
	scb_write_reg(i2c->reg_base, SCB_INT_CLEAR_REG, 0xffffffff);

	/* Clear the scb_line_status events. */
	scb_write_reg(i2c->reg_base, SCB_CLEAR_REG, 0xffffffff);
	img_scb_switch_mode(i2c, MODE_SUSPEND);
	i2c_unlock_adapter(&i2c->adap);

	return 0;
}

int i2c_img_resume(struct pci_dev *dev)
{
	struct img_i2c *i2c = pci_get_drvdata(dev);
	if ( i2c == NULL )
	{
		dev_err(&dev->dev, "img_i2c not allocated yet\n");
		return -EFAULT;
	}

	/* Ensure the device has been reinitialised. */
	i2c_lock_adapter(&i2c->adap);

	/* re-initialise the device */
	if (i2c->mode == MODE_SUSPEND)
	{
	    img_scb_init(i2c);
	}

	i2c_unlock_adapter(&i2c->adap);
	return 0;
}
