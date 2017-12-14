/**
*******************************************************************************
 @file Camera.cpp

 @brief Implementation of ISPC::Camera

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
#define LOG_TAG "ISPC_CAMERA"

#include "ispc/Camera.h"
#include <ci/ci_api.h>
#include <felixcommon/userlog.h>

#include <list>
#include <ostream>

#include "ispc/ModuleOUT.h"
#include "ispc/Save.h"

#ifdef INFOTM_ISP
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleTNM.h"
#include "ispc/ModuleR2Y.h"
#include "ispc/ModuleSHA.h"
#include "ispc/ModuleDNS.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleESC.h"
#include "ispc/ModuleDPF.h"
#include "ispc/ControlAE.h"
#include "ispc/ControlTNM.h"
#include "ispc/ControlDNS.h"
#include "ispc/ControlAWB_PID.h"
#include "ispc/ControlLSH.h"
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) || defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
#include "felix_hw_info.h"
#include "ispc/ControlCMC.h"
#endif
#endif //INFOTM_ISP

#include <cmath>
#include <string>
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif


#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
static bool s_flat_mode_status = 0;
bool FlatModeStatusGet(void)
{
    return s_flat_mode_status; //Is no color mode (black and white)
}

void FlatModeStatusSet(bool flag)
{
    s_flat_mode_status = flag;
}
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

#ifdef INFOTM_ENABLE_GAIN_LEVEL_IDX
static int s_gain_level_idx = 0;
int GainLevelGet(void)
{
    return s_gain_level_idx;
}

void GainLevelSet(int idx)
{
    s_gain_level_idx = idx;
}
#endif //INFOTM_ENABLE_GAIN_LEVEL_IDX

// to enable to have verbose use LOG_DEBUG (mostly about state)
#define V_LOG_DEBUG

const char* ISPC::Camera::StateName(ISPC::Camera::State e)
{
    switch (e)
    {
    case CAM_ERROR:
        return "CAM_ERROR";
    case CAM_DISCONNECTED:
        return "CAM_DISCONNECTED";
    case CAM_CONNECTED:
        return "CAM_CONNECTED";
    case CAM_REGISTERED:
        return "CAM_REGISTERED";
    case CAM_SET_UP:
        return "CAM_SET_UP";
    case CAM_PROGRAMMED:
        return "CAM_PROGRAMMED";
    case CAM_READY:
        return "CAM_READY";
    case CAM_CAPTURING:
        return "CAM_CAPTURING";
    }
    return "unknown";
}

void ISPC::Camera::init(int sensorMode, int sensorFlipping)
{
    IMG_RESULT ret;
    if (!sensor)
    {
        LOG_ERROR("Camera has no sensor!\n");
        this->state = CAM_ERROR;
        return;  // IMG_ERROR_NOT_INITIALISED
    }
    if (sensorMode >= 0)
    {
        ret = sensor->configure(sensorMode, sensorFlipping);
        if (ret)
        {
            LOG_ERROR("Unable to initialize the sensor for mode %d with "\
                "flipping 0x%x\n",
                sensorMode, sensorFlipping);
            this->state = CAM_ERROR;
            V_LOG_DEBUG("state=CAM_ERROR\n");
            return;  // ret
        }
    }

    // Create pipeline and register all default modules
    this->pipeline = new Pipeline(ciConnection.getConnection(), ctxNumber,
        sensor);
#ifdef INFOTM_ISP
    this->bUpdateASAP = true;
    this->capture_mode_flag = false;
    this->isp_marker_debug_string = NULL;
#endif //INFOTM_ISP
}

// this is a protected constructor
ISPC::Camera::Camera(unsigned int contextNumber)
    : ctxNumber(contextNumber), pipeline(NULL), sensor(NULL),
    bOwnSensor(false), state(CAM_DISCONNECTED), bUpdateASAP(false)
{
    LOG_DEBUG("Enter\n");
    CI_CONNECTION *pConn = ciConnection.getConnection();
    // Connect CI driver
    if (!pConn)
    {
        LOG_ERROR("Error connecting to CI\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return;
    }

    this->state = CAM_CONNECTED;  // Camera sucessfully connected to driver
    V_LOG_DEBUG("state=CAM_CONNECTED\n");

    this->hwInfo = pConn->sHWInfo;
    this->bOwnSensor = false;  // no sensor created - imported
}

//
// public
//
#ifdef INFOTM_ISP
ISPC::Camera::Camera(unsigned int contextNumber, int sensorId,
    int sensorMode, int sensorFlipping, int index)
    : ctxNumber(contextNumber), pipeline(NULL), sensor(NULL),
    bOwnSensor(false), state(CAM_DISCONNECTED), bUpdateASAP(false)
{
    LOG_DEBUG("Enter\n");
    CI_CONNECTION *pConn = ciConnection.getConnection();

    // Connect CI driver
    if (!pConn)
    {
        LOG_ERROR("Error connecting to CI\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return;
    }

    this->state = CAM_CONNECTED;  // Camera successfully connected to driver
    V_LOG_DEBUG("state=CAM_CONNECTED\n");
    this->hwInfo = pConn->sHWInfo;

    // Create sensor object to interface with the HW sensor
    this->sensor = new Sensor(sensorId, index);

#ifdef INFOTM_ISP
    //add probe camera sensor
    if (this->sensor->GetSensorState()== Sensor::SENSOR_ERROR)
    {
        LOG_ERROR("Error can't get camera sensor, please insmod sensor driver\n");

        return;
    }
#endif //INFOTM_ISP

    this->bOwnSensor = true;

    init(sensorMode, sensorFlipping);
}
#endif

ISPC::Camera::Camera(unsigned int contextNumber, int sensorId,
    int sensorMode, int sensorFlipping)
    : ctxNumber(contextNumber), pipeline(NULL), sensor(NULL),
    bOwnSensor(false), state(CAM_DISCONNECTED), bUpdateASAP(false)
{
    LOG_DEBUG("Enter\n");
    CI_CONNECTION *pConn = ciConnection.getConnection();

    // Connect CI driver
    if (!pConn)
    {
        LOG_ERROR("Error connecting to CI\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return;
    }

    this->state = CAM_CONNECTED;  // Camera successfully connected to driver
    V_LOG_DEBUG("state=CAM_CONNECTED\n");
    this->hwInfo = pConn->sHWInfo;

    // Create sensor object to interface with the HW sensor
    this->sensor = new Sensor(sensorId);

#ifdef INFOTM_ISP
    //add probe camera sensor 
    if (this->sensor->GetSensorState()== Sensor::SENSOR_ERROR)
    {
        LOG_ERROR("Error can't get camera sensor, please insmod sensor driver\n");

        return;
    }
#endif //INFOTM_ISP

    this->bOwnSensor = true;

    init(sensorMode, sensorFlipping);
}

ISPC::Camera::Camera(unsigned int contextNumber, Sensor *pSensor)
    : ctxNumber(contextNumber), pipeline(NULL), sensor(NULL),
    bOwnSensor(false), state(CAM_DISCONNECTED), bUpdateASAP(false)
{
    LOG_DEBUG("Enter\n");
    CI_CONNECTION *pConn = ciConnection.getConnection();
    // Connect CI driver
    if (!pConn)
    {
        LOG_ERROR("Error connecting to CI\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return;
    }

    this->state = CAM_CONNECTED;  // Camera sucessfully connected to driver
    V_LOG_DEBUG("state=CAM_CONNECTED\n");
    this->hwInfo = pConn->sHWInfo;
    this->pipeline = 0;

    if (!pSensor)
    {
        LOG_ERROR("Given sensor is NULL\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return;
    }
    this->sensor = pSensor;
    this->bOwnSensor = false;

    init(-1);
}

ISPC::Camera::~Camera()
{
    LOG_DEBUG("Enter\n");
    if (CAM_CAPTURING == state)
    {
        LOG_WARNING("Camera in capture state, should be stopped "\
            "before destroying.\n");
        stopCapture();
    }

    if (pipeline)
    {
        delete pipeline;
        pipeline = 0;
    }

    if (sensor && bOwnSensor)
    {
        delete sensor;
        sensor = 0;
    }
    
#ifdef INFOTM_ISP
    if (this->isp_marker_debug_string == NULL)
    {
        IMG_FREE(this->isp_marker_debug_string);
        this->isp_marker_debug_string = NULL;
    }
#endif

    // destruction of CI_Connection disconnects from CI
}

unsigned int ISPC::Camera::getContextNumber() const
{
    return ctxNumber;
}

ISPC::Pipeline* ISPC::Camera::getPipeline()
{
    return this->pipeline;
}

const ISPC::Pipeline* ISPC::Camera::getPipeline() const
{
    return this->pipeline;
}

ISPC::Sensor* ISPC::Camera::getSensor()
{
    return sensor;
}

const ISPC::Sensor* ISPC::Camera::getSensor() const
{
    return sensor;
}

bool ISPC::Camera::ownsSensor() const
{
    return bOwnSensor;
}

void ISPC::Camera::setOwnSensor(bool b)
{
    bOwnSensor = b;
}

CI_CONNECTION* ISPC::Camera::getConnection()
{
    return ciConnection.getConnection();
}

const CI_CONNECTION* ISPC::Camera::getConnection() const
{
    return ciConnection.getConnection();
}

//
// SETUP MODULE FUNCTIONS
//

IMG_RESULT ISPC::Camera::loadParameters(const ParameterList &parameters)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_CONNECTED == state)
    {
        LOG_ERROR("Camera modules must be registered before "\
            "loading parameters.\n");
        state = CAM_ERROR;
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_REGISTERED != state && CAM_SET_UP != state
        && CAM_PROGRAMMED != state)
    {
        LOG_WARNING("Possible invalid camera state for registering "\
            "pipeline modules\n");
    }

    if (bOwnSensor)
    {
        ret = this->sensor->load(parameters);
        if (ret)
        {
            LOG_ERROR("error loading setup parameters in the sensor.\n");
            return ret;
        }
    }

    ret = this->pipeline->reloadAll(parameters);
    if (ret)
    {
        LOG_ERROR("error loading setup parameters in the pipeline.\n");
        return ret;
    }

    ret = this->control.loadAll(parameters);
    if (ret)
    {
        LOG_ERROR("error loading control parameters in the pipeline\n");
        return ret;
    }

    ret = this->pipeline->setupAll();
    if (ret)  // call setup function for all modules
    {
        LOG_ERROR("Setting up pipeline modules.\n");
        return ret;
    }

    if (CAM_REGISTERED == state)
    {
        state = CAM_SET_UP;  // modules have been set up
        V_LOG_DEBUG("state=CAM_SET_UP\n");
    }
    // else we are already in CAM_SET_UP state or some more advanced state

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::loadControlParameters(const ParameterList &parameters)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    /* if (CAM_REGISTERED != state && CAM_SET_UP != state
        && CAM_PROGRAMMED != state)
     {
        LOG_WARNING("Possible invalid camera state for registering "\
            "pipeline modules\n");
    }*/

    ret = this->control.loadAll(parameters);
    if (ret)
    {
        LOG_ERROR("error loading control parameters in the pipeline\n");
        // return ret;
    }
    return ret;
}

IMG_RESULT ISPC::Camera::saveParameters(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    IMG_RESULT ret = IMG_SUCCESS;
    int saved = 3;
    LOG_DEBUG("Enter\n");

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    ret = sensor->save(parameters, t);
    if (ret)
    {
        LOG_WARNING("Saving the parameters of the sensor failed.\n");
        saved--;
    }

    ret = pipeline->saveAll(parameters, t);
    if (ret)
    {
        LOG_WARNING("Saving the parameter of the modules failed.\n");
        saved--;
    }

    ret = control.saveAll(parameters, t);
    if (ret)
    {
        LOG_ERROR("Saving the parameter of the controls failed.\n");
        saved--;
    }

    if (3 != saved)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::setupModules()
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_CONNECTED == state)
    {
        LOG_ERROR("Camera modules must be registered before set up.\n");
        state = CAM_ERROR;
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_REGISTERED != state && CAM_SET_UP != state
        && CAM_PROGRAMMED != state)
    {
        LOG_WARNING("Possible invalid camera state for setting up modules.\n");
    }

    // setup all the modules in the registry

    ret = this->pipeline->setupAll();
    if (ret)
    {
        LOG_ERROR("setting up modules.\n");
        return ret;
    }

    if (CAM_REGISTERED == state)
    {
        state = CAM_SET_UP;  // modules have been set up
        V_LOG_DEBUG("state=CAM_SET_UP\n");
    }
    // else we are already in CAM_SET_UP state or some more advanced state
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::setupModule(SetupID id)
{
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_REGISTERED != state && CAM_SET_UP != state)
    {
        LOG_WARNING("Possible invalid camera state for setting up module.\n");
    }

    return this->pipeline->setupModule(id);  // setup up the requested module
}

