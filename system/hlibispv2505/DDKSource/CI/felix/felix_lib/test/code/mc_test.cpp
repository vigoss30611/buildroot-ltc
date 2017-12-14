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
#include "felix_hw_info.h"
#include "hw_struct/ctx_config.h"
#include <ctx_reg_precisions.h>

#include <cstdlib> // rand+srand
#include <cmath>
#include <algorithm>

#ifdef WIN32
// log2 not available on all versions of windows
#define log2(n) (log10(n) / log10(2.0))
#endif

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

TEST(MC, LSHConvert_DISABLED)
{
    //  update for new method
#if 0
    LSH_GRID sGrid = LSH_GRID();
    CI_PIPELINE sPipe;

  #ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&sGrid,
		"testdata/deshading_diag.lsh", -1, NULL));
  #else
    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&sGrid,
        "testdata/deshading_diag.lsh"));
  #endif

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
#endif /* 0 */
}

void LSH_Update(const MC_LSH &sMC_LSH, FullFelix &driver)
{
    //  update for new method
#if 0
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
#endif /* 0 */
}

TEST(LSH, ComputeMinBitDiff)
{
    LSH_GRID sGrid = LSH_GRID();
    IMG_UINT8 ui8BitsPerDiff = 0;
    unsigned int y, x, c;
    // do not take the sign because we do not go negative
    const IMG_INT32 maxMult = (1 << (LSH_VERTEX_INT + LSH_VERTEX_FRAC));
    const LSH_FLOAT maxValue = (maxMult - 1) / static_cast<LSH_FLOAT>(1 << LSH_VERTEX_FRAC);

    EXPECT_EQ(0, MC_LSHComputeMinBitdiff(NULL, NULL));

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&sGrid,
        "testdata/deshading_diag.lsh", -1, NULL));  // 20x20 32t
#else
    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&sGrid,
        "testdata/deshading_diag.lsh"));  // 20x20 32t
#endif

    ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
    EXPECT_EQ(8, ui8BitsPerDiff);

    // set the grid so that all entries are the same - expecting the min diff
    for (c = 0; c < LSH_MAT_NO; c++)
    {
        for (y = 0; y < sGrid.ui16Height; y++)
        {
            for (x = 0; x < sGrid.ui16Width; x++)
            {
                sGrid.apMatrix[c][y*sGrid.ui16Width + x] = 1.0;
            }
        }
    }

    // expects to compute 0 but return min possible delta
    ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
    EXPECT_EQ(LSH_DELTA_BITS_MIN, ui8BitsPerDiff);

    unsigned int expDiff = 0;
    IMG_INT32 value;

    for (y = 0; y < sGrid.ui16Height; y++)
    {
        value = (IMG_UINT16)IMG_Fix_Clip(0.0, PREC(LSH_VERTEX));
        // 1 bit of diff per line
        value += (1 << y);
        LSH_FLOAT f = (value) / static_cast<LSH_FLOAT>(1 << LSH_VERTEX_FRAC);
        if (value <= maxMult)
        {
            // +1 because we add 1 when computing to handle the sign
            // but here we do not have negative differences
            expDiff = IMG_MAX_INT(expDiff, y + 1);
        }
        else
        {
            f = maxValue;
        }

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            sGrid.apMatrix[c][y*sGrid.ui16Width] = 0.0;
            for (x = 1; x < sGrid.ui16Width; x++)
            {
                sGrid.apMatrix[c][y*sGrid.ui16Width + x] = f;
            }
        }
    }

    /* generates a lot of clipping warnings so we could change to the max
     * value that generates 0x1000 instead of 0xfff */
    ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
    EXPECT_EQ(expDiff, ui8BitsPerDiff);
    EXPECT_GT(ui8BitsPerDiff, LSH_DELTA_BITS_MAX) << "test badly designed";

    //
    // try with negative difference
    //
    expDiff = 0;

    for (y = 0; y < sGrid.ui16Height; y++)
    {
        value = (IMG_UINT16)IMG_Fix_Clip(
            maxValue, PREC(LSH_VERTEX));
        // 1 bit of diff per line
        value -= (1 << y);
        LSH_FLOAT f = (value) / static_cast<LSH_FLOAT>(1 << LSH_VERTEX_FRAC);
        if ((value + 1) >= 0)
        {
            // +1 because we add 1 when computing to handle the sign
            // but here we do not have negative differences
            expDiff = IMG_MAX_INT(expDiff, y + 1);
        }
        else
        {
            f = 0.0;
        }

        for (c = 0; c < LSH_MAT_NO; c++)
        {
            sGrid.apMatrix[c][y*sGrid.ui16Width] = maxValue;
            for (x = 1; x < sGrid.ui16Width; x++)
            {
                sGrid.apMatrix[c][y*sGrid.ui16Width + x] = f;
            }
        }
    }

    ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
    EXPECT_EQ(expDiff, ui8BitsPerDiff);
    EXPECT_GT(ui8BitsPerDiff, LSH_DELTA_BITS_MAX) << "test badly designed";

    LSH_Free(&sGrid);
}

