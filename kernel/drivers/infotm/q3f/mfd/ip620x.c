#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/mfd/core.h>
#include <linux/seq_file.h>
#include <mach/hw_cfg.h>
#include <mach/ip6208.h>
#include <mach/ip6205.h>
#include <mach/ec610.h>
#include <linux/power_supply.h>

struct i2c_client *ip620x_i2c_client = NULL;
int gSensorEnable;
EXPORT_SYMBOL(gSensorEnable);

static int ip620x_i2c_write(struct i2c_client *client,
		uint8_t reg, uint16_t val)
{
	int ret;
	uint8_t buf[3];
	struct i2c_msg msg;

	buf[0] = reg;
	buf[1] = (uint8_t)((val & 0xff00) >> 8);
	buf[2] = (uint8_t)(val & 0xff);

	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = 3;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret != 1) {
		pr_err("failed to write 0x%x to 0x%x\n", val, reg);
		return -1;
	}

	return 0;
}

static int ip620x_i2c_read(struct i2c_client *client,
		uint8_t reg, uint16_t *val)
{
	int ret;
	uint8_t buf[2];
	struct i2c_msg msg[2];

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = &reg;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = buf;
	msg[1].len = 2;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret != 2) {
		pr_err("failed to read 0x%x\n", reg);
		return -1;
	}

	*val = (buf[0] << 8) | buf[1];

	return 0;
}

int ip620x_write(struct device *dev, uint8_t reg, uint16_t val)
{
	struct ip620x *ip620x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip620x->io_lock);
	ret = ip620x_i2c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip620x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_write);

int ip620x_read(struct device *dev, uint8_t reg, uint16_t *val)
{
	struct ip620x *ip620x = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip620x->io_lock);
	ret = ip620x_i2c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip620x->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_read);

/*------------------------------------------------------*/
/*                 codec io interface     	        */
/*------------------------------------------------------*/
int ip620x_codec_write(uint8_t reg, uint16_t val)
{
	struct ip620x *ip620x = dev_get_drvdata(&ip620x_i2c_client->dev);
	int ret = 0;

	if(reg >= 0xa0 && reg <= 0xf4){
		mutex_lock(&ip620x->io_lock);
		ret = ip620x_i2c_write(to_i2c_client(&ip620x_i2c_client->dev), reg, val);
		mutex_unlock(&ip620x->io_lock);
	}else{
		pr_err("write codec reg addr is over the value\n");
	}

	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_codec_write);

int ip620x_codec_read(uint8_t reg, uint16_t *val)
{
	struct ip620x *ip620x = dev_get_drvdata(&ip620x_i2c_client->dev);
	int ret = 0;

	if(reg >= 0xa0 && reg <= 0xf4){
		mutex_lock(&ip620x->io_lock);
		ret = ip620x_i2c_read(to_i2c_client(&ip620x_i2c_client->dev), reg, val);
		mutex_unlock(&ip620x->io_lock);
	}else{
		pr_err("read codec reg addr is over the value\n");
	}

	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_codec_read);

