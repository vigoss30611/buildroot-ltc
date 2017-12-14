#ifndef __IP6203_REGISTER__
#define __IP6203_REGISTER__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/regulator/machine.h>

#define IP6203_DEBUG_IF
#define IP6203_ERR_FLAG		1
#define IP6203_INFO_FLAG		1
#define IP6203_DEBUG_FLAG	0
//#define IP6203_LDO1 

#if IP6203_ERR_FLAG
#define IP6203_ERR(format, args...)  printk(KERN_ERR "[IP6203]"format, ##args)
#else
#define IP6203_ERR(format, args...)  do {} while (0)
#endif

#if IP6203_INFO_FLAG
#define IP6203_INFO(format, args...)  printk(KERN_INFO "[IP6203]"format, ##args)
#else
#define IP6203_INFO(format, args...)  do {} while (0)
#endif

#if IP6203_DEBUG_FLAG
#define IP6203_DBG(format, args...)  printk(KERN_ERR "[IP6203]"format, ##args)
#else
#define IP6203_DBG(format, args...)  do {} while (0)
#endif

#define IP6203_PMIC_IRQ              (214)

#define DEFAULT_INT_MASK			(IP6203_ONOFF_PRESS_INT | IP6203_ONOFF_SHORT_INT | IP6203_ONOFF_LONG_INT)
#define INT_MASK_ALL					(IP6203_ONOFF_PRESS_INT | IP6203_ONOFF_SHORT_INT | IP6203_ONOFF_LONG_INT | IP6203_ALARM_INT | IP6203_IRQS_INT)

#define MAX_INTERRUPT_MASKS	11
#define MAX_MAIN_INTERRUPT	8
#define MAX_GPEDGE_REG		1

struct ip6203 {
	struct device		*dev;
	struct i2c_client	*client;
	struct mutex		io_lock;
	struct input_dev *ip6203_pwr;
	int		irq;
};

/* for regulator begin */

enum ip6203_regulator_id {
	IP6203_ID_dc2 = 0,
	IP6203_ID_dc4,
	IP6203_ID_dc5,
#ifdef IP6203_LDO1 
	IP6203_ID_ldo1,
#endif
	IP6203_ID_ldo6,
	IP6203_ID_ldo7,
	IP6203_ID_ldo8,
	IP6203_ID_ldo9,
	IP6203_ID_ldo11,  
	IP6203_ID_ldo12,
};

enum ip6203_dc_nsteps {
	IP6203_DC_NULL,
	IP6203_DC_870UV = 0x40,
	IP6203_DC_210UV = 0x80,
	IP6203_DC_150UV = 0xC0,
};

struct ip6203_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct ip6203_platform_data {
	struct ip6203_subdev_info *subdevs;
	int	num_subdevs;
	void	*ip;
};

struct ip6203_regulator_platform_data {
		struct regulator_init_data regulator;
};

