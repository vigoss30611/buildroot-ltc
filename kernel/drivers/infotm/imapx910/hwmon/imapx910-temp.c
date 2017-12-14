/*
 *  Thermistor -- hardware monitor driver for imapx9
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
#include <linux/mutex.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <mach/hw_cfg.h>


static int r_d = 12000; /*value of divider resistance is 4.7K*/
extern u32 get_lmic_voltage(void);

struct tmr_compensation {
	int temp_c;
	unsigned int ohm;
};

static const struct tmr_compensation SDNT1608X103J3950HTF[] = {
	{ .temp_c   = -40, .ohm = 364600 },
	{ .temp_c   = -35, .ohm = 259800 },
	{ .temp_c   = -30, .ohm = 187440 },
	{ .temp_c   = -25, .ohm = 297060 },
	{ .temp_c   = -20, .ohm = 136830 },
	{ .temp_c   = -15, .ohm = 75370 },
	{ .temp_c   = -10, .ohm = 56800 },
	{ .temp_c   = -5, .ohm = 43220 },
	{ .temp_c   = 0, .ohm = 33180 },
	{ .temp_c   = 5, .ohm = 25700 },
	{ .temp_c   = 10, .ohm = 20070 },
	{ .temp_c   = 15, .ohm = 15790 },
	{ .temp_c   = 20, .ohm = 12520 },
	{ .temp_c   = 25, .ohm  = 10000 },
	{ .temp_c   = 30, .ohm  = 8040 },
	{ .temp_c   = 35, .ohm  = 6510 },
	{ .temp_c   = 40, .ohm  = 5300 },
	{ .temp_c   = 45, .ohm  = 4340 },
	{ .temp_c   = 50, .ohm  = 3580 },
	{ .temp_c   = 55, .ohm  = 2960 },
	{ .temp_c   = 60, .ohm  = 2470 },
	{ .temp_c   = 62, .ohm  = 2300 },
	{ .temp_c   = 65, .ohm  = 2070 },
	{ .temp_c   = 68, .ohm  = 1860 },
	{ .temp_c   = 70, .ohm  = 1740 },
	{ .temp_c   = 72, .ohm  = 1620 },
	{ .temp_c   = 75, .ohm  = 1470 },
	{ .temp_c   = 77, .ohm  = 1370 },
	{ .temp_c   = 79, .ohm  = 1290 },
	{ .temp_c   = 80, .ohm  = 1250 },
	//{ .temp_c   = 81, .ohm  = 1210 },
	{ .temp_c   = 82, .ohm  = 1170 },
	//{ .temp_c   = 83, .ohm  = 1130 },
//	{ .temp_c   = 84, .ohm  = 1110 },
	{ .temp_c   = 85, .ohm  = 1060 },
//	{ .temp_c   = 86, .ohm  = 1030 },
//	{ .temp_c   = 87, .ohm  = 1000 },
	{ .temp_c   = 88, .ohm  = 970 },
//	{ .temp_c   = 89, .ohm  = 940 },
	{ .temp_c   = 90, .ohm  = 910 },
	{ .temp_c   = 92, .ohm  = 850 },
	{ .temp_c   = 95, .ohm  = 780 },
	{ .temp_c   = 98, .ohm  = 710 },
	{ .temp_c   = 100, .ohm = 670 },
	{ .temp_c   = 101, .ohm = 650 },
	{ .temp_c   = 102, .ohm = 640 },
	{ .temp_c   = 105, .ohm = 580 },
	{ .temp_c   = 108, .ohm = 540 },
	{ .temp_c   = 110, .ohm = 510 },
	{ .temp_c   = 113, .ohm = 470 },
	{ .temp_c   = 115, .ohm = 440 },
	{ .temp_c   = 118, .ohm = 410 },
	{ .temp_c   = 120, .ohm = 390 },
	{ .temp_c   = 123, .ohm = 360 },
	{ .temp_c   = 125, .ohm = 340 },
};

/* client data */
struct imapx9_tmr_data {
	struct device *hwmon_dev;
	const struct tmr_compensation *comp;
	int n_comp;
	const char *name;
};

enum {
	SHOW_CPUTEMP,
	SHOW_GPUTEMP,
	SHOW_NAME
};

static void lookup_comp(struct imapx9_tmr_data *data, unsigned int ohm,
		int *i_low, int *i_high)
{
	int start, end, mid;
	/*
	 * Handle special cases: Resistance is higher than or equal to     
	 * resistance in first table entry, or resistance is lower or equal
	 * to resistance in last table entry.                              
	 * In these cases, return i_low == i_high, either pointing to the  
	 * beginning or to the end of the table depending on the condition.
	 */
	if (ohm >= data->comp[0].ohm) {
		*i_low = 0;
		*i_high = 0;
		return;
	}
	if (ohm <= data->comp[data->n_comp - 1].ohm) {
		*i_low = data->n_comp - 1;
		*i_high = data->n_comp - 1;
		return;
	}
	/* Do a binary search on compensation table */
	start = 0;
	end = data->n_comp;
	while (start < end) {
		mid = start + (end - start) / 2;
		/*                                                              
		 * start <= mid < end                                           
		 * data->comp[start].ohm > ohm >= data->comp[end].ohm           
		 *                                                              
		 * We could check for "ohm == data->comp[mid].ohm" here, but    
		 * that is a quite unlikely condition, and we would have to     
		 * check again after updating start. Check it at the end instead
		 * for simplicity.                                              
		 */                                                             
		if (ohm >= data->comp[mid].ohm)
			end = mid;
		else {
			start = mid + 1;
			if (ohm >= data->comp[start].ohm)
				end = start;
		}

		/*                                                     
		 * start <= end                                        
		 * data->comp[start].ohm >= ohm >= data->comp[end].ohm 
		 */                                                    
	}

	/*                           
	 * start == end              
	 * ohm >= data->comp[end].ohm
	 */                          
	*i_low = end;
	if (ohm == data->comp[end].ohm)
		*i_high = end;
	else
		*i_high = end - 1;
}

