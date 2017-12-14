/**
******************************************************************************
 @file ModuleDNS.cpp

 @brief Implementation of ISPC::ModuleDNS

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
#include "ispc/ModuleDNS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>

#define LOG_TAG "ISPC_MOD_DNS"

#define SUPPORT_LOAD_SETUP_SETTING

#include <cmath>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

// to access to the parameters to add them to the denoiser group
#include "ispc/ModuleSHA.h"
#include "ispc/Pipeline.h"
#include "ispc/Sensor.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleDNS::DNS_COMBINE(
    "DNS_COMBINE_ENABLE", true);
const ISPC::ParamDef<double> ISPC::ModuleDNS::DNS_STRENGTH(
    "DNS_STRENGTH", 0.0, 6.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleDNS::DNS_GREYSCALE_THRESH(
    "DNS_GREYSC_PIXTHRESH_MULT", 0.07, 14.5, 0.25);

// deprecated
const ISPC::ParamDef<double> ISPC::ModuleDNS::DNS_ISOGAIN(
    "DNS_ISO_GAIN", 0.0, 128.0, 1.0);
// deprecated
const ISPC::ParamDef<int> ISPC::ModuleDNS::DNS_SENSORDEPTH(
    "DNS_SENSOR_BITDEPTH", 8, 16, 10);
// deprecated
const ISPC::ParamDef<int> ISPC::ModuleDNS::DNS_WELLDEPTH("DNS_WELL_DEPTH",
    0, 65535, 5000);
// deprecated
const ISPC::ParamDef<double> ISPC::ModuleDNS::DNS_READNOISE("DNS_READ_NOISE",
    0.0, 100.0, 0.0);

ISPC::ParameterGroup ISPC::ModuleDNS::GetGroup()
{
    ParameterGroup group;

    group.header = "// Denoiser parameters";

    group.parameters.insert(DNS_COMBINE.name);
    group.parameters.insert(DNS_STRENGTH.name);
    group.parameters.insert(DNS_GREYSCALE_THRESH.name);
    group.parameters.insert(DNS_ISOGAIN.name);
    group.parameters.insert(DNS_SENSORDEPTH.name);
    group.parameters.insert(DNS_WELLDEPTH.name);
    group.parameters.insert(DNS_READNOISE.name);

#ifndef INFOTM_ISP
    // add the secondary denoiser parameters
    group.parameters.insert(ModuleSHA::SHA_DENOISE_BYPASS.name);
    group.parameters.insert(ModuleSHA::SHADN_TAU.name);
    group.parameters.insert(ModuleSHA::SHADN_SIGMA.name);
#endif //INFOTM_ISP

    return group;
}

ISPC::ModuleDNS::ModuleDNS()
    : SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleDNS::load(const ParameterList &parameters)
{
    const Sensor *sensor = NULL;
    if (pipeline)
    {
        sensor = pipeline->getSensor();
    }

    bCombine = parameters.getParameter(DNS_COMBINE);
    fStrength = parameters.getParameter(DNS_STRENGTH);
    fGreyscaleThreshold = parameters.getParameter(DNS_GREYSCALE_THRESH);

    if (sensor)
    {
        fSensorGain = sensor->getGain();
        ui32SensorBitdepth = sensor->uiBitDepth;
        ui32SensorWellDepth = sensor->uiWellDepth;
        fSensorReadNoise = sensor->flReadNoise;
    }
    else
    {
        // to initialise the values
        fSensorGain = 1.0;
        ui32SensorBitdepth = 10;
        ui32SensorWellDepth = 10;
        fSensorReadNoise = 1.0;
    }
#ifdef SUPPORT_LOAD_SETUP_SETTING
    // deprecated loading:
    {
        //fSensorGain = parameters.getParameter(DNS_ISOGAIN);
        //ui32SensorBitdepth = parameters.getParameter(DNS_SENSORDEPTH);
        ui32SensorWellDepth = parameters.getParameter(DNS_WELLDEPTH);
        fSensorReadNoise = parameters.getParameter(DNS_READNOISE);
    }
#endif

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDNS::save(ParameterList &parameters, SaveType t) const
{
    LOG_PERF_IN();
    static ParameterGroup group;

    if ( group.parameters.size() == 0 )
    {
        group = ModuleDNS::GetGroup();
    }

    parameters.addGroup("ModuleDNS", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(DNS_COMBINE, this->bCombine);
        parameters.addParameter(DNS_STRENGTH, this->fStrength);
        parameters.addParameter(DNS_GREYSCALE_THRESH, this->fGreyscaleThreshold);

        // deprecated parameters, added here for unit tests
        parameters.addParameter(Parameter("// " + DNS_ISOGAIN.name,
            "deprecated see " + Sensor::SENSOR_GAIN.name));
        parameters.addParameter(Parameter("// " + DNS_SENSORDEPTH.name,
            "deprecated see " + Sensor::SENSOR_BITDEPTH.name));
#ifdef INFOTM_ISP
        /* parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));*/
        /* parameters.addParameter(DNS_ISOGAIN, this->fSensorGain);
        parameters.addParameter(DNS_SENSORDEPTH, this->ui32SensorBitdepth);*/
        parameters.addParameter(DNS_WELLDEPTH, (int)this->ui32SensorWellDepth);
        parameters.addParameter(DNS_READNOISE, this->fSensorReadNoise);
