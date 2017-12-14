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
** Revision History:
**     1.0  09/15/2009    James Xu
**	   2.0  20/12/2013	  Sun
*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <mach/hardware.h>
#include <linux/io.h>
#include <asm/dma.h>
#include <mach/imap-iomap.h>
#include <mach/imap-iis.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include <mach/audio.h>
#include <mach/items.h>
#include "../codecs/hpdetect.h"
#include "imapx-dma.h"
#include "imapx-i2s.h"
#if (defined(CONFIG_DISPLAY_IMAPX) || defined(CONFIG_INFOTM_DISPLAY2)) && defined(CONFIG_ARCH_APOLLO)
#include <audioParams.h>
#endif


struct imapx_i2s_info imapx_i2s;
static struct imapx_dma_client imapx_pch = {
	.name = "I2S PCM Stereo out"
};

static struct imapx_dma_client imapx_pch_in = {
	.name = "I2S PCM Stereo in"
};

static struct imapx_pcm_dma_params imapx_i2s_pcm_stereo_out = {
	.client		= &imapx_pch,
	.channel    = IMAPX_I2S_MASTER_TX,
	.dma_addr	= IMAP_IIS0_BASE + TXDMA,
	.dma_size	= 2,
};

static struct imapx_pcm_dma_params imapx_i2s_pcm_stereo_in = {
	.client		= &imapx_pch_in,
	.channel    = IMAPX_I2S_MASTER_RX,
	.dma_addr	= IMAP_IIS0_BASE + RXDMA,
	.dma_size	= 2,
};

static void imapx_snd_txctrl(int on)
{
	if (on) {
		/*
		 * 1. iis transmitter block enable register
		 * 2. iis transmitter enable register0
		 * 3. clock enable register
		 * 4. iis enable register
		 */
		writel(0x1, imapx_i2s.regs + ITER);
		writel(0x1, imapx_i2s.regs + TER0);
		writel(0x1, imapx_i2s.regs + CER);
		writel(0x1, imapx_i2s.regs + IER);
	} else {
		/*
		 * 1. flush the tx fifo
		 * 2. iis transmitter block enable register
		 * 3. iis transmitter enable register
		 */
		writel(0x1, imapx_i2s.regs + TFF0);
		writel(0x0, imapx_i2s.regs + ITER);
		writel(0x0, imapx_i2s.regs + TER0);
	}

}

void imapx_snd_rxctrl(int on)
{
	if (on) {
		writel(0x1, imapx_i2s.regs + IRER);
		writel(0x1, imapx_i2s.regs + RER0);
		writel(0x1, imapx_i2s.regs + CER);
		writel(0x1, imapx_i2s.regs + IER);
	} else {
		writel(0x1, imapx_i2s.regs + RFF0);
		writel(0x0, imapx_i2s.regs + IRER);
		writel(0x0, imapx_i2s.regs + RER0);
	}

}
EXPORT_SYMBOL(imapx_snd_rxctrl);
#if (defined(CONFIG_DISPLAY_IMAPX) || defined(CONFIG_INFOTM_DISPLAY2)) && defined(CONFIG_ARCH_APOLLO)
extern int  hdmi_set_audio_basic_config(audioParams_t *audio_t);
static int imapx_i2s_to_hdmi(struct snd_pcm_hw_params *params)
{
    audioParams_t audio_cfg;
	int format_value;

	// hdmi sample size only support 24, 20, 16
	switch (params_format(params)) {
	case 0:
	case 1:
		format_value = 16;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		format_value = 16;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		format_value = 24;
		break;
	}

    memset((void *)&audio_cfg, 0, sizeof(audioParams_t));

    audio_cfg.mSampleSize = format_value;
    audio_cfg.mSamplingFrequency = params_rate(params);

    hdmi_set_audio_basic_config(&audio_cfg);

    return 0;
}
#endif
static int imapx_i2s_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params,
		struct snd_soc_dai *socdai)
{
	uint32_t imr0, ccr, tcr0, rcr0, tfcr0, rfcr0;
	struct imapx_pcm_dma_params *dma_data;

	dev_dbg(socdai->dev, "substream->stream : %d, format is %d\n",
			substream->stream, params_format(params));

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dma_data = &imapx_i2s_pcm_stereo_out;
	else
		dma_data = &imapx_i2s_pcm_stereo_in;

