#include <linux/config.h>
//#include <common.h>
//#include <command.h>
#include <bootlist.h>
#include <asm-arm/arch-q3f/ssp.h>
#include <malloc.h>
#include <ssp.h>
#include <linux/types.h>
#include <vstorage.h>
#include <asm-arm/io.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
//#include <asm/io.h>
#include <efuse.h>
#include <asm-arm/timer.h>
#include <asm-arm/arch-q3f/imapx_sdi.h>
#include <asm-arm/arch-q3f/imapx_pads.h>
#include <asm-arm/arch-q3f/imapx_base_reg.h>
//#define SDHC_DMA_TEST
#include <preloader.h>


#define memcpy irf->memcpy
#define module_enable irf->module_enable
#define pads_chmod irf->pads_chmod
#define pads_pull irf->pads_pull
#define sprintf irf->sprintf
#define printf irf->printf
#define module_get_clock irf->module_get_clock

struct sdhc_dma_desc
{
	uint des0;
#define IDMAC_DES0_DIC	(1<<1)
#define IDMAC_DES0_LD	(1<<2)
#define IDMAC_DES0_FD	(1<<3)
#define IDMAC_DES0_CH	(1<<4)
#define IDMAC_DES0_ER	(1<<5)
#define IDMAC_DES0_CES	(1<<30)
#define IDMAC_DES0_OWN	(1<<31)		
	uint des1;
	uint des2;
	uint des3;
};


static int32_t mmc_readl(struct mmc *mmc, int32_t offset)
{
	return *(volatile int32_t *)(mmc->sdhc_base_pa + offset);
}

static void mmc_writel(struct mmc *mmc, int32_t val, int32_t offset)
{
	*(volatile int32_t *)(mmc->sdhc_base_pa + offset) = val;
}

#if 0
static int64_t mmc_readq(struct mmc *mmc, int32_t offset)
{
	return *(volatile int64_t *)(mmc->sdhc_base_pa + offset);
}

static void mmc_writeq(struct mmc *mmc, int64_t val, int32_t offset)
{
	*(volatile int64_t *)(mmc->sdhc_base_pa + offset) = val;
}
#endif

static void mci_pull_data32(struct mmc *mmc, void *buf, int len)
{
	uint32_t *pdata = (uint32_t *)buf;
	
	len = len >> 2;
	while(len > 0) {
		*pdata++ = mmc_readl(mmc, SDMMC_DATA);
		len--;
	}
}

static void mci_push_data32(struct mmc *mmc, void *buf, int len)
{
	uint32_t *pdata = (uint32_t *)buf;

	len = len >> 2;
	while(len > 0) {
		mmc_writel(mmc, *pdata++, SDMMC_DATA);
		len--;
	}
}

#if 0
static void mci_pull_data64(struct mmc *mmc, void *buf, int len)
{
	uint64_t *pdata = (uint64_t *)buf;

	len = len >> 3;
	while(len > 0) {
		*pdata++ = mmc_readq(mmc, SDMMC_DATA);
		len--;
	}
}

static void mci_push_data64(struct mmc *mmc, void *buf, int len)
{
	uint64_t *pdata = (uint64_t *)buf;

	len = len >> 3;
	while(len > 0) {
		mmc_writeq(mmc, *pdata++, SDMMC_DATA);
		len--;
	}
}
#endif

