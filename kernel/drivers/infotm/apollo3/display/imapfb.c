/***************************************************************************** 
** drivers/video/infotm/imapfb.c
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: Implementation file of Display Controller.
**
** Author:
**     Davinci Liang<davinci.liang@infotm.com>
**      
** Revision History: 
** ----------------- 
**
*****************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/dma-buf.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/ctype.h>
#include <linux/debugfs.h>
#include <linux/ioport.h>
#include <linux/io.h>

#include <linux/gpio.h>

#include <mach/rballoc.h>
#include <mach/nvedit.h>
#include <mach/power-gate.h>
#include <mach/rballoc.h>
#include <mach/items.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/param.h>

#include <ids_access.h> 
#include <dss_common.h>
#include <abstraction.h>
#include <implementation.h>
#include <controller.h>
#include <interface.h>
#include <scaler.h>

#include "api.h"
#include "imapfb.h"

#include "osd.h"
#include "logo_bmp.h"
#include "ip2906-cvbs.h"

#define IMAP_IDS_TEST
#ifdef IMAP_IDS_TEST
	#define IMAPX_FBLOG(str,args...) printk("imapfb : "str,##args);
#else 
	#define IMAPX_FBLOG(str,args...)
#endif

#define DSS_LAYER	"[imapfb]"
#define DSS_SUB1	""
#define DSS_SUB2	""

int dss_log_level = DSS_INFO_SHORT; /*Note: enabke all  messages to debug ids */

struct ids_core core;

imapfb_info_t imapfb_info[IMAPFB_NUM];
imapfb_fimd_info_t imapfb_fimd;

typedef struct {
	struct dentry *dir;
	/* atomic ops */
	struct dentry *addr_set;
	struct dentry *reg;
	struct dentry *fps;
	struct dentry *color_bar;
	struct dentry *cvbs;
	unsigned int addr;
} dbgfs_t;

static dbgfs_t dbgfs;

extern int terminal_configure(char * terminal_name, int vic, int enable);
extern int backlight_en_control(int enable);

