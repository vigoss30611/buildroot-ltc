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

int a5pv10_binit(void)
{
    return 0;
}

int a5pv10_reset(void)   
{
    rtcbit_set("resetflag", BOOT_ST_RESET);
    writel(3, RTC_SYSM_ADDR);

    for(;;);
    return 0;
}

int a5pv10_shut(void)    
{
    /*shut down */
    writel(0xff, RTC_SYSM_ADDR + 0x28);
    writel(0, RTC_SYSM_ADDR + 0x7c);
    writel(0x2, RTC_SYSM_ADDR);
    asm("wfi");

    return 0;
}

int a5pv10_bootst(void)  
{
    int tmp = rtcbit_get("resetflag");
    static int flag = -1;

    if(flag >= 0) {
        printf("bootst exist: %d\n", flag);
        goto end;
    }

#if !defined(CONFIG_PRELOADER)
    flag = rbgetint("bootstats");
#else /* PRELOADER */
    printf("boot state(%d)\n", tmp);

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
            if((tmp & 3) == 0)
                flag = BOOT_ST_RESUME;
            else
                printf("warning: invalid boot state\n", tmp);
    }
#endif

end:
    printf("---------------bootst: %d\n", flag);
    return flag;
}

int a5pv10_acon(void) {
    return !!(readl(RTC_GPINPUT_ST) & 0x4);
}

