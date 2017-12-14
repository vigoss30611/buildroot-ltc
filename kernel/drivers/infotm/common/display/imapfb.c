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
#include <imapx800_g2d.h>
#include <mach/rballoc.h>
#include <mach/nvedit.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/param.h>

#include <mach/rballoc.h>
//#include <InfotmMedia.h>
//#include <IM_buffallocapi.h>
//#include <IM_types.h>
#include "imapfb.h"
//#include <IM_idsapi.h>
//#include "dss/hdmi/ids_lib.h"

//#include "logo_1024_768.h"
//#include "logo_800_480.h"

//#include "dss/hdmi/edid/edid.h"
//#include "dss/logo_bmp.h"
//#include "dss/imapx800_ids.h"
//#include "dss/hdmi/edid/dtd.h"

#include <mach/items.h>
#include <dss_common.h>


// defination instead of ids_lib.h
//typedef void * fblayer_handle_t;
//typedef void * wlayer_handle_t;
//typedef void * vmode_handle_t;


//#define UPALIGN_SIZE(size)	(((size) + (PMM_PAGE_SIZE - 1)) & ~(PMM_PAGE_SIZE -1))

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

extern int hdmi_VICs[6];
int use_main_fb_resize;
unsigned int main_VIC;
unsigned int main_scaler_factor;

imapfb_fimd_info_t imapfb_fimd;
//static di_screen_info_t  gDI_Info;



extern int swap_cvbs_buffer(u32 phy_addr);
extern int osd_dma_addr(int idsx, dma_addr_t addr, int frame_size, int fbx);
extern int osd_swap(int idsx, int fbx);
extern int terminal_configure(char * terminal_name, int vic, int enable);
extern int swap_hdmi_buffer(u32 phy_addr);
extern struct g2d * g2d_data_global;
extern int g2d_work(int idsx, int VIC, int factor_width,  
		        int factor_height, int main_fbx, int * g2d_fbx_p);
extern int g2d_convert_logo(unsigned int bmp_width, unsigned int bmp_height, int fbx);
extern int pt;
extern int cvbs_style;
extern int hdmi_get_vic(int a[], int len);
extern int backlight_en_control(int enable);
extern int ids_writeword(int idsx, unsigned int addr, unsigned int val);
extern int hdmi_hplg_flow(int en);
extern int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height);
extern int display_atoi(char *s);



//int pmmdrv_open(void *phandle, char *owner);
//int pmmdrv_mm_alloc(void * handle, int size, int flag, alc_buffer_t *alcBuffer);

#if 0
int ids_fblayer_init(fblayer_handle_t *handle, int buf_num, di_screen_info_t *info);
int ids_fblayer_deinit(fblayer_handle_t handle);
int ids_fblayer_open(fblayer_handle_t handle);
int ids_fblayer_close(fblayer_handle_t handle);
int ids_fblayer_swap_buffer(fblayer_handle_t handle, IM_Buffer *buffer);
int ids_fblayer_set_position(fblayer_handle_t handle,unsigned int left_x,unsigned int top_y,unsigned int width,unsigned int height);
#endif

void * fb_pmmHandle = NULL;

//Imap Framebuffer Stucture Array
imapfb_info_t imapfb_info[IMAPFB_NUM];
/* Set RGB IF Data Line and Control Singal Line */


int fill_fb_something(unsigned char * fb_base, unsigned int frame_size, 
							unsigned int width, unsigned int height,  int fbx)
{
	int i, j, k, l, m;
	int lines;
	unsigned int color;
	unsigned int pixel;
	unsigned int fb_des = (unsigned int)fb_base + frame_size * fbx;
	dma_addr_t tmp;

	printk("fill_fb_something. fbx = %d, width = %d\n", fbx, width);
#if 1
	/* All combination of R,G,B part */
	lines = height/8;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			for (k = 0; k < 2; k++) {
				//pixel = 0 << 24 | (0xFF*i) << 16 | (0xFF*j) << 8 | (0xFF*k) << 0;
				for (l = 0; l < lines; l++) {
					color = 0xFF -(256*l/lines);
					pixel = 0 << 24 | (color*i) << 16 | (color*j) << 8 | (color*k) << 0;
					for (m = 0; m < width; m++) {
						writel(pixel, fb_des +(((i*4+j*2+k*1)*lines+l)*width+m)*4);					
					}
				}
			}
#endif
#if 0
	/* R - G - B  three part */
	lines = height/3;
	for (i = 0; i < 3; i++)
		for (j = 0; j < lines; j++) {
			pixel = (0xFF-(256*j/lines)) << (8*(2-i));
			for (k = 0; k < width; k++) {
				writel(pixel, (u32 *)(fb_des + ((i*lines + j) * width  + k) * 4));
			}
		}
#endif
#if 0
	/* All Red color */
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			writel(0x00FF0000, fb_des + (i*width+j)*4);
#endif
#if 0
	/* All white color */
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			writel(0x00FFFFFF, fb_des + (i*width+j)*4);
#endif
#if 0
	/* All black color */
	memset(fb_des, 0, frame_size);
#endif
	/* Write cache to memory */
	tmp = dma_map_single(NULL, (void *)fb_des, frame_size, DMA_TO_DEVICE);
	dma_unmap_single(NULL, tmp, frame_size, DMA_TO_DEVICE);

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
        //idsdrv_fblayer_close(imapfb_info[0].fbhandle);
}
EXPORT_SYMBOL(imapfb_shutdown);