int scaler_compute_out_size(
		unsigned int scaler_in_width, unsigned int scaler_in_height,
		unsigned int lcd_width,unsigned int lcd_height,
		unsigned int actual_width, unsigned int actual_height,
		unsigned int *scaler_out_width,unsigned int *scaler_out_height)
{
	int width, height;
	dtd_t dtd, lcd_dtd;

	/* Note: what different about main_VIC and lcd_vic ? */
	vic_fill(&dtd, core.main_VIC, 60); 
	vic_fill(&lcd_dtd, core.lcd_vic, 60);

	if (lcd_dtd.mHActive >= lcd_dtd.mVActive) {
		actual_width = 16;
		actual_height = 9;
	} else {
		actual_width = 9;
		actual_height = 16;
	}

	/*Note: */
	if ((scaler_in_width * actual_height) == (actual_width * scaler_in_height)) {
		*scaler_out_width  = lcd_width;
		*scaler_out_height = lcd_height;
	} else if(actual_width * scaler_in_height < actual_height * scaler_in_width) {
		height = scaler_in_width * actual_height / actual_width;
		*scaler_out_width  = lcd_width;
		*scaler_out_height = lcd_height * scaler_in_height / height;
	} else if(actual_width * scaler_in_height > actual_height * scaler_in_width) {
		width = scaler_in_height * actual_width / actual_height;
		*scaler_out_height = lcd_height;
		*scaler_out_width = lcd_width * scaler_in_width / width;
	}

	*scaler_out_height &= ~(0x7); 
	*scaler_out_width &= ~(0x7);

	if((scaler_in_width*dtd.mVActive) ==  (dtd.mHActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}

	if((scaler_in_width*dtd.mHActive) ==  (dtd.mVActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}

	if ((scaler_in_width*lcd_dtd.mVActive) == (lcd_dtd.mHActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}

	return 0;
}
EXPORT_SYMBOL(scaler_compute_out_size);

int imapfb_config_osd0_prescaler(imapfb_info_t *fbi, 
		unsigned int scaler_in_width, unsigned int scaler_in_height,
		unsigned int scaler_in_virt_width, unsigned int scaler_in_virt_height,
		unsigned int scaler_in_left_x, unsigned int scaler_in_top_y,
		dma_addr_t scaler_buff, int format)
{
	unsigned int lcd_width,lcd_height;
	unsigned int scaler_out_width,scaler_out_height; 
	unsigned int osd0_pos_x,osd0_pos_y;
	dtd_t lcd_dtd;

	vic_fill(&lcd_dtd, core.lcd_vic, 60); 

	if(fbi->win_id!=0) {
		printk(KERN_ERR "config osd0 & prescaler error,"
				"win_id=%d,should be 0 \n",fbi->win_id);
		return -1;
	}

	lcd_width  = lcd_dtd.mHActive;
	lcd_height = lcd_dtd.mVActive;

	scaler_compute_out_size(scaler_in_width,scaler_in_height,
			lcd_width, lcd_height, 16, 9, 
			&scaler_out_width,&scaler_out_height);

	/* compute osd0 offset in OSD */
	/*Note: this codes has been ingored "scaler_compute_out_suze" result*/
	osd0_pos_x = 0;
	osd0_pos_y = 0;
	scaler_out_width = lcd_width;
	scaler_out_height = lcd_height;

	printk(KERN_INFO "scaler_in_width=%d "
			"scaler_in_height=%d "
			"scaler_out_width=%d "
			"scaler_out_height=%d\n ",
			scaler_in_width,scaler_in_height,
			scaler_out_width,scaler_out_height);

	printk(KERN_INFO "osd0_x_offset=%d osd0_y_offset=%d \n",osd0_pos_x,osd0_pos_y);

	scaler_config_size(fbi->win_id, scaler_in_width, scaler_in_height, scaler_in_virt_width, 
			scaler_in_virt_height, scaler_out_width, scaler_out_height,
			osd0_pos_x, osd0_pos_y, scaler_in_left_x, scaler_in_top_y,
			scaler_buff, format, lcd_dtd.mInterlaced);
	return 0;
}
EXPORT_SYMBOL(imapfb_config_osd0_prescaler);

int imapfb_osd_prescaler_init(
		unsigned int scaler_in_width, unsigned int scaler_in_height,
		unsigned int scaler_in_virt_width, unsigned int scaler_in_virt_height,
		dma_addr_t scaler_buff, int format)
{ 
	unsigned int scaler_out_width,scaler_out_height; 
	unsigned int osd0_pos_x,osd0_pos_y;
	dtd_t lcd_dtd;

	vic_fill(&lcd_dtd, core.lcd_vic, 60); /*get lcd parameter*/

	/* compute osd0 offset in OSD */
	osd0_pos_x = 0;
	osd0_pos_y = 0;
	scaler_out_width = lcd_dtd.mHActive;
	scaler_out_height = lcd_dtd.mVActive;

	dss_trace("scaler_in_width=%d "
			"scaler_in_height=%d "
			"scaler_out_width=%d "
			"scaler_out_height=%d\n ",
			scaler_in_width,scaler_in_height,
			scaler_out_width,scaler_out_height);

	dss_trace("osd0_x_offset=%d osd0_y_offset=%d \n",osd0_pos_x,osd0_pos_y);

	set_vic_osd_prescaler_par(0, scaler_in_width, scaler_in_height, scaler_in_virt_width,
				scaler_in_virt_height, scaler_out_width, scaler_out_height,
				osd0_pos_x, osd0_pos_y, scaler_buff, format, lcd_dtd.mInterlaced);
	return 0;
}


int fb_get_addr_and_size(unsigned int *addr, unsigned int *size)
{
	if (imapfb_info[0].map_dma_f1 == 0 || imapfb_info[0].map_size_f1 == 0){
		printk(" framebuffer address not allocated ,check it - \n");
		return -1;
	}
	*addr = imapfb_info[0].map_dma_f1;
	*size = imapfb_info[0].map_size_f1;
	return 0;
}
EXPORT_SYMBOL(fb_get_addr_and_size);

/* Added a function shell interface for code merge. */
void imapfb_shutdown(void)
{
	/* TODO */
}
EXPORT_SYMBOL(imapfb_shutdown);

/*Note: to get the framebuffer relative paramters form item.
	if no parameters got, use the default values. */
static int fb_screen_param_init(dtd_t *dtd)
{
	struct clk *ids0_clk;
	u32 pixel_clock;
	unsigned int refreshRate;
	dss_trace(" %d  %s -- \n",__LINE__,__func__);
	memset((void *)&imapfb_fimd, 0, sizeof(imapfb_fimd_info_t));

	// default value
	imapfb_fimd.sync = 0;
	imapfb_fimd.cmap_static = 1;
	imapfb_fimd.osd_xoffset = 0;
	imapfb_fimd.osd_yoffset = 0;
	imapfb_fimd.hsync_len = dtd->mHSyncPulseWidth;
	imapfb_fimd.vsync_len = dtd->mVSyncPulseWidth;
	imapfb_fimd.left_margin = dtd->mHSyncOffset;
	imapfb_fimd.upper_margin = dtd->mVSyncOffset;
	imapfb_fimd.right_margin = dtd->mHBlanking - dtd->mHSyncOffset - dtd->mHSyncPulseWidth;
	imapfb_fimd.lower_margin = dtd->mVBlanking - dtd->mVSyncOffset - dtd->mVSyncPulseWidth;

	ids0_clk = clk_get_sys(IMAP_IDS0_EITF_CLK, IMAP_IDS0_PARENT_CLK);
	pixel_clock = clk_get_rate(ids0_clk);
	dss_trace("real pixel clock = %d\n", pixel_clock);
	
	refreshRate = pixel_clock / (dtd->mHActive + dtd->mHBlanking) * 1000 / (dtd->mVActive + dtd->mVBlanking);
	dss_trace("refreshRate = %d\n", refreshRate);

	imapfb_fimd.pixclock = 1000000000 / (dtd->mHActive + dtd->mHBlanking) * 1000
		/(dtd->mVActive + dtd->mVBlanking) * 1000 / refreshRate;

	dss_trace("imapfb_fimd.pixclock = %d\n", imapfb_fimd.pixclock);
	imapfb_fimd.bpp = 32;
	imapfb_fimd.bytes_per_pixel = 4;

	imapfb_fimd.xres = dtd->mHActive;
	imapfb_fimd.yres = dtd->mVActive;
	imapfb_fimd.osd_xres = imapfb_fimd.xres;
	imapfb_fimd.osd_yres = imapfb_fimd.yres;
	imapfb_fimd.osd_xres_virtual = imapfb_fimd.osd_xres; 
	imapfb_fimd.osd_yres_virtual = imapfb_fimd.osd_yres * 2;

	dss_trace("width=%d, height=%d,bpp=%d,hblank=%d,hfpd=%d,hbpd=%d,"
		"hspw=%d,vblank=%d,vfpd=%d,vbpd=%d,vspw=%d, pixclock=%d\n",
		imapfb_fimd.xres, imapfb_fimd.yres, imapfb_fimd.bpp, 
		(imapfb_fimd.left_margin + imapfb_fimd.right_margin + imapfb_fimd.hsync_len),
		imapfb_fimd.left_margin, imapfb_fimd.right_margin, imapfb_fimd.hsync_len, 
		(imapfb_fimd.upper_margin + imapfb_fimd.lower_margin + imapfb_fimd.vsync_len),
		imapfb_fimd.upper_margin, imapfb_fimd.lower_margin, 
		imapfb_fimd.vsync_len, imapfb_fimd.pixclock);
	return 0;
}

/*****************************************************************************
** -Function:
**    imapfb_map_video_memory(imapfb_info_t *fbi)
**
** -Description:
**    This function implement special features. The process is,
**		1. Allocate framebuffer memory for input framebuffer window.
**		2. Save physical address, virtual address and size of framebuffer memory.
**		3. If input framebuffer window is win0 and kernel logo is configured, then show this logo.
**
** -Input Param
**    *fbi        Imap Framebuffer Structure Pointer
**
** -Output Param
**    *fbi        Imap Framebuffer Structure Pointer
**
** -Return
**    -ENOMEM	: Failure
**	0			: Success
**
*****************************************************************************/

static int imapfb_map_video_memory(imapfb_info_t *fbi)
{
	dss_info("imapfb_map_video_memory size= 0x%X -- (%d Bytes)\n",fbi->fb.fix.smem_len, fbi->fb.fix.smem_len);
	//Allocate framebuffer memory and save physical address, virtual address and size
	fbi->map_size_f1 = PAGE_ALIGN(fbi->fb.fix.smem_len);
	fbi->map_cpu_f1 = dma_alloc_writecombine(fbi->dev, fbi->map_size_f1,
			&fbi->map_dma_f1, GFP_KERNEL);
	if (!fbi->map_cpu_f1) {
		printk("ERROR: failed to allocate framebuffer size %d Bytes\n", fbi->map_size_f1);
		return -1;
	}
	fbi->map_size_f1 = fbi->fb.fix.smem_len;

	//If succeed in allocating framebuffer memory, then init the memory with some color or kernel logo 
	if (fbi->map_cpu_f1) {
		memset(fbi->map_cpu_f1, 0x0, fbi->map_size_f1);

		//Set physical and virtual address for future use
		fbi->fb.screen_base = fbi->map_cpu_f1;
		fbi->fb.fix.smem_start = fbi->map_dma_f1;
	} else {
		printk(KERN_ERR "[imapfb_map_video_memory]: Overlay%d fail to allocate framebuffer memory\n", fbi->win_id);
		return -ENOMEM;
	}

	printk("framebuffer address: phy: 0x%X. virt: 0x%X\n", (u32)fbi->map_dma_f1, (u32)fbi->map_cpu_f1);

	/*Note: set window[0 |1] first frame buffer*/
	osd_dma_addr(fbi->win_id, fbi->map_dma_f1, fbi->map_size_f1/2, 0);

	return 0;
}

/*****************************************************************************
** -Function:
**    imapfb_unmap_video_memory(imapfb_info_t *fbi)
**
** -Description:
**    This function implement special features. The process is,
**		1. Free framebuffer memory for input framebuffer window.
**
** -Input Param
**    *fbi        Imap Framebuffer Structure Pointer
**
** -Output Param
**    none
**
** -Return
**    none
**
*****************************************************************************/
static void imapfb_unmap_video_memory(imapfb_info_t *fbi)
{
	dss_trace(" %d  %s -- \n",__LINE__,__func__);
#ifndef FB_MEM_ALLOC_FROM_IDS_DRV
	dma_free_writecombine(fbi->dev, fbi->map_size_f1, fbi->map_cpu_f1,  fbi->map_dma_f1);
#endif
}

/*****************************************************************************
** -Function:
**    imapfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
**
** -Description:
**    This function implement special features. The process is,
**		1. Check input video params of 'var'. If a value doesn't fit, round it up. If it's too big,
**		    return -EINVAL.
**
** -Input Param
**    *var	Variable Screen Info. Structure Pointer
**    *info	Framebuffer Structure Pointer
**
** -Output Param
**    *var	Variable Screen Info. Structure Pointer
**
** -Return
**    none
**
*****************************************************************************/ 
static int imapfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	if (!var || !info) {
		printk(KERN_ERR "[imapfb_check_var]: input argument null\n");
		return -EINVAL;
	}

	switch (var->bits_per_pixel)
	{
		case 8:
			var->red = imapfb_a1rgb232_8.red;
			var->green = imapfb_a1rgb232_8.green;
			var->blue = imapfb_a1rgb232_8.blue;
			var->transp = imapfb_a1rgb232_8.transp;
			imapfb_fimd.bpp = 8;
			imapfb_fimd.bytes_per_pixel = 1;
			break;

		case 16:
			var->red = imapfb_rgb565_16.red;
			var->green = imapfb_rgb565_16.green;
			var->blue = imapfb_rgb565_16.blue;
			var->transp = imapfb_rgb565_16.transp;
			imapfb_fimd.bpp = 16;
			imapfb_fimd.bytes_per_pixel = 2;
			break;

		case 18:
			var->red = imapfb_rgb666_18.red;
			var->green = imapfb_rgb666_18.green;
			var->blue = imapfb_rgb666_18.blue;
			var->transp = imapfb_rgb666_18.transp;
			imapfb_fimd.bpp = 18;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		case 19:
			var->red = imapfb_a1rgb666_19.red;
			var->green = imapfb_a1rgb666_19.green;
			var->blue = imapfb_a1rgb666_19.blue;
			var->transp = imapfb_a1rgb666_19.transp;
			imapfb_fimd.bpp = 19;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		case 24:
			var->red = imapfb_rgb888_24.red;
			var->green = imapfb_rgb888_24.green;
			var->blue = imapfb_rgb888_24.blue;
			var->transp = imapfb_rgb888_24.transp;
			imapfb_fimd.bpp = 24;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		case 25:
			var->red = imapfb_a1rgb888_25.red;
			var->green = imapfb_a1rgb888_25.green;
			var->blue = imapfb_a1rgb888_25.blue;
			var->transp = imapfb_a1rgb888_25.transp;
			imapfb_fimd.bpp = 25;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		case 28:
			var->red = imapfb_a4rgb888_28.red;
			var->green = imapfb_a4rgb888_28.green;
			var->blue = imapfb_a4rgb888_28.blue;
			var->transp = imapfb_a4rgb888_28.transp;
			imapfb_fimd.bpp = 28;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		case 32:
			var->red = imapfb_a8rgb888_32.red;
			var->green = imapfb_a8rgb888_32.green;
			var->blue = imapfb_a8rgb888_32.blue;
			var->transp = imapfb_a8rgb888_32.transp;
			imapfb_fimd.bpp = 32;
			imapfb_fimd.bytes_per_pixel = 4;
			break;

		default:
			printk(KERN_ERR "[imapfb_check_var]: input bits_per_pixel of var invalid\n");
			return -EINVAL;
	}

	info->fix.line_length = imapfb_fimd.bytes_per_pixel * info->var.xres;

	return 0;
}

/*****************************************************************************
** -Function:
**    imapfb_set_par(struct fb_info *info)
**
** -Description:
**    This function implement special features. The process is,
**		1. According to input framebuffer struct, set new var struct value and set these new
**		    values to relevant registers.
**
** -Input Param
**    *info	Framebuffer Structure Pointer
**
** -Output Param
**	*info	Framebuffer Structure Pointer
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/ 
static int imapfb_set_par(struct fb_info *info)
{
	struct fb_var_screeninfo *var = NULL;
	struct fb_fix_screeninfo *fix = NULL;
	imapfb_info_t *fbi = (imapfb_info_t*) info;
	int ret = 0;
	u16 xpos, ypos, xsize, ysize, format;
	dss_trace(" %d  %s  --- \n",__LINE__,__func__);

	if (!info) {
		printk(KERN_ERR "[imapfb_set_par]: input argument null\n");
		return -EINVAL;
	}

	var = &info->var;
	fix = &info->fix;

	//Set Visual Color Type
	if (var->bits_per_pixel == 8 || var->bits_per_pixel == 16 ||
		var->bits_per_pixel == 18 || var->bits_per_pixel == 19
		|| var->bits_per_pixel == 24 || var->bits_per_pixel == 25 ||
		var->bits_per_pixel == 28 || var->bits_per_pixel == 32)
		fbi->fb.fix.visual = FB_VISUAL_TRUECOLOR;
	else if (var->bits_per_pixel == 1 || var->bits_per_pixel == 2 || var->bits_per_pixel == 4)
		fbi->fb.fix.visual = FB_VISUAL_PSEUDOCOLOR;
	else {
		printk(KERN_ERR "[imapfb_set_par]: input bits_per_pixel invalid\n");
		ret = -EINVAL;
		goto out;
	}

	//Check Input Params
	ret = imapfb_check_var(var, info);
	if (ret) {
		printk(KERN_ERR "[imapfb_set_par]: fail to check var\n");
		ret = -EINVAL;
		goto out;
	}	

	if (fbi->win_id == 1) {
		xpos = (var->nonstd>>8) & 0xfff;      //visiable pos in panel
		ypos = (var->nonstd>>20) & 0xfff;
		xsize = (var->grayscale>>8) & 0xfff;  //visiable size in panel
		ysize = (var->grayscale>>20) & 0xfff; 
		format = var->nonstd&0x0f;  // data format 0:rgb 1:yuv 
	} else if (fbi->win_id == 0) {
		format = var->nonstd&0x0f;  // data format 0:rgb 1:NV21 2:NV12 
	}
out:
	return ret;
}

/*****************************************************************************
** -Function:
**    imapfb_blank(int blank_mode, struct fb_info *info)
**
** -Description:
**    This function implement special features. The process is,
**        1. According to input blank mode, Lcd display blank mode in screen.
**		a. No Blank
**		b. Vsync Suspend
**		c. Hsync Suspend
**		d. Power Down
**
** -Input Param
**	blank_mode	Blank Mode
**	*info		Framebuffer Structure
**
** -Output Param
**    none
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/

static int imapfb_blank(int blank_mode, struct fb_info *info)
{
	imapfb_info_t *fbi = (imapfb_info_t *)info;
	struct clk  *clk_ids0_osd, *clk_ids0_eitf;

	clk_ids0_osd = clk_get_sys(IMAP_IDS0_OSD_CLK, IMAP_IDS0_PARENT_CLK);
	if (!clk_ids0_osd) {
		printk("clk_get(ids0_osd) failed\n");
		return -1;
	}

	clk_ids0_eitf = clk_get_sys(IMAP_IDS0_EITF_CLK, IMAP_IDS0_PARENT_CLK);
	if (!clk_ids0_eitf) {
		printk("clk_get(ids0_eitf) failed\n");
		return -1;
	}

	switch (blank_mode) {
		/* Lcd and Backlight on */
		case FB_BLANK_UNBLANK:
			if (core.blanking == 0)
				return 0;

			dss_trace("imapfb_blank: unblank id %d\n", fbi->win_id);
			core.blanking = 0;

			if (fbi->win_id == 0) {
				module_power_on(SYSMGR_IDS0_BASE);
				msleep(50);

#ifndef CONFIG_APOLLO3_FPGA_PLATFORM
				clk_set_rate(clk_ids0_eitf, core.eitf_clk_bak); /*Note: 150MHz*/
				clk_set_rate(clk_ids0_osd, core.osd_clk_bak);
#else
				clk_set_rate(clk_ids0_eitf, 40000000); /*Note:40 MHz*/
				clk_set_rate(clk_ids0_osd, 40000000);
#endif
				clk_prepare_enable(clk_ids0_osd);
				clk_prepare_enable(clk_ids0_eitf);

				ids_writeword(DSS_IDS0, IDSINTMSK, 0);

				terminal_configure("lcd_panel", core.lcd_vic, 1); /*config lcd paned parm*/
				backlight_en_control(1); 

			} else {
				// idsx ,osd_num,enable
				osd_n_enable(DSS_IDS0, DSS_IDS_OSD1, DSS_ENABLE);  //enable osd1
			}

			break;
			/* LCD and backlight off */
		case FB_BLANK_POWERDOWN:
		case VESA_POWERDOWN:
			if (core.blanking == 1)
				return 0;

			core.osd_clk_bak = clk_get_rate(clk_ids0_osd);
			core.eitf_clk_bak = clk_get_rate(clk_ids0_eitf);

			dss_info("osd_clk = %u | eitf_clk = %u\n",
			core.osd_clk_bak, core.eitf_clk_bak);

			dss_trace("imapfb_blank: blank id %x\n", fbi->win_id);

			if (fbi->win_id == 0) { 
				backlight_en_control(0);
				/*Note: why disable osd-win1 ?*/
				osd_n_enable(DSS_IDS0, DSS_IDS_OSD1, DSS_DISABLE);  //disable osd1
				terminal_configure("lcd_panel", core.lcd_vic, 0);

				clk_disable_unprepare(clk_ids0_eitf);
				clk_disable_unprepare(clk_ids0_osd);

				module_power_down(SYSMGR_IDS0_BASE);

				core.blanking = 1;
				msleep(10);
			} else {
				/* idsx ,osd_num,enable */
				osd_n_enable(DSS_IDS0, DSS_IDS_OSD1, DSS_DISABLE);  //disable osd1
			} 

			break ;

		case VESA_VSYNC_SUSPEND:
		case VESA_HSYNC_SUSPEND:
			break;

		default:
			dss_err("[imapfb_blank]: input blank mode %d invalid\n", blank_mode);
			return -EINVAL;
	}

	return 0;
}

