/**
*******************************************************************************
@file ci_interrupt.c

@brief Internal hardware structures descriptions

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
#include "ci_kernel/ci_kernel.h" // for CI_FATAL

// for interrutp handler in case of page fault
#include <mmulib/mmu.h>
#include "ci_kernel/ci_hwstruct.h"
// to access debugFS variables in interrupt handler
#include "ci_kernel/ci_debug.h"

#ifdef FELIX_HAS_DG
// for interrupt handler of external DG
#include <dg_kernel/dg_camera.h>
// to access debugFS variables in interrupt handler
#include <dg_kernel/dg_debug.h>
// to access test io to check line status register
#include <registers/test_io.h>
#endif

#include <img_types.h>

#include <felix_hw_info.h>

#ifdef CI_PDP
// we need access to PDP function
#include "ci_internal/ci_pdp.h"
#endif

#include <tal.h>
#include <reg_io2.h>

#include <registers/core.h>
#include <registers/context0.h>

#if FELIX_VERSION_MAJ != 2
#error this file is for HW v2
#endif

#include <registers/gasket_mipi.h>
#include <registers/gasket_parallel.h>
#include <registers/data_generator.h>
#include <registers/mmu.h>

#ifdef INFOTM_HW_AWB_METHOD
#include <hw_struct/save.h>
#include "mmulib/heap.h"
#endif //INFOTM_HW_AWB_METHOD

/*
* Interrupt Handler
*/

// to reduce the size of macros
#define INT_ST_MASK(A) FELIX_CONTEXT0_INTERRUPT_STATUS_## A ##_MASK
#define INT_DG_MASK(A) FELIX_DG_IIF_DG_INTER_STATUS_## A ##_MASK

struct HW_CI_IRQ_STATUS
{
    sCell_T sCell;
    IMG_UINT32 hardTimestamps;

    IMG_UINT32 ui32CtxStatus[CI_N_CONTEXT];
    IMG_UINT32 ui32CtxLastFrameInfo[CI_N_CONTEXT];
    IMG_UINT32 ui32IntDGStatus[CI_N_IIF_DATAGEN];
    IMG_UINT32 ui32IntDGFrameCount[CI_N_IIF_DATAGEN];

    IMG_UINT32 ui32GasketFrameCount[CI_N_IMAGERS];

    // used to know if MMU got interrupt
    IMG_UINT32 coreIntSource;
    IMG_UINT32 mmuStatus0;
    IMG_UINT32 mmuStatus1;
    IMG_UINT32 mmuControl;

#ifdef FELIX_HAS_DG
    int dgHardHandle;  // return from hard int
#endif /* FELIX_HAS_DG */
};

//
// DO NOT CALL DIRECTLY! REGISTER IT TO THE INTERRUPT HANDLING MECHANISM
//
irqreturn_t HW_CI_DriverHardHandleInterrupt(int irq, void *dev_id)
{
    //
    // THIS IS IN INTERRUPT CONTEXT!
    //
    IMG_UINT32 i;
    irqreturn_t ret = IRQ_NONE;
    struct timespec t0, t1;
    struct HW_CI_IRQ_STATUS status;
#ifdef FELIX_FAKE
    // so that pdump script has enough time between frames
    IMG_UINT32 pollTime = 0;
    IMG_UINT32 pollTries = 2;
#endif /* FELIX_FAKE */

    IMG_MEMSET(&status, 0, sizeof(struct HW_CI_IRQ_STATUS));
    getnstimeofday(&t0);

#ifdef FELIX_FAKE
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "start");
#else

    HW_CI_ReadTimestamps(&status.hardTimestamps);
    CI_DEBUG("Hard interrupt %d at %u\n", irq, status.hardTimestamps);
#endif /* FELIX_FAKE */

#if defined(CI_CLEAR_GASKET_FIFO)
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NImagers; i++)
    {
        IMG_UINT32 reg;

        if (g_psCIDriver->sHWInfo.gasket_aType[i] & CI_GASKET_MIPI)
        {
            TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
                FELIX_GAS_MIPI_GASKET_MIPI_STATUS2_OFFSET, &reg);

#if defined(VERBOSE_DEBUG)
            CI_DEBUG("reset MIPI FIFO flag for gasket %d "\
                "(current register value 0x%08x)",
                i, reg);
#endif

            TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
                FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET, &reg);
            REGIO_WRITE_FIELD(reg, FELIX_GAS_MIPI, GASKET_CONTROL,
                GASKET_CLEAR_FIFO_FLAG, 1);
            TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
                FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET, reg);
        }
    }
#endif /* CI_CLEAR_GASKET_FIFO */

    //
    // checking which context generated the interrupt
    //
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        IMG_UINT32 thisCTXVal, thisCtxEna, thisCtxLast;
        IMG_HANDLE contextHandle =
            g_psCIDriver->sTalHandles.hRegFelixContext[i];

        thisCTXVal = 0;
        TALREG_ReadWord32(contextHandle,
            FELIX_CONTEXT0_INTERRUPT_STATUS_OFFSET, &thisCTXVal);

        thisCtxEna = 0;
        TALREG_ReadWord32(contextHandle,
            FELIX_CONTEXT0_INTERRUPT_ENABLE_OFFSET, &thisCtxEna);

        TALREG_ReadWord32(contextHandle,
            FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET, &thisCtxLast);

        /* if thisCtxEna is 0 then the context has been disabled we are in
        * the stopping process */
        if (thisCtxEna & thisCTXVal)
        {
#if defined(VERBOSE_DEBUG)
            CI_DEBUG("--- CI context %d interrupt 0x%x ---\n", i, thisCTXVal);
#endif
            status.ui32CtxStatus[i] = thisCTXVal;
            status.ui32CtxLastFrameInfo[i] = thisCtxLast;
            ret = IRQ_WAKE_THREAD;
#ifdef FELIX_FAKE
            /* clear interrupts
            * but not frame done because it will be cleared after POLL for HW to
            * sync when replaying pdumps */
            TALREG_WriteWord32(contextHandle,
                FELIX_CONTEXT0_INTERRUPT_CLEAR_OFFSET,
                thisCTXVal&(~INT_ST_MASK(INT_FRAME_DONE_ALL)));
#else
            // clear interrupts
            TALREG_WriteWord32(contextHandle,
                FELIX_CONTEXT0_INTERRUPT_CLEAR_OFFSET, thisCTXVal);
#endif /* FELIX_FAKE */
        }
        else if (thisCTXVal)  // && !thisCtxEna
        {
            CI_WARNING("trying to service interrupt while interrupts "\
                "are disabled\n");
        }

