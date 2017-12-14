/* linux/arch/arm/mach-imapx9/common.c
 *
 *  Base function of this platform
 *
 * Copyright (c) 2013 Infotm ic  Co., Ltd.
 *      http://www.infotmic.com.cn/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <linux/kernel.h>
#include <linux/io.h>
#include <mach/imap-iomap.h>
#include <asm/irq.h>
#include <asm/smp_twd.h>
#include <asm/hardware/cache-l2x0.h>
#include "core.h"


void imapx910_power_off(void)
{
	a9pv10_poweroff();
	return;
}

void imapx910_enter_lowpower(void)
{
	a9pv10_enter_lowpower();
	return;
}

void __init imapx910_init_l2x0(void)
{
#ifdef CONFIG_CACHE_L2X0
	int lt, ld;
	void __iomem *base = IO_ADDRESS(IMAP_SCU_BASE) + 0x2000;

	/* 256KB (16KB/way), 16way associativity,
	 * evmon/parity/share enabled
	 * Bits:  .... ...0 0111 0011 0000 .... .... .... */
	writel(0x2, IO_ADDRESS(IMAP_SYSMGR_BASE + 0x884c));

	lt  = readl_relaxed(base + L2X0_TAG_LATENCY_CTRL);
	lt &= ~(0x7<<8 | 0x7<<4 | 0x7);
	lt |=  (0x1<<8 | 0x3<<4 | 0x1);
	writel_relaxed(lt, base + L2X0_TAG_LATENCY_CTRL);

	ld  = readl_relaxed(base + L2X0_DATA_LATENCY_CTRL);
	ld &= ~(0x7<<8 | 0x7<<4 | 0x7);
	ld |=  (0x1<<8 | 0x4<<4 | 0x1);
	writel_relaxed(ld, base + L2X0_DATA_LATENCY_CTRL);

	/* common l2x0 init */
	ld  = readl_relaxed(base + L2X0_PREFETCH_CTRL);
	ld |=  0xffffff00;
	writel_relaxed(ld, base + L2X0_PREFETCH_CTRL);

	/* common l2x0 init */
	l2x0_init(base, 0x70750000, 0xfe000fff);

#endif
	return;
}

#ifdef CONFIG_LOCAL_TIMERS
DEFINE_TWD_LOCAL_TIMER(twd_local_timer, SCU_PRIVATE_TIMER, IRQ_LOCALTIMER)
#endif
void __init imapx910_twd_init(void)
{
#ifdef CONFIG_LOCAL_TIMERS
	int err = twd_local_timer_register(&twd_local_timer);
	if (err)
		pr_err("twd_local_timer_register failed !\n");
#endif
	return;
}
