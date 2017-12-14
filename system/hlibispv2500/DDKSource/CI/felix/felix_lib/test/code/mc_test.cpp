/**
 ******************************************************************************
 @file mc_tests.cpp

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
#include <img_types.h>
#include <img_defs.h>

#include "felix.h"

#include "ci/ci_modules.h"
#include "mc/module_config.h"
#include "ci_kernel/ci_kernel.h"
#include "felix.h"

#include <gtest/gtest.h>
#include <ctx_reg_precisions.h>

#include <cmath>
#include <algorithm>

TEST(FixedPoint, IMG_Fix_Clip_u32)
{
#define INTEGER 6
#define FRACT 10
#define SIGNED IMG_FALSE
    IMG_UINT32 v[] = {
        0,
        1 << FRACT,
        16896 };
    MC_FLOAT f[] = {
        0.0f,
        1.0f,
        16.5f };
    const double prec = 1.0 / (1 << FRACT);
    MC_FLOAT tf = 0;
    int i;

    ASSERT_EQ(sizeof(v) / sizeof(v[0]), sizeof(f) / sizeof(f[0]))
        << "badly designed test";

    for (i = 0; i < sizeof(v) / sizeof(v[0]); i++)
    {
        IMG_UINT32 ti = IMG_Fix_Clip(f[i], INTEGER, FRACT, SIGNED, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, SIGNED, "test");
        EXPECT_EQ((IMG_UINT32)v[i], ti)
            << "failed to convert to ui32[" << i << "] " << f[i];
        EXPECT_NEAR(f[i], tf, prec)
            << "failed to revert ui32[" << i << "] " << ti;
    }

#undef INTEGER
#undef FRACT
#undef SIGNED
}

TEST(FixedPoint, IMG_Fix_Clip_s32)
{
#define INTEGER 6
#define FRACT 10
#define SIGNED IMG_TRUE
    IMG_INT32 v[] = {
        0,
        1 << FRACT,
        16896,
        -32410 };
    MC_FLOAT f[] = {
        0.0f,
        1.0f,
        16.5f,
        -31.65f };
    const double prec = 1.0 / (1 << FRACT);
    MC_FLOAT tf = 0;
    int i;

    ASSERT_EQ(sizeof(v) / sizeof(v[0]), sizeof(f) / sizeof(f[0]))
        << "badly designed test";

    for (i = 0; i < sizeof(v) / sizeof(v[0]); i++)
    {
        IMG_INT32 ti = IMG_Fix_Clip(f[i], INTEGER, FRACT, SIGNED, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, SIGNED, "test");
        EXPECT_EQ(v[i], ti)
            << "failed to convert to i32[" << i << "] " << f[i];
        EXPECT_NEAR(f[i], tf, prec)
            << "failed to revert i32[" << i << "] " << ti;
    }

#undef INTEGER
#undef FRACT
#undef SIGNED
}

TEST(FixedPoint, IMG_Fix_Clip_u16)
{
#define INTEGER 6
#define FRACT 10
#define SIGNED IMG_FALSE
    IMG_INT16 v[] = {
        0,
        1 << FRACT,
        16896 };
    MC_FLOAT f[] = {
        0.0f,
        1.0f,
        16.5f };
    const double prec = 1.0 / (1 << FRACT);
    MC_FLOAT tf = 0;
    int i;

    ASSERT_EQ(sizeof(v) / sizeof(v[0]), sizeof(f) / sizeof(f[0]))
        << "badly designed test";

    for (i = 0; i < sizeof(v) / sizeof(v[0]); i++)
    {
        IMG_INT16 ti = IMG_Fix_Clip(f[i], INTEGER, FRACT, SIGNED, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, SIGNED, "test");
        EXPECT_EQ(v[i], ti)
            << "failed to convert ui16[" << i << "] " << f[i];
        EXPECT_NEAR(f[i], tf, prec)
            << "failed to revert ui16[" << i << "] " << ti;
    }

#undef INTEGER
#undef FRACT
#undef SIGNED
}

TEST(FixedPoint, IMG_Fix_Clip_s16)
{
#define INTEGER 6
#define FRACT 10
#define SIGNED IMG_TRUE
    IMG_UINT16 v[] = {
        0,
        1 << FRACT,
        16896,
        -32410 };
    MC_FLOAT f[] = {
        0.0f,
        1.0f,
        16.5f,
        -31.65f };
    const double prec = 1.0 / (1 << FRACT);
    MC_FLOAT tf = 0;
    int i;

    ASSERT_EQ(sizeof(v) / sizeof(v[0]), sizeof(f) / sizeof(f[0]))
        << "badly designed test";

    for (i = 0; i < sizeof(v) / sizeof(v[0]); i++)
    {
        IMG_UINT16 ti = IMG_Fix_Clip(f[i], INTEGER, FRACT, SIGNED, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, SIGNED, "test");
        EXPECT_EQ(v[i], ti)
            << "failed to convert i16[" << i << "] " << f[i];
        EXPECT_NEAR(f[i], tf, prec)
            << "failed to revert i16[" << i << "] " << ti;
    }

#undef INTEGER
#undef FRACT
#undef SIGNED
}

TEST(FixedPoint, IMG_Fix_Clip_8b)
{
#define INTEGER 3
#define FRACT 2
    MC_FLOAT f = 0.0f;
    MC_FLOAT tf;
    const double prec = 1.0 / (1 << FRACT);

    {
        IMG_UINT8 v = 27;
        f = 6.75f;
        IMG_UINT8 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_FALSE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_FALSE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_NEAR(f, tf, prec);
    }

    {
        IMG_INT8 v = -5;
        f = -1.25f;
        IMG_INT8 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_TRUE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_TRUE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_NEAR(f, tf, prec);
    }

#undef INTEGER
#undef FRACT
}

// test negative fractional bits
TEST(FixedPoint, IMG_Fix_Clip_neg_frac)
{
#define INTEGER 7
#define FRACT -2
    MC_FLOAT f = 0.0f;
    MC_FLOAT tf;

    {
        IMG_UINT16 v = 27;
        f = 108.0f;
        IMG_UINT16 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_FALSE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_FALSE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_EQ(f, tf);
    }

    {
        IMG_INT16 v = -5;
        f = -20.0f;
        IMG_INT16 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_TRUE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_TRUE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_EQ(f, tf);
    }

#undef INTEGER
#undef FRACT
}

// test negative integer bits
TEST(FixedPoint, IMG_Fix_Clip_neg_int)
{
#define INTEGER -2
#define FRACT 7
    MC_FLOAT f = 0.0f;
    MC_FLOAT tf;
    const double prec = 1.0 / (1 << FRACT);

    {
        IMG_UINT16 v = 13;
        f = 0.10f;
        IMG_UINT16 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_FALSE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_FALSE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_NEAR(f, tf, prec);
    }

    {
        IMG_INT16 v = -5;
        f = -0.0390625f;
        IMG_INT16 ti = IMG_Fix_Clip(f, INTEGER, FRACT, IMG_TRUE, "test");
        tf = IMG_Fix_Revert(ti, INTEGER, FRACT, IMG_TRUE, "test");
        EXPECT_EQ(v, ti);
        EXPECT_NEAR(f, tf, prec);
    }

#undef INTEGER
#undef FRACT
}

TEST(MC, HISExtract)
{
    FILE *f = NULL;
    IMG_UINT32 *pSaveStruct = NULL;
    int b = 0;

    pSaveStruct = (IMG_UINT32*)calloc(FELIX_SAVE_BYTE_SIZE, 1);
    ASSERT_TRUE(pSaveStruct != NULL);

    for (b = 0; b < FELIX_SAVE_BYTE_SIZE / sizeof(IMG_UINT32); b++)
    {
        pSaveStruct[b] = b;
    }

    MC_STATS_HIS sMCStat;

    EXPECT_EQ(IMG_SUCCESS, MC_HISExtract(pSaveStruct, &sMCStat));

    b = 0;
    for (int column = 0; column < HIS_REGION_HTILES; column++)
    {
        for (int row = 0; row < HIS_REGION_VTILES; row++)
        {
            for (int bin = 0; bin < HIS_REGION_BINS; bin++)
            {
                b = FELIX_SAVE_HIS_REGION_0_0_BINS_OFFSET / sizeof(IMG_UINT32)
                    + ((row*HIS_REGION_HTILES) + column)*HIS_REGION_BINS
                    + bin;
                EXPECT_EQ(b, sMCStat.regionHistograms[row][column][bin]);
            }
        }
    }

    free(pSaveStruct);
}

TEST(MC, EXSExtract)
{
    FILE *f = NULL;
    IMG_UINT16 *pSaveStruct = NULL;
    int b = 0;

    pSaveStruct = (IMG_UINT16*)calloc(FELIX_SAVE_BYTE_SIZE, 1);
    ASSERT_TRUE(pSaveStruct != NULL);

    for (b = 0; b < FELIX_SAVE_BYTE_SIZE / sizeof(IMG_UINT16); b++)
    {
        pSaveStruct[b] = b;
    }

    MC_STATS_EXS sMCStat;

    EXPECT_EQ(IMG_SUCCESS, MC_EXSExtract(pSaveStruct, &sMCStat));

    unsigned char stride = (FELIX_SAVE_EXS_TILE_CC01_0_STRIDE
        + FELIX_SAVE_EXS_REGION_RESERVED_0_STRIDE) / 2;

    // divided by 2 to express it in 16bit words
    unsigned char strideTile = FELIX_SAVE_EXS_TILE_CC01_0_STRIDE / 2;
    // divided by 2 to express it in 16bit words
    unsigned char strideRow = FELIX_SAVE_EXS_TILE_CC01_0_STRIDE
        *HIS_REGION_HTILES / 2;
    // divided by 2 to express it in 16bit words
    unsigned char strideReserved = FELIX_SAVE_EXS_REGION_RESERVED_0_STRIDE
        *FELIX_SAVE_EXS_REGION_RESERVED_0_NO_ENTRIES / 2;

    b = 0;
    for (int row = 0; row < EXS_V_TILES; row++)
    {
        for (int column = 0; column < EXS_H_TILES; column++)
        {
            for (int channel = 0; channel < 4; channel++)
            {
                b = FELIX_SAVE_EXS_TILE_CC01_0_OFFSET / sizeof(IMG_UINT16) +
                    row*(strideRow + strideReserved) + column*strideTile
                    + channel;
                EXPECT_EQ(pSaveStruct[b],
                    sMCStat.regionClipped[row][column][channel]);
            }
        }
    }

    free(pSaveStruct);
}

TEST(LSH, load_bin)
{
    LSH_GRID sLSH;
    double prec = 0.001;

    memset(&sLSH, 0, sizeof(LSH_GRID));

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, LSH_Load_bin(&(sLSH),
        "testdata/deshading_diag_old.lsh"));

    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sLSH),
        "testdata/deshading_diag.lsh"));

    EXPECT_EQ(21, sLSH.ui16Width);
    EXPECT_EQ(16, sLSH.ui16Height);
    EXPECT_EQ(16, sLSH.ui16TileSize);
    for (int c = 0; c < 4; c++)
    {
        EXPECT_TRUE(sLSH.apMatrix[c] != NULL) << "channel " << c << " is NULL";
        double base = 1.0*(c + 1);

        for (int y = 0; y < sLSH.ui16Height && !HasFailure(); y++)
        {
            for (int x = 0; x < sLSH.ui16Width && !HasFailure(); x++)
            {
                double expected = std::max(0.0, base - (x)*0.1 - (y)*0.1);
                EXPECT_NEAR(expected, sLSH.apMatrix[c][y*sLSH.ui16Width + x],
                    prec);
            }
        }
    }

    LSH_Free(&(sLSH));
}

/**
 * @brief Tests that saving the LSH grid as binary does not alterate it
 *
 * @note assumes LSH_Load_bin() works
 */
