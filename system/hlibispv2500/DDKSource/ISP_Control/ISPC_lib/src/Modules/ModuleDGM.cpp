/**
*******************************************************************************
 @file ModuleDGM.cpp

 @brief Implementation of ISPC::ModuleDGM

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
#include "ispc/ModuleDGM.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_DGM"

#include <string>
#include <vector>

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

static const double DGM_COEFF_DEF[MGM_N_COEFF] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
IMG_STATIC_ASSERT(MGM_N_COEFF == 6, DGM_N_COEFF_CHANGED);
const ISPC::ParamDefArray<double> ISPC::ModuleDGM::DGM_COEFF("DGM_COEFF",
    -2.0, 2.0, DGM_COEFF_DEF, MGM_N_COEFF);

static const double DGM_SLOPE_DEF[MGM_N_SLOPE] = { 1.0, 1.0, 1.0 };
IMG_STATIC_ASSERT(MGM_N_SLOPE == 3, DGM_N_SLOPE_CHANGED);
const ISPC::ParamDefArray<double> ISPC::ModuleDGM::DGM_SLOPE("DGM_SLOPE",
    0.0, 2.0, DGM_SLOPE_DEF, MGM_N_SLOPE);

const ISPC::ParamDef<double> ISPC::ModuleDGM::DGM_SRC_NORM("DGM_SRC_NORM",
    0.0, 2.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleDGM::DGM_CLIP_MIN("DGM_CLIP_MIN",
    -1.0, 0.0, -0.5);
const ISPC::ParamDef<double> ISPC::ModuleDGM::DGM_CLIP_MAX("DGM_CLIP_MAX",
    0.0, 2.0, 1.5);

ISPC::ParameterGroup ISPC::ModuleDGM::getGroup()
{
    ParameterGroup group;

    group.header = "// Display Gamut Mapper parameters";

    group.parameters.insert(DGM_COEFF.name);
    group.parameters.insert(DGM_CLIP_MIN.name);
    group.parameters.insert(DGM_SRC_NORM.name);
    group.parameters.insert(DGM_CLIP_MAX.name);
    group.parameters.insert(DGM_SLOPE.name);

    return group;
}

ISPC::ModuleDGM::ModuleDGM(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleDGM::load(const ParameterList &parameters)
{
    int i;
    for (i = 0 ; i < MGM_N_COEFF; i++)
    {
        aCoeff[i] = parameters.getParameter(DGM_COEFF, i);
    }

    fClipMin = parameters.getParameter(DGM_CLIP_MIN);
    fSrcNorm = parameters.getParameter(DGM_SRC_NORM);
    fClipMax = parameters.getParameter(DGM_CLIP_MAX);

    for (i = 0; i < MGM_N_SLOPE; i++)
    {
        aSlope[i] = parameters.getParameter(DGM_SLOPE, i);
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDGM::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleDGM::getGroup();
    }

    parameters.addGroup("ModuleDGM", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(this->aCoeff[i]));
        }
        parameters.addParameter(Parameter(DGM_COEFF.name, values));

        parameters.addParameter(Parameter(DGM_CLIP_MIN.name,
            toString(this->fClipMin)));
        parameters.addParameter(Parameter(DGM_SRC_NORM.name,
            toString(this->fSrcNorm)));
        parameters.addParameter(Parameter(DGM_CLIP_MAX.name,
            toString(this->fClipMax)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE; i++)
        {
            values.push_back(toString(this->aSlope[i]));
        }
        parameters.addParameter(Parameter(DGM_SLOPE.name, values));
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(DGM_COEFF.min));
        }
        parameters.addParameter(Parameter(DGM_COEFF.name, values));

        parameters.addParameter(Parameter(DGM_CLIP_MIN.name,
            toString(DGM_CLIP_MIN.min)));
        parameters.addParameter(Parameter(DGM_SRC_NORM.name,
            toString(DGM_SRC_NORM.min)));
        parameters.addParameter(Parameter(DGM_CLIP_MAX.name,
            toString(DGM_CLIP_MAX.min)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(DGM_SLOPE.min));
        }
        parameters.addParameter(Parameter(DGM_SLOPE.name, values));
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(DGM_COEFF.max));
        }
        parameters.addParameter(Parameter(DGM_COEFF.name, values));

        parameters.addParameter(Parameter(DGM_CLIP_MIN.name,
            toString(DGM_CLIP_MIN.max)));
        parameters.addParameter(Parameter(DGM_SRC_NORM.name,
            toString(DGM_SRC_NORM.max)));
        parameters.addParameter(Parameter(DGM_CLIP_MAX.name,
            toString(DGM_CLIP_MAX.max)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(DGM_SLOPE.max));
        }
        parameters.addParameter(Parameter(DGM_SLOPE.name, values));
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            values.push_back(toString(DGM_COEFF.def[i]));
        }
        values.push_back("// " + getParameterInfo(DGM_COEFF));
        parameters.addParameter(Parameter(DGM_COEFF.name, values));

        parameters.addParameter(Parameter(DGM_CLIP_MIN.name,
            toString(DGM_CLIP_MIN.def)
            + " // " + getParameterInfo(DGM_CLIP_MIN)));
        parameters.addParameter(Parameter(DGM_SRC_NORM.name,
            toString(DGM_SRC_NORM.def)
            + " // " + getParameterInfo(DGM_SRC_NORM)));
        parameters.addParameter(Parameter(DGM_CLIP_MAX.name,
            toString(DGM_CLIP_MAX.def)
            + " // " + getParameterInfo(DGM_CLIP_MAX)));

        values.clear();
        for (i = 0 ; i < MGM_N_SLOPE ; i++)
        {
            values.push_back(toString(DGM_SLOPE.def[i]));
        }
        values.push_back("// " + getParameterInfo(DGM_SLOPE));
        parameters.addParameter(Parameter(DGM_SLOPE.name, values));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDGM::setup()
{
    MC_PIPELINE *pMCPipeline = NULL;
    const ModuleOUT *pOut = NULL;
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
    pOut = pipeline->getModule<const ModuleOUT>();
    if (!pOut)
    {
        MOD_LOG_ERROR("pipeline has no output module\n");
        return IMG_ERROR_FATAL;
    }

    configure(pMCPipeline->sDGM);

    if (BGR_888_24 == pOut->displayType
       || BGR_888_32 == pOut->displayType
       || BGR_101010_32 == pOut->displayType)
    {
        /* coeefs are: 0=green to red, 1=blue to red, 2=blue to green,
         * 3=red to green, 4=red to blue, 5=green to blue */
        // so we need to inverse them
        unsigned int i;
        for (i = 0 ; i < MGM_N_COEFF ; i++)
        {
            pMCPipeline->sDGM.aCoeff[i] = aCoeff[MGM_N_COEFF-1-i];
        }
    }

    this->setupFlag = true;
    return IMG_SUCCESS;
}