int ip620x_set_bits(struct device *dev, uint8_t reg, uint16_t mask)
{
	struct ip620x *ip620x = dev_get_drvdata(dev);
	uint16_t val;
	int ret = 0;

	mutex_lock(&ip620x->io_lock);
	ret = ip620x_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if ((val & mask) != mask) {
		val |= mask;
		ret = ip620x_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip620x->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_set_bits);

int ip620x_clr_bits(struct device *dev, uint8_t reg, uint16_t mask)
{
	struct ip620x *ip620x = dev_get_drvdata(dev);
	uint16_t val;
	int ret = 0;

	mutex_lock(&ip620x->io_lock);
	ret = ip620x_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if (val & mask) {
		val &= ~mask;
		ret = ip620x_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip620x->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip620x_clr_bits);

#define IP620x_DC_CTL 0x50 
#define IP620x_DC1_VSET 0x55 
#define IP620x_PSTATE_CTL 0x0 
#define IP620x_PSTATE_SET 0x1 
#define IP620x_POFF_LDO 0x16 
#define IP620x_POFF_DCDC 0x17 
#define IP620x_PPATH_STATUS 0x3 
static void ip620x_chip_init(struct pmu_cfg *ip)
{
	u16 value = 0;

	/*enable DC1*/
	ip620x_i2c_read(ip620x_i2c_client, IP620x_DC_CTL, &value);
	value |= 0x2;
	ip620x_i2c_write(ip620x_i2c_client, IP620x_DC_CTL, value);
	/*set DC1(core voltage)::default voltage is 1.05v*/
	if(ip->core_voltage == -1)
		value = 0x24;
	else if(ip->core_voltage >= 1050)
		value = ((ip->core_voltage * 10) - 6000)/125;
	else if(ip->core_voltage < 1050){
		printk("The core voltage must be bigger than 1.05V\n");
		return;
	}
	ip620x_i2c_write(ip620x_i2c_client, IP620x_DC1_VSET, value);
	
	ip620x_i2c_read(ip620x_i2c_client, IP620x_PSTATE_CTL, &value);
	//reset key
	value |= 0x200;
    //dc in
	value |= 0x8;
    //short press wakeup disable
    value &= 0xffef;
    //long press wakeup enable
    value |= 0x20;
	ip620x_i2c_write(ip620x_i2c_client, IP620x_PSTATE_CTL, value);

	ip620x_i2c_read(ip620x_i2c_client, IP620x_PSTATE_SET, &value);
	//set the power-key's super long press time from 6s to 10s
	value |= 0x10;
    //set the long press time to 1s, but actually is 2s, as pmu's time clock is double while sleep
    value &= 0xfff3;
	ip620x_i2c_write(ip620x_i2c_client, IP620x_PSTATE_SET, value);
}

void ip620x_deep_sleep(void)
{
	u16 value = 0;

	/*set pmu do not wake up DCIN or VBUS*/
	ip620x_i2c_read(ip620x_i2c_client, IP620x_PSTATE_SET, &value);
	value &= 0xc;
	value |= 0x2;
	ip620x_i2c_write(ip620x_i2c_client, IP620x_PSTATE_SET, value);

	/*set LDO and DCDC shutdown in s2 and goto s3*/
	value = 0x0;
	ip620x_i2c_write(ip620x_i2c_client, IP620x_POFF_LDO, value);
	ip620x_i2c_write(ip620x_i2c_client, IP620x_POFF_DCDC, value);
	/*set DCIN*/
	/*1:judge if DCIN exist or not
	 *2:acroding to 1, set bit3 in IP620x_PSTATE_CTL
	 * */
	value = 0;
	ip620x_i2c_read(ip620x_i2c_client, IP620x_PPATH_STATUS, &value);
	if(value & 0x2){
		/*exist DCIN*/
		ip620x_i2c_read(ip620x_i2c_client, IP620x_PSTATE_CTL, &value);
		value &= 0xfff3; 
		value |= 0x1;
		ip620x_i2c_write(ip620x_i2c_client, IP620x_PSTATE_CTL, value);
	}else{
		/*NO DCIN, maybe BAT exist*/
		ip620x_i2c_read(ip620x_i2c_client, IP620x_PSTATE_CTL, &value);
		value &= 0xfff3; 
		value |= 0x1;
		value |= 0x8;
		ip620x_i2c_write(ip620x_i2c_client, IP620x_PSTATE_CTL, value);
	}
}
EXPORT_SYMBOL(ip620x_deep_sleep);

static int ip620x_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ip620x_remove_subdevs(struct ip620x *ip620x)
{
	return device_for_each_child(ip620x->dev, NULL,
			ip620x_remove_subdev);
}

void ip620x_reset(void)
{
	u8 value = 0;
	/*set watch dog to count 2s and enable
	 * 	 * after 2s, the sys will reboot
	 * 	 	 */
	value = 0xc;
	ip620x_i2c_write(ip620x_i2c_client, IP6208_WDOG_CTL, value);
}
EXPORT_SYMBOL_GPL(ip620x_reset);

static int ip620x_add_subdevs(struct ip620x *ip620x,
		struct ip620x_platform_data *pdata)
{
	struct ip620x_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ip620x->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret) {
			pr_err("ip620x_add_subdevs failed: %d\n", subdev->id);
			goto failed;
		}
	}
	return 0;

failed:
	ip620x_remove_subdevs(ip620x);
	return ret;
}

#ifdef CONFIG_PMIC_DEBUG_IF
static uint8_t reg_addr;

static ssize_t ip620x_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint16_t val;
	ip620x_read(&ip620x_i2c_client->dev, reg_addr, &val);
	return sprintf(buf, "REG[0x%02x]=0x%04x\n", reg_addr, val);
}

static ssize_t ip620x_reg_store(struct class *class,
		struct class_attribute *attr,
		const char *buf, size_t count)
{
	uint16_t val;
	uint32_t tmp;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp <= 0xff) {
		reg_addr = tmp;
	} else {
		val = tmp & 0xffff;
		reg_addr = (tmp >> 16) & 0xff;
		ip620x_write(&ip620x_i2c_client->dev, reg_addr, val);
	}

		return count;
}

static ssize_t ip620x_poweron_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint16_t val;
	ip620x_read(&ip620x_i2c_client->dev, 0xf, &val);
	if (val & 0x40)
		return sprintf(buf, "EXTINT\n");
	else if (val & 0x2)
		return sprintf(buf, "ACIN\n");
	else if (val & 0x30)
		return sprintf(buf, "POWERKEY\n");
	else
		return sprintf(buf, "Unknown\n");
}

