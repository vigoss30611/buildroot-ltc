/**
******************************************************************************
@file ci_api_test.cpp

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

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"
#include "ci_kernel/ci_kernel.h"

#include <target.h>
#include <tal.h>
#include <registers/core.h>

TEST(CI_Driver, init_finalise)
{
    CI_CONNECTION *driverConnection = NULL;
    KRN_CI_DRIVER sCIDriver;
    SYS_DEVICE sDevice;

#ifdef IMG_MALLOC_CHECK
    printf ("IMG:alloc %u - free %u\n", gui32Alloc, gui32Free);
#endif

    ASSERT_TRUE (g_psCIDriver == NULL);

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_DriverInit(NULL));

    sDevice.probeSuccess = NULL;
    EXPECT_EQ(IMG_SUCCESS, SYS_DevRegister(&sDevice));
    EXPECT_EQ (IMG_SUCCESS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, CI_DEF_GMACURVE, &sDevice));
    EXPECT_TRUE (g_psCIDriver != NULL);

    EXPECT_EQ (IMG_SUCCESS, KRN_CI_DriverDestroy(&sCIDriver));

    EXPECT_TRUE (g_psCIDriver == NULL) << "did not remove the driver singleton";

    driverConnection = NULL;

    // default file cannot be found
    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_DriverCreate(&sCIDriver, 0, 256, CI_DEF_GMACURVE, NULL));
    EXPECT_EQ(IMG_SUCCESS, SYS_DevDeregister(&sDevice));

#ifdef IMG_MALLOC_CHECK
    printf ("IMG:alloc %u - free %u\n", gui32Alloc, gui32Free);
#endif
}

#define OVERLAPS(A_sta, A_end, B_sta, B_end) (          \
        ((A_sta) >= (B_sta) && (A_sta) < (B_end) ) ||   \
        ((A_end) > (B_sta) && (A_end) <= (B_end) )      \
        )
// to verify that the sizes were computed correctly
static bool overlaps(CI_LINESTORE *pLinestore, IMG_UINT8 ui8NContexts)
{
    for ( int i = 0 ; i < ui8NContexts ; i++ )
    {
        if ( pLinestore->aStart[i] < 0 )
        {
            continue;
        }
        for ( int j = i+1 ; j < ui8NContexts ; j++ )
        {
            if ( pLinestore->aStart[j] < 0 )
            {
                continue;
            }
            if ( OVERLAPS((IMG_UINT32)pLinestore->aStart[i], (IMG_UINT32)pLinestore->aStart[i]+pLinestore->aSize[i]-1,
                          (IMG_UINT32)pLinestore->aStart[j], (IMG_UINT32)pLinestore->aStart[j]+pLinestore->aSize[j]-1) )
            {
                CI_FATAL("context %d (%d;%d) and context %d (%d;%d) are overlapping\n",
                         i, pLinestore->aStart[i], pLinestore->aStart[i]+pLinestore->aSize[i]-1,
                         j, pLinestore->aStart[j], pLinestore->aStart[j]+pLinestore->aSize[j]-1
                    );
                return true;
            }
        }
    }
    return false;
}
#undef OVERLAPS

TEST(CI_Driver, linestore_verif)
{
    CI_LINESTORE sLine;

    {
        ASSERT_TRUE (g_psCIDriver == NULL); // driver not initialised
        ASSERT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DriverVerifLinestore(NULL, NULL)) << "null parameters";
        ASSERT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_DriverVerifLinestore(NULL, &sLine)) << "null parameter";
    }

    Felix driver;
    CI_CONNECTION *pConnection = NULL;

    driver.configure();

    pConnection = driver.getConnection();

    IMG_MEMCPY(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE));

    ASSERT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_DriverVerifLinestore(pConnection, NULL)) << "null parameter";

    EXPECT_EQ (IMG_SUCCESS, CI_DriverVerifLinestore(pConnection, &sLine)) << "default linestore is valid";
    EXPECT_TRUE(overlaps(&sLine, pConnection->sHWInfo.config_ui8NContexts) == false) << "sizes were not computed correctly";

    // 8k
    EXPECT_EQ (CONTEXT_MAX_WIDTH, pConnection->sHWInfo.ui32MaxLineStore) << "line store value";

    sLine.aStart[0] = pConnection->sHWInfo.ui32MaxLineStore+5;

    EXPECT_NE(0, IMG_MEMCMP(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE))) << "does not modify connection's linestore";

    EXPECT_EQ (IMG_ERROR_VALUE_OUT_OF_RANGE, CI_DriverVerifLinestore(pConnection, &sLine)) << "ctx 0 start further than max";
    //EXPECT_TRUE(overlaps(&sLine, pConnection->sHWInfo.config_ui8NContexts) == false) << "sizes were not computed correctly";

    // reverse linestore
    // | ctx 2        | ctx 1 | ctx 0 |
    //
    sLine.aStart[0] = pConnection->sHWInfo.ui32MaxLineStore/2 + pConnection->sHWInfo.ui32MaxLineStore/4;

    if ( pConnection->sHWInfo.config_ui8NContexts > 1 )
        sLine.aStart[1] = pConnection->sHWInfo.ui32MaxLineStore/2;
    if ( pConnection->sHWInfo.config_ui8NContexts > 2 )
        sLine.aStart[2] = 0;

    // ctx 0 use it all
    sLine.aStart[0] = 0;
    sLine.aStart[1] = -1;

    EXPECT_EQ (IMG_SUCCESS, CI_DriverVerifLinestore(pConnection, &sLine)) << "ctx0 use it all linestore is valid";
    EXPECT_TRUE(overlaps(&sLine, pConnection->sHWInfo.config_ui8NContexts) == false) << "sizes were not computed correctly";

    // ctx 1 use it all
    sLine.aStart[0] = -1;
    sLine.aStart[1] = 0;

    EXPECT_EQ (IMG_SUCCESS, CI_DriverVerifLinestore(pConnection, &sLine)) << "ctx1 use it all linestore is valid";
    EXPECT_TRUE(overlaps(&sLine, pConnection->sHWInfo.config_ui8NContexts) == false) << "sizes were not computed correctly";

    // no ctx
    sLine.aStart[0] = -1;
    sLine.aStart[1] = -1;

    EXPECT_EQ (IMG_ERROR_FATAL, CI_DriverVerifLinestore(pConnection, &sLine)) << "no ctx enabled";

#if CI_N_CONTEXT > 3
#error "add case"
#endif

    /*EXPECT_EQ (IMG_ERROR_VALUE_OUT_OF_RANGE, CI_DriverVerifLinestore(pLine)) << "ctx1 linestore is bigger than its max";
      if ( pConnection->sHWInfo.config_ui8NContexts > 1 )
      EXPECT_EQ (pConnection->sHWInfo.ui16MaxLineStore/4, pLine->aSize[0]);
      if ( pConnection->sHWInfo.config_ui8NContexts > 2 )
      EXPECT_EQ (pConnection->sHWInfo.ui16MaxLineStore/4, pLine->aSize[1]);
      #if CI_N_CONTEXT > 3
      #error "add case"
      #endif*/

    // driver deinit here
}

