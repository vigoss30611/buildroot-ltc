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

int q3pv10_binit(void)
{
	/*uint8_t addr, buf;

	iic_init(0, 1, 0x60, 0, 1, 0);

	addr = 0x4d;
	iicreg_write(0, &addr, 0x01, &buf, 0x00, 0x01);
	iicreg_read(0, &buf, 0x1);
	buf |= 0x08;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);

	addr = 0x80;
	iicreg_write(0, &addr, 0x01, &buf, 0x00, 0x01);
	iicreg_read(0, &buf, 0x1);
	buf |= 0x0e;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);

	addr = 0x21;
	buf = 0x28;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);*/

	return 0;
}

int q3pv10_reset(void)
{
	uint8_t addr, buf;

	rtcbit_set("resetflag", BOOT_ST_RESET);

#if 1
	addr = 0x01;
	buf = 0x08;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);
#else
	writel(3, RTC_SYSM_ADDR);
#endif

	for(;;);

	return 0;
}

int q3pv10_shut(void)
{
	uint8_t addr, buf;

	addr = 0x08;
	buf = 0x80;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);

	addr = 0x02;
	buf = 0x42;
	iicreg_write(0, &addr, 0x01, &buf, 0x01, 0x01);

	return 0;
}

int q3pv10_bootst(void)
{
	uint8_t addr, buf, val;
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
	flag = BOOT_ST_NORMAL;
	goto end;

	addr = 0x00;
	iicreg_write(0, &addr, 0x01, &val, 0x00, 0x01);
	iicreg_read(0, &val, 0x1);

	addr = 0x08;
	iicreg_write(0, &addr, 0x01, &buf, 0x00, 0x01);
	iicreg_read(0, &buf, 0x1);

	printf("resetflag=(0x%x), poweron=(0x%x), sleep_ctrl=(0x%x)\n",
			tmp, val, buf);

	if (buf & 0x80) {
		addr = 0x08;
		val = 0;
		iicreg_write(0, &addr, 0x01, &val, 0x01, 0x01);
		flag = BOOT_ST_RESUME;
		goto end;
	} else if (val & 0x08) {
		flag = BOOT_ST_RESET;
		goto end;
	}

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
			printf("warning: invalid boot state\n", tmp);
			break;
	}
#endif

end:
	printf("---------------bootst: %d\n", flag);
	return flag;
}

int q3pv10_acon(void)
{
	return 0;
}