#ifdef FELIX_UNIT_TESTS
        // reset the interrupt value manually
        TALREG_WriteWord32(contextHandle,
            FELIX_CONTEXT0_INTERRUPT_CLEAR_OFFSET, 0);
#endif
    }

    //
    // checking internal data generator interrupts only for HW version > 1.x
    //
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator; i++)
    {
        IMG_UINT32 thisDGVal;
        IMG_UINT32 thisDGEnbl;
        IMG_HANDLE dgHandle = g_psCIDriver->sTalHandles.hRegIIFDataGen[i];

        thisDGVal = 0;
        TALREG_ReadWord32(dgHandle,
            FELIX_DG_IIF_DG_INTER_STATUS_OFFSET, &thisDGVal);
        thisDGEnbl = 0;
        TALREG_ReadWord32(dgHandle,
            FELIX_DG_IIF_DG_INTER_ENABLE_OFFSET, &thisDGEnbl);

#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- IIF DG %d interrupt 0x%x ---\n", i, thisDGVal);
#endif

        status.ui32IntDGFrameCount[i] = HW_CI_DatagenFrameCount(i);
        status.ui32IntDGStatus[i] = thisDGVal;

        if (thisDGVal & thisDGEnbl)
        {
            ret = IRQ_WAKE_THREAD;

#ifdef FELIX_FAKE
            /* clear interrupts
            * but not frame done because it will be cleared after POLL for HW
            * to sync when replaying pdumps */
            TALREG_WriteWord32(dgHandle, FELIX_DG_IIF_DG_INTER_CLEAR_OFFSET,
                thisDGVal&(~INT_DG_MASK(DG_INT_END_OF_FRAME)));
#else
            // clear interrupts
            TALREG_WriteWord32(dgHandle, FELIX_DG_IIF_DG_INTER_CLEAR_OFFSET,
                thisDGVal);
#endif /* FELIX_FAKE */
        }

#ifdef FELIX_UNIT_TESTS
        // reset the interrupt value manually
        TALREG_WriteWord32(dgHandle, FELIX_DG_IIF_DG_INTER_CLEAR_OFFSET, 0);
#endif
    }

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NImagers; i++)
    {
#ifndef NDEBUG
        // done in debug only because gasket interrupts are not enabled!
        IMG_UINT32 statusR, maskR;

#if FELIX_GAS_PARA_GASKET_INTER_ENABLE_OFFSET != FELIX_GAS_MIPI_GASKET_INTER_ENABLE_OFFSET \
    || FELIX_GAS_PARA_GASKET_INTER_STATUS_OFFSET != FELIX_GAS_MIPI_GASKET_INTER_STATUS_OFFSET \
    || FELIX_GAS_PARA_GASKET_INTER_CLEAR_OFFSET != FELIX_GAS_MIPI_GASKET_INTER_CLEAR_OFFSET
#error gasket interrupt registers moved
#endif

        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
            FELIX_GAS_MIPI_GASKET_INTER_STATUS_OFFSET, &statusR);
        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
            FELIX_GAS_MIPI_GASKET_INTER_ENABLE_OFFSET, &maskR);

        // if we enable interrupts in gasket we have to handle them here

#if defined(VERBOSE_DEBUG)
        CI_DEBUG("Gasket %d interrupt 0x%x & 0x%x\n", i, statusR, maskR);
#endif
        if (statusR & maskR)
        {
            TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[i],
                FELIX_GAS_MIPI_GASKET_INTER_CLEAR_OFFSET, statusR);
        }
#endif /* NDEBUG */

        //  [GM] - for HW_3 provide MIPI virtual channel number
        status.ui32GasketFrameCount[i] = HW_CI_GasketFrameCount(i, 0);
    }

    if (g_psCIDriver->sMMU.apDirectory[0])
    {
#if (FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6)
        static const int int_src = FELIX_CORE_FELIX_INTERRUPT_STATUS_OFFSET;
        static const int int_mask =
            FELIX_CORE_FELIX_INTERRUPT_STATUS_MMU_INTERRUPT_MASK;
#else
        static const int int_src = FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET;
        static const int int_mask =
            FELIX_CORE_FELIX_INTERRUPT_SOURCE_MMU_INTERRUPT_MASK;
#endif
        /* read interrupt source to know if MMU has interrupt because
        * MMU_STATUS0 can legetimatly be 0 */
        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
            int_src, &(status.coreIntSource));

        if ((status.coreIntSource&int_mask) != 0)
        {
            /* no POLL on the int_src register because CSIM does not support
            * page fault without dying so pdumps cannot be generated */
            TALREG_ReadWord32(g_psCIDriver->sMMU.hRegFelixMMU,
                IMG_VIDEO_BUS4_MMU_MMU_STATUS0_OFFSET,
                &(status.mmuStatus0));
            TALREG_ReadWord32(g_psCIDriver->sMMU.hRegFelixMMU,
                IMG_VIDEO_BUS4_MMU_MMU_STATUS1_OFFSET,
                &(status.mmuStatus1));
            TALREG_ReadWord32(g_psCIDriver->sMMU.hRegFelixMMU,
                IMG_VIDEO_BUS4_MMU_MMU_ADDRESS_CONTROL_OFFSET,
                &(status.mmuControl));

            ret = IRQ_WAKE_THREAD;
            CI_WARNING("interrupt from MMU!\n");

            // clear the interrupt
            i = 0;
            REGIO_WRITE_FIELD(i, IMG_VIDEO_BUS4_MMU,
                MMU_CONTROL1, MMU_FAULT_CLEAR, 1);
            TALREG_WriteWord32(g_psCIDriver->sMMU.hRegFelixMMU,
                IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, i);
#if (FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6)
            i = 0;
            REGIO_WRITE_FIELD(i, FELIX_CORE,
                FELIX_INTERRUPT_CLEAR, MMU_INTERRUPT_CLEAR, 1);
            TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
                FELIX_CORE_FELIX_INTERRUPT_CLEAR_OFFSET, i);
