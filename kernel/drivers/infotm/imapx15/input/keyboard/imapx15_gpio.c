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

struct imapx15_gpio {
	char name[16];
	int code;
	int irq;
	int index;		/*pad index */
	int num;		/*0~25 */
	unsigned long type;	/*interrupt type */
	int flt;		/*filter */
	struct work_struct work;
	struct input_dev *input;

	struct delayed_work power_work;
	struct delayed_work emu_work;
	u32 key_poll_delay;
	bool keydown;
};

struct imapx15_gpio imapx15_gpio[] = {
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
	 .code = KEY_RESET,
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
static int loop;
static int invoke;

static void imapx15_emu_key_work(struct work_struct *work)
{
	struct imapx15_gpio *ptr = container_of(to_delayed_work(work),
						 struct imapx15_gpio,
						 emu_work);
	unsigned int stat = 0x40;

	if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x58)) & stat) {
		loop++;
		if (loop >= 350 && invoke == 0)
			invoke = 1;
	} else {
		ptr->keydown = false;
		input_event(ptr->input, EV_KEY, ptr->code, 0);
		input_sync(ptr->input);
		rtcbit_set("retry_reboot", 0);
	}

	if (ptr->keydown)
		schedule_delayed_work(&ptr->emu_work, msecs_to_jiffies(10));
}

int imapx15_iokey_emu(int index)
{
#if 0
	if (imapx15_gpio[index].code != KEY_POWER) {
		input_event(imapx15_gpio[index].input, EV_KEY,
			    imapx15_gpio[index].code, 1);
		input_sync(imapx15_gpio[index].input);
		input_event(imapx15_gpio[index].input, EV_KEY,
			    imapx15_gpio[index].code, 0);
		input_sync(imapx15_gpio[index].input);
	} else {
		if (!item_exist("board.arch")
		    || item_equal("board.arch", "a5pv10", 0)) {
			loop = 0;
			invoke = 0;
			imapx15_gpio[index].keydown = true;
			input_event(imapx15_gpio[index].input, EV_KEY,
				    imapx15_gpio[index].code, 1);
			input_sync(imapx15_gpio[index].input);

			schedule_delayed_work(&imapx15_gpio[index].emu_work,
					      msecs_to_jiffies(10));
		}
	}
#endif
	gpio_dbg("imap_iokey_emu\n");
	return 0;
}
EXPORT_SYMBOL(imapx15_iokey_emu);

static void imapx15_gpio_work(struct work_struct *work)
{
	struct imapx15_gpio *ptr =
	    container_of(work, struct imapx15_gpio, work);

	/*SOFT RESET  added by peter */
	if (unlikely(ptr->code == KEY_RESET)) {
		sys_sync();
		__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));
		__raw_writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE + 0x28));
		__raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0x7c));
	} else {
		input_event(ptr->input, EV_KEY, ptr->code,
			    !gpio_get_value(ptr->index));
		input_sync(ptr->input);
		enable_irq(ptr->irq);
	}
	pr_err("Reporting KEY:name=%s, code=%d, val=%d\n", ptr->name,
	       ptr->code, !gpio_get_value(ptr->index));

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

static void imapx15_pwkey_work(struct work_struct *work)
{
	struct imapx15_gpio *ptr =
	    container_of(to_delayed_work(work), struct imapx15_gpio,
			 power_work);
	unsigned int pwkey_stat = 0x40;

	if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x58)) & pwkey_stat) {
		loop++;

		if (!ptr->keydown) {
			/* key down */
			ptr->keydown = true;
			input_event(ptr->input, EV_KEY, ptr->code, 1);
			input_sync(ptr->input);
			gpio_dbg("Reporting KEY Down:name=%s, code=%d\n",
				 ptr->name, ptr->code);

			/* system may shutdown suddenly */
			queue_work(onsleep_work_queue, &onsleep_sync_work);
		}

		if (loop >= 350 && invoke == 0)
			invoke = 1;
	} else {
		/* key up */
		ptr->keydown = false;
		input_event(ptr->input, EV_KEY, ptr->code, 0);
		input_sync(ptr->input);
		gpio_dbg("Reporting KEY Up:name=%s, code=%d\n", ptr->name,
			 ptr->code);
		rtcbit_set("retry_reboot", 0);
	}

	if (ptr->keydown)
		schedule_delayed_work(&ptr->power_work,
				      msecs_to_jiffies(ptr->key_poll_delay));
	else
		 writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
}

