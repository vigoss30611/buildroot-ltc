/**
******************************************************************************
 @file ci_shot_test.cpp

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

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_hwstruct.h"

extern "C" {
    IMG_RESULT IMG_CI_ShotMallocBuffers(KRN_CI_SHOT *pBuffer); // defined in ci_shot.c
}

TEST(KRN_CI_SHOT, acquire)
{
    FullFelix driver;
    /*KRN_CI_SHOT *pBuffer = NULL;
    CI_SHOT *pPublicBuffer = NULL;

    EXPECT_EQ (IMG_SUCCESS, INT_CI_ShotCreate(&pBuffer));

    pPublicBuffer = &(pBuffer->publicBuffer);

    KRN_CI_ShotConfigure(

    //pPublicBuffer->eType = CI_TYPE_ENCODER;
    pPublicBuffer->aEncYSize[0] = 32;
    pPublicBuffer->aEncYSize[1] = 32;
    pPublicBuffer->aEncCbCrSize[0] = 16;
    pPublicBuffer->aEncCbCrSize[1] = 16;
    //pPublicBuffer->ui32Stride = 128;

    EXPECT_EQ (IMG_SUCCESS, INT_CI_ShotMallocBuffers(pBuffer));*/
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    KRN_CI_SHOT *pKrnShot = NULL;
    KRN_CI_BUFFER *pKrnBuffer = NULL;
    sCell_T *pHead = NULL;

    driver.configure();

    pKrnConfig = driver.getKrnPipeline();

    ASSERT_TRUE((pHead=List_getHead(&(pKrnConfig->sList_available))) != NULL);
    
    pKrnShot = (KRN_CI_SHOT*)pHead->object;

    EXPECT_EQ( CI_SHOT_AVAILABLE, pKrnShot->eStatus );
    ASSERT_TRUE(pKrnShot->phwEncoderOutput == NULL); // not attached to shot

    ASSERT_TRUE((pHead=List_getHead(&(pKrnConfig->sList_availableBuffers))) != NULL);

    pKrnBuffer = (KRN_CI_BUFFER*)pHead->object;
    EXPECT_TRUE (Platform_MemGetMemory(&(pKrnBuffer->sMemory)) != NULL);

    // driver deinit here
}

TEST(KRN_CI_SHOT, create_destroy_invalid)
{
    KRN_CI_SHOT *pShot = NULL;
    IMG_RESULT ret;

    gui32AllocFails = 1;
    pShot = KRN_CI_ShotCreate(&ret);
    EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, ret);
    EXPECT_TRUE ( pShot == NULL );

    pShot = KRN_CI_ShotCreate(&ret);
    EXPECT_EQ (IMG_SUCCESS, ret);
    EXPECT_TRUE (pShot != NULL);
    
    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ShotDestroy(NULL));
    // it is possible to destroy the shot if driver pointer is NULL
    //EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, KRN_CI_ShotDestroy(pShot)) << "no drivers";

    //Felix driver; // init drivers
    EXPECT_EQ (IMG_SUCCESS, KRN_CI_ShotDestroy(pShot));
    // deinit drivers here
}

