/*#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <asm/uaccess.h>*/

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/interrupt.h>
#include <mach/pad.h>
#include <mach/hw_cfg.h>

#include <mach/gpio.h>
#include <linux/delay.h>
#include <mach/imap-rtc.h>
#include <mach/items.h>

struct work_struct bt_work;

void enable_32k_rtc(void)
{
        int tmp = 0x0;
        //32k
        void __iomem *rtc_sys = IO_ADDRESS(SYSMGR_RTC_BASE);

        tmp = __raw_readl(rtc_sys + SYS_RTC_IO_ENABLE);
        tmp &= ~(0x1<<5);
        __raw_writel(tmp,  rtc_sys + SYS_RTC_IO_ENABLE);

        tmp = __raw_readl(rtc_sys + SYS_IO_CFG2);
        tmp |= (0x1<<4);
        __raw_writel(tmp,  rtc_sys + SYS_IO_CFG2);
     //   msleep(5);
        printk("enable_32k_rtc \n");
}



void  bcm6212_gpio_init()
{
    //int bt_enable, bt_reset, error;
    int bt_reset, error;

/*    if (item_exist("bt.enable")) {
        bt_enable = item_integer("bt.enable", 0);
    }
    else{
    	printk("not find bt.enable in item\n");
	return -1;
    }*/
    if (item_exist("bt.reset")) {
       bt_reset = item_integer("bt.reset", 0);
    }
    else{
    	printk("not find bt.reset in item\n");
	return;
    }

   /* error = gpio_request(bt_enable, "bcm6212");
    if(error){
	pr_err("failed request enable gpio for bcm6212\n");
    return -1;
    } else {
	gpio_direction_output(bt_enable, 0);
	gpio_set_value(bt_enable, 0);
    }
*/
    error = gpio_request(bt_reset, "bcm6212");
    if(error){
	pr_err("failed request reset gpio for bcm6212\n");
  //  return -1;
    } else {
/*	gpio_direction_output(bt_reset, 0);
	gpio_set_value(bt_reset, 0);
	mdelay(50);
	gpio_set_value(bt_reset, 1);
	mdelay(50);*/
    }

    gpio_direction_output(bt_reset, 0);
  //gpio_set_value(bt_reset, 0);
    msleep(100);
    gpio_set_value(bt_reset, 1);
    msleep(200);
    pr_err("bcm6212 bt gpio change \n");

   //gpio_free(bt_enable);
   //imapx_pad_set_mode(bt_enable, "function");

    return;
}

static void bcm6212_init_work(struct work_struct *work){
	enable_32k_rtc();
	bcm6212_gpio_init();
}

static int bcm6212_probe(struct platform_device *pdev)
{

    INIT_WORK(&bt_work, bcm6212_init_work);
    schedule_work(&bt_work);
    return 0;
}

static int bcm6212_remove(struct platform_device *pdev)
{
    return 0;
}

static int bcm6212_suspend(struct platform_device *pdev, pm_message_t state)
{
    return 0;
}

static int bcm6212_resume(struct platform_device *pdev)
{
    return 0;
}

static struct platform_driver bcm6212_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "bcm6212",
    },
    .probe = bcm6212_probe,
    .remove = bcm6212_remove,
    .suspend = bcm6212_suspend, 
    .resume = bcm6212_resume,
};

static int __init led_init(void)
{
    return platform_driver_register(&bcm6212_driver);
}

static void __exit led_exit(void)
{
    platform_driver_unregister(&bcm6212_driver);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhifeng Huang");
MODULE_DESCRIPTION("LED driver on IMAPX platform");
