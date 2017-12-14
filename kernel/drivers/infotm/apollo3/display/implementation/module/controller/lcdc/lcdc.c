/*
 * lcdc.c - display lcd controller driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <dss_common.h>
#include <ids_access.h>
#include <dss_item.h>
#include <emif.h>
#include <mach/pad.h>

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[controller]"
#define DSS_SUB2	"[lcdc]"

/* Preset strategy about LCDVCLKFSR */
#define PRE_LCDVCLKFSR_CDOWN		0xF		// 0xF: Disable
#define PRE_LCDVCLKFSR_RFRM_NUM		0x1

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);

static struct module_attr lcdc_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node lcdc_module = {
	.name = "lcdc",
	.attributes = lcdc_attr,
	.idsx = 0,
	.init = dss_init_module,
};

static dtd_t gdtd[2];
static  int cdown[2] = {PRE_LCDVCLKFSR_CDOWN, PRE_LCDVCLKFSR_CDOWN};
static int rfrm_num[2] = {PRE_LCDVCLKFSR_RFRM_NUM, PRE_LCDVCLKFSR_CDOWN};

int lcdc_pwren_polarity(int idsx, int polarity)
{
/* Only available on 820 and X15 */
#ifdef CONFIG_MACH_IMAPX800
	dss_trace("ids%d, polarity %d\n", idsx, polarity);
	ids_write(idsx, LCDCON5 , LCDCON5_INVPWREN , 1, polarity);
#endif
	return 0;
}

int lcdc_pwren(int idsx, int enable)
{
/* Only available on 820 and X15 */
#ifdef CONFIG_MACH_IMAPX800
	dss_trace("ids%d, enable %d\n", idsx, enable);
	ids_write(idsx, LCDCON5 , LCDCON5_PWREN , 1, enable);
#endif
	return 0;
}

int lcdc_output_en(int idsx, int enable)
{
	dss_trace("ids%d, enable %d\n", idsx, enable);
	ids_write(idsx, LCDCON1 , LCDCON1_ENVID , 1, enable);
	return 0;
}

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	ids_write(idsx, LCDCON1 ,0 , 1, enable);
	return 0;
}

#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
/* ids1 dedicated to hdmi, ids0 may connect lcd panel or cvbs */
static int set_pixel_clk(int idsx)
{
	int i, vic, ret, val;
	unsigned int clkdiv = 1;
	struct clk * ids_eitf[2] = {NULL};
	int clkdiff = 20000000;
	int reqclk, retclk;
	dss_trace("ids%d\n", idsx);

	vic = gdtd[idsx].mCode;
	dss_dbg("Pixel Clock %d\n", gdtd[idsx].mPixelClock);

	if (idsx == 0) {
		ids_eitf[0] = clk_get_sys(IMAP_IDS0_EITF_CLK, IMAP_IDS0_PARENT_CLK);
		clk_set_rate(ids_eitf[0], 54000000);
	} else if (idsx == 1) {
		ids_eitf[1] = clk_get_sys(IMAP_IDS1_EITF_CLK, IMAP_IDS1_PARENT_CLK);
		clk_set_rate(ids_eitf[1], 297000000);
	} else {
		dss_err("No valid idsx :%d", idsx);
		return -1;
	}

	if (gdtd[idsx].mPixelClock == 2700 || gdtd[idsx].mPixelClock == 2702)
		clkdiv = 11;
	else if (gdtd[idsx].mPixelClock == 7425 || gdtd[idsx].mPixelClock == 7417)
		clkdiv = 4;
	else if (gdtd[idsx].mPixelClock == 14850 || gdtd[idsx].mPixelClock == 14835)
		clkdiv = 2;
	else {
		/* Auto calculate best clkdiv */
		for (i = 1; i < 8; i++) {
			reqclk = gdtd[idsx].mPixelClock * 10 * i;	// KHz
			dss_dbg("reqclk = %d\n", reqclk);
			if (reqclk > 350000) {
				dss_dbg("reqclk exceed 350M\n");
				break;
			}
			ret = clk_set_rate(ids_eitf[idsx], reqclk * 1000);
			if (ret < 0) {
				dss_err("clk_set_rate(ids0_eitf, %d) failed\n", reqclk);
				break;
			}
			retclk = clk_get_rate(ids_eitf[idsx]);
			retclk/= 1000;
			dss_dbg("return clk %d\n", retclk);
			if (retclk == reqclk) {
				clkdiv = i;
				break;
			}
			else if (abs(retclk - reqclk) < clkdiff) {
				clkdiff = abs(retclk - reqclk);
				clkdiv = i;
			}
		}

		if (clkdiff == 20000) {
			dss_err("no smaller clkdiff less than 20M found.\n");
			return -1;
		}

		reqclk = gdtd[idsx].mPixelClock * 10 * clkdiv;
		ret = clk_set_rate(ids_eitf[idsx], reqclk * 1000);
		if (ret < 0) {
			dss_err("clk_set_rate(ids0_eitf, %d) failed\n", reqclk);
			return -1;
		}
		ret = clk_get_rate(ids_eitf[idsx]);
		dss_dbg("ids0_eitf clk set to %d, actual %d, clkdiv %d\n", reqclk, ret/1000, clkdiv);
	}

	val = 0;
	dss_dbg("ids%d, clkdiv = %d\n", idsx, clkdiv);
	val = ((clkdiv-1) << LCDCON1_CLKVAL) ||(0<<LCDCON1_ENVID);
	ids_writeword(idsx, LCDCON1, val);

	return 0;
}
#endif

