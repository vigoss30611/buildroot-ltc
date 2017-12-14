/***************************************************************************** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Allow register operation from user space.
**
** Author:
**     warits <warits.wang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.1  09/12/2012
*****************************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/poll.h>
#include <linux/rtc.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <mach/belt.h>
#include <mach/items.h>
#include <mach/nvedit.h>

#include "register.h"

#define REMAP_RANGE SZ_4K

uint32_t __belt_base = 0;
void __iomem * __belt_remapped_base = NULL;

static int __addr_remap(uint32_t addr)
{
    if(!__belt_remapped_base ||
            (addr < __belt_base || addr >= __belt_base + REMAP_RANGE))
    {
        __belt_base = addr;
        __belt_remapped_base = ioremap(__belt_base, REMAP_RANGE);

        if(!__belt_remapped_base) {
            printk(KERN_ERR "belt reg: remap failed.\n");
            return -1;
        }
    }

    return 0;
}

uint32_t belt_register_read(uint32_t addr)
{
    addr &= ~3; // make align
    if(__addr_remap(addr))
        return -1;

    return readl(__belt_remapped_base + (addr - __belt_base));
}
EXPORT_SYMBOL(belt_register_read);

int belt_register_write(uint32_t val, uint32_t addr)
{
    addr &= ~3; // make align
    if(__addr_remap(addr))
        return -1;

    writel(val, __belt_remapped_base + (addr - __belt_base));
    return 0;
}
EXPORT_SYMBOL(belt_register_write);

void do_belt_register_smp(void *info)
{
    struct belt_register *reg = info;
    if(reg->dir)
        reg->val = belt_register_read(reg->addr);
    else
        belt_register_write(reg->val, reg->addr);

//    printk(KERN_ERR "smpid: %d\n", raw_smp_processor_id());
}

void belt_register_smp(struct belt_register *reg)
{
    if(reg->options)
        smp_call_function_single(reg->options & 0x7, do_belt_register_smp, (void *)reg, 1);
    else
        do_belt_register_smp(reg);
}