#endif
        }
    }

#if defined(IMG_SCB_DRIVER) && !defined(NDEBUG) && defined(VERBOSE_DEBUG)
    // debug: read SCB register
    {
        uintptr_t scb_off = 0;
        int mask = 0;

#if FELIX_VERSION_MAJ == 1
        scb_off += 0x6000;
#elif FELIX_VERSION_MAJ == 2
        if (g_psCIDriver->sHWInfo.rev_ui8Minor >= 3)
        {
            scb_off += 0x15000;
        }
        else
        {
            scb_off += 0xB000;
        }
#elif FELIX_VERSION_MAJ == 3
        scb_off += 0x15000;
#else
#error unknown SCB offset
#endif

        mask = ioread32(g_psCIDriver->pDevice->uiRegisterCPUVirtual
            + scb_off + 0x48);
        i = ioread32(g_psCIDriver->pDevice->uiRegisterCPUVirtual
            + scb_off + 0x40);
        CI_WARNING("SCB interrupt (0x%x) 0x%x & 0x%x\n", scb_off, i, mask);
    }
#endif /* IMG_SCB_DRIVER */

#ifdef FELIX_HAS_DG
    status.dgHardHandle = KRN_DG_DriverHardHandleInterrupt(irq, dev_id);
    if (IRQ_WAKE_THREAD == status.dgHardHandle)
    {
        // in case it was only the DG!
        ret = IRQ_WAKE_THREAD;
    }
#endif

    if (IRQ_WAKE_THREAD == ret)
    {
        struct HW_CI_IRQ_STATUS *pStatus = NULL;

#ifdef FELIX_FAKE
        pStatus = IMG_CALLOC(1, sizeof(struct HW_CI_IRQ_STATUS));
#else
        pStatus = kcalloc(1, sizeof(struct HW_CI_IRQ_STATUS), GFP_ATOMIC);
#endif /* FELIX_FAKE */
        if (!pStatus)
        {
            CI_FATAL("failed to allocate atomic message for bottom half!\n");
            ret = IRQ_NONE;
        }
        else
        {
            IMG_MEMCPY(pStatus, &status, sizeof(struct HW_CI_IRQ_STATUS));
            pStatus->sCell.object = pStatus;

            SYS_SpinlockAcquire(&(g_psCIDriver->sWorkSpinlock));
            {
                List_pushBack(&(g_psCIDriver->sWorkqueue), &(pStatus->sCell));
            }
            SYS_SpinlockRelease(&(g_psCIDriver->sWorkSpinlock));
        }
    }

    SYS_DevClearInterrupts(g_psCIDriver->pDevice);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "done");

    getnstimeofday(&t1);

#if defined(W_TIME_PRINT)
    W_TIME_PRINT(t0, t1, "=");
#endif
#if defined(CI_DEBUGFS)
    {
        IMG_INT64 diff =
            (IMG_INT64)((t1.tv_nsec - t0.tv_nsec) / 1000 % 1000000);
        //IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
        g_ui32NServicedHardInt++;

        if (diff > g_i64LongestHardIntUS)
        {
            g_i64LongestHardIntUS = diff;
        }
    }
#endif /* CI_DEBUGFS */

    return ret;
}

