/*
 * A driver for the infoTM 4-wires SSP/SPI bus master
 * Copyright (C) 2016-2100 infotm D.Platsoft
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/spi/spi.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/platform_data/spi-imapx.h>
#include <linux/vmalloc.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include "spi-imapx.h"

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#endif

#define SCATTER_SIZE (PAGE_SIZE*4)

//extern int spi_log_enable;
static int spi_dma_enable = 1;
static int spi_log_enable = 0;
//static int spi_dma_enable = 0;

static void imapx_spi_dump_regs(struct imapx_spi *host) {
	pr_err("SPI_EN(%x)	0x%x\n", SPI_EN, spi_readl(host, SPI_EN));
	pr_err("SPI_CTL(%x)	0x%x\n", SPI_CTL, spi_readl(host, SPI_CTL));
	pr_err("SPI_CFDF(%x)	0x%x\n", SPI_CFDF, spi_readl(host, SPI_CFDF));
	pr_err("SPI_IMSC(%x) 	0x%x\n", SPI_IMSC, spi_readl(host, SPI_IMSC));
	pr_err("SPI_RIS(%x)	0x%x\n", SPI_RIS, spi_readl(host, SPI_RIS));
	pr_err("SPI_MIS(%x)	0x%x\n", SPI_MIS, spi_readl(host, SPI_MIS));
	pr_err("SPI_ICR(%x)	0x%x\n", SPI_ICR, spi_readl(host, SPI_ICR));
	pr_err("SPI_DMACR(%x)	0x%x\n", SPI_DMACR, spi_readl(host, SPI_DMACR));
	pr_err("SPI_FTHD(%x)	0x%x\n", SPI_FTHD, spi_readl(host, SPI_FTHD));
	pr_err("SPI_FSTA(%x)	0x%x\n", SPI_FSTA, spi_readl(host, SPI_FSTA));
	pr_err("SPI_TXTOUT(%x)	0x%x\n", SPI_TXTOUT, spi_readl(host, SPI_TXTOUT));
	pr_err("SPI_TFDAC(%x)	0x%x\n", SPI_TFDAC, spi_readl(host, SPI_TFDAC));
	pr_err("SPI_RFDAC(%x)	0x%x\n", SPI_RFDAC, spi_readl(host, SPI_RFDAC));
	pr_err("SPI_DR0(%x)	0x%x\n", SPI_DR0, spi_readl(host, SPI_DR0));
	pr_err("SPI_DCTL0(%x)	0x%x\n", SPI_DCTL0, spi_readl(host, SPI_DCTL0));
	pr_err("SPI_DR1(%x)	0x%x\n", SPI_DR1, spi_readl(host, SPI_DR1));
	pr_err("SPI_DCTL1(%x)	0x%x\n", SPI_DCTL1, spi_readl(host, SPI_DCTL1));
	pr_err("SPI_DR2(%x)	0x%x\n", SPI_DR2, spi_readl(host, SPI_DR2));
	pr_err("SPI_DCTL2(%x)	0x%x\n", SPI_DCTL2, spi_readl(host, SPI_DCTL2));
	pr_err("SPI_DR3(%x)	0x%x\n", SPI_DR3, spi_readl(host, SPI_DR3));
	pr_err("SPI_DCTL3(%x)	0x%x\n", SPI_DCTL3, spi_readl(host, SPI_DCTL3));
	pr_err("SPI_DR4(%x)	0x%x\n", SPI_DR4, spi_readl(host, SPI_DR4));
	pr_err("SPI_DCTL4(%x)	0x%x\n", SPI_DCTL4, spi_readl(host, SPI_DCTL4));
	pr_err("SPI_FDR(%x)	0x%x\n", SPI_FDR, spi_readl(host, SPI_FDR));
	pr_err("SPI_FDRCTL(%x)	0x%x\n", SPI_FDRCTL, spi_readl(host, SPI_FDRCTL));
	return;
}

#ifdef CONFIG_DEBUG_FS
static ssize_t  imapx_spi_show_regs(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos) {
	struct imapx_spi *host = file->private_data;
	imapx_spi_dump_regs(host);
	return 0;
}

static ssize_t  imapx_spi_show_loglevel(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos) {
	pr_info("[spimul] spi_log_enable - %d\n", spi_log_enable);
	return 0;
}

static ssize_t imapx_spi_store_loglevel(struct file *file, const char __user *user_buf,
		size_t count, loff_t *ppos) {
	char buf[16];
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;
	spi_log_enable = simple_strtoul(buf, NULL, 10);
	return count;
}


static const struct file_operations imapx_spi_regs_ops = {
	.owner          = THIS_MODULE,
	.open           = simple_open,
	.read           = imapx_spi_show_regs,
	.llseek         = default_llseek,
};

static const struct file_operations imapx_spi_log_ops = {
	.owner          = THIS_MODULE,
	.open           = simple_open,
	.read           = imapx_spi_show_loglevel,
	.write          = imapx_spi_store_loglevel,
	.llseek         = default_llseek,
};

static int imapx_spi_debugfs_init(struct imapx_spi *host) {
	host->debugfs = debugfs_create_dir("spi-imapx", NULL);
	if(!host->debugfs)
		return -ENOMEM;
	 debugfs_create_file("registers", S_IFREG | S_IRUGO,
			 host->debugfs, host, &imapx_spi_regs_ops);
	 debugfs_create_file("log", S_IFREG | S_IRUGO | S_IWUGO,
			 host->debugfs, host, &imapx_spi_log_ops);
	 return 0;
}

static void imapx_spi_debugfs_remove(struct imapx_spi *host) {
	if(host->debugfs)
		debugfs_remove_recursive(host->debugfs);
}
#else
static inline int imapx_spi_debugfs_init(struct imapx_spi *host) {
	return 0;
}

static void imapx_spi_debugfs_remove(struct imapx_spi *host) {
}
#endif


static void imapx_spi_hwinit(struct imapx_spi *host) { 
	int i;
	/*Disble SPI Host Controller*/
	spi_writel(host, SPI_EN, 0); 
	/*Clear all irq state*/
	spi_writel(host, SPI_ICR, 0x7); 
	/*Disable TX&RX DMA function*/
	spi_writel(host, SPI_DMACR, 0); 
	/*Disable all data port*/
	for(i = 0; i < 4; i++){ 
		spi_writel(host, SPI_DCTL(i), 0); 
		spi_writel(host, SPI_DR(i), 0); 
	}

	/*Disable all data port*/
	spi_writel(host, SPI_FDRCTL, 0); 

	/*config tx fifo timeout clkcnt*/
	spi_writel(host, SPI_TXTOUT, 0xffffffff); 

	/*set tx & rx fifo watermark*/
	spi_writel(host, SPI_FTHD, (32 << 8) | 32);
}

static void imapx_spi_caculate_clkdiv(struct imapx_spi *host, struct chip_data *chip) {
	int remainder = -1;
	int divisor = -1;
	int target_speed = chip->speed_hz;

	if( chip->speed_hz <= 0) {
		pr_err("[spimul] cannot caculate clk divder.\n");
		chip->clk_div = -1; 
		return;
	}
	divisor = host->extclk_rate / chip->speed_hz;
	remainder = host->extclk_rate % chip->speed_hz;

	if(divisor < host->min_clk_div)
		divisor = host->min_clk_div;
	else if(divisor > host->max_clk_div)
		divisor = host->max_clk_div;
	else if(remainder)
		divisor += (divisor % 2) ? 1 : 2;
	else
		divisor += (divisor % 2) ? 1 : 0;
	chip->speed_hz = host->extclk_rate / divisor;
	chip->clk_div = divisor;
	host->cur_speed = chip->speed_hz;
	if(spi_log_enable)
		pr_info("[spimul] srcclk(%dHz), target(%dHz), actual(%dHz), clkdiv(%d).\n",
			host->extclk_rate, target_speed, chip->speed_hz, chip->clk_div);
	return;
}

#if 0
static void imapx_spi_tx_buffer_LEtoBE(u32 *src, u32 *dst, int len) {
	int i;
	u8 *p = src;
	for(i = 0; i < len; i++) {
		*dst = (*dst << 8) | *p;
		p++;
	}
	return;

}
#endif