#define IP6203_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg) \
	static struct ip6203_regulator_platform_data pdata_##_name\
	=								\
	{								\
		.regulator = {						\
			.constraints = {				\
				.min_uV = (_minmv)*1000,		\
				.max_uV = (_maxmv)*1000,		\
				.valid_modes_mask = (REGULATOR_MODE_NORMAL |  \
						     REGULATOR_MODE_STANDBY), \
				.valid_ops_mask = (REGULATOR_CHANGE_MODE |    \
						   REGULATOR_CHANGE_STATUS |  \
						   REGULATOR_CHANGE_VOLTAGE), \
			},						\
			.num_consumer_supplies =			\
				ARRAY_SIZE(ip6203_##_name##_supply),\
			.consumer_supplies =				\
				ip6203_##_name##_supply,	\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define IP6203_REGULATOR(_id)				\
{								\
	.id		= IP6203_ID_##_id,			\
	.name		= "ip6203-regulator",			\
	.platform_data	= &pdata_##_id,		\
}

#define IP6203_PWRKEY					\
{							\
	.id		= -1,				\
	.name		= "ip6203-pwrkey",		\
	.platform_data	= NULL,				\
}

static struct regulator_consumer_supply ip6203_dc2_supply[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};
static struct regulator_consumer_supply ip6203_dc4_supply[] = {
	REGULATOR_SUPPLY("dc4", NULL),
};
static struct regulator_consumer_supply ip6203_dc5_supply[] = {
	REGULATOR_SUPPLY("dc5", NULL),
};
#ifdef IP6203_LDO1 
static struct regulator_consumer_supply ip6203_ldo1_supply[] = {
	REGULATOR_SUPPLY("ldo1", NULL),
};
#endif
static struct regulator_consumer_supply ip6203_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};
static struct regulator_consumer_supply ip6203_ldo7_supply[] = {
	REGULATOR_SUPPLY("ldo7", NULL),
};
static struct regulator_consumer_supply ip6203_ldo8_supply[] = {
	REGULATOR_SUPPLY("ldo8", NULL),
};
static struct regulator_consumer_supply ip6203_ldo9_supply[] = {
	REGULATOR_SUPPLY("ldo9", NULL),
};
static struct regulator_consumer_supply ip6203_ldo11_supply[] = {
	REGULATOR_SUPPLY("ldo11", NULL),
};
static struct regulator_consumer_supply ip6203_ldo12_supply[] = {
	REGULATOR_SUPPLY("ldo12", NULL),
};

IP6203_PDATA_INIT(dc2, 600, 2600, 0);
IP6203_PDATA_INIT(dc4, 600, 2600, 0);
IP6203_PDATA_INIT(dc5, 600, 3400, 0);
#ifdef IP6203_LDO1 
IP6203_PDATA_INIT(ldo1, 1000, 3300, 0);
#endif
IP6203_PDATA_INIT(ldo6, 700, 3300, 0);
IP6203_PDATA_INIT(ldo7, 700, 3300, 0);
IP6203_PDATA_INIT(ldo8, 700, 3300, 0);
IP6203_PDATA_INIT(ldo9, 700, 3300, 0);
IP6203_PDATA_INIT(ldo11, 700, 3300, 0);
IP6203_PDATA_INIT(ldo12, 700, 3400, 0);

static struct ip6203_subdev_info ip6203_subdevs[] = {
	IP6203_REGULATOR(dc2),
	IP6203_REGULATOR(dc4),
	IP6203_REGULATOR(dc5),
#ifdef IP6203_LDO1 
	IP6203_REGULATOR(ldo1),
#endif
	IP6203_REGULATOR(ldo6),
	IP6203_REGULATOR(ldo7),
	IP6203_REGULATOR(ldo8),
	IP6203_REGULATOR(ldo9),
	IP6203_REGULATOR(ldo11),
	IP6203_REGULATOR(ldo12),
	IP6203_PWRKEY,
};

static struct ip6203_platform_data ip6203_platform = {
	.subdevs	= ip6203_subdevs,
	.num_subdevs	= ARRAY_SIZE(ip6203_subdevs),
	.ip		= NULL,
};

/* for regulator end */

typedef enum {
	RESET = 0,
	SET = !RESET
} FlagStatus, ITStatus;

typedef enum {
	StateDISABLE = 0,
	StatetENABLE = !StateDISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef struct
{
  unsigned char YEAR;
  unsigned char MON; 
  unsigned char DATE; 
  unsigned char WEEK; 
  unsigned char HOUR; 
  unsigned char MIN; 
  unsigned char SEC; 
}RTC_StructTypeDef;

typedef struct
{
  unsigned char YEAR;
  unsigned char MON; 
  unsigned char DATE;  
  unsigned char HOUR; 
  unsigned char MIN; 
  unsigned char SEC; 
}ALARM_StructTypeDef;



/******************************************************************************/
/*                            Registers_Address_Definition                    */
/******************************************************************************/

#define IP6203_I2C_ADDR                      (0xA3)

/*!< Control */
#define IP6203_PSTATE_CTL                    (0x00)
#define IP6203_PSTATE_SET                    (0x01)
#define IP6203_PPATH_CTL                     (0x02)
#define IP6203_PPATH_STATUS                  (0x03)
#define IP6203_PROTECT_CTL1                  (0x05)
#define IP6203_PROTECT_CTL2                  (0x06)
#define IP6203_PM_INT_TH                     (0x07)
#define IP6203_PWRINT_FLAG                   (0x08)
#define IP6203_LDO_OCFLAG                    (0x09)
#define IP6203_DCDC_OCFLAG                   (0x0A)
#define IP6203_DCDC_GOOD                     (0x0B)
#define IP6203_LDO_GOOD                      (0x0C)
#define IP6203_PWRON_REC                     (0x0F)
#define IP6203_PWROFF_REC                    (0x10)
#define IP6203_POFF_LDO                      (0x16)
#define IP6203_POFF_DCDC                     (0x17)
#define IP6203_WDOG_CTL                      (0x18)
#define IP6203_LDO_MASK                      (0x19)

/*!< CHG/FUEL */
#define IP6203_CHG_CTL2                      (0x22)
#define IP6203_CHG_CTL3                      (0x23)
#define IP6203_FG_CTL                        (0x28)
#define IP6203_FG_QMAX                       (0x29)
#define IP6203_FG_SOC                        (0x2A)
#define IP6203_FG_DEBUG0                     (0x2B)
#define IP6203_FG_DEBUG1                     (0x2C)
#define IP6203_FG_DEBUG2                     (0x2D)

/*!< LDO */
#define IP6203_LDO_EN                        (0x30)
#define IP6203_LDOSW_EN                      (0x31)
#define IP6203_LDODIS                        (0x32)
#define IP6203_LDO6_VSET                     (0x3A)
#define IP6203_LDO7_VSET                     (0x3B)
#define IP6203_LDO8_VSET                     (0x3C)
#define IP6203_LDO9_VSET                     (0x3D)
#define IP6203_LDO11_VSET                    (0x3E)
#define IP6203_LDO12_VSET                    (0x3F)
#define IP6203_SVCC_CTL                      (0x42)
#define IP6203_SLDO1_CTL                     (0x43)

/*!< DCDC */
#define IP6203_DC_CTL                        (0x50)
#define IP6203_DC2_VSET                      (0x5A)
#define IP6203_DC4_VSET                      (0x64)
#define IP6203_DC5_VSET                      (0x69)

/*1< ADC */
#define IP6203_ADC_CTL1                      (0x80)
#define IP6203_ADC_CTL2                      (0x81)
#define IP6203_VSYS_ADC_DATA                 (0x84)
#define IP6203_VDCIN_ADC_DATA                (0x85)
#define IP6203_IBAT_ADC_DATA                 (0x86)
#define IP6203_IDCIN_ADC_DATA                (0x88)
#define IP6203_TEMP_ADC_DATA                 (0x89)
#define IP6203_GP1_ADC_DATA                  (0x8A)
#define IP6203_GP2_ADC_DATA                  (0x8B)

/*!< INT/MFP */
#define IP6203_INTS_CTL                      (0xA0)
#define IP6203_INT_FLAG                      (0xA1)
#define IP6203_INT_MASK                      (0xA2)
#define IP6203_MFP_CTL1                      (0xA4)
#define IP6203_MFP_CTL2                      (0xA5)
#define IP6203_MFP_CTL3                      (0xA6)
#define IP6203_GPIO_OE                       (0xA7)
#define IP6203_GPIO_IE                       (0xA8)
#define IP6203_GPIO_DAT                      (0xA9)
#define IP6203_PAD_CTL                       (0xAC)
#define IP6203_INT_PENDING1                  (0xAD)
#define IP6203_INT_PENDING2                  (0xAE)
#define IP6203_PAD_DRV                       (0xAF)

/*!< RTC */
#define IP6203_RTC_CTL                       (0xC0)



/******************************************************************************/
/*                              Registers_Bits_Definition                     */
/******************************************************************************/

/****************  Bit definition for IP6203_WAKE_FLAG register  ***************/
#define IP6203_WK_EIRQ                       (0x01)
#define IP6203_WK_ALARM                      (0x02)
#define IP6203_WK_VINOK                      (0x04)
#define IP6203_WK_RESTART                    (0x08)
#define IP6203_WK_ONOFF_S                    (0x10)
#define IP6203_WK_ONOFF_L                    (0x20)
#define IP6203_WK_ONOFF_SL                   (0x40)
#define IP6203_ONOFF_SL_TIME                 (0x80)

/***************  Bit definition for IP6203_WAKE_CTL register  *****************/
#define IP6203_WK_EN_EIRQ                    (0x01)
#define IP6203_WK_EN_ALARM                   (0x02)
#define IP6203_WK_EN_RESTART                 (0x08)
#define IP6203_WK_EN_ONOFF_S                 (1<<4)
#define IP6203_WK_EN_ONOFF_L                 (1<<5)
#define IP6203_WK_EN_ONOFF_SL                (0x40)
#define IP6203_ONOFF_SL_RST                  (1<<9)

/***************  Bit definition for IP6203_STATUS_CTL register  *****************/
#define IP6203_SLEEP_EN_n                    (0x01)
#define IP6203_SLEEP_TIME                    (0x1C)
#define IP6203_SLEEP_TIME_EN                 (0x20)
#define IP6203_ONOFF_TIME                    (0xC0)

#define IP6203_SLEEP_TIME_0                  (0x04)
#define IP6203_SLEEP_TIME_1                  (0x08)
#define IP6203_SLEEP_TIME_2                  (0x10) 

#define IP6203_ONOFF_TIME_0                  (0x40)
#define IP6203_ONOFF_TIME_1                  (0x80)

typedef enum
{ 
  IP6203_SLEEP_TIME_H25   = 0x00,
  IP6203_SLEEP_TIME_H5    = 0x04,
  IP6203_SLEEP_TIME_1H    = 0x08,
  IP6203_SLEEP_TIME_2H    = 0x0C, 
  IP6203_SLEEP_TIME_3H    = 0x10,
  IP6203_SLEEP_TIME_4H    = 0x14,
  IP6203_SLEEP_TIME_5H    = 0x18,
  IP6203_SLEEP_TIME_6H    = 0x1C  
}IP6203_SLEEP_TIME_TypeDef;
#define IS_IP6203_SLEEP_TIME(TIME) (((TIME) == IP6203_SLEEP_TIME_H25) || ((TIME) == IP6203_SLEEP_TIME_H5) || \
                                   ((TIME) == IP6203_SLEEP_TIME_1H) || ((MODE) == IP6203_SLEEP_TIME_2H) || \
																	 ((TIME) == IP6203_SLEEP_TIME_3H) || ((TIME) == IP6203_SLEEP_TIME_4H) || \
                                   ((TIME) == IP6203_SLEEP_TIME_5H) || ((MODE) == IP6203_SLEEP_TIME_6H))

typedef enum
{ 
  IP6203_ONOFF_TIME_S5    = 0x00,
  IP6203_ONOFF_TIME_1S    = 0x40,
  IP6203_ONOFF_TIME_2S    = 0x80,
  IP6203_ONOFF_TIME_4S    = 0xC0 
}IP6203_ONOFF_TIME_TypeDef;
#define IS_IP6203_ONOFF_TIME(TIME) (((TIME) == IP6203_ONOFF_TIME_S5) || ((TIME) == IP6203_ONOFF_TIME_1S) || \
                                   ((TIME) == IP6203_ONOFF_TIME_2S) || ((MODE) == IP6203_ONOFF_TIME_4S))

/***************  Bit definition for IP6203_POWER_FLAG0 register  **************/
#define IP6203_DC0_PWR_OK                    (0x01)
#define IP6203_DC1_PWR_OK                    (0x02)
#define IP6203_DC2_PWR_OK                    (0x04)
#define IP6203_DC3_PWR_OK                    (0x08)

/***************  Bit definition for IP6203_POWER_FLAG1 register  **************/
#define IP6203_LDO0_PWR_OK                   (0x01)
#define IP6203_LDO1_PWR_OK                   (0x02)
#define IP6203_LDO2_PWR_OK                   (0x04)
#define IP6203_LDO4_PWR_OK                   (0x10)
#define IP6203_LDO5_PWR_OK                   (0x20)
#define IP6203_LDO6_PWR_OK                   (0x40)

/***************  Bit definition for IP6203_SL_PWR_CTL0 register  **************/
#define IP6203_SL_DC2_PWR                    (0x02)
#define IP6203_SL_DC4_PWR                    (0x10)
#define IP6203_SL_DC5_PWR                    (0x20)

#define IP6203_SL_OFF_SCAL_0                 (0x20)
#define IP6203_SL_OFF_SCAL_1                 (0x40)

typedef enum
{ 
  IP6203_SL_OFF_SCAL_1MS   = 0x00,
  IP6203_SL_OFF_SCAL_2MS   = 0x20,
  IP6203_SL_OFF_SCAL_4MS   = 0x40,
  IP6203_SL_OFF_SCAL_8MS   = 0x60 
}IP6203_SL_OFF_SCAL_TypeDef;
#define IS_IP6203_SL_OFF_SCAL(TIME) (((TIME) == IP6203_SL_OFF_SCAL_1MS) || ((TIME) == IP6203_SL_OFF_SCAL_2MS) || \
                                    ((TIME) == IP6203_SL_OFF_SCAL_4MS) || ((MODE) == IP6203_SL_OFF_SCAL_8MS))

