/***************************************************************************** 
** 
** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Soc camif of infotm x15 register defination header file     
**
** Revision History: 
** ----------------- 
** v1.0.1	sam@2014/03/31: first version.
**
*****************************************************************************/

#ifndef _CAMIF_REG_H
#define _CAMIF_REG_H

#define		CAMIF_CISRCFMT			(0x0000)
#define		CAMIF_CIWDOFST			(0x0004)
#define		CAMIF_CIGCTRL			(0x0008)
#define		CAMIF_CICOTRGSIZE		(0x0044)
#define		CAMIF_CICOTRGFMT		(0x0048)
#define		CAMIF_CIPRTRGSIZE		(0x0078)
#define		CAMIF_CIPRTRGFMT		(0x007C)
#define		CAMIF_CIPRSCPRERATIO	(0x0084)
#define		CAMIF_CIIMGCPT			(0x00A0)
#define		CAMIF_CICPTSTATUS		(0x00A4)
#define     CAMIF_CIPRFIFOSTATUS	(0x00C0)
#define     CAMIF_CICOFIFOSTATUS	(0x00C4)
#define		CAMIF_CICOEF11			(0x00D0)
#define		CAMIF_CICOEF12			(0x00D4)
#define		CAMIF_CICOEF13			(0x00D8)
#define		CAMIF_CICOEF21			(0x00DC)
#define		CAMIF_CICOEF22			(0x00E0)
#define		CAMIF_CICOEF23			(0x00E4)
#define		CAMIF_CICOEF31			(0x00E8)
#define		CAMIF_CICOEF32			(0x00EC)
#define		CAMIF_CICOEF33			(0x00F0)
#define		CAMIF_CICOMC			(0x00F4)
#define    CAMIF_INPUT_SUMPIXS     (0x00F8)
#define    CAMIF_ACTIVE_SIZE       (0x00FC)
#define		CAMIF_CH4FIFOPOPCNT		(0x00AC)
#define		CAMIF_CH3FIFOPOPCNT		(0x00B0)
#define		CAMIF_CH2FIFOPOPCNT		(0x00B4)
#define		CAMIF_CH1FIFOPOPCNT		(0x00B8)
#define		CAMIF_CH0FIFOPOPCNT		(0x00BC)
#define     CAMIF_CH4FIFOPUSHCNT    (0x0050)
#define     CAMIF_CH3FIFOPUSHCNT    (0x0054)
#define     CAMIF_CH2FIFOPUSHCNT    (0x0058)
#define     CAMIF_CH1FIFOPUSHCNT    (0x005C)
#define     CAMIF_CH0FIFOPUSHCNT    (0x0060)
#define	    CAMIF_PPATHPIXELCNT		(0x0064)
#define     CAMIF_CPATHPIXELCNT     (0x0068)
#define     CAMIF_CH0DMAFB1			(0x0100)
#define     CAMIF_CH0DMACC1			(0x0104)
#define     CAMIF_CH0DMAFB2			(0x0108)
#define     CAMIF_CH0DMACC2			(0x010C)
#define     CAMIF_CH0DMAFB3			(0x0110)
#define     CAMIF_CH0DMACC3			(0x0114)
#define     CAMIF_CH0DMAFB4			(0x0118)
#define     CAMIF_CH0DMACC4			(0x011C)
#define     CAMIF_CH1DMAFB1			(0x0120)
#define     CAMIF_CH1DMACC1			(0x0124)
#define     CAMIF_CH1DMAFB2			(0x0128)
#define     CAMIF_CH1DMACC2			(0x012C)
#define     CAMIF_CH1DMAFB3			(0x0130)
#define     CAMIF_CH1DMACC3			(0x0134)
#define     CAMIF_CH1DMAFB4			(0x0138)
#define     CAMIF_CH1DMACC4			(0x013C)
#define     CAMIF_CH2DMAFB1			(0x0140)
#define     CAMIF_CH2DMACC1			(0x0144)
#define     CAMIF_CH2DMAFB2			(0x0148)
#define     CAMIF_CH2DMACC2			(0x014C)
#define     CAMIF_CH2DMAFB3			(0x0150)
#define     CAMIF_CH2DMACC3			(0x0154)
#define     CAMIF_CH2DMAFB4			(0x0158)
#define     CAMIF_CH2DMACC4			(0x015C)
#define     CAMIF_CH3DMAFB1			(0x0160)
#define     CAMIF_CH3DMACC1			(0x0164)
#define     CAMIF_CH3DMAFB2			(0x0168)
#define     CAMIF_CH3DMACC2			(0x016C)
#define     CAMIF_CH3DMAFB3			(0x0170)
#define     CAMIF_CH3DMACC3			(0x0174)
#define     CAMIF_CH3DMAFB4			(0x0178)
#define     CAMIF_CH3DMACC4			(0x017C)
#define     CAMIF_CH4DMAFB1			(0x0180)
#define     CAMIF_CH4DMACC1			(0x0184)
#define     CAMIF_CH4DMAFB2			(0x0188)
#define     CAMIF_CH4DMACC2			(0x018C)
#define     CAMIF_CH4DMAFB3			(0x0190)
#define     CAMIF_CH4DMACC3			(0x0194)
#define     CAMIF_CH4DMAFB4			(0x0198)
#define     CAMIF_CH4DMACC4			(0x019C)

