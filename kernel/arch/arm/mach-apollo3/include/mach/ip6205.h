#ifndef __IP6205_H__
#define __IP6205_H__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/regulator/machine.h>
#include <mach/ip6208.h>

#define IP620x_DEBUG_IF

#define IP6205_PMIC_IRQ              (213)
#define IP6205_DEFAULT_INT_MASK (IP6205_ONOFF_SUPER_SHORT_INT | IP6205_ONOFF_SHORT_INT | IP6205_ONOFF_LONG_INT|IP6205_DCINPLUG_INT|IP6205_DCOUTPLUG_INT)

/* Registers */
#define IP6205_PSTATE_CTL		0x00
#define IP6205_PSTATE_SET		0x01
#define IP6205_PPATH_CTL		0x02
#define IP6205_PPATH_STATUS		0x03
#define IP6205_PROTECT_CTL1		0x05
#define IP6205_PROTECT_CTL2		0x06
#define IP6205_PM_INT_TH		0x07
#define IP6205_PWRINT_FLAG		0x08
#define IP6205_LDO_OCFLAG		0x09
#define IP6205_DCDC_OCFLAG		0x0a
#define IP6205_DCDC_GOOD		0x0b
#define IP6205_LDO_GOOD			0x0c
#define IP6205_PWRON_CTL		0x0d
#define IP6205_PWRON_REC		0x0f
#define IP6205_PWROFF_REC		0x10
#define IP6205_POFF_LDO			0x16
#define IP6205_POFF_DCDC		0x17
#define IP6205_WDOG_CTL			0x18
#define IP6205_LDO_MASK			0x19
#define IP6205_REV_PMU			0x1b

#define IP6205_LDO_EN			0x30
#define IP6205_LDOSW_EN			0x31
#define IP6205_LDO_DIS			0x32
#define IP6205_LDO2_VSET		0x36
#define IP6205_LDO3_VSET		0x37
#define IP6205_LDO4_VSET		0x38
#define IP6205_LDO6_VSET		0x3a
#define IP6205_LDO7_VSET		0x3b
#define IP6205_LDO8_VSET		0x3c
#define IP6205_LDO9_VSET		0x3d
#define IP6205_SVCC_CTL			0x42
#define IP6205_DC_CTL			0x50
#define IP6205_DC1_VSET			0x55
#define IP6205_DC4_VSET			0x64
#define IP6205_DC5_VSET			0x69
#define IP6205_DCOV_FLAG		0x77

#define IP6205_ADC_CTL1			0x80
#define IP6205_ADC_CTL2			0x81
#define IP6205_VBAT_ADC_DATA		0x82
#define IP6205_VSYS_ADC_DATA		0x84
#define IP6205_VDCIN_ADC_DATA		0x85
#define IP6205_IBUS_ADC_DATA		0x87
#define IP6205_IDCIN_ADC_DATA		0x88
#define IP6205_TEMP_ADC_DATA		0x89
#define IP6205_GP1_ADC_DATA		0x8a
#define IP6205_GP2_ADC_DATA		0x8b

#define IP6205_INTS_CTL                 0xa0//this register is not show in spec, but truely exist
#define IP6205_INT_FLAG			0xa1
#define IP6205_INT_MASK			0xa2
#define IP6205_I2C_ADDR			0xa3
#define IP6205_MFP_CTL1			0xa4
#define IP6205_MFP_CTL2			0xa5
#define IP6205_MFP_CTL3			0xa6
#define IP6205_GPIO_OE			0xa7
#define IP6205_GPIO_IE			0xa8
#define IP6205_GPIO_DAT			0xa9
#define IP6205_PAD_PU			0xaa
#define IP6205_PAD_PD			0xab
#define IP6205_PAD_CTL			0xac
#define IP6205_INT_PENDING1		0xad
#define IP6205_INT_PENDING2		0xae
#define IP6205_PAD_DRV			0xaf

