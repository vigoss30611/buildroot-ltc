/**
******************************************************************************
@file ci_pipeline_test.cpp

@brief Test the CI_Pipeline functions

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
#include <algorithm>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"
#include "felixcommon/ci_alloc_info.h"

struct CI_Pipeline : public FullFelix, public ::testing::Test
{
    int nBuffers;

    CI_Pipeline() : nBuffers(3) {}

    void SetUp()
    {
        configure(32, 32, YUV_420_PL12_8, PXL_NONE, false, nBuffers, IMG_FALSE);
    }
};

/**
 * @brief Test that CI_PipelineHasPending has the correct values
 */
TEST_F(CI_Pipeline, HasPending)
{
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    ASSERT_EQ(IMG_FALSE, CI_PipelineHasPending(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));

    ASSERT_EQ(IMG_TRUE, CI_PipelineHasPending(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

    ASSERT_EQ(IMG_FALSE, CI_PipelineHasPending(pPipeline));
}

/**
 * @brief Test invalid calls to CI_PipelineHasPending
 */
TEST_F(CI_Pipeline, HasPending_invalid)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasPending(NULL));
    EXPECT_EQ(IMG_FALSE, CI_PipelineHasPending(pPipeline2))
        << "not registered";
}

/**
 * @brief Test that CI_PipelineHasWaiting has the correct values
 */
TEST_F(CI_Pipeline, HasWaiting)
{
    CI_SHOT *pShot;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasWaiting(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

    EXPECT_EQ(IMG_TRUE, CI_PipelineHasWaiting(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pShot));

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasWaiting(pPipeline));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pShot));
}

/**
 * @brief Test invalid calls to CI_PipelineHasWaiting
 */
TEST_F(CI_Pipeline, HasWaiting_invalid)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_LT(CI_PipelineHasWaiting(NULL), 0);
    EXPECT_LT(CI_PipelineHasWaiting(pPipeline2), 0)
        << "not registered";
}

/**
 * @brief Test that CI_PipelineHasAvailableShots(),
 * CI_PipelineHasAvailableBuffers() and CI_PipelineHasAvailable() have the
 * correct values
 */
TEST_F(CI_Pipeline, HasAvailable)
{
    CI_SHOT *pShot;
    IMG_BOOL8 rS, rB;
    int i;

    ASSERT_GT(this->nBuffers, 1) << "need at least 2 buffers to that test";

    // as many shots than buffers
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    for (i = 0; i < this->nBuffers; i++)
    {
        EXPECT_EQ(IMG_TRUE, rS=CI_PipelineHasAvailableShots(pPipeline))
            << " shot " << i << "/" << this->nBuffers;
        EXPECT_EQ(IMG_TRUE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
            << " buffer " << i << "/" << this->nBuffers;
        EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
            << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
            << this->nBuffers;

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
    }

    EXPECT_EQ(IMG_FALSE, rS = CI_PipelineHasAvailableShots(pPipeline))
        << " shot " << i << "/" << this->nBuffers;
    ASSERT_EQ(IMG_FALSE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
        << " buffer " << i << "/" << this->nBuffers;
    EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
        << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
        << this->nBuffers;

    for (i = 0; i < this->nBuffers; i++)
    {
        ASSERT_EQ(IMG_SUCCESS, this->fakeInterrupt(pPipeline->ui8Context));

        // should not be available yet
        EXPECT_EQ(IMG_FALSE, rS = CI_PipelineHasAvailableShots(pPipeline))
            << " shot " << i << "/" << this->nBuffers;
        EXPECT_EQ(IMG_FALSE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
            << " buffer " << i << "/" << this->nBuffers;
        EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
            << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
            << this->nBuffers;
    }

    for (i = 0; i < this->nBuffers; i++)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pShot));
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pShot));
        
        // should be available now
        EXPECT_EQ(IMG_TRUE, rS = CI_PipelineHasAvailableShots(pPipeline))
            << " shot " << i << "/" << this->nBuffers;
        EXPECT_EQ(IMG_TRUE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
            << " buffer " << i << "/" << this->nBuffers;
        EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
            << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
            << this->nBuffers;
    }

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline));

    // add 1 extra shot than buffer so there is always 1 shot free
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, 1));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    for (int i = 0; i < this->nBuffers; i++)
    {
        EXPECT_EQ(IMG_TRUE, rS = CI_PipelineHasAvailableShots(pPipeline))
            << " shot " << i << "/" << this->nBuffers + 1;
        EXPECT_EQ(IMG_TRUE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
            << " buffer " << i << "/" << this->nBuffers;
        EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
            << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
            << this->nBuffers << " shot " << i << "/" << this->nBuffers + 1;

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
    }

    EXPECT_EQ(IMG_TRUE, rS = CI_PipelineHasAvailableShots(pPipeline))
        << " shot " << i << "/" << this->nBuffers + 1;
    EXPECT_EQ(IMG_FALSE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
        << " buffer " << i << "/" << this->nBuffers;
    EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
        << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
        << this->nBuffers << " shot " << i << "/" << this->nBuffers + 1;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pPipeline));

    // delete all shots and add 1 less than buffers
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineDeleteShots(pPipeline));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pPipeline, this->nBuffers-1));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    for (int i = 0; i < this->nBuffers - 1; i++)
    {
        EXPECT_EQ(IMG_TRUE, rS = CI_PipelineHasAvailableShots(pPipeline))
            << " shot " << i << "/" << this->nBuffers - 1;
        EXPECT_EQ(IMG_TRUE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
            << " buffer " << i << "/" << this->nBuffers;
        EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
            << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
            << this->nBuffers << " shot " << i << "/" << this->nBuffers - 1;

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
    }

    EXPECT_EQ(IMG_FALSE, rS = CI_PipelineHasAvailableShots(pPipeline))
        << " shot " << i << "/" << this->nBuffers - 1;
    EXPECT_EQ(IMG_TRUE, rB = CI_PipelineHasAvailableBuffers(pPipeline))
        << " buffer " << i << "/" << this->nBuffers;
    EXPECT_EQ(rS&&rB, CI_PipelineHasAvailable(pPipeline) ? true : false)
        << "rS=" << (int)rS << " rB=" << (int)rB << " buffer " << i << "/"
        << this->nBuffers << " shot " << i << "/" << this->nBuffers - 1;
}

