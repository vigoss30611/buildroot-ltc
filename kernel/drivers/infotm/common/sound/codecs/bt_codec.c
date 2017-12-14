#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/io.h>
#include <mach/pad.h>
#include <mach/hardware.h>
#include <mach/audio.h>

#define AUDIO_NAME "BT"

#ifdef FR1023_DEBUG
#define dbg(format, arg...) \
    printk(KERN_DEBUG AUDIO_NAME ": " format "\n", ## arg)
#else
#define dbg(format, arg...) do {} while (0)
#endif
#define err(format, arg...) \
    printk(KERN_ERR AUDIO_NAME ": " format "\n", ## arg)
#define info(format, arg...) \
    printk(KERN_INFO AUDIO_NAME ": " format "\n", ## arg)
	
/* clock inputs */
#define BT_MCLK         0
#define BT_PCM_CLK      1

/* clock divider id's */
#define BT_PCMDIV       0
#define BT_BCLKDIV      1
#define BT_VXCLKDIV     2

#define BT_1536FS       1536
#define BT_1024FS       1024
#define BT_768FS        768
#define BT_512FS        512
#define BT_384FS        384
#define BT_256FS        256
#define BT_128FS        128	

enum data_mode bt_mode;	


static int bt_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
        int div_id, int div)
{
    return 0;
}

static int bt_set_dai_sysclk(struct snd_soc_dai *dai,
        int clk_id, unsigned int freq, int dir)
{
    struct snd_soc_codec *codec = dai->codec;

    switch (freq) {
        case 11289600:
        case 12000000:
        case 12288000:
        case 16934400:
        case 18432000:
            return 0;
    }
    return 0;
}

static int bt_set_dai_fmt(struct snd_soc_dai *codec_dai,
        unsigned int fmt)
{
    return 0;
}

static int bt_pcm_hw_params(struct snd_pcm_substream *substream,
        struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
    return 0;
}

static int bt_mute(struct snd_soc_codec *codec,
        enum snd_soc_bias_level level)
{
    return 0;
}

static int bt_set_bias_level(struct snd_soc_codec *codec,
        enum snd_soc_bias_level level)
{
    return 0;
}

#define bt_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
        SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |\
        SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 |\
        SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

#define bt_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
        SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops bt_ops = {
    .hw_params  = bt_pcm_hw_params,
    .set_fmt    = bt_set_dai_fmt,
    .set_sysclk = bt_set_dai_sysclk,
    .digital_mute = bt_mute,
    .set_clkdiv = bt_set_dai_clkdiv,
};

static struct snd_soc_dai_driver bt_dai = {
    .name = "bt_dai",
    .playback = {
        .stream_name = "Playback",
        .channels_min = 1,
        .channels_max = 2,
        .rates = bt_RATES,
        .formats = bt_FORMATS,
    },
    .capture = {
        .stream_name = "Capture",
        .channels_min = 1,
        .channels_max = 2,
        .rates = bt_RATES,
        .formats = bt_FORMATS,
    },
    .ops = &bt_ops
};

static int bt_suspend(struct snd_soc_codec *codec)
{
    struct bt_priv *bt = snd_soc_codec_get_drvdata(codec);
    int i;
    
    bt_set_bias_level(codec, SND_SOC_BIAS_OFF);

    return 0;
}

static int bt_resume(struct snd_soc_codec *codec)
{
    struct bt_priv *bt = snd_soc_codec_get_drvdata(codec);

    bt_set_bias_level(codec, SND_SOC_BIAS_ON);
   
    return 0;
}

static int bt_probe(struct snd_soc_codec *codec)
{
    bt_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

    return 0;
}

static int bt_remove(struct snd_soc_codec *codec)
{
    bt_set_bias_level(codec, SND_SOC_BIAS_OFF);
    return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_bt = {
    .probe      = bt_probe,
    .remove     = bt_remove,
    .suspend    = bt_suspend,
    .resume     = bt_resume,
    .set_bias_level = bt_set_bias_level,
};

static int imapx_bt_codec_probe(struct platform_device *pdev)
{
	int ret = 0;
	ret =  snd_soc_register_codec(&pdev->dev, &soc_codec_dev_bt, &bt_dai, 1);
	if (ret < 0) {
        err("[BT] cannot register codec\n");
    }
    return 0;
}

static int imapx_bt_codec_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver imapx_bt_codec_driver = {
	.driver = {
		.name = "bt",
		.owner = THIS_MODULE,
	},
	.probe = imapx_bt_codec_probe,
	.remove = imapx_bt_codec_remove,
};

static int __init bt_modinit(void)
{
	int ret = 0;
	return platform_driver_register(&imapx_bt_codec_driver);
}
module_init(bt_modinit);

static void __exit bt_modexit(void)
{
    platform_driver_unregister(&imapx_bt_codec_driver);
}
module_exit(bt_modexit);

MODULE_DESCRIPTION("Asoc BT codec driver");
MODULE_LICENSE("GPL");


static int imapx_bt_hw_params(struct snd_pcm_substream *substream,
        struct snd_pcm_hw_params *params)
{
    struct snd_soc_pcm_runtime *rtd = substream->private_data;
    struct snd_soc_dai *codec_dai = rtd->codec_dai;
    struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

    unsigned int pll_out = 0, bclk = 0;
    int ret = 0;
    unsigned int iisdiv = 0;

    printk("Entered %s, rate = %d\n", __func__, params_rate(params));
    printk("Entered %s, channel = %d\n", __func__, params_channels(params));
    printk("CPU_DAI name is %s\n", cpu_dai->name);

    switch (params_rate(params)) {
        case 8000:
            iisdiv = BT_256FS;
            pll_out = 12288000;
            break;
        case 11025:
            iisdiv = BT_1536FS;
            pll_out = 16934400;
            break;
        case 16000:
            iisdiv = BT_768FS;
            pll_out = 12288000;
            break;
        case 22050:
            iisdiv = BT_512FS;
            pll_out = 11289600;
            break;
        case 32000:
            iisdiv = BT_384FS;
            pll_out = 12288000;
            break;
        case 44100:
            iisdiv = BT_256FS;
            pll_out = 16934400;
            break;
        case 48000:
            iisdiv = BT_384FS;
            pll_out = 18432000;
            break;
        case 96000:
            iisdiv = BT_128FS;
            pll_out = 12288000;
            break;
    }
    if (iisdiv > BT_256FS){
		iisdiv = BT_256FS;
	}
        
    /* set codec DAI configuration */
    ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
            SND_SOC_DAIFMT_NB_IF | SND_SOC_DAIFMT_CBM_CFM);
    if (ret < 0) {
        err("codec dai set fmt err\n");
        return ret;
    }

    ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
            SND_SOC_DAIFMT_NB_IF | SND_SOC_DAIFMT_CBM_CFM);
    if (ret < 0) {
        err("cpu dai set fmt err\n");
        return ret;
    }

    ret = snd_soc_dai_set_sysclk(codec_dai, BT_MCLK,
            pll_out, SND_SOC_CLOCK_IN);
    if (ret < 0) {
        err("codec dai set sysclk err\n");
        return ret;
    }

    if (bt_mode == IIS_MODE) {
        ret = snd_soc_dai_set_sysclk(cpu_dai, iisdiv,
                params_rate(params), params_format(params));
    } else if (bt_mode == PCM_MODE) {
        ret = snd_soc_dai_set_sysclk(cpu_dai, params_channels(params),
                params_rate(params), params_format(params));
    }
    if (ret < 0) {
        err("cpu dai set sysclk err\n");
        return ret;
    }

    ret = snd_soc_dai_set_clkdiv(codec_dai, BT_BCLKDIV, bclk);
    if (ret < 0) {
        err("codec dai set BCLK err\n");
        return ret;
    }

    return 0;
}

static int imapx_bt_hw_free(struct snd_pcm_substream *substream)
{
    return 0;
}

static struct snd_soc_ops imapx_hifi_ops = {
    .hw_params = imapx_bt_hw_params,
    .hw_free = imapx_bt_hw_free,
};

static struct snd_soc_dai_link imapx_bt_dai[] = {
    [0] = {
        .name = "IMAPX BT",
        .stream_name = "soc-audio BT HiFi",
        .codec_dai_name = "bt_dai",
        .codec_name = "bt.0",
        .ops = &imapx_hifi_ops,
    },
};

static struct snd_soc_card bt = {
    .name       = "bt-pcm",
    .owner      = THIS_MODULE,
    .dai_link   = imapx_bt_dai,
    .num_links  = ARRAY_SIZE(imapx_bt_dai),
};

struct platform_device *imapx_bt_device = NULL;

int imapx_bt_init(char *codec_name, char *cpu_dai_name, enum data_mode data, int enable, int id)
{
	int ret = 0;
	
	printk("----%s-------%s----\n",__func__,cpu_dai_name);

	imapx_bt_device = platform_device_alloc("soc-audio", -1);
    if (!imapx_bt_device){
        return -ENOMEM;
	}
	
    imapx_bt_dai[0].cpu_dai_name = cpu_dai_name;
    imapx_bt_dai[0].platform_name = cpu_dai_name;
    platform_set_drvdata(imapx_bt_device, &bt);
	
	bt_mode = data;
	imapx_bt_device->id = id;

    ret = platform_device_add(imapx_bt_device);
    if (ret){
		platform_device_put(imapx_bt_device);
	}		
   
    return 0;
}

