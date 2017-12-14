/**
*******************************************************************************
 @file ModuleCCM.cpp

 @brief Implementation of ISPC::ModuleCCM

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
#include "ispc/ModuleCCM.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_CCM"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

const ISPC::ParamDefArray<double> ISPC::ModuleCCM::CCM_MATRIX("CCM_MATRIX",
    CCM_MATRIX_MIN, CCM_MATRIX_MAX, CCM_MATRIX_DEF, 9);

const ISPC::ParamDefArray<double> ISPC::ModuleCCM::CCM_OFFSETS("CCM_OFFSETS",
    CCM_OFFSETS_MIN, CCM_OFFSETS_MAX, CCM_OFFSETS_DEF, 3);

ISPC::ParameterGroup ISPC::ModuleCCM::getGroup()
{
    ParameterGroup group;

    group.header = "// Colour Correction Matrix parameters";

    group.parameters.insert(CCM_MATRIX.name);
    group.parameters.insert(CCM_OFFSETS.name);

    return group;
}

ISPC::ModuleCCM::ModuleCCM(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleCCM::load(const ParameterList &parameters)
{
    int i;
    for (i = 0 ; i < 9 ; i++)
    {
        aMatrix[i] = parameters.getParameter(CCM_MATRIX, i);
    }
    for (i = 0 ; i < 3 ; i++)
    {
        aOffset[i] = parameters.getParameter(CCM_OFFSETS, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleCCM::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleCCM::getGroup();
    }

    parameters.addGroup("ModuleCCM", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < 9 ; i++)
        {
            values.push_back(toString(this->aMatrix[i]));
        }
        parameters.addParameter(Parameter(CCM_MATRIX.name, values));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(this->aOffset[i]));
        }
        parameters.addParameter(Parameter(CCM_OFFSETS.name, values));
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0 ; i < 9 ; i++)
        {
            values.push_back(toString(CCM_MATRIX.min));
        }
        parameters.addParameter(Parameter(CCM_MATRIX.name, values));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(CCM_OFFSETS.min));
        }
        parameters.addParameter(Parameter(CCM_OFFSETS.name, values));
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0 ; i < 9 ; i++)
        {
            values.push_back(toString(CCM_MATRIX.max));
        }
        parameters.addParameter(Parameter(CCM_MATRIX.name, values));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(CCM_OFFSETS.max));
        }
        parameters.addParameter(Parameter(CCM_OFFSETS.name, values));
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0 ; i < 9 ; i++)
        {
            values.push_back(toString(CCM_MATRIX.def[i]));
        }
        // add type as comment
        values.push_back("// " + getParameterInfo(CCM_MATRIX));
        parameters.addParameter(Parameter(CCM_MATRIX.name, values));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(CCM_OFFSETS.def[i]));
        }
        // add type as comment
        values.push_back("// " + getParameterInfo(CCM_OFFSETS));
        parameters.addParameter(Parameter(CCM_OFFSETS.name, values));
        break;
    }

    return IMG_SUCCESS;
}
IMG_RESULT ISPC::ModuleCCM::setup()
{
    IMG_INT32 i, j;
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

    // Using 3x3 matrix
    for (i = 0; i < RGB_COEFF_NO; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMCPipeline->sCCM.aCoeff[i][j] = 0;
        }
    }

    pMCPipeline->sCCM.aCoeff[0][0] = aMatrix[0];
    pMCPipeline->sCCM.aCoeff[0][1] = aMatrix[1];
    pMCPipeline->sCCM.aCoeff[0][2] = aMatrix[2];
    pMCPipeline->sCCM.aCoeff[0][3] = 0;

    pMCPipeline->sCCM.aCoeff[1][0] = aMatrix[3];
    pMCPipeline->sCCM.aCoeff[1][1] = aMatrix[4];
    pMCPipeline->sCCM.aCoeff[1][2] = aMatrix[5];
    pMCPipeline->sCCM.aCoeff[1][3] = 0;

    pMCPipeline->sCCM.aCoeff[2][0] = aMatrix[6];
    pMCPipeline->sCCM.aCoeff[2][1] = aMatrix[7];
    pMCPipeline->sCCM.aCoeff[2][2] = aMatrix[8];
    pMCPipeline->sCCM.aCoeff[2][3] = 0;

    pMCPipeline->sCCM.aOffset[0] = aOffset[0];
    pMCPipeline->sCCM.aOffset[1] = aOffset[1];
    pMCPipeline->sCCM.aOffset[2] = aOffset[2];

    /*MOD_LOG_DEBUG("{{%f %f %f}{%f %f %f}{%f %f %f}} +{%f %f %f}\n",
        aMatrix[0], aMatrix[1], aMatrix[2],
        aMatrix[3], aMatrix[4], aMatrix[5],
        aMatrix[6], aMatrix[7], aMatrix[8],
        aOffset[0], aOffset[1], aOffset[2]
    );*/

    this->setupFlag = true;
    return IMG_SUCCESS;
}

ISPC::ModuleCCM& ISPC::ModuleCCM::operator=(const ModuleCCM &other)
{
    for (unsigned int i = 0; i < ISPC::ModuleCCM::CCM_MATRIX.n; i++)
    {
        aMatrix[i] = other.aMatrix[i];
    }
    for (unsigned int i = 0; i < ISPC::ModuleCCM::CCM_OFFSETS.n; i++)
    {
        aOffset[i] = other.aOffset[i];
    }
    return *this;
}
