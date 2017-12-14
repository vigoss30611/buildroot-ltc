/* display scaler driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>
#include <linux/fb.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"scaler"

/* spline algorithm image quality is better than bilinear */

static unsigned int csclar_spline[] = {
	0x1000, 0x0ff6, 0x0fda, 0x0fab, 0x0f6c, 0x0f1d, 0x0ec1, 0x0e56,
	0x0de0, 0x0d5f, 0x0cd4, 0x0c40, 0x0ba4, 0x0b02, 0x0a5b, 0x09af,
	0x0900, 0x084f, 0x079e, 0x06ec, 0x063c, 0x058e, 0x04e5, 0x043f,
	0x03a0, 0x0308, 0x0278, 0x01f1, 0x0174, 0x0103, 0x009f, 0x0048,

	0x0000, 0x203c, 0x2071, 0x209e, 0x20c4, 0x20e4, 0x20fe, 0x2111,
	0x2120, 0x212a, 0x212f, 0x212f, 0x212c, 0x2125, 0x211c, 0x210f,
	0x2100, 0x20ef, 0x20dd, 0x20c9, 0x20b4, 0x209f, 0x208a, 0x2074,
	0x2060, 0x204d, 0x203b, 0x202a, 0x201c, 0x2010, 0x2008, 0x2002,
};

static unsigned int csclar_bilinear[] = {
	0x1000, 0x0ff8, 0x0fe1, 0x0fbb, 0x0f88, 0x0f48, 0x0efb, 0x0ea3,
	0x0e40, 0x0dd3, 0x0d5d, 0x0cde, 0x0c58, 0x0bcb, 0x0b37, 0x0a9e,
	0x0a00, 0x095e, 0x08b9, 0x0811, 0x0768, 0x06be, 0x0613, 0x0569,
	0x04c0, 0x0419, 0x0375, 0x02d4, 0x0238, 0x01a1, 0x010f, 0x0084,

	0x0000, 0x2078, 0x20e1, 0x213b, 0x2188, 0x21c8, 0x21fb, 0x2223,
	0x2240, 0x2253, 0x225d, 0x225e, 0x2258, 0x224b, 0x2237, 0x221e,
	0x2200, 0x21de, 0x21b9, 0x2191, 0x2168, 0x213e, 0x2113, 0x20e9,
	0x20c0, 0x2099, 0x2075, 0x2054, 0x2038, 0x2021, 0x200f, 0x2004,
};
static int __display_auto_config_scaler(int path);

static int scaler_enable(struct display_device *pdev, int en)
{
	int ret;
	int prescaler_en = 0, image_bypass = 1;

	pdev->scale_needed = 0;

	if (en) {
		ret = __display_auto_config_scaler(pdev->path);
		if (ret) {
			dlog_err("auto config scaler failed %d\n", ret);
			return ret;
		}
		if (!pdev->scale_needed)
			en = 0;
		else {
			if (pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER)
				prescaler_en = 1;
			else
				image_bypass = 0;
		}
	}

	pdev->enable = en;
	ids_write(pdev->path, OVCDCR, OVCDCR_Image_bypass, 1, image_bypass);
	ids_writeword(pdev->path, SCLCR, (((!pdev->scale_needed) & 0x01) << SCLCR_BYPASS)
								| ((pdev->scale_needed & 0x01) << SCLCR_ENABLE));
	ids_write(pdev->path, OVCPSCCR, OVCPSCCR_EN, 1, prescaler_en);
	osd_load_param(pdev);

	return 0;
}

static int scaler_set_mode(struct display_device *pdev, enum display_scaler_mode mode)
{
	pdev->scaler_mode = mode;
	ids_write(pdev->path, OVCDCR, OVCDCR_ScalerMode, 1, mode);
	osd_load_param(pdev);

	return 0;
}

static int prescaler_set_channel(struct display_device *pdev, int chn)
{
	pdev->prescaler_chn = chn;
	ids_write(pdev->path, OVCPSCCR, OVCPSCCR_WINSEL, 3, chn);
	osd_load_param(pdev);

	return 0;
}

