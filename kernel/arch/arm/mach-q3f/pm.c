#include <linux/suspend.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/input.h>
#include <linux/amba/serial.h>
#include <linux/interrupt.h>
#include <linux/suspend.h>

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>
#include <linux/irqchip/arm-gic.h>
#include <asm/irq.h>
#include <asm/suspend.h>

#include <mach/imap-iomap.h>
#include <mach/imap-rtc.h>
#include <mach/imap-emif.h>
#include <mach/power-gate.h>
#include <mach/belt.h>
#include <mach/items.h>
#include <mach/memory.h>
#include <mach/sram.h>
#include <mach/pad.h>
#include <mach/irqs.h>
#include <mach/rtcbits.h>
#include <mach/gpio.h>
#include "core.h"

#define PM_INFO_LOG
#ifdef PM_INFO_LOG
#define PM_INFO(fmt, ...) printk(fmt, ##__VA_ARGS__)
#else
#define PM_INFO(fmt, ...)
#endif

extern void q3f_cpu_resume(void);
extern int q3f_cpu_save(unsigned long *saveblk, long);
extern void l2x0_flush_all(void);
extern void versatile_secondary_startup(void);
int (*pm_cpu_sleep)(unsigned long);

extern uint32_t emif_set_freq(uint32_t freq);

extern void __sramfunc emif_entry_low_power(void);
extern void __sramfunc emif_exit_low_power(void);

static unsigned int gic_enable_reg[256];
//static unsigned int gic_secure_reg[256];
static unsigned int gic_priority_reg[256];
static unsigned int gic_target_reg[256];
static unsigned int gic_config_reg[256];
static unsigned int gic_ctrl;
static unsigned int scu_control;
static unsigned int cpui_primask;
static unsigned int cpui_ctrl;
static unsigned int l2x0_aux_ctrl;
static unsigned int l2x0_ctrl;
static unsigned int pad_casemode;
static unsigned int pcloc_value;

static void pad_case_save(void)
{

	void __iomem *base = IO_ADDRESS(IMAP_SYSMGR_BASE) + 0x9000;
	pad_casemode = readl(base);

}

static void pad_case_resume(void)
{

	void __iomem *base = IO_ADDRESS(IMAP_SYSMGR_BASE) + 0x9000;
	writel(pad_casemode, base);
}

static void l2x0_save(void)
{

	void __iomem *base = IO_ADDRESS(IMAP_SCU_BASE) + 0x2000;

	l2x0_aux_ctrl = readl(base + L2X0_AUX_CTRL);
	l2x0_ctrl = readl(base + L2X0_CTRL);

}

static void l2x0_resume(void)
{

	void __iomem *base = IO_ADDRESS(IMAP_SCU_BASE) + 0x2000;

	writel(l2x0_aux_ctrl, base + L2X0_AUX_CTRL);
	writel(l2x0_ctrl, base + L2X0_CTRL);

}

static void q3f_gic_save(void)
{
	int i = 0;
	int length = 0;
	int count = 0;
	void __iomem *scu_base = IO_ADDRESS(IMAP_SCU_BASE);
	void __iomem *gic_dist_base = scu_base + 0x1000;
	void __iomem *gic_cpu_base = scu_base + 0x100;

	/*save gic ctrl register */
	gic_ctrl = readl(gic_dist_base + GIC_DIST_CTRL);

	/*save gic set enable register */
	length = 0x17c - 0x100;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		gic_enable_reg[i] =
		    readl(gic_dist_base + GIC_DIST_ENABLE_SET + i * 4);
	}
/*
 * [KP-peter]  GIC_DIST_SECURE make nosense
 */
#if 0
	/*save gic set secure register */
	length = 0xfc - 0x84;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		gic_secure_reg[i] =
		    readl(gic_dist_base + GIC_DIST_SECURE + 0x4 + i * 4);
	}
#endif
	/*save gic priority register */
	length = 0x7f8 - 0x400;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		gic_priority_reg[i] =
		    readl(gic_dist_base + GIC_DIST_PRI + i * 4);
	}

	/*save gic target register */
	length = 0xBF8 - 0x820;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		gic_target_reg[i] =
		    readl(gic_dist_base + GIC_DIST_TARGET + 0x20 + i * 4);
	}

	/*save gic config register */
	length = 0xcfc - 0xc04;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		gic_config_reg[i] =
		    readl(gic_dist_base + GIC_DIST_CONFIG + 0x4 + i * 4);
	}

	cpui_primask = readl(gic_cpu_base + GIC_CPU_PRIMASK);
	cpui_ctrl = readl(gic_cpu_base + GIC_CPU_CTRL);

	scu_control = readl(scu_base);
}

static void q3f_gic_restore(void)
{
	int i = 0;
	int length = 0;
	int count = 0;
	void __iomem *scu_base = IO_ADDRESS(IMAP_SCU_BASE);
	void __iomem *gic_dist_base = scu_base + 0x1000;
	void __iomem *gic_cpu_base = scu_base + 0x100;

	/*restore gic set enable register */
	length = 0x17c - 0x100;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		writel(gic_enable_reg[i],
		       gic_dist_base + GIC_DIST_ENABLE_SET + i * 4);
	}

