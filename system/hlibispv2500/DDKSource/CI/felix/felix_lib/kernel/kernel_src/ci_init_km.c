/**
******************************************************************************
@file ci_init_km.c

@brief Point of entry when building kernel module

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
#include <img_types.h>
#include <img_errors.h>

#ifndef IMG_KERNEL_MODULE
#error insmod only for kernel
#endif

#include <linux/init.h>
#include <linux/module.h>       /* Needed by all modules */
#include <linux/errno.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/moduleparam.h> // permissions from linux/stat.h
#include <linux/kernel.h>
#include <linux/miscdevice.h>

#ifdef INFOTM_ISP
#include <linux/clk.h>
#endif //INFOTM_ISP

#include "ci_kernel/ci_ioctrl.h" // for toErrno
#include "ci_kernel/ci_kernel.h"
#include "ci_internal/ci_errors.h"

#ifdef INFOTM_ISP
#include <asm/io.h>
#include <mach/imap-iomap.h>
#include <mach/imap-isp.h>
#include <mach/power-gate.h>
#endif //INFOTM_ISP

#ifdef INFOTM_ISP
#include <mach/items.h>
#endif //INFOTM_ISP

#define CI_SEM_WAIT 1000 // maximum timed waited to acquire a frame in ms

#if defined(FELIX_HAS_DG)
#undef CI_SEM_WAIT
// increased to cope with FPGA system memory latency that is increased when using external DG
#define CI_SEM_WAIT (5*1000) // 5 sec
#endif
#if defined(ANDROID_EMULATOR)
#undef CI_SEM_WAIT
// increased when compiling for the Android emulator to cope with simulator latency
#define CI_SEM_WAIT (50*1000) // 50 sec
#endif

/// @brief Enable the MMU - 0 bypass MMU (1:1 virt to phys), 1 MMU 32b, 2 MMU extended addressing - read when insmod
#ifdef INFOTM_ISP
static int mmucontrol = 0;
#else
static int mmucontrol = 2;
#endif //INFOTM_ISP
/// @brief Enable the MMU tiling (256 = 256Bx16, 512 = 512Bx8)
static int tilingScheme = 256;
/// @brief Time to wait for semaphores (see KRN_CI_DRIVER::uiSemWait)
static uint frametimeout = CI_SEM_WAIT;
/// @brief Gamma curve to use in the driver (see KRN_CI_DriverDefaultsGammaLUT() for legitimate values)
static uint gammaCurve = CI_DEF_GMACURVE;
/// @brief Control of the printing using CI_FATAL CI_WARNING CI_INFO and CI_DEBUG

IMG_UINT ciLogLevel = CI_LOG_LEVEL;
#if defined(INFOTM_ISP)
uint ui32ClkRate = 192000000;
#endif

module_param(mmucontrol, int, S_IRUGO);
MODULE_PARM_DESC(mmucontrol, "MMU control: 0 disabled (direct mapping), 1 32b mapping, 2 extended range mapping (using HW size)");

module_param(tilingScheme, int, S_IRUGO);
MODULE_PARM_DESC(tilingScheme, "MMU tiling: choice of tiling scheme 256 = 256Bx16, 512 = 512Bx8 (active only if mmucontrol>=1)");

module_param(frametimeout, uint, S_IRUGO);
MODULE_PARM_DESC(frametimeout, "Timeout in ms when acquiring a frame (>0!)");

module_param(gammaCurve, uint, S_IRUGO);
MODULE_PARM_DESC(gammaCurve, "Gamma curve to use (>=0!)");

module_param(ciLogLevel, uint, S_IRUGO);
MODULE_PARM_DESC(ciLogLevel, "CI Logging level between CI_LOG_LEVEL_QUIET (0)  and CI_LOG_LEVEL_DBG (4)");
#if defined(INFOTM_ISP)
module_param(ui32ClkRate, uint, S_IRUGO);
MODULE_PARM_DESC(ui32ClkRate, "isp working clock");
#endif
#ifdef CI_DEBUGFS

//#include "ci_target.h" // information about registers banks
#include "ci_kernel/ci_debug.h" // to declare the variables of debugfs when present and keep sparse quite about them needing to be static
#define CORE_BANK 0
#define CTX_BANK 4
#define MMU_BANK 6

