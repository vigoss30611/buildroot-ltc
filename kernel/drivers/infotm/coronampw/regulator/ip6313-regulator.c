#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/ip6313.h>

struct ip6313_regulator {
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

struct pmu_cfg_ip6313 {
	char name[64]; 
	char ctrl_bus[64];
	int chan;
	int drvbus; 
	int extraid;
	int acboot;
	int batt_cap;
	int batt_rdc;
};

static int ip6313_suspend_status;

static struct device *to_ip6313_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int ip6313_reg_is_enabled(struct regulator_dev *rdev)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6313_dev(rdev);
	uint8_t val;
	int ret;

	ret = ip6313_read(parent, ri->en_reg, &val);
	if (ret < 0) {
		dev_err(&rdev->dev, "failed reading control register\n");
		return ret;
	}

	return (val & (1 << ri->en_bit)) ? 1: 0;
}

static int ip6313_reg_enable(struct regulator_dev *rdev)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6313_dev(rdev);
	int ret;

	ret = ip6313_set_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed setting control register\n");
		goto exit;
	}

	udelay(ri->delay);
exit:
	return ret;
}

static int ip6313_reg_disable(struct regulator_dev *rdev)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6313_dev(rdev);
	int ret;

	ret = ip6313_clr_bits(parent, ri->en_reg, (1 << ri->en_bit));
	if (ret < 0) {
		dev_err(&rdev->dev, "failed clearing control register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip6313_list_voltage(struct regulator_dev *rdev, unsigned index)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);

	return ri->min_uv + (index * ri->step_uv);
}

static int __ip6313_set_voltage(struct device *parent,
				struct ip6313_regulator *ri,
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

	ret = ip6313_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vout &= ~(ri->vout_mask << ri->vout_shift);
	vout |= (vsel & ri->vout_mask) << ri->vout_shift;

	ret = ip6313_write(parent, ri->vout_reg, vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in writing the Voltage register\n");
		goto exit;
	}

exit:
	return ret;
}

static int ip6313_set_voltage(struct regulator_dev *rdev,
				int min_uv, int max_uv, unsigned *selector)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6313_dev(rdev);

	if (ip6313_suspend_status)
		return -EBUSY;

	return __ip6313_set_voltage(parent, ri, min_uv, max_uv, selector);
}

static int ip6313_get_voltage(struct regulator_dev *rdev)
{
	struct ip6313_regulator *ri = rdev_get_drvdata(rdev);
	struct device *parent = to_ip6313_dev(rdev);
	uint8_t vout;
	uint8_t vsel;
	int ret;

	ret = ip6313_read(parent, ri->vout_reg, &vout);
	if (ret < 0) {
		dev_err(ri->dev, "Error in reading the Voltage register\n");
		goto exit;
	}

	vsel = (vout & (ri->vout_mask << ri->vout_shift)) >> ri->vout_shift;

	return ri->min_uv + vsel * ri->step_uv;

exit:
	return ret;
}

static struct regulator_ops ip6313_ops = {
	.list_voltage	= ip6313_list_voltage,
	.set_voltage	= ip6313_set_voltage,
	.get_voltage	= ip6313_get_voltage,
	.enable		= ip6313_reg_enable,
	.disable	= ip6313_reg_disable,
	.is_enabled	= ip6313_reg_is_enabled,
};

#define regulator_alias(x)	#x

#define IP6313_REGU(ip_name_, _id, _en_reg, _en_bit, _vout_reg, _vout_mask,	\
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

static struct ip6313_regulator ip6313_regulators[] = {
	IP6313_REGU(IP6313, dc1, IP6313_DC_CTL, 0, IP6313_DC1_VSET, 0xff, 0, \
				600000, 3600000, 12500, 241, ip6313_ops, 500),
	IP6313_REGU(IP6313, dc2, IP6313_DC_CTL, 1, IP6313_DC2_VSET, 0xff, 0, \
				600000, 3600000, 12500, 241, ip6313_ops, 500),
	IP6313_REGU(IP6313, sldo1, IP6313_LDO_EN, 1, IP6313_SLDO1_2_VSET, 0x1f, 3, \
				700000, 3800000, 100000, 32, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo2, IP6313_LDO_EN, 2, IP6313_LDO2_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo3, IP6313_LDO_EN, 3, IP6313_LDO3_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo4, IP6313_LDO_EN, 4, IP6313_LDO4_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo5, IP6313_LDO_EN, 5, IP6313_LDO5_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo6, IP6313_LDO_EN, 6, IP6313_LDO6_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
	IP6313_REGU(IP6313, ldo7, IP6313_LDO_EN, 7, IP6313_LDO7_VSET, 0x7f, 0, \
				700000, 3400000, 25000, 109, ip6313_ops, 500),
};

static struct ip6313_regulator *find_regulator_info(int id, 
				struct platform_device *pdev)
{
	struct device *paltform_parent;
	struct ip6313_platform_data *pdata;
	struct pmu_cfg_ip6313 *get_ip6313_item_name;
	struct ip6313_regulator *ri;
	int i;

	paltform_parent = pdev->dev.parent;
	pdata = paltform_parent->platform_data;
	get_ip6313_item_name = pdata->ip;
	if(!strcmp(get_ip6313_item_name->name, "ip6313")){
		for (i = 0; i < ARRAY_SIZE(ip6313_regulators); i++) {
			ri = &ip6313_regulators[i];
			if (ri->desc.id == id)
				return ri;
		}
	}

	return NULL;
}

static int ip6313_regulator_probe(struct platform_device *pdev)
{
	struct ip6313_regulator *ri = NULL;
	struct regulator_dev *rdev;
	struct ip6313_regulator_platform_data *pdata;
	struct regulator_config config = { };
	int id = pdev->id;

	ri = find_regulator_info(id, pdev);
	if (ri == NULL) {
		dev_err(&pdev->dev, "invalid regulator ID specified\n");
		return -EINVAL;
	}

	pdata = pdev->dev.platform_data;
	ri->dev = &pdev->dev;
	ip6313_suspend_status = 0;

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

static int ip6313_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);

	return 0;
}

#ifdef CONFIG_PM
static int ip6313_regulator_suspend(struct device *dev)
{
	ip6313_suspend_status = 1;
	return 0;
}

static int ip6313_regulator_resume(struct device *dev)
{
	ip6313_suspend_status = 0;
	return 0;
}

static const struct dev_pm_ops ip6313_regulator_pm_ops = {
	.suspend        = ip6313_regulator_suspend,
	.resume         = ip6313_regulator_resume,
};
#endif

static struct platform_driver ip6313_regulator_driver = {
	.driver	= {
		.name	= "ip6313-regulator",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ip6313_regulator_pm_ops,
#endif
	},
	.probe		= ip6313_regulator_probe,
	.remove		= ip6313_regulator_remove,
};

static int __init ip6313_regulator_init(void)
{
	return platform_driver_register(&ip6313_regulator_driver);
}
subsys_initcall(ip6313_regulator_init);

static void __exit ip6313_regulator_exit(void)
{
	platform_driver_unregister(&ip6313_regulator_driver);
}
module_exit(ip6313_regulator_exit);

MODULE_DESCRIPTION("IP6313 regulator driver");
MODULE_LICENSE("Dual BSD/GPL");
