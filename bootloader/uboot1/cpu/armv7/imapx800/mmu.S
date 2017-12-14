/***************************************************************************** 
** mmu.S
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: start MMU operation
**
** Author:
**      
** Revision History: 
** ----------------- 
** 1.1  08/24/2010
*****************************************************************************/

#include <config.h>

/* we remap 2G physical area here
 * while 0x00000000 ~ 0x7fffffff is cached
 * and 0x80000000 ~ 0xffffffff is uncached
 */

.globl mmu_table_init
mmu_table_init:
	ldr r3, =0x40e
	ldr r4, =CONFIG_RESV_PG_TB
	ldr r5, =0x2000
	mov r1, #0

tb_l:
	add r0, r4, r1
	add r2, r3, r1, lsl #18
	str r2, [r0]
	bic r2, #0xc
	add r0, r0, #0x2000
	str r2, [r0]

	add r1, r1, #4
	cmp r1, r5
	bne tb_l

	mov pc, lr

.globl mmu_start
mmu_start:
	mov r0, #1
	mcr p15, 0, r0, c3, c0, 0
	ldr r0, =CONFIG_RESV_PG_TB
	mcr p15, 0, r0, c2, c0, 0
	ldr r0, =0x40e
	mcr p15, 0, r0, c8, c7, 0
	mrc p15, 0, r0, c1, c0, 0
	ldr r1, =0x1075
	orr r0, r0, r1
	mcr p15, 0, r0, c1, c0, 0
	mov r0, r0
	mov r0, r0

	mov pc, lr

.globl mmu_stop
mmu_stop:
	mov r1, #1
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, r1
	mcr p15, 0, r0, c1, c0, 0
	mov pc, lr
 
