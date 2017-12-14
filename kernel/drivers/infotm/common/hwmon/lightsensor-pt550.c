/*
 *  Lightsensor -- hardware monitor driver for imapx9
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/adc.h>
#include <linux/mutex.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <mach/hw_cfg.h>


/* client data */
struct ls_pt550_data {
	struct device *hwmon_dev;
	const struct tmr_compensation *comp;
	struct adc_device *adc;
	int n_comp;
	const char *name;
};

enum {
	SHOW_LUMENS,
	SHOW_ORIGN_VOL,
	SHOW_NAME
};


static ssize_t show_name(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct ls_pt550_data *data = dev_get_drvdata(dev);
	return sprintf(buf, "%s\n", data->name);
}

static ssize_t show_lumens(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct ls_pt550_data *data = dev_get_drvdata(dev);
	unsigned int val, val1; 
	int ret;

	adc_enable(data->adc);
	ret = adc_read_raw(data->adc, &val, &val1);
	adc_disable(data->adc);
	if(ret == ADC_VAL_INT)
		return sprintf(buf, "%d\n", val);
	else
		return sprintf(buf, "Invalid Value\n");
}

static ssize_t show_orign_voltage(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct ls_pt550_data *data = dev_get_drvdata(dev);
	unsigned int val, val1; 
	int ret;

	adc_enable(data->adc);
	ret = adc_read_raw(data->adc, &val, &val1);
	adc_disable(data->adc);
	if(ret == ADC_VAL_INT)
		return sprintf(buf, "%d\n", val);
	else
		return sprintf(buf, "Invalid Value\n");
		

}

static SENSOR_DEVICE_ATTR(name, S_IRUGO, show_name, NULL, SHOW_NAME);
static SENSOR_DEVICE_ATTR(lumen, S_IRUGO, show_lumens, NULL, SHOW_LUMENS);
static SENSOR_DEVICE_ATTR(voltage, S_IRUGO, show_orign_voltage, NULL, SHOW_ORIGN_VOL);

static struct attribute *ls_pt550_attributes[] = {
	&sensor_dev_attr_name.dev_attr.attr,
	&sensor_dev_attr_lumen.dev_attr.attr,
	&sensor_dev_attr_voltage.dev_attr.attr,
	NULL
};

static const struct attribute_group ls_pt550_group = {
	.attrs = ls_pt550_attributes,
};

static int ls_pt550_probe(struct platform_device *pdev)
{
	struct ls_pt550_data *data;
	struct lightsen_hw_cfg *pdata = pdev->dev.platform_data;
	struct adc_request adc_req;
	int err;

	if(strcmp(pdata->model, "pt550"))
		return -ENOMEM;

	if(strcmp(pdata->bus, "adc"))
		return -EINVAL;

	pr_info("[Light Sensor: %s] driver probe, request adc channel %d!\n", pdata->model, pdata->chan);

	data = devm_kzalloc(&pdev->dev, sizeof(struct ls_pt550_data),
			GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	adc_req.id = pdata->chan;
	adc_req.mode = ADC_DEV_SAMPLE;
	adc_req.label = pdata->model;
	adc_req.chip_name = "imap-adc";
	data->adc = adc_request(&adc_req);
	if(!data->adc) {
		pr_err("lightsensor can't request adc channel%d\n", adc_req.id);
		return -EINVAL;
	}
	data->name = pdata->model;

	platform_set_drvdata(pdev, data);
	err = sysfs_create_group(&pdev->dev.kobj, &ls_pt550_group);
	if(err)
		return err;
	data->hwmon_dev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		dev_err(&pdev->dev, "Class registration failed (%d)\n",
				err);
		goto exit_remove;
	}
	return 0;
exit_remove:
	sysfs_remove_group(&pdev->dev.kobj, &ls_pt550_group);
	return err;
}

static int ls_pt550_remove(struct platform_device *pdev)
{
	struct ls_pt550_data *data = platform_get_drvdata(pdev);
	adc_free(data->adc);
	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_group(&pdev->dev.kobj, &ls_pt550_group);
	return 0;
}


static struct platform_device_id ls_pt550_ids[] = {
	{"pt550", 0xffff},
};

MODULE_DEVICE_TABLE(platform, ls_pt550_ids);

static struct platform_driver ls_pt550_driver = {
	.driver = {
		.name = "pt550",
		.owner = THIS_MODULE,
	},
	.probe =  ls_pt550_probe, 
	.remove = ls_pt550_remove,
	.id_table = ls_pt550_ids,
};

module_platform_driver(ls_pt550_driver);

MODULE_AUTHOR("Infotm-PS");
MODULE_DESCRIPTION("LightSensor driver");
MODULE_LICENSE("GPL");

