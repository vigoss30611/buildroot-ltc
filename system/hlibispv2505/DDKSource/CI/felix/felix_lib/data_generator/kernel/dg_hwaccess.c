/**
*******************************************************************************
@file dg_hwaccess.c

@brief Access to DG registers

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
#include "ci_kernel/ci_kernel.h"  // CI_FATAL

#include <tal.h>
#include <reg_io2.h>

#include <registers/ext_data_generator.h>
// #include <hw_struct/ext_data_generation.h>  // dg linked list not used
#include <registers/ext_phy.h>  // to write PLL

#define DG_PLL_MULT_MIN 5
#define DG_PLL_MULT_MAX 64
#define DG_PLL_DIV_MIN 1
#define DG_PLL_DIV_MAX 128

#define HW_DG_OFF(reg) DATA_GENERATION_POINTERS_GROUP_ ## reg ## _OFFSET

#ifdef FELIX_UNIT_TESTS

extern IMG_BOOL8 gbUseTalNULL;

#define FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value) \
    (((IMG_UINT32) (ui32value) << (group##_##reg##_##field##_SHIFT)) \
    & (group##_##reg##_##field##_MASK))

#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value) \
    if ( gbUseTalNULL == IMG_TRUE ) \
TALREG_WriteWord32(TalHandle, group ## _ ## reg ## _OFFSET, \
    FELIX_FORCEPOLLREG__tmp(group, reg, field, ui32value));

#else
#define FELIX_FORCEPOLLREG(TalHandle, group, reg, field, ui32value)
#endif

static void HW_DG_WriteGasket(const DG_CAMERA *pCamera)
{
    IMG_UINT32 tmpReg;
    IMG_HANDLE registerBank = NULL;

    IMG_ASSERT(pCamera);

    registerBank = g_pDGDriver->hRegFelixDG[pCamera->ui8Gasket];

    if (CI_DGFMT_MIPI <= pCamera->eFormat)
    {
        CI_PDUMP_COMMENT(registerBank, "configure MIPI protocol");
        // set DG_FRAME_SIZE in HW_DG_CameraSetNextFrame()

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_ZERO,
            pCamera->ui16MipiTHS_zero);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_PREPARE,
            pCamera->ui16MipiTHS_prepare);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_START_OF_DATA_TRANSMISSION, DG_TLPX,
            pCamera->ui16MipiTLPX);
        TALREG_WriteWord32(registerBank,
            FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_END_OF_DATA_TRANSMISSION,
            DG_THS_EXIT, pCamera->ui16MipiTHS_exit);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_END_OF_DATA_TRANSMISSION,
            DG_THS_TRAIL, pCamera->ui16MipiTHS_trail);
        TALREG_WriteWord32(registerBank,
            FELIX_TEST_DG_DG_MIPI_END_OF_DATA_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_ZERO,
            pCamera->ui16MipiTCLK_zero);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_PREPARE,
            pCamera->ui16MipiTCLK_prepare);
        TALREG_WriteWord32(registerBank,
            FELIX_TEST_DG_DG_MIPI_START_OF_CLOCK_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg,
            FELIX_TEST_DG, DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_TRAIL,
            pCamera->ui16MipiTCLK_trail);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_POST,
            pCamera->ui16MipiTCLK_post);
        TALREG_WriteWord32(registerBank,
            FELIX_TEST_DG_DG_MIPI_END_OF_CLOCK_TRANSMISSION_OFFSET, tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG,
            MIPI_CTRL, DG_MIPI_LANES,
            pCamera->ui8MipiLanes);
        TALREG_WriteWord32(registerBank,
            FELIX_TEST_DG_MIPI_CTRL_OFFSET, tmpReg);
    }
    else  // parallel
    {
        // set DG_FRAME_SIZE in HW_DG_CameraSetNextFrame()

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, PARALLEL_CTRL,
            PARALLEL_V_SYNC_POLARITY, pCamera->bParallelActive[0]);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, PARALLEL_CTRL,
            PARALLEL_H_SYNC_POLARITY, pCamera->bParallelActive[1]);
        TALREG_WriteWord32(registerBank, FELIX_TEST_DG_PARALLEL_CTRL_OFFSET,
            tmpReg);
    }
}

void HW_DG_CameraReset(IMG_UINT8 ui8Context)
{
    IMG_HANDLE hDGTalHandle = g_pDGDriver->hRegFelixDG[ui8Context];
    IMG_UINT32 tmpReg = 0;

    // reset gasket counter (MIPI_ERROR_CLEAR)
    // reset data gen (DG_RESET)
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_RESET, DG_RESET, 1);
    TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_RESET_OFFSET,
        tmpReg);
#ifdef FELIX_FAKE
    // arbitrary time given by HW
    TAL_Wait(g_psCIDriver->sTalHandles.hRegFelixCore, CI_REGPOLL_TIMEOUT);
#endif
}

IMG_RESULT HW_DG_CameraConfigure(KRN_DG_CAMERA *pCamera)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT8 ui8Context = pCamera->publicCamera.ui8Gasket;
    IMG_BOOL8 bIsMIPI = IMG_FALSE;

    if (CI_DGFMT_MIPI <= pCamera->publicCamera.eFormat)
    {
        bIsMIPI = IMG_TRUE;
    }

    SYS_LockAcquire(&(g_pDGDriver->sCameraLock));
    {
        IMG_HANDLE hDGTalHandle = g_pDGDriver->hRegFelixDG[ui8Context];
        IMG_UINT32 tmpReg = 0;

        CI_PDUMP_COMMENT(hDGTalHandle, "start");

        HW_DG_CameraReset(ui8Context);

        tmpReg = 0;
        TALREG_ReadWord32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET,
            &tmpReg);
#if FELIX_VERSION_MAJ < 3
        g_pDGDriver->aFrameSent[ui8Context] = REGIO_READ_FIELD(tmpReg,
            FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
#else
        g_pDGDriver->aFrameSent[ui8Context] = REGIO_READ_FIELD(tmpReg,
            FELIX_TEST_DG, DG_STATUS, DG_CAPTURES_SENT);
#endif
#ifdef FELIX_FAKE
        TALREG_Poll32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET,
            TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif

        // wait for DG to be disabled?
        // effective only when using unit tests with TAL NULL interface
        FELIX_FORCEPOLLREG(hDGTalHandle, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 0);

        if (TALREG_Poll32(hDGTalHandle, FELIX_TEST_DG_DG_CTRL_OFFSET,
            TAL_CHECKFUNC_ISEQUAL, 0, FELIX_TEST_DG_DG_CTRL_DG_ENABLE_MASK,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to poll the control-enable bit\n");
            ret = IMG_ERROR_FATAL;
            goto start_out;
        }

        // set interrupt: end of frame + error
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_INTER_ENABLE,
            DG_INT_END_OF_FRAME_EN, 1);
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_INTER_ENABLE,
            DG_INT_ERROR_EN, 1);
        TALREG_WriteWord32(hDGTalHandle,
            FELIX_TEST_DG_DG_INTER_ENABLE_OFFSET, tmpReg);

        /* in a separated function for the moment because we may choose to
         * update these parameters per frame */
        HW_DG_WriteGasket(&(pCamera->publicCamera));

        // setup DG_CTRL
        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 1);

        if (!bIsMIPI)
        {
            if (10 == pCamera->publicCamera.ui8FormatBitdepth)
            {
                REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL,
                    DG_PIXEL_FORMAT, 0);  // 10b
            }
            else if (12 == pCamera->publicCamera.ui8FormatBitdepth)
            {
                REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL,
                    DG_PIXEL_FORMAT, 1);  // 12b
            }
            else
            {
                CI_FATAL("invalid parallel bithdepth %d!\n",
                    pCamera->publicCamera.ui8FormatBitdepth);
                ret = IMG_ERROR_FATAL;
                goto start_out;
            }
        }

        // MIPI or BT656
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_IF_MODE, bIsMIPI);
#if FELIX_VERSION_MAJ < 3
        // LS flag - if BT656 this should be IMG_FALSE (0)
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_MIPI_LS_EN,
            pCamera->publicCamera.eFormat == CI_DGFMT_MIPI_LF ? 1 : 0);