IMG_RESULT ISPC::Camera::program()
{
    IMG_RESULT ret;
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_SET_UP != state && CAM_PROGRAMMED != state
        && CAM_READY != state && CAM_CAPTURING != state)
    {
        LOG_ERROR("Error programming pipeline. Camera modules must be "\
            "set up before programming the pipeline.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = this->pipeline->programPipeline(bUpdateASAP);
    if (ret)
    {
        LOG_ERROR("Unable to program pipeline\n");
        return ret;
    }

    // if state is camera set up move to the next state
    if (CAM_SET_UP == state)
    {
        state = CAM_PROGRAMMED;
        V_LOG_DEBUG("state=CAM_PROGRAMMED\n");
    }
    // else: we are already in CAM_PROGRAMMED state

    return IMG_SUCCESS;
}

//
// Buffer and Shot control
//

IMG_RESULT ISPC::Camera::allocateBufferPool(unsigned int num)
{
    std::list<IMG_UINT32> bufferIds;
    return allocateBufferPool(num, bufferIds);
}

IMG_RESULT ISPC::Camera::allocateBufferPool(unsigned int num,
    std::list<IMG_UINT32> &bufferIds)
{
    IMG_RESULT ret;

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_FATAL;
    }

    // should it be an assert? because it is created in constructor...
    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_PROGRAMMED != state && CAM_READY != state
        && CAM_CAPTURING != state)
    {
        LOG_ERROR("Invalid camera state (%d)\n", state);
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_FATAL;
    }

    unsigned int b;
    IMG_UINT32 buff_id;
    const ModuleOUT *glb = pipeline->getModule<const ModuleOUT>();

    if (!glb)
    {
        LOG_ERROR("global module not found\n");
        return IMG_ERROR_FATAL;
    }

    // use addShots to ensure pipeline is ready
    ret = pipeline->addShots(num);
    if (ret)
    {
        LOG_ERROR("Failed to add shots\n");
        return ret;
    }

    if (PXL_NONE != glb->encoderType)
    {
        for (b = 0 ; b < num ; b++)
        {
            ret = pipeline->allocateBuffer(CI_TYPE_ENCODER, 0, false,
                &buff_id);
            if (ret)
            {
                LOG_ERROR("Failed to allocate Encoder buffer %d/%d\n",
                    b+1, num);
                return ret;
            }
            bufferIds.push_back(buff_id);
        }
    }
    if (PXL_NONE != glb->displayType)
    {
        for (b = 0 ; b < num ; b++)
        {
            ret = pipeline->allocateBuffer(CI_TYPE_DISPLAY, 0, false,
                &buff_id);
            if (ret)
            {
                LOG_ERROR("Failed to allocate Display buffer %d/%d\n",
                    b+1, num);
                return ret;
            }
            bufferIds.push_back(buff_id);
        }
    }
    else if (PXL_NONE != glb->dataExtractionType)
    {
        if (CI_INOUT_NONE != glb->dataExtractionPoint)
        {
            for (b = 0 ; b < num ; b++)
            {
                ret = pipeline->allocateBuffer(CI_TYPE_DATAEXT, 0, false,
                    &buff_id);
                if (ret)
                {
                    LOG_ERROR("Failed to allocate Bayer buffer %d/%d\n",
                        b+1, num);
                    return ret;
                }
                bufferIds.push_back(buff_id);
            }
        }
        else
        {
            LOG_ERROR("Data extraction enabled but data extraction "\
                "point set to NONE\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    /* this output is not supported by all versions! But it should
     * be checked in CI */
    if (PXL_NONE != glb->hdrExtractionType)
    {
        for (b = 0 ; b < num ; b++)
        {
            ret = pipeline->allocateBuffer(CI_TYPE_HDREXT, 0, false,
                &buff_id);
            if (ret)
            {
                LOG_ERROR("Failed to allocate HDRExt buffer %d/%d\n",
                    b+1, num);
                return ret;
            }
            bufferIds.push_back(buff_id);
        }
    }

    /* this output is not supported by all versions! But it should
     * be checked in CI */
    if (PXL_NONE != glb->hdrInsertionType)
    {
        for (b = 0 ; b < num ; b++)
        {
            ret = pipeline->allocateBuffer(CI_TYPE_HDRINS, 0, false,
                &buff_id);
            if (ret)
            {
                LOG_ERROR("Failed to allocate HDRIns buffer %d/%d\n",
                    b+1, num);
                return ret;
            }
            bufferIds.push_back(buff_id);
        }
    }

    /* this output is not supported by all versions! But it should
     * be checked in CI */
    if (glb->raw2DExtractionType != PXL_NONE)
    {
        for (b = 0 ; b < num ; b++)
        {
            ret = pipeline->allocateBuffer(CI_TYPE_RAW2D, 0, false, &buff_id);
            if (ret)
            {
                LOG_ERROR("Failed to allocate RAW2D Extraction buffer "\
                    "%d/%d\n", b+1, num);
                return ret;
            }
            bufferIds.push_back(buff_id);
        }
    }

    // get to the next state (ready for capturing)
    if (CAM_PROGRAMMED == state && ISPC_Ctx_READY == pipeline->ctxStatus)
    {
        state = CAM_READY;
        V_LOG_DEBUG("state=CAM_READY\n");
    }

    return ret;
}

IMG_RESULT ISPC::Camera::addShots(unsigned int num)
{
    IMG_RESULT ret;
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // should it be an assert? because it is created in constructor...
    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_PROGRAMMED != state && CAM_READY != state
        && CAM_CAPTURING != state)
    {
        LOG_ERROR("Invalid camera state %d\n", state);
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (num < 1)
    {
        LOG_ERROR("Given number of Shots to add is too small: %d\n", num);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* pipeline must be programmed before requesting buffer allocation
    if (pipeline->programPipeline(bUpdateASAP) != IMG_SUCCESS)
    {
        LOG_ERROR("unable to program pipelien prior to buffer allocation\n");
        return IMG_ERROR_FATAL;
    }*/

    ret = this->pipeline->addShots(num);
    if (ret)
    {
        LOG_ERROR("Error adding buffer pool\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
    }

    return ret;
}

IMG_RESULT ISPC::Camera::deleteShots()
{
    IMG_RESULT ret;
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // should it be an assert? because it is created in constructor...
    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    ret = this->pipeline->deleteShots();
    if (ret)
    {
        LOG_ERROR("Failed to delete shots\n");
    }
    return ret;
}

IMG_RESULT ISPC::Camera::allocateBuffer(CI_BUFFTYPE eBuffer,
    IMG_UINT32 ui32Size, bool isTiled, IMG_UINT32 *pBufferId)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (CAM_PROGRAMMED != state && CAM_READY != state
        && CAM_CAPTURING != state)
    {
        LOG_ERROR("invalid camera state (%d)\n", state);
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_PROGRAMMED == state && pipeline->ctxStatus == ISPC_Ctx_READY)
    {
        state = CAM_READY;
        LOG_DEBUG("state=CAM_READY\n");
    }

    ret = pipeline->allocateBuffer(eBuffer, ui32Size, isTiled, pBufferId);
    if (ret)
    {
        LOG_ERROR("Error while allocating buffer\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
    }
    return ret;
}

IMG_RESULT ISPC::Camera::allocateBuffer(CI_BUFFTYPE eBuffer, bool isTiled,
    IMG_UINT32 *pBufferId)
{
    return allocateBuffer(eBuffer, 0, isTiled, pBufferId);
}

IMG_RESULT ISPC::Camera::importBuffer(CI_BUFFTYPE eBuffer, IMG_UINT32 ionFd,
    IMG_UINT32 ui32Size, bool isTiled, IMG_UINT32 *pBufferId)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    if (0 == ui32Size)
    {
        /* in practice could import a buffer of size 0 because CI would
         * compute the size but it is better than the external allocator
         * provides the size so that CI checks it fits for the configured
         * pipeline outputs */
        LOG_ERROR("size is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (CAM_PROGRAMMED != state && CAM_READY != state
        && CAM_CAPTURING != state)
    {
        LOG_ERROR("invalid camera state (%d)\n", state);
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_PROGRAMMED == state && ISPC_Ctx_READY == pipeline->ctxStatus)
    {
        state = CAM_READY;
        V_LOG_DEBUG("state=CAM_READY\n");
    }

    ret = this->pipeline->importBuffer(eBuffer, ionFd, ui32Size, isTiled,
        pBufferId);
    if (ret)
    {
        LOG_ERROR("Error while importing buffer\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    return ret;
}

IMG_RESULT ISPC::Camera::deregisterBuffer(IMG_UINT32 uiBufferID)
{
    IMG_RESULT ret;

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    ret = pipeline->deregisterBuffer(uiBufferID);
    if (ret)
    {
        LOG_ERROR("Failed to deregister buffer %d\n", uiBufferID);
    }
    return ret;
}

//
// Capture start & stop (reserving/releasing the HW for our use)
//

IMG_RESULT ISPC::Camera::startCapture(const bool enSensor)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_READY != state)
    {
        LOG_ERROR("Camera not ready to start capture.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline object is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = control.configureStatistics();
    if (ret)
    {
        LOG_ERROR("Could not configure the control modules statistics!\n");
        return ret;
    }

    ret = this->pipeline->setupRequested();
    if (ret)
    {
        LOG_ERROR("Unable to setup modules before starting\n");
        return ret;
    }

    ret = this->pipeline->programPipeline(bUpdateASAP);
    if (ret)
    {
        LOG_ERROR("Unable to program pipeline before starting\n");
        return ret;
    }

    ret = this->pipeline->startCapture();
    if (ret)
    {
        LOG_ERROR("Unable to start capture\n");
        return ret;
    }

    if (bOwnSensor)
    {
        if (!sensor)
        {
            LOG_ERROR("Camera pipeline sensor object is NULL\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
        if (enSensor)
        {
            ret = this->sensor->enable();
            if (ret)
            {
                LOG_ERROR("Unable to start sensor!\n");
                this->pipeline->stopCapture();
                /// @ change state to camera error?
                return ret;
            }
        }
        else
        {
            sensor->state = Sensor::SENSOR_ENABLED;
        }
    }

    state = CAM_CAPTURING;
    V_LOG_DEBUG("state=CAM_CAPTURING\n");
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::stopCapture(const bool enSensor)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");
    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!pipeline)
    {
        LOG_ERROR("Camera pipeline object is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (CAM_CAPTURING != state)
    {
        LOG_WARNING("Camera was not capturing.\n");
        return IMG_ERROR_UNEXPECTED_STATE;  // NO fatal error
    }

    ret = pipeline->stopCapture();
    if (ret)
    {
        LOG_ERROR("stopping capture.\n");
        state = CAM_ERROR;
    }

    if (bOwnSensor)
    {
        if (!sensor)
        {
            LOG_ERROR("Camera sensor object is NULL\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
        if (enSensor)
        {
            ret = sensor->disable();
            if (ret)
            {
                LOG_ERROR("Failed to stop the sensor!\n");
                state = CAM_ERROR;
            }
        }
        else
        {
            sensor->state = Sensor::SENSOR_CONFIGURED;
        }
    }

    if (state != CAM_ERROR)
    {
        state = CAM_READY;
        V_LOG_DEBUG("state=CAM_READY\n");
        return IMG_SUCCESS;
    }
    V_LOG_DEBUG("state=CAM_ERROR\n");
    return IMG_ERROR_FATAL;
}

IMG_RESULT ISPC::Camera::enqueueShot()
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_CAPTURING != state)
    {
        LOG_ERROR("Camera must me programmed and buffers allocated before "\
            "enqueuing a shot.\n");
        state = CAM_ERROR;
        LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    // update the setup for all modules that have been requested to be updated
    ret = this->pipeline->setupRequested();
    if (ret)
    {
        LOG_ERROR("Error updating requested modules\n");
        state = CAM_ERROR;
        LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    ret = this->pipeline->programPipeline(bUpdateASAP);
    if (ret)
    {
        LOG_ERROR("Error programming pipeline\n");
        state = CAM_ERROR;
        LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    ret = this->pipeline->programShot();
    if (ret)
    {
        LOG_ERROR("Error programming shot\n");
        state = CAM_ERROR;
        LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::enqueueSpecifiedShot(const CI_BUFFID &buffIds)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CAM_CAPTURING != state)
    {
        LOG_ERROR("Camera must me programmed and buffers allocated before "\
            "enqueuing a shot.\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    // update the setup for all modules that have been requested to be updated
    ret = this->pipeline->setupRequested();
    if (ret)
    {
        LOG_ERROR("Error updating requested modules\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    ret = this->pipeline->programPipeline(bUpdateASAP);
    if (ret)
    {
        LOG_ERROR("Error programming pipeline\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    ret = this->pipeline->programSpecifiedShot(buffIds);
    if (ret)
    {
        LOG_ERROR("Error programming shot\n");
        state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::acquireShot(Shot &shot, bool block,
    bool updateControl)
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    ret = this->pipeline->acquireShot(shot, block);
    if (ret)
    {
        if (block) {
            LOG_ERROR("Unable to get shot\n");
        }
        return ret;
    }

    // run the control algorithms if it is requested
    if (updateControl)
    {
        if (!shot.bFrameError)
        {
            ret = control.runControlModules(UPDATE_SHOT, shot.metadata);
            if (ret)
            {
                LOG_ERROR("Unable to run control modules\n");
                return ret;
            }
        }
        else
        {
            LOG_WARNING("frame is erroneous - skipping control "\
                "module writing\n");
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::acquireShot(Shot &shot, bool updateControl)
{
    return acquireShot(shot, true, updateControl);
}

IMG_RESULT ISPC::Camera::tryAcquireShot(Shot &shot, bool updateControl)
{
    return acquireShot(shot, false, updateControl);
}

IMG_RESULT ISPC::Camera::releaseShot(Shot &shot)
{
    LOG_DEBUG("Enter\n");

    if (CAM_ERROR == state)
    {
        LOG_ERROR("Unable to perform operation, camera is in error state.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!this->pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    return this->pipeline->releaseShot(shot);
}

IMG_RESULT ISPC::Camera::registerControlModule(ControlModule *module)
{
#ifdef INFOTM_ISP
    module->pCamera = this;
#endif //INFOTM_ISP
    return control.registerControlModule(module, pipeline);
}

ISPC::ControlModule *ISPC::Camera::getControlModule(ControlID id)
{
    return control.getControlModule(id);
}

IMG_RESULT ISPC::Camera::setDisplayDimensions(unsigned int width,
    unsigned int height)
{
    LOG_DEBUG("Enter\n");

    if (CAM_CAPTURING == state)
    {
        LOG_ERROR("Can't change display buffer size when pipeline is running");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    return pipeline->setDisplayDimensions(width, height);
}

IMG_RESULT ISPC::Camera::setEncoderDimensions(unsigned int width,
    unsigned int height)
{
    LOG_DEBUG("Enter\n");

    if (CAM_CAPTURING == state)
    {
        LOG_ERROR("Can't change encoder buffer size when pipeline is running");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!pipeline)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    return pipeline->setEncoderDimensions(width, height);
}

std::ostream& ISPC::Camera::printState(std::ostream &os) const
{
    os << "Camera:" << std::endl;
    os << SAVE_A1 << "state = " << StateName(state) << std::endl;
    os << SAVE_A1 << "ctx = " << ctxNumber << std::endl;
    os << SAVE_A1 << "ownSensor = " << (int)bOwnSensor << std::endl;
    if (sensor)
    {
        // could print sensor state here
    }
    if (pipeline)
    {
        // could print pipeline state here
    }
    control.printAllState(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const ISPC::Camera &c)
{
    return c.printState(os);
}

#ifdef INFOTM_ISP
//AWB

static IMG_BOOL is_mono_mode = IMG_FALSE;
//Saturation 0:mono, 1:colorful
IMG_RESULT ISPC::Camera::updateTNMSaturation(IMG_UINT8 pTNMSaturation)
{
    printf("====>updateTNMSaturation(%d)\n", pTNMSaturation);

    ModuleTNM *pTNM = pipeline->getModule<ModuleTNM>();
    ModuleR2Y *pR2Y = pipeline->getModule<ModuleR2Y>();

    static double fOriginalStaturation      = pTNM->fColourSaturation;
    static double fOriginalR2YStaturation   = pR2Y->fSaturation;

    static double fOriginalR2YaRangeMult[3] = {0};
    if (fOriginalR2YaRangeMult[0] == 0) {
        fOriginalR2YaRangeMult[0] = pR2Y->aRangeMult[0];
        fOriginalR2YaRangeMult[1] = pR2Y->aRangeMult[1];
        fOriginalR2YaRangeMult[2] = pR2Y->aRangeMult[2];
    }

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    switch (pTNMSaturation) {
        case 0:
            pTNM->fColourSaturation = 0.0f;
            pR2Y->fSaturation = 0.0f;
            pR2Y->aRangeMult[0] = 1;
            pR2Y->aRangeMult[1] = 0;
            pR2Y->aRangeMult[2] = 0;
            is_mono_mode = IMG_TRUE;
            break;

        default:
            pTNM->fColourSaturation = fOriginalStaturation;
            pR2Y->fSaturation = fOriginalR2YStaturation;
            pR2Y->aRangeMult[0] = fOriginalR2YaRangeMult[0];
            pR2Y->aRangeMult[1] = fOriginalR2YaRangeMult[1];
            pR2Y->aRangeMult[2] = fOriginalR2YaRangeMult[2];
            is_mono_mode = IMG_FALSE;
            break;
    }
    pTNM->requestUpdate();
    pR2Y->requestUpdate();

    //printf("pTNM->fColourSaturation=%f\n", pTNM->fColourSaturation);

    return IMG_SUCCESS;
}

//ISO Limit
IMG_RESULT ISPC::Camera::updateISOLimit(IMG_UINT8 pISOLimit)
{
    //printf("updataISOLimit\n");

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    return sensor->setISOLimit(pISOLimit);
}

//Sharpness
IMG_RESULT ISPC::Camera::updateSHAStrength(IMG_UINT8 pSHAStrength)
{
    //printf("updataSHAStrength\n");

    ModuleSHA *pSHA = pipeline->getModule<ModuleSHA>();

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    switch (pSHAStrength)
    {
        case 0:
            pSHA->fStrength = 0.1;
            break;

        case 1:
            pSHA->fStrength = 0.3;
            break;

        case 2:
            pSHA->fStrength = 0.6;
            break;

        default:
            pSHA->fStrength = 0.3;
            break;
    }
    pSHA->requestUpdate();
    //printf("pSHA->fStrength=%f\n", pSHA->fStrength);

    return IMG_SUCCESS;
}

//EV compensation
IMG_RESULT ISPC::Camera::updateEVTarget(IMG_UINT8 pEVTarget)
{
    //printf("updataEVTarget\n");

    ControlAE *pAE = control.getControlModule<ISPC::ControlAE>();

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    switch (pEVTarget)
    {
        case 0:
            pAE->setTargetBrightness(-0.8);
            break;

        case 1:
            pAE->setTargetBrightness(-0.6);
            break;

        case 2:
            pAE->setTargetBrightness(-0.4);
            break;

        case 3:
            pAE->setTargetBrightness(-0.2);
            break;

        case 4:
            pAE->setTargetBrightness(0);
            break;

        case 5:
            pAE->setTargetBrightness(0.2);
            break;

        case 6:
            pAE->setTargetBrightness(0.4);
            break;

        case 7:
            pAE->setTargetBrightness(0.6);
            break;

        case 8:
            pAE->setTargetBrightness(0.8);
            break;

        default:
            //set to identity.
            pAE->setTargetBrightness(0);
            break;
    }

    return IMG_SUCCESS;
}

//set sensor flip & mirror, 0x00:none, 0x01:mirror, 0x02:flip 0x03:both mirror&flip
IMG_RESULT ISPC::Camera::setSensorFlipMirror(IMG_UINT8 flag)
{
    //printf("setSensorFlipMirror\n");

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    return sensor->setFlipMirror(flag);
}

static IMG_BOOL is_sepia_mode = IMG_FALSE;
//set sepia mode. 0:normal mode, 1:sepia mode
IMG_RESULT ISPC::Camera::setSepiaMode(IMG_UINT8 pMode)
{
    printf("====>setSepiaMode(%d)\n", pMode);

    ModuleR2Y *pR2Y = pipeline->getModule<ModuleR2Y>();

    static double fOriginalR2YaRangeMult[3] = {0};
    if (fOriginalR2YaRangeMult[0] == 0) {
        fOriginalR2YaRangeMult[0] = pR2Y->aRangeMult[0];
        fOriginalR2YaRangeMult[1] = pR2Y->aRangeMult[1];
        fOriginalR2YaRangeMult[2] = pR2Y->aRangeMult[2];
    }

    if (pipeline==NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

    switch (pMode) {
        case 1:
            pR2Y->aRangeMult[0] = 1;
            pR2Y->aRangeMult[1] = 0;
            pR2Y->aRangeMult[2] = 0;

            pR2Y->fOffsetU = -50;//-30.7436;    //cb
            pR2Y->fOffsetV = 40;//26.7304;        //cr
            is_sepia_mode = IMG_TRUE;
            break;

        default:
            //set to normal mode.
            pR2Y->aRangeMult[0] = fOriginalR2YaRangeMult[0];
            pR2Y->aRangeMult[1] = fOriginalR2YaRangeMult[1];
            pR2Y->aRangeMult[2] = fOriginalR2YaRangeMult[2];

            pR2Y->fOffsetU = 0;
            pR2Y->fOffsetV = 0;
            is_sepia_mode = IMG_FALSE;
            break;
    }
    pR2Y->requestUpdate();

    return IMG_SUCCESS;
}

#define GAIN_CNT 10
IMG_RESULT ISPC::Camera::CalcAvgGain()
{
    ControlAE *pAE = getControlModule<ISPC::ControlAE>();
    int j = 0;
    static int i = 0, inited = 0;
    static double GainList[GAIN_CNT] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double TotalGain = 0.0;

    if (inited == 0)
    {
        inited = 1;

        AvgGain = this->getSensor()->getGain();

        for (j = 0; j < GAIN_CNT; j++)
        {
            GainList[j] = AvgGain;
        }
        return IMG_SUCCESS;
    }

    GainList[i++] = this->getSensor()->getGain();
    i %= GAIN_CNT;

    for (j = 0; j < GAIN_CNT; j++)
    {
        TotalGain += GainList[j];
    }

    AvgGain = (double)TotalGain/GAIN_CNT;

    //printf("===>AvgGain %f\n", AvgGain);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::enableLSH(IMG_BOOL Flag)
{
    ISPC::ControlLSH *pLSH = this->getControlModule<ISPC::ControlLSH>();
    pLSH->enableControl(Flag);
    printf("==>enableLSH %d\n", Flag);
    //pLSH->setup();

    //while ( Flag != pLSH->isEnabled())
    {
        //usleep(5000);
    }

    return IMG_SUCCESS;
}

#ifdef AUTO_CHANGE_LSH
IMG_RESULT ISPC::Camera::DynamicChangeLSH()
{
    static IMG_BOOL lastFlag = IMG_FALSE;
#if defined(INFOTM_LANCHOU_PROJECT)
    if (lastFlag == capture_mode_flag )
    {
        return IMG_SUCCESS;
    }

    enableLSH(!capture_mode_flag);
    lastFlag = capture_mode_flag;
    return IMG_SUCCESS;
#endif
}
#endif

//////////////////////////////////////////////////
IMG_RESULT ISPC::Camera::SetCaptureIQFlag(IMG_BOOL Flag)
{
    ControlCMC *pCMC = getControlModule<ISPC::ControlCMC>();
    capture_mode_flag = Flag;

#ifdef INFOTM_SUPPORT_SWTICH_R2Y_MARTIX
    ISPC::ModuleR2Y *pR2Y = pipeline->getModule<ISPC::ModuleR2Y>();
    ISPC::ModuleTNM *pTNM = pipeline->getModule<ISPC::ModuleTNM>();

    static ISPC::ModuleR2YBase::StdMatrix fOriginalR2YeMatrix = pR2Y->eMatrix;
    static double fOriginalR2YaRangeMult[3] = {0};
    if (fOriginalR2YaRangeMult[0] == 0)
    {
        fOriginalR2YaRangeMult[0] = pR2Y->aRangeMult[0];
        fOriginalR2YaRangeMult[1] = pR2Y->aRangeMult[1];
        fOriginalR2YaRangeMult[2] = pR2Y->aRangeMult[2];
    }

    if (pCMC->bEnableCaptureIQ)
    {
        if (capture_mode_flag)
        {
            if (pR2Y->eMatrix != ISPC::ModuleR2YBase::JFIF)
            {
                pR2Y->eMatrix = ISPC::ModuleR2YBase::JFIF;
                if (!is_sepia_mode && !is_mono_mode)
                {
                    pR2Y->aRangeMult[0] = pCMC->fCmcCaptureR2YRangeMul[0];
                    pR2Y->aRangeMult[1] = pCMC->fCmcCaptureR2YRangeMul[1];
                    pR2Y->aRangeMult[2] = pCMC->fCmcCaptureR2YRangeMul[2];
                    //pR2Y->requestUpdate();
                    pR2Y->setup();
                }
            }

            {
                int i;
                for (i = 0; i < 2; i++)
                {
                    pTNM->aInY[i] = pCMC->iCmcCaptureInY[i];
                    pTNM->aOutY[i] = pCMC->iCmcCaptureOutY[i];
                    pTNM->aOutC[i] = pCMC->iCmcCaptureOutC[i];
                }
                
                pTNM->setup();
            }
        }
        else
        {
            int i;
            
            if (!is_sepia_mode && !is_mono_mode && pR2Y->eMatrix != fOriginalR2YeMatrix)
            {
                pR2Y->eMatrix = fOriginalR2YeMatrix;
                pR2Y->aRangeMult[0] = fOriginalR2YaRangeMult[0];
                pR2Y->aRangeMult[1] = fOriginalR2YaRangeMult[1];
                pR2Y->aRangeMult[2] = fOriginalR2YaRangeMult[2];
                //pR2Y->requestUpdate();
                pR2Y->setup();
            }

            if (FlatModeStatusGet())
            {
                for (i = 0; i < 2; i++)
                {
                    pTNM->aInY[i] = pCMC->iInY_fcm[i];
                    pTNM->aOutY[i] = pCMC->iOutY_fcm[i];
                    pTNM->aOutC[i] = pCMC->iOutC_fcm[i];
                }
            }
            else
            {
                for (i = 0; i < 2; i++)
                {
                    pTNM->aInY[i] = pCMC->iInY_acm[i];
                    pTNM->aOutY[i] = pCMC->iOutY_acm[i];
                    pTNM->aOutC[i] = pCMC->iOutC_acm[i];
                }
            }
            pTNM->setup();
        }
    }
#endif


#if 0//defined(INFOTM_LANCHOU_PROJECT)
    enableLSH(!capture_mode_flag);
#endif
    return IMG_SUCCESS;
}

IMG_BOOL ISPC::Camera::GetCaptureIQFlag()
{
    return capture_mode_flag;
}

//////////////////////////////////////////////////
//AR0330
//#define SelfLBCGainMin    4.0
//#define SelfLBCGainMax    12.0

//OV2710
#if 0
// Maximum sensor gain is 8.0x.
#define SelfLBCGainMin    2.0
#define SelfLBCGainMax    6.0
#else
// Maximum sensor gain is 16.0x.
#define SelfLBCGainMin    4.0
#define SelfLBCGainMax    12.0
#endif

IMG_RESULT ISPC::Camera::DynamicChangeRatio(double & dbSensorGain, double & dbRatio)
{
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) && defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    ControlCMC *pCMC = getControlModule<ISPC::ControlCMC>();
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE && INFOTM_ENABLE_GAIN_LEVEL_IDX
    IMG_RESULT ret = IMG_SUCCESS;

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) && defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    if (NULL == pCMC) {
        printf("CMC control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    if (pCMC->bEnableCalcAvgGain) {
        CalcAvgGain();
        dbSensorGain = AvgGain;
    }

    if (!pCMC->bEnableInterpolationGamma) {
        if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGain_lv1)) {
            dbRatio = 0;
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGainLv1Interpolation)) {
            dbRatio = (dbSensorGain - pCMC->fSensorGain_lv1) / (pCMC->fSensorGainLv1Interpolation - pCMC->fSensorGain_lv1);
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGain_lv2)) {
            dbRatio = 0;
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGainLv2Interpolation)) {
            dbRatio = (dbSensorGain - pCMC->fSensorGain_lv2) / (pCMC->fSensorGainLv2Interpolation - pCMC->fSensorGain_lv2);
        } else {
            dbRatio = 0;
        }
    } else {
        int index = 0;

        if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGain_lv1)) {
            dbRatio = 0;
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGainLv1Interpolation)) {
            index = GMA_P_CNT*(dbSensorGain - pCMC->fSensorGain_lv1)/(pCMC->fSensorGainLv1Interpolation - pCMC->fSensorGain_lv1);
            if (index == GMA_P_CNT)
            {
                index = GMA_P_CNT - 1;
            }
            dbRatio = pCMC->afGammaLUT[0][index];
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGain_lv2)) {
            dbRatio = pCMC->afGammaLUT[1][0];
        } else if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= pCMC->fSensorGainLv2Interpolation)) {
            index = GMA_P_CNT*(dbSensorGain - pCMC->fSensorGain_lv2)/(pCMC->fSensorGainLv2Interpolation - pCMC->fSensorGain_lv2);
            if (index == GMA_P_CNT)
            {
                index = GMA_P_CNT - 1;
            }
            dbRatio = pCMC->afGammaLUT[1][index];
        } else {
            double cmc_max_gain = pCMC->fSensorMaxGain_acm;

            if (FlatModeStatusGet())
            {
                cmc_max_gain = pCMC->fSensorMaxGain_fcm;
            }
            
            if ((pCMC->blSensorGainLevelCtrl) && (dbSensorGain <= cmc_max_gain))
            {
                index = GMA_P_CNT*(dbSensorGain - pCMC->fSensorGainLv2Interpolation)/(cmc_max_gain - pCMC->fSensorGainLv2Interpolation);
                if (index == GMA_P_CNT)
                {
                    index = GMA_P_CNT - 1;
                }
                dbRatio = pCMC->afGammaLUT[2][index];
            }else {
                dbRatio = 0;
            }
        }
        
        //printf("==>index %d, dbSensorGain %f, lv1 %f, interpolation_v1 %f, lv2 %f, interpolation_v2 %f, lv3 %f\n", index, dbSensorGain, pCMC->fSensorGain_lv1, pCMC->fSensorGainLv1Interpolation, pCMC->fSensorGain_lv2, pCMC->fSensorGainLv2Interpolation, pCMC->fSensorMaxGain_acm);
    }

    //printf("==>DynamicChangeRatio %f, CurGain %f, lv1_Gain %f, lv2_Gain %f, max Gain %f\n", 
    //    dbRatio, dbSensorGain, pCMC->fSensorGain_lv1, pCMC->fSensorGain_lv2, pCMC->fSensorMaxGain_acm);
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE && INFOTM_ENABLE_GAIN_LEVEL_IDX

    return ret;
}

IMG_RESULT ISPC::Camera::DynamicChange3DDenoiseIdx(IMG_UINT32 & ui32DnTargetIdx, bool bForceCheck)
{
    ControlAE *pAE = getControlModule<ISPC::ControlAE>();
    ControlCMC *pCMC = getControlModule<ISPC::ControlCMC>();
    IMG_RESULT ret = IMG_SUCCESS;

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
    if (NULL == pCMC) {
        printf("CMC control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (false == pCMC->bDnTargetIdxChageEnable) {
        return IMG_SUCCESS;
    }

    if (NULL == pAE) {
        printf("AE control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    if (pAE && (!(pAE->hasConverged()) || bForceCheck)) {
        double Newgain = 0;
        static IMG_UINT8 CurDnTargetIdx = 0;
        IMG_UINT8 NextDnTargetIdx = 0;

        Newgain = pAE->getNewGain();

  #if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
        //set 3d denoise parameters
        if ((!pCMC->blSensorGainLevelCtrl) ||
            ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGain_lv1))) {
            s_gain_level_idx = 0;
  #endif //#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 0
  #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 0 @@@@\n", __FUNCTION__);
  #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm;
            } else {
                //Advanced Color Mode, Level 0
  #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 0 @@@@\n", __FUNCTION__);
  #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm;
            }
  #if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv1Interpolation)) {
            s_gain_level_idx = 1;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm_lv1;
            } else {
                //Advanced Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm_lv1;
            }
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGain_lv2)) {
            s_gain_level_idx = 1;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm_lv1;
            } else {
                //Advanced Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm_lv1;
            }
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv2Interpolation)) {
            s_gain_level_idx = 2;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm_lv2;
            } else {
                //Advanced Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm_lv2;
            }
        } else {
            if (!pCMC->bEnableInterpolationGamma) {
                s_gain_level_idx = 2;

                if (FlatModeStatusGet()) {
                    //Flat Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Flat Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm_lv2;
                } else {
                    //Advanced Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Advanced Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm_lv2;
                }
            } else {
                if (FlatModeStatusGet()) {
                    s_gain_level_idx = 2;
                    //Flat Color Mode, Level max
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Flat Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    NextDnTargetIdx = pCMC->ui32DnTargetIdx_fcm_lv2;
                } else {
                    s_gain_level_idx = 3;
                    //Advanced Color Mode, Level 3
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Advanced Color Mode, Level 3 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    NextDnTargetIdx = pCMC->ui32DnTargetIdx_acm_lv3;
                }
            }
        }
  #endif //#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)

        if (CurDnTargetIdx != NextDnTargetIdx) {
            //printf("NextDnTargetIdx = %d\n", NextDnTargetIdx);
            CurDnTargetIdx = NextDnTargetIdx;
        }
        ui32DnTargetIdx = CurDnTargetIdx;
    }
#endif //#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)

    return ret;
}

IMG_RESULT ISPC::Camera::DynamicChangeIspParameters(bool bForceCheck)
{
    ControlAE *pAE = getControlModule<ISPC::ControlAE>();
    ControlCMC *pCMC = getControlModule<ISPC::ControlCMC>();
    ControlAWB_PID *pAWB = getControlModule<ISPC::ControlAWB_PID>();
    ControlTNM *pTNMC = getControlModule<ISPC::ControlTNM>();
    ModuleBLC *pBLC = pipeline->getModule<ModuleBLC>();
    ModuleTNM *pTNM = pipeline->getModule<ModuleTNM>();
    ModuleDNS *pDNS = pipeline->getModule<ModuleDNS>();
    ModuleSHA *pSHA = pipeline->getModule<ModuleSHA>();
    ModuleR2Y *pR2Y = pipeline->getModule<ModuleR2Y>();
    ModuleHIS *pHIS = pipeline->getModule<ModuleHIS>();
    ModuleDPF *pDPF = pipeline->getModule<ModuleDPF>();
    int iIdx;
    double adTemp[TNMC_WDR_SEGMENT_CNT];
    IMG_RESULT ret = IMG_SUCCESS;

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
    if (NULL == pCMC) {
        printf("CMC control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (false == pCMC->bCmcEnable) {
        return IMG_SUCCESS;
    }

    if (NULL == pAE) {
        printf("AE control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pBLC) {
        printf("Black Level Correction module does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pTNM) {
        printf("Tone mapper module does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pDNS) {
        printf("Primary denoise module does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pSHA) {
        printf("Sharpness module does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pAWB) {
        printf("AWB PID control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (NULL == pTNMC) {
        printf("Tone mapper control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    if ((pCMC->bShaStrengthChange) || (pCMC->bCmcHasChange)) {
        pCMC->CalcShaStrength();
        pCMC->bCmcHasChange = false;
        bForceCheck = true;
    }

    if (pAE && (!(pAE->hasConverged()) || bForceCheck)) {
        IMG_UINT32 uiNewExposure;
        double Newgain = 0;
        double dbRatio = 0.0;

        //set 3d denoise parameters
        uiNewExposure = (IMG_UINT32)pAE->getNewExposure();
        Newgain = pAE->getNewGain();
        DynamicChangeRatio(Newgain, dbRatio);

        if (FlatModeStatusGet()) {
            ret = pTNMC->enableGamma(pCMC->bEnableGamma_fcm);
            pAE->setAeTargetMin(pCMC->fAeTargetMin_fcm);
  #if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
            // This instruction is for ISP v2500
            this->sensor->setMaxGain(pCMC->fSensorMaxGain_fcm);
  //#elif FELIX_VERSION_MAJ == 2
  #else
            // This instruction is for ISP v2505
            pAE->setMaxAeGain(pCMC->fSensorMaxGain_fcm);
  #endif
        } else {
            ret = pTNMC->enableGamma(pCMC->bEnableGamma_acm);
            pAE->setAeTargetMin(pCMC->fAeTargetMin_acm);
  #if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
            // This instruction is for ISP v2500
            this->sensor->setMaxGain(pCMC->fSensorMaxGain_acm);
  //#elif FELIX_VERSION_MAJ == 2
  #else
            // This instruction is for ISP v2505
            pAE->setMaxAeGain(pCMC->fSensorMaxGain_acm);
  #endif
        }
  #ifdef INFOTM_ENABLE_GAIN_LEVEL_IDX
        if ((!pCMC->blSensorGainLevelCtrl) ||
            ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGain_lv1))) {
            s_gain_level_idx = 0;
  #endif //#ifdef INFOTM_ENABLE_GAIN_LEVEL_IDX

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 0
  #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 0 @@@@\n", __FUNCTION__);
  #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm;

                pDNS->fStrength = pCMC->fDnsStrength_fcm;
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm;

                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm;

                pSHA->fRadius = pCMC->fRadius_fcm;
                pSHA->fStrength = pCMC->fStrength_fcm;
                pSHA->fThreshold = pCMC->fThreshold_fcm;
                pSHA->fDetail = pCMC->fDetail_fcm;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm;
                pSHA->fEdgeScale = pCMC->fEdgeScale_fcm;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm;

                pR2Y->fContrast = pCMC->fContrast_fcm;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm);
                
                pR2Y->fBrightness = pCMC->fBrightness_fcm; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm[iIdx];
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_fcm;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm;

                pTNMC->setHistMin(pCMC->fHisMin_fcm);
                pTNMC->setHistMax(pCMC->fHisMax_fcm);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_fcm[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_fcm[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_fcm[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_fcm;
                pTNM->fWeightLine = pCMC->fWeightLine_fcm;
                pTNM->fColourConfidence = pCMC->fColourConfidence_fcm;
                pTNM->fColourSaturation = pCMC->fColourSaturation_fcm;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_fcm);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_fcm);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm);
                ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm);
            } else {
                //Advanced Color Mode, Level 0
  #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 0 @@@@\n", __FUNCTION__);
  #endif
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm;

  #if 0
    #if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
                if (50.0 == pAE->getFlickerRejectionFrequency())
    //#elif FELIX_VERSION_MAJ == 2
    #else
                if (50.0 == pAE->getFlickerFreqConfig())
    #endif
                {
                    if (7800 > uiNewExposure) {
                        pDNS->fStrength = 0;
                    } else if (9800 > uiNewExposure) {
                        pDNS->fStrength = pCMC->fDnsStrength_acm * (double)(uiNewExposure - 7800) / (double)(9800 - 7800);
                    } else {
                        pDNS->fStrength = pCMC->fDnsStrength_acm;
                    }
                } else {
                    if (7300 > uiNewExposure) {
                        pDNS->fStrength = 0;
                    } else if (8300 > uiNewExposure) {
                        pDNS->fStrength = pCMC->fDnsStrength_acm * (double)(uiNewExposure - 7300) / (double)(7300 - 8300);
                    } else {
                        pDNS->fStrength = pCMC->fDnsStrength_acm;
                    }
                }
  #else
                pDNS->fStrength = pCMC->fDnsStrength_acm;  
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm;
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm;
  #endif //#if 0

                pSHA->fRadius = pCMC->fRadius_acm;
                pSHA->fStrength = pCMC->fStrength_acm;
                pSHA->fThreshold = pCMC->fThreshold_acm;
                pSHA->fDetail = pCMC->fDetail_acm;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm;
                pSHA->fEdgeScale = pCMC->fEdgeScale_acm;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm;

                pR2Y->fContrast = pCMC->fContrast_acm;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm);
                
                pR2Y->fBrightness = pCMC->fBrightness_acm; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm[iIdx];
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_acm;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_acm;

                pTNMC->setHistMin(pCMC->fHisMin_acm);
                pTNMC->setHistMax(pCMC->fHisMax_acm);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_acm[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_acm[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_acm[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_acm;
                pTNM->fWeightLine = pCMC->fWeightLine_acm;
                pTNM->fColourConfidence = pCMC->fColourConfidence_acm;
                pTNM->fColourSaturation = pCMC->fColourSaturation_acm;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_acm);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_acm);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm);
                ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm);
            }
  #if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv1Interpolation)) {
            s_gain_level_idx = 1;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm[0] + ((pCMC->ui32BlcSensorBlack_fcm_lv1[0] - pCMC->ui32BlcSensorBlack_fcm[0]) * dbRatio);
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm[1] + ((pCMC->ui32BlcSensorBlack_fcm_lv1[1] - pCMC->ui32BlcSensorBlack_fcm[1]) * dbRatio);
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm[2] + ((pCMC->ui32BlcSensorBlack_fcm_lv1[2] - pCMC->ui32BlcSensorBlack_fcm[2]) * dbRatio);
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm[3] + ((pCMC->ui32BlcSensorBlack_fcm_lv1[3] - pCMC->ui32BlcSensorBlack_fcm[3]) * dbRatio);
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm + ((pCMC->ui32SystemBlack_fcm_lv1 - pCMC->ui32SystemBlack_fcm)*dbRatio);

                pDNS->fStrength = pCMC->fDnsStrength_fcm + ((pCMC->fDnsStrength_fcm_lv1 - pCMC->fDnsStrength_fcm) * dbRatio);
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm + ((pCMC->fDnsReadNoise_fcm_lv1 - pCMC->fDnsReadNoise_fcm) * dbRatio);
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm + (pCMC->iDnsWellDepth_fcm_lv1 - pCMC->iDnsWellDepth_fcm) * dbRatio;

                pSHA->fRadius = pCMC->fRadius_fcm + ((pCMC->fRadius_fcm_lv1- pCMC->fRadius_fcm) * dbRatio);
                pSHA->fStrength = pCMC->fStrength_fcm + ((pCMC->fStrength_fcm_lv1 - pCMC->fStrength_fcm) * dbRatio);
                pSHA->fThreshold = pCMC->fThreshold_fcm + ((pCMC->fThreshold_fcm_lv1 - pCMC->fThreshold_fcm) * dbRatio);
                pSHA->fDetail = pCMC->fDetail_fcm + ((pCMC->fDetail_fcm_lv1 - pCMC->fDetail_fcm) * dbRatio);
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm + ((pCMC->fEdgeOffset_fcm_lv1 - pCMC->fEdgeOffset_fcm) * dbRatio);
                pSHA->fEdgeScale = pCMC->fEdgeScale_fcm + ((pCMC->fEdgeScale_fcm_lv1 - pCMC->fEdgeScale_fcm) * dbRatio);
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm_lv1;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm + ((pCMC->fDenoiseSigma_fcm_lv1 - pCMC->fDenoiseSigma_fcm) * dbRatio);
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm + ((pCMC->fDenoiseTau_fcm_lv1 - pCMC->fDenoiseTau_fcm) * dbRatio);

                pR2Y->fContrast = pCMC->fContrast_fcm + ((pCMC->fContrast_fcm_lv1 - pCMC->fContrast_fcm) * dbRatio);
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm + ((pCMC->fSaturation_fcm_lv1 - pCMC->fSaturation_fcm) * dbRatio));
                
                pR2Y->fBrightness = pCMC->fBrightness_fcm + (pCMC->fBrightness_fcm_lv1 - pCMC->fBrightness_fcm)*dbRatio; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm[iIdx] + (pCMC->aRangeMult_fcm_lv1[iIdx] - pCMC->aRangeMult_fcm[iIdx])*dbRatio;
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_fcm + ((pCMC->fDpfWeight_fcm_lv1 - pCMC->fDpfWeight_fcm) * dbRatio);
                pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm + ((pCMC->iDpfThreshold_fcm_lv1 - pCMC->iDpfThreshold_fcm) * dbRatio);

                pTNMC->setHistMin(pCMC->fHisMin_fcm + (pCMC->fHisMin_fcm_lv1 - pCMC->fHisMin_fcm)*dbRatio);
                pTNMC->setHistMax(pCMC->fHisMax_fcm + (pCMC->fHisMax_fcm_lv1 - pCMC->fHisMax_fcm)*dbRatio);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm_lv1);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_fcm[iIdx] + ((pCMC->iInY_fcm_lv1[iIdx] - pCMC->iInY_fcm[iIdx]) * dbRatio);
                    pTNM->aOutY[iIdx] = pCMC->iOutY_fcm[iIdx] + ((pCMC->iOutY_fcm_lv1[iIdx] - pCMC->iOutY_fcm[iIdx]) * dbRatio);
                    pTNM->aOutC[iIdx] = pCMC->iOutC_fcm[iIdx] + ((pCMC->iOutC_fcm_lv1[iIdx] - pCMC->iOutC_fcm[iIdx]) * dbRatio);
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_fcm + ((pCMC->fFlatFactor_fcm_lv1 - pCMC->fFlatFactor_fcm) * dbRatio);
                pTNM->fWeightLine = pCMC->fWeightLine_fcm + ((pCMC->fWeightLine_fcm_lv1 - pCMC->fWeightLine_fcm) * dbRatio);
                pTNM->fColourConfidence = pCMC->fColourConfidence_fcm + ((pCMC->fColourConfidence_fcm_lv1 - pCMC->fColourConfidence_fcm) * dbRatio);
                pTNM->fColourSaturation = pCMC->fColourSaturation_fcm + ((pCMC->fColourSaturation_fcm_lv1 - pCMC->fColourSaturation_fcm) * dbRatio);
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm + ((pCMC->fEqualBrightSupressRatio_fcm_lv1 - pCMC->fEqualBrightSupressRatio_fcm) * dbRatio));
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm + ((pCMC->fEqualDarkSupressRatio_fcm_lv1 - pCMC->fEqualDarkSupressRatio_fcm) * dbRatio));
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm + ((pCMC->fOvershotThreshold_fcm_lv1 - pCMC->fOvershotThreshold_fcm) * dbRatio));
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrCeiling_fcm[iIdx] + ((pCMC->fWdrCeiling_fcm_lv1[iIdx] - pCMC->fWdrCeiling_fcm[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrCeiling(adTemp);
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrFloor_fcm[iIdx] + ((pCMC->fWdrFloor_fcm_lv1[iIdx] - pCMC->fWdrFloor_fcm[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrFloor(adTemp);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm_lv1);
                ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm + ((pCMC->fTnmGamma_fcm_lv1 - pCMC->fTnmGamma_fcm) * dbRatio));
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm + ((pCMC->fBezierCtrlPnt_fcm_lv1 - pCMC->fBezierCtrlPnt_fcm) * dbRatio));
            } else {
                //Advanced Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm[0] + ((pCMC->ui32BlcSensorBlack_acm_lv1[0] - pCMC->ui32BlcSensorBlack_acm[0]) * dbRatio);
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm[1] + ((pCMC->ui32BlcSensorBlack_acm_lv1[1] - pCMC->ui32BlcSensorBlack_acm[1]) * dbRatio);
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm[2] + ((pCMC->ui32BlcSensorBlack_acm_lv1[2] - pCMC->ui32BlcSensorBlack_acm[2]) * dbRatio);
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm[3] + ((pCMC->ui32BlcSensorBlack_acm_lv1[3] - pCMC->ui32BlcSensorBlack_acm[3]) * dbRatio);
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm + ((pCMC->ui32SystemBlack_acm_lv1 - pCMC->ui32SystemBlack_acm)*dbRatio);

                pDNS->fStrength = pCMC->fDnsStrength_acm + ((pCMC->fDnsStrength_acm_lv1 - pCMC->fDnsStrength_acm) * dbRatio);
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm + ((pCMC->fDnsReadNoise_acm_lv1 - pCMC->fDnsReadNoise_acm) * dbRatio);
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm + (pCMC->iDnsWellDepth_acm_lv1 - pCMC->iDnsWellDepth_acm) * dbRatio;

                pSHA->fRadius = pCMC->fRadius_acm + ((pCMC->fRadius_acm_lv1 - pCMC->fRadius_acm) * dbRatio);
                pSHA->fStrength = pCMC->fStrength_acm + ((pCMC->fStrength_acm_lv1 - pCMC->fStrength_acm) * dbRatio);
                pSHA->fThreshold = pCMC->fThreshold_acm + ((pCMC->fThreshold_acm_lv1- pCMC->fThreshold_acm) * dbRatio);
                pSHA->fDetail = pCMC->fDetail_acm + ((pCMC->fDetail_acm_lv1 - pCMC->fDetail_acm) * dbRatio);
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm + ((pCMC->fEdgeOffset_acm_lv1 - pCMC->fEdgeOffset_acm) * dbRatio);
                pSHA->fEdgeScale = pCMC->fEdgeScale_acm + ((pCMC->fEdgeScale_acm_lv1 - pCMC->fEdgeScale_acm) * dbRatio);
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm_lv1;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm + ((pCMC->fDenoiseSigma_acm_lv1 - pCMC->fDenoiseSigma_acm) * dbRatio);
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm + ((pCMC->fDenoiseTau_acm_lv1 - pCMC->fDenoiseTau_acm) * dbRatio);

                pR2Y->fContrast = pCMC->fContrast_acm + ((pCMC->fContrast_acm_lv1 - pCMC->fContrast_acm) * dbRatio);
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm + ((pCMC->fSaturation_acm_lv1 - pCMC->fSaturation_acm) * dbRatio));
                
                pR2Y->fBrightness = pCMC->fBrightness_acm + (pCMC->fBrightness_acm_lv1 - pCMC->fBrightness_acm)*dbRatio; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm[iIdx] + (pCMC->aRangeMult_acm_lv1[iIdx] - pCMC->aRangeMult_acm[iIdx])*dbRatio;
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_acm + ((pCMC->fDpfWeight_acm_lv1 - pCMC->fDpfWeight_acm) * dbRatio);
                pDPF->ui32Threshold = pCMC->iDpfThreshold_acm + ((pCMC->iDpfThreshold_acm_lv1 - pCMC->iDpfThreshold_acm) * dbRatio);

                pTNMC->setHistMin(pCMC->fHisMin_acm + (pCMC->fHisMin_acm_lv1 - pCMC->fHisMin_acm)*dbRatio);
                pTNMC->setHistMax(pCMC->fHisMax_acm + (pCMC->fHisMax_acm_lv1 - pCMC->fHisMax_acm)*dbRatio);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm_lv1);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_acm[iIdx] + ((pCMC->iInY_acm_lv1[iIdx] - pCMC->iInY_acm[iIdx]) * dbRatio);
                    pTNM->aOutY[iIdx] = pCMC->iOutY_acm[iIdx] + ((pCMC->iOutY_acm_lv1[iIdx] - pCMC->iOutY_acm[iIdx]) * dbRatio);
                    pTNM->aOutC[iIdx] = pCMC->iOutC_acm[iIdx] + ((pCMC->iOutC_acm_lv1[iIdx] - pCMC->iOutC_acm[iIdx]) * dbRatio);
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_acm + ((pCMC->fFlatFactor_acm_lv1 - pCMC->fFlatFactor_acm) * dbRatio);
                pTNM->fWeightLine = pCMC->fWeightLine_acm + ((pCMC->fWeightLine_acm_lv1 - pCMC->fWeightLine_acm) * dbRatio);
                pTNM->fColourConfidence = pCMC->fColourConfidence_acm + ((pCMC->fColourConfidence_acm_lv1 - pCMC->fColourConfidence_acm) * dbRatio);
                pTNM->fColourSaturation = pCMC->fColourSaturation_acm + ((pCMC->fColourSaturation_acm_lv1 - pCMC->fColourSaturation_acm) * dbRatio);
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm + ((pCMC->fEqualBrightSupressRatio_acm_lv1 - pCMC->fEqualBrightSupressRatio_acm) * dbRatio));
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm + ((pCMC->fEqualDarkSupressRatio_acm_lv1 - pCMC->fEqualDarkSupressRatio_acm) * dbRatio));
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm + ((pCMC->fOvershotThreshold_acm_lv1 - pCMC->fOvershotThreshold_acm) * dbRatio));
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrCeiling_acm[iIdx] + ((pCMC->fWdrCeiling_acm_lv1[iIdx] - pCMC->fWdrCeiling_acm[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrCeiling(adTemp);
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrFloor_acm[iIdx] + ((pCMC->fWdrFloor_acm_lv1[iIdx] - pCMC->fWdrFloor_acm[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrFloor(adTemp);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm_lv1);
                ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm + ((pCMC->fTnmGamma_acm_lv1 - pCMC->fTnmGamma_acm) * dbRatio));
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm + ((pCMC->fBezierCtrlPnt_acm_lv1 - pCMC->fBezierCtrlPnt_acm) * dbRatio));
            }
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGain_lv2)) {
            s_gain_level_idx = 1;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm_lv1[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm_lv1[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm_lv1[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm_lv1[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm_lv1;
                
                pDNS->fStrength = pCMC->fDnsStrength_fcm_lv1;
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm_lv1;
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm_lv1;

                pSHA->fRadius = pCMC->fRadius_fcm_lv1;
                pSHA->fStrength = pCMC->fStrength_fcm_lv1;
                pSHA->fThreshold = pCMC->fThreshold_fcm_lv1;
                pSHA->fDetail = pCMC->fDetail_fcm_lv1;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm_lv1;
                pSHA->fEdgeScale = pCMC->fEdgeScale_fcm_lv1;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm_lv1;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm_lv1;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm_lv1;

                pR2Y->fContrast = pCMC->fContrast_fcm_lv1;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm_lv1);
                
                pR2Y->fBrightness = pCMC->fBrightness_fcm_lv1; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm_lv1[iIdx];
                }


                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_fcm_lv1;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm_lv1;

                pTNMC->setHistMin(pCMC->fHisMin_fcm_lv1);
                pTNMC->setHistMax(pCMC->fHisMax_fcm_lv1);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm_lv1);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_fcm_lv1[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_fcm_lv1[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_fcm_lv1[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_fcm_lv1;
                pTNM->fWeightLine = pCMC->fWeightLine_fcm_lv1;
                pTNM->fColourConfidence = pCMC->fColourConfidence_fcm_lv1;
                pTNM->fColourSaturation = pCMC->fColourSaturation_fcm_lv1;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm_lv1);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm_lv1);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm_lv1);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_fcm_lv1);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_fcm_lv1);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm_lv1);
                ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm_lv1);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm_lv1);
            } else {
                //Advanced Color Mode, Level 1
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 1 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
               pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm_lv1[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm_lv1[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm_lv1[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm_lv1[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm_lv1;
                
                pDNS->fStrength = pCMC->fDnsStrength_acm_lv1;
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm_lv1;             
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm_lv1;
                
                pSHA->fRadius = pCMC->fRadius_acm_lv1;
                pSHA->fStrength = pCMC->fStrength_acm_lv1;
                pSHA->fThreshold = pCMC->fThreshold_acm_lv1;
                pSHA->fDetail = pCMC->fDetail_acm_lv1;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm_lv1;
                pSHA->fEdgeScale = pCMC->fEdgeScale_acm_lv1;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm_lv1;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm_lv1;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm_lv1;

                pR2Y->fContrast = pCMC->fContrast_acm_lv1;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm_lv1);
                
                pR2Y->fBrightness = pCMC->fBrightness_acm_lv1; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm_lv1[iIdx];
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_acm_lv1;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_acm_lv1;

                pTNMC->setHistMin(pCMC->fHisMin_acm_lv1);
                pTNMC->setHistMax(pCMC->fHisMax_acm_lv1);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm_lv1);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_acm_lv1[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_acm_lv1[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_acm_lv1[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_acm_lv1;
                pTNM->fWeightLine = pCMC->fWeightLine_acm_lv1;
                pTNM->fColourConfidence = pCMC->fColourConfidence_acm_lv1;
                pTNM->fColourSaturation = pCMC->fColourSaturation_acm_lv1;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm_lv1);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm_lv1);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm_lv1);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_acm_lv1);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_acm_lv1);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm_lv1);
                ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm_lv1);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm_lv1);
            }
        } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv2Interpolation)) {
            s_gain_level_idx = 2;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm_lv1[0] + ((pCMC->ui32BlcSensorBlack_fcm_lv2[0] - pCMC->ui32BlcSensorBlack_fcm_lv1[0]) * dbRatio);
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm_lv1[1] + ((pCMC->ui32BlcSensorBlack_fcm_lv2[1] - pCMC->ui32BlcSensorBlack_fcm_lv1[1]) * dbRatio);
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm_lv1[2] + ((pCMC->ui32BlcSensorBlack_fcm_lv2[2] - pCMC->ui32BlcSensorBlack_fcm_lv1[2]) * dbRatio);
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm_lv1[3] + ((pCMC->ui32BlcSensorBlack_fcm_lv2[3] - pCMC->ui32BlcSensorBlack_fcm_lv1[3]) * dbRatio);
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm_lv1 + ((pCMC->ui32SystemBlack_fcm_lv2 - pCMC->ui32SystemBlack_fcm_lv1)*dbRatio);

                pDNS->fStrength = pCMC->fDnsStrength_fcm_lv1 + ((pCMC->fDnsStrength_fcm_lv2 - pCMC->fDnsStrength_fcm_lv1) * dbRatio);
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm_lv1 + ((pCMC->fDnsReadNoise_fcm_lv2 - pCMC->fDnsReadNoise_fcm_lv1) * dbRatio);
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm_lv1 + (pCMC->iDnsWellDepth_fcm_lv2 - pCMC->iDnsWellDepth_fcm_lv1) * dbRatio;

                pSHA->fRadius = pCMC->fRadius_fcm_lv1 + ((pCMC->fRadius_fcm_lv2 - pCMC->fRadius_fcm_lv1) * dbRatio);
                pSHA->fStrength = pCMC->fStrength_fcm_lv1 + ((pCMC->fStrength_fcm_lv2 - pCMC->fStrength_fcm_lv1) * dbRatio);
                pSHA->fThreshold = pCMC->fThreshold_fcm_lv1 + ((pCMC->fThreshold_fcm_lv2 - pCMC->fThreshold_fcm_lv1) * dbRatio);
                pSHA->fDetail = pCMC->fDetail_fcm_lv1 + ((pCMC->fDetail_fcm_lv2 - pCMC->fDetail_fcm_lv1) * dbRatio);
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm_lv1 + ((pCMC->fEdgeOffset_fcm_lv2 - pCMC->fEdgeOffset_fcm_lv1) * dbRatio);
                pSHA->fEdgeScale = pCMC->fEdgeScale_fcm_lv1 + ((pCMC->fEdgeScale_fcm_lv2- pCMC->fEdgeScale_fcm_lv1) * dbRatio);
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm_lv2;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm_lv1 + ((pCMC->fDenoiseSigma_fcm_lv2 - pCMC->fDenoiseSigma_fcm_lv1) * dbRatio);
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm_lv1 + ((pCMC->fDenoiseTau_fcm_lv2 - pCMC->fDenoiseTau_fcm_lv1) * dbRatio);

                pR2Y->fContrast = pCMC->fContrast_fcm_lv1 + ((pCMC->fContrast_fcm_lv2 - pCMC->fContrast_fcm_lv1) * dbRatio);
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm_lv1 + ((pCMC->fSaturation_fcm_lv2 - pCMC->fSaturation_fcm_lv1) * dbRatio));
                
                pR2Y->fBrightness = pCMC->fBrightness_fcm_lv1 + (pCMC->fBrightness_fcm_lv2 - pCMC->fBrightness_fcm_lv1)*dbRatio; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm_lv1[iIdx] + (pCMC->aRangeMult_fcm_lv2[iIdx] - pCMC->aRangeMult_fcm_lv1[iIdx])*dbRatio;
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_fcm_lv1 + ((pCMC->fDpfWeight_fcm_lv2 - pCMC->fDpfWeight_fcm_lv1) * dbRatio);
                pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm_lv1 + ((pCMC->iDpfThreshold_fcm_lv2 - pCMC->iDpfThreshold_fcm_lv1) * dbRatio);

                pTNMC->setHistMin(pCMC->fHisMin_fcm_lv1 + (pCMC->fHisMin_fcm_lv2 - pCMC->fHisMin_fcm_lv1)*dbRatio);
                pTNMC->setHistMax(pCMC->fHisMax_fcm_lv1 + (pCMC->fHisMax_fcm_lv2 - pCMC->fHisMax_fcm_lv1)*dbRatio);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm_lv2);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_fcm_lv1[iIdx] + ((pCMC->iInY_fcm_lv2[iIdx] - pCMC->iInY_fcm_lv1[iIdx]) * dbRatio);
                    pTNM->aOutY[iIdx] = pCMC->iOutY_fcm_lv1[iIdx] + ((pCMC->iOutY_fcm_lv2[iIdx] - pCMC->iOutY_fcm_lv1[iIdx]) * dbRatio);
                    pTNM->aOutC[iIdx] = pCMC->iOutC_fcm_lv1[iIdx] + ((pCMC->iOutC_fcm_lv2[iIdx] - pCMC->iOutC_fcm_lv1[iIdx]) * dbRatio);
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_fcm_lv1 + ((pCMC->fFlatFactor_fcm_lv2 - pCMC->fFlatFactor_fcm_lv1) * dbRatio);
                pTNM->fWeightLine = pCMC->fWeightLine_fcm_lv1 + ((pCMC->fWeightLine_fcm_lv2 - pCMC->fWeightLine_fcm_lv1) * dbRatio);
                pTNM->fColourConfidence = pCMC->fColourConfidence_fcm_lv1 + ((pCMC->fColourConfidence_fcm_lv2 - pCMC->fColourConfidence_fcm_lv1) * dbRatio);
                pTNM->fColourSaturation = pCMC->fColourSaturation_fcm_lv1 + ((pCMC->fColourSaturation_fcm_lv2 - pCMC->fColourSaturation_fcm_lv1) * dbRatio);
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm_lv1 + ((pCMC->fEqualBrightSupressRatio_fcm_lv2 - pCMC->fEqualBrightSupressRatio_fcm_lv1) * dbRatio));
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm_lv1 + ((pCMC->fEqualDarkSupressRatio_fcm_lv2 - pCMC->fEqualDarkSupressRatio_fcm_lv1) * dbRatio));
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm_lv1 + ((pCMC->fOvershotThreshold_fcm_lv2 - pCMC->fOvershotThreshold_fcm_lv1) * dbRatio));
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrCeiling_fcm_lv1[iIdx] + ((pCMC->fWdrCeiling_fcm_lv2[iIdx] - pCMC->fWdrCeiling_fcm_lv1[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrCeiling(adTemp);
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrFloor_fcm_lv1[iIdx] + ((pCMC->fWdrFloor_fcm_lv2[iIdx] - pCMC->fWdrFloor_fcm_lv1[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrFloor(adTemp);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm_lv2);
                ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm_lv1 + ((pCMC->fTnmGamma_fcm_lv2 - pCMC->fTnmGamma_fcm_lv1) * dbRatio));
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm_lv1 + ((pCMC->fBezierCtrlPnt_fcm_lv2 - pCMC->fBezierCtrlPnt_fcm_lv1) * dbRatio));
            } else {
                //Advanced Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm_lv1[0] + ((pCMC->ui32BlcSensorBlack_acm_lv2[0] - pCMC->ui32BlcSensorBlack_acm_lv1[0]) * dbRatio);
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm_lv1[1] + ((pCMC->ui32BlcSensorBlack_acm_lv2[1] - pCMC->ui32BlcSensorBlack_acm_lv1[1]) * dbRatio);
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm_lv1[2] + ((pCMC->ui32BlcSensorBlack_acm_lv2[2] - pCMC->ui32BlcSensorBlack_acm_lv1[2]) * dbRatio);
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm_lv1[3] + ((pCMC->ui32BlcSensorBlack_acm_lv2[3] - pCMC->ui32BlcSensorBlack_acm_lv1[3]) * dbRatio);
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm_lv1 + ((pCMC->ui32SystemBlack_acm_lv2 - pCMC->ui32SystemBlack_acm_lv1)*dbRatio);

                pDNS->fStrength = pCMC->fDnsStrength_acm_lv1 + ((pCMC->fDnsStrength_acm_lv2 - pCMC->fDnsStrength_acm_lv1) * dbRatio);
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm_lv1 + ((pCMC->fDnsReadNoise_acm_lv2 - pCMC->fDnsReadNoise_acm_lv1) * dbRatio);
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm_lv1 + (pCMC->iDnsWellDepth_acm_lv2 - pCMC->iDnsWellDepth_acm_lv1) * dbRatio;

                pSHA->fRadius = pCMC->fRadius_acm_lv1 + ((pCMC->fRadius_acm_lv2 - pCMC->fRadius_acm_lv1) * dbRatio);
                pSHA->fStrength = pCMC->fStrength_acm_lv1 + ((pCMC->fStrength_acm_lv2 - pCMC->fStrength_acm_lv1) * dbRatio);
                pSHA->fThreshold = pCMC->fThreshold_acm_lv1 + ((pCMC->fThreshold_acm_lv2 - pCMC->fThreshold_acm_lv1) * dbRatio);
                pSHA->fDetail = pCMC->fDetail_acm_lv1 + ((pCMC->fDetail_acm_lv2 - pCMC->fDetail_acm_lv1) * dbRatio);
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm_lv1 + ((pCMC->fEdgeOffset_acm_lv2 - pCMC->fEdgeOffset_acm_lv1) * dbRatio);
                pSHA->fEdgeScale = pCMC->fEdgeScale_acm_lv1 + ((pCMC->fEdgeScale_acm_lv2 - pCMC->fEdgeScale_acm_lv1) * dbRatio);
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm_lv2;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm_lv1 + ((pCMC->fDenoiseSigma_acm_lv2 - pCMC->fDenoiseSigma_acm_lv1) * dbRatio);
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm_lv1 + ((pCMC->fDenoiseTau_acm_lv2 - pCMC->fDenoiseTau_acm_lv1) * dbRatio);

                pR2Y->fContrast = pCMC->fContrast_acm_lv1 + ((pCMC->fContrast_acm_lv2 - pCMC->fContrast_acm_lv1) * dbRatio);
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm_lv1 + ((pCMC->fSaturation_acm_lv2 - pCMC->fSaturation_acm_lv1) * dbRatio));

                pR2Y->fBrightness = pCMC->fBrightness_acm_lv1 + (pCMC->fBrightness_acm_lv2 - pCMC->fBrightness_acm_lv1)*dbRatio; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm_lv1[iIdx] + (pCMC->aRangeMult_acm_lv2[iIdx] - pCMC->aRangeMult_acm_lv1[iIdx])*dbRatio;
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm_lv1[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm_lv1[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_acm_lv1 + ((pCMC->fDpfWeight_acm_lv2 - pCMC->fDpfWeight_acm_lv1) * dbRatio);
                pDPF->ui32Threshold = pCMC->iDpfThreshold_acm_lv1 + ((pCMC->iDpfThreshold_acm_lv2 - pCMC->iDpfThreshold_acm_lv1) * dbRatio);

                pTNMC->setHistMin(pCMC->fHisMin_acm_lv1 + (pCMC->fHisMin_acm_lv2 - pCMC->fHisMin_acm_lv1)*dbRatio);
                pTNMC->setHistMax(pCMC->fHisMax_acm_lv1 + (pCMC->fHisMax_acm_lv2 - pCMC->fHisMax_acm_lv1)*dbRatio);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm_lv2);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_acm_lv1[iIdx] + ((pCMC->iInY_acm_lv2[iIdx] - pCMC->iInY_acm_lv1[iIdx]) * dbRatio);
                    pTNM->aOutY[iIdx] = pCMC->iOutY_acm_lv1[iIdx] + ((pCMC->iOutY_acm_lv2[iIdx] - pCMC->iOutY_acm_lv1[iIdx]) * dbRatio);
                    pTNM->aOutC[iIdx] = pCMC->iOutC_acm_lv1[iIdx] + ((pCMC->iOutC_acm_lv2[iIdx] - pCMC->iOutC_acm_lv1[iIdx]) * dbRatio);
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_acm_lv1 + ((pCMC->fFlatFactor_acm_lv2 - pCMC->fFlatFactor_acm_lv1) * dbRatio);
                pTNM->fWeightLine = pCMC->fWeightLine_acm_lv1 + ((pCMC->fWeightLine_acm_lv2 - pCMC->fWeightLine_acm_lv1) * dbRatio);
                pTNM->fColourConfidence = pCMC->fColourConfidence_acm_lv1 + ((pCMC->fColourConfidence_acm_lv2 - pCMC->fColourConfidence_acm_lv1) * dbRatio);
                pTNM->fColourSaturation = pCMC->fColourSaturation_acm_lv1 + ((pCMC->fColourSaturation_acm_lv2 - pCMC->fColourSaturation_acm_lv1) * dbRatio);
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm_lv1 + ((pCMC->fEqualBrightSupressRatio_acm_lv2 - pCMC->fEqualBrightSupressRatio_acm_lv1) * dbRatio));
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm_lv1 + ((pCMC->fEqualDarkSupressRatio_acm_lv2 - pCMC->fEqualDarkSupressRatio_acm_lv1) * dbRatio));
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm_lv1 + ((pCMC->fOvershotThreshold_acm_lv2 - pCMC->fOvershotThreshold_acm_lv1) * dbRatio));
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrCeiling_acm_lv1[iIdx] + ((pCMC->fWdrCeiling_acm_lv2[iIdx] - pCMC->fWdrCeiling_acm_lv1[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrCeiling(adTemp);
                for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    adTemp[iIdx] = pCMC->fWdrFloor_acm_lv1[iIdx] + ((pCMC->fWdrFloor_acm_lv2[iIdx] - pCMC->fWdrFloor_acm_lv1[iIdx]) * dbRatio);
                }
                ret = pTNMC->setWdrFloor(adTemp);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm_lv2);
                ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm_lv1 + ((pCMC->fTnmGamma_acm_lv2 - pCMC->fTnmGamma_acm_lv1) * dbRatio));
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm_lv1 + ((pCMC->fBezierCtrlPnt_acm_lv2 - pCMC->fBezierCtrlPnt_acm_lv1) * dbRatio));
            }
        } else if (!pCMC->bEnableInterpolationGamma/* && (Newgain <= pCMC->fSensorMaxGain_acm)*/) {
            s_gain_level_idx = 2;

            if (FlatModeStatusGet()) {
                //Flat Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Flat Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm_lv2[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm_lv2[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm_lv2[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm_lv2[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm_lv2;

                pDNS->fStrength = pCMC->fDnsStrength_fcm_lv2;
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm_lv2;
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm_lv2;


                pSHA->fRadius = pCMC->fRadius_fcm_lv2;
                pSHA->fStrength = pCMC->fStrength_fcm_lv2;
                pSHA->fThreshold = pCMC->fThreshold_fcm_lv2;
                pSHA->fDetail = pCMC->fDetail_fcm_lv2;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm_lv2;
                pSHA->fEdgeScale = pCMC->fEdgeScale_fcm_lv2;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm_lv2;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm_lv2;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm_lv2;

                pR2Y->fContrast = pCMC->fContrast_fcm_lv2;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm_lv2);

                pR2Y->fBrightness = pCMC->fBrightness_fcm_lv2; 
                for (iIdx = 0; iIdx < 3; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm_lv2[iIdx];
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm_lv2[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm_lv2[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_fcm_lv2;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm_lv2;

                pTNMC->setHistMin(pCMC->fHisMin_fcm_lv2);
                pTNMC->setHistMax(pCMC->fHisMax_fcm_lv2);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm_lv2);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_fcm_lv2[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_fcm_lv2[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_fcm_lv2[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_fcm_lv2;
                pTNM->fWeightLine = pCMC->fWeightLine_fcm_lv2;
                pTNM->fColourConfidence = pCMC->fColourConfidence_fcm_lv2;
                pTNM->fColourSaturation = pCMC->fColourSaturation_fcm_lv2;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm_lv2);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm_lv2);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm_lv2);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_fcm_lv2);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_fcm_lv2);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm_lv2);
                ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm_lv2);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm_lv2);
            } else {
                //Advanced Color Mode, Level 2
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                printf("@@@@ %s::Advanced Color Mode, Level 2 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm_lv2[0];
                pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm_lv2[1];
                pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm_lv2[2];
                pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm_lv2[3];
                pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm_lv2;

                pDNS->fStrength = pCMC->fDnsStrength_acm_lv2;
                pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm_lv2;
                pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm_lv2;

                pSHA->fRadius = pCMC->fRadius_acm_lv2;
                pSHA->fStrength = pCMC->fStrength_acm_lv2;
                pSHA->fThreshold = pCMC->fThreshold_acm_lv2;
                pSHA->fDetail = pCMC->fDetail_acm_lv2;
                pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm_lv2;
                pSHA->fEdgeScale = pCMC->fEdgeScale_acm_lv2;
                pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm_lv2;
                pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm_lv2;
                pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm_lv2;

                pR2Y->fContrast = pCMC->fContrast_acm_lv2;
                pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm_lv2);

                pR2Y->fBrightness = pCMC->fBrightness_acm_lv2; 
                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm_lv2[iIdx];
                }

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm_lv2[iIdx];
                    pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm_lv2[iIdx];
                }

                pDPF->fWeight = pCMC->fDpfWeight_acm_lv2;
                pDPF->ui32Threshold = pCMC->iDpfThreshold_acm_lv2;

                pTNMC->setHistMin(pCMC->fHisMin_acm_lv2);
                pTNMC->setHistMax(pCMC->fHisMax_acm_lv2);
                pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm_lv2);

                for (iIdx = 0; iIdx < 2; iIdx++) {
                    pTNM->aInY[iIdx] = pCMC->iInY_acm_lv2[iIdx];
                    pTNM->aOutY[iIdx] = pCMC->iOutY_acm_lv2[iIdx];
                    pTNM->aOutC[iIdx] = pCMC->iOutC_acm_lv2[iIdx];
                }
                pTNM->fFlatFactor = pCMC->fFlatFactor_acm_lv2;
                pTNM->fWeightLine = pCMC->fWeightLine_acm_lv2;
                pTNM->fColourConfidence = pCMC->fColourConfidence_acm_lv2;
                pTNM->fColourSaturation = pCMC->fColourSaturation_acm_lv2;
                ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm_lv2);
                ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm_lv2);
                ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm_lv2);
                ret = pTNMC->setWdrCeiling(pCMC->fWdrCeiling_acm_lv2);
                ret = pTNMC->setWdrFloor(pCMC->fWdrFloor_acm_lv2);
                ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm_lv2);
                ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm_lv2);
                ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm_lv2);
            }
        } else {
            if (pCMC->bEnableInterpolationGamma) {
                s_gain_level_idx = 3;

                if (FlatModeStatusGet()) {
                    //Flat Color Mode, Level 3
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Flat Color Mode, Level 3 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_fcm_lv2[0] + ((pCMC->ui32BlcSensorBlack_fcm_lv3[0] - pCMC->ui32BlcSensorBlack_fcm_lv2[0]) * dbRatio);
                    pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_fcm_lv2[1] + ((pCMC->ui32BlcSensorBlack_fcm_lv3[1] - pCMC->ui32BlcSensorBlack_fcm_lv2[1]) * dbRatio);
                    pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_fcm_lv2[2] + ((pCMC->ui32BlcSensorBlack_fcm_lv3[2] - pCMC->ui32BlcSensorBlack_fcm_lv2[2]) * dbRatio);
                    pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_fcm_lv2[3] + ((pCMC->ui32BlcSensorBlack_fcm_lv3[3] - pCMC->ui32BlcSensorBlack_fcm_lv2[3]) * dbRatio);
                    pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_fcm_lv2 + ((pCMC->ui32SystemBlack_fcm_lv3 - pCMC->ui32SystemBlack_fcm_lv2)*dbRatio);

                    pDNS->fStrength = pCMC->fDnsStrength_fcm_lv2 + ((pCMC->fDnsStrength_fcm_lv3 - pCMC->fDnsStrength_fcm_lv2) * dbRatio);
                    pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_fcm_lv2 + ((pCMC->fDnsReadNoise_fcm_lv3 - pCMC->fDnsReadNoise_fcm_lv2) * dbRatio);
                    pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_fcm_lv2 + (pCMC->iDnsWellDepth_fcm_lv3 - pCMC->iDnsWellDepth_fcm_lv2) * dbRatio;
                    
                    pSHA->fRadius = pCMC->fRadius_fcm_lv2 + ((pCMC->fRadius_fcm_lv3 - pCMC->fRadius_fcm_lv2) * dbRatio);
                    pSHA->fStrength = pCMC->fStrength_fcm_lv2 + ((pCMC->fStrength_fcm_lv3 - pCMC->fStrength_fcm_lv2) * dbRatio);
                    pSHA->fThreshold = pCMC->fThreshold_fcm_lv2 + ((pCMC->fThreshold_fcm_lv3 - pCMC->fThreshold_fcm_lv2) * dbRatio);
                    pSHA->fDetail = pCMC->fDetail_fcm_lv2 + ((pCMC->fDetail_fcm_lv3 - pCMC->fDetail_fcm_lv2) * dbRatio);
                    pSHA->fEdgeOffset = pCMC->fEdgeOffset_fcm_lv2 + ((pCMC->fEdgeOffset_fcm_lv3 - pCMC->fEdgeOffset_fcm_lv2) * dbRatio);
                    pSHA->fEdgeScale = pCMC->fEdgeScale_fcm_lv2 + ((pCMC->fEdgeScale_fcm_lv3- pCMC->fEdgeScale_fcm_lv2) * dbRatio);
                    pSHA->bBypassDenoise = pCMC->bBypassDenoise_fcm_lv3;
                    pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_fcm_lv2 + ((pCMC->fDenoiseSigma_fcm_lv3 - pCMC->fDenoiseSigma_fcm_lv2) * dbRatio);
                    pSHA->fDenoiseTau = pCMC->fDenoiseTau_fcm_lv2 + ((pCMC->fDenoiseTau_fcm_lv3 - pCMC->fDenoiseTau_fcm_lv2) * dbRatio);

                    pR2Y->fContrast = pCMC->fContrast_fcm_lv2 + ((pCMC->fContrast_fcm_lv3 - pCMC->fContrast_fcm_lv2) * dbRatio);
                    pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_fcm_lv2 + ((pCMC->fSaturation_fcm_lv3 - pCMC->fSaturation_fcm_lv2) * dbRatio));

                    pR2Y->fBrightness = pCMC->fBrightness_fcm_lv2 + (pCMC->fBrightness_fcm_lv3 - pCMC->fBrightness_fcm_lv2)*dbRatio; 
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_fcm_lv2[iIdx] + (pCMC->aRangeMult_fcm_lv3[iIdx] - pCMC->aRangeMult_fcm_lv2[iIdx])*dbRatio;
                    }

                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_fcm_lv2[iIdx] + (pCMC->iGridStartCoords_fcm_lv3[iIdx] - pCMC->iGridStartCoords_fcm_lv2[iIdx]) * dbRatio;
                        pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_fcm_lv2[iIdx] + (pCMC->iGridTileDimensions_fcm_lv3[iIdx] - pCMC->iGridTileDimensions_fcm_lv2[iIdx]) * dbRatio;
                    }

                    pDPF->fWeight = pCMC->fDpfWeight_fcm_lv2 + ((pCMC->fDpfWeight_fcm_lv3 - pCMC->fDpfWeight_fcm_lv2) * dbRatio);
                    pDPF->ui32Threshold = pCMC->iDpfThreshold_fcm_lv2 + ((pCMC->iDpfThreshold_fcm_lv3 - pCMC->iDpfThreshold_fcm_lv2) * dbRatio);

                    pTNMC->setHistMin(pCMC->fHisMin_fcm_lv2 + (pCMC->fHisMin_fcm_lv3 - pCMC->fHisMin_fcm_lv2)*dbRatio);
                    pTNMC->setHistMax(pCMC->fHisMax_fcm_lv2 + (pCMC->fHisMax_fcm_lv3 - pCMC->fHisMax_fcm_lv2)*dbRatio);
                    pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_fcm_lv3);

                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pTNM->aInY[iIdx] = pCMC->iInY_fcm_lv2[iIdx] + ((pCMC->iInY_fcm_lv3[iIdx] - pCMC->iInY_fcm_lv2[iIdx]) * dbRatio);
                        pTNM->aOutY[iIdx] = pCMC->iOutY_fcm_lv2[iIdx] + ((pCMC->iOutY_fcm_lv3[iIdx] - pCMC->iOutY_fcm_lv2[iIdx]) * dbRatio);
                        pTNM->aOutC[iIdx] = pCMC->iOutC_fcm_lv2[iIdx] + ((pCMC->iOutC_fcm_lv3[iIdx] - pCMC->iOutC_fcm_lv2[iIdx]) * dbRatio);
                    }
                    pTNM->fFlatFactor = pCMC->fFlatFactor_fcm_lv2 + ((pCMC->fFlatFactor_fcm_lv3 - pCMC->fFlatFactor_fcm_lv2) * dbRatio);
                    pTNM->fWeightLine = pCMC->fWeightLine_fcm_lv2 + ((pCMC->fWeightLine_fcm_lv3 - pCMC->fWeightLine_fcm_lv2) * dbRatio);
                    pTNM->fColourConfidence = pCMC->fColourConfidence_fcm_lv2 + ((pCMC->fColourConfidence_fcm_lv3 - pCMC->fColourConfidence_fcm_lv2) * dbRatio);
                    pTNM->fColourSaturation = pCMC->fColourSaturation_fcm_lv2 + ((pCMC->fColourSaturation_fcm_lv3 - pCMC->fColourSaturation_fcm_lv2) * dbRatio);
                    ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_fcm_lv2 + ((pCMC->fEqualBrightSupressRatio_fcm_lv3 - pCMC->fEqualBrightSupressRatio_fcm_lv2) * dbRatio));
                    ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_fcm_lv2 + ((pCMC->fEqualDarkSupressRatio_fcm_lv3 - pCMC->fEqualDarkSupressRatio_fcm_lv2) * dbRatio));
                    ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_fcm_lv2 + ((pCMC->fOvershotThreshold_fcm_lv3 - pCMC->fOvershotThreshold_fcm_lv2) * dbRatio));
                    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                        adTemp[iIdx] = pCMC->fWdrCeiling_fcm_lv2[iIdx] + ((pCMC->fWdrCeiling_fcm_lv3[iIdx] - pCMC->fWdrCeiling_fcm_lv2[iIdx]) * dbRatio);
                    }
                    ret = pTNMC->setWdrCeiling(adTemp);
                    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                        adTemp[iIdx] = pCMC->fWdrFloor_fcm_lv2[iIdx] + ((pCMC->fWdrFloor_fcm_lv3[iIdx] - pCMC->fWdrFloor_fcm_lv2[iIdx]) * dbRatio);
                    }
                    ret = pTNMC->setWdrFloor(adTemp);
                    ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_fcm_lv3);
                    ret = pTNMC->setGammaFCM(pCMC->fTnmGamma_fcm_lv2 + ((pCMC->fTnmGamma_fcm_lv3 - pCMC->fTnmGamma_fcm_lv2) * dbRatio));
                    ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_fcm_lv2 + ((pCMC->fBezierCtrlPnt_fcm_lv3 - pCMC->fBezierCtrlPnt_fcm_lv2) * dbRatio));
                } else {
                    //Advanced Color Mode, Level 3
    #if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    printf("@@@@ %s::Advanced Color Mode, Level 3 @@@@\n", __FUNCTION__);
    #endif //#if defined(INFOTM_GAIN_LEVEL_DEBUG_MESSAGE)
                    pBLC->aSensorBlack[0] = pCMC->ui32BlcSensorBlack_acm_lv2[0] + ((pCMC->ui32BlcSensorBlack_acm_lv3[0] - pCMC->ui32BlcSensorBlack_acm_lv2[0]) * dbRatio);
                    pBLC->aSensorBlack[1] = pCMC->ui32BlcSensorBlack_acm_lv2[1] + ((pCMC->ui32BlcSensorBlack_acm_lv3[1] - pCMC->ui32BlcSensorBlack_acm_lv2[1]) * dbRatio);
                    pBLC->aSensorBlack[2] = pCMC->ui32BlcSensorBlack_acm_lv2[2] + ((pCMC->ui32BlcSensorBlack_acm_lv3[2] - pCMC->ui32BlcSensorBlack_acm_lv2[2]) * dbRatio);
                    pBLC->aSensorBlack[3] = pCMC->ui32BlcSensorBlack_acm_lv2[3] + ((pCMC->ui32BlcSensorBlack_acm_lv3[3] - pCMC->ui32BlcSensorBlack_acm_lv2[3]) * dbRatio);
                    pBLC->ui32SystemBlack = pCMC->ui32SystemBlack_acm_lv2 + ((pCMC->ui32SystemBlack_acm_lv3 - pCMC->ui32SystemBlack_acm_lv2)*dbRatio);

                    pDNS->fStrength = pCMC->fDnsStrength_acm_lv2 + ((pCMC->fDnsStrength_acm_lv3 - pCMC->fDnsStrength_acm_lv2) * dbRatio);
                    pDNS->fSensorReadNoise = pCMC->fDnsReadNoise_acm_lv2 + ((pCMC->fDnsReadNoise_acm_lv3 - pCMC->fDnsReadNoise_acm_lv2) * dbRatio);
                    pDNS->ui32SensorWellDepth = pCMC->iDnsWellDepth_acm_lv2 + (pCMC->iDnsWellDepth_acm_lv3 - pCMC->iDnsWellDepth_acm_lv2) * dbRatio;

                    pSHA->fRadius = pCMC->fRadius_acm_lv2 + ((pCMC->fRadius_acm_lv3 - pCMC->fRadius_acm_lv2) * dbRatio);
                    pSHA->fStrength = pCMC->fStrength_acm_lv2 + ((pCMC->fStrength_acm_lv3 - pCMC->fStrength_acm_lv2) * dbRatio);
                    pSHA->fThreshold = pCMC->fThreshold_acm_lv2 + ((pCMC->fThreshold_acm_lv3 - pCMC->fThreshold_acm_lv2) * dbRatio);
                    pSHA->fDetail = pCMC->fDetail_acm_lv2 + ((pCMC->fDetail_acm_lv3 - pCMC->fDetail_acm_lv2) * dbRatio);
                    pSHA->fEdgeOffset = pCMC->fEdgeOffset_acm_lv2 + ((pCMC->fEdgeOffset_acm_lv3 - pCMC->fEdgeOffset_acm_lv2) * dbRatio);
                    pSHA->fEdgeScale = pCMC->fEdgeScale_acm_lv2 + ((pCMC->fEdgeScale_acm_lv3 - pCMC->fEdgeScale_acm_lv2) * dbRatio);
                    pSHA->bBypassDenoise = pCMC->bBypassDenoise_acm_lv3;
                    pSHA->fDenoiseSigma = pCMC->fDenoiseSigma_acm_lv2 + ((pCMC->fDenoiseSigma_acm_lv3 - pCMC->fDenoiseSigma_acm_lv2) * dbRatio);
                    pSHA->fDenoiseTau = pCMC->fDenoiseTau_acm_lv2 + ((pCMC->fDenoiseTau_acm_lv3 - pCMC->fDenoiseTau_acm_lv2) * dbRatio);

                    pR2Y->fContrast = pCMC->fContrast_acm_lv2 + ((pCMC->fContrast_acm_lv3 - pCMC->fContrast_acm_lv2) * dbRatio);
                    pR2Y->fSaturation = pCMC->CalcR2YSaturation(pCMC->fSaturation_acm_lv2 + ((pCMC->fSaturation_acm_lv3 - pCMC->fSaturation_acm_lv2) * dbRatio));

                    pR2Y->fBrightness = pCMC->fBrightness_acm_lv2 + (pCMC->fBrightness_acm_lv3 - pCMC->fBrightness_acm_lv2)*dbRatio; 
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pR2Y->aRangeMult[iIdx] = pCMC->aRangeMult_acm_lv2[iIdx] + (pCMC->aRangeMult_acm_lv3[iIdx] - pCMC->aRangeMult_acm_lv2[iIdx])*dbRatio;
                    }

                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pHIS->aGridStartCoord[iIdx] = pCMC->iGridStartCoords_acm_lv2[iIdx] + (pCMC->iGridStartCoords_acm_lv3[iIdx] - pCMC->iGridStartCoords_acm_lv2[iIdx]) * dbRatio;
                        pHIS->aGridTileSize[iIdx] = pCMC->iGridTileDimensions_acm_lv2[iIdx] + (pCMC->iGridTileDimensions_acm_lv3[iIdx] - pCMC->iGridTileDimensions_acm_lv2[iIdx]) * dbRatio;
                    }

                    pDPF->fWeight = pCMC->fDpfWeight_acm_lv2 + ((pCMC->fDpfWeight_acm_lv3 - pCMC->fDpfWeight_acm_lv2) * dbRatio);
                    pDPF->ui32Threshold = pCMC->iDpfThreshold_acm_lv2 + ((pCMC->iDpfThreshold_acm_lv3 - pCMC->iDpfThreshold_acm_lv2) * dbRatio);

                    pTNMC->setHistMin(pCMC->fHisMin_acm_lv2 + (pCMC->fHisMin_acm_lv3 - pCMC->fHisMin_acm_lv2)*dbRatio);
                    pTNMC->setHistMax(pCMC->fHisMax_acm_lv2 + (pCMC->fHisMax_acm_lv3 - pCMC->fHisMax_acm_lv2)*dbRatio);
                    pTNMC->enableAdaptiveTNM(pCMC->bTnmcAdaptive_acm_lv3);

                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pTNM->aInY[iIdx] = pCMC->iInY_acm_lv2[iIdx] + ((pCMC->iInY_acm_lv3[iIdx] - pCMC->iInY_acm_lv2[iIdx]) * dbRatio);
                        pTNM->aOutY[iIdx] = pCMC->iOutY_acm_lv2[iIdx] + ((pCMC->iOutY_acm_lv3[iIdx] - pCMC->iOutY_acm_lv2[iIdx]) * dbRatio);
                        pTNM->aOutC[iIdx] = pCMC->iOutC_acm_lv2[iIdx] + ((pCMC->iOutC_acm_lv3[iIdx] - pCMC->iOutC_acm_lv2[iIdx]) * dbRatio);
                    }
                    pTNM->fFlatFactor = pCMC->fFlatFactor_acm_lv2 + ((pCMC->fFlatFactor_acm_lv3 - pCMC->fFlatFactor_acm_lv2) * dbRatio);
                    pTNM->fWeightLine = pCMC->fWeightLine_acm_lv2 + ((pCMC->fWeightLine_acm_lv3 - pCMC->fWeightLine_acm_lv2) * dbRatio);
                    pTNM->fColourConfidence = pCMC->fColourConfidence_acm_lv2 + ((pCMC->fColourConfidence_acm_lv3 - pCMC->fColourConfidence_acm_lv2) * dbRatio);
                    pTNM->fColourSaturation = pCMC->fColourSaturation_acm_lv2 + ((pCMC->fColourSaturation_acm_lv3 - pCMC->fColourSaturation_acm_lv2) * dbRatio);
                    ret = pTNMC->setEqualBrightSupressRatio(pCMC->fEqualBrightSupressRatio_acm_lv2 + ((pCMC->fEqualBrightSupressRatio_acm_lv3 - pCMC->fEqualBrightSupressRatio_acm_lv2) * dbRatio));
                    ret = pTNMC->setEqualDarkSupressRatio(pCMC->fEqualDarkSupressRatio_acm_lv2 + ((pCMC->fEqualDarkSupressRatio_acm_lv3 - pCMC->fEqualDarkSupressRatio_acm_lv2) * dbRatio));
                    ret = pTNMC->setOvershotThreshold(pCMC->fOvershotThreshold_acm_lv2 + ((pCMC->fOvershotThreshold_acm_lv3 - pCMC->fOvershotThreshold_acm_lv2) * dbRatio));
                    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                        adTemp[iIdx] = pCMC->fWdrCeiling_acm_lv2[iIdx] + ((pCMC->fWdrCeiling_acm_lv3[iIdx] - pCMC->fWdrCeiling_acm_lv2[iIdx]) * dbRatio);
                    }
                    ret = pTNMC->setWdrCeiling(adTemp);
                    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                        adTemp[iIdx] = pCMC->fWdrFloor_acm_lv2[iIdx] + ((pCMC->fWdrFloor_acm_lv3[iIdx] - pCMC->fWdrFloor_acm_lv2[iIdx]) * dbRatio);
                    }
                    ret = pTNMC->setWdrFloor(adTemp);
                    ret = pTNMC->setGammaCurveMode(pCMC->ui32GammaCrvMode_acm_lv3);
                    ret = pTNMC->setGammaACM(pCMC->fTnmGamma_acm_lv2 + ((pCMC->fTnmGamma_acm_lv3 - pCMC->fTnmGamma_acm_lv2) * dbRatio));
                    ret = pTNMC->setBezierCtrlPnt(pCMC->fBezierCtrlPnt_acm_lv2 + ((pCMC->fBezierCtrlPnt_acm_lv3 - pCMC->fBezierCtrlPnt_acm_lv2) * dbRatio));
                }
            }
        }
  #endif //#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)

        if (is_mono_mode) {
            //printf("===>is_mono_mode, flat_mode %d\n", FlatModeStatusGet());
            pTNM->fColourConfidence = 0.0;
            pTNM->fColourSaturation = 0.0;
            pR2Y->fSaturation = 0.0;
            pTNM->aOutC[0] = 0;
            pTNM->aOutC[0] = 10;
        }

        this->getSensor()->flReadNoise = pDNS->fSensorReadNoise;
        this->getSensor()->uiWellDepth = pDNS->ui32SensorWellDepth;
        
