#ifndef __IP6208_H__
#define __IP6208_H__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/regulator/machine.h>

#define IP620x_DEBUG_IF
#define IP620x_ERR_FLAG		1
#define IP620x_INFO_FLAG	1
#define IP620x_DEBUG_FLAG	0

#if IP620x_ERR_FLAG
#define IP620x_ERR(format, args...)  printk(KERN_ERR "[IP620x]"format, ##args)
#else
#define IP620x_ERR(format, args...)  do {} while (0)
#endif

#if IP620x_INFO_FLAG
#define IP620x_INFO(format, args...)  printk(KERN_INFO "[IP620x]"format, ##args)
#else
#define IP620x_INFO(format, args...)  do {} while (0)
#endif

#if IP620x_DEBUG_FLAG
#define IP620x_DBG(format, args...)  printk(KERN_DEBUG "[IP620x]"format, ##args)
#else
#define IP620x_DBG(format, args...)  do {} while (0)
#endif

#define IP6208_PMIC_IRQ              (213)
#define IP6208_DEFAULT_INT_MASK (IP6208_ONOFF_SUPER_SHORT_INT | IP6208_ONOFF_SHORT_INT | IP6208_ONOFF_LONG_INT)

/* Registers */
#define IP6208_PSTATE_CTL		0x00
#define IP6208_PSTATE_SET		0x01
#define IP6208_PPATH_CTL		0x02
#define IP6208_PPATH_STATUS		0x03
#define IP6208_PROTECT_CTL1		0x05
#define IP6208_PROTECT_CTL2		0x06
#define IP6208_PM_INT_TH		0x07
#define IP6208_PWRINT_FLAG		0x08
#define IP6208_LDO_OCFLAG		0x09
#define IP6208_DCDC_OCFLAG		0x0a
#define IP6208_DCDC_GOOD		0x0b
#define IP6208_LDO_GOOD			0x0c
#define IP6208_PWRON_CTL		0x0d
#define IP6208_PWRON_REC		0x0f
#define IP6208_PWROFF_REC		0x10
#define IP6208_POFF_LDO			0x16
#define IP6208_POFF_DCDC		0x17
#define IP6208_WDOG_CTL			0x18
#define IP6208_LDO_MASK			0x19
#define IP6208_REV_PMU			0x1b

#define IP6208_CHG_CTL2			0x22
#define IP6208_CHG_CTL3			0x23
#define IP6208_FG_CTL			0x28
#define IP6208_FG_QMAX			0x29
#define IP6208_FG_SOC			0x2a
#define IP6208_FG_DEBUG0		0x2b
#define IP6208_FG_DEBUG1		0x2c
#define IP6208_FG_DEBUG2		0x2d

#define IP6208_LDO_EN			0x30
#define IP6208_LDOSW_EN			0x31
#define IP6208_LDO_DIS			0x32
#define IP6208_LDO2_VSET		0x36
#define IP6208_LDO3_VSET		0x37
#define IP6208_LDO4_VSET		0x38
#define IP6208_LDO5_VSET		0x39
#define IP6208_LDO6_VSET		0x3a
#define IP6208_LDO7_VSET		0x3b
#define IP6208_LDO8_VSET		0x3c
#define IP6208_LDO9_VSET		0x3d
#define IP6208_LDO10_VSET		0x3e
#define IP6208_LDO11_VSET		0x3f
#define IP6208_LDO12_VSET		0x40
#define IP6208_SVCC_CTL			0x42
#define IP6208_SLDO1_CTL		0x43
#define IP6208_DC_CTL			0x50
#define IP6208_DC1_VSET			0x55
#define IP6208_DC2_VSET			0x5a
#define IP6208_DC3_VSET			0x5f
#define IP6208_DC4_VSET			0x64
#define IP6208_DC5_VSET			0x69
#define IP6208_DC6_VSET			0x6e
#define IP6208_DCOV_FLAG		0x77

#define IP6208_ADC_CTL1			0x80
#define IP6208_ADC_CTL2			0x81
#define IP6208_VBAT_ADC_DATA		0x82
#define IP6208_VBUS_ADC_DATA		0x83
#define IP6208_VSYS_ADC_DATA		0x84
#define IP6208_VDCIN_ADC_DATA		0x85
#define IP6208_IBAT_ADC_DATA		0x86
#define IP6208_IBUS_ADC_DATA		0x87
#define IP6208_IDCIN_ADC_DATA		0x88
#define IP6208_TEMP_ADC_DATA		0x89
#define IP6208_GP1_ADC_DATA		0x8a
#define IP6208_GP2_ADC_DATA		0x8b

