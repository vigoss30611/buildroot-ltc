/**
******************************************************************************
 @file ci_capture_test.cpp

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
#include "ci/ci_api_internal.h"
#include "ci_kernel/ci_kernel.h"
#include <registers/context0.h>
#include <tal.h>
#include <reg_io2.h>

// stolen from ci_connection.
struct count_status
{
    enum KRN_CI_SHOT_eSTATUS eLookingFor;
    int result;
};

static IMG_BOOL8 IMG_CI_PipelineCount(void *elem, void *param)
{
    struct count_status *pParam = (struct count_status*)param;
    KRN_CI_SHOT *pShot = (KRN_CI_SHOT*)elem;

    if ( pShot->eStatus == pParam->eLookingFor )
    {
        pParam->result++;
    }

    return IMG_TRUE;
}
// end of stolen code

class CI_Capture: public FullFelix, public ::testing::Test
{
    void SetUp()
    {
        configure();
    }

    void TearDown()
    {
        //FullFelix::clean(); // the capture & config
        //Felix::clean(); // the driver
    }
};

TEST(FullFelix, threadsafe)
{
  FullFelix driver;

  driver.configure();
}

TEST(Felix, threadsafe)
{
  Felix driver;
  driver.configure();
}

TEST_F(CI_Capture, start_stop)
{
    IMG_UINT32 ui32Val = 0;

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pPipeline->config.ui8Context], FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, &ui32Val);
    EXPECT_EQ(FELIX_CONTEXT0_CAPTURE_MODE_CAP_DISABLE, ui32Val) << "context not started";

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting";

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pPipeline->config.ui8Context], FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, &ui32Val);
    EXPECT_EQ(FELIX_CONTEXT0_CAPTURE_MODE_CAP_LINKED_LIST, ui32Val) << "context not started";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline)) << "stopping";

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pPipeline->config.ui8Context], FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, &ui32Val);
    EXPECT_EQ(FELIX_CONTEXT0_CAPTURE_MODE_CAP_DISABLE, REGIO_READ_FIELD(ui32Val, FELIX_CONTEXT0, CONTEXT_CONTROL, CAPTURE_MODE) ) << "context not started";
}

TEST_F(CI_Capture, triggerShoot)
{
    // driver are init here    
    
    KRN_CI_PIPELINE *pPrivateCapture = this->getKrnPipeline();
    IMG_UINT32 nbBuffers = pPrivateCapture->sList_available.ui32Elements; // number of buffers - not safe without the lock
    CI_SHOT *pBuffer = NULL;
    KRN_CI_SHOT *pPrivateBuffer = NULL;
    IMG_UINT32 acquired = 0;
    CI_SHOT **acquiredShots;
    sCell_T *pHead = NULL;
    
    EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, CI_PipelineTriggerShoot(pPipeline)) << "not started";

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting";

    acquiredShots = (CI_SHOT**)IMG_CALLOC(nbBuffers, sizeof(CI_SHOT*));

    for ( IMG_UINT32 i = 0 ; i < nbBuffers ; i++ )
    {
        pHead = List_getHead(&(pPrivateCapture->sList_available)); // not safe without the lock but nothing else is running
        // capture frame: pops head from config to tail of capture
        if ( pHead != NULL )
        {
            pPrivateBuffer = (KRN_CI_SHOT*)pHead->object;
        
            EXPECT_EQ(CI_SHOT_AVAILABLE, pPrivateBuffer->eStatus) << "from CI_CONFIG";
            EXPECT_EQ (IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline)) << "failed " << i;
            EXPECT_EQ(CI_SHOT_PENDING, pPrivateBuffer->eStatus) << "just pushed in CI_CAPTURE";
        }
        else
        {
            FAIL() << "no element in the list";
        }
    }

    // no more buffers available (all triggered)
    EXPECT_EQ (IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, CI_PipelineTriggerShoot(pPipeline)) << "no more buffers";

    EXPECT_EQ (0, pPrivateCapture->sList_available.ui32Elements);
    EXPECT_EQ (nbBuffers, pPrivateCapture->sList_pending.ui32Elements); // not safe without the lock but nothing else is running
    
    acquired = 0;
    for ( IMG_UINT32 i = 0 ; i < nbBuffers ; i++ )
    {
        pHead = List_getHead(&(pPrivateCapture->sList_pending));
        ASSERT_TRUE(pHead != NULL);
        pPrivateBuffer = (KRN_CI_SHOT*)pHead->object; // not safe without the lock but nothing else is running

        EXPECT_EQ (CI_SHOT_PENDING, pPrivateBuffer->eStatus) << "head of pending";
        EXPECT_EQ (IMG_SUCCESS, Felix::fakeInterrupt(pPrivateCapture)) << "generating fake interrupt failed!";
        EXPECT_EQ (CI_SHOT_PROCESSED, pPrivateBuffer->eStatus) << "just finished by HW";

        EXPECT_EQ (nbBuffers-i-1, pPrivateCapture->sList_pending.ui32Elements) << "wrong number of peding to HW buffers";

        if ( i%2 == 0 )
        {
            pHead = List_getHead(&(pPrivateCapture->sList_processed));

            ASSERT_TRUE(pHead != NULL);
            pPrivateBuffer = (KRN_CI_SHOT*)pHead->object;

            EXPECT_EQ (CI_SHOT_PROCESSED, pPrivateBuffer->eStatus) << "still waiting for HW interrupt";
            EXPECT_EQ (i+1-acquired, pPrivateCapture->sList_processed.ui32Elements) << "wrong number of available to user buffers";
            EXPECT_EQ (IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pBuffer));
            EXPECT_EQ (i+1-(acquired+1), pPrivateCapture->sList_processed.ui32Elements) << "number of available to user buffers did not change!";
            EXPECT_EQ (CI_SHOT_SENT, pPrivateBuffer->eStatus) << "interrupt caught! ready to be acquired";
            //EXPECT_TRUE ( pBuffer == &(pPrivateBuffer->publicBuffer) ); // buffers are not the same anymore
            EXPECT_TRUE (pBuffer != NULL);

            acquiredShots[acquired] = pBuffer;
            acquired++;
        }

        EXPECT_EQ (i+1-acquired, pPrivateCapture->sList_processed.ui32Elements) << "wrong number of available to user buffers";
        EXPECT_EQ (acquired, pPrivateCapture->sList_sent.ui32Elements) << "wrong number of user acquired buffers";
    }

    // release the acquired ones
    for ( IMG_UINT32 i = 0 ; i < acquired ; i++ )
    {        
        pHead = List_getHead(&(pPrivateCapture->sList_sent));
        ASSERT_TRUE(pHead != NULL);
        pPrivateBuffer = (KRN_CI_SHOT*)pHead->object;
        
        EXPECT_EQ (nbBuffers-acquired, pPrivateCapture->sList_processed.ui32Elements) << "wrong number of available to user buffers";
        EXPECT_EQ (acquired-i, pPrivateCapture->sList_sent.ui32Elements) << "wrong number of user acquired buffers";

        EXPECT_EQ (IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, acquiredShots[i]));
        acquiredShots[i] = NULL;

        EXPECT_EQ (i+1, pPrivateCapture->sList_available.ui32Elements) << "wrong number of available";
        EXPECT_EQ(CI_SHOT_AVAILABLE, pPrivateBuffer->eStatus) << "just released and moved back to CI_CONFIG";

        EXPECT_EQ (acquired-(i+1), pPrivateCapture->sList_sent.ui32Elements) << "wrong number of user acquired buffers";
        EXPECT_EQ (nbBuffers-acquired, pPrivateCapture->sList_processed.ui32Elements) << "number of available to user buffers changed!";
    }
    
    // no more acquired
    // nbBuffer/2 in available
    // nbBuffer/2 in filled
    acquired = 0;
    // will put buffers in all lists to check deinit removes them

    for ( IMG_UINT32 i = 0 ; i < 2 ; i++ )
    {
        EXPECT_EQ (IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
        EXPECT_EQ (IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pBuffer));
        
        acquiredShots[i] = pBuffer;
    }
    // need to release before destruction but we don't do it to ensure DDK frees memory anyway!
    /*for( IMG_UINT32 i = 0 ; i < 2 ; i++ )
    {
        EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, acquiredShots[i]));
    }*/

    
    IMG_FREE(acquiredShots);
    // driver will deinit here
}

