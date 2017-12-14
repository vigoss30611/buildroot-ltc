/*
 * osd_x15.c - x15 display osd driver
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
#include <linux/delay.h>
#include <dss_common.h>
#include <implementation.h>
#include <ids_access.h>
#include <controller.h>
#include <mach/items.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include "osd.h"

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[osd]"
#define DSS_SUB2	""

#define PRE_OVCDCR_ENREFETCH		0x1		// 0x1: Enable
#define PRE_OVCDCR_WAITTIME		0xF		// 0x0 ~ 0xF
#define HDMI_SCALER_RATIO	95

#define NO_PRESCALER

#define SCALER_BYPASS 1
#define SCALER_NO_BYPASS 0

/*Note: for OSD_Q3*/
static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int set_swap_context(int idsx, int *params, int num, unsigned int fb_addr, int win_id);
static int dss_init_module(int idsx);
int 		osd_n_enable(int idsx, int osd_num, int enable);


/* FALSE stand for optional attribute, and default disable */
static struct module_attr osd_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node osd_module = {
	.name = "osd",
	.attr_ctrl = module_attr_ctrl,
	.attributes = osd_attr,
	.idsx = 0,
	.init = dss_init_module,
};

/* Refetch strategy */
static unsigned int enrefetch[2] = {PRE_OVCDCR_ENREFETCH, PRE_OVCDCR_ENREFETCH};
static unsigned int waittime[2] = {PRE_OVCDCR_WAITTIME, PRE_OVCDCR_WAITTIME};
/*Note: for window 0*/
static dma_addr_t gdma_addr;
static int gframe_size;
/*Note: for window 1*/
static dma_addr_t gdma1_addr; 
static int gframe1_size;
static int logo_fbx = 0;
/*Note: current buffer index in window*/
static int fbx;
static int osd0_x1 = 0;
static int osd0_x2 = 0;
static int osd0_y1 = 0;
static int osd0_y2 = 0;
static int prescaler_virt_in_width = 0;
static int prescaler_virt_in_height = 0;
static int prescaler_win_in_width = 0;
static int prescaler_win_in_height = 0;
static int prescaler_left_top_x = 0;
static int prescaler_left_top_y = 0;
static int osd_win_id = 0;
static int fb_idx = 0;
static int in_format = NV12;
static int bpp = 32; /*ARGB8888*/
static unsigned int video_buff = 0;
static int YUV2RGB_NV12[] = {298, 516, 0, 298, -100, 1840, 298, 0, 409};
static int YUV2RGB_NV21[] = {298, 0, 409, 298, -100, 1840, 298, 516, 0};
static DEFINE_MUTEX(video_buffer_mutex);

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	//int OVCWxCR= OVCW0CR ;

	/*TODO: parse params form upper caller.*/

	dss_trace("ids%d, enable %d\n", idsx, enable);
	//ids_write(idsx, OVCWxCR, OVCWxCR_ENWIN, 1, enable);
	return 0;
}

int osd_swap(int idsx, int fbx, unsigned int fb_addr, int win_id)
{
	return set_swap_context(idsx, &fbx, 1, fb_addr, win_id);
}

int osd_dma_addr(int widx, dma_addr_t addr, int frame_size, int fbx)
{
	/*Note: OSD-win0 => gdma_addr and  OSD-win1 => gdma1_addr */
	if (widx == 0) {
		gdma_addr = addr;
		gframe_size = frame_size;
		logo_fbx = fbx;
	} else {
		gdma1_addr = addr;
		gframe1_size = frame_size;
	}
	return 0;
}

void osd_config_scaler(int idsx, unsigned int in_width, unsigned int in_height, 
		unsigned int out_width, unsigned int out_height)
{
	if (((in_width > out_width) && (in_height <= out_height)) ||
		((in_width <= out_width) && (in_height > out_height)) ||
		((in_width < out_width) && (in_height <= out_height)) ||
		((in_width <= out_width) && (in_height < out_height))) {

		/* Note: both zoom in&out or zoom in */
		ids_writeword(idsx, SCLAZOR, 2);
		ids_writeword(idsx, SCLAZIR, 0);
		ids_writeword(idsx, CSCLAR0, 0x1000);
		ids_writeword(idsx, CSCLAR1, 0x0ff6);
		ids_writeword(idsx, CSCLAR2, 0x0fda);
		ids_writeword(idsx, CSCLAR3, 0x0fab);

	} else if ((in_width >= out_width) && (in_height >= out_height)) {
		if (in_width/out_width >= 4 || in_height/out_height >= 4) {

			/* zoom out */
			ids_writeword(idsx, SCLAZOR, 1);
			ids_writeword(idsx, CSCLAR0, 0x0400);
			ids_writeword(idsx, CSCLAR1, 0x0400);
			ids_writeword(idsx, CSCLAR2, 0x0400);
			ids_writeword(idsx, CSCLAR3, 0x0400);
		} else {
			ids_writeword(idsx, SCLAZOR, 0);
		}
	}

}