static unsigned int read_ohm_from_board(int mv)
{
	/* 3300mv--R(1)-|(mv)--R(2)---G */
	return (3300 - mv) * r_d / mv;
}

static int get_temp_by_ohm(struct imapx9_tmr_data *data, unsigned int ohm)
{
	int low, high;                                                 
	int temp;                                                      
	                                                               
	lookup_comp(data, ohm, &low, &high);                           
	if (low == high) {                                             
		/* Unable to use linear approximation */                   
		temp = data->comp[low].temp_c * 1000;                      
	} else {                                                       
		temp = data->comp[low].temp_c * 1000 +                     
			((data->comp[high].temp_c - data->comp[low].temp_c) *  
			 1000 * ((int)ohm - (int)data->comp[low].ohm)) /       
			((int)data->comp[high].ohm - (int)data->comp[low].ohm);
	}                                                              
	return temp;                                                   
}

static ssize_t show_name(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(devattr);
	if (attr->index == SHOW_NAME)
		return sprintf(buf, "%s\n", "imapx9-thermitor");
	else
		return 0;
}

static ssize_t show_cpu_temp(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct imapx9_tmr_data *data = dev_get_drvdata(dev);
	int read_mv;
	unsigned int ohm; 
	//read_mv = read_mv_from_board();
	read_mv = get_lmic_voltage();
	//pr_err("get voltage %dmv!!\n");
	ohm = read_ohm_from_board(read_mv);
	return sprintf(buf, "%d\n", get_temp_by_ohm(data, ohm));
}

static ssize_t show_gpu_temp(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	int gpu_temp = 70;
	return sprintf(buf, "%d", gpu_temp);
}

static SENSOR_DEVICE_ATTR(name, S_IRUGO, show_name, NULL, SHOW_NAME);
static SENSOR_DEVICE_ATTR(cpu_temp, S_IRUGO, show_cpu_temp, NULL, SHOW_CPUTEMP);
static SENSOR_DEVICE_ATTR(gpu_temp, S_IRUGO, show_gpu_temp, NULL, SHOW_GPUTEMP);

static struct attribute *imapx9_temp_attributes[] = {
	&sensor_dev_attr_name.dev_attr.attr,
	&sensor_dev_attr_cpu_temp.dev_attr.attr,
	&sensor_dev_attr_gpu_temp.dev_attr.attr,
	NULL
};

static const struct attribute_group imapx9_temp_group = {
	.attrs = imapx9_temp_attributes,
};

static int imapx9_tmr_probe(struct platform_device *pdev)
{
	struct imapx9_tmr_data *data;
	struct tempmon_cfg *pdata = pdev->dev.platform_data;
	int err;

	if(strcmp(pdata->model, "SNDT1608X"))
		return -ENOMEM;

	pr_err("Temperature monitor SNDT1608X driver init!\n");

	data = devm_kzalloc(&pdev->dev, sizeof(struct imapx9_tmr_data),
			GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	data->name = "imapx9-thermitor";
	data->comp = SDNT1608X103J3950HTF; 
	data->n_comp = ARRAY_SIZE(SDNT1608X103J3950HTF);

	platform_set_drvdata(pdev, data);
	err = sysfs_create_group(&pdev->dev.kobj, &imapx9_temp_group);
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
	sysfs_remove_group(&pdev->dev.kobj, &imapx9_temp_group);
	return err;
}

static int imapx9_tmr_remove(struct platform_device *pdev)
{
	struct imapx9_tmr_data *data = platform_get_drvdata(pdev);
	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_group(&pdev->dev.kobj, &imapx9_temp_group);
	return 0;
}


static struct platform_device_id imapx9_tmr_ids[] = {
	{"imapx9-thermitor", 0xffff},
};

MODULE_DEVICE_TABLE(platform, imapx9_tmr_ids);

static struct platform_driver imapx9_tmr_driver = {
	.driver = {
		.name = "imapx9-thermitor",
		.owner = THIS_MODULE,
	},
	.probe =  imapx9_tmr_probe, 
	.remove = imapx9_tmr_remove,
	.id_table = imapx9_tmr_ids,
};

module_platform_driver(imapx9_tmr_driver);

MODULE_AUTHOR("Peter.Fu");
MODULE_DESCRIPTION("TempCPU driver");
MODULE_LICENSE("GPL");

