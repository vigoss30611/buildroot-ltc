/**
*******************************************************************************
@file ci_hwstruct.c

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
#include "ci/ci_modules_structs.h"
#include "ci_kernel/ci_hwstruct.h"
#include "ci_kernel/ci_kernel.h" // for CI_FATAL
#include "ci/ci_version.h"
#include "ci_kernel/ci_connection.h"
#include <img_types.h>

#include <hw_struct/save.h>
#include <hw_struct/ctx_config.h>
#include <hw_struct/ctx_config_2_4.h>
#include <hw_struct/ctx_pointers.h>

#include <registers/context0.h>
#include <registers/core.h>
#include <registers/gammalut.h>
#if FELIX_VERSION_MAJ == 1
#include <registers/gaskets.h>
#else
#include <registers/gasket_mipi.h>
#include <registers/gasket_parallel.h>
#include <registers/data_generator.h>
#endif
#include <registers/mmu.h>

#include <felix_hw_info.h>

#include <tal.h>
#include <reg_io2.h>

#include "ci_internal/sys_mem.h"

/*
 * Shortcut for very long offsets macro
 */
#define HW_LOAD_OFF(reg) (FELIX_LOAD_STRUCTURE_ ## reg ## _OFFSET)
#define HW_LOAD_NO(reg) FELIX_LOAD_STRUCTURE_ ## reg ## _NO_ENTRIES

#define HW_LLIST_OFF(reg) FELIX_LINK_LIST_ ## reg ## _OFFSET

/*
 * test of array sizes at pre-processor time
 */

#ifndef LSH_NOT_AVAILABLE
#if HW_LOAD_NO(LSH_GRADIENTS_X) != LSH_GRADS_NO
#error "channel gradients value is not valid"
#endif
#endif // LSH_NOT_AVAILABLE

#if HW_LOAD_NO(LAT_CA_DELTA_COEFF_RED) != YCC_COEFF_NO
#error "lateral chroma aberration value is not valid"
#endif
#if HW_LOAD_NO(CC_COEFFS_A) != YCC_COEFF_NO
#error "colour correction value is not valid"
#endif
#if YCC_COEFF_NO != RGB_COEFF_NO \
    || HW_LOAD_NO(RGB_TO_YCC_MATRIX_COEFFS) != RGB_COEFF_NO \
    || HW_LOAD_NO(YCC_TO_RGB_MATRIX_COEFFS) != RGB_COEFF_NO \
    || HW_LOAD_NO(RGB_TO_YCC_MATRIX_COEFF_OFFSET) != RGB_COEFF_NO \
    || HW_LOAD_NO(YCC_TO_RGB_MATRIX_COEFF_OFFSET) != RGB_COEFF_NO
#error "colour conversion matrices are not symetrical"
#endif

/* HW has N registers with 2 fields and stores (N/2)-1 curve points and
 * 1 input delta
 * SW stores the curve */
#ifndef TNM_NOT_AVAILABLE
#if (2*HW_LOAD_NO(TNM_GLOBAL_CURVE)-1) != TNM_CURVE_NPOINTS
#error "tone mapping curve point value is not valid"
#endif
#endif // TNM_NOT_AVAILABLE

#if HW_LOAD_NO(DISP_SCAL_V_CHROMA_TAPS_0_TO_1) != 4 \
    || HW_LOAD_NO(DISP_SCAL_V_LUMA_TAPS_0_TO_3) != 4 \
    || HW_LOAD_NO(DISP_SCAL_H_CHROMA_TAPS_0_TO_3) != 4 \
    || HW_LOAD_NO(DISP_SCAL_H_LUMA_TAPS_0_TO_3) != 4 \
    || HW_LOAD_NO(DISP_SCAL_H_LUMA_TAPS_4_TO_7) != 4
#error "Number of scaler taps is unexpected"
#endif
#if IMG_TRUE != 1
#error "IMG_TRUE is not 1"
#endif

#if DSC_H_LUMA_TAPS != (ESC_H_LUMA_TAPS/2) \
    || DSC_H_CHROMA_TAPS != (ESC_H_CHROMA_TAPS/2) \
    || DSC_V_LUMA_TAPS != (ESC_V_LUMA_TAPS/2) \
    || DSC_V_CHROMA_TAPS != (ESC_V_CHROMA_TAPS/2)
#error "Internal way of computing the number of taps for the DSC schanged"
#endif

#if BLC_OFFSET_NO != 4
#error "Number of black correction offsets unexpected"
#endif

// 2 fields per GMA register
#if GMA_N_POINTS/2 > FELIX_GAMMA_LUT_GMA_RED_POINT_NO_ENTRIES
#error GMA_N_POINTS is wrong
#endif

#if FELIX_LOAD_STRUCTURE_BYTE_SIZE > 4096 // bigger than a page
#error need some code modification for pdumping
#endif

// SHA
#if FELIX_CONTEXT0_SHA_DN_EDGE_SIMIL_COMP_PTS_0_SHA_DN_EDGE_SIMIL_COMP_PTS_0_TO_3_NO_REPS+FELIX_CONTEXT0_SHA_DN_EDGE_SIMIL_COMP_PTS_1_SHA_DN_EDGE_SIMIL_COMP_PTS_4_TO_6_NO_REPS != SHA_N_COMPARE
#error number of comparison in SHA DN changed
#endif

// MIE
#if MIE_GAUSS_SC_N != MIE_GAUSS_GN_N
#error MIE gauss scale and gauss gain are no longuer the same
#endif

#if FELIX_VERSION_MAJ >= 2
#if FELIX_GAS_MIPI_GASKET_FRAME_CNT_GASKET_FRAME_CNT_MASK != FELIX_GAS_PARA_GASKET_FRAME_CNT_GASKET_FRAME_CNT_MASK
#error the gasket frame counter size should be equal
#endif
#endif

#if RLT_SLICE_N != 4
#error Number of RLT slices changed need to update HW_CI_Load_RLT
#endif

#ifndef CI_HW_REF_CLK
#error CI_HW_REF_CLK should be defined when compiling!
#endif

/*
 * When using the FAKE driver the CSIM may take a while to do some
 * actions therefore we prepare the poll by doing several reads with longuer
 * timeouts
 * 
 * If compiled for unit tests the POLL is faked by writing the expected value
 * as there is no HW to actually update the register
 */
#if defined(FELIX_FAKE) || defined(ANDROID_EMULATOR)

#ifdef FELIX_UNIT_TESTS
#define FAKE_POLL(handle, offset, value, mask) \
    TALREG_WriteWord32(handle, offset, value&mask);
#else

static void msleep(IMG_UINT32 ui32SleepMiliSec);

#define FAKE_POLL(handle, offset, value, mask) \
    { \
        int tries = CI_REGPOLL_TRIES; \
        int v; \
        while (tries) { \
            TALREG_ReadWord32(handle, offset, &v); \
            if ((v&mask) == (value&mask)) { \
                tries = 0; \
                break; \
            } \
            tries--; \
            msleep(CI_REGPOLL_TIMEOUT*10); \
        } \
    }

#endif // FELIX_UNIT_TESTS

#else // FELIX_FAKE

#define FAKE_POLL(...)

#endif

/*
 * pre-processor tests done - compile time checks for enums
 */

/**
 * @param pMem memory pointer to start of memory
 * @param uiOffset in bytes
 * @param ui32Val value
 */
static void WriteMem(IMG_UINT32 *pMem, IMG_SIZE uiOffset, IMG_UINT32 ui32Val)
{
    IMG_ASSERT(uiOffset < FELIX_LOAD_STRUCTURE_BYTE_SIZE);
    pMem[ uiOffset/4 ] = ui32Val;

#if defined(FELIX_FAKE) || CI_LOG_LEVEL > 4
    {
        char message[128];
        sprintf(message, "Write STAMP_%p:0x%"IMG_SIZEPR"X 0x%X", 
            pMem, uiOffset, ui32Val);
#ifdef FELIX_FAKE
        TALPDUMP_Comment(g_psCIDriver->sTalHandles.hMemHandle, message);
#else
        CI_INFO(message);
#endif
    }
#endif
}

/* Function ReadMem() is not used at the moment */
/*
static IMG_UINT32 ReadMem(IMG_UINT32 *pMem, IMG_SIZE uiOffset)
{
    IMG_ASSERT(uiOffset < FELIX_LOAD_STRUCTURE_BYTE_SIZE);
    return pMem[ uiOffset/4 ];
}
*/

/*
 * size access
 */

IMG_SIZE HW_CI_LinkedListSize(void)
{
    return FELIX_LINK_LIST_BYTE_SIZE;
}

IMG_SIZE HW_CI_LoadStructureSize(void)
{
    return FELIX_LOAD_STRUCTURE_BYTE_SIZE;
}

IMG_SIZE HW_CI_SaveStructureSize(void)
{
    return FELIX_SAVE_BYTE_SIZE;
}
        

/*
 * Register write
 */

/// @warning assumes that the global context lock is acquired
static void HW_CI_Reg_IFF(IMG_HANDLE contextHandle, CI_MODULE_IIF *pIIF)
{
    IMG_UINT32 value;
    enum FELIX_CONTEXT0_IMGR_CTRL_IMGR_BAYER_FORMAT_ENUM eBayerFormat = 
        FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB;

    switch(pIIF->eBayerFormat)
    {
    case MOSAIC_RGGB:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB;
        break;
    case MOSAIC_GRBG:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GRBG;
        break;
    case MOSAIC_GBRG:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GBRG;
        break;
    case MOSAIC_BGGR:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_BGGR;
        break;
    default:
        CI_FATAL("user-side mosaic is unknown - using RGGB\n");
    }

    // imagers information
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL, 
        IMGR_INPUT_SEL, pIIF->ui8Imager);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL, 
        IMGR_BAYER_FORMAT, eBayerFormat);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL, 
        IMGR_SCALER_BOTTOM_BORDER, pIIF->ui16ScalerBottomBorder);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL, 
        IMGR_BUF_THRESH, pIIF->ui16BuffThreshold);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_IMGR_CTRL_OFFSET, value);

    // clipping rectangle
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CAP_OFFSET,
        IMGR_CAP_OFFSET_X, pIIF->ui16ImagerOffset[0]);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_CAP_OFFSET, 
        IMGR_CAP_OFFSET_Y, pIIF->ui16ImagerOffset[1]);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_IMGR_CAP_OFFSET_OFFSET,
        value);

    // output size
    value = 0;
    // warning: number of CFA rows (half nb captured -1)
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_OUT_SIZE,
        IMGR_OUT_ROWS_CFA, pIIF->ui16ImagerSize[1]);
    // warning: number of CFA columns (half nb captured -1)
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_OUT_SIZE,
        IMGR_OUT_COLS_CFA, pIIF->ui16ImagerSize[0]);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_IMGR_OUT_SIZE_OFFSET, 
        value);

    // decimation
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_IMAGE_DECIMATION, 
        BLACK_BORDER_OFFSET, pIIF->ui8BlackBorderOffset);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_IMAGE_DECIMATION, 
        IMGR_COMP_SKIP_X, pIIF->ui16ImagerDecimation[0]);
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, IMGR_IMAGE_DECIMATION, 
        IMGR_COMP_SKIP_Y, pIIF->ui16ImagerDecimation[1]);
    TALREG_WriteWord32(contextHandle, 
        FELIX_CONTEXT0_IMGR_IMAGE_DECIMATION_OFFSET, value);
}

IMG_RESULT HW_CI_PipelineRegStart(KRN_CI_PIPELINE *pCapture)
{
    IMG_UINT32 value;
    IMG_UINT8 ui8Context;
    IMG_HANDLE contextHandle;
    IMG_RESULT ret;
#ifdef FELIX_FAKE // no pdump in kernel
    IMG_CHAR pszMessage[32];
#endif

    IMG_ASSERT(pCapture);
    IMG_ASSERT(g_psCIDriver);

    ui8Context = pCapture->userPipeline.ui8Context;
    contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[ui8Context];

    if (!pCapture->bStarted)
    {
        CI_FATAL("capture is not started.\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

#ifdef FELIX_FAKE
    sprintf(pszMessage, "start ctx %d", ui8Context);
    CI_PDUMP_COMMENT(contextHandle, pszMessage);
#endif

    TALREG_ReadWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        &value);
    ret = REGIO_READ_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        CAPTURE_MODE);
    if (FELIX_CONTEXT0_CAPTURE_MODE_CAP_DISABLE != ret)
    {
        CI_FATAL("context already initialised\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL,
        CAPTURE_STOP, 1);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
    
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_STATUS,
        CONTEXT_STATE, FELIX_CONTEXT0_CONTEXT_STATE_FR_IDLE);

    FAKE_POLL(contextHandle, FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET,
        value, FELIX_CONTEXT0_CONTEXT_STATUS_CONTEXT_STATE_MASK);
    
    ret = TALREG_Poll32(contextHandle, FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET,
        TAL_CHECKFUNC_ISEQUAL,
        value,
        FELIX_CONTEXT0_CONTEXT_STATUS_CONTEXT_STATE_MASK,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to wait for IDLE before starting context %d!\n",
            (int)ui8Context);
        return IMG_ERROR_FATAL;
    }
    
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, 
        CONTEXT_CONTROL, CRC_CLEAR, 1); // reset CRC
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
    
    // we can't check CRCs got reset but we check for frame counter
    
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        COUNTERS_CLEAR, 1); // reset counters
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
    
    /*FAKE_POLL(contextHandle, FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET,
        0, FELIX_CONTEXT0_LAST_FRAME_INFO_LAST_CONTEXT_FRAMES_MASK);
    
    ret = TALREG_Poll32(contextHandle, FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET,
        TAL_CHECKFUNC_ISEQUAL,
        0,
        FELIX_CONTEXT0_LAST_FRAME_INFO_LAST_CONTEXT_FRAMES_MASK,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to wait for frame counter to reset before "\
            "starting context %d!\n", (int)ui8Context);
#ifndef FELIX_FAKE
        return IMG_ERROR_FATAL;
#endif
    }*/
    
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        LINK_LIST_CLEAR, 1); // clear the linked list - to be sure
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
    
#ifdef FELIX_UNIT_TESTS
    /* have the correct value because in unit tests there is no HW to
     * reset the value */
    TALREG_WriteWord32(contextHandle,
        FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET,
        g_psCIDriver->sHWInfo.context_aPendingQueue[ui8Context]);
    
    /* no need for a long wait when FELIX_FAKE because it should be empty
     * already */
#endif
    
    ret = TALREG_Poll32(contextHandle, 
        FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET, 
        TAL_CHECKFUNC_NOTEQUAL, 0, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to initialise the linked list\n");
        return IMG_ERROR_FATAL;
    }

    // initialise the internal semaphore
    SYS_SemInit(&(g_psCIDriver->aListQueue[ui8Context]), 
        g_psCIDriver->sHWInfo.context_aPendingQueue[ui8Context]);

    ret = IMG_SUCCESS;
    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        // set linestore value
        value = 0;
        // the information is given in "pair" of pixels
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, LS_BUFFER_ALLOCATION, 
            LS_CONTEXT_OFFSET, 
            g_psCIDriver->sLinestoreStatus.aStart[ui8Context]>>1); 
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_LS_BUFFER_ALLOCATION_OFFSET, value);

        // setup interrupt enable
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, INTERRUPT_ENABLE, 
            INT_EN_FRAME_DONE_ALL, 1);
#ifndef INFOTM_DISABLE_INT_EN_ERROR_IGNORE
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, INTERRUPT_ENABLE, 
            INT_EN_ERROR_IGNORE, 1);
#endif //INFOTM_DISABLE_INT_EN_ERROR_IGNORE
#ifndef INFOTM_DISABLE_INT_START_OF_FRAME_RECEIVED
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, INTERRUPT_ENABLE, 
            INT_EN_START_OF_FRAME_RECEIVED, 1);
#endif //INFOTM_DISABLE_INT_START_OF_FRAME_RECEIVED
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_INTERRUPT_ENABLE_OFFSET, value);

        // setup cache usage
#if FELIX_VERSION_MAJ == 1
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENC_CACHE_POLICY, USE_ENC_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENC_FORCE_FENCE, USE_ENC_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DISP_DE_CACHE_POLICY, USE_DISP_DE_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DISP_DE_FORCE_FENCE, USE_DISP_DE_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            REG_CACHE_POLICY, USE_REG_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            LENS_CACHE_POLICY, USE_LENS_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            MEM_DUMP_CACHE_POLICY, USE_MEM_DUMP_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENS_CACHE_POLICY, USE_ENS_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            MEM_DUMP_REMOVE_FENCE, USE_MEM_DUMP_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENC_L_REMOVE_FENCE, USE_ENC_L_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DISP_DE_REMOVE_FENCE, USE_DISP_DE_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            OTHERS_REMOVE_FENCE, USE_OTHERS_REMOVE_FENCE);
        TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_MEMORY_CONTROL_OFFSET, 
            value);
#else // FELIX_VERSION_MAJ == 2
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENC_CACHE_POLICY, USE_ENC_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DISP_DE_CACHE_POLICY, USE_DISP_DE_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            REG_CACHE_POLICY, USE_REG_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            LENS_CACHE_POLICY, USE_LENS_CACHE_POLICY);
        // also used by WBS
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            MEM_DUMP_CACHE_POLICY, USE_MEM_DUMP_CACHE_POLICY); 
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            ENS_CACHE_POLICY, USE_ENS_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            RAW_2D_CACHE_POLICY, USE_RAW_2D_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            HDF_RD_CACHE_POLICY, USE_HDF_RD_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DPF_READ_CACHE_POLICY, USE_DPF_READ_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            DPF_WRITE_CACHE_POLICY, USE_DPF_WRITE_CACHE_POLICY);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL, 
            HDF_WR_CACHE_POLICY, USE_HDF_WR_CACHE_POLICY);
        TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_MEMORY_CONTROL_OFFSET,
            value);
        
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            ENC_FORCE_FENCE, USE_ENC_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            DISP_DE_FORCE_FENCE, USE_DISP_DE_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            HDF_WR_FORCE_FENCE, USE_HDF_WR_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            RAW_2D_FORCE_FENCE, USE_RAW_2D_FORCE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            MEM_DUMP_REMOVE_FENCE, USE_MEM_DUMP_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            ENC_L_REMOVE_FENCE, USE_ENC_L_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            DISP_DE_REMOVE_FENCE, USE_DISP_DE_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            OTHERS_REMOVE_FENCE, USE_OTHERS_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            HDF_WR_REMOVE_FENCE, USE_HDF_WR_REMOVE_FENCE);
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, MEMORY_CONTROL2, 
            RAW_2D_REMOVE_FENCE, USE_RAW_2D_REMOVE_FENCE);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_MEMORY_CONTROL2_OFFSET, value);
#endif // FELIX_VERSION
        
        HW_CI_Reg_IFF(contextHandle, 
            &(pCapture->userPipeline.sImagerInterface));

        // system black level
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, SYS_BLACK_LEVEL, 
            SYS_BLACK_LEVEL, pCapture->userPipeline.ui16SysBlackLevel);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_SYS_BLACK_LEVEL_OFFSET, value);

        // enable CRCs in advance otherwise HW doesn't know until LL is loaded
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, SAVE_CONFIG_FLAGS, 
            CRC_ENABLE, 
            ((pCapture->userPipeline.eOutputConfig&CI_SAVE_CRC) != 0 ? 1 : 0));
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_SAVE_CONFIG_FLAGS_OFFSET, value);

        // enable context
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
            CAPTURE_MODE, FELIX_CONTEXT0_CAPTURE_MODE_CAP_LINKED_LIST);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, value);

        // load ENS
        HW_CI_Load_ENS(pCapture->pLoadStructStamp, 
            &(pCapture->userPipeline.sEncStats));
        
        CI_PDUMP_COMMENT(contextHandle, 
            "manually enable/disable CRC at start");
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, SAVE_CONFIG_FLAGS, 
            CRC_ENABLE, 
            (pCapture->userPipeline.eOutputConfig&CI_SAVE_CRC)!=0?1:0);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_SAVE_CONFIG_FLAGS_OFFSET, value);
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

