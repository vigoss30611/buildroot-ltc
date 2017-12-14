/* display register access driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/display_device.h>
#include <mach/items.h>
#include <mach/rballoc.h>

#include "display_register.h"
#include "display_items.h"

#define DISPLAY_MODULE_LOG		"init"


static int __init display_init(void)
{
	char name[64] = {0}; 
	char default_p[64] = {0};
	int i, peripheral_num = 0;
	int ret = 0;
	struct display_device *pdev;
	struct display_function *func;
	int logo_width = 0, logo_height = 0;
	int logo_size;
	enum display_logo_scale logo_scale = 0;
	int logo_background = 0;

	if (!display_support())
		return 0;

	/* fill logo buffer */
	display_get_function(0, DISPLAY_MODULE_TYPE_FB, &func, &pdev);

	pdev->logo_virt_addr = rbget(DISPLAY_UBOOT_LOGO_ADDR);
	if (pdev->logo_virt_addr) {
		pdev->uboot_logo = 1;
		dlog_info("using uboot logo\n");
	}
	else {
		pdev->uboot_logo = 0;
		pdev->kernel_logo = kernel_logo();
		if (pdev->kernel_logo) {

#if defined(CONFIG_DISPLAY2_LOGO_SCALE_NONSCALE)
			logo_scale = DISPLAY_LOGO_SCALE_NONSCALE;
#elif defined(CONFIG_DISPLAY2_LOGO_SCALE_FIT)
			logo_scale = DISPLAY_LOGO_SCALE_FIT;
#elif defined(CONFIG_DISPLAY2_LOGO_SCALE_STRETCH)
			logo_scale = DISPLAY_LOGO_SCALE_STRETCH;
#else
			logo_scale = DISPLAY_LOGO_SCALE_NONSCALE;
#endif

#if defined(CONFIG_DISPLAY2_LOGO_BACKGROUND)
			logo_background = simple_strtoul(CONFIG_DISPLAY2_LOGO_BACKGROUND, NULL, 16);
#else
			logo_background = 0;
#endif
			dlog_dbg("logo scale is %d, background color is 0x%X\n", logo_scale, logo_background);

			logo_width = logo_get_width();
			logo_height = logo_get_height();
			logo_size = logo_width * logo_height * 4;
			pdev->logo_virt_addr = kzalloc(logo_size, GFP_KERNEL|GFP_DMA);
			if (!pdev->logo_virt_addr) {
				dlog_err("dma alloc failed\n");
				return -ENOMEM;
			}

			ret = fill_logo_buffer((unsigned int)pdev->logo_virt_addr);
			if (ret) {
				dlog_err("fill logo buffer failed %d\n", ret);
				return ret;
			}
		}
		else
			dlog_info("no kernel logo\n");
	}

	/* init and enable path */
	for (i = 0; i < IDS_PATH_NUM; i++) {
		sprintf(name, DISPLAY_ITEM_PERIPHERAL_DEFAULT, i);
		if (!item_exist(name)) {
			dlog_info("path%d: default disable\n", i);
			continue;
		}
		item_string(default_p, name, 0);
		dlog_info("path%d: default peripheral = %s\n", i, default_p);
		ret = display_select_peripheral(i, default_p);
		if (ret) {
			dlog_err("failed to set path%d default peripheral %s\n", i, default_p);
			return ret;
		}
		peripheral_num += 1;
		if (!pdev->uboot_logo && pdev->kernel_logo) {
			ret = display_set_logo(i, virt_to_phys(pdev->logo_virt_addr), logo_width, logo_height, 
													logo_scale, logo_background);
			if (ret) {
				dlog_err("set logo failed %d\n", ret);
				return ret;
			}
			ret = display_path_enable(i);
			if (ret) {
				dlog_err("enabling path%d failed\n", i);
				continue;
			}
			dlog_info("path%d enabled\n", i);
		}
	}

	if (!peripheral_num)
		dlog_info("no peripheral device enabled\n");

	return 0;
}

device_initcall_sync(display_init);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display init driver");
MODULE_LICENSE("GPL");
