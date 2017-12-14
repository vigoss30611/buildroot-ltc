/**
******************************************************************************
@file sys_device_transif_kernel.c

@brief discovers and register platform device required by transif

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

#define PLATFORM_DEVICE_NAME "felix_ci_platform"

struct SYS_TRANSIF_DEVICE {
    struct platform_driver sDriver;

    int module_irq;
    void * reg_base_addr;
    unsigned int module_reg_base;
    unsigned int module_reg_size;
    bool device_detected;

    SYS_DEVICE *pSysDevice;
};

int ci_platform_driver_probe(struct platform_device * device);

static struct SYS_TRANSIF_DEVICE g_TRANSIF_driver = {
    .sDriver = {
        .driver = {
            .name = PLATFORM_DEVICE_NAME,
            .owner = THIS_MODULE,
        },
        .probe = ci_platform_driver_probe
    },
    .module_irq = -1,
    .reg_base_addr = NULL,
    .module_reg_base = 0,
    .module_reg_size = 0,
    .device_detected = IMG_FALSE,
    .pSysDevice = NULL
};

int ci_platform_driver_probe(struct platform_device * device) {
    printk(KERN_DEBUG "Probing platform device %s...\n", device->name);

    // registers
    IMG_ASSERT(device->resource[0].flags == IORESOURCE_MEM);
    g_TRANSIF_driver.module_reg_base = device->resource[0].start;
    g_TRANSIF_driver.module_reg_size = device->resource[0].end -
        device->resource[0].start + 1;

    // interrupt line
    IMG_ASSERT(device->resource[1].flags == IORESOURCE_IRQ);
    g_TRANSIF_driver.module_irq = device->resource[1].start;
    printk("module irq = %d\n", g_TRANSIF_driver.module_irq);

    g_TRANSIF_driver.device_detected = IMG_TRUE;
    return 0;
}

static int ci_suspend(struct platform_device * pdev, pm_message_t state)
{
    int ret = 0;
    /* note: state is one of these: 
       PMSG_INVALID PMSG_ON PMSG_FREEZE PMSG_QUIESCE PMSG_SUSPEND PMSG_HIBERNATE PMSG_RESUME
       PMSG_THAW PMSG_RESTORE PMSG_RECOVER PMSG_USER_SUSPEND PMSG_USER_RESUME PMSG_REMOTE_RESUME	
       PMSG_AUTO_SUSPEND PMSG_AUTO_RESUME
    */
    dev_info(&pdev->dev, "%s state:%d\n", __func__, state.event);

    if ( g_TRANSIF_driver.pSysDevice && g_TRANSIF_driver.pSysDevice->suspend )
    {
        ret=g_TRANSIF_driver.pSysDevice->suspend(g_TRANSIF_driver.pSysDevice, state);
    }
    
    return ret;
}

static int ci_resume(struct platform_device * pdev)
{
    int ret = 0;
    dev_info(&pdev->dev, "%s\n", __func__);

    if ( g_TRANSIF_driver.pSysDevice && g_TRANSIF_driver.pSysDevice->resume )
    {
        ret=g_TRANSIF_driver.pSysDevice->resume(g_TRANSIF_driver.pSysDevice);
    }

    return ret;
}

/*
 * Functions accessible from outside
 */

