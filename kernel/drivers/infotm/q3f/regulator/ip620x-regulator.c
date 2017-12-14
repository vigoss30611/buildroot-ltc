
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/ip6208.h>
#include <mach/ip6205.h>

struct ip620x_regulator {
	int			id;
	/* Register */
	uint8_t			en_reg;
	int			en_bit;

	uint8_t			vout_reg;
	int			vout_mask;
	int			vout_shift;
	/* Constraints */
	uint32_t		min_uv;
	uint32_t		max_uv;
	uint32_t		step_uv;

	int                     nsteps_uv;

	uint16_t		delay;
	/* used by regulator core */
	struct regulator_desc	desc;

	struct device		*dev;
};

struct pmu_cfg_ip620x {
	char name[64]; 
	char ctrl_bus[64];
	int chan;
	int drvbus; 
	int extraid;
	int acboot;
	int batt_cap;
	int batt_rdc;
};

static int ip620x_suspend_status;

static struct device *to_ip620x_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int ip620x_reg_is_enabled(struct regulator_dev *rdev)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip620x_dev(rdev);
	uint16_t val;
	int ret;

	ret = ip620x_read(parent, ri->en_reg, &val);
	if (ret < 0) {
		dev_err(&rdev->dev, "failed reading control register\n");
		return ret;
	}

	return (val & (1 << ri->en_bit)) ? 1: 0;
}

static int ip620x_reg_enable(struct regulator_dev *rdev)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip620x_dev(rdev);
	int ret;

	ret = ip620x_set_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed setting control register\n");
		goto exit;
	}
	udelay(ri->delay);
exit:
	return ret;
}

static int ip620x_reg_disable(struct regulator_dev *rdev)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip620x_dev(rdev);
	int ret;

	ret = ip620x_clr_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed clearing control register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip620x_list_voltage(struct regulator_dev *rdev, unsigned index)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);

	return ri->min_uv + (index * ri->step_uv);
}

static int __ip620x_set_voltage(struct device *parent,
				struct ip620x_regulator *ri,
				int min_uv, int max_uv, unsigned *selector)
{
	int vsel;
	int ret;
	uint16_t vout;

	if ((min_uv < ri->min_uv) || (max_uv > ri->max_uv))
		return -EDOM;

	vsel = (min_uv - ri->min_uv) / ri->step_uv;

	if (selector)
		*selector = vsel;

	ret = ip620x_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vout &= ~(ri->vout_mask << ri->vout_shift);
	vout |= (vsel & ri->vout_mask) << ri->vout_shift;

	ret = ip620x_write(parent, ri->vout_reg, vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in writing the Voltage register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip620x_set_voltage(struct regulator_dev *rdev,
				int min_uv, int max_uv, unsigned *selector)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip620x_dev(rdev);

	if (ip620x_suspend_status)
		return -EBUSY;

	return __ip620x_set_voltage(parent, ri, min_uv, max_uv, selector);
}

static int ip620x_get_voltage(struct regulator_dev *rdev)
{
	struct ip620x_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip620x_dev(rdev);
	uint16_t vout;
	uint8_t vsel;
	int ret;

	ret = ip620x_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vsel = (vout & (ri->vout_mask << ri->vout_shift)) >> ri->vout_shift;

	return ri->min_uv + vsel * ri->step_uv;

exit:
	return ret;
}

static struct regulator_ops ip620x_ops = {
	.list_voltage	= ip620x_list_voltage,
	.set_voltage	= ip620x_set_voltage,
	.get_voltage	= ip620x_get_voltage,
	.enable		= ip620x_reg_enable,
	.disable	= ip620x_reg_disable,
	.is_enabled	= ip620x_reg_is_enabled,
};

#define regulator_alias(x)	#x

#define IP620x_REGU(ip_name_, _id, _en_reg, _en_bit, _vout_reg, _vout_mask,	\
		_vout_shift, _min_uv, _max_uv, _step_uv, _nsteps, _ops, _delay)	\
{									\
	.en_reg		= _en_reg,					\
	.en_bit		= _en_bit,					\
	.vout_reg	= _vout_reg,					\
	.vout_mask	= _vout_mask,					\
	.vout_shift	= _vout_shift,					\
	.min_uv		= _min_uv,					\
	.max_uv		= _max_uv,					\
	.step_uv	= _step_uv,					\
	.nsteps_uv	= _nsteps,					\
	.delay		= _delay,					\
	.id		= ip_name_##_ID_##_id,				\
	.desc = {							\
		.name = regulator_alias(_id),				\
		.id = ip_name_##_ID_##_id,					\
		.n_voltages = _nsteps,					\
		.ops = &_ops,						\
		.type = REGULATOR_VOLTAGE,				\
		.owner = THIS_MODULE,					\
	},								\
}

