/*
 * tps65910.c  --  TI tps65910
 *
 * Copyright 2012 Shanghai Infotm Inc.
 *
 * Author: zhanglei <lei_zhang@infotm.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/mfd/tps65910.h>
#include <mach/items.h>

//#define TPS65910_REG_DEBUG
#ifdef TPS65910_REG_DEBUG
#include <linux/regulator/consumer.h>
#endif

#define TPS65910_REG_VRTC		0
#define TPS65910_REG_VIO		1
#define TPS65910_REG_VDD1		2
#define TPS65910_REG_VDD2		3
#define TPS65910_REG_VDD3		4
#define TPS65910_REG_VDIG1		5
#define TPS65910_REG_VDIG2		6
#define TPS65910_REG_VPLL		7
#define TPS65910_REG_VDAC		8
#define TPS65910_REG_VAUX1		9
#define TPS65910_REG_VAUX2		10
#define TPS65910_REG_VAUX33		11
#define TPS65910_REG_VMMC		12

#define TPS65910_SUPPLY_STATE_ENABLED	0x1

static const u16 VRTC_VSEL_table[] = {
	3300,
};

/* supported VIO voltages in milivolts */
static const u16 VIO_VSEL_table[] = {
	1500, 1800, 2500, 3300,
};

/* supported VDD1 voltages in 10*milivolts */
const u16 VDD1_VSEL_table[]={
         6000,  6125,  6250,  6375,  6500,  6625,  6750,  6875,
         7000,  7125,  7250,  7375,  7500,  7625,  7750,  7875,
         8000,  8125,  8250,  8375,  8500,  8625,  8750,  8875,
         9000,  9125,  9250,  9375,  9500,  9625,  9750,  9875,
        10000, 10125, 10250, 10375, 10500, 10625, 10750, 10875,
        11000, 11125, 11250, 11375, 11500, 11625, 11750, 11875,
        12000, 12125, 12250, 12375, 12500, 12625, 12750, 12875,
        13000, 13125, 13250, 13375, 13500, 13625, 13750, 13875,
        14000, 14125, 14250, 14375, 14500, 14625, 14750, 14875,
        15000, 22000, 32000
};

/* supported VDD2 voltages in 10*milivolts */
const u16 VDD2_VSEL_table[]={
         6000,  6125,  6250,  6375,  6500,  6625,  6750,  6875,
         7000,  7125,  7250,  7375,  7500,  7625,  7750,  7875,
         8000,  8125,  8250,  8375,  8500,  8625,  8750,  8875,
         9000,  9125,  9250,  9375,  9500,  9625,  9750,  9875,
        10000, 10125, 10250, 10375, 10500, 10625, 10750, 10875,
        11000, 11125, 11250, 11375, 11500, 11625, 11750, 11875,
        12000, 12125, 12250, 12375, 12500, 12625, 12750, 12875,
        13000, 13125, 13250, 13375, 13500, 13625, 13750, 13875,
        14000, 14125, 14250, 14375, 14500, 14625, 14750, 14875,
        15000, 22000, 33000
};


/* supported VDD3 voltages in milivolts */
static const u16 VDD3_VSEL_table[] = {
	5000,
};

/* supported VDIG1 voltages in milivolts */
static const u16 VDIG1_VSEL_table[] = {
	1200, 1500, 1800, 2700,
};

/* supported VDIG2 voltages in milivolts */
static const u16 VDIG2_VSEL_table[] = {
	1000, 1100, 1200, 1800,
};

/* supported VPLL voltages in milivolts */
static const u16 VPLL_VSEL_table[] = {
	1000, 1100, 1800, 2500,
};

/* supported VDAC voltages in milivolts */
static const u16 VDAC_VSEL_table[] = {
	1800, 2600, 2800, 2850,
};

/* supported VAUX1 voltages in milivolts */
static const u16 VAUX1_VSEL_table[] = {
	1800, 2500, 2800, 2850,
};

