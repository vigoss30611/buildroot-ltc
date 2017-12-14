/**
******************************************************************************
@file ci_load_hw2.c

@brief Definition of the loading for HW v2

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

*****************************************************************************/
#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"
#include "ci/ci_modules_structs.h"
#include "ci/ci_version.h"

#include <img_types.h>

#include <hw_struct/save.h>
#include <hw_struct/ctx_config.h>
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
#include <img_errors.h>

#include <reg_io2.h>
#define LOG_TAG "CI_API"
#include <felixcommon/userlog.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Shortcut for very long offsets macro
 */
#define HW_LOAD_OFF(reg) (FELIX_LOAD_STRUCTURE_ ## reg ## _OFFSET)
#define HW_LOAD_NO(reg) FELIX_LOAD_STRUCTURE_ ## reg ## _NO_ENTRIES

#ifdef FELIX_FAKE
#include "ci_kernel/ci_kernel.h"  // access to g_psCIDriver

#undef CI_PDUMP_COMMENT  // remove the kernel one
#define CI_PDUMP_COMMENT(A) \
{ IMG_ASSERT(g_psCIDriver); \
  if (g_psCIDriver) \
    LOG_CI_PdumpComment(__FUNCTION__, \
        g_psCIDriver->sTalHandles.hMemHandle, A); \
}

#else
#define CI_PDUMP_COMMENT(A)
#endif

#if FELIX_VERSION_MAJ == 2
/**
* @param pMem memory pointer to start of memory
* @param uiOffset in bytes
* @param ui32Val value
*/
static void WriteMem(IMG_UINT32 *pMem, IMG_SIZE uiOffset, IMG_UINT32 ui32Val)
{
    IMG_ASSERT(uiOffset < FELIX_LOAD_STRUCTURE_BYTE_SIZE);
    pMem[uiOffset / 4] = ui32Val;

#ifdef FELIX_FAKE
    {
        char message[128];
        sprintf(message, "Write STAMP_%p:0x%"IMG_SIZEPR"X 0x%X",
            pMem, uiOffset, ui32Val);
        CI_PDUMP_COMMENT(message);
    }
#endif
}

static IMG_UINT32 ReadMem(const IMG_UINT32 *pMem, IMG_SIZE uiOffset)
{
    IMG_ASSERT(uiOffset < FELIX_LOAD_STRUCTURE_BYTE_SIZE);
    return pMem[uiOffset / 4];
}

