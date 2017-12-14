/*
 *  mma8451.c - Linux kernel modules for 3-Axis Orientation/Motion
 *  Detection Sensor
 *
 *  Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/input-polldev.h>
#include <mach/items.h>
#include <linux/regulator/consumer.h>

#define MMA7660_I2C_ADDR	0x4c

#define POLL_INTERVAL_MIN	1
#define POLL_INTERVAL_MAX	500
#define POLL_INTERVAL		100 /* msecs */
// if sensor is standby ,set POLL_STOP_TIME to slow down the poll
#define POLL_STOP_TIME		10000  

#define INPUT_FUZZ		32
#define INPUT_FLAT		32
#define MODE_CHANGE_DELAY_MS	100

#define MMA7660_STATUS_ZYXDR	0x08
#define MMA7660_BUF_SIZE	3
//#define CONFIG_SENSORS_MMA_POSITION 4
/*MMA7660 register definition*/

#define MMA7660_XOUT			0x00
#define MMA7660_YOUT			0x01
#define MMA7660_ZOUT			0x02
#define MMA7660_TILT			0x03
#define MMA7660_SRST			0x04
#define MMA7660_SPCNT			0x05
#define MMA7660_INTSU			0x06
#define MMA7660_MODE			0x07
#define MMA7660_SR				0x08
#define MMA7660_PDET			0x09
#define MMA7660_PD				0x0A
#define MMA7660_MAX_DELAY               200
enum {
	MMA_STANDBY = 0,
	MMA_ACTIVED,
};
struct mma7660_data_axis{
	short x;
	short y;
	short z;
};

#define SENSOR_DATA_SIZE 3
#define AVG_NUM 5
#define max_num 770
#define min_num -770
static s16 avg[SENSOR_DATA_SIZE];
static s16 standby[SENSOR_DATA_SIZE];

struct i2c_client *this_client;

struct mma7660_data{
	atomic_t enable;
	atomic_t delay;
	struct i2c_client *mma7660_client;
//	struct input_polled_dev *poll_dev;
	struct input_dev *input;
	struct mutex enable_mutex;
	struct mutex data_lock;
//	int active;
//	int position;
	struct delayed_work work;
};

static struct regulator *regulator;

int mma7660_i2c_test(void)
{    
    u8 ret;
    struct i2c_client *client = this_client;
    printk("Enter %s\n", __func__);
    ret = i2c_smbus_read_byte_data(client, MMA7660_MODE);
    if (ret < 0)
        return -1;

    return 0;
}
EXPORT_SYMBOL(mma7660_i2c_test);

static int mma7660_position_setting[8][3][3] =
{
   {{-1,  0,  0}, { 0, -1,	0}, {0, 0,	1}},
   {{ 0,  1,  0}, {-1,  0,	0}, {0, 0,	1}},
   {{ 1,  0,  0}, { 0,  1,	0}, {0, 0,	1}},
   {{ 0, -1,  0}, { 1,  0,	0}, {0, 0,	1}},

   {{-1,  0,  0}, { 0,  1,	0}, {0, 0,  -1}},
   {{ 0,  1,  0}, { 1,  0,	0}, {0, 0,  -1}},
   {{ 1,  0,  0}, { 0, -1,	0}, {0, 0,  -1}},
   {{ 0, -1,  0}, {-1,  0,	0}, {0, 0,  -1}},
  
};