/* supported VAUX2 voltages in milivolts */
static const u16 VAUX2_VSEL_table[] = {
	1800, 2800, 2900, 3300,
};

/* supported VAUX33 voltages in milivolts */
static const u16 VAUX33_VSEL_table[] = {
	1800, 2000, 2800, 3300,
};

/* supported VMMC voltages in milivolts */
static const u16 VMMC_VSEL_table[] = {
	1800, 2800, 3000, 3300,
};

struct tps_info {
	const char *name;
	unsigned min_uV;
	unsigned max_uV;
	u8 table_len;
	const u16 *table;
};

static struct tps_info tps65910_regs[] = {
	{
		.name = "vrtc",
		.min_uV = 3300000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VRTC_VSEL_table),
		.table = VRTC_VSEL_table,
	},
	{
		.name = "vio",
		.min_uV = 1500000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VIO_VSEL_table),
		.table = VIO_VSEL_table,
	},
	{
		.name = "vdd1",
		.min_uV = 600000,
		.max_uV = 3200000,
		.table_len = ARRAY_SIZE(VDD1_VSEL_table),
		.table = VDD1_VSEL_table,
	},
	{
		.name = "vdd2",
		.min_uV = 600000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VDD2_VSEL_table),
		.table = VDD2_VSEL_table,
	},
	{
		.name = "vdd3",
		.min_uV = 5000000,
		.max_uV = 5000000,
		.table_len = ARRAY_SIZE(VDD3_VSEL_table),
		.table = VDD3_VSEL_table,
	},
	{
		.name = "vdig1",
		.min_uV = 1200000,
		.max_uV = 2700000,
		.table_len = ARRAY_SIZE(VDIG1_VSEL_table),
		.table = VDIG1_VSEL_table,
	},
	{
		.name = "vdig2",
		.min_uV = 1000000,
		.max_uV = 1800000,
		.table_len = ARRAY_SIZE(VDIG2_VSEL_table),
		.table = VDIG2_VSEL_table,
	},
	{
		.name = "vpll",
		.min_uV = 1000000,
		.max_uV = 2500000,
		.table_len = ARRAY_SIZE(VPLL_VSEL_table),
		.table = VPLL_VSEL_table,
	},
	{
		.name = "vdac",
		.min_uV = 1800000,
		.max_uV = 2850000,
		.table_len = ARRAY_SIZE(VDAC_VSEL_table),
		.table = VDAC_VSEL_table,
	},
	{
		.name = "vaux1",
		.min_uV = 1800000,
		.max_uV = 2850000,
		.table_len = ARRAY_SIZE(VAUX1_VSEL_table),
		.table = VAUX1_VSEL_table,
	},
	{
		.name = "vaux2",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VAUX2_VSEL_table),
		.table = VAUX2_VSEL_table,
	},
	{
		.name = "vaux33",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VAUX33_VSEL_table),
		.table = VAUX33_VSEL_table,
	},
	{
		.name = "vmmc",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.table_len = ARRAY_SIZE(VMMC_VSEL_table),
		.table = VMMC_VSEL_table,
	},
};

struct tps65910_reg {
	struct regulator_desc *desc;
	struct tps65910 *mfd;
	struct regulator_dev **rdev;
	struct tps_info **info;
	struct mutex mutex;
	int num_regulators;
	int mode;
	int  (*get_ctrl_reg)(int);
};

static inline int tps65910_read(struct tps65910_reg *pmic, u8 reg)
{
	u8 val;
	int err;

	err = pmic->mfd->read(pmic->mfd, reg, 1, &val);
	if (err)
		return err;

	return val;
}

static inline int tps65910_write(struct tps65910_reg *pmic, u8 reg, u8 val)
{
	return pmic->mfd->write(pmic->mfd, reg, 1, &val);
}