/*
* made as a function because FELIX_FAKE makes it quite big...
*/
static irqreturn_t IMG_HandleCtxIRQ(const struct HW_CI_IRQ_STATUS *pStatus,
    IMG_UINT32 i, const IMG_INT8 *i8IntDgReplacesGasket)
{
    IMG_UINT32 thisCTXVal;
    irqreturn_t ret = IRQ_NONE;
#ifdef FELIX_FAKE
    // so that pdump script has enough time between frames
    IMG_UINT32 pollTime = 0;
    IMG_UINT32 pollTries = 2;
#endif

    thisCTXVal = pStatus->ui32CtxStatus[i];
#if defined(VERBOSE_DEBUG)
    CI_DEBUG("--- CI context %d interrupt 0x%x ---\n", i, thisCTXVal);
#endif

#ifdef CI_DEBUGFS
    g_ui32NCTXDoneInt[i] +=
        (thisCTXVal & INT_ST_MASK(INT_FRAME_DONE_ALL)) ? 1 : 0;
    g_ui32NCTXIgnoreInt[i] +=
        (thisCTXVal & INT_ST_MASK(INT_ERROR_IGNORE)) ? 1 : 0;
    g_ui32NCTXStartInt[i] +=
        (thisCTXVal & INT_ST_MASK(INT_START_OF_FRAME_RECEIVED)) ? 1 : 0;
    g_ui32NCTXInt[i] +=
        (thisCTXVal & INT_ST_MASK(INT_FRAME_DONE_ALL)) ? 1 : 0;
#endif

#ifndef INFOTM_DISABLE_INT_START_OF_FRAME_RECEIVED
    if ((thisCTXVal & INT_ST_MASK(INT_START_OF_FRAME_RECEIVED)) != 0)
    {
        IMG_UINT32 gasketCount = 0, gasketFrames = 0;
        IMG_UINT32 gasketCountMax = 0;
        KRN_CI_PIPELINE *pCapture = NULL;

        IMG_UINT8 ui8Gasket = g_psCIDriver->aActiveCapture[i]->\
            iifConfig.ui8Imager;

        if (i8IntDgReplacesGasket[ui8Gasket] < 0)
        {
            gasketCount = pStatus->ui32GasketFrameCount[ui8Gasket];
            gasketCountMax =
                FELIX_GAS_MIPI_GASKET_FRAME_CNT_GASKET_FRAME_CNT_LSBMASK;
            /* only works if frame count are same size for mipi and
            * parallel (static assert above) */
        }
        else
        {
            gasketCount = pStatus->ui32IntDGFrameCount[
                i8IntDgReplacesGasket[ui8Gasket]];
            gasketCountMax = FELIX_DG_IIF_DG_STATUS_DG_FRAMES_SENT_LSBMASK;
        }

        ret = IRQ_HANDLED;

#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- lock activeContextLock\n");
#endif
        SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
        {
            pCapture = g_psCIDriver->aActiveCapture[i];

            if (gasketCount < g_psCIDriver->aLastGasketCount[i])
            {
                /* counter rolled over! - it is unlikely that it does
                * it twice in a row
                * counter is 32b (that means a lot of missed frames...) */
                gasketFrames = (gasketCountMax
                    - g_psCIDriver->aLastGasketCount[i] + 1)
                    + gasketCount;
            }
            else
            {
                gasketFrames = gasketCount
                    - g_psCIDriver->aLastGasketCount[i];
#if defined(VERBOSE_DEBUG)
                CI_DEBUG("ctx %d gasketFrames=%u (gasketCount=%u, "\
                    "lastCount=%u)\n",
                    i, gasketFrames, gasketCount,
                    g_psCIDriver->aLastGasketCount[i]);
#endif
            }

            g_psCIDriver->aLastGasketCount[i] = gasketCount;
        }
        SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- unlock activeContextLock\n");
#endif

        if (pCapture)
        {
            KRN_CI_SHOT *pShot = NULL;
            SYS_LockAcquire(&(pCapture->sListLock));
            {
                sCell_T *pHead = List_getHead(&(pCapture->sList_pending));
                if (pHead)
                {
                    pShot = container_of(pHead, KRN_CI_SHOT, sListCell);
                }
            }
            SYS_LockRelease(&(pCapture->sListLock));
            if (pShot)
            {
                if (gasketFrames < 1)
                {
                    CI_WARNING("Gasket frame counter was not "\
                        "incremented! Legitimate for first frame.\n");
                    pShot->userShot.i32MissedFrames = 0;
                }
                else
                {
                    pShot->userShot.i32MissedFrames = gasketFrames - 1;
                }
            }
        }  //end of if (pCapture)
    } //end of if ((thisCTXVal & INT_ST_MASK(INT_START_OF_FRAME_RECEIVED)) != 0)
#endif //INFOTM_DISABLE_INT_START_OF_FRAME_RECEIVED

    if ((thisCTXVal & INT_ST_MASK(INT_FRAME_DONE_ALL)) != 0)
    {
        KRN_CI_PIPELINE *pCapture = NULL;
        KRN_CI_SHOT *pShot = NULL;
        IMG_UINT8 currentId = 0, popedId = 0;
        int safe = 0;

        //IMG_INT32 i32MissedFrameCounter = 0;
        ret = IRQ_HANDLED;

#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- lock activeContextLock\n");
#endif
        SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
        {
            pCapture = g_psCIDriver->aActiveCapture[i];
            //i32MissedFrameCounter = g_psCIDriver->aMissedFrames[i];
            //g_psCIDriver->aMissedFrames[i] = 0;
        }
        SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- unlock activeContextLock\n");
#endif
        /* interrupts should be turned on after aActiveCapture is
        * populated and turned off before it is set to NULL */
        IMG_ASSERT(pCapture != NULL);

#ifdef FELIX_FAKE
        {
            IMG_HANDLE contextHandle =
                g_psCIDriver->sTalHandles.hRegFelixContext[i];

                // 2 clock per pixel
                // +40 H because DG always has 40 columns of blanking
                // wait per number of contexts in case of line interleaving
                pollTime = 2 * (pCapture->iifConfig.\
                    ui16ImagerSize[0] * 2 + 1 + 40)
                    *(pCapture->iifConfig.\
                    ui16ImagerSize[1] * 2 + 1 + 15)
                    * g_psCIDriver->sHWInfo.config_ui8NContexts;

            // HW testbench has a maximum
            if (pollTime / pollTries >= CI_REGPOLL_MAXTIME)
            {
                pollTries = pollTime / CI_REGPOLL_MAXTIME + 1;
            }

            /* needed because WriteDevMemRef is pdumped in device memory
            * file when running multiple pdump context */
            TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);

            TALPDUMP_Comment(contextHandle, ""); // space to help HW debug
            CI_PDUMP_COMMENT(contextHandle, "poll when choosing which "\
                "CTX got an interrupt");
            TALPDUMP_Comment(contextHandle, "");

            /* polltime/2 but number of tries*2 to have smaller waiting
            * time masking with only the DONE flags to avoid to catch
            * next frame's FS flag */
            TALREG_Poll32(contextHandle,
                FELIX_CONTEXT0_INTERRUPT_STATUS_OFFSET,
                TAL_CHECKFUNC_ISEQUAL, thisCTXVal,
                INT_ST_MASK(INT_FRAME_DONE_ALL),
                CI_REGPOLL_TRIES*pollTries, pollTime / pollTries);
            CI_PDUMP_COMMENT(contextHandle,
                "ensure INT_ERROR_IGNORE is not up")
                /* poll asked by HW to ensure tests are passing when using
                * latency pdump testing */
                TALREG_Poll32(contextHandle,
                FELIX_CONTEXT0_INTERRUPT_STATUS_OFFSET,
                TAL_CHECKFUNC_ISEQUAL, thisCTXVal,
                INT_ST_MASK(INT_ERROR_IGNORE),
                CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
            IMG_ASSERT((thisCTXVal&INT_ST_MASK(INT_ERROR_IGNORE)) == 0);

            /// @ enable when CSIM supports it
            /*TALREG_Poll32(g_pDGDriver->hRegTestIO,
            FELIX_TEST_IO_FELIX_IRQ_STATUS_OFFSET,
            TAL_CHECKFUNC_ISEQUAL, 1, 0x1,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);*/
#if defined(FELIX_HAS_DG) && !defined(FELIX_UNIT_TESTS)
            /* when running unit tests we don't need to have this extract
            * poll that the HW wants to validate the interrupt is up
            * because we use the NULL interface which does not have
            * any actual register behaviours
            * plus g_pDGDriver may be NULL in unit tests */
            IMG_ASSERT(g_pDGDriver);
            IMG_ASSERT(g_pDGDriver->hRegTestIO);
            CI_PDUMP_COMMENT(g_pDGDriver->hRegTestIO,
                "verify that interrupt line is up");
            TALREG_Poll32(g_pDGDriver->hRegTestIO,
                FELIX_TEST_IO_FELIX_IRQ_STATUS_OFFSET,
                TAL_CHECKFUNC_ISEQUAL,
                FELIX_TEST_IO_FELIX_IRQ_STATUS_FELIX_IRQ_MASK,
                FELIX_TEST_IO_FELIX_IRQ_STATUS_FELIX_IRQ_MASK,
                CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif
            // clear the interrupt in case the image is small in the test
            TALREG_WriteWord32(contextHandle,
                FELIX_CONTEXT0_INTERRUPT_CLEAR_OFFSET,
                thisCTXVal&INT_ST_MASK(INT_FRAME_DONE_ALL));
        }
#endif /* FELIX_FAKE */

        currentId = REGIO_READ_FIELD(pStatus->ui32CtxLastFrameInfo[i],
            FELIX_CONTEXT0, LAST_FRAME_INFO, LAST_CONTEXT_TAG);
#if defined(VERBOSE_DEBUG)
        CI_DEBUG("Current Frame ID %u\n", currentId);

        CI_DEBUG("--- lock processingSpinlock (ctx)\n");
#endif
        SYS_LockAcquire(&(pCapture->sListLock));
        /* the interrupt contains at least 1 element
        * cannot have more than what was in the pending queue handled
        * at once
        * safe=0, pShot=NULL */
        do
        {
            pShot = KRN_CI_PipelineShotCompleted(pCapture);
            /* it should never be NULL if the interrupt was legit!
            * but cannot die in the lock */
            if (pShot)
            {
                popedId = pShot->iIdentifier;
                //pShot->userShot.i32MissedFrames written at frame start
#if defined(VERBOSE_DEBUG)
                CI_DEBUG("poping out shot %u\n", popedId);
#endif
                safe++;
            }
            else
            {
                // can happen when stopping the context
                CI_WARNING("no frame found in the processing list for "\
                    "context %d (int 0x%x, last frame %d)\n",
                    i, thisCTXVal, currentId);
                break;
            }
        } while (currentId != popedId
            && safe < g_psCIDriver->sHWInfo.context_aPendingQueue[i]);
        SYS_LockRelease(&(pCapture->sListLock));
#if defined(VERBOSE_DEBUG)
        CI_DEBUG("--- unlock processingSpinlock (ctx)\n");
#endif
        CI_DEBUG("handled %d interrupts for ctx %d\n", safe, i);
    } // if context had interrupt
    return ret;
}

static irqreturn_t IMG_HandleIntDGIRQ(const struct HW_CI_IRQ_STATUS *pStatus,
    IMG_UINT32 i)
{
    irqreturn_t ret = IRQ_NONE;
    IMG_UINT32 thisDGVal = pStatus->ui32IntDGStatus[i];
    KRN_CI_DATAGEN *pDatagen = NULL;
#ifdef FELIX_FAKE
    // so that pdump script has enough time between frames
    IMG_UINT32 pollTime = 0;
    IMG_UINT32 pollTries = 2;
#endif

#ifdef CI_DEBUGFS
    g_ui32NIntDGErrorInt[i] +=
        (thisDGVal & INT_DG_MASK(DG_INT_ERROR)) ? 1 : 0;
    g_ui32NIntDGFrameEndInt[i] +=
        (thisDGVal & INT_DG_MASK(DG_INT_END_OF_FRAME)) ? 1 : 0;
#endif

#if defined(VERBOSE_DEBUG)
    CI_DEBUG("--- IIF DG %d interrupt 0x%x ---\n", i, thisDGVal);
#endif
    SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
    {
        pDatagen = g_psCIDriver->pActiveDatagen;
    }
    SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));

    if (0 != (thisDGVal&INT_DG_MASK(DG_INT_ERROR)))
    {
        IMG_ASSERT(pDatagen != NULL);

        CI_FATAL("IntDG signals that the frame could not be read - "\
            "check bandwidth limitations or memory latency - IntDG "\
            "needs to be restarted\n");
        pDatagen->bNeedsReset = IMG_TRUE;
    }

    if (0 != (thisDGVal&INT_DG_MASK(DG_INT_END_OF_FRAME)))
    {
        IMG_ASSERT(pDatagen);
        // to protect the sList_busy and sList_processed
        SYS_LockAcquire(&(pDatagen->slistLock));
        {
            sCell_T *pFound = NULL;

#ifdef FELIX_FAKE
            /* to compute the polling time for pdump we need to know the
            * size of the frame */
            IMG_HANDLE dgHandle =
                g_psCIDriver->sTalHandles.hRegIIFDataGen[i];
            const int mask = INT_DG_MASK(DG_INT_END_OF_FRAME);
            // get the current head to have the size
            pFound = List_getHead(&(pDatagen->sList_busy));

            pollTries = 2;
            if (pFound)
            {
                KRN_CI_DGBUFFER *pDGBuffer =
                    container_of(pFound, KRN_CI_DGBUFFER, sCell);

                // 2 clocks per pixels
                // +40 Horiz blanking always added to register
                pollTime = 2 * (pDGBuffer->ui16Width + pDGBuffer->\
                    ui16HorizontalBlanking + 40)
                    *(pDGBuffer->ui16Height
                    + pDGBuffer->ui16VerticalBlanking + 15);
            }
            else
            {
                CI_FATAL("IntDG interrut without frame!\n");
                pollTime = CI_REGPOLL_TIMEOUT;
            }

            // HW testbench has a maximum
            if (CI_REGPOLL_MAXTIME <= pollTime / pollTries)
            {
                pollTries = pollTime / CI_REGPOLL_MAXTIME + 1;
            }

            TALPDUMP_Comment(dgHandle, "");
            CI_PDUMP_COMMENT(dgHandle, "poll when choosing which IIF "\
                "DG got an interrupt");
            TALPDUMP_Comment(dgHandle, "");

            TALREG_Poll32(dgHandle,
                FELIX_DG_IIF_DG_INTER_STATUS_OFFSET,
                TAL_CHECKFUNC_ISEQUAL, thisDGVal, mask,
                CI_REGPOLL_TRIES*pollTries, pollTime / pollTries);

            /* needed because WriteDevMemRef is pdumped in device
            * memory file when running multiple pdump context */
            TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);

            /* clear interrupt because submitting next frame on dg may
            * be troublesome when having small images and running pdump */
            TALREG_WriteWord32(dgHandle,
                FELIX_DG_IIF_DG_INTER_CLEAR_OFFSET,
                thisDGVal&INT_DG_MASK(DG_INT_END_OF_FRAME));
#endif /* FELIX_FAKE */

            // detach from sList_busy
            pFound = List_popFront(&(pDatagen->sList_busy));
            if (pFound)
            {
                List_pushBack(&(pDatagen->sList_processed), pFound);
                SYS_SemIncrement(&(pDatagen->sSemProcessed));

                // submit the next head
                pFound = List_getHead(&(pDatagen->sList_busy));
            }
            else
            {
                CI_FATAL("got an interrupt but has no frame in "\
                    "busy list!\n");
            }

            if (pFound && pDatagen->bNeedsReset == IMG_FALSE)
            {
                KRN_CI_DGBUFFER *pBuff =
                    container_of(pFound, KRN_CI_DGBUFFER, sCell);
                HW_CI_DatagenTrigger(pBuff);
            }
        }
        SYS_LockRelease(&(pDatagen->slistLock));
    }
    return ret;
}