TEST(CI_Driver, linestore_update)
{
    {
        ASSERT_TRUE (g_psCIDriver == NULL);// driver not initialised
        EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_DriverGetLinestore(NULL));
    }
    Felix driver;
    CI_CONNECTION *pConnection = NULL;
    //CI_LINESTORE sLine;

    driver.configure();

    pConnection = driver.getConnection();

    //IMG_MEMCPY(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE));

    EXPECT_EQ (IMG_SUCCESS, CI_DriverGetLinestore(pConnection));

    ASSERT_NE (640u, g_psCIDriver->sLinestoreStatus.aSize[0]) << "change test value";
    ASSERT_NE (IMG_TRUE, g_psCIDriver->sLinestoreStatus.aActive[0]) << "change test value";

    // fake a started context from another client
    g_psCIDriver->sLinestoreStatus.aSize[0] = 640;
    g_psCIDriver->sLinestoreStatus.aActive[0] = IMG_TRUE;

    // local linestore does not know about this started context
    EXPECT_NE(0, IMG_MEMCMP(&(pConnection->sLinestoreStatus), &(g_psCIDriver->sLinestoreStatus), sizeof(CI_LINESTORE)));

    // update take values from driver
    EXPECT_EQ(IMG_SUCCESS, CI_DriverGetLinestore(pConnection));

    EXPECT_EQ (640, pConnection->sLinestoreStatus.aSize[0]);
    EXPECT_EQ (IMG_TRUE, pConnection->sLinestoreStatus.aActive[0]);
    EXPECT_EQ(0, IMG_MEMCMP(&(pConnection->sLinestoreStatus), &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE)));

    // driver deinit here
}