static int tps65910_modify_bits(struct tps65910_reg *pmic, u8 reg,
					u8 set_mask, u8 clear_mask)
{
	int err, data;

	mutex_lock(&pmic->mutex);

	data = tps65910_read(pmic, reg);
	if (data < 0) {
		dev_err(pmic->mfd->dev, "Read from reg 0x%x failed\n", reg);
		err = data;
		goto out;
	}

	data &= ~clear_mask;
	data |= set_mask;
	err = tps65910_write(pmic, reg, data);
	if (err)
		dev_err(pmic->mfd->dev, "Write for reg 0x%x failed\n", reg);

out:
	mutex_unlock(&pmic->mutex);
	return err;
}

static int tps65910_reg_read(struct tps65910_reg *pmic, u8 reg)
{
	int data;

	mutex_lock(&pmic->mutex);

	data = tps65910_read(pmic, reg);
	if (data < 0)
		dev_err(pmic->mfd->dev, "Read from reg 0x%x failed\n", reg);

	mutex_unlock(&pmic->mutex);
	return data;
}

static int tps65910_reg_write(struct tps65910_reg *pmic, u8 reg, u8 val)
{
	int err;

	mutex_lock(&pmic->mutex);

	err = tps65910_write(pmic, reg, val);
	if (err < 0)
		dev_err(pmic->mfd->dev, "Write for reg 0x%x failed\n", reg);

	mutex_unlock(&pmic->mutex);
	return err;
}

static int tps65910_get_ctrl_register(int id)
{
	switch (id) {
	case TPS65910_REG_VRTC:
		return TPS65910_VRTC;
	case TPS65910_REG_VIO:
		return TPS65910_VIO;
	case TPS65910_REG_VDD1:
		return TPS65910_VDD1;
	case TPS65910_REG_VDD2:
		return TPS65910_VDD2;
	case TPS65910_REG_VDD3:
		return TPS65910_VDD3;
	case TPS65910_REG_VDIG1:
		return TPS65910_VDIG1;
	case TPS65910_REG_VDIG2:
		return TPS65910_VDIG2;
	case TPS65910_REG_VPLL:
		return TPS65910_VPLL;
	case TPS65910_REG_VDAC:
		return TPS65910_VDAC;
	case TPS65910_REG_VAUX1:
		return TPS65910_VAUX1;
	case TPS65910_REG_VAUX2:
		return TPS65910_VAUX2;
	case TPS65910_REG_VAUX33:
		return TPS65910_VAUX33;
	case TPS65910_REG_VMMC:
		return TPS65910_VMMC;
	default:
		return -EINVAL;
	}
}

static int tps65910_is_enabled(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int reg, value=0, id = rdev_get_id(dev), rep_cnt=3;

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;

	while(rep_cnt--)
	{
		value = tps65910_reg_read(pmic, reg);
		if (value >= 0)
			break;
	}

	return value & TPS65910_SUPPLY_STATE_ENABLED;
}

static int tps65910_enable(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	struct tps65910 *mfd = pmic->mfd;
	int reg, id = rdev_get_id(dev);

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;
	return tps65910_set_bits(mfd, reg, TPS65910_SUPPLY_STATE_ENABLED);
}

static int tps65910_disable(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	struct tps65910 *mfd = pmic->mfd;
	int reg, id = rdev_get_id(dev);

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;

	return tps65910_clear_bits(mfd, reg, TPS65910_SUPPLY_STATE_ENABLED);
}


static int tps65910_set_mode(struct regulator_dev *dev, unsigned int mode)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	struct tps65910 *mfd = pmic->mfd;
	int reg, value, id = rdev_get_id(dev);

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;

	switch (mode) {
	case REGULATOR_MODE_NORMAL:
		return tps65910_modify_bits(pmic, reg, LDO_ST_ON_BIT,
							LDO_ST_MODE_BIT);
	case REGULATOR_MODE_IDLE:
		value = LDO_ST_ON_BIT | LDO_ST_MODE_BIT;
		return tps65910_set_bits(mfd, reg, value);
	case REGULATOR_MODE_STANDBY:
		return tps65910_clear_bits(mfd, reg, LDO_ST_ON_BIT);
	}

	return -EINVAL;
}