TEST(KRN_CI_SHOT, ShotFromConfig)
{
    FullFelix driver;
    KRN_CI_SHOT *pShot = NULL;
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    IMG_RESULT ret;

    SCOPED_TRACE(::testing::UnitTest::GetInstance()->current_test_info()->name());
    pShot = KRN_CI_ShotCreate(&ret);
    EXPECT_EQ(IMG_SUCCESS, ret);
    EXPECT_TRUE(pShot != NULL);

    driver.configure(SYSMEM_ALIGNMENT, 32, YVU_420_PL12_8, RGB_888_24, false, 1); // cannot configure without output
    driver.getKrnPipeline()->userPipeline.eEncType.eBuffer = TYPE_NONE;
    driver.getKrnPipeline()->userPipeline.eDispType.eBuffer = TYPE_NONE;

    KRN_CI_BufferFromConfig(&(pShot->userShot), driver.getKrnPipeline(), IMG_FALSE);
    EXPECT_EQ(0, pShot->userShot.aEncYSize[0]);
    EXPECT_EQ(0, pShot->userShot.aEncYSize[1]);
    EXPECT_EQ(0, pShot->userShot.aEncCbCrSize[0]);
    EXPECT_EQ(0, pShot->userShot.aEncCbCrSize[1]);
    EXPECT_EQ(0, pShot->userShot.aDispSize[0]);
    EXPECT_EQ(0, pShot->userShot.aDispSize[1]);

    IMG_MEMSET(&(pShot->userShot), 0, sizeof(CI_SHOT));
    driver.getKrnPipeline()->userPipeline.eEncType.eBuffer = TYPE_YUV;

    // 420 PL12
    KRN_CI_BufferFromConfig(&(pShot->userShot), driver.getKrnPipeline(), IMG_FALSE);
    EXPECT_EQ(SYSMEM_ALIGNMENT, pShot->userShot.aEncYSize[0]);
    EXPECT_EQ(32, pShot->userShot.aEncYSize[1]);
    EXPECT_EQ(SYSMEM_ALIGNMENT, pShot->userShot.aEncCbCrSize[0]);
    EXPECT_EQ(16, pShot->userShot.aEncCbCrSize[1]);
    EXPECT_EQ(0, pShot->userShot.aDispSize[0]);
    EXPECT_EQ(0, pShot->userShot.aDispSize[1]);

    IMG_MEMSET(&(pShot->userShot), 0, sizeof(CI_SHOT));
    driver.getKrnPipeline()->userPipeline.eEncType.eBuffer = TYPE_NONE;
    driver.getKrnPipeline()->userPipeline.eDispType.eBuffer = TYPE_RGB;

    // RGB24
    KRN_CI_BufferFromConfig(&(pShot->userShot), driver.getKrnPipeline(), IMG_FALSE);
    EXPECT_EQ(0, pShot->userShot.aEncYSize[0]);
    EXPECT_EQ(0, pShot->userShot.aEncYSize[1]);
    EXPECT_EQ(0, pShot->userShot.aEncCbCrSize[0]);
    EXPECT_EQ(0, pShot->userShot.aEncCbCrSize[1]);
    EXPECT_EQ(3*SYSMEM_ALIGNMENT, pShot->userShot.aDispSize[0]);
    EXPECT_EQ(32, pShot->userShot.aDispSize[1]);

    IMG_MEMSET(&(pShot->userShot), 0, sizeof(CI_SHOT));

    driver.clean();
    driver.configure(SYSMEM_ALIGNMENT, 32, YVU_422_PL12_8, RGB_888_32, false, 1); // cannot configure without output

    // 422 PL12 AND RGB32
    KRN_CI_BufferFromConfig(&(pShot->userShot), driver.getKrnPipeline(), IMG_FALSE);
    EXPECT_EQ(SYSMEM_ALIGNMENT, pShot->userShot.aEncYSize[0]);
    EXPECT_EQ(32, pShot->userShot.aEncYSize[1]);
    EXPECT_EQ(SYSMEM_ALIGNMENT, pShot->userShot.aEncCbCrSize[0]);
    EXPECT_EQ(2*16, pShot->userShot.aEncCbCrSize[1]);
    EXPECT_EQ(4*SYSMEM_ALIGNMENT, pShot->userShot.aDispSize[0]);
    EXPECT_EQ(32, pShot->userShot.aDispSize[1]);
    
    IMG_MEMSET(&(pShot->userShot), 0, sizeof(CI_SHOT));
    EXPECT_EQ(IMG_SUCCESS, KRN_CI_ShotDestroy(pShot));
}

TEST(KRN_CI_SHOT, MallocBuffers)
{
    FullFelix driver;
    KRN_CI_SHOT *pShot = NULL;
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    IMG_RESULT ret;

    pShot = KRN_CI_ShotCreate(&ret);
    EXPECT_EQ(IMG_SUCCESS, ret);
    EXPECT_TRUE(pShot != NULL);

    driver.configure(SYSMEM_ALIGNMENT, 32, YVU_420_PL12_8, RGB_888_24, false, 1); // cannot configure without output

    KRN_CI_BufferFromConfig(&(pShot->userShot), driver.getKrnPipeline(), IMG_FALSE);
    pShot->pPipeline = driver.getKrnPipeline();

    // 5 alloc per buffer in fact (2 internal structures + 3*actual memory) but actual memory allocation is not controlled by gui32AllocFails
    for ( int i = 1 ; i <= 5 ; i++ ) 
    {
        gui32AllocFails = i;
        EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, IMG_CI_ShotMallocBuffers(pShot)) << "buffer " << i;
        EXPECT_TRUE (pShot->phwEncoderOutput == NULL) << "buffer " << i;
        EXPECT_TRUE (pShot->phwDisplayOutput == NULL) << "buffer " << i;
        EXPECT_TRUE (pShot->hwSave.sMemory.pAlloc == NULL) << "buffer " << i;
        EXPECT_TRUE (pShot->hwLoadStructure.pAlloc == NULL) << "buffer " << i;
        EXPECT_TRUE (pShot->hwLinkedList.pAlloc == NULL) << "buffer " << i;
    }

    EXPECT_EQ(IMG_SUCCESS, IMG_CI_ShotMallocBuffers(pShot));
    EXPECT_TRUE (pShot->phwEncoderOutput == NULL) << "dynamic Encoder buffer should be allocated separatly";
    EXPECT_TRUE (pShot->phwDisplayOutput == NULL) << "dynamic Display buffer should be allocated separatly";
    EXPECT_TRUE (pShot->hwSave.sMemory.pAlloc != NULL);
    EXPECT_TRUE (pShot->hwLoadStructure.pAlloc != NULL);
    EXPECT_TRUE (pShot->hwLinkedList.pAlloc != NULL);

    EXPECT_EQ(IMG_SUCCESS, KRN_CI_ShotDestroy(pShot));
}

