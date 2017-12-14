/**
******************************************************************************
@file sys_device_memmapped.c

@brief discover and register platform device

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/io.h>

#include <img_defs.h>
#include <img_errors.h>
#include <img_types.h>
#include <ci_internal/sys_device.h>

#if defined(IMGVIDEO_ALLOC)

#include <sysdev_utils.h>
#include <sysmem_utils.h>

#endif

#define CI_DEVICE_NAME "isp_ci_platform"
/*
 * CI platform Linux device resources.
 * MUST be modified to reflect specific platform configuration.
 */
static struct resource ci_device_resources[] = {
#ifdef INFOTM_ISP
	{
		.name = "reg_base_addr",
		.start = IMAP_ISP_BASE,
		.end = IMAP_ISP_BASE + IMAP_ISP_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	#if !defined(INFOTM_CI_MEM_FR_ON)
	{
		.name = "mem_base_addr",
		.start = 0x50000000 - 0x8600000, //for carve out memory marchine.
		.end = 0x50000000 - 0x3600000,
		.flags = IORESOURCE_MEM,
	},
	#endif //INFOTM_CI_MEM_FR_ON
	{
		.name = "irq",
		.start = 80,
		.flags = IORESOURCE_IRQ,
	},
    #ifdef INFOTM_HW_AWB_METHOD
	{
		.name = "hw_awb_irq",
		.start = 81,
		.flags = IORESOURCE_IRQ,
	}
    #endif //INFOTM_HW_AWB_METHOD
#else
	{
		.name = "reg_base_addr",
		.start = 0x00,
		.end = 0x10,
		.flags = IORESOURCE_MEM,
	},
	{
		.name = "mem_base_addr",
		.start = 0x1000000,
		.end = 0x2000000,
		.flags = IORESOURCE_MEM,
	},
	{
		.name = "irq",
		.start = 16,
		.flags = IORESOURCE_IRQ,
	}
#endif //INFOTM_ISP	
};

/*
 * imgvideo functions
 */
#if defined(IMGVIDEO_ALLOC)

// defined in sys_mem_imgvideo.c
IMG_PHYSADDR Platform_paddr_to_devpaddr(struct SYSDEVU_sInfo *sysdev,
                                      IMG_PHYSADDR paddr);

IMG_RESULT FELIXKM_fnDevRegister (DMANKM_sDevRegister *psDevRegister)
{
    psDevRegister->ui32ConnFlags    = DMAN_CFLAG_SHARED;
    psDevRegister->pfnDevInit       = NULL;
    psDevRegister->pfnDevDeinit     = NULL;
    psDevRegister->pfnDevDisconnect = NULL;
    psDevRegister->pfnDevConnect    = NULL;
    psDevRegister->pfnDevKmHisr     = NULL;
    psDevRegister->pfnDevKmLisr     = NULL;
    psDevRegister->pfnDevPowerPreS5  = NULL;
    psDevRegister->pfnDevPowerPostS0 = NULL;
    return IMG_SUCCESS;
}

static SYSDEVU_sInfo sysdevu_device = {
    IMG_NULL,
    SYS_DEVICE(CI_DEVICE_NAME, FELIX, IMG_FALSE)
};
static struct SYSDEV_ops sysdevu_ops = {
    .free_device = NULL,
    .resume_device = NULL,
    .suspend_device = NULL,
    .paddr_to_devpaddr = Platform_paddr_to_devpaddr
};

#endif // IMGVIDEO_ALLOC

/*
 * Linux device driver for CI platform
 */

struct ci_drvdata {
	int irq;
	void __iomem *reg_base;
	void __iomem *mem_base;
};

static int ci_platform_probe(struct platform_device *pdev)
{
	struct ci_drvdata *ci;
	struct resource *r;
	int ret = 0;

	printk(KERN_DEBUG "Probing platform device %s...\n", pdev->name);

	ci = kzalloc(sizeof(*ci), GFP_KERNEL);
	if (ci == NULL) {
		ret = -ENOMEM;
		goto err_ci_alloc_failed;
	}

	platform_set_drvdata(pdev, ci);

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "reg_base_addr");
	if (r == NULL) {
		ret = -ENODEV;
		goto err_no_reg_io_base;
	}

	/* Registers */
#ifdef INFOTM_ISP
	ci->reg_base = ioremap_nocache(r->start, r->end - r->start);
#else
	ci->reg_base = ioremap(r->start, r->end - r->start);
#endif //INFOTM_ISP

#if !defined(INFOTM_CI_MEM_FR_ON)
	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mem_base_addr");
	if (r == NULL) {
		ret = -ENODEV;
		goto err_no_mem_io_base;
	}

	/* Carveout memory */
	ci->mem_base = ioremap(r->start, r->end - r->start);
#endif //INFOTM_CI_MEM_FR_ON

	/* Interrupt line */
	ci->irq = platform_get_irq(pdev, 0);
    
    return ret;
#if !defined(INFOTM_CI_MEM_FR_ON)
err_no_mem_io_base:
#endif //INFOTM_CI_MEM_FR_ON
	iounmap(ci->reg_base);
err_no_reg_io_base:
	kfree(ci);
err_ci_alloc_failed:
	return ret;
}

