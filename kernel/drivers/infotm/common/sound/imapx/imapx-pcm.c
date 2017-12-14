#include <linux/clk.h>
#include <linux/io.h>
#include <mach/pad.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <mach/imap_pl330.h>
#include <mach/power-gate.h>
#include <linux/module.h>
#include "imapx-dma.h"
#include "imapx-pcm.h"

#define IPC_AP6212

static struct imapx_dma_client imapx_pcm_dma_client_out = {
	.name       = "PCM Stereo out"
};

static struct imapx_dma_client imapx_pcm_dma_client_in = {
	.name       = "PCM Stereo in"
};

static struct imapx_pcm_dma_params imapx_pcm_stereo_out[] = {
	[0] = {
		.client		= &imapx_pcm_dma_client_out,
		.channel    = IMAPX_PCM0_TX,
		.dma_addr	= IMAP_PCM0_BASE + IMAPX_PCM_TXFIFO,
		.dma_size	= 2,
	},
	[1] = {
		.client		= &imapx_pcm_dma_client_out,
		.channel	= IMAPX_PCM1_TX,
		.dma_addr	= IMAP_PCM1_BASE + IMAPX_PCM_TXFIFO,
		.dma_size	= 2,
	},
};

static struct imapx_pcm_dma_params imapx_pcm_stereo_in[] = {
	[0] = {
		.client		= &imapx_pcm_dma_client_in,
		.channel	= IMAPX_PCM0_RX,
		.dma_addr	= IMAP_PCM0_BASE + IMAPX_PCM_RXFIFO,
		.dma_size	= 2,
	},
	[1] = {
		.client		= &imapx_pcm_dma_client_in,
		.channel	= IMAPX_PCM1_RX,
		.dma_addr	= IMAP_PCM1_BASE + IMAPX_PCM_RXFIFO,
		.dma_size	= 2,
	},
};

static struct imapx_pcm_info imapx_pcm[2];

static void imapx_pcm_disable_module(struct imapx_pcm_info *pcm)
{
	int clkctl, cfg;
	dev_dbg(pcm->dev, "Entered %s\n", __func__);

    /*   disable pcm clock */
	clkctl = readl(pcm->regs + IMAPX_PCM_CLKCTL);
	clkctl &= ~(0xf << 0);
	writel(clkctl, pcm->regs + IMAPX_PCM_CLKCTL);
    /*  disable pcm module */
	writel(0x0, pcm->regs + IMAPX_PCM_CTL);
    /*  disable pcm dma */
	cfg = readl(pcm->regs + IMAPX_PCM_CFG);
	cfg &= ~(3 << 2);
	writel(cfg, pcm->regs + IMAPX_PCM_CFG);
}

static void imapx_pcm_enable_module(struct imapx_pcm_info *pcm, int dir)
{
	int clkctl, cfg;
	dev_dbg(pcm->dev, "Entered %s\n", __func__);

    /* disable pcm clock */
	clkctl = readl(pcm->regs + IMAPX_PCM_CLKCTL);
	clkctl |= (0xf << 0);
	writel(clkctl, pcm->regs + IMAPX_PCM_CLKCTL);
    /* disable pcm module */
	writel(0x3d, pcm->regs + IMAPX_PCM_CTL);
    /* disable pcm dma */
	cfg = readl(pcm->regs + IMAPX_PCM_CFG);
//	cfg &= ~(3 << 2);
	cfg |= (dir << 2);
	writel(cfg, pcm->regs + IMAPX_PCM_CFG);
}

static void imapx_pcm_snd_rxctrl(struct imapx_pcm_info *pcm, int on)
{
	dev_dbg(pcm->dev, "Entered %s\n", __func__);

	if (on)
		imapx_pcm_enable_module(pcm, 0x1);
//	else
//		imapx_pcm_disable_module(pcm);
}

static void imapx_pcm_snd_txctrl(struct imapx_pcm_info *pcm, int on)
{
	dev_dbg(pcm->dev, "Entered %s\n", __func__);

	if (on)
		imapx_pcm_enable_module(pcm, 0x2);
//	else
//		imapx_pcm_disable_module(pcm);
}