static uint32_t mci_prepare_command(struct mmc_cmd *cmd, struct mmc_data *data)
{
	uint32_t cmdr = 0;

	if(cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		cmdr |= SDMMC_CMD_STOP;
	else
		cmdr |= SDMMC_CMD_PRV_DAT_WAIT;
	if(cmd->resp_type & MMC_RSP_CRC)
		cmdr |= SDMMC_CMD_RESP_CRC;
	/* fpga mode use, when asic is delete?????? */
	cmdr |= SDMMC_CMD_USEHOLD;
	if(cmd->resp_type & MMC_RSP_PRESENT)
	{
		cmdr |= SDMMC_CMD_RESP_EXP;
		if(cmd->resp_type & MMC_RSP_136)
			cmdr |= SDMMC_CMD_RESP_LONG;
	}

	if(data) {
		cmdr |= SDMMC_CMD_DAT_EXP;
		if(data->flags & MMC_DATA_WRITE)
			cmdr |= SDMMC_CMD_DAT_WR;
	}
	
	return cmdr|(cmd->cmdidx); 

}

static void imap_sdhc_reset(struct mmc *mmc)
{
	uint64_t tt;
	uint32_t ctrl;

	mmc_writel(mmc, 0x7, SDMMC_CTRL);
	tt = get_ticks();
	do {
		ctrl = mmc_readl(mmc, SDMMC_CTRL);
		if(!(ctrl & 0x7))
			return ;
		udelay(1);
	}
	while((get_ticks()-tt) < 500000);
	printf("sdhc_reset failed\n");
	return;
}

#ifndef SDHC_DMA_TEST		
static int imap_pio_read_write(struct mmc *mmc, struct mmc_data *data)
{
	uint bytes_xfered, bytes_total, len;
        uint intsts;
	char *buffer;
	int ret = 0;
	unsigned long long start_tick;

	bytes_total = data->blocks*data->blocksize;
	bytes_xfered = 0;
	start_tick = get_ticks();
	if(data->flags & MMC_DATA_READ) 
	{
		buffer = data->dest;
		/* 2000ms is problem, clk*width*len*/
		do {
			if(get_ticks() > (start_tick+1000000)){
				printf("~!read time out,bus width:%d\n", mmc->bus_width);
				return COMM_ERR;
			}
			intsts = mmc_readl(mmc, SDMMC_RINTSTS);
			if(intsts & (SDMMC_INT_RXDR|SDMMC_INT_DATA_OVER))
			{
				if(intsts & SDMMC_INT_RXDR)
					mmc_writel(mmc, SDMMC_INT_RXDR, SDMMC_RINTSTS);
				len = ((SDMMC_GET_FCNT(mmc_readl(mmc, SDMMC_STATUS))) << mmc->shift);
				if((bytes_total - bytes_xfered) < len)
					len = bytes_total -bytes_xfered;
				mmc->pull_data(mmc, buffer + bytes_xfered, len);
				bytes_xfered += len;
			}
		}
		while(((intsts & SDMMC_INT_DATA_OVER) == 0));
	}
	else
	{
		buffer = (char *)data->src;
		/* 2000ms is problem, allow clk*width*len */
		do {
			if(get_ticks() > (start_tick+1000000)){
				printf("~!write time out,bus width:%d\n", mmc->bus_width);
				return COMM_ERR;
			}
			intsts = mmc_readl(mmc, SDMMC_RINTSTS);
			if(intsts & SDMMC_INT_TXDR)
			{
				mmc_writel(mmc, SDMMC_INT_TXDR, SDMMC_RINTSTS);
				len = ((SDMMC_FIFO_SZ - SDMMC_GET_FCNT(mmc_readl(mmc, SDMMC_STATUS))) << mmc->shift);
				if((bytes_total-bytes_xfered) < len)
					len = bytes_total-bytes_xfered;
				mmc->push_data(mmc, buffer+bytes_xfered, len);
				bytes_xfered += len;
			}
		}
		while((intsts & SDMMC_INT_DATA_OVER) == 0);
	}
	/* data transfer complete or timeout to transfer error */
	if(!(intsts & SDMMC_INT_DATA_OVER))
		ret = TIMEOUT;

	mmc_writel(mmc, intsts, SDMMC_RINTSTS);
	return ret;
}
#endif

static int imap_sdhc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	uint32_t cmdflags, intsts, bytes_total;
	uint64_t tt;
	int ret = 0;
#ifdef SDHC_DMA_TEST		
	uint32_t bytes_remained, len;
	struct sdhc_dma_desc *p;
	uint32_t data_buf_addr, offset, val, val1;
#endif

	/* set up for a data transfer */
	if(data) {
		bytes_total = data->blocksize*data->blocks;
		if(data->flags & MMC_DATA_READ)
			mmc_writel(mmc, (data->blocksize<<16)|0x1, SDMMC_CARDTHRCTL);
		mmc_writel(mmc, data->blocksize, SDMMC_BLKSIZ);
		mmc_writel(mmc, bytes_total, SDMMC_BYTCNT);
#ifdef SDHC_DMA_TEST		
		/* start dma operation */
		bytes_remained = bytes_total;
		offset = 0;
		//printf("send_data: 0x%x\n", bytes_total);

		val = mmc_readl(mmc, SDMMC_CTRL);
		val |= (1<<5);
		mmc_writel(mmc, val, SDMMC_CTRL);

		p = (struct sdhc_dma_desc *)mmc->dma_desc_addr;
		if(data->flags & MMC_DATA_READ)
			data_buf_addr = (uint32_t)data->dest;
		else
			data_buf_addr = (uint32_t)data->src;
#define DMA_XFER_SIZE	8176		
		do {
			if(bytes_remained > DMA_XFER_SIZE)
				len = DMA_XFER_SIZE;
			else
				len = bytes_remained;
			p->des0 = IDMAC_DES0_OWN|IDMAC_DES0_DIC|IDMAC_DES0_CH;
			p->des1 = (p->des1 & 0x3ffe000)|(len&0x1fff);
			p->des2 = data_buf_addr + offset;
			bytes_remained -= len;
			offset += len;
			p++;
		}
		while(bytes_remained);
		/* set last descriptor */
		p--;
		p->des0 &=~(IDMAC_DES0_DIC);
	       	p->des0 |= IDMAC_DES0_LD;
		/* set first descriptor */
		p = (struct sdhc_dma_desc *)mmc->dma_desc_addr;
		p->des0 |= IDMAC_DES0_FD;

		val = mmc_readl(mmc, SDMMC_CTRL);
		val |= (1<<25);
		mmc_writel(mmc, val, SDMMC_CTRL);
		val = mmc_readl(mmc, SDMMC_BMOD);
		val |= (1<<7);
		mmc_writel(mmc, val, SDMMC_BMOD);
		mmc_writel(mmc, 0x1, SDMMC_PLDMND);
#endif		
	}
	cmdflags = mci_prepare_command(cmd, data);
	if(mmc->card_need_init) {
		cmdflags |= SDMMC_CMD_INIT;
		mmc->card_need_init = 0;
	}
	//printf("send_cmd:0x%x\n", cmdflags);
	mmc_writel(mmc, cmd->cmdarg, SDMMC_CMDARG);
	mmc_writel(mmc, cmdflags|SDMMC_CMD_START, SDMMC_CMD);
	
	/* timeout = 100 ms ????? */
	tt = get_ticks();
	while(1)
       	{
		if(get_ticks()-tt > 100000)
		{
			printf("cmd time out\n");
			return COMM_ERR;	
		}
		intsts = mmc_readl(mmc, SDMMC_RINTSTS);
		if(intsts & SDMMC_INT_CMD_DONE)
			break;
	}
	intsts = mmc_readl(mmc, SDMMC_RINTSTS);
	mmc_writel(mmc, SDMMC_INT_CMD_DONE, SDMMC_RINTSTS);

	if(intsts & DW_MCI_CMD_ERROR_FLAGS)
	{
		mmc_writel(mmc, intsts, SDMMC_RINTSTS);
		if(cmd->cmdidx == 8
		|| cmd->cmdidx == 55) { /* iNAND detect */ }
		  else
		    printf("cmd%d intsts=0x%x err!\n", cmd->cmdidx, intsts);

		if(intsts & SDMMC_INT_RTO)
			return TIMEOUT;
		else
			return COMM_ERR;
	}
	if(cmd->resp_type & MMC_RSP_136) {
		cmd->response[0] = mmc_readl(mmc, SDMMC_RESP3);
		cmd->response[1] = mmc_readl(mmc, SDMMC_RESP2);
		cmd->response[2] = mmc_readl(mmc, SDMMC_RESP1);
		cmd->response[3] = mmc_readl(mmc, SDMMC_RESP0);
	}
	else 
	{
		cmd->response[0] = mmc_readl(mmc, SDMMC_RESP0);
	}

	/* wait until all of the blocks transfer complete */
	if(data) {
#ifdef SDHC_DMA_TEST
		while(1)
		{
			val = mmc_readl(mmc, SDMMC_RINTSTS);
			val1 = mmc_readl(mmc, SDMMC_IDSTS);
			if(data->flags & MMC_DATA_READ)
			{
				if((val & SDMMC_INT_DATA_OVER) && (val1 & 0x2))
				{
					mmc_writel(mmc, 0x2, SDMMC_IDSTS);
					break;
				}
			}
			else {
				if((val & SDMMC_INT_DATA_OVER) && (val1 & 0x1))
				{
					mmc_writel(mmc, 0x1, SDMMC_IDSTS);
					break;
				}
			}
		}
		mmc_writel(mmc, val, SDMMC_RINTSTS);
		/* Disable and reset the IDMAC interface */
		val = mmc_readl(mmc, SDMMC_CTRL);
		val &= ~(1<<25);
		val |= (1<<2);
		mmc_writel(mmc, val, SDMMC_CTRL);
		/* Stop the IDMAC running */
		val = mmc_readl(mmc, SDMMC_BMOD);
		val &= ~(1<<7);
		mmc_writel(mmc, val, SDMMC_BMOD);
#else		
		ret = imap_pio_read_write(mmc, data);
#endif		
	}

	return ret;
}

