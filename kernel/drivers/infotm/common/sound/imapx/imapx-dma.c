/*****************************************************************************
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** Use of Infotm's code is governed by terms and conditions
** stated in the accompanying licensing statement.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Author:
**     James Xu   <James Xu@infotmic.com.cn>
**
** Revision History:
**     1.0  12/21/2009    James Xu
**	   2.0  12/25/2013	  Sun
*****************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>
#include <linux/dmaengine.h>
#include <mach/power-gate.h>
#include <asm/dma.h>
#include <mach/hardware.h>
#include <linux/ratelimit.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include "imapx-dma.h"
#include "imapx-i2s.h"
#include "imapx-adc.h"
#define TAG "ImapDma"

#ifdef DMA_DEBUG
#define DMA_DBG(fmt...)		pr_err(TAG""fmt)
#else
#define DMA_DBG(...)		do {} while (0)
#endif

#define DMA_ERR(fmt...)		printk(TAG""fmt)

#ifdef CONFIG_TQLP9624_ADC
extern struct mic_voltage_device *mic_vol_dev;
#endif

struct snd_dma_buffer *dma_buffer_info;

static const struct snd_pcm_hardware imapx_pcm_hardware = {
	.info			=	SNDRV_PCM_INFO_INTERLEAVED |
						SNDRV_PCM_INFO_BLOCK_TRANSFER |
						SNDRV_PCM_INFO_MMAP |
						SNDRV_PCM_INFO_MMAP_VALID |
						SNDRV_PCM_INFO_PAUSE |
						SNDRV_PCM_INFO_RESUME,
	.formats		=	SNDRV_PCM_FMTBIT_S32_LE |
						SNDRV_PCM_FMTBIT_U32_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_U24_LE |
						SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_U16_LE |
						SNDRV_PCM_FMTBIT_U8 |
						SNDRV_PCM_FMTBIT_S8,
	.channels_min		= 1,
	.channels_max		= 2,
	.buffer_bytes_max	= PAGE_SIZE*32,
	.period_bytes_min	= PAGE_SIZE,
	.period_bytes_max	= PAGE_SIZE*2,
	.periods_min		= 2,
	.periods_max		= 4,
	.fifo_size			= 64,

#if defined(CONFIG_SND_USE_AECV2)
	.periods_max      = 64,
	.period_bytes_min = (PAGE_SIZE >> 4),
#endif

};

extern void aec_ctrlnode_create(struct device *device);
extern void dump_playback_data(uint8_t *pos, uint32_t len);
extern void dump_capture_ldata(uint8_t *pos, uint32_t len);
extern void dump_capture_rdata(uint8_t *pos, uint32_t len);
extern int aec_buf_prepare(struct snd_pcm_hw_params *params);
extern void aec_pcm_trigger_start(struct snd_pcm_substream *substream);
extern void aec_pcm_trigger_stop(struct snd_pcm_substream *substream);
extern void aec_capture_stream(struct snd_pcm_substream *substream);
extern int imapx_pcm_update(struct snd_pcm_substream *substream);

#define KERNEL_AUD_TIMESTAMP /* IPC app need this */

static void imapx_audio_buffdone(void *data)
{
	struct snd_pcm_substream *substream = data;
	struct imapx_runtime_data *prtd = substream->runtime->private_data;
	struct snd_card *card = substream->pstr->pcm->card;
	int dma_point = 0;
	uint64_t timestamp = 0;
	struct timespec now;

	//DMA_DBG("Entered %s\n", __func__);
	if (prtd->state & ST_RUNNING) {
#ifdef CONFIG_SND_IMAPX_SOC_IP6205
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			if (strcmp(card->id, "ip6205iis") == 0) {
				char *hwbuf = substream->runtime->dma_area + (prtd->dma_pos - prtd->dma_start);
				do {
					*(u32*)(hwbuf + dma_point) = *(u32*)(hwbuf + dma_point) << 1;
					dma_point+=4;
				} while ((dma_point) < prtd->dma_period);
			}
		}
#endif
        if (0 == strcmp(card->id, "ip6205iis")) {
            aec_capture_stream(substream);
        }
#ifdef KERNEL_AUD_TIMESTAMP
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE && strcmp(card->id, "btpcm")) {
			get_monotonic_boottime(&now);
			timestamp = now.tv_sec * 1000 + now.tv_nsec / 1000000;
			char *hwbuf = substream->runtime->dma_area + (prtd->dma_pos - prtd->dma_start);
			uint16_t time = 0;
			int i;
			for (i = 0; i < 4; i++) {
				time = (uint16_t) (timestamp & 0x0000ffff);
				*(uint16_t*)hwbuf = time;
				timestamp >>= 16;
				hwbuf+=4;
			}
		}
