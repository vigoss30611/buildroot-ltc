#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
//#include <asm-generic/errno-base.h>
#include "display_fb.h"

static int debug = 0;

#define dlog_dbg(format, arg...)	\
({if(debug) printf("[buf2fb] dbg: " format, ## arg);0;})

#define dlog_info(format, arg...)	\
({printf("[buf2fb] info: " format, ## arg);0;})

#define dlog_err(format, arg...)	\
({printf("[buf2fb] error: " format, ## arg);0;})


int __calc_overlay_size(int *buffer_width_p, int buffer_height, int peripheral_w, int peripheral_h, int scale, int format,
						int *overlay_width, int *overlay_height, int *overlay_xoffset, int *overlay_yoffset, int *osd_width, int *osd_height)
{
	int tmp;
	double ratio_b, ratio_p;
	int buffer_width = *buffer_width_p;

	/* No scale */
	if (scale == 0) {
		if (buffer_width > peripheral_w || buffer_height > peripheral_h) {
			dlog_err("When no scale, buffer size can NOT exceed peripheral device size\n");
			return -EINVAL;
		}
		*overlay_width = buffer_width;
		*overlay_height = buffer_height;
		*osd_width = peripheral_w;
		*osd_height = peripheral_h;
	}
	else if (scale == 1) {
		ratio_b = (double)buffer_width/buffer_height;
		ratio_p = (double)peripheral_w/peripheral_h;
		if (ratio_b >= ratio_p) {
			*overlay_width = peripheral_w;
			tmp = peripheral_w*buffer_height;
			*overlay_height = tmp/buffer_width;
			if ((tmp%buffer_width)*2 >= buffer_width)		// round
				*overlay_height += 1;
			if (*overlay_height > peripheral_h)
				*overlay_height = peripheral_h;
		}
		else {
			*overlay_height = peripheral_h;
			tmp = peripheral_h*buffer_width;
			*overlay_width = tmp/buffer_height;
			if ((tmp%buffer_height)*2 >= buffer_height)		// round
				*overlay_width += 1;
			if (*overlay_width > peripheral_w)
				*overlay_width = peripheral_w;
		}
		*osd_width = peripheral_w;
		*osd_height = peripheral_h;
	}
	else if (scale == 2) {
		*overlay_width = peripheral_w;
		*overlay_height = peripheral_h;
		*osd_width = peripheral_w;
		*osd_height = peripheral_h;
	}
	else if (scale == 3) {
		ratio_b = (double)buffer_width/buffer_height;
		ratio_p = (double)peripheral_w/peripheral_h;
		if (ratio_b >= ratio_p) {
			*osd_width = buffer_width;
			tmp = buffer_width*peripheral_h;
			*osd_height = tmp/peripheral_w;
			if ((tmp%peripheral_w)*2 >= peripheral_w)		// round
				*osd_height += 1;
			if (*osd_height < buffer_height)
				*osd_height = buffer_height;
		}
		else {
			*osd_height = buffer_height;
			tmp = buffer_height*peripheral_w;
			*osd_width = tmp/peripheral_h;
			if ((tmp%peripheral_h)*2 >= peripheral_h)		// round
				*osd_width += 1;
			if (*osd_width < buffer_width)
				*osd_width = buffer_width;
		}
		*overlay_width = buffer_width;
		*overlay_height = buffer_height;
	}
	else if (scale == 4) {
		*overlay_width = buffer_width;
		*overlay_height = buffer_height;
		*osd_width = *overlay_width;
		*osd_height = *overlay_height;
	}
	else {
		dlog_err("Invalid scale %d\n", scale);
		return -EINVAL;
	}

	if (scale == 2 || scale == 4) {
		*overlay_xoffset = 0;
		*overlay_yoffset = 0;
	}
	else if (scale == 0 || scale == 1) {
		*overlay_xoffset = (peripheral_w - *overlay_width)/2;
		*overlay_yoffset = (peripheral_h - *overlay_height)/2;
	}
	else if (scale == 3) {
		*overlay_xoffset = (*osd_width - buffer_width)/2;
		*overlay_yoffset = (*osd_height - buffer_height)/2;
	}
	else {
		dlog_err("Invalid scale %d\n", scale);
		return -EINVAL;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int overlay, phys_addr, width, height, format, scale;
	unsigned int alpha=0xFFFFFF;
	int peripheral_w, peripheral_h;
	int overlay_w, overlay_h, overlay_xoffset, overlay_yoffset, osd_w, osd_h;
	struct display_fb_win_size win_size;
	struct fb_var_screeninfo var;
	char str_dev_node[16] = {0};

	if (argc != 7 && argc != 8 && argc != 9)
		goto usage;

	if (argc >= 8)
		alpha = strtoul(argv[7], NULL, 16);

	if (argc == 9)
		debug = strtoul(argv[8], NULL, 10);

	overlay = strtoul(argv[1], NULL, 10);
	phys_addr = strtoul(argv[2], NULL, 16);
	width = strtoul(argv[3], NULL, 10);
	height = strtoul(argv[4], NULL, 10);
	scale = strtoul(argv[5], NULL, 10);
	format = strtoul(argv[6], NULL, 10);

	dlog_info("overlay=%d, phys_addr=0x%X, width=%d, height=%d, scale=%d, format=%d, alpha=0x%X\n",
			overlay, phys_addr, width, height, scale, format, alpha);

	memset(&var, 0, sizeof(var));

	sprintf(str_dev_node, "/dev/fb%d", overlay);
	dlog_dbg("open %s\n", str_dev_node);
	fd = open(str_dev_node, O_RDWR|O_SYNC);	
	if (fd < 0) {
		dlog_err("Failed to open %s\n", str_dev_node);
		return fd;
	}

	dlog_dbg("FBIOBLANK: POWERDOWN\n");
	ret = ioctl(fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	if (ret) {
		dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIO_GET_PERIPHERAL_SIZE\n");
	ret = ioctl(fd, FBIO_GET_PERIPHERAL_SIZE, &win_size);
	if (ret) {
		dlog_err("ioctl FBIO_GET_PERIPHERAL_SIZE failed: %s\n", strerror(errno));
		return ret;
	}
	peripheral_w = win_size.width;
	peripheral_h = win_size.height;

	dlog_dbg("__calc_overlay_size\n");
	ret = __calc_overlay_size(&width, height, peripheral_w, peripheral_h, scale, format,
						&overlay_w, &overlay_h, &overlay_xoffset, &overlay_yoffset, &osd_w, &osd_h);
	if (ret) {
		dlog_err("calculate overlay size failed\n");
		return ret;
	}

	dlog_info("peripheral width=%d, height=%d, overlay width=%d, height=%d, xoffset=%d, yoffset=%d, buffer width=%d\n",
				peripheral_w, peripheral_h, overlay_w, overlay_h, overlay_xoffset, overlay_yoffset, width);

	win_size.width = osd_w;
	win_size.height = osd_h;

	dlog_dbg("FBIO_SET_OSD_SIZE\n");
	ret = ioctl(fd, FBIO_SET_OSD_SIZE, &win_size);
	if (ret) {
		dlog_err("ioctl FBIO_SET_OSD_SIZE failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIOGET_VSCREENINFO\n");
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
	if (ret) {
		dlog_err("ioctl FBIOGET_VSCREENINFO failed: %s\n", strerror(errno));
		return ret;
	}

	var.xres = width;
	var.yres = height;
	var.xres_virtual = width;
	var.yres_virtual = height;
	var.xoffset = 0;
	var.yoffset = 0;
	var.width = overlay_w;
	var.height = overlay_h;
	var.reserved[0] = overlay_xoffset;
	var.reserved[1] = overlay_yoffset;
	if (format == 0)
		var.grayscale = V4L2_PIX_FMT_RGB32;
	else if (format == 1)
		var.grayscale = V4L2_PIX_FMT_BGR32;
	else if (format == 2)
		var.grayscale = V4L2_PIX_FMT_RGB565;
	else if (format == 3)
		var.grayscale = V4L2_PIX_FMT_NV12;
	else if (format == 4)
		var.grayscale = V4L2_PIX_FMT_NV21;
	else {
		dlog_err("Invalid format %d\n", format);
		goto usage;
	}
	var.activate = FB_ACTIVATE_FORCE;		// necessary

	dlog_dbg("FBIOPUT_VSCREENINFO\n");
	ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var);
	if (ret) {
		dlog_err("ioctl FBIOPUT_VSCREENINFO failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIO_SET_BUFFER_ADDR\n");
	ret = ioctl(fd, FBIO_SET_BUFFER_ADDR, phys_addr);
	if (ret) {
		dlog_err("ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", strerror(errno));
		return ret;
	}

	if (overlay) {
		dlog_dbg("FBIO_SET_ALPHA_TYPE\n");
		ret = ioctl(fd, FBIO_SET_ALPHA_TYPE, 0);
		if (ret) {
			dlog_err("ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", strerror(errno));
		}
		dlog_dbg("FBIO_SET_ALPHA_VALUE\n");
		ret = ioctl(fd, FBIO_SET_ALPHA_VALUE, alpha);
		if (ret) {
			dlog_err("ioctl FBIO_SET_ALPHA_VALUE failed: %s\n", strerror(errno));
		}
	}
	dlog_dbg("FBIOBLANK: UNBLANK\n");
	ret = ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK);
	if (ret) {
		dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("end\n");

	close(fd);

	return 0;

usage:
	printf("Usage:\n"
			"\tbuf2fb <overlay> <phys_addr> <width> <height> <scale> <format> [alpha] [debug]\n"
			"\t@overlay: which overlay to use\n"
			"\t@phys_addr: buffer physical address in HEX\n"
			"\t@width & @height: buffer width and height\n"
			"\t@scale: 0: Not scale. 1: Prescaler - Fit screen & keep ratio. 2: Prescaler - Fit & Full screen, not keep ratio.\n"
			"\t\t3: Postscaler - Fit screen & keep ratio. 4: Postscaler - Fit & Full screen, not keep ratio.\n"
			"\t@format: 0: RGB32. 1: BGR32. 2: RGB565 3: NV12. 4: NV21.\n"
			"\t@alpha: plane blending mode, alpha value in RGB888. Default 0xFFFFFF. Not valid for overlay0.\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");
	return -EINVAL;
}