/**
 * @brief Test invalid calls to CI_PipelineHasAvailableShots(),
 * CI_PipelineHasAvailableBuffers() and CI_PipelineHasAvailable() 
 */
TEST_F(CI_Pipeline, HasAvailable_invalid)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailableShots(NULL));
    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailableShots(pPipeline2))
        << "not registered";

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailableBuffers(NULL));
    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailableBuffers(pPipeline2))
        << "not registered";

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailable(NULL));
    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAvailable(pPipeline2))
        << "not registered";
}

/**
 * @brief Test that CI_PipelineHasBuffers has the correct values
 */
TEST_F(CI_Pipeline, HasBuffers)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    EXPECT_LT(CI_PipelineHasBuffers(pPipeline2), 0);

    ASSERT_NE(pPipeline->eEncType.eFmt, PXL_NONE)
        << "test assumes YUV output is enabled!";

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_LT(CI_PipelineHasBuffers(pPipeline2), 0)
        << "not registered yet";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineRegister(pPipeline2));

    EXPECT_EQ(0, CI_PipelineHasBuffers(pPipeline2))
        << "no buffers allocated yet";

    const int nBuff = 5;
    int b;
    IMG_UINT32 bufferId[nBuff];

    for (b = 0; b < nBuff; b++)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pPipeline2,
            CI_TYPE_ENCODER, 0, IMG_FALSE, &bufferId[b]));

        EXPECT_EQ(b+1, CI_PipelineHasBuffers(pPipeline2));
    }

    for (b = nBuff - 1; b >= 0; b--)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineDeregisterBuffer(pPipeline2,
            bufferId[b]));

        EXPECT_EQ(b, CI_PipelineHasBuffers(pPipeline2));
    }
}

/**
 * @brief Test invalid calls to CI_PipelineHasBuffers
 */
TEST_F(CI_Pipeline, HasBuffers_invalid)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_LT(CI_PipelineHasBuffers(NULL), 0);
    EXPECT_LT(CI_PipelineHasBuffers(pPipeline2), 0)
        << "not registered";
}

/**
 * @brief Test that CI_PipelineHasAcquired has the correct values
 */
TEST_F(CI_Pipeline, HasAcquired)
{
    CI_SHOT *pShots[10];
    int nBuff = std::min(nBuffers, 10);
    int i;

    ASSERT_GT(this->nBuffers, 1) << "need at least 2 buffers to that test";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline));

    for (i = 0; i < nBuff; i++)
    {
        EXPECT_EQ(IMG_FALSE, CI_PipelineHasAcquired(pPipeline))
            << "no buffer acquired yet!";

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pPipeline));
        ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));
    }

    // we now should have nBuff waiting buffers

    for (i = 0; i < nBuff; i++)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &pShots[i]));

        EXPECT_EQ(IMG_TRUE, CI_PipelineHasAcquired(pPipeline))
            << i+1 << " buffers acquired already!";
    }

    for (i = 0; i < nBuff; i++)
    {
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, pShots[i]));
    }

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAcquired(pPipeline))
        << "all buffers should have been released!";
}

