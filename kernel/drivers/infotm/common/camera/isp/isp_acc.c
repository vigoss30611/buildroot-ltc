#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include "isp_core.h"
#include "isp_reg.h"


int imapx_isp_acc_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 11, 0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 11, 1);

	return 0;
}

int imapx_isp_acc_cfg(struct imapx_vp *vp, struct acc_info *acc)
{
	struct imapx_isp *host = vp->isp_dev;

	pr_info("[ISP-ACC] %s\n", __func__);
	if (!acc->enable) {
		pr_info("[ISP-ACC] acc need to close\n");
		return 0;
	}

	imapx_isp_set_bit(host, ISP_ACC_CNTL, 1, 1, 0);

	isp_write(host, ISP_ACC_ABCD, acc->comat.coefa | (acc->comat.coefb << 8)
					| (acc->comat.coefc << 11) | (acc->comat.coefd << 24));

	isp_write(host, ISP_ACC_HISA, acc->romat.roalo | (acc->romat.roahi << 16)
					| (acc->histframe << 27));
	isp_write(host, ISP_ACC_HISB, acc->romat.roblo | (acc->romat.robhi << 16));
	isp_write(host, ISP_ACC_HISC, acc->romat.roclo | (acc->romat.rochi << 16));

	isp_write(host, ISP_ACC_CNTL, acc->rdmode | (acc->lutapbs << 2)
				| (acc->histround << 3) | (acc->coefe << 16));

	imapx_isp_set_bit(host, ISP_ACC_CNTL, 1, 1, 1);
	memcpy(&vp->isp_cfg.acc, acc, sizeof(struct acc_info));
	return 0;
}

