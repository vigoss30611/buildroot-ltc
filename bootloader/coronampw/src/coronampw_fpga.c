 #include <asm-arm/io.h>
 #include <bootlist.h>
 #include <dramc_init.h>
 #include <items.h>
 #include <preloader.h>
 #include <rballoc.h>
 #include <rtcbits.h>
#include <asm-arm/arch-coronampw/imapx_base_reg.h>
#include <asm-arm/arch-coronampw/imapx_pads.h>


#if defined(CONFIG_PRELOADER)
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#endif

int coronampw_fpga_binit(void)
{
	return 0;
}

int coronampw_fpga_reset(void)
{
	for(;;);
	return 0;
}

int coronampw_fpga_shut(void)
{
	return 0;
}

int coronampw_fpga_bootst(void)
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

int coronampw_fpga_acon(void)
{
	return 0;
}
