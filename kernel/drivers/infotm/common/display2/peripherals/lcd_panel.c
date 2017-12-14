/* display lcd panel driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/display_device.h>
#include <mach/items.h>

#include "display_register.h"
#include "display_items.h"

#define DISPLAY_MODULE_LOG		"lcd_panel"


/* @_@ extern specific lcd panel parameter */
extern struct peripheral_param_config GM05004001Q_RGB_800_480;
extern struct peripheral_param_config ILI8961_SerialRGBDummy_360_240;
extern struct peripheral_param_config ILI8961_BT656NTSC_720_480;
extern struct peripheral_param_config ILI8961_BT656PAL_720_576;
extern struct peripheral_param_config ILI8961_BT601_720_480;
extern struct peripheral_param_config TFT3P3469_220_176;


/* ^_^ Add new lcd panel parameter here */
static struct peripheral_param_config * panel_configs[] = {
	&GM05004001Q_RGB_800_480,
	&ILI8961_SerialRGBDummy_360_240,
	&ILI8961_BT656NTSC_720_480,
	&ILI8961_BT656PAL_720_576,
	&ILI8961_BT601_720_480,
	&TFT3P3469_220_176,
	NULL,
};

/* ^_^ Add new lcd_panel i2c id here */
static const struct i2c_device_id lcd_panel_i2c_id[] = {
	{}
};
MODULE_DEVICE_TABLE(i2c, chip_i2c_id);

/* lcd panel will only be power on when (*enable) is called */
static int lcd_panel_enable(struct display_device *pdev, int en)
{
	int ret;

	pdev->enable = en;

	if (en)
		display_peripheral_sequential_power(pdev, en);

	if (en && pdev->interface_type == DISPLAY_INTERFACE_TYPE_I80) {
		i80_lcdc_enable(0);
		i80_default_cfg(pdev->i80_format);
		i80_enable(en);
	}

	if (en && pdev->peripheral_config.init) {
		ret = pdev->peripheral_config.init(pdev);
		if (ret) {
			dlog_err("peripheral device init failed %d\n", ret);
			return ret;
		}
	}
	if (pdev->peripheral_config.enable) {
		ret = pdev->peripheral_config.enable(pdev, en);
		if (ret) {
			dlog_err("peripheral_config.enable(%d) failed %d\n", en, ret);
			return ret;
		}
	}

	if (en && pdev->interface_type == DISPLAY_INTERFACE_TYPE_I80) {
		if (pdev->peripheral_config.i80_refresh) {
			if (pdev->peripheral_config.i80_mem_phys_addr)
				pdev->peripheral_config.i80_refresh(pdev, pdev->peripheral_config.i80_mem_phys_addr);
		}
		else {
			i80_enable(en);
			i80_lcdc_enable(en);
		}
	}

	if (!en && pdev->interface_type == DISPLAY_INTERFACE_TYPE_I80) {
		i80_lcdc_enable(en);
		i80_enable(en);
	}

	if (!en)
		display_peripheral_sequential_power(pdev, en);

	return 0;
}

static int lcd_panel_probe(struct display_device *pdev)
{
	int i, j;
	char item[64] = {0};
	char str[64] = {0};
	unsigned long i2c_bus;
	struct i2c_adapter *adap;
	struct i2c_client *client;
	
	dlog_trace();

	/* get lcd panel params */
	sprintf(item, DISPLAY_ITEM_PERIPHERAL_NAME, pdev->path, "lcd_panel");
	if (!item_exist(item)) {
		dlog_err("item %s does NOT exist.\n", item);
		return -EINVAL;
	}
	item_string(str, item, 0);
	dlog_info("panel name=%s\n", str);
	if (sysfs_streq(str, "auto_detect")) {
		// TODO: To be implement
		/*
		ret = display_panel_auto_detect(pdev, DISPLAY_PANEL_DETECT_METHOD, str);
		if (ret == FALSE) {
			dlog_err("auto detect failed\n");
			return -EIO;
		}
		*/
	}

	/* match */
	for (i = 0; panel_configs[i]; i++) {
		if (sysfs_streq(panel_configs[i]->name, str))
			break;
	}
	if (panel_configs[i] == NULL) {
		dlog_err("lcd panel %s NOT found in panel_configs\n", str);
		return -EINVAL;
	}
	memcpy(&pdev->peripheral_config, panel_configs[i], sizeof(struct peripheral_param_config));
	/* init status value for robustness */
	pdev->rgb_order = panel_configs[i]->rgb_order;
	pdev->bt656_pad_map = panel_configs[i]->bt656_pad_map;
	pdev->bt601_yuv_format = panel_configs[i]->bt601_yuv_format;
	pdev->bt601_data_width = panel_configs[i]->bt601_data_width;
	pdev->interface_type = panel_configs[i]->interface_type;
	pdev->i80_format = panel_configs[i]->i80_format;
	pdev->vic = DISPLAY_SPECIFIC_VIC_PERIPHERAL;

	display_peripheral_resource_init(pdev);

	if (panel_configs[i]->attrs) {
		for (j = 0; panel_configs[i]->attrs[j].attr.name != NULL; j++) {
			sysfs_create_file(&pdev->dev.kobj, &panel_configs[i]->attrs[j].attr);
		}
	}

	if (pdev->peripheral_config.control_interface != DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE) {
		if (!pdev->peripheral_config.enable) {
			dlog_err("when has control interface, (*enable) can NOT leave blank.\n");
			return -EINVAL;
		}
		if (pdev->fixed_resolution && pdev->peripheral_config.set_vic)
			dlog_info("for fixed resolution peripheral device, (*set_vic) should leave blank.\n");
	}
	
	if (pdev->peripheral_config.control_interface == DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI) {
		if (!pdev->peripheral_config.spi_info.mode && !pdev->peripheral_config.spi_info.controller_data) {
			dlog_err("when SPI control interface, spi_info can NOT leave blank.\n");
			return -EINVAL;
		}
		pdev->peripheral_config.spi_info.platform_data = pdev;
		spi_register_board_info(&pdev->peripheral_config.spi_info, 1);
	}
	else if (pdev->peripheral_config.control_interface == DISPLAY_PERIPHERAL_CONTROL_INTERFACE_I2C) {
		if (!pdev->peripheral_config.i2c_info.addr) {
			dlog_err("when I2C control interface, i2c_info can NOT leave blank.\n");
			return -EINVAL;
		}
		sprintf(item, DISPLAY_ITEM_PERIPHERAL_I2CBUS, pdev->path, "lcd_panel");
		if (!item_exist(item)) {
			dlog_err("item %s does NOT exist.\n", item);
			return -EINVAL;
		}
		item_string(str, item, 0);
		if (kstrtoul(str, 10, &i2c_bus)) {
			dlog_err("item %s can NOT be parsed to integer correctly.\n", str);
			return -EINVAL;
		}
		dlog_dbg("register i2c device, bus=%ld, addr=0x%02X, name=%s\n",
						i2c_bus, pdev->peripheral_config.i2c_info.addr, pdev->peripheral_config.i2c_info.type);
		pdev->peripheral_config.i2c_info.platform_data = pdev;
		adap = i2c_get_adapter(i2c_bus);
		client = i2c_new_device(adap, &pdev->peripheral_config.i2c_info);
		pdev->i2c = client;
		if (client)
			i2c_set_clientdata(client, pdev);
		else
			dlog_info("register i2c device failed, the chip device should handle i2c_client by itself if share use\n");
	}
	else if  (pdev->peripheral_config.control_interface != DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE) {
		dlog_err("Unrecognize control interface %d\n", pdev->peripheral_config.control_interface);
		return -EINVAL;
	}

	return 0;
}

