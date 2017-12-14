/* display chip driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/display_device.h>
#include <mach/items.h>

#include "display_register.h"
#include "display_items.h"

#define DISPLAY_MODULE_LOG		"chip"


/* @_@ extern specific chip parameter */
#ifdef CONFIG_SND_SOC_IP2906
extern struct peripheral_param_config CVBS_IP2906;
#endif

/* ^_^ Add new chip parameter here */
static struct peripheral_param_config * chip_configs[] = {
#ifdef CONFIG_SND_SOC_IP2906
	&CVBS_IP2906,
#endif
	NULL,
};

/* ^_^ Add new chip i2c id here */
static const struct i2c_device_id chip_i2c_id[] = {
#ifdef CONFIG_SND_SOC_IP2906
	{ "ip2906_cvbs", 0},
#endif
	{}
};
MODULE_DEVICE_TABLE(i2c, chip_i2c_id);


static int chip_enable(struct display_device *pdev, int en)
{
	int ret;

	pdev->enable = en;

	if (pdev->peripheral_config.control_interface != DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE) {
		if (pdev->irq && !en)
			disable_irq(pdev->irq);

		ret = pdev->peripheral_config.enable(pdev, en);
		if (ret) {
			dlog_err("peripheral_config.enable(%d) failed %d\n", en, ret);
			return ret;
		}

		if (pdev->irq && en)
			enable_irq(pdev->irq);
	}

	return 0;
}

static int chip_set_vic(struct display_device *pdev, int vic)
{
	dlog_dbg("vic=%d\n", vic);

	pdev->vic = vic;

	if (pdev->peripheral_config.control_interface != DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE)
		return pdev->peripheral_config.set_vic(pdev, vic);

	return 0;
}

static irqreturn_t chip_irq_thread(int irq, void *data)
{
	struct display_device *pdev = data;
	int ret;
	
	dlog_trace();

	if (!pdev->enable) {
		dlog_dbg("ignore irq when disabled\n");
		return IRQ_HANDLED;
	}

	ret = pdev->peripheral_config.irq_handler(pdev);
	if (ret)
		dlog_err("chip irq handler error %d\n", ret);

	return IRQ_HANDLED;
}

static int chip_probe(struct display_device *pdev)
{
	int i, j, ret;
	char item[64] = {0};
	char str[64] = {0};
	unsigned long i2c_bus, irq_gpio;
	unsigned int irq;
	struct i2c_adapter *adap;
	struct i2c_client *client;
	
	dlog_trace();

	/* get chip params */
	sprintf(item, DISPLAY_ITEM_PERIPHERAL_NAME, pdev->path, "chip");
	if (!item_exist(item)) {
		dlog_err("item %s does NOT exist.\n", item);
		return -EINVAL;
	}
	item_string(str, item, 0);
	dlog_info("chip name=%s\n", str);

	/* match */
	for (i = 0; chip_configs[i]; i++) {
		if (sysfs_streq(chip_configs[i]->name, str))
			break;
	}
	if (chip_configs[i] == NULL) {
		dlog_err("chip %s NOT found in chip_configs[]\n", str);
		return -EINVAL;
	}
	memcpy(&pdev->peripheral_config, chip_configs[i], sizeof(struct peripheral_param_config));
	/* init status value for robustness */
	pdev->rgb_order = chip_configs[i]->rgb_order;
	pdev->bt656_pad_map = chip_configs[i]->bt656_pad_map;
	pdev->bt601_yuv_format = chip_configs[i]->bt601_yuv_format;
	pdev->bt601_data_width = chip_configs[i]->bt601_data_width;
	pdev->interface_type = chip_configs[i]->interface_type;
	pdev->vic = 0;

	display_peripheral_resource_init(pdev);

	if (chip_configs[i]->attrs) {
		for (j = 0; chip_configs[i]->attrs[j].attr.name != NULL; j++) {
			sysfs_create_file(&pdev->dev.kobj, &chip_configs[i]->attrs[j].attr);
		}
	}

	if (pdev->peripheral_config.control_interface != DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE) {
		if (!pdev->peripheral_config.enable) {
			dlog_err("when has control interface, (*enable) can NOT leave blank.\n");
			return -EINVAL;
		}
		if (!pdev->fixed_resolution && !pdev->peripheral_config.set_vic) {
			dlog_err("for variable resolution peripheral device, (*set_vic) can NOT leave blank.\n");
			return -EINVAL;
		}
	}

	if (pdev->peripheral_config.control_interface == DISPLAY_PERIPHERAL_CONTROL_INTERFACE_I2C) {
		if (!pdev->peripheral_config.i2c_info.addr) {
			dlog_err("when I2C control interface, i2c_info can NOT leave blank.\n");
			return -EINVAL;
		}
		sprintf(item, DISPLAY_ITEM_PERIPHERAL_I2CBUS, pdev->path, "chip");
		if (!item_exist(item)) {
			dlog_err("item %s does NOT exist.\n", item);
			return -EINVAL;
		}
		item_string(str, item, 0);
		if (kstrtoul(str, 10, &i2c_bus)) {
			dlog_err("item %s can NOT be parsed to integer correctly.\n", str);
			return -EINVAL;
		}
		dlog_dbg("register i2c device, bus=%ld, addr=0x%02X, name=%s\n",
						i2c_bus, pdev->peripheral_config.i2c_info.addr, pdev->peripheral_config.i2c_info.type);
		pdev->peripheral_config.i2c_info.platform_data = pdev;
		adap = i2c_get_adapter(i2c_bus);
		client = i2c_new_device(adap, &pdev->peripheral_config.i2c_info);
		pdev->i2c = client;
		if (client)
			i2c_set_clientdata(client, pdev);
		else
			dlog_info("register i2c device failed, the chip device should handle i2c_client by itself if share use\n");
		
		/* irq should rely on control interface */
		sprintf(item, DISPLAY_ITEM_PERIPHERAL_IRQ, pdev->path, "chip");
		if (item_exist(item)) {
			if (!pdev->peripheral_config.irq_handler) {
				dlog_err("irq handler NOT specify while irq number is given.\n");
				return -EINVAL;
			}
			item_string(str, item, 0);
			if (kstrtoul(str, 10, &irq_gpio)) {
				dlog_err("item %s can NOT be parsed to integer correctly.\n", str);
				return -EINVAL;
			}
			irq = gpio_to_irq(irq_gpio);
			dlog_dbg("request irq, gpio=%ld, irq=%d\n", irq_gpio, irq);
			ret = request_threaded_irq(irq, NULL, chip_irq_thread, 
						IRQF_ONESHOT | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						"display_chip_irq", pdev);
			if (ret) {
				dlog_err("request irq%d(gpio%ld) failed %d\n", irq, irq_gpio, ret);
				return ret;
			}
			disable_irq(irq);
			pdev->irq = irq;
		}
	}
	else if (pdev->peripheral_config.control_interface == DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI) {
		dlog_err("Not support spi control interface for chip.\n");
		return -EINVAL;
	}

	return 0;
}

