/* Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** FPGA soc verify mipi csi2 dma defination header file     
**
** Revision History: 
** ----------------- 
** v0.0.1	sam.zhou@2014/06/23: first version.
**
*****************************************************************************/

#ifndef __CSI_DMA_H__
#define __CSI_DMA_H__

#ifdef __cplusplus
extern  "C"
{	
#endif
//
// regs.
//
#define CSIDMAREG_CONFIG	(0x400)
#define CSIDMAREG_INTSTAT	(0x404)
#define CSIDMAREG_INTMASK	(0x408)
#define CSIDMAREG_ALTCFG	(0x40c)
#define CSIDMAREG_ALT0ADDR	(0x410)
#define CSIDMAREG_ALT0LEN	(0x414)
#define CSIDMAREG_ALT1ADDR	(0x418)
#define CSIDMAREG_ALT1LEN	(0x41c)
#define CSIDMAREG_ALT2ADDR	(0x420)
#define CSIDMAREG_ALT2LEN	(0x424)
#define CSIDMAREG_ALT3ADDR	(0x428)
#define CSIDMAREG_ALT3LEN	(0x42c)
#define CSIDMAREG_PKTINFO	(0x430)
#define CSIDMAREG_ALTCFG2	(0x434)
#define CSIDMAREG_ALTCFG3	(0x438)


//
// reg bits.
//
// CSIDMAREG_CONFIG
#define CDRCONFIG_RGB888	(3)	// else csi format.
#define CDRCONFIG_VC		(0)

// CSIDMAREG_INTSTAT & CSIDMAREG_INTMASK
#define CDRINT_MIPIFIFO_FULL   (4)
#define CDRINT_FRAME			3
#define CDRINT_FIFOOV		2
#define CDRINT_BLOCKEND		1
#define CDRINT_DMADONE		0

// CSIDMAREG_ALTCFG
#define CDRALTCFG_CH3AUTO	12
#define CDRALTCFG_CH3ALTEN	11
#define CDRALTCFG_CH3DIROUT	10
#define CDRALTCFG_CH2AUTO	9
#define CDRALTCFG_CH2ALTEN	8
#define CDRALTCFG_CH2DIROUT	7
#define CDRALTCFG_CH1AUTO	6
#define CDRALTCFG_CH1ALTEN	5
#define CDRALTCFG_CH1DIROUT	4
#define CDRALTCFG_CH0AUTO	3
#define CDRALTCFG_CH0ALTEN	2
#define CDRALTCFG_CH0DIROUT	1
#define CDRALTCFG_ENABLE		0

// CSIDMAREG_ALTCFG2
#define CDRALTCFG2_WKING_INDEX	0

// CSIDMAREG_ALTCFG3
#define CDRALTCFG3_DMAHAFSWP	4
#define CDRALTCFG3_DMABYTSWP	3
#define CDRALTCFG3_ISPHAFSWP	2
#define CDRALTCFG3_ISPBYTSWP	1
#define CDRALTCFG3_DMARST		0

// CSIDMAREG_PKTINFO
#define CDRPKTINFO_DT		19	// [31:26]
#define CDRPKTINFO_ECC		18	// [25:18]
#define CDRPKTINFO_VC		16	// [17:16]	
#define CDRPKTINFO_WC		0	// [15:0]

//define configuration
#define CSI_VC_USED 		(1)
#define CSI_DMA_BUF_LEN 	(1024*1024*8)	
#define CSI_FRAME_SIZE		(48*48) //48*48 pixels
#define CSI_SENSOR_FRAME_SIZE (1920*1080) //1920*1080 pixels

//
typedef struct {
	void *vir_addr;
	void *phy_addr;
	uint32_t size;
	uint32_t flag;
}csi_buf_t;

//
typedef struct{
	uint8_t		ispBytSwp;
	uint8_t		ispHafSwp;
	uint8_t		dmaBytSwp;
	uint8_t		dmaHafSwp;
}csi_swp_t;

//define dma buffer structure
typedef struct{
	csi_buf_t 	buffer;
	uint32_t	length;
}csi_dma_t;
//define dma configuration structure
typedef struct{
	uint32_t		vc;
	uint8_t		fmtFixedRGB888;	// else csi format.
	csi_swp_t		swp;
	csi_dma_t		dma[4];
}csi_dma_config_t;

typedef struct{
	uint32_t		vc;
	uint32_t		dt;
	uint32_t		ecc;
	uint32_t		wc;
	csi_buf_t		buffer;
}csi_pkt_info_t;


//
//
error_t csi_dma_init(csi_dma_config_t *dcfg);
error_t csi_dma_deinit(void);
error_t csi_dma_start(void);
error_t csi_dma_stop(void);
error_t csi_dma_reset(void);
error_t csi_dma_set_swap(csi_swp_t *swp);
error_t csi_dma_clean_intr(void);
error_t csi_get_pkt(csi_pkt_info_t *pktinfo,  csi_buf_t *replaceBuffer);
error_t csi_dma_config(csi_dma_config_t *cdma, uint32_t dma_size);

#ifdef __cplusplus
}
#endif
#endif	// __CSI_DMA_H__
