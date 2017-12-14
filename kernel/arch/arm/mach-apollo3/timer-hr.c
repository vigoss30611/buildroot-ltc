#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <asm/hardware/arm_timer.h>
#include <mach/timer-hr.h>
#include <mach/power-gate.h>

#define TIMER_LENGTH	8

struct clocksource_hr *imapx_hr = NULL;
struct clocksource_hr *swap_hr = NULL;
static spinlock_t lock;

static inline struct clocksource_hr *to_hr_clksrc(struct clocksource *c)
{
	return container_of(c, struct clocksource_hr, cs);
}

static cycle_t imapx_cal_fake_secs(struct clocksource *c)
{
	struct clocksource_hr *hr = to_hr_clksrc(c);
	uint32_t time, a, least;
	uint32_t b = msecs_to_jiffies(TIMER_LENGTH * 1000);

	time = jiffies;
	a = (time - hr->caltime.time) / b;
	if (a) {
		least = time - hr->caltime.time - a * b;
		least = (least > (hr->load - hr->caltime.subtotal)) ? 0 : -1;
		return a + least;
	} else
		return 0;
}

static cycle_t imapx_cal_least_ticks(struct clocksource *c)
{
	struct clocksource_hr *hr = to_hr_clksrc(c);
	uint32_t val = readl(hr->reg + TIMER_VALUE);
	uint32_t flag;

	flag = readl(hr->reg + TIMER_RIS);
	if (flag) {
		readl(hr->reg + TIMER_INTCLR);
		hr->fake_sec += (1 + imapx_cal_fake_secs(c));
		val = readl(hr->reg + TIMER_VALUE);
	}
	hr->caltime.time = jiffies;
	hr->caltime.subtotal = (cycle_t)hr->load - (cycle_t)val;

	return hr->caltime.subtotal;
}
cycle_t total_bak = 0;
static cycle_t imapx_hr_read(struct clocksource *c)
{
	struct clocksource_hr *hr = to_hr_clksrc(c);
	cycle_t total;
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);
	total = imapx_cal_least_ticks(c);
	total += (hr->fake_sec * (cycle_t)hr->load + hr->least);
	if (total_bak > total || (total - total_bak) > (cycle_t)hr->load) {
		printk("last 0x%llx, cur 0x%llx, sub %lld\n",
				total_bak, total, total - total_bak);
	}
	total_bak = total;
	spin_unlock_irqrestore(&lock, flags);
	return total;
}

static long  __get_clock_rate(const char *name)
{
	long rate;

#if !defined(CONFIG_IMAPX910_FPGA_PLATFORM)
	struct clk *clk;
	int err;

	clk = clk_get_sys("imap-cmn-timer.1" , "imap-cmn-timer");
	if (IS_ERR(clk)) {
		pr_err("gtimer: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	err  = clk_prepare_enable(clk);
	if (err) {
		pr_err("gtimer: %s clock failed to enable: %d\n", name, err);
		clk_put(clk);
		return err;
	}

	rate = clk_get_rate(clk);
	if (rate < 0) {
		pr_err("gtimer: %s clock failed to get rate: %ld\n",
			name, rate);
		clk_disable(clk);
		clk_put(clk);
	}
#else
	rate =  5000000;
#endif
	return rate;
}

static void imapx_hr_start(struct clocksource *c, uint32_t load)
{
	struct clocksource_hr *hr = to_hr_clksrc(c);
	/* disable cmn */
	writel(0, hr->reg + TIMER_CTRL);
	/* set auto-load*/
	writel(load, hr->reg + TIMER_LOAD);
	/* enable cmn */
	writel(0x3, hr->reg + TIMER_CTRL);
}

#ifdef CONFIG_PM
static void imapx_hr_suspend(struct clocksource *cs)
{
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);
	imapx_hr->least += imapx_cal_least_ticks(cs);
	spin_unlock_irqrestore(&lock, flags);
}

static void imapx_hr_resume(struct clocksource *cs)
{
	module_power_on(SYSMGR_CMNTIMER_BASE);
	imapx_hr_start(&imapx_hr->cs, imapx_hr->load);
}
#else
#define imapx_hr_suspend	NULL
#define imapx_hr_resume		NULL
#endif

int imapx_hr_init(void __iomem *reg, unsigned int irq,
		const char *name)
{
	long rate = __get_clock_rate(name);

	if (rate < 0)
		return -EFAULT;

	imapx_hr = kzalloc(sizeof(struct clocksource_hr), GFP_KERNEL);
	/* For fake hr timer when antutu */
	swap_hr = kzalloc(sizeof(struct clocksource_hr), GFP_KERNEL);
	if (!imapx_hr || !swap_hr)
		return -ENOMEM;
	spin_lock_init(&lock);
	imapx_hr->reg			= reg;
	imapx_hr->cs.name		= name;
	imapx_hr->cs.rating		= 300;
	imapx_hr->cs.mask		= CLOCKSOURCE_MASK(64);
	imapx_hr->cs.flags		= CLOCK_SOURCE_IS_CONTINUOUS;
	imapx_hr->cs.read		= imapx_hr_read;
	imapx_hr->cs.suspend	= imapx_hr_suspend;
	imapx_hr->cs.resume		= imapx_hr_resume;
	/* describe irq num */
	imapx_hr->fake_sec		= 0;
	/* record the time before suspend */
	imapx_hr->least			= 0;
	/* init time parameter */
	imapx_hr->caltime.time	= jiffies;
	imapx_hr->caltime.subtotal	= 0;
	imapx_hr->flags = HR_USED;
	if ((0xffffffff / TIMER_LENGTH) < rate) {
		pr_err("hr timer set too large\n");
		return -EFAULT;
	}
	/* set 8s to generate irq */
	imapx_hr->load			= rate * TIMER_LENGTH;
	memcpy(swap_hr, imapx_hr, sizeof(struct clocksource_hr));
	swap_hr->flags = HR_NOUSED;
	imapx_hr_start(&imapx_hr->cs, imapx_hr->load);
	pr_info("hr timer register\n");
	
	return clocksource_register_hz(&imapx_hr->cs, rate);
}

struct clocksource_hr *twist_swap_hr(void)
{
	struct clocksource_hr *fake_time = NULL;
	
	if (imapx_hr->flags == HR_USED) {
		memcpy(swap_hr, imapx_hr, sizeof(struct clocksource_hr));
		swap_hr->flags = HR_USED;
		imapx_hr->flags = HR_NOUSED;
		fake_time = swap_hr;
	} else if (swap_hr->flags == HR_USED) {
		memcpy(imapx_hr, swap_hr, sizeof(struct clocksource_hr));
		swap_hr->flags = HR_NOUSED;
		imapx_hr->flags = HR_USED;
		fake_time = imapx_hr;
	} else
		BUG();

	return fake_time;
}

void twist_imapx_hr(int p)
{
	struct clocksource_hr *fake_time;
	long rate;
	long long tmp;
	
	fake_time = twist_swap_hr();
	if(!fake_time)
		return;

	rate = fake_time->load / TIMER_LENGTH;

	tmp = rate * 100ll;
	do_div(tmp, p);
	rate = tmp;

	printk(KERN_ERR "%s: %d\n", __func__, rate);
	__clocksource_updatefreq_hz(&fake_time->cs, rate);
	if (fake_time->cs.mult == 0)
		BUG();
	timekeeping_notify(&fake_time->cs);
}

