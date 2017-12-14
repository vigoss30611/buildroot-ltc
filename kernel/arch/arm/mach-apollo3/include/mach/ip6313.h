#ifndef __IP6313_H__
#define __IP6313_H__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/regulator/machine.h>

#define IP6313_DEBUG_IF
#define IP6313_ERR_FLAG		1
#define IP6313_INFO_FLAG	1
#define IP6313_DEBUG_FLAG	0

#if IP6313_ERR_FLAG
#define IP6313_ERR(format, args...)  printk(KERN_ERR "[IP6313]"format, ##args)
#else
#define IP6313_ERR(format, args...)  do {} while (0)
#endif

#if IP6313_INFO_FLAG
#define IP6313_INFO(format, args...)  printk(KERN_INFO "[IP6313]"format, ##args)
#else
#define IP6313_INFO(format, args...)  do {} while (0)
#endif

#if IP6313_DEBUG_FLAG
#define IP6313_DBG(format, args...)  printk(KERN_DEBUG "[IP6313]"format, ##args)
#else
#define IP6313_DBG(format, args...)  do {} while (0)
#endif

#define IP6313_PMIC_IRQ              (213)
#define IP6313_DEFAULT_INT_MASK (IP6313_ONOFF_SUPER_SHORT_INT | IP6313_ONOFF_SHORT_INT | IP6313_ONOFF_LONG_INT)

/* Registers */
#define IP6313_PSTATE_CTL0		0x00
#define IP6313_PSTATE_CTL1		0x01
#define IP6313_PSTATE_CTL3		0x03
#define IP6313_PSTATE_SET		0x04
#define IP6313_PROTECT_CTL0		0x06
#define IP6313_LDO_OCFLAG		0x0c
#define IP6313_DCDC_OCFLAG		0x0d
#define IP6313_LDO_GOOD			0x0e
#define IP6313_PWRON_REC0		0x10
#define IP6313_PWROFF_REC0		0x11
#define IP6313_PWROFF_REC1		0x12
#define IP6313_POFF_LDO			0x18
#define IP6313_POFF_DCDC		0x19
#define IP6313_WDOG_CTL			0x1a
#define IP6313_LDO_MASK			0x1b
#define IP6313_PWRON_REC1		0x1c

#define IP6313_DC_CTL			0x20
#define IP6313_DC1_VSET			0x21
#define IP6313_DC2_VSET			0x26

#define IP6313_LDO_EN			0x40
#define IP6313_LDOSW_EN			0x41
#define IP6313_LDO2_VSET		0x42
#define IP6313_LDO3_VSET		0x43
#define IP6313_LDO4_VSET		0x44
#define IP6313_LDO5_VSET		0x45
#define IP6313_LDO6_VSET		0x46
#define IP6313_LDO7_VSET		0x47
#define IP6313_SLDO1_2_VSET		0x4d

#define IP6313_CHG_DIG_CTL0		0x53
#define IP6313_CHG_DIG_CTL3		0x58

#define IP6313_ADC_ANA_CTL0		0x60
#define IP6313_ADC_DATA_VBAT		0x64
#define IP6313_ADC_DATA_IBAT		0x65
#define IP6313_ADC_DATA_ICHG		0x66
#define IP6313_ADC_DATA_GP1		0x67
#define IP6313_ADC_DATA_GP2		0x68

#define IP6313_INTS_CTL			0x70
#define IP6313_INT_FLAG0		0x71
#define IP6313_INT_FLAG1		0x72
#define IP6313_INT_MASK0		0x73
#define IP6313_INT_MASK1		0x74
#define IP6313_MFP_CTL0			0x75
#define IP6313_MFP_CTL1			0x76
#define IP6313_MFP_CTL2			0x77
#define IP6313_GPIO_OE0			0x78
#define IP6313_GPIO_OE1			0x79
#define IP6313_GPIO_IE0			0x7a
#define IP6313_GPIO_IE1			0x7b
#define IP6313_GPIO_DAT0		0x7c
#define IP6313_GPIO_DAT1		0x7d
#define IP6313_PAD_PU0			0x7e
#define IP6313_PAD_PU1			0x7f
#define IP6313_PAD_PD0			0x80
#define IP6313_PAD_PD1			0x81
#define IP6313_PAD_CTL			0x82
#define IP6313_INT_PENDING0		0x83
#define IP6313_INT_PENDING1		0x84
#define IP6313_PAD_DRV0			0x85
#define IP6313_PAD_DRV1			0x86

#define IP6313_ADDR_CTL			0x99

#define IP6313_RTC_CTL			0xa0
#define IP6313_RTC_SEC_ALM		0xa1
#define IP6313_RTC_MIN_ALM		0xa2
#define IP6313_RTC_HOUR_ALM		0xa3
#define IP6313_RTC_DATE_ALM             0xa4
#define IP6313_RTC_MON_ALM              0xa5
#define IP6313_RTC_YEAR_ALM             0xa6
#define IP6313_RTC_SEC			0xa7
#define IP6313_RTC_MIN			0xa8
#define IP6313_RTC_HOUR			0xa9
#define IP6313_RTC_DATE            	0xaa
#define IP6313_RTC_MON             	0xab
#define IP6313_RTC_YEAR            	0xac