TEST_F(CI_Capture, triggerShootNB)
{
    KRN_CI_PIPELINE *pPrivateCapture = this->getKrnPipeline();
    IMG_UINT32 nbBuffers = pPrivateCapture->sList_available.ui32Elements; // number of buffers - not safe without the lock
    IMG_UINT32 hwQueue = this->getConnection()->sHWInfo.context_aPendingQueue[0];

    ASSERT_EQ (IMG_ERROR_UNEXPECTED_STATE, CI_PipelineTriggerShootNB(pPipeline)) << "not started";

    if (nbBuffers < hwQueue+1)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, (hwQueue+1 -nbBuffers)));
        for ( unsigned i = 0 ; i < (hwQueue+1 -nbBuffers) ; i++ )
        {
            ASSERT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));
        }
    }

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting";

    for ( unsigned i = 0 ; i < hwQueue ; i++ )
    {
        EXPECT_EQ (IMG_SUCCESS, CI_PipelineTriggerShootNB(pPipeline));
    }

    // now we have 1 more available buffer but no more room in the HW queue
    EXPECT_EQ(1, pPrivateCapture->sList_available.ui32Elements);

    EXPECT_EQ (IMG_ERROR_INTERRUPTED, CI_PipelineTriggerShootNB(pPipeline)) << "HW queue is full";
    EXPECT_EQ(1, pPrivateCapture->sList_available.ui32Elements) << "buffer was not taken";

    // we cheat - add 2 elements to the HW queue list
    SYS_SemIncrement(&(g_psCIDriver->aListQueue[0]));
    SYS_SemIncrement(&(g_psCIDriver->aListQueue[0]));

    // 1st addition should work - still 1 buffer available
    EXPECT_EQ (IMG_SUCCESS, CI_PipelineTriggerShootNB(pPipeline));
    EXPECT_EQ(0, pPrivateCapture->sList_available.ui32Elements) << "no more buffers";

    EXPECT_EQ (IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, CI_PipelineTriggerShootNB(pPipeline)) << "no more available buffer";
}

