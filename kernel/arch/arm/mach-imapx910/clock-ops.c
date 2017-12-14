#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/serial_core.h>
#include <linux/io.h>

#include <asm/mach/map.h>
#include <mach/hardware.h>
#include <mach/imap-clock.h>
#include <mach/clock-ops.h>
#include <mach/clock.h>
#include <mach/items.h>
#include <mach/rballoc.h>
#include "clock_cpu.h"
#include "core.h"
#define IMAP_CLK_ADDR_OFF  (IO_ADDRESS(IMAP_PA_SYSMGR_CLK))

#define __clk_readl(x)          __raw_readl((IMAP_CLK_ADDR_OFF + (x)))
#define __clk_writel(val, x)    __raw_writel(val, (IMAP_CLK_ADDR_OFF + (x)))
static void imapx800_apll_clk_init(struct clk *clk);

/* PLL */
static inline uint32_t imapx800_pll_clk_lock_check(uint32_t pll_type)
{
	return __clk_readl(PLL_LOCKED_STATUS(pll_type));
}

static inline void imapx800_pll_gate_ctrl(uint32_t pll_type, uint32_t mode)
{
	uint32_t val;
	 val = __clk_readl(PLL_LOAD_AND_GATE(pll_type));
	val &= ~(1 << PLL_OUT_GATE);
	val |= mode << PLL_OUT_GATE;
	__clk_writel(val, PLL_LOAD_AND_GATE(pll_type));
}

static inline void pll_parameter_set(uint32_t pll_type, uint32_t low_byte,
					   uint32_t high_byte)
{
	__clk_writel(low_byte, PLL_PARAMETER_LOW(pll_type));
	__clk_writel(high_byte, PLL_PARAMETER_HIGH(pll_type));
}

static inline void pll_load_para_control(uint32_t pll_type,
					       uint32_t mode)
{
	uint32_t val;
	 val = __clk_readl(PLL_LOAD_AND_GATE(pll_type));
	val &= ~(1 << PLL_LOAD_PA);
	val |= mode << PLL_LOAD_PA;
	__clk_writel(val, PLL_LOAD_AND_GATE(pll_type));
}

/*
 * set pll parameter
 */
static inline uint32_t pll_para_integr(uint32_t bypass, uint32_t od, uint32_t r,
				       uint32_t f)
{
	return ((bypass & 0x1) << PLL_BYPASS) | ((od & 0x3) << PLL_OD) |
		 ((r & 0x1f) << PLL_R) | ((f & 0x7f) << PLL_F);
}


/*
 * imapx800 pll clk init
 *
 * here we only set the parameter
 * TODO
 *
 * generally do not set pll output and src
 * before using this func should disable pll
 * after init need over 1us delay then enable pll
 */
static void imapx800_pll_clk_init(struct clk *clk)
{
	uint32_t para;
	uint32_t pll = clk->m_name;
	struct pll_clk_info *p = (struct pll_clk_info *)clk->info;

	    /* unload */
	    pll_load_para_control(pll, UNLOAD);

	    /* set para */
	    para = pll_para_integr(p->bypass, p->od, p->r, p->f);
	pll_parameter_set(pll, para & 0xff, para >> 8);

	    /* load */
	    pll_load_para_control(pll, LOAD);
	 clk->state = CLK_ON;
}

/*
 * imapx800 pll clk enable
 *
 * enable pll, then check lock, finally enable pll gate
 */
static int imapx800_pll_clk_enable(struct clk *clk)
{
	uint32_t pll = clk->m_name;
	 __clk_writel(ENABLE, PLL_ENABLE(pll));
	do {} while (!imapx800_pll_clk_lock_check(pll));
	imapx800_pll_gate_ctrl(pll, GATE_ENABLE);
	 return 0;
}


/*
 * imapx800 pll clk disable
 */
static void imapx800_pll_clk_disable(struct clk *clk)
{
	uint32_t pll = clk->m_name;
	 imapx800_pll_gate_ctrl(pll, GATE_DISABLE);
	__clk_writel(DISABLE, PLL_ENABLE(pll));
}

/*
 * imapx800 pll clk get rate
 *
 * rate is the parent rate
 *
 * Fout = Fref * NF / NO     NF = 2 * (f + 1), NO = 2 ^ od
 */
static unsigned long imapx800_pll_clk_get_rate(struct clk *clk)
{
	unsigned long rate;
	struct pll_clk_info *p = (struct pll_clk_info *)clk->info;
	 rate = clk->parent->rate;
	if (p->out_sel == REFERENCE_CLK)
		return rate;
	return (rate * 2 * (p->f + 1)) >> (p->od);
}


/*
 * impax800 pll clk set rate
 */
static int imapx800_cal_pll(struct clk *clk, unsigned long rate)
{
	struct pll_clk_info *p = (struct pll_clk_info *)clk->info;
	unsigned long parent_rate = clk_get_rate(clk->parent);
	unsigned long fvco;
	 if (!clk->parent)
		return -1;
	if (rate >= FOUT_CLK_MAX || rate <= FOUT_CLK_MIN)
		return -1;
	 p->bypass = 0;
	p->r = parent_rate / FREF_CLK_MIN - 1;
	p->od = -1;
	fvco = FVCO_CLK_MAX / rate;
	while (fvco) {
		fvco = fvco >> 1;
		p->od += 1;
	}
	p->f = rate * (1 << p->od) / (parent_rate * 2) - 1;
	return 0;
}

static int imapx800_pll_clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (!imapx800_cal_pll(clk, rate)) {
		imapx800_pll_clk_init(clk);
		return imapx800_pll_clk_get_rate(clk);
	} else
		return -1;
}

static struct clk_ops imapx800_pll_ops = { .init =
	    imapx800_apll_clk_init, .enable =
	    imapx800_pll_clk_enable, .disable =
	    imapx800_pll_clk_disable, .set_rate =
	    imapx800_pll_clk_set_rate, .get_rate = imapx800_pll_clk_get_rate,
};


/* apll */
/*
 * that apll changing frequency is a bit diff,
 * as cpu clk is based on apll,
 * so after unload parameter and set done parameter
 * we need to set rSOFT_EVENT1_REG 1
 * then after cpu reback to work, check the reg again ,
 * when its value is 0, means apll has locked
 **/

/*
 * imapx800 apll clk init
 *
 */
static void imapx800_apll_clk_init(struct clk *clk)
{
	uint32_t para;
	uint32_t pll = clk->m_name;
	struct pll_clk_info *p = (struct pll_clk_info *)clk->info;

	    /*
	     * normally, do not change apll clock frequency
	     * so, here only to get the value has already been set
	     */
	    para =
	    (__clk_readl(PLL_PARAMETER_HIGH(pll)) << 8) |
	    (__clk_readl(PLL_PARAMETER_LOW(pll)));
	p->bypass = (para >> PLL_BYPASS) & 0x1;
	p->od = (para >> PLL_OD) & 0x3;
	p->r = (para >> PLL_R) & 0x1f;
	p->f = (para >> PLL_F) & 0x7f;
	pr_err("Mode %d: Set pll %lu\n", pll,
		  imapx800_pll_clk_get_rate(clk));
	clk->state = CLK_ON;
}

