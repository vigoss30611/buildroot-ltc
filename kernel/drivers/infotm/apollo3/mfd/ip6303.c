#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/mfd/core.h>
#include <linux/seq_file.h>
#include <mach/hw_cfg.h>
#include <mach/ip6303.h>
#include <linux/power_supply.h>

struct i2c_client *ip6303_i2c_client = NULL;

static int ip6303_i2c_write(struct i2c_client *client,
		uint8_t reg, uint8_t val)
{
	int ret;
	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret < 0) { 
		dev_err(&client->dev, "failed writing 0x%02x to 0x%02x, ret=%d\n",
				val, reg, ret);
		return ret;
	}

	return 0;
}

static int ip6303_i2c_read(struct i2c_client *client,
		uint8_t reg, uint8_t *val)
{
	int ret;
	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "failed reading at 0x%02x, ret=%d\n",
				reg, ret);
		return ret;
	}

	*val = (uint8_t)ret;

	return 0;
}

int ip6303_write(struct device *dev, uint8_t reg, uint8_t val)
{
	struct ip6303 *ip6303 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6303->io_lock);
	ret = ip6303_i2c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6303->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6303_write);

int ip6303_read(struct device *dev, uint8_t reg, uint8_t *val)
{
	struct ip6303 *ip6303 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6303->io_lock);
	ret = ip6303_i2c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6303->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6303_read);

int ip6303_set_bits(struct device *dev, uint8_t reg, uint8_t mask)
{
	struct ip6303 *ip6303 = dev_get_drvdata(dev);
	uint8_t val;
	int ret = 0;

	mutex_lock(&ip6303->io_lock);
	ret = ip6303_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if ((val & mask) != mask) {
		val |= mask;
		ret = ip6303_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip6303->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6303_set_bits);

int ip6303_clr_bits(struct device *dev, uint8_t reg, uint8_t mask)
{
	struct ip6303 *ip6303 = dev_get_drvdata(dev);
	uint8_t val;
	int ret = 0;

	mutex_lock(&ip6303->io_lock);
	ret = ip6303_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if (val & mask) {
		val &= ~mask;
		ret = ip6303_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip6303->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6303_clr_bits);

static void ip6303_chip_init(struct pmu_cfg *ip)
{
	uint8_t value = 0;

	/*enable DC1*/
	ip6303_i2c_read(ip6303_i2c_client, IP6303_DC_CTL, &value);
	value |= 0x1;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_DC_CTL, value);
	/*set DC1(core voltage):default voltage is 1.05v*/
	if(ip->core_voltage == -1)
		value = 0x24;
	else if(ip->core_voltage >= 1050)
		value = ((ip->core_voltage * 10) - 6000)/125;
	else if(ip->core_voltage < 1050){
		printk("The core voltage must be bigger than 1.05V\n");
		return;
	}
	ip6303_i2c_write(ip6303_i2c_client, IP6303_DC1_VSET, value);
	
#if 0
	ip6303_i2c_read(ip6303_i2c_client, IP6303_PSTATE_CTL1, &value);
	//reset key
	value |= 0x1;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_PSTATE_CTL1, value);

	//VBUS wake up enable
	ip6303_i2c_read(ip6303_i2c_client, IP6303_PSTATE_CTL0, &value);
	value |= 0x8;
	//short press wakeup disable
	value &= 0xef;
	//long press wakeup enable
	value |= 0x20;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_PSTATE_CTL0, value);

	ip6303_i2c_read(ip6303_i2c_client, IP6303_PSTATE_SET, &value);
	//set the power-key's super long press time from 6s to 10s
	value |= 0x8;
    	//set the long press time to 1s, but actually is 2s, as pmu's time clock is double while sleep
    	value &= 0xf9;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_PSTATE_SET, value);
#endif
}

void ip6303_deep_sleep(void)
{
	u8 value = 0;

	ip6303_i2c_read(ip6303_i2c_client, IP6303_PSTATE_CTL0, &value);
	value &= 0x6;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_PSTATE_CTL0, value);

	/*set LDO and DCDC shutdown in s2 and goto s3*/
	ip6303_i2c_read(ip6303_i2c_client, IP6303_POFF_LDO, &value);
	value &= 0x1;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_POFF_LDO, value);
	ip6303_i2c_read(ip6303_i2c_client, IP6303_POFF_DCDC, &value);
	value &= 0xf9;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_POFF_DCDC, value);

	/*set pmu Do not wake up when VBUS*/
	ip6303_i2c_read(ip6303_i2c_client, IP6303_PSTATE_CTL0, &value);
	value &= 0xf7;
	value |= 0x1;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_PSTATE_CTL0, value);
}
EXPORT_SYMBOL(ip6303_deep_sleep);

static int ip6303_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ip6303_remove_subdevs(struct ip6303 *ip6303)
{
	return device_for_each_child(ip6303->dev, NULL,
			ip6303_remove_subdev);
}

void ip6303_reset(void)
{
	u8 value = 0;
	/*set watch dog to count 2s and enable
	 * 	 * after 2s, the sys will reboot
	 * 	 	 */
	value = 0xc;
	ip6303_i2c_write(ip6303_i2c_client, IP6303_WDOG_CTL, value);
}
EXPORT_SYMBOL_GPL(ip6303_reset);