static int chip_remove(struct display_device *pdev)
{
	dlog_trace();

	display_peripheral_sequential_power(pdev, 0);

	return 0;
}

//=============== i2c ctrl interface ==============

static int chip_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	dlog_trace();

	return 0;
}

static int chip_i2c_remove(struct i2c_client *client)
{
	dlog_trace();

	return 0;
}




static struct i2c_driver chip_i2c_driver = {
	.driver = {
		.name = "chip_i2c",
		.owner = THIS_MODULE,
	},
	.probe = chip_i2c_probe,
	.remove = chip_i2c_remove,
	.id_table = chip_i2c_id,
};

static struct display_driver chip_driver = {
	.probe  = chip_probe,
	.remove = chip_remove,
	.driver = {
		.name = "chip",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_PERIPHERAL,
	.function = {
		.enable = chip_enable,
		.set_vic = chip_set_vic,
	}
};

static struct display_device chip_devices[IDS_PATH_NUM] = {
	{
		.name = "chip",
		.id = -1,
		.path = 0,
		.fixed_resolution = 0,
	},
};


static int __init chip_driver_init(void)
{
	int ret;
	int i, j;
	char name[64] = {0};
	char peripheral[64] = {0};
	
	ret = display_driver_register(&chip_driver);
	if (ret) {
		dlog_err("register chip driver failed %d\n", ret);
		return ret;
	}
	ret = i2c_add_driver(&chip_i2c_driver);
	if (ret) {
		dlog_err("register chip i2c driver failed %d\n", ret);
		return ret;
	}
	
	for (i = 0; i < IDS_PATH_NUM; i++) {
		for (j = 0; j < MAX_PATH_PERIPHERAL_NUM; j++) { 
			sprintf(name, DISPLAY_ITEM_PERIPHERAL_TYPE, i, j);
			if (!item_exist(name))
				break;
			item_string(peripheral, name, 0);
			if (sysfs_streq(peripheral, "chip")) {
				dlog_info("path%d: register peripheral%d device %s\n", i, j, peripheral);
				display_device_register(&chip_devices[i]);
				goto out;
			}
		}
	}
	dlog_info("no chip device\n");

out:
	return 0;
}
module_init(chip_driver_init);

static void __exit chip_driver_exit(void)
{
	i2c_del_driver(&chip_i2c_driver);
	display_driver_unregister(&chip_driver);
}
module_exit(chip_driver_exit);

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display chip peripheral driver");
MODULE_LICENSE("GPL");
