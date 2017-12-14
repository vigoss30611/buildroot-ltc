
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/items.h>
#include <mach/pad.h>
#include "isp_core.h"
#include "isp_reg.h"

/*
 *
 */
void imapx_isp_dump_regs(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;
	pr_err("0x28200000	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_GLB_CFG0), isp_read(host, ISP_GLB_CFG1),
			isp_read(host, ISP_GLB_CFG2), isp_read(host, ISP_GLB_CFG3));
	pr_err("0x28200010	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_GLB_RV), isp_read(host, ISP_GLB_RH),
			isp_read(host, ISP_GLB_INTS), isp_read(host, ISP_GLB_INTM));
	pr_err("0x2820c000	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_CH0DMA_FBA1), isp_read(host, ISP_CH0DMA_CC1),
			isp_read(host, ISP_CH0DMA_FBA2), isp_read(host, ISP_CH0DMA_CC2));
	pr_err("0x2820c100	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_CH1DMA_FBA1), isp_read(host, ISP_CH1DMA_CC1),
			isp_read(host, ISP_CH1DMA_FBA2), isp_read(host, ISP_CH1DMA_CC2));
	pr_err("0x2820c200	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_CH2DMA_FBA1), isp_read(host, ISP_CH2DMA_CC1),
			isp_read(host, ISP_CH2DMA_FBA2), isp_read(host, ISP_CH2DMA_CC2));
	pr_err("0x2820a000	%8x		%8x		%8x		%8x\n",
			isp_read(host, ISP_SCL_IFSR), isp_read(host, ISP_SCL_OFSR),
			isp_read(host, ISP_SCL_OFSTR), isp_read(host, ISP_SCL_SCR));

	return;
}

static void imapx_isp_set_sensor_voltage(struct imapx_vp *vp)
{
	int iovdd_pad, dvdd_pad, ret;

	iovdd_pad = (vp->sen_info)->iovdd_pad;
	dvdd_pad = (vp->sen_info)->dvdd_pad;

	/*step 1 power up sensor voltage */
	if(vp->iovdd_rg) {
		regulator_set_voltage(vp->iovdd_rg, 1800000, 1800000);
	    ret = regulator_enable(vp->iovdd_rg);
	}
	else {
		if(-1 != iovdd_pad)
		{
			if(0 > iovdd_pad)
			{
				gpio_request(-iovdd_pad, "iovdd_pad");
				gpio_direction_output(-iovdd_pad, 0);
			}
			else
			{
				gpio_request(iovdd_pad, "iovdd_pad");
				gpio_direction_output(iovdd_pad, 1);
			}
		}
	}

	if(vp->dvdd_rg) {
		regulator_set_voltage(vp->dvdd_rg, 2800000, 2800000);
		ret = regulator_enable(vp->dvdd_rg);
	}
	else {
		if(-1 != dvdd_pad)
		{
			if(0 > dvdd_pad)
			{
				gpio_request(-dvdd_pad, "dvdd_pad");
				gpio_direction_output(dvdd_pad, 0);
			}
			else
			{
				gpio_request(dvdd_pad, "dvdd_pad");
				gpio_direction_output(dvdd_pad, 1);
			}
		}
		
	}
	
}

static void imapx_isp_disable_voltage(struct imapx_vp *vp)
{
	int pdown; 
	int iovdd_pad, dvdd_pad;

	pdown = (vp->sen_info)->pdown_pin;
	iovdd_pad = (vp->sen_info)->iovdd_pad;
	dvdd_pad = (vp->sen_info)->dvdd_pad;

	//power down sensor
	if(0 < pdown) {
		gpio_direction_output(pdown, 1);
		gpio_free(pdown);
	}
	else {
		gpio_direction_output(-pdown, 0);
		gpio_free(-pdown);
	}

	if(vp->dvdd_rg){
		regulator_disable(vp->dvdd_rg);
	}
	else {
		if(-1 != dvdd_pad)
		{
			if(0 > dvdd_pad)
			{
				gpio_direction_output(-dvdd_pad, 1);
				gpio_free(-dvdd_pad);
			}
			else
			{
				gpio_direction_output(dvdd_pad, 0);
				gpio_free(dvdd_pad);
			}
		}
	}
		
	if(vp->iovdd_rg){
		regulator_disable(vp->iovdd_rg);
	}
	else {
		if(-1 != iovdd_pad)
		{
			if(0 > iovdd_pad)
			{
				gpio_direction_output(-iovdd_pad, 1);
				gpio_free(-iovdd_pad);
			}
			else
			{
				gpio_direction_output(iovdd_pad, 0);
				gpio_free(iovdd_pad);
			}
		}

	}
	
}

