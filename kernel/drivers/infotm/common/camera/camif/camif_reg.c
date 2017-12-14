/***************************************************************************** 
** 
** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** camif register operation  file
**
** Revision History: 
** ----------------- 
** v1.0.1	sam@2014/03/31: first version.
**
*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <mach/items.h>
#include <mach/pad.h>
#include "camif_core.h"
#include "camif_reg.h"
#include <linux/gpio.h>

static struct camif_config_t camif_init_config= {
	.in_width	 = VGA_WIDTH,
	.in_height	 = VGA_HEIGHT,
	.xoffset	 = 0,
	.yoffset	 = 0,
	.itu601		 = 1,
	.uvoffset	 = 0,
	.interlaced  = 0,
	.yuv_sequence = CAMIF_INPUT_YUV_ORDER_YCbYCr,
	.invpclk	 = 0,
	.invvsync    = 0,
	.invhref     = 0,
	.invfield    = 0,
	.c_enable    = 1,
	.c_width     = VGA_WIDTH,
	.c_height    = VGA_HEIGHT,
	.c_format    = CODEC_IMAGE_FORMAT_YUV420,
	.c_store_format = CODEC_STORE_FORMAT_PLANAR,
	.c_hwswap    = 0,
	.c_byteswap  = 0,
	.pre_enable  = 0,
	.pre_width   = VGA_WIDTH,
	.pre_height  = VGA_HEIGHT,
	.pre_store_format =	PREVIEW_SPLANAR_420,
	.pre_hwswap  = 0,
	.pre_byteswap = 0,
	.pre_bpp16fmt = PREVIEW_BPP16_RGB565,
	.pre_rgb24_endian = PREVIEW_BPP24BL_LITTLE_ENDIAN,
	.intt_type	 = CAMIF_INTR_SOURCE_DMA_DONE	

};

void imapx_camif_dump_regs(struct imapx_video *vp)
{
	struct imapx_camif *host = vp->camif_dev;
	pr_info("Reg[0 ,4, 8, 44 ]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CISRCFMT), camif_read(host,CAMIF_CIWDOFST),
			camif_read(host,CAMIF_CIGCTRL), camif_read(host,CAMIF_CICOTRGSIZE));
	pr_info("Reg[68, 58, B4, F8]    %8x     %8x     %8x     %8x\n",
	            camif_read(host,CAMIF_CPATHPIXELCNT), camif_read(host,CAMIF_CH2FIFOPUSHCNT),
	            camif_read(host,CAMIF_CH2FIFOPOPCNT),
	            camif_read(host,CAMIF_INPUT_SUMPIXS));
	pr_info("Reg[48, A0, A4, C4]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CICOTRGFMT), camif_read(host,CAMIF_CIIMGCPT),
			camif_read(host,CAMIF_CICPTSTATUS),
			camif_read(host,CAMIF_CICOFIFOSTATUS));
	pr_info("Reg[100, 104,108, 10c]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CH0DMAFB1), camif_read(host,CAMIF_CH0DMACC1),
			camif_read(host,CAMIF_CH0DMAFB2), camif_read(host,CAMIF_CH0DMACC2));
	pr_info("Reg[120,124,128,12c]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CH1DMAFB1), camif_read(host,CAMIF_CH1DMACC1),
			camif_read(host,CAMIF_CH1DMAFB2), camif_read(host,CAMIF_CH1DMACC2));
	pr_info("Reg[130,134,138,13c]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CH1DMAFB3), camif_read(host,CAMIF_CH1DMACC3),
			camif_read(host,CAMIF_CH1DMAFB4), camif_read(host,CAMIF_CH1DMACC4));
	
	pr_info("Reg[140,144,148,14c]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CH2DMAFB1), camif_read(host,CAMIF_CH2DMACC1),
			camif_read(host,CAMIF_CH2DMAFB2), camif_read(host,CAMIF_CH2DMACC2));
	pr_info("Reg[150,154,158,15c]	%8x		%8x		%8x		%8x\n",
			camif_read(host,CAMIF_CH2DMAFB3), camif_read(host,CAMIF_CH2DMACC3),
			camif_read(host,CAMIF_CH2DMAFB4), camif_read(host,CAMIF_CH2DMACC4));

	return;
}

void imapx_camif_set_bit(struct imapx_camif *host, uint32_t addr,
						int bits, int offset, uint32_t value)
{
	uint32_t tmp;
	tmp = camif_read(host, addr);
	tmp = (value << offset) | (~(((1 << bits)- 1) << offset)  & tmp);
	camif_write(host, addr, tmp);
}

uint32_t imapx_camif_get_bit(struct imapx_camif *host,uint32_t addr,
		int bits, int offset)
{
	uint32_t tmp;
	tmp = camif_read(host, addr);
	tmp = (tmp >> offset) & ((1 << bits)- 1) ;
	return  tmp;
}

void imapx_camif_mask_irq(struct imapx_video *vp, uint32_t mbit) 
{
	struct imapx_camif *host = vp->camif_dev;
	uint32_t tmp;

	tmp = camif_read(host, CAMIF_CIGCTRL);
	tmp |= mbit;
	camif_write(host, CAMIF_CIGCTRL, tmp);
}

void imapx_camif_unmask_irq(struct imapx_video *vp, uint32_t umbit) 
{
	struct imapx_camif *host = vp->camif_dev;
	uint32_t tmp;

	tmp = camif_read(host, CAMIF_CIGCTRL);
	tmp &= ~umbit;
	camif_write(host, CAMIF_CIGCTRL, tmp);
}


int  imapx_camif_set_fmt(struct imapx_video *vp)
{
	struct imapx_camif *host = vp->camif_dev;
	/*
	 *config camif input source :use codec path no scaler
	 */

	//camif output config 
	switch (vp->outfmt.pixelformat){
		case V4L2_PIX_FMT_NV12:
        /* YCBCR420 */
        //set codec target format
        {
             //set yuv420
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_ycc422,
                    CODEC_IMAGE_FORMAT_YUV420);
            //set store format
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_StoredFormat,
                    CODEC_STORE_FORMAT_SPLANAR);
            break;
        }
		case V4L2_PIX_FMT_NV21:
        /* YCRCB420*/
        //set codec target format
        {
             //set yuv420
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_ycc422,
                    CODEC_IMAGE_FORMAT_YUV420);
            //set store format
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_StoredFormat,
                    CODEC_STORE_FORMAT_SPLANAR);
            //set BTSWAP
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_ByteSwap,1);
            break;
        }
		case V4L2_PIX_FMT_YUYV:
		{
		    //csi-->camif : raw data
		    if (!strcmp(vp->sen_info->interface, "camif-csi") && (vp->sen_info->mode >= 0x2a) ) {
		        vp->outfmt.width /=2;
		    }
            //set yuv422
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_ycc422,
                    CODEC_IMAGE_FORMAT_YUV422);
            //set store format
            imapx_camif_set_bit(host, CAMIF_CICOTRGFMT,
                    1, CICOTRGFMT_StoredFormat,
                    CODEC_STORE_INTERLEAVED);
            break;
		}
		default:
			break;
	}
	pr_db("camif driver :set code size: x = %d, y= %d\n",vp->outfmt.width,
			vp->outfmt.height);
	//set output size_t
	camif_write(host, CAMIF_CICOTRGSIZE,
			((vp->outfmt).width << CICOTRGSIZE_TargetHsize) |
			((vp->outfmt).height << CICOTRGSIZE_TargetVsize));
	return 0;

}

