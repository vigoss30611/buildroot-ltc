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
#include <controller.h>
#include <dss_item.h>
#include <emif.h>
#include <mach/pad.h> 

#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[controller]"
#define DSS_SUB2	"[lcdc]"



/* Preset strategy about LCDVCLKFSR */
#define PRE_LCDVCLKFSR_CDOWN		0xF		// 0xF: Disable
#define PRE_LCDVCLKFSR_RFRM_NUM		0x1


static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);
extern int board_type;



static struct module_attr lcdc_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node lcdc_module = {
	.name = "lcdc",
	.attributes = lcdc_attr,
	.idsx = 1,
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

	if (board_type != BOARD_I80)
		ids_write(idsx, LCDCON1 ,0 , 1, enable);
	return 0;
}

/* ids1 dedicated to hdmi, ids0 may connect lcd panel or cvbs */
static int set_pixel_clk(int idsx)
{
	int i, vic, ret, val;
	unsigned int clkdiv = 1;
	struct clk * ids0_eitf = NULL;
	struct clk * ids1_eitf = NULL;
	int clkdiff = 20000000;
	int reqclk, retclk;
	dss_trace("ids%d\n", idsx);

	vic = gdtd[idsx].mCode;
	dss_dbg("Pixel Clock %d\n", gdtd[idsx].mPixelClock);
	if (idsx == 0) {
		ids0_eitf = clk_get_sys("imap-ids0-eitf", "imap-ids0");
		clk_set_rate(ids0_eitf, 297000000);
	}
	else {
		ids1_eitf = clk_get_sys("imap-ids1-eitf", "imap-ids1");
	//	clk_set_rate(ids1_eitf, 297000000/*297000000*/); // changed by 
		clk_set_rate(ids1_eitf, 297000000); // changed by 
		// linyun.xiong @2015-09-04
	}
	
		
	if (gdtd[idsx].mPixelClock == 2700 || gdtd[idsx].mPixelClock == 2702)
		clkdiv = 11;
	else if (gdtd[idsx].mPixelClock == 7425 || gdtd[idsx].mPixelClock == 7417)
		clkdiv = 4;
	else if (gdtd[idsx].mPixelClock == 14850 || gdtd[idsx].mPixelClock == 14835)
		clkdiv = 2;
	else {
		/* Auto calculate best clkdiv */
		for (i = 2; i < 8; i++) {
			reqclk = gdtd[idsx].mPixelClock * 10 * i;	// KHz
			dss_dbg("reqclk = %d\n", reqclk);
			if (reqclk > 350000) {
				dss_dbg("reqclk exceed 350M\n");
				break;
			}
			ret = clk_set_rate(ids1_eitf, reqclk * 1000);
			if (ret < 0) {
				dss_err("clk_set_rate(ids0_eitf, %d) failed\n", reqclk);
				break;
			}
			retclk = clk_get_rate(ids1_eitf);
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
		ret = clk_set_rate(ids1_eitf, reqclk * 1000);
		if (ret < 0) {
			dss_err("clk_set_rate(ids0_eitf, %d) failed\n", reqclk);
			return -1;
		}
		ret = clk_get_rate(ids1_eitf);
		dss_dbg("ids1_eitf clk set to %d, actual %d, clkdiv %d\n", reqclk, ret/1000, clkdiv);
	}

	dss_dbg("ids%d, clkdiv = %d\n", idsx, clkdiv);
	val = ((clkdiv-1) << LCDCON1_CLKVAL) | (0 << LCDCON1_VMMODE) | (3 << LCDCON1_PNRMODE) |
		(12<<LCDCON1_STNBPP) | (0<<LCDCON1_ENVID);
	ids_writeword(idsx, LCDCON1, val);

	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	unsigned int val, vbpd, hbpd;
	int clkdiv = 0;
	int ret;
	int rgb_seq;
	static int init = 0;
	int	ubootLogoState = 1;//0-disabled;1-enabled                                      
	dtd_t dtd;

	dss_info("ids%d, vic %d\n", idsx, vic);
	if(item_exist("config.uboot.logo")){                                              
		unsigned char str[ITEM_MAX_LEN] = {0};                                            
		item_string(str,"config.uboot.logo",0);                                       
		if(strncmp(str,"0",1) == 0 )                                           
			ubootLogoState = 0;//disabled                                      
	}        

	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);
	
	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	// Temp
	vic_fill(&gdtd[idsx], vic, 60000);

	
	vbpd = dtd.mVBlanking - dtd.mVSyncOffset - dtd.mVSyncPulseWidth;
	hbpd = dtd.mHBlanking - dtd.mHSyncOffset - dtd.mHSyncPulseWidth;
	dss_dbg("VBPD = %d, HPBD = %d\n", vbpd, hbpd);

	val = ((vbpd-1) << LCDCON2_VBPD) | ((dtd.mVSyncOffset-1) << LCDCON2_VFPD);
	ids_writeword(idsx, LCDCON2, val);

	val = ((dtd.mVSyncPulseWidth-1) << LCDCON3_VSPW) | ((dtd.mHSyncPulseWidth-1) << LCDCON3_HSPW);
	ids_writeword(idsx, LCDCON3, val);

	val = ((hbpd-1) << LCDCON4_HBPD) | ((dtd.mHSyncOffset-1) << LCDCON4_HFPD);
	ids_writeword(idsx, LCDCON4, val);

	if (idsx == 0) {                                 
		if (item_exist(P_IDS0_RGBSEQ)) {             
			rgb_seq = item_integer(P_IDS0_RGBSEQ, 0);
		} else {                                     
			rgb_seq = 0x6;                           
		}                                            
	}                                                
	if (idsx == 1) {                                 
		if (item_exist(P_IDS1_RGBSEQ)) {             
			rgb_seq = item_integer(P_IDS1_RGBSEQ, 0);
		} else {                                     
			rgb_seq = 0x6;                           
		}                                            
	}                                                

	val = (rgb_seq << LCDCON5_RGBORDER) | (0 << LCDCON5_DSPTYPE) |
		((!dtd.mVclkInverse) << LCDCON5_INVVCLK) | ((!dtd.mHSyncPolarity) << LCDCON5_INVVLINE) |
		((!dtd.mVSyncPolarity) << LCDCON5_INVVFRAME) | (0 << LCDCON5_INVVD) |
		(0 << LCDCON5_INVVDEN) | (0 << LCDCON5_INVPWREN) |
		(1 << LCDCON5_PWREN);

	ids_writeword(idsx, LCDCON5, val);

	if (lcd_interface == DSS_INTERFACE_LCDC && dtd.mInterlaced == 1)
		val = ((dtd.mVActive -1) << LCDCON6_LINEVAL) | ((dtd.mHActive*4 -1) << LCDCON6_HOZVAL);
	else
		val = ((dtd.mVActive -1) << LCDCON6_LINEVAL) | ((dtd.mHActive -1) << LCDCON6_HOZVAL);
	ids_writeword(idsx, LCDCON6, val);

	/* LCD VCLKFSR Strategy */
	ids_write(idsx, LCDVCLKFSR, 24, 4, PRE_LCDVCLKFSR_CDOWN);
	ids_write(idsx, LCDVCLKFSR, 16, 6, PRE_LCDVCLKFSR_RFRM_NUM);

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


	// For FPGA
	//clkdiv = 2;
	if (clkdiv < 1)	
		clkdiv = 1;
	dss_dbg("IDS%d EITF clk-divder = %d\n", idsx, clkdiv);
	val = ((clkdiv-1) << LCDCON1_CLKVAL) | (0 << LCDCON1_VMMODE) | (3 << LCDCON1_PNRMODE) |
			(12<<LCDCON1_STNBPP) | (0<<LCDCON1_ENVID);
	if ( 0 == ubootLogoState || init != 0)
		ids_writeword(idsx, LCDCON1, val);

	/* IFTYPE: RGB */
	ids_write(idsx, OVCDCR, 1, 1, 0x0);

	if ( 0 == ubootLogoState || init != 0)
		set_pixel_clk(idsx);

	/* Enable LCDC controller */
	if ( (0 == ubootLogoState || init != 0) && (board_type != BOARD_I80))
		ids_write(idsx, LCDCON1 ,0 , 1, 1);

	/* Ensure the signal is stable before enabling HDMI or cs8556 */
	msleep(10);	// Tune if necessary
	init = 1;

	return 0;
}



static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	if(imapx_pad_init(("rgb0")) == -1 ){  
		dss_err("pad config IMAPX_RGB%d failed.\n", idsx);
		return -1;                                        
	}                                                     

	ids_write(idsx, LCDVCLKFSR, 24, 4, cdown[idsx]);
	ids_write(idsx, LCDVCLKFSR, 16, 6, rfrm_num[idsx]);
	/* IFTYPE: RGB */
	ids_write(idsx, OVCDCR, 1, 1, 0x0);
	/* Clear mask */
	ids_writeword(idsx, IDSINTMSK, 0);
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display lcd controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
