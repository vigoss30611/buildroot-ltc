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

struct q3f_gpio_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct delayed_work power_work;
	bool pressed_first;
	bool keydown;
	int irq;
};

static struct q3f_gpio_pwrkey *g_pwrkey;
static int loop;

/*
 * We use RTCGP0 as wake up source here on Q3F
 */

static int q3f_gpio_pwrkey_wakeup_mask(int en) {
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);
	val = readl(rtc_base + 0x28);
	if (en)
		val |=  1 ;
	else
		val &= ~1;
	writel(val, rtc_base + 0x28);

	return 0;
}

static int q3f_gpio_pwrkey_irq_enable(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x5c);
	if (en)
		val |= 1 ;
	else
		val &= ~1;
	writel(val, rtc_base + 0x5c);

	return 0;
}

static int q3f_gpio_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 3))
		writel((1 << 3), rtc_base + 0x4);
	return 0;
}

static int q3f_gpio_pwrkey_set_dir(int dir)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x4c);
	if (dir)
		val |= 1;
	else
		val &= ~1;
	writel(val, rtc_base + 0x4c);

	return 0;
}

static int q3f_gpio_pwrkey_set_pulldown(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x54);
	if (en)
		val |= 1;
	else
		val &= ~1;
	writel(val, rtc_base + 0x54);

	return 0;
}

static int q3f_gpio_pwrkey_set_polarity(int polar)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x5c);
	if (polar)
		val |= 1 << 4;
	else
		val &= ~(1 << 4);
	writel(val, rtc_base + 0x5c);

	return 0;
}

static void q3f_gpio_pwrkey_work(struct work_struct *work)
{
#if 0
	ret = q3f_gpio_read(pwrkey->dev->parent, EC610_INTS_FLAG0, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in reading reg 0x%02x, error: %d\n",
				EC610_INTS_FLAG0, ret);
		return;
	}
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_FLAG0, status);
#endif
		

	if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x58)) & 0x1) {
		loop++;
		if(g_pwrkey->keydown == false) {
			pr_err("power key down\n");
			g_pwrkey->keydown = true;
			input_event(g_pwrkey->input, EV_KEY, KEY_POWER, 1);
			input_sync(g_pwrkey->input);
		}
	} else {
		pr_err("power key up\n");
		g_pwrkey->keydown = false;
		input_event(g_pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(g_pwrkey->input);
	}
	if(g_pwrkey->keydown == true)
		schedule_delayed_work(&g_pwrkey->power_work, msecs_to_jiffies(10));
	else
		q3f_gpio_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t q3f_gpio_pwrkey_isr(int irq, void *data)
{
	struct q3f_gpio_pwrkey *pwrkey = data;
	pr_err("q3f_gpio_pwrkey_isr: \n");

	/*TODO: clear irq*/
	q3f_gpio_pwrkey_irq_enable(0);
	q3f_gpio_pwrkey_irq_clr();

	//queue_work(pwrkey->workqueue, &pwrkey->work);
	schedule_delayed_work(&g_pwrkey->power_work, msecs_to_jiffies(10));

	return IRQ_HANDLED;
}

static int q3f_gpio_pwrkey_hw_init(void)
{
	/* disable rtc gpio0 irq */
	q3f_gpio_pwrkey_irq_enable(0);
	/* set rtc gpio0 to input */
	q3f_gpio_pwrkey_set_dir(1);
	/* rtc gpio0 pulldown disable */
	q3f_gpio_pwrkey_set_pulldown(0);
	/* high valid */
	q3f_gpio_pwrkey_set_polarity(0);
	/* enable rtc gpio0 irq */
	q3f_gpio_pwrkey_irq_enable(1);

	return 0;
}

static int q3f_gpio_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct q3f_gpio_pwrkey *pwrkey;
	/*struct q3f_gpio_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	pr_err("%s++\n", __func__);
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate q3f_gpio_pwrkey\n");
		return -ENOMEM;
	}
	g_pwrkey = pwrkey;
	pwrkey->irq = GIC_RTCGP0_ID;
	pwrkey->dev = &pdev->dev;
	/*pwrkey->pdata = pdata;*/
	pwrkey->pressed_first = false;
	pwrkey->keydown = false;

	input = input_allocate_device();
	if (!input) {
		dev_err(&pdev->dev, "Can't allocate input device\n");
		err = -ENOMEM;
		goto free_pwrkey;
	}

	input_set_capability(input, EV_KEY, KEY_POWER);
	input->name = "q3f_gpio_pwrkey";
	input->phys = "q3f_gpio_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);
	//INIT_WORK(&pwrkey->work, q3f_gpio_pwrkey_work);
	INIT_DELAYED_WORK(&pwrkey->power_work, q3f_gpio_pwrkey_work);

#if 0
	/* irq output enable */
	q3f_gpio_read(pwrkey->dev->parent, EC610_MFP_CTL5, &val);
	val &= 0xE0;
	val |= EC610_IRQ_OUT|0x10;
	q3f_gpio_write(pwrkey->dev->parent, EC610_MFP_CTL5, val);

	/* clear flags */
	val = 0xff;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_FLAG0, val);
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_FLAG1, val);

	/* enable irq */
	val = DEFAULT_INT_MASK;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_CTL0, val);
	val = 0x00;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_CTL1, val);