static irqreturn_t IMG_HandleMMUIRQ(const struct HW_CI_IRQ_STATUS *pStatus)
{
#if (FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6)
    static const int int_mask =
        FELIX_CORE_FELIX_INTERRUPT_STATUS_MMU_INTERRUPT_MASK;
#else
    static const int int_mask =
        FELIX_CORE_FELIX_INTERRUPT_SOURCE_MMU_INTERRUPT_MASK;
#endif
    irqreturn_t ret = IRQ_NONE;

    if ((pStatus->coreIntSource&int_mask) != 0)
    {
        IMG_UINT32 dirIndex = 0;

        CI_FATAL("--- MMU fault status0=0x%x status1=0x%x "\
            "addr_ctrl=0x%x (page fault %d, read fault %d, write "\
            "fault %d, ext addressing %d) ---\n",
            pStatus->mmuStatus0, pStatus->mmuStatus1, pStatus->mmuControl,
            // dirIndex used to know if it is a page fault
            REGIO_READ_FIELD(pStatus->mmuStatus0, IMG_VIDEO_BUS4_MMU,
            MMU_STATUS0, MMU_PF_N_RW),
            REGIO_READ_FIELD(pStatus->mmuStatus1, IMG_VIDEO_BUS4_MMU,
            MMU_STATUS1, MMU_FAULT_RNW),
            REGIO_READ_FIELD(pStatus->mmuStatus1, IMG_VIDEO_BUS4_MMU,
            MMU_STATUS1, MMU_FAULT_RNW) == 0,
            REGIO_READ_FIELD(pStatus->mmuControl, IMG_VIDEO_BUS4_MMU,
            MMU_ADDRESS_CONTROL, MMU_ENABLE_EXT_ADDRESSING));

#ifdef FELIX_FAKE
        TALREG_Poll32(g_psCIDriver->sMMU.hRegFelixMMU,
            IMG_VIDEO_BUS4_MMU_MMU_STATUS0_OFFSET,
            TAL_CHECKFUNC_ISEQUAL, pStatus->mmuStatus0, ~0,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif

        dirIndex = REGIO_READ_FIELD(pStatus->mmuStatus1,
            IMG_VIDEO_BUS4_MMU, MMU_STATUS1, MMU_FAULT_INDEX);

        if (dirIndex < CI_N_CONTEXT)
        {
            IMG_UINT32 reg;

            reg = pStatus->mmuStatus0;
            // status without bottom bit is the virtual address failed at
            REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU,
                MMU_STATUS0, MMU_PF_N_RW, 0);
            CI_FATAL("MMU fault @ v0x%x on directory %u - direct "\
                "entry = 0x%x - page entry = 0x%x\n",
                reg, dirIndex,
                IMGMMU_DirectoryGetDirectoryEntry(
                g_psCIDriver->sMMU.apDirectory[dirIndex],
                reg & ~0x1),
                IMGMMU_DirectoryGetPageTableEntry(
                g_psCIDriver->sMMU.apDirectory[dirIndex],
                reg & ~0x1)
                );

            if (g_psCIDriver->aActiveCapture[dirIndex] != NULL)
            {
                /* stop the HW so that the interrupt does not reoccur
                * - user space should time out waiting for frame and
                * decide what to do */
                HW_CI_PipelineRegStop(
                    g_psCIDriver->aActiveCapture[dirIndex],
                    IMG_FALSE);
            }

#if (FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN >= 6)
            /* turn off MMU interrupt because we get too many, when starting
            * again later the interrupt should be turned back on */
            reg = 0;
            TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
                FELIX_CORE_FELIX_INTERRUPT_ENABLE_OFFSET, &reg);
            REGIO_WRITE_FIELD(reg, FELIX_CORE, FELIX_INTERRUPT_ENABLE,
                MMU_INTERRUPT_EN, 0);
            TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
                FELIX_CORE_FELIX_INTERRUPT_ENABLE_OFFSET, reg);
#endif
        }
        else
        {
            CI_FATAL("MMU fault @ v0x%x but MMU dir index %u is too big "\
                "for HW\n", pStatus->mmuStatus0 & ~0x1, dirIndex);
            // could add a debugFS counter but should never occur
        }

        ret = IRQ_HANDLED;
    }

    return ret;
}