//#define     CAMIF_CH1DMAFB1

#define CAMIF_CH0DMA_FB(x)			(CAMIF_CH0DMAFB1 + 0x8 * x)                   
#define CAMIF_CH1DMA_FB(x)			(CAMIF_CH1DMAFB1 + 0x8 * x)                   
#define CAMIF_CH2DMA_FB(x)			(CAMIF_CH2DMAFB1 + 0x8 * x)                   

#define CAMIF_CH0DMA_CC(x)			(CAMIF_CH0DMACC1 + 0x8 * x)                   
#define CAMIF_CH1DMA_CC(x)			(CAMIF_CH1DMACC1 + 0x8 * x)                   
#define CAMIF_CH2DMA_CC(x)			(CAMIF_CH2DMACC1 + 0x8 * x)

//############################bit offset#######################################

//CISRCFMT
#define	CISRCFMT_ITU601or656		(31)
#define	CISRCFMT_UVOffset			(30)
#define	CISRCFMT_ScanMode			(29)
#define	CISRCFMT_Order422			(14)
	
//CIWDOFST
#define	CIWDOFST_WinHorOfst			(16)
#define	CIWDOFST_WinVerOfst			(0)
// CIGCTRL
#define	CIGCTRL_IRQ_OvFiCH4_en		(31)
#define	CIGCTRL_IRQ_OvFiCH3_en		(30)
#define	CIGCTRL_IRQ_OvFiCH2_en		(29)
#define	CIGCTRL_IRQ_OvFiCH1_en		(28)
#define	CIGCTRL_IRQ_OvFiCH0_en		(27)
#define CIGCTRL_DEBUG_EN			(26)
#define	CIGCTRL_IRQ_en				(24)
#define	CIGCTRL_IRQ_Int_Mask_Pr		(22)
#define	CIGCTRL_IRQ_Int_Mask_Co		(20)
#define	CIGCTRL_IRQ_Bad_SYN_en		(19)
#define	CIGCTRL_InvPolCAMPCLK		(5)
#define	CIGCTRL_InvPolCAMVSYNC		(4)
#define	CIGCTRL_InvPolCAMHREF		(3)
#define	CIGCTRL_InvPolCAMFIELD		(2)
#define	CIGCTRL_CamRst				(1)
#define	CIGCTRL_SwRst				(0)