static struct ip620x_regulator ip6208_regulator[] = {
	IP620x_REGU(IP6208, dc1, IP6208_DC_CTL, 1, IP6208_DC1_VSET, 0xff, 0, \
				600000, 2600000, 12500, 161, ip620x_ops, 5),
	IP620x_REGU(IP6208, dc2, IP6208_DC_CTL, 2, IP6208_DC2_VSET, 0xff, 0, \
				600000, 2600000, 12500, 161, ip620x_ops, 5),
	IP620x_REGU(IP6208, dc3, IP6208_DC_CTL, 3, IP6208_DC3_VSET, 0xff, 0, \
				600000, 3400000, 12500, 225, ip620x_ops, 5),
	IP620x_REGU(IP6208, dc4, IP6208_DC_CTL, 4, IP6208_DC4_VSET, 0xff, 0, \
				600000, 3400000, 12500, 225, ip620x_ops, 5),
	IP620x_REGU(IP6208, dc5, IP6208_DC_CTL, 5, IP6208_DC5_VSET, 0xff, 0, \
				600000, 2600000, 12500, 161, ip620x_ops, 5),
	IP620x_REGU(IP6208, dc6, IP6208_DC_CTL, 6, IP6208_DC1_VSET, 0xff, 0, \
				600000, 3400000, 12500, 225, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo1, IP6208_SLDO1_CTL, 0, IP6208_SLDO1_CTL, 0x1f, 4, \
				700000, 3400000, 100000, 28, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo2, IP6208_LDO_EN, 2, IP6208_LDO2_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo3, IP6208_LDO_EN, 3, IP6208_LDO3_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo4, IP6208_LDO_EN, 4, IP6208_LDO4_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo5, IP6208_LDO_EN, 5, IP6208_LDO5_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo6, IP6208_LDO_EN, 6, IP6208_LDO6_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo7, IP6208_LDO_EN, 7, IP6208_LDO7_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo8, IP6208_LDO_EN, 8, IP6208_LDO8_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo9, IP6208_LDO_EN, 9, IP6208_LDO9_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo10, IP6208_LDO_EN, 10, IP6208_LDO10_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo11, IP6208_LDO_EN, 11, IP6208_LDO11_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6208, ldo12, IP6208_LDO_EN, 12, IP6208_LDO12_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
};

static struct ip620x_regulator ip6205_regulator[] = {
	IP620x_REGU(IP6205, dc1, IP6205_DC_CTL, 1, IP6205_DC1_VSET, 0xff, 0, \
				600000, 2600000, 12500, 161, ip620x_ops, 5),
	IP620x_REGU(IP6205, dc4, IP6205_DC_CTL, 4, IP6205_DC4_VSET, 0xff, 0, \
				600000, 3400000, 12500, 225, ip620x_ops, 5),
	IP620x_REGU(IP6205, dc5, IP6205_DC_CTL, 5, IP6205_DC5_VSET, 0xff, 0, \
				600000, 2600000, 12500, 161, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo2, IP6205_LDO_EN, 2, IP6205_LDO2_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo3, IP6205_LDO_EN, 3, IP6205_LDO3_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo4, IP6205_LDO_EN, 4, IP6205_LDO4_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo6, IP6205_LDO_EN, 6, IP6205_LDO6_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo7, IP6205_LDO_EN, 7, IP6205_LDO7_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo8, IP6205_LDO_EN, 8, IP6205_LDO8_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
	IP620x_REGU(IP6205, ldo9, IP6205_LDO_EN, 9, IP6205_LDO9_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip620x_ops, 5),
};

static struct ip620x_regulator *find_regulator_info(int id, 
				struct platform_device *pdev)
{
	struct device *paltform_parent;
	struct ip620x_platform_data *pdata;
	struct pmu_cfg_ip620x *get_ip620x_item_name;
	struct ip620x_regulator *ri;
	int i;

	paltform_parent = pdev->dev.parent;
	pdata = paltform_parent->platform_data;
	get_ip620x_item_name = pdata->ip;
	if(!strcmp(get_ip620x_item_name->name, "ip6208")){
		for (i = 0; i < ARRAY_SIZE(ip6208_regulator); i++) {
			ri = &ip6208_regulator[i];
			if (ri->desc.id == id)
				return ri;
		}
	}else if(!strcmp(get_ip620x_item_name->name, "ip6205")){
		for (i = 0; i < ARRAY_SIZE(ip6205_regulator); i++) {
			ri = &ip6205_regulator[i];
			if (ri->desc.id == id)
				return ri;
		}
	}

	return NULL;
}

static int ip620x_regulator_probe(struct platform_device *pdev)
{
	struct ip620x_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct ip620x_regulator_platform_data *pdata;
	struct regulator_config config = { };
	int id = pdev->id;

	ri = find_regulator_info(id, pdev);
	if (ri == NULL) {
		dev_err(&pdev->dev, "invalid regulator ID specified\n");
		return -EINVAL;
	}

	pdata = pdev->dev.platform_data;
	ri->dev = &pdev->dev;
	ip620x_suspend_status = 0;

	config.dev = &pdev->dev;
	config.init_data = &pdata->regulator;
	config.driver_data = ri;

	rdev = regulator_register(&ri->desc, &config);
	if (IS_ERR_OR_NULL(rdev)) {
		dev_err(&pdev->dev, "failed to register regulator %s\n",
				ri->desc.name);
		return PTR_ERR(rdev);
	}

	platform_set_drvdata(pdev, rdev);

	return 0;
}

static int ip620x_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);

	return 0;
}

#ifdef CONFIG_PM
static int ip620x_regulator_suspend(struct device *dev)
{
	ip620x_suspend_status = 1;
	return 0;
}

static int ip620x_regulator_resume(struct device *dev)
{
	ip620x_suspend_status = 0;
	return 0;
}

static const struct dev_pm_ops ip620x_regulator_pm_ops = {
	.suspend        = ip620x_regulator_suspend,
	.resume         = ip620x_regulator_resume,
};
#endif

static struct platform_driver ip620x_regulator_driver = {
	.driver	= {
		.name	= "ip620x-regulator",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ip620x_regulator_pm_ops,
#endif
	},
	.probe		= ip620x_regulator_probe,
	.remove		= ip620x_regulator_remove,
};

static int __init ip620x_regulator_init(void)
{
	return platform_driver_register(&ip620x_regulator_driver);
}
subsys_initcall(ip620x_regulator_init);

static void __exit ip620x_regulator_exit(void)
{
	platform_driver_unregister(&ip620x_regulator_driver);
}
module_exit(ip620x_regulator_exit);

MODULE_DESCRIPTION("IP6208 regulator driver");
MODULE_LICENSE("Dual BSD/GPL");

