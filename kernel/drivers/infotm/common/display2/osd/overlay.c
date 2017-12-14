/* display overlay driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/clk.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"overlay"


static int osd_set_background_color(struct display_device *pdev, int color)
{
	pdev->osd_background_color = color;
	ids_writeword(pdev->path, OVCBKCOLOR, color);
	osd_load_param(pdev);

	return 0;
}

static int osd_set_vic(struct display_device *pdev, int vic)
{
	int ret;
	dtd_t dtd;
	pdev->vic = vic;
	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	ids_writeword(pdev->path, OVCWPR, ((dtd.mVActive - 1) << 12) | (dtd.mHActive - 1));
	return 0;
}

static int overlay_enable(struct display_device *pdev, int overlay, int en)
{
	int OVCWxCR = __overlay_OVCWx(overlay, EOVCWxCR);
	dtd_t dtd;
	int ret;
	if (en && !pdev->overlays[overlay].buffer_addr) {
		dlog_dbg("buffer address is blank, skip enable overlay%d\n", overlay);
		return 0;
	}

	ret = vic_fill(pdev->path, &dtd, pdev->overlays[overlay].vic);
	if (en && !(!ret && dtd.mHActive && dtd.mVActive)) {
		dlog_dbg("vic, width or height is blank, skip enable overlay%d\n", overlay);
		return 0;
	}

	pdev->overlays[overlay].enable = en;
	ids_write(pdev->path, OVCWxCR, OVCWxCR_ENWIN, 1, en);
	osd_load_param(pdev);

	return 0;
}

static int __overlay_OVCDCR_init(struct display_device *pdev)
{
	unsigned int val;
	int scaler_mode, iftype, interlace, image_bypass;

	val = ids_readword(pdev->path, OVCDCR);
	image_bypass = (val >> OVCDCR_Image_bypass) & 0x01;
	scaler_mode = (val >> OVCDCR_ScalerMode) & 0x01;
	iftype = (val >> OVCDCR_IfType) & 0x01;
	interlace = (val >> OVCDCR_Interlace) & 0x01;

	val = (image_bypass << OVCDCR_Image_bypass)
			| (0 << OVCDCR_w1_sp_load)
			| (0 << OVCDCR_w0_sp_load)
			| (0 << OVCDCR_sp_load_en)
			| (1 << OVCDCR_UpdateReg)
			| (scaler_mode << OVCDCR_ScalerMode)
			| (1 << OVCDCR_Enrefetch)
			| (1 << OVCDCR_Enrelax)
			| (0x0F << OVCDCR_WaitTime)
			| (0 << OVCDCR_SafeBand)
			| (0 << OVCDCR_AllFetch)
			| (iftype << OVCDCR_IfType)
			| (interlace << OVCDCR_Interlace);
	ids_writeword(pdev->path, OVCDCR, val);

	return 0;
}


static int __overlay_set_output_size_and_position(
			struct display_device *pdev, int overlay, int vic, int left_top_x, int left_top_y)
{
	int ret;
	dtd_t dtd;
	int OVCWxPCAR = __overlay_OVCWx(overlay, EOVCWxPCAR);
	int OVCWxPCBR = __overlay_OVCWx(overlay, EOVCWxPCBR);

	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}

	ids_writeword(pdev->path, OVCWxPCAR, \
			(left_top_x << OVCWxPCAR_LeftTopX) | (left_top_y << OVCWxPCAR_LeftTopY));
	ids_writeword(pdev->path, OVCWxPCBR,	\
			((left_top_x+dtd.mHActive-1) << OVCWxPCBR_RightBotX)	\
			| ((left_top_y+dtd.mVActive-1) << OVCWxPCBR_RightBotY));

	return 0;
}

static int __overlay_set_cbcr_buffer_addr(struct display_device *pdev, int overlay, int addr)
{
	dtd_t dtd, osd_dtd, p_dtd;
	int ret;
	int cbcr_buffer_addr;
	int addr_offset;

	ret = vic_fill(pdev->path, &dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START+overlay);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	ret = vic_fill(pdev->path, &osd_dtd, pdev->vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	ret = vic_fill(pdev->path, &p_dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	if (dtd.mHImageSize < dtd.mHActive) {
		dlog_err("overlay%d buffer virtual width %d can NOT be less than buffer width %d.\n",
						overlay, dtd.mHImageSize, dtd.mHActive);
		return -EINVAL;
	}
	if (dtd.mVImageSize < dtd.mVActive) {
		dlog_err("overlay%d buffer virtual height %d can NOT be less than buffer height %d.\n",
						overlay, dtd.mVImageSize, dtd.mVActive);
		return -EINVAL;
	}
	if (dtd.mVBorder % 2) {
		dlog_err("overlay%d NV12/NV21 buffer yoffset can NOT be odd.\n", overlay);
		return -EINVAL;
	}
	addr_offset = ((dtd.mHImageSize * dtd.mVBorder)>>1) + dtd.mHBorder;

	/* when lcdc, not full screen, overlay fetch YCbCr buffer, color will shift to right for 8 pixel.
		color shift can be solved by add 8 to CbCr start address.
		but the LeftTop will still has several bad pixels.
	 	according to apollo3 limit */
