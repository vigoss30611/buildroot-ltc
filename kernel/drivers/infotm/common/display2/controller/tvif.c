/* display tvif driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/display_device.h>
#include <mach/pad.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"tvif"


static int tvif_enable(struct display_device *pdev, int en)
{
	if (!pdev->enable && en) {
		clk_prepare_enable(pdev->clk_src);
		ids_write(pdev->path, IMAP_TVCCR, 31, 1, 1);
		ids_write(pdev->path, IMAP_TVICR, 31, 1, 1);
	}
	else if (pdev->enable && !en) {
		ids_write(pdev->path, IMAP_TVICR, 31, 1, 0);
		ids_write(pdev->path, IMAP_TVCCR, 31, 1, 0);
		clk_disable_unprepare(pdev->clk_src);
	}
	else
		dlog_dbg("skip duplicated tvif enable operation\n");

	pdev->enable = en;

	return 0;
}

static int __tvif_set_clkdiv(struct display_device *pdev, int sub_divider)
{
	dlog_dbg("tvif clock divider is %d\n", sub_divider);
	ids_write(pdev->path, IMAP_TVCCR, 0, 8, (sub_divider-1));
	return 0;
}

static int __tvif_calc_fps(struct display_device *pdev, struct clk *clk_src,
						int vic, int sub_divider)
{
	dtd_t dtd;
	unsigned long htotal, vtotal, freq, mod;

	vic_fill(pdev->path, &dtd, vic);
	if (dtd.mPixelRepetitionInput)
		htotal = dtd.mHActive*2+dtd.mHBlanking;
	else
		htotal = dtd.mHActive+dtd.mHBlanking;
	if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_BT656)
		htotal -= 8;
	vtotal = (dtd.mVActive/2+dtd.mVBlanking)*2+1;
	freq = clk_get_rate(clk_src);
	do_div(freq, sub_divider);
	mod = do_div(freq, vtotal);	
	if (mod * 2 >= vtotal)
			freq++;
	mod = do_div(freq, htotal);
	if (mod * 2 >= htotal)
		freq++;

	return (int)freq;
}

static int __tvif_set_pixel_clk(struct display_device *pdev, int vic)
{
	int ret;
	int sub_divider;
	int fps;

#ifdef CONFIG_ARCH_APOLLO
	pdev->clk_src = clk_get_sys(IDS_TVIF_CLK_SRC, IDS_CLK_SRC_PARENT);
#else
	pdev->clk_src = clk_get_sys(IDS_EITF_CLK_SRC, IDS_CLK_SRC_PARENT);
#endif
	ret = display_set_clk(pdev, pdev->clk_src, vic, &sub_divider);
	if (ret) {
		dlog_err("set clk failed %d\n", ret);
		return ret;
	}
	ret = __tvif_set_clkdiv(pdev, sub_divider);
	if (ret) {
		dlog_err("tvif set vclk failed %d\n", ret);
		return ret;
	}
	fps = __tvif_calc_fps(pdev, pdev->clk_src, vic, sub_divider);
	dlog_info("tvif fps is %d\n", fps);

	return 0;
}

static int __tvif_set_timing_params(struct display_device *pdev, int vic)
{
	int ret;
	dtd_t dtd;
	unsigned int vbpd, field_width, field_height, tvif_hblanking, htotal;
	signed int coefficient[3][3] = {{66, 129, 25}, {-38, -74, 112}, {112, -94, -18}};

	ret = vic_fill(pdev->path, &dtd, vic);
	if (ret) {
		dlog_err("vic_fill failed\n");
		return ret;
	}

	if (dtd.mInterlaced)
		field_height = dtd.mVActive/2;
	else
		field_height = dtd.mVActive;

	if (dtd.mPixelRepetitionInput)
		field_width = dtd.mHActive*2;
	else
		field_width = dtd.mHActive;

	htotal = field_width + dtd.mHBlanking;

	if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_BT656)
		tvif_hblanking = dtd.mHBlanking - 8;
	else
		tvif_hblanking = dtd.mHBlanking;

	ids_write(pdev->path, OVCDCR, OVCDCR_Interlace, 1, dtd.mInterlaced);

	ids_write(pdev->path, IMAP_TVCCR, 9, 1, !dtd.mVclkInverse);
	ids_write(pdev->path, IMAP_TVICR, 13, 1, dtd.mVSyncPolarity);
	ids_write(pdev->path, IMAP_TVICR, 12, 1, dtd.mHSyncPolarity);

	vbpd = dtd.mVBlanking - dtd.mVSyncOffset;
	ids_writeword(pdev->path, IMAP_TVUBA1, vbpd);
	ids_writeword(pdev->path, IMAP_TVUNBA, field_height);
	ids_writeword(pdev->path, IMAP_TVUBA2, dtd.mVSyncOffset);
	ids_writeword(pdev->path, IMAP_TVLBA1, vbpd+1);
	ids_writeword(pdev->path, IMAP_TVLNBA, field_height);
	ids_writeword(pdev->path, IMAP_TVLBA2, dtd.mVSyncOffset);
	ids_writeword(pdev->path, IMAP_TVBLEN, tvif_hblanking);
	ids_writeword(pdev->path, IMAP_TVVLEN, field_width);
	ids_write(pdev->path, IMAP_TVHSCR, 16, 13, dtd.mHSyncOffset);
	ids_write(pdev->path, IMAP_TVHSCR, 0, 13, dtd.mHSyncPulseWidth);
	ids_write(pdev->path, IMAP_TVVSHCR, 16, 13, dtd.mHSyncOffset);
	ids_write(pdev->path, IMAP_TVVSHCR, 0, 16, dtd.mVSyncPulseWidth*htotal);
	ids_write(pdev->path, IMAP_TVVSLCR, 16, 13, htotal/2+dtd.mHSyncOffset);
	ids_write(pdev->path, IMAP_TVVSLCR, 0, 16, dtd.mVSyncPulseWidth*htotal);
	
	ids_writeword(pdev->path, IMAP_TVXSIZE, dtd.mHActive-1);
	ids_writeword(pdev->path, IMAP_TVYSIZE, dtd.mVActive-1);

	ids_writeword(pdev->path, IMAP_TVCOEF11, (u32)coefficient[0][0]);
	ids_writeword(pdev->path, IMAP_TVCOEF12, (u32)coefficient[0][1]);
	ids_writeword(pdev->path, IMAP_TVCOEF13, (u32)coefficient[0][2]);
	ids_writeword(pdev->path, IMAP_TVCOEF21, (u32)coefficient[1][0]);
	ids_writeword(pdev->path, IMAP_TVCOEF22, (u32)coefficient[1][1]);
	ids_writeword(pdev->path, IMAP_TVCOEF23, (u32)coefficient[1][2]);
	ids_writeword(pdev->path, IMAP_TVCOEF31, (u32)coefficient[2][0]);
	ids_writeword(pdev->path, IMAP_TVCOEF32, (u32)coefficient[2][1]);
	ids_writeword(pdev->path, IMAP_TVCOEF33, (u32)coefficient[2][2]);

	return 0;
}

static int tvif_set_vic(struct display_device *pdev, int vic)
{
	pdev->vic = vic;
	__tvif_set_timing_params(pdev, vic);
	__tvif_set_pixel_clk(pdev, vic);

	return 0;
}

static int tvif_set_interface_type(struct display_device *pdev, 
			enum display_interface_type type)
{
	int bt601 = 0;

	pdev->interface_type = type;

	if (type == DISPLAY_INTERFACE_TYPE_BT601)
		bt601 = 1;
	else if (type == DISPLAY_INTERFACE_TYPE_BT656)
		bt601 = 0;
	else {
		dlog_err("Invalid tvif interface type %d\n", type);
		return -EINVAL;
	}

	__tvif_set_timing_params(pdev, pdev->vic);

	ids_write(pdev->path, LCDCON5, LCDCON5_DSPTYPE, 2, 2);
	ids_write(pdev->path, OVCDCR, OVCDCR_IfType, 1, 1);
	ids_write(pdev->path, IMAP_TVICR, 30, 1, bt601);
	ids_write(pdev->path, IMAP_TVICR, 0, 1, !bt601);
	ids_write(pdev->path, IMAP_TVCMCR, 0, 5, 16);
	ids_write(pdev->path, IMAP_TVCMCR, 8, 5, 0);
	ids_write(pdev->path, IMAP_TVHSCR, 30, 2, 0);
	
	osd_load_param(pdev);

	return 0;
}

static int tvif_set_bt656_pad_map(struct display_device *pdev,
			enum display_bt656_pad_map map)
{
	pdev->bt656_pad_map = map;

#ifdef CONFIG_ARCH_APOLLO
	if (map == DISPLAY_BT656_PAD_DATA8_TO_DATA15) {
		dlog_err("Apollo does NOT support BT656 pad map DATA8~15 mode\n");
		return -EINVAL;
	}
#else	
	ids_write(pdev->path, LCDCON5, LCDCON5_BT656_MODE, 1, map);
#endif

	if (map == DISPLAY_BT656_PAD_DATA0_TO_DATA7)
		imapx_pad_init("srgb");
	else
		imapx_pad_init("bt656");

	return 0;
}

static int tvif_set_bt601_yuv_format(struct display_device *pdev,
			int data_width, enum display_yuv_format format)
{
	int direct = 0, bit16 = 0;
	int data_order = 0;

	pdev->bt601_data_width = data_width;
	pdev->bt601_yuv_format = format;

	if (data_width == 24) {
		imapx_pad_init("rgb0");
		direct = 1;
		switch (format) {
			case DISPLAY_YUV_FORMAT_YCBCR:
				data_order = 0;
				break;
			case DISPLAY_YUV_FORMAT_YCRCB:
				data_order = 1;
				break;
			case DISPLAY_YUV_FORMAT_CBCRY:
				data_order = 2;
				break;
			case DISPLAY_YUV_FORMAT_CRCBY:
				data_order = 3;
				break;
			default:
				dlog_err("Invalid bt601 yuv format %d\n", format);
				return -EINVAL;
				break;
		}
	}
	else if (data_width == 16) {
		imapx_pad_init("srgb");
		imapx_pad_init("bt656");
		bit16 = 1;
		switch (format) {
			case DISPLAY_YUV_FORMAT_YCRYCB:
				data_order = 0;
				break;
			case DISPLAY_YUV_FORMAT_YCBYCR:
				data_order = 1;
				break;
			case DISPLAY_YUV_FORMAT_CRYCBY:
				data_order = 2;
				break;
			case DISPLAY_YUV_FORMAT_CBYCRY:
				data_order = 3;
				break;
			default:
				dlog_err("Invalid bt601 yuv format %d\n", format);
				return -EINVAL;
				break;
		}
	}
	else if (data_width == 8) {
		imapx_pad_init("srgb");
		bit16 = 0;
		switch (format) {
			case DISPLAY_YUV_FORMAT_CBYCRY:
				data_order = 0;
				break;
			case DISPLAY_YUV_FORMAT_CRYCBY:
				data_order = 1;
				break;
			case DISPLAY_YUV_FORMAT_YCBYCR:
				data_order = 2;
				break;
			case DISPLAY_YUV_FORMAT_YCRYCB:
				data_order = 3;
				break;
			default:
				dlog_err("Invalid bt601 yuv format %d\n", format);
				return -EINVAL;
				break;
		}
	}
	else {
		dlog_err("Invalid bt601 width %d\n", data_width);
		return -EINVAL;
	}

	ids_write(pdev->path, IMAP_TVICR, 28, 1, direct);
	ids_write(pdev->path, IMAP_TVICR, 29, 1, bit16);
	ids_write(pdev->path, IMAP_TVICR, 16, 2, data_order);

	return 0;
}

static int tvif_probe(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static int tvif_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver tvif_driver = {
	.probe  = tvif_probe,
	.remove = tvif_remove,
	.driver = {
		.name = "tvif",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_TVIF,
	.function = {
		.enable = tvif_enable,
		.set_vic = tvif_set_vic,
		.set_interface_type = tvif_set_interface_type,
		.set_bt656_pad_map = tvif_set_bt656_pad_map,
		.set_bt601_yuv_format = tvif_set_bt601_yuv_format,
	}
};

module_display_driver(tvif_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display tvif controller driver");
MODULE_LICENSE("GPL");