static int scaler_set_size(struct display_device *pdev, int overlay)
{
	dtd_t dtd_in, dtd_out;
	int vic_in, vic_out;
	int ret;
	int in_height, out_height;
	int ratio_width, ratio_height;
	int scale_algorithm;
	int i;

	if (pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER) {
		vic_in = DISPLAY_SPECIFIC_VIC_BUFFER_START + overlay;
		vic_out = DISPLAY_SPECIFIC_VIC_OVERLAY_START + overlay;
	}
	else if (pdev->scaler_mode == DISPLAY_SCALER_MODE_POSTSCALER) {
		vic_in = DISPLAY_SPECIFIC_VIC_OSD;
		vic_out = DISPLAY_SPECIFIC_VIC_PERIPHERAL;
	}
	else {
		dlog_err("Invalid scaler mode %d\n", pdev->scaler_mode);
		return -EINVAL;
	}

	ret = vic_fill(pdev->path, &dtd_in, vic_in);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	ret = vic_fill(pdev->path, &dtd_out, vic_out);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}

	if (dtd_out.mInterlaced) {
		dlog_dbg("interlaced\n");
		/* if prescaler+tvif interlaced, both scaler input & output height limit to even,
				and scaler output height*2 must be equal or larger than peripheral height,
				scaler input height*2 must be equal or smaller than buffer height.
				refers to fb module display_fb_set_par() description.
				according to all Q3 series limit */
		dtd_in.mVActive = dtd_in.mVActive & (~0x01);
		dtd_out.mVActive = (dtd_out.mVActive+1) & (~0x01);
	}

	if (pdev->scaler_mode == DISPLAY_SCALER_MODE_PRESCALER) {
		ids_writeword(pdev->path, OVCPSCPCR, ((dtd_in.mVActive - 1) << 16) | (dtd_in.mHActive - 1));
	}

	in_height = dtd_out.mInterlaced? (dtd_in.mVActive/2) : dtd_in.mVActive;
	out_height = dtd_out.mInterlaced? (dtd_out.mVActive/2) : dtd_out.mVActive;
	ids_writeword(pdev->path, SCLIFSR, (in_height << 16) | dtd_in.mHActive);
	ids_writeword(pdev->path, SCLOFSR, (out_height << 16) | dtd_out.mHActive);

	ratio_width = dtd_in.mHActive * 8192;
	do_div(ratio_width, dtd_out.mHActive);
	ratio_height = dtd_in.mVActive * 8192;
	do_div(ratio_height, dtd_out.mVActive);
	ids_writeword(pdev->path, RHSCLR, ratio_width & 0x1ffff);
	ids_writeword(pdev->path, RVSCLR, ratio_height & 0x1ffff);

	ids_writeword(pdev->path, SCLAZIR, 0);
	if ((dtd_in.mHActive >= dtd_out.mHActive*4) || (dtd_in.mVActive >= dtd_out.mVActive*4))
		scale_algorithm = 1;
	else
		scale_algorithm = 2;
	ids_writeword(pdev->path, SCLAZOR, scale_algorithm);
	if (scale_algorithm == 1) {
		ids_writeword(pdev->path, CSCLAR0, 0x0400);
		ids_writeword(pdev->path, CSCLAR1, 0x0400);
		ids_writeword(pdev->path, CSCLAR2, 0x0400);
		ids_writeword(pdev->path, CSCLAR3, 0x0400);
	}
	else if (scale_algorithm == 2) {
		for (i = 0; i < 64; i++)
			ids_writeword(pdev->path, CSCLAR0+i*4, csclar_spline[i]);
	}
	else if (scale_algorithm == 4) {
		for (i = 0; i < 64; i++)
			ids_writeword(pdev->path, CSCLAR0+i*4, csclar_bilinear[i]);
	}

	dlog_dbg("scale_algorithm = %d\n", scale_algorithm);

	osd_load_param(pdev);

	return 0;
}

static int prescaler_set_vw_width(struct display_device *pdev, int vw_width)
{
	pdev->prescaler_vw_width = vw_width;
	ids_writeword(pdev->path, OVCPSVSSR, vw_width & 0xFFFF);
	osd_load_param(pdev);

	return 0;
}

static int __prescaler_set_cbcr_buffer_addr(struct display_device *pdev, int addr)
{
	dtd_t dtd;
	int ret;
	int addr_offset;
	int cbcr_buffer_addr;

	ret = vic_fill(pdev->path, &dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START + pdev->prescaler_chn);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	if (dtd.mVBorder % 2) {
		dlog_err("overlay%d NV12/NV21 buffer yoffset can NOT be odd.\n", pdev->prescaler_chn);
		return -EINVAL;
	}
	addr_offset = ((dtd.mHImageSize * dtd.mVBorder)>>1) + dtd.mHBorder;
	cbcr_buffer_addr = addr + (dtd.mHImageSize * dtd.mVImageSize) + addr_offset;
	//dlog_dbg("addr_offset = %d, cbcr_buffer_addr=0x%X\n", addr_offset, cbcr_buffer_addr);
	if (pdev->buf_idx == 0)
		ids_writeword(pdev->path, OVCPSCB0SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 1)
		ids_writeword(pdev->path, OVCPSCB1SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 2)
		ids_writeword(pdev->path, OVCPSCB2SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 3)
		ids_writeword(pdev->path, OVCPSCB3SAR, cbcr_buffer_addr);

	return 0;
}

