/**
*******************************************************************************
 @file ControlLSH_test.cpp

 @brief Test the LSH control class

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
#include "ispc/ControlLSH.h"
#include "ci/ci_api.h"
#include "felix.h"
#include "ispc_utest.h"

#include <gtest/gtest.h>

struct FakeAWB : public ISPC::ControlAWB
{
    /*
     * functions to disable ControlAWB
     */
    virtual IMG_RESULT load(const ISPC::ParameterList &parameters)
    {
        return IMG_SUCCESS;
    }

    virtual IMG_RESULT save(ISPC::ParameterList &parameters,
        ModuleBase::SaveType t) const
    {
        return IMG_SUCCESS;
    }

    virtual IMG_RESULT update(const ISPC::Metadata &metadata)
    {
        return IMG_SUCCESS;
    }

    /*
     * functions to use to set measured temperature to choose LSH
     */

    FakeAWB() : ISPC::ControlAWB()
    {

    }

    void setMeasuredTemperature(double t)
    {
        this->measuredTemperature = t;
    }


};

struct ControlLSH_test : ::testing::Test, ISPCUTest
{
    ISPC::ModuleLSH *pLSH;  // LSH module in the camera
    ISPC::ControlLSH *pCLSH;  // LSH control in the camera
    FakeAWB *pAWB;  // AWB control in the camera

    ControlLSH_test()
    {
        pLSH = NULL;
        pCLSH = NULL;
        pAWB = NULL;
    }

    virtual void addControlModules()
    {
        pAWB = new FakeAWB();
        pCLSH = new ISPC::ControlLSH();
        pCLSH->registerCtrlAWB(pAWB);

        pCamera->registerControlModule(pAWB);
        pCamera->registerControlModule(pCLSH);
    }

