/*
 * es8323.c -- es8323 ALSA SoC audio driver
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <richard@openedhand.com>
 *
 * Based on es8323.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/io.h>
#include "es8323.h"
#include "hpdetect.h"

#define AUDIO_NAME "es8323"
#define es8323_VERSION "v0.12"

#ifdef es8323_DEBUG
#define dbg(format, arg...) \
	dbg(KERN_DEBUG AUDIO_NAME ": " format "\n" , ## arg)
#else
#define dbg(format, arg...) do {} while (0)
#endif
#define err(format, arg...) \
	dbg(KERN_ERR AUDIO_NAME ": " format "\n" , ## arg)
#define info(format, arg...) \
	dbg(KERN_INFO AUDIO_NAME ": " format "\n" , ## arg)
#define warn(format, arg...) \
	dbg(KERN_WARNING AUDIO_NAME ": " format "\n" , ## arg)

/* codec private data */
struct es8323_priv {
	enum snd_soc_control_type control_type;
	unsigned int sysclk;
    struct delayed_work init_work;
	struct regulator *regulator;
    int spken;
};
static unsigned es8323_sysclk;
struct snd_soc_codec *es8323_codec;

/*
 * es8323 register cache
 * We can't read the es8323 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static const u16 es8323_reg[] = {
	0x00b7, 0x0097, 0x0000, 0x0000,  /*  0 */
	0x0000, 0x0008, 0x0000, 0x002a,  /*  4 */
	0x0000, 0x0000, 0x007F, 0x007F,  /*  8 */
	0x000f, 0x000f, 0x0000, 0x0000,  /* 12 */
	0x0080, 0x007b, 0x0000, 0x0032,  /* 16 */
	0x0000, 0x00E0, 0x00E0, 0x00c0,  /* 20 */
	0x0000, 0x000e, 0x0000, 0x0000,  /* 24 */
	0x0000, 0x0000, 0x0000, 0x0000,  /* 28 */
	0x0000, 0x0000, 0x0050, 0x0050,  /* 32 */
	0x0050, 0x0050, 0x0050, 0x0050,  /* 36 */
	0x0000, 0x0000, 0x0079,          /* 40 */
};

/*
 * read es8323 register cache
 */
static inline unsigned int es8323_read_reg_cache(struct snd_soc_codec *codec,
		unsigned int reg)
{
	u16 *cache = codec->reg_cache;
#ifndef	CONFIG_HHTECH_MINIPMP
	if (reg > es8323_CACHE_REGNUM)
#else
		if (reg > ARRAY_SIZE(es8323_reg))
#endif
			return -1;
	return cache[reg];
}

/*
 * write es8323 register cache
 */
static inline void snd_soc_write_reg_cache(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
#ifndef	CONFIG_HHTECH_MINIPMP
	if (reg > es8323_CACHE_REGNUM)
#else
		if (reg > ARRAY_SIZE(es8323_reg))
#endif
			return;
	cache[reg] = value;
}

static int es8323_spk_get(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
    struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);

    ucontrol->value.integer.value[0] = es8323->spken;

    return 0;
}

static int es8323_spk_put(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value * ucontrol)
{
    struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
    struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);

    es8323->spken = ucontrol->value.integer.value[0];
    hpdetect_spk_on(es8323->spken);

    return 0;
};

static const struct snd_kcontrol_new es8323_snd_controls[] = {
    SOC_SINGLE_EXT("Speaker Enable Switch", 0, 0, 1, 0,
            es8323_spk_get, es8323_spk_put),

};

static const struct snd_soc_dapm_widget es8323_dapm_widgets[] = {

};

static const struct snd_soc_dapm_route audio_map[] = {

};
static int es8323_add_widgets(struct snd_soc_codec *codec)
{
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	snd_soc_dapm_new_controls(dapm, es8323_dapm_widgets,
			ARRAY_SIZE(es8323_dapm_widgets));
	snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));
	snd_soc_dapm_new_widgets(dapm);
	return 0;
}

struct _coeff_div {
	u32 mclk;
	u32 rate;
	u16 fs;
	u8 sr:5;
	u8 usb:1;
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
	/* 8k */
	{12288000, 8000, 1536, 0x6, 0x0},
	{11289600, 8000, 1408, 0x16, 0x0},
	{18432000, 8000, 2304, 0x7, 0x0},
	{16934400, 8000, 2112, 0x17, 0x0},
	{12000000, 8000, 1500, 0x6, 0x1},

	/* 11.025k */
	{11289600, 11025, 1024, 0x18, 0x0},
	{16934400, 11025, 1536, 0x19, 0x0},
	{12000000, 11025, 1088, 0x19, 0x1},

	/* 16k */
	{12288000, 16000, 768, 0xa, 0x0},
	{18432000, 16000, 1152, 0xb, 0x0},
	{12000000, 16000, 750, 0xa, 0x1},

