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
#include <mach/ip6203.h>

struct ip6203_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct workqueue_struct *workqueue;
	struct work_struct work;
	bool pressed_first;
	int irq;
};

static int ip6203_pwrkey_irq_enable(int en)
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

static int ip6203_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 4))
		writel((1 << 4), rtc_base + 0x4);
	return 0;
}

static int ip6203_pwrkey_set_dir(int dir)
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

static int ip6203_pwrkey_set_pulldown(int en)
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

static int ip6203_pwrkey_set_polarity(int polar)
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

static uint16_t __cnt = 0, __pri_key = 0;

static void ip6203_pwrkey_work(struct work_struct *work)
{
	int ret;
	struct ip6203_pwrkey *pwrkey =
			container_of(work, struct ip6203_pwrkey, work);
	uint16_t status = 0;
	uint16_t key_press;
	uint16_t key_changed = 0;
	uint16_t key_tmp = 0;
	uint16_t value = 0;
#if 0
	ip6203_read(pwrkey->dev->parent, 0x0, &value);
	ip6203_read(pwrkey->dev->parent, IP6203_INTS_CTL, &value);
	ip6203_read(pwrkey->dev->parent, IP6203_INT_FLAG, &value);
	ip6203_write(pwrkey->dev->parent, IP6203_INT_FLAG, 0x80);
	//ip6203_write(pwrkey->dev->parent, IP6203_CHG_CTL2, 0xfc00);
	//ip6203_write(pwrkey->dev->parent, IP6203_INT_MASK, 1<<7);
	ip6203_read(pwrkey->dev->parent, 0xa2, &value);
	ip6203_read(pwrkey->dev->parent, IP6203_INT_FLAG, &value);
	ip6203_read(pwrkey->dev->parent, 0x16, &value);
	ip6203_read(pwrkey->dev->parent, 0x17, &value);
#endif
	ret = ip6203_read(pwrkey->dev->parent, IP6203_INT_FLAG, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in reading reg 0x%02x, error: %d\n",
				IP6203_INT_FLAG, ret);
		return;
	}
	ip6203_write(pwrkey->dev->parent, IP6203_INT_FLAG, status);
#if 1
	key_press = status & 0x8000;
	if(__cnt == 0){
		__pri_key = key_press;
		__cnt = 2;
	}
	else {
		__cnt = 1;
		if (key_press != __pri_key){
			key_changed = 1;
			__pri_key = key_press;
		}
	}