static void __apll_reset(uint32_t value)
{
	int i;
	/* out=ref, src=osc */
	__clk_writel(0, APLL_SRC_STATUS);
	/* disable load, but preserve output */
	__clk_writel(0, APLL_LOAD_AND_GATE);
	/* disable pll */
	__clk_writel(0, APLL_ENABLE);
	/* prepare: change parameters */
	__clk_writel(value & 0xff, APLL_PARAMETER_LOW);
	__clk_writel(value >> 8, APLL_PARAMETER_HIGH);
	/* enable load */
	__clk_writel(2, APLL_LOAD_AND_GATE);
	/* enable pll */
	__clk_writel(1, APLL_ENABLE);
	for (i = 0; (i < 6666) && !(__clk_readl(APLL_LOCKED_STATUS) & 1);
		i++)
		;
	__clk_writel(2, APLL_SRC_STATUS);	/* out=pll, src=osc */
}


/*
 * imapx800 apll clk enable
 */
static int imapx800_apll_clk_enable(struct clk *clk)
{
	__clk_writel(ENABLE, SOFT_EVENT1_REG);
	do {} while (!__clk_readl(SOFT_EVENT1_REG));
	return 0;
}


/*
 * imapx800 apll clk disable
 */
static void imapx800_apll_clk_disable(struct clk *clk)
{
}

/*
 * imapx800 apll clk set rate
 */
static int imapx800_apll_clk_set_rate(struct clk *clk, unsigned long rate)
{
	uint32_t para;
	uint32_t pll = clk->m_name;
	struct pll_clk_info *p = (struct pll_clk_info *)clk->info;

	    /* TODO
	     *
	     * need to add code to check the rate
	     * and set the rate
		 **/
	if (!imapx800_cal_pll(clk, rate)) {
		/*  unload */
		pll_load_para_control(pll, UNLOAD);
		/*  set para */
		para = pll_para_integr(p->bypass, p->od, p->r, p->f);
		pll_parameter_set(pll, para & 0xff, para >> 8);
		__apll_reset(para);
		return imapx800_pll_clk_get_rate(clk);
	} else
		return -1;
}

static struct clk_ops imapx800_apll_ops = {
	.init = imapx800_apll_clk_init,
	.enable = imapx800_apll_clk_enable,
	.disable = imapx800_apll_clk_disable,
	.set_rate = imapx800_apll_clk_set_rate,
	.get_rate = imapx800_pll_clk_get_rate,
};


/* bus and dev */
static inline void bus_and_dev_clk_para_set(uint32_t index, uint32_t para)
{
	__clk_writel(para, BUS_CLK_SRC(index));
}

static inline void bus_and_dev_clk_divider_ratio_set(uint32_t index,
							   uint32_t r)
{
	__clk_writel((r & 0x1f), BUS_CLK_DIVI_R(index));
}

static inline void bus_and_dev_nco_set(uint32_t index, uint32_t val)
{
	__clk_writel(val, BUS_CLK_NCO(index));
}

/*
 * imapx800 bus and dev clk init
 *
 * this function set clk src and divider way(nco or divider)
 * should use bus and dev clk disable before
 * and enable after
 */
static void imapx800_bus_and_dev_clk_init(struct clk *clk)
{
	uint32_t val;
	uint32_t index = clk->m_name;
	struct bus_and_dev_clk_info *info =
		(struct bus_and_dev_clk_info *)clk->info;
	if (strcmp(clk->name, "ddr-phy") == 0)
		return;
	if (strcmp(clk->name, "touch screen") == 0)
		return;

	val = ((info->nco_en & 0x1) << BUS_NCO) | ((info->clk_src & 0x7) <<
		BUS_CLK_SRC_BIT);
	bus_and_dev_clk_para_set(index, val);
	bus_and_dev_clk_divider_ratio_set(index, 0);
	 if (info->nco_en)
		bus_and_dev_nco_set(index, info->nco_value);

	else
		bus_and_dev_clk_divider_ratio_set(index, info->clk_divider);
}


/*
 * imapx800 bus and dev clk enable
 */
int imapx800_bus_and_dev_clk_enable(struct clk *clk)
{
	if (strcmp(clk->name, "ddr-phy") == 0)
		return 0;
	__clk_writel(GATE_ENABLE, BUS_CLK_GATE(clk->m_name));
	return 0;
}
EXPORT_SYMBOL(imapx800_bus_and_dev_clk_enable);

/*
 * imapx800 bus and dev clk disable
 */
static void imapx800_bus_and_dev_clk_disable(struct clk *clk)
{
	if (strcmp(clk->name, "bus1") == 0)
		return;
	if (strcmp(clk->name, "bus2") == 0)
		return;
	if (strcmp(clk->name, "bus3") == 0)
		return;
	if (strcmp(clk->name, "bus4") == 0)
		return;
	if (strcmp(clk->name, "bus5") == 0)
		return;
	if (strcmp(clk->name, "bus6") == 0)
		return;
	if (strcmp(clk->name, "bus7") == 0)
		return;
	 __clk_writel(GATE_DISABLE, BUS_CLK_GATE(clk->m_name));
}


/*
 * imapx800 bus and dev clk set parent
 */
static int imapx800_bus_and_dev_clk_set_parent(struct clk *c, struct clk *p)
{
	uint32_t index = c->m_name;
	uint32_t val;

	    /* TODO
	     *
	     * need to add code
	     * check whether if p is correct
	     * set new parent and comput new rate
	     **/

	    /* disable clk */
	    imapx800_bus_and_dev_clk_disable(c);
	val = __clk_readl(BUS_CLK_SRC(index));
	val &= ~(0x7 << 0);
	val |= p->m_name;
	__clk_writel(val, BUS_CLK_SRC(index));

	    /* enable clk */
	    imapx800_bus_and_dev_clk_enable(c);
	return 0;
}


/*
 * imapx800 bus and dev clk set rate
 */
static int imapx800_bus_and_dev_clk_set_rate(struct clk *c,
					     unsigned long rate)
{
	struct bus_and_dev_clk_info *info =
	    (struct bus_and_dev_clk_info *)c->info;
	uint32_t index = c->m_name;
	int val;

	    /* TODO
	     *
	     * need to add code
	     * check whether if rate is possibility to be set
	     * and reset rate
	     **/
	    imapx800_bus_and_dev_clk_disable(c);
	val = __clk_readl(BUS_CLK_SRC(index));
	if (((val >> 3) & 0x1) != info->nco_en) {
		val &= ~(0x1 << 3);
		val |= info->nco_en << 3;
		bus_and_dev_clk_para_set(index, val);
	}
	if (info->nco_en)
		bus_and_dev_nco_set(index, info->nco_value);

	else
		bus_and_dev_clk_divider_ratio_set(index, info->clk_divider);
	imapx800_bus_and_dev_clk_enable(c);
	 return rate;
}