static int ip6303_add_subdevs(struct ip6303 *ip6303,
		struct ip6303_platform_data *pdata)
{
	struct ip6303_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ip6303->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret) {
			pr_err("ip6303_add_subdevs failed: %d\n", subdev->id);
			goto failed;
		}
	}
	return 0;

failed:
	ip6303_remove_subdevs(ip6303);
	return ret;
}

#ifdef IP6303_DEBUG_IF
static uint8_t reg_addr;

static ssize_t ip6303_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint8_t val;
	ip6303_read(&ip6303_i2c_client->dev, reg_addr, &val);
	return sprintf(buf, "REG[0x%02x]=0x%02x\n", reg_addr, val);
}

static ssize_t ip6303_reg_store(struct class *class,
		struct class_attribute *attr,
		const char *buf, size_t count)
{
	uint8_t val;
	uint16_t tmp;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp <= 0xff) {
		if (strncmp(buf, "00", 2))
			reg_addr = tmp;
		else
			ip6303_write(&ip6303_i2c_client->dev, 0x00, tmp);
	} else {
		val = tmp & 0xff;
		reg_addr = (tmp >> 8) & 0xff;
		ip6303_write(&ip6303_i2c_client->dev, reg_addr, val);
	}

	return count;
}

static ssize_t ip6303_poweron_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint8_t val;
	ip6303_read(&ip6303_i2c_client->dev, IP6303_PWRON_REC0, &val);
	if (val & 0x10)
		return sprintf(buf, "EXTINT\n");
	else if (val & 0x1)
		return sprintf(buf, "ACIN\n");
	else if (val & 0xe)
		return sprintf(buf, "POWERKEY\n");
	else
		return sprintf(buf, "Unknown\n");
}

static struct class_attribute ip6303_class_attrs[] = {
	__ATTR(reg, 0644, ip6303_reg_show, ip6303_reg_store),
	__ATTR(on, 0644, ip6303_poweron_show, NULL),
	__ATTR_NULL
};

static struct class ip6303_class = {
	.name = "pmu-debug",
	.class_attrs = ip6303_class_attrs,
};
#endif
static int ip6303_probe(struct i2c_client *client, 
					const struct i2c_device_id *id)
{
	struct ip6303_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	struct ip6303 *ip6303;
	int ret = 0;

	if (strcmp(ip->name, "ip6303")){
		pr_err("PMU is not ip6303\n");
		ret = -ENODEV;
		goto err_nodev;
	}

	ip6303 = kzalloc(sizeof(struct ip6303), GFP_KERNEL);
	if (ip6303 == NULL) {
		pr_err("PMU driver error: failed to alloc mem\n");
		ret = -ENOMEM;
		goto err_nomem;
	}

	ip6303->client = client;
	ip6303->dev = &client->dev;
	i2c_set_clientdata(client, ip6303);

	mutex_init(&ip6303->io_lock);

	ip6303_i2c_client = client;

	ip6303_chip_init(ip);

	ret = ip6303_add_subdevs(ip6303, pdata);
	if (ret) {
		pr_err("add sub devices failed: %d\n", ret);
		kfree(ip6303);
		goto err_add_subdevs;
	}

#ifdef IP6303_DEBUG_IF
	class_register(&ip6303_class);
#endif
	IP6303_ERR("PMU: %s done!\n", __func__);

	return 0;

err_add_subdevs:
err_nomem:
err_nodev:
	IP6303_ERR("%s: error! ret=%d\n", __func__, ret);
	return ret;
}

static int ip6303_remove(struct i2c_client *client)
{
	struct ip6303 *ip6303 = i2c_get_clientdata(client);

	ip6303_remove_subdevs(ip6303);
	kfree(ip6303);

	return 0;
}

static void ip6303_shutdown(struct i2c_client *client)
{
	return;
}

#ifdef CONFIG_PM
static int ip6303_suspend(struct i2c_client *client, pm_message_t state)
{
	return 0;
}

static int ip6303_resume(struct i2c_client *client)
{
	return 0;
}
#endif

static const struct i2c_device_id ip6303_i2c_id[] = {
	{"ip6303", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, ip6303_i2c_id);

static struct i2c_driver ip6303_i2c_driver = {
	.driver = {
		.name = "ip6303",
		.owner = THIS_MODULE,
	},
	.probe = ip6303_probe,
	.remove = ip6303_remove,
	.shutdown = ip6303_shutdown,
#ifdef CONFIG_PM
	.suspend = ip6303_suspend,
	.resume = ip6303_resume,
#endif
	.id_table = ip6303_i2c_id,
};

static int __init ip6303_init(void)
{
	int ret = -ENODEV;

	ret = i2c_add_driver(&ip6303_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ip6303_init);

static void __exit ip6303_exit(void)
{
	i2c_del_driver(&ip6303_i2c_driver);
}
module_exit(ip6303_exit);

MODULE_DESCRIPTION("Injoinic PMU ip6303 driver");
MODULE_LICENSE("GPL");
