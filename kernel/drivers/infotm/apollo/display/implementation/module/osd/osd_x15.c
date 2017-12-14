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

#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[osd]"
#define DSS_SUB2	""

#define PRE_OVCDCR_ENREFETCH		0x1		// 0x1: Enable
#define PRE_OVCDCR_WAITTIME		0xF		// 0x0 ~ 0xF
#define HDMI_SCALER_RATIO	100
/*
#ifdef VIDEO_OSD_RGB
#define NO_PRESCALER
#endif
*/

static int NO_PRESCALER = 0;
extern int board_type;
static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int set_swap(int idsx, int *params, int num, unsigned int fb_addr, int win_id);//fengwu alex 20150514
static int dss_init_module(int idsx);
int osd_n_init(int idsx, int osd_num,int vic,dma_addr_t dma_addr);
extern unsigned int main_VIC;
extern unsigned int lcd_vic;
static int rgb_bpp = 32;
int osd_n_enable(int idsx, int osd_num, int enable);
extern int hdmi_get_current_vic(void);
extern hdmi_state;
static unsigned int video_buff = 0;
extern int hdmi_hp;
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
	.idsx = 1,
	.init = dss_init_module,
};


/* Refetch strategy */
static unsigned int enrefetch[2] = {PRE_OVCDCR_ENREFETCH, PRE_OVCDCR_ENREFETCH};
static unsigned int waittime[2] = {PRE_OVCDCR_WAITTIME, PRE_OVCDCR_WAITTIME};

static dma_addr_t gdma_addr;
static int gframe_size;
static dma_addr_t gdma1_addr;
static int gframe1_size;
static int logo_fbx;
static int fbx;
extern int pt;

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	int OVCWxCR= OVCW0CR ;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);
#if 0 
	if(idsx){                                         
		OVCWxCR = OVCW1CR; //x15 only win1 support mmu
	}
#endif	
	ids_write(idsx, OVCWxCR, OVCWxCR_ENWIN, 1, enable);

	return 0;
}

// Temp for lighten lcd
int osd_swap(int idsx, int fbx, unsigned int fb_addr, int win_id)//fengwu alex 20150514
{
	return set_swap(idsx, &fbx, 1, fb_addr, win_id); //fengwu alex 20150514
}

// Temp for lighten lcd
int osd_dma_addr(int idsx, dma_addr_t addr, int frame_size, int fbx)
{
	if (idsx == 0) {
		gdma_addr = addr;
		gframe_size = frame_size;
		logo_fbx = fbx;
	} else {
		gdma1_addr = addr;
		gframe1_size = frame_size;
	}

	return 0;
}
static int scaler_in_width = 0;
static int scaler_in_height = 0;
static int osd0_x1 = 0;
static int osd0_x2 = 0;
static int osd0_y1 = 0;
static int osd0_y2 = 0;
static int prescaler_virt_in_width = 0;
static int prescaler_virt_in_height = 0;
static int prescaler_win_in_width = 0;
static int prescaler_win_in_height = 0;
static int osd0_x1_hdmi = 0;
static int osd0_x2_hdmi = 0;
static int osd0_y1_hdmi = 0;
static int osd0_y2_hdmi = 0;
static int nv = 2;
static int prescaler_in_x = 0;
static int prescaler_in_y = 0;

void osd_config_scaler(unsigned int in_width, unsigned int in_height, 
		unsigned int out_width, unsigned int out_height)
{
	int i;
	if (((in_width > out_width) && (in_height <= out_height)) ||
			((in_width <= out_width) && (in_height > out_height)) ||
			((in_width < out_width) && (in_height <= out_height)) ||
			((in_width <= out_width) && (in_height < out_height))) {	//both zoom in&out or zoom in
		ids_write(1, SCLCR, 1, 1, 0);
		ids_writeword(1, SCLAZOR, 2);
		ids_writeword(1, SCLAZIR, 0);
		ids_writeword(1, CSCLAR0, 0x1000);
		ids_writeword(1, CSCLAR1, 0x0ff6);
		ids_writeword(1, CSCLAR2, 0x0fda);
		ids_writeword(1, CSCLAR3, 0x0fab);
		/*
		for (i = 0; i < 64; i++) {
			ids_writeword(1, CSCLAR0 + 0x4*i, zoom[2][i+2]);
			ids_writeword(1, CSCLAR0 + 0x4*i+1, zoom[2][i+1]);
			i += 2;
		}
		*/
	} else if ((in_width >= out_width) && (in_height >= out_height)) {    //zoom out
		if (in_width/out_width >= 4 || in_height/out_height >= 4) {
			ids_write(1, SCLCR, 1, 1, 0);
			ids_writeword(1, SCLAZOR, 1);
			ids_writeword(1, CSCLAR0, 0x0400);
			ids_writeword(1, CSCLAR1, 0x0400);
			ids_writeword(1, CSCLAR2, 0x0400);
			ids_writeword(1, CSCLAR3, 0x0400);
		} else if ((in_width == out_width) && (in_height == out_height)) {
			ids_write(1, SCLCR, 1, 1, 1);
			ids_writeword(1, SCLAZOR, 0);
		} else {
			ids_writeword(1, SCLAZOR, 0);
		}
	}
}

