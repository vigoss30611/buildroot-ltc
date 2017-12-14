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

static uint32_t aecv2_control = 0; /* AECV2.0 default is not on */
static inline uint32_t snd_aecv2_is_enable(void)
{
    return aecv2_control;
}

#define SAVED_PERIOD	(2)
#define PLAYBACK_POOL(period)	(period * (SAVED_PERIOD+7))
#define DELAY_COMPLEMENT(x) (512)
#define ALLOWANCE(x) (x*1)

static uint8_t capture_dma_enable = 0;
static uint8_t capture_dma_done = 0;
static int8_t play_sync = 0;
static int8_t wait_play_period = 0;
static uint32_t miss_play_period = 0;
static unsigned long ply_buf_size = 0;
static uint8_t *ply_buf_start = NULL;
static uint8_t *ply_buf_end = NULL;
static uint8_t *p_ply_rd = NULL;
static uint8_t *p_ply_wt = NULL;

static unsigned long complement = 0;
static unsigned long cap_buf_size = 0;
static uint8_t *cap_buf_start = NULL;
static uint8_t *cap_buf_end = NULL;
static uint8_t *p_cap_rd = NULL;
static uint8_t *p_cap_wt = NULL;

#define U16_STEP  (sizeof(uint16_t))
#define U32_STEP  (sizeof(uint32_t))

static uint32_t dma_width = U32_STEP;

void imapx_dma_width_get(uint32_t width)
{
    dma_width = width;
}
EXPORT_SYMBOL(imapx_dma_width_get);

static void inline simple_copy(void *dst, void *src, uint32_t width)
{
    if (width & U16_STEP) {
        *(uint16_t *)dst = *(uint16_t *)src;
    } else {
        *(uint32_t *)dst = *(uint32_t *)src;
    }
}

static void inline simple_assign(void *dst, uint32_t data, uint32_t width)
{
    if (width & U16_STEP) {
        *(uint16_t *)dst = data;
    } else {
        *(uint32_t *)dst = data;
    }
}

static void fill_left_chan(uint8_t *pos, uint32_t len)
{
    int i = 0;
    uint8_t *p = pos;   /* point to left chan data: LRLR */
    uint32_t step = dma_width; /* 2bytyes or 4bytes */
    uint32_t frames = (len / 2 / dma_width);

    if (play_sync > 0) {
        if (likely(snd_aecv2_is_enable())) {
            for (i = 0; i < frames; i++) {
                simple_copy(p, p_ply_rd, step);
                p += 2 * step;
                p_ply_rd += step;
                if (p_ply_rd >= ply_buf_end) {
                    p_ply_rd = ply_buf_start;
                }
            }
        } else {
            for (i = 0; i < frames; i++) {
                p_ply_rd += step; /* just consume it, not fill  */
                if (p_ply_rd >= ply_buf_end) {
                    p_ply_rd = ply_buf_start;
                }
            }
        }
        play_sync--;
    } else { /* play_sync <= 0 */
        if (likely(snd_aecv2_is_enable())) {
            for (i = 0; i < frames; i++) {
                simple_assign(p, 0, step); /* fill left chans with 0 */
                p += 2 * step;
            }
        }
    }
}

void dump_capture_ldata(uint8_t *pos, uint32_t len)
{
    if(!pos || !p_ply_rd) {
        return;
    }
    /*	snd_aecv2_status_show(); */
    fill_left_chan(pos, len);
}
EXPORT_SYMBOL(dump_capture_ldata);


static void capture_fill_right_chan(uint8_t *pos, uint32_t len)
{
    int i = 0;
    uint8_t *p = pos;
    uint32_t step = dma_width; /* 2bytyes or 4bytes */
    uint32_t frames = (len / 2 / dma_width);

    p = pos + step; /* point right chan; LRLRLR */
    for (i = 0; i < frames; i++) {
        simple_copy(p_cap_wt, p, step);
        p_cap_wt += step;
        p += 2 * step;
        if (p_cap_wt >= cap_buf_end) {
            p_cap_wt = cap_buf_start;
        }
    }

    p = (pos + step); /* point right chan; LRLRLR */
    if (likely(snd_aecv2_is_enable())) {
        for (i = 0; i < frames; i++) {
            simple_copy(p, p_cap_rd, step);
            p += 2 * step;
            p_cap_rd += step;
            if (p_cap_rd >= cap_buf_end) {
                p_cap_rd = cap_buf_start;
            }
        }
    } else {
        p_cap_rd += step; /* just consume it */
        if (p_cap_rd >= cap_buf_end) {
            p_cap_rd = cap_buf_start;
        }
    }
}

