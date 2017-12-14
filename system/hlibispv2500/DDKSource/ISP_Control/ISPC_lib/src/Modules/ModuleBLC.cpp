/**
*******************************************************************************
 @file ModuleBLC.cpp

 @brief Implementation of ISPC::ModuleBLC

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
#include "ispc/ModuleBLC.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_BLC"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

static const int BLC_SENSOR_BLACK_DEF[4] = { 127, 127, 127, 127 };
const ISPC::ParamDefArray<int> ISPC::ModuleBLC::BLC_SENSOR_BLACK(
    "BLC_SENSOR_BLACK", -128, 127, BLC_SENSOR_BLACK_DEF, 4);
const ISPC::ParamDef<int> ISPC::ModuleBLC::BLC_SYS_BLACK(
    "BLC_SYS_BLACK", 0, 32767, 64);

ISPC::ParameterGroup ISPC::ModuleBLC::getGroup()
{
    ParameterGroup group;

    group.header = "// Black Level Correction parameters";

    group.parameters.insert(BLC_SENSOR_BLACK.name);
    group.parameters.insert(BLC_SYS_BLACK.name);

    return group;
}

ISPC::ModuleBLC::ModuleBLC(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleBLC::load(const ParameterList &parameters)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        aSensorBlack[i] = parameters.getParameter(BLC_SENSOR_BLACK, i);
    }
    ui32SystemBlack = parameters.getParameter(BLC_SYS_BLACK);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleBLC::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if ( group.parameters.size() == 0 )
    {
        group = ModuleBLC::getGroup();
    }

    parameters.addGroup("ModuleBLC", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for ( i = 0 ; i < 4 ; i++ )
        {
            values.push_back(toString(this->aSensorBlack[i]));
        }
        parameters.addParameter(Parameter(BLC_SENSOR_BLACK.name, values));
        parameters.addParameter(Parameter(BLC_SYS_BLACK.name,
            toString(this->ui32SystemBlack)));
        break;

    case SAVE_MIN:
        values.clear();
        for ( i = 0 ; i < 4 ; i++ )
        {
            values.push_back(toString(BLC_SENSOR_BLACK.min));
        }
        parameters.addParameter(Parameter(BLC_SENSOR_BLACK.name, values));
        parameters.addParameter(Parameter(BLC_SYS_BLACK.name,
            toString(BLC_SYS_BLACK.min)));
        break;

    case SAVE_MAX:
        values.clear();
        for ( i = 0 ; i < 4 ; i++ )
        {
            values.push_back(toString(BLC_SENSOR_BLACK.max));
        }
        parameters.addParameter(Parameter(BLC_SENSOR_BLACK.name, values));
        parameters.addParameter(Parameter(BLC_SYS_BLACK.name,
            toString(BLC_SYS_BLACK.max)));
        break;

    case SAVE_DEF:
        values.clear();
        for ( i = 0 ; i < 4 ; i++ )
        {
            values.push_back(toString(BLC_SENSOR_BLACK.def[i]));
        }
        // add type as comment
        values.push_back("// " + getParameterInfo(BLC_SENSOR_BLACK));
        parameters.addParameter(Parameter(BLC_SENSOR_BLACK.name, values));
        parameters.addParameter(Parameter(BLC_SYS_BLACK.name,
            toString(BLC_SYS_BLACK.def)
            + " // " + getParameterInfo(BLC_SYS_BLACK)));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleBLC::setup()
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

    // speciefies if the current is a black frame. By default == false
    pMCPipeline->sBLC.bBlackFrame = IMG_FALSE;  // DEFAULT VALUE

    /* bRowAverage corresponds to black mode. if
     * MODE ==  ROW AVERAGE --> bRowAverage = true and the black levels
     * are averaged from the OBD */
    /* if MODE = FIXED then we set bRowAverage to false. Implies that the
     * Black level are fixed values */
    pMCPipeline->sBLC.bRowAverage = IMG_FALSE;  // By default

    if (pMCPipeline->sBLC.bRowAverage)  // MODE == ROW AVERAGE
    {
        // By default we use the maximum (defined as a 6.2 value -> max = 64)
        pMCPipeline->sBLC.ui16PixelMax = 64.0;
        pMCPipeline->sBLC.aFixedOffset[0] = 0;
        pMCPipeline->sBLC.aFixedOffset[1] = 0;
        pMCPipeline->sBLC.aFixedOffset[2] = 0;
        pMCPipeline->sBLC.aFixedOffset[3] = 0;
    }
    else  // MODE == FIXED
    {
        pMCPipeline->sBLC.ui16PixelMax = 64.0;
        pMCPipeline->sBLC.aFixedOffset[0] = -1 * (aSensorBlack[0]);
        pMCPipeline->sBLC.aFixedOffset[1] = -1 * (aSensorBlack[1]);
        pMCPipeline->sBLC.aFixedOffset[2] = -1 * (aSensorBlack[2]);
        pMCPipeline->sBLC.aFixedOffset[3] = -1 * (aSensorBlack[3]);
    }

    pMCPipeline->ui16SystemBlackLevel = ui32SystemBlack;

    this->setupFlag = true;
    return IMG_SUCCESS;
}