#include <registers/core.h>
#include <registers/context0.h>
#include <registers/mmu.h>

#define DEBUGFS_MOD 0555

#endif // CI_DEBUGFS

#ifdef FELIX_HAS_DG
#include "dg_kernel/dg_camera.h"

#define DEV_FIRST_MINOR_DG 0
#define DEV_NUMBER_DG 1
#define DEV_NAME_DG "imgfelixDG"

#ifdef CI_DEBUGFS
#include <registers/ext_data_generator.h> // used in debugfs
#include "dg_kernel/dg_debug.h" // to declare the variables of debugfs when present and keep sparse quite about them needing to be static
#define DG_BANK 7
#endif // CI_DEBUGFS

#endif // FELIX_HAS_DG

#if IMG_SUCCESS != 0
#error "IMG_SUCCESS is not 0!"
#endif

#ifndef HAVE_UNLOCKED_IOCTL // newer kernel use unlocked_ioctl instead of ioctl that was protected by the big lock
#error "unlocked_ioctl function pointer is requiered (linux kernel >= 2.6.11)"
#endif

//#define USE_CHAR_DRIVER // use char device driver (or misc driver if not defined)

#define DEV_FIRST_MINOR 0
#define DEV_NUMBER 1
#define DEV_NAME "imgfelix"

struct DEV_KERNEL {
    dev_t deviceNumber; // device identifier
    unsigned int uiNumberDevices; // number of devices in chrdev region

#ifdef USE_CHAR_DRIVER
    struct cdev sCharDevice;

    struct class *pSysClass;
    struct device *pSysDevice;
#else
    struct miscdevice miscdev;
#endif
};

static SYS_DEVICE sSysDevice;
static struct KRN_CI_DRIVER sFelixDriver;
static struct DEV_KERNEL g_CIKernelInfo;

// used for debugfs
#ifdef CI_DEBUGFS
static struct dentry *pDebugFsDir;

static struct dentry *pDriverNConnections; // use list info
#define DEBUGFS_NCONN "DriverNConnections"

static struct dentry *pDriverNCTXTriggered[CI_N_CONTEXT];
#define DEBUGFS_NCTXTRIGGERED_S "DriverCTX%dTriggeredHW"
static struct dentry *pDriverNCTXSubmitted[CI_N_CONTEXT];
#define DEBUGFS_NCTXSUBMITTED_S "DriverCTX%dTriggeredSW"
static struct dentry *pDriverNCTXInt[CI_N_CONTEXT];
#define DEBUGFS_NCTXINT_S "DriverCTX%dInt"
static struct dentry *pDriverNCTXDoneInt[CI_N_CONTEXT];
#define DEBUGFS_NCTXDONEINT_S "DriverCTX%dInt_DoneAll"
static struct dentry *pDriverNCTXIgnoreInt[CI_N_CONTEXT];
#define DEBUGFS_NCTXIGNOREINT_S "DriverCTX%dInt_Ignore"
static struct dentry *pDriverNCTXStartInt[CI_N_CONTEXT];
#define DEBUGFS_NCTXSTARTINT_S "DriverCTX%dInt_Start"

static struct dentry *pDriverNIntDGErrorInt[CI_N_IIF_DATAGEN];
#define DEBUGFS_NINTDGERRORINT_S "DriverIntDG%dInt_Error"
static struct dentry *pDriverNIntDGFrameEnd[CI_N_IIF_DATAGEN];
#define DEBUGFS_NINTDGFRAMEEND_S "DriverIntDG%dInt_EndOfFrame"

#ifdef FELIX_HAS_DG

static struct dentry *pDriverNDGInt[CI_N_IMAGERS];
#define DEBUGFS_NDGINT_S "DriverDG%dInt"
static struct dentry *pDriverNDGTriggered[CI_N_IMAGERS];
#define DEBUGFS_NDGTRIGGERED_S "DriverDG%dTriggeredHW"
static struct dentry *pDriverNDGSubmitted[CI_N_IMAGERS];
#define DEBUGFS_NDGSUBMITTED_S "DriverDG%dSubmittedSW"