static void HW_CI_Load_BLC(char *pMemory, const struct CI_MODULE_BLC *pBlack)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pBlack);
    CI_PDUMP_COMMENT("start");

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
    for (i = 0; i < BLC_OFFSET_NO; i++)
    {
        REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE, BLACK_FIXED_OFFSET,
            BLACK_FIXED, i, pBlack->i8FixedOffset[i]);
    }
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(BLACK_FIXED_OFFSET), tmp);

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_RLT(char *pMemory, const CI_MODULE_RLT *pRawLut)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pRawLut);
    CI_PDUMP_COMMENT("start");

    tmp = 0; // assumes enabled
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL, RLT_ENABLE, 1);
    switch (pRawLut->eMode)
    {
    case CI_RLT_LINEAR:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL,
            RLT_INTERP_MODE,
            FELIX_LOAD_STRUCTURE_RLT_INTERP_MODE_RLT_INTERP_MODE_LINEAR);
        break;
    case CI_RLT_CUBIC:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL,
            RLT_INTERP_MODE,
            FELIX_LOAD_STRUCTURE_RLT_INTERP_MODE_RLT_INTERP_MODE_CUBIC);
        break;
    case CI_RLT_DISABLED:
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_CONTROL,
            RLT_ENABLE, 0);
        break;
    default:
        LOG_ERROR("unknown RLT mode value!\n");
        IMG_ASSERT(0);
    }
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(RLT_CONTROL), tmp);

    for (i = 0; i < RLT_SLICE_N_ENTRIES; i++)
    {
        /*
        * OOD/EVEN naming is a bit confusing here because 1st entry of
        * RLT (0) is not present therefore first value stored is in fact
        * number 1, which is odd numbering...
        */
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_0_POINTS,
            RLT_LUT_0_POINTS_ODD_1_TO_15, pRawLut->aPoints[0][2 * i + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_0_POINTS,
            RLT_LUT_0_POINTS_EVEN_2_TO_16, pRawLut->aPoints[0][2 * i + 1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
            RLT_LUT_VALUES_0_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_1_POINTS,
            RLT_LUT_1_POINTS_ODD_1_TO_15, pRawLut->aPoints[1][2 * i + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_1_POINTS,
            RLT_LUT_1_POINTS_EVEN_2_TO_16, pRawLut->aPoints[1][2 * i + 1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,

            RLT_LUT_VALUES_1_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_2_POINTS,
            RLT_LUT_2_POINTS_ODD_1_TO_15, pRawLut->aPoints[2][2 * i + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_2_POINTS,
            RLT_LUT_2_POINTS_EVEN_2_TO_16, pRawLut->aPoints[2][2 * i + 1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
            RLT_LUT_VALUES_2_POINTS, i), tmp);

        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_3_POINTS,
            RLT_LUT_3_POINTS_ODD_1_TO_15, pRawLut->aPoints[3][2 * i + 0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, RLT_LUT_VALUES_3_POINTS,
            RLT_LUT_3_POINTS_EVEN_2_TO_16, pRawLut->aPoints[3][2 * i + 1]);
        WriteMem((IMG_UINT32*)pMemory, REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
            RLT_LUT_VALUES_3_POINTS, i), tmp);
    }

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_LSH_WB(char *pMemory, const struct CI_MODULE_WBC *pWBC)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pWBC);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_WBC(char *pMemory, const struct CI_MODULE_WBC *pWBC)
{
#ifdef WBC_AVAILABLE
    IMG_UINT32 tmp;
    IMG_UINT8 mode = 0;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pWBC);

    CI_PDUMP_COMMENT("start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_GAIN,
        WBC_GAIN_0, pWBC->aRGBGain[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_GAIN,
        WBC_GAIN_1, pWBC->aRGBGain[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_GAIN,
        WBC_GAIN_2, pWBC->aRGBGain[2]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(WBC_GAIN), tmp);
    LOG_DEBUG("WBC_GAIN 0x%x\n", tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_THRESHOLD,
        WBC_THRES_0, pWBC->aRGBThreshold[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_THRESHOLD,
        WBC_THRES_1, pWBC->aRGBThreshold[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_THRESHOLD,
        WBC_THRES_2, pWBC->aRGBThreshold[2]);
    switch(pWBC->eRGBThresholdMode)
    {
        case WBC_SATURATION:
            mode = 0;
            break;
        case WBC_THRESHOLD:
            mode = 1;
            break;
        default:
            LOG_ERROR("unknown mode %d - using mode %d\n",
                (int)pWBC->eRGBThresholdMode, mode);
    }
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, WBC_THRESHOLD,
        WBC_MODE, mode);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(WBC_THRESHOLD), tmp);
    LOG_DEBUG("WBC_THRESHOLD 0x%x\n", tmp);

    CI_PDUMP_COMMENT("done");
#else
    CI_PDUMP_COMMENT("WBC_NOT_AVAILABLE");
    LOG_DEBUG("WBC_NOT_AVAILABLE\n");
#endif /* WBC_AVAILABLE */
}

static void HW_CI_Load_DPF(char *pMemory, const CI_MODULE_DPF *pDefective)
{
    IMG_UINT32 tmp = 0;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pDefective);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_R2Y(char *pMemory, const CI_MODULE_R2Y *pConv)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pConv);
    CI_PDUMP_COMMENT("start");

    for (i = 0; i < RGB_COEFF_NO; i++)
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_Y2R(char *pMemory, const CI_MODULE_Y2R *pConv)
{
    IMG_UINT32 tmp, i;
    // pConfig and pConv != NULL tested before

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pConv);
    CI_PDUMP_COMMENT("start");

    for (i = 0; i < RGB_COEFF_NO; i++)
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_LCA(char *pMemory, const struct CI_MODULE_LCA *pChroma)
{
    IMG_UINT32 i, tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pChroma);
    CI_PDUMP_COMMENT("start");

    for (i = 0; i < YCC_COEFF_NO; i++)
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_CCM(char *pMemory, const CI_MODULE_CCM *pCorr)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pCorr);
    CI_PDUMP_COMMENT("start");

    for (i = 0; i < RGB_COEFF_NO; i++)
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_MIE(char *pMemory, const CI_MODULE_MIE *pMie)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pMie);
    CI_PDUMP_COMMENT("start");

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
        for (i = 0; i + 1 < MIE_VIBRANCY_N; i += 2) // 2 elements per registers
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_FUN,
                MIE_VIB_SATMULT_0, pMie->aVibrancySatMul[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_VIB_FUN,
                MIE_VIB_SATMULT_1, pMie->aVibrancySatMul[i + 1]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_VIB_FUN, i / 2), tmp);
        }
    }

    if (pMie->bMemoryColour)
    {
        for (i = 0; i < MIE_NUM_MEMCOLOURS; i++)
        {
            // this will be overriden if using HW 2.4
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_PARAMS_Y, MIE_MC_YOFF, pMie->aYOffset[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_PARAMS_Y, MIE_MC_YSCALE, pMie->aYScale[i]);
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
                MIE_MC_ROT00, pMie->aRot[2 * i + 0]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, MIE_MC_ROT_PARAMS,
                MIE_MC_ROT01, pMie->aRot[2 * i + 1]);
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

            // this will be overriden if using HW 2.4
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CB0,
                pMie->aGaussCB[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_1, MIE_MC_GAUSS_CR0,
                pMie->aGaussCR[i]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_1, i), tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_2,
                MIE_MC_GAUSS_R00, pMie->aGaussRot[2 * i + 0]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_2,
                MIE_MC_GAUSS_R01, pMie->aGaussRot[2 * i + 1]);
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

        for (i = 0; i < MIE_GAUSS_SC_N / 2; i++)
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_4,
                MIE_MC_GAUSS_S0, pMie->aGaussScale[i * 2]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_4,
                MIE_MC_GAUSS_S1, pMie->aGaussScale[i * 2 + 1]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_4, i), tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_5,
                MIE_MC_GAUSS_G0, pMie->aGaussGain[i * 2]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_5,
                MIE_MC_GAUSS_G1, pMie->aGaussGain[i * 2 + 1]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_5, i), tmp);
        }
    }

    CI_PDUMP_COMMENT("done");
}

