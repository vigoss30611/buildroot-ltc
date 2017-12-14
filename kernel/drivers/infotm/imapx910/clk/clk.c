#include <linux/clkdev.h>
#include <linux/syscore_ops.h>
#include <mach/imap-iomap.h>
#include <mach/clk.h>
#include <linux/clk-provider.h>
#include <mach/items.h>

static struct clk *clks[clk_max];
static int clk_num;

static const char *pll_clk[] = {
	"apll", "dpll", "epll", "vpll"
};

static const char *dev_parent[] = {
	"apll", "dpll", "epll", "vpll", "osc-clk"
};
/*
 * for dev clk on bus, the range is the normal work range
 * about bus
 * name: device name,
 * max~min: bus work range
 */
static struct bus_freq_range bus_range[7] = {
	BUS_FREQ_RANGE("bus1", 300000, 100000),
	BUS_FREQ_RANGE("bus2", 300000, 100000),
	BUS_FREQ_RANGE("bus3", 500000, 40000),
	BUS_FREQ_RANGE("bus4", 300000, 100000),
	BUS_FREQ_RANGE("bus5", 300000, 100000),
	BUS_FREQ_RANGE("bus6", 300000, 100000),
	BUS_FREQ_RANGE("bus7", 300000, 30000),
};

static struct bus_freq_range apb_range =
				BUS_FREQ_RANGE("apb_pclk",  300000, 100000);

/*
 * for dev clk on pll, parameter represent:
 * dev_id: id of clk
 * nco_en: whether enable nco mode or not
 * clk_src: parent clk id
 * nco_value: if nco mode enable (nco_en = 0),
 *			  clk = parent_rate * nco_value / 256
 * clk_divider: if div mode enable (nco_en = 1),
 *				clk = parent_rate / (clk_divider + 1)
 * nco_disable: it is only used in clk_set_rate interface,
 *				if enable, it will compare with nco or div
 *				mode, the better divider mode will be used.
 */
