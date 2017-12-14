/**
 ******************************************************************************
 @file dg_camera.c

 @brief kernel-side implementation for the Data Generator

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
#include "ci_kernel/ci_kernel.h" // CI_FATAL

#include <tal.h>
#include <reg_io2.h>

#include <registers/ext_data_generator.h>
//#include <hw_struct/ext_data_generation.h> // dg linked list not used

#include <mmulib/heap.h>

#define HW_DG_OFF(reg) DATA_GENERATION_POINTERS_GROUP_ ## reg ## _OFFSET

#ifdef FELIX_UNIT_TESTS

extern IMG_BOOL8 gbUseTalNULL;

#define FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value) \
    (((IMG_UINT32) (ui32value) << (group##_##reg##_##field##_SHIFT)) & (group##_##reg##_##field##_MASK))

#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value) \
    if ( gbUseTalNULL == IMG_TRUE ) \
TALREG_WriteWord32(TalHandle, group ## _ ## reg ## _OFFSET, FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value));

#else
#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value)
#endif

static IMG_BOOL8 List_FindFrame(void *listelem, void *searched)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listelem;
    int *pID = (int*)searched;

    if (pFrame->identifier == *pID)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_MapFrame(void *listelem, void *directory)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listelem;
    struct MMUDirectory *pDir = (struct MMUDirectory*)directory;

    if (KRN_DG_FrameMap(pFrame, pDir) != IMG_SUCCESS)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_UnmapFrame(void *listelem, void *unused)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listelem;

    (void)unused;
    KRN_DG_FrameUnmap(pFrame);
    return IMG_TRUE;
}

// camera

IMG_RESULT KRN_DG_CameraCreate(KRN_DG_CAMERA **ppCamera)
{
    IMG_RESULT ret;
    IMG_UINT32 i;

    if (ppCamera == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppCamera != NULL)
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppCamera = (KRN_DG_CAMERA*)IMG_CALLOC(1, sizeof(KRN_DG_CAMERA));
    if (*ppCamera == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    ret = SYS_LockInit(&((*ppCamera)->sPendingLock));
    if (ret)
    {
        goto cam_lock_failed;
    }

    ret = SYS_SemInit(&((*ppCamera)->sSemAvailable), 0);
    if (ret)
    {
        goto cam_semlock_failed;
    }

    ret = List_init(&((*ppCamera)->sList_available));
    if (ret)
    {
        goto cam_avail_failed;
    }

    ret = List_init(&((*ppCamera)->sList_converting));
    if (ret)
    {
        goto cam_convert_failed;
    }

    ret = List_init(&((*ppCamera)->sList_pending));
    if (ret)
    {
        goto cam_pending_failed;
    }

    for (i = 0; g_pDGDriver->sMMU.apDirectory[0] != NULL && i < DG_N_HEAPS; i++)
    {
        (*ppCamera)->apHeap[i] = IMGMMU_HeapCreate(
            g_pDGDriver->sMMU.aHeapStart[i], g_pDGDriver->sMMU.uiAllocAtom,
            g_pDGDriver->sMMU.aHeapSize[i], &ret);
        if (ret)
        {
            while (i > 0)
            {
                IMGMMU_HeapDestroy((*ppCamera)->apHeap[i - 1]);
                i--;
            }
            goto cam_pending_failed;
        }
    }

    return IMG_SUCCESS;

cam_pending_failed:
    // converting list is empty
cam_convert_failed :
    // available list is empty
cam_avail_failed :
    SYS_SemDestroy(&((*ppCamera)->sSemAvailable));
cam_semlock_failed:
    SYS_LockDestroy(&((*ppCamera)->sPendingLock));
cam_lock_failed:
    IMG_FREE(*ppCamera);
    *ppCamera = NULL;
    return ret;
}

IMG_RESULT KRN_DG_CameraShoot(KRN_DG_CAMERA *pCamera, int frameId, IMG_BOOL8 bTrigger)
{
    KRN_DG_FRAME *pFrame = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pCamera == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (pCamera->bStarted == IMG_FALSE)
    {
        return IMG_ERROR_NOT_SUPPORTED;
    }

    SYS_LockAcquire(&(pCamera->sPendingLock));
    {
        pFound = List_visitor(&(pCamera->sList_converting),
            (void*)&frameId, &List_FindFrame);

        if (pFound != NULL)
        {
            pFrame = (KRN_DG_FRAME*)pFound->object;
            List_detach(pFound);
        }
    }
    SYS_LockRelease(&(pCamera->sPendingLock));

    if (pFrame == NULL)
    {
        return IMG_ERROR_FATAL;
    }

#ifdef DEBUG_FIRST_LINE
    {
        int *pMem = (int*)Platform_MemGetMemory(&(pFrame->sBuffer));
        IMG_SIZE m = pFrame->sBuffer.uiAllocated/sizeof(int);
        CI_DEBUG("DG image first bytes: 0x%8x 0x%8x 0x%8x 0x%8x\n", pMem[0], pMem[1], pMem[2], pMem[3]);
        CI_DEBUG("DG image last bytes: 0x%8x 0x%8x 0x%8x 0x%8x\n", pMem[m-4], pMem[m-3], pMem[m-2], pMem[m-1]);
}
#endif

    // done in 2 steps because we need to update the memory
    // and this could be "long"
    Platform_MemUpdateDevice(&(pFrame->sBuffer));

    if (bTrigger == IMG_TRUE)
    {
        SYS_LockAcquire(&(pCamera->sPendingLock));
        {
            List_pushBack(&(pCamera->sList_pending), pFound);

            if (pCamera->sList_pending.ui32Elements == 1) // 1st element need to be submitted
            {
                HW_DG_CameraSetNextFrame(pCamera, (KRN_DG_FRAME*)pFound->object);
            }
            else
            {
                CI_WARNING("queue frame %u in DG %u\n", ((KRN_DG_FRAME*)pFound->object)->identifier, pCamera->publicCamera.ui8DGContext);
            }
        }
        SYS_LockRelease(&(pCamera->sPendingLock));
#ifdef CI_DEBUGFS
        g_ui32NDGSubmitted[pCamera->publicCamera.ui8DGContext]++;
#endif
    }
    else
    {
        // if fails go back to available
        SYS_LockAcquire(&(pCamera->sPendingLock));
        {
            List_pushBack(&(pCamera->sList_available), pFound);
            SYS_SemIncrement(&(pCamera->sSemAvailable));
        }
        SYS_LockRelease(&(pCamera->sPendingLock));
    }

    return ret;
}

IMG_RESULT KRN_DG_CameraStart(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
    struct MMUDirectory *pDir = NULL;

    IMG_ASSERT(pCamera->sList_converting.ui32Elements == 0); // there should be no left over frames!
    IMG_ASSERT(pCamera->sList_pending.ui32Elements == 0); // there should be no left over frames!

    if ((ret = KRN_DG_DriverAcquireContext(pCamera)) != IMG_SUCCESS)
    {
        return ret;
    }

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], "start");

    pDir = g_pDGDriver->sMMU.apDirectory[pCamera->publicCamera.ui8DGContext];
    if (pDir != NULL)
    {
        // no interrupt should occure: DG HW is not started yet
        //SYS_SpinlockAcquire(&(pCamera->sPendingSpinLock));
        {
            // should not disable interrupts when mapping - returns NULL if all cells were visited
            if (List_visitor(&(pCamera->sList_available), pDir, &List_MapFrame) != NULL)
            {
                return IMG_ERROR_FATAL; // failed to map
            }
        }
        //SYS_SpinlockRelease(&(pCamera->sPendingSpinLock));

        KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU), pCamera->publicCamera.ui8DGContext, IMG_TRUE);
    }

    if ((ret = HW_DG_CameraConfigure(pCamera)) != IMG_SUCCESS)
    {
        KRN_DG_DriverReleaseContext(pCamera);
    }
    else
    {
        pCamera->bStarted = IMG_TRUE;
    }

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], "done");

    return ret;
}

void HW_DG_WriteGasket(DG_CAMERA *pCamera, IMG_HANDLE registerBank)
{
    IMG_UINT32 tmpReg;

    if (pCamera->eBufferFormats == DG_MEMFMT_BT656_10 || pCamera->eBufferFormats == DG_MEMFMT_BT656_12) // BT656
    {
        // set DG_FRAME_SIZE
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_SIZE, DG_FRAME_HEIGHT, pCamera->uiHeight - 1);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_SIZE, DG_FRAME_WIDTH, pCamera->uiWidth - 1); // this is the number of pixels not the number of bytes (thus not convWidth)
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_FRAME_SIZE_OFFSET, tmpReg);

        //   set DG_BT656_CTRL - values ???
        /** @ values for PARALLEL_V_SYNC_POLARITY and PARALLEL_H_SYNC_POLARITY in structure */
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, PARALLEL_CTRL, PARALLEL_V_SYNC_POLARITY, 1); // active high...
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, PARALLEL_CTRL, PARALLEL_H_SYNC_POLARITY, 1); // active high...
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_PARALLEL_CTRL_OFFSET, tmpReg);
    }
    else // MIPI
    {
        // set DG_FRAME_SIZE
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_SIZE, DG_FRAME_HEIGHT, pCamera->uiHeight - 1);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_SIZE, DG_FRAME_WIDTH, pCamera->uiConvWidth - 1); // MIPI
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_FRAME_SIZE_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_ZERO, pCamera->ui16MipiTHS_zero);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_PREPARE, pCamera->ui16MipiTHS_prepare);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_START_OF_DATA_TRANSMISSION, DG_TLPX, pCamera->ui16MipiTLPX);
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_END_OF_DATA_TRANSMISSION, DG_THS_EXIT, pCamera->ui16MipiTHS_exit);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_END_OF_DATA_TRANSMISSION, DG_THS_TRAIL, pCamera->ui16MipiTHS_trail);
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_MIPI_END_OF_DATA_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_ZERO, pCamera->ui16MipiTCLK_zero);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_PREPARE, pCamera->ui16MipiTCLK_prepare);
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_MIPI_START_OF_CLOCK_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_TRAIL, pCamera->ui16MipiTCLK_trail);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_POST, pCamera->ui16MipiTCLK_post);
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_DG_MIPI_END_OF_CLOCK_TRANSMISSION_OFFSET, tmpReg);
    }
}