static int imapx_pcm_set_sysclk(struct snd_soc_dai *cpu_dai,
		int clk_id, unsigned int freq, int dir)
{
	struct imapx_pcm_info *pcm = snd_soc_dai_get_drvdata(cpu_dai);
	int format_value = 0, cfg, clkcfg;
	int mclkdiv, bitclkdiv, syncdiv, bit_clk, pcm_div;

	dev_dbg(pcm->dev, "Entered %s : channel = %d, freq is %d, format is %d\n",
			 __func__, clk_id, freq, dir);

	switch (dir) {
	case 0:
	case 1:
		format_value = PCM_WL_REG_8;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		format_value = PCM_WL_REG_16;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		format_value = PCM_WL_REG_24;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
		format_value = PCM_WL_REG_32;
		break;
	}
    /* set pcm cd length and channel num */
	cfg = readl(pcm->regs + IMAPX_PCM_CFG);
	cfg &= ~((0x7 << 23) | (0x31 << 16));
	cfg |= format_value << 16;
	cfg |= (clk_id - 1) << 23;
#ifdef IPC_AP6212
	cfg |= (1 << 26);
#endif
	writel(cfg, pcm->regs + IMAPX_PCM_CFG);

    /* set bclk and mclk */
	if ((format_value + 16) > 32)
		format_value = 8;
	else
		format_value = PCM_WL_REG_16 + 16;

	if (freq == 96000)
		pcm_div = 128;
	else
		pcm_div = 256;
	bit_clk = freq * clk_id * format_value;
	bitclkdiv = pcm->iis_clock_rate / bit_clk;
	mclkdiv = bitclkdiv / (pcm_div / (format_value * clk_id));
#if defined(CONFIG_Q3F_FPGA_PLATFORM) || defined(CONFIG_APOLLO3_FPGA_PLATFORM) || defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	bitclkdiv = CONFIG_FPGA_EXTEND_CLK / bit_clk;
#else
	bitclkdiv = mclkdiv * (pcm_div / (format_value * clk_id));
#endif
	writel(mclkdiv - 1, pcm->regs + IMAPX_PCM_MCLK_CFG);
	writel(bitclkdiv - 1, pcm->regs + IMAPX_PCM_BCLK_CFG);
	dev_err(pcm->dev, "bclk is %d, mclk is %d\n",
			bitclkdiv - 1, mclkdiv - 1);
    /* set pcm syncdiv */
#ifdef IPC_AP6212
	syncdiv = format_value * clk_id + 1;
#else
	syncdiv = format_value * clk_id;
#endif
	clkcfg = readl(pcm->regs + IMAPX_PCM_CLKCTL);
	clkcfg &= ~(0x1ff << 8);
	clkcfg |= ((syncdiv-1) << 8);
	writel(clkcfg, pcm->regs + IMAPX_PCM_CLKCTL);

	return 0;
}

static int imapx_pcm_trigger(struct snd_pcm_substream *substream, int cmd,
		struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct imapx_pcm_info *pcm = snd_soc_dai_get_drvdata(rtd->cpu_dai);
	unsigned long flags;

	dev_dbg(pcm->dev, "Entered %s, state = %x\n", __func__, cmd);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		spin_lock_irqsave(&pcm->lock, flags);
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			imapx_pcm_snd_rxctrl(pcm, 1);
		else
			imapx_pcm_snd_txctrl(pcm, 1);
		spin_unlock_irqrestore(&pcm->lock, flags);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		spin_lock_irqsave(&pcm->lock, flags);
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			imapx_pcm_snd_rxctrl(pcm, 0);
		else
			imapx_pcm_snd_txctrl(pcm, 0);
		spin_unlock_irqrestore(&pcm->lock, flags);
		break;
	default:
		dev_err(pcm->dev, "Unsupport this cmd: %d\n", cmd);
		return -EINVAL;
	}

	return 0;
}

static int imapx_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *socdai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct imapx_pcm_info *pcm = snd_soc_dai_get_drvdata(rtd->cpu_dai);
	struct imapx_pcm_dma_params *dma_data;
	unsigned long flags;
	int cfg, irqctl;

	dev_dbg(pcm->dev, "Entered %s\n", __func__);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dma_data = pcm->dma_playback;
	else
		dma_data = pcm->dma_capture;
	snd_soc_dai_set_dma_data(rtd->cpu_dai, substream, dma_data);

	spin_lock_irqsave(&pcm->lock, flags);
    /* set fifo size */
	cfg = readl(pcm->regs + IMAPX_PCM_CFG);
	cfg &= ~(0x3ff<<4);
	cfg |= (16 << 10) | (16 << 4);
    /* set pcm transfer */
