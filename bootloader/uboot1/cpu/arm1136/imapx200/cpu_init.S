/***************************************************************************** 
** cpu/arm1136/imapx200/cpu_init.S
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Memory controllor asm initialization.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/23/2009 XXX	Warits
*****************************************************************************/

#include <config.h>
#include <asm/arch/imapx200.h>

#include CONFIG_SYS_DENALI_FILE

	.globl mem_ctrl_asm_init

mem_ctrl_asm_init:

	/* locate r2 to denali data */
	ldr	r2, _denali_ctl_pa_data
	add	r2, r2, #4

#ifdef CONFIG_BOOT_AUTO_ADJUST
	ldr r0, =CONFIG_AU_MEM_MAGIC_OFFS
	ldr r3, [r0]
	ldr r1, =CONFIG_AU_MEM_MAGIC
	cmp r1, r3
@	bne denali_start
	/* to set 50ohm half at the first time */
	bne denali_first

	mov r3, #4
	mov r4, #248

rw_dll_reload:
	ldr r1, [r0, r3]
	str r1, [r2, r4]			@denali: 61 + r3 / 4
	cmp r3, #32
//	beq denali_start
    beq denali_driving
	add r3, r3, #4
	add r4, r4, #4
	b rw_dll_reload
denali_driving:
        ldr r1, [r0, #-8]
        str r1, [r2, #184]
        ldr r1, [r0, #-4]
        str r1, [r2, #296]

        ldr r1, [r0, #36]
        str r1, [r2, #188]          
        ldr r1, [r0, #40]
        str r1, [r2, #72]        
        b denali_start

denali_first:
        ldr r1, =0x00060642
        str r1, [r2, #184]

        ldr r1, =0x00000006
        str r1, [r2, #188]

        ldr r1, =0x00a00004
        str r1, [r2, #296]

denali_start:
#endif

	ldr	r1, =PERIPHERAL_BASE_ADDR_PA
	orr	r1, r1, #0x20000

	/* turn off denali */
	mov r0, #0
	str r0, [r1, #36]

	mov	r4, #0
denali_l:
	ldr	r3, [r2, r4]
	str	r3, [r1, r4]
	add r4, r4, #4
	cmp r4, #0x150
	bne denali_l

	ldr r3, [r2, r4]
	str r3, [r1, #36]

	/* Init memory pool for MMU */
mmp_init:

	/* Set MP to normal mode */
	ldr	r1, =PERIPHERAL_BASE_ADDR_PA
	mov r2, #0
	str r2, [r1, #0x120]

	/* Power on MP */
	ldr r2, [r1, #0x210]
	orr r2, r2, #4
	str r2, [r1, #0x210]
	
mp_pow_check:
	ldr r2, [r1, #0x214]
	tst r2, #4
	beq mp_pow_check
	
	/* Reset mempool */
	mov r2, #4
	str r2, [r1, #0x21c]

	mov r3, #0
mp_rst_l:
	ldr r2, [r1, #0x21c]
	add r3, r3, #1
	cmp r3, #1000
	bne mp_rst_l
	mov r2, #0
	str r2, [r1, #0x21c]
	
	/* Turn on isolation */
	ldr r2, [r1, #0x218]
	bic r2, r2, #4
	str r2, [r1, #0x218]

	mov pc, lr