/*****************************************************************************
 ** -Function:
 **    imapfb_set_fb_addr(const imapfb_info_t *fbi)
 **
 ** -Description:
 **    This function implement special features. The process is,
 **        1. According to virtual offset in both direction, set new start address of input
 **		window in framebuffer ram which equals framebuffer start address plus offset.
 **
 ** -Input Param
 **    *fbi        Imap Framebuffer Structure Pointer
 **
 ** -Output Param
 **    none
 **
 ** -Return
 **    none
 **
 *****************************************************************************/
static void imapfb_set_fb_addr(const imapfb_info_t *fbi, struct fb_var_screeninfo *var)
{
	static int num = 0;
	static int fbx = 0;
	unsigned int fb_phy_addr = 0;
	dss_trace("frame:%d  | fb addr %x\n", num ++, var->reserved[0]);
	fbx = fbi->fb.var.yoffset ? 1 : 0; /*Note: yoffset == 0, fbx = 0*/

	if (var->reserved[0]) {
		fb_phy_addr = (unsigned int) var->reserved[0];
		osd_swap(DSS_IDS0, fbx, fb_phy_addr, fbi->win_id);
	} else {
		osd_swap(DSS_IDS0, fbx, 0, fbi->win_id);
	}

}

/*****************************************************************************
** -Function:
**    imapfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
**
** -Description:
**    This function implement special features. The process is,
**        1. Pan the display using 'xoffset' and 'yoffset' fields of input var structure.
**
** -Input Param
**	*var		Variable Screen Parameter Structure
**	*info	Framebuffer Structure
**
** -Output Param
**    *info	Framebuffer Structure
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/
static int imapfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	imapfb_info_t *fbi = (imapfb_info_t*)info;
	int ret;

	dss_trace(" var->xoffset=%d, var->yoffset=%d - \n",
			var->xoffset,var->yoffset);
	if (!var || !info) {
		printk(KERN_ERR "[imapfb_pan_display]: input arguments null\n");
		return -EINVAL;
	}

	if (var->xoffset + info->var.xres > info->var.xres_virtual) {
		printk(KERN_ERR "[imapfb_pan_display]: pan display out of range in horizontal direction\n");
		return -EINVAL;
	}

	if (var->yoffset + info->var.yres > info->var.yres_virtual && var->yoffset < 0x80000000) {
		printk(KERN_ERR "[imapfb_pan_display]: pan display out of range in vertical direction\n");
		return -EINVAL;
	}

	fbi->fb.var.xoffset = var->xoffset;
	fbi->fb.var.yoffset = var->yoffset;

	//Check Input Params
	ret = imapfb_check_var(&fbi->fb.var, info);
	if (ret) {
		printk(KERN_ERR "[imapfb_pan_display]: fail to check var\n");
		return -EINVAL;
	}

	imapfb_set_fb_addr(fbi, var);
	return 0;
}

static inline UINT32 imapfb_chan_to_field(UINT32 chan, struct fb_bitfield bf)
{
#if defined (CONFIG_FB_IMAP_BPP16)
	chan &= 0xffff;
	chan >>= 16 - bf.length;
#elif defined (CONFIG_FB_IMAP_BPP32)
	chan &= 0xffffffff;
	chan >>= 32 - bf.length;
#endif

	return chan << bf.offset;
}

/*****************************************************************************
** -Function:
**    imapfb_setcolreg(unsigned int regno, unsigned int red, unsigned int green,
**		unsigned int blue, unsigned int transp, struct fb_info *info)
**
** -Description:
**    This function implement special features. The process is,
**        1. Set color register
**		a. Fake palette for true color: for some special use
**		b. Real palette for paletter color: need to modify registers
**
** -Input Param
**	regno	Number of Color Register
**	red		Red Part of input Color
**	green	Green Part of input Color
**	blue		Blue Part of input Color
**	transp	Transparency Part of input Color
**	*info	Framebuffer Structure
**
** -Output Param
**    *info	Framebuffer Structure
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/
static int imapfb_setcolreg(unsigned int regno, unsigned int red, unsigned int green,
	unsigned int blue, unsigned int transp, struct fb_info *info)
{
	imapfb_info_t *fbi = (imapfb_info_t*)info;
	UINT32 val;

	if (!fbi) {
		printk(KERN_ERR "[imapfb_setcolreg]: input info null\n");
		return -EINVAL;
	}

	switch (fbi->fb.fix.visual) {
		/* Modify Fake Palette of 16 Colors */ 
		case FB_VISUAL_TRUECOLOR:
			if (regno < 256) {/* Modify Fake Palette of 256 Colors */
				unsigned int *pal = fbi->fb.pseudo_palette;

				val = imapfb_chan_to_field(red, fbi->fb.var.red);
				val |= imapfb_chan_to_field(green, fbi->fb.var.green);
				val |= imapfb_chan_to_field(blue, fbi->fb.var.blue);
				val |= imapfb_chan_to_field(transp, fbi->fb.var.transp);
				pal[regno] = val;
			} else {
				printk(KERN_ERR "[imapfb_setcolreg]: input register number %d invalid\n", regno);
				return -EINVAL;
			}
			break;

		case FB_VISUAL_PSEUDOCOLOR:
		default:
			printk(KERN_ERR "[imapfb_setcolreg]: unknown color type\n");
			return -EINVAL;
	}

	return 0;
}