TEST(LSH, save_bin)
{
    int c, y, x;
    int imgWidth = 640 / CI_CFA_WIDTH, imgHeight = 480 / CI_CFA_HEIGHT;
    LSH_GRID sGrid;

    memset(&sGrid, 0, sizeof(LSH_GRID));

    ASSERT_EQ(IMG_SUCCESS, LSH_CreateMatrix(&sGrid, imgWidth, imgHeight, 16));
    EXPECT_EQ(21, sGrid.ui16Width);
    EXPECT_EQ(16, sGrid.ui16Height);
    EXPECT_EQ(16, sGrid.ui16TileSize);

    for (c = 0; c < 4; c++)
    {
        float base = 1.0f*(c + 1);
        for (y = 0; y < sGrid.ui16Height; y++)
        {
            for (x = 0; x < sGrid.ui16Width; x++)
            {
                sGrid.apMatrix[c][y*sGrid.ui16Width + x] = std::max(0.0f,
                    (float)(base - (x)*0.1 - (y)*0.1));
            }
        }
    }

    EXPECT_EQ(IMG_SUCCESS, LSH_Save_bin(&sGrid, "deshading_out.lsh"));

    if (!HasFailure())
    {
        double prec = 0.001;
        LSH_GRID sCheck;

        memset(&sCheck, 0, sizeof(LSH_GRID));

        ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sCheck), "deshading_out.lsh"));
        EXPECT_EQ(sGrid.ui16Width, sCheck.ui16Width);
        EXPECT_EQ(sGrid.ui16Height, sCheck.ui16Height);
        EXPECT_EQ(sGrid.ui16TileSize, sCheck.ui16TileSize);

        for (c = 0; c < 4; c++)
        {
            for (y = 0; y < sCheck.ui16Height; y++)
            {
                for (x = 0; x < sCheck.ui16Width && !HasFailure(); x++)
                {
                    EXPECT_NEAR(sGrid.apMatrix[c][y*sGrid.ui16Width + x],
                        sCheck.apMatrix[c][y*sGrid.ui16Width + x], prec)
                        << "c=" << c << " x=" << x << " y=" << y;
                }
            }
        }

        LSH_Free(&sCheck);
    }
    LSH_Free(&sGrid);
}