/*
 * imapx800 bus and dev clk get rate
 */
static unsigned long imapx800_bus_and_dev_clk_get_rate(struct clk *c)
{

	unsigned long rate;
	struct bus_and_dev_clk_info *info =
	    (struct bus_and_dev_clk_info *)c->info;
	 if (c->parent->rate)
		rate = c->parent->rate;

	else
		rate = c->parent->ops->get_rate(c->parent);
	 if (info->nco_en)
		return rate / 256 * info->nco_value;

	else
		return rate / (info->clk_divider + 1);
}

static unsigned long clk_sub(unsigned long a, unsigned long b)
{
	if (a > b)
		return a - b;

	else
		return b - a;
}


#define CLK_DIV(a, b)   ((a + b / 2) / b)

/*
 * imapx800 bus and dev clk round rate
 */
static long imapx800_bus_and_dev_clk_round_rate(struct clk *c,
						unsigned long rate)
{
	struct bus_and_dev_clk_info *info =
	    (struct bus_and_dev_clk_info *)c->info;
	unsigned long parent_rate = clk_get_rate(c->parent);

	    /* TODO
	     *
	     * need to add code
	     * comput new rate that around the rate given
	     * if ok , set rate
	     **/
	    if (info->nco_disable == ENABLE) {
		parent_rate = parent_rate / 1000;
		info->clk_divider = CLK_DIV(parent_rate, rate) - 1;
		if (info->clk_divider > 31)
			info->clk_divider = 31;
		info->nco_value = CLK_DIV(rate * 256, parent_rate);
		if (info->nco_value > 256)
			info->nco_value = 256;

		else if (info->nco_value < 1)
			info->nco_value = 1;
		if (clk_sub(parent_rate / (info->clk_divider + 1), rate) >
		     clk_sub(rate, ((parent_rate * info->nco_value) >> 8))) {
			info->nco_en = 1;
			return (parent_rate * info->nco_value) / 256;
		} else {
			info->nco_en = 0;
			return parent_rate / (info->clk_divider + 1);
		}
	} else {
		parent_rate = parent_rate / 1000;
		info->clk_divider = CLK_DIV(parent_rate, rate) - 1;
		if (info->clk_divider > 31)
			info->clk_divider = 31;
		info->nco_en = 0;
		return parent_rate / (info->clk_divider + 1);
	}
}

static struct clk_ops imapx800_bus_and_dev_ops = { .init =
	    imapx800_bus_and_dev_clk_init, .enable =
	    imapx800_bus_and_dev_clk_enable, .disable =
	    imapx800_bus_and_dev_clk_disable, .set_parent =
	    imapx800_bus_and_dev_clk_set_parent, .set_rate =
	    imapx800_bus_and_dev_clk_set_rate, .get_rate =
	    imapx800_bus_and_dev_clk_get_rate, .round_rate =
	    imapx800_bus_and_dev_clk_round_rate,
};


/* cpu */
static inline void cpu_clk_para_control(uint32_t mode)
{
	__clk_writel(mode, CPU_CLK_PARA_EN);
}
static inline void cpu_clk_sel(uint32_t type)
{
	__clk_writel(type, CPU_CLK_OUT_SEL);
}
static inline void cpu_divider_para_set(uint32_t en, uint32_t len)
{
	__clk_writel(((en & 0x1) << CPU_DIVIDER_EN) |
		      ((len & 0x3f) << CPU_DIVLEN), CPU_CLK_DIVIDER_PA);
}
static inline void cpu_clk_set(uint32_t len, uint32_t *cpu,
				     uint32_t *axi, uint32_t *apb,
				     uint32_t *axi_cpu, uint32_t *apb_cpu,
				     uint32_t *apb_axi)
{
	uint32_t i;
	 for (i = 0; i < len; i++) {
		__clk_writel(cpu[i], CPU_CLK_DIVIDER_SEQ(i));
		__clk_writel(axi[i], CPU_AXI_CLK_DIVIDER_SEQ(i));
		__clk_writel(apb[i], CPU_APB_CLK_DIVIDER_SEQ(i));
		__clk_writel(axi_cpu[i], CPU_AXI_A_CPU_CLK_EN_SEQ(i));
		__clk_writel(apb_cpu[i], CPU_APB_A_CPU_CLK_EN_SEQ(i));
		__clk_writel(apb_axi[i], CPU_APB_A_AXI_CLK_EN_SEQ(i));
	}
}


/*
 * imapx800 cpu clk init
 */
static void imapx800_cpu_clk_init(struct clk *clk)
{

	    /* TODO
	     *
	     * mostly cpu ratio is already defined from boot
	     * normaly may not need to set this
	     */
}

/*
 * imapx800 cpu clk enable
 */
static int imapx800_cpu_clk_enable(struct clk *clk)
{
	if (!strcmp(clk->name, "apb_pclk"))
		return 0;
	cpu_clk_para_control(GATE_ENABLE);
	return 0;
}


/*
 * imapx800 cpu clk disable
 */
static void imapx800_cpu_clk_disable(struct clk *clk)
{
	if (!strcmp(clk->name, "apb_pclk"))
		return;
	cpu_clk_para_control(GATE_DISABLE);
}


/*
	cpu frequency control
	this funciton can be used to change cpu clk fr
	first disable para load
	then set total seq len and clk sel,
	here if cpu coefficent is bigger than 1, we need to set CPU_DIVIDER_CLK
	finally enable para load
*/
static void cpu_fr_con(struct cpu_clk_info *info)
{
	uint32_t r_num;
	uint32_t clk_sel;
	struct cpu_clk_info *c = info;
	 r_num = (c->seq_len >> 3) + 1;
	if (c->cpu_co == 1)
		clk_sel = CPU_APLL_CLK;

	else
		clk_sel = CPU_DIVIDER_CLK;
	 cpu_clk_para_control(DISABLE);
	cpu_clk_sel(clk_sel);
	cpu_divider_para_set(ENABLE, c->seq_len);
	cpu_clk_set(r_num, c->cpu_clk, c->axi_clk, c->apb_clk, c->axi_cpu,
		     c->apb_cpu, c->apb_axi);
	cpu_clk_para_control(ENABLE);
}


/*
 * imapx800 cpu clk set rate
 * when you set cpu axi apb clk, rate is the ratio about cpu_clk(8bit) :
 * axi_clk(8bit) : apb_clk(8bit). for example, rate = 0x00010306, means
 * cpu_clk:axi_clk:apb_clk = 1:3:6
 */
