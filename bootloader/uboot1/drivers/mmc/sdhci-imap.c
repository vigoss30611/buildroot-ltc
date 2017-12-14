/****************************************************
 * ** driver/mmc/sdhci-imap.c
 * **
 * **
 * ** Copyright (c) 2009~2014 ShangHai Infotm .Ltd all rights reserved. 
 * **
 * ** Use of infoTM's code  is governed by terms and conditions 
 * ** stated in the accompanying licensing statment.
 * **	
 * ** Description: u-boot driver for infoTM SD/MMC host controller
 * **
 * ** Author:
 * **
 * **	Neville		<neville@infotm.com>
 * ** Revision History:
 * ** ----------------
 * **  1.1  03/06/2010	neville
 * **************************************************/

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h> 
#include <asm/errno.h>
#include <asm/io.h>

#include "sdhci-imap.h"


#define   DRIVER_NAME "imapx-sdhci"

#define	CONFIG_IMAP_SDI_REGS_BASE  0x20c60000

struct imap_host {
	struct mmc		*mmc;
	unsigned int		base;
	struct mmc_cmd		*cmd;
	struct mmc_data		*data;
	unsigned int 		clock;

	unsigned int 		datalen;
};

static void imap_set_gpio(int channel,int width)
{
	 unsigned int tmp0,tmp1, tmp2;
	 unsigned int cfg_val;

	 switch(channel)
	 {
		 case 0:
			 if(width == 4)
			 {
				  tmp0 = readl(GPFCON);
				  tmp0 = (tmp0 & (~0xFFF)) | 0xaaa;
				  writel(tmp0,GPFCON);
				  tmp1 = readw(GPFPUD);
				  tmp1 &= ~(0x3F);
			          writew(tmp1,GPFPUD);
			 }
			cfg_val = readl(DIV_CFG3);		
			cfg_val &= ~(0xff);        
			cfg_val |= 0x12;           
			writel(cfg_val, DIV_CFG3);


			 break;
		case 1:
			tmp2 = readl(AHBP_EN);  
			tmp2 |= (0x1 << 20);    
			writel(tmp2, AHBP_EN);  
			tmp2 = readl(HCLK_MASK);
			tmp2 &= ~(1 << 20);     
			writel(tmp2 ,HCLK_MASK);
			 if(width == 4)
			 {
				 tmp0 = readl(GPOCON);
				 tmp0 = (tmp0 & (~0xFFF)) | 0xaaa;
				 writel(tmp0,GPOCON);
				 tmp1 = readw(GPOPUD);
				 tmp1 &= ~(0x3F);
				 writew(tmp1,GPOPUD);
			 }
			cfg_val = readl(DIV_CFG3);		
			cfg_val &= ~(0xff00);        
			cfg_val |= 0x1200;           
			writel(cfg_val, DIV_CFG3);
			 break;
		case 2 :
			tmp2 = readl(AHBP_EN);  
			tmp2 |= (0x1 << 21);    
			writel(tmp2, AHBP_EN);  
			tmp2 = readl(HCLK_MASK);
			tmp2 &= ~(1 << 21);     
			writel(tmp2 ,HCLK_MASK);

			 if(width == 4)
			{
		       		tmp0 = readl(GPOCON);
				tmp0 = (tmp0 &(~0xFFF000)) | 0xaaa000;
			      	writel(tmp0,GPOCON);
				tmp1 = readw(GPOPUD);
	                        tmp1 &= ~(0xFC0);	
				writew(tmp1,GPOPUD);			
	   		}
			cfg_val = readl(DIV_CFG3);		
			cfg_val &= ~(0xff0000);        
			cfg_val |= 0x120000;           
			writel(cfg_val, DIV_CFG3);
			 break;
		default :
			 break;
	 }
}

void imap_mmc_writel(struct imap_host *host,int offset,int mode)
{
	writel(mode, host->base+offset);
}

void imap_mmc_writew(struct imap_host *host,int offset,int mode)
{	
	writew(mode,host->base+offset);
}

void imap_mmc_writeb(struct imap_host *host,int offset,int mode)
{
	writeb(mode,host->base+offset);
}

