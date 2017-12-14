/*
 * display-core.c - display core infrastructure
 *
 * Copyright (c) 2017-2022 feng.qu@infotm.com
 *
 * This file is released under the GPLv2
 *
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/idr.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/suspend.h>
#include <linux/display_device.h>
#include <mach/items.h>
#include <mach/imap-iomap.h>
#include <mach/pad.h>

#include "display_register.h"
#include "display_items.h"

#define DISPLAY_MODULE_LOG		"core"

/* For automatically allocated device IDs */
static DEFINE_IDA(display_devid_ida);

static int path_enable[IDS_PATH_NUM] = {0};

int display_log_level = LOG_LEVEL_INFO;

#define to_display_driver(drv)	(container_of((drv), struct display_driver, driver))

extern const char *__clk_get_name(struct clk *clk);
#ifndef CONFIG_ARCH_APOLLO
extern int efuse_get_efuse_id(uint32_t *efuse_id);
#endif


struct device display_bus = {
	.init_name	= "display",
};
EXPORT_SYMBOL_GPL(display_bus);
/*
  * Chip ID		efuse-id		status
  * Q3420P	3420010		Lock
  * Q3420F	3420110		Lock
  * C20			3420310		ok
  * C23			3420210		ok
*/
bool display_support(void)
{
#ifndef CONFIG_ARCH_APOLLO	
	unsigned int efuse_id;
	if (efuse_get_efuse_id(&efuse_id))
		if (efuse_id == 0x3420010 || efuse_id == 0x3420110)
			return false;
#endif
	return true;
}
EXPORT_SYMBOL_GPL(display_support);

struct resource *display_get_resource(struct display_device *dev,
				       unsigned int type, unsigned int num)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];

		if (type == resource_type(r) && num-- == 0)
			return r;
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(display_get_resource);

/**
 * display_get_irq - get an IRQ for a device
 * @dev: display device
 * @num: IRQ number index
 */
int display_get_irq(struct display_device *dev, unsigned int num)
{
	struct resource *r = display_get_resource(dev, IORESOURCE_IRQ, num);

	return r ? r->start : -ENXIO;
}
EXPORT_SYMBOL_GPL(display_get_irq);

/**
 * display_get_resource_byname - get a resource for a device by name
 * @dev: display device
 * @type: resource type
 * @name: resource name
 */
struct resource *display_get_resource_byname(struct display_device *dev,
					      unsigned int type,
					      const char *name)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];

		if (unlikely(!r->name))
			continue;

		if (type == resource_type(r) && !strcmp(r->name, name))
			return r;
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(display_get_resource_byname);

/**
 * display_get_irq_byname - get an IRQ for a device by name
 * @dev: display device
 * @name: IRQ name
 */
int display_get_irq_byname(struct display_device *dev, const char *name)
{
	struct resource *r = display_get_resource_byname(dev, IORESOURCE_IRQ,
							  name);

	return r ? r->start : -ENXIO;
}
EXPORT_SYMBOL_GPL(display_get_irq_byname);

/**
 * display_add_devices - add a numbers of display devices
 * @devs: array of display devices to add
 * @num: number of display devices in array
 */
