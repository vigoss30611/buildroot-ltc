/*
 * mipi-csi-receiver-interface.c
 * INFOTM MIPI CSI
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/memory.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/videodev2.h>
#include <media/infotm__fimc.h>
#include <media/v4l2-of.h>
#include <media/v4l2-subdev.h>

#include "mipi-csi-receiver-interface.h"

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Debug level (0-2)");

/* Register map definition */

/* CSI global control */
#define INFOTM_CSI_CTRL			0x00
#define INFOTM_CSI_CTRL_DPDN_DEFAULT	(0 << 31)
#define INFOTM_CSI_CTRL_DPDN_SWAP		(1 << 31)
#define INFOTM_CSI_CTRL_ALIGN_32BIT	(1 << 20)
#define INFOTM_CSI_CTRL_UPDATE_SHADOW	(1 << 16)
#define INFOTM_CSI_CTRL_WCLK_EXTCLK	(1 << 8)
#define INFOTM_CSI_CTRL_RESET		(1 << 4)
#define INFOTM_CSI_CTRL_ENABLE		(1 << 0)

/* D-PHY control */
#define INFOTM_CSI_DPHYCTRL		0x04
#define INFOTM_CSI_DPHYCTRL_HSS_MASK	(0x1f << 27)
#define INFOTM_CSI_DPHYCTRL_ENABLE		(0x1f << 0)

#define INFOTM_CSI_CONFIG			0x08
#define INFOTM_CSI_CFG_FMT_YCBCR422_8BIT	(0x1e << 2)
#define INFOTM_CSI_CFG_FMT_RAW8		(0x2a << 2)
#define INFOTM_CSI_CFG_FMT_RAW10		(0x2b << 2)
#define INFOTM_CSI_CFG_FMT_RAW12		(0x2c << 2)
/* User defined formats, x = 1...4 */
#define INFOTM_CSI_CFG_FMT_USER(x)		((0x30 + x - 1) << 2)
#define INFOTM_CSI_CFG_FMT_MASK		(0x3f << 2)
#define INFOTM_CSI_CFG_NR_LANE_MASK	3

/* Interrupt mask */
#define INFOTM_CSI_INTMSK			0x10
#define INFOTM_CSI_INTMSK_EN_ALL		0xf000103f
#define INFOTM_CSI_INTMSK_EVEN_BEFORE	(1 << 31)
#define INFOTM_CSI_INTMSK_EVEN_AFTER	(1 << 30)
#define INFOTM_CSI_INTMSK_ODD_BEFORE	(1 << 29)
#define INFOTM_CSI_INTMSK_ODD_AFTER	(1 << 28)
#define INFOTM_CSI_INTMSK_ERR_SOT_HS	(1 << 12)
#define INFOTM_CSI_INTMSK_ERR_LOST_FS	(1 << 5)
#define INFOTM_CSI_INTMSK_ERR_LOST_FE	(1 << 4)
#define INFOTM_CSI_INTMSK_ERR_OVER		(1 << 3)
#define INFOTM_CSI_INTMSK_ERR_ECC		(1 << 2)
#define INFOTM_CSI_INTMSK_ERR_CRC		(1 << 1)
#define INFOTM_CSI_INTMSK_ERR_UNKNOWN	(1 << 0)

/* Interrupt source */
#define INFOTM_CSI_INTSRC			0x14
#define INFOTM_CSI_INTSRC_EVEN_BEFORE	(1 << 31)
#define INFOTM_CSI_INTSRC_EVEN_AFTER	(1 << 30)
#define INFOTM_CSI_INTSRC_EVEN		(0x3 << 30)
#define INFOTM_CSI_INTSRC_ODD_BEFORE	(1 << 29)
#define INFOTM_CSI_INTSRC_ODD_AFTER	(1 << 28)
#define INFOTM_CSI_INTSRC_ODD		(0x3 << 28)
#define INFOTM_CSI_INTSRC_NON_IMAGE_DATA	(0xff << 28)
#define INFOTM_CSI_INTSRC_ERR_SOT_HS	(0xf << 12)
#define INFOTM_CSI_INTSRC_ERR_LOST_FS	(1 << 5)
#define INFOTM_CSI_INTSRC_ERR_LOST_FE	(1 << 4)
#define INFOTM_CSI_INTSRC_ERR_OVER		(1 << 3)
#define INFOTM_CSI_INTSRC_ERR_ECC		(1 << 2)
#define INFOTM_CSI_INTSRC_ERR_CRC		(1 << 1)
#define INFOTM_CSI_INTSRC_ERR_UNKNOWN	(1 << 0)
#define INFOTM_CSI_INTSRC_ERRORS		0xf03f