static struct dentry *pDriverDGLongestHardIntUS;
#define DEBUGFS_DGLONGESTHARDINTUS "DriverDGLongestHardIntUS"
static struct dentry *pDriverDGLongestThreadIntUS;
#define DEBUGFS_DGLONGESTTHREADINTUS "DriverDGLongestThreadIntUS"

static struct dentry *pDriverServicedDGHardInt;
#define DEBUGFS_SERVICEDDGHARDINT "DriverDGNServicedHardInt"
static struct dentry *pDriverServicedDGThreadInt;
#define DEBUGFS_SERVICEDDGTHREADINT "DriverDGNServicedThreadInt"

#endif

static struct dentry *pDriverServicedHardInt;
#define DEBUGFS_SERVICEDHARDINT "DriverNServicedHardInt"

static struct dentry *pDriverServicedThreadInt;
#define DEBUGFS_SERVICEDTHREADINT "DriverNServicedThreadInt"

static struct dentry *pDriverLongestHardIntUS;
#define DEBUGFS_LONGESTHARDINTUS "DriverLongestHardIntUS"

static struct dentry *pDriverLongestThreadIntUS;
#define DEBUGFS_LONGESTTHREADINTUS "DriverLongestThreadIntUS"

static struct dentry *pDriverCtxActive[CI_N_CONTEXT]; // uses the linestore information from the driver
#define DEBUGFS_CTXACTIVE_S "DriverCTX%dActive"

static struct dentry *pDriverDevMemUsed;
#define DEBUGFS_DEVMEMUSED "DevMemUsed"

static struct dentry *pDriverDevMemMaxUsed;
#define DEBUGFS_DEVMEMMAXUSED "DevMemMaxUsed"

IMG_UINT32 g_ui32NCTXTriggered[CI_N_CONTEXT];
IMG_UINT32 g_ui32NCTXSubmitted[CI_N_CONTEXT];
IMG_UINT32 g_ui32NCTXInt[CI_N_CONTEXT];
IMG_UINT32 g_ui32NCTXDoneInt[CI_N_CONTEXT];
IMG_UINT32 g_ui32NCTXIgnoreInt[CI_N_CONTEXT];
IMG_UINT32 g_ui32NCTXStartInt[CI_N_CONTEXT];

IMG_UINT32 g_ui32NIntDGErrorInt[CI_N_IIF_DATAGEN];
IMG_UINT32 g_ui32NIntDGFrameEndInt[CI_N_IIF_DATAGEN];

IMG_UINT32 g_ui32NServicedHardInt;
IMG_INT64 g_i64LongestHardIntUS;
IMG_UINT32 g_ui32NServicedThreadInt;
IMG_INT64 g_i64LongestThreadIntUS;

#ifdef FELIX_HAS_DG
// interesting only if dg is available
IMG_UINT32 g_ui32NDGInt[CI_N_IMAGERS];
IMG_UINT32 g_ui32NDGTriggered[CI_N_IMAGERS]; // triggered for shoot in HW
IMG_UINT32 g_ui32NDGSubmitted[CI_N_IMAGERS]; // triggered for shoot in SW

IMG_UINT32 g_ui32NServicedDGHardInt;
IMG_UINT32 g_ui32NServicedDGThreadInt;

IMG_INT64 g_i64LongestDGHardIntUS;
IMG_INT64 g_i64LongestDGThreadIntUS;
#endif

DEFINE_MUTEX(g_DebugFSDevMemMutex); // used when accessing memory related debugFS variables
IMG_UINT32 g_ui32DevMemUsed; // contains both imported and allocated memory
IMG_UINT32 g_ui32DevMemMaxUsed;

#endif // CI_DEBUGFS
#include "ci_kernel/ci_debug.h"

#ifdef FELIX_HAS_DG
static struct KRN_DG_DRIVER sDGDriver;
static struct DEV_KERNEL g_DGKernelInfo;

#endif // FELIX_HAS_DG

