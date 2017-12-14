/* 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/of.h>
#include "imapx_wdt.h"

static bool nowayout	= WATCHDOG_NOWAYOUT;
static int tmr_margin = 10000;
static int tmr_atboot	= 0;

module_param(tmr_margin,  int, 0);
module_param(tmr_atboot,  int, 0);
module_param(nowayout,   bool, 0);

MODULE_PARM_DESC(tmr_margin, "Watchdog tmr_margin in seconds. (default="
		__MODULE_STRING(WATCHDOG_TIMEOUT) ")");
MODULE_PARM_DESC(tmr_atboot,
		"Watchdog is started at boot time if set to 1, default="
			__MODULE_STRING(0));
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
			__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");


struct imapx_wdt_chip {
	struct watchdog_device	wdd;
	struct device	*wdt_dev;
	struct clk	*wdt_clock;
	unsigned long	clk_rate;
	void __iomem	*wdt_base;
};

static DEFINE_SPINLOCK(wdt_lock);

/* functions */
static inline struct imapx_wdt_chip *to_imapx_wdt_chip(struct watchdog_device *wdd)
{
	return container_of(wdd, struct imapx_wdt_chip, wdd);
}

static inline u32 wdt_readl(struct imapx_wdt_chip *chip, int offset)
{
	return readl(chip->wdt_base + offset);
}

static inline void wdt_writel(struct imapx_wdt_chip *chip, int offset, u32 val)
{
	writel(val, chip->wdt_base + offset);
}

static int imapx_wdt_keepalive(struct watchdog_device *wdd)
{
	struct imapx_wdt_chip *pc = to_imapx_wdt_chip(wdd);
	spin_lock(&wdt_lock);
	wdt_writel(pc, WDT_CRR, WDT_RESTART_DEFAULT_VALUE);
	spin_unlock(&wdt_lock);
	return 0;
}

static int imapx_wdt_stop(struct watchdog_device *wdd)
{
	unsigned long wtcon;
	struct imapx_wdt_chip *pc = to_imapx_wdt_chip(wdd);
	spin_lock(&wdt_lock);
	wtcon = wdt_readl(pc, WDT_CR);
	wtcon &= ~(ENABLE);
	wdt_writel(pc, WDT_CR, wtcon);
	spin_unlock(&wdt_lock);
	return 0;
}

static int imapx_wdt_start(struct watchdog_device *wdd)
{
	unsigned long wtcon;
	struct imapx_wdt_chip *pc = to_imapx_wdt_chip(wdd);
	spin_lock(&wdt_lock);

	imapx_wdt_stop(wdd);

	wtcon = wdt_readl(pc, WDT_CR);
	wtcon |= ENABLE;	
	wdt_writel(pc, WDT_CR, wtcon);
	spin_unlock(&wdt_lock);
	
	return 0;
}

static int imapx_wdt_set_heartbeat(struct watchdog_device *wdd, unsigned timeout)
{
	struct imapx_wdt_chip *pc = to_imapx_wdt_chip(wdd);
	unsigned long freq = pc->clk_rate;
	
	unsigned long count,t;
	int wdt_count = 0;
	if (timeout < 1)
		return -EINVAL;

	freq /= 1000;
	count = timeout * freq;

	while(count >>= 1)
        wdt_count++;
    wdt_count++;  //for greater than what you set time
    if (wdt_count < 16)
        wdt_count = 0;
    if (wdt_count > 31)
        wdt_count = 15;
    else 
        wdt_count -= 16;

	wdt_writel(pc, WDT_TORR, wdt_count);
	wdt_writel(pc, WDT_CRR, WDT_RESTART_DEFAULT_VALUE);

	t = (1<<(16+wdt_count));

	wdd->timeout = t/freq;
	return 0;
}

static unsigned int imapx_wdt_get_status(struct watchdog_device *wdd)
{
	struct imapx_wdt_chip *pc = to_imapx_wdt_chip(wdd);
	return wdt_readl(pc, WDT_CCVR);
}

#define OPTIONS (WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE)

static const struct watchdog_info imapx_wdt_ident = {
	.options          =     OPTIONS,
	.firmware_version =	0,
	.identity         =	"imapx Watchdog",
};

static struct watchdog_ops imapx_wdt_ops = {
	.owner = THIS_MODULE,
	.start = imapx_wdt_start,
	.stop = imapx_wdt_stop,
	.ping = imapx_wdt_keepalive,
	.set_timeout = imapx_wdt_set_heartbeat,
	.status = imapx_wdt_get_status,
};

/* interrupt handler code */

static irqreturn_t imapx_wdt_irq(int irqno, void *param)
{
	struct imapx_wdt_chip *pc = param;
	unsigned int wtcon;
	wtcon = wdt_readl(pc, WDT_STAT);
	if (wtcon){
		pr_info("watchdog timer expired (irq)\n");
		wdt_readl(pc, WDT_EOI);
		}
	
	return IRQ_HANDLED;
}