/* Pixel resolution */
#define INFOTM_CSI_RESOL			0x2c
#define CSI_MAX_PIX_WIDTH		0xffff
#define CSI_MAX_PIX_HEIGHT		0xffff

/* Non-image packet data buffers */
#define INFOTM_CSI_PKTDATA_ODD		0x2000
#define INFOTM_CSI_PKTDATA_EVEN		0x3000
#define INFOTM_CSI_PKTDATA_SIZE		SZ_4K

enum {
	CSI_CLK_MUX,
	CSI_CLK_GATE,
};

static char *csi_clock_name[] = {
	[CSI_CLK_MUX]  = "sclk_csi",
	[CSI_CLK_GATE] = "csi",
};
#define NUM_CSI_CLOCKS	ARRAY_SIZE(csi_clock_name)
#define DEFAULT_SCLK_CSI_FREQ	166000000UL

static const char * const csi_supply_name[] = {
	"vddcore",  /* CSI Core (1.0V, 1.1V or 1.2V) suppply */
	"vddio",    /* CSI I/O and PLL (1.8V) supply */
};
#define CSI_NUM_SUPPLIES ARRAY_SIZE(csi_supply_name)

enum {
	ST_POWERED	= 1,
	ST_STREAMING	= 2,
	ST_SUSPENDED	= 4,
};

struct infotm_csi_event {
	u32 mask;
	const char * const name;
	unsigned int counter;
};

static const struct infotm_csi_event infotm_csi_events[] = {
	/* Errors */
	{ INFOTM_CSI_INTSRC_ERR_SOT_HS,	"SOT Error" },
	{ INFOTM_CSI_INTSRC_ERR_LOST_FS,	"Lost Frame Start Error" },
	{ INFOTM_CSI_INTSRC_ERR_LOST_FE,	"Lost Frame End Error" },
	{ INFOTM_CSI_INTSRC_ERR_OVER,	"FIFO Overflow Error" },
	{ INFOTM_CSI_INTSRC_ERR_ECC,	"ECC Error" },
	{ INFOTM_CSI_INTSRC_ERR_CRC,	"CRC Error" },
	{ INFOTM_CSI_INTSRC_ERR_UNKNOWN,	"Unknown Error" },
	/* Non-image data receive events */
	{ INFOTM_CSI_INTSRC_EVEN_BEFORE,	"Non-image data before even frame" },
	{ INFOTM_CSI_INTSRC_EVEN_AFTER,	"Non-image data after even frame" },
	{ INFOTM_CSI_INTSRC_ODD_BEFORE,	"Non-image data before odd frame" },
	{ INFOTM_CSI_INTSRC_ODD_AFTER,	"Non-image data after odd frame" },
};
#define INFOTM_CSI_NUM_EVENTS ARRAY_SIZE(infotm_csi_events)

struct csi_pktbuf {
	u32 *data;
	unsigned int len;
};

/**
 * struct csi_state - the driver's internal state data structure
 * @lock: mutex serializing the subdev and power management operations,
 *        protecting @format and @flags members
 * @pads: CSI pads array
 * @sd: v4l2_subdev associated with CSI device instance
 * @index: the hardware instance index
 * @pdev: CSI platform device
 * @regs: mmaped I/O registers memory
 * @supplies: CSI regulator supplies
 * @clock: CSI clocks
 * @irq: requested infotm_-mipi-csi irq number
 * @flags: the state variable for power and streaming control
 * @clock_frequency: device bus clock frequency
 * @hs_settle: HS-RX settle time
 * @num_lanes: number of MIPI-CSI data lanes used
 * @max_num_lanes: maximum number of MIPI-CSI data lanes supported
 * @wclk_ext: CSI wrapper clock: 0 - bus clock, 1 - external SCLK_CAM
 * @csi_fmt: current CSI pixel format
 * @format: common media bus format for the source and sink pad
 * @slock: spinlock protecting structure members below
 * @pkt_buf: the frame embedded (non-image) data buffer
 * @events: MIPI-CSI event (error) counters
 */
struct csi_state {
	struct mutex lock;
	struct media_pad pads[CSI_PADS_NUM];
	struct v4l2_subdev sd;
	u8 index;
	struct platform_device *pdev;
	void __iomem *regs;
	struct regulator_bulk_data supplies[CSI_NUM_SUPPLIES];
	struct clk *clock[NUM_CSI_CLOCKS];
	int irq;
	u32 flags;

	u32 clk_frequency;
	u32 hs_settle;
	u32 num_lanes;
	u32 max_num_lanes;
	u8 wclk_ext;

	const struct csi_pix_format *csi_fmt;
	struct v4l2_mbus_framefmt format;

