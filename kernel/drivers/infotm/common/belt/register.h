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

#if !defined(__REG_H__)
#define __REG_H__

struct belt_register {
    uint32_t addr;
    uint32_t val;
    int dir;
    int options;
};

extern uint32_t belt_register_read(uint32_t addr);
extern int belt_register_write(uint32_t val, uint32_t addr);
extern void belt_register_smp(struct belt_register *);

#endif

