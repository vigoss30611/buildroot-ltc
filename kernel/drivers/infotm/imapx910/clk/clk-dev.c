#include <linux/slab.h>
#include <linux/clk-private.h>
#include <mach/clk.h>

static int dev_clk_div(int a, int div, int min, int max)
{
	int val;

	val = (a + div / 2) / div;
	if (val > max)
		val = max;
	else if (val < min)
		val = min;

	return val;
}

static unsigned long clk_sub(unsigned long a, unsigned long b)
{
	unsigned long sub;

	if (a > b)
		sub = a - b;
	else
		sub = b - a;
	return sub;
}

static inline void dev_clk_set_src(uint32_t index, uint32_t para)
{
	__clk_writel(para, BUS_CLK_SRC(index));
}

static inline void dev_clk_set_div(uint32_t index, uint32_t r)
{
	__clk_writel((r & 0x1f), BUS_CLK_DIVI_R(index));
}

static inline void dev_clk_set_nco(uint32_t index, uint32_t val)
{
	__clk_writel(val, BUS_CLK_NCO(index));
}

static int imapx_dev_is_enabled(struct clk_hw *hw)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	uint32_t val;

	val = __clk_readl(BUS_CLK_GATE(dev->type));

	return !(val & 0x1);
}

static int imapx_dev_enable(struct clk_hw *hw)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	unsigned long flags = 0;

	/* ddr clk do not need to enable in kernel */
	if (dev->type == DDRPHY_CLK_SRC)
		return 0;

	if (dev->lock)
		spin_lock_irqsave(dev->lock, flags);

	__clk_writel(GATE_ENABLE, BUS_CLK_GATE(dev->type));

	if (dev->lock)
		spin_unlock_irqrestore(dev->lock, flags);
	return 0;
}

static void imapx_dev_disable(struct clk_hw *hw)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	unsigned long flags = 0;

	/* we can not disable bus clk, because when suspend&resume,
	 * kernel maybe cannot work right
	 * */
	if (dev->type >= BUS1 && dev->type <= BUS7)
		return;

	if (dev->type == DDRPHY_CLK_SRC)
		return;

	if (dev->lock)
		spin_lock_irqsave(dev->lock, flags);

	__clk_writel(GATE_DISABLE, BUS_CLK_GATE(dev->type));

	if (dev->lock)
		spin_unlock_irqrestore(dev->lock, flags);
}

static int imapx_dev_set_parent(struct clk_hw *hw, u8 index)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	unsigned long flags = 0;
	uint32_t val;

	if (index < APLL && index > VPLL)
		return -EINVAL;

	if (dev->lock)
		spin_lock_irqsave(dev->lock, flags);

	imapx_dev_disable(hw);

	val = __clk_readl(BUS_CLK_SRC(dev->type));
	val &= ~(0x7 << 0);
	val |= index;
	__clk_writel(val, BUS_CLK_SRC(dev->type));

	imapx_dev_enable(hw);

	if (dev->lock)
		spin_unlock_irqrestore(dev->lock, flags);
	return 0;
}

static u8 imapx_dev_get_parent(struct clk_hw *hw)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	uint32_t val;

	val = __clk_readl(BUS_CLK_SRC(dev->type));
	val = val & 0x7;

	return val;
}

/*
 * in nco mode, clk = parent_rate * div / 256;
 * in div mode, clk = parent_rate / (div + 1).
 */
static unsigned long imapx_dev_recalc_rate(struct clk_hw *hw,
					unsigned long parent_rate)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	unsigned long rate = parent_rate;
	int div, val;

	val = __clk_readl(BUS_CLK_SRC(dev->type));
	if ((val & 0x7) == OSC_CLK)
		return rate;

	if (val & CLK_NCO_MODE) {
		div = __clk_readl(BUS_CLK_NCO(dev->type));
		rate = parent_rate / 256 * div;
	} else {
		div = __clk_readl(BUS_CLK_DIVI_R(dev->type));
		rate = parent_rate / (div + 1);
	}

	return rate;
}

