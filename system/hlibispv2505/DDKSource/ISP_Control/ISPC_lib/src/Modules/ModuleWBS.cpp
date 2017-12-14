/**
*******************************************************************************
 @file ModuleWBS.cpp

 @brief Implementation of ISPC::ModuleWBS

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
#include "ispc/ModuleWBS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_WBS"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDef<int> ISPC::ModuleWBS::WBS_ROI("WBS_ROI_ENABLE", 0, 2, 0);

#define WBS_RGBMAX_MIN 0.0f
#define WBS_RGBMAX_MAX 1.0f
static const double WBS_RGBMAX_DEF[] = {0.75, 0.75};
const ISPC::ParamDefArray<double> ISPC::ModuleWBS::WBS_R_MAX(
    "WBS_RMAX_TH", WBS_RGBMAX_MIN, WBS_RGBMAX_MAX, WBS_RGBMAX_DEF, WBS_NUM_ROI);
const ISPC::ParamDefArray<double> ISPC::ModuleWBS::WBS_G_MAX(
    "WBS_GMAX_TH", WBS_RGBMAX_MIN, WBS_RGBMAX_MAX, WBS_RGBMAX_DEF, WBS_NUM_ROI);
const ISPC::ParamDefArray<double> ISPC::ModuleWBS::WBS_B_MAX(
    "WBS_BMAX_TH", WBS_RGBMAX_MIN, WBS_RGBMAX_MAX, WBS_RGBMAX_DEF, WBS_NUM_ROI);

// there are 2 ROI but the values are duplicated
static const double WBS_Y_HLW_DEF[] = {0.75, 0.75};
const ISPC::ParamDefArray<double> ISPC::ModuleWBS::WBS_Y_HLW(
    "WBS_YHLW_TH", 0.0f, 1.0f, WBS_Y_HLW_DEF, WBS_NUM_ROI);

// there are 2 ROI but the values are duplicated
static const int WBS_ROISTART_DEF[] = {0, 0, 0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleWBS::WBS_ROISTART(
    "WBS_ROI_START_COORDS", 0, 32767, WBS_ROISTART_DEF, WBS_NUM_ROI * 2);

// there are 2 ROI but the values are duplicated
static const int WBS_ROIEND_DEF[] = {0, 0, 0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleWBS::WBS_ROIEND(
    "WBS_ROI_END_COORDS", 0, 32767, WBS_ROIEND_DEF, WBS_NUM_ROI * 2);

ISPC::ParameterGroup ISPC::ModuleWBS::getGroup()
{
    ParameterGroup group;

    group.header = "// White Balance Statistics parameters";

    group.parameters.insert(WBS_ROI.name);
    group.parameters.insert(WBS_R_MAX.name);
    group.parameters.insert(WBS_G_MAX.name);
    group.parameters.insert(WBS_B_MAX.name);
    group.parameters.insert(WBS_Y_HLW.name);
    group.parameters.insert(WBS_ROISTART.name);
    group.parameters.insert(WBS_ROIEND.name);

    return group;
}

ISPC::ModuleWBS::ModuleWBS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleWBS::load(const ParameterList &parameters)
{
    int i;
    ui32NROIEnabled = parameters.getParameter(WBS_ROI);

    for (i = 0; i < WBS_NUM_ROI; i++)
    {
        aRedMaxTH[i] = parameters.getParameter(WBS_R_MAX, i);
    }
    for (i = 0 ; i < WBS_NUM_ROI ; i++)
    {
        aGreenMaxTH[i] = parameters.getParameter(WBS_G_MAX, i);
    }
    for (i = 0 ; i < WBS_NUM_ROI ; i++)
    {
        aBlueMaxTH[i] = parameters.getParameter(WBS_B_MAX, i);
    }
    for (i = 0 ; i < WBS_NUM_ROI ; i++)
    {
        aYHLWTH[i] = parameters.getParameter(WBS_Y_HLW, i);
    }

    for (i = 0 ; i < WBS_NUM_ROI ; i++)
    {
        for (int j = 0 ; j < 2 ; j++)
        {
            aROIStartCoord[i][j] =
                parameters.getParameter(WBS_ROISTART, i * 2 + j);
            aROIEndCoord[i][j] =
                parameters.getParameter(WBS_ROIEND, i * 2 + j);
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBS::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleWBS::getGroup();
    }

    parameters.addGroup("ModuleWBS", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(WBS_ROI, (int)this->ui32NROIEnabled);
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aRedMaxTH[i]));
        }
        parameters.addParameter(WBS_R_MAX, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aGreenMaxTH[i]));
        }
        parameters.addParameter(WBS_G_MAX, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aBlueMaxTH[i]));
        }
        parameters.addParameter(WBS_B_MAX, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aYHLWTH[i]));
        }
        parameters.addParameter(WBS_Y_HLW, values);

        values.clear();
        for (i = 0 ; i < WBS_NUM_ROI ; i++)
        {
            for (int j = 0 ; j < 2 ; j++)
            {
                values.push_back(toString(this->aROIStartCoord[i][j]));
            }
        }
        parameters.addParameter(WBS_ROISTART, values);

        values.clear();
        for (i = 0 ; i < WBS_NUM_ROI ; i++)
        {
            for (int j = 0 ; j < 2 ; j++)
            {
                values.push_back(toString(this->aROIEndCoord[i][j]));
            }
        }
        parameters.addParameter(WBS_ROIEND, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(WBS_ROI);
        parameters.addParameterMin(WBS_R_MAX);
        parameters.addParameterMin(WBS_G_MAX);
        parameters.addParameterMin(WBS_B_MAX);
        parameters.addParameterMin(WBS_Y_HLW);
        parameters.addParameterMin(WBS_ROISTART);
        parameters.addParameterMin(WBS_ROIEND);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(WBS_ROI);
        parameters.addParameterMax(WBS_R_MAX);
        parameters.addParameterMax(WBS_G_MAX);
        parameters.addParameterMax(WBS_B_MAX);
        parameters.addParameterMax(WBS_Y_HLW);
        parameters.addParameterMax(WBS_ROISTART);
        parameters.addParameterMax(WBS_ROIEND);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(WBS_ROI);
        parameters.addParameterDef(WBS_R_MAX);
        parameters.addParameterDef(WBS_G_MAX);
        parameters.addParameterDef(WBS_B_MAX);
        parameters.addParameterDef(WBS_Y_HLW);
        parameters.addParameterDef(WBS_ROISTART);
        parameters.addParameterDef(WBS_ROIEND);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleWBS::setup()
{
    LOG_PERF_IN();
    int roi;
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

    // Whole image covered by default
    for (roi = 0; roi < WBS_NUM_ROI; roi++)
    {
        pMCPipeline->sWBS.aRoiTop[roi]= aROIStartCoord[roi][0];
        pMCPipeline->sWBS.aRoiLeft[roi]= aROIStartCoord[roi][1];

        pMCPipeline->sWBS.aRoiWidth[roi] = aROIEndCoord[roi][0] -
                                            aROIStartCoord[roi][0];
        pMCPipeline->sWBS.aRoiHeight[roi]= aROIEndCoord[roi][1] -
                                            aROIStartCoord[roi][1];
     }

    // Default value
    pMCPipeline->sWBS.fYOffset = -127;
    pMCPipeline->sWBS.fRGBOffset = 0.0;

    for (roi = 0; roi < WBS_NUM_ROI; roi++)
    {
        pMCPipeline->sWBS.aRMax[roi] = static_cast<IMG_INT32>(aRedMaxTH[roi]
            * (1 << (WBS_USR_BITDEPTH - 1)));
        pMCPipeline->sWBS.aGMax[roi] = static_cast<IMG_INT32>(aGreenMaxTH[roi]
            * (1 << (WBS_USR_BITDEPTH - 1)));
        pMCPipeline->sWBS.aBMax[roi] = static_cast<IMG_INT32>(aBlueMaxTH[roi]
            * (1 << (WBS_USR_BITDEPTH - 1)));
        // This is measured after R2Y conversion and seems to have 1 bit less
        // : re-check this is true
        pMCPipeline->sWBS.aYMax[roi] = static_cast<IMG_INT32>(aYHLWTH[roi]
            * (1 << (WBS_USR_BITDEPTH - 1)));
    }

    pMCPipeline->sWBS.ui8ActiveROI = ui32NROIEnabled;
    if (pMCPipeline->sWBS.ui8ActiveROI > WBS_NUM_ROI)
    {
        MOD_LOG_WARNING("Invalid number of regions activated %d, "\
            "Max number is %d\n", ui32NROIEnabled, WBS_NUM_ROI);
        pMCPipeline->sWBS.ui8ActiveROI = WBS_NUM_ROI;
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