#else
        parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));
        /* parameters.addParameter(DNS_ISOGAIN, this->fSensorGain);
        parameters.addParameter(DNS_SENSORDEPTH, this->ui32SensorBitdepth);
        parameters.addParameter(DNS_WELLDEPTH, this->ui32SensorWellDepth);
        parameters.addParameter(DNS_READNOISE, this->fSensorReadNoise);*/
#endif //INFOTM_ISP
        break;

    case SAVE_MIN:
        parameters.addParameterMin(DNS_COMBINE);  // bool does not have min
        parameters.addParameterMin(DNS_STRENGTH);
        parameters.addParameterMin(DNS_GREYSCALE_THRESH);
        // deprecated parameters, added here for unit tests
        parameters.addParameter(Parameter("// " + DNS_ISOGAIN.name,
            "deprecated see " + Sensor::SENSOR_GAIN.name));
        parameters.addParameter(Parameter("// " + DNS_SENSORDEPTH.name,
            "deprecated see " + Sensor::SENSOR_BITDEPTH.name));
#ifdef INFOTM_ISP
        /* parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));*/
        /* parameters.addParameterMin(DNS_ISOGAIN);
        parameters.addParameterMin(DNS_SENSORDEPTH);*/
        parameters.addParameterMin(DNS_WELLDEPTH);
        parameters.addParameterMin(DNS_READNOISE);
#else
        parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));
        /* parameters.addParameterMin(DNS_ISOGAIN);
        parameters.addParameterMin(DNS_SENSORDEPTH);
        parameters.addParameterMin(DNS_WELLDEPTH);
        parameters.addParameterMin(DNS_READNOISE);*/
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        parameters.addParameterMax(DNS_COMBINE);  // bool does not have max
        parameters.addParameterMax(DNS_STRENGTH);
        parameters.addParameterMax(DNS_GREYSCALE_THRESH);
        // deprecated parameters, added here for unit tests
        parameters.addParameter(Parameter("// " + DNS_ISOGAIN.name,
            "deprecated see " + Sensor::SENSOR_GAIN.name));
        parameters.addParameter(Parameter("// " + DNS_SENSORDEPTH.name,
            "deprecated see " + Sensor::SENSOR_BITDEPTH.name));
#ifdef INFOTM_ISP
        /* parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));*/
        /* parameters.addParameterMax(DNS_ISOGAIN);
        parameters.addParameterMax(DNS_SENSORDEPTH);*/
        parameters.addParameterMax(DNS_WELLDEPTH);
        parameters.addParameterMax(DNS_READNOISE);
#else
        parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));
        /* parameters.addParameterMax(DNS_ISOGAIN);
        parameters.addParameterMax(DNS_SENSORDEPTH);
        parameters.addParameterMax(DNS_WELLDEPTH);
        parameters.addParameterMax(DNS_READNOISE);*/
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        parameters.addParameterDef(DNS_COMBINE);
        parameters.addParameterDef(DNS_STRENGTH);
        parameters.addParameterDef(DNS_GREYSCALE_THRESH);
        // deprecated parameters
        parameters.addParameter(Parameter("// " + DNS_ISOGAIN.name,
            "deprecated see " + Sensor::SENSOR_GAIN.name));
        parameters.addParameter(Parameter("// " + DNS_SENSORDEPTH.name,
            "deprecated see " + Sensor::SENSOR_BITDEPTH.name));
#ifdef INFOTM_ISP
        /* parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));*/
        /*parameters.addParameterDef(DNS_ISOGAIN);
        parameters.addParameterDef(DNS_SENSORDEPTH);*/
        parameters.addParameterDef(DNS_WELLDEPTH);
        parameters.addParameterDef(DNS_READNOISE);
