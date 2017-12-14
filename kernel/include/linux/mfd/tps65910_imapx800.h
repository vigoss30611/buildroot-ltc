#include <linux/regulator/machine.h>
#include <linux/mfd/tps65910.h>
#include <linux/i2c.h>

static struct regulator_consumer_supply tps65910_vrtc_supply[] = {
	REGULATOR_SUPPLY("vrtc", "imapx_tps65910_vrtc_null"),
};

static struct regulator_consumer_supply tps65910_vio_supply[] = {
	REGULATOR_SUPPLY("vio", "imapx800_tps65910_vio_null"),
};

static struct regulator_consumer_supply tps65910_vdd1_supply[] = {
	REGULATOR_SUPPLY("vdd1", "imapx800_tps65910_vdd1_null"),
};

static struct regulator_consumer_supply tps65910_vdd2_supply[] = {
	REGULATOR_SUPPLY("vdd2", "imapx800_tps65910_vdd2_null"),
};

static struct regulator_consumer_supply tps65910_vdd3_supply[] = {
	REGULATOR_SUPPLY("vdd3", "imap-backlight"),
};

static struct regulator_consumer_supply tps65910_vdig1_supply[] = {
	REGULATOR_SUPPLY("vdig1", "timed-gpio"),
};

static struct regulator_consumer_supply tps65910_vdig2_supply[] = {
	REGULATOR_SUPPLY("vdig2", "imapx-isp"),
};

static struct regulator_consumer_supply tps65910_vpll_supply[] = {
	REGULATOR_SUPPLY("vpll", "imapx800_tps65910_vpll_null"),
};

static struct regulator_consumer_supply tps65910_vdac_supply[] = {
	REGULATOR_SUPPLY("vdac", "3-0018"),
};

static struct regulator_consumer_supply tps65910_vaux1_supply[] = {
	REGULATOR_SUPPLY("vaux1", "imapx-isp"),
};

static struct regulator_consumer_supply tps65910_vaux2_supply[] = {
	REGULATOR_SUPPLY("vaux2", "imap-gps"),
};

static struct regulator_consumer_supply tps65910_vaux33_supply[] = {
	REGULATOR_SUPPLY("vaux33", "1-0010"),
    	REGULATOR_SUPPLY("vaux33", "1-001a"),
    	REGULATOR_SUPPLY("vaux33", "imapx-isp"),
};

static struct regulator_consumer_supply tps65910_vmmc_supply[] = {
	REGULATOR_SUPPLY("vmmc", "imapx-ids"),
};

#define tps65910_vreg_init_data(_name, _min, _max, _modes, _ops, consumer) { \
	.constraints = {\
		.name = _name,\
		.min_uV = _min,\
		.max_uV = _max,\
		.valid_modes_mask = _modes,\
		.valid_ops_mask = _ops,\
	},\
	.num_consumer_supplies = ARRAY_SIZE(consumer),\
	.consumer_supplies = consumer,\
}

static struct regulator_init_data tps65910_verg_init_pdata[] = {
	tps65910_vreg_init_data("vrtc", 3300000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE, tps65910_vrtc_supply),
	tps65910_vreg_init_data("vio", 1500000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vio_supply),
	tps65910_vreg_init_data("vdd1", 600000, 3200000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vdd1_supply),
	tps65910_vreg_init_data("vdd2", 600000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vdd2_supply),
	tps65910_vreg_init_data("vdd3", 5000000, 5000000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE, tps65910_vdd3_supply),
	tps65910_vreg_init_data("vdig1", 1200000, 2700000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vdig1_supply),
	tps65910_vreg_init_data("vdig2", 1000000, 1800000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vdig2_supply),
	tps65910_vreg_init_data("vpll", 1000000, 2500000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vpll_supply),
	tps65910_vreg_init_data("vdac", 1800000, 2850000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vdac_supply),
	tps65910_vreg_init_data("vaux1", 1800000, 2850000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vaux1_supply),
	tps65910_vreg_init_data("vaux2", 1500000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vaux2_supply),
	tps65910_vreg_init_data("vaux33", 1500000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vaux33_supply),
	tps65910_vreg_init_data("vmmc",  1500000, 3300000, REGULATOR_MODE_NORMAL| REGULATOR_MODE_STANDBY,
			REGULATOR_CHANGE_MODE |REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, tps65910_vmmc_supply),
};

static struct tps65910_platform_data tps65910_platform_data = {
	.tps65910_pmic_init_data = tps65910_verg_init_pdata,
};

//static struct i2c_board_info imapx200_i2c0_boardinfo[] = {
//	{
//		I2C_BOARD_INFO("tps65910", 0x2d),
//		.flags = I2C_CLIENT_WAKE,
//		.platform_data = &tps65910_platform_data,
//	},
//};