u32 imap_mmc_readl(struct imap_host *host,int offset)
{
	u32 val = readl(host->base + offset);
	if(host->mmc->index == 0)
	{
	}else if((host->mmc->index == 1) ||host->mmc->index == 2 )
	{
		if(offset == IMAP_PRNSTS)
		{
			val |= (7 << IMAP_PRNSTS_CARDIN );
		}
	}
	return val;
}
u16  imap_mmc_readw(struct imap_host *host,int offset)
{
	return readw(host->base+offset);
}
u8  imap_mmc_readb(struct imap_host *host,int offset)
{
	return readb(host->base+offset);
}

static void imap_softreset(struct imap_host *host,u8 mode)
{
	unsigned long timeout;
	imap_mmc_writeb(host,IMAP_SWORST,mode);

	if(mode & IMAP_SWORST_ALL)
		host->clock = 0;
	timeout = 100;
	while(imap_mmc_readb(host,IMAP_SWORST) & mode) {
		if(timeout == 0) {
			printf("[u-boot]-sdhci:Reset never completed.\n");
			return;
		}
		timeout--;
		udelay(1000);
	}

}

static void imap_finish_request(struct imap_host *host,struct mmc_cmd *cmd,struct mmc_data *data)
{
	host->cmd = NULL;
	host->data = NULL;
}

static int imap_read_response(struct imap_host *host,unsigned int stat)
{
	struct mmc_cmd  *cmd = host->cmd;
	u32 *resp = (u32 *)cmd->response;
	int i;
	if(!cmd)
		return 0;
	if(cmd->resp_type & MMC_RSP_PRESENT) {
		 if (cmd->resp_type & MMC_RSP_136) {
			 for(i = 0;i < 4; i++) {
					resp[i] = imap_mmc_readl(host,IMAP_RSPREG0+(3-i)*4) << 8;
					if(i!=3)
						resp[i] |= imap_mmc_readb(host,IMAP_RSPREG0 +(3-1)*4-1);
			 }		
		 } else {
				resp[0] = imap_mmc_readl(host,IMAP_RSPREG0);
			}
	}
	return 0;
}



static int imap_read_block(struct imap_host *host,void *_buf,int bytes)
{
	unsigned int stat;
	u32 *buf = _buf;
	u32 intstatus;
	int i;
	
	while(1) {
		intstatus = imap_mmc_readl(host,IMAP_NORINTSTS);
		imap_mmc_writel(host,IMAP_NORINTSTS,intstatus);

		if(intstatus & IMAP_NORINTSTS_DATA){
			for(i=0;i< 128;i++)
			{
				*buf++ = imap_mmc_readl(host,IMAP_BDATA);
				bytes -= 4;
			}
		}

		if(intstatus & IMAP_NORINTSTS_TRNCMP){

			stat = 0;
			break;
		}
		if(intstatus & 0x700000){
			printf("[u-boot]-sdhci:data error %08x.\n",intstatus);
			stat = 1;
			break;
		}
	}

	return stat;
	
}

static int imap_write_block(struct imap_host *host, void *_buf, int  bytes)
{
	unsigned int stat;
	u32 *buf = _buf;
	u32  intstatus;
	int i;

	while(1) {
		intstatus = imap_mmc_readl(host,IMAP_NORINTSTS);
		imap_mmc_writel(host, IMAP_NORINTSTS,intstatus);
		
		if(intstatus & IMAP_NORINTSTS_SPACE ){
			for(i=0; i<128;i++)
			{
				imap_mmc_writel(host,IMAP_BDATA,*buf++);
				bytes -=4;
			}
		}

		if(intstatus & IMAP_NORINTSTS_TRNCMP){
			stat = 0;
			break;
		}
		if(intstatus & 0x700000) {
			stat = 1;
			break;
		}
	}
	return stat;
}

static int imap_transfer_data(struct imap_host *host)
{
	struct mmc_data *data = host->data;
	int stat;
	unsigned long length ;
	
	length = data->blocks * data->blocksize;
	
	host->datalen = 0;
	if(data->flags & MMC_DATA_READ) {
		stat = imap_read_block(host,data->dest,length);
		if(stat)
			return stat;
		host->datalen += length;
	} else {
		/********************************************\
		 *   WRITE  TODO 
		\********************************************/
		stat = imap_write_block(host,data->dest,length);
		if(stat)
			return stat;
		host->datalen += length;
	}
	return 0;
}

static int imap_finish_data(struct imap_host *host,int stat)
{
	/***TODO***/
	host->data = NULL;

	if(stat)
		return stat;
	return 0;
}

