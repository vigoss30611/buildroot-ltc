#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/pad.h>
#include <mach/imap-pwm.h>

#define CPU_VOL_COUNT		(28)

extern void __iomem *ioaddr;
extern u32 pwm_readl(void __iomem * base, int offset);
extern int imap_timer_setup(int channel,unsigned long g_tcnt,unsigned long g_tcmp);

struct pwm_regulator {
	int	id;

	int     min_uv;
	int     max_uv;
	int     nsteps;

	struct device   *dev;
	struct regulator_desc   desc;
	struct regulator_ops    *ops;
};

static int pwm_vol_sel[] = {
	1033000, 1054000, 1075000, 1096000, 1116000, 1137000,
	1158000, 1179000, 1200000, 1221000, 1242000, 1262000,
	1283000, 1304000, 1325000, 1346000, 1367000, 1388000,
};

static int pwm_set_voltage(struct regulator_dev *rdev,
				int min_uV, int max_uV,
				unsigned *selector)
{
	int i;
	struct pwm_regulator *pwm = rdev_get_drvdata(rdev);

	if ((min_uV < pwm_vol_sel[0]) || (max_uV > pwm_vol_sel[pwm->nsteps-1]))
		return -EINVAL;

	for (i = 0; i < pwm->nsteps; i++) {
		if (min_uV <= pwm_vol_sel[i])
			break;
	}

	imap_timer_setup(pwm->desc.id, CPU_VOL_COUNT, pwm->nsteps-i-1);

	return 0;
}

static int pwm_get_voltage(struct regulator_dev *rdev)
{
	uint32_t tmp;
	struct pwm_regulator *pwm = rdev_get_drvdata(rdev);

	tmp = pwm_readl(ioaddr, IMAP_TCMPB(pwm->desc.id));
	return pwm_vol_sel[pwm->nsteps-tmp-1];
}

static struct regulator_ops pwm_ops = {
	.set_voltage = pwm_set_voltage,
	.get_voltage = pwm_get_voltage,
};

struct pwm_regulator pwm_regulator[] = {
	{
		.id	= 0,
		.min_uv	= 1033000,
		.max_uv	= 1388000,
		.nsteps	= 18,
		.desc	= {
			.name	= "pwm0",
			.id	= 0,
			.n_voltages	= 18,
			.ops	= &pwm_ops,
			.type	= REGULATOR_VOLTAGE,
			.owner	= THIS_MODULE,
		},
	},
	{
		.id	= 1,
		.min_uv	= 1033000,
		.max_uv	= 1388000,
		.nsteps	= 18,
		.desc	= {
			.name	= "pwm1",
			.id	= 1,
			.n_voltages	= 18,
			.ops	= &pwm_ops,
			.type	= REGULATOR_VOLTAGE,
			.owner	= THIS_MODULE,
		},
	},
	{
		.id	= 2,
		.min_uv	= 1033000,
		.max_uv	= 1388000,
		.nsteps	= 18,
		.desc	= {
			.name	= "pwm2",
			.id	= 2,
			.n_voltages	= 18,
			.ops	= &pwm_ops,
			.type	= REGULATOR_VOLTAGE,
			.owner	= THIS_MODULE,
		},
	},
};

static struct pwm_regulator *find_regulator_info(int id)
{
	struct pwm_regulator *pwm;
	int i;

	for (i = 0; i < ARRAY_SIZE(pwm_regulator); i++) {
		pwm = &pwm_regulator[i];
		if (pwm->desc.id == id)
			return pwm;
	}

	return NULL;
}

static int pwm_regulator_probe(struct platform_device *pdev)
{
	struct pwm_regulator *pwm;
	struct regulator_dev *rdev;
	struct regulator_init_data *pdata;
	struct regulator_config config = { };
	pr_err("%s: id=%d\n", __func__, pdev->id);

	pwm = find_regulator_info(pdev->id);
	if (pwm == NULL) {
		pr_err("invalid regulator ID specified\n");
		return -EINVAL;
	}

	pdata = pdev->dev.platform_data;
	pwm->dev = &pdev->dev;

	config.dev = &pdev->dev;
	config.init_data = pdata;
	config.driver_data = pwm;

	rdev = regulator_register(&pwm->desc, &config);
	if (IS_ERR_OR_NULL(rdev)) {
		pr_err("failed to register regulator %s\n", pwm->desc.name);
		return PTR_ERR(rdev);
	}
	platform_set_drvdata(pdev, rdev);

	imap_timer_setup(pwm->desc.id, CPU_VOL_COUNT, pwm->nsteps >> 1);
	imapx_pad_init(pwm->desc.name);

	return 0;
}

static int pwm_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);

	return 0;
}

static struct platform_driver pwm_regulator_driver = {
	.driver = {
		.name = "pwm-regulator",
		.owner = THIS_MODULE,
	},
	.probe = pwm_regulator_probe,
	.remove = pwm_regulator_remove,
};

static int __init pwm_regulator_init(void)
{
	return platform_driver_register(&pwm_regulator_driver);
}
module_init(pwm_regulator_init);

static void __exit pwm_regulator_exit(void)
{
	platform_driver_unregister(&pwm_regulator_driver);
}
module_exit(pwm_regulator_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Sun <david.sun@infotmic.com.cn>");
MODULE_DESCRIPTION("Regulator Driver for PWM");