#ifdef FELIX_FAKE // no pdump in kernel
    sprintf(pszMessage, "done (ctx %d)", ui8Context);
    CI_PDUMP_COMMENT(contextHandle, pszMessage);
#endif

    return ret;
}

void HW_CI_PipelineRegStop(KRN_CI_PIPELINE *pCapture, IMG_BOOL8 bPoll)
{
    IMG_UINT32 value = 0;
    IMG_UINT8 ui8Context = pCapture->userPipeline.ui8Context;
    IMG_HANDLE contextHandle =
        g_psCIDriver->sTalHandles.hRegFelixContext[ui8Context];
    IMG_RESULT ret;
#ifdef FELIX_FAKE // no pdump in kernel
    IMG_CHAR pszMessage[32];
#endif

    IMG_ASSERT(pCapture);
    IMG_ASSERT(g_psCIDriver);

#ifdef FELIX_FAKE
    sprintf(pszMessage, "stop ctx %d - start", ui8Context);
    CI_PDUMP_COMMENT(contextHandle, pszMessage);
#endif

    // disable all interrupts for that context
    // if we get any frame done now we consider it too late anyway
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_INTERRUPT_ENABLE_OFFSET,
        0);

    // disable the context - no more frames will start
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        CAPTURE_MODE, FELIX_CONTEXT0_CAPTURE_MODE_CAP_DISABLE);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
    
    // stop currently processing frame
    // not done otherwise the HW status does not recover well
    /*value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        CAPTURE_STOP, 1);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);*/

    /*
     * poll is optional because this can be call in interrupt handler
     * in case of MMU page fault to stop the HW and POLs are waiting
     */ 
    if (bPoll)
    {
        // prepare value for POLL
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_STATUS,
            CONTEXT_STATE, FELIX_CONTEXT0_CONTEXT_STATE_FR_IDLE);

#ifdef FELIX_UNIT_TESTS
        // have the correct value because in unit tests there are not HW
        TALREG_WriteWord32(contextHandle,
            FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET, value);
#endif

        /* need to find the correct waiting time - should be
         * ~1 line of processing */
        ret = TALREG_Poll32(contextHandle,
            FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET, TAL_CHECKFUNC_ISEQUAL,
            value, FELIX_CONTEXT0_CONTEXT_STATUS_CONTEXT_STATE_MASK,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("failed to wait for IDLE after stopping context %d!\n",
                (int)ui8Context);

#ifdef INFOTM_ISP
            //patch from jazz, try to debug not IDLE mode after stopping context
            {
                IMG_UINT32 resetVal = 0;
                IMG_UINT32 i = 0;

                REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
                        COUNTERS_CLEAR, 1);
                REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
                        CRC_CLEAR, 1);
                REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
                        LINK_LIST_CLEAR, 1);

                for ( i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NContexts ; i++ )
                {
                    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[i],
                            FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, resetVal);
                }
                
                CI_FATAL("ResetHW, try to debug not IDLE mode!!!\n");
            }
#endif //INFOTM_ISP			
        }
    }

    // clear linked list
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONTROL, 
        LINK_LIST_CLEAR, 1); // clear the linked list - to be sure
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET,
        value);
#ifdef FELIX_FAKE
    sprintf(pszMessage, "stop ctx %d - done", ui8Context);
    CI_PDUMP_COMMENT(contextHandle, pszMessage);
#endif
}

/*
 *
 */

IMG_UINT32 HW_CI_EnsOutputSize(IMG_UINT8 ui8NcoupleExp, 
    IMG_UINT32 uiImageHeight)
{
    IMG_UINT32 outSize = 1;
    IMG_UINT32 couples = 1<<(ui8NcoupleExp+ENS_EXP_ADD);
    if (uiImageHeight > couples)
    {
        outSize = uiImageHeight/(couples);
        if (0 != uiImageHeight %( couples ))
        {
            outSize++;
        }
    }

#if defined(VERBOSE_DEBUG)
    CI_DEBUG("couple=%d, height=%d, outSize=%d (rounded? %d)\n",
        couples, uiImageHeight,
        outSize, uiImageHeight % (couples) != 0);
#endif
    
    return outSize*ENS_OUT_SIZE;
}

void HW_CI_Load_BLC(char *pMemory, const struct CI_MODULE_BLC *pBlack)
{
#ifndef BLC_NOT_AVAILABLE
    IMG_UINT32 tmp, i;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pBlack);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, BLACK_CONTROL, 
        BLACK_MODE, pBlack->bRowAverage);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, BLACK_CONTROL, 
        BLACK_FRAME, pBlack->bBlackFrame);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(BLACK_CONTROL), tmp);


    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, BLACK_ANALYSIS, 
        BLACK_ROW_RECIPROCAL, pBlack->ui16RowReciprocal);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, BLACK_ANALYSIS, 
        BLACK_PIXEL_MAX, pBlack->ui16PixelMax);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(BLACK_ANALYSIS), tmp);

    tmp = 0;
    for ( i = 0 ; i < BLC_OFFSET_NO ; i++ )
    {
        REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            BLACK_FIXED_OFFSET, BLACK_FIXED, i, pBlack->i8FixedOffset[i]);
    }
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(BLACK_FIXED_OFFSET), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "BLC_NOT_AVAILABLE");
#endif // BLC_NOT_AVAILABLE
}

void HW_CI_Load_RLT(char *pMemory, const CI_MODULE_RLT *pRawLut)
{
#ifndef RLT_NOT_AVAILABLE
    IMG_UINT32 tmp, i;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pRawLut);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0; // assumes enabled
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL, RLT_ENABLE, 1); 
    switch(pRawLut->eMode)
    {
    case CI_RLT_LINEAR:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL, 
            RLT_INTERP_MODE, 
            FELIX_CONTEXT0_RLT_INTERP_MODE_RLT_INTERP_MODE_LINEAR);
        break;
    case CI_RLT_CUBIC:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL, 
            RLT_INTERP_MODE, 
            FELIX_CONTEXT0_RLT_INTERP_MODE_RLT_INTERP_MODE_CUBIC);
        break;
    case CI_RLT_DISABLED:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL, 
            RLT_ENABLE, 0);
        break;
    default:
        CI_FATAL("unknown RLT mode value!\n");
        IMG_ASSERT(0);
    }
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(RLT_CONTROL), tmp);

    for(i = 0 ; i < RLT_SLICE_N_ENTRIES ; i++)
    {
        /*
         * OOD/EVEN naming is a bit confusing here because 1st entry of 
         * RLT (0) is not present therefore first value stored is in fact 
         * number 1, which is odd numbering...
         */
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_0_POINTS,
            RLT_LUT_0_POINTS_ODD_1_TO_15, pRawLut->aPoints[0][2*i +0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_0_POINTS, 
            RLT_LUT_0_POINTS_EVEN_2_TO_16, pRawLut->aPoints[0][2*i +1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            RLT_LUT_VALUES_0_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_1_POINTS, 
            RLT_LUT_1_POINTS_ODD_1_TO_15, pRawLut->aPoints[1][2*i +0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_1_POINTS, 
            RLT_LUT_1_POINTS_EVEN_2_TO_16, pRawLut->aPoints[1][2*i +1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            RLT_LUT_VALUES_1_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_2_POINTS, 
            RLT_LUT_2_POINTS_ODD_1_TO_15, pRawLut->aPoints[2][2*i +0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_2_POINTS, 
            RLT_LUT_2_POINTS_EVEN_2_TO_16, pRawLut->aPoints[2][2*i +1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            RLT_LUT_VALUES_2_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_3_POINTS, 
            RLT_LUT_3_POINTS_ODD_1_TO_15, pRawLut->aPoints[3][2*i +0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_3_POINTS, 
            RLT_LUT_3_POINTS_EVEN_2_TO_16, pRawLut->aPoints[3][2*i +1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            RLT_LUT_VALUES_3_POINTS, i), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "RLT_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_LSH_WB(char *pMemory, const struct CI_MODULE_WBC *pWBC)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pWBC);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_GAIN_0, 
        WHITE_BALANCE_GAIN_0, pWBC->aGain[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_GAIN_0, 
        WHITE_BALANCE_GAIN_1, pWBC->aGain[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LSH_GAIN_0), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_GAIN_1, 
        WHITE_BALANCE_GAIN_2, pWBC->aGain[2]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_GAIN_1, 
        WHITE_BALANCE_GAIN_3, pWBC->aGain[3]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LSH_GAIN_1), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_CLIP_0, 
        WHITE_BALANCE_CLIP_0, pWBC->aClip[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_CLIP_0, 
        WHITE_BALANCE_CLIP_1, pWBC->aClip[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LSH_CLIP_0), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_CLIP_1, 
        WHITE_BALANCE_CLIP_2, pWBC->aClip[2]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_CLIP_1, 
        WHITE_BALANCE_CLIP_3, pWBC->aClip[3]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LSH_CLIP_1), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "LSH_NOT_AVAILABLE");
#endif // LSH_NOT_AVAILABLE
}

void HW_CI_Load_LSH_DS(char *pMemory, const struct CI_MODULE_LSH *pLens)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pLens);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_ALIGNMENT_X, 
        LSH_SKIP_X, pLens->ui16SkipX);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LSH_ALIGNMENT_X, 
        LSH_OFFSET_X, pLens->ui16OffsetX);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LSH_ALIGNMENT_X), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "BLC_NOT_AVAILABLE");
#endif // BLC_NOT_AVAILABLE
}

/* ASSUMES the capture currently has its context locked! */
IMG_RESULT HW_CI_Reg_LSH_DS(KRN_CI_PIPELINE *pCapture)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_UINT32 tmp;
    IMG_UINT32 i;
    CI_MODULE_LSH *pLens = NULL;
    IMG_HANDLE contextHandle = NULL;

    IMG_ASSERT(pCapture);

    pLens = &(pCapture->userPipeline.sDeshading);

    tmp = pCapture->userPipeline.ui8Context;
    contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[tmp];

    if (!pCapture->bStarted)
    {
        CI_FATAL("capture is not started or is not active.\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    CI_PDUMP_COMMENT(contextHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, 
        LSH_ENABLE, pLens->bUseDeshadingGrid == IMG_TRUE ? 1 : 0);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, 
        LSH_VERTEX_DIFF_BITS, pLens->ui8BitsPerDiff);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_LSH_CTRL_OFFSET, tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, 
        LSH_SKIP_Y, pLens->ui16SkipY);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, 
        LSH_OFFSET_Y, pLens->ui16OffsetY);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, 
        tmp);

    if (pLens->bUseDeshadingGrid)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_GRID_TILE, 
            TILE_SIZE_LOG2, pLens->ui8TileSizeLog2);
        TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_LSH_GRID_TILE_OFFSET,
            tmp);

#ifdef FELIX_FAKE
        /* needed because WriteDevMemRef is pdumped in device memory file when
         * running multiple pdump context */
        TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);
#endif
        SYS_MemWriteAddressToReg(contextHandle, 
            FELIX_CONTEXT0_LSH_GRID_BASE_ADDR_OFFSET, 
            &(pCapture->sDeshadingGrid), 0);

        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_LSH_GRID_STRIDE_OFFSET, 
            pCapture->uiLSHMatrixStride);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_GRID_LINE_SIZE, 
            LSH_GRID_LINE_SIZE, pCapture->ui16LSHLineSize);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_LSH_GRID_LINE_SIZE_OFFSET, tmp);
    }

    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_GRADIENTS_Y, 
            SHADING_GRADIENT_Y, pLens->aGradientsY[i]);
        TALREG_WriteWord32(contextHandle, REGIO_TABLE_OFF(FELIX_CONTEXT0,
            LSH_GRADIENTS_Y, i), tmp);
    }

    // other registers are loaded from device memory, see HW_CI_Load_LSH_DS()

    CI_PDUMP_COMMENT(contextHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(contextHandle, "LSH_NOT_AVAILABLE");
#endif // LSH_NOT_AVAILABLE
    return IMG_SUCCESS;
}

/* do NOT call if the capture is already running! */
IMG_RESULT HW_CI_DPF_ReadMapConvert(KRN_CI_PIPELINE *pCapture,
    IMG_UINT16 *apDefectRead, IMG_UINT32 ui32NDefect)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(pCapture);

    if (pCapture->userPipeline.sDefectivePixels.sInput.ui32NDefect > 0)
    {
        IMG_SIZE needed = ui32NDefect*DPF_MAP_INPUT_SIZE;

        if (pCapture->sDPFReadMap.uiAllocated < needed)
        {
            IMG_UINT32 i;
            // only if started - not available at the moment
            //SYS_MemUnmap(&(pCapture->sDPFReadMap)); 
            SYS_MemFree(&(pCapture->sDPFReadMap));
            pCapture->uiInputMapSize = 0;

#ifdef INFOTM_ISP
            ret = SYS_MemAlloc(&(pCapture->sDPFReadMap), needed,
                pCapture->apHeap[CI_DATA_HEAP], 0, "DPFReadMap");
#else
            ret = SYS_MemAlloc(&(pCapture->sDPFReadMap), needed,
                pCapture->apHeap[CI_DATA_HEAP], 0);
#endif //INFOTM_ISP
            if (IMG_SUCCESS != ret)
            {
                CI_FATAL("failed to allocate the DPF input map in "\
                    "system memory ("IMG_SIZEPR"u bytes)\n", needed);
                return ret;
            }
            /* size is in nb of defects (if it was in bytes it would be 
             * using needed) */
            pCapture->uiInputMapSize = ui32NDefect; 

            // map the buffer if already running! not possible with current HW
            /*if (pCapture->bStarted) 
            {
                i = pCapture->userPipeline.ui8Context;
                SYS_MemMap(&(pCapture->sDPFReadMap), 
                    g_psCIDriver->sMMU.apDirectory[i], MMU_RO);
                KRN_CI_MMUFlushCache(&(g_psCIDriver->sMMU), 
                    pCapture->userPipeline.ui8Context, IMG_FALSE);
            }*/

            for ( i = 0 ; i < ui32NDefect ; i++ )
            {
                IMG_UINT32 defect = 0;

                REGIO_WRITE_FIELD(defect, DPF, MAP_COORD, 
                    X_COORD, apDefectRead[2*i +0]);
                REGIO_WRITE_FIELD(defect, DPF, MAP_COORD, 
                    Y_COORD, apDefectRead[2*i +1]);
                SYS_MemWriteWord(&(pCapture->sDPFReadMap), 
                    (i*DPF_MAP_INPUT_SIZE)+DPF_MAP_COORD_OFFSET, defect);
            }
            Platform_MemUpdateDevice(&(pCapture->sDPFReadMap));
        }


    }

    return ret;
}

void HW_CI_Reg_DPF(KRN_CI_PIPELINE *pCapture)
{
#ifndef DPF_NOT_AVAILABLE
    IMG_HANDLE contextHandle = NULL;
    IMG_UINT32 tmp = 0;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pCapture);

    tmp = pCapture->userPipeline.ui8Context;
    contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[tmp];

    CI_PDUMP_COMMENT(contextHandle, "start");

    if (pCapture->userPipeline.sDefectivePixels.sInput.ui32NDefect > 0
        && pCapture->sDPFReadMap.uiAllocated > 0)
    {
        SYS_MemWriteAddressToReg(contextHandle, 
            FELIX_CONTEXT0_DPF_RD_MAP_ADDR_OFFSET, &(pCapture->sDPFReadMap), 
            0);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_SIZE_CACHE,
            DPF_READ_MAP_SIZE, pCapture->uiInputMapSize);
#if FELIX_VERSION_MAJ == 1
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_SIZE_CACHE, 
            DPF_READ_CACHE_POLICY, USE_DPF_READ_CACHE_POLICY);
#endif
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_DPF_READ_SIZE_CACHE_OFFSET, tmp);
    }
    else
    {
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_DPF_RD_MAP_ADDR_OFFSET, SYSMEM_INVALID_ADDR);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_SIZE_CACHE, 
            DPF_READ_MAP_SIZE, 0);
        TALREG_WriteWord32(contextHandle, 
            FELIX_CONTEXT0_DPF_READ_SIZE_CACHE_OFFSET, tmp);
    }

    /* warning: DPF_READ_BUF_CONTEXT_SIZE value is reserved when starting
     * the capture */
    tmp = 0; 
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_BUF_SIZE, 
        DPF_READ_BUF_CONTEXT_SIZE, 
        pCapture->userPipeline.sDefectivePixels.sInput.ui16InternalBufSize);
    TALREG_WriteWord32(contextHandle, 
        FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_SKIP, 
        DPF_MAP_SKIP_X, pCapture->userPipeline.sDefectivePixels.aSkip[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_SKIP, 
        DPF_MAP_SKIP_Y, pCapture->userPipeline.sDefectivePixels.aSkip[1]);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_DPF_SKIP_OFFSET, tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_OFFSET, 
        DPF_MAP_OFFSET_X, pCapture->userPipeline.sDefectivePixels.aOffset[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_OFFSET, 
        DPF_MAP_OFFSET_Y, pCapture->userPipeline.sDefectivePixels.aOffset[1]);
    TALREG_WriteWord32(contextHandle, FELIX_CONTEXT0_DPF_OFFSET_OFFSET, tmp);

    CI_PDUMP_COMMENT(contextHandle, "stop");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "DPF_NOT_AVAILABLE");
#endif // DPF_NOT_AVAILABLE
}

void HW_CI_Load_DPF(char *pMemory, const CI_MODULE_DPF *pDefective)
{
#ifndef DPF_NOT_AVAILABLE
    IMG_UINT32 tmp = 0;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pDefective);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DPF_ENABLE, 
        DPF_DETECT_ENABLE, pDefective->eDPFEnable&CI_DPF_DETECT_ENABLED);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DPF_ENABLE), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DPF_SENSITIVITY, 
        DPF_THRESHOLD, pDefective->ui8Threshold);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DPF_SENSITIVITY, 
        DPF_WEIGHT, pDefective->ui8Weight);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DPF_SENSITIVITY), tmp);

    /*tmp = 0;
    // size/cache policy are written in HW_CI_Update_DPF at trigger time
    WriteMem((IMG_UINT32*)pMemory, 
        HW_LOAD_OFF(DPF_MODIFICATIONS_MAP_SIZE_CACHE), tmp);*/

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DPF_MODIFICATIONS_THRESHOLD, 
        DPF_WRITE_MAP_POS_THR, pDefective->ui8PositiveThreshold);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DPF_MODIFICATIONS_THRESHOLD,
        DPF_WRITE_MAP_NEG_THR, pDefective->ui8NegativeThreshold);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DPF_MODIFICATIONS_THRESHOLD),
        tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "DPF_NOT_AVAILABLE");
#endif // DPF_NOT_AVAILABLE
}

void HW_CI_Update_DPF(char *pMemory, IMG_UINT32 ui32DPFOutputMapSize)
{
#ifndef DPF_NOT_AVAILABLE
    IMG_UINT32 tmp = 0;
    /*ReadMem((IMG_UINT32*)pMemory, 
        HW_LOAD_OFF(DPF_MODIFICATIONS_MAP_SIZE_CACHE));*/

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "");
    /* this should always be true because we round up when allocating 
     * with SYS_MemAlloc */
    IMG_ASSERT(ui32DPFOutputMapSize%SYSMEM_ALIGNMENT==0); 
    

#if FELIX_VERSION_MAJ == 1
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
        DPF_MODIFICATIONS_MAP_SIZE_CACHE, DPF_WRITE_CACHE_POLICY, 
        USE_DPF_WRITE_CACHE_POLICY);
#endif
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
        DPF_MODIFICATIONS_MAP_SIZE_CACHE, DPF_WRITE_MAP_SIZE, 
        ui32DPFOutputMapSize/SYSMEM_ALIGNMENT);
    WriteMem((IMG_UINT32*)pMemory, 
        HW_LOAD_OFF(DPF_MODIFICATIONS_MAP_SIZE_CACHE), tmp);
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "DPF_NOT_AVAILABLE");
#endif // DPF_NOT_AVAILABLE
}