static int imap_cmd_done(struct imap_host *host,unsigned int stat)
{
	int datastatus;
	int ret;
	ret = imap_read_response(host,stat);

	if(ret) {
		imap_finish_request(host,host->cmd,host->data);
		return ret;
	}
	if(!host->data) {
		imap_finish_request(host,host->cmd,host->data);
		return 0;
	}
	datastatus = imap_transfer_data(host);
	ret = imap_finish_data(host,datastatus);
	imap_finish_request(host,host->cmd,host->data);
	if(ret)
	{
	//	printf("[u-boot]-sdhci:reset CMD&DATA line.\n");
		imap_softreset(host,IMAP_SWORST_CMD);
		imap_softreset(host,IMAP_SWORST_DATA);
	}
	return ret;

}

static int imap_send_cmd(struct imap_host *host,struct mmc_cmd *cmd)
{
	u16 mode;
	if(!(cmd->resp_type & MMC_RSP_PRESENT))
		mode = IMAP_CMD_RESPNONE;
	else if (cmd->resp_type & MMC_RSP_136)
		mode = IMAP_CMD_RESPLONG;
	else if (cmd->resp_type & MMC_RSP_BUSY)
		mode = IMAP_CMD_RESPSHORTBUSY;
	else 
		mode = IMAP_CMD_RESPSHORT;
	if(cmd->resp_type & MMC_RSP_CRC)
		mode |= IMAP_CMD_CRC;
	if(cmd->resp_type & MMC_RSP_OPCODE)
		mode |= IMAP_CMD_INDEX;
	if(host->data)
		mode |= IMAP_CMD_DATA;
	
//	printf("[u-boot]-sdhci:send cmd%d.ARG:%08x\n", cmd->cmdidx,cmd->cmdarg);
	imap_mmc_writel(host,IMAP_ARGUMENT,cmd->cmdarg);
	imap_mmc_writew(host,IMAP_CMDREG,IMAP_MAKE_CMD(cmd->cmdidx,mode));
	return 0;
}



static void imap_setup_data(struct imap_host *host,struct mmc_data *data)
{
	unsigned int blknum = data->blocks;
	unsigned int blksz = data->blocksize;
	u16 mode;
	if(data == NULL)
		return;
	host->data = data;
	imap_mmc_writew(host,IMAP_BLKSIZE,IMAP_MAKE_BLKSZ(7,blksz));
	imap_mmc_writew(host,IMAP_BLKCNT,blknum);
	host->datalen = blknum * blksz;
	
	mode = IMAP_TRNMOD_ENBLKCNT;
	if(blknum >1)
		mode |= (IMAP_TRNMOD_ENACMD12 | IMAP_TRNMOD_MUTI);
			
	if(data->flags & MMC_DATA_READ)
		mode |= IMAP_TRNMOD_READ;


/***************************************\
 * FIXME USE of DMA is to be decided
\***************************************/
	imap_mmc_writew(host,IMAP_TRNMOD, mode);
}


/**********************************\
  SDHCI request 
\**********************************/

static int imap_request(struct mmc *mmc, struct mmc_cmd *cmd,struct mmc_data *data)
{
	struct imap_host *host = mmc->priv;
	u32 mask;
	u32 intstatus;
	int ret;
	unsigned long timeout = 10;

//	imap_softreset(host,IMAP_SWORST_ALL);   // reset the host before request
//	imap_softreset(host,IMAP_SWORST_CMD);
//	imap_softreset(host,IMAP_SWORST_DATA);
	mask = IMAP_PRNSTS_CMDINHIBIT;
	if((data!=NULL) || (cmd->resp_type & MMC_RSP_BUSY))
		mask |= IMAP_PRNSTS_DATAINHIBIT;
	if((data!=NULL) &&(cmd->cmdidx == 12))
		mask &= ~IMAP_PRNSTS_DATAINHIBIT;
	while(imap_mmc_readl(host,IMAP_PRNSTS) & mask)
	{
		if(timeout == 0) {
			printf("[u-boot]-sdhci:Cotroller never released CMD/DATA inbits.\n");
			return 1;
		}
		timeout --;
		udelay(1000);
	}

	host->cmd = cmd;
	imap_setup_data(host,data);
	if((ret = imap_send_cmd(host,cmd))){
		printf("[u-boot]-sdhci:send cmd err:%d.\n",ret);
		imap_finish_request(host,cmd,data);
		return ret ;
	}
/**************************************\
 * polling for command complete
\**************************************/
	while(1)
	{
	
	intstatus = imap_mmc_readl(host,IMAP_NORINTSTS);
	if((intstatus & IMAP_NORINTSTS_CMDCMP) !=0)
		break;
	if((intstatus & 0xFF0000) !=0)
		break;
	}
/*clear the interrupts */
	imap_mmc_writel(host,IMAP_NORINTSTS,intstatus);
	if((intstatus & 0xFF0000) !=0){
//		printf("[u-boot]-sdhci:ERROR interrupt %08x.\n",intstatus);
		if((intstatus & 0x10000) !=0)
			return -19;
		else
			return 1;
	}
	else 
		return imap_cmd_done(host,intstatus);


}

