/* drivers/leds/leds-apollo.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/leds.h>
#include <mach/items.h>

struct apollo_led {
	struct led_classdev      cdev;
	int brightness;
	int led_index;
	int led_gpio;
	int polarity;
};

static void apollo_led_set(struct led_classdev *led_cdev,
				enum led_brightness value)
{
	struct apollo_led *led = container_of(led_cdev, struct apollo_led, cdev);
	int state = ((value ? 0 : 1) ^ (led->polarity));

	gpio_set_value(led->led_gpio, state);
}

static int apollo_led_remove(struct platform_device *dev)
{
	struct apollo_led *led = platform_get_drvdata(dev);

	led_classdev_unregister(&led->cdev);

	return 0;
}

static int apollo_led_probe(struct platform_device *dev)
{
	struct led_platform_data *pdata = dev->dev.platform_data;
	struct apollo_led *led;
	int i, ret;
	const char pads[32];
	const char polarity[32];

	if (pdata == NULL) {
		dev_err(&dev->dev, "No platform data for LED\n");
		return -ENODEV;
	}

	led = devm_kzalloc(&dev->dev, sizeof(struct apollo_led) * pdata->num_leds,
			GFP_KERNEL);
	if (led == NULL) {
		dev_err(&dev->dev, "No memory for device\n");
		return -ENOMEM;
	}

	for (i = 0; i < pdata->num_leds; i++) {
		sprintf(pads, "led%d.ctrl", i);
		sprintf(polarity, "led%d.polarity", i);
		if (!(item_exist(pads)) || !(item_exist(polarity))) {
			dev_err(&dev->dev, "no led%d control pad exists\n", i);
			continue;
	//		return -ENODEV;
		} 
		led[i].led_gpio = item_integer(pads, 1);
		led[i].polarity = item_integer(polarity, 0);
		if (gpio_is_valid(led[i].led_gpio)) {
			ret = gpio_request(led[i].led_gpio, pads);
			if (ret) {
				dev_err(&dev->dev, "failed request gpio for led\n");
				gpio_free(led[i].led_gpio);
				continue;
			//	return -ENODEV;
			}
			gpio_direction_output(led[i].led_gpio, 1);
			gpio_set_value(led[i].led_gpio,  1 ^ led[i].polarity);
		}


		led[i].cdev.brightness_set = apollo_led_set;
		led[i].cdev.brightness = LED_OFF;
		led[i].brightness = LED_OFF;
		led[i].cdev.name = pdata->leds[i].name;
		led[i].led_index = pdata->leds[i].flags;
		led[i].cdev.flags |= LED_CORE_SUSPENDRESUME;
		led[i].cdev.default_trigger = "timer";
		led[i].cdev.blink_delay_on = 0;
		led[i].cdev.blink_delay_off = 1;

		ret = led_classdev_register(&dev->dev.parent, &led[i].cdev);
		if (ret < 0) {
			dev_err(&dev->dev, "led_classdev_register failed\n");
			return ret;
		}
	}

	platform_set_drvdata(dev, led);
	return ret;
}

static struct platform_driver apollo_led_driver = {
	.probe		= apollo_led_probe,
	.remove		= apollo_led_remove,
	.driver		= {
		.name		= "apollo_led",
		.owner		= THIS_MODULE,
	},
};

module_platform_driver(apollo_led_driver);

MODULE_DESCRIPTION("APOLLO LED driver");
MODULE_LICENSE("GPL");
