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
#include <linux/regulator/consumer.h>
#include <mach/hw_cfg.h>
#include <linux/io.h>
#include "hpdetect.h"
#include <mach/ip2906.h>
#include <linux/clk.h>

struct ip2906_priv {
	enum snd_soc_control_type control_type;
	struct regulator *regulator;
	struct codec_cfg *cfg;
};

struct ip2906_reg_val {
	uint8_t reg;
	uint16_t val;
};

int ip2906_reg[] = {
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,
	0x06,	0x07,	0x08,	0x1f,
	0x20,	0x21,	0x22,	0x23,	0x24,	0x25,
	0x26,	0x27,	0x30,	0x31,	0x32,	0x33,
	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,
	0x40,	0x41,	0x42,	0x43,	0x44,	0x45,
	0x46,	0x47,	0x48,	0x49,	0x4a,	0x4b,
	0x4c,	0x4d,	0x4e,	0x4f,	0x50,	0x51,
};

struct snd_soc_codec *ip2906_codec;
struct i2c_client *ip2906_client = NULL;

int ip2906_i2c_write(struct i2c_client *client,
		uint8_t reg, uint16_t val)
{
	int ret;
	uint8_t buf[3];
	struct i2c_msg msg;

	buf[0] = reg;
	buf[1] = (uint8_t)((val & 0xff00) >> 8);
	buf[2] = (uint8_t)(val & 0xff);

	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = 3;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret < 0) {
		IP2906_ERR("failed to write 0x%x to 0x%x\n", val, reg);
		return -1;
	}

	return 0;
}

int ip2906_i2c_read(struct i2c_client *client,
		uint8_t reg, uint16_t *val)
{
	int ret;
	uint8_t buf[2];
	struct i2c_msg msg[2];

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = &reg;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = buf;
	msg[1].len = 2;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0) {
		IP2906_ERR("failed to read 0x%x\n", reg);
		return -1;
	}

	*val = (buf[0] << 8) | buf[1];

	return 0;
}

int ip2906_write(uint8_t reg, uint16_t val)
{
	int ret;
	if (ip2906_client)
		ret = ip2906_i2c_write(ip2906_client, reg, val);
	else
		ret = -1;

	return ret;
}

int ip2906_read(uint8_t reg, uint16_t *val)
{
	int ret;
	if (ip2906_client)
		ret = ip2906_i2c_read(ip2906_client, reg, val);
	else
		ret = -1;

	return ret;
}

static ssize_t ip2906_reg_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	uint8_t reg;
	uint16_t val;
	uint32_t tmp;

	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp <= 0xff) {
		reg = tmp;
		ip2906_read(reg, &val);
		printk("0x%x: 0x%x\n", reg, val);
	} else if (tmp > 0xff && tmp <= 0xffffff) {
		val = tmp & 0xffff;
		reg = (tmp >> 16) & 0xff;
		ip2906_write(reg, val);
	} else {
		printk("ERROR, out of the range\n");
	}

	return count;
}

static ssize_t ip2906_reg_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int i, ret;
	uint16_t val;

	for (i = 0; i < ARRAY_SIZE(ip2906_reg); ++i) {
		ip2906_read(ip2906_reg[i], &val);
		printk("0x%x: 0x%x\n", ip2906_reg[i], val);
	}

	return 0;
}
static DEVICE_ATTR(index_reg, 0644, ip2906_reg_show, ip2906_reg_store);

struct ip2906_reg_val ip2906_init_val[] = {
	{IP2906_MFP_PAD,	0x0100},
	{IP2906_CMU_CLKEN,	0x0001},
	{IP2906_DEV_RST,	0x0000},
	{IP2906_DEV_RST,	0x0007},

	{IP2906_IIS_CONFIG,	0x0283},
//	{IP2906_IIS_TXMIX,	0x00a0},
	{IP2906_ADC_CTL,	0x0003},
	{IP2906_ADC_VOL,	0xdede},
	{IP2906_DAC_CTL,	0x0183},
	{IP2906_DAC_VOL,	0xbebe},
	{IP2906_DAC_MIX,	0x0840},