TEST(MC, LSHConvert)
{
    MC_LSH sMC_LSH;
    CI_PIPELINE sPipe;

    MC_LSHInit(&sMC_LSH);

    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sMC_LSH.sGrid),
        "testdata/deshading_diag.lsh"));
    sMC_LSH.bUseDeshadingGrid = IMG_TRUE;

    memset(&(sPipe.sDeshading), 0, sizeof(CI_MODULE_LSH));

    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvert(&sMC_LSH, &(sPipe.sDeshading)));

    for (int c = 0; c < 4; c++)
    {
        EXPECT_TRUE(sMC_LSH.sGrid.apMatrix[c] != NULL)
            << "channel " << c << " is NULL";
        double base = 1.0*(c + 1);
        int x, y;
        double expectedF;
        IMG_INT32 prev, added, expectedI, diff;

        EXPECT_TRUE(sPipe.sDeshading.matrixDiff[c] != NULL)
            << "channel " << c << " has NULL diff";
        EXPECT_TRUE(sPipe.sDeshading.matrixStart[c] != NULL)
            << "channel " << c << " has NULL start";

        for (y = 0; y < sMC_LSH.sGrid.ui16Height && !HasFailure(); y++)
        {
            x = 0;
            expectedF = std::max(0.0, base - (x)*0.1 - (y)*0.1);
            expectedI = IMG_Fix_Clip(expectedF, PREC(LSH_VERTEX));

            prev = sPipe.sDeshading.matrixStart[c][y];
            EXPECT_EQ(expectedI, prev)
                << "first value for line c=" << c << " y=" << y
                << " is wrong - f=" << expectedF;

            for (x = 0; x < sMC_LSH.sGrid.ui16Width - 1 && !HasFailure(); x++)
            {
                expectedF = std::max(0.0, base - (x + 1)*0.1 - (y)*0.1);
                expectedI = IMG_Fix_Clip(expectedF, PREC(LSH_VERTEX));

                diff = sPipe.sDeshading.matrixDiff[c]\
                    [y*(sMC_LSH.sGrid.ui16Width - 1) + x];

                added = prev + diff;
                EXPECT_EQ(expectedI - prev, diff)
                    << "difference check for {" << c << "}(" << x + 1
                    << "," << y << ") - f=" << expectedF;

                EXPECT_EQ(expectedI, added)
                    << "value check for {" << c << "}(" << x + 1 << ","
                    << y << ") - f=" << expectedF;
                EXPECT_LE(added, 1 << (LSH_VERTEX_INT + LSH_VERTEX_FRAC))
                    << "value overflowed!";
                EXPECT_GE(added, 0) << "value is negative!";

                prev = added;
            }
        }
    }

    LSH_Free(&(sMC_LSH.sGrid));
    CI_PipelineFreeLSH(&(sPipe.sDeshading));
}

