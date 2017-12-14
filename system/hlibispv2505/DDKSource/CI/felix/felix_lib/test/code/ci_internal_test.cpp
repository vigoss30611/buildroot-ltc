/**
******************************************************************************
@file ci_internal_test.cpp

@brief CI API internal tests

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
#include <cmath>

#ifdef WIN32
// some version of visual studio don't have log2
#define log2(x) log10(x)/log10(2.0)
#endif

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "unit_tests.h"
#include "felix.h"
extern void InitTalReset(void); // in felix.cpp - used in drivers as well

#include <tal.h>

#include "ci/ci_api.h"
#include "ci/ci_converter.h"
#include "mc/module_config.h"

#include "reg_io2.h"
#include "registers/core.h"

#include "ci_kernel/ci_kernel_structs.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_internal/sys_mem.h"
#include "ci_kernel/ci_ioctrl.h"

#include "registers/context0.h"

// move this test

// defined in felix.cpp!
extern void (*gpfnInitTal)(void); // this is used to setup the TAL default values when using the NULL interface

static int gNImagers = CI_N_IMAGERS-1;
static int gNContexts = CI_N_CONTEXT-1;

void InitTalReset_MaxHW()
{
    InitTalReset(); // normal setup

    // now only change the numbers to be bigger than what is supported
    TAL_HANDLES talHandles;
    IMG_UINT32 value;

#if FELIX_VERSION_MAJ == 1
    TALREG_ReadWord32(talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, &value);

    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_NUM_IMAGERS, gNImagers); // that's +1 of current value
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_NUM_CONTEXT, gNContexts); // that's +1 of current value

    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, value);
#else
    TALREG_ReadWord32(talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID2_OFFSET, &value);

    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_NUM_IMAGERS, gNImagers); // that's +1 of current value
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_NUM_CONTEXT, gNContexts); // that's +1 of current value

    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID2_OFFSET, value);
#endif
}

TEST(IMG_CI_Driver, MaxHWSupport)
{
    IMG_UINT32 savedNImagers = gNImagers, savedNContext = gNContexts;
    KRN_CI_DRIVER sCIDriver; // resilient
    SYS_DEVICE sDevice;
    IMG_BOOL8 bMMU;
    gpfnInitTal = &InitTalReset_MaxHW;

    sDevice.probeSuccess = NULL;
    ASSERT_EQ(IMG_SUCCESS, SYS_DevRegister(&sDevice));

    for ( bMMU = 0 ; bMMU <= 1 ; bMMU++ )
    {
        gNImagers++; // more imagers than supported
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, 0, CI_DEF_GMACURVE, &sDevice));

        gNContexts++; // more contexts than supported and more imagers than supported
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, 0, CI_DEF_GMACURVE, &sDevice));

        gNImagers--; // more contexts than supported
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, 0, CI_DEF_GMACURVE, &sDevice));

        if ( CI_N_CONTEXT > 1 )
        {
            gNContexts = 1; // less contexts than supported - good
            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, 0, CI_DEF_GMACURVE, &sDevice));

            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));
        }
        if ( CI_N_IMAGERS > 1 )
        {
            gNImagers = 1; // less imagers than supported - good
            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, 0, CI_DEF_GMACURVE, &sDevice));

            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));
        }

        ASSERT_TRUE(g_psCIDriver == NULL);
        gNImagers = savedNImagers;
        gNContexts = savedNContext;
    }

    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));
}

/**
 * @brief Creates the driver with different Gamma curves and ensure they are different
 */
