#ifndef __EC610_REGISTER__
#define __EC610_REGISTER__

#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/regulator/machine.h>

#define EC610_DEBUG_IF
#define EC610_ERR_FLAG		1
#define EC610_INFO_FLAG		1
#define EC610_DEBUG_FLAG	0

#if EC610_ERR_FLAG
#define EC610_ERR(format, args...)  printk(KERN_ERR "[EC610]"format, ##args)
#else
#define EC610_ERR(format, args...)  do {} while (0)
#endif

#if EC610_INFO_FLAG
#define EC610_INFO(format, args...)  printk(KERN_INFO "[EC610]"format, ##args)
#else
#define EC610_INFO(format, args...)  do {} while (0)
#endif

#if EC610_DEBUG_FLAG
#define EC610_DBG(format, args...)  printk(KERN_DEBUG "[EC610]"format, ##args)
#else
#define EC610_DBG(format, args...)  do {} while (0)
#endif

#define EC610_PMIC_IRQ              (214)

#define DEFAULT_INT_MASK			(EC610_ONOFF_PRESS_INT | EC610_ONOFF_SHORT_INT | EC610_ONOFF_LONG_INT)
#define INT_MASK_ALL					(EC610_ONOFF_PRESS_INT | EC610_ONOFF_SHORT_INT | EC610_ONOFF_LONG_INT | EC610_ALARM_INT | EC610_IRQS_INT)

#define MAX_INTERRUPT_MASKS	11
#define MAX_MAIN_INTERRUPT	8
#define MAX_GPEDGE_REG		1

struct ec610 {
	struct device		*dev;
	struct i2c_client	*client;
	struct mutex		io_lock;
	struct input_dev *ec610_pwr;
	int		irq;
};

/* for regulator begin */

enum ec610_regulator_id {
	EC610_ID_dc0 = 0,
	EC610_ID_dc1,
	EC610_ID_dc2,
	EC610_ID_dc3,
	EC610_ID_ldo0,
	EC610_ID_ldo1,
	EC610_ID_ldo2,
	EC610_ID_ldo4,
	EC610_ID_ldo5,
	EC610_ID_ldo6,
};

enum ec610_dc_nsteps {
	EC610_DC_NULL,
	EC610_DC_870UV = 0x40,
	EC610_DC_210UV = 0x80,
	EC610_DC_150UV = 0xC0,
};

struct ec610_subdev_info {
	int		id;
	const char	*name;
	void		*platform_data;
};

struct ec610_platform_data {
	struct ec610_subdev_info *subdevs;
	int	num_subdevs;
	void	*ip;
};

struct ec610_regulator_platform_data {
		struct regulator_init_data regulator;
};

#define EC610_PDATA_INIT(_name, _minmv, _maxmv, _supply_reg) \
	static struct ec610_regulator_platform_data pdata_##_name\
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
				ARRAY_SIZE(ec610_##_name##_supply),\
			.consumer_supplies =				\
				ec610_##_name##_supply,	\
			.supply_regulator = _supply_reg,		\
		},							\
	}

#define EC610_REGULATOR(_id)				\
{								\
	.id		= EC610_ID_##_id,			\
	.name		= "ec610-regulator",			\
	.platform_data	= &pdata_##_id,		\
}

#define EC610_PWRKEY					\
{							\
	.id		= -1,				\
	.name		= "ec610-pwrkey",		\
	.platform_data	= NULL,				\
}

static struct regulator_consumer_supply ec610_dc0_supply[] = {
	REGULATOR_SUPPLY("dc0", NULL),
};
static struct regulator_consumer_supply ec610_dc1_supply[] = {
	REGULATOR_SUPPLY("dc1", NULL),
};
static struct regulator_consumer_supply ec610_dc2_supply[] = {
	REGULATOR_SUPPLY("dc2", NULL),
};
static struct regulator_consumer_supply ec610_dc3_supply[] = {
	REGULATOR_SUPPLY("dc3", NULL),
};
static struct regulator_consumer_supply ec610_ldo0_supply[] = {
	REGULATOR_SUPPLY("ldo0", NULL),
};
static struct regulator_consumer_supply ec610_ldo1_supply[] = {
	REGULATOR_SUPPLY("ldo1", NULL),
};
static struct regulator_consumer_supply ec610_ldo2_supply[] = {
	REGULATOR_SUPPLY("ldo2", NULL),
};
static struct regulator_consumer_supply ec610_ldo4_supply[] = {
	REGULATOR_SUPPLY("ldo4", NULL),
};
static struct regulator_consumer_supply ec610_ldo5_supply[] = {
	REGULATOR_SUPPLY("ldo5", NULL),
};
static struct regulator_consumer_supply ec610_ldo6_supply[] = {
	REGULATOR_SUPPLY("ldo6", NULL),
};

