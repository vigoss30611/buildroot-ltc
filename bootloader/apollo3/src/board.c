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

#include <asm-arm/io.h>
#include <bootlist.h>
#include <dramc_init.h>
#include <items.h>
#include <preloader.h>
#include <rballoc.h>
#include <rtcbits.h>
#include <board.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#endif
#ifdef UBOOT_DEBUG
#define printf(x...) irf->printf(x)
#else
#define printf(x...)
#endif

#if defined(CONFIG_IMAPX_FPGATEST)
int apollo3_fpga_bootst(void)
{
	uint8_t addr, buf, val;
	int tmp = rtcbit_get("resetflag");
	int slp = rtcbit_get("sleeping");
	static int flag = -1;
	uint8_t val2[2] = {0};

	if(flag >= 0) {
		printf("bootst exist: %d\n", flag);
		goto end;
	}

#if !defined(CONFIG_PRELOADER)
	flag = rbgetint("bootstats");
#else /* PRELOADER */
	printf("new boot state(%d), sleep(%d)\n", tmp, slp);
	
	if(!(readl(SYS_RST_ST) & 0x2)) {
		flag = BOOT_ST_NORMAL;
		goto end;
	}


	/* clear reset state */
	writel(0x3, SYS_RST_CLR);
	rtcbit_clear("resetflag");

	switch(tmp) {
		case BOOT_ST_RESET:
		case BOOT_ST_RECOVERY:
		case BOOT_ST_WATCHDOG:
		case BOOT_ST_CHARGER:
		case BOOT_ST_BURN:
		case BOOT_ST_FASTBOOT:
		case BOOT_ST_RESETCORE:
			flag = tmp;
			break;
		default:
			if((tmp & 3) == 0) {
				rtcbit_clear("sleeping");
				flag = BOOT_ST_RESUME;
			} else
				printf("warning: invalid boot state\n", tmp);
			break;
	}
#endif

end:
	printf("---------------bootst: %d\n", flag);
	return flag;
}
#else
int apollo3pv10_bootst(void) {
	return 0;
}

int apollo3pv10_binit(void) {
	return 0;
}

int apollo3pv10_reset(void) {
	return 0;
}

int apollo3pv10_shut(void) {
	return 0;
}

int apollo3pv10_acon(void) {
	return 0;
}

#endif //endif CONFIG_IMAPX_FPGATEST



void board_init_i2c(void)
{
#if defined(CONFIG_IMAPX_FPGATEST)
	return;
#else
	if(!item_exist("board.arch") || item_equal("board.arch", "apollo3pv10", 0))
		iic_init(0, 1, 0x68, 0, 1, 0);
#endif
}

int board_binit(void) {
#if defined(CONFIG_IMAPX_FPGATEST)
	return 0;
#else
	return apollo3pv10_binit();
#endif
}

int board_reset(void) {
#if defined(CONFIG_IMAPX_FPGATEST)
	for(;;);
	return 0;
#else
	return apollo3pv10_reset();
#endif
}

int board_shut(void) {
#if defined(CONFIG_IMAPX_FPGATEST)
	return 0;
#else
	return apollo3pv10_shut();
#endif
}

int board_bootst(void) {
#if defined(CONFIG_IMAPX_FPGATEST)
	return apollo3_fpga_bootst();
#else
	return apollo3pv10_bootst();
#endif
}

int board_acon(void) {
#if defined(CONFIG_IMAPX_FPGATEST)
	return 0;
#else
	return apollo3pv10_acon();
#endif
}

