/**
*******************************************************************************
@file ModuleY2R.cpp

@brief Implementation of ISPC::ModuleY2R

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
#include "ispc/ModuleY2R.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_Y2R"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

#define BT601_STR "BT601"
#define BT709_STR "BT709"
#define JFIF_STR "JFIF"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleY2R::Y2R_MATRIX_STD(
    "Y2R_MATRIX", BT709_STR);
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_BRIGHTNESS(
    "Y2R_BRIGHTNESS", -0.5, 0.5, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_CONTRAST(
    "Y2R_CONTRAST", 0.1, 10.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_SATURATION(
    "Y2R_SATURATION", 0.1, 10.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_HUE(
    "Y2R_HUE", -30.0, 30.0, 0.0);
// same min, max tha R2Y but different default
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_OFFSETU(
    "Y2R_OFFSETU", R2Y_OFFSETUV_MIN, R2Y_OFFSETUV_MAX, Y2R_OFFSETUV_DEF);
// same min, max tha R2Y but different default
const ISPC::ParamDef<double> ISPC::ModuleY2R::Y2R_OFFSETV(
    "Y2R_OFFSETV", R2Y_OFFSETUV_MIN, R2Y_OFFSETUV_MAX, Y2R_OFFSETUV_DEF);

const double Y2R_RANGEMULT_DEF[3] = { 1.0f, 1.0f, 1.0f };
const ISPC::ParamDefArray<double> ISPC::ModuleY2R::Y2R_RANGEMULT(
    "Y2R_RANGE_MUL", 0.0, 2.0, Y2R_RANGEMULT_DEF, 3);

ISPC::ParameterGroup ISPC::ModuleY2R::getGroup()
{
    ParameterGroup group;

    group.header = "// YUV to RGB parameters";

    group.parameters.insert(Y2R_MATRIX_STD.name);
    group.parameters.insert(Y2R_BRIGHTNESS.name);
    group.parameters.insert(Y2R_CONTRAST.name);
    group.parameters.insert(Y2R_SATURATION.name);
    group.parameters.insert(Y2R_HUE.name);
    group.parameters.insert(Y2R_RANGEMULT.name);
    group.parameters.insert(Y2R_OFFSETU.name);
    group.parameters.insert(Y2R_OFFSETV.name);

    return group;
}

ISPC::ModuleY2R::ModuleY2R() : SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleY2R::load(const ParameterList &parameters)
{
    int i;
    std::string conversionMatrix = parameters.getParameterString(
        Y2R_MATRIX_STD.name, 0, Y2R_MATRIX_STD.def);

    if (0 == conversionMatrix.compare(BT601_STR))
    {
        eMatrix = BT601;
    }
    else if (0 == conversionMatrix.compare(BT709_STR))
    {
        eMatrix = BT709;
    }
    // JFIF not supported by driver
#ifdef INFOTM_SUPPORT_JFIF
    else if(0 == conversionMatrix.compare(JFIF_STR))
    {
        eMatrix = JFIF;
    }
#else
    /*else if(0 == conversionMatrix.compare(JFIF_STR))
    {
    MATRIX = JFIF;
    } */