#ifdef CONFIG_ARCH_APOLLO3
	if (!p_dtd.mInterlaced && (dtd.mHActive != osd_dtd.mHActive || dtd.mVActive != osd_dtd.mVActive))
		cbcr_buffer_addr = addr + (dtd.mHImageSize * dtd.mVImageSize) + addr_offset + 8;
	else
#endif
		cbcr_buffer_addr = addr + (dtd.mHImageSize * dtd.mVImageSize) + addr_offset;

	//dlog_dbg("addr_offset = %d, cbcr_buffer_addr=0x%X\n", addr_offset, cbcr_buffer_addr);
	if (pdev->buf_idx == 0)
		ids_writeword(pdev->path, OVCBRB0SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 1)
		ids_writeword(pdev->path, OVCBRB1SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 2)
		ids_writeword(pdev->path, OVCBRB2SAR, cbcr_buffer_addr);
	else if (pdev->buf_idx == 3)
		ids_writeword(pdev->path, OVCBRB3SAR, cbcr_buffer_addr);

	return 0;
}

static int overlay_set_vic(struct display_device *pdev, int overlay, int vic)
{
	pdev->overlays[overlay].vic = vic;
	__overlay_OVCDCR_init(pdev);
	__overlay_set_output_size_and_position(pdev, overlay, vic,
			pdev->overlays[overlay].left_top_x, pdev->overlays[overlay].left_top_y);
	if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		__overlay_set_cbcr_buffer_addr(pdev, overlay, pdev->overlays[overlay].buffer_addr);
	osd_load_param(pdev);

	return 0;
}

static int overlay_set_position(struct display_device *pdev, int overlay, int lt_x, int lt_y)
{
	pdev->overlays[overlay].left_top_x = lt_x;
	pdev->overlays[overlay].left_top_y = lt_y;
	__overlay_set_output_size_and_position(pdev, overlay, pdev->overlays[overlay].vic, lt_x, lt_y);
	osd_load_param(pdev);

	return 0;
}

static int overlay_set_vw_width(struct display_device *pdev, int overlay, int vw_width)
{
	int OVCWxVSSR = __overlay_OVCWx(overlay, EOVCWxVSSR);

	pdev->overlays[overlay].vw_width = vw_width;
	ids_writeword(pdev->path, OVCWxVSSR, vw_width & 0xFFFF);
	osd_load_param(pdev);

	return 0;
}

static const int yuv2rgb_nv12[] = {298, 0, 409, 298, 1948, 1840, 298, 516, 0};
static const int yuv2rgb_nv21[] = {298, 516, 0, 298, 1948, 1840, 298, 0, 409};

static int __overlay_set_color_coefficient(struct display_device *pdev, int overlay,
			enum display_yuv_format yuv_format)
{
	ids_write(pdev->path, OVCOMC, OVCOMC_ToRGB, 1, 0);