IMG_RESULT SYS_DevRegister(SYS_DEVICE *pDevice)
{
    int ret = 0;

    printk(KERN_DEBUG "Felix CI driver for transif initialization...\n");

    if (g_TRANSIF_driver.pSysDevice != NULL)
    {
        printk(KERN_ERR "Device %s already detected!\n",
                pDevice->pszDeviceName);
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    g_TRANSIF_driver.pSysDevice = pDevice;
    g_TRANSIF_driver.sDriver.suspend = &ci_suspend;
    g_TRANSIF_driver.sDriver.resume = &ci_resume;

    // Register platform driver.
    ret = platform_driver_register(&g_TRANSIF_driver.sDriver);

    if (ret != 0) {
        ret = IMG_ERROR_DEVICE_NOT_FOUND;
        printk(KERN_ERR "platform_driver_register failed!\n");
        goto platform_failure_register;
    }

    if (g_TRANSIF_driver.device_detected != IMG_TRUE) {
        printk(KERN_ERR "device not probed!\n");
        ret = IMG_ERROR_DEVICE_NOT_FOUND;
        goto platform_failure_detect;
    }

    g_TRANSIF_driver.reg_base_addr = ioremap(g_TRANSIF_driver.module_reg_base,
            g_TRANSIF_driver.module_reg_size);

    if (!g_TRANSIF_driver.reg_base_addr) {
        printk(KERN_ERR "Failed to remap registers\n");
        ret = IMG_ERROR_GENERIC_FAILURE;
        goto platform_failure_map_reg;
    }

    // Register space setup.
    g_TRANSIF_driver.pSysDevice->uiRegisterSize =
        (IMG_UINTPTR)g_TRANSIF_driver.module_reg_size;
    g_TRANSIF_driver.pSysDevice->uiRegisterPhysical =
        (IMG_UINTPTR)g_TRANSIF_driver.module_reg_base;
    g_TRANSIF_driver.pSysDevice->uiRegisterCPUVirtual =
        (IMG_UINTPTR)g_TRANSIF_driver.reg_base_addr;

    // Memory space setup.
    g_TRANSIF_driver.pSysDevice->uiMemorySize = 0;
    g_TRANSIF_driver.pSysDevice->uiMemoryPhysical = 0;
    g_TRANSIF_driver.pSysDevice->uiMemoryCPUVirtual = 0;

    // Board register setup.
    g_TRANSIF_driver.pSysDevice->uiBoardSize = 0;
    g_TRANSIF_driver.pSysDevice->uiBoardPhysical = 0;
    g_TRANSIF_driver.pSysDevice->uiBoardCPUVirtual = 0;

    g_TRANSIF_driver.pSysDevice->irq_line = g_TRANSIF_driver.module_irq;

    // and at last call the probe success callback
    if (g_TRANSIF_driver.pSysDevice->probeSuccess)
    {
        int ret;
        printk(KERN_DEBUG "Calling probe...\n");
        if ((ret=g_TRANSIF_driver.pSysDevice->probeSuccess(
                        g_TRANSIF_driver.pSysDevice)) != 0)
        {
            //error - this is not our device
            printk(KERN_WARNING "Driver found but refused device!\n");
            goto platform_failure_map_reg;
        }

        SYS_DevClearInterrupts(NULL); // clear whatever interrupts was there
    }
    else
    {
        printk(KERN_ERR "Driver found but not probe success handle given!\n");
        goto platform_failure_map_reg;
    }

    printk(KERN_DEBUG "Felix CI driver for transif initialized.\n");

    return IMG_SUCCESS;

platform_failure_map_reg:
platform_failure_detect:
    platform_driver_unregister(&g_TRANSIF_driver.sDriver);
platform_failure_register:
    return IMG_ERROR_FATAL;
}

IMG_RESULT SYS_DevClearInterrupts(SYS_DEVICE *pDevice)
{
    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRemove(SYS_DEVICE *pDevice)
{
    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevDeregister(SYS_DEVICE *pDevice)
{
    printk(KERN_DEBUG "Unregistering TRANSIF driver...\n");

    if (!g_TRANSIF_driver.device_detected)
        return IMG_SUCCESS;

    iounmap(g_TRANSIF_driver.reg_base_addr);

    platform_driver_unregister(&g_TRANSIF_driver.sDriver);
    g_TRANSIF_driver.pSysDevice = NULL;

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRequestIRQ(SYS_DEVICE *pDevice, const char *pszName)
{
	int result;

	// shared with others, no entropy
	result = request_threaded_irq(pDevice->irq_line,
        pDevice->irqHardHandle, pDevice->irqThreadHandle,
        IRQF_SHARED, pszName, pDevice->handlerParam);

	if ( result != 0)
	{
		return IMG_ERROR_UNEXPECTED_STATE;
	}
	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevFreeIRQ(SYS_DEVICE *pDevice)
{
	free_irq(pDevice->irq_line, pDevice->handlerParam);
	return IMG_SUCCESS;
}

IMG_RESULT SYS_DevPowerControl(SYS_DEVICE *pDevice, IMG_BOOL8 bDeviceActive)
{
    // this function does nothing on that implementation
    return IMG_SUCCESS;
}
