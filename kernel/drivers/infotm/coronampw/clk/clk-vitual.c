#include <linux/slab.h>
#include <mach/clk.h>

static const struct clk_ops dump_ops;
/*
 * for some clk which in standard driver, may apply for but,
 * in infotm soc, the clk need not.
 */
struct clk *imapx_vitual_clk_register(const char *name)
{
	struct imapx_vitual_clk *vitual;
	struct clk_init_data init;

	vitual = kzalloc(sizeof(*vitual), GFP_KERNEL);
	if (!vitual)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &dump_ops;
	init.flags = CLK_IS_BASIC;
	init.parent_names = NULL;
	init.num_parents = 0;

	vitual->hw.init = &init;
	return clk_register(NULL, &vitual->hw);
}
