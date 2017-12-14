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
#include "../imapx/imapx-spdif.h"

#ifdef SND_DEBUG
#define snd_dbg(x...) printk(x)
#else
#define snd_dbg(x...)
#endif
#define snd_err(x...) printk(x)

static int imapx_spdif_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;

	snd_dbg("Entered %s, rate = %d\n", __func__, params_rate(params));
	snd_dbg("Entered %s, channel = %d\n", __func__,
			params_channels(params));
	snd_dbg("CPU_DAI name is %s\n", cpu_dai->name);

	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_NB_IF |
			SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		snd_err("Cpu dai set fmt err\n");
		return ret;
	}
	ret = snd_soc_dai_set_sysclk(cpu_dai, params_channels(params),
			params_rate(params), params_format(params));
	if (ret < 0) {
		snd_err("Cpu dai set sysclk err\n");
		return ret;
	}
	ret = snd_soc_dai_set_clkdiv(cpu_dai, 0, 0);
	if (ret < 0) {
		snd_err("Cpu dai set clkdiv err\n");
		return ret;
	}

	return 0;
}

static struct snd_soc_ops imapx_spdif_ops = {
	.hw_params = imapx_spdif_hw_params,
};

struct snd_soc_dai_link imapx_spdif_dai = {
    .name = "IMAPX800 SPDIF",
    .stream_name = "spdif hifi",
    .codec_dai_name = "virtual-spdif",
    .cpu_dai_name = "imapx-spdif",
    .codec_name = "virtual-codec",
    .platform_name = "imapx-spdif",
    .ops = &imapx_spdif_ops,
};
EXPORT_SYMBOL(imapx_spdif_dai);