/*****************************************************************************
** -Function:
**    imapfb_set_win_coordinate(imapfb_info_t *fbi, UINT32 left_x, UINT32 top_y,
**		UINT32 width, UINT32 height)
**
** -Description:
**    This function implement special features. The process is,
**        1. Set visual window coordinate and size
**        2. Modify visual window offset relative to virtual framebuffer
**
** -Input Param
**	*fbi		Imap Framebuffer Structure Pointer
**	left_x	Coordinate in x-axis of visual window left top corner (unit in pixel)
**	top_y	Coordinate in y-axis of visual window left top corner (unit in pixel)
**	width	Width in x-axis of visual window (unit in pixel)
**	height	Height in y-axis of visual window (unit in pixel)
**
** -Output Param
**    *fbi		Imap Framebuffer Structure Pointer
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/
static int imapfb_set_win_coordinate(imapfb_info_t *fbi, UINT32 left_x, 
		UINT32 top_y, UINT32 width, UINT32 height)
{
	struct fb_var_screeninfo *var= &(fbi->fb.var);	
	UINT32 win_num = fbi->win_id;

	dss_trace(" %d  %s -- \n",__LINE__,__func__);
	if (win_num >= IMAPFB_NUM) {
		printk(KERN_ERR "[imapfb_onoff_win]: input win id %d invalid\n", fbi->win_id);
		return -EINVAL;
	}

	var->xoffset += left_x;
	var->yoffset += top_y;
	var->xres = width;
	var->yres = height;

	return 0;
}

/*****************************************************************************
** -Function:
**    imapfb_set_win_pos_size(imapfb_info_t *fbi, SINT32 left_x, SINT32 top_y,
**		UINT32 width, UINT32 height)
**
** -Description:
**    This function implement special features. The process is,
**        1. Adjust visual window coordinate and size to proper value and set them
**        2. Modify visual window offset relative to virtual framebuffer
**        3. Modify start address of visual window in framebuffer memory
**
** -Input Param
**	*fbi		Imap Framebuffer Structure Pointer
**	left_x	Coordinate in x-axis of visual window left top corner (unit in pixel)
**	top_y	Coordinate in y-axis of visual window left top corner (unit in pixel)
**	width	Width in x-axis of visual window (unit in pixel)
**	height	Height in y-axis of visual window (unit in pixel)
**
** -Output Param
**    *fbi		Imap Framebuffer Structure Pointer
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/
static int imapfb_set_win_pos_size(imapfb_info_t *fbi, SINT32 left_x, 
		SINT32 top_y, UINT32 width, UINT32 height)
{
	UINT32 xoffset, yoffset;
	int ret = 0;
	dss_trace(" %d  %s -- \n",__LINE__,__func__);
	
	if (left_x >= imapfb_fimd.xres) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input left_x invalid, "
				"beyond screen width\n");
		return -EINVAL;
	}

	if (top_y >= imapfb_fimd.yres) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input top_y invalid, "
				"beyond screen height\n");
		return -EINVAL;
	}

	if (left_x + width <= 0) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input left_x and width too small, "
				"out of screen width\n");
		return -EINVAL;
	}

	if (top_y + height <= 0) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input top_y and height too small,"
				" out of screen height\n");
		return -EINVAL;
	}
	
	if (left_x < 0) {	
		width = left_x + width;
		xoffset = -left_x;
		left_x = 0;
	}
	
	if (top_y < 0) {
		height = top_y + height;
		yoffset = -top_y;
		top_y = 0;
	}
	
	if (left_x + width > imapfb_fimd.xres) {
		width = imapfb_fimd.xres - left_x;
	}
	
	if (top_y + height > imapfb_fimd.yres) {
		height = imapfb_fimd.yres - top_y;
	}

	ret = imapfb_set_win_coordinate(fbi, left_x, top_y, width, height);
	if (ret) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: fail to "
				"set win%d coordinate\n", fbi->win_id);
		return ret;
	}

	//Check Input Params
	ret = imapfb_check_var(&fbi->fb.var, &fbi->fb);
	if (ret) {
		printk(KERN_ERR "[imapfb_set_win_pos_size]: fail to check var\n");
		return ret;
	}

	imapfb_set_fb_addr(fbi, &fbi->fb.var);

	return 0;
}

static int imapfb_config_video_size(imapfb_info_t *fbi,
	imapfb_video_size *video_info)
{
	int ret = 0;
	if (!video_info->win_width || !video_info->win_height) {
		video_info->win_width = video_info->width;
		video_info->win_height = video_info->height;
	}
	printk(KERN_ERR " win:%dx%d buff:%d*%d\n",
		video_info->win_width,video_info->win_height,
		video_info->width,video_info->height);

	ret = imapfb_config_osd0_prescaler(fbi,
	video_info->win_width,
	video_info->win_height,
	video_info->width,
	video_info->height,
	video_info->scaler_left_x,
	video_info->scaler_top_y,
	video_info->scaler_buff,
	video_info->format);

	return ret;
}

#ifdef CONFIG_ION
int fb_get_dmabuf(struct fb_info *info, int flags)
{
	struct dma_buf *dmabuf;

	if (info->fbops->fb_dmabuf_export == NULL)
		return -ENOTTY;

	dmabuf = info->fbops->fb_dmabuf_export(info);
	if (IS_ERR(dmabuf))
		return PTR_ERR(dmabuf);

	return dma_buf_fd(dmabuf, flags);
}
#endif

/*****************************************************************************
** -Function:
**    imapfb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
**
** -Description:
**    This function implement special features. The process is,
**        1. According input command code, perform some operations
**		a. Set the position and size of some window
**		b. Open/close some window
**		c. Set value of alpha0/1 of some window
**		d. Start/stop color key function of some window
**		e. Start/stop alpha blending in color key function of some window
**		f. Set color key info. of some window
**		g. Power up/down lcd screen
**		h. Get edid info. of lcd screen
**		i. Detect lcd connection
**		j. Get framebuffer physical address info. of some window
**
** -Input Param
**    *info	Framebuffer Structure
**    cmd		Command Code
**    arg		Command Argument
**
** -Output Param
**    *info	Framebuffer Structure
**    arg		Command Argument
**
** -Return
**	0		Success
**	others	Failure
**
*****************************************************************************/
static int imapfb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	imapfb_info_t *fbi = (imapfb_info_t *)info;
	imapfb_win_pos_size win_info;
	static imapfb_video_size video_info;
	imapfb_dma_info_t fb_dma_addr_info;
	struct fb_dmabuf_export dmaexp;

	static dma_addr_t suspend_phy_addr = 0;
	static u_char *   suspend_vir_addr = NULL;
	static unsigned int suspend_size = 0;
	static int lcd_suspend_flag = 0;

	void __user *argp = (void __user *)arg;
	int ret = 0;

	dss_trace(" %d  %s -- \n",__LINE__,__func__);

	if (!fbi) {
		printk(KERN_ERR "[imapfb_ioctl]: input info null\n");
		return -EINVAL;
	}

	switch(cmd) {
	case IMAPFB_OSD_SET_POS_SIZE:
		if (copy_from_user(&win_info,
			(imapfb_win_pos_size *)arg,
			sizeof(imapfb_win_pos_size))) {
			printk(KERN_ERR "[imapfb_ioctl]: copy win%d position and size info. "
				"from user space error\n", fbi->win_id);
			return -EFAULT;
		}

		ret = imapfb_set_win_pos_size(fbi, win_info.left_x, win_info.top_y,
			win_info.width, win_info.height);
		if (ret) {
			printk(KERN_ERR "[imapfb_ioctl]: set win%d position and size error\n",
				fbi->win_id);
			ret = -EFAULT;
		}
		break;

	case IMAPFB_LCD_SUSPEND:
		if(lcd_suspend_flag)
			break;
		/* ToDo: other pixel format should be considered , default: NV12/NV21 */
		suspend_size = PAGE_ALIGN(video_info.width * video_info.height * 3 >> 1);
		suspend_vir_addr = dma_alloc_writecombine(fbi->dev, suspend_size,
			&suspend_phy_addr, GFP_KERNEL);
		if(!suspend_vir_addr) {
			printk(KERN_ERR "[imapfb_ioctl]: alloc suspend buf failed, size: %d\n",
				suspend_size);
			break;
		}
		memcpy(suspend_vir_addr, phys_to_virt(video_info.scaler_buff), suspend_size);
		video_info.scaler_buff = suspend_phy_addr;
		imapfb_config_video_size(fbi, &video_info);
		lcd_suspend_flag = 1;
		break;

	case IMAPFB_LCD_UNSUSPEND:
		if(lcd_suspend_flag) {
			dma_free_writecombine(fbi->dev, suspend_size, suspend_vir_addr,
				suspend_phy_addr);
			lcd_suspend_flag = 0;
		}
	break;

	case IMAPFB_CONFIG_VIDEO_SIZE: //reconfig  osd0 & scaler by user's application program
		if (copy_from_user(&video_info,
			(imapfb_video_size *)arg,
			sizeof(imapfb_video_size))) {
			printk(KERN_ERR "[imapfb_ioctl]: reconfig  win%d position and"
				" size info. from user space error\n", fbi->win_id);
			return -EFAULT;
		}
		ret = imapfb_config_video_size(fbi, &video_info);
		if (ret) {
			printk(KERN_ERR "[imapfb_ioctl]: set win%d position and size error\n",
				fbi->win_id);
			ret = -EFAULT;
		}
		break;

	case IMAPFB_SWITCH_ON:
	case IMAPFB_SWITCH_OFF:
		break;

	case IMAPFB_GET_BUF_PHY_ADDR:
		fb_dma_addr_info.map_dma_f1 = fbi->map_dma_f1;
		fb_dma_addr_info.map_dma_f2 = fbi->map_dma_f2;
		fb_dma_addr_info.map_dma_f3 = fbi->map_dma_f3;
		fb_dma_addr_info.map_dma_f4 = fbi->map_dma_f4;

		if (copy_to_user((void *) arg, (const void *)&fb_dma_addr_info,
				sizeof(imapfb_dma_info_t))) {
			printk(KERN_ERR "[imapfb_ioctl]: copy win%d framebuffer "
				"physical address info. to user space error\n", fbi->win_id);
			return -EFAULT;
		}
		break;

#ifdef FB_MEM_ALLOC_FROM_IDS_DRV
		case IMAPFB_GET_PMM_DEVADDR:
			//__put_user(gDI_Info.devAddr, (unsigned int*)arg);
			break;
#endif
		case IMAPFB_GET_PHY_ADDR:
			__put_user(imapfb_info[0].map_dma_f1, (unsigned int*)arg);
			break;
#ifdef CONFIG_ION
		case FBIOGET_DMABUF:
			if (copy_from_user(&dmaexp, argp, sizeof(dmaexp)))
				return -EFAULT;

			dmaexp.fd = fb_get_dmabuf(info, dmaexp.flags);

			if (dmaexp.fd < 0)
				return dmaexp.fd;

			ret = copy_to_user(argp, &dmaexp, sizeof(dmaexp)) ? -EFAULT : 0;
			break;
#endif
		default:
			printk(KERN_ERR "[imapfb_ioctl]: unknown command type\n");
			return -EFAULT;
	}

	return 0;
}