	snd_soc_dai_set_dma_data(socdai, substream, dma_data);
#if defined(CONFIG_DISPLAY_IMAPX) && defined(CONFIG_ARCH_APOLLO)
       //for hdmi interface
       if(item_exist("dss.implementation.hdmi.en") && (item_integer("dss.implementation.hdmi.en",0) == 1))
    	       imapx_i2s_to_hdmi(params);
#endif
#if defined(CONFIG_INFOTM_DISPLAY2) && defined(CONFIG_ARCH_APOLLO)
	imapx_i2s_to_hdmi(params);
#endif
	imr0 = (0x1<<5) | (0x1<<1);
	writel(imr0, imapx_i2s.regs + IMR0);
	/*set fifo*/
	tfcr0 = 0xc;
	rfcr0 = 0x8;
	writel(tfcr0, imapx_i2s.regs + TFCR0);
	writel(rfcr0, imapx_i2s.regs + RFCR0);
	/*set bit mode and gata*/
	ccr = readl(imapx_i2s.regs + CCR);
	tcr0 = readl(imapx_i2s.regs + TCR0);
	rcr0 = readl(imapx_i2s.regs + RCR0);
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S8:
		ccr = 0x3 << 3;
		tcr0 = 0x0;
		rcr0 = 0x0;
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
#if defined(CONFIG_ARCH_IMAPX910) || defined(CONFIG_ARCH_APOLLO) || defined(CONFIG_ARCH_APOLLO3)
		ccr = 0x0 << 3;
#else
		ccr = 0x2 << 3;
#endif
		tcr0 = 0x02;
		rcr0 = 0x02;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		ccr = 0x2 << 3;
		tcr0 = 0x04;
		rcr0 = 0x04;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		ccr = 0x2 << 3;
		tcr0 = 0x05;
		rcr0 = 0x05;
		break;
	default:
		return -EINVAL;
	}

	writel(ccr, imapx_i2s.regs + CCR);
	writel(tcr0, imapx_i2s.regs + TCR0);
	writel(rcr0, imapx_i2s.regs + RCR0);

	/*by sun for disable iis 1,2,3*/
	writel(0, imapx_i2s.regs + RER1);
	writel(0, imapx_i2s.regs + TER1);
	writel(0, imapx_i2s.regs + RER2);
	writel(0, imapx_i2s.regs + TER2);
	writel(0, imapx_i2s.regs + RER3);
	writel(0, imapx_i2s.regs + TER3);

	return 0;

}

static int spkon_delay = 0;
static int AudioRunStatus = 0;
int imapx_audio_get_state(void)
{
	return AudioRunStatus;
}

static int imapx_i2s_trigger(struct snd_pcm_substream *substream,
		int cmd, struct snd_soc_dai *dai)
{
	int ret = 0;

	dev_dbg(dai->dev, "Entered %s: cmd = %x\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			imapx_snd_rxctrl(1);
		else if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			AudioRunStatus = 1;
#ifdef CONFIG_INFOTM_AUTO_ENABLE_SPK
			hpdetect_spk_on(1);
			msleep(spkon_delay);
#endif
			imapx_snd_txctrl(1);
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			imapx_snd_rxctrl(0);
		else if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			AudioRunStatus = 0;
			imapx_snd_txctrl(0);
#ifdef CONFIG_INFOTM_AUTO_ENABLE_SPK
			hpdetect_spk_on(0);
#endif
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int imapx_i2s_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	return 0;
}

int imapx_i2s_set_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int format)
{

	int mclkdiv, bitclkdiv, format_value = 0;
	int bit_clk, codec_clk;

	if (dai)
		dev_err(dai->dev,
				"Entered %s : clk_id = %d, freq is %d, format is %d\n",
				__func__, clk_id, freq, format);

	switch (format) {
	case 0:
	case 1:
		format_value = 8;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		format_value = 16;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		format_value = 32;
		break;
	}

	if (format_value > 16) {
		imapx_i2s_pcm_stereo_in.dma_size = 4;
		imapx_i2s_pcm_stereo_out.dma_size = 4;
	}
    imapx_dma_width_get(imapx_i2s_pcm_stereo_in.dma_size);

	codec_clk = clk_id * freq;
	while (imapx_i2s.rate < codec_clk) {
		freq = freq / 2;
		codec_clk = clk_id * freq;
	}
	bit_clk = freq * format_value * 2;
	codec_clk = clk_id * freq;
	mclkdiv = imapx_i2s.rate / codec_clk - 1;
#if defined(CONFIG_ARCH_IMAPX910) || defined(CONFIG_ARCH_APOLLO)
	bitclkdiv = (mclkdiv + 1) * (clk_id / (format_value * 2)) - 1;
#else
	bitclkdiv = (mclkdiv + 1) * (clk_id / (format_value * 2)) - 1;
#endif
	if (dai)
		dev_err(dai->dev, "mclkdiv = %d, bitclkdiv = %d\n",
				mclkdiv, bitclkdiv);

	writel(mclkdiv, imapx_i2s.mclk);
	writel(bitclkdiv, imapx_i2s.bclk);

#if defined(CONFIG_ARCH_IMAPX910) || defined(CONFIG_ARCH_APOLLO)
	writel(0x3, imapx_i2s.interface);
#else
	writel(0x6, imapx_i2s.interface);
#endif

	return 0;
}
EXPORT_SYMBOL(imapx_i2s_set_sysclk);