void LSH_Update(const MC_LSH &sMC_LSH, FullFelix &driver)
{
    int fail;
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, CI_UPD_ALL,
        &fail));

    /*  check that the matrix was converted to HW format correctly
     * for different bits per diff */
    int gridWidth = driver.pPipeline->sImagerInterface.ui16ImagerSize[0]
        / (1 << driver.pPipeline->sDeshading.ui8TileSizeLog2);
    KRN_CI_PIPELINE *pKRNPipeline = driver.getKrnPipeline();
    int n16BDIff = pKRNPipeline->ui16LSHLineSize + 1;
    int diffStride = pKRNPipeline->uiLSHMatrixStride;
    IMG_UINT8* matrix =
        (IMG_UINT8*)Platform_MemGetMemory(&(pKRNPipeline->sDeshadingGrid));

    ASSERT_TRUE(matrix != NULL);
    EXPECT_EQ(sMC_LSH.sGrid.ui16TileSize,
        1 << (driver.pPipeline->sDeshading.ui8TileSizeLog2));
    EXPECT_LE(gridWidth, sMC_LSH.sGrid.ui16Width);
    ASSERT_LT(n16BDIff * 2, diffStride)
        << "nb of 16B differences per line is bigger than stride!";

    IMG_INT32 *differences = (IMG_INT32*)calloc(gridWidth - 1,
        sizeof(IMG_INT32));

    for (int c = 0; c < 4; c++)
    {
        double base = 1.0*(c + 1);
        int x = 0, y = 0;
        double expectedF;
        int lineStart = 0; // in bytes
        IMG_INT32 prev, added, expectedI, diff;

        for (y = 0;
            y < sMC_LSH.sGrid.ui16Height && !testing::Test::HasFailure();
            y++)
        {
            x = 0;
            expectedF =
                sMC_LSH.sGrid.apMatrix[c][y*sMC_LSH.sGrid.ui16Width + x];
            expectedI = IMG_Fix_Clip(expectedF, PREC(LSH_VERTEX));

            lineStart = y * 4 * diffStride + c*diffStride;
            prev = *((IMG_UINT16*)(&matrix[lineStart]));

            EXPECT_EQ(expectedI, prev)
                << "first value for line c=" << c << " y=" << y
                << " is wrong - f=" << expectedF << " - diffStride="
                << diffStride << " lineStart=" << lineStart;

            // only uses grid-1
            memset(differences, 0, (gridWidth - 1)*sizeof(IMG_INT32));

            int nbits = 0; // nb of bits loaded
            int nDiff = 0; // nb of diff loaded
            int bitsPerDiff =
                pKRNPipeline->userPipeline.sDeshading.ui8BitsPerDiff;
            IMG_UINT32 mask = ((1 << bitsPerDiff) - 1);
            IMG_INT32 loaded = 0;
            int packedI = 0;

            // extract the differences
            while (nDiff < (gridWidth - 1))
            {
                // skip the 1st 2 Bytes and load 16b (max bit per diff is 10)
                loaded |=
                    (*((IMG_UINT16*)(&matrix[lineStart + 2 + 2 * packedI])))
                    << nbits;
                packedI++;  // next time load the next 16b
                nbits += 16;

                while (nbits >= bitsPerDiff && nDiff < (gridWidth - 1))
                {
                    differences[nDiff] = (loaded&mask);
                    loaded = loaded >> bitsPerDiff;
                    nbits -= bitsPerDiff;
                    if (differences[nDiff] >= 1 << (bitsPerDiff - 1))
                    {
                        // it should be a negative value!
                        differences[nDiff] = differences[nDiff]
                            - (1 << bitsPerDiff);
                    }
                    nDiff++;
                }
            }

            for (x = 0; x < gridWidth - 1 && !testing::Test::HasFailure(); x++)
            {
                expectedF = sMC_LSH.sGrid.apMatrix[c]\
                    [y*sMC_LSH.sGrid.ui16Width + x + 1];
                expectedI = IMG_Fix_Clip(expectedF, PREC(LSH_VERTEX));

                diff = differences[x];
                EXPECT_EQ(driver.pPipeline->sDeshading.matrixDiff[c]\
                    [y*(driver.pPipeline->sDeshading.ui16Width - 1) + x],
                    diff);

                added = prev + diff;
                EXPECT_EQ(expectedI - prev, diff)
                    << "difference check for {" << c << "}(" << x + 1 << ","
                    << y << ") - f=" << expectedF;

                EXPECT_EQ(expectedI, added)
                    << "value check for {" << c << "}(" << x + 1 << ","
                    << y << ") - f=" << expectedF;

                prev = added;
            }
        }
    }

    if (differences)
    {
        free(differences);
    }
}