	spinlock_t slock;
	struct csi_pktbuf pkt_buf;
	struct infotm_csi_event events[INFOTM_CSI_NUM_EVENTS];
};

/**
 * struct csi_pix_format - CSI pixel format description
 * @pix_width_alignment: horizontal pixel alignment, width will be
 *                       multiple of 2^pix_width_alignment
 * @code: corresponding media bus code
 * @fmt_reg: INFOTM_CSI_CONFIG register value
 * @data_alignment: MIPI-CSI data alignment in bits
 */
struct csi_pix_format {
	unsigned int pix_width_alignment;
	enum v4l2_mbus_pixelcode code;
	u32 fmt_reg;
	u8 data_alignment;
};

static const struct csi_pix_format infotm_csi_formats[] = {
	{
		.code = V4L2_MBUS_FMT_VYUY8_2X8,
		.fmt_reg = INFOTM_CSI_CFG_FMT_YCBCR422_8BIT,
		.data_alignment = 32,
	}, {
		.code = V4L2_MBUS_FMT_JPEG_1X8,
		.fmt_reg = INFOTM_CSI_CFG_FMT_USER(1),
		.data_alignment = 32,
	}, {
		.code = V4L2_MBUS_FMT_S5C_UYVY_JPEG_1X8,
		.fmt_reg = INFOTM_CSI_CFG_FMT_USER(1),
		.data_alignment = 32,
	}, {
		.code = V4L2_MBUS_FMT_SGRBG8_1X8,
		.fmt_reg = INFOTM_CSI_CFG_FMT_RAW8,
		.data_alignment = 24,
	}, {
		.code = V4L2_MBUS_FMT_SGRBG10_1X10,
		.fmt_reg = INFOTM_CSI_CFG_FMT_RAW10,
		.data_alignment = 24,
	}, {
		.code = V4L2_MBUS_FMT_SGRBG12_1X12,
		.fmt_reg = INFOTM_CSI_CFG_FMT_RAW12,
		.data_alignment = 24,
	}
};

#define infotm_csi_write(__csi, __r, __v) writel(__v, __csi->regs + __r)
#define infotm_csi_read(__csi, __r) readl(__csi->regs + __r)

static struct csi_state *sd_to_csi_state(struct v4l2_subdev *sdev)
{
	return container_of(sdev, struct csi_state, sd);
}

static const struct csi_pix_format *find_csi_format(
	struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(infotm_csi_formats); i++)
		if (mf->code == infotm_csi_formats[i].code)
			return &infotm_csi_formats[i];
	return NULL;
}

static void infotm_csi_enable_interrupts(struct csi_state *state, bool on)
{
	u32 val = infotm_csi_read(state, INFOTM_CSI_INTMSK);

	val = on ? val | INFOTM_CSI_INTMSK_EN_ALL :
		   val & ~INFOTM_CSI_INTMSK_EN_ALL;
	infotm_csi_write(state, INFOTM_CSI_INTMSK, val);
}

static void infotm_csi_reset(struct csi_state *state)
{
	u32 val = infotm_csi_read(state, INFOTM_CSI_CTRL);

	infotm_csi_write(state, INFOTM_CSI_CTRL, val | INFOTM_CSI_CTRL_RESET);
	udelay(10);
}

static void infotm_csi_system_enable(struct csi_state *state, int on)
{
	u32 val, mask;

	val = infotm_csi_read(state, INFOTM_CSI_CTRL);
	if (on)
		val |= INFOTM_CSI_CTRL_ENABLE;
	else
		val &= ~INFOTM_CSI_CTRL_ENABLE;
	infotm_csi_write(state, INFOTM_CSI_CTRL, val);

	val = infotm_csi_read(state, INFOTM_CSI_DPHYCTRL);
	val &= ~INFOTM_CSI_DPHYCTRL_ENABLE;
	if (on) {
		mask = (1 << (state->num_lanes + 1)) - 1;
		val |= (mask & INFOTM_CSI_DPHYCTRL_ENABLE);
	}
	infotm_csi_write(state, INFOTM_CSI_DPHYCTRL, val);
}

/* Called with the state.lock mutex held */
static void __infotm_csi_set_format(struct csi_state *state)
{
	struct v4l2_mbus_framefmt *mf = &state->format;
	u32 val;

	v4l2_dbg(1, debug, &state->sd, "fmt: %#x, %d x %d\n",
		 mf->code, mf->width, mf->height);

	/* Color format */
	val = infotm_csi_read(state, INFOTM_CSI_CONFIG);
	val = (val & ~INFOTM_CSI_CFG_FMT_MASK) | state->csi_fmt->fmt_reg;
	infotm_csi_write(state, INFOTM_CSI_CONFIG, val);

	/* Pixel resolution */
	val = (mf->width << 16) | mf->height;
	infotm_csi_write(state, INFOTM_CSI_RESOL, val);
}

