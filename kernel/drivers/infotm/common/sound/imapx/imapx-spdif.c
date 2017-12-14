#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <mach/imap_pl330.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include <linux/module.h>
#include "imapx-dma.h"
#include "imapx-spdif.h"

#define TAG		"SPDIF: "

#ifdef SPDIF_DEBUG
#define spdif_dbg(x...) printk(TAG""x)
#else
#define spdif_dbg(...)	do {} while (0)
#endif

static int imapx_spdif_set_channel(struct imapx_spdif_info *spdif,
				int clk_id)
{
	int rtxconfig_value;

	if (clk_id < 0 || clk_id > 2) {
		dev_err(spdif->dev, "Spdif channel num err: %d\n", clk_id);
		return -1;
	}
	rtxconfig_value = readl(spdif->regs + SP_TXCONFIG);
	rtxconfig_value &= ~(0x1 << 17);
	rtxconfig_value |= clk_id - 1;
	writel(rtxconfig_value, spdif->regs + SP_TXCONFIG);

	return 0;
}

static struct imapx_dma_client imapx_pch = {
	.name = "Spdif Stereo out"
};

static struct imapx_pcm_dma_params imapx_spdif_stereo_out = {
	.client     = &imapx_pch,
	.channel    = IMAPX_SPDIF_TX,
	.dma_addr   = IMAP_SPDIF_BASE + SP_TXFIFO,
	.dma_size   = 2,
};

static void imapx_spdif_playback(struct imapx_spdif_info *spdif, int flags)
{
	int sp_ctrl = 0, sp_txconfig = 0;

	/* enable the module or not */
	sp_ctrl = readl(spdif->regs + SP_CTRL);
	sp_ctrl &= ~(0x3f);
	if (flags)
		sp_ctrl |= 0x3f;
	writel(sp_ctrl, spdif->regs + SP_CTRL);
	/* enable dma or not */
	sp_txconfig = readl(spdif->regs + SP_TXCONFIG);
	sp_txconfig &= ~(0x1<<7);
	if (flags)
		sp_txconfig |= 0x1<<7;
	writel(sp_txconfig, spdif->regs + SP_TXCONFIG);
}

static int imapx_spdif_set_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int format)
{
	struct imapx_spdif_info *spdif = snd_soc_dai_get_drvdata(dai);
	int format_value, sp_txconfig, mdiv, sp_ctrl;

	spdif_dbg("channel is %d, freq is %d, format is %d\n",
			clk_id, freq, format);

	imapx_spdif_set_channel(spdif, clk_id);
	/* set word length */
	switch (format) {
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
		format_value = 24;
		break;
	default:
		dev_err(spdif->dev, "Not support this format: %d\n",
					format);
		return -EINVAL;
	}
	sp_txconfig = readl(spdif->regs + SP_TXCONFIG);
	sp_txconfig &= ~(0xf<<28);
	sp_txconfig |= (format_value - 16)<<28;
	writel(sp_txconfig, spdif->regs + SP_TXCONFIG);
	/* set clock source */
	sp_ctrl = readl(spdif->regs + SP_CTRL);
	sp_ctrl &= ~(0x3 << 6);
	sp_ctrl |= 0x3 << 6;
	writel(sp_ctrl, spdif->regs + SP_CTRL);
	/* set rate ratio */
	mdiv = spdif->iis_clock_rate / (128 * freq) - 1;
	spdif_dbg("The spdif div is %d\n", mdiv);
	sp_txconfig = readl(spdif->regs + SP_TXCONFIG);
	sp_txconfig &= ~(0xff<<20);
	sp_txconfig |= (mdiv<<20);
	writel(sp_txconfig, spdif->regs + SP_TXCONFIG);

	return 0;
}

static int imapx_spdif_set_clkdiv(struct snd_soc_dai *dai,
		int div_id, int div)
{
	spdif_dbg("Entered %s\n", __func__);
	return 0;
}

static int imapx_spdif_trigger(struct snd_pcm_substream *substream,
		int cmd, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct imapx_spdif_info *spdif = snd_soc_dai_get_drvdata(rtd->cpu_dai);

	spdif_dbg("Entered %s\n", __func__);

	spin_lock(&spdif->lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			dev_err(spdif->dev, "Spdif only support playback\n");
			spin_unlock(&spdif->lock);
			return -EINVAL;
		} else
			imapx_spdif_playback(spdif, 1);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			dev_err(spdif->dev, "Spdif only support playback\n");
			spin_unlock(&spdif->lock);
			return -EINVAL;
		} else
			imapx_spdif_playback(spdif, 0);
		module_power_down(SYSMGR_SPDIF_BASE);
		break;
	default:
		dev_err(spdif->dev, "Unsupport this cmd: %d\n", cmd);
		spin_unlock(&spdif->lock);
		return -EINVAL;
	}
	spin_unlock(&spdif->lock);

	return 0;
}