#define IP6208_INTS_CTL			0xa0
#define IP6208_INT_FLAG			0xa1
#define IP6208_INT_MASK			0xa2
#define IP6208_I2C_ADDR			0xa3
#define IP6208_MFP_CTL1			0xa4
#define IP6208_MFP_CTL2			0xa5
#define IP6208_MFP_CTL3			0xa6
#define IP6208_GPIO_OE			0xa7
#define IP6208_GPIO_IE			0xa8
#define IP6208_GPIO_DAT			0xa9
#define IP6208_PAD_PU			0xaa
#define IP6208_PAD_PD			0xab
#define IP6208_PAD_CTL			0xac
#define IP6208_INT_PENDING1		0xad
#define IP6208_INT_PENDING2		0xae
#define IP6208_PAD_DRV			0xaf

#define IP6208_RTC_CTL			0xc0
#define IP6208_RTC_MSALM		0xc1
#define IP6208_RTC_HALM			0xc2
#define IP6208_RTC_YMDALM		0xc3
#define IP6208_RTC_MS			0xc4
#define IP6208_RTC_H			0xc5
#define IP6208_RTC_YMD			0xc6

#define IP6208_I2S_CONFIG		0xd0
#define IP6208_I2S_TXMIX		0xd2
#define IP6208_PCM_CTL0			0xd3
#define IP6208_PCM_CTL1			0xd4
#define IP6208_PCM_CTL2			0xd5
#define IP6208_ADC_CTL			0xd6
#define IP6208_ADC_VOL			0xd7
#define IP6208_DAC_CTL			0xd8
#define IP6208_DAC_MIX			0xda
#define IP6208_AUDIO_DEBUG		0xdb
#define IP6208_AUI_CTL1			0xe0
#define IP6208_AUI_CTL2			0xe1
#define IP6208_AUI_CTL3			0xe2
#define IP6208_AUI_CTL4			0xe3
#define IP6208_AUI_CTL5			0xe4
#define IP6208_AUO_CTL1			0xf0
#define IP6208_AUO_CTL2			0xf1
#define IP6208_AUO_CTL4			0xf3
#define IP6208_AUO_CTL5			0xf4
/*-------------------------------Bit definition for IP6208_INTS_MASK register---------------------*/
#define IP6208_ONOFF_SHORT_INT               (0x1)
#define IP6208_ONOFF_LONG_INT                (0x2)
#define IP6208_ONOFF_SUPER_SHORT_INT               (0x4)

struct ip620x {
	struct device		*dev;
	struct i2c_client	*client;
	struct mutex		io_lock;
	struct input_dev	*ip6208_pwr;
	int			irq;
};

enum ip6208_regulator_id {
	IP6208_ID_dc1 = 0,
	IP6208_ID_dc2,
	IP6208_ID_dc3,
	IP6208_ID_dc4,
	IP6208_ID_dc5,
	IP6208_ID_dc6,
	IP6208_ID_ldo1,
	IP6208_ID_ldo2,
	IP6208_ID_ldo3,
	IP6208_ID_ldo4,
	IP6208_ID_ldo5,
	IP6208_ID_ldo6,
	IP6208_ID_ldo7,
	IP6208_ID_ldo8,
	IP6208_ID_ldo9,
	IP6208_ID_ldo10,
	IP6208_ID_ldo11,
	IP6208_ID_ldo12,
};

struct ip620x_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct ip620x_platform_data {
	struct ip620x_subdev_info	*subdevs;
	int				num_subdevs;
	void				*ip;
};

struct ip620x_regulator_platform_data {
		struct regulator_init_data	regulator;
};