/**
 * @brief Tests the convertion to HW format with several number of bits per
 * differences
 */
TEST(LSH, KRNConvert)
{
    FullFelix driver;
    MC_LSH sMC_LSH;

    driver.configure(640, 480, PXL_NONE, RGB_888_32, false, 1);

    MC_LSHInit(&sMC_LSH);

    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sMC_LSH.sGrid),
        "testdata/deshading_diag.lsh"));  // 20x20 32t
    sMC_LSH.bUseDeshadingGrid = IMG_TRUE;

    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvert(&sMC_LSH,
        &(driver.pPipeline->sDeshading)));

    LSH_Update(sMC_LSH, driver);

    LSH_Free(&(sMC_LSH.sGrid));
}

TEST(LSH, KRNConvert_multiBit)
{
    FullFelix driver;
    MC_LSH sMC_LSH;

    driver.configure(640, 480, PXL_NONE, RGB_888_32, false, 1);

    MC_LSHInit(&sMC_LSH);

    ASSERT_EQ(IMG_SUCCESS, LSH_CreateMatrix(&(sMC_LSH.sGrid),
        640 / CI_CFA_WIDTH, 480 / CI_CFA_WIDTH, 8));

    float aCorner[5] = { 1.0f, 0.8f, 0.7f, 0.6f, 1.0f };
    for (int c = 0; c < 4; c++)
    {
        EXPECT_EQ(IMG_SUCCESS, LSH_FillLinear(&(sMC_LSH.sGrid), c, aCorner));
    }
    sMC_LSH.bUseDeshadingGrid = IMG_TRUE;

    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvert(&sMC_LSH,
        &(driver.pPipeline->sDeshading)));
    do
    {
        std::cout << "testing delta "
            << (int)driver.pPipeline->sDeshading.ui8BitsPerDiff << std::endl;
        LSH_Update(sMC_LSH, driver);
        driver.pPipeline->sDeshading.ui8BitsPerDiff++;

    } while (driver.pPipeline->sDeshading.ui8BitsPerDiff <= LSH_DELTA_BITS_MAX
        && !HasFailure());

    LSH_Free(&(sMC_LSH.sGrid));
}