TEST(IMG_CI_Driver, GMA_default)
{
#define N_GMA 2
    KRN_CI_DRIVER sCIDriver; // resilient
    SYS_DEVICE sDevice;
    gpfnInitTal = &InitTalReset_MaxHW;
    IMG_UINT16 gmaCurve_red[N_GMA][GMA_N_POINTS],
        gmaCurve_green[N_GMA][GMA_N_POINTS],
        gmaCurve_blue[N_GMA][GMA_N_POINTS];
    int gma = 0;

    sDevice.probeSuccess = NULL;
    ASSERT_EQ(IMG_SUCCESS, SYS_DevRegister(&sDevice));

    for ( gma = 0 ; gma < N_GMA ; gma++ )
    {
        ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, IMG_TRUE, 256, 0, gma, &sDevice));

        IMG_MEMCPY(gmaCurve_red[gma], sCIDriver.sGammaLUT.aRedPoints, sizeof(IMG_UINT16)*GMA_N_POINTS);
        IMG_MEMCPY(gmaCurve_green[gma], sCIDriver.sGammaLUT.aGreenPoints, sizeof(IMG_UINT16)*GMA_N_POINTS);
        IMG_MEMCPY(gmaCurve_blue[gma], sCIDriver.sGammaLUT.aBluePoints, sizeof(IMG_UINT16)*GMA_N_POINTS);

        ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));
    }
    EXPECT_EQ(IMG_ERROR_FATAL, KRN_CI_DriverCreate(&sCIDriver, IMG_TRUE, 256, 0, gma, &sDevice)) << "should not support another gamma curve! update test if legitimate";

    for ( gma = 0 ; gma < N_GMA ; gma++ )
    {
        for ( int gmacmp = gma+1 ; gmacmp < N_GMA ; gmacmp++ )
        {
            EXPECT_NE(0, IMG_MEMCMP(gmaCurve_red[gma], gmaCurve_red[gmacmp], sizeof(IMG_UINT16)*GMA_N_POINTS)) << "gma=" << gma << " gmacmp=" << gmacmp << " unexpected same red curves";
            EXPECT_NE(0, IMG_MEMCMP(gmaCurve_green[gma], gmaCurve_green[gmacmp], sizeof(IMG_UINT16)*GMA_N_POINTS)) << "gma=" << gma << " gmacmp=" << gmacmp << " unexpected same red curves";
            EXPECT_NE(0, IMG_MEMCMP(gmaCurve_blue[gma], gmaCurve_blue[gmacmp], sizeof(IMG_UINT16)*GMA_N_POINTS)) << "gma=" << gma << " gmacmp=" << gmacmp << " unexpected same red curves";
        }
    }

    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));
}

#define CHECK_SIZE(A, B) \
    std::cout << "sizeof "#A " " << sizeof(A) << " bytes" << std::endl; \
    ASSERT_LT(sizeof(A), B);
/**
 * @brief Verfies that the structures used in the kernel side are of resonable size to be used with kmalloc
 */
TEST(IMG_CI_Driver, KRN_Struct_sizes)
{
    CHECK_SIZE(KRN_CI_DRIVER, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_CONNECTION, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_PIPELINE, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_BUFFER, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_SHOT, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_DATAGEN, (unsigned)PAGE_SIZE);
    CHECK_SIZE(KRN_CI_DGBUFFER, (unsigned)PAGE_SIZE);

    // CI structs copied to kernel in ioctl
    CHECK_SIZE(CI_CONNECTION, (unsigned)PAGE_SIZE);
    CHECK_SIZE(CI_LINESTORE, (unsigned)PAGE_SIZE);
    CHECK_SIZE(CI_MODULE_GMA_LUT, (unsigned)PAGE_SIZE);
    CHECK_SIZE(CI_RTM_INFO, (unsigned)PAGE_SIZE);
    CHECK_SIZE(CI_PIPELINE, (unsigned)PAGE_SIZE);
    CHECK_SIZE(CI_SHOT, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_HAS_AVAIL, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_POOL_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_PIPE_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_ALLOC_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_BUFFER_TRIGG, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_BUFFER_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_GASKET, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_GASKET_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_DG_PARAM, (unsigned)PAGE_SIZE);
    CHECK_SIZE(struct CI_DG_FRAMEINFO, (unsigned)PAGE_SIZE);

    // other sizes not echanged with kernel so not checked
}

