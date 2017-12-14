
 /*
  *  imapx isp scale function
  */
 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/delay.h>
 #include "isp_core.h"
 #include "isp_reg.h"

int imapx_isp_ief_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;

	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 14, 0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 14, 1);

	return 0;
}

int imapx_isp_iefcsc_enable(struct imapx_vp *vp, int enable)
{
	struct imapx_isp *host = vp->isp_dev;
	if (enable)
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 13, 0);
	else
		imapx_isp_set_bit(host, ISP_GLB_CFG2, 1, 13, 1);
	return 0;
}

int imapx_isp_ief_cfg(struct imapx_vp *vp, struct ief_info *ief)
{
	struct imapx_isp *host = vp->isp_dev;
	pr_info("[ISP-IEF] %s\n", __func__);

	if (ief->enable) {
		/*  set IE TYPE */
		imapx_isp_set_bit(host, ISP_IE_CNTL, 4, 0, ief->type);
		isp_write(host, ISP_IE_RGCF1,
				(ief->coeffcr << 23) | (ief->coeffcb << 14));
		/*  set IE SMD*/
		isp_write(host, ISP_IE_SMD,
				ief->selmat.mode | (ief->selmat.tha << 8) | (ief->selmat.thb << 16));
	}
	/* set IECSC matrix */
	if (ief->cscen) {
		isp_write(host, ISP_IE_COEF11, ief->cscmat.coef11);
		isp_write(host, ISP_IE_COEF12, ief->cscmat.coef12);
		isp_write(host, ISP_IE_COEF13, ief->cscmat.coef13);
		isp_write(host, ISP_IE_COEF21, ief->cscmat.coef21);
		isp_write(host, ISP_IE_COEF22, ief->cscmat.coef22);
		isp_write(host, ISP_IE_COEF23, ief->cscmat.coef23);
		isp_write(host, ISP_IE_COEF31, ief->cscmat.coef31);
		isp_write(host, ISP_IE_COEF32, ief->cscmat.coef32);
		isp_write(host, ISP_IE_COEF33, ief->cscmat.coef33);
		isp_write(host, ISP_IE_OFFSET,
				ief->cscmat.oft_b | (ief->cscmat.oft_a << 16));
	}

	memcpy(&vp->isp_cfg.ief, ief, sizeof(struct ief_info));
	return 0;
}