TEST(MC, PipelineInit)
{
    MC_PIPELINE sConfig;
    Felix driver;

    driver.configure();
    ASSERT_TRUE(driver.getConnection() != NULL);
    IMG_MEMSET(&sConfig, 0, sizeof(MC_PIPELINE));

    // expects IIF to be set-up before init
    //sConfig.sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    //sConfig.sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    EXPECT_NE(IMG_SUCCESS, MC_PipelineInit(&sConfig,
        &(driver.getConnection()->sHWInfo)));
}

/** @brief test convertion to CI and reverting */
TEST(MC, EXSRevert)
{
    MC_IIF sIIF;
    MC_EXS ori, reverted;
    CI_MODULE_EXS converted;
    IMG_UINT32 config = 0;

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_EXSInit(&ori, &sIIF));

    ori.bRegionEnable = IMG_TRUE;
    ori.bGlobalEnable = IMG_FALSE;
    ori.ui16Left = 80;
    ori.ui16Width = 1200;
    ori.ui16Top = 20;
    ori.ui16Height = 700;
    ori.fPixelMax = 288;

    ASSERT_EQ(IMG_SUCCESS, MC_EXSConvert(&ori, &converted));

    if (ori.bRegionEnable) config |= CI_SAVE_EXS_REGION;
    if (ori.bGlobalEnable) config |= CI_SAVE_EXS_GLOBAL;
    ASSERT_EQ(IMG_SUCCESS, MC_EXSRevert(&converted, config, &reverted));

    EXPECT_EQ(ori.bRegionEnable, reverted.bRegionEnable);
    EXPECT_EQ(ori.bGlobalEnable, reverted.bGlobalEnable);
    EXPECT_EQ(ori.ui16Left, reverted.ui16Left);
    EXPECT_EQ(ori.ui16Top, reverted.ui16Top);
    EXPECT_EQ(ori.ui16Width, reverted.ui16Width);
    EXPECT_EQ(ori.ui16Height, reverted.ui16Height);
    EXPECT_EQ(ori.fPixelMax, reverted.fPixelMax);
}

