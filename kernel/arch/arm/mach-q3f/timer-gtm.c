/*
 *  linux/arch/arm/mach-q3f/timer-gtm.c
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
	int flags;
};

enum swap_status {
	HR_NOUSED,  
	HR_USED,    
};              

struct clocksource_gtm *q3f_gtm;
struct clocksource_gtm *swap_gtm;

static inline struct clocksource_gtm *to_q3f_gtm(struct clocksource *c)
{
	return container_of(c, struct clocksource_gtm, cs);
}

static long __time_twist = 100;
static long  __get_clock_rate(const char *name)
{
	long rate;
	long long tmp;

#if defined(CONFIG_APOLLO_FPGA_PLATFORM)|| defined(CONFIG_Q3F_FPGA_PLATFORM) 
	rate =  CONFIG_FPGA_CPU_CLK;
	pr_err("Got ca5-timer rate: %ld\n", rate);
#else
	struct clk *clk;
	int err;

	clk = clk_get_sys("gtm-clk", NULL);
	if (IS_ERR(clk)) {
		pr_err("gtimer: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	err = clk_prepare_enable(clk);
	if (err) {
		pr_err("gtimer: %s clock failed to enable: %d\n", name, err);
		clk_put(clk);
		return err;
	}

	rate = clk_get_rate(clk);
	if (rate < 0) {
		pr_err("gtimer: %s clock failed to get rate: %ld\n",
			name, rate);
		clk_disable_unprepare(clk);
		clk_put(clk);
	}
#endif

	tmp = rate * 100ll;
	do_div(tmp, __time_twist);
	return tmp;
}

static cycle_t q3f_gtimer_read(struct clocksource *c)
{
	unsigned long long  l, h, t;

	/* read counter value */
	t = readl(to_q3f_gtm(c)->reg + G_TIMER_COUNTER_H);
	l = readl(to_q3f_gtm(c)->reg + G_TIMER_COUNTER_L);

	/* incase h32 changed */
	h = readl(to_q3f_gtm(c)->reg + G_TIMER_COUNTER_H);
	if (h != t)
		l = readl(to_q3f_gtm(c)->reg + G_TIMER_COUNTER_L);

	return h << 32 | l;
}

static void  q3f_gtimer_start(struct clocksource *c, cycle_t n)
{
	/* stop timer */
	writel(0, to_q3f_gtm(c)->reg + G_TIMER_CONTROL);

	/* reset counter value */
	writel((u32)(n & 0xffffffffull),
			to_q3f_gtm(c)->reg + G_TIMER_COUNTER_L);
	writel((u32)(n >> 32), to_q3f_gtm(c)->reg + G_TIMER_COUNTER_H);

	/* enable timer
	 * here we use CPU_PERIPHYCIAL_CLOCK(CPC) directly without
	 * any prescaler. CPC is 100MHz approximately, thus 5,000
	 * years is need to make this timer exceed.
	 */
	writel(1, to_q3f_gtm(c)->reg + G_TIMER_CONTROL);
}

#ifdef CONFIG_PM
static cycle_t q3f_gtm_cycles;

static void q3f_gtimer_suspend(struct clocksource *cs)
{
	q3f_gtm_cycles = q3f_gtimer_read(cs);
}

static void q3f_gtimer_resume(struct clocksource *cs)
{
	q3f_gtimer_start(cs, q3f_gtm_cycles);
}
#endif

#ifdef CONFIG_CPU_FREQ
struct clocksource_gtm *q3f_swap_gtm(void)
{
	struct clocksource_gtm *fake_time = NULL;
	
	if (q3f_gtm->flags == HR_USED) {
		memcpy(swap_gtm, q3f_gtm, sizeof(struct clocksource_gtm));
		swap_gtm->flags = HR_USED;
		q3f_gtm->flags = HR_NOUSED;
		fake_time = swap_gtm;
	} else if (swap_gtm->flags == HR_USED) {
		memcpy(q3f_gtm, swap_gtm, sizeof(struct clocksource_gtm));
		swap_gtm->flags = HR_NOUSED;
		q3f_gtm->flags = HR_USED;
		fake_time = q3f_gtm;
	} else
		BUG();

	return fake_time;
}

void q3f_gtimer_update(long rate)
{
	struct clocksource_gtm *fake_time;

	fake_time = q3f_swap_gtm();
	if (!fake_time)
		return;

	__clocksource_updatefreq_hz(&fake_time->cs, rate);
	if (fake_time->cs.mult == 0)
		BUG();
	timekeeping_notify(&fake_time->cs);
}
EXPORT_SYMBOL(q3f_gtimer_update);

void q3f_time_twist(int t)
{
	long rate;
	if (t < 10 || t > 200) {
		pr_err("<<< time twist region [10, 200]\n");
		return;
	}
	__time_twist = t;

	rate = __get_clock_rate(q3f_gtm->cs.name);
	q3f_gtimer_update(rate);
	q3f_cmn_timer_update(rate);
}
EXPORT_SYMBOL(q3f_time_twist);
#endif

int q3f_gtimer_init(void __iomem *reg, const char *name)
{
	long rate = __get_clock_rate(name);

	if (rate < 0)
		return -EFAULT;

	q3f_gtm = kzalloc(sizeof(struct clocksource_gtm), GFP_KERNEL);
	swap_gtm = kzalloc(sizeof(struct clocksource_gtm), GFP_KERNEL);
	if (!q3f_gtm || !swap_gtm)
		return -ENOMEM;
	q3f_gtm->reg       = reg;
	q3f_gtm->cs.name   = name;
	q3f_gtm->cs.rating = 300;
	q3f_gtm->cs.mask   = CLOCKSOURCE_MASK(64);
	q3f_gtm->cs.flags  = CLOCK_SOURCE_IS_CONTINUOUS;
	q3f_gtm->cs.read   = q3f_gtimer_read;
#ifdef CONFIG_PM
	q3f_gtm->cs.suspend = q3f_gtimer_suspend;
	q3f_gtm->cs.resume  = q3f_gtimer_resume;
#endif
	q3f_gtm->flags		= HR_USED;
	
	memcpy(swap_gtm, q3f_gtm, sizeof(struct clocksource_gtm));
	swap_gtm->flags = HR_NOUSED;
	/* start at count zero */
	q3f_gtimer_start(&q3f_gtm->cs, 0);

	core_msg("%s registered\n", name);
	return clocksource_register_hz(&q3f_gtm->cs, rate);
}

