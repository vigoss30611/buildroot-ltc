/*
 * LEDs driver for GPIOs
 *
 * Copyright (C) 2007 8D Technologies inc.
 * Raphael Assenat <raph@8d.com>
 * Copyright (C) 2008 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <linux/pinctrl/consumer.h>
#include <linux/err.h>

struct gpio_led_data {
	struct led_classdev cdev;
	unsigned gpio;
	struct work_struct work;
	u8 new_level;
	u8 can_sleep;
	u8 active_low;
	u8 default_state;
	u8 blinking;
	int (*platform_gpio_blink_set)(unsigned gpio, int state,
			unsigned long *delay_on, unsigned long *delay_off);
};

static void gpio_led_work(struct work_struct *work)
{
	struct gpio_led_data	*led_dat =
		container_of(work, struct gpio_led_data, work);

	if (led_dat->blinking) {
		led_dat->platform_gpio_blink_set(led_dat->gpio,
						 led_dat->new_level,
						 NULL, NULL);
		led_dat->blinking = 0;
	} else
		gpio_set_value_cansleep(led_dat->gpio, led_dat->new_level);
}

static void gpio_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	struct gpio_led_data *led_dat =
		container_of(led_cdev, struct gpio_led_data, cdev);
	int level;

	if (value == LED_OFF)
		level = 0;
	else
		level = 1;

	if (led_dat->active_low)
		level = !level;

	if (led_dat->default_state==3)
		gpio_direction_output(led_dat->gpio, level);
	else {

		if (led_dat->can_sleep) {
			led_dat->new_level = level;
			schedule_work(&led_dat->work);
		} else {
			if (led_dat->blinking) {
				led_dat->platform_gpio_blink_set(led_dat->gpio, level,
								 NULL, NULL);
				led_dat->blinking = 0;
			} else
				gpio_set_value(led_dat->gpio, level);
		}
	}
}
 
static void delete_gpio_led(struct gpio_led_data *led)
{
	if (!gpio_is_valid(led->gpio))
		return;
	led_classdev_unregister(&led->cdev);
	cancel_work_sync(&led->work);
}

struct gpio_leds_priv {
	int num_leds;
	struct gpio_led_data leds[];
};

static inline int sizeof_gpio_leds_priv(int num_leds)
{
	return sizeof(struct gpio_leds_priv) +
		(sizeof(struct gpio_led_data) * num_leds);
}

/* Code to create from OpenFirmware platform devices */
#ifdef CONFIG_OF_GPIO
static struct gpio_leds_priv *gpio_leds_create_of(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	struct gpio_leds_priv *priv;
	int count, ret;

	/* count LEDs in this device, so we know how much to allocate */
	count = of_get_child_count(np);
	if (!count)
		return ERR_PTR(-ENODEV);

	for_each_child_of_node(np, child)
		if (of_get_gpio(child, 0) == -EPROBE_DEFER)
			return ERR_PTR(-EPROBE_DEFER);

	priv = devm_kzalloc(&pdev->dev, sizeof_gpio_leds_priv(count),
			GFP_KERNEL);
	if (!priv)
		return ERR_PTR(-ENOMEM);

	for_each_child_of_node(np, child) {
		struct gpio_led led = {};
		enum of_gpio_flags flags;
		const char *state;

		led.gpio = of_get_gpio_flags(child, 0, &flags);
		led.active_low = flags & OF_GPIO_ACTIVE_LOW;
		led.name = of_get_property(child, "label", NULL) ? : child->name;
		led.default_trigger =
			of_get_property(child, "linux,default-trigger", NULL);
		state = of_get_property(child, "default-state", NULL);
		if (state) {
			if (!strcmp(state, "keep"))
				led.default_state = LEDS_GPIO_DEFSTATE_KEEP;
			else if (!strcmp(state, "on"))
				led.default_state = LEDS_GPIO_DEFSTATE_ON;
			else
				led.default_state = LEDS_GPIO_DEFSTATE_OFF;
		}

		ret = create_gpio_led(&led, &priv->leds[priv->num_leds++],
				      &pdev->dev, NULL);
		if (ret < 0) {
			of_node_put(child);
			goto err;
		}
	}

	return priv;

err:
	for (count = priv->num_leds - 2; count >= 0; count--)
		delete_gpio_led(&priv->leds[count]);
	return ERR_PTR(-ENODEV);
}

