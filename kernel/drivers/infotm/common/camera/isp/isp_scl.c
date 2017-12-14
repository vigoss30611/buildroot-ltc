
/*
 *  imapx isp scale function
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include "isp_core.h"
#include "isp_reg.h"

int imapx_isp_scale_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable) {
		imapx_isp_set_bit(host, ISP_SCL_SCR, 1, 30, 0);
		imapx_isp_set_bit(host, ISP_SCL_SCR, 1, 14, 0);
	} else {
		imapx_isp_set_bit(host, ISP_SCL_SCR, 1, 30, 1);
		imapx_isp_set_bit(host, ISP_SCL_SCR, 1, 14, 1);
	}
	return 0;
}


int imapx_isp_scale_cfg(struct imapx_vp *vp, struct scale_info *scl)
{
	struct imapx_isp *host = vp->isp_dev;

	if (scl == NULL || vp == NULL) {
		pr_err("[ISP]--%s error eixt!\n", __func__);
		return -1;
	}
	/* cfg input & output size */
	isp_write(host, ISP_SCL_IFSR, (scl->ihres - 1) | ((scl->ivres - 1) << 16));
	isp_write(host, ISP_SCL_OFSR, (scl->ohres - 1) | ((scl->ovres - 1) << 16));

	scl->vscaling = (scl->ivres * 1024) / scl->ovres;
	scl->hscaling = (scl->ihres * 1024) / scl->ohres;

	isp_write(host, ISP_SCL_SCR, ((scl->vscaling << 16) | scl->hscaling));

	return 0;
}

