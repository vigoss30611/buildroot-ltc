/**
*******************************************************************************
@file ModuleFOS.cpp

@brief Implementation of ISPC::ModuleFOS

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
#include "ispc/ModuleFOS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_FOS"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"


const ISPC::ParamDefSingle<bool> ISPC::ModuleFOS::FOS_ROI("FOS_ROI_ENABLE",
    false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleFOS::FOS_GRID("FOS_GRID_ENABLE",
    false);

static const int FOS_ROISTART_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleFOS::FOS_ROISTART(
    "FOS_ROI_START_COORDS", 0, 32767, FOS_ROISTART_DEF, 2);

static const int FOS_ROIEND_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleFOS::FOS_ROIEND(
    "FOS_ROI_END_COORDS", 0, 32767, FOS_ROIEND_DEF, 2);

static const int FOS_GRIDSTART_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleFOS::FOS_GRIDSTART(
    "FOS_GRID_START_COORDS", 0, 32767, FOS_GRIDSTART_DEF, 2);

static const int FOS_GRIDSIZE_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleFOS::FOS_GRIDSIZE(
    "FOS_GRID_TILE_SIZE", 0, 32767, FOS_GRIDSIZE_DEF, 2);

ISPC::ParameterGroup ISPC::ModuleFOS::getGroup()
{
    ParameterGroup group;

    group.header = "// Focus Statistics parameters";

    group.parameters.insert(FOS_ROI.name);
    group.parameters.insert(FOS_ROISTART.name);
    group.parameters.insert(FOS_ROIEND.name);
    group.parameters.insert(FOS_GRID.name);
    group.parameters.insert(FOS_GRIDSTART.name);
    group.parameters.insert(FOS_GRIDSIZE.name);

    return group;
}

ISPC::ModuleFOS::ModuleFOS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleFOS::load(const ParameterList &parameters)
{
    int i;
    bEnableROI = parameters.getParameter(FOS_ROI);
    for (i = 0 ; i < 2 ; i++)
    {
        aRoiStartCoord[i] = parameters.getParameter(FOS_ROISTART, i);
    }
    for (i = 0 ; i < 2 ; i++)
    {
        aRoiEndCoord[i] = parameters.getParameter(FOS_ROIEND, i);
    }

    bEnableGrid = parameters.getParameter(FOS_GRID);
    for (i = 0 ; i < 2 ; i++)
    {
        aGridStartCoord[i] = parameters.getParameter(FOS_GRIDSTART, i);
    }
    for (i = 0 ; i < 2 ; i++)
    {
        aGridTileSize[i] = parameters.getParameter(FOS_GRIDSIZE, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleFOS::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleFOS::getGroup();
    }

    parameters.addGroup("ModuleFOS", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(FOS_ROI.name,
            toString(this->bEnableROI)));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aRoiStartCoord[i]));
        }
        parameters.addParameter(Parameter(FOS_ROISTART.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aRoiEndCoord[i]));
        }
        parameters.addParameter(Parameter(FOS_ROIEND.name, values));

        parameters.addParameter(Parameter(FOS_GRID.name,
            toString(this->bEnableGrid)));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aGridStartCoord[i]));
        }
        parameters.addParameter(Parameter(FOS_GRIDSTART.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aGridTileSize[i]));
        }
        parameters.addParameter(Parameter(FOS_GRIDSIZE.name, values));
        break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(FOS_ROI.name,
            toString(FOS_ROI.def)));  // bool do not have min
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROISTART.min));
        }
        parameters.addParameter(Parameter(FOS_ROISTART.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROIEND.min));
        }
        parameters.addParameter(Parameter(FOS_ROIEND.name, values));

        parameters.addParameter(Parameter(FOS_GRID.name,
            toString(FOS_GRID.def)));  // bool do not have min
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSTART.min));
        }
        parameters.addParameter(Parameter(FOS_GRIDSTART.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSIZE.min));
        }
        parameters.addParameter(Parameter(FOS_GRIDSIZE.name, values));
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(FOS_ROI.name,
            toString(FOS_ROI.def)));  // bool do not have max
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROISTART.max));
        }
        parameters.addParameter(Parameter(FOS_ROISTART.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROIEND.max));
        }
        parameters.addParameter(Parameter(FOS_ROIEND.name, values));

        parameters.addParameter(Parameter(FOS_GRID.name,
            toString(FOS_GRID.def)));  // bool do not have max

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSTART.max));
        }
        parameters.addParameter(Parameter(FOS_GRIDSTART.name, values));

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSIZE.max));
        }
        parameters.addParameter(Parameter(FOS_GRIDSIZE.name, values));
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(FOS_ROI.name,
            toString(FOS_ROI.def) + " // " + getParameterInfo(FOS_ROI)));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROISTART.def[i]));
        }
        values.push_back("// " + getParameterInfo(FOS_ROISTART));
        parameters.addParameter(Parameter(FOS_ROISTART.name, values));

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_ROIEND.def[i]));
        }
        values.push_back("// " + getParameterInfo(FOS_ROIEND));
        parameters.addParameter(Parameter(FOS_ROIEND.name, values));

        parameters.addParameter(Parameter(FOS_GRID.name,
            toString(FOS_GRID.def) + " // " + getParameterInfo(FOS_GRID)));

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSTART.def[i]));
        }
        values.push_back("// " + getParameterInfo(FOS_GRIDSTART));
        parameters.addParameter(Parameter(FOS_GRIDSTART.name, values));

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(FOS_GRIDSIZE.def[i]));
        }
        values.push_back("// " + getParameterInfo(FOS_GRIDSIZE));
        parameters.addParameter(Parameter(FOS_GRIDSIZE.name, values));
        break;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleFOS::setup()
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

    pMCPipeline->sFOS.bGlobalEnable = bEnableGrid ? IMG_TRUE : IMG_FALSE;
    pMCPipeline->sFOS.bRegionEnable = bEnableROI ? IMG_TRUE : IMG_FALSE;

    pMCPipeline->sFOS.ui16Left = aGridStartCoord[0];
    pMCPipeline->sFOS.ui16Top = aGridStartCoord[1];
    pMCPipeline->sFOS.ui16Width = aGridTileSize[0];
    pMCPipeline->sFOS.ui16Height = aGridTileSize[1];

    pMCPipeline->sFOS.ui16ROILeft = aRoiStartCoord[0];
    pMCPipeline->sFOS.ui16ROITop = aRoiStartCoord[1];
    pMCPipeline->sFOS.ui16ROIWidth = aRoiEndCoord[0] - aRoiStartCoord[0] + 1;
    pMCPipeline->sFOS.ui16ROIHeight = aRoiEndCoord[1] - aRoiStartCoord[1] + 1;

    this->setupFlag = true;
    return IMG_SUCCESS;
}
