/**
******************************************************************************
 @file ModuleIIF.cpp

 @brief Implementation of ISPC::ModuleIIF

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
#include "ispc/ModuleIIF.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_IIF"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

#define RGGB_STR "RGGB"
#define GRBG_STR "GRBG"
#define GBRG_STR "GBRG"
#define BGGR_STR "BGGR"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleIIF::IIF_BAYERFMT(
    "IIF_BAYER_FORMAT", RGGB_STR);

static const int IIF_DECIMATION_DEF[2] = {1, 1};
const ISPC::ParamDefArray<int> ISPC::ModuleIIF::IIF_DECIMATION(
    "IIF_DECIMATION", 1, 16, IIF_DECIMATION_DEF, 2);

static const int IIF_CAPRECT_TL_DEF[2] = {0, 0};  // default is different!
const ISPC::ParamDefArray<int> ISPC::ModuleIIF::IIF_CAPRECT_TL(
    "IIF_CAP_RECT_TL", 0, 8192, IIF_CAPRECT_TL_DEF, 2);

// default is instead sensor size
static const int IIF_CAPRECT_BR_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleIIF::IIF_CAPRECT_BR(
    "IIF_CAP_RECT_BR", 0, 8192, IIF_CAPRECT_BR_DEF, 2);

ISPC::ParameterGroup ISPC::ModuleIIF::getGroup()
{
    ParameterGroup group;

    group.header = "// Imager Interface parameters";

    group.parameters.insert(IIF_CAPRECT_TL.name);
    group.parameters.insert(IIF_CAPRECT_BR.name);
    group.parameters.insert(IIF_DECIMATION.name);
    group.parameters.insert(IIF_BAYERFMT.name);

    return group;
}

std::string ISPC::ModuleIIF::getMosaicString(eMOSAIC mosaic)
{
    switch (mosaic)
    {
    case MOSAIC_GRBG:
        return GRBG_STR;

    case MOSAIC_GBRG:
        return GBRG_STR;

    case MOSAIC_BGGR:
        return BGGR_STR;

    case MOSAIC_RGGB:
    default:
        return RGGB_STR;
    }
}

ISPC::ModuleIIF::ModuleIIF(): SetupModuleBase(LOG_TAG)
{
    // ParameterList defaults;
    // load(defaults);
    // cannot load defaults because pipeline is not set yet
    // write defaults manually
    aDecimation[0] = IIF_DECIMATION.def[0]-1;
    aDecimation[1] = IIF_DECIMATION.def[1]-1;
    aCropTL[0] = IIF_CAPRECT_TL.def[0];
    aCropTL[1] = IIF_CAPRECT_TL.def[1];
    aCropBR[0] = IIF_CAPRECT_BR.def[0];
    aCropBR[1] = IIF_CAPRECT_BR.def[1];
}

IMG_RESULT ISPC::ModuleIIF::load(const ParameterList &parameters)
{
    int i;
    const Sensor* sensor = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("Pipeline pointer not setup!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    sensor = pipeline->getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("Pipeline does not have a sensor to get "\
            "information from!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    int value;
    int thedefwidth = ispc_min(IIF_CAPRECT_BR.max,
        ispc_max(IIF_CAPRECT_BR.min, static_cast<int>(sensor->uiWidth))) - 1;
    int thedefheight = ispc_min(IIF_CAPRECT_BR.max,
        ispc_max(IIF_CAPRECT_BR.min, static_cast<int>(sensor->uiHeight))) - 1;

    value = parameters.getParameter(IIF_CAPRECT_TL, 0);
    aCropTL[0] = ispc_min(value, static_cast<int>(sensor->uiWidth));

    value = parameters.getParameter(IIF_CAPRECT_TL, 1);
    aCropTL[1] = ispc_min(value, static_cast<int>(sensor->uiHeight));

    /* use sensor width as default - to do so it's not using template
     * with ParamDef object */
    value = parameters.getParameter<int>(IIF_CAPRECT_BR.name, 0,
        IIF_CAPRECT_BR.min, IIF_CAPRECT_BR.max, thedefwidth);
    aCropBR[0] = ispc_min(value, static_cast<int>(sensor->uiWidth) - 1);

    /* use sensor height as default - to do so it's not using template
     * with ParamDef object */
    value = parameters.getParameter<int>(IIF_CAPRECT_BR.name, 1,
        IIF_CAPRECT_BR.min, IIF_CAPRECT_BR.max, thedefheight);
    aCropBR[1] = ispc_min(value, static_cast<int>(sensor->uiHeight) - 1);

    for (i = 0 ; i < 2 ; i++)
    {
        /* We use 0 as no decimation. In the parameters we have 1
         * as no decimation */
        aDecimation[i] = parameters.getParameter(IIF_DECIMATION, i) - 1;
    }

    /// @ check if BR <= TL!!!

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleIIF::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleIIF::getGroup();
    }

    parameters.addGroup("ModuleIIF", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aCropTL[i]));
        }
        parameters.addParameter(IIF_CAPRECT_TL, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aCropBR[i]));
        }
        parameters.addParameter(IIF_CAPRECT_BR, values);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aDecimation[i]+1));
        }
        parameters.addParameter(IIF_DECIMATION, values);

        if (pipeline->getSensor())
        {
            parameters.addParameter(IIF_BAYERFMT,
                getMosaicString(pipeline->getSensor()->eBayerFormat));
        }
        else
        {
            MOD_LOG_WARNING("Cannot save sensor's Bayer format as "\
                "attached pipeline does not have a sensor!\n");
        }
        break;

    case SAVE_MIN:
        parameters.addParameterMin(IIF_CAPRECT_TL);
        parameters.addParameterMin(IIF_CAPRECT_BR);
        parameters.addParameterMin(IIF_DECIMATION);
        parameters.addParameterMin(IIF_BAYERFMT);  // string does not have min
        break;

    case SAVE_MAX:
        parameters.addParameterMax(IIF_CAPRECT_TL);
        parameters.addParameterMax(IIF_CAPRECT_BR);
        parameters.addParameterMax(IIF_DECIMATION);
        parameters.addParameterMax(IIF_BAYERFMT);  // string does not have max
        break;

    case SAVE_DEF:
        parameters.addParameterDef(IIF_CAPRECT_TL);
        parameters.addParameterDef(IIF_CAPRECT_BR);
        parameters.addParameterDef(IIF_DECIMATION);

        {
            std::ostringstream defComment;

            defComment.str("");
            defComment << "{"
                << RGGB_STR
                << ", " << GRBG_STR
                << ", " << GBRG_STR
                << ", " << BGGR_STR
                << "}";

            parameters.addParameterDef(IIF_BAYERFMT);
            parameters.getParameter(IIF_BAYERFMT.name)->setInfo(
                    defComment.str());
        }
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleIIF::setup()
{
    LOG_PERF_IN();
    MC_PIPELINE *pMCPipeline = NULL;
    const Sensor *sensor = NULL;
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
    sensor = pipeline->getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("Pipeline does not have a sensor to get "\
            "information from!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // ID of the imager to be used
    pMCPipeline->sIIF.ui8Imager = sensor->uiImager;

    // Bayer format, currently received from parameters
    pMCPipeline->sIIF.eBayerFormat = sensor->eBayerFormat;

    // Decimation
    pMCPipeline->sIIF.ui16ImagerDecimation[0] = aDecimation[0];
    pMCPipeline->sIIF.ui16ImagerDecimation[1] = aDecimation[1];

    // Clipping offset left,top (From parameters)
    pMCPipeline->sIIF.ui16ImagerOffset[0] = aCropTL[0];  // pixels
    pMCPipeline->sIIF.ui16ImagerOffset[1] = aCropTL[1];  // pixels

    // Cliping offset bottom, right
    pMCPipeline->sIIF.ui16ImagerSize[0] =
        (aCropBR[0] - aCropTL[0] + CI_CFA_WIDTH)
        / (CI_CFA_WIDTH*(aDecimation[0] + 1));
    pMCPipeline->sIIF.ui16ImagerSize[1] =
        (aCropBR[1] - aCropTL[1] + CI_CFA_HEIGHT)
        / (CI_CFA_HEIGHT*(aDecimation[1] + 1));

    pMCPipeline->sIIF.ui8BlackBorderOffset = 0;  // pixels

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
