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
#include <mach/ip6205.h>
#include <mach/items.h>
#include <mach/pad.h>
#include <linux/power_supply.h>


struct ip6205_pwrkey {
	struct device *dev;
	struct input_dev *input;
	struct workqueue_struct *workqueue;
	struct delayed_work work;
	bool pressed_first;
	bool keydown;
	int irq;
};

static int ip6205_pwrkey_irq_enable(int en)
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

static int ip6205_pwrkey_irq_clr(void)
{
	uint8_t val;
	void __iomem *rtc_base = IO_ADDRESS(SYSMGR_RTC_BASE);

	val = readl(rtc_base + 0x8);
	if (val & (1 << 3))
		writel((1 << 3), rtc_base + 0x4);
	return 0;
}

static int ip6205_pwrkey_set_dir(int dir)
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

static int ip6205_pwrkey_set_pulldown(int en)
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

static int ip6205_pwrkey_set_polarity(int polar)
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
#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE
extern void batt_set_charging_status(int batt_status);
#endif
static void ip6205_pwrkey_work(struct work_struct *work)
{
	int ret;
	struct ip6205_pwrkey *pwrkey =
			container_of(work, struct ip6205_pwrkey, work);
	uint16_t status = 0;
	uint32_t key_press;
	uint8_t key_shortpress;
	uint8_t key_longpress;
	uint8_t key_us_press;;
#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE
	u8 dcin_inserted = -1;
	u8 pullout_inserted = -1;
	u8 gpio_val=0;
	int index;
	int abs_index;
#endif	

	ret = ip620x_read(pwrkey->dev->parent, IP6205_INT_FLAG, &status);
	if (ret < 0) {
		dev_err(pwrkey->dev,
				"Error in reading reg 0x%02x, error: %d\n",
				IP6205_INT_FLAG, ret);
		return;
	}
#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE	
	//pr_err("====%s=====status=%d\n", __func__,status);	
	dcin_inserted = status & 0x20;
	pullout_inserted = status & 0x40;
	if (dcin_inserted) {

		if(item_exist("power.dcdetect"))
		{
			index = item_integer("power.dcdetect", 1);
			abs_index = (index>0)? index : (-1*index);
			if (gpio_is_valid(abs_index))
			{
				gpio_val =gpio_get_value(abs_index);
				if(1 == gpio_val)
				{
					batt_set_charging_status(POWER_SUPPLY_STATUS_CHONNGDIANBAO_CHARGING);
					pr_err("chongdianbao has inserted\n");
				}
				else
				{

					batt_set_charging_status(POWER_SUPPLY_STATUS_CHARGING);
					pr_err("chongdianqi has inserted\n");
				}
			}

		}
	}
	else if(pullout_inserted)
	{
		batt_set_charging_status(POWER_SUPPLY_STATUS_DISCHARGING);
		pr_err("dcin has pull out\n");
	}
	else
	{
		key_us_press = status & 0x04;
		key_press = (status >> 15) & 0x1;
		key_shortpress = status & 0x01;
		key_longpress = status & 0x02;
		//pr_err("====key_us_press=%d,key_shortpress=%d key_longpress=%d\n",key_us_press,key_shortpress,key_longpress);
		if (!pwrkey->keydown && key_press) {
			IP620x_INFO("power key down\n");
			pwrkey->keydown = true;
			input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
			input_sync(pwrkey->input);
		} else if(!key_press && pwrkey->keydown){
			IP620x_INFO("power key up\n");
			pwrkey->keydown = false;
			input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
			input_sync(pwrkey->input);
		}
	}
#else
	key_us_press = status & 0x04;
	key_press = (status >> 15) & 0x1;
	key_shortpress = status & 0x01;
	key_longpress = status & 0x02;

	if (!pwrkey->keydown && key_press) {
		IP620x_INFO("power key down\n");
		pwrkey->keydown = true;
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 1);
		input_sync(pwrkey->input);
	} else if(!key_press && pwrkey->keydown){
		IP620x_INFO("power key up\n");
		pwrkey->keydown = false;
		input_event(pwrkey->input, EV_KEY, KEY_POWER, 0);
		input_sync(pwrkey->input);
	}
#endif
	if(pwrkey->keydown)
		queue_delayed_work(pwrkey->workqueue, &pwrkey->work,msecs_to_jiffies(10));
	else
		ip6205_pwrkey_irq_enable(1);

	return;
}

static irqreturn_t ip6205_pwrkey_isr(int irq, void *data)
{
	struct ip6205_pwrkey *pwrkey = data;
	IP620x_DBG("ip6205_pwrkey_isr: \n");

	/*TODO: clear irq*/
	ip6205_pwrkey_irq_enable(0);
	ip6205_pwrkey_irq_clr();

	queue_delayed_work(pwrkey->workqueue, &pwrkey->work,msecs_to_jiffies(50));

	return IRQ_HANDLED;
}

static int ip6205_pwrkey_hw_init(void)
{
	/* disable rtc gpio0 irq */
	ip6205_pwrkey_irq_enable(0);
	/* set rtc gpio0 to input */
	ip6205_pwrkey_set_dir(1);
	/* rtc gpio0 pulldown disable */
	ip6205_pwrkey_set_pulldown(0);
	/* low valid */
	ip6205_pwrkey_set_polarity(1);
	/* enable rtc gpio0 irq */
	ip6205_pwrkey_irq_enable(1);

	return 0;
}