static void imapx_spi_rx_buffer_LEtoBE(u32 *src, u32 *dst, int nbytes, int len) {
	int i ;
	u8 *p = (u8 *)src + nbytes - 1;
	*dst = 0;
	for(i = 0; i < len; i++) {
		*dst = *dst | (*p << (i * 8));
		p--;
	}
	return;
}

static void imapx_spi_irq_mask(struct imapx_spi *host, int mask_en, int irq_mask) {
	u32 int_mask = spi_readl(host, SPI_IMSC);
	if(mask_en) {
		int_mask &= ~(irq_mask);
	} else {
		int_mask |= irq_mask;
	}
	//pr_info("[spimul] irq mask enable :%d, irqbit: %x, int_mask %x\n", mask_en, irq_mask, int_mask);
	spi_writel(host, SPI_IMSC, int_mask);
}


static void imapx_spi_chip_select(struct imapx_spi *host, int select) {
	//pr_info("[spimul] %s, select-%d\n", __func__, select);
	host->cur_chip->cs_control(!!select);
	return;
}

static void imapx_spi_flush_fifo(struct imapx_spi *host) {
	unsigned long limit = loops_per_jiffy << 1;
	//pr_info("[spimul] %s\n", __func__);
	do {
		while(!(spi_readl(host, SPI_FSTA) & (0x1 << 3)))
			spi_readl(host, SPI_FDR);
	} while((spi_readl(host, SPI_FSTA) & (0x1 << 4)) && limit--);
}

static void imapx_spi_wait_leisure(struct imapx_spi *host) {
	unsigned long limit = loops_per_jiffy << 1;
	while((spi_readl(host, SPI_FSTA) & (0x1 << 4)) && limit--);
}

static void imapx_spi_enable(struct imapx_spi *host, int enable) {
	if(spi_log_enable) {
		pr_info("[spimul] %s, enbale-%d\n", __func__, enable);
		pr_info("\n");
	}
	spi_writel(host, SPI_EN, enable);
}

#ifdef CONFIG_DMA_ENGINE
static int imapx_spi_caculate_current_len(struct imapx_spi *host) {
	struct spi_transfer *transfer = host->cur_transfer;
	unsigned len = 0;
	if(transfer->rx_buf && (host->quirk & SPIMUL_QUIRK_RXLEN_ALLIGN_8BYTES))
		len = roundup(transfer->len, 8);
	else 
		len = transfer->len;
	//pr_info("[spimul] transfer-len-%d, host rx len %d\n", transfer->len, len);
	return len;
}

static int imapx_spi_setup_dma_scatter(struct imapx_spi *host, void *buffer,
					unsigned int length, int extra_len, struct sg_table *sgt) {

	void *bufp = buffer;
	dma_addr_t dma_addr = host->buffer_dma_addr;
	struct page *vm_page;
	int bytesleft = length;
	int mapbytes;
	int i;

	BUG_ON(!bufp);

	if(spi_log_enable)
		pr_info("[spimul] length -%d,  extra_len -%ld\n", length, extra_len);
	for(i = 0; i < sgt->nents ; i++) {

		if((i == sgt->nents - 1) && extra_len) {
			mapbytes = extra_len;
			bufp = host->buffer; 
		} else {

			if (bytesleft < (SCATTER_SIZE - offset_in_page(bufp)))
				mapbytes = bytesleft;
			else
				mapbytes = SCATTER_SIZE - offset_in_page(bufp);
		}
		if(spi_log_enable)
			pr_info("[spimul] sg[%d] mapbytes -%d, offset -%ld\n", i, mapbytes, offset_in_page(bufp));

		if(is_vmalloc_addr(bufp)) {
			vm_page = vmalloc_to_page(bufp);
			if (!vm_page) {
				sg_free_table(sgt);
				pr_err("[spimul] vm_page is null\n");
				return -ENOMEM;
			}
			sg_set_page(&sgt->sgl[i], vm_page, mapbytes, offset_in_page(bufp));
		} else {
			if((i == sgt->nents - 1) && extra_len) {
				sg_set_page(&sgt->sgl[i], phys_to_page(dma_addr),
						mapbytes, 0);
			} else {
				sg_set_page(&sgt->sgl[i], virt_to_page(bufp), 
						mapbytes, offset_in_page(bufp));
			}
		}
		if((i != sgt->nents - 1) || !extra_len) { 
			bufp += mapbytes;
			bytesleft -= mapbytes;
		}

	}
	BUG_ON(bytesleft);
	return 0;
}

static void imapx_spi_dma_callback(void *data) {
	struct imapx_spi *host = data;
	struct spi_message *msg = host->message;
	struct spi_transfer *transfer = host->cur_transfer;

	host->dma_running = false;
	if(transfer->rx_buf) {
		if(spi_log_enable)
			pr_info("[spimul] rx dma_callback---\n");
		dma_unmap_sg(host->dma_rx_channel->device->dev, host->sgt_rx.sgl,
				host->sgt_rx.nents, DMA_FROM_DEVICE);
		sg_free_table(&host->sgt_rx); 
		if(host->cur_rx_len > SCATTER_SIZE) {
			if(host->cur_rx_len != transfer->len)
				memcpy((u8*)transfer->rx_buf + rounddown(transfer->len, 8) , 
					host->buffer, transfer->len - rounddown(transfer->len, 8)); 
		} else {
			memcpy(transfer->rx_buf, host->buffer, transfer->len); 
		}
#if 0
		print_hex_dump(KERN_ERR, "HostBuffer: ",
				       DUMP_PREFIX_OFFSET,
				       16,
				       1,
				       host->buffer,
				       transfer->len,
				       1);
#endif

	} else if (transfer->tx_buf) {
		if(spi_log_enable)
			pr_info("[spimul] tx dma_callback---\n");
		dma_unmap_sg(host->dma_tx_channel->device->dev, host->sgt_tx.sgl,
				host->sgt_tx.nents, DMA_TO_DEVICE);
		sg_free_table(&host->sgt_tx); 
	}
	msg->actual_length += transfer->len;
	msg->status = 0;
	msg->state = STATE_DONE;
	imapx_spi_wait_leisure(host);
	imapx_spi_chip_select(host, SPI_CS_DESELECT);
	spi_writel(host, SPI_DCTL0, 0);
	spi_writel(host, SPI_DCTL1, 0);
	spi_writel(host, SPI_DCTL2, 0);
	spi_writel(host, SPI_DCTL3, 0);
	spi_writel(host, SPI_DCTL4, 0);
	spi_writel(host, SPI_FDRCTL, 0);
	imapx_spi_enable(host, 0);
	spi_finalize_current_message(host->master);
}


static void imapx_spi_config_dma(struct imapx_spi *host) {
	u32 dmacr = 0x3;
	u32 rx_thd = 0;
	u32 tx_thd = 0;

	/*enable rx dma */
	switch(host->rx_lev_trig) {
		case SPIMUL_RX_1_OR_MORE_ELEM:
			rx_thd |= 1;
			break;
		case SPIMUL_RX_4_OR_MORE_ELEM:
			rx_thd |= 4;
			break;
		case SPIMUL_RX_8_OR_MORE_ELEM:
			rx_thd |= 8;
			break;
		case SPIMUL_RX_16_OR_MORE_ELEM:
			rx_thd |= 16;
			break;
		case SPIMUL_RX_32_OR_MORE_ELEM:
			rx_thd |= 32;
			break;
		default:
			rx_thd |= 4;
			break;
	}

	/* enable tx dma*/
	switch(host->tx_lev_trig) {
		case SPIMUL_TX_1_OR_MORE_EMPTY_LOC:
			tx_thd |= 1;
			break;
		case SPIMUL_TX_4_OR_MORE_EMPTY_LOC:
			tx_thd |= 4;
			break;
		case SPIMUL_TX_8_OR_MORE_EMPTY_LOC:
			tx_thd |= 8;
			break;
		case SPIMUL_TX_16_OR_MORE_EMPTY_LOC:
			tx_thd |= 16;
			break;
		case SPIMUL_TX_32_OR_MORE_EMPTY_LOC:
			tx_thd |= 32;
			break;
		default:
			tx_thd |= 4;
			break;
	}

	spi_writel(host, SPI_DMACR, dmacr);
	spi_writel(host, SPI_FTHD, (rx_thd << 8) | tx_thd);
	if(spi_log_enable)
		pr_info("[spimul] dmacr-^%x, fifo_thd -%x\n", dmacr, (rx_thd << 8) | tx_thd);
	/* disable all irq in dma mode */
	imapx_spi_irq_mask(host, 1, SPI_INT_MASK_ALL);
}

