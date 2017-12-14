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
#include <mach/ip6313.h>
#define HW_RESET		(1)
//#define IMAP_IIC0_BASE_OFF	(IO_ADDRESS(IMAP_IIC0_BASE))

static uint8_t __sramdata low_power_data[2] = {0x02, 0x42};
static uint8_t __sramdata low_power_len = 2;

static int coronampw_ip6313_check_adapter(void)
{
	return 0;
}

//void __sramfunc coronampwpv10_enter_lowpower(void)
void coronampw_ip6313_enter_lowpower(void)
{
	return ;
	pr_err("%s--\n", __func__);
#if defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	int tmp = 0;
	writel(0xff, IO_ADDRESS(SYSMGR_GDMA_BASE));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x130));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x150));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x160));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x180));

	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x134));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x154));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x164));
	writel(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x184));

	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x13c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x15c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x16c));
	writel(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE + 0x18c));

	writel(0xff, IO_ADDRESS(SYSMGR_IIS_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CMNTIMER_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PWM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IIC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_PCM_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_SSP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_UART_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ADC_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_GPIO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_IDS0_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_CRYPTO_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_VENC265_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_ISP_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MIPI_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_USBH_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_OTG_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC1_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC2_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MMC0_BASE));
	writel(0xff, IO_ADDRESS(SYSMGR_MAC_BASE));
	__raw_writel(0x1, IMAP_EMIF_ADDR_OFF + 0x30);
	__raw_writel(0x01, IO_ADDRESS(SYSMGR_SYSMGR_BASE) + 0x14); // reset IROM
	asm("b 1f\n\t"
		".align 5\n\t"
		"1:\n\t"
		"mcr p15, 0, %0, c7, c10, 5\n\t"
		"mcr p15, 0, %0, c7, c10, 4\n\t"
		"wfi" : : "r" (tmp));
#else
#endif
}

void coronampw_ip6313_reset(char mode, const char *cmd)
{
	int tmp = 0;

	if (cmd) {
		if (!strncmp(cmd, "recover", 7))
			rtcbit_set("resetflag", BOOT_ST_RECOVERY);
		else if (!strncmp(cmd, "charger", 7))
			rtcbit_set("resetflag", BOOT_ST_CHARGER);
		else
			rtcbit_set("resetflag", BOOT_ST_RESET);
	}

	if (coronampw_ip6313_check_adapter()) {
		tmp = __raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
		tmp |= (0x1 << 1);
		__raw_writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
	}

#if HW_RESET
#ifdef CONFIG_CORONAMPW_MFD_IP6313
	ip6313_reset();
#endif
	//rtcbit_set("retry_reboot", 0);
	pr_err("HW_RESET: reset\n");
	mdelay(5000);
#else
	__raw_writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE)); /*soft reset */
	pr_err("RTC: reset\n");
#endif
	pr_err("WARNING: reset FAIL, retry!!!");
	while (1) {
		__raw_writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE));
		mdelay(5000);
		pr_err("WARNING: reset Retry FAIL!");
	}
}

void coronampw_ip6313_poweroff(void)
{
#if defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x04);
	writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78) & ~0x4,
			IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78);

	__raw_writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE + 0x28));
	__raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0x7c));
	__raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));
	asm("wfi");
#else
	writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x04);
	writel(readl(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78) & ~0x4,
			IO_ADDRESS(SYSMGR_RTC_BASE) + 0x78);
#ifdef CONFIG_CORONAMPW_MFD_IP6313
	ip6313_deep_sleep();
#endif
#endif
}
