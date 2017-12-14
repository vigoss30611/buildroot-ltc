/**
******************************************************************************
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
#include "ci_kernel/ci_kernel.h" // KRM_CI_MMU functions

#include <tal.h>
#include <reg_io2.h>

#include <mmulib/mmu.h>
#include <mmulib/heap.h>

#ifdef FELIX_FAKE

#include <talalloc/talalloc.h>

#endif // CI_MMU_TAL_ALLOC

#include <registers/ext_data_generator.h>
//#include <hw_struct/ext_data_generation.h> // dg linked list is not used
#include <registers/mmu.h>

#ifdef FELIX_UNIT_TESTS
#include "unit_tests.h" // gbUseTalNULL
#endif

#ifndef __maybe_unused
#define __maybe_unused
#endif

KRN_DG_DRIVER *g_pDGDriver = NULL;

#define DG_INT_STRING(regVal) \
    (regVal&FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_ERROR_MASK?"error":\
    (regVal&FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK?"end of frame":\
    (regVal&FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_START_OF_FRAME_MASK?"start of frame":"unknown"\
    )))

#define INT_MASK 2
static IMG_UINT32 masks[INT_MASK] __maybe_unused = {
    FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK,
    FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_ERROR_MASK
};

IMG_STATIC_ASSERT(CI_N_EXT_DATAGEN <= CI_MMU_N_DIR, More_DG_than_MMU_dirs); // cannot store DG directories in generic structure
IMG_STATIC_ASSERT(CI_N_EXT_DATAGEN <= CI_MMU_N_REQ, More_DG_than_MMU_reqs); // cannot store DG requestors in genereic structure

static void List_destroyConnection(void *listelem)
{
    KRN_DG_CONNECTION *pConn = (KRN_DG_CONNECTION*)listelem;

    KRN_DG_ConnectionDestroy(pConn);
}

IMG_STATIC_ASSERT(DG_N_HEAPS == 2, DG_N_HEAPS_CHANGED);

IMG_RESULT KRN_DG_DriverCreate(KRN_DG_DRIVER *pDriver, IMG_UINT8 ui8EnableMMU)
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

        sprintf(talname, "REG_TEST_DG_%d", i);
        pDriver->hRegFelixDG[i] = TAL_GetMemSpaceHandle(talname);
    }

    pDriver->hRegTestIO = TAL_GetMemSpaceHandle("REG_TEST_IO");
    pDriver->sMMU.hRegFelixMMU = TAL_GetMemSpaceHandle("REG_TEST_MMU");
    // equivalent to TAL_GetMemSpaceHandle("SYSMEM");
    pDriver->hMemHandle = g_psCIDriver->sTalHandles.hMemHandle;

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

    if (ui8EnableMMU > 0) // if it is using the MMU
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

    g_pDGDriver = pDriver;

    return IMG_SUCCESS;
}

struct HW_DG_IRQ_STATUS
{
    sCell_T sCell;
    IMG_UINT32 ui32DGStatus[CI_N_EXT_DATAGEN];
    IMG_UINT32 ui32DGFrameSent[CI_N_EXT_DATAGEN];

    IMG_UINT32 mmuStatus0;
    IMG_UINT32 mmuStatus1;
    IMG_UINT32 mmuControl;
};

irqreturn_t KRN_DG_DriverHardHandleInterrupt(int irq, void *dev_id)
{
    int i;
    irqreturn_t ret = IRQ_NONE;
    struct HW_DG_IRQ_STATUS status;
    struct timespec t0, t1;

#ifdef FELIX_UNIT_TESTS
    if (!g_pDGDriver)
    {
        /* because it is legitimate not to have dg driver when running
         * unit tests */
        return IRQ_WAKE_THREAD;
    }
#else
    // but it is not allowed not to have a dg driver otherwise
    IMG_ASSERT(g_pDGDriver);
