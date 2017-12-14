/**
*******************************************************************************
 @file ModuleWBC.cpp

 @brief Implementation of ISPC::ModuleWBC

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
#define LOG_TAG "ISPC_MOD_WBC"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"


const ISPC::ParamDefArray<double> ISPC::ModuleWBC::WBC_GAIN(
    "LSH_WBGAIN", WBC_GAIN_MIN, WBC_GAIN_MAX, WBC_GAIN_DEF, 4);

const ISPC::ParamDefArray<double> ISPC::ModuleWBC::WBC_CLIP(
    "LSH_WBCLIP", WBC_CLIP_MIN, WBC_CLIP_MAX, WBC_CLIP_DEF, 4);

ISPC::ParameterGroup ISPC::ModuleWBC::getGroup()
{
    ParameterGroup group;

    group.header = "// White Balance Correction parameters";

    group.parameters.insert(WBC_GAIN.name);
    group.parameters.insert(WBC_CLIP.name);

    return group;
}

ISPC::ModuleWBC::ModuleWBC(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleWBC::load(const ParameterList &parameters)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        aWBGain[i] = parameters.getParameter(WBC_GAIN, i);
#ifdef INFOTM_SW_AWB_METHOD
		aWBGain_def[i] = parameters.getParameter(WBC_GAIN, i);
#endif //INFOTM_SW_AWB_METHOD
    }
    for (i = 0; i < 4; i++)
    {
        aWBClip[i] = parameters.getParameter(WBC_CLIP, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBC::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleWBC::getGroup();
    }

    parameters.addGroup("ModuleWBC", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(this->aWBGain[i]));
        }
        parameters.addParameter(Parameter(WBC_GAIN.name, values));

        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(this->aWBClip[i]));
        }
        parameters.addParameter(Parameter(WBC_CLIP.name, values));
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_GAIN.min));
        }
        parameters.addParameter(Parameter(WBC_GAIN.name, values));

        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_CLIP.min));
        }
        parameters.addParameter(Parameter(WBC_CLIP.name, values));
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_GAIN.max));
        }
        parameters.addParameter(Parameter(WBC_GAIN.name, values));

        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_CLIP.max));
        }
        parameters.addParameter(Parameter(WBC_CLIP.name, values));
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_GAIN.def[i]));
        }
        values.push_back("// " + getParameterInfo(WBC_GAIN));
        parameters.addParameter(Parameter(WBC_GAIN.name, values));

        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(WBC_CLIP.def[i]));
        }
        values.push_back("// " + getParameterInfo(WBC_CLIP));
        parameters.addParameter(Parameter(WBC_CLIP.name, values));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBC::setup()
{
    int i;
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

    for (i = 0; i < LSH_GRADS_NO; i++)
    {
        // white balance gain per channel, float between 0.5-8
        pMCPipeline->sWBC.aGain[i] = aWBGain[i];
        // clipping level per channel
        pMCPipeline->sWBC.aClip[i] = aWBClip[i] * 256.0;
    }

    this->setupFlag = true;
    return IMG_SUCCESS;
}