/** @brief test convertion to CI and reverting */
TEST(MC, FOSRevert)
{
    MC_IIF sIIF;
    MC_FOS ori, reverted;
    CI_MODULE_FOS converted;
    IMG_UINT32 config = 0;

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_FOSInit(&ori, &sIIF));

    // warning: these are not necessarily valid values!

    ori.bRegionEnable = IMG_FALSE;
    ori.bGlobalEnable = IMG_TRUE;
    ori.ui16ROILeft = 80;
    ori.ui16ROIWidth = 1200;
    ori.ui16ROITop = 20;
    ori.ui16ROIHeight = 700;

    ASSERT_EQ(IMG_SUCCESS, MC_FOSConvert(&ori, &converted));

    if (ori.bRegionEnable) config |= CI_SAVE_FOS_ROI;
    if (ori.bGlobalEnable) config |= CI_SAVE_FOS_GRID;
    ASSERT_EQ(IMG_SUCCESS, MC_FOSRevert(&converted, config, &reverted));

    EXPECT_EQ(ori.bRegionEnable, reverted.bRegionEnable);
    EXPECT_EQ(ori.bGlobalEnable, reverted.bGlobalEnable);
    EXPECT_EQ(ori.ui16Left, reverted.ui16Left);
    EXPECT_EQ(ori.ui16Top, reverted.ui16Top);
    EXPECT_EQ(ori.ui16Width, reverted.ui16Width);
    EXPECT_EQ(ori.ui16Height, reverted.ui16Height);
    EXPECT_EQ(ori.ui16ROILeft, reverted.ui16ROILeft);
    EXPECT_EQ(ori.ui16ROITop, reverted.ui16ROITop);
    EXPECT_EQ(ori.ui16ROIWidth, reverted.ui16ROIWidth);
    EXPECT_EQ(ori.ui16ROIHeight, reverted.ui16ROIHeight);
}

/** @brief test convertion to CI and reverting */
TEST(MC, FLDRevert)
{
    MC_IIF sIIF;
    MC_FLD ori, reverted;
    CI_MODULE_FLD converted;
    IMG_UINT32 config = 0;
    const double prec = 0.2; // 1.0 / (1 << FLD_FRAC_STEP_50_FRAC);

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_FLDInit(&ori, &sIIF));

    // warning: these are not necessarily valid values!

    ori.bEnable = IMG_TRUE;
    ori.ui16VTot = 1080;
    ori.fFrameRate = 60.0f;

    ASSERT_EQ(IMG_SUCCESS, MC_FLDConvert(&ori, &converted));

    if (ori.bEnable) config |= CI_SAVE_FLICKER;
    ASSERT_EQ(IMG_SUCCESS, MC_FLDRevert(&converted, config, &reverted));

    EXPECT_EQ(ori.bEnable, reverted.bEnable);
    EXPECT_EQ(ori.ui16VTot, reverted.ui16VTot);
    EXPECT_NEAR(ori.fFrameRate, reverted.fFrameRate, prec);
    EXPECT_EQ(ori.ui16CoefDiff, reverted.ui16CoefDiff);
    EXPECT_EQ(ori.ui16NFThreshold, reverted.ui16NFThreshold);
    EXPECT_EQ(ori.ui32SceneChange, reverted.ui32SceneChange);
    EXPECT_EQ(ori.ui8RShift, reverted.ui8RShift);
    EXPECT_EQ(ori.ui8MinPN, reverted.ui8MinPN);
    EXPECT_EQ(ori.ui8PN, reverted.ui8PN);
    EXPECT_EQ(ori.bReset, reverted.bReset);
}

/** @brief test convertion to CI and reverting */
TEST(MC, HISRevert)
{
    MC_IIF sIIF;
    MC_HIS ori, reverted;
    CI_MODULE_HIS converted;
    IMG_UINT32 config = 0;
    //const double prec0 = 1.0 / (1 << 10);
    //const double prec1 = 1.0 / (1 << 10);

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_HISInit(&ori, &sIIF));

    // warning: these are not necessarily valid values!

    ori.bRegionEnable = IMG_TRUE;
    ori.bGlobalEnable = IMG_TRUE;
    ori.fInputOffset = 64;
    ori.fInputScale = (MC_FLOAT)(1 << 12);  // full 12b range
    ori.ui16Left = 80;
    ori.ui16Width = 1200;
    ori.ui16Top = 20;
    ori.ui16Height = 700;

    ASSERT_EQ(IMG_SUCCESS, MC_HISConvert(&ori, &converted));

    if (ori.bRegionEnable) config |= CI_SAVE_HIST_REGION;
    if (ori.bGlobalEnable) config |= CI_SAVE_HIST_GLOBAL;
    ASSERT_EQ(IMG_SUCCESS, MC_HISRevert(&converted, config, &reverted));

    // enable are managed by other functions
    EXPECT_EQ(ori.fInputOffset, reverted.fInputOffset);
    EXPECT_EQ(ori.fInputScale, reverted.fInputScale);
    EXPECT_EQ(ori.ui16Left, reverted.ui16Left);
    EXPECT_EQ(ori.ui16Top, reverted.ui16Top);
    EXPECT_EQ(ori.ui16Width, reverted.ui16Width);
    EXPECT_EQ(ori.ui16Height, reverted.ui16Height);
}