#if defined(CONFIG_Q3F_FPGA_PLATFORM)||defined(CONFIG_APOLLO3_FPGA_PLATFORM)||defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	cfg &= ~(0x3 << 21);
	cfg |= 0x3 << 21;
#else
	cfg &= ~(0x3 << 21);
#endif
    /* MSB pos */
	cfg &= ~(0x3 << 0);
#ifdef IPC_AP6212
	cfg |= (0x3 << 0);
#endif
	writel(cfg, pcm->regs + IMAPX_PCM_CFG);
    /* set irq */
	irqctl = TX_FIFO_ALMOST_EMPTY | TX_FIFO_EMPTY |
		EN_IRQ_TO_ARM;
	writel(irqctl, pcm->regs + IMAPX_PCM_IRQ_CTL);
	spin_unlock_irqrestore(&pcm->lock, flags);
	return 0;
}

static int imapx_pcm_set_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	struct imapx_pcm_info *pcm = snd_soc_dai_get_drvdata(cpu_dai);
	int ret = 0, clkcfg, cfg;
	unsigned long flags;
	dev_dbg(pcm->dev, "Entered %s\n", __func__);

	spin_lock_irqsave(&pcm->lock, flags);
	clkcfg = readl(pcm->regs + IMAPX_PCM_CLKCTL);
	clkcfg &= ~(0x3 << 5);
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_NB_IF:
#ifndef IPC_AP6212
		clkcfg |= (0x1 << 5);
#endif
		break;
	case SND_SOC_DAIFMT_IB_NF:
		clkcfg |= (0x2 << 5);
		break;
	case SND_SOC_DAIFMT_IB_IF:
		clkcfg |= (0x3 << 4);
		break;
	default:
		dev_err(pcm->dev, "Unsupported this mode clock inversion: %d\n",
					fmt);
		ret = -EINVAL;
		goto exit;
	}
	writel(clkcfg, pcm->regs + IMAPX_PCM_CLKCTL);

	cfg = readl(pcm->regs + IMAPX_PCM_CFG);
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		cfg &= ~(1 << 27);
		break;
	default:
		dev_err(pcm->dev, "Now only support master mode: %d\n",
				fmt & SND_SOC_DAIFMT_MASTER_MASK);
		ret = -EINVAL;
		goto exit;
	}
	writel(cfg, pcm->regs + IMAPX_PCM_CFG);
exit:
	spin_unlock_irqrestore(&pcm->lock, flags);
	return ret;
}

static struct snd_soc_dai_ops imapx_pcm_dai_ops = {
	.set_sysclk = imapx_pcm_set_sysclk,
	.trigger    = imapx_pcm_trigger,
	.hw_params  = imapx_pcm_hw_params,
	.set_fmt    = imapx_pcm_set_fmt,
};

#define IMAPX_PCM_RATES  SNDRV_PCM_RATE_8000_96000

#define IMAPX_PCM_DAI_DECLARE					\
	.symmetric_rates = 1,						\
	.ops = &imapx_pcm_dai_ops,					\
	.playback = {	\
		.channels_min   = 1,					\
		.channels_max   = 2,					\
		.rates      = IMAPX_PCM_RATES,			\
		.formats    = SNDRV_PCM_FMTBIT_S16_LE,  \
	},				\
	.capture = {	\
		.channels_min   = 1,					\
		.channels_max   = 2,					\
		.rates      = IMAPX_PCM_RATES,			\
		.formats    = SNDRV_PCM_FMTBIT_S16_LE,  \
	}

struct snd_soc_dai_driver imapx_pcm_dai[] = {
	[0] = {
		.name   = "imapx-pcm.0",
		IMAPX_PCM_DAI_DECLARE,
	},
	[1] = {
		.name   = "imapx-pcm.1",
		IMAPX_PCM_DAI_DECLARE,
	}
};

static const struct snd_soc_component_driver imapx_pcm_component = {
	.name   = "imapx-pcm",
};

