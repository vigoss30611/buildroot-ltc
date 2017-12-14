/***************************************************************************** 
** XXX nand_spl/board/infotm/imapx/boot_main.c XXX
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: The first C function in PRELOADER.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 03/17/2010 XXX	Warits
*****************************************************************************/

#include <common.h>
#include <asm/io.h>
#include <bootlist.h>
#include <dramc_init.h>
#include <items.h>
#include <nand.h>
#include <vstorage.h>
#include <preloader.h>
#include <rballoc.h>
#include <asm/arch/nand.h>

#define DEFAULT_ITEM_OFFSET 0xC000
int init_config(void)
{
	int id, ret, len = 0;
	void * cfg = (void *)ITEMS_LOWBASE;
	uint8_t *item_mem = NULL;
#if defined(MODE) && (MODE == SIMPLE)
	item_mem = rbget("itemrrtb");
	printf("rbget item_mem = %p\n", item_mem);

	if (item_mem != NULL) {
		return item_init(item_mem, ITEM_SIZE_NORMAL);
	} else {
		item_mem = malloc(ITEM_SIZE_NORMAL);
		if (!item_mem)
			return -1;
		vs_assign_by_id(DEV_FLASH, 1);
		vs_read(item_mem, DEFAULT_ITEM_OFFSET, ITEM_SIZE_NORMAL, 0);
		return item_init(item_mem, ITEM_SIZE_NORMAL);
	}
#else
		printf("cfg: 0x%p\n", cfg);
	id = boot_device();
	switch(id) {
		case DEV_EEPROM:
		case DEV_FLASH:
			cfg = (void *)(IRAM_BASE_PA + BL_SIZE_FIXED - ITEM_SIZE_EMBEDDED);
			len = ITEM_SIZE_EMBEDDED;
			break ;
		case DEV_BND:
			id = DEV_FND;
		default:
			printf("read items: id=%d\n", id);
			item_mem = rbget("itemrrtb");
			if (item_mem != NULL) {
				printf("item in ius\n");
				memcpy(cfg, item_mem, ITEM_SIZE_NORMAL);
			} else {
				ret = vs_assign_by_id(id, 1);
				if (ret)
					return ret;
				ret = vs_read(cfg, INFOTM_ITEM_OFFSET, ITEM_SIZE_NORMAL, 0);
				if (ret < 0)
					return ret;
			}
			len = ITEM_SIZE_NORMAL;
	}

	printf("begin init.\n");
	return item_init(cfg, len);
#endif /* (MODE == SIMPLE) */
}

