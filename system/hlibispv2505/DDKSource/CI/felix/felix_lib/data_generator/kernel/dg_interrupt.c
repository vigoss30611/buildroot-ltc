/**
*******************************************************************************
@file dg_interrupt.c

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

#include "dg_kernel/dg_debug.h"
#include "ci_kernel/ci_kernel.h"  // KRM_CI_MMU functions

#include <mmulib/mmu.h>

#include <tal.h>
#include <reg_io2.h>

#include <registers/ext_data_generator.h>
#include <registers/mmu.h>

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
#if FELIX_VERSION_MAJ < 3
            status.ui32DGFrameSent[i] = REGIO_READ_FIELD(thisDGStatus,
                FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
#else
            status.ui32DGFrameSent[i] = REGIO_READ_FIELD(thisDGStatus,
                FELIX_TEST_DG, DG_STATUS, DG_CAPTURES_SENT);
#endif
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
        // note: free in threaded handler
        pStatus = IMG_CALLOC(1, sizeof(struct HW_DG_IRQ_STATUS));
#else
        // note: free in threaded handler
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
        // IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
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
                sCell_T *pHead = NULL;
                int mask =
                    FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_END_OF_FRAME_MASK
                    | FELIX_TEST_DG_DG_INTER_STATUS_DG_INT_ERROR_MASK;

                // should always have a head but verify just in case
                pHead = List_getHead(&(pCamera->sList_pending));
                if (pHead)
                {
                    KRN_DG_FRAME *pFrame = container_of(pHead, KRN_DG_FRAME,
                        sListCell);

                    // 2 clocks per pixels
                    // +40 Horiz blanking always added to register
                    pollTime = 2 * (pFrame->ui16Width
                        + pFrame->ui32HoriBlanking + DG_H_BLANKING_SUB)
                        *(pFrame->ui16Height
                        + pFrame->ui32VertBlanking + 15);

                    // HW testbench has a maximum
                    if (pollTime / pollTries >= CI_REGPOLL_MAXTIME)
                    {
                        pollTries = pollTime / CI_REGPOLL_MAXTIME + 1;
                    }

                    TALPDUMP_Comment(g_pDGDriver->hRegFelixDG[i], "");
                    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[i],
                        "poll when choosing which DG got an interrupt");
                    TALPDUMP_Comment(g_pDGDriver->hRegFelixDG[i], "");
                    /* done to allow the pdump to be playable with the same
                     * order of execution */
                    TALREG_Poll32(g_pDGDriver->hRegFelixDG[i],
                        FELIX_TEST_DG_DG_INTER_STATUS_OFFSET,
                        TAL_CHECKFUNC_ISEQUAL, thisDGVal, mask,
                        CI_REGPOLL_TRIES*pollTries, pollTime / pollTries);
                }

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
                // maximum of frame sent
                IMG_UINT16 tmp =
#if FELIX_VERSION_MAJ < 3
                    (1 << (FELIX_TEST_DG_DG_STATUS_DG_FRAMES_SENT_LENGTH));
#else
                    (1 << (FELIX_TEST_DG_DG_STATUS_DG_CAPTURES_SENT_LENGTH));
#endif
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
                iret = KRN_DG_CameraFrameCaptured(pCamera);

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
                // dirIndex used to know if it is a page fault
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
                    pStatus->mmuStatus0 & ~0x1));

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
        // IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
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
        /* was not allocated with IMG_MALLOC to use GFP_ATOMIC */
        kfree(pStatus);
#endif
    }

    return ret;
}

IMG_RESULT KRN_DG_CameraFrameCaptured(KRN_DG_CAMERA *pCamera)
{
    sCell_T *pHead = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pCamera);

    SYS_LockAcquire(&(pCamera->sLock));
    {
        pHead = List_popFront(&(pCamera->sList_pending));

        /* received an interrupt without pending frame is erroneous!
        * but it could happen if we force a stop before the interrupt */
        if (pHead)
        {
            KRN_DG_FRAME *pFrame = container_of(pHead, KRN_DG_FRAME,
                sListCell);
            CI_INFO("DG %d completed frame %u\n",
                pCamera->publicCamera.ui8Gasket,
                pFrame->identifier);

            List_pushBack(&(pCamera->sList_processed), pHead);
            SYS_SemIncrement(&(pCamera->sSemProcessed));

            // get the new head
            pHead = List_getHead(&(pCamera->sList_pending));
            if (pHead)  // NULL if no elements
            {
                pFrame = container_of(pHead, KRN_DG_FRAME, sListCell);
                ret = HW_DG_CameraSetNextFrame(pCamera, pFrame);
                if (ret)
                {
                    CI_FATAL("Failed to enqueue next frame!\n");
                }
            }
        }
    }
    SYS_LockRelease(&(pCamera->sLock));

    return IMG_SUCCESS;
}