static int lcd_panel_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

//=============== spi ctrl interface ==============

static int lcd_panel_spi_probe(struct spi_device *spi)
{
	int ret;
	struct display_device *pdev = spi->dev.platform_data;
	
	dlog_trace();

	pdev->spi = spi;

	spi->bits_per_word = pdev->peripheral_config.spi_bits_per_word;
	ret = spi_setup(spi);
	if (ret < 0) {
		dlog_err("spi setup failed %d\n", ret);
		return ret;
	}

	return 0;
}

static int lcd_panel_spi_remove(struct spi_device *spi)
{
	dlog_trace();

	return 0;
}


//=============== i2c ctrl interface ==============

static int lcd_panel_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	dlog_trace();

	return 0;
}

static int lcd_panel_i2c_remove(struct i2c_client *client)
{
	dlog_trace();

	return 0;
}



static struct i2c_driver lcd_panel_i2c_driver = {
	.driver = {
		.name = "lcd_panel_i2c",
		.owner = THIS_MODULE,
	},
	.probe = lcd_panel_i2c_probe,
	.remove = lcd_panel_i2c_remove,
	.id_table = lcd_panel_i2c_id,
};

static struct spi_driver lcd_panel_spi_driver = {
	.driver = {
		.name	= "lcd_panel_spi",
		.owner	= THIS_MODULE,
	},
	.probe		= lcd_panel_spi_probe,
	.remove		= lcd_panel_spi_remove,
};

static struct display_driver lcd_panel_driver = {
	.probe  = lcd_panel_probe,
	.remove = lcd_panel_remove,
	.driver = {
		.name = "lcd_panel",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_PERIPHERAL,
	.function = {
		.enable = lcd_panel_enable,
	}
};

static struct display_device lcd_panel_device = {
	.name = "lcd_panel",
	.id = -1,
	.path = -1,
	.fixed_resolution = 1,
};


static int __init lcd_panel_driver_init(void)
{
	int ret;
	int i, j;
	char name[64] = {0};
	char peripheral[64] = {0};
	
	ret = display_driver_register(&lcd_panel_driver);
	if (ret) {
		dlog_err("register lcd_panel driver failed %d\n", ret);
		return ret;
	}
	ret = spi_register_driver(&lcd_panel_spi_driver);
	if (ret) {
		dlog_err("register lcd panel spi driver failed %d\n", ret);
		return ret;
	}
	ret = i2c_add_driver(&lcd_panel_i2c_driver);
	if (ret) {
		dlog_err("register lcd panel i2c driver failed %d\n", ret);
		return ret;
	}

	for (i = 0; i < IDS_PATH_NUM; i++) {
		for (j = 0; j < MAX_PATH_PERIPHERAL_NUM; j++) { 
			sprintf(name, DISPLAY_ITEM_PERIPHERAL_TYPE, i, j);
			if (!item_exist(name))
				break;
			item_string(peripheral, name, 0);
			if (sysfs_streq(peripheral, "lcd_panel")) {
				dlog_info("path%d: register peripheral%d device %s\n", i, j, peripheral);
				lcd_panel_device.path = i;
				display_device_register(&lcd_panel_device);
				goto out;
			} 
		}
	}
	dlog_info("no lcd_panel device\n");

out:
	return 0;
}
module_init(lcd_panel_driver_init);

static void __exit lcd_panel_driver_exit(void)
{
	spi_unregister_driver(&lcd_panel_spi_driver);
	display_driver_unregister(&lcd_panel_driver);
}
module_exit(lcd_panel_driver_exit);

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display lcd panel peripheral driver");
MODULE_LICENSE("GPL");
