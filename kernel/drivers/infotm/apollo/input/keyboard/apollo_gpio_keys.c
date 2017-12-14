#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/syscalls.h>
#include <linux/mtd/mtd.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <asm/irq.h>
#include <asm/mach/irq.h>
#include <asm-generic/bitops/non-atomic.h>
#include <mach/imap-iomap.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <mach/nand.h>
#include <mach/rtcbits.h>
#include <mach/hw_cfg.h>
#include <linux/reboot.h>
#include <linux/switch.h>
#include <linux/delay.h>

#define KEY_RESET       0x1314

#define INTTYPE_LOW     (0)
#define INTTYPE_HIGH    (1)
#define INTTYPE_FALLING (2)
#define INTTYPE_RISING  (4)
#define INTTYPE_BOTH    (6)

/*#define GPIO_DEBUG*/
#ifdef	GPIO_DEBUG
#define	gpio_dbg(x...)	pr_err(KERN_ERR "GPIO DEBUG:" x)
#else
#define gpio_dbg(x...)
#endif

struct apollo_gpio {
	char name[16];
	int code;
	int irq;
	int index;		/* pad index */
	int num;		/* interrupt io num */
	unsigned long type;	/* interrupt type */
	int flt;		/* filter */
	struct work_struct work;
	struct input_dev *input;

	struct delayed_work power_work;
	struct delayed_work emu_work;
	u32 key_poll_delay;
	bool keydown;
};

struct apollo_gpio apollo_gpio[] = {
	{
		.name = "key-home",
		.code = KEY_HOME,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},
	{
		.name = "key-back",
		.code = KEY_BACK,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},

	{
		.name = "key-power",
		.code = KEY_POWER,
		.input = NULL,
	},
	{
		.name = "key-reset",
		.code = KEY_RESTART,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},
	{
		.name = "key-vol+",
		.code = KEY_VOLUMEUP,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},
	{
		.name = "key-vol-",
		.code = KEY_VOLUMEDOWN,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},
	{
		.name = "key-menu",
		.code = KEY_MENU,
		.type = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
		.flt = 0xff,
	},
};

struct gpio_key_cfg *pdata;

static void apollo_gpio_work(struct work_struct *work)
{
	struct apollo_gpio *ptr =
	    container_of(work, struct apollo_gpio, work);

	/*SOFT RESET  added by peter */
	if (unlikely(ptr->code == KEY_RESET)) {
		sys_sync();
		__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));
		__raw_writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE + 0x28));
		__raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0x7c));
	} else {
		input_event(ptr->input, EV_KEY, ptr->code,
		ptr->index > 0 ? !gpio_get_value(abs(ptr->index)) : gpio_get_value(abs(ptr->index)));
		input_sync(ptr->input);
		enable_irq(ptr->irq);
	}
	pr_err("Reporting KEY:name=%s, code=%d, val=%d\n", ptr->name,
	       ptr->code, gpio_get_value(abs(ptr->index)));

	return;
}

static void __onsleep_sync(struct work_struct *work)
{
	pr_err("Pre-syncing fs ...\n");
	sys_sync();
	pr_err("Pre-syncing done.\n");
}

static DECLARE_WORK(onsleep_sync_work, __onsleep_sync);
struct workqueue_struct *onsleep_work_queue;

static void apollo_pwkey_work(struct work_struct *work)
{
	struct apollo_gpio *ptr =
	    container_of(to_delayed_work(work), struct apollo_gpio,
			 power_work);
	unsigned int pwkey_stat = 0x40;
	uint8_t tmp;

	if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x58)) & pwkey_stat) {
		if (!ptr->keydown) {
			/* key down */
			ptr->keydown = true;
			input_event(ptr->input, EV_KEY, ptr->code, 1);
			input_sync(ptr->input);
			pr_err("Reporting KEY Down:name=%s, code=%d\n",
				 ptr->name, ptr->code);

			/* system may shutdown suddenly */
			queue_work(onsleep_work_queue, &onsleep_sync_work);
		}
	} else {
		/* key up */
		ptr->keydown = false;
		input_event(ptr->input, EV_KEY, ptr->code, 0);
		input_sync(ptr->input);
		pr_err("Reporting KEY Up:name=%s, code=%d\n", ptr->name,
			 ptr->code);
		rtcbit_set("retry_reboot", 0);
	}

	if (ptr->keydown)
		schedule_delayed_work(&ptr->power_work,
				      msecs_to_jiffies(ptr->key_poll_delay));
	else {
		tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
		tmp &= ~(0x33);
		writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
	}
}

