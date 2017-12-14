#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/io.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/regulator/consumer.h>
#include <mach/hw_cfg.h>
#include <mach/pad.h>
#include "hpdetect.h"
#include <mach/ip6208.h>

#define AUDIO_NAME "IP6205"

#ifdef IP6205_DEBUG
#define dbg(format, arg...) \
    printk(KERN_DEBUG AUDIO_NAME ": " format "\n", ## arg)
#else
#define dbg(format, arg...) do {} while (0)
#endif
#define err(format, arg...) \
    printk(KERN_ERR AUDIO_NAME ": " format "\n", ## arg)
#define info(format, arg...) \
    printk(KERN_INFO AUDIO_NAME ": " format "\n", ## arg)

struct ip6205_priv {
	unsigned int sysclk;
	struct regulator *regulator;
	struct codec_cfg *cfg;
	struct delayed_work reg_show;
};

struct ip6205_reg_val {
	uint8_t reg;
	uint16_t val;
};

struct snd_soc_codec * ip6205_codec;

int ip6205_reg[] = {
	0xd0, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8,
	0xda, 0xdb, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xf0,
	0xf1, 0xf3, 0xf4, 0x30, 0x3d,
};

static ssize_t ip6205_index_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct snd_soc_codec *codec = ip6205_codec;
	int i;
	uint16_t val;

	for (i = 0; i < ARRAY_SIZE(ip6205_reg); i++) {
		ip620x_codec_read(ip6205_reg[i], &val);
		err("[IP6205] reg 0x%x\t 0x%x\n", ip6205_reg[i], val);
	}

	return 0;
}

static DEVICE_ATTR(index_reg, 0444, ip6205_index_show, NULL);

struct ip6205_reg_val ip6205_init_val[] = {
	{0xa4, 0x00},
	{0xa6, 0x03},

	{0xd0, 0x063b},
	{0xd8, 0x0183},
	{0xda, 0x0220},
	{0xf0, 0x003f},
	{0xf1, 0x12c0},
	{0xf3, 0x07fb},
	{0xf4, 0x07ff},
	{0xd0, 0x863b},

	{0xd6, 0x0003},
//	{0xd7, 0xbebe},
	{0xd7, 0xe0be},
//	{0xe0, 0x1913},
	{0xe0, 0x0707}, //0707 //1913
//	{0xe1, 0x0088},
	{0xe1, 0x00ee},
	{0xe2, 0x0041}, //41 //82
	{0xe3, 0xfd55},

//	MIC1 & MIC2
	{0xe0, 0x1f17},
	{0xe2, 0x00c3},
	{0xe3, 0xfd55},

	{0xd0, 0x869b},
};

/**
 * ip6205_get_volsw - single mixer get callback
 * @kcontrol: mixer control
 * @ucontrol: control element information
 *
 * Callback to get the value of a single mixer control, or a double mixer
 * control that spans 2 registers.
 *
 * Returns 0 for success.
 */
static int ip6205_get_volsw(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	uint16_t val;

	ip620x_codec_read(reg, &val);
	ucontrol->value.integer.value[0] =
		(val >> shift) & mask;

	if (invert)
		ucontrol->value.integer.value[0] =
			max - ucontrol->value.integer.value[0];

	if (snd_soc_volsw_is_stereo(mc)) {
		if (reg == reg2) {
			ip620x_codec_read(reg, &val);
			ucontrol->value.integer.value[1] =
				(val >> rshift) & mask;
		}
		else {
			ip620x_codec_read(reg2, &val);
			ucontrol->value.integer.value[1] =
				(val >> shift) & mask;
		}
		if (invert)
			ucontrol->value.integer.value[1] =
				max - ucontrol->value.integer.value[1];
	}

	return 0;
}

/**
 * ip6205_put_volsw - single mixer put callback
 * @kcontrol: mixer control
 * @ucontrol: control element information
 *
 * Callback to set the value of a single mixer control, or a double mixer
 * control that spans 2 registers.
 *
 * Returns 0 for success.
 */
int ip6205_put_volsw(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	int err;
	bool type_2r = 0;
	unsigned int val2 = 0;
	unsigned int old_val, val, val_mask;

	val = (ucontrol->value.integer.value[0] & mask);
	if (invert)
		val = max - val;
	val_mask = mask << shift;
	val = val << shift;
	if (snd_soc_volsw_is_stereo(mc)) {
		val2 = (ucontrol->value.integer.value[1] & mask);
		if (invert)
			val2 = max - val2;
		if (reg == reg2) {
			val_mask |= mask << rshift;
			val |= val2 << rshift;
		} else {
			val2 = val2 << shift;
			type_2r = 1;
		}
	}

	err = ip620x_codec_read(reg, &old_val);
	if (err < 0) {
		return err;
	}
	err = ip620x_codec_write(reg, (old_val & ~(val_mask)) | (val & val_mask));
	if (err < 0) {
		return err;
	}

	if (type_2r) {
		err = ip620x_codec_read(reg2, &old_val);
		if (err < 0) {
			return err;
		}
		err = ip620x_codec_write(reg2, (old_val & ~(val_mask)) | (val2 & val_mask));
	}

	return err;
}


static const struct snd_kcontrol_new ip6205_snd_controls[] = {

	/* AIN Channel 1 L to ADC L */
    SOC_SINGLE_EXT("Mic L1AL Enable Switch", 0xe2, 0, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit0/1, max=1, invert=0 */
	/* AIN Channel 2 L to ADC L */
    SOC_SINGLE_EXT("Mic L2AL Enable Switch", 0xe2, 1, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit0/1, max=1, invert=0 */

	/* AIN Channel 1 L to ADC R */
    SOC_SINGLE_EXT("Mic L1AR Enable Switch", 0xe2, 4, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit4, max=1, invert=0 */
	/* AIN Channel 2 L to ADC R */
    SOC_SINGLE_EXT("Mic L2AR Enable Switch", 0xe2, 5, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit5, max=1, invert=0 */

	/* AIN Channel 1 R to ADC R */
    SOC_SINGLE_EXT("Mic R1AR Enable Switch", 0xe2, 6, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit6, max=1, invert=0 */
	/* AIN Channel 2 R to ADC R */
    SOC_SINGLE_EXT("Mic R2AR Enable Switch", 0xe2, 7, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit7, max=1, invert=0 */

	/* DAC L to ADC L */
    SOC_SINGLE_EXT("Mic DLAL Enable Switch", 0xe2, 8, 1, 0, ip6205_get_volsw, ip6205_put_volsw), /*reg:0xe2, shift=bit8, max=1, invert=0 */
};

static int ip6205_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct ip6205_priv *ip6205 = snd_soc_codec_get_drvdata(codec);

	switch (freq) {
		case 11289600:
		case 12000000:
		case 12288000:
		case 16934400:
		case 18432000:
			ip6205->sysclk = freq;
			return 0;
	}

	return -EINVAL;
}

static int ip6205_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	return 0;
}

static int ip6205_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	return 0;
}

static int ip6205_mute(struct snd_soc_codec *codec, int mute)
{
	return 0;
}

static int ip6205_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	return 0;
}

static int ip6205_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
	return 0;
}

static int ip6205_codec_init(struct snd_soc_codec *codec)
{
	int i, ret;
	ip6205_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	printk("%s\n", __func__);
	for (i = 0; i < (ARRAY_SIZE(ip6205_init_val)); ++i) {
		ret = ip620x_codec_write(ip6205_init_val[i].reg, ip6205_init_val[i].val);
		if (ret < 0) {
			err("[ip6205] init reg: %x failed\n", ip6205_init_val[i].reg);
			return ret;
		}
	}

	return 0;
}

static struct snd_soc_dai_ops ip6205_ops = {
	.hw_params	= ip6205_pcm_hw_params,
	.set_fmt 	= ip6205_set_dai_fmt,
	.set_sysclk	= ip6205_set_dai_sysclk,
	.digital_mute = ip6205_mute,
	.set_clkdiv	= ip6205_set_dai_clkdiv,
};

#define ip6205_RATES (SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_96000 |\
		SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_32000 |\
		SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_8000 |\
		SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_44100 |\
		SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_11025)

#define ip6205_FORMATS (SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_S24_LE |\
		SNDRV_PCM_FMTBIT_S16_LE)

static struct snd_soc_dai_driver ip6205_dai = {
	.name = "ip6205_dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = ip6205_RATES,
		.formats = SNDRV_PCM_FMTBIT_S32_LE,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = ip6205_RATES,
		.formats = SNDRV_PCM_FMTBIT_S32_LE,
	},
	.ops = &ip6205_ops
};

