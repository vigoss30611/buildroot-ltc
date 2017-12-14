/**
*******************************************************************************
@file ModuleGMA.cpp

@brief Implementation of ISPC::ModuleGMA

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
#include "ispc/ModuleGMA.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_GMA"

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleGMA::GMA_BYPASS("GMA_BYPASS",
    false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleGMA::USE_CUSTOM_GAM("USE_CUSTOM_GAM", false);
const IMG_UINT16 CUSTOM_GAM_CURVE_DEF[63] =
{
	0, 173, 269, 307, 340, 370, 397, 447, 491, 531,
	568, 634, 693, 747, 796, 841, 884, 925, 963, 999,
	1034, 1068, 1100, 1161, 1218, 1272, 1323, 1372, 1418, 1463,
	1506, 1547, 1587, 1626, 1664, 1700, 1736, 1770, 1804, 1837,
	1869, 1900, 1931, 1961, 1991, 2020, 2048, 2076, 2103, 2130,
	2157, 2183, 2208, 2234, 2259, 2283, 2307, 2331, 2355, 2378,
	2401, 2423, 2446
};

const ISPC::ParamDefArray<IMG_UINT16> ISPC::ModuleGMA::CUSTOM_GAM_CURVE("CUSTOM_GAM_CURVE", 0,  std::numeric_limits<IMG_UINT16>::max(), CUSTOM_GAM_CURVE_DEF, 63);
ISPC::ParameterGroup ISPC::ModuleGMA::getGroup()
{
    ParameterGroup group;

    group.header = "// Gamma Look Up Table parameters";

    group.parameters.insert(GMA_BYPASS.name);
    group.parameters.insert(USE_CUSTOM_GAM.name);

   	group.parameters.insert(CUSTOM_GAM_CURVE.name);

    return group;
}

ISPC::ModuleGMA::ModuleGMA(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleGMA::load(const ParameterList &parameters)
{
    bBypass = parameters.getParameter(GMA_BYPASS);
    this->useCustomGam = parameters.getParameter(USE_CUSTOM_GAM);

    for (int i = 0; i < 63; i++)
	{
		this->customGamCurve[i] = parameters.getParameter(CUSTOM_GAM_CURVE, i);
	}

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleGMA::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;
    std::vector<std::string> values;

    if (0 == group.parameters.size())
    {
        group = ModuleGMA::getGroup();
    }

    parameters.addGroup("ModuleGMA", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(GMA_BYPASS, this->bBypass);
        parameters.addParameter(USE_CUSTOM_GAM, this->useCustomGam);

        values.clear();
        for (int i = 0; i < 63; i++)
        {
            values.push_back(toString(this->customGamCurve[i]));
        }
        parameters.addParameter(CUSTOM_GAM_CURVE, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(GMA_BYPASS);
        break;
    case SAVE_MAX:
        parameters.addParameterMax(GMA_BYPASS);
        break;
    case SAVE_DEF:
        parameters.addParameterDef(GMA_BYPASS);
        break;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleGMA::setup()
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

    pMCPipeline->sGMA.bBypass = bBypass ? IMG_TRUE : IMG_FALSE;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