#ifdef FELIX_2_4_LS_MIE_MC_PARAMS_Y_OFFSET
/* ensure 2.X and 2.4 have same offset for the registers with different
 * precisions */
IMG_STATIC_ASSERT(FELIX_LOAD_STRUCTURE_MIE_MC_PARAMS_Y_OFFSET \
    == FELIX_2_4_LS_MIE_MC_PARAMS_Y_OFFSET, mie_y_off_offset_incompatible);
IMG_STATIC_ASSERT(FELIX_LOAD_STRUCTURE_MIE_MC_GAUSS_PARAMS_1_OFFSET \
    == FELIX_2_4_LS_MIE_MC_GAUSS_PARAMS_1_OFFSET, mie_gauss_incompatible);
#endif

// changes some entries to fit the HW 2.4 different precisions
static void HW_CI_Load_MIE_2_4(char *pMemory, const CI_MODULE_MIE *pMie)
{
#ifdef FELIX_2_4_LS_MIE_MC_PARAMS_Y_OFFSET
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pMie);
    CI_PDUMP_COMMENT("start");

    if (pMie->bMemoryColour)
    {
        for (i = 0; i < MIE_NUM_MEMCOLOURS; i++)
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS, MIE_MC_PARAMS_Y,
                MIE_MC_YOFF, pMie->aYOffset[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS, MIE_MC_PARAMS_Y,
                MIE_MC_YSCALE, pMie->aYScale[i]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, MIE_MC_PARAMS_Y, i),
                tmp);

            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS,
                MIE_MC_GAUSS_PARAMS_1,
                MIE_MC_GAUSS_CB0, pMie->aGaussCB[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS,
                MIE_MC_GAUSS_PARAMS_1,
                MIE_MC_GAUSS_CR0, pMie->aGaussCR[i]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
                MIE_MC_GAUSS_PARAMS_1, i), tmp);
        }
    }

    CI_PDUMP_COMMENT("done");
#else
    LOG_ERROR("not implemented yet\n");
#endif
}

static void HW_CI_Load_TNM(char *pMemory, const struct CI_MODULE_TNM *pMap)
{
    IMG_UINT32 i, tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pMap);
    CI_PDUMP_COMMENT("start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_BYPASS,
        TNM_BYPASS, pMap->bBypassTNM != 0 ? 1 : 0);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(TNM_BYPASS), tmp);

    if (!pMap->bBypassTNM)
    {
        CI_PDUMP_COMMENT("curve");

        i = 0;
        while (i + 1 < TNM_CURVE_NPOINTS - 1)
        {
            tmp = 0;
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
                TNM_CURVE_POINT_0, pMap->aCurve[i]);
            REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
                TNM_CURVE_POINT_1, pMap->aCurve[i + 1]);
            WriteMem((IMG_UINT32*)pMemory,
                REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
                (i + 1) / 2), tmp);
            i += 2;
        }
        // last one is only 1 element in the curve
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
            TNM_CURVE_POINT_0, pMap->aCurve[TNM_CURVE_NPOINTS - 1]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, TNM_GLOBAL_CURVE,
            (TNM_CURVE_NPOINTS - 1) / 2), tmp);

        CI_PDUMP_COMMENT("curve done");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_DSC(char *pMemory,
    const struct CI_MODULE_SCALER *pScaler)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pScaler);
    CI_PDUMP_COMMENT("start");

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
        CI_PDUMP_COMMENT("bypass - done");
        return;
    }

    for (i = 0; i < (SCALER_PHASES / 2); i++)
    {
        /*
        * v chroma taps
        */
        tmp = 0;
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
        tmp = 0;
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
        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_ESC(char *pMemory,
    const struct CI_MODULE_SCALER *pScaler)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pScaler);
    CI_PDUMP_COMMENT("start");

    /* written in advance because the vertical pitch may be used even if
    * bypass is enabled for decimation purposes */
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_SCAL_V_PITCH),
        pScaler->aPitch[1]);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_SCAL_V_SETUP,
        ENC_SCAL_422_NOT_420, pScaler->bOutput422 ? 1 : 0);
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
        CI_PDUMP_COMMENT("bypass - done");
        return;
    }

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENC_422_TO_420_CTRL,
        ENC_422_TO_420_ENABLE, 0);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENC_422_TO_420_CTRL), tmp);

    for (i = 0; i < (SCALER_PHASES / 2); i++)
    {
        /*
        * v chroma taps
        */

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

        tmp = 0;
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_Scaler(char *pMemory, const struct CI_MODULE_SCALER *pScaler)
{
    IMG_ASSERT(pMemory);
    IMG_ASSERT(pScaler);

    if (pScaler->bIsDisplay)
    {
        HW_CI_Load_DSC(pMemory, pScaler);
    }
    else
    {
        HW_CI_Load_ESC(pMemory, pScaler);
    }
}

static void HW_CI_Load_DNS(char *pMemory, const struct CI_MODULE_DNS *pDenoiser)
{
    IMG_UINT32 i, tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pDenoiser);
    CI_PDUMP_COMMENT("start");

    for (i = 0; i < (DNS_N_LUT0 / 2); i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC0_POINTS, PIX_THRESH_LUT_POINTS_0TO7_0,
            pDenoiser->aPixThresLUT[2 * i]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC0_POINTS, PIX_THRESH_LUT_POINTS_0TO7_1,
            pDenoiser->aPixThresLUT[2 * i + 1]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC0_POINTS, i), tmp);
    }
    for (i = 0; i < (DNS_N_LUT1 / 2); i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC1_POINTS, PIX_THRESH_LUT_POINTS_8TO13_0,
            pDenoiser->aPixThresLUT[2 * i + DNS_N_LUT0]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC1_POINTS, PIX_THRESH_LUT_POINTS_8TO13_1,
            pDenoiser->aPixThresLUT[2 * i + DNS_N_LUT0 + 1]);
        WriteMem((IMG_UINT32*)pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC1_POINTS, i), tmp);
    }
    for (i = 0; i < (DNS_N_LUT2 / 2); i++)
    {
        tmp = 0;
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC2_POINTS, PIX_THRESH_LUT_POINTS_14TO19_0,
            pDenoiser->aPixThresLUT[2 * i + DNS_N_LUT0 + DNS_N_LUT1]);
        REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            PIX_THRESH_LUT_SEC2_POINTS, PIX_THRESH_LUT_POINTS_14TO19_1,
            pDenoiser->aPixThresLUT[2 * i + DNS_N_LUT0 + DNS_N_LUT1 + 1]);
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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_MGM(char *pMemory, const CI_MODULE_MGM *pGamut)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamut);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_DGM(char *pMemory, const CI_MODULE_DGM *pGamut)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamut);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_GMA(char *pMemory, const CI_MODULE_GMA *pGamma)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pGamma);
    CI_PDUMP_COMMENT("start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, GMA_BYPASS,
        GMA_BYPASS, pGamma->bBypass);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(GMA_BYPASS), tmp);

    CI_PDUMP_COMMENT("done");
}