static unsigned int tps65910_get_mode(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int reg, value, id = rdev_get_id(dev);

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;

	value = tps65910_reg_read(pmic, reg);
	if (value < 0)
		return value;

	if (value & LDO_ST_ON_BIT)
		return REGULATOR_MODE_STANDBY;
	else if (value & LDO_ST_MODE_BIT)
		return REGULATOR_MODE_IDLE;
	else
		return REGULATOR_MODE_NORMAL;
}

static int tps65910_get_voltage_dcdc(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int id = rdev_get_id(dev), voltage = 0;
	int opvsel = 0, srvsel = 0, vselmax = 0, mult = 0, sr = 0;

	switch (id) {
	case TPS65910_REG_VDD1:
		opvsel = tps65910_reg_read(pmic, TPS65910_VDD1_OP);
		mult = tps65910_reg_read(pmic, TPS65910_VDD1);
		mult = (mult & VDD1_VGAIN_SEL_MASK) >> VDD1_VGAIN_SEL_SHIFT;
		srvsel = tps65910_reg_read(pmic, TPS65910_VDD1_SR);
		sr = opvsel & VDD1_OP_CMD_MASK;
		opvsel &= VDD1_OP_SEL_MASK;
		srvsel &= VDD1_SR_SEL_MASK;
		vselmax = 75;
		break;
	case TPS65910_REG_VDD2:
		opvsel = tps65910_reg_read(pmic, TPS65910_VDD2_OP);
		mult = tps65910_reg_read(pmic, TPS65910_VDD2);
		mult = (mult & VDD2_VGAIN_SEL_MASK) >> VDD2_VGAIN_SEL_SHIFT;
		srvsel = tps65910_reg_read(pmic, TPS65910_VDD2_SR);
		sr = opvsel & VDD2_OP_CMD_MASK;
		opvsel &= VDD2_OP_SEL_MASK;
		srvsel &= VDD2_SR_SEL_MASK;
		vselmax = 75;
		break;
	}

	/* multiplier 0 == 1 but 2,3 normal */
	if (!mult)
		mult=1;

	if (sr) {
		/* normalise to valid range */
		if (srvsel < 3)
			srvsel = 3;
		if (srvsel > vselmax)
			srvsel = vselmax;
		srvsel -= 3;

		voltage = (srvsel * VDD1_2_OFFSET + VDD1_2_MIN_VOLT) * 100;
	} else {

		/* normalise to valid range*/
		if (opvsel < 3)
			opvsel = 3;
		if (opvsel > vselmax)
			opvsel = vselmax;
		opvsel -= 3;

		voltage = (opvsel * VDD1_2_OFFSET + VDD1_2_MIN_VOLT) * 100;
	}

	voltage *= mult;

	return voltage;
}

static int tps65910_get_voltage_vdd3(struct regulator_dev *dev)
{
	return 5 * 1000 * 1000;
}

static int tps65910_get_voltage(struct regulator_dev *dev)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int reg, value, id = rdev_get_id(dev), voltage = 0;

	reg = pmic->get_ctrl_reg(id);
	if (reg < 0)
		return reg;

	value = tps65910_reg_read(pmic, reg);
	if (value < 0)
		return value;

	switch (id) {
	case TPS65910_REG_VIO:
	case TPS65910_REG_VDIG1:
	case TPS65910_REG_VDIG2:
	case TPS65910_REG_VPLL:
	case TPS65910_REG_VDAC:
	case TPS65910_REG_VAUX1:
	case TPS65910_REG_VAUX2:
	case TPS65910_REG_VAUX33:
	case TPS65910_REG_VMMC:
		value &= LDO_SEL_MASK;
		value >>= LDO_SEL_SHIFT;
		break;
	default:
		return -EINVAL;
	}

	voltage = pmic->info[id]->table[value] * 1000;

	return voltage;
}