static struct bus_and_dev_clk_info dev_clk_info[] = {
	DEV_CLK_INFO(BUS1, 0, EPLL, 0, 3, ENABLE),
	DEV_CLK_INFO(BUS2, 0, EPLL, 0, 3, ENABLE),
	/* default bus3 setting by shaft */
	DEV_CLK_INFO(BUS3, 0, EPLL, 0, 0, DISABLE),
	/* bus4 bus5 do not use in imapx9*/
	DEV_CLK_INFO(BUS4, 0, EPLL, 0, 4, ENABLE),
	DEV_CLK_INFO(BUS5, 0, DPLL, 0, 9, ENABLE),
	DEV_CLK_INFO(BUS6, 0, EPLL, 0, 6, ENABLE),
	/* dpll cannot give us clk>240M by ayakashi */
	DEV_CLK_INFO(BUS7, 0, DPLL, 0, 4, ENABLE),
	DEV_CLK_INFO(IDS0_EITF_CLK_SRC, 0, EPLL, 0, 22, DISABLE),
	DEV_CLK_INFO(IDS0_OSD_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	DEV_CLK_INFO(IDS0_TVIF_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	DEV_CLK_INFO(IDS1_EITF_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	DEV_CLK_INFO(IDS1_OSD_CLK_SRC, 0, EPLL, 0, 4, DISABLE),
	DEV_CLK_INFO(IDS1_TVIF_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	DEV_CLK_INFO(MIPI_DPHY_CON_CLK_SRC, 1, EPLL, 6, 0, ENABLE),
	DEV_CLK_INFO(MIPI_DPHY_REF_CLK_SRC, 1, EPLL, 6, 0, ENABLE),
	DEV_CLK_INFO(MIPI_DPHY_PIXEL_CLK_SRC, 0, EPLL, 0, 0, DISABLE),
	DEV_CLK_INFO(ISP_OSD_CLK_SRC, 0, DPLL, 0, 5, ENABLE),
	DEV_CLK_INFO(SD0_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	DEV_CLK_INFO(SD1_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	DEV_CLK_INFO(SD2_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	DEV_CLK_INFO(NFECC_CLK_SRC, 0, EPLL, 0, 3, ENABLE),
	DEV_CLK_INFO(HDMI_CLK_SRC, 1, DPLL, 4, 31, ENABLE),
	DEV_CLK_INFO(UART_CLK_SRC, 0, EPLL, 0, 3, ENABLE),
	DEV_CLK_INFO(SPDIF_CLK_SRC, 0, DPLL, 0, 24, DISABLE),
	DEV_CLK_INFO(AUDIO_CLK_SRC, 0, DPLL, 0, 24, DISABLE),
	DEV_CLK_INFO(USBREF_CLK_SRC, 1, DPLL, 4, 31, ENABLE),
	DEV_CLK_INFO(CLKOUT0_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	DEV_CLK_INFO(CLKOUT1_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	DEV_CLK_INFO(SATAPHY_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	DEV_CLK_INFO(CAMO_CLK_SRC, 1, DPLL, 4, 19, ENABLE),
	DEV_CLK_INFO(DDRPHY_CLK_SRC, 0, VPLL, 0, 1, ENABLE),
	DEV_CLK_INFO(SD3_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	DEV_CLK_INFO(ACODEC_CLK_SRC, 0, OSC_CLK, 0, 0, ENABLE),
	DEV_CLK_INFO(SYSBUS_CLK_SRC, 0, EPLL, 0, 5, ENABLE),
	DEV_CLK_INFO(APB_CLK_SRC, 0, EPLL, 0, 9, ENABLE),
};

static struct imapx_clk_init dev_clk[] = {
	CLK_INIT("bus1", NULL, NULL, BUS1),
	CLK_INIT("bus2", NULL, NULL, BUS2),
	CLK_INIT("bus3", NULL, "bus3", BUS3),
	CLK_INIT("bus4", NULL, NULL, BUS4),
	CLK_INIT("bus5", NULL, NULL, BUS5),
	CLK_INIT("bus6", NULL, NULL, BUS6),
	CLK_INIT("bus7", NULL, NULL, BUS7),
	CLK_INIT("ids0-eitf", "imap-ids0-eitf", "imap-ids0", IDS0_EITF_CLK_SRC),
	CLK_INIT("ids0-ods", "imap-ids0-ods", "imap-ids0", IDS0_OSD_CLK_SRC),
	CLK_INIT("ids0-tvif", "imap-ids0-tvif", "imap-ids0", IDS0_TVIF_CLK_SRC),
	CLK_INIT("ids1-eitf", "imap-ids1-eitf", "imap-ids1", IDS1_EITF_CLK_SRC),
	CLK_INIT("ids1-ods", "imap-ids1-ods", "imap-ids1", IDS1_OSD_CLK_SRC),
	CLK_INIT("ids1-tvif", "imap-ids1-tvif", "imap-ids1", IDS1_TVIF_CLK_SRC),
	CLK_INIT("mipi-dphy-con", "imap-mipi-dphy-con",
			"imap-mipi-dsi", MIPI_DPHY_CON_CLK_SRC),
	CLK_INIT("mipi-dphy-ref", "imap-mipi-dphy-ref",
			"imap-mipi-dsi", MIPI_DPHY_REF_CLK_SRC),
	CLK_INIT("mipi-dphy-pix", "imap-mipi-dphy-pix",
			"imap-mipi-dsi", MIPI_DPHY_PIXEL_CLK_SRC),
	CLK_INIT("isp-osd", "imap-isp-osd", "imap-isp-osd", ISP_OSD_CLK_SRC),
	CLK_INIT("sd-mmc0", "imap-mmc.0", "sd-mmc0", SD0_CLK_SRC),
	CLK_INIT("sd-mmc1", "imap-mmc.1", "sd-mmc1", SD1_CLK_SRC),
	CLK_INIT("sd-mmc2", "imap-mmc.2", "sd-mmc2", SD2_CLK_SRC),
	CLK_INIT("nand-ecc", "imap-nand-ecc", "imap-nand", NFECC_CLK_SRC),
	CLK_INIT("hdmi", "imap-hdmi", "imap-hdmi", HDMI_CLK_SRC),
	CLK_INIT("uart0", "imap-uart.0", NULL, UART_CLK_SRC),
	CLK_INIT("uart1", "imap-uart.1", NULL, UART_CLK_SRC),
	CLK_INIT("uart2", "imap-uart.2", NULL, UART_CLK_SRC),
	CLK_INIT("uart3", "imap-uart.3", NULL, UART_CLK_SRC),
	CLK_INIT("spdif", "imap-spdif", "imap-spdif", SPDIF_CLK_SRC),
	CLK_INIT("audio-clk", "imap-audio-clk",
			"imap-audio-clk", AUDIO_CLK_SRC),
	CLK_INIT("usb-ref", "imap-usb-ref", "imap-usb", USBREF_CLK_SRC),
	CLK_INIT("clk-out0", "imap-clk.0", "imap-clk.0", CLKOUT0_CLK_SRC),
	CLK_INIT("clk-out1", "imap-clk.1", "imap-clk.1", CLKOUT1_CLK_SRC),
	CLK_INIT("sata-phy", "imap-sata-phy", "imap-sata", SATAPHY_CLK_SRC),
	CLK_INIT("camo", "imap-camo", "imap-camo", CAMO_CLK_SRC),
	CLK_INIT("ddr-phy", "imap-ddr-phy", "imap-ddr-phy", DDRPHY_CLK_SRC),
	CLK_INIT("sd-mmc3", "imap-mmc.3", "sd-mmc3", SD3_CLK_SRC),
	CLK_INIT("acodec", "imap-acodec", "imap-acodec", ACODEC_CLK_SRC),
	CLK_INIT("sysbus", "imap-sysbus", "imap-sysbus", SYSBUS_CLK_SRC),
	CLK_INIT("apb_pclk",  NULL, "apb_pclk", APB_CLK_SRC),
};

static struct imapx_clk_init bus_clk[] = {
	CLK_INIT("ids0", "imap-ids.0", "imap-ids0", BUS1),
	CLK_INIT("dsi", "imap-dsi", "imap-dsi", BUS1),
	CLK_INIT("ids1", "imap-ids.1", "imap-ids1", BUS2),
	CLK_INIT("hdmi-tx", "imap-hdmi-tx", "imap-hdmi-tx", BUS2),
	CLK_INIT("gpu", "imap-gpu", "imap-gpu", BUS3),
	CLK_INIT("usb-ohci", "imap-usb-ohci", "imap-usb", BUS6),
	CLK_INIT("usb-ehci", "imap-usb-ehci", "imap-usb", BUS6),
	CLK_INIT("usb-otg", "imap-otg", NULL, BUS6),
	CLK_INIT("sdmmc0", "sdmmc.0", "sdmmc0", BUS6),
	CLK_INIT("sdmmc1", "sdmmc.1", "sdmmc1", BUS6),
	CLK_INIT("sdmmc2", "sdmmc.2", "sdmmc2", BUS6),
	CLK_INIT("sdmmc3", "sdmmc.3", "sdmmc3", BUS6),
	CLK_INIT("nandflash", "imap-nandflash", "imap-nand", BUS7),
	CLK_INIT("vdec", "imap-vdec", NULL, BUS7),
	CLK_INIT("isp", "imap-isp", "imap-isp", BUS7),
	CLK_INIT("eth", "imap-mac", NULL, BUS7),
	CLK_INIT("mipi-csi", "imap-mipi-csi", NULL, BUS7),
	CLK_INIT("venc", "imap-venc", NULL, BUS7),

};

static struct imapx_clk_init apb_clk[] = {
	CLK_APB_INIT("iis", "imap-iis", "imap-iis"),
	CLK_APB_INIT("pcm0", "imap-pcm.0", "imap-pcm"),
	CLK_APB_INIT("pcm1", "imap-pcm.1", "imap-pcm"),
	CLK_APB_INIT("spdif-core", "imap-spdif-core", "imap-spdif-core"),
	CLK_APB_INIT("audiocodec", "imap-audiocodec", "imap-audiocodec"),
	CLK_APB_INIT("gpio", "imap-gpio", "imap-gpio"),
	CLK_APB_INIT("dma", "dma-pl330", "dma"),
	CLK_APB_INIT("cpu-core", "imap-cpu", "imap-cpu"),
	CLK_APB_INIT("rtc", "imap-rtc", "imap-rtc"),
	CLK_APB_INIT("sysm", "imap-sysm", "imap-sysm"),
	CLK_APB_INIT("ps2-0", "imap-pic.0", "imap-pic"),
	CLK_APB_INIT("ps2-1", "imap-pic.1", "imap-pic"),
	CLK_APB_INIT("pwm0", "imap-pwm.0", "imap-pwm"),
	CLK_APB_INIT("pwm1", "imap-pwm.1", "imap-pwm"),
	CLK_APB_INIT("pwm2", "imap-pwm.2", "imap-pwm"),
	CLK_APB_INIT("pwm3", "imap-pwm.3", "imap-pwm"),
	CLK_APB_INIT("pwm4", "imap-pwm.4", "imap-pwm"),
	CLK_APB_INIT("i2c0", "imap-iic.0", "imap-iic"),
	CLK_APB_INIT("i2c1", "imap-iic.1", "imap-iic"),
	CLK_APB_INIT("i2c2", "imap-iic.2", "imap-iic"),
	CLK_APB_INIT("i2c3", "imap-iic.3", "imap-iic"),
	CLK_APB_INIT("spi0", "imap-ssp.0", "imap-ssp"),
	CLK_APB_INIT("spi1", "imap-ssp.1", "imap-ssp"),
	CLK_APB_INIT("spi2", "imap-ssp.2", "imap-ssp"),
	CLK_APB_INIT("watch-dog", "imap-wtd", NULL),
	CLK_APB_INIT("cmn-timer0", "imap-cmn-timer.0", "imap-cmn-timer"),
	CLK_APB_INIT("cmn-timer1", "imap-cmn-timer.1", "imap-cmn-timer"),
};

static struct imapx_clk_init vitual_clk[] = {
	/*CLK_VITUAL_INIT("dma", "dma-pl330", "dma"),*/
};

static struct imapx_clk_init cpu_clk[] = {
	CLK_CPU_INIT("gtm-clk", "gtm-clk", NULL),
	CLK_CPU_INIT("cpu-clk", "cpu-clk", NULL),
	CLK_CPU_INIT("smp_twd", "smp_twd", NULL),
};

/*
 * for register osc-clk
 */
static int imapx_osc_clk_init(void)
{
	struct clk *clk;

	clk = clk_register_fixed_rate(NULL, "osc-clk", NULL,
				CLK_IS_ROOT, IMAP_OSC_CLK);
	clks[clk_num++] = clk;
	return clk_register_clkdev(clk, "osc-clk", NULL);
}

/*
 * for register clk on bus and clk on apb
 */
static int imapx_bus_clk_init(void)
{
	struct clk *clk;
	int ret, i;

	for (i = 0; i < ARRAY_SIZE(bus_clk); i++) {
		struct bus_freq_range *range = &bus_range[bus_clk[i].type];
		clk = imapx_bus_clk_register(bus_clk[i].name, range->name,
				range, CLK_GET_RATE_NOCACHE
				| CLK_SET_RATE_PARENT);
		ret = clk_register_clkdev(clk, bus_clk[i].con_id,
							bus_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}

	for (i = 0; i < ARRAY_SIZE(apb_clk); i++) {
		clk = imapx_bus_clk_register(apb_clk[i].name, "apb_pclk",
				&apb_range, CLK_GET_RATE_NOCACHE
				| CLK_SET_RATE_PARENT);
		ret = clk_register_clkdev(clk, apb_clk[i].con_id,
							apb_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}

	return 0;
}

/*
 * for register clk depend on apll, only for gtm-clk, cpu-clk, smp-twd
 */
static int imapx_cpu_clk_init(void)
{
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(cpu_clk); i++) {
		clk = imapx_cpu_clk_register(cpu_clk[i].name, "apll", 0,
								NULL, 0x10306);
		ret = clk_register_clkdev(clk, cpu_clk[i].con_id,
							cpu_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}
	return 0;
}

/*
 * for register normal clk
 */
static int imapx_dev_clk_init(void)
{
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(dev_clk); i++) {
		int type = dev_clk[i].type;

		/* because type = 32, not existed*/
		if (type > SD3_CLK_SRC)
			type = type - ACODEC_CLK_SRC + SD3_CLK_SRC + 1;

		clk = imapx_dev_clk_register(dev_clk[i].name, dev_parent,
				5, dev_clk[i].type, CLK_GET_RATE_NOCACHE,
				&dev_clk_info[dev_clk[i].type], NULL);
		ret = clk_register_clkdev(clk, dev_clk[i].con_id,
							dev_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}

	return 0;
}

/*
 * for register clk: apll, dpll, epll, vpll
 */
static int imapx_pll_clk_init(void)
{
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(pll_clk); i++) {
		clk = imapx_pll_clk_register(pll_clk[i], "osc-clk", i,
				CLK_IGNORE_UNUSED, NULL);
		ret = clk_register_clkdev(clk, pll_clk[i], NULL);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}

	return 0;
}

/*
 * for register clk which need to clk_get in kernel standard
 * driver, but do not need to get in infotm soc, so we vitual
 * a clk
 */
static int imapx_vitual_clk_init(void)
{
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(vitual_clk); i++) {
		clk = imapx_vitual_clk_register(vitual_clk[i].name);
		ret = clk_register_clkdev(clk, vitual_clk[i].con_id,
							vitual_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks[clk_num++] = clk;
	}

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static struct clk_restore reg;

void set_pll(int pll, uint16_t value)
{
	int i;
	if (pll < 0 || pll > 3)
		return;

	/* prepare: disable load, close gate */
	writeb(1, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0xc);

	/* prepare: change parameters */
	writeb(value & 0xff, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18);
	writeb(value >> 8, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 4);

	/* disable pll */
	writeb(0, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0x10);

	/* out=pll, src=osc */
	writeb(2, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0x8);

	/* enable load */
	writeb(3, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0xc);

	/* enable pll */
	writeb(1, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0x10);

	/* wait pll stable:
	 * according to RTL simulation, PLLs need 200us before stable
	 * the poll register cycle is about 0.75us, so 266 cycles
	 * is needed.
	 * we put 6666 cycle here to perform a 5ms timeout.
	 */
	for (i = 0; (i < 6666) && !(readb(IO_ADDRESS(SYSMGR_CLKGEN_BASE)
					+ pll * 0x18 + 0x14) & 1); i++)
		;
	/* open gate */
	writel(2, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + pll * 0x18 + 0xc);
}

static int imapx_clk_suspend(void)
{
	int i;
	reg.pllval[DPLL] = __clk_readl(0x18) | (__clk_readl(0x1c) << 8);

	for (i = 0; i < DEV_NUM; i++) {
		uint32_t type = dev_clk_info[i].dev_id;

		reg.clkgate[i] = __clk_readl(BUS_CLK_GATE(type));
		reg.clknco[i] = __clk_readl(BUS_CLK_NCO(type));
		reg.clksrc[i] = __clk_readl(BUS_CLK_SRC(type));
		reg.clkdiv[i] = __clk_readl(BUS_CLK_DIVI_R(type));
	}
	return 0;
}

static void imapx_clk_resume(void)
{
	int i;

	/*
	 * Because in clk system, soft only know rate is identically.
	 */
	set_pll(DPLL, reg.pllval[DPLL]);
	for (i = 0; i < DEV_NUM; i++) {
		uint32_t type = dev_clk_info[i].dev_id;

		__clk_writel(reg.clkgate[i], BUS_CLK_GATE(type));
		__clk_writel(reg.clknco[i], BUS_CLK_NCO(type));

		if (type == DDRPHY_CLK_SRC || type == SYSBUS_CLK_SRC
			|| type == APB_CLK_SRC)
			continue;

		__clk_writel(reg.clksrc[i], BUS_CLK_SRC(type));
		__clk_writel(reg.clkdiv[i], BUS_CLK_DIVI_R(type));
	}
}

static struct syscore_ops clk_syscore_ops = {
	.suspend = imapx_clk_suspend,
	.resume = imapx_clk_resume,
};
#endif

static char pll_clk_id[] = {
	apll, dpll, epll, vpll, osc
};

struct clk_init_enable infotm_clk[] = {
	CLK_INIT_CONF(osc, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(apll, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(dpll, 1536000000, ENABLE),
	CLK_INIT_CONF(epll, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(vpll, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(cpu, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(sysbus, DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF(ddr_phy, DEFAULT_RATE, ENABLE),
};

static void imapx_init_infotm_conf(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(infotm_clk); i++) {
		int type = infotm_clk[i].type;
		struct clk *clk = clks[type];

		if (IS_ERR_OR_NULL(clk))
			return;

		if (infotm_clk[i].rate)
			if (clk_set_rate(clk, infotm_clk[i].rate)) {
				clk_err("%s: Failed to set rate %u of %s\n",
						__func__, infotm_clk[i].rate,
						__clk_get_name(clk));
				WARN_ON(1);
			}

		if (infotm_clk[i].state == ENABLE)
			if (clk_prepare_enable(clk)) {
				clk_err("%s: Failed to enable %s\n", __func__,
						__clk_get_name(clk));
				WARN_ON(1);
			}
	}
}

static int imapx_get_clks_id(int id)
{
	int src, src1;

	if (id < bus1 || id > apb_pclk)
		return -1;

	src = dev_clk[id - bus1].type;
	if (src > SD3_CLK_SRC)
		src = src - ACODEC_CLK_SRC + SD3_CLK_SRC + 1;
	src1 = dev_clk_info[src].clk_src;

	return pll_clk_id[src1];
}

static void imapx_set_default_parent(void)
{
	int i;

	/*
	 * for set the parent of clk.
	 *
	 */
	for (i = bus1; i < sysbus; i++) {
		int parent_id = imapx_get_clks_id(i);
		struct clk *parent = clks[parent_id];

		if (i == ddr_phy)
			continue;

		if (clk_set_parent(clks[i], parent)) {
			clk_err("%s: Failed to set parent %s of %s\n",
					__func__, __clk_get_name(parent),
					__clk_get_name(clks[i]));
			WARN_ON(1);
		}
	}

	/* for box gpu */
	if (item_exist("bus3.src")) {
		if (item_equal("bus3.src", "apll", 0))
			clk_set_parent(clks[i], clks[apll]);

		if (item_equal("bus3.src", "dpll", 0))
			clk_set_parent(clks[i], clks[dpll]);
	}
}

void imapx_clock_init(void)
{
	/* register the clk num */
	clk_num = 0;
	/* register different clk depend on clk feature */
	imapx_osc_clk_init();
	imapx_pll_clk_init();
	imapx_cpu_clk_init();
	imapx_dev_clk_init();
	imapx_bus_clk_init();
	imapx_vitual_clk_init();

	/*
	 * default state of clk
	 * In new clk architecture, if clk do not enable,
	 * it may auto disable. So we enable some in advanced.
	 */
	imapx_init_infotm_conf();

	/* set right parent about clk */
	imapx_set_default_parent();

#ifdef CONFIG_PM_SLEEP
	register_syscore_ops(&clk_syscore_ops);
#endif
}