	{IP2906_AUI_CTL1,	0x0707},
	{IP2906_AUI_CTL2,	0x008f},
	{IP2906_AUI_CTL3,	0x0011},
	{IP2906_AUI_CTL4,	0x015d},
	{IP2906_AUO_CTL1,	0x003f},
	{IP2906_AUO_CTL2,	0x10a0},
//	{IP2906_AUO_CTL4,	0x015d},
	{IP2906_AUO_CTL5,	0x07cf},

//	{IP2906_DEV_RST,	0x0007},
};

static int ip2906_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
#define WIDTH_MSK   (1 << 5)
#define USE_16BITS  (1 << 5)
#define USE_32BITS  (0 << 5)

    uint32_t format = 0;
    uint32_t ctrldata = 0;
    static uint32_t prev_format = -1;

    format = params_format(params);
    if (likely(format == prev_format)) {
        return 0; /* same format, not set it again */
    }
    prev_format = format;

    ctrldata = snd_soc_read(dai->codec, IP2906_IIS_CONFIG);
    ctrldata &= ~(WIDTH_MSK);

	switch (format) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
        ctrldata |= USE_16BITS;  /* 16bits */
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
        ctrldata |= USE_32BITS;  /* 32bits */
		break;
	}
    snd_soc_write(dai->codec, IP2906_IIS_CONFIG, ctrldata);
	return 0;
}

static int ip2906_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	return 0;
}

static int ip2906_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	return 0;
}

static int ip2906_mute(struct snd_soc_codec *codec, int mute)
{
	return 0;
}

static int ip2906_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	return 0;
}

static int ip2906_codec_init(struct snd_soc_codec *codec)
{
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(ip2906_init_val); ++i) {
		ret = ip2906_write(ip2906_init_val[i].reg, ip2906_init_val[i].val);
		if (ret < 0) {
			IP2906_ERR("init reg: %x failed\n", ip2906_init_val[i].reg);
			return ret;
		}
	}

	return 0;
}

static struct snd_soc_dai_ops ip2906_ops = {
	.hw_params	= ip2906_pcm_hw_params,
	.set_fmt	= ip2906_set_dai_fmt,
	.set_sysclk	= ip2906_set_dai_sysclk,
	.digital_mute = ip2906_mute,
	.set_clkdiv	= ip2906_set_dai_clkdiv,
};

#define ip2906_RATES (SNDRV_PCM_RATE_192000 |\
		SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_48000 |\
		SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_16000 |\
		SNDRV_PCM_RATE_8000  | SNDRV_PCM_RATE_88200 |\
		SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_22050 |\
		SNDRV_PCM_RATE_11025)
#define ip2906_FORMATS (SNDRV_PCM_FMTBIT_S32_LE |\
		SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S16_LE)

static struct snd_soc_dai_driver ip2906_dai = {
	.name = "ip2906_dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = ip2906_RATES,
//		.formats = SNDRV_PCM_FMTBIT_S32_LE,
		.formats = ip2906_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = ip2906_RATES,
//		.formats = SNDRV_PCM_FMTBIT_S32_LE,
		.formats = ip2906_FORMATS,
	},
	.ops = &ip2906_ops,
};

static int ip2906_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
	return 0;
}

static int ip2906_probe(struct snd_soc_codec *codec)
{
	struct ip2906_priv *ip2906 = snd_soc_codec_get_drvdata(codec);
	int ret;
	uint16_t val;

	ret = ip2906_codec_init(codec);
//	ret = device_create_file(codec->dev, &dev_attr_index_reg);

	return 0;
}

static int ip2906_remove(struct snd_soc_codec *codec)
{
//	device_remove_file(codec->dev, &dev_attr_index_reg);

	return 0;
}

static int ip2906_suspend(struct snd_soc_codec *codec)
{
	return 0;
}

static int ip2906_resume(struct snd_soc_codec *codec)
{
	return 0;
}

static unsigned int ip2906_codec_read(struct snd_soc_codec *codec, unsigned int reg)
{
    unsigned short reg_data = 0;
    ip2906_read(reg, &reg_data);
    return reg_data;
}

