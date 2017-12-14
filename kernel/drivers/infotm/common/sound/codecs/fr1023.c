#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/regulator/consumer.h>
#include <linux/io.h>
#include <mach/hw_cfg.h>
#include <linux/debugfs.h>
#include "hpdetect.h"
#include "fr1023.h"
#include <mach/pad.h>

#define AUDIO_NAME "fr1023"

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


struct fr1023_priv {
    enum snd_soc_control_type control_type;
    unsigned int sysclk;
    int spken;
    struct regulator *regulator;
    struct delayed_work reg_test;
    struct delayed_work init_work;
#ifdef CONFIG_DEBUG_FS
    struct dentry                   *debugfs;
#endif
};

struct snd_soc_codec *fr1023_codec;
static int init_work_count = 0;

static const u8 fr1023_reg[] = {
    0x00,   0x01,   0x02,   0x03,   0x04,   0x05,
    0x06,   0x07,   0x08,   0x09,   0x0a,   0x0b,
    0x0c,   0x0d,   0x0e,   0x0f,   0x10,   0x11,
    0x12,   0x13,   0x14,   0x15,   0x16,   0x17,
    0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,
    0x1e,   0x1f,   0x20,   0x21,   0x22,   0x23,
};

static int fr1023_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
        int div_id, int div)
{
	printk("Enter fr1023_set_dai_clkdiv\n");
	return 0;
}

#if defined(CONFIG_DEBUG_FS)

static ssize_t  fr1023_show_regs(struct file *file, char __user *user_buf,
		                        size_t count, loff_t *ppos) {
	struct fr1023_priv *fr1023 = file->private_data;
	struct snd_soc_codec *codec = fr1023_codec;
	int i;
	for (i = 0; i < (sizeof(fr1023_reg)); ++i)
		pr_err("[fr1023] register 0x%02x : 0x%02x.\n", fr1023_reg[i], snd_soc_read(codec, fr1023_reg[i]));
	return 0;
}

static const struct file_operations fr1023_regs_ops = {
	.owner          = THIS_MODULE,
	.open           = simple_open,
	.read           = fr1023_show_regs,
	.llseek         = default_llseek,
};


static int fr1023_debugfs_init(struct fr1023_priv *fr1023)
{
	fr1023->debugfs = debugfs_create_dir("fr1023", NULL);
	if(!fr1023->debugfs)
		        return -ENOMEM;
	 debugfs_create_file("registers", S_IFREG | S_IRUGO,
			 fr1023->debugfs, (void *)fr1023, &fr1023_regs_ops);
	  return 0;

}
#endif

static int fr1023_set_dai_sysclk(struct snd_soc_dai *dai,
        int clk_id, unsigned int freq, int dir)
{
    struct snd_soc_codec *codec = dai->codec;
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);
    printk("Enter fr1023_set_dai_sysclk %d\n", freq);
    switch (freq) {
        case 11289600:
        case 12000000:
        case 12288000:
        case 16934400:
        case 18432000:
            fr1023->sysclk = freq;
            return 0;
    }
    return -EINVAL;
}

static int fr1023_set_dai_fmt(struct snd_soc_dai *codec_dai,
        unsigned int fmt) {
	struct snd_soc_codec *codec = fr1023_codec;
	
	printk("Enter fr1023_set_dai_fmt\n");
	return 0;
}

static int fr1023_pcm_hw_params(struct snd_pcm_substream *substream,
        struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = fr1023_codec;
	printk("%s params channels = %d\n", __func__, params_channels(params));
	return 0;
}

static int fr1023_mute(struct snd_soc_codec *codec,
        enum snd_soc_bias_level level)
{
	return 0;
}

static int fr1023_set_bias_level(struct snd_soc_codec *codec,
        enum snd_soc_bias_level level)
{
    return 0;
}

#define fr1023_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
        SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |\
        SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 |\
        SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