static int imapx_spdif_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *socdai)
{
	struct imapx_spdif_info *spdif = snd_soc_dai_get_drvdata(socdai);
	struct imapx_pcm_dma_params *dma_data;
	int sp_txconfig, sp_rxconfig, sp_intmask;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dma_data = &imapx_spdif_stereo_out;
	else {
		dev_err(spdif->dev, "Now spdif only support playback mode\n");
		return -ENODEV;
	}
	snd_soc_dai_set_dma_data(socdai, substream, dma_data);
	/* set fifo level */
	sp_txconfig = readl(spdif->regs + SP_TXCONFIG);
	sp_txconfig &= ~0x1f;
	sp_txconfig |= 0x4;
	writel(sp_txconfig, spdif->regs + SP_TXCONFIG);
	sp_rxconfig = readl(spdif->regs + SP_RXCONFIG);
	sp_rxconfig &= ~0x1f;
	sp_rxconfig |= 0x4;
	writel(sp_rxconfig, spdif->regs + SP_RXCONFIG);
	/* save rx chan and user data */
	sp_rxconfig = readl(spdif->regs + SP_RXCONFIG);
	sp_rxconfig &= ~(0xf << 18);
	sp_rxconfig |= 0xf << 18;
	writel(sp_rxconfig, spdif->regs + SP_RXCONFIG);
	/* set tx validy */
	sp_txconfig = readl(spdif->regs + SP_TXCONFIG);
	sp_txconfig &= ~(0x1 << 6);
	sp_txconfig |= 0x1 << 6;
	writel(sp_txconfig, spdif->regs + SP_TXCONFIG);
	/* mask irq */
	sp_intmask = readl(spdif->regs + SP_INTMASK);
	sp_intmask |= 0xffffffff;
	sp_intmask &= ~(0x3 << 24);
	writel(sp_intmask, spdif->regs + SP_INTMASK);

	return 0;
}

static int imapx_spdif_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct imapx_spdif_info *spdif = snd_soc_dai_get_drvdata(dai);

	spdif_dbg("Entered %s\n", __func__);
	module_power_on(SYSMGR_SPDIF_BASE);
	/* only for reset the control */
	writel(0, spdif->regs + SP_TXCONFIG);
	writel(0, spdif->regs + SP_RXCONFIG);
	writel(0, spdif->regs + SP_TXCHST);
	writel(0, spdif->regs + SP_INTMASK);
	writel(0, spdif->regs + SP_CTRL);
	return 0;
}

static struct snd_soc_dai_ops imapx_spdif_dai_ops = {
	.set_sysclk = imapx_spdif_set_sysclk,
	.set_clkdiv = imapx_spdif_set_clkdiv,
	.trigger    = imapx_spdif_trigger,
	.hw_params  = imapx_spdif_hw_params,
	.set_fmt    = imapx_spdif_set_fmt,
};

struct snd_soc_dai_driver spdif_dai = {
	.name   = "imap-spdif",
	.playback = {
		.channels_min   = 2,
		.channels_max   = 2,
		.rates		= SNDRV_PCM_RATE_8000_192000,
		.formats	= SNDRV_PCM_FMTBIT_S16_LE |
					  SNDRV_PCM_FMTBIT_S24_LE,
	},
	.ops = &imapx_spdif_dai_ops,
};

static const struct snd_soc_component_driver imapx_spdif_component = {
	.name   = "imapx-spdif",
};

static void imapx_spdif_module(void)
{
	//module_power_on(SYSMGR_SPDIF_BASE);
	imapx_pad_init("spdif");
}

static int imapx_spdif_probe(struct platform_device *pdev)
{
	struct imapx_spdif_info *spdif;
	struct resource *mem, *ioarea;
	int ret = 0;

	imapx_spdif_module();

	if (!pdev) {
		dev_err(&pdev->dev, "No spdif platform data\n");
		return -ENODEV;
	}
	spdif = kzalloc(sizeof(struct imapx_spdif_info), GFP_KERNEL);
	if (IS_ERR(spdif)) {
		dev_err(&pdev->dev, "malloc spdif info err\n");
		return -ENXIO;
	}
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "Get register mem err\n");
		goto err_alloc;
	}
	ioarea = request_mem_region(mem->start, resource_size(mem), pdev->name);
	if (!ioarea) {
		dev_err(&pdev->dev, "Mem have already claimed\n");
		goto err_alloc;
	}
	spdif->extclk = clk_get_sys("imap-spdif", "imap-spdif");
	if (IS_ERR(spdif->extclk)) {
		dev_err(&pdev->dev, "Spdif clock get err\n");
		goto err_alloc;
	}
	clk_prepare_enable(spdif->extclk);
	spdif->iis_clock_rate = clk_get_rate(spdif->extclk);
	spdif_dbg("Spdif clock is %d\n", spdif->iis_clock_rate);

	spdif->regs = ioremap(mem->start, resource_size(mem));
	if (!spdif->regs) {
		dev_err(&pdev->dev, "Spdif ioremap err\n");
		goto err_alloc;
	}
	spdif->dev = &pdev->dev;
	spin_lock_init(&spdif->lock);

	ret = snd_soc_register_component(&pdev->dev, &imapx_spdif_component,
			&spdif_dai, 1);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to register spdif dai\n");
		ret = -EBUSY;
		goto err_unmap;
	}
	ret = imapx_asoc_platform_probe(&pdev->dev);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to register the platform\n");
		ret = -EBUSY;
		goto err_unmap;
	}
	dev_set_drvdata(&pdev->dev, spdif);

	return 0;

err_unmap:
	iounmap(spdif->regs);
err_alloc:
	kfree(spdif);

	return ret;
}

static int imapx_spdif_remove(struct platform_device *pdev)
{
	struct imapx_spdif_info *spdif = dev_get_drvdata(&pdev->dev);
	struct resource *mem_res;

	imapx_asoc_platform_remove(&pdev->dev);
	snd_soc_unregister_component(&pdev->dev);

	iounmap(spdif->regs);
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(mem_res->start, resource_size(mem_res));
	clk_disable_unprepare(spdif->extclk);
	clk_put(spdif->extclk);

	kfree(spdif);
	return 0;
}

static struct platform_driver imapx_spdif_driver = {
	.probe  = imapx_spdif_probe,
	.remove = imapx_spdif_remove,
	.driver = {
		.name   = "imapx-spdif",
		.owner  = THIS_MODULE,
	},
};
module_platform_driver(imapx_spdif_driver);

MODULE_AUTHOR("SUN");
MODULE_DESCRIPTION("IMAPX800 SPDIF Controller Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:imapx-spdif");
