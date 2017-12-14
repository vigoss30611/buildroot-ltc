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

//#include <common.h>
//#include <lowlevel_api.h>
//#include <asm/io.h>
#include <linux/types.h>
#include <bootlist.h>
#include <asm-arm/arch-apollo3/ssp.h>
#include <malloc.h>
#include <ssp.h>
#include <vstorage.h>
#include <asm-arm/io.h>
#include <bootlist.h>
//#include <asm/proc/ptrace.h>
#include <div64.h>
//#include <asm-arm/arch-imapx800/imapx_base_reg.h>
#include <asm-arm/arch-apollo3/imapx_a5timer.h>
#include <preloader.h>

#define module_get_clock irf->module_get_clock
#define readl(a)			(*(volatile unsigned int *)(a))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

/*
 * cotex-a5 g-timer, super ! 
 */
#define do_div(n,base) ({				\
		uint32_t __base = (base);			\
		uint32_t __rem; 				\
		(void)(((typeof((n)) *)0) == ((uint64_t *)0));	\
		if (((n) >> 32) == 0) { 		\
			__rem = (uint32_t)(n) % __base; 	\
			(n) = (uint32_t)(n) / __base;		\
		} else						\
			__rem = __div64_32(&(n), __base);	\
		__rem;						\
	 })
	
	/* Wrapper for do_div(). Doesn't modify dividend and returns
	 * the result, not reminder.
	 */
	static inline uint64_t lldiv(uint64_t dividend, uint32_t divisor)
	{
		uint64_t __res = dividend;
		do_div(__res, divisor);
		return(__res);
	}

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
/*
unsigned long get_timer(unsigned long base) {
	unsigned long long t = get_ticks();
	return lldiv(t, 1000) - base;
}
*/
ulong get_timer_masked(void)
{
	unsigned long long t = get_ticks();
	return lldiv(t, 1000);
}