	if (yuv_format == DISPLAY_YUV_FORMAT_NV12) {
		ids_writeword(pdev->path, OVCOEF11, yuv2rgb_nv12[0]);
		ids_writeword(pdev->path, OVCOEF12, yuv2rgb_nv12[1]);
		ids_writeword(pdev->path, OVCOEF13, yuv2rgb_nv12[2]);
		ids_writeword(pdev->path, OVCOEF21, yuv2rgb_nv12[3]);
		ids_writeword(pdev->path, OVCOEF22, yuv2rgb_nv12[4]);
		ids_writeword(pdev->path, OVCOEF23, yuv2rgb_nv12[5]);
		ids_writeword(pdev->path, OVCOEF31, yuv2rgb_nv12[6]);
		ids_writeword(pdev->path, OVCOEF32, yuv2rgb_nv12[7]);
		ids_writeword(pdev->path, OVCOEF33, yuv2rgb_nv12[8]);
	}
	else if (yuv_format == DISPLAY_YUV_FORMAT_NV21) {
		ids_writeword(pdev->path, OVCOEF11, yuv2rgb_nv21[0]);
		ids_writeword(pdev->path, OVCOEF12, yuv2rgb_nv21[1]);
		ids_writeword(pdev->path, OVCOEF13, yuv2rgb_nv21[2]);
		ids_writeword(pdev->path, OVCOEF21, yuv2rgb_nv21[3]);
		ids_writeword(pdev->path, OVCOEF22, yuv2rgb_nv21[4]);
		ids_writeword(pdev->path, OVCOEF23, yuv2rgb_nv21[5]);
		ids_writeword(pdev->path, OVCOEF31, yuv2rgb_nv21[6]);
		ids_writeword(pdev->path, OVCOEF32, yuv2rgb_nv21[7]);
		ids_writeword(pdev->path, OVCOEF33, yuv2rgb_nv21[8]);
	}
	else if (yuv_format != DISPLAY_YUV_FORMAT_NOT_USE) {
		dlog_err("Invalid yuv format %d\n", yuv_format);
		return -EINVAL;
	}

	ids_write(pdev->path, OVCOMC, OVCOMC_CBCR_CH, 3, overlay);
	ids_write(pdev->path, OVCOMC, OVCOMC_oft_b, 5, 16);
	ids_write(pdev->path, OVCOMC, OVCOMC_ToRGB, 1, 1); 		// according to bpp

	return 0;
}

static int overlay_set_bpp(struct display_device *pdev, int overlay,
	enum display_data_format_bpp bpp, enum display_yuv_format yuv_format, int use_prescaler)
{
	int OVCWxCR = __overlay_OVCWx(overlay, EOVCWxCR);

	if (!use_prescaler) {
		pdev->overlays[overlay].bpp = bpp;
		pdev->overlays[overlay].yuv_format = yuv_format;
		/* whatever prescaler or overlay, only one CbCr can be used, according to all Q3 series limit */
		if (pdev->overlays[0].bpp == DISPLAY_DATA_FORMAT_YCBCR_420
				&& pdev->overlays[1].bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
			dlog_err("!whatever prescaler or overlay, only one CbCr can be used, according to all Q3 series limit\n");
	}
	ids_write(pdev->path, OVCWxCR, OVCWxCR_BPPMODE, 5, bpp);

	if (bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		__overlay_set_cbcr_buffer_addr(pdev, overlay, pdev->overlays[overlay].buffer_addr);

	if (yuv_format != DISPLAY_YUV_FORMAT_NOT_USE)
		__overlay_set_color_coefficient(pdev, overlay, yuv_format);

	osd_load_param(pdev);

	return 0;
}

static int overlay_set_buffer_addr(struct display_device *pdev, int overlay, int addr)
{
	int OVCWxB0SAR = __overlay_OVCWx(overlay, EOVCWxB0SAR);
	int OVCWxCR =  __overlay_OVCWx(overlay, EOVCWxCR);

	int ret;
	dtd_t dtd;
	int addr_offset;

	pdev->overlays[overlay].buffer_addr = addr;

	ret = vic_fill(pdev->path, &dtd, DISPLAY_SPECIFIC_VIC_BUFFER_START+overlay);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}
	addr_offset = dtd.mHImageSize * dtd.mVBorder + dtd.mHBorder;
	if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_32BPP_ARGB8888
			|| pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_24BPP_RGB888)
		addr_offset *= 4;
	else if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_16BPP_RGB565)
		addr_offset *= 2;
	else
		addr_offset *= 1;

	osd_unload_param(pdev);

	if (pdev->buf_idx == 0)
		OVCWxB0SAR = __overlay_OVCWx(overlay, EOVCWxB0SAR);
	else if (pdev->buf_idx == 1)
		OVCWxB0SAR = __overlay_OVCWx(overlay, EOVCWxB1SAR);
	else if (pdev->buf_idx == 2)
		OVCWxB0SAR = __overlay_OVCWx(overlay, EOVCWxB2SAR);
	else if (pdev->buf_idx == 3)
		OVCWxB0SAR = __overlay_OVCWx(overlay, EOVCWxB3SAR);

	ids_writeword(pdev->path, OVCWxB0SAR, addr + addr_offset);
	if (pdev->overlays[overlay].bpp == DISPLAY_DATA_FORMAT_YCBCR_420)
		__overlay_set_cbcr_buffer_addr(pdev, overlay, addr);

	ids_write(pdev->path, OVCWxCR, OVCWxCR_BUFSEL, 2, pdev->buf_idx);
	osd_load_param(pdev);

	pdev->buf_idx = (pdev->buf_idx+1)%4;
	return 0;
}

