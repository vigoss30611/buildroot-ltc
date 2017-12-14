/********************************************************
** linux-2.6.31.6/arch/arm/plat-imap/mem_reserve.c
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** Use of Infotm's code is governed by terms and conditions
** stated in the accompanying licensing statement.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Author:
**     Raymond Wang   <raymond.wang@infotmic.com.cn>
**
** Revision History:
**     1.0  12/17/2009    Raymond Wang
**     1.1  12/17/2009    remove x200 token, these code will be
**                        used on every imap platforms.
**  leo@2012/09/22: add pmm.reserve.size item support for RESERVEMEM_DEV_PMM.
**  leo@2013/05/09: add pmm size auto-calc, it should work with items.
************************************************************/

#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/swap.h>
#include <asm/setup.h>
#include <linux/io.h>
#include <mach/memory.h>
#include <mach/items.h>
#include <linux/export.h>
#include <mach/mem-reserve.h>
#include "core.h"

static struct imap_reservemem_device reservemem_devs[RESERVEMEM_DEV_MAX] = {
	{
	 .id = RESERVEMEM_DEV_ETH,
	 .name = "iMAP_Ethernet",

#ifdef CONFIG_IMAP_RESERVEMEM_SIZE_ETH
	 .size = CONFIG_IMAP_RESERVEMEM_SIZE_ETH * SZ_1K,
#else
	 .size = 0,
#endif
	 .paddr = 0,
	 },
	{
	 .id = RESERVEMEM_DEV_MEMALLOC,
	 .name = "memalloc",
#ifdef CONFIG_IMAP_MEMALLOC_SYSTEM_RESERVE_SIZE
	 .size = CONFIG_IMAP_MEMALLOC_SYSTEM_RESERVE_SIZE * SZ_1M,
#else
	 .size = 0,
#endif /* CONFIG_IMAP_MEMALLOC_SYSTEM_RESERVE_SIZE */
	 .paddr = 0,
	 },
	{
	 .id = RESERVEMEM_DEV_PMM,
	 .name = "imapx-pmm",
#ifdef CONFIG_PMM_RESERVE_MEM_SIZE_MB
	 .size = CONFIG_PMM_RESERVE_MEM_SIZE_MB * SZ_1M,
#else
	 .size = 0,
#endif
	 .paddr = 0,
	 },
};

static struct imap_reservemem_device *get_reservemem_device(int dev_id)
{
	struct imap_reservemem_device *dev = NULL;
	int i, found;

	if (dev_id < 0 || dev_id >= RESERVEMEM_DEV_MAX)
		return NULL;

	i = 0;
	found = 0;
	while (!found && (i < RESERVEMEM_DEV_MAX)) {
		dev = &reservemem_devs[i];
		if (dev->id == dev_id)
			found = 1;
		else
			i++;
	}

	if (!found)
		dev = NULL;

	return dev;
}

dma_addr_t imap_get_reservemem_paddr(int dev_id)
{
	struct imap_reservemem_device *dev;

	dev = get_reservemem_device(dev_id);
	if (!dev) {
		pr_err("invalid device!\n");
		return 0;
	}

	if (!dev->paddr) {
		pr_err("no memory for %s\n", dev->name);
		return 0;
	}

	return dev->paddr;
}
EXPORT_SYMBOL(imap_get_reservemem_paddr);

size_t imap_get_reservemem_size(int dev_id)
{
	struct imap_reservemem_device *dev;

	dev = get_reservemem_device(dev_id);
	if (!dev) {
		pr_err("invalid device!\n");
		return 0;
	}

	return dev->size;
}
EXPORT_SYMBOL(imap_get_reservemem_size);