TEST(IMG_CI_Driver, CI_PipelineSize)
{
    KRN_CI_DRIVER sCIDriver;
    SYS_DEVICE sDevice;
    CI_CONNECTION *pConn = NULL;

    gpfnInitTal = &InitTalReset;
    gbUseTalNULL = IMG_TRUE;
    sDevice.probeSuccess = NULL;
    ASSERT_EQ(IMG_SUCCESS, SYS_DevRegister(&sDevice));
    EXPECT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, 0, CI_DEF_GMACURVE, &sDevice));

    sCIDriver.sHWInfo.uiSizeOfPipeline += 10; // fake the fact that the driver was compiled with a different CI_PIPELINE

    EXPECT_EQ(IMG_ERROR_FATAL, CI_DriverInit(&pConn));

    KRN_CI_DriverDestroy(&sCIDriver);
    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));
}

/**
 * @brief ensures that updating the LSH matrix while a pipeline is not started
 * does not affect the registers but also ensures that enabling the matrix
 * writes the expected registers (does not check the load structure stamp)
 */
TEST(INT_CI, LSH_Matrix_regs)
{
    Felix driver;
    CI_PIPELINE *pPipeline = NULL;
    MC_PIPELINE sConfiguration;
    float lshCorners[5] = { 0.2f, 0.3f, 0.4f, 0.5f, 1.0f };

    IMG_UINT32 matrixId;

    driver.configure();
    ASSERT_TRUE(driver.getConnection() != NULL);
    IMG_MEMSET(&sConfiguration, 0, sizeof(MC_PIPELINE));

    sConfiguration.sIIF.ui8Imager = 0;
    sConfiguration.sIIF.ui16ImagerSize[0] = 128;
    sConfiguration.sIIF.ui16ImagerSize[1] = 128;
    sConfiguration.sIIF.eBayerFormat = MOSAIC_RGGB; // verified by AWS
    ASSERT_EQ(IMG_SUCCESS,
        MC_PipelineInit(&sConfiguration, &(driver.getConnection()->sHWInfo)));

    ASSERT_EQ(IMG_SUCCESS,
        CI_PipelineCreate(&pPipeline, driver.getConnection()));

    ASSERT_EQ(IMG_SUCCESS, MC_PipelineConvert(&sConfiguration, pPipeline));

    pPipeline->config.ui8Context = 0;
    pPipeline->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineRegister(pPipeline));

    CreateLSHMatrix(pPipeline,
        sConfiguration.sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH,
        sConfiguration.sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT,
        LSH_TILE_MIN, 9, matrixId, lshCorners);

    IMG_UINT32 tmp;
    IMG_UINT32 field;
    CI_LSHMAT sMat = CI_LSHMAT();
    /* ensure that register do not contain the current values, acquire the
     * matrix just to access the configuration
     * we cheat: sMat will still contain the values even if we release it,
     * or we could just copy the CI_MODULE_LSH_MAT part of the sMat */
    ASSERT_EQ(IMG_SUCCESS,
        CI_PipelineAcquireLSHMatrix(pPipeline, matrixId, &sMat));
    ASSERT_EQ(IMG_SUCCESS,
        CI_PipelineReleaseLSHMatrix(pPipeline, &sMat));

    tmp = 0;  // ensure control part is disabled
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE, 0);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_VERTEX_DIFF_BITS, 0);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, tmp);

    tmp = 0;  // ensure aligbment is different than what will be used
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_OFFSET_Y,
        sMat.config.ui16OffsetY + 2);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y,
        sMat.config.ui16SkipY + 2);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, tmp);

    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_GRID_TILE, TILE_SIZE_LOG2,
        sMat.config.ui8TileSizeLog2 + 1);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_GRID_TILE_OFFSET, tmp);

    ASSERT_EQ(IMG_SUCCESS,
        CI_PipelineUpdateLSHMatrix(pPipeline, matrixId))
        << "should be able to set the matrix before starting";

    //
    // registers should not have changed because capture is not started!
    //

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE);
    EXPECT_EQ(0, field);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_VERTEX_DIFF_BITS);
    EXPECT_NE(sMat.config.ui8BitsPerDiff, field);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_OFFSET_Y);
    EXPECT_NE(sMat.config.ui16OffsetY, field);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y);
    EXPECT_NE(sMat.config.ui16SkipY, field);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_GRID_TILE_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_GRID_TILE, TILE_SIZE_LOG2);
    EXPECT_NE(sMat.config.ui8TileSizeLog2, field);

    // we need Shots to start the capture
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, 1));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));
    //
    // start capture - register should be written
    //

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE);
    EXPECT_EQ(1, field)
        << "starting the pipeline should have enabled the matrix";
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_VERTEX_DIFF_BITS);
    EXPECT_EQ(sMat.config.ui8BitsPerDiff, field);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_OFFSET_Y);
    EXPECT_EQ(sMat.config.ui16OffsetY, field);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y);
    EXPECT_EQ(sMat.config.ui16SkipY, field);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_GRID_TILE_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_GRID_TILE, TILE_SIZE_LOG2);
    EXPECT_EQ(sMat.config.ui8TileSizeLog2, field);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdateLSHMatrix(pPipeline, 0))
        << "should be able to disable the matrix";

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE);
    EXPECT_EQ(0, field)
        << "disabling the matrix should have stopped the HW";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline));

    /* write registers as if enabled to ensure starting the capture with no
     * LSH matrix writes the enable bit to 0 */
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE, 1);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, tmp);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0],
        FELIX_CONTEXT0_LSH_CTRL_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_CTRL, LSH_ENABLE);
    EXPECT_EQ(0, field)
        << "disabling the matrix should have stopped the HW";

    //
    // clean
    //

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineDeregisterLSHMatrix(pPipeline, matrixId));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineDestroy(pPipeline));

    // driver deinit here
}

