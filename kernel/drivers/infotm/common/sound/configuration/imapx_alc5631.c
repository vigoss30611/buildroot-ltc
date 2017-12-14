/*****************************************************************************
** linux-2.6.28.5/sound/soc/imapx/imapx_alc5631.c
**
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
**     1.0  09/15/2009    James Xu
**     1.0  04/26/2011    Warits Wang
**	   2.0	12/26/2013	  Sun
*****************************************************************************/

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
#include <mach/audio.h>
#include <linux/io.h>
#include "../imapx/imapx-dma.h"

#define TAG		"Alc5631: "
#ifdef CONFIG_SND_DEBUG
#define imapx_dbg(x...)	printk(TAG""x)
#else
#define imapx_dbg(...)	do {} while (0)
#endif

enum data_mode rt5631_mode;

static int imapx_hifi_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned int pll_out = 0;
	int ret = 0;
	unsigned int iisdiv = 0;

	imapx_dbg("Entered %s, rate = %d\n", __func__, params_rate(params));
	imapx_dbg("Entered %s, channel = %d\n",
				__func__, params_channels(params));
	imapx_dbg("CPU_DAI name is %s\n", cpu_dai->name);
	/********************************************************************/
	switch (params_rate(params)) {
	case 8000:
		iisdiv = 1536;
		pll_out = 12288000;
		break;
	case 11025:
		iisdiv = 256;
		pll_out = 2822400;
		break;
	case 16000:
		iisdiv = 256;
		pll_out = 4096000;
		break;
	case 22050:
		iisdiv = 512;
		pll_out = 11289600;
		break;
	case 32000:
		iisdiv = 384;
		pll_out = 12288000;
		break;
	case 44100:
		iisdiv = 256;
		pll_out = 11289600;
		break;
	case 48000:
		iisdiv = 256;
		pll_out = 12288000;
		break;
	case 96000:
		iisdiv = 128;
		pll_out = 12288000;
		break;
	}
	/* set codec DAI configuration */
	if (rt5631_mode == IIS_MODE) {
		ret = codec_dai->driver->ops->set_fmt(codec_dai,
				SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS);
	} else if (rt5631_mode == PCM_MODE) {
		ret = codec_dai->driver->ops->set_fmt(codec_dai,
				SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS);
	}
	if (ret < 0)
		return ret;

	ret = cpu_dai->driver->ops->set_fmt(cpu_dai,
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_IF |
			SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0)
		return ret;

	ret = codec_dai->driver->ops->set_sysclk(codec_dai, 0, pll_out,
			SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	if (rt5631_mode == IIS_MODE) {
		ret = cpu_dai->driver->ops->set_sysclk(cpu_dai, iisdiv,
				params_rate(params), params_format(params));
	} else if (rt5631_mode == PCM_MODE) {
		ret = cpu_dai->driver->ops->set_sysclk(cpu_dai,
				params_channels(params), params_rate(params),
				params_format(params));
	}
	if (ret < 0)
		return ret;

	return 0;
}

static struct snd_soc_ops imapx_hifi_ops = {
	.hw_params = imapx_hifi_hw_params,
};

static const struct snd_soc_dapm_widget rt5631_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Mic Bias", NULL),
	SND_SOC_DAPM_LINE("Line In Jack", NULL),
};

static const struct snd_soc_dapm_route audio_map[] = {

	{"Headphone Jack", NULL, "HPOL"},
	{"Headphone Jack", NULL, "HPOR"},

	{ "MIC2", NULL, "MIC Bias2" },
	{ "MIC Bias2", NULL, "Mic Bias" },

	{"MIC1", NULL, "MIC Bias1"},
	{"MIC Bias1", NULL, "Line In Jack"},

};

static const struct snd_kcontrol_new rt5631_imapx_controls[] = {

};

static int rt5631_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;


	/* Add imapx specific widgets */
	snd_soc_dapm_new_controls(dapm, rt5631_dapm_widgets,
			ARRAY_SIZE(rt5631_dapm_widgets));

	/* add imapx specific controls */
	snd_soc_add_codec_controls(codec, rt5631_imapx_controls,
			ARRAY_SIZE(rt5631_imapx_controls));

	snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));
	/* always connected */
	snd_soc_dapm_enable_pin(dapm, "Mic Bias");
	snd_soc_dapm_enable_pin(dapm, "Headphone Jack");
	snd_soc_dapm_enable_pin(dapm, "Line In Jack");
	snd_soc_dapm_sync(dapm);
	return 0;
}

#ifdef CONFIG_SND_IMAPX_SOC_SPDIF
extern struct snd_soc_dai_link imapx_spdif_dai;
struct snd_soc_dai_link imapx_rt5631_spdif_dai[2] = {
	[0] = {
		.name = "RT5631",
		.stream_name = "soc-audio RT5631 HiFi",
		.codec_dai_name = "rt5631-hifi",
		.platform_name = "imapx-audio",
		.init = rt5631_init,
		.ops = &imapx_hifi_ops,
	},
};

static struct snd_soc_card rt5631_spdif = {
	.name		= "rt5631",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_rt5631_spdif_dai,
	.num_links	= ARRAY_SIZE(imapx_rt5631_spdif_dai),
};
#endif

struct snd_soc_dai_link imapx_rt5631_dai[] = {
	[0] = {
		.name = "RT5631",
		.stream_name = "soc-audio RT5631 HiFi",
		.codec_dai_name = "rt5631-hifi",
		.platform_name = "imapx-audio",
		.init = rt5631_init,
		.ops = &imapx_hifi_ops,
	},
};

static struct snd_soc_card rt5631 = {
	.name		= "rt5631",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_rt5631_dai,
	.num_links	= ARRAY_SIZE(imapx_rt5631_dai),
};

static struct platform_device *imapx_rt5631_device;

int imapx_rt5631_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret;

	imapx_rt5631_device = platform_device_alloc("soc-audio", -1);
	if (!imapx_rt5631_device)
		return -ENOMEM;

    if (enable == 1)
    {
#ifdef CONFIG_SND_IMAPX_SOC_SPDIF
        imapx_rt5631_spdif_dai[0].codec_name = codec_name;
        imapx_rt5631_spdif_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_rt5631_spdif_dai[0].platform_name = cpu_dai_name;
        memcpy(&imapx_rt5631_spdif_dai[1], &imapx_spdif_dai, sizeof(struct snd_soc_dai_link));
        platform_set_drvdata(imapx_rt5631_device, &rt5631_spdif);
#endif
    } else {
        imapx_rt5631_dai[0].codec_name = codec_name;
        imapx_rt5631_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_rt5631_dai[0].platform_name = cpu_dai_name;
        platform_set_drvdata(imapx_rt5631_device, &rt5631);
    }

	rt5631_mode = data;
	
	imapx_rt5631_device->id = id;

	ret = platform_device_add(imapx_rt5631_device);
	if (ret)
		platform_device_put(imapx_rt5631_device);

	return 0;
}

MODULE_AUTHOR("Sun");
MODULE_DESCRIPTION("ALSA SoC RT5631");
MODULE_LICENSE("GPL");
