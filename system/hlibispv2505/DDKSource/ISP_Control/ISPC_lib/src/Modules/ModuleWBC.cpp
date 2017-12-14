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
#include "ispc/ModuleLSH.h"
#include "ispc/PerfTime.h"

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

ISPC::ModuleWBC::ModuleWBC(const std::string &loggingName)
    : SetupModuleBase(loggingName)
{
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
		aWBGain_def[i] = aWBGain[i];
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
        parameters.addParameter(WBC_GAIN, values);

        values.clear();
        for (i = 0; i < 4; i++)
        {
            values.push_back(toString(this->aWBClip[i]));
        }
        parameters.addParameter(WBC_CLIP, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(WBC_GAIN);
        parameters.addParameterMin(WBC_CLIP);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(WBC_GAIN);
        parameters.addParameterMax(WBC_CLIP);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(WBC_GAIN);
        parameters.addParameterDef(WBC_CLIP);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBC::setup()
{
    LOG_PERF_IN();
    int i;
    MC_PIPELINE *pMCPipeline = NULL;
    const ModuleLSH *pLSH = NULL;
    double scaleWB = 1.0;
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
    pLSH = pipeline->getModule<const ModuleLSH>();
    if (pLSH)
    {
        scaleWB = pLSH->getCurrentScaleWB();
        if (scaleWB != 1.0)
        {
            MOD_LOG_DEBUG("apply scaleWB of %lf\n", scaleWB);
        }
    }
    else
    {
        MOD_LOG_WARNING("pipeline does not have an LSH module to get "\
            "scaleWB from\n");
    }

    for (i = 0; i < LSH_GRADS_NO; i++)
    {
        // white balance gain per channel, float between 0.5-8
        pMCPipeline->sWBC.aGain[i] = aWBGain[i] * scaleWB;
        // clipping level per channel
        pMCPipeline->sWBC.aClip[i] = aWBClip[i] * 256.0;
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
