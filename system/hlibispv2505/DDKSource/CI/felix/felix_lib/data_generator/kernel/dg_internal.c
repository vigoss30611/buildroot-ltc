/**
*******************************************************************************
@file dg_internal.c

@brief Internal implementation of the kernel-module for the Data Generator

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
#include "dg_kernel/dg_camera.h"

#include <img_defs.h>
#include "dg_kernel/dg_connection.h"
#include "dg_kernel/dg_debug.h"
#include "ci_kernel/ci_kernel.h"  // KRM_CI_MMU functions

#include <tal.h>
#include <reg_io2.h>

#include <mmulib/mmu.h>
#include <mmulib/heap.h>

#ifdef FELIX_FAKE

// use talalloc to have correct pdumps
#include <talalloc/talalloc.h>

#endif /* FELIX_FAKE */

#ifdef FELIX_UNIT_TESTS
#include "unit_tests.h"  // gbUseTalNULL
#endif

#ifndef __maybe_unused
#define __maybe_unused
#endif

KRN_DG_DRIVER *g_pDGDriver = NULL;

// cannot store DG directories in generic structure
IMG_STATIC_ASSERT(CI_N_EXT_DATAGEN <= CI_MMU_N_DIR, More_DG_than_MMU_dirs);
IMG_STATIC_ASSERT(CI_N_EXT_DATAGEN <= CI_MMU_N_REQ, More_DG_than_MMU_reqs);
IMG_STATIC_ASSERT(DG_N_HEAPS == 2, DG_N_HEAPS_CHANGED);

static void List_destroyConnection(void *listelem)
{
    KRN_DG_CONNECTION *pConn = (KRN_DG_CONNECTION*)listelem;

    KRN_DG_ConnectionDestroy(pConn);
}