/**
 * @brief Test invalid calls to CI_PipelineHasAcquired
 */
TEST_F(CI_Pipeline, HasAcquired_invalid)
{
    CI_PIPELINE *pPipeline2 = NULL;

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline2,
        this->getConnection()));

    IMG_MEMCPY(pPipeline2, pPipeline, sizeof(CI_PIPELINE));

    pPipeline2->ui8Context = 1;

    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAcquired(NULL));
    EXPECT_EQ(IMG_FALSE, CI_PipelineHasAcquired(pPipeline2))
        << "not registered";
}

/**
 * @brief Test that allocate check the size
 */
TEST_F(CI_Pipeline, AllocateBuffer)
{
    CI_SIZEINFO sSizeInfo;
    IMG_UINT32 ui32NeededSize = 0;

    ASSERT_TRUE(pPipeline->eEncType.eFmt != PXL_NONE)
        << "badly configured test";

    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&(pPipeline->eEncType),
        pPipeline->ui16MaxEncOutWidth, pPipeline->ui16MaxEncOutHeight,
        IMG_FALSE, &sSizeInfo))
        << "failed to get size info";
    
    ui32NeededSize = sSizeInfo.ui32Stride*sSizeInfo.ui32Height
        + sSizeInfo.ui32CStride*sSizeInfo.ui32CHeight;
    
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pPipeline,
        CI_TYPE_ENCODER, ui32NeededSize, IMG_FALSE, NULL))
        << "failed to allocate minimum size buffer";

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineAllocateBuffer(pPipeline,
        CI_TYPE_ENCODER, ui32NeededSize-10, IMG_FALSE, NULL))
        << "should not have manage to allocate buffer (too small)";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pPipeline,
        CI_TYPE_ENCODER, 2*ui32NeededSize, IMG_FALSE, NULL))
        << "failed to allocate bigger size buffer";

    // : would be better to test all outputs
}

//  add test with trigger using different strides

/**
 * @brief test that a bigger buffer can be allocated to change the capture
 * stride and that the stride value is propagated back in user-space
 */
TEST_F(CI_Pipeline, TriggerWithStride)
{
    CI_SIZEINFO sSizeInfo;
    IMG_UINT32 normalStride = 0;
    IMG_UINT32 ui32NeededSize = 0;
    IMG_UINT32 biggerStride = 0;
    IMG_UINT32 biggerSize = 0;
    IMG_UINT32 id0, id1;
    CI_BUFFID oriId;

    ASSERT_TRUE(pPipeline->eEncType.eFmt != PXL_NONE)
        << "badly configured test";

    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&(pPipeline->eEncType),
        pPipeline->ui16MaxEncOutWidth, pPipeline->ui16MaxEncOutHeight,
        IMG_FALSE, &sSizeInfo))
        << "failed to get size info";

    EXPECT_EQ(sSizeInfo.ui32Stride, sSizeInfo.ui32CStride)
        << "chorma stride is different from luma stride!";
    normalStride = sSizeInfo.ui32Stride;
    ui32NeededSize = normalStride*sSizeInfo.ui32Height
        + normalStride*sSizeInfo.ui32CHeight;

    biggerStride = sSizeInfo.ui32Stride + 10 * SYSMEM_ALIGNMENT;
    biggerSize = biggerStride*sSizeInfo.ui32Height
        + biggerStride*sSizeInfo.ui32CHeight;


    ASSERT_EQ(IMG_SUCCESS, CI_PipelineFindFirstAvailable(pPipeline, &oriId))
        << "failed to get initial IDs";
    ASSERT_GT(oriId.encId, 0u)
        << "did not get a valid ID";

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, biggerSize,
        IMG_FALSE, &id0))
        << "failed to allocate a bigger buffer 0";

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, biggerSize,
        IMG_FALSE, &id1))
        << "failed to allocate a bigger buffer 1";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline))
        << "failed to start pipeline";

    CI_BUFFID ids;
    CI_SHOT *shot0 = NULL, *shot1 = NULL;
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    ids.encStrideY = biggerStride;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "failed to trigger frame 0 with bigger luma stride";

    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id1;
    ids.encStrideC = biggerStride;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "failed to trigger frame 1 with bigger chroma stride";

    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));
    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot0))
        << "could not acquire frame 0 with bigger luma stride";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot1))
        << "could not acquire frame 1 with bigger chroma stride";

    EXPECT_EQ(biggerStride, shot0->aEncYSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height, shot0->aEncYSize[1]);
    EXPECT_EQ(normalStride, shot0->aEncCbCrSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height / pPipeline->eEncType.ui8VSubsampling,
        shot0->aEncCbCrSize[1]);

    EXPECT_EQ(normalStride, shot1->aEncYSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height, shot1->aEncYSize[1]);
    EXPECT_EQ(biggerStride, shot1->aEncCbCrSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height / pPipeline->eEncType.ui8VSubsampling,
        shot1->aEncCbCrSize[1]);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot0));
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot1));

    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    ids.encStrideY = biggerStride;
    ids.encStrideC = biggerStride;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "failed to trigger frame 2 with bigger strides";

    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot0))
        << "could not acquire frame 2 with bigger strides";

    EXPECT_EQ(biggerStride, shot0->aEncYSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height, shot0->aEncYSize[1]);
    EXPECT_EQ(biggerStride, shot0->aEncCbCrSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height / pPipeline->eEncType.ui8VSubsampling,
        shot0->aEncCbCrSize[1]);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot0));

    // check that we cannot trigger a bigger stride with any buffer
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = oriId.encId;
    ids.encStrideY = biggerStride;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should not be able to trigger a bigger stride than computed!";

    // now should be possible as we reset the stride
    ids.encStrideY = normalStride;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "failed to trigger frame 3 with normal chroma stride";

    ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot0))
        << "could not acquire frame 3 with normal strides";

    EXPECT_EQ(normalStride, shot0->aEncYSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height, shot0->aEncYSize[1]);
    EXPECT_EQ(normalStride, shot0->aEncCbCrSize[0]);
    EXPECT_EQ(sSizeInfo.ui32Height / pPipeline->eEncType.ui8VSubsampling,
        shot0->aEncCbCrSize[1]);

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot0));

    // : would be better to test all outputs
}