TEST(CI_Driver, gasketManagement)
{
    KRN_CI_DRIVER sCIDriver;
    SYS_DEVICE sDevice;

    gpfnInitTal = &InitTalReset;
    gbUseTalNULL = IMG_TRUE;
    sDevice.probeSuccess = NULL;
    ASSERT_EQ(IMG_SUCCESS, SYS_DevRegister(&sDevice));
    ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, 0, CI_DEF_GMACURVE, &sDevice));

    {
        CI_CONNECTION *pConn = NULL;
        EXPECT_EQ(IMG_SUCCESS, CI_DriverInit(&pConn));
        ASSERT_GT(g_psCIDriver->sHWInfo.config_ui8NImagers, 1);

        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL) << "initially no gasket should be acquired!";
        }

        CI_GASKET sGasketA, sGasketB, sGasketC;

        CI_GasketInit(&sGasketA);
        CI_GasketInit(&sGasketB);
        CI_GasketInit(&sGasketC);

        sGasketA.uiGasket = pConn->sHWInfo.config_ui8NImagers-1;
        if ( (pConn->sHWInfo.gasket_aType[sGasketA.uiGasket]&CI_GASKET_PARALLEL) )
        {
            sGasketA.bParallel = true;
            sGasketA.ui8ParallelBitdepth = pConn->sHWInfo.gasket_aBitDepth[sGasketA.uiGasket];
        }
        else
        {
            sGasketA.bParallel = false;
        }
        EXPECT_EQ(IMG_SUCCESS, CI_GasketAcquire(&sGasketA, pConn));

        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            if ( i == sGasketA.uiGasket )
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] != NULL) << "gasketA should be acquired!";
            }
            else
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL) << "other gaskets should not be acquired";
            }
        }

        EXPECT_EQ(IMG_SUCCESS, CI_GasketAcquire(&sGasketB, pConn));

        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            if ( i == sGasketA.uiGasket || i == sGasketB.uiGasket )
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] != NULL) << "gasketA and gasketB should be acquired!";
            }
            else
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL) << "other gaskets should not be acquired";
            }
        }

        EXPECT_EQ(IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, CI_GasketAcquire(&sGasketC, pConn));

        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            if ( i == sGasketA.uiGasket || i == sGasketB.uiGasket )
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] != NULL) << "gasketA and gasketB should still be acquired!";
            }
            else
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL) << "other gaskets should not be acquired";
            }
        }

        EXPECT_EQ(IMG_SUCCESS, CI_GasketRelease(&sGasketA, pConn));

        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            if ( i == sGasketB.uiGasket )
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] != NULL) << "gasketB should still be acquired!";
            }
            else
            {
                EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL) << "other gaskets should not be acquired";
            }
        }

        EXPECT_EQ(IMG_SUCCESS, CI_DriverFinalise(pConn));

        // expect cleaning to have happen
        for ( int i = 0 ; i < g_psCIDriver->sHWInfo.config_ui8NImagers ; i++ )
        {
            EXPECT_TRUE(g_psCIDriver->apGasketUser[i] == NULL)  << "all gaskets should be released!";
        }
    }

    ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(g_psCIDriver));
    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));
}

