/**
*******************************************************************************
@file ModuleMGM.cpp

@brief Implementation of ISPC::ModuleMGM

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
#include "ispc/ModuleMGM.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_MGM"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

static const double MGM_COEFF_DEF[MGM_N_COEFF] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
IMG_STATIC_ASSERT(MGM_N_COEFF == 6, MGM_N_COEFF_CHANGED);
const ISPC::ParamDefArray<double> ISPC::ModuleMGM::MGM_COEFF("MGM_COEFF",
    -2.0, 2.0, MGM_COEFF_DEF, MGM_N_COEFF);

static const double MGM_SLOPE_DEF[MGM_N_SLOPE] = { 1.0, 1.0, 1.0 };
IMG_STATIC_ASSERT(MGM_N_SLOPE == 3, MGM_N_SLOPE_CHANGED);
const ISPC::ParamDefArray<double> ISPC::ModuleMGM::MGM_SLOPE("MGM_SLOPE",
    0.0, 2.0, MGM_SLOPE_DEF, MGM_N_SLOPE);

const ISPC::ParamDef<double> ISPC::ModuleMGM::MGM_SRC_NORM("MGM_SRC_NORM",
    0.0, 2.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleMGM::MGM_CLIP_MIN("MGM_CLIP_MIN",
    -1.0, 0.0, -0.5);
const ISPC::ParamDef<double> ISPC::ModuleMGM::MGM_CLIP_MAX("MGM_CLIP_MAX",
    0.0, 2.0, 1.5);

ISPC::ParameterGroup ISPC::ModuleMGM::getGroup()
{
    ParameterGroup group;

    group.header = "// Main Gamut Mapper parameters";

    group.parameters.insert(MGM_COEFF.name);
    group.parameters.insert(MGM_CLIP_MIN.name);
    group.parameters.insert(MGM_SRC_NORM.name);
    group.parameters.insert(MGM_CLIP_MAX.name);
    group.parameters.insert(MGM_SLOPE.name);

    return group;
}

ISPC::ModuleMGM::ModuleMGM(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleMGM::load(const ParameterList &parameters)
{
    int i;
    for (i = 0 ; i < MGM_N_COEFF ; i++)
    {
        aCoeff[i] = parameters.getParameter(MGM_COEFF, i);
    }

    fClipMin = parameters.getParameter(MGM_CLIP_MIN);
    fSrcNorm = parameters.getParameter(MGM_SRC_NORM);
    fClipMax = parameters.getParameter(MGM_CLIP_MAX);

    for (i = 0 ; i < MGM_N_SLOPE ; i++)
    {
        aSlope[i] = parameters.getParameter(MGM_SLOPE, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleMGM::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleMGM::getGroup();
    }

    parameters.addGroup("ModuleMGM", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(this->aCoeff[i]));
        }
        parameters.addParameter(MGM_COEFF, values);

        parameters.addParameter(MGM_CLIP_MIN, this->fClipMin);
        parameters.addParameter(MGM_SRC_NORM, this->fSrcNorm);
        parameters.addParameter(MGM_CLIP_MAX, this->fClipMax);

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(this->aSlope[i]));
        }
        parameters.addParameter(MGM_SLOPE, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(MGM_COEFF);
        parameters.addParameterMin(MGM_CLIP_MIN);
        parameters.addParameterMin(MGM_SRC_NORM);
        parameters.addParameterMin(MGM_CLIP_MAX);
        parameters.addParameterMin(MGM_SLOPE);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(MGM_COEFF);
        parameters.addParameterMax(MGM_CLIP_MIN);
        parameters.addParameterMax(MGM_SRC_NORM);
        parameters.addParameterMax(MGM_CLIP_MAX);
        parameters.addParameterMax(MGM_SLOPE);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(MGM_COEFF);
        parameters.addParameterDef(MGM_CLIP_MIN);
        parameters.addParameterDef(MGM_SRC_NORM);
        parameters.addParameterDef(MGM_CLIP_MAX);
        parameters.addParameterDef(MGM_SLOPE);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleMGM::setup()
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

    configure(pMCPipeline->sMGM);

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

//
// ModuleGMBase
//

IMG_RESULT ISPC::ModuleGMBase::configure(MC_GAMUTMAPPER &gamut)
{
    unsigned int i;
    for (i = 0 ; i < MGM_N_COEFF ; i++)
    {
        gamut.aCoeff[i] = aCoeff[i];
    }

    gamut.fClipMin  = fClipMin;
    gamut.fSrcNorm  = fSrcNorm;
    gamut.fClipMax  = fClipMax;

    for (i = 0 ; i < MGM_N_SLOPE ; i++)
    {
        gamut.aSlope[i] = aSlope[i];
    }
    return IMG_SUCCESS;
}