	/* 22.05k */
	{11289600, 22050, 512, 0x1a, 0x0},
	{16934400, 22050, 768, 0x1b, 0x0},
	{12000000, 22050, 544, 0x1b, 0x1},

	/* 32k */
	{12288000, 32000, 384, 0xc, 0x0},
	{18432000, 32000, 576, 0xd, 0x0},
	{12000000, 32000, 375, 0xc, 0x1},

	/* 44.1k */
	{11289600, 44100, 256, 0x10, 0x0},
	{16934400, 44100, 384, 0x11, 0x0},
	{12000000, 44100, 272, 0x11, 0x1},

	/* 48k */
	{12288000, 48000, 256, 0x0, 0x0},
	{18432000, 48000, 384, 0x1, 0x0},
	{12000000, 48000, 250, 0x0, 0x1},

	/* 88.2k */
	{11289600, 88200, 128, 0x1e, 0x0},
	{16934400, 88200, 192, 0x1f, 0x0},
	{12000000, 88200, 136, 0x1f, 0x1},

	/* 96k */
	{12288000, 96000, 128, 0xe, 0x0},
	{18432000, 96000, 192, 0xf, 0x0},
	{12000000, 96000, 125, 0xe, 0x1},
};

static inline int get_coeff(int mclk, int rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
			return i;
	}

	dbg(KERN_ERR "es8323: could not get coeff for mclk %d @ rate %d\n",
			mclk, rate);
	return -EINVAL;
}

static int es8323_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	return 0;
}

static int es8323_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
#ifndef	CONFIG_HHTECH_MINIPMP
	struct snd_soc_codec *codec = dai->codec;
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);
#endif

	switch (freq) {
	case 11289600:
	case 12000000:
	case 12288000:
	case 16934400:
	case 18432000:
#ifndef	CONFIG_HHTECH_MINIPMP
		es8323->sysclk = freq;
#else
	case 24576000:
		es8323_sysclk = freq;
#endif
		return 0;
	}
	return -EINVAL;
}

static int es8323_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	u16 iface = 0;

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		iface = 0x0040;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= 0x0002;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= 0x0001;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= 0x0003;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface |= 0x0013;
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface |= 0x0090;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= 0x0080;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface |= 0x0010;
		break;
	default:
		return -EINVAL;
	}

#ifdef	CONFIG_HHTECH_MINIPMP
	iface |= 0x20;
#endif
	return 0;
}

static int es8323_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	snd_soc_write(codec, ES8323_DACCONTROL17, 0xb8);
	snd_soc_write(codec, ES8323_DACCONTROL20, 0xb8);
	return 0;
}

static int es8323_mute(struct snd_soc_dai *dai, int mute)
{
	/* comment by mhfan */
	return 0;
}

static int es8323_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
	return 0;
}

#define es8323_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
		SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 | \
		SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 | \
		SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

#define es8323_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
		SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_ops es8323_ops = {
	.hw_params = es8323_pcm_hw_params,
	.set_fmt = es8323_set_dai_fmt,
	.set_sysclk = es8323_set_dai_sysclk,
	.digital_mute = es8323_mute,
	.set_clkdiv = es8323_set_dai_clkdiv,
};


static struct snd_soc_dai_driver es8323_dai = {
	.name = "es8323_dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = es8323_RATES,
		.formats = es8323_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = es8323_RATES,
		.formats = es8323_FORMATS,},
	.ops = &es8323_ops,
};

struct reg_val_t {
	unsigned char reg;
	unsigned char val;
};

struct reg_val_t init_val[] = {
	{0x02, 0xf3},
	{0x2B, 0x80},
	{0x08, 0x00},
	{0x00, 0x32},
	{0x01, 0x50},
	{0x03, 0x00},
	{0x05, 0x00},
	{0x06, 0x00},
	{0x07, 0x7c},
	//{0x09, 0x88},
	{0x09, 0x77},
    // difference
	//{0x0a, 0xf0},
	//{0x0b, 0x82},
	{0x0a, 0x50},
	{0x0b, 0x02},
	//{0x0C, 0x4c},
	{0x0C, 0x0c},
	{0x0d, 0x02},
	{0x10, 0x00},
	{0x11, 0x00},
	//{0x12, 0xea},
	{0x12, 0xa0},
	{0x13, 0xc0},
	{0x14, 0x05},
	{0x15, 0x06},
	//{0x16, 0x53},
	{0x16, 0x00},
	{0x17, 0x18},
	{0x18, 0x02},
	{0x1A, 0x00},
	{0x1B, 0x00},
	{0x26, 0x12},
	{0x27, 0xb8},
	{0x28, 0x38},
	{0x29, 0x38},
	{0x2A, 0xb8},
	{0x02, 0x00},
	{0x19, 0x02},
	{0x04, 0x0c},
	{0x2e, 0x00},
	{0x2f, 0x00},
	{0x30, 0x08},
	{0x31, 0x08},
	{0x2e, 0x08},
	{0x2f, 0x08},	
	{0x30, 0x0f},
	{0x31, 0x0f},
	{0x2e, 0x0f},
	{0x2f, 0x0f},		
	{0x30, 0x18},
	{0x31, 0x18},
	{0x2e, 0x18},
	{0x2f, 0x18},		
	{0x30, 0x1e},
	{0x31, 0x1e},
	{0x2e, 0x1e},
	{0x2f, 0x1e},		
	{0x04, 0x3c},
};