int display_add_devices(struct display_device **devs, int num)
{
	int i, ret = 0;

	for (i = 0; i < num; i++) {
		ret = display_device_register(devs[i]);
		if (ret) {
			while (--i >= 0)
				display_device_unregister(devs[i]);
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(display_add_devices);

struct display_object {
	struct display_device pdev;
	char name[1];
};

/**
 * display_device_put - destroy a display device
 * @pdev: display device to free
 *
 * Free all memory associated with a display device.  This function must
 * _only_ be externally called in error cases.  All other usage is a bug.
 */
void display_device_put(struct display_device *pdev)
{
	if (pdev)
		put_device(&pdev->dev);
}
EXPORT_SYMBOL_GPL(display_device_put);

static void display_device_release(struct device *dev)
{
	struct display_object *pa = container_of(dev, struct display_object,
						  pdev.dev);

	kfree(pa->pdev.dev.platform_data);
	kfree(pa->pdev.resource);
	kfree(pa);
}

/**
 * display_device_alloc - create a display device
 * @name: base name of the device we're adding
 * @id: instance id
 *
 * Create a display device object which can have other objects attached
 * to it, and which will have attached objects freed when it is released.
 */
struct display_device *display_device_alloc(const char *name, int id)
{
	struct display_object *pa;

	pa = kzalloc(sizeof(struct display_object) + strlen(name), GFP_KERNEL);
	if (pa) {
		strcpy(pa->name, name);
		pa->pdev.name = pa->name;
		pa->pdev.id = id;
		device_initialize(&pa->pdev.dev);
		pa->pdev.dev.release = display_device_release;
	}

	return pa ? &pa->pdev : NULL;
}
EXPORT_SYMBOL_GPL(display_device_alloc);

/**
 * display_device_add_resources - add resources to a display device
 * @pdev: display device allocated by display_device_alloc to add resources to
 * @res: set of resources that needs to be allocated for the device
 * @num: number of resources
 *
 * Add a copy of the resources to the display device.  The memory
 * associated with the resources will be freed when the display device is
 * released.
 */
int display_device_add_resources(struct display_device *pdev,
				  const struct resource *res, unsigned int num)
{
	struct resource *r = NULL;

	if (res) {
		r = kmemdup(res, sizeof(struct resource) * num, GFP_KERNEL);
		if (!r)
			return -ENOMEM;
	}

	kfree(pdev->resource);
	pdev->resource = r;
	pdev->num_resources = num;
	return 0;
}
EXPORT_SYMBOL_GPL(display_device_add_resources);

/**
 * display_device_add_data - add display-specific data to a display device
 * @pdev: display device allocated by display_device_alloc to add resources to
 * @data: display specific data for this display device
 * @size: size of display specific data
 *
 * Add a copy of display specific data to the display device's
 * platform_data pointer.  The memory associated with the display data
 * will be freed when the display device is released.
 */
int display_device_add_data(struct display_device *pdev, const void *data,
			     size_t size)
{
	void *d = NULL;

	if (data) {
		d = kmemdup(data, size, GFP_KERNEL);
		if (!d)
			return -ENOMEM;
	}

	kfree(pdev->dev.platform_data);
	pdev->dev.platform_data = d;
	return 0;
}
EXPORT_SYMBOL_GPL(display_device_add_data);

/**
 * display_device_add - add a display device to device hierarchy
 * @pdev: display device we're adding
 *
 * This is part 2 of display_device_register(), though may be called
 * separately _iff_ pdev was allocated by display_device_alloc().
 */
int display_device_add(struct display_device *pdev)
{
	int i, ret;

	if (!display_support())
			return 0;

	if (!pdev)
		return -EINVAL;

	if (!pdev->dev.parent)
		pdev->dev.parent = &display_bus;

	pdev->dev.bus = &display_bus_type;

	switch (pdev->id) {
	default:
		dev_set_name(&pdev->dev, "%s.%d", pdev->name,  pdev->id);
		break;
	case DISPLAY_DEVID_NONE:
		dev_set_name(&pdev->dev, "%s", pdev->name);
		break;
	case DISPLAY_DEVID_AUTO:
		/*
		 * Automatically allocated device ID. We mark it as such so
		 * that we remember it must be freed, and we append a suffix
		 * to avoid namespace collision with explicit IDs.
		 */
		ret = ida_simple_get(&display_devid_ida, 0, 0, GFP_KERNEL);
		if (ret < 0)
			goto err_out;
		pdev->id = ret;
		pdev->id_auto = true;
		dev_set_name(&pdev->dev, "%s.%d.auto", pdev->name, pdev->id);
		break;
	}

	for (i = 0; i < pdev->num_resources; i++) {
		struct resource *p, *r = &pdev->resource[i];

		if (r->name == NULL)
			r->name = dev_name(&pdev->dev);

		p = r->parent;
		if (!p) {
			if (resource_type(r) == IORESOURCE_MEM)
				p = &iomem_resource;
			else if (resource_type(r) == IORESOURCE_IO)
				p = &ioport_resource;
		}

		if (p && insert_resource(p, r)) {
			dev_err(&pdev->dev, "failed to claim resource %d\n", i);
			ret = -EBUSY;
			goto failed;
		}
	}

	pr_debug("Registering display device '%s'. Parent at %s\n",
		 dev_name(&pdev->dev), dev_name(pdev->dev.parent));

	ret = device_add(&pdev->dev);
	if (ret == 0)
		return ret;

 failed:
	if (pdev->id_auto) {
		ida_simple_remove(&display_devid_ida, pdev->id);
		pdev->id = DISPLAY_DEVID_AUTO;
	}

	while (--i >= 0) {
		struct resource *r = &pdev->resource[i];
		unsigned long type = resource_type(r);

		if (type == IORESOURCE_MEM || type == IORESOURCE_IO)
			release_resource(r);
	}

 err_out:
	return ret;
}
EXPORT_SYMBOL_GPL(display_device_add);

/**
 * display_device_del - remove a display-level device
 * @pdev: display device we're removing
 *
 * Note that this function will also release all memory- and port-based
 * resources owned by the device (@dev->resource).  This function must
 * _only_ be externally called in error cases.  All other usage is a bug.
 */
void display_device_del(struct display_device *pdev)
{
	int i;

	if (pdev) {
		device_del(&pdev->dev);

		if (pdev->id_auto) {
			ida_simple_remove(&display_devid_ida, pdev->id);
			pdev->id = DISPLAY_DEVID_AUTO;
		}

		for (i = 0; i < pdev->num_resources; i++) {
			struct resource *r = &pdev->resource[i];
			unsigned long type = resource_type(r);

			if (type == IORESOURCE_MEM || type == IORESOURCE_IO)
				release_resource(r);
		}
	}
}
EXPORT_SYMBOL_GPL(display_device_del);

/**
 * display_device_register - add a display-level device
 * @pdev: display device we're adding
 */
int display_device_register(struct display_device *pdev)
{
	device_initialize(&pdev->dev);
	return display_device_add(pdev);
}
EXPORT_SYMBOL_GPL(display_device_register);

/**
 * display_device_unregister - unregister a display-level device
 * @pdev: display device we're unregistering
 *
 * Unregistration is done in 2 steps. First we release all resources
 * and remove it from the subsystem, then we drop reference count by
 * calling display_device_put().
 */
void display_device_unregister(struct display_device *pdev)
{
	display_device_del(pdev);
	display_device_put(pdev);
}
EXPORT_SYMBOL_GPL(display_device_unregister);

static int display_drv_probe(struct device *_dev)
{
	struct display_driver *drv = to_display_driver(_dev->driver);
	struct display_device *dev = to_display_device(_dev);
	int ret;

	ret = drv->probe(dev);

	if (!ret)
		dev->module_type = drv->module_type;

	return ret;
}

static int display_drv_remove(struct device *_dev)
{
	struct display_driver *drv = to_display_driver(_dev->driver);
	struct display_device *dev = to_display_device(_dev);
	int ret;

	ret = drv->remove(dev);

	return ret;
}

static void display_drv_shutdown(struct device *_dev)
{
	struct display_driver *drv = to_display_driver(_dev->driver);
	struct display_device *dev = to_display_device(_dev);

	drv->shutdown(dev);
}

/**
 * display_driver_register - register a driver for display-level devices
 * @drv: display driver structure
 */
int display_driver_register(struct display_driver *drv)
{
	if (!display_support())
			return 0;
	
	drv->driver.bus = &display_bus_type;
	if (drv->probe)
		drv->driver.probe = display_drv_probe;
	if (drv->remove)
		drv->driver.remove = display_drv_remove;
	if (drv->shutdown)
		drv->driver.shutdown = display_drv_shutdown;

	return driver_register(&drv->driver);
}
EXPORT_SYMBOL_GPL(display_driver_register);

/**
 * display_driver_unregister - unregister a driver for display-level devices
 * @drv: display driver structure
 */
void display_driver_unregister(struct display_driver *drv)
{
	driver_unregister(&drv->driver);
}
EXPORT_SYMBOL_GPL(display_driver_unregister);

struct display_match_struct {
	enum display_module_type module_type;
	int path;
};

static int __display_match_path_module_type(struct device *dev, void *data)
{
	struct display_device *pdev = to_display_device(dev);
	struct display_match_struct *match_data = data;
	if (pdev->module_type == match_data->module_type
			&& pdev->path == match_data->path) {
		if (pdev->module_type == DISPLAY_MODULE_TYPE_PERIPHERAL)
			return pdev->peripheral_selected;
		return 1;
	}
	else
		return 0;
}

/**
 * display_get_function - get function struct by path and module type
 * @path: ids path
 * @type: module type
 * @func: function struct pointer to save
 * @pdev: target module handle to save
 */
int display_get_function(int path, enum display_module_type type,
			struct display_function **func, struct display_device **pdev)
{
	struct device *dev;
	struct display_driver *display_drv;
	struct display_match_struct data;

	data.module_type = type;
	data.path = path;
	dev = bus_find_device(&display_bus_type, NULL, &data, __display_match_path_module_type);
	if (!dev)
		return -EINVAL;

	display_drv = to_display_driver(dev->driver);
	if (!display_drv) {
		dlog_info("module %d driver NOT found\n", type);
		return -EINVAL;
	}

	*func = &display_drv->function;
	*pdev = to_display_device(dev);

	return 0;
}
EXPORT_SYMBOL_GPL(display_get_function);

/**
 * display_get_device - get device struct by path and module type
 * @path: ids path
 * @type: module type
 * @pdev: target module handle to save
 */
int display_get_device(int path, enum display_module_type type,
			struct display_device  **pdev)
{
	struct display_function *func;
	return display_get_function(path, type, &func, pdev);
}
EXPORT_SYMBOL_GPL(display_get_device);

/* @return result after round */
static int __do_div_round(int n, int base)
{
	int mod = do_div(n, base);
	if (mod * 2 >= base)
		n++;
	return n;
}

static int __display_center_logo(int overlay_width, int overlay_height,
													int osd_width, int osd_height, int *xoffset, int *yoffset)
{
	if (overlay_width > osd_width)
		*xoffset = 0;
	else
		*xoffset = __do_div_round(osd_width - overlay_width, 2);

	if (overlay_height > osd_height)
		*yoffset = 0;
	else
		*yoffset = __do_div_round(osd_height - overlay_height, 2);

	*xoffset &= (~0x01);
	*yoffset &= (~0x01);

	return 0;
}

/* use prescaler */
static int 	__display_calc_logo_scale(int logo_width, int logo_height, 
		int osd_width, int osd_height, enum display_logo_scale logo_scale, 
		int *overlay_width, int *overlay_height, int *overlay_xoffset, int *overlay_yoffset)
{
	unsigned int logo_ratio, osd_ratio;

	dlog_dbg("logo_scale = %d, logo=%d*%d, osd=%d*%d\n", 
					logo_scale, logo_width, logo_height, osd_width, osd_height);

	logo_ratio = __do_div_round(logo_width * 1000, logo_height);
	osd_ratio = __do_div_round(osd_width * 1000, osd_height);
	
	if (logo_scale == DISPLAY_LOGO_SCALE_NONSCALE) {
		*overlay_width = logo_width;
		*overlay_height = logo_height;
	}
	else if (logo_scale == DISPLAY_LOGO_SCALE_FIT) {
		if (logo_ratio >= osd_ratio) {
			*overlay_width = osd_width;
			*overlay_height = __do_div_round((logo_height * osd_width), logo_width);
			if (*overlay_height > osd_height) {
				dlog_err("fit scale algorithm error\n");
				return -EINVAL;
			}
		}
		else {
			*overlay_width = __do_div_round((logo_width * osd_height), logo_height);
			*overlay_height = osd_height;
			if (*overlay_width > osd_width) {
				dlog_err("fit scale  algorithm error\n");
				return -EINVAL;
			}
		}
	}
	else if (logo_scale == DISPLAY_LOGO_SCALE_STRETCH) {
		*overlay_width = osd_width;
		*overlay_height = osd_height;
	}

	else {
		dlog_err("Invalid logo scale %d\n", logo_scale);
		return -EINVAL;
	}

	/* necessary? */
	*overlay_width &= (~0x03);
	*overlay_height &= (~0x03);

	__display_center_logo(*overlay_width, *overlay_height, osd_width,
											osd_height, overlay_xoffset, overlay_yoffset);

	return 0;
}

/* must be called after display_set_peripheral_vic if necessary */
int display_set_logo(int path, int buf_addr, int logo_width, int logo_height,
				enum display_logo_scale logo_scale, int background)
{
	struct display_device  *pdev;
	struct display_function *func;
	int peripheral_width, peripheral_height;
	int overlay_width, overlay_height, overlay_xoffset, overlay_yoffset;
	dtd_t dtd = {0};
	int ret;

	/* get peripheral vic */
	display_get_function(path, DISPLAY_MODULE_TYPE_VIC, &func, &pdev);
	ret = vic_fill(path, &dtd, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
	if (ret) {
		dlog_err("vic fill failed %d\n", ret);
		return ret;
	}
	dtd.mHImageSize = 0;
	peripheral_width = dtd.mHActive;
	peripheral_height = dtd.mVActive;
#if 1
	/* prescaler */

	/* save osd dtd */
	dtd.mCode = DISPLAY_SPECIFIC_VIC_OSD;
	func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_OSD, &dtd);
	if (ret) {
		dlog_err("save osd dtd failed %d\n", ret);
		return ret;
	}

	/* save buffer dtd */
	dtd.mHActive = logo_width;
	dtd.mVActive = logo_height,
	dtd.mHImageSize = logo_width;
	dtd.mCode = DISPLAY_SPECIFIC_VIC_BUFFER_START+0;
	ret = func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_BUFFER_START+0, &dtd);
	if (ret) {
		dlog_err("save buffer dtd failed %d\n", ret);
		return ret;
	}

	/* calc overlay dtd */
	ret = __display_calc_logo_scale(logo_width, logo_height, peripheral_width, peripheral_height,
					logo_scale, &overlay_width, &overlay_height, &overlay_xoffset, &overlay_yoffset);
	if (ret) {
		dlog_err("calc logo scale failed %d\n", ret);
		return ret;
	}
	dlog_dbg("path%d calculated overlay params: width=%d, height=%d, xoffset=%d, yoffset=%d\n",
						path, overlay_width, overlay_height, overlay_xoffset, overlay_yoffset);

	/* save overlay dtd */
	dtd.mHActive = overlay_width;
	dtd.mVActive = overlay_height;
	dtd.mHImageSize = 0;
	dtd.mCode = DISPLAY_SPECIFIC_VIC_OVERLAY_START+0;
	func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_OVERLAY_START+0, &dtd);
	if (ret) {
		dlog_err("save overlay dtd failed %d\n", ret);
		return ret;
	}
#else
	/* postscaler */

	/* save buffer dtd */
	dtd.mHActive = logo_width;
	dtd.mVActive = logo_height,
	dtd.mHImageSize = logo_width;
	dtd.mCode = DISPLAY_SPECIFIC_VIC_BUFFER_START+0;
	ret = func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_BUFFER_START+0, &dtd);
	if (ret) {
		dlog_err("save buffer dtd failed %d\n", ret);
		return ret;
	}

	/* save overlay dtd */
	dtd.mCode = DISPLAY_SPECIFIC_VIC_OVERLAY_START+0;
	func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_OVERLAY_START+0, &dtd);
	if (ret) {
		dlog_err("save overlay dtd failed %d\n", ret);
		return ret;
	}

	/* save osd dtd */
	dtd.mCode = DISPLAY_SPECIFIC_VIC_OSD;
	func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_OSD, &dtd);
	if (ret) {
		dlog_err("save osd dtd failed %d\n", ret);
		return ret;
	}
	overlay_width = 0;
	overlay_height = 0;
	overlay_xoffset = 0;
	overlay_yoffset = 0;