static irqreturn_t apollo_gpio_isr(int irq, void *dev_id)
{
	struct apollo_gpio *ptr = dev_id;
	uint8_t tmp;

	//pr_err("%s invoked with name %s\n", __func__, ptr->name);

	if (ptr->code == KEY_POWER) {
		/*check int status */
		if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x8)) & 0x3) {
			/*mask int */
			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			tmp |= 0x3;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			/*clear int */
			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x4));
			tmp |= 0x3;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x4));
		}
		if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x160)) & 0x1) {
			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			tmp |= 0x10;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));

			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x160));
			tmp |= 0x1;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x160));
		}
		if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x160)) & 0x2) {
			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			tmp |= 0x20;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));

			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x160));
			tmp |= 0x2;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x160));
		}

		schedule_delayed_work(&ptr->power_work,
				msecs_to_jiffies(ptr->key_poll_delay));
	} else {
		disable_irq_nosync(ptr->irq);
		schedule_work(&ptr->work);
	}

	return IRQ_HANDLED;
}

static int apollo_gpio_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	int i;
	int error;
	uint8_t tmp;
	pdata = pdev->dev.platform_data;

	input = input_allocate_device();
	if (!input) {
		pr_err("allocate input device failed.\n");
		return -ENOMEM;
	}

	/* work queue must be created before IRQ is registered */
	pr_info("Create workqueue for onsleep sync.\n");
	onsleep_work_queue = create_singlethread_workqueue("ossync");
	if (onsleep_work_queue == NULL)
		pr_err("failed to create onsleep work queue.\n");

	for (i = 0; i < ARRAY_SIZE(apollo_gpio); i++) {

		if(apollo_gpio[i].code == KEY_POWER) {
			continue;/*TODO: to be fix*/
			if(item_exist("board.arch")
				&& !item_equal("board.arch", "a5pv10", 0))
				continue;

			tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x4C));
			tmp |= 1 << 4;
			writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x4C));

			writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			INIT_DELAYED_WORK(&apollo_gpio[i].power_work, apollo_pwkey_work);
			apollo_gpio[i].irq = GIC_RTCSLP_ID;/* 210 */
			apollo_gpio[i].key_poll_delay = 10;
			apollo_gpio[i].keydown = false;
			apollo_gpio[i].index = -ITEMS_EINT;

			apollo_gpio[i].input = input;
			input_set_capability(input, EV_KEY, apollo_gpio[i].code);
			error = request_irq(apollo_gpio[i].irq, apollo_gpio_isr,
					0, apollo_gpio[i].name, &apollo_gpio[i]);
			if (error) {
				pr_err("claim irq %d fail,name is %s\n",
					apollo_gpio[i].irq,apollo_gpio[i].name);
			} else {
				pr_info("claim irq %d success,name is %s\n",
					apollo_gpio[i].irq,apollo_gpio[i].name);
			}
		} else {
			INIT_WORK(&apollo_gpio[i].work, apollo_gpio_work);
			if (apollo_gpio[i].code == KEY_MENU)
				apollo_gpio[i].index = pdata->menu;
			else if (apollo_gpio[i].code == KEY_HOME)
				apollo_gpio[i].index = pdata->home;
			else if (apollo_gpio[i].code == KEY_BACK)
				apollo_gpio[i].index = pdata->back;
			else if (apollo_gpio[i].code == KEY_RESTART)
				apollo_gpio[i].index = pdata->reset;
			else if (apollo_gpio[i].code == KEY_VOLUMEUP)
				apollo_gpio[i].index = pdata->volup;
			else if (apollo_gpio[i].code == KEY_VOLUMEDOWN)
				apollo_gpio[i].index = pdata->voldown;
			else
				continue;

			if (apollo_gpio[i].index == -ITEMS_EINT)
				continue;

			if (gpio_is_valid(abs(apollo_gpio[i].index))) {
				error = gpio_request(abs(apollo_gpio[i].index), apollo_gpio[i].name);
				if (error) {
					pr_err("failed request gpio for %s\n", apollo_gpio[i].name);
					continue;
				}
			}

			apollo_gpio[i].irq = gpio_to_irq(abs(apollo_gpio[i].index));
			gpio_direction_input(abs(apollo_gpio[i].index));

			apollo_gpio[i].input = input;
			input_set_capability(input, EV_KEY, apollo_gpio[i].code);

			error = request_irq(apollo_gpio[i].irq, apollo_gpio_isr,
					apollo_gpio[i].type,
					apollo_gpio[i].name,
					&apollo_gpio[i]);

			if (error) {
				pr_err("claim irq %d fail,name is %s\n",
						apollo_gpio[i].irq, apollo_gpio[i].name);
				goto __fail_exit__;
			} else {
				pr_info("claim irq %d success,name is %s\n",
						apollo_gpio[i].irq, apollo_gpio[i].name);
			}
		}
	}

	input->name = "imap-gpio";
	input->phys = "imap-gpio/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	error = input_register_device(input);
	if (error) {
		pr_err("imap-gpio: Unable to register input device, error: %d\n",
				error);
		goto __fail_exit__;
	}

	return 0;

