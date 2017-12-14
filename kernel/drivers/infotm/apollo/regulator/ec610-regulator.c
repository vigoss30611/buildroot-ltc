/*
 * drivers/regulator/ec610-regulator.c
 *
 * Regulator driver for Elitechip EC610 power management chip.
 *
 * Copyright (C) 2012-2013 Elitechip COMPANY,LTD
 *
 * Based on code
 *	Copyright (C) 2011 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*#define DEBUG			1*/
/*#define VERBOSE_DEBUG		1*/
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/ec610.h>

struct ec610_regulator {
	int		id;
	
	/* Regulator register address.*/
	u8		en_reg;
	u8		en_bit;
	
	u8		vout_reg_cache;
	u8		vout_reg;
	u8		vout_mask;
	u8		vout_shift;

	/* chip constraints on regulator behavior */
	int			min_uv;
	int			max_uv;
	int			step_uv;
	
	int			nsteps_reg;
	int			nsteps_mask;
	int			nsteps_uv;
	
	/* regulator specific turn-on delay */
	u16			delay;

	/* used by regulator core */
	struct regulator_desc	desc;

	/* Device */
	struct device		*dev;
};

static unsigned int ec610_suspend_status;

static inline struct device *to_ec610_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int ec610_reg_is_enabled(struct regulator_dev *rdev)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ec610_dev(rdev);
	uint8_t control;
	int ret;

	ret = ec610_read(parent, ri->en_reg, &control);
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in reading the control register\n");
		return ret;
	}
	return (((control >> ri->en_bit) & 1) == 1);
}

static int ec610_reg_enable(struct regulator_dev *rdev)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ec610_dev(rdev);
	int ret;

	ret = ec610_set_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in updating the STATE register\n");
		return ret;
	}
	
	udelay(ri->delay);
	
	//dev_err(&rdev->dev, "[%s]: reg=0x%2x, bit=%d\n", __func__, ri->en_reg, ri->en_bit);
	
	return ret;
}

static int ec610_reg_disable(struct regulator_dev *rdev)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ec610_dev(rdev);
	int ret;

	ret = ec610_clr_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0)
		dev_err(&rdev->dev, "Error in updating the STATE register\n");
	
	//dev_err(&rdev->dev, "[%s]: reg=0x%2x, bit=%d\n", __func__, ri->en_reg, ri->en_bit);

	return ret;
}

static int ec610_list_voltage(struct regulator_dev *rdev, unsigned index)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);

	return ri->min_uv + (ri->step_uv * index);
}

static int __ec610_set_voltage(struct device *parent,
		struct ec610_regulator *ri, int min_uv, int max_uv,
		unsigned *selector)
{
	int vsel;
	int ret;
	uint8_t vout_val;

	if ((min_uv < ri->min_uv) || (max_uv > ri->max_uv))
		return -EDOM;

	vsel = (min_uv - ri->min_uv)/ri->step_uv;

	if (selector)
		*selector = vsel;

	vout_val = (ri->vout_reg_cache & ~ri->vout_mask) |
				((vsel << ri->vout_shift) & ri->vout_mask);
	//dev_err(ri->dev, "[%s]: reg=0x%2x, val=0x%2x\n", __func__, ri->vout_reg, vout_val);
	ret = ec610_write(parent, ri->vout_reg, vout_val);
	if (ret < 0)
		dev_err(ri->dev, "Error in writing the Voltage register\n");
	else
		ri->vout_reg_cache = vout_val;

	return ret;
}

static int ec610_set_voltage(struct regulator_dev *rdev,
		int min_uv, int max_uv, unsigned *selector)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ec610_dev(rdev);

	if (ec610_suspend_status)
		return -EBUSY;

	return __ec610_set_voltage(parent, ri, min_uv, max_uv, selector);
}

