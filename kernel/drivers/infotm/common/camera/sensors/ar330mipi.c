/*
 * A V4L2 driver for ar330 cameras.
 *
 * Copyright 2006 One Laptop Per Child Association, Inc.  Written
 * by Jonathan Corbet with substantial inspiration from Mark
 * McClelland's ovcamchip code.
 *
 * Copyright 2006-7 Jonathan Corbet <corbet@lwn.net>
 *
 * This file may be distributed under the terms of the GNU General
 * Public License, version 2.
 */


 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/slab.h>
 #include <linux/i2c.h>
 #include <linux/delay.h>
 #include <linux/videodev2.h>
 #include <media/v4l2-device.h>
 #include <media/v4l2-chip-ident.h>
 #include <media/v4l2-ctrls.h>
 #include <media/v4l2-mediabus.h>

#define SENSOR_MAX_HEIGHT   1920
#define SENSOR_MAX_WIDTH    1080

#define SENSOR_MIN_WIDTH    640
#define SENSOR_MIN_HEIGHT   480

struct reginfo {
	unsigned short reg;
	unsigned short val;
};

struct ar330_win_size {
	int width;
	int height;
	struct reginfo *regs;
};

struct ar330_info {
	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrls;
	struct i2c_client *client;
	enum v4l2_mbus_type bus_type;
	struct mutex		lock;

};

#define TEST_PARTERN

static struct reginfo sensor_init[] = {
    //mipi 30fps lanes = 2 for evb1.1
#if 1
    {0x301A, 0x0058},   // Disable Streaming
    {0x31AE, 0x0202},   // SERIAL_FORMAT
    {0x31AC, 0x0A0A},   // DATA_FORMAT_BITS = 10-bit
    {0x31B0, 0x0028},   // FRAME_PREAMBLE = 40 clocks
    {0x31B2, 0x000E},   // LINE_PREAMBLE = 14 clocks
    {0x31B4, 0x2743},   // MIPI_TIMING_0
    {0x31B6, 0x124E},   // MIPI_TIMING_1 //0x114E
    {0x31B8, 0x2049},   // MIPI_TIMING_2
    {0x31BA, 0x0186},   // MIPI_TIMING_3
    {0x31BC, 0x8005},   // MIPI_TIMING_4
    {0x31BE, 0x2003},   // MIPI_CONFIG_STATUS

    {0x302A, 0x0005},       //VT_PIX_CLK_DIV = 5
    {0x302C, 0x0002},   //VT_SYS_CLK_DIV = 2
    {0x302E, 0x0002},   //PRE_PLL_CLK_DIV = 3
    {0x3030, 0x0020},   //PLL_MULTIPLIER = 80
    {0x3036, 0x000A},   //OP_PIX_CLK_DIV = 10
    {0x3038, 0x0001},   //OP_SYS_CLK_DIV = 1

    {0x3002, 0x00EA},   //Y_ADDR_START = 234
    {0x3004, 0x00C6},   //X_ADDR_START = 198
    {0x3006, 0x0521},   //Y_ADDR_END = 1313
    {0x3008, 0x0845},   //X_ADDR_END = 2117
    {0x300A, 0x0450},   //FRAME_LENGTH_LINES = 1717
    {0x300C, 0x0486},   //LINE_LENGTH_PCK = 1242
    //{0x300A, 0x0444},     //FRAME_LENGTH_LINES = 1092
    //{0x300C, 0x07A1},     //LINE_LENGTH_PCK = 1953
    {0x3012, 0x0497},   //1091
    //{0x3012, 0x0612},     //COARSE_INTEGRATION_TIME = 1554
    {0x3014, 0x0000},       //FINE_INTEGRATION_TIME = 0
    {0x30A2, 0x0001},       //X_ODD_INC = 1
    {0x30A6, 0x0001},       //Y_ODD_INC = 1
    #if 0
    {0x308C, 0x0006},       //Y_ADDR_START_CB = 6
    {0x308A, 0x0006},       //X_ADDR_START_CB = 6
    {0x3090, 0x0605},       //Y_ADDR_END_CB = 1541
    {0x308E, 0x0905},       //X_ADDR_END_CB = 2309
    {0x30AA, 0x060C},       //FRAME_LENGTH_LINES_CB = 1548
    {0x303E, 0x04E0},       //LINE_LENGTH_PCK_CB = 1248
    {0x3016, 0x060B},       //COARSE_INTEGRATION_TIME_CB = 1547
    {0x3018, 0x0000},       //FINE_INTEGRATION_TIME_CB = 0
    {0x30AE, 0x0001},       //X_ODD_INC_CB = 1
    {0x30A8, 0x0001},       //Y_ODD_INC_CB = 1
    #endif
    ////
    {0x3040, 0x0000},       //READ_MODE = 0
    //{0x3042, 0x0333},     //EXTRA_DELAY = 819
    //{0x3042, 0x0291},       //EXTRA_DELAY = 657
    {0x30BA, 0x002C},       //DIGITAL_CTRL = 44