/** @brief test convertion to CI and reverting */
TEST(MC, WBSRevert)
{
    MC_IIF sIIF;
    MC_WBS ori, reverted;
    CI_MODULE_WBS converted;
    IMG_UINT32 config = 0;
    //const double prec0 = 1.0 / (1 << 10);
    //const double prec1 = 1.0 / (1 << 10);

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_WBSInit(&ori, &sIIF));

    // warning: these are not necessarily valid values!

    ori.ui8ActiveROI = 1;
    ori.fRGBOffset = 10;
    ori.fYOffset = 12;

    ori.aRoiLeft[0] = 80;
    ori.aRoiWidth[0] = 1200;
    ori.aRoiTop[0] = 20;
    ori.aRoiHeight[0] = 700;

    ori.aRMax[0] = (1 << 12) - 20;
    ori.aGMax[0] = (1 << 12) - 30;
    ori.aBMax[0] = (1 << 12) - 40;
    ori.aYMax[0] = (1 << 12) - 50;

    ori.aRoiLeft[1] = 0;
    ori.aRoiWidth[1] = 1000;
    ori.aRoiTop[1] = 50;
    ori.aRoiHeight[0] = 500;

    ori.aRMax[1] = (1 << 10) - 50;
    ori.aGMax[1] = (1 << 10) - 40;
    ori.aBMax[1] = (1 << 10) - 30;
    ori.aYMax[1] = (1 << 10) - 20;

    ASSERT_EQ(IMG_SUCCESS, MC_WBSConvert(&ori, &converted));

    if (ori.ui8ActiveROI > 0) config |= CI_SAVE_WHITEBALANCE;
    ASSERT_EQ(IMG_SUCCESS, MC_WBSRevert(&converted, config, &reverted));

    EXPECT_EQ(ori.ui8ActiveROI, reverted.ui8ActiveROI);
    EXPECT_EQ(ori.fRGBOffset, reverted.fRGBOffset);
    EXPECT_EQ(ori.fYOffset, reverted.fYOffset);
    for (int i = 0; i < WBS_NUM_ROI; i++)
    {
        EXPECT_EQ(ori.aRoiLeft[i], reverted.aRoiLeft[i]);
        EXPECT_EQ(ori.aRoiTop[i], reverted.aRoiTop[i]);
        EXPECT_EQ(ori.aRoiWidth[i], reverted.aRoiWidth[i]);
        EXPECT_EQ(ori.aRoiHeight[i], reverted.aRoiHeight[i]);

        EXPECT_EQ(ori.aRMax[i], reverted.aRMax[i]);
        EXPECT_EQ(ori.aGMax[i], reverted.aGMax[i]);
        EXPECT_EQ(ori.aBMax[i], reverted.aBMax[i]);
        EXPECT_EQ(ori.aYMax[i], reverted.aYMax[i]);
    }
}

/** @brief test convertion to CI and reverting */
TEST(MC, ENSRevert)
{
    MC_ENS ori, reverted;
    CI_MODULE_ENS converted;
    IMG_UINT32 config = 0;

    ASSERT_EQ(IMG_SUCCESS, MC_ENSInit(&ori));

    // warning: these are not necessarily valid values!

    ori.bEnable = IMG_TRUE;
    ori.ui32NLines = 64;
    ori.ui32KernelSubsampling = 16;
    
    ASSERT_EQ(IMG_SUCCESS, MC_ENSConvert(&ori, &converted));

    if (ori.bEnable) config |= CI_SAVE_ENS;
    ASSERT_EQ(IMG_SUCCESS, MC_ENSRevert(&converted, config, &reverted));

    // enable are managed by other functions
    EXPECT_EQ(ori.ui32NLines, reverted.ui32NLines);
    EXPECT_EQ(ori.ui32KernelSubsampling, reverted.ui32KernelSubsampling);
}