// CICOTRGSIZE
#define	CICOTRGSIZE_TargetHsize		(16)
#define	CICOTRGSIZE_TargetVsize		(0)
// CICOTRGFMT
#define	CICOTRGFMT_ycc422			(31)
#define	CICOTRGFMT_HalfWordSwap		(30)
#define	CICOTRGFMT_ByteSwap			(29)
#define	CICOTRGFMT_StoredFormat		(15)
// CIPRTRGSIZE
#define	CIPRTRGSIZE_TargetHsize		(16)
#define	CIPRTRGSIZE_TargetVsize		(0)
// CIPRTRGFMT
#define	CIPRTRGFMT_HalfWordSwap		(30)
#define	CIPRTRGFMT_ByteSwap			(29)
#define	CIPRTRGFMT_BPP24BL			(15)
#define	CIPRTRGFMT_BPP16Format		(14)
#define	CIPRTRGFMT_StoreFormat		(0)
// CIPRSCPRERATIO
#define	CIPRSCPRERATIO_PreHorRatio	(16)
#define	CIPRSCPRERATIO_PreVerRatio	(0)
// CIIMGCPT
#define	CIIMGCPT_CAMIF_EN			(31)
#define	CIIMGCPT_ImgCptEn_CoSc		(30)
#define	CIIMGCPT_ImgCptEn_PrSc		(29)
#define	CIIMGCPT_OneShot_CoSc		(28)
#define	CIIMGCPT_OneShot_PrSc		(27)
// CICPTSTATUS
#define	CICPTSTATUS_OvFiCH4_Pr		(31)
#define	CICPTSTATUS_OvFiCH3_Pr		(30)
#define	CICPTSTATUS_OvFiCH2_Co		(29)
#define	CICPTSTATUS_OvFiCH1_Co		(28)
#define	CICPTSTATUS_OvFiCH0_Co		(27)
#define	CICPTSTATUS_UNFICH4STS		(26)
#define	CICPTSTATUS_UNFICH3STS		(25)
#define	CICPTSTATUS_UNFICH2STS		(24)
#define	CICPTSTATUS_UNFICH1STS		(23)
#define	CICPTSTATUS_UNFICH0STS		(22)
#define	CICPTSTATUS_P_PATH_FIFO_DIRTY_STATUS	(21)
#define	CICPTSTATUS_C_PATH_FIFO_DIRTY_STATUS	(20)
#define	CICPTSTATUS_BadSynFlag		(19)
#define	CICPTSTATUS_P_PATH_LEISURE	(18)
#define	CICPTSTATUS_C_PATH_LEISURE	(17)
#define	CICPTSTATUS_DMA_CH4_Once	(16)
#define	CICPTSTATUS_DMA_CH3_Once	(15)
#define	CICPTSTATUS_DMA_CH2_Once	(14)
#define	CICPTSTATUS_DMA_CH1_Once	(13)
#define	CICPTSTATUS_DMA_CH0_Once	(12)
#define	CICPTSTATUS_DMA_CH4_Twice	(11)
#define	CICPTSTATUS_DMA_CH3_Twice	(10)
#define	CICPTSTATUS_DMA_CH2_Twice	(9)
#define	CICPTSTATUS_DMA_CH1_Twice	(8)
#define	CICPTSTATUS_DMA_CH0_Twice	(7)
#define	CICPTSTATUS_Smart_Over_Status_Pr	(6)
#define	CICPTSTATUS_Smart_Over_Status_Co	(5)
#define	CICPTSTATUS_Frame_Over_Status_Pr	(4)
#define	CICPTSTATUS_Frame_Over_Status_Co	(3)
#define	CICPTSTATUS_P_PATH_DMA_SUCCESS		(2)
#define	CICPTSTATUS_C_PATH_DMA_SUCCESS		(1)
#define	CICPTSTATUS_DMA_HAS_BEGIN_POP		(0)
// CIPRFIFOSTATUS
#define	CIPRFIFOSTATUS_CH4_fifo_wordcnt		(16)
#define	CIPRFIFOSTATUS_CH3_fifo_wordcnt		(0)
// CICOFIFOSTATUS
#define	CICOFIFOSTATUS_CH2_fifo_wordcnt	(16)
#define	CICOFIFOSTATUS_CH1_fifo_wordcnt	(8)
#define	CICOFIFOSTATUS_CH0_fifo_wordcnt	(0)
// CIMATRIX
#define	CIMATRIX_CFG_toRGB		(31)
#define	CIMATRIX_CFG_PassBy		(30)
#define	CIMATRIX_CFG_InvMsbIn	(29)
#define	CIMATRIX_CFG_InvMsbOut	(28)
#define	CIMATRIX_CFG_OftB		(8)
#define	CIMATRIX_CFG_OftA		(0)
// CIOBSSTATUS
#define	CIOBSSTATUS_VSYNC		(23)
#define	CICPTSTATUS_ImgCptEn_CoSc	(21)
#define	CICPTSTATUS_ImgCptEn_PrSc	(20)
// CICHxCTRL
#define	CICHxCTRL_DMAEn			(31)
#define	CICHxCTRL_RST			(30)
#define	CICHxCTRL_WorkRegInd	(28)
#define	CICHxCTRL_AltEn			(27)
#define	CICHxCTRL_Autoload		(26)
#define	CICHxCTRL_Dir			(25)
#define	CICHxCTRL_DMALen		(0)
// CICHxALTCTRL
#define	CICHxALTCTRL_AltEn		(27)
#define	CICHxALTCTRL_Autoload	(26)
#define	CICHxALTCTRL_Dir		(25)
#define	CICHxALTCTRL_DMALen		(0)
//#############################################################################
//[ITU601]/[ITU656]
#define CAMIF_ITU_MODE_601				(1)
#define CAMIF_ITU_MODE_656				(0)
//define interlaced/progressive
#define CAMIF_SCAN_MODE_PROGRESSIVE		(0)
#define CAMIF_SCAN_MODE_INTERLACED		(1)	
//[YcbYcr]/[YcrYcb]/[cbYcrY]/[crYcbY]
#define CAMIF_INPUT_YUV_ORDER_YCbYCr	(0)
#define CAMIF_INPUT_YUV_ORDER_YCrYCb	(1)
#define CAMIF_INPUT_YUV_ORDER_CbYCrY	(2)
#define CAMIF_INPUT_YUV_ORDER_CrYCbY	(3)
//the polarity of CAMPCLK
#define CAMIF_POLARITY_CAMPCLK_NORMAL	(0)
#define CAMIF_POLARITY_CAMPCLK_INVERSE	(1)
//the polarity of CAMVSYNC
#define CAMIF_POLARITY_CAMVSYNC_NORMAL	(0)
#define CAMIF_POLARITY_CAMVSYNC_INVERSE	(1)
//the polarity of CAMHREF
#define CAMIF_POLARITY_CAMHREF_NORMAL	(0)
#define CAMIF_POLARITY_CAMHREF_INVERSE	(1)
//the polarity of CAMFIELD
#define CAMIF_POLARITY_CAMFIELD_NORMAL	(0)
#define CAMIF_POLARITY_CAMFIELD_INVERSE	(1)
//intr source  
#define CAMIF_INTR_SOURCE_FRAME_END		(1)
#define CAMIF_INTR_SOURCE_DMA_DONE		(2)
#define CAMIF_INTR_SOURCE_SMART			(3)
//codec target format 420/422 
#define CODEC_IMAGE_FORMAT_YUV422		(1)
#define CODEC_IMAGE_FORMAT_YUV420       (0)
#define CODEC_STORE_FORMAT_PLANAR		(0)
#define CODEC_STORE_FORMAT_SPLANAR		(1)
#define CODEC_STORE_INTERLEAVED         (2)
//preview target format 
#define PREVIEW_16BIT_RGB			(0)
#define PREVIEW_24BIT_RGB			(1)
#define PREVIEW_SPLANAR_420			(2)
#define PREVIEW_SPLANAR_422         (3)
#define PREVIEW_BPP16_RGB565        (0)
#define PREVIEW_BPP16_RGB5551		(1)
#define PREVIEW_BPP24BL_BIG_ENDIAN	(1)
#define PREVIEW_BPP24BL_LITTLE_ENDIAN   (0)
// [YUV]/[YUV-->RGB]
#define CAMIF_MATRIX_DIR_RGBtoYUV	(0)
#define CAMIF_MATRIX_DIR_YUVtoRGB	(1)		
// camif path.
#define CAMIF_PATH_PREVIEW			(0)
#define CAMIF_PATH_CODEC			(1)
// ping-ping buffer mode
#define CAMIF_PPMODE_DISABLE		(0)	
#define CAMIF_PPMODE_1_BUFFER		(1)
#define CAMIF_PPMODE_2_BUFFER		(2)	
#define CAMIF_PPMODE_4_BUFFER		(4)	
//codec win limite 
#define	CAMIF_CO_XSIZE_MIN		(4)
#define	CAMIF_CO_YSIZE_MIN		(2)
#define	CAMIF_CO_XSIZE_MAX		(0x1FFF)
#define	CAMIF_CO_YSIZE_MAX		(0x1FFF)
// preview win limite
#define	CAMIF_PR_XSIZE_MIN		(4)
#define	CAMIF_PR_YSIZE_MIN		(2)
#define	CAMIF_PR_XSIZE_MAX		(0x1FFF)
#define	CAMIF_PR_YSIZE_MAX		(0x1FFF)
// camif intt
#define CAMIF_INTR_PREVIEW		(1<<0)
#define CAMIF_INTR_CODEC		(1<<1)
#define CAMIF_INTR_ERROR		(1<<2)