#endif
	/* config overlay 0 */
	display_get_function(path, DISPLAY_MODULE_TYPE_OVERLAY, &func, &pdev);
	func->set_vic(pdev, DISPLAY_SPECIFIC_VIC_OSD);
	func->set_overlay_vic(pdev, 0, DISPLAY_SPECIFIC_VIC_OVERLAY_START+0);
	func->set_overlay_position(pdev, 0, overlay_xoffset, overlay_yoffset);
	func->set_overlay_vw_width(pdev, 0, logo_width);
	func->set_overlay_bpp(pdev, 0, DISPLAY_DATA_FORMAT_32BPP_ARGB8888, DISPLAY_YUV_FORMAT_NOT_USE, 0);
	func->set_overlay_buffer_addr(pdev, 0, buf_addr);
	func->set_background_color(pdev, background);

	/* save buffer address if using i80 manual refresh method */
	display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
	if (pdev->interface_type == DISPLAY_INTERFACE_TYPE_I80 &&
			pdev->peripheral_config.i80_refresh) {
		pdev->peripheral_config.i80_mem_phys_addr = buf_addr;
	}

	return 0;
}

int resource_find_regulator(struct peripherial_resource *power_resource, char *name)
{
	int i;

	for (i = 0; i < MAX_REGULATOR_NUM; i++)
		if (!strcmp(power_resource->regulators_name[i], name))
			return i;
	return -1;
}
EXPORT_SYMBOL_GPL(resource_find_regulator);