/**
 * @brief test the computed grid size according to different max active width
 */
TEST(CI_Driver, LSH_bufferSize)
{
    /* values are from:
     * HW 2.1 004 (or HW 2.6 004)
     * HW 2.1 014
     * HW 2.4 006 (rounding needed on tile size)
     * HW 2.7 000
     * HW 2.7 003
     */
    IMG_UINT32 aMaxActiveWidth[] = { 6144, 5856, 4100, 1920, 3264};
    IMG_UINT32 aLSHRamSize[] = { 3968, 3712, 2688, 1280, 2176 };
    int i;

    ASSERT_EQ(sizeof(aMaxActiveWidth), sizeof(aLSHRamSize))
        << "badly configured test";

    for (i = 0;
        i < sizeof(aMaxActiveWidth) / sizeof(IMG_UINT32) && !HasFailure();
        i++)
    {
        g_context_max_width = aMaxActiveWidth[i];
        Felix drv;
        KRN_CI_DRIVER *pDriver = NULL;

        drv.configure();

        pDriver = drv.getKrnDriver();

        /* no assert because we want to reset g_context_max_width afterwards
         * so we have to check with IF as well */
        EXPECT_TRUE(NULL != pDriver);
        if (pDriver)
        {
            EXPECT_EQ(aMaxActiveWidth[i],
                pDriver->sHWInfo.context_aMaxWidthSingle[0]);
            EXPECT_EQ(aMaxActiveWidth[i],
                pDriver->sHWInfo.ui32MaxLineStore);
            EXPECT_EQ(aLSHRamSize[i],
                pDriver->sHWInfo.ui32LSHRamSizeBits);
        }
    }

    g_context_max_width = CONTEXT_MAX_WIDTH_DEF;
}

/**
 * @brief Test that calling Hard handler several time is handled by the
 * the threaded interrupt (only Context)
 */
TEST(IMG_CI_Driver, Multiple_interrupts)
{
    FullFelix drv;
    KRN_CI_DRIVER *pDriver = NULL;
    irqreturn_t ret;

    drv.configure();

    pDriver = drv.getKrnDriver();

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(drv.pPipeline));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShootNB(drv.pPipeline));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShootNB(drv.pPipeline));

    // fake we received first frame
    ASSERT_EQ(IMG_SUCCESS, FullFelix::fakeCtxInterrupt(0, true, false));

    EXPECT_EQ(0, pDriver->sWorkqueue.ui32Elements);

    // call handler once
    ret = pDriver->pDevice->irqHardHandle(5, NULL);

    EXPECT_EQ(IRQ_WAKE_THREAD, ret);
    EXPECT_EQ(1, pDriver->sWorkqueue.ui32Elements);

    // fake we receive 2nd interrupt before threaded was called
    ASSERT_EQ(IMG_SUCCESS, FullFelix::fakeCtxInterrupt(0, true, false));

    // call handler another time
    ret = pDriver->pDevice->irqHardHandle(5, NULL);
    EXPECT_EQ(IRQ_WAKE_THREAD, ret);

    EXPECT_EQ(2, pDriver->sWorkqueue.ui32Elements);

    // finally: call threaded handler

    ret = pDriver->pDevice->irqThreadHandle(5, NULL);
    EXPECT_EQ(IRQ_HANDLED, ret);

    EXPECT_EQ(0, pDriver->sWorkqueue.ui32Elements);
}