TEST(KRN_CI_SHOT, configure)
{
    FullFelix driver;
    KRN_CI_SHOT *pShot = NULL;
    KRN_CI_PIPELINE *pPipeline = NULL;
    IMG_RESULT ret;

    driver.configure(32, 32, YVU_420_PL12_8, RGB_888_24, false, 1);
    pPipeline = driver.getKrnPipeline();
    
    pShot = KRN_CI_ShotCreate(&ret);
    EXPECT_EQ (IMG_SUCCESS, ret);
    EXPECT_TRUE (pShot != NULL);
        
    // Configure is done in several steps:
    // - configure shot from config
    // - allocate buffers
    // - setup HW cell

    // these are invalid test cases as the inputs are asserted to be not NULL (it is not accessible by user)
    //EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ShotConfigure(NULL, NULL));
    //EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ShotConfigure(pShot, NULL));
    //EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_CI_ShotConfigure(NULL, pPipeline));

    pShot->pPipeline = pPipeline;
    EXPECT_EQ (IMG_ERROR_ALREADY_INITIALISED, KRN_CI_ShotConfigure(pShot, pPipeline));
    pShot->pPipeline = NULL;

    EXPECT_EQ (IMG_SUCCESS, KRN_CI_ShotConfigure(pShot, pPipeline));

    EXPECT_EQ(IMG_SUCCESS, KRN_CI_ShotDestroy(pShot));
    // driver deinit here
}

TEST(CI_SHOT, displayMap)
{
    FullFelix driver;
    
    driver.configure(32, 32, PXL_NONE, RGB_888_24, false, 1); // 1 rgb buffer
}

/**
 * @brief Tests that gasket count is used properly to know if a frame was missed
 */
TEST(CI_SHOT, missedFrames)
{
    FullFelix driver;
    int gasketCnt = 0;
    CI_SHOT *pShot = NULL;
    
    driver.configure(32, 32, PXL_NONE, RGB_888_24, false, 1); // 1 rgb buffer
    
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    //
    // 1st shot - will not miss a frame
    //
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    // fake that gasket got 1 frame & fake interrupt
    gasketCnt = HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager);
    
    // fake interrupt 
    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(driver.pPipeline->ui8Context)) << "fake interrupt failed";

    EXPECT_EQ(++gasketCnt, HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager)) << "fake interrupt did not increment the interrutp!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

    EXPECT_EQ(0, pShot->i32MissedFrames) << "should not have missed a frame!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));

    //
    // 2nd shot - will miss a frame
    //
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, ++gasketCnt); // fake that we missed a frame

    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(driver.pPipeline->ui8Context)) << "fake interrupt failed";

    EXPECT_EQ(gasketCnt+1, HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager)) << "fake interrupt did not increment the interrutp!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

    EXPECT_EQ(1, pShot->i32MissedFrames) << "should have missed a frame!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));
}

/**
 * @brief Tests that gasket count roll-over is coped by interrupt handler
 */
TEST(CI_SHOT, Gasketrollover)
{
    FullFelix driver;
    CI_SHOT *pShot = NULL;
    
    driver.configure(32, 32, PXL_NONE, RGB_888_24, false, 1); // 1 rgb buffer
    
    // biggest value
    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, (IMG_UINT32)-1);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    //
    // 1st shot - will not miss a frame
    //
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));
        
    // fake interrupt 
    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(driver.pPipeline->ui8Context)) << "fake interrupt failed";

    EXPECT_EQ(0, HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager)) << "fake interrupt did not increment the interrutp! or it did not roll over";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

    EXPECT_EQ(0, pShot->i32MissedFrames) << "should not have missed a frame!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline));



    // biggest value
    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, (IMG_UINT32)-1);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    //
    // will miss a frame
    //
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, 0); // fake that we missed a frame

    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(driver.pPipeline->ui8Context)) << "fake interrupt failed";

    EXPECT_EQ(1, HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager)) << "fake interrupt did not increment the interrutp!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

    EXPECT_EQ(1, pShot->i32MissedFrames) << "should have missed a frame!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline));


    // almost biggest value
    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, ((IMG_UINT32)-1)-5);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    //
    // will miss 10 frames while rolling over
    //
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    FullFelix::writeGasketCount(driver.pPipeline->sImagerInterface.ui8Imager, 4); // fake that we missed 10 frames

    EXPECT_EQ(IMG_SUCCESS, driver.fakeInterrupt(driver.pPipeline->ui8Context)) << "fake interrupt failed";

    EXPECT_EQ(5, HW_CI_GasketFrameCount(driver.pPipeline->sImagerInterface.ui8Imager)) << "fake interrupt did not increment the interrutp!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

    EXPECT_EQ(10, pShot->i32MissedFrames) << "should have missed a frame!";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));
}

// @ add test with KRN_ShotFromConfig with scaler enabled, bypass and no output

