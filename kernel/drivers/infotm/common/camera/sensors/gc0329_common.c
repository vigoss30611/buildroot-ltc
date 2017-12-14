/***************************************************************************** 
** 
** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** camif gc0329 sensor v4l2 driver file
**
** Revision History: 
** ----------------- 
** v1.0.1	sam@2014/03/31: first version.
**
*****************************************************************************/
#include "gc0329.h"
#include "../camif/camif_core.h"

extern struct gc0329_reginfo gc0329_init_regs[];
extern struct gc0329_reginfo sensor_qcif[];
extern struct gc0329_reginfo sensor_qvga[];
extern struct gc0329_reginfo sensor_vga[];
extern struct gc0329_reginfo sensor_cif[];
extern struct gc0329_reginfo ev_neg4_regs [];
extern struct gc0329_reginfo ev_neg3_regs [];
extern struct gc0329_reginfo ev_neg2_regs []; 
extern struct gc0329_reginfo ev_neg1_regs [];
extern struct gc0329_reginfo ev_zero_regs [];
extern struct gc0329_reginfo ev_pos1_regs [];
extern struct gc0329_reginfo ev_pos2_regs [];
extern struct gc0329_reginfo ev_pos3_regs [];
extern struct gc0329_reginfo ev_pos4_regs [];
extern struct gc0329_reginfo wb_auto_regs [];
extern struct gc0329_reginfo wb_incandescent_regs [];
extern struct gc0329_reginfo wb_flourscent_regs [];
extern struct gc0329_reginfo wb_daylight_regs [];
extern struct gc0329_reginfo wb_cloudy_regs [];
extern struct gc0329_reginfo scence_auto_regs  [];
extern struct gc0329_reginfo scence_night60hz_regs []; 
extern struct gc0329_reginfo scence_sunset60hz_regs [];
extern struct gc0329_reginfo scence_portrait_regs [];
extern struct gc0329_reginfo scence_landscape_regs [];
extern struct gc0329_reginfo scence_party_indoor_regs [];
extern struct gc0329_reginfo scence_sports_regs [];
extern struct gc0329_reginfo colorfx_sepia_regs []; 
extern struct gc0329_reginfo colorfx_negative_regs [];
extern struct gc0329_reginfo colorfx_mono_regs []; 
extern struct gc0329_reginfo colorfx_none_regs [];
extern struct gc0329_reginfo antibanding_60hz_regs []; 
extern struct gc0329_reginfo antibanding_50hz_regs [];


static  struct gc0329_reginfo  sensor_out_yuv[] = {
	{0x00,0x00}
};

struct gc0329_win_size gc0329_win_sizes[] = { 
	/* QCIF */
	{                                                      
		.width	= QCIF_WIDTH,                                      
		.height = QCIF_HEIGHT,                                     
		.regs	= sensor_qcif,                              
	},	
	/* QVGA */
	{                                                      
		.width = QVGA_WIDTH,                                      
		.height = QVGA_HEIGHT,                                     
		.regs  = sensor_qvga,                              
	},
	/* CIF */
	{                                                      
		.width = CIF_WIDTH,                                      
		.height = CIF_HEIGHT,                                     
		.regs  = sensor_cif,                              
	},	
	/* VGA */                                             
	{                                                      
		.width = VGA_WIDTH,                                      
		.height = VGA_HEIGHT,                                     
		.regs  = sensor_vga,                              
	}
};                                                         


/* only one fixed colorspace per pixelcode */ 
struct gc0329_data_fmt {                       
	enum v4l2_mbus_pixelcode code;            
	enum v4l2_colorspace colorspace;          
	struct gc0329_reginfo	*regs;
};


struct gc0329_data_fmt gc0329_data_fmts[] = { 
	{
		.code = V4L2_MBUS_FMT_YUYV8_2X8, 
		.colorspace = V4L2_COLORSPACE_JPEG,
		.regs	= sensor_out_yuv, //FIXME 
	},
};                                                          

static struct gc0329_data_fmt *sensor_find_datafmt(             
		enum v4l2_mbus_pixelcode code, struct gc0329_data_fmt *fmt, 
		int n)                                                           
{                                                                    
	int i;                                                           
	for (i = 0; i < n; i++)
		if (fmt[i].code == code)
			return &fmt[i];

	return fmt;                                                     
}      