/* save dtd and config controller */
static int __display_config_controller(int path)
{
	struct display_device * display_dev;
	struct display_device * pdev;
	struct display_function * func;
	dtd_t dtd = {0};
	int ret;
	unsigned char default_mask = (1<<IDSINTPND_OSDERR) | (1<<IDSINTPND_VCLKINT);

	ret = display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &display_dev);
	if (ret) {
		dlog_err("Failed to get path%d peripheral function\n", path);
		return ret;
	}

	/* store peripheral dtd which is useful for controller */
	display_get_function(path, DISPLAY_MODULE_TYPE_VIC, &func, &pdev);
	if (!strcmp(display_dev->name, "lcd_panel")) {
		func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_PERIPHERAL, 
										&display_dev->peripheral_config.dtd);
	}
	else if (!strcmp(display_dev->name, "chip") || !strcmp(display_dev->name, "hdmi")) {
		if (!display_dev->vic) {
			dlog_err("display_dev->vic should NOT be zero\n");
			return -EINVAL;
		}
		ret = vic_fill(path, &dtd, display_dev->vic);
		if (ret) {
			dlog_err("vic fill peripheral vic %d failed %d\n", display_dev->vic, ret);
			return ret;
		}
		dtd.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL;
		func->set_dtd(pdev, DISPLAY_SPECIFIC_VIC_PERIPHERAL, &dtd);
	}
	// TODO: else if: mipi_panel ...

	/* config controller */
	if ((display_dev->interface_type >= DISPLAY_INTERFACE_TYPE_PARALLEL_RGB888
			&& display_dev->interface_type <= DISPLAY_INTERFACE_TYPE_I80)
			|| display_dev->interface_type == DISPLAY_INTERFACE_TYPE_HDMI
			|| display_dev->interface_type == DISPLAY_INTERFACE_TYPE_MIPI_DSI
			|| display_dev->interface_type == DISPLAY_INTERFACE_TYPE_LVDS) {

		/* set lcdc controller vic, interface type and rgb order */
		display_get_function(path, DISPLAY_MODULE_TYPE_LCDC, &func, &pdev);
		ret = func->set_vic(pdev, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
		if (ret) {
			dlog_err("path%d: set lcdc vic failed %d\n", path, ret);
			return ret;
		}
		ret = func->set_interface_type(pdev, display_dev->peripheral_config.interface_type);
		if (ret) {
			dlog_err("path%d: set lcdc interface type failed %d\n", path, ret);
			return ret;
		}
		ret = func->set_rgb_order(pdev, display_dev->peripheral_config.rgb_order);
		if (ret) {
			dlog_err("path%d: set lcdc rgb order failed %d\n", path, ret);
			return ret;
		}
		ret = func->set_auto_vclk(pdev, 0x0f, 0x01);
		if (ret) {
			dlog_err("path%d: set lcdc auto vclk failed %d\n", path, ret);
			return ret;
		}
	}
	else if (display_dev->interface_type == DISPLAY_INTERFACE_TYPE_BT656
				|| display_dev->interface_type == DISPLAY_INTERFACE_TYPE_BT601) {

		/* set tvif controller vic, interface type, bt656 pad map and bt601 yuv format */
		display_get_function(path, DISPLAY_MODULE_TYPE_TVIF, &func, &pdev);
		ret = func->set_vic(pdev, DISPLAY_SPECIFIC_VIC_PERIPHERAL);
		if (ret) {
			dlog_err("path%d: set tvif vic failed %d\n", path, ret);
			return ret;
		}
		ret = func->set_interface_type(pdev, display_dev->peripheral_config.interface_type);
		if (ret) {
			dlog_err("path%d: set tvif interface type failed %d\n", path, ret);
			return ret;
		}
		if (display_dev->interface_type == DISPLAY_INTERFACE_TYPE_BT656) {
			ret = func->set_bt656_pad_map(pdev, display_dev->peripheral_config.bt656_pad_map);
			if (ret) {
				dlog_err("path%d: set tvif bt656 pad map failed %d\n", path, ret);
				return ret;
			}
		}
		else {
			ret = func->set_bt601_yuv_format(pdev, display_dev->peripheral_config.bt601_data_width,
																		display_dev->peripheral_config.bt601_yuv_format);
			if (ret) {
				dlog_err("path%d: set tvif bt601 yuv format failed %d\n", path, ret);
				return ret;
			}
		}
	}
	else {
		dlog_err("Invalid target peripheral device interface type %d\n", display_dev->peripheral_config.interface_type);
		return -EINVAL;
	}

	if (display_dev->interface_type == DISPLAY_INTERFACE_TYPE_HDMI) {
		display_get_function(path, DISPLAY_MODULE_TYPE_HDMI, &func, &pdev);
		ret = func->set_vic(pdev, display_dev->vic);
		if (ret) {
			dlog_err("path%d: set hdmi vic %d failed %d\n", path, display_dev->vic, ret);
			return ret;
		}
	}

	display_get_function(path, DISPLAY_MODULE_TYPE_IRQ, &func, &pdev);
	if (display_dev->interface_type == DISPLAY_INTERFACE_TYPE_BT656
			|| display_dev->interface_type == DISPLAY_INTERFACE_TYPE_BT601) {
		func->set_irq_mask(pdev, ~(default_mask | (1<<IDSINTPND_TVINT)));
	}
	else {
		func->set_irq_mask(pdev, ~(default_mask | (1<<IDSINTPND_LCDINT)));
	}	

	return 0;
}