static int imapx_spi_configure_dma(struct imapx_spi *host) {
	struct spi_transfer *transfer = host->cur_transfer;
	void *tx = (void *)transfer->tx_buf;
	void *rx = (void *)transfer->rx_buf;
	int rx_sglen, tx_sglen;
	int sgs;
	int status;

	if(transfer->rx_buf) {
		struct dma_chan *rxchan = host->dma_rx_channel;
		struct dma_async_tx_descriptor *rxdesc;
		struct dma_slave_config rx_conf = {
			.src_addr = host->phy_base + SPI_FDR,
			.direction = DMA_DEV_TO_MEM,
			.device_fc = false,
		};

		if(!rxchan)
			return -ENODEV;
		switch(host->rx_lev_trig) {
			case SPIMUL_RX_1_OR_MORE_ELEM:
				rx_conf.src_maxburst = 1;
				break;
			case SPIMUL_RX_4_OR_MORE_ELEM:
				rx_conf.src_maxburst = 4;
				break;
			case SPIMUL_RX_8_OR_MORE_ELEM:
				rx_conf.src_maxburst = 8;
				break;
			case SPIMUL_RX_16_OR_MORE_ELEM:
				rx_conf.src_maxburst = 16;
				break;
			case SPIMUL_RX_32_OR_MORE_ELEM:
				rx_conf.src_maxburst = 32;
				break;
			default:
				rx_conf.src_maxburst = 4;
				break;
		}
		switch(host->rx_bitwidth) {
			case  BIT_WIDTH_U8:
				rx_conf.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
				break;
			case  BIT_WIDTH_U16:
				rx_conf.src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
				break;
			case  BIT_WIDTH_U32:
				rx_conf.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
				break;
			default:
				pr_info("[spimul] unsupport bits width for read, set it to 1 byte\n");
				rx_conf.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
				break;
		}

		if(host->quirk & SPIMUL_QUIRK_DMA_BYTESWAP) {
			switch(host->rx_bitwidth) {
			case  BIT_WIDTH_U8:
				rx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;
				break;
			case  BIT_WIDTH_U16:
				rx_conf.byteswap = DMA_SLAVE_BYTESWAP_16;
				break;
			case  BIT_WIDTH_U32:
				rx_conf.byteswap = DMA_SLAVE_BYTESWAP_32;
				break;
			default:
				rx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;
				break;
			}
		} else
			rx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;

		if(spi_log_enable)
			pr_info("[spimul] set rx byte_swap_mode to %d, src_addr_width = %d, src_maxburst = %d\n",rx_conf.byteswap, rx_conf.src_addr_width, rx_conf.src_maxburst);

		dmaengine_slave_config(rxchan, &rx_conf);
#if 0
		if(vmalloced_buf)
			sgs = DIV_ROUND_UP(transfer->len + offset_in_page(transfer->rx_buf), SCATTER_SIZE);
		else
			sgs = DIV_ROUND_UP(transfer->len, SCATTER_SIZE);
#endif
		if(host->cur_rx_len > SCATTER_SIZE) {
			sgs = DIV_ROUND_UP(transfer->len + offset_in_page(transfer->rx_buf), SCATTER_SIZE);
			if(host->cur_rx_len != transfer->len)
				sgs += 1;
		} else
			sgs = 1;
		/*
		 *  We use one page
		 */

		if(spi_log_enable)
			pr_info("[spimul] using %d sgs for rx transfer len %d\n", sgs, transfer->len);
		status = sg_alloc_table(&host->sgt_rx, sgs, GFP_ATOMIC);
		if(status)
			goto err_alloc_rx_sg;
		if(host->cur_rx_len > SCATTER_SIZE)
			imapx_spi_setup_dma_scatter(host, rx, rounddown(transfer->len, 8), 
					host->cur_rx_len - rounddown(transfer->len, 8), &host->sgt_rx);
		else
			imapx_spi_setup_dma_scatter(host, host->buffer, host->cur_rx_len, 0, &host->sgt_rx);


		rx_sglen = dma_map_sg(rxchan->device->dev, host->sgt_rx.sgl,
					host->sgt_rx.nents, DMA_FROM_DEVICE);
		if (!rx_sglen)
			goto err_rx_sgmap;
		rxdesc = dmaengine_prep_slave_sg(rxchan, host->sgt_rx.sgl,
					rx_sglen, DMA_DEV_TO_MEM,
					DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
		if (!rxdesc)
			goto err_rxdesc;
		rxdesc->callback = imapx_spi_dma_callback;
		rxdesc->callback_param = host;
		dmaengine_submit(rxdesc);

		return 0;
err_rxdesc:
		dmaengine_terminate_all(rxchan);
		dma_unmap_sg(rxchan->device->dev, host->sgt_rx.sgl,
				host->sgt_rx.nents, DMA_FROM_DEVICE);
err_rx_sgmap:
		sg_free_table(&host->sgt_tx);
err_alloc_rx_sg:
		return -ENOMEM;

	} else if(transfer->tx_buf) {
		struct dma_chan *txchan = host->dma_tx_channel;
		struct dma_async_tx_descriptor *txdesc;
		struct dma_slave_config tx_conf = {
			.dst_addr = host->phy_base + SPI_FDR,
			.direction = DMA_MEM_TO_DEV,
			.device_fc = false,
		};
		if(!txchan)
			return -ENODEV;
		switch(host->tx_lev_trig) {
			case SPIMUL_TX_1_OR_MORE_EMPTY_LOC:
				tx_conf.dst_maxburst = 1;
				break;
			case SPIMUL_TX_4_OR_MORE_EMPTY_LOC:
				tx_conf.dst_maxburst = 4;
				break;
			case SPIMUL_TX_8_OR_MORE_EMPTY_LOC:
				tx_conf.dst_maxburst = 8;
				break;
			case SPIMUL_TX_16_OR_MORE_EMPTY_LOC:
				tx_conf.dst_maxburst = 16;
				break;
			case SPIMUL_TX_32_OR_MORE_EMPTY_LOC:
				tx_conf.dst_maxburst = 32;
				break;
			default:
				tx_conf.dst_maxburst = 4;
				break;
		}
		switch(host->tx_bitwidth) {
			case  BIT_WIDTH_U8:
				tx_conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
				break;
			case  BIT_WIDTH_U16:
				tx_conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
				break;
			case  BIT_WIDTH_U32:
				tx_conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
				break;
			default:
				pr_info("[spimul] unsupport bits width for tx, set it to 1 byte\n");
				tx_conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
				break;
		}

		if(host->quirk & SPIMUL_QUIRK_DMA_BYTESWAP) {
			switch(host->tx_bitwidth) {
			case  BIT_WIDTH_U8:
				tx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;
				break;
			case  BIT_WIDTH_U16:
				tx_conf.byteswap = DMA_SLAVE_BYTESWAP_16;
				break;
			case  BIT_WIDTH_U32:
				tx_conf.byteswap = DMA_SLAVE_BYTESWAP_32;
				break;
			default:
				tx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;
				break;
			}
		} else
			tx_conf.byteswap = DMA_SLAVE_BYTESWAP_NO;

		if(spi_log_enable)
			pr_info("[spimul] set tx byte_swap_mode to %d, dst_addr_width = %d, dst_maxburst = %d\n",tx_conf.byteswap, tx_conf.dst_addr_width, tx_conf.dst_maxburst);

		dmaengine_slave_config(txchan, &tx_conf);

		sgs = DIV_ROUND_UP(transfer->len + offset_in_page(transfer->tx_buf), SCATTER_SIZE);
		if(spi_log_enable)
			pr_info("[spimul] using %d sgs for tx transfer len %d\n", sgs, transfer->len);

		status = sg_alloc_table(&host->sgt_tx, sgs, GFP_ATOMIC);
		if(status)
			goto err_alloc_tx_sg;
		imapx_spi_setup_dma_scatter(host, tx, transfer->len, 0, &host->sgt_tx);
		tx_sglen = dma_map_sg(txchan->device->dev, host->sgt_tx.sgl,
					host->sgt_tx.nents, DMA_TO_DEVICE);
		if (!tx_sglen)
			goto err_tx_sgmap;
		txdesc = dmaengine_prep_slave_sg(txchan, host->sgt_tx.sgl,
					tx_sglen, DMA_MEM_TO_DEV,
					DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
		if (!txdesc)
			goto err_txdesc;
		txdesc->callback = imapx_spi_dma_callback;
		txdesc->callback_param = host;
		dmaengine_submit(txdesc);
		return 0;
err_txdesc:
		dmaengine_terminate_all(txchan);
		dma_unmap_sg(txchan->device->dev, host->sgt_tx.sgl,
				host->sgt_tx.nents, DMA_TO_DEVICE);
err_tx_sgmap:
		sg_free_table(&host->sgt_tx);
err_alloc_tx_sg:
		return -ENOMEM;
	}
	return -ENODEV;
}

static int imapx_spi_start_dma(struct imapx_spi *host) {
	struct spi_transfer *transfer = host->cur_transfer;
	if(transfer->rx_buf) {
		dma_async_issue_pending(host->dma_rx_channel);
		host->dma_running = true;
	}
	if(transfer->tx_buf) {
		dma_async_issue_pending(host->dma_tx_channel);
		host->dma_running = true;
	}
	return 0;
}

static int imapx_spi_dma_probe(struct imapx_spi *host) {

	dma_cap_mask_t mask;
	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);
	host->dma_rx_channel = dma_request_channel(mask,
			host->master_info->dma_filter,
			host->master_info->dma_rx_param);
	if(!host->dma_rx_channel) {
		pr_err("[spimul] no RX DMA channel!\n");
		goto err_no_rxchan;
	}

	host->dma_tx_channel = dma_request_channel(mask,
			host->master_info->dma_filter,
			host->master_info->dma_tx_param);
	if(!host->dma_tx_channel) {
		pr_err("[spimul] no TX DMA channel!\n");
		goto err_no_txchan;
	}


	host->buffer = dma_alloc_coherent(host->dev,
			SCATTER_SIZE, &host->buffer_dma_addr, GFP_KERNEL);
	if (!host->buffer) { 
		pr_err("[spimul] can not alloc dma datapage!\n");
		goto err_no_dummypage;
	}
	pr_info("[spimul] alloc host DMA page 0x%p, 0x%x\n", host->buffer, host->buffer_dma_addr);
	pr_info("[spimul] setup for DMA on RX %s, TX %s\n",
			dma_chan_name(host->dma_rx_channel),
			dma_chan_name(host->dma_tx_channel));
	return 0;
err_no_dummypage:
	dma_release_channel(host->dma_tx_channel);
err_no_txchan:
	dma_release_channel(host->dma_rx_channel);
	host->dma_rx_channel = NULL;
err_no_rxchan:
	pr_err("[spimul] Failed to work in dma mode, work in polling mode!\n");

	return -ENODEV;
}

static void imapx_spi_terminate_dma(struct imapx_spi *host) {
	struct dma_chan *rxchan = host->dma_rx_channel;
	struct dma_chan *txchan = host->dma_tx_channel;
	dmaengine_terminate_all(rxchan);
	dmaengine_terminate_all(txchan);
	host->dma_running = false;
}

static void imapx_spi_dma_remove(struct imapx_spi *host) {
	 if (host->dma_running)
		 imapx_spi_terminate_dma(host);
	 if (host->dma_tx_channel)
		 dma_release_channel(host->dma_tx_channel);
	 if (host->dma_rx_channel)
		 dma_release_channel(host->dma_rx_channel);

	 dma_free_coherent(host->dev, SCATTER_SIZE, host->buffer,
			 	host->buffer_dma_addr);
}

#else
static inline int imapx_spi_configure_dma(struct imapx_spi *host) {
	return -ENODEV;
}

static inline int imapx_spi_dma_probe(struct imapx_spi *host) {
	return 0;
}

static inline void imapx_spi_dma_remove(struct imapx_spi *host) {
}
#endif

static int imapx_spi_is_message_valid(struct spi_message *message) {
	/*
	 * We only support 1-2 half-duplex transfer in a single message
	 *  transfer[0]: a, only for tx 
	 *                b, tx length must be less than 17 bytes according to spimul
	 *  transfer[1]: a, can be tx or rx 
	 * Acocording to SPIMUL, transfer 0 & 1 are bounded as a single read/write for flash.
	 * So, The transfers can not be excuted untill all Hardware parameter prepared
	 */


	 
	return 0;
}

static int imapx_spi_get_transfer_nbits(struct spi_transfer *transfer) {
	int wires = 0;
	if(transfer->tx_buf) {
		switch(transfer->tx_nbits) {
			case SPI_NBITS_DUAL:
				wires = 1;
				break;
			case SPI_NBITS_QUAD:
				wires = 2;
				break;
			default:
				wires = 0;
				break;
		}
	} else if(transfer->rx_buf) {
		switch(transfer->rx_nbits) {
			case SPI_NBITS_DUAL:
				wires = 1;
				break;
			case SPI_NBITS_QUAD:
				wires = 2;
				break;
			default:
				wires = 0;
				break;
		}
	}
	return wires;
}

static int imapx_spi_config_dp0to4(struct imapx_spi *host, struct spi_transfer *transfer, unsigned int cnt) {
	int i, xlen, wires;
	void *tx; 
	uint32_t *dr;
	uint32_t *dctl;
	int *enable;

	if(!transfer) {
		pr_err("[spimul] transfer is NULL\n");
		return -EINVAL;
	}

	if(!transfer->tx_buf || transfer->rx_buf) {
		pr_err("[spimul] first transfer must only be tx\n");
		pr_err("[spimul] transfer->tx_buf = 0x%x, transfer->rx_buf = 0x%x\n", transfer->tx_buf, transfer->rx_buf);
		return -EINVAL;
	}

	if(transfer->len > 4) {
		pr_err("[spimul] every transfer can only tx less than 4\n");
		return -EINVAL;
	}

	if(cnt == 0 && transfer->len > 1) {
		pr_err("[spimul] first tx transfer(CMD) couldn't be more than 1\n");
		return -EINVAL;
	}

	wires = imapx_spi_get_transfer_nbits(transfer);
	host->cur_wire = wires;
	if(spi_log_enable) {
		pr_info("[spimul] tx transfer len = %d, wires = %d\n", transfer->len, wires);
	}

	tx = (u8*)transfer->tx_buf;

	switch(cnt) {
		case 0:
			dr = &(host->dp0.dr);
			dctl = &(host->dp0.dctl);
			enable = &(host->dp0.enable);
			break;
		case 1:
			dr = &(host->dp1.dr);
			dctl = &(host->dp1.dctl);
			enable = &(host->dp1.enable);
			break;
		case 2:
			dr = &(host->dp2.dr);
			dctl = &(host->dp2.dctl);
			enable = &(host->dp2.enable);
			break;
		case 3:
			dr = &(host->dp3.dr);
			dctl = &(host->dp3.dctl);
			enable = &(host->dp3.enable);
			break;
		case 4:
			dr = &(host->dp4.dr);
			dctl = &(host->dp4.dctl);
			enable = &(host->dp4.enable);
			break;

		default:
			pr_err("[spimul] tx transfer counter excess the maximum\n");
			return -EINVAL;
	}

	xlen = transfer->len;
	for(i = 0; i < xlen; i++) {
		if(spi_log_enable)
			pr_info("[spimul] CMD(%d)-%x\n", i, *(u8 *)tx);
		*dr = (*dr << 8) | *(u8 *)tx;
		tx++;
	}
	if(spi_log_enable)
		pr_info("*dr = 0x%x\n", *dr);

	*dctl = ((xlen > 0) ? ((8 * xlen - 1) << 8 | 1) : 0) | (wires << 4);
	if(spi_log_enable)
		pr_info("*dctl = 0x%x\n", *dctl);

	if(*dctl & 1) *enable = 1; else *enable = 0;

	return 0;
}

static void imapx_spi_update_dp0to4(struct imapx_spi *host) {

	if(host->dp0.enable) {
		spi_writel(host, SPI_DCTL0, host->dp0.dctl);
		spi_writel(host, SPI_DR0, host->dp0.dr);
		if(spi_log_enable) {
			pr_info("host->dp0.dctl=0x%x\n", host->dp0.dctl);
			pr_info("host->dp0.dr=0x%x\n", host->dp0.dr);
		}
		host->dp0.dctl = 0;
		host->dp0.dr = 0;
		host->dp0.enable = 0;
	}
	if(host->dp1.enable) {
		spi_writel(host, SPI_DCTL1, host->dp1.dctl);
		spi_writel(host, SPI_DR1, host->dp1.dr);
		if(spi_log_enable) {
			pr_info("host->dp1.dctl=0x%x\n", host->dp1.dctl);
			pr_info("host->dp1.dr=0x%x\n", host->dp1.dr);
		}
		host->dp1.dctl = 0;
		host->dp1.dr = 0;
		host->dp1.enable = 0;
	}
	if(host->dp2.enable) {
		spi_writel(host, SPI_DCTL2, host->dp2.dctl);
		spi_writel(host, SPI_DR2, host->dp2.dr);
		if(spi_log_enable) {
			pr_info("host->dp2.dctl=0x%x\n", host->dp2.dctl);
			pr_info("host->dp2.dr=0x%x\n", host->dp2.dr);
		}
		host->dp2.dctl = 0;
		host->dp2.dr = 0;
		host->dp2.enable = 0;
	}
	if(host->dp3.enable) {
		spi_writel(host, SPI_DCTL3, host->dp3.dctl);
		spi_writel(host, SPI_DR3, host->dp3.dr);
		if(spi_log_enable){
			pr_info("host->dp3.dctl=0x%x\n", host->dp3.dctl);
			pr_info("host->dp3.dr=0x%x\n", host->dp3.dr);
		}
		host->dp3.dctl = 0;
		host->dp3.dr = 0;
		host->dp3.enable = 0;
	}
	if(host->dp4.enable) {
		spi_writel(host, SPI_DCTL4, host->dp4.dctl);
		spi_writel(host, SPI_DR4, host->dp4.dr);
		if(spi_log_enable){
			pr_info("host->dp4.dctl=0x%x\n", host->dp4.dctl);
			pr_info("host->dp4.dr=0x%x\n", host->dp4.dr);
		}
		host->dp4.dctl = 0;
		host->dp4.dr = 0;
		host->dp4.enable = 0;
	}
	return;
}

static void imapx_spi_config_update_fdp(struct imapx_spi *host) {
	struct spi_transfer *transfer = host->cur_transfer;
	int direction, wires, nfifo;
	int bitswidth = host->cur_chip->n_bytes * 8;  

	if(transfer->rx_buf){ 
		direction = 1;
		if(host->rx_bitwidth == BIT_WIDTH_U32)
			bitswidth = 32;
		nfifo = DIV_ROUND_UP(host->cur_rx_len, bitswidth / 8);
	}
	else if(transfer->tx_buf){ 
		direction = 0;
		if(host->tx_bitwidth == BIT_WIDTH_U32)
			bitswidth = 32;
		nfifo = DIV_ROUND_UP(transfer->len, bitswidth / 8);
	}

	wires = imapx_spi_get_transfer_nbits(transfer);
	host->cur_wire = wires;

	if(nfifo > 0) {
		host->fdp.dctl = ((nfifo - 1) << 16) | ((bitswidth - 1) << 8) | (wires << 4) | ( direction << 1) | 1;
		host->fdp.enable = 1;
		if(spi_log_enable)
			pr_info("[spimul] update fdctl-0x%x\n", host->fdp.dctl);
		spi_writel(host, SPI_FDRCTL, host->fdp.dctl);
	}
	return;
}

static unsigned long imapx_spi_caculate_timeout(struct imapx_spi *host) {
	struct spi_transfer *transfer = host->cur_transfer;
	unsigned long timeout;
	/*
	 * We caculate a very rough timeout(ms) value for current transfer
	 *  Assume ext speed 10MHZ, wires = 1; 
	 */

	timeout = transfer->len / 1250; 

	/*
	 * 400ms increment for safe 
	 */
	timeout += 400;
	return timeout;
}

static int imapx_spi_poll_write_timeout(struct imapx_spi *host, unsigned long t) {
	struct spi_transfer *transfer = host->cur_transfer;
	unsigned long time, timeout =  jiffies + msecs_to_jiffies(t);
	void *tx = (void *)transfer->tx_buf;
	int status = 0;
	int len = 0;

	while(len < transfer->len) {
		while(!(spi_readl(host, SPI_FSTA) & 0x1) && len  < transfer->len) {
			//pr_err("*tx = 0x%x\n", *(u8*)tx);
			spi_writel(host, SPI_FDR, *(u8 *)tx);
			tx++;
			len++;
		}

		time = jiffies;
		if (time_after(time, timeout)){
			status = -1;
			break;
		}
		cpu_relax();

	}
	return status;
}

static int imapx_spi_poll_read_timeout(struct imapx_spi *host, unsigned long t) {
	struct spi_transfer *transfer = host->cur_transfer;
	unsigned long time, timeout =  jiffies + msecs_to_jiffies(t);
	void *rx = transfer->rx_buf;
	int status = 0;
	uint32_t tmp;
	int len = 0, fifo_cnt, nbytes, cur_len;

	if(host->rx_bitwidth == BIT_WIDTH_U32)
		nbytes = 4;
	else
		nbytes = 1;

	while(len < transfer->len) {
		while(spi_readl(host, SPI_RFDAC) > 0 && len < transfer->len) {
			fifo_cnt = spi_readl(host, SPI_RFDAC );
			if(fifo_cnt >= 64) 
				pr_info("[spimul] Poll read error: RX FIFO Overrun occured\n");

			cur_len = (transfer->len - len) >= nbytes ? nbytes : (transfer->len - len);
			tmp = spi_readl(host, SPI_FDR);
			imapx_spi_rx_buffer_LEtoBE(&tmp, (u32 *)rx, nbytes, cur_len);
			len += cur_len;
			rx += cur_len;
		}
		time = jiffies;
		if (time_after(time, timeout)) {
			//imapx_spi_dump_regs(host);
			status = -1;
			break;
		}
		cpu_relax();
	}
	return status;
}
/*
 *  spi dma mode can only partly support on spimul.
 *  for the first transfer of the the message,we just use cpu mode.
 *  since spumul is a IP only for spi nor-flash or nand-flash.
 *  which is incompatiable with a general SPI Host Controller
 */

static int imapx_spi_dma_transfer(struct imapx_spi *host) {
	struct spi_message *message = host->message;
	struct spi_transfer *transfer, *tmp_trans;
	unsigned long cur_len;
	//struct chip_data *chip = host->cur_chip;
	int status;
	unsigned int tx_cnt = 0;
	/*
	 * step 1: check  if the message  is valid or not 
	 */
	if(imapx_spi_is_message_valid(message)) {
		message->state = STATE_ERROR;
		goto out;
	}
	message->state = STATE_START;

	/* disable all irq in dma mode */
	imapx_spi_hwinit(host);
	imapx_spi_flush_fifo(host);
	if (!host->next_msg_cs_active) {                     
		//pr_info("cs ------\n");
		imapx_spi_chip_select(host, SPI_CS_SELECT);
	}
	/* the first transfer of the message */
	list_for_each_entry_safe(transfer, tmp_trans, &message->transfers, transfer_list) {
		int first = 0, last = 0;
		if (&transfer->transfer_list == message->transfers.next)
			first = 1;
		if (&transfer->transfer_list == message->transfers.prev)
			last = 1;

		host->cur_transfer = transfer;

		if(transfer->rx_buf && transfer->tx_buf) {
			pr_err("[spimul] host controller is half duplex\n");
			return -EINVAL;
		}
		if(!last && transfer->rx_buf) {
			pr_err("[spimul] acording to SPIMUL, only last transfer of message can be configured to read\n");
			message->state = STATE_ERROR;
			goto out;
		}
		if(transfer->tx_buf && tx_cnt > 6) {
			pr_err("[spimul] according to SPIMUL, no more than 6 tx transfer could be sent out in one message\n");
			message->state = STATE_ERROR;
			goto out;
		}

		host->cur_rx_len = cur_len = imapx_spi_caculate_current_len(host);
		if(cur_len % 64 == 0) {
			host->rx_lev_trig = SPIMUL_RX_16_OR_MORE_ELEM;
			host->tx_lev_trig = SPIMUL_TX_16_OR_MORE_EMPTY_LOC;
		} else if(cur_len % 32 == 0) {
			host->rx_lev_trig = SPIMUL_RX_8_OR_MORE_ELEM;
			host->tx_lev_trig = SPIMUL_TX_8_OR_MORE_EMPTY_LOC;
		} else if(cur_len % 16 == 0) {
			host->rx_lev_trig = SPIMUL_RX_4_OR_MORE_ELEM;
			host->tx_lev_trig = SPIMUL_TX_4_OR_MORE_EMPTY_LOC;
		} else {
			host->rx_lev_trig = SPIMUL_RX_1_OR_MORE_ELEM;
			host->tx_lev_trig = SPIMUL_TX_1_OR_MORE_EMPTY_LOC;
		}

		imapx_spi_config_dma(host);

		if(transfer->tx_buf) {

			if(first == 1 && last == 1) {
				if(spi_log_enable)
					pr_info("[spimul] dma tx first&last!, tx_cnt = %d\n", tx_cnt);

				status = imapx_spi_config_dp0to4(host, transfer, tx_cnt);
				if(status){
					pr_err("[spimul] transfer config initial error!\n");
					goto out;
				}

				imapx_spi_update_dp0to4(host);
				imapx_spi_enable(host, 1);
				imapx_spi_wait_leisure(host);
				message->state = STATE_DONE;
			} else if(last != 1) {
				if(spi_log_enable)
					pr_info("[spimul] dma tx !last, tx_cnt = %d\n", tx_cnt);
				status = imapx_spi_config_dp0to4(host, transfer, tx_cnt);
				if(status){
					pr_err("[spimul] transfer config initial error!\n");
				}
			} else if(last == 1) {
				if(spi_log_enable)
					pr_info("[spimul] dma tx last, tx_cnt = %d\n", tx_cnt);
				/*to write*/	
				imapx_spi_configure_dma(host);
				imapx_spi_start_dma(host);
				imapx_spi_update_dp0to4(host);
				imapx_spi_config_update_fdp(host);
				imapx_spi_enable(host, 1);
				message->state = STATE_RUNNING;
			}
			tx_cnt++;
		}

		if(transfer->rx_buf) {

			if(spi_log_enable)
				pr_info("[spimul] dma rx\n");

			/*to read*/
			if(first == 1 || !last) {
				pr_err("[spimul] invalid rx transfer\n");
				message->state = STATE_ERROR;
				goto out;
			}
			imapx_spi_configure_dma(host);
			imapx_spi_start_dma(host);
			imapx_spi_update_dp0to4(host);
			imapx_spi_config_update_fdp(host);
			imapx_spi_enable(host, 1);
			message->state = STATE_RUNNING;
		}

		/*Only The 1st transfer need send by cpu */
		if(!last)
			message->actual_length += transfer->len;
	}
out:
	if(message->state == STATE_DONE) {
		spi_writel(host, SPI_DCTL0, 0);
		spi_writel(host, SPI_DCTL1, 0);
		spi_writel(host, SPI_DCTL2, 0);
		spi_writel(host, SPI_DCTL3, 0);
		spi_writel(host, SPI_DCTL4, 0);
		spi_writel(host, SPI_FDRCTL, 0);
		imapx_spi_enable(host, 0);
		message->status = 0;
		if (transfer->cs_change)
			imapx_spi_chip_select(host, SPI_CS_DESELECT);
		spi_finalize_current_message(host->master);
	}
	else if(message->state == STATE_ERROR) {
		spi_writel(host, SPI_DCTL0, 0);
		spi_writel(host, SPI_DCTL1, 0);
		spi_writel(host, SPI_DCTL2, 0);
		spi_writel(host, SPI_DCTL3, 0);
		spi_writel(host, SPI_DCTL4, 0);
		spi_writel(host, SPI_FDRCTL, 0);
		imapx_spi_enable(host, 0);
		message->status = -EIO;
			imapx_spi_chip_select(host, SPI_CS_DESELECT);
		spi_finalize_current_message(host->master);
	} else if(message->state == STATE_RUNNING) {
#if 1
		return 0;
#else
		transfer = host->cur_transfer;
		{

			unsigned long time, timeout = jiffies + msecs_to_jiffies(10000);
			unsigned long print_len, len = transfer->len;
			uint8_t *buf;

			while(host->dma_running == true)
			{
				cpu_relax();
				time = jiffies;
				if (time_after(time, timeout))
				{
					message->state = STATE_ERROR;
					pr_err("[spimul] DMA Transfer  TIMEOUT\n");
					pr_err("[spimul] transfer->len = %d, transfer->rx_buf = 0x%x, host->buffer = 0x%x\n", transfer->len, transfer->rx_buf, host->buffer);
					if(transfer->rx_buf) {
						struct scatterlist *sg;
						unsigned int i;

						buf = (transfer->len > SCATTER_SIZE) ? transfer->rx_buf : host->buffer;
						for_each_sg(host->sgt_rx.sgl, sg, host->sgt_rx.nents, i) {
							print_len =	(len > SCATTER_SIZE)? SCATTER_SIZE: len;
							pr_err("[spimul] SPI RX SG: %d, buf = 0x%x, print_len = %d, len = %d\n", i, buf, print_len, len);
							print_hex_dump(KERN_ERR, "SPI RX: ",
									DUMP_PREFIX_OFFSET,
									16,
									1,
									buf,
									print_len,
									1);
							len -= print_len;
							if(len < 0)
								break;
							buf += print_len;
						}
					}else if(transfer->tx_buf) {
						struct scatterlist *sg;
						unsigned int i;

						buf = transfer->tx_buf;
						for_each_sg(host->sgt_tx.sgl, sg, host->sgt_tx.nents, i) {
							print_len =	(len > SCATTER_SIZE)? SCATTER_SIZE: len;
							pr_err("[spimul] SPI TX SG: %d, buf = 0x%x, print_len = %d, len = %d\n", i, buf, print_len, len);
							print_hex_dump(KERN_ERR, "SPI TX: ",
									DUMP_PREFIX_OFFSET,
									16,
									1,
									buf,
									print_len,
									1);
							len -= print_len;
							if(len < 0)
								break;
							buf += print_len;
						}
					}
					message->status = -EIO;
					if (transfer->cs_change)
						imapx_spi_chip_select(host, SPI_CS_DESELECT);
					spi_finalize_current_message(host->master);
					//imapx_spi_dump_regs(host);
				}
			}
		}
#endif
	}

	return 0;
}

static int imapx_spi_poll_transfer(struct imapx_spi *host) {
	struct spi_message *message = host->message;
	struct spi_transfer *previous, *transfer, *tmp_trans, *last_transfer;
	unsigned long timeout;
	int status;
	unsigned int tx_cnt = 0;
	
	message->state = STATE_START;

	imapx_spi_hwinit(host);
	imapx_spi_flush_fifo(host);

	list_for_each_entry_safe(transfer, tmp_trans, &message->transfers, transfer_list) {
		int first = 0, last = 0;

		if(message->state == STATE_ERROR)
			goto out;
		if (&transfer->transfer_list == message->transfers.next)
			first = 1;

		if (&transfer->transfer_list == message->transfers.prev)
			last = 1;

		host->cur_transfer = transfer;

		if(transfer->rx_buf && transfer->tx_buf) {
			pr_err("[spimul] host controller is half duplex\n");
			return -EINVAL;
		}
		if(!last && transfer->rx_buf) {
			pr_err("[spimul] acording to SPIMUL, first transfer of message can't config to read\n");
			message->state = STATE_ERROR;
			goto out;
		}
		if(transfer->tx_buf && tx_cnt > 6) {
			pr_err("[spimul] according to SPIMUL, no more than 5 tx transfer could be send out in one message\n");
			message->state = STATE_ERROR;
			goto out;
		}

		if(message->state == STATE_RUNNING) {
			previous = list_entry(transfer->transfer_list.prev,
					struct spi_transfer, transfer_list);
			if(previous->delay_usecs)
				udelay(previous->delay_usecs);
			if (previous->cs_change)
				imapx_spi_chip_select(host, SPI_CS_SELECT);
		} else if (message->state == STATE_START) {
			message->state = STATE_RUNNING;
			if(!host->next_msg_cs_active)
				imapx_spi_chip_select(host, SPI_CS_SELECT);
		}

		
		if(transfer->tx_buf) {
			if (first == 1 && last == 1) {
				if(spi_log_enable)
					pr_info("[spimul] poll tx first&last!, tx_cnt = %d\n", tx_cnt);
				status =  imapx_spi_config_dp0to4(host, transfer, tx_cnt);
				if(status)
					pr_err("[spimul] transfer config initial error!\n");
				/*the only transfer of this message*/
				imapx_spi_update_dp0to4(host);
				imapx_spi_enable(host, 1);
				imapx_spi_wait_leisure(host);
				message->state = STATE_DONE;
			} else if(last != 1) {
				if(spi_log_enable)
					pr_info("[spimul] poll tx !last, tx_cnt = %d\n", tx_cnt);
				status =  imapx_spi_config_dp0to4(host, transfer, tx_cnt);
				if(status)
					pr_err("[spimul] transfer config initial error!\n");
			} else if(last ==1) {

				struct spi_transfer remnant_transfer;
				int i, len = transfer->len;

				if(spi_log_enable)
					pr_info("[spimul] poll tx last, tx_cnt = %d\n", tx_cnt);

				for(i = 0; tx_cnt < 5; i++, tx_cnt++) {

					struct spi_transfer tmp_transfer;
					memset(&tmp_transfer, 0, sizeof(tmp_transfer));
					if(len <= 0)
						break;

					tmp_transfer.tx_buf = (u8*)transfer->tx_buf + (i * 4);
					tmp_transfer.len = len < 4 ? len : 4;
					tmp_transfer.tx_nbits = transfer->tx_nbits;
					status = imapx_spi_config_dp0to4(host, &tmp_transfer, tx_cnt);
					if(status) {
						pr_err("[spimul] transfer config initial error!\n");
						goto out;
					}

					len -= tmp_transfer.len;
				}

				remnant_transfer.tx_buf = (u8*)transfer->tx_buf + (transfer->len - len);
				remnant_transfer.len = len;
				remnant_transfer.tx_nbits = transfer->tx_nbits;
				host->cur_transfer = &remnant_transfer;

				timeout = imapx_spi_caculate_timeout(host);
				imapx_spi_update_dp0to4(host);
				imapx_spi_config_update_fdp(host);
				imapx_spi_enable(host, 1);
				status = imapx_spi_poll_write_timeout(host, timeout);
				if(status) {
					pr_err("[spimul] poll write failed: timeout\n");
					message->state = STATE_ERROR;
					goto out;
				}
				imapx_spi_wait_leisure(host);
			}
			tx_cnt++;
		}
		
		if(transfer->rx_buf) {
			if(spi_log_enable)
				pr_info("[spimul] poll rx last\n", tx_cnt);
			host->cur_rx_len = transfer->len;
			timeout = imapx_spi_caculate_timeout(host);
			imapx_spi_update_dp0to4(host);
			imapx_spi_config_update_fdp(host);
			imapx_spi_enable(host, 1);
			status = imapx_spi_poll_read_timeout(host, timeout);
			if(status) {
				pr_err("[spimul] poll read failed: timeout\n");
				message->state = STATE_ERROR;
				goto out;
			}
		}

		message->actual_length += transfer->len;

		if (transfer->cs_change)
			imapx_spi_chip_select(host, SPI_CS_DESELECT);

		if(last == 1)
			message->state = STATE_DONE;
		
	}
out:
	imapx_spi_enable(host, 0);
	spi_writel(host, SPI_DCTL0, 0);
	spi_writel(host, SPI_DCTL1, 0);
	spi_writel(host, SPI_DCTL2, 0);
	spi_writel(host, SPI_DCTL3, 0);
	spi_writel(host, SPI_DCTL4, 0);
	spi_writel(host, SPI_FDRCTL, 0);

	if(message->state == STATE_DONE)
		message->status = 0;
	else
		message->status = -EIO;

	host->next_msg_cs_active = false;
	last_transfer = list_entry(message->transfers.prev,
					struct spi_transfer, transfer_list);
	if(last_transfer->delay_usecs)
		udelay(previous->delay_usecs);
	if(!last_transfer->cs_change) {
		struct spi_message *next_msg;
		next_msg = spi_get_next_queued_message(host->master);
		if (next_msg && next_msg->spi != message->spi)
			next_msg = NULL;
		if(!next_msg || message->state == STATE_ERROR)
			imapx_spi_chip_select(host, SPI_CS_DESELECT);
		else
			host->next_msg_cs_active = true;
	}

	return 0;
}

static int imapx_spi_transfer_one_message(struct spi_master *master,
		struct spi_message *msg) {
	struct imapx_spi *host = spi_master_get_devdata(master);
	struct chip_data *chip = spi_get_ctldata(msg->spi); 
	struct spi_transfer *transfer, *tmp_trans;
	int status;
	int total_len = 0;

	host->message = msg;
	host->cur_transfer = list_entry(msg->transfers.next,
				struct spi_transfer, transfer_list);
	host->cur_chip = chip;

	spi_writel(host, SPI_CTL, chip->mode);
	spi_writel(host, SPI_CFDF, chip->clk_div);

	list_for_each_entry_safe(transfer, tmp_trans, &msg->transfers, transfer_list) {
		total_len += transfer->len;                                            
	}                                                                              


	if(chip->enable_dma && spi_dma_enable && total_len > 64) {
	//if(chip->enable_dma && total_len > 64) {
		status = imapx_spi_dma_transfer(host);
	} else {
		status = imapx_spi_poll_transfer(host);
		spi_finalize_current_message(master);
	}
	return status;
}

static int imapx_spi_unprepare_transfer_hardware(struct spi_master *master) {
	return 0;
}

static int imapx_spi_prepare_transfer_hardware(struct spi_master *master) {
	return 0;
}

static void imap_ssp_cs_control(u32 command) {
	imapx_pad_set_mode(74, "output");
	imapx_pad_set_value(74, !!command);
}

/*
 * A piece of default chip info unless the platform
 * supplies it.
 */
static struct imapx_spi_chip_config  imapx_default_chip_info = {
	.com_mode = SPIMUL_TRANS_DMA,
	.iface = SPIMUL_INTERFACE_MOTOROLA_SPI,
	.rx_lev_trig = SPIMUL_RX_8_OR_MORE_ELEM,
	.tx_lev_trig = SPIMUL_TX_8_OR_MORE_EMPTY_LOC,
	.cs_control = imap_ssp_cs_control,
};

/*
 * 
 */
static int imapx_spi_setup(struct spi_device *spi) {
	struct imapx_spi_chip_config *chip_info;
	struct chip_data *chip;
	struct imapx_spi *host = spi_master_get_devdata(spi->master);

	if(spi->bits_per_word != 8 && spi->bits_per_word != 16) {
		pr_err("[spimul] Not support bits_per_word %d right now.\n", spi->bits_per_word);
		return -EINVAL;
	}

	if (!spi->max_speed_hz) {
		pr_err("[spimul] No max speed HZ parameter\n");
		return -EINVAL;
	}
	/* Only alloc on first setup */
	chip = spi_get_ctldata(spi);
	if (!chip) {
		chip = kzalloc(sizeof(struct chip_data), GFP_KERNEL);
		if (!chip) {
			pr_err("[spimul] cannot allocate controller state\n");
			return -ENOMEM;
		}
	}

	chip_info = spi->controller_data;
	if (!chip_info) {
		chip_info = &imapx_default_chip_info;
		pr_info("[spimul] use default chip config\n");
	}

	host->rx_lev_trig = chip_info->rx_lev_trig;
	host->tx_lev_trig = chip_info->tx_lev_trig;
	chip->cs_control = chip_info->cs_control;
	chip->xfer_type = chip_info->com_mode;
	chip->speed_hz = spi->max_speed_hz;
	imapx_spi_caculate_clkdiv(host, chip);

	if (spi->mode & SPI_CPOL) 
		chip->mode = 0x1;
	else
		chip->mode = 0x0;

	if(spi->mode & SPI_CPHA )
		chip->mode |= 0x2;
	else
		chip->mode &= ~0x2;

	if(spi->bits_per_word > 32) {
		pr_err("[spimul] illegal bits per word for this controller");
		return -EINVAL;
	} else if( spi->bits_per_word <= 8) {
		chip->n_bytes = 1;
		host->bitwidth = BIT_WIDTH_U8;
	} else if( spi->bits_per_word <= 16) {
		chip->n_bytes = 2;
		host->bitwidth = BIT_WIDTH_U16;
	} else {
		chip->n_bytes = 4;
		host->bitwidth = BIT_WIDTH_U32;
	}

#if 0
	host->bitwidth = BIT_WIDTH_U8;
	host->rx_bitwidth = host->tx_bitwidth = host->bitwidth;
#else
	host->rx_bitwidth = BIT_WIDTH_U32;
	host->tx_bitwidth = BIT_WIDTH_U8;
#endif

	if(chip_info->com_mode == SPIMUL_TRANS_DMA && host->master_info->enable_dma) {
		chip->enable_dma = true;
		/* enable tx and rx dma */
		spi_writel(host, SPI_DMACR, 0x3);

	} else {
		chip->enable_dma = false;
		/* disable tx and rx dma */
		spi_writel(host, SPI_DMACR, 0x0);
	}

	spi_set_ctldata(spi, chip);
	return 0;
}

static void imapx_spi_cleanup(struct spi_device *spi) {
	pr_info("[spimul] %s\n", __func__);
}


#if 0
static irqreturn_t imapx_spi_isr(int irq, void *dev_id) {
	struct imapx_spi *host = dev_id;
	u32 status, rawstatus, mask;
	status = spi_readl(host, SPI_MIS);
	rawstatus = spi_readl(host, SPI_RIS);
	mask = spi_readl(host, SPI_IMSC);
	pr_info("[spimul] spi_irq mask-%x, status-0x%x, raw status-0x%x\n", mask, status, rawstatus);
	//imapx_spi_dump_regs(host);
	spi_writel(host, SPI_ICR, 0x7);
	return IRQ_HANDLED;
}
#endif

static int imapx_spi_probe(struct platform_device *pdev) {
	struct imapx_spi_host_config *pdata = pdev->dev.platform_data;
	struct spi_master *master;
	struct imapx_spi *host;
	struct resource *res;
	int ret;

	pr_info("Infotm 4-wires SPI Host controller driver probe\n");
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENXIO;

	master = spi_alloc_master(&pdev->dev, sizeof(struct imapx_spi));
	if (master == NULL) {
		 pr_err("[spimul] cannot alloc SPI master\n");
		 return -ENOMEM;
	}
	host = spi_master_get_devdata(master);
	host->dev = &pdev->dev;
	host->master = master;
	host->next_msg_cs_active = false;
	host->master_info = pdata;
	host->irq = platform_get_irq(pdev, 0);
	host->phy_base = res->start; 
	host->quirk = host->master_info->quirk;
	host->base = devm_ioremap_resource(&pdev->dev, res);
	if(IS_ERR(host->base)) {
		ret = -ENOMEM;
		goto err_no_ioremap;
	}

	platform_set_drvdata(pdev, host);
	/*
	 * Bus Number Which has been Assigned to this SSP controller
	 * on this board
	 */
	master->bus_num = host->master_info->bus_id;
	master->num_chipselect = host->master_info->num_chipselect;
	master->rt = host->master_info->rt; 
	master->cleanup = imapx_spi_cleanup;
	master->setup = imapx_spi_setup;
	master->prepare_transfer_hardware = imapx_spi_prepare_transfer_hardware;
	master->transfer_one_message = imapx_spi_transfer_one_message;
	master->unprepare_transfer_hardware = imapx_spi_unprepare_transfer_hardware;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH | SPI_LOOP 
				| SPI_RX_DUAL | SPI_RX_QUAD | SPI_TX_DUAL | SPI_TX_QUAD; 
	master->flags = SPI_MASTER_HALF_DUPLEX;

	/* for imapx-spi power domain and clk check */
	//module_power_down(SYSMGR_SSP_BASE);
	module_power_on(SYSMGR_SPIMUL_BASE);
	imapx_pad_init("ssp0");

	host->busclk = clk_get_sys("spibus.0", "spibus0");
	if (IS_ERR(host->busclk)) {
		pr_err("[spimul] could not retrieve SSP/SPI bus clock\n");
		ret = -ENXIO;
		goto err_no_busclk;
	}
	host->busclk_rate = clk_get_rate(host->busclk);
	clk_prepare_enable(host->busclk);

	host->extclk = clk_get_sys("imap-ssp", "ext-spi");
	if (IS_ERR(host->extclk)) {
		pr_err("[spimul] cannot retrieve SSP/SPI ext clock\n");
		ret = -ENXIO;
		goto err_no_extclk;
	}
	/*
	 * set ext-clk to paltform asked 
	 */
	clk_set_rate(host->extclk, host->master_info->extclk_rate);
	host->extclk_rate = clk_get_rate(host->extclk);
	pr_info("[spimul] bus_clk: %dHz, ext_clk: %dHz\n", 
			host->busclk_rate, host->extclk_rate);
	clk_prepare_enable(host->extclk);

	host->max_clk_div = 65536;
	host->min_clk_div = 2;

	/*
	 * request irq resource
	 */
	imapx_spi_hwinit(host);
	imapx_spi_irq_mask(host, 1, SPI_INT_MASK_ALL);
#if 0
	ret = devm_request_irq(host->dev, host->irq, imapx_spi_isr,
				0, dev_name(host->dev), host);
	if (ret) {
		pr_err("[spimul] cannot request irq!\n");
		goto err_no_isr;
	}
#endif

	if(host->master_info->enable_dma) {
		if(imapx_spi_dma_probe(host)) {
			pr_err("[spimul] cannot init dma! work in polling mode\n");
			host->master_info->enable_dma = 0;
		}
	}

	ret = spi_register_master(master);
	if(ret) {
		pr_err("[spimul] cannot register SPI master\n");
		goto err_spi_register; 
	}
	imapx_spi_debugfs_init(host);
	pr_info("Infotm 4-wires SPI Host controller driver probe OK!!\n");
	return 0;

err_spi_register:
	if(host->master_info->enable_dma) 
		imapx_spi_dma_remove(host);
//err_no_isr:
err_no_extclk:
	clk_unprepare(host->extclk);
	clk_put(host->extclk);
err_no_busclk:
	module_power_down(SYSMGR_SPIMUL_BASE);
err_no_ioremap:
	spi_master_put(master);
	return ret;
}

static int imapx_spi_remove(struct platform_device *pdev) {
	struct spi_master *master = spi_master_get(platform_get_drvdata(pdev));
	struct imapx_spi *host = spi_master_get_devdata(master);

	imapx_spi_debugfs_remove(host);
	if(host->master_info->enable_dma) 
		imapx_spi_dma_remove(host);
	clk_disable_unprepare(host->extclk);
	spi_unregister_master(master);
	platform_set_drvdata(pdev, NULL);
	spi_master_put(master);
	return 0;
}

#ifdef CONFIG_SUSPEND
static int imapx_spi_suspend(struct device *dev) {
	return 0;
}

static int imapx_spi_resume(struct device *dev) {
	return 0;
}
#endif
#ifdef CONFIG_PM_RUNTIME
static int imapx_spi_runtime_suspend(struct device *dev) {
	return 0;
}

static int imapx_spi_runtime_resume(struct device *dev) {
	return 0;
}
#endif


static const struct dev_pm_ops imapx_spi_pm = {
	SET_SYSTEM_SLEEP_PM_OPS(imapx_spi_suspend, imapx_spi_resume)
	SET_RUNTIME_PM_OPS(imapx_spi_runtime_suspend,
			imapx_spi_runtime_resume, NULL)
};

static struct platform_driver imapx_spi_driver = {
	.driver = {
		.name = "imapx-spi",
		.pm = &imapx_spi_pm,
		.owner  = THIS_MODULE,
	},
	//.id_table = imapx_spi_ids,
	.probe  = imapx_spi_probe,
	.remove = imapx_spi_remove,
};

static int __init imapx_spi_init(void) {
	return platform_driver_register(&imapx_spi_driver);
}
subsys_initcall(imapx_spi_init);

static void __exit imapx_spi_exit(void) {
	platform_driver_unregister(&imapx_spi_driver);
}
module_exit(imapx_spi_exit);

MODULE_DESCRIPTION("IMAPX 4-Wire SPI Master Controller driver");
MODULE_LICENSE("GPL");