#define IP6205_I2S_CONFIG		0xd0
#define IP6205_I2S_TXMIX		0xd2
#define IP6205_PCM_CTL0			0xd3
#define IP6205_PCM_CTL1			0xd4
#define IP6205_PCM_CTL2			0xd5
#define IP6205_ADC_CTL			0xd6
#define IP6205_ADC_VOL			0xd7
#define IP6205_DAC_CTL			0xd8
#define IP6205_DAC_VOL			0xd9
#define IP6205_DAC_MIX			0xda
#define IP6205_AUDIO_DEBUG		0xdb
#define IP6205_AUI_CTL1			0xe0
#define IP6205_AUI_CTL2			0xe1
#define IP6205_AUI_CTL3			0xe2
#define IP6205_AUI_CTL4			0xe3
#define IP6205_AUI_CTL5			0xe4
#define IP6205_AUO_CTL1			0xf0
#define IP6205_AUO_CTL2			0xf1
#define IP6205_AUO_CTL4			0xf3
#define IP6205_AUO_CTL5			0xf4

/*-------------------------------Bit definition for IP6208_INTS_MASK register---------------------*/ 
#define IP6205_ONOFF_SHORT_INT               (0x1)
#define IP6205_ONOFF_LONG_INT                (0x2)
#define IP6205_ONOFF_SUPER_SHORT_INT               (0x4)
#define IP6205_DCINPLUG_INT               (0x20)
#define IP6205_DCOUTPLUG_INT               (0x40)

enum ip6205_regulator_id {
	IP6205_ID_dc1 = 0,
	IP6205_ID_dc4,
	IP6205_ID_dc5,
	IP6205_ID_ldo2,
	IP6205_ID_ldo3,
	IP6205_ID_ldo4,
	IP6205_ID_ldo6,
	IP6205_ID_ldo7,
	IP6205_ID_ldo8,
	IP6205_ID_ldo9,
};

#define IP6205_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg)		\
	static struct ip620x_regulator_platform_data pdata_ip6205_##_name	\
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
				ARRAY_SIZE(ip6205_##_name##_supply),	\
			.consumer_supplies =				\
				ip6205_##_name##_supply,		\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define IP6205_REGULATOR(_id)			\
{						\
	.id		= IP6205_ID_##_id,	\
	.name		= "ip620x-regulator",	\
	.platform_data	= &pdata_ip6205_##_id,		\
}

#define IP6205_PWRKEY				\
{						\
	.id		= -1,			\
	.name		= "ip6205-pwrkey",	\
	.platform_data	= NULL,			\
}

static struct regulator_consumer_supply ip6205_dc1_supply[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};
static struct regulator_consumer_supply ip6205_dc4_supply[] = {
	REGULATOR_SUPPLY("dc4", NULL),
};
static struct regulator_consumer_supply ip6205_dc5_supply[] = {
	REGULATOR_SUPPLY("dc5", NULL),
};
static struct regulator_consumer_supply ip6205_ldo2_supply[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};
static struct regulator_consumer_supply ip6205_ldo3_supply[] = {
	REGULATOR_SUPPLY("ldo3", NULL),
};
static struct regulator_consumer_supply ip6205_ldo4_supply[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};
static struct regulator_consumer_supply ip6205_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};
static struct regulator_consumer_supply ip6205_ldo7_supply[] = {
	REGULATOR_SUPPLY("ldo7", NULL),
};
static struct regulator_consumer_supply ip6205_ldo8_supply[] = {
	REGULATOR_SUPPLY("ldo8", NULL),
};
static struct regulator_consumer_supply ip6205_ldo9_supply[] = {
	REGULATOR_SUPPLY("ldo9", NULL),
};

IP6205_PDATA_INIT(dc1, 600, 2600, NULL);
IP6205_PDATA_INIT(dc4, 600, 3400, NULL);
IP6205_PDATA_INIT(dc5, 600, 2600, NULL);
IP6205_PDATA_INIT(ldo2, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo3, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo4, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo6, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo7, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo8, 700, 3400, NULL);
IP6205_PDATA_INIT(ldo9, 700, 3400, NULL);

static struct ip620x_subdev_info ip6205_subdevs[] = {
	IP6205_REGULATOR(dc1),
	IP6205_REGULATOR(dc4),
	IP6205_REGULATOR(dc5),
	IP6205_REGULATOR(ldo2),
	IP6205_REGULATOR(ldo3),
	IP6205_REGULATOR(ldo4),
	IP6205_REGULATOR(ldo6),
	IP6205_REGULATOR(ldo7),
	IP6205_REGULATOR(ldo8),
	IP6205_REGULATOR(ldo9),
	IP6205_PWRKEY,
};

static struct ip620x_platform_data ip6205_platform = {
	.subdevs	= ip6205_subdevs,
	.num_subdevs	= ARRAY_SIZE(ip6205_subdevs),
	.ip		= NULL,
};
#endif
