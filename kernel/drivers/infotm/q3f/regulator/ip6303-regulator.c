#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/ip6303.h>

struct ip6303_regulator {
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

struct pmu_cfg_ip6303 {
	char name[64]; 
	char ctrl_bus[64];
	int chan;
	int drvbus; 
	int extraid;
	int acboot;
	int batt_cap;
	int batt_rdc;
};

static int ip6303_suspend_status;

static struct device *to_ip6303_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int ip6303_reg_is_enabled(struct regulator_dev *rdev)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6303_dev(rdev);
	uint8_t val;
	int ret;

	ret = ip6303_read(parent, ri->en_reg, &val);
	if (ret < 0) {
		dev_err(&rdev->dev, "failed reading control register\n");
		return ret;
	}

	return (val & (1 << ri->en_bit)) ? 1: 0;
}

static int ip6303_reg_enable(struct regulator_dev *rdev)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6303_dev(rdev);
	int ret;

	ret = ip6303_set_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed setting control register\n");
		goto exit;
	}

	udelay(ri->delay);
exit:
	return ret;
}

static int ip6303_reg_disable(struct regulator_dev *rdev)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6303_dev(rdev);
	int ret;

	ret = ip6303_clr_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed clearing control register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip6303_list_voltage(struct regulator_dev *rdev, unsigned index)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);

	return ri->min_uv + (index * ri->step_uv);
}

static int __ip6303_set_voltage(struct device *parent,
				struct ip6303_regulator *ri,
				int min_uv, int max_uv, unsigned *selector)
{
	int vsel;
	int ret;
	uint8_t vout;

	if ((min_uv < ri->min_uv) || (max_uv > ri->max_uv))
		return -EDOM;

	vsel = (min_uv - ri->min_uv) / ri->step_uv;

	if (selector)
		*selector = vsel;

	ret = ip6303_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vout &= ~(ri->vout_mask << ri->vout_shift);
	vout |= (vsel & ri->vout_mask) << ri->vout_shift;

	ret = ip6303_write(parent, ri->vout_reg, vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in writing the Voltage register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip6303_set_voltage(struct regulator_dev *rdev,
				int min_uv, int max_uv, unsigned *selector)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6303_dev(rdev);

	if (ip6303_suspend_status)
		return -EBUSY;

	return __ip6303_set_voltage(parent, ri, min_uv, max_uv, selector);
}

static int ip6303_get_voltage(struct regulator_dev *rdev)
{
	struct ip6303_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6303_dev(rdev);
	uint8_t vout;
	uint8_t vsel;
	int ret;

	ret = ip6303_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vsel = (vout & (ri->vout_mask << ri->vout_shift)) >> ri->vout_shift;

	return ri->min_uv + vsel * ri->step_uv;

exit:
	return ret;
}

static struct regulator_ops ip6303_ops = {
	.list_voltage	= ip6303_list_voltage,
	.set_voltage	= ip6303_set_voltage,
	.get_voltage	= ip6303_get_voltage,
	.enable		= ip6303_reg_enable,
	.disable	= ip6303_reg_disable,
	.is_enabled	= ip6303_reg_is_enabled,
};

#define regulator_alias(x)	#x

#define IP6303_REGU(ip_name_, _id, _en_reg, _en_bit, _vout_reg, _vout_mask,	\
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

static struct ip6303_regulator ip6303_regulators[] = {
	IP6303_REGU(IP6303, dc1, IP6303_DC_CTL, 0, IP6303_DC1_VSET, 0xff, 0, \
				600000, 3600000, 12500, 241, ip6303_ops, 500),
	IP6303_REGU(IP6303, dc2, IP6303_DC_CTL, 1, IP6303_DC2_VSET, 0xff, 0, \
				600000, 3600000, 12500, 241, ip6303_ops, 500),
	IP6303_REGU(IP6303, dc3, IP6303_DC_CTL, 2, IP6303_DC3_VSET, 0xff, 0, \
				600000, 3600000, 12500, 241, ip6303_ops, 500),
	IP6303_REGU(IP6303, sldo1, IP6303_LDO_EN, 1, IP6303_SLDO1_2_VSET, 0x1f, 3, \
				700000, 3800000, 100000, 32, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo2, IP6303_LDO_EN, 2, IP6303_LDO2_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo3, IP6303_LDO_EN, 3, IP6303_LDO3_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo4, IP6303_LDO_EN, 4, IP6303_LDO4_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo5, IP6303_LDO_EN, 5, IP6303_LDO5_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo6, IP6303_LDO_EN, 6, IP6303_LDO6_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
	IP6303_REGU(IP6303, ldo7, IP6303_LDO_EN, 7, IP6303_LDO7_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6303_ops, 500),
};

static struct ip6303_regulator *find_regulator_info(int id, 
				struct platform_device *pdev)
{
	struct device *paltform_parent;
	struct ip6303_platform_data *pdata;
	struct pmu_cfg_ip6303 *get_ip6303_item_name;
	struct ip6303_regulator *ri;
	int i;

	paltform_parent = pdev->dev.parent;
	pdata = paltform_parent->platform_data;
	get_ip6303_item_name = pdata->ip;
	if(!strcmp(get_ip6303_item_name->name, "ip6303")){
		for (i = 0; i < ARRAY_SIZE(ip6303_regulators); i++) {
			ri = &ip6303_regulators[i];
			if (ri->desc.id == id)
				return ri;
		}
	}

	return NULL;
}

static int ip6303_regulator_probe(struct platform_device *pdev)
{
	struct ip6303_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct ip6303_regulator_platform_data *pdata;
	struct regulator_config config = { };
	int id = pdev->id;

	ri = find_regulator_info(id, pdev);
	if (ri == NULL) {
		dev_err(&pdev->dev, "invalid regulator ID specified\n");
		return -EINVAL;
	}

	pdata = pdev->dev.platform_data;
	ri->dev = &pdev->dev;
	ip6303_suspend_status = 0;

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

static int ip6303_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);

	return 0;
}

#ifdef CONFIG_PM
static int ip6303_regulator_suspend(struct device *dev)
{
	ip6303_suspend_status = 1;
	return 0;
}

static int ip6303_regulator_resume(struct device *dev)
{
	ip6303_suspend_status = 0;
	return 0;
}

static const struct dev_pm_ops ip6303_regulator_pm_ops = {
	.suspend        = ip6303_regulator_suspend,
	.resume         = ip6303_regulator_resume,
};
#endif

static struct platform_driver ip6303_regulator_driver = {
	.driver	= {
		.name	= "ip6303-regulator",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ip6303_regulator_pm_ops,
#endif
	},
	.probe		= ip6303_regulator_probe,
	.remove		= ip6303_regulator_remove,
};

static int __init ip6303_regulator_init(void)
{
	return platform_driver_register(&ip6303_regulator_driver);
}
subsys_initcall(ip6303_regulator_init);

static void __exit ip6303_regulator_exit(void)
{
	platform_driver_unregister(&ip6303_regulator_driver);
}
module_exit(ip6303_regulator_exit);

MODULE_DESCRIPTION("ip6303 regulator driver");
MODULE_LICENSE("GPL");
