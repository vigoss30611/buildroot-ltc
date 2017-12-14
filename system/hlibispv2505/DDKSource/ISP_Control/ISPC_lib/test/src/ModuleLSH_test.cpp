/**
*******************************************************************************
 @file ModuleLSH_test.cpp

 @brief Test the LSH module class

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

#include "ispc/Camera.h"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleOUT.h"
#include "ci/ci_api.h"
#include "felix.h"
#include "ispc_utest.h"

#include <gtest/gtest.h>

struct ModuleLSH_test : ::testing::Test, ISPCUTest
{
    ISPC::ModuleLSH *pLSH;  // LSH module in the camera

    ModuleLSH_test()
    {
        pLSH = NULL;
    }

    virtual void SetUp()
    {
        if(configure() != IMG_SUCCESS)
        {
            FAIL();
        }

        pLSH = pCamera->getModule<ISPC::ModuleLSH>();
        if (NULL == pLSH)
        {
            std::cerr << "failed to get LSH module!" << std::endl;
            delete pCamera;
            pCamera = NULL;
        }
    }

    virtual bool checkReady()
    {
        EXPECT_TRUE(pCamera != NULL)
            << "camera not created";
        EXPECT_TRUE(pSensor != NULL)
            << "camera not created";
        EXPECT_TRUE(pLSH != NULL)
            << "LSH module not populated";
        if(HasFatalFailure() ||
           HasNonfatalFailure())
        {
            return false;
        }
        return true;
    }

    virtual void TearDown()
    {
        finalise();
        pLSH = NULL;
    }
};

TEST_F(ModuleLSH_test, addGrid)
{
    int tile = LSH_TILE_MIN;

    std::list<IMG_UINT32> matIds;
    IMG_RESULT ret;

    ASSERT_TRUE(checkReady());
    auto tileSizes = getLSHTileSizes();

    for (auto tile: tileSizes)
    {
        IMG_UINT32 mat = 0;
        IMG_UINT32 otherMat = 0;
        LSH_GRID grid = LSH_GRID();

        ret = startCapture(2, false);
        EXPECT_EQ(IMG_SUCCESS, ret) << "failed to start for ts=" << tile;

        ret = newLSHGrid(grid, tile);
        if (IMG_SUCCESS != ret)
        {
            ADD_FAILURE() << "failed to create for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            continue;  // try next tile size
        }

        ret = pLSH->addMatrix(grid, mat);
        if (IMG_SUCCESS != ret)
        {
            LSH_Free(&grid);
            ADD_FAILURE() << "failed to add for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            continue;  // try next tile size
        }
        // from now on grid is added, no need to free it
        std::cout << "new matrix " << mat << std::endl;
        matIds.push_back(mat);

        ret = pLSH->configureMatrix(mat);
        if (IMG_SUCCESS != ret)
        {
            ADD_FAILURE() << "failed to configure matrix 1 for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            ret = pLSH->configureMatrix(0);
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to configure matrix to NULL after error for ts="
                << tile;
            continue;  // try next tile size
        }

        LSH_GRID otherGrid = LSH_GRID();

        ret = newLSHGrid(otherGrid, tile);
        if (IMG_SUCCESS != ret)
        {
            ADD_FAILURE() << "failed to create for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            continue;  // try next tile size
        }

        ret = pLSH->addMatrix(otherGrid, otherMat);
        if (IMG_SUCCESS != ret)
        {
            LSH_Free(&otherGrid);
            ADD_FAILURE() << "failed to add matrix 2 for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            ret = pLSH->configureMatrix(0);
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to configure matrix to NULL after error for ts="
                << tile;
            continue;  // try next tile size
        }
        // from now on otherGrid is added, no need to free it
        std::cout << "new matrix " << otherMat << std::endl;
        matIds.push_back(otherMat);

        ret = stopCapture();
        ASSERT_EQ(IMG_SUCCESS, ret) << "failed to stop for ts=" << tile;

        ret = pLSH->configureMatrix(otherMat);
        if (IMG_SUCCESS != ret)
        {
            ADD_FAILURE() << "failed to configure matrix 2 for ts=" << tile;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            ret = pLSH->configureMatrix(0);
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to configure matrix to NULL after error for ts="
                << tile;
        }

        ret = startCapture(2, true);
        ASSERT_EQ(IMG_SUCCESS, ret) << "failed to restart for ts=" << tile;

        ret = stopCapture();
        ASSERT_EQ(IMG_SUCCESS, ret) << "failed to restop for ts=" << tile;

        // ensure no matrix is configured for next tile size
        ret = pLSH->configureMatrix(0);
        ASSERT_EQ(IMG_SUCCESS, ret)
            << "failed to configure matrix to NULL for ts=" << tile;
    }

    std::list<IMG_UINT32>::iterator it;
    for (it = matIds.begin(); it != matIds.end(); it++)
    {
        ret = pLSH->removeMatrix(*it);
        EXPECT_EQ(IMG_SUCCESS, ret) << "failed to remove mat " << *it;
    }
    matIds.clear();
}

/**
 * Check that changing current matrix reports updated ID
 */
