/**
******************************************************************************
 @file ModuleESC.cpp

 @brief Implementation of ISPC::ModuleESC

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
#include "ispc/ModuleESC.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_ESC"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

#define ESC_CLIP_STR "cliprect"
#define ESC_CROP_STR "croprect"
#define ESC_SIZE_STR "outsize"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleESC::ESC_RECTTYPE(
    "ESC_RECT_TYPE", ESC_CROP_STR);
const ISPC::ParamDefSingle<bool> ISPC::ModuleESC::ESC_ADJUSTCUTOFF(
    "ESC_ADJUST_CUTOFF_FREQ", false);

static const double ESC_PITCH_DEF[2] = { 1.0, 1.0 };
// min should be 1.0 but we handle the case when < 1.0 in setup
const ISPC::ParamDefArray<double> ISPC::ModuleESC::ESC_PITCH("ESC_PITCH",
    0.0, 16.0, ESC_PITCH_DEF, 2);

static const int ESC_RECT_DEF[4] = { 0, 0, 0, 0 };
const ISPC::ParamDefArray<int> ISPC::ModuleESC::ESC_RECT("ESC_RECT",
    0, 8192, ESC_RECT_DEF, 4);

#define ESC_INTERSTITIAL_STR "inter"
#define ESC_COSITED_STR "co-sited"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleESC::ESC_CHROMA_MODE(
    "ESC_CHROMA_MODE", ESC_INTERSTITIAL_STR);

ISPC::ParameterGroup ISPC::ModuleESC::getGroup()
{
    ParameterGroup group;

    group.header = "// Encoder pipeline Scaler parameters";

    group.parameters.insert(ESC_RECTTYPE.name);
    group.parameters.insert(ESC_CHROMA_MODE.name);
    group.parameters.insert(ESC_ADJUSTCUTOFF.name);
    group.parameters.insert(ESC_PITCH.name);
    group.parameters.insert(ESC_RECT.name);

    return group;
}

ISPC::ModuleESC::ModuleESC(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleESC::load(const ParameterList &parameters)
{
    int i;
    // default so that sizes of 0 means the whole image
    std::string rectangleType = parameters.getParameter(ESC_RECTTYPE);
    if (0 == rectangleType.compare(ESC_CLIP_STR))
    {
        eRectType = SCALER_RECT_CLIP;
    }
    else if (0 == rectangleType.compare(ESC_CROP_STR))
    {
        eRectType = SCALER_RECT_CROP;
    }
    else if (0 == rectangleType.compare(ESC_SIZE_STR))
    {
        eRectType = SCALER_RECT_SIZE;
    }
    else
    {
        MOD_LOG_ERROR("Invalid rectangle type: %s\n", rectangleType.c_str());
        return IMG_ERROR_FATAL;
    }

    std::string chromaMode = parameters.getParameter(ESC_CHROMA_MODE);
    if (0 == chromaMode.compare(ESC_INTERSTITIAL_STR))
    {
        bChromaInterstitial = true;
    }
    else if (0 == chromaMode.compare(ESC_COSITED_STR))
    {
        bChromaInterstitial = false;
    }
    else
    {
        MOD_LOG_ERROR("Invalid chroma mode: %s\n", chromaMode.c_str());
        return IMG_ERROR_FATAL;
    }

    bAdjustTaps  = parameters.getParameter(ESC_ADJUSTCUTOFF);
    for (i = 0 ; i < 2 ; i++)
    {
        aPitch[i] = parameters.getParameter(ESC_PITCH, i);
    }

    for (i = 0 ; i < 4 ; i++)
    {
        aRect[i] = parameters.getParameter(ESC_RECT, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleESC::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleESC::getGroup();
    }

    parameters.addGroup("ModuleESC", group);

    switch (t)
    {
    case SAVE_VAL:
        if (SCALER_RECT_CLIP == this->eRectType)
        {
            parameters.addParameter(ESC_RECTTYPE,
                std::string(ESC_CLIP_STR));
        }
        else if (SCALER_RECT_CROP == this->eRectType)
        {
            parameters.addParameter(ESC_RECTTYPE,
                    std::string(ESC_CROP_STR));
        }
        else if (this->eRectType == SCALER_RECT_SIZE )
        {
            parameters.addParameter(ESC_RECTTYPE,
                    std::string(ESC_SIZE_STR));
        }
        else
        {
            MOD_LOG_ERROR("Invalid rectangle type: %d\n", this->eRectType);
            return IMG_ERROR_UNEXPECTED_STATE;
        }

        if (this->bChromaInterstitial)
        {
            parameters.addParameter(ESC_CHROMA_MODE,
                    std::string(ESC_INTERSTITIAL_STR));
        }
        else
        {
            parameters.addParameter(ESC_CHROMA_MODE,
                    std::string(ESC_COSITED_STR));
        }

        parameters.addParameter(ESC_ADJUSTCUTOFF, this->bAdjustTaps);

        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aPitch[i]));
        }
        parameters.addParameter(ESC_PITCH, values);

        values.clear();
        for (i = 0 ; i < 4 ; i++)
        {
            values.push_back(toString(this->aRect[i]));
        }
        parameters.addParameter(ESC_RECT, values);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(ESC_RECTTYPE);  // rectype does not have a min
        parameters.addParameterMin(ESC_CHROMA_MODE);  // chroma mode does not have a min
        parameters.addParameterMin(ESC_ADJUSTCUTOFF);
        // adjust cutoff does not have a min
        parameters.addParameterMin(ESC_PITCH);
        parameters.addParameterMin(ESC_RECT);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(ESC_RECTTYPE);  // rectype does not have a max
        parameters.addParameterMax(ESC_CHROMA_MODE);  // chroma mode does not have a max
        parameters.addParameterMax(ESC_ADJUSTCUTOFF);
        parameters.addParameterMax(ESC_PITCH);
        parameters.addParameterMax(ESC_RECT);
        break;

    case SAVE_DEF:
        {
            std::ostringstream defComment;

            defComment.str("");
            defComment << "{"
                << ESC_CLIP_STR
                << ", " << ESC_CROP_STR
                << ", " << ESC_SIZE_STR
                << "}";

            parameters.addParameterDef(ESC_RECTTYPE);
            parameters.getParameter(ESC_RECTTYPE.name)->setInfo(
                                defComment.str());

            defComment.str("");
            defComment << "{"
                << ESC_INTERSTITIAL_STR
                << ", " << ESC_COSITED_STR
                << "}";

            parameters.addParameterDef(ESC_CHROMA_MODE);
            parameters.getParameter(ESC_CHROMA_MODE.name)->setInfo(
                    defComment.str());
            parameters.addParameterDef(ESC_ADJUSTCUTOFF);
        }
        parameters.addParameterDef(ESC_PITCH);
        parameters.addParameterDef(ESC_RECT);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleESC::setup()
{
    LOG_PERF_IN();
    IMG_INT32 requiredSizeX, requiredSizeY;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 image_width, image_height;
    const ModuleOUT *glb = pipeline->getModule<ModuleOUT>();

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

    if (aPitch[0] < 1.0 )
    {
        if (0 == aRect[2])
        {
            MOD_LOG_WARNING("forcing ESC output width to the imager's size\n");
            aRect[2] = image_width;
        }
        else if (aRect[2] > image_width )
        {
            MOD_LOG_ERROR("the width %d to compute the ESC horizontal "\
                "pitch from is bigger than the imager's width (%d)!\n",
                aRect[2], image_width);
            return IMG_ERROR_FATAL;
        }

        if (SCALER_RECT_SIZE != eRectType)
        {
            MOD_LOG_WARNING("force rectangle type to be output sizes "\
                "to compute the ESC pitch\n");
        }
        eRectType = SCALER_RECT_SIZE;
        this->aPitch[0] = static_cast<double>(image_width)/aRect[2];
        if (aPitch[0] > ESC_PITCH.max)
        {
            MOD_LOG_WARNING("computed H pitch %lf too big, max %lf "\
                "forced instead\n",
                aPitch[0], ESC_PITCH.max);
            aPitch[0] = ESC_PITCH.max;
        }
    }
    if (aPitch[1] < 1.0 )
    {
        if (0 == aRect[3])
        {
            MOD_LOG_WARNING("forcing ESC output height to the "\
                "imager's size\n");
            aRect[3] = image_height;
        }
        else if (aRect[3] > image_height )
        {
            MOD_LOG_ERROR("ERROR - the height %d to compute the ESC "\
                "vertical pitch from is bigger than the imager's "\
                "height (%d)!\n",
                aRect[3], image_height);
            return IMG_ERROR_FATAL;
        }

        if (SCALER_RECT_SIZE != eRectType)
        {
            MOD_LOG_WARNING("force rectangle type to be output sizes "\
                "to compute the ESC pitch\n");
        }
        eRectType = SCALER_RECT_SIZE;
        this->aPitch[1] = static_cast<double>(image_height)/aRect[3];
        if (aPitch[1] > ESC_PITCH.max)
        {
            MOD_LOG_WARNING("computed V pitch %lf too big, max %lf "\
                "forced instead\n",
                aPitch[1], ESC_PITCH.max);
            aPitch[1] = ESC_PITCH.max;
        }
    }

    pMCPipeline->sESC.aPitch[0] = aPitch[0];
    pMCPipeline->sESC.aPitch[1] = aPitch[1];

    /* : Check below that the global_Setup->SENSOR_WIDTH and
     * SENSOR_HEIGHT is what corresponds here */
    pMCPipeline->sESC.aOffset[0] = aRect[0];
    pMCPipeline->sESC.aOffset[1] = aRect[1];
    switch (eRectType)
    {
        case SCALER_RECT_CLIP:  // cliprect
            pMCPipeline->sESC.aOutputSize[0]=
                static_cast<IMG_UINT16>((aRect[2] - aRect[0]) / aPitch[0]);
            pMCPipeline->sESC.aOutputSize[1] =
                static_cast<IMG_UINT16>((aRect[3] - aRect[1]) / aPitch[1]);
            requiredSizeX = aRect[2];
            requiredSizeY = aRect[3];
            break;

        case SCALER_RECT_CROP:  // croprect
            pMCPipeline->sESC.aOutputSize[0] =
                static_cast<IMG_UINT16>((image_width - aRect[0] - aRect[2])
                / aPitch[0]);
            pMCPipeline->sESC.aOutputSize[1] =
                static_cast<IMG_UINT16>((image_height - aRect[1] - aRect[3])
                / aPitch[1]);

            requiredSizeX = aRect[0] + aRect[2] + (IMG_INT)(ceil(aPitch[0]));
            requiredSizeY = aRect[1] + aRect[3] + (IMG_INT)(ceil(aPitch[1]));
            break;

        case SCALER_RECT_SIZE:  // outsize
            pMCPipeline->sESC.aOutputSize[0] = aRect[2];
            pMCPipeline->sESC.aOutputSize[1] = aRect[3];
            requiredSizeX = aRect[0]
                + static_cast<IMG_INT32>(ceil(aRect[2] * aPitch[0]));
            requiredSizeY = aRect[1]
                + static_cast<IMG_INT32>(ceil(aRect[3] * aPitch[1]));
    }

    pMCPipeline->sESC.bAdjustCutoff = bAdjustTaps ? IMG_TRUE : IMG_FALSE;
    pMCPipeline->sESC.bBypassESC = IMG_FALSE;
    // we don't bypass the scaler if we are here
    pMCPipeline->sESC.bChromaInter = bChromaInterstitial ?
        IMG_TRUE : IMG_FALSE;

    // ensure output size is even
    if (0 != pMCPipeline->sESC.aOutputSize[0]%2)
    {
        pMCPipeline->sESC.aOutputSize[0]--;
    }
    if (0 != pMCPipeline->sESC.aOutputSize[1]%2)
    {
        pMCPipeline->sESC.aOutputSize[1]--;
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
