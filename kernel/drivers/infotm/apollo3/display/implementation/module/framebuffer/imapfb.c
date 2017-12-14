/*
 * framebuffer.c - display framebuffer driver
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
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <dss_common.h>
#include <implementation.h>


#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[framebuffer]"
#define DSS_SUB2	""



	// TODO: FrameBuffer driver is temp for X9 verification.
	// TODO: Integral FrameBuffer driver will be implemented after X9 verification.


static int obtain_information(int idsx);
extern int osd_swap(int idsx, int fbx);
extern int osd_dma_addr(int idsx, dma_addr_t addr, int frame_size);



typedef int (*swap_fb)(int idsx, int fbx);

swap_fb swap;
static struct platform_device *pdev_priv;
void * fb_pmmHandle = NULL;
static dma_addr_t phy_addr = 0;
static unsigned int frame_size = 0;

// Temp
static int t_width = 0, t_height = 0;


//static dss_limit mainfb_limitation[] = {
	// ???
//	NULL,
//};

//static struct module_attr mainfb_attr[] = {
	// ???
//	{FALSE, ATTR_END, NULL},
//};

struct module_node mainfb_module = {
	.name = "mainfb",
	//.limitation = mainfb_limitation,
	.attr_ctrl = module_attr_ctrl,
	//.attributes = mainfb_attr,
	.idsx = 0,
	//.init = dss_init_module,
	.obtain_info = obtain_information,
};

// Temp
int get_lcd_width(void)
{
	return t_width;
}
int get_lcd_height(void)
{
	return t_height;
}



static int obtain_information(int idsx)
{
	// TODO: Obtain swap function
	// TODO: Temp

	osd_dma_addr(idsx, phy_addr, frame_size);
	osd_swap(idsx, 1);
	
	return 0;
}

static int tmp_fill_logo(struct platform_device *pdev)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	unsigned int __iomem * fb_logo = (unsigned int *)(info->screen_base + info->fix.smem_len/2);
	int width = info->var.xres;
	int height = info->var.yres;
	int part = height/3;	// lines
	int dim = 256/part;	// value step each line
	unsigned int pixel, byte;
	unsigned char shit[3] = {16, 8, 0};		// rgb shit
	int i, j, k;
	dss_trace("~\n");

	for (i = 0; i < 3; i++) {
		for (j = 0; j < part; j++) {
			byte = 255 - dim * j;
			pixel = byte << shit[i];
			for (k = 0; k < width; k++)
				fb_logo[i*(part*width)+(j*width)+k] = pixel;
		}
	}
	return 0;
}

static int imapfb_swap(int idsx, int fbx)
{
	//swap(idsx, &fbx, 1);
	osd_swap(idsx, fbx);	// Temp
	return 0;
}

static int imapfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	int fbx;
	
	fbx = (var->yoffset == 0) ? 0 : 1;
	imapfb_swap(mainfb_module.idsx, fbx);

	return 0;
}

/* Framebuffer operations structure */
struct fb_ops imapfb_ops = {
	.owner			= THIS_MODULE,
	.fb_pan_display	= imapfb_pan_display,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
};

