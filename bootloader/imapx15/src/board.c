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
#include <imapx_iic.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#define module_enable irf->module_enable
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

/*********************************a5pv20 ****************************************/
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

	addr = 0x91;
	buf[0] = 0xf5;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	addr = 0x90;
	buf[0] = 0x03;
	iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);

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

/*****************************************a9pv10************************/
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

/**************************************************************************************/

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
    }
};

static struct arch_functions *af = archf;

void board_scan_arch(void)
{
    int i;

    for(i = 0; i < ARRAY_SIZE(archf); i++)
        if(item_equal("board.arch", archf[i].arch, 0))
            af = &archf[i];

    printf("board arch is set to: %s\n", af->arch);
}

void board_init_i2c(void)
{
	if (!item_exist("board.arch") ||
			item_equal("board.arch", "a5pv10", 0) ||
			item_equal("board.arch", "a5pv20", 0))
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