/* Framebuffer operations structure */
struct fb_ops imapfb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var	= imapfb_check_var,
	.fb_set_par		= imapfb_set_par, /* may called after imapfb_init_registers */
	.fb_blank 		= imapfb_blank,
	.fb_pan_display	= imapfb_pan_display,
	.fb_setcolreg		= imapfb_setcolreg,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
	.fb_ioctl			= imapfb_ioctl,
};

/*****************************************************************************
** -Function:
**    imapfb_init_fbinfo(imapfb_info_t *finfo, char *drv_name, int index)
**
** -Description:
**    This function implement special features. The process is,
**		1. Init framebuffer struct of input imap framebuffer struct, including fix and var struct.
**		2. If input index is 0, then init hardware setting.
**
** -Input Param
**    *fbi        		Imap Framebuffer Structure Pointer
**	drv_name	Driver Name
**	index		Window ID
**
** -Output Param
**    *fbi        Imap Framebuffer Structure Pointer
**
** -Return
**	none
**
*****************************************************************************/
static void imapfb_init_fbinfo(imapfb_info_t *finfo, UINT8 *drv_name, UINT32 index)
{
	int i = 0;

	dss_trace(" %d  %s -- \n",__LINE__,__func__);
	if (!finfo) {
		printk(KERN_ERR "[imapfb_init_fbinfo]: input finfo null\n");
		return;
	}

	if (index >= IMAPFB_NUM) {
		printk(KERN_ERR "[imapfb_init_fbinfo]: input index invalid\n");
		return;
	}

	strcpy(finfo->fb.fix.id, drv_name);

	finfo->win_id = index; /*Note: set osd-win index*/
	finfo->fb.fix.type = FB_TYPE_PACKED_PIXELS;
	finfo->fb.fix.type_aux = 0;
	finfo->fb.fix.xpanstep = 0;
	finfo->fb.fix.ypanstep = 1;
	finfo->fb.fix.ywrapstep = 0;
	finfo->fb.fix.accel = FB_ACCEL_NONE;
	finfo->fb.fix.mmio_start = IMAP_IDS0_BASE;
	finfo->fb.fix.mmio_len = SZ_1M;

	finfo->fb.fbops = &imapfb_ops;
	finfo->fb.flags = FBINFO_FLAG_DEFAULT;

	finfo->fb.pseudo_palette = &finfo->pseudo_pal;

	finfo->fb.var.nonstd = 0;
	finfo->fb.var.activate = FB_ACTIVATE_NOW;
	finfo->fb.var.accel_flags = 0;
	finfo->fb.var.vmode = FB_VMODE_NONINTERLACED;

	finfo->fb.var.xres = imapfb_fimd.osd_xres;
	finfo->fb.var.yres = imapfb_fimd.osd_yres;
	finfo->fb.var.xres_virtual = imapfb_fimd.osd_xres_virtual;
	finfo->fb.var.yres_virtual = imapfb_fimd.osd_yres_virtual;
	finfo->fb.var.xoffset = imapfb_fimd.osd_xoffset;
	finfo->fb.var.yoffset = imapfb_fimd.osd_yoffset;
	
	finfo->fb.var.bits_per_pixel = imapfb_fimd.bpp;
	finfo->fb.var.pixclock = imapfb_fimd.pixclock;
	finfo->fb.var.hsync_len = imapfb_fimd.hsync_len;
	finfo->fb.var.left_margin = imapfb_fimd.left_margin;
	finfo->fb.var.right_margin = imapfb_fimd.right_margin;
	finfo->fb.var.vsync_len = imapfb_fimd.vsync_len;
	finfo->fb.var.upper_margin = imapfb_fimd.upper_margin;
	finfo->fb.var.lower_margin = imapfb_fimd.lower_margin;
	finfo->fb.var.sync = imapfb_fimd.sync;
	finfo->fb.var.grayscale = imapfb_fimd.cmap_grayscale;

#if defined(CONFIG_FB_IMAP_DOUBLE_BUFFER)
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual * 
			finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	dss_trace("Double FrameBuffer.\n");
#else
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual *
			finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	dss_trace("Single FrameBuffer.\n");
#endif
	finfo->fb.fix.line_length = finfo->fb.var.xres * imapfb_fimd.bytes_per_pixel;

	for (i = 0; i < 256; i++)
		finfo->palette_buffer[i] = IMAPFB_PALETTE_BUFF_CLEAR;
}

static void imapfb_item_init(void)
{
	//TODO: add custom item in here.
	if(item_exist(IDS_LCD_VIC)){
		core.lcd_vic = item_integer(IDS_LCD_VIC,0); 
		dss_info( "get lcd.vic %d\n",core.lcd_vic);
	}

	if(item_exist(IDS_PRESCALER_VIC)) {
		core.prescaler_vic = item_integer(IDS_PRESCALER_VIC,0); 
		dss_info("get prescaler.vic %d\n",core.prescaler_vic);
	}

	if(item_exist(IDS_LCD_INTERFACE)) {
		core.lcd_interface = item_integer(IDS_LCD_INTERFACE,0); 
		dss_info("init lcd default interface %d\n",core.lcd_interface);
	}

	if(item_exist(P_SRGB_IFTYPE)) {
		core.srgb_iftype = item_integer(P_SRGB_IFTYPE,0);
		dss_info("init default %s %d\n", P_SRGB_IFTYPE, core.srgb_iftype);

		if (core.srgb_iftype != SRGB_IF_RGB &&
			core.srgb_iftype != SRGB_IF_RGBDUMMY &&
			core.srgb_iftype != SRGB_IF_DUMMYRGB) {
				dss_err("can't support this type %s %d!\n", 
					P_SRGB_IFTYPE, core.srgb_iftype);
				core.srgb_iftype = SRGB_IF_RGBDUMMY;
		}
	}

	if(item_exist(P_DEV_RESET)) {
		core.dev_reset  = item_integer(P_DEV_RESET,0);
		dss_info("init default %s %d\n", P_DEV_RESET, core.dev_reset);
	}

	if(item_exist(P_LCD_NAME)) {
		item_string(core.dev_name , P_LCD_NAME, 0);
		dss_info("init default %s %s\n", P_LCD_NAME, core.dev_name);
	}

	if(item_exist(P_BT656_PAD_MODE)) {
		core.bt656_pad_mode = item_integer(P_BT656_PAD_MODE,0);
		dss_info("init default %s %d\n", P_BT656_PAD_MODE, core.bt656_pad_mode);
	}

	if (item_exist(P_LCD_RGBBPP)) {
		core.rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
		dss_info("init default %s %d\n", P_LCD_RGBBPP, core.rgb_bpp);
	} else {
		core.rgb_bpp = 32;
	}

	if (item_exist(P_OSD0_RBEXG)) {
		core.osd0_rbexg = item_integer(P_OSD0_RBEXG, 0);
		dss_info("init default %s %d\n", P_OSD0_RBEXG, core.osd0_rbexg);
	} else {
		core.osd0_rbexg  = 0;
	}

	if (item_exist(P_OSD1_RBEXG)) {
		core.osd1_rbexg = item_integer(P_OSD1_RBEXG, 0);
		dss_info("init default %s %d\n", P_OSD1_RBEXG, core.osd1_rbexg);
	} else {
		core.osd1_rbexg  = 0;
	}

	if (item_exist(P_OSD1_COLORKEY)) {
		core.colorkey = item_integer(P_OSD1_COLORKEY, 0);
		dss_info("init default %s %d\n", P_OSD1_COLORKEY, core.colorkey);
	} else {
		core.colorkey  = 0x010101;
	}

	if (item_exist(P_OSD1_ALPHA)) {
		core.alpha = item_integer(P_OSD1_ALPHA, 0);
		dss_info("init default %s %d\n", P_OSD1_ALPHA, core.alpha);
	} else {
		core.alpha  = 0xffffff;
	}

	if (core.idsx == 0) {
		if (item_exist(P_IDS0_RGB_ORDER)) {
			core.rgb_order = item_integer(P_IDS0_RGB_ORDER, 0);
			dss_info("init default %s %d\n", P_IDS0_RGB_ORDER, core.rgb_order);
		} else {
			core.rgb_order = 0x6; /* rgb default order R-G-B */
		}
	} else {
		if (item_exist(P_IDS1_RGB_ORDER)) {
			core.rgb_order = item_integer(P_IDS0_RGB_ORDER, 0);
		} else {
			core.rgb_order = 0x6;
		}
	}

	core.main_VIC = core.prescaler_vic;
	core.main_scaler_factor = 100;
}