static void lcdc_set_ids_eitf_clk(int mhz)
{
	struct clk* ids_eitf = clk_get_sys(IMAP_IDS0_EITF_CLK, IMAP_IDS0_PARENT_CLK);
	dss_dbg("set eitf clock :%d MHz\n", mhz);
	clk_set_rate(ids_eitf, mhz * 1000000); /*Note: set 40MHz*/
}

static void lcdc_set_screen_size(int idsx, int width, int height)
{
	u32  val = 0;
	val = (((height - 1) & 0x7ff) << LCDCON6_LINEVAL) | ((width - 1) & 0x7ff);
	dss_dbg("width = 0x%x, height = 0x%x, val = 0x%x\n", width, height, val);
	ids_writeword(idsx, LCDCON6, val);
}

static void lcdc_set_serial_rgb_clk(int idsx, dtd_t* dtd, int iftype, int pix_clk_mhz)
{
	int val, div, col;
	int pix_clk = 0;

	pix_clk = pix_clk_mhz * 1000000;
	lcdc_set_ids_eitf_clk(pix_clk_mhz);

	if (iftype == SRGB_IF_RGB)
		col = 3;
	else
		col = 4;

#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
	//div = pix_clk / (dtd->mPixelClock * 1000 * col);
	//if (div < 1)
	//	div = 1;
	div = 2;
#else
	div =2;
#endif

	val = ids_readword(idsx, LCDCON1);
	val &= ~(0x3ff << LCDCON1_CLKVAL);
	val |= ((div - 1) << LCDCON1_CLKVAL);
	dss_dbg("serial clock is %d, val is 0x%x\n", div, val);
	ids_writeword(idsx, LCDCON1, val);

}

static void lcdc_set_serial_rgb_if(int idsx, dtd_t* dtd, int iftype, int enable)
{
	int val, col;

	dss_dbg("iftype = %d, enable = %d\n", iftype, enable);
	val =  ids_readword(idsx, LCDCON5);
	dss_dbg("lcdc set srgb LCDCON5	is 0x%x\n", val);
	val |= (((iftype& 0x3) << LCDCON5_SRGB_TYPE) | ((enable & 0x1) << LCDCON5_SRGB_ENABLE));
	ids_writeword(idsx, LCDCON5, val);

	dss_dbg("lcdc set srgb LCDCON5  is 0x%x\n", val);

	if (enable) {
		if (iftype == SRGB_IF_RGB)
			col = 3;
		else
			col = 4;
		lcdc_set_screen_size(idsx, col * dtd->mHActive, dtd->mVActive);
	}
}