void imapx_camif_cfg_addr(struct imapx_video *vp, 
						struct camif_buffer *buf,
						int index)
{
	struct imapx_camif *host = vp->camif_dev;
	u32 len = 0;
	
	if(buf->paddr.cr_len && vp->dmause.ch0){
		camif_write(host, CAMIF_CH0DMA_FB(index), buf->paddr.cr);
		imapx_camif_set_bit(host, CAMIF_CH0DMA_CC(index),
						  25, 0, (( (buf->paddr).cr_len) >> 3));
	}

	if(buf->paddr.cb_len && vp->dmause.ch1){
		camif_write(host, CAMIF_CH1DMA_FB(index),( buf->paddr.cb));
		len = (buf->paddr.cb_len) >> 3;
		imapx_camif_set_bit(host, CAMIF_CH1DMA_CC(index),
						  25, 0, len);//(( (buf->paddr).cb_len) >> 3));

	}

	if(buf->paddr.y_len && vp->dmause.ch2){
		camif_write(host, CAMIF_CH2DMA_FB(index),(buf->paddr.y));
		len = (buf->paddr.y_len) >> 3;
		imapx_camif_set_bit(host, CAMIF_CH2DMA_CC(index), 
						  25, 0, len);

	}

}


void imapx_camif_enable_capture(struct imapx_video *vp)
{

	struct imapx_camif *host = vp->camif_dev;
	int idx;

	for(idx = 0; idx < 4; idx++){
		if(vp->dmause.ch0){   //set dma buffer autoload, next frame enable
			imapx_camif_set_bit(host, CAMIF_CH0DMA_CC(idx), 3,
					CICHxALTCTRL_Dir, 0x07);
		}

		if(vp->dmause.ch1){
			imapx_camif_set_bit(host, CAMIF_CH1DMA_CC(idx), 3,
					CICHxALTCTRL_Dir, 0x07);
		}

		if(vp->dmause.ch2){
			imapx_camif_set_bit(host, CAMIF_CH2DMA_CC(idx), 3,
					CICHxALTCTRL_Dir, 0x07);

		}
	}

	//enable dma channel
	if(vp->dmause.ch0){
		imapx_camif_set_bit(host, CAMIF_CH0DMA_CC(0), 1,
				CICHxCTRL_DMAEn, 0x01);
	}

	if(vp->dmause.ch1){	
		imapx_camif_set_bit(host, CAMIF_CH1DMA_CC(0), 1,
				CICHxCTRL_DMAEn, 0x01);
	}
	
	if(vp->dmause.ch2){	
		imapx_camif_set_bit(host, CAMIF_CH2DMA_CC(0), 1,
				CICHxCTRL_DMAEn, 0x01);
	}
	

	//enable camif and code path 
	imapx_camif_set_bit(host, CAMIF_CIIMGCPT, 3, 
				CIIMGCPT_ImgCptEn_PrSc, 0x06);	

}