void HW_CI_Load_R2Y(char *pMemory, const CI_MODULE_R2Y *pConv)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pConv);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RGB_TO_YCC_MATRIX_COEFFS, 
            RGB_TO_YCC_COEFF_COL_0, pConv->aCoeff[i][0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RGB_TO_YCC_MATRIX_COEFFS, 
            RGB_TO_YCC_COEFF_COL_1, pConv->aCoeff[i][1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            RGB_TO_YCC_MATRIX_COEFFS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            RGB_TO_YCC_MATRIX_COEFF_OFFSET, 
            RGB_TO_YCC_COEFF_COL_2, pConv->aCoeff[i][2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            RGB_TO_YCC_MATRIX_COEFF_OFFSET, 
            RGB_TO_YCC_OFFSET, pConv->aOffset[i]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                RGB_TO_YCC_MATRIX_COEFF_OFFSET, i), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
}

void HW_CI_Load_Y2R(char *pMemory, const CI_MODULE_Y2R *pConv)
{
    IMG_UINT32 tmp, i;
    // pConfig and pConv != NULL tested before

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pConv);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, YCC_TO_RGB_MATRIX_COEFFS, 
            YCC_TO_RGB_COEFF_COL_0, pConv->aCoeff[i][0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, YCC_TO_RGB_MATRIX_COEFFS, 
            YCC_TO_RGB_COEFF_COL_1, pConv->aCoeff[i][1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            YCC_TO_RGB_MATRIX_COEFFS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            YCC_TO_RGB_MATRIX_COEFF_OFFSET, 
            YCC_TO_RGB_COEFF_COL_2, pConv->aCoeff[i][2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            YCC_TO_RGB_MATRIX_COEFF_OFFSET, 
            YCC_TO_RGB_OFFSET, pConv->aOffset[i]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                YCC_TO_RGB_MATRIX_COEFF_OFFSET, i), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
}

void HW_CI_Load_LCA(char *pMemory, const struct CI_MODULE_LCA *pChroma)
{
#ifndef LCA_NOT_AVAILABLE
    IMG_UINT32 i, tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pChroma);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for ( i = 0 ; i < YCC_COEFF_NO ; i++ )
    {
        // red
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_DELTA_COEFF_RED, 
            LAT_CA_COEFF_X_RED, pChroma->aCoeffRed[i][0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_DELTA_COEFF_RED, 
            LAT_CA_COEFF_Y_RED, pChroma->aCoeffRed[i][1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            LAT_CA_DELTA_COEFF_RED, i), tmp);

        // blue
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_DELTA_COEFF_BLUE, 
            LAT_CA_COEFF_X_BLUE, pChroma->aCoeffBlue[i][0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_DELTA_COEFF_BLUE, 
            LAT_CA_COEFF_Y_BLUE, pChroma->aCoeffBlue[i][1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
            LAT_CA_DELTA_COEFF_BLUE, i), tmp);
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_RED, 
        LAT_CA_OFFSET_X_RED, pChroma->aOffsetRed[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_RED, 
        LAT_CA_OFFSET_Y_RED, pChroma->aOffsetRed[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LAT_CA_OFFSET_RED), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_BLUE,
        LAT_CA_OFFSET_X_BLUE, pChroma->aOffsetBlue[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_OFFSET_BLUE,
        LAT_CA_OFFSET_Y_BLUE, pChroma->aOffsetBlue[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LAT_CA_OFFSET_BLUE), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_NORMALIZATION, 
        LAT_CA_SHIFT_Y, pChroma->aShift[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_NORMALIZATION, 
        LAT_CA_DEC_Y, pChroma->aDec[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_NORMALIZATION, 
        LAT_CA_SHIFT_X, pChroma->aShift[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, LAT_CA_NORMALIZATION, 
        LAT_CA_DEC_X, pChroma->aDec[0]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(LAT_CA_NORMALIZATION), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "LCA_NOT_AVAILABLE");
#endif // LCA_NOT_AVAILABLE
}

void HW_CI_Load_CCM(char *pMemory, const CI_MODULE_CCM *pCorr)
{
#ifndef CCM_NOT_AVAILABLE
    IMG_UINT32 tmp, i;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pCorr);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_COEFFS_A, 
            CC_COEFF_0, pCorr->aCoeff[i][0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_COEFFS_A, 
            CC_COEFF_1, pCorr->aCoeff[i][1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, CC_COEFFS_A, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_COEFFS_B, 
            CC_COEFF_2, pCorr->aCoeff[i][2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_COEFFS_B, 
            CC_COEFF_3, pCorr->aCoeff[i][3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, CC_COEFFS_B, i), tmp);
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_OFFSETS_1_0, 
        CC_OFFSET_0, pCorr->aOffset[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_OFFSETS_1_0, 
        CC_OFFSET_1, pCorr->aOffset[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(CC_OFFSETS_1_0), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, CC_OFFSETS_2, 
        CC_OFFSET_2, pCorr->aOffset[2]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(CC_OFFSETS_2), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "CCM_NOT_AVAILABLE");
#endif // CCM_NOT_AVAILABLE
}

void HW_CI_Load_MIE(char *pMemory, const CI_MODULE_MIE *pMie)
{
#ifndef MIE_NOT_AVAILABLE
    IMG_UINT32 tmp, i;
    IMG_BOOL8 b2_4HW = IMG_FALSE;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pMie);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    /*
     * note to customer: if your HW is 2.4 then this could be done directly
     * as const IMG_BOOL8 b2_4HW = IMG_TRUE
     *
     * If your HW is not 2.4 then the same with IMG_FALSE
     */
    if (2 == g_psCIDriver->sHWInfo.rev_ui8Major
        && 4 == g_psCIDriver->sHWInfo.rev_ui8Minor)
    {
        b2_4HW = IMG_TRUE;
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_PARAMS_0, 
        MIE_MC_ON, pMie->bMemoryColour);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_PARAMS_0, 
        MIE_VIB_ON, pMie->bVibrancy);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_PARAMS_0, 
        MIE_BLACK_LEVEL, pMie->ui16BlackLevel);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MIE_VIB_PARAMS_0), tmp);

    if (pMie->bVibrancy)
    {
        for ( i = 0 ; i+1 < MIE_VIBRANCY_N ; i+=2 ) // 2 elements per registers
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_FUN, 
                MIE_VIB_SATMULT_0, pMie->aVibrancySatMul[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_FUN, 
                MIE_VIB_SATMULT_1, pMie->aVibrancySatMul[i+1]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_VIB_FUN, i/2), tmp);
        }
    }

    if (pMie->bMemoryColour)
    {
        for ( i = 0 ; i < MIE_NUM_MEMCOLOURS ; i++ )
        {
            tmp = 0;
            if (b2_4HW)
            {
                REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT,
                    MIE_MC_PARAMS_Y, MIE_MC_YOFF, pMie->aYOffset[i]);
                REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT,
                    MIE_MC_PARAMS_Y, MIE_MC_YSCALE, pMie->aYScale[i]);
            }
            else
            {
                REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                    MIE_MC_PARAMS_Y, MIE_MC_YOFF, pMie->aYOffset[i]);
                REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                    MIE_MC_PARAMS_Y, MIE_MC_YSCALE, pMie->aYScale[i]);
            }
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_MC_PARAMS_Y, i),
                tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_MC_PARAMS_C,
                MIE_MC_COFF, pMie->aCOffset[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_MC_PARAMS_C,
                MIE_MC_CSCALE, pMie->aCScale[i]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_MC_PARAMS_C, i), 
                tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_MC_ROT_PARAMS, 
                MIE_MC_ROT00, pMie->aRot[2*i + 0]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_MC_ROT_PARAMS, 
                MIE_MC_ROT01, pMie->aRot[2*i + 1]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_MC_ROT_PARAMS, i), 
                tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_0,
                MIE_MC_GAUSS_MINY, pMie->aGaussMinY[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_0,
                MIE_MC_GAUSS_YSCALE, pMie->aGaussScaleY[i]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                    MIE_MC_GAUSS_PARAMS_0, i), tmp);

            tmp = 0;
            if (b2_4HW)
            {
                REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT,
                    MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CB0,
                    pMie->aGaussCB[i]);
                REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT,
                    MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CR0,
                    pMie->aGaussCR[i]);
            }
            else
            {
                REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                    MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CB0,
                    pMie->aGaussCB[i]);
                REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                    MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CR0,
                    pMie->aGaussCR[i]);
            }
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                    MIE_MC_GAUSS_PARAMS_1, i), tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_2, 
                MIE_MC_GAUSS_R00, pMie->aGaussRot[2*i + 0]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_2, 
                MIE_MC_GAUSS_R01, pMie->aGaussRot[2*i + 1]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                    MIE_MC_GAUSS_PARAMS_2, i), tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                MIE_MC_GAUSS_PARAMS_3, MIE_MC_GAUSS_KB, pMie->aGaussKB[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                MIE_MC_GAUSS_PARAMS_3, MIE_MC_GAUSS_KR, pMie->aGaussKR[i]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                    MIE_MC_GAUSS_PARAMS_3, i), tmp);

        }

        for ( i = 0 ; i < MIE_GAUSS_SC_N/2 ; i++ )
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_4, 
                MIE_MC_GAUSS_S0, pMie->aGaussScale[i*2]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_4, 
                MIE_MC_GAUSS_S1, pMie->aGaussScale[i*2+1]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                    MIE_MC_GAUSS_PARAMS_4, i), tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                MIE_MC_GAUSS_PARAMS_5, 
                MIE_MC_GAUSS_G0, pMie->aGaussGain[i*2]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                MIE_MC_GAUSS_PARAMS_5, 
                MIE_MC_GAUSS_G1, pMie->aGaussGain[i*2+1]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                    MIE_MC_GAUSS_PARAMS_5, i), tmp);
        }
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "MIE_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_TNM(char *pMemory, const struct CI_MODULE_TNM *pMap)
{
#ifndef TNM_NOT_AVAILABLE
    IMG_UINT32 i, tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pMap);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_BYPASS, 
        TNM_BYPASS, pMap->bBypassTNM != 0 ? 1 : 0);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_BYPASS), tmp);

    if (!pMap->bBypassTNM)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "curve");

        i = 0;
        while( i+1 < TNM_CURVE_NPOINTS-1 )
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
                TNM_CURVE_POINT_0, pMap->aCurve[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 
                TNM_CURVE_POINT_1, pMap->aCurve[i+1]);
            WriteMem((IMG_UINT32*)pMemory, 
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 
                    (i+1)/2), tmp);
            i+=2;
        }
        // last one is only 1 element in the curve
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 
            TNM_CURVE_POINT_0, pMap->aCurve[TNM_CURVE_NPOINTS-1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE, 
                (TNM_CURVE_NPOINTS-1)/2), tmp);

        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "curve done");

        //
        // histogram
        //
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_HIST_FLATTEN_0, 
            TNM_HIST_FLATTEN_MIN, pMap->histFlattenMin);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_HIST_FLATTEN_0, 
            TNM_HIST_FLATTEN_THRES, pMap->histFlattenThreshold[0]);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_HIST_FLATTEN_0), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_HIST_FLATTEN_1, 
            TNM_HIST_FLATTEN_RECIP, pMap->histFlattenThreshold[1]);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_HIST_FLATTEN_1), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_HIST_NORM, 
            TNM_HIST_NORM_FACTOR_LAST, pMap->histNormLastFactor);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_HIST_NORM, 
            TNM_HIST_NORM_FACTOR, pMap->histNormFactor);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_HIST_NORM), tmp);

        //
        //
        //
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_CHR_0, 
            TNM_CHR_SAT_SCALE, pMap->chromaSaturationScale);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_CHR_0, 
            TNM_CHR_CONF_SCALE, pMap->chromaConfigurationScale);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_CHR_0), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_CHR_1, 
            TNM_CHR_IO_SCALE, pMap->chromaIOScale);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_CHR_1), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_LOCAL_COL, 
            TNM_LOCAL_COL_WIDTH, pMap->localColWidth[0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_LOCAL_COL, 
            TNM_LOCAL_COL_WIDTH_RECIP, pMap->localColWidth[1]);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_LOCAL_COL), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_CTX_LOCAL_COL,
            TNM_CTX_LOCAL_COL_IDX, pMap->columnsIdx);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_CTX_LOCAL_COL, 
            TNM_CTX_LOCAL_COLS, pMap->localColumns);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_CTX_LOCAL_COL), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_WEIGHTS, 
            TNM_LOCAL_WEIGHT, pMap->localWeights);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_WEIGHTS, 
            TNM_CURVE_UPDATE_WEIGHT, pMap->updateWeights);
        WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_WEIGHTS), tmp);

    } // bypass

    //
    // Luma input and output
    //
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_INPUT_LUMA, 
        TNM_INPUT_LUMA_OFFSET, pMap->inputLumaOffset);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_INPUT_LUMA, 
        TNM_INPUT_LUMA_SCALE, pMap->inputLumaScale);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_INPUT_LUMA), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_OUTPUT_LUMA, 
        TNM_OUTPUT_LUMA_OFFSET, pMap->outputLumaOffset);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_OUTPUT_LUMA, 
        TNM_OUTPUT_LUMA_SCALE, pMap->outputLumaScale);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_OUTPUT_LUMA), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle,
        "TNM_NOT_AVAILABLE");
#endif // TNM_NOT_AVAILABLE
}

static void HW_CI_Load_DSC(char *pMemory, 
    const struct CI_MODULE_SCALER *pScaler)
{
    IMG_UINT32 tmp, i;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    /* written in advance because the vertical pitch may be used if bypass is
     * enabled for decimation purposes */
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DISP_SCAL_V_PITCH), 
        pScaler->aPitch[1]);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DISP_SCAL_V_SETUP,
        DISP_SCAL_V_OFFSET, pScaler->aOffset[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DISP_SCAL_V_SETUP, 
        DISP_SCAL_OUTPUT_ROWS, pScaler->aOutputSize[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DISP_SCAL_V_SETUP), tmp);

    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DISP_SCAL_H_PITCH), 
        pScaler->aPitch[0]);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DISP_SCAL_H_SETUP,
        DISP_SCAL_BYPASS, pScaler->bBypassScaler);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DISP_SCAL_H_SETUP,
        DISP_SCAL_H_OFFSET, pScaler->aOffset[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DISP_SCAL_H_SETUP, 
        DISP_SCAL_OUTPUT_COLUMNS, pScaler->aOutputSize[0]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DISP_SCAL_H_SETUP), tmp);

    if (pScaler->bBypassScaler)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
            "bypass - done");
        return;
    }

    for ( i = 0 ; i < (SCALER_PHASES/2) ; i++ )
    {
        /*
         * v chroma taps
         */
        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_CHROMA_TAPS_0_TO_1, DISP_SCAL_V_CHROMA_TAP_0, 
            pScaler->VChroma[i*DSC_V_CHROMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_CHROMA_TAPS_0_TO_1, DISP_SCAL_V_CHROMA_TAP_1, 
            pScaler->VChroma[i*DSC_V_CHROMA_TAPS + 1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                DISP_SCAL_V_CHROMA_TAPS_0_TO_1, i), tmp);

        /*
         * v luma taps
         */
        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_LUMA_TAPS_0_TO_3, DISP_SCAL_V_LUMA_TAP_0, 
            pScaler->VLuma[i*DSC_V_LUMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_LUMA_TAPS_0_TO_3, DISP_SCAL_V_LUMA_TAP_1, 
            pScaler->VLuma[i*DSC_V_LUMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_LUMA_TAPS_0_TO_3, DISP_SCAL_V_LUMA_TAP_2, 
            pScaler->VLuma[i*DSC_V_LUMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_V_LUMA_TAPS_0_TO_3, DISP_SCAL_V_LUMA_TAP_3, 
            pScaler->VLuma[i*DSC_V_LUMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                DISP_SCAL_V_LUMA_TAPS_0_TO_3, i), tmp);

        /*
         * h chroma taps
         */
        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_CHROMA_TAPS_0_TO_3, DISP_SCAL_H_CHROMA_TAP_0, 
            pScaler->HChroma[i*DSC_H_CHROMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_CHROMA_TAPS_0_TO_3, DISP_SCAL_H_CHROMA_TAP_1,
            pScaler->HChroma[i*DSC_H_CHROMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            DISP_SCAL_H_CHROMA_TAPS_0_TO_3, DISP_SCAL_H_CHROMA_TAP_2, 
            pScaler->HChroma[i*DSC_H_CHROMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_CHROMA_TAPS_0_TO_3, DISP_SCAL_H_CHROMA_TAP_3, 
            pScaler->HChroma[i*DSC_H_CHROMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                DISP_SCAL_H_CHROMA_TAPS_0_TO_3, i), tmp);

        /*
         * h luma taps
         */

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_0_TO_3, DISP_SCAL_H_LUMA_TAP_0, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_0_TO_3, DISP_SCAL_H_LUMA_TAP_1,
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            DISP_SCAL_H_LUMA_TAPS_0_TO_3, DISP_SCAL_H_LUMA_TAP_2,
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_0_TO_3, DISP_SCAL_H_LUMA_TAP_3, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                DISP_SCAL_H_LUMA_TAPS_0_TO_3, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_4_TO_7, DISP_SCAL_H_LUMA_TAP_4, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 4]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_4_TO_7, DISP_SCAL_H_LUMA_TAP_5, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 5]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            DISP_SCAL_H_LUMA_TAPS_4_TO_7, DISP_SCAL_H_LUMA_TAP_6, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 6]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            DISP_SCAL_H_LUMA_TAPS_4_TO_7, DISP_SCAL_H_LUMA_TAP_7, 
            pScaler->HLuma[i*DSC_H_LUMA_TAPS + 7]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                DISP_SCAL_H_LUMA_TAPS_4_TO_7, i), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
}