static void infotm_csi_set_hsync_settle(struct csi_state *state, int settle)
{
	u32 val = infotm_csi_read(state, INFOTM_CSI_DPHYCTRL);

	val = (val & ~INFOTM_CSI_DPHYCTRL_HSS_MASK) | (settle << 27);
	infotm_csi_write(state, INFOTM_CSI_DPHYCTRL, val);
}

static void infotm_csi_set_params(struct csi_state *state)
{
	u32 val;

	val = infotm_csi_read(state, INFOTM_CSI_CONFIG);
	val = (val & ~INFOTM_CSI_CFG_NR_LANE_MASK) | (state->num_lanes - 1);
	infotm_csi_write(state, INFOTM_CSI_CONFIG, val);

	__infotm_csi_set_format(state);
	infotm_csi_set_hsync_settle(state, state->hs_settle);

	val = infotm_csi_read(state, INFOTM_CSI_CTRL);
	if (state->csi_fmt->data_alignment == 32)
		val |= INFOTM_CSI_CTRL_ALIGN_32BIT;
	else /* 24-bits */
		val &= ~INFOTM_CSI_CTRL_ALIGN_32BIT;

	val &= ~INFOTM_CSI_CTRL_WCLK_EXTCLK;
	if (state->wclk_ext)
		val |= INFOTM_CSI_CTRL_WCLK_EXTCLK;
	infotm_csi_write(state, INFOTM_CSI_CTRL, val);

	/* Update the shadow register. */
	val = infotm_csi_read(state, INFOTM_CSI_CTRL);
	infotm_csi_write(state, INFOTM_CSI_CTRL, val | INFOTM_CSI_CTRL_UPDATE_SHADOW);
}

static void infotm_csi_clk_put(struct csi_state *state)
{
	int i;

	for (i = 0; i < NUM_CSI_CLOCKS; i++) {
		if (IS_ERR(state->clock[i]))
			continue;
		clk_unprepare(state->clock[i]);
		clk_put(state->clock[i]);
		state->clock[i] = ERR_PTR(-EINVAL);
	}
}

static int infotm_csi_clk_get(struct csi_state *state)
{
	struct device *dev = &state->pdev->dev;
	int i, ret;

	for (i = 0; i < NUM_CSI_CLOCKS; i++)
		state->clock[i] = ERR_PTR(-EINVAL);

	for (i = 0; i < NUM_CSI_CLOCKS; i++) {
		state->clock[i] = clk_get(dev, csi_clock_name[i]);
		if (IS_ERR(state->clock[i])) {
			ret = PTR_ERR(state->clock[i]);
			goto err;
		}
		ret = clk_prepare(state->clock[i]);
		if (ret < 0) {
			clk_put(state->clock[i]);
			state->clock[i] = ERR_PTR(-EINVAL);
			goto err;
		}
	}
	return 0;
err:
	infotm_csi_clk_put(state);
	dev_err(dev, "failed to get clock: %s\n", csi_clock_name[i]);
	return ret;
}

static void dump_regs(struct csi_state *state, const char *label)
{
	struct {
		u32 offset;
		const char * const name;
	} registers[] = {
		{ 0x00, "CTRL" },
		{ 0x04, "DPHYCTRL" },
		{ 0x08, "CONFIG" },
		{ 0x0c, "DPHYSTS" },
		{ 0x10, "INTMSK" },
		{ 0x2c, "RESOL" },
		{ 0x38, "SDW_CONFIG" },
	};
	u32 i;

	v4l2_info(&state->sd, "--- %s ---\n", label);

	for (i = 0; i < ARRAY_SIZE(registers); i++) {
		u32 cfg = infotm_csi_read(state, registers[i].offset);
		v4l2_info(&state->sd, "%10s: 0x%08x\n", registers[i].name, cfg);
	}
}

static void infotm_csi_start_stream(struct csi_state *state)
{
	infotm_csi_reset(state);
	infotm_csi_set_params(state);
	infotm_csi_system_enable(state, true);
	infotm_csi_enable_interrupts(state, true);
}

static void infotm_csi_stop_stream(struct csi_state *state)
{
	infotm_csi_enable_interrupts(state, false);
	infotm_csi_system_enable(state, false);
}

static void infotm_csi_clear_counters(struct csi_state *state)
{
	unsigned long flags;
	int i;

	spin_lock_irqsave(&state->slock, flags);
	for (i = 0; i < INFOTM_CSI_NUM_EVENTS; i++)
		state->events[i].counter = 0;
	spin_unlock_irqrestore(&state->slock, flags);
}