    //  //Recommended Configuration
    {0x31E0, 0x0303},
    {0x3064, 0x1802},
    {0x3ED2, 0x0146},
    {0x3ED4, 0x8F6C},
    {0x3ED6, 0x66CC},
    {0x3ED8, 0x8C42},
    {0x3064, 0x1802},
    {0x301A, 0x0058},
    {0x0000, 0x0000}
#else
    //[AR330-1088p 30fps 1lane Fvco = ? MHz] for qiwo
    {0x301A, 0x0058},     //RESET_REGISTER = 88
    {0x302A, 0x0005},     //VT_PIX_CLK_DIV = 5
    {0x302C, 0x0004},  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
    {0x302E, 0x0003},     //PRE_PLL_CLK_DIV = 1
    {0x3030, 0x0020},     //PLL_MULTIPLIER = 32
    {0x3036, 0x000A},     //OP_PIX_CLK_DIV = 10
    {0x3038, 0x0001},     //OP_SYS_CLK_DIV = 1
    {0x31AC, 0x0A0A},     //DATA_FORMAT_BITS = 2570
    {0x31AE, 0x0201},     //SERIAL_FORMAT = 514 ------ [0x201 1 lane] [0x202 2 lanes] [0x204 4 lanes]
    {0x31B0, 0x003E},     //FRAME_PREAMBLE = 62
    {0x31B2, 0x0018},     //LINE_PREAMBLE = 24
    {0x31B4, 0x4F66},     //MIPI_TIMING_0 = 20326
    {0x31B6, 0x4215},     //MIPI_TIMING_1 = 16917
    {0x31B8, 0x308B},     //MIPI_TIMING_2 = 12427
    {0x31BA, 0x028A},     //MIPI_TIMING_3 = 650
    {0x31BC, 0x8008},     //MIPI_TIMING_4 = 32776
    {0x3002, 0x00EA},     //Y_ADDR_START = 234
    {0x3004, 0x00C6},     //X_ADDR_START = 198
    {0x3006, 0x0521},     //Y_ADDR_END = 1313 //21->29 fengwu
    {0x3008, 0x0845},     //X_ADDR_END = 2117
    {0x300A, 0x0450},     //0x4bcFRAME_LENGTH_LINES = 1212
    {0x300C, 0x0484},     //0x840LINE_LENGTH_PCK = 2112//484
    {0x3012, 0x02D7},     //COARSE_INTEGRATION_TIME = 727
    {0x3014, 0x0000},     //FINE_INTEGRATION_TIME = 0
    {0x30A2, 0x0001},     //X_ODD_INC = 1
    {0x30A6, 0x0001},     //Y_ODD_INC = 1
    {0x3040, 0x0000},     //READ_MODE = 0
    {0x3042, 0x0080},     //EXTRA_DELAY = 128
    {0x30BA, 0x002C},     //DIGITAL_CTRL = 44
    {0x3088, 0x80BA},     // SEQ_CTRL_PORT
    {0x3086, 0x0253},     // SEQ_DATA_PORT

