
/*
 *  imapx isp crop function
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include "isp_core.h"
#include "isp_reg.h"


int imapx_isp_crop_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 20, 0x0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 20, 0x1);
	return 0;
}

int  imapx_isp_crop_cfg(struct imapx_vp *vp, struct crop_info *crop)
{
	struct imapx_isp *host = vp->isp_dev;

	crop->width = (vp->infmt.width * (30 - crop->ratio) / 30) & (~0x1);
	crop->height = (vp->infmt.height * (30 - crop->ratio) / 30) & (~0x1);

	crop->xstart = ((vp->infmt.width - crop->width) >> 1) & (~0x01);
	crop->ystart = ((vp->infmt.height - crop->height) >> 1) & (~0x01);

	crop->xend = crop->xstart + crop->width;
	crop->yend = crop->ystart + crop->height;

	isp_write(host, ISP_CROP_START, (crop->xstart) | (crop->ystart << 16));
	isp_write(host, ISP_CROP_END, (crop->xend - 1) | ((crop->yend - 1) << 16));

	return 0;

}