static int mma7660_data_convert(struct mma7660_data* pdata,struct mma7660_data_axis *axis_data)
{
   short rawdata[3],data[3];
   int i,j;
//   int position = pdata->position ;
//   if(position < 0 || position > 7 )
   int position = 4;
   rawdata [0] = axis_data->x ; rawdata [1] = axis_data->y ; rawdata [2] = axis_data->z ;  
   for(i = 0; i < 3 ; i++)
   {
   	data[i] = 0;
   	for(j = 0; j < 3; j++)
		data[i] += rawdata[j] * mma7660_position_setting[position][i][j];
   }
   axis_data->x = data[0];
   axis_data->y = data[1];
   axis_data->z = data[2];
   return 0;
}
static int mma7660_device_detect(struct i2c_client *client)
{
	u8 val = 0xaa , old_val;
	u8 ret;
	ret = i2c_smbus_read_byte_data(client, MMA7660_MODE);
	i2c_smbus_write_byte_data(client, MMA7660_MODE, ret & ~0x01);
	old_val = i2c_smbus_read_byte_data(client, MMA7660_SR);
	i2c_smbus_write_byte_data(client, MMA7660_SR, val);
	ret = i2c_smbus_read_byte_data(client, MMA7660_SR);
	if(ret == val){
		i2c_smbus_write_byte_data(client, MMA7660_SR,old_val);
		return 0;
	}
	return -1;
}
static int mma7660_device_init(struct i2c_client *client)
{
	int result;
//	struct mma7660_data *pdata = i2c_get_clientdata(client);
	result = i2c_smbus_write_byte_data(client, MMA7660_MODE, 0);
	if (result < 0)
		goto out;
//	pdata->active = MMA_STANDBY;
	msleep(MODE_CHANGE_DELAY_MS);
	return 0;
out:
	dev_err(&client->dev, "error when init mma7660:(%d)", result);
	return result;
}
static int mma7660_device_stop(struct i2c_client *client)
{
	u8 val;
	val = i2c_smbus_read_byte_data(client, MMA7660_MODE);
	i2c_smbus_write_byte_data(client, MMA7660_MODE,(val & ~0x01));
	return 0;
}

static int mma7660_read_data(struct i2c_client *client,struct mma7660_data_axis *data)
{
	u8 tmp_data[MMA7660_BUF_SIZE];
	int ret;
	
	do {
		ret = i2c_smbus_read_i2c_block_data(client,
				MMA7660_XOUT, 1, &tmp_data[0]);
		ret = i2c_smbus_read_i2c_block_data(client,
				MMA7660_YOUT, 1, &tmp_data[1]);
		ret = i2c_smbus_read_i2c_block_data(client,
				MMA7660_ZOUT, 1, &tmp_data[2]);
		if(ret < 1) break;
	}while((tmp_data[0] & (1<<6)) || (tmp_data[1] & (1<<6)) || (tmp_data[2] & (1<<6)));
/*
	ret = i2c_smbus_read_i2c_block_data(client,
			MMA7660_XOUT, 1, &tmp_data[0]);
	ret = i2c_smbus_read_i2c_block_data(client,
			MMA7660_YOUT, 1, &tmp_data[1]);
	ret = i2c_smbus_read_i2c_block_data(client,
			MMA7660_ZOUT, 1, &tmp_data[2]);


	if (ret < 1) {
		dev_err(&client->dev, "i2c block read failed\n");
		return -EIO;
	}
	if(tmp_data[0] & (1<<6)){
		 printk(KERN_ERR "************** ret = %d\n", ret);
	}
*/
//	data->x = (short)((tmp_data[0] > 32) ? (tmp_data[0] - 64) : tmp_data[0]);
//	data->y = (short)((tmp_data[1] > 32) ? (tmp_data[1] - 64) : tmp_data[1]);
//	data->z = (short)((tmp_data[2] > 32) ? (tmp_data[2] - 64) : tmp_data[2]);
	data->x = ((short)((tmp_data[0] << 10)) >>2) * 3;
	data->y = ((short)((tmp_data[1] << 10)) >>2) * 3;
	data->z = ((short)((tmp_data[2] << 10)) >>2) * 3;
	return 0;
}

static void average(void){
	int temp, i;	
	static int first_flag = 1;

	if(first_flag){
		for(i = 0; i < SENSOR_DATA_SIZE; i++){
			standby[i] = avg[i];
		}
		first_flag = 0;
	}
		
	
	for(i = 0; i < SENSOR_DATA_SIZE; i++){
		temp = avg[i] - standby[i];
		if(temp < max_num && temp > min_num){
			avg[i] = standby[i];	
		}else if((temp > max_num && temp < (2*max_num)) || (temp < min_num && temp > (2*min_num))){
			avg[i] = standby[i] + (avg[i] - standby[i])/2;
			standby[i] = avg[i];
		}else{
			standby[i] = avg[i];
		}
	}
}

