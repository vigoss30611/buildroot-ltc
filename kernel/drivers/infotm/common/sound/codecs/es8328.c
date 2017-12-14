/*
 * es8328.c -- es8328 ALSA SoC audio driver
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <richard@openedhand.com>
 *
 * Based on es8328.c
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
#include <mach/hw_cfg.h>
#include <mach/audio.h>
#include <mach/pad.h>
#include "es8328.h"

#define AUDIO_NAME "es8328"
#define es8328_VERSION "v0.12"

static int mute_initial;
static struct snd_soc_codec *ES8328_codec;
/*
 * Debug
 */

#ifdef es8328_DEBUG
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
struct es8328_priv {
	enum snd_soc_control_type control_type;
	unsigned int sysclk;
	struct regulator *regulator;
	struct codec_cfg *cfg;
};
static unsigned es8328_sysclk;

/*
 * es8328 register cache
 * We can't read the es8328 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static const u16 es8328_reg[] = {
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
 * read es8328 register cache
 */
static inline unsigned int es8328_read_reg_cache(struct snd_soc_codec *codec,
		unsigned int reg)
{
	u16 *cache = codec->reg_cache;
#ifndef	CONFIG_HHTECH_MINIPMP
	if (reg > es8328_CACHE_REGNUM)
#else
	if (reg > ARRAY_SIZE(es8328_reg))
#endif
		return -1;
	return cache[reg];
}

/*
 * write es8328 register cache
 */
static inline void snd_soc_write_reg_cache(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
#ifndef	CONFIG_HHTECH_MINIPMP
	if (reg > es8328_CACHE_REGNUM)
#else
	if (reg > ARRAY_SIZE(es8328_reg))
#endif
		return;
	cache[reg] = value;
}

static const struct snd_kcontrol_new es8328_snd_controls[] = {

};

static const struct snd_soc_dapm_widget es8328_dapm_widgets[] = {

};

static const struct snd_soc_dapm_route audio_map[] = {
};

static int es8328_add_widgets(struct snd_soc_codec *codec)
{
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	snd_soc_dapm_new_controls(dapm, es8328_dapm_widgets,
			ARRAY_SIZE(es8328_dapm_widgets));
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

	dbg(KERN_ERR "es8328: could not get coeff for mclk %d @ rate %d\n",
			mclk, rate);
	return -EINVAL;
}

static int es8328_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	return 0;
}

static int es8328_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
#ifndef	CONFIG_HHTECH_MINIPMP
	struct snd_soc_codec *codec = dai->codec;
	struct es8328_priv *es8328 = snd_soc_codec_get_drvdata(codec);
#endif

	switch (freq) {
	case 11289600:
	case 12000000:
	case 12288000:
	case 16934400:
	case 18432000:
#ifndef	CONFIG_HHTECH_MINIPMP
		es8328->sysclk = freq;
#else
	case 24576000:
		es8328_sysclk = freq;
#endif
		return 0;
	}
	return -EINVAL;
}

static int es8328_set_dai_fmt(struct snd_soc_dai *codec_dai,
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

static int es8328_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	return 0;
}

static int es8328_mute(struct snd_soc_dai *dai, int mute)
{
	if (mute_initial == 0)
		mute_initial = 1;

	/* comment by mhfan */
	return 0;
}

static int es8328_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
	return 0;
}

#define es8328_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
		SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 | \
		SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 | \
		SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

#define es8328_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
		SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops es8328_ops = {
	.hw_params = es8328_pcm_hw_params,
	.set_fmt = es8328_set_dai_fmt,
	.set_sysclk = es8328_set_dai_sysclk,
	.digital_mute = es8328_mute,
	.set_clkdiv = es8328_set_dai_clkdiv,
};


static struct snd_soc_dai_driver es8328_dai = {
	.name = "es8328_dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = es8328_RATES,
		.formats = es8328_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = es8328_RATES,
		.formats = es8328_FORMATS,},
	.ops = &es8328_ops,
};

