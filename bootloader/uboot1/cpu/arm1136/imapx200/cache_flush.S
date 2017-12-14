/***************************************************************************** 
** cpu/arm1136/imapx200/cache-flush.S
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: iMAPx200 cache-flush function(arm1136)
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 03/22/2010 XXX	Warits
*****************************************************************************/

#include <asm/arch/imapx200.h>
#define D_CACHE_LINE_SIZE 32

.globl imapx200_cache_flush

imapx200_cache_flush:
	bic r0, r0, #D_CACHE_LINE_SIZE - 1
1:
	mcr p15, 0, r0, c7, c14, 1      @ clean & invalidate D line
	add r0, r0, #D_CACHE_LINE_SIZE
	cmp r0, r1
	blo 1b
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4      @ drain write buffer
	mov pc, lr

.globl imapx200_cache_inv

imapx200_cache_inv:
	tst r0, #D_CACHE_LINE_SIZE - 1
	bic r0, r0, #D_CACHE_LINE_SIZE - 1
	mcrne   p15, 0, r0, c7, c10, 1      @ clean D line
	tst r1, #D_CACHE_LINE_SIZE - 1
	bic r1, r1, #D_CACHE_LINE_SIZE - 1
	mcrne   p15, 0, r1, c7, c14, 1      @ clean & invalidate D line
2:
	mcr p15, 0, r0, c7, c6, 1       @ invalidate D line
	add r0, r0, #D_CACHE_LINE_SIZE
	cmp r0, r1
	blo 2b
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4      @ drain write buffer
	mov pc, lr

.globl imapx200_cache_clean

imapx200_cache_clean:
    bic r0, r0, #D_CACHE_LINE_SIZE - 1
3:
	mcr p15, 0, r0, c7, c10, 1      @ clean D line
	add r0, r0, #D_CACHE_LINE_SIZE
	cmp r0, r1
	blo 3b
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4      @ drain write buffer
	mov pc, lr

