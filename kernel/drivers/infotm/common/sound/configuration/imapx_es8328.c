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
 **     1.0  09/15/2009    James Xu
 **     1.0  04/26/2011    Warits Wang
 ****************************************************************************/

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
#include "../codecs/es8328.h"
#include "../imapx/imapx-dma.h"

#ifdef CONFIG_SND_DEBUG
#define es8328_dbg(x...) printk(x)
#else
#define es8328_dbg(x...)
#endif

enum data_mode es8328_mode;

static int imapx_hifi_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	unsigned int pll_out = 0, bclk = 0;
	int ret = 0;
	unsigned int iisdiv = 0;

	es8328_dbg("Entered %s, rate = %d\n", __func__, params_rate(params));
	es8328_dbg("Entered %s, channel = %d\n", __func__,
			params_channels(params));
	es8328_dbg("CPU_DAI name is %s\n", cpu_dai->name);
	/********************************************************************/
	switch (params_rate(params)) {
	case 8000:
		iisdiv = ES8328_256FS;
		pll_out = 12288000;
		break;
	case 11025:
		iisdiv = ES8328_1536FS;
		pll_out = 16934400;
		break;
	case 16000:
		iisdiv = ES8328_768FS;
		pll_out = 12288000;
		break;
	case 22050:
		iisdiv = ES8328_512FS;
		pll_out = 11289600;
		break;
	case 32000:
		iisdiv = ES8328_384FS;
		pll_out = 12288000;
		break;
	case 44100:
		iisdiv = ES8328_256FS;
		pll_out = 16934400;
		break;
	case 48000:
		iisdiv = ES8328_384FS;
		pll_out = 18432000;
		break;
	case 96000:
		iisdiv = ES8328_128FS;
		pll_out = 12288000;
		break;
	}
	if (iisdiv > ES8328_256FS)
		iisdiv = ES8328_256FS;
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S
			| SND_SOC_DAIFMT_NB_IF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0)
		return ret;
	ret = cpu_dai->driver->ops->set_fmt(cpu_dai,
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_IF |
			SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0)
		return ret;

	ret = codec_dai->driver->ops->set_sysclk(codec_dai,
			ES8328_MCLK, pll_out, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	if (es8328_mode == IIS_MODE) {
		ret = cpu_dai->driver->ops->set_sysclk(cpu_dai, iisdiv,
				params_rate(params), params_format(params));
	} else if (es8328_mode == PCM_MODE) {
		ret = cpu_dai->driver->ops->set_sysclk(cpu_dai,
				params_channels(params), params_rate(params),
				params_format(params));
	}
	if (ret < 0)
		return ret;

	ret = codec_dai->driver->ops->set_clkdiv(codec_dai,
			ES8328_BCLKDIV, bclk);
	if (ret < 0)
		return ret;

	return 0;
}

static struct snd_soc_ops imapx_hifi_ops = {
	.hw_params = imapx_hifi_hw_params,
};

static const struct snd_soc_dapm_widget es8328_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Mic Bias", NULL),
	SND_SOC_DAPM_LINE("Line In Jack", NULL),
};


/* example machine audio_mapnections */
static const struct snd_soc_dapm_route audio_map[] = {

	{"Headphone Jack", NULL, "LOUT2"},
	{"Headphone Jack", NULL, "ROUT2"},

	{ "LINPUT2", NULL, "Mic Bias" },
	{ "Mic Bias", NULL, "Mic Jack" },

	{"LINPUT1", NULL, "Line In Jack"},
	{"RINPUT1", NULL, "Line In Jack"},

};

static const struct snd_kcontrol_new es8328_imapx_controls[] = {

};

static int es8328_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add imapx specific widgets */
	snd_soc_dapm_new_controls(dapm, es8328_dapm_widgets,
			ARRAY_SIZE(es8328_dapm_widgets));

	/* add imapx specific controls */
	snd_soc_add_codec_controls(codec, es8328_imapx_controls,
			ARRAY_SIZE(es8328_imapx_controls));

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
static struct snd_soc_dai_link imapx_es8328_spdif_dai[2] = {
	[0] = {
		.name = "IMPX800 ES8328",
		.stream_name = "soc-audio ES8328 HiFi",
		.codec_dai_name = "es8328_dai",
		.init = es8328_init,
		.ops = &imapx_hifi_ops,
	},
};

static struct snd_soc_card es8328_spdif = {
	.name		= "es8328",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_es8328_spdif_dai,
	.num_links	= ARRAY_SIZE(imapx_es8328_spdif_dai),
};
#endif

static struct snd_soc_dai_link imapx_es8328_dai[] = {
	[0] = {
		.name = "IMPX800 ES8328",
		.stream_name = "soc-audio ES8328 HiFi",
		.codec_dai_name = "es8328_dai",
		.init = es8328_init,
		.ops = &imapx_hifi_ops,
	},
};

static struct snd_soc_card es8328 = {
	.name		= "es8328",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_es8328_dai,
	.num_links	= ARRAY_SIZE(imapx_es8328_dai),
};

static struct platform_device *imapx_es8328_device;

int imapx_es8328_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret;

	imapx_es8328_device = platform_device_alloc("soc-audio", -1);
	if (!imapx_es8328_device)
		return -ENOMEM;

    if (enable == 1)
    {
#ifdef CONFIG_SND_IMAPX_SOC_SPDIF
        imapx_es8328_spdif_dai[0].codec_name = codec_name;
        imapx_es8328_spdif_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_es8328_spdif_dai[0].platform_name = cpu_dai_name;
        memcpy(&imapx_es8328_spdif_dai[1], &imapx_spdif_dai,sizeof(struct snd_soc_dai_link));
        platform_set_drvdata(imapx_es8328_device, &es8328_spdif);
#endif
    } else {
        imapx_es8328_dai[0].codec_name = codec_name;
        imapx_es8328_dai[0].cpu_dai_name = cpu_dai_name;
        imapx_es8328_dai[0].platform_name = cpu_dai_name;
        platform_set_drvdata(imapx_es8328_device, &es8328);
    }

	es8328_mode = data;
	imapx_es8328_device->id = id;

	ret = platform_device_add(imapx_es8328_device);
	if (ret)
		platform_device_put(imapx_es8328_device);

	return 0;
}

MODULE_AUTHOR("Sun");
MODULE_DESCRIPTION("ALSA SoC ES8328");
MODULE_LICENSE("GPL");