static int ec610_get_voltage(struct regulator_dev *rdev)
{
	struct ec610_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ec610_dev(rdev);
	uint8_t vsel;

	//vsel = (ri->vout_reg_cache >> ri->vout_shift) & (ri->vout_mask >> ri->vout_shift);
	ec610_read(parent, ri->vout_reg, &vsel);
	if (ri->id < 3)
		vsel = vsel >> 1;
	else if (ri->id == 3)
		vsel = (vsel & 0x7e) >> 1;
	//dev_err(ri->dev, "[%s]: reg=0x%2x, lastval=0x%2x, val=%d\n", __func__, ri->vout_reg, ri->vout_reg_cache, (ri->min_uv + vsel * ri->step_uv));
	return ri->min_uv + vsel * ri->step_uv;
}

static struct regulator_ops ec610_ops = {
	.list_voltage	= ec610_list_voltage,
	.set_voltage	= ec610_set_voltage,
	.get_voltage	= ec610_get_voltage,
	.enable		= ec610_reg_enable,
	.disable	= ec610_reg_disable,
	.is_enabled	= ec610_reg_is_enabled,
};

#define ec610_rails(_name) ""#_name

#define EC610_REG(_id, _en_reg, _en_bit, _vout_reg, _vout_mask, _vout_shift, \
		_min_mv, _max_mv, _step_mv, _nsteps_reg, _nsteps_mask, _nsteps, _ops, _delay)		\
{								\
	.en_reg	= _en_reg,				\
	.en_bit		= _en_bit,				\
	.vout_reg	= _vout_reg,				\
	.vout_mask	= _vout_mask,				\
	.vout_shift	= _vout_shift,				\
	.min_uv		= _min_mv * 1000,			\
	.max_uv		= _max_mv * 1000,			\
	.step_uv	= _step_mv * 1000,				\
	.nsteps_reg	= _nsteps_reg,				\
	.nsteps_mask	= _nsteps_mask,				\
	.nsteps_uv		= _nsteps,				\
	.delay		= _delay,				\
	.id		= EC610_ID_##_id,			\
	.desc = {						\
		.name = ec610_rails(_id),			\
		.id = EC610_ID_##_id,			\
		.n_voltages = _nsteps,				\
		.ops = &_ops,					\
		.type = REGULATOR_VOLTAGE,			\
		.owner = THIS_MODULE,				\
	},							\
}

static struct ec610_regulator ec610_regulator[] = {
	EC610_REG(dc0, 0x20, 0, 0x21, 0x3E, 1, 600, 1375, 25,
			0x22, 0xC0, 0x1F, ec610_ops, 500),
	EC610_REG(dc1, 0x20, 1, 0x28, 0x3E, 1, 600, 1375, 25,
			0x29, 0xC0, 0x1F, ec610_ops, 500),
	EC610_REG(dc2, 0x20, 2, 0x2F, 0x3E, 1, 600, 2175, 25,
			0x30, 0xC0, 0x3F, ec610_ops, 500),
	EC610_REG(dc3, 0x20, 3, 0x36, 0x3E, 1, 2200, 3500, 25,
			0x37, 0xC0, 0x34, ec610_ops, 500),
	EC610_REG(ldo0, 0x41, 0, 0x42, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),
	EC610_REG(ldo1, 0x41, 1, 0x44, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),
	EC610_REG(ldo2, 0x41, 2, 0x46, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),
	EC610_REG(ldo4, 0x41, 4, 0x4A, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),
	EC610_REG(ldo5, 0x41, 5, 0x4C, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),
	EC610_REG(ldo6, 0x41, 6, 0x4E, 0x7F, 0, 700, 3400, 25,
			0, 0, 0x6C, ec610_ops, 500),

};

static inline struct ec610_regulator *find_regulator_info(int id)
{
	struct ec610_regulator *ri;
	int i;

	for (i = 0; i < ARRAY_SIZE(ec610_regulator); i++) {
		ri = &ec610_regulator[i];
		if (ri->desc.id == id)
			return ri;
	}
	return NULL;
}