static void gsensor_calibrate(struct i2c_client *client){
//	struct i2c_client *client = to_i2c_client(dev);
	struct mma7660_data *pdata = i2c_get_clientdata(client);
	int i;
	int xyz_acc[SENSOR_DATA_SIZE];
	static struct mma7660_data_axis data;

	memset(xyz_acc, 0, sizeof(xyz_acc));
	memset(avg, 0, sizeof(avg));
	
	for(i =0; i < AVG_NUM; i++){
		mma7660_read_data(pdata->mma7660_client, &data);
		xyz_acc[0] += data.x;
		xyz_acc[1] += data.y;
		xyz_acc[2] += data.z;
	}
	
	for(i = 0; i < SENSOR_DATA_SIZE; i++){
		avg[i] = (s16) (xyz_acc[i] / AVG_NUM);
//		printk(KERN_ERR "***avg[0] = %d\n", i, avg[0]);
	}
	average();
//	for(i = 0; i < SENSOR_DATA_SIZE; i++){
//		printk(KERN_ERR "************** %d********\n",avg[i]);
//	}
}

static void mma7660_work_func(struct work_struct *work)
{
#if 0
	struct input_polled_dev * poll_dev = pdata->poll_dev;
	struct mma7660_data_axis data;
	mutex_lock(&pdata->data_lock);
	if(pdata->active == MMA_STANDBY){
		poll_dev->poll_interval = POLL_STOP_TIME;
		goto out;
	}else{
		if(poll_dev->poll_interval == POLL_STOP_TIME)
			poll_dev->poll_interval = POLL_INTERVAL;
	}
	if (mma7660_read_data(pdata->client,&data) != 0)
		goto out;
    	mma7660_data_convert(pdata,&data);
	input_report_abs(poll_dev->input, ABS_X, data.x);
	input_report_abs(poll_dev->input, ABS_Y, data.y);
	input_report_abs(poll_dev->input, ABS_Z, data.z);
	input_sync(poll_dev->input);
out:
	mutex_unlock(&pdata->data_lock);
}
#else
	struct mma7660_data *pdata = container_of((struct delayed_work *)work,
			struct mma7660_data, work);
	unsigned long delay = msecs_to_jiffies(atomic_read(&pdata->delay));
	struct mma7660_data_axis data;
	delay /= 3;
//	mutex_lock(&pdata->data_lock);
//	if (mma7660_read_data(pdata->mma7660_client,&data) != 0)
//		goto out;
	mma7660_read_data(pdata->mma7660_client,&data);
	gsensor_calibrate(pdata->mma7660_client);
	data.x = avg[0];
	data.y = avg[1];
	data.z = -avg[2];
	mma7660_data_convert(pdata,&data);
	msleep(2);
    swap(data.x,data.y);
//	printk(KERN_ERR "data = %d, %d, %d\n", data.x, data.y, data.z);
	input_report_abs(pdata->input, ABS_X, data.x);
	input_report_abs(pdata->input, ABS_Y, data.y);
	input_report_abs(pdata->input, ABS_Z, data.z);
	input_sync(pdata->input);
	schedule_delayed_work(&pdata->work, delay);
//out:
//	mutex_unlock(&pdata->data_lock);
#endif
}