// CI_DriverSetLinestore function was removed temporarly
TEST(CI_Driver, linestore_set)
{
    Felix driver;
    CI_CONNECTION *pConnection = NULL;
    CI_LINESTORE sLine;

    driver.configure();

    pConnection = driver.getConnection();

    // fake a started context
    g_psCIDriver->sLinestoreStatus.aSize[0] = 640;
    g_psCIDriver->sLinestoreStatus.aActive[0] = IMG_TRUE;

    // update take values from driver
    EXPECT_EQ(IMG_SUCCESS, CI_DriverGetLinestore(pConnection));
    IMG_MEMCPY(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE)); // do a local copy

    ASSERT_TRUE( pConnection->sHWInfo.config_ui8NContexts > 1);

    // move ctx 1: possible it is inactive - it gets bigger
    sLine.aStart[1] = sLine.aSize[0];
    EXPECT_EQ (IMG_SUCCESS, CI_DriverSetLinestore(pConnection, &sLine));
    EXPECT_TRUE(IMG_MEMCMP(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE)) == 0);

    // cannot move ctx 0: it is active
    sLine.aStart[1] -= 10; // reduce ctx of 10
    EXPECT_EQ (IMG_ERROR_FATAL, CI_DriverSetLinestore(pConnection, &sLine));
    EXPECT_TRUE(IMG_MEMCMP(&sLine, &(pConnection->sLinestoreStatus), sizeof(CI_LINESTORE)) != 0);

    //EXPECT_EQ(IMG_SUCCESS, CI_DriverGetLinestore()); // reset correct linestore

    // driver deinit here
}

/**
 * @brief Test the behaviour of the creation of capture that use the whole linestore while having 2 ctx - context 0 tries to get the whole linestore
 */
TEST(CI_Driver, linestore_greedy0)
{
    FullFelix driver;
    CI_CONNECTION *pConnection = NULL;
    CI_PIPELINE *pSecondCtx = NULL;
    //CI_LINESTORE sLine;

    driver.configure();
    pConnection = driver.getConnection();

    // : make as if the configured context is for ctx1
    driver.getKrnPipeline()->userPipeline.ui8Context = 1;
    driver.pPipeline->ui8Context = 1;
    // end of 

    // modify the linestore directly, not very nice. having a local copy is cleaner but this is a test!
    pConnection->sLinestoreStatus.aStart[0] = 0; // context 0 uses the whole linestore
    pConnection->sLinestoreStatus.aStart[1] = -1;

    EXPECT_EQ(IMG_SUCCESS, CI_DriverSetLinestore(pConnection, &(pConnection->sLinestoreStatus)));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pSecondCtx, pConnection));

    IMG_MEMCPY(pSecondCtx, driver.pPipeline, sizeof(CI_PIPELINE));
    pSecondCtx->ui8Context = 0;
    pSecondCtx->ui16MaxDispOutWidth = pConnection->sHWInfo.ui32MaxLineStore;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pSecondCtx));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pSecondCtx, 1));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pSecondCtx, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pSecondCtx));

    EXPECT_EQ(IMG_ERROR_MINIMUM_LIMIT_NOT_MET, CI_PipelineStartCapture(driver.pPipeline));
}

/**
 * @brief Test the behaviour of the creation of capture that use the whole linestore while having 2 ctx - context 1 tries to get the whole linestore
 */
TEST(CI_Driver, linestore_greedy1)
{
    FullFelix driver;
    CI_CONNECTION *pConnection = NULL;
    CI_PIPELINE *pSecondCtx = NULL;
    //CI_LINESTORE sLine;

    driver.configure();
    pConnection = driver.getConnection();

    // modify the linestore directly is not very clean, a local copy is better but this is a test!
    pConnection->sLinestoreStatus.aStart[0] = -1; // -1
    pConnection->sLinestoreStatus.aStart[1] = 0; // context 1 uses the whole linestore

    EXPECT_EQ(IMG_SUCCESS, CI_DriverSetLinestore(pConnection, &(pConnection->sLinestoreStatus)));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pSecondCtx, pConnection));

    IMG_MEMCPY(pSecondCtx, driver.pPipeline, sizeof(CI_PIPELINE));
    pSecondCtx->ui8Context = 1;
    pSecondCtx->ui16MaxDispOutWidth = pConnection->sHWInfo.context_aMaxWidthMult[1]; // is not necessarly the maximum linestore for ctx1

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pSecondCtx));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pSecondCtx, 1));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pSecondCtx, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pSecondCtx));

    // although it is not stricly legit because ctx0 could be starting from (ui16MaxLineStore-context_aMaxWidth[1]) because ctx1 usually uses less linestore than ctx0
    EXPECT_EQ(IMG_ERROR_MINIMUM_LIMIT_NOT_MET, CI_PipelineStartCapture(driver.pPipeline));
}