/***************  Bit definition for IP6203_SL_PWR_CTL1 register  **************/
#define IP6203_SL_LDO0_PWR                   (0x01)
#define IP6203_SL_LDO1_PWR                   (0x02)
#define IP6203_SL_LDO2_PWR                   (0x04)
#define IP6203_SL_LDO4_PWR                   (0x10)
#define IP6203_SL_LDO5_PWR                   (0x20)
#define IP6203_SL_LDO6_PWR                   (0x40)
#define IP6203_DEEP_SL_CTL                   (1<<0)
#define IP6203_DC_EN		                 (1<<3)

/****************  Bit definition for IP6203_DCDC_CTL register  ****************/
#define IP6203_DC0_EN                        (0x01)
#define IP6203_DC1_EN                        (0x02)
#define IP6203_DC2_EN                        (0x04)
#define IP6203_DC3_EN                        (0x08)
#define IP6203_DC_FRQ                        (0x30)
#define IP6203_DC_SSEN                       (0x80)

#define IP6203_DC_FRQ_0                      (0x10)
#define IP6203_DC_FRQ_1                      (0x20)

typedef enum
{ 
  IP6203_DC_FRQ_2M    = 0x00,
  IP6203_DC_FRQ_2M4   = 0x10,
  IP6203_DC_FRQ_2M7   = 0x20,
  IP6203_DC_FRQ_3M1   = 0x30 
}IP6203_DC_FRQ_TypeDef;
#define IS_IP6203_DC_FRQ(FRQ) (((FRQ) == IP6203_DC_FRQ_2M) || ((FRQ) == IP6203_DC_FRQ_2M4) || \
                              ((FRQ) == IP6203_DC_FRQ_2M7) || ((FRQ) == IP6203_DC_FRQ_3M1))