EC610_PDATA_INIT(dc0, 600, 1375, 0);
EC610_PDATA_INIT(dc1, 600, 1375, 0);
EC610_PDATA_INIT(dc2, 600, 2175, 0);
EC610_PDATA_INIT(dc3, 2200, 3500, 0);
EC610_PDATA_INIT(ldo0, 700, 3400, 0);
EC610_PDATA_INIT(ldo1, 700, 3400, 0);
EC610_PDATA_INIT(ldo2, 700, 3400, 0);
EC610_PDATA_INIT(ldo4, 700, 3400, 0);
EC610_PDATA_INIT(ldo5, 700, 3400, 0);
EC610_PDATA_INIT(ldo6, 700, 3400, 0);

static struct ec610_subdev_info ec610_subdevs[] = {
	EC610_REGULATOR(dc0),
	EC610_REGULATOR(dc1),
	EC610_REGULATOR(dc2),
	EC610_REGULATOR(dc3),
	EC610_REGULATOR(ldo0),
	EC610_REGULATOR(ldo1),
	EC610_REGULATOR(ldo2),
	EC610_REGULATOR(ldo4),
	EC610_REGULATOR(ldo5),
	EC610_REGULATOR(ldo6),
	EC610_PWRKEY,
};

static struct ec610_platform_data ec610_platform = {
	.subdevs	= ec610_subdevs,
	.num_subdevs	= ARRAY_SIZE(ec610_subdevs),
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

#define EC610_I2C_ADDR                      (0x30)

/*!< Control */
#define EC610_WAKE_FLAG                     (0x00)
#define EC610_WAKE_CTL                      (0x01)
#define EC610_STATUS_CTL                    (0x02)
#define EC610_PROTECT_CTL0                  (0x03)
#define EC610_PROTECT_CTL1                  (0x04)
#define EC610_POWER_FLAG0                   (0x05)
#define EC610_POWER_FLAG1                   (0x06)
#define EC610_SL_PWR_CTL0                   (0x07)
#define EC610_SL_PWR_CTL1                   (0x08)

/*!< DCDC */
#define EC610_DCDC_CTL                      (0x20)
#define EC610_DC0_VSEL                      (0x21)
#define EC610_DC0_CTL1                      (0x22)
#define EC610_DC0_CTL2                      (0x23)
#define EC610_DC0_CTL3                      (0x24)
#define EC610_DC0_CTL4                      (0x25)
#define EC610_DC0_CTL5                      (0x26)
#define EC610_DC0_CTL6                      (0x27)
#define EC610_DC1_VSEL                      (0x28)
#define EC610_DC1_CTL1                      (0x29)
#define EC610_DC1_CTL2                      (0x2A)
#define EC610_DC1_CTL3                      (0x2B)
#define EC610_DC1_CTL4                      (0x2C)
#define EC610_DC1_CTL5                      (0x2D)
#define EC610_DC1_CTL6                      (0x2E)
#define EC610_DC2_VSEL                      (0x2F)
#define EC610_DC2_CTL1                      (0x30)
#define EC610_DC2_CTL2                      (0x31)
#define EC610_DC2_CTL3                      (0x32)
#define EC610_DC2_CTL4                      (0x33)
#define EC610_DC2_CTL5                      (0x34)
#define EC610_DC2_CTL6                      (0x35)
#define EC610_DC3_VSEL                      (0x36)
#define EC610_DC3_CTL1                      (0x37)
#define EC610_DC3_CTL2                      (0x38)
#define EC610_DC3_CTL3                      (0x39)
#define EC610_DC3_CTL4                      (0x3A)
#define EC610_DC3_CTL5                      (0x3B)
#define EC610_DC3_CTL6                      (0x3C)

/*!< LDO */
#define EC610_SW_EN                         (0x40)
#define EC610_LDO_EN                        (0x41)
#define EC610_LDO0_VSEL                     (0x42)
#define EC610_LDO0_CTL1                     (0x43)
#define EC610_LDO1_VSEL                     (0x44)
#define EC610_LDO1_CTL1                     (0x45)
#define EC610_LDO2_VSEL                     (0x46)
#define EC610_LDO2_CTL1                     (0x47)
#define EC610_LDO4_VSEL                     (0x4A)
#define EC610_LDO4_CTL1                     (0x4B)
#define EC610_LDO5_VSEL                     (0x4C)
#define EC610_LDO5_CTL1                     (0x4D)
#define EC610_LDO6_VSEL                     (0x4E)
#define EC610_LDO6_CTL1                     (0x4F)
#define EC610_SVCC_CTL                      (0x51)

/*!< INT */
#define EC610_INTS_CTL0                     (0x60)
#define EC610_INTS_CTL1                     (0x61)
#define EC610_INTS_FLAG0                    (0x62)
#define EC610_INTS_FLAG1                    (0x63)

/*!< RTC */
#define EC610_RTC_CTL                       (0x70)
#define EC610_RTC_SEC_ALM                   (0x71)
#define EC610_RTC_MIN_ALM                   (0x72)
#define EC610_RTC_HOUR_ALM                  (0x73)
#define EC610_RTC_DATE_ALM                  (0x74)
#define EC610_RTC_MON_ALM                   (0x75)
#define EC610_RTC_YEAR_ALM                  (0x76)
#define EC610_RTC_SEC                       (0x77)
#define EC610_RTC_MIN                       (0x78)
#define EC610_RTC_HOUR                      (0x79)
#define EC610_RTC_DATE                      (0x7A)
#define EC610_RTC_MON                       (0x7B)
#define EC610_RTC_YEAR                      (0x7C)

/*!< MFP */
#define EC610_MFP_CTL0                      (0x80)
#define EC610_MFP_CTL1                      (0x81)
#define EC610_MFP_CTL2                      (0x82)
#define EC610_MFP_CTL3                      (0x83)
#define EC610_MFP_CTL4                      (0x84)
#define EC610_MFP_CTL5                      (0x85)
#define EC610_MFP_CTL6                      (0x86)

/*!< I2C */
#define EC610_I2C_CTL0                      (0xFD)
#define EC610_I2C_CTL1                      (0xFE)


/******************************************************************************/
/*                              Registers_Bits_Definition                     */
/******************************************************************************/

/****************  Bit definition for EC610_WAKE_FLAG register  ***************/
#define EC610_WK_EIRQ                       (0x01)
#define EC610_WK_ALARM                      (0x02)
#define EC610_WK_VINOK                      (0x04)
#define EC610_WK_RESTART                    (0x08)
#define EC610_WK_ONOFF_S                    (0x10)
#define EC610_WK_ONOFF_L                    (0x20)
#define EC610_WK_ONOFF_SL                   (0x40)
#define EC610_ONOFF_SL_TIME                 (0x80)

/***************  Bit definition for EC610_WAKE_CTL register  *****************/
#define EC610_WK_EN_EIRQ                    (0x01)
#define EC610_WK_EN_ALARM                   (0x02)
#define EC610_WK_EN_VINOK                   (0x04)
#define EC610_WK_EN_RESTART                 (0x08)
#define EC610_WK_EN_ONOFF_S                 (0x10)
#define EC610_WK_EN_ONOFF_L                 (0x20)
#define EC610_WK_EN_ONOFF_SL                (0x40)
#define EC610_ONOFF_SL_RST                  (0x80)

/***************  Bit definition for EC610_STATUS_CTL register  *****************/
#define EC610_SLEEP_EN_n                    (0x01)
#define EC610_SLEEP_TIME                    (0x1C)
#define EC610_SLEEP_TIME_EN                 (0x20)
#define EC610_ONOFF_TIME                    (0xC0)

#define EC610_SLEEP_TIME_0                  (0x04)
#define EC610_SLEEP_TIME_1                  (0x08)
#define EC610_SLEEP_TIME_2                  (0x10) 

#define EC610_ONOFF_TIME_0                  (0x40)
#define EC610_ONOFF_TIME_1                  (0x80)

typedef enum
{ 
  EC610_SLEEP_TIME_H25   = 0x00,
  EC610_SLEEP_TIME_H5    = 0x04,
  EC610_SLEEP_TIME_1H    = 0x08,
  EC610_SLEEP_TIME_2H    = 0x0C, 
  EC610_SLEEP_TIME_3H    = 0x10,
  EC610_SLEEP_TIME_4H    = 0x14,
  EC610_SLEEP_TIME_5H    = 0x18,
  EC610_SLEEP_TIME_6H    = 0x1C  
}EC610_SLEEP_TIME_TypeDef;
#define IS_EC610_SLEEP_TIME(TIME) (((TIME) == EC610_SLEEP_TIME_H25) || ((TIME) == EC610_SLEEP_TIME_H5) || \
                                   ((TIME) == EC610_SLEEP_TIME_1H) || ((MODE) == EC610_SLEEP_TIME_2H) || \
																	 ((TIME) == EC610_SLEEP_TIME_3H) || ((TIME) == EC610_SLEEP_TIME_4H) || \
                                   ((TIME) == EC610_SLEEP_TIME_5H) || ((MODE) == EC610_SLEEP_TIME_6H))