int scaler_config_size(unsigned int in_width, unsigned int in_height, unsigned int virt_in_width, unsigned int virt_in_height,
		unsigned int out_width,unsigned int out_height,
		unsigned int osd0_pos_x, unsigned int osd0_pos_y,
		unsigned int scaler_left_x, unsigned int scaler_top_y,
		dma_addr_t scaler_buff, int format, int interlaced)
{
	unsigned int scale_ratio_w;
	unsigned int scale_ratio_h;
	int idsx = 1;
	int win_w, win_h = 0;

	if (scaler_buff)
		video_buff = scaler_buff;

	in_width &= ~(0x1);
	in_height &= ~(0x1);
	if (in_width && in_height) {
		scaler_in_width = in_width;
		scaler_in_height = in_height;
	} else
		dss_err("scaler input size error\n");

	prescaler_in_x = scaler_left_x & ~(0x1);
	prescaler_in_y = scaler_top_y & ~(0x1);
	//win_w = in_width>1920?1920:in_width;
	//win_h = in_height>1080?1080:in_height;
	win_w = virt_in_width>1920?1920:virt_in_width;
	win_h = virt_in_height>1080?1080:virt_in_height;
	osd0_x1_hdmi = (1920 - win_w)/2;
	osd0_y1_hdmi = (1080- win_h)/2;
	osd0_x2_hdmi = osd0_x1_hdmi + win_w;
	osd0_y2_hdmi = osd0_y1_hdmi + win_h;
	if (lcd_interface == DSS_INTERFACE_LCDC && interlaced == 1) {
		osd0_x1 = osd0_pos_x*4;
		osd0_x2 = (osd0_pos_x + out_width)*4;
	} else {
		osd0_x1 = osd0_pos_x;
		osd0_x2 = osd0_pos_x + out_width;
	}
	osd0_y1 = osd0_pos_y;
	osd0_y2 = osd0_pos_y + out_height;

//	if (rgb_bpp == 8)
//		ids_write(1, OVCW1CKCR, 25, 1, 0);

	if (hdmi_hp == 1) {
		if (nv != format)
			nv = format;
		return 0;
	}
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);

	scale_ratio_w= in_width*8192/out_width;
	scale_ratio_h= in_height*8192/out_height;
	if (!NO_PRESCALER) {
		osd_config_scaler(in_width, in_height, out_width, out_height);

		//	ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 17);
		ids_writeword(idsx, OVCPSCPCR, (in_height-1) << 16 | (in_width-1));
	//	ids_write(idsx, OVCPSVSSR, 0, 16, in_width);
		prescaler_virt_in_width = virt_in_width;
		prescaler_virt_in_height  = virt_in_height;
		prescaler_win_in_width = in_width;
		prescaler_win_in_height  = in_height;
		ids_write(idsx, OVCPSVSSR, 0, 16, prescaler_virt_in_width);

		if(lcd_interface == DSS_INTERFACE_TVIF && hdmi_state == 0)//interlace mode
		{	
			ids_writeword(idsx, SCLIFSR, in_height/2 << 16 | in_width);
			ids_writeword(idsx, SCLOFSR, out_height/2 << 16 | out_width);
		}else{
			ids_writeword(idsx, SCLIFSR, in_height << 16 | in_width);
			ids_writeword(idsx, SCLOFSR, out_height << 16 | out_width);
		}
		ids_writeword(idsx, RHSCLR, scale_ratio_w & 0x1ffff);
		ids_writeword(idsx, RVSCLR, scale_ratio_h & 0x1ffff);
	}

	if (lcd_interface == DSS_INTERFACE_LCDC && interlaced == 1) {
		ids_writeword(idsx, OVCW0PCAR,\ 
				((osd0_pos_x*4) << OVCWxPCAR_LeftTopX) | ((osd0_pos_y) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR, \
				(((osd0_pos_x + out_width)*4 - 1) << OVCWxPCBR_RightBotX) | ((osd0_pos_y + out_height - 1) << OVCWxPCBR_RightBotY));	
		ids_writeword(idsx, OVCW0VSSR, out_width*4);
	} else {
		ids_writeword(idsx, OVCW0PCAR,\ 
				((osd0_pos_x) << OVCWxPCAR_LeftTopX) | ((osd0_pos_y) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR, \
				((osd0_pos_x + out_width - 1) << OVCWxPCBR_RightBotX) | ((osd0_pos_y + out_height - 1) << OVCWxPCBR_RightBotY));	
		ids_writeword(idsx, OVCW0VSSR, out_width);
	}

	//	ids_writeword(idsx, OVCWPR, ((out_width - 1) << OVCWPR_OSDWINX) | ((out_height - 1)<< OVCWPR_OSDWINY));
	if (!NO_PRESCALER) {
		if (nv != 0)
			ids_writeword(idsx, OVCPSB0SAR, scaler_buff + prescaler_in_x + prescaler_in_y * virt_in_width);
		else
			ids_writeword(idsx, OVCPSB0SAR, scaler_buff + (prescaler_in_x + prescaler_in_y * virt_in_width)*4);
		//	ids_writeword(idsx, OVCPSCB0SAR, scaler_buff + in_height * in_width);
		ids_writeword(idsx, OVCPSCB0SAR, scaler_buff + prescaler_in_x + prescaler_in_y * virt_in_width/2 + virt_in_width * virt_in_height);
	} else {
		ids_writeword(1, OVCW0B0SAR, scaler_buff);
		//ids_writeword(1, OVCBRB0SAR, scaler_buff + in_height * in_width);
		ids_writeword(1, OVCBRB0SAR, scaler_buff + in_height * virt_in_width);
	}


	if(format < 0 || format > 2) {
		dss_err("unsupported format\n");
		return -1;
	}

#if 0
	if (in_width/out_width >= 4 || in_height/out_height >= 4) {
			ids_writeword(idsx, SCLAZOR, 1);
			ids_writeword(idsx, CSCLAR0, 0x0400);
			ids_writeword(idsx, CSCLAR1, 0x0400);
			ids_writeword(idsx, CSCLAR2, 0x0400);
			ids_writeword(idsx, CSCLAR3, 0x0400);
	} else {
			ids_writeword(idsx, SCLAZOR, 0);
	}
#endif

	if (!NO_PRESCALER) {
		if (nv != format) {
			nv = format;
			if(format == 0) { //RGB
				ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 0);
				ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 16);
			} else if ((((rgb_bpp == 8 && format == 2) || (rgb_bpp == 32 && format == 1)) && hdmi_state == 0) ||
					(format == 2 && hdmi_state == 1)) { //NV21
				ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 17);
				ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
				ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
				ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
				ids_writeword(idsx, OVCOEF11, 298);
				ids_writeword(idsx, OVCOEF12, 516);
				ids_writeword(idsx, OVCOEF13, 0);
				ids_writeword(idsx, OVCOEF21, 298);
				ids_writeword(idsx, OVCOEF22, -100);
				ids_writeword(idsx, OVCOEF23, 1840);
				ids_writeword(idsx, OVCOEF31, 298);
				ids_writeword(idsx, OVCOEF32, 0);
				ids_writeword(idsx, OVCOEF33, 409);
			} else if ((((rgb_bpp == 8 && format == 1) || (rgb_bpp == 32 && format == 2)) && hdmi_state == 0) ||
					(format == 1 && hdmi_state == 1)) { //NV21
				ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 17);
				ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
				ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
				ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
				ids_writeword(idsx, OVCOEF11, 298);
				ids_writeword(idsx, OVCOEF12, 0);
				ids_writeword(idsx, OVCOEF13, 409);
				ids_writeword(idsx, OVCOEF21, 298);
				ids_writeword(idsx, OVCOEF22, -100);
				ids_writeword(idsx, OVCOEF23, 1840);
				ids_writeword(idsx, OVCOEF31, 298);
				ids_writeword(idsx, OVCOEF32, 516);
				ids_writeword(idsx, OVCOEF33, 0);
			}
		}
	}

	dss_err("scaler in:%d*%d pos %d*%d out:%d*%d addr:%p format:%d virt:%d*%d\n", in_width, in_height, prescaler_in_x, prescaler_in_y,
			(lcd_interface == DSS_INTERFACE_LCDC && interlaced == 1)?out_width*4:out_width, out_height, 
			scaler_buff, format, virt_in_width, virt_in_height);

	if (board_type == BOARD_I80) 
			wait_vsync_timeout(idsx, IRQ_I80INT, TIME_SWAP_INT);
	else {
		if (lcd_interface == DSS_INTERFACE_LCDC || rgb_bpp == 8 || hdmi_state == 1) {
			wait_vsync_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
		} else {
			wait_vsync_timeout(idsx, IRQ_TVINT, TIME_SWAP_INT);
		}
	}

	/*
	if (lcd_interface != DSS_INTERFACE_LCDC || interlaced != 1) {
		ids_write(idsx, LCDCON1 ,0 , 1, 0);
		msleep(10);
		ids_write(idsx, LCDCON1 ,0 , 1, 1);
	}
	*/


	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	return 0;
}

