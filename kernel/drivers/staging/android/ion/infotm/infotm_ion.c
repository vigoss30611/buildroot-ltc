/*
 * drivers/gpu/infotm/infotm_ion.c
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/dma-buf.h>
#include <linux/scatterlist.h>

#include "../ion.h"
#include "../ion_priv.h"

#include <mach/devices.h>

/* -----------------------------------------------
 * Change Log:
 * v2013.12.25: first commit by shaft.huang@infotm
 * v2013.12.27: 1) ion_dmmu_xxx change name to imapx_dmmu_xxx;
 *              2) add ref_count in infotm_dmmu_device for imapx_dmmu_init/deinit manage
 * v2014.01.18: mapping to a universal buf_uid when ion_buffer create, used by dev_mmu driver             
 * v2014.02.08: use ion_heap_cma instead of ion_heap_system_contig to alloc contig memory
 * 
 * -----------------------------------------------
 */
#define INFOTM_ION_VERSION "v2014.02.08"

#define INFOTM_ION_IOC_GET_INFOS 3
#define SIZE_TO_PAGE(size)  ((size) >> PAGE_SHIFT)

static struct ion_platform_heap infotm_ion_heaps[] = {
		{
			.type = ION_HEAP_TYPE_SYSTEM,
			.name = "ion_system_heap",
			.id = ION_HEAP_TYPE_SYSTEM,
		},
#if 0
		{
			.type = ION_HEAP_TYPE_SYSTEM_CONTIG,
			.name = "ion_system_contig_heap",
			.id = ION_HEAP_TYPE_SYSTEM_CONTIG,
		},
		{
			.type = ION_HEAP_TYPE_CARVEOUT,
			.name = "ion_carveout_heap",
			.size = SZ_64M,
			.id = ION_HEAP_TYPE_CARVEOUT,
		},
#endif
		{
			.type = ION_HEAP_TYPE_DMA,
			.name = "ion_dma_mem_heap",
			.id = ION_HEAP_TYPE_DMA,
		},
};

static struct ion_platform_data infotm_ion_data = {
	.nr = 2,
	.heaps = infotm_ion_heaps
};

struct infotm_ion_buffer_data {
	int fd;
	int buf_uid;
	unsigned int dev_addr;
	unsigned int phy_addr;
	size_t size;
	size_t num_pages;
};

struct ion_device *ion_infotm;
struct platform_device *infotm_ion_platform_device = NULL;

static int num_heaps;
static struct ion_heap **heaps;

//#define dbg_infotm_ion(cond, exp)   if(cond) exp
#define dbg_infotm_ion(cond, exp)

#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT
// alloc memory for kernel usage
int infotm_kernel_mem_alloc(size_t len, size_t align, unsigned int flags, void **vaddr)
{
	if (ion_infotm==NULL)
		return -1;

	return ion_alloc_kernel_mem((void*)ion_infotm, len, align, flags, vaddr);
}
EXPORT_SYMBOL(infotm_kernel_mem_alloc);

void infotm_kernel_mem_free(int buf_uid)
{
	if (buf_uid >= 0)
		ion_free_kernel_mem(buf_uid);
}
EXPORT_SYMBOL(infotm_kernel_mem_free);
#endif

static long infotm_ion_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg)
{
	struct infotm_ion_buffer_data *data;
	int ret;
	data = (struct infotm_ion_buffer_data *)arg;

    dbg_infotm_ion(1, printk("infotm_ion_ioctl(%d)\n", cmd));

	switch (cmd) {
	case INFOTM_ION_IOC_GET_INFOS:
		{
			struct ion_handle *handle;
			ion_phys_addr_t phys;
			size_t len;
			size_t numPages = 1; //TODO:
			int buf_uid;
			
			handle = ion_import_dma_buf(client, data->fd);
			if (IS_ERR_OR_NULL(handle))
				return -EFAULT;

			ret = ion_phys(client, handle, &phys, &len);
			if (ret<0) {
                ion_free(client, handle);
				return ret;
            }

#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT
			buf_uid = ion_get_buf_uid(client, handle);
#else
			buf_uid = -1;
#endif

            data->buf_uid = buf_uid;
			data->phy_addr = phys;
			data->size = len;
			data->num_pages = numPages;
			pr_debug("INFOTM_ION_IOC_GET_INFOS: fd=%d, phy_addr=0x%x\n", data->fd, data->phy_addr);
            
            ion_free(client, handle);
		}
		break;

	default:
		return -EFAULT;
	}
	return 0;
}

static int infotm_ion_probe(struct platform_device *pdev)
{
	struct ion_platform_data *pdata = pdev->dev.platform_data;
	int err;
	int i;

	dbg_infotm_ion(1, printk("infotm_ion_probe!\n"));

	num_heaps = pdata->nr;

	heaps = kzalloc(sizeof(struct ion_heap *) * pdata->nr, GFP_KERNEL);
	if (!heaps)
		return -ENOMEM;

	ion_infotm = ion_device_create(infotm_ion_ioctl);
	if (IS_ERR_OR_NULL(ion_infotm)) {
		kfree(heaps);
		return PTR_ERR(ion_infotm);
	}

	/* create the heaps as specified in the board file */
	for (i = 0; i < num_heaps; i++) {
		struct ion_platform_heap *heap_data = &pdata->heaps[i];

		printk("infotm ion creat No.%d heap: %p, type=%d\n", i, heap_data, heap_data->type);
		heaps[i] = ion_heap_create(heap_data);
		if (IS_ERR_OR_NULL(heaps[i])) {
			err = PTR_ERR(heaps[i]);
			goto err;
		}
		ion_device_add_heap(ion_infotm, heaps[i]);
	}
	platform_set_drvdata(pdev, ion_infotm);
	infotm_ion_platform_device = pdev;

	printk("ION device create OK!\n");
	return 0;
err:
	for (i = 0; i < num_heaps; i++) {
		if (heaps[i])
			ion_heap_destroy(heaps[i]);
	}
	kfree(heaps);
	return err;
}

int infotm_ion_remove(struct platform_device *pdev)
{
	struct ion_device *idev = platform_get_drvdata(pdev);
	int i;

	ion_device_destroy(idev);
	for (i = 0; i < num_heaps; i++)
		ion_heap_destroy(heaps[i]);
	kfree(heaps);
	infotm_ion_platform_device = NULL;
	return 0;
}

static struct platform_driver infotm_ion_driver = {
	.probe = infotm_ion_probe,
	.remove = infotm_ion_remove,
	.driver = { .name = "infotm-ion" }
};

static int __init infotm_ion_init(void)
{
	int err=0;

	printk("Init Infotm ION driver %s\n", INFOTM_ION_VERSION);

	err = platform_device_add_data(&infotm_ion_device, &infotm_ion_data, sizeof(infotm_ion_data));

	if (0 !=err ) {
		return err;
	}

	err = platform_driver_register(&infotm_ion_driver);
	if (0 != err) {
		platform_device_unregister(&infotm_ion_device);
		return err;
	}

	printk("Infotm ION driver %s register done!\n", INFOTM_ION_VERSION);
	return 0;
}

static void __exit infotm_ion_exit(void)
{
	platform_driver_unregister(&infotm_ion_driver);
	platform_device_unregister(&infotm_ion_device);
}

module_init(infotm_ion_init);
module_exit(infotm_ion_exit);

