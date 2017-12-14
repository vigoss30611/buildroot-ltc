/* linux/arch/arm/mach-imapx9/mach-imapx9.c
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
#include <linux/bug.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/pm.h>
#include <asm/mach/arch.h>
#include <asm/mach-types.h>
#include <asm/setup.h>
#include <asm/mach/map.h>
#include <asm/irq.h>
#include <mach/items.h>
#include <mach/rtcbits.h>
#include <mach/rballoc.h>
#include <mach/imap-iomap.h>
#include <mach/timer-gtm.h>
#include <mach/timer-cmn.h>
#include <mach/power-gate.h>
#include <mach/io.h>
#include <mach/timer-hr.h>
#include <mach/clk.h>
#include "core.h"

static struct map_desc imap_iodesc[] __initdata = {
	{
		.virtual    = (unsigned long)IMAP_SYSMGR_VA,
		.pfn        = __phys_to_pfn(IMAP_SYSMGR_BASE),
		.length     =  IMAP_SYSMGR_SIZE,
		.type       = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_SCU_VA,
		.pfn        = __phys_to_pfn(IMAP_SCU_BASE),
		.length     = IMAP_SCU_SIZE,
		.type           = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_TIMER_VA,
		.pfn        = __phys_to_pfn(IMAP_TIMER_BASE),
		.length     = IMAP_TIMER_SIZE,
		.type           = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_IRAM_VA,
		.pfn        = __phys_to_pfn(IMAP_IRAM_BASE),
		.length     = IMAP_IRAM_SIZE,
		.type           = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_EMIF_VA,
			.pfn        = __phys_to_pfn(IMAP_EMIF_BASE),
			.length     = IMAP_EMIF_SIZE,
			.type       = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_IIC0_VA,
			.pfn        = __phys_to_pfn(IMAP_IIC0_BASE),
			.length     = IMAP_IIC0_SIZE,
			.type       = MT_DEVICE,
	}, {
		.virtual    = (unsigned long)IMAP_SRAM_VIRTUAL_ADDR,
			.pfn        = __phys_to_pfn(IMAP_SRAM_PHYS_ADDR),
			.length     = IMAP_SRAM_SIZE,
			.type       = MT_MEMORY_NONCACHED,
	}
};

int imapx15_unexpect_shut(void)
{
	//return rbgetint("forceshut");
	return 1;
}
EXPORT_SYMBOL(imapx15_unexpect_shut);


static void __init imapx15_init_early(void)
{
	int cur_case = -1;
	item_init(rbget("itemrrtb"), ITEM_SIZE_NORMAL);
	rtcbit_init();

	/* change iMAPx820 case number according to config */
	if (item_exist("board.case")) {
		cur_case = item_integer("board.case", 0);
		pr_err("Set cpu case to: %d\n", cur_case);
		writel(cur_case, IO_ADDRESS(IMAP_SYSMGR_BASE + 0x9000));
	}

	return;
}

static void __init imapx15_init_irq(void)
{
	/* start from 29 to enable local timer */
	gic_init(0, 29, IO_ADDRESS(IMAP_GIC_DIST_BASE),
			IO_ADDRESS(IMAP_GIC_CPU_BASE));
	return;
}

static void __init imapx15_map_io(void)
{
	iotable_init(imap_iodesc, ARRAY_SIZE(imap_iodesc));
	return;
}

static void __init imapx15_init_module_mem(void)
{
	/* power down module memory as default, it can
	 * be power on by module_power_on() when needed*/
	module_power_down(SYSMGR_PCM_BASE);
	module_power_down(SYSMGR_SSP_BASE);
	module_power_down(SYSMGR_PWDT_BASE);
	module_power_down(SYSMGR_IDS1_BASE);
	module_power_down(SYSMGR_USBH_BASE);
	module_power_down(SYSMGR_OTG_BASE);
	module_power_down(SYSMGR_MMC1_BASE);
	module_power_down(SYSMGR_MMC2_BASE);
	module_power_down(SYSMGR_MMC0_BASE);
	module_power_down(SYSMGR_MAC_BASE);
	module_power_down(SYSMGR_MIPI_BASE);
	module_power_down(SYSMGR_ISP_BASE);
	module_power_down(SYSMGR_VDEC264_BASE);
	module_power_down(SYSMGR_VENC264_BASE);
	module_power_down(SYSMGR_VDEC265_BASE);
	module_power_down(SYSMGR_VENC265_BASE);
}

static void __init imapx15_machine_init(void)
{
	imapx15_reset_module();
	imapx15_init_l2x0();
	imapx15_init_devices();
	apollo_init_gpio();
	//imapx15_init_module_mem();
	pm_power_off = imapx15_power_off;
	return;
}

static void __init imapx15_init_late(void)
{
#ifdef CONFIG_PM
	imapx15_init_pm();
#endif
	return;
}

static void __init imapx15_init_time(void)
{
	imapx_clock_init();

	/* lowlevel clock event */
	module_power_on(SYSMGR_CMNTIMER_BASE);
//	imapx15_gtimer_init(IO_ADDRESS(SCU_GLOBAL_TIMER), "timer-ca5");
	imapx15_cmn_timer_init(IO_ADDRESS(IMAP_TIMER_BASE),
					GIC_CMNT0_ID, "imap-cmn-timer");
	imapx_hr_init(IO_ADDRESS(IMAP_TIMER_BASE) + 0x14,
					GIC_CMNT1_ID, "imap-cmn-timer1");
	imapx15_twd_init();
	return;
}

void imapx15_restart(char mode, const char *cmd)
{
		q3pv10_reset(mode, cmd);
	return;
}
EXPORT_SYMBOL(imapx15_restart);

MACHINE_START(IMAPX15, "iMAPx15")
	.nr				= 0x8f9,
	.atag_offset    = 0x100,
	.smp			= smp_ops(imapx15_smp_ops),
	.init_early		= imapx15_init_early,
	.init_irq		= imapx15_init_irq,
	.map_io			= imapx15_map_io,
	.init_machine   = imapx15_machine_init,
	.init_time		= imapx15_init_time,
	.init_late		= imapx15_init_late,
	.restart		= imapx15_restart,
MACHINE_END