static int imapx_wdt_probe(struct platform_device *pdev)
{
	struct imapx_wdt_chip *wdt;
	struct resource	*wdt_mem;
	struct resource	*wdt_irq;
	int started = 0;
	int ret;

	module_power_on(SYSMGR_PWDT_BASE);

	wdt = devm_kzalloc(&pdev->dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	wdt->wdt_dev = &pdev->dev;
	wdt->wdd.info = &imapx_wdt_ident;
	wdt->wdd.ops = &imapx_wdt_ops,
	wdt->wdd.timeout = WATCHDOG_TIMEOUT;

	platform_set_drvdata(pdev, wdt);

	wdt_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (wdt_mem == NULL) {
		pr_err("no memory resource specified\n");
		return -ENOENT;
	}

	wdt_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (wdt_irq == NULL) {
		pr_err("no irq resource specified\n");
		ret = -ENOENT;
		goto err;
	}

	/* get the memory region for the watchdog timer */
	wdt->wdt_base = devm_ioremap_resource(wdt->wdt_dev, wdt_mem);
	if (IS_ERR(wdt->wdt_base)) {
		ret = PTR_ERR(wdt->wdt_base);
		goto err;
	}

	imapx_wdt_stop(&wdt->wdd);
	
	ret = readl(IO_ADDRESS(IMAP_SYSMGR_BASE+0xac08));
    ret |= (0x1 << 4);
    writel(ret, IO_ADDRESS(IMAP_SYSMGR_BASE+0xac08));

	wdt_writel(wdt, WDT_CRR, WDT_RESTART_DEFAULT_VALUE);
	
	wdt->wdt_clock = clk_get_sys("imap-wtd",NULL);
	if (IS_ERR(wdt->wdt_clock)) {
		pr_err("failed to find watchdog clock source\n");
		ret = PTR_ERR(wdt->wdt_clock);
		goto err;
	}

	wdt->clk_rate = clk_get_rate(wdt->wdt_clock);
	
	ret = clk_prepare_enable(wdt->wdt_clock);
	if(ret < 0) {
		pr_err("failed to enable watchdog clock\n");
		return ret;
	}
	
	/* see if we can actually set the requested timer margin, and if
	 * not, try the default value */

	watchdog_init_timeout(&wdt->wdd, tmr_margin,  &pdev->dev);
	
	if (imapx_wdt_set_heartbeat(&wdt->wdd, wdt->wdd.timeout)) {
		started = imapx_wdt_set_heartbeat(&wdt->wdd, WATCHDOG_TIMEOUT);

		if (started == 0)
			pr_info("tmr_margin value out of range, default %d used\n",
			       WATCHDOG_TIMEOUT);
		else
			pr_info("default timer value is out of range, cannot start\n");
	}

	ret = devm_request_irq(wdt->wdt_dev, wdt_irq->start, imapx_wdt_irq, 0,
				pdev->name, wdt);
	if (ret != 0) {
		pr_err("failed to install irq (%d)\n", ret);
		goto err;
	}

	watchdog_set_nowayout(&wdt->wdd, nowayout);

	ret = watchdog_register_device(&wdt->wdd);
	if (ret) {
		pr_info("cannot register watchdog (%d)\n", ret);
		goto err;
	}

	if (tmr_atboot && started == 0) {
		pr_info("starting watchdog timer\n");
		imapx_wdt_start(&wdt->wdd);
	} else if (!tmr_atboot) {
		/* if we're not enabling the watchdog, then ensure it is
		 * disabled if it has been left running from the bootloader
		 * or other source */
		imapx_wdt_stop(&wdt->wdd);
	}
	return 0;
 err:
	wdt_irq = NULL;
	wdt_mem = NULL;
	return ret;
}

static int imapx_wdt_remove(struct platform_device *dev)
{
	struct imapx_wdt_chip *pc = platform_get_drvdata(dev);
	
	watchdog_unregister_device(&pc->wdd);

	clk_disable_unprepare(pc->wdt_clock);
	return 0;
}

static void imapx_wdt_shutdown(struct platform_device *dev)
{
	struct imapx_wdt_chip *pc = platform_get_drvdata(dev);
	imapx_wdt_stop(&pc->wdd);
}

#ifdef CONFIG_PM

static unsigned long wtcon_save;
static unsigned long wtdat_save;

static int imapx_wdt_suspend(struct platform_device *dev, pm_message_t state)
{
	/* Save watchdog state, and turn it off. */
	struct imapx_wdt_chip *pc = platform_get_drvdata(dev);
	
	wtcon_save = wdt_readl(pc, WDT_CR);
	wtdat_save = wdt_readl(pc, WDT_TORR);

	/* Note that WTCNT doesn't need to be saved. */
	imapx_wdt_stop(&pc->wdd);
	return 0;
}

static int imapx_wdt_resume(struct platform_device *dev)
{
	/* Restore watchdog state. */
	struct imapx_wdt_chip *pc = platform_get_drvdata(dev);
	wdt_writel(pc, WDT_TORR, wtdat_save);
	wdt_writel(pc, WDT_CR, wtcon_save);
	return 0;
}

#else
#define imapx_wdt_suspend NULL
#define imapx_wdt_resume  NULL
#endif /* CONFIG_PM */


static struct platform_driver imapx_wdt_driver = {
	.probe		= imapx_wdt_probe,
	.remove		= imapx_wdt_remove,
	.shutdown	= imapx_wdt_shutdown,
	.suspend	= imapx_wdt_suspend,
	.resume		= imapx_wdt_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "imap-wdt",
	},
};
static int __init imapx_wdt_init(void)
{
	pr_info("***Imapx watchdog init!***\n");
	return platform_driver_register(&imapx_wdt_driver);
}
subsys_initcall(imapx_wdt_init);


MODULE_AUTHOR("Infotm");
MODULE_DESCRIPTION("imapx Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS("platform:imap-wdt");
