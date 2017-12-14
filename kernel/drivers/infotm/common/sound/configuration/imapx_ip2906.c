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

#define IP2906_1536FS	1536
#define IP2906_768FS	768
#define IP2906_384FS	384
#define IP2906_256FS	256
#define IP2906_128FS	128
#define IP2906_64FS		64
#define IP2906_405FS	405
#define IP2906_810FS	810
#define IP2906_1620FS	1620
#define IP2906_648FS	648

static int imapx_ip2906_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;
	unsigned int pll_out = 0, bclk = 0, iisdiv = 0;

	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		printk("codec dai set fmt err\n");
		return ret;
	}

	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_IF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		printk("cpu dai set fmt err\n");
		return ret;
	}

	switch (params_rate(params)) {
		case 8000:
			iisdiv = IP2906_1536FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
		case 16000:
			iisdiv = IP2906_768FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
		case 32000:
			iisdiv = IP2906_384FS;
			pll_out = 12288000;
			break;
		case 48000:
			iisdiv = IP2906_256FS;
			pll_out = 12288000;
			bclk = pll_out / 4;
			break;
		case 96000:
			iisdiv = IP2906_128FS;
			pll_out = 12288000;
			break;
		case 192000:
			iisdiv = IP2906_64FS;
			pll_out = 12288000;
			break;
		case 88200:
			iisdiv = IP2906_405FS;
			break;
		case 44100:
			iisdiv = IP2906_810FS;
			break;	
		case 22050:
			iisdiv = IP2906_1620FS;
			break;
		case 11025:
			iisdiv = IP2906_648FS;
			break;
	}

	ret = cpu_dai->driver->ops->set_sysclk(cpu_dai, iisdiv,
			params_rate(params), params_format(params));
	if (ret < 0) {
		printk("cpu dai set sysclk err\n");
		return ret;
	}

	return 0;
}

static int imapx_ip2906_hw_free(struct snd_pcm_substream *substream)
{
	return 0;
}

static struct snd_soc_ops imapx_ip2906_ops = {
	.hw_params	= imapx_ip2906_hw_params,
	.hw_free	= imapx_ip2906_hw_free,
};

static struct snd_soc_dai_link imapx_ip2906_dai[] = {
	[0] = {
		.name	= "IP2906",
		.stream_name	= "soc-audio ip2906 hifi",
		.codec_dai_name	= "ip2906_dai",
//		.codec_name		= "ip2906.0",
		.ops	= &imapx_ip2906_ops,
	},
};

static struct snd_soc_card ip2906 = {
	.name		= "ip2906-iis",
	.owner		= THIS_MODULE,
	.dai_link	= imapx_ip2906_dai,
	.num_links	= ARRAY_SIZE(imapx_ip2906_dai),
};

static struct platform_device *imapx_ip2906_device;

int imapx_ip2906_init(char *codec_name,
		char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret;

	imapx_ip2906_device = platform_device_alloc("soc-audio", -1);
	if (!imapx_ip2906_device)
		return -ENOMEM;

	imapx_ip2906_dai[0].codec_name = codec_name;
	imapx_ip2906_dai[0].cpu_dai_name = cpu_dai_name;
	imapx_ip2906_dai[0].platform_name = cpu_dai_name;
	platform_set_drvdata(imapx_ip2906_device, &ip2906);

	ret = platform_device_add(imapx_ip2906_device);
	if (ret)
		platform_device_put(imapx_ip2906_device);

	return 0;
}