static int ci_platform_remove(struct platform_device *pdev)
{
	struct ci_drvdata *ci = platform_get_drvdata(pdev);

	iounmap(ci->reg_base);
	iounmap(ci->mem_base);
	kfree(ci);

	return 0;
}

static int ci_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret = 0;
	SYS_DEVICE *sysDevice = pdev->dev.platform_data;

	dev_info(&pdev->dev, "%s state:%d\n", __func__, state.event);

	if (sysDevice && sysDevice->suspend)
	{
		ret = sysDevice->suspend(sysDevice, state);
	}

	return ret;
}

static int ci_resume(struct platform_device * pdev)
{
	int ret = 0;
	SYS_DEVICE *sysDevice = pdev->dev.platform_data;

	dev_info(&pdev->dev, "%s\n", __func__);

	if (sysDevice && sysDevice->resume)
	{
		ret = sysDevice->resume(sysDevice);
	}

	return ret;
}

static struct platform_driver ci_platform_driver = {
	.probe = ci_platform_probe,
	.remove = ci_platform_remove,
	.suspend = ci_suspend,
	.resume = ci_resume,
	.driver = {
		.name = CI_DEVICE_NAME
	}
};

/*
 * Functions accessible from outside
 */

IMG_RESULT SYS_DevRegister(SYS_DEVICE *sysDevice)
{
	int ret = IMG_SUCCESS;
	struct ci_drvdata *ci;
	struct resource *res;
	struct platform_device *ci_device;

	printk(KERN_DEBUG "CI platform ISP driver registration.\n");

	ret = platform_driver_register(&ci_platform_driver);
	if (ret) {
		printk(KERN_ERR "platform_driver_register failed!\n");
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto err_driver_register;
	}

	ci_device = platform_device_alloc(CI_DEVICE_NAME, -1);
	if (!ci_device) {
		ret = IMG_ERROR_OUT_OF_MEMORY;
		goto err_platform_device_alloc;
	}

	ret = platform_device_add_resources(
			ci_device, ci_device_resources, ARRAY_SIZE(ci_device_resources));
	if (ret) {
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto err_device_add_resources;
	}

	ret = platform_device_add_data(ci_device, sysDevice, sizeof(sysDevice));
	if (ret) {
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto err_device_add_platdata;
	}

	ret = platform_device_add(ci_device);
	if (ret) {
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto err_device_add;
	}

	ci = platform_get_drvdata(ci_device);
	if (!ci) {
		ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		goto err_get_drvdata;
	}

	/* Store CI platform device in sys device */
	sysDevice->data = ci_device;

	res = platform_get_resource_byname(
			ci_device, IORESOURCE_MEM, "reg_base_addr");
	if (!res) {
		ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		goto err_get_resource;
	}

	/* Register space setup */
	sysDevice->uiRegisterSize = res->end - res->start;
	sysDevice->uiRegisterPhysical = res->start;
	sysDevice->uiRegisterCPUVirtual = (IMG_UINTPTR)ci->reg_base;

#if !defined(INFOTM_CI_MEM_FR_ON)
	res = platform_get_resource_byname(
			ci_device, IORESOURCE_MEM, "mem_base_addr");
	if (!res) {
		ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		goto err_get_resource;
	}

	/* Memory space setup */
    sysDevice->uiDevMemoryPhysical = res->start;
	sysDevice->uiMemorySize = res->end - res->start;
	sysDevice->uiMemoryPhysical = res->start;
	sysDevice->uiMemoryCPUVirtual = (IMG_UINTPTR)ci->mem_base;

#endif //INFOTM_CI_MEM_FR_ON
	/* Board register setup */
	sysDevice->uiBoardSize = 0;
	sysDevice->uiBoardPhysical = 0;
	sysDevice->uiBoardCPUVirtual = 0;

	/* IRQ setup */
	sysDevice->irq_line = ci->irq;

#if defined(IMGVIDEO_ALLOC)
    {
        IMG_RESULT ui32Result;
    
        // populate sysdevu_device
        SYSDEVU_SetDevMap(&sysdevu_device, 0, 0, 0,
                        (IMG_PHYSADDR)sysDevice->uiMemoryPhysical,
                        (IMG_UINT32 *)sysDevice->uiMemoryCPUVirtual,
                        (IMG_UINT32)sysDevice->uiMemorySize,
                        0);
        SYSDEVU_SetDeviceOps(&sysdevu_device, &sysdevu_ops);
        sysdevu_device.pPrivate = sysDevice;

    #if defined(IMGVIDEO_ION)
        ui32Result = SYSMEMKM_AddIONMemory(&sysdevu_device, &sysdevu_device.sMemPool);
    #elif defined(FPGA_BUS_MASTERING)
        ui32Result = SYSMEMKM_AddSystemMemory(&sysdevu_device, &sysdevu_device.sMemPool);
    #else
        ui32Result = SYSMEMKM_AddCarveoutMemory(&sysdevu_device,
                                                (IMG_UINTPTR)sysdevu_device.pui32KmMemBase,
                                                sysdevu_device.paPhysMemBase,
                                                sysdevu_device.ui32MemSize,
                                                &sysdevu_device.sMemPool);
    #endif
        if (ui32Result != IMG_SUCCESS)
        {
            printk(KERN_ERR "SYSMEMKM_AddCarveoutMemory failed %d\n", ui32Result);
            goto err_get_resource;
        }

        ui32Result = SYSDEVU_RegisterDevice(&sysdevu_device);
        if (ui32Result != IMG_SUCCESS)
        {
            printk(KERN_ERR "SYSDEVU_RegisterDevice failed %d\n", ui32Result);
            goto sysdevu_register_error;
        }

        sysDevice->data = &sysdevu_device;
    }
#endif // IMGVIDEO_ALLOC
    
	/* Call probe success callback */
	if (!sysDevice->probeSuccess)
	{
		ret = IMG_ERROR_INVALID_PARAMETERS;
		goto err_no_callback;
	}

	printk(KERN_DEBUG "Calling client's probe.\n");
	if (sysDevice->probeSuccess(sysDevice))
	{
		/* Error - this is not our device */
		printk(KERN_WARNING "Driver found but refused device!\n");
		ret = IMG_ERROR_DEVICE_UNAVAILABLE;
		goto err_callback_failed;
	}

	/* Clear interrupts if were any */
	SYS_DevClearInterrupts(NULL);

	printk(KERN_DEBUG "Felix ISP driver initialized.!\n");
	return ret;

err_callback_failed:
err_no_callback:
#if defined(IMGVIDEO_ALLOC)
    sysDevice->data = NULL;
    SYSDEVU_UnRegisterDevice(&sysdevu_device);
#endif
#if defined(IMGVIDEO_ALLOC)
sysdevu_register_error:
    SYSMEMU_RemoveMemoryHeap(sysdevu_device.sMemPool);
#endif
err_get_resource:
err_get_drvdata:
err_device_add:
err_device_add_platdata:
err_device_add_resources:
	platform_device_put(ci_device);
err_platform_device_alloc:
	platform_driver_unregister(&ci_platform_driver);
err_driver_register:
	return ret;
}