TEST_F(ModuleLSH_test, getCurrent)
{
    LSH_GRID grid = LSH_GRID();
    IMG_RESULT ret;
    IMG_UINT32 matId = 0, otherMatId = 0;

    ASSERT_TRUE(checkReady());

    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "initial id should be 0";

    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 1 failed";

    ret = pLSH->addMatrix(grid, matId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 1 failed";

    grid = LSH_GRID();
    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 2 failed";

    ret = pLSH->addMatrix(grid, otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 2 failed";

    EXPECT_EQ(0, pLSH->getCurrentMatrixId())
        << "adding matrix should not change current matrix";

    ret = pLSH->configureMatrix(matId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(matId, pLSH->getCurrentMatrixId())
        << "should have updated from 0 to matId";

    ret = pLSH->configureMatrix(otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(otherMatId, pLSH->getCurrentMatrixId())
        << "should have updated from matId to otherMatId";

    ret = pLSH->configureMatrix(0);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(0, pLSH->getCurrentMatrixId())
        << "should have updated from otherMatId to 0";
}

/**
 * Duplicate behaviour of ISPC_tcp
 */
TEST_F(ModuleLSH_test, addWhileRunning)
{
    LSH_GRID grid = LSH_GRID();
    IMG_RESULT ret;
    IMG_UINT32 matId = 0, otherMatId = 0;

    ASSERT_TRUE(checkReady());

    startCapture(2, true);

    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "initial id should be 0";

    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 1 failed";

    ret = pLSH->addMatrix(grid, matId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 1 failed";

    grid = LSH_GRID();
    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 2 failed";

    ret = pLSH->addMatrix(grid, otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 2 failed";

    EXPECT_EQ(0, pLSH->getCurrentMatrixId())
        << "adding matrix should not change current matrix";

    // now try to change matrices

    stopCapture();

    ret = pLSH->configureMatrix(matId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(matId, pLSH->getCurrentMatrixId())
        << "should have updated from 0 to matId";

    startCapture(2, true);
    stopCapture();

    /*
    ret = pLSH->configureMatrix(0);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(0, pLSH->getCurrentMatrixId())
        << "should have updated from otherMatId to 0";
    */

    ret = pLSH->configureMatrix(otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(otherMatId, pLSH->getCurrentMatrixId())
        << "should have updated from matId to otherMatId";

    startCapture(2, true);
    stopCapture();

    /*
    ret = pLSH->configureMatrix(0);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(0, pLSH->getCurrentMatrixId())
        << "should have updated from otherMatId to 0";
    */

    ret = pLSH->configureMatrix(matId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    EXPECT_EQ(matId, pLSH->getCurrentMatrixId())
        << "should have updated from 0 to matId";

    // check that we don't get an already mapped virtual address
    startCapture(2, true);

    // end
    stopCapture();
}

/**
 * Check that remove current matrix possible in different situations
 */
TEST_F(ModuleLSH_test, removeMatrix)
{
    LSH_GRID grid = LSH_GRID();
    IMG_RESULT ret;
    IMG_UINT32 matId = 0, otherMatId = 0;

    ASSERT_TRUE(checkReady());

    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "initial id should be 0";

    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 1 failed";

    ret = pLSH->addMatrix(grid, matId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 1 failed";

    grid = LSH_GRID();
    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 2 failed";

    ret = pLSH->addMatrix(grid, otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 2 failed";

    ret = pLSH->configureMatrix(matId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    ASSERT_EQ(matId, pLSH->getCurrentMatrixId())
        << "should have updated from 0 to matId";

    ret = pLSH->removeMatrix(matId);
    EXPECT_EQ(IMG_SUCCESS, ret)
        << "should be able to delete current matrix because capture is not started";
    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "should have been reset to 0";

    ret = pLSH->configureMatrix(otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret);
    ASSERT_EQ(otherMatId, pLSH->getCurrentMatrixId())
        << "should have updated from matId to otherMatId";

    ret = startCapture(2, false);
    ASSERT_EQ(IMG_SUCCESS, ret) << "failed to start capture";

    ret = pLSH->removeMatrix(otherMatId);
    EXPECT_EQ(IMG_ERROR_FATAL, ret)
        << "should not be able to delete current matrix as capture is started";
    ASSERT_EQ(otherMatId, pLSH->getCurrentMatrixId())
        << "should not have affected current matrix";

    ret = pLSH->configureMatrix(0);
    ASSERT_EQ(IMG_SUCCESS, ret)
        << "should be able to disable matrix while capture is started";
    ASSERT_EQ(0, pLSH->getCurrentMatrixId())
        << "should have updated from otherMatId to 0";
    ret = pLSH->removeMatrix(otherMatId);
    EXPECT_EQ(IMG_SUCCESS, ret)
        << "should be able to delete previous matrix";

    ret = stopCapture();
    EXPECT_EQ(IMG_SUCCESS, ret) << "failed to stop the capture";
}