// : test that driver enforces the stride to be respecting SYSMEM_ALIGNMENT (but not raw2d)

/**
 * @brief verify that we can trigger a capture with an offset and that it is
 * propagated to user-space
 */
TEST_F(CI_Pipeline, TriggerWithOffset)
{
    const unsigned int max_offset = 10 * SYSMEM_ALIGNMENT;
    unsigned int offset;
    CI_SIZEINFO sSizeInfo;
    IMG_UINT32 ui32NeededSize = 0, minChromaOff = 0;
    IMG_UINT32 biggerSize = 0;
    IMG_UINT32 id0, id1;

    ASSERT_TRUE(pPipeline->eEncType.eFmt != PXL_NONE)
        << "badly configured test";

    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&(pPipeline->eEncType),
        pPipeline->ui16MaxEncOutWidth, pPipeline->ui16MaxEncOutHeight,
        IMG_FALSE, &sSizeInfo))
        << "failed to get size info";

    minChromaOff = sSizeInfo.ui32Stride * sSizeInfo.ui32Height;
    ui32NeededSize = minChromaOff
        + sSizeInfo.ui32CStride * sSizeInfo.ui32CHeight;
    
    biggerSize = ui32NeededSize + max_offset;
    
    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, biggerSize,
        IMG_FALSE, &id0))
        << "failed to allocate a bigger buffer 0";

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, biggerSize,
        IMG_FALSE, &id1))
        << "failed to allocate a bigger buffer 1";

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline))
        << "failed to start pipeline";

    CI_BUFFID ids;
    CI_SHOT *shot0 = NULL, *shot1 = NULL;

    for (offset = 0;
        offset < max_offset;
        offset += max_offset / 2)
    {
        IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

        ids.encId = id0;
        ids.encOffsetY = offset;

        EXPECT_EQ(IMG_SUCCESS,
            CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
            << "failed to trigger frame with Y offset " << offset;

        IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

        ids.encId = id1;
        ids.encOffsetC = minChromaOff + offset;

        EXPECT_EQ(IMG_SUCCESS,
            CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
            << "failed to trigger frame with C offset " << offset;

        ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

        ASSERT_EQ(IMG_SUCCESS, fakeInterrupt(pPipeline->ui8Context));

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot0))
            << "could not acquire frame with Y offset " << offset;

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pPipeline, &shot1))
            << "could not acquire frame with C offset " << offset;

        EXPECT_EQ(sSizeInfo.ui32Stride, shot0->aEncYSize[0]);
        EXPECT_EQ(sSizeInfo.ui32Height, shot0->aEncYSize[1]);
        EXPECT_EQ(sSizeInfo.ui32CStride, shot0->aEncCbCrSize[0]);
        EXPECT_EQ(sSizeInfo.ui32CHeight, shot0->aEncCbCrSize[1]);
        EXPECT_EQ(offset, shot0->aEncOffset[0]);
        EXPECT_EQ(minChromaOff + offset, shot0->aEncOffset[1]);

        EXPECT_EQ(sSizeInfo.ui32Stride, shot1->aEncYSize[0]);
        EXPECT_EQ(sSizeInfo.ui32Height, shot1->aEncYSize[1]);
        EXPECT_EQ(sSizeInfo.ui32CStride, shot1->aEncCbCrSize[0]);
        EXPECT_EQ(sSizeInfo.ui32CHeight, shot1->aEncCbCrSize[1]);
        EXPECT_EQ(0, shot1->aEncOffset[0]);
        EXPECT_EQ(minChromaOff + offset, shot1->aEncOffset[1]);

        ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot0));
        ASSERT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pPipeline, shot1));
    }

    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    // does not use max_offset because it fits in the allocated page
    ids.encOffsetY = PAGE_SIZE;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should have failed to trigger frame with Y offset " << offset;

    // : would be better if we also checked other outputs
}