struct reg_val_t exit_val[] = {
	{0x19, 0x06},
	{0x01, 0x58},
	{0x00, 0x32},
	{0x03, 0xff},
	{0x02, 0xf3},
	{0x04, 0xc0},
	{0x00, 0x34},
};

static int es8323_suspend(struct snd_soc_codec *codec)
{
	int i;

    hpdetect_suspend();
	es8323_set_bias_level(codec, SND_SOC_BIAS_OFF);

	for (i = 0; i < (sizeof(exit_val)/2); ++i)
		snd_soc_write(codec, exit_val[i].reg, exit_val[i].val);

	return 0;
}

static int es8323_codec_init(struct snd_soc_codec *codec)
{
	int i, ret;
	es8323_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	for (i = 0; i < (sizeof(init_val)/2); ++i) {
		ret = snd_soc_write(codec, init_val[i].reg, init_val[i].val);
        if (ret < 0)
            return ret;
    }

	return 0;
}

static void codec_init_work(struct work_struct *work)
{
    struct es8323_priv *es8323 = container_of(to_delayed_work(work), struct es8323_priv, init_work);
    struct snd_soc_codec *codec = es8323_codec;
    int i, ret;
    
    ret = es8323_codec_init(codec);
    if (ret < 0)
        schedule_delayed_work(&es8323->init_work, msecs_to_jiffies(300));
    for (i = 0; i < (sizeof(init_val)/2); ++i)
        printk(KERN_INFO "******[es8323] register 0x%x : 0x%x.\n", init_val[i].reg, snd_soc_read(codec, init_val[i].reg));
}

static int es8323_resume(struct snd_soc_codec *codec)
{
    struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);
//    msleep(200);
    hpdetect_resume();
    schedule_delayed_work(&es8323->init_work, msecs_to_jiffies(300));
	es8323_set_bias_level(codec, SND_SOC_BIAS_ON);
//	es8323_codec_init(codec);
	return 0;
}


static int es8323_probe(struct snd_soc_codec *codec)
{
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);
	int ret = 0;
	info("Audio Codec Driver %s", es8323_VERSION);
	ret = snd_soc_codec_set_cache_io(codec, 8, 8, es8323->control_type);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}
    es8323_codec = codec;
	es8323_codec_init(codec);
	es8323_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	snd_soc_add_codec_controls(codec, es8323_snd_controls,
			ARRAY_SIZE(es8323_snd_controls));
	es8323_add_widgets(codec);
    INIT_DELAYED_WORK(&es8323->init_work, codec_init_work);
    
	return ret;
}

/* power down chip */
static int es8323_remove(struct snd_soc_codec *codec)
{
	es8323_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8323 = {
	.probe =    es8323_probe,
	.remove =   es8323_remove,
	.suspend =  es8323_suspend,
	.resume =   es8323_resume,
	.set_bias_level = es8323_set_bias_level,
};

static int es8323_i2c_probe(struct i2c_client *i2c,
		const struct i2c_device_id *id)
{
	struct es8323_priv *es8323;
    struct codec_cfg *es8323_cfg;
	int ret;

	es8323 = kzalloc(sizeof(struct es8323_priv), GFP_KERNEL);
	if (es8323 == NULL)
		return -ENOMEM;

    es8323_cfg = i2c->dev.platform_data;
    if (!es8323_cfg) {
        err("ES8323 get platform_data failed\n");
        return -EINVAL;
    }
    hpdetect_init(es8323_cfg);

	udelay(2000);
	i2c_set_clientdata(i2c, es8323);
	es8323->control_type = SND_SOC_I2C;

	ret = snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_es8323, &es8323_dai, 1);
	if (ret < 0) {
		err("cannot register codec\n");
		kfree(es8323);
	}
	return ret;
}

static int es8323_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	kfree(i2c_get_clientdata(client));
	return 0;
}

static const struct i2c_device_id es8323_i2c_id[] = {
	{ "es8323", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es8323_i2c_id);

static struct i2c_driver es8323_i2c_driver = {
	.driver = {
		.name = "es8323",
		.owner = THIS_MODULE,
	},
	.probe    = es8323_i2c_probe,
	.remove   = es8323_i2c_remove,
	.id_table = es8323_i2c_id,
};

static int __init es8323_modinit(void)
{
	return i2c_add_driver(&es8323_i2c_driver);
}
module_init(es8323_modinit);

static void __exit es8323_exit(void)
{
	i2c_del_driver(&es8323_i2c_driver);
}
module_exit(es8323_exit);

MODULE_DESCRIPTION("ASoC ES8323 driver");
MODULE_LICENSE("GPL");
