#ifndef _IMAPX800_DMA_H_
#define _IMAPX800_DMA_H_

#define ST_RUNNING		(1<<0)
#define ST_OPENED		(1<<1)

#include <mach/imap_pl330.h>

extern void *imapx_dmadev_get_ops(void);
extern int imapx_asoc_platform_probe(struct device *dev);
extern void imapx_asoc_platform_remove(struct device *dev);
extern void imapx_dma_width_get(uint32_t width);

struct imapx_pcm_dma_params {
	struct imapx_dma_client *client;	/* stream identifier */
	int channel;
	dma_addr_t dma_addr;
	int dma_size;			/* Size of the DMA transfer */
	unsigned ch;
	struct imapx_dma_ops *ops;
};

struct imapx_runtime_data {
	spinlock_t lock;
	int state;
	unsigned int dma_loaded;
	unsigned int dma_limit;
	unsigned int dma_period;
	dma_addr_t dma_start;
	dma_addr_t dma_pos;
	dma_addr_t dma_end;
	struct imapx_pcm_dma_params *params;
};

#endif