/**
 * display_get_peripheral_vic - get peripheral device vic, only valid for variable resolution device
 * @path: ids path
 * @vic: target peripheral device vic
 * when not valid, return -EINVAL
 */
int display_get_peripheral_vic(int path)
{
	struct display_device * pdev;
	int ret;

	ret = display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
	if (ret) {
		dlog_err("Failed to get path%d peripheral function\n", path);
		return ret;
	}
	if (pdev->fixed_resolution) {
		dlog_err("path%d: peripheral %s has a fixed resolution, use fb_var_screeninfo instead\n",
					path, pdev->name);
		return -EINVAL;
	}
	return pdev->vic;
}

/**
 * display_set_peripheral_vic - set peripheral device vic, must be called when path disabled
 * @path: ids path
 * @vic: target peripheral device vic
 */
int display_set_peripheral_vic(int path, int vic)
{
	struct display_device * pdev;
	struct display_function * func;
	int ret;
	dtd_t dtd;

	/* check */
	ret = vic_fill(path, &dtd, vic);
	if (ret) {
		dlog_err("Invalid vic %d\n", vic);
		return ret;
	}
	ret = display_get_function(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &func, &pdev);
	if (ret) {
		dlog_err("Failed to get path%d peripheral function\n", path);
		return ret;
	}
	if (pdev->fixed_resolution) {
		dlog_err("path%d: peripheral %s has a fixed resolution, can NOT set vic.\n",
					path, pdev->name);
		return -EINVAL;
	}
	if (!func->set_vic) {
		dlog_err("path%d peripheral driver has NO set_vic function.\n", path);
		return -EINVAL;
	}
	if (pdev->enable) {
		dlog_err("Can not set peripheral vic when path is enabled.\n");
		return -EINVAL;
	}

	/* set vic */
	ret = func->set_vic(pdev, vic);
	if (ret) {
		dlog_err("path%d: set peripheral vic failed %d\n", path, ret);
		return ret;
	}
	pdev->vic = vic;
	
	/* save dtd and config controller */
	ret = __display_config_controller(path);
	if (ret) {
		dlog_err("path%d: config controller failed %d\n", path, ret);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(display_set_peripheral_vic);

/**
 * display_select_peripheral - select target peripheral, must be called when path disabled
 * @path: ids path
 * @target_peripheral: target peripheral to select
 */
int display_select_peripheral(int path, char *target_peripheral)
{
	struct display_device  *pdev;
	struct display_device *display_dev;
	struct device *dev;
	int ret;

	/* dettach current peripheral */
	ret = display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
	if (!ret) {
		if (pdev->enable) {
			dlog_err("Can not select peripheral device when path is enabled.\n");
			return -EINVAL;
		}
		pdev->peripheral_selected = 0;
		if (!strcmp(pdev->name, "chip"))
			display_peripheral_sequential_power(pdev, 0);
	}
	else
		dlog_dbg("currently, there's no peripheral selected\n");

	/* select target peripheral */
	dlog_dbg("target peripheral %s\n", target_peripheral);
	dev = bus_find_device_by_name(&display_bus_type, NULL, target_peripheral);
	if (!dev) {
		dlog_err("target peripheral %s NOT found\n", target_peripheral);
		return -EINVAL;
	}
	display_dev = to_display_device(dev);

	/* chip need to keep power when set vic */
	if (!strcmp(display_dev->name, "chip")) {
		if (display_dev->peripheral_config.osc_clk) {
			display_set_clk_freq(display_dev, display_dev->clk_src,
												display_dev->peripheral_config.osc_clk, NULL);
		}
		display_peripheral_sequential_power(display_dev, 1);
		if (!display_dev->peripheral_config.init) {
			dlog_err("chip (*init) should NOT leave blank.\n");
			return -EINVAL;
		}
		ret = display_dev->peripheral_config.init(display_dev);
		if (ret) {
			dlog_err("peripheral chip init failed %d\n", ret);
			return ret;
		}
	}

	/* config peripheral for non-fixed resolution device */
	if (!display_dev->fixed_resolution) {
		if (!display_dev->vic) {
			if (!display_dev->peripheral_config.default_vic) {
				dlog_err("peripheral device default vic should NOT leave blank.\n");
				return -EINVAL;
			}
			dlog_dbg("peripheral default vic %d\n", display_dev->peripheral_config.default_vic);
			display_dev->vic = display_dev->peripheral_config.default_vic;
		}
		if (display_dev->peripheral_config.set_vic) {
			ret = display_dev->peripheral_config.set_vic(display_dev, display_dev->vic);
			if (ret) {
				dlog_err("set peripheral vic %d failed %d\n", display_dev->vic, ret);
				return ret;
			}
		}
	}
	display_dev->peripheral_selected = 1;

	/* save dtd and config controller */
	ret = __display_config_controller(path);
	if (ret) {
		dlog_err("path%d: config controller failed %d\n", path, ret);
		return ret;
	}

	return 0;	
}
EXPORT_SYMBOL_GPL(display_select_peripheral);

/**
 * display_path_ctrl - enable or disable all modules of a specific path
 * @path: ids path
 * @en: enable or disable
 */
static int display_path_ctrl(int path, int en)
{
	struct display_device *display_dev;
	struct display_function *func;
	struct display_device *pdev;
	int module_type;
	enum display_interface_type peripheral_interface;
	int ret;
	int i;

	dlog_dbg("%s path %d\n", en?"enabling":"disabling", path);

	ret = display_get_device(path, DISPLAY_MODULE_TYPE_PERIPHERAL, &display_dev);
	if (ret) {
		dlog_err("peripheral device NOT found\n");
		return -EINVAL;
	}
	peripheral_interface = display_dev->interface_type;
	dlog_dbg("peripheral_interface=%d\n", peripheral_interface);

	if (display_dev->enable && en) {
		dlog_err("path%d already enabled\n", path);
		return -EINVAL;
	}

	if (!display_dev->enable && !en) {
		dlog_err("path%d already disabled\n", path);
		return -EINVAL;
	}

	/* sequential enable or disable */
	for (module_type = en ? DISPLAY_MODULE_TYPE_FB : DISPLAY_MODULE_TYPE_PERIPHERAL;
			en ? (module_type <= DISPLAY_MODULE_TYPE_PERIPHERAL) : (module_type >= DISPLAY_MODULE_TYPE_FB);
			en ? (module_type++) : (module_type--)) {

		//dlog_dbg("iterating module %d ...\n", module_type);

		ret = display_get_function(path, module_type, &func, &pdev);
		if (ret)
			continue;

		if (module_type != DISPLAY_MODULE_TYPE_OVERLAY && !func->enable) {
			dlog_dbg("module %d func->enable NOT implement\n", module_type);
			continue;
		}

		if (module_type == DISPLAY_MODULE_TYPE_PERIPHERAL && !func->enable) {
			dlog_err("peripheral func->enable MUST be implement to record enable state\n");
			return -EINVAL;
		}

		if (module_type == DISPLAY_MODULE_TYPE_OVERLAY) {
			for (i = 0; i < IDS_OVERLAY_NUM; i++) {
				dlog_dbg("%s path%d module %d overlay%d\n", en?"enabling":"disabling", path, module_type, i);
				ret = func->overlay_enable(pdev, i, en);
				if (ret) {
					dlog_err("%s path%d overlay%d failed %d\n", en?"enabling":"disabling", path, i, ret);
					return ret;
				}
			}
		}
		else if (module_type == DISPLAY_MODULE_TYPE_LCDC) {
			if ((peripheral_interface >= DISPLAY_INTERFACE_TYPE_PARALLEL_RGB888
					&& peripheral_interface <= DISPLAY_INTERFACE_TYPE_SERIAL_RGB)
					|| peripheral_interface == DISPLAY_INTERFACE_TYPE_HDMI) {
				dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
				ret = func->enable(pdev, en);
			}
		}
		else if (module_type == DISPLAY_MODULE_TYPE_TVIF) {
			if (peripheral_interface == DISPLAY_INTERFACE_TYPE_BT656
				|| peripheral_interface == DISPLAY_INTERFACE_TYPE_BT601) {
				dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
				ret = func->enable(pdev, en);
			}
		}
		else if (module_type == DISPLAY_MODULE_TYPE_HDMI) {
			if (peripheral_interface == DISPLAY_INTERFACE_TYPE_HDMI) {
				dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
				ret = func->enable(pdev, en);
			}
		}
		else if (module_type == DISPLAY_MODULE_TYPE_MIPI_DSI) {
			if (peripheral_interface == DISPLAY_INTERFACE_TYPE_MIPI_DSI) {
				dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
				ret = func->enable(pdev, en);
			}
		}
		else if (module_type == DISPLAY_MODULE_TYPE_PERIPHERAL) {
			if (!pdev->peripheral_selected) {
				dlog_err("peripheral_selected is false, logic error\n");
				return -EINVAL;
			}
			dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
			ret = func->enable(pdev, en);
		}
		else {
			dlog_dbg("%s path%d module %d\n", en?"enabling":"disabling", path, module_type);
			ret = func->enable(pdev, en);
		}

		if (ret) {
			dlog_err("%s path%d module %d failed %d\n", en?"enable":"disable", path, module_type, ret);
			return ret;
		}
	}

	return 0;
}

int display_path_enable(int path)
{
	int ret;
	ret = display_path_ctrl(path, 1);
	if (ret) {
		dlog_err("enable path%d failed %d\n", path, ret);
		return ret;
	}
	path_enable[path] = 1;

	return 0;
}
EXPORT_SYMBOL_GPL(display_path_enable);

/**
 * display_path_disable - disable all modules of a specific path, stop display
 * @path: ids path
 */
int display_path_disable(int path)
{
	int ret;
	ret = display_path_ctrl(path, 0);
	if (ret) {
		dlog_err("disable path%d failed %d\n", path, ret);
		return ret;
	}
	path_enable[path] = 0;

	return 0;
}
EXPORT_SYMBOL_GPL(display_path_disable);

int display_peripheral_resource_init(struct display_device *pdev)
{
	int ret;
	int i;
	char item[64] = {0};
	char str[64] = {0};
	int val;
	int gpio_cnt = 0;
	int regulator_cnt = 0;
	struct regulator *regp;
	
	dlog_dbg("%s\n", pdev->name);

	memset(pdev->power_resource.regulators_name, 0, MAX_REGULATOR_NUM * REGULATOR_NAME_LEN);
	
	/* init resources */
	for (i = 0; i < DISPLAY_ITEM_PWRSEQ_MAX; i++) {
		memset(item, 0, 64);
		sprintf(item, DISPLAY_ITEM_PERIPHERAL_PWRSEQ, pdev->path, pdev->name, i);
		if (!item_exist(item))
			break;
		item_string(str, item, 0);
		if (!strncmp(str, "gpio", 4)) {
			val = simple_strtoul(str+4, NULL, 10);
			pdev->power_resource.gpios[gpio_cnt++] = val;
			dlog_dbg("item resource: gpio %d\n", val);
			gpio_request(val, "lcd_pwr_seq");
			gpio_direction_output(val, 0);
		}
		else if (!strncmp(str, "ldo", 3) || !strncmp(str, "dc", 2)) {
			if (resource_find_regulator(&pdev->power_resource, str) >= 0)
				continue;
			dlog_dbg("item resource: power regulator %s\n", str);
			if (strlen(str) >= REGULATOR_NAME_LEN) {
				dlog_err("item resource power regulator name should be less than %d\n",
								REGULATOR_NAME_LEN);
				return -EINVAL;
			}
			val = item_integer(item, 1);
			regp = regulator_get_exclusive(&pdev->dev, str);
			if (IS_ERR(regp)) {
				dlog_info("regulator_get_exclusive failed %ld, try regulator_get\n", PTR_ERR(regp));
				regp = regulator_get(&pdev->dev, str);
				if (IS_ERR(regp)) {
					dlog_err("regulator_get failed %ld\n", PTR_ERR(regp));
					return PTR_ERR(regp);
				}
			}
			pdev->power_resource.regulators[regulator_cnt] = regp;
			strcpy(pdev->power_resource.regulators_name[regulator_cnt], str);
			regulator_set_voltage(regp, 0, 0);
			regulator_force_disable(regp);
			regulator_cnt++;
		}
		else if (sysfs_streq(str, "clk")) {
			if (pdev->clk_src) {
				dlog_warn("item resource: lcd panel clk dumplicate!\n");
				ret = -EINVAL;
				goto release_resource;
			}
			memset(item, 0, 64);
			sprintf(item, DISPLAY_ITEM_PERIPHERAL_CLKSRC, pdev->path, pdev->name);
			if (!item_exist(item)) {
				dlog_err("item %s does NOT exist.\n", item);
				ret = -EINVAL;
				goto release_resource;
			}
			item_string(str, item, 0);
			pdev->clk_src = clk_get_sys(str, str);
			if (IS_ERR(pdev->clk_src)) {
				ret = PTR_ERR(pdev->clk_src);
				dlog_err("get resource clk %s failed %d\n", str, ret);
				goto release_resource;
			}
		}
		else {
			dlog_err("Unrecognized %s param in item %s\n", str, item);
			ret = -EINVAL;
			goto release_resource;
		}
	}

	return 0;

release_resource:
	for (i = 0; pdev->power_resource.gpios[i]; i++)
		gpio_free(pdev->power_resource.gpios[i]);
	for (i = 0; pdev->power_resource.regulators[i]; i++)
		regulator_put(pdev->power_resource.regulators[i]);

	return ret;
}
EXPORT_SYMBOL_GPL(display_peripheral_resource_init);

/* delay in ms */
static void __power_delay(int delay, int en)
{
	if (en) {
		if (delay > 20)
			msleep(delay);
		else
			usleep_range(delay*1000, delay*2000);
	}
	else
		udelay(100);
}

int display_peripheral_sequential_power(struct display_device *pdev, int en)
{
	int i, index;
	int target, val, delay;
	char item[64] = {0};
	char str[64] = {0};
	struct regulator *regp;

	dlog_dbg("%s\n", pdev->name);

	if (pdev->module_type != DISPLAY_MODULE_TYPE_PERIPHERAL) {
		dlog_err("pdev must be peripheral device, logic error\n");
		return -EINVAL;
	}
	
	/* power on/off with sequence */
	for (i = 0; i < DISPLAY_ITEM_PWRSEQ_MAX; i++) {
		memset(item, 0, 64);
		sprintf(item, DISPLAY_ITEM_PERIPHERAL_PWRSEQ, pdev->path, pdev->name,
					en? i : (DISPLAY_ITEM_PWRSEQ_MAX-1-i));
		if (!item_exist(item)) {
			if (en)
				break;
			else
				continue;
		}
		item_string(str, item, 0);
		if (!strncmp(str, "gpio", 4)) {
			target = simple_strtoul(str+4, NULL, 10);
			val = item_integer(item, 1);
			delay = item_integer(item, 2);
			dlog_dbg("Power %s Seq%d: gpio%d -> %d, delay %d ms\n",
					en?"on":"down", (en? i : (DISPLAY_ITEM_PWRSEQ_MAX-1-i)), target, val, delay);
			/* ensure gpio mode */
			imapx_pad_set_mode(target, "output");
			imapx_pad_set_pull(target, "float");
			imapx_pad_set_value(target, en?val:!val);
			gpio_set_value(target, en?val:!val);
			barrier();
			__power_delay(delay, en);
		}
		else if (!strncmp(str, "ldo", 3) || !strncmp(str, "dc", 2)) {
			val = item_integer(item, 1);
			delay = item_integer(item, 2);
			dlog_dbg("Power %s Seq%d: %s -> %d, delay %d ms\n", en?"on":"down", i, str, val, delay);
			index = resource_find_regulator(&pdev->power_resource, str);
			regp = pdev->power_resource.regulators[index];
			if (!regp) {
				dlog_err("regulator NULL pointer\n");
				return -EINVAL;
			}
			if (en) {
				regulator_set_voltage(regp, val, val);
				regulator_enable(regp);
			}
			else {
				regulator_disable(regp);
			}
			__power_delay(delay, en);
		}
		else if (sysfs_streq(str, "clk")) {
			delay = item_integer(item, 1);
			dlog_dbg("Power %s Seq%d: %s -> %s, delay %d ms\n", (en?"on":"down"), i, str, (en?"on":"off"), delay);
			if (en)
				clk_prepare_enable(pdev->clk_src);
			else
				clk_disable_unprepare(pdev->clk_src);
#if defined(CONFIG_ARCH_APOLLO3) || defined(CONFIG_ARCH_Q3F)
			/* for apollo3 and Q3F CLK_OUT pad io control */
			if (sysfs_streq(__clk_get_name(pdev->clk_src), "imap-clk1")) {
				val = ioread32(IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);
				iowrite32(val | ((en?1:0) << 3), IO_ADDRESS(SYSMGR_PAD_BASE) + 0x3f8);
			}
#endif
			__power_delay(delay, en);
		}
		else {
			dlog_err("Unrecognized %s param in item %s\n", str, item);
			return -EINVAL;
		}
		/* backlight is controlled by FB_BLANK_xxx */
	}

	return 0;
}
EXPORT_SYMBOL_GPL(display_peripheral_sequential_power);

static ssize_t path_show(struct bus_type *bus, char *buf)
{
	struct display_device *pdev;
	int i, ret, cnt = 0;
	for (i = 0; i < IDS_PATH_NUM; i++) {
		ret = display_get_device(i, DISPLAY_MODULE_TYPE_PERIPHERAL, &pdev);
		cnt += sprintf(buf, "%d:%d,\n", i, ret?0:pdev->enable);
	}
	return cnt;	
}

static ssize_t path_store(struct bus_type *bus, const char *buf, size_t count)
{
	int path, en;

	sscanf(buf, "%d,%d", &path, &en);

	if (path < 0 || path > IDS_PATH_NUM || (en != 0 && en != 1)) {
		dlog_err("Invalid param path %d, en %d\n", path, en);
		return -EINVAL;
	}

	dlog_dbg("%s path%d\n", en?"enable":"disable", path);

	if (en)
		display_path_enable(path);
	else
		display_path_disable(path);

	return count;
}

static ssize_t log_level_show(struct bus_type *bus, char *buf)
{
	return sprintf(buf, "%d\n", display_log_level);
}

static ssize_t log_level_store(struct bus_type *bus, const char *buf, size_t count)
{
	int level;

	sscanf(buf, "%d", &level);

	if (level < LOG_LEVEL_ERR || level > LOG_LEVEL_IRQ)
		dlog_err("Invalid log level %d\n", level);

	display_log_level = level;

	return count;
}

static struct bus_attribute display_bus_attrs[] = {
	__ATTR(path, 0666, path_show, path_store),
	__ATTR(log_level, 0666, log_level_show, log_level_store),
	__ATTR_NULL,
};


static ssize_t enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct display_device *pdev = to_display_device(dev);
	return sprintf(buf, "%d\n", pdev->enable);
}

static ssize_t enable_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long en;
	struct display_device *pdev = to_display_device(dev);
	struct display_driver *drv = to_display_driver(dev->driver);
	struct display_function *func = &drv->function;
	int i;
	
	ret = kstrtoul(buf, 0, &en);
	if (ret)
		return ret;

	if (en != 0 && en != 1) {
		dlog_err("Invalid param %ld\n", en);
		return -EINVAL;
	}

	if (!func->enable && !func->overlay_enable) {
		dlog_err("(*enable) nor (*overlay_enable) function NOT implemented\n");
		return -EINVAL;
	}

	dlog_dbg("en=%ld\n", en);

	if (func->enable) {
		ret = func->enable(pdev, en);
		if (ret) {
			dlog_err("enable() failed %d\n", ret);
			return ret;
		}
	}
	else {
		for (i = 0; i < IDS_OVERLAY_NUM; i++) {
			ret = func->overlay_enable(pdev, i, en);
			if (ret) {
				dlog_err("overlay_enable(%d, %ld) failed %d\n", i, en, ret);
				return ret;
			}
		}
	}

	return count;
}

