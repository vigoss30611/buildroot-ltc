#ifndef __IP6303_H__
#define __IP6303_H__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/regulator/machine.h>

#define IP6303_DEBUG_IF
#define IP6303_ERR_FLAG		1
#define IP6303_INFO_FLAG	1
#define IP6303_DEBUG_FLAG	1

#if IP6303_ERR_FLAG
#define IP6303_ERR(format, args...)  printk(KERN_ERR "[IP6303]"format, ##args)
#else
#define IP6303_ERR(format, args...)  do {} while (0)
#endif

#if IP6303_INFO_FLAG
#define IP6303_INFO(format, args...)  printk(KERN_INFO "[IP6303]"format, ##args)
#else
#define IP6303_INFO(format, args...)  do {} while (0)
#endif

#if IP6303_DEBUG_FLAG
#define IP6303_DBG(format, args...)  printk(KERN_DEBUG "[IP6303]"format, ##args)
#else
#define IP6303_DBG(format, args...)  do {} while (0)
#endif

#define IP6303_PMIC_IRQ              (213)
#define IP6303_DEFAULT_INT_MASK (IP6303_ONOFF_SUPER_SHORT_INT | IP6303_ONOFF_SHORT_INT | IP6303_ONOFF_LONG_INT)

/* Registers */
#define IP6303_PSTATE_CTL0		0x00
#define IP6303_PSTATE_CTL1		0x01
#define IP6303_PSTATE_CTL3		0x03
#define IP6303_PSTATE_SET		0x04
#define IP6303_PROTECT_CTL0		0x06
#define IP6303_LDO_OCFLAG		0x0c
#define IP6303_DCDC_OCFLAG		0x0d
#define IP6303_LDO_GOOD			0x0e
#define IP6303_PWRON_REC0		0x10
#define IP6303_PWROFF_REC0		0x11
#define IP6303_PWROFF_REC1		0x12
#define IP6303_POFF_LDO			0x18
#define IP6303_POFF_DCDC		0x19
#define IP6303_WDOG_CTL			0x1a
#define IP6303_LDO_MASK			0x1b
#define IP6303_PWRON_REC1		0x1c

#define IP6303_DC_CTL			0x20
#define IP6303_DC1_VSET			0x21
#define IP6303_DC2_VSET			0x26
#define IP6303_DC3_VSET			0x2b

#define IP6303_LDO_EN			0x40
#define IP6303_LDOSW_EN			0x41
#define IP6303_LDO2_VSET		0x42
#define IP6303_LDO3_VSET		0x43
#define IP6303_LDO4_VSET		0x44
#define IP6303_LDO5_VSET		0x45
#define IP6303_LDO6_VSET		0x46
#define IP6303_LDO7_VSET		0x47
#define IP6303_SLDO1_2_VSET		0x4d

#define IP6303_CHG_DIG_CTL0		0x53
#define IP6303_CHG_DIG_CTL3		0x58

#define IP6303_ADC_ANA_CTL0		0x60
#define IP6303_ADC_DATA_VBAT		0x64
#define IP6303_ADC_DATA_IBAT		0x65
#define IP6303_ADC_DATA_ICHG		0x66
#define IP6303_ADC_DATA_GP1		0x67
#define IP6303_ADC_DATA_GP2		0x68

#define IP6303_INTS_CTL			0x70
#define IP6303_INT_FLAG0		0x71
#define IP6303_INT_FLAG1		0x72
#define IP6303_INT_MASK0		0x73
#define IP6303_INT_MASK1		0x74
#define IP6303_MFP_CTL0			0x75
#define IP6303_MFP_CTL1			0x76
#define IP6303_MFP_CTL2			0x77
#define IP6303_GPIO_OE0			0x78
#define IP6303_GPIO_OE1			0x79
#define IP6303_GPIO_IE0			0x7a
#define IP6303_GPIO_IE1			0x7b
#define IP6303_GPIO_DAT0		0x7c
#define IP6303_GPIO_DAT1		0x7d
#define IP6303_PAD_PU0			0x7e
#define IP6303_PAD_PU1			0x7f
#define IP6303_PAD_PD0			0x80
#define IP6303_PAD_PD1			0x81
#define IP6303_PAD_CTL			0x82
#define IP6303_INT_PENDING0		0x83
#define IP6303_INT_PENDING1		0x84
#define IP6303_PAD_DRV0			0x85
#define IP6303_PAD_DRV1			0x86

#define IP6303_ADDR_CTL			0x99

#define IP6303_RTC_CTL			0xa0
#define IP6303_RTC_SEC_ALM		0xa1
#define IP6303_RTC_MIN_ALM		0xa2
#define IP6303_RTC_HOUR_ALM		0xa3
#define IP6303_RTC_DATE_ALM             0xa4
#define IP6303_RTC_MON_ALM              0xa5
#define IP6303_RTC_YEAR_ALM             0xa6
#define IP6303_RTC_SEC			0xa7
#define IP6303_RTC_MIN			0xa8
#define IP6303_RTC_HOUR			0xa9
#define IP6303_RTC_DATE            	0xaa
#define IP6303_RTC_MON             	0xab
#define IP6303_RTC_YEAR            	0xac