static int es8328_suspend(struct snd_soc_codec *codec)
{

	es8328_set_bias_level(codec, SND_SOC_BIAS_OFF);
	/* add for MID suspend depop in earphone */
	snd_soc_write(codec, ES8328_DACCONTROL3, 0x06);
	snd_soc_write(codec, ES8328_CONTROL2, 0x58);
	snd_soc_write(codec, ES8328_CONTROL1, 0x32);
	snd_soc_write(codec, ES8328_ADCPOWER, 0xff);
	snd_soc_write(codec, ES8328_CHIPPOWER, 0xf3);
	snd_soc_write(codec, ES8328_DACPOWER, 0xc0);
	snd_soc_write(codec, ES8328_CONTROL1, 0x34);
	msleep(100);
	return 0;
}

static int es8328_codec_init(struct snd_soc_codec *codec)
{
	struct es8328_priv *es8328 = snd_soc_codec_get_drvdata(codec);
	struct codec_cfg *cfg = es8328->cfg;

	snd_soc_write(codec, ES8328_DACCONTROL3, 0x22);
	/*
	 * reset to default value.for i2c_shutdown will set to 0x06,
	 * but codec init never reset to default value. while cause
	 * no audio issue when after charger mode boot MID.
	 */
	es8328_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
#ifdef	CONFIG_HHTECH_MINIPMP
	snd_soc_write(codec, ES8328_MASTERMODE, 0x00);
	snd_soc_write(codec, ES8328_CHIPPOWER, 0xf3);
	snd_soc_write(codec, ES8328_CONTROL1, 0x02);
	snd_soc_write(codec, ES8328_CONTROL2, 0x040);
	snd_soc_write(codec, ES8328_ADCPOWER, 0x0000);
	snd_soc_write(codec, ES8328_DACPOWER,  0x30);
	snd_soc_write(codec, ES8328_ADCCONTROL1,  0x88);
	snd_soc_write(codec, ES8328_ADCCONTROL2,  0xf0);
	snd_soc_write(codec, ES8328_ADCCONTROL3,  0x82);
#endif
	snd_soc_write(codec, ES8328_DACCONTROL21, 0x80);
	if (cfg->i2s_mode == IIS_MODE)
		snd_soc_write(codec, ES8328_ADCCONTROL4,  0x4c);
	else if (cfg->i2s_mode == PCM_MODE)
		snd_soc_write(codec, ES8328_ADCCONTROL4,  0x2F);
	snd_soc_write(codec, ES8328_ADCCONTROL5,  0x02);
	snd_soc_write(codec, ES8328_ADCCONTROL7,  0xf0);
	snd_soc_write(codec, ES8328_ADCCONTROL8, 0x00);
	snd_soc_write(codec, ES8328_ADCCONTROL9, 0x00);
	snd_soc_write(codec, ES8328_ADCCONTROL10,  0xea);
	snd_soc_write(codec, ES8328_ADCCONTROL11, 0xa0);
	snd_soc_write(codec, ES8328_ADCCONTROL12, 0x12);
	snd_soc_write(codec, ES8328_ADCCONTROL13, 0x06);
	snd_soc_write(codec, ES8328_ADCCONTROL14, 0x53);
	if (cfg->i2s_mode == IIS_MODE)
		snd_soc_write(codec, ES8328_DACCONTROL1, 0x18);
	else if (cfg->i2s_mode == PCM_MODE)
		snd_soc_write(codec, ES8328_DACCONTROL1, 0x5E);
	snd_soc_write(codec, ES8328_DACCONTROL2, 0x02);
	snd_soc_write(codec, ES8328_DACCONTROL4, 0x08);
	snd_soc_write(codec, ES8328_DACCONTROL5, 0x08);
	snd_soc_write(codec, 0x1E, 0x01);
	snd_soc_write(codec, 0x1F, 0x86);
	snd_soc_write(codec, 0x20, 0x37);
	snd_soc_write(codec, 0x21, 0xCF);
	snd_soc_write(codec, 0x22, 0x20);
	snd_soc_write(codec, 0x23, 0x6C);
	snd_soc_write(codec, 0x24, 0xAD);
	snd_soc_write(codec, 0x25, 0xFF);
	snd_soc_write(codec, ES8328_DACCONTROL16, 0x09);
	snd_soc_write(codec, ES8328_DACCONTROL17, 0xb8);
	snd_soc_write(codec, ES8328_DACCONTROL18, 0x38);
	snd_soc_write(codec, ES8328_DACCONTROL19, 0x38);
	snd_soc_write(codec, ES8328_DACCONTROL20, 0xb8);
	snd_soc_write(codec, ES8328_CHIPPOWER, 0x00);
	snd_soc_write(codec, ES8328_CONTROL1, 0x06);
	snd_soc_write(codec, ES8328_DACPOWER, 0x30);
	snd_soc_write(codec, ES8328_DACCONTROL24, 0x1e);
	snd_soc_write(codec, ES8328_DACCONTROL25, 0x1e);

	return 0;
}

