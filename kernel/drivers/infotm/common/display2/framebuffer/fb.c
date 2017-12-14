/* display fb driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fb.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <mach/items.h>
#include <linux/display_device.h>
#include <linux/display_fb.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"fb"

struct fb_priv {
	struct display_device *pdev;
};

#ifdef CONFIG_ARCH_Q3F
/*
  * Q3F limit:
  * 	When LCDC, whether use prescaler or not, fetch NV12 buffer, buffer width must satisfy some limit.
  * 	else IDS does NOT work.
*/
static int filter_yuv_input_width(int width, int height, int *s_width)
{
	int i, j;
	int scaler_w = width & (~0x07);

	width &= (~0x07);
	for (i = 0; i < 4; i++) {
		scaler_w = width-(i*8);
		for(j = 1 ; j <=  height; j++) {
			if((j*scaler_w/8)%256==255) {
				break;
			}
		}
		if (j > height)
			break;
	}
	if (i >= 4) {
		dlog_err("buffer input width can NOT be accept by Q3F IDS due to hardware limit\n");
		return -1;
	}

	dlog_dbg("filter input width is %d\n", scaler_w);
	*s_width = scaler_w;

	return 0;
}
#endif

static int __display_fb_release_logo(struct display_device * tpdev)
{
	int i;
	dtd_t buffer_dtd = {0}, win_dtd = {0};
	struct display_function * func;
	struct display_device * pdev;
	int path = tpdev->path;

	if (!tpdev->logo_virt_addr)
		return 0;

	/* free logo memory and disable overlay0 at first ioctl operation */
	dlog_dbg("disable logo\n");
	for (i = 0; i < IDS_PATH_NUM; i++)
		display_path_disable(i);
	dlog_dbg("free logo memory, disable overlay0\n");
	kfree(tpdev->logo_virt_addr);
	tpdev->logo_virt_addr = 0;

	buffer_dtd.mHActive = 0;
	buffer_dtd.mVActive = 0;
	buffer_dtd.mHImageSize = 0;
	buffer_dtd.mVImageSize = 0;
	buffer_dtd.mCode = DISPLAY_SPECIFIC_VIC_BUFFER_START+ 0;
	win_dtd.mHActive = 0;
	win_dtd.mVActive = 0;
	win_dtd.mInterlaced = 0;
	win_dtd.mPixelRepetitionInput = 0;
	win_dtd.mCode= DISPLAY_SPECIFIC_VIC_OVERLAY_START+ 0;
	display_get_function(path, DISPLAY_MODULE_TYPE_VIC, &func, &pdev);
	func->set_dtd(pdev, buffer_dtd.mCode, &buffer_dtd);
	func->set_dtd(pdev, win_dtd.mCode, &win_dtd);

	return 0;
}

static int display_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	return 0;
}

static int display_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	/* TODO */
	return 0;
}

static int display_fb_blank(int blank_mode, struct fb_info *info)
{
	struct fb_priv * priv = info->par;
	struct display_device * tpdev = priv->pdev;
	int path = tpdev->path;
	struct display_device * pdev;

	display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);

	dlog_dbg("fb%d, path%d: blank_mode=%d, pdev->enable=%d\n",
					info->node, path, blank_mode, pdev->enable);

	if (blank_mode == FB_BLANK_UNBLANK && !pdev->enable)
		return display_path_enable(path);
	else if (blank_mode != FB_BLANK_UNBLANK && pdev->enable)
		return display_path_disable(path);
	else
		dlog_dbg("skip duplicated blank operation\n");

	return 0;
}