static void HW_CI_Load_ESC(char *pMemory, 
    const struct CI_MODULE_SCALER *pScaler)
{
    IMG_UINT32 tmp, i;

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    /* written in advance because the vertical pitch may be used even if 
     * bypass is enabled for decimation purposes */
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_SCAL_V_PITCH), 
        pScaler->aPitch[1]);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_V_SETUP, 
        ENC_SCAL_422_NOT_420, 
        pScaler->eSubsampling == EncOut420_scaler ? 0 : 1);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_V_SETUP, 
        ENC_SCAL_V_OFFSET, pScaler->aOffset[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_V_SETUP, 
        ENC_SCAL_OUTPUT_ROWS, pScaler->aOutputSize[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_SCAL_V_SETUP), tmp);

    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_SCAL_H_PITCH),
        pScaler->aPitch[0]);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_H_SETUP, 
        ENC_SCAL_BYPASS, pScaler->bBypassScaler);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_H_SETUP, 
        ENC_SCAL_H_OFFSET, pScaler->aOffset[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_H_SETUP, 
        ENC_SCAL_OUTPUT_COLUMNS, pScaler->aOutputSize[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_H_SETUP,
        ENC_SCAL_CHROMA_MODE, pScaler->bChromaInter);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_SCAL_H_SETUP), tmp);

    if (pScaler->bBypassScaler)
    {
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
            "bypass - done");
        return;
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_422_TO_420_CTRL, 
        ENC_422_TO_420_ENABLE, 
        pScaler->eSubsampling == EncOut420_vss ? 1 : 0);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_422_TO_420_CTRL), tmp);

    for ( i = 0 ; i < (SCALER_PHASES/2) ; i++ )
    {
        /*
         * v chroma taps
         */

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_CHROMA_TAPS_0_TO_3, ENC_SCAL_V_CHROMA_TAP_0, 
            pScaler->VChroma[i*ESC_V_CHROMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_CHROMA_TAPS_0_TO_3, ENC_SCAL_V_CHROMA_TAP_1, 
            pScaler->VChroma[i*ESC_V_CHROMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_CHROMA_TAPS_0_TO_3, ENC_SCAL_V_CHROMA_TAP_2, 
            pScaler->VChroma[i*ESC_V_CHROMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            ENC_SCAL_V_CHROMA_TAPS_0_TO_3, ENC_SCAL_V_CHROMA_TAP_3, 
            pScaler->VChroma[i*ESC_V_CHROMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_V_CHROMA_TAPS_0_TO_3, i), tmp);

        /*
         * h chroma taps
         */

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_0_TO_3, ENC_SCAL_H_CHROMA_TAP_0, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_0_TO_3, ENC_SCAL_H_CHROMA_TAP_1, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_0_TO_3, ENC_SCAL_H_CHROMA_TAP_2, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_0_TO_3, ENC_SCAL_H_CHROMA_TAP_3, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_H_CHROMA_TAPS_0_TO_3, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_4_TO_7, ENC_SCAL_H_CHROMA_TAP_4, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 4]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_4_TO_7, ENC_SCAL_H_CHROMA_TAP_5, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 5]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_4_TO_7, ENC_SCAL_H_CHROMA_TAP_6, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 6]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_CHROMA_TAPS_4_TO_7, ENC_SCAL_H_CHROMA_TAP_7, 
            pScaler->HChroma[i*ESC_H_CHROMA_TAPS + 7]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                ENC_SCAL_H_CHROMA_TAPS_4_TO_7, i), tmp);

        /*
         * v luma taps
         */

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_0_TO_3, ENC_SCAL_V_LUMA_TAP_0, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_0_TO_3, ENC_SCAL_V_LUMA_TAP_1, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_0_TO_3, ENC_SCAL_V_LUMA_TAP_2, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_0_TO_3, ENC_SCAL_V_LUMA_TAP_3, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_V_LUMA_TAPS_0_TO_3, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_4_TO_7, ENC_SCAL_V_LUMA_TAP_4, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 4]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_4_TO_7, ENC_SCAL_V_LUMA_TAP_5,
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 5]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_4_TO_7, ENC_SCAL_V_LUMA_TAP_6, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 6]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_V_LUMA_TAPS_4_TO_7, ENC_SCAL_V_LUMA_TAP_7, 
            pScaler->VLuma[i*ESC_V_LUMA_TAPS + 7]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_V_LUMA_TAPS_4_TO_7, i), tmp);

        /*
         * h luma taps
         */

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_0_TO_3, ENC_SCAL_H_LUMA_TAP_0, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_0_TO_3, ENC_SCAL_H_LUMA_TAP_1, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_0_TO_3, ENC_SCAL_H_LUMA_TAP_2, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 2]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_0_TO_3, ENC_SCAL_H_LUMA_TAP_3, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 3]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_H_LUMA_TAPS_0_TO_3, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_4_TO_7, ENC_SCAL_H_LUMA_TAP_4, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 4]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_4_TO_7, ENC_SCAL_H_LUMA_TAP_5, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 5]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_4_TO_7, ENC_SCAL_H_LUMA_TAP_6, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 6]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_4_TO_7, ENC_SCAL_H_LUMA_TAP_7, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 7]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_H_LUMA_TAPS_4_TO_7, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_8_TO_11, ENC_SCAL_H_LUMA_TAP_8, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 8]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_8_TO_11, ENC_SCAL_H_LUMA_TAP_9, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 9]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_8_TO_11, ENC_SCAL_H_LUMA_TAP_10, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 10]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_8_TO_11, ENC_SCAL_H_LUMA_TAP_11, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 11]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_H_LUMA_TAPS_8_TO_11, i), tmp);

        tmp=0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_12_TO_15, ENC_SCAL_H_LUMA_TAP_12, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 12]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_12_TO_15, ENC_SCAL_H_LUMA_TAP_13, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 13]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_12_TO_15, ENC_SCAL_H_LUMA_TAP_14, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 14]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            ENC_SCAL_H_LUMA_TAPS_12_TO_15, ENC_SCAL_H_LUMA_TAP_15, 
            pScaler->HLuma[i*ESC_H_LUMA_TAPS + 15]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                ENC_SCAL_H_LUMA_TAPS_12_TO_15, i), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
}

void HW_CI_Load_Scaler(char *pMemory, const struct CI_MODULE_SCALER *pScaler)
{
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pScaler);

    if (pScaler->bIsDisplay)
        HW_CI_Load_DSC(pMemory, pScaler);
    else
        HW_CI_Load_ESC(pMemory, pScaler);
}

void HW_CI_Load_DNS(char *pMemory, const struct CI_MODULE_DNS *pDenoiser)
{
#ifndef DNS_NOT_AVAILABLE
    IMG_UINT32 i, tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pDenoiser);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    for(i=0 ; i < (DNS_N_LUT0/2) ; i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            PIX_THRESH_LUT_SEC0_POINTS, PIX_THRESH_LUT_POINTS_0TO7_0, 
            pDenoiser->aPixThresLUT[2*i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            PIX_THRESH_LUT_SEC0_POINTS, PIX_THRESH_LUT_POINTS_0TO7_1, 
            pDenoiser->aPixThresLUT[2*i + 1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                PIX_THRESH_LUT_SEC0_POINTS, i), tmp);
    }
    for(i=0 ; i < (DNS_N_LUT1/2) ; i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            PIX_THRESH_LUT_SEC1_POINTS, PIX_THRESH_LUT_POINTS_8TO13_0,
            pDenoiser->aPixThresLUT[2*i + DNS_N_LUT0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC1_POINTS, PIX_THRESH_LUT_POINTS_8TO13_1, 
            pDenoiser->aPixThresLUT[2*i + DNS_N_LUT0 + 1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                PIX_THRESH_LUT_SEC1_POINTS, i), tmp);
    }
    for(i=0 ; i < (DNS_N_LUT2/2) ; i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            PIX_THRESH_LUT_SEC2_POINTS, PIX_THRESH_LUT_POINTS_14TO19_0,
            pDenoiser->aPixThresLUT[2*i + DNS_N_LUT0 + DNS_N_LUT1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
            PIX_THRESH_LUT_SEC2_POINTS, PIX_THRESH_LUT_POINTS_14TO19_1,
            pDenoiser->aPixThresLUT[2*i + DNS_N_LUT0 + DNS_N_LUT1 + 1]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                PIX_THRESH_LUT_SEC2_POINTS, i), tmp);
    }
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
        PIX_THRESH_LUT_SEC3_POINTS, PIX_THRESH_LUT_POINT_20,
        pDenoiser->aPixThresLUT[DNS_N_LUT - 1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(PIX_THRESH_LUT_SEC3_POINTS),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DENOISE_CONTROL,
        DENOISE_COMBINE_1_2, pDenoiser->bCombineChannels);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DENOISE_CONTROL,
        LOG2_GREYSC_PIXTHRESH_MULT, pDenoiser->i8Log2PixThresh)
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DENOISE_CONTROL), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "DNS_NOT_AVAILABLE");
#endif // DNS_NOT_AVAILABLE
}

void HW_CI_Load_MGM(char *pMemory, const CI_MODULE_MGM *pGamut)
{
#ifndef MGM_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamut);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_CLIP_IN, 
        MGM_CLIP_MIN, pGamut->i16ClipMin);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_CLIP_IN, 
        MGM_CLIP_MAX, pGamut->ui16ClipMax);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_CLIP_IN), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_CORE_IN_OUT, 
        MGM_SRC_NORM, pGamut->ui16SrcNorm);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_CORE_IN_OUT), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_SLOPE_0_1, 
        MGM_SLOPE_0, pGamut->aSlope[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_SLOPE_0_1, 
        MGM_SLOPE_1, pGamut->aSlope[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_SLOPE_0_1), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_SLOPE_2_R, 
        MGM_SLOPE_2, pGamut->aSlope[2]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_SLOPE_2_R), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_0_3, 
        MGM_COEFF_0, pGamut->aCoeff[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_0_3, 
        MGM_COEFF_1, pGamut->aCoeff[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_0_3, 
        MGM_COEFF_2, pGamut->aCoeff[2]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_0_3, 
        MGM_COEFF_3, pGamut->aCoeff[3]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_COEFFS_0_3), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_4_5,
        MGM_COEFF_4, pGamut->aCoeff[4]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MGM_COEFFS_4_5,
        MGM_COEFF_5, pGamut->aCoeff[5]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(MGM_COEFFS_4_5), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "MGM_NOT_AVAILABLE");
#endif //MGM_NOT_AVAILABLE
}

void HW_CI_Load_DGM(char *pMemory, const CI_MODULE_DGM *pGamut)
{
#ifndef DGM_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamut);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_CLIP_IN,
        DGM_CLIP_MIN, pGamut->i16ClipMin);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_CLIP_IN, 
        DGM_CLIP_MAX, pGamut->ui16ClipMax);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_CLIP_IN), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_CORE_IN_OUT, 
        DGM_SRC_NORM, pGamut->ui16SrcNorm);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_CORE_IN_OUT), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_SLOPE_0_1,
        DGM_SLOPE_0, pGamut->aSlope[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_SLOPE_0_1,
        DGM_SLOPE_1, pGamut->aSlope[1]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_SLOPE_0_1), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_SLOPE_2_R, 
        DGM_SLOPE_2, pGamut->aSlope[2]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_SLOPE_2_R), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_0_3, 
        DGM_COEFF_0, pGamut->aCoeff[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_0_3,
        DGM_COEFF_1, pGamut->aCoeff[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_0_3,
        DGM_COEFF_2, pGamut->aCoeff[2]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_0_3,
        DGM_COEFF_3, pGamut->aCoeff[3]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_COEFFS_0_3), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_4_5,
        DGM_COEFF_4, pGamut->aCoeff[4]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, DGM_COEFFS_4_5,
        DGM_COEFF_5, pGamut->aCoeff[5]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(DGM_COEFFS_4_5), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, 
        "DGM_NOT_AVAILABLE");
#endif //DGM_NOT_AVAILABLE
}

void HW_CI_Reg_GMA(const CI_MODULE_GMA_LUT *pGamma)
{
#ifndef GMA_NOT_AVAILABLE
    IMG_UINT32 i;
    IMG_UINT32 val;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pGamma);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, "start");

    for ( i = 0 ; i < GMA_N_POINTS/2 ; i++ )
    {
        val = 0;
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_RED_POINT, 
            GMA_RED_EVEN, pGamma->aRedPoints[2*i]);
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_RED_POINT, 
            GMA_RED_ODD, pGamma->aRedPoints[2*i+1]);
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
            REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_RED_POINT, i), val);

        val = 0;
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_GREEN_POINT, 
            GMA_GREEN_EVEN, pGamma->aGreenPoints[2*i]);
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_GREEN_POINT, 
            GMA_GREEN_ODD, pGamma->aGreenPoints[2*i+1]);
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
            REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_GREEN_POINT, i), val);

        val = 0;
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_BLUE_POINT, 
            GMA_BLUE_EVEN, pGamma->aBluePoints[2*i]);
        REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_BLUE_POINT, 
            GMA_BLUE_ODD, pGamma->aBluePoints[2*i+1]);
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
            REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_BLUE_POINT, i), val);
    }
#if GMA_N_POINTS%2 == 1
    // write the last value
    val = 0;
    REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_RED_POINT, 
        GMA_RED_EVEN, pGamma->aRedPoints[2*i]);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
        REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_RED_POINT, i), val);

    val = 0;
    REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_GREEN_POINT, 
        GMA_GREEN_EVEN, pGamma->aGreenPoints[2*i]);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
        REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_GREEN_POINT, i), val);

    val = 0;
    REGIO_WRITE_FIELD(val, FELIX_GAMMA_LUT, GMA_BLUE_POINT, 
        GMA_BLUE_EVEN, pGamma->aBluePoints[2*i]);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGammaLut, 
        REGIO_TABLE_OFF(FELIX_GAMMA_LUT, GMA_BLUE_POINT, i), val);
#endif

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGammaLut, "done");
#else
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, 
        "GMA_NOT_AVAILABLE");
#endif // GMA_NOT_AVAILABLE
}

void HW_CI_Load_GMA(char *pMemory, const CI_MODULE_GMA *pGamma)
{
#ifndef GMA_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamma);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, GMA_BYPASS, 
        GMA_BYPASS, pGamma->bBypass);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(GMA_BYPASS), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, 
        "GMA_NOT_AVAILABLE");
#endif // GMA_NO
}

void HW_CI_Load_SHA(char *pMemory, const CI_MODULE_SHA *pSharp)
{
#ifndef SHA_NOT_AVAILABLE
    IMG_UINT32 tmp;
    IMG_BOOL8 b2_4HW = IMG_FALSE;

    IMG_ASSERT(g_psCIDriver);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    /*
    * note to customer: if your HW is 2.4 then this could be done directly
    * as const IMG_BOOL8 b2_4HW = IMG_TRUE
    *
    * If your HW is not 2.4 then the same with IMG_FALSE
    */
    if (2 == g_psCIDriver->sHWInfo.rev_ui8Major
        && 4 == g_psCIDriver->sHWInfo.rev_ui8Minor)
    {
        b2_4HW = IMG_TRUE;
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_0,
        SHA_THRESH, pSharp->ui8Threshold);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_0,
        SHA_STRENGTH, pSharp->ui8Strength);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_0,
        SHA_DETAIL, pSharp->ui8Detail);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_0,
        SHA_DENOISE_BYPASS, pSharp->bDenoiseBypass);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(SHA_PARAMS_0), tmp);

    tmp = 0;
    if (b2_4HW)
    {
        REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT, SHA_PARAMS_1,
            SHA_GWEIGHT_0, pSharp->aGainWeight[0]);
        REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT, SHA_PARAMS_1,
            SHA_GWEIGHT_1, pSharp->aGainWeight[1]);
        REGIO_WRITE_FIELD(tmp, FELIX_2_4_0_LOAD_STRUCT, SHA_PARAMS_1,
            SHA_GWEIGHT_2, pSharp->aGainWeight[2]);
    }
    else
    {
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
            SHA_GWEIGHT_0, pSharp->aGainWeight[0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
            SHA_GWEIGHT_1, pSharp->aGainWeight[1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
            SHA_GWEIGHT_2, pSharp->aGainWeight[2]);
    }
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(SHA_PARAMS_1), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_ELW_SCALEOFF,
        SHA_ELW_SCALE, pSharp->ui8ELWScale);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_ELW_SCALEOFF,
        SHA_ELW_OFFS, pSharp->i8ELWOffset);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(SHA_ELW_SCALEOFF), tmp);

    if (!pSharp->bDenoiseBypass)
    {
        IMG_UINT32 i;

        tmp = 0;
        for ( i = 0 ;
            i < FELIX_CONTEXT0_SHA_DN_EDGE_SIMIL_COMP_PTS_0_SHA_DN_EDGE_SIMIL_COMP_PTS_0_TO_3_NO_REPS ;
            i++ )
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                SHA_DN_EDGE_SIMIL_COMP_PTS_0, 
                SHA_DN_EDGE_SIMIL_COMP_PTS_0_TO_3, i, pSharp->aDNSimil[i]);
        }
        WriteMem((IMG_UINT32*)pMemory, 
            HW_LOAD_OFF(SHA_DN_EDGE_SIMIL_COMP_PTS_0), tmp);

        tmp = 0;
        for ( i = 0 ;
            i < FELIX_CONTEXT0_SHA_DN_EDGE_SIMIL_COMP_PTS_1_SHA_DN_EDGE_SIMIL_COMP_PTS_4_TO_6_NO_REPS ;
            i++ )
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                SHA_DN_EDGE_SIMIL_COMP_PTS_1, 
                SHA_DN_EDGE_SIMIL_COMP_PTS_4_TO_6, i, pSharp->aDNSimil[4+i]);
        }
        WriteMem((IMG_UINT32*)pMemory, 
            HW_LOAD_OFF(SHA_DN_EDGE_SIMIL_COMP_PTS_1), tmp);

        tmp = 0;
        for ( i = 0 ;
            i < FELIX_CONTEXT0_SHA_DN_EDGE_AVOID_COMP_PTS_0_SHA_DN_EDGE_AVOID_COMP_PTS_0_TO_3_NO_REPS ;
            i++ )
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                SHA_DN_EDGE_AVOID_COMP_PTS_0, 
                SHA_DN_EDGE_AVOID_COMP_PTS_0_TO_3, i, pSharp->aDNAvoid[i]);
        }
        WriteMem((IMG_UINT32*)pMemory, 
            HW_LOAD_OFF(SHA_DN_EDGE_AVOID_COMP_PTS_0), tmp);

        tmp = 0;
        for ( i = 0 ;
            i < FELIX_CONTEXT0_SHA_DN_EDGE_AVOID_COMP_PTS_1_SHA_DN_EDGE_AVOID_COMP_PTS_4_TO_6_NO_REPS ;
            i++ )
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, 
                SHA_DN_EDGE_AVOID_COMP_PTS_1, 
                SHA_DN_EDGE_AVOID_COMP_PTS_4_TO_6, i, pSharp->aDNAvoid[4+i]);
        }
        WriteMem((IMG_UINT32*)pMemory, 
            HW_LOAD_OFF(SHA_DN_EDGE_AVOID_COMP_PTS_1), tmp);
    }

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore, 
        "SHA_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_EXS(char *pMemory, const CI_MODULE_EXS *pExposure)
{
#ifndef EXS_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    // used only if CI_SAVE_EXS_GLOBAL
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, EXS_GRID_START_COORDS,
        EXS_GRID_COL_START, pExposure->ui16Left);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, EXS_GRID_START_COORDS,
        EXS_GRID_ROW_START, pExposure->ui16Top);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(EXS_GRID_START_COORDS), tmp);

    // used only if CI_SAVE_EXS_GLOBAL
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, EXS_GRID_TILE,
        EXS_GRID_TILE_WIDTH, pExposure->ui16Width);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, EXS_GRID_TILE,
        EXS_GRID_TILE_HEIGHT, pExposure->ui16Height);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(EXS_GRID_TILE), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, EXS_PIXEL_MAX,
        EXS_PIXEL_MAX, pExposure->ui16PixelMax);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(EXS_PIXEL_MAX), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "EXS_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_FOS(char *pMemory, const CI_MODULE_FOS *pFocus)
{
#ifndef FOS_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_GRID_START_COORDS,
        FOS_GRID_START_COL, pFocus->ui16Left);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_GRID_START_COORDS,
        FOS_GRID_START_ROW, pFocus->ui16Top);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FOS_GRID_START_COORDS), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_GRID_TILE,
        FOS_GRID_TILE_WIDTH, pFocus->ui16Width);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_GRID_TILE,
        FOS_GRID_TILE_HEIGHT, pFocus->ui16Height);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FOS_GRID_TILE), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_ROI_COORDS_START,
        FOS_ROI_START_COL, pFocus->ui16ROILeft);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_ROI_COORDS_START,
        FOS_ROI_START_ROW, pFocus->ui16ROITop);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FOS_ROI_COORDS_START), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_ROI_COORDS_END,
        FOS_ROI_END_COL, pFocus->ui16ROIRight);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FOS_ROI_COORDS_END,
        FOS_ROI_END_ROW, pFocus->ui16ROIBottom);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FOS_ROI_COORDS_END), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "FOS_NOT_AVAILABLE");
#endif // FOS_NOT_AVAILABLE
}

void HW_CI_Load_FLD(char *pMemory, const CI_MODULE_FLD *pFlicker)
{
#ifndef FLD_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_FRAC_STEP,
        FLD_FRAC_STEP_50, pFlicker->ui16FracStep50);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_FRAC_STEP,
        FLD_FRAC_STEP_60, pFlicker->ui16FracStep60);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FLD_FRAC_STEP), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_TH,
        FLD_COEF_DIFF_TH, pFlicker->ui16CoefDiff);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_TH,
        FLD_NF_TH, pFlicker->ui16NFThreshold);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FLD_TH), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_SCENE_CHANGE_TH,
        FLD_SCENE_CHANGE_TH, pFlicker->ui32SceneChange);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FLD_SCENE_CHANGE_TH), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_DFT,
        FLD_RSHIFT, pFlicker->ui8RShift);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_DFT,
        FLD_MIN_PN, pFlicker->ui8MinPN);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_DFT,
        FLD_PN, pFlicker->ui8PN);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FLD_DFT), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, FLD_RESET,
        FLD_RESET, pFlicker->bReset);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(FLD_RESET), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "FLD_NOT_AVAILABLE");
#endif // FLD_NOT_AVAILABLE
}

