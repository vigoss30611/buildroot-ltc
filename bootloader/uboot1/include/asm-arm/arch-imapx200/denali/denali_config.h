/*****************************************************************************
** denali_config.h
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: memory study code, seach for a proper eye to r/w DDR2.
**
** Author:
**     warits <warits.wang@infotmic.com.cn>
**
** Revision History:
** -----------------
** 1.1  08/23/2010
** 1.2  08/29/2010
*****************************************************************************/

#ifndef __DENALI_CONFIG_H__
#define __DENALI_CONFIG_H__

//#define __UBOOT_ASIC__
#ifndef __UBOOT_ASIC__

#include <common.h>
#include <asm/io.h>

extern void boot_set_stat(uint16_t stat);
extern void boot_set_stat2(uint16_t stat);
extern void * check_ubimg(void);
extern void nand_boot(void);
extern void imapx200_check_reset(void);
extern int usb_otg_check(void);
extern int usb_otg_load(void);
extern int timer_init(void);
extern void udelay(ulong us);
extern void imapx200_init_gpio(void);
extern void mmu_table_init(void);
extern void mmu_start(void);
extern void denali_study(void);
extern void imapx200_cache_clean(uint32_t start, uint32_t end);
extern void imapx200_cache_inv(uint32_t start, uint32_t end);

#ifndef clean_cache
#define clean_cache imapx200_cache_clean
#endif	/* clean_cache */
#ifndef inv_cache
#define inv_cache imapx200_cache_inv
#endif	/* inv_cache */

#else	/* __UBOOT_ASIC__ */
#include "uboot_emu.h"
#endif	/* __UBOOT_ASIC__ */

#define ADDRESS_TEST       0

#if ADDRESS_TEST
#define ROW                 13
#define BANK                3
#define COL                 10
#define MEM_START           0x40000000
//#define ADDRESS_ERROR_LOC   0x3fff8090
#endif

#endif	/* __DENALI_CONFIG_H__ */
