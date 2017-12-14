/**
*******************************************************************************
@file ModuleLCA.cpp

@brief Implementation of ISPC::ModuleLCA

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
#include "ispc/ModuleLCA.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_LCA"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

#define LCA_POLY_MIN (-16.0f)
#define LCA_POLY_MAX (15.9f)

static const double LCA_POLY_DEF[LCA_COEFFS_NO] = {0.0, 0.0, 0.0};
IMG_STATIC_ASSERT(LCA_COEFFS_NO == 3, LCA_COEFFS_NO_CHANGED);
const ISPC::ParamDefArray<double> ISPC::ModuleLCA::LCA_REDPOLY_X(
    "LCA_REDPOLY_X", LCA_POLY_MIN, LCA_POLY_MAX, LCA_POLY_DEF, LCA_COEFFS_NO);
const ISPC::ParamDefArray<double> ISPC::ModuleLCA::LCA_REDPOLY_Y(
    "LCA_REDPOLY_Y", LCA_POLY_MIN, LCA_POLY_MAX, LCA_POLY_DEF, LCA_COEFFS_NO);
const ISPC::ParamDefArray<double> ISPC::ModuleLCA::LCA_BLUEPOLY_X(
    "LCA_BLUEPOLY_X", LCA_POLY_MIN, LCA_POLY_MAX, LCA_POLY_DEF, LCA_COEFFS_NO);
const ISPC::ParamDefArray<double> ISPC::ModuleLCA::LCA_BLUEPOLY_Y(
    "LCA_BLUEPOLY_Y", LCA_POLY_MIN, LCA_POLY_MAX, LCA_POLY_DEF, LCA_COEFFS_NO);

#define LCA_CENTER_MIN (-4095)
#define LCA_CENTER_MAX (4095)

static const int LCA_CENTER_DEF[2] = {0 , 0};
const ISPC::ParamDefArray<int> ISPC::ModuleLCA::LCA_REDCENTER(
    "LCA_RED_CENTER", LCA_CENTER_MIN, LCA_CENTER_MAX, LCA_CENTER_DEF, 2);
const ISPC::ParamDefArray<int> ISPC::ModuleLCA::LCA_BLUECENTER(
    "LCA_BLUE_CENTER", LCA_CENTER_MIN, LCA_CENTER_MAX, LCA_CENTER_DEF, 2);

static const int LCA_SHIFT_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleLCA::LCA_SHIFT("LCA_SHIFT",
    0, 3, LCA_SHIFT_DEF, 2);

static const int LCA_DEC_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleLCA::LCA_DEC("LCA_DEC",
    0, 15, LCA_DEC_DEF, 2);

ISPC::ParameterGroup ISPC::ModuleLCA::getGroup()
{
    ParameterGroup group;

    group.header = "// Lateral Chromatic Aberration parameters";

    group.parameters.insert(LCA_REDPOLY_X.name);
    group.parameters.insert(LCA_REDPOLY_Y.name);
    group.parameters.insert(LCA_BLUEPOLY_X.name);
    group.parameters.insert(LCA_BLUEPOLY_Y.name);
    group.parameters.insert(LCA_REDCENTER.name);
    group.parameters.insert(LCA_BLUECENTER.name);
    group.parameters.insert(LCA_SHIFT.name);
    group.parameters.insert(LCA_DEC.name);

    return group;
}

ISPC::ModuleLCA::ModuleLCA(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleLCA::load(const ParameterList &parameters)
{
    int i;

    for (i = 0 ; i < LCA_COEFFS_NO ; i++)
    {
        aRedPoly_X[i] = parameters.getParameter(LCA_REDPOLY_X, i);
    }
    for (i = 0 ; i < LCA_COEFFS_NO ; i++)
    {
        aRedPoly_Y[i] = parameters.getParameter(LCA_REDPOLY_Y, i);
    }

    for (i = 0 ; i < LCA_COEFFS_NO ; i++)
    {
        aBluePoly_X[i] = parameters.getParameter(LCA_BLUEPOLY_X, i);
    }
    for (i = 0 ; i < LCA_COEFFS_NO ; i++)
    {
        aBluePoly_Y[i] = parameters.getParameter(LCA_BLUEPOLY_Y, i);
    }

    for (i = 0 ; i < 2 ; i++)
    {
        aRedCenter[i] = parameters.getParameter(LCA_REDCENTER, i);
    }

    for (i = 0 ; i < 2 ; i++)
    {
        aBlueCenter[i] = parameters.getParameter(LCA_BLUECENTER, i);
    }

    for (i = 0 ; i < 2 ; i++)
    {
        aShift[i] = parameters.getParameter(LCA_SHIFT, i);
    }

    for (i = 0 ; i < 2 ; i++)
    {
        aDecimation[i] = parameters.getParameter(LCA_DEC, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLCA::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleLCA::getGroup();
    }

    parameters.addGroup("ModuleLCA", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < LCA_COEFFS_NO ; i++)
        {
            values.push_back(toString(this->aRedPoly_X[i]));
        }
        parameters.addParameter(LCA_REDPOLY_X, values);

        values.clear();
        for (i = 0 ; i < LCA_COEFFS_NO ; i++)
        {
            values.push_back(toString(this->aRedPoly_Y[i]));
        }
        parameters.addParameter(LCA_REDPOLY_Y, values);

        values.clear();
        for (i = 0 ; i < LCA_COEFFS_NO ; i++)
        {
            values.push_back(toString(this->aBluePoly_X[i]));
        }
        parameters.addParameter(LCA_BLUEPOLY_X, values);

        values.clear();
        for (i = 0 ; i < LCA_COEFFS_NO ; i++)
        {
            values.push_back(toString(this->aBluePoly_Y[i]));
        }
        parameters.addParameter(LCA_BLUEPOLY_Y, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aRedCenter[i]));
        }
        parameters.addParameter(LCA_REDCENTER, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aBlueCenter[i]));
        }
        parameters.addParameter(LCA_BLUECENTER, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aShift[i]));
        }
        parameters.addParameter(LCA_SHIFT, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aDecimation[i]));
        }
        parameters.addParameter(Parameter(LCA_DEC.name, values));
        break;

    case SAVE_MIN:
        parameters.addParameterMin(LCA_REDPOLY_X);
        parameters.addParameterMin(LCA_REDPOLY_Y);
        parameters.addParameterMin(LCA_BLUEPOLY_X);
        parameters.addParameterMin(LCA_BLUEPOLY_Y);
        parameters.addParameterMin(LCA_REDCENTER);
        parameters.addParameterMin(LCA_BLUECENTER);
        parameters.addParameterMin(LCA_SHIFT);
        parameters.addParameterMin(LCA_DEC);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(LCA_REDPOLY_X);
        parameters.addParameterMax(LCA_REDPOLY_Y);
        parameters.addParameterMax(LCA_BLUEPOLY_X);
        parameters.addParameterMax(LCA_BLUEPOLY_Y);
        parameters.addParameterMax(LCA_REDCENTER);
        parameters.addParameterMax(LCA_BLUECENTER);
        parameters.addParameterMax(LCA_SHIFT);
        parameters.addParameterMax(LCA_DEC);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(LCA_REDPOLY_X);
        parameters.addParameterDef(LCA_REDPOLY_Y);
        parameters.addParameterDef(LCA_BLUEPOLY_X);
        parameters.addParameterDef(LCA_BLUEPOLY_Y);
        parameters.addParameterDef(LCA_REDCENTER);
        parameters.addParameterDef(LCA_BLUECENTER);
        parameters.addParameterDef(LCA_SHIFT);
        parameters.addParameterDef(LCA_DEC);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLCA::setup()
{
    LOG_PERF_IN();
    IMG_INT32 i;
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

    for (i = 0; i < LCA_COEFFS_NO; i++)
    {
        pMCPipeline->sLCA.aCoeffRed[i][0] = aRedPoly_X[i];
        pMCPipeline->sLCA.aCoeffRed[i][1] = aRedPoly_Y[i];
        pMCPipeline->sLCA.aCoeffBlue[i][0] = aBluePoly_X[i];
        pMCPipeline->sLCA.aCoeffBlue[i][1] = aBluePoly_Y[i];
    }

    /* The offset is distance in X and Y from the top left corner of the
     * frame to the optical center. */
    pMCPipeline->sLCA.aOffsetRed[0] = aRedCenter[0];  // decimated CFA units
    pMCPipeline->sLCA.aOffsetRed[1] = aRedCenter[1];  // decimated CFA units
    pMCPipeline->sLCA.aOffsetBlue[0] = aBlueCenter[0];  // decimated CFA units
    pMCPipeline->sLCA.aOffsetBlue[1] = aBlueCenter[1];  // decimated CFA units

    pMCPipeline->sLCA.aDec[0] = aDecimation[0];
    pMCPipeline->sLCA.aDec[1] = aDecimation[1];

    pMCPipeline->sLCA.aShift[0] = aShift[0];
    pMCPipeline->sLCA.aShift[1] = aShift[1];

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