#endif

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "start");
    CI_DEBUG("Interrupt %d\n", irq);

    IMG_MEMSET(&status, 0, sizeof(struct HW_DG_IRQ_STATUS));
    getnstimeofday(&t0);

    for (i = 0; i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
    {
        IMG_UINT32 thisDGVal;

        TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[i],
            FELIX_TEST_DG_DG_INTER_STATUS_OFFSET, &thisDGVal);

        if (thisDGVal)
        {
            IMG_UINT32 thisDGStatus;
            status.ui32DGStatus[i] = thisDGVal;

            ret = IRQ_WAKE_THREAD;

            TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[i],
                FELIX_TEST_DG_DG_STATUS_OFFSET, &thisDGStatus);
            status.ui32DGFrameSent[i] = REGIO_READ_FIELD(thisDGStatus,
                FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
        }

#ifdef FELIX_FAKE
        // clear interrupts - but not frame done because it was already cleared
        TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[i],
            FELIX_TEST_DG_DG_INTER_CLEAR_OFFSET,
            thisDGVal
            &(~FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK));

        /// @ enable when CSIM supports it
        /*TALREG_Poll32(g_pDGDriver->hRegTestIO,
        FELIX_TEST_IO_FELIX_IRQ_STATUS_OFFSET, TAL_CHECKFUNC_ISEQUAL,
        0, 0x1, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);*/
#else
        // clear interrupts
        TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[i],
            FELIX_TEST_DG_DG_INTER_CLEAR_OFFSET, thisDGVal);
#endif

#ifdef FELIX_UNIT_TESTS
        // reset the interrupt value
        TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[i],
            FELIX_TEST_DG_DG_INTER_STATUS_OFFSET, 0);
#endif
    }

    if (g_pDGDriver->sMMU.apDirectory[0])
    {
        /* read interrupt source to know if MMU has interrupt because
         * MMU_STATUS0 can legetimatly be 0 */
        /* but the HW does not give that source for external DG and as we
         * do not use the first page for mapping address 0 cannot be used */
        TALREG_ReadWord32(g_pDGDriver->sMMU.hRegFelixMMU,
            IMG_VIDEO_BUS4_MMU_MMU_STATUS0_OFFSET, &(status.mmuStatus0));
        TALREG_ReadWord32(g_pDGDriver->sMMU.hRegFelixMMU,
            IMG_VIDEO_BUS4_MMU_MMU_STATUS1_OFFSET, &(status.mmuStatus1));
        TALREG_ReadWord32(g_pDGDriver->sMMU.hRegFelixMMU,
            IMG_VIDEO_BUS4_MMU_MMU_ADDRESS_CONTROL_OFFSET,
            &(status.mmuControl));

        if (status.mmuStatus0)
        {
            ret = IRQ_WAKE_THREAD;
        }
    }

    if (IRQ_WAKE_THREAD == ret)
    {
        struct HW_DG_IRQ_STATUS *pStatus = NULL;
#if defined(FELIX_FAKE)
        pStatus = IMG_CALLOC(1, sizeof(struct HW_DG_IRQ_STATUS));
#else
        pStatus = kcalloc(1, sizeof(struct HW_DG_IRQ_STATUS), GFP_ATOMIC);
#endif
        if (!pStatus)
        {
            CI_FATAL("failed to allocate atomic element\n");
            return IRQ_NONE;
        }

        IMG_MEMCPY(pStatus, &status, sizeof(struct HW_DG_IRQ_STATUS));
        pStatus->sCell.object = pStatus;

        SYS_SpinlockAcquire(&(g_pDGDriver->sWorkSpinlock));
        {
            List_pushBack(&(g_pDGDriver->sWorkqueue), &(pStatus->sCell));
        }
        SYS_SpinlockRelease(&(g_pDGDriver->sWorkSpinlock));
    }

    getnstimeofday(&t1);

#if defined(W_TIME_PRINT)
    W_TIME_PRINT(t0, t1, "=");
#endif
#if defined(CI_DEBUGFS)
    {
        IMG_INT64 diff =
            (IMG_INT64)((t1.tv_nsec - t0.tv_nsec) / 1000 % 1000000);
        //IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
        g_ui32NServicedDGHardInt++;

        if (diff > g_i64LongestDGHardIntUS)
        {
            g_i64LongestDGHardIntUS = diff;
        }
    }
