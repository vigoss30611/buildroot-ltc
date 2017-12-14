/*
* driver/input/misc/ricoh618-pwrkey.c
*
* Power Key driver for RICOH R5T618 power management chip.
*
* Copyright (C) 2012-2013 RICOH COMPANY,LTD
*
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <mach/ricoh618.h>
#include <asm/io.h>
#include <mach/imap-rtc.h>

#define RICOH61x_ONKEY_TRIGGER_LEVEL	0
#define RICOH61x_ONKEY_OFF_IRQ		0

struct ricoh61x_pwrkey {
	struct device *dev;
	struct input_dev *pwr;
	#if RICOH61x_ONKEY_TRIGGER_LEVEL
		struct timer_list timer;
	#endif
	struct workqueue_struct *workqueue;
	struct work_struct work;
	unsigned long delay;
	int key_irq;
	bool pressed_first;
	struct ricoh618_pwrkey_platform_data *pdata;
	spinlock_t lock;
};

struct ricoh61x_pwrkey *g_pwrkey;

#if RICOH61x_ONKEY_TRIGGER_LEVEL
void ricoh61x_pwrkey_timer(unsigned long t)
{
	queue_work(g_pwrkey->workqueue, &g_pwrkey->work);
}
#endif

void ricoh61x_pwrkey_emu(void)
{
	input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 1);
	input_event(g_pwrkey->pwr, EV_SYN, 0, 0);
	input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 0);
	input_event(g_pwrkey->pwr, EV_SYN, 0, 0);

	return;
}
EXPORT_SYMBOL(ricoh61x_pwrkey_emu);

static void ricoh61x_irq_work(struct work_struct *work)
{
	/* unsigned long flags; */
	uint8_t val;
    void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);
	
    RICOH_DBG("PMU: %s:\n", __func__);
	/* spin_lock_irqsave(&g_pwrkey->lock, flags); */

	if (pwrkey_wakeup) {
		if (readl(rtc_base + SYS_INT_ST) & 0x80) {
			RICOH_ERR("PMU: %s: rtc_wakeup\n", __func__);
			/* clr rtc wake up interrupt */
			val = readl(rtc_base + SYS_INT_CLR);
			val |= 0x80;
			writel(val, rtc_base + SYS_INT_CLR);
			/* disable rtc powerup */
			val = readl(rtc_base + SYS_ALARM_WEN);
			val &= 0x3;
			writel(val, rtc_base + SYS_ALARM_WEN);
		} else {
			RICOH_ERR("PMU: %s: pwrkey_wakeup\n", __func__);
			input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 1);
			input_event(g_pwrkey->pwr, EV_SYN, 0, 0);
			input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 0);
			input_event(g_pwrkey->pwr, EV_SYN, 0, 0);
		}

		pwrkey_wakeup = 0;

		goto exit;
	}

	ricoh61x_read(g_pwrkey->dev->parent, RICOH61x_INT_MON_SYS, &val);
	dev_dbg(g_pwrkey->dev, "pwrkey is pressed?(0x%x): 0x%x\n",
						RICOH61x_INT_MON_SYS, val);
	RICOH_DBG("PMU: %s: val=0x%x\n", __func__, val);
	if (readl(rtc_base + SYS_INT_ST) & 0x80)
		RICOH_ERR("@_@		rtc wakeup interrupt set\n");

	val &= 0x1;
	if (val) {
		#if (RICOH61x_ONKEY_TRIGGER_LEVEL)
		g_pwrkey->timer.expires = jiffies + g_pwrkey->delay;
		dd_timer(&g_pwrkey->timer);
		#endif
		if (!g_pwrkey->pressed_first) {
			g_pwrkey->pressed_first = true;
			RICOH_ERR("%s: Power Key press!!!\n", __func__);
			/* input_report_key(g_pwrkey->pwr, KEY_POWER, 1); */
			/* input_sync(g_pwrkey->pwr); */
			input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 1);
			input_event(g_pwrkey->pwr, EV_SYN, 0, 0);
		}
	} else {
		if (g_pwrkey->pressed_first) {
			RICOH_ERR("%s: Power Key release!!!\n", __func__);
			/* input_report_key(g_pwrkey->pwr, KEY_POWER, 0); */
			/* input_sync(g_pwrkey->pwr); */
			input_event(g_pwrkey->pwr, EV_KEY, KEY_POWER, 0);
			input_event(g_pwrkey->pwr, EV_SYN, 0, 0);
		}
		g_pwrkey->pressed_first = false;
	}

	/* spin_unlock_irqrestore(&g_pwrkey->lock, flags); */

