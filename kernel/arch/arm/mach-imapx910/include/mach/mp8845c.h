/*
 *
 */

#ifndef __MP8845C_H__
#define __MP8845C_H__

#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

enum for_id {
	FOR_VCPU = 0,
	FOR_VCORE,
};

enum regulator_id {
	MP8845C_ID_FOR_VCPU = 0,
	MP8845C_ID_FOR_VCORE,
};

struct mp8845c {
	struct device *dev;
	struct i2c_client *client;
	struct mutex mpc_lock;
};

struct mp8845c_regulator {
	int	id;

	/* register address */
	uint8_t	vout_reg;

	/* chip constraints on regulator behavior */
	int	min_uv;
	int	max_uv;
	int	step_uv;
	int	nsteps;

	/* regulator core */
	struct regulator_desc   desc;

	struct device	*dev;
	struct regulator_ops	*ops;
};

struct mp8845c_regulator_platform_data {
	struct regulator_init_data regulator;
};

struct mp8845c_subdev_info {
	int id;
	const char *name;
	void *platform_data;
};

struct mp8845c_platform_data {
	int num_subdevs;
	struct mp8845c_subdev_info *subdevs;
};

static struct regulator_consumer_supply mp8845c_cpu_supply[] = {
	REGULATOR_SUPPLY("vcpu", NULL),
};

static struct regulator_consumer_supply mp8845c_core_supply[] = {
	REGULATOR_SUPPLY("vcore", NULL),
};

static struct mp8845c_regulator_platform_data pdata_for_vcpu = {
	.regulator = {
		.constraints = {
			.min_uV = 974600,
			.max_uV = 1202100,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
						REGULATOR_MODE_STANDBY,
			.valid_ops_mask = REGULATOR_CHANGE_MODE |
						REGULATOR_CHANGE_VOLTAGE,
		},
		.num_consumer_supplies = ARRAY_SIZE(mp8845c_cpu_supply),
		.consumer_supplies = mp8845c_cpu_supply,
	},
};

static struct mp8845c_regulator_platform_data pdata_for_vcore = {
	.regulator = {
		.constraints = {
			.min_uV = 974600,
			.max_uV = 1202100,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
						REGULATOR_MODE_STANDBY,
			.valid_ops_mask = REGULATOR_CHANGE_MODE |
						REGULATOR_CHANGE_VOLTAGE,
		},
		.num_consumer_supplies = ARRAY_SIZE(mp8845c_core_supply),
		.consumer_supplies = mp8845c_core_supply,
	},
};


static struct mp8845c_subdev_info mp8845c_cpu_subdevs[] = {
	{
		.id	= MP8845C_ID_FOR_VCPU,
		.name	= "mp8845c_cpu_regulator",
		.platform_data	= &pdata_for_vcpu,
	},
};

static struct mp8845c_subdev_info mp8845c_core_subdevs[] = {
	{
		.id	= MP8845C_ID_FOR_VCORE,
		.name	= "mp8845c_core_regulator",
		.platform_data	= &pdata_for_vcore,
	},
};

static struct mp8845c_platform_data mp8845c_cpu_platform_data = {
	.num_subdevs	= ARRAY_SIZE(mp8845c_cpu_subdevs),
	.subdevs	= mp8845c_cpu_subdevs,
};

static struct mp8845c_platform_data mp8845c_core_platform_data = {
	.num_subdevs	= ARRAY_SIZE(mp8845c_core_subdevs),
	.subdevs	= mp8845c_core_subdevs,
};

extern int mp8845c_cpu_read(struct device *dev, uint8_t reg, uint8_t *val);
extern int mp8845c_cpu_write(struct device *dev, uint8_t reg, uint8_t val);
extern int mp8845c_core_read(struct device *dev, uint8_t reg, uint8_t *val);
extern int mp8845c_core_write(struct device *dev, uint8_t reg, uint8_t val);

#endif