#endif
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_PRELOAD,
            pCamera->publicCamera.bPreload == IMG_TRUE ? 1 : 0);
#if FELIX_VERSION_MAJ > 1
        // choose the MMU requestor to use
        REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_CTRL, DG_GROUP_OVERRIDE,
            pCamera->publicCamera.ui8Gasket%CI_MMU_N_DIR);
#endif
        TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_CTRL_OFFSET,
            tmpReg);

        /* use the DG as manual: the mode is set when submitting a frame
         * in HW_DG_CameraSetNextFrame */

        CI_PDUMP_COMMENT(hDGTalHandle, "done");
    }
start_out:
    SYS_LockRelease(&(g_pDGDriver->sCameraLock));

    return ret;
}

IMG_RESULT HW_DG_CameraStop(KRN_DG_CAMERA *pCamera)
{
    IMG_HANDLE hDGTalHandle = NULL;

    IMG_ASSERT(g_pDGDriver);
    IMG_ASSERT(pCamera);

    hDGTalHandle = g_pDGDriver->hRegFelixDG[pCamera->publicCamera.ui8Gasket];
    // reset gasket counter (MIPI_ERROR_CLEAR)
    // reset data gen (DG_RESET)
    // IMG_UINT32 tmpReg = 0;

    // no need for reset at the end: reset is done at start
    // REGIO_WRITE_FIELD(tmpReg, FELIX_TEST_DG, DG_RESET, DG_RESET, 1);
    TALREG_WriteWord32(hDGTalHandle, FELIX_TEST_DG_DG_CTRL_OFFSET, 0);

    // wait for DG to be disabled
    // effective only when using unit tests with TAL NULL interface
    FELIX_FORCEPOLLREG(hDGTalHandle, FELIX_TEST_DG, DG_CTRL, DG_ENABLE, 0);

    if (TALREG_Poll32(hDGTalHandle, FELIX_TEST_DG_DG_CTRL_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, 0, FELIX_TEST_DG_DG_CTRL_DG_ENABLE_MASK,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT) != IMG_SUCCESS)
    {
        CI_FATAL("Failed to stop the data generator\n");
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

// assumes that pCamera->sPendingSpinLock is already locked
IMG_RESULT HW_DG_CameraSetNextFrame(KRN_DG_CAMERA *pCamera,
    KRN_DG_FRAME *pFrame)
{
    IMG_UINT32 ui32CurrentVal;
    IMG_UINT8 dg = pCamera->publicCamera.ui8Gasket;

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "submit frame");

    TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_MODE_OFFSET, &ui32CurrentVal);
    if (ui32CurrentVal != FELIX_TEST_DG_DG_MODE_DG_DISABLE)
    {
        CI_WARNING("read DG_MODE 0x%X while expecting 0x%X\n", ui32CurrentVal,
            FELIX_TEST_DG_DG_MODE_DG_DISABLE);
        return IMG_ERROR_FATAL;
    }
    TALREG_Poll32(g_pDGDriver->hRegFelixDG[dg], FELIX_TEST_DG_DG_MODE_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, ui32CurrentVal, ~0,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

    // set DG_FRAME_BLANKING
    ui32CurrentVal = 0;
    REGIO_WRITE_FIELD(ui32CurrentVal, FELIX_TEST_DG, DG_FRAME_BLANKING,
        DG_VERTICAL_BLANKING, pFrame->ui32VertBlanking);
    REGIO_WRITE_FIELD(ui32CurrentVal, FELIX_TEST_DG, DG_FRAME_BLANKING,
        DG_HORIZONTAL_BLANKING, pFrame->ui32HoriBlanking);
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_FRAME_BLANKING_OFFSET, ui32CurrentVal);

    /* in the future we may want to call HW_DG_WriteGasket to update the
     * configuration per frame */
    // HW_DG_WriteGasket(pFrame, g_pDGDriver->hRegFelixDG[dg]);

    // set DG_FRAME_SIZE
    ui32CurrentVal = 0;
    REGIO_WRITE_FIELD(ui32CurrentVal, FELIX_TEST_DG, DG_FRAME_SIZE,
        DG_FRAME_HEIGHT, pFrame->ui16Height - 1);
    /* this is the number of pixels not the number of bytes for parallel
     * For MIPI it is the converted width in bytes */
    /*if (CI_DGFMT_MIPI <= pFrame->pParent->publicCamera.eFormat)
    {
        REGIO_WRITE_FIELD(ui32CurrentVal, FELIX_TEST_DG, DG_FRAME_SIZE,
            DG_FRAME_WIDTH, pFrame->ui32PacketWidth - 1);
    }
    else*/
    {
        REGIO_WRITE_FIELD(ui32CurrentVal, FELIX_TEST_DG, DG_FRAME_SIZE,
            DG_FRAME_WIDTH, pFrame->ui16Width - 1);
    }
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_FRAME_SIZE_OFFSET, ui32CurrentVal);

