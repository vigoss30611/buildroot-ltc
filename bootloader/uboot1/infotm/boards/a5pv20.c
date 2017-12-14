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
#include <imapx_iic.h>

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#endif

int a5pv20_binit(void)
{
    /*
     *  should init iic bus here
     */
	uint8_t addr, buf[2];

	/* set gpio1 as output mode, and output 1 to enable ctp power supply */
	addr = 0x83;
	buf[0] = 0x80;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	addr = 0x92;
	buf[0] = 0x01;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	
	// added by linyun.xiong for spi flash power up @2015-03-24
#if defined(MODE) && (MODE == SIMPLE)
	addr = 0x91;
	buf[0] = 0xf5;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	addr = 0x90;
	buf[0] = 0x03;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01); 
#endif
	///////////////////////////////////////////////////////////
	
    return 0;
}

int a5pv20_reset(void)   
{
    rtcbit_set("resetflag", BOOT_ST_RESET);
    writel(3, RTC_SYSM_ADDR);

    for(;;);
    return 0;
}

int a5pv20_shut(void)    
{
    uint8_t addr, reg;

    /* record status of shutdown, distinguish between shutdown and sleep */
    addr = 0xf;
    reg = 0;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);

    /* shut down system */
    addr = 0x32;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    reg |= 0x1 << 2;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);
    reg |= 0x1 << 7;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);

    return 0;
}

int a5pv20_bootst(void)  
{
    int tmp = rtcbit_get("resetflag");
    static int flag = -1;
    int reg;
    uint8_t reg_val, reg_n_oe_val;

    if(flag >= 0) {
        printf("bootst exist: %d\n", flag);
        goto end;
    }

#if !defined(CONFIG_PRELOADER)
    flag = rbgetint("bootstats");
#else /* PRELOADER */
    printf("boot state(%d)\n", tmp);

	reg = 0xf;
	iicreg_write(0, &reg, 0x01, &reg_val, 0x00, 0x01);
	iicreg_read(0, &reg_val, 0x1);
	if (reg_val == 1) {
		reg_val = 0x0;
		iicreg_write(0, &reg, 0x01, &reg_val, 0x01, 0x01);
		reg = 0x4b;
		iicreg_write(0, &reg, 0x01, &reg_n_oe_val, 0x00, 0x01);
		iicreg_read(0, &reg_n_oe_val, 0x1);
		if (!(reg_n_oe_val & 0x80))
			return BOOT_ST_RESUME;
	}

    if(!(readl(SYS_RST_ST) & 0x2)) {
        flag = BOOT_ST_NORMAL;
        goto end;
    }

    /* clear state off-hand */
    writel(3, SYS_RST_ST + 4);
    rtcbit_clear("resetflag");

     switch(tmp)
     {
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
			 printf("warning: invalid boot state\n", tmp);
     }
#endif
end:
     printf("---------------bootst: %d\n", flag);
     return flag;
}

int a5pv20_acon(void)
{
    uint8_t addr, reg;

    addr = 0x00;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);

    return ((reg & 0x1) ? 1 : 0);
}