static void imap_set_clk(struct imap_host *host,unsigned int clock)
{
	int div;

	unsigned int clk_src;
	u16 clk;
	unsigned long timeout;
	clk_src = imap_mmc_readl(host,IMAP_CONTROL2);
	clk_src |= (1 << 5);
	imap_mmc_writel(host,IMAP_CONTROL2 , clk_src);

//	printf("set EPLL clock\n");
	if(clock == host->clock)
		return;
	imap_mmc_writew(host,IMAP_CLKCON,0);
	if (clock == 0)
		goto out;
	for (div = 1;div <256;div *=2)
	{
		if((host->mmc->f_max/div) <=clock)
			break;
	}
	div >>= 1;
	
	clk = div << IMAP_CLKCON_SHIFT;
	clk |= IMAP_CLKCON_INTEN;
	imap_mmc_writew(host,IMAP_CLKCON,clk);
	/*wait max 10 ms*/
	timeout = 10;
	while(!((clk = imap_mmc_readw(host,IMAP_CLKCON))& IMAP_CLKCON_INTSTABLE))
	{
		if(timeout == 0){
			printf("[u-boot]-sdhci:Internal clock never stablished.\n");
			return;
		}
		timeout--;
		udelay(1000);
	}
	clk |= IMAP_CLKCON_CARDEN;
	imap_mmc_writew(host,IMAP_CLKCON,clk);
	imap_mmc_writeb(host,IMAP_TIMEOUTCON,0xE);
out:
	host->clock = clock;
}

static void imap_set_ios(struct mmc *mmc)
{
	u8 ctrl;
	struct imap_host *host = mmc->priv;
	
	imap_set_clk(host,mmc->clock);
	
	ctrl = imap_mmc_readb(host,IMAP_HOSTCTL);

	if(mmc->bus_width == 4)
		ctrl|= IMAP_HOSTCTL_4BIT;	
	else
	
		ctrl&=~IMAP_HOSTCTL_4BIT;
	
	if(mmc->card_caps & MMC_MODE_HS)
		ctrl |= IMAP_HOSTCTL_HS; 
	else
		ctrl &= ~IMAP_HOSTCTL_HS;

	imap_mmc_writeb(host,IMAP_HOSTCTL,ctrl);
	imap_softreset(host,IMAP_SWORST_CMD |IMAP_SWORST_DATA);
	
/*******************************************************\
FIXME: if mmc->clock = 0 ,we should stop the clock here.
\*******************************************************/
	//else imap_stop_clock(host);
	

}



static int imap_detect_card(struct imap_host *host){
	u32	mode;
	mode = imap_mmc_readl(host,IMAP_PRNSTS);
	if((mode & (7 << IMAP_PRNSTS_CARDIN)) == (7 << IMAP_PRNSTS_CARDIN)){
		imap_mmc_writeb(host,IMAP_PWRCON,IMAP_PWRCON_POWER_330 | IMAP_PWRCON_PWRON);
		udelay(5000);
		return 0;
	}
	else 
		return 1;
}