static const struct of_device_id of_gpio_leds_match[] = {
	{ .compatible = "imap_led", },
	{},
};
#else /* CONFIG_OF_GPIO */
static struct gpio_leds_priv *gpio_leds_create_of(struct platform_device *pdev)
{
	return ERR_PTR(-ENODEV);
}
#endif /* CONFIG_OF_GPIO */


static int gpio_led_probe(struct platform_device *pdev)
{
	struct gpio_led_platform_data *pdata = pdev->dev.platform_data;
	struct gpio_leds_priv *priv;
	int i, ret = 0;
 
	if (pdata && pdata->num_leds) {
		
 		priv = devm_kzalloc(&pdev->dev,
				    sizeof_gpio_leds_priv(pdata->num_leds),
				    GFP_KERNEL);
		if (!priv)
			return -ENOMEM;

		for (i = 0; i < pdata->num_leds; i++) {
			
 			struct gpio_led *cur_led = &pdata->leds[i];
			struct gpio_led_data *led_dat = &priv->leds[i];
 
			if (gpio_is_valid(pdata->leds[i].gpio)) {
				ret = gpio_request(pdata->leds[i].gpio, cur_led->name);
				if (ret) {
  					pr_err("failed request gpio for %s\n",cur_led->name);
					gpio_free(pdata->leds[i].gpio);
					continue;
				}
			}
			
 			led_dat->cdev.name = cur_led->name;
			led_dat->cdev.default_trigger = "timer";
			led_dat->gpio = cur_led->gpio;
			led_dat->can_sleep = gpio_cansleep(cur_led->gpio);
			led_dat->active_low = cur_led->active_low;
			led_dat->cdev.blink_delay_on = 0;
			led_dat->cdev.blink_delay_off = 1;
			led_dat->blinking = 0;
			led_dat->cdev.brightness_set = gpio_led_set;
			led_dat->cdev.flags |= LED_CORE_SUSPENDRESUME;
			led_dat->default_state = cur_led->default_state;
			if (cur_led->default_state != 3) {
				if (cur_led->default_state == 0)
					led_dat->cdev.brightness = LED_OFF;
				else if (cur_led->default_state == 1)
					led_dat->cdev.brightness = LED_FULL;
				else if (cur_led->default_state == 2){
					led_dat->cdev.blink_delay_on = cur_led->delay_on;
					led_dat->cdev.blink_delay_off = cur_led->delay_off;
					}
				else
					led_dat->cdev.brightness = LED_OFF;
				
	 			ret = gpio_direction_output(led_dat->gpio, led_dat->cdev.brightness);
				if (ret < 0)
					return ret;
				}
			
			INIT_WORK(&led_dat->work, gpio_led_work);
			
			ret = led_classdev_register(&pdev->dev, &led_dat->cdev);
			if (ret < 0)
				goto err;
		}
		priv->num_leds = pdata->num_leds;
	} else {
		priv = gpio_leds_create_of(pdev);
		if (!priv)
			return -ENODEV;
	}

	platform_set_drvdata(pdev, priv);

	return 0;

err:
	while (i--)
		led_classdev_unregister(&priv->leds[i].cdev);

	return ret;
}

static int gpio_led_remove(struct platform_device *pdev)
{
	struct gpio_leds_priv *priv = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < priv->num_leds; i++)
		delete_gpio_led(&priv->leds[i]);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver gpio_led_driver = {
	.probe		= gpio_led_probe,
	.remove		= gpio_led_remove,
	.driver		= {
		.name	= "imap_led",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_gpio_leds_match),
	},
};

module_platform_driver(gpio_led_driver);

MODULE_DESCRIPTION("GPIO LED driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:leds-gpio");