static void mci_send_cmd(struct mmc *mmc, uint32_t cmd, uint32_t arg)
{
	uint64_t tt;
	uint32_t cmd_status;

	mmc_writel(mmc, arg, SDMMC_CMDARG);
	mmc_writel(mmc, cmd|SDMMC_CMD_START, SDMMC_CMD);

	/* timeout is 500 ms */
	tt = get_ticks();
	do {
		cmd_status = mmc_readl(mmc, SDMMC_CMD);
		if(!(cmd_status & SDMMC_CMD_START))
			break;
	}
	while((get_ticks()-tt) < 500000);
}
#define CLOCK 65535
static void imap_sdhc_set_ios(struct mmc *mmc)
{
	uint32_t div, sdhc_clk;

	/* set mmc clock */
	if(mmc->clock != mmc->curr_clk)
	{
		sdhc_clk = module_get_clock(SD_CLK_BASE(mmc->port_num));
		if(sdhc_clk <= mmc->clock)
		  div = 0;
		else {
		//	if(sdhc_clk % mmc->clock)
		//	{
		//	   div = ((sdhc_clk / mmc->clock) >> 1) + 1;
		//	}
		//	else
		//       {
			   div = (sdhc_clk / mmc->clock) >> 1;
		//	 }
		}
		/* disable clock */
		mmc_writel(mmc, 0, SDMMC_CLKENA);
		mmc_writel(mmc, 0, SDMMC_CLKSRC);
		/* inform CIU */
		mci_send_cmd(mmc, SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0);
		/* set clock to desired speed */
		mmc_writel(mmc, div, SDMMC_CLKDIV);
		mci_send_cmd(mmc, SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0);
		/* enable clock */
		mmc_writel(mmc, 0x1, SDMMC_CLKENA);
		mci_send_cmd(mmc, SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0);
	}

	/* set bus width */
	if(mmc->bus_width == 4)
		mmc_writel(mmc, 0x1, SDMMC_CTYPE);
	else if(mmc->bus_width == 8)
		mmc_writel(mmc, 0x10001, SDMMC_CTYPE);
	else 
		mmc_writel(mmc, 0, SDMMC_CTYPE);
}


