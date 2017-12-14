#include <linux/clkdev.h>
#include <linux/syscore_ops.h>
#include <mach/imap-iomap.h>
#include <mach/clk.h>
#include <linux/clk-provider.h>
#include <mach/items.h>
#include <linux/list.h>

#include "clk-debugfs.c"

#define MAX_NAME_LEN 32

struct list_head register_clk_head;

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
	BUS_FREQ_RANGE("bus4", 300000, 30000),
	BUS_FREQ_RANGE("bus5", 300000, 100000),
	BUS_FREQ_RANGE("bus6", 300000, 100000),
	BUS_FREQ_RANGE("bus7", 300000, 30000),
};

static struct bus_freq_range apb_range =
				BUS_FREQ_RANGE("apb_output",  300000, 100000);

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
	/* default bus3 setting by shaft */
	DEV_CLK_INFO(BUS3, 0, EPLL, 0, 3, DISABLE),
	/* bus4 bus5 do not use in imapx9*/
	DEV_CLK_INFO(BUS4, 0, EPLL, 0, 1, DISABLE),
	DEV_CLK_INFO(BUS6, 0, DEFAULT_SRC, 0, 3, ENABLE),

//	DEV_CLK_INFO(DSP_CLK_SRC, 0, DPLL, 0, 2, ENABLE),

	/*Note: why set IDS0 to this value? */
	DEV_CLK_INFO(IDS0_EITF_CLK_SRC, 0, EPLL, 0, 2, DISABLE),
	DEV_CLK_INFO(IDS0_OSD_CLK_SRC, 0, EPLL, 0, 1, DISABLE),

	DEV_CLK_INFO(MIPI_DPHY_CON_CLK_SRC, 1, EPLL, 6, 0, ENABLE),
	DEV_CLK_INFO(SD2_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	DEV_CLK_INFO(SD1_CLK_SRC, 0, EPLL, 0, 14, DISABLE),
	DEV_CLK_INFO(SD0_CLK_SRC, 0, EPLL, 0, 14, DISABLE),
	DEV_CLK_INFO(UART_CLK_SRC, 0, EPLL, 0, 5, ENABLE),
	DEV_CLK_INFO(SSP_CLK_SRC, 0, EPLL, 0, 3, ENABLE),

	DEV_CLK_INFO(AUDIO_CLK_SRC, 0, DPLL, 0, 24, DISABLE),
	DEV_CLK_INFO(USBREF_CLK_SRC, 1, DPLL, 4, 31, ENABLE),

	DEV_CLK_INFO(CAMO_CLK_SRC, 1, DPLL, 4, 19, ENABLE),
	DEV_CLK_INFO(CLKOUT1_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	DEV_CLK_INFO(MIPI_DPHY_PIXEL_CLK_SRC, 0, VPLL, 0, 21, DISABLE), /*  DPLL 5fps(127) to VPLL(21) 15fps*/
	DEV_CLK_INFO(DDRPHY_CLK_SRC, 0, DEFAULT_SRC, 0, 1, ENABLE),
	DEV_CLK_INFO(TSC_CLK_SRC, 0, OSC_CLK, 0, 0, ENABLE),
	DEV_CLK_INFO(APB_CLK_SRC, 0, DEFAULT_SRC, 0, 9, ENABLE),
};

static struct imapx_clk_init dev_clk[] = {
	CLK_INIT("bus1", NULL, "bus1", BUS1),
	CLK_INIT("bus3", NULL, "bus3", BUS3),
	CLK_INIT("bus4", NULL, NULL, BUS4),
	CLK_INIT("bus6", NULL, NULL, BUS6),
//	CLK_INIT("dsp", "imap-dsp", "q3f-dsp",  DSP_CLK_SRC),

	CLK_INIT("ids0-eitf", "imap-ids0-eitf", "imap-ids0", IDS0_EITF_CLK_SRC),
	CLK_INIT("ids0-osd", "imap-ids0-osd", "imap-ids0", IDS0_OSD_CLK_SRC),

	CLK_INIT("mipi-dphy-con", "imap-mipi-dphy-con",
			"imap-mipi-dsi", MIPI_DPHY_CON_CLK_SRC),

//	CLK_INIT("mipi-dphy-ref", "imap-mipi-dphy-ref",
//			"imap-mipi-dsi", MIPI_DPHY_REF_CLK_SRC),

	CLK_INIT("sd-mmc2", "imap-mmc.2", "sd-mmc2", SD2_CLK_SRC),
	CLK_INIT("sd-mmc1", "imap-mmc.1", "sd-mmc1", SD1_CLK_SRC),
	CLK_INIT("sd-mmc0", "imap-mmc.0", "sd-mmc0", SD0_CLK_SRC),
	CLK_INIT("uart", "imap-uart", NULL, UART_CLK_SRC),
	CLK_INIT("ssp-ext",   "imap-ssp", "ext-spi", SSP_CLK_SRC),
	CLK_INIT("audio-clk", "imap-audio-clk",
			"imap-audio-clk", AUDIO_CLK_SRC),
	CLK_INIT("usb-ref", "imap-usb-ref", "imap-usb", USBREF_CLK_SRC),
	CLK_INIT("camo", "imap-camo", "imap-camo", CAMO_CLK_SRC),
	CLK_INIT("clk-out1", "imap-clk1", "imap-clk1", CLKOUT1_CLK_SRC),
	CLK_INIT("mipi-dphy-pix", "imap-mipi-dphy-pix",
			"imap-mipi-dsi", MIPI_DPHY_PIXEL_CLK_SRC),
	CLK_INIT("ddr-phy", "imap-ddr-phy", "imap-ddr-phy", DDRPHY_CLK_SRC),
	CLK_INIT("touch screen", "imap-tsc", NULL, TSC_CLK_SRC),
	CLK_INIT("apb_output", "apb_output", "apb_output", APB_CLK_SRC),
};

static struct imapx_clk_init bus_clk[] = {
	CLK_INIT("isp", "imap-isp", "imap-isp", BUS1),
	CLK_INIT("camif", "imap-camif", "imap-camif", BUS1),
	CLK_INIT("mipi-csi", "imap-mipi-csi", "imap-mipi-csi", BUS1),
	CLK_INIT("ids0", "imap-ids.0", "imap-ids0", BUS1),
	CLK_INIT("ispost", "imap-ispost", "imap-ispost", BUS3),
	CLK_INIT("venc265-ctrl", "imap-venc265", "imap-venc265", BUS4),
	CLK_INIT("jenc", "imap-jenc", "imap-jenc", BUS4),
	CLK_INIT("crypto-dma", "imap-crypto-dma", "imap-crypto", BUS6),
	CLK_INIT("usb-ohci", "imap-usb-ohci", "imap-usb", BUS6),
	CLK_INIT("usb-ehci", "imap-usb-ehci", "imap-usb", BUS6),
	CLK_INIT("usb-otg", "imap-otg", NULL, BUS6),
	CLK_INIT("sdmmc0", "sdmmc.0", "sdmmc0", BUS6),
	CLK_INIT("sdmmc1", "sdmmc.1", "sdmmc1", BUS6),
	CLK_INIT("sdmmc2", "sdmmc.2", "sdmmc2", BUS6),
	CLK_INIT("dma", "dma-pl330", "dma-pl330", BUS6),	
	CLK_INIT("eth", "imap-mac", NULL, BUS6),
	CLK_INIT("spi-bus", "spibus.0", "spibus0" , BUS6),
};

static struct imapx_clk_init apb_clk[] = {
	CLK_APB_INIT("iis", "imap-iis", "imap-iis"),
	CLK_APB_INIT("cmn-timer0", "imap-cmn-timer.0", "imap-cmn-timer"),
	CLK_APB_INIT("cmn-timer1", "imap-cmn-timer.1", "imap-cmn-timer"),
	CLK_APB_INIT("pwm",  "imap-pwm", "imap-pwm"),
	CLK_APB_INIT("i2c0", "imap-iic.0", "imap-iic"),
	CLK_APB_INIT("i2c1", "imap-iic.1", "imap-iic"),
	CLK_APB_INIT("i2c2", "imap-iic.2", "imap-iic"),
	CLK_APB_INIT("i2c3", "imap-iic.3", "imap-iic"),
	CLK_APB_INIT("i2c4", "imap-iic.4", "imap-iic"),
	CLK_APB_INIT("pcm0", "imap-pcm.0", "imap-pcm"),
	CLK_APB_INIT("spi0", "imap-ssp.0", "imap-ssp"),
	CLK_APB_INIT("spi1", "imap-ssp.1", "imap-ssp"),
	CLK_APB_INIT("uart-core", "imap-uart-core", NULL),
	CLK_APB_INIT("audiocodec", "imap-audiocodec", "imap-audiocodec"),
	CLK_APB_INIT("tsc-ctrl", "imap-tsc-ctrl", "imap-tsc"),
	CLK_APB_INIT("gpio", "imap-gpio", "imap-gpio"),
	CLK_APB_INIT("watch-dog", "imap-wtd", NULL),
};

static struct imapx_clk_init vitual_clk[] = {
	/*CLK_VITUAL_INIT("dma", "dma-pl330", "dma"),*/
};

static struct imapx_clk_init cpu_clk[] = {
	CLK_CPU_INIT("gtm-clk", "gtm-clk", NULL),
	CLK_CPU_INIT("cpu-clk", "cpu-clk", NULL),
	CLK_CPU_INIT("apb_pclk", NULL, "apb_pclk"),
};

/*
 * for register osc-clk
 */
static int imapx_osc_clk_init(void)
{
	struct clk_collect *clks;
	struct clk *clk;
	int ret;

	clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);

	clk = clk_register_fixed_rate(NULL, "osc-clk", NULL,
				CLK_IS_ROOT, IMAP_OSC_CLK);
	ret = clk_register_clkdev(clk, "osc-clk", NULL);
	if(ret < 0)
		return -EINVAL;

	clks->clk = clk;
	list_add(&clks->list,&register_clk_head);

	return 0;
}