static struct gc0329_win_size *sensor_find_win(             
		struct v4l2_mbus_framefmt *mf, struct gc0329_win_size *win, 
		int n)                                                           
{                                                                    
	int i;
	for (i = 0; i < n; i++)
		if (win[i].width == mf->width && win[i].height == mf->height)
			return &win[i];                                          

	return win;                                                     
}                                                               

static struct gc0329_info * to_sensor(struct v4l2_subdev *sd)            
{                                                                           
	return container_of(sd, struct gc0329_info, sd);
}                                                                           

static int gc0329_write(struct i2c_client *client, uint8_t reg, uint8_t val)
{
	int err,cnt;
	u8 buf[2];
	struct i2c_msg msg[1];

	buf[0] = reg & 0xFF;
	buf[1] = val;

	msg->addr = client->addr;
	msg->flags = I2C_M_NOSTART;
	msg->buf = buf;
	msg->len = sizeof(buf);
	cnt = 3;
	err = -EAGAIN;
	err = i2c_transfer(client->adapter, msg, 1);
	if(1 != err) {
		pr_err("gc0329 write reg(0x%x, val:0x%x)failed,	ecode=%d \
					\n", reg, val, err);
		return err;
	}
	return 0;
}

static uint8_t gc0329_read(struct i2c_client *client, uint8_t reg) 
{
	int err ,cnt;
	struct i2c_msg msg[2];
	char buf= 0xff;

	msg[0].addr = client->addr;                
	msg[0].flags = I2C_M_NOSTART;              
	msg[0].buf = &reg;                          
	msg[0].len = 1;                  

	msg[1].addr = client->addr;               
	msg[1].flags = I2C_M_RD;    
	msg[1].buf = &buf; 
	msg[1].len = 1;                           

	cnt = 1;
	err = -EAGAIN;
	err = i2c_transfer(client->adapter, msg, 2); 
	if(err != 2)
		pr_err("gc0329 read reg(0x%x)failed!\n", reg);

	return buf;
}

static int gc0329_write_array(struct i2c_client *client,
								struct gc0329_reginfo *regarray)
{
	int i = 0, err;
	
	while((regarray[i].reg != 0) || (regarray[i].val != 0))                     
	{                                                                              
		err = gc0329_write(client, regarray[i].reg, regarray[i].val);              
		if (err != 0)                                                              
		{                                                                       
			printk(KERN_ERR "gc0329 write failed current i = %d\n",i);
			return err;                                                            
		}                                                                          
		i++;                                                                       
	}                                                                              
	return 0;
}


#if 0
static const struct v4l2_subdev_pad_ops gc0329_pad_ops = { 
	.enum_mbus_code = gc0329_enum_mbus_code,               
	.enum_frame_size = gc0329_enum_frame_sizes,            
	.get_fmt = gc0329_get_fmt,                             
	.set_fmt = gc0329_set_fmt,                             
};                                                         
#endif

static int gc0329_g_chip_ident(struct v4l2_subdev *sd,                    
		struct v4l2_dbg_chip_ident *chip)                                 
{                                                                         
	struct i2c_client *client = v4l2_get_subdevdata(sd);                  
	return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_GC0329, 0);
}                                                                         


static int gc0329_reset(struct v4l2_subdev *sd, u32 val)
{                                                       
	return 0;                                           
}                                                       

static int is_sensor_gc0329(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	u8 sen ;
	//need to enable data clock and analog pwd	
	gc0329_write(client, 0xfc, 0x16);
	sen = gc0329_read(client, 0x00);
	pr_info("[gc0329] Sensor ID:%x,req:0xc0\n",sen);
	return (sen == 0xc0) ? 1 : 0;
}

static int gc0329_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	/*
	 * val - 1: Just read sensor ID
	 *     - 0: init sensor
	 */
	if(val) 
		return !is_sensor_gc0329(sd);
	else
		return gc0329_write_array(client, gc0329_init_regs);
}	

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int gc0329_g_register(struct v4l2_subdev *sd,
							struct v4l2_dbg_register *reg)
{
	pr_info("[gc0329] %s\n", __func__);
	return 0;
}

static int gc0329_s_register(struct v4l2_subdev *sd, 
							const struct v4l2_dbg_register *reg)
{
	pr_info("[gc0329] %s\n", __func__);
	return 0;
}

#endif


