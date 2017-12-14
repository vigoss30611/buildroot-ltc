#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/string.h>
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
#include <mach/ricoh618.h>
#include <mach/ricoh618_battery.h>

#define HW_RESET	(1)

static int a9pv10_check_adapter(void)
{
	return ricoh61x_charger_in();
}

void a9pv10_enter_lowpower(void)
{
	int tmp = 0;
	void __iomem *rtc_sys = IO_ADDRESS(SYSMGR_RTC_BASE);

	/*RTC_GP1 config */
	tmp = readl(rtc_sys + SYS_IO_CFG);
	tmp |= 0x20;
	writel(tmp, rtc_sys + SYS_IO_CFG);
	writel(0xff, rtc_sys + 0x4);
	tmp = readl(rtc_sys + SYS_WAKEUP_MASK);
	tmp &= ~0x2;
	writel(tmp, rtc_sys + SYS_WAKEUP_MASK);

	/* rtc powerup enable*/
	tmp = readl(rtc_sys + SYS_ALARM_WEN);
	tmp |= 0xfc;
	writel(tmp, rtc_sys + SYS_ALARM_WEN);

	/* rtc powerup time max, disable auto clr*/
	writel(0x7f, rtc_sys + 0x1D0);

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

	writel(0xff, IO_ADDRESS(SYSMGR_G2D_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IIS_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CMNTIMER_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IIC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PCM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_SSP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_SPDIF_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_SIMC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PIC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_AC97_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_KEYBD_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ADC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_DMIC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_GPIO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWDT_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWMA_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IDS0_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IDS1_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_GPS_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CRYPTO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_GPU_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VDEC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VENC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ISP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MIPI_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_TSIF_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_USBH_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_OTG_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC1_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC2_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC0_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_NAND_BASE));
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

	return;
}

void a9pv10_reset(char mode, const char *cmd)
{
	int tmp = 0;

	pr_err("a9pv10_reset: in\n");

	if (cmd) {
		if (!strncmp(cmd, "recover", 7))
			rtcbit_set("resetflag", BOOT_ST_RECOVERY);
		else if (!strncmp(cmd, "charger", 7))
			rtcbit_set("resetflag", BOOT_ST_CHARGER);
		else
			rtcbit_set("resetflag", BOOT_ST_RESET);
	}

	if (a9pv10_check_adapter()) {
		tmp = __raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
		tmp |= (0x1 << 1);
		__raw_writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
	}
	pr_err("a9pv10_reset: check adapter ok, next set repwron\n");
#if HW_RESET
	/* AP soft shutdown, this will cause PMU power off, so
	 * we must ensure PMU REPWRON func is enabled here. After
	 * boot on, we should check whether boot reason is reset,
	 * if so, disable PMU REPWRON function as default*/

	/*hw reset nand power off, clean rtc retry level*/
	rtcbit_set("retry_reboot", 0);

	ricoh61x_set_repwron(1);
	pr_err("a9pv10_reset: set repwron ok, ready to reset\n");
	mdelay(500);
	__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));	/*soft shutdown */
#else
	__raw_writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE));	/*soft reset */
#endif

	mdelay(5000);
	pr_err("WARNING: reset FAIL, retry!!!");
	while (1) {
		__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));
		mdelay(5000);
		pr_err("WARNING: reset Retry FAIL!");
	}

	return;
}

void a9pv10_poweroff(void)
{
	writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x04);
	writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78) & ~0x4,
			IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78);

	/* if adapter is connected, ensure power on after
	 * power off, we choose reset flow in this case*/
	if (item_exist("power.acboot")
	    && item_equal("power.acboot", "1", 0)
	    && a9pv10_check_adapter()) {
		a9pv10_reset(0, "charger");
	} else {
		/* pmu power off */
		ricoh618_power_off();
	}

	return;
}