static int imapx_pcm_probe(struct platform_device *pdev)
{
	struct imapx_pcm_info *pcm;
	struct resource *mem, *ioarea;
	int ret = 0;

	module_power_on(SYSMGR_PCM_BASE);
	
	if ((pdev->id < 0) || pdev->id >= ARRAY_SIZE(imapx_pcm)) {
		dev_err(&pdev->dev, "id %d out of range\n", pdev->id);
		return -EINVAL;
	}
	if (pdev->id == 0)
		imapx_pad_init("pcm0");
	else
		imapx_pad_init("pcm1");

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "Unable to get register mem\n");
		return -ENXIO;
	}
	ioarea = request_mem_region(mem->start, resource_size(mem), pdev->name);
	if (!ioarea) {
		dev_err(&pdev->dev, "PCM region already claimed!\n");
		return -EBUSY;
	}

	pcm = &imapx_pcm[pdev->id];
	pcm->dev = &pdev->dev;

	spin_lock_init(&pcm->lock);
	pcm->extclk = clk_get_sys("imap-pcm.0","imap-pcm");
	if (IS_ERR(pcm->extclk)) {
		dev_err(&pdev->dev,"get pcm bus clk fail\n");
		ret = -ENODEV;
		goto free_mem;
	}
	ret = clk_prepare_enable(pcm->extclk);
	if(ret){
		dev_err(&pdev->dev,"prepare pcm bus clk fail\n");
		goto free_clk;
	}

	switch (pdev->id) {
	case 0:
		pcm->extclk = clk_get_sys("imap-audio-clk",
							"imap-audio-clk");
		break;
	case 1:
		pcm->extclk = clk_get_sys("imap-audio-clk",
							"imap-audio-clk");
		break;
	}
	if (IS_ERR(pcm->extclk)) {
		ret = -ENODEV;
		goto free_mem;
	}
	ret = clk_prepare_enable(pcm->extclk);
	if(ret){
		printk("prepare pcm device clk fail\n");
		goto free_clk;
	}
#if defined(CONFIG_Q3F_FPGA_PLATFORM)||defined(CONFIG_APOLLO3_FPGA_PLATFORM)||defined(CONFIG_CORONAMPW_FPGA_PLATFORM)
	pcm->iis_clock_rate = CONFIG_FPGA_EXTEND_CLK;
#else
	pcm->iis_clock_rate = clk_get_rate(pcm->extclk);
#endif

	pcm->regs = ioremap(mem->start, resource_size(mem));
	if (!pcm->regs) {
		dev_err(&pdev->dev, "failure mapping io resources!\n");
		ret = -EBUSY;
		goto free_clk;
	}

	ret = snd_soc_register_component(&pdev->dev, &imapx_pcm_component,
			&imapx_pcm_dai[pdev->id], 1);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to register the dai\n");
		ret = -EBUSY;
		goto free_remap;
	}
	ret = imapx_asoc_platform_probe(&pdev->dev);
	if (ret != 0) {
		pr_err("failed to register the platform\n");
		ret = -ENODEV;
		goto free_remap;
	}

	dev_set_drvdata(&pdev->dev, pcm);
	pcm->dma_playback = &imapx_pcm_stereo_out[pdev->id];
	pcm->dma_capture = &imapx_pcm_stereo_in[pdev->id];
	imapx_pcm_disable_module(pcm);

	return 0;

free_remap:
	iounmap(pcm->regs);
free_clk:
	clk_disable_unprepare(pcm->extclk);
	clk_put(pcm->extclk);
free_mem:
	release_mem_region(mem->start, resource_size(mem));

	return ret;
}

static int imapx_pcm_remove(struct platform_device *pdev)
{
	struct imapx_pcm_info *pcm = &imapx_pcm[pdev->id];
	struct resource *mem_res;
	
	imapx_asoc_platform_remove(&pdev->dev);
	snd_soc_unregister_component(&pdev->dev);

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(mem_res->start, resource_size(mem_res));
	clk_disable_unprepare(pcm->extclk);
	clk_put(pcm->extclk);

	return 0;
}

static struct platform_driver imapx_pcm_driver = {
	.probe  = imapx_pcm_probe,
	.remove = imapx_pcm_remove,
	.driver = {
		.name   = "imapx-pcm",
		.owner  = THIS_MODULE,
	},
};
module_platform_driver(imapx_pcm_driver);

MODULE_AUTHOR("SUN");
MODULE_DESCRIPTION("IMAPX PCM Controller Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:imapx-pcm");