void imapx_camif_disable_capture(struct imapx_video *vp)
{
	//disable dma channel
	struct imapx_camif *host = vp->camif_dev;

	if(vp->dmause.ch0){
		imapx_camif_set_bit(host, CAMIF_CH0DMA_CC(0), 2,
				CICHxCTRL_RST, 0x01);
	}

	if(vp->dmause.ch1){	
		imapx_camif_set_bit(host, CAMIF_CH1DMA_CC(0), 2,
				CICHxCTRL_RST, 0x01);
	}
	
	if(vp->dmause.ch2){	
		imapx_camif_set_bit(host, CAMIF_CH2DMA_CC(0), 2,
				CICHxCTRL_RST, 0x01);
	}
	
	//disable camif and code path 
	imapx_camif_set_bit(host, CAMIF_CIIMGCPT, 3, 
				CIIMGCPT_ImgCptEn_PrSc, 0x00);	
}


void imapx_camif_host_deinit(struct imapx_video *vp)
{
	struct imapx_camif *host = vp->camif_dev;
	imapx_camif_set_bit(host, CAMIF_CIIMGCPT, 1, CIIMGCPT_CAMIF_EN, 0);
	imapx_camif_set_bit(host, CAMIF_CIIMGCPT, 1, CIIMGCPT_ImgCptEn_CoSc, 0);
	imapx_camif_set_bit(host, CAMIF_CIIMGCPT, 1, CIIMGCPT_ImgCptEn_PrSc, 0);
	return;

}

void imapx_camif_host_init(struct imapx_video *vp)
{
	struct imapx_camif *host = vp->camif_dev;

	/* camif soft reset*/
	imapx_camif_set_bit(host, CAMIF_CIGCTRL, 1, CIGCTRL_SwRst, 1); 
	mdelay(5);
	imapx_camif_set_bit(host, CAMIF_CIGCTRL, 1, CIGCTRL_SwRst, 0);
	// set camif  x,y offset ,input w,h
	camif_write(host, CAMIF_CIWDOFST, 
				(camif_init_config.xoffset << CIWDOFST_WinHorOfst) |
				(camif_init_config.yoffset << CIWDOFST_WinVerOfst));
	//set camif input format 
	camif_write(host, CAMIF_CISRCFMT,
				(camif_init_config.itu601 << CISRCFMT_ITU601or656) |
				(camif_init_config.uvoffset << CISRCFMT_UVOffset) |
				(camif_init_config.interlaced << CISRCFMT_ScanMode) |
				(camif_init_config.yuv_sequence << CISRCFMT_Order422));
	
	//set camif interrupt and signal polarity
	camif_write(host, CAMIF_CIGCTRL, (1 << CIGCTRL_IRQ_OvFiCH4_en) |
			(1 << CIGCTRL_IRQ_OvFiCH3_en) | (1 << CIGCTRL_DEBUG_EN) |
			(1 << CIGCTRL_IRQ_OvFiCH2_en) | (1 << CIGCTRL_IRQ_en) |
			(1 << CIGCTRL_IRQ_OvFiCH1_en) | (1 << CIGCTRL_IRQ_Bad_SYN_en) | 
			(1 << CIGCTRL_IRQ_OvFiCH0_en) | 
			(camif_init_config.intt_type << CIGCTRL_IRQ_Int_Mask_Pr) |
			(camif_init_config.intt_type << CIGCTRL_IRQ_Int_Mask_Co) |
			(camif_init_config.invpclk   << CIGCTRL_InvPolCAMPCLK)   |
			((camif_init_config.invvsync | (vp->sen_info->imager & 0x01)) << CIGCTRL_InvPolCAMVSYNC)  |
			((camif_init_config.invhref | ((vp->sen_info->imager & 0x02) >> 1))  << CIGCTRL_InvPolCAMHREF)   |
			(camif_init_config.invfield  << CIGCTRL_InvPolCAMFIELD)); 
	
	// set codec output 
	
	camif_write(host, CAMIF_CICOTRGSIZE,
			(camif_init_config.c_width << CICOTRGSIZE_TargetHsize) |
			(camif_init_config.c_height << CICOTRGSIZE_TargetVsize));
	
	return;
}





















