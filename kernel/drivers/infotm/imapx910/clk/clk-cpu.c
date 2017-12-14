#include <linux/string.h>
#include <linux/clk-private.h>
#include <linux/slab.h>
#include <mach/clk.h>
#include "clock_cpu.h"

static inline void cpu_clk_ctrl(uint32_t mode)
{
	__clk_writel(mode, CPU_CLK_PARA_EN);
}

static inline void cpu_clk_sel(uint32_t type)
{
	__clk_writel(type, CPU_CLK_OUT_SEL);
}

static inline void cpu_clk_set_div(uint32_t en, uint32_t len)
{
	__clk_writel(((en & 0x1) << CPU_DIVIDER_EN) |
			((len & 0x3f) << CPU_DIVLEN), CPU_CLK_DIVIDER_PA);
}

static inline void cpu_clk_set(uint32_t len, uint32_t *cpu,
					uint32_t *axi, uint32_t *apb,
					uint32_t *axi_cpu, uint32_t *apb_cpu,
					uint32_t *apb_axi)
{
	int i;

	for (i = 0; i < len; i++) {
		__clk_writel(cpu[i], CPU_CLK_DIVIDER_SEQ(i));
		__clk_writel(axi[i], CPU_AXI_CLK_DIVIDER_SEQ(i));
		__clk_writel(apb[i], CPU_APB_CLK_DIVIDER_SEQ(i));
		__clk_writel(axi_cpu[i], CPU_AXI_A_CPU_CLK_EN_SEQ(i));
		__clk_writel(apb_cpu[i], CPU_APB_A_CPU_CLK_EN_SEQ(i));
		__clk_writel(apb_axi[i], CPU_APB_A_AXI_CLK_EN_SEQ(i));
	}
}

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

	cpu_clk_ctrl(DISABLE);
	cpu_clk_sel(clk_sel);
	cpu_clk_set_div(ENABLE, c->seq_len);
	cpu_clk_set(r_num, c->cpu_clk, c->axi_clk, c->apb_clk, c->axi_cpu,
		     c->apb_cpu, c->apb_axi);
	cpu_clk_ctrl(ENABLE);
}

static int imapx_cpu_is_enabled(struct clk_hw *hw)
{
	uint32_t val;

	val = __clk_readl(CPU_CLK_PARA_EN);
	return !(val & 0x1);
}

static int imapx_cpu_enable(struct clk_hw *hw)
{
	struct imapx_cpu_clk *cpu = to_clk_cpu(hw);
	unsigned long flags = 0;

	if (cpu->lock)
		spin_lock_irqsave(cpu->lock, flags);

	cpu_clk_ctrl(GATE_ENABLE);

	if (cpu->lock)
		spin_unlock_irqrestore(cpu->lock, flags);

	return 0;
}

static void imapx_cpu_disable(struct clk_hw *hw)
{
	struct imapx_cpu_clk *cpu = to_clk_cpu(hw);
	unsigned long flags = 0;

	if (cpu->lock)
		spin_lock_irqsave(cpu->lock, flags);

	cpu_clk_ctrl(GATE_DISABLE);

	if (cpu->lock)
		spin_unlock_irqrestore(cpu->lock, flags);
}

static unsigned long imapx_cpu_recalc_rate(struct clk_hw *hw,
					unsigned long parent_rate)
{
	struct imapx_cpu_clk *cpu = to_clk_cpu(hw);
	struct clk *clk = hw->clk;

	if (!strcmp(clk->name, "cpu-clk"))
		return parent_rate / ((cpu->ratio >> 16) & 0xff);
	else if (!strcmp(clk->name, "gtm-clk"))
		return parent_rate / (((cpu->ratio >> 16) & 0xff) * 8);
	else if (!strcmp(clk->name, "smp_twd"))
		return parent_rate / (((cpu->ratio >> 16) & 0xff) * 8);
	else
		return parent_rate / (cpu->ratio & 0xff);
}

static long imapx_cpu_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	struct clk *clk = hw->clk;
	int i;

	if (strcmp(clk->name, "cpu-clk"))
		return -1;

	for (i = 0; i < CPU_CLK_NUM; i++)
		if (rate == cpu_clk_data[i][0])
			return rate;

	return -1;
}

static int imapx_cpu_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct imapx_cpu_clk *cpu = to_clk_cpu(hw);
	struct clk *clk = hw->clk;
	uint64_t clk_data[6];
	struct cpu_clk_info c;
	unsigned long flags = 0;
	int i, j, len;

	if (strcmp(clk->name, "cpu-clk"))
		return -1;

	for (i = 0; i < CPU_CLK_NUM; i++) {
		if (cpu_clk_data[i][0] == rate) {
			cpu->ratio = cpu_clk_data[i][0];
			c.cpu_co = (rate >> 16) & 0xff;
			c.seq_len = (rate & 0xff) - 1;
			len = (c.seq_len) >> 3;
			len += 1;
			for (j = 0; j < 6; j++) {
				clk_data[j] =
				    (uint64_t) cpu_clk_data[i][2 + j * 2];
				clk_data[j] = clk_data[j] << 32;
				clk_data[j] |=
				    (uint64_t) cpu_clk_data[i][1 + j * 2];
			}
			for (j = 0; j < len; j++) {
				c.cpu_clk[j] =
				    (uint32_t) ((clk_data[0] >> (j * 8)) &
						0xff);
				c.axi_clk[j] =
				    (uint32_t) ((clk_data[1] >> (j * 8)) &
						0xff);
				c.apb_clk[j] =
				    (uint32_t) ((clk_data[2] >> (j * 8)) &
						0xff);
				c.axi_cpu[j] =
				    (uint32_t) ((clk_data[3] >> (j * 8)) &
						0xff);
				c.apb_cpu[j] =
				    (uint32_t) ((clk_data[4] >> (j * 8)) &
						0xff);
				c.apb_axi[j] =
				    (uint32_t) ((clk_data[5] >> (j * 8)) &
						0xff);
			}
			break;
		}
	}
	if (i == CPU_CLK_NUM)
		return -1;

	if (cpu->lock)
		spin_lock_irqsave(cpu->lock, flags);

	cpu_fr_con(&c);

	if (cpu->lock)
		spin_unlock_irqrestore(cpu->lock, flags);

	return 0;
}

const struct clk_ops cpu_ops = {
	.is_enabled = imapx_cpu_is_enabled,
	.enable = imapx_cpu_enable,
	.disable = imapx_cpu_disable,
	.recalc_rate = imapx_cpu_recalc_rate,
	.round_rate = imapx_cpu_round_rate,
	.set_rate = imapx_cpu_set_rate,
};

struct clk *imapx_cpu_clk_register(const char *name,
				const char *parent_names, unsigned long flags,
				spinlock_t *lock, uint32_t ratio)
{
	struct imapx_cpu_clk *cpu;
	struct clk_init_data init;

	cpu = kzalloc(sizeof(*cpu), GFP_KERNEL);
	if (!cpu)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &cpu_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = parent_names ? &parent_names : NULL;
	init.num_parents = parent_names ? 1 : 0;

	cpu->lock = lock;
	cpu->ratio = ratio;
	cpu->hw.init = &init;

	return clk_register(NULL, &cpu->hw);
}
