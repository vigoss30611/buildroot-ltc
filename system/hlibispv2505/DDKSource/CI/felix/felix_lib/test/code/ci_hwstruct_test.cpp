/**
******************************************************************************
 @file ci_hwstruct_test.cpp

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
#include <gtest/gtest.h>

#include <cstdlib>
#include <cstdio>

#include "unit_tests.h"
#include "felix.h"

#include "img_defs.h"

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"
#include "ci_kernel/ci_kernel.h"

#include <registers/context0.h>
#include <hw_struct/ctx_pointers.h>
#include <hw_struct/ctx_config.h>
#include <hw_struct/save.h>

#include <tal.h>
#include <reg_io2.h>

#include "ci_kernel/ci_hwstruct.h"

#include "felix.h"
#include "unit_tests.h"

/**
 * @brief clip value in the CI_MODULE_* structure
 */
#define CLIP_REG_FIELD(val, group, reg, field) \
    ((val) & (group##_##reg##_##field##_LSBMASK))

/*
 * @brief read register field value from memory buffer, clip and shift
 */
#define READ_MEM_FIELD(mem, group, reg, field) \
    ((*(IMG_UINT32*)((IMG_UINT8*)mem+ group##_##reg##_OFFSET ) & \
     group##_##reg##_##field##_MASK ) >> group##_##reg##_##field##_SHIFT ); \
    /* check register alignment */ \
    IMG_STATIC_ASSERT( \
        group##_##reg##_OFFSET % sizeof(IMG_UINT32) == 0, \
            check_ptr_##group##_##reg );

/**
 * @brief verify if register field in memory is equal to struct field
 */
#define TEST_MEM_FIELD(name, group, reg, field) \
    { \
        IMG_UINT32 val =  READ_MEM_FIELD(&pConfig, group, reg, field); \
        EXPECT_EQ(val, CLIP_REG_FIELD(name, group, reg, field)); \
    }

/**
 * @brief read an array register from context and verify it matches val,
 *
 * Expected: IMG_HANDLE ctxbank to exist (context to read from)
 */
#define TEST_CTX_ARRAY_FIELD(val, i, reg, field) \
    { \
        IMG_UINT32 regval = 0; \
        IMG_UINT32 offset = REGIO_TABLE_OFF(FELIX_CONTEXT0, reg, i); \
        IMG_UINT32 fieldval = 0; \
        TALREG_ReadWord32(ctxbank, offset, &regval); \
        fieldval = REGIO_READ_FIELD(regval, FELIX_CONTEXT0, reg, field); \
        EXPECT_EQ(fieldval, CLIP_REG_FIELD(val, FELIX_CONTEXT0, reg, field)); \
    }

class HW_Load: public FullFelix, public ::testing::Test
{
public:
    char pConfig[FELIX_LOAD_STRUCTURE_BYTE_SIZE];

    HW_Load(): FullFelix()
    {
        IMG_MEMSET(pConfig, 0, FELIX_LOAD_STRUCTURE_BYTE_SIZE);
    }

    void SetUp()
    {
        configure(32, 32, YVU_420_PL12_8, RGB_888_24, false, 1);
    }

    void randomize(IMG_UINT8* buffer, const IMG_SIZE size)
    {
        // 'randomize' the buffer using known seed
        std::srand(0x12563478);
        for(IMG_SIZE i=0;i<size;++i)
        {
            buffer[i] = std::rand()&0xff;
        }
    }
};

#warning " [GM] Check if LS is verified in user-space"
#if 0  // done in user-space
TEST_F(HW_Load, BLC)
{
    /// @ put values & check results
    HW_CI_Load_BLC(pConfig, &(pPipeline->sBlackCorrection));
}

TEST_F(HW_Load, RLT)
{
    /// @ put values & check results
    HW_CI_Load_RLT(pConfig, &(pPipeline->sRawLUT));
}

TEST_F(HW_Load, LSH_Deshading)
{
    /// @ put values & check results
    //HW_CI_Load_LSH_Matrix(pConfig, &(pPipeline->));
}

TEST_F(HW_Load, colourConv_YCC)
{
    /// @ put values & check results
    HW_CI_Load_Y2R(pConfig, &(pPipeline->sYCCToRGB));
}

TEST_F(HW_Load, colourConv_RGB)
{
    /// @ put values & check results
    HW_CI_Load_R2Y(pConfig, &(pPipeline->sRGBToYCC));
}

TEST_F(HW_Load, LSH)
{
    /// @ put values & check results
    HW_CI_Load_LCA(pConfig, &(pPipeline->sChromaAberration));
}

TEST_F(HW_Load, CCM)
{
    /// @ put values & check results
    HW_CI_Load_CCM(pConfig, &(pPipeline->sColourCorrection));
}

TEST_F(HW_Load, TNM)
{
    /// @ put values & check results
    HW_CI_Load_TNM(pConfig, &(pPipeline->sToneMapping));
}

TEST_F(HW_Load, DSC)
{
    /// @ put values & check results
    HW_CI_Load_Scaler(pConfig, &(pPipeline->sDisplayScaler));
}

TEST_F(HW_Load, ESC)
{
    /// @ put values & check results
    HW_CI_Load_Scaler(pConfig, &(pPipeline->sEncoderScaler));
}
#endif /* 0 */

#ifdef AWS_AVAILABLE
TEST_F(HW_Load, AWS_LL)
{
    // Prepare the seeded random data.
    // The fields, after being clipped,  would be eventually compared to
    // respective pConfig memory locations
    // note : this causes "does not fit in" warnings to be printed on console
    // but we don't worry about it, the values are clipped before comparison
    randomize(reinterpret_cast<IMG_UINT8*>(&pPipeline->stats.sAutoWhiteBalanceStats),
        sizeof(pPipeline->stats.sAutoWhiteBalanceStats));


    HW_CI_Load_AWS(pConfig, &(pPipeline->stats.sAutoWhiteBalanceStats));
    CI_MODULE_AWS* pS = &pPipeline->stats.sAutoWhiteBalanceStats;

    TEST_MEM_FIELD(pS->ui16GridStartColumn,
            FELIX_LOAD_STRUCTURE,
            AWS_GRID_START_COORDS,
            AWS_GRID_START_COL);

    TEST_MEM_FIELD(pS->ui16GridStartRow,
            FELIX_LOAD_STRUCTURE,
            AWS_GRID_START_COORDS,
            AWS_GRID_START_ROW);

    TEST_MEM_FIELD(pS->ui16GridTileWidth,
            FELIX_LOAD_STRUCTURE,
            AWS_GRID_TILE,
            AWS_GRID_TILE_CFA_WIDTH_MIN1);

    TEST_MEM_FIELD(pS->ui16GridTileHeight,
            FELIX_LOAD_STRUCTURE,
            AWS_GRID_TILE,
            AWS_GRID_TILE_CFA_HEIGHT_MIN1);

    TEST_MEM_FIELD(pS->ui16Log2_R_Qeff,
            FELIX_LOAD_STRUCTURE,
            AWS_LOG2_QEFF,
            AWS_LOG2_R_QEFF);

    TEST_MEM_FIELD(pS->ui16Log2_B_Qeff,
            FELIX_LOAD_STRUCTURE,
            AWS_LOG2_QEFF,
            AWS_LOG2_B_QEFF);

    TEST_MEM_FIELD(pS->ui16RedDarkThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_0,
            AWS_R_DARK_THRESH);

    TEST_MEM_FIELD(pS->ui16GreenDarkThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_0,
            AWS_G_DARK_THRESH);

    TEST_MEM_FIELD(pS->ui16BlueDarkThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_1,
            AWS_B_DARK_THRESH);

    TEST_MEM_FIELD(pS->ui16RedClipThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_1,
            AWS_R_CLIP_THRESH);

    TEST_MEM_FIELD(pS->ui16GreenClipThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_2,
            AWS_G_CLIP_THRESH);

    TEST_MEM_FIELD(pS->ui16BlueClipThresh,
            FELIX_LOAD_STRUCTURE,
            AWS_THRESHOLDS_2,
            AWS_B_CLIP_THRESH);

    TEST_MEM_FIELD(pS->ui16BbDist,
            FELIX_LOAD_STRUCTURE,
            AWS_BB_DIST,
            AWS_BB_DIST);

    const IMG_INT32 flag = pS->bDebugBitmap ? 1 : 0;
    TEST_MEM_FIELD(flag,
            FELIX_LOAD_STRUCTURE,
            AWS_DEBUG_BITMAP,
            AWS_DEBUG_BITMAP_ENABLE);
}

#ifdef AWS_LINE_SEG_AVAILABLE
TEST_F(HW_Load, AWS_Reg)
{
    int i;
    KRN_CI_PIPELINE *pKrnPipe = getKrnPipeline();
    IMG_HANDLE ctxbank = g_psCIDriver->sTalHandles.hRegFelixContext[0];
    CI_AWS_CURVE_COEFFS *pS = &(pKrnPipe->awsConfig);

    randomize(reinterpret_cast<IMG_UINT8*>(pS), sizeof(CI_AWS_CURVE_COEFFS));
    HW_CI_Reg_AWS(pKrnPipe);

    for (i = 0; i < AWS_LINE_SEG_N; i++)
    {
        TEST_CTX_ARRAY_FIELD(pS->aCurveCoeffX[i], i,
            AWS_CURVE_COEFF, AWS_CURVE_X_COEFF);
        TEST_CTX_ARRAY_FIELD(pS->aCurveCoeffY[i], i,
            AWS_CURVE_COEFF, AWS_CURVE_Y_COEFF);
        TEST_CTX_ARRAY_FIELD(pS->aCurveOffset[i], i,
            AWS_CURVE_OFFSET, AWS_CURVE_OFFSET);
        TEST_CTX_ARRAY_FIELD(pS->aCurveBoundary[i], i,
            AWS_CURVE_BOUNDARY, AWS_CURVE_BOUNDARY);
    }
}
#endif /* AWS_LINE_SEG_AVAILABLE */
#endif /* AWS_AVAILABLE */