IMG_RESULT HW_DG_CameraConfigure(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8DGContext;
    IMG_BOOL8 bIsMIPI = IMG_TRUE;

    // checking for BT656 because there are less possibilities
    if (pCamera->publicCamera.eBufferFormats == DG_MEMFMT_BT656_10 ||
        pCamera->publicCamera.eBufferFormats == DG_MEMFMT_BT656_12)
    {
        bIsMIPI = IMG_FALSE;
    }

    SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
    {
        IMG_HANDLE hDGTalHandle = g_pDGDriver->hRegFelixDG[ui8Context];
        IMG_UINT32 tmpReg = 0;

        CI_PDUMP_COMMENT(hDGTalHandle, "start");

        // reset gasket counter (MIPI_ERROR_CLEAR)
        // reset data gen (DG_RESET)
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_RESET, DG_RESET, 1);
        TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_RESET_OFFSET, tmpReg);
#ifdef FELIX_FAKE
        TAL_Wait(g_psCIDriver->sTalHandles.hRegFelixCore, CI_REGPOLL_TIMEOUT); // harbitrary time given by HW
#endif

        tmpReg = 0;
        TALREG_ReadWord32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET, &tmpReg);
        g_pDGDriver->aFrameSent[ui8Context] = REGIO_READ_FIELD(tmpReg, FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
#ifdef FELIX_FAKE
        TALREG_Poll32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET, TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif

        // wait for DG to be disabled?
        FELIX_FORCEPOLLREG(hDGTalHandle, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 0); // effective only when using unit tests with TAL NULL interface

        if (REGIO_POLL32(hDGTalHandle, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, TAL_CHECKFUNC_ISEQUAL, 0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to poll the control-enable bit\n");
            ret = IMG_ERROR_FATAL;
            goto start_out;
        }

        // setup pixel block (bt656) or byte clock (MIPI)
        //    DG_PIXEL_REF_CLK* DG_PIXEL_CLK_MULT/ DG_PIXEL_CLK_DIV

        // signal frequency is set up: set DG_REF_CLK_SET to 1
        /*tmpReg = 0;
          REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_REF_CLK_SET, DG_REF_CLK_SET, 1);
          TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_REF_CLK_SET_OFFSET, tmpReg);*/

        // poll DG_REF_CLK_SET == 0
        /*FELIX_FORCEPOLLREG(hDGTalHandle, FELIX_TEST_DG, DG_REF_CLK_SET, DG_REF_CLK_SET, 0); // effective only when using unit tests with TAL NULL interface

          if ( REGIO_POLL32(hDGTalHandle, FELIX_TEST_DG, DG_REF_CLK_SET, DG_REF_CLK_SET, TAL_CHECKFUNC_ISEQUAL, 0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT) != IMG_SUCCESS )
          {
          CI_FATAL("Failed to poll the clock reset register\n");
          ret = IMG_ERROR_FATAL;
          goto start_out;
          }*/

        // setup link list is not ready
        //TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_LINK_ADDR_OFFSET, SYSMEM_INVALID_ADDR);

        // set interrupt: end of frame + error
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_INTER_ENABLE, DG_INT_END_OF_FRAME_EN, 1);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_INTER_ENABLE, DG_INT_ERROR_EN, 1);
        TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_INTER_ENABLE_OFFSET, tmpReg);

        // set DG_FRAME_BLANKING
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_BLANKING, DG_VERTICAL_BLANKING, pCamera->publicCamera.ui16VertBlanking);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_FRAME_BLANKING, DG_HORIZONTAL_BLANKING, pCamera->publicCamera.ui16HoriBlanking);
        TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_FRAME_BLANKING_OFFSET, tmpReg);

        HW_DG_WriteGasket(&(pCamera->publicCamera), hDGTalHandle);

        // setup DG_CTRL
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 1);

        if (pCamera->publicCamera.eBufferFormats == DG_MEMFMT_BT656_12)
        {
            REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_PIXEL_FORMAT, 1); // 12b
        }
        else
        {
            REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_PIXEL_FORMAT, 0); // 10b
        }

        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_IF_MODE, bIsMIPI); // MIPI or BT656
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_MIPI_LS_EN, pCamera->publicCamera.bMIPI_LF); // LS flag - if BT656 this should be IMG_FALSE (0)
#if FELIX_VERSION_MAJ > 1
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_GROUP_OVERRIDE, pCamera->publicCamera.ui8DGContext%CI_MMU_N_DIR); // choose the MMU requestor to use
#endif
        TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_CTRL_OFFSET, tmpReg);

        // use the DG as manual: the mode is set when submitting a frame in HW_DG_CameraSetNextFrame

        CI_PDUMP_COMMENT(hDGTalHandle, "done");
    }
