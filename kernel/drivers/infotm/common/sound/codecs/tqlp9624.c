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
#include <mach/power-gate.h>
#include <sound/tlv.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/export.h>
#include <mach/pad.h>
#include <linux/regulator/consumer.h>
#include <mach/audio.h>
#include <mach/hw_cfg.h>
#include <sound/control.h>
#include "hpdetect.h"
#include "tqlp9624.h"

//#define CONFIG_SND_DEBUG
#ifdef CONFIG_SND_DEBUG
#define snd_dbg(x...) printk(x)
#else
#define snd_dbg(x...)
#endif

#define snd_err(x...) printk(x)
#define snd_info(x...)	printk(x)

static int tqlp9624_code_init(struct snd_soc_codec *codec);
struct snd_soc_codec *tqlp9624_codec;

int reg[] = {
	0x0000, 0x0004, 0x0400, 0x0404, 0x0408, 0x040c,
	0x0430, 0x0434, 0x0438, 0x0444, 0x0454, 0x0458,
	0x045c, 0x0460, 0x0464, 0x0468, 0x0474, 0x047c,
	0x0484, 0x0490, 0x0494, 0x0498, 0x049C, 0x04D0,
	0x04D4, 0x04E0, 0x04E4, 0x051C, 0x0520, 0x0554,
	0x0558, 0x0564, 0x0568, 0x0688, 0x068C, 0x0690,
	0x0694, 0x0698, 0x069C, 0x06A0, 0x06A4, 0x06A8,
	0x06AC, 0x06B0, 0x06B4, 0x06B8, 0x06BC, 0x06C0,
	0x06C4, 0x06C8, 0x06CC, 0x06D0, 0x06D4, 0x06D8,
	0x06DC, 0x06E0, 0x06E4, 0x06E8, 0x06EC, 0x06F0,
	0x06F4, 0x06F8, 0x06FC, 0x0700, 0x0704, 0x0708,
	0x070C, 0x0720, 0x0724, 0x0728, 0x072C, 0x0730,
	0x0734, 0x0738, 0x0748, 0x074C, 0x0750, 0x0754,
	0x07C0, 0x07C4, 0x07C8, 0x07CC, 0x07E0, 0x07E4,
	0x07E8, 0x07EC
};

static ssize_t tqlp9624_index_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct tqlp9624_priv *tqlp9624 = dev_get_drvdata(dev);
	int i;

	snd_info("tqlp9624 reg: %d\n", ARRAY_SIZE(reg));
	for (i = 0; i < ARRAY_SIZE(reg); i++) {
		snd_info("0x%x      0x%x\n", reg[i], readl(tqlp9624->base
					+ reg[i]));
	}
	return 0;
}

static DEVICE_ATTR(index_reg, 0444, tqlp9624_index_show, NULL);

static void tqlp9624_latch(struct snd_soc_codec *codec)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	writel(0x1, tqlp9624->base + LTHR1);
	writel(0x0, tqlp9624->base + LTHR1);
}

static int tqlp9624_latch_signal(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	uint32_t val;
	
	/* set the master switch*/	
	val = readl(tqlp9624->base + PC0R21);
	val |= 0x1;
	writel(val, tqlp9624->base + PC0R21);
	/* To guarantee that all the required power 
	 * ontrol signals are loaded to the Audio Codec IP simultaneously
	 */
	tqlp9624_latch(codec);
	return 0;
}

static unsigned int tqlp9624_read(struct snd_soc_codec *codec,
		unsigned int reg)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	return readl(tqlp9624->base + reg);	
}

static int tqlp9624_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int val)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	writel(val, tqlp9624->base + reg);
	return 0;
}