static int calc_pmm_reserved_size(void)
{
#define BYTES_UPALIGN_TO_1M(n)     ((n + (SZ_1M - 1)) & (~(SZ_1M - 1)))

#if 0
	int size = -1, fb_size = 0, hdmi_size = 0, camera_size =
	    0, camera_front_size = 0, camera_rear_size = 0;
	if (item_exist("ids.fb.width") && item_exist("ids.fb.height")
	    && item_exist("ids.fb.pixfmt")) {
		fb_size =
		    item_integer("ids.fb.width",
				 0) * item_integer("ids.fb.height",
						   0) *
		    (item_equal("ids.fb.pixfmt", "888", 0) ? 4 : 2);
		fb_size *= 4;
		fb_size = BYTES_UPALIGN_TO_1M(fb_size);
	} else {
		goto next;
	}

	/* Assume only support one extdev, and it's HDMI.*/
	if (item_exist("ids.ext.dev.num")
	    && (item_integer("ids.ext.dev.num", 0) == 1)
	    && item_exist("ids.ext.dev.prefer")
	    && (item_integer("ids.ext.dev.prefer", 0) == 0)
	    && item_exist("ids.ext.dev0.interface")
	    && item_equal("ids.ext.dev0.interface", "hdmi", 0)) {
		hdmi_size = 1920 * 1080 * 2 * 2;
		hdmi_size = BYTES_UPALIGN_TO_1M(hdmi_size);
	}

	if (item_exist("camera.front.interface")
	    || (item_exist("camera.rear.interface"))) {
		if (item_exist("camera.front.interface")) {
			if (item_exist("camera.front.resolution")) {
				camera_front_size =
				    item_integer("camera.front.resolution", 0);
			} else {
				goto next;
			}
		}

		if (item_exist("camera.rear.interface")) {
			if (item_exist("camera.rear.resolution")) {
				camera_rear_size =
				    item_integer("camera.rear.resolution", 0);
			} else {
				goto next;
			}
		}

		camera_size =
		    camera_front_size >
		    camera_rear_size ? camera_front_size : camera_rear_size;
		camera_size *= 10000;
		camera_size = ((camera_size * 3) >> 1) * 7 + (6 << 20);
		camera_size = BYTES_UPALIGN_TO_1M(camera_size);
	}

	/* expand 8MB for safety.*/
	size = fb_size + hdmi_size + camera_size + (8 << 20);
	pr_info("pmm calc: fb_size=%d, hdmi_size=%d, camera_size=%d, size=%d\n",
	       fb_size, hdmi_size, camera_size, size);
	goto out;

next:
	if (item_exist("pmm.reserve.size")) {
		size = item_integer("pmm.reserve.size", 0) * SZ_1M;
		pr_info("pmm.reserve.size=%d\n", size);
	}

out:
	return size;
#else
	int size = -1;
	if (item_exist("config.media.pmm")) {
		size = item_integer("config.media.pmm", 0) * SZ_1M;
		pr_info("config.media.pmm=%d\n", size);
	} else if (item_exist("pmm.reserve.size")) {
		size = item_integer("pmm.reserve.size", 0) * SZ_1M;
		pr_info("pmm.reserve.size=%d\n", size);
	}

	return size;
#endif
}

void __init coronampw_mem_reserve(void)
{
	struct imap_reservemem_device *dev;
	int i, tmp;

	for (i = 0; i < sizeof(reservemem_devs) / sizeof(reservemem_devs[0]);
	     i++) {
		dev = &reservemem_devs[i];

		/* we need aujust the order to put
			the item access function front of
		*/
		if (dev->id == RESERVEMEM_DEV_PMM) {
			tmp = calc_pmm_reserved_size();
			if (tmp != -1)
				dev->size = tmp;
			pr_info("pmm reserved size=%dMB\n", dev->size >> 20);
		}

		if (dev->size > 0) {
			dev->paddr = virt_to_phys(alloc_bootmem_low(dev->size));
			pr_info(
			       "imap: %luMB memory reserved for %s.\n",
			       (unsigned long)(dev->size >> 20), dev->name);
		}
	}
}
