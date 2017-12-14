/**
*******************************************************************************
 @file ModuleRLT.cpp

 @brief Implementation of ISPC::ModuleRLT

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
#include "ispc/ModuleRLT.h"

#include <mc/module_config.h>
#include <ctx_reg_precisions.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_RLT"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#define RLT_DISABLE_STR "DISABLED"
#define RLT_LINEAR_STR "LINEAR"
#define RLT_CUBIC_STR "CUBIC"

#define RLT_POINTS_MAX (1 << \
    (RLT_LUT_0_POINTS_ODD_1_TO_15_INT + RLT_LUT_0_POINTS_ODD_1_TO_15_FRAC))
static const unsigned int RLT_POINTS_DEF[RLT_SLICE_N_POINTS] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

const ISPC::ParamDefSingle<std::string> ISPC::ModuleRLT::RLT_CORRECTION_MODE(
    "RLT_CORRECTION_MODE", RLT_DISABLE_STR);
const ISPC::ParamDefArray<unsigned int> ISPC::ModuleRLT::RLT_POINTS_S(
    "RLT_POINTS", 0, RLT_POINTS_MAX, RLT_POINTS_DEF, RLT_SLICE_N_POINTS);

ISPC::ParameterGroup ISPC::ModuleRLT::getGroup()
{
    ParameterGroup group;
    std::ostringstream paramname;
    int s;

    group.header = "// Raw Look Up Table parameters";

    group.parameters.insert(RLT_CORRECTION_MODE.name);

    for (s = 0; s < RLT_SLICE_N; s++)
    {
        paramname.str("");
        paramname << RLT_POINTS_S.name << "_" << s;
        group.parameters.insert(paramname.str());
    }

    return group;
}

enum ISPC::ModuleRLT::Mode ISPC::ModuleRLT::getRLTMode(
    const std::string &stringFormat)
{
    if (0 == stringFormat.compare(RLT_LINEAR_STR))
    {
        return RLT_LINEAR;
    }
    else if (0 == stringFormat.compare(RLT_CUBIC_STR))
    {
        return RLT_CUBIC;
    }
    else if (0 == stringFormat.compare(RLT_DISABLE_STR))
    {
        return RLT_DISABLED;
    }
    // MOD_LOG_WARNING("Invalid RLT mode: %s\n", stringFormat.c_str());
    /* no need to print a warning (other will print a warning for
     * empty strings) */
    return RLT_DISABLED;
}

std::string ISPC::ModuleRLT::getRLTModeString(enum ISPC::ModuleRLT::Mode mode)
{
    switch (mode)
    {
    case RLT_LINEAR:
        return RLT_LINEAR_STR;
    case RLT_CUBIC:
        return RLT_CUBIC_STR;
    case RLT_DISABLED:
    default:
        return RLT_DISABLE_STR;
    }
}

ISPC::ModuleRLT::ModuleRLT(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleRLT::load(const ParameterList &parameters)
{
    std::ostringstream paramname;
    int i, s;

    for (s = 0 ; s < RLT_SLICE_N ; s++)
    {
        paramname.str("");
        paramname << RLT_POINTS_S.name << "_" << s;
        for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
        {
            aLUTPoints[s][i] = parameters.getParameter<unsigned int>(
                paramname.str(), i,
                RLT_POINTS_S.min, RLT_POINTS_S.max, RLT_POINTS_S.def[i]);
        }
    }

    eMode = getRLTMode(parameters.getParameter(RLT_CORRECTION_MODE));

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleRLT::save(ParameterList &parameters, SaveType t) const
{
    std::ostringstream paramname;
    int i, s;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleRLT::getGroup();
    }

    parameters.addGroup("ModuleRLT", group);

    switch (t)
    {
    case SAVE_VAL:
        for (s = 0 ; s < RLT_SLICE_N ; s++)
        {
            values.clear();
            paramname.str("");
            paramname << RLT_POINTS_S.name << "_" << s;
            for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
            {
                values.push_back(toString(aLUTPoints[s][i]));
            }
            parameters.addParameter(Parameter(paramname.str(), values));
        }
        parameters.addParameter(Parameter(RLT_CORRECTION_MODE.name,
            getRLTModeString(eMode)));
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
        {
            values.push_back(toString(RLT_POINTS_S.min));
        }
        for (s = 0 ; s < RLT_SLICE_N ; s++)
        {
            paramname.str("");
            paramname << RLT_POINTS_S.name << "_" << s;

            parameters.addParameter(Parameter(paramname.str(), values));
        }
        parameters.addParameter(Parameter(RLT_CORRECTION_MODE.name,
            RLT_CORRECTION_MODE.def));  // enum do not have min
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
        {
            values.push_back(toString(RLT_POINTS_S.max));
        }
        for (s = 0 ; s < RLT_SLICE_N ; s++)
        {
            paramname.str("");
            paramname << RLT_POINTS_S.name << "_" << s;

            parameters.addParameter(Parameter(paramname.str(), values));
        }
        parameters.addParameter(Parameter(RLT_CORRECTION_MODE.name,
            RLT_CORRECTION_MODE.def));  // enum do not have max
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
        {
            values.push_back(toString(RLT_POINTS_S.def[i]));
        }
        values.push_back("// " + getParameterInfo(RLT_POINTS_S));
        for (s = 0 ; s < RLT_SLICE_N ; s++)
        {
            paramname.str("");
            paramname << RLT_POINTS_S.name << "_" << s;

            parameters.addParameter(Parameter(paramname.str(), values));
        }

        std::ostringstream defComment;

        defComment.str("");
        defComment << "// {"
            << RLT_DISABLE_STR
            << ", " << RLT_LINEAR_STR
            << ", " << RLT_CUBIC_STR
            << "}";

        values.clear();
        values.push_back(RLT_CORRECTION_MODE.def);
        values.push_back(defComment.str());

        parameters.addParameter(Parameter(RLT_CORRECTION_MODE.name, values));
        break;
    }

    return IMG_SUCCESS;
}

// need to update module if that changes
IMG_STATIC_ASSERT(RLT_SLICE_N == 4, rltNumSliceChanged);
IMG_STATIC_ASSERT(static_cast<int>(ISPC::ModuleRLT::RLT_DISABLED)
    == static_cast<int>(CI_RLT_DISABLED), rltDisabledChanged);
IMG_STATIC_ASSERT(static_cast<int>(ISPC::ModuleRLT::RLT_LINEAR)
    == static_cast<int>(CI_RLT_LINEAR), rltLinearChanged);
IMG_STATIC_ASSERT(static_cast<int>(ISPC::ModuleRLT::RLT_CUBIC)
    == static_cast<int>(CI_RLT_CUBIC), rltCubicChanged);

IMG_RESULT ISPC::ModuleRLT::setup()
{
    MC_PIPELINE *pMCPipeline = NULL;
    int i, s;
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

    switch (eMode)
    {
    case RLT_DISABLED:
    case RLT_LINEAR:
    case RLT_CUBIC:
        pMCPipeline->sRLT.eMode = static_cast<enum  CI_RLT_MODES>(this->eMode);
        break;
    default:
        MOD_LOG_ERROR("unknown RLT mode %d\n", (int)eMode);
        return IMG_ERROR_FATAL;
    }

    for (s = 0 ; s < RLT_SLICE_N ; s++)
    {
        for (i = 0 ; i < RLT_SLICE_N_POINTS ; i++)
        {
            pMCPipeline->sRLT.aPoints[s][i] = aLUTPoints[s][i];
        }
    }

    this->setupFlag = true;
    return IMG_SUCCESS;
}
