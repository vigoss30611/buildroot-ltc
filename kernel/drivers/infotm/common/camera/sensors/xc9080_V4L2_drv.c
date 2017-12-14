#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <mach/items.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-mediabus.h>
#include <media/media-entity.h>
#include "xc9080_common.h"
#include "../sensors/power_up.h"
struct xc9080_win_size {
	int width;
	int height;
	struct reginfo *regs;
};

struct xc9080_info {
	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrls;
	struct i2c_client *client;
	enum v4l2_mbus_type bus_type;
	struct mutex		lock;

};

#define XC9080_CONFIG	XC9080_2160_1080_YUV//XC9080_2160_1080_YUV_CROP//XC9080_2160_1080_YUV

static struct reginfo *xc9080_config;
static struct reginfo *hm2131_config;
static int xc9080_config_array_size;
static int hm2131_config_array_size;

struct xc9080_win_size xc9080_win_sizes[] = {
    /* 2160x1080 */
	{
#if (XC9080_CONFIG == XC9080_1920_1080_RAW10) || (XC9080_CONFIG == XC9080_1920_1080_YUV)
			.width = 1920,
#elif (XC9080_CONFIG == XC9080_2160_1080_RAW10) || (XC9080_CONFIG == XC9080_2160_1080_YUV)
			.width = 2160,
#elif (XC9080_CONFIG == XC9080_3840_1920_RAW10)
			.width = 3840,
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV_CROP || XC9080_CONFIG == XC9080_2160_1080_YUV_SCALE)
			.width = 1280,
#endif

#if (XC9080_CONFIG == XC9080_1920_1080_RAW10) || (XC9080_CONFIG == XC9080_1920_1080_YUV) \
	|| (XC9080_CONFIG == XC9080_2160_1080_RAW10) || (XC9080_CONFIG == XC9080_2160_1080_YUV)
			.height = 1080,
#elif (XC9080_CONFIG == XC9080_3840_1920_RAW10)
			.height = 1920,
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV_CROP || XC9080_CONFIG == XC9080_2160_1080_YUV_SCALE)
			.height = 640,
#endif
		.regs  = sensor_2160_1080,
	},
};

/* only one fixed colorspace per pixelcode */
struct xc9080_data_fmt {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	struct reginfo	*regs;
};

struct xc9080_data_fmt xc9080_data_fmts[] = {
	{
		.code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.regs	= sensor_out_yuv, //FIXME
	},
};

static struct xc9080_data_fmt *sensor_find_datafmt(
		enum v4l2_mbus_pixelcode code,
		struct xc9080_data_fmt *fmt,
		int n)
{
	int i;
	pr_db("[xc9080] %s\n",__func__);
	for (i = 0; i < n; i++)
		if (fmt[i].code == code)
			return &fmt[i];

	return fmt;
}

static struct xc9080_win_size *sensor_find_win(
		struct v4l2_mbus_framefmt *mf,
		struct xc9080_win_size *win,
		int n)
{
	int i;
	pr_db("[xc9080] %s\n",__func__);
	for (i = 0; i < n; i++)
		if (win[i].width == mf->width && win[i].height == mf->height)
			return &win[i];

	return win;
}

static struct xc9080_info * to_sensor(struct v4l2_subdev *sd)
{
	pr_db("[xc9080] %s\n",__func__);
	return container_of(sd, struct xc9080_info, sd);
}

static int xc9080_g_chip_ident(struct v4l2_subdev *sd,
		struct v4l2_dbg_chip_ident *chip)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	pr_db("[xc9080] %s\n",__func__);
	return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_XC9080, 0);
}


static int xc9080_reset(struct v4l2_subdev *sd, u32 val)
{
	pr_db("[xc9080] %s\n",__func__);
	return 0;
}

