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
#include <mach/hw_cfg.h>
#include <linux/regulator/consumer.h>

struct imapnull_priv {
	struct regulator *regulator;
	struct codec_cfg *cfg;
};

#define IMAPX_SPDIF_FORMAT (SNDRV_PCM_FMTBIT_S16_LE | \
		SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_driver null_dai[] = {
	{
		.name = "virtual-spdif",
		.playback = {
			.stream_name = "Spdif Playback",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = IMAPX_SPDIF_FORMAT,
		},
	},
};

static struct snd_soc_codec_driver null_driver = {

};

static int imapnull_probe(struct platform_device *pdev)
{
	struct imapnull_priv *imapnull;
	struct codec_cfg *imapnull_cfg;
	int ret;

	imapnull_cfg = pdev->dev.platform_data;
	if (!imapnull_cfg) {
		dev_err(&pdev->dev, "imapnull get platform_data failed\n");
		return -EINVAL;
	}

	imapnull = kzalloc(sizeof(struct imapnull_priv), GFP_KERNEL);
	if (!imapnull) {
		dev_err(&pdev->dev, "unable to allocate mem\n");
		return -ENOMEM;
	}
	imapnull->cfg = imapnull_cfg;
#if 0
	imapnull->regulator = regulator_get(NULL, imapnull_cfg->power_pmu);
	if (IS_ERR(imapnull->regulator)) {
		dev_err(&pdev->dev, "%s: get regulator fail\n", __func__);
		return -1;
	}
	ret = regulator_set_voltage(imapnull->regulator, 3300000, 3300000);
	if (ret) {
		dev_err(&pdev->dev, "%s: set regulator fail\n", __func__);
		return -1;
	}
	ret = regulator_enable(imapnull->regulator);
#endif
	platform_set_drvdata(pdev, imapnull);
	return snd_soc_register_codec(&pdev->dev,
			&null_driver, null_dai, ARRAY_SIZE(null_dai));
}

static int imapnull_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver imapnull_driver = {
	.driver = {
		.name = "virtual-codec",
		.owner = THIS_MODULE,
	},
	.probe = imapnull_probe,
	.remove = imapnull_remove,
};

static int __init imapnull_modinit(void)
{
	return platform_driver_register(&imapnull_driver);
}
module_init(imapnull_modinit);

static void __exit imapnull_exit(void)
{
	platform_driver_unregister(&imapnull_driver);
}
module_exit(imapnull_exit);

MODULE_DESCRIPTION("ASoC vitual driver");
MODULE_AUTHOR("Sun");
MODULE_LICENSE("GPL");