static int imap_sdhc_init(struct mmc *mmc)
{
	unsigned int val;

	mmc->curr_clk = 0;
	mmc->clock = 0;
	mmc->card_need_init = 1;
	switch(mmc->port_num)
	{
	case 0:
		pads_chmod(PADSRANGE(0, 10), PADS_MODE_CTRL, 0);
	//	if(ecfg_check_flag(ECFG_ENABLE_PULL_MMC)) {
			pads_pull(PADSRANGE(0, 10), 1, 1);
			pads_pull(8, 1, 0);		/* clk */
	//	}
		module_enable(MMC0_SYSM_ADDR);
		break;
	case 1:
		pads_chmod(PADSRANGE(31, 37), PADS_MODE_CTRL, 0);
	//	if(ecfg_check_flag(ECFG_ENABLE_PULL_MMC)) {
			pads_pull(PADSRANGE(31, 37), 1, 1);
			pads_pull(35, 1, 0);	/* clk */
	//	}
		module_enable(MMC1_SYSM_ADDR);
		break;
	case 2:
		pads_chmod(PADSRANGE(38, 43), PADS_MODE_CTRL, 0);
	//	if(ecfg_check_flag(ECFG_ENABLE_PULL_MMC)) {
			pads_pull(PADSRANGE(38, 43), 1, 1);
			pads_pull(42, 1, 0);		/* clk */
	//	}
		module_enable(MMC2_SYSM_ADDR);
		break;
	}

	mmc->shift = ((mmc_readl(mmc, SDMMC_HCON)>>18)&0x7) == 2?3:2;
	mmc->pull_data = mci_pull_data32;
	mmc->push_data = mci_push_data32;

	/* by csl, add for dwc_mmc control */
	imap_sdhc_reset(mmc);
	mmc_writel(mmc, 0, SDMMC_INTMASK);
	mmc_writel(mmc, 0xffffffff, SDMMC_RINTSTS);
	mmc_writel(mmc, 0xffffffff, SDMMC_TMOUT);
	val = mmc_readl(mmc, SDMMC_FIFOTH);
	val = (val>>16)&0x7ff;
	mmc_writel(mmc, (0x0<<28)|((val/2)<<16)|(val/2), SDMMC_FIFOTH);
	mmc_writel(mmc, 0, SDMMC_CLKENA);
	val = mmc_readl(mmc, SDMMC_PWREN);
	val |= 0x1;
	mmc_writel(mmc, val, SDMMC_PWREN);
	mmc_writel(mmc, 0, SDMMC_GPIO);
	mmc_writel(mmc, 0x3fa80000, SDMMC_DLYLINE);

#if 0
	/* detecting the card */
	if(imap_mmc_getcd(mmc) == 0)
		return NO_CARD_ERR;
#endif

#ifdef SDHC_DMA_TEST	
	/* idma initialize configure */
#define SDHC_ADMA_DESC_LEN	64	
	mmc_writel(mmc, 0x103, SDMMC_IDINTEN);
	mmc_writel(mmc, mmc->dma_desc_addr, SDMMC_DBADDR);
	val = mmc_readl(mmc, SDMMC_BMOD);
	val &=~0x7c;
	val |= 0x8;
	mmc_writel(mmc, val, SDMMC_BMOD);
//	printf("dma_desc_addr:0x%x, 0x%x\n", mmc->dma_desc_addr, mmc_readl(mmc, SDMMC_DBADDR));
#endif	
	return 0;
}