/// @ move that test
/*TEST(CI_Driver, ConfigRegistration)
  {
  Felix driver; // init drivers
  CI_CONFIG *pConfig = NULL;
  IMG_CI_CONFIG *pPrivateConfig = NULL;
  KRN_CI_CONNECTION *pConnection = driver.getKrnConnection();

  EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ConnectionRegisterConfig(NULL,NULL));

  EXPECT_EQ (IMG_SUCCESS, INT_CI_ConfigCreate(&pPrivateConfig));
  //pPrivateConfig = container_of(pConfig, IMG_CI_CONFIG*, publicConfig);
  pConfig = &(pPrivateConfig->publicConfig);

  pConfig->ui16ImagerSize[1] = 32/CI_CFA_WIDTH;
  pConfig->ui16ImagerSize[0] = 32/CI_CFA_HEIGHT;
  //pConfig->ui32Stride = 128;

  EXPECT_EQ(IMG_SUCCESS, KRN_CI_ConnectionRegisterConfig(pConnection, pPrivateConfig));
  EXPECT_EQ (IMG_ERROR_ALREADY_COMPLETE, KRN_CI_ConnectionRegisterConfig(pConnection, pPrivateConfig)); // register twice fails

  //
  // deregister
  //

  EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ConnectionDeregisterConfig(NULL, NULL));

  EXPECT_EQ (IMG_SUCCESS, KRN_CI_ConnectionDeregisterConfig(pConnection, pPrivateConfig));

  EXPECT_EQ (IMG_ERROR_FATAL, KRN_CI_ConnectionDeregisterConfig(pConnection, pPrivateConfig));

  EXPECT_EQ (IMG_SUCCESS, CI_ConfigDestroy(pConfig));

  // deinit drivers
  }*/

/**
 * Try to update GMA LUT
 */
TEST(CI_Driver, GMALUT_update)
{
    FullFelix driver;
    CI_MODULE_GMA_LUT sOriginalLUT, sNewLUT;

    driver.configure();

    IMG_MEMCPY(&sOriginalLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT));
    IMG_MEMCPY(&sNewLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT));

    {
        for (int i = 1 ; i < GMA_N_POINTS ; i++ ) // because 0 is 0
        {
            //printf("GMA[%u] RED=%u GREEN=%u BLUE=%u\n", i, driver.getConnection()->sGammaLUT.aRedPoints[i], driver.getConnection()->sGammaLUT.aGreenPoints[i], driver.getConnection()->sGammaLUT.aBluePoints[i]);
            sNewLUT.aRedPoints[i]--;
            sNewLUT.aGreenPoints[i]--;
            sNewLUT.aBluePoints[i]--;
        }
    }

    CI_PipelineStartCapture(driver.pPipeline);

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_DriverSetGammaLUT(driver.getConnection(), &sNewLUT));

    EXPECT_NE(0, IMG_MEMCMP(&sNewLUT, &sOriginalLUT, sizeof(CI_MODULE_GMA_LUT)));
    EXPECT_NE(0, IMG_MEMCMP(&sNewLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT)));

    CI_PipelineStopCapture(driver.pPipeline);

    EXPECT_EQ(IMG_SUCCESS, CI_DriverSetGammaLUT(driver.getConnection(), &sNewLUT));
    EXPECT_EQ(0, IMG_MEMCMP(&sNewLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT)));

}

