/*
 *  linux/arch/arm/mach-q3f/timer-cmn.c
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
#include <mach/power-gate.h>
#include <linux/timer-cmn1.h>

static void __iomem* cmn1_timer_base = NULL;
static unsigned long cmn1_timer_reload = 0;

struct cmn1_timer_event{
	void                    (*event_handler)(unsigned long);
    unsigned long           para;
#ifdef  TEST_CMN1_TIMER_TASKLET
    struct tasklet_struct   cmn1_tasklet;
    int                     tasklet_registed;
#endif
	struct list_head	        list;
};

static struct cmn1_timer_event cmn1_timer_isr_event = {
    .event_handler      = NULL,
#ifdef  TEST_CMN1_TIMER_TASKLET
    .tasklet_registed   = 0,
#endif
};

/*
 * IRQ handler for cmn timer 1
 */
static irqreturn_t cmn1_timer_interrupt(int irq, void *dev_id)
{
    struct cmn1_timer_event* event = (struct cmn1_timer_event*)dev_id;

	readl(cmn1_timer_base + TIMER_INTCLR);

    if(event->event_handler)
        event->event_handler(event->para);

#ifdef  TEST_CMN1_TIMER_TASKLET
    if(event->tasklet_registed)
        tasklet_schedule(&event->cmn1_tasklet);
#endif

	return IRQ_HANDLED;
}
static struct irqaction cmn1_timer_irq = {
	.name		= "cmn-timer.1",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= cmn1_timer_interrupt,
	.dev_id		= &cmn1_timer_isr_event,
};

static long cmn1_get_clock_rate(const char *name)
{
	long rate;
#if defined(CONFIG_APOLLO_FPGA_PLATFORM) || defined(CONFIG_Q3F_FPGA_PLATFORM) || defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	rate = CONFIG_FPGA_EXTEND_CLK;
	pr_err("Got cmn-timer rate: %ld\n", rate);
#else
	struct clk *clk;
	int err;
	clk = clk_get_sys("imap-cmn-timer.1" , "imap-cmn-timer");
	if (IS_ERR(clk)) {
		pr_err("cmn1: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}
	//err = clk_enable(clk);
	err = clk_prepare_enable(clk);
	if (err) {
		pr_err("cmn1: %s clock failed to enable: %d\n", name, err);
		clk_put(clk);
		return err;
	}
	rate = clk_get_rate(clk);
	if (rate < 0) {
		pr_err("cmn1: %s clock failed to get rate: %ld\n", name, rate);
		clk_disable(clk);
		clk_put(clk);
	}
	pr_err("Got cmn1-timer rate: %ld\n", rate);
#endif
	return rate;
}

#ifdef  TEST_CMN1_TIMER_TASKLET
/* * register and deregister IRQ handelr takslet function for cmn timer 1**/
void cmn1_timer_register_tasklet(void (*func)(unsigned long),unsigned long para)
{
    tasklet_init(&cmn1_timer_isr_event.cmn1_tasklet, func, para);
    cmn1_timer_isr_event.tasklet_registed = 1;
}
void cmn1_timer_deregister_tasklet(void)
{
    cmn1_timer_isr_event.tasklet_registed = 0;
    tasklet_kill(&cmn1_timer_isr_event.cmn1_tasklet);
}
#endif
/*
 * register IRQ handelr callback function for cmn timer 1
 */
long cmn1_timer_register_handler(void (*func)(unsigned long),unsigned long para)
{
    if(0 == cmn1_timer_base)
        return -1;

    cmn1_timer_isr_event.event_handler = func;
    cmn1_timer_isr_event.para = para;

    setup_irq(GIC_CMNT1_ID, &cmn1_timer_irq);
    return 0;
}
/*
 * deregister IRQ handelr callback function for cmn timer 1
 */
void cmn1_timer_deregister_handler(void)
{
    remove_irq(GIC_CMNT1_ID, &cmn1_timer_irq);
    cmn1_timer_isr_event.event_handler = NULL;
}
/*
 * Get irq number for cmn timer 1
 */
long cmn1_timer_get_irq(void)
{
    return GIC_CMNT1_ID;
}
/*
 * Clear timer 1 interrupt status register.
 */
void cmn1_timer_clear_int(void)
{
    readl(cmn1_timer_base + TIMER_INTCLR);
}
/*
 * set cmn timer 1 reload value, in msec.
 */
int cmn1_timer_set_period(long msec)
{
    long rate;
    
    if(0 == cmn1_timer_base)
        return -1;

    rate = cmn1_get_clock_rate("imap-cmn-timer1");
    if(rate <= 0)
        return -1;

    cmn1_timer_reload = (rate/1000)*msec;
    writel(cmn1_timer_reload, cmn1_timer_base + TIMER_LOAD);

    return 0;
}
/*
 * enable cmn timer 1
 */
int cmn1_timer_enable(void)
{
    if(0 == cmn1_timer_base)
        return -1;
    
    writel(cmn1_timer_reload, cmn1_timer_base + TIMER_LOAD);
    writel(3, cmn1_timer_base + TIMER_CTRL);
    return 0;
}
/*
 * disable cmn timer 1
 */
void cmn1_timer_disable(void)
{
    writel(0, cmn1_timer_base + TIMER_CTRL);
}
/*
 * init cmn timer 1
 */
void cmn1_timer_init(void)
{
    cmn1_timer_base = IO_ADDRESS(IMAP_TIMER_BASE)+0x14;
    writel(0, cmn1_timer_base + TIMER_CTRL);
}