void HW_CI_Load_HIS(char *pMemory, const CI_MODULE_HIS *pHistograms)
{
#ifndef HIS_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_INPUT_RANGE,
        HIS_INPUT_OFFSET, pHistograms->ui16InputOffset);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_INPUT_RANGE,
        HIS_INPUT_SCALE, pHistograms->ui16InputScale);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(HIS_INPUT_RANGE), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_REGION_GRID_START_COORDS,
        HIS_REGION_GRID_COL_START, pHistograms->ui16Left);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_REGION_GRID_START_COORDS,
        HIS_REGION_GRID_ROW_START, pHistograms->ui16Top);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(HIS_REGION_GRID_START_COORDS),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_REGION_GRID_TILE,
        HIS_REGION_GRID_TILE_WIDTH, pHistograms->ui16Width);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, HIS_REGION_GRID_TILE,
        HIS_REGION_GRID_TILE_HEIGHT, pHistograms->ui16Height);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(HIS_REGION_GRID_TILE), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "HIS_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_WBS(char *pMemory, const CI_MODULE_WBS *pWhiteBalance)
{
#ifndef WBS_NOT_AVAILABLE
    IMG_UINT32 tmp, i;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    // configuration of each region of interest
    for ( i = 0 ; i < WBS_NUM_ROI ; i++ )
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_ROI_START_COORDS,
            WBS_ROI_COL_START, pWhiteBalance->aRoiLeft[i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_ROI_START_COORDS,
            WBS_ROI_ROW_START, pWhiteBalance->aRoiTop[i]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, 
                WBS_ROI_START_COORDS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_ROI_END_COORDS,
            WBS_ROI_COL_END, pWhiteBalance->aRoiRight[i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_ROI_END_COORDS,
            WBS_ROI_ROW_END, pWhiteBalance->aRoiBottom[i]);
        WriteMem((IMG_UINT32*)pMemory, 
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_ROI_END_COORDS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_0,
            WBS_RMAX_TH, pWhiteBalance->aRMax[i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_0,
            WBS_GMAX_TH, pWhiteBalance->aGMax[i]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_0, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_1,
            WBS_BMAX_TH, pWhiteBalance->aBMax[i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_1,
            WBS_YHLW_TH, pWhiteBalance->aYMax[i]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_1, i), tmp);
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_MISC,
        WBS_ROI_ACT, pWhiteBalance->ui8ActiveROI);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_MISC,
        WBS_RGB_OFFSET, pWhiteBalance->ui16RGBOffset);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_MISC,
        WBS_Y_OFFSET, pWhiteBalance->ui16YOffset);
#if FELIX_VERSION_MAJ == 1
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBS_MISC,
        WBS_CACHE_POLICY, USE_WBS_CACHE_POLICY);
#endif
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(WBS_MISC), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "WBS_NOT_AVAILABLE");
#endif
}

void HW_CI_Load_ENS(char *pMemory, const CI_MODULE_ENS *pEncStats)
{
#ifndef ENS_NOT_AVAILABLE
    IMG_UINT32 tmp;

    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENS_NCOUPLES,
        ENS_NCOUPLES_EXP, pEncStats->ui8Log2NCouples);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENS_NCOUPLES), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENS_REGION_REGS,
        ENS_SUBS_EXP, pEncStats->ui8SubExp);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENS_REGION_REGS), tmp);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
#else
    IMG_ASSERT(g_psCIDriver);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "ENS_NOT_AVAILABLE");
#endif
}

/*
 * register read
 */

IMG_RESULT HW_CI_DriverLoadInfo(KRN_CI_DRIVER *pDriver, CI_HWINFO *pInfo)
{
    IMG_UINT32 ui32RegVal, i, nCtx, tmp;

    IMG_ASSERT(pDriver);
    IMG_ASSERT(pInfo);

    // assumes pInfo is not NULL!
    IMG_MEMSET(pInfo, 0, sizeof(CI_HWINFO));

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_CORE_ID_OFFSET, &ui32RegVal);
    pInfo->ui8AllocationId = 
        REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, CORE_ID, ALLOCATION_ID);
    pInfo->ui8GroupId = 
        REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, CORE_ID, GROUP_ID);
#if FELIX_VERSION_MAJ == 1
    pInfo->config_ui8BitDepth = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, 
        CORE_ID, FELIX_BIT_DEPTH)+1;
    pInfo->config_ui8NImagers = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, 
        CORE_ID, FELIX_NUM_IMAGERS)+1;
    pInfo->config_ui8NContexts = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, 
        CORE_ID, FELIX_NUM_CONTEXT)+1;
    pInfo->config_ui8Parallelism = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, 
        CORE_ID, FELIX_PARALLELISM)+1;

    pInfo->ui16ConfigId = 0; /// @ is it available in config 1.2?
#else // FELIX_VERSION_MAJ == 2
    pInfo->ui16ConfigId = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        CORE_ID, CONFIG_ID);

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_CORE_ID2_OFFSET, &ui32RegVal);
    pInfo->config_ui8BitDepth = REGIO_READ_FIELD(ui32RegVal,
        FELIX_CORE, CORE_ID2, FELIX_BIT_DEPTH)+1;
    pInfo->config_ui8NImagers = REGIO_READ_FIELD(ui32RegVal,
        FELIX_CORE, CORE_ID2, FELIX_NUM_IMAGERS)+1;
    pInfo->config_ui8NContexts = REGIO_READ_FIELD(ui32RegVal,
        FELIX_CORE, CORE_ID2, FELIX_NUM_CONTEXT)+1;
    pInfo->config_ui8Parallelism = REGIO_READ_FIELD(ui32RegVal,
        FELIX_CORE, CORE_ID2, FELIX_PARALLELISM)+1;
#endif // FELIX_VERSION_MAJ

#ifdef INFOTM_ISP
	#ifdef INFOTM_ENABLE_ISP_DEBUG
	CI_INFO("Core ID: %d.%d.%d\n", pInfo->ui8AllocationId, pInfo->ui8GroupId, pInfo->ui16ConfigId);
	#endif //INFOTM_ENABLE_ISP_DEBUG	
#endif //INFOTM_ISP

    if (pInfo->ui8GroupId != FELIX_GROUP_ID
        || pInfo->ui8AllocationId != FELIX_ALLOCATION_ID)
    {
        CI_FATAL("Device found is not the expected one! "\
            "Found group=0x%x/allocation=0x%x while expecting 0x%x/0x%x\n",
            pInfo->ui8GroupId, pInfo->ui8AllocationId,
            FELIX_GROUP_ID, FELIX_ALLOCATION_ID);
        return IMG_ERROR_NOT_SUPPORTED;
    }

#ifdef INFOTM_ISP
	#ifdef INFOTM_ENABLE_ISP_DEBUG
	CI_INFO("Core BitDepth: %d\n", pInfo->config_ui8BitDepth);
	CI_INFO("Core Imagers: %d\n", pInfo->config_ui8NImagers);
	CI_INFO("Core Contexts: %d\n", pInfo->config_ui8NContexts);
	CI_INFO("Core Parallelism: %d\n", pInfo->config_ui8Parallelism);
	#endif //INFOTM_ENABLE_ISP_DEBUG
#endif //INFOTM_ISP
    pInfo->config_ui8NIIFDataGenerator = 0;
#if FELIX_VERSION_MAJ == 2
    for (i = 0 ; i < CI_N_IIF_DATAGEN ; i++)
    {
        TALREG_ReadWord32(pDriver->sTalHandles.hRegIIFDataGen[i],
            FELIX_DG_IIF_DG_CONFIG_OFFSET, &ui32RegVal);
        tmp = REGIO_READ_FIELD(ui32RegVal, FELIX_DG_IIF, DG_CONFIG, 
            DG_ENABLE_STATUS);
        if (0 != tmp)
        {
            pInfo->config_ui8NIIFDataGenerator++;
        }
        else
        {
            break;
        }
    }
#ifdef INFOTM_ISP
	#ifdef INFOTM_ENABLE_ISP_DEBUG
	CI_INFO("Core IIF DG: %d\n", pInfo->config_ui8NIIFDataGenerator);
	#endif //INFOTM_ENABLE_ISP_DEBUG	
#endif //INFOTM_ISP
#endif

    nCtx = IMG_MIN_INT(pInfo->config_ui8NContexts, CI_N_CONTEXT);

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_CORE_REVISION_OFFSET, &ui32RegVal);
    pInfo->rev_ui8Designer = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        CORE_REVISION, CORE_DESIGNER);
    pInfo->rev_ui8Major = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        CORE_REVISION, CORE_MAJOR_REV);
    pInfo->rev_ui8Minor = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        CORE_REVISION, CORE_MINOR_REV);
    pInfo->rev_ui8Maint = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        CORE_REVISION, CORE_MAINT_REV);
		
#ifdef INFOTM_ISP
	#ifdef INFOTM_ENABLE_ISP_DEBUG
	CI_INFO("Core Revision: %d.%d.%d.%d\n", pInfo->rev_ui8Designer, pInfo->rev_ui8Major, pInfo->rev_ui8Minor, pInfo->rev_ui8Maint);
	#endif //INFOTM_ENABLE_ISP_DEBUG
#endif //INFOTM_ISP		

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_UID_NUM_OFFSET, &(pInfo->rev_uid));

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DESIGNER_REV_FIELD1_OFFSET, &ui32RegVal);
    pInfo->config_ui8PixPerClock = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD1, GASKET_PIXELS_PER_CLOCK)+1;
    // FELIX_SEL_CONF

    tmp = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, DESIGNER_REV_FIELD1,
        MEMORY_LATENCY);
    switch(tmp)
    {
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_1024: 
            pInfo->config_ui16MemLatency = 1024;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_960:
            pInfo->config_ui16MemLatency = 960;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_896:
            pInfo->config_ui16MemLatency = 896;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_832:
            pInfo->config_ui16MemLatency = 832;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_768:
            pInfo->config_ui16MemLatency = 768;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_704:
            pInfo->config_ui16MemLatency = 704;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_640:
            pInfo->config_ui16MemLatency = 640;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_576:
            pInfo->config_ui16MemLatency = 576;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_512:
            pInfo->config_ui16MemLatency = 512;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_448:
            pInfo->config_ui16MemLatency = 448;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_384:
            pInfo->config_ui16MemLatency = 384;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_320:
            pInfo->config_ui16MemLatency = 320;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_256:
            pInfo->config_ui16MemLatency = 256;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_192:
            pInfo->config_ui16MemLatency = 192;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_128:
            pInfo->config_ui16MemLatency = 128;
            break;
        case FELIX_CORE_MEMORY_LATENCY_LATENCY_64:
            pInfo->config_ui16MemLatency = 64;
            break;
    }

    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DPF_READ_MAP_FIFO_SIZE_OFFSET,
        &(pInfo->config_ui32DPFInternalSize));

    pDriver->sHWInfo.config_ui8NRTMRegisters = FELIX_MAX_RTM_VALUES;

    // load scaler information
    TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DESIGNER_REV_FIELD3_OFFSET, &ui32RegVal);
    // encoder scaler
    pInfo->scaler_ui8EncHLumaTaps = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD3, ENC_SCALER_H_LUMA_TAPS) +1;
    pInfo->scaler_ui8EncVLumaTaps = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD3, ENC_SCALER_V_LUMA_TAPS) +1;
    //pInfo->scaler_ui8EncHChromaTaps
    pInfo->scaler_ui8EncVChromaTaps = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD3, ENC_SCALER_V_CHROMA_TAPS) +1;
    // display scaler
    pInfo->scaler_ui8DispHLumaTaps = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD3, DISP_SCALER_H_LUMA_TAPS) +1;
    pInfo->scaler_ui8DispVLumaTaps = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
        DESIGNER_REV_FIELD3, DISP_SCALER_V_LUMA_TAPS) +1;
    //pInfo->scaler_ui8DispHChromaTaps
    //pInfo->scaler_ui8DispVChromaTaps

    /* max value - will be the maximum buffer size between all contexts 
     * - usually it is CTX0 */
    pInfo->ui32MaxLineStore = 0; 
    for ( i = 0 ; i < nCtx ; i++ )
    {
#if FELIX_VERSION_MAJ == 1
        // we do not use the buffer width
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, &ui32RegVal);
        /*pInfo->context_aBufferWidth[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG, CONTEXT_BUFFER_WIDTH)+1;*/
        pInfo->context_aBitDepth[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG, CONTEXT_BIT_DEPTH)+1;

        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONFIG_2_OFFSET, &ui32RegVal);
        pInfo->context_aMaxWidthMult[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_WIDTH)+1;
        /* this version of the HW is not very precise on the max size for
         * a single context*/
        pInfo->context_aMaxWidthSingle[i] = pInfo->context_aMaxWidthMult[i];
        pInfo->context_aMaxHeight[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_HEIGHT)+1;
#else // FELIX_VERSION_MAJ == 2
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, &ui32RegVal);
        pInfo->context_aMaxWidthSingle[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG, MAX_CONTEXT_WIDTH_SGL)+1;
        pInfo->context_aBitDepth[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG, CONTEXT_BIT_DEPTH)+9;
        /* register definitions says context bit depths is -1 but is
         * -9 in practice */

        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONFIG_2_OFFSET, &ui32RegVal);
        pInfo->context_aMaxWidthMult[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_WIDTH_MULT)+1;
        pInfo->context_aMaxHeight[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_HEIGHT)+1;
#endif // FELIX_VERSION_MAJ

        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET, &ui32RegVal);
        pInfo->context_aPendingQueue[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CONTEXT0, CONTEXT_LINK_EMPTYNESS, CONTEXT_LINK_EMPTYNESS);

        if (pInfo->context_aMaxWidthSingle[i] > pInfo->ui32MaxLineStore)
        {
            pInfo->ui32MaxLineStore = pInfo->context_aMaxWidthSingle[i];
        }
        
#if FELIX_VERSION_MAJ == 2
        if (2 == pInfo->rev_ui8Major 
            && 0 == pInfo->rev_ui8Minor 
            && 4 == pInfo->ui16ConfigId)
        {
            //BRN50031
            IMG_UINT32 context_width = 0;
            IMG_ASSERT(nCtx == 2);

            CI_PDUMP_COMMENT(pDriver->sTalHandles.hRegFelixCore,
                "BRN50031 changes CONTEXT_CONFIG values");

            TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[0],
                FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, &ui32RegVal);
            // this field does not exists in current register version
            /*context_width = REGIO_READ_FIELD(ui32RegVal, FELIX_CONTEXT0,
                CONTEXT_CONFIG, CONTEXT_BUFFER_WIDTH)+1;*/ 
            context_width = (ui32RegVal & 0x7FFF); // bits [0..14]

            // multiple context
            pInfo->context_aMaxWidthMult[0] = context_width/2; 
            // single context
            pInfo->context_aMaxWidthSingle[0] = 
                IMG_MIN_INT(context_width, pInfo->ui32MaxLineStore);

            TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixContext[1],
                FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, &ui32RegVal);
            // this field does not exists in current register version
            /*context_width = REGIO_READ_FIELD(ui32RegVal, FELIX_CONTEXT0,
            CONTEXT_CONFIG, CONTEXT_BUFFER_WIDTH)+1;*/ 
            context_width = (ui32RegVal & 0x7FFF); // bits [0..14]

            // multiple context
            pInfo->context_aMaxWidthMult[1] = context_width/2; 
            // single context
            pInfo->context_aMaxWidthSingle[1] = context_width/2; 
        }
#endif // FELIX_VERSION_MAJ
    } // for each context
    //
    // nCtx used for the imagers
    //
    nCtx = IMG_MIN_INT(pInfo->config_ui8NImagers, CI_N_IMAGERS);
    for ( i = 0 ; i < nCtx ; i++ )
    {
#if FELIX_VERSION_MAJ == 1
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
            REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD4, i), &ui32RegVal);
        pInfo->gasket_aImagerPortWidth[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CORE, DESIGNER_REV_FIELD4, IMGR_PORT_WIDTH);
        pInfo->gasket_aType[i] = (enum CI_GASKETTYPE)(
            REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, 
            DESIGNER_REV_FIELD4, GASKET_TYPE));
        pInfo->gasket_aBitDepth[i] = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
            DESIGNER_REV_FIELD4, GASKET_BIT_DEPTH);
        pInfo->gasket_aMaxWidth[i] = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
            DESIGNER_REV_FIELD4, GASKET_MAX_FRAME_WIDTH);
#else // FELIX_VERSION_MAJ == 2
        TALREG_ReadWord32(pDriver->sTalHandles.hRegFelixCore,
            REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD5, i), &ui32RegVal);
        pInfo->gasket_aImagerPortWidth[i] = REGIO_READ_FIELD(ui32RegVal,
            FELIX_CORE, DESIGNER_REV_FIELD5, IMGR_PORT_WIDTH);
        pInfo->gasket_aType[i] = (enum CI_GASKETTYPE)(
            REGIO_READ_FIELD(ui32RegVal, FELIX_CORE, DESIGNER_REV_FIELD5,
            GASKET_TYPE));
        pInfo->gasket_aBitDepth[i] = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
            DESIGNER_REV_FIELD5, GASKET_BIT_DEPTH);
        pInfo->gasket_aMaxWidth[i] = REGIO_READ_FIELD(ui32RegVal, FELIX_CORE,
            DESIGNER_REV_FIELD5, GASKET_MAX_FRAME_WIDTH);
#endif // FELIX_VERSION_MAJ
    }

    pInfo->eFunctionalities = CI_INFO_SUPPORTED_NONE;
    if ( pInfo->rev_ui8Major > 1 )
    {
        if (pInfo->rev_ui8Major == 2 && pInfo->rev_ui8Minor == 3)
        {
            pInfo->eFunctionalities |= CI_INFO_SUPPORTED_TILING;
        }
        else
        {
            pInfo->eFunctionalities |= CI_INFO_SUPPORTED_TILING
                | CI_INFO_SUPPORTED_HDR_EXT | CI_INFO_SUPPORTED_HDR_INS
                | CI_INFO_SUPPORTED_RAW2D_EXT;
        }
    }
    if ( pInfo->config_ui8NIIFDataGenerator > 0 )
    {
        pInfo->eFunctionalities |= CI_INFO_SUPPORTED_IIF_DATAGEN;
        // supported from 3.X
        /*if (pInfo->rev_ui8Major >= 2 && pInfo->rev_ui8Minor >= 2)
        {
            pInfo->eFunctionalities |= CI_INFO_SUPPORTED_IIF_PARTERNGEN;
        }*/
    }

    TALREG_ReadWord32(pDriver->sMMU.hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONFIG1_OFFSET, &ui32RegVal);

    pInfo->ui32MMUPageSize =
        1<<(12+REGIO_READ_FIELD(ui32RegVal, IMG_VIDEO_BUS4_MMU,
            MMU_CONFIG1, PAGE_SIZE));

    pInfo->uiSizeOfPipeline = sizeof(CI_PIPELINE);
    pInfo->uiChangelist = CI_CHANGELIST;
#ifdef INFOTM_ISP
    pInfo->ui32RefClockMhz = ui32ClkRate / 1000000;
#else
    pInfo->ui32RefClockMhz = CI_HW_REF_CLK;
#endif

    return IMG_SUCCESS;
}

IMG_RESULT HW_CI_WriteGasket(CI_GASKET *pGasket, int enable)
{
#if FELIX_VERSION_MAJ == 1
    IMG_UINT32 tmpReg;

    IMG_ASSERT(pGasket->uiGasket < CI_N_IMAGERS);
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGaskets[0], "start");
    
    if (0 != enable && pGasket->bParallel)
    {
        enum FELIX_GASKET_PARALLEL_CTRL_PARALLEL_PIXEL_FORMAT_ENUM format;

        switch(pGasket->ui8ParallelBitdepth)
        {
        case 12:
            format = FELIX_GASKET_PARALLEL_PIXEL_FORMAT_GASKET_12BIT_FORMAT;
            break;
        case 10:
            format = FELIX_GASKET_PARALLEL_PIXEL_FORMAT_GASKET_10BIT_FORMAT;
            break;
        default:
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGaskets[0],
                "failed");
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, PARALLEL_FRAME_SIZE,
            PARALLEL_FRAME_HEIGHT, pGasket->uiHeight);
        /* this is the number of pixels not the number of bytes 
         * (thus not convWidth) */
        REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, PARALLEL_FRAME_SIZE,
            PARALLEL_FRAME_WIDTH, pGasket->uiWidth); 
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[0],
            REGIO_TABLE_OFF(FELIX_GASKET, PARALLEL_FRAME_SIZE,
                pGasket->uiGasket), tmpReg);

        tmpReg = 0;
        REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, PARALLEL_CTRL,
            PARALLEL_PIXEL_FORMAT, format);
        REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, PARALLEL_CTRL,
            PARALLEL_V_SYNC_POLARITY, pGasket->bVSync); // active high...
        REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, PARALLEL_CTRL,
            PARALLEL_H_SYNC_POLARITY, pGasket->bHSync); // active high...
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[0],
            REGIO_TABLE_OFF(FELIX_GASKET, PARALLEL_CTRL, pGasket->uiGasket),
            tmpReg);
    }