static struct device_attribute display_dev_attrs[] = {
	__ATTR(enable, 0666, enable_show, enable_store),
	__ATTR_NULL,
};

static const struct platform_device_id *display_match_id(
			const struct platform_device_id *id,
			struct display_device *pdev)
{
	while (id->name[0]) {
		if (strcmp(pdev->name, id->name) == 0) {
			pdev->id_entry = id;
			return id;
		}
		id++;
	}
	return NULL;
}

/**
 * display_match - bind display device to display driver.
 * @dev: device.
 * @drv: driver.
 *
 * Platform device IDs are assumed to be encoded like this:
 * "<name><instance>", where <name> is a short description of the type of
 * device, like "pci" or "floppy", and <instance> is the enumerated
 * instance of the device, like '0' or '42'.  Driver IDs are simply
 * "<name>".  So, extract the <name> from the display_device structure,
 * and compare it against the name of the driver. Return whether they match
 * or not.
 */
static int display_match(struct device *dev, struct device_driver *drv)
{
	struct display_device *pdev = to_display_device(dev);
	struct display_driver *pdrv = to_display_driver(drv);

	/* Then try to match against the id table */
	if (pdrv->id_table)
		return display_match_id(pdrv->id_table, pdev) != NULL;

	/* fall-back to driver name match */
	return (strcmp(pdev->name, drv->name) == 0);
}

