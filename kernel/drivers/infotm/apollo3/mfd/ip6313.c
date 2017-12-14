#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/mfd/core.h>
#include <linux/seq_file.h>
#include <mach/hw_cfg.h>
#include <mach/ip6313.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

struct i2c_client *ip6313_i2c_client = NULL;

static int ip6313_i2c_write(struct i2c_client *client,
		uint8_t reg, uint16_t val)
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

static int ip6313_i2c_read(struct i2c_client *client,
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

int ip6313_write(struct device *dev, uint8_t reg, uint8_t val)
{
	struct ip6313 *ip6313 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6313->io_lock);
	ret = ip6313_i2c_write(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6313->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6313_write);

int ip6313_read(struct device *dev, uint8_t reg, uint8_t *val)
{
	struct ip6313 *ip6313 = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&ip6313->io_lock);
	ret = ip6313_i2c_read(to_i2c_client(dev), reg, val);
	mutex_unlock(&ip6313->io_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(ip6313_read);

int ip6313_set_bits(struct device *dev, uint8_t reg, uint8_t mask)
{
	struct ip6313 *ip6313 = dev_get_drvdata(dev);
	uint8_t val;
	int ret = 0;

	mutex_lock(&ip6313->io_lock);
	ret = ip6313_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if ((val & mask) != mask) {
		val |= mask;
		ret = ip6313_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip6313->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6313_set_bits);

int ip6313_clr_bits(struct device *dev, uint8_t reg, uint8_t mask)
{
	struct ip6313 *ip6313 = dev_get_drvdata(dev);
	uint8_t val;
	int ret = 0;

	mutex_lock(&ip6313->io_lock);
	ret = ip6313_i2c_read(to_i2c_client(dev), reg, &val);
	if (ret)
		goto exit;

	if (val & mask) {
		val &= ~mask;
		ret = ip6313_i2c_write(to_i2c_client(dev), reg, val);
	}

exit:
	mutex_unlock(&ip6313->io_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(ip6313_clr_bits);

static void ip6313_chip_init(struct pmu_cfg *ip)
{
	u8 value = 0;

	/*enable DC1*/
	ip6313_i2c_read(ip6313_i2c_client, IP6313_DC_CTL, &value);
	value |= 0x1;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_DC_CTL, value);
	/*set DC1(core voltage):default voltage is 1.05v*/
	if(ip->core_voltage == -1)
		    value = 0x24;
	else if(ip->core_voltage >= 1050) 
		    value = ((ip->core_voltage * 10) - 6000)/125;          
	else if(ip->core_voltage < 1050){ 
		    printk("The core voltage must be bigger than 1.05V\n");
			return;
	}
	ip6313_i2c_write(ip6313_i2c_client, IP6313_DC1_VSET, value);
	
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL1, &value);
	//reset key
	value |= 0x1;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL1, value);

	//VBUS wake up enable
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL0, &value);
	value |= 0x8;
	//short press wakeup disable
	value &= 0xef;
	//long press wakeup enable
	value |= 0x20;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL0, value);

	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_SET, &value);
	//set the power-key's super long press time from 6s to 10s
	value |= 0x8;
    	//set the long press time to 1s, but actually is 2s, as pmu's time clock is double while sleep
    	value &= 0xf9;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_SET, value);
}

void ip6313_deep_sleep(void)
{
	u8 value = 0;

	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL0, &value);
	value &= 0x6;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL0, value);

	/*set LDO and DCDC shutdown in s2 and goto s3*/
	ip6313_i2c_read(ip6313_i2c_client, IP6313_POFF_LDO, &value);
	value &= 0x1;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_POFF_LDO, value);
	ip6313_i2c_read(ip6313_i2c_client, IP6313_POFF_DCDC, &value);
	value &= 0xf9;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_POFF_DCDC, value);

	/*set pmu Do not wake up when VBUS*/
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL0, &value);
	value &= 0xf7;
	value |= 0x1;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL0, value);
}
EXPORT_SYMBOL(ip6313_deep_sleep);

void ip6313_sleep(void){
	u16 value = 0;
	/*set 0x6 reg bit 2 to 1'b0*/
	/*
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PROTECT_CTL0, &value);
	value &= ~(0x4);
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PROTECT_CTL0, value);
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PROTECT_CTL0, &value);
	printk("--------------%s:IP6313_PROTECT_CTL0,%x\n",__func__,value);
	*/
	/*
	 *in apollo3 , the reset signer must keep high, in which condition, 
	 the cpu can wake up. otherwise, the ON/OFF key can not wake the cpu up
	 */
	/*
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_SET, &value);
	value |= 0x40;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_SET, value);
	*/
	//set wake up source
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL0, &value);
	value |= 0xf0;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL0, value);
	ip6313_i2c_read(ip6313_i2c_client, IP6313_PSTATE_CTL0, &value);
	value |= 0x8;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_PSTATE_CTL1, value);
	//set the channel, which keep on when sleep
	/*channel which connect to rtc and ddr must keep on when sleep*/
	value = 0x2;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_POFF_LDO, value);
	value = 0x4;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_POFF_DCDC, value);
}