static int imapfb_init_fbinfo(struct platform_device *pdev, int width, int height)
{
	// TODO: All of shit, just copy for test. Rebuild later.


	struct fb_info *info = platform_get_drvdata(pdev);
	dma_addr_t dma_addr;
	//int ret;
	dss_trace("~\n");

	info->fix.type = FB_TYPE_PACKED_PIXELS;
	info->fix.type_aux = 0;
	info->fix.xpanstep = 0;
	info->fix.ypanstep = 1;
	info->fix.ywrapstep = 0;
	info->fix.accel = FB_ACCEL_NONE;
	//info->fix.mmio_start = 0x22000000; //LCD_BASE_REG_PA; //sam :
	//info->fix.mmio_len = SZ_16K;
	info->fix.line_length = width * 4;

	info->fbops = &imapfb_ops;
	info->flags = FBINFO_FLAG_DEFAULT;

	//info->pseudo_palette = &finfo->pseudo_pal;

	info->var.nonstd = 0;
	info->var.activate = FB_ACTIVATE_NOW;
	info->var.accel_flags = 0;
	info->var.vmode = FB_VMODE_NONINTERLACED;

	info->var.xres = width;
	info->var.yres = height;
	info->var.xres_virtual = width;
	info->var.yres_virtual = height * 2;
	info->var.xoffset = 0;
	info->var.yoffset = 0;
	
	info->var.bits_per_pixel = 32;

	info->fix.smem_len = info->var.xres_virtual * info->var.yres_virtual * (roundup(info->var.bits_per_pixel, 8)/8);
	dss_dbg("xres = %d, yres = %d, smem_len = %d\n", info->var.xres, info->var.yres, info->fix.smem_len);
/*
	info->var.pixclock = 30;	// ?
	info->var.hsync_len = imapfb_fimd.hsync_len;
	info->var.left_margin = imapfb_fimd.left_margin;
	info->var.right_margin = imapfb_fimd.right_margin;
	info->var.vsync_len = imapfb_fimd.vsync_len;
	info->var.upper_margin = imapfb_fimd.upper_margin;
	info->var.lower_margin = imapfb_fimd.lower_margin;
	info->var.sync = imapfb_fimd.sync;
	info->var.grayscale = imapfb_fimd.cmap_grayscale;
*/
	info->var.red.offset = 16;
	info->var.red.length = 8;
	info->var.green.offset = 8;
	info->var.green.length = 8;
	info->var.blue.offset = 0;
	info->var.blue.length = 8;
	info->var.transp.offset = 24;
	info->var.transp.length = 8;
	
	info->screen_base = dma_alloc_coherent(&pdev->dev, info->fix.smem_len, &dma_addr, GFP_KERNEL);
	if (info->screen_base == NULL) {
		dss_err("dma_alloc_coherent failed\n");
		return -1;
	}
	info->fix.smem_start = (unsigned long)dma_addr;
	phy_addr = dma_addr;
	frame_size = info->fix.smem_len/2;
	dss_dbg("mainfb virtual addr = 0x%X, physical addr = 0x%X\n", (u32)info->screen_base, (u32)info->fix.smem_start);

	tmp_fill_logo(pdev);
	//ids_set_main_fb_addr(fbi->map_dma_f1, fbi->map_size_f1);

	if (fb_alloc_cmap(&info->cmap, 256, 0) < 0) {
		dss_err("color map allocation failed\n");
		return -1;
	}

	return 0;
}

static int __init imapfb_probe(struct platform_device *pdev)
{
	// TODO: Not consider 'goto' currently
	
	struct fb_info *info;
	dtd_t dtd;
	int vic, width, height;
	int ret;
	char lcd_name[64] = {0};
	char tmp[64] = {0};
	char * token;
	int len;
	dss_trace("~\n");
	
	info = framebuffer_alloc(sizeof(struct fb_info), &pdev->dev);
	if (!info) {
		dss_err("framebuffer allocation failed\n");
		return -1;
	}
	platform_set_drvdata(pdev, info);

	if (!item_exist(P_MAINFB_VIC)) {
		dss_err("#item# Lack of Main fb vic item.\n");
		return -1;
	}
	vic = item_integer(P_MAINFB_VIC, 0);
	if (vic < 1000) {
		vic_fill(&dtd, vic, 60);
		width = dtd.mHActive;
		height = dtd.mVActive;
	}
	else {
		item_string(lcd_name, P_LCD_NAME, 0);
		dss_dbg("get lcd panel name: %s\n", lcd_name);
		token = strchr(lcd_name, '_');
		token++;						// first _
		len = strcspn(token, "_");
		strncpy(tmp, token, len);
		tmp[len] = '\0';
		sscanf(tmp, "%d", &width);
		token = strchr(token, '_');
		token++;	
		sscanf(token, "%d", &height);
		dss_dbg("parse lcd panel width %d, height %d\n", width, height);
	}
	dss_info("Main FrameBuffer size [%d * %d]\n", width, height);
	
	ret = imapfb_init_fbinfo(pdev, width, height);
	if (ret < 0)
		return -1;
	printk("Before register framebuffer.\n");
	//ret = register_framebuffer(info);
	//if (ret < 0) {
	//	dss_err("register framebuffer faild %d\n", ret);
	//	return -1;
	//}
	printk("After register framebuffer.\n");

	pdev_priv = pdev;

	return 0;
}


static struct platform_driver imapfb_driver = {
	.probe		= imapfb_probe,
	//.remove		= imapfb_remove,
	.driver		= {
		.name	= "imap-fb",
		.owner	= THIS_MODULE,
	},
};

int __init imapfb_init(void)
{
	return platform_driver_register(&imapfb_driver);
}
static void __exit imapfb_exit(void)
{
	platform_driver_unregister(&imapfb_driver);
}

module_init(imapfb_init);
module_exit(imapfb_exit);

MODULE_DESCRIPTION("InfoTM iMAP display framebuffer driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