typedef enum
{ 
  EC610_ONOFF_TIME_S5    = 0x00,
  EC610_ONOFF_TIME_1S    = 0x40,
  EC610_ONOFF_TIME_2S    = 0x80,
  EC610_ONOFF_TIME_4S    = 0xC0 
}EC610_ONOFF_TIME_TypeDef;
#define IS_EC610_ONOFF_TIME(TIME) (((TIME) == EC610_ONOFF_TIME_S5) || ((TIME) == EC610_ONOFF_TIME_1S) || \
                                   ((TIME) == EC610_ONOFF_TIME_2S) || ((MODE) == EC610_ONOFF_TIME_4S))

/***************  Bit definition for EC610_POWER_FLAG0 register  **************/
#define EC610_DC0_PWR_OK                    (0x01)
#define EC610_DC1_PWR_OK                    (0x02)
#define EC610_DC2_PWR_OK                    (0x04)
#define EC610_DC3_PWR_OK                    (0x08)

/***************  Bit definition for EC610_POWER_FLAG1 register  **************/
#define EC610_LDO0_PWR_OK                   (0x01)
#define EC610_LDO1_PWR_OK                   (0x02)
#define EC610_LDO2_PWR_OK                   (0x04)
#define EC610_LDO4_PWR_OK                   (0x10)
#define EC610_LDO5_PWR_OK                   (0x20)
#define EC610_LDO6_PWR_OK                   (0x40)

