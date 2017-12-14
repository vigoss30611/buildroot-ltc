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
#include <linux/io.h>
#include <mach/gpio.h>
#include <mach/items.h>
#include <mach/pad.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <mach/mp8845c.h>
#define MP8845C_ADDR 0x1c

#define MP8845C_ADDR_READ 0x38
#define MP8845C_ADDR_WRITE 0x39
#define VOL_REG       0x00
#define SYS_CNTL1_REG 0x01
#define SYS_CNTL2_REG 0x02
#define ID1_REG       0X03
#define ID2_REG       0X04
#define STATUS_REG    0X05

static struct i2c_client *i2c_client_cpu;

static int __mp8845c_read(struct i2c_client *client, uint8_t reg,uint8_t *val)

{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading at 0x%02x\n", reg);
		return ret;
	}

	*val = (uint8_t)ret;

	return 0;
}

int mp8845c_cpu_read(struct device *dev, uint8_t reg,uint8_t *val)
{
	int ret;
	struct mp8845c *mp8845c = dev_get_drvdata(dev);

	mutex_lock(&mp8845c->mpc_lock);
	ret = __mp8845c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&mp8845c->mpc_lock);

	return ret;
}

static int __mp8845c_write(struct i2c_client *client, uint8_t reg, uint8_t val)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret < 0) {
		dev_err(&client->dev,
				"failed writing 0x%02x to 0x%02x\n", val, reg);
		return ret;
	}

	return 0;
}

int mp8845c_cpu_write(struct device *dev, uint8_t reg, uint8_t val)
{
	int ret;
	struct mp8845c *mp8845c = dev_get_drvdata(dev);

	mutex_lock(&mp8845c->mpc_lock);
	ret = __mp8845c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&mp8845c->mpc_lock);

	return ret;
}

static int mp8845c_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int mp8845c_remove_subdevs(struct mp8845c *mp8845c)
{
	return device_for_each_child(mp8845c->dev, NULL, mp8845c_remove_subdev);
}

static int mp8845c_add_subdevs(struct mp8845c *mp8845c,
				struct mp8845c_platform_data *pdata)
{
	struct mp8845c_subdev_info *subdev;
	struct platform_device *pdev;
	int i;
	int ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = mp8845c->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret)
			goto failed;
	}
	return 0;

failed:
	mp8845c_remove_subdevs(mp8845c);
	return ret;
}

static int mp8845c_init_chip(struct mp8845c *mp8845c)
{
	uint8_t tmp;

	mp8845c_cpu_read(mp8845c->dev, SYS_CNTL2_REG, &tmp);
	tmp |= 1 << 5;
	mp8845c_cpu_write(mp8845c->dev, SYS_CNTL2_REG, tmp);

	return 0;
}

static int __init mp8845c_probe(struct i2c_client *client,
				const struct i2c_device_id *id) 
{
	int result;
	struct mp8845c *mp8845c;
	struct mp8845c_platform_data *pdata = client->dev.platform_data;
	printk(KERN_ERR "mp8845c vcpu probe\n");

	mp8845c = kzalloc(sizeof(struct mp8845c), GFP_KERNEL);
	if (mp8845c == NULL)
		return -ENOMEM;

	mp8845c->client = client;
	mp8845c->dev = &client->dev;
	i2c_set_clientdata(client, mp8845c);

	mutex_init(&mp8845c->mpc_lock);

	result = mp8845c_add_subdevs(mp8845c, pdata);
	if (result) {
		dev_err(&client->dev, "add devices failed: %d\n", result);
		goto err_add_devs;
	}

	result = i2c_check_functionality(client->adapter, 
			I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA);   
	if (!result)                                                  
		goto err_add_devs;

	mp8845c_init_chip(mp8845c);

	i2c_client_cpu = client;

	return 0;	

err_add_devs:
	kfree(mp8845c);
	return result;
}

static int __exit mp8845c_remove(struct i2c_client *client)
{
	struct mp8845c *mp8845c = i2c_get_clientdata(client);

	mp8845c_remove_subdevs(mp8845c);
	kfree(mp8845c);
	return 0;
}

static const struct i2c_device_id mp8845c_id[] = {
	{"mp8845c-cpu", 0},                           
	{ }                                       
};                                                

static struct i2c_driver mp8845c_driver = {
	.driver = {
		.name = "mp8845c-cpu",
		.owner = THIS_MODULE,
	},
	.probe = mp8845c_probe,
	.remove = mp8845c_remove,
	.id_table = mp8845c_id,
};              

static int __init mp8845c_init(void)
{
	struct i2c_board_info info;  
	struct i2c_adapter *adapter; 
	struct i2c_client *client;

	if (!item_exist("config.vol.regulator")
		|| !item_equal("config.vol.regulator", "mp8845c", 0))
		return -1;

	memset(&info, 0, sizeof(struct i2c_board_info));
	info.addr = MP8845C_ADDR;
	info.platform_data = &mp8845c_cpu_platform_data;
	strlcpy(info.type, "mp8845c-cpu", I2C_NAME_SIZE);

	adapter = i2c_get_adapter(1);
	if (!adapter) {                                 
		printk(KERN_ERR "*******get_adapter error!\n");  
	}        
	client = i2c_new_device(adapter, &info);

	return i2c_add_driver(&mp8845c_driver);                                                 
}
module_init(mp8845c_init);

static void __exit mp8845c_exit(void)
{
	i2c_del_driver(&mp8845c_driver);
}
module_exit(mp8845c_exit);

MODULE_DESCRIPTION("mp8845c driver");
MODULE_LICENSE("GPL");