static void osd_set_no_pre_out(int idsx, int window, int in_format, int out_format,
	int in_width, int in_height, int out_width, int out_height,
	int overlay_x1, int overlay_y1, int overlay_x2, int overlay_y2)
{
	u8* uv_addr = NULL;

	dss_trace("\n");
	if (!window) {
		/* Set Overlay0 */
		ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB
		
		ids_write(idsx, OVCW0CR, OVCWxCR_BPPMODE, 5, out_format);
		ids_writeword(idsx, OVCW0VSSR, out_width);
		ids_writeword(idsx, OVCW0PCAR, 
				((overlay_x1) << OVCWxPCAR_LeftTopX) | ((overlay_y1) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR,
				((overlay_x2 - 1) << OVCWxPCBR_RightBotX) | ((overlay_y2 - 1) << OVCWxPCBR_RightBotY)); 
		dss_dbg("ids%d, Y = 0x%X\n", idsx, (u32)gdma_addr);
		
		ids_writeword(idsx, OVCW0B0SAR, (u32)gdma_addr);

		dss_dbg("ids%d, Y  = 0x%X\n", idsx, (u32)gdma_addr);
		if (in_format == NV12 || in_format == NV21) {
			uv_addr = ((u8*)gdma_addr) + prescaler_virt_in_width * prescaler_virt_in_height;
			dss_dbg("ids%d, uv  = 0x%X\n", idsx, (u32)uv_addr);
			ids_writeword(idsx, OVCBRB0SAR, (u32)uv_addr);
		}

		ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, 0);

	} else {
		/* Set Overlay1 */
		ids_write(idsx, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB

		/* set osd output */
		ids_write(idsx, OVCW1CR, OVCWxCR_BPPMODE, 5, out_format);
		ids_writeword(idsx, OVCW1VSSR, out_width);
		ids_writeword(idsx, OVCW1PCAR, 
				((overlay_x1) << OVCWxPCAR_LeftTopX) | ((overlay_y1) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW1PCBR,
				((overlay_x2 - 1) << OVCWxPCBR_RightBotX) | ((overlay_y2 - 1) << OVCWxPCBR_RightBotY)); 

		dss_dbg("ids%d, window%d dma_addr[0] = 0x%X, \n", idsx, window,gdma1_addr);

		ids_writeword(idsx, OVCW1B0SAR, (u32)gdma1_addr);
		ids_writeword(idsx, OVCW1B1SAR, (u32)gdma1_addr + gframe1_size);
		if (in_format == NV12 || in_format == NV21) {
			uv_addr = ((u8*)gdma1_addr) + in_width * in_height;
			ids_writeword(idsx, OVCBRB0SAR, (u32)uv_addr);
		}
		ids_write(idsx, OVCW1CR, OVCWxCR_BUFSEL, 2, fb_idx);
	}

#if 0
	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);
	ids_writeword(idsx, OVCWPR, (out_width- 1) << OVCWPR_OSDWINX |
		(out_height- 1) << OVCWPR_OSDWINY);

	//ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, OVCDCR_SCALER_BEFORE_OSD);
	//ids_write(idsx, OVCPSCCR, 0, 1, 0); /* Note: disable prescaler module */
#endif
}

static void osd_set_out(int idsx, int window, int in_format, int out_format,
	int in_width, int in_height, int out_width, int out_height,
	int overlay_x1, int overlay_y1, int overlay_x2, int overlay_y2)
{
	u8* uv_addr = NULL;

	if (!window) {
		/* Set Overlay0 */
		ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB

		/* set osd output */
		ids_write(idsx, OVCW0CR, OVCWxCR_BPPMODE, 5, out_format);
		ids_writeword(idsx, OVCW0VSSR, out_width);
		ids_writeword(idsx, OVCW0PCAR, 
				((overlay_x1) << OVCWxPCAR_LeftTopX) | ((overlay_y1) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR,
				((overlay_x2 - 1) << OVCWxPCBR_RightBotX) | ((overlay_y2 - 1) << OVCWxPCBR_RightBotY)); 

		dss_dbg("ids%d, window %d Y  = 0x%X\n", idsx, window, (u32)gdma_addr);

		ids_writeword(idsx, OVCPSB0SAR, gdma_addr);
		if (in_format == NV12 || in_format == NV21) {
			uv_addr = ((u8*)gdma_addr) + prescaler_virt_in_width * prescaler_virt_in_height;
			dss_dbg("ids%d, uv  = 0x%X\n", idsx, (u32)uv_addr);
			ids_writeword(idsx, OVCPSCB0SAR, (u32)uv_addr);
		}
	} else {
		/* Set Overlay1 */
		ids_write(idsx, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB

		/* set osd output */
		ids_write(idsx, OVCW1CR, OVCWxCR_BPPMODE, 5, out_format);
		ids_writeword(idsx, OVCW1VSSR, out_width);
		ids_writeword(idsx, OVCW1PCAR, 
				((overlay_x1) << OVCWxPCAR_LeftTopX) | ((overlay_y1) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW1PCBR,
				((overlay_x2 - 1) << OVCWxPCBR_RightBotX) | ((overlay_y2 - 1) << OVCWxPCBR_RightBotY)); 

		dss_dbg("ids%d, window%d dma_addr[0] = 0x%X, \n", idsx, window, (u32)gdma1_addr);

		ids_writeword(idsx, OVCPSB0SAR, (u32)gdma1_addr);
		ids_writeword(idsx, OVCW1B1SAR, (u32)gdma1_addr + gframe1_size);
		if (in_format == NV12 || in_format == NV21) {
			uv_addr = ((u8*)gdma1_addr) + in_width * in_height;
			ids_writeword(idsx, OVCPSCB0SAR, (u32)uv_addr);
		}
	}

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);
	ids_writeword(idsx, OVCWPR, (out_width- 1) << OVCWPR_OSDWINX |
		(out_height- 1) << OVCWPR_OSDWINY);

	ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, OVCDCR_SCALER_BEFORE_OSD);

}

static void osd_set_yuv2rgb(int idsx, int window, int format)
{
	/*Note: set yuv to rgb*/
	ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
	if (!window)
		ids_write(idsx, OVCOMC, OVCOMC_CBCR_CH, 2, 0);
	else
		ids_write(idsx, OVCOMC, OVCOMC_CBCR_CH, 2, 1);

	ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
	ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);

	if (format == NV12) {
		ids_writeword(idsx, OVCOEF11, YUV2RGB_NV12[0]);
		ids_writeword(idsx, OVCOEF12, YUV2RGB_NV12[1]);
		ids_writeword(idsx, OVCOEF13, YUV2RGB_NV12[2]);
		ids_writeword(idsx, OVCOEF21, YUV2RGB_NV12[3]);
		ids_writeword(idsx, OVCOEF22, YUV2RGB_NV12[4]);
		ids_writeword(idsx, OVCOEF23, YUV2RGB_NV12[5]);
		ids_writeword(idsx, OVCOEF31, YUV2RGB_NV12[6]);
		ids_writeword(idsx, OVCOEF32, YUV2RGB_NV12[7]);
		ids_writeword(idsx, OVCOEF33, YUV2RGB_NV12[8]);
	} else if (format == NV21) {
		//TODO
		ids_writeword(idsx, OVCOEF11, YUV2RGB_NV21[0]);
		ids_writeword(idsx, OVCOEF12, YUV2RGB_NV21[1]);
		ids_writeword(idsx, OVCOEF13, YUV2RGB_NV21[2]);
		ids_writeword(idsx, OVCOEF21, YUV2RGB_NV21[3]);
		ids_writeword(idsx, OVCOEF22, YUV2RGB_NV21[4]);
		ids_writeword(idsx, OVCOEF23, YUV2RGB_NV21[5]);
		ids_writeword(idsx, OVCOEF31, YUV2RGB_NV21[6]);
		ids_writeword(idsx, OVCOEF32, YUV2RGB_NV21[7]);
		ids_writeword(idsx, OVCOEF33, YUV2RGB_NV21[8]);
	}

	//if (core.lcd_interface == DSS_INTERFACE_TVIF)
	//	ids_write(idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 1); /* ARGB-> ABGR */
}

static void osd_set_prescaler(int idsx, int window, int format,
	int in_width, int in_height, int out_width, int out_height, int bypass)
{
	u32 scale_ratio_w, scale_ratio_h;

	if (!window)
		ids_write(idsx, OVCPSCCR, OVCPSCCR_WINSEL, 2, 0);
	else
		ids_write(idsx, OVCPSCCR, OVCPSCCR_WINSEL, 2, 1);

	ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, format);
	osd_config_scaler(0, in_width, in_height, out_width, out_height);

	ids_writeword(idsx, OVCPSCPCR, (in_height-1) << 16 | (in_width-1));
	ids_write(idsx, OVCPSVSSR, OVCPSVSSR_VW_WIDTH,  OVCPSVSSR_VW_WIDTH_SZ, prescaler_virt_in_width);

	if(core.lcd_interface == DSS_INTERFACE_TVIF)//interlace mode
	{
		ids_writeword(idsx, SCLIFSR, in_height/2 << 16 | in_width);
		ids_writeword(idsx, SCLOFSR, out_height/2 << 16 | out_width);
	}else{
		ids_writeword(idsx, SCLIFSR, in_height << 16 | in_width);
		ids_writeword(idsx, SCLOFSR, out_height<< 16 | out_width);
	}

	scale_ratio_w= in_width*8192/out_width;
	scale_ratio_h= in_height*8192/out_height;

	ids_writeword(idsx, RHSCLR, scale_ratio_w & 0x1ffff);
	ids_writeword(idsx, RVSCLR, scale_ratio_h & 0x1ffff);

	ids_write(idsx, SCLCR, SCLCR_BYPASS, 1, bypass);
	ids_write(idsx, SCLCR, SCLCR_ENABLE, 1, 1);

	ids_write(idsx, OVCPSCCR, OVCPSCCR_EN, 1, 1);
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
}