static void infotm_csi_log_counters(struct csi_state *state, bool non_errors)
{
	int i = non_errors ? INFOTM_CSI_NUM_EVENTS : INFOTM_CSI_NUM_EVENTS - 4;
	unsigned long flags;

	spin_lock_irqsave(&state->slock, flags);

	for (i--; i >= 0; i--) {
		if (state->events[i].counter > 0 || debug)
			v4l2_info(&state->sd, "%s events: %d\n",
				  state->events[i].name,
				  state->events[i].counter);
	}
	spin_unlock_irqrestore(&state->slock, flags);
}

/*
 * V4L2 subdev operations
 */
static int infotm_csi_s_power(struct v4l2_subdev *sd, int on)
{
	struct csi_state *state = sd_to_csi_state(sd);
	struct device *dev = &state->pdev->dev;

	if (on)
		return pm_runtime_get_sync(dev);

	return pm_runtime_put_sync(dev);
}

static int infotm_csi_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct csi_state *state = sd_to_csi_state(sd);
	int ret = 0;

	v4l2_dbg(1, debug, sd, "%s: %d, state: 0x%x\n",
		 __func__, enable, state->flags);

	if (enable) {
		infotm_csi_clear_counters(state);
		ret = pm_runtime_get_sync(&state->pdev->dev);
		if (ret && ret != 1)
			return ret;
	}

	mutex_lock(&state->lock);
	if (enable) {
		if (state->flags & ST_SUSPENDED) {
			ret = -EBUSY;
			goto unlock;
		}
		infotm_csi_start_stream(state);
		state->flags |= ST_STREAMING;
	} else {
		infotm_csi_stop_stream(state);
		state->flags &= ~ST_STREAMING;
		if (debug > 0)
			infotm_csi_log_counters(state, true);
	}
unlock:
	mutex_unlock(&state->lock);
	if (!enable)
		pm_runtime_put(&state->pdev->dev);

	return ret == 1 ? 0 : ret;
}

static int infotm_csi_enum_mbus_code(struct v4l2_subdev *sd,
				  struct v4l2_subdev_fh *fh,
				  struct v4l2_subdev_mbus_code_enum *code)
{
	if (code->index >= ARRAY_SIZE(infotm_csi_formats))
		return -EINVAL;

	code->code = infotm_csi_formats[code->index].code;
	return 0;
}

static struct csi_pix_format const *infotm_csi_try_format(
	struct v4l2_mbus_framefmt *mf)
{
	struct csi_pix_format const *csi_fmt;

	csi_fmt = find_csi_format(mf);
	if (csi_fmt == NULL)
		csi_fmt = &infotm_csi_formats[0];

	mf->code = csi_fmt->code;
	v4l_bound_align_image(&mf->width, 1, CSI_MAX_PIX_WIDTH,
			      csi_fmt->pix_width_alignment,
			      &mf->height, 1, CSI_MAX_PIX_HEIGHT, 1,
			      0);
	return csi_fmt;
}

static struct v4l2_mbus_framefmt *__infotm_csi_get_format(
		struct csi_state *state, struct v4l2_subdev_fh *fh,
		enum v4l2_subdev_format_whence which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return fh ? v4l2_subdev_get_try_format(fh, 0) : NULL;

	return &state->format;
}

static int infotm_csi_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	struct csi_state *state = sd_to_csi_state(sd);
	struct csi_pix_format const *csi_fmt;
	struct v4l2_mbus_framefmt *mf;

	mf = __infotm_csi_get_format(state, fh, fmt->which);

	if (fmt->pad == CSI_PAD_SOURCE) {
		if (mf) {
			mutex_lock(&state->lock);
			fmt->format = *mf;
			mutex_unlock(&state->lock);
		}
		return 0;
	}
	csi_fmt = infotm_csi_try_format(&fmt->format);
	if (mf) {
		mutex_lock(&state->lock);
		*mf = fmt->format;
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE)
			state->csi_fmt = csi_fmt;
		mutex_unlock(&state->lock);
	}
	return 0;
}

static int infotm_csi_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	struct csi_state *state = sd_to_csi_state(sd);
	struct v4l2_mbus_framefmt *mf;

	mf = __infotm_csi_get_format(state, fh, fmt->which);
	if (!mf)
		return -EINVAL;

	mutex_lock(&state->lock);
	fmt->format = *mf;
	mutex_unlock(&state->lock);
	return 0;
}