    {0x31E0, 0x0303},
    {0x3064, 0x1802},
    {0x3ED2, 0x0146},
    {0x3ED4, 0x8F6C},
    {0x3ED6, 0x66CC},
    {0x3ED8, 0x8C42},
    {0x3EDA, 0x88BC},
    {0x3EDC, 0xAA63},
    {0x305E, 0x00A0},
    {0x0000, 0x0000}
#endif
};


static struct reginfo sensor_vga[] = {


	{0x00, 0x00},
};

static struct reginfo sensor_svga[] = {

	{0x00, 0x00},
};

static struct reginfo sensor_xga[] = {


	{0x00, 0x00},
};


static struct reginfo sensor_uxga[] = {


	{0x00, 0x00},
};

static  struct reginfo  ev_neg4_regs[] = {

	{0x00, 0x00},
};
static  struct reginfo  ev_neg3_regs[] = {

	{0x00, 0x00},
};
static  struct reginfo  ev_neg2_regs[] = {

	{0x00, 0x00},
};
static  struct reginfo  ev_neg1_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  ev_zero_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  ev_pos1_regs[] = {

	{0x00, 0x00},
};
static  struct reginfo  ev_pos2_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  ev_pos3_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  ev_pos4_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  wb_auto_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  wb_incandescent_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  wb_flourscent_regs[] = {

	{0x00, 0x00},
};
static  struct reginfo  wb_daylight_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  wb_cloudy_regs[] = {

	{0x00, 0x00},
};

static  struct reginfo  scence_night60hz_regs[] = {

	{0x00, 0x00},
};


static  struct reginfo  scence_auto_regs[] = {

	{0x00, 0x00},
};


static  struct reginfo  scence_sunset60hz_regs[] = {

	{0x00, 0x00}

};

static struct reginfo scence_party_indoor_regs [] = {

	{0x00, 0x00}
};

static struct reginfo scence_sports_regs [] = {

	{0x00, 0x00}
};

static  struct reginfo  colorfx_none_regs[] = {

	{0x00, 0x00}
};
static  struct reginfo  colorfx_mono_regs[] = {

	{0x00, 0x00}
};
static  struct reginfo  colorfx_negative_regs[] = {

	{0x00, 0x00}
};

static  struct reginfo  colorfx_sepiagreen_regs[] = {

	{0x00, 0x00}
};

static  struct reginfo  colorfx_sepiablue_regs[] = {

	{0x00, 0x00}
};

static  struct reginfo  colorfx_sepia_regs[] = {

	{0x00, 0x00}
};

static struct reginfo antibanding_50hz_regs[] = {

	{0x00, 0x00},
};

static struct reginfo antibanding_60hz_regs[] = {

	{0x00, 0x00},
};

static struct reginfo  sensor_out_yuv[] = {
	{0x00, 0x00},
};


static const struct ar330_win_size ar330_win_sizes[] = {

	/* VGA */
	{
		.width = 640,
		.height = 480,
		.regs  = sensor_vga,
	},
	/* XGA */
	{
		.width = 1280,
		.height = 720,
		.regs = sensor_xga,
	},
	/* UXGA */
	{
		.width = 1920,
		.height = 1080,
		.regs  = sensor_uxga,
	}

};


/* only one fixed colorspace per pixelcode */
struct ar330_data_fmt {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	struct reginfo	*regs;
};

static const struct ar330_data_fmt ar330_data_fmts[] = {
	{
		.code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.regs	= sensor_out_yuv,
	},
};

static const struct ar330_data_fmt *sensor_find_datafmt(
		enum v4l2_mbus_pixelcode code,
		const struct ar330_data_fmt *fmt,
		int n)
{
	int i;
	for (i = 0; i < n; i++)
		if (fmt[i].code == code)
			return fmt + i;

	return fmt;
}

static const struct ar330_win_size *sensor_find_win(
		struct v4l2_mbus_framefmt *mf,
		const struct ar330_win_size *win,
		int n)
{
	int i;
	for (i = 0; i < n; i++)
		if (win[i].width == mf->width && win[i].height == mf->height)
			return win + i;

	return win;
}

static struct ar330_info *to_sensor(struct v4l2_subdev *sd)
{
	return container_of(sd, struct ar330_info, sd);
}