static int tps65910_set_voltage_dcdc(struct regulator_dev *dev, int min_uV, 
	int max_uV, unsigned *selector)
{

	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int  id = rdev_get_id(dev);
       int reg, reg2, val, vsel;

        if(min_uV < pmic->info[id]->min_uV ||
                        min_uV > pmic->info[id]->max_uV)
                goto err;
        if(max_uV < pmic->info[id]->min_uV ||
                        max_uV > pmic->info[id]->max_uV)
                goto err;

        for(vsel = 0; vsel < pmic->info[id]->table_len; vsel++){
                int val = pmic->info[id]->table[vsel];
                int uV = val * 100;

                if(min_uV <= uV && uV <= max_uV)
                        break;
        }

        if(vsel == pmic->info[id]->table_len)
                goto err;

        if(id == TPS65910_REG_VDD1){
                reg = TPS65910_VDD1;
                reg2 = TPS65910_VDD1_OP;
        }
        else if(id == TPS65910_REG_VDD2){
                reg = TPS65910_VDD2;
                reg2 = TPS65910_VDD2_OP;
        }
        else
                goto err;

        if(vsel == pmic->info[id]->table_len - 1){
                val = tps65910_reg_read(pmic, reg);
                val |= 0xC0;
                val = tps65910_reg_write(pmic, reg, val);
        }else if(vsel == pmic->info[id]->table_len - 2){
                val = tps65910_reg_read(pmic, reg);
                val &= 0x3f;
                val |= 0x40;
                val = tps65910_reg_write(pmic, reg, val);
        }else{
                val = tps65910_reg_read(pmic, reg2);
                val &= 0x7f;
                val |= (vsel + 3);
                val = tps65910_reg_write(pmic, reg2,val);
                val = tps65910_reg_read(pmic, reg);
                val &= 0x3f;
                val = tps65910_reg_write(pmic, reg, val);
        }

        return 0;
err:
        dev_err(pmic->mfd->dev, "TPS65910 set vdd voltage error!\n");
        return -EINVAL;

}

static int tps65910_set_voltage(struct regulator_dev *dev, int min_uV, 
	int max_uV, unsigned *selector)

{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int reg, val, vsel, id = rdev_get_id(dev);

	printk("%s: run !\n", __func__);
	
        if(min_uV < pmic->info[id]->min_uV ||
                        min_uV > pmic->info[id]->max_uV)
                goto err;
        if(max_uV < pmic->info[id]->min_uV ||
                        max_uV > pmic->info[id]->max_uV)
                goto err;

        for(vsel = 0; vsel < pmic->info[id]->table_len; vsel++){
                int Val = pmic->info[id]->table[vsel];
                int uV = Val * 1000;

                if(min_uV <= uV && uV <= max_uV)
                        break;
        }

        if(vsel >= pmic->info[id]->table_len)
                goto err;

        reg = pmic->get_ctrl_reg(id);
        val = tps65910_reg_read(pmic, reg);
        val &= 0xf3;
        val |= (vsel << 2);
        val = tps65910_reg_write(pmic, reg, val);

        return 0;

err:
        dev_err(pmic->mfd->dev, "TPS65910 set ldo voltage failed!\n");
        return -EINVAL;


}

static int tps65910_list_voltage(struct regulator_dev *dev,
					unsigned selector)
{
	struct tps65910_reg *pmic = rdev_get_drvdata(dev);
	int id = rdev_get_id(dev), voltage;

	if (id < TPS65910_REG_VRTC || id > TPS65910_REG_VMMC)
		return -EINVAL;

	if (selector >= pmic->info[id]->table_len)
		return -EINVAL;
	else if((id == TPS65910_REG_VDD1) ||(id == TPS65910_REG_VDD2))
		voltage = pmic->info[id]->table[selector] * 100;
	else
		voltage = pmic->info[id]->table[selector] * 1000;

	return voltage;
}


/* Regulator ops (except VRTC) */
static struct regulator_ops tps65910_ops_dcdc = {
	.is_enabled		= tps65910_is_enabled,
	.enable			= tps65910_enable,
	.disable		= tps65910_disable,
	.set_mode		= tps65910_set_mode,
	.get_mode		= tps65910_get_mode,
	.get_voltage		= tps65910_get_voltage_dcdc,
	.set_voltage	= tps65910_set_voltage_dcdc,
	.list_voltage		= tps65910_list_voltage,
};