/*
static void mma7660_dev_poll(struct input_polled_dev *dev)
{
    struct mma7660_data* pdata = (struct mma7660_data*)dev->private;
	mma7660_report_data(pdata);
}

static ssize_t mma7660_enable_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct input_polled_dev *poll_dev = dev_get_drvdata(dev);
	struct mma7660_data *pdata = (struct mma7660_data *)(poll_dev->private);
	struct i2c_client *client = pdata->client;
	u8 val;
    int enable;
	
	mutex_lock(&pdata->data_lock);
	val = i2c_smbus_read_byte_data(client, MMA7660_MODE);  
	if((val & 0x01) && pdata->active == MMA_ACTIVED)
		enable = 1;
	else
		enable = 0;
	mutex_unlock(&pdata->data_lock);
	return sprintf(buf, "%d\n", enable);
}

static ssize_t mma7660_enable_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct input_polled_dev *poll_dev = dev_get_drvdata(dev);
	struct mma7660_data *pdata = (struct mma7660_data *)(poll_dev->private);
	struct i2c_client *client = pdata->client;
	int ret;
	unsigned long enable;
	u8 val = 0;
	enable = simple_strtoul(buf, NULL, 10);    
	mutex_lock(&pdata->data_lock);
	enable = (enable > 0) ? 1 : 0;
    if(enable && pdata->active == MMA_STANDBY)
	{  
	   val = i2c_smbus_read_byte_data(client,MMA7660_MODE);
	   ret = i2c_smbus_write_byte_data(client, MMA7660_MODE, val|0x01);  
	   if(!ret){
	   	 pdata->active = MMA_ACTIVED;
		 printk("mma enable setting active \n");
	    }
	}
	else if(enable == 0  && pdata->active == MMA_ACTIVED)
	{
		val = i2c_smbus_read_byte_data(client,MMA7660_MODE);
	    ret = i2c_smbus_write_byte_data(client, MMA7660_MODE,val & 0xFE);
		if(!ret){
		 pdata->active= MMA_STANDBY;
		 printk("mma enable setting inactive \n");
		}
	}
	mutex_unlock(&pdata->data_lock);
	return count;
}
*/

static ssize_t mma7660_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mma7660_data *pdata = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&pdata->delay));

}

static ssize_t mma7660_delay_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;
	struct i2c_client *client = to_i2c_client(dev);
	struct mma7660_data *pdata = i2c_get_clientdata(client);

	error = strict_strtoul(buf, 10, &data);
	if (error)
		return error;
	if (data > MMA7660_MAX_DELAY)
		data = MMA7660_MAX_DELAY;
	atomic_set(&pdata->delay, (unsigned int) data);

	return count;
}


static ssize_t mma7660_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = this_client;//to_i2c_client(dev);
	struct mma7660_data *pdata = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&pdata->enable));

}

static void mma7660_set_enable(struct device *dev, int enable)
{
	struct i2c_client *client = this_client;//to_i2c_client(dev);
	struct mma7660_data *pdata = i2c_get_clientdata(client);
	int pre_enable = atomic_read(&pdata->enable);
	int val;
	int ret;

	mutex_lock(&pdata->enable_mutex);
	if (enable) {
		if (pre_enable == 0) {
			val = i2c_smbus_read_byte_data(client,MMA7660_MODE);
			ret = i2c_smbus_write_byte_data(client, MMA7660_MODE, val|0x01);  
			if(!ret){
				 printk("mma enable setting active \n");
			}
			schedule_delayed_work(&pdata->work,
				msecs_to_jiffies(atomic_read(&pdata->delay)));
			atomic_set(&pdata->enable, 1);
		}

	} else {
		if (pre_enable == 1) {
			val = i2c_smbus_read_byte_data(client,MMA7660_MODE);
			ret = i2c_smbus_write_byte_data(client, MMA7660_MODE,val & 0xFE);
			if(!ret){
				 printk("mma enable setting inactive \n");
			}
			cancel_delayed_work_sync(&pdata->work);
			atomic_set(&pdata->enable, 0);
		}
	}
	mutex_unlock(&pdata->enable_mutex);

}

static ssize_t mma7660_enable_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;

	error = strict_strtoul(buf, 10, &data);
	if (error)
		return error;
	if ((data == 0) || (data == 1))
		mma7660_set_enable(dev, data);

	return count;
}

/*
static ssize_t mma7660_position_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
    	struct 
	struct input_polled_dev *poll_dev = dev_get_drvdata(dev);
//	struct mma7660_data *pdata = (struct mma7660_data *)(poll_dev->private);
  
 	int position = 0;
	mutex_lock(&pdata->data_lock);
	position = pdata->position ;
	mutex_unlock(&pdata->data_lock);
	return sprintf(buf, "%d\n", position);
}

static ssize_t mma7660_position_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
    struct input_polled_dev *poll_dev = dev_get_drvdata(dev);
	struct mma7660_data *pdata = (struct mma7660_data *)(poll_dev->private);
	int  position;
	position = simple_strtoul(buf, NULL, 10);    
	mutex_lock(&pdata->data_lock);
    pdata->position = position;
	mutex_unlock(&pdata->data_lock);
	return count;
}
*/
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP|S_IWOTH,
		   mma7660_enable_show, mma7660_enable_store);