#define IP6208_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg)		\
	static struct ip620x_regulator_platform_data pdata_ip6208_##_name	\
	=								\
	{								\
		.regulator = {						\
			.constraints = {				\
				.min_uV = (_minmv) * 1000,		\
				.max_uV = (_maxmv) * 1000,		\
				.valid_modes_mask = (REGULATOR_MODE_NORMAL |  \
						     REGULATOR_MODE_STANDBY), \
				.valid_ops_mask = (REGULATOR_CHANGE_MODE |    \
						   REGULATOR_CHANGE_STATUS |  \
						   REGULATOR_CHANGE_VOLTAGE), \
			},						\
			.num_consumer_supplies =			\
				ARRAY_SIZE(ip6208_##_name##_supply),	\
			.consumer_supplies =				\
				ip6208_##_name##_supply,		\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define IP6208_REGULATOR(_id)			\
{						\
	.id		= IP6208_ID_##_id,	\
	.name		= "ip620x-regulator",	\
	.platform_data	= &pdata_ip6208_##_id,		\
}

#define IP6208_PWRKEY				\
{						\
	.id		= -1,			\
	.name		= "ip6208-pwrkey",	\
	.platform_data	= NULL,			\
}

static struct regulator_consumer_supply ip6208_dc1_supply[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};
static struct regulator_consumer_supply ip6208_dc2_supply[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};
static struct regulator_consumer_supply ip6208_dc3_supply[] = {
	REGULATOR_SUPPLY("dc3", NULL),
};
static struct regulator_consumer_supply ip6208_dc4_supply[] = {
	REGULATOR_SUPPLY("dc4", NULL),
};
static struct regulator_consumer_supply ip6208_dc5_supply[] = {
	REGULATOR_SUPPLY("dc5", NULL),
};
static struct regulator_consumer_supply ip6208_dc6_supply[] = {
	REGULATOR_SUPPLY("dc6", NULL),
};
static struct regulator_consumer_supply ip6208_ldo1_supply[] = {
	REGULATOR_SUPPLY("ldo1", NULL),
};
static struct regulator_consumer_supply ip6208_ldo2_supply[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};
static struct regulator_consumer_supply ip6208_ldo3_supply[] = {
	REGULATOR_SUPPLY("ldo3", NULL),
};
static struct regulator_consumer_supply ip6208_ldo4_supply[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};
static struct regulator_consumer_supply ip6208_ldo5_supply[] = {
	REGULATOR_SUPPLY("ldo5", NULL),
};
static struct regulator_consumer_supply ip6208_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};
static struct regulator_consumer_supply ip6208_ldo7_supply[] = {
	REGULATOR_SUPPLY("ldo7", NULL),
};
static struct regulator_consumer_supply ip6208_ldo8_supply[] = {
	REGULATOR_SUPPLY("ldo8", NULL),
};
static struct regulator_consumer_supply ip6208_ldo9_supply[] = {
	REGULATOR_SUPPLY("ldo9", NULL),
};
static struct regulator_consumer_supply ip6208_ldo10_supply[] = {
	REGULATOR_SUPPLY("ldo10", NULL),
};
static struct regulator_consumer_supply ip6208_ldo11_supply[] = {
	REGULATOR_SUPPLY("ldo11", NULL),
};
static struct regulator_consumer_supply ip6208_ldo12_supply[] = {
	REGULATOR_SUPPLY("ldo12", NULL),
};

IP6208_PDATA_INIT(dc1, 600, 2600, NULL);
IP6208_PDATA_INIT(dc2, 600, 2600, NULL);
IP6208_PDATA_INIT(dc3, 600, 3400, NULL);
IP6208_PDATA_INIT(dc4, 600, 3400, NULL);
IP6208_PDATA_INIT(dc5, 600, 2600, NULL);
IP6208_PDATA_INIT(dc6, 600, 3400, NULL);
IP6208_PDATA_INIT(ldo1, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo2, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo3, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo4, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo5, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo6, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo7, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo8, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo9, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo10, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo11, 700, 3400, NULL);
IP6208_PDATA_INIT(ldo12, 700, 3400, NULL);

static struct ip620x_subdev_info ip6208_subdevs[] = {
	IP6208_REGULATOR(dc1),
	IP6208_REGULATOR(dc2),
	IP6208_REGULATOR(dc3),
	IP6208_REGULATOR(dc4),
	IP6208_REGULATOR(dc5),
	IP6208_REGULATOR(dc6),
	IP6208_REGULATOR(ldo1),
	IP6208_REGULATOR(ldo2),
	IP6208_REGULATOR(ldo3),
	IP6208_REGULATOR(ldo4),
	IP6208_REGULATOR(ldo5),
	IP6208_REGULATOR(ldo6),
	IP6208_REGULATOR(ldo7),
	IP6208_REGULATOR(ldo8),
	IP6208_REGULATOR(ldo9),
	IP6208_REGULATOR(ldo10),
	IP6208_REGULATOR(ldo11),
	IP6208_REGULATOR(ldo12),
	IP6208_PWRKEY,
};

static struct ip620x_platform_data ip6208_platform = {
	.subdevs	= ip6208_subdevs,
	.num_subdevs	= ARRAY_SIZE(ip6208_subdevs),
	.ip		= NULL,
};

extern void ip620x_reset(void);
extern void ip620x_deep_sleep(void);
int ip620x_write(struct device *dev, uint8_t reg, uint16_t val);
int ip620x_read(struct device *dev, uint8_t reg, uint16_t *val);
int ip620x_codec_write(uint8_t reg, uint16_t val);
int ip620x_codec_read(uint8_t reg, uint16_t *val);
int ip620x_set_bits(struct device *dev, uint8_t reg, uint16_t mask);
int ip620x_clr_bits(struct device *dev, uint8_t reg, uint16_t mask);

#endif
