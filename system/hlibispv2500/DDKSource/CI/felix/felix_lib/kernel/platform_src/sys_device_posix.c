/**
*******************************************************************************
@file sys_device_posix.c

@brief

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
#include <ci_internal/sys_device.h>

#include <ci_internal/sys_parallel.h>

#include <ci_kernel/ci_kernel.h> // to access registers
#include <registers/core.h>
#include <tal.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#ifndef FELIX_FAKE
#error "compiling sys_device_posix.c but not using felix fake device"
#endif

#include <pthread.h>

#ifdef WIN32
#include <Windows.h> // for Sleep
#endif

#define MAX_INTERRUPT 16

struct posix_interrupt
{
    pthread_t sThread;
    /**
    * @brief Signal thread started - should be threadsafe because it
    * should be manipulated atomically by the CPU */
    IMG_BOOL8 bStarted;
    /**
     * @brief Time to stop the thread - should be threadsafe because it
     * should be manipulated atomically by the CPU */
    IMG_BOOL8 bQuit;
    SYS_DEVICE *pDevice;
};

static pthread_mutex_t sUsedInterruptLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned g_active = 0;
static struct posix_interrupt **g_usedInterrupts;

static void mysleep(IMG_UINT32 ui32SleepMiliSec)
{
    if (ui32SleepMiliSec > 0)
    {
#ifdef _WIN32
        Sleep(ui32SleepMiliSec);
#else
        usleep(ui32SleepMiliSec*100);
#endif
    }
}

/**
 * When running the the CSIM no interrupts are generated through TCP/IP
 * therefore we use a thread that actively reads the interrupt source
 * to know if an interrupt occured
 */
static void* IMG_SYS_IRQThreadRun(void *param)
{
    struct posix_interrupt *pIRQThread = (struct posix_interrupt *)param;
    int ret;

    pIRQThread->bStarted = IMG_TRUE;

    while (1)
    {
        IMG_UINT32 regValue = 0;
        IMG_ASSERT(g_psCIDriver);

        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
            FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, &regValue);
        while (pIRQThread->bQuit == IMG_FALSE && regValue == 0)
        {
            mysleep(CI_REGPOLL_TIMEOUT * 10);
            TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
                FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, &regValue);
        }
        // checked a second time in case it changed in between register update
        if (pIRQThread->bQuit == IMG_TRUE)
        {
            break;
        }

        ret = pIRQThread->pDevice->irqHardHandle(
            pIRQThread->pDevice->irq_line,
            pIRQThread->pDevice->handlerParam);
        if (IRQ_WAKE_THREAD == ret)
        {
            pIRQThread->pDevice->irqThreadHandle(
                pIRQThread->pDevice->irq_line,
                pIRQThread->pDevice->handlerParam);
        }

#ifdef FELIX_UNIT_TESTS
        // reset the interrupt value
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
            FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, 0);
#endif

        sched_yield(); // let the other threads have a chance to run
    }

    return NULL;
}

IMG_RESULT SYS_DevRegister(SYS_DEVICE *pDevice)
{
    if (pDevice == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pthread_mutex_lock(&sUsedInterruptLock);
    {
        if (g_usedInterrupts == NULL)
        {
            g_usedInterrupts =
                (struct posix_interrupt **)IMG_CALLOC(MAX_INTERRUPT,
                sizeof(struct posix_interrupt *));
        }
        pDevice->irq_line = g_active; // fake probing
        g_active++;
    }
    pthread_mutex_unlock(&sUsedInterruptLock);

    if (pDevice->irq_line == MAX_INTERRUPT)
    {
        return IMG_ERROR_FATAL;
    }

    pDevice->uiBoardCPUVirtual = 0;
    pDevice->uiBoardPhysical = 0; // does not matter in fake HW
    pDevice->uiBoardSize = -1; // maximum size because it does not matter

    pDevice->uiRegisterCPUVirtual = 0;
    pDevice->uiRegisterPhysical = 0; // does not matter in fake HW
    pDevice->uiRegisterSize = -1; // maximum size because it does not matter

    pDevice->uiMemoryCPUVirtual = 0;
    pDevice->uiMemoryPhysical = 0; // does not matter in fake HW
    pDevice->uiMemorySize = -1; // maximum size because it does not matter

    if (pDevice->probeSuccess)
    {
        pDevice->probeSuccess(pDevice);
    }

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRemove(SYS_DEVICE *pDevice)
{
    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevDeregister(SYS_DEVICE *pDevice)
{
    if (pDevice == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pthread_mutex_lock(&sUsedInterruptLock);
    {
        g_active--;
        if (g_active == 0)
        {
            IMG_FREE(g_usedInterrupts);
            g_usedInterrupts = NULL;
        }
    }
    pthread_mutex_unlock(&sUsedInterruptLock);

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevClearInterrupts(SYS_DEVICE *pDevice)
{
    (void)pDevice;
    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRequestIRQ(SYS_DEVICE *pDevice, const char *pszName)
{
    struct posix_interrupt *pIrqThread = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pDevice == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (pDevice->irqHardHandle == NULL || pDevice->irqThreadHandle == NULL
        || pDevice->irq_line > MAX_INTERRUPT)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    pthread_mutex_lock(&sUsedInterruptLock);
    {
        if (g_usedInterrupts[pDevice->irq_line] == NULL)
        {
            g_usedInterrupts[pDevice->irq_line] =
                (struct posix_interrupt *)IMG_CALLOC(1,
                sizeof(struct posix_interrupt));
            pIrqThread = g_usedInterrupts[pDevice->irq_line];
        }
        // otherwise the interrupt is already in use
    }
    pthread_mutex_unlock(&sUsedInterruptLock);

    if (pIrqThread == NULL)
    {
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    pDevice->bIrqAcquired = IMG_TRUE;
    pIrqThread->pDevice = pDevice;
    pthread_create(&(pIrqThread->sThread), NULL, &IMG_SYS_IRQThreadRun,
        (void*)pIrqThread);

    // sync with interrupt thread to ensure it was created
    while (!pIrqThread->bStarted)
    {
        sched_yield();  // let the other threads have a chance to run
    }

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevFreeIRQ(SYS_DEVICE *pDevice)
{
    struct posix_interrupt *pIrqThread = NULL;
    if (pDevice == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (pDevice->irqHardHandle == NULL || pDevice->irqThreadHandle == NULL
        || pDevice->irq_line > MAX_INTERRUPT
        || pDevice->bIrqAcquired == IMG_FALSE)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    pIrqThread = g_usedInterrupts[pDevice->irq_line];
    pIrqThread->bQuit = IMG_TRUE; // it should stop at some point...

    // sync with the other thread until it stops
    pthread_join(pIrqThread->sThread, NULL);

    pthread_mutex_lock(&sUsedInterruptLock);
    {
        IMG_FREE(g_usedInterrupts[pDevice->irq_line]);
        g_usedInterrupts[pDevice->irq_line] = NULL; // can be reused now
    }
    pthread_mutex_unlock(&sUsedInterruptLock);

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevPowerControl(SYS_DEVICE *pDevice, IMG_BOOL8 bDeviceActive)
{
    // this function does nothing on that implementation
    return IMG_SUCCESS;
}