static u16 ar330_read(struct i2c_client *client, uint16_t reg)
{
    int err;
    struct i2c_msg msg[2];
    u8 buf[2];
    u16 val = 0;
    buf[0] = (reg >> 8) & 0xff;
    buf[1] = reg & 0xff;
    msg[0].addr = client->addr;
    msg[0].flags = I2C_M_NOSTART;
    msg[0].buf = &buf;
    msg[0].len = sizeof(buf);

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = &buf;
    msg[1].len = sizeof(buf);

    err = i2c_transfer(client->adapter, msg, 2);
    if (err != 2)
        pr_err("[ar330] read reg(0x%x)failed!\n", reg);
    val = buf[1] | (buf[0] << 8);
    return val;
}

static int ar330_write(struct i2c_client *client, uint16_t reg, uint16_t val)
{
	int err, cnt;
	u8 buf[4];
	struct i2c_msg msg[1];

	buf[0] = (reg >> 8 ) & 0xFF;
	buf[1] = reg & 0xFF;
	buf[2] = (val >> 8 ) & 0xFF;
	buf[3] = val & 0xFF;

	msg->addr = client->addr;
	msg->flags = I2C_M_NOSTART;
	msg->buf = buf;
	msg->len = sizeof(buf);

	err = -EAGAIN;
	err = i2c_transfer(client->adapter, msg, 1);
	if(1 != err) {
		pr_err("[ar330] write reg(0x%x, val:0x%x) failed, ecode=%d \
				\n", reg, val, err);
		return err;
	}
	//pr_err("[ar330] write reg(0x%x, val:0x%x) \
	                \n", reg, val);
	//pr_err("[ar330] read reg(0x%x, val:0x%x) \n", reg, ar330_read(client, reg));
	return 0;
}

static int ar330_write_array(struct i2c_client *client,
		struct reginfo *regarray)
{
	int i = 0, err;

	while ((regarray[i].reg != 0) || (regarray[i].val != 0)) {
		err = ar330_write(client, regarray[i].reg, regarray[i].val);
		if (err != 0) {
			pr_err("[ar330] I2c write %dth regs faild!\n", i);
			return err;
		}
		i++;
	}
	return 0;
}


#if 0
static const struct v4l2_subdev_pad_ops ar330_pad_ops = {
	.enum_mbus_code = ar330_enum_mbus_code,
	.enum_frame_size = ar330_enum_frame_sizes,
	.get_fmt = ar330_get_fmt,
	.set_fmt = ar330_set_fmt,
};
#endif

static int ar330_g_chip_ident(struct v4l2_subdev *sd,
		struct v4l2_dbg_chip_ident *chip)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_AR330, 0);
}


static int ar330_reset(struct v4l2_subdev *sd, u32 val)
{
	return 0;
}

static int is_sensor_ar330(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	u32 chip_id = 0;
	chip_id = ar330_read(client, 0x3000);
	pr_info("[ar330]-Sensor id 0x%x!\n", chip_id);
	if (0x2604 == chip_id)
		return 1;
	else
		return 0;
}

static int ar330_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (val)
		return !is_sensor_ar330(sd);
	else
		return ar330_write_array(client, sensor_init);

}
#ifdef CONFIG_VIDEO_ADV_DEBUG
static int ar330_g_register(struct v4l2_subdev *sd,
		struct v4l2_dbg_register *reg)
{
	pr_info("[ar330] %s\n", __func__);
	return 0;
}

static int ar330_s_register(struct v4l2_subdev *sd,
		const struct v4l2_dbg_register *reg)
{
	pr_info("[ar330] %s\n", __func__);
	return 0;
}
#endif


static int ar330_set_exposure(struct i2c_client *client,
		struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch (ctrl->val) {
		case -4:
			ret = ar330_write_array(client, ev_neg4_regs);
			break;
		case -3:
			ret = ar330_write_array(client, ev_neg3_regs);
			break;
		case -2:
			ret = ar330_write_array(client, ev_neg2_regs);
			break;
		case -1:
			ret = ar330_write_array(client, ev_neg1_regs);
			break;
		case 0:
			ret = ar330_write_array(client, ev_zero_regs);
			break;
		case 1:
			ret = ar330_write_array(client, ev_pos1_regs);
			break;
		case 2:
			ret = ar330_write_array(client, ev_pos2_regs);
			break;
		case 3:
			ret = ar330_write_array(client, ev_pos3_regs);
			break;
		case 4:
			ret = ar330_write_array(client, ev_pos4_regs);
			break;
		default:
			break;
	}
	return ret;
}