static int overlay_set_mapcolor(struct display_device *pdev, int overlay,
			int en, int map_color)
{
	int OVCWxCMR = __overlay_OVCWx(overlay, EOVCWxCMR);

	en = en?1:0;
	pdev->overlays[overlay].mapcolor_en = en;
	pdev->overlays[overlay].mapcolor = map_color;
	ids_writeword(pdev->path, OVCWxCMR, (en << 24) | map_color);
	osd_load_param(pdev);

	return 0;
}

static int overlay_set_red_blue_exchange(struct display_device *pdev, int overlay,
			int rb_exchange)
{
	int OVCWxCR = __overlay_OVCWx(overlay, EOVCWxCR);

	rb_exchange = rb_exchange?1:0;
	pdev->overlays[overlay].rb_exchange = rb_exchange;
	ids_write(pdev->path, OVCWxCR, OVCWxCR_RBEXG, 1, rb_exchange);
	osd_load_param(pdev);

	return 0;
}

static int overlay_set_chroma_key(struct display_device *pdev, int overlay,
			int enable, int chroma_key, int alpha_nonkey_area, int alpha_key_area)
{
	int OVCWxCR = __overlay_OVCWx(overlay, EOVCWxCR);
	int OVCWxPCCR = __overlay_OVCWx(overlay, EOVCWxPCCR);
	int OVCWxCKCR = __overlay_OVCWx(overlay, EOVCWxCKCR);
	int OVCWxCKR = __overlay_OVCWx(overlay, EOVCWxCKR);
	int alpha_nonkey_red = ((alpha_nonkey_area >> 16) >> 4) & 0x0F;
	int alpha_nonkey_green = ((alpha_nonkey_area >> 8) >> 4) & 0x0F;
	int alpha_nonkey_blue = ((alpha_nonkey_area >> 0) >> 4) & 0x0F;
	int alpha_nonkey_reg = (alpha_nonkey_red << 8) | (alpha_nonkey_green << 4) | (alpha_nonkey_blue);
	int alpha_key_red = ((alpha_key_area >> 16) >> 4) & 0x0F;
	int alpha_key_green = ((alpha_key_area >> 8) >> 4) & 0x0F;
	int alpha_key_blue = ((alpha_key_area >> 0) >> 4) & 0x0F;
	int alpha_key_reg = (alpha_key_red << 8) | (alpha_key_green << 4) | (alpha_key_blue);

	if (OVCWxPCCR < 0)
		return OVCWxPCCR;

	enable = enable?1:0;
	pdev->overlays[overlay].ck_enable= enable;
	pdev->overlays[overlay].ck_value = chroma_key;
	pdev->overlays[overlay].ck_alpha_nonkey_area= alpha_nonkey_area;
	pdev->overlays[overlay].ck_alpha_key_area= alpha_key_area;
	if (enable) {
		ids_write(pdev->path, OVCWxCR, OVCWxCR_BLD_PIX, 1, DISPLAY_ALPHA_TYPE_PIXEL);
		ids_write(pdev->path, OVCWxCR, OVCWxCR_ALPHA_SEL, 1, 0);
		ids_writeword(pdev->path, OVCWxPCCR, (alpha_nonkey_reg << 12) | alpha_key_reg);
	}
	else {
		ids_write(pdev->path, OVCWxCR, OVCWxCR_BLD_PIX, 1, DISPLAY_ALPHA_TYPE_PLANE);
		ids_write(pdev->path, OVCWxCR, OVCWxCR_ALPHA_SEL, 1, 1);
		ids_writeword(pdev->path, OVCWxPCCR, 0xFFFFFF);
	}
	ids_writeword(pdev->path, OVCWxCKR, chroma_key);
	ids_write(pdev->path, OVCWxCKCR, OVCW1CKCR_KEYBLEN, 1, enable);
	ids_write(pdev->path, OVCWxCKCR, OVCW1CKCR_KEYEN, 1, enable);
	osd_load_param(pdev);

	return 0;
}