// if fails change merging of irq return
IMG_STATIC_ASSERT(IRQ_NONE == 0, IRQ_NONE_IS_ZERO);
// if fails change merging of irq return
IMG_STATIC_ASSERT(IRQ_HANDLED != 0, IRQ_HANDLE_NOT_ZERO);

//
// DO NOT CALL DIRECTLY! REGISTER IT TO THE INTERRUPT HANDLING MECHANISM
//
irqreturn_t HW_CI_DriverThreadHandleInterrupt(int irq, void *dev_id)
{
    //
    // THIS IS IN THREADED CONTEXT
    //
    IMG_UINT32 i;
    irqreturn_t ret = IRQ_NONE;
    struct HW_CI_IRQ_STATUS *pStatus = NULL;
    struct timespec t0, t1;

    // internal dg replaces a gasket cannot happen without an internal DG
    // assumes it does not change even if we get multiple iterations
    // of the handling loop
    IMG_INT8 i8IntDgReplacesGasket[CI_N_IMAGERS];

#ifdef FELIX_FAKE
    IMG_ASSERT(g_psCIDriver);

    getnstimeofday(&t0);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "start");
    i = 0;  // to fake timestamps
#else

    if (!g_psCIDriver)
    {
        // unexpected! interrupt handler should be removed before destruction
        CI_FATAL("g_psCIDriver is NULL!\n");
        return IRQ_NONE;
    }

    getnstimeofday(&t0);
    HW_CI_ReadTimestamps(&i);

    CI_DEBUG("Threaded Interrupt %d at %u\n", irq, i);
