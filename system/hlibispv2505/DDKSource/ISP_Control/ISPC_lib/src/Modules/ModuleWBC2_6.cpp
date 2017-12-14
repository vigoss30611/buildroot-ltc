 /**
*******************************************************************************
 @file ModuleWBC2_6.cpp

 @brief Implementation of ISPC::ModuleWBC2_6

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
#include "ispc/ModuleWBC.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_WBC2_6"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

static const double WBC_RGB_GAIN_DEF[3] = { 1.0, 1.0, 1.0 };
const ISPC::ParamDefArray<double> ISPC::ModuleWBC2_6::WBC_RGB_GAIN(
    "WBC_RGB_GAIN", 0.0, 16.0, WBC_RGB_GAIN_DEF, 3);

static const double WBC_RGB_THRESHOLD_DEF[3] = { 1.0, 1.0, 1.0 };
const ISPC::ParamDefArray<double> ISPC::ModuleWBC2_6::WBC_RGB_THRESHOLD(
    "WBC_RGB_THRESHOLD", 0.0, 1.0, WBC_RGB_THRESHOLD_DEF, 3);

#define WBC_RGB_SATURATION_STR "saturation"
#define WBC_RGB_THRESHOLD_STR "threshold"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleWBC2_6::WBC_RGB_MODE(
    "WBC_RGB_MODE", WBC_RGB_SATURATION_STR);

ISPC::ParameterGroup ISPC::ModuleWBC2_6::getGroup()
{
    // separate the 2 groups
    ParameterGroup group;  // = ModuleWBC::getGroup();
    group.header = "// White Balance Clipping parameters (HW 2.6)";

    group.parameters.insert(WBC_RGB_GAIN.name);
    group.parameters.insert(WBC_RGB_THRESHOLD.name);
    group.parameters.insert(WBC_RGB_MODE.name);

    return group;
}

ISPC::ModuleWBC2_6::ModuleWBC2_6() : ModuleWBC(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

enum WBC_MODES ISPC::ModuleWBC2_6::getRGBMode(const std::string &param)
{
    enum WBC_MODES e = (enum WBC_MODES) -1;  // invalid mode
    if (0 == param.compare(WBC_RGB_SATURATION_STR))
    {
        e = WBC_SATURATION;
    }
    else if (0 == param.compare(WBC_RGB_THRESHOLD_STR))
    {
        e = WBC_THRESHOLD;
    }
    return e;
}

std::string ISPC::ModuleWBC2_6::getRGBModeString(enum WBC_MODES e)
{
    std::string value("unknown");

    switch (e)
    {
    case WBC_SATURATION:
        value = WBC_RGB_SATURATION_STR;
        break;
    case WBC_THRESHOLD:
        value = WBC_RGB_THRESHOLD_STR;
        break;
    }
    return value;
}

IMG_RESULT ISPC::ModuleWBC2_6::load(const ParameterList &parameters)
{
    int i;
    IMG_RESULT ret = ModuleWBC::load(parameters);
    if (ret)
    {
        MOD_LOG_ERROR("loading ModuleWBC parameters - returned %d\n", ret);
        return ret;
    }

    for (i = 0; i < 3; i++)
    {
        afRGBGain[i] = parameters.getParameter(WBC_RGB_GAIN, i);
        afRGBThreshold[i] = parameters.getParameter(WBC_RGB_THRESHOLD, i);
    }

    std::string modeType = parameters.getParameter(WBC_RGB_MODE);
    eRGBThresholdMode = getRGBMode(modeType);
    if (-1 == (int)eRGBThresholdMode)
    {
        MOD_LOG_ERROR("Invalid saturation mode value %s\n", modeType.c_str());
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBC2_6::save(ParameterList &parameters,
    SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;
    IMG_RESULT ret = ModuleWBC::save(parameters, t);

    if (ret)
    {
        MOD_LOG_ERROR("failed to save ModuleWBC parameters - returned %d\n",
            ret);
        return ret;
    }

    if (0 == group.parameters.size())
    {
        group = ModuleWBC2_6::getGroup();
    }

    parameters.addGroup("ModuleWBC2_6", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0; i < 3; i++)
        {
            values.push_back(toString(this->afRGBGain[i]));
        }
        parameters.addParameter(WBC_RGB_GAIN, values);

        values.clear();
        for (i = 0; i < 3; i++)
        {
            values.push_back(toString(this->afRGBThreshold[i]));
        }
        parameters.addParameter(WBC_RGB_THRESHOLD, values);
        parameters.addParameter(WBC_RGB_MODE,
                getRGBModeString(eRGBThresholdMode));
        break;

    case SAVE_MIN:
        parameters.addParameterMin(WBC_RGB_GAIN);
        parameters.addParameterMin(WBC_RGB_THRESHOLD);
        parameters.addParameterMin(WBC_RGB_MODE);  // no min for a string
        break;

    case SAVE_MAX:
        parameters.addParameterMax(WBC_RGB_GAIN);
        parameters.addParameterMax(WBC_RGB_THRESHOLD);
        parameters.addParameterMax(WBC_RGB_MODE);  // no max for a string
        break;

    case SAVE_DEF:
        parameters.addParameterDef(WBC_RGB_GAIN);
        parameters.addParameterDef(WBC_RGB_THRESHOLD);
        parameters.addParameterDef(WBC_RGB_MODE);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBC2_6::setup()
{
    LOG_PERF_IN();
    int i;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_RESULT ret;
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

    ret = ModuleWBC::setup();
    if (ret)
    {
        MOD_LOG_ERROR("failed to setup ModuleWBC - returned %d\n", ret);
        return ret;
    }

    for (i = 0; i < WBC_CHANNEL_NO; i++)
    {
        // white balance gain per channel, float between 0.5-8
        pMCPipeline->sWBC.afRGBGain[i] = afRGBGain[i];
        // clipping level per channel
        pMCPipeline->sWBC.afRGBThreshold[i] = afRGBThreshold[i];
    }
    pMCPipeline->sWBC.eRGBThresholdMode = eRGBThresholdMode;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