TEST(CI_Driver, GMALUT_directupdate)
{
    FullFelix driver;
    CI_MODULE_GMA_LUT sOriginalLUT, sNewLUT;

    driver.configure();

    IMG_MEMCPY(&sOriginalLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT));
    IMG_MEMCPY(&sNewLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT));

    {
        for (int i = 1 ; i < GMA_N_POINTS ; i++ ) // because 0 is 0
        {
            //printf("GMA[%u] RED=%u GREEN=%u BLUE=%u\n", i, driver.getConnection()->sGammaLUT.aRedPoints[i], driver.getConnection()->sGammaLUT.aGreenPoints[i], driver.getConnection()->sGammaLUT.aBluePoints[i]);
            sNewLUT.aRedPoints[i]--;
            sNewLUT.aGreenPoints[i]--;
            sNewLUT.aBluePoints[i]--;
        }
    }

    IMG_MEMCPY(&(driver.getConnection()->sGammaLUT), &sNewLUT, sizeof(CI_MODULE_GMA_LUT));

    CI_PipelineStartCapture(driver.pPipeline);

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_DriverSetGammaLUT(driver.getConnection(), &(driver.getConnection()->sGammaLUT)));

    EXPECT_NE(0, IMG_MEMCMP(&sOriginalLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT)));

    CI_PipelineStopCapture(driver.pPipeline);

    EXPECT_EQ(IMG_SUCCESS, CI_DriverSetGammaLUT(driver.getConnection(), &(driver.getConnection()->sGammaLUT)));
    EXPECT_NE(0, IMG_MEMCMP(&sOriginalLUT, &(driver.getConnection()->sGammaLUT), sizeof(CI_MODULE_GMA_LUT)));
}

TEST(CI_Driver, specify_buffer)
{
    FullFelix driver;
    CI_PIPELINE *pSpecPipe = NULL;
    KRN_CI_PIPELINE *pNonSpecifyPipe = NULL;
    KRN_CI_PIPELINE *pSpecifyPipe = NULL;
    CI_BUFFID sBuffId;
    IMG_MEMSET(&sBuffId, 0, sizeof(CI_BUFFID));
    IMG_UINT32 encIds[4], dispIds[4];

    driver.configure(40, 40, YVU_420_PL12_8, RGB_888_24, false, 3, IMG_FALSE);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pSpecPipe, driver.getConnection()));

    IMG_MEMCPY(pSpecPipe, driver.pPipeline, sizeof(CI_PIPELINE));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pSpecPipe)) << "failed to register pipeline";

    pNonSpecifyPipe = driver.getKrnPipeline();
    pSpecifyPipe = driver.getKrnPipeline(pSpecPipe);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pSpecPipe, 4)) << "failed to add buffer to pipeline"; // should insert buffer 1, 2, 3
    for ( int b = 0 ; b < 4 ; b++ )
    {
        KRN_CI_BUFFER *pEnc = 0;
        KRN_CI_BUFFER *pDisp = 0;
        sCell_T *pLast = 0;
        int store = 4-b-1;

        // store them in the opposite direction to trigger them from different order than allocated
        EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pSpecPipe, CI_TYPE_ENCODER, 0, IMG_FALSE, &(encIds[store]))) << "failed to allocate YUV buffer";
        EXPECT_EQ(2*b+1, pSpecifyPipe->sList_availableBuffers.ui32Elements);

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pSpecPipe, CI_TYPE_DISPLAY, 0, IMG_FALSE, &(dispIds[store]))) << "failed to allocate RGB buffer";
        EXPECT_EQ(2*(b+1), pSpecifyPipe->sList_availableBuffers.ui32Elements);

        pLast = List_getTail(&(pSpecifyPipe->sList_availableBuffers));
        pEnc = NULL;
        pDisp = NULL;
        while ( pLast != NULL && (!pEnc || !pDisp) )
        {
            KRN_CI_BUFFER *tmp = (KRN_CI_BUFFER*)pLast->object;
            if ( tmp->type == CI_BUFF_DISP )
            {
                pDisp = tmp;
            }
            if ( tmp->type == CI_BUFF_ENC )
            {
                pEnc = tmp;
            }
            pLast = List_getPrev(pLast);
        }

        ASSERT_TRUE(pEnc!=NULL) << "encoder buffer " << b << " not found";
        ASSERT_TRUE(pDisp!=NULL) << "display buffer " << b << " not found";

        EXPECT_EQ(encIds[store], pEnc->ID);
        EXPECT_EQ(dispIds[store], pDisp->ID);

        
    }

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pSpecPipe));
        
    EXPECT_EQ(2*3, pNonSpecifyPipe->sList_availableBuffers.ui32Elements) << "should have 2*nShots elements in buffer list";
    
    //EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_PipelineTriggerShoot(pSpecPipe)) << "should not be able to trigger without specified buffer";

    //int enc[4], disp[4];

    for ( int b = 0 ; b < 4 ; b++ )
    {
        // find and trigger all buffers
        
        sBuffId.encId = encIds[b];
        sBuffId.dispId = dispIds[b];

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerSpecifiedShoot(pSpecPipe, &sBuffId)) << "should use the buffer " << b << " for both encoder and display";

        if ( b == 1 ) // we triggered 2 shots, we will acquire some
        {
            // fake interrupt shot 0
            EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(pSpecifyPipe->userPipeline.ui8Context)) << "fake interrupt failed";

            EXPECT_EQ(2*2, pSpecifyPipe->sList_availableBuffers.ui32Elements) << "should have 2*nShots-4 elements in buffer list";

            CI_SHOT *pShot = NULL;
            INT_SHOT *pIntShot = NULL;

            EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShotNB(pSpecPipe, &pShot)) << "should aqcuire shot";

            pIntShot = container_of(pShot, INT_SHOT, publicShot);
            EXPECT_TRUE(pIntShot->pEncOutput != NULL);
            EXPECT_TRUE(pIntShot->pDispOutput != NULL);
            EXPECT_EQ(encIds[0], pIntShot->pEncOutput->ID);
            EXPECT_EQ(dispIds[0], pIntShot->pDispOutput->ID);

            // return shot 0 - stays in sList_available
            EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pSpecPipe, pShot));

            EXPECT_EQ(2*4-(2*((b+1)-1)), pSpecifyPipe->sList_availableBuffers.ui32Elements) << "we released a shot, 2 more buffers should be available";

            // fake interrupt shot 1
            EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(pSpecifyPipe->userPipeline.ui8Context)) << "fake interrupt failed";

            EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShotNB(pSpecPipe, &pShot)) << "should aqcuire shot";

            pIntShot = container_of(pShot, INT_SHOT, publicShot);
            EXPECT_TRUE(pIntShot->pEncOutput != NULL);
            EXPECT_TRUE(pIntShot->pDispOutput != NULL);
            EXPECT_EQ(encIds[1], pIntShot->pEncOutput->ID);
            EXPECT_EQ(dispIds[1], pIntShot->pDispOutput->ID);

            // not releasing shot 1 - stays in sList_sent
            // will now find and trigger the 2 other buffers
        }
    }
    
    // fake interrupt shot 2 - stays in sList_processed
    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(pSpecifyPipe->userPipeline.ui8Context)) << "fake interrupt failed";
        
    // has 1 shot in every list
    EXPECT_EQ(2, pSpecifyPipe->sList_availableBuffers.ui32Elements);
    EXPECT_EQ(1, pSpecifyPipe->sList_available.ui32Elements);
    EXPECT_EQ(1, pSpecifyPipe->sList_pending.ui32Elements);
    EXPECT_EQ(1, pSpecifyPipe->sList_processed.ui32Elements);
    EXPECT_EQ(1, pSpecifyPipe->sList_sent.ui32Elements);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineDestroy(pSpecPipe));
}