//static DEVICE_ATTR(position, S_IWUSR | S_IRUGO,
//		   mma7660_position_show, mma7660_position_store);
static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP,
		   mma7660_delay_show, mma7660_delay_store);

static struct attribute *mma7660_attributes[] = {
	&dev_attr_enable.attr,
//	&dev_attr_position.attr,
	&dev_attr_delay.attr,
	NULL
};

static const struct attribute_group mma7660_attr_group = {
	.attrs = mma7660_attributes,
};

static int __init mma7660_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	int result;
	int err = 0;
	char buf[ITEM_MAX_LEN];
	struct input_dev *idev;
	struct mma7660_data *pdata;
//	struct i2c_adapter *adapter;
//	struct input_polled_dev *poll_dev;
	struct regulator *regulator;

//	adapter = to_i2c_adapter(client->dev.parent);

	if(item_exist("sensor.grivaty.power")){
		item_string(buf, "sensor.grivaty.power", 1);
		//sprintf(channel, "tps65910_%s", buf);
	
		regulator = regulator_get(&client->dev, buf);
		if(IS_ERR(regulator))
		{
			printk("%s: get regulator fail\n", __func__);
			return -1;
		}

		err = regulator_set_voltage(regulator, 0, 0);
		if(err)
		{
			printk("%s: ******set regulator fail\n", __func__);
		}
		
		msleep(20);
	
		err = regulator_set_voltage(regulator, 2800000, 3300000);
		if(err)
		{
			printk("%s: set regulator fail\n", __func__);
		}

		err = regulator_enable(regulator);
		printk("%s: regulator enable\n", __func__);
	
		msleep(2);	
	}


	result = i2c_check_functionality(client->adapter,
					 I2C_FUNC_SMBUS_BYTE |
					 I2C_FUNC_SMBUS_BYTE_DATA);
	if (!result)
		goto err_out;

	/*check connection */
	result = mma7660_device_detect(client);
	if(result)
	{
		result = -ENOMEM;
		dev_err(&client->dev, "detect the sensor connection error!\n");
		goto err_out;
	}
	pdata = kzalloc(sizeof(struct mma7660_data), GFP_KERNEL);
	if(!pdata){
		result = -ENOMEM;
		dev_err(&client->dev, "alloc data memory error!\n");
		goto err_out;
    }

//	i2c_set_clientdata(client,pdata);
	/* Initialize the MMA7660 chip */
/*
	if(item_exist("sensor.grivaty.orientation")){
		if(item_equal("sensor.grivaty.orientation", "xyz", 0) || item_equal("sensor.grivaty.orientation", "yxz", 0)){
			pdata->position = 0;
		}
		else if(item_equal("sensor.grivaty.orientation", "Xyz", 0) || item_equal("sensor.grivaty.orientation", "Yxz", 0)){
			pdata->position = 1;
		}
		else if(item_equal("sensor.grivaty.orientation", "XYz", 0) || item_equal("sensor.grivaty.orientation", "YXz", 0)){
			pdata->position = 2;
		}
		else if(item_equal("sensor.grivaty.orientation", "xYz", 0) || item_equal("sensor.grivaty.orientation", "yXz", 0)){
			pdata->position = 3;
		}
		else if(item_equal("sensor.grivaty.orientation", "xYZ", 0) || item_equal("sensor.grivaty.orientation", "yXZ", 0)){
			pdata->position = 4;
		}
		else if(item_equal("sensor.grivaty.orientation", "XYZ", 0) || item_equal("sensor.grivaty.orientation", "YXZ", 0)){
			pdata->position = 5;
		}
		else if(item_equal("sensor.grivaty.orientation", "XyZ", 0) || item_equal("sensor.grivaty.orientation", "YxZ", 0)){
			pdata->position = 6;
		}	
		else if(item_equal("sensor.grivaty.orientation", "xyZ", 0) || item_equal("sensor.grivaty.orientation", "yxZ", 0)){
			pdata->position = 7;
		}
	}
	else{
		pdata->position = CONFIG_SENSORS_MMA_POSITION;
	}
*/
	/*check connection */
	mutex_init(&pdata->data_lock);
	mutex_init(&pdata->enable_mutex);
	atomic_set(&pdata->delay, MMA7660_MAX_DELAY);
	atomic_set(&pdata->enable, 0);