exit:
	rtc_gpio_irq_enable(1, 1);
}

static irqreturn_t pwrkey_irq(int irq, void *_pwrkey)
{
	RICOH_DBG("PMU: %s:\n", __func__);

	rtc_gpio_irq_enable(1, 0);
	rtc_gpio_irq_clr(1);

	#if (RICOH61x_ONKEY_TRIGGER_LEVEL)
	g_pwrkey->timer.expires = jiffies + g_pwrkey->delay;
	add_timer(&g_pwrkey->timer);
	#else
	queue_work(g_pwrkey->workqueue, &g_pwrkey->work);
	#endif
	return IRQ_HANDLED;
}

#if RICOH61x_ONKEY_OFF_IRQ
static irqreturn_t pwrkey_irq_off(int irq, void *_pwrkey)
{
	dev_warn(g_pwrkey->dev, "ONKEY is pressed long time!\n");
	return IRQ_HANDLED;
}
#endif

static int ricoh61x_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *pwr;
	int key_irq;
	int err;
	struct ricoh61x_pwrkey *pwrkey;
	struct ricoh618_pwrkey_platform_data *pdata = pdev->dev.platform_data;
	uint8_t val;

	RICOH_ERR("PMU: %s:\n", __func__);

	if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}
	key_irq = (pdata->irq + RICOH61x_IRQ_POWER_ON);
	RICOH_INFO("PMU1: %s: key_irq=%d\n", __func__, key_irq);
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey)
		return -ENOMEM;

	pwrkey->dev = &pdev->dev;
	pwrkey->pdata = pdata;
	pwrkey->pressed_first = false;
	pwrkey->delay = HZ / 1000 * pdata->delay_ms;
	//g_pwrkey = pwrkey;
	pwr = input_allocate_device();
	if (!pwr) {
		dev_dbg(&pdev->dev, "Can't allocate power button\n");
		err = -ENOMEM;
		goto free_pwrkey;
	}
	input_set_capability(pwr, EV_KEY, KEY_POWER);
	pwr->name = "ricoh61x_pwrkey";
	pwr->phys = "ricoh61x_pwrkey/input0";
	pwr->dev.parent = &pdev->dev;

	#if RICOH61x_ONKEY_TRIGGER_LEVEL
	init_timer(&pwrkey->timer);
	pwrkey->timer.function = ricoh61x_pwrkey_timer;
	#endif

	spin_lock_init(&pwrkey->lock);
	err = input_register_device(pwr);
	if (err) {
		dev_dbg(&pdev->dev, "Can't register power key: %d\n", err);
		goto free_input_dev;
	}
	pwrkey->key_irq = key_irq;
	pwrkey->pwr = pwr;
	platform_set_drvdata(pdev, pwrkey);

	/* Check if power-key is pressed at boot up */
	err = ricoh61x_read(pwrkey->dev->parent, RICOH61x_INT_MON_SYS, &val);
	if (err < 0) {
		dev_err(&pdev->dev, "Key-press status at boot failed rc=%d\n",
									 err);
		goto unreg_input_dev;
	}
	val &= 0x1;
	if (val) {
		input_report_key(pwrkey->pwr, KEY_POWER, 1);
		RICOH_DBG("******KEY_POWER:1\n");
		input_sync(pwrkey->pwr);
		pwrkey->pressed_first = true;
	}

	#if !(RICOH61x_ONKEY_TRIGGER_LEVEL)
		/* trigger both edge */
		ricoh61x_set_bits(pwrkey->dev->parent, RICOH61x_PWR_IRSEL, 0x1);
	#endif

		/*david*/
	err = request_threaded_irq(key_irq, NULL, pwrkey_irq,
					IRQF_SHARED|IRQF_DISABLED|IRQF_ONESHOT,
					"ricoh61x_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
								key_irq, err);
		goto unreg_input_dev;
	}

	#if RICOH61x_ONKEY_OFF_IRQ
	err = request_threaded_irq(key_irq + RICOH61x_IRQ_ONKEY_OFF,
					NULL, pwrkey_irq_off,
					IRQF_ONESHOT|IRQF_DISABLED|IRQF_SHARED,
					"ricoh61x_pwrkey_off", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
			key_irq + RICOH61x_IRQ_ONKEY_OFF, err);
		free_irq(key_irq, pwrkey);
		goto unreg_input_dev;
	}
	#endif

	pwrkey->workqueue = create_singlethread_workqueue("ricoh61x_pwrkey");
	INIT_WORK(&pwrkey->work, ricoh61x_irq_work);
	g_pwrkey = pwrkey;

	/* Enable power key IRQ */
	/* trigger both edge */
	ricoh61x_set_bits(pwrkey->dev->parent, RICOH61x_PWR_IRSEL, 0x1);
	/* Enable system interrupt */
	ricoh61x_set_bits(pwrkey->dev->parent, RICOH61x_INTC_INTEN, 0x1);
	/* Enable power-on interrupt */
	ricoh61x_set_bits(pwrkey->dev->parent, RICOH61x_INT_EN_SYS, 0x1);
	return 0;