/*****************  Bit definition for IP6203_SW_EN register  ******************/
#define IP6203_SW0_EN                        (0x01)
#define IP6203_SW1_EN                        (0x02)
#define IP6203_SW2_EN                        (0x04)
#define IP6203_SW4_EN                        (0x10)
#define IP6203_SW5_EN                        (0x20)
#define IP6203_SW6_EN                        (0x40)

/*****************  Bit definition for IP6203_LDO_EN register  *****************/
#define IP6203_LDO0_EN                       (0x01)
#define IP6203_LDO1_EN                       (0x02)
#define IP6203_LDO2_EN                       (0x04)
#define IP6203_LDO4_EN                       (0x10)
#define IP6203_LDO5_EN                       (0x20)
#define IP6203_LDO6_EN                       (0x40)

/****************  Bit definition for IP6203_INTS_CTL0 register  ***************/
/****************  Bit definition for IP6203_INTS_FLAG0 register  **************/
#define IP6203_ONOFF_PRESS_INT               (0x01)
#define IP6203_ONOFF_SHORT_INT               (0x02)
#define IP6203_ONOFF_LONG_INT                (0x04)
#define IP6203_ALARM_INT                     (0x08)
#define IP6203_IRQS_INT                      (0x10)

/****************  Bit definition for IP6203_INTS_CTL1 register  ***************/
/****************  Bit definition for IP6203_INTS_FLAG1 register  **************/
#define IP6203_LDO0_OC_INT                   (0x01)
#define IP6203_LDO1_OC_INT                   (0x02)
#define IP6203_LDO2_OC_INT                   (0x04)
#define IP6203_LDO4_OC_INT                   (0x10)
#define IP6203_LDO5_OC_INT                   (0x20)
#define IP6203_LDO6_OC_INT                   (0x40)