//static int ec610_regulator_preinit(struct device *parent,
//		struct ec610_regulator *ri,
//		struct EC610618_regulator_platform_data *ec610_pdata)
//{
//	int ret = 0;
//
//	if (!ec610_pdata->init_apply)
//		return 0;
//
//	if (ec610_pdata->init_uv >= 0) {
//		ret = __ec610_set_voltage(parent, ri,
//				ec610_pdata->init_uv,
//				ec610_pdata->init_uv, 0);
//		if (ret < 0) {
//			dev_err(ri->dev,
//				"fail to initialize voltage %d for rail %d, err %d\n",
//				ec610_pdata->init_uv, ri->desc.id, ret);
//			return ret;
//		}
//	}
//
//	if (ec610_pdata->init_enable)
//		ret = ec610_set_bits(parent, ri->reg_en_reg,
//						(1 << ri->en_bit));
//	else
//		ret = ec610_clr_bits(parent, ri->reg_en_reg,
//						(1 << ri->en_bit));
//	if (ret < 0)
//		dev_err(ri->dev, "Not able to %s rail %d err %d\n",
//			(ec610_pdata->init_enable) ? "enable" : "disable",
//			ri->desc.id, ret);
//
//	return ret;
//}

static inline int ec610_cache_regulator_register(struct device *parent,
	struct ec610_regulator *ri)
{
	ri->vout_reg_cache = 0;
	return ec610_read(parent, ri->vout_reg, &ri->vout_reg_cache);
}

static int ec610_regulator_probe(struct platform_device *pdev)
{
	struct ec610_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct ec610_regulator_platform_data *tps_pdata;
	struct regulator_config config = { };
	int id = pdev->id;
	int err;

	ri = find_regulator_info(id);
	if (ri == NULL) {
		dev_err(&pdev->dev, "invalid regulator ID specified\n");
		return -EINVAL;
	}
	tps_pdata = pdev->dev.platform_data;
	ri->dev = &pdev->dev;
	ec610_suspend_status = 0;

	err = ec610_cache_regulator_register(pdev->dev.parent, ri);
	if (err) {
		dev_err(&pdev->dev, "Fail in caching register\n");
		return err;
	}

//	err = ec610_regulator_preinit(pdev->dev.parent, ri, tps_pdata);
//	if (err) {
//		dev_err(&pdev->dev, "Fail in pre-initialisation\n");
//		return err;
//	}

	config.dev = &pdev->dev;
	config.init_data = &tps_pdata->regulator;
	config.driver_data = ri;

	rdev = regulator_register(&ri->desc, &config);
	/* rdev = regulator_register(&ri->desc, &pdev->dev,
				&tps_pdata->regulator, ri, NULL); */
	if (IS_ERR_OR_NULL(rdev)) {
		dev_err(&pdev->dev, "failed to register regulator %s\n",
				ri->desc.name);
		return PTR_ERR(rdev);
	}

	platform_set_drvdata(pdev, rdev);
	
	/*dev_err(&pdev->dev, "----------------id=%d\n", id);*/
	return 0;
}

static int ec610_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);
	return 0;
}

#ifdef CONFIG_PM
static int ec610_regulator_suspend(struct device *dev)
{
//	struct ec610_regulator *info = dev_get_drvdata(dev);

	EC610_DBG("PMU: %s\n", __func__);
	ec610_suspend_status = 1;

	return 0;
}

static int ec610_regulator_resume(struct device *dev)
{
//	struct ec610_regulator *info = dev_get_drvdata(dev);

	EC610_DBG("PMU: %s\n", __func__);
	ec610_suspend_status = 0;

	return 0;
}

static const struct dev_pm_ops ec610_regulator_pm_ops = {
	.suspend	= ec610_regulator_suspend,
	.resume		= ec610_regulator_resume,
};
#endif

static struct platform_driver ec610_regulator_driver = {
	.driver	= {
		.name	= "ec610-regulator",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ec610_regulator_pm_ops,
#endif
	},
	.probe		= ec610_regulator_probe,
	.remove		= ec610_regulator_remove,
};

static int __init ec610_regulator_init(void)
{
	EC610_INFO("PMU: %s\n", __func__);
	return platform_driver_register(&ec610_regulator_driver);
}
subsys_initcall(ec610_regulator_init);

static void __exit ec610_regulator_exit(void)
{
	platform_driver_unregister(&ec610_regulator_driver);
}
module_exit(ec610_regulator_exit);

MODULE_DESCRIPTION("EC610 R5T618 regulator driver");
MODULE_ALIAS("platform:EC610618-regulator");
MODULE_LICENSE("GPL");
