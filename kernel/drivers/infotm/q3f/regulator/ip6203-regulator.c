/*
 * drivers/regulator/ip6203-regulator.c
 *
 * Regulator driver for Elitechip IP6203 power management chip.
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
#include <mach/ip6203.h>

struct ip6203_regulator {
	int		id;
	
	/* Regulator register address.*/
	u8		en_reg;
	u8		en_bit;
	
	u16		vout_reg_cache;
	u8		vout_reg;
	u16		vout_mask;
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

static unsigned int ip6203_suspend_status;

static inline struct device *to_ip6203_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int ip6203_reg_is_enabled(struct regulator_dev *rdev)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6203_dev(rdev);
	uint16_t control;
	int ret;

	ret = ip6203_read(parent, ri->en_reg, &control);
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in reading the control register\n");
		return ret;
	}
	return (((control >> ri->en_bit) & 1) == 1);
}

static int ip6203_reg_enable(struct regulator_dev *rdev)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6203_dev(rdev);
	int ret;

	ret = ip6203_set_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in updating the STATE register\n");
		return ret;
	}
	
	udelay(ri->delay);
	
	//dev_err(&rdev->dev, "[%s]: reg=0x%2x, bit=%d\n", __func__, ri->en_reg, ri->en_bit);
	
	return ret;
}

static int ip6203_reg_disable(struct regulator_dev *rdev)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6203_dev(rdev);
	int ret;

	ret = ip6203_clr_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0)
		dev_err(&rdev->dev, "Error in updating the STATE register\n");
	
	//dev_err(&rdev->dev, "[%s]: reg=0x%2x, bit=%d\n", __func__, ri->en_reg, ri->en_bit);

	return ret;
}

static int ip6203_list_voltage(struct regulator_dev *rdev, unsigned index)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);

	return ri->min_uv + (ri->step_uv * index);
}

static int __ip6203_set_voltage(struct device *parent,
		struct ip6203_regulator *ri, int min_uv, int max_uv,
		unsigned *selector)
{
	int vsel;
	int ret;
	uint16_t vout_val;

	if ((min_uv < ri->min_uv) || (max_uv > ri->max_uv))
		return -EDOM;

	vsel = (min_uv - ri->min_uv)/ri->step_uv;

	if (selector)
		*selector = vsel;

	vout_val = (ri->vout_reg_cache & ~ri->vout_mask) |
				((vsel << ri->vout_shift) & ri->vout_mask);
	//dev_err(ri->dev, "[%s]: reg=0x%x, val=0x%x\n", __func__, ri->vout_reg, vout_val);
	ret = ip6203_write(parent, ri->vout_reg, vout_val);
	if (ret < 0)
		dev_err(ri->dev, "Error in writing the Voltage register\n");
	else
		ri->vout_reg_cache = vout_val;

	return ret;
}

static int ip6203_set_voltage(struct regulator_dev *rdev,
		int min_uv, int max_uv, unsigned *selector)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6203_dev(rdev);

	if (ip6203_suspend_status)
		return -EBUSY;

	return __ip6203_set_voltage(parent, ri, min_uv, max_uv, selector);
}

static int ip6203_get_voltage(struct regulator_dev *rdev)
{
	struct ip6203_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6203_dev(rdev);
	uint16_t vsel;

	//vsel = (ri->vout_reg_cache >> ri->vout_shift) & (ri->vout_mask >> ri->vout_shift);
	ip6203_read(parent, ri->vout_reg, &vsel);
	//printk("%d, %d, %d, %d\n", vsel, ri->min_uv, ri->max_uv, ri->step_uv);
#if 0
	if (ri->id < 3)
		vsel = vsel >> 1;
	else if (ri->id == 3)
		vsel = (vsel & 0x7e) >> 1;
#endif
	return ri->min_uv + vsel * ri->step_uv;
}

static struct regulator_ops ip6203_ops = {
	.list_voltage	= ip6203_list_voltage,
	.set_voltage	= ip6203_set_voltage,
	.get_voltage	= ip6203_get_voltage,
	.enable		= ip6203_reg_enable,
	.disable	= ip6203_reg_disable,
	.is_enabled	= ip6203_reg_is_enabled,
};

#define ip6203_rails(_name) ""#_name

#define IP6203_REG(_id, _en_reg, _en_bit, _vout_reg, _vout_mask, _vout_shift, \
		_min_mv, _max_mv, _step_mv, _nsteps_reg, _nsteps_mask, _nsteps, _ops, _delay)		\
{								\
	.en_reg	= _en_reg,				\
	.en_bit		= _en_bit,				\
	.vout_reg	= _vout_reg,				\
	.vout_mask	= _vout_mask,				\
	.vout_shift	= _vout_shift,				\
	.min_uv		= _min_mv * 1000,			\
	.max_uv		= _max_mv * 1000,			\
	.step_uv	= _step_mv * 100,				\
	.nsteps_reg	= _nsteps_reg,				\
	.nsteps_mask	= _nsteps_mask,				\
	.nsteps_uv		= _nsteps,				\
	.delay		= _delay,				\
	.id		= IP6203_ID_##_id,			\
	.desc = {						\
		.name = ip6203_rails(_id),			\
		.id = IP6203_ID_##_id,			\
		.n_voltages = _nsteps,				\
		.ops = &_ops,					\
		.type = REGULATOR_VOLTAGE,			\
		.owner = THIS_MODULE,				\
	},							\
}