int  mmc_init_param(int dev_num){                            
	    struct mmc* mmc = find_mmc_device(dev_num);

	mmc->shift = ((mmc_readl(mmc, SDMMC_HCON)>>18)&0x7) == 2?3:2;
	mmc->pull_data = mci_pull_data32;
	mmc->push_data = mci_push_data32;

	mmc->curr_clk = 0;
	mmc->clock = 0;
	mmc->card_need_init = 0;
	mmc->bus_width = 4;
	
	mmc->block_dev.blksz = 512;
	mmc->read_bl_len = 512;
	mmc->write_bl_len = 512;
	mmc->block_dev.lba = 213099097402386;
	mmc->high_capacity = 1;
	mmc->rca = 0;
	mmc->voltages = 0xFF8000;
	mmc->version = MMC_VERSION_4;
	mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_HS;
	mmc->card_caps = MMC_MODE_4BIT | MMC_MODE_HS;
	mmc_set_clock(mmc, 52000000);
	/*	
	printf("mmc->block_dev.blksz=%ld\n",mmc->block_dev.blksz);
	printf("mmc->read_bl_len=%d\n",mmc->read_bl_len);
	printf("mmc->block_dev.lba=0x%llx\n",mmc->block_dev.lba);
	printf("mmc->high_capacity=%d\n",mmc->high_capacity);
	printf("mmc->rca=%d\n",mmc->rca);
	printf("mmc->write_bl_len=%d\n",mmc->write_bl_len);
	printf("mmc->voltages=%d\n",mmc->voltages);
	printf("mmc->version=%d\n",mmc->version);
	printf("mmc->card_caps=%d\n",mmc->card_caps);
	*/
	return 0;
} 

