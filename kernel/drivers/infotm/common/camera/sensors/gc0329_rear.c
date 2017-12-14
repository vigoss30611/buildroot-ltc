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
extern struct v4l2_ctrl_ops gc0329_ctrl_ops;
extern const struct v4l2_subdev_core_ops gc0329_core_ops; 
extern const struct v4l2_subdev_ops gc0329_subdev_ops;
extern const struct v4l2_subdev_video_ops gc0329_video_ops; 
static const struct i2c_device_id gc0329_id[] = {
	{"gc0329_rear", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, gc0329_id);

//initialize sensor driver
static int gc0329_probe(struct i2c_client *client, 
		const struct i2c_device_id *id)
{
	struct v4l2_subdev *sd;
	struct gc0329_info *sensor = NULL;
	int ret = 0;

	sensor = devm_kzalloc(&(client->dev), sizeof(*sensor), GFP_KERNEL);
	if(IS_ERR(sensor)) {
		pr_err("In gc0329 driver kalloc memeory failure\n");
		return -ENOMEM;
	}
	//initialize lock
	mutex_init(&(sensor->lock));
	sensor->client = client;
	sd = &(sensor->sd);
	ret = v4l2_ctrl_handler_init(&(sensor->ctrls), 4);
	if(0 > ret) {
		return ret;
	}
	//add new ctrl
	v4l2_ctrl_new_std(&(sensor->ctrls), &(gc0329_ctrl_ops),
			V4L2_CID_EXPOSURE_ABSOLUTE, -4, 4, 1, 0);

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0329_ctrl_ops,
			V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, 9, ~0x14e,
			V4L2_WHITE_BALANCE_AUTO);   

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0329_ctrl_ops,
			V4L2_CID_SCENE_MODE, V4L2_SCENE_MODE_TEXT, ~0x1f81,
			V4L2_SCENE_MODE_NONE);

	v4l2_ctrl_new_std_menu(&sensor->ctrls, &gc0329_ctrl_ops, 
			V4L2_CID_POWER_LINE_FREQUENCY, V4L2_CID_POWER_LINE_FREQUENCY_AUTO,
			~0xf, V4L2_CID_POWER_LINE_FREQUENCY_AUTO);

	sensor->sd.ctrl_handler = &sensor->ctrls;                    
	v4l2_i2c_subdev_init(sd, client, &gc0329_subdev_ops);
	strlcpy(sd->name, "gc0329_rear", sizeof(sd->name));    

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE |  
			V4L2_SUBDEV_FL_HAS_EVENTS;        
	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;

	ret = media_entity_init(&sd->entity, 1, &sensor->pad, 0);
	if (ret < 0) {                                             
		return ret;
	}	
										
	return 0;
}

static int gc0329_remove(struct i2c_client *client)
{
	return 0;
}
//i2c drvier module
static struct i2c_driver gc0329_demo_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name  = "gc0329_rear",
	},
	.probe	   = gc0329_probe,
	.remove    = gc0329_remove,
	.id_table  = gc0329_id,
};


module_i2c_driver(gc0329_demo_driver);

MODULE_AUTHOR("sam");
MODULE_DESCRIPTION("GC0329 CMOS image sensor driver");
MODULE_LICENSE("GPL");