int tqlp9624_mic_adc_set(int channel, int enable)
{
    struct snd_soc_codec *codec = tqlp9624_codec;
    struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	volatile uint32_t val;
 
    if (enable) {
		
			val = readl(tqlp9624->base + PC0R21);		//turn on bias
			val |=(0x1<<1);
			writel(val, tqlp9624->base + PC0R21);
		
			writel(0x00, tqlp9624->base + ADCHPMR211);

        if (channel == 1) {								//config the left channel
			val =0;
			val = readl(tqlp9624->base + PC1R22);		//turn on left adc and pga power
			val |= 0xf;
			writel(val, tqlp9624->base + PC1R22);

			val =0;
			val = readl(tqlp9624->base + MC0R29);		//config left pga mute and record mute
			val &= ~0xf;
			writel(val, tqlp9624->base + MC0R29);

            writel(0x14, tqlp9624->base + RLCHMVCR36);	 //left channel master volume
            writel(0x12, tqlp9624->base + PGALCHVCR38);	 //left pga volume

            writel(0x05, tqlp9624->base + RLCHISR71);    //set left mic Left microphone (single ended)

       } else if (channel == 2) {						//config the right channel

			val = readl(tqlp9624->base + PC1R22);		//turn on right adc power
			val |= 0xf;
			writel(val, tqlp9624->base + PC1R22);

			val = readl(tqlp9624->base + MC0R29);		//config the pga mute and record mute
			val &= ~0xf;
		    writel(val, tqlp9624->base + MC0R29);

            writel(0x14, tqlp9624->base + RRCHMVCR37);	//left channel master volume
            writel(0x12, tqlp9624->base + PGARCHVCR39);

            writel(0x05, tqlp9624->base + RRCHISR72);
        }
    } else {
        writel(0x05, tqlp9624->base + ADCHPMR211);
        if (channel == 1) {
			val = readl(tqlp9624->base + PC1R22);		//turn off left adc power
			val &= ~(0x1<<1|0x1<<3);
			writel(val, tqlp9624->base + PC1R22);

            val = readl(tqlp9624->base + MC0R29);
			val |= 0x11;
			writel(val, tqlp9624->base + MC0R29);
            writel(0x04, tqlp9624->base + RLCHISR71);
            writel(0x14, tqlp9624->base + RLCHMVCR36);
            writel(0x2a, tqlp9624->base + PGALCHVCR38);
        } else if (channel == 2) {

			val = readl(tqlp9624->base + PC1R22);		//turn off right adc power
			val &= ~(0x1<<0|0x1<<2);
			writel(val, tqlp9624->base + PC1R22);

			val = readl(tqlp9624->base + MC0R29);
			val |= 0x22;
			writel(val, tqlp9624->base + MC0R29);
            writel(0x04, tqlp9624->base + RRCHISR72);
            writel(0x14, tqlp9624->base + RRCHMVCR37);
            writel(0x2a, tqlp9624->base + PGARCHVCR39);
        }
    }
	tqlp9624_latch(codec);
    return 1;
}
EXPORT_SYMBOL(tqlp9624_mic_adc_set);

void tqlp9624_switch_spk(int flags)
{
	struct snd_soc_codec *codec = tqlp9624_codec;
	struct tqlp9624_priv *tqlp9624 =
		snd_soc_codec_get_drvdata(codec);
	snd_dbg("%s: %d\n", __func__, flags);
	if (codec) {
		if (flags)
			writel(0x0, tqlp9624->base + MC4R33);
		else
			writel(0x3, tqlp9624->base + MC4R33);
	} else
		snd_dbg("codec is NULL\n");
}
EXPORT_SYMBOL(tqlp9624_switch_spk);

static int tqlp9624_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	int flags = 0;

	snd_dbg("%s\n", __func__);
	/* ADC master clock frequency */
	if (readl(tqlp9624->base + ADCSR2) != tqlp9624->clk_sel) {
		writel(tqlp9624->clk_sel, tqlp9624->base + ADCSR2);
		writel(tqlp9624->clk_sel, tqlp9624->base + DACSR3);
		flags++;
	}

	/* ADC sampling frequency select */
	if (readl(tqlp9624->base + I2SC0R12) != tqlp9624->sample_rate) {
		writel(tqlp9624->sample_rate, tqlp9624->base + I2SC0R12);
		writel(tqlp9624->sample_rate, tqlp9624->base + I2SC1R13);
		flags++;
	}

	if (flags) {
		writel(0, tqlp9624->base + RSTR0);
		tqlp9624_latch(codec);
		udelay(100);
		writel(0x3, tqlp9624->base + RSTR0);
		tqlp9624_latch(codec);
		udelay(500);
	}
	return 0;
}

static int tqlp9624_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	snd_dbg("%s\n", __func__);
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		snd_err("tqlp9624 only support slave mode\n");
		return -1;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	default:
		snd_err("tqlp9624 only support I2S transfer\n");
		return -1;
	}
	return 0;
}

struct sample_freq tqlp9624_sel[] = {
	{ 8000,         SAMPLE_FREQ_8000 },
	{ 11025,        SAMPLE_FREQ_11025 },
	{ 12000,        SAMPLE_FREQ_12000 },
	{ 16000,        SAMPLE_FREQ_16000 },
	{ 22025,        SAMPLE_FREQ_22025 },
	{ 24000,        SAMPLE_FREQ_24000 },
	{ 32000,        SAMPLE_FREQ_32000 },
	{ 44100,        SAMPLE_FREQ_44100 },
	{ 48000,        SAMPLE_FREQ_48000 },
	{ 96000,        SAMPLE_FREQ_96000 },
	{ 192000,       SAMPLE_FREQ_192000 },
};

static int tqlp9624_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	int i;

	snd_dbg("%s\n", __func__);

	if (clk_id != CLOCK_SEL_256 && clk_id != CLOCK_SEL_384) {
		snd_err("clk_id must from CLOCK_SEL_256 or CLOCK_SEL_384\n");
		return -1;
	}
	tqlp9624->clk_sel = clk_id;

	for (i = 0; i < ARRAY_SIZE(tqlp9624_sel); i++)
		if (freq == tqlp9624_sel[i].freq) {
			tqlp9624->sample_rate = tqlp9624_sel[i].sample_sel;
			return 0;
		}

	return -1;
}

static int tqlp9624_mute(struct snd_soc_dai *dai, int mute)
{
	return 0;
}

static int tqlp9624_hdmi_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = tqlp9624->hdmien;
	return 0;
}

static int tqlp9624_hdmi_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	tqlp9624->hdmien = ucontrol->value.integer.value[0];
	/*TODO enable hdmi*/
	return 0;
};

