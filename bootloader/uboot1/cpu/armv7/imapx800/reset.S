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

#include <asm/arch/imapx_base_reg.h>

.globl reset_cpu
reset_cpu:
	ldr r0, =RTC_SYSM_ADDR
	mov r1, #1
	str r1, [r0, #0x2c]
	mov r1, #3
	str r1, [r0]

	@ this should never be reached
	b .

