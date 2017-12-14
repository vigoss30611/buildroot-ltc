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
#include <mach/ip6208.h>

struct ip6208_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct workqueue_struct *workqueue;
	struct delayed_work work;
	bool pressed_first;
	int irq;
};

static int ip6208_pwrkey_irq_enable(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x5c);
	if (en)
		val |= 1 << 0;
	else
		val &= ~(1 << 0);
	writel(val, rtc_base + 0x5c);

	return 0;
}

static int ip6208_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 3))
		writel((1 << 3), rtc_base + 0x4);
	return 0;
}

static int ip6208_pwrkey_set_dir(int dir)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x4c);
	if (dir)
		val |= 1 << 0;
	else
		val &= ~(1 << 0);
	writel(val, rtc_base + 0x4c);

	return 0;
}

static int ip6208_pwrkey_set_pulldown(int en)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x54);
	if (en)
		val |= 1 << 0;
	else
		val &= ~(1 << 0);
	writel(val, rtc_base + 0x54);

	return 0;
}

static int ip6208_pwrkey_set_polarity(int polar)
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

static void ip6208_pwrkey_work(struct work_struct *work)
{
	int ret;
	struct ip6208_pwrkey *pwrkey =
			container_of(work, struct ip6208_pwrkey, work);
	uint16_t status = 0;
	uint32_t key_press;
	uint8_t key_shortpress;
	uint8_t key_longpress;
	uint8_t key_us_press;;

	ret = ip620x_read(pwrkey->dev->parent, IP6208_INT_FLAG, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in reading reg 0x%02x, error: %d\n",
				IP6208_INT_FLAG, ret);
		return;
	}
	ip620x_write(pwrkey->dev->parent, IP6208_INT_FLAG, status);

	key_us_press = status & 0x04;
	key_press = (status >> 15) & 0x1;
	key_shortpress = status & 0x01;
	key_longpress = status & 0x02;

	if (key_press) {
		IP620x_INFO("power key down\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
		input_sync(pwrkey->input);
	} else if (!key_press) {
		IP620x_INFO("power key up\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(pwrkey->input);
	}

	ip6208_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t ip6208_pwrkey_isr(int irq, void *data)
{
	struct ip6208_pwrkey *pwrkey = data;
	IP620x_DBG("ip6208_pwrkey_isr: \n");

	/*TODO: clear irq*/
	ip6208_pwrkey_irq_enable(0);
	ip6208_pwrkey_irq_clr();

	queue_delayed_work(pwrkey->workqueue, &pwrkey->work,msecs_to_jiffies(50));

	return IRQ_HANDLED;
}

static int ip6208_pwrkey_hw_init(void)
{
	/* disable rtc gpio0 irq */
	ip6208_pwrkey_irq_enable(0);
	/* set rtc gpio0 to input */
	ip6208_pwrkey_set_dir(1);
	/* rtc gpio0 pulldown disable */
	ip6208_pwrkey_set_pulldown(0);
	/* high valid */
	ip6208_pwrkey_set_polarity(0);
	/* enable rtc gpio0 irq */
	ip6208_pwrkey_irq_enable(1);

	return 0;
}

static int ip6208_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct ip6208_pwrkey *pwrkey;
	/*struct ip6208_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;
	uint16_t val;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate ip6208_pwrkey\n");
		return -ENOMEM;
	}

	pwrkey->irq = IP6208_PMIC_IRQ;
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
	input->name = "ip6208_pwrkey";
	input->phys = "ip6208_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);

	pwrkey->workqueue = create_singlethread_workqueue("ip6208_pwrkey_workqueue");
	INIT_DELAYED_WORK(&pwrkey->work, ip6208_pwrkey_work);

	/* set gpio13 to cpuirq function*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_MFP_CTL2, val);
	val = 0x2000;
	ip620x_write(pwrkey->dev->parent, IP6208_PAD_CTL, val);
	/* set cpuirq low*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_INTS_CTL, val); 

	/* clear flags */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_INT_FLAG, val);

	/* enable irq */
	val = 0xffff & ~(IP6208_DEFAULT_INT_MASK);
	ip620x_write(pwrkey->dev->parent, IP6208_INT_MASK, val);

	err = request_threaded_irq(pwrkey->irq, NULL, ip6208_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"ip6208_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	ip6208_pwrkey_hw_init();

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

static int ip6208_pwrkey_remove(struct platform_device *pdev)
{
	struct ip6208_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void ip6208_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ip6208_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int ip6208_pwrkey_suspend(struct device *dev)
{
	struct ip6208_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_work_sync(&pwrkey->work);
	flush_workqueue(pwrkey->workqueue);

	return 0;
}

static int ip6208_pwrkey_resume(struct device *dev)
{
	uint8_t val;
	struct ip6208_pwrkey *pwrkey = dev_get_drvdata(dev);

	/* set gpio13 to cpuirq function*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_MFP_CTL2, val);
	val = 0x2000;
	ip620x_write(pwrkey->dev->parent, IP6208_PAD_CTL, val);
	/* set cpuirq low*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_INTS_CTL, val); 

	/* clear flags */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6208_INT_FLAG, val);

	/* enable irq */
	val = 0xffff & ~(IP6208_DEFAULT_INT_MASK);
	ip620x_write(pwrkey->dev->parent, IP6208_INT_MASK, val);

	queue_work(pwrkey->workqueue, &pwrkey->work);
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops ip6208_pwrkey_pm_ops = {
	.suspend        = ip6208_pwrkey_suspend,
	.resume         = ip6208_pwrkey_resume,
};
#endif

static struct platform_driver ip6208_pwrkey_driver = {
	.probe  = ip6208_pwrkey_probe,
	.remove = ip6208_pwrkey_remove,
	.shutdown = ip6208_pwrkey_shutdown,
	.driver = {
		.name   = "ip6208-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &ip6208_pwrkey_pm_ops,
#endif
	},
};

static int __init ip6208_pwrkey_init(void)
{
	return platform_driver_register(&ip6208_pwrkey_driver);
}
module_init(ip6208_pwrkey_init);

static void __exit ip6208_pwrkey_exit(void)
{
	platform_driver_unregister(&ip6208_pwrkey_driver);
}
module_exit(ip6208_pwrkey_exit);

MODULE_DESCRIPTION("ip6208 Power Key driver");
MODULE_ALIAS("platform:ip6208-pwrkey");
MODULE_LICENSE("GPL v2");