static int ip6313_remove_subdev(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

static int ip6313_remove_subdevs(struct ip6313 *ip6313)
{
	return device_for_each_child(ip6313->dev, NULL,
			ip6313_remove_subdev);
}

void ip6313_reset(void)
{
	u8 value = 0;
	/*set watch dog to count 2s and enable
	 * 	 * after 2s, the sys will reboot
	 * 	 	 */
	value = 0xc;
	ip6313_i2c_write(ip6313_i2c_client, IP6313_WDOG_CTL, value);
}
EXPORT_SYMBOL_GPL(ip6313_reset);

static int ip6313_add_subdevs(struct ip6313 *ip6313,
		struct ip6313_platform_data *pdata)
{
	struct ip6313_subdev_info *subdev;
	struct platform_device *pdev;
	int i, ret = 0;

	for (i = 0; i < pdata->num_subdevs; i++) {
		subdev = &pdata->subdevs[i];

		pdev = platform_device_alloc(subdev->name, subdev->id);

		pdev->dev.parent = ip6313->dev;
		pdev->dev.platform_data = subdev->platform_data;

		ret = platform_device_add(pdev);
		if (ret) {
			pr_err("ip6313_add_subdevs failed: %d\n", subdev->id);
			goto failed;
		}
	}
	return 0;

failed:
	ip6313_remove_subdevs(ip6313);
	return ret;
}

#ifdef CONFIG_PMIC_DEBUG_IF
static uint8_t reg_addr;

static ssize_t ip6313_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint8_t val;
	ip6313_read(&ip6313_i2c_client->dev, reg_addr, &val);
	return sprintf(buf, "REG[0x%02x]=0x%04x\n", reg_addr, val);
}

static ssize_t ip6313_reg_store(struct class *class,
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
		ip6313_write(&ip6313_i2c_client->dev, reg_addr, val);
	}

		return count;
}

static ssize_t ip6313_poweron_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint8_t val;
	ip6313_read(&ip6313_i2c_client->dev, 0xf, &val);
	if (val & 0x40)
		return sprintf(buf, "EXTINT\n");
	else if (val & 0x2)
		return sprintf(buf, "ACIN\n");
	else if (val & 0x30)
		return sprintf(buf, "POWERKEY\n");
	else
		return sprintf(buf, "Unknown\n");
}

static struct class_attribute ip6313_class_attrs[] = {
	__ATTR(reg, 0644, ip6313_reg_show, ip6313_reg_store),
	__ATTR(on, 0644, ip6313_poweron_show, NULL),
	__ATTR_NULL
};

static struct class ip6313_class = {
	.name = "pmu-debug",
	.class_attrs = ip6313_class_attrs,
};
#endif
static int ip6313_probe(struct i2c_client *client, 
					const struct i2c_device_id *id)
{
	struct ip6313_platform_data *pdata = client->dev.platform_data;
	struct pmu_cfg *ip = pdata->ip;
	struct ip6313 *ip6313;
	int ret = 0;

	if (strcmp(ip->name, "ip6313")){
		pr_err("PMU is not ip6313\n");
		ret = -ENODEV;
		goto err_nodev;
	}

	ip6313 = kzalloc(sizeof(struct ip6313), GFP_KERNEL);
	if (ip6313 == NULL) {
		pr_err("PMU driver error: failed to alloc mem\n");
		ret = -ENOMEM;
		goto err_nomem;
	}

	ip6313->client = client;
	ip6313->dev = &client->dev;
	i2c_set_clientdata(client, ip6313);

	mutex_init(&ip6313->io_lock);

	ip6313_i2c_client = client;

	ip6313_chip_init(ip);

	ret = ip6313_add_subdevs(ip6313, pdata);
	if (ret) {
		pr_err("add sub devices failed: %d\n", ret);
		kfree(ip6313);
		goto err_add_subdevs;
	}

#ifdef CONFIG_PMIC_DEBUG_IF
	class_register(&ip6313_class);
#endif
	IP6313_ERR("PMU: %s done!\n", __func__);

	return 0;

err_add_subdevs:
err_nomem:
err_nodev:
	IP6313_ERR("%s: error! ret=%d\n", __func__, ret);
	return ret;
}

static int ip6313_remove(struct i2c_client *client)
{
	struct ip6313 *ip6313 = i2c_get_clientdata(client);

	ip6313_remove_subdevs(ip6313);
	kfree(ip6313);

	return 0;
}

static void ip6313_shutdown(struct i2c_client *client)
{
}

#ifdef CONFIG_PM
static int ip6313_suspend(struct i2c_client *client, pm_message_t state)
{
	struct ip6313 *ip6313 = i2c_get_clientdata(client);
	printk("PMU : %s\n",__func__);
	if(ip6313->irq)
		disable_irq(ip6313->irq);
	ip6313_sleep();
	return 0;
}

static int ip6313_resume(struct i2c_client *client)
{
	return 0;
}
#endif

static const struct i2c_device_id ip6313_i2c_id[] = {
	{"ip6313", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, ip6313_i2c_id);

static struct i2c_driver ip6313_i2c_driver = {
	.driver = {
		.name = "ip6313",
		.owner = THIS_MODULE,
	},
	.probe = ip6313_probe,
	.remove = ip6313_remove,
	.shutdown = ip6313_shutdown,
#ifdef CONFIG_PM
	.suspend = ip6313_suspend,
	.resume = ip6313_resume,
#endif
	.id_table = ip6313_i2c_id,
};

static int __init ip6313_init(void)
{
	int ret = -ENODEV;

	ret = i2c_add_driver(&ip6313_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register I2C driver: %d\n", ret);

	return ret;
}
subsys_initcall(ip6313_init);

static void __exit ip6313_exit(void)
{
	i2c_del_driver(&ip6313_i2c_driver);
}
module_exit(ip6313_exit);

MODULE_DESCRIPTION("Injoinic PMU ip6313 driver");
MODULE_LICENSE("Dual BSD/GPL");