void imapfb_create_logo(dtd_t dtd)
{
	char* uboot_logo_pix_data = NULL;
	char str[64] = {0};
	/* 
	Note:
		1)copy uboot-logo data from rballoc 
		2)destination:map_cpu_f1
	 */
	if(item_exist(IDS_CONFIG_UBOOT_LOGO)){

		item_string(str,IDS_CONFIG_UBOOT_LOGO,0);
		if(strncmp(str,"0",1) == 0 )
			core.ubootlogo_status = 0;//disabled
	}

	if (core.ubootlogo_status) {
		uboot_logo_pix_data = rbget(IDS_LOGO_FB);
		if( NULL != uboot_logo_pix_data){
			memcpy(imapfb_info[0].map_cpu_f1,
				uboot_logo_pix_data,
				dtd.mHActive* dtd.mVActive*imapfb_fimd.bytes_per_pixel );
		}else{
			printk(KERN_ALERT "[imapfb]<DEBUG>: uboot_logo_pix_data == NULL \n");
		}
	}
}

static int imapfb_create_fb(dtd_t dtd, imapfb_info_t *info)
{
	int ret, index;
	
	/*Note: create 2 framebuffer devices in apollo3, 
		the win 0  is mapping fb0 and the win1 is mapping fb1*/
	for (index = 0; index < IMAPFB_NUM; index++) {
		fb_screen_param_init(&dtd);

		imapfb_info[index].mem = info->mem;
		imapfb_info[index].io = info->io;
		imapfb_info[index].clk1 = info->clk1;
		imapfb_info[index].clk2 = info->clk2;

		imapfb_init_fbinfo(&imapfb_info[index], "imapfb", index);

		/* Initialize video memory */
		ret = imapfb_map_video_memory(&imapfb_info[index]);
		if (ret) {
			printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate framebuffer video ram\n", index);
			ret = -ENOMEM;
			goto release_io;
		}

		ret = imapfb_check_var(&imapfb_info[index].fb.var, &imapfb_info[index].fb);
		if (ret) {
			printk(KERN_ERR "[imapfb]<ERR>: win %d fail to check var\n", index);
			ret = -EINVAL;
			goto free_video_memory;
		}

		dss_trace( "%s %d\n", "fb_alloc_cmap", index);
		if (index < 2) {
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 256, 0) < 0) {
				printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		} else {
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 16, 0) < 0) {
				printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		}
		
		dss_trace("%s %d\n", "register_framebuffer", index);
		ret = register_framebuffer(&imapfb_info[index].fb);
		if (ret < 0) {
			printk(KERN_ERR "[imapfb]<ERR>: Failed to register framebuffer device %d\n", ret);
			goto dealloc_cmap;
		}
		dss_trace(KERN_INFO "fb%d: %s frame buffer device\n", imapfb_info[index].fb.node, 
				imapfb_info[index].fb.fix.id);
	}

	return 0;

release_io:
	iounmap(info->io);

dealloc_cmap:
	fb_dealloc_cmap(&imapfb_info[index].fb.cmap);

free_video_memory:
	imapfb_unmap_video_memory(&imapfb_info[index]);

	return -1;
}

static void imapfb_ids_init(struct resource* irq_res, struct resource *mem_res)
{
	ids_irq_set(DSS_IDS0, irq_res->start);
	ids_access_Initialize(DSS_IDS0, mem_res);
	implementation_init();
	abstraction_init();
}

static void imapfb_ids_core_init(void)
{
	core.idsx = DSS_IDS0;
	core.lcd_interface = DSS_INTERFACE_LCDC;

	core.srgb_iftype = SRGB_IF_RGBDUMMY;
	core.dev_reset =0 ;
	memset(core.dev_name, 0, sizeof(core.dev_name));

	core.bt656_pad_mode = 1; /*0: pad RGB0_DA[0 - 7] , 1: pad RGB0_DA[8 - 16]*/

	core.main_VIC = 2006; 	/* what meaning about main_VIC? it may be osd configs*/
	core.lcd_vic = 2000; 	/* lcd input(soc display module output) paremeters */
	core.prescaler_vic = 2006;	 /* prescaler input parameters */
	core.main_scaler_factor =100;

	core.cvbs_style = 0;
	core.blanking = 0;
	core.ubootlogo_status = 0;
	core.rgb_order = 0x06;

	core.wait_irq_timeout = 20;/*20 * 50ms */

	core.fps = 0;
	core.frame_last_cnt = 0;
	core.frame_cur_cnt = 0;
	core.last_timestamp = jiffies_to_usecs(jiffies);
	core.cur_timestamp = jiffies_to_usecs(jiffies+1);

	core.colorkey = 0x010101;
	core.alpha = 0xffffff;

	core.logo_type = LOGO_360_240;
	core.osd1_en = DSS_ENABLE;

	core.osd1_rbexg = 0;
	core.osd0_rbexg = 0;
}

void color_bar_test(int i)
{
	int width, height;

	switch (core.lcd_interface) {
	case DSS_INTERFACE_LCDC:
		width = 800;
		height = 480;
		break;

	case DSS_INTERFACE_TVIF:
		width = 720;
		height = 576;
		break;

	case DSS_INTERFACE_SRGB:
		if (i == COLOR_BAR_OSD0_NV12) {
			width = 640;
			height = 480;
		} else {
			width = 360;
			height = 240;
		}
		break;

	default:
		dss_err("can not support interface :%d\n", core.lcd_interface);
		return ;
	}

	switch (i) {
	case COLOR_BAR_OSD0_RGB:
		osd_n_enable(0,DSS_IDS_OSD1, DSS_DISABLE);
		colorbar_fill_fb((unsigned char*)imapfb_info[0].map_cpu_f1, NULL, width, height, i);

		scaler_config_size(0, width, height,
			width, height, width, height,0, 0, 0, 0,
			imapfb_info[0].map_dma_f1, ARGB8888, 0);
		break;

	case COLOR_BAR_OSD0_NV12:
		osd_n_enable(0,DSS_IDS_OSD1, DSS_DISABLE);
		colorbar_fill_fb((unsigned char*)imapfb_info[0].map_cpu_f1,
			(unsigned char*)imapfb_info[1].map_cpu_f1, width, height, i);

		scaler_config_size(0, width, height,
			width, height, width, height,0, 0, 0, 0,
			imapfb_info[0].map_dma_f1, NV12, 0);
		break;

	case COLOR_BAR_ODS1_RGB:
		colorbar_fill_fb((unsigned char*)imapfb_info[1].map_cpu_f1, NULL, width, height, i);
		set_vic_osd_ui(0, core.lcd_vic, DSS_IDS_OSD1, ARGB8888,
			core.alpha, core.colorkey);
		break;

	default:
		dss_err("can't support color bar test case\n");
		break;
	}
}

void cvbs_test(uint32_t arg)
{
#if defined(CONFIG_SND_SOC_IP2906) && defined(CONFIG_APOLLO3_CVBS_I2C_IP2906)
	uint8_t debug_mode = arg >> 31;
	uint8_t w_mode = (arg >> 30) & 0x1;

	uint8_t format = arg & 0x07;
	uint8_t colorbar = arg >> 3;

	uint8_t reg = (arg >> 16)&0xff;
	uint16_t val = arg & 0xffff;
	uint16_t val_r = 0;

	if (!debug_mode) { /* 0x000000XX */
		printk("cvbs format = 0x%x colorbar = 0x%x\n", format, colorbar);
		(void)ip2906_cvbs_config(colorbar,format);
	} else {
		if (w_mode) { /* 0xC0XXXXXX */
			printk("cvbs [w] reg = 0x%x val = 0x%x\n", reg, val);
			(void)ip2906_write(reg,val);

			(void)ip2906_read(reg, &val_r);
			if (val_r != val)
				printk("cvbs write failed | reg = 0x%x w = 0x%x r= 0x%x\n", reg, val, val_r);
		} else { /* 0x80XXXXXX */
			(void)ip2906_read(reg, &val);
			printk("cvbs [r] reg = 0x%x val = 0x%x\n", reg, val);
		}
	}
#endif
}