static int xc9080_register_init(struct i2c_client *client)
{
	int ret = 0;

	ret = xc9080_get_id(client);
	if(ret == 0) {
		xc9080_write_array2(client, xc9080_config, xc9080_config_array_size);

		xc9080_bypass_change(client, XC9080_BYPASS_ON);

		ret = hm2131_get_id(client);
		if(ret == 0)
		{
			hm2131_write_array2(client, hm2131_config, hm2131_config_array_size);

		}

		xc9080_bypass_change(client, XC9080_BYPASS_OFF);
	}

	return ret;
}

static int xc9080_init(struct v4l2_subdev *sd, u32 val)
{
	int ret = 0;
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	pr_db("[xc9080] %s val=%d\n",__func__, val);
	/*
	 * val - 1: Just read sensor ID
	 *     - 0: init sensor
	 */
	if(val) {
		ret = xc9080_get_id(client);
		if(ret == 0) {
		    xc9080_write_array2(client, xc9080_config, xc9080_config_array_size);
			xc9080_bypass_change(client, XC9080_BYPASS_ON);
			ret = hm2131_get_id(client);
			xc9080_bypass_change(client, XC9080_BYPASS_OFF);
		}
	} else {
		ret = xc9080_register_init(client);
	}
	return ret;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int xc9080_g_register(struct v4l2_subdev *sd,
		struct v4l2_dbg_register *reg)
{
	pr_db("[xc9080] %s\n", __func__);
	return 0;
}

static int xc9080_s_register(struct v4l2_subdev *sd,
		const struct v4l2_dbg_register *reg)
{
	pr_db("[xc9080] %s\n", __func__);
	return 0;
}

#endif


static int xc9080_set_exposure(struct i2c_client *client,
								struct v4l2_ctrl *ctrl)
{
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch(ctrl->val) {
		case -4:
			ret = xc9080_write_array(client, ev_neg4_regs);
			break;
		case -3:
			ret = xc9080_write_array(client, ev_neg3_regs);
			break;
		case -2:
			ret = xc9080_write_array(client, ev_neg2_regs);
			break;
		case -1:
			ret = xc9080_write_array(client, ev_neg1_regs);
			break;
		case 0:
			ret = xc9080_write_array(client, ev_zero_regs);
			break;
		case 1:
			ret = xc9080_write_array(client, ev_pos1_regs);
			break;
		case 2:
			ret = xc9080_write_array(client, ev_pos2_regs);
			break;
		case 3:
			ret = xc9080_write_array(client, ev_pos3_regs);
			break;
		case 4:
			ret = xc9080_write_array(client, ev_pos4_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int xc9080_set_wb(struct i2c_client *client,  struct v4l2_ctrl *ctrl)
{
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch(ctrl->val) {
		case V4L2_WHITE_BALANCE_AUTO:
			ret = xc9080_write_array(client, wb_auto_regs);
			break;
		case V4L2_WHITE_BALANCE_INCANDESCENT:
			ret = xc9080_write_array(client, wb_incandescent_regs);
			break;
		case V4L2_WHITE_BALANCE_FLUORESCENT:
			ret = xc9080_write_array(client, wb_flourscent_regs);
			break;
		case V4L2_WHITE_BALANCE_DAYLIGHT:
			ret = xc9080_write_array(client, wb_daylight_regs);
			break;
		case V4L2_WHITE_BALANCE_CLOUDY:
			ret = xc9080_write_array(client, wb_cloudy_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int xc9080_set_scence(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch(ctrl->val) {
		case V4L2_SCENE_MODE_NONE:
			ret = xc9080_write_array(client, scence_auto_regs);
			break;
		case V4L2_SCENE_MODE_NIGHT:
			ret = xc9080_write_array(client, scence_night60hz_regs);
			break;
		case V4L2_SCENE_MODE_SUNSET:
			ret = xc9080_write_array(client, scence_sunset60hz_regs);
			break;
		case V4L2_SCENE_MODE_PARTY_INDOOR:
			ret = xc9080_write_array(client, scence_party_indoor_regs);
			break;
		case V4L2_SCENE_MODE_SPORTS:
			ret = xc9080_write_array(client, scence_sports_regs);
			break;
		default:
			break;
	}
	return ret;

}

static int xc9080_set_colorfx(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch(ctrl->val) {
		case V4L2_COLORFX_NONE:
			ret = xc9080_write_array(client, colorfx_none_regs);
			break;
		case V4L2_COLORFX_BW:
			ret = xc9080_write_array(client, colorfx_mono_regs);
			break;
		case V4L2_COLORFX_SEPIA:
			ret = xc9080_write_array(client, colorfx_sepia_regs);
			break;
		case V4L2_COLORFX_NEGATIVE:
			ret = xc9080_write_array(client, colorfx_negative_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int xc9080_set_powlinefreq(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch(ctrl->val) {
		case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED:
		case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
		case V4L2_CID_POWER_LINE_FREQUENCY_AUTO:
			ret = xc9080_write_array(client, antibanding_60hz_regs);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
			ret = xc9080_write_array(client, antibanding_50hz_regs);
			break;
		default:
			break;
	}
	return -1;
}
static int xc9080_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = &container_of(ctrl->handler, struct xc9080_info, ctrls)->sd;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = -1;

	pr_db("[xc9080] %s\n",__func__);
	switch (ctrl->id) {
		case V4L2_CID_EXPOSURE_ABSOLUTE:
			ret = xc9080_set_exposure(client, ctrl);
			break;
		case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
			ret = xc9080_set_wb(client, ctrl);
			break;
		case V4L2_CID_SCENE_MODE:
			ret = xc9080_set_scence(client, ctrl);
			break;
		case V4L2_CID_COLORFX:
			ret = xc9080_set_colorfx(client, ctrl);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY:
			ret = xc9080_set_powlinefreq(client, ctrl);
			break;
		default:
		   break;
	}
	return ret;
}

static struct v4l2_ctrl_ops xc9080_ctrl_ops = {
	.s_ctrl = xc9080_s_ctrl,
};

static const struct v4l2_subdev_core_ops xc9080_core_ops = {
	.g_chip_ident = xc9080_g_chip_ident,
	.reset = xc9080_reset,
	.init = xc9080_init,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = xc9080_g_register,
	.s_register = xc9080_s_register,
#endif
};



static int xc9080_enum_mbus_fmt(struct v4l2_subdev *sd, unsigned int index,
							enum v4l2_mbus_pixelcode *code)
{
	pr_db("[xc9080] %s\n",__func__);
	if (index >= ARRAY_SIZE(xc9080_data_fmts))
		return -EINVAL;
	*code = xc9080_data_fmts[index].code;
	return 0;
}


static int xc9080_try_mbus_fmt_internal(struct xc9080_info *sensor,
		struct v4l2_mbus_framefmt *mf,
		struct xc9080_data_fmt **ret_fmt,
		struct xc9080_win_size  **ret_size)
{
	int ret = 0;
	pr_db("[xc9080] %s\n",__func__);
	if(ret_fmt != NULL)
		*ret_fmt = sensor_find_datafmt(mf->code, xc9080_data_fmts,
				ARRAY_SIZE(xc9080_data_fmts));
	if(ret_size != NULL)
		*ret_size = sensor_find_win(mf, xc9080_win_sizes,
				ARRAY_SIZE(xc9080_win_sizes));

	return ret;
}

static int xc9080_try_mbus_fmt(struct v4l2_subdev *sd,
									struct v4l2_mbus_framefmt *mf)
{

	struct xc9080_info *sensor = to_sensor(sd);
	pr_db("[xc9080] %s\n",__func__);
	return xc9080_try_mbus_fmt_internal(sensor, mf, NULL, NULL);
}

static int xc9080_s_mbus_fmt(struct v4l2_subdev *sd,
								struct v4l2_mbus_framefmt *mf)
{
	struct xc9080_data_fmt *ovfmt;
	struct xc9080_win_size *wsize;
	struct i2c_client *client =  v4l2_get_subdevdata(sd);
	struct xc9080_info *sensor = to_sensor(sd);
	int ret;

	pr_db("[xc9080] %s\n",__func__);
	ret = xc9080_try_mbus_fmt_internal(sensor, mf, &ovfmt, &wsize);
	if(ovfmt->regs)
		ret = xc9080_write_array(client, ovfmt->regs);
	if(wsize->regs)
		ret = xc9080_write_array(client, wsize->regs);
	return ret;
}

static int xc9080_g_mbus_fmt(struct v4l2_subdev *sd,
		struct v4l2_mbus_framefmt *mf)
{
	pr_db("[xc9080] %s\n",__func__);
	return 0;
}

static int xc9080_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	pr_db("[xc9080] %s\n",__func__);
	return 0;
}

static int xc9080_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	pr_db("[xc9080] %s\n",__func__);
	return 0;
}


static int xc9080_enum_framesizes(struct v4l2_subdev *sd,
		        struct v4l2_frmsizeenum *fsize)
{
	//pr_db("[xc9080] %s index=%d\n", __func__, fsize->index);
	if(fsize->index >=  ARRAY_SIZE(xc9080_win_sizes))
		return -1;
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = xc9080_win_sizes[fsize->index].width;
	fsize->discrete.height = xc9080_win_sizes[fsize->index].height;

	return 0;
}

static unsigned int xc9080_numerator = 1;
static unsigned int xc9080_denominator = 15;

static int xc9080_g_frame_interval(struct v4l2_subdev *sd,
        struct v4l2_subdev_frame_interval *interval)
{

    pr_db("[xc9080] %s\n",__func__);
    if(interval == NULL)
        return -1;

    interval->interval.numerator = 1;
    interval->interval.denominator = 15;

    return 0;
}

static	int xc9080_s_frame_interval(struct v4l2_subdev *sd,
        struct v4l2_subdev_frame_interval *interval)
{

    pr_db("[xc9080] %s\n",__func__);

    if(interval == NULL)
        return -1;

    xc9080_numerator = interval->interval.numerator;
    xc9080_denominator = interval->interval.denominator;


    return 0;
}

static	int xc9080_enum_frameintervals(struct v4l2_subdev *sd,
        struct v4l2_frmivalenum *fival)
{

    pr_db("[xc9080] %s\n",__func__);
    if(fival == NULL)
        return -1;

    if(fival->index != 0)
       return -1;

    fival->type = V4L2_FRMSIZE_TYPE_DISCRETE;
    fival->discrete.numerator = 1;
    fival->discrete.denominator = 15;

	return 0;
}

static int xc9080_enable(struct v4l2_subdev *sd, int enable)
{
	int ret = 0;
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    pr_db("[xc9080] %s enable = %d\n", __func__, enable);
    if (enable) {
    	//ret = xc9080_register_init(client);
		//xc9080_write_array(client, xc9080_mipi_enable);
        xc9080_bypass_change(client, XC9080_BYPASS_ON);
    	hm2131_write(client, 0x0100, 0x03);
    	xc9080_bypass_change(client, XC9080_BYPASS_OFF);
    }
    else {
        xc9080_bypass_change(client, XC9080_BYPASS_ON);
        hm2131_write(client, 0x0100, 0x03);
        xc9080_bypass_change(client, XC9080_BYPASS_OFF);
		xc9080_write_array(client, xc9080_mipi_disable);

    }
    return ret;
}

static const struct v4l2_subdev_video_ops xc9080_video_ops = {
    .enum_mbus_fmt = xc9080_enum_mbus_fmt,
    .try_mbus_fmt = xc9080_try_mbus_fmt,
    .s_mbus_fmt = xc9080_s_mbus_fmt,
    .g_mbus_fmt = xc9080_g_mbus_fmt,
    .s_parm = xc9080_s_parm,
    .g_parm = xc9080_g_parm,
    .enum_framesizes = xc9080_enum_framesizes,
    .enum_frameintervals = xc9080_enum_frameintervals,
    .s_frame_interval = xc9080_s_frame_interval,
    .g_frame_interval = xc9080_g_frame_interval,
    .s_stream = xc9080_enable,
};

static const struct v4l2_subdev_ops xc9080_subdev_ops = {
	.core = &xc9080_core_ops,
	.video = &xc9080_video_ops,
};

static const struct i2c_device_id xc9080_id[] = {
	{ "xc9080", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, xc9080_id);

static int xc9080_probe(struct i2c_client *client,
		            const struct i2c_device_id *id)
{
	struct v4l2_subdev *sd;
	struct xc9080_info *sensor;
	int ret;

	pr_db("[xc9080] %s\n", __func__);

#if (XC9080_CONFIG == XC9080_1920_1080_RAW10)
	xc9080_config = xc9080_1920_1080_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_1920_1080_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_RAW10)
	xc9080_config = xc9080_2160_1080_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_2160_1080_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_3840_1920_RAW10)
	xc9080_config = xc9080_3840_1920_raw10;
	hm2131_config = hm2131_raw10;
	xc9080_config_array_size = xc9080_3840_1920_raw10_array_size;
	hm2131_config_array_size = hm2131_raw10_array_size;
#elif (XC9080_CONFIG == XC9080_1920_1080_YUV)
	xc9080_config = xc9080_1920_1080_yuv;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_1920_1080_yuv_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV)
	xc9080_config = xc9080_2160_1080_yuv;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_2160_1080_yuv_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV_CROP)
	xc9080_config = xc9080_2160_1080_yuv_crop;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_2160_1080_yuv_crop_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#elif (XC9080_CONFIG == XC9080_2160_1080_YUV_SCALE)
	xc9080_config = xc9080_2160_1080_yuv_scale;
	hm2131_config = hm2131_yuv;
	xc9080_config_array_size = xc9080_2160_1080_yuv_scale_array_size;
	hm2131_config_array_size = hm2131_yuv_array_size;
#endif

	sensor = devm_kzalloc(&client->dev, sizeof(*sensor), GFP_KERNEL);
	if (!sensor)
		return -ENOMEM;

	mutex_init(&sensor->lock);
	sensor->client = client;

	sd = &sensor->sd;

	ret = v4l2_ctrl_handler_init(&sensor->ctrls, 4);
	if (ret < 0)
		return ret;
	v4l2_ctrl_new_std(&sensor->ctrls, &xc9080_ctrl_ops,
			V4L2_CID_EXPOSURE_ABSOLUTE, -4, 4, 1, 0);

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &xc9080_ctrl_ops,
			V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, 9, ~0x14e,
			V4L2_WHITE_BALANCE_AUTO);
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &xc9080_ctrl_ops,
			V4L2_CID_SCENE_MODE, V4L2_SCENE_MODE_TEXT, ~0x1f01,
			V4L2_SCENE_MODE_NONE);

/*
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &xc9080_ctrl_ops,
			V4L2_CID_COLORFX, V4L2_COLORFX_EMBOSS, ~0xf,
			V4L2_COLORFX_NONE);
*/
	v4l2_ctrl_new_std_menu(&sensor->ctrls, &xc9080_ctrl_ops,
			V4L2_CID_POWER_LINE_FREQUENCY,
			V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
			V4L2_CID_POWER_LINE_FREQUENCY_AUTO);

	v4l2_i2c_subdev_init(sd, client, &xc9080_subdev_ops);
	strlcpy(sd->name, "xc9080", sizeof(sd->name));

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE |
		V4L2_SUBDEV_FL_HAS_EVENTS;
	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &sensor->pad, 0);
	if (ret < 0)
		return ret;

	return 0;
}

static int xc9080_remove(struct i2c_client *client)
{
	return 0;
}
static struct i2c_driver xc9080_driver = {
	.driver = {
		.owner  = THIS_MODULE,
		.name   = "xc9080",
	},
	.probe      = xc9080_probe,
	.remove     = xc9080_remove,
	.id_table   = xc9080_id,
};

module_i2c_driver(xc9080_driver);
MODULE_DESCRIPTION("driver for xc9080.");
MODULE_LICENSE("Dual BSD/GPL");
