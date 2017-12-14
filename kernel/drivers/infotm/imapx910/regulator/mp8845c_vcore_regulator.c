/*
 * mp8845c-regulator.c
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <mach/mp8845c.h>

static uint32_t mp8845c_uv_sel[] = {
	600000, 606700, 613400, 620100, 626800, 633500, 640100, 646800,
	653500, 660200, 666900, 673600, 680300, 687000, 693700, 700400,
	707000, 713700, 720400, 727100, 733800, 740500, 747200, 753900,
	760600, 767300, 773900, 780600, 787300, 794000, 800700, 807400,
	814100, 820800, 827500, 834200, 840800, 847500, 854200, 860900,
	867600, 874300, 881000, 887700, 894400, 901100, 907700, 914400,
	921100, 927800, 934500, 941200, 947900, 954600, 961300, 968000,
	974600, 981300, 988000, 994700, 1001400, 1008100, 1014800, 1021500,
	1028200, 1034900, 1041500, 1048200, 1054900, 1061600, 1068300, 1075000,
	1081700, 1088400, 1095100, 1101800, 1108400, 1115100, 1121800, 1128500,
	1135200, 1141900, 1148600, 1155300, 1162000, 1168700, 1175300, 1182000,
	1188700, 1195400, 1202100, 1208800, 1215500, 1222200, 1228900, 1235600,
	1242200, 1248900, 1255600, 1262300, 1269000, 1275700, 1282400, 1289100,
	1295800, 1302500, 1309100, 1315800, 1322500, 1329200, 1335900, 1342600,
	1349300, 1356000, 1362700, 1368400, 1376000, 1382700, 1389400, 1396100,
	1402800, 1409500, 1416200, 1422900, 1429600, 1436300, 1442900, 1449600,
};

static struct device *to_mp8845c_dev(struct regulator_dev *rdev)
{
	return rdev_get_dev(rdev)->parent->parent;
}

static int mp8845c_set_voltage(struct regulator_dev *rdev,
				int min_uV, int max_uV,
				unsigned *selector)
{
	int ret;
	uint8_t vsel;
	struct mp8845c_regulator *mp = rdev_get_drvdata(rdev);
	struct device *dev = to_mp8845c_dev(rdev);

	if ((min_uV < mp->min_uv) || (max_uV > mp->max_uv))
		return -EINVAL;

	vsel = (min_uV - mp8845c_uv_sel[0] + mp->step_uv - 1) / mp->step_uv;
	if (min_uV > mp8845c_uv_sel[vsel] && min_uV < mp8845c_uv_sel[vsel+1])
		vsel++;

	ret = mp8845c_core_write(dev, mp->vout_reg, (vsel|0x80));
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in writting the VOUT register\n");
		return ret;
	}

	return 0;
}

static int mp8845c_get_voltage(struct regulator_dev *rdev)
{
	int ret;
	uint8_t tmp;
	struct mp8845c_regulator *mp = rdev_get_drvdata(rdev);
	struct device *dev = to_mp8845c_dev(rdev);

	ret = mp8845c_core_read(dev, mp->vout_reg, &tmp);
	if (ret < 0) {
		dev_err(&rdev->dev, "Error in reading the VOUT register\n");
		return ret;
	}

	return mp8845c_uv_sel[tmp&0x7f];
}

static struct regulator_ops mp8845c_ops = {
	.set_voltage	= mp8845c_set_voltage,
	.get_voltage	= mp8845c_get_voltage,
};

#define MP8845C_REGULATOR(_id, _vout_reg, _min_uv, _max_uv, _step_uv,	\
			_nsteps, _ops)					\
{									\
	.id		= MP8845C_ID_##_id,				\
	.vout_reg	= _vout_reg,					\
	.min_uv		= _min_uv,					\
	.max_uv		= _max_uv,					\
	.step_uv	= _step_uv,					\
	.nsteps		= _nsteps,					\
	.desc = {							\
		.name	= "MP8845C_"#_id,				\
		.id	= MP8845C_ID_##_id,				\
		.n_voltages	= _nsteps,				\
		.ops	= &_ops,					\
		.type	= REGULATOR_VOLTAGE,				\
		.owner	= THIS_MODULE,					\
	},								\
}

static struct mp8845c_regulator mp8845c_regulator[] = {
	MP8845C_REGULATOR(FOR_VCORE, 0x00, 600000, 1449600, 6700, 128, mp8845c_ops),
};

static struct mp8845c_regulator *find_regulator_info(int id)
{
	struct mp8845c_regulator *mp;
	int i;

	for (i = 0; i < ARRAY_SIZE(mp8845c_regulator); i++) {
		mp = &mp8845c_regulator[i];
		if (mp->desc.id == id)
			return mp;
	}

	return NULL;
}

static int mp8845c_regulator_probe(struct platform_device *pdev)
{
	struct mp8845c_regulator *mp = NULL;
	struct regulator_dev *rdev;
	struct mp8845c_regulator_platform_data *pdata;
	struct regulator_config config = { };

	mp = find_regulator_info(pdev->id);
	if (mp == NULL) {
		pr_err("invalid regulator ID specified\n");
		return -EINVAL;
	}

	pdata = pdev->dev.platform_data;
	mp->dev = &pdev->dev;

	config.dev = &pdev->dev;
	config.init_data = &pdata->regulator;
	config.driver_data = mp;

	rdev = regulator_register(&mp->desc, &config);
	if (IS_ERR_OR_NULL(rdev)) {
		pr_err("failed to register regulator %s\n", mp->desc.name);
		return PTR_ERR(rdev);
	}

	platform_set_drvdata(pdev, rdev);

	return 0;
}

static int mp8845c_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	regulator_unregister(rdev);

	return 0;
}

static struct platform_driver mp8845c_regulator_driver = {
	.probe	= mp8845c_regulator_probe,
	.remove	= mp8845c_regulator_remove,
	.driver	= {
		.name	= "mp8845c_core_regulator",
		.owner	= THIS_MODULE,
	},
};


static int __init mp8845c_regulator_init(void)
{
	return platform_driver_register(&mp8845c_regulator_driver);
}
module_init(mp8845c_regulator_init);

static void __exit mp8845c_regulator_exit(void)
{
	platform_driver_unregister(&mp8845c_regulator_driver);
}
module_exit(mp8845c_regulator_exit);

MODULE_DESCRIPTION("MP8845C regulator driver for core");
MODULE_LICENSE("GPL");
