#ifndef __IMAPX_DMA__
#define __IMAPX_DMA__

/*
 * PL330 can assign any channel to communicate with
 * any of the peripherals attched to the DMAC.
 * For the sake of consistency across client drivers,
 * We keep the channel names unchanged and only add
 * missing peripherals are added.
 * Order is not important since IMAPX PL330 API driver
 * use these just as IDs.
 */
enum dma_ch {
	IMAPX_PCM0_TX = 0,
	IMAPX_PCM0_RX = 2,
	IMAPX_SSP0_TX = 4,
	IMAPX_SSP1_TX = 5,
	IMAPX_SSP0_RX = 6,
	IMAPX_SSP1_RX = 7,
	IMAPX_UART0_TX = 10,
	IMAPX_UART1_TX = 11,
	IMAPX_UART2_TX = 12,
	IMAPX_UART3_TX = 13,
	IMAPX_UART0_RX = 14,
	IMAPX_UART1_RX = 15,
	IMAPX_UART2_RX = 16,
	IMAPX_UART3_RX = 17,
	IMAPX_I2S_MASTER_TX = 20,
	IMAPX_I2S_MASTER_RX = 21,
	IMAPX_TOUCHSCREEN_RX = 22,
	/*  END Marker, also used to denote a reserved channel */
	IMAPX_MAX,
	IMAPX_I2S_SLAVE_TX,
	IMAPX_I2S_SLAVE_RX,
	IMAPX_AC97_TX,
	IMAPX_AC97_RX,
	IMAPX_PCM1_TX,
	IMAPX_PCM1_RX,
	IMAPX_SPDIF_TX,
	IMAPX_SPDIF_RX,
	IMAPX_PWMA_TX,
	IMAPX_DMIC_RX,
};

#include <linux/dma-direction.h>
#include <linux/dmaengine.h>

struct imapx_dma_client {
	char *name;
};

struct imapx_dma_prep_info {
	enum dma_transaction_type cap;
	enum dma_data_direction direction;
	dma_addr_t buf;
	unsigned long period;
	unsigned long len;
	void (*fp)(void *data);
	void *fp_param;
};

struct imapx_dma_info {
	enum dma_transaction_type cap;
	enum dma_data_direction direction;
	enum dma_slave_buswidth width;
	dma_addr_t fifo;
	struct imapx_dma_client *client;
};

struct imapx_dma_ops {
	unsigned (*request)(enum dma_ch ch, struct imapx_dma_info *info);
	int (*release)(unsigned ch, struct imapx_dma_client *client);
	int (*prepare)(unsigned ch, struct imapx_dma_prep_info *info);
	int (*trigger)(unsigned ch);
	int (*started)(unsigned ch);
	int (*flush)(unsigned ch);
	int (*stop)(unsigned ch);
	/**added by ayakashi for get dma real transfer position */
	int (*dma_getposition)(unsigned ch, dma_addr_t *src, dma_addr_t *dst);
};

extern bool pl330_filter(struct dma_chan *chan, void *param);

#endif