#else
        parameters.addParameter(Parameter("// " + DNS_WELLDEPTH.name,
            "deprecated see " + Sensor::SENSOR_WELLDEPTH.name));
        parameters.addParameter(Parameter("// " + DNS_READNOISE.name,
            "deprecated see " + Sensor::SENSOR_READNOISE.name));
        /*parameters.addParameterDef(DNS_ISOGAIN);
        parameters.addParameterDef(DNS_SENSORDEPTH);
        parameters.addParameterDef(DNS_WELLDEPTH);
        parameters.addParameterDef(DNS_READNOISE);*/
#endif //INFOTM_ISP
        break;
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDNS::setup()
{
    LOG_PERF_IN();
    IMG_INT32 iICorrected, i;
    double fSum1, fSum2;

    MC_PIPELINE *pMCPipeline = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // predefined PixThresh LUT data points
    static const IMG_INT32 DNS_PIXTHRESH_ENTRIES_0[8] = {
        0, 1, 2, 3, 4, 5, 6, 7
    };
    static const IMG_INT32 DNS_PIXTHRESH_ENTRIES_1[6] = {2, 3, 4, 5, 6, 7};
    static const IMG_INT32 DNS_PIXTHRESH_ENTRIES_2[6] = {2, 3, 4, 5, 6, 7};

    pMCPipeline->sDNS.bCombineChannels = bCombine;
    pMCPipeline->sDNS.fGreyscalePixelThreshold = fGreyscaleThreshold;

    // CALCULATION OF pixThresh LUT
    //////////////////////////////

    // calculate some reused values
#ifdef USE_MATH_NEON
    fSum1 = (double)powf_neon(
        (float)((1<<8) * fSensorGain * fSensorReadNoise /
        static_cast<double>(ui32SensorWellDepth)), 2.0f);
#else
    fSum1 = pow(
        (1<<8) * fSensorGain * fSensorReadNoise /
        static_cast<double>(ui32SensorWellDepth), 2);
#endif
    fSum2 = ((1<<8) * fSensorGain / ui32SensorWellDepth);

    //-- Fill in first segment of the pixThresh LUT
    for (i = 0; i < DNS_N_LUT0; i++)
    {
        iICorrected = DNS_PIXTHRESH_ENTRIES_0[i] << 1;
#ifdef USE_MATH_NEON
        pMCPipeline->sDNS.aPixThresLUT[i] = fStrength
            * (double)sqrtf_neon((float)(fSum1 + fSum2 * iICorrected));
#else
        pMCPipeline->sDNS.aPixThresLUT[i] = fStrength
            * sqrt(fSum1 + fSum2 * iICorrected);
#endif
    }

    //-- Fill in second segment of the pixThresh LUT
    for (i = 0; i < DNS_N_LUT1; i++)
    {
        iICorrected = DNS_PIXTHRESH_ENTRIES_1[i] << 3;
#ifdef USE_MATH_NEON
        pMCPipeline->sDNS.aPixThresLUT[i + DNS_N_LUT0] = fStrength
            * (double)sqrtf_neon((float)(fSum1 + fSum2 * iICorrected));
#else
        pMCPipeline->sDNS.aPixThresLUT[i + DNS_N_LUT0] = fStrength
            * sqrt(fSum1 + fSum2 * iICorrected);
#endif
    }

    //-- Fill in third segment of the pixThresh LUT
    for (i = 0; i < DNS_N_LUT2; i++)
    {
        iICorrected = DNS_PIXTHRESH_ENTRIES_2[i] << 5;
#ifdef USE_MATH_NEON
        pMCPipeline->sDNS.aPixThresLUT[i + DNS_N_LUT0 + DNS_N_LUT1] =
            fStrength * (double)sqrtf_neon((float)(fSum1 + fSum2 * iICorrected));
#else
        pMCPipeline->sDNS.aPixThresLUT[i + DNS_N_LUT0 + DNS_N_LUT1] =
            fStrength * sqrt(fSum1 + fSum2 * iICorrected);
#endif
    }

    //-- Fill last value of the pixThresh LUT
#ifdef USE_MATH_NEON
    pMCPipeline->sDNS.aPixThresLUT[20] = fStrength
        * (double)sqrtf_neon((float)(fSum1 + fSum2 * (2 << 7)));
#else
    pMCPipeline->sDNS.aPixThresLUT[20] = fStrength
        * sqrt(fSum1 + fSum2 * (2 << 7));
#endif

    /* It is required to set the sensor bitdepth for a proper
     * precision conversion */
    pMCPipeline->sDNS.ui8SensorBitdepth = ui32SensorBitdepth;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