TEST_F(CI_Capture, privateValue)
{
    IMG_UINT8 oldPrivate = 0;
    KRN_CI_PIPELINE *pPrivateCapture = getKrnPipeline();
    CI_SHOT *pBuffer = NULL;

    oldPrivate = pPipeline->config.ui8PrivateValue;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    // submit two frames
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));

    // fake interrupt for frame 0
    ASSERT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(pPrivateCapture));
    
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pBuffer));
    EXPECT_EQ(oldPrivate, pBuffer->ui8PrivateValue);
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pBuffer));
    pBuffer = NULL;

    pPipeline->config.ui8PrivateValue++; // change capture private value
    // because LSH and DPF needs access to registers
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(pPipeline, CI_UPD_ALL&(~CI_UPD_REG), NULL));

    // this new submitted frame should have a new private value
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));

    // fake interrupt for frame 1 - still using old capture private
    ASSERT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(pPrivateCapture));
    // fake interrupt for frame 2 - still using old capture private
    ASSERT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(pPrivateCapture));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pBuffer));
    EXPECT_EQ(oldPrivate, pBuffer->ui8PrivateValue) << "frame 1 should use original private value (was changed after submitted)";
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pBuffer));
    pBuffer = NULL;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pBuffer));
    EXPECT_EQ(oldPrivate+1, pBuffer->ui8PrivateValue) << "frame 2 should use update private value";
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pBuffer));
    pBuffer = NULL;
}

/**
 * @brief Verify that the basic behaviour of suspend/resume works
 *
 * @brief start a capture, suspend, expect user/kernel to be out of sync, resume
 */
