/*
 *  linux/arch/arm/mach-imapx15/timer-cmn.c
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
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/module.h>
#include <asm/hardware/arm_timer.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include "core.h"

static long imapx15_cmn_get_clock_rate(const char *name)
{
	long rate;
#if !defined(CONFIG_IMAPX15_FPGA_PLATFORM)
	struct clk *clk;
	int err;
	clk = clk_get_sys("imap-cmn-timer.0" , "imap-cmn-timer");
	if (IS_ERR(clk)) {
		pr_err("cmn: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}
	//err = clk_enable(clk);
	err = clk_prepare_enable(clk);
	if (err) {
		pr_err("cmn: %s clock failed to enable: %d\n", name, err);
		clk_put(clk);
		return err;
	}
	rate = clk_get_rate(clk);
	if (rate < 0) {
		pr_err("cmn: %s clock failed to get rate: %ld\n", name, rate);
		clk_disable(clk);
		clk_put(clk);
	}
	pr_err("Got cmn-timer rate: %ld\n", rate);
#else
	rate = CONFIG_FPGA_CPU_CLK;
#endif
	return rate;
}

static void __iomem *clkevt_base;
static unsigned long clkevt_reload;

/*
 * IRQ handler for the timer
 */
static irqreturn_t imapx15_cmn_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = dev_id;
	/* clear the interrupt */
	readl(clkevt_base + TIMER_INTCLR);

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

int cmn_enabled = -1;

static void imapx15_cmn_set_mode(enum clock_event_mode mode,
	struct clock_event_device *evt)
{
	/* disable timer */
	writel(0, clkevt_base + TIMER_CTRL);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		writel(clkevt_reload, clkevt_base + TIMER_LOAD);

		/* auto-reload & enable */
		writel(0x3, clkevt_base + TIMER_CTRL);
		cmn_enabled = 1;
		break;

	case CLOCK_EVT_MODE_ONESHOT:
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	default:
		pr_err("common timer is disabled. %d\n", mode);
		cmn_enabled = 0;
		break;
	}

}

static struct clock_event_device cmn_clockevent = {
	.shift		= 32,
	.features       = CLOCK_EVT_FEAT_PERIODIC,
	.set_mode	= imapx15_cmn_set_mode,
	.rating		= 250,
	.cpumask	= cpu_all_mask,
};

static struct irqaction cmn_timer_irq = {
	.name		= "cmn-timer0",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= imapx15_cmn_timer_interrupt,
	.dev_id		= &cmn_clockevent,
};

void __init imapx15_cmn_timer_init(void __iomem *base, unsigned int irq,
	const char *name)
{
	struct clock_event_device *evt = &cmn_clockevent;
	long rate = imapx15_cmn_get_clock_rate(name);

	if (rate < 0)
		return;

	clkevt_base = base;
	clkevt_reload = DIV_ROUND_CLOSEST(rate, HZ);

	evt->name = name;
	evt->irq = irq;
	evt->mult = div_sc(rate, NSEC_PER_SEC, evt->shift);
	evt->max_delta_ns = clockevent_delta2ns(0xffffffff, evt);
	evt->min_delta_ns = clockevent_delta2ns(0xf, evt);

	setup_irq(irq, &cmn_timer_irq);

	clockevents_register_device(evt);
	core_msg("timer-cmn registered.\n");
}

#ifdef CONFIG_CPU_FREQ

void imapx15_cmn_timer_update(long x)
{
	long rate = imapx15_cmn_get_clock_rate(cmn_clockevent.name);
	clkevt_reload = DIV_ROUND_CLOSEST(rate, HZ);
	cmn_clockevent.mult = div_sc(rate, NSEC_PER_SEC, cmn_clockevent.shift);
	if (cmn_enabled == 1)
		imapx15_cmn_set_mode(CLOCK_EVT_MODE_PERIODIC, &cmn_clockevent);
}
#endif

int imapx15_cmn_timer_resume(void)
{
	imapx15_cmn_set_mode(CLOCK_EVT_MODE_PERIODIC, NULL);
	return 0;
}