static int ip6205_suspend(struct snd_soc_codec *codec)
{
	struct ip6205_priv *ip6205 = snd_soc_codec_get_drvdata(codec);

	hpdetect_suspend();
	ip6205_set_bias_level(codec, SND_SOC_BIAS_OFF);

	return 0;
}

static int ip6205_resume(struct snd_soc_codec *codec)
{
	ip6205_set_bias_level(codec, SND_SOC_BIAS_ON);
	hpdetect_resume();

	return 0;
}

static int ip6205_probe(struct snd_soc_codec *codec)
{
	struct ip6205_priv *ip6205 = snd_soc_codec_get_drvdata(codec);
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;

	ip6205_codec = codec;

	printk("%s\n", __func__);
	ret = ip6205_codec_init(codec);
	ret = ip6205_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	ret = snd_soc_add_codec_controls(codec, ip6205_snd_controls,
			ARRAY_SIZE(ip6205_snd_controls));
	ret = device_create_file(codec->dev, &dev_attr_index_reg);

	return 0;
}

static int ip6205_remove(struct snd_soc_codec *codec)
{
	ip6205_set_bias_level(codec, SND_SOC_BIAS_OFF);
	device_remove_file(codec->dev, &dev_attr_index_reg);

	return 0;
}

static struct snd_soc_codec_driver imap_ip6205 = {
	.probe		= ip6205_probe,
	.remove		= ip6205_remove,
	.suspend	= ip6205_suspend,
	.resume		= ip6205_resume,
	.set_bias_level	= ip6205_set_bias_level,
};