int imapx_isp_sen_power(struct imapx_vp *vp, uint32_t on)
{
	int ret = 0;
	int rst = (vp->sen_info)->reset_pin;
	int pdown = (vp->sen_info)->pdown_pin;

	if(on) {
		
		/*step 1 power up sensor voltage */
		imapx_isp_set_sensor_voltage(vp);

		/*reset */
		//set gpio rest_pin work mode  
		if(0 < rst) {
			gpio_request(rst, "rst_pin");
			gpio_direction_output(rst, 1);

			mdelay(5);
			//imapx_pad_set_outdat(0, 1, rst);
			gpio_set_value(rst, 0);
			mdelay(5);
			gpio_set_value(rst, 1);
		}
		else {
			rst = -rst;
			gpio_request(rst, "rst_pin");
			gpio_direction_output(rst, 0);
			mdelay(5);
			//imapx_pad_set_outdat(0, 1, rst);
			gpio_set_value(rst, 1);
			mdelay(5);
			gpio_set_value(rst, 0);

		}
		/*PW DOWN*/
		mdelay(5);
		if(0 < pdown) {
			gpio_request(pdown, "pdown_pin");
			gpio_direction_output(pdown, 0);
		}
		else {
			gpio_request(-pdown, "pdown_pin");
			gpio_direction_output(-pdown, 1);
		}
		
	}
	else {
		imapx_isp_disable_voltage(vp);
		if(0 < pdown) {
			gpio_free(pdown);
		}
		else {
			gpio_free(-pdown);
		}
		
		if(0 < rst) {
			gpio_free(rst);
		}
		else {
			gpio_free(-rst);
		}

	}

	return ret;
}

void imapx_isp_set_bit(struct imapx_isp *host, uint32_t addr,
						int bits, int offset, uint32_t value)
{
	uint32_t tmp;
	tmp = isp_read(host, addr);
	tmp = (value << offset) | (~(((1 << bits) - 1) << offset)  & tmp);
	isp_write(host, addr, tmp);
}

uint32_t imapx_isp_get_bit(struct imapx_isp *host, uint32_t addr,
		int bits, int offset)
{
	uint32_t tmp;
	tmp = isp_read(host, addr);
	tmp = (tmp >> offset) & ((1 << bits) - 1);
	return  tmp;
}

void imapx_isp_mask_irq(struct imapx_vp *vp, uint32_t mbit)
{
	struct imapx_isp *host = vp->isp_dev;
	uint32_t tmp;

	tmp = isp_read(host, ISP_GLB_INTM);
	tmp |= mbit;
	isp_write(host, ISP_GLB_INTM, tmp);
}

void imapx_isp_unmask_irq(struct imapx_vp *vp, uint32_t umbit)
{
	struct imapx_isp *host = vp->isp_dev;
	uint32_t tmp;

	tmp = isp_read(host, ISP_GLB_INTM);
	tmp &= ~umbit;
	isp_write(host, ISP_GLB_INTM, tmp);
}


