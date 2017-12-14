/* display lcdc driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/display_device.h>
#include <mach/pad.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"lcdc"


static int lcdc_enable(struct display_device *pdev, int en)
{
	if (!pdev->enable && en) {
		ids_write(pdev->path, LCDCON1, LCDCON1_ENVID, 1, 1);
	}
	else if (pdev->enable && !en) {
		ids_write(pdev->path, LCDCON1, LCDCON1_ENVID, 1, 0);
	}
	else
		dlog_dbg("skip duplicated lcdc enable operation\n");

	pdev->enable = en;

	return 0;
}

static int __lcdc_set_panel_size(struct display_device *pdev, int vic)
{
	dtd_t dtd;
	int ret, width;

	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}

	if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY ||
			pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_DUMMY_RGB)
		width = dtd.mHActive * 4;
	else if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB)
		width = dtd.mHActive * 3;
	else if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_PARALLEL_RGB888 ||
			pdev->interface_type == DISPLAY_INTERFACE_TYPE_PARALLEL_RGB565 ||
			pdev->interface_type == DISPLAY_INTERFACE_TYPE_I80 ||
			pdev->interface_type == DISPLAY_INTERFACE_TYPE_HDMI)
		width = dtd.mHActive;
	else {
		dlog_err("Invalid interface_type %d\n", pdev->interface_type);
		return -EINVAL;
	}

	ids_writeword(pdev->path, LCDCON6, ((dtd.mVActive - 1) << LCDCON6_LINEVAL) | (width - 1));

	return 0;
}

static int __lcdc_set_vclk(struct display_device *pdev, int sub_divider)
{
	dlog_dbg("lcdc vclk divider is %d\n", sub_divider);
	ids_write(pdev->path, LCDCON1, LCDCON1_CLKVAL, 10, (sub_divider-1));
	return 0;
}

static int __lcdc_calc_fps(struct display_device *pdev, struct clk *clk_src,
						int vic, int sub_divider)
{
	dtd_t dtd;
	unsigned long htotal, vtotal, freq, mod;

	vic_fill(pdev->path, &dtd, vic);
	if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY ||
			pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_DUMMY_RGB)
		htotal = dtd.mHActive*4+dtd.mHBlanking;
	else if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB)
		htotal = dtd.mHActive*3+dtd.mHBlanking;
	else
		htotal = dtd.mHActive+dtd.mHBlanking;
	vtotal = dtd.mVActive+dtd.mVBlanking;
	freq = clk_get_rate(clk_src);
	do_div(freq, sub_divider);
	mod = do_div(freq, vtotal);	
	if (mod * 2 >= vtotal)
			freq++;
	mod = do_div(freq, htotal);
	if (mod * 2 >= htotal)
		freq++;

	return (int)freq;
}

static int __lcdc_set_pixel_clk(struct display_device *pdev, int vic)
{
	int ret;
	int sub_divider;
	int fps;
	dtd_t dtd;
	struct clk *clkp;
	unsigned long freq;

	clkp = clk_get_sys(IDS_OSD_CLK_SRC, IDS_CLK_SRC_PARENT);
	freq = clk_get_rate(clkp);
	
	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	dlog_dbg("dtd.mHActive=%d, osd freq=%ld\n", dtd.mHActive, freq);
	if (dtd.mHActive >= 3840 && freq != IDS_OSD_CLK_FREQ_H)
		clk_set_rate(clkp, IDS_OSD_CLK_FREQ_H);
	else if (dtd.mHActive >= 1920 && dtd.mHActive < 3840 && freq != IDS_OSD_CLK_FREQ_M)
		clk_set_rate(clkp, IDS_OSD_CLK_FREQ_M);
	else if (dtd.mHActive < 1920 && freq != IDS_OSD_CLK_FREQ_L)
		clk_set_rate(clkp, IDS_OSD_CLK_FREQ_L);
	
	pdev->clk_src = clk_get_sys(IDS_EITF_CLK_SRC, IDS_CLK_SRC_PARENT);
	ret = display_set_clk(pdev, pdev->clk_src, vic, &sub_divider);
	if (ret) {
		dlog_err("set clk failed %d\n", ret);
		return ret;
	}
	ret = __lcdc_set_vclk(pdev, sub_divider);
	if (ret) {
		dlog_err("lcdc set vclk failed %d\n", ret);
		return ret;
	}
	if (pdev->interface_type != DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY &&
			pdev->interface_type != DISPLAY_INTERFACE_TYPE_SERIAL_DUMMY_RGB &&
			pdev->interface_type != DISPLAY_INTERFACE_TYPE_SERIAL_RGB) {
		fps = __lcdc_calc_fps(pdev, pdev->clk_src, vic, sub_divider);
		dlog_info("lcdc fps is %d\n", fps);
	}

	return 0;
}

static int __lcdc_set_timing_params(struct display_device *pdev, int vic)
{
	int ret;
	dtd_t dtd;
	unsigned int val, vbpd, hbpd;

	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}

	vbpd = dtd.mVBlanking - dtd.mVSyncOffset - dtd.mVSyncPulseWidth;
	hbpd = dtd.mHBlanking - dtd.mHSyncOffset - dtd.mHSyncPulseWidth;
	dlog_dbg("vbpd = %d, hbpd = %d\n", vbpd, hbpd);

	val = ((vbpd-1) << LCDCON2_VBPD) |
			((dtd.mVSyncOffset-1) << LCDCON2_VFPD);
	ids_writeword(pdev->path, LCDCON2, val);

	val = ((dtd.mVSyncPulseWidth-1) << LCDCON3_VSPW) |
			((dtd.mHSyncPulseWidth-1) << LCDCON3_HSPW);
	ids_writeword(pdev->path, LCDCON3, val);

	val = ((hbpd-1) << LCDCON4_HBPD) |
			((dtd.mHSyncOffset-1) << LCDCON4_HFPD);
	ids_writeword(pdev->path, LCDCON4, val);

	val = ((!dtd.mVclkInverse) << LCDCON5_INVVCLK) |
			((!dtd.mHSyncPolarity) << LCDCON5_INVVLINE) |
			((!dtd.mVSyncPolarity) << LCDCON5_INVVFRAME) |
			(0 << LCDCON5_INVVD) |
			(0 << LCDCON5_INVVDEN);
	ids_write(pdev->path, LCDCON5, 6, 5, (val>>LCDCON5_INVVDEN));

	return 0;
}

static int lcdc_set_vic(struct display_device *pdev, int vic)
{
	pdev->vic = vic;
	__lcdc_set_timing_params(pdev, vic);
	__lcdc_set_panel_size(pdev, vic);
	__lcdc_set_pixel_clk(pdev, vic);

	return 0;
}

static int lcdc_set_interface_type(struct display_device *pdev, 
			enum display_interface_type type)
{
	int rgb565 = 0, serial_rgb = 0, srgb_type = 0, ds_type = 0;
	pdev->interface_type = type;

	ids_write(pdev->path, OVCDCR, OVCDCR_IfType, 1, 0);
	ids_write(pdev->path, OVCDCR, OVCDCR_Interlace, 1, 0);

	if (type == DISPLAY_INTERFACE_TYPE_PARALLEL_RGB565)
		rgb565 = 1;
	ids_write(pdev->path, LCDCON5, LCDCON5_RGB565IF, 1, rgb565);

	if (type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY ||
			type == DISPLAY_INTERFACE_TYPE_SERIAL_DUMMY_RGB ||
			type == DISPLAY_INTERFACE_TYPE_SERIAL_RGB) {
		serial_rgb = 1;
		srgb_type = type - DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY;
#ifdef CONFIG_ARCH_APOLLO
		dlog_err("Apollo does NOT support SerialRGB\n");
		return -EINVAL;
#endif
	}

#ifndef CONFIG_ARCH_APOLLO
	ids_write(pdev->path, LCDCON5, LCDCON5_SRGB_TYPE, 2, srgb_type);
	ids_write(pdev->path, LCDCON5, LCDCON5_SRGB_ENABLE, 1, serial_rgb);
#endif

	if (type == DISPLAY_INTERFACE_TYPE_I80)
		ds_type = 1;
	ids_write(pdev->path, LCDCON5, LCDCON5_DSPTYPE, 2, ds_type);

	__lcdc_set_panel_size(pdev, pdev->vic);	// sRGB width need * 3 or * 4
	osd_load_param(pdev);

	if (serial_rgb)
		imapx_pad_init("srgb");
	else
		imapx_pad_init("rgb0");

	return 0;
}

static int lcdc_set_rgb_order(struct display_device *pdev,
			enum display_lcd_rgb_order order)
{
	pdev->rgb_order = order;
	ids_write(pdev->path, LCDCON5, LCDCON5_RGBORDER, 6, order);
	return 0;
}

static int lcdc_set_auto_vclk(struct display_device *pdev, int cdown, int rfrm_num)
{
	pdev->cdown = cdown;
	pdev->rfrm_num = rfrm_num;
	ids_write(pdev->path, LCDVCLKFSR, 24, 4, cdown);
	ids_write(pdev->path, LCDVCLKFSR, 16, 6, rfrm_num);
	return 0;
}

static int lcdc_probe(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static int lcdc_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver lcdc_driver = {
	.probe  = lcdc_probe,
	.remove = lcdc_remove,
	.driver = {
		.name = "lcdc",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_LCDC,
	.function = {
		.enable = lcdc_enable,
		.set_vic = lcdc_set_vic,
		.set_interface_type = lcdc_set_interface_type,
		.set_rgb_order = lcdc_set_rgb_order,
		.set_auto_vclk = lcdc_set_auto_vclk,
	}
};

module_display_driver(lcdc_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display lcd controller driver");
MODULE_LICENSE("GPL");