#endif
		prtd->dma_pos += prtd->dma_period;
		if (prtd->dma_pos >= prtd->dma_end)
			prtd->dma_pos = prtd->dma_start;

		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_pcm_period_elapsed(substream);
		}

		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE){
			snd_pcm_period_elapsed(substream);
		}

        if (0 == strcmp(card->id, "ip6205iis")) {
            imapx_pcm_update(substream);
        }
	}
}


/* imapx_pcm_enqueue
 *
 * place a dma buffer onto the queue for the dma system
 * to handle.
 */
void imapx_pcm_enqueue(struct snd_pcm_substream *substream)
{
	struct imapx_runtime_data *prtd = substream->runtime->private_data;
	dma_addr_t pos = prtd->dma_pos;
	int limit;
	struct imapx_dma_prep_info dma_info;

	DMA_DBG("Entered %s\n", __func__);

#if defined (CONFIG_SND_USE_AECV2)
	limit = 1;
	dma_info.cap = DMA_SLAVE;
#else
	dma_info.cap = DMA_LLI;
	limit = (prtd->dma_end - prtd->dma_start) / prtd->dma_period;
#endif

	dma_info.direction =
		(substream->stream == SNDRV_PCM_STREAM_PLAYBACK
		 ? DMA_MEM_TO_DEV : DMA_DEV_TO_MEM);
	dma_info.fp = imapx_audio_buffdone;
	dma_info.fp_param = substream;
	dma_info.period = prtd->dma_period;
	dma_info.len = prtd->dma_period*limit;
	dma_info.buf = pos;
	prtd->params->ops->prepare(prtd->params->ch, &dma_info);

	prtd->dma_pos = pos;
}

static int imapx_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{

	struct snd_pcm_runtime *runtime = substream->runtime;
	struct imapx_runtime_data *prtd = runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_card *card = substream->pstr->pcm->card;
	struct imapx_pcm_dma_params *dma;
	unsigned long totbytes = params_buffer_bytes(params);
	unsigned long period = params_period_bytes(params);
	struct imapx_dma_info dma_info;

	DMA_DBG("Entered %s\n", __func__);

	dma = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	DMA_DBG("cpu_dat is %s\n", rtd->cpu_dai->name);
	/* return if this is a bufferless transfer e.g.
	 * codec <--> BT codec or GSM modem -- lg FIXME */
	if (!dma)
		return 0;
	/* this may get called several times by oss emulation
	 * with different params -HW */
	if (prtd->params == NULL) {
		/* prepare DMA */
		prtd->params = dma;

		DMA_DBG("params %p, client %p, channel %d\n", prtd->params,
				prtd->params->client, prtd->params->channel);

		prtd->params->ops = imapx_dmadev_get_ops();

		dma_info.cap = DMA_CYCLIC;
		dma_info.client = prtd->params->client;
		dma_info.direction =
			(substream->stream == SNDRV_PCM_STREAM_PLAYBACK ?
			DMA_MEM_TO_DEV : DMA_DEV_TO_MEM);
		dma_info.width = prtd->params->dma_size;
		dma_info.fifo = prtd->params->dma_addr;

		if(prtd->params->channel==IMAPX_I2S_MASTER_RX){
#ifdef CONFIG_TQLP9624_ADC
			if(mic_vol_dev->ch !=0){
				prtd->params->ops->flush(mic_vol_dev->ch);
				prtd->params->ops->release(mic_vol_dev->ch, NULL);
				mic_vol_dev->dma_state |= ST_RUNNING;
			}
#endif
			prtd->params->ch = prtd->params->ops->request(
				prtd->params->channel, &dma_info);
#ifdef CONFIG_TQLP9624_ADC
			mic_vol_dev->ch = prtd->params->ch;
#endif
		}else{
			prtd->params->ch = prtd->params->ops->request(
			prtd->params->channel, &dma_info);
		}
	}

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);

