/* display hdmi driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>
#include <mach/items.h>

#include "display_register.h"
#include "display_items.h"

#define DISPLAY_MODULE_LOG		"hdmi"

static int default_vic = 4;

static int hdmi_enable(struct display_device *pdev, int en)
{
	pdev->enable = en;
	return 0;
}

static int hdmi_set_vic(struct display_device *pdev, int vic)
{
	pdev->vic = vic;
	return 0;
}

static int hdmi_probe(struct display_device *pdev)
{
	dlog_trace();

	pdev->peripheral_config.default_vic = default_vic;
	pdev->peripheral_config.rgb_order = DISPLAY_LCD_RGB_ORDER_RGB;
	pdev->peripheral_config.interface_type = DISPLAY_INTERFACE_TYPE_HDMI;
	pdev->interface_type = pdev->peripheral_config.interface_type;

	pdev->vic = default_vic;
	
	return 0;
}

static int hdmi_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}


static struct display_driver hdmi_driver = {
	.probe  = hdmi_probe,
	.remove = hdmi_remove,
	.driver = {
		.name = "hdmi",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_PERIPHERAL,
	.function = {
		.enable = hdmi_enable,
		.set_vic = hdmi_set_vic,
	}
};

static struct display_device hdmi_device = {
		.name = "hdmi",
		.id = -1,
		.path = 0,
		.fixed_resolution = 0,
};


static int __init hdmi_driver_init(void)
{
	int ret;
	int i, j;
	char name[64] = {0};
	char peripheral[64] = {0};
	
	ret = display_driver_register(&hdmi_driver);
	if (ret) {
		dlog_err("register hdmi driver failed %d\n", ret);
		return ret;
	}
	
	for (i = 0; i < IDS_PATH_NUM; i++) {
		for (j = 0; j < MAX_PATH_PERIPHERAL_NUM; j++) { 
			sprintf(name, DISPLAY_ITEM_PERIPHERAL_TYPE, i, j);
			if (!item_exist(name))
				break;
			item_string(peripheral, name, 0);
			if (sysfs_streq(peripheral, "hdmi")) {
				sprintf(name, DISPLAY_ITEM_PERIPHERAL_HDMI_DFVIC, i, peripheral);
				if (item_exist(name))
					default_vic = item_integer(name, 0);
				dlog_info("path%d: register peripheral%d device %s\n", i, j, peripheral);
				hdmi_device.path = i;
				display_device_register(&hdmi_device);
				goto out;
			}
		}
	}
	dlog_info("no hdmi device\n");

out:
	return 0;
}
module_init(hdmi_driver_init);

static void __exit hdmi_driver_exit(void)
{
	display_driver_unregister(&hdmi_driver);
}
module_exit(hdmi_driver_exit);

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display hdmi sink peripheral driver");
MODULE_LICENSE("GPL");