// HW_CI_Load_SHA_2_4 can be called to override one of the register
static void HW_CI_Load_SHA(char *pMemory, const CI_MODULE_SHA *pSharp)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pSharp);
    CI_PDUMP_COMMENT("start");
    
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

    // this can be overriden by HW_CI_Load_SHA_2_4
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
        SHA_GWEIGHT_0, pSharp->aGainWeight[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
        SHA_GWEIGHT_1, pSharp->aGainWeight[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, SHA_PARAMS_1,
        SHA_GWEIGHT_2, pSharp->aGainWeight[2]);
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
        for (i = 0;
            i < FELIX_LOAD_STRUCTURE_SHA_DN_EDGE_SIMIL_COMP_PTS_0_SHA_DN_EDGE_SIMIL_COMP_PTS_0_TO_3_NO_REPS;
            i++)
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                SHA_DN_EDGE_SIMIL_COMP_PTS_0,
                SHA_DN_EDGE_SIMIL_COMP_PTS_0_TO_3, i, pSharp->aDNSimil[i]);
        }
        WriteMem((IMG_UINT32*)pMemory,
            HW_LOAD_OFF(SHA_DN_EDGE_SIMIL_COMP_PTS_0), tmp);

        tmp = 0;
        for (i = 0;
            i < FELIX_LOAD_STRUCTURE_SHA_DN_EDGE_SIMIL_COMP_PTS_1_SHA_DN_EDGE_SIMIL_COMP_PTS_4_TO_6_NO_REPS;
            i++)
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                SHA_DN_EDGE_SIMIL_COMP_PTS_1,
                SHA_DN_EDGE_SIMIL_COMP_PTS_4_TO_6, i, pSharp->aDNSimil[4 + i]);
        }
        WriteMem((IMG_UINT32*)pMemory,
            HW_LOAD_OFF(SHA_DN_EDGE_SIMIL_COMP_PTS_1), tmp);

        tmp = 0;
        for (i = 0;
            i < FELIX_LOAD_STRUCTURE_SHA_DN_EDGE_AVOID_COMP_PTS_0_SHA_DN_EDGE_AVOID_COMP_PTS_0_TO_3_NO_REPS;
            i++)
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                SHA_DN_EDGE_AVOID_COMP_PTS_0,
                SHA_DN_EDGE_AVOID_COMP_PTS_0_TO_3, i, pSharp->aDNAvoid[i]);
        }
        WriteMem((IMG_UINT32*)pMemory,
            HW_LOAD_OFF(SHA_DN_EDGE_AVOID_COMP_PTS_0), tmp);

        tmp = 0;
        for (i = 0;
            i < FELIX_LOAD_STRUCTURE_SHA_DN_EDGE_AVOID_COMP_PTS_1_SHA_DN_EDGE_AVOID_COMP_PTS_4_TO_6_NO_REPS;
            i++)
        {
            REGIO_WRITE_REPEATED_FIELD(tmp, FELIX_LOAD_STRUCTURE,
                SHA_DN_EDGE_AVOID_COMP_PTS_1,
                SHA_DN_EDGE_AVOID_COMP_PTS_4_TO_6, i, pSharp->aDNAvoid[4 + i]);
        }
        WriteMem((IMG_UINT32*)pMemory,
            HW_LOAD_OFF(SHA_DN_EDGE_AVOID_COMP_PTS_1), tmp);
    }

    CI_PDUMP_COMMENT("done");
}