/*
 *  [KP-peter] FIXME GIC_DIST_SECURE make nosense
 */
#if 0
	/*restore gic set secure register */
	length = 0xfc - 0x84;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		writel(gic_secure_reg[i],
		       gic_dist_base + GIC_DIST_SECURE + 0x4 + i * 4);
	}
#endif
	/*restore gic priority register */
	length = 0x7f8 - 0x400;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		writel(gic_priority_reg[i],
		       gic_dist_base + GIC_DIST_PRI + i * 4);
	}

	/*restore gic target register */
	length = 0xBF8 - 0x820;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		writel(gic_target_reg[i],
		       gic_dist_base + GIC_DIST_TARGET + 0x20 + i * 4);
	}

	/*restore gic config register */
	length = 0xcfc - 0xc04;
	count = length / 4 + 1;
	for (i = 0; i < count; i++) {
		writel(gic_config_reg[i],
		       gic_dist_base + GIC_DIST_CONFIG + 0x4 + i * 4);
	}

	writel(cpui_primask, gic_cpu_base + GIC_CPU_PRIMASK);
	writel(cpui_ctrl, gic_cpu_base + GIC_CPU_CTRL);

	/*restore gic ctrl register */
	writel(gic_ctrl, gic_dist_base + GIC_DIST_CTRL);

	writel(scu_control, scu_base);
}

static void q3f_gic_disable(void)
{
	int length = 0, count = 0;
	int i = 0;
	void __iomem *gic_dist_base = IO_ADDRESS(IMAP_SCU_BASE) + 0x1000;

	__raw_writel(0x0, gic_dist_base + GIC_DIST_CTRL);

	/*gic set clear enable register */
	length = 0x1fc - 0x180;
	count = length / 4 + 1;
	for (i = 0; i < count; i++)
		__raw_writel(0xffffffff,
			     gic_dist_base + GIC_DIST_ENABLE_CLEAR + i * 4);
}

/* callback from assembly code */
void q3f_pm_cb_flushcache(void)
{
	flush_cache_all();
	outer_flush_all();
}

static int q3f_pm_prepare(void)
{
	rtcbit_set("resetflag", BOOT_ST_RESUME);
	rtcbit_set("sleeping", 1);
	pcloc_value = readl(phys_to_virt(IMAP_SDRAM_BASE + 0xe0));
	writel(virt_to_phys(q3f_cpu_resume), phys_to_virt(IMAP_SDRAM_BASE + 0xe0));
	return 0;
}

void imap_resume_l2x0(void)
{
	int lt, ld;
	void __iomem *base = IO_ADDRESS(IMAP_SCU_BASE) + 0x2000;

	/* 256KB (16KB/way), 16-way associativity,
	 * evmon/parity/share enabled
	 * Bits:  .... ...0 0111 0011 0000 .... .... .... */
	writel(0x2, IO_ADDRESS(IMAP_SYSMGR_BASE + 0x884c));

	lt = readl_relaxed(base + L2X0_TAG_LATENCY_CTRL);
    lt &= ~(0x7<<8 | 0x7<<4 | 0x7);
    lt |=  (0x0<<8 | 0x1<<4 | 0x0);
	writel_relaxed(lt, base + L2X0_TAG_LATENCY_CTRL);

	ld = readl_relaxed(base + L2X0_DATA_LATENCY_CTRL);
    ld &= ~(0x7<<8 | 0x7<<4 | 0x7);
    ld |=  (0x0<<8 | 0x1<<4 | 0x0);
	writel_relaxed(ld, base + L2X0_DATA_LATENCY_CTRL);

	/* common l2x0 init */
	l2x0_resume();
}

#ifdef CONFIG_CACHE_L2X0
extern void __init imap_init_l2x0(void);
extern int cmn_timer_resume(void);
#endif

extern int console_suspend_enabled;
extern void imap_sram_reinit(void);

static void pl011_resume_reset(int baudrate)
{
	struct clk *uart = clk_get_sys("imap-uart.3", NULL);
	int rate, ibr, fbr;
	uint32_t uart_im;
	void __iomem *base = ioremap(IMAP_UART3_BASE, IMAP_UART3_SIZE);

	if (IS_ERR(uart) || base == NULL)
		return;

#if defined(CONFIG_APOLLO_FPGA_PLATFORM) || defined(CONFIG_Q3F_FPGA_PLATFORM)
	rate = CONFIG_FPGA_UART_CLK;
#else
	rate = clk_get_rate(uart);
#endif
	ibr = (rate / (16 * baudrate)) & 0xffff;
	fbr = (((rate - ibr * (16 * baudrate)) * 64) / (16 * baudrate)) & 0x3f;
	/* for resume uart */
	writew(fbr, base + UART011_FBRD);
	writew(ibr, base + UART011_IBRD);
	writew(0x70, base + UART011_LCRH);
	writew(0x301, base + UART011_CR);
	uart_im = readw(base + UART011_IMSC);
	uart_im |= UART011_TXIM;
	writew(uart_im, base + UART011_IMSC);
}