#define fr1023_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
        SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops fr1023_ops = {
    .hw_params  = fr1023_pcm_hw_params,
    .set_fmt    = fr1023_set_dai_fmt,
    .set_sysclk = fr1023_set_dai_sysclk,
    .digital_mute = fr1023_mute,
    .set_clkdiv = fr1023_set_dai_clkdiv,
};

static struct snd_soc_dai_driver fr1023_dai = {
    .name = "fr1023_dai",
    .playback = {
        .stream_name = "Playback",
        .channels_min = 2,
        .channels_max = 2,
        .rates = fr1023_RATES,
        .formats = fr1023_FORMATS,
    },
    .capture = {
        .stream_name = "Capture",
        .channels_min = 2,
        .channels_max = 2,
        .rates = fr1023_RATES,
        .formats = fr1023_FORMATS,
    },
    .ops = &fr1023_ops
};

struct fr1023_reg_val {
    unsigned char reg;
    unsigned char val;
};

struct fr1023_reg_val fr1023_init_val[] = {
    {0x09, 0x02},
    {0x0a, 0x80},
//    {0x0b, 0x03},
    {0x0e, 0xcb},
    {0x10, 0x70},
    {0x12, 0x00},
//    {0x12, 0x0c},
    {0x13, 0x00},
//    {0x13, 0x0f},    
    {0x14, 0x0f},
    {0x15, 0x00},
//    {0x16, 0x7f},
    {0x16, 0x38},
    {0x17, 0x00},
    //{0x23, 0x09},
    {0x23, 0x0b},
};

struct fr1023_reg_val fr1023_exit_val[] = {
    {0x15, 0xff},
    {0x16, 0xff},
    {0x17, 0xff},
};

static int fr1023_spk_get(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);

    ucontrol->value.integer.value[0] = fr1023->spken;
printk("[fr1023] register 0x12 : 0x%02x.\n", snd_soc_read(codec, 0x12));
printk("[fr1023] register 0x13 : 0x%02x.\n", snd_soc_read(codec, 0x13)); 
    return 0;
}

static int fr1023_spk_put(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);

    fr1023->spken = ucontrol->value.integer.value[0];
    hpdetect_spk_on(fr1023->spken);

    return 0;
}

static const struct snd_kcontrol_new fr1023_snd_controls[] = {
    SOC_SINGLE_EXT("Speaker Enable Switch", 0, 0, 1, 0,
            fr1023_spk_get, fr1023_spk_put),
    	SOC_DOUBLE_R("Speaker Playback Volume", FR1023_CONTROL18, FR1023_CONTROL19, 0, 63, 1),


};

