#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <mach/items.h>
#include <linux/random.h>


struct delayed_work inner_test_work;
struct inner_test_data {
	struct i2c_client *inner_client;
	struct delayed_work work;
};


static u32 _read_interface(struct i2c_client *client, u8 reg, u8 *buf, u32 num)                             
{                                                                                                              
	int ret;
	struct i2c_msg xfer_msg[2];                                                                                

	xfer_msg[0].addr = client->addr;                                                                           
	xfer_msg[0].len = 1;                                                                                       
	xfer_msg[0].flags = client->flags & I2C_M_TEN;                                                             
	xfer_msg[0].buf = &reg;                                                                                    

	xfer_msg[1].addr = client->addr;                                                                           
	xfer_msg[1].len = num;                                                                                     
	xfer_msg[1].flags |= I2C_M_RD;                                                                             
	xfer_msg[1].buf = buf;                                                                                     

	ret = i2c_transfer(client->adapter, xfer_msg, ARRAY_SIZE(xfer_msg));                                         
	msleep(5);                                                                                             

	return ret == 2 ? 0 : -EFAULT;
}                                                                                                              

static u32 _write_interface(struct i2c_client *client, const u8 reg, u8 *buf, u32 num)                      
{                                                                                                              
	struct i2c_msg xfer_msg[1];                                                                                

	buf[0] = reg;                                                                                              

	xfer_msg[0].addr = client->addr;                                                                           
	xfer_msg[0].len = num + 1;                                                                                 
	xfer_msg[0].flags = client->flags & I2C_M_TEN;                                                             
	xfer_msg[0].buf = buf;                                                                                     

	return i2c_transfer(client->adapter, xfer_msg, 1) == 1 ? 0 : -EFAULT;                                      
}                                                                                                              

#define DATA_SIZE	0xff

static void inner_test_work_func(struct work_struct *work)
{
	int i;
	int test_result = 0;
	uint8_t data_send[DATA_SIZE + 1];
	uint8_t data_rev[DATA_SIZE];
	struct inner_test_data *pdata = container_of((struct delayed_work *)work,
			struct inner_test_data, work);

	for (i = 1; i < DATA_SIZE + 1; i++) {
		data_send[i] = get_random_int() % 0xff;
//		printk("%s data[%d]:%x\n",__func__,i,data_send[i]);
	}

	if (_write_interface(pdata->inner_client, 0, data_send, DATA_SIZE)) {
		printk("%s write error\n", __func__);
	}
	if (_read_interface(pdata->inner_client, 0, data_rev, DATA_SIZE)) {
		printk("%s read error\n", __func__);
	}

	for (i = 0; i < DATA_SIZE; i++) {
		if(data_send[i + 1] != data_rev[i]) {
			printk("%s data[%d] read %x not equal to write %x\n", 
					__func__, i, data_rev[i], data_send[i]);
			test_result = 1;
		}
	}

	if(!test_result)
		printk("%s test OK!\n",__func__);

	schedule_delayed_work(&pdata->work, msecs_to_jiffies(500));
}



static int __init inner_test_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct inner_test_data *data;

	data = kzalloc(sizeof(struct inner_test_data), GFP_KERNEL);
	if (!data) {
		dev_err(&client->dev, "mm error for inner test driver!\n");
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&data->work, inner_test_work_func);
	schedule_delayed_work(&data->work, 0);
	printk("iic innner device driver probe successfully\n");

	return 0;
}

static int inner_test_remove(struct i2c_client *client)
{
	return 0;
}


static const struct i2c_device_id test_id[] = {
	{"inner_test", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, test_id);

static struct i2c_driver inner_test_driver = {
	.driver = {
		   .name = "inner_test",
		   .owner = THIS_MODULE,
		   },
	.probe = inner_test_probe,
	.remove = inner_test_remove,
	.id_table = test_id,
};

static int __init i2c_inner_test_init(void)
{
	int res;
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	if (item_exist("iic.inner.test")) {
		memset(&info, 0, sizeof(struct i2c_board_info));
		info.addr = item_integer("iic.slave.addr", 0);
		strlcpy(info.type, "iic_slave", I2C_NAME_SIZE);                     
		                                                                  
		adapter = i2c_get_adapter(item_integer("iic.inner.test.host", 1));
		if (!adapter) {
			    printk("*******get_adapter error!\n");
				return -1;
		}                                                                 
		client = i2c_new_device(adapter, &info);

		res = i2c_add_driver(&inner_test_driver);
		if (res < 0) {
			    printk(KERN_INFO "add i2c inner test driver failed\n");
				    return -ENODEV;
		}
		return res;
	}

	return -1;
}

static void __exit i2c_inner_test_exit(void)
{
	i2c_del_driver(&inner_test_driver);
}

MODULE_LICENSE("GPL");

late_initcall(i2c_inner_test_init);
module_exit(i2c_inner_test_exit);