/***************  Bit definition for EC610_SL_PWR_CTL0 register  **************/
#define EC610_SL_DC0_PWR                    (0x01)
#define EC610_SL_DC1_PWR                    (0x02)
#define EC610_SL_DC2_PWR                    (0x04)
#define EC610_SL_DC3_PWR                    (0x08)
#define EC610_SL_OFF_SCH                    (0x10)
#define EC610_SL_OFF_SCAL                   (0x60)

#define EC610_SL_OFF_SCAL_0                 (0x20)
#define EC610_SL_OFF_SCAL_1                 (0x40)

typedef enum
{ 
  EC610_SL_OFF_SCAL_1MS   = 0x00,
  EC610_SL_OFF_SCAL_2MS   = 0x20,
  EC610_SL_OFF_SCAL_4MS   = 0x40,
  EC610_SL_OFF_SCAL_8MS   = 0x60 
}EC610_SL_OFF_SCAL_TypeDef;
#define IS_EC610_SL_OFF_SCAL(TIME) (((TIME) == EC610_SL_OFF_SCAL_1MS) || ((TIME) == EC610_SL_OFF_SCAL_2MS) || \
                                    ((TIME) == EC610_SL_OFF_SCAL_4MS) || ((MODE) == EC610_SL_OFF_SCAL_8MS))

/***************  Bit definition for EC610_SL_PWR_CTL1 register  **************/
#define EC610_SL_LDO0_PWR                   (0x01)
#define EC610_SL_LDO1_PWR                   (0x02)
#define EC610_SL_LDO2_PWR                   (0x04)
#define EC610_SL_LDO4_PWR                   (0x10)
#define EC610_SL_LDO5_PWR                   (0x20)
#define EC610_SL_LDO6_PWR                   (0x40)
#define EC610_DEEP_SL_CTL                   (0x80)