static void freeDevice(struct DEV_KERNEL *pKrnInfo)
{
#ifdef USE_CHAR_DRIVER
    if ( pKrnInfo == NULL )
    {
        CI_FATAL("oups! pKrnInfo is NULL\n");
    }
    else
    {
        CI_DEBUG("free device %d\n", pKrnInfo->deviceNumber);
        unregister_chrdev_region(pKrnInfo->deviceNumber, pKrnInfo->uiNumberDevices);
        class_destroy(pKrnInfo->pSysClass);
    }
#endif
}

static void deregisterDriver(struct DEV_KERNEL *pKrnInfo)
{
#ifdef USE_CHAR_DRIVER
    if ( pKrnInfo->pSysDevice != NULL )
    {
        CI_DEBUG("deregister device %d\n", pKrnInfo->deviceNumber);
        device_destroy(pKrnInfo->pSysClass, pKrnInfo->deviceNumber);
    }
    CI_DEBUG("remove char dev\n");
    cdev_del(&(pKrnInfo->sCharDevice)); // needed only if dynamically allocated?

#else
    if ( misc_deregister(&(pKrnInfo->miscdev)) )
    {
        CI_FATAL("Failed to deregister misc driver\n");
    }
#endif
}

static int allocDevice(struct DEV_KERNEL *pKrnInfo, int firstMinor, int devCount, const char *baseDevName)
{
#ifdef USE_CHAR_DRIVER
    // create file in /sys/class/
    pKrnInfo->pSysClass = class_create(THIS_MODULE, baseDevName);
    if ( IS_ERR(pKrnInfo->pSysClass) )
    {
        CI_FATAL("Cannot create class %s\n", baseDevName);
        return -ENODEV;
    }

    // find char device region
    if ( alloc_chrdev_region(&(pKrnInfo->deviceNumber), firstMinor, devCount, baseDevName) != 0 )
    {
        CI_FATAL("Cannot allocate char device region for device %s (first minor %d, count %d)\n", baseDevName, firstMinor, devCount);
        class_destroy(pKrnInfo->pSysClass);
        pKrnInfo->pSysClass = NULL;
        return -ENODEV;
    }

    pKrnInfo->uiNumberDevices = devCount;
#endif



    CI_DEBUG("Device '%s' region %d allocated\n", baseDevName, pKrnInfo->deviceNumber);
    return 0;
}

static int registerDriver(struct DEV_KERNEL *pKrnInfo, struct file_operations *pDevFileOps, const char *baseDevName)
{
    char device_name[32];
    int ret = 0;

    snprintf(device_name, sizeof device_name, "%s%d", baseDevName, MINOR(pKrnInfo->deviceNumber));
    CI_DEBUG("Creating device '%s'\n", device_name);

#ifdef USE_CHAR_DRIVER
    // register the char device
    cdev_init(&(pKrnInfo->sCharDevice), pDevFileOps);

    pKrnInfo->sCharDevice.owner = THIS_MODULE;
    pKrnInfo->sCharDevice.ops = pDevFileOps;

    if ( (ret=cdev_add(&(pKrnInfo->sCharDevice), pKrnInfo->deviceNumber, 1)) != 0 )
    {
        CI_FATAL("Failed to add the char device %s (returned %d)\n", baseDevName, ret);
        return ret;
    }

    pKrnInfo->pSysDevice = device_create(pKrnInfo->pSysClass, 0, pKrnInfo->deviceNumber, NULL, device_name);

    if ( IS_ERR(pKrnInfo->pSysDevice) )
    {
        CI_FATAL("Failed to create device %s (returned 0x%x)\n", device_name, pKrnInfo->pSysDevice);
        cdev_del(&(pKrnInfo->sCharDevice)); // needed only if dynamically allocated?
        return -ENODEV;
    }
#else
    pKrnInfo->miscdev.name = device_name;
    pKrnInfo->miscdev.fops = pDevFileOps;
    pKrnInfo->miscdev.minor = MISC_DYNAMIC_MINOR;

    if ( (ret=misc_register(&(pKrnInfo->miscdev))) )
    {
        CI_FATAL("Failed to register the misc driver for %s\n", baseDevName);
        return ret;
    }
#endif
    return 0;
}