static int infotm_csi_s_rx_buffer(struct v4l2_subdev *sd, void *buf,
			       unsigned int *size)
{
	struct csi_state *state = sd_to_csi_state(sd);
	unsigned long flags;

	*size = min_t(unsigned int, *size, INFOTM_CSI_PKTDATA_SIZE);

	spin_lock_irqsave(&state->slock, flags);
	state->pkt_buf.data = buf;
	state->pkt_buf.len = *size;
	spin_unlock_irqrestore(&state->slock, flags);

	return 0;
}

static int infotm_csi_log_status(struct v4l2_subdev *sd)
{
	struct csi_state *state = sd_to_csi_state(sd);

	mutex_lock(&state->lock);
	infotm_csi_log_counters(state, true);
	if (debug && (state->flags & ST_POWERED))
		dump_regs(state, __func__);
	mutex_unlock(&state->lock);
	return 0;
}

static int infotm_csi_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);

	format->colorspace = V4L2_COLORSPACE_JPEG;
	format->code = infotm_csi_formats[0].code;
	format->width = INFOTM_CSI_DEF_PIX_WIDTH;
	format->height = INFOTM_CSI_DEF_PIX_HEIGHT;
	format->field = V4L2_FIELD_NONE;

	return 0;
}

static const struct v4l2_subdev_internal_ops infotm_csi_sd_internal_ops = {
	.open = infotm_csi_open,
};

static struct v4l2_subdev_core_ops infotm_csi_core_ops = {
	.s_power = infotm_csi_s_power,
	.log_status = infotm_csi_log_status,
};

static struct v4l2_subdev_pad_ops infotm_csi_pad_ops = {
	.enum_mbus_code = infotm_csi_enum_mbus_code,
	.get_fmt = infotm_csi_get_fmt,
	.set_fmt = infotm_csi_set_fmt,
};

static struct v4l2_subdev_video_ops infotm_csi_video_ops = {
	.s_rx_buffer = infotm_csi_s_rx_buffer,
	.s_stream = infotm_csi_s_stream,
};

static struct v4l2_subdev_ops infotm_csi_subdev_ops = {
	.core = &infotm_csi_core_ops,
	.pad = &infotm_csi_pad_ops,
	.video = &infotm_csi_video_ops,
};

static irqreturn_t infotm_csi_irq_handler(int irq, void *dev_id)
{
	struct csi_state *state = dev_id;
	struct csi_pktbuf *pktbuf = &state->pkt_buf;
	unsigned long flags;
	u32 status;

	status = infotm_csi_read(state, INFOTM_CSI_INTSRC);
	spin_lock_irqsave(&state->slock, flags);

	if ((status & INFOTM_CSI_INTSRC_NON_IMAGE_DATA) && pktbuf->data) {
		u32 offset;

		if (status & INFOTM_CSI_INTSRC_EVEN)
			offset = INFOTM_CSI_PKTDATA_EVEN;
		else
			offset = INFOTM_CSI_PKTDATA_ODD;

		memcpy(pktbuf->data, state->regs + offset, pktbuf->len);
		pktbuf->data = NULL;
		rmb();
	}

	/* Update the event/error counters */
	if ((status & INFOTM_CSI_INTSRC_ERRORS) || debug) {
		int i;
		for (i = 0; i < INFOTM_CSI_NUM_EVENTS; i++) {
			if (!(status & state->events[i].mask))
				continue;
			state->events[i].counter++;
			v4l2_dbg(2, debug, &state->sd, "%s: %d\n",
				 state->events[i].name,
				 state->events[i].counter);
		}
		v4l2_dbg(2, debug, &state->sd, "status: %08x\n", status);
	}
	spin_unlock_irqrestore(&state->slock, flags);

	infotm_csi_write(state, INFOTM_CSI_INTSRC, status);
	return IRQ_HANDLED;
}

static int infotm_csi_get_platform_data(struct platform_device *pdev,
				     struct csi_state *state)
{
	struct infotm__platform_mipi_csi *pdata = pdev->dev.platform_data;

	if (pdata == NULL) {
		dev_err(&pdev->dev, "Platform data not specified\n");
		return -EINVAL;
	}

	state->clk_frequency = pdata->clk_rate;
	state->num_lanes = pdata->lanes;
	state->hs_settle = pdata->hs_settle;
	state->index = max(0, pdev->id);
	state->max_num_lanes = state->index ? CSI1_MAX_LANES :
					      CSI0_MAX_LANES;
	return 0;
}

#ifdef CONFIG_OF
static int infotm_csi_parse_dt(struct platform_device *pdev,
			    struct csi_state *state)
{
	struct device_node *node = pdev->dev.of_node;
	struct v4l2_of_endpoint endpoint;

	if (of_property_read_u32(node, "clock-frequency",
				 &state->clk_frequency))
		state->clk_frequency = DEFAULT_SCLK_CSI_FREQ;
	if (of_property_read_u32(node, "bus-width",
				 &state->max_num_lanes))
		return -EINVAL;

