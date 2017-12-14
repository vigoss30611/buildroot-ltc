/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.  The Work may be further protected by one or more patents and
* be protected as a part of a trade secret package.
*
* No part of the Work may be copied, photocopied, reproduced, translated, or
* reduced to any electronic medium or machine-readable form, in whole or in
* part, without prior written consent of the copyright owner. Any other
* reproduction in any form without the permission of the copyright owner is
* prohibited.
*
* All Work are protected by the copyright laws of all relevant jurisdictions,
* including protection under the United States copyright laws, and may not be
* reproduced, distributed, transmitted, displayed, published, or broadcast
* without the prior written permission of the copyright owner.
*
------------------------------------------------------------------------------*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/genalloc.h>

#include <asm/tlb.h>
#include <asm/cacheflush.h>
#include <asm/mach/map.h>
#include <asm/memory.h>

#include <mach/imap-iomap.h>
#include <mach/sram.h>

/* symbols from linker to assist sram setup */
/*
 * [KP-peter] TODO
 */
#if 1
extern char __sram_copy_in_dram, __sram_start, __sram_bss, __sram_end;
#endif
/*caculate the physical address of a symbol in SRAM*/
#define __sram_pa(x) (((unsigned long)(x)) -  IMAP_SRAM_VIRTUAL_ADDR)

const volatile void *__sramdata sram_jump_addr;
const volatile void *__sramdata dfs_stack;
uint32_t __sramdata sram_stack[IMAP_SRAM_STACK_SIZE / sizeof(long)];

extern void imap_sram_reinit(void);

void imap_sram_reinit(void)
{
/*
 * [KP-peter] TODO
 */
#if 1
	char *ram = &__sram_copy_in_dram;
	char *start = &__sram_start;
	char *end = &__sram_end;
	char *bss = &__sram_bss;

	BUG_ON(end - start > IMAP_SRAM_SIZE);

	pr_info("atxx imap_sram_reinitinit, copy %d bytes from %p to %p\n",
	       bss - start, ram, start);
	memset(start, 0, IMAP_SRAM_SIZE);
	memcpy(start, ram, bss - start);
#endif
}

void atxx_sram_set_jump_addr(unsigned long addr)
{
/*
 * [KP-peter] TODO
 */
#if 0
	sram_jump_addr = (void *)virt_to_phys((void *)addr);
	__cpuc_flush_dcache_area((void *)&sram_jump_addr,
				 sizeof(sram_jump_addr));
	outer_flush_range(__sram_pa(&sram_jump_addr),
			  __sram_pa(&sram_jump_addr + 1));
#endif
}