static struct regulator_ops tps65910_ops_vdd3 = {
	.is_enabled		= tps65910_is_enabled,
	.enable			= tps65910_enable,
	.disable		= tps65910_disable,
	.set_mode		= tps65910_set_mode,
	.get_mode		= tps65910_get_mode,
	.get_voltage		= tps65910_get_voltage_vdd3,
	.list_voltage		= tps65910_list_voltage,
};

static struct regulator_ops tps65910_ops = {
	.is_enabled		= tps65910_is_enabled,
	.enable			= tps65910_enable,
	.disable		= tps65910_disable,
	.set_mode		= tps65910_set_mode,
	.get_mode		= tps65910_get_mode,
	.get_voltage		= tps65910_get_voltage,
	.set_voltage	= tps65910_set_voltage,
	.list_voltage		= tps65910_list_voltage,
};

#ifdef TPS65910_REG_DEBUG
static void tps65910_test(void)
{
	static struct regulator *rreg;
	//struct device *dev;
	int err;

	struct device *dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		printk("kzalloc dev fail\n");
	}
	dev->init_name = "imap-fb";

	printk("%s: run!!!\n", __func__);
	rreg = regulator_get(dev, "TPS65910_VMMC");
	if(IS_ERR(rreg))
	{
		printk("%s: get regulator fail!\n", __func__);
		regulator_put(rreg);
		return;
	}
	printk("%s: regulator get success. \n", __func__);

	err = regulator_disable(rreg);
	if(err<0)
	{
		printk("regulator disable fail\n");
	}
	printk("regulator disable success\n");
	
	err = regulator_set_voltage(rreg, 3300000, 3300000);
	if(err < 0)
	{
		printk("%s: regulator_set_voltage fail\n", __func__);
		return;
	}
	printk("%s: regulator set voltage success. \n", __func__);
	
	err = regulator_enable(rreg);
	if(err == 0)
		printk("%s: regulator_enable success\n", __func__);
	else
		printk("%s: regulator enable fail\n", __func__);

	printk("regulator has been enabled, test is success!\n");
}
#endif