#define IP6313_IRC_CTL			0xb0
#define IP6313_IRC_STAT			0xb1
#define IP6313_IRC_CC			0xb2
#define IP6313_IRC_WKDC			0xb3
#define IP6313_IRC_KDC0			0xb4
#define IP6313_IRC_KDC1			0xb5
#define IP6313_IRC_RCC0			0xb6
#define IP6313_IRC_RCC1			0xb7
/*-------------------------------Bit definition for IP6313_INTS_MASK register---------------------*/
#define IP6313_ONOFF_SHORT_INT               (0x1)
#define IP6313_ONOFF_LONG_INT                (0x2)
#define IP6313_ONOFF_SUPER_SHORT_INT               (0x4)

struct ip6313 {
	struct device		*dev;
	struct i2c_client	*client;
	struct mutex		io_lock;
	struct input_dev	*ip6313_pwr;
	int			irq;
};

enum ip6313_regulator_id {
	IP6313_ID_dc1 = 0,
	IP6313_ID_dc2,
	IP6313_ID_sldo1,
	IP6313_ID_ldo2,
	IP6313_ID_ldo3,
	IP6313_ID_ldo4,
	IP6313_ID_ldo5,
	IP6313_ID_ldo6,
	IP6313_ID_ldo7,
};

struct ip6313_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct ip6313_platform_data {
	struct ip6313_subdev_info	*subdevs;
	int				num_subdevs;
	void				*ip;
};

struct ip6313_regulator_platform_data {
		struct regulator_init_data	regulator;
};

#define IP6313_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg)		\
	static struct ip6313_regulator_platform_data pdata_ip6313_##_name	\
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
				ARRAY_SIZE(ip6313_##_name##_supply),	\
			.consumer_supplies =				\
				ip6313_##_name##_supply,		\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define IP6313_REGULATOR(_id)			\
{						\
	.id		= IP6313_ID_##_id,	\
	.name		= "ip6313-regulator",	\
	.platform_data	= &pdata_ip6313_##_id,		\
}

#define IP6313_PWRKEY				\
{						\
	.id		= -1,			\
	.name		= "ip6313-pwrkey",	\
	.platform_data	= NULL,			\
}

static struct regulator_consumer_supply ip6313_dc1_supply[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};
static struct regulator_consumer_supply ip6313_dc2_supply[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};
static struct regulator_consumer_supply ip6313_sldo1_supply[] = {
	REGULATOR_SUPPLY("sldo1", NULL),
};
static struct regulator_consumer_supply ip6313_ldo2_supply[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};
static struct regulator_consumer_supply ip6313_ldo3_supply[] = {
	REGULATOR_SUPPLY("ldo3", NULL),
};
static struct regulator_consumer_supply ip6313_ldo4_supply[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};
static struct regulator_consumer_supply ip6313_ldo5_supply[] = {
	REGULATOR_SUPPLY("ldo5", NULL),
};
static struct regulator_consumer_supply ip6313_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};
static struct regulator_consumer_supply ip6313_ldo7_supply[] = {
	REGULATOR_SUPPLY("ldo7", NULL),
};

IP6313_PDATA_INIT(dc1, 600, 3600, NULL);
IP6313_PDATA_INIT(dc2, 600, 3600, NULL);
IP6313_PDATA_INIT(sldo1, 700, 3800, NULL);
IP6313_PDATA_INIT(ldo2, 700, 3400, NULL);
IP6313_PDATA_INIT(ldo3, 700, 3400, NULL);
IP6313_PDATA_INIT(ldo4, 700, 3400, NULL);
IP6313_PDATA_INIT(ldo5, 700, 3400, NULL);
IP6313_PDATA_INIT(ldo6, 700, 3400, NULL);
IP6313_PDATA_INIT(ldo7, 700, 3400, NULL);

static struct ip6313_subdev_info ip6313_subdevs[] = {
	IP6313_REGULATOR(dc1),
	IP6313_REGULATOR(dc2),
	IP6313_REGULATOR(sldo1),
	IP6313_REGULATOR(ldo2),
	IP6313_REGULATOR(ldo3),
	IP6313_REGULATOR(ldo4),
	IP6313_REGULATOR(ldo5),
	IP6313_REGULATOR(ldo6),
	IP6313_REGULATOR(ldo7),
	IP6313_PWRKEY,
};

static struct ip6313_platform_data ip6313_platform = {
	.subdevs	= ip6313_subdevs,
	.num_subdevs	= ARRAY_SIZE(ip6313_subdevs),
	.ip		= NULL,
};

extern void ip6313_reset(void);
extern void ip6313_deep_sleep(void);
extern int ip6313_write(struct device *dev, uint8_t reg, uint8_t val);
extern int ip6313_read(struct device *dev, uint8_t reg, uint8_t *val);
int ip6313_set_bits(struct device *dev, uint8_t reg, uint8_t mask);
int ip6313_clr_bits(struct device *dev, uint8_t reg, uint8_t mask);

#endif