IMG_RESULT SYS_DevClearInterrupts(SYS_DEVICE *sysDevice)
{
	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRemove(SYS_DEVICE *sysDevice)
{
	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevDeregister(SYS_DEVICE *sysDevice)
{
#ifndef INFOTM_ISP
	struct platform_device *ci_device = (struct platform_device*)sysDevice->data;
	struct platform_driver *ci_drv = platform_get_drvdata(ci_device);
#if defined(IMGVIDEO_ALLOC)
    SYSDEVU_sInfo *psSysdev = sysDevice->data;
#endif

	printk(KERN_DEBUG "Unregistering CI platform driver...\n");
    
#if defined(IMGVIDEO_ALLOC)
    if (psSysdev)
    {
        SYSDEVU_UnRegisterDevice(psSysdev);
        SYSMEMU_RemoveMemoryHeap(psSysdev->sMemPool);
    }
#endif

	platform_device_put(ci_device);
	platform_driver_unregister(ci_drv);
	kfree(ci_device);
#else //INFOTM_ISP
    //Fixed Ticket 98374 - rmmod Felix.ko

	struct platform_device *ci_device = (struct platform_device*)sysDevice->data;
	struct platform_driver *ci_drv = &ci_platform_driver;
	#if defined(IMGVIDEO_ALLOC)
    SYSDEVU_sInfo *psSysdev = sysDevice->data;
	#endif //IMGVIDEO_ALLOC

	printk(KERN_INFO "Unregistering CI platform driver (no free)...\n");
    
	#if defined(IMGVIDEO_ALLOC)
    if (psSysdev)
    {
        SYSDEVU_UnRegisterDevice(psSysdev);
        SYSMEMU_RemoveMemoryHeap(psSysdev->sMemPool);
    }
	#endif //IMGVIDEO_ALLOC

    if (ci_device)
    {
	    platform_device_unregister(ci_device);
        sysDevice->data = NULL;
    }
    else
    {
        printk(KERN_WARNING "no platform_device!\n");
    }
    if (ci_drv)
    {
    	platform_driver_unregister(ci_drv);
    }
    else
    {
        printk(KERN_WARNING "no platform_driver!\n");
    }
#endif //INFOTM_ISP
	

	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRequestIRQ(SYS_DEVICE *sysDevice, const char *name)
{
    int ret;
    ret = request_threaded_irq(sysDevice->irq_line,
        sysDevice->irqHardHandle, sysDevice->irqThreadHandle,
        IRQF_SHARED, name, sysDevice->handlerParam);
	if (ret)
		return IMG_ERROR_UNEXPECTED_STATE;
	else
		return IMG_SUCCESS;
}

IMG_RESULT SYS_DevFreeIRQ(SYS_DEVICE *sysDevice)
{
	free_irq(sysDevice->irq_line, sysDevice->handlerParam);
	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevPowerControl(SYS_DEVICE *sysDevice, IMG_BOOL8 bDeviceActive)
{
	return IMG_SUCCESS;
}