#define IP6303_IRC_CTL			0xb0
#define IP6303_IRC_STAT			0xb1
#define IP6303_IRC_CC			0xb2
#define IP6303_IRC_WKDC			0xb3
#define IP6303_IRC_KDC0			0xb4
#define IP6303_IRC_KDC1			0xb5
#define IP6303_IRC_RCC0			0xb6
#define IP6303_IRC_RCC1			0xb7
/*-------------------------------Bit definition for IP6303_INTS_MASK register---------------------*/
#define IP6303_ONOFF_SHORT_INT               (0x1)
#define IP6303_ONOFF_LONG_INT                (0x2)
#define IP6303_ONOFF_SUPER_SHORT_INT               (0x4)

struct ip6303 {
	struct device		*dev;
	struct i2c_client	*client;
	struct mutex		io_lock;
	struct input_dev	*ip6303_pwr;
	int			irq;
};

enum ip6303_regulator_id {
	IP6303_ID_dc1 = 0,
	IP6303_ID_dc2,
	IP6303_ID_dc3,
	IP6303_ID_sldo1,
	IP6303_ID_ldo2,
	IP6303_ID_ldo3,
	IP6303_ID_ldo4,
	IP6303_ID_ldo5,
	IP6303_ID_ldo6,
	IP6303_ID_ldo7,
};

struct ip6303_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct ip6303_platform_data {
	struct ip6303_subdev_info	*subdevs;
	int				num_subdevs;
	void				*ip;
};

struct ip6303_regulator_platform_data {
		struct regulator_init_data	regulator;
};

#define IP6303_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg)		\
	static struct ip6303_regulator_platform_data pdata_ip6303_##_name	\
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
				ARRAY_SIZE(ip6303_##_name##_supply),	\
			.consumer_supplies =				\
				ip6303_##_name##_supply,		\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define IP6303_REGULATOR(_id)			\
{						\
	.id		= IP6303_ID_##_id,	\
	.name		= "ip6303-regulator",	\
	.platform_data	= &pdata_ip6303_##_id,		\
}

#define IP6303_PWRKEY				\
{						\
	.id		= -1,			\
	.name		= "ip6303-pwrkey",	\
	.platform_data	= NULL,			\
}

static struct regulator_consumer_supply ip6303_dc1_supply[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};
static struct regulator_consumer_supply ip6303_dc2_supply[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};
static struct regulator_consumer_supply ip6303_dc3_supply[] = {
	REGULATOR_SUPPLY("dc3", NULL),
};
static struct regulator_consumer_supply ip6303_sldo1_supply[] = {
	REGULATOR_SUPPLY("sldo1", NULL),
};
static struct regulator_consumer_supply ip6303_ldo2_supply[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};
static struct regulator_consumer_supply ip6303_ldo3_supply[] = {
	REGULATOR_SUPPLY("ldo3", NULL),
};
static struct regulator_consumer_supply ip6303_ldo4_supply[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};
static struct regulator_consumer_supply ip6303_ldo5_supply[] = {
	REGULATOR_SUPPLY("ldo5", NULL),
};
static struct regulator_consumer_supply ip6303_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};
static struct regulator_consumer_supply ip6303_ldo7_supply[] = {
	REGULATOR_SUPPLY("ldo7", NULL),
};

IP6303_PDATA_INIT(dc1, 600, 3600, NULL);
IP6303_PDATA_INIT(dc2, 600, 3600, NULL);
IP6303_PDATA_INIT(dc3, 600, 3600, NULL);
IP6303_PDATA_INIT(sldo1, 700, 3800, NULL);
IP6303_PDATA_INIT(ldo2, 700, 3400, NULL);
IP6303_PDATA_INIT(ldo3, 700, 3400, NULL);
IP6303_PDATA_INIT(ldo4, 700, 3400, NULL);
IP6303_PDATA_INIT(ldo5, 700, 3400, NULL);
IP6303_PDATA_INIT(ldo6, 700, 3400, NULL);
IP6303_PDATA_INIT(ldo7, 700, 3400, NULL);

static struct ip6303_subdev_info ip6303_subdevs[] = {
	IP6303_REGULATOR(dc1),
	IP6303_REGULATOR(dc2),
	IP6303_REGULATOR(dc3),
	IP6303_REGULATOR(sldo1),
	IP6303_REGULATOR(ldo2),
	IP6303_REGULATOR(ldo3),
	IP6303_REGULATOR(ldo4),
	IP6303_REGULATOR(ldo5),
	IP6303_REGULATOR(ldo6),
	IP6303_REGULATOR(ldo7),
	//IP6303_PWRKEY,
};

static struct ip6303_platform_data ip6303_platform = {
	.subdevs	= ip6303_subdevs,
	.num_subdevs	= ARRAY_SIZE(ip6303_subdevs),
	.ip		= NULL,
};

extern void ip6303_reset(void);
extern void ip6303_deep_sleep(void);
extern int ip6303_write(struct device *dev, uint8_t reg, uint8_t val);
extern int ip6303_read(struct device *dev, uint8_t reg, uint8_t *val);
int ip6303_set_bits(struct device *dev, uint8_t reg, uint8_t mask);
int ip6303_clr_bits(struct device *dev, uint8_t reg, uint8_t mask);

#endif
