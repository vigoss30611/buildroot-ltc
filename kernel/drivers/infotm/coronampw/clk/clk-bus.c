#include <linux/clk-provider.h>
#include <linux/slab.h>
#include <mach/clk.h>

static unsigned long imapx_bus_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	return parent_rate;
}

static long imapx_bus_round_rate(struct clk_hw *hw,
				unsigned long rate, unsigned long *prate)
{
	struct imapx_bus_clk *bus = to_clk_bus(hw);
	struct bus_freq_range *range = bus->range;
	unsigned long best_parent = rate;

	if (!range) {
		if (rate > range->max)
			best_parent = range->max;
		else if (rate < range->min)
			best_parent = range->min;
	}
	*prate =  __clk_round_rate(__clk_get_parent(hw->clk), best_parent);

	return *prate;
}

static int imapx_bus_set_rate(struct clk_hw *hw,
				unsigned long rate, unsigned long parent_rate)
{
	return 0;
}

const struct clk_ops bus_ops = {
	.recalc_rate = imapx_bus_recalc_rate,
	.round_rate = imapx_bus_round_rate,
	.set_rate = imapx_bus_set_rate,
};

struct clk *imapx_bus_clk_register(const char *name,
			const char *parent_names, struct bus_freq_range *range,
			unsigned long flags)
{
	struct imapx_bus_clk *bus;
	struct clk_init_data init;

	bus = kzalloc(sizeof(*bus), GFP_KERNEL);
	if (!bus)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.flags = flags | CLK_IS_BASIC;
	init.ops = &bus_ops;
	init.parent_names = parent_names ? &parent_names : NULL;
	init.num_parents = parent_names ? 1 : 0;

	bus->range = range;
	bus->hw.init = &init;

	return clk_register(NULL, &bus->hw);
}