unreg_input_dev:
	input_unregister_device(pwr);
	pwr = NULL;

free_input_dev:
	input_free_device(pwr);
free_pwrkey:
	kfree(pwrkey);

	RICOH_ERR("%s: error !\n", __func__);
	return err;
}

static int ricoh61x_pwrkey_remove(struct platform_device *pdev)
{
	struct ricoh61x_pwrkey *pwrkey = platform_get_drvdata(pdev);

	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	free_irq(pwrkey->key_irq, pwrkey);
	input_unregister_device(pwrkey->pwr);
	kfree(pwrkey);

	return 0;
}

static void ricoh61x_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ricoh61x_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->key_irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
}

#ifdef CONFIG_PM
static int ricoh61x_pwrkey_suspend(struct device *dev)
{
	struct ricoh61x_pwrkey *info = dev_get_drvdata(dev);

	RICOH_DBG("PMU: %s\n", __func__);

	if (info->key_irq)
		disable_irq(info->key_irq);
	cancel_work_sync(&info->work);
	flush_workqueue(info->workqueue);

	return 0;
}

static int ricoh61x_pwrkey_resume(struct device *dev)
{
	struct ricoh61x_pwrkey *info = dev_get_drvdata(dev);

	RICOH_DBG("PMU: %s\n", __func__);
	queue_work(info->workqueue, &info->work);
	if (info->key_irq)
		enable_irq(info->key_irq);

	return 0;
}

static const struct dev_pm_ops ricoh61x_pwrkey_pm_ops = {
	.suspend	= ricoh61x_pwrkey_suspend,
	.resume		= ricoh61x_pwrkey_resume,
};
#endif

static struct platform_driver ricoh61x_pwrkey_driver = {
	.probe	= ricoh61x_pwrkey_probe,
	.remove	= ricoh61x_pwrkey_remove,
	.shutdown = ricoh61x_pwrkey_shutdown,
	.driver	= {
		.name	= "ricoh618-pwrkey",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ricoh61x_pwrkey_pm_ops,
#endif
	},
};

static int __init ricoh61x_pwrkey_init(void)
{
	RICOH_INFO("%s\n", __func__);
	return platform_driver_register(&ricoh61x_pwrkey_driver);
}
module_init(ricoh61x_pwrkey_init);

static void __exit ricoh61x_pwrkey_exit(void)
{
	platform_driver_unregister(&ricoh61x_pwrkey_driver);
}
module_exit(ricoh61x_pwrkey_exit);


MODULE_DESCRIPTION("RICOH R5T618 Power Key driver");
MODULE_ALIAS("platform:ricoh618-pwrkey");
MODULE_LICENSE("GPL v2");