start_out:
    SYS_LockRelease(&(g_pDGDriver->sCameraLock));

    return ret;
}

IMG_RESULT KRN_DG_CameraStop(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], "start");

    SYS_LockAcquire(&(pCamera->sPendingLock));
    {
        sCell_T *pWaiting = NULL;
        while ((pWaiting = List_popFront(&(pCamera->sList_converting))) != NULL)
        {
            List_pushBack(&(pCamera->sList_available), pWaiting);
            SYS_SemIncrement(&(pCamera->sSemAvailable));
        }
        while ((pWaiting = List_popFront(&(pCamera->sList_pending))) != NULL) // if the stop was forced the list may be empty when receiving an interrupt
        {
            List_pushBack(&(pCamera->sList_available), pWaiting);
            SYS_SemIncrement(&(pCamera->sSemAvailable));
        }
    }
    SYS_LockRelease(&(pCamera->sPendingLock));

    // reset gasket counter (MIPI_ERROR_CLEAR)
    // reset data gen (DG_RESET)
    //IMG_UINT32 tmpReg = 0;

    //REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_RESET, DG_RESET, 1); // no need for reset at the end: reset is done at start
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], FELIX_TEST_DG_DG_CTRL_OFFSET, 0);

    // wait for DG to be disabled
    FELIX_FORCEPOLLREG(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 0); // effective only when using unit tests with TAL NULL interface

    if (TALREG_Poll32(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], FELIX_TEST_DG_DG_CTRL_OFFSET, TAL_CHECKFUNC_ISEQUAL, 0, FELIX_TEST_DG_DG_CTRL_DG_ENABLE_MASK, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT) != IMG_SUCCESS)
    {
        CI_FATAL("Failed to stop the data generator\n");
        return IMG_ERROR_FATAL;
    }

    if ((ret = KRN_DG_DriverReleaseContext(pCamera)) != IMG_SUCCESS)
    {
        CI_FATAL("Failed to release the DG Camera\n");
        return ret;
    }

    // unmap from MMU before adding the flushed buffers - do not disable interrupts for that
    List_visitor(&(pCamera->sList_available), NULL, &List_UnmapFrame);

    if (g_pDGDriver->sMMU.uiAllocAtom > 0)
    {
        KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU), pCamera->publicCamera.ui8DGContext, IMG_TRUE);
    }

    pCamera->bStarted = IMG_FALSE;

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8DGContext], "done");

    return ret;
}