/**
 * @brief Test that calling Hard handler several time is handled by the
 * the threaded interrupt (but with IIFDG and Context)
 */
TEST(IMG_CI_Driver, Multiple_interrupt_sources)
{
    FullFelix drv;
    KRN_CI_DRIVER *pDriver = NULL;
    CI_DATAGEN *pDG = NULL;
    irqreturn_t ret;
    IMG_RESULT iret;
    CI_CONVERTER converter;
    CI_CONNECTION *pConn = NULL;
    CI_DG_FRAME *pFrame = NULL;
    const int w = 32;
    const int h = 32;

    drv.configure(w, h);

    pDriver = drv.getKrnDriver();
    pConn = drv.getConnection();

    // set IIF data-generator
    iret = CI_DatagenCreate(&pDG, pConn);
    ASSERT_EQ(IMG_SUCCESS, iret);

    // default image is 32x32
    iret = CI_ConverterConfigure(&converter, CI_DGFMT_PARALLEL,
        pConn->sHWInfo.context_aBitDepth[0]);
    ASSERT_EQ(IMG_SUCCESS, iret);

    pDG->eFormat = converter.eFormat;
    pDG->ui8FormatBitdepth = converter.ui8FormatBitdepth;
    pDG->ui8Gasket = drv.pPipeline->sImagerInterface.ui8Imager;

    IMG_UINT32 size = CI_ConverterFrameSize(&converter, w, h);
    ASSERT_GT(size, 0);

    iret = CI_DatagenAllocateFrame(pDG, size, NULL);
    ASSERT_EQ(IMG_SUCCESS, iret);

    iret = CI_PipelineStartCapture(drv.pPipeline);
    ASSERT_EQ(IMG_SUCCESS, iret);

    iret = CI_DatagenStart(pDG);
    ASSERT_EQ(IMG_SUCCESS, iret);

    iret = CI_PipelineTriggerShootNB(drv.pPipeline);
    ASSERT_EQ(IMG_SUCCESS, iret);

    pFrame = CI_DatagenGetAvailableFrame(pDG);

    // normally we would write the frame here
    pFrame->eFormat = converter.eFormat;
    pFrame->ui8FormatBitdepth = converter.ui8FormatBitdepth;
    pFrame->ui32Width = w;
    pFrame->ui32Height = h;
    pFrame->ui32Stride = size / h;
    pFrame->eBayerMosaic = pDG->eBayerMosaic;
    pFrame->ui32HorizontalBlanking = FELIX_MIN_H_BLANKING;
    pFrame->ui32VerticalBlanking = FELIX_MIN_V_BLANKING;


    iret = CI_DatagenInsertFrame(pFrame);
    ASSERT_EQ(IMG_SUCCESS, iret);

    EXPECT_EQ(0, pDriver->sWorkqueue.ui32Elements);

    // fake we received first frame
    ASSERT_EQ(IMG_SUCCESS, FullFelix::fakeCtxInterrupt(0, true, false));

    EXPECT_EQ(0, pDriver->sWorkqueue.ui32Elements);

    // call handler once
    ret = pDriver->pDevice->irqHardHandle(5, NULL);

    EXPECT_EQ(IRQ_WAKE_THREAD, ret);
    EXPECT_EQ(1, pDriver->sWorkqueue.ui32Elements);

    // fake we receive 2nd interrupt before threaded was called
    ASSERT_EQ(IMG_SUCCESS, FullFelix::fakeIIFDgInterrupt(0, false));

    // call handler another time
    ret = pDriver->pDevice->irqHardHandle(5, NULL);
    EXPECT_EQ(IRQ_WAKE_THREAD, ret);

    EXPECT_EQ(2, pDriver->sWorkqueue.ui32Elements);

    // finally: call threaded handler

    ret = pDriver->pDevice->irqThreadHandle(5, NULL);
    EXPECT_EQ(IRQ_HANDLED, ret);

    EXPECT_EQ(0, pDriver->sWorkqueue.ui32Elements);

    iret = CI_ConverterClear(&converter);
}