/*
 * for register clk on bus and clk on apb
 */
static int imapx_bus_clk_init(void)
{
	struct clk_collect *clks;
	struct clk *clk;
	int ret, i;
	struct bus_freq_range *range;

	for (i = 0; i < ARRAY_SIZE(bus_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		range = &bus_range[bus_clk[i].type];
		clk = imapx_bus_clk_register(bus_clk[i].name, range->name,
				range, CLK_GET_RATE_NOCACHE
				| CLK_SET_RATE_PARENT);
		ret = clk_register_clkdev(clk, bus_clk[i].con_id,
							bus_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;

		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
	}

	for (i = 0; i < ARRAY_SIZE(apb_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		clk = imapx_bus_clk_register(apb_clk[i].name, "apb_output",
				&apb_range, CLK_GET_RATE_NOCACHE
				| CLK_SET_RATE_PARENT);
		ret = clk_register_clkdev(clk, apb_clk[i].con_id,
							apb_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;

		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
	}

	return 0;
}

/*
 * for register clk depend on apll, only for gtm-clk, cpu-clk, smp-twd
 */
static int imapx_cpu_clk_init(void)
{
	struct clk_collect *clks;
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(cpu_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		clk = imapx_cpu_clk_register(cpu_clk[i].name, "apll", 0,
								NULL, 0x10306);
		ret = clk_register_clkdev(clk, cpu_clk[i].con_id,
							cpu_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;
		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
	}
	return 0;
}

/*
 * for register normal clk
 */
static int imapx_dev_clk_init(void)
{
	struct clk_collect *clks;
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(dev_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		clk = imapx_dev_clk_register(dev_clk[i].name, dev_parent,
				5, dev_clk[i].type, CLK_GET_RATE_NOCACHE,
				&dev_clk_info[i], NULL);
		ret = clk_register_clkdev(clk, dev_clk[i].con_id,
							dev_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;

		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
	}

	return 0;
}

/*
 * for register clk: apll, dpll, epll, vpll
 */
static int imapx_pll_clk_init(void)
{
	struct clk_collect *clks;
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(pll_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		clk = imapx_pll_clk_register(pll_clk[i], "osc-clk", i,
				CLK_IGNORE_UNUSED, NULL);
		ret = clk_register_clkdev(clk, pll_clk[i], NULL);
		if (ret < 0)
			return -EINVAL;

		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
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
	struct clk_collect *clks;
	struct clk *clk;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(vitual_clk); i++) {
		clks = (struct clk_collect *)kmalloc(sizeof(struct clk_collect), GFP_KERNEL);
		clk = imapx_vitual_clk_register(vitual_clk[i].name);
		ret = clk_register_clkdev(clk, vitual_clk[i].con_id,
							vitual_clk[i].dev_id);
		if (ret < 0)
			return -EINVAL;

		clks->clk = clk;
		list_add(&clks->list,&register_clk_head);
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

	for (i = 0; i < ARRAY_SIZE(dev_clk_info); i++) {
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
	set_pll(1, reg.pllval[DPLL]);
	for (i = 0; i < ARRAY_SIZE(dev_clk_info); i++) {
		uint32_t type = dev_clk_info[i].dev_id;

		__clk_writel(reg.clkgate[i], BUS_CLK_GATE(type));
		__clk_writel(reg.clknco[i], BUS_CLK_NCO(type));

		if (type >= DDRPHY_CLK_SRC)
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

extern struct clk *get_clk_by_name(char *name){
	struct clk_collect *entry;
	struct list_head *p;
	list_for_each(p, &register_clk_head){
		 entry = list_entry(p, struct clk_collect, list);
		 if(!strcmp(entry->clk->name, name))
			 return entry->clk;
	}
	return NULL;
}

struct clk_init_enable infotm_clk[] = {
	CLK_INIT_CONF("osc-clk", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("apll", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("dpll", 1536000000, ENABLE),
	CLK_INIT_CONF("epll", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("vpll", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("cpu-clk", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("ddr-phy", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("apb_output", DEFAULT_RATE, ENABLE),
	CLK_INIT_CONF("bus6", DEFAULT_RATE, ENABLE),
};

static void imapx_init_infotm_conf(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(infotm_clk); i++) {
		struct clk *clk = get_clk_by_name(infotm_clk[i].name);

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

static void imapx_set_default_parent(void)
{
	int i;
	struct imapx_dev_clk *dev;
	struct clk *clk, *parent;

	/*
	 * for set the parent of clk.
	 *
	 */
	for (i = 0; i < ARRAY_SIZE(dev_clk); i++) {
		clk = get_clk_by_name(dev_clk[i].name);
		dev = to_clk_dev(clk->hw);
		if(dev->info->clk_src == OSC_CLK)
			parent = get_clk_by_name("osc-clk");
		else if(dev->info->clk_src == APLL)
			parent = get_clk_by_name("apll");
		else if(dev->info->clk_src == DPLL)
			parent = get_clk_by_name("dpll");
		else if(dev->info->clk_src == EPLL)
			parent = get_clk_by_name("epll");
		else if(dev->info->clk_src == VPLL)
			parent = get_clk_by_name("vpll");
		else if(dev->info->clk_src == DEFAULT_SRC)
			continue;
		else{
			clk_err("%s clk's default parent is err!\n",clk->name);
			continue;
		}

		if (!strcmp(dev_clk[i].name, "ddr-phy") || 
				!strcmp(dev_clk[i].name, "ddr-phy") || 
				!strcmp(dev_clk[i].name, "bus6"))
			continue;

		if (clk_set_parent(clk, parent)) {
			clk_err("%s: Failed to set parent %s of %s\n",
					__func__, __clk_get_name(parent),
					__clk_get_name(clk));
			WARN_ON(1);
		}
	}

	/* for box gpu */
	if (item_exist("bus3.src")) {
		if (item_equal("bus3.src", "apll", 0)){
			clk = get_clk_by_name("bus3");
			parent = get_clk_by_name("apll");
			clk_set_parent(clk, parent);
		}

		if (item_equal("bus3.src", "dpll", 0)){
			clk = get_clk_by_name("bus3");
			parent = get_clk_by_name("dpll");
			clk_set_parent(clk, parent);
		}
	}
}

void imapx_clock_init(void)
{
	INIT_LIST_HEAD(&register_clk_head);

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
