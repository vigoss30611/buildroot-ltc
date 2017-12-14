/*
 * tvif.c - display tvif controller driver
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
#include <mach/pad.h>

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[controller]"
#define DSS_SUB2	"[tvif]"

/* Preset strategy about LCDVCLKFSR */
#define PRE_LCDVCLKFSR_CDOWN		0xF		// 0xF: Disable
#define PRE_LCDVCLKFSR_RFRM_NUM		0x1

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);

static struct module_attr tvif_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node tvif_module = {
	.name = "tvif",
	.attributes = tvif_attr,
	.idsx = 0,
	.init = dss_init_module,
};

static dtd_t gdtd[2];
static  int cdown[2] = {PRE_LCDVCLKFSR_CDOWN, PRE_LCDVCLKFSR_CDOWN};
static int rfrm_num[2] = {PRE_LCDVCLKFSR_RFRM_NUM, PRE_LCDVCLKFSR_CDOWN};

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	/* TVIF enable */
	ids_write(idsx, IMAP_TVICR, 31, 1, 0x1);

	/* TVIF clock enable */
	ids_write(idsx, IMAP_TVCCR, 31, 1, 1);  

	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	int clkdiv = 2;
	unsigned short vactive;
	unsigned int reg_temp[16];
	int ret;
	struct clk * ids0_eitf = NULL;
	struct clk * ids1_eitf = NULL;
	signed int coefficient[3][3] = {{66, 129, 25}, 
							{-38, -74, 112}, 
							{112, -94, -18}};
	dtd_t dtd;
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);

	if (idsx == 0) {
		ids0_eitf = clk_get_sys(IMAP_IDS0_EITF_CLK, IMAP_IDS0_PARENT_CLK);
		clk_set_rate(ids0_eitf, 54000000);
	} else {
		ids1_eitf = clk_get_sys(IMAP_IDS1_EITF_CLK,IMAP_IDS1_PARENT_CLK);
		clk_set_rate(ids1_eitf, 297000000);
	}

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	// Temp
	vic_fill(&gdtd[idsx], vic, 60000);

	/* DSTYPE: TV display */
	ids_write(idsx, LCDCON5, LCDCON5_DSPTYPE, 2, 0x2);

	/* IFTYPE: ITU-R BT601/BT656 */
	ids_write(idsx, OVCDCR, OVCDCR_IfType, 1, 0x1);
	/* Full VActive */
	if (dtd.mInterlaced)
		vactive = dtd.mVActive * 2; // Interlace
	else
		vactive = dtd.mVActive; // Progressive

	/* Set HDMI/CVBS clk divider */
	if (idsx == 0)	// Currently CVBS only
		clkdiv = 2;	// 27MHz
	else if (idsx == 1) {	// HDMI
		if (vic == 3 || vic == 18)
			clkdiv = 11;
		else if (vic == 4 || vic == 32 || vic == 33 || vic == 34)	// 720P@60Hz, 1080P@24,25,30Hz
			clkdiv = 4;	// 74.25MHz
		else if (vic == 16 || vic == 31)	// 1080P@60,50Hz
			clkdiv = 2;	// 148.5MHz
		else
			dss_err("idsx %d, VIC %d currently have no related clkdiv settings.\n", idsx, vic);
	} else
		dss_err("Invalid idsx %d\n", idsx);

	dss_info("tvif clkdiv = %d\n", clkdiv);
	reg_temp[0] = (1 << 31) |		/* Clock_enable */
			(0 << 11) |			/* TV_PCLK_mode */
			(1 << 9) |			/* Inv_clock */
			(1 << 8) |			/* clock_sel */
			((clkdiv-1) << 0);		/* Clock_div */

	if (idsx == 0) {	// BT656
		dss_info("TVIF-BT656-%s\n", vic ==3?"NTSC":(vic ==18?"PAL":"Invalid"));

		/* TVIF: Interlace */
		ids_write(idsx, OVCDCR, OVCDCR_Interlace, 1, 1);

		if (core.bt656_pad_mode)
			ids_write(idsx, LCDCON5, LCDCON5_BT656_MODE, 1, 1); /*set bt656 pad mode 1 rgb_da(8 ~ 16) in evb_va board*/
		else
			ids_write(idsx, LCDCON5, LCDCON5_BT656_MODE, 1, 0); /*set bt656 pad mode 0 rgb_da(0 ~ 7) in evb_va board*/

		/* RGB->YCC convert matrix coefficient */
		ids_writeword(idsx, IMAP_TVCOEF11, (u32)coefficient[0][0]);
		ids_writeword(idsx, IMAP_TVCOEF12, (u32)coefficient[0][1]);
		ids_writeword(idsx, IMAP_TVCOEF13, (u32)coefficient[0][2]);
		ids_writeword(idsx, IMAP_TVCOEF21, (u32)coefficient[1][0]);
		ids_writeword(idsx, IMAP_TVCOEF22, (u32)coefficient[1][1]);
		ids_writeword(idsx, IMAP_TVCOEF23, (u32)coefficient[1][2]);
		ids_writeword(idsx, IMAP_TVCOEF31, (u32)coefficient[2][0]);
		ids_writeword(idsx, IMAP_TVCOEF32, (u32)coefficient[2][1]);
		ids_writeword(idsx, IMAP_TVCOEF33, (u32)coefficient[2][2]);

		reg_temp[1] = (0 << 31) |			/* tvif_enable */
					(0 << 30) |			/* ITU601_656n */		//0: ITU 656 1: ITU601
					(0 << 29) |			/* Bit16ofITU60 */
					(0 << 28) |			/* Direct_data */		//0: ITU 656 & ITU601
					(0 << 18) |			/* Bitswap */
					(0 << 16) |			/* Data_order */		//@Must be zero  // modified by shuyu
					(0 << 13) |			/* Inv_vsync */			//@@@
					(0 << 12) |			/* Inv_hsync */			//@@@
					(0 << 11) |			/* Inv_href */
					(0 << 10) |			/* Inv_field */
					(1 << 0);				/* Begin_with_EAV */	//@@@

		reg_temp[2] = (0 << 31) |			/* Matrix_mode */
					(0 << 30) |			/* Passby */			//@@@
					(0 << 29) |			/* Inv_MSB_in */
					(0 << 28) |			/* Inv_MSB_out */
					(0 << 8) |			/* Matrix_oft_b */
					(16 << 0);			/* Matrix_oft_a */		//@@@

		if (vic == 3) {		// NTSC
			reg_temp[3] = 19;				/* UBA1_LEN 16 */
			reg_temp[4] = 240;				/* UNBA_LEN 244 */
			reg_temp[5] = 3;				/* UNBA2_LEN 2 */
			reg_temp[6] = 20;				/* LBA1_LEN 17 */
			reg_temp[7] = 240;				/* LNBA_LEN 243 */
			reg_temp[8] = 3;				/* LBA2_LEN 4 */
			reg_temp[9] = 268;				/* BLANK_LEN */
			reg_temp[10] = 1440;			/* VIDEO_LEN */
			reg_temp[11] = (0 << 30) |		/* Hsync_VB1_ctrl */
						(32 << 16) |		/* Hsync_delay */
						(124 << 0);		/* Hsync_extend */
			reg_temp[12] = (32 << 16) |		/* Vsync_delay_upper */
						(5148);			/* Vsync_extend_upper */
			reg_temp[13] = (890 << 16) |	/* Vsync_delay_lower */
						(5148);			/* Vsync_extend_lower */
			reg_temp[14] = 719;			/* DISP_XSIZE */
			reg_temp[15] = 479;			/* DISP_YSIZE */
		}
		else if (vic == 18) {	// PAL
			reg_temp[3] = 22;				/* UBA1_LEN */
			reg_temp[4] = 288;				/* UNBA_LEN */
			reg_temp[5] = 2;				/* UNBA2_LEN */
			reg_temp[6] = 23;				/* LBA1_LEN */
			reg_temp[7] = 288;				/* LNBA_LEN */
			reg_temp[8] = 2;				/* LBA2_LEN */
			reg_temp[9] = 280;				/* BLANK_LEN */
			reg_temp[10] = 1440;			/* VIDEO_LEN */
			reg_temp[11] = (0 << 30) |		/* Hsync_VB1_ctrl */
						(24 << 16) |		/* Hsync_delay */
						(126 << 0);		/* Hsync_extend */
			reg_temp[12] = (24 << 16) |		/* Vsync_delay_upper */
						(4320);			/* Vsync_extend_upper */
			reg_temp[13] = (888 << 16) |	/* Vsync_delay_lower */
						(4320);			/* Vsync_extend_lower */
			reg_temp[14] = 719;			/* DISP_XSIZE */
			reg_temp[15] = 575;			/* DISP_YSIZE */
		} else {
			dss_err("Only support NTSC and PAL.\n");
			return -1;
		}
	} else {	// BT601 with RGB data to simulate LCD timing
		dss_info("TVIF-BT601-VIC%d\n", vic);

		/* TVIF: Progressive */
		ids_write(idsx, OVCDCR, 0, 1,  dtd.mInterlaced);

		reg_temp[1] = (0 << 31) |			/* tvif_enable */
					(1 << 30) |			/* ITU601_656n */
					(0 << 29) |			/* Bit16ofITU60 */
					(1 << 28) |			/* Direct_data */
					(0 << 18) |			/* Bitswap */
					(3 << 16) |			/* Data_order */
					(dtd.mVSyncPolarity << 13) |	/* Inv_vsync */
					(dtd.mHSyncPolarity << 12) |	/* Inv_hsync */
					(0 << 11) |			/* Inv_href */
					(0 << 10) |			/* Inv_field */
					(0 << 0);				/* Begin_with_EAV */

		reg_temp[2] = (0 << 31) |			/* Matrix_mode */
					(1 << 30) |			/* Passby */
					(0 << 29) |			/* Inv_MSB_in */
					(0 << 28) |			/* Inv_MSB_out */
					(0 << 8) |			/* Matrix_oft_b */
					(0 << 0);				/* Matrix_oft_a */

		reg_temp[3] = (dtd.mVBlanking - dtd.mVSyncOffset);		/* UBA1_LEN */
		reg_temp[4] = dtd.mVActive;							/* UNBA_LEN */
		reg_temp[5] = dtd.mVSyncOffset;						/* UNBA2_LEN */
		if (dtd.mInterlaced)
			reg_temp[6] = reg_temp[3] + 1; /* LBA1_LEN */
		else
			reg_temp[6] = reg_temp[3];
		reg_temp[7] = reg_temp[4];							/* LNBA_LEN */
		reg_temp[8] = reg_temp[5];							/* LBA2_LEN */
		reg_temp[9] = dtd.mHBlanking - 8;					/* BLANK_LEN */
		reg_temp[10] = dtd.mHActive;						/* VIDEO_LEN */
		reg_temp[11] = (0 << 30) |							/* Hsync_VB1_ctrl */
					((dtd.mHSyncOffset - 4) << 16) |			/* Hsync_delay. -4 is design mistake */
					(dtd.mHSyncPulseWidth << 0);			/* Hsync_extend */
		reg_temp[12] = ((dtd.mHSyncOffset - 4) << 16) |		/* Vsync_delay_upper */
					((dtd.mHActive + dtd.mHBlanking) * dtd.mVSyncPulseWidth);	/* Vsync_extend_upper */
		if (dtd.mInterlaced)
			reg_temp[13] = ( (dtd.mHSyncOffset - 4)  + (dtd.mHActive + dtd.mHBlanking)/2) << 16 | /* Vsync_delay_lower */
				((dtd.mHActive + dtd.mHBlanking) * dtd.mVSyncPulseWidth); /* Vsync_extend_lower */
		else
			reg_temp[13] = reg_temp[12];

		reg_temp[14] = dtd.mHActive - 1;
		reg_temp[15] = vactive - 1;
	}

	ids_writeword(idsx, IMAP_TVCCR, reg_temp[0]);
	ids_writeword(idsx, IMAP_TVICR, reg_temp[1]);
	ids_writeword(idsx, IMAP_TVCMCR, reg_temp[2]);
	ids_writeword(idsx, IMAP_TVUBA1, reg_temp[3]);
	ids_writeword(idsx, IMAP_TVUNBA, reg_temp[4]);
	ids_writeword(idsx, IMAP_TVUBA2, reg_temp[5]);
	ids_writeword(idsx, IMAP_TVLBA1, reg_temp[6]);
	ids_writeword(idsx, IMAP_TVLNBA, reg_temp[7]);
	ids_writeword(idsx, IMAP_TVLBA2, reg_temp[8]);
	ids_writeword(idsx, IMAP_TVBLEN, reg_temp[9]);
	ids_writeword(idsx, IMAP_TVVLEN, reg_temp[10]);
	ids_writeword(idsx, IMAP_TVHSCR, reg_temp[11]);
	ids_writeword(idsx, IMAP_TVVSHCR, reg_temp[12]);
	ids_writeword(idsx, IMAP_TVVSLCR, reg_temp[13]);
	ids_writeword(idsx, IMAP_TVXSIZE, reg_temp[14]);
	ids_writeword(idsx, IMAP_TVYSIZE, reg_temp[15]);

	/* TVIF enable */
	ids_write(idsx, IMAP_TVICR, 31, 1, 0x1);

	/* Ensure the signal is stable before enabling HDMI or cs8556 */
	msleep(10);	// Tune if necessary

	return 0;
}

static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	if ((core.lcd_interface != DSS_INTERFACE_TVIF)) {
		dss_dbg("In lcd mode, tvif don't init\n");
		return 0;
	}

	if(imapx_pad_init(("bt656")) == -1 ){
		dss_err("pad config bt656 %d failed.\n", idsx);
		return -1;
	}

	ids_write(idsx, LCDVCLKFSR, 24, 4, cdown[idsx]);
	ids_write(idsx, LCDVCLKFSR, 16, 6, rfrm_num[idsx]);
	/* IFTYPE: ITU-R BT601/656*/
	ids_write(idsx, OVCDCR, OVCDCR_IfType, 1, 0x1);
	/* Clear mask */
	ids_writeword(idsx, IDSINTMSK, 0);
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display tvif controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
