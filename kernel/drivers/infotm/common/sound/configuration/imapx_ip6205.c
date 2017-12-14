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

#ifdef SND_DEBUG
#define snd_dbg(x...) printk(x)
#else
#define snd_dbg(...)  do {} while (0)
#endif
#define snd_err(x...) printk(x)

#define IP6205_1536FS	1536
#define IP6205_768FS	768
#define IP6205_256FS	256

static int imapx_ip6205_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;
	unsigned int pll_out = 0, bclk = 0, iisdiv = 0;

	snd_dbg("Entered %s, rate = %d\n", __func__, params_rate(params));
	snd_dbg("Entered %s, channel = %d\n", __func__,
			params_channels(params));
	snd_dbg("Entered %s, format = %s\n", __func__, params_format(params));
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

	switch (params_rate(params)) {
		case 8000:
			iisdiv = IP6205_1536FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
		case 16000:
			iisdiv = IP6205_768FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
		case 48000:
			iisdiv = IP6205_256FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
	}

	ret = cpu_dai->driver->ops->set_sysclk(cpu_dai, iisdiv,
			params_rate(params), params_format(params));
	if (ret < 0) {
		snd_err("cpu dai set sysclk err\n");
		return ret;
	}

	return 0;
}

static int imapx_ip6205_hw_free(struct snd_pcm_substream *substream)
{
	return 0;
}

static struct snd_soc_ops imapx_ip6205_ops = {
	.hw_params	= imapx_ip6205_hw_params,
	.hw_free	= imapx_ip6205_hw_free,
};

static struct snd_soc_dai_link imapx_ip6205_dai[] = {
	[0] = {
		.name		= "IP6205",
		.stream_name = "soc-audio ip6205 hifi",
		.codec_dai_name = "ip6205_dai",
		.codec_name = "ip6205.0",
		.ops = &imapx_ip6205_ops,
	},
};

static struct snd_soc_card ip6205 = {
	.name		= "ip6205-iis",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_ip6205_dai,
	.num_links	= ARRAY_SIZE(imapx_ip6205_dai),
};

static struct pltform_device *imapx_ip6205_device;

int imapx_ip6205_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret;

	snd_err("$$$$$$$ start %s $$$$$\n", __func__);

	imapx_ip6205_device = platform_device_alloc("soc-audio", -1);
	if (!imapx_ip6205_device)
		return -ENOMEM;

	if (enable == 1) {
	} else {
	}

	printk("codec_name %s\n", codec_name);

//	imapx_ip6205_dai[0].codec_name = codec_name;
	imapx_ip6205_dai[0].cpu_dai_name = cpu_dai_name;
	imapx_ip6205_dai[0].platform_name = cpu_dai_name;
	platform_set_drvdata(imapx_ip6205_device, &ip6205);

	ret = platform_device_add(imapx_ip6205_device);
	if (ret)
		platform_device_put(imapx_ip6205_device);

	return 0;
}