static int ar330_set_wb(struct i2c_client *client,  struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	pr_info("[ar330] %s\n", __func__);
	switch (ctrl->val) {
		case V4L2_WHITE_BALANCE_AUTO:
			ret = ar330_write_array(client, wb_auto_regs);
			break;
		case V4L2_WHITE_BALANCE_INCANDESCENT:
			ret = ar330_write_array(client, wb_incandescent_regs);
			break;
		case V4L2_WHITE_BALANCE_FLUORESCENT:
			ret = ar330_write_array(client, wb_flourscent_regs);
			break;
		case V4L2_WHITE_BALANCE_DAYLIGHT:
			ret = ar330_write_array(client, wb_daylight_regs);
			break;
		case V4L2_WHITE_BALANCE_CLOUDY:
			ret = ar330_write_array(client, wb_cloudy_regs);
			break;
		default:
			break;
	}
	return ret;
}


static int ar330_set_scence(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	pr_info("[ar330] %s\n", __func__);
	switch (ctrl->val) {
		case V4L2_SCENE_MODE_NONE:
			ret = ar330_write_array(client, scence_auto_regs);
			break;
		case V4L2_SCENE_MODE_NIGHT:
			ret = ar330_write_array(client, scence_night60hz_regs);
			break;
		case V4L2_SCENE_MODE_SUNSET:
			ret = ar330_write_array(client, scence_sunset60hz_regs);
			break;
		case V4L2_SCENE_MODE_PARTY_INDOOR:
			ret = ar330_write_array(client, scence_party_indoor_regs);
			break;
		case V4L2_SCENE_MODE_SPORTS:
			ret = ar330_write_array(client, scence_sports_regs);
			break;
		default:
			break;
	}
	return ret;

}



static int ar330_set_colorfx(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	pr_info("[ar330] %s\n", __func__);
	switch (ctrl->val) {
		case V4L2_COLORFX_NONE:
			ret = ar330_write_array(client, colorfx_none_regs);
			break;
		case V4L2_COLORFX_BW:
			ret = ar330_write_array(client, colorfx_mono_regs);
			break;
		case V4L2_COLORFX_SEPIA:
			ret = ar330_write_array(client, colorfx_sepia_regs);
			break;
		case V4L2_COLORFX_NEGATIVE:
			ret = ar330_write_array(client, colorfx_negative_regs);
			break;
		default:
			break;
	}
	return ret;
}



static int ar330_set_powlinefreq(struct i2c_client *client,
		struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	pr_info("[ar330] %s\n", __func__);
	switch (ctrl->val) {
		case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED:
		case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
		case V4L2_CID_POWER_LINE_FREQUENCY_AUTO:
			ret = ar330_write_array(client, antibanding_60hz_regs);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
			ret = ar330_write_array(client, antibanding_50hz_regs);
			break;
		default:
			break;
	}
	return ret;
}


static int ar330_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd =
		&container_of(ctrl->handler, struct ar330_info, ctrls)->sd;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = -1;
	pr_info("[ar330] %s\n", __func__);
	switch (ctrl->id) {
		case V4L2_CID_EXPOSURE_ABSOLUTE:
			ret = ar330_set_exposure(client, ctrl);
			break;
		case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
			ret = ar330_set_wb(client, ctrl);
			break;
		case V4L2_CID_SCENE_MODE:
			ret = ar330_set_scence(client, ctrl);
			break;
		case V4L2_CID_COLORFX:
			ret = ar330_set_colorfx(client, ctrl);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY:
			ret = ar330_set_powlinefreq(client, ctrl);
			break;
		default:
			break;
	}
	return ret;
}