static int overlay_set_alpha(struct display_device *pdev, int overlay,
			enum display_alpha_type alpha_type, int alpha_value)
{
	int OVCWxCR = __overlay_OVCWx(overlay, EOVCWxCR);
	int OVCWxPCCR = __overlay_OVCWx(overlay, EOVCWxPCCR);
	int alpha_red = ((alpha_value >> 16) >> 4) & 0x0F;
	int alpha_green = ((alpha_value >> 8) >> 4) & 0x0F;
	int alpha_blue = ((alpha_value >> 0) >> 4) & 0x0F;
	int alpha_reg = (alpha_red << 8) | (alpha_green << 4) | (alpha_blue);

	dlog_dbg("alpha_type=%d, alpha_value=0x%X\n", alpha_type, alpha_value);

	if (OVCWxPCCR < 0)
		return OVCWxPCCR;

	if (pdev->overlays[overlay].ck_enable) {
		dlog_err("Chroma key function is enabled. Disable chroma key before to use alpha function.\n");
		return -EPERM;
	}

	pdev->overlays[overlay].alpha_type = alpha_type;
	pdev->overlays[overlay].alpha_value = alpha_value;
	ids_write(pdev->path, OVCWxCR, OVCWxCR_BLD_PIX, 1, alpha_type);
	ids_write(pdev->path, OVCWxCR, OVCWxCR_ALPHA_SEL, 1, 0);
	ids_writeword(pdev->path, OVCWxPCCR, alpha_reg<<12);
	osd_load_param(pdev);

	return 0;
}


static int overlay_probe(struct display_device *pdev)
{
	dlog_trace();

	pdev->overlays = kzalloc(sizeof(struct display_overlay) * IDS_OVERLAY_NUM, GFP_KERNEL);
	if (!pdev->overlays) {
		dlog_err("alloc pdev overlays failed\n");
		return -ENOMEM;
	}

	pdev->buf_idx = 0;
	return 0;
}

static int overlay_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver overlay_driver = {
	.probe  = overlay_probe,
	.remove = overlay_remove,
	.driver = {
		.name = "overlay",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_OVERLAY,
	.function = {
		.set_background_color = osd_set_background_color,
		.set_vic = osd_set_vic,
		.overlay_enable = overlay_enable,
		.set_overlay_vic = overlay_set_vic,
		.set_overlay_position = overlay_set_position,
		.set_overlay_vw_width = overlay_set_vw_width,
		.set_overlay_bpp = overlay_set_bpp,
		.set_overlay_buffer_addr = overlay_set_buffer_addr,
		.set_overlay_mapcolor = overlay_set_mapcolor,
		.set_overlay_red_blue_exchange = overlay_set_red_blue_exchange,
		.set_overlay_chroma_key = overlay_set_chroma_key,
		.set_overlay_alpha = overlay_set_alpha,
	}
};

module_display_driver(overlay_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display overlay driver");
MODULE_LICENSE("GPL");