/****************  Bit definition for EC610_DCDC_CTL register  ****************/
#define EC610_DC0_EN                        (0x01)
#define EC610_DC1_EN                        (0x02)
#define EC610_DC2_EN                        (0x04)
#define EC610_DC3_EN                        (0x08)
#define EC610_DC_FRQ                        (0x30)
#define EC610_DC_SSEN                       (0x80)

#define EC610_DC_FRQ_0                      (0x10)
#define EC610_DC_FRQ_1                      (0x20)

typedef enum
{ 
  EC610_DC_FRQ_2M    = 0x00,
  EC610_DC_FRQ_2M4   = 0x10,
  EC610_DC_FRQ_2M7   = 0x20,
  EC610_DC_FRQ_3M1   = 0x30 
}EC610_DC_FRQ_TypeDef;
#define IS_EC610_DC_FRQ(FRQ) (((FRQ) == EC610_DC_FRQ_2M) || ((FRQ) == EC610_DC_FRQ_2M4) || \
                              ((FRQ) == EC610_DC_FRQ_2M7) || ((FRQ) == EC610_DC_FRQ_3M1))

/*****************  Bit definition for EC610_SW_EN register  ******************/
#define EC610_SW0_EN                        (0x01)
#define EC610_SW1_EN                        (0x02)
#define EC610_SW2_EN                        (0x04)
#define EC610_SW4_EN                        (0x10)
#define EC610_SW5_EN                        (0x20)
#define EC610_SW6_EN                        (0x40)

/*****************  Bit definition for EC610_LDO_EN register  *****************/
#define EC610_LDO0_EN                       (0x01)
#define EC610_LDO1_EN                       (0x02)
#define EC610_LDO2_EN                       (0x04)
#define EC610_LDO4_EN                       (0x10)
#define EC610_LDO5_EN                       (0x20)
#define EC610_LDO6_EN                       (0x40)

/****************  Bit definition for EC610_INTS_CTL0 register  ***************/
/****************  Bit definition for EC610_INTS_FLAG0 register  **************/
#define EC610_ONOFF_PRESS_INT               (0x01)
#define EC610_ONOFF_SHORT_INT               (0x02)
#define EC610_ONOFF_LONG_INT                (0x04)
#define EC610_ALARM_INT                     (0x08)
#define EC610_IRQS_INT                      (0x10)

/****************  Bit definition for EC610_INTS_CTL1 register  ***************/
/****************  Bit definition for EC610_INTS_FLAG1 register  **************/
#define EC610_LDO0_OC_INT                   (0x01)
#define EC610_LDO1_OC_INT                   (0x02)
#define EC610_LDO2_OC_INT                   (0x04)
#define EC610_LDO4_OC_INT                   (0x10)
#define EC610_LDO5_OC_INT                   (0x20)
#define EC610_LDO6_OC_INT                   (0x40)

/*****************  Bit definition for EC610_RTC_CTL register  ****************/
#define EC610_RTC_RST                       (0x01)
#define EC610_RTC_EN                        (0x02)
#define EC610_RTC_CKSS                      (0x04)
#define EC610_EOSC_EN                       (0x08)
#define EC610_EOSC_LGS                      (0x30)
#define EC610_EOSC_STATUS                   (0x80)

#define EC610_EOSC_LGS_0                    (0x10)
#define EC610_EOSC_LGS_1                    (0x20)

/*****************  Bit definition for EC610_MFP_CTL0 register  ***************/
#define EC610_GPIO5_DATA                    (0x01)
#define EC610_LDO5_MFP                      (0x0E)
#define EC610_GPIO6_DATA                    (0x10)
#define EC610_LDO6_MFP                      (0xE0)

#define EC610_LDO5_MFP_0                    (0x02)
#define EC610_LDO5_MFP_1                    (0x04)
#define EC610_LDO5_MFP_2                    (0x08)
#define EC610_LDO6_MFP_0                    (0x20)
#define EC610_LDO6_MFP_1                    (0x40)
#define EC610_LDO6_MFP_2                    (0x80)

