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
**     Sam Ye<weize_ye@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0.1	 Sam@2012/3/20 :  first commit
** 1.0.6	 Sam@2012/6/12 :  add item & pmu support
** 1.0.6 	 Sam@2012/6/13    remove item judge, replace them in ids drv
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
#include <mach/rballoc.h>
#include <mach/nvedit.h>
#include <mach/power-gate.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/param.h>

#include <mach/rballoc.h>
#include <ids_access.h> //fengwu 0424 d-osd
#include "imapfb.h"

#include <mach/items.h>
#include <dss_common.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include "api.h"

//#define IMAPX_800_IDS_TEST
#ifdef IMAPX_800_IDS_TEST
	#define IMAPX_FBLOG(str,args...) printk("kernel sam : "str,##args);
#else 
	#define IMAPX_FBLOG(str,args...)
#endif

//#define FB_MEM_ALLOC_FROM_IDS_DRV
#define MAIN_FB_RESIZE	"smtv.ids.mainfb.autoresize"
#define MAIN_FB_VIC		"smtv.ids.mainfb.VIC"
#define P_VF 				"smtv.ids.videoformat"
#define P_VSH			"smtv.ids.videoscale.hdmi"

int	lcd_interface=0;
extern int hdmi_VICs[6];
int use_main_fb_resize;
unsigned int main_VIC=0;
unsigned int lcd_vic=0;       //lcd input(soc display module output) paremeters
unsigned int prescaler_vic=0; //prescaler input parameters
unsigned int main_scaler_factor;

imapfb_fimd_info_t imapfb_fimd;

extern int swap_cvbs_buffer(u32 phy_addr);
extern int osd_n_init(int idsx, int osd_num,int vic,dma_addr_t dma_addr);
extern int osd_n_enable(int idsx, int osd_num, int enable);
extern int osd_dma_addr(int idsx, dma_addr_t addr, int frame_size, int fbx);
//extern int osd_swap(int idsx, int fbx);
//fengwu alex advice 20150514
extern int osd_swap(int idsx, int fbx, unsigned int fb_addr, int win_id);
extern int osd_set_par(u16 xpos, u16 ypos, u16 xsize, u16 ysize,
               u16 format, u32 width, dma_addr_t addr);//fengwu 0424 d-osd
//extern int osd_set_nv(int idsx, int format);
extern int terminal_configure(char * terminal_name, int vic, int enable);
extern int swap_hdmi_buffer(u32 phy_addr);
extern int pt;
extern int cvbs_style;
extern int hdmi_get_vic(int a[], int len);
extern int backlight_en_control(int enable);
//extern int ids_writeword(int idsx, unsigned int addr, unsigned int val);
extern int hdmi_hplg_flow(int en);
extern int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height);
extern int fill_framebuffer_hdmi_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height);
extern int display_atoi(char *s);
extern int hdmi_state;
extern int hdmi_connect;
extern int hdmi_en;
extern int hdmi_enable(int en);
int hdmi_app_display=0;
imapfb_info_t imapfb_info[IMAPFB_NUM];


extern int osd0_config_pos_size(unsigned int in_width,unsigned int in_height, unsigned int in_virt_width, unsigned int in_virt_height, 
								unsigned int video_buff,unsigned int format, unsigned local_width, unsigned int local_height);
extern int scaler_config_size(unsigned int in_width, unsigned int in_height, unsigned int virt_in_width, unsigned int virt_in_height,
		unsigned int out_width,unsigned int out_height,
		unsigned int osd0_pos_x, unsigned int osd0_pos_y,
		unsigned int scaler_left_x, unsigned int scaler_top_y,
		dma_addr_t scaler_buff, int format, int interlaced);