static long imapx_dev_round_rate(struct clk_hw *hw, unsigned long rate,
				 unsigned long *prate)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	struct bus_and_dev_clk_info *info = dev->info;
	uint32_t div_rate, nco_rate;

	/*
	 * if nco mode enable, we must compare with div mode,
	 * we choose the best one.
	 */

	info->clk_divider = dev_clk_div(*prate, rate, 1, 32) - 1;
	info->nco_en = 0;
	div_rate = *prate / (info->clk_divider + 1);

	if (info->nco_disable == ENABLE) {
		/* if we do not div 1000, the val rate * 256 may > 0xffffffff */
		info->nco_value = dev_clk_div(rate / 1000 * 256,  *prate / 1000,
									1, 64);
		nco_rate = *prate / 256 * info->nco_value;

		if (clk_sub(div_rate, rate) > clk_sub(nco_rate, rate)) {
			info->nco_en = 1;
			return nco_rate;
		}
	}
	return div_rate;
}

static int imapx_dev_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	struct bus_and_dev_clk_info *info = dev->info;
	unsigned long flags = 0;
	int val;

	if (dev->lock)
		spin_lock_irqsave(dev->lock, flags);

	imapx_dev_disable(hw);
	/*
	 * check whether if rate is possibility to be set
	 * and reset rate
	 */
	val = __clk_readl(BUS_CLK_SRC(dev->type));
	if (((val >> 3) & 0x1) != info->nco_en) {
		val &= ~(0x1 << 3);
		val |= info->nco_en << 3;
		dev_clk_set_src(dev->type, val);
	}

	if (info->nco_en)
		dev_clk_set_nco(dev->type, info->nco_value);
	else
		dev_clk_set_div(dev->type, info->clk_divider);
	imapx_dev_enable(hw);

	if (dev->lock)
		spin_unlock_irqrestore(dev->lock, flags);
	return 0;
}

static void imapx_dev_init(struct clk_hw *hw)
{
	struct imapx_dev_clk *dev = to_clk_dev(hw);
	struct bus_and_dev_clk_info *info = dev->info;
	uint32_t val;

	if (dev->type == DDRPHY_CLK_SRC || dev->type == SYSBUS_CLK_SRC
		|| dev->type == APB_CLK_SRC)
		return;

	val = ((info->nco_en & 0x1) << BUS_NCO) | ((info->clk_src & 0x7) <<
			BUS_CLK_SRC_BIT);
	dev_clk_set_src(dev->type, val);
	dev_clk_set_div(dev->type, 0);
	if (info->nco_en)
		dev_clk_set_nco(dev->type, info->nco_value);
	else
		dev_clk_set_div(dev->type, info->clk_divider);
}

const struct clk_ops dev_ops = {
	.is_enabled = imapx_dev_is_enabled,
	.enable = imapx_dev_enable,
	.disable = imapx_dev_disable,
	.set_parent = imapx_dev_set_parent,
	.get_parent = imapx_dev_get_parent,
	.recalc_rate = imapx_dev_recalc_rate,
	.round_rate = imapx_dev_round_rate,
	.set_rate = imapx_dev_set_rate,
	.init = imapx_dev_init,
};

struct clk *imapx_dev_clk_register(const char *name,
				const char **parent_names, int num_parents,
				enum dev_type type, unsigned long flags,
				struct bus_and_dev_clk_info *info,
				spinlock_t *lock)
{
	struct imapx_dev_clk *dev;
	struct clk_init_data init;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.flags = flags;
	init.ops = &dev_ops;
	init.parent_names = parent_names;
	init.num_parents = num_parents;

	dev->type = type;
	dev->lock = lock;
	dev->info = info;
	dev->hw.init = &init;
	return clk_register(NULL, &dev->hw);
}