IMG_RESULT KRN_DG_CameraDestroy(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret;
    IMG_UINT32 i;
    IMG_ASSERT(pCamera != NULL);

    // if g_pDGDriver is NULL we are cleaning the left-overs so it does not matter much does it?
    if (pCamera->bStarted == IMG_TRUE && g_pDGDriver != NULL)
    {
        if ((ret = KRN_DG_CameraStop(pCamera)) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to stop DG camera\n");
            return ret;
        }
    }

    // no need for locking the capture is stopped no other object can access
    //SYS_SpinlockAcquire(&(pCamera->sPendingSpinLock));
    {
        // these lists can't use the clear() function because the cells should not be destroyed!
        List_visitor(&(pCamera->sList_available), NULL, &List_DestroyFrames);
        //    while(List_popBack(&(pCamera->sList_available)));

        List_visitor(&(pCamera->sList_converting), NULL, &List_DestroyFrames);
        //    while(List_popBack(&(pCamera->sList_converting)));

        List_visitor(&(pCamera->sList_pending), NULL, &List_DestroyFrames);
        //    while(List_popBack(&(pCamera->sList_pending)));
    }
    //SYS_SpinlockRelease(&(pCamera->sPendingSpinLock));

    SYS_SemDestroy(&(pCamera->sSemAvailable));

    SYS_LockDestroy(&(pCamera->sPendingLock));

    for (i = 0; i < DG_N_HEAPS; i++)
    {
        if (pCamera->apHeap[i] != NULL)
        {
            IMGMMU_HeapDestroy(pCamera->apHeap[i]);
        }
    }

    IMG_FREE(pCamera);

    return IMG_SUCCESS;
}

