/*
 *
 */

#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <mach/io.h>
#include <mach/items.h>
#include <mach/belt.h>
#include <mach/imap-iomap.h>
#include <mach/imap-iic.h>
#include <mach/imap-emif.h>
#include <mach/imap-rtc.h>
#include <mach/sram.h>
#include <mach/rtcbits.h>

#define IMAP_IIC0_BASE_OFF              (IO_ADDRESS(IMAP_IIC0_BASE))

static uint8_t __sramdata low_power_data[6] = {
	0xf, 0x1, 0x31, 0xb, 0x12, 0x10 };
static uint8_t __sramdata low_power_len = 6;

/* [KP-peter]-TODO  add for compile */

extern int axp_charger_in(void);
extern void axp_power_off(void);

extern int read_rtc_gpx(int io_num);
extern void rtc_set_delayed_alarm(u32 delay);
/*int axp_charger_in(void)
{
	return 1;
}

void axp_power_off(void)
{
}*/


static int a5pv20_check_adapter(void)
{

#if !defined(CONFIG_IMAPX800_FPGA_PLATFORM)
	return axp_charger_in();
#else
	return 0;
#endif

}

void __sramfunc a5pv20_enter_lowpower(void)
{

	int i;
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCFG);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_DTUCFG);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCTL);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_STAT);
	__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_ACIOCR);
	__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_DSGCR);
	__raw_readl(IO_ADDRESS(SYSMGR_EMIF_BASE) + 0x44);
	__raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_GP_OUTPUT0);

	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_ENABLE);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_TAR);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_DATA_CMD);

	__raw_writel(1, IMAP_EMIF_ADDR_OFF + UMCTL_DTUCFG);
	__raw_writel(__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCFG) & (~(1 << 0)),
		     IMAP_EMIF_ADDR_OFF + UMCTL_SCFG);

	__raw_writel(SCTL_SLEEP, IMAP_EMIF_ADDR_OFF + UMCTL_SCTL);
	do {} while (STAT_LOW_POWER !=
	       (__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_STAT) &
		UMCTL_STAT_MASK));
	__raw_writel(__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_ACIOCR) |
		     (0x7 << 8 | 0xf << 18 | 0x1 << 3),
		     IMAP_EMIF_ADDR_OFF + PHY_ACIOCR);
	__raw_writel(__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_DSGCR) | (0xf << 20),
		     IMAP_EMIF_ADDR_OFF + PHY_DSGCR);
	__raw_writel(__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_DXCCR) | (0x3 << 2),
		     IMAP_EMIF_ADDR_OFF + PHY_DXCCR);
	__raw_writel(0xfc, IO_ADDRESS(SYSMGR_EMIF_BASE) + 0x44);

	/* freeze memory state */
	__raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_GP_OUTPUT0);

	__raw_writel(0, IMAP_IIC0_BASE_OFF + IIC_ENABLE);
	__raw_writel((0x68 >> 1), IMAP_IIC0_BASE_OFF + IIC_TAR);
	__raw_writel(1, IMAP_IIC0_BASE_OFF + IIC_ENABLE);

	for (i = 0; i < low_power_len; i++) {
		while (1) {
			if (__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS) &
			    (1 << 1)) {
				__raw_writel(low_power_data[i],
					     IMAP_IIC0_BASE_OFF + IIC_DATA_CMD);
				break;
			}
		}
	}

	do {} while (!(__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS)
		& (1 << 2)));
	do {} while (1);
	return;
}

void a5pv20_reset(char mode, const char *cmd)
{

#if !defined(CONFIG_IMAPX800_FPGA_PLATFORM)
	int tmp = 0;

	if (cmd && (cmd[0] == 'r')
	    && (cmd[1] == 'e')
	    && (cmd[2] == 'c')
	    && (cmd[3] == 'o')
	    && (cmd[4] == 'v')
	    && (cmd[5] == 'e')
	    && (cmd[6] == 'r')) {
		rtcbit_set("resetflag", BOOT_ST_RECOVERY);
	} else if (cmd && (cmd[0] == 'c')
		   && (cmd[1] == 'h')
		   && (cmd[2] == 'a')
		   && (cmd[3] == 'r')
		   && (cmd[4] == 'g')
		   && (cmd[5] == 'e')
		   && (cmd[6] == 'r')) {
		rtcbit_set("resetflag", BOOT_ST_CHARGER);
	} else
		rtcbit_set("resetflag", BOOT_ST_RESET);

	if (a5pv20_check_adapter()) {
		tmp = __raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
		tmp |= (0x1 << 1);
		__raw_writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
	}

	__raw_writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE));
#endif
	mdelay(5000);
	pr_err("a5pv20_reset ERROR!!!!!!\n");
	return;
}

void a5pv20_poweroff(void)
{
#if !defined(CONFIG_IMAPX800_FPGA_PLATFORM)
	writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x04);
	writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78) & ~0x4,
			IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78);

	axp_power_off();
#endif

	return;
}
