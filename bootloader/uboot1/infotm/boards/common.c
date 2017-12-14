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
#include <preloader.h>
#include <rballoc.h>
#include <rtcbits.h>
#include <board.h>

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#endif

struct arch_functions {
    char arch[16];
    int (*init)(void);
    int (*reset)(void);
    int (*shut)(void);
    int (*bootst)(void);
    int (*acon)(void);
};

static struct arch_functions archf[] = {
    {
        "a5pv10",
        a5pv10_binit,
        a5pv10_reset,
        a5pv10_shut,
        a5pv10_bootst,
        a5pv10_acon,
    }, {
        "a5pv20",
        a5pv20_binit,
        a5pv20_reset,
        a5pv20_shut,
        a5pv20_bootst,
        a5pv20_acon,
    }, {
        "a9pv10",
        a9pv10_binit,
        a9pv10_reset,
        a9pv10_shut,
        a9pv10_bootst,
        a9pv10_acon,
    }, {
	"q3pv10",
	q3pv10_binit,
	q3pv10_reset,
	q3pv10_shut,
	q3pv10_bootst,
	q3pv10_acon,
    },
};

static struct arch_functions *af = archf;

void board_scan_arch(void)
{
    int i;

//    printf("-->board_scan_arch\n");
    
    for(i = 0; i < ARRAY_SIZE(archf); i++)
    {
        if(item_equal("board.arch", archf[i].arch, 0))
	{
    		af = &archf[i];
//		printf("--->cur arch is: %s\n", af->arch);
	}
    }
    printf("board arch is set to: %s\n", af->arch);
}

void board_init_i2c(void)
{
	if (!item_exist("board.arch") ||
			item_equal("board.arch", "a5pv10", 0) ||
			item_equal("board.arch", "a5pv20", 0) || 
			item_equal("board.arch", "q3pv20", 0)) 
		iic_init(0, 1, 0x68, 0, 1, 0);
	else if (item_equal("board.arch", "a9pv10", 0))
		iic_init(0, 1, 0x64, 0, 1, 0);
}

int board_binit(void) {
    return af->init();
}

int board_reset(void) {
    return af->reset();
}

int board_shut(void) {
    return af->shut();
}

int board_bootst(void) {
    return af->bootst();
}

int board_acon(void) {
    return af->acon();
}