// Assumes that sPendingSpinLock is locked when called
IMG_RESULT KRN_DG_CameraFrameCaptured(KRN_DG_CAMERA *pCamera)
{
    //
    // done in interrupt handler context
    //
    sCell_T *pHead = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pCamera != NULL);

    pHead = List_popFront(&(pCamera->sList_pending));

    /* received an interrupt without pending frame is erroneous!
     * but it could happen if we force a stop before the interrupt */
    if (pHead != NULL)
    {
        CI_INFO("DG %d received frame %u\n",
            pCamera->publicCamera.ui8DGContext,
            ((KRN_DG_FRAME*)pHead->object)->identifier);

        List_pushBack(&(pCamera->sList_available), pHead);
        SYS_SemIncrement(&(pCamera->sSemAvailable));

        if (pCamera->sList_pending.ui32Elements > 0)
        {
            // get the new head
            pHead = List_getHead(&(pCamera->sList_pending));
            IMG_ASSERT(pHead != NULL);
            ret = HW_DG_CameraSetNextFrame(pCamera,
                (KRN_DG_FRAME*)pHead->object);
        }
    }

    return IMG_SUCCESS;
}

// frame

IMG_RESULT KRN_DG_FrameCreate(KRN_DG_FRAME **ppFrame, KRN_DG_CAMERA *pCam)
{
    IMG_SIZE allocSize = 0;
    IMG_RESULT ret;

    IMG_ASSERT(ppFrame != NULL);
    IMG_ASSERT(pCam != NULL);

    if (*ppFrame != NULL)
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppFrame = (KRN_DG_FRAME*)IMG_CALLOC(1, sizeof(KRN_DG_FRAME));

    if (*ppFrame == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    (*ppFrame)->sListCell.object = *ppFrame;

    (*ppFrame)->pParent = pCam;
    // need to be aligned to memory per line
    (*ppFrame)->uiStride = (pCam->publicCamera.uiStride + SYSMEM_ALIGNMENT - 1) /
        SYSMEM_ALIGNMENT * SYSMEM_ALIGNMENT;
    allocSize = (*ppFrame)->uiStride * pCam->publicCamera.uiHeight;

    if (allocSize == 0)
    {
        CI_FATAL("The frame allocation stride is 0\n");
        IMG_FREE(*ppFrame);
        *ppFrame = NULL;
        return IMG_ERROR_FATAL;
    }

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " - start");

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " buffer allocation");
#ifdef INFOTM_ISP
    if ((ret = SYS_MemAlloc(&((*ppFrame)->sBuffer), allocSize, pCam->apHeap[DG_IMAGE_HEAP], 0, "DgBuf")) != IMG_SUCCESS)