//	mma7660_device_init(client);
	idev = input_allocate_device();
	if (!idev) {
		result = -ENOMEM;
		dev_err(&client->dev, "alloc poll device failed!\n");
		goto err_alloc_poll_device;
	}
/*
	poll_dev->poll = mma7660_dev_poll;
	poll_dev->poll_interval = POLL_STOP_TIME;
	poll_dev->poll_interval_min = POLL_INTERVAL_MIN;
	poll_dev->poll_interval_max = POLL_INTERVAL_MAX;
	poll_dev->private = pdata;
	idev = poll_dev->input;
*/
	idev->name = "mma7660";
	idev->uniq = "mma7660";
	idev->id.bustype = BUS_I2C;
	pdata->mma7660_client = client;
	this_client = client;
	i2c_set_clientdata(client,pdata);

	pdata->input = idev;
//	idev->evbit[0] = BIT_MASK(EV_ABS);
	set_bit(EV_ABS, pdata->input->evbit);
	input_set_abs_params(pdata->input, ABS_X, -0x7fff, 0x7fff, 0, 0);
	input_set_abs_params(pdata->input, ABS_Y, -0x7fff, 0x7fff, 0, 0);
	input_set_abs_params(pdata->input, ABS_Z, -0x7fff, 0x7fff, 0, 0);
       
//	pdata->mma7660_client = client;
//	i2c_set_clientdata(client,pdata);

	mma7660_device_init(client);
	INIT_DELAYED_WORK(&pdata->work, mma7660_work_func);
/*
	result = input_register_polled_device(pdata->poll_dev);
	if (result) {
		dev_err(&client->dev, "register poll device failed!\n");
		goto err_register_polled_device;
	}
*/
	input_set_drvdata(pdata->input, pdata);
	err = input_register_device(pdata->input);
	if (err < 0){
		input_free_device(pdata->input);
		goto err_alloc_poll_device;
	}
//	pdata->input = idev;
	result = sysfs_create_group(&pdata->input->dev.kobj, &mma7660_attr_group);
	if (result) {
		dev_err(&client->dev, "create device file failed!\n");
		result = -EINVAL;
		goto err_create_sysfs;
	}
	printk("mma7660 device driver probe successfully\n");
	return 0;
err_create_sysfs:
	input_unregister_device(pdata->input);
err_alloc_poll_device:
	kfree(pdata);
err_out:
	return result;
}
static int __exit mma7660_remove(struct i2c_client *client)
{
	struct mma7660_data *pdata = i2c_get_clientdata(client);
	
	regulator_put(regulator);
	sysfs_remove_group(&pdata->input->dev.kobj, &mma7660_attr_group);
	input_unregister_device(pdata->input);
	mma7660_device_stop(client);
/*	if(pdata){
		input_unregister_polled_device(poll_dev);
		input_free_polled_device(poll_dev);
		kfree(pdata);
	}
*/
	return 0;
}

#if 1
#ifdef CONFIG_PM_SLEEP
static int mma7660_suspend(struct device *dev)
{
	struct i2c_client *client = this_client;//to_i2c_client(dev);
  //    struct mma7660_data *pdata = i2c_get_clientdata(client);
   // if(pdata->active == MMA_ACTIVED)
	mma7660_device_stop(client);
	return 0;
}

