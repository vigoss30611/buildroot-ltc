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
#define IMAP_IIC0_BASE_OFF	(IO_ADDRESS(IMAP_IIC0_BASE))

static uint8_t __sramdata low_power_data[2] = {0x0, 0xf1};
static uint8_t __sramdata low_power_len = 2;

static int apollo3_ip6313_check_adapter(void)
{
	return 0;
}

//void __sramfunc apollo3pv10_enter_lowpower(void)
void __sramfunc apollo3_ip6313_enter_lowpower(void){
	int i;

#if 1
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCFG);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_DTUCFG);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCTL);
	__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_STAT);
	__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_ACIOCR);
	__raw_readl(IMAP_EMIF_ADDR_OFF + PHY_DSGCR);
	__raw_readl(IO_ADDRESS(SYSMGR_EMIF_BASE) + 0x44);
	__raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_GP_OUTPUT0);
#endif

	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_ENABLE);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_TAR);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS);
	__raw_readl(IMAP_IIC0_BASE_OFF + IIC_DATA_CMD);

	/*__raw_writel(1, IMAP_EMIF_ADDR_OFF + UMCTL_DTUCFG);
	__raw_writel(__raw_readl(IMAP_EMIF_ADDR_OFF + UMCTL_SCFG) & (~(1 << 0)),
		     IMAP_EMIF_ADDR_OFF + UMCTL_SCFG);*/

#if 1
	__raw_writel(0x1, IMAP_EMIF_ADDR_OFF + 0x30);
	do {} while (0x3 !=
	       (__raw_readl(IMAP_EMIF_ADDR_OFF + 0x4) & 0x7));
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
#endif
	__raw_writel(0, IMAP_IIC0_BASE_OFF + IIC_ENABLE);
	__raw_writel(0x30, IMAP_IIC0_BASE_OFF + IIC_TAR);
	__raw_writel(1, IMAP_IIC0_BASE_OFF + IIC_ENABLE);
	for (i = 0; i < low_power_len; i++) {
		while (1) {
			if (__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS) & (1 << 1)) {
				__raw_writel(low_power_data[i], IMAP_IIC0_BASE_OFF + IIC_DATA_CMD);
				break;
			}
		}
	}
	do {} while (!(__raw_readl(IMAP_IIC0_BASE_OFF + IIC_STATUS) & (1 << 2)));
	do {} while (1);
}

void apollo3_ip6313_reset(char mode, const char *cmd)
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

	if (apollo3_ip6313_check_adapter()) {
		tmp = __raw_readl(IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
		tmp |= (0x1 << 1);
		__raw_writel(tmp, IO_ADDRESS(SYSMGR_RTC_BASE + 0x60));
	}

#if HW_RESET
#ifdef CONFIG_APOLLO3_MFD_IP6313
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

void apollo3_ip6313_poweroff(void)
{
#if defined(CONFIG_APOLLO3_FPGA_PLATFORM)
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
#ifdef CONFIG_APOLLO3_MFD_IP6313
	ip6313_deep_sleep();
#endif
#endif
}