static int imap_init(struct mmc *mmc)
{
	struct imap_host *host = mmc->priv;
	u32 mask = 0;
	int ret;
/*******************************\
 * step 1  detect card 
\ ******************************/
	imap_softreset(host,IMAP_SWORST_ALL);
//	printf("[u-boot]-sdhci: Searching for SD ...\n");
	if((ret=imap_detect_card(host)))
	  return 1;

//	printf("[u-boot]-sdhci: SD detected.\n");
/******************************\
  FIXME TO enable the IRQ??
\******************************/
	mask|= (IMAP_ERRINTSTS_BUSPOWER | IMAP_ERRINTSTS_TRNENDBIT | 
		IMAP_ERRINTSTS_TRNCRC |IMAP_ERRINTSTS_TRNTOUT |
		IMAP_ERRINTSTS_INDEX |IMAP_ERRINTSTS_ENDBIT |
		IMAP_ERRINTSTS_CRC |IMAP_ERRINTSTS_TOUT |
		IMAP_NORINTSTS_CMDCMP|IMAP_NORINTSTS_TRNCMP|
		IMAP_NORINTSTS_SPACE |IMAP_NORINTSTS_DATA |
		IMAP_NORINTSTS_ERR);
	mask &= ~IMAP_NORINTSTS_CARD;
	imap_mmc_writel(host,IMAP_NORINTSTSEN,mask);
	imap_mmc_writel(host,IMAP_NORINTSIGEN,mask);
	return 0;

}

static int imap_mmc_initialize_host(int channel)
{
	struct mmc *mmc = NULL;
	struct imap_host *host;

	mmc= malloc(sizeof(struct mmc));
	host = malloc(sizeof(struct imap_host));
	if(!mmc || !host)
	  printf("buffer, mmc %08x, host %08x\n", (unsigned int)mmc, (unsigned int)host);
	if(!mmc)
		return -ENOMEM;
	
	printf("MMC: %d\n",channel);
	mmc->send_cmd = imap_request;
	mmc->set_ios = imap_set_ios;
	mmc->init    = imap_init;
	mmc->host_caps = MMC_MODE_4BIT |MMC_MODE_HS;
	switch(channel){
		case 0:
			imap_set_gpio(0,4);
			sprintf(mmc->name,"IMAP_SDHCI0");
			host->base = CONFIG_IMAP_SDI_REGS_BASE; 
			break;
		case 1:
			imap_set_gpio(1,4);
			sprintf(mmc->name,"IMAP_SDHCI1");
			host->base = CONFIG_IMAP_SDI_REGS_BASE + 0x1000; 
			break;
		case 2:
			imap_set_gpio(2,4);
			sprintf(mmc->name,"IMAP_SDHCI2");
			host->base = CONFIG_IMAP_SDI_REGS_BASE + 0x2000; 
			break;
		default:
			printf("invalid channel %d\n",channel);
			goto err_out;
		   break;
	}
	mmc->priv = host;
	host->mmc = mmc;	
	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->f_min = 100000;
	mmc->f_max = 100000000;
	mmc->index = channel;
	mmc->Wflag=1;
	mmc_register(mmc);

	return 0;
err_out:
	printf("Failed to register host mmc.\n");
	if(mmc)
		free(mmc);
	if(host)
		free(host);
	return 1;
}
static void imap_mmc_power_supply(void)
{
	//this will be implemented by platform developer.
#ifdef CONFIG_INAND_GPIO
	unsigned long i;
	unsigned long reg_val, gpio_addr, gpio_num;
	gpio_addr = CONFIG_INAND_GPIO;
	gpio_num  = CONFIG_INAND_GPIO_Bit;

	reg_val = readl(gpio_addr + 4);
	reg_val &= ~(0x3 << (gpio_num * 2));
	reg_val |= 0x1 << (gpio_num * 2);
	writel(reg_val, gpio_addr + 4);

	reg_val = readl(gpio_addr);
	if(reg_val & (0x1<<gpio_num))
	{
		reg_val &= ~(0x1 << gpio_num);
		writel(reg_val, gpio_addr);
		printf("turn off iNand\n");
	}
	for(i = 0; i<100; i++)
	  udelay(1000);

	reg_val = readl(gpio_addr);
	reg_val |= (0x1 << gpio_num);
	writel(reg_val, gpio_addr);
	printf("turn on iNand\n");
#endif
}

static int imap_mmc_initialize(bd_t *bis)
{
	int ret;
	imap_mmc_power_supply();
#ifdef SOURCE_CHANNEL
	ret=imap_mmc_initialize_host(SOURCE_CHANNEL);
#endif
#ifdef iNAND_CHANNEL
	ret=imap_mmc_initialize_host(iNAND_CHANNEL);
#endif
	return 0;
}
int board_mmc_init(bd_t *bis)
{
	return imap_mmc_initialize(bis);
}
