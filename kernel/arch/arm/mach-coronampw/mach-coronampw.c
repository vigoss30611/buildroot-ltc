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
#include <linux/memblock.h>
#include <asm/mach/arch.h>
#include <asm/mach-types.h>
#include <asm/setup.h>
#include <asm/mach/map.h>
#include <asm/irq.h>
#include <mach/items.h>
#include <mach/nvedit.h>
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
#include <mach/hw_cfg.h>

extern phys_addr_t arm_lowmem_limit;

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
	return rbgetint("forceshut");
}
EXPORT_SYMBOL(imapx15_unexpect_shut);


static void __init coronampw_init_early(void)
{
	item_init(rbget("itemrrtb"), ITEM_SIZE_NORMAL);
	rtcbit_init();
	//initenv();
	/* peter: reserve nothing for qsdk
	   coronampw_mem_reserve();
	 */
	return;
}

static void __init coronampw_init_irq(void)
{
    void __iomem *base1, *base2;
    base1 = ioremap(IMAP_GIC_DIST_BASE, 0x1000);
    base2 = ioremap(IMAP_GIC_CPU_BASE, 0x1000);
    printk("base1 is 0x%x, base2 is 0x%x \n", base1, base2);
	/* start from 29 to enable local timer */
	//gic_init(0, 29, IO_ADDRESS(IMAP_GIC_DIST_BASE),
	//		IO_ADDRESS(IMAP_GIC_CPU_BASE));
	gic_init(0, 29, base1,
			base2);
	return;
}

static void __init coronampw_map_io(void)
{
	iotable_init(imap_iodesc, ARRAY_SIZE(imap_iodesc));
	return;
}

static void __init coronampw_init_module_mem(void)
{
	/* power down module memory as default, it can
	 * be power on by module_power_on() when needed*/
	module_power_down(SYSMGR_CAMIF_BASE);
	module_power_down(SYSMGR_ISPOST_BASE);
	module_power_down(SYSMGR_GDMA_BASE);
	module_power_down(SYSMGR_IDS0_BASE);
	module_power_down(SYSMGR_USBH_BASE);
	module_power_down(SYSMGR_OTG_BASE);
	module_power_down(SYSMGR_MMC1_BASE);
	module_power_down(SYSMGR_MMC2_BASE);
	module_power_down(SYSMGR_MMC0_BASE);
	module_power_down(SYSMGR_MAC_BASE);
	module_power_down(SYSMGR_MIPI_BASE);
	module_power_down(SYSMGR_ISP_BASE);
	module_power_down(SYSMGR_VENC265_BASE);
	module_power_down(SYSMGR_SSP_BASE);
	module_power_down(SYSMGR_SPIMUL_BASE);
}

static void __init coronampw_machine_init(void)
{
	coronampw_reset_module();
	coronampw_init_l2x0();
	coronampw_init_devices();
	apollo_init_gpio();
	coronampw_init_module_mem();
	pm_power_off = coronampw_power_off;
	return;
}

static void __init coronampw_init_late(void)
{
#ifdef CONFIG_PM
	coronampw_init_pm();
#endif
	return;
}

static void __init coronampw_init_time(void)
{
	imapx_clock_init();

	/* lowlevel clock event */
	module_power_on(SYSMGR_CMNTIMER_BASE);
	coronampw_gtimer_init(IO_ADDRESS(SCU_GLOBAL_TIMER), "timer-ca5");
	coronampw_cmn_timer_init(IO_ADDRESS(IMAP_TIMER_BASE),
					GIC_CMNT0_ID, "imap-cmn-timer");
	/*
	imapx_hr_init(IO_ADDRESS(IMAP_TIMER_BASE) + 0x14,
					GIC_CMNT1_ID, "imap-cmn-timer1");
					*/
	coronampw_twd_init();
	return;
}

static void __init coronampw_reserve(void)
{
    memblock_reserve(arm_lowmem_limit, 0x80000);/* for external p/d tcm */
}

void coronampw_restart(char mode, const char *cmd)
{
	pr_err("[CPU] imap_restart\n");
	if(!strcmp(imapx_pmu_cfg.name ,"ip6313")){
		coronampw_ip6313_reset(mode, cmd);
	}
	return;
}
EXPORT_SYMBOL(coronampw_restart);

MACHINE_START(IMAPX15, "iMAPx15")
	.nr				= 0x8f9,
	.atag_offset    = 0x100,
	.smp			= smp_ops(coronampw_smp_ops),
	.init_early		= coronampw_init_early,
	.init_irq		= coronampw_init_irq,
	.map_io			= coronampw_map_io,
	.init_machine   = coronampw_machine_init,
	.init_time		= coronampw_init_time,
	.init_late		= coronampw_init_late,
	.restart		= coronampw_restart,
	.reserve		= coronampw_reserve,
MACHINE_END