static void q3f_module_power_on(void)
{
	int val;

	/* for dma power on and set dma to secure mode */
	module_power_on(SYSMGR_GDMA_BASE);
	val = readl(IO_ADDRESS(SYSMGR_GDMA_BASE) + 0x20);
	val |= (1 << 0);
	writel(val, IO_ADDRESS(SYSMGR_GDMA_BASE) + 0x20);

}

void rtc_set_delayed_alarm(u32 delay);

static int q3f_cpu_suspend(unsigned long arg)
{
	/*volatile unsigned int hw_ddr_lowpow = 0;*/
	unsigned int hw_ddr_lowpow = 0;
	void __iomem *rtc_sys = IO_ADDRESS(SYSMGR_RTC_BASE);
	void __iomem *emif_base = IO_ADDRESS(IMAP_EMIF_BASE);
	int flag_charger_func = 0;
	int flag_charger_state = 0;
	int reg_data;
	printk(KERN_ERR "%s---%d\n",__func__,__LINE__);

	if (item_exist("power.acboot")) {
		if (item_equal("power.acboot", "1", 0)) {
			flag_charger_func = 1;
			if (readl(rtc_sys + SYS_RTC_IO_IN) & 0x04)
				flag_charger_state = 1;
		}
	}

	q3f_gic_disable();
	q3f_pm_cb_flushcache();

/*
	if (!item_equal("board.arch", "a5pv20", 0)
			&& !item_equal("board.arch", "q3pv10", 0)) {
		hw_ddr_lowpow = __raw_readl(emif_base);
		hw_ddr_lowpow |= 0x1;
		__raw_writel(hw_ddr_lowpow, emif_base);
	}
*/

	__raw_writel(0xff, rtc_sys + SYS_INT_CLR);
	__raw_writel(0x03, rtc_sys + SYS_RST_CLR);
	__raw_writel(0x03, rtc_sys + SYS_POWUP_CLR);
	__raw_writel(0xfe, rtc_sys + SYS_INT_MASK);
	//__raw_writel(0xf, rtc_sys + SYS_IO_CFG);

	if (flag_charger_func) {
		if (flag_charger_state) {
			reg_data = readl(rtc_sys + SYS_IO_CFG);
			reg_data |= 0x40;
			writel(reg_data, rtc_sys + SYS_IO_CFG);

			reg_data = readl(rtc_sys + SYS_WAKEUP_MASK);
			reg_data &= ~0x04;
			writel(reg_data, rtc_sys + SYS_WAKEUP_MASK);
		} else {
			/*charger is not exist*/
			reg_data = readl(rtc_sys + SYS_IO_CFG);
			reg_data &= ~0x40;
			writel(reg_data, rtc_sys + SYS_IO_CFG);

			reg_data = readl(rtc_sys + SYS_WAKEUP_MASK);
			reg_data &= ~0x04;
			writel(reg_data, rtc_sys + SYS_WAKEUP_MASK);

		}
	}

	pr_err("---> step 2 <---\n");
	q3f_enter_lowpower();
	panic("sleep resumed to originator?");
}

static int q3f_pm_enter(suspend_state_t state)
{
	if (item_equal("board.arch", "a5pv20", 0) 
			|| item_equal("board.arch", "q3pv10", 0))
		imap_sram_reinit();

	q3f_gpio_save();
	pad_case_save();
	l2x0_save();
	q3f_gic_save();
	/* call cpu specific preparation */
	q3f_pm_prepare();
	/* flush cache back to ram */
	cpu_suspend(0, pm_cpu_sleep);

#ifdef CONFIG_CACHE_L2X0
	outer_inv_all();
	imap_resume_l2x0();
#endif
	cpu_init();
	writel(pcloc_value, (unsigned int *)0xc00000e0);
	writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE + 0xC));
	writel(0x3, IO_ADDRESS(SYSMGR_RTC_BASE + 0x4));

	q3f_gic_restore();
	pad_case_resume();
	module_power_on(SYSMGR_GPIO_BASE);
	q3f_gpio_restore();

	return 0;
}

static void q3f_pm_finish(void)
{
	PM_INFO("++ %s ++\n", __func__);
}

static void q3f_pm_wake(void)
{
	q3f_module_power_on();
	/* for uart resume */
	if (!console_suspend_enabled) {
		imapx_pad_init("uart3");
		pl011_resume_reset(115200);
	}
}

static const struct platform_suspend_ops q3f_pm_ops = {
	.enter = q3f_pm_enter,
	.prepare = q3f_pm_prepare,
	.finish = q3f_pm_finish,
	.valid = suspend_valid_only_mem,
	.wake = q3f_pm_wake,
};



void __init q3f_init_pm(void)
{
	PM_INFO("++ %s ++\n", __func__);
	pm_cpu_sleep = q3f_cpu_suspend;
	suspend_set_ops(&q3f_pm_ops);
	return;
}