static struct v4l2_ctrl_ops ar330_ctrl_ops = {
	.s_ctrl = ar330_s_ctrl,
};



static const struct v4l2_subdev_core_ops ar330_core_ops = {
	.g_chip_ident = ar330_g_chip_ident,
	.reset = ar330_reset,
	.init = ar330_init,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = ar330_g_register,
	.s_register = ar330_s_register,
#endif
};



static int ar330_enum_mbus_fmt(struct v4l2_subdev *sd, unsigned int index,
		enum v4l2_mbus_pixelcode *code)
{
	pr_info("[ar330] %s\n", __func__);
	if (index >= ARRAY_SIZE(ar330_data_fmts))
		return -EINVAL;
	*code = ar330_data_fmts[index].code;
	return 0;
}


static int ar330_try_mbus_fmt_internal(struct ar330_info *sensor,
		struct v4l2_mbus_framefmt *mf,
		struct ar330_data_fmt **ret_fmt,
		struct ar330_win_size  **ret_size)
{
	int ret = 0;

	if (ret_fmt != NULL)
		*ret_fmt = sensor_find_datafmt(mf->code, ar330_data_fmts,
				ARRAY_SIZE(ar330_data_fmts));
	if (ret_size != NULL)
		*ret_size = sensor_find_win(mf, ar330_win_sizes,
				ARRAY_SIZE(ar330_win_sizes));

	return ret;
}

static int ar330_try_mbus_fmt(struct v4l2_subdev *sd,
		struct v4l2_mbus_framefmt *mf)
{

	struct ar330_info *sensor = to_sensor(sd);
	pr_info("[ar330] %s\n", __func__);
	return ar330_try_mbus_fmt_internal(sensor, mf, NULL, NULL);
}

static int ar330_s_mbus_fmt(struct v4l2_subdev *sd,
		struct v4l2_mbus_framefmt *mf)
{
	struct ar330_data_fmt *ovfmt;
	struct ar330_win_size *wsize;
	struct i2c_client *client =  v4l2_get_subdevdata(sd);
	struct ar330_info *sensor = to_sensor(sd);
	int ret;

	pr_info("[ar330] %s\n", __func__);
	ret = ar330_try_mbus_fmt_internal(sensor, mf, &ovfmt, &wsize);
	if (ovfmt->regs)
		ret = ar330_write_array(client, ovfmt->regs);
	if (wsize->regs)
		ret = ar330_write_array(client, wsize->regs);
	return ret;


}

static int ar330_g_mbus_fmt(struct v4l2_subdev *sd,
		struct v4l2_mbus_framefmt *mf)
{
	pr_info("[ar330] %s\n", __func__);
	return 0;
}

static int ar330_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	pr_info("[ar330] %s\n", __func__);
	return 0;
}

static int ar330_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	pr_info("[ar330] %s\n", __func__);
	return 0;
}


static int ar330_enum_framesizes(struct v4l2_subdev *sd,
		struct v4l2_frmsizeenum *fsize)
{
	if (fsize->index >=  ARRAY_SIZE(ar330_win_sizes))
		return -1;
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = ar330_win_sizes[fsize->index].width;
	fsize->discrete.height = ar330_win_sizes[fsize->index].height;
	return 0;
}

static unsigned int ar330_numerator = 1;
static unsigned int ar330_denominator = 15;

static int ar330_g_frame_interval(struct v4l2_subdev *sd,
		struct v4l2_subdev_frame_interval *interval)
{

	pr_info("[ar330] %s\n",__func__);
	if(interval == NULL)
		return -1;

	interval->interval.numerator = 1;
	interval->interval.denominator = 15;

	return 0;                              
}

static int ar330_s_frame_interval(struct v4l2_subdev *sd,
		struct v4l2_subdev_frame_interval *interval)
{

	pr_info("[ar330] %s\n",__func__);

	if(interval == NULL)
		return -1;

	ar330_numerator = interval->interval.numerator;
	ar330_denominator = interval->interval.denominator;


	return 0;                              
}

static int ar330_enum_frameintervals(struct v4l2_subdev *sd,
		struct v4l2_frmivalenum *fival)
{

