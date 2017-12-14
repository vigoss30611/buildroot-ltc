/**
*******************************************************************************
 @file ispc_utest.cpp

 @brief Base class to help unit tests of ISPC modules

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

#include "ispc_utest.h"

#include <img_types.h>
#include <img_errors.h>
#include <felixcommon/lshgrid.h>
#include <list>

#include "ispc/Camera.h"
#include "ispc/ModuleOUT.h"

#include "ci/ci_api.h"
#include "sensors/iifdatagen.h"
#include "felix.h"

ISPCUTest::ISPCUTest()
{
    pCamera = NULL;
}

ISPCUTest::~ISPCUTest()
{
    if (pCamera)
    {
        delete pCamera;
    }
}

void ISPCUTest::addControlModules()
{

}

IMG_RESULT ISPCUTest::configure(int ctx, int gasket)
{
    IMG_RESULT ret;

    ret = driver.configure();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to fake insmod" << std::endl;
        return IMG_ERROR_FATAL;
    }

    // setup camera to have YUV output and DG sensor
    pCamera = new ISPC::DGCamera(ctx, "testdata/600x200_10bit_rggb.flx",
        gasket, true);
    if (!pCamera)
    {
        std::cerr << "failed to create Camera" << std::endl;
        return IMG_ERROR_MALLOC_FAILED;
    }
    if (pCamera->state == ISPC::Camera::CAM_ERROR)
    {
        std::cerr << "failed to create Camera" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }
    pSensor = dynamic_cast<ISPC::DGSensor *>(pCamera->getSensor());

    if (pSensor->isInternal)
    {
        ret = IIFDG_ExtendedSetBlanking(pSensor->getHandle(), 600, FELIX_MIN_V_BLANKING);
        if (IMG_SUCCESS != ret)
        {
            std::cerr << "failed to set blanking" << std::endl;
            delete pCamera;
            pCamera = NULL;
            pSensor = NULL;
            return IMG_ERROR_FATAL;
        }
    }

    ret = ISPC::CameraFactory::populateCameraFromHWVersion(*pCamera,
        pCamera->getSensor());
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to populate the Camera" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }

    addControlModules();

    ISPC::ParameterList p;

    // load defaults (not needed)
    ret = pCamera->loadParameters(p);
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to load defaults!" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }

    ISPC::ModuleOUT *pOut = pCamera->getModule<ISPC::ModuleOUT>();
    if (NULL == pOut)
    {
        std::cerr << "failed to get output module!" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }
    else
    {
        pOut->encoderType = YUV_420_PL12_8;
        pOut->requestUpdate();
    }

    ret = pCamera->setupModules();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to setup the camera!" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }

    ret = pCamera->program();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to program the camera!" << std::endl;
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

void ISPCUTest::finalise()
{
    if (pCamera)
    {
        delete pCamera;
        pCamera = NULL;
        pSensor = NULL;
    }

    driver.clean();
}

IMG_RESULT ISPCUTest::startCapture(int n, bool enqueue)
{
    IMG_RESULT ret;

    IMG_ASSERT(pCamera != NULL);

    ret = pCamera->allocateBufferPool(n, allocBufferId);
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to allocate " << n << " buffers+shots"
            << std::endl;
        return ret;
    }

    ret = pCamera->startCapture();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to start" << std::endl;
        return ret;
    }

    if (enqueue)
    {
        ret = pCamera->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            std::cerr << "failed to enqueue" << std::endl;
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPCUTest::stopCapture()
{
    IMG_RESULT ret = IMG_SUCCESS;
    ISPC::Shot s;
    IMG_BOOL8 hasPending = IMG_FALSE;
    CI_PIPELINE *pPipe = NULL;

    IMG_ASSERT(pCamera != NULL);
    IMG_ASSERT(pSensor != NULL);
    pPipe = pCamera->getPipeline()->getCIPipeline();

    // wait for all acquired frames
    hasPending = CI_PipelineHasPending(pPipe);
    while (hasPending)
    {
        FullFelix::fakeCtxInterrupt(pCamera->getContextNumber(), false, false);
        if (pSensor->isInternal)
        {
            ret = FullFelix::fakeIIFDgInterrupt(0, true);
        }
#ifdef FELIX_HAS_DG
        else
        {
            ret = Datagen::fakeDgxInterrupt(pCamera->getSensor()->uiImager, true, true);
        }
#else
        else
        {
            std::cerr << "external DG not support but DGSensor is "
                << "not internal?" << std::endl;
            return IMG_ERROR_FATAL;
        }
#endif
        if (IMG_SUCCESS != ret)
        {
            std::cerr << "failed to trigger interrupt!" << std::endl;
            return IMG_ERROR_FATAL;
        }

        ret = pCamera->acquireShot(s, true, true);
        pCamera->releaseShot(s);
        hasPending = CI_PipelineHasPending(pPipe);
    }

    ret = pCamera->stopCapture();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to stop" << std::endl;
        return ret;
    }

    std::list<IMG_UINT32>::iterator it;
    for (it = allocBufferId.begin(); it != allocBufferId.end(); it++)
    {
        ret = pCamera->deregisterBuffer(*it);
        if (IMG_SUCCESS != ret)
        {
            std::cerr << "failed to deregister buffer " << *it
                << std::endl;
        }
    }
    allocBufferId.clear();

    ret = pCamera->deleteShots();
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "failed to delete shots" << std::endl;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPCUTest::newLSHGrid(LSH_GRID &grid, int tile,
    const float *corners)
{
    // values do not matter we just want to generate a matrix
    float aCorners[5] = { 1.0, 1.0, 1.0, 1.0, 1.0 };
    ISPC::Sensor *pSensor = NULL;
    IMG_RESULT ret;

    if (NULL == pCamera)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    pSensor = pCamera->getSensor();
    if (NULL == pSensor)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (pSensor->uiWidth < CI_CFA_WIDTH || pSensor->uiHeight < CI_CFA_HEIGHT)
    {
        std::cerr << "sensor not setup" << std::endl;
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = LSH_CreateMatrix(&grid, pSensor->uiWidth / CI_CFA_WIDTH,
        pSensor->uiHeight / CI_CFA_HEIGHT, tile);
    if (IMG_SUCCESS != ret)
    {
        std::cerr << "create matrix failed" << std::endl;
        return ret;
    }

    for (int c = 0; c < 4; c++)
    {
        if (corners)
        {
            ret = LSH_FillLinear(&grid, c, corners);
        }
        else
        {
            ret = LSH_FillLinear(&grid, c, aCorners);
        }
        if (IMG_SUCCESS != ret)
        {
            std::cerr << "fill channel" << (int)c << " failed" << std::endl;
            LSH_Free(&grid);
            return ret;
        }
    }
    return IMG_SUCCESS;
}

std::list<IMG_UINT32> ISPCUTest::getLSHTileSizes()
{
    unsigned int tile = LSH_TILE_MIN;
    std::list<IMG_UINT32> tileSizes;

    tileSizes.push_back(tile);
    tile *= 2;
    while (tile < LSH_TILE_MAX)
    {
        tileSizes.push_back(tile);
        tile *= 2;
    }

    return tileSizes;
}