#ifdef FELIX_FAKE
    if (0 == enable)
    {
        // when releasing check the number of frames
        tmpReg = 0;
        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[0],
            REGIO_TABLE_OFF(FELIX_GASKET, GASKETS_FRAME_CNT,
                pGasket->uiGasket), &tmpReg);
        /* BRN49465 gasket counter does not work properly 
         * - disable POL on counter */
        /*TALREG_Poll32(g_psCIDriver->sTalHandles.hRegGaskets[0], 
            REGIO_TABLE_OFF(FELIX_GASKET, GASKETS_FRAME_CNT,
            pGasket->uiGasket), TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0,
            CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);*/
        if (0 == tmpReg)
        {
            CI_WARNING("The frame counter for gasket %d is 0 when disabled\n",
                pGasket->uiGasket);
        }
    }
#endif
    // no setup requiered for mipi

    tmpReg = 0; // enable/disable the gasket
    REGIO_WRITE_FIELD(tmpReg, FELIX_GASKET, GASKET_CONTROL,
        GASKETS_ENABLE, enable);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[0],
        REGIO_TABLE_OFF(FELIX_GASKET, GASKET_CONTROL, pGasket->uiGasket),
        tmpReg);
        
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegGaskets[0], "done");
    return IMG_SUCCESS;
#else // FELIX_VERSION_MAJ == 2
    IMG_UINT32 tmpReg;
    IMG_HANDLE gasket = NULL;
    IMG_BOOL8 bIsParallel;
    IMG_UINT8 ui8Gasket = pGasket->uiGasket;
    IMG_ASSERT(pGasket->uiGasket < CI_N_IMAGERS);
    
    gasket = g_psCIDriver->sTalHandles.hRegGaskets[ui8Gasket];
    
    bIsParallel = IMG_FALSE;
    if ((g_psCIDriver->sHWInfo.gasket_aType[ui8Gasket] & CI_GASKET_PARALLEL))
    {
        bIsParallel = IMG_TRUE;
    }

    CI_PDUMP_COMMENT(gasket, "start");

    /* when disabling the "gasket" struct was built up by the kernel 
     * therefore it does not contain a type */
    if (0 != enable && pGasket->bParallel != bIsParallel ) 
    {
        CI_FATAL("Try to configure a %s gasket with wrong format (%s).\n",
            bIsParallel?"Parallel":"MIPI", 
            pGasket->bParallel?"Parallel":"MIPI");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (bIsParallel)
    {
        if (0 != enable)
        {
#ifndef INFOTM_ISP
//not used for hardware
            if (pGasket->ui8ParallelBitdepth !=
                g_psCIDriver->sHWInfo.gasket_aBitDepth[ui8Gasket])
            {
                CI_FATAL("Try to configure a %db gasket with %db data!\n",
                    g_psCIDriver->sHWInfo.gasket_aBitDepth[ui8Gasket],
                    pGasket->ui8ParallelBitdepth);
                return IMG_ERROR_NOT_SUPPORTED;
            }
#endif
            // clear counters before enabling
            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_CONTROL,
                GASKET_RESET, 1);
            TALREG_WriteWord32(gasket, FELIX_GAS_PARA_GASKET_CONTROL_OFFSET,
                tmpReg);
            
            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_CONTROL,
                GASKET_CLEAR_CNT, 1);
            TALREG_WriteWord32(gasket, FELIX_GAS_PARA_GASKET_CONTROL_OFFSET,
                tmpReg);
            
            // maybe add a POL to wait for HW to reset?
            
            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_FRAME_SIZE,
                PARALLEL_FRAME_HEIGHT, pGasket->uiHeight);
            /* this is the number of pixels not the number of bytes
             * (thus not convWidth) */
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_FRAME_SIZE,
                PARALLEL_FRAME_WIDTH, pGasket->uiWidth); 
            TALREG_WriteWord32(gasket, 
                FELIX_GAS_PARA_GASKET_FRAME_SIZE_OFFSET, tmpReg);

            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_FORMAT,
                PARALLEL_V_SYNC_POLARITY, pGasket->bVSync); // active high...
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_FORMAT,
                PARALLEL_H_SYNC_POLARITY, pGasket->bHSync); // active high...
            TALREG_WriteWord32(gasket,
                FELIX_GAS_PARA_GASKET_FORMAT_OFFSET, tmpReg);
        }

        /* BRN56343 ensures gasket interrupts are disabled because of
         * cross domain crossing */
        tmpReg = 0;
        TALREG_WriteWord32(gasket, FELIX_GAS_PARA_GASKET_INTER_ENABLE_OFFSET,
            tmpReg);

        tmpReg = 0; // enable/disable the gasket
        REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_PARA, GASKET_CONTROL,
            GASKET_ENABLE, enable);
        TALREG_WriteWord32(gasket,
            FELIX_GAS_PARA_GASKET_CONTROL_OFFSET, tmpReg);
        
#ifdef FELIX_FAKE
    if (0 == enable && 0 )
    {
        /* disabled because of BRN49615: When the imager interface is setup 
         * to crop the bottom of the image from the frame then clocks may be
         * turned off to the gasket frame counter register before the frame
         * end has been received. */
        // when releasing check the number of frames
        tmpReg = 0;
        TALREG_ReadWord32(gasket, FELIX_GAS_PARA_GASKET_FRAME_CNT_OFFSET,
            &tmpReg);
        TALREG_Poll32(gasket, FELIX_GAS_PARA_GASKET_FRAME_CNT_OFFSET,
            TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0, CI_REGPOLL_TRIES,
            CI_REGPOLL_TIMEOUT);
        if (0 == tmpReg)
        {
            CI_WARNING("The frame counter for gasket %d is 0 when disabled\n",
                ui8Gasket);
        }
    }
#endif // FELIX_FAKE
    }
    else 
    {
        // no setup requiered for mipi
        
        if(enable)
        {
            // clear counters before enabling
            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_MIPI, GASKET_CONTROL,
                GASKET_RESET, 1);
            TALREG_WriteWord32(gasket, FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET,
                tmpReg);
            
            tmpReg = 0;
            REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_MIPI, GASKET_CONTROL,
                GASKET_CLEAR_CNT, 1);
            TALREG_WriteWord32(gasket, FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET,
                tmpReg);
            
            // maybe add a POL to wait for HW to reset?
#ifdef INFOTM_ISP
            TALREG_WriteWord32(gasket, FELIX_GAS_MIPI_GASKET_INTER_CLEAR_OFFSET,
                0xffffffff);
#endif
        }

        tmpReg = 0; // enable/disable the gasket
        REGIO_WRITE_FIELD(tmpReg, FELIX_GAS_MIPI, GASKET_CONTROL,
            GASKET_ENABLE, enable);
        TALREG_WriteWord32(gasket, FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET,
            tmpReg);
        
        /* BRN56343 ensures gasket interrupts are disabled because of
         * cross domain crossing */
        tmpReg = 0;
        TALREG_WriteWord32(gasket, FELIX_GAS_MIPI_GASKET_INTER_ENABLE_OFFSET,
            tmpReg);

#ifdef FELIX_FAKE
        if (0 == enable && 0 ) 
        {
            /* disabled because of BRN49615: When the imager interface is
             * setup to crop the bottom of the image from the frame then
             * clocks may be turned off to the gasket frame counter register
             * before the frame end has been received. */
            // when releasing check the number of frames
            tmpReg = 0;
            TALREG_ReadWord32(gasket, FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET,
                &tmpReg);
            TALREG_Poll32(gasket, FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET,
                TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0, CI_REGPOLL_TRIES,
                CI_REGPOLL_TIMEOUT);
            if (0 == tmpReg)
            {
                CI_WARNING("The frame counter for gasket %d is 0 when "\
                    "disabled\n", ui8Gasket);
            }
        }
#endif // FELIX_FAKE
    }
        
    CI_PDUMP_COMMENT(gasket, "done");
    return IMG_SUCCESS;
#endif // FELIX_VERSION_MAJ
}

IMG_UINT32 HW_CI_GasketFrameCount(IMG_UINT32 ui32Gasket)
{
    IMG_UINT32 gasketCount = 0;

    IMG_ASSERT(ui32Gasket < CI_N_IMAGERS);
    IMG_ASSERT(g_psCIDriver);

#if FELIX_VERSION_MAJ == 1
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[0], 
        REGIO_TABLE_OFF(FELIX_GASKET, GASKETS_FRAME_CNT, ui32Gasket),
        &(gasketCount)
    );
#elif FELIX_VERSION_MAJ >= 2
    if (CI_GASKET_MIPI == g_psCIDriver->sHWInfo.gasket_aType[ui32Gasket])
    {
        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[ui32Gasket], 
            FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET,
            &(gasketCount));
    }
    else if (CI_GASKET_PARALLEL ==
        g_psCIDriver->sHWInfo.gasket_aType[ui32Gasket])
    {
        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegGaskets[ui32Gasket], 
            FELIX_GAS_PARA_GASKET_FRAME_CNT_OFFSET,
            &(gasketCount));
    }
#endif

    return gasketCount;
}

IMG_STATIC_ASSERT(PWR_AUTO == 
    (int)FELIX_CORE_CLOCK_GATECTRL_GRP_0_AUTO_CLOCK_GATING,
    autoClockGateValue);
IMG_STATIC_ASSERT(PWR_FORCE_ON ==
    (int)FELIX_CORE_CLOCK_GATECTRL_GRP_0_FORCE_CLOCK_ENABLE,
    forceClockGateValue);
#if FELIX_VERSION_MAJ == 2
IMG_STATIC_ASSERT(PWR_AUTO ==
    (int)FELIX_DG_IIF_CLOCK_GATECTRL_DG_AUTO_CLOCK_GATING,
    autoDGClockGateValue);
IMG_STATIC_ASSERT(PWR_FORCE_ON ==
    (int)FELIX_DG_IIF_CLOCK_GATECTRL_DG_FORCE_CLOCK_ENABLE,
    forceDGClockGateValue);
#endif

void HW_CI_DriverPowerManagement(enum PwrCtrl eCtrl)
{
    IMG_UINT32 regValue = 0;
#if FELIX_VERSION_MAJ == 2
    unsigned int i;
#endif
    IMG_ASSERT(g_psCIDriver);

    if ((int)FELIX_CORE_CLOCK_GATECTRL_GRP_0_FORCE_CLOCK_GATE == eCtrl)
    {
        CI_WARNING("turning off power! HW will become unavailable.\n");
    }

    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_0, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_1, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_2, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_3, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_4, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_5, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_GRP_6, eCtrl);

    // force clock on because hardware does not manage SCB clocks correctly
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_SCB, PWR_FORCE_ON);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_MEM, eCtrl);

#if FELIX_VERSION_MAJ == 1
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_CTX_0, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_CTX_1, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_CTX_2, eCtrl);

    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_CTRL, PWR_FORCE_ON);
#else // FELIX_VERSION_MAJ
    /* may need to force the clock if gasket still has frame counter 
     * issue as in HW 1.2 */
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL,
        CLOCK_GATECTRL_CTRL, eCtrl);
#endif
    

    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL_OFFSET, regValue);

#if FELIX_VERSION_MAJ == 2
    regValue = 0;
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_0, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_1, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_2, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_3, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_4, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_5, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_6, eCtrl);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_7, eCtrl);

    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL2_OFFSET, regValue);

    regValue = 0;
    REGIO_WRITE_FIELD(regValue, FELIX_DG_IIF, DG_PWR_CTRL,
        CLOCK_GATECTRL_DG, eCtrl);

    for (i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NIIFDataGenerator ; i++)
    {
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegIIFDataGen[i],
            FELIX_DG_IIF_DG_PWR_CTRL_OFFSET, regValue);
    }
#endif // FELIX_VERSION_MAJ
}

void HW_CI_DriverPowerManagement_BRN48137(IMG_UINT8 ui8Context,
    enum PwrCtrl eCtrl)
{
    IMG_UINT32 value = 0;
        
#if FELIX_VERSION_MAJ == 1
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL_OFFSET, &value);
    switch(ui8Context)
    {
    case 0:
        REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL,
            CLOCK_GATECTRL_CTX_0, eCtrl);
        break;
    case 1:
        REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL,
            CLOCK_GATECTRL_CTX_1, eCtrl);
        break;
    case 2:
        REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL,
            CLOCK_GATECTRL_CTX_2, eCtrl);
        break;
    }

    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL_OFFSET, value);
#else // FELIX_VERSION_MAJ == 2
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL2_OFFSET, &value);
    switch(ui8Context)
    {
    case 0: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_0, eCtrl); break;
    case 1: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_1, eCtrl); break;
    case 2: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_2, eCtrl); break;
    case 3: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_3, eCtrl); break;
    case 4: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_4, eCtrl); break;
    case 5: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_5, eCtrl); break;
    case 6: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_6, eCtrl); break;
    case 7: REGIO_WRITE_FIELD(value, FELIX_CORE, FELIX_PWR_CTRL2,
        CLOCK_GATECTRL_CTX_7, eCtrl); break;
    }

    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_PWR_CTRL2_OFFSET, value);
#endif // FELIX_VERSION_MAJ
}

void HW_CI_DriverTimestampManagement(void)
{
    IMG_UINT32 regValue = 0;
    IMG_ASSERT(g_psCIDriver);

#ifndef FELIX_FAKE
    /* CSIM limitation: when dumping CRC it dumps timestamp but CSIM does not
     * manipulates TS correctly for HW verification */
    /* could read the register before if it had other fields to enable other
     * things but it does not at the moment */
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_CONTROL_OFFSET, &regValue);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_CONTROL, TS_ENABLE, 1);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_CONTROL_OFFSET, regValue);
#else
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixCore,
        "Would enable TS here - but CSIM does not support timestamps");
#endif
}

void HW_CI_DriverTilingManagement(void)
{
    // HW v1 does not support tiling!
#if FELIX_VERSION_MAJ == 2
    IMG_UINT32 regValue = 0;
    IMG_ASSERT(g_psCIDriver);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_CONTROL_OFFSET, &regValue);
    REGIO_WRITE_FIELD(regValue, FELIX_CORE, FELIX_CONTROL,
        MMU_TILING_SCHEME, g_psCIDriver->sMMU.uiTiledScheme == 512 ? 1 : 0);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_CONTROL_OFFSET, regValue);
#endif // FELIX_VERSION_MAJ
}

void HW_CI_DriverResetHW(void)
{
    IMG_UINT32 i, resetVal;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(g_psCIDriver->sTalHandles.hRegFelixCore);

    // reset the HW
    resetVal = 0;
    REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, CORE_RESET, 1);
    //REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, MMU_RESET, 1);
    REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, GASKET_RESET, 1);
    REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, SCB_RESET, 1);
#if FELIX_VERSION_MAJ == 2
    //REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, ALL_RESET, 1);
    REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, DG_IIF_RESET, 1);
    //REGIO_WRITE_FIELD(resetVal, FELIX_CORE, CORE_RESET, AXI_RESET, 1);
#endif
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_CORE_RESET_OFFSET, resetVal);
    // harbitrary time given by HW
    TAL_Wait(g_psCIDriver->sTalHandles.hRegFelixCore, CI_REGPOLL_TIMEOUT);

    resetVal = 0;
    REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
        COUNTERS_CLEAR, 1);
    REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
        CRC_CLEAR, 1);
    REGIO_WRITE_FIELD(resetVal, FELIX_CONTEXT0, CONTEXT_CONTROL,
        LINK_LIST_CLEAR, 1);

    for ( i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NContexts ; i++ )
    {
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, resetVal);

#ifdef FELIX_UNIT_TESTS
        // clean the register
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[i],
            FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, 0);
#endif
    }

    //g_psCIDriver->ui8EncOutContext = CI_N_CONTEXT;
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_ENC_OUT_CTRL_OFFSET,
        FELIX_CORE_ENC_OUT_CTRL_SEL_DISABLE
            <<FELIX_CORE_ENC_OUT_CTRL_ENC_OUT_CTRL_SEL_SHIFT);
}

void HW_CI_DriverWriteEncOut(IMG_UINT8 ui8Context, IMG_UINT8 ui8EncOutPulse)
{
    IMG_UINT32 value = 0;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(g_psCIDriver->sTalHandles.hRegFelixCore);

    if (ui8Context < CI_N_CONTEXT)
    {
        REGIO_WRITE_FIELD(value, FELIX_CORE, ENC_OUT_CTRL,
            ENC_OUT_CTRL_SEL, ui8Context);
        REGIO_WRITE_FIELD(value, FELIX_CORE, ENC_OUT_CTRL,
            ENC_OUT_PULSE_WTH_MINUS1, ui8EncOutPulse);
    }
    else
    {
        REGIO_WRITE_FIELD(value, FELIX_CORE, ENC_OUT_CTRL,
            ENC_OUT_CTRL_SEL, FELIX_CORE_ENC_OUT_CTRL_SEL_DISABLE);
    }
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_ENC_OUT_CTRL_OFFSET, value);
}

void HW_CI_PipelineEnqueueShot(KRN_CI_PIPELINE *pPipeline, KRN_CI_SHOT *pShot)
{
    IMG_UINT8 ui8Context = 0;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pShot);

    ui8Context = pPipeline->userPipeline.ui8Context;

#ifdef FELIX_FAKE
    {
        TALREG_Poll32(g_psCIDriver->sTalHandles.hRegFelixContext[ui8Context],
            FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET,
            TAL_CHECKFUNC_NOTEQUAL, 0, ~0, CI_REGPOLL_TRIES,
            CI_REGPOLL_TIMEOUT);
        /* needed because WriteDevMemRef is pdumped in device memory file
         * when running multiple pdump context */
        TALPDUMP_BarrierWithId(CI_SYNC_MEM_ADDRESS);
    }
#endif
    SYS_MemWriteAddressToReg(
        g_psCIDriver->sTalHandles.hRegFelixContext[ui8Context],
        FELIX_CONTEXT0_CONTEXT_LINK_ADDR_OFFSET, &(pShot->hwLinkedList), 0);

}

