/* display clk driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/display_device.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"clk"

extern const char *__clk_get_name(struct clk *clk);

static bool __clk_switch_pll(struct display_device *pdev,
			char * pll_str, unsigned long target_freq, unsigned long *clk_mod_p)
{
	struct clk * clk_pll;
	unsigned long pll_clk_rate, clk_mod;
	int ret = false;

	clk_pll = clk_get_sys(NULL, pll_str);
	if (IS_ERR(clk_pll)) {
		dlog_err("Failed to get clk %s, ret=%ld\n", pll_str, PTR_ERR(clk_pll));
		return PTR_ERR(clk_pll);
	}
	pll_clk_rate = clk_get_rate(clk_pll);
	clk_mod = do_div(pll_clk_rate, target_freq);
	if (!clk_mod) {
		dlog_dbg("=> %s can satisfy destination clock %ld\n", pll_str, target_freq);
		ret = true;
	}
	else {
		dlog_dbg("=> %s can NOT satisfy destination clock %ld, clk_mod=%ld\n",
						pll_str, target_freq, clk_mod);
		ret = false;
	}

	*clk_mod_p = clk_mod;
	return ret;
}

int display_set_clk_freq(struct display_device *pdev, struct clk *clk_src,
															unsigned long target_freq, int *sub_divider)
{
	int ret, i;
	char *try_pll_str[3] = {"dpll", "epll", "vpll"};
	unsigned long clk_mod = 0, min_mod = 1000000000, min_index = 0;
	unsigned long new_rate, parent_rate, new_mod, div, sub_div=1, pre_freq;
	struct clk * clk_pll;

	dlog_info("=> Set clk %s to %ld\n", __clk_get_name(clk_src), target_freq);
	if (!target_freq) {
		dlog_err("target_freq can NOT be zero.\n");
		return -EINVAL;
	}

	for (i = 0; i < 3; i++) {
		ret = __clk_switch_pll(pdev, try_pll_str[i], target_freq, &clk_mod);
		if (min_mod > clk_mod) {
			min_mod = clk_mod;
			min_index = i;
		}
		if (ret == true) {
			min_index = i;
			break;
		}
	}
	if (i >= 3) {
		dlog_info("=> Target pixel clock %ld can NOT be divided evenly. The min mod clk is %ld, pll is %s\n",
						target_freq, min_mod, try_pll_str[min_index]);
	}
	dlog_info("=> Switch to %s\n", try_pll_str[min_index]);
	clk_pll = clk_get_sys(NULL, try_pll_str[min_index]);
	if (IS_ERR(clk_pll)) {
		dlog_err("Failed to get clk %s, ret=%ld\n", try_pll_str[min_index], PTR_ERR(clk_pll));
		return PTR_ERR(clk_pll);
	}
	ret = clk_set_parent(clk_src, clk_pll);
	if (ret < 0) {
		dlog_err("Failed to set clk parent %d\n", ret);
		return ret;
	}
	pre_freq = target_freq;
calc_div:
	ret = clk_set_rate(clk_src, pre_freq);
	if (ret < 0) {
		dlog_err("Fail to set clk to target freq %ld\n", pre_freq);
		return ret;
	}
	new_rate = clk_get_rate(clk_src);
	parent_rate = clk_get_rate(clk_pll);
	new_mod = do_div(parent_rate, new_rate);
	dlog_dbg("pre_freq=%ld, new_rate=%ld, mod=%ld\n", pre_freq, new_rate, new_mod);

	/* round */
	div = parent_rate;
	if (new_mod * 2 >= new_rate)
		div++;

	/* assume that nco is not used, divider max value is 64 */
	if (div >= 64 && new_rate > pre_freq && sub_divider) {
		sub_div++;
		pre_freq = target_freq * sub_div;
		goto calc_div;
	}

	if (sub_divider)
		*sub_divider = sub_div;
	do_div(new_rate, sub_div);
	dlog_info("target_freq is %ld, pre_freq is %ld, sub_divider is %ld, actual freq is %ld\n",
						target_freq, pre_freq, sub_div, new_rate);

	return 0;
}

int display_set_clk(struct display_device *pdev, struct clk *clk_src, int vic, int *sub_divider)
{
	int ret;
	dtd_t dtd;
	unsigned long target_freq;

	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	target_freq = (unsigned long)(dtd.mPixelClock) * 1000;
	return display_set_clk_freq(pdev, clk_src, target_freq, sub_divider);
}

static int clock_probe(struct display_device *pdev)
{
	struct clk *clkp;
	int ret;

	dlog_trace();

	/* ids bus clk and osd clk must be enabled before read/write ids register */
	clkp = clk_get_sys(IDS_BUS_CLK_SRC, IDS_CLK_SRC_PARENT);
	if (IS_ERR(clkp)) {
		dlog_err("Failed to get clock %s\n", IDS_BUS_CLK_SRC);
		return PTR_ERR(clkp);
	}
	clk_prepare_enable(clkp);

	clkp = clk_get_sys(IDS_OSD_CLK_SRC, IDS_CLK_SRC_PARENT);
	if (IS_ERR(clkp)) {
		dlog_err("Failed to get clock %s\n", IDS_OSD_CLK_SRC);
		return PTR_ERR(clkp);
	}
	ret = clk_set_rate(clkp, IDS_OSD_CLK_FREQ_L);
	if (ret < 0) {
		dlog_err("Fail to set ids osd clk to freq %d\n", IDS_OSD_CLK_FREQ_L);
		return ret;
	}
	clk_prepare_enable(clkp);

	clkp = clk_get_sys(IDS_EITF_CLK_SRC, IDS_CLK_SRC_PARENT);
	if (IS_ERR(clkp)) {
		dlog_err("Failed to get clock %s\n", IDS_EITF_CLK_SRC);
		return PTR_ERR(clkp);
	}
	clk_prepare_enable(clkp);

	return 0;
}

static int clock_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver clock_driver = {
	.probe  = clock_probe,
	.remove = clock_remove,
	.driver = {
		.name = "clock",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_CLK,
};

module_display_driver(clock_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display clock driver");
MODULE_LICENSE("GPL");
