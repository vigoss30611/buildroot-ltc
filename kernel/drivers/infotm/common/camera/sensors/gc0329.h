/* **************************************************************************** 
 * ** 
 * ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** camif gc0329 sensor initialize file
 * **
 * ** Revision History: 
 * ** ----------------- 
 * ** v1.0.1	sam@2014/03/31: first version.
 * **
 * ***************************************************************************/
#ifndef __GC0329_H
#define __GC0329_H

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

struct gc0329_reginfo {
	unsigned char	reg;
	unsigned char val;
};

struct gc0329_win_size {
	int width;
	int height;
	struct gc0329_reginfo *regs;
};

struct gc0329_info {
	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrls;
	struct i2c_client *client;
	enum v4l2_mbus_type bus_type;
	struct mutex	lock;
};
#endif