#ifdef FELIX_2_4_LS_SHA_PARAMS_1_OFFSET
/* ensure 2.X and 2.4 have same offset for the registers with different
* precisions */
IMG_STATIC_ASSERT(FELIX_LOAD_STRUCTURE_SHA_PARAMS_1_OFFSET \
    == FELIX_2_4_LS_SHA_PARAMS_1_OFFSET, sha_param1_incompatible);
#endif

// overrides some registers from HW_CI_Load_SHA
static void HW_CI_Load_SHA_2_4(char *pMemory, const CI_MODULE_SHA *pSharp)
{
#ifdef FELIX_2_4_LS_SHA_PARAMS_1_OFFSET
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pSharp);
    CI_PDUMP_COMMENT("start");

    REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS, SHA_PARAMS_1,
        SHA_GWEIGHT_0, pSharp->aGainWeight[0]);
    REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS, SHA_PARAMS_1,
        SHA_GWEIGHT_1, pSharp->aGainWeight[1]);
    REGIO_WRITE_FIELD(tmp, FELIX_2_4_LS, SHA_PARAMS_1,
        SHA_GWEIGHT_2, pSharp->aGainWeight[2]);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(SHA_PARAMS_1), tmp);

    CI_PDUMP_COMMENT("done");
#else
    LOG_ERROR("not implemented yet\n");
#endif
}

// statistics

static void HW_CI_Load_EXS(char *pMemory, const CI_MODULE_EXS *pExposure)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pExposure);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_EXS(const CI_SHOT *pShot, CI_MODULE_EXS *pExposure)
{
    IMG_UINT32 tmp;
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pExposure);
//    CI_PDUMP_COMMENT("start");

    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pExposure, 0, sizeof(CI_MODULE_EXS));

    // used only if CI_SAVE_EXS_GLOBAL
    tmp = ReadMem(pMemory, HW_LOAD_OFF(EXS_GRID_START_COORDS));
    pExposure->ui16Left = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        EXS_GRID_START_COORDS, EXS_GRID_COL_START);
    pExposure->ui16Top = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        EXS_GRID_START_COORDS, EXS_GRID_ROW_START);

    // used only if CI_SAVE_EXS_GLOBAL
    tmp = ReadMem(pMemory, HW_LOAD_OFF(EXS_GRID_TILE));
    pExposure->ui16Width = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        EXS_GRID_TILE, EXS_GRID_TILE_WIDTH);
    pExposure->ui16Height = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        EXS_GRID_TILE, EXS_GRID_TILE_HEIGHT);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(EXS_PIXEL_MAX));
    pExposure->ui16PixelMax = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        EXS_PIXEL_MAX, EXS_PIXEL_MAX);

//    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_FOS(char *pMemory, const CI_MODULE_FOS *pFocus)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pFocus);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_FOS(const CI_SHOT *pShot, CI_MODULE_FOS *pFocus)
{
    IMG_UINT32 tmp;
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pFocus);
//    CI_PDUMP_COMMENT("start");

    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pFocus, 0, sizeof(CI_MODULE_FOS));

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FOS_GRID_START_COORDS));
    pFocus->ui16Left = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_GRID_START_COORDS, FOS_GRID_START_COL);
    pFocus->ui16Top = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_GRID_START_COORDS, FOS_GRID_START_ROW);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FOS_GRID_TILE));
    pFocus->ui16Width = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_GRID_TILE, FOS_GRID_TILE_WIDTH);
    pFocus->ui16Height = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_GRID_TILE, FOS_GRID_TILE_HEIGHT);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FOS_ROI_COORDS_START));
    pFocus->ui16ROILeft = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_ROI_COORDS_START, FOS_ROI_START_COL);
    pFocus->ui16ROITop = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_ROI_COORDS_START, FOS_ROI_START_ROW);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FOS_ROI_COORDS_END));
    pFocus->ui16ROIRight = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_ROI_COORDS_END, FOS_ROI_END_COL);
    pFocus->ui16ROIBottom = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FOS_ROI_COORDS_END, FOS_ROI_END_ROW);