IMG_RESULT KRN_DG_DriverCreate(KRN_DG_DRIVER *pDriver,
    IMG_UINT8 ui8EDGPLL_mult, IMG_UINT8 ui8EDGPLL_div,
    IMG_UINT8 ui8EnableMMU)
{
    IMG_INT32 i;
    IMG_RESULT ret = IMG_SUCCESS;

    if (g_pDGDriver != NULL)
    {
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    IMG_MEMSET(pDriver, 0, sizeof(KRN_DG_DRIVER));

    // cannot store DG heaps in generic structure
    if ((int)DG_N_HEAPS > (int)CI_MMU_N_HEAPS)
    {
        CI_FATAL("cannot store DG_N_HEAPS > CI_MMU_N_HEAPS "\
            "- compile the driver with more heaps\n");
        return IMG_ERROR_FATAL;
    }

    // owner is set by the insmode init function
    pDriver->sDevice.sFileOps.open = &DEV_DG_Open;
    pDriver->sDevice.sFileOps.release = &DEV_DG_Close;
    pDriver->sDevice.sFileOps.unlocked_ioctl = &DEV_DG_Ioctl;
    pDriver->sDevice.sFileOps.mmap = &DEV_DG_Mmap;

    ret = List_init(&(pDriver->sList_connection));
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to initialise internal list\n");
        return ret;
    }
    ret = List_init(&(pDriver->sWorkqueue));
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to initialise workqueue\n");
        return ret;
    }

    ret = SYS_LockInit(&(pDriver->sConnectionLock));
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to create connection lock\n");
        return ret;
    }

    ret = SYS_LockInit(&(pDriver->sCameraLock));
    if (ret)  // unlikely
    {
        CI_FATAL("Failed to create camera lock\n");
        SYS_LockDestroy(&(pDriver->sConnectionLock));
        // no need to clear the list nothing was added to it
        return ret;
    }

    ret = SYS_SpinlockInit(&(pDriver->sWorkSpinlock));
    if (ret)
    {
        CI_FATAL("Failed to create work spinlock\n");
        SYS_LockDestroy(&(pDriver->sCameraLock));
        SYS_LockDestroy(&(pDriver->sConnectionLock));
        // no need to clear the list nothing was added to it
        return ret;
    }

    // not using pDriver->ui8NDataGen because creating objects
    // TAL handles for unexisting memory is not going to be used
    for (i = 0; i < CI_N_EXT_DATAGEN; i++)
    {
        char talname[32];

        snprintf(talname, sizeof(talname), "REG_TEST_DG_%d", i);
        pDriver->hRegFelixDG[i] = TAL_GetMemSpaceHandle(talname);
    }

    pDriver->hRegTestIO = TAL_GetMemSpaceHandle("REG_TEST_IO");
    pDriver->hRegGasketPhy = TAL_GetMemSpaceHandle("REG_GASK_PHY_0");
    pDriver->sMMU.hRegFelixMMU = TAL_GetMemSpaceHandle("REG_TEST_MMU");
    // equivalent to TAL_GetMemSpaceHandle("SYSMEM");
    pDriver->hMemHandle = g_psCIDriver->sTalHandles.hMemHandle;

    /*
     * get the HW info from CI -  change to read from registers
     */

    pDriver->sHWInfo.ui8GroupId = g_psCIDriver->sHWInfo.ui8GroupId;
    pDriver->sHWInfo.ui8AllocationId = g_psCIDriver->sHWInfo.ui8AllocationId;
    pDriver->sHWInfo.ui16ConfigId = g_psCIDriver->sHWInfo.ui16ConfigId;

    pDriver->sHWInfo.rev_ui8Designer = g_psCIDriver->sHWInfo.rev_ui8Designer;
    pDriver->sHWInfo.rev_ui8Major = g_psCIDriver->sHWInfo.rev_ui8Major;
    pDriver->sHWInfo.rev_ui8Minor = g_psCIDriver->sHWInfo.rev_ui8Minor;
    pDriver->sHWInfo.rev_ui8Maint = g_psCIDriver->sHWInfo.rev_ui8Maint;

    pDriver->sHWInfo.rev_uid = g_psCIDriver->sHWInfo.rev_uid;

    pDriver->sHWInfo.config_ui8Parallelism =
        g_psCIDriver->sHWInfo.config_ui8Parallelism;

    for (i = 0; i < CI_N_IMAGERS; i++)
    {
        pDriver->sHWInfo.gasket_aImagerPortWidth[i] =
            g_psCIDriver->sHWInfo.gasket_aImagerPortWidth[i];
        pDriver->sHWInfo.gasket_aType[i] =
            g_psCIDriver->sHWInfo.gasket_aType[i];
        pDriver->sHWInfo.gasket_aMaxWidth[i] =
            g_psCIDriver->sHWInfo.gasket_aMaxWidth[i];
        pDriver->sHWInfo.gasket_aBitDepth[i] =
            g_psCIDriver->sHWInfo.gasket_aBitDepth[i];
        pDriver->sHWInfo.gasket_aNVChan[i] = 1;
    }

    // number of DG is the same than the number of gaskets
    pDriver->sHWInfo.config_ui8NDatagen =
        g_psCIDriver->sHWInfo.config_ui8NImagers;

    if (pDriver->sHWInfo.config_ui8NDatagen > CI_N_EXT_DATAGEN
        || 0 == pDriver->sHWInfo.config_ui8NDatagen)
    {
        CI_FATAL("Driver compiled to support %d data generators but HW "\
            "has %d.\n",
            CI_N_EXT_DATAGEN, pDriver->sHWInfo.config_ui8NDatagen);
        SYS_LockDestroy(&(pDriver->sConnectionLock));
        SYS_LockDestroy(&(pDriver->sCameraLock));
        SYS_SpinlockDestroy(&(pDriver->sWorkSpinlock));
        // no need to clear the list nothing was added to it
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (ui8EnableMMU > 0)  // if it is using the MMU
    {
        /* max 32b value - as the maximum memory usable in virtual address
         * (it is always 32b) */
        IMG_UINT64 max = 0x100000000;
        IMG_UINT64 total = 0;

        // data heap size
        pDriver->sMMU.aHeapSize[1] = 1 << (9 + 20);  // 256 MB

        /* image heap - use all memory but the bottom 256 MB and 1st page
         * is avoided to have virtual address 0 invalid */
        pDriver->sMMU.uiAllocAtom = PAGE_SIZE;
        pDriver->sMMU.aHeapStart[0] = PAGE_SIZE;
        pDriver->sMMU.aHeapSize[0] =
            (IMG_INT32)((max - pDriver->sMMU.aHeapSize[1]) - PAGE_SIZE)
            / PAGE_SIZE *PAGE_SIZE;  // remaining of the memory
        /* remove last page because kernel before 3.12 have trouble with it
         * on 32b machines when using genalloc (and it's only 4kB less on
         * 1.8GB) */
        pDriver->sMMU.aHeapSize[0] -= PAGE_SIZE;
        total = pDriver->sMMU.aHeapStart[0] + pDriver->sMMU.aHeapSize[0];

        // data heap starts
        pDriver->sMMU.aHeapStart[1] = (IMG_UINT32)total;
        total += pDriver->sMMU.aHeapSize[1];

        IMG_ASSERT(total <= max);

        for (i = 0; i < pDriver->sHWInfo.config_ui8NDatagen; i++)
        {
            pDriver->sMMU.apDirectory[i] = KRN_CI_MMUDirCreate(NULL, &ret);
            if (!pDriver->sMMU.apDirectory[i])
            {
                i--;
                while (i >= 0)
                {
                    KRN_CI_MMUDirDestroy(pDriver->sMMU.apDirectory[i]);
                    i--;
                }
                CI_FATAL("Failed to create DG MMU directory %d (HW has %d)\n",
                    i, pDriver->sHWInfo.config_ui8NDatagen);
                ret = IMG_ERROR_NOT_SUPPORTED;
                SYS_LockDestroy(&(pDriver->sConnectionLock));
                SYS_LockDestroy(&(pDriver->sCameraLock));
                SYS_SpinlockDestroy(&(pDriver->sWorkSpinlock));
                // no need to clear the list nothing was added to it
                return IMG_ERROR_NOT_SUPPORTED;
            }
            // configure 1 requestor per MMU directory
            pDriver->sMMU.aRequestors[i] = i;
        }

        pDriver->sMMU.bEnableExtAddress = ui8EnableMMU > 1;

        CI_INFO("DG MMU pre-configured (ext address range %d)\n",
            pDriver->sMMU.bEnableExtAddress);
    }

    // check PLLs
    if (0 == ui8EDGPLL_mult || 0 == ui8EDGPLL_div)
    {
        IMG_UINT8 def_pll = pDriver->sHWInfo.config_ui8Parallelism;
        def_pll = 10 * IMG_MAX_INT(def_pll, 2);

        // default pll is same for mult and div based on parallelism
        pDriver->aEDGPLL[0] = def_pll;  // multiplier
        pDriver->aEDGPLL[1] = def_pll;  // divider
        CI_WARNING("given pll %d/%d but using default %d/%d\n",
            (int)ui8EDGPLL_mult, (int)ui8EDGPLL_div,
            (int)def_pll, (int)def_pll);
    }
    else
    {
        // values should be checked by HW_DG_WritePLL() when writing plls
        pDriver->aEDGPLL[0] = ui8EDGPLL_mult;  // multiplier
        pDriver->aEDGPLL[1] = ui8EDGPLL_div;  // divider
    }

    g_pDGDriver = pDriver;

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_DriverDestroy(KRN_DG_DRIVER *pDriver)
{
    IMG_UINT32 i;
    sCell_T* pCurrent;
    IMG_ASSERT(pDriver == g_pDGDriver);  // because if not it's a bug

    g_pDGDriver = NULL;  // so nobody can do anything

    IMG_ASSERT(pDriver->sWorkqueue.ui32Elements == 0);

    // the MMU is not stopped nor the page table not used freed
    // but there should be no connection left if this is a legit rmmod
    pCurrent = List_popFront(&(pDriver->sList_connection));
    while (pCurrent)
    {
        CI_DEBUG("Destroying connection...\n");
        KRN_DG_ConnectionDestroy((KRN_DG_CONNECTION*) pCurrent->object);
        pCurrent = List_popFront(&(pDriver->sList_connection));
    }

    /* not using g_pDGDriver->ui8NDataGen because CI_N_EXT_DATAGEN is used
     * for creation */
    for (i = 0; i < pDriver->sHWInfo.config_ui8NDatagen; i++)
    {
        // we could protect lock destruction... but we are quitting anyway!
        KRN_CI_MMUDirDestroy(pDriver->sMMU.apDirectory[i]);
        pDriver->sMMU.apDirectory[i] = NULL;
    }
    SYS_LockDestroy(&(pDriver->sCameraLock));
    SYS_LockDestroy(&(pDriver->sConnectionLock));
    SYS_SpinlockDestroy(&(pDriver->sWorkSpinlock));

    return IMG_SUCCESS;
}

IMG_RESULT DEV_DG_Suspend(SYS_DEVICE *pDev, pm_message_t state)
{
    //  implement suspend (but not possible on FPGA)
    /* could stop running DG here - the problem being we can't stop a
     * processing DG or reset it because memory requests may be lost
     * so we may need to reset the MMU and the DG in a specific order to
     * avoid complications
     * But because ext DG is only available on FPGA and not to customers
     * we don't need to cope with suspend */
    return IMG_SUCCESS;
}

IMG_RESULT DEV_DG_Resume(SYS_DEVICE *pDev)
{
    IMG_RESULT ret = IMG_ERROR_UNEXPECTED_STATE;
    if (g_pDGDriver)
    {
        IMG_UINT8 i;

        CI_DEBUG("reset DG banks\n");
        CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[0],
            "reset DG banks before setting default PLL");
        for (i = 0; i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
        {
            HW_DG_CameraReset(i);
        }

        CI_DEBUG("reset PLL to %d/%d\n", (int)g_pDGDriver->aEDGPLL[0],
            (int)g_pDGDriver->aEDGPLL[1]);
        ret = HW_DG_WritePLL(g_pDGDriver->aEDGPLL[0], g_pDGDriver->aEDGPLL[1]);
        if (ret)
        {
            CI_FATAL("failed to set default PLL!\n");
        }
    }
#ifdef FELIX_UNIT_TESTS
    else
    {
        /* when running unit tests it is possible that the DG kernel object
         * isn't created when the felix kernel object is created */
        ret = IMG_SUCCESS;
    }
#endif
    return ret;
}

IMG_RESULT KRN_DG_DriverAcquireContext(KRN_DG_CAMERA *pCamera)
{
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8Gasket;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(g_pDGDriver);  // assumes CI driver exists

    if (((pCamera->publicCamera.eFormat >= CI_DGFMT_MIPI)
        && (g_pDGDriver->sHWInfo.\
        gasket_aType[ui8Context] & CI_GASKET_MIPI) == 0)
        || (pCamera->publicCamera.eFormat == CI_DGFMT_PARALLEL
        && (g_pDGDriver->sHWInfo.\
        gasket_aType[ui8Context] & CI_GASKET_PARALLEL) == 0)
        )
    {
        CI_FATAL("DG Context %d uses an uncompatible Gasket Format for "\
            "ISP gasket\n", pCamera->publicCamera.ui8Gasket);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
    {
        if (!g_pDGDriver->aActiveCamera[ui8Context])
        {
            g_pDGDriver->aActiveCamera[ui8Context] = pCamera;
        }
        else
        {
            CI_FATAL("DG Context %d is used\n", ui8Context);
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
    }
    SYS_LockRelease(&(g_pDGDriver->sCameraLock));

    return ret;
}

IMG_RESULT KRN_DG_DriverReleaseContext(KRN_DG_CAMERA *pCamera)
{
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8Gasket;
    IMG_RESULT ret = IMG_SUCCESS;

    SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
    {
        if (g_pDGDriver->aActiveCamera[ui8Context] == pCamera)
        {
            g_pDGDriver->aActiveCamera[ui8Context] = NULL;
        }
        else
        {
            CI_FATAL("DG Context %d is not used by this DG Camera\n",
                ui8Context);
            ret = IMG_ERROR_UNEXPECTED_STATE;
        }
    }
    SYS_LockRelease(&(g_pDGDriver->sCameraLock));

    return ret;
}
