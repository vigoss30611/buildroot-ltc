/**
*******************************************************************************
@file ModuleHIS.cpp

@brief Implementation of ISPC::ModuleHIS

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
#include "ispc/ModuleHIS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_HIS"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleHIS::HIS_GLOBAL(
    "HIS_GLOBAL_ENABLE", false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleHIS::HIS_GRID(
    "HIS_REGIONAL_ENABLE", false);
const ISPC::ParamDef<int> ISPC::ModuleHIS::HIS_INPUTOFF("HIS_INPUT_OFFSET",
    0, 1023, 256);
const ISPC::ParamDef<int> ISPC::ModuleHIS::HIS_INPUTSCALE("HIS_INPUT_SCALE",
    0, 65535, 32767);

static const int HIS_GRIDSTART_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleHIS::HIS_GRIDSTART(
    "HIS_GRID_START_COORDS", 0, 8191, HIS_GRIDSTART_DEF, 2);

static const int HIS_GRIDSIZE_DEF[2] =
    { HIS_MIN_TILE_WIDTH, HIS_MIN_TILE_HEIGHT};
const ISPC::ParamDefArray<int> ISPC::ModuleHIS::HIS_GRIDSIZE(
    "HIS_GRID_TILE_DIMENSIONS", 8, 4095, HIS_GRIDSIZE_DEF, 2);

ISPC::ParameterGroup ISPC::ModuleHIS::getGroup()
{
    ParameterGroup group;

    group.header = "// Histogram Statistics parameters";

    group.parameters.insert(HIS_GLOBAL.name);
    group.parameters.insert(HIS_GRID.name);
    group.parameters.insert(HIS_INPUTOFF.name);
    group.parameters.insert(HIS_INPUTSCALE.name);
    group.parameters.insert(HIS_GRIDSTART.name);
    group.parameters.insert(HIS_GRIDSIZE.name);

    return group;
}

ISPC::ModuleHIS::ModuleHIS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleHIS::load(const ParameterList &parameters)
{
    int i;

    bEnableGlobal = parameters.getParameter(HIS_GLOBAL);
    bEnableROI = parameters.getParameter(HIS_GRID);
    ui32InputOffset = parameters.getParameter(HIS_INPUTOFF);
    ui32InputScale = parameters.getParameter(HIS_INPUTSCALE);

    for (i = 0 ; i < 2 ; i++)
    {
        aGridStartCoord[i] = parameters.getParameter(HIS_GRIDSTART, i);
    }
    for (i = 0 ; i < 2 ; i++)
    {
        aGridTileSize[i] = parameters.getParameter(HIS_GRIDSIZE, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleHIS::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleHIS::getGroup();
    }

    parameters.addGroup("ModuleHIS", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(HIS_GLOBAL, this->bEnableGlobal);
        parameters.addParameter(HIS_GRID, this->bEnableROI);
        parameters.addParameter(HIS_INPUTOFF, (int)this->ui32InputOffset);
        parameters.addParameter(HIS_INPUTSCALE, (int)this->ui32InputScale);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aGridStartCoord[i]));
        }
        parameters.addParameter(HIS_GRIDSTART, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aGridTileSize[i]));
        }
        parameters.addParameter(HIS_GRIDSIZE, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(HIS_GLOBAL);  // bool does not have min
        parameters.addParameterMin(HIS_GRID);  // bool does not have min
        parameters.addParameterMin(HIS_INPUTOFF);
        parameters.addParameterMin(HIS_INPUTSCALE);
        parameters.addParameterMin(HIS_GRIDSTART);
        parameters.addParameterMin(HIS_GRIDSIZE);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(HIS_GLOBAL);  // bool does not have max
        parameters.addParameterMax(HIS_GRID);  // bool does not have max
        parameters.addParameterMax(HIS_INPUTOFF);
        parameters.addParameterMax(HIS_INPUTSCALE);
        parameters.addParameterMax(HIS_GRIDSTART);
        parameters.addParameterMax(HIS_GRIDSIZE);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(HIS_GLOBAL);
        parameters.addParameterDef(HIS_GRID);
        parameters.addParameterDef(HIS_INPUTOFF);
        parameters.addParameterDef(HIS_INPUTSCALE);
        parameters.addParameterDef(HIS_GRIDSTART);
        parameters.addParameterDef(HIS_GRIDSIZE);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleHIS::setup()
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

    pMCPipeline->sHIS.bGlobalEnable = bEnableGlobal;
    pMCPipeline->sHIS.bRegionEnable = bEnableROI;

    pMCPipeline->sHIS.fInputOffset = ui32InputOffset;
    pMCPipeline->sHIS.fInputScale = ui32InputScale;
    pMCPipeline->sHIS.ui16Left = aGridStartCoord[0];
    pMCPipeline->sHIS.ui16Top = aGridStartCoord[1];
    pMCPipeline->sHIS.ui16Width = aGridTileSize[0];
    pMCPipeline->sHIS.ui16Height = aGridTileSize[1];

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