#endif

	err = request_threaded_irq(pwrkey->irq, NULL, q3f_gpio_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"q3f_gpio_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	q3f_gpio_pwrkey_hw_init();

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

static int q3f_gpio_pwrkey_remove(struct platform_device *pdev)
{
	struct q3f_gpio_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void q3f_gpio_pwrkey_shutdown(struct platform_device *pdev)
{
	struct q3f_gpio_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int q3f_gpio_pwrkey_suspend(struct device *dev)
{
	struct q3f_gpio_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_delayed_work(&pwrkey->power_work);
	q3f_gpio_pwrkey_wakeup_mask(0);
	

	return 0;
}

static int q3f_gpio_pwrkey_resume(struct device *dev)
{
	struct q3f_gpio_pwrkey *pwrkey = dev_get_drvdata(dev);

#if 0
	/* irq output enable */
	q3f_gpio_read(pwrkey->dev->parent, EC610_MFP_CTL5, &val);
	val &= 0xE0;
	val |= EC610_IRQ_OUT|0x10;
	q3f_gpio_write(pwrkey->dev->parent, EC610_MFP_CTL5, val);

	/* clear flags */
	val = 0xff;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_FLAG0, val);
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_FLAG1, val);

	/* enable irq */
	val = DEFAULT_INT_MASK;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_CTL0, val);
	val = 0x00;
	q3f_gpio_write(pwrkey->dev->parent, EC610_INTS_CTL1, val);
#endif
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops q3f_gpio_pwrkey_pm_ops = {
	.suspend        = q3f_gpio_pwrkey_suspend,
	.resume         = q3f_gpio_pwrkey_resume,
};
#endif

static struct platform_driver q3f_gpio_pwrkey_driver = {
	.probe  = q3f_gpio_pwrkey_probe,
	.remove = q3f_gpio_pwrkey_remove,
	.shutdown = q3f_gpio_pwrkey_shutdown,
	.driver = {
		.name   = "q3f_gpio-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &q3f_gpio_pwrkey_pm_ops,
#endif
	},
};

static int __init q3f_gpio_pwrkey_init(void)
{
	return platform_driver_register(&q3f_gpio_pwrkey_driver);
}
module_init(q3f_gpio_pwrkey_init);

static void __exit q3f_gpio_pwrkey_exit(void)
{
	platform_driver_unregister(&q3f_gpio_pwrkey_driver);
}
module_exit(q3f_gpio_pwrkey_exit);

MODULE_DESCRIPTION("FPGA Power Key driver");
MODULE_ALIAS("platform:FPGA-pwrkey");
MODULE_LICENSE("GPL v2");