static int ip6205_pwrkey_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct ip6205_pwrkey *pwrkey;
	/*struct ip6205_pwrkey_platform_data *pdata = pdev->dev.platform_data;*/
	int err;
	uint16_t val;

	/*if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}*/
	
	pwrkey = kzalloc(sizeof(*pwrkey), GFP_KERNEL);
	if (!pwrkey) {
		dev_dbg(&pdev->dev, "Can't allocate ip6205_pwrkey\n");
		return -ENOMEM;
	}

	pwrkey->irq = IP6205_PMIC_IRQ;
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
	input->name = "ip6205_pwrkey";
	input->phys = "ip6205_pwrkey/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register input device: %d\n", err);
		goto free_input_dev;
	}

	pwrkey->input = input;
	platform_set_drvdata(pdev, pwrkey);

	pwrkey->workqueue = create_singlethread_workqueue("ip6205_pwrkey_workqueue");
	INIT_DELAYED_WORK(&pwrkey->work, ip6205_pwrkey_work);

	/* set gpio13 to cpuirq function
	 * this is the same as ip6208, but in 6205 spec
	 * did not show these bits.
	 * */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_MFP_CTL2, val);
	/* set cpuirq low*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_INTS_CTL, val);

	/* clear flags */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_INT_FLAG, val);

	/* enable irq */
	val = 0xffff & ~(IP6205_DEFAULT_INT_MASK);
	ip620x_write(pwrkey->dev->parent, IP6205_INT_MASK, val);

	err = request_threaded_irq(pwrkey->irq, NULL, ip6205_pwrkey_isr,
				IRQF_SHARED | IRQF_DISABLED | IRQF_ONESHOT,
				"ip6205_pwrkey", pwrkey);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for pwrkey: %d\n",
				pwrkey->irq, err);
		goto unregister_input_dev;
	}

	ip6205_pwrkey_hw_init();
#ifdef CONFIG_INFT_CHONGDIANBAO_DETECTED_ENABLE	
	ip620x_read(pwrkey->dev->parent, IP6205_PPATH_STATUS, &val);
	//pr_err("====%s=====IP6205_PPATH_STATUS=%d\n", __func__,val);
	u8 gpio_val=0;
	int index;
	int abs_index;
	if (val & 0x2) {		
		if(item_exist("power.dcdetect"))
		{
			index = item_integer("power.dcdetect", 1);
			abs_index = (index>0)? index : (-1*index);
			if (gpio_is_valid(abs_index))
			{
				gpio_val =gpio_get_value(abs_index);
				if(1 == gpio_val)
				{
					batt_set_charging_status(POWER_SUPPLY_STATUS_CHONNGDIANBAO_CHARGING);
					pr_err("chongdianbao has inserted\n");
				}
				else
				{

					batt_set_charging_status(POWER_SUPPLY_STATUS_CHARGING);
					pr_err("chongdianqi has inserted\n");
				}
			}
		}
	}	
	else 	
	{		
		batt_set_charging_status(POWER_SUPPLY_STATUS_DISCHARGING);
		pr_err("dcin has pull out\n");	
	}
#endif	
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

static int ip6205_pwrkey_remove(struct platform_device *pdev)
{
	struct ip6205_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);

	return 0;
}

static void ip6205_pwrkey_shutdown(struct platform_device *pdev)
{
	struct ip6205_pwrkey *pwrkey = platform_get_drvdata(pdev);

	free_irq(pwrkey->irq, pwrkey);
	flush_workqueue(pwrkey->workqueue);
	destroy_workqueue(pwrkey->workqueue);
	input_unregister_device(pwrkey->input);
	kfree(pwrkey);
}

#ifdef CONFIG_PM
static int ip6205_pwrkey_suspend(struct device *dev)
{
	struct ip6205_pwrkey *pwrkey = dev_get_drvdata(dev);

	if (pwrkey->irq)
		disable_irq(pwrkey->irq);
	cancel_work_sync(&pwrkey->work);
	flush_workqueue(pwrkey->workqueue);

	return 0;
}

static int ip6205_pwrkey_resume(struct device *dev)
{
	uint8_t val;
	struct ip6205_pwrkey *pwrkey = dev_get_drvdata(dev);

	/* set gpio13 to cpuirq function
	 * this is the same as ip6208, but in 6205 spec
	 * did not show these bits.
	 * */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_MFP_CTL2, val);
	/* set cpuirq low*/
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_INTS_CTL, val);

	/* clear flags */
	val = 0x0;
	ip620x_write(pwrkey->dev->parent, IP6205_INT_FLAG, val);

	/* enable irq */
	val = 0xffff & ~(IP6205_DEFAULT_INT_MASK);
	ip620x_write(pwrkey->dev->parent, IP6205_INT_MASK, val);

	queue_work(pwrkey->workqueue, &pwrkey->work);
	if (pwrkey->irq)
		enable_irq(pwrkey->irq);

	return 0;
}

static const struct dev_pm_ops ip6205_pwrkey_pm_ops = {
	.suspend        = ip6205_pwrkey_suspend,
	.resume         = ip6205_pwrkey_resume,
};
#endif

static struct platform_driver ip6205_pwrkey_driver = {
	.probe  = ip6205_pwrkey_probe,
	.remove = ip6205_pwrkey_remove,
	.shutdown = ip6205_pwrkey_shutdown,
	.driver = {
		.name   = "ip6205-pwrkey",
		.owner  = THIS_MODULE,
#ifdef CONFIG_PM
		.pm     = &ip6205_pwrkey_pm_ops,
#endif
	},
};

static int __init ip6205_pwrkey_init(void)
{
	return platform_driver_register(&ip6205_pwrkey_driver);
}
module_init(ip6205_pwrkey_init);

static void __exit ip6205_pwrkey_exit(void)
{
	platform_driver_unregister(&ip6205_pwrkey_driver);
}
module_exit(ip6205_pwrkey_exit);

MODULE_DESCRIPTION("ip6205 Power Key driver");
MODULE_ALIAS("platform:ip6205-pwrkey");
MODULE_LICENSE("GPL v2");