#if defined(CONFIG_SND_USE_AECV2)
	totbytes -= (totbytes % period); /* dma buffer must be divided exactly by period */
#endif
	runtime->dma_bytes = totbytes;

	spin_lock_irq(&prtd->lock);
	prtd->dma_loaded = 0;
	prtd->dma_limit = runtime->hw.periods_min;

	prtd->dma_start = runtime->dma_addr;
	prtd->dma_period = params_period_bytes(params);
	prtd->dma_end = prtd->dma_start + totbytes;
	prtd->dma_pos = prtd->dma_start;
	spin_unlock_irq(&prtd->lock);

    if (0 == strcmp(card->id, "ip6205iis")) {
        aec_buf_prepare(params);
    }
	return 0;
}

static int imapx_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct imapx_runtime_data *prtd = substream->runtime->private_data;

	DMA_DBG("Entered %s\n", __func__);
	/* TODO - do we need to ensure DMA flushed */
	snd_pcm_set_runtime_buffer(substream, NULL);

	if (prtd->params) {
		prtd->params->ops->flush(prtd->params->ch);
		prtd->params->ops->release(prtd->params->ch,
				prtd->params->client);
#ifdef CONFIG_TQLP9624_ADC
		if(mic_vol_dev->ch !=0 && prtd->params->channel==IMAPX_I2S_MASTER_RX){
			mic_vol_dev->ch = 0;
			mic_vol_dev->dma_state &= ~ST_RUNNING;
		}
#endif
		prtd->params = NULL;
	}
	return 0;
}

static int imapx_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct imapx_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;

	DMA_DBG("Entered %s\n", __func__);

	/* return if this is a bufferless transfer e.g.
	 * codec <--> BT codec or GSM modem -- lg FIXME */
	if (!prtd->params)
		return 0;
	/* flush the DMA channel */
	prtd->params->ops->flush(prtd->params->ch);
	prtd->dma_loaded = 0;
	prtd->dma_pos = prtd->dma_start;

	/* enqueue dma buffers */
	imapx_pcm_enqueue(substream);
	return ret;
}



static int imapx_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct imapx_runtime_data *prtd = substream->runtime->private_data;
    struct snd_card *card = substream->pstr->pcm->card;
	int ret = 0;

	DMA_DBG("Entered %s, cmd = %x", __func__, cmd);

	spin_lock(&prtd->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_RESUME:
		/*
		 * added by ayakashi @20130117
		 * when resume, when must prepare the dma description again
		 * because the descreption will be delete by dma flush(suspend).
		 * Some Registers are wrong when alc5631  or es8328 resume.
		 * TODO:make codec registers restore correctly  when resume
		 */
		imapx_pcm_prepare(substream);
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
        if (0 == strcmp(card->id, "ip6205iis")) {
            aec_pcm_trigger_start(substream);
        }
		prtd->state |= ST_RUNNING;
		prtd->params->ops->trigger(prtd->params->ch);
#ifdef CONFIG_TQLP9624_ADC
		if(prtd->params->channel == IMAPX_I2S_MASTER_RX)
			mic_vol_dev->dma_state |= ST_RUNNING;
#endif
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
        if (0 == strcmp(card->id, "ip6205iis")) {
            aec_pcm_trigger_stop(substream);
        }
		prtd->state &= ~ST_RUNNING;
		prtd->params->ops->stop(prtd->params->ch);
#ifdef CONFIG_TQLP9624_ADC
		if(prtd->params->channel == IMAPX_I2S_MASTER_RX)
			mic_vol_dev->dma_state &= ~ST_RUNNING;
#endif
		break;
	default:
		ret = -EINVAL;
		break;
	}

	spin_unlock(&prtd->lock);

	return ret;
}

static snd_pcm_uframes_t
imapx_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct imapx_runtime_data *prtd = runtime->private_data;
	unsigned long res;
	dma_addr_t src;
	dma_addr_t dst;
	int ret;

