/*
 *  arch/arm/mach-imapx800/board-a5pv10.c
 */

#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <mach/io.h>
#include <mach/items.h>
#include <mach/belt.h>
#include <mach/imap-iomap.h>
#include <mach/rtcbits.h>

extern void rtc_set_delayed_alarm(u32 delay);
extern int read_rtc_gpx(int ionum);

static int  a5pv10_check_adapter(void)
{
	return !!read_rtc_gpx(2);
}

void a5pv10_reset(char mode, const  char *cmd)
{

	int tmp = 0, flag = BOOT_ST_RESET;
	if (cmd) {
		if (strncmp(cmd, "recover", 7) == 0)
			flag = BOOT_ST_RECOVERY;
		else if (strncmp(cmd, "charger", 7) == 0)
			flag = BOOT_ST_CHARGER;
	}

	rtcbit_set("resetflag", flag);
	if (a5pv10_check_adapter()) {
		tmp = readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
		tmp |= (0x1 << 1);
		writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
	}

	writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE));

	/* never reach */
	do {} while (1);
	return;
}

void a5pv10_poweroff(void)
{
	int try_unmask = 1;
    /* as knowing: there are two bug during shuting down
     *
     *   1: charger power mask on && charger in
     *        enter bug state whether soft or force shutting down
     *   2: charger power mask on && charger not in
     *      enter bug state if force shutting down
     *
     *      so we must use a trick to avoid the machine from entering
     *      bug state if user wants to shut down in these situations.
     *
     *  charger.pwron is to be depracated as it's not a standard
     *  method to perform charging
     */

	if (item_exist("power.acboot") && item_equal("power.acboot", "1", 0)) {
		if (a5pv10_check_adapter())
			a5pv10_reset(0, "charger");
	} else {
		try_unmask = 0;
	}

	writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x04);
	writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78) & ~0x4,
			IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78);

	if (try_unmask && !a5pv10_check_adapter())
		writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60)) & ~0x2,
			IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));

    /* shut down */
	__raw_writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE + 0x28));
	__raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0x7c));
	__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));
	asm("wfi");
}

void a5pv10_enter_lowpower(void)
{
	int tmp;
	writel(0xff, IO_ADDRESS(SYSMGR_GDMA_BASE));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x130));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x140));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x150));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x160));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x170));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x180));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x190));

	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x134));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x144));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x154));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x164));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x174));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x184));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x194));

	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x13c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x14c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x15c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x16c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x17c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x18c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x19c));

	writel(0xff, IO_ADDRESS(SYSMGR_IIS_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CMNTIMER_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IIC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PCM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_SSP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_UART_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ADC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_GPIO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWDT_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IDS1_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CRYPTO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VDEC264_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VENC264_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VDEC265_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VENC265_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ISP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MIPI_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_USBH_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_OTG_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC1_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC2_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC0_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MAC_BASE));
	__raw_writel(0x01, IO_ADDRESS(SYSMGR_SYSMGR_BASE) + 0x14);


	tmp = 0;
	__raw_writel(0x01, IO_ADDRESS(0x21a00030));
	asm("b 1f\n\t"
		".align 5\n\t"
		"1:\n\t"
		"mcr p15, 0, %0, c7, c10, 5\n\t"
		"mcr p15, 0, %0, c7, c10, 4\n\t"
		"wfi" : : "r" (tmp));
}