void checkSizes(const int width, const int height, const int tile,
    const int bitsPerDiff)
{
    IMG_UINT32 uiAlloc = 0, uiLineSize = 0, uiStride = 0;
    LSH_GRID sGrid = LSH_GRID();

    IMG_UINT32 grid_height = (height / tile);
    IMG_UINT32 grid_width = (width / tile);
    IMG_UINT32 lineSize = 0;
    IMG_UINT32 stride = 0;
    IMG_UINT32 allocation = 0;

    if (0)
    {
        std::cout << __FUNCTION__ << " w=" << width << " h=" << height
            << " t=" << tile << " b=" << bitsPerDiff << std::endl;
    }
    /*
     * line size = 2B + round_up_to_byte((width - 1) * bit_per_diff)
     * but line size has to be a multiple of 16B (HW requirement)
     *
     * stride is line size rounded up to multiple of 64B
     *
     * allocation is stride * height * 4 channels
     *
     * allocation in pages is allocation rounded up to page size (4KB or 16KB)
     */
    if (width % tile == 0)
    {
        grid_width++;  // extra tile to cope with corner
    }
    lineSize = 2 + (((grid_width - 1) * bitsPerDiff + 7) / 8);
    lineSize =  // round up power of 2 - to multiple of 16 Bytes
        (lineSize + 15) / 16 * 16;

    stride =  // round up power of 2 to system alignment
        (lineSize + (SYSMEM_ALIGNMENT - 1)) / SYSMEM_ALIGNMENT
        * SYSMEM_ALIGNMENT;

    if (height % tile == 0)
    {
        grid_height++;  // extra tile to cope with corner
    }
    allocation = grid_height * stride * 4;
    allocation =  // round up power of 2 to system alignment
        (allocation + (SYSMEM_ALIGNMENT - 1)) / SYSMEM_ALIGNMENT
        * SYSMEM_ALIGNMENT;

    sGrid.ui16TileSize = LSH_TILE_MIN;
    sGrid.ui16Width = grid_width;
    sGrid.ui16Height = grid_height;

    ASSERT_LE(bitsPerDiff, LSH_DELTA_BITS_MAX) << "wrong test design";
    ASSERT_GE(bitsPerDiff, LSH_DELTA_BITS_MIN) << "wrong test design";
    ASSERT_EQ(tile, 1 << static_cast<int>(log2(tile))) << "wrong test design";

    uiAlloc = MC_LSHGetSizes(&sGrid, bitsPerDiff, &uiLineSize, &uiStride);
    EXPECT_TRUE(uiAlloc%SYSMEM_ALIGNMENT == 0)
        << "allocation should be aligned to system alignment";
    EXPECT_TRUE(uiLineSize % 16 == 0)
        << "line size should be multiple of 16";
    EXPECT_TRUE(uiStride%SYSMEM_ALIGNMENT == 0)
        << "line stride should be multiple of system alignment";
    EXPECT_EQ(lineSize, uiLineSize) << "wrong line size";
    EXPECT_EQ(stride, uiStride) << "wrong stride";
    EXPECT_EQ(allocation, uiAlloc) << "wrong allocation size!";
}

