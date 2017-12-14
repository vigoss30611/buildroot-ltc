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
#include <mach/ip6313.h>

struct ip6313_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct workqueue_struct *workqueue;
	struct delayed_work work;
	bool pressed_first;
	bool keydown;
	int irq;
};

static int ip6313_pwrkey_irq_enable(int en)
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

static int ip6313_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 3))
		writel((1 << 3), rtc_base + 0x4);
	return 0;
}

static int ip6313_pwrkey_set_dir(int dir)
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

static int ip6313_pwrkey_set_pulldown(int en)
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

static int ip6313_pwrkey_set_polarity(int polar)
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

static void ip6313_pwrkey_work(struct work_struct *work)
{
	int ret;
	struct ip6313_pwrkey *pwrkey =
			container_of(work, struct ip6313_pwrkey, work);
	IP6313_INFO("power key process...\n");
	uint8_t status = 0;
	uint8_t status1 = 0;
	uint8_t key_press = 0;
	uint8_t key_shortpress = 0;
	uint8_t key_longpress = 0;
	uint8_t key_us_press = 0;

	ret = ip6313_read(pwrkey->dev->parent, IP6313_INT_FLAG0, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in flag0 reg 0x%02x, error: %d\n",
				IP6313_INT_FLAG0, ret);
		return;
	}
	ip6313_write(pwrkey->dev->parent, IP6313_INT_FLAG0, status);

	ret = ip6313_read(pwrkey->dev->parent, IP6313_INT_FLAG1, &status1);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in flag1 reg 0x%02x, error: %d\n",
				IP6313_INT_FLAG1, ret);
		return;
	}

	key_us_press = status & 0x04;
	key_press = (status1 >> 5) & 0x1;
	key_shortpress = status & 0x01;
	key_longpress = status & 0x02;

	if (!pwrkey->keydown && key_press) {
		IP6313_INFO("power key down\n");
		pwrkey->keydown = true;
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
		input_sync(pwrkey->input);
	} else if (!key_press && pwrkey->keydown) {
		IP6313_INFO("power key up\n");
		pwrkey->keydown = false;
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(pwrkey->input);
	}

	if(pwrkey->keydown){
		queue_delayed_work(pwrkey->workqueue, &pwrkey->work, msecs_to_jiffies(10));
	}
	else
		ip6313_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t ip6313_pwrkey_isr(int irq, void *data)
{
	struct ip6313_pwrkey *pwrkey = data;
	IP6313_DBG("ip6313_pwrkey_isr: \n");

	/*TODO: clear irq*/
	ip6313_pwrkey_irq_enable(0);
	ip6313_pwrkey_irq_clr();

	queue_delayed_work(pwrkey->workqueue, &pwrkey->work,msecs_to_jiffies(50));

	return IRQ_HANDLED;
}

static int ip6313_pwrkey_hw_init(void)
{
	/* disable rtc gpio0 irq */
	ip6313_pwrkey_irq_enable(0);
	/* set rtc gpio0 to input */
	ip6313_pwrkey_set_dir(1);
	/* rtc gpio0 pulldown enable */
	ip6313_pwrkey_set_pulldown(0);
	/* high valid */
	ip6313_pwrkey_set_polarity(0);
	/* enable rtc gpio0 irq */
	ip6313_pwrkey_irq_enable(1);

	return 0;
}

static int ip6313_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct ip6313_pwrkey *pwrkey;
	/*struct ip6313_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;
	uint8_t val;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate ip6313_pwrkey\n");
		return -ENOMEM;
	}

	pwrkey->irq = IP6313_PMIC_IRQ;
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
	input->name = "ip6313_pwrkey";
	input->phys = "ip6313_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);

	pwrkey->workqueue = create_singlethread_workqueue("ip6313_pwrkey_workqueue");
	INIT_DELAYED_WORK(&pwrkey->work, ip6313_pwrkey_work);

	/* set gpio3 to cpuirq function*/
	val = 0x0;
	ip6313_write(pwrkey->dev->parent, IP6313_MFP_CTL0, val);
	val = 0x8;
	ip6313_write(pwrkey->dev->parent, IP6313_PAD_CTL, val);
	/* set cpuirq high valid*/
	val = 0x1;
	ip6313_write(pwrkey->dev->parent, IP6313_INTS_CTL, val); 

	/* clear flags */
	val = 0x0;
	ip6313_write(pwrkey->dev->parent, IP6313_INT_FLAG0, val);

	/* enable irq */
	val = 0xff & ~(IP6313_DEFAULT_INT_MASK);
	ip6313_write(pwrkey->dev->parent, IP6313_INT_MASK0, val);

	err = request_threaded_irq(pwrkey->irq, NULL, ip6313_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"ip6313_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	ip6313_pwrkey_hw_init();

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

static int ip6313_pwrkey_remove(struct platform_device *pdev)
{
	struct ip6313_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void ip6313_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ip6313_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int ip6313_pwrkey_suspend(struct device *dev)
{
	struct ip6313_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_work_sync(&pwrkey->work);
	flush_workqueue(pwrkey->workqueue);

	return 0;
}

static int ip6313_pwrkey_resume(struct device *dev)
{
	uint8_t val;
	struct ip6313_pwrkey *pwrkey = dev_get_drvdata(dev);
	/* set gpio3 to cpuirq function*/
	val = 0x0;
	ip6313_write(pwrkey->dev->parent, IP6313_MFP_CTL0, val);
	val = 0x8;
	ip6313_write(pwrkey->dev->parent, IP6313_PAD_CTL, val);
	/* set cpuirq low valid*/
	val = 0x0;
	ip6313_write(pwrkey->dev->parent, IP6313_INTS_CTL, val); 

	/* clear flags */
	val = 0xFF;
	ip6313_write(pwrkey->dev->parent, IP6313_INT_FLAG0, val);

	/* enable irq */
	val = 0xff & ~(IP6313_DEFAULT_INT_MASK);
	ip6313_write(pwrkey->dev->parent, IP6313_INT_MASK0, val);

	queue_work(pwrkey->workqueue, &pwrkey->work);
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops ip6313_pwrkey_pm_ops = {
	.suspend        = ip6313_pwrkey_suspend,
	.resume         = ip6313_pwrkey_resume,
};
#endif

static struct platform_driver ip6313_pwrkey_driver = {
	.probe  = ip6313_pwrkey_probe,
	.remove = ip6313_pwrkey_remove,
	.shutdown = ip6313_pwrkey_shutdown,
	.driver = {
		.name   = "ip6313-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &ip6313_pwrkey_pm_ops,
#endif
	},
};

static int __init ip6313_pwrkey_init(void)
{
	return platform_driver_register(&ip6313_pwrkey_driver);
}
module_init(ip6313_pwrkey_init);

static void __exit ip6313_pwrkey_exit(void)
{
	platform_driver_unregister(&ip6313_pwrkey_driver);
}
module_exit(ip6313_pwrkey_exit);

MODULE_DESCRIPTION("ip6313 Power Key driver");
MODULE_ALIAS("platform:ip6313-pwrkey");
MODULE_LICENSE("GPL v2");
