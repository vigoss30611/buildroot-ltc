/**
******************************************************************************
 @file ci_mmu_test.cpp

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
#include <felix.h>
#include <gtest/gtest.h>

#ifdef FELIX_HAS_DG
#include <dg/dg_api.h>
#endif

#include "ci_kernel/ci_kernel.h"
#include "felixcommon/ci_alloc_info.h"

/**
 * @brief Ensures that max tiling strides by felixcommon fit in the MMU
 */
TEST(MMU, tiling_max_sizes)
{
    struct CI_TILINGINFO res;
    IMG_UINT32 tiled;

    memset(&res, 0, sizeof(struct CI_TILINGINFO));
    tiled = 256;
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(tiled, &res));

    EXPECT_LE((unsigned)MMU_MIN_TILING_STR, res.ui32MinTileStride)
        << "tiled format " << tiled;
    EXPECT_GE((unsigned)MMU_MAX_TILING_STR, res.ui32MaxTileStride)
        << "tiled format " << tiled;

    memset(&res, 0, sizeof(struct CI_TILINGINFO));
    tiled = 512;
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(tiled, &res));

    EXPECT_LE(MMU_MIN_TILING_STR*2u, res.ui32MinTileStride)
        << "tiled format " << tiled;
    EXPECT_GE(MMU_MAX_TILING_STR*2u, res.ui32MaxTileStride)
        << "tiled format " << tiled;
}

TEST(MMU, config_on_off)
{
    Felix driver;

    driver.configure(DEFAULT_VAL, IMG_TRUE);
}

TEST(MMU, config_map)
{
    FullFelix driver;

    driver.configure(32, 32, YVU_420_PL12_8, PXL_NONE, false, 5, IMG_TRUE);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline))
        << "map things";
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline))
        << "unmap things";
}

TEST(MMU, config_rough_exit)
{
    FullFelix driver;

    driver.configure(32, 32, YVU_420_PL12_8, PXL_NONE, false, 5, IMG_TRUE);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline))
        << "map things";
}

#ifdef FELIX_HAS_DG
TEST(MMU, dg_map)
{
    Datagen driver;
    DG_CAMERA *pCamera = NULL;

    driver.configure(DEFAULT_VAL, IMG_TRUE);

    EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    pCamera->eFormat = CI_DGFMT_MIPI;
    pCamera->ui8FormatBitdepth = 10;

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(IMG_SUCCESS, DG_CameraAllocateFrame(pCamera,
            SYSMEM_ALIGNMENT, NULL));
    }

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStart(pCamera));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStop(pCamera));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraDestroy(pCamera));
}

TEST(MMU, dg_rough_exit)
{
    Datagen driver;
    DG_CAMERA *pCamera = NULL;

    driver.configure(DEFAULT_VAL, IMG_TRUE);

    EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    pCamera->eFormat = CI_DGFMT_MIPI;
    pCamera->ui8FormatBitdepth = 10;

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(IMG_SUCCESS, DG_CameraAllocateFrame(pCamera,
            SYSMEM_ALIGNMENT, NULL));
    }

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStart(pCamera));
}

TEST(MMU, config_dg_map)
{
    FullFelix driver;
    KRN_DG_DRIVER sDGDriver;
    DG_CONNECTION *pDGConnection = NULL;
    DG_CAMERA *pCamera = NULL;

    driver.configure(32, 32, YVU_420_PL12_8, PXL_NONE, false, 5, IMG_TRUE);

    // default PLL and disabled MMU
    EXPECT_EQ(IMG_SUCCESS, KRN_DG_DriverCreate(&sDGDriver, 0, 0, 0));
    EXPECT_EQ(IMG_SUCCESS, DG_DriverInit(&pDGConnection));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera, pDGConnection));

    pCamera->eFormat = CI_DGFMT_MIPI;
    pCamera->ui8FormatBitdepth = 10;

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(IMG_SUCCESS, DG_CameraAllocateFrame(pCamera,
            SYSMEM_ALIGNMENT, NULL));
    }

    /* in real life we would start the gasket too here but this does not use
     * the HW so we don't have to */

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStart(pCamera));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline)) << "map things";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline)) << "unmap things";

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStop(pCamera));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraDestroy(pCamera));

    DG_DriverFinalise(pDGConnection);
    KRN_DG_DriverDestroy(&sDGDriver);
    // felix driver stops here
}

TEST(MMU, config_dg_rough)
{
    FullFelix driver;
    KRN_DG_DRIVER sDGDriver;
    DG_CONNECTION *pDGConnection = NULL;
    DG_CAMERA *pCamera = NULL;

    driver.configure(32, 32, YVU_420_PL12_8, PXL_NONE, false, 5, IMG_TRUE);

    // default PLL and disabled MMU
    EXPECT_EQ(IMG_SUCCESS, KRN_DG_DriverCreate(&sDGDriver, 0, 0, 0));
    EXPECT_EQ(IMG_SUCCESS, DG_DriverInit(&pDGConnection));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera, pDGConnection));

    pCamera->eFormat = CI_DGFMT_MIPI;
    pCamera->ui8FormatBitdepth = 10;

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(IMG_SUCCESS, DG_CameraAllocateFrame(pCamera,
            SYSMEM_ALIGNMENT, NULL));
    }

    /* in real life we would start the gasket too here but this does not use
     * the HW so we don't have to */

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStart(pCamera));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline))
        << "map things";

    // because it is not running the clean procedure otherwise
    DG_DriverFinalise(pDGConnection);
    KRN_DG_DriverDestroy(&sDGDriver);
}

#endif /* FELIX_HAS_DG */