static void lcdc_set_parallel_rgb_clk(int idsx, int vic, int init )
{
	int clkdiv, val;
#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
	/* Set HDMI/CVBS clk divider */
	if (idsx == 0)	// Currently CVBS only
		clkdiv = 2;	// 27MHz
	else if (idsx == 1) {	// HDMI
		if (vic == 3 || vic == 18 || vic == 7 || vic == 2002)
			clkdiv = 8;
		else if (vic == 4 || vic == 32 || vic == 33 || vic == 34)	// 720P@60Hz, 1080P@24,25,30Hz
			clkdiv = 4;	// 74.25MHz
		else if (vic == 16 || vic == 31)	// 1080P@60,50Hz
			clkdiv = 2;	// 148.5MHz
		else
			dss_err("idsx %d, VIC %d currently have no related clkdiv settings.\n", idsx, vic);
	}
	else
		dss_err("Invalid idsx %d\n", idsx);

	if (clkdiv < 1)
		clkdiv = 1;
#else
	/* For FPGA */
	clkdiv = 1;
#endif
	dss_dbg("IDS%d EITF clk-divder = %d\n", idsx, clkdiv);

	val = ((clkdiv-1) << LCDCON1_CLKVAL) | (0<<LCDCON1_ENVID);
	if ( !core.ubootlogo_status || init)
		ids_writeword(idsx, LCDCON1, val);

#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
	if ( !core.ubootlogo_status || init)
		set_pixel_clk(idsx);
#else
	lcdc_set_ids_eitf_clk(40);/* Note: set eitf 40MHz*/
#endif
}