static irqreturn_t imapx15_gpio_isr(int irq, void *dev_id)
{
	struct imapx15_gpio *ptr = dev_id;

	//pr_err("%s invoked with name %s\n", __func__, ptr->name);

	if (ptr->code == KEY_POWER) {
		/*check int status */
		if (readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x8)) & 0x3) {
			/*mask int */
			writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			/*clear int */
			writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE + 0x4));

			loop = 0;
			invoke = 0;

			schedule_delayed_work(&ptr->power_work,
					      msecs_to_jiffies(ptr->
							       key_poll_delay));
		}
//	} else if (imapx_pad_irq_pending(ptr->index)) {
//		imapx_pad_irq_clear(ptr->index);
//		schedule_work(&ptr->work);
	} else {
		disable_irq_nosync(ptr->irq);
		schedule_work(&ptr->work);
	}

	return IRQ_HANDLED;
}

static int imapx15_gpio_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	int i;
	int error;
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

	for (i = 0; i < ARRAY_SIZE(imapx15_gpio); i++) {

		if(imapx15_gpio[i].code == KEY_POWER) {
			if(item_exist("board.arch")
				&& !item_equal("board.arch", "a5pv10", 0))
				continue;

			writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
			INIT_DELAYED_WORK(&imapx15_gpio[i].power_work, imapx15_pwkey_work);
			imapx15_gpio[i].irq = GIC_MGRPM_ID;
			imapx15_gpio[i].key_poll_delay = 10;
			imapx15_gpio[i].keydown = false;
			imapx15_gpio[i].index = -ITEMS_EINT;

			imapx15_gpio[i].input = input;
			input_set_capability(input, EV_KEY, imapx15_gpio[i].code);
			error = request_irq(imapx15_gpio[i].irq, imapx15_gpio_isr,
					0, imapx15_gpio[i].name, &imapx15_gpio[i]);
			if (error) {
				pr_err("claim irq %d fail,name is %s\n",
					imapx15_gpio[i].irq,imapx15_gpio[i].name);
			} else {
				pr_info("claim irq %d success,name is %s\n",
					imapx15_gpio[i].irq,imapx15_gpio[i].name);
			}
		} else {
			INIT_WORK(&imapx15_gpio[i].work, imapx15_gpio_work);
			if (imapx15_gpio[i].code == KEY_MENU)
				imapx15_gpio[i].index = pdata->menu;
			else if (imapx15_gpio[i].code == KEY_HOME)
				imapx15_gpio[i].index = pdata->home;
			else if (imapx15_gpio[i].code == KEY_BACK)
				imapx15_gpio[i].index = pdata->back;
			else if (imapx15_gpio[i].code == KEY_RESET)
				imapx15_gpio[i].index = pdata->reset;
			else if (imapx15_gpio[i].code == KEY_VOLUMEUP)
				imapx15_gpio[i].index = pdata->volup;
			else if (imapx15_gpio[i].code == KEY_VOLUMEDOWN)
				imapx15_gpio[i].index = pdata->voldown;
			else
				continue;
//			else if (imapx15_gpio[i].code == KEY_POWER) {
//				imapx15_gpio[i].index = -ITEMS_EINT;
//				continue;
//			}

			if (imapx15_gpio[i].index == -ITEMS_EINT)
				continue;

			if (gpio_is_valid(imapx15_gpio[i].index)) {
				error = gpio_request(imapx15_gpio[i].index, imapx15_gpio[i].name);
				if (error) {
					pr_err("failed request gpio for %s\n", imapx15_gpio[i].name);
					continue;
				}
			}

			/*		imapx15_gpio[i].irq =
					imapx_pad_irq_number(imapx15_gpio[i].index);
					imapx_pad_set_mode(0, 1, imapx15_gpio[i].index);
					imapx_pad_set_dir(1, 1, imapx15_gpio[i].index);
					imapx_pad_irq_config(imapx15_gpio[i].index,
					imapx15_gpio[i].type,
					imapx15_gpio[i].flt);
					*/
			imapx15_gpio[i].irq = gpio_to_irq(imapx15_gpio[i].index);
			gpio_direction_input(imapx15_gpio[i].index);

			imapx15_gpio[i].input = input;
			input_set_capability(input, EV_KEY, imapx15_gpio[i].code);

			error = request_irq(imapx15_gpio[i].irq, imapx15_gpio_isr,
					imapx15_gpio[i].type,
					imapx15_gpio[i].name,
					&imapx15_gpio[i]);

			if (error) {
				pr_err("claim irq %d fail,name is %s\n",
						imapx15_gpio[i].irq, imapx15_gpio[i].name);
				goto __fail_exit__;
			} else {
				pr_info("claim irq %d success,name is %s\n",
						imapx15_gpio[i].irq, imapx15_gpio[i].name);
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

static int imapx15_gpio_remove(struct platform_device *pdev)
{
	input_unregister_device(imapx15_gpio->input);
	return 0;
}

#ifdef	CONFIG_PM
static int imapx15_gpio_suspend(struct platform_device *pdev,
				 pm_message_t state)
{
	writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
	imapx15_gpio[2].keydown = false;
	return 0;
}

static int imapx15_gpio_resume(struct platform_device *pdev)
{
	int i;
	uint8_t tmp;
	pdata = pdev->dev.platform_data;

	writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));

	for (i = 0; i < ARRAY_SIZE(imapx15_gpio); i++) {
/*		if (imapx15_gpio[i].index) {
			imapx_pad_set_mode(0, 1, imapx15_gpio[i].index);
			imapx_pad_set_dir(1, 1, imapx15_gpio[i].index);
			imapx_pad_irq_config(imapx15_gpio[i].index,
					     imapx15_gpio[i].type,
					     imapx15_gpio[i].flt);
		}
*/
		if (gpio_is_valid(imapx15_gpio[i].index)
			&& imapx15_gpio[i].code != KEY_POWER) {
			gpio_direction_input(imapx15_gpio[i].index);
			/*irq_set_irq_type(imapx15_gpio[i].irq, imapx15_gpio[i].type);*/
		}
	}
	
	tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x8));
	if (!(tmp & 0x80) && imapx15_gpio[2].input) {
		input_event(imapx15_gpio[2].input, EV_KEY, KEY_POWER, 1);
		input_sync(imapx15_gpio[2].input);
		input_event(imapx15_gpio[2].input, EV_KEY, KEY_POWER, 0);
		input_sync(imapx15_gpio[2].input);
	}

	return 0;
}
#else

#define	imapx15_gpio_suspend	NULL
#define	imapx15_gpio_resume	NULL

#endif

static struct platform_driver imapx15_gpio_device_driver = {
	.probe = imapx15_gpio_probe,
	.remove = imapx15_gpio_remove,
	.suspend = imapx15_gpio_suspend,
	.resume = imapx15_gpio_resume,
	.driver = {
		   .name = "imap-gpio",
		   .owner = THIS_MODULE,
		   }
};

static int __init imapx15_gpio_init(void)
{
	return platform_driver_register(&imapx15_gpio_device_driver);
}

module_init(imapx15_gpio_init);

static void __exit imapx15_gpio_exit(void)
{
	platform_driver_unregister(&imapx15_gpio_device_driver);
}

module_exit(imapx15_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Keyboard driver for CPU GPIOs");
MODULE_ALIAS("platform:imap-gpio");