int imap_sdhc_initialize(uint32_t ch, struct mmc *mmc)
{
#ifdef SDHC_DMA_TEST	
	int i;
	struct sdhc_dma_desc *p;
#endif
	if(!mmc) {
		printf("no mmc struct buffer\n");
		return -1;
	}

	/* initialize sdhc ch base address */
	if(ch == 0) {
		//module_enable(MMC0_SYSM_ADDR);
		mmc->sdhc_base_pa = SD0_BASE_ADDR;
	}
	else if(ch == 1) {
		//module_sdmmc_enable(MMC1_SYSM_ADDR);
		mmc->sdhc_base_pa = SD1_BASE_ADDR;
	}
	else if(ch == 2) {
		//module_sdmmc_enable(MMC2_SYSM_ADDR);
		mmc->sdhc_base_pa = SD2_BASE_ADDR;
	}
	else {
		printf("ch para invalid!\n");
		return -1;
	}

	/* by csl, add for dwc_mmc varit initialize */
	mmc->port_num = ch;
	//mmc->curr_clk = 0;
	//mmc->clock = 0;
	//mmc->card_need_init = 1;

	mmc->priv = NULL;
	mmc->send_cmd = imap_sdhc_send_cmd;
	mmc->set_ios = imap_sdhc_set_ios;
	mmc->init = imap_sdhc_init;

	mmc->voltages = 0xff8000; //MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195; 
	//mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_8BIT | MMC_MODE_HS;
	mmc->host_caps = MMC_MODE_4BIT;
	
	mmc->f_min = 400000;
	mmc->f_max = 50000000;

	mmc->b_max = 0;
	mmc->has_init = 0;
	mmc->block_dev.blksz = 512;
#ifdef SDHC_DMA_TEST	
	/* idma initialize configure */
#define SDHC_ADMA_DESC_LEN	64	
	p = (struct sdhc_dma_desc *)mmc->dma_desc_addr;
	for(i=0; i<SDHC_ADMA_DESC_LEN-1; i++, p++)
	{
		p->des3 = mmc->dma_desc_addr + sizeof(struct sdhc_dma_desc)*(i+1);
	}
	p->des3 = mmc->dma_desc_addr;
	p->des0 = IDMAC_DES0_ER;
//	printf("dma_desc_addr:0x%x, 0x%x\n", mmc->dma_desc_addr, mmc_readl(mmc, SDMMC_DBADDR));
#endif	

	mmc_register(mmc);
	return 0;
}