/*****************  Bit definition for IP6203_RTC_CTL register  ****************/
#define IP6203_RTC_RST                       (0x01)
#define IP6203_RTC_EN                        (0x02)
#define IP6203_RTC_CKSS                      (0x04)
#define IP6203_EOSC_EN                       (0x08)
#define IP6203_EOSC_LGS                      (0x30)
#define IP6203_EOSC_STATUS                   (0x80)

#define IP6203_EOSC_LGS_0                    (0x10)
#define IP6203_EOSC_LGS_1                    (0x20)

/*****************  Bit definition for IP6203_MFP_CTL0 register  ***************/
#define IP6203_GPIO5_DATA                    (0x01)
#define IP6203_LDO5_MFP                      (0x0E)
#define IP6203_GPIO6_DATA                    (0x10)
#define IP6203_LDO6_MFP                      (0xE0)

#define IP6203_LDO5_MFP_0                    (0x02)
#define IP6203_LDO5_MFP_1                    (0x04)
#define IP6203_LDO5_MFP_2                    (0x08)
#define IP6203_LDO6_MFP_0                    (0x20)
#define IP6203_LDO6_MFP_1                    (0x40)
#define IP6203_LDO6_MFP_2                    (0x80)

typedef enum
{ 
  IP6203_LDO5_Z     = 0x00,
  IP6203_LDO5_GPI   = 0x04,
  IP6203_LDO5_GPO   = 0x06,
  IP6203_LDO5_ODO   = 0x08, 
  IP6203_LDO5_32KO  = 0x0A,
  IP6203_LDO5_IRQS  = 0x0C,
  IP6203_LDO5_LDO   = 0x0E 
}IP6203_LDO5_MFP_TypeDef;
#define IS_IP6203_LDO5_MFP(MODE) (((MODE) == IP6203_LDO5_Z) || ((MODE) == IP6203_LDO5_GPI) || \
                                 ((MODE) == IP6203_LDO5_GPO) || ((MODE) == IP6203_LDO5_ODO) || \
                                 ((MODE) == IP6203_LDO5_32KO) || ((MODE) == IP6203_LDO5_IRQS) || \
                                 ((MODE) == IP6203_LDO5_LDO))