int osd0_config_pos_size(unsigned int in_width,unsigned int in_height, unsigned int in_virt_width, unsigned int in_virt_height,
			 unsigned int video_buff, unsigned int format, unsigned int local_width, unsigned int local_height)
{
	int idsx = 1;
	int ret = 0;
	int win_w, win_h = 0;
	unsigned int lcd_width, lcd_height = 0;
	dtd_t lcd_dtd;
	
	
	if (in_width && in_height) {
		scaler_in_width = in_width;
		scaler_in_height = in_height;
	} else
		dss_err("scaler input size error\n");
	
	vic_fill(&lcd_dtd, lcd_vic, 60);
	lcd_width  = lcd_dtd.mHActive;
	lcd_height = lcd_dtd.mVActive;

	if (lcd_interface == DSS_INTERFACE_LCDC && lcd_dtd.mInterlaced == 1) {
		osd0_x1 = (lcd_width - local_width)*2;
		osd0_x2 = osd0_x1 + local_width*4;
	} else {
		osd0_x1 = (lcd_width - local_width)/2;
		osd0_x2 = osd0_x1 + local_width;
	}
	osd0_y1 = (lcd_height - local_height)/2;
	osd0_y2 = osd0_y1 + local_height;

	dss_err("w%d h%d\n", in_width, in_height);
	win_w = in_virt_width>1920?1920:in_virt_width;
	win_h = in_virt_height>1080?1080:in_virt_height;
	osd0_x1_hdmi = (1920 - win_w)/2;
	osd0_y1_hdmi = (1080- win_h)/2;
	osd0_x2_hdmi = osd0_x1_hdmi + win_w;
	osd0_y2_hdmi = osd0_y1_hdmi + win_h;
	prescaler_virt_in_width = in_virt_width;
	prescaler_virt_in_height  = in_virt_height;
	prescaler_win_in_width = in_width;
	prescaler_win_in_height  = in_height;
	if (hdmi_hp == 1)
		return 0;
	dss_err("x1=%d y1=%d x2=%d y2=%d format %d\n",
		osd0_x1, osd0_y1, osd0_x2, osd0_y2, format);
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);

	if (in_width > 1680) {
		ids_writeword(idsx, OVCW0PCAR,\ 
				((osd0_x1_hdmi + (osd0_x2_hdmi - osd0_x1_hdmi)*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopX) 
				 | ((osd0_y1_hdmi + (osd0_y2_hdmi - osd0_y1_hdmi)*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0VSSR, win_w);
		ids_writeword(idsx, OVCW0PCBR, \
				((osd0_x2_hdmi - (osd0_x2_hdmi - osd0_x1_hdmi)*(100 - HDMI_SCALER_RATIO)/100/2-1) << OVCWxPCBR_RightBotX) 
				 | ((osd0_y2_hdmi - (osd0_y2_hdmi - osd0_y1_hdmi)*(100 - HDMI_SCALER_RATIO)/100/2-1) << OVCWxPCBR_RightBotY));	
	} else {
		ids_writeword(idsx, OVCW0PCAR,\ 
				((osd0_x1_hdmi) << OVCWxPCAR_LeftTopX) | ((osd0_y1_hdmi) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0VSSR, win_w);
		ids_writeword(idsx, OVCW0PCBR, \
			((osd0_x2_hdmi-1) << OVCWxPCBR_RightBotX) | ((osd0_y2_hdmi-1) << OVCWxPCBR_RightBotY));	
	}

	if (nv != format) {
		nv = format;
		if ((((rgb_bpp == 8 && format == 2) || (rgb_bpp == 32 && format == 1)) && hdmi_state == 0) ||
				(format == 2 && hdmi_state == 1)) { //NV21
			ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
			ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
			ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
			ids_writeword(idsx, OVCOEF11, 298);
			ids_writeword(idsx, OVCOEF12, 516);
			ids_writeword(idsx, OVCOEF13, 0);
			ids_writeword(idsx, OVCOEF21, 298);
			ids_writeword(idsx, OVCOEF22, -100);
			ids_writeword(idsx, OVCOEF23, 1840);
			ids_writeword(idsx, OVCOEF31, 298);
			ids_writeword(idsx, OVCOEF32, 0);
			ids_writeword(idsx, OVCOEF33, 409);
		} else if ((((rgb_bpp == 8 && format == 1) || (rgb_bpp == 32 && format == 2)) && hdmi_state == 0) ||
				(format == 1 && hdmi_state == 1)) { //NV21
			ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
			ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
			ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
			ids_writeword(idsx, OVCOEF11, 298);
			ids_writeword(idsx, OVCOEF12, 0);
			ids_writeword(idsx, OVCOEF13, 409);
			ids_writeword(idsx, OVCOEF21, 298);
			ids_writeword(idsx, OVCOEF22, -100);
			ids_writeword(idsx, OVCOEF23, 1840);
			ids_writeword(idsx, OVCOEF31, 298);
			ids_writeword(idsx, OVCOEF32, 516);
			ids_writeword(idsx, OVCOEF33, 0);
		}
	}

	if (video_buff) {
		ids_writeword(1, OVCW0B0SAR, video_buff);
		ids_writeword(1, OVCBRB0SAR, video_buff + in_height *in_width);
	}

	wait_vsync_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	wait_vsync_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}
int osd0_set_par(u16 xpos, u16 ypos, u16 xsize, u16 ysize, 
		u16 format, u32 width, dma_addr_t addr)
{

}

//fengwu 0424 d-osd
int osd_set_par(u16 xpos, u16 ypos, u16 xsize, u16 ysize, 
		u16 format, u32 width, dma_addr_t addr)
{

	dtd_t dtd;

	u16 x_lefttop, y_lefttop, x_rightbottom, y_rightbottom;

	dss_info("xpos %d ypos %d xsize %d ysize %d format %d width %d addr %x\n",
			xpos, ypos, xsize, ysize, format, width, addr);
	if (xsize == 0 || ysize == 0 || format > 1 || width == 0 || xsize >width)
		return -1;

	x_lefttop = xpos;
	y_lefttop = ypos;
	if (lcd_interface == DSS_INTERFACE_LCDC && dtd.mInterlaced == 1)
		x_rightbottom = xpos + xsize*4;
	else
		x_rightbottom = xpos + xsize;
	y_rightbottom = ypos + ysize;

	if (format == 11)
		format = 11;
	else if (rgb_bpp == 32)
		format = 16;
	else if (rgb_bpp == 8)
		format = 3;
	else
		format = 16;

	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 0);
	//   ids_write(1, OVCW1CR, OVCWxCR_BPPMODE, 5, format==11?11:16);
	ids_write(1, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);    // Disable Auto FB
	ids_write(1, OVCW1CR, OVCWxCR_BPPMODE, 5, format);
	if (lcd_interface == DSS_INTERFACE_LCDC && dtd.mInterlaced == 1)
		ids_writeword(1, OVCW1VSSR, width*4);
	else
		ids_writeword(1, OVCW1VSSR, width);
	ids_writeword(1, OVCW1PCAR, 
			((x_lefttop) << OVCWxPCAR_LeftTopX) | ((y_lefttop) << OVCWxPCAR_LeftTopY));
	ids_writeword(1, OVCW1PCBR,
			((x_rightbottom-1) << OVCWxPCBR_RightBotX) |
			((y_rightbottom-1) << OVCWxPCBR_RightBotY));    
	ids_writeword(1, OVCW1B0SAR, addr);
	//ids_write(1, OVCW1CR, OVCWxCR_RBEXG, 1, 1); //fengwu 0515
	//   ids_write(1, OVCW1CR, OVCWxCR_BLD_PIX, 1, 1);
	//   ids_write(1, OVCW1CR, OVCWxCR_ALPHA_SEL, 1, 1);

	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 1);
}


int osd_set_nv(int idsx, int format)
{
	if(format < 0 || format > 2) {
		dss_err("unsupported format\n");
		return -1;
	}

	if (nv != format) {
		nv = format;

		if(format == 0) { //RGB
			ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 11);
		} else if ((((rgb_bpp == 8 && format == 2) || (rgb_bpp == 32 && format == 1)) && hdmi_state == 0) ||
				(format == 2 && hdmi_state == 1)) { //NV21
			ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 17);
			ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
			ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
			ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
			ids_writeword(idsx, OVCOEF11, 298);
			ids_writeword(idsx, OVCOEF12, 516);
			ids_writeword(idsx, OVCOEF13, 0);
			ids_writeword(idsx, OVCOEF21, 298);
			ids_writeword(idsx, OVCOEF22, -100);
			ids_writeword(idsx, OVCOEF23, 1840);
			ids_writeword(idsx, OVCOEF31, 298);
			ids_writeword(idsx, OVCOEF32, 0);
			ids_writeword(idsx, OVCOEF33, 409);
		} else if ((((rgb_bpp == 8 && format == 1) || (rgb_bpp == 32 && format == 2)) && hdmi_state == 0) ||
				(format == 1 && hdmi_state == 1)) { //NV21
			ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, 17);
			ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
			ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
			ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
			ids_writeword(idsx, OVCOEF11, 298);
			ids_writeword(idsx, OVCOEF12, 0);
			ids_writeword(idsx, OVCOEF13, 409);
			ids_writeword(idsx, OVCOEF21, 298);
			ids_writeword(idsx, OVCOEF22, -100);
			ids_writeword(idsx, OVCOEF23, 1840);
			ids_writeword(idsx, OVCOEF31, 298);
			ids_writeword(idsx, OVCOEF32, 516);
			ids_writeword(idsx, OVCOEF33, 0);
		}
	}
	return 0;
}