static int gc0329_set_exposure(struct i2c_client *client,
								struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case -4:
			ret = gc0329_write_array(client, ev_neg4_regs);
			break;
		case -3:
			ret = gc0329_write_array(client, ev_neg3_regs);
			break;
		case -2:
			ret = gc0329_write_array(client, ev_neg2_regs);
			break;
		case -1:
			ret = gc0329_write_array(client, ev_neg1_regs);
			break;
		case 0:
			ret = gc0329_write_array(client, ev_zero_regs);
			break;
		case 1:
			ret = gc0329_write_array(client, ev_pos1_regs);
			break;
		case 2:
			ret = gc0329_write_array(client, ev_pos2_regs);
			break;
		case 3:
			ret = gc0329_write_array(client, ev_pos3_regs);
			break;
		case 4:
			ret = gc0329_write_array(client, ev_pos4_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0329_set_wb(struct i2c_client *client,  struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_WHITE_BALANCE_AUTO: 
			ret = gc0329_write_array(client, wb_auto_regs);
			break;
		case V4L2_WHITE_BALANCE_INCANDESCENT:
			ret = gc0329_write_array(client, wb_incandescent_regs);
			break;
		case V4L2_WHITE_BALANCE_FLUORESCENT:
			ret = gc0329_write_array(client, wb_flourscent_regs);
			break;
		case V4L2_WHITE_BALANCE_DAYLIGHT:
			ret = gc0329_write_array(client, wb_daylight_regs);
			break;
		case V4L2_WHITE_BALANCE_CLOUDY:
			ret = gc0329_write_array(client, wb_cloudy_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0329_set_scence(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_SCENE_MODE_NONE:
			ret = gc0329_write_array(client, scence_auto_regs); 
			break;
		case V4L2_SCENE_MODE_NIGHT:
			ret = gc0329_write_array(client, scence_night60hz_regs);
			break;
		case V4L2_SCENE_MODE_SUNSET:
			ret = gc0329_write_array(client, scence_sunset60hz_regs);
			break;
		case V4L2_SCENE_MODE_PORTRAIT:
			ret = gc0329_write_array(client, scence_portrait_regs);
			break;
		case V4L2_SCENE_MODE_LANDSCAPE:
			ret = gc0329_write_array(client, scence_landscape_regs);
			break;
		case V4L2_SCENE_MODE_PARTY_INDOOR:
			ret = gc0329_write_array(client, scence_party_indoor_regs);
			break;
		case V4L2_SCENE_MODE_SPORTS:
			ret = gc0329_write_array(client, scence_sports_regs);
			break;
		default:
			break;
	}
	return ret;

}

static int gc0329_set_colorfx(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_COLORFX_NONE:
			ret = gc0329_write_array(client, colorfx_none_regs);
			break;
		case V4L2_COLORFX_BW:
			ret = gc0329_write_array(client, colorfx_mono_regs);
			break;
		case V4L2_COLORFX_SEPIA:
			ret = gc0329_write_array(client, colorfx_sepia_regs);
			break;
		case V4L2_COLORFX_NEGATIVE:
			ret = gc0329_write_array(client, colorfx_negative_regs);
			break;
		default:
			break;
	}
	return ret;
}

static int gc0329_set_powlinefreq(struct i2c_client *client, struct v4l2_ctrl *ctrl)
{
	int ret = -1;
	switch(ctrl->val) {
		case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED:
		case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
		case V4L2_CID_POWER_LINE_FREQUENCY_AUTO:
			ret = gc0329_write_array(client, antibanding_60hz_regs);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
			ret = gc0329_write_array(client, antibanding_50hz_regs);
			break;
		default:
			break;
	}
	return -1;
}

int gc0329_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = &container_of(ctrl->handler,
								struct gc0329_info, ctrls)->sd;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = -1;
	switch (ctrl->id) {
		case V4L2_CID_EXPOSURE_ABSOLUTE:
			ret = gc0329_set_exposure(client, ctrl);
			break;	
		case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
			ret = gc0329_set_wb(client, ctrl);
			break;
		case V4L2_CID_SCENE_MODE:
			ret = gc0329_set_scence(client, ctrl);
			break;
		case V4L2_CID_COLORFX:
			ret = gc0329_set_colorfx(client, ctrl);
			break;
		case V4L2_CID_POWER_LINE_FREQUENCY:
			ret = gc0329_set_powlinefreq(client, ctrl);
			break;
		default:
		   break;
	}
	return ret; 
}

struct v4l2_ctrl_ops gc0329_ctrl_ops = {
	.s_ctrl = gc0329_s_ctrl,
};

const struct v4l2_subdev_core_ops gc0329_core_ops = { 
	.g_chip_ident = gc0329_g_chip_ident,                     
	.reset = gc0329_reset,                                   
	.init = gc0329_init,
#ifdef CONFIG_VIDEO_ADV_DEBUG                                
	.g_register = gc0329_g_register,                         
	.s_register = gc0329_s_register,                         
#endif                                                       
};                                           

static int gc0329_enum_mbus_fmt(struct v4l2_subdev *sd, unsigned int index,
		enum v4l2_mbus_pixelcode *code)
{
		pr_info("[gc0329] %s\n",__func__);
		if (index >= ARRAY_SIZE(gc0329_data_fmts))
			return -EINVAL;
		*code = gc0329_data_fmts[index].code;     
			return 0;
}


static int gc0329_try_mbus_fmt_internal(struct gc0329_info *sensor,
		struct v4l2_mbus_framefmt *mf,
		struct gc0329_data_fmt **ret_fmt,
		struct gc0329_win_size  **ret_size)
{
		int ret = 0;                             
		if(ret_fmt != NULL)
			*ret_fmt = sensor_find_datafmt(mf->code, gc0329_data_fmts,
	                   ARRAY_SIZE(gc0329_data_fmts));        
		if(ret_size != NULL)
			*ret_size = sensor_find_win(mf, gc0329_win_sizes,
	                   ARRAY_SIZE(gc0329_win_sizes));        

		return ret;

}

static int gc0329_try_mbus_fmt(struct v4l2_subdev *sd, 
			struct v4l2_mbus_framefmt *mf)
{
		struct gc0329_info *sensor = to_sensor(sd);
		pr_info("[gc0329] %s\n",__func__);
		return gc0329_try_mbus_fmt_internal(sensor, mf, NULL, NULL);
}

static int gc0329_s_mbus_fmt(struct v4l2_subdev *sd, 
		struct v4l2_mbus_framefmt *mf)
{
		struct gc0329_data_fmt *ovfmt;
		struct gc0329_win_size *wsize;
		struct i2c_client *client =  v4l2_get_subdevdata(sd);
		struct gc0329_info *sensor = to_sensor(sd);
		int ret;

		pr_info("[gc0329] %s\n",__func__);
		ret = gc0329_try_mbus_fmt_internal(sensor, mf, &ovfmt, &wsize);
		if(ovfmt->regs)
			gc0329_write_array(client, ovfmt->regs);
		if(wsize->regs)
			gc0329_write_array(client, wsize->regs);
		return ret;
}

static int gc0329_g_mbus_fmt(struct v4l2_subdev *sd,
							struct v4l2_mbus_framefmt *mf)
{
		pr_info("[gc0329] %s\n",__func__);
		return 0;
}

static int gc0329_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{                                                                              
		pr_info("[gc0329] %s\n",__func__);
		return 0;                              
}

static int gc0329_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{                                                                              
		pr_info("[gc0329] %s\n",__func__);
		return 0;                              
}

static int gc0329_enum_framesizes(struct v4l2_subdev *sd,
				        struct v4l2_frmsizeenum *fsize)
{
		if(fsize->index >=  ARRAY_SIZE(gc0329_win_sizes))
	        return -1;
		fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
		fsize->discrete.width = gc0329_win_sizes[fsize->index].width;
		fsize->discrete.height = gc0329_win_sizes[fsize->index].height;
		return 0;
}


const struct v4l2_subdev_video_ops gc0329_video_ops = {
	.enum_mbus_fmt = gc0329_enum_mbus_fmt,
	.try_mbus_fmt = gc0329_try_mbus_fmt,
	.s_mbus_fmt = gc0329_s_mbus_fmt,
	.g_mbus_fmt = gc0329_g_mbus_fmt,
	.s_parm = gc0329_s_parm,
	.g_parm = gc0329_g_parm,
	.enum_framesizes = gc0329_enum_framesizes,

};

const struct v4l2_subdev_ops gc0329_subdev_ops = {
	.core  = &gc0329_core_ops,
	.video = &gc0329_video_ops,
};