//    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_FLD(char *pMemory, const CI_MODULE_FLD *pFlicker)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pFlicker);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_FLD(const CI_SHOT *pShot, CI_MODULE_FLD *pFlicker)
{
    IMG_UINT32 tmp;
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pFlicker);
//    CI_PDUMP_COMMENT("start");

    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pFlicker, 0, sizeof(CI_MODULE_FLD));

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FLD_FRAC_STEP));
    pFlicker->ui16FracStep50 = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_FRAC_STEP, FLD_FRAC_STEP_50);
    pFlicker->ui16FracStep60 = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_FRAC_STEP, FLD_FRAC_STEP_60);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FLD_TH));
    pFlicker->ui16CoefDiff = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_TH, FLD_COEF_DIFF_TH);
    pFlicker->ui16NFThreshold = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_TH, FLD_NF_TH);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FLD_SCENE_CHANGE_TH));
    pFlicker->ui32SceneChange = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_SCENE_CHANGE_TH, FLD_SCENE_CHANGE_TH);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FLD_DFT));
    pFlicker->ui8RShift = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_DFT, FLD_RSHIFT);
    pFlicker->ui8MinPN = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_DFT, FLD_MIN_PN);
    pFlicker->ui8PN = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_DFT, FLD_PN);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(FLD_RESET));
    pFlicker->bReset = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        FLD_RESET, FLD_RESET);

//    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_HIS(char *pMemory, const CI_MODULE_HIS *pHistograms)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pHistograms);
    CI_PDUMP_COMMENT("start");

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

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_HIS(const CI_SHOT *pShot, CI_MODULE_HIS *pHistograms)
{
    IMG_UINT32 tmp;
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pHistograms);
//    CI_PDUMP_COMMENT("start");

    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pHistograms, 0, sizeof(CI_MODULE_HIS));

    tmp = ReadMem(pMemory, HW_LOAD_OFF(HIS_INPUT_RANGE));
    pHistograms->ui16InputOffset = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_INPUT_RANGE, HIS_INPUT_OFFSET);
    pHistograms->ui16InputScale = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_INPUT_RANGE, HIS_INPUT_SCALE);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(HIS_REGION_GRID_START_COORDS));
    pHistograms->ui16Left = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_REGION_GRID_START_COORDS, HIS_REGION_GRID_COL_START);
    pHistograms->ui16Top = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_REGION_GRID_START_COORDS, HIS_REGION_GRID_ROW_START);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(HIS_REGION_GRID_TILE));
    pHistograms->ui16Width = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_REGION_GRID_TILE, HIS_REGION_GRID_TILE_WIDTH);
    pHistograms->ui16Height = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        HIS_REGION_GRID_TILE, HIS_REGION_GRID_TILE_HEIGHT);

//    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_WBS(char *pMemory, const CI_MODULE_WBS *pWhiteBalance)
{
    IMG_UINT32 tmp, i;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pWhiteBalance);
    CI_PDUMP_COMMENT("start");

    // configuration of each region of interest
    for (i = 0; i < WBS_NUM_ROI; i++)
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
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(WBS_MISC), tmp);

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_WBS(const CI_SHOT *pShot,
    CI_MODULE_WBS *pWhiteBalance)
{
    IMG_UINT32 tmp, i;
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pWhiteBalance);
//    CI_PDUMP_COMMENT("start");

    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pWhiteBalance, 0, sizeof(CI_MODULE_WBS));

    tmp = ReadMem(pMemory, HW_LOAD_OFF(WBS_MISC));
    pWhiteBalance->ui8ActiveROI = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        WBS_MISC, WBS_ROI_ACT);
    pWhiteBalance->ui16RGBOffset = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        WBS_MISC, WBS_RGB_OFFSET);
    pWhiteBalance->ui16YOffset = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        WBS_MISC, WBS_Y_OFFSET);

    /*if (pShot->eOutputConfig | CI_SAVE_WHITEBALANCE)
    {
        // 0 base register
        n = pWhiteBalance->ui8ActiveROI + 1;
    }*/
    // otherwise n is 0
    
    // configuration of each region of interest
    for (i = 0; i < WBS_NUM_ROI; i++)
    {
        tmp = ReadMem(pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_ROI_START_COORDS, i));
        pWhiteBalance->aRoiLeft[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_ROI_START_COORDS, WBS_ROI_COL_START);
        pWhiteBalance->aRoiTop[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_ROI_START_COORDS, WBS_ROI_ROW_START);

        tmp = ReadMem(pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_ROI_END_COORDS, i));
        pWhiteBalance->aRoiRight[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_ROI_END_COORDS, WBS_ROI_COL_END);
        pWhiteBalance->aRoiBottom[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_ROI_END_COORDS, WBS_ROI_ROW_END);

        tmp = ReadMem(pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_0, i));
        pWhiteBalance->aRMax[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_RGBY_TH_0, WBS_RMAX_TH);
        pWhiteBalance->aGMax[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_RGBY_TH_0, WBS_GMAX_TH);

        tmp = ReadMem(pMemory,
            REGIO_TABLE_OFF(FELIX_LOAD_STRUCTURE, WBS_RGBY_TH_1, i));
        pWhiteBalance->aBMax[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_RGBY_TH_1, WBS_BMAX_TH);
        pWhiteBalance->aYMax[i] = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
            WBS_RGBY_TH_1, WBS_YHLW_TH);
    }