void dump_capture_rdata(uint8_t *pos, uint32_t len)
{
    if(!pos || !p_cap_wt || !p_cap_rd || !cap_buf_start)
        return;

    /*  snd_aecv2_status_show(); */
    capture_fill_right_chan(pos, len);
}
EXPORT_SYMBOL(dump_capture_rdata);

void dump_playback_data(uint8_t *pos, uint32_t len)
{
    int i = 0;
    uint8_t *p = pos;
    uint32_t step = dma_width; /* 2bytyes or 4bytes */
    uint32_t frames = (len / 2 / dma_width);

    for (i = 0; i < frames; i++) {
        simple_copy(p_ply_wt, p, step);
        p_ply_wt += step;
        p += 2 * step;

        if (p_ply_wt >= ply_buf_end)
            p_ply_wt = ply_buf_start;
    }
    play_sync++;
    if (play_sync > SAVED_PERIOD) { //too much latency acummulated, shift toward front
        p_ply_rd += (play_sync - 1) * step * frames;
        if (p_ply_rd >= ply_buf_end)
            p_ply_rd -= (ply_buf_end - ply_buf_start) * step;
        play_sync = 1;
    }
}

#if defined(CONFIG_SND_USE_AECV2)
void aec_capture_stream(struct snd_pcm_substream *substream)
{
    struct imapx_runtime_data *prtd = substream->runtime->private_data;
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        char *hwbuf = substream->runtime->dma_area + (prtd->dma_pos - prtd->dma_start);

        if(play_sync > 0 && wait_play_period) {

            hwbuf = hwbuf - prtd->dma_period;
            if(hwbuf < substream->runtime->dma_area) {
                hwbuf = hwbuf + (prtd->dma_end - prtd->dma_start);
            }
            dump_capture_ldata(hwbuf, prtd->dma_period);/* place spk data to left-channel*/

            wait_play_period = 0;
            hwbuf = hwbuf + prtd->dma_period; /* out of dma area, turn around ??? */
            miss_play_period = 0;
        }

        if(play_sync <= 0 && ++miss_play_period <= 1) {
            wait_play_period = 1;
        }
        else if(miss_play_period > 1){
            wait_play_period = 0;
        }
        else if(play_sync > 0) {
            miss_play_period = 0;
        }

        dump_capture_ldata(hwbuf, prtd->dma_period);
        dump_capture_rdata(hwbuf, prtd->dma_period);
        capture_dma_done = 1;
    }

    if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        char *pbuf = substream->runtime->dma_area + (prtd->dma_pos - prtd->dma_start);
        dump_playback_data(pbuf, prtd->dma_period); /* save playback data to buf, if play too much, drop old data  */
    }
}
#else
void aec_capture_stream(struct snd_pcm_substream *substream)
{
}
#endif
EXPORT_SYMBOL(aec_capture_stream);


#if defined(CONFIG_USE_AECV2)
void aec_pcm_trigger_start(struct snd_pcm_substream *substream)
{
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        capture_dma_enable = 1;
    }

    if (capture_dma_enable && substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        capture_dma_done = 0;
        while (!capture_dma_done) {
            usleep_range(1000, 1000);
        }
        capture_dma_done = 0;
    }
}

void aec_pcm_trigger_stop(struct snd_pcm_substream *substream)
{
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        capture_dma_enable = 0;
    }
}
#else
void aec_pcm_trigger_start(struct snd_pcm_substream *substream)
{
}

void aec_pcm_trigger_stop(struct snd_pcm_substream *substream)
{
}
#endif
EXPORT_SYMBOL(aec_pcm_trigger_start);
EXPORT_SYMBOL(aec_pcm_trigger_stop);


#if defined(CONFIG_SND_USE_AECV2)

extern void imapx_pcm_enqueue(struct snd_pcm_substream *substream);

