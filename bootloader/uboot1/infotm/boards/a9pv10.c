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
#include <gpio.h>

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#define module_enable irf->module_enable
#endif

int a9pv10_binit(void)
{
    return 0;
}

int a9pv10_reset(void)   
{
    uint8_t addr, val;

    rtcbit_set("resetflag", BOOT_ST_RESET);

#if 1
    /*REPWRON enable*/
    addr = 0x0F;
    iicreg_write(0, &addr, 0x01, &val, 0x00, 0x00);
    iicreg_read(0, &val, 0x1);
    val |= 0x1;
    iicreg_write(0, &addr, 0x01, &val, 0x01, 0x01);

    /*soft shutdown*/
    writel(2, RTC_SYSM_ADDR);
#else
    /*soft reset*/
    writel(3, RTC_SYSM_ADDR);
#endif

    for(;;);

    return 0;
}

int a9pv10_shut(void)    
{
    uint8_t addr, val;

    /*change to PMU poweroff sequence*/
    addr = 0x0E;
    iicreg_write(0, &addr, 0x01, &val, 0x00, 0x00);
    iicreg_read(0, &val, 0x1);
    val |= 0x1;
    iicreg_write(0, &addr, 0x01, &val, 0x01, 0x01);

    return 0;
}

int a9pv10_acon(void)
{
    uint8_t addr, val;

    /*use charge state, also can use GPIO1 input st*/
    addr = 0xBD;
    iicreg_write(0, &addr, 0x01, &val, 0x00, 0x00);
    iicreg_read(0, &val, 0x1); 
    return (val&0xC0)?1:0;
}

int a9pv10_bootst(void)  
{
    int tmp = rtcbit_get("resetflag");
    int slp = rtcbit_get("sleeping");
    uint8_t addr, val, val2, ret;
    static int flag = -1;
    int count = 0;

    if(flag >= 0) {
	printf("bootst exist: %d\n", flag);
	goto end;
    }

#if !defined(CONFIG_PRELOADER)
    flag = rbgetint("bootstats");
#else/* PRELOADER */

__retry__:
    ret = 1;

    addr = 0x09;
    ret &= iicreg_write(0, &addr, 0x01, &val, 0x00, 0x00);
    ret &= iicreg_read(0, &val, 0x1);
    if (!ret) {
	    count++;
	    if (count == 1) {
		    iic_init(0, 1, 0x64, 0, 1, 0);
		    printf("@@@ reinit iic0 controller @@@\n");
		    goto __retry__;
	    } else if (count == 2){
		    module_enable(GPIO_SYSM_ADDR);
		    iic_init(0, 1, 0x64, 0, 1, 0);
		    printf("@@@ reset gpio and iic0 @@@\n");
		    goto __retry__;
	    } else {
		    printf("@@@ recover iic0 FAIL, halt!!! @@@\n");
		    while(1);
	    }
    }

    if (count) {
	    printf("@@@ recover iic0 SUCCESS, halt!!! @@@\n");
	    while(1);
    }

    addr = 0x0A;
    iicreg_write(0, &addr, 0x01, &val2, 0x00, 0x00);
    iicreg_read(0, &val2, 0x1);
    printf("resetflag(%d), sleepflag(%d), poweron(0x%x), poweroff(0x%x)\n",
		    tmp, slp, val, val2);

#if 0
    printf("boot state(%d)\n", tmp);

    if(!(readl(SYS_RST_ST) & 0x2)) {
	flag = BOOT_ST_NORMAL;
	goto end;
    }
#endif

    if(tmp == 0) {
	if(slp == 0) {
	    if(val == 1) {
		flag = BOOT_ST_NORMAL;
	    }else if(val == 2) {
		    if (val2 == 0x1)
			    flag = BOOT_ST_NORMAL;
		    else
			    flag = BOOT_ST_RESET;
	    }else if(val == 4) {
		flag = BOOT_ST_NORMAL;
	    }
	    goto end;
	}else {/*sleep flag set*/
	    rtcbit_clear("sleeping");
	    flag = BOOT_ST_RESET;
	    goto end;
	}
    }else {
	if(slp) {/*sleep flag set*/
	    rtcbit_clear("sleeping");
	    flag = BOOT_ST_RESUME;
	    goto end;
	}
    }

    /* clear state off-hand */
    writel(3, SYS_RST_ST + 4);
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
	    printf("warning: invalid boot state\n", tmp);
	    break;
    }
#endif

end:
    if(!a9pv10_acon()) {
	uint8_t addr, val;
	addr = 0x0F;
	iicreg_write(0, &addr, 0x01, &val, 0x00, 0x00);
	iicreg_read(0, &val, 0x1);
	val &= ~(0x1);
	iicreg_write(0, &addr, 0x01, &val, 0x01, 0x01);
    }

    printf("---------------bootst: %d\n", flag);
    return flag;
}