//	DMA_DBG("Entered %s\n", __func__);

	spin_lock(&prtd->lock);

	ret = prtd->params->ops->dma_getposition(prtd->params->ch, &src, &dst);
	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		res = dst - prtd->dma_start;
	} else {
		res = src - prtd->dma_start;
	}
#if defined(CONFIG_SND_USE_AECV2)
	res  &=  ~(prtd->dma_period - 1); /* must be align when use aecv2.0 */
#else
	res = prtd->dma_period * (res / prtd->dma_period);
#endif

	spin_unlock(&prtd->lock);

	/* we seem to be getting the odd error from the pcm library due
	 * to out-of-bounds pointers. this is maybe due to the dma engine
	 * not having loaded the new values for the channel before being
	 * callled... (todo - fix )
	 */

	if (res >= snd_pcm_lib_buffer_bytes(substream)) {
		if (res == snd_pcm_lib_buffer_bytes(substream))
			res = 0;
	}

	return bytes_to_frames(substream->runtime, res);
}

static int imapx_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct imapx_runtime_data *prtd;

	DMA_DBG("Entered %s\n", __func__);

	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	snd_soc_set_runtime_hwparams(substream, &imapx_pcm_hardware);

	prtd = kzalloc(sizeof(struct imapx_runtime_data), GFP_KERNEL);
	if (prtd == NULL)
		return -ENOMEM;

	spin_lock_init(&prtd->lock);

	runtime->private_data = prtd;
	return 0;
}

static int imapx_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct imapx_runtime_data *prtd = runtime->private_data;

	DMA_DBG("Entered %s\n", __func__);

	if (!prtd)
		DMA_ERR("dma_close called with prtd == NULL\n");

	kfree(prtd);

	return 0;
}

static int imapx_pcm_mmap(struct snd_pcm_substream *substream,
		struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	DMA_DBG("Entered %s\n", __func__);
	return dma_mmap_writecombine(substream->pcm->card->dev, vma,
			runtime->dma_area,
			runtime->dma_addr,
			runtime->dma_bytes);
}

static struct snd_pcm_ops imapx_pcm_ops = {
	.open		= imapx_pcm_open,
	.close		= imapx_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= imapx_pcm_hw_params,
	.hw_free	= imapx_pcm_hw_free,
	.prepare	= imapx_pcm_prepare,
	.trigger	= imapx_pcm_trigger,
	.pointer	= imapx_pcm_pointer,
	.mmap		= imapx_pcm_mmap,
};

static int imapx_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = imapx_pcm_hardware.buffer_bytes_max;

	DMA_DBG("Entered %s\n", __func__);

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_writecombine(pcm->card->dev, size,
			&buf->addr, GFP_KERNEL);
	if (!buf->area)
		return -ENOMEM;
	buf->bytes = size;
	dma_buffer_info = buf;
	return 0;
}



static void imapx_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	DMA_DBG("Entered %s\n", __func__);

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;

		dma_free_writecombine(pcm->card->dev, buf->bytes,
				buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 imapx_pcm_dmamask = DMA_BIT_MASK(32);

static int imapx_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;

	DMA_DBG("Entered %s\n", __func__);
	if (!card->dev->dma_mask)
		card->dev->dma_mask = &imapx_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_BIT_MASK(32);

	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream) {
		ret = imapx_pcm_preallocate_dma_buffer(pcm,
				SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}
	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
		ret = imapx_pcm_preallocate_dma_buffer(pcm,
				SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
out:
	return ret;
}

static struct snd_soc_platform_driver imapx_soc_platform = {
	.ops		= &imapx_pcm_ops,
	.pcm_new	= imapx_pcm_new,
	.pcm_free	= imapx_pcm_free_dma_buffers,
};

int imapx_asoc_platform_probe(struct device *dev)
{
	aec_ctrlnode_create(dev);
	return snd_soc_register_platform(dev, &imapx_soc_platform);
}


void imapx_asoc_platform_remove(struct device *dev)
{
	snd_soc_unregister_platform(dev);
}


MODULE_AUTHOR("Sun");
MODULE_DESCRIPTION("INFOTM imapx PCM DMA module");
MODULE_LICENSE("GPL");