/*add dugfs for ids*/
/* atomic operations from debugfs */
enum dbgfs_type { 
	IDS_REG_ADDR,
	IDS_REG_VAL, IDS_FPS,
	IDS_COLOR_BAR,
	IDS_CVBS,
};
/* reading operations */
static ssize_t dbgfs_read(char __user *, size_t , loff_t *, enum dbgfs_type);
static ssize_t dbgfs_addr_read(struct file *, char __user *, size_t , loff_t *);
static ssize_t dbgfs_get_reg_read(struct file *, char __user *, size_t , loff_t *);
static ssize_t dbgfs_get_fps_read(struct file *, char __user *, size_t , loff_t *);

/* writting operations */
static int dbgfs_write(const char __user *buff, size_t , enum dbgfs_type);
static ssize_t dbgfs_addr_write(struct file *, const char __user *, size_t , loff_t *);
static ssize_t dbgfs_set_reg_write(struct file *, const char __user *, size_t , loff_t *);
static ssize_t dbgfs_set_color_bar_write(struct file *, const char __user *, size_t , loff_t *);
static ssize_t dbgfs_set_cvbs_write(struct file *, const char __user *, size_t , loff_t *);

/* ops for atomic operations */
static const struct file_operations dbgfs_addr_ops = {
	.open		= nonseekable_open,
	.read		= dbgfs_addr_read,
	.write		= dbgfs_addr_write,
	.llseek		= no_llseek,
};

static const struct file_operations dbgfs_reg_ops = {
	.open		= nonseekable_open,
	.write		= dbgfs_set_reg_write,
	.read		= dbgfs_get_reg_read,
	.llseek		= no_llseek,
};

static const struct file_operations dbgfs_fps_ops = {
	.open		= nonseekable_open,
	.read		= dbgfs_get_fps_read,
	.llseek		= no_llseek,
};

static const struct file_operations dbgfs_colorbar_ops = {
	.open		= nonseekable_open,
	.write		= dbgfs_set_color_bar_write,
	.llseek		= no_llseek,
};

static const struct file_operations dbgfs_cvbs_ops = {
	.open		= nonseekable_open,
	.write		= dbgfs_set_cvbs_write,
	.llseek		= no_llseek,
};

/* atomic operations */
static ssize_t dbgfs_read(char __user *buff, size_t count, loff_t *ppos, enum dbgfs_type type)
{
	char *str;
	u32 data = 0;
	ssize_t len = 0;
	u32 time_diff;

	str = kzalloc(count, GFP_KERNEL);
	if (!str)
		return -ENOMEM;

	switch (type) {
	case IDS_REG_ADDR:
		len = sprintf(str, "addr = 0x%x\n", dbgfs.addr);
		break;
	case IDS_REG_VAL:
		/*read resigter value */
		data = ids_readword(core.idsx, dbgfs.addr);
		len = sprintf(str, "addr = 0x%x, value = 0x%x\n", dbgfs.addr, data);
		break;
	case IDS_FPS:
#if 0
		dss_err("cur_cnt = %u, lst_cnt = %u, cur_t = %u, lst_t = %u \n",
			core.frame_cur_cnt, core.frame_last_cnt,
			core.cur_timestamp, core.last_timestamp);
#endif
		if(core.cur_timestamp != core.last_timestamp) {
			if (core.cur_timestamp > core.last_timestamp)
				time_diff = (core.cur_timestamp - core.last_timestamp);
			else
				time_diff = (core.cur_timestamp + (0xffffffff - core.last_timestamp));
			core.fps =((core.frame_cur_cnt - core.frame_last_cnt) * 1000 *1000)/time_diff;

			core.last_timestamp = core.cur_timestamp;
			core.frame_last_cnt = core.frame_cur_cnt;
		} else {
			core.fps = 0;
		}

		len = sprintf(str, "irq = %d, fps = %d\n", core.frame_cur_cnt, core.fps);
		break;

	case IDS_COLOR_BAR:
	case IDS_CVBS:
		break;

	default:
		break;
	}

	if (len < 0)
		dss_err("Can't read data\n");
	else
		len = simple_read_from_buffer(buff, count, ppos, str, len);

	kfree(str);

	return len;
}

static int dbgfs_write(const char __user *buff, size_t count, enum dbgfs_type type)
{
	int err = 0, i = 0, ret = 0;
	unsigned int arg = 0;
	char *start, *str;

	str = kzalloc(count, GFP_KERNEL);
	if (!str)
		return -ENOMEM;

	if (copy_from_user(str, buff, count)) {
		ret = -EFAULT;
		goto exit_dbgfs_write;
	}

	start = str;

	while (*start == ' ')
		start++;

	/* strip ending whitespace */
	for (i = count - 1; i > 0 && isspace(str[i]); i--)
		str[i] = 0;

	switch (type) {
	case IDS_REG_ADDR:
		if (kstrtouint(start, 16, &(dbgfs.addr)))
			ret = -EINVAL;

		if (dbgfs.addr > 0xFFFFF) {
			dss_err("error: the addr %d is out of region.\n", dbgfs.addr);
			dbgfs.addr = 0;
			goto exit_dbgfs_write;
		}

		if (dbgfs.addr%4) {
			dss_err("error: the addr %d don't align with 4.\n", dbgfs.addr);
			dbgfs.addr -= dbgfs.addr%4;
			goto exit_dbgfs_write;
		}

		break;
	case IDS_REG_VAL:
		if (kstrtouint(start, 16, &arg)) {
			ret = -EINVAL;
			goto exit_dbgfs_write;
		}

		printk("addr = 0x%x, value =0x%x\n", dbgfs.addr, arg);
		ids_writeword(core.idsx, dbgfs.addr,arg);
		break;

	case IDS_FPS:
		break;

	case IDS_COLOR_BAR:
		if (kstrtouint(start, 16, &arg)) {
			ret = -EINVAL;
			goto exit_dbgfs_write;
		}

		color_bar_test(arg);
		break;

	case IDS_CVBS:
		if (kstrtouint(start, 16, &arg)) {
			ret = -EINVAL;
			goto exit_dbgfs_write;
		}

		cvbs_test(arg);
		break;
	default:
		break;
	}

	if (err) {
		dss_err("error\n");
		ret = -1;
	}

exit_dbgfs_write:
	kfree(str);
	return ret;
}

static ssize_t dbgfs_addr_read(struct file *file, char __user *buff,
				size_t count, loff_t *ppos)
{
	ssize_t len = 0;

	if (*ppos < 0 || !count)
		return -EINVAL;

	len = dbgfs_read(buff, count, ppos, IDS_REG_ADDR);

	return len;
}

static ssize_t dbgfs_addr_write(struct file *file, const char __user *buff,
				size_t count, loff_t *ppos)
{
	ssize_t ret = 0;
	int err = 0;

	ret = count;

	if (*ppos < 0 || !count)
		return -EINVAL;

	err = dbgfs_write(buff, count, IDS_REG_ADDR);
	if (err < 0)
		return err;

	*ppos += ret;

	return ret;
}

static ssize_t dbgfs_set_reg_write(struct file *file, const char __user *buff,
				    size_t count, loff_t *ppos)
{
	ssize_t ret = 0;
	int err = 0;

	ret = count;

	if (*ppos < 0 || !count)
		return -EINVAL;

	err = dbgfs_write(buff, count, IDS_REG_VAL);
	if (err < 0)
		return err;

	*ppos += ret;

	return count;
}

static ssize_t dbgfs_set_color_bar_write(struct file *file,
	const char __user *buff, size_t count, loff_t *ppos)
{
	ssize_t ret = 0;
	int err = 0;

	ret = count;

	if (*ppos < 0 || !count)
		return -EINVAL;

	err = dbgfs_write(buff, count, IDS_COLOR_BAR);
	if (err < 0)
		return err;

	*ppos += ret;

	return count;
}

static ssize_t dbgfs_set_cvbs_write(struct file *file, const char __user *buff,
				    size_t count, loff_t *ppos)
{
	ssize_t ret = 0;
	int err = 0;

	ret = count;

	if (*ppos < 0 || !count)
		return -EINVAL;

	err = dbgfs_write(buff, count, IDS_CVBS);
	if (err < 0)
		return err;

	*ppos += ret;

	return count;
}

static ssize_t dbgfs_get_reg_read(struct file *file, char __user *buff,
				   size_t count, loff_t *ppos)
{
	ssize_t len;

	if (*ppos < 0 || !count)
		return -EINVAL;

	len = dbgfs_read(buff, count, ppos, IDS_REG_VAL);
	return len;
}

static ssize_t dbgfs_get_fps_read(struct file *file, char __user *buff,
				   size_t count, loff_t *ppos)
{
	ssize_t len;

	if (*ppos < 0 || !count)
		return -EINVAL;

	len = dbgfs_read(buff, count, ppos, IDS_FPS);
	return len;
}