void KRN_CI_ResetDebugFS(void)
{
#ifdef CI_DEBUGFS
    int ctx;
    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        g_ui32NCTXTriggered[ctx] = 0;
        g_ui32NCTXSubmitted[ctx] = 0;
        g_ui32NCTXInt[ctx] = 0;
        g_ui32NCTXDoneInt[ctx] = 0;
        g_ui32NCTXIgnoreInt[ctx] = 0;
        g_ui32NCTXStartInt[ctx] = 0;
    }
    for ( ctx = 0 ; ctx < CI_N_IIF_DATAGEN ; ctx++ )
    {
        g_ui32NIntDGErrorInt[ctx] = 0;
        g_ui32NIntDGFrameEndInt[ctx] = 0;
    }
#ifdef FELIX_HAS_DG
    for ( ctx = 0 ; ctx < CI_N_IMAGERS ; ctx++ )
    {
        g_ui32NDGInt[ctx] = 0;
        g_ui32NDGTriggered[ctx] = 0;
        g_ui32NDGSubmitted[ctx] = 0;
    }
    g_ui32NServicedDGHardInt = 0;
    g_ui32NServicedDGThreadInt = 0;
    g_i64LongestDGHardIntUS = 0;
    g_i64LongestDGThreadIntUS = 0;
#endif
    g_ui32NServicedHardInt = 0;
    g_i64LongestHardIntUS = 0;
    g_ui32NServicedThreadInt = 0;
    g_i64LongestThreadIntUS = 0;

    g_ui32DevMemUsed = 0;
    g_ui32DevMemMaxUsed = 0;
#endif
}

static void DebugFs_Clean(void)
{
#ifdef CI_DEBUGFS
    if ( pDebugFsDir != NULL )
    {
        CI_DEBUG("clean DebugFS\n");
        debugfs_remove_recursive(pDebugFsDir);
    }
#endif
}

#ifdef FELIX_HAS_DG
// DebugFs_Create should be called prior
static void DebugFs_DGCreate(struct KRN_DG_DRIVER *pDGKernel, struct KRN_CI_DRIVER *pCIKernel)
{
#ifdef CI_DEBUGFS
    if ( pDebugFsDir != NULL && g_pDGDriver != NULL )
    {
        char name[64];
        int dg;

        for ( dg = 0 ; dg < g_pDGDriver->sHWInfo.config_ui8NDatagen ; dg++ )
        {
            sprintf(name, DEBUGFS_NDGINT_S, dg);
            pDriverNDGInt[dg] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NDGInt[dg]));
            if ( IS_ERR(pDriverNDGInt[dg]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NDGTRIGGERED_S, dg);
            pDriverNDGTriggered[dg] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NDGTriggered[dg]));
            if ( IS_ERR(pDriverNDGTriggered) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NDGSUBMITTED_S, dg);
            pDriverNDGSubmitted[dg] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NDGSubmitted[dg]));
            if ( IS_ERR(pDriverNDGSubmitted[dg]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }
        }

        pDriverServicedDGHardInt = debugfs_create_u32(DEBUGFS_SERVICEDDGHARDINT, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NServicedDGHardInt));
        if (IS_ERR(pDriverServicedDGHardInt))
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_SERVICEDDGHARDINT);
            return;
        }

        pDriverServicedDGThreadInt = debugfs_create_u32(DEBUGFS_SERVICEDDGTHREADINT, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NServicedDGThreadInt));
        if (IS_ERR(pDriverServicedDGThreadInt))
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_SERVICEDDGTHREADINT);
            return;
        }

        pDriverDGLongestHardIntUS = debugfs_create_u64(DEBUGFS_DGLONGESTHARDINTUS, DEBUGFS_MOD, pDebugFsDir, &(g_i64LongestDGHardIntUS));
        if ( IS_ERR(pDriverDGLongestHardIntUS) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_LONGESTHARDINTUS);
            return;
        }

        pDriverDGLongestThreadIntUS = debugfs_create_u64(DEBUGFS_DGLONGESTTHREADINTUS, DEBUGFS_MOD, pDebugFsDir, &(g_i64LongestDGThreadIntUS));
        if ( IS_ERR(pDriverDGLongestHardIntUS) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_DGLONGESTTHREADINTUS);
            return;
        }
    }
    else
    {
        CI_FATAL("failed to create DG debugfs! DG driver should be created and debugfs folder initialised beforehand!\n");
    }