#else
    if ((ret = SYS_MemAlloc(&((*ppFrame)->sBuffer), allocSize, pCam->apHeap[DG_IMAGE_HEAP], 0)) != IMG_SUCCESS)
#endif //INFOTM_ISP
    {
        CI_FATAL("failed to allocate frame buffer\n");
        return ret;
    }

#if defined(DEBUG_MEMSET_IMG_BUFFERS)
    {
        void *ptr = Platform_MemGetMemory(&((*ppFrame)->sBuffer));
        CI_DEBUG("memset DG buffer\n");

        IMG_MEMSET(ptr, 51, allocSize); // 0x33

        Platform_MemUpdateDevice(&((*ppFrame)->sBuffer));
}
#endif

    /*CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " linked-list allocation");
      if ( (ret=SYS_MemAlloc(&((*ppFrame)->sConfig), DATA_GENERATION_POINTERS_GROUP_BYTE_SIZE, pCam->apHeap[DG_DATA_HEAP]))!=IMG_SUCCESS )
      {
      CI_FATAL("failed to allocate configuration buffer\n");
      return ret;
      }

      SYS_MemWriteWord(&((*ppFrame)->sConfig), HW_DG_OFF(DG_LINK_ADDR), SYSMEM_INVALID_ADDR); // next element is NULL
      //SYS_MemWriteWord(&pBuffer->sConfig, HW_DG_OFF(DG_TAG), pBuffer->pParent); // this is not used
      SYS_MemWriteAddress(&((*ppFrame)->sConfig), HW_DG_OFF(DG_ADDR_DATA), &((*ppFrame)->sBuffer), 0); // the image address
      SYS_MemWriteWord(&((*ppFrame)->sConfig), HW_DG_OFF(DG_DATA_STRIDE), (*ppFrame)->uiStride); // no need to divide by sysmem the bottom bits are masked
      Platform_MemUpdateDevice(&((*ppFrame)->sConfig));*/

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, " - done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameMap(KRN_DG_FRAME *pFrame, struct MMUDirectory *pDir)
{
    IMG_ASSERT(pFrame != NULL);

    if (pDir != NULL)
    {
        CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "map DG buffer");
        //SYS_MemMap(&(pFrame->sConfig), pDir, MMU_RO);
        SYS_MemMap(&(pFrame->sBuffer), pDir, MMU_RO);
    }
    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameUnmap(KRN_DG_FRAME *pFrame)
{
    IMG_ASSERT(pFrame != NULL);

    //SYS_MemUnmap(&(pFrame->sConfig));
    SYS_MemUnmap(&(pFrame->sBuffer));

    return IMG_SUCCESS;
}

