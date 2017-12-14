
/*
 * imapx isp ee function
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include "isp_core.h"
#include "isp_reg.h"

int imapx_isp_ee_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 9, 0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 9, 1);

	return 0;

}

int imapx_isp_ee_cfg(struct imapx_vp *vp, struct ee_info *ee)
{
	struct imapx_isp *host = vp->isp_dev;
	pr_info("[ISP-EE] %s\n", __func__);
	/* gasmode */
	if (ee->gasmode == ISP_EE_GAUSS_MODE_1)
		isp_write(host, ISP_EE_MAT4,
				(4 << 3) | (4 << 9) | (16 << 12) | (4 << 16) | (4 << 22));
	else if (ee->gasmode == ISP_EE_GAUSS_MODE_0)
		isp_write(host, ISP_EE_MAT4,
				2 | (4 << 3) | (2 << 6) | (4 << 9) |
				(8 << 12) | (4 << 16) | (2 << 19) | (4 << 22) | (2 << 25));


	if (!ee->enable) {
		pr_info("[ISP-EE] ee need to close\n");
		return 0;
	}

	isp_write(host, ISP_EE_CNTL,
			(ee->coefw << 16) | (ee->coefa << 8) |
			(ee->gasen << 1) | ee->rdmode);
	isp_write(host, ISP_EE_THE, ee->errth);
	isp_write(host, ISP_EE_TH, ee->thrmat.hth | (ee->thrmat.vth << 8) |
			(ee->thrmat.d0th << 16) | (ee->thrmat.d1th << 24));

	memcpy(&vp->isp_cfg.ee, ee, sizeof(struct ee_info));
	return 0;
}