int scaler_compute_out_size(unsigned int scaler_in_width, unsigned int scaler_in_height,unsigned int lcd_width,unsigned int lcd_height,\
		                    unsigned int actual_width, unsigned int actual_height, 
							unsigned int *scaler_out_width,unsigned int *scaler_out_height)
{
	int width, height;
	dtd_t dtd, lcd_dtd;

	vic_fill(&dtd, main_VIC, 60); //fengwu alex 20150514
	vic_fill(&lcd_dtd, lcd_vic, 60); //fengwu alex 20150514
	if (lcd_dtd.mHActive >= lcd_dtd.mVActive) {
		actual_width = 16;
		actual_height = 9;
	} else {
		actual_width = 9;
		actual_height = 16;
	}

	*scaler_out_width  = lcd_width;
	*scaler_out_height = lcd_height;
#if 0

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
	//*scaler_out_height = *scaler_out_height - *scaler_out_height % 10;
	//   scaler_out_width = scaler_out_width - scaler_out_width % 10;

//	if (scaler_in_width == dtd.mHActive && scaler_in_height == dtd.mVActive) {
//	if ((lcd_dtd.mHActive >= lcd_dtd.mVActive) && 
//			((scaler_in_width*dtd.mVActive) ==  (dtd.mHActive*scaler_in_height))) {
	if((scaler_in_width*dtd.mVActive) ==  (dtd.mHActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}

	//if (scaler_in_width == dtd.mVActive && scaler_in_height == dtd.mHActive) {
	//if ((lcd_dtd.mHActive < lcd_dtd.mVActive) && 
	//		((scaler_in_width*dtd.mHActive) == (dtd.mVActive*scaler_in_height))) {
	if((scaler_in_width*dtd.mHActive) ==  (dtd.mVActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}

//	if (scaler_in_width == lcd_dtd.mHActive && scaler_in_height == lcd_dtd.mVActive) {
	if ((scaler_in_width*lcd_dtd.mVActive) == (lcd_dtd.mHActive*scaler_in_height)) {
		*scaler_out_width = lcd_width;
		*scaler_out_height = lcd_height;
	}
#endif
	*scaler_out_width &= ~(0x7);
	*scaler_out_height &= ~(0x7); 
	return 0;
}
EXPORT_SYMBOL(scaler_compute_out_size);

int imapfb_config_osd0_prescaler(imapfb_info_t *fbi, unsigned int scaler_in_width, unsigned int scaler_in_height,
		unsigned int scaler_in_virt_width, unsigned int scaler_in_virt_height,
		unsigned int scaler_in_left_x, unsigned int scaler_in_top_y,
		dma_addr_t scaler_buff, int format)
{
   unsigned int lcd_width,lcd_height,scaler_out_width,scaler_out_height; 
   unsigned int osd0_pos_x,osd0_pos_y;
   dtd_t lcd_dtd;

   vic_fill(&lcd_dtd, lcd_vic, 60); //fengwu alex 20150514

   if(fbi->win_id!=0){
	  printk(KERN_ERR "config osd0 & prescaler error,win_id=%d,should be 0 \n",fbi->win_id);
      return -1;
   }

   lcd_width  = lcd_dtd.mHActive;
   lcd_height = lcd_dtd.mVActive;
   
   scaler_compute_out_size(scaler_in_width,scaler_in_height,lcd_width, lcd_height, 16, 9, \
		                    &scaler_out_width,&scaler_out_height);
   //compute osd0 offset in OSD 
   osd0_pos_x = (lcd_width - scaler_out_width)/2;	
   //osd0_pos_y = (lcd_height - scaler_out_height)/2;
   if (lcd_height > scaler_out_height)	
	   osd0_pos_y = (lcd_height - scaler_out_height) - 1;
   else
	   osd0_pos_y = (lcd_height - scaler_out_height)/2;
#if 0
   osd0_pos_x = 0;
   osd0_pos_y = 0;
   scaler_out_width = lcd_width;
   scaler_out_height = lcd_height;
#endif
   printk(KERN_INFO "scaler_in_width=%d scaler_in_height=%d scaler_out_width=%d,scaler_out_height=%d\n ", scaler_in_width,scaler_in_height,scaler_out_width,scaler_out_height); 
   printk(KERN_INFO "osd0_x_offset=%d osd0_y_offset=%d \n",osd0_pos_x,osd0_pos_y);
    
   scaler_config_size(scaler_in_width, scaler_in_height, scaler_in_virt_width, 
	   scaler_in_virt_height, scaler_out_width, scaler_out_height,
		   osd0_pos_x, osd0_pos_y, scaler_in_left_x, scaler_in_top_y,
		   scaler_buff, format, lcd_dtd.mInterlaced);
//   osd0_config_pos_size(osd0_pos_x,osd0_pos_y,scaler_out_width,scaler_out_height);
   return 0;
}
EXPORT_SYMBOL(imapfb_config_osd0_prescaler);

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

}
EXPORT_SYMBOL(imapfb_shutdown);

// to get the framebuffer relative paramters form item.
// if no parameters got, use the default values.
static int fb_screen_param_init(dtd_t *dtd, int index)
{
	struct clk *ids1_clk;
	u32 pixel_clock;
	unsigned int refreshRate;
	//IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
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
	/* Maybe need to change by sun */
	ids1_clk = clk_get_sys("imap-ids1-eitf", "imap-ids1");
	pixel_clock = clk_get_rate(ids1_clk);
	//printk("real pixel clock = %d\n", pixel_clock);
	refreshRate = pixel_clock / (dtd->mHActive + dtd->mHBlanking)  * 1000 / (dtd->mVActive + dtd->mVBlanking);
	//printk("refreshRate = %d\n", refreshRate);
	imapfb_fimd.pixclock = 1000000000 / (dtd->mHActive + dtd->mHBlanking) * 1000
		/(dtd->mVActive + dtd->mVBlanking) * 1000 / refreshRate;
	//printk("imapfb_fimd.pixclock = %d\n", imapfb_fimd.pixclock);
	if ((index == 1) && item_exist("dss.implementation.ids1.rgbbpp"))
		imapfb_fimd.bpp = item_integer("dss.implementation.ids1.rgbbpp", 0);
	else
		imapfb_fimd.bpp = 32;
	imapfb_fimd.bytes_per_pixel = (imapfb_fimd.bpp >> 3);

	imapfb_fimd.xres = dtd->mHActive;
	imapfb_fimd.yres = dtd->mVActive;
	imapfb_fimd.osd_xres = imapfb_fimd.xres;
	imapfb_fimd.osd_yres = imapfb_fimd.yres;
	imapfb_fimd.osd_xres_virtual = imapfb_fimd.osd_xres; 
	imapfb_fimd.osd_yres_virtual = imapfb_fimd.osd_yres * 2;

#if 0
	printk("width=%d, height=%d,bpp=%d,hblank=%d,hfpd=%d,hbpd=%d,hspw=%d,vblank=%d,vfpd=%d,vbpd=%d,vspw=%d, pixclock=%d\n",
			imapfb_fimd.xres, imapfb_fimd.yres, imapfb_fimd.bpp, 
			(imapfb_fimd.left_margin + imapfb_fimd.right_margin + imapfb_fimd.hsync_len),
			imapfb_fimd.left_margin, imapfb_fimd.right_margin, imapfb_fimd.hsync_len, 
			(imapfb_fimd.upper_margin + imapfb_fimd.lower_margin + imapfb_fimd.vsync_len),
			imapfb_fimd.upper_margin, imapfb_fimd.lower_margin, imapfb_fimd.vsync_len, imapfb_fimd.pixclock);
#endif
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
dma_addr_t      gmap_dma_f1;
u_char *        gmap_cpu_f1;
EXPORT_SYMBOL(gmap_dma_f1);
EXPORT_SYMBOL(gmap_cpu_f1);
dma_addr_t		map_dma_cvbs;	/* physical */
u_char *			map_cpu_cvbs;	/* virtual */
int map_cvbs_size;
static int imapfb_map_video_memory(imapfb_info_t *fbi)
{
	printk("imapfb_map_video_memory size= 0x%X -- (%d Bytes)\n",fbi->fb.fix.smem_len, fbi->fb.fix.smem_len);
	//Allocate framebuffer memory and save physical address, virtual address and size
	fbi->map_size_f1 = PAGE_ALIGN(fbi->fb.fix.smem_len);
	fbi->map_cpu_f1 = dma_alloc_writecombine(fbi->dev, fbi->map_size_f1, &fbi->map_dma_f1, GFP_KERNEL);
	if (!fbi->map_cpu_f1) {
		printk("ERROR: failed to allocate framebuffer size %d Bytes\n", fbi->map_size_f1);
		return -1;
	}
	fbi->map_size_f1 = fbi->fb.fix.smem_len;

	//If succeed in allocating framebuffer memory, then init the memory with some color or kernel logo 
	if (fbi->map_cpu_f1)
	{
		memset(fbi->map_cpu_f1, 0x0, fbi->map_size_f1);
		//Set physical and virtual address for future use
		fbi->fb.screen_base = fbi->map_cpu_f1;
		fbi->fb.fix.smem_start = fbi->map_dma_f1;
	}
	else
	{
		printk(KERN_ERR "[imapfb_map_video_memory]: Overlay%d fail to allocate framebuffer memory\n", fbi->win_id);
		return -ENOMEM;
	}
	printk("FrameBuffer Address: Physical: 0x%X. Virtual: 0x%X\n", (u32)fbi->map_dma_f1, (u32)fbi->map_cpu_f1);
	gmap_dma_f1 = fbi->map_dma_f1;
	gmap_cpu_f1 = fbi->map_cpu_f1;
    if (fbi->win_id == 0) {//fengwu 0424 d-osd
       osd_dma_addr(0, fbi->map_dma_f1, fbi->map_size_f1/2, 0);
    } else 
       osd_dma_addr(1, fbi->map_dma_f1, fbi->map_size_f1/2, 0);
	if (pt == PRODUCT_BOX) {
		if (cvbs_style == 0) //pal
			map_cvbs_size = 720*576*imapfb_fimd.bytes_per_pixel;
		else if (cvbs_style == 1) //ntsc
			map_cvbs_size = 720*480*imapfb_fimd.bytes_per_pixel;

		map_cpu_cvbs = dma_alloc_writecombine(NULL, map_cvbs_size, &map_dma_cvbs, GFP_KERNEL);
		if (!map_cpu_cvbs) {
			printk("ERROR: failed to allocate framebuffer size %d Bytes for cvbs\n", map_cvbs_size);
			return -1;
		}
	}
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
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
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
	IMAPX_FBLOG(" %d  %s - bits_per_pixel = %d - \n",__LINE__,__func__,var->bits_per_pixel);
	if (!var || !info)
	{
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
    struct fb_fix_screeninfo *fix = NULL;//fengwu 0424 d-osd
	imapfb_info_t *fbi = (imapfb_info_t*) info;
	int ret = 0;
    u16 xpos, ypos, xsize, ysize, format;//fengwu 0424 d-osd
	IMAPX_FBLOG(" %d  %s  --- \n",__LINE__,__func__);

	if (!info)
	{
		printk(KERN_ERR "[imapfb_set_par]: input argument null\n");
		return -EINVAL;
	}
	
	var = &info->var;
    fix = &info->fix;//fengwu 0424 d-osd

	//Set Visual Color Type
	if (var->bits_per_pixel == 8 || var->bits_per_pixel == 16 || var->bits_per_pixel == 18 || var->bits_per_pixel == 19
		|| var->bits_per_pixel == 24 || var->bits_per_pixel == 25 || var->bits_per_pixel == 28 || var->bits_per_pixel == 32)
		fbi->fb.fix.visual = FB_VISUAL_TRUECOLOR;
	else if (var->bits_per_pixel == 1 || var->bits_per_pixel == 2 || var->bits_per_pixel == 4)
		fbi->fb.fix.visual = FB_VISUAL_PSEUDOCOLOR;
	else
	{
		printk(KERN_ERR "[imapfb_set_par]: input bits_per_pixel invalid\n");
		ret = -EINVAL;
		goto out;
	}

	//Check Input Params
	ret = imapfb_check_var(var, info);
	if (ret)
	{
		printk(KERN_ERR "[imapfb_set_par]: fail to check var\n");
		ret = -EINVAL;
		goto out;
	}	

    //fengwu 0424 d-osd
	if (fbi->win_id == 1) {
		xpos = (var->nonstd>>8) & 0xfff;      //visiable pos in panel
		ypos = (var->nonstd>>20) & 0xfff;
		xsize = (var->grayscale>>8) & 0xfff;  //visiable size in panel
		ysize = (var->grayscale>>20) & 0xfff; 
		format = var->nonstd&0x0f;  // data format 0:rgb 1:yuv 
		osd_set_par(xpos, ypos, xsize, ysize, format, var->xres, fix->smem_start);
	} else if (fbi->win_id == 0) {
		format = var->nonstd&0x0f;  // data format 0:rgb 1:NV21 2:NV12 
	//	osd_set_nv(1, format);
	}
out:
	return ret;
}

int blanking = 0;
EXPORT_SYMBOL(blanking);
int blanking_2 = 0;
EXPORT_SYMBOL(blanking_2);
struct work_struct blank_work;
static struct mutex blanking_mutex;
extern int sync_by_manual;
static void blanking_work_func(struct work_struct *work)
{
	struct timeval s, e;
	mutex_lock(&blanking_mutex);
	if (blanking == 0) {
		blanking_2 = 1;
#ifdef SYSTEM_HDMI_ENABLE
		disable_irq_nosync(GIC_HDMITX_ID);
#endif
		backlight_en_control(0);
		if (!sync_by_manual) {
			terminal_configure("lcd_panel", lcd_vic, 0);
			module_power_down(SYSMGR_IDS1_BASE);
		}
		blanking = 1;
	} else {
		blanking = 0;
		if (!sync_by_manual) {
			module_power_on(SYSMGR_IDS1_BASE);
			ids_writeword(1, IDSINTMSK, 0);
#ifdef SYSTEM_HDMI_ENABLE
			api_Initialize(0, 1, 2500, 1);
#endif
			terminal_configure("lcd_panel", lcd_vic, 1);
		}
		backlight_en_control(1);
#ifdef SYSTEM_HDMI_ENABLE
		enable_irq(GIC_HDMITX_ID);
#endif
		blanking_2 = 0;
	}
	mutex_unlock(&blanking_mutex);
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
	int ret; 
    imapfb_info_t *fbi = (imapfb_info_t *)info;//fengwu 0424 d-osd
	struct clk *clk_ids1, *clk_ids1_osd, *clk_ids1_eitf;

	if (hdmi_state == 1)
		return 0;
#if 1
	clk_ids1 = clk_get_sys("imap-ids.1", "imap-ids1");
	if (!clk_ids1) {
		printk("clk_get(ids.1) failed\n");
		return -1;
	}

	clk_ids1_osd = clk_get_sys("imap-ids1-ods", "imap-ids1");
	if (!clk_ids1_osd) {
		printk("clk_get(ids1_osd) failed\n");
		return -1;
	}

	clk_ids1_eitf = clk_get_sys("imap-ids1-eitf", "imap-ids1");
	if (!clk_ids1_eitf) {
		printk("clk_get(ids1_eitf) failed\n");
		return -1;
	}
#endif

	switch (blank_mode)
	{
		/* Lcd and Backlight on */
		case FB_BLANK_UNBLANK:
			if (blanking == 0)
				return 0;
		//	printk("imapfb_blank: unblank id %d\n", fbi->win_id);
			//		case VESA_NO_BLANKING:
//			blanking = 0;
#if 1 
			if (fbi->win_id == 0 || sync_by_manual) 
			{
				schedule_work(&blank_work);
#if 0
				module_power_on(SYSMGR_IDS1_BASE);
	//			msleep(50);
                          // begin added by linyun.xiong @2015-09-04 ??? for test
#if 0
                          clk_set_rate(clk_ids1, /*297000000*/100000000);
                          clk_set_rate(clk_ids1_osd, /*297000000*/100000000);
                          //clk_set_rate(clk_ids1_eitf, 297000000);
#endif
                          // end added by linyun.xiong @2015-09-04 ??? for test
//				clk_prepare_enable(clk_ids1);
//				clk_prepare_enable(clk_ids1_osd);
//				clk_prepare_enable(clk_ids1_eitf);
//				ids_writeword(1, 0x1090, 0x0);
				ids_writeword(1, IDSINTMSK, 0);
				ret = api_Initialize(0, 1, 2500, 1);
				if(ret == 0) {
					printk("api_Initialize() failed\n");
					return -1;
				}

				terminal_configure("lcd_panel", lcd_vic, 1);
				//				osd_n_enable(1,1,1);  //enable osd1
				backlight_en_control(1);
				enable_irq(GIC_HDMITX_ID);
#endif
			} else {
				// idsx ,osd_num,enable
				osd_n_enable(1,1,1);  //enable osd1
			}
#endif
			//     ids_write(1,0x0,0,1,0);	
			break;
			/* LCD and backlight off */
		case FB_BLANK_POWERDOWN:
		case VESA_POWERDOWN:
			if (blanking == 1)
				return 0;
		//	printk("imapfb_blank: blank id %x\n", fbi->win_id);
#if 1
			if (fbi->win_id == 0 || sync_by_manual) 
			{ 
				schedule_work(&blank_work);
#if 0
				disable_irq_nosync(GIC_HDMITX_ID);
				backlight_en_control(0);
				osd_n_enable(1,1,0);  //disable osd1
				terminal_configure("lcd_panel", lcd_vic, 0);
//				clk_disable_unprepare(clk_ids1_eitf);
//				clk_disable_unprepare(clk_ids1_osd);
//				clk_disable_unprepare(clk_ids1);
				//  ids_writeword(1, 0x1090, 0x1000000);
				module_power_down(SYSMGR_IDS1_BASE);
				blanking = 1;
//				msleep(10);
#endif
			} else {
				// idsx ,osd_num,enable
				osd_n_enable(1,1,0);  //disable osd1
			} 
#endif
			//     ids_write(1,0x0,0,1,0);	
			break ;

		case VESA_VSYNC_SUSPEND:
		case VESA_HSYNC_SUSPEND:
			break;

		default:
			printk(KERN_ERR "[imapfb_blank]: input blank mode %d invalid\n", blank_mode);
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
extern void i80_display_manual(char *mem);
static void imapfb_set_fb_addr(const imapfb_info_t *fbi, struct fb_var_screeninfo *var)
{
	UINT32 video_phy_temp_f1 = fbi->map_dma_f1;	/* framebuffer start address */
	UINT32 start_address;						/* new start address */
	UINT32 start_offset;									/* virtual offset */
	IM_Buffer buffer;
	static int num = 0;
	//int fbx = 0;
	static int fbx = 0; //fengwu alex advice 20150514
	unsigned int fb_phy_addr = 0;
	unsigned int *cpu_addr = 0;

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	//fengwu alex advice 20150514
	//printk(KERN_ERR "var %x fb addr %x\n", var, var->reserved[0]);
	if ((num < IMAPFB_MAX_NUM) && !sync_by_manual){
		num ++;
		return ;
	}

	if (var->reserved[0])
		fb_phy_addr = var->reserved[0]; 	

	//Get Start Address Offset
	start_offset = (fbi->fb.var.xres_virtual * fbi->fb.var.yoffset + fbi->fb.var.xoffset) * imapfb_fimd.bytes_per_pixel;
	//fbx = start_offset?1:0; //fengwu 20150514
	
	//New Start Address with Offset
	start_address = video_phy_temp_f1 + start_offset;

	IMAPX_FBLOG(" %d  %s - map_dma_f1=0x%x ,start_offset=0x%x - \n",__LINE__,__func__,video_phy_temp_f1,start_offset);
	IMAPX_FBLOG(" (%d) -- xres_virtual=%d, yoffset=%d , xoffset=%d -- \n",__LINE__,fbi->fb.var.xres_virtual, fbi->fb.var.yoffset,fbi->fb.var.xoffset);
	buffer.vir_addr = (void *)((UINT32)fbi->map_cpu_f1 + start_offset);
	buffer.phy_addr = start_address;
	buffer.size = fbi->fb.fix.smem_len - start_offset;
	buffer.flag = IM_BUFFER_FLAG_PHY;
	//osd_swap(0, fbx);
	//swap_hdmi_buffer(gmap_dma_f1);
    fbx = fbi->fb.var.yoffset ? 1 : 0;
	if (sync_by_manual) {
		if (fbi->fb.var.yoffset)
			cpu_addr = fbi->map_cpu_f1 + start_offset;
		else 
			cpu_addr = fbi->map_cpu_f1;
		i80_display_manual(cpu_addr);
	} else
		osd_swap(1, fbx, fb_phy_addr, fbi->win_id); //fengwu alex 20150514
    //fbx = fbx?0:1;
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

	if (fbi->win_id != 1 && sync_by_manual)
		return 0;
	IMAPX_FBLOG(" %d  %s - var->xoffset=%d, var->yoffset=%d - \n",__LINE__,__func__,var->xoffset,var->yoffset);
	if (!var || !info)
	{
		printk(KERN_ERR "[imapfb_pan_display]: input arguments null\n");
		return -EINVAL;
	}

	if (var->xoffset + info->var.xres > info->var.xres_virtual)
	{
		printk(KERN_ERR "[imapfb_pan_display]: pan display out of range in horizontal direction\n");
		return -EINVAL;
	}

	if (var->yoffset + info->var.yres > info->var.yres_virtual && var->yoffset < 0x80000000)
	{
		printk(KERN_ERR "[imapfb_pan_display]: pan display out of range in vertical direction\n");
		return -EINVAL;
	}

	fbi->fb.var.xoffset = var->xoffset;
	fbi->fb.var.yoffset = var->yoffset;

	//Check Input Params
	ret = imapfb_check_var(&fbi->fb.var, info);
	if (ret)
	{
		printk(KERN_ERR "[imapfb_pan_display]: fail to check var\n");
		return -EINVAL;
	}

	//imapfb_set_fb_addr(fbi);
    imapfb_set_fb_addr(fbi, var); //fengwu alex 20150514

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

	if (!fbi)
	{
		printk(KERN_ERR "[imapfb_setcolreg]: input info null\n");
		return -EINVAL;
	}

	switch (fbi->fb.fix.visual)
	{
		/* Modify Fake Palette of 16 Colors */ 
		case FB_VISUAL_TRUECOLOR:
			//if (regno < 16)
			if (regno < 256)	/* Modify Fake Palette of 256 Colors */ 
			{				
				unsigned int *pal = fbi->fb.pseudo_palette;

				val = imapfb_chan_to_field(red, fbi->fb.var.red);
				val |= imapfb_chan_to_field(green, fbi->fb.var.green);
				val |= imapfb_chan_to_field(blue, fbi->fb.var.blue);
				val |= imapfb_chan_to_field(transp, fbi->fb.var.transp);			

				pal[regno] = val;
			}
			else
			{
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
static int imapfb_set_win_coordinate(imapfb_info_t *fbi, UINT32 left_x, UINT32 top_y, UINT32 width, UINT32 height)
{
	struct fb_var_screeninfo *var= &(fbi->fb.var);	
	UINT32 win_num = fbi->win_id;

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	if (win_num >= IMAPFB_NUM)
	{
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
static int imapfb_set_win_pos_size(imapfb_info_t *fbi, SINT32 left_x, SINT32 top_y, UINT32 width, UINT32 height)
{
	UINT32 xoffset, yoffset;
	int ret = 0;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
    printk(KERN_ERR " %d  %s -- \n",__LINE__,__func__);//fengwu alex 20150514
	
	if (left_x >= imapfb_fimd.xres)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input left_x invalid, beyond screen width\n");
		return -EINVAL;
	}

	if (top_y >= imapfb_fimd.yres)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input top_y invalid, beyond screen height\n");
		return -EINVAL;
	}

	if (left_x + width <= 0)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input left_x and width too small, out of screen width\n");
		return -EINVAL;
	}

	if (top_y + height <= 0)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: input top_y and height too small, out of screen height\n");
		return -EINVAL;
	}
	
	if (left_x < 0)
	{		
		width = left_x + width;
		xoffset = -left_x;
		left_x = 0;
	}
	
	if (top_y < 0)
	{		
		height = top_y + height;
		yoffset = -top_y;
		top_y = 0;
	}
	
	if (left_x + width > imapfb_fimd.xres)
	{
		width = imapfb_fimd.xres - left_x;
	}
	
	if (top_y + height > imapfb_fimd.yres)
	{
		height = imapfb_fimd.yres - top_y;
	}

	ret = imapfb_set_win_coordinate(fbi, left_x, top_y, width, height);
	if (ret)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: fail to set win%d coordinate\n", fbi->win_id);
		return ret;
	}

	//Check Input Params
	ret = imapfb_check_var(&fbi->fb.var, &fbi->fb);
	if (ret)
	{
		printk(KERN_ERR "[imapfb_set_win_pos_size]: fail to check var\n");
		return ret;
	}
	
	//imapfb_set_fb_addr(fbi);
    // imapfb_set_fb_addr(fbi, &fbi->fb.var); //fengwu 201505124. is this correct?

	return 0;
}

static int imapfb_config_video_size(imapfb_info_t *fbi, imapfb_video_size *video_info)
{
    dtd_t lcd_dtd;
    hdmi_en = 1;
    static int init = 0;
    int ret = 0;
    vic_fill(&lcd_dtd, lcd_vic, 60);
    if (!video_info->win_width || !video_info->win_height) {
        video_info->win_width = video_info->width;
        video_info->win_height = video_info->height;
    }
    if (hdmi_state == 0  && hdmi_connect == 0) {
#ifdef SYSTEM_LCD_DISPLAY_CROP
        video_info->win_width = (video_info->win_width * 80) / 100;
        video_info->win_height = (video_info->win_height * 80) / 100;
#endif
        if (lcd_dtd.mHBorder != 0 && lcd_dtd.mVBorder != 0) {
            if (lcd_dtd.mHBorder * video_info->win_height < lcd_dtd.mVBorder * video_info->win_width)
                video_info->win_width = (video_info->win_height * lcd_dtd.mHBorder) / lcd_dtd.mVBorder;
            else if (lcd_dtd.mHBorder * video_info->win_height > lcd_dtd.mVBorder * video_info->win_width)
                video_info->win_height  = (video_info->win_width * lcd_dtd.mVBorder) / lcd_dtd.mHBorder;
        }
        if ((video_info->win_width == lcd_dtd.mHActive) && (video_info->win_height == lcd_dtd.mVActive)) {
            video_info->win_width -= 8;
            video_info->win_height -= 8;
        }

        ret = imapfb_config_osd0_prescaler(fbi,
                video_info->win_width, video_info->win_height,
                video_info->width, video_info->height,
                video_info->scaler_left_x, video_info->scaler_top_y,
                video_info->scaler_buff, video_info->format);
    } else {
        static unsigned int lcd_width, lcd_height, scaler_out_width, scaler_out_height = 0;
        lcd_width  = lcd_dtd.mHActive;
        lcd_height = lcd_dtd.mVActive;

        ret = osd0_config_pos_size(video_info->win_width, video_info->win_height,
                video_info->width, video_info->height,
                video_info->scaler_buff, video_info->format,
                lcd_width, lcd_height);

        if (!init) {
            hdmi_enable(1);
            init = 1;
        }
    }

    if (ret) {
        printk(KERN_ERR "[imapfb_ioctl]: set win%d position and size error\n", fbi->win_id);
        ret = -EFAULT;
    }
    return 0;
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
	imapfb_color_key_info_t colorkey_info;

    static dma_addr_t suspend_phy_addr = 0;
    static u_char *   suspend_vir_addr = NULL;
    static unsigned int suspend_size = 0;
    static int lcd_suspend_flag = 0;

	struct fb_dmabuf_export dmaexp;
	void __user *argp = (void __user *)arg;
	int ret = 0, rgb_bpp = 32;
	int i, j, pix = 0;
	UINT32 alpha_val = 0,data=0;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);

	if (!fbi)
	{
		printk(KERN_ERR "[imapfb_ioctl]: input info null\n");
		return -EINVAL;
	}
		
	switch(cmd)
	{
        case IMAPFB_OSD_SET_POS_SIZE:
			if (copy_from_user(&win_info, (imapfb_win_pos_size *)arg, sizeof(imapfb_win_pos_size)))
			{
				printk(KERN_ERR "[imapfb_ioctl]: copy win%d position and size info. from user space error\n", fbi->win_id);
				return -EFAULT;
			}

			ret = imapfb_set_win_pos_size(fbi, win_info.left_x, win_info.top_y, win_info.width, win_info.height);
			if (ret)
			{
				printk(KERN_ERR "[imapfb_ioctl]: set win%d position and size error\n", fbi->win_id);
				ret = -EFAULT;
			}
			break;

        case IMAPFB_LCD_SUSPEND:
            if(lcd_suspend_flag)
                break;
            /* ToDo: other pixel format should be considered , default: NV12/NV21 */
            suspend_size = PAGE_ALIGN(video_info.width * video_info.height * 3 >> 1);
		    suspend_vir_addr = dma_alloc_writecombine(fbi->dev, suspend_size, &suspend_phy_addr, GFP_KERNEL);
            if(!suspend_vir_addr) {
                printk(KERN_ERR "[imapfb_ioctl]: alloc suspend buf failed, size: %d\n", suspend_size);
                break;
            }
            memcpy(suspend_vir_addr, phys_to_virt(video_info.scaler_buff), suspend_size);
            video_info.scaler_buff = suspend_phy_addr;
            imapfb_config_video_size(fbi, &video_info);
            lcd_suspend_flag = 1;
            break;

       case IMAPFB_LCD_UNSUSPEND:
            if(lcd_suspend_flag) {
                dma_free_writecombine(fbi->dev, suspend_size, suspend_vir_addr, suspend_phy_addr);
                lcd_suspend_flag = 0;
            }
            break;

        case IMAPFB_CONFIG_VIDEO_SIZE: //reconfig  osd0 & scaler by user's application program
            if (copy_from_user(&video_info, (imapfb_video_size *)arg, sizeof(imapfb_video_size)))
            {
                printk(KERN_ERR "[imapfb_ioctl]: reconfig  win%d position and size info. from user space error\n", fbi->win_id);
                return -EFAULT;
            }
            imapfb_config_video_size(fbi, &video_info);
			break;

		case IMAPFB_SWITCH_ON:
			break;

		case IMAPFB_SWITCH_OFF:
			break;

		case IMAPFB_GET_BUF_PHY_ADDR:
			fb_dma_addr_info.map_dma_f1 = fbi->map_dma_f1;
			fb_dma_addr_info.map_dma_f2 = fbi->map_dma_f2;
			fb_dma_addr_info.map_dma_f3 = fbi->map_dma_f3;
			fb_dma_addr_info.map_dma_f4 = fbi->map_dma_f4;

			if (copy_to_user((void *) arg, (const void *)&fb_dma_addr_info, sizeof(imapfb_dma_info_t)))
			{
				printk(KERN_ERR "[imapfb_ioctl]: copy win%d framebuffer physical address info. to user space error\n", fbi->win_id);
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

		case IMAPFB_COLOR_KEY_GET_INFO:
			colorkey_info.direction = (ids_readword(1, OVCW1CKCR) & (1 << 24)) ? 1 : 0;
			colorkey_info.colval = ids_readword(1, OVCW1CKR);
			ret = copy_to_user(argp, &colorkey_info, sizeof(imapfb_color_key_info_t)) ? -EFAULT : 0;
			break;

		case IMAPFB_COLOR_KEY_SET_INFO:
			if (copy_from_user(&colorkey_info, argp, sizeof(imapfb_color_key_info_t)))
				return -EFAULT;
			if (item_exist("dss.implementation.ids1.rgbbpp"))
				rgb_bpp = item_integer("dss.implementation.ids1.rgbbpp", 0);
			ids_write(1, OVCW1CKCR, 26, 1, 0);
			ids_write(1, OVCW1CKCR, 24, 1, colorkey_info.direction);
			ids_write(1, OVCW1CKCR, 25, 1, 1);
			if (rgb_bpp = 16) {
				pix = ((colorkey_info.colval & 0x1f) << 3);
				pix |= (((colorkey_info.colval >> 5) & 0x1f) << 11);
				pix |= (((colorkey_info.colval >> 10) & 0x1f) << 19);
				colorkey_info.colval = pix;
			}
			ids_writeword(1, OVCW1CKR, colorkey_info.colval);
			if(item_exist(P_HDMI_EN) && (item_integer(P_HDMI_EN,0) == 1))
			{
				msleep(1000);//delay for hdmi display bug.
				hdmi_app_display =1;
			}
			break;
		//ALPHA function.	
		case IMAPFB_COLOR_KEY_ALPHA_START:
			ids_write(1, OVCW1CR, OVCWxCR_BLD_PIX, 1, 0);//OVCW1CR is ids1
			ids_write(1, OVCW1CR, OVCWxCR_ALPHA_SEL, 1, 0);//0 use IMAPFB_OSD_ALPHA0_SET;1 use IMAPFB_OSD_ALPHA1_SET
			ids_write(1, OVCW1CKCR, 26, 1, 1);	
		      // ids_writeword(1, OVCW1PCCR, 0xfff111);
			break;
			
		case IMAPFB_COLOR_KEY_ALPHA_STOP:
			ids_write(1, OVCW1CKCR, 26, 1, 0);
			break;
			
		case IMAPFB_OSD_ALPHA0_SET:
			if (copy_from_user(&alpha_val, argp, sizeof(UINT32)))
				return -EFAULT;
			data = ids_readword(1, OVCW1PCCR);
			data =((alpha_val&0xfff))|(data&0xfff000);
				
			ids_writeword(1, OVCW1PCCR, data);//small is more transparency.
			break;

		case IMAPFB_OSD_ALPHA1_SET:
			if (copy_from_user(&alpha_val, argp, sizeof(UINT32)))
				return -EFAULT;
			data = ids_readword(1, OVCW1PCCR);
			data =((alpha_val&0xfff)<<12)|(data&0x000fff);
			ids_writeword(1, OVCW1PCCR, data);
			break;	
			
		default:
			printk(KERN_ERR "[imapfb_ioctl]: unknown command type\n");
			return -EFAULT;
	}

	return 0;
}

/* Framebuffer operations structure */
struct fb_ops imapfb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var		= imapfb_check_var,
	.fb_set_par		= imapfb_set_par, // may called after imapfb_init_registers
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

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	if (!finfo)
	{
		printk(KERN_ERR "[imapfb_init_fbinfo]: input finfo null\n");
		return;
	}

	if (index >= IMAPFB_NUM)
	{
		printk(KERN_ERR "[imapfb_init_fbinfo]: input index invalid\n");
		return;
	}

	strcpy(finfo->fb.fix.id, drv_name);

	finfo->win_id = index;
	finfo->fb.fix.type = FB_TYPE_PACKED_PIXELS;
	finfo->fb.fix.type_aux = 0;
	finfo->fb.fix.xpanstep = 0;
	finfo->fb.fix.ypanstep = 1;
	finfo->fb.fix.ywrapstep = 0;
	finfo->fb.fix.accel = FB_ACCEL_NONE;
	finfo->fb.fix.mmio_start = IMAP_IDS1_BASE; //LCD_BASE_REG_PA; //sam :
	finfo->fb.fix.mmio_len = SZ_16K;

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
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual * finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	//printk("Double FrameBuffer.\n");
#else
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual * finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	//printk("Single FrameBuffer.\n");
#endif
	finfo->fb.fix.line_length = finfo->fb.var.xres * imapfb_fimd.bytes_per_pixel;

	for (i = 0; i < 256; i++)
		finfo->palette_buffer[i] = IMAPFB_PALETTE_BUFF_CLEAR;
}

extern void imapx_bl_power_on(void);


static int __init imapfb_probe(struct platform_device *pdev)
{
	struct fb_info *fbinfo;
	imapfb_info_t *info;
	UINT32 index;
	int ret;
	int vic[40];
	int box_vic;
	dtd_t dtd;
	char* uboot_logo_pix_data = NULL;
	int ubootLogoState = 1; //0-disabled; 1-enabled;
	int bmp_width, bmp_height;
    struct resource *res;
	int size;

	//printk(KERN_INFO "[imapfb_probe]: Imap Framebuffer Driver Initialization Start!\n");
	
	if (!pdev)
	{
		printk(KERN_ERR "[imapfb_probe]: input argument null\n");
		return -EINVAL;
	}

	//Allocate one Framebuffer Structure
	fbinfo = framebuffer_alloc(sizeof(imapfb_info_t), &pdev->dev);
	if (!fbinfo)
	{
		printk(KERN_ERR "[imapfb_probe]: fail to allocate framebuffer\n");
	   	ret = -ENOMEM;
	   	goto dealloc_fb;
   	}

	platform_set_drvdata(pdev, fbinfo);
	info = fbinfo->par;
	info->dev = &pdev->dev;
// add by shuyu
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get memory registers\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}
	size = resource_size(res);
	info->mem = request_mem_region(res->start, size, pdev->name);
	if (info->mem == NULL) {
		dev_err(&pdev->dev, "failed to get memory region\n");
		ret = -ENOENT;
		goto dealloc_fb;
	}

	info->io = ioremap(res->start, size);
	if (info->io == NULL) {
		dev_err(&pdev->dev, "ioremap() of registers failed\n");
		ret = -ENXIO;
		goto release_mem;
	}
//end add by shuyu
// add by shuyu	
   	if(item_exist("lcd.vic")){                               
		lcd_vic = item_integer("lcd.vic",0); 
	    //printk(KERN_INFO "get lcd.vic %d\n",lcd_vic);
	}else{
	   printk(KERN_INFO "item lcd.vic is not exist \n");
	}
//end add by shuyu
// add by shuyu	
   	if(item_exist("prescaler.vic")){                               
		prescaler_vic = item_integer("prescaler.vic",0); 
	    //printk(KERN_INFO "get prescaler.vic %d\n",prescaler_vic);
	}else{
	   printk(KERN_INFO "item prescaler.vic is not exist \n");
	}
//end add by shuyu

  //if (pt == PRODUCT_MID)
	//	main_VIC = lcd_vic;
	//else if (pt == PRODUCT_BOX) {
	if (pt == PRODUCT_MID) { //fengwu 20150514
		main_VIC = prescaler_vic;  //add by shuyu
		//   main_VIC = 2001;
		//	main_VIC = lcd_vic;
	} else if (pt == PRODUCT_BOX) {
		main_VIC = 4;	//720p
	}

	main_scaler_factor = 100;
	//vic_fill(&dtd, main_VIC, 60);
    vic_fill(&dtd, lcd_vic, 60); //fengwu alex 20150514

	
	//printk("main_VIC = %d, main_scaler_factor = %d, mHActive = %d, mVActive = %d, FrameBuffer Size: %d * %d\n", 
	//		main_VIC, main_scaler_factor, dtd.mHActive, dtd.mVActive, dtd.mHActive, dtd.mVActive);
    //printk("lcd_vic = %d, main_scaler_factor = %d, mHActive = %d, mVActive = %d, FrameBuffer Size: %d * %d\n", 
     //      lcd_vic, main_scaler_factor, dtd.mHActive, dtd.mVActive, dtd.mHActive, dtd.mVActive);


	INIT_WORK(&blank_work, blanking_work_func);
	mutex_init(&blanking_mutex);
	for (index = 0; index < 2 /*IMAPFB_NUM*/; index++) //fengwu 0424 d-osd
	{
		fb_screen_param_init(&dtd, index);

		imapfb_info[index].mem = info->mem;
		imapfb_info[index].io = info->io;
		imapfb_info[index].clk1 = info->clk1;
		imapfb_info[index].clk2 = info->clk2;
		
		imapfb_init_fbinfo(&imapfb_info[index], "imapfb", index);
		/* Initialize video memory */
		ret = imapfb_map_video_memory(&imapfb_info[index]);
		if (ret)
		{
			printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate framebuffer video ram\n", index);
			ret = -ENOMEM;
			//goto release_clock2;
			goto release_io;
		}

		ret = imapfb_check_var(&imapfb_info[index].fb.var, &imapfb_info[index].fb);
		if (ret)
		{
			printk(KERN_ERR "[imapfb]<ERR>: win %d fail to check var\n", index);
			ret = -EINVAL;
			goto free_video_memory;
		}
		
		if (index < 2)
		{
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 256, 0) < 0)
			{
				printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		}
		else
		{
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 16, 0) < 0)
			{
				printk(KERN_ERR "[imapfb]<ERR>: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		}
		
		//printk(KERN_ALERT "[imapfb]<DEBUG>: Before register frame buffer.\n");
		ret = register_framebuffer(&imapfb_info[index].fb);
		//printk(KERN_ALERT "[imapfb]<DEBUG>: After register frame buffer.\n");
		if (ret < 0)
		{
			printk(KERN_ERR "[imapfb]<ERR>: Failed to register framebuffer device %d\n", ret);
			goto dealloc_cmap;
		}

		//printk(KERN_INFO "fb%d: %s frame buffer device\n", imapfb_info[index].fb.node, imapfb_info[index].fb.fix.id);
	}

	/* 
	 *(1)copy uboot-logo data from rballoc 
	 *(2)destination:map_cpu_f1
	 */
    if(item_exist("config.uboot.logo")){                                              
		unsigned char str[ITEM_MAX_LEN] = {0};                                            
		item_string(str,"config.uboot.logo",0);                                       
		if(strncmp(str,"0",1) == 0 )                                           
			ubootLogoState = 0;//disabled                                      
	}        
    
	if (1 == ubootLogoState) {
		uboot_logo_pix_data = rbget("idslogofb");
		if( NULL != uboot_logo_pix_data){
			memcpy(imapfb_info[0].map_cpu_f1,uboot_logo_pix_data,dtd.mHActive* dtd.mVActive*imapfb_fimd.bytes_per_pixel );
		}else{
		
		    printk(KERN_ALERT "[imapfb]<DEBUG>: uboot_logo_pix_data == NULL \n");
		}
	}
	//printk(KERN_INFO "Imap Framebuffer Driver Initialization OK!\n");
// add by shuyu	
   	if(item_exist("lcd.interface")){                               
		lcd_interface = item_integer("lcd.interface",0); 
	    //printk(KERN_INFO "init lcd default interface %d!\n",lcd_interface);
	}else{
	   printk(KERN_INFO "error lcd.interface is not exist \n");
	}
//end add by shuyu
	// Temp for test
	if (pt == PRODUCT_BOX) {
		char *p = NULL; //getenv("vic");
		if (p == NULL) {
			box_vic = 16;	//1080p
		} else {
			box_vic = display_atoi(p);
			//printk(KERN_ERR "get vic %d by env\n", box_vic);
		}
#ifdef SYSTEM_HDMI_ENABLE
		hdmi_get_vic(vic, 64);
        terminal_configure("hdmi_sink", box_vic, 1);
		//fill_framebuffer_logo((u32)map_cpu_cvbs, &bmp_width, &bmp_height);
#endif		
		fill_framebuffer_logo((u32)imapfb_info[1].map_cpu_f1 , &bmp_width, &bmp_height);
		terminal_configure("cvbs_sink", cvbs_style?3:18, 1);
		//swap_cvbs_buffer(map_dma_cvbs);
	} else if (pt == PRODUCT_MID) {
		 /*                                                                             
		   *(1)Author:summer                                                             
		   *(2)Date:2014-6-26                                                            
		   *(3)Reason:                                                                   
		   *  The logo from uboot to kernel can't be continous,the modification is    
		   *  required by alex.                                                          
		 * */     

		/*
		static int i, j = 0;
		for (i = 0; i < 800; i++)
			for (j = 0; j < 480; j++) {
				if (i < 250)
				*((volatile unsigned int *)(u32)imapfb_info[1].map_cpu_f1 + j + i*480) = 0x00ff0000;
				else if (i < 500)
				*((volatile unsigned int *)(u32)imapfb_info[1].map_cpu_f1 + j + i*480) = 0x0000ff00;
				else if (i < 800)
				*((volatile unsigned int *)(u32)imapfb_info[1].map_cpu_f1 + j + i*480) = 0x000000ff;
			}
			*/
	
			//hdmi display logo 1920x1080 bpp=16
			//./anatur_tool_rgb555 1920 1080
			//antaur.h
			 if(item_exist(P_HDMI_EN) && (item_integer(P_HDMI_EN,0) == 1))
			 {
				fill_framebuffer_hdmi_logo((u32)imapfb_info[1].map_cpu_f1 , &bmp_width, &bmp_height);
				hdmi_en = 1;
				hdmi_enable(1);
			}
			else
			{
				fill_framebuffer_logo((u32)imapfb_info[1].map_cpu_f1 , &bmp_width, &bmp_height);
				terminal_configure("lcd_panel", lcd_vic, 1); //fengwu alex 20150514
			}
		
	}
	return 0;

dealloc_cmap:
	fb_dealloc_cmap(&imapfb_info[index].fb.cmap);

free_video_memory:
	imapfb_unmap_video_memory(&imapfb_info[index]);

release_io:
	iounmap(info->io);
release_mem:
	release_mem_region(res->start, size);
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
	for (index = 0; index < IMAPFB_NUM; index++)
	{
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

static struct platform_driver imapfb_driver = {
	.probe		= imapfb_probe,
	.remove		= imapfb_remove,
	.driver		= {
		.name	= "imap-fb",
		.owner	= THIS_MODULE,
	},
};

int __init imapfb_init(void)
{
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	return platform_driver_register(&imapfb_driver);
}
static void __exit imapfb_cleanup(void)
{
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	platform_driver_unregister(&imapfb_driver);
}

module_init(imapfb_init);
module_exit(imapfb_cleanup);

MODULE_AUTHOR("Sam Ye");
MODULE_DESCRIPTION("IMAP Framebuffer Driver");
MODULE_LICENSE("GPL");