//    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Load_ENS(char *pMemory, const CI_MODULE_ENS *pEncStats)
{
    IMG_UINT32 tmp;

    IMG_ASSERT(pMemory);
    IMG_ASSERT(pEncStats);
    CI_PDUMP_COMMENT("start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENS_NCOUPLES,
        ENS_NCOUPLES_EXP, pEncStats->ui8Log2NCouples);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENS_NCOUPLES), tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, ENS_REGION_REGS,
        ENS_SUBS_EXP, pEncStats->ui8SubExp);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(ENS_REGION_REGS), tmp);

    CI_PDUMP_COMMENT("done");
}

static void HW_CI_Revert_ENS(const CI_SHOT *pShot, CI_MODULE_ENS *pEncStats)
{
    IMG_UINT32 tmp;
    IMG_UINT32 n = 0;  // number of enabled ROI
    const IMG_UINT32 *pMemory = NULL;

    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pStatistics);
    IMG_ASSERT(pEncStats);
//    CI_PDUMP_COMMENT("start");
    pMemory = (IMG_UINT32*)pShot->pStatistics;
    IMG_MEMSET(pEncStats, 0, sizeof(CI_MODULE_ENS));

    tmp = ReadMem(pMemory, HW_LOAD_OFF(ENS_NCOUPLES));
    pEncStats->ui8Log2NCouples = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        ENS_NCOUPLES, ENS_NCOUPLES_EXP);

    tmp = ReadMem(pMemory, HW_LOAD_OFF(ENS_REGION_REGS));
    pEncStats->ui8SubExp = REGIO_READ_FIELD(tmp, FELIX_LOAD_STRUCTURE,
        ENS_REGION_REGS, ENS_SUBS_EXP);

//    CI_PDUMP_COMMENT("done");
}


/** @brief  Write AWS configuration to registers.
 *  Used here because CI unit tests need it. */
void HW_CI_Load_AWS(char *pMemory,
    const struct CI_MODULE_AWS *pWhiteBalance)
{
#ifdef AWS_AVAILABLE
    IMG_UINT32 tmp;

    CI_PDUMP_COMMENT("start");

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_GRID_START_COORDS,
        AWS_GRID_START_COL, pWhiteBalance->ui16GridStartColumn);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_GRID_START_COORDS,
        AWS_GRID_START_ROW, pWhiteBalance->ui16GridStartRow);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_GRID_START_COORDS),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_GRID_TILE,
        AWS_GRID_TILE_CFA_WIDTH_MIN1, pWhiteBalance->ui16GridTileWidth);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_GRID_TILE,
        AWS_GRID_TILE_CFA_HEIGHT_MIN1, pWhiteBalance->ui16GridTileHeight);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_GRID_TILE),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_LOG2_QEFF,
        AWS_LOG2_R_QEFF, pWhiteBalance->ui16Log2_R_Qeff);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_LOG2_QEFF,
        AWS_LOG2_B_QEFF, pWhiteBalance->ui16Log2_B_Qeff);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_LOG2_QEFF),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_0,
        AWS_R_DARK_THRESH, pWhiteBalance->ui16RedDarkThresh);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_0,
        AWS_G_DARK_THRESH, pWhiteBalance->ui16GreenDarkThresh);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_THRESHOLDS_0),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_1,
        AWS_B_DARK_THRESH, pWhiteBalance->ui16BlueDarkThresh);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_1,
        AWS_R_CLIP_THRESH, pWhiteBalance->ui16RedClipThresh);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_THRESHOLDS_1),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_2,
        AWS_G_CLIP_THRESH, pWhiteBalance->ui16GreenClipThresh);
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_THRESHOLDS_2,
        AWS_B_CLIP_THRESH, pWhiteBalance->ui16BlueClipThresh);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_THRESHOLDS_2),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_BB_DIST,
        AWS_BB_DIST, pWhiteBalance->ui16BbDist);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_BB_DIST),
        tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_LOAD_STRUCTURE, AWS_DEBUG_BITMAP,
        AWS_DEBUG_BITMAP_ENABLE, pWhiteBalance->bDebugBitmap ? 1 : 0);
    WriteMem((IMG_UINT32*)pMemory, HW_LOAD_OFF(AWS_DEBUG_BITMAP),
        tmp);

#else
    CI_PDUMP_COMMENT("AWS_NOT_AVAILABLE");
#endif
}

#endif /* FELIX_VERSION_MAJ == 2 */

IMG_UINT32 INT_CI_SizeLS_HW2(void)
{
#if FELIX_VERSION_MAJ == 2
    return FELIX_LOAD_STRUCTURE_BYTE_SIZE;
#else
    return 0;
#endif
}