int swap_cvbs_buffer(u32 phy_addr)
{
	if (fbx == 0)
		ids_writeword(0, OVCW0B0SAR, phy_addr);
	else
		ids_writeword(0, OVCW0B1SAR, phy_addr);
	ids_write(0, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(0, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	fbx = !fbx;
	wait_swap_timeout(0, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}

int swap_hdmi_buffer(u32 phy_addr)
{
	if (fbx == 0)
		ids_writeword(1, OVCW0B0SAR, phy_addr);
	else
		ids_writeword(1, OVCW0B1SAR, phy_addr);
	ids_write(1, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	fbx = fbx?0:1;
	wait_swap_timeout(1, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}
extern int blanking;
extern int blanking_2;
extern int backlight_en_control(int enable);
static int fb_id = 0;
static int fb_idx = 0;
void osd_set_parameter(void)
{
	dtd_t main_dtd;
	vic_fill(&main_dtd, main_VIC, 60000);

	if (fb_id == 0) {
		if (hdmi_state == 0) {
			if (!NO_PRESCALER) {
				ids_writeword(1, OVCW0B0SAR, video_buff);
			} else {
				if (nv != 0)
					ids_writeword(1, OVCPSB0SAR, video_buff + prescaler_in_x + prescaler_in_y * prescaler_virt_in_width);
				else
					ids_writeword(1, OVCPSB0SAR, video_buff + (prescaler_in_x + prescaler_in_y * prescaler_virt_in_width)*4);
				if (prescaler_virt_in_height && prescaler_virt_in_width)
					//	ids_writeword(1, OVCPSCB0SAR, video_buff + scaler_in_height * scaler_in_width);
					ids_writeword(1, OVCPSCB0SAR, video_buff + prescaler_virt_in_height * prescaler_virt_in_width +
							prescaler_in_x + prescaler_in_y * prescaler_virt_in_width/2);
				else
					//	ids_writeword(1, OVCPSCB0SAR, video_buff + main_dtd.mHActive * main_dtd.mVActive);
					ids_writeword(1, OVCPSCB0SAR, video_buff + main_dtd.mHActive * main_dtd.mVActive);
			}
		} else {
			ids_writeword(1, OVCW0B0SAR, video_buff);
			if (prescaler_virt_in_height && prescaler_virt_in_width)
				ids_writeword(1, OVCBRB0SAR, video_buff + prescaler_virt_in_height * prescaler_virt_in_width);
			else
				ids_writeword(1, OVCBRB0SAR, video_buff + main_dtd.mHActive * main_dtd.mVActive);
		}
	} else  {
		//dss_err("fbx %d\n", fb_idx);
		ids_write(1, OVCW1CR, OVCWxCR_BUFSEL, 2, fb_idx);
	}

//	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 1);
}
EXPORT_SYMBOL(osd_set_parameter);

static int set_swap(int idsx, int *params, int num, unsigned int fb_addr, int win_id)//fengwu 20150514
{
	int fbx = *params;
	dtd_t main_dtd;
	int ret = 0; //fengwu 20150514
	struct timeval start, end;
	//unsigned int ovcpsccr = ids_readword(idsx, OVCPSCCR);
	assert_num(1);

	if (fb_addr)
		video_buff = fb_addr;
	fb_id= win_id;
	if (fb_id == 1)
		fb_idx = fbx;

	if (blanking_2 == 1 || hdmi_hp == 1) {
		return 0;	
	}
	//dss_trace("ids%d, fbx %d\n", idsx, fbx);
	//	dss_err("********************* addr %x\n", fb_addr);

	//ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);

	//swap prescaler addr
	ret = vic_fill(&main_dtd, main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", main_VIC, ret);
		return ret;
	}

#if 0
	if (hdmi_state == 0) {
		ids_writeword(idsx, OVCPSB0SAR, fb_addr);
		if (scaler_in_height && scaler_in_width)
			ids_writeword(idsx, OVCPSCB0SAR, fb_addr + scaler_in_height * scaler_in_width);
		else
			ids_writeword(idsx, OVCPSCB0SAR, fb_addr + main_dtd.mHActive * main_dtd.mVActive);
	} else {
		ids_writeword(1, OVCW0B0SAR, fb_addr);
		if (scaler_in_height && scaler_in_width)
			ids_writeword(1, OVCBRB0SAR, fb_addr + scaler_in_height * scaler_in_width);
		else
			ids_writeword(1, OVCBRB0SAR, fb_addr + main_dtd.mHActive * main_dtd.mVActive);
	}
#endif

	if (board_type == BOARD_I80) 
			wait_vsync_timeout(idsx, IRQ_I80INT, TIME_SWAP_INT);
	else {
		if (lcd_interface == DSS_INTERFACE_LCDC || rgb_bpp == 8 || hdmi_state == 1) {
			wait_vsync_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
		} else {
			//do {
			wait_vsync_timeout(idsx, IRQ_TVINT, TIME_SWAP_INT);
			//} while(!((ids_readword(1, OVCW1PCAR) >> 31) & 0x1));
		}
	}

//	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	int ret, width, height, xoffset = 0, yoffset = 0, bpp0_mode, bpp1_mode;
	dtd_t dtd, main_dtd, lcd_dtd;
	int scaled_width;
	int scaled_height;
	int scale_w;
	int scale_h;
	int overlay_x1, overlay_x2, overlay_y1, overlay_y2;
	static int init = 0;
	struct clk *clk_osd = NULL;
	int prescaler_bpp = 17;
	int prescaler_in_width, prescaler_in_height;
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);

//	printk(KERN_INFO "osd set vic : ids%d vic %d main_VIC %d  0001\n",idsx, vic,main_VIC);

	if (item_exist("dss.implementation.ids1.noscale"))
		NO_PRESCALER = item_integer("dss.implementation.ids1.noscale", 0);
	clk_osd = clk_get_sys("imap-ids1-ods", "imap-ids1");
	if (!clk_osd) {
		dss_err("clk_get(ids1_osd) failed\n");
		return -1;
	}

	if (hdmi_state == 1) {
		if ((idsx == 1) && item_exist("dss.implementation.ids1.rgbbpp"))
			rgb_bpp = item_integer("dss.implementation.ids1.rgbbpp", 0);
		else
			rgb_bpp = 32;
		clk_set_rate(clk_osd, 198000000);
	} else if (item_exist(P_LCD_RGBBPP)) {
		rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
		clk_set_rate(clk_osd, 198000000);
	}

	ret = vic_fill(&main_dtd, main_VIC, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", main_VIC, ret);
		return ret;
	}

	ret = vic_fill(&lcd_dtd, lcd_vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill lcd dtd VIC = %d, ret = %d\n", lcd_vic, ret);
		return ret;
	}

	if (scaler_in_width && scaler_in_height) {
		main_dtd.mHActive = scaler_in_width;
		main_dtd.mVActive = scaler_in_height;
	}
	if (video_buff)
		gdma_addr = video_buff;
	//width = main_dtd.mHActive;
	//height = main_dtd.mVActive;
	ret = vic_fill(&dtd, vic, 60000); //fengwu alex 20150514
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}   

	if (lcd_interface == DSS_INTERFACE_LCDC && dtd.mInterlaced == 1)
		width = dtd.mHActive*4;
	else 
		width = dtd.mHActive;
	height = dtd.mVActive; 

	//  width = main_dtd.mHActive;
	//  height = main_dtd.mVActive; 
	if (hdmi_state == 0) {
		if (rgb_bpp == 32) {
			bpp0_mode = 16;
			bpp1_mode = 16;
		} else if (rgb_bpp == 8) {
			bpp0_mode = 3;
			bpp1_mode = 3;
		} else {
			bpp0_mode = 16;
			bpp1_mode = 16;
		}
	} else {
		bpp0_mode = 17;
		if (rgb_bpp == 32)
			bpp1_mode = 16;
		else if (rgb_bpp == 16)
			bpp1_mode = 6;
		else
			bpp1_mode = 16;
	}

	if (hdmi_state == 0) {
	//	dss_err("%d %d %d %d \n", osd0_x1, osd0_x2, osd0_y1, osd0_y2);
		if (osd0_x1 || osd0_x2 || osd0_y1 || osd0_y2) {
			overlay_x1 = osd0_x1;
			overlay_x2 = osd0_x2;
			overlay_y1 = osd0_y1;
			overlay_y2 = osd0_y2;
			if (rgb_bpp == 8)
				dtd.mHActive = (osd0_x2 - osd0_x1)/4;
			else
				dtd.mHActive = osd0_x2 - osd0_x1;
			dtd.mVActive = osd0_y2 - osd0_y1;
		} else {
			overlay_x1 = 0;
			overlay_y1 = 0;
			overlay_x2 = width;
			overlay_y2 = height;
		}
	} else {
		if (osd0_x1_hdmi || osd0_x2_hdmi || osd0_y1_hdmi || osd0_y2_hdmi) {
			overlay_x1 = osd0_x1_hdmi;
			overlay_x2 = osd0_x2_hdmi;
			overlay_y1 = osd0_y1_hdmi;
			overlay_y2 = osd0_y2_hdmi;
		} else {
			overlay_x1 = 0;
			overlay_y1 = 0;
			overlay_x2 = width;
			overlay_y2 = height;
		}
		dss_err("--- %d %d %d %d %d %d %d %d \n", osd0_x1_hdmi, osd0_x2_hdmi, osd0_y1_hdmi, osd0_y2_hdmi,
					dtd.mHActive, dtd.mVActive, width, height);
	}

	
	dss_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d\n", width, height, xoffset, yoffset);

	//ids_write(idsx, ENH_IECR, ENH_IECR_ACMPASSBY, 1 , 1); // Disable

	/* Set Overlay0 */
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB
	if (idsx == 1 && hdmi_state == 0)
		ids_write(idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 1); // due to Mali only support 0BGR888,R and B channel exchange workaround for CTS test
	else if (hdmi_state == 1)
		ids_write(idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 0); // due to Mali only support 0BGR888,R and B channel exchange workaround for CTS test

	//	ids_write(idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 0); //add by shuyu

	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0); //fengwu 20150514
	if (rgb_bpp == 8) {
		ids_write(1, OVCW0CR, OVCW0CR_HAWSWP, 1, 1);
		ids_write(1, OVCW0CR, OVCW0CR_BYTSWP, 1, 1);
	}
	ids_write(idsx, OVCW0CR, OVCWxCR_BPPMODE, 5, bpp0_mode);
	if (hdmi_state == 0) {
		ids_writeword(idsx, OVCW0VSSR, width);
		ids_writeword(idsx, OVCW0PCAR, 
				((overlay_x1) << OVCWxPCAR_LeftTopX) | ((overlay_y1) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR,
				((overlay_x2 - 1) << OVCWxPCBR_RightBotX) | ((overlay_y2 - 1) << OVCWxPCBR_RightBotY));	
	} else {
		ids_writeword(idsx, OVCW0VSSR, overlay_x2 - overlay_x1);
		ids_writeword(idsx, OVCW0PCAR, 
			((overlay_x1 + (overlay_x2 - overlay_x1)*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopX) 	
			| ((overlay_y1 + (overlay_y2 - overlay_y1)*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopY));
		ids_writeword(idsx, OVCW0PCBR,
			((overlay_x2 - (overlay_x2 - overlay_x1)*(100 - HDMI_SCALER_RATIO)/100/2 - 1) << OVCWxPCBR_RightBotX) 
			| ((overlay_y2 - (overlay_y2 - overlay_y1)*(100 - HDMI_SCALER_RATIO)/100/2 - 1) << OVCWxPCBR_RightBotY));	
	}
	ids_writeword(idsx, OVCWPR, ((width - 1) << OVCWPR_OSDWINX) | ((height - 1)<< OVCWPR_OSDWINY));

	int i = 0;                                                
	ids_write(1, OVCPCR, 15, 1, 1);                           
	for (i = 0; i < 256; i++) {                               
		ids_writeword(1, OVCW0PAL0 + i*4, i);
		ids_writeword(1, OVCW1PAL0 + i*4, i);
	}                                                         
	ids_write(1, OVCPCR, 15, 1, 0);                           

	if (hdmi_state == 0) {
		ids_write(1, OVCW1CKCR, 26, 1, 0);
		ids_write(1, OVCW1CKCR, 0, 24, 0x0);
		ids_write(1, OVCW1CKCR, 25, 1, 1);
		if (rgb_bpp == 32)
			ids_writeword(1, OVCW1CKR, 0x010101);
		else if (rgb_bpp == 16)
			ids_writeword(1, OVCW1CKR, 0x000101);
		else if (rgb_bpp == 8)
			ids_writeword(1, OVCW1CKR, 0x000001);
	} else {
		ids_write(1, OVCW1CR, OVCWxCR_BLD_PIX, 1, 0);
		ids_write(1, OVCW1CR, OVCWxCR_ALPHA_SEL, 1, 0);
		//ids_write(1, OVCW1CKCR, 26, 1, 0);
	      if(item_exist(P_HDMI_EN) && (item_integer(P_HDMI_EN,0) == 1))
	      	{
			ids_write(1, OVCW1CKCR, 24, 1, 0);
			ids_write(1, OVCW1CKCR, 25, 1, 1);
			ids_writeword(1, OVCW1CKR, 0x1f);
	      	}
	}

	/*set osd1*/
	//	if (init == 0) {
	if (rgb_bpp == 8) {
		ids_write(1, OVCW1CR, OVCW1CR_HAWSWP, 1, 1);
		ids_write(1, OVCW1CR, OVCW1CR_BYTSWP, 1, 1);
	}
	ids_write(1, OVCW1CR, OVCWxCR_BPPMODE, 5, bpp1_mode);
	ids_writeword(1, OVCW1VSSR, width);
	if (hdmi_state == 1) {
		ids_writeword(1, OVCW1PCAR, 
				((width*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopX) | ((height*(100 - HDMI_SCALER_RATIO)/100/2) << OVCWxPCAR_LeftTopY));
		ids_writeword(1, OVCW1PCBR,
				((width*(100 - HDMI_SCALER_RATIO)/100/2 + width*HDMI_SCALER_RATIO/100 - 1) << OVCWxPCBR_RightBotX) |
				((height*(100 - HDMI_SCALER_RATIO)/100/2 + height*HDMI_SCALER_RATIO/100 - 1) << OVCWxPCBR_RightBotY));    
	} else {
		ids_writeword(1, OVCW1PCAR, 
				((0) << OVCWxPCAR_LeftTopX) | ((0) << OVCWxPCAR_LeftTopY));
		ids_writeword(1, OVCW1PCBR,
				((width - 1) << OVCWxPCBR_RightBotX) |
				((height - 1) << OVCWxPCBR_RightBotY));    
	}
	ids_write(1, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);    // Disable Auto FB
	ids_writeword(1, OVCW1B0SAR, gdma1_addr);
	ids_writeword(1, OVCW1B1SAR, gdma1_addr + lcd_dtd.mHActive*lcd_dtd.mVActive*4);
	ids_write(1, OVCW1CR, OVCWxCR_BUFSEL, 2, fb_idx);
	//	} else {
	//		osd_set_par(0, 0, width, height, bpp_mode, width, gdma1_addr); 
	//	}

	/* Set DMA address */
	//	if (bpp_mode == 17) {
	//		if (vic == main_VIC) {
	//			ids_writeword(idsx, OVCW0B0SAR, gdma_addr);
	//			ids_writeword(idsx, OVCBRB0SAR, gdma_addr + main_dtd.mHActive*main_dtd.mVActive);
	//			ids_writeword(idsx, OVCW0B1SAR, gdma_addr + main_dtd.mHActive*main_dtd.mVActive + main_dtd.mHActive*main_dtd.mVActive/2);
	//			ids_writeword(idsx, OVCBRB1SAR, gdma_addr + main_dtd.mHActive*main_dtd.mVActive*2 + main_dtd.mHActive*main_dtd.mVActive/2);
	//		} else {
	//		}

	ids_writeword(idsx, OVCPSCCR, 0);
	ids_write(idsx, OVCOMC, OVCOMC_ToRGB, 1, 1);
	ids_write(idsx, OVCOMC, OVCOMC_oft_b, 5, 16);
	ids_write(idsx, OVCOMC, OVCOMC_oft_a, 5, 0);
	if ((((rgb_bpp == 8 && nv == 1) || (rgb_bpp == 32 && nv == 2)) && hdmi_state == 0) ||
				(nv == 1 && hdmi_state == 1)) { //NV21
		ids_writeword(idsx, OVCOEF11, 298);
		ids_writeword(idsx, OVCOEF12, 0);
		ids_writeword(idsx, OVCOEF13, 409);
		ids_writeword(idsx, OVCOEF21, 298);
		//	ids_writeword(idsx, OVCOEF22, 1984);
		ids_writeword(idsx, OVCOEF22, -100);
		ids_writeword(idsx, OVCOEF23, 1840);
		ids_writeword(idsx, OVCOEF31, 298);
		ids_writeword(idsx, OVCOEF32, 516);
		ids_writeword(idsx, OVCOEF33, 0);
	} else if ((((rgb_bpp == 8 && nv == 2) || (rgb_bpp == 32 && nv == 1)) && hdmi_state == 0) ||
				(nv == 2 && hdmi_state == 1)) { //NV21
		ids_writeword(idsx, OVCOEF11, 298);
		ids_writeword(idsx, OVCOEF12, 516);
		ids_writeword(idsx, OVCOEF13, 0);
		ids_writeword(idsx, OVCOEF21, 298);
		ids_writeword(idsx, OVCOEF22, -100);
		//		ids_writeword(idsx, OVCOEF22, 1984);
		ids_writeword(idsx, OVCOEF23, 1840);
		ids_writeword(idsx, OVCOEF31, 298);
		ids_writeword(idsx, OVCOEF32, 0);
		ids_writeword(idsx, OVCOEF33, 409);
	}

	dss_dbg("ids%d, dma_addr[0~1] = 0x%X, 0x%X\n", idsx, gdma_addr, gdma_addr + gframe_size);

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);

	if (NO_PRESCALER) {
		ids_writeword(1, OVCW0B0SAR, gdma_addr);
		ids_writeword(1, OVCBRB0SAR, gdma_addr + main_dtd.mHActive * main_dtd.mVActive);
		ids_write(idsx, OVCDCR, 10, 1, 1);
	} else {
		if ((vic != main_VIC && hdmi_state == 0) || (vic != lcd_vic && hdmi_state == 1)) {//need scaler
			if (hdmi_state == 0) {
				if (nv != 0)
					ids_writeword(1, OVCPSB0SAR, video_buff + prescaler_in_x + prescaler_in_y * prescaler_virt_in_width);    
				else
					ids_writeword(1, OVCPSB0SAR, video_buff + (prescaler_in_x + prescaler_in_y * prescaler_virt_in_width)*4);
				if (prescaler_virt_in_height && prescaler_virt_in_width)
					//  ids_writeword(1, OVCPSCB0SAR, video_buff + scaler_in_height * scaler_in_width);
					ids_writeword(1, OVCPSCB0SAR, video_buff + prescaler_virt_in_height * prescaler_virt_in_width +
							prescaler_in_x + prescaler_in_y * prescaler_virt_in_width/2);                                    
				else
					//  ids_writeword(1, OVCPSCB0SAR, video_buff + main_dtd.mHActive * main_dtd.mVActive);                   
					ids_writeword(1, OVCPSCB0SAR, video_buff + main_dtd.mHActive * main_dtd.mVActive);
	
				ids_writeword(idsx, OVCPSB2SAR, gdma_addr + main_dtd.mHActive * main_dtd.mVActive*2);
				ids_writeword(idsx, OVCPSCB2SAR, gdma_addr + main_dtd.mHActive * main_dtd.mVActive*3);
				if (nv != 0)
					prescaler_bpp = 17;
				else
					prescaler_bpp = 16;
				if (prescaler_win_in_width && prescaler_win_in_height) {
					prescaler_in_width = prescaler_win_in_width;
					prescaler_in_height = prescaler_win_in_height;
				} else {
					prescaler_in_width = main_dtd.mHActive;
					prescaler_in_height = main_dtd.mVActive;
				}
				ids_write(idsx, OVCPSCCR, OVCPSCCR_WINSEL, 2, 0);
			} else {
				ids_writeword(idsx, OVCPSB0SAR, gdma1_addr);
				ids_writeword(1, OVCW0B0SAR, gdma_addr);
				ids_writeword(1, OVCBRB0SAR, gdma_addr + main_dtd.mHActive * main_dtd.mVActive);
				prescaler_bpp = 16;
				prescaler_in_width = lcd_dtd.mHActive;
				prescaler_in_height = lcd_dtd.mVActive;
				ids_write(idsx, OVCPSCCR, OVCPSCCR_WINSEL, 2, 1);
			}
			if (hdmi_state == 0) {                                 
				scaled_width = dtd.mHActive;                       
				scaled_height = dtd.mVActive;                      
			} else {                                               
				scaled_width = dtd.mHActive*HDMI_SCALER_RATIO/100; 
				scaled_height = dtd.mVActive*HDMI_SCALER_RATIO/100;
			}                                                      
	
			if (hdmi_state == 0 && init == 0) {
				scaled_width &= ~(0x7);
				scaled_height &= ~(0x7);
				int osd_x, osd_y = 0;
				osd_x = (dtd.mHActive - scaled_width)/2;
				if (dtd.mVActive > scaled_height)
					osd_y = (dtd.mVActive - scaled_height) - 1;
				else
					osd_y = (dtd.mVActive - scaled_height)/2;
				if (lcd_interface == DSS_INTERFACE_LCDC && dtd.mInterlaced == 1) {
					ids_writeword(idsx, OVCW0PCAR, 
						(osd_x << OVCWxPCAR_LeftTopX) | (osd_y << OVCWxPCAR_LeftTopY));
					ids_writeword(idsx, OVCW0PCBR,
						((osd_x + scaled_width*4 - 1) << OVCWxPCBR_RightBotX) | ((osd_y + scaled_height - 1) << OVCWxPCBR_RightBotY));	
					ids_writeword(idsx, OVCW0VSSR, scaled_width*4);
				} else {
					ids_writeword(idsx, OVCW0PCAR, 
							((osd_x) << OVCWxPCAR_LeftTopX) | (osd_y << OVCWxPCAR_LeftTopY));
					ids_writeword(idsx, OVCW0PCBR, 
							((osd_x + scaled_width - 1) << OVCWxPCBR_RightBotX) | ((osd_y + scaled_height - 1) << OVCWxPCBR_RightBotY));	
					ids_writeword(idsx, OVCW0VSSR, scaled_width);
				}
			}
	
			//ret = vic_fill(&dtd, vic, 60000);
			//if (ret != 0) {
			//	dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
			//	return ret;
			//}
			ids_write(idsx, OVCDCR, 10, 1, 1);
	
			scale_w = prescaler_in_width*8192/scaled_width;
			scale_h = prescaler_in_height*8192/scaled_height;
	
			osd_config_scaler(prescaler_in_width, prescaler_in_height, scaled_width, scaled_height);
			ids_write(idsx, OVCPSCCR, OVCPSCCR_BPPMODE, 5, prescaler_bpp);
			ids_writeword(idsx, OVCPSCPCR, (prescaler_in_height-1) << 16 | (prescaler_in_width-1));
			if (prescaler_virt_in_width && hdmi_state == 0)
				ids_write(idsx, OVCPSVSSR, 0, 16, prescaler_virt_in_width);
			else
				ids_write(idsx, OVCPSVSSR, 0, 16, prescaler_in_width);
			//add by shuyu	
			//	ids_write(idsx, OVCDCR, 10, 1, 0);
			if(lcd_interface == DSS_INTERFACE_TVIF && hdmi_state == 0)//interlace mode
			{	
				ids_writeword(idsx, SCLIFSR, prescaler_in_height/2 << 16 | prescaler_in_width);
				ids_writeword(idsx, SCLOFSR, scaled_height/2 << 16 | scaled_width);
			}else{
				ids_writeword(idsx, SCLIFSR, prescaler_in_height<< 16 | prescaler_in_width);
				ids_writeword(idsx, SCLOFSR, scaled_height << 16 | scaled_width);
			}
	
			//end add by shuyu	
			ids_writeword(idsx, RHSCLR, scale_w & 0x1ffff);
			ids_writeword(idsx, RVSCLR, scale_h & 0x1ffff);
	
	
			//   ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);
			ids_writeword(idsx, SCLCR, 0x1);
			ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
			ids_write(idsx, OVCPSCCR, OVCPSCCR_EN, 1, 1); //fengwu 20150514
		} else {
			ids_write(idsx, OVCDCR, 10, 1, 1);
		}
	}
//		ids_writeword(idsx, OVCBKCOLOR, 0xffffffff);

#if 0

	/* by sun lcd in ids1, but it don't need to scaler */
	if (idsx == 1) {
		if((item_integer("hdmi.scale", 0) == 1)){
			ret = vic_fill(&dtd, 4, 60000);
			if (ret != 0) {
				dss_err("Failed to fill dtd ret = %d\n", ret);
				return ret;
			}
			scaled_width = dtd.mHActive & (~0x3);
			scaled_height = dtd.mVActive & (~0x3);
			if ((dtd.mHActive - scaled_width) % 4 || (dtd.mVActive - scaled_height) % 4) {
				dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", scaled_width, scaled_height);
				return -1;
			}
			ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 0);
			ids_write(1, OVCDCR, 10, 1, 0);
			ids_write(1, OVCDCR, 22, 1, 0);
			ids_writeword(1, 0x100C, (600-1) << 12 | (1024-1));
			ids_writeword(1, 0x5100, (600-1) << 16 | (1024-1));
			ids_writeword(1, 0x5104, (scaled_height-1) << 16 | (scaled_width-1));
			ids_writeword(1, 0x5108, (0) << 16 | (0));
			scale_w =1024*1024/scaled_width;
			scale_h = 600*1024/scaled_height;
			printk("osd_x15, scale, scaled_width = %d, scaled_height = %d, scale_w = %d, scale_h =%d\n",
					scaled_width, scaled_height, scale_w, scale_h);
			ids_writeword(1, 0x510C, (scale_h << 16) |scale_w | (0<<30) | (0<<14));
		}else {
			/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
			ids_writeword(idsx, 0x100C, (height-1) << 12 | (width-1));            
			ids_writeword(idsx, 0x5100, (height-1) << 16 | (width-1));            
			ids_writeword(idsx, 0x5104, (height-1) << 16 | (width-1));            
			ids_writeword(idsx, 0x510C, (0x400<< 16) | 0x400 | (1<<30) | (1<<14));
			ids_write(idsx, OVCDCR, 22, 1, 1);                                    
		}
	}
#endif
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */

	//	ids_writeword(idsx, SCLCR, 0x1);
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	//	ids_write(idsx, OVCPSCCR, OVCPSCCR_EN, 1, 1); //fengwu 20150514

	/* Disable Overlay 1~3 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 1);
	ids_write(idsx, OVCW2CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_ENWIN, 1, 0);

	/* Enabe Overlay 0 */
	ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	init = 1;
	return 0;
}

int osd_n_init(int idsx, int osd_num,int vic,dma_addr_t dma_addr)
{	
	int ret=-1;
	//	int osd_vic=vic;
	dtd_t osd_dtd;
	ret = vic_fill(&osd_dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill main dtd VIC = %d, ret = %d\n", main_VIC, ret);
		return ret;
	}
	if(osd_num==0){

	}else if(osd_num==1){
		// fmt 0: rgb 1:yuv
		osd_set_par(0,0,osd_dtd.mHActive,osd_dtd.mVActive,0,osd_dtd.mHActive,dma_addr); 

		ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 0);
		ids_write(1, OVCW1CR, OVCWxCR_BLD_PIX, 1, 0);
		ids_write(1, OVCW1CR, OVCWxCR_ALPHA_SEL, 1, 1);
		ids_writeword(idsx, OVCW1PCCR, 0xFFFFFF);
		// idsx ,osd_num,enable
		osd_n_enable(1,1,0);  //disable osd1
		ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	}else{
		dss_err("osd num error!\n");
		return -1;
	}
	return 0;
}
int osd_n_enable(int idsx, int osd_num, int enable)
{
	int OVCWxCR = OVCW0CR;
	dss_info("ids%d, osd%d %s\n", idsx,osd_num,enable > 0?"enable":"disable");
	if (osd_num == 1 && enable == 0)
		return 0;
	switch(osd_num){	
		case 0: 
			OVCWxCR = OVCW0CR; 
			break;
		case 1:
			OVCWxCR = OVCW1CR; 
			break;
		case 2:
		default:
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

	if (item_exist(P_LCD_RGBBPP))
		rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
	else
		rgb_bpp = 32;
		
	if (rgb_bpp == 32)
		nv = 1;
	else
		nv = 2;

	/* Disable Auto FB */
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW2CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_BUFAUTOEN, 1, 0);
	/* Set alpha */
	ids_writeword(idsx, OVCW1PCCR, 0xFFFFFF);
	ids_writeword(idsx, OVCW2PCCR, 0xFFFFFF);
	ids_writeword(idsx, OVCW3PCCR, 0xFFFFFF);
	/* Refetch strategy */
	//dss_info("enrefetch[%d] = %d, waittime[%d] = %d\n", idsx, enrefetch[idsx], idsx, waittime[idsx]);
	ids_write(idsx, OVCDCR, 9, 1, enrefetch[idsx]);
	ids_write(idsx, OVCDCR, 4, 4, waittime[idsx]);
	/* set scaler mode in prescaler, by sun */
	ids_write(idsx, OVCDCR, OVCDCR_ScalerMode, 1, 1);
	/* Disable osd1~osd3 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW2CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_ENWIN, 1, 0);
	/* Update osd params */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display osd driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