void HW_CI_ReadGasket(CI_GASKET_INFO *pInfo, IMG_UINT8 uiGasket)
{
    IMG_HANDLE gasket = NULL;
    IMG_UINT32 tmpReg = 0;
    IMG_BOOL8 bEnabled = IMG_FALSE;

    IMG_ASSERT(uiGasket < CI_N_IMAGERS);
    IMG_ASSERT(pInfo);
    IMG_ASSERT(g_psCIDriver);

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        if(g_psCIDriver->apGasketUser[uiGasket])
        {
            bEnabled = IMG_TRUE;
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

#if FELIX_VERSION_MAJ == 1
    
    gasket = g_psCIDriver->sTalHandles.hRegGaskets[0];
    CI_PDUMP_COMMENT(gasket, "start");

    TALREG_ReadWord32(gasket, REGIO_TABLE_OFF(FELIX_GASKET, GASKET_CONTROL,
        uiGasket), &tmpReg);

    if (bEnabled)
    {
        // it is enabled find if it is MIPI or Parallel
        pInfo->eType = g_psCIDriver->sHWInfo.gasket_aType[uiGasket];

        if ((pInfo->eType&CI_GASKET_MIPI))
        {
            TALREG_ReadWord32(gasket, 
                REGIO_TABLE_OFF(FELIX_GASKET, GASKET_MIPI_STATUS, uiGasket),
                &tmpReg);

            pInfo->ui8MipiEccCorrected = REGIO_READ_FIELD(tmpReg,
                FELIX_GASKET, GASKET_MIPI_STATUS, MIPI_ECC_CORRECTED_CNT);
            pInfo->ui8MipiEccError = REGIO_READ_FIELD(tmpReg,
                FELIX_GASKET, GASKET_MIPI_STATUS, MIPI_ECC_ERROR_CNT);
            pInfo->ui8MipiHdrError = REGIO_READ_FIELD(tmpReg,
                FELIX_GASKET, GASKET_MIPI_STATUS, MIPI_HDR_ERROR_CNT);
            pInfo->ui8MipiCrcError = REGIO_READ_FIELD(tmpReg,
                FELIX_GASKET, GASKET_MIPI_STATUS, MIPI_CRC_ERROR_CNT);

            TALREG_ReadWord32(gasket, REGIO_TABLE_OFF(FELIX_GASKET,
                GASKETS_FRAME_CNT, uiGasket), &(pInfo->ui32FrameCount));

            
            TALREG_ReadWord32(gasket, 
                REGIO_TABLE_OFF(FELIX_GASKET, GASKET_MIPI_STATUS2, uiGasket),
                &tmpReg);

            pInfo->ui8MipiEnabledLanes = REGIO_READ_FIELD(tmpReg,
                FELIX_GASKET, GASKET_MIPI_STATUS2, MIPI_ENABLE_LANE);

            // ui8MipiFifoFull not available for HW v1
        }
    }
    else
    {
        pInfo->eType = CI_GASKET_NONE;
    }

#else // FELIX_VERSION_MAJ == 2
    
    gasket = g_psCIDriver->sTalHandles.hRegGaskets[uiGasket];
    CI_PDUMP_COMMENT(gasket, "start");

    pInfo->eType = 0; // no type - gasket is not enabled

    if ((g_psCIDriver->sHWInfo.gasket_aType[uiGasket]&CI_GASKET_PARALLEL))
    {
        TALREG_ReadWord32(gasket, FELIX_GAS_PARA_GASKET_CONTROL_OFFSET,
            &tmpReg);

        if (bEnabled)
        {
            pInfo->eType = CI_GASKET_PARALLEL;

            TALREG_ReadWord32(gasket, FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET, 
                &(pInfo->ui32FrameCount));
        }        
    }
    else // mipi
    {
        TALREG_ReadWord32(gasket, FELIX_GAS_MIPI_GASKET_CONTROL_OFFSET,
            &tmpReg);

        if (bEnabled)
        {
            // it is enabled so we can gather data
            pInfo->eType = CI_GASKET_MIPI;

            TALREG_ReadWord32(gasket, 
                FELIX_GAS_MIPI_GASKET_MIPI_STATUS_OFFSET, &tmpReg);

            pInfo->ui8MipiEccCorrected = REGIO_READ_FIELD(tmpReg, 
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS, MIPI_ECC_CORRECTED_CNT);
            pInfo->ui8MipiEccError = REGIO_READ_FIELD(tmpReg, 
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS, MIPI_ECC_ERROR_CNT);
            pInfo->ui8MipiHdrError = REGIO_READ_FIELD(tmpReg, 
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS, MIPI_HDR_ERROR_CNT);
            pInfo->ui8MipiCrcError = REGIO_READ_FIELD(tmpReg, 
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS, MIPI_CRC_ERROR_CNT);
            
            TALREG_ReadWord32(gasket, FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET,
                &(pInfo->ui32FrameCount));
            
            TALREG_ReadWord32(gasket, 
                FELIX_GAS_MIPI_GASKET_MIPI_STATUS2_OFFSET, &tmpReg);

            pInfo->ui8MipiFifoFull = REGIO_READ_FIELD(tmpReg, 
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS2, MIPI_FIFO_FULL);
            pInfo->ui8MipiEnabledLanes = REGIO_READ_FIELD(tmpReg,
                FELIX_GAS_MIPI, GASKET_MIPI_STATUS2, MIPI_ENABLE_LANE);
        }
    }

    CI_PDUMP_COMMENT(gasket, "done");
#endif // FELIX_VERSION_MAJ
}

void HW_CI_ReadTimestamps(IMG_UINT32 *pTimestamps)
{
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(g_psCIDriver->sTalHandles.hRegFelixCore);
    IMG_ASSERT(pTimestamps);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_TS_COUNTER_OFFSET, pTimestamps);
}

void HW_CI_ReadCurrentDEPoint(KRN_CI_DRIVER *pKrnDriver,
    enum CI_INOUT_POINTS *eCurrent)
{
    IMG_UINT32 regval, i;
    IMG_ASSERT(pKrnDriver);
    IMG_ASSERT(pKrnDriver->sTalHandles.hRegFelixCore);
    IMG_ASSERT(eCurrent);

    TALREG_ReadWord32(pKrnDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DE_CTRL_OFFSET, &regval);
    i = REGIO_READ_FIELD(regval, FELIX_CORE, DE_CTRL, DE_SELECT);
    // no need to lock it is not available to change yet
    *eCurrent = (enum CI_INOUT_POINTS)i; 
}

void HW_CI_UpdateCurrentDEPoint(enum CI_INOUT_POINTS eWanted)
{
    IMG_UINT32 value = 0;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(g_psCIDriver->sTalHandles.hRegFelixCore);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DE_CTRL_OFFSET, &value);
    REGIO_WRITE_FIELD(value, FELIX_CORE, DE_CTRL, DE_SELECT, eWanted);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_DE_CTRL_OFFSET, value);
}

IMG_UINT32 HW_CI_ReadCoreRTM(IMG_UINT8 ui8RTM)
{
    IMG_UINT32 value = (IMG_UINT32)-1;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(g_psCIDriver->sTalHandles.hRegFelixCore);
    IMG_ASSERT(ui8RTM < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters);
    
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_RTM_A_CTRL_OFFSET, (IMG_UINT32)ui8RTM);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_RTM_A_OFFSET, &value);

#if defined(VERBOSE_DEBUG)
    CI_DEBUG("read RTM %u: 0x%x\n", (unsigned)ui8RTM, value);
#endif
    
    return value;
}

void HW_CI_ReadContextRTM(IMG_UINT8 ctx, CI_RTM_INFO *pRTMInfo)
{
    IMG_HANDLE ctxhandle = NULL;
    int i;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(ctx < g_psCIDriver->sHWInfo.config_ui8NContexts);
    IMG_ASSERT(FELIX_CONTEXT0_GCTRL_POS_Y_NO_ENTRIES
        == g_psCIDriver->sHWInfo.config_ui8NRTMRegisters);
    IMG_ASSERT(pRTMInfo);

    ctxhandle = g_psCIDriver->sTalHandles.hRegFelixContext[ctx];
    
    TALREG_ReadWord32(ctxhandle, FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET,
        &(pRTMInfo->context_status[ctx]));
    TALREG_ReadWord32(ctxhandle, FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET,
        &(pRTMInfo->context_linkEmptyness[ctx]));
    
#if defined(VERBOSE_DEBUG)
    CI_DEBUG("read CTX %d status 0x%x and LL empty 0x%x\n",
        (int)ctx, pRTMInfo->context_status[ctx],
        pRTMInfo->context_linkEmptyness[ctx]);
#endif

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
    {
        TALREG_ReadWord32(ctxhandle,
            REGIO_TABLE_OFF(FELIX_CONTEXT0, GCTRL_POS_Y, i),
            &(pRTMInfo->context_position[ctx][i]));
        
#if defined(VERBOSE_DEBUG)
        CI_DEBUG("read CTX %d position %d 0x%x\n",
            (int)ctx, i, pRTMInfo->context_position[ctx][i]);
#endif
    }
}

#ifdef INFOTM_ISP
void HW_CI_ReadContextToPrint(IMG_UINT8 ctx)
{
    IMG_HANDLE ctxhandle = NULL;
    IMG_UINT32 ui32Idx, ui32RegStart, ui32Value;
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(ctx < g_psCIDriver->sHWInfo.config_ui8NContexts);

    ctxhandle = g_psCIDriver->sTalHandles.hRegFelixContext[ctx];

    ui32RegStart = FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET;
    for (ui32Idx = 0; ui32Idx < 0x100; ui32Idx += 4)
    {
		if ((ui32Idx % 0x10) == 0)
			printk("\n%04X | ", ui32RegStart + ui32Idx);
		TALREG_ReadWord32(ctxhandle, ui32RegStart + ui32Idx,
			&ui32Value);
		printk("%08X ", ui32Value);
    }
    printk("\n");

}
#endif //INFOTM_ISP

//
// linked list
//

IMG_RESULT HW_CI_ShotLoadLinkedList(KRN_CI_SHOT *pShot,
    unsigned int eConfigOutput, IMG_UINT32 ui32LoadConfigReg)
{
    IMG_UINT32 config = 0; // config of the buffer in HW list
    IMG_UINT32 dpfConfig = 0;
    IMG_UINT32 ui32TimeStamps = 0;
    
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pPipeline);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");

    // we may not know the buffer ID yet
    SYS_MemWriteWord(&pShot->hwLinkedList, HW_LLIST_OFF(CONTEXT_TAG),
        pShot->iIdentifier);

    // load given value
    config = 0; // used for LOAD_CONFIG_FLAGS
    if (CI_UPD_NONE != ui32LoadConfigReg)
    {
        /** @ could compute the correct flags for loading only the 
         * needed elements */
        // puts all the LOAD_CONFIG_GRP_X to 1
        config = (1<<FELIX_LINK_LIST_LOAD_CONFIG_FLAGS_CONFIG_TAG_SHIFT)-1;
    }
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, LOAD_CONFIG_FLAGS,
        CONFIG_TAG, pShot->pPipeline->userPipeline.ui8PrivateValue);
    SYS_MemWriteWord(&pShot->hwLinkedList, HW_LLIST_OFF(LOAD_CONFIG_FLAGS),
        config);

    config = 0; // used for SAVE_CONFIG_FLAGS

    /* same than checking pShot->pPipeline->userPipeline.eEncType.eBuffer
     * != TYPE_NONE */
    if (pShot->phwEncoderOutput) 
    {
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            ENC_ENABLE, 1);

        // 0 if 8b, 1 if 10b
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            ENC_FORMAT,
            pShot->pPipeline->userPipeline.eEncType.ui8BitDepth != 8 ? 1 : 0);
    }

    if (pShot->phwDisplayOutput)
    {
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            DISP_DE_ENABLE, 1);

        if (CI_INOUT_NONE != pShot->pPipeline->userPipeline.eDataExtraction)
        {
            // data extraction is enabled
            REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
                DE_NO_DISP, 1);
            switch(pShot->pPipeline->userPipeline.eDispType.ui8BitDepth)
            {
                case 8:
                    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST,
                        SAVE_CONFIG_FLAGS, DISP_DE_FORMAT,
                        FELIX_CONTEXT0_DISP_DE_FORMAT_BD8_24);
                    break;
                case 10:
                    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST,
                        SAVE_CONFIG_FLAGS, DISP_DE_FORMAT,
                        FELIX_CONTEXT0_DISP_DE_FORMAT_BD10);
                    break;
                case 12:
                    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST,
                        SAVE_CONFIG_FLAGS, DISP_DE_FORMAT,
                        FELIX_CONTEXT0_DISP_DE_FORMAT_BD12);
                    break;
                default:
                    CI_FATAL("unknown Bayer format bitdepth %d!\n",
                        pShot->pPipeline->userPipeline.eDispType.ui8BitDepth);
                    return IMG_ERROR_FATAL;
            }
        }
        else
        {
            // display pipeline is enabled
            // 24b
            if (3 == pShot->pPipeline->userPipeline.eDispType.ui8PackedStride)
            {
                REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
                    DISP_DE_FORMAT, FELIX_CONTEXT0_DISP_DE_FORMAT_BD8_24);
            }
            else // 32b - either 8b or 10b
            {
                int fmt = FELIX_CONTEXT0_DISP_DE_FORMAT_BD8_32;
                // if not 8b it is 10b
                if (8 != pShot->pPipeline->userPipeline.eDispType.ui8BitDepth)
                {
                    fmt = FELIX_CONTEXT0_DISP_DE_FORMAT_BD10;
                }
                REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
                    DISP_DE_FORMAT, fmt);
            }
        }
    }

#if FELIX_VERSION_MAJ == 2
    if (pShot->phwHDRExtOutput)
    {
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            HDF_WR_ENABLE, 1);
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            HDF_WR_FORMAT, FELIX_LINK_LIST_HDF_WR_FORMAT_RGB10);
    }

    if (pShot->phwHDRInsertion)
    {
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            HDF_RD_ENABLE, 1);
        // assumes 1 because register def has no valid value
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            HDF_RD_FORMAT, 1); 
    }

    if (pShot->phwRaw2DExtOutput)
    {
        int raw2dfmt = 0;

        switch(pShot->pPipeline->userPipeline.eRaw2DExtraction.eFmt)
        {
        case BAYER_TIFF_10:
            raw2dfmt = 0;
            break;
        case BAYER_TIFF_12:
            raw2dfmt = 1;
            break;
        default:
            CI_WARNING("Unknown TIFF format for RAW2D extraction! "\
                "Output disabled\n");
            raw2dfmt = -1;
        }

        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            RAW_2D_ENABLE, raw2dfmt>=0?1:0);
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            RAW_2D_FORMAT, raw2dfmt);
    }
    else
    {
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            RAW_2D_ENABLE, 0);
    }
#endif // FELIX_VERSION_MAJ

    dpfConfig = pShot->pPipeline->userPipeline.sDefectivePixels.eDPFEnable;

    // configure the output statistics according to the config's value
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        TIME_STAMP_ENABLE, ((eConfigOutput&CI_SAVE_TIMESTAMP)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        CRC_ENABLE, ((eConfigOutput&CI_SAVE_CRC)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        FLD_ENABLE, ((eConfigOutput&CI_SAVE_FLICKER)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        HIS_REGION_ENABLE, ((eConfigOutput&CI_SAVE_HIST_REGION)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        HIS_GLOB_ENABLE, ((eConfigOutput&CI_SAVE_HIST_GLOBAL)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        WBS_ENABLE, ((eConfigOutput&CI_SAVE_WHITEBALANCE)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        FOS_ROI_ENABLE, ((eConfigOutput&CI_SAVE_FOS_ROI)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        FOS_GRID_ENABLE, ((eConfigOutput&CI_SAVE_FOS_GRID)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        EXS_GLOB_ENABLE, ((eConfigOutput&CI_SAVE_EXS_GLOBAL)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        EXS_REGION_ENABLE, ((eConfigOutput&CI_SAVE_EXS_REGION)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        ENS_ENABLE, ((eConfigOutput&CI_SAVE_ENS)? 1 : 0));
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        DPF_READ_MAP_ENABLE, (dpfConfig&CI_DPF_READ_MAP_ENABLED) ? 1 : 0);
    REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
        DPF_WRITE_MAP_ENABLE, (dpfConfig&CI_DPF_WRITE_MAP_ENABLED) ? 1 : 0);

    if ((eConfigOutput&CI_SAVE_ENS)
        && pShot->sENSOutput.sMemory.uiAllocated
        < pShot->pPipeline->uiENSSizeNeeded)
    {
        CI_WARNING("force ENS to be disabled because associated buffer "\
            "does not a big enough ENS buffer (%uB but needs %uB)\n",
            pShot->sENSOutput.sMemory.uiAllocated,
            pShot->pPipeline->uiENSSizeNeeded);
        REGIO_WRITE_FIELD(config, FELIX_LINK_LIST, SAVE_CONFIG_FLAGS,
            ENS_ENABLE, 0);
    }

#if defined(VERBOSE_DEBUG)
    CI_DEBUG("writing SAVE_CONFIG_FLAGS=0x%x\n", config);
#endif
    
    HW_CI_ReadTimestamps(&ui32TimeStamps);
    CI_DEBUG("Update linked list at %u\n", ui32TimeStamps);

    // write the config to the structure
    SYS_MemWriteWord(&(pShot->hwLinkedList), HW_LLIST_OFF(SAVE_CONFIG_FLAGS),
        config);

    pShot->userShot.ui32LinkedListTS = ui32TimeStamps;
        
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");

    return IMG_SUCCESS;
}

void HW_CI_ShotUpdateAddresses(KRN_CI_SHOT *pShot)
{
#if FELIX_VERSION_MAJ == 2
    IMG_UINT32 firstPage = 0;
    IMG_UINT32 lastPageOff = 0;
    IMG_UINT32 tilingConf = 0; // config of the tiling
#endif

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pShot);

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "start");
    
    /* same than checking pShot->pPipeline->userPipeline.eEncType.eBuffer
     * != TYPE_NONE */
    if (pShot->phwEncoderOutput) 
    {
#ifdef INFOTM_ISP
        if (pShot->pPipeline->userPipeline.uiTotalPipeline > 1)           
        {
            KRN_CI_BUFFER *phwDualEncoderOutput;
            IMG_UINT32 EncYOffset0, EncYOffset1, EncCbCrOffset0, EncCbCrOffset1;

            phwDualEncoderOutput = (KRN_CI_BUFFER*)pShot->phwEncoderOutput->pDualBuffer;
            EncYOffset0 = 0;
            EncYOffset1 = pShot->userShot.aEncYSize[0] * pShot->userShot.aEncYSize[1];
            EncCbCrOffset0 = EncYOffset1 + pShot->userShot.aEncYSize[0] * pShot->userShot.aEncYSize[1];
            EncCbCrOffset1 = EncCbCrOffset0 + pShot->userShot.aEncCbCrSize[0] * pShot->userShot.aEncCbCrSize[1];
            //  #ifdef INFOTM_ENABLE_ISP_DEBUG
            //	printk("@@@ ShotUpdat[%d]: EncYOffset0=%d, EncYOffset0=%d, EncCbCrOffset0=%d, EncCbCrOffset1=%d, aEncOffset[0]=%d, aEncOffset[1]=%d, name=%s\n",
            //                  pShot->pPipeline->userPipeline.ui8Context,
            //                  EncYOffset0,
            //                  EncYOffset1,
            //                  EncCbCrOffset0,
            //                  EncCbCrOffset1,
            //                  pShot->userShot.aEncOffset[0],
            //                  pShot->userShot.aEncOffset[1],
            //                  phwDualEncoderOutput->sMemory.szMemName);
            //	#endif //INFOTM_ENABLE_ISP_DEBUG

            if (pShot->pPipeline->userPipeline.ui8Context == 1)
            {
                /*
                 * setting up a YUV output
                 */
#ifdef INFOTM_ENABLE_ISP_DEBUG
                //printk("<<< ENC_Y_ADDR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
#endif //INFOTM_ENABLE_ISP_DEBUG
                SYS_MemWriteAddress(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_Y_ADDR),
                    &(phwDualEncoderOutput->sMemory),
                    EncYOffset1);
                // no need to divide by sysmem the bottom bits are masked
                //printk("<<< ENC_Y_STR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_Y_STR), pShot->userShot.aEncYSize[0]);

                //printk("<<< ENC_C_ADDR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteAddress(&pShot->hwLinkedList,
                    HW_LLIST_OFF(MEM_ENC_C_ADDR),
                    &(phwDualEncoderOutput->sMemory),
                    EncCbCrOffset1);

                // no need to divide by sysmem the bottom bits are masked
                //printk("<<< ENC_C_STR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&pShot->hwLinkedList,
                    HW_LLIST_OFF(MEM_ENC_C_STR), pShot->userShot.aEncCbCrSize[0]);

#if FELIX_VERSION_MAJ == 2
                // first and last page
                firstPage =	SYS_MemGetFirstAddress(&(phwDualEncoderOutput->sMemory)) + EncYOffset1;
                lastPageOff = EncYOffset1 +  pShot->userShot.aEncYSize[0] * pShot->userShot.aEncYSize[1];
                /* masking the bottom bits when writing gives the page - cannot use
                 * offset because it will not be a multiple of 4 and TAL will assert */
                //printk("<<< ENC_Y_LAST_PAGE[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList), HW_LLIST_OFF(MEM_ENC_Y_LAST_PAGE), firstPage + lastPageOff-1);

                firstPage =	SYS_MemGetFirstAddress(&(pShot->phwEncoderOutput->sMemory)) + EncCbCrOffset1;
                lastPageOff = EncCbCrOffset1 + pShot->userShot.aEncCbCrSize[0] * pShot->userShot.aEncCbCrSize[1];
                /* masking the bottom bits when writing gives the page - cannot use
                 * offset because it will not be a multiple of 4 and TAL will assert */
                //printk("<<< ENC_C_LAST_PAGE[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList), HW_LLIST_OFF(MEM_ENC_C_LAST_PAGE), firstPage + lastPageOff-1);

                REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
                    ENC_L_TILE_EN, pShot->phwEncoderOutput->bTiled);
                REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
                    ENC_C_TILE_EN, pShot->phwEncoderOutput->bTiled);
#endif //FELIX_VERSION_MAJ == 2
            }
            else //else pShot->pPipeline->userPipeline.ui8Context
            {
                /*
                 * setting up a YUV output
                 */
                //printk("<<< ENC_Y_ADDR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteAddress(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_Y_ADDR),
                    &(pShot->phwEncoderOutput->sMemory),
                    EncYOffset0);
                // no need to divide by sysmem the bottom bits are masked
                //printk("<<< ENC_Y_STR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_Y_STR), pShot->userShot.aEncYSize[0]);
                //printk("<<< ENC_C_ADDR ");
                SYS_MemWriteAddress(&pShot->hwLinkedList,
                    HW_LLIST_OFF(MEM_ENC_C_ADDR),
                    &(pShot->phwEncoderOutput->sMemory),
                    EncCbCrOffset0);

                // no need to divide by sysmem the bottom bits are masked
                //printk("<<< ENC_C_STR[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&pShot->hwLinkedList,
                    HW_LLIST_OFF(MEM_ENC_C_STR), pShot->userShot.aEncCbCrSize[0]);

#if FELIX_VERSION_MAJ == 2
                // first and last page
                firstPage =	SYS_MemGetFirstAddress(&(pShot->phwEncoderOutput->sMemory)) + EncYOffset0;
                lastPageOff = EncYOffset0 + pShot->userShot.aEncYSize[0] * pShot->userShot.aEncYSize[1];
                /* masking the bottom bits when writing gives the page - cannot use
                 * offset because it will not be a multiple of 4 and TAL will assert */
                //printk("<<< ENC_Y_LAST_PAGE[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_Y_LAST_PAGE), firstPage + lastPageOff-1);

                firstPage =	SYS_MemGetFirstAddress(&(pShot->phwEncoderOutput->sMemory)) + EncCbCrOffset0;
                lastPageOff = EncCbCrOffset0 + pShot->userShot.aEncCbCrSize[0] * pShot->userShot.aEncCbCrSize[1];
                /* masking the bottom bits when writing gives the page - cannot use
                 * offset because it will not be a multiple of 4 and TAL will assert */
                //printk("<<< ENC_C_LAST_PAGE[%d] ", pShot->pPipeline->userPipeline.ui8Context);
                SYS_MemWriteWord(&(pShot->hwLinkedList),
                    HW_LLIST_OFF(MEM_ENC_C_LAST_PAGE), firstPage + lastPageOff-1);
                REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
                    ENC_L_TILE_EN, pShot->phwEncoderOutput->bTiled);
                REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
                    ENC_C_TILE_EN, pShot->phwEncoderOutput->bTiled);