void checkConvertion(const int width, const int height, const int tile,
    const int bitsPerDiff)
{
    float lshCorners[5] = { 0.2f, 0.3f, 0.4f, 0.5f, 1.0f };
    IMG_UINT32 uiAlloc = 0, uiLineSize = 0, uiStride = 0;
    LSH_GRID sGrid = LSH_GRID();
    CI_LSHMAT sMat;

    if (0)
    {
        std::cout << __FUNCTION__ << " w=" << width << " h=" << height
            << " t=" << tile << " b=" << bitsPerDiff << std::endl;
    }

    uiAlloc = MC_LSHGetSizes(&sGrid, bitsPerDiff, &uiLineSize, &uiStride);

    ASSERT_EQ(IMG_SUCCESS,
        LSH_CreateMatrix(&sGrid, width, height, tile));
    // all the values are 0 but we don't care about the content

    uiAlloc = MC_LSHGetSizes(&sGrid, bitsPerDiff,
        &uiLineSize, &uiStride);
    ASSERT_GT(uiAlloc, 0u);

    // fake the allocation of a matrix
    sMat.ui32Size = uiAlloc;
    sMat.data = calloc(uiAlloc, 1);
    sMat.id = 42;

    // set skip and offset to ensure MC_LSHConvertGrid does not change them
    sMat.config.ui16SkipX = 45;
    sMat.config.ui16SkipY = 46;
    sMat.config.ui16OffsetX = 47;
    sMat.config.ui16OffsetY = 48;
    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvertGrid(&sGrid, bitsPerDiff, &sMat));
    // skip and offset not changed
    EXPECT_EQ(45, sMat.config.ui16SkipX);
    EXPECT_EQ(46, sMat.config.ui16SkipY);
    EXPECT_EQ(47, sMat.config.ui16OffsetX);
    EXPECT_EQ(48, sMat.config.ui16OffsetY);
    // set them back to actual values
    sMat.config.ui16SkipX = 0;
    sMat.config.ui16SkipY = 0;
    sMat.config.ui16OffsetX = 0;
    sMat.config.ui16OffsetY = 0;
    // verify output of MC_LSHConvertGrid
    EXPECT_EQ(log2(tile), sMat.config.ui8TileSizeLog2);
    EXPECT_EQ(bitsPerDiff, sMat.config.ui8BitsPerDiff);
    EXPECT_EQ(sGrid.ui16Width, sMat.config.ui16Width);
    EXPECT_EQ(sGrid.ui16Height, sMat.config.ui16Height);
    EXPECT_EQ((uiLineSize / 16) - 1, sMat.config.ui16LineSize);
    EXPECT_EQ(uiStride, sMat.config.ui32Stride);
    EXPECT_EQ(42, sMat.id);

    //  here we could check the content

    LSH_Free(&sGrid);
    free(sMat.data);
}

TEST(LSH, GetSizes)
{
    int width[] = { 640, 1280, 1920, 2560, 4096 };
    int height[] = { 480, 720, 1080, 1440, 2160 };

    ASSERT_EQ(sizeof(width), sizeof(height)) << "wrong test design";

    // some manual tests were sizes were computed by hand
    {
        IMG_UINT32 uiAlloc = 0, uiLineSize = 0, uiStride = 0;
        LSH_GRID sGrid = LSH_GRID();
        if (0)
        {
            std::cout << "manual w=32 h=32 t=8 b=10" << std::endl;
        }
        sGrid.ui16Width = 5;  // (32%8) == 0 so need one more tile
        sGrid.ui16Height = 5;  // (32%8) == 0 so need one more tile
        sGrid.ui16TileSize = 8;
        uiAlloc = MC_LSHGetSizes(&sGrid, 10, &uiLineSize, &uiStride);
        ASSERT_EQ(16, uiLineSize);
        ASSERT_EQ(64, uiStride);
        ASSERT_EQ(1280, uiAlloc);

        if (0)
        {
            std::cout << "manual w=2100 h=1200 t=16 b=7" << std::endl;
        }
        sGrid.ui16Width = 132;
        sGrid.ui16Height = 75;
        sGrid.ui16TileSize = 16;
        uiAlloc = MC_LSHGetSizes(&sGrid, 7, &uiLineSize, &uiStride);
        ASSERT_EQ(128, uiLineSize);
        ASSERT_EQ(128, uiStride);
        ASSERT_EQ(38400, uiAlloc);
    }

    // then run for several resolutions, all tile size and all bits per diff
    for (int i = 0; i < (sizeof(width) / sizeof(int)); i++)
    {
        for (int t = LSH_TILE_MIN; t <= LSH_TILE_MAX; t*=2)
        {
            for (int b = LSH_DELTA_BITS_MIN; b <= LSH_DELTA_BITS_MAX
                && !HasFailure(); b++)
            {
                checkSizes(width[i], height[i], t, b);
            }
        }
    }
}