static int imapx_ip6205_probe(struct platform_device *pdev)
{
	struct ip6205_priv *ip6205;
	struct codec_cfg *ip6205_cfg;
	int ret;

	info("Codec driver is ip6205\n");

	ip6205_cfg = pdev->dev.platform_data;

	ip6205 = kzalloc(sizeof(struct ip6205_priv), GFP_KERNEL);
	if (ip6205 == NULL) {
		err("Alloc ip6205 failed\n");
		return -ENOMEM;
	}

	ip6205->cfg = ip6205_cfg;

	hpdetect_init(ip6205_cfg);
	printk("ip6205_cfg->power_pmu %s\n", ip6205_cfg->power_pmu);

	if (strcmp(ip6205_cfg->power_pmu, "NOPMU")) {
		ip6205->regulator = regulator_get(&pdev->dev, ip6205_cfg->power_pmu);
		if (IS_ERR(ip6205->regulator)) {
			err("[IP6205] get regulator failed\n");
			return -1;
		}

		ret = regulator_set_voltage(ip6205->regulator, 3300000, 3300000);
		if (ret) {
			err("[IP6205] set regulator failed\n");
			return -1;
		}

		regulator_enable(ip6205->regulator);
	}

	platform_set_drvdata(pdev, ip6205);

	return snd_soc_register_codec(&pdev->dev, &imap_ip6205,
			&ip6205_dai, 1);
	if (ret < 0) {
		err("[IP6205] cannot register codec\n");
		kfree(ip6205);
	}

	return ret;
}

static int imapx_ip6205_remove(struct platform_device *pdev)
{
	struct ip6205_priv *ip6205 = platform_get_drvdata(pdev);

	kfree(ip6205);
	snd_soc_unregister_codec(&pdev->dev);

	return 0;
}

static struct platform_driver imapx_ip6205_driver = {
	.driver = {
		.name = "ip6205",
		.owner = THIS_MODULE,
	},
	.probe = imapx_ip6205_probe,
	.remove = imapx_ip6205_remove,
};

static int __init imapx_ip6205_modinit(void)
{
	return platform_driver_register(&imapx_ip6205_driver);
//	return i2c_add_driver(&ip6205_i2c_driver);
}
module_init(imapx_ip6205_modinit);

static void __exit imapx_ip6205_modexit(void)
{
	platform_driver_unregister(&imapx_ip6205_driver);
//	i2c_del_driver(&ip6205_i2c_driver);
}
module_exit(imapx_ip6205_modexit);

MODULE_DESCRIPTION("Asoc IP6205 Driver");
MODULE_AUTHOR("Junery");
MODULE_LICENSE("GPL");