	node = v4l2_of_get_next_endpoint(node, NULL);
	if (!node) {
		dev_err(&pdev->dev, "No port node at %s\n",
				pdev->dev.of_node->full_name);
		return -EINVAL;
	}
	/* Get port node and validate MIPI-CSI channel id. */
	v4l2_of_parse_endpoint(node, &endpoint);

	state->index = endpoint.port - FIMC_INPUT_MIPI_CSI2_0;
	if (state->index < 0 || state->index >= CSI_MAX_ENTITIES)
		return -ENXIO;

	/* Get MIPI CSI-2 bus configration from the endpoint node. */
	of_property_read_u32(node, "samsung,csi-hs-settle",
					&state->hs_settle);
	state->wclk_ext = of_property_read_bool(node,
					"samsung,csi-wclk");

	state->num_lanes = endpoint.bus.mipi_csi2.num_data_lanes;

	of_node_put(node);
	return 0;
}
#else
#define infotm_csi_parse_dt(pdev, state) (-ENOSYS)
#endif

static int infotm_csi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *mem_res;
	struct csi_state *state;
	int ret = -ENOMEM;
	int i;

	state = devm_kzalloc(dev, sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	mutex_init(&state->lock);
	spin_lock_init(&state->slock);
	state->pdev = pdev;

	if (dev->of_node)
		ret = infotm_csi_parse_dt(pdev, state);
	else
		ret = infotm_csi_get_platform_data(pdev, state);
	if (ret < 0)
		return ret;

	if (state->num_lanes == 0 || state->num_lanes > state->max_num_lanes) {
		dev_err(dev, "Unsupported number of data lanes: %d (max. %d)\n",
			state->num_lanes, state->max_num_lanes);
		return -EINVAL;
	}

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	state->regs = devm_ioremap_resource(dev, mem_res);
	if (IS_ERR(state->regs))
		return PTR_ERR(state->regs);

	state->irq = platform_get_irq(pdev, 0);
	if (state->irq < 0) {
		dev_err(dev, "Failed to get irq\n");
		return state->irq;
	}

	for (i = 0; i < CSI_NUM_SUPPLIES; i++)
		state->supplies[i].supply = csi_supply_name[i];

	ret = devm_regulator_bulk_get(dev, CSI_NUM_SUPPLIES,
				 state->supplies);
	if (ret)
		return ret;

	ret = infotm_csi_clk_get(state);
	if (ret < 0)
		return ret;

	if (state->clk_frequency)
		ret = clk_set_rate(state->clock[CSI_CLK_MUX],
				   state->clk_frequency);
	else
		dev_WARN(dev, "No clock frequency specified!\n");
	if (ret < 0)
		goto e_clkput;

	ret = clk_enable(state->clock[CSI_CLK_MUX]);
	if (ret < 0)
		goto e_clkput;

	ret = devm_request_irq(dev, state->irq, infotm_csi_irq_handler,
			       0, dev_name(dev), state);
	if (ret) {
		dev_err(dev, "Interrupt request failed\n");
		goto e_clkdis;
	}

	v4l2_subdev_init(&state->sd, &infotm_csi_subdev_ops);
	state->sd.owner = THIS_MODULE;
	snprintf(state->sd.name, sizeof(state->sd.name), "%s.%d",
		 CSI_SUBDEV_NAME, state->index);
	state->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	state->csi_fmt = &infotm_csi_formats[0];

	state->format.code = infotm_csi_formats[0].code;
	state->format.width = INFOTM_CSI_DEF_PIX_WIDTH;
	state->format.height = INFOTM_CSI_DEF_PIX_HEIGHT;

	state->pads[CSI_PAD_SINK].flags = MEDIA_PAD_FL_SINK;
	state->pads[CSI_PAD_SOURCE].flags = MEDIA_PAD_FL_SOURCE;
	ret = media_entity_init(&state->sd.entity,
				CSI_PADS_NUM, state->pads, 0);
	if (ret < 0)
		goto e_clkdis;

	/* This allows to retrieve the platform device id by the host driver */
	v4l2_set_subdevdata(&state->sd, pdev);

	/* .. and a pointer to the subdev. */
	platform_set_drvdata(pdev, &state->sd);
	memcpy(state->events, infotm_csi_events, sizeof(state->events));
	pm_runtime_enable(dev);

	dev_info(&pdev->dev, "lanes: %d, hs_settle: %d, wclk: %d, freq: %u\n",
		 state->num_lanes, state->hs_settle, state->wclk_ext,
		 state->clk_frequency);
	return 0;

e_clkdis:
	clk_disable(state->clock[CSI_CLK_MUX]);
e_clkput:
	infotm_csi_clk_put(state);
	return ret;
}