#endif
    else
    {
        MOD_LOG_ERROR("Invalid matrix conversion format: '%s'\n",
            conversionMatrix.c_str());
        return IMG_ERROR_FATAL;
    }

    fBrightness = parameters.getParameter(Y2R_BRIGHTNESS);
    fContrast = parameters.getParameter(Y2R_CONTRAST);
    fSaturation = parameters.getParameter(Y2R_SATURATION);
    fHue = parameters.getParameter(Y2R_HUE);
    fOffsetU = parameters.getParameter(Y2R_OFFSETU);
    fOffsetV = parameters.getParameter(Y2R_OFFSETV);

    for (i = 0; i < 3; i++)
    {
        aRangeMult[i] = parameters.getParameter(Y2R_RANGEMULT, i);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleY2R::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = ModuleY2R::getGroup();
    }

    parameters.addGroup("ModuleY2R", group);

    switch (t)
    {
    case SAVE_VAL:
        if (eMatrix == BT601)
        {
            parameters.addParameter(Y2R_MATRIX_STD, std::string(BT601_STR));
        }
        else if (eMatrix == BT709)
        {
            parameters.addParameter(Y2R_MATRIX_STD, std::string(BT709_STR));
        }
        else if (eMatrix == JFIF)
        {
            parameters.addParameter(Y2R_MATRIX_STD, std::string(JFIF_STR));
        }

        parameters.addParameter(Y2R_BRIGHTNESS, this->fBrightness);
        parameters.addParameter(Y2R_CONTRAST, this->fContrast);
        parameters.addParameter(Y2R_SATURATION, this->fSaturation);
        parameters.addParameter(Y2R_HUE, this->fHue);
        parameters.addParameter(Y2R_OFFSETU, this->fOffsetU);
        parameters.addParameter(Y2R_OFFSETV, this->fOffsetV);

        values.clear();
        for (i = 0; i < 3; i++)
        {
            values.push_back(toString(this->aRangeMult[i]));
        }
        parameters.addParameter(Y2R_RANGEMULT, values);
        break;

    case SAVE_MIN:
        // matrix std does not have a min
        parameters.addParameterMin(Y2R_MATRIX_STD);
        parameters.addParameterMin(Y2R_BRIGHTNESS);
        parameters.addParameterMin(Y2R_CONTRAST);
        parameters.addParameterMin(Y2R_SATURATION);
        parameters.addParameterMin(Y2R_HUE);
        parameters.addParameterMin(Y2R_OFFSETU);
        parameters.addParameterMin(Y2R_OFFSETV);
        parameters.addParameterMin(Y2R_RANGEMULT);
        break;

    case SAVE_MAX:
        // matrix std does not have a max
        parameters.addParameterMax(Y2R_MATRIX_STD);
        parameters.addParameterMax(Y2R_BRIGHTNESS);
        parameters.addParameterMax(Y2R_CONTRAST);
        parameters.addParameterMax(Y2R_SATURATION);
        parameters.addParameterMax(Y2R_HUE);
        parameters.addParameterMax(Y2R_OFFSETU);
        parameters.addParameterMax(Y2R_OFFSETV);
        parameters.addParameterMax(Y2R_RANGEMULT);
        break;

    case SAVE_DEF:
    {
        std::ostringstream defComment;

        defComment.str("");
        defComment << "{"
            << BT601_STR
            << ", " << BT709_STR
            << ", " << JFIF_STR
            << "}";
        parameters.addParameterDef(Y2R_MATRIX_STD);
        parameters.getParameter(Y2R_MATRIX_STD.name)->setInfo(
                defComment.str());
    }
    parameters.addParameterDef(Y2R_BRIGHTNESS);
    parameters.addParameterDef(Y2R_CONTRAST);
    parameters.addParameterDef(Y2R_SATURATION);
    parameters.addParameterDef(Y2R_HUE);
    parameters.addParameterDef(Y2R_OFFSETU);
    parameters.addParameterDef(Y2R_OFFSETV);
    parameters.addParameterDef(Y2R_RANGEMULT);
    break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleY2R::setup()
{
    LOG_PERF_IN();
    IMG_RESULT ret;
    MC_PIPELINE *pMCPipeline = NULL;
    const ModuleOUT *pOut = NULL;
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
    pOut = pipeline->getModule<const ModuleOUT>();

    bool swapOut = false;
    // swap input of Y2R is YUV instead of YVU
    bool swapYUV = (YUV_420_PL12_8 == pOut->encoderType
        || YUV_422_PL12_8 == pOut->encoderType
        || YUV_420_PL12_10 == pOut->encoderType
        || YUV_422_PL12_10 == pOut->encoderType);

    // support for packed YUV444 - Y2R in passthrough mode
    if(PXL_ISP_444IL3YCrCb8 == pOut->displayType
        || PXL_ISP_444IL3YCbCr8 == pOut->displayType
        || PXL_ISP_444IL3YCrCb10 == pOut->displayType
        || PXL_ISP_444IL3YCbCr10 == pOut->displayType)
    {
        pMCPipeline->sY2R.eType = YCC_TO_YCC;

        // swap only if YUV input different than YUV output
        swapOut = !swapYUV ^ (PXL_ISP_444IL3YCbCr8 == pOut->displayType
                           || PXL_ISP_444IL3YCbCr10 == pOut->displayType);
    } else {
        pMCPipeline->sY2R.eType = YCC_TO_RGB;
        // we have to swap RGB output if BGR instead of RGB
        swapOut = (BGR_888_24 == pOut->displayType
            || BGR_888_32 == pOut->displayType
            || BGR_101010_32 == pOut->displayType);
    }
    ret = configure(pMCPipeline->sY2R, swapYUV, swapOut);

    if (IMG_SUCCESS == ret)
    {
        this->setupFlag = true;
    }
    LOG_PERF_OUT();
    return ret;
}