typedef enum
{ 
  EC610_LDO5_Z     = 0x00,
  EC610_LDO5_GPI   = 0x04,
  EC610_LDO5_GPO   = 0x06,
  EC610_LDO5_ODO   = 0x08, 
  EC610_LDO5_32KO  = 0x0A,
  EC610_LDO5_IRQS  = 0x0C,
  EC610_LDO5_LDO   = 0x0E 
}EC610_LDO5_MFP_TypeDef;
#define IS_EC610_LDO5_MFP(MODE) (((MODE) == EC610_LDO5_Z) || ((MODE) == EC610_LDO5_GPI) || \
                                 ((MODE) == EC610_LDO5_GPO) || ((MODE) == EC610_LDO5_ODO) || \
                                 ((MODE) == EC610_LDO5_32KO) || ((MODE) == EC610_LDO5_IRQS) || \
                                 ((MODE) == EC610_LDO5_LDO))

typedef enum
{ 
  EC610_LDO6_Z     = 0x00,
  EC610_LDO6_GPI   = 0x40,
  EC610_LDO6_GPO   = 0x60,
  EC610_LDO6_ODO   = 0x80, 
  EC610_LDO6_32KO  = 0xA0,
  EC610_LDO6_IRQS  = 0xC0,
  EC610_LDO6_LDO   = 0xE0 
}EC610_LDO6_MFP_TypeDef;
#define IS_EC610_LDO6_MFP(MODE) (((MODE) == EC610_LDO6_Z) || ((MODE) == EC610_LDO6_GPI) || \
                                 ((MODE) == EC610_LDO6_GPO) || ((MODE) == EC610_LDO6_ODO) || \
                                 ((MODE) == EC610_LDO6_32KO) || ((MODE) == EC610_LDO6_IRQS) || \
                                 ((MODE) == EC610_LDO6_LDO))

/*****************  Bit definition for EC610_MFP_CTL1 register  ***************/
#define EC610_GPIO4_DATA                    (0x10)
#define EC610_LDO4_MFP                      (0xE0)

#define EC610_LDO4_MFP_0                    (0x20)
#define EC610_LDO4_MFP_1                    (0x40)
#define EC610_LDO4_MFP_2                    (0x80)

typedef enum
{ 
  EC610_LDO4_Z     = 0x00,
  EC610_LDO4_GPI   = 0x40,
  EC610_LDO4_GPO   = 0x60,
  EC610_LDO4_ODO   = 0x80, 
  EC610_LDO4_32KO  = 0xA0,
  EC610_LDO4_IRQS  = 0xC0,
  EC610_LDO4_LDO   = 0xE0 
}EC610_LDO4_MFP_TypeDef;
#define IS_EC610_LDO4_MFP(MODE) (((MODE) == EC610_LDO4_Z) || ((MODE) == EC610_LDO4_GPI) || \
                                 ((MODE) == EC610_LDO4_GPO) || ((MODE) == EC610_LDO4_ODO) || \
                                 ((MODE) == EC610_LDO4_32KO) || ((MODE) == EC610_LDO4_IRQS) || \
                                 ((MODE) == EC610_LDO4_LDO))

/*****************  Bit definition for EC610_MFP_CTL2 register  ***************/
#define EC610_GPIO2_DATA                    (0x01)
#define EC610_GPIO2_IEN                     (0x02)
#define EC610_GPIO2_OEN                     (0x04)
#define EC610_EXTP_EN_MFP                   (0x08)

/*****************  Bit definition for EC610_MFP_CTL3 register  ***************/
#define EC610_GPIO0_DATA                    (0x01)
#define EC610_GPIO0_IEN                     (0x02)
#define EC610_GPIO0_OEN                     (0x04)
#define EC610_GPIO1_DATA                    (0x08)
#define EC610_GPIO1_IEN                     (0x10)
#define EC610_GPIO1_OEN                     (0x20)
#define EC610_LOSC_MFP                      (0x40)

/*****************  Bit definition for EC610_MFP_CTL4 register  ***************/
#define EC610_GPIO0_PUPD                    (0x03)
#define EC610_GPIO1_PUPD                    (0x0C)
#define EC610_GPIO2_PUPD                    (0x30)

#define EC610_GPIO0_PUPD_0                  (0x01)
#define EC610_GPIO0_PUPD_1                  (0x02)
#define EC610_GPIO1_PUPD_0                  (0x04)
#define EC610_GPIO1_PUPD_1                  (0x08)
#define EC610_GPIO2_PUPD_0                  (0x10)
#define EC610_GPIO2_PUPD_1                  (0x20)