#ifdef CONFIG_PM_SLEEP

static int display_legacy_suspend(struct device *dev, pm_message_t mesg)
{
	struct display_driver *pdrv = to_display_driver(dev->driver);
	struct display_device *pdev = to_display_device(dev);
	int ret = 0;

	if (dev->driver && pdrv->suspend)
		ret = pdrv->suspend(pdev, mesg);

	return ret;
}

static int display_legacy_resume(struct device *dev)
{
	struct display_driver *pdrv = to_display_driver(dev->driver);
	struct display_device *pdev = to_display_device(dev);
	int ret = 0;

	if (dev->driver && pdrv->resume)
		ret = pdrv->resume(pdev);

	return ret;
}

#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_SUSPEND

int display_pm_suspend(struct device *dev)
{
	struct device_driver *drv = dev->driver;
	int ret = 0;

	if (!drv)
		return 0;

	if (drv->pm) {
		if (drv->pm->suspend)
			ret = drv->pm->suspend(dev);
	} else {
		ret = display_legacy_suspend(dev, PMSG_SUSPEND);
	}

	return ret;
}

int display_pm_resume(struct device *dev)
{
	struct device_driver *drv = dev->driver;
	int ret = 0;

	if (!drv)
		return 0;

	if (drv->pm) {
		if (drv->pm->resume)
			ret = drv->pm->resume(dev);
	} else {
		ret = display_legacy_resume(dev);
	}

	return ret;
}