__fail_exit__:
	pr_err("%s exit with error\n", __func__);
	input_free_device(input);

	return error;
}

static int apollo_gpio_remove(struct platform_device *pdev)
{
	input_unregister_device(apollo_gpio->input);
	return 0;
}

#ifdef	CONFIG_PM
static int apollo_gpio_suspend(struct platform_device *pdev,
				 pm_message_t state)
{
	writel(0x33, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
	apollo_gpio[2].keydown = false;
	return 0;
}

static int apollo_gpio_resume(struct platform_device *pdev)
{
	int i;
	uint8_t tmp;
	pdata = pdev->dev.platform_data;

	writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));

	for (i = 0; i < ARRAY_SIZE(apollo_gpio); i++) {
		if (gpio_is_valid(abs(apollo_gpio[i].index))
			&& apollo_gpio[i].code != KEY_POWER) {
			gpio_direction_input(abs(apollo_gpio[i].index));
		}
	}

	tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x8));
	if (!(tmp & 0x80) && apollo_gpio[2].input) {
		input_event(apollo_gpio[2].input, EV_KEY, KEY_POWER, 1);
		input_sync(apollo_gpio[2].input);
		input_event(apollo_gpio[2].input, EV_KEY, KEY_POWER, 0);
		input_sync(apollo_gpio[2].input);
	}

	return 0;
}
#else

#define	apollo_gpio_suspend	NULL
#define	apollo_gpio_resume	NULL

#endif

static struct platform_driver apollo_gpio_device_driver = {
	.probe = apollo_gpio_probe,
	.remove = apollo_gpio_remove,
	.suspend = apollo_gpio_suspend,
	.resume = apollo_gpio_resume,
	.driver = {
		   .name = "imap-gpio",
		   .owner = THIS_MODULE,
		   }
};

static int __init apollo_gpio_init(void)
{
	return platform_driver_register(&apollo_gpio_device_driver);
}

module_init(apollo_gpio_init);

static void __exit apollo_gpio_exit(void)
{
	platform_driver_unregister(&apollo_gpio_device_driver);
}

module_exit(apollo_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Keyboard driver for CPU GPIOs");
MODULE_ALIAS("platform:imap-gpio");
