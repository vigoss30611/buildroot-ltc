/***************************************************************************** 
** board/infotm/imapx/lowlevel_init.S
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Board specific low level initilizations.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/24/2009 XXX	Warits
*****************************************************************************/

/*
 * TODO: U-BOOT use only SVC32 mode, so FIQ, IRQ stack is not set
 *		 be different with FPGA init code, INT is not disable after
 *		 reset.
 *	fix these if any problem happens.
 */

#include <config.h>
#include <version.h>

.globl lowlevel_init
lowlevel_init:
	mov pc, lr		@ save return address