static int osd_reinit_nv(int idsx,int window, int vic, int format)
{
	int ret;
	dtd_t dtd, main_dtd; 
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;

	int in_width, in_height;
	int out_width, out_height;
	int bpp_mode;

	dss_trace("ids%d, vic %d\n", idsx, vic);
	printk(KERN_INFO "osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,core.main_VIC);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	ret = vic_fill(&main_dtd, core.main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", core.main_VIC, ret);
		return ret;
	}

	if (prescaler_win_in_width && prescaler_win_in_height) {
		in_width = prescaler_win_in_width;
		in_height = prescaler_win_in_height;
	} else {
		in_width = main_dtd.mHActive;
		in_height = main_dtd.mVActive;
	}

	if (video_buff) {
		if (!window)
			gdma_addr = video_buff;
		else
			gdma1_addr = video_buff;
	}

	out_width  = dtd.mHActive & (~0x3);
	out_height  = dtd.mVActive & (~0x3);

	if ((dtd.mHActive - out_width) % 4  || (dtd.mVActive - out_height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", out_width, out_height);
		return -1;
	}

	dss_dbg("out_width = %d, out_height = %d, in_width = %d, in_height = %d\n",
		out_width, out_height, in_width, in_height);

	overlay_x1 = osd0_x1;
	overlay_y1 = osd0_y1;
	overlay_x2 = osd0_x2;
	overlay_y2 = osd0_y2;
	bpp_mode = OSD_IMAGE_RGB_BPP24_888;
	//bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;

#ifndef TEST_NO_PRESCALER_NV
	osd_set_out(idsx, window, format, bpp_mode,
		in_width, in_height, out_width, out_height,
		overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	osd_set_yuv2rgb(idsx, window, format);

	/*Note: input frame is YUV420SP(17) */
	osd_set_prescaler(idsx, window, OSD_IMAGE_YUV_420SP,
		in_width,  in_height, out_width, out_height, SCALER_NO_BYPASS);
#else
	osd_set_no_pre_out(idsx, window, format, OSD_IMAGE_YUV_420SP,
		in_width, in_height, out_width, out_height, 
		overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);
	ids_writeword(idsx, OVCWPR, (out_width- 1) << OVCWPR_OSDWINX |
		(out_height- 1) << OVCWPR_OSDWINY);

	ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, OVCDCR_SCALER_BEFORE_OSD);
	ids_write(idsx, OVCPSCCR, 0, 1, 0); /* Note: disable prescaler module */

	osd_set_yuv2rgb(idsx, window, format);

	ids_write(idsx, SCLCR, SCLCR_BYPASS, 1, SCALER_BYPASS);
	ids_write(idsx, SCLCR, SCLCR_ENABLE, 1,0);
	ids_write(idsx, OVCPSCCR, OVCPSCCR_EN, 1, 0);
#endif

	if (!window) {
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	return 0;
}

static int osd_reinit_rgb(int idsx,int window, int vic, int format)
{
	int ret, bpp_mode;
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;

	int in_width, in_height;
	int out_width, out_height;

	dtd_t dtd, main_dtd;

	dss_trace("ids%d, vic %d\n", idsx, vic);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	ret = vic_fill(&main_dtd, core.main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", core.main_VIC, ret);
		return ret;
	}

	if (prescaler_win_in_width && prescaler_win_in_height) {
		in_width = prescaler_win_in_width;
		in_height = prescaler_win_in_height;
	} else {
		in_width = main_dtd.mHActive;
		in_height = main_dtd.mVActive;
	}

	if (video_buff) {
		if (!window)
			gdma_addr = video_buff;
		else
			gdma1_addr = video_buff;
	}

	out_width = dtd.mHActive & (~0x3);
	out_height = dtd.mVActive & (~0x3);
	if ((dtd.mHActive - out_width) % 4  || (dtd.mVActive - out_height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", 
			out_width, out_height);
		return -1;
	}

	dss_dbg("out_width = %d, out_height = %d, in_width = %d, in_height = %d\n",
		out_width, out_height, in_width, in_height);

	overlay_x1 = osd0_x1;
	overlay_y1 = osd0_y1;
	overlay_x2 = osd0_x2;
	overlay_y2 = osd0_y2;
	//bpp_mode = OSD_IMAGE_RGB_BPP24_888;
	//bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;

	if (format == ARGB8888)
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;
	else if (format == _IDS_RGB888)
		bpp_mode = OSD_IMAGE_RGB_BPP24_888;
	else
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;

	osd_set_out(idsx, window, format, bpp_mode,
		in_width, in_height, out_width, out_height,
		overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 0);
	/*Note: input frame is ARGB8888(16) */
	osd_set_prescaler(idsx, window, bpp_mode,
		in_width, in_height, out_width,  out_height, SCALER_NO_BYPASS);

	if (!window) {
		/* Enabe Overlay 0 */
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		/* Enabe Overlay 1 */
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	
	return 0;
}

static int is_valid_size(int iwidth, int iheight)
{
	int n = 0;
	int t  = 0;

	if (iwidth%8)
		return -1;

	dss_dbg("# iwidth = %d, iheight = %d\n", iwidth, iheight);
	for(n = 1 ; n <=  iheight; n++) {
		t = (n *iwidth / 8) % 256;
		if (t > 250)
			dss_dbg("# %d\n", t);
		if((n *iwidth / 8) % 256 == 255) {
			return -1;
		}
	}
	return 0;
}

static int osd_resolution_filter(u32 width, u32 height, u32* rwidth)
{
	int ret = 0, i = 0;
	u32  width_t = 0;
	ret = is_valid_size(width, height);
	if (!ret) {
		dss_dbg("#1 actual width = %d\n", width);
		*rwidth = width;
		return 0;
	}

	width = (width/8)*8;
	dss_dbg("## width = %d\n", (width/8)*8);
	for (i = 0; i < 4; i++) {
		width_t = (width/8)*8 - i*8;
		ret = is_valid_size(width_t, height);
		dss_dbg("# width = %d\n", width_t);
		if (!ret) break;
	}

	if (i == 4){
		dss_err("don't get resolution , it is a bug!\n");
		return -1;
	}

	dss_dbg("#2 actual width = %d\n", width_t);
	*rwidth = width_t;
	return 0;
}

int scaler_config_size(unsigned int window, unsigned int in_width, unsigned int in_height,
		unsigned int virt_in_width, unsigned int virt_in_height,
		unsigned int out_width,unsigned int out_height,
		unsigned int osd0_pos_x, unsigned int osd0_pos_y,
		unsigned int scaler_left_x, unsigned int scaler_top_y,
		dma_addr_t scaler_buff, int format, int interlaced)
{
	int i = 0, ret = 0;
	u32 r_in_width = 0,r_virt_in_width = 0;
	if (scaler_buff)
		video_buff = scaler_buff;

	if (format == NV12 || format == NV21) {
		ret = osd_resolution_filter(in_width, in_height, &r_in_width);
		if (ret < 0)
			return -1;

		ret = osd_resolution_filter(in_width, in_height, &r_virt_in_width);
		if (ret < 0)
			return -1;
	} else {
		r_virt_in_width = virt_in_width;
		r_in_width = in_width;
	}

	dss_info("# actual width = %d, actual virt width = %d\n", r_in_width, r_virt_in_width);
	prescaler_virt_in_width = virt_in_width;
	prescaler_virt_in_height  = virt_in_height;
	prescaler_win_in_width = r_in_width;
	prescaler_win_in_height = in_height;
	prescaler_left_top_x = scaler_left_x & ~0x1;
	prescaler_left_top_y = scaler_top_y & ~0x1;
	osd0_x1 = osd0_pos_x;
	osd0_x2 = osd0_pos_x + out_width;
	osd0_y1 = osd0_pos_y;
	osd0_y2 = osd0_pos_y + out_height;

	in_format = format;

	if (format == NV12 || format == NV21)
		osd_reinit_nv(0, window,core.lcd_vic, format);
	else
		osd_reinit_rgb(0, window,core.lcd_vic, format);

	if (core.lcd_interface == DSS_INTERFACE_LCDC ||core.lcd_interface == DSS_INTERFACE_SRGB ) {
		wait_vsync_timeout(core.idsx, IRQ_LCDINT, TIME_SWAP_INT * core.wait_irq_timeout);
	} else if (core.lcd_interface == DSS_INTERFACE_TVIF) {
		do {
			wait_vsync_timeout(core.idsx, IRQ_TVINT, TIME_SWAP_INT*core.wait_irq_timeout);
		} while((!((ids_readword(1, OVCW1PCAR) >> 31) & 0x1)) &&( i++ < 3));
		/*wait top field and bottom field to complete.*/
	} else {
		dss_err("ids can not support interface :%d\n", core.lcd_interface);
	}
	return 0;
}

int swap_buffer(u32 phy_addr)
{
	if (fbx == 0)
		ids_writeword(0, OVCW0B0SAR, phy_addr);
	else
		ids_writeword(0, OVCW0B1SAR, phy_addr);

	ids_write(0, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(0, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	/*Note: need to mutex */
	fbx = fbx?0:1;

	wait_swap_timeout(1, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}

static int __get_osd_ctl_reg(int win)
{
	return (win) ? (OVCW1CR):(OVCW0CR);
}

static int __get_osd_buf_addr(int win, int sel, bool cbcr)
{
	if (!cbcr) {
		switch (sel){
		case 0: 
			return ((!win) ? (OVCW0B0SAR):(OVCW1B0SAR));
		case 1:
			return  ((!win) ? (OVCW0B1SAR):(OVCW1B1SAR));
		case 2:
			return ((!win) ? (OVCW0B2SAR):(OVCW1B2SAR));
		case 3:
			return ((!win) ? (OVCW0B3SAR):(OVCW1B3SAR));
		default:
			return ((!win) ? (OVCW0B0SAR):(OVCW1B0SAR));
		}
	} else {
		switch (sel){
		case 0: 
			return OVCBRB0SAR;
		case 1:
			return  OVCBRB1SAR;
		case 2:
			return OVCBRB2SAR;
		case 3:
			return OVCBRB3SAR;
		default:
			return OVCBRB0SAR;
		}
	}

	return OVCW0B0SAR;
}
int osd_set_buffer_addr(u32 idsx,u32 win, u32 phy_addr, u32 bsel)
{
	u32 buf_addr = __get_osd_buf_addr(win, bsel, false);

	dss_trace("buff reg addr: 0x%x", buf_addr);
	ids_writeword(idsx, buf_addr, phy_addr);
	ids_write(idsx, __get_osd_ctl_reg(win), OVCWxCR_BUFSEL, 2, bsel);

	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	wait_swap_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}

void osd_set_parameter(void)
{
#if defined(TEST_NO_PRESCALER_RGB)
	return ;
#else
	u8* pb = NULL;
	u8* pPsc = NULL;
	/*Note:problem: video_buff used*/
	dtd_t main_dtd;

	/*Note: get prescaler module parm */
	vic_fill(&main_dtd, core.main_VIC, 60000);

	if (!video_buff)
		pb = (u8*)gdma_addr; /*Note: display test image */
	else
		pb = (u8*)video_buff; /*Note: after setting format and w & h */

	/* osd_win0 handle NV12 and NV21 format video and osd win1 handle rgb888 format video. */
	if (osd_win_id == 0) {
		ids_writeword(0, OVCPSB0SAR, (u32)pb); /*Note: Y or rgb*/
		dss_trace("Y :0x%x , h * w: %d * %d\n", 
				(u32)pb, prescaler_virt_in_height, prescaler_virt_in_width);
		/*Note: used to nv format*/
		if (in_format == NV21 || in_format == NV12) {
			/*Note: used to nv format*/
			pPsc = pb + (prescaler_left_top_x + prescaler_left_top_y * prescaler_virt_in_width);
			ids_writeword(0, OVCPSB0SAR, (u32)pPsc); /*Note: Y*/
			/*Note: CbCr*/
			if (prescaler_virt_in_height && prescaler_virt_in_width) {
				pPsc = pb + prescaler_virt_in_width * prescaler_virt_in_height
					+ (prescaler_left_top_x + (prescaler_left_top_y * prescaler_virt_in_width >> 1));
				ids_writeword(0, OVCPSCB0SAR, (u32)pPsc);
			} else {
				pPsc = pb + main_dtd.mHActive * main_dtd.mVActive;
				ids_writeword(0, OVCPSCB0SAR, (u32)pPsc);
			}

			dss_trace("CbCr : 0x%x\n", (u32)pPsc);
		} 
	}else {
		ids_write(1, OVCW1CR, OVCWxCR_BUFSEL, 2, fb_idx);
	}
#endif
}
EXPORT_SYMBOL(osd_set_parameter);

static int set_swap_context(int idsx, int *params, int num, 
		unsigned int fb_addr, int win_id)
{
	int i = 0;
	int fbx = *params;

	mutex_lock(&video_buffer_mutex);
	if (fb_addr)
		video_buff = fb_addr;
	mutex_unlock(&video_buffer_mutex);

	dss_dbg("video buffer addr = 0x%x\n", video_buff);
	barrier();

	osd_win_id= win_id;
	if (osd_win_id == 1) {
		fb_idx = fbx;
		dss_trace("set osd1 fb idx %d\n", fbx);
	}

	if (core.blanking == 1) {
		return 0;
	}

	if (core.lcd_interface == DSS_INTERFACE_LCDC ||
		core.lcd_interface == DSS_INTERFACE_SRGB ) {
		wait_vsync_timeout(core.idsx, IRQ_LCDINT,
			TIME_SWAP_INT * core.wait_irq_timeout);
	} else {
		do {
			wait_vsync_timeout(core.idsx, IRQ_TVINT,
				TIME_SWAP_INT * core.wait_irq_timeout);
		} while((!((ids_readword(1, OVCW1PCAR) >> 31) & 0x1))&&( i++ < 3));
		/*wait top field and bottom field to complete.*/
	}

	return 0;
}

#ifdef TEST_NO_PRESCALER_RGB
static int set_vic_no_prescaler_rgb(int idsx, int vic, int window)
{
	int ret, width, height, xoffset = 0, yoffset = 0, bpp_mode;
	dtd_t dtd;

	dss_trace("ids%d, vic %d\n", idsx, vic);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	width = dtd.mHActive & (~0x3);
	height = dtd.mVActive & (~0x3);
	if ((dtd.mHActive - width) % 4  || (dtd.mVActive - height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", width, height);
		return -1;
	}
	dss_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d\n", width, height, xoffset, yoffset);

	bpp_mode = OSD_IMAGE_RGB_BPP24_888; /*RGB 24bit*/

	/* Set Overlay0 */
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB

	ids_write(idsx, OVCW0CR, OVCWxCR_BPPMODE, 5, bpp_mode);
	ids_writeword(idsx, OVCW0PCAR, 
		((0) << OVCWxPCAR_LeftTopX) | ((0) << OVCWxPCAR_LeftTopY));
	ids_writeword(idsx, OVCW0VSSR, width);
	ids_writeword(idsx, OVCW0PCAR, 
		((0) << OVCWxPCAR_LeftTopX) | ((0) << OVCWxPCAR_LeftTopY));
	ids_writeword(idsx, OVCW0PCBR,
		((width-1) << OVCWxPCBR_RightBotX) | ((height-1) << OVCWxPCBR_RightBotY));

	/* Set DMA address */
	ids_writeword(idsx, OVCW0B0SAR, gdma_addr);
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, 0);
	dss_dbg("ids%d, dma_addr[0] = 0x%X\n", idsx, gdma_addr);

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);
	ids_writeword(idsx, OVCWPR, (dtd.mVActive - 1) << 12 | (dtd.mHActive - 1));
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	/* Disable Overlay 1 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 0);

	/* Enabe Overlay 0 */
	ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	
	return 0;
}
#endif

static int set_vic_prescaler_rgb(int idsx, int vic, int window, int format)
{
	static int init = 0;
	int ret;
	dtd_t dtd, main_dtd;
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;

	int in_width, in_height;
	int out_width, out_height;

	//int format = ARGB8888;
	int bpp_mode;

	dss_trace("ids%d, vic %d\n", idsx, vic);
	printk(KERN_INFO "osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,core.main_VIC);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	ret = vic_fill(&main_dtd, core.main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", core.main_VIC, ret);
		return ret;
	}

	if (prescaler_win_in_width && prescaler_win_in_height) {
		in_width = prescaler_win_in_width;
		in_height = prescaler_win_in_height;
	} else {
		in_width = main_dtd.mHActive;
		in_height = main_dtd.mVActive;
	}

	if (video_buff)
		gdma_addr = video_buff;

	out_width  = dtd.mHActive & (~0x3);
	out_height  = dtd.mVActive & (~0x3);

	if ((dtd.mHActive - out_width) % 4  || (dtd.mVActive - out_height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", out_width, out_height);
		return -1;
	}
	dss_dbg("out_width = %d, out_height = %d, in_width = %d, in_height = %d\n",
		out_width, out_height, in_width, in_height);

	overlay_x1 = osd0_x1;
	overlay_y1 = osd0_y1;
	overlay_x2 = osd0_x2;
	overlay_y2 = osd0_y2;

	if (format == ARGB8888)
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;
	else if (format == _IDS_RGB888)
		bpp_mode = OSD_IMAGE_RGB_BPP24_888;
	else
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;

	osd_set_out(idsx, window, format, bpp_mode,
		in_width, in_height, out_width, out_height,
		overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 0);

	/*Note: input frame is OSD_IMAGE_RGB_BPP32_8A888(16) */
	osd_set_prescaler(idsx, window, bpp_mode,
		in_width,  in_height, out_width,  out_height, SCALER_NO_BYPASS);

	if (!window) {
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	init = 1;
	return 0;
}

#ifdef TEST_NO_PRESCALER_NV
static int set_vic_no_prescaler_nv(int idsx, int vic, int window)
{
	static int init = 0;
	int ret;
	dtd_t dtd, main_dtd; 
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;
	u8* uv_addr = NULL;
	int format  = NV12;
	//int window = 0;
	int width, height;

	dss_trace("ids%d, vic %d\n", idsx, vic);
	
	dss_info("osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,core.main_VIC);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	width = dtd.mHActive & (~0x3);
	height = dtd.mVActive & (~0x3);
	if ((dtd.mHActive - width) % 4  || (dtd.mVActive - height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", width, height);
		return -1;
	}
	dss_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d\n", width, height, xoffset, yoffset);

	overlay_x1 = osd0_x1;
	overlay_y1 = osd0_y1;
	overlay_x2 = osd0_x2;
	overlay_y2 = osd0_y2;

	dss_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d\n", width, height, xoffset, yoffset);

	osd_set_no_pre_out(idsx, window, format, OSD_IMAGE_YUV_420SP,
		width, height, width, height, overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);
	ids_writeword(idsx, OVCWPR, (width- 1) << OVCWPR_OSDWINX |
		(height- 1) << OVCWPR_OSDWINY);

	ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, OVCDCR_SCALER_BEFORE_OSD);
	ids_write(idsx, OVCPSCCR, 0, 1, 0); /* Note: disable prescaler module */

	osd_set_yuv2rgb(idsx, window, format);

	ids_write(idsx, SCLCR, SCLCR_BYPASS, 1, SCALER_BYPASS);
	ids_write(idsx, SCLCR, SCLCR_ENABLE, 1,0);
	ids_write(idsx, OVCPSCCR, OVCPSCCR_EN, 1, 0);

	if (!window) {
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	init = 1;
	return 0;
}
#endif

int set_vic_osd_prescaler_par(unsigned int window, unsigned int in_width, unsigned int in_height,
		unsigned int virt_in_width, unsigned int virt_in_height,
		unsigned int out_width,unsigned int out_height, unsigned int osd0_pos_x,
		unsigned int osd0_pos_y, dma_addr_t scaler_buff, int format, int interlaced)
{
	int ret = 0;
	u32 r_in_width = 0,r_virt_in_width = 0;

	if (scaler_buff)
		video_buff = scaler_buff;

	if (format == NV12 ||format == NV21) {
		ret = osd_resolution_filter(in_width, in_height, &r_in_width);
		if (ret < 0)
			return -1;

		ret = osd_resolution_filter(in_width, in_height, &r_virt_in_width);
		if (ret < 0)
			return -1;
	} else {
		r_virt_in_width = virt_in_width;
		r_in_width = in_width;
	}

	dss_dbg("# actual width = %d, actual virt width = %d\n", r_in_width, r_virt_in_width);
	prescaler_virt_in_width = r_virt_in_width;
	prescaler_virt_in_height  = virt_in_height;
	prescaler_win_in_width = r_in_width;
	prescaler_win_in_height = in_height;
	osd0_x1 = osd0_pos_x;
	osd0_x2 = osd0_pos_x + out_width;
	osd0_y1 = osd0_pos_y;
	osd0_y2 = osd0_pos_y + out_height;

	in_format = format;
	return 0;
}

static int set_vic_prescaler_nv(int idsx, int vic, int window)
{
	static int init = 0;
	int ret;
	dtd_t dtd, main_dtd; 
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;

	int in_width, in_height;
	int out_width, out_height;

	//u32 window = 0; /* < default used osd0*/
	int format = NV12;
	int bpp_mode;

	dss_trace("ids%d, vic %d\n", idsx, vic);
	printk(KERN_INFO "osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,core.main_VIC);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	ret = vic_fill(&main_dtd, core.main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", core.main_VIC, ret);
		return ret;
	}

	if (prescaler_win_in_width && prescaler_win_in_height) {
		in_width = prescaler_win_in_width;
		in_height = prescaler_win_in_height;
	} else {
		in_width = main_dtd.mHActive;
		in_height = main_dtd.mVActive;
	}

	if (video_buff) {
		if (!window)
			gdma_addr = video_buff;
		else
			gdma1_addr = video_buff;
	}

	out_width  = dtd.mHActive & (~0x3);
	out_height	= dtd.mVActive & (~0x3);

	if ((dtd.mHActive - out_width) % 4	|| (dtd.mVActive - out_height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", out_width, out_height);
		return -1;
	}

	dss_dbg("out_width = %d, out_height = %d, in_width = %d, in_height = %d\n",
		out_width, out_height, in_width, in_height);

	overlay_x1 = osd0_x1;
	overlay_y1 = osd0_y1;
	overlay_x2 = osd0_x2;
	overlay_y2 = osd0_y2;
	bpp_mode = OSD_IMAGE_RGB_BPP24_888;

	osd_set_out(idsx, window, format, bpp_mode,
		in_width, in_height, out_width, out_height,
		overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	osd_set_yuv2rgb(idsx, window, format);

	/*Note: input frame is YUV420SP(17) */
	/*Bug: when prescaler bppmode set to YUV420SP, then srgb don't work. 
	*but set to OSD_IMAGE_RGB_BPP32_8A888 is ok*/
	osd_set_prescaler(idsx, window, OSD_IMAGE_YUV_420SP,
		in_width,  in_height, out_width, out_height, SCALER_NO_BYPASS);

	if (!window) {
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	init = 1;
	return 0;
}

int set_vic_osd_ui(int idsx,int vic, int window, int iformat, int alpha, int colorkey)
{
	static int init = 0;
	int ret;
	dtd_t dtd;
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;
	int width, height;
	//int i = 0;
	int bpp_mode;

	dss_trace("ids%d, vic %d\n", idsx, vic);
	dss_info("osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,core.main_VIC);

	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}

	width = dtd.mHActive & (~0x3);
	height = dtd.mVActive & (~0x3);
	if ((dtd.mHActive - width) % 4  || (dtd.mVActive - height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", width, height);
		return -1;
	}

	dss_dbg("out_width = %d, out_height = %d\n", width, height);
	overlay_x1 = 0;
	overlay_y1 = 0;
	overlay_x2 = width;
	overlay_y2 = height;

#if 0
	ids_write(idsx, OVCPCR, 15, 1, 1);
	for (i = 0; i < 256; i++) {
		ids_writeword(idsx, OVCW0PAL0 + i*4, i);
		ids_writeword(idsx, OVCW1PAL0 + i*4, i);
	}
	ids_write(idsx, OVCPCR, 15, 1, 0);
#endif

	/*Note: color key setting at blow*/
	ids_write(idsx, OVCW1CKCR, OVCW1CKCR_KEYBLEN, 1, 0);
	ids_write(idsx, OVCW1CKCR, OVCW1CKCR_KEYEN, 1, 1);
	ids_write(idsx, OVCW1CKCR, OVCW1CKCR_COMPKEY, 24, 0x0);
	ids_writeword(idsx, OVCW1CKR, colorkey);	/*set color key value, 0x01*/

	/*Note: alpha value setting*/
	ids_writeword(idsx, OVCW1PCCR, alpha);

	/*Note: 	Select Alpha value.
		When Per plane blending case( BLD_PIX ==0)
			0 = using ALPHA0_R/G/B values
			1 = using ALPHA1_R/G/B values
		When Per pixel blending ( BLD_PIX ==1)
			0 = selected by AEN (A value) or Chroma key
			1 = using DATA[27:24] data (when BPPMODE=5'b01110 &5'b01111),
		using DATA[31:24] data (when BPPMODE=5'b10000)
	*/
	//ids_write(idsx, OVCW1CR, OVCWxCR_BLD_PIX, 1, 0);
	//ids_write(idsx, OVCW1CR, OVCWxCR_ALPHA_SEL, 1, 1);
	if (core.osd1_rbexg == 1) {
		//ids_write(idsx, OVCW1CR, OVCW1CR_HAWSWP, 1, 1);
		//ids_write(idsx, OVCW1CR, OVCW1CR_BYTSWP, 1, 1);
		ids_write(idsx, OVCW1CR, OVCWxCR_RBEXG, 1, 1); /* ARGB-> ABGR */
	}

	if (iformat == NV12 || iformat == NV21) {
		dss_err("UI layer  can not supprt nv video format !\n");
		iformat = ARGB8888; /*Note: can running with rgb format */
	}

	if (iformat == ARGB8888)
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;
	else if (iformat == _IDS_RGB888)
		bpp_mode = OSD_IMAGE_RGB_BPP24_888;
	else
		bpp_mode = OSD_IMAGE_RGB_BPP32_8A888;

	osd_set_no_pre_out(idsx, window, iformat, bpp_mode,
		width, height, width, height, overlay_x1, overlay_y1, overlay_x2, overlay_y2);

	if (!window) {
		ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);
	} else {
		ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	}

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	init = 1;
	return 0;
}

static void set_rb_channel_exchange(void)
{
	if (core.osd0_rbexg == 1) {
		ids_write(core.idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 1); /* ARGB-> ABGR */
	}

	if (core.osd1_rbexg == 1) {
		ids_write(core.idsx, OVCW1CR, OVCWxCR_RBEXG, 1, 1); /* ARGB-> ABGR */
	}
}
static int set_vic(int idsx, int *params, int num)
{
	int ret = 0;
	int vic = *params;
	struct clk* clk_osd = NULL;
	clk_osd = clk_get_sys(IMAP_IDS0_OSD_CLK, IMAP_IDS0_PARENT_CLK);
	if (!clk_osd) {
		dss_err("clk_get(ids0_osd) failed\n");
		return -1;
	}

#ifndef CONFIG_CORONAMPW_FPGA_PLATFORM
	//clk_set_rate(clk_osd, 108000000); /* 108MHz */
	clk_set_rate(clk_osd, 150000000); /* 150MHz */
#else
	clk_set_rate(clk_osd, 40000000); /* 40MHz */
#endif

	set_rb_channel_exchange();
	if (in_format == NV12 || in_format == NV21) {
		ret = set_vic_prescaler_nv(idsx, vic, DSS_IDS_OSD0);
	} else {
		ret = set_vic_prescaler_rgb(idsx,vic, DSS_IDS_OSD0, in_format);
	}

	if (core.osd1_en)
		set_vic_osd_ui(idsx, vic, DSS_IDS_OSD1, ARGB8888,
			core.alpha, core.colorkey);
	return ret;
}

int osd_n_enable(int idsx, int osd_num, int enable)
{
	int OVCWxCR = OVCW0CR;

	dss_info("ids%d, osd%d %s\n", idsx,osd_num,enable > 0?"enable":"disable");

	switch(osd_num){
		case 0: 
			OVCWxCR = OVCW0CR; 
			break;
		case 1:
			OVCWxCR = OVCW1CR; 
			break;
		case 2:
		default:
			return -1;
			break;
	}
	
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);
	if(enable){
		ids_write(idsx, OVCWxCR, OVCWxCR_ENWIN, 1, 1); //enable
	}else{
		ids_write(idsx, OVCWxCR, OVCWxCR_ENWIN, 1, 0); //disable
	}

	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	return 0;
}

/* ids module must be power on and clk on, before any register operation */
static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	/*Note: why ?*/
	bpp = core.rgb_bpp;

	/* Note: Disable Auto FB */
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);

	/* Note: Set alpha */
	ids_writeword(idsx, OVCW1PCCR, 0xFFFFFF);

	/* Note: Refetch strategy */
	dss_info("enrefetch[%d] = %d, waittime[%d] = %d\n", idsx, enrefetch[idsx], idsx, waittime[idsx]);
	ids_write(idsx, OVCDCR, OVCDCR_Enrefetch, 1, enrefetch[idsx]);
	ids_write(idsx, OVCDCR, OVCDCR_WaitTime, 4, waittime[idsx]);

	/* Note: set scaler mode in prescaler */
	ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, 1);

	/*Note:  Disable osd1 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 0);

	/* Note: Update osd params */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display osd driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