static int display_fb_set_par(struct fb_info *info)
{
	unsigned int buffer_xsize, buffer_ysize, buffer_vwidth, buffer_vheight, buffer_xoffset, buffer_yoffset;
	unsigned int win_xsize, win_ysize, win_xoffset, win_yoffset;
	struct display_function * func;
	struct fb_priv * priv = info->par;
	struct display_device * tpdev = priv->pdev;
	int path = tpdev->path;
	struct display_device * pdev;
	dtd_t buffer_dtd = {0}, win_dtd = {0}, p_dtd = {0};
	enum display_data_format_bpp bpp = DISPLAY_DATA_FORMAT_32BPP_ARGB8888;
	enum display_yuv_format yuv = 0;
	int red_blue_swap = 0;
	int overlay = info->node;
	int ret;

	dlog_dbg("fb%d, path%d, overlay%d\n",
			info->node, path, overlay);

	if (!info) {
		dlog_err("null info param\n");
		return -EINVAL;
	}

	if (tpdev->logo_virt_addr)
		__display_fb_release_logo(tpdev);

	/* parse param from var */
	buffer_xsize = info->var.xres;
	buffer_ysize = info->var.yres;
	buffer_vwidth = info->var.xres_virtual;
	buffer_vheight = info->var.yres_virtual;
	buffer_xoffset = info->var.xoffset;
	buffer_yoffset = info->var.yoffset;
	win_xsize = info->var.width;
	win_ysize = info->var.height;
	win_xoffset = info->var.reserved[0];
	win_yoffset = info->var.reserved[1];

	info->fix.smem_len = buffer_xsize * buffer_ysize * 4;

	if (info->var.grayscale == V4L2_PIX_FMT_RGB565)
		bpp = DISPLAY_DATA_FORMAT_16BPP_RGB565;
	else if (info->var.grayscale == V4L2_PIX_FMT_RGB24)
		bpp = DISPLAY_DATA_FORMAT_24BPP_RGB888;
	else if (info->var.grayscale == V4L2_PIX_FMT_BGR24) {
		bpp = DISPLAY_DATA_FORMAT_24BPP_RGB888;
		red_blue_swap = 1;
	}
	else if (info->var.grayscale == V4L2_PIX_FMT_RGB32)
		bpp = DISPLAY_DATA_FORMAT_32BPP_ARGB8888;
	else if (info->var.grayscale == V4L2_PIX_FMT_BGR32) {
		bpp = DISPLAY_DATA_FORMAT_32BPP_ARGB8888;
		red_blue_swap = 1;
	}
	else if (info->var.grayscale == V4L2_PIX_FMT_NV12) {
		bpp = DISPLAY_DATA_FORMAT_YCBCR_420;
		yuv = DISPLAY_YUV_FORMAT_NV12;
	}
	else if (info->var.grayscale == V4L2_PIX_FMT_NV21) {
		bpp = DISPLAY_DATA_FORMAT_YCBCR_420;
		yuv = DISPLAY_YUV_FORMAT_NV21;
	}
	else {
		dlog_err("Invalid var.grayscale %d\n", info->var.grayscale);
		return -EINVAL;
	}

#if defined(CONFIG_ARCH_Q3F) || defined(CONFIG_ARCH_APOLLO3)
	if (overlay == 1 && bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		dlog_err("for Q3F, suggest to use RGB format on overlay1 to avoid BUGs.\n");
#endif

	ret = vic_fill(path, &p_dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
	if (ret) {
		dlog_err("vic fill peripheral failed %d\n", ret);
		return ret;
	}

	/* when TVIF interlaced, if not use prescaler, overlay fetch NV12 buffer, buffer width must be align with 8,
		else will output pure color instead.
		according to all Q3 series limit */
	if (p_dtd.mInterlaced && buffer_xsize == win_xsize && buffer_ysize == win_ysize
			&& bpp == DISPLAY_DATA_FORMAT_YCBCR_420) {
		buffer_xsize &= (~0x07);
		win_xsize &= (~0x07);
	}
#ifdef CONFIG_ARCH_Q3F
	/* when TVIF interlaced, use prescaler fetch NV12 buffer, buffer width must be align with 8,
		else will output pure color instead.
		when LCDC, whether use prescaler or not, fetch NV12 buffer, buffer width must satisfy some limit.
		according to Q3F limit.
		in order to support various resolution peripheral, prescaler is always used on overlay0 for Q3F. */
	if (bpp == DISPLAY_DATA_FORMAT_YCBCR_420) {
		filter_yuv_input_width(info->var.xres, buffer_ysize, &buffer_xsize);
		if (overlay != 0)
			filter_yuv_input_width(info->var.width, win_ysize, &win_xsize);
	}
#endif

	/* fill dtd */
	buffer_dtd.mHActive = buffer_xsize;
	buffer_dtd.mVActive = buffer_ysize;
	buffer_dtd.mHImageSize = buffer_vwidth;
	buffer_dtd.mVImageSize = buffer_vheight;
	buffer_dtd.mHBorder = buffer_xoffset;
	buffer_dtd.mVBorder = buffer_yoffset;
	buffer_dtd.mCode = DISPLAY_SPECIFIC_VIC_BUFFER_START+info->node;

	win_dtd.mHActive = win_xsize;
	win_dtd.mVActive = win_ysize;
	win_dtd.mHBorder = win_xoffset;
	win_dtd.mVBorder = win_yoffset;
	win_dtd.mCode= DISPLAY_SPECIFIC_VIC_OVERLAY_START+info->node;
	win_dtd.mInterlaced = p_dtd.mInterlaced;

#if 0
	/* this is another method to solve odd line problem when prescaler+tvif interlace.
			refers to scaler module scaler_set_size() description, can be coexist with it.
			according to all Q3 series limit */
	if ((buffer_xsize != win_xsize || buffer_ysize != win_ysize) && p_dtd.mInterlaced) {
		buffer_dtd.mVActive = buffer_ysize&(~0x01);
		win_dtd.mVActive = win_ysize&(~0x01);
	}
#endif

	/* save dtd */
	display_get_function(path, DISPLAY_MODULE_TYPE_VIC, &func, &pdev);
	func->set_dtd(pdev, buffer_dtd.mCode, &buffer_dtd);
	func->set_dtd(pdev, win_dtd.mCode, &win_dtd);

	/* config overlay */
	display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);

	func->set_overlay_vic(pdev, overlay, win_dtd.mCode);
	func->set_overlay_position(pdev, overlay, win_xoffset, win_yoffset);
	func->set_overlay_vw_width(pdev, overlay, buffer_vwidth);
	func->set_overlay_bpp(pdev, overlay, bpp, yuv, 0);
	func->set_overlay_red_blue_exchange(pdev, overlay, red_blue_swap);

	return 0;
}

static int display_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	struct fb_priv * priv = info->par;
	struct display_device * tpdev = priv->pdev;
	int path = tpdev->path;
	struct display_device * pdev;
	struct display_function * func;
	struct display_fb_win_size win_size;
	void __user *argp = (void __user *)arg;
	dtd_t dtd = {0};
	int ret = 0;
	int vic;
	char name[64] = {0};
	unsigned int addr, addr_offset;
	int en;
	unsigned int color, val, timeout;
	int overlay = info->node;

	if (tpdev->logo_virt_addr)
		__display_fb_release_logo(tpdev);

	switch (cmd) {
		case FBIO_WAITFORVSYNC:
			timeout = (unsigned int)arg;	// in ms
			display_get_function(path, DISPLAY_MODULE_TYPE_IRQ, &func, &pdev);
			return func->wait_vsync(pdev, timeout*HZ/1000);
			break;

		case FBIO_GET_OSD_BACKGROUND:
			dlog_dbg("ioctl:FBIO_GET_OSD_BACKGROUND\n");
			display_get_device(path, DISPLAY_MODULE_TYPE_OVERLAY, &pdev);
			if (copy_to_user(argp, &pdev->osd_background_color, sizeof(int)))
				return -EFAULT;
			break;

		case FBIO_SET_OSD_BACKGROUND:
			dlog_dbg("ioctl:FBIO_SET_OSD_BACKGROUND\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_background_color(pdev, color);

		case FBIO_GET_OSD_SIZE:
			dlog_dbg("ioctl:FBIO_GET_OSD_SIZE\n");
			vic_fill(path, &dtd, DISPLAY_SPECIFIC_VIC_OSD);
			if (copy_to_user(argp, &dtd, sizeof(dtd)))
				return -EFAULT;
			break;

		case FBIO_SET_OSD_SIZE:
			dlog_dbg("ioctl:FBIO_SET_OSD_SIZE\n");
			if (copy_from_user(&win_size, (struct display_fb_win_size *)arg, sizeof(win_size)))
				return -EFAULT;
			/* if postscaler+tvif interlaced, osd height must be even, according to all Q3 series limit */
			vic_fill(path, &dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
			if (dtd.mInterlaced && (win_size.width != dtd.mHActive || win_size.height != dtd.mVActive)) {
				win_size.height = (win_size.height+2)&(~0x01);
			}
			dtd.mHActive = win_size.width;
			dtd.mVActive = win_size.height;
			dtd.mCode = DISPLAY_SPECIFIC_VIC_OSD;
			display_get_function(path, DISPLAY_MODULE_TYPE_VIC, &func, &pdev);
			ret = func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_OSD, &dtd);
			if (ret) {
				dlog_err("path%d: set_dtd failed %d\n", path, ret);
				return ret;
			}
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			ret = func->set_vic(pdev, dtd.mCode);
			if (ret) {
				dlog_err("path%d: set osd vic failed %d\n", path, ret);
				return ret;
			}
			break;

		case FBIO_GET_PERIPHERAL_SIZE:
			dlog_dbg("ioctl:FBIO_GET_PERIPHERAL_SIZE\n");
			vic_fill(path, &dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
			win_size.width = dtd.mHActive;
			win_size.height = dtd.mVActive;
			if (copy_to_user(argp, &win_size, sizeof(win_size)))
				return -EFAULT;
			break;

		case FBIO_GET_PERIPHERAL_VIC:
			dlog_dbg("ioctl:FBIO_GET_PERIPHERAL_VIC\n");
			vic = display_get_peripheral_vic(path);
			if (copy_to_user(argp, &vic, sizeof(vic)))
				return -EFAULT;
			return (vic < 0 ? vic : 0);
			break;

		case FBIO_SET_PERIPHERAL_VIC:
			dlog_dbg("ioctl:FBIO_SET_PERIPHERAL_VIC\n");
			vic = (int)arg;
			return display_set_peripheral_vic(path, vic);
			break;

		case FBIO_GET_BLANK_STATE:
			dlog_dbg("ioctl:FBIO_GET_BLANK_STATE\n");
			display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
			en = pdev->enable;
			if (copy_to_user(argp, &en, sizeof(en)))
				return -EFAULT;
			return 0;
			break;

		case FBIO_SET_PERIPHERAL_DEVICE:
			dlog_dbg("ioctl:FBIO_SET_PERIPHERAL_DEVICE\n");
			if (copy_from_user(name, (char *)arg, 64))
				return -EFAULT;
			return display_select_peripheral(path, name);
			break;

		case FBIO_SET_BUFFER_ADDR:
			addr = (unsigned int)arg;
			//dlog_dbg("ioctl:FBIO_SET_BUFFER_ADDR 0x%X\n", addr);
			display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
			if (pdev->peripheral_config.interface_type == DISPLAY_INTERFACE_TYPE_I80 &&
					pdev->peripheral_config.i80_refresh) {
				vic_fill(path, &dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START+overlay);
				addr_offset = dtd.mHImageSize * dtd.mVBorder + dtd.mHBorder;
				display_get_device(path, DISPLAY_MODULE_TYPE_OVERLAY, &pdev);
				if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_32BPP_ARGB8888
						|| pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_24BPP_RGB888)
					addr_offset *= 4;
				else if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_16BPP_RGB565)
					addr_offset *= 2;
				else
					addr_offset *= 1;
				pdev->peripheral_config.i80_mem_phys_addr = addr+addr_offset;
				return pdev->peripheral_config.i80_refresh(pdev, addr+addr_offset);
			}
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			ret = func->set_overlay_buffer_addr(pdev, overlay, addr);
			display_get_function(path, DISPLAY_MODULE_TYPE_SCALER, &func, &pdev);
			if (pdev->enable && pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER
						&& pdev->prescaler_chn == overlay)
				ret = func->set_prescaler_buffer_addr(pdev, addr);
			return ret;
			break;

		case FBIO_SET_BUFFER_ENABLE:
			dlog_dbg("ioctl:FBIO_SET_BUFFER_ENABLE\n");
			en = (int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			func->overlay_enable(pdev, overlay, en);
			display_get_function(path, DISPLAY_MODULE_TYPE_SCALER, &func, &pdev);
			if (en) {
				func->enable(pdev, en);
				if (pdev->scale_needed && pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER
							&& pdev->prescaler_chn == overlay) {
					display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
					if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_BT656
							|| pdev->interface_type == DISPLAY_INTERFACE_TYPE_BT601)
						display_get_function(path, DISPLAY_MODULE_TYPE_TVIF, &func, &pdev);
					else
						display_get_function(path, DISPLAY_MODULE_TYPE_LCDC, &func, &pdev);
					func->enable(pdev, 0);
					func->enable(pdev, 1);
				}
			}
			else {
				if (pdev->enable && pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER
							&& pdev->prescaler_chn == overlay)
					func->enable(pdev, en);
			}
			break;

		case FBIO_SET_MAPCOLOR_ENABLE:
			dlog_dbg("ioctl:FBIO_SET_MAPCOLOR_ENABLE\n");
			en = (int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_mapcolor(pdev, overlay, en, pdev->overlays[overlay].mapcolor);
			break;

		case FBIO_SET_MAPCOLOR:
			dlog_dbg("ioctl:FBIO_SET_MAPCOLOR\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_mapcolor(pdev, overlay, pdev->overlays[overlay].mapcolor_en, color);
			break;

		case FBIO_SET_CHROMA_KEY_ENABLE:
			dlog_dbg("ioctl:FBIO_SET_CHROMA_KEY_ENABLE\n");
			en = (int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_chroma_key(pdev, overlay,	\
						en, pdev->overlays[overlay].ck_value,	\
						pdev->overlays[overlay].ck_alpha_nonkey_area,	\
						pdev->overlays[overlay].ck_alpha_key_area);
			break;

		case FBIO_SET_CHROMA_KEY_VALUE:
			dlog_dbg("ioctl:FBIO_SET_CHROMA_KEY_VALUE\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_chroma_key(pdev, overlay,	\
						pdev->overlays[overlay].ck_enable, color,	\
						pdev->overlays[overlay].ck_alpha_nonkey_area,	\
						pdev->overlays[overlay].ck_alpha_key_area);
			break;

		case FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA:
			dlog_dbg("ioctl:FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_chroma_key(pdev, overlay,	\
						pdev->overlays[overlay].ck_enable,	\
						pdev->overlays[overlay].ck_value,	color,	\
						pdev->overlays[overlay].ck_alpha_key_area);
			break;

		case FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA:
			dlog_dbg("ioctl:FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_chroma_key(pdev, overlay,	\
						pdev->overlays[overlay].ck_enable,	\
						pdev->overlays[overlay].ck_value,		\
						pdev->overlays[overlay].ck_alpha_nonkey_area, color);
			break;

		case FBIO_SET_ALPHA_TYPE:
			dlog_dbg("ioctl:FBIO_SET_ALPHA_TYPE\n");
			val = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_alpha(pdev, overlay, 	\
						val, pdev->overlays[overlay].alpha_value);
			break;

		case FBIO_SET_ALPHA_VALUE:
			dlog_dbg("ioctl:FBIO_SET_ALPHA_VALUE\n");
			color = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
			return func->set_overlay_alpha(pdev, overlay, 	\
						pdev->overlays[overlay].alpha_type, color);
			break;

		case FBIO_SET_LCDVCLK_CDOWN:
			dlog_dbg("ioctl:FBIO_SET_LCDVCLK_CDOWN\n");
			val = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_LCDC, &func, &pdev);
			return func->set_auto_vclk(pdev, val, pdev->rfrm_num);
			break;

		case FBIO_SET_LCDVCLK_RFRM_NUM:
			dlog_dbg("ioctl:FBIO_SET_LCDVCLK_RFRM_NUM\n");
			val = (unsigned int)arg;
			display_get_function(path, DISPLAY_MODULE_TYPE_LCDC, &func, &pdev);
			return func->set_auto_vclk(pdev, pdev->cdown, val);
			break;

		default:
			dlog_err("Unrecognized ioctl cmd %d\n", cmd);
			return -EFAULT;
			break;
	}

	return 0;
}

static struct fb_ops display_fb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var		= display_fb_check_var,
	.fb_set_par		= display_fb_set_par,
	.fb_blank 		= display_fb_blank,
	.fb_pan_display	= display_fb_pan_display,
	.fb_ioctl			= display_fb_ioctl,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
};

static int fb_enable(struct display_device *pdev, int en)
{
	dlog_dbg("en=%d\n", en);
	pdev->enable = en;		// store
	return 0;
}

static int fb_probe(struct display_device *pdev)
{
	struct fb_info *info;
	struct fb_priv *priv;
	int i;

	dlog_trace();

	for (i = 0; i < IDS_OVERLAY_NUM; i++) {
		info = framebuffer_alloc(sizeof(struct fb_priv), &pdev->dev);
		if (!info) {
			dlog_err("No memory for fb_info and par\n");
			return -ENOMEM;
		}
		priv = info->par;
		priv->pdev = pdev;

		//info->screen_base = display_logo_addr;
		//info->screen_size = ?
		info->fbops = &display_fb_ops;
		strcpy(info->fix.id, "display-fb");
		info->fix.capabilities = FB_CAP_FOURCC;
		info->fix.type = FB_TYPE_FOURCC;
		info->fix.visual = FB_VISUAL_FOURCC;
		info->fix.type_aux = 0;
		info->fix.xpanstep = 0;
		info->fix.ypanstep = 0;
		info->fix.ywrapstep = 0;
		info->fix.accel = FB_ACCEL_NONE;

		info->var.nonstd = 0;
		info->var.activate = FB_ACTIVATE_NOW;
		info->var.accel_flags = FB_ACCEL_NONE;
		info->var.vmode = FB_VMODE_NONINTERLACED;

		/* useless */
		info->var.xres = 640;
		info->var.yres = 480;
		info->var.xres_virtual = 640;
		info->var.yres = 480;
		info->var.xoffset = 0;
		info->var.yoffset = 0;
		info->var.bits_per_pixel = 32;
		info->var.pixclock = 27000000;
		info->var.grayscale = 0;

		info->flags = FBINFO_DEFAULT;
		if (fb_alloc_cmap(&info->cmap, 256, 0))
			return -ENOMEM;

		if (register_framebuffer(info) < 0) {
			fb_dealloc_cmap(&info->cmap);
			return -EINVAL;
		}
	}

	display_set_drvdata(pdev, info);

	return 0;
}

static int fb_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver fb_driver = {
	.probe  = fb_probe,
	.remove = fb_remove,
	.driver = {
		.name = "fb",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_FB,
	.function = {
		.enable = fb_enable,
	},
};

module_display_driver(fb_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display framebuffer driver");
MODULE_LICENSE("GPL");