static int es8328_resume(struct snd_soc_codec *codec)
{
	es8328_set_bias_level(codec, SND_SOC_BIAS_ON);
	es8328_codec_init(codec);
	return 0;
}

static int es8328_probe(struct snd_soc_codec *codec)
{
	struct es8328_priv *es8328 = snd_soc_codec_get_drvdata(codec);
	int ret = 0;
	info("Audio Codec Driver %s", es8328_VERSION);
	ES8328_codec = codec;
	mute_initial = 0;
	ret = snd_soc_codec_set_cache_io(codec, 8, 8, es8328->control_type);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}
	es8328_codec_init(codec);
	es8328_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	snd_soc_add_codec_controls(codec, es8328_snd_controls,
			ARRAY_SIZE(es8328_snd_controls));
	es8328_add_widgets(codec);

	return ret;
}

/* power down chip */
static int es8328_remove(struct snd_soc_codec *codec)
{
	es8328_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8328 = {
	.probe =    es8328_probe,
	.remove =   es8328_remove,
	.suspend =  es8328_suspend,
	.resume =   es8328_resume,
	.set_bias_level = es8328_set_bias_level,
};

static int es8328_i2c_probe(struct i2c_client *i2c,
		const struct i2c_device_id *id)
{
	struct es8328_priv *es8328;
	struct codec_cfg *es8328_cfg;
	int ret;

	es8328_cfg = i2c->dev.platform_data;
	if (!es8328_cfg) {
		err("es8328 get platform_data failed\n");
		return -EINVAL;
	}
	es8328 = kzalloc(sizeof(struct es8328_priv), GFP_KERNEL);
	if (es8328 == NULL)
		return -ENOMEM;
	es8328->cfg = es8328_cfg;
	udelay(2000);
	i2c_set_clientdata(i2c, es8328);
	es8328->control_type = SND_SOC_I2C;

	ret = snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_es8328, &es8328_dai, 1);
	if (ret < 0) {
		err("cannot register codec\n");
		kfree(es8328);
	}
	return ret;
}

static int es8328_i2c_remove(struct i2c_client *client)
{

	snd_soc_unregister_codec(&client->dev);
	kfree(i2c_get_clientdata(client));
	return 0;
}

static void es8328_i2c_shutdown(struct i2c_client *client)
{
	struct snd_soc_codec *codec = ES8328_codec;

	snd_soc_write(codec, ES8328_DACCONTROL3, 0x06);
	snd_soc_write(codec, ES8328_CONTROL2, 0x58);
	snd_soc_write(codec, ES8328_CONTROL1, 0x32);
	snd_soc_write(codec, ES8328_ADCPOWER, 0xff);
	snd_soc_write(codec, ES8328_CHIPPOWER, 0xf3);
	snd_soc_write(codec, ES8328_DACPOWER, 0xc0);
	snd_soc_write(codec, ES8328_CONTROL1, 0x34);
	msleep(100);
}

static const struct i2c_device_id es8328_i2c_id[] = {
	{ "es8328", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es8328_i2c_id);

static struct i2c_driver es8328_i2c_driver = {
	.driver = {
		.name = "es8328",
		.owner = THIS_MODULE,
	},
	.probe    = es8328_i2c_probe,
	.remove   = es8328_i2c_remove,
	.shutdown = es8328_i2c_shutdown,
	.id_table = es8328_i2c_id,
};

static int __init es8328_modinit(void)
{
	return i2c_add_driver(&es8328_i2c_driver);
}
module_init(es8328_modinit);

static void __exit es8328_exit(void)
{
	i2c_del_driver(&es8328_i2c_driver);
}
module_exit(es8328_exit);

MODULE_DESCRIPTION("ASoC ES8328 driver");
MODULE_LICENSE("GPL");