#ifdef FELIX_FAKE
    /* needed because WriteDevMemRef is pdumped in device memory file when
     * running multiple pdump context */
    TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);
#endif

    SYS_MemWriteAddressToReg(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_ADDR_DATA_OFFSET, &(pFrame->sBuffer), 0);
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_DATA_STRIDE_OFFSET, pFrame->uiStride);

    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dg],
        FELIX_TEST_DG_DG_MODE_OFFSET, FELIX_TEST_DG_DG_MODE_DG_MANUAL);

#ifdef FELIX_FAKE
    DG_WaitForMaster(dg);  // if using transif need to wait for master

    // IDLE for 500 clocks for HW testing
    TAL_Wait(g_pDGDriver->hRegFelixDG[dg], 500);
#endif

#ifdef CI_DEBUGFS
    g_ui32NDGTriggered[dg]++;
#endif

    CI_PDUMP_COMMENT(g_pDGDriver->hMemHandle, "submit frame done");

    return IMG_SUCCESS;
}

IMG_RESULT HW_DG_WritePLL(IMG_UINT8 ui8ClkMult, IMG_UINT8 ui8ClkDiv)
{
#if defined(DG_WRITE_PLL)
    IMG_UINT32 ui32Value = 0;
    IMG_RESULT ret = IMG_SUCCESS;

    if (ui8ClkMult < ui8ClkDiv)
    {
        CI_FATAL("pll cannot be smaller than 1: %d/%d\n",
            (int)ui8ClkMult, (int)ui8ClkDiv);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (ui8ClkMult < DG_PLL_MULT_MIN || ui8ClkMult > DG_PLL_MULT_MAX
        || ui8ClkDiv < DG_PLL_DIV_MIN || ui8ClkDiv > DG_PLL_DIV_MAX)
    {
        CI_FATAL("pll %d/%d out of range (mult: %d-%d, div %d-%d)\n",
            (int)ui8ClkMult, (int)ui8ClkDiv,
            DG_PLL_MULT_MIN, DG_PLL_MULT_MAX,
            DG_PLL_DIV_MIN, DG_PLL_DIV_MAX);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    CI_WARNING("setting DG PLL to %d/%d\n", (int)ui8ClkMult, (int)ui8ClkDiv);
    REGIO_WRITE_FIELD(ui32Value, FELIX_GASK_PHY, DG_PIXEL_RATE,
        DG_PIXEL_CLK_MULT, ui8ClkMult);
    REGIO_WRITE_FIELD(ui32Value, FELIX_GASK_PHY, DG_PIXEL_RATE,
        DG_PIXEL_CLK_DIV, ui8ClkDiv);
    TALREG_WriteWord32(g_pDGDriver->hRegGasketPhy,
        FELIX_GASK_PHY_DG_PIXEL_RATE_OFFSET, ui32Value);
    // write DG_REF_CLK_SET to 1 and wait for it to settle at 0
    ui32Value = 1;
    TALREG_WriteWord32(g_pDGDriver->hRegGasketPhy,
        FELIX_GASK_PHY_DG_REF_CLK_SET_OFFSET, ui32Value);
    ret = TALREG_Poll32(g_pDGDriver->hRegGasketPhy,
        FELIX_GASK_PHY_DG_REF_CLK_SET_OFFSET,
        TAL_CHECKFUNC_NOTEQUAL, 0, 0x1, 1000, 20);
    if (ret)
    {
        CI_FATAL("PLLs failed to settle - FPGA needs reprogramming!\n");
        ret = IMG_ERROR_FATAL;
    }

    return ret;
#else
    CI_DEBUG("Ignore PLL writing because compiled without DG_WRITE_PLL\n");
    return IMG_SUCCESS;
#endif /* DG_WRITE_PLL */
}
