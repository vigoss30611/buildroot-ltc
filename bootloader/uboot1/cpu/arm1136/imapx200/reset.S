/***************************************************************************** 
** cpu/arm1136/imapx200/reset.S
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Software reset cpu.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/21/2009 XXX	Warits
*****************************************************************************/

#include <asm/arch/imapx200.h>

.globl reset_cpu
reset_cpu:
	ldr	r1, =PERIPHERAL_BASE_ADDR_PA
	ldr	r2, =0x6565

	/* Store 0x6565 to SW_RST to do a system reset */
	str	r2, [r1, #0x100]
_loop_forever:
	b	_loop_forever

.globl imapx200_check_reset

#if defined(CONFIG_NAND_SPL) || defined(CONFIG_MEM_POOL)
imapx200_check_reset:
	ldr r0, =RST_ST				@0x20c00110
	ldr r2, [r0]
	tst r2, #0x8
	bne normal_boot
	tst r2, #0x10
	beq normal_boot

	/* This is a wakeup state, clear RST_ST */
	mov r1, #0xff
	str r1, [r0]
	
	/* Return to kernel sapce */
	ldr r0, =INFO0
	ldr pc, [r0]

	/* Nerver reached here */
	b .

normal_boot:
	mov pc, lr

.global hang
hang:
	b .
#endif