    virtual void SetUp()
    {
        // will call addControlModules()
        if(configure() != IMG_SUCCESS)
        {
            pLSH = NULL;
            pCLSH = NULL;
            pAWB = NULL;
            return;
        }

        pLSH = pCamera->getModule<ISPC::ModuleLSH>();
        if (NULL == pLSH)
        {
            std::cerr << "failed to get LSH module!" << std::endl;
            delete pCamera;
            pCamera = NULL;
            pCLSH = NULL;
            pAWB = NULL;
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
        EXPECT_TRUE(pCLSH != NULL)
            << "LSH control not populated";
        EXPECT_TRUE(pAWB != NULL)
            << "AWB control not populated";

        if(HasFatalFailure() ||
           HasNonfatalFailure())
        {
            return false;
        }
        return true;
    }

    virtual void TearDown()
    {
        finalise();  // delete the camera
        pLSH = NULL;
        pCLSH = NULL;
        pAWB = NULL;
    }
};

/**
 * Check that both enabled and pCtrlAWB are check for isEnabled()
 */
TEST_F(ControlLSH_test, isEnabled)
{
    ASSERT_TRUE(checkReady());

    ASSERT_TRUE(pCLSH->isEnabled())
        << "assumes both pCtrlAWB and enabled are set";

    pCLSH->enableControl(false);
    ASSERT_FALSE(pCLSH->isEnabled())
        << "should not be enabled if disabled && pAWB != NULL";

    pCLSH->registerCtrlAWB(NULL);
    ASSERT_FALSE(pCLSH->isEnabled())
        << "should not be enabled if !enabled && pAWB == NULL";

    pCLSH->enableControl(true);
    ASSERT_FALSE(pCLSH->isEnabled())
        << "should not be enabled on if enabled && pAWB == NULL";

    pCLSH->registerCtrlAWB(pAWB);
    ASSERT_TRUE(pCLSH->isEnabled())
        << "should be enabled if enabled && pAWB != NULL";
}

TEST_F(ControlLSH_test, addGrid)
{
    int tile = LSH_TILE_MIN;
    ISPC::ControlLSH::IMG_TEMPERATURE_TYPE temp = 2500;
    std::list<IMG_UINT32> matIds;
    IMG_RESULT ret;

    ASSERT_TRUE(checkReady());

    auto tileSizes = getLSHTileSizes();
    pCLSH->enableControl(false);  // so that we can choose the matrix
    ASSERT_GT(tileSizes.size(), 0u);

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

        ret = pCLSH->addMatrix(++temp, grid, mat);
        if (IMG_SUCCESS != ret)
        {
            LSH_Free(&grid);
            ADD_FAILURE() << "failed to add for ts=" << tile
                << " temp=" << temp;
            ret = stopCapture();
            ASSERT_EQ(IMG_SUCCESS, ret)
                << "failed to stop after error for ts=" << tile;
            continue;  // try next tile size
        }
        // from now on grid is added, no need to free it
        std::cout << "new matrix " << otherMat << " temp=" << temp
            << std::endl;
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

        // increasing temperature or addition fails
        ret = pCLSH->addMatrix(++temp, otherGrid, otherMat);
        if (IMG_SUCCESS != ret)
        {
            LSH_Free(&otherGrid);
            ADD_FAILURE() << "failed to add matrix 2 for ts=" << tile
                << " temp=" << temp;
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
        std::cout << "new matrix " << otherMat << " temp=" << temp
            << std::endl;
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

    // get a temperature that is already used to ensure it fails
    if (matIds.size() > 0)
    {
        LSH_GRID lastGrid = LSH_GRID();
        IMG_UINT32 lastMat = 0;

        ret = newLSHGrid(lastGrid, *tileSizes.begin());
        if (IMG_SUCCESS != ret)
        {
            ADD_FAILURE() << "failed to add for ts=" << tile;
        }
        else
        {
            temp = pCLSH->getTemperature(*(matIds.begin()));

            ret = pCLSH->addMatrix(temp, lastGrid, lastMat);
            if (IMG_SUCCESS == ret)
            {
                ADD_FAILURE() << "should have failed to add temperature "
                    << temp << " twice" << std::endl;
                matIds.push_back(lastMat);
            }
            else
            {
                LSH_Free(&lastGrid);
            }
        }
    }

    std::list<IMG_UINT32>::iterator it;
    for (it = matIds.begin(); it != matIds.end(); it++)
    {
        ret = pCLSH->removeMatrix(*it);
        EXPECT_EQ(IMG_SUCCESS, ret) << "failed to remove mat " << *it;
    }
    matIds.clear();
}

/**
 * Check that changing current matrix reports updated ID
 */
TEST_F(ControlLSH_test, chooseMatrix)
{
    IMG_RESULT ret;
    IMG_UINT32 mat;
    const ISPC::ControlLSH::IMG_TEMPERATURE_TYPE temp[] = {
        2500, 3200, 5000, 6500
    };
    const size_t nTemp =
        sizeof(temp) / sizeof(ISPC::ControlLSH::IMG_TEMPERATURE_TYPE);
    std::map<ISPC::ControlLSH::IMG_TEMPERATURE_TYPE, IMG_UINT32> mapTemp2Matid;
    int i;

    ASSERT_TRUE(checkReady());

    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "initial id should be 0";
    EXPECT_EQ(0, pCLSH->getChosenMatrixId()) << "initial id should be 0";

    for (i = 0; i < nTemp; i++)
    {
        LSH_GRID grid = LSH_GRID();

        ret = newLSHGrid(grid, LSH_TILE_MIN);
        ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix " << temp[i]
            << " failed";
        /* we use assert which will make us leak all previously allocated
         * matrices on failure */

        ret = pCLSH->addMatrix(temp[i], grid, mat);
        ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix " << temp[i] << " failed";
        /* we use assert which will make us leak all previously allocated
         * matrices on failure */

        // to ensure all matrix are applicable and that last one is loaded
        ret = pLSH->configureMatrix(mat);
        ASSERT_EQ(IMG_SUCCESS, ret)
            << "could not apply matrix " << mat << " for temp " << temp[i];

        mapTemp2Matid[temp[i]] = mat;
    }

    pCLSH->enableControl(true);
    startCapture(2, false);

    ISPC::Metadata meta;

    // try to set AWB temperature to all added matrices to ensure they are used
    for (i = 0; i < nTemp; i++)
    {
        mat = pCLSH->getMatrixId(temp[i]);

        EXPECT_EQ(mapTemp2Matid[temp[i]], mat)
            << "ControlLSH does not report expected mat for temp " << temp[i];
        EXPECT_EQ(temp[i], pCLSH->getTemperature(mat))
            << "ControlLSH is not consistent between matrixId and temperature";

        // we applied all matrices so last one should be applied when we start
        EXPECT_NE(mat, pLSH->getCurrentMatrixId())
            << "matrix for temp " << temp[i] << " is already applied!";

        // we update what AWB reports as temperature
        pAWB->setMeasuredTemperature(temp[i]);
        // and set CLSH to ask AWB what temperature to use
        pCLSH->update(meta);

        EXPECT_EQ(temp[i], pCLSH->getChosenTemperature())
            << "ControlLSH did not update the temperature to " << temp[i];
        EXPECT_EQ(mat, pCLSH->getChosenMatrixId())
            << "ControlLSH did not apply the matrix "
            << "registered for temp " << temp[i];
        EXPECT_EQ(pCLSH->getChosenMatrixId(), pLSH->getCurrentMatrixId())
            << "expecting LSH and CLSH to have the same matrix!";
    }

    ASSERT_EQ(ISPC::ControlLSH::LINEAR, pCLSH->getAlgorithm())
        << "assumes linear algorithm is used";

    // check that the linear choice is correct
    for (i = 0; i < nTemp - 1; i++)
    {
        IMG_UINT32 otherMat;
        ISPC::ControlLSH::IMG_TEMPERATURE_TYPE middleTemp
            = (temp[i + 1] + temp[i]) / 2;
        mat = pCLSH->getMatrixId(temp[i]);
        otherMat = pCLSH->getMatrixId(temp[i + 1]);

        pAWB->setMeasuredTemperature(middleTemp - 1);
        ret = pCLSH->update(meta);
        ASSERT_EQ(IMG_SUCCESS, ret);

        EXPECT_EQ(mat, pCLSH->getChosenMatrixId())
            << "setting temp=" << middleTemp - 1 << " between " << temp[i]
            << " and " << temp[i + 1] << " did not give correct matrix";
        EXPECT_EQ(temp[i], pCLSH->getChosenTemperature())
            << "setting temp=" << middleTemp - 1 << " between " << temp[i]
            << " and " << temp[i + 1] << " did not give correct temperature";

        pAWB->setMeasuredTemperature(middleTemp + 1);
        pCLSH->update(meta);

        EXPECT_EQ(otherMat, pCLSH->getChosenMatrixId())
            << "setting temp=" << middleTemp + 1 << " between " << temp[i]
            << " and " << temp[i + 1] << " did not give correct matrix";
        EXPECT_EQ(temp[i + 1], pCLSH->getChosenTemperature())
            << "setting temp=" << middleTemp + 1 << " between " << temp[i]
            << " and " << temp[i + 1] << " did not give correct temperature";
    }

    // verify that disabling the control module disables the LSH grid
    ASSERT_NE(0, pCLSH->getChosenMatrixId());

    pCLSH->enableControl(false);
    ret = pCLSH->update(meta);
    ASSERT_EQ(IMG_SUCCESS, ret);

    ASSERT_EQ(0, pCLSH->getChosenMatrixId());
    ASSERT_EQ(pCLSH->getChosenMatrixId(), pLSH->getCurrentMatrixId());
}

/**
 * Check that remove current matrix possible in different situations
 */
TEST_F(ControlLSH_test, removeMatrix)
{
    LSH_GRID grid = LSH_GRID();
    IMG_RESULT ret;
    IMG_UINT32 matId = 0, otherMatId = 0;
    ISPC::Metadata meta;

    ASSERT_TRUE(checkReady());

    EXPECT_EQ(0, pLSH->getCurrentMatrixId()) << "initial id should be 0";

    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 1 failed";

    ret = pCLSH->addMatrix(2500, grid, matId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 1 failed";

    grid = LSH_GRID();
    ret = newLSHGrid(grid, LSH_TILE_MIN);
    ASSERT_EQ(IMG_SUCCESS, ret) << "create matrix 2 failed";

    ret = pCLSH->addMatrix(3200, grid, otherMatId);
    ASSERT_EQ(IMG_SUCCESS, ret) << "add matrix 2 failed";

    pAWB->setMeasuredTemperature(2500);
    ret = pCLSH->update(meta);
    ASSERT_EQ(IMG_SUCCESS, ret);

    ASSERT_EQ(matId, pCLSH->getChosenMatrixId())
        << "should have updated from 0 to matId";
    EXPECT_EQ(pLSH->getCurrentMatrixId(), pCLSH->getChosenMatrixId())
        << "should always match";

    ret = pCLSH->removeMatrix(matId);
    EXPECT_EQ(IMG_SUCCESS, ret)
        << "should be able to delete current matrix because capture is "
        << "not started";
    EXPECT_EQ(0, pCLSH->getChosenMatrixId())
        << "should have been reset to 0";
    EXPECT_EQ(pLSH->getCurrentMatrixId(), pCLSH->getChosenMatrixId())
        << "should always match";

    pAWB->setMeasuredTemperature(3200);
    ret = pCLSH->update(meta);
    ASSERT_EQ(IMG_SUCCESS, ret);

    ASSERT_EQ(otherMatId, pCLSH->getChosenMatrixId())
        << "should have updated from matId to otherMatId";
    EXPECT_EQ(pLSH->getCurrentMatrixId(), pCLSH->getChosenMatrixId())
        << "should always match";

    ret = startCapture(2, false);
    ASSERT_EQ(IMG_SUCCESS, ret) << "failed to start capture";

    ret = pCLSH->removeMatrix(otherMatId);
    EXPECT_EQ(IMG_ERROR_FATAL, ret)
        << "should not be able to delete current matrix as capture is started";
    ASSERT_EQ(otherMatId, pCLSH->getChosenMatrixId())
        << "should not have affected current matrix";
    EXPECT_EQ(pLSH->getCurrentMatrixId(), pCLSH->getChosenMatrixId())
        << "should always match";

    pCLSH->enableControl(false);

    // changing the matrix only in LSH does not update current matrix in CLSH
    ret = pLSH->configureMatrix(0);
    ASSERT_EQ(IMG_SUCCESS, ret)
        << "should be able to disable matrix while capture is started";
    ASSERT_EQ(0, pLSH->getCurrentMatrixId())
        << "should have updated from otherMatId to 0";
    ASSERT_EQ(otherMatId, pCLSH->getChosenMatrixId())
        << "should not have updated from otherMatId to 0";
    ret = pCLSH->removeMatrix(otherMatId);
    EXPECT_EQ(IMG_SUCCESS, ret)
        << "should be able to delete previous matrix";
    ASSERT_EQ(0, pCLSH->getChosenMatrixId())
        << "should have updated from otherMatId to 0";

    ret = stopCapture();
    EXPECT_EQ(IMG_SUCCESS, ret) << "failed to stop the capture";
}
