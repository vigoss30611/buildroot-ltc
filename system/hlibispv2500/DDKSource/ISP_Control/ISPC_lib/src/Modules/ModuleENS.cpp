/**
*******************************************************************************
 @file ModuleENS.cpp

 @brief Implementation of Encoder Statistics module

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
#include "ispc/ModuleENS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_ENS"

#include "ispc/Pipeline.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleENS::ENS_ENABLE("ENS_ENABLE",
    false);
const ISPC::ParamDef<int> ISPC::ModuleENS::ENS_REGION_NUMLINES(
    "ENS_REGION_NUMLINES", 8, 256, 16);  // must be a power of 2
const ISPC::ParamDef<int> ISPC::ModuleENS::ENS_SUBS_FACTOR("ENS_SUBS_FACTOR",
    1, 32, 1);  // must be a power of 2

ISPC::ParameterGroup ISPC::ModuleENS::getGroup()
{
    ParameterGroup group;

    group.header = "// Defective Pixels parameters";

    group.parameters.insert(ENS_ENABLE.name);
    group.parameters.insert(ENS_REGION_NUMLINES.name);
    group.parameters.insert(ENS_SUBS_FACTOR.name);

    return group;
}

ISPC::ModuleENS::ModuleENS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleENS::load(const ParameterList &parameters)
{
    IMG_UINT32 log2Value = 0, tmpValue = 0;

    bEnable = parameters.getParameter(ENS_ENABLE);
    ui32NLines = parameters.getParameter(ENS_REGION_NUMLINES);
    ui32SubsamplingFactor = parameters.getParameter(ENS_SUBS_FACTOR);

    log2Value = 0;
    tmpValue = ui32NLines;
    while (tmpValue >>= 1)  // compute log2()
    {
        log2Value++;
    }
    if ((1u << log2Value) != ui32NLines)
    {
        log2Value++;
        MOD_LOG_WARNING("loaded value for %s (%d) is not a power of 2!"\
            " Rounding to next power of 2: %d\n",
            ENS_REGION_NUMLINES.name.c_str(), ui32NLines, 1 << (log2Value));
        ui32NLines = 1 << (log2Value);
    }

    log2Value = 0;
    tmpValue = ui32SubsamplingFactor;
    while (tmpValue >>= 1)  // compute log2()
    {
        log2Value++;
    }
    if ((1u << log2Value) != ui32SubsamplingFactor)
    {
        log2Value++;
        MOD_LOG_WARNING("loaded value for %s (%d) is not a power of 2!"\
            " Rounding to next power of 2: %d\n",
            ENS_SUBS_FACTOR.name.c_str(), ui32SubsamplingFactor,
            1 << (log2Value));
        ui32SubsamplingFactor = 1 << (log2Value);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleENS::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleENS::getGroup();
    }

    parameters.addGroup("ModuleENS", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(ENS_ENABLE.name,
            toString(this->bEnable)));
        parameters.addParameter(Parameter(ENS_REGION_NUMLINES.name,
            toString(this->ui32NLines)));
        parameters.addParameter(Parameter(ENS_SUBS_FACTOR.name,
            toString(this->ui32SubsamplingFactor)));
    break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(ENS_ENABLE.name,
            toString(ENS_ENABLE.def)));  // bool do not have min
        parameters.addParameter(Parameter(ENS_REGION_NUMLINES.name,
            toString(ENS_REGION_NUMLINES.min)));
        parameters.addParameter(Parameter(ENS_SUBS_FACTOR.name,
            toString(ENS_SUBS_FACTOR.min)));
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(ENS_ENABLE.name,
            toString(ENS_ENABLE.def)));  // bool do not have max
        parameters.addParameter(Parameter(ENS_REGION_NUMLINES.name,
            toString(ENS_REGION_NUMLINES.max)));
        parameters.addParameter(Parameter(ENS_SUBS_FACTOR.name,
            toString(ENS_SUBS_FACTOR.max)));
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(ENS_ENABLE.name,
            toString(ENS_ENABLE.def) + " // " + getParameterInfo(ENS_ENABLE)));
        parameters.addParameter(Parameter(ENS_REGION_NUMLINES.name,
            toString(ENS_REGION_NUMLINES.def) + " // "
            + getParameterInfo(ENS_REGION_NUMLINES)));
        parameters.addParameter(Parameter(ENS_SUBS_FACTOR.name,
            toString(ENS_SUBS_FACTOR.def) + " // "
            + getParameterInfo(ENS_SUBS_FACTOR)));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleENS::setup()
{
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

    pMCPipeline->sENS.bEnable = bEnable;
    pMCPipeline->sENS.ui32NLines = ui32NLines;
    pMCPipeline->sENS.ui32KernelSubsampling = ui32SubsamplingFactor;

    this->setupFlag = true;
    return IMG_SUCCESS;
}