#endif

    return ret;
}

irqreturn_t KRN_DG_DriverThreadHandleInterrupt(int irq, void *dev_id)
{
    int i;
    irqreturn_t ret = IRQ_NONE;
    struct HW_DG_IRQ_STATUS *pStatus = NULL;
    struct timespec t0, t1;

#ifdef FELIX_FAKE
    // so that pdump script has enough time between frames
    IMG_UINT32 pollTime = 0;
    IMG_UINT32 pollTries = 2;

    if (!g_pDGDriver)
    {
#ifdef FELIX_UNIT_TESTS
        // because it is legit not to have dg driver when running unit tests
        return IRQ_HANDLED;
#else
        // but we must have a driver
        return IRQ_NONE;
#endif
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "start");

#else
    if (!g_pDGDriver)
    {
        CI_FATAL("try to handle interupt without a driver!\n");
        return IRQ_NONE;
    }
    CI_DEBUG("Interrupt %d\n", irq);
#endif /* FELIX_FAKE */

    getnstimeofday(&t0);

    SYS_SpinlockAcquire(&(g_pDGDriver->sWorkSpinlock));
    {
        sCell_T *pHead = List_popFront(&(g_pDGDriver->sWorkqueue));
        if (pHead)
        {
            pStatus = container_of(pHead, struct HW_DG_IRQ_STATUS, sCell);
        }
    }
    SYS_SpinlockRelease(&(g_pDGDriver->sWorkSpinlock));

    if (!pStatus)
    {
        CI_FATAL("did not find a status to handle!\n");
        return IRQ_NONE;
    }

    for (i = 0; i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
    {
        IMG_UINT32 thisDGVal = pStatus->ui32DGStatus[i];

#ifdef CI_DEBUGFS
        g_ui32NDGInt[i] += (thisDGVal
            & FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK)?1:0;
#endif

        if ((thisDGVal
            & FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK) != 0)
        {
            KRN_DG_CAMERA *pCamera = NULL;
            IMG_UINT32 frameSent = 0;
            int safe = 0;

            ret = IRQ_HANDLED;
            CI_DEBUG("--- DG context %d handles 0x%x ---\n", i, thisDGVal);

            CI_DEBUG("--- lock cameraLock\n");
            SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
            {
                pCamera = g_pDGDriver->aActiveCamera[i];
            }
            SYS_LockRelease(&(g_pDGDriver->sCameraLock));
            CI_DEBUG("--- unlock cameraLock\n");

            if (!pCamera)
            {
                CI_FATAL("could not find an associated camera object "\
                    "for DG %d\n", i);
                continue;
            }

#ifdef FELIX_FAKE
            {
                int mask =
                    FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK
                    | FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_ERROR_MASK;

                // 2 clocks per pixels
                // +40 Horiz blanking always added to register
                pollTime = 2 * (pCamera->publicCamera.uiWidth
                    + pCamera->publicCamera.ui16HoriBlanking + 40)
                    *(pCamera->publicCamera.uiHeight
                    + pCamera->publicCamera.ui16VertBlanking + 15);

                // HW testbench has a maximum
                if (pollTime / pollTries >= CI_REGPOLL_MAXTIME)
                {
                    pollTries = pollTime / CI_REGPOLL_MAXTIME + 1;
                }

                TALPDUMP_Comment(g_pDGDriver->hRegFelixDG[i], "");
                CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[i],
                    "poll when choosing which DG got an interrupt");
                TALPDUMP_Comment(g_pDGDriver->hRegFelixDG[i], "");
                /* done to allow the pdump to be playable with the same order
                 * of execution */
                TALREG_Poll32(g_pDGDriver->hRegFelixDG[i],
                    FELIX_TEST_DG_DG_INTER_STATUS_OFFSET,
                    TAL_CHECKFUNC_ISEQUAL, thisDGVal, mask,
                    CI_REGPOLL_TRIES*pollTries, pollTime / pollTries);

                /* clear interrupt because submitting next frame on dg may
                 * be troublesome when having small images and running pdump */
                TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[i],
                    FELIX_TEST_DG_DG_INTER_CLEAR_OFFSET,
                    thisDGVal
                    &FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK);
            }
#endif
            frameSent = pStatus->ui32DGFrameSent[i];

            // POL on frameSent?

            // handle the frame(s)
            if (frameSent < g_pDGDriver->aFrameSent[i])
            {
                //maximum of frame sent
                IMG_UINT16 tmp =
                    (1 << (FELIX_TEST_DG_DG_STATUS_DG_FRAMES_SENT_LENGTH));
                // number of frames to add to frameSent
                tmp = tmp - g_pDGDriver->aFrameSent[i];
                // save previously read number
                g_pDGDriver->aFrameSent[i] = frameSent;
                frameSent += tmp;  // number of interrupts generated
            }
            else
            {
                IMG_UINT16 tmp = frameSent - g_pDGDriver->aFrameSent[i];
                g_pDGDriver->aFrameSent[i] = frameSent;
                frameSent = tmp;
            }

            if (frameSent == 0)
            {
                CI_DEBUG("DG interrupt but no frame! interrupt=%d\n",
                    thisDGVal);
            }
            else if (frameSent > 1)
            {
                // not normally possible as we are running it manually!
                CI_WARNING("DG has more than 1 frame pending!\n");
            }

            /* this is most likely 1, but could be 2 if we missed an interrupt
             * it isn't likely to be more than 1 as we run the DG frame
             * by frame */
            safe = 0;
            while (frameSent > 0 && safe < 5)
            {
                IMG_RESULT iret = IMG_SUCCESS;

                CI_DEBUG("--- lock pendingLock (dg)\n");
                SYS_LockAcquire(&(pCamera->sPendingLock));
                {
                    iret = KRN_DG_CameraFrameCaptured(pCamera);
                }
                SYS_LockRelease(&(pCamera->sPendingLock));
                CI_DEBUG("--- unlock pendingLock (dg)\n");

                if (iret)
                {
                    CI_FATAL("could not handle DG interrupt\n");
                    break;
                }

                frameSent--;
                safe++;
            }
            CI_DEBUG("DG handled %d interrupts\n", safe);
        }
    }  // for each DG

    if (g_pDGDriver->sMMU.apDirectory[0])
    {
        /* read interrupt source to know if MMU has interrupt because
         * MMU_STATUS0 can legetimatly be 0
         * but the HW does not give that source for external DG and as we
         * do not use the first page for mapping address 0 cannot be used */

        IMG_UINT32 dirIndex = 0;

        if (pStatus->mmuStatus0)
        {
            CI_FATAL("--- Ext MMU fault status0=0x%x status1=0x%x "\
                "addr_ctrl=0x%x (page fault %d, read fault %d, "\
                "write fault %d, ext adressing %d) ---\n",
                pStatus->mmuStatus0, pStatus->mmuStatus1, pStatus->mmuControl,
                REGIO_READ_FIELD(pStatus->mmuStatus0,
                IMG_VIDEO_BUS4_MMU, MMU_STATUS0, MMU_PF_N_RW),
                REGIO_READ_FIELD(pStatus->mmuStatus1,
                IMG_VIDEO_BUS4_MMU, MMU_STATUS1, MMU_FAULT_RNW),
                REGIO_READ_FIELD(pStatus->mmuStatus1,
                IMG_VIDEO_BUS4_MMU, MMU_STATUS1, MMU_FAULT_RNW) == 0,
                REGIO_READ_FIELD(pStatus->mmuControl,
                IMG_VIDEO_BUS4_MMU, MMU_ADDRESS_CONTROL,
                MMU_ENABLE_EXT_ADDRESSING));

#ifdef FELIX_FAKE
            TALREG_Poll32(g_pDGDriver->sMMU.hRegFelixMMU,
                IMG_VIDEO_BUS4_MMU_MMU_STATUS0_OFFSET,
                TAL_CHECKFUNC_ISEQUAL, pStatus->mmuStatus0, ~0,
                CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif

            dirIndex = REGIO_READ_FIELD(pStatus->mmuStatus1,
                IMG_VIDEO_BUS4_MMU, MMU_STATUS1, MMU_FAULT_INDEX);

            if (dirIndex < CI_N_CONTEXT)
            {
                // status without bottom bit is the virtual address failed at
                REGIO_WRITE_FIELD(pStatus->mmuStatus0,
                    IMG_VIDEO_BUS4_MMU, MMU_STATUS0, MMU_PF_N_RW, 0);
                CI_FATAL("Ext MMU fault @ v0x%x on directory %u - "\
                    "direct entry = 0x%x - page entry = 0x%x\n",
                    pStatus->mmuStatus0, dirIndex,
                    IMGMMU_DirectoryGetDirectoryEntry(
                    g_pDGDriver->sMMU.apDirectory[dirIndex],
                    pStatus->mmuStatus0 & ~0x1),
                    IMGMMU_DirectoryGetPageTableEntry(
                    g_pDGDriver->sMMU.apDirectory[dirIndex],
                    pStatus->mmuStatus0 & ~0x1)
                    );

                /* here we could do something to cope with the fault like
                 * stopping the DG */
                ret = IRQ_HANDLED;
            }
            else
            {
                CI_FATAL("Ext MMU fault @ v0x%x but MMU dir index %u "\
                    "is too big for HW\n",
                    pStatus->mmuStatus0 & ~0x1, dirIndex);
            }
        }
    }

    getnstimeofday(&t1);

#if defined(W_TIME_PRINT)
    W_TIME_PRINT(t0, t1, "=");
#endif
#if defined(CI_DEBUGFS)
    {
        IMG_INT64 diff =
            (IMG_INT64)((t1.tv_nsec - t0.tv_nsec) / 1000 % 1000000);
        //IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
        g_ui32NServicedDGThreadInt++;

        if (diff > g_i64LongestDGThreadIntUS)
        {
            g_i64LongestDGThreadIntUS = diff;
        }
    }
#endif

    if (pStatus)
    {
#if defined(FELIX_FAKE)
        IMG_FREE(pStatus);
#else
        kfree(pStatus);
#endif
    }

    return ret;
}

