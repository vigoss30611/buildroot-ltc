/***************************************************************************** 
** driver/timer/timer.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: cotex-a5 g-timer functions for iMAPx800.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 08/23/2011 XXX	Initialized by Warits
*****************************************************************************/

#include <common.h>
#include <lowlevel_api.h>
#include <asm/io.h>

#include <asm/proc/ptrace.h>
#include <div64.h>

/*
 * cotex-a5 g-timer, super ! 
 */

int init_timer(void)
{
	uint32_t prescaler;

	/* calc prescaler, 1 ticks equals to 1 us */
	prescaler = module_get_clock(CPUP_CLK_BASE) / 1000000 - 1;
	prescaler <<= 8;

	/* stop timer */
	writel(0, GTIMER_CTLR);

	/* reset counter value */
	writel(0, GTIMER_CNTR_LOWER);
	writel(0, GTIMER_CNTR_UPPER);

	/* enable timer */
	writel(prescaler | 1, GTIMER_CTLR);

	return 0;
}

unsigned long long get_ticks(void)
{
	unsigned long long  lower, upper, tmp;

	/* read the upper 32-bit */
	tmp = readl(GTIMER_CNTR_UPPER);

	/* read the lower 32-bit */
	lower = readl(GTIMER_CNTR_LOWER);

	/* incase upper changed */
	if((upper = readl(GTIMER_CNTR_UPPER))
				!= tmp)
	  lower = readl(GTIMER_CNTR_LOWER);

	return (upper << 32 | lower);

}

void udelay(unsigned long us)
{
	unsigned long long t = get_ticks();
	while (get_ticks() < t + us);
}

unsigned long get_timer(unsigned long base) {
	unsigned long long t = get_ticks();
	return lldiv(t, 1000) - base;
}

ulong get_timer_masked(void)
{
	unsigned long long t = get_ticks();
	return lldiv(t, 1000);
}