int  imapx_isp_set_fmt(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;
	/*
	 *cfg isp input source
	 */
	isp_write(host, ISP_GLB_CFG0, (vp->infmt.width) |
					((vp->infmt.height) << 16));
	isp_write(host, ISP_GLB_RV, 1024 * 1024 / vp->infmt.width);
	isp_write(host, ISP_GLB_RH, 1024 * 1024 / vp->infmt.height);

	switch (vp->infmt.pixelformat) {
		case V4L2_PIX_FMT_NV12:
			/* YCBYCR */
			isp_write(host, ISP_GLB_CFG3, 0x2f4);
			break;
		case V4L2_PIX_FMT_NV21:
			/* YCRCB420*/
			isp_write(host, ISP_GLB_CFG3, 0x2f4);
			break;
		default:
			break;
	}
	switch (vp->outfmt.pixelformat) {
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV21:
			isp_write(host, ISP_SCL_OFSTR, 0xc);
			break;
	default:
		break;
	}
	return 0;
}

void imapx_isp_cfg_addr(struct imapx_vp *vp, struct isp_buffer *buf, int index)
{
	struct imapx_isp *host = vp->isp_dev;
	imapx_isp_set_bit(host, ISP_REG_UPDATE, 3, 0, 1);
	if (buf->paddr.y_len) {
		isp_write(host, ISP_CH0DMA_FB(index), buf->paddr.y);
		imapx_isp_set_bit(host, ISP_CH0DMA_CC(index),
					25, 0, buf->paddr.y_len >> 3);
	}
	if (buf->paddr.cb_len) {
		isp_write(host, ISP_CH1DMA_FB(index), buf->paddr.cb);
		imapx_isp_set_bit(host, ISP_CH1DMA_CC(index),
					25, 0, buf->paddr.cb_len >> 3);
	}
	if (buf->paddr.cr_len) {
		isp_write(host, ISP_CH2DMA_FB(index), buf->paddr.cr);
		imapx_isp_set_bit(host, ISP_CH2DMA_CC(index),
					25, 0, buf->paddr.cr_len >> 3);
	}
}


void imapx_isp_enable_capture(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;
	int index;
	/* step 1:cfg DMA config*/
	for (index = 0; index < 4; index++) {
		if (vp->dmause.ch0)
			imapx_isp_set_bit(host, ISP_CH0DMA_CC(index), 3, 25, 0x7);

		if (vp->dmause.ch1)
			imapx_isp_set_bit(host, ISP_CH1DMA_CC(index), 3, 25, 0x7);

		if (vp->dmause.ch2)
			imapx_isp_set_bit(host, ISP_CH2DMA_CC(index), 3, 25, 0x7);
	}
	if (vp->dmause.ch0)
		imapx_isp_set_bit(host, ISP_CH0DMA_CC(0), 1, 31, 0x1);
	if (vp->dmause.ch1)
		imapx_isp_set_bit(host, ISP_CH1DMA_CC(0), 1, 31, 0x1);
	if (vp->dmause.ch2)
		imapx_isp_set_bit(host, ISP_CH2DMA_CC(0), 1, 31, 0x1);
	/* step 2: start isp */
	/*imapx_isp_dump_regs(vp);*/
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 0, 1);

}

void imapx_isp_disable_capture(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;

	if (vp->dmause.ch0)
		imapx_isp_set_bit(host, ISP_CH0DMA_CC(0), 1, 31, 0x0);
	if (vp->dmause.ch1)
		imapx_isp_set_bit(host, ISP_CH1DMA_CC(0), 1, 31, 0x0);
	if (vp->dmause.ch2)
		imapx_isp_set_bit(host, ISP_CH2DMA_CC(0), 1, 31, 0x0);

	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 0, 0);

	return;
}


void imapx_isp_host_deinit(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 0, 0);
	return;
}
void imapx_isp_host_init(struct imapx_vp *vp)
{
	struct imapx_isp *host = vp->isp_dev;

	/* host cotroller soft reset*/
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 1, 1);
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 1, 0);
	/* enable DMA VSYNC */
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 1, 9, 1);
	/* enable DMA CHANNELS */
	imapx_isp_set_bit(host, ISP_GLB_CFG1, 3, 5, 0x3);
	/* Disable all  ISP function now */
	imapx_isp_set_bit(host, ISP_GLB_CFG2, 21, 0, 0x17dFFF);
	/* enable irq */
	imapx_isp_unmask_irq(vp, INT_MASK_ALL);
	return;
}





