TEST_F(CI_Capture, suspend_resume)
{
    KRN_CI_PIPELINE *pKrnPipeline = this->getKrnPipeline();
    IMG_UINT32 nbBuffers = pKrnPipeline->sList_available.ui32Elements; // number of buffers - not safe without the lock
    INT_PIPELINE *pIntPipeline = container_of(pPipeline, INT_PIPELINE, publicPipeline);
    CI_SHOT *pShot = NULL;
    pm_message_t mess;
    mess.event = 0;

    EXPECT_EQ(IMG_FALSE, pKrnPipeline->bStarted);
    EXPECT_EQ(IMG_FALSE, pIntPipeline->bStarted);

    EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, CI_PipelineTriggerShootNB(pPipeline)) << "not started";

    if (nbBuffers <= 1)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, 2));
        for ( unsigned i = 0 ; i < 2 ; i++ )
        {
            ASSERT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));
        }
        nbBuffers += 2;
    }

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting";

    EXPECT_EQ(IMG_TRUE, pKrnPipeline->bStarted);
    EXPECT_EQ(IMG_TRUE, pIntPipeline->bStarted);

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineTriggerShootNB(pPipeline));
    
    // suspend is called! - message is not used
    EXPECT_EQ(IMG_SUCCESS, g_psCIDriver->pDevice->suspend(g_psCIDriver->pDevice, mess)) << "failed to suspend!";

    EXPECT_EQ(IMG_FALSE, pKrnPipeline->bStarted) << "kernel space should have stopped!";
    EXPECT_EQ(IMG_TRUE, pIntPipeline->bStarted) << "user-space should have lost sync!";

    EXPECT_EQ(IMG_SUCCESS, g_psCIDriver->pDevice->resume(g_psCIDriver->pDevice)) << "failed to resume!";

    EXPECT_EQ(IMG_FALSE, pKrnPipeline->bStarted) << "kernel space should still be stopped!";
    EXPECT_EQ(IMG_TRUE, pIntPipeline->bStarted) << "user-space should still not be in sync!";

    EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, CI_PipelineTriggerShootNB(pPipeline)) << "signal sync was lost!";

    EXPECT_EQ(IMG_FALSE, pKrnPipeline->bStarted) << "kernel space should still be stopped!";
    EXPECT_EQ(IMG_FALSE, pIntPipeline->bStarted) << "user-space shoudl have been updated";

    EXPECT_EQ(IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, CI_PipelineAcquireShotNB(pPipeline, &pShot)) << "buffer should have been cleared as no interrupt was received!";
}

/**
* @brief Ensures that even if pipeline was stopped with processed frames the
* sProcessedSem stays in sync
*/
TEST_F(CI_Capture, semProcessSync)
{
    unsigned int t = 0, i;
    KRN_CI_PIPELINE *pKrnPipeline = getKrnPipeline(pPipeline);
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting";

    while (CI_PipelineHasAvailable(pPipeline))
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShootNB(pPipeline));
        t++;
    }

    ASSERT_GT(t, 1u) << "should have triggered more than 1 frame";
    
    for (i = 0; i < t-1; i++)
    {
        // interrupts
        ASSERT_EQ(IMG_SUCCESS, fakeCtxInterrupt(pPipeline->config.ui8Context));

        // ensure that we added 1 element to the semaphore
        EXPECT_EQ(IMG_SUCCESS,
            SYS_SemTryDecrement(&(pKrnPipeline->sProcessedSem)))
            << "should have been incremented after interrupt";
    }
    // we should have 1 element in the pending and t-1 in processed

    EXPECT_EQ(IMG_ERROR_CANCELLED,
        SYS_SemTryDecrement(&(pKrnPipeline->sProcessedSem)))
        << "should have no more tokens as we decremented after interrupts";

    for (i = 0; i < pKrnPipeline->sList_processed.ui32Elements; i++)
    {
        // re-add all the tokens
        EXPECT_EQ(IMG_SUCCESS,
            SYS_SemIncrement(&(pKrnPipeline->sProcessedSem)))
            << "should not fail incrementing semaphore!";
    }

    // now all is in the same state than if we did not check the semaphore
    // therefore if we stop the pipeline it will flush both lists (pending
    // and processed) and we will have to ensure the semaphore is reset to
    // have no elements

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline));

    EXPECT_EQ(IMG_ERROR_CANCELLED,
        SYS_SemTryDecrement(&(pKrnPipeline->sProcessedSem)))
        << "should have no more tokens as we decremented after stopping";
}

#if CI_N_CONTEXT > 1
#include <registers/core.h>