static uint32_t cpu_axi_apb = 0x00010306;
static int imapx800_cpu_clk_set_rate(struct clk *clk, unsigned long rate)
{
	struct cpu_clk_info *c = (struct cpu_clk_info *)clk->info;
	uint64_t clk_data[6];
	int i, j, len;
	 if (!strcmp(clk->name, "apb_pclk"))
		return 0;

	    /* TODO
	     *
	     * need to add code
	     * check rate, may using table to fill in cpu clk info
	     * then
	     **/
	    for (i = 0; i < CPU_CLK_NUM; i++) {
		if (cpu_clk_data[i][0] == rate) {
			cpu_axi_apb = cpu_clk_data[i][0];
			c->cpu_co = (rate >> 16) & 0xff;
			c->seq_len = (rate & 0xff) - 1;
			len = (c->seq_len) >> 3;
			len += 1;
			for (j = 0; j < 6; j++) {
				clk_data[j] =
				    (uint64_t) cpu_clk_data[i][2 + j * 2];
				clk_data[j] = clk_data[j] << 32;
				clk_data[j] |=
				    (uint64_t) cpu_clk_data[i][1 + j * 2];
			}
			for (j = 0; j < len; j++) {
				c->cpu_clk[j] =
				    (uint32_t) ((clk_data[0] >> (j * 8)) &
						0xff);
				c->axi_clk[j] =
				    (uint32_t) ((clk_data[1] >> (j * 8)) &
						0xff);
				c->apb_clk[j] =
				    (uint32_t) ((clk_data[2] >> (j * 8)) &
						0xff);
				c->axi_cpu[j] =
				    (uint32_t) ((clk_data[3] >> (j * 8)) &
						0xff);
				c->apb_cpu[j] =
				    (uint32_t) ((clk_data[4] >> (j * 8)) &
						0xff);
				c->apb_axi[j] =
				    (uint32_t) ((clk_data[5] >> (j * 8)) &
						0xff);
			}
			break;
		}
	}
	if (i == CPU_CLK_NUM)
		return -1;
	cpu_fr_con(c);
	 return 0;
}


/*
 * imapx800 cpu clk get rate
 */
static unsigned long imapx800_cpu_clk_get_rate(struct clk *clk)
{
	unsigned long rate = clk_get_rate(clk->parent);
	 if (strcmp(clk->name, "cpu-clk") == 0)
		return rate / ((cpu_axi_apb >> 16) & 0xff);
	else if (strcmp(clk->name, "apb_pclk") == 0)
		return 118800000;
	else if (strcmp(clk->name, "gtm-clk") == 0)
		return rate / (((cpu_axi_apb >> 16) & 0xff) * 8);
	else if (strcmp(clk->name, "smp_twd") == 0)
		return rate / (((cpu_axi_apb >> 16) & 0xff) * 8);
	else
		return rate / (cpu_axi_apb & 0xff);
}

static long imapx800_cpu_clk_round_rate(struct clk *clk,
					   unsigned long rate)
{
	int i;
	 if (!strcmp(clk->name, "apb_pclk"))
		return 0;
	 for (i = 0; i < CPU_CLK_NUM; i++) {
		if (rate == cpu_clk_data[i][0])
			return rate;
	}
	if (i == CPU_CLK_NUM)
		return -1;
	return 0;
}


/*
 * take both cpu clk and apb clk
 */
static struct clk_ops imapx800_cpu_ops = { .init =
	    imapx800_cpu_clk_init, .enable =
	    imapx800_cpu_clk_enable, .disable =
	    imapx800_cpu_clk_disable, .set_rate =
	    imapx800_cpu_clk_set_rate, .get_rate =
	    imapx800_cpu_clk_get_rate, .round_rate =
	    imapx800_cpu_clk_round_rate,
};


/* apb */

/*
 * imapx800 apb clk enable
 */
static int imapx800_apb_clk_enable(struct clk *clk)
{
	return 0;
}


/*
 * imapx800 apb clk disable
 */
static void imapx800_apb_clk_disable(struct clk *clk)
{
}

static struct clk_ops imapx800_apb_ops = {
	.enable = imapx800_apb_clk_enable,
	.disable = imapx800_apb_clk_disable,
};


/* dev on bus */

/*
 * imapx800 dev on bus init
 */
static void imapx800_dev_on_bus_init(struct clk *clk)
{
}

/*
 * imapx800 dev on bus enable
 */
static int imapx800_dev_on_bus_enable(struct clk *clk)
{
	return 0;
}


/*
 * imapx800 dev on bus disable
 */
static void imapx800_dev_on_bus_disable(struct clk *clk)
{
}

/*
 * imapx800 dev on bus set rate
 */
static int imapx800_dev_on_bus_set_rate(struct clk *clk, unsigned long rate)
{

	    /* TODO
	     *
	     **/
	return clk_set_rate(clk->parent, rate);
}


/*
 * imapx800 dev on bus round rate
 */
static long imapx800_dev_on_bus_round_rate(struct clk *clk,
					   unsigned long rate)
{
	struct bus_freq_range *c = (struct bus_freq_range *)clk->info;

	    /* TODO
	     *
	     **/
	if (rate > c->max)
		rate = c->max;
	else if (rate < c->min)
		rate = c->min;
	return rate;
}

static struct clk_ops imapx800_dev_on_bus_ops = { .init =
	    imapx800_dev_on_bus_init, .enable =
	    imapx800_dev_on_bus_enable, .disable =
	    imapx800_dev_on_bus_disable, .set_rate =
	    imapx800_dev_on_bus_set_rate, .round_rate =
	    imapx800_dev_on_bus_round_rate,
};


/* clock definitions */

/* apll, dpll, epll, vpll */
#define PLL_CLK_INFO_SET(_bypass, _od, _r, _f, _out_sel, _src) \
{\
	.bypass = _bypass, \
	.od = _od, \
	.r = _r, \
	.f = _f, \
	.out_sel =  _out_sel, \
	.src = _src, \
}

static struct pll_clk_info imapx800_pll_clk_info[4];

/* cpu clk, apb clk */
static struct cpu_clk_info imapx800_cpu_clk_info;
static struct cpu_clk_info imapx800_apb_clk_info;
static struct cpu_clk_info imapx800_gtm_clk_info;
static struct cpu_clk_info imapx800_smp_twd_info;

/* bus and dev clk */
#define BUS_AND_DEV_CLK_INFO_SET(_dev_id, _nco_en, _clk_src,\
		_nco_value, _clk_divider, disable) \
{\
	.dev_id = _dev_id, \
	.nco_en = _nco_en, \
	.clk_src = _clk_src, \
	.nco_value = _nco_value, \
	.clk_divider = _clk_divider, \
	.nco_disable = disable, \
}
/*
 *apll 804M
 *dpll 480M
 *epll 888M
 */