void imapfb_debugfs_create(void)
{
	dbgfs.dir = debugfs_create_dir("ids", NULL);
	if (dbgfs.dir == NULL) {
		dss_err(" cannot create debugfs directory\n");
		return;
	}

	/* atomic ops */
	dbgfs.addr_set = debugfs_create_file("addr",
				S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
				dbgfs.dir, NULL,
				&dbgfs_addr_ops);
	if (dbgfs.addr_set == NULL)
		dss_err("cannot create debugfs entry addr\n");

	dbgfs.reg = debugfs_create_file("reg",
				S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
				dbgfs.dir, NULL,
				&dbgfs_reg_ops);
	if (dbgfs.reg == NULL)
		dss_err("cannot create debugfs entry reg\n");

	dbgfs.fps = debugfs_create_file("fps",
				S_IFREG | S_IRUSR | S_IRGRP,
				dbgfs.dir, NULL,
				&dbgfs_fps_ops);
	if (dbgfs.fps == NULL)
		dss_err("cannot create debugfs entry fps\n");

	dbgfs.color_bar= debugfs_create_file("colorbar",
				S_IFREG | S_IWUSR | S_IWGRP,
				dbgfs.dir, NULL,
				&dbgfs_colorbar_ops);
	if (dbgfs.color_bar == NULL)
		dss_err("cannot create debugfs entry colorbar\n");

	dbgfs.cvbs= debugfs_create_file("cvbs",
				S_IFREG | S_IWUSR | S_IWGRP,
				dbgfs.dir, NULL,
				&dbgfs_cvbs_ops);
	if (dbgfs.cvbs == NULL)
		dss_err("cannot create debugfs entry colorbar\n");
}

/* extern void imapx_bl_power_on(void); */
static int __init imapfb_probe(struct platform_device *pdev)
{
	int ret,size;
	dtd_t dtd;
	dtd_t lcd_dtd;

	struct resource *mem_res;
	struct resource *irq_res;

	struct fb_info *fbinfo;
	imapfb_info_t *info;

	imapfb_ids_core_init();
	imapfb_item_init();
	imapfb_debugfs_create();

	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!irq_res) {
		dss_err("get io resouce irq  failed !");
		return -1;
	}

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_res) {
		dss_err("get io resouce memory is fail");
		return -1;
	}

	imapfb_ids_init(irq_res, mem_res);
		
	dss_trace(KERN_INFO "[imapfb_probe]: Imap Framebuffer Driver Initialization Start!\n");
	if (!pdev) {
		printk(KERN_ERR "[imapfb_probe]: input argument null\n");
		return -EINVAL;
	}

	//Note: Allocate one Framebuffer Structure
	fbinfo = framebuffer_alloc(sizeof(imapfb_info_t), &pdev->dev);
	if (!fbinfo) {
		printk(KERN_ERR "[imapfb_probe]: fail to allocate framebuffer\n");
		ret = -ENOMEM;
		goto dealloc_fb;
	}

	platform_set_drvdata(pdev, fbinfo);
	info = fbinfo->par;
	info->dev = &pdev->dev;

	size = resource_size(mem_res);
	info->mem = request_mem_region(mem_res->start, size, pdev->name);
	if (info->mem == NULL) {
		dev_err(&pdev->dev, "failed to get memory region\n");
		ret = -ENOENT;
		goto dealloc_fb;
	}

	info->io = (void*)ids_get_base_addr(0);
	if (info->io == NULL) {
		dev_err(&pdev->dev, "ioremap() of registers failed\n");
		ret = -ENXIO;
		goto release_mem;
	}

	vic_fill(&dtd, core.prescaler_vic, 60);
	vic_fill(&lcd_dtd, core.lcd_vic, 60);

	if (core.lcd_interface == DSS_INTERFACE_LCDC) {
		bmp_logo_init(LOGO_800_480);
		dtd.mHActive = 800;
		dtd.mVActive = 480;
	} else if (core.lcd_interface == DSS_INTERFACE_TVIF) {
		bmp_logo_init(LOGO_720_576);
		dtd.mHActive = 720;
		dtd.mVActive = 576;
	} else if (core.lcd_interface == DSS_INTERFACE_SRGB) {
		bmp_logo_init(LOGO_360_240);
		dtd.mHActive = 360;
		dtd.mVActive = 240;
		/*Note: 360x240 nv12 input can not run at apollo3, it is a bug*/
	} else {
		dss_err("can not support interface :%d\n", core.lcd_interface);
		goto dealloc_fb;
	}

	dss_info("lcd_vic = %d, main_scaler_factor = %d,"
		"mHActive = %d,mVActive = %d, "
		"FrameBuffer Size: %d * %d\n", 
		core.lcd_vic, core.main_scaler_factor,
		dtd.mHActive, dtd.mVActive,
		dtd.mHActive, dtd.mVActive);

	imapfb_create_fb(dtd,info);
	dss_info("imap framebuffer driver init ok\n");

	imapfb_create_logo(dtd);

#if defined( TEST_NO_PRESCALER_RGB) ||defined (TEST_PRESCALER_RGB)
	fill_framebuffer_logo((u32)imapfb_info[0].map_cpu_f1,
		(u32*)&lcd_dtd.mHActive, (u32*)&lcd_dtd.mVActive);

	core.osd1_en = DSS_DISABLE; /*Note: only use osd0 */
	imapfb_osd_prescaler_init(
			lcd_dtd.mHActive, lcd_dtd.mVActive,
			lcd_dtd.mHActive, lcd_dtd.mVActive,
			imapfb_info[0].map_dma_f1, _IDS_RGB888);
#else
	ids_fill_dummy_yuv((u32)imapfb_info[0].map_cpu_f1 ,
		(u32)dtd.mHActive, (u32)dtd.mVActive, 0);

	#if defined( TEST_2_OSD_BLENDING)
	fill_framebuffer_logo((u32)imapfb_info[1].map_cpu_f1 ,
		(u32*)&lcd_dtd.mHActive, (u32*)&lcd_dtd.mVActive);
	#endif

	imapfb_osd_prescaler_init(
			dtd.mHActive, dtd.mVActive,
			dtd.mHActive, dtd.mVActive,
			imapfb_info[0].map_dma_f1, NV12);
#endif

	terminal_configure("lcd_panel", core.lcd_vic, 1);

	return 0;

release_mem:
	release_mem_region(mem_res->start, size);
dealloc_fb:
	framebuffer_release(fbinfo);
	return ret;
}

static int imapfb_remove(struct platform_device *pdev)
{
	struct fb_info *fbinfo = platform_get_drvdata(pdev);
	imapfb_info_t *info = fbinfo->par;
	UINT32 index = 0;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);

	msleep(1);

	//Free Framebuffer Memory and Unregister Framebuffer Device
	for (index = 0; index < IMAPFB_NUM; index++) {
		imapfb_unmap_video_memory((imapfb_info_t*)&imapfb_info[index]);
		unregister_framebuffer(&imapfb_info[index].fb);
	}

	//Unmap IO Memory
	iounmap(info->io);

	//Free IO Memory Resource
	release_resource(info->mem);
	kfree(info->mem);

	//Release Framebuffer Structure
	framebuffer_release(fbinfo);

	return 0;
}

static int imapfb_suspend(struct device *dev)
{
	dss_trace("%d\n", __LINE__);

	/*  Note: blacking is nodification to imapfb driver ,it shoud not use exteren variable */
	if (core.blanking == 0) {
		terminal_configure("lcd_panel", core.lcd_vic, 0);

		/*Note: Window's color mapping control bit. If this bit is enabled then 
		Video DMA will stop, and MAPCOLOR will be appear on 
		back-ground image instead of original image. 
		*/

		ids_writeword(0, OVCW0CMR, (1<<24));
		core.blanking = 1;
	}

	disable_irq(GIC_IDS0_ID);
	return 0;
}

static int imapfb_resume(struct device *dev)
{
	dss_trace("%d\n", __LINE__);

	ids_access_sysmgr_init();
	implementation_init();

	/* ids_writeword(0, 0x1090, 0x0);*/
	ids_writeword(0, OVCW0CMR, 0); /*Note: why do that ?*/

	terminal_configure("lcd_panel", core.lcd_vic, 1);

	/* imapx_bl_power_on(); */
	core.blanking = 0;
	enable_irq(GIC_IDS0_ID);
	return 0;
}

static const struct dev_pm_ops imapfb_pm_ops = {
	.suspend		= imapfb_suspend,
	.resume		= imapfb_resume,
};

static struct platform_driver imapfb_driver = {
	.probe		= imapfb_probe,
	.remove		= imapfb_remove,
	.driver		= {
		.name	= "imap-fb",
		.owner	= THIS_MODULE,
		.pm = &imapfb_pm_ops,
	},
};

/*
Chip ID(APOLLO3)    efuse-id       lcd
Q3420P           3420010       Lock
Q3420F           3420110       Lock
C20                3420310       ok
C23                3420210       ok
*/
extern int efuse_get_efuse_id(uint32_t *efuse_id);

int __init imapfb_init(void)
{
	IMAPX_FBLOG("  %s -- \n",__func__);
	uint32_t efuse_id;
	if(efuse_get_efuse_id(&efuse_id) == 1)
	{
		if(efuse_id == 0x3420010 ||efuse_id == 0x3420110)
		{
			dss_err("it has no lcd module,so don't load lcd driver.\n");
			return 0;
		}	
	}
	return platform_driver_register(&imapfb_driver);
}
static void __exit imapfb_cleanup(void)
{
	IMAPX_FBLOG(" %s -- \n",__func__);
	platform_driver_unregister(&imapfb_driver);
}

module_init(imapfb_init);
module_exit(imapfb_cleanup);

MODULE_AUTHOR("Davinci Liang");
MODULE_DESCRIPTION("IMAP Framebuffer Driver");
MODULE_LICENSE("GPL");
