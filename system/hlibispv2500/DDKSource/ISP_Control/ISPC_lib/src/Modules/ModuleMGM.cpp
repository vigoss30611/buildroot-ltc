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
        parameters.addParameter(Parameter(MGM_COEFF.name, values));

        parameters.addParameter(Parameter(MGM_CLIP_MIN.name,
            toString(this->fClipMin)));
        parameters.addParameter(Parameter(MGM_SRC_NORM.name,
            toString(this->fSrcNorm)));
        parameters.addParameter(Parameter(MGM_CLIP_MAX.name,
            toString(this->fClipMax)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(this->aSlope[i]));
        }
        parameters.addParameter(Parameter(MGM_SLOPE.name, values));
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(MGM_COEFF.min));
        }
        parameters.addParameter(Parameter(MGM_COEFF.name, values));

        parameters.addParameter(Parameter(MGM_CLIP_MIN.name,
            toString(MGM_CLIP_MIN.min)));
        parameters.addParameter(Parameter(MGM_SRC_NORM.name,
            toString(MGM_SRC_NORM.min)));
        parameters.addParameter(Parameter(MGM_CLIP_MAX.name,
            toString(MGM_CLIP_MAX.min)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(MGM_SLOPE.min));
        }
        parameters.addParameter(Parameter(MGM_SLOPE.name, values));
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(MGM_COEFF.max));
        }
        parameters.addParameter(Parameter(MGM_COEFF.name, values));

        parameters.addParameter(Parameter(MGM_CLIP_MIN.name,
            toString(MGM_CLIP_MIN.max)));
        parameters.addParameter(Parameter(MGM_SRC_NORM.name,
            toString(MGM_SRC_NORM.max)));
        parameters.addParameter(Parameter(MGM_CLIP_MAX.name,
            toString(MGM_CLIP_MAX.max)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(MGM_SLOPE.max));
        }
        parameters.addParameter(Parameter(MGM_SLOPE.name, values));
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(MGM_COEFF.def[i]));
        }
        values.push_back("// " + getParameterInfo(MGM_COEFF));
        parameters.addParameter(Parameter(MGM_COEFF.name, values));

        parameters.addParameter(Parameter(MGM_CLIP_MIN.name,
            toString(MGM_CLIP_MIN.def)
            + " // " + getParameterInfo(MGM_CLIP_MIN)));
        parameters.addParameter(Parameter(MGM_SRC_NORM.name,
            toString(MGM_SRC_NORM.def)
            + " // " + getParameterInfo(MGM_SRC_NORM)));
        parameters.addParameter(Parameter(MGM_CLIP_MAX.name,
            toString(MGM_CLIP_MAX.def)
            + " // " + getParameterInfo(MGM_CLIP_MAX)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(MGM_SLOPE.def[i]));
        }
        values.push_back("// " + getParameterInfo(MGM_SLOPE));
        parameters.addParameter(Parameter(MGM_SLOPE.name, values));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleMGM::setup()
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

    configure(pMCPipeline->sMGM);

    this->setupFlag = true;
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