typedef enum
{ 
  IP6203_LDO6_Z     = 0x00,
  IP6203_LDO6_GPI   = 0x40,
  IP6203_LDO6_GPO   = 0x60,
  IP6203_LDO6_ODO   = 0x80, 
  IP6203_LDO6_32KO  = 0xA0,
  IP6203_LDO6_IRQS  = 0xC0,
  IP6203_LDO6_LDO   = 0xE0 
}IP6203_LDO6_MFP_TypeDef;
#define IS_IP6203_LDO6_MFP(MODE) (((MODE) == IP6203_LDO6_Z) || ((MODE) == IP6203_LDO6_GPI) || \
                                 ((MODE) == IP6203_LDO6_GPO) || ((MODE) == IP6203_LDO6_ODO) || \
                                 ((MODE) == IP6203_LDO6_32KO) || ((MODE) == IP6203_LDO6_IRQS) || \
                                 ((MODE) == IP6203_LDO6_LDO))

/*****************  Bit definition for IP6203_MFP_CTL1 register  ***************/
#define IP6203_GPIO4_DATA                    (0x10)
#define IP6203_LDO4_MFP                      (0xE0)

#define IP6203_LDO4_MFP_0                    (0x20)
#define IP6203_LDO4_MFP_1                    (0x40)
#define IP6203_LDO4_MFP_2                    (0x80)