static struct bus_and_dev_clk_info imapx800_bus_and_dev_clk_info[34] = {
	BUS_AND_DEV_CLK_INFO_SET(BUS1, 0, EPLL, 0, 3, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(BUS2, 0, EPLL, 0, 3, ENABLE),
	/* default bus3 setting by shaft */
	BUS_AND_DEV_CLK_INFO_SET(BUS3, 0, EPLL, 0, 0, ENABLE),
	/* dpll cannot give us clk>240M by ayakashi */
	BUS_AND_DEV_CLK_INFO_SET(BUS4, 0, EPLL, 0, 4, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(BUS5, 0, DPLL, 0, 9, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(BUS6, 0, EPLL, 0, 6, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(BUS7, 0, DPLL, 0, 5, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS0_EITF_CLK_SRC, 0, EPLL, 0, 22, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS0_OSD_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS0_TVIF_CLK_SRC, 0, EPLL, 0, 7, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS1_EITF_CLK_SRC, 0, EPLL, 0, 7,
				  DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS1_OSD_CLK_SRC, 0, EPLL, 0, 4, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(IDS1_TVIF_CLK_SRC, 0, EPLL, 0, 7,
				  DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(MIPI_DPHY_CON_CLK_SRC, 1, EPLL, 6, 0,
				  ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(MIPI_DPHY_REF_CLK_SRC, 1, EPLL, 6, 0,
				  ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(MIPI_DPHY_PIXEL_CLK_SRC, 0, EPLL, 0, 0,
				  DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(ISP_OSD_CLK_SRC, 0, DPLL, 0, 5, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(SD0_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(SD1_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(SD2_CLK_SRC, 0, DPLL, 0, 15, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(NFECC_CLK_SRC, 0, EPLL, 0, 3, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(HDMI_CLK_SRC, 1, DPLL, 4, 31, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(UART_CLK_SRC, 0, EPLL, 0, 3, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(SPDIF_CLK_SRC, 0, DPLL, 0, 24, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(AUDIO_CLK_SRC, 0, DPLL, 0, 24, DISABLE),
	BUS_AND_DEV_CLK_INFO_SET(USBREF_CLK_SRC, 1, DPLL, 4, 31, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(CLKOUT0_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(CLKOUT1_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(SATAPHY_CLK_SRC, 0, DPLL, 0, 0, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(CAMO_CLK_SRC, 1, DPLL, 4, 19, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(DDRPHY_CLK_SRC, 0, VPLL, 0, 1, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(CRYPTO_CLK_SRC, 0, DPLL, 0, 9, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(TSC_CLK_SRC, 0, OSC_CLK, 0, 0, ENABLE),
	BUS_AND_DEV_CLK_INFO_SET(DMIC_CLK_SRC, 0, DPLL, 0, 12, ENABLE),
};

static struct clk imapx800_osc_clk = { .name = "osc-clk", .parent =
	    NULL, .ops = NULL, .rate = 12000000, .state =
	    CLK_UNINITIAL, .m_name = OSC_CLK, .info = NULL,
};

static struct clk imapx800_apll_clk = { .name = "apll", .parent =
	    &imapx800_osc_clk, .ops = &imapx800_apll_ops, .state =
	    CLK_UNINITIAL, .m_name = APLL, .info =
	    (void *)&imapx800_pll_clk_info[0],
};
static struct clk imapx800_dpll_clk = { .name = "dpll", .parent =
	    &imapx800_osc_clk, .ops = &imapx800_pll_ops, .state =
	    CLK_UNINITIAL, .m_name = DPLL, .info =
	    (void *)&imapx800_pll_clk_info[1],
};

static struct clk imapx800_epll_clk = { .name = "epll", .parent =
	    &imapx800_osc_clk, .ops = &imapx800_pll_ops, .state =
	    CLK_UNINITIAL, .m_name = EPLL, .info =
	    (void *)&imapx800_pll_clk_info[2],
};

static struct clk imapx800_vpll_clk = { .name = "vpll", .parent =
	    &imapx800_osc_clk, .ops = &imapx800_pll_ops, .state =
	    CLK_UNINITIAL, .m_name = VPLL, .info =
	    (void *)&imapx800_pll_clk_info[3],
};


#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
static struct clk imapx800_cpu_clk = { .name = "cpu-clk", .parent =
	    NULL, .ops = &imapx800_cpu_ops, .rate = FPGA_CPU_CLK, .state =
	    CLK_UNINITIAL, .info = &imapx800_cpu_clk_info,
};

static struct clk imapx800_apb_clk = { .name = "apb_pclk", .parent =
	    NULL, .ops = &imapx800_cpu_ops, .rate = FPGA_APB_CLK, .state =
	    CLK_UNINITIAL, .info = &imapx800_apb_clk_info,
};


#else /*  */
static struct clk imapx800_cpu_clk = { .name = "cpu-clk", .parent =
	    &imapx800_apll_clk, .ops = &imapx800_cpu_ops, .state =
	    CLK_UNINITIAL, .info = &imapx800_cpu_clk_info,
};

static struct clk imapx800_gtm_clk = { .name = "gtm-clk", .parent =
	    &imapx800_apll_clk, .ops = &imapx800_cpu_ops, .state =
	    CLK_UNINITIAL, .info = &imapx800_gtm_clk_info,
};

static struct clk imapx800_smp_twd = { .name = "smp_twd", .parent =
	    &imapx800_apll_clk, .ops = &imapx800_cpu_ops, .state =
	    CLK_UNINITIAL, .info = &imapx800_smp_twd_info, .lookup = {
		.dev_id = "smp_twd", .con_id = NULL},
};

static struct clk imapx800_apb_clk = { .name = "apb_pclk", .parent =
	    &imapx800_apll_clk, .ops = &imapx800_cpu_ops, .state =
	    CLK_UNINITIAL, .info = &imapx800_apb_clk_info,
};


#endif /*  */
static struct clk *imapx800_sys_clk[] = {
	&imapx800_osc_clk,
	&imapx800_apll_clk,
	&imapx800_dpll_clk,
	&imapx800_epll_clk,
	&imapx800_vpll_clk,
	&imapx800_cpu_clk,
	&imapx800_apb_clk,
#if !defined(CONFIG_IMAPX910_FPGA_PLATFORM)
	&imapx800_gtm_clk,
	&imapx800_smp_twd,
#endif /*  */
};


#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
#define BUS_CLK_INFO_SET(_name, _m_name, _info, _parent) \
{\
	.name = _name, \
	.parent = NULL, \
	.ops = &imapx800_bus_and_dev_ops, \
	.state = CLK_UNINITIAL, \
	.rate = FPGA_BUS_CLK, \
	.m_name = _m_name, \
	.info = _info, \
}
#else /*  */
#define BUS_CLK_INFO_SET(_name, _m_name, _info, _parent) \
{\
	.name = _name, \
	.parent = _parent, \
	.ops = &imapx800_bus_and_dev_ops, \
	.state = CLK_UNINITIAL, \
	.m_name = _m_name, \
	.info = _info, \
}
#endif /*  */
static struct clk imapx800_bus_clk[7] = {
	BUS_CLK_INFO_SET("bus1", BUS1, &imapx800_bus_and_dev_clk_info[BUS1],
			&imapx800_epll_clk),
	BUS_CLK_INFO_SET("bus2", BUS2, &imapx800_bus_and_dev_clk_info[BUS2],
			&imapx800_epll_clk),
	BUS_CLK_INFO_SET("bus3", BUS3, &imapx800_bus_and_dev_clk_info[BUS3],
			  &imapx800_epll_clk),
	BUS_CLK_INFO_SET("bus4", BUS4, &imapx800_bus_and_dev_clk_info[BUS4],
			  &imapx800_epll_clk),
	BUS_CLK_INFO_SET("bus5", BUS5, &imapx800_bus_and_dev_clk_info[BUS5],
			  &imapx800_dpll_clk),
	BUS_CLK_INFO_SET("bus6", BUS6, &imapx800_bus_and_dev_clk_info[BUS6],
			  &imapx800_epll_clk),
	BUS_CLK_INFO_SET("bus7", BUS7, &imapx800_bus_and_dev_clk_info[BUS7],
			  &imapx800_dpll_clk),
};


#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
#define DEV_CLK_INFO_SET(_name, _dev, _con, _m_name, _info, _parent) \
{\
	.name = _name, \
	.lookup = {\
		.dev_id = _dev, \
		.con_id = _con, \
		}, \
	.parent = NULL, \
	.ops = &imapx800_bus_and_dev_ops, \
	.rate = FPGA_EXTEND_CLK, \
	.state = CLK_UNINITIAL, \
	.m_name = _m_name, \
	.info = _info, \
}
#else /*  */
#define DEV_CLK_INFO_SET(_name, _dev, _con,  _m_name, _info, _parent) \
{\
	.name = _name, \
	.lookup = {\
		.dev_id = _dev, \
		.con_id = _con, \
	}, \
	.parent = _parent, \
	.ops = &imapx800_bus_and_dev_ops, \
	.state = CLK_UNINITIAL, \
	.m_name = _m_name, \
	.info = _info, \
}
#endif /*  */
static struct clk imapx800_dev_clk[] = {
	DEV_CLK_INFO_SET("ids0-eitf", "imap-ids0-eitf", "imap-ids0",
		IDS0_EITF_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[IDS0_EITF_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("ids0-ods", "imap-ids0-ods", "imap-ids0",
		IDS0_OSD_CLK_SRC,
		&imapx800_bus_and_dev_clk_info
		[IDS0_OSD_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("ids0-tvif", "imap-ids0-tvif", "imap-ids0",
		IDS0_TVIF_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[IDS0_TVIF_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("ids1-eitf", "imap-ids1-eitf", "imap-ids1",
		IDS1_EITF_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[IDS1_EITF_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("ids1-ods", "imap-ids1-ods", "imap-ids1",
		IDS1_OSD_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[IDS1_OSD_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("ids1-tvif", "imap-ids1-tvif", "imap-ids1",
		IDS1_TVIF_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[IDS1_TVIF_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("mipi-dphy-con", "imap-mipi-dphy-con", "imap-mipi-dsi",
		MIPI_DPHY_CON_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[MIPI_DPHY_CON_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("mipi-dphy-ref", "imap-mipi-dphy-ref", "imap-mipi-dsi",
		MIPI_DPHY_REF_CLK_SRC,
		&imapx800_bus_and_dev_clk_info[MIPI_DPHY_REF_CLK_SRC],
		&imapx800_epll_clk),
	DEV_CLK_INFO_SET("mipi-dphy-pixel", "imap-mipi-dphy-pixel",
		"imap-mipi-dsi",
		MIPI_DPHY_PIXEL_CLK_SRC,
		&imapx800_bus_and_dev_clk_info
		[MIPI_DPHY_PIXEL_CLK_SRC], &imapx800_epll_clk),
	DEV_CLK_INFO_SET("isp-osd", "imap-isp-osd", "imap-isp-osd",
			  ISP_OSD_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[ISP_OSD_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("sd-mmc0", "imap-mmc.0", "sd-mmc0", SD0_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[SD0_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("sd-mmc1", "imap-mmc.1", "sd-mmc1", SD1_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[SD1_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("sd-mmc2", "imap-mmc.2", "sd-mmc2", SD2_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[SD2_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("nand-ecc", "imap-nand-ecc", "imap-nand",
			  NFECC_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[NFECC_CLK_SRC],
			  &imapx800_epll_clk),
	DEV_CLK_INFO_SET("hdmi", "imap-hdmi", "imap-hdmi", HDMI_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[HDMI_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("uart0", "imap-uart.0", NULL, UART_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[UART_CLK_SRC],
			  &imapx800_epll_clk),
	DEV_CLK_INFO_SET("uart1", "imap-uart.1", NULL, UART_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[UART_CLK_SRC],
			  &imapx800_epll_clk),
	DEV_CLK_INFO_SET("uart2", "imap-uart.2", NULL, UART_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[UART_CLK_SRC],
			  &imapx800_epll_clk),
	DEV_CLK_INFO_SET("uart3", "imap-uart.3", NULL, UART_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[UART_CLK_SRC],
			  &imapx800_epll_clk),
	DEV_CLK_INFO_SET("spdif", "imap-spdif", "imap-spdif", SPDIF_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[SPDIF_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("audio-clk", "imap-audio-clk", "imap-audio-clk",
			  AUDIO_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[AUDIO_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("usb-ref", "imap-usb-ref", "imap-usb", USBREF_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[USBREF_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("clk-out0", "imap-clk.0", "imap-clk.0",
			  CLKOUT0_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[CLKOUT0_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("clk-out1", "imap-clk.1", "imap-clk.1",
			  CLKOUT1_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[CLKOUT1_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("sata-phy", "imap-sata-phy", "imap-sata",
			  SATAPHY_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[SATAPHY_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("camo", "imap-camo", "imap-camo", CAMO_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[CAMO_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("ddr-phy", "imap-ddr-phy", "imap-ddr-phy",
			  DDRPHY_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[DDRPHY_CLK_SRC],
			  &imapx800_vpll_clk),
	DEV_CLK_INFO_SET("crypto", "imap-crypto", "imap-crytpo",
			  CRYPTO_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[CRYPTO_CLK_SRC],
			  &imapx800_dpll_clk),
	DEV_CLK_INFO_SET("touch screen", "imap-tsc", "NULL", TSC_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[TSC_CLK_SRC],
			  &imapx800_osc_clk),
	DEV_CLK_INFO_SET("dmic", "imap-dmic", "imap-dmic", DMIC_CLK_SRC,
			  &imapx800_bus_and_dev_clk_info[DMIC_CLK_SRC],
			  &imapx800_dpll_clk),
};


#define APB_DEV_CLK_SET(_name, _dev, _con) \
{\
	.name = _name, \
	.lookup = {\
		.dev_id = _dev, \
		.con_id = _con, \
	}, \
	.parent = &imapx800_apb_clk, \
	.ops = &imapx800_apb_ops, \
	.state = CLK_UNINITIAL, \
	.info = NULL, \
}
static struct clk imapx800_apb_dev_clk[] = {
	APB_DEV_CLK_SET("ps2-0", "imap-pic.0", "imap-pic"),
	APB_DEV_CLK_SET("ps2-1", "imap-pic.1", "imap-pic"),
	APB_DEV_CLK_SET("pwm0", "imap-pwm.0", "imap-pwm"),
	APB_DEV_CLK_SET("pwm1", "imap-pwm.1", "imap-pwm"),
	APB_DEV_CLK_SET("pwm2", "imap-pwm.2", "imap-pwm"),
	APB_DEV_CLK_SET("pwm3", "imap-pwm.3", "imap-pwm"),
	APB_DEV_CLK_SET("pwm4", "imap-pwm.4", "imap-pwm"),
	APB_DEV_CLK_SET("i2c0", "imap-iic.0", "imap-iic"),
	APB_DEV_CLK_SET("i2c1", "imap-iic.1", "imap-iic"),
	APB_DEV_CLK_SET("i2c2", "imap-iic.2", "imap-iic"),
	APB_DEV_CLK_SET("i2c3", "imap-iic.3", "imap-iic"),
	APB_DEV_CLK_SET("i2c4", "imap-iic.4", "imap-iic"),
	APB_DEV_CLK_SET("i2c5", "imap-iic.5", "imap-iic"),
	APB_DEV_CLK_SET("spi0", "imap-ssp.0", "imap-ssp"),
	APB_DEV_CLK_SET("spi1", "imap-ssp.1", "imap-ssp"),
	APB_DEV_CLK_SET("keyboard", "imap-keybd", "NULL"),
	APB_DEV_CLK_SET("watch dog", "imap-wtd", "NULL"),
	APB_DEV_CLK_SET("cmn-timer0", "imap-cmn-timer.0", "imap-cmn-timer"),
	APB_DEV_CLK_SET("cmn-timer1", "imap-cmn-timer.1", "imap-cmn-timer"),
	APB_DEV_CLK_SET("simc0", "imap-simc.0", "imap-simc"),
	APB_DEV_CLK_SET("simc1", "imap-simc.1", "imap-simc"),
};


#define DEV_ON_BUS_CLK_SET(_name, _dev, _con, _info, _parent) \
{\
	.name = _name, \
	.lookup = {\
		.dev_id = _dev, \
		.con_id = _con, \
	}, \
	.parent = _parent, \
	.ops = &imapx800_dev_on_bus_ops, \
	.state = CLK_UNINITIAL, \
	.info = _info, \
}
#define BUS_FREQ_RANGE(_dev_id, _max, _min) \
{\
	.dev_id = _dev_id, \
	.max = _max, \
	.min = _min, \
}

/* unit K */
static struct bus_freq_range imapx800_bus_range[7] = {
	BUS_FREQ_RANGE(BUS1, 300000, 100000),
	BUS_FREQ_RANGE(BUS2, 300000, 100000),
	BUS_FREQ_RANGE(BUS3, 500000, 40000),
	BUS_FREQ_RANGE(BUS4, 300000, 30000),
	BUS_FREQ_RANGE(BUS5, 300000, 100000),
	BUS_FREQ_RANGE(BUS6, 300000, 100000),
	BUS_FREQ_RANGE(BUS7, 300000, 100000),
};

static struct clk imapx800_dev_on_bus_clk[] = {
	DEV_ON_BUS_CLK_SET("ids0", "imap-ids.0", "imap-ids0",
			&imapx800_bus_range[BUS1], &imapx800_bus_clk[BUS1]),
	DEV_ON_BUS_CLK_SET("ids1", "imap-ids.1", "imap-ids1",
			&imapx800_bus_range[BUS2], &imapx800_bus_clk[BUS2]),
	DEV_ON_BUS_CLK_SET("gps", "imap-gps", "NULL",
			&imapx800_bus_range[BUS2], &imapx800_bus_clk[BUS2]),
	DEV_ON_BUS_CLK_SET("crypto-dma", "imap-crypto-dma", "imap-crypto",
			&imapx800_bus_range[BUS2], &imapx800_bus_clk[BUS2]),
	DEV_ON_BUS_CLK_SET("gpu", "imap-gpu", "imap-gpu",
			&imapx800_bus_range[BUS3], &imapx800_bus_clk[BUS3]),
	DEV_ON_BUS_CLK_SET("vdec", "imap-vdec", "imap-vdec",
			&imapx800_bus_range[BUS4], &imapx800_bus_clk[BUS4]),
	DEV_ON_BUS_CLK_SET("venc", "imap-venc", "imap-venc",
			&imapx800_bus_range[BUS4], &imapx800_bus_clk[BUS4]),
	DEV_ON_BUS_CLK_SET("isp", "imap-isp", "imap-isp",
			&imapx800_bus_range[BUS7], &imapx800_bus_clk[BUS7]),
	DEV_ON_BUS_CLK_SET("mipi-csi", "imap-mipi-csi", "imap-mipi-csi",
			&imapx800_bus_range[BUS5], &imapx800_bus_clk[BUS5]),
	DEV_ON_BUS_CLK_SET("tsif", "imap-tsif", "imap-tsif",
			&imapx800_bus_range[BUS5], &imapx800_bus_clk[BUS5]),
	DEV_ON_BUS_CLK_SET("usb ohci", "imap-usb-ohci", "imap-usb",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("usb ehci", "imap-usb-ehci", "imap-usb",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("usb otg", "imap-otg", "NULL",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("nandflash", "imap-nandflash", "imap-nand",
			&imapx800_bus_range[BUS7], &imapx800_bus_clk[BUS7]),
	DEV_ON_BUS_CLK_SET("sdmmc0", "sdmmc.0", "sdmmc0",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("sdmmc1", "sdmmc.1", "sdmmc1",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("sdmmc2", "sdmmc.2", "sdmmc2",
			&imapx800_bus_range[BUS6], &imapx800_bus_clk[BUS6]),
	DEV_ON_BUS_CLK_SET("sata", "imap-sata", "imap-sata",
			&imapx800_bus_range[BUS7], &imapx800_bus_clk[BUS7]),
	DEV_ON_BUS_CLK_SET("eth", "imap-mac", "NULL",
			&imapx800_bus_range[BUS7], &imapx800_bus_clk[BUS7]),

};


#define CLK_FOR_VITUAL(_name, _dev, _con) \
{\
	.name = _name, \
	.lookup = {\
		.dev_id = _dev, \
		.con_id = _con, \
	}, \
	.parent = NULL, \
	.ops = NULL, \
	.state = CLK_UNINITIAL, \
}
/* vitual clk */
static struct clk imapx800_vitual_clk[] = {
	CLK_FOR_VITUAL("dma", "dma-pl330", "dma"),
};


/*
 * imapx init one clock
 */
static void imapx_init_one_clock(struct clk *c)
{
	clk_init(c);

	/* TODO
	 *
	 * depand on tegra code
	 * it as "INIT_LIST_HEAD(&c->shared_bus_list)"
	 */
	if (!c->lookup.dev_id && !c->lookup.con_id)
		c->lookup.con_id = c->name;
	c->lookup.clk = c;
	clkdev_add(&c->lookup);
}

static int __init reset_gpu_clk(void)
{
	if (item_exist("bus3.src")) {
		if (item_equal("bus3.src", "apll", 0)) {
#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
			imapx800_bus_clk[2].parent = NULL;
#else /*  */
			imapx800_bus_clk[2].parent = &imapx800_apll_clk;
#endif /*  */
			imapx800_bus_and_dev_clk_info[2].clk_src = APLL;
		}
		if (item_equal("bus3.src", "dpll", 0)) {
#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
			imapx800_bus_clk[2].parent = NULL;
#else /*  */
			imapx800_bus_clk[2].parent = &imapx800_dpll_clk;
#endif /*  */
			imapx800_bus_and_dev_clk_info[2].clk_src = DPLL;
		}
	}
	return 0;
}

core_initcall(reset_gpu_clk);
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

static void reset_apll(uint16_t pll)
{
	writel(0x00, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x0c);
	writel(pll & 0xff, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x00);
	writel((pll >> 8) & 0xff, IO_ADDRESS(SYSMGR_CLKGEN_BASE) + 0x04);
	writel(0x01, IO_ADDRESS(SYSMGR_SYSMGR_BASE) + 0x18);
}
EXPORT_SYMBOL(reset_apll);


/*
 * imap init clocks
 */
void __init imapx910_init_clocks(void)
{
	int i;
	struct clk *clksrc;
	set_pll(DPLL, 0x003f);
#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
	imapx_init_one_clock(&imapx800_cpu_clk);
	imapx_init_one_clock(&imapx800_apb_clk);

#else /*  */
	for (i = 0; i < ARRAY_SIZE(imapx800_sys_clk); i++)
		imapx_init_one_clock(imapx800_sys_clk[i]);

#endif /*  */
	    for (i = 0; i < ARRAY_SIZE(imapx800_bus_clk); i++)
		imapx_init_one_clock(&imapx800_bus_clk[i]);
	 for (i = 0; i < ARRAY_SIZE(imapx800_dev_clk); i++)
		imapx_init_one_clock(&imapx800_dev_clk[i]);
	 for (i = 0; i < ARRAY_SIZE(imapx800_apb_dev_clk); i++)
		imapx_init_one_clock(&imapx800_apb_dev_clk[i]);
	for (i = 0; i < ARRAY_SIZE(imapx800_dev_on_bus_clk); i++)
		imapx_init_one_clock(&imapx800_dev_on_bus_clk[i]);
	 for (i = 0; i < ARRAY_SIZE(imapx800_vitual_clk); i++)
		imapx_init_one_clock(&imapx800_vitual_clk[i]);

}


#ifdef CONFIG_PM
static void imapx800_bus_and_dev_clk(struct bus_and_dev_clk_info *info,
				     uint32_t index)
{
	uint32_t val;
	 val =
	    ((info->nco_en & 0x1) << BUS_NCO) | ((info->clk_src & 0x7) <<
						 BUS_CLK_SRC_BIT);
	bus_and_dev_clk_para_set(index, val);
	 if (info->nco_en)
		bus_and_dev_nco_set(index, info->nco_value);
	else
		bus_and_dev_clk_divider_ratio_set(index, info->clk_divider);
}

static uint32_t g_clkgate[34];
static uint32_t g_clknco[34];
static uint16_t g_pllval[4];
void imapx910_suspend_clock(void)
{
	int i;

		/* apll */
	g_pllval[0] = __clk_readl(0x0) | (__clk_readl(0x4) << 8);

	/* dpll */
	g_pllval[1] = __clk_readl(0x18) | (__clk_readl(0x1c) << 8);

	/* epll */
	g_pllval[2] = __clk_readl(0x30) | (__clk_readl(0x34) << 8);
	for (i = 0; i < 34; i++) {
		g_clkgate[i] = __clk_readl(0x13c + i * 0x10);
		g_clknco[i] = __clk_readl(0x138 + i * 0x10);
	}
}

void imapx910_resume_clock(void)
{
	int i;
	/*reset_apll(g_pllval[0]);*/
	set_pll(DPLL, g_pllval[1]);
	/*set_pll(EPLL, g_pllval[2]);*/
	for (i = 0; i < 34; i++) {
		__clk_writel(g_clkgate[i], 0x13c + i * 0x10);
		__clk_writel(g_clknco[i], 0x138 + i * 0x10);
	}

	/* resume bus and dev clk */
	for (i = 0; i < ARRAY_SIZE(imapx800_bus_and_dev_clk_info); i++) {
		if (i <= 29)
			imapx800_bus_and_dev_clk(
					&imapx800_bus_and_dev_clk_info[i], i);
	}
}

#endif /*  */
void imap_reset_clock(void)
{
	struct clk *clk;

	__clk_writel(0x1, 0x1ac);
	__clk_writel(0x1, 0x1bc);
	__clk_writel(0x1, 0x1cc);

	__clk_writel(0x1, 0x1dc);
	__clk_writel(0x1, 0x1ec);
	__clk_writel(0x1, 0x1fc);

	__clk_writel(0x1, 0x20c);
	__clk_writel(0x1, 0x21c);
	__clk_writel(0x1, 0x22c);

	__clk_writel(0x1, 0x23c);

	__clk_writel(0x1, 0x24c);
	__clk_writel(0x1, 0x25c);
	__clk_writel(0x1, 0x26c);

	__clk_writel(0x1, 0x27c);

	__clk_writel(0x1, 0x28c);

	__clk_writel(0x1, 0x2ac);

	__clk_writel(0x1, 0x2bc);

	__clk_writel(0x1, 0x2cc);

	__clk_writel(0x1, 0x2dc);
	__clk_writel(0x1, 0x2ec);

	__clk_writel(0x1, 0x2fc);

	__clk_writel(0x1, 0x30c);

	__clk_writel(0x1, 0x32c);

	__clk_writel(0x1, 0x33c);
	__clk_writel(0x1, 0x34c);

	__clk_writel(0x1, 0x13c);

	__clk_writel(0x1, 0x14c);
	__clk_writel(0x1, 0x15c);

	__clk_writel(0x1, 0x17c);
	__clk_writel(0x1, 0x18c);
	__clk_writel(0x1, 0x19c);

	clk = clk_get(NULL, "ddr-phy");
	if (clk)
		clk_enable(clk);
}