struct camif_config_t {
	u32 in_width;
	u32 in_height;
	u32 xoffset;
	u32 yoffset;
	__u8 itu601;
	__u8 uvoffset;
	__u8 interlaced;
	u32  yuv_sequence;
	__u8 invpclk;
	__u8 invvsync;
	__u8 invhref;
	__u8 invfield;
	__u8 c_enable;
	u32  c_width;
	u32  c_height;
	__u8 c_format;
	__u8 c_store_format;
	__u8 c_hwswap;
	__u8 c_byteswap;
	__u8 pre_enable;
	u32  pre_width;
	u32  pre_height;
	__u8 pre_store_format;
	__u8 pre_hwswap;
	__u8 pre_byteswap;
	__u8 pre_bpp16fmt;
	__u8 pre_rgb24_endian;
	u32  intt_type;
};

void imapx_camif_cfg_addr(struct imapx_video *vp, 
						struct camif_buffer *buf,
						int index);
void imapx_camif_enable_capture(struct imapx_video *vp);
void imapx_camif_disable_capture(struct imapx_video *vp);
void imapx_camif_host_deinit(struct imapx_video *vp);
void imapx_camif_host_init(struct imapx_video *vp);
int imapx_camif_set_sensor_power(struct imapx_video *vp, uint32_t on);
int  imapx_camif_set_fmt(struct imapx_video *vp);
void imapx_camif_dump_regs(struct imapx_video *vp);
#endif