static int display_pm_nc(struct notifier_block *nb,
			unsigned long action, void *data)
{
	int i;
	
	for (i = 0; i < IDS_PATH_NUM; i++) {
		if (path_enable[i]) {
			if (action == PM_SUSPEND_PREPARE)
				display_path_ctrl(i, 0);
			else if (action == PM_POST_SUSPEND)
				display_path_ctrl(i, 1);
		}
	}

	// TODO: To be implement, basic suspend not available on apollo series chip currently
	
	return 0;
}

static struct notifier_block display_pm_nb = {
	.notifier_call = display_pm_nc,
};

#endif /* CONFIG_SUSPEND */

static const struct dev_pm_ops display_dev_pm_ops = {
	.runtime_suspend = pm_generic_runtime_suspend,
	.runtime_resume = pm_generic_runtime_resume,
	.runtime_idle = pm_generic_runtime_idle,
	.suspend = display_pm_suspend,
	.resume = display_pm_resume,
};

struct bus_type display_bus_type = {
	.name		= "display",
	.bus_attrs = display_bus_attrs,
	.dev_attrs	= display_dev_attrs,
	.match		= display_match,
	.pm			= &display_dev_pm_ops,
};
EXPORT_SYMBOL_GPL(display_bus_type);

int __init display_bus_init(void)
{
	int error;

	error = device_register(&display_bus);
	if (error)
		return error;
	error =  bus_register(&display_bus_type);
	if (error)
		device_unregister(&display_bus);

#ifdef CONFIG_SUSPEND
	register_pm_notifier(&display_pm_nb);
#endif

	return error;
}