#if 0 // defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
        printf("==>pR2Y->fBrightness %f, pR2Y->aRangeMult == <%f, %f, %f>\n", pR2Y->fBrightness, pR2Y->aRangeMult[0], pR2Y->aRangeMult[1], pR2Y->aRangeMult[2]);
#endif

        pBLC->requestUpdate();
        pDNS->requestUpdate();
        pSHA->requestUpdate();
        pR2Y->requestUpdate();
        pTNM->requestUpdate();
        pHIS->requestUpdate();
        pDPF->requestUpdate();
    }
#endif //#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::DayModeDetect(int & nDayMode)
{
    ControlAE *pAE = getControlModule<ISPC::ControlAE>();
    ControlCMC *pCMC = getControlModule<ISPC::ControlCMC>();
    static int nMode = CMC_DAY_MODE_DAY;
    static double dbBrightness = 0;
    static double dbGain = 0;
    double dbCurrentWeighting;

    if (NULL == pCMC) {
        printf("CMC control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
    if (false == pCMC->bCmcNightModeDetectEnable) {
        nDayMode = CMC_DAY_MODE_DAY;
        return IMG_SUCCESS;
    }

    if (NULL == pAE) {
        printf("AE control does not exist!\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }

    dbCurrentWeighting = 1 - pCMC->fCmcNightModeDetectWeighting;
    dbBrightness = (pAE->getCurrentBrightness() * dbCurrentWeighting) +
        (dbBrightness * pCMC->fCmcNightModeDetectWeighting);
    dbGain = (pAE->getNewGain() * dbCurrentWeighting) +
        (dbGain * pCMC->fCmcNightModeDetectWeighting);

    switch (nMode) {
        case CMC_DAY_MODE_NIGHT:
            if ((pCMC->fCmcNightModeDetectBrightnessExit <= dbBrightness) &&
                (pCMC->fCmcNightModeDetectGainExit >= dbGain)) {
                nMode = CMC_DAY_MODE_DAY;
                nDayMode = CMC_DAY_MODE_DAY;
            } else {
                nDayMode = CMC_DAY_MODE_NIGHT;
            }
            break;

        case CMC_DAY_MODE_DAY:
            if ((pCMC->fCmcNightModeDetectBrightnessEnter >= dbBrightness) &&
                (pCMC->fCmcNightModeDetectGainEnter <= dbGain)) {
                nMode = CMC_DAY_MODE_NIGHT;
                nDayMode = CMC_DAY_MODE_NIGHT;
            } else {
                nDayMode = CMC_DAY_MODE_DAY;
            }
            break;
    }

#if 0
    printf("%s dbBrightness:%llf dbGain:%llf nDayMode:%d \n",__FUNCTION__, dbBrightness, dbGain, nDayMode);
#endif
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::IsUpdateSelfLBC(void)
{
    //printf("IsUpdateSelfLBC\n");

    ControlAE *pAE = getControlModule<ISPC::ControlAE>();
#if defined(INFOTM_ENABLE_GLOBAL_TONE_MAPPER) || defined(INFOTM_ENABLE_LOCAL_TONE_MAPPER)
    ControlTNM *pTNM = getControlModule<ISPC::ControlTNM>();
#endif
#if 0
    ModuleSHA *pSHA = pipeline->getModule<ModuleSHA>();
#endif
    ModuleR2Y *pR2Y = pipeline->getModule<ModuleR2Y>();

    //gain, brightness, total
    double Fg, Fb, Ft = 0;
    double TNMHistMax, TNMHistMin = 0;
    double TNMLocalStrength = 0;
    double R2YSaturation = 0;

    if (pipeline == NULL)
    {
        LOG_ERROR("Camera pipeline not defined (== NULL)\n");
        return IMG_ERROR_FATAL;
    }

       if ( pAE && !(pAE->hasConverged()) )
    {
        double Newgain = 0;
        double CurBrightness = 0;

        //1.Gain Part ((set 3d denoise parameters))
        Newgain = pAE->getNewGain();
        //printf("Newgain = %f\n", Newgain);

        if (Newgain < SelfLBCGainMin)
        {
            Newgain = SelfLBCGainMin;
        }
        else if (Newgain > SelfLBCGainMax)
        {
            Newgain = SelfLBCGainMax;
        }

        Fg = (Newgain - SelfLBCGainMin) / (SelfLBCGainMax - SelfLBCGainMin);

#if 0
        //2.Brightness Part
        CurBrightness = pAE->getCurrentBrightness();
        //printf("CurBrightness = %f\n", CurBrightness);
        CurBrightness -= pAE->getTargetBrightness();
        if (-0.5 >= CurBrightness)
            CurBrightness = -0.5;
        if (CurBrightness > 0)
            CurBrightness = 0;
        Fb = (0 - CurBrightness) * 2;
#endif

        //3.Total (set TNM Histmin, Histmax, and LocalStrength)
#if 0
        Ft = Fb * Fg;
#else
        Ft = Fg * Fg;
#endif

#if 0
        TNMHistMin = (0.25 - 0.035)*Ft + 0.035;
        TNMHistMax = TNMHistMin + 0.25 - 0.035;
#else
        TNMHistMin = (0.25 - 0.18)*Ft + 0.18;
        TNMHistMax = TNMHistMin + 0.25 - 0.18;
#endif

        //TNMLocalStrength = 0.1 - (0.1 - 0.001) * Ft;
        TNMLocalStrength = 0.2 - (0.2 - 0.001) * Ft;

#if defined(INFOTM_ENABLE_GLOBAL_TONE_MAPPER)
        if (pTNM)
        {
            pTNM->setHistMin(TNMHistMin);
            pTNM->setHistMax(TNMHistMax);
        }
#endif
#if defined(INFOTM_ENABLE_LOCAL_TONE_MAPPER)
        if (pTNM)
        {
            pTNM->setLocalStrength(TNMLocalStrength);
        }
#endif

        // Reduce sharpness and saturation.
#if 0
        if (pSHA)
        {
            pSHA->fStrength = 0.4 - (0.05 * Ft);
            pSHA->requestUpdate();
        }
#endif
        if (pR2Y)
        {
            pR2Y->fSaturation = 1.2 - ((1.2 - 1.0) * Ft);
            pR2Y->requestUpdate();
        }

        //printf("pR2Y->fSaturation=%f\n", pR2Y->fSaturation);
        //printf("Af-curBrightness=%f\n", CurBrightness);
        //printf("Fg=%f, Fb=%f, Ft=%f\n", Fg, Fb, Ft);
#if defined(INFOTM_ENABLE_GLOBAL_TONE_MAPPER)
        //printf("pTNM->setHistMin = %f, pTNM->setHistMax =%f\n", pTNM->getHistMin(), pTNM->getHistMax());
#endif
        //printf("TNMLocalStrength=%f\n\n", TNMLocalStrength);
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Camera::CameraDumpReg(IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Size)
{
    IMG_UINT32 res = 0;
    CI_CONNECTION *pConn = ciConnection.getConnection();
    int i = 0;
    if (!pConn)
    {
        LOG_ERROR("Error connecting to CI\n");
        this->state = CAM_ERROR;
        V_LOG_DEBUG("state=CAM_ERROR\n");
        return IMG_ERROR_FATAL;
    }

    switch (eBank)
    {
        case 2:
        {
            if(pConn->sHWInfo.config_ui8NContexts < CI_N_CONTEXT)
            {

                for (i = 0 ; i < ui32Size ; i++)
                 {
                    CI_DriverDebugRegRead(pConn,
                            eBank + pConn->sHWInfo.config_ui8NContexts - 1,
                            ui32Offset + i*4, &res);
                    printf("CI_BANK_CTX [offset: 0x%x] = 0x%x\n", ui32Offset + i*4, res);
                 }
            }

            break;
        }
        default:
            LOG_ERROR("Error ISP HW eBank.\n");
            break;
    }


    return IMG_SUCCESS;
}

IMG_BOOL ISPC::Camera::IsMonoMode(void)
{
    return is_mono_mode;
}

IMG_CHAR* ISPC::Camera::GetISPMarkerDebugInfo(void)
{
    // if (isp_marker_debug_string == NULL)
    //{
    //    isp_marker_debug_string = (IMG_CHAR*)IMG_MALLOC(512*sizeof(IMG_CHAR));
    //}

    
    //IMG_FREE(isp_marker_debug_string);
    //isp_marker_debug_string = NULL;
    
    return NULL;
}

#endif //INFOTM_ISP
