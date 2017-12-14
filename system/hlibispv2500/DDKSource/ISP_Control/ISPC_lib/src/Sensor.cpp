/**
*******************************************************************************
@file Sensor.cpp

@brief Implementation of ISPC::Sensor

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
#include "ispc/Sensor.h"

#include <sensorapi/sensorapi.h>
#ifdef ISPC_EXT_DATA_GENERATOR
#include <sensors/dgsensor.h>
#endif
#include <sensors/iifdatagen.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_SENSOR"

#include <iostream>
#include <string>
#include <utility>  // pair
#include <list>
#include <vector>

#include "ispc/ModuleDNS.h"  // to have access to deprecated parameters
#include "ispc/ModuleFLD.h"  // to have access to deprecated parameters

//
// public static
//

// not an actual maximum or default
const ISPC::ParamDef<double> ISPC::Sensor::SENSOR_EXPOSURE(
    "SENSOR_EXPOSURE_MS", 0.0, 5000.0, 35.0);

const ISPC::ParamDef<double> ISPC::Sensor::SENSOR_GAIN(
    "SENSOR_GAIN", 0.0, 128.0, 1.0);

const ISPC::ParamDef<unsigned int> ISPC::Sensor::SENSOR_BITDEPTH(
    "SENSOR_BITDEPTH", 8, 16, 10);

const ISPC::ParamDef<unsigned int> ISPC::Sensor::SENSOR_WELLDEPTH(
    "SENSOR_WELL_DEPTH", 0, 65535, 5000);

const ISPC::ParamDef<double> ISPC::Sensor::SENSOR_READNOISE(
    "SENSOR_READ_NOISE", 0.0, 100.0, 0.0);

const ISPC::ParamDef<double> ISPC::Sensor::SENSOR_FRAMERATE(
    "SENSOR_FRAME_RATE", 1.0, 255.0, 30.0);

static const unsigned int sensor_size_def[2] = { 1280, 720 };
const ISPC::ParamDefArray<unsigned int> ISPC::Sensor::SENSOR_SIZE(
    "SENSOR_ACTIVE_SIZE", 0, 16384, sensor_size_def, 2);

const ISPC::ParamDef<unsigned int> ISPC::Sensor::SENSOR_VTOT(
    "SENSOR_VTOT", 0, 16383, 525);

ISPC::ParameterGroup ISPC::Sensor::GetGroup()
{
    ParameterGroup group;

    group.header = "// Sensor information";

    group.parameters.insert(SENSOR_EXPOSURE.name);
    group.parameters.insert(SENSOR_GAIN.name);
    group.parameters.insert(SENSOR_BITDEPTH.name);
    group.parameters.insert(SENSOR_WELLDEPTH.name);
    group.parameters.insert(SENSOR_READNOISE.name);
    group.parameters.insert(SENSOR_FRAMERATE.name);
    group.parameters.insert(SENSOR_SIZE.name);
    group.parameters.insert(SENSOR_VTOT.name);

    return group;
}

const char* ISPC::Sensor::StateName(ISPC::Sensor::State s)
{
    switch (s)
    {
    case SENSOR_ERROR:
        return "SENSOR_ERROR";
    case SENSOR_INITIALIZED:
        return "SENSOR_INITIALIZED";
    case SENSOR_ENABLED:
        return "SENSOR_ENABLED";
    case SENSOR_CONFIGURED:
        return "SENSOR_CONFIGURED";
    default:
        return "UNKNOWN";
    }
}

void ISPC::Sensor::GetSensorNames(std::list<std::pair<std::string, int> >
    &result)
{
    int nSensors = Sensor_NumberSensors();
    const char ** sensorNames = Sensor_ListAll();

    for (int s = 0; s < nSensors; s++)
    {
        result.push_back(
            std::pair<std::string, int>(std::string(sensorNames[s]), s));
    }
}

int ISPC::Sensor::GetSensorId(const std::string &name)
{
    std::list<std::pair<std::string, int> > sensors;
    std::list<std::pair<std::string, int> >::const_iterator it;

    GetSensorNames(sensors);

    for (it = sensors.begin(); it != sensors.end(); it++)
    {
        if (0 == it->first.compare(name))
        {
            return it->second;
        }
    }
    return -1;
}

//
// protected
//

ISPC::Sensor::Sensor()
{
    hSensorHandle = NULL;
    state = SENSOR_ERROR;
    focusSupported = false;
}

//
// public
//

ISPC::Sensor::Sensor(int sensorId)
{
    hSensorHandle = NULL;
    state = SENSOR_ERROR;
    focusSupported = false;

    init(sensorId);
}

#ifdef INFOTM_ISP

ISPC::Sensor::Sensor(int sensorId, int index) :
        programmedExposure(SENSOR_EXPOSURE.def),
        programmedGain(SENSOR_GAIN.def),
        programmedFocus(0),
        minExposure(SENSOR_EXPOSURE.min),
        maxExposure(SENSOR_EXPOSURE.max),
        minFocus(0),
        maxFocus(0),
        minGain(SENSOR_GAIN.min),
        maxGain(SENSOR_GAIN.max),
        focusSupported(false),
        uiWidth(SENSOR_SIZE.def[0]),
        uiHeight(SENSOR_SIZE.def[1]),
        uiImager(0),
        nVTot(SENSOR_VTOT.def),
        eBayerFormat(MOSAIC_NONE),
        ui8SensorContexts(0),
        uiBitDepth(SENSOR_BITDEPTH.def),
        flFrameRate(SENSOR_FRAMERATE.def),
        uiWellDepth(SENSOR_WELLDEPTH.def),
        flReadNoise(SENSOR_READNOISE.def),
        flAperture(0.0),
        uiFocalLength(0)
{
    hSensorHandle = NULL;
    state = SENSOR_ERROR;
    focusSupported = false;

    init(sensorId, index);
}

IMG_RESULT ISPC::Sensor::GetSensorState()
{
	return this->state;
}

#endif //INFOTM_ISP

#ifdef INFOTM_ISP
void ISPC::Sensor::init(int sensorId, int index)
{
    IMG_RESULT ret;

    ret = Sensor_Initialise(sensorId, &hSensorHandle, index);

    if (!hSensorHandle || ret)
    {
        LOG_ERROR("Failed to get a sensor handle for identifier %d.\n",
            sensorId);
        state = SENSOR_ERROR;
    }
    else
    {
        state = SENSOR_INITIALIZED;
    }
}
#else

void ISPC::Sensor::init(int sensorId)
{
    IMG_RESULT ret;

    ret = Sensor_Initialise(sensorId, &hSensorHandle);

    if (!hSensorHandle || ret)
    {
        LOG_ERROR("Failed to get a sensor handle for identifier %d.\n",
            sensorId);
        state = SENSOR_ERROR;
    }
    else
    {
        state = SENSOR_INITIALIZED;
    }
}
#endif
ISPC::Sensor::~Sensor()
{
    // disable the sensor if it was enabled
    if (hSensorHandle)
    {
        if (SENSOR_ENABLED == state)
        {
            Sensor_Disable(hSensorHandle);
        }

        Sensor_Destroy(hSensorHandle);
    }
}

IMG_RESULT ISPC::Sensor::getMode(int mode, SENSOR_MODE &sMode) const
{
    IMG_RESULT ret;
    ret = Sensor_GetMode(hSensorHandle, mode, &sMode);
    return ret;
}

IMG_RESULT ISPC::Sensor::configure(int sensorMode, int flipping)
{
    SENSOR_INFO sCamInfo;
    IMG_RESULT ret = IMG_ERROR_FATAL;

    // sensor can only be enabled when the sensor is sending frames
    if (SENSOR_ENABLED == state)
    {
        LOG_ERROR("Sensor is in state %s, expecting !=%s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_SetMode(hSensorHandle, sensorMode, flipping);
    if (ret)
    {
        LOG_ERROR("failed to set sensor's mode to %d with flipping 0x%x\n",
            sensorMode, flipping);
        state = SENSOR_ERROR;
        return ret;
    }

    ret = Sensor_GetInfo(hSensorHandle, &sCamInfo);
    if (ret)
    {
        LOG_ERROR("failed to get sensor's info\n");
        state = SENSOR_ERROR;
        return ret;
    }
    state = SENSOR_CONFIGURED;

    this->uiImager = sCamInfo.ui8Imager;
    this->eBayerFormat = sCamInfo.eBayerEnabled;
    this->uiWellDepth = sCamInfo.ui32WellDepth;
    this->flReadNoise = sCamInfo.flReadNoise;
    this->flAperture = sCamInfo.fNumber;
    this->uiFocalLength = sCamInfo.ui16FocalLength;

    // this.uiImager =/// @ load this information from the mode

    this->uiBitDepth = sCamInfo.sMode.ui8BitDepth;
    this->flFrameRate = sCamInfo.sMode.flFrameRate;
#ifdef INFOTM_ISP
    this->flCurrentFPS = this->flFrameRate;
    IMG_STRCPY(this->pszSensorName, sCamInfo.pszSensorName);
#endif
    this->nVTot = sCamInfo.sMode.ui16VerticalTotal;
    this->uiWidth = sCamInfo.sMode.ui16Width;
    this->uiHeight = sCamInfo.sMode.ui16Height;

    // get exposure setings from the sensor
    ret = Sensor_GetExposure(hSensorHandle, &(this->programmedExposure), 0);
    if (ret)
    {
        LOG_WARNING("Failed to get initial exposure (returned %d)\n", ret);
        state = SENSOR_ERROR;
        return IMG_ERROR_FATAL;
    }
    ret = Sensor_GetExposureRange(hSensorHandle, &(this->minExposure),
        &(this->maxExposure), &(this->ui8SensorContexts));
    if (ret)
    {
        LOG_ERROR("Failed to get exposure range (returned %d)!\n", ret);
        state = SENSOR_ERROR;
        return IMG_ERROR_FATAL;
    }
    ret = Sensor_GetGainRange(hSensorHandle, &(this->minGain),
        &(this->maxGain), &(this->ui8SensorContexts));
    if (ret)
    {
        LOG_ERROR("Failed to get gain range (returned %d)!\n", ret);
        state = SENSOR_ERROR;
        return IMG_ERROR_FATAL;
    }
    this->programmedGain = 1.0;
    ret = Sensor_SetGain(hSensorHandle, this->programmedGain,
        this->ui8SensorContexts);
    if (ret)
    {
        LOG_ERROR("Failed to set initial gain to 1.0 (returned %d)\n", ret);
        state = SENSOR_ERROR;
        return IMG_ERROR_FATAL;
    }

    // get focus distance values from the sensor
    this->programmedFocus = 0;
    ret = Sensor_GetFocusRange(hSensorHandle, &(this->minFocus),
        &(this->maxFocus));
    if (IMG_SUCCESS == ret)
    {
        focusSupported = true;
        ret = Sensor_GetCurrentFocus(hSensorHandle, &(this->programmedFocus));
        if (ret)
        {
            LOG_WARNING("failed to get initial focus\n");
            state = SENSOR_ERROR;
            return IMG_ERROR_FATAL;
        }
    }

    LOG_INFO("Sensor information (mode %d)\n", sensorMode);
    LOG_INFO("\tResolution: %dx%d @ %3.2f FPS\n",
        uiWidth, uiHeight, flFrameRate);
    LOG_INFO("\tExposure min: %d, max: %d, programmed: %d\n",
        minExposure, maxExposure, programmedExposure);
    LOG_INFO("\tGain min: %f, max %f, programmed %f\n",
        minGain, maxGain, programmedGain);
    if (focusSupported)
    {
        LOG_INFO("\tFocus min: %u, max %u, current %u\n",
            minFocus, maxFocus, programmedFocus);
    }
    else
    {
        LOG_INFO("\tFocus: N/A\n");
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Sensor::enable()
{
    IMG_RESULT ret;

    if (SENSOR_CONFIGURED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_CONFIGURED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_Enable(hSensorHandle);
    if (ret)
    {
        LOG_ERROR("Failed to start transmitting data from the sensor!\n");
        state = SENSOR_ERROR;
    }
    else
    {
        state = SENSOR_ENABLED;
    }

    return ret;
}

IMG_RESULT ISPC::Sensor::disable()
{
    IMG_RESULT ret;

    if (SENSOR_ENABLED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_Disable(hSensorHandle);
    if (ret)
    {
        LOG_ERROR("Failed to stop transmitting data from the sensor!\n");
        state = SENSOR_ERROR;
    }
    else
    {
        state = SENSOR_CONFIGURED;
    }
    return ret;
}

IMG_RESULT ISPC::Sensor::setExposure(unsigned long uSeconds)
{
    IMG_RESULT ret;
    if (SENSOR_ENABLED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_SetExposure(hSensorHandle, uSeconds, ui8SensorContexts);
    if (ret)
    {
        LOG_ERROR("Failed to set exposure to %u\n", uSeconds);
        return ret;
    }

    ret = Sensor_GetExposure(hSensorHandle, &programmedExposure,
        ui8SensorContexts);
    if (ret)
    {
        LOG_WARNING("Failed to acquire new programmed exposure!\n");
    }
    LOG_DEBUG("current exposure %u\n", programmedExposure);

    // could return different code if uSeconds != programmedExposure
    return IMG_SUCCESS;
}

unsigned long ISPC::Sensor::getExposure() const
{
    return programmedExposure;
}

IMG_RESULT ISPC::Sensor::setGain(double gain)
{
    if (SENSOR_ENABLED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    Sensor_SetGain(hSensorHandle, gain, ui8SensorContexts);

    Sensor_GetCurrentGain(hSensorHandle, &programmedGain, ui8SensorContexts);

    // could return different code if gain is different than programmedGain
    return IMG_SUCCESS;
}

double ISPC::Sensor::getGain() const
{
    return programmedGain;
}

SENSOR_HANDLE ISPC::Sensor::getHandle()
{
    return hSensorHandle;
}

IMG_RESULT ISPC::Sensor::setFocusDistance(unsigned int focusDistance)
{
    if (focusSupported)
    {
        if (focusDistance < minFocus || focusDistance > maxFocus)
        {
            LOG_WARNING("Requested focus distance %d out of range (%d; %d). "
                "Setting focus distance to minimum value\n",
                focusDistance, minFocus, maxFocus);
            focusDistance = minFocus;
        }

        Sensor_SetFocus(hSensorHandle, focusDistance);

        Sensor_GetCurrentFocus(hSensorHandle, &programmedFocus);
    }

    // could return different code if focusDistance different than
    // programmedFocus
    return IMG_SUCCESS;
}

unsigned int ISPC::Sensor::getFocusDistance() const
{
    return programmedFocus;
}

unsigned long ISPC::Sensor::getMaxExposure() const
{
    return (unsigned long)this->maxExposure;
}

unsigned long ISPC::Sensor::getMinExposure() const
{
    return (unsigned long)this->minExposure;
}

unsigned int ISPC::Sensor::getMinFocus() const
{
    return minFocus;
}

unsigned int ISPC::Sensor::getMaxFocus() const
{
    return maxFocus;
}

double ISPC::Sensor::getMaxGain() const
{
    return this->maxGain;
}

// added by linyun.xiong @2015-07-15 for support change Max Gain
#ifdef INFOTM_ISP_TUNING
void ISPC::Sensor::setMaxGain(double maxGain)
{
    this->maxGain = maxGain;
}
#endif //INFOTM_ISP_TUNING
ISPC::Sensor::State ISPC::Sensor::getState() const
{
    return this->state;
}

double ISPC::Sensor::getMinGain() const
{
    return this->minGain;
}

// added by linyun.xiong @2015-07-15 for support change Min Gain
#ifdef INFOTM_ISP_TUNING
void ISPC::Sensor::setMinGain(double minGain)
{
    this->minGain = minGain;
}
#endif //INFOTM_ISP_TUNING

bool ISPC::Sensor::getFocusSupported() const
{
    return focusSupported;
}

IMG_RESULT ISPC::Sensor::insert()
{
    return Sensor_Insert(hSensorHandle);
}

IMG_RESULT ISPC::Sensor::load(const ISPC::ParameterList &parameters)
{
    // no parameters to load as this is a real sensor

    if (parameters.exists(ModuleDNS::DNS_WELLDEPTH.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleDNS::DNS_WELLDEPTH.name.c_str(),
            Sensor::SENSOR_WELLDEPTH.name.c_str());
        uiWellDepth = parameters.getParameter(ModuleDNS::DNS_WELLDEPTH);
    }
    else
    {
        uiWellDepth = parameters.getParameter(Sensor::SENSOR_WELLDEPTH);
    }

    if (parameters.exists(ModuleDNS::DNS_READNOISE.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleDNS::DNS_READNOISE.name.c_str(),
            Sensor::SENSOR_READNOISE.name.c_str());
        flReadNoise = parameters.getParameter(ModuleDNS::DNS_READNOISE);
    }
    else
    {
        flReadNoise = parameters.getParameter(Sensor::SENSOR_READNOISE);
    }
    
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Sensor::save(ISPC::ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = Sensor::GetGroup();
    }

    parameters.addGroup("Sensor", group);

    switch (t)
    {
    case ModuleBase::SAVE_VAL:
        parameters.addParameter(Parameter(SENSOR_EXPOSURE.name,
            toString(this->programmedExposure/1000.0)));
        parameters.addParameter(Parameter(SENSOR_GAIN.name,
            toString(this->programmedGain)));
        parameters.addParameter(Parameter(SENSOR_BITDEPTH.name,
            toString(this->uiBitDepth)));
        parameters.addParameter(Parameter(SENSOR_WELLDEPTH.name,
            toString(this->uiWellDepth)));
        parameters.addParameter(Parameter(SENSOR_READNOISE.name,
            toString(this->flReadNoise)));
        parameters.addParameter(Parameter(SENSOR_FRAMERATE.name,
            toString(this->flFrameRate)));

        values.clear();
        values.push_back(toString(uiWidth));
        values.push_back(toString(uiHeight));
        parameters.addParameter(Parameter(SENSOR_SIZE.name, values));

        parameters.addParameter(Parameter(SENSOR_VTOT.name,
            toString(this->nVTot)));
        break;

    case ModuleBase::SAVE_MIN:
        parameters.addParameter(Parameter(SENSOR_EXPOSURE.name,
            toString(SENSOR_EXPOSURE.min)));
        parameters.addParameter(Parameter(SENSOR_GAIN.name,
            toString(SENSOR_GAIN.min)));
        parameters.addParameter(Parameter(SENSOR_BITDEPTH.name,
            toString(SENSOR_BITDEPTH.min)));
        parameters.addParameter(Parameter(SENSOR_WELLDEPTH.name,
            toString(SENSOR_WELLDEPTH.min)));
        parameters.addParameter(Parameter(SENSOR_READNOISE.name,
            toString(SENSOR_READNOISE.min)));
        parameters.addParameter(Parameter(SENSOR_FRAMERATE.name,
            toString(SENSOR_READNOISE.min)));

        values.clear();
        for (i = 0; i < 2; i++)
        {
            values.push_back(toString(SENSOR_SIZE.min));
        }
        parameters.addParameter(Parameter(SENSOR_SIZE.name, values));

        parameters.addParameter(Parameter(SENSOR_VTOT.name,
            toString(SENSOR_READNOISE.min)));
        break;

    case ModuleBase::SAVE_MAX:
        parameters.addParameter(Parameter(SENSOR_EXPOSURE.name,
            toString(SENSOR_EXPOSURE.max)));
        parameters.addParameter(Parameter(SENSOR_GAIN.name,
            toString(SENSOR_GAIN.max)));
        parameters.addParameter(Parameter(SENSOR_BITDEPTH.name,
            toString(SENSOR_BITDEPTH.max)));
        parameters.addParameter(Parameter(SENSOR_WELLDEPTH.name,
            toString(SENSOR_WELLDEPTH.max)));
        parameters.addParameter(Parameter(SENSOR_READNOISE.name,
            toString(SENSOR_READNOISE.max)));
        parameters.addParameter(Parameter(SENSOR_FRAMERATE.name,
            toString(SENSOR_READNOISE.max)));

        values.clear();
        for (i = 0; i < 2; i++)
        {
            values.push_back(toString(SENSOR_SIZE.max));
        }
        parameters.addParameter(Parameter(SENSOR_SIZE.name, values));

        parameters.addParameter(Parameter(SENSOR_VTOT.name,
            toString(SENSOR_READNOISE.max)));
        break;

    case ModuleBase::SAVE_DEF:
        parameters.addParameter(Parameter(SENSOR_EXPOSURE.name,
            toString(SENSOR_EXPOSURE.def)
            + " // " + getParameterInfo(SENSOR_EXPOSURE) + " (in ms)"));
        parameters.addParameter(Parameter(SENSOR_GAIN.name,
            toString(SENSOR_GAIN.def)
            + " // " + getParameterInfo(SENSOR_GAIN)));
        parameters.addParameter(Parameter(SENSOR_BITDEPTH.name,
            toString(SENSOR_BITDEPTH.def)
            + " // " + getParameterInfo(SENSOR_BITDEPTH)));
        parameters.addParameter(Parameter(SENSOR_WELLDEPTH.name,
            toString(SENSOR_WELLDEPTH.def)
            + " // " + getParameterInfo(SENSOR_WELLDEPTH)));
        parameters.addParameter(Parameter(SENSOR_READNOISE.name,
            toString(SENSOR_READNOISE.def)
            + " // " + getParameterInfo(SENSOR_READNOISE)));
        parameters.addParameter(Parameter(SENSOR_FRAMERATE.name,
            toString(SENSOR_FRAMERATE.def)
            + " // " + getParameterInfo(SENSOR_FRAMERATE)));

        values.clear();
        for (i = 0; i < 2; i++)
        {
            values.push_back(toString(SENSOR_SIZE.max));
        }
        values.push_back("// " + getParameterInfo(SENSOR_SIZE));
        parameters.addParameter(Parameter(SENSOR_SIZE.name, values));

        parameters.addParameter(Parameter(SENSOR_VTOT.name,
            toString(SENSOR_VTOT.def)
            + " // " + getParameterInfo(SENSOR_VTOT)));
        break;
    }

    return IMG_SUCCESS;
}


//
// Data Generator Sensor
//

ISPC::DGSensor::DGSensor(const std::string &filename, IMG_UINT8 gasketNumber,
    bool _isInternal)
    : Sensor(), isInternal(_isInternal)
{
    CI_CONNECTION *pConn = ciConnection.getConnection();
    int sensorId = 0;

    if (!pConn)
    {
        LOG_ERROR("Failed to connect to CI\n");
        return;
    }

    if (_isInternal)
    {
        sensorId = GetSensorId(IIFDG_SENSOR_INFO_NAME);
    }
    else
    {
#ifdef ISPC_EXT_DATA_GENERATOR
        sensorId = GetSensorId(EXTDG_SENSOR_INFO_NAME);
#else
        LOG_ERROR("external DG not supported!\n");
        return;
#endif
    }
#ifdef INFOTM_ISP
    init(sensorId, 0);
#else
    init(sensorId);
#endif
    if (SENSOR_ERROR == state)
    {
        LOG_ERROR("Failed to initialise sensor\n");
        return;
    }

    if (isInternal)
    {
        IMG_BOOL8 bIsVideo = IMG_TRUE;  // always on
        IMG_RESULT ret;

        ret = IIFDG_ExtendedSetConnection(hSensorHandle, pConn);
        if (ret)
        {
            LOG_ERROR("Failed to setup IIFDG connection\n");
            state = SENSOR_ERROR;
            return;
        }
        ret = IIFDG_ExtendedSetSourceFile(hSensorHandle, filename.c_str());
        if (ret)
        {
            LOG_ERROR("Failed to setup IIFDG source file\n");
            state = SENSOR_ERROR;
            return;
        }
        ret = IIFDG_ExtendedSetGasket(hSensorHandle, gasketNumber);
        if (ret)
        {
            LOG_ERROR("Failed to setup IIFDG gasket\n");
            state = SENSOR_ERROR;
            return;
        }
        ret = IIFDG_ExtendedSetIsVideo(hSensorHandle, bIsVideo);
        if (ret)
        {
            LOG_ERROR("Failed to setup IIFDG isVideo\n");
            state = SENSOR_ERROR;
            return;
        }
    }
    else
    {
#ifdef ISPC_EXT_DATA_GENERATOR
        IMG_RESULT ret;

        ret = DGCam_ExtendedSetConnection(hSensorHandle, pConn);

        ret = DGCam_ExtendedSetSourceFile(hSensorHandle, filename.c_str());

        ret = DGCam_ExtendedSetGasket(hSensorHandle, gasketNumber);

#else
        LOG_ERROR("external DG not supported!\n");
        return;
#endif
    }
}

ISPC::DGSensor::~DGSensor()
{
    // even if ~Sensor() disables the sensor, we must explicitly do it here
    // before disconnecting from kernel
    if (hSensorHandle && state == SENSOR_ENABLED)
    {
        Sensor_Disable(hSensorHandle);
        state = SENSOR_CONFIGURED;
    }
    // destruction of CI_Connection disconnects from CI
}

IMG_RESULT ISPC::DGSensor::load(const ISPC::ParameterList &parameters)
{
    // SENSOR_EXPOSURE is not loaded

    if (parameters.exists(ModuleDNS::DNS_ISOGAIN.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleDNS::DNS_ISOGAIN.name.c_str(),
            Sensor::SENSOR_GAIN.name.c_str());
        programmedGain = parameters.getParameter(ModuleDNS::DNS_ISOGAIN);
    }
    else
    {
        programmedGain = parameters.getParameter(Sensor::SENSOR_GAIN);
    }

    // SENSOR_BITDEPTH should come from the configured input file not setup

    if (parameters.exists(ModuleDNS::DNS_WELLDEPTH.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleDNS::DNS_WELLDEPTH.name.c_str(),
            Sensor::SENSOR_WELLDEPTH.name.c_str());
        uiWellDepth = parameters.getParameter(ModuleDNS::DNS_WELLDEPTH);
    }
    else
    {
        uiWellDepth = parameters.getParameter(Sensor::SENSOR_WELLDEPTH);
    }

    if (parameters.exists(ModuleDNS::DNS_READNOISE.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleDNS::DNS_READNOISE.name.c_str(),
            Sensor::SENSOR_READNOISE.name.c_str());
        flReadNoise = parameters.getParameter(ModuleDNS::DNS_READNOISE);
    }
    else
    {
        flReadNoise = parameters.getParameter(Sensor::SENSOR_READNOISE);
    }

    if (parameters.exists(ModuleFLD::FLD_FRAMERATE.name))
    {
        LOG_WARNING("%s is deprecated use %s instead!\n",
            ModuleFLD::FLD_FRAMERATE.name.c_str(),
            Sensor::SENSOR_FRAMERATE.name.c_str());
        flReadNoise = parameters.getParameter(ModuleFLD::FLD_FRAMERATE);
    }
    else
    {
        flFrameRate = parameters.getParameter(Sensor::SENSOR_FRAMERATE);
    }

    // SENSOR_VTOT should come from the configured data generator

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::Sensor::setISOLimit(IMG_UINT8 pISOLimit)
{
	switch (pISOLimit)
	{
		case 0:
			this->minGain = 1.0f;
			this->maxGain = 4.0f;
			break;

		case 1:
			this->minGain = 1.0f;
			this->maxGain = 16.0f;
			break;

		case 2:
			this->minGain = 1.0f;
			this->maxGain = 64.0f;
			break;

		 default:
			this->minGain = 1.0f;
			this->maxGain = 16.0f;
			break;
	}

	return IMG_SUCCESS;
}

// set sensor flip & mirror, 0~3
IMG_RESULT ISPC::Sensor::setFlipMirror(IMG_UINT8 flag)
{
    if(state != SENSOR_ENABLED)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return Sensor_SetFlipMirror(hSensorHandle, flag);
}

IMG_RESULT ISPC::Sensor::SetFPS(double fps)
{
	SENSOR_STATUS status;
	IMG_RESULT ret = IMG_ERROR_FATAL;
	if(state != SENSOR_ENABLED)
	{
		LOG_ERROR("Sensor is in state %s, expecting %s\n",
			Sensor::StateName(state),
			Sensor::StateName(SENSOR_ENABLED));
		return IMG_ERROR_NOT_SUPPORTED;
	}
	ret =  Sensor_SetFPS(hSensorHandle, fps);
	if(IMG_SUCCESS != ret)
	{
	   return ret;
	}
	ret = Sensor_GetState(hSensorHandle, &status);
	if(IMG_SUCCESS != ret)
	{
	   return ret;
	}
	this->flCurrentFPS = status.fCurrentFps;
	return IMG_SUCCESS;

}

IMG_RESULT ISPC::Sensor::GetFixedFPS(int *pfixed)
{
	if(state != SENSOR_ENABLED)
	{
		LOG_ERROR("Sensor is in state %s, expecting %s\n",
			Sensor::StateName(state),
			Sensor::StateName(SENSOR_ENABLED));
		return IMG_ERROR_NOT_SUPPORTED;
	}

	return Sensor_GetFixedFPS(hSensorHandle, pfixed);
}

IMG_RESULT ISPC::Sensor::GetFlipMirror(SENSOR_STATUS &status)
{

	IMG_RESULT ret;
	if (SENSOR_ENABLED != state)
	{
		LOG_ERROR("Sensor is in state %s, expecting %s\n",
			Sensor::StateName(state),
			Sensor::StateName(SENSOR_ENABLED));
		return IMG_ERROR_NOT_SUPPORTED;
	}

	ret = Sensor_GetState(hSensorHandle, &status);
	if(IMG_SUCCESS !=ret)
	{
		return IMG_ERROR_NOT_SUPPORTED;
	}

	return IMG_SUCCESS;
}

IMG_RESULT ISPC::Sensor::SetGainAndExposure(double flGain, IMG_UINT32 ui32Exposure)
{
     if(state != SENSOR_ENABLED)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    Sensor_SetGainAndExposure(hSensorHandle, flGain, ui32Exposure, ui8SensorContexts);
    
    Sensor_GetExposure(hSensorHandle, &programmedExposure, ui8SensorContexts);
    Sensor_GetCurrentGain(hSensorHandle, &programmedGain, ui8SensorContexts);
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Sensor::SetResolution(int imgW, int imgH)
{
	if(state != SENSOR_ENABLED)
	{
		LOG_ERROR("Sensor is in state %s, expecting %s\n",
			Sensor::StateName(state),
			Sensor::StateName(SENSOR_ENABLED));
		return IMG_ERROR_NOT_SUPPORTED;
	}

	return Sensor_SetResolution(hSensorHandle, imgW, imgH);
}

IMG_RESULT ISPC::Sensor::GetSnapResolution(reslist_t &resList)
{
	if(state != SENSOR_ENABLED)
	{
		LOG_ERROR("Sensor is in state %s, expecting %s\n",
			Sensor::StateName(state),
			Sensor::StateName(SENSOR_ENABLED));
		return IMG_ERROR_NOT_SUPPORTED;
	}
	reslist_t res ;
	int ret = IMG_SUCCESS;
	ret = Sensor_GetSnapRes(hSensorHandle, &res);
	if( ret != IMG_SUCCESS)
	{
		LOG_ERROR("Sensor snap resolution failed\n");
		return IMG_ERROR_FATAL;
	}

	resList = res;
	return IMG_SUCCESS;
}

IMG_RESULT ISPC::Sensor::reset()
{
    IMG_RESULT ret;

    if (SENSOR_ENABLED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_Reset(hSensorHandle);
    if (ret)
    {
        LOG_ERROR("Failed to reset sensor\n");
        state = SENSOR_ERROR;
    }
    else
    {
        state = SENSOR_ENABLED;
    }
    return ret;
}

IMG_RESULT ISPC::Sensor::GetGasketNum(IMG_UINT32 *Gasket)
{
    IMG_RESULT ret;
    if (SENSOR_ENABLED != state)
    {
        LOG_ERROR("Sensor is in state %s, expecting %s\n",
            Sensor::StateName(state),
            Sensor::StateName(SENSOR_ENABLED));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = Sensor_GetGasketNum(hSensorHandle, Gasket);
    if(IMG_SUCCESS !=ret)
    {
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}
#endif //INFOTM_ISP