#endif //FELIX_VERSION_MAJ == 2
            } // end pShot->pPipeline->userPipeline.ui8Context
        }else // else pShot->pPipeline->userPipeline.uiTotalPipeline
#endif
#ifdef INFOTM_ISP
        {
#endif
        /*
         * setting up a YUV output
         */
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_ENC_Y_ADDR),
            &(pShot->phwEncoderOutput->sMemory),
            pShot->userShot.aEncOffset[0]);
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_ENC_Y_STR), pShot->userShot.aEncYSize[0]);
        if (IsLowLight) {

        	SYS_MemWriteAddress(&pShot->hwLinkedList, HW_LLIST_OFF(MEM_ENC_C_ADDR),
        				&g_buffer, pShot->userShot.aEncOffset[1]);
        }
        else {
			SYS_MemWriteAddress(&pShot->hwLinkedList,
				HW_LLIST_OFF(MEM_ENC_C_ADDR),
				&(pShot->phwEncoderOutput->sMemory),
				pShot->userShot.aEncOffset[1]);

        }
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&pShot->hwLinkedList,
            HW_LLIST_OFF(MEM_ENC_C_STR), pShot->userShot.aEncCbCrSize[0]);

#if FELIX_VERSION_MAJ == 2
        // first and last page
        firstPage = 
            SYS_MemGetFirstAddress(&(pShot->phwEncoderOutput->sMemory)); 
        lastPageOff = pShot->userShot.aEncOffset[0]
            + pShot->userShot.aEncYSize[0]*pShot->userShot.aEncYSize[1];
        /* masking the bottom bits when writing gives the page - cannot use
         * offset because it will not be a multiple of 4 and TAL will assert */
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_ENC_Y_LAST_PAGE), firstPage + lastPageOff-1); 

        lastPageOff = pShot->userShot.aEncOffset[1]
            + pShot->userShot.aEncCbCrSize[0]*pShot->userShot.aEncCbCrSize[1];
        /* masking the bottom bits when writing gives the page - cannot use
         * offset because it will not be a multiple of 4 and TAL will assert */
        if (IsLowLight) {
        	SYS_MemWriteWord(&(pShot->hwLinkedList),
        	                 HW_LLIST_OFF(MEM_ENC_C_LAST_PAGE), 0x00);
        }
        else {
        	SYS_MemWriteWord(&(pShot->hwLinkedList),
        			HW_LLIST_OFF(MEM_ENC_C_LAST_PAGE), firstPage + lastPageOff-1);
        }
        REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
            ENC_L_TILE_EN, pShot->phwEncoderOutput->bTiled);
        REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
            ENC_C_TILE_EN, pShot->phwEncoderOutput->bTiled);
#endif
#ifdef INFOTM_ISP
        }// end pShot->pPipeline->userPipeline.uiTotalPipeline
#endif
    }
    else
    {
        SYS_MemWriteWord(&pShot->hwLinkedList, HW_LLIST_OFF(MEM_ENC_Y_ADDR),
            SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList), HW_LLIST_OFF(MEM_ENC_Y_STR),
            0);

        SYS_MemWriteWord(&pShot->hwLinkedList, HW_LLIST_OFF(MEM_ENC_C_ADDR),
            SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList), HW_LLIST_OFF(MEM_ENC_C_STR),
            0);

#if FELIX_VERSION_MAJ == 2
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_ENC_Y_LAST_PAGE), SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_ENC_C_LAST_PAGE), SYSMEM_INVALID_ADDR);
#endif
    }

    if (pShot->phwDisplayOutput)
    {
        /*
         * setting up RGB output
         */
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_ADDR),
            &(pShot->phwDisplayOutput->sMemory),
            pShot->userShot.ui32DispOffset);
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_STR), pShot->userShot.aDispSize[0]);

#if FELIX_VERSION_MAJ == 2
        firstPage =
            SYS_MemGetFirstAddress(&(pShot->phwDisplayOutput->sMemory));
        lastPageOff = pShot->userShot.ui32DispOffset
            + pShot->userShot.aDispSize[0] * pShot->userShot.aDispSize[1];
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_LAST_PAGE), firstPage + lastPageOff-1);

        REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
            DISP_DE_TILE_EN, pShot->phwDisplayOutput->bTiled);
#endif
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_ADDR), SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_STR), 0);
#if FELIX_VERSION_MAJ == 2
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_DISP_DE_LAST_PAGE), SYSMEM_INVALID_ADDR);
#endif
    }

#if FELIX_VERSION_MAJ == 2
    if (pShot->phwHDRExtOutput)
    {
        /*
         * HDR extraction output
         */
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_ADDR),
            &(pShot->phwHDRExtOutput->sMemory),
            pShot->userShot.ui32HDRExtOffset);
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_STR), pShot->userShot.aHDRExtSize[0]);

        firstPage = SYS_MemGetFirstAddress(&(pShot->phwHDRExtOutput->sMemory));
        lastPageOff = pShot->userShot.ui32HDRExtOffset
            + pShot->userShot.aHDRExtSize[0] * pShot->userShot.aHDRExtSize[1];
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_LAST_PAGE), firstPage + lastPageOff-1);

        REGIO_WRITE_FIELD(tilingConf, FELIX_LINK_LIST, TILING_CONTROL,
            HDF_WR_TILE_EN, pShot->phwHDRExtOutput->bTiled);
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_ADDR), SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_STR), 0);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_WR_LAST_PAGE), SYSMEM_INVALID_ADDR);
    }

    if (pShot->phwHDRInsertion)
    {
        /*
         * HDR Insertion
         */
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_RD_ADDR),
            &(pShot->phwHDRInsertion->sMemory),
            pShot->userShot.ui32HDRInsOffset);
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_RD_STR), pShot->userShot.aHDRInsSize[0]);
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_RD_ADDR), SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_HDF_RD_STR), 0);
    }

    if (pShot->phwRaw2DExtOutput)
    {
        /*
         * Tiff extraction output
         */
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_ADDR),
            &(pShot->phwRaw2DExtOutput->sMemory),
            pShot->userShot.ui32Raw2DOffset);
        // no need to divide by sysmem the bottom bits are masked
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_STR), pShot->userShot.aRaw2DSize[0]);

        firstPage =
            SYS_MemGetFirstAddress(&(pShot->phwRaw2DExtOutput->sMemory));
        lastPageOff = pShot->userShot.ui32Raw2DOffset
            + pShot->userShot.aRaw2DSize[0] * pShot->userShot.aRaw2DSize[1];
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_LAST_PAGE), firstPage + lastPageOff-1);
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_ADDR), SYSMEM_INVALID_ADDR);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_STR), 0);
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(MEM_RAW_2D_LAST_PAGE), SYSMEM_INVALID_ADDR);
    }
#endif // FELIX_VERSION_MAJ

    // address of the config
    SYS_MemWriteAddress(&(pShot->hwLinkedList),
        HW_LLIST_OFF(LOAD_CONFIG_ADDR), &(pShot->hwLoadStructure), 0);

    // address of the stat output
    SYS_MemWriteAddress(&(pShot->hwLinkedList),
        HW_LLIST_OFF(SAVE_RESULTS_ADDR), &(pShot->hwSave.sMemory), 0);

    // address of the DPF output map
    if (pShot->sDPFWrite.sMemory.uiAllocated > 0)
    {
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(DPF_WR_MAP_ADDR), &(pShot->sDPFWrite.sMemory), 0);
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(DPF_WR_MAP_ADDR), SYSMEM_INVALID_ADDR);
    }

    if (pShot->sENSOutput.sMemory.uiAllocated > 0)
    {
        SYS_MemWriteAddress(&(pShot->hwLinkedList),
            HW_LLIST_OFF(ENS_WR_ADDR), &(pShot->sENSOutput.sMemory), 0);
    }
    else
    {
        SYS_MemWriteWord(&(pShot->hwLinkedList),
            HW_LLIST_OFF(ENS_WR_ADDR), SYSMEM_INVALID_ADDR);
    }
    
#if FELIX_VERSION_MAJ == 2
    SYS_MemWriteWord(&(pShot->hwLinkedList),
        HW_LLIST_OFF(TILING_CONTROL), tilingConf);
#endif

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "done");
}

void HW_CI_ShotUpdateSubmittedLS(KRN_CI_SHOT *pShot)
{
    void *pBuffer;
    IMG_UINT32 ui32LoadConfigReg = 0;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pPipeline);
    IMG_ASSERT(pShot->pPipeline->pLoadStructStamp);

    pBuffer = Platform_MemGetMemory(&(pShot->hwLoadStructure));
    
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "disable flags");
    // disable all flags until memory has been updated
    SYS_MemWriteWord(&(pShot->hwLinkedList), 
        HW_LLIST_OFF(LOAD_CONFIG_FLAGS), 0); 

    // copy the new configuration
    IMG_MEMCPY(pBuffer, pShot->pPipeline->pLoadStructStamp,
        FELIX_LOAD_STRUCTURE_BYTE_SIZE);

    // update the HW memory
    Platform_MemUpdateDevice(&(pShot->hwLoadStructure)); 

    // re-enable loading flags
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "enable flags");
    /// @ compute the correct flags for loading only the needed elements
    // puts all the LOAD_CONFIG_GRP_X to 1
    ui32LoadConfigReg =
        (1<<FELIX_LINK_LIST_LOAD_CONFIG_FLAGS_CONFIG_TAG_SHIFT)-1;
    REGIO_WRITE_FIELD(ui32LoadConfigReg, FELIX_LINK_LIST, LOAD_CONFIG_FLAGS,
        CONFIG_TAG, pShot->pPipeline->userPipeline.ui8PrivateValue);
    SYS_MemWriteWord(&(pShot->hwLinkedList),
        HW_LLIST_OFF(LOAD_CONFIG_FLAGS), ui32LoadConfigReg);

#ifdef FELIX_UNIT_TESTS
        /* update save structure as well... because there is no HW to do that
         * during unit tests */
        ui32LoadConfigReg = 0;
        REGIO_WRITE_FIELD(ui32LoadConfigReg, FELIX_SAVE, CONTEXT_LOGS,
            SAVE_CONFIG_TAG, pShot->pPipeline->userPipeline.ui8PrivateValue);
        SYS_MemWriteWord(&(pShot->hwSave.sMemory),
            FELIX_SAVE_CONTEXT_LOGS_OFFSET, ui32LoadConfigReg);
#endif   
}

void HW_CI_ShotUpdateLS(KRN_CI_SHOT *pShot)
{
    void *pBuffer;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pPipeline);
    IMG_ASSERT(pShot->pPipeline->pLoadStructStamp);

    pBuffer = Platform_MemGetMemory(&(pShot->hwLoadStructure));

#ifdef FELIX_FAKE
    {
        char message[64];
        sprintf(message, "Context %u uses STAMP_%p",
            pShot->pPipeline->userPipeline.ui8Context,
            pShot->pPipeline->pLoadStructStamp);
        CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, message);
    }
#endif
    // apply DPF output map information
    if (pShot->pPipeline->userPipeline.ui32DPFWriteMapSize > 0)
    {
        HW_CI_Update_DPF(pShot->pPipeline->pLoadStructStamp,
            pShot->pPipeline->userPipeline.ui32DPFWriteMapSize);
    }

    IMG_MEMCPY(pBuffer, pShot->pPipeline->pLoadStructStamp,
        FELIX_LOAD_STRUCTURE_BYTE_SIZE);

    Platform_MemUpdateDevice(&(pShot->hwLoadStructure));
}


/*
 * internal DG functions
 */

IMG_RESULT HW_CI_DatagenStart(KRN_CI_DATAGEN *pDatagen)
{
#if FELIX_VERSION_MAJ >= 2
    IMG_UINT32 tmpReg;
    IMG_HANDLE hDGTalHandle = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pDatagen);
    // should have been checked before
    IMG_ASSERT(pDatagen->userDG.ui8IIFDGIndex == 0); 

    tmpReg = pDatagen->userDG.ui8IIFDGIndex;
    hDGTalHandle = g_psCIDriver->sTalHandles.hRegIIFDataGen[tmpReg];
    
    // reset data gen
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_RESET, DG_RESET, 1);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_RESET_OFFSET, tmpReg);

    TAL_Wait(hDGTalHandle, CI_REGPOLL_TIMEOUT); // harbitrary time given by HW

    /*tmpReg = 0;
    TALREG_ReadWord32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET, &tmpReg);
    g_pDGDriver->aFrameSent[ui8Context] = REGIO_READ_FIELD(tmpReg,
        FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
#ifdef FELIX_FAKE
    TALREG_Poll32(hDGTalHandle, FELIX_TEST_DG_DG_STATUS_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, tmpReg, ~0, CI_REGPOLL_TRIES,
        CI_REGPOLL_TIMEOUT);
#endif*/

    // wait for DG to be disabled
#ifdef FELIX_UNIT_TESTS
    // as if the HW finished reseting
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_CTRL_OFFSET, 0);
#endif
    
    ret = TALREG_Poll32(hDGTalHandle, FELIX_DG_IIF_DG_CTRL_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, 0, FELIX_DG_IIF_DG_CTRL_DG_ENABLE_MASK,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
    if (ret != IMG_SUCCESS)
    {
        CI_FATAL("Failed to poll the control-enable bit\n");
        return IMG_ERROR_FATAL;
    }

    // clear restart status
    pDatagen->bNeedsReset = IMG_FALSE;

    // set interrupt: end of frame + error
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_INTER_ENABLE,
        DG_INT_END_OF_FRAME_EN, 1);
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_INTER_ENABLE,
        DG_INT_ERROR_EN, 1);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_INTER_ENABLE_OFFSET,
        tmpReg);

    // setup gasket muxing
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_GASKET_MUX, DG_GASKET_MUX,
        1<<pDatagen->userDG.ui8Gasket);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_GASKET_MUX_OFFSET,
        tmpReg);

    // manual mode - write that when a frame is ready for capture
    /*tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_MODE, DG_MODE,
        FELIX_DG_IIF_DG_MODE_DG_MANUAL);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_MODE_OFFSET, tmpReg);*/

    // frame size and blanking should be set when triggering

    // last: enable
    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_IIF_CACHE_POLICY,
        USE_DG_IFF_CACHE_POLICY);
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_PRELOAD,
        pDatagen->userDG.bPreload?1:0);

    switch(pDatagen->userDG.eFormat)
    {
    case CI_DGFMT_PARALLEL_10:
        REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_PIXEL_FORMAT, 0);
        break;
    case CI_DGFMT_PARALLEL_12:
        REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_PIXEL_FORMAT, 1);
        break;

    default:
        ret = IMG_ERROR_NOT_SUPPORTED;
    }

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Unsupported format!\n");
        return ret;
    }

    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_ENABLE, 1);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_CTRL_OFFSET, tmpReg);

    return ret;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif
}

IMG_RESULT HW_CI_DatagenStop(KRN_CI_DATAGEN *pDatagen)
{
#if FELIX_VERSION_MAJ >= 2
    IMG_UINT32 tmpReg;
    IMG_HANDLE hDGTalHandle = NULL;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pDatagen);
    // should have been checked before
    IMG_ASSERT(pDatagen->userDG.ui8IIFDGIndex == 0); 

    tmpReg = pDatagen->userDG.ui8IIFDGIndex;
    hDGTalHandle = g_psCIDriver->sTalHandles.hRegIIFDataGen[tmpReg];

    tmpReg = 0;
    REGIO_WRITE_FIELD(tmpReg, FELIX_DG_IIF, DG_CTRL, DG_ENABLE, 0);
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_CTRL_OFFSET, tmpReg);

    // disable interrupts
    TALREG_WriteWord32(hDGTalHandle, FELIX_DG_IIF_DG_INTER_ENABLE_OFFSET, 0);

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif
}

IMG_UINT32 HW_CI_DatagenFrameCount(IMG_UINT32 ui32InternalDatagen)
{
#if FELIX_VERSION_MAJ >= 2
    IMG_UINT32 reg = 0;
    IMG_UINT32 count = 0;

    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(ui32InternalDatagen < CI_N_IIF_DATAGEN);

    TALREG_ReadWord32(
        g_psCIDriver->sTalHandles.hRegIIFDataGen[ui32InternalDatagen],
        FELIX_DG_IIF_DG_STATUS_OFFSET, &reg);
    count = REGIO_READ_FIELD(reg, FELIX_DG_IIF, DG_STATUS, DG_FRAMES_SENT);

    return count;
#else
    return 0;
#endif
}

#if defined(FELIX_FAKE) || defined(ANDROID_EMULATOR)

#ifdef WIN32
#undef CONTEXT_CONTROL
#include <Windows.h> // for sleep
#elif defined(ANDROID_EMULATOR)
#include <linux/delay.h> // for mdelay
#else
#include <stdlib.h> // for usleep
#endif

void msleep(IMG_UINT32 ui32SleepMiliSec)
{
    if (ui32SleepMiliSec>0)
    {
#ifdef WIN32
        Sleep(ui32SleepMiliSec);
        
#elif defined(ANDROID_EMULATOR)
        mdelay(ui32SleepMiliSec * 1000);
#else
        usleep(ui32SleepMiliSec * 1000);
#endif
    }
}

#endif

