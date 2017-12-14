
/* **************************************************************************** 
 * ** 
 * ** Copyright (c) 2014~2112 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Soc FPGA verify: csi_dma.c :offer csi dma operation
 * **
 * ** Revision History: 
 * ** ----------------- 
 * ** v0.0.1	sam.zhou@2014/06/23: first version.
 * **
 * *****************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>

#include <linux/moduleparam.h>
#include <linux/ctype.h>
#include <linux/crypto.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/random.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/file.h>

#include <linux/camera/csi/csi_core.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_dma.h>
//#include <malloc.h>

csi_dma_config_t gdcfg = {0};
extern struct csi2_host g_host;

#define regW(value, offset) csi_write(&g_host, offset, value)
#define regR(offset) csi_read(&g_host, offset)

error_t csi_dma_init(csi_dma_config_t *dcfg)
{
	uint32_t i;
	uint32_t ispBytSwp, ispHafSwp, dmaBytSwp, dmaHafSwp;
	
	regW(((dcfg->fmtFixedRGB888?1:0)<<CDRCONFIG_RGB888) | (dcfg->vc << CDRCONFIG_VC), CSIDMAREG_CONFIG);

	ispBytSwp = dcfg->swp.ispBytSwp?1:0;
	ispHafSwp = dcfg->swp.ispHafSwp?1:0;
	dmaBytSwp = dcfg->swp.dmaBytSwp?1:0;
	dmaHafSwp = dcfg->swp.dmaHafSwp?1:0;
	regW((ispBytSwp<<CDRALTCFG3_ISPBYTSWP) | ( ispHafSwp<<CDRALTCFG3_ISPHAFSWP) | (dmaBytSwp<<CDRALTCFG3_DMABYTSWP) | (dmaHafSwp<<CDRALTCFG3_DMAHAFSWP), CSIDMAREG_ALTCFG3);
	
	for(i=0; i<4; i++){
		regW((uint32_t)dcfg->dma[i].buffer.phy_addr, CSIDMAREG_ALT0ADDR + i*8);
		regW(dcfg->dma[i].length, CSIDMAREG_ALT0LEN + i*8);
	}

	regW((0x7<<10) | (0x7<<7) | (0x7<<4) | (0x7<<1), CSIDMAREG_ALTCFG);
	//don't mask interrupt
	regW(~((1<<CDRINT_BLOCKEND) | (1<<CDRINT_FIFOOV) | (1 << CDRINT_FRAME) |
				(1 << CDRINT_DMADONE) | (1 << CDRINT_MIPIFIFO_FULL)), CSIDMAREG_INTMASK);


	//memcpy((void *)&gdcfg, dcfg, sizeof(csi_dma_config_t));

	return SUCCESS;
}

error_t csi_dma_deinit(void)
{
	csi_dma_stop();
	regW(0, CSIDMAREG_CONFIG);
	return SUCCESS;
}

error_t csi_dma_start(void)
{
	uint32_t tmp;

	tmp = regR(CSIDMAREG_ALTCFG);
	tmp |= (1<<CDRALTCFG_ENABLE);
	regW(tmp, CSIDMAREG_ALTCFG);
	return SUCCESS;
}

error_t csi_dma_stop(void)
{
	uint32_t tmp;

	tmp = regR(CSIDMAREG_ALTCFG);
	tmp &= ~(1<<CDRALTCFG_ENABLE);
	regW(tmp, CSIDMAREG_ALTCFG);
	return SUCCESS;
}

error_t csi_dma_reset(void)
{
	uint32_t tmp;

	tmp = regR(CSIDMAREG_ALTCFG3);
	tmp |= (1<<CDRALTCFG3_DMARST);
	regW(tmp, CSIDMAREG_ALTCFG3);
	return SUCCESS;
}

error_t csi_dma_set_swap(csi_swp_t *swp)
{
	uint32_t ispBytSwp, ispHafSwp, dmaBytSwp, dmaHafSwp;
	ispBytSwp = swp->ispBytSwp?1:0;
	ispHafSwp = swp->ispHafSwp?1:0;
	dmaBytSwp = swp->dmaBytSwp?1:0;
	dmaHafSwp = swp->dmaHafSwp?1:0;
	regW((ispBytSwp<<CDRALTCFG3_ISPBYTSWP) |( ispHafSwp<<CDRALTCFG3_ISPHAFSWP) | (dmaBytSwp<<CDRALTCFG3_DMABYTSWP) | (dmaHafSwp<<CDRALTCFG3_DMAHAFSWP), CSIDMAREG_CONFIG);

	return SUCCESS;
}

error_t csi_dma_clean_intr(void)
{
	regW(0xffffffff, CSIDMAREG_INTSTAT);
	return SUCCESS;
}

error_t csi_get_pkt(csi_pkt_info_t *pktinfo, csi_buf_t *replaceBuffer)
{
	uint32_t ready = regR(CSIDMAREG_ALTCFG2) & 0x3;
	uint32_t info = regR(CSIDMAREG_PKTINFO);
	if(ready == 0){
		ready = 3;
	}else{
		ready -= 1;
	}

	pktinfo->vc = (info >> CDRPKTINFO_VC) & 0x3;
	pktinfo->dt = (info >> CDRPKTINFO_DT) & 0x3f;
	pktinfo->ecc = (info >> CDRPKTINFO_ECC) & 0xff;
	pktinfo->wc = (info >> CDRPKTINFO_WC) & 0xffff;
	pktinfo->buffer.vir_addr = gdcfg.dma[ready].buffer.vir_addr;
	pktinfo->buffer.phy_addr = gdcfg.dma[ready].buffer.phy_addr;
	pktinfo->buffer.size = gdcfg.dma[ready].buffer.size;
	pktinfo->buffer.flag = gdcfg.dma[ready].buffer.flag;

	if(replaceBuffer){
		regW((uint32_t)replaceBuffer->phy_addr, CSIDMAREG_ALT0ADDR + ready*4);
		gdcfg.dma[ready].buffer.vir_addr = replaceBuffer->vir_addr;
		gdcfg.dma[ready].buffer.phy_addr = replaceBuffer->phy_addr;
		gdcfg.dma[ready].buffer.size = replaceBuffer->size;
		gdcfg.dma[ready].buffer.flag = replaceBuffer->flag;
	}

	return SUCCESS;
}

error_t csi_dma_config(csi_dma_config_t *cdma, uint32_t dma_size)
{
	int i;
	//set virtual channel
	cdma->vc = CSI_VC_USED;
	cdma->swp.ispBytSwp = CSI_DISABLE;
	cdma->swp.ispHafSwp = CSI_DISABLE;
	cdma->swp.dmaBytSwp = CSI_DISABLE;
	cdma->swp.dmaHafSwp = CSI_DISABLE;
	cdma->fmtFixedRGB888 = CSI_DISABLE;
	g_host.dma_buf_saddr = kmalloc(CSI_DMA_BUF_LEN, GFP_KERNEL);
	if (NULL == g_host.dma_buf_saddr) {
		CSI_ERR("malloc dma buffer failed\n");
		return EPNULL;
	}

	printk("-->csi dma target buffer address = 0x%x\n", (uint32_t)g_host.dma_buf_saddr);
	memset((void *)g_host.dma_buf_saddr, 0, CSI_DMA_BUF_LEN);
	

	for(i = 0; i < 4; i++) {
		cdma->dma[i].buffer.size = dma_size;
		cdma->dma[i].buffer.vir_addr = g_host.dma_buf_saddr;
		cdma->dma[i].buffer.phy_addr = g_host.dma_buf_saddr;
		cdma->dma[i].length = dma_size;
	}

	return SUCCESS;
}