#if 0
static int new_logo = 0;
static int new_logo_exist(unsigned char *p)
{
	typedef struct{
		unsigned char scan;
		unsigned char gray;
		unsigned short w;
		unsigned short h;
		unsigned char is565;
		unsigned char rgb;
	}logo_head_t;

	logo_head_t *header_p;

	if( NULL == p)
		return 0;
	header_p = (logo_head_t *)(p);
	if (header_p->gray == 0x10 && header_p->is565 == 0x1){
		printk("#current logo formate is RGB565#\n");
		return 1;
	}else if(header_p->gray == 0x20){
		printk("#current logo formate is RGB888#\n");
		return 1;
	}

	return 0;
}
#endif

// to get the framebuffer relative paramters form item.
// if no parameters got, use the default values.
static int fb_screen_param_init(dtd_t *dtd)
{
	struct clk *ids0_clk;
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
#if 1
	ids0_clk = clk_get_sys("imap-ids0-eitf", "imap-ids0");
	pixel_clock = clk_get_rate(ids0_clk);
	printk("real pixel clock = %d\n", pixel_clock);
	refreshRate = pixel_clock / (dtd->mHActive + dtd->mHBlanking)  * 1000 / (dtd->mVActive + dtd->mVBlanking);
	printk("refreshRate = %d\n", refreshRate);
	imapfb_fimd.pixclock = 1000000000 / (dtd->mHActive + dtd->mHBlanking) * 1000
		/(dtd->mVActive + dtd->mVBlanking) * 1000 / refreshRate;
	printk("imapfb_fimd.pixclock = %d\n", imapfb_fimd.pixclock);
#else
	// pixel clock in pico second 
	printk("mPixelClock = %d\n", dtd->mPixelClock);
	refreshRate = (u32)10000000 /(dtd->mHActive + dtd->mHBlanking) 
		* dtd->mPixelClock / (dtd->mVActive + dtd->mVBlanking);
	printk("refreshRate = %d\n", refreshRate);
	imapfb_fimd.pixclock = 1000000000 / (dtd->mHActive + dtd->mHBlanking)
		/ (dtd->mVActive + dtd->mVBlanking) * 1000000 / refreshRate;
	printk("imapfb_fimd.pixclock = %d\n", imapfb_fimd.pixclock);
#endif
	imapfb_fimd.bpp = 32;
	imapfb_fimd.bytes_per_pixel = 4;

	imapfb_fimd.xres = dtd->mHActive;
	imapfb_fimd.yres = dtd->mVActive;
	imapfb_fimd.osd_xres = imapfb_fimd.xres;
	imapfb_fimd.osd_yres = imapfb_fimd.yres;
	imapfb_fimd.osd_xres_virtual = imapfb_fimd.osd_xres; 
	imapfb_fimd.osd_yres_virtual = imapfb_fimd.osd_yres * 2;

	printk("width=%d, height=%d,bpp=%d,hblank=%d,hfpd=%d,hbpd=%d,hspw=%d,vblank=%d,vfpd=%d,vbpd=%d,vspw=%d, pixclock=%d\n",
			imapfb_fimd.xres, imapfb_fimd.yres, imapfb_fimd.bpp, 
			(imapfb_fimd.left_margin + imapfb_fimd.right_margin + imapfb_fimd.hsync_len),
			imapfb_fimd.left_margin, imapfb_fimd.right_margin, imapfb_fimd.hsync_len, 
			(imapfb_fimd.upper_margin + imapfb_fimd.lower_margin + imapfb_fimd.vsync_len),
			imapfb_fimd.upper_margin, imapfb_fimd.lower_margin, imapfb_fimd.vsync_len, imapfb_fimd.pixclock);
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
EXPROT_SYMBOL(gmap_dma_f1);
EXPROT_SYMBOL(gmap_cpu_f1);
dma_addr_t		map_dma_cvbs;	/* physical */
u_char *			map_cpu_cvbs;	/* virtual */
int map_cvbs_size;
static int imapfb_map_video_memory(imapfb_info_t *fbi)
{
	int ret;
	alc_buffer_t alcBuffer;
	unsigned int flag;
	printk("imapfb_map_video_memory size= 0x%X -- (%d Bytes)\n",fbi->fb.fix.smem_len, fbi->fb.fix.smem_len);
	//Allocate framebuffer memory and save physical address, virtual address and size
	fbi->map_size_f1 = PAGE_ALIGN(fbi->fb.fix.smem_len);
#if 0
        /* Open pmmdrv handle also for G2D. */
        ret = pmmdrv_open(&fb_pmmHandle, "idsdrv");
        if(ret != 0){
                printk("ERR: imapfb pmmdrv_open failed ");
                return -ENOMEM;
        }
        /* Modified by Sololz. */
        if (item_exist("memory.highres") && item_integer("smtv.ids.videoformat", 0) == 3) {
                /* 
                 * FIXME:
                 * For 1080p test, split double framebuffer to be located on different 
                 * memory bank. On IMAPX8xx chip, a bank is 128MB size. So to this method
                 * will wast 256MB-(2*fbsize).
                 * Actually, we find that GPU will greatly affect IDS while accessing 
                 * DDR. So this method is to divide IDS and GPU to avoid accessing the
                 * same memory bank simultaneously.
                 * ... | bank 6 | bank 7 |
                 *       ... 8MB|8MB ...
                 *        ... f0|f1 ...
                 * So, while IDS is using *f1*, GPU can only access *f0*. This will guarantee
                 * the same memory bank can't be accessed by GPU and IDS at the same time.
                 *
                 * In order to reserve the last 256MB memory, the Uboot mem param must be
                 * modified.
                 */
                /* 
                 * Fuckin fuck framebuffer size is 1920*1080*4 a bit less than 8MB. So
                 * here use 16MB 
                 */
                unsigned long rsvfb_phys = 0xb8000000 - (8 * 1024 * 1024);
                unsigned long rsvfb_size = 16 * 1024 * 1024;
                unsigned long fbintv = (rsvfb_size - fbi->fb.fix.smem_len) / 2;
                void __iomem * rsvfb_virt = ioremap_cached(rsvfb_phys, rsvfb_size);
                if (rsvfb_virt == NULL) {
                        printk(KERN_ALERT "Map reserved framebuffer error!\n");
                        return -EFAULT;
                }
                fbintv = (fbintv + 4095) & (~4095);
                fbi->map_cpu_f1 = (void *)((unsigned long)rsvfb_virt + fbintv);
                fbi->map_dma_f1 = rsvfb_phys + fbintv;
                printk(KERN_ALERT "DDR bank mode for IDS framebuffer.\n");
        } else {
                // FIXME: for
#ifdef CONFIG_INFOTM_MEDIA_PMM_I800_SUPPORT
                //	fbi->map_cpu_f1 = gDI_Info.buffer.vir_addr;
                //	fbi->map_dma_f1 = gDI_Info.buffer.phy_addr;
                printk("Allocating FB mem from pmmdrv.\n");
                flag = ALC_FLAG_PHY_MUST | ALC_FLAG_DEVADDR;
                ret = pmmdrv_mm_alloc(fb_pmmHandle, fbi->map_size_f1, flag, &alcBuffer);
                if (ret == -1){
                        printk("ERR: imapfb pmmdrv_mm_alloc failed -- \n");
                        return -ENOMEM;
                }
                fbi->map_cpu_f1 = alcBuffer.buffer.vir_addr;
                fbi->map_dma_f1 = alcBuffer.buffer.phy_addr;
#else
                fbi->map_cpu_f1 = dma_alloc_writecombine(fbi->dev, fbi->map_size_f1, &fbi->map_dma_f1, GFP_KERNEL);
#endif
        }
#endif

#if 0
	printk("order %d\n", get_order(fbi->map_size_f1));
	fbi->map_cpu_f1 = __get_free_pages(GFP_KERNEL| __GFP_DMA32|__GFP_NOFAIL, get_order(fbi->map_size_f1));
	if (fbi->map_cpu_f1)
		fbi->map_dma_f1 = virt_to_phys(fbi->map_cpu_f1);
#endif
#if 0
	fbi->map_cpu_f1 = kzalloc(fbi->map_size_f1, GFP_KERNEL | __GFP_NOFAIL);
	if (fbi->map_cpu_f1)
		fbi->map_dma_f1 = virt_to_phys(fbi->map_cpu_f1);
#endif
#if 0
	fbi->map_cpu_f1 = dma_alloc_coherent(fbi->dev, fbi->map_size_f1, &fbi->map_dma_f1, GFP_KERNEL | __GFP_NOFAIL);
#endif
#if 1
	fbi->map_cpu_f1 = dma_alloc_writecombine(fbi->dev, fbi->map_size_f1, &fbi->map_dma_f1, GFP_KERNEL);
#endif
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
	//ids_set_main_fb_addr(fbi->map_dma_f1, fbi->map_size_f1);
//	fill_fb_something(fbi->map_cpu_f1, fbi->map_size_f1/2, imapfb_fimd.osd_xres, imapfb_fimd.osd_yres, 1);
	osd_dma_addr(0, fbi->map_dma_f1, fbi->map_size_f1/2, 0);
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
	imapfb_info_t *fbi = (imapfb_info_t*) info;
	int ret = 0;
	IMAPX_FBLOG(" %d  %s  --- \n",__LINE__,__func__);

	if (!info)
	{
		printk(KERN_ERR "[imapfb_set_par]: input argument null\n");
		return -EINVAL;
	}
	
	var = &info->var;

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
int blanking = 0;
EXPORT_SYMBOL(blanking);
static int imapfb_blank(int blank_mode, struct fb_info *info)
{
	//imapfb_info_t *fbi = (imapfb_info_t *)info;
	switch (blank_mode)
	{
		/* Lcd and Backlight on */
		case FB_BLANK_UNBLANK:
		/* also VESA_NO_BLANKING */
			if(!blanking)
				return 0;

			blanking = 0;
			ids_writeword(1, 0x1090, 0x0);

			terminal_configure("lcd_panel", LCD_VIC, 1);
            backlight_en_control(1);
			printk("imapfb_blank: unblank\n");
			break;

		/* LCD and backlight off */
		case FB_BLANK_POWERDOWN:
		case VESA_POWERDOWN:
			blanking = 1;
            backlight_en_control(0);

			terminal_configure("lcd_panel", LCD_VIC, 0);
			ids_writeword(1, 0x1090, 0x1000000);

			printk("imapfb_blank: blank\n");
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
static void imapfb_set_fb_addr(const imapfb_info_t *fbi)
{
	UINT32 video_phy_temp_f1 = fbi->map_dma_f1;	/* framebuffer start address */
	UINT32 start_address;						/* new start address */
	UINT32 start_offset;									/* virtual offset */
	IM_Buffer buffer;
	static int num = 0;
	int fbx = 0;
	int fb_num;
	//fblayer_handle_t handle = fbi->fbhandle;

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	if (num < 10){
		num ++;
		return ;
	}
	
	//Get Start Address Offset
	start_offset = (fbi->fb.var.xres_virtual * fbi->fb.var.yoffset + fbi->fb.var.xoffset) * imapfb_fimd.bytes_per_pixel;
	fbx = start_offset?1:0;
	
	//New Start Address with Offset
	start_address = video_phy_temp_f1 + start_offset;

	IMAPX_FBLOG(" %d  %s - map_dma_f1=0x%x ,start_offset=0x%x - \n",__LINE__,__func__,video_phy_temp_f1,start_offset);
	IMAPX_FBLOG(" (%d) -- xres_virtual=%d, yoffset=%d , xoffset=%d -- \n",__LINE__,fbi->fb.var.xres_virtual, fbi->fb.var.yoffset,fbi->fb.var.xoffset);
	buffer.vir_addr = (void *)((UINT32)fbi->map_cpu_f1 + start_offset);
	buffer.phy_addr = start_address;
	buffer.size = fbi->fb.fix.smem_len - start_offset;
	buffer.flag = IM_BUFFER_FLAG_PHY;
	//ids_fblayer_swap_buffer(handle, &buffer);
	//ids_swap_framebuffer(fbx);
	osd_swap(0, fbx);
//	g2d_work(1, 4, 100, 100, !fbx, &fb_num);            
#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
	g2d_convert_logo(800, 480, fbx);
	swap_hdmi_buffer(g2d_data_global->main_fb_phyaddr);
#else
	swap_hdmi_buffer(gmap_dma_f1);
#endif
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

	imapfb_set_fb_addr(fbi);

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
//	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
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
	//fblayer_handle_t handle = fbi->fbhandle;

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	if (win_num >= IMAPFB_NUM)
	{
		printk(KERN_ERR "[imapfb_onoff_win]: input win id %d invalid\n", fbi->win_id);
		return -EINVAL;
	}
		
	//ids_fblayer_set_position(handle,left_x,top_y,width,height);

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
	
	imapfb_set_fb_addr(fbi);

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
	imapfb_dma_info_t fb_dma_addr_info;
	struct fb_dmabuf_export dmaexp;
	void __user *argp = (void __user *)arg;
	int ret = 0;
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

		case IMAPFB_SWITCH_ON:
			//ids_fblayer_open(fbi->fbhandle);
			break;

		case IMAPFB_SWITCH_OFF:
			//ids_fblayer_close(fbi->fbhandle);
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
	finfo->fb.fix.mmio_start = 0x22000000; //LCD_BASE_REG_PA; //sam :
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
	//finfo->fb.fix.smem_len = 2 * finfo->fb.var.xres_virtual * finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	// The osd_yres_virtual has already plus 2.
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual * finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	printk("Double FrameBuffer.\n");
#else
	finfo->fb.fix.smem_len = finfo->fb.var.xres_virtual * finfo->fb.var.yres_virtual * imapfb_fimd.bytes_per_pixel;
	printk("Single FrameBuffer.\n");
#endif
	finfo->fb.fix.line_length = finfo->fb.var.xres * imapfb_fimd.bytes_per_pixel;

	for (i = 0; i < 256; i++)
		finfo->palette_buffer[i] = IMAPFB_PALETTE_BUFF_CLEAR;
}

#if 1
static int __init imapfb_probe(struct platform_device *pdev)
{
//	struct resource *res;
	struct fb_info *fbinfo;
	imapfb_info_t *info;
	UINT8 driver_name[] = "imapfb";
	UINT32 index;
//	UINT32 size;
	int ret;
	int vic[40];
	int box_vic;
//	fblayer_handle_t handle ;
//	UINT32 start_offset, vir_addr;									/* virtual offset */
//	IM_Buffer buffer;
//	unsigned char *hex_from_bin=NULL, *logo_data = NULL; 
	dtd_t dtd;
	int hdmi_format = 0;
	int hdmi_scaler[2] = {95,95};
	char* uboot_logo_pix_data = NULL;
	int tmp;
	int ubootLogoState = 1; //0-disabled; 1-enabled;

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	printk(KERN_INFO "[imapfb_probe]: Imap Framebuffer Driver Initialization Start!\n");
	
	//Check Input Argument
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

#if 0
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
	{
		printk(KERN_ERR "[imapfb_probe]: fail to get io registers\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}
	size = (res->end - res->start) + 1;
	printk("res->start = 0x%X, size = 0x%X, pdev->name = %s\n", res->start, size, pdev->name);
	info->mem = request_mem_region(res->start, size, pdev->name);
	if (!info->mem)
	{
		printk(KERN_ERR "[imapfb_probe]: fail to get memory region\n");
		ret = -ENOENT;
		goto dealloc_fb;
	}
	//info->io = ioremap(res->start, size);
	info->io = ioremap_nocache(res->start, size);
	if (!info->io)
	{
		printk(KERN_ERR "[imapfb_probe]: ioremap of registers fail\n");
		ret = -ENXIO;
		goto release_mem;
	}
#endif

#if 0
	/* Set default values */
	use_main_fb_resize = 1;
	main_VIC = 4;
	main_scaler_factor = 100;

	/* Check if need to use main framebuffer resize function */
	if (!item_exist(MAIN_FB_RESIZE)) {
		printk("Error: item %s does not exist.\n", MAIN_FB_RESIZE);
	}
	else {

		use_main_fb_resize = item_integer(MAIN_FB_RESIZE, 0);
		printk("%s = %d\n", MAIN_FB_RESIZE, use_main_fb_resize);
	}
	
	/* Get items */
	if (use_main_fb_resize) {
		if (!item_exist(P_VF) || !item_exist(P_VSH)) {
			printk("Error: item %s or item %s does not exist.\n", P_VF, P_VSH);
		}
		else {
			hdmi_format = item_integer(P_VF, 0);
			hdmi_scaler[hdmi_format] = item_integer(P_VSH, hdmi_format);
			main_scaler_factor = hdmi_scaler[hdmi_format];
			if (main_scaler_factor <= 0 || main_scaler_factor > 100) {

				printk("\n>>> [Imapfb]<Error>: Invalid scaler factor %d, force to 95%%. Check items. <<<\n\n", 
					main_scaler_factor);
				main_scaler_factor = 95;
				msleep(3000);
			}
			main_VIC = hdmi_VICs[hdmi_format];
			printk("Got items: hdmi_format = %d (main_VIC = %d), main_scaler_factor = %d\n",
					hdmi_format, main_VIC, main_scaler_factor);
		}

		/* Calcualte width and height */
		vic_fill(&dtd, main_VIC, 60000);
		tmp = dtd.mHActive;
		dtd.mHActive = (tmp * main_scaler_factor /100) & (~0x3);
		dtd.mHBlanking += (tmp - dtd.mHActive);
		tmp = dtd.mVActive;
		dtd.mVActive = (tmp * main_scaler_factor /100) & (~0x3);
		dtd.mVBlanking += (tmp - dtd.mVActive);
		if (dtd.mHActive % 4 || dtd.mVActive % 4)
			printk("Error: mHActive or mVActive does not aligned with 4. Program Mistake.\n");		
	}
	else {
		if (!item_exist(MAIN_FB_VIC)) {
			printk("Error: item %s does not exist.\n", MAIN_FB_VIC);
		}
		else {
			main_VIC = item_integer(MAIN_FB_VIC, 0);
			printk("%s = %d\n", MAIN_FB_VIC, main_VIC);
		}
		main_scaler_factor = 100;

		vic_fill(&dtd, main_VIC, 60000);
	}
#endif
	
	if (pt == PRODUCT_MID)
		main_VIC = LCD_VIC;
	else if (pt == PRODUCT_BOX) {
		main_VIC = 4;	//720p
	}

	main_scaler_factor = 100;
	vic_fill(&dtd, main_VIC, 60);

	
	printk("main_VIC = %d, main_scaler_factor = %d, mHActive = %d, mVActive = %d, FrameBuffer Size: %d * %d\n", 
			main_VIC, main_scaler_factor, dtd.mHActive, dtd.mVActive, dtd.mHActive, dtd.mVActive);

	for (index = 0; index < 1 /*IMAPFB_NUM*/; index++)
	{
		//memset((void *)&gDI_Info, 0, sizeof (di_screen_info_t));
		//handle = NULL;
		// double buffer 
		//ret = ids_fblayer_init(&handle, 2, &gDI_Info);
		//if(ret < 0){
		//	printk(KERN_ERR "[imap_probe] : ids_fblayer_init failed  --\n");
		//}
		//imapfb_info[index].fbhandle = (void *)handle;
		
		// ++leo@2012/06/18, set info->fbhandle.
		//info->fbhandle = (void *)handle;
		// --leo@2012/06/18, set info->fbhandle.


		fb_screen_param_init(&dtd);

		imapfb_info[index].mem = info->mem;
		imapfb_info[index].io = info->io;
		imapfb_info[index].clk1 = info->clk1;
		imapfb_info[index].clk2 = info->clk2;

		imapfb_init_fbinfo(&imapfb_info[index], driver_name, index);

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
		
		printk(KERN_ALERT "[imapfb]<DEBUG>: Before register frame buffer.\n");
		ret = register_framebuffer(&imapfb_info[index].fb);
		printk(KERN_ALERT "[imapfb]<DEBUG>: After register frame buffer.\n");
		if (ret < 0)
		{
			printk(KERN_ERR "[imapfb]<ERR>: Failed to register framebuffer device %d\n", ret);
			goto dealloc_cmap;
		}

		printk(KERN_INFO "fb%d: %s frame buffer device\n", imapfb_info[index].fb.node, imapfb_info[index].fb.fix.id);
	}

	// power-up logo configuration 
#if 0
	if(gDI_Info.pixFmt == FBLAYER_PIXFMT_16BPP_RGB565){
		if (imapfb_fimd.xres == 800 && imapfb_fimd.yres == 480){
			hex_from_bin = hex_from_bin_800x480;
		}else if (imapfb_fimd.xres == 1024 && imapfb_fimd.yres == 768){
			hex_from_bin = hex_from_bin_1024x768;
		}
		start_offset = imapfb_info[0].map_size_f1 >> 1;
		buffer.vir_addr = (void *)((UINT32)imapfb_info[0].map_cpu_f1 + start_offset);
		buffer.phy_addr = (unsigned int)imapfb_info[0].map_dma_f1 + start_offset;
		buffer.size = imapfb_info[0].map_size_f1 - start_offset;
		buffer.flag = IM_BUFFER_FLAG_PHY;
		/*
		 *   check logo valid ?
		 */
		new_logo = new_logo_exist(hex_from_bin);
		printk(KERN_INFO "new_logo = %d\n",new_logo);
		if (new_logo)
			logo_data = hex_from_bin + 8;
		if (logo_data){
			memcpy(buffer.vir_addr,logo_data,buffer.size);
			ids_fblayer_swap_buffer(handle, &buffer);
		}
	}
#endif

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

	if ( 1 == ubootLogoState ) {
		uboot_logo_pix_data = rbget("idslogofb");
		if( NULL != uboot_logo_pix_data){
			memcpy(imapfb_info[0].map_cpu_f1,uboot_logo_pix_data,dtd.mHActive* dtd.mVActive*imapfb_fimd.bytes_per_pixel ); 
		}
	}

	//ids_fblayer_swap_buffer(handle, &buffer);

	//ids_fblayer_open(imapfb_info[0].fbhandle);

	/* ######################################################################### */

	//ids_complete = 1;
	printk(KERN_INFO "Imap Framebuffer Driver Initialization OK!\n");

	// Temp for test
	if (pt == PRODUCT_BOX) {
		char *p = getenv("vic");
		if (p == NULL) {
			box_vic = 4;	//1080p
		} else {
			box_vic = display_atoi(p);
			printk(KERN_ERR "get vic %d by env\n", box_vic);
		}

		int bmp_width, bmp_height;
		hdmi_get_vic(vic, 64);
        terminal_configure("hdmi_sink", box_vic, 1);
		fill_framebuffer_logo((u32)map_cpu_cvbs, &bmp_width, &bmp_height);
		terminal_configure("cvbs_sink", cvbs_style?3:18, 1);
		swap_cvbs_buffer(map_dma_cvbs);
	} else if (pt == PRODUCT_MID) {
		 /*                                                                             
		   *(1)Author:summer                                                             
		   *(2)Date:2014-6-26                                                            
		   *(3)Reason:                                                                   
		   *  The logo from uboot to kernel can't be continous,the modification is    
		   *  required by alex.                                                          
		 * */     
		terminal_configure("lcd_panel", main_VIC, 1);
		//summer modification ends here
	}

	return 0;

dealloc_cmap:
	fb_dealloc_cmap(&imapfb_info[index].fb.cmap);

free_video_memory:
	imapfb_unmap_video_memory(&imapfb_info[index]);

release_io:
	iounmap(info->io);

//release_mem:
	release_resource(info->mem);
	kfree(info->mem);

dealloc_fb:
	framebuffer_release(fbinfo);
	return ret;
}
#endif

#if 0
static int __init imapfb_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct fb_info *fbinfo;
	imapfb_info_t *info;
	UINT8 driver_name[] = "imapfb";
	UINT32 index, size;
	int ret;
	fblayer_handle_t handle ;
	UINT32 start_offset;									/* virtual offset */
	IM_Buffer buffer;
	unsigned char *hex_from_bin=NULL, *logo_data = NULL; 

	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	printk(KERN_INFO "Imap Framebuffer Driver Initialization Start!\n");
	
	//Check Input Argument
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
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, fbinfo);
	info = fbinfo->par;
	info->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
	{
		printk(KERN_ERR "[imapfb_probe]: fail to get io registers\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}
	size = (res->end - res->start) + 1;
	info->mem = request_mem_region(res->start, size, pdev->name);
	if (!info->mem)
	{
		printk(KERN_ERR "[imapfb_probe]: fail to get memory region\n");
		ret = -ENOENT;
		goto dealloc_fb;
	}
	//info->io = ioremap(res->start, size);
	info->io = ioremap_nocache(res->start, size);
	if (!info->io)
	{
		printk(KERN_ERR "[imapfb_probe]: ioremap of registers fail\n");
		ret = -ENXIO;
		goto release_mem;
	}

	for (index = 0; index < 1 /*IMAPFB_NUM*/; index++)
	{
		memset((void *)&gDI_Info, 0, sizeof (di_screen_info_t));
		handle = NULL;
		// double buffer 
		ret = ids_fblayer_init(&handle, 2, &gDI_Info);
		if(ret < 0){
			printk(KERN_ERR "[imap_probe] : ids_fblayer_init failed  --\n");
		}
		imapfb_info[index].fbhandle = (void *)handle;
		
		// ++leo@2012/06/18, set info->fbhandle.
		info->fbhandle = (void *)handle;
		// --leo@2012/06/18, set info->fbhandle.


		fb_screen_param_init();

		imapfb_info[index].mem = info->mem;
		imapfb_info[index].io = info->io;
		imapfb_info[index].clk1 = info->clk1;
		imapfb_info[index].clk2 = info->clk2;

		imapfb_init_fbinfo(&imapfb_info[index], driver_name, index);

		/* Initialize video memory */
		ret = imapfb_map_video_memory(&imapfb_info[index]);
		if (ret)
		{
			printk(KERN_ERR "[imapfb_probe]: win %d fail to allocate framebuffer video ram\n", index);
			ret = -ENOMEM;
			//goto release_clock2;
			goto release_io;
		}

		ret = imapfb_check_var(&imapfb_info[index].fb.var, &imapfb_info[index].fb);
		if (ret)
		{
			printk(KERN_ERR "[imapfb_probe]: win %d fail to check var\n", index);
			ret = -EINVAL;
			goto free_video_memory;
		}
		
		if (index < 2)
		{
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 256, 0) < 0)
			{
				printk(KERN_ERR "[imapfb_probe]: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		}
		else
		{
			if (fb_alloc_cmap(&imapfb_info[index].fb.cmap, 16, 0) < 0)
			{
				printk(KERN_ERR "[imapfb_probe]: win %d fail to allocate color map\n", index);
				goto free_video_memory;
			}
		}
		
		printk(KERN_ALERT "Before register frame buffer.\n");
		ret = register_framebuffer(&imapfb_info[index].fb);
		printk(KERN_ALERT "After register frame buffer.\n");
		if (ret < 0)
		{
			printk(KERN_ERR "[imapfb_probe]: failed to register framebuffer device %d\n", ret);
			goto dealloc_cmap;
		}

		printk(KERN_INFO "fb%d: %s frame buffer device\n", imapfb_info[index].fb.node, imapfb_info[index].fb.fix.id);
	}

	// power-up logo configuration 
#if 1
	if(gDI_Info.pixFmt == FBLAYER_PIXFMT_16BPP_RGB565){
		if (imapfb_fimd.xres == 800 && imapfb_fimd.yres == 480){
			hex_from_bin = hex_from_bin_800x480;
		}else if (imapfb_fimd.xres == 1024 && imapfb_fimd.yres == 768){
			hex_from_bin = hex_from_bin_1024x768;
		}
		start_offset = imapfb_info[0].map_size_f1 >> 1;
		buffer.vir_addr = (void *)((UINT32)imapfb_info[0].map_cpu_f1 + start_offset);
		buffer.phy_addr = (unsigned int)imapfb_info[0].map_dma_f1 + start_offset;
		buffer.size = imapfb_info[0].map_size_f1 - start_offset;
		buffer.flag = IM_BUFFER_FLAG_PHY;
		/*
		 *   check logo valid ?
		 */
		new_logo = new_logo_exist(hex_from_bin);
		printk(KERN_INFO "new_logo = %d\n",new_logo);
		if (new_logo)
			logo_data = hex_from_bin + 8;
		if (logo_data){
			memcpy(buffer.vir_addr,logo_data,buffer.size);
			ids_fblayer_swap_buffer(handle, &buffer);
		}
	}
#endif

	start_offset = imapfb_info[0].map_size_f1 >> 1;
	buffer.vir_addr = (void *)((UINT32)imapfb_info[0].map_cpu_f1 + start_offset);
	buffer.phy_addr = (unsigned int)imapfb_info[0].map_dma_f1 + start_offset;
	buffer.size = imapfb_info[0].map_size_f1 - start_offset;
	buffer.flag = IM_BUFFER_FLAG_PHY;
	fill_framebuffer_logo((UINT32)buffer.vir_addr);
	ids_fblayer_swap_buffer(handle, &buffer);

	ids_fblayer_open(imapfb_info[0].fbhandle);

	/* ######################################################################### */

	ids_complete = 1;
	printk(KERN_INFO "Imap Framebuffer Driver Initialization OK!\n");

	return 0;

dealloc_cmap:
	fb_dealloc_cmap(&imapfb_info[index].fb.cmap);

free_video_memory:
	imapfb_unmap_video_memory(&imapfb_info[index]);

release_io:
	iounmap(info->io);

release_mem:
	release_resource(info->mem);
	kfree(info->mem);

dealloc_fb:
	framebuffer_release(fbinfo);
	return ret;
}
#endif

static int imapfb_remove(struct platform_device *pdev)
{
	struct fb_info *fbinfo = platform_get_drvdata(pdev);
	imapfb_info_t *info = fbinfo->par;
	UINT32 index = 0;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);

	//ids_fblayer_deinit(info->fbhandle); 

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

int imapfb_suspend(struct platform_device *dev, pm_message_t state)
{
	//struct fb_info *fbinfo = platform_get_drvdata(dev);
	//imapfb_info_t *info = fbinfo->par;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);

	//ids_fblayer_close(info->fbhandle);

	return 0;
}

int imapfb_resume(struct platform_device *dev)
{
	//struct fb_info *fbinfo = platform_get_drvdata(dev);
	//imapfb_info_t *info = fbinfo->par;
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);

	//ids_fblayer_open(info->fbhandle); 

	return 0;
}

static struct platform_driver imapfb_driver = {
	.probe		= imapfb_probe,
	.remove		= imapfb_remove,
//	.suspend		= imapfb_suspend,
//	.resume		= imapfb_resume,
	.driver		= {
		.name	= "imap-fb",
		.owner	= THIS_MODULE,
	},
};

int __init imapfb_init(void)
{
	IMAPX_FBLOG(" %d  %s -- \n",__LINE__,__func__);
	printk("imapfb init\n");
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