static void lcdc_set_lcdcon(int idsx, dtd_t dtd)
{
	unsigned int val, vbpd, hbpd;

	vbpd = dtd.mVBlanking - dtd.mVSyncOffset - dtd.mVSyncPulseWidth;
	hbpd = dtd.mHBlanking - dtd.mHSyncOffset - dtd.mHSyncPulseWidth;
	dss_info("VFPD = %d, VBPD = %d, VPD = %d, "
		"HFPD = %d, HPBD = %d, HPD = %d\n",
		dtd.mVSyncOffset, vbpd, dtd.mVSyncPulseWidth,
		dtd.mHSyncOffset, hbpd,dtd.mHSyncPulseWidth);

	val = ((vbpd-1) << LCDCON2_VBPD) | ((dtd.mVSyncOffset-1) << LCDCON2_VFPD);
	ids_writeword(idsx, LCDCON2, val);

	val = ((dtd.mVSyncPulseWidth-1) << LCDCON3_VSPW) |
		((dtd.mHSyncPulseWidth-1) << LCDCON3_HSPW);
	ids_writeword(idsx, LCDCON3, val);

	val = ((hbpd-1) << LCDCON4_HBPD) | ((dtd.mHSyncOffset-1) << LCDCON4_HFPD);
	ids_writeword(idsx, LCDCON4, val);

	dss_info("mVclkInverse = %d, mHSyncPolarity = %d, mVSyncPolarity = %d\n",
		dtd.mVclkInverse, dtd.mHSyncPolarity, dtd.mVSyncPolarity);

	/*Note: why set ~value with vclk, hsync, vsync ? */
	val = (core.rgb_order << LCDCON5_RGBORDER) | (TFT_LCD_DISPLAY << LCDCON5_DSPTYPE) |
		((!dtd.mVclkInverse) << LCDCON5_INVVCLK) |
		((!dtd.mHSyncPolarity) << LCDCON5_INVVLINE) |
		((!dtd.mVSyncPolarity) << LCDCON5_INVVFRAME) | (0 << LCDCON5_INVVD) |
		(0 << LCDCON5_INVVDEN);

	ids_writeword(idsx, LCDCON5, val);

	switch(core.lcd_interface) {
		case DSS_INTERFACE_LCDC:
			if (dtd.mInterlaced == 1)
				val = ((dtd.mVActive -1) << LCDCON6_LINEVAL) |
					((dtd.mHActive*4 -1) << LCDCON6_HOZVAL);
			else
				val = ((dtd.mVActive -1) << LCDCON6_LINEVAL) |
					((dtd.mHActive -1) << LCDCON6_HOZVAL);
			ids_writeword(idsx, LCDCON6, val);
			break;

		case DSS_INTERFACE_SRGB:
			lcdc_set_serial_rgb_if( idsx, &dtd, core.srgb_iftype, DSS_ENABLE);
			break;

		default:
			dss_err("can't support interface %d\n", core.lcd_interface);
			break;
	}
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	static int init = 0;
	dtd_t dtd;
	int ret;

	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);

	ret = vic_fill(&dtd, vic, 60000); /*Nore: It is lcd.vic */
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	// Temp
	vic_fill(&gdtd[idsx], vic, 60000);

	lcdc_set_lcdcon( idsx, dtd);

	/* LCD VCLKFSR Strategy */

	/* ids_write(idsx, LCDVCLKFSR, 24, 4, PRE_LCDVCLKFSR_CDOWN); */
	/* ids_write(idsx, LCDVCLKFSR, 16, 6, PRE_LCDVCLKFSR_RFRM_NUM); */

	/*Enable VCLKFSR Strategy*/
	ids_write(idsx, LCDVCLKFSR, 24, 4, 0x0);
	ids_write(idsx, LCDVCLKFSR, 16, 6, 0x1);

	switch(core.lcd_interface ) {
		case DSS_INTERFACE_LCDC:
			lcdc_set_parallel_rgb_clk(idsx, vic, init);
			break;

		case DSS_INTERFACE_SRGB:
#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
			lcdc_set_serial_rgb_clk(idsx, &dtd, core.srgb_iftype, 54); /*Note: pix clk: 54Mhz*/
#else
			lcdc_set_serial_rgb_clk(idsx, &dtd, core.srgb_iftype, 40); /*Note: pix clk: 40Mhz in FPGA*/
#endif
			break;

		case DSS_INTERFACE_DSI:
		case DSS_INTERFACE_I80:
		case DSS_INTERFACE_HDMI:
			dss_err("can't support interface %d\n", core.lcd_interface);
			break;
		default:
			lcdc_set_parallel_rgb_clk(idsx, vic, init);
			break;
	}

	/* IFTYPE: RGB */
	ids_write(idsx, OVCDCR, OVCDCR_IfType, 1, 0x0);

	/* Enable LCDC controller */
	if ( !core.ubootlogo_status || init) {
		dss_dbg (" ####### Enable LCDC controller #######\n");
		ids_write(idsx, LCDCON1 , LCDCON1_ENVID , 1, 1);
	}

	/* Ensure the signal is stable before enabling HDMI or cs8556 */
	msleep(10);	/* Tune if necessary */
	init = 1;

	return 0;
}

static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	if (core.lcd_interface == DSS_INTERFACE_TVIF) {
		dss_dbg("In tvif mode, lcdc don't init\n");
		return 0;
	}

	if (core.lcd_interface == DSS_INTERFACE_LCDC) {
		if(imapx_pad_init(("rgb0")) == -1 ){
			dss_err("pad config RGB%d failed.\n", idsx);
			goto fail;
		}
	} else if (core.lcd_interface == DSS_INTERFACE_SRGB) {
		if(imapx_pad_init(("srgb")) == -1 ){
			dss_err("pad config SRGB%d failed.\n", idsx);
			goto fail;
		}
	} else {
		dss_err("ids can not support interface :%d\n", core.lcd_interface);
		goto fail;
	}

	ids_write(idsx, LCDVCLKFSR, 24, 4, cdown[idsx]);
	ids_write(idsx, LCDVCLKFSR, 16, 6, rfrm_num[idsx]);

	/* IFTYPE: RGB */
	ids_write(idsx, OVCDCR, OVCDCR_IfType, 1, 0x0);
	/* Clear mask */
	ids_writeword(idsx, IDSINTMSK, 0);
fail:
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display lcd controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
