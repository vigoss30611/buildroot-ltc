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

.globl dram_init
dram_init:

	mov pc, lr @return 