static void imapx_i2s_hw_init(void)
{
	module_power_on(SYSMGR_IIS_BASE);
#if !defined(CONFIG_ARCH_IMAPX910) && !defined(CONFIG_ARCH_APOLLO)
	imapx_pad_init("iis0");
#endif
}

static int imapx_i2s_probe(struct snd_soc_dai *dai)
{
	dev_err(dai->dev, "Entered %s\n", __func__);

	imapx_i2s.regs = ioremap(IMAP_IIS0_BASE, SZ_4K);
	if (imapx_i2s.regs == NULL) {
		dev_err(dai->dev, "failed to remap I/O memory\n");
		return -ENXIO;
	}

	return 0;
}

#ifdef CONFIG_PM
static int imapx_i2s_suspend(struct snd_soc_dai *dai)
{
	dev_dbg(dai->dev, "Entered %s\n", __func__);
	clk_disable_unprepare(imapx_i2s.clk);
	imapx_i2s.registers.ier		= readl(imapx_i2s.regs + IER);
	imapx_i2s.registers.irer	= readl(imapx_i2s.regs + IRER);
	imapx_i2s.registers.iter	= readl(imapx_i2s.regs + ITER);
	imapx_i2s.registers.cer		= readl(imapx_i2s.regs + CER);
	imapx_i2s.registers.ccr		= readl(imapx_i2s.regs + CCR);
	imapx_i2s.registers.mcr		= readl(imapx_i2s.mclk);
	imapx_i2s.registers.i2smode	= readl(imapx_i2s.interface);
	imapx_i2s.registers.bcr		= readl(imapx_i2s.bclk);
	imapx_i2s.registers.rer		= readl(imapx_i2s.regs + RER0);
	imapx_i2s.registers.ter		= readl(imapx_i2s.regs + TER0);
	imapx_i2s.registers.rcr		= readl(imapx_i2s.regs + RCR0);
	imapx_i2s.registers.tcr		= readl(imapx_i2s.regs + TCR0);
	imapx_i2s.registers.imr		= readl(imapx_i2s.regs + IMR0);
	imapx_i2s.registers.rfcr	= readl(imapx_i2s.regs + RFCR0);
	imapx_i2s.registers.tfcr	= readl(imapx_i2s.regs + TFCR0);

	return 0;
}

void enable_i2s_clk(void)
{
	clk_prepare_enable(imapx_i2s.clk);
}

void disable_i2s_clk(void)
{
	clk_disable_unprepare(imapx_i2s.clk);
}

void resume_i2s_clk(void)
{
	imapx_i2s_hw_init();
	clk_prepare_enable(imapx_i2s.clk);
	writel(imapx_i2s.registers.mcr, imapx_i2s.mclk);
	writel(imapx_i2s.registers.bcr, imapx_i2s.bclk);
	writel(imapx_i2s.registers.i2smode, imapx_i2s.interface);

	imapx_i2s_set_sysclk(NULL, 256, 48000, SNDRV_PCM_FORMAT_S16_LE);
}

int imapx_i2s_resume(struct snd_soc_dai *dai)
{
	dev_dbg(dai->dev, "Entered %s\n", __func__);
#if	!defined(CONFIG_ARCH_IMAPX910) && !defined(CONFIG_ARCH_APOLLO)
	resume_i2s_clk();
#endif
	writel(0x0, imapx_i2s.regs + IER);
	writel(0x0, imapx_i2s.regs + IRER);
	writel(0x0, imapx_i2s.regs + ITER);
	writel(0x0, imapx_i2s.regs + CER);
	writel(0x0, imapx_i2s.regs + RER1);
	writel(0x0, imapx_i2s.regs + TER1);
	writel(0x0, imapx_i2s.regs + RER2);
	writel(0x0, imapx_i2s.regs + TER2);
	writel(0x0, imapx_i2s.regs + RER3);
	writel(0x0, imapx_i2s.regs + TER3);
	writel(imapx_i2s.registers.ccr,		imapx_i2s.regs + CCR);
	writel(imapx_i2s.registers.rcr,		imapx_i2s.regs + RER0);
	writel(imapx_i2s.registers.ter,		imapx_i2s.regs + TER0);
	writel(imapx_i2s.registers.rcr,		imapx_i2s.regs + RCR0);
	writel(imapx_i2s.registers.tcr,		imapx_i2s.regs + TCR0);
	writel(imapx_i2s.registers.imr,		imapx_i2s.regs + IMR0);
	writel(imapx_i2s.registers.rfcr,	imapx_i2s.regs + RFCR0);
	writel(imapx_i2s.registers.tfcr,	imapx_i2s.regs + TFCR0);
	writel(imapx_i2s.registers.ier,		imapx_i2s.regs + IER);
	writel(imapx_i2s.registers.irer,	imapx_i2s.regs + IRER);
	writel(imapx_i2s.registers.iter,	imapx_i2s.regs + ITER);
	writel(imapx_i2s.registers.cer,		imapx_i2s.regs + CER);
	return 0;
}

