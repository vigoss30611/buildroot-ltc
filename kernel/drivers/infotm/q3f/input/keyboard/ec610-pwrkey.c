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
#include <asm/io.h>
#include <mach/imap-rtc.h>
#include <mach/ec610.h>

struct ec610_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct workqueue_struct *workqueue;
	struct work_struct work;
	bool pressed_first;
	int irq;
};

static int ec610_pwrkey_irq_enable(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x5c);
	if (en)
		val |= 1 << 1;
	else
		val &= ~(1 << 1);
	writel(val, rtc_base + 0x5c);

	return 0;
}

static int ec610_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 4))
		writel((1 << 4), rtc_base + 0x4);
	return 0;
}

static int ec610_pwrkey_set_dir(int dir)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x4c);
	if (dir)
		val |= 1 << 1;
	else
		val &= ~(1 << 1);
	writel(val, rtc_base + 0x4c);

	return 0;
}

static int ec610_pwrkey_set_pulldown(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x54);
	if (en)
		val |= 1 << 1;
	else
		val &= ~(1 << 1);
	writel(val, rtc_base + 0x54);

	return 0;
}

static int ec610_pwrkey_set_polarity(int polar)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x5c);
	if (polar)
		val |= 1 << 5;
	else
		val &= ~(1 << 5);
	writel(val, rtc_base + 0x5c);

	return 0;
}

static void ec610_pwrkey_work(struct work_struct *work)
{
	int ret;
	struct ec610_pwrkey *pwrkey =
			container_of(work, struct ec610_pwrkey, work);
	uint8_t status = 0;
	uint8_t key_press;
	uint8_t key_shortpress;
	uint8_t key_longpress;
	uint8_t key_changed;

	ret = ec610_read(pwrkey->dev->parent, EC610_INTS_FLAG0, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in reading reg 0x%02x, error: %d\n",
				EC610_INTS_FLAG0, ret);
		return;
	}
	ec610_write(pwrkey->dev->parent, EC610_INTS_FLAG0, status);

	key_changed = status & 0x01;
	key_press = status & 0x20;
	key_shortpress = status & 0x02;
	key_longpress = status & 0x04;

	if (key_changed && key_press) {
		EC610_INFO("power key down\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
		input_sync(pwrkey->input);
	} else if (key_changed && !key_press) {
		EC610_INFO("power key up\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(pwrkey->input);
	}

	ec610_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t ec610_pwrkey_isr(int irq, void *data)
{
	struct ec610_pwrkey *pwrkey = data;
	EC610_DBG("ec610_pwrkey_isr: \n");

	/*TODO: clear irq*/
	ec610_pwrkey_irq_enable(0);
	ec610_pwrkey_irq_clr();

	queue_work(pwrkey->workqueue, &pwrkey->work);

	return IRQ_HANDLED;
}

static int ec610_pwrkey_hw_init(void)
{
	/* disable rtc gpio1 irq */
	ec610_pwrkey_irq_enable(0);
	/* set rtc gpio1 to input */
	ec610_pwrkey_set_dir(1);
	/* rtc gpio1 pulldown disable */
	ec610_pwrkey_set_pulldown(0);
	/* high valid */
	ec610_pwrkey_set_polarity(0);
	/* enable rtc gpio1 irq */
	ec610_pwrkey_irq_enable(1);

	return 0;
}

static int ec610_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct ec610_pwrkey *pwrkey;
	/*struct ec610_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;
	uint8_t val;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate ec610_pwrkey\n");
		return -ENOMEM;
	}

	pwrkey->irq = EC610_PMIC_IRQ;
	pwrkey->dev = &pdev->dev;
	/*pwrkey->pdata = pdata;*/
	pwrkey->pressed_first = false;

	input = input_allocate_device();
	if (!input) {
		dev_err(&pdev->dev, "Can't allocate input device\n");
		err = -ENOMEM;
		goto free_pwrkey;
	}

	input_set_capability(input, EV_KEY, KEY_POWER);
	input->name = "ec610_pwrkey";
	input->phys = "ec610_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);

	pwrkey->workqueue = create_singlethread_workqueue("ec610_pwrkey_workqueue");
	INIT_WORK(&pwrkey->work, ec610_pwrkey_work);

	/* irq output enable */
	ec610_read(pwrkey->dev->parent, EC610_MFP_CTL5, &val);
	val &= 0xE0;
	val |= EC610_IRQ_OUT|0x10;
	ec610_write(pwrkey->dev->parent, EC610_MFP_CTL5, val);

	/* clear flags */
	val = 0xff;
	ec610_write(pwrkey->dev->parent, EC610_INTS_FLAG0, val);
	ec610_write(pwrkey->dev->parent, EC610_INTS_FLAG1, val);

	/* enable irq */
	val = DEFAULT_INT_MASK;
	ec610_write(pwrkey->dev->parent, EC610_INTS_CTL0, val);
	val = 0x00;
	ec610_write(pwrkey->dev->parent, EC610_INTS_CTL1, val);

	err = request_threaded_irq(pwrkey->irq, NULL, ec610_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"ec610_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	ec610_pwrkey_hw_init();

	return 0;

unregister_input_dev:
	input_unregister_device(input);
	input = NULL;
free_input_dev:
	input_free_device(input);
free_pwrkey:
	kfree(pwrkey);

	return err;
}

static int ec610_pwrkey_remove(struct platform_device *pdev)
{
	struct ec610_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void ec610_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ec610_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int ec610_pwrkey_suspend(struct device *dev)
{
	struct ec610_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_work_sync(&pwrkey->work);
	flush_workqueue(pwrkey->workqueue);

	return 0;
}

static int ec610_pwrkey_resume(struct device *dev)
{
	uint8_t val;
	struct ec610_pwrkey *pwrkey = dev_get_drvdata(dev);

	/* irq output enable */
	ec610_read(pwrkey->dev->parent, EC610_MFP_CTL5, &val);
	val &= 0xE0;
	val |= EC610_IRQ_OUT|0x10;
	ec610_write(pwrkey->dev->parent, EC610_MFP_CTL5, val);

	/* clear flags */
	val = 0xff;
	ec610_write(pwrkey->dev->parent, EC610_INTS_FLAG0, val);
	ec610_write(pwrkey->dev->parent, EC610_INTS_FLAG1, val);

	/* enable irq */
	val = DEFAULT_INT_MASK;
	ec610_write(pwrkey->dev->parent, EC610_INTS_CTL0, val);
	val = 0x00;
	ec610_write(pwrkey->dev->parent, EC610_INTS_CTL1, val);

	queue_work(pwrkey->workqueue, &pwrkey->work);
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops ec610_pwrkey_pm_ops = {
	.suspend        = ec610_pwrkey_suspend,
	.resume         = ec610_pwrkey_resume,
};
#endif

static struct platform_driver ec610_pwrkey_driver = {
	.probe  = ec610_pwrkey_probe,
	.remove = ec610_pwrkey_remove,
	.shutdown = ec610_pwrkey_shutdown,
	.driver = {
		.name   = "ec610-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &ec610_pwrkey_pm_ops,
#endif
	},
};

static int __init ec610_pwrkey_init(void)
{
	return platform_driver_register(&ec610_pwrkey_driver);
}
module_init(ec610_pwrkey_init);

static void __exit ec610_pwrkey_exit(void)
{
	platform_driver_unregister(&ec610_pwrkey_driver);
}
module_exit(ec610_pwrkey_exit);

MODULE_DESCRIPTION("EC610 Power Key driver");
MODULE_ALIAS("platform:ec610-pwrkey");
MODULE_LICENSE("GPL v2");