typedef enum
{ 
  IP6203_LDO4_Z     = 0x00,
  IP6203_LDO4_GPI   = 0x40,
  IP6203_LDO4_GPO   = 0x60,
  IP6203_LDO4_ODO   = 0x80, 
  IP6203_LDO4_32KO  = 0xA0,
  IP6203_LDO4_IRQS  = 0xC0,
  IP6203_LDO4_LDO   = 0xE0 
}IP6203_LDO4_MFP_TypeDef;
#define IS_IP6203_LDO4_MFP(MODE) (((MODE) == IP6203_LDO4_Z) || ((MODE) == IP6203_LDO4_GPI) || \
                                 ((MODE) == IP6203_LDO4_GPO) || ((MODE) == IP6203_LDO4_ODO) || \
                                 ((MODE) == IP6203_LDO4_32KO) || ((MODE) == IP6203_LDO4_IRQS) || \
                                 ((MODE) == IP6203_LDO4_LDO))

/*****************  Bit definition for IP6203_MFP_CTL2 register  ***************/
#define IP6203_GPIO2_DATA                    (0x01)
#define IP6203_GPIO2_IEN                     (0x02)
#define IP6203_GPIO2_OEN                     (0x04)
#define IP6203_EXTP_EN_MFP                   (0x08)

/*****************  Bit definition for IP6203_MFP_CTL3 register  ***************/
#define IP6203_GPIO0_DATA                    (0x01)
#define IP6203_GPIO0_IEN                     (0x02)
#define IP6203_GPIO0_OEN                     (0x04)
#define IP6203_GPIO1_DATA                    (0x08)
#define IP6203_GPIO1_IEN                     (0x10)
#define IP6203_GPIO1_OEN                     (0x20)
#define IP6203_LOSC_MFP                      (0x40)

/*****************  Bit definition for IP6203_MFP_CTL4 register  ***************/
#define IP6203_GPIO0_PUPD                    (0x03)
#define IP6203_GPIO1_PUPD                    (0x0C)
#define IP6203_GPIO2_PUPD                    (0x30)

#define IP6203_GPIO0_PUPD_0                  (0x01)
#define IP6203_GPIO0_PUPD_1                  (0x02)
#define IP6203_GPIO1_PUPD_0                  (0x04)
#define IP6203_GPIO1_PUPD_1                  (0x08)
#define IP6203_GPIO2_PUPD_0                  (0x10)
#define IP6203_GPIO2_PUPD_1                  (0x20)

typedef enum
{ 
  IP6203_GPIO0_PU    = 0x01,
  IP6203_GPIO0_PD    = 0x02,
  IP6203_GPIO0_Z     = 0x00
}IP6203_GPIO0_MODE_TypeDef;
#define IS_IP6203_GPIO0_MODE(MODE) (((MODE) == IP6203_GPIO0_PU) || ((MODE) == IP6203_GPIO0_PD) || ((MODE) == IP6203_GPIO0_Z))

