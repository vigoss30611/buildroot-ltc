/**
*******************************************************************************
 @file DGCamera.cpp

 @brief Implementation of ISPC::DGCamera

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
#define LOG_TAG "ISPC_DGCAMERA"
#include "ispc/Camera.h"
#include "ispc/ModuleDNS.h"

#ifdef ISPC_EXT_DATA_GENERATOR
#include <sensors/dgsensor.h>
#endif
#include <ci/ci_api.h>  // for internal DG
#include <felixcommon/userlog.h>

#include <string>

bool ISPC::DGCamera::supportExtDG()
{
#ifdef ISPC_EXT_DATA_GENERATOR
    return true;
#else
    return false;
#endif
}

bool ISPC::DGCamera::supportIntDG()
{
    CI_Connection conn;

    CI_CONNECTION *pV = conn.getConnection();
    if (pV)
    {
        // compare with 0 because casting to bool
        return 0 !=
            (pV->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN);
    }
    LOG_ERROR("failed to connect to driver to enquire about internal "\
        "data generator\n");
    return false;
}

ISPC::DGCamera::DGCamera(unsigned int contextNumber,
    const std::string &filename, unsigned int gasketNumber, bool isInternal)
    : Camera(contextNumber)
{
    this->sensor = new DGSensor(filename, gasketNumber, isInternal);
    this->bOwnSensor = true;

    /// @ load sensor info from parameter file

    if ( sensor && sensor->getState() != Sensor::SENSOR_ERROR )
    {
        Camera::init(0);
        updateSensorInfo(ParameterList());
    }
    else
    {
        LOG_ERROR("Cannot init DG sensor!\n");
        this->state = CAM_ERROR;
    }
}

ISPC::DGCamera::DGCamera(unsigned int contextNumber, Sensor *sensor):
    Camera(contextNumber)
{
    this->sensor = sensor;
    this->bOwnSensor = false;

    Camera::init(-1);
}

ISPC::DGCamera::~DGCamera()
{
    /*if ( sensor )
    {
        delete sensor;
        sensor = 0;
    }*/
}

IMG_RESULT ISPC::DGCamera::updateSensorInfo(const ParameterList &param)
{
    if (!sensor)
    {
        LOG_ERROR("sensor object is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    // need to ensure it is in the correct state
    // this->sensor->setGain(param.getParameter(ModuleDNS::DNS_ISOGAIN));
    this->sensor->uiWellDepth = param.getParameter(ModuleDNS::DNS_WELLDEPTH);
    this->sensor->flReadNoise = param.getParameter(ModuleDNS::DNS_READNOISE);
    // sensor depth is loaded from image given to DG
    // sensor bayer format is loaded from image given to DG
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::DGCamera::enqueueShot()
{
    IMG_RESULT ret;

    if (!sensor)
    {
        LOG_ERROR("sensor object is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("enqueue shot on DG camera\n");
    ret = Camera::enqueueShot();
    if (IMG_SUCCESS == ret && bOwnSensor)
    {
        ret = sensor->insert();
    }
    return ret;
}

IMG_RESULT ISPC::DGCamera::enqueueSpecifiedShot(CI_BUFFID &buffId)
{
    IMG_RESULT ret;

    if (!sensor)
    {
        LOG_ERROR("sensor object is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = Camera::enqueueSpecifiedShot(buffId);
    if (IMG_SUCCESS == ret && bOwnSensor)
    {
        ret = this->sensor->insert();
    }

    return ret;
}
