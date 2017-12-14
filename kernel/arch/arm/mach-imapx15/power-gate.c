/* arch/arm/mach-imapx15/power-gate.c
 *
 * control each module power gate
 *
 *  Copyright (c) 2013 Infotm ic  Co., Ltd.
 *  http://www.infotmic.com.cn/
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <mach/rballoc.h>
#include <mach/rtcbits.h>
#include <asm/mach/map.h>
#include <mach/hardware.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <mach/io.h>


#define __pow_readl(x)           __raw_readl(IO_ADDRESS(x))
#define __pow_writel(val, x)     __raw_writel(val, IO_ADDRESS(x))

static DEFINE_SPINLOCK(power_gate_lock);

#define POWER_TIMEOUT_MS    0xff

/*
 * module reset
 *
 * module addr : module pa addr
 * reset_set : in some module there are more
 * than 1 bit to control sub modules to reset
 */
int module_reset(uint32_t module_addr, uint32_t reset_sel)
{
	unsigned long flags;
	int i = 5;
	spin_lock_irqsave(&power_gate_lock, flags);

	__pow_writel(reset_sel, SYS_SOFT_RESET(module_addr));

	do {} while (i--);
	__pow_writel(DISABLE, SYS_SOFT_RESET(module_addr));

	spin_unlock_irqrestore(&power_gate_lock, flags);

	return 0;
}
EXPORT_SYMBOL(module_reset);

/*
 * module power on
 *
 * module addr : should be the module off addr in the system manager
 *
 * here we open all the clocks
 *
 * return : 1 means timeout error
 */
int __module_power_on(uint32_t module_addr, int lite)
{
	uint32_t ret = 0;
	unsigned long flags;
	unsigned long timeout = 0;
	spin_lock_irqsave(&power_gate_lock, flags);
	/* module reset */
	__pow_writel(0xffffffff, SYS_SOFT_RESET(module_addr));
	/* close bus */
	__pow_writel(DISABLE, SYS_BUS_MANAGER(module_addr));
	/* enable clk */
	__pow_writel(0xffffffff, SYS_CLOCK_GATE_EN(module_addr));

	if(!lite) {
		/* power up */
		__pow_writel(MODULE_POWERON, SYS_POWER_CONTROL(module_addr));

		ret = __pow_readl(SYS_POWER_CONTROL(module_addr));
		while (!(ret & (1 << MODULE_POWERON_ACK))) {
			ret = __pow_readl(SYS_POWER_CONTROL(module_addr));
			timeout++;
			if (timeout == POWER_TIMEOUT_MS)
				goto err;
		}

		/* set isolation module output zero */
		__pow_writel(MODULE_ISO_CLOSE, SYS_POWER_CONTROL(module_addr));
	} else {
		__pow_writel(__pow_readl(SYS_POWER_CONTROL(module_addr))
				| 0x10, SYS_POWER_CONTROL(module_addr));
		__pow_writel(__pow_readl(SYS_POWER_CONTROL(module_addr))
				& ~1,SYS_POWER_CONTROL(module_addr));
	}

	/* bus signal isolation en */
	__pow_writel(ENABLE, SYS_BUS_ISOLATION_R(module_addr));
	/* enable bus */
	__pow_writel(MODULE_BUS_ENABLE, SYS_BUS_MANAGER(module_addr));
	__pow_writel(DISABLE, SYS_BUS_QOS_MANAGER1(module_addr));
	/* release reset */
	__pow_writel(DISABLE, SYS_SOFT_RESET(module_addr));
	spin_unlock_irqrestore(&power_gate_lock, flags);
	return 0;
err:
	spin_unlock_irqrestore(&power_gate_lock, flags);
	return 1;
}

int module_power_on(uint32_t addr)
{
	return __module_power_on(addr, 0);
}

int module_power_on_lite(uint32_t addr)
{
	printk(KERN_ERR "power_on_lite: do not do power on.\n");
	return __module_power_on(addr, 1);
}

int module_special_config(uint32_t addr, uint32_t cfg)
{
    return 1;
}

EXPORT_SYMBOL(module_power_on);
EXPORT_SYMBOL(module_power_on_lite);
EXPORT_SYMBOL(module_special_config);


/*
 * module ce
 */
int __module_power_down(uint32_t module_addr)
{
	uint32_t ret = 0;
	unsigned long flags;
	unsigned long timeout = 0;

	spin_lock_irqsave(&power_gate_lock, flags);

	/* module reset */
	__pow_writel(0xffffffff, SYS_SOFT_RESET(module_addr));
	/* close bus */
	__pow_writel(DISABLE, SYS_BUS_MANAGER(module_addr));
	__pow_writel(DISABLE, SYS_BUS_ISOLATION_R(module_addr));
	__pow_writel(ENABLE, SYS_BUS_QOS_MANAGER1(module_addr));
	/* power down */
	__pow_writel(MODULE_POWERDOWN, SYS_POWER_CONTROL(module_addr));
	/* wait for power */
	ret = __pow_readl(SYS_POWER_CONTROL(module_addr));
	while (ret & (1 << MODULE_POWERON_ACK)) {
		ret = __pow_readl(SYS_POWER_CONTROL(module_addr));
		timeout++;
		if (timeout == POWER_TIMEOUT_MS)
			goto error;
	}
	/* release reset */
	__pow_writel(DISABLE, SYS_SOFT_RESET(module_addr));

	spin_unlock_irqrestore(&power_gate_lock, flags);

	return 0;

error:
	/* release reset */
	__pow_writel(DISABLE, SYS_SOFT_RESET(module_addr));

	spin_unlock_irqrestore(&power_gate_lock, flags);
	return 1;
}

int module_power_down(uint32_t module_addr)
{
    return __module_power_down(module_addr);
}

int module_power_down_lite(uint32_t module_addr)
{
   return  __module_power_down(module_addr);
}

EXPORT_SYMBOL(module_power_down);
EXPORT_SYMBOL(module_power_down_lite);


void imapx15_reset_core(int index, uint32_t hold_base)
{
	uint32_t tmp;                                        

	if(index < 0 || index >= CONFIG_NR_CPUS)         
		return ;                                           

	if(hold_base & 0xff) {                               
		pr_err("core cold base must be 256B aligned.\n");
		return ;                                         
	}                                                    

	/* set hold base address */                    
	rtcbit_set("holdbase", hold_base >> 8);    
	rtcbit_set("resetflag", BOOT_ST_RESETCORE);
	
	/* set hold base init */                     
	writel(0, IO_ADDRESS(hold_base) + index * 4);

	/* reset core */                                                
	tmp = readl(IO_ADDRESS(SYSMGR_CPU_BASE));                       
	writel(tmp & ~(1 << (4 + index)), IO_ADDRESS(SYSMGR_CPU_BASE)); 
	udelay(50);                                                     
	writel(tmp |  (1 << (4 + index)), IO_ADDRESS(SYSMGR_CPU_BASE)); 
	                                                                
	/* wait core ready */                                           
	while(!readl(IO_ADDRESS(hold_base) + index * 4));               
	                                                                
	/* clear hold base */                                           
	rtcbit_set("holdbase", 0);                                  
}

void imapx15_reset_module(void)
{
	__raw_writel(0xff, IO_ADDRESS(SYSMGR_CRYPTO_BASE));
}