int imapx_pcm_update(struct snd_pcm_substream *substream)
{
	struct imapx_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;

	/* return if this is a bufferless transfer e.g.
	 * codec <--> BT codec or GSM modem -- lg FIXME */
	if (!prtd->params)
		return 0;

	prtd->state &= ~ST_RUNNING;
	prtd->params->ops->stop(prtd->params->ch);
	/* flush the DMA channel */
	prtd->params->ops->flush(prtd->params->ch);
	prtd->dma_loaded = 0;

	/* enqueue dma buffers */
	imapx_pcm_enqueue(substream);
	prtd->state |= ST_RUNNING;
	prtd->params->ops->trigger(prtd->params->ch);
	return ret;
}
#else
int imapx_pcm_update(struct snd_pcm_substream *substream)
{
    return 0;
}
#endif
EXPORT_SYMBOL(imapx_pcm_update);


#if defined(CONFIG_SND_USE_AECV2)
int aec_buf_prepare(struct snd_pcm_hw_params *params)
{
	uint8_t *ac_buf = NULL;
	unsigned long period = params_period_bytes(params);
	unsigned long old_ply_buf_size = ply_buf_size;
	unsigned long old_cap_buf_size = cap_buf_size;

	ply_buf_size = PLAYBACK_POOL(period);
	if((old_ply_buf_size != ply_buf_size) || (ply_buf_start == NULL)) {
		pr_err("****** need to malloc for playback buf ******\n");
		if(ply_buf_start)
			kfree(ply_buf_start);
		ply_buf_start = NULL;

		ac_buf = kzalloc(ply_buf_size, GFP_KERNEL);
		if (ac_buf == NULL) {
			pr_err("****** failed to malloc for playback buf ******\n");
			return -1;
		}

		ply_buf_start = (uint8_t *)ac_buf;
		ply_buf_end = (uint8_t *)(ac_buf + ply_buf_size);
		play_sync = 0;
		p_ply_wt = ply_buf_start;
		p_ply_rd = ply_buf_start;

	}

	complement = DELAY_COMPLEMENT(period) & (~(4UL-1)); //align to 4 bytes
	cap_buf_size = (complement + period + ALLOWANCE(period)) & (~(4UL-1)); //align to 4 bytes
	if((old_cap_buf_size != cap_buf_size) || (cap_buf_start == NULL)) {
		pr_err("****** need to malloc for capture buf ******\n");
		if(cap_buf_start)
			kfree(cap_buf_start);
		cap_buf_start = NULL;

		ac_buf = kzalloc(cap_buf_size, GFP_KERNEL);
		if (ac_buf == NULL) {
			pr_err("****** failed to malloc for capture buf ******\n");
			return -1;
		}

		cap_buf_start = (uint8_t *)ac_buf;
		cap_buf_end = (uint8_t *)(ac_buf + cap_buf_size);
		p_cap_rd = (uint8_t *)ac_buf;
		p_cap_wt = (uint8_t *)(ac_buf + complement); //write <complement> 0s to capture buf
	}
    return 0;
}
#else
int aec_buf_prepare(struct snd_pcm_hw_params *params)
{
    return 0;
}
#endif
EXPORT_SYMBOL(aec_buf_prepare);


static ssize_t aecv2_ctrl_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    if (aecv2_control) {
        printk("=====> Kernel: AECV2 is enable ======= \n");
    } else {
        printk("=====> Kernel: AECV2 is disable ======= \n");
    }
    return 0;
}
static ssize_t aecv2_ctrl_store(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    uint32_t data = simple_strtoul(buf, NULL, 16);
    aecv2_control = !!data;
    if (aecv2_control) {
        printk("=====> Kernel: start AECV2 ======= \n");
    } else {
        printk("=====> Kernel: stop AECV2 ======= \n");
    }
    return count;
}
static DEVICE_ATTR(aecv2_ctrl, 0666, aecv2_ctrl_show, aecv2_ctrl_store);

#if defined(CONFIG_SND_USE_AECV2)
void aec_ctrlnode_create(struct device *device)
{
    device_create_file(device, &dev_attr_aecv2_ctrl);
}
#else
void aec_ctrlnode_create(struct device *device)
{
}
#endif
EXPORT_SYMBOL(aec_ctrlnode_create);
