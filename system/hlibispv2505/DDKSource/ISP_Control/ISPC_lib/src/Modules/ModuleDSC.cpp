/**
******************************************************************************
 @file ModuleDSC.cpp

 @brief Implementation of ISPC::ModuleDSC

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
#include "ispc/ModuleDSC.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>

#define LOG_TAG "ISPC_MOD_DSC"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

#define DSC_CLIP_STR "cliprect"
#define DSC_CROP_STR "croprect"
#define DSC_SIZE_STR "outsize"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleDSC::DSC_RECTTYPE(
    "DSC_RECT_TYPE", DSC_CROP_STR);
const ISPC::ParamDefSingle<bool> ISPC::ModuleDSC::DSC_ADJUSTCUTOFF(
    "DSC_ADJUST_CUTOFF_FREQ", false);

static const double DSC_PITCH_DEF[2] = { 1.0, 1.0 };
// min should be 1.0 but we handle the case when < 1.0 in setup
const ISPC::ParamDefArray<double> ISPC::ModuleDSC::DSC_PITCH("DSC_PITCH",
    0.0, 16.0, DSC_PITCH_DEF, 2);

static const int DSC_RECT_DEF[4] = { 0, 0, 0, 0 };
const ISPC::ParamDefArray<int> ISPC::ModuleDSC::DSC_RECT("DSC_RECT",
    0, 8192, DSC_RECT_DEF, 4);

ISPC::ParameterGroup ISPC::ModuleDSC::getGroup()
{
    ParameterGroup group;

    group.header = "// Display pipeline Scaler parameters";

    group.parameters.insert(DSC_RECTTYPE.name);
    group.parameters.insert(DSC_ADJUSTCUTOFF.name);
    group.parameters.insert(DSC_PITCH.name);
    group.parameters.insert(DSC_RECT.name);

    return group;
}

ISPC::ModuleDSC::ModuleDSC(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleDSC::load(const ParameterList &parameters)
{
    int i;
    // default so that sizes of 0 means whole image
    std::string rectangleType = parameters.getParameter(DSC_RECTTYPE);
    if (0 == rectangleType.compare(DSC_CLIP_STR))
    {
        eRectType = SCALER_RECT_CLIP;
    }
    else if (0 == rectangleType.compare(DSC_CROP_STR))
    {
        eRectType = SCALER_RECT_CROP;
    }
    else if (0 == rectangleType.compare(DSC_SIZE_STR))
    {
        eRectType = SCALER_RECT_SIZE;
    }
    else
    {
        MOD_LOG_ERROR("Inavalid rectangle type: %s\n", rectangleType.c_str());
        return IMG_ERROR_FATAL;
    }

    bAdjustTaps  = parameters.getParameter(DSC_ADJUSTCUTOFF);
    for (i = 0 ; i < 2 ; i++)
    {
        aPitch[i] = parameters.getParameter(DSC_PITCH, i);
    }

    for (i = 0 ; i < 4 ; i++)
    {
        aRect[i] = parameters.getParameter(DSC_RECT, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDSC::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleDSC::getGroup();
    }

    parameters.addGroup("ModuleDSC", group);

    switch (t)
    {
    case SAVE_VAL:
        if (SCALER_RECT_CLIP == this->eRectType)
        {
            parameters.addParameter(DSC_RECTTYPE,
                    std::string(DSC_CLIP_STR));
        }
        else if (SCALER_RECT_SIZE == this->eRectType)
        {
            parameters.addParameter(DSC_RECTTYPE,
                    std::string(DSC_SIZE_STR));
        }
        else
        {
            parameters.addParameter(DSC_RECTTYPE,
                    std::string(DSC_CROP_STR));
        }

        parameters.addParameter(DSC_ADJUSTCUTOFF, this->bAdjustTaps);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aPitch[i]));
        }
        parameters.addParameter(DSC_PITCH, values);

        values.clear();
        for (i = 0 ; i < 4 ; i++)
        {
            values.push_back(toString(this->aRect[i]));
        }
        parameters.addParameter(DSC_RECT, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(DSC_RECTTYPE);
        parameters.addParameterMin(DSC_ADJUSTCUTOFF);
        parameters.addParameterMin(DSC_PITCH);
        parameters.addParameterMin(DSC_RECT);
        break;

    case SAVE_MAX:
        parameters.addParameterMin(DSC_RECTTYPE);
        parameters.addParameterMax(DSC_ADJUSTCUTOFF);
        parameters.addParameterMax(DSC_PITCH);
        parameters.addParameterMax(DSC_RECT);
        break;

    case SAVE_DEF:
        {
            std::ostringstream defComment;

            defComment.str("");
            defComment << "{"
                << DSC_CLIP_STR
                << ", " << DSC_CROP_STR
                << ", " << DSC_SIZE_STR
                << "}";

            parameters.addParameterDef(DSC_RECTTYPE);
            parameters.getParameter(DSC_RECTTYPE.name)->setInfo(
                    defComment.str());
            parameters.addParameterDef(DSC_ADJUSTCUTOFF);
        }
        parameters.addParameterDef(DSC_PITCH);
        parameters.addParameterDef(DSC_RECT);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDSC::setup()
{
    LOG_PERF_IN();
    IMG_INT32 requiredSizeX, requiredSizeY;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 image_width, image_height;
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

    if (0 == pMCPipeline->sIIF.ui16ImagerSize[0]
        || 0 == pMCPipeline->sIIF.ui16ImagerSize[1])
    {
        MOD_LOG_ERROR("IIF should have been setup beforehand!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    image_width = pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
    image_height = pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

    /** @ this has to be removed onced we got the info from the camera
     * in the GUI */
    if (aPitch[0] < 1.0)
    {
        if (0 == aRect[2])
        {
            MOD_LOG_WARNING("forcing the DSC width to the imager's size\n");
            aRect[2] = image_width;
        }
        else if (aRect[2] > image_width)
        {
            MOD_LOG_ERROR("the width %d to compute the DSC horizontal "\
                "pitch from is bigger than the imager's width (%d)!\n",
                aRect[2], image_width);
            return IMG_ERROR_FATAL;
        }

        if (SCALER_RECT_SIZE != eRectType)
        {
            MOD_LOG_WARNING("force rectangle type to be output sizes to "\
                "compute the DSC pitch\n");
        }
        eRectType = SCALER_RECT_SIZE;
        aPitch[0] = static_cast<double>(image_width) / aRect[2];
        if (aPitch[0] > DSC_PITCH.max)
        {
            MOD_LOG_WARNING("computed H pitch %lf too big, max %lf "\
                "forced instead\n",
                aPitch[0], DSC_PITCH.max);
            aPitch[0] = DSC_PITCH.max;
        }
    }
    if (aPitch[1] < 1.0)
    {
        if (0 == aRect[3])
        {
            MOD_LOG_WARNING("forcing the DSC height to the imager's size\n");
            aRect[3] = image_height;
        }
        else if (aRect[3] > image_height)
        {
            MOD_LOG_ERROR("the height %d to compute the DSC vertical "\
                "pitch from is bigger than the imager's height (%d)!\n",
                aRect[3], image_height);
            return IMG_ERROR_FATAL;
        }

        if (SCALER_RECT_SIZE != eRectType)
        {
            MOD_LOG_WARNING("force rectangle type to be output sizes to "\
                "compute the DSC pitch\n");
        }
        eRectType = SCALER_RECT_SIZE;
        aPitch[1] = static_cast<double>(image_height)/aRect[3];
        if (aPitch[1] > DSC_PITCH.max)
        {
            MOD_LOG_WARNING("computed V pitch %lf too big, max %lf "\
                "forced instead\n",
                aPitch[1], DSC_PITCH.max);
            aPitch[1] = DSC_PITCH.max;
        }
    }

    pMCPipeline->sDSC.aPitch[0] = aPitch[0];
    pMCPipeline->sDSC.aPitch[1] = aPitch[1];

    /* : Check below taht the global_Setup->SENSOR_WIDTH and
     * SENSOR_HEIGHT is what corresponds here */
    pMCPipeline->sDSC.aOffset[0] = aRect[0];
    pMCPipeline->sDSC.aOffset[1] = aRect[1];
    switch (eRectType)
    {
        case SCALER_RECT_CLIP:  // cliprect
            pMCPipeline->sDSC.aOutputSize[0] =
                static_cast<IMG_UINT16>((aRect[2] - aRect[0]) / aPitch[0]);
            pMCPipeline->sDSC.aOutputSize[1] =
                static_cast<IMG_UINT16>((aRect[3] - aRect[1]) / aPitch[1]);
            requiredSizeX = aRect[2];
            requiredSizeY = aRect[3];
            break;

        case SCALER_RECT_CROP:  // croprect
            pMCPipeline->sDSC.aOutputSize[0] =
                static_cast<IMG_UINT16>((image_width - aRect[0] - aRect[2])
                / aPitch[0]);
            pMCPipeline->sDSC.aOutputSize[1] =
                static_cast<IMG_UINT16>((image_height - aRect[1] - aRect[3])
                / aPitch[1]);
            requiredSizeX = aRect[0] + aRect[2]
                + static_cast<IMG_INT32>(ceil(aPitch[0]));
            requiredSizeY = aRect[1] + aRect[3]
                + static_cast<IMG_INT32>(ceil(aPitch[1]));
            break;

        case SCALER_RECT_SIZE:  // outsize
            pMCPipeline->sDSC.aOutputSize[0] = aRect[2];
            pMCPipeline->sDSC.aOutputSize[1] = aRect[3];
            requiredSizeX = aRect[0]
                + static_cast<IMG_INT32>(ceil(aRect[2] * aPitch[0]));
            requiredSizeY = aRect[1]
                + static_cast<IMG_INT32>(ceil(aRect[3] * aPitch[1]));
    }

    pMCPipeline->sDSC.bAdjustCutoff = bAdjustTaps ? IMG_TRUE : IMG_FALSE;
    // we don't bypass the scaler if we are here
    pMCPipeline->sDSC.bBypassDSC = IMG_FALSE;

    // ensure output width is even
    if (0 != pMCPipeline->sDSC.aOutputSize[0]%2)
    {
        pMCPipeline->sDSC.aOutputSize[0]--;
    }

    /* global may not have been setup and if DE is enabled DSC will not be
     * used by HW anyway */
    // if (pipeline->globalSetup.EXTRACTION_POINT != CI_INOUT_NONE)
    //  pMCPipeline->sDSC.bBypassDSC = IMG_TRUE;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