IMG_RESULT KRN_DG_DriverDestroy(KRN_DG_DRIVER *pDriver)
{
    IMG_UINT32 i;
    IMG_ASSERT(pDriver == g_pDGDriver);  // because if not it's a bug

    g_pDGDriver = NULL;  // so nobody can do anything

    IMG_ASSERT(pDriver->sWorkqueue.ui32Elements == 0);

    // the MMU is not stopped nor the page table not used freed
    // but there should be no connection left if this is a legit rmmod
    List_clearObjects(&(pDriver->sList_connection), &List_destroyConnection);

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

IMG_RESULT KRN_DG_DriverAcquireContext(KRN_DG_CAMERA *pCamera)
{
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8DGContext;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(g_psCIDriver != NULL);

    if (((pCamera->publicCamera.eBufferFormats == DG_MEMFMT_MIPI_RAW10 || pCamera->publicCamera.eBufferFormats == DG_MEMFMT_MIPI_RAW12 || pCamera->publicCamera.eBufferFormats == DG_MEMFMT_MIPI_RAW14)
        && (g_psCIDriver->sHWInfo.gasket_aType[pCamera->publicCamera.ui8DGContext] & CI_GASKET_MIPI) == 0)
        ||
        ((pCamera->publicCamera.eBufferFormats == DG_MEMFMT_BT656_10 || pCamera->publicCamera.eBufferFormats == DG_MEMFMT_BT656_12)
        && (g_psCIDriver->sHWInfo.gasket_aType[pCamera->publicCamera.ui8DGContext] & CI_GASKET_PARALLEL) == 0)
        )
    {
        CI_FATAL("DG Context %d uses an uncompatible Gasket Format for felix gasket\n", pCamera->publicCamera.ui8DGContext);
        return IMG_ERROR_FATAL;
    }


    SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
    {
        if (g_pDGDriver->aActiveCamera[ui8Context] == NULL)
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
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8DGContext;
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
