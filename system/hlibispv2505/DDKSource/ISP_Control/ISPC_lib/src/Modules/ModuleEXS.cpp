/**
*******************************************************************************
 @file ModuleEXS.cpp

 @brief Implementation of ISPC::ModuleEXS

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
#include "ispc/ModuleEXS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_EXS"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleEXS::EXS_GLOBAL(
    "EXS_GLOBAL_ENABLE", false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleEXS::EXS_REGIONAL(
    "EXS_REGIONAL_ENABLE", false);

static const int EXS_GRIDSTART_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleEXS::EXS_GRIDSTART(
    "EXS_GRID_START_COORDS", 0, 8192, EXS_GRIDSTART_DEF, 2);

static const int EXS_GRIDTILE_DEF[2] = { 9, 9};
const ISPC::ParamDefArray<int> ISPC::ModuleEXS::EXS_GRIDTILE(
    "EXS_GRID_TILE_DIMENSIONS", 8, 8191, EXS_GRIDTILE_DEF, 2);

const ISPC::ParamDef<int> ISPC::ModuleEXS::EXS_PIXELMAX("EXS_PIXEL_MAX",
    0, 4095, 4095);

ISPC::ParameterGroup ISPC::ModuleEXS::getGroup()
{
    ParameterGroup group;

    group.header = "// Exposure Statistics parameters";

    group.parameters.insert(EXS_GLOBAL.name);
    group.parameters.insert(EXS_REGIONAL.name);
    group.parameters.insert(EXS_GRIDSTART.name);
    group.parameters.insert(EXS_GRIDTILE.name);
    group.parameters.insert(EXS_PIXELMAX.name);

    return group;
}

ISPC::ModuleEXS::ModuleEXS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleEXS::load(const ParameterList &parameters)
{
    int i;

    bEnableGlobal = parameters.getParameter(EXS_GLOBAL);
    bEnableRegion = parameters.getParameter(EXS_REGIONAL);

    for ( i = 0 ; i < 2 ; i++ )
    {
        aGridStart[i] = parameters.getParameter(EXS_GRIDSTART, i);
    }

    // : Fix tile dimensions width and height (are inverted now)
    for ( i = 0 ; i < 2 ; i++ )
    {
        aGridTileSize[i] = parameters.getParameter(EXS_GRIDTILE, i);
    }

    i32PixelMax = parameters.getParameter(EXS_PIXELMAX);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleEXS::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleEXS::getGroup();
    }

    parameters.addGroup("ModuleEXS", group);

    switch (t)
    {
        case SAVE_VAL:
            parameters.addParameter(EXS_GLOBAL, this->bEnableGlobal);
            parameters.addParameter(EXS_REGIONAL, this->bEnableRegion);

            values.clear();
            for (i = 0 ; i < 2 ; i++)
            {
                values.push_back(toString(this->aGridStart[i]));
            }
            parameters.addParameter(EXS_GRIDSTART, values);

            values.clear();
            for (i = 0 ; i < 2 ; i++)
            {
                values.push_back(toString(this->aGridTileSize[i]));
            }
            parameters.addParameter(EXS_GRIDTILE, values);
            parameters.addParameter(EXS_PIXELMAX, this->i32PixelMax);
            break;

        case SAVE_MIN:
            parameters.addParameterMin(EXS_GLOBAL);  // bool do not have a min
            parameters.addParameterMin(EXS_REGIONAL);  // bool do not have a min
            parameters.addParameterMin(EXS_GRIDSTART);
            parameters.addParameterMin(EXS_GRIDTILE);
            parameters.addParameterMin(EXS_PIXELMAX);
            break;

        case SAVE_MAX:
            parameters.addParameterMax(EXS_GLOBAL);  // bool do not have a max
            parameters.addParameterMax(EXS_REGIONAL);  // bool do not have a max
            parameters.addParameterMax(EXS_GRIDSTART);
            parameters.addParameterMax(EXS_GRIDTILE);
            parameters.addParameterMax(EXS_PIXELMAX);
            break;

        case SAVE_DEF:
            parameters.addParameterDef(EXS_GLOBAL);
            parameters.addParameterDef(EXS_REGIONAL);
            parameters.addParameterDef(EXS_GRIDSTART);
            parameters.addParameterDef(EXS_GRIDTILE);
            parameters.addParameterDef(EXS_PIXELMAX);
            break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleEXS::setup()
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

    pMCPipeline->sEXS.bGlobalEnable = bEnableGlobal ? IMG_TRUE : IMG_FALSE;
    pMCPipeline->sEXS.bRegionEnable = bEnableRegion ? IMG_TRUE : IMG_FALSE;
    pMCPipeline->sEXS.ui16Left = aGridStart[0];
    pMCPipeline->sEXS.ui16Top = aGridStart[1];
    pMCPipeline->sEXS.ui16Width = aGridTileSize[0];
    pMCPipeline->sEXS.ui16Height = aGridTileSize[1];
    pMCPipeline->sEXS.fPixelMax = i32PixelMax;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