#endif

	if ((key_changed && key_press) || ((__cnt == 2) && key_press)) {
		IP6203_INFO("power key down\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
		input_sync(pwrkey->input);
	} else if (key_changed && !key_press) {
		IP6203_INFO("power key up\n");
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(pwrkey->input);
	}

	ip6203_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t ip6203_pwrkey_isr(int irq, void *data)
{
	struct ip6203_pwrkey *pwrkey = data;

	/*TODO: clear irq*/
	ip6203_pwrkey_irq_enable(0);
	ip6203_pwrkey_irq_clr();

	queue_work(pwrkey->workqueue, &pwrkey->work);

	return IRQ_HANDLED;
}

static int ip6203_pwrkey_hw_init(void)
{
	/* disable rtc gpio1 irq */
	ip6203_pwrkey_irq_enable(0);
	/* set rtc gpio1 to input */
	ip6203_pwrkey_set_dir(1);
	/* rtc gpio1 pulldown disable */
	ip6203_pwrkey_set_pulldown(0);
	/* high valid ---> change low valid*/
	ip6203_pwrkey_set_polarity(1);
	/* enable rtc gpio1 irq */
	ip6203_pwrkey_irq_enable(1);

	return 0;
}

static int ip6203_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct ip6203_pwrkey *pwrkey;
	/*struct ip6203_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;
	uint16_t val;
	uint16_t key_tmp = 0;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate ip6203_pwrkey\n");
		return -ENOMEM;
	}

	IP6203_ERR("new IP6203 regs read: \n");
	pwrkey->irq = IP6203_PMIC_IRQ;
	pwrkey->dev = &pdev->dev;
	/*pwrkey->pdata = pdata;*/
	pwrkey->pressed_first = false;

	input = input_allocate_device();
	if (!input) {
		dev_err(&pdev->dev, "Can't allocate input device\n");
		err = -ENOMEM;
		goto free_pwrkey;
	}

	IP6203_ERR("new IP6203 regs read: \n");
	input_set_capability(input, EV_KEY, KEY_POWER);
	input->name = "ip6203_pwrkey";
	input->phys = "ip6203_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);

	pwrkey->workqueue = create_singlethread_workqueue("ip6203_pwrkey_workqueue");
	INIT_WORK(&pwrkey->work, ip6203_pwrkey_work);

	/* irq output enable */
	ip6203_read(pwrkey->dev->parent, IP6203_PAD_CTL, &val);
	val &= ~(1 << 12 | 1 << 13);
	ip6203_write(pwrkey->dev->parent, IP6203_PAD_CTL, val);
	val = 0x0;
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_CTL, val);
	
	/*config cpuirq pads*/
	ip6203_write(pwrkey->dev->parent, IP6203_MFP_CTL2, 0x0);

	ip6203_read(pwrkey->dev->parent, IP6203_PAD_CTL, &key_tmp);
	/* clear flags */
	val = 0xffff;
	ip6203_write(pwrkey->dev->parent, IP6203_INT_FLAG, val);

	IP6203_ERR("new IP6203 regs read: \n");
	/* enable irq */
	ip6203_read(pwrkey->dev->parent, IP6203_INT_MASK, &val);
	val &= ~DEFAULT_INT_MASK;
	ip6203_write(pwrkey->dev->parent, IP6203_INT_MASK, val);

	IP6203_ERR("new IP6203 regs read: \n");
	err = request_threaded_irq(pwrkey->irq, NULL, ip6203_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"ip6203_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	ip6203_pwrkey_hw_init();

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

static int ip6203_pwrkey_remove(struct platform_device *pdev)
{
	struct ip6203_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void ip6203_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ip6203_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int ip6203_pwrkey_suspend(struct device *dev)
{
	struct ip6203_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_work_sync(&pwrkey->work);
	flush_workqueue(pwrkey->workqueue);

	return 0;
}

static int ip6203_pwrkey_resume(struct device *dev)
{
	uint16_t val;
	struct ip6203_pwrkey *pwrkey = dev_get_drvdata(dev);
#if 0
	/* irq output enable */
	ip6203_read(pwrkey->dev->parent, IP6203_MFP_CTL5, &val);
	val &= 0xE0;
	val |= IP6203_IRQ_OUT|0x10;
	ip6203_write(pwrkey->dev->parent, IP6203_MFP_CTL5, val);

	/* clear flags */
	val = 0xff;
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_FLAG0, val);
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_FLAG1, val);

	/* enable irq */
	val = DEFAULT_INT_MASK;
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_CTL0, val);
	val = 0x00;
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_CTL1, val);
#endif
	/*config cpuirq pads*/
	uint16_t key_tmp;
	ip6203_pwrkey_hw_init();
	ip6203_write(pwrkey->dev->parent, IP6203_MFP_CTL2, 0x0);

	/* irq output enable */
	ip6203_read(pwrkey->dev->parent, IP6203_PAD_CTL, &val);
	val &= ~(1 << 12 | 1 << 13);
	ip6203_write(pwrkey->dev->parent, IP6203_PAD_CTL, val);
	val = 0x0;
	ip6203_write(pwrkey->dev->parent, IP6203_INTS_CTL, val);

	/* clear flags */
	val = 0xffff;
	ip6203_write(pwrkey->dev->parent, IP6203_INT_FLAG, val);

	/* enable irq */
	ip6203_read(pwrkey->dev->parent, IP6203_INT_MASK, &val);
	val &= ~DEFAULT_INT_MASK;
	ip6203_write(pwrkey->dev->parent, IP6203_INT_MASK, val);
	ip6203_read(pwrkey->dev->parent, IP6203_INT_MASK, &val);

	queue_work(pwrkey->workqueue, &pwrkey->work);
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops ip6203_pwrkey_pm_ops = {
	.suspend        = ip6203_pwrkey_suspend,
	.resume         = ip6203_pwrkey_resume,
};
#endif

static struct platform_driver ip6203_pwrkey_driver = {
	.probe  = ip6203_pwrkey_probe,
	.remove = ip6203_pwrkey_remove,
	.shutdown = ip6203_pwrkey_shutdown,
	.driver = {
		.name   = "ip6203-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &ip6203_pwrkey_pm_ops,
#endif
	},
};

static int __init ip6203_pwrkey_init(void)
{
	return platform_driver_register(&ip6203_pwrkey_driver);
}
module_init(ip6203_pwrkey_init);

static void __exit ip6203_pwrkey_exit(void)
{
	platform_driver_unregister(&ip6203_pwrkey_driver);
}
module_exit(ip6203_pwrkey_exit);

MODULE_DESCRIPTION("IP6203 Power Key driver");
MODULE_ALIAS("platform:ip6203-pwrkey");
MODULE_LICENSE("GPL v2");
