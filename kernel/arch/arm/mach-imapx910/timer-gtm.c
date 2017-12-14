/*
 *  linux/arch/arm/mach-imapx910/timer-gtm.c
 *
 *  Copyright (C) 1999 - 2003 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ported from timer-sp.c: by warits Mar.13 2012
 */
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
#include <mach/timer-gtm.h>

#include <linux/cpufreq.h>
#include "core.h"

/*
 * [KP-sun] for support local timer
 * 1. rm old interface about cpufreq:
 *    ca5_register_freqfb, ca5_gtimer_cpufreq_deregister,
 *    ca5_gtimer_cpufreq_register, ca5_gtimer_cpufreq_transition,
 *    timers_update
 * 2. adjust interface for suitable new style about cpufreq:
 *    ca5_timer_update and imap_time_twist
 */

struct clocksource_gtm {
	void __iomem *reg;
	struct clocksource cs;
	struct notifier_block		freq_transition;
};

struct clocksource_gtm *imapx910_gtm;

static inline struct clocksource_gtm *to_imapx910_gtm(struct clocksource *c)
{
	return container_of(c, struct clocksource_gtm, cs);
}

static long __time_twist = 100;
static long  __get_clock_rate(const char *name)
{
	long rate;
	long long tmp;

#if !defined(CONFIG_IMAPX910_FPGA_PLATFORM)
	struct clk *clk;
	int err;

	clk = clk_get(NULL, "gtm-clk");
	if (IS_ERR(clk)) {
		pr_err("gtimer: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	err = clk_enable(clk);
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

	tmp = rate * 100ll;
	do_div(tmp, __time_twist);
	return tmp;
}

static cycle_t imapx910_gtimer_read(struct clocksource *c)
{
	unsigned long long  l, h, t;

	/* read counter value */
	t = readl(to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_H);
	l = readl(to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_L);

	/* incase h32 changed */
	h = readl(to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_H);
	if (h != t)
		l = readl(to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_L);

	return h << 32 | l;
}

static void  imapx910_gtimer_start(struct clocksource *c, cycle_t n)
{
	/* stop timer */
	writel(0, to_imapx910_gtm(c)->reg + G_TIMER_CONTROL);

	/* reset counter value */
	writel((u32)(n & 0xffffffffull),
			to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_L);
	writel((u32)(n >> 32), to_imapx910_gtm(c)->reg + G_TIMER_COUNTER_H);

	/* enable timer
	 * here we use CPU_PERIPHYCIAL_CLOCK(CPC) directly without
	 * any prescaler. CPC is 100MHz approximately, thus 5,000
	 * years is need to make this timer exceed.
	 */
	writel(1, to_imapx910_gtm(c)->reg + G_TIMER_CONTROL);
}

#ifdef CONFIG_PM
static cycle_t imapx910_gtm_cycles;

static void imapx910_gtimer_suspend(struct clocksource *cs)
{
	imapx910_gtm_cycles = imapx910_gtimer_read(cs);
}

static void imapx910_gtimer_resume(struct clocksource *cs)
{
	imapx910_gtimer_start(cs, imapx910_gtm_cycles);
}
#endif

#ifdef CONFIG_CPU_FREQ
void imapx910_gtimer_update(long rate)
{
	__clocksource_updatefreq_hz(&imapx910_gtm->cs, rate);
	if (imapx910_gtm->cs.mult == 0)
		BUG();
	timekeeping_notify(&imapx910_gtm->cs);
}
EXPORT_SYMBOL(imapx910_gtimer_update);

void imapx910_time_twist(int t)
{
	long rate;
	if (t < 10 || t > 200) {
		pr_err("<<< time twist region [10, 200]\n");
		return;
	}
	__time_twist = t;

	rate = __get_clock_rate(imapx910_gtm->cs.name);
	imapx910_gtimer_update(rate);
	imapx910_cmn_timer_update(rate);
}
EXPORT_SYMBOL(imapx910_time_twist);
#endif

int imapx910_gtimer_init(void __iomem *reg, const char *name)
{
	long rate = __get_clock_rate(name);

	if (rate < 0)
		return -EFAULT;

	imapx910_gtm = kzalloc(sizeof(struct clocksource_gtm), GFP_KERNEL);
	if (!imapx910_gtm)
		return -ENOMEM;

	imapx910_gtm->reg       = reg;
	imapx910_gtm->cs.name   = name;
	imapx910_gtm->cs.rating = 300;
	imapx910_gtm->cs.mask   = CLOCKSOURCE_MASK(64);
	imapx910_gtm->cs.flags  = CLOCK_SOURCE_IS_CONTINUOUS;
	imapx910_gtm->cs.read   = imapx910_gtimer_read;
	imapx910_gtm->cs.suspend = imapx910_gtimer_suspend;
	imapx910_gtm->cs.resume  = imapx910_gtimer_resume;

	/* start at count zero */
	imapx910_gtimer_start(&imapx910_gtm->cs, 0);

	core_msg("%s registered\n", name);
	return clocksource_register_hz(&imapx910_gtm->cs, rate);
}