static struct class_attribute ip620x_class_attrs[] = {
	__ATTR(reg, 0644, ip620x_reg_show, ip620x_reg_store),
	__ATTR(on, 0644, ip620x_poweron_show, NULL),
	__ATTR_NULL
};

static struct class ip620x_class = {
	.name = "pmu-debug",
	.class_attrs = ip620x_class_attrs,
};
#endif
#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE
extern void batt_set_charging_status(int batt_status);
#endif
static int ip620x_probe(struct i2c_client *client, 
					const struct i2c_device_id *id)
{
	struct ip620x_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	struct ip620x *ip620x;
	int ret = 0;

	if (strcmp(ip->name, "ip6208") && strcmp(ip->name, "ip6205")){
		pr_err("PMU is not ip6208 / ip6205\n");
		ret = -ENODEV;
		goto err_nodev;
	}

	ip620x = kzalloc(sizeof(struct ip620x), GFP_KERNEL);
	if (ip620x == NULL) {
		pr_err("PMU driver error: failed to alloc mem\n");
		ret = -ENOMEM;
		goto err_nomem;
	}

	ip620x->client = client;
	ip620x->dev = &client->dev;
	i2c_set_clientdata(client, ip620x);

	mutex_init(&ip620x->io_lock);

	ip620x_i2c_client = client;

	ip620x_chip_init(ip);

	ret = ip620x_add_subdevs(ip620x, pdata);
	if (ret) {
		pr_err("add sub devices failed: %d\n", ret);
		kfree(ip620x);
		goto err_add_subdevs;
	}

#ifdef CONFIG_PMIC_DEBUG_IF
	class_register(&ip620x_class);
#endif
	IP620x_ERR("PMU: %s done!\n", __func__);

#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE	
    int val = 0;
    ip620x_i2c_read(ip620x_i2c_client, IP620x_PPATH_STATUS, &val);
	u8 gpio_val=0;
	int index;
	int abs_index;
	if (val & 0x2) {		
		if(item_exist("power.dcdetect"))
		{
			index = item_integer("power.dcdetect", 1);
			abs_index = (index>0)? index : (-1*index);
			if (gpio_is_valid(abs_index))
			{
				gpio_val =gpio_get_value(abs_index);
				if(1 == gpio_val)
				{
					batt_set_charging_status(POWER_SUPPLY_STATUS_CHONNGDIANBAO_CHARGING);
					pr_err("chongdianbao has inserted\n");
				}
				else
				{

					batt_set_charging_status(POWER_SUPPLY_STATUS_CHARGING);
					pr_err("chongdianqi has inserted\n");
				}
			}
		}
	}	
	else 	
	{		
		batt_set_charging_status(POWER_SUPPLY_STATUS_DISCHARGING);
		pr_err("dcin has pull out\n");	
	}
#endif	
	return 0;

err_add_subdevs:
err_nomem:
err_nodev:
	IP620x_ERR("%s: error! ret=%d\n", __func__, ret);
	return ret;
}

static int ip620x_remove(struct i2c_client *client)
{
	struct ip620x *ip620x = i2c_get_clientdata(client);

	ip620x_remove_subdevs(ip620x);
	kfree(ip620x);

	return 0;
}

static void ip620x_shutdown(struct i2c_client *client)
{
}

#ifdef CONFIG_PM
static int ip620x_suspend(struct i2c_client *client, pm_message_t state)
{
	return 0;
}

static int ip620x_resume(struct i2c_client *client)
{
	return 0;
}
#endif

static const struct i2c_device_id ip620x_i2c_id[] = {
	{"ip6208", 0},
	{"ip6205", 1},
	{}
};
MODULE_DEVICE_TABLE(i2c, ip620x_i2c_id);

static struct i2c_driver ip620x_i2c_driver = {
	.driver = {
		.name = "ip620x",
		.owner = THIS_MODULE,
	},
	.probe = ip620x_probe,
	.remove = ip620x_remove,
	.shutdown = ip620x_shutdown,
#ifdef CONFIG_PM
	.suspend = ip620x_suspend,
	.resume = ip620x_resume,
#endif
	.id_table = ip620x_i2c_id,
};

static int __init ip620x_init(void)
{
	int ret = -ENODEV;

	ret = i2c_add_driver(&ip620x_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ip620x_init);

static void __exit ip620x_exit(void)
{
	i2c_del_driver(&ip620x_i2c_driver);
}
module_exit(ip620x_exit);

MODULE_DESCRIPTION("Injoinic PMU ip6208 driver");
MODULE_LICENSE("Dual BSD/GPL");