typedef enum
{ 
  EC610_GPIO0_PU    = 0x01,
  EC610_GPIO0_PD    = 0x02,
  EC610_GPIO0_Z     = 0x00
}EC610_GPIO0_MODE_TypeDef;
#define IS_EC610_GPIO0_MODE(MODE) (((MODE) == EC610_GPIO0_PU) || ((MODE) == EC610_GPIO0_PD) || ((MODE) == EC610_GPIO0_Z))

typedef enum
{ 
  EC610_GPIO1_PU    = 0x04,
  EC610_GPIO1_PD    = 0x08,
  EC610_GPIO1_Z     = 0x00
}EC610_GPIO1_MODE_TypeDef;
#define IS_EC610_GPIO1_MODE(MODE) (((MODE) == EC610_GPIO1_PU) || ((MODE) == EC610_GPIO1_PD) || ((MODE) == EC610_GPIO1_Z))

typedef enum
{ 
  EC610_GPIO2_PU    = 0x10,
  EC610_GPIO2_PD    = 0x20,
  EC610_GPIO2_Z     = 0x00
}EC610_GPIO2_MODE_TypeDef;
#define IS_EC610_GPIO2_MODE(MODE) (((MODE) == EC610_GPIO2_PU) || ((MODE) == EC610_GPIO2_PD) || ((MODE) == EC610_GPIO2_Z))

/*****************  Bit definition for EC610_MFP_CTL5 register  ***************/
#define EC610_IRQ_CTL                       (0x03)
#define EC610_IRQS_TYPE                     (0x0C)
#define EC610_IRQ_TYPE_H                    (0x10)

#define EC610_IRQ_CTL_0                     (0x01)
#define EC610_IRQ_CTL_1                     (0x02)
#define EC610_IRQS_TYPE_0                   (0x04)
#define EC610_IRQS_TYPE_1                   (0x08)

typedef enum
{ 
  EC610_IRQ_IN         = 0x02,
  EC610_IRQ_OUT        = 0x01,
  EC610_IRQ_Z          = 0x00
}EC610_IRQ_MODE_TypeDef;
#define IS_EC610_IRQ_MODE(MODE) (((MODE) == EC610_IRQ_IN) || ((MODE) == EC610_IRQ_OUT) || ((MODE) == EC610_IRQ_Z))

typedef enum
{ 
  EC610_IRQS_TYPE_H       = 0x00,
  EC610_IRQS_TYPE_L       = 0x04,
  EC610_IRQS_TYPE_RE      = 0x08,
  EC610_IRQS_TYPE_LE      = 0x0C
}EC610_IRQS_TYPE_TypeDef;
#define IS_EC610_IRQS_TYPE(TYPE) (((TYPE) == EC610_IRQS_TYPE_H) || ((TYPE) == EC610_IRQS_TYPE_L) ||\
																 ((TYPE) == EC610_IRQS_TYPE_RE) || ((MODE) == EC610_IRQS_TYPE_L))

/*****************  Bit definition for EC610_MFP_CTL6 register  ***************/
#define EC610_MFP_CTL3_PWR_UP_RST           (0x01)
#define EC610_MFP_CTL2_PWR_UP_RST           (0x02)
#define EC610_MFP_CTL1_L_PWR_UP_RST         (0x04)
#define EC610_MFP_CTL1_H_PWR_UP_RST         (0x08)
#define EC610_MFP_CTL0_L_PWR_UP_RST         (0x10)
#define EC610_MFP_CTL0_H_PWR_UP_RST         (0x20)

/*****************  Bit definition for EC610_I2C_CTL0 register  ***************/
#define EC610_I2C_LVSFT                     (0x01)


extern int mic_battery_adapter_in(void);

extern void ec610_sleep(void);
extern void ec610_deep_sleep(void);
extern void ec610_reset(void);
extern int ec610_read(struct device *dev, uint8_t reg, uint8_t *val);
extern int ec610_write(struct device *dev, u8 reg, uint8_t val);
int ec610_set_bits(struct device *dev, u8 reg, uint8_t bit_mask);
int ec610_clr_bits(struct device *dev, u8 reg, uint8_t bit_mask);
#endif