static struct ip6203_regulator ip6203_regulator[] = {
	IP6203_REG(dc2, IP6203_DC_CTL, 2, IP6203_DC2_VSET, 0xFF, 0, 600, 2600, 125,
			0x0, 0x0, 0xA0, ip6203_ops, 500),
	IP6203_REG(dc4, IP6203_DC_CTL, 4, IP6203_DC4_VSET, 0xFF, 0, 600, 2600, 125,
			0x0, 0x0, 0xA0, ip6203_ops, 500),
	IP6203_REG(dc5, IP6203_DC_CTL, 5, IP6203_DC5_VSET, 0xFF, 0, 600, 3400, 125,
			0x0, 0x0, 0xE0, ip6203_ops, 500),
#ifdef IP6203_LDO1 
	IP6203_REG(ldo1, IP6203_SLDO1_CTL, 0, IP6203_SLDO1_CTL, 0x1F0, 4, 1000, 3300, 1000,
			0, 0, 0x17, ip6203_ops, 500),
#endif
	IP6203_REG(ldo6, IP6203_LDO_EN, 6, IP6203_LDO6_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),
	IP6203_REG(ldo7, IP6203_LDO_EN, 7, IP6203_LDO7_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),
	IP6203_REG(ldo8, IP6203_LDO_EN, 8, IP6203_LDO8_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),
	IP6203_REG(ldo9, IP6203_LDO_EN, 9, IP6203_LDO9_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),
	IP6203_REG(ldo11, IP6203_LDO_EN, 11, IP6203_LDO11_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),
	IP6203_REG(ldo12, IP6203_LDO_EN, 12, IP6203_LDO12_VSET, 0xFF, 0, 700, 3300, 250,
			0, 0, 0x68, ip6203_ops, 500),

};

static inline struct ip6203_regulator *find_regulator_info(int id)
{
	struct ip6203_regulator *ri;
	int i;

	for (i = 0; i < ARRAY_SIZE(ip6203_regulator); i++) {
		ri = &ip6203_regulator[i];
		if (ri->desc.id == id)
			return ri;
	}
	return NULL;
}

//static int ip6203_regulator_preinit(struct device *parent,
//		struct ip6203_regulator *ri,
//		struct IP6203618_regulator_platform_data *ip6203_pdata)
//{
//	int ret = 0;
//
//	if (!ip6203_pdata->init_apply)
//		return 0;
//
//	if (ip6203_pdata->init_uv >= 0) {
//		ret = __ip6203_set_voltage(parent, ri,
//				ip6203_pdata->init_uv,
//				ip6203_pdata->init_uv, 0);
//		if (ret < 0) {
//			dev_err(ri->dev,
//				"fail to initialize voltage %d for rail %d, err %d\n",
//				ip6203_pdata->init_uv, ri->desc.id, ret);
//			return ret;
//		}
//	}
//
//	if (ip6203_pdata->init_enable)
//		ret = ip6203_set_bits(parent, ri->reg_en_reg,
//						(1 << ri->en_bit));
//	else
//		ret = ip6203_clr_bits(parent, ri->reg_en_reg,
//						(1 << ri->en_bit));
//	if (ret < 0)
//		dev_err(ri->dev, "Not able to %s rail %d err %d\n",
//			(ip6203_pdata->init_enable) ? "enable" : "disable",
//			ri->desc.id, ret);
//
//	return ret;
//}

static inline int ip6203_cache_regulator_register(struct device *parent,
	struct ip6203_regulator *ri)
{
	ri->vout_reg_cache = 0;
	return ip6203_read(parent, ri->vout_reg, &ri->vout_reg_cache);
}

static int ip6203_regulator_probe(struct platform_device *pdev)
{
	struct ip6203_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct ip6203_regulator_platform_data *tps_pdata;
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
	ip6203_suspend_status = 0;

	err = ip6203_cache_regulator_register(pdev->dev.parent, ri);
	if (err) {
		dev_err(&pdev->dev, "Fail in caching register\n");
		return err;
	}

//	err = ip6203_regulator_preinit(pdev->dev.parent, ri, tps_pdata);
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

static int ip6203_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);
	return 0;
}

#ifdef CONFIG_PM
static int ip6203_regulator_suspend(struct device *dev)
{
//	struct ip6203_regulator *info = dev_get_drvdata(dev);

	IP6203_DBG("PMU: %s\n", __func__);
	ip6203_suspend_status = 1;

	return 0;
}

static int ip6203_regulator_resume(struct device *dev)
{
//	struct ip6203_regulator *info = dev_get_drvdata(dev);

	IP6203_DBG("PMU: %s\n", __func__);
	ip6203_suspend_status = 0;

	return 0;
}

static const struct dev_pm_ops ip6203_regulator_pm_ops = {
	.suspend	= ip6203_regulator_suspend,
	.resume		= ip6203_regulator_resume,
};
#endif

static struct platform_driver ip6203_regulator_driver = {
	.driver	= {
		.name	= "ip6203-regulator",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ip6203_regulator_pm_ops,
#endif
	},
	.probe		= ip6203_regulator_probe,
	.remove		= ip6203_regulator_remove,
};

static int __init ip6203_regulator_init(void)
{
	IP6203_INFO("PMU: %s\n", __func__);
	return platform_driver_register(&ip6203_regulator_driver);
}
subsys_initcall(ip6203_regulator_init);

static void __exit ip6203_regulator_exit(void)
{
	platform_driver_unregister(&ip6203_regulator_driver);
}
module_exit(ip6203_regulator_exit);

MODULE_DESCRIPTION("IP6203 R5T618 regulator driver");
MODULE_ALIAS("platform:IP6203618-regulator");
MODULE_LICENSE("GPL");