#endif
}
#endif

static void DebugFs_Create(struct KRN_CI_DRIVER *pCIKernel)
{
#ifdef CI_DEBUGFS
    if ( pDebugFsDir == NULL )
    {
        CI_DEBUG("initialise %s debugFS\n", pCIKernel->pDevice->pszDeviceName);
        pDebugFsDir = debugfs_create_dir(pCIKernel->pDevice->pszDeviceName, NULL);
    }

    if ( IS_ERR(pDebugFsDir) )
    {
        CI_WARNING("Failed to create debugFS directory '%s'\n", pCIKernel->pDevice->pszDeviceName);
        pDebugFsDir = NULL;
    }
    else
    {
        int ctx;
        char name[64];

        if ( g_psCIDriver == NULL )
        {
            CI_FATAL("driver should be initialised before creating debugfs!\n");
            return;
        }

        // clean globals
        KRN_CI_ResetDebugFS();

        pDriverDevMemUsed = debugfs_create_u32(DEBUGFS_DEVMEMUSED, DEBUGFS_MOD, pDebugFsDir, &g_ui32DevMemUsed);
        if ( IS_ERR(pDriverDevMemUsed) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_DEVMEMUSED);
            return;
        }

        pDriverDevMemMaxUsed = debugfs_create_u32(DEBUGFS_DEVMEMMAXUSED, DEBUGFS_MOD, pDebugFsDir, &g_ui32DevMemMaxUsed);
        if ( IS_ERR(pDriverDevMemMaxUsed) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_DEVMEMMAXUSED);
            return;
        }

        pDriverNConnections = debugfs_create_u32(DEBUGFS_NCONN, DEBUGFS_MOD, pDebugFsDir, &(pCIKernel->sList_connection.ui32Elements));
        if ( IS_ERR(pDriverNConnections) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_NCONN);
            return;
        }

        pDriverServicedHardInt = debugfs_create_u32(DEBUGFS_SERVICEDHARDINT, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NServicedHardInt));
        if (IS_ERR(pDriverServicedHardInt))
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_SERVICEDHARDINT);
            return;
        }

        pDriverServicedThreadInt = debugfs_create_u32(DEBUGFS_SERVICEDTHREADINT, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NServicedThreadInt));
        if (IS_ERR(pDriverServicedThreadInt))
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_SERVICEDTHREADINT);
            return;
        }

        pDriverLongestHardIntUS = debugfs_create_u64(DEBUGFS_LONGESTHARDINTUS, DEBUGFS_MOD, pDebugFsDir, &(g_i64LongestHardIntUS));
        if ( IS_ERR(pDriverLongestHardIntUS) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_LONGESTHARDINTUS);
            return;
        }

        pDriverLongestThreadIntUS = debugfs_create_u64(DEBUGFS_LONGESTTHREADINTUS, DEBUGFS_MOD, pDebugFsDir, &(g_i64LongestThreadIntUS));
        if ( IS_ERR(pDriverLongestHardIntUS) )
        {
            CI_WARNING("failed to create %s\n", DEBUGFS_LONGESTTHREADINTUS);
            return;
        }

        // all debugfs that are unique per context
        for ( ctx = 0 ; ctx < g_psCIDriver->sHWInfo.config_ui8NContexts ; ctx++ )
        {
            sprintf(name, DEBUGFS_NCTXTRIGGERED_S, ctx);
            pDriverNCTXTriggered[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXTriggered[ctx]));
            if ( IS_ERR(pDriverNCTXTriggered[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NCTXSUBMITTED_S, ctx);
            pDriverNCTXSubmitted[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXSubmitted[ctx]));
            if ( IS_ERR(pDriverNCTXSubmitted[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NCTXINT_S, ctx);
            pDriverNCTXInt[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXInt[ctx]));
            if ( IS_ERR(pDriverNCTXInt[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NCTXDONEINT_S, ctx);
            pDriverNCTXDoneInt[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXDoneInt[ctx]));
            if ( IS_ERR(pDriverNCTXDoneInt[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NCTXIGNOREINT_S, ctx);
            pDriverNCTXIgnoreInt[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXIgnoreInt[ctx]));
            if ( IS_ERR(pDriverNCTXInt[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            sprintf(name, DEBUGFS_NCTXSTARTINT_S, ctx);
            pDriverNCTXStartInt[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NCTXStartInt[ctx]));
            if ( IS_ERR(pDriverNCTXStartInt[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }

            // from linestore info
            sprintf(name, DEBUGFS_CTXACTIVE_S, ctx);
            pDriverCtxActive[ctx] = debugfs_create_u8(name, DEBUGFS_MOD, pDebugFsDir, &(g_psCIDriver->sLinestoreStatus.aActive[ctx]));

            if ( IS_ERR(pDriverCtxActive[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }
        }

        for ( ctx = 0 ; ctx < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator ; ctx++ )
        {
            sprintf(name, DEBUGFS_NINTDGERRORINT_S, ctx);
            pDriverNIntDGErrorInt[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NIntDGErrorInt[ctx]));
            if ( IS_ERR(pDriverNIntDGErrorInt[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }
            
            sprintf(name, DEBUGFS_NINTDGFRAMEEND_S, ctx);
            pDriverNIntDGFrameEnd[ctx] = debugfs_create_u32(name, DEBUGFS_MOD, pDebugFsDir, &(g_ui32NIntDGFrameEndInt[ctx]));
            if ( IS_ERR(pDriverNIntDGFrameEnd[ctx]) )
            {
                CI_WARNING("failed to create %s\n", name);
                return;
            }
        }

#ifdef INFOTM_ISP
        // Extend Debug FS for Infotm ISP
        ExtDebugFS_Create(pDebugFsDir);
#endif

    }
#endif
}

static int probeSuccess(SYS_DEVICE *pDevice)
{
    // device was found!
    int ret = 0;

    CI_DEBUG("start\n");

    if ( (ret=allocDevice(&g_CIKernelInfo, DEV_FIRST_MINOR, DEV_NUMBER, DEV_NAME)) != 0 )
    {
        return ret;
    }

    if ( (ret = KRN_CI_DriverCreate(&sFelixDriver, mmucontrol, tilingScheme, gammaCurve, pDevice)) != IMG_SUCCESS )
    {
        CI_FATAL("felix driver creation failed - returned %d\n", ret);
        freeDevice(&g_CIKernelInfo);
        return toErrno(ret);
    }

#if defined(INFOTM_ISP)
    if (item_exist("sensor.frametimeout"))  // added by linyun.xiong@2015-06-15 for support frametimeout by items
    {
        frametimeout = item_integer("sensor.frametimeout", 0);
    }
	else //if ( frametimeout == 0 )  // modify by linyun.xiong@2015-06-15 for support frametimeout by items
#else
	if ( frametimeout == 0 )
#endif //INFOTM_ISP
	{
		frametimeout = CI_SEM_WAIT;
		CI_WARNING("frametimeout parameter is 0, forced to %u\n", frametimeout);
	}
#if defined(INFOTM_ISP)

	//if ( frametimeout != CI_SEM_WAIT )  // modify by linyun.xiong@2015-06-15 for support frametimeout by items
#else
	if ( frametimeout != CI_SEM_WAIT )
#endif //INFOTM_ISP
	{
		CI_INFO("Timeout to acquire frame changed to %u ms\n", frametimeout);
	}
	sFelixDriver.uiSemWait = frametimeout;

    // now we need to register some operations to the kernel
    pDevice->sFileOps.owner = THIS_MODULE;
    // other operations are set in KERN_DG_DriverCreate

    if ( (ret=registerDriver(&g_CIKernelInfo, &(sFelixDriver.pDevice->sFileOps), DEV_NAME)) != 0 )
    {
        KRN_CI_DriverDestroy(&sFelixDriver);
        freeDevice(&g_CIKernelInfo);
        return ret;
    }

    DebugFs_Create(&sFelixDriver);

#ifdef FELIX_HAS_DG

    if ( (ret=allocDevice(&g_DGKernelInfo, DEV_FIRST_MINOR_DG, DEV_NUMBER_DG, DEV_NAME_DG)) != 0 )
    {
        goto dg_failed;
    }

    if ( (ret = KRN_DG_DriverCreate(&sDGDriver, mmucontrol)) != IMG_SUCCESS )
    {
        CI_FATAL("failed to create the DG driver - returned %d\n", ret);
        ret = toErrno(ret);
        freeDevice(&g_DGKernelInfo);

        goto dg_failed;
    }

    DebugFs_DGCreate(&sDGDriver, &sFelixDriver);

    if ( (ret=registerDriver(&g_DGKernelInfo, &(sDGDriver.sDevice.sFileOps), DEV_NAME_DG)) != 0 )
    {
        CI_FATAL("failed to register the felix DG driver to the kernel\n");
        ret = toErrno(ret);
        KRN_DG_DriverDestroy(&sDGDriver);
        freeDevice(&g_DGKernelInfo);
        goto dg_failed;
    }

    return ret;
dg_failed:
    DebugFs_Clean();
    deregisterDriver(&g_CIKernelInfo);
    KRN_CI_DriverDestroy(&sFelixDriver);
    freeDevice(&g_CIKernelInfo);
#endif // FELIX_HAS_DG

    return ret;
}

// 0 is success
static int __init felix_init(void)
{
    int ret = 0;
#ifdef INFOTM_ISP
	struct clk * busclk;
#endif //INFOTM_ISP

    CI_INFO("driver initialisation\n");

    printk("**************************************\n");
    printk("*** ISP driver 2.7.0 v2016.12.16-1 ***\n");
    printk("**************************************\n");

    sSysDevice.probeSuccess = &probeSuccess;
    sSysDevice.pszDeviceName = DEV_NAME;

	// set ISP bus clock.
	//==============================================
#ifdef INFOTM_ISP
	busclk = clk_get_sys("imap-isp", "imap-isp");
	if (IS_ERR(busclk)) {
		pr_err("fail to get clk 'bus1'\n");
		return PTR_ERR(busclk);
	}
	//ui32ClkRate = 256000000/*175000000*//*300000000*/;
	ret = clk_set_rate(busclk, ui32ClkRate);
	if (0 > ret) {
		printk("failed to set imap isp clock\n");
	} else {
		clk_prepare_enable(busclk);
	}
	printk("isp working clock -> %d\n", (IMG_UINT32)clk_get_rate(busclk));

    module_power_on(SYSMGR_ISP_BASE);
    module_reset(SYSMGR_ISP_BASE, 0);
    module_reset(SYSMGR_ISP_BASE, 1);

#endif //INFOTM_ISP

    if ( (ret=SYS_DevRegister(&(sSysDevice))) != IMG_SUCCESS )
    {
        CI_FATAL("Failed to register the device driver!\n");
        return toErrno(ret);
    }

    return 0;
}

// 0 is success
static void __exit felix_exit(void)
{
    int ret = 0;

    if ( g_psCIDriver )
    {
#ifdef FELIX_HAS_DG
        deregisterDriver(&g_DGKernelInfo);

        ret=KRN_DG_DriverDestroy(&sDGDriver);
        CI_DEBUG("felix DG driver stopped - returned %d\n", ret);

        freeDevice(&g_DGKernelInfo);
#endif // FELIX_HAS_DG

        DebugFs_Clean();
        deregisterDriver(&g_CIKernelInfo);

        ret = KRN_CI_DriverDestroy(&sFelixDriver);
        CI_DEBUG("felix driver stopped - returned %d\n", ret);

        freeDevice(&g_CIKernelInfo);

        // removing the device after all connections are closed to insure interrupts are disabled by the time the device is removed
        SYS_DevRemove(&(sSysDevice));
    }
    SYS_DevDeregister(&(sSysDevice));

#ifdef INFOTM_ISP
	module_power_down(SYSMGR_ISP_BASE);
#endif //INFOTM_ISP	

}

module_init(felix_init);
module_exit(felix_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Imagination Technologies Ltd");
#ifdef FELIX_HAS_DG
MODULE_DESCRIPTION("The Felix ISP and Data Generator drivers");
#else
MODULE_DESCRIPTION("The Felix ISP driver");
#endif