static int infotm_csi_pm_suspend(struct device *dev, bool runtime)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct v4l2_subdev *sd = platform_get_drvdata(pdev);
	struct csi_state *state = sd_to_csi_state(sd);
	int ret = 0;

	v4l2_dbg(1, debug, sd, "%s: flags: 0x%x\n",
		 __func__, state->flags);

	mutex_lock(&state->lock);
	if (state->flags & ST_POWERED) {
		infotm_csi_stop_stream(state);
		ret = infotm__csi_phy_enable(state->index, false);
		if (ret)
			goto unlock;
		ret = regulator_bulk_disable(CSI_NUM_SUPPLIES,
					     state->supplies);
		if (ret)
			goto unlock;
		clk_disable(state->clock[CSI_CLK_GATE]);
		state->flags &= ~ST_POWERED;
		if (!runtime)
			state->flags |= ST_SUSPENDED;
	}
 unlock:
	mutex_unlock(&state->lock);
	return ret ? -EAGAIN : 0;
}

static int infotm_csi_pm_resume(struct device *dev, bool runtime)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct v4l2_subdev *sd = platform_get_drvdata(pdev);
	struct csi_state *state = sd_to_csi_state(sd);
	int ret = 0;

	v4l2_dbg(1, debug, sd, "%s: flags: 0x%x\n",
		 __func__, state->flags);

	mutex_lock(&state->lock);
	if (!runtime && !(state->flags & ST_SUSPENDED))
		goto unlock;

	if (!(state->flags & ST_POWERED)) {
		ret = regulator_bulk_enable(CSI_NUM_SUPPLIES,
					    state->supplies);
		if (ret)
			goto unlock;
		ret = infotm__csi_phy_enable(state->index, true);
		if (!ret) {
			state->flags |= ST_POWERED;
		} else {
			regulator_bulk_disable(CSI_NUM_SUPPLIES,
					       state->supplies);
			goto unlock;
		}
		clk_enable(state->clock[CSI_CLK_GATE]);
	}
	if (state->flags & ST_STREAMING)
		infotm_csi_start_stream(state);

	state->flags &= ~ST_SUSPENDED;
 unlock:
	mutex_unlock(&state->lock);
	return ret ? -EAGAIN : 0;
}

#ifdef CONFIG_PM_SLEEP
static int infotm_csi_suspend(struct device *dev)
{
	return infotm_csi_pm_suspend(dev, false);
}

static int infotm_csi_resume(struct device *dev)
{
	return infotm_csi_pm_resume(dev, false);
}
#endif

#ifdef CONFIG_PM_RUNTIME
static int infotm_csi_runtime_suspend(struct device *dev)
{
	return infotm_csi_pm_suspend(dev, true);
}

static int infotm_csi_runtime_resume(struct device *dev)
{
	return infotm_csi_pm_resume(dev, true);
}
#endif

static int infotm_csi_remove(struct platform_device *pdev)
{
	struct v4l2_subdev *sd = platform_get_drvdata(pdev);
	struct csi_state *state = sd_to_csi_state(sd);

	pm_runtime_disable(&pdev->dev);
	infotm_csi_pm_suspend(&pdev->dev, false);
	clk_disable(state->clock[CSI_CLK_MUX]);
	pm_runtime_set_suspended(&pdev->dev);
	infotm_csi_clk_put(state);

	media_entity_cleanup(&state->sd.entity);

	return 0;
}

static const struct dev_pm_ops infotm_csi_pm_ops = {
	SET_RUNTIME_PM_OPS(infotm_csi_runtime_suspend, infotm_csi_runtime_resume,
			   NULL)
	SET_SYSTEM_SLEEP_PM_OPS(infotm_csi_suspend, infotm_csi_resume)
};

static const struct of_device_id infotm_csi_of_match[] = {
	{ .compatible = "infotm,infotm-mipi-csi" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, infotm_csi_of_match);

static struct platform_driver infotm_csi_driver = {
	.probe		= infotm_csi_probe,
	.remove		= infotm_csi_remove,
	.driver		= {
		.of_match_table = infotm_csi_of_match,
		.name		= CSI_DRIVER_NAME,
		.owner		= THIS_MODULE,
		.pm		= &infotm_csi_pm_ops,
	},
};

module_platform_driver(infotm_csi_driver);

MODULE_AUTHOR("linyun.xiong <linyun.xiong@infotm.com.cn>");
MODULE_DESCRIPTION("INFOTM SoC MIPI-CSI2 receiver driver");
MODULE_LICENSE("GPL");
