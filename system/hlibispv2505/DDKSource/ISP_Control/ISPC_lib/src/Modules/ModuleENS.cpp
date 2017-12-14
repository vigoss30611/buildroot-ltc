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

#include "ispc/PerfTime.h"

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
        parameters.addParameter(ENS_ENABLE, this->bEnable);
        parameters.addParameter(ENS_REGION_NUMLINES, (int)this->ui32NLines);
        parameters.addParameter(ENS_SUBS_FACTOR, (int)this->ui32SubsamplingFactor);
    break;

    case SAVE_MIN:
        parameters.addParameterMin(ENS_ENABLE);  // bool do not have min
        parameters.addParameterMin(ENS_REGION_NUMLINES);
        parameters.addParameterMin(ENS_SUBS_FACTOR);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(ENS_ENABLE);  // bool do not have max
        parameters.addParameterMax(ENS_REGION_NUMLINES);
        parameters.addParameterMax(ENS_SUBS_FACTOR);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(ENS_ENABLE);
        parameters.addParameterDef(ENS_REGION_NUMLINES);
        parameters.addParameterDef(ENS_SUBS_FACTOR);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleENS::setup()
{
    LOG_PERF_IN();
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
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
