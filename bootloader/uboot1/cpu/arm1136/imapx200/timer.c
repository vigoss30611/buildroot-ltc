/***************************************************************************** 
** cpu/arm1136/imapx200/timer.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: u-boot timer functions for iMAPx200.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/19/2009 XXX	Initialized by Warits
*****************************************************************************/

#include <common.h>
#include <asm/io.h>
#include <oem_func.h>

#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEM_POOL)
#include <asm/proc/ptrace.h>
#include <div64.h>
#else
#	ifndef CONFIG_SYS_IMAPX200_APLL
#		define CONFIG_SYS_IMAPX200_APLL		(0x80000041)
#	endif
#	ifndef CONFIG_SYS_IMAPX200_DCFG0
#		define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	endif
#	define spl_fclk	\
		(((CONFIG_SYS_CLK_FREQ / 1000) * (	\
		2*((CONFIG_SYS_IMAPX200_APLL & 0x7f) + 1))) / (		\
		(((CONFIG_SYS_IMAPX200_APLL >> 7) & 0x1f) + 1) * (	\
		1 << ((CONFIG_SYS_IMAPX200_APLL >> 12) & 0x3)))	* 1000)
#	define spl_hclk	\
		(spl_fclk / ((CONFIG_SYS_IMAPX200_DCFG0 >> 4) & 0xf))

#	define spl_pclk	\
		spl_hclk / (1 << ((CONFIG_SYS_IMAPX200_DCFG0 >> 16) & 0x3))

#	define spl_prescaler	\
		(spl_pclk / (8 * 100000) - 1)
#endif


/*
 * CTM have a very fast freq.(to fast? we r still TBC..), so PWM is used
 * here as u-boot timer
 * iMAPx200 have 5x16bit PWM timers(0, 1, 2, 3, 4), timer4 is used
 */

static const ushort wrap_count = 0xffff;
/* Last decremeneter snapshot */
static ulong prescaler, lastdec;
/* Monotonic incrementing timer */
static unsigned long long timestamp;

static inline ulong read_timer(void)
{
	/* FIXME: The register is 16 bit, so I use readw to access */
	return readw(TCNTO4);
}

int timer_init(void)
{
	/* Adjust prescaler to 1 tick 1 us(div = 1/2) */
	ulong tcfg;

#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEM_POOL)
	prescaler = get_PCLK() / (8 * 100000) - 1;
#else
	prescaler = spl_prescaler;
#endif
	if (prescaler > 255) prescaler = 255;
	tcfg = readl(TCFG0) & ~(0xff << 8);
	tcfg |= prescaler << 8;
	writel(tcfg, TCFG0);

	/*
	 * Adjust timer4 divider and wrap_count, after prescaler configuration
	 * timer4 wraps about 60ms
	 */
	tcfg = (readl(TCFG1) & ~0xf0000) | 0xa0000; /* divider set to 1/2 */
	writel(tcfg, TCFG1);

	/* set reload value */
	writew(wrap_count, TCNTB4);

	tcfg = readl(TCON) & ~0x00700000;
	tcfg |= (1 << 22);		/* auto reload */
	tcfg |= (1 << 21);		/* update value */
	writel(tcfg, TCON);

	/* start timer */
	tcfg = readl(TCON) & ~0x00700000;
	tcfg |= (1 << 22);		/* auto reload */
	tcfg |= (1 << 20);		/* start */
	writel(tcfg, TCON);
	
	lastdec = wrap_count;
	timestamp = 0;

	return 0;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	ulong now = read_timer();

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + wrap_count - now;
	}
	lastdec = now;

	return timestamp;
}

#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEM_POOL)
/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return get_PCLK() / (2 * (prescaler + 1));
}

void reset_timer_masked(void)
{
	/* reset time */
	lastdec = read_timer();
	timestamp = 0;
	return ;
}

void reset_timer(void)
{
	reset_timer_masked();
	return ;
}

ulong get_timer_masked(void)
{
	return get_ticks();
}

ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

void set_timer(ulong t)
{
	timestamp = t;
}
#endif	/* !CONFIG_NAND_SPL */

void udelay(ulong us)
{
	unsigned long long tmp;

/*
#ifndef CONFIG_NAND_SPL
	tmp = get_ticks() + (us / 10 + 1);
#else
	tmp = get_ticks() + ((us >> 3) + 1);
#endif
*/
#if defined(CONFIG_NAND_SPL) || defined(CONFIG_MEM_POOL)
	tmp = get_ticks() + ((us >> 3) + 1);
#else
	tmp = get_ticks() + (us / 10 + 1);
#endif


	while (get_ticks() < tmp);
	return ;
}