#endif /* FELIX_FAKE */

    for (i = 0; i < CI_N_IMAGERS; i++)
    {
        i8IntDgReplacesGasket[i] = -1;  // means no Int DG replacing gasket
    }

    /*for ( i = 0 ;
    i < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator ; i++ )*/
    {
        SYS_LockAcquire(&(g_psCIDriver->sActiveContextLock));
        {
            if (g_psCIDriver->pActiveDatagen != NULL)
            {
                i8IntDgReplacesGasket\
                    [g_psCIDriver->pActiveDatagen->userDG.ui8Gasket] =
                    g_psCIDriver->pActiveDatagen->userDG.ui8IIFDGIndex;
            }
        }
        SYS_LockRelease(&(g_psCIDriver->sActiveContextLock));
    }

    SYS_SpinlockAcquire(&(g_psCIDriver->sWorkSpinlock));
    {
        sCell_T *pCell = List_popFront(&(g_psCIDriver->sWorkqueue));
        if (pCell)
        {
            pStatus = container_of(pCell, struct HW_CI_IRQ_STATUS, sCell);
        }
    }
    SYS_SpinlockRelease(&(g_psCIDriver->sWorkSpinlock));

    while (NULL != pStatus)
    {
        //
        // check which context generated the interrupt
        //
        for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
        {
            ret |= IMG_HandleCtxIRQ(pStatus, i, i8IntDgReplacesGasket);
        } // for each context

        //
        // checking internal data generator interrupts
        //
        for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator; i++)
        {
            ret |= IMG_HandleIntDGIRQ(pStatus, i);
        }

        if (g_psCIDriver->sMMU.apDirectory[0])
        {
            ret |= IMG_HandleMMUIRQ(pStatus);
        } /* for mmu */

        // if the HW does not have DG then interrupt should not be generated
#ifdef FELIX_HAS_DG
        if (IRQ_WAKE_THREAD == pStatus->dgHardHandle)
        {
            ret |= KRN_DG_DriverThreadHandleInterrupt(irq, dev_id);
        }
#endif

#if defined(FELIX_FAKE)
        IMG_FREE(pStatus);
#else
        kfree(pStatus);
#endif
        pStatus = NULL;

        SYS_SpinlockAcquire(&(g_psCIDriver->sWorkSpinlock));
        {
            sCell_T *pCell = List_popFront(&(g_psCIDriver->sWorkqueue));
            if (pCell)
            {
                pStatus = container_of(pCell, struct HW_CI_IRQ_STATUS, sCell);
            }
        }
        SYS_SpinlockRelease(&(g_psCIDriver->sWorkSpinlock));
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, "done");
    CI_DEBUG("IRQ COMPLETED\n");
    getnstimeofday(&t1);

#if defined(W_TIME_PRINT)
    W_TIME_PRINT(t0, t1, "=");