static int tqlp9624_spk_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	ucontrol->value.integer.value[0] = tqlp9624->spken;
	return 0;
}

static int tqlp9624_spk_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);

	tqlp9624->spken = ucontrol->value.integer.value[0];
	/*TODO enable spk*/
	hpdetect_spk_on(tqlp9624->spken);
	return 0;
};


#define TQLP9624_FORMAT \
	(SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_LE | \
	 SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_ops tqlp9624_ops = {
	.hw_params          = tqlp9624_pcm_hw_params,
	.set_fmt            = tqlp9624_set_dai_fmt,
	.set_sysclk         = tqlp9624_set_dai_sysclk,
	.digital_mute       = tqlp9624_mute,
};

static struct snd_soc_dai_driver tqlp9624_dai = {
	.name = "tqlp9624_dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = TQLP9624_FORMAT,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = TQLP9624_FORMAT,
	},
	.ops = &tqlp9624_ops,
};

#if 0
#define TEST_BYPASS	1
#endif

struct set_reg tqlp9624_init[] = {
	/* mclk set */
	SET_REG_VAL(ADCSR2,      0x0000,         CTRL_SET_REG),
	SET_REG_VAL(DACSR3,      0x0000,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	SET_REG_VAL(DEFAULT_REG, 500,            CTRL_SET_SLEEP),
	/* power control */
	SET_REG_VAL(PC3R24,      0x00f3,         CTRL_SET_REG),
#if 0
	SET_REG_VAL(PC0R21,      0x00fe,         CTRL_SET_REG),
	SET_REG_VAL(PC1R22,      0x00ff,         CTRL_SET_REG),
	SET_REG_VAL(PC2R23,      0x00ff,         CTRL_SET_REG),
	SET_REG_VAL(PC3R24,      0x00ff,         CTRL_SET_REG),
	SET_REG_VAL(PC4R25,      0x00ff,         CTRL_SET_REG),
	SET_REG_VAL(PC5R26,      0x00ff,         CTRL_SET_REG),
#endif
	/* configure the bypass prechage */
	SET_REG_VAL(SPCR17,      0x0000,         CTRL_SET_REG),
#ifdef TEST_BYPASS
	SET_REG_VAL(PMLCHCR89,   0x0003,         CTRL_SET_REG),
	SET_REG_VAL(PMRCHCR90,   0x0003,         CTRL_SET_REG),
#else
	SET_REG_VAL(PMLCHCR89,   0x0001,         CTRL_SET_REG),
	SET_REG_VAL(PMRCHCR90,   0x0001,         CTRL_SET_REG),
#endif
	/* enable line out */
	SET_REG_VAL(ACLINEOE0R,  0x0001,         CTRL_SET_REG),
	SET_REG_VAL(ACLINEOE1R,  0x0001,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	SET_REG_VAL(DEFAULT_REG, 100,            CTRL_SET_SLEEP),
	/* POWERUP_FASTCHARGE */
	SET_REG_VAL(DAAT2R241,   0x0000,         CTRL_SET_REG),
	SET_REG_VAL(DAAT1R240,   0x0000,         CTRL_SET_REG),
	/* Enable the new FSM for power up */
	SET_REG_VAL(POPSC1R162,  0x0083,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	SET_REG_VAL(DEFAULT_REG, 100,            CTRL_SET_SLEEP),
	/* Power up block and bias generator */
	SET_REG_VAL(PC0R21,      0x0005,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	/* spk default volume */
	SET_REG_VAL(PLCHMVCR52, 0x004e,           CTRL_SET_REG),
	SET_REG_VAL(PRCHMVCR53, 0x004e,           CTRL_SET_REG),
	/* hp default volume */
	SET_REG_VAL(SHDLCHVCR56, 0x0028,           CTRL_SET_REG),
	SET_REG_VAL(SHDRCHVCR57, 0x0028,           CTRL_SET_REG),
	/* record channel choose */
	SET_REG_VAL(RRCHISR72,   0x0004,         CTRL_SET_REG),
	SET_REG_VAL(RLCHISR71,   0x0004,         CTRL_SET_REG),
	/* record default volume */
#ifdef TEST_BYPASS
	SET_REG_VAL(RLCHMVCR36,  0x0000,         CTRL_SET_REG),
	SET_REG_VAL(RRCHMVCR37,  0x0000,         CTRL_SET_REG),
#else
	SET_REG_VAL(RLCHMVCR36,  0x000a,         CTRL_SET_REG),
	SET_REG_VAL(RRCHMVCR37,  0x000a,         CTRL_SET_REG),
#endif
	/* for input micR, record stero */
	SET_REG_VAL(ADCHPMR211,	0x0005,			CTRL_SET_REG),
#ifdef TEST_BYPASS
	/* record pga default value */
	SET_REG_VAL(PGALCHVCR38, 0x003c,         CTRL_SET_REG),
	SET_REG_VAL(PGARCHVCR39, 0x003c,         CTRL_SET_REG),
#else
	SET_REG_VAL(PGALCHVCR38, 0x002a,         CTRL_SET_REG),
	SET_REG_VAL(PGARCHVCR39, 0x002a,         CTRL_SET_REG),
#endif
	SET_REG_VAL(DNGCR206,    0x0007,         CTRL_SET_REG),
#if 0
	/* enable alc circuit */
	SET_REG_VAL(ALCC0R200,   0x0003,			CTRL_SET_REG),
	SET_REG_VAL(ALCC1R201,	0x0007,			CTRL_SET_REG),
	SET_REG_VAL(ALCC2R202,	0x0002,			CTRL_SET_REG),
	SET_REG_VAL(ALCRDCR203,	0x0006,			CTRL_SET_REG),
	SET_REG_VAL(ALCRUCR204,	0x0006,			CTRL_SET_REG),
	SET_REG_VAL(ALCMASGR205,	0x002a,			CTRL_SET_REG),
#endif
	/* format set */
	SET_REG_VAL(I2SC0R12,    0x0000,         CTRL_SET_REG),
	SET_REG_VAL(I2SC1R13,    0x0000,         CTRL_SET_REG),
	SET_REG_VAL(I2SC2R14,    0x0001,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	/* reset data path */
	SET_REG_VAL(RSTR0,       0x0000,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	SET_REG_VAL(DEFAULT_REG, 100,            CTRL_SET_SLEEP),
	SET_REG_VAL(RSTR0,       0x0003,         CTRL_SET_REG),
	SET_REG_VAL(DEFAULT_REG, DEFAULT_VAL,    CTRL_SET_LATCH),
	SET_REG_VAL(DEFAULT_REG, 500,            CTRL_SET_SLEEP),
	SET_REG_VAL(DEFAULT_REG, 6000,           CTRL_SET_SLEEP),
};

static const DECLARE_TLV_DB_SCALE(digital_playback_vol, -12600, 150, 0);
static const DECLARE_TLV_DB_SCALE(analog_playback_vol, -4000, 100, 0);
static const DECLARE_TLV_DB_SCALE(pga_record_vol, -1800, 100, 0);
static const DECLARE_TLV_DB_SCALE(digital_record_vol, -9600, 150, 0);

static const struct snd_kcontrol_new tqlp9624_snd_controls[] = {
	/* HP */
	SOC_DOUBLE("Headset Enable Switch", MC2R31, HEADSET_L_MUTE_SHIFT,
			HEADSET_R_MUTE_SHIFT, 1, 1),
	SOC_DOUBLE_R_TLV("Headset Playback Volume", SHDLCHVCR56,
			SHDRCHVCR57, 0, 0x2e, 0, analog_playback_vol),
	/* SPK */
	SOC_SINGLE_EXT("Speaker Enable Switch",  0, 0, 1, 0,
			tqlp9624_spk_get, tqlp9624_spk_put),
	/* HDMI */
	SOC_SINGLE_EXT("HDMI Enable Switch",  0, 0, 1, 0,
			tqlp9624_hdmi_get, tqlp9624_hdmi_put),
	/* RECORD */
	SOC_DOUBLE("PGA Enable Switch", MC0R29,	PGA_L_MUTE_SHIFT,
			PGA_L_MUTE_SHIFT, 1, 1),
	SOC_DOUBLE_R_TLV("PGA Record Volume", PGALCHVCR38, PGARCHVCR39,
			0, 0x3c, 0, pga_record_vol),
	SOC_DOUBLE("Record Enable Switch", MC0R29, RECORD_L_MUTE_SHIFT,
			RECORD_R_MUTE_SHIFT, 1, 1),
	SOC_DOUBLE_R_TLV("Digital Record Volume", RLCHMVCR36, RRCHMVCR37,
			0, 0x54, 0, digital_record_vol),
	/* BYPASS */
	SOC_DOUBLE("Bypass Enable Switch", MC2R31, BYPASS_L_MUTE_SHIFT,
			BYPASS_R_MUTE_SHIFT, 1, 1),
	SOC_DOUBLE_R_TLV("Bypass Playback Volume", PLCHMVCR52, PRCHMVCR53,
			0, 0x54, 0, digital_playback_vol),
    /* LATCH */
    SOC_SINGLE("Latch Single", LTHR1, 0, 1, 0),
};

static const char *const tqlp9624_pga_l_src_sel[] = {
	"Left input 1 (differential)", "Left input 1 (single ended)", "NONE",
	"Left input 2 (single ended)", "Left microphone (differential)",
	"Left microphone (single ended)"
};

static const char *const tqlp9624_pga_r_src_sel[] = {
	"Right input 1 (differential)", "Right input 1 (single ended)", "NONE",
	"Right input 2 (single ended)", "Right microphone (differential)",
	"Right microphone (single ended)"
};

static const SOC_ENUM_SINGLE_DECL(
	tqlp9624_pga_l_src_enum, RLCHISR71, 0, tqlp9624_pga_l_src_sel);
static const SOC_ENUM_SINGLE_DECL(
	tqlp9624_pga_r_src_enum, RRCHISR72, 0, tqlp9624_pga_r_src_sel);


static const struct snd_kcontrol_new tqlp9624_pga_l_mux_control =
	SOC_DAPM_ENUM("PGA L SRC", tqlp9624_pga_l_src_enum);
static const struct snd_kcontrol_new tqlp9624_pga_r_mux_control =
	SOC_DAPM_ENUM("PGA R SRC", tqlp9624_pga_r_src_enum);

static const char *const tqlp9624_playback_r_src_sel[] = {
	"No Mix", "Right Digital", "Analog",
	"Analog Mix Right Digital", "Left Digital",
	"Right Digital Mix Left Digital",
	"Analog Mix Left Digital",
	"Analog Mix Right&Left Digital"
};

static const char *const tqlp9624_playback_l_src_sel[] = {
	"No Mix", "Left Digital", "Analog",
	"Analog Mix Left Digital", "Right Digital",
	"Left Digital Mix Right Digital",
	"Analog Mix Right Digital",
	"Analog Mix Left&Right Digital"
};

static const SOC_ENUM_SINGLE_DECL(
	tqlp9624_playback_l_src_enum, PMLCHCR89, 0, tqlp9624_playback_l_src_sel);
static const SOC_ENUM_SINGLE_DECL(
	tqlp9624_playback_r_src_enum, PMRCHCR90, 0, tqlp9624_playback_r_src_sel);

static const struct snd_kcontrol_new tqlp9624_playback_l_src_mux_control =
	SOC_DAPM_ENUM("PLAYBACK L SRC", tqlp9624_playback_l_src_enum);	
static const struct snd_kcontrol_new tqlp9624_playback_r_src_mux_control =
	SOC_DAPM_ENUM("PLAYBACK R SRC", tqlp9624_playback_r_src_enum);

static const struct snd_soc_dapm_widget tqlp9624_dapm_widgets[] = {
	/* input */
	SND_SOC_DAPM_INPUT("MICL"),
	SND_SOC_DAPM_INPUT("LINE2L"),
	SND_SOC_DAPM_INPUT("LINE1L"),
	SND_SOC_DAPM_INPUT("LINE1R"),
	SND_SOC_DAPM_INPUT("LINE2R"),
	SND_SOC_DAPM_INPUT("MICR"),
#if 1	
	/* MICBIAS */
	SND_SOC_DAPM_MICBIAS("MIC Bias", PC0R21, PWR_MICBIAS_BIT, 0),
	/* Input Side */	
	SND_SOC_DAPM_MUX("PGA L Mixer", SND_SOC_NOPM, 0, 0,
			&tqlp9624_pga_l_mux_control),
	SND_SOC_DAPM_MUX("PGA R Mixer", SND_SOC_NOPM, 0, 0,
			&tqlp9624_pga_r_mux_control),
    SND_SOC_DAPM_PGA("PGA L IN", SND_SOC_NOPM, 0, 0, NULL, 0),
    SND_SOC_DAPM_PGA("PGA R IN", SND_SOC_NOPM, 0, 0, NULL, 0),
    SND_SOC_DAPM_SUPPLY("PGA L Supply", PC1R22, 2, 0, NULL, 0),
    SND_SOC_DAPM_SUPPLY("PGA R Supply", PC1R22, 3, 0, NULL, 0),
    /* ADC and DAC */
	SND_SOC_DAPM_ADC("Right ADC", "Right Capture", SND_SOC_NOPM, 0, 0),   
	SND_SOC_DAPM_ADC("Left ADC", "Left Capture", SND_SOC_NOPM, 0, 0),     
	SND_SOC_DAPM_DAC("Right DAC", "Right Playback", SND_SOC_NOPM, 0, 0),  
	SND_SOC_DAPM_DAC("Left DAC", "Left Playback", SND_SOC_NOPM, 0, 0),    
	SND_SOC_DAPM_SUPPLY("Right ADC Supply", PC1R22, 1, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Left ADC Supply", PC1R22, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Right DAC Supply", PC3R24,	1, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Left DAC Supply", PC3R24, 0, 0, NULL, 0),
    /* Output Side */
	SND_SOC_DAPM_MUX("Playback L Channel", SND_SOC_NOPM, 0, 0,
			&tqlp9624_playback_l_src_mux_control),
	SND_SOC_DAPM_MUX("Playback R Channel", SND_SOC_NOPM, 0, 0,
			&tqlp9624_playback_r_src_mux_control),
	SND_SOC_DAPM_SUPPLY("Left Line Output", SND_SOC_NOPM, 0, 0, NULL, 0), 
	SND_SOC_DAPM_SUPPLY("Right Line Output", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Left Headset", SND_SOC_NOPM, 0, 0, NULL, 0),     
	SND_SOC_DAPM_SUPPLY("Right Headset", SND_SOC_NOPM, 0,  0, NULL, 0),   
#if 0
    SND_SOC_DAPM_SUPPLY("Left Line Output", PC3R24, 7, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Right Line Output", PC3R24, 6, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Left Headset", PC3R24, 5, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Right Headset", PC3R24, 4, 0, NULL, 0),
#endif
#else
	SND_SOC_DAPM_MUX("PGA L Mixer", SND_SOC_NOPM, 0, 0,                   
			&tqlp9624_pga_l_mux_control),                           
	SND_SOC_DAPM_MUX("PGA R Mixer", SND_SOC_NOPM, 0, 0,                   
			&tqlp9624_pga_r_mux_control),                           
	SND_SOC_DAPM_ADC("Right ADC", "Right Capture", SND_SOC_NOPM, 0, 0),   
	SND_SOC_DAPM_ADC("Left ADC", "Left Capture", SND_SOC_NOPM, 0, 0),     
	SND_SOC_DAPM_DAC("Right DAC", "Right Playback", SND_SOC_NOPM, 0, 0),  
	SND_SOC_DAPM_DAC("Left DAC", "Left Playback", SND_SOC_NOPM, 0, 0),    
	SND_SOC_DAPM_MUX("Playback L Channel", SND_SOC_NOPM, 0, 0,      
			&tqlp9624_playback_l_src_mux_control),                  
	SND_SOC_DAPM_MUX("Playback R Channel", SND_SOC_NOPM, 0, 0,      
			&tqlp9624_playback_r_src_mux_control),      
	SND_SOC_DAPM_SUPPLY("Left Line Output", SND_SOC_NOPM, 0, 0, NULL, 0), 
	SND_SOC_DAPM_SUPPLY("Right Line Output", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Left Headset", SND_SOC_NOPM, 0, 0, NULL, 0),     
	SND_SOC_DAPM_SUPPLY("Right Headset", SND_SOC_NOPM, 0,  0, NULL, 0),   
#endif
	SND_SOC_DAPM_POST("Latch Signal", tqlp9624_latch_signal),	
	/* output */
	SND_SOC_DAPM_OUTPUT("HSOUTL"),
	SND_SOC_DAPM_OUTPUT("HSOUTR"),
	SND_SOC_DAPM_OUTPUT("LOUTLP"),
	SND_SOC_DAPM_OUTPUT("LOUTLN"),
	SND_SOC_DAPM_OUTPUT("LOUTRP"),
	SND_SOC_DAPM_OUTPUT("LOUTRN"),

};

static const struct snd_soc_dapm_route tqlp9624_dapm_routes[] = {
	{"PGA L Mixer", "Left input 1 (differential)", "MICL"},
	{"PGA L Mixer", "Left input 1 (single ended)", "MICL"},
	{"PGA L Mixer", "Left input 2 (single ended)", "LINE2L"},
	{"PGA L Mixer", "Left microphone (differential)", "LINE1L"},
	{"PGA L Mixer", "Left microphone (single ended)", "LINE1L"},
	{"PGA R Mixer", "Right input 1 (differential)", "LINE1R"},
	{"PGA R Mixer", "Right input 1 (single ended)", "LINE1R"},
	{"PGA R Mixer", "Right input 2 (single ended)", "LINE2R"},
	{"PGA R Mixer", "Right microphone (differential)", "MICR"},
	{"PGA R Mixer", "Right microphone (single ended)", "MICR"},

    {"PGA L IN", NULL, "PGA L Mixer"},
    {"PGA L IN", NULL, "PGA L Supply"},
    {"PGA R IN", NULL, "PGA R Mixer"},
    {"PGA R IN", NULL, "PGA R Supply"},

    {"Left ADC", NULL, "PGA L Mixer"},
    {"Left ADC", NULL, "PGA L Supply"},
    {"Left ADC", NULL, "Left ADC Supply"},
    {"Right ADC", NULL, "PGA R Mixer"},
    {"Right ADC", NULL, "PGA R Supply"},
    {"Right ADC", NULL, "Right ADC Supply"},

    {"Left DAC", NULL, "PGA L Mixer"},
    {"Left DAC", NULL, "Left DAC Supply"},
    {"Right DAC", NULL, "PGA R Mixer"},
    {"Right DAC", NULL, "Right DAC Supply"},

    {"Playback R Channel", "No Mix", "Right DAC"},
    {"Playback R Channel", "Right Digital", "Right DAC"},
    {"Playback R Channel", "Analog", "PGA R IN"},
    {"Playback R Channel", "Analog Mix Right Digital", "Right DAC"},
    {"Playback R Channel", "Analog Mix Right Digital", "PGA R IN"},
    {"Playback R Channel", "Left Digital", "Left DAC"},
    {"Playback R Channel", "Right Digital Mix Left Digital", "Left DAC"},
    {"Playback R Channel", "Right Digital Mix Left Digital", "Right DAC"},
    {"Playback R Channel", "Analog Mix Left Digital", "Right DAC"},
    {"Playback R Channel", "Analog Mix Left Digital", "PGA R IN"},
    {"Playback R Channel", "Analog Mix Right&Left Digital", "Right DAC"},
    {"Playback R Channel", "Analog Mix Right&Left Digital", "Left DAC"},
    {"Playback R Channel", "Analog Mix Right&Left Digital", "PGA R IN"},
    {"Playback L Channel", "No Mix", "Left DAC"},
    {"Playback L Channel", "Left Digital", "Left DAC"},
    {"Playback L Channel", "Analog", "PGA L IN"},
    {"Playback L Channel", "Analog Mix Left Digital", "Left DAC"},
    {"Playback L Channel", "Analog Mix Left Digital", "PGA L IN"},
    {"Playback L Channel", "Right Digital", "Right DAC"},
    {"Playback L Channel", "Left Digital Mix Right Digital", "Left DAC"},
    {"Playback L Channel", "Left Digital Mix Right Digital", "Right DAC"},
    {"Playback L Channel", "Analog Mix Right Digital", "Right DAC"},
    {"Playback L Channel", "Analog Mix Right Digital", "PGA L IN"},
    {"Playback L Channel", "Analog Mix Left&Right Digital", "Left DAC"},
    {"Playback L Channel", "Analog Mix Left&Right Digital", "Right DAC"},
    {"Playback L Channel", "Analog Mix Left&Right Digital", "PGA L IN"},

    //	{"Left Line Output", "Headset Enable Switch", "Playback L Channel"},
    //	{"Right Line Output", "Headset Enable Switch", "Playback R Channel"},
    //	{"Left Headset", "Headset Enable Switch", "Playback L Channel"},
    //	{"Right Headset", "Headset Enable Switch", "Playback R Channel"},

    {"HSOUTR", NULL, "Playback R Channel"},
    {"HSOUTL", NULL, "Playback L Channel"},
    {"LOUTRP", NULL, "Playback R Channel"},
    {"LOUTRN", NULL, "Playback R Channel"},
    {"LOUTLP", NULL, "Playback L Channel"},
    {"LOUTLN", NULL, "Playback L Channel"},

    {"HSOUTR", NULL, "Left Headset"},
    {"HSOUTL", NULL, "Right Headset"},
    {"LOUTLP", NULL, "Left Line Output"},
    {"LOUTLN", NULL, "Left Line Output"},
    {"LOUTRP", NULL, "Right Line Output"},
	{"LOUTRN", NULL, "Right Line Output"},
};

static void tqlp9624_reset(struct snd_soc_codec *codec)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	void __iomem *sysbase = IO_ADDRESS(SYSMGR_TQLP9624_BASE);

	snd_dbg("%s\n", __func__);
	/* StartupSequence
	 *
	 * 1. acodec interface enable connect to IIS
	 * 2. codec reset
	 * 3. pdz set to 0, rst_z set to 0
	 * 4. acodec latch
	 * 5. codec out of reset
	 **/
	writel(0x1, sysbase + ACODEC_INTF_CTRL);
	writel(0x2, sysbase + ACODEC_SOFT_RST);
	writel(0x0, tqlp9624->base + PC0R21);
	tqlp9624_latch(codec);
	writel(0x0, sysbase + ACODEC_SOFT_RST);

	udelay(500);
}

static int tqlp9624_code_init(struct snd_soc_codec *codec)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	int i;

	snd_dbg("%s\n", __func__);

	writel(CLK_SYS_MODE, tqlp9624->base + MCSSR);
	/* for acodec control clock */
	writel(0x8, tqlp9624->base + RACDR);
	writel(CLK_IIS_MODE, tqlp9624->base + MCSSR);

	tqlp9624_reset(codec);

	for (i = 0; i < ARRAY_SIZE(tqlp9624_init); i++) {
		if (tqlp9624_init[i].mode == CTRL_SET_SLEEP)
			udelay(tqlp9624_init[i].val);
		else if (tqlp9624_init[i].mode == CTRL_SET_LATCH)
			tqlp9624_latch(codec);
		else
			writel(tqlp9624_init[i].val, tqlp9624->base +
					tqlp9624_init[i].reg);
	}
	return 0;
}

static int tqlp9624_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
	struct tqlp9624_priv *tqlp9624 = snd_soc_codec_get_drvdata(codec);
	uint32_t val;

	snd_dbg("%s: %d\n", __func__, level);
	switch (level) {
	case SND_SOC_BIAS_ON:
		/* open headset */
		val = readl(tqlp9624->base + MC2R31);
		val &= ~(0x3 << 4);
		writel(val, tqlp9624->base + MC2R31);
		/* open spk */
		writel(0, tqlp9624->base + MC4R33);
		break;
	case SND_SOC_BIAS_PREPARE:
		/* pull up mic bias */
		val = readl(tqlp9624->base + PC0R21);
		val |= (0x1 << 1);
		writel(val, tqlp9624->base + PC0R21);
		break;
	case SND_SOC_BIAS_OFF:
		/* the pdz bit signal should be set to low */
		val = readl(tqlp9624->base + PC0R21);
		val &= ~0x1;
		writel(val, tqlp9624->base + PC0R21);
		break;
	case SND_SOC_BIAS_STANDBY:
		if (codec->dapm.bias_level == SND_SOC_BIAS_OFF) {
			writel(0x1, tqlp9624->base + PC0R21);
			tqlp9624_code_init(codec);
		}
		break;
	default:
		snd_dbg("%s: Not support this mode %d\n", __func__,
				level);
	}
	codec->dapm.bias_level = level;

	return 0;
}

static int tqlp9624_probe(struct snd_soc_codec *codec)
{
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;

	snd_dbg("%s\n", __func__);
	tqlp9624_codec = codec;

	tqlp9624_code_init(codec);

	codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;
	snd_soc_add_codec_controls(codec, tqlp9624_snd_controls,
			ARRAY_SIZE(tqlp9624_snd_controls));
	snd_soc_dapm_new_controls(dapm, tqlp9624_dapm_widgets,
			ARRAY_SIZE(tqlp9624_dapm_widgets));
	snd_soc_dapm_add_routes(dapm, tqlp9624_dapm_routes,
			ARRAY_SIZE(tqlp9624_dapm_routes));
	ret = device_create_file(codec->dev, &dev_attr_index_reg);
	if (ret < 0) {
		dev_err(codec->dev,
				"Failed to create sysfs files: %d\n", ret);
		return -1;
	}

	snd_dbg("tqlp9624 init ok\n");
	return 0;
}

static int  tqlp9624_remove(struct snd_soc_codec *codec)
{
	snd_dbg("%s\n", __func__);
	tqlp9624_set_bias_level(codec, SND_SOC_BIAS_OFF);
	device_remove_file(codec->dev, &dev_attr_index_reg);
	snd_soc_dapm_free(&codec->dapm);
	return 0;
}

#ifdef CONFIG_PM
static int tqlp9624_suspend(struct snd_soc_codec *codec)
{
	snd_dbg("%s\n", __func__);
    hpdetect_suspend();
	tqlp9624_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int tqlp9624_resume(struct snd_soc_codec *codec)
{
	snd_dbg("%s\n", __func__);
	hpdetect_resume();
	module_power_on(SYSMGR_TQLP9624_BASE);
	resume_i2s_clk();
	tqlp9624_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}
#else
#define tqlp9624_resume         NULL
#define tqlp9624_suspend        NULL
#endif

static struct snd_soc_codec_driver imap_tqlp9624 = {
	.probe =        tqlp9624_probe,
	.remove =       tqlp9624_remove,
	.suspend =      tqlp9624_suspend,
	.resume =       tqlp9624_resume,
	.read	=		tqlp9624_read,
	.write	=		tqlp9624_write,
	.set_bias_level = tqlp9624_set_bias_level,
};

static int imapx_tqlp9624_probe(struct platform_device *pdev)
{
	struct tqlp9624_priv *tqlp9624;
	struct resource *res;
	struct codec_cfg *tqlp9624_cfg;
	int err;

	module_power_on(SYSMGR_TQLP9624_BASE);
	
	tqlp9624_cfg = pdev->dev.platform_data;
	if (!tqlp9624_cfg) {
		dev_err(&pdev->dev, "Tqlp9624 get platform_data failed\n");
		return -EINVAL;
	}

	tqlp9624 = kzalloc(sizeof(struct tqlp9624_priv), GFP_KERNEL);
	if (!tqlp9624) {
		dev_err(&pdev->dev, "unable to allocate mem\n");
		return -ENOMEM;
	}
	tqlp9624->cfg = tqlp9624_cfg;
	/*TODO: mod by david potentially*/
	if (strcmp(tqlp9624_cfg->power_pmu, "NOPMU")) {
		tqlp9624->regulator = regulator_get(NULL, tqlp9624_cfg->power_pmu);
		if (IS_ERR(tqlp9624->regulator)) {
			dev_err(&pdev->dev, "%s: get regulator fail\n", __func__);
			return -1;
		}
		err = regulator_set_voltage(tqlp9624->regulator, 3300000, 3300000);
		if (err) {
			dev_err(&pdev->dev, "%s: set regulator fail\n", __func__);
			return -1;
		}

		err = regulator_enable(tqlp9624->regulator);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get I/O memory\n");
		err = -ENXIO;
		goto free_mem;
	}

	res = request_mem_region(res->start, resource_size(res), pdev->name);
	if (!res) {
		dev_err(&pdev->dev, "failed to request I/O memory\n");
		err = -EBUSY;
		goto free_mem;
	}

	tqlp9624->base = ioremap(res->start, resource_size(res));
	if (!tqlp9624->base) {
		dev_err(&pdev->dev, "failed to remap I/O memory\n");
		err = -ENXIO;
		goto free_mem;
	}

	platform_set_drvdata(pdev, tqlp9624);
	
	hpdetect_init(tqlp9624_cfg);
	return snd_soc_register_codec(&pdev->dev,
			&imap_tqlp9624, &tqlp9624_dai, 1);
free_mem:
	kfree(tqlp9624);
	return err;
}

static int imapx_tqlp9624_remove(struct platform_device *pdev)
{
	struct tqlp9624_priv *tqlp9624 =
		platform_get_drvdata(pdev);

	iounmap(tqlp9624->base);
	kfree(tqlp9624);

	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver imapx_tqlp9624_driver = {
	.driver = {
		.name = "tqlp9624",
		.owner = THIS_MODULE,
	},
	.probe = imapx_tqlp9624_probe,
	.remove = imapx_tqlp9624_remove,
};

static int __init imapx_tqlp9624_modinit(void)
{
	return platform_driver_register(&imapx_tqlp9624_driver);
}
module_init(imapx_tqlp9624_modinit);

static void __exit imapx_tqlp9624_exit(void)
{
	platform_driver_unregister(&imapx_tqlp9624_driver);
}
module_exit(imapx_tqlp9624_exit);

MODULE_DESCRIPTION("ASoC TQLP9624 driver");
MODULE_AUTHOR("Sun");
MODULE_LICENSE("GPL");