IMG_RESULT INT_CI_Load_HW2(const INT_PIPELINE *pIntPipeline,
    char* loadstruct)
{
#if FELIX_VERSION_MAJ == 2
    const CI_PIPELINE *pPipeline = NULL;
    IMG_BOOL8 b2_4HW = IMG_FALSE;

    IMG_ASSERT(pIntPipeline);
    IMG_ASSERT(pIntPipeline->pConnection);
    IMG_ASSERT(loadstruct);

    if (FELIX_VERSION_MAJ
        != pIntPipeline->pConnection->publicConnection.sHWInfo.rev_ui8Major)
    {
        LOG_ERROR("Should only be called with HW %d.X\n", FELIX_VERSION_MAJ);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (pIntPipeline->pConnection->publicConnection.sHWInfo\
        .uiSizeOfLoadStruct != FELIX_LOAD_STRUCTURE_BYTE_SIZE)
    {
        // this means the driver is corrupted
        LOG_ERROR("Different load structure size from user/kernel\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pPipeline = &(pIntPipeline->publicPipeline);

    /*
     * note to customer: if your HW is 2.4 then this could be done directly
     * as const IMG_BOOL8 b2_4HW = IMG_TRUE
     *
     * If your HW is not 2.4 then the same with IMG_FALSE
     */
    if (2 == pIntPipeline->pConnection->publicConnection.sHWInfo.rev_ui8Major
        && 4 == pIntPipeline->pConnection->publicConnection.sHWInfo\
        .rev_ui8Minor)
    {
        b2_4HW = IMG_TRUE;
    }

    HW_CI_Load_BLC(loadstruct, &(pPipeline->sBlackCorrection));

    HW_CI_Load_RLT(loadstruct, &(pPipeline->sRawLUT));

    HW_CI_Load_LSH_WB(loadstruct, &(pPipeline->sWhiteBalance));

    HW_CI_Load_DNS(loadstruct, &(pPipeline->sDenoiser));

    HW_CI_Load_DPF(loadstruct, &(pPipeline->sDefectivePixels));

    HW_CI_Load_LCA(loadstruct, &(pPipeline->sChromaAberration));

    HW_CI_Load_WBC(loadstruct, &(pPipeline->sWhiteBalance));

    HW_CI_Load_CCM(loadstruct, &(pPipeline->sColourCorrection));

    HW_CI_Load_MGM(loadstruct, &(pPipeline->sMainGamutMapper));

    HW_CI_Load_GMA(loadstruct, &(pPipeline->sGammaCorrection));

    HW_CI_Load_R2Y(loadstruct, &(pPipeline->sRGBToYCC));

    HW_CI_Load_MIE(loadstruct, &(pPipeline->sImageEnhancer));
    if (b2_4HW)
    {
        HW_CI_Load_MIE_2_4(loadstruct, &(pPipeline->sImageEnhancer));
    }

    HW_CI_Load_TNM(loadstruct, &(pPipeline->sToneMapping));

    HW_CI_Load_SHA(loadstruct, &(pPipeline->sSharpening));
    if (b2_4HW)
    {
        HW_CI_Load_SHA_2_4(loadstruct, &(pPipeline->sSharpening));
    }

    HW_CI_Load_Scaler(loadstruct, &(pPipeline->sEncoderScaler));

    HW_CI_Load_Scaler(loadstruct, &(pPipeline->sDisplayScaler));

    HW_CI_Load_Y2R(loadstruct, &(pPipeline->sYCCToRGB));

    HW_CI_Load_DGM(loadstruct, &(pPipeline->sDisplayGamutMapper));

    // statistics

    HW_CI_Load_EXS(loadstruct, &(pPipeline->stats.sExposureStats));

    HW_CI_Load_FOS(loadstruct, &(pPipeline->stats.sFocusStats));

    HW_CI_Load_WBS(loadstruct, &(pPipeline->stats.sWhiteBalanceStats));

    HW_CI_Load_AWS(loadstruct,
        &(pPipeline->stats.sAutoWhiteBalanceStats));

    HW_CI_Load_HIS(loadstruct, &(pPipeline->stats.sHistStats));

    HW_CI_Load_FLD(loadstruct, &(pPipeline->stats.sFlickerStats));

    // ENS is not loaded by the LL in user-space

    return IMG_SUCCESS;
#else /* FELIX_VERSION_MAJ == 2 */
    return IMG_ERROR_NOT_SUPPORTED;
#endif
}

IMG_RESULT INT_CI_Revert_HW2(const CI_SHOT *pShot, CI_PIPELINE_STATS *pStats)
{
#if FELIX_VERSION_MAJ == 2
    IMG_ASSERT(pShot);
    IMG_ASSERT(pShot->pLoadStruct);
    IMG_ASSERT(pStats);

    HW_CI_Revert_EXS(pShot, &(pStats->sExposureStats));

    HW_CI_Revert_FOS(pShot, &(pStats->sFocusStats));

    HW_CI_Revert_WBS(pShot, &(pStats->sWhiteBalanceStats));

    HW_CI_Revert_HIS(pShot, &(pStats->sHistStats));

    HW_CI_Revert_FLD(pShot, &(pStats->sFlickerStats));

    HW_CI_Revert_ENS(pShot, &(pStats->sEncStats));

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif
}


#ifdef __cplusplus
}
#endif