#endif
#if defined(CI_DEBUGFS)
    {
        IMG_INT64 diff =
            (IMG_INT64)((t1.tv_nsec - t0.tv_nsec) / 1000 % 1000000);
        //IMG_ASSERT(t1.tv_sec - t0.tv_sec < 1);
        g_ui32NServicedThreadInt++;

        if (diff > g_i64LongestThreadIntUS)
        {
            g_i64LongestThreadIntUS = diff;
        }
    }
#endif

    return ret;
}

IMG_RESULT HW_CI_DatagenTrigger(KRN_CI_DGBUFFER *pDGBuffer)
{
    //
    // this is called during interrupt context if pending elements on SW stack
    // this is called from normal context when pushing the 1st frame
    //
    // internal data generator only available for > HW 1.x
    IMG_UINT32 tmpReg;
    IMG_HANDLE hDGTalHandle = NULL;

    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(pDGBuffer != NULL);
    IMG_ASSERT(pDGBuffer->pDatagen != NULL);
    // should have been checked before
    IMG_ASSERT(pDGBuffer->pDatagen->userDG.ui8IIFDGIndex == 0);

    hDGTalHandle = g_psCIDriver->sTalHandles.\
        hRegIIFDataGen[pDGBuffer->pDatagen->userDG.ui8IIFDGIndex];

    // it should not need reset!
    IMG_ASSERT(pDGBuffer->pDatagen->bNeedsReset == IMG_FALSE);

    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_FRAME_BLANKING,
        DG_HORIZONTAL_BLANKING, pDGBuffer->ui16HorizontalBlanking);
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_FRAME_BLANKING,
        DG_VERTICAL_BLANKING, pDGBuffer->ui16VerticalBlanking);
    TALREG_WriteWord32(hDGTalHandle,
        FELIX_DG_IIF_DG_FRAME_BLANKING_OFFSET, tmpReg);

    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_FRAME_SIZE,
        DG_FRAME_WIDTH, pDGBuffer->ui16Width - 1);
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_FRAME_SIZE,
        DG_FRAME_HEIGHT, pDGBuffer->ui16Height - 1);
    TALREG_WriteWord32(hDGTalHandle,
        FELIX_DG_IIF_DG_FRAME_SIZE_OFFSET, tmpReg);

    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_DATA_STRIDE_OFFSET,
        pDGBuffer->ui32Stride);

    SYS_MemWriteAddressToReg(hDGTalHandle, FELIX_DG_IIF_DG_ADDR_DATA_OFFSET,
        &(pDGBuffer->sMemory), 0);

    CI_DEBUG("Trigger IIF DG frame %d\n", pDGBuffer->ID);

    // trigger the HW
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_MODE, DG_MODE,
        FELIX_DG_IIF_DG_MODE_DG_MANUAL);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_MODE_OFFSET, tmpReg);

    return IMG_SUCCESS;
}

KRN_CI_SHOT* KRN_CI_PipelineShotCompleted(KRN_CI_PIPELINE *pPipeline)
{
    //
    // this is done in interrupt context!
    //
    KRN_CI_SHOT *pShot = NULL;
    sCell_T *pShotCell = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 ui32Timestamps = 0;

#ifdef INFOTM_ISP
    struct timespec time;
    IMG_UINT64 ui64SystemTS = 0;
#endif

    IMG_ASSERT(pPipeline);

    if (!pPipeline->bStarted)
    {
        CI_FATAL("capture is not started.\n");
        return NULL;
    }

    HW_CI_ReadTimestamps(&ui32Timestamps);

#ifdef INFOTM_ISP
    get_monotonic_boottime(&time);
    ui64SystemTS = time.tv_sec * 1000ull + time.tv_nsec / 1000000;
#endif

    pShotCell = List_popFront(&(pPipeline->sList_pending));
    if (pShotCell)
    {
        pShot = (KRN_CI_SHOT*)pShotCell->object;

#ifdef INFOTM_HW_AWB_METHOD
    if ( hw_awb_module_scd_status_done_get() == 1 )
    {
        AWB_UINT32* pawb_data;

        pawb_data = (AWB_UINT32*)((IMG_UINT8*)pShot->hwSave.sMemory.pVirtualAddress + (FELIX_SAVE_BYTE_SIZE + 32));
//        printk("addr:0x%p *pawb_data=0x%X\n", pawb_data, *pawb_data);
        *pawb_data = 0x000a;	//done set flag = a;

        hw_awb_module_scd_irq_set(0);
        //hw_awb_module_sc_enable_set(0);
    }
    else
    {
//        printk(" Busy, scd is still busy now !!\n");
    }
#endif //INFOTM_HW_AWB_METHOD

        pShot->eStatus = CI_SHOT_PROCESSED;
        ret = List_pushBack(&(pPipeline->sList_processed), pShotCell);
        IMG_ASSERT(IMG_SUCCESS == ret); // should never fail

        pShot->userShot.ui32InterruptTS = ui32Timestamps;
        CI_DEBUG("interrupt serviced at %u\n", ui32Timestamps);
#ifdef INFOTM_ISP
        pShot->userShot.ui64SystemTS = ui64SystemTS;
        CI_DEBUG("interrupt serviced at %lld\n", ui64SystemTS);
#endif

        // incrementing a semaphore cannot sleep
        SYS_SemIncrement(&(pPipeline->sProcessedSem));
        /* one more element is available in linked list for the HW
        * (it could have been done earlier when receiving frame start) */
        SYS_SemIncrement(
            &(g_psCIDriver->aListQueue[pPipeline->pipelineConfig.ui8Context]));

#ifdef CI_PDP
        if (pShot->phwDisplayOutput
            && TYPE_RGB == pPipeline->pipelineConfig.eDispType.eBuffer)
        {
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
                "writing display output to PDP");
            Platform_MemWritePDPAddress(&(pShot->phwDisplayOutput->sMemory),
                0);
        }
#endif
    }
    else
    {
        CI_WARNING("empty pending list for pipeline - no buffer pushed to "\
            "processed list\n");
    }

    return pShot;
}