static int scaler_set_bpp(struct display_device *pdev,
			enum display_data_format_bpp bpp, enum display_yuv_format yuv_format)
{
	pdev->scaler_bpp = bpp;
	pdev->scaler_yuv_format = yuv_format;		// actually yuv format only take effect in overlay module
	ids_write(pdev->path, OVCPSCCR, OVCPSCCR_BPPMODE, 5, bpp);
	if (bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		__prescaler_set_cbcr_buffer_addr(pdev, pdev->scaler_buffer_addr);
	osd_load_param(pdev);

	return 0;
}

static int prescaler_set_buffer_addr(struct display_device *pdev, int addr)
{
	int ret;
	dtd_t dtd;
	int addr_offset;

	pdev->scaler_buffer_addr = addr;

	ret = vic_fill(pdev->path, &dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START + pdev->prescaler_chn);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	addr_offset = dtd.mHImageSize * dtd.mVBorder + dtd.mHBorder;
	if (pdev->scaler_bpp == DISPLAY_DATA_FORMAT_32BPP_ARGB8888
			|| pdev->scaler_bpp == DISPLAY_DATA_FORMAT_24BPP_RGB888)
		addr_offset *= 4;
	else if (pdev->scaler_bpp == DISPLAY_DATA_FORMAT_16BPP_RGB565)
		addr_offset *= 2;
	else
		addr_offset *= 1;

	osd_unload_param(pdev);
	if (pdev->buf_idx == 0)
		ids_writeword(pdev->path, OVCPSB0SAR, addr + addr_offset);
	else if (pdev->buf_idx == 1)
		ids_writeword(pdev->path, OVCPSB1SAR, addr + addr_offset);
	else if (pdev->buf_idx == 2)
		ids_writeword(pdev->path, OVCPSB2SAR, addr + addr_offset);
	else if (pdev->buf_idx == 3)
		ids_writeword(pdev->path, OVCPSB3SAR, addr + addr_offset);

	if (pdev->scaler_bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		__prescaler_set_cbcr_buffer_addr(pdev, addr);
	ids_write(pdev->path, OVCPSCCR, OVCPSCCR_BUFSEL, 2, pdev->buf_idx);
	osd_load_param(pdev);

	pdev->buf_idx = (pdev->buf_idx+1)%4;

	return 0;
}

//=========================================

static int __display_auto_config_scaler(int path)
{
	struct display_function *func;
	struct display_device *fpdev;
	struct display_device *opdev;
	struct display_device *spdev;
	int ret;
	int i;
	int cnt = 0;
	dtd_t buffer_dtd;
	dtd_t overlay_dtd;
	dtd_t osd_dtd;
	dtd_t peripheral_dtd;
	int vw_width;

	ret = display_get_device(path, DISPLAY_MODULE_TYPE_FB, &fpdev);
	if (ret) {
		dlog_err("get fb device failed %d\n", ret);
		return ret;
	}
	if (!fpdev->enable) {
		dlog_dbg("scaler will not take effect when path disabled.\n");
		return 0;
	}

	ret = display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &opdev);
	if (ret) {
		dlog_err("get overlay device failed %d\n", ret);
		return 0;
	}

	ret = display_get_device(path, DISPLAY_MODULE_TYPE_SCALER, &spdev);
	if (ret) {
		dlog_dbg("there's no scaler module.\n");
		return 0;
	}

	spdev->scale_needed = 0;

	/* compare osd size and peripheral size */
	ret = vic_fill(path, &osd_dtd, DISPLAY_SPECIFIC_VIC_OSD);
	if (ret) {
		dlog_err("osd vic %d not found\n", DISPLAY_SPECIFIC_VIC_OSD);
		return ret;
	}
	ret = vic_fill(path, &peripheral_dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
	if (ret) {
		dlog_err("peripheral vic %d not found\n", DISPLAY_SPECIFIC_VIC_PERIPHERAL);
		return ret;
	}
	dlog_dbg("osd=%d*%d, peripheral=%d*%d\n", osd_dtd.mHActive, osd_dtd.mVActive,
						peripheral_dtd.mHActive, peripheral_dtd.mVActive);
	if (osd_dtd.mHActive != peripheral_dtd.mHActive
			|| osd_dtd.mVActive != peripheral_dtd.mVActive) {
		cnt++;
		if (cnt > 1)
			goto err_multi_need;
		/* config postscaler */
		dlog_dbg("use postscaler\n");
		scaler_set_mode(spdev, DISPLAY_SCALER_MODE_POSTSCALER);
		ret = scaler_set_size(spdev, 0);
		if (ret)
			return ret;
		spdev->scale_needed = 1;
	}

	/* compare buffer size and overlay size if postscaler is not needed */
	if (!spdev->scale_needed) {
		for (i = 0; i < IDS_OVERLAY_NUM; i++) {
			ret = vic_fill(path, &buffer_dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START+i);
			if (ret) {
				dlog_err("buffer vic %d not found\n", DISPLAY_SPECIFIC_VIC_BUFFER_START+i);
				return ret;
			}
			ret = vic_fill(path, &overlay_dtd, DISPLAY_SPECIFIC_VIC_OVERLAY_START+i);
			if (ret) {
				dlog_err("overlay vic %d not found\n", DISPLAY_SPECIFIC_VIC_OVERLAY_START+i);
				return ret;
			}
			dlog_dbg("overlay%d: buffer=%d*%d, overlay=%d*%d, buffer_addr=0x%X, %s\n", i,
					buffer_dtd.mHActive, buffer_dtd.mVActive, overlay_dtd.mHActive,
					overlay_dtd.mVActive, opdev->overlays[i].buffer_addr,
					opdev->overlays[i].bpp==17?(opdev->overlays[i].yuv_format==1?"NV12":"NV21"):"RGB");
#if defined(CONFIG_ARCH_Q3F) || defined(CONFIG_ARCH_APOLLO3)
			if (i != 0 && (buffer_dtd.mHActive != overlay_dtd.mHActive
					|| buffer_dtd.mVActive != overlay_dtd.mVActive)
					&& buffer_dtd.mHActive && buffer_dtd.mVActive
					&& overlay_dtd.mHActive && overlay_dtd.mVActive
					&& opdev->overlays[i].buffer_addr) {
				dlog_err("for Q3F, prescaler is always used on overlay0, exclude other overlay\n");
				return -EINVAL;
			}
			if (i == 0 && buffer_dtd.mHActive && buffer_dtd.mVActive
					&& overlay_dtd.mHActive && overlay_dtd.mVActive
					&& opdev->overlays[i].buffer_addr)		// always use prescaler for overlay0
#else
			if ((buffer_dtd.mHActive != overlay_dtd.mHActive
					|| buffer_dtd.mVActive != overlay_dtd.mVActive)
					&& buffer_dtd.mHActive && buffer_dtd.mVActive
					&& overlay_dtd.mHActive && overlay_dtd.mVActive
					&& opdev->overlays[i].buffer_addr)
#endif
			{
				cnt++;
				if (cnt > 1)
					goto err_multi_need;
				/* config prescaler */
				dlog_dbg("use prescaler\n");
				if (buffer_dtd.mHImageSize < buffer_dtd.mHActive) {
					dlog_err("overlay%d buffer virtual width %d can NOT be less than buffer width %d.\n",
									i, buffer_dtd.mHImageSize, buffer_dtd.mHActive);
					return -EINVAL;
				}
				vw_width = buffer_dtd.mHImageSize;
				dlog_dbg("vw_width is %d\n", vw_width);
				scaler_set_mode(spdev, DISPLAY_SCALER_MODE_PRESCALER);
				ret = scaler_set_size(spdev, i);
				if (ret)
					return ret;
				scaler_set_bpp(spdev, opdev->overlays[i].bpp, opdev->overlays[i].yuv_format);
				func->set_overlay_bpp(opdev, i, DISPLAY_DATA_FORMAT_32BPP_ARGB8888,
														opdev->overlays[i].yuv_format, 1);
				prescaler_set_channel(spdev, i);
				prescaler_set_vw_width(spdev, vw_width);
				prescaler_set_buffer_addr(spdev, opdev->overlays[i].buffer_addr);
				spdev->scale_needed = 1;
			}
		}
	}

	dlog_dbg("scaler %s\n", spdev->scale_needed?"needed":"NOT needed");

	/* Q3 limit: When output 4K * 2K resolution, scaler can NOT work well, suggest NOT to use scaler */
	if (peripheral_dtd.mHActive >= 3840 && spdev->scale_needed)
		dlog_warn("When output 4K * 2K resolution, scaler can NOT work well, suggest NOT to use scaler\n");

	return 0;

err_multi_need:
	dlog_err("Multiple place need scaler, but there's only one scaler. check param.\n");
	return -EINVAL;
}

static int scaler_probe(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static int scaler_remove(struct display_device *pdev)
{
	dlog_trace();
	pdev->buf_idx = 0;
	return 0;
}

static struct display_driver scaler_driver = {
	.probe  = scaler_probe,
	.remove = scaler_remove,
	.driver = {
		.name = "scaler",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_SCALER,
	.function = {
		.enable = scaler_enable,
		.set_scaler_mode = scaler_set_mode,
		.set_scaler_size = scaler_set_size,
		.set_scaler_bpp = scaler_set_bpp,
		.set_prescaler_channel = prescaler_set_channel,
		.set_prescaler_vw_width = prescaler_set_vw_width,
		.set_prescaler_buffer_addr = prescaler_set_buffer_addr,
	}
};

module_display_driver(scaler_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display scaler driver");
MODULE_LICENSE("GPL");
