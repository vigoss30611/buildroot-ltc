#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/amba/pl330.h>
#include <linux/scatterlist.h>
#include <mach/imap_pl330.h>
#include <linux/export.h>


static int imapx_dma_getposition(unsigned ch, dma_addr_t *src,
				    dma_addr_t *dst)
{
	struct dma_chan *chan = (struct dma_chan *)ch;

	return chan->device->device_dma_getposition(chan, src, dst);
}

static unsigned imapx_dmadev_request(enum dma_ch dma_ch,
					struct imapx_dma_info *info)
{
	struct dma_chan *chan;
	dma_cap_mask_t mask;
	struct dma_slave_config slave_config;

	dma_cap_zero(mask);
	dma_cap_set(info->cap, mask);

	chan = dma_request_channel(mask, pl330_filter, (void *)dma_ch);

	if (info->direction == DMA_FROM_DEVICE) {
		memset(&slave_config, 0, sizeof(struct dma_slave_config));
		slave_config.direction = info->direction;
		slave_config.src_addr = info->fifo;
		slave_config.src_addr_width = info->width;
		slave_config.src_maxburst = 8;
		slave_config.byteswap = DMA_SLAVE_BYTESWAP_NO;
		dmaengine_slave_config(chan, &slave_config);
	} else if (info->direction == DMA_TO_DEVICE) {
		memset(&slave_config, 0, sizeof(struct dma_slave_config));
		slave_config.direction = info->direction;
		slave_config.dst_addr = info->fifo;
		slave_config.dst_addr_width = info->width;
		slave_config.dst_maxburst = 8;
		slave_config.byteswap = DMA_SLAVE_BYTESWAP_NO;
		dmaengine_slave_config(chan, &slave_config);
	}

	return (unsigned)chan;
}

static int imapx_dmadev_release(unsigned ch,
				   struct imapx_dma_client *client)
{
	dma_release_channel((struct dma_chan *)ch);

	return 0;
}

static int imapx_dmadev_prepare(unsigned ch,
				   struct imapx_dma_prep_info *info)
{
	struct scatterlist sg;
	struct dma_chan *chan = (struct dma_chan *)ch;
	struct dma_async_tx_descriptor *desc;

	switch (info->cap) {
	case DMA_SLAVE:
		sg_init_table(&sg, 1);
		sg_dma_len(&sg) = info->len;
		sg_set_page(&sg, pfn_to_page(PFN_DOWN(info->buf)),
			    info->len, offset_in_page(info->buf));
		sg_dma_address(&sg) = info->buf;

		desc = chan->device->device_prep_slave_sg(chan,
							  &sg, 1,
							  info->direction,
							  DMA_PREP_INTERRUPT,
							  NULL);
		break;
	case DMA_CYCLIC:
		desc = chan->device->device_prep_dma_cyclic(chan,
							    info->buf,
							    info->len,
							    info->period,
							    info->direction, 0,
							    NULL);
		break;
	case DMA_LLI:
		desc = chan->device->device_prep_dma_lli(chan,
							 info->buf, info->len,
							 info->period,
							 info->direction);
		break;
	default:
		dev_err(&chan->dev->device, "unsupported format\n");
		return -EFAULT;
	}

	if (!desc) {
		dev_err(&chan->dev->device, "cannot prepare cyclic dma\n");
		return -EFAULT;
	}

	desc->callback = info->fp;
	desc->callback_param = info->fp_param;

	dmaengine_submit((struct dma_async_tx_descriptor *)desc);

	return 0;
}

static inline int imapx_dmadev_trigger(unsigned ch)
{
	dma_async_issue_pending((struct dma_chan *)ch);

	return 0;
}

static inline int imapx_dmadev_flush(unsigned ch)
{
	return dmaengine_terminate_all((struct dma_chan *)ch);
}

struct imapx_dma_ops dmadev_ops = {
	.request = imapx_dmadev_request,
	.release = imapx_dmadev_release,
	.prepare = imapx_dmadev_prepare,
	.trigger = imapx_dmadev_trigger,
	.started = NULL,
	.flush = imapx_dmadev_flush,
	.stop = imapx_dmadev_flush,
	.dma_getposition = imapx_dma_getposition,
};

void *imapx_dmadev_get_ops(void)
{
	return &dmadev_ops;
}