	pr_info("[ar330] %s\n",__func__);
	if(fival == NULL)
		return -1;

	if(fival->index != 0)
		return -1; 

	fival->type = V4L2_FRMSIZE_TYPE_DISCRETE; 
	fival->discrete.numerator = 1;
	fival->discrete.denominator = 15;

	return 0;                              
}

static int ar330_enable(struct v4l2_subdev *sd, int enable)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    printk("in ar330 enable = %d ***************\n", enable);
    if (enable) {
        ar330_write(client, 0x301a, 0x005C);
    }
    else {
        ar330_write(client, 0x301a, 0x0058);
    }
    return 0;
}

static const struct v4l2_subdev_video_ops ar330_video_ops = {
	.enum_mbus_fmt = ar330_enum_mbus_fmt,
	.try_mbus_fmt = ar330_try_mbus_fmt,
	.s_mbus_fmt = ar330_s_mbus_fmt,
	.g_mbus_fmt = ar330_g_mbus_fmt,
	.s_parm = ar330_s_parm,
	.g_parm = ar330_g_parm,
	.enum_framesizes = ar330_enum_framesizes,
	.enum_frameintervals = ar330_enum_frameintervals,
	.s_frame_interval = ar330_s_frame_interval,
	.g_frame_interval = ar330_g_frame_interval,
	.s_stream = ar330_enable,

};

static const struct v4l2_subdev_ops ar330_subdev_ops = {
	.core = &ar330_core_ops,
	.video = &ar330_video_ops,
};

static const struct i2c_device_id ar330_id[] = {
	{ "ar330mipi", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, ar330_id);

static int ar330_probe(struct i2c_client *client,
		    const struct i2c_device_id *id)
{
	struct v4l2_subdev *sd;
	struct ar330_info *sensor;
	int ret;

	pr_info("[ar330] %s\n", __func__);

	sensor = devm_kzalloc(&client->dev, sizeof(*sensor), GFP_KERNEL);
	if (!sensor)
		return -ENOMEM;

	mutex_init(&sensor->lock);
	sensor->client = client;

	sd = &sensor->sd;

	ret = v4l2_ctrl_handler_init(&sensor->ctrls, 4);
	if (ret < 0)
		return ret;
	v4l2_ctrl_new_std(&sensor->ctrls, &ar330_ctrl_ops,
			V4L2_CID_EXPOSURE_ABSOLUTE, -4, 4, 1, 0);

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &ar330_ctrl_ops,
			V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, 9, ~0x14e,
			V4L2_WHITE_BALANCE_AUTO);
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &ar330_ctrl_ops,
			V4L2_CID_SCENE_MODE,
			V4L2_SCENE_MODE_TEXT, ~0x1f01,
			V4L2_SCENE_MODE_NONE);

/*
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &ar330_ctrl_ops,
			V4L2_CID_COLORFX, V4L2_COLORFX_EMBOSS, ~0xf,
			V4L2_COLORFX_NONE);
*/
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &ar330_ctrl_ops,
			V4L2_CID_POWER_LINE_FREQUENCY,
			V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
			V4L2_CID_POWER_LINE_FREQUENCY_AUTO);
	sensor->sd.ctrl_handler = &sensor->ctrls;


	v4l2_i2c_subdev_init(sd, client, &ar330_subdev_ops);
	strlcpy(sd->name, "ar330mipi", sizeof(sd->name));

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE |
		V4L2_SUBDEV_FL_HAS_EVENTS;
	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &sensor->pad, 0);
	if (ret < 0)
		return ret;
	return 0;
}

static int ar330_remove(struct i2c_client *client)
{
	return 0;
}
static struct i2c_driver ar330_demo_driver = {
	.driver = {
		.owner  = THIS_MODULE,
		.name   = "ar330mipi",
	},
	.probe      = ar330_probe,
	.remove     = ar330_remove,
	.id_table   = ar330_id,
};

module_i2c_driver(ar330_demo_driver);

MODULE_AUTHOR("sam");
MODULE_DESCRIPTION("AR330 CMOS Image Sensor driver");
MODULE_LICENSE("GPL");