static int ip2906_codec_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int data)
{
    unsigned short reg_data = (unsigned short)data;
    ip2906_write(reg, reg_data);
    return 0;
}

static struct snd_soc_codec_driver imap_ip2906 = {
	.probe		= ip2906_probe,
	.remove		= ip2906_remove,
	.suspend	= ip2906_suspend,
	.resume		= ip2906_resume,
	.set_bias_level	= ip2906_set_bias_level,
    .read = ip2906_codec_read,
    .write = ip2906_codec_write,
};

static int ip2906_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct ip2906_priv *ip2906;
	struct codec_cfg *ip2906_cfg;
	struct clk *ip2906_clk;
	int ret;
	uint16_t val;

	ip2906 = kzalloc(sizeof(struct ip2906_priv), GFP_KERNEL);
	if (ip2906 == NULL) {
		IP2906_ERR("ip2906 alloc failed\n");
		return -ENOMEM;
	}

	ip2906_cfg = client->dev.platform_data;
	if (ip2906_cfg == NULL) {
		IP2906_ERR("get platform data failed\n");
		return -EINVAL;
	}

	if (strcmp(ip2906_cfg->power_pmu, "NOPMU")) {
		ip2906->regulator = regulator_get(&client->dev, ip2906_cfg->power_pmu);
		if (IS_ERR(ip2906->regulator)) {
			IP2906_ERR("get regulator failed\n");
			return -1;
		}
		ret = regulator_set_voltage(ip2906->regulator, 3300000, 3300000);
		if (ret) {
			IP2906_ERR("set regulator failed\n");
			return -1;
		}
		regulator_enable(ip2906->regulator);
	}

	if (strlen(ip2906_cfg->clk_src)) {
		IP2906_INFO("clk_src=%s\n", ip2906_cfg->clk_src);
		ip2906_clk = clk_get_sys(ip2906_cfg->clk_src, ip2906_cfg->clk_src);
		if (IS_ERR(ip2906_clk)) {
			IP2906_ERR("get clk failed\n");
			return -1;
		}
		if (clk_set_rate(ip2906_clk, 24000000)) {
			IP2906_ERR("set clk rate failed\n");
			return -1;
		}
		if (clk_prepare_enable(ip2906_clk)) {
			IP2906_ERR("enable clk failed\n");
			return -1;
		}
		if (!strncmp(ip2906_cfg->clk_src, "imap-clk1", 9)) {
			IP2906_INFO("clk_src is imap-clk1, enable CLK_OUT pad\n");
			val = ioread32(IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);
			iowrite32(val | (1 << 3), IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);
		}
	}

	hpdetect_init(ip2906_cfg);
	ip2906_client = client;
	i2c_set_clientdata(client, ip2906);
	ip2906->control_type = SND_SOC_I2C;

	ret = device_create_file(&client->dev, &dev_attr_index_reg);
	//ip2906_reg_show();
	ret = snd_soc_register_codec(&client->dev, &imap_ip2906,
			&ip2906_dai, 1);
	if (ret < 0) {
		IP2906_ERR("cannot register codec\n");
		kfree(ip2906);
	}

	return ret;
}

static int ip2906_i2c_remove(struct i2c_client *client)
{
	device_remove_file(&client->dev, &dev_attr_index_reg);
	snd_soc_unregister_codec(&client->dev);
	kfree(i2c_get_clientdata(client));

	return 0;
}

static const struct i2c_device_id ip2906_i2c_id[] = {
	{ "ip2906", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, ip2906_i2c_id);

static struct i2c_driver ip2906_i2c_driver = {
	.driver = {
		.name = "ip2906",
		.owner = THIS_MODULE,
	},
	.probe	= ip2906_i2c_probe,
	.remove	= ip2906_i2c_remove,
	.id_table = ip2906_i2c_id,
};

static int __init ip2906_modinit(void)
{
	return i2c_add_driver(&ip2906_i2c_driver);
}
module_init(ip2906_modinit);

static int __exit ip2906_exit(void)
{
	i2c_del_driver(&ip2906_i2c_driver);
}
module_exit(ip2906_exit);

MODULE_DESCRIPTION("ASoc IP2906 driver");
MODULE_LICENSE("GPL");