typedef enum
{ 
  IP6203_GPIO1_PU    = 0x04,
  IP6203_GPIO1_PD    = 0x08,
  IP6203_GPIO1_Z     = 0x00
}IP6203_GPIO1_MODE_TypeDef;
#define IS_IP6203_GPIO1_MODE(MODE) (((MODE) == IP6203_GPIO1_PU) || ((MODE) == IP6203_GPIO1_PD) || ((MODE) == IP6203_GPIO1_Z))

typedef enum
{ 
  IP6203_GPIO2_PU    = 0x10,
  IP6203_GPIO2_PD    = 0x20,
  IP6203_GPIO2_Z     = 0x00
}IP6203_GPIO2_MODE_TypeDef;
#define IS_IP6203_GPIO2_MODE(MODE) (((MODE) == IP6203_GPIO2_PU) || ((MODE) == IP6203_GPIO2_PD) || ((MODE) == IP6203_GPIO2_Z))

/*****************  Bit definition for IP6203_MFP_CTL5 register  ***************/
#define IP6203_IRQ_CTL                       (0x03)
#define IP6203_IRQS_TYPE                     (0x0C)
#define IP6203_IRQ_TYPE_H                    (0x10)

#define IP6203_IRQ_CTL_0                     (0x01)
#define IP6203_IRQ_CTL_1                     (0x02)
#define IP6203_IRQS_TYPE_0                   (0x04)
#define IP6203_IRQS_TYPE_1                   (0x08)

typedef enum
{ 
  IP6203_IRQ_IN         = 0x02,
  IP6203_IRQ_OUT        = 0x01,
  IP6203_IRQ_Z          = 0x00
}IP6203_IRQ_MODE_TypeDef;
#define IS_IP6203_IRQ_MODE(MODE) (((MODE) == IP6203_IRQ_IN) || ((MODE) == IP6203_IRQ_OUT) || ((MODE) == IP6203_IRQ_Z))

typedef enum
{ 
  IP6203_IRQS_TYPE_H       = 0x00,
  IP6203_IRQS_TYPE_L       = 0x04,
  IP6203_IRQS_TYPE_RE      = 0x08,
  IP6203_IRQS_TYPE_LE      = 0x0C
}IP6203_IRQS_TYPE_TypeDef;
#define IS_IP6203_IRQS_TYPE(TYPE) (((TYPE) == IP6203_IRQS_TYPE_H) || ((TYPE) == IP6203_IRQS_TYPE_L) ||\
																 ((TYPE) == IP6203_IRQS_TYPE_RE) || ((MODE) == IP6203_IRQS_TYPE_L))

/*****************  Bit definition for IP6203_MFP_CTL6 register  ***************/
#define IP6203_MFP_CTL3_PWR_UP_RST           (0x01)
#define IP6203_MFP_CTL2_PWR_UP_RST           (0x02)
#define IP6203_MFP_CTL1_L_PWR_UP_RST         (0x04)
#define IP6203_MFP_CTL1_H_PWR_UP_RST         (0x08)
#define IP6203_MFP_CTL0_L_PWR_UP_RST         (0x10)
#define IP6203_MFP_CTL0_H_PWR_UP_RST         (0x20)

/*****************  Bit definition for IP6203_I2C_CTL0 register  ***************/
#define IP6203_I2C_LVSFT                     (0x01)


extern int mic_battery_adapter_in(void);

extern void ip6203_sleep(void);
extern void ip6203_deep_sleep(void);
extern void ip6203_reset(void);
extern int ip6203_read(struct device *dev, uint8_t reg, uint16_t *val);
extern int ip6203_write(struct device *dev, u8 reg, uint16_t val);
int ip6203_set_bits(struct device *dev, u8 reg, uint16_t bit_mask);
int ip6203_clr_bits(struct device *dev, u8 reg, uint16_t bit_mask);
#endif