/**
 * @brief verify that we can trigger a capture with an offset and that it is
 * propagated to user-space
 */
TEST_F(CI_Pipeline, TriggerWithOffset_invalidYUV)
{
    unsigned int max_offset = 10 * SYSMEM_ALIGNMENT;
    CI_SIZEINFO sSizeInfo;
    IMG_UINT32 ui32NeededSize = 0, minChromaOff = 0;
    IMG_UINT32 biggerSize = 0;
    IMG_UINT32 id0;

    ASSERT_TRUE(pPipeline->eEncType.eFmt != PXL_NONE)
        << "badly configured test";

    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&(pPipeline->eEncType),
        pPipeline->ui16MaxEncOutWidth, pPipeline->ui16MaxEncOutHeight,
        IMG_FALSE, &sSizeInfo))
        << "failed to get size info";

    EXPECT_EQ(sSizeInfo.ui32Stride, sSizeInfo.ui32CStride)
        << "chorma stride is different from luma stride!";
    
    minChromaOff = sSizeInfo.ui32Stride * sSizeInfo.ui32Height;
    ui32NeededSize = minChromaOff
        + sSizeInfo.ui32CStride * sSizeInfo.ui32CHeight;

    biggerSize = ui32NeededSize + max_offset;
    max_offset = SYSMEM_ALIGNMENT
        +((minChromaOff + (SYSMEM_ALIGNMENT - 1)) / SYSMEM_ALIGNMENT)
        * SYSMEM_ALIGNMENT;

    ASSERT_GT(max_offset, minChromaOff) << "test badly configured";

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, biggerSize,
        IMG_FALSE, &id0))
        << "failed to allocate a bigger buffer 0";
    
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pPipeline))
        << "failed to start pipeline";

    CI_BUFFID ids;
    CI_SHOT *shot0 = NULL;

    // test that chroma offset has to be outside luma
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    ids.encOffsetY = SYSMEM_ALIGNMENT;
    ids.encOffsetC = minChromaOff;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should have failed to trigger frame with Y offset "
        << ids.encOffsetY << " and C offset " << ids.encOffsetC;

    // test that chroma has is after luma
    // (need to start at 64B otherwise 0 means compute C offset)
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    ids.encOffsetY = SYSMEM_ALIGNMENT 
        + sSizeInfo.ui32CStride*sSizeInfo.ui32CHeight;
    ids.encOffsetC = SYSMEM_ALIGNMENT;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should have failed to trigger frame with Y offset "
        << ids.encOffsetY << " and C offset " << ids.encOffsetC;

    // test that buffer size takes offsets into account
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    // the allocation is 1 pages, this offset will not fit
    ids.encOffsetY = PAGE_SIZE;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should have failed to trigger frame with Y offset "
        << ids.encOffsetY << " and C offset " << ids.encOffsetC;

    // test that buffer size takes offsets into account
    IMG_MEMSET(&ids, 0, sizeof(CI_BUFFID));

    ids.encId = id0;
    // the allocation is 1 pages, this offset will not fit
    ids.encOffsetC = PAGE_SIZE;

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED,
        CI_PipelineTriggerSpecifiedShoot(pPipeline, &ids))
        << "should have failed to trigger frame with Y offset "
        << ids.encOffsetY << " and C offset " << ids.encOffsetC;
}

// : add test that ensure offset respect the SYSMEM_ALIGNMENT limitation (but not raw2d)