#else
#define imapx_i2s_suspend	NULL
#define imapx_i2s_resume	NULL
#endif

static struct snd_soc_dai_ops imapx_i2s_dai_ops = {
	.trigger = imapx_i2s_trigger,
	.hw_params = imapx_i2s_hw_params,
	.set_sysclk = imapx_i2s_set_sysclk,
	.set_fmt = imapx_i2s_set_fmt,
};

#define IMAPX_I2S_FORMATS \
	(SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_LE | \
	 SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_driver imapx_i2s_dai = {
	.probe = imapx_i2s_probe,
	.suspend = imapx_i2s_suspend,
	.resume = imapx_i2s_resume,
	.playback = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = IMAPX_I2S_FORMATS},
	.capture = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = IMAPX_I2S_FORMATS},
	.ops = &imapx_i2s_dai_ops,
};

static const struct snd_soc_component_driver imapx_i2s_component = {
	.name	= "imapx-iis",
};

static int imapx_iis_dev_probe(struct platform_device *pdev)
{
	int ret;

	imapx_i2s_hw_init();
	imapx_i2s.mclk = ioremap(I2S_MCLKDIV, 4);
	imapx_i2s.bclk = ioremap(I2S_BCLKDIV, 4);
	imapx_i2s.interface = ioremap(I2S_INTERFACE, 4);
	if (imapx_i2s.mclk == NULL || imapx_i2s.bclk == NULL
			|| imapx_i2s.interface == NULL) {
		pr_err("failed to remap I/O memory\n");
		return -ENXIO;
	}
	imapx_i2s.clk = clk_get_sys("imap-iis","imap-iis");
	if (imapx_i2s.clk == NULL) {
		pr_err("failed to get audio bus clk\n");
		ret = -ENODEV;
		goto err1;
	}
	ret = clk_prepare_enable(imapx_i2s.clk);
	if(ret){
		pr_err("prepare audio bus clk fail\n");
		goto unprepare_clk;
	}

	imapx_i2s.clk = clk_get_sys("imap-audio-clk", "imap-audio-clk");
	if (imapx_i2s.clk == NULL) {
		pr_err("failed to get audio clk\n");
		ret = -ENODEV;
		goto err1;
	}
	ret = clk_prepare_enable(imapx_i2s.clk);
	if(ret){
		pr_err("prepare audio clk fail\n");
		goto unprepare_clk;
	}
#ifndef CONFIG_APOLLO_FPGA_PLATFORM
	imapx_i2s.rate = clk_get_rate(imapx_i2s.clk);
#else
	imapx_i2s.rate = CONFIG_FPGA_EXTEND_CLK;
#endif
#if defined(CONFIG_ARCH_IMAPX910) || defined(CONFIG_ARCH_APOLLO)
	imapx_i2s_set_sysclk(NULL, 256, 8000, SNDRV_PCM_FORMAT_S24_LE);
#endif

	ret = snd_soc_register_component(&pdev->dev, &imapx_i2s_component,
			&imapx_i2s_dai, 1);
	if (ret) {
		pr_err("failed to register the dai\n");
		ret = -ENODEV;
		goto err1;
	}

	ret = imapx_asoc_platform_probe(&pdev->dev);
	if (ret) {
		pr_err("failed to register the platform\n");
		ret = -ENODEV;
		goto err1;
	}

	spkon_delay = item_exist("sound.spkon_delay") ?
		item_integer("sound.spkon_delay", 0) : 0;

	return 0;
unprepare_clk:
	clk_disable_unprepare(imapx_i2s.clk);
	clk_put(imapx_i2s.clk);
err1:
	iounmap(imapx_i2s.mclk);
	iounmap(imapx_i2s.bclk);
	iounmap(imapx_i2s.interface);
	return ret;
}

static int imapx_iis_dev_remove(struct platform_device *pdev)
{
	imapx_asoc_platform_remove(&pdev->dev);
	snd_soc_unregister_component(&pdev->dev);
	return 0;
}

static struct platform_driver imapx_iis_driver = {
	.probe  = imapx_iis_dev_probe,
	.remove = imapx_iis_dev_remove,
	.driver = {
		.name = "imapx-i2s",
		.owner = THIS_MODULE,
	},
};
module_platform_driver(imapx_iis_driver);
EXPORT_SYMBOL(imapx_i2s);
MODULE_AUTHOR("Sun, sun.sun@infotmic.com.cn");
MODULE_DESCRIPTION("imapx I2S SoC Interface");
MODULE_LICENSE("GPL");
