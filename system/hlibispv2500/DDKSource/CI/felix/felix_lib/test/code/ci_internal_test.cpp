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

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "unit_tests.h"
#include "felix.h"
extern void InitTalReset(void); // in felix.cpp - used in drivers as well

#include <tal.h>

#include "ci/ci_api.h"
#include "mc/module_config.h"

#include "reg_io2.h"
#include "registers/core.h"

#include "ci_kernel/ci_kernel_structs.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_internal/sys_mem.h"
#include "ci_kernel/ci_ioctrl.h"

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
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, CI_DEF_GMACURVE, &sDevice));

        gNContexts++; // more contexts than supported and more imagers than supported
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, CI_DEF_GMACURVE, &sDevice));

        gNImagers--; // more contexts than supported
        ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, CI_DEF_GMACURVE, &sDevice));

        if ( CI_N_CONTEXT > 1 )
        {
            gNContexts = 1; // less contexts than supported - good
            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, CI_DEF_GMACURVE, &sDevice));

            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));
        }
        if ( CI_N_IMAGERS > 1 )
        {
            gNImagers = 1; // less imagers than supported - good
            ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, bMMU, 256, CI_DEF_GMACURVE, &sDevice));

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
        ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, IMG_TRUE, 256, gma, &sDevice));

        IMG_MEMCPY(gmaCurve_red[gma], sCIDriver.sGammaLUT.aRedPoints, sizeof(IMG_UINT16)*GMA_N_POINTS);
        IMG_MEMCPY(gmaCurve_green[gma], sCIDriver.sGammaLUT.aGreenPoints, sizeof(IMG_UINT16)*GMA_N_POINTS);
        IMG_MEMCPY(gmaCurve_blue[gma], sCIDriver.sGammaLUT.aBluePoints, sizeof(IMG_UINT16)*GMA_N_POINTS);

        ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));
    }
    EXPECT_EQ(IMG_ERROR_FATAL, KRN_CI_DriverCreate(&sCIDriver, IMG_TRUE, 256, gma, &sDevice)) << "should not support another gamma curve! update test if legitimate";

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
    EXPECT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, CI_DEF_GMACURVE, &sDevice));

    sCIDriver.sHWInfo.uiSizeOfPipeline += 10; // fake the fact that the driver was compiled with a different CI_PIPELINE

    EXPECT_EQ(IMG_ERROR_FATAL, CI_DriverInit(&pConn));

    KRN_CI_DriverDestroy(&sCIDriver);
    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));
}

TEST(INT_CI, LSH_Matrix)
{
    Felix driver;
    CI_PIPELINE *pPipeline = NULL;
    MC_PIPELINE sConfiguration;
    float lshCorners[5] = {0.2f, 0.3f, 0.4f, 0.5f, 1.0f};
    int c;

    driver.configure();
    ASSERT_TRUE(driver.getConnection() != NULL);
    IMG_MEMSET(&sConfiguration, 0, sizeof(MC_PIPELINE));

    sConfiguration.sIIF.ui8Imager = 0;
    sConfiguration.sIIF.ui16ImagerSize[0] = 128;
    sConfiguration.sIIF.ui16ImagerSize[1] = 128;
    EXPECT_EQ(IMG_SUCCESS, MC_PipelineInit(&sConfiguration, &(driver.getConnection()->sHWInfo)));

    LSH_CreateMatrix(&(sConfiguration.sLSH.sGrid), 32, 32, LSH_TILE_MIN);
    for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        LSH_FillBowl(&(sConfiguration.sLSH.sGrid), c, lshCorners);
    }
    sConfiguration.sLSH.bUseDeshadingGrid = IMG_TRUE;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline, driver.getConnection()));

    EXPECT_EQ(IMG_SUCCESS, MC_PipelineConvert(&sConfiguration, pPipeline));
    LSH_Free(&(sConfiguration.sLSH.sGrid));

    pPipeline->ui8Context = 0;
    pPipeline->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineRegister(pPipeline)) << "matrix isn't big enought"; // LSH matrix is copied at registration time (1st update)...


    LSH_CreateMatrix(&(sConfiguration.sLSH.sGrid), sConfiguration.sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH, sConfiguration.sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT, LSH_TILE_MIN);
    for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        LSH_FillBowl(&(sConfiguration.sLSH.sGrid), c, lshCorners);
    }

    //EXPECT_EQ(IMG_SUCCESS, MC_PipelineConvert(&sConfiguration, pPipeline));
    // convert only LSH
    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvert(&(sConfiguration.sLSH), &(pPipeline->sDeshading)));
    LSH_Free(&(sConfiguration.sLSH.sGrid));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pPipeline)) << "matrix should be big enough";

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
    ASSERT_EQ(IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, CI_DEF_GMACURVE, &sDevice));

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
