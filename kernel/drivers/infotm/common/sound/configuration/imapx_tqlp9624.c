/* ***************************************************************************
 * ** linux-2.6.28.5/sound/soc/imapx/imapx_snd.c
 * **
 * ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
 * **
 * ** Use of Infotm's code is governed by terms and conditions
 * ** stated in the accompanying licensing statement.
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **************************************************************************/
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/gpio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <asm/mach-types.h>
#include <asm/hardware/scoop.h>
#include <mach/pad.h>
#include <mach/hardware.h>
#include <linux/io.h>
#include <mach/audio.h>
#include "../imapx/imapx-dma.h"
#include "../codecs/tqlp9624.h"

#ifdef SND_DEBUG
#define snd_dbg(x...) printk(x)
#else
#define snd_dbg(...)  do {} while (0)
#endif
#define snd_err(x...) printk(x)


static int imapx_tqlp9624_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;

	snd_dbg("Entered %s, rate = %d\n", __func__, params_rate(params));
	snd_dbg("Entered %s, channel = %d\n", __func__,
				params_channels(params));
	snd_dbg("CPU_DAI name is %s\n", cpu_dai->name);

	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		snd_err("codec dai set fmt err\n");
		return ret;
	}

	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_IF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		snd_err("cpu dai set fmt err\n");
		return ret;
	}

	ret = snd_soc_dai_set_sysclk(codec_dai, CLOCK_SEL_256,
			params_rate(params), params_format(params));
	if (ret < 0) {
		snd_err("codec dai set sysclk err\n");
		return ret;
	}

	ret = snd_soc_dai_set_sysclk(cpu_dai, 256,
			params_rate(params), params_format(params));
	if (ret < 0) {
		snd_err("cpu dai set sysclk err\n");
		return ret;
	}

	return 0;
}

static struct snd_soc_ops imapx_tqlp9624_ops = {
	.hw_params = imapx_tqlp9624_hw_params,
};

#ifdef CONFIG_SND_IMAPX_SOC_SPDIF
extern struct snd_soc_dai_link imapx_spdif_dai;
static  struct snd_soc_dai_link imapx_tqlp9624_spdif_dai[2] = {
	[0] = {
		.name = "IMAPX800 9624tqlp",
		.stream_name = "soc-audio tqlp9624 hifi",
		.codec_dai_name = "tqlp9624_dai",
		.codec_name = "tqlp9624.0",
		.ops = &imapx_tqlp9624_ops,
	},
};

static struct snd_soc_card tqlp9624_spdif = {
	.name		= "tqlp9624-iis",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_tqlp9624_spdif_dai,
	.num_links	= ARRAY_SIZE(imapx_tqlp9624_spdif_dai),
};
#endif

static  struct snd_soc_dai_link imapx_tqlp9624_dai[] = {
	[0] = {
		.name = "IMAPX800 9624tqlp",
		.stream_name = "soc-audio tqlp9624 hifi",
		.codec_dai_name = "tqlp9624_dai",
		.codec_name = "tqlp9624.0",
		.ops = &imapx_tqlp9624_ops,
	},
};

static struct snd_soc_card tqlp9624 = {
	.name		= "tqlp9624-iis",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_tqlp9624_dai,
	.num_links	= ARRAY_SIZE(imapx_tqlp9624_dai),
};

static struct platform_device *imapx_tqlp9624_device;

int imapx_tqlp9624_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret;

	imapx_tqlp9624_device = platform_device_alloc("soc-audio", -1);
	if (!imapx_tqlp9624_device)
		return -ENOMEM;

    if (enable == 1)
    {
#ifdef CONFIG_SND_IMAPX_SOC_SPDIF
        imapx_tqlp9624_spdif_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_tqlp9624_spdif_dai[0].platform_name = cpu_dai_name;
        memcpy(&imapx_tqlp9624_spdif_dai[1], &imapx_spdif_dai, sizeof(struct snd_soc_dai_link));
        platform_set_drvdata(imapx_tqlp9624_device, &tqlp9624_spdif);
#endif
    } else {
        imapx_tqlp9624_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_tqlp9624_dai[0].platform_name = cpu_dai_name;
        platform_set_drvdata(imapx_tqlp9624_device, &tqlp9624);
    }
    
    imapx_tqlp9624_device->id = id;

	ret = platform_device_add(imapx_tqlp9624_device);
	if (ret)
		platform_device_put(imapx_tqlp9624_device);

	return 0;
}

MODULE_AUTHOR("Sun");
MODULE_DESCRIPTION("ALSA SoC TQLP9624");
MODULE_LICENSE("GPL");