TEST(LSH, ConvertGrid)
{
    int width[] = { 640, 1280, 1920, 2560, 4096 };
    int height[] = { 480, 720, 1080, 1440, 2160 };

    ASSERT_EQ(sizeof(width), sizeof(height)) << "wrong test design";

    // then run for several resolutions, all tile size and all bits per diff
    for (int i = 0; i < (sizeof(width) / sizeof(int)); i++)
    {
        for (int t = LSH_TILE_MIN; t <= LSH_TILE_MAX; t *= 2)
        {
            for (int b = LSH_DELTA_BITS_MIN; b <= LSH_DELTA_BITS_MAX
                && !HasFailure(); b++)
            {
                checkConvertion(width[i], height[i], t, b);
            }
        }
    }
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

/**
 * @brief Convert double to fixed point
 * @note the same as IMG_Fix_Clip() but without clipping,
 * used to test value ranges
 *
 */
static IMG_UINT32 img_fix(MC_FLOAT fl, IMG_INT32 intBits, IMG_INT32 fractBits,
        IMG_BOOL isSigned, const char *dbg_regname)
    {
        int flSign = 1;
        IMG_INT32 ret;

        if(isSigned && fl < 0.0)
        {
            flSign = -1;
        }

        if(fractBits < 0)
        {
            ret = (IMG_INT32)fl >> abs(fractBits);
        }
        else
        {
            ret = (IMG_INT32)(fl*(IMG_INT64)(1 << fractBits) + 0.5 * flSign);
        }
#if(0)
        printf("%s: %f -> 0x%x (%d, %d)\n",
              dbg_regname, fl, ret, intBits, fractBits);
#endif
        return ret;
    }

/**
 * @brief fixed point precision for specific register
 */
#define FP_PREC(REG) (1.0/(1 << (REG##_FRAC)))

#ifdef AWS_AVAILABLE

TEST(MC, AWSInit)
{
    MC_IIF sIIF;
    MC_AWS aws;

    IMG_UINT16 width=1280, height=720;

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));
    sIIF.ui16ImagerSize[0] = width / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = height / CI_CFA_HEIGHT;
    ASSERT_EQ(IMG_SUCCESS, MC_AWSInit(&aws, &sIIF));

    EXPECT_EQ(aws.bDebugBitmap, 0);
    EXPECT_EQ(aws.bEnable, 0);

    EXPECT_LE(aws.ui16GridTileWidth*AWS_NUM_GRID_TILES_HORIZ, width);
    EXPECT_LE(aws.ui16GridTileHeight*AWS_NUM_GRID_TILES_VERT, height);
    EXPECT_GE(aws.ui16GridTileWidth, CI_CFA_WIDTH);
    EXPECT_GE(aws.ui16GridTileHeight, CI_CFA_HEIGHT);

    // verify that the default values are reasonable and cause no
    // unintended clipping
    IMG_UINT32 integer;

    integer = img_fix(aws.fLog2_R_Qeff, PREC(AWS_LOG2_R_QEFF));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_LOG2_QEFF_AWS_LOG2_R_QEFF_LSBMASK);

    integer = img_fix(aws.fLog2_B_Qeff, PREC(AWS_LOG2_B_QEFF));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_LOG2_QEFF_AWS_LOG2_B_QEFF_LSBMASK);

    integer = img_fix(aws.fRedDarkThresh, PREC(AWS_R_DARK_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_0_AWS_R_DARK_THRESH_LSBMASK);

    integer = img_fix(aws.fBlueDarkThresh, PREC(AWS_B_DARK_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_1_AWS_B_DARK_THRESH_LSBMASK);

    integer = img_fix(aws.fGreenDarkThresh, PREC(AWS_G_DARK_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_0_AWS_G_DARK_THRESH_LSBMASK);

    integer = img_fix(aws.fRedClipThresh, PREC(AWS_R_CLIP_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_1_AWS_R_CLIP_THRESH_LSBMASK);

    integer = img_fix(aws.fBlueClipThresh, PREC(AWS_B_CLIP_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_2_AWS_B_CLIP_THRESH_LSBMASK);

    integer = img_fix(aws.fGreenClipThresh, PREC(AWS_G_CLIP_THRESH));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_2_AWS_G_CLIP_THRESH_LSBMASK);

    integer = img_fix(aws.fBbDist, PREC(AWS_BB_DIST));
    EXPECT_LE(integer,
            FELIX_LOAD_STRUCTURE_AWS_THRESHOLDS_2_AWS_G_CLIP_THRESH_LSBMASK);

}

#define ALIGN_SIZE(size) (((size)+4096-1) & ~(4096-1))

/**
 * @brief verify using autogenerated row offsets
 * @note needed because MC_AWSExtract() calculates every row
 */
#define VALIDATE_AWS_ROW(row) \
{ \
    IMG_UINT32* pR = blob+FELIX_SAVE_AWS_COLLECTED_R_TILE_##row##_OFFSET/4; \
    IMG_UINT32* pG = blob+FELIX_SAVE_AWS_COLLECTED_G_TILE_##row##_OFFSET/4; \
    IMG_UINT32* pB = blob+FELIX_SAVE_AWS_COLLECTED_B_TILE_##row##_OFFSET/4; \
    IMG_UINT32* pCFA = blob+FELIX_SAVE_AWS_NUM_CFAS_TILE_##row##_OFFSET/4; \
    for(int i=0;i<AWS_NUM_GRID_TILES_HORIZ;++i) \
    { \
        EXPECT_DOUBLE_EQ(stats.tileStats[row][i].fCollectedRed, \
            IMG_Fix_Revert(*pR, PREC(AWS_COLLECTED_R))); \
        EXPECT_DOUBLE_EQ(stats.tileStats[row][i].fCollectedBlue, \
            IMG_Fix_Revert(*pB, PREC(AWS_COLLECTED_B))); \
        EXPECT_DOUBLE_EQ(stats.tileStats[row][i].fCollectedGreen, \
            IMG_Fix_Revert(*pG, PREC(AWS_COLLECTED_G))); \
        EXPECT_EQ(stats.tileStats[row][i].numCFAs, *pCFA & \
            FELIX_SAVE_AWS_NUM_CFAS_TILE_0_AWS_NUM_CFAS_TILE_0_LSBMASK); \
        pR += FELIX_SAVE_AWS_COLLECTED_R_TILE_##row##_STRIDE/4; \
        pG += FELIX_SAVE_AWS_COLLECTED_G_TILE_##row##_STRIDE/4; \
        pB += FELIX_SAVE_AWS_COLLECTED_B_TILE_##row##_STRIDE/4; \
        pCFA += FELIX_SAVE_AWS_NUM_CFAS_TILE_##row##_STRIDE/4; \
    } \
}

TEST(MC, AWSExtract)
{
    // each row is aligned to 128 bytes
    int awsStatsSizeBytes =
            FELIX_SAVE_AWS_COLLECTED_R_TILE_0_OFFSET +
            128 * AWS_NUM_GRID_TILES_VERT;
    awsStatsSizeBytes = ALIGN_SIZE(awsStatsSizeBytes);
    const int awsStatsSizeWords = awsStatsSizeBytes/sizeof(IMG_UINT32);
    IMG_UINT32* blob = new IMG_UINT32[awsStatsSizeWords];
    ASSERT_NE(blob, (void*)0);
    {
        // 'randomize' the buffer using known seed
        std::srand(0x12563478);
        for(int i=0;i<awsStatsSizeWords;++i)
        {
            blob[i] = std::rand();
        }
    }

    MC_STATS_AWS stats;
    ASSERT_EQ(IMG_SUCCESS, MC_AWSExtract(blob, &stats));

    // this test covers configurations with at least 7 rows
    IMG_STATIC_ASSERT(AWS_NUM_GRID_TILES_VERT==7, checkAWS_has7Rows);

    VALIDATE_AWS_ROW(0);
    VALIDATE_AWS_ROW(1);
    VALIDATE_AWS_ROW(2);
    VALIDATE_AWS_ROW(3);
    VALIDATE_AWS_ROW(4);
    VALIDATE_AWS_ROW(5);
    VALIDATE_AWS_ROW(6);

    delete[] blob;
}
#undef VALIDATE_AWS_ROW

/** @brief test convertion to CI and reverting */
TEST(MC, AWSRevert)
{
    MC_IIF sIIF;
    MC_AWS ori, reverted;
    CI_MODULE_AWS converted;
    IMG_UINT32 config = 0;

    ASSERT_EQ(IMG_SUCCESS, MC_IIFInit(&sIIF));

    sIIF.ui16ImagerSize[0] = 1280 / CI_CFA_WIDTH;
    sIIF.ui16ImagerSize[1] = 720 / CI_CFA_HEIGHT;

    ASSERT_EQ(IMG_SUCCESS, MC_AWSInit(&ori, &sIIF));

    // let's use some unique values
    ori.bDebugBitmap = IMG_TRUE;
    ori.bEnable = IMG_TRUE;
    ori.fBbDist = 13.23;
    ori.fBlueClipThresh = 23.43;
    ori.fBlueDarkThresh = 65.76;
    ori.fGreenClipThresh = 76.25;
    ori.fGreenDarkThresh = 45.12;
    ori.fLog2_B_Qeff = 64.23;
    ori.fLog2_R_Qeff = 24.58;
    ori.fRedClipThresh = 123.49;
    ori.fRedDarkThresh = 98.76;
    ori.ui16GridStartColumn = 10;
    ori.ui16GridStartRow = 20;
    ori.ui16GridTileWidth = 30;
    ori.ui16GridTileHeight = 40;

    ASSERT_EQ(IMG_SUCCESS, MC_AWSConvert(&ori, &converted));

    if (ori.bEnable > 0) config |= CI_SAVE_WHITEBALANCE_EXT;
    ASSERT_EQ(IMG_SUCCESS, MC_AWSRevert(&converted, config, &reverted));

    EXPECT_EQ(reverted.bDebugBitmap, ori.bDebugBitmap);
    EXPECT_EQ(reverted.bEnable, ori.bEnable);
    EXPECT_NEAR(reverted.fBbDist, ori.fBbDist,
            FP_PREC(AWS_BB_DIST));
    EXPECT_NEAR(reverted.fBlueClipThresh, ori.fBlueClipThresh,
            FP_PREC(AWS_B_CLIP_THRESH));
    EXPECT_NEAR(reverted.fBlueDarkThresh, ori.fBlueDarkThresh,
            FP_PREC(AWS_B_DARK_THRESH));
    EXPECT_NEAR(reverted.fGreenClipThresh, ori.fGreenClipThresh,
            FP_PREC(AWS_G_CLIP_THRESH));
    EXPECT_NEAR(reverted.fGreenDarkThresh, ori.fGreenDarkThresh,
            FP_PREC(AWS_G_DARK_THRESH));
    EXPECT_NEAR(reverted.fLog2_B_Qeff, ori.fLog2_B_Qeff,
            FP_PREC(AWS_LOG2_B_QEFF));
    EXPECT_NEAR(reverted.fLog2_R_Qeff, ori.fLog2_R_Qeff,
            FP_PREC(AWS_LOG2_R_QEFF));
    EXPECT_NEAR(reverted.fRedClipThresh, ori.fRedClipThresh,
            FP_PREC(AWS_R_CLIP_THRESH));
    EXPECT_NEAR(reverted.fRedDarkThresh, ori.fRedDarkThresh,
            FP_PREC(AWS_R_DARK_THRESH));
    EXPECT_EQ(reverted.ui16GridStartColumn, ori.ui16GridStartColumn);
    EXPECT_EQ(reverted.ui16GridStartRow, ori.ui16GridStartRow);
    EXPECT_EQ(reverted.ui16GridTileWidth, ori.ui16GridTileWidth);
    EXPECT_EQ(reverted.ui16GridTileHeight, ori.ui16GridTileHeight);
}
#endif /* AWS_AVAILABLE */

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