static int tps65910_probe(struct platform_device *pdev)
{
	struct tps65910 *tps65910 = dev_get_drvdata(pdev->dev.parent);
	struct tps_info *info;
	struct regulator_init_data *reg_data;
	struct regulator_dev *rdev;
	struct tps65910_reg *pmic;
	struct tps65910_platform_data *pmic_plat_data;
	int i, err;
	struct regulator_config config = { };

	//printk("****** %s run ! ******\n", __func__);

	pmic_plat_data = dev_get_platdata(tps65910->dev);
	if (!pmic_plat_data)
		return -EINVAL;

	reg_data = pmic_plat_data->tps65910_pmic_init_data;

	pmic = kzalloc(sizeof(*pmic), GFP_KERNEL);
	if (!pmic)
		return -ENOMEM;

	mutex_init(&pmic->mutex);
	pmic->mfd = tps65910;
	platform_set_drvdata(pdev, pmic);

	/* Give control of all register to control port */
	//tps65910_set_bits(pmic->mfd, TPS65910_DEVCTRL,
	//			DEVCTRL_SR_CTL_I2C_SEL_MASK);

	pmic->get_ctrl_reg = tps65910_get_ctrl_register;
	pmic->num_regulators = ARRAY_SIZE(tps65910_regs);
	info = tps65910_regs;
	//printk("tps65910_regs size is %d\n", pmic->num_regulators);

	pmic->desc = kcalloc(pmic->num_regulators,
			sizeof(struct regulator_desc), GFP_KERNEL);
	if (!pmic->desc) {
		err = -ENOMEM;
		goto err_free_pmic;
	}

	pmic->info = kcalloc(pmic->num_regulators,
			sizeof(struct tps_info *), GFP_KERNEL);
	if (!pmic->info) {
		err = -ENOMEM;
		goto err_free_desc;
	}

	pmic->rdev = kcalloc(pmic->num_regulators,
			sizeof(struct regulator_dev *), GFP_KERNEL);
	if (!pmic->rdev) {
		err = -ENOMEM;
		goto err_free_info;
	}

	for (i = 0; i < pmic->num_regulators; i++, info++, reg_data++) {
		/* Register the regulators */
		//printk("register the regulator %d\n", i);
		pmic->info[i] = info;

		pmic->desc[i].name = info->name;
		pmic->desc[i].id = i;
		pmic->desc[i].n_voltages = info->table_len;
		//printk("pmic->desc[%d].name is %s\n", i, pmic->desc[i].name);
		
		if (i == TPS65910_REG_VDD1 || i == TPS65910_REG_VDD2) {
			pmic->desc[i].ops = &tps65910_ops_dcdc;
			//pmic->desc[i].n_voltages = VDD1_2_NUM_VOLT_FINE *
			//				VDD1_2_NUM_VOLT_COARSE;
		} else if (i == TPS65910_REG_VDD3) {
			pmic->desc[i].ops = &tps65910_ops_vdd3;
		} else {
			pmic->desc[i].ops = &tps65910_ops;
		}

		pmic->desc[i].type = REGULATOR_VOLTAGE;
		pmic->desc[i].owner = THIS_MODULE;

		config.dev = tps65910->dev;
		config.init_data = reg_data;
		config.driver_data = pmic;

		rdev = regulator_register(&pmic->desc[i], &config);
		//printk("regulator_register() end\n");
		if (IS_ERR(rdev)) {
			dev_err(tps65910->dev,
				"failed to register %s regulator\n",
				pdev->name);
			err = PTR_ERR(rdev);
			goto err_unregister_regulator;
		}

		/* Save regulator for cleanup */
		pmic->rdev[i] = rdev;
	}
	
	//printk("****** %s end! ****** \n", __func__);

#ifdef TPS65910_REG_DEBUG
	printk("*** regulator ready test! ***\n");
	tps65910_test();
	printk("*** regulator ready test over! ***\n");
#endif
	return 0;

err_unregister_regulator:
	//printk("err_unregister_regulator\n");
	while (--i >= 0)
		regulator_unregister(pmic->rdev[i]);
	kfree(pmic->rdev);
err_free_info:
	kfree(pmic->info);
err_free_desc:
	kfree(pmic->desc);
err_free_pmic:
	kfree(pmic);

	printk("****** %s Error! ******\n", __func__);
	return err;
}

static int tps65910_remove(struct platform_device *pdev)
{
	struct tps65910_reg *pmic = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < pmic->num_regulators; i++)
		regulator_unregister(pmic->rdev[i]);

	kfree(pmic->rdev);
	kfree(pmic->info);
	kfree(pmic->desc);
	kfree(pmic);
	return 0;
}

static struct platform_driver tps65910_driver = {
	.driver = {
		.name = "tps65910-pmic",
		.owner = THIS_MODULE,
	},
	.probe = tps65910_probe,
	.remove = tps65910_remove,
};

static int __init tps65910_init(void)
{
	printk("****** %s run ! ******\n", __func__);
	if(item_exist("pmu.model"))
	{
		if(item_equal("pmu.model", "tps65910", 0))
			return platform_driver_register(&tps65910_driver);
		else
			printk(KERN_ERR "%s: pmu model is not tps65910\n", __func__);
	}
	else
		printk(KERN_ERR "%s: pmu model is not exist\n", __func__);

	return -1;
}
subsys_initcall(tps65910_init);
//module_init(tps65910_init);

static void __exit tps65910_cleanup(void)
{
	platform_driver_unregister(&tps65910_driver);
}
module_exit(tps65910_cleanup);

MODULE_AUTHOR("zhanglei <lei_zhang@infotm.com>");
MODULE_DESCRIPTION("TPS65910 voltage regulator driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:tps65910-pmic");

