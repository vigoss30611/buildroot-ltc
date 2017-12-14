#include <linux/module.h>
#include <linux/delay.h>
#include "isp_core.h"
#include "isp_reg.h"


int imapx_isp_acm_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 12, 0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 12, 1);

	return 0;
}


int imapx_isp_acm_cfg(struct imapx_vp *vp, struct acm_info *acm)
{
	struct imapx_isp *host = vp->isp_dev;

	pr_info("[ISP-ACM] %s\n", __func__);
	if (!acm->enable) {
		pr_info("[ISP-ACM] acm need to close\n");
		return 0;
	}

	isp_write(host, ISP_ACM_CNTLA, acm->rdmode | (acm->ths << 8));
	isp_write(host, ISP_ACM_CNTLB, acm->thrmat.tha | (acm->thrmat.thb << 8)
					| (acm->thrmat.thc << 16));
	isp_write(host, ISP_ACM_COEFPR,
			acm->comat.coefp | (acm->comat.coefr << 16));
	isp_write(host, ISP_ACM_COEFGB,
			acm->comat.coefg | (acm->comat.coefb << 16));
	isp_write(host, ISP_ACM_COEF0, acm->comat.m0r);
	isp_write(host, ISP_ACM_COEF1, acm->comat.m1r | (acm->comat.m2r << 16));
	isp_write(host, ISP_ACM_COEF2, acm->comat.m3r | (acm->comat.m4r << 16));
	isp_write(host, ISP_ACM_COEF3, acm->comat.m0g | (acm->comat.m1g << 16));
	isp_write(host, ISP_ACM_COEF4, acm->comat.m2g | (acm->comat.m3g << 16));
	isp_write(host, ISP_ACM_COEF5, acm->comat.m4g | (acm->comat.m0b << 16));
	isp_write(host, ISP_ACM_COEF6, acm->comat.m1b | (acm->comat.m2b << 16));
	isp_write(host, ISP_ACM_COEF7, acm->comat.m3b | (acm->comat.m4b << 16));

	memcpy(&vp->isp_cfg.acm, acm, sizeof(struct acm_info));

	return 0;
}
