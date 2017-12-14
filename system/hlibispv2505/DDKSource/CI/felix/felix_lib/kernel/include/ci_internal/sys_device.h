/**
*******************************************************************************
 @file sys_device.h

 @brief define the function a device should implement

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
#ifndef SYS_DEVICE_H_
#define SYS_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FELIX_FAKE

enum irqreturn
{
    IRQ_NONE = 0,
    IRQ_HANDLED = 1,
    IRQ_WAKE_THREAD = IRQ_HANDLED << 1
};

typedef enum irqreturn irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(int, void*);

typedef struct pm_message
{
    int event;
} pm_message_t;

// fake the struct file_operations from linux/fs.h
#include "sys/sys_userio_fake.h"

#else
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/pm.h>
#endif

#include <img_types.h>

// CI_KERNEL_TOOLS defined in sys_mem.h
/**
 * @addtogroup CI_KERNEL_TOOLS
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_KERNEL_TOOLS (defined in sys_mem.h)
 *---------------------------------------------------------------------------*/

/**
 * @name Device abstraction
 * @brief Basic information about a device (abstracted so that it can be
 * implemented for different systems, e.g. for CSIM or PCI FPGA board).
 * @{
 */

typedef irq_handler_t SYS_IRQ_HANDLER;
struct SYS_DEVICE;

/** @brief Stores the information the driver needs about a device */
typedef struct SYS_DEVICE
{
    const char *pszDeviceName;

    /** @brief interrupt line used */
    unsigned int irq_line;
    /**
     * @brief hard interrupt handler function to call (i.e. cannot sleep)
     *
     * See Linux request_threaded_irq()
     */
    SYS_IRQ_HANDLER irqHardHandle;
    /**
     * @brief soft interrupt handler function to call (i.e. can sleep)
     *
     * See Linux request_threaded_irq()
     */
    SYS_IRQ_HANDLER irqThreadHandle;
    /** @brief parameter to the interrupt handler function */
    void *handlerParam;
    /** @brief know if IRQ was required in OS */
    IMG_BOOL8 bIrqAcquired;

#ifdef INFOTM_HW_AWB_METHOD
    unsigned int hw_awb_irq_line;
    SYS_IRQ_HANDLER hw_awb_irqHandler;
    void *hw_awb_handlerParam;
#endif //INFOTM_HW_AWB_METHOD

    /** @brief which functions to call when system calls are received */
    struct file_operations sFileOps;
    /** @brief will be called when probe found a device */
    int (*probeSuccess)(struct SYS_DEVICE *);

    /** @brief will be called when suspend is called */
    IMG_RESULT (*suspend)(struct SYS_DEVICE *pDev, pm_message_t state);
    /** @brief will be called when resume is called */
    IMG_RESULT (*resume)(struct SYS_DEVICE *pDev);

    /**
     * @name board registers when using FPGA
     * @{
     */
    /** @brief CPU virtual address with ioremap */
    IMG_UINTPTR uiBoardCPUVirtual;
    /** @brief physical offset */
    IMG_UINTPTR uiBoardPhysical;
    /** @brief in bytes */
    IMG_UINTPTR uiBoardSize;

    /**
     * @}
     * @name device registers
     * @{
     */
    /** @brief CPU virtual address with ioremap */
    IMG_UINTPTR uiRegisterCPUVirtual;
    /** @brief physical offset */
    IMG_UINTPTR uiRegisterPhysical;
    /** @brief in bytes */
    IMG_UINTPTR uiRegisterSize;

    /**
    * @}
    * @name device memory
    * @{
    */
    /** @brief CPU virtual address with ioremap */
    IMG_UINTPTR uiMemoryCPUVirtual;
    /** @brief memory base address visible by cpu */
    IMG_UINTPTR uiMemoryPhysical;
    /** @brief memory base address visible by device */
    IMG_UINTPTR uiDevMemoryPhysical;
    /** @brief in bytes */
    IMG_UINTPTR uiMemorySize;
    /**
     * @}
     */

    /**
     * @brief sys device specific data pointer
     */
    void *data;
} SYS_DEVICE;

IMG_RESULT SYS_DevRegister(SYS_DEVICE *pDevice);
IMG_RESULT SYS_DevDeregister(SYS_DEVICE *pDevice);

/**
 * @brief Called when the kernel module is removed
 */
IMG_RESULT SYS_DevRemove(SYS_DEVICE *pDevice);

/**
 * @brief Implement that function if the device needs to clear more
 * registers than the felix ones when receiving an interrupt
 */
IMG_RESULT SYS_DevClearInterrupts(SYS_DEVICE *pDevice);

/**
 * @brief Request the IRQ associated with the given device
 *
 * @param pDevice the device
 * @param pszName name of the handler
 *
 * @return IMG_SUCCESS if the interrupt was acquired
 * @return IMG_ERROR_UNEXPECTED_STATE if the interrupt could not be acquired
 * at that time
 */
IMG_RESULT SYS_DevRequestIRQ(SYS_DEVICE *pDevice, const char *pszName);

/**
 * @brief Release an aquired interrupt (if not acquired then nothing happens)
 */
IMG_RESULT SYS_DevFreeIRQ(SYS_DEVICE *pDevice);

/**
 * @brief This function is called by the driver to signal that the driver
 * is becoming Idle or active
 *
 * @param pDevice
 * @param bDeviceActive IMG_TRUE if the device needs to become active
 * (power ON) or IMG_FALSE if the device can become inactive (power OFF)
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT SYS_DevPowerControl(SYS_DEVICE *pDevice, IMG_BOOL8 bDeviceActive);

/**
 * @}
 */
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of Device abstractions
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SYS_DEVICE_H_ */