TEST_F(CI_Capture, multiContext_LS)
{
    IMG_UINT32 ui32Val = 0;
    CI_PIPELINE *pOtherPipeline = NULL;

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "starting"; // LS is written when started

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pPipeline->config.ui8Context], FELIX_CONTEXT0_LS_BUFFER_ALLOCATION_OFFSET, &ui32Val);
    ui32Val = REGIO_READ_FIELD(ui32Val, FELIX_CONTEXT0, LS_BUFFER_ALLOCATION, LS_CONTEXT_OFFSET)*2; // given as pair of pixels
    EXPECT_EQ(pUserConnection->sLinestoreStatus.aStart[pPipeline->config.ui8Context], ui32Val);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pOtherPipeline, pUserConnection));

    memcpy(pOtherPipeline, pPipeline, sizeof(CI_PIPELINE));
    pOtherPipeline->config.ui8Context = 1;
    ASSERT_NE(pOtherPipeline->config.ui8Context, pPipeline->config.ui8Context) << "the pipeline object should have different ctx";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pOtherPipeline));

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pOtherPipeline->config.ui8Context], FELIX_CONTEXT0_LS_BUFFER_ALLOCATION_OFFSET, &ui32Val);
    ui32Val = REGIO_READ_FIELD(ui32Val, FELIX_CONTEXT0, LS_BUFFER_ALLOCATION, LS_CONTEXT_OFFSET)*2; // given as pair of pixels
    EXPECT_NE(pUserConnection->sLinestoreStatus.aStart[pOtherPipeline->config.ui8Context], ui32Val) << "LS should not have been written yet";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pOtherPipeline, 1));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pOtherPipeline, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pOtherPipeline)); // LS is written when started

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pOtherPipeline->config.ui8Context], FELIX_CONTEXT0_LS_BUFFER_ALLOCATION_OFFSET, &ui32Val);
    ui32Val = REGIO_READ_FIELD(ui32Val, FELIX_CONTEXT0, LS_BUFFER_ALLOCATION, LS_CONTEXT_OFFSET)*2; // given as pair of pixels
    EXPECT_EQ(pUserConnection->sLinestoreStatus.aStart[pOtherPipeline->config.ui8Context], ui32Val) << "LS should have been written now...";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline)) << "stopping";

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[pPipeline->config.ui8Context], FELIX_CONTEXT0_CONTEXT_CONTROL_OFFSET, &ui32Val);
    EXPECT_EQ(FELIX_CONTEXT0_CAPTURE_MODE_CAP_DISABLE, REGIO_READ_FIELD(ui32Val, FELIX_CONTEXT0, CONTEXT_CONTROL, CAPTURE_MODE) ) << "context not started";
}

#if FELIX_VERSION_MAJ >= 2
// because HW v1 does not have a difference between aMaxWidthSingle and aMaxWidthMult
#include <algorithm>
/**
 * test that starting a context fails when using sizes > max mult
 */
