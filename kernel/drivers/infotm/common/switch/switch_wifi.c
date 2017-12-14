/*
 *modify by michael base on switch_gpio.c 12/24/2013
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <mach/pad.h>
#include <mach/items.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>

struct wifi_switch_data {
	unsigned gpio_power;
	unsigned gpio_reset;
	struct regulator *regulator;
};
static struct wifi_switch_data *switch_data;

//#define FIX_EVB_WIFI_POWER_ERR // added by linyun.xiong for fix x15_evb wifi power control error @2015-03-08

void wifi_power(int power)
{
printk("==wifi_power======%d\n",power);
	if (power == 1) {
		if (switch_data->gpio_reset) {
//			imapx_pad_set_mode(1, 1, switch_data->gpio_reset);
//			imapx_pad_set_dir(0, 1, switch_data->gpio_reset);
//			imapx_pad_set_outdat(0, 1, switch_data->gpio_reset);
			gpio_set_value(switch_data->gpio_reset, 0);
//			msleep(10);
		}

		if (switch_data->regulator) {
			regulator_enable(switch_data->regulator);
		} else if (switch_data->gpio_power) {
//			imapx_pad_set_mode(1, 1, switch_data->gpio_power);
//			imapx_pad_set_dir(0, 1, switch_data->gpio_power);
			if (item_integer("wifi.power", 1) >= 0)
#ifdef FIX_EVB_WIFI_POWER_ERR
				gpio_set_value(switch_data->gpio_power, !power);
#else
				gpio_set_value(switch_data->gpio_power, power);
#endif
//				imapx_pad_set_outdat(power, 1, switch_data->gpio_power);
			else
				gpio_set_value(switch_data->gpio_power, !power);
//				imapx_pad_set_outdat(!power, 1, switch_data->gpio_power);
		}
//		msleep(3);

		if (switch_data->gpio_reset) {
//			imapx_pad_set_outdat(1, 1, switch_data->gpio_reset);
			gpio_set_value(switch_data->gpio_reset, 1);
//			msleep(12);
		}
	} else {
		if (switch_data->regulator) {
			regulator_disable(switch_data->regulator);
		} else if (switch_data->gpio_power) {
			if (item_integer("wifi.power", 1) >= 0)
#ifdef FIX_EVB_WIFI_POWER_ERR
				gpio_set_value(switch_data->gpio_power, !power);
#else
				gpio_set_value(switch_data->gpio_power, power);
#endif
//				imapx_pad_set_outdat(power, 1, switch_data->gpio_power);
			else
				gpio_set_value(switch_data->gpio_power, !power);
//				imapx_pad_set_outdat(!power, 1, switch_data->gpio_power);
		}

		if (switch_data->gpio_reset)
			gpio_set_value(switch_data->gpio_reset, 0);
//			imapx_pad_set_outdat(0, 1, switch_data->gpio_reset);
	}
}
EXPORT_SYMBOL(wifi_power);

static int wifi_switch_probe(struct platform_device *pdev)
{
	char buf[ITEM_MAX_LEN];
	char pmu_channel[ITEM_MAX_LEN];
	int gpio_index;
	int ret = 0;

	switch_data = kzalloc(sizeof(struct wifi_switch_data), GFP_KERNEL);
	if (!switch_data)
		return -ENOMEM;

	if (item_exist("wifi.power")) {
		item_string(buf, "wifi.power", 0);
		if (!strncmp(buf, "pads", 4)) {
			gpio_index = item_integer("wifi.power", 1);
			gpio_index = (gpio_index < 0) ? (-gpio_index) : gpio_index;
//			imapx_pad_set_mode(1, 1, gpio_index);
//			imapx_pad_set_dir(0, 1, gpio_index);
//			imapx_pad_get_indat(gpio_index);
			switch_data->gpio_power = gpio_index;

			if (gpio_is_valid(switch_data->gpio_power)) {
				ret = gpio_request(switch_data->gpio_power, "wifi_power");
				if (ret) {
					pr_err("failed request gpio for wifi_power\n");
				//	return -1;;
				}
			}
			gpio_direction_output(switch_data->gpio_power, (item_integer("wifi.power", 1) > 0) ? 0 : 1);
		} else if (!strncmp(buf, "pmu", 3)) {
			item_string(pmu_channel, "wifi.power", 1);
			switch_data->regulator = regulator_get(&pdev->dev, pmu_channel);
			if (IS_ERR(switch_data->regulator)) {
				return -1;
			}
			regulator_set_voltage(switch_data->regulator, 3300000, 3300000);
		}
		//wifi_power(1);
		//wifi_power(0);
	}

	if (item_exist("wifi.reset")) {
		item_string(buf, "wifi.reset", 0);
		if (!strncmp(buf, "pads", 4)) {
			gpio_index = item_integer("wifi.reset", 1);
			gpio_index = (gpio_index < 0) ? (-gpio_index) : gpio_index;
//			imapx_pad_set_mode(1, 1, gpio_index);
//			imapx_pad_set_dir(0, 1, gpio_index);
//			imapx_pad_get_indat(gpio_index);
			switch_data->gpio_reset = gpio_index;

			if (gpio_is_valid(switch_data->gpio_reset)) {
				ret = gpio_request(switch_data->gpio_reset, "wifi_reset");
				if (ret) {
					pr_err("failed reuqest gpio for wifi_reset\n");
				//	return -1;
				}
			}
			gpio_direction_output(switch_data->gpio_reset, 0);
		} else
			printk("wifi.reset is congiged to wrong type\n");
		}

	return 0;
}

static int wifi_switch_remove(struct platform_device *pdev)
{
	kfree(switch_data);
	return 0;
}

static struct platform_driver wifi_switch_driver = {
	.probe		= wifi_switch_probe,
	.remove		= wifi_switch_remove,
	.driver		= {
		.name	= "switch-wifi",
		.owner	= THIS_MODULE,
	},
};

static int __init wifi_switch_init(void)
{
	return platform_driver_register(&wifi_switch_driver);
}

static void __exit wifi_switch_exit(void)
{
	platform_driver_unregister(&wifi_switch_driver);
}

module_init(wifi_switch_init);
module_exit(wifi_switch_exit);

MODULE_AUTHOR("Bob.yang <Bob.yang@infotmic.com.cn>");
MODULE_DESCRIPTION("WIFI Switch driver");
MODULE_LICENSE("GPL");