static const struct snd_soc_dapm_widget fr1023_dapm_widgets[] = {
    /* input */
    SND_SOC_DAPM_INPUT("MIC"),
//    SND_SOC_DAPM_INPUT("MICR"),
    
    /* MICBIAS */
    SND_SOC_DAPM_MICBIAS("MIC Bias", FR1023_CONTROL22, PWR_MIC_BIT, 1),
    /* Input Side */
    SND_SOC_DAPM_PGA("PGA Stage 1", FR1023_CONTROL22, PGA_STAGE_1_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_PGA("PGA Stage 2", FR1023_CONTROL22, PGA_STAGE_2_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_PGA("PGA Stage Inverter", FR1023_CONTROL22, PGA_STAGE_INV_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_ADC("Left ADC", "Left Capture", SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_ADC("Right ADC", "Right Capture", SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_SUPPLY("Left ADC Supply", FR1023_CONTROL22, ADC_CHANNEL, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Right ADC Supply", FR1023_CONTROL22, ADC_CHANNEL, 1, NULL, 0),
    /* Output Side */
    SND_SOC_DAPM_DAC("Left DAC", "Left Playback", SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_DAC("Right DAC", "Right Playback", SND_SOC_NOPM, 0, 0),
    SND_SOC_DAPM_SUPPLY("Left DAC Supply", FR1023_CONTROL23, LEFT_DAC_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Right DAC Supply", FR1023_CONTROL23, RIGHT_DAC_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_MIXER("Left Mix", SND_SOC_NOPM, 0, 0, NULL, 0),
    SND_SOC_DAPM_MIXER("Right Mix", SND_SOC_NOPM, 0, 0, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Left Mix Supply", FR1023_CONTROL23, LEFT_MIX_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Right Mix Supply", FR1023_CONTROL23, RIGHT_MIX_SHIFT, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Left Output", FR1023_CONTROL23, LEFT_OUTPUT, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Right Output", FR1023_CONTROL23, RIGHT_OUTPUT, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("PA Output", FR1023_CONTROL22, PA_OUTPUT, 1, NULL, 0),
//    SND_SOC_DAPM_SUPPLY("BAIS Power", FR1023_CONTROL22, BAIS_POWER, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Left VMID Buffer", FR1023_CONTROL23, LEFT_VMID_BUFFER, 1, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Right VMID Buffer", FR1023_CONTROL23, RIGHT_VMID_BUFFER, 1, NULL, 0),

    /* clock */
    SND_SOC_DAPM_SUPPLY("Clock Enable", FR1023_CONTROL10, CLOCK_SHIFT, 0, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Clock INT", FR1023_CONTROL11, CLOCK_INT_SHIFT, 0, NULL, 0),
    SND_SOC_DAPM_SUPPLY("Clock DEC", FR1023_CONTROL11, CLOCK_DEC_SHIFT, 0, NULL, 0),

    /*  output */
    SND_SOC_DAPM_OUTPUT("LOUT"),
    SND_SOC_DAPM_OUTPUT("ROUT"),
};

static const struct snd_soc_dapm_route fr1023_dapm_routes [] = {
    {"MIC Bias", NULL, "MIC"},

    {"PGA Stage 1", NULL, "MIC Bias"},
    {"PGA Stage 2", NULL, "MIC Bias"},
    {"PGA Stage Inverter", NULL, "MIC Bias"},

    {"Left ADC", NULL, "PGA Stage 1"},
    {"Left ADC", NULL, "PGA Stage 2"},
    {"Left ADC", NULL, "PGA Stage Inverter"},
    {"Left ADC", NULL, "Left ADC Supply"},
    {"Left ADC", NULL, "Clock Enable"},
    {"Left ADC", NULL, "Clock DEC"},
//    {"Left ADC", NULL, "BAIS Power"},
    {"Right ADC", NULL, "PGA Stage 1"},
    {"Right ADC", NULL, "PGA Stage 2"},
    {"Right ADC", NULL, "PGA Stage Inverter"},
    {"Right ADC", NULL, "Right ADC Supply"},
    {"Right ADC", NULL, "Clock Enable"},
    {"Right ADC", NULL, "Clock DEC"},
//    {"Right ADC", NULL, "BAIS Power"},

    {"Left DAC", NULL, "Left DAC Supply"},
    {"Left DAC", NULL, "Clock Enable"},
    {"Left DAC", NULL, "Clock INT"},
    {"Right DAC", NULL, "Right DAC Supply"},
    {"Right DAC", NULL, "Clock Enable"},
    {"Right DAC", NULL, "Clock INT"},

    {"Left Mix", NULL, "Left DAC"},
    {"Left Mix", NULL, "Left Mix Supply"},
    {"Left Mix", NULL, "Right Mix Supply"},
    {"Right Mix", NULL, "Right DAC"},
    {"Right Mix", NULL, "Right Mix Supply"},
    {"Right Mix", NULL, "Left Mix Supply"},

    {"LOUT", NULL, "Left Mix"},
    {"ROUT", NULL, "Right Mix"},

    {"LOUT", NULL, "Left Output"},
    {"LOUT", NULL, "PA Output"},
    {"LOUT", NULL, "Left VMID Buffer"},
//    {"LOUT", NULL, "BAIS Power"},
    {"ROUT", NULL, "Right Output"},
    {"ROUT", NULL, "PA Output"},
    {"ROUT", NULL, "Right VMID Buffer"},
//    {"ROUT", NULL, "BAIS Power"},
};

static int fr1023_codec_init(struct snd_soc_codec *codec)
{
    int i, ret;
    fr1023_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

    for (i = 0; i < (sizeof(fr1023_init_val)/2); ++i) {
        ret = snd_soc_write(codec, fr1023_init_val[i].reg, fr1023_init_val[i].val);
        if (ret < 0)
            return ret;
    }

    return 0;
}

static void codec_init_work(struct work_struct *work)
{
    struct fr1023_priv *fr1023 = container_of(to_delayed_work(work), struct fr1023_priv, init_work);
    struct snd_soc_codec *codec = fr1023_codec;
    int i, ret;

    if (init_work_count > 10)
        return;

    ret = fr1023_codec_init(codec);
    init_work_count++;
    if (ret < 0) {
        err("codec init failed.\n");
        schedule_delayed_work(&fr1023->init_work, msecs_to_jiffies(800));
    }
}

static void reg_show(struct work_struct *work)
{
    struct fr1023_priv *fr1023 = container_of(to_delayed_work(work), struct fr1023_priv, reg_test);
    struct snd_soc_codec *codec = fr1023_codec;
    int i, ret;
#if 0
    for (i = 0; i < (sizeof(fr1023_init_val)/2); ++i)
        printk("[fr1023] register 0x%0x : 0x%0x.\n", fr1023_init_val[i].reg, snd_soc_read(codec, fr1023_init_val[i].reg));
    schedule_delayed_work(&fr1023->reg_test, msecs_to_jiffies(10000));
#endif
    for (i = 0; i < (sizeof(fr1023_reg)); ++i)
        printk("[fr1023] register 0x%0x : 0x%0x.\n", fr1023_reg[i], snd_soc_read(codec, fr1023_reg[i]));
    //ret = fr1023_codec_init(codec);
    schedule_delayed_work(&fr1023->reg_test, msecs_to_jiffies(10000));
    if (ret < 0) {
        err("fr1023 codec init failed\n");
    }
}

static void imapx15_test_reg_show(void)
{
    struct snd_soc_codec *codec = fr1023_codec;
    int i;
    for (i = 0; i < (sizeof(fr1023_reg)); ++i)
        printk("[fr1023] register 0x%02x : 0x%02x.\n", fr1023_reg[i], snd_soc_read(codec, fr1023_reg[i]));
}

static int fr1023_suspend(struct snd_soc_codec *codec)
{
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);
    int i;
printk("Enter fr1023_suspend\n");
    cancel_delayed_work_sync(&fr1023->init_work);
    hpdetect_suspend();
    fr1023_set_bias_level(codec, SND_SOC_BIAS_OFF);

    for (i = 0; i < (sizeof(fr1023_exit_val)/2); ++i)
        snd_soc_write(codec, fr1023_exit_val[i].reg, fr1023_exit_val[i].val);

    return 0;
}

static int fr1023_resume(struct snd_soc_codec *codec)
{
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);

    hpdetect_resume();
    fr1023_set_bias_level(codec, SND_SOC_BIAS_ON);
    init_work_count = 1;
    schedule_delayed_work(&fr1023->init_work, msecs_to_jiffies(800));
//    fr1023_codec_init(codec);
    return 0;
}

static int fr1023_probe(struct snd_soc_codec *codec)
{
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);
    struct snd_soc_dapm_context *dapm = &codec->dapm;
    int ret = 0;
    printk("Enter fr1023_probe\n");
    ret = snd_soc_codec_set_cache_io(codec, 8, 8, fr1023->control_type);
    if (ret < 0) {
        err("[FR1023] failed to set cache I/O: %d\n", ret);
        return ret;
    }
    fr1023_codec = codec;

    fr1023_codec_init(codec);
    fr1023_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
    snd_soc_add_codec_controls(codec, fr1023_snd_controls,
            ARRAY_SIZE(fr1023_snd_controls));
#if 1
    snd_soc_dapm_new_controls(dapm, fr1023_dapm_widgets,
            ARRAY_SIZE(fr1023_dapm_widgets));
    snd_soc_dapm_add_routes(dapm, fr1023_dapm_routes,
            ARRAY_SIZE(fr1023_dapm_routes));
#endif
    INIT_DELAYED_WORK(&fr1023->init_work, codec_init_work);
//    schedule_delayed_work(&fr1023->init_work, msecs_to_jiffies(14000));
//    INIT_DELAYED_WORK(&fr1023->reg_test, reg_show);
//    schedule_delayed_work(&fr1023->reg_test, msecs_to_jiffies(50000));

    //imapx15_test_reg_show();
#if defined(CONFIG_DEBUG_FS)
    fr1023_debugfs_init(fr1023);
#endif 
	
    return ret;
}

static int fr1023_remove(struct snd_soc_codec *codec)
{
    struct fr1023_priv *fr1023 = snd_soc_codec_get_drvdata(codec);
    cancel_delayed_work_sync(&fr1023->init_work);
    fr1023_set_bias_level(codec, SND_SOC_BIAS_OFF);
    return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_fr1023 = {
    .probe      = fr1023_probe,
    .remove     = fr1023_remove,
    .suspend    = fr1023_suspend,
    .resume     = fr1023_resume,
    .set_bias_level = fr1023_set_bias_level,
};

static int fr1023_i2c_probe(struct i2c_client *i2c,
        const struct i2c_device_id *id)
{
    struct fr1023_priv *fr1023;
    struct codec_cfg *fr1023_cfg;
    int ret;

    info("Codec dirver is fr1023.\n");

    fr1023 = kzalloc(sizeof(struct fr1023_priv), GFP_KERNEL);
    if (fr1023 == NULL)
        return -ENOMEM;

    fr1023_cfg = i2c->dev.platform_data;
    if (fr1023_cfg == NULL) {
        err("FR1023 get platform_fata failed.\n");
        return -ENOMEM;
    }

    if (strcmp(fr1023_cfg->power_pmu, "NOPMU")) {
	    fr1023->regulator = regulator_get(NULL, fr1023_cfg->power_pmu);
	    if (IS_ERR(fr1023->regulator)) {
		    pr_err("%s: get regulator fail\n", __func__);
		    return -1;
	    }
	    ret = regulator_set_voltage(fr1023->regulator, 3300000, 3300000);
	    if (ret) {
		    pr_err("%s: set regulator fail\n", __func__);
		    return -1;
	    }

	    ret = regulator_enable(fr1023->regulator);
    }


    hpdetect_init(fr1023_cfg);

    i2c_set_clientdata(i2c, fr1023);
    fr1023->control_type = SND_SOC_I2C;

    ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_fr1023, 
            &fr1023_dai, 1); 
    if (ret < 0) {
        err("[FR1023] cannot register codec\n");
        kfree(fr1023);
    }

    return ret;
}

static int fr1023_i2c_remove(struct i2c_client *client)
{
    snd_soc_unregister_codec(&client->dev);
    kfree(i2c_get_clientdata(client));

    return 0;
}

static const struct i2c_device_id fr1023_i2c_id[] = {
    {"fr1023", 0},
    { }
};
MODULE_DEVICE_TABLE(i2c, fr1023_i2c_id);

static struct i2c_driver fr1023_i2c_driver = {
    .driver = {
        .name = "fr1023",
        .owner = THIS_MODULE,
    },
    .probe  = fr1023_i2c_probe,
    .remove = fr1023_i2c_remove,
    .id_table   = fr1023_i2c_id,
};

static int __init fr1023_modinit(void)
{
    return i2c_add_driver(&fr1023_i2c_driver);
}
module_init(fr1023_modinit);

static void __exit fr1023_modexit(void)
{
    i2c_del_driver(&fr1023_i2c_driver);
}
module_exit(fr1023_modexit);

MODULE_DESCRIPTION("Asoc FR1023 driver");
MODULE_LICENSE("GPL");