TEST_F(CI_Capture, start_multifail)
{
    // context 0 runs an image with maximum size
    unsigned size0 = g_psCIDriver->sHWInfo.context_aMaxWidthMult[0]+10;
    unsigned size1 = 0;
    if ( size0 > g_psCIDriver->sHWInfo.context_aMaxWidthSingle[0])
    {
        size0 = g_psCIDriver->sHWInfo.context_aMaxWidthSingle[0];
    }
    size1 = (g_psCIDriver->sHWInfo.ui32MaxLineStore-size0)/2;
    if (size1 > g_psCIDriver->sHWInfo.context_aMaxWidthMult[1])
    {
        size1 = g_psCIDriver->sHWInfo.context_aMaxWidthMult[1];
    }
    
    ASSERT_GT(size1, 0u);
    ASSERT_GT(size0, size1);
    ASSERT_LT(size0+size1, g_psCIDriver->sHWInfo.ui32MaxLineStore);


    ASSERT_EQ(IMG_SUCCESS, CI_PipelineDestroy(pPipeline));
    pPipeline = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline, getConnection()));

    pPipeline->config.ui8Context = 0;
    pPipeline->sImagerInterface.ui8Imager = 0;
    pPipeline->sImagerInterface.eBayerFormat = MOSAIC_RGGB;
    pPipeline->sImagerInterface.ui16ImagerSize[0] = (size0/2) -1;
    pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

    pPipeline->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineRegister(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, 2));

    CI_PIPELINE *pOtherPipeline = NULL;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pOtherPipeline, pUserConnection));

    memcpy(pOtherPipeline, pPipeline, sizeof(CI_PIPELINE));
    pOtherPipeline->config.ui8Context = 1;
    pOtherPipeline->sImagerInterface.ui16ImagerSize[0] = (size1/2) -1;
    ASSERT_NE(pOtherPipeline->config.ui8Context, pPipeline->config.ui8Context) << "the pipeline object should have different ctx";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineRegister(pOtherPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pOtherPipeline, 2));

    // update the linestore
    CI_CONNECTION *pConn = getConnection();
    ASSERT_EQ(IMG_SUCCESS, CI_DriverGetLinestore(pConn));

    // move CT0 further to leave room to CT1
    pConn->sLinestoreStatus.aStart[0] = size1;
    pConn->sLinestoreStatus.aStart[1] = 0;
    ASSERT_EQ(IMG_SUCCESS, CI_DriverSetLinestore(pConn, &(pConn->sLinestoreStatus)));

    // start CT1 - uses the maximum multi-context widht
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pOtherPipeline));

    EXPECT_EQ(IMG_ERROR_MINIMUM_LIMIT_NOT_MET, CI_PipelineStartCapture(pPipeline)) << "should not be able to reserve HW for single use as CT1 is running";

    // stop to allow CT0 to start
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pOtherPipeline));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "CT1 has been stopped - CT0 should be able to start now";

    EXPECT_EQ(IMG_ERROR_MINIMUM_LIMIT_NOT_MET, CI_PipelineStartCapture(pOtherPipeline)) << "should not be able to reserve HW as CT1 is running with limit over mult max size";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline));
}
#endif // FELIX_VERSION_MAJ

// need 2 running contexts for that
TEST_F(CI_Capture, dataExtraction)
{
    IMG_UINT32 regval;
    KRN_CI_PIPELINE *pPrivateCapture = this->getKrnPipeline();
    
    pPrivateCapture->pipelineConfig.eDataExtraction = CI_INOUT_FILTER_LINESTORE;
    ASSERT_EQ(0, pPipeline->config.ui8Context);

    EXPECT_NE(CI_INOUT_FILTER_LINESTORE, g_psCIDriver->eCurrentDataExtraction) << "default DE point changed";
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore, FELIX_CORE_DE_CTRL_OFFSET, &regval);
    EXPECT_NE(FELIX_CORE_DE_SELECT_SEL_DE_FLS_IN, regval);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline)) << "correct extraction point";

    EXPECT_EQ(CI_INOUT_FILTER_LINESTORE, g_psCIDriver->eCurrentDataExtraction) << "DE point changed";
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore, FELIX_CORE_DE_CTRL_OFFSET, &regval);
    EXPECT_EQ(FELIX_CORE_DE_SELECT_SEL_DE_FLS_IN, regval);

    CI_PIPELINE *otherCapture = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&otherCapture, pUserConnection));
    regval = 0;
    
    IMG_MEMCPY(otherCapture, this->pPipeline, sizeof(CI_PIPELINE));
    otherCapture->config.eDataExtraction = CI_INOUT_BLACK_LEVEL;

    otherCapture->config.ui8Context = 1;
    otherCapture->sImagerInterface.ui8Imager = 1;
    otherCapture->config.ui16MaxDispOutWidth = 32;
    otherCapture->config.ui16MaxDispOutHeight = 32;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineRegister(otherCapture));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(otherCapture, 5));
    for ( unsigned i = 0 ; i < 5 ; i++ )
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(otherCapture, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));
    }
    
    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineStartCapture(otherCapture)) << "different data extraction";

    EXPECT_EQ(CI_INOUT_FILTER_LINESTORE, g_psCIDriver->eCurrentDataExtraction) << "did not change DE point";
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore, FELIX_CORE_DE_CTRL_OFFSET, &regval);
    EXPECT_EQ(FELIX_CORE_DE_SELECT_SEL_DE_FLS_IN, regval);
}

/// @ add test to try start a capture on the same context than another
/// @ add test to try start a capture on the same imager than another

#endif // more than 1 context