// assumes that pCamera->sPendingSpinLock is already locked
IMG_RESULT HW_DG_CameraSetNextFrame(KRN_DG_CAMERA *pCamera, KRN_DG_FRAME *pFrame)
{
    IMG_UINT32 ui32CurrentVal;
    IMG_UINT8 dg = pCamera->publicCamera.ui8DGContext;

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "submit frame");

    TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_MODE_OFFSET, &ui32CurrentVal);
    if (ui32CurrentVal != FELIX_TEST_DG_DG_MODE_DG_DISABLE)
    {
        CI_WARNING("read DG_MODE 0x%X while expecting 0x%X\n", ui32CurrentVal, FELIX_TEST_DG_DG_MODE_DG_DISABLE);
        return IMG_ERROR_FATAL;
    }
    TALREG_Poll32(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_MODE_OFFSET, TAL_CHECKFUNC_ISEQUAL, ui32CurrentVal, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

#ifdef FELIX_FAKE
    // needed because WriteDevMemRef is pdumped in device memory file when running multiple pdump context
    TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);
#endif
    SYS_MemWriteAddressToReg(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_ADDR_DATA_OFFSET, &(pFrame->sBuffer), 0);
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_DATA_STRIDE_OFFSET, pFrame->uiStride);

    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_MODE_OFFSET, FELIX_TEST_DG_DG_MODE_DG_MANUAL);
#ifdef FELIX_FAKE
    DG_WaitForMaster(dg);

    TAL_Wait(g_pDGDriver->hRegFelixDG[dg], 500); // IDLE for 500 clocks for HW
#endif

    //    CI_INFO("submit frame %u on DG %u (phys 0x%" IMG_PTRDPR "x)\n", pFrame->identifier, dg, Platform_MemGetDevMem(&(pFrame->sBuffer), 0));
#ifdef CI_DEBUGFS
    g_ui32NDGTriggered[dg]++;
#endif

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "submit frame done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_DG_FrameDestroy(KRN_DG_FRAME *pFrame)
{
    IMG_ASSERT(pFrame != NULL);

    if (pFrame->sBuffer.uiAllocated > 0)
    {
        SYS_MemFree(&(pFrame->sBuffer));
    }

    /*if ( pFrame->sConfig.uiAllocated > 0 )
      {
      SYS_MemFree(&(pFrame->sConfig));
      }*/

    IMG_FREE(pFrame);

    return IMG_SUCCESS;
}