TEST(CI_Driver, timestamp)
{
    IMG_UINT32 timestamp = 4251;

    Felix driver;

    driver.configure();

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DriverGetTimestamp(NULL, NULL)) << "should not work without kernel side connection";
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DriverGetTimestamp(NULL, &timestamp)) << "should not work without kernel side connection";
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_DriverGetTimestamp(driver.getConnection(), NULL)) << "should not work without kernel side connection";
    EXPECT_EQ(4251, timestamp) << "timestamp should not have been accessed";

    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore, FELIX_CORE_FELIX_TS_COUNTER_OFFSET, 6387);
    EXPECT_EQ(IMG_SUCCESS, CI_DriverGetTimestamp(driver.getConnection(), &timestamp));
    EXPECT_EQ(6387, timestamp);
}

TEST(CI_Driver, linestore_reset)
{
    FullFelix driver;
    IMG_UINT32 reset;

    driver.configure(64, 32, YVU_420_PL12_8, PXL_NONE, false, 1);

    reset = g_psCIDriver->sLinestoreStatus.aSize[0];

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    EXPECT_EQ(64, g_psCIDriver->sLinestoreStatus.aSize[0]) << "image size";
    EXPECT_EQ(IMG_TRUE, g_psCIDriver->sLinestoreStatus.aActive[0]) << "no frames were submitted";
    EXPECT_EQ(0, g_psCIDriver->sLinestoreStatus.aStart[0]) << "pipeline is reserved";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline));

    EXPECT_EQ(reset, g_psCIDriver->sLinestoreStatus.aSize[0]) << "max size";
    EXPECT_EQ(IMG_FALSE, g_psCIDriver->sLinestoreStatus.aActive[0]) << "not owned so no captures";
    EXPECT_EQ(IMG_FALSE, g_psCIDriver->sLinestoreStatus.aStart[0]) << "not reserved";
}