static int mma7660_resume(struct device *dev)
{
    int val = 0;
	struct i2c_client *client = to_i2c_client(dev);
    struct mma7660_data *pdata = i2c_get_clientdata(client);
  //  if(pdata->active == MMA_ACTIVED){
	   val = i2c_smbus_read_byte_data(pdata->mma7660_client,MMA7660_MODE);
	   i2c_smbus_write_byte_data(pdata->mma7660_client, MMA7660_MODE, val|0x01);  
    
	return 0;
	  
}
#endif
#endif

static const struct i2c_device_id mma7660_id[] = {
	{"mma7660", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, mma7660_id);

static SIMPLE_DEV_PM_OPS(mma7660_pm_ops, mma7660_suspend, mma7660_resume);
static struct i2c_driver mma7660_driver = {
	.driver = {
		   .name = "mma7660",
		   .owner = THIS_MODULE,
		   .pm = &mma7660_pm_ops,
		   },
	.probe = mma7660_probe,
	.remove = mma7660_remove,
	.id_table = mma7660_id,
};

//extern int sensor_auto_detect;
int sensor_auto_detect = 0;
#define LOG printk

static int DoI2cScanDetectDev(void)
{

	struct i2c_adapter *adapter;
	__u16 i2cAddr= MMA7660_I2C_ADDR;//slave addr
	__u8 subByteAddr=0x0;//fix to read 0
	char databuf[8];
	int ret;

#define I2C_DETECT_TRY_SIZE  5
	int i,i2cDetecCnt=0;

	struct i2c_msg I2Cmsgs[2] = 
	{ 
		{
			.addr	= i2cAddr,
			.flags	= 0,
			.len	= 1,
			.buf	= &subByteAddr,
		},
		{
			.addr	= i2cAddr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= databuf,
		}
	};

	if(sensor_auto_detect ==1)
		return 0;

	LOG("A%s\n",__FUNCTION__);

	if(item_exist("sensor.grivaty"))
	{
		if(item_equal("sensor.grivaty.model", "auto", 0))
		{
			//get i2c handle	
			adapter = i2c_get_adapter(item_integer("sensor.grivaty.ctrl", 1));
			if (!adapter) {
				printk("****** get_adapter error! ******\n");
				return 0;
			}
			LOG("C%s\n",__FUNCTION__);
			//scan i2c 
			i2cDetecCnt=0;
			for(i=0;i<I2C_DETECT_TRY_SIZE;i++)
			{
				ret=i2c_transfer(adapter,I2Cmsgs,2);
				LOG("D-%s,ret=%d\n",__FUNCTION__,ret);
				if(ret>0)
					i2cDetecCnt++;				
			}
			LOG("E-%s,thd=%d,%d\n",__FUNCTION__,(I2C_DETECT_TRY_SIZE/2),i2cDetecCnt);
			if(i2cDetecCnt<=(I2C_DETECT_TRY_SIZE/2)){
				return 0;
			}else {
				sensor_auto_detect =1;
				return 1;
			}
		}
	}
	return 0;
}


static int __init mma7660_init(void)
{
	/* register driver */
	int res;
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	printk("****** %s run ! ******\n", __func__);

	if(item_exist("sensor.grivaty") && (item_equal("sensor.grivaty.model", "mma7660", 0) || DoI2cScanDetectDev()))
	{
		memset(&info, 0, sizeof(struct i2c_board_info));
		info.addr = MMA7660_I2C_ADDR;  
		strlcpy(info.type, "mma7660", I2C_NAME_SIZE);

		adapter = i2c_get_adapter(item_integer("sensor.grivaty.ctrl", 1));
		if (!adapter) {
			printk("*******get_adapter error!\n");
		}
		client = i2c_new_device(adapter, &info);
		
		res = i2c_add_driver(&mma7660_driver);
		if (res < 0) {
			printk(KERN_INFO "add mma7660 i2c driver failed\n");
			return -ENODEV;
		}
		return res;
	}
	else
	{
		printk("%s: G-sensor is not mma7660 or not exist.\n", __func__);
		return -1;
	}
}

static void __exit mma7660_exit(void)
{
	i2c_del_driver(&mma7660_driver);
}

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("MMA7660 3-Axis Orientation/Motion Detection Sensor driver");
MODULE_LICENSE("GPL");

late_initcall(mma7660_init);
module_exit(mma7660_exit);
